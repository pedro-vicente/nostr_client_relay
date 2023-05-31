#include "client_wss.hpp"
#include <future>
#include <fstream> 
#include <strstream>
#include "nostri.h"
#include "log.hh"
#include "message.hh"
#include "database.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("nostro");
std::vector<std::string> store;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// usage examples
// nostril --uri relay.damus.io --content hello
// nostril --content hello --sec 7f612229528369d91ddcaae527f097ab4c7cacd0058fa46d5857f74f88ad1a5e 
// nostril --content hello --mine-pubkey --pow 1 
// 
// relays 
// wss://relay.snort.social
// wss://relay.damus.io
// wss://eden.nostr.land
// wss://nostr-pub.wellorder.net
// wss://nos.lol
/////////////////////////////////////////////////////////////////////////////////////////////////////


void usage()
{
  printf("usage: nostro [OPTIONS]\n");
  printf("\n");
  printf("  OPTIONS\n");
  printf("\n");
  printf("      --uri <string>                  URI to send (e.g 'relay.damus.io', default 'localhost:8080/nostr' for vostro listening)\n");
  printf("      --content <string>              the content of the note\n");
  printf("      --dm <hex pubkey>               make an encrypted dm to said pubkey. sets kind and tags.\n");
  printf("      --kind <number>                 set kind\n");
  printf("      --created-at <unix timestamp>   set a specific created-at time\n");
  printf("      --sec <hex seckey>              set the secret key for signing, otherwise one will be randomly generated\n");
  printf("      --pow <difficulty>              number of leading 0 bits of the id to mine\n");
  printf("      --mine-pubkey                   mine a pubkey instead of id\n");
  printf("      --tag <key> <value>             add a tag\n");
  printf("      -e <event_id>                   shorthand for --tag e <event_id>\n");
  printf("      -p <pubkey>                     shorthand for --tag p <pubkey>\n");
  printf("      -t <hashtag>                    shorthand for --tag t <hashtag>\n");
  printf("\n");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
  // default uri
  std::string uri = "localhost:8080/nostr";
  events::start_log();
  usage();

  struct args args = { 0 };
  struct nostr_event ev = { 0 };
  struct key key;
  secp256k1_context* ctx;
  if (!init_secp_context(&ctx))
    return 2;

  args.uri = uri.c_str();

  if (!parse_args(argc, argv, &args, &ev))
  {
    usage();
    return 10;
  }

  uri = args.uri;
  args.flags |= HAS_ENVELOPE;

  if (args.tags)
  {
    ev.explicit_tags = args.tags;
  }

  make_event_from_args(&ev, &args);

  if (args.sec)
  {
    if (!decode_key(ctx, args.sec, &key))
    {
      return 8;
    }
  }
  else
  {
    int* difficulty = NULL;
    if ((args.flags & HAS_DIFFICULTY) && (args.flags & HAS_MINE_PUBKEY))
    {
      difficulty = &args.difficulty;
    }

    if (!generate_key(ctx, &key, difficulty))
    {
      fprintf(stderr, "could not generate key\n");
      return 4;
    }
    fprintf(stderr, "secret_key ");
    print_hex(key.secret, sizeof(key.secret));
    fprintf(stderr, "\n");
  }

  if (args.flags & HAS_ENCRYPT)
  {
    int kind = args.flags & HAS_KIND ? args.kind : 4;
    if (!make_encrypted_dm(ctx, &key, &ev, args.encrypt_to, kind))
    {
      fprintf(stderr, "error making encrypted dm\n");
      return 0;
    }
  }

  // set the event's pubkey
  memcpy(ev.pubkey, key.pubkey, 32);

  if (args.flags & HAS_DIFFICULTY && !(args.flags & HAS_MINE_PUBKEY))
  {
    if (!mine_event(&ev, args.difficulty))
    {
      fprintf(stderr, "error when mining id\n");
      return 22;
    }
  }
  else
  {
    if (!generate_event_id(&ev))
    {
      fprintf(stderr, "could not generate event id\n");
      return 5;
    }
  }

  if (!sign_event(ctx, &key, &ev))
  {
    fprintf(stderr, "could not sign event\n");
    return 6;
  }

  char* json = (char*)malloc(102400);
  if (!print_event(&ev, args.flags & HAS_ENVELOPE, &json))
  {
    fprintf(stderr, "buffer too small\n");
    return 88;
  }

  WssClient client(uri, false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::stringstream ss;
    std::string str = in_message->string();
    ss << "Received: " << "\"" << str << "\"";
    events::log(ss.str());
    store.push_back(str);

    connection->send_close(1000);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_open = [&](std::shared_ptr<WssClient::Connection> connection)
  {
    std::stringstream ss;
    ss << "Opened connection: HTTP " << connection.get()->http_version << " , code " << connection.get()->status_code;
    events::log(ss.str());

    std::string out_message = json;
    ss.str(std::string());
    ss.clear();
    ss << "Sending: \"" << out_message << "\"";
    events::log(ss.str());

    connection->send(out_message);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_close = [](std::shared_ptr<WssClient::Connection>, int status, const std::string&)
  {
    std::stringstream ss;
    ss << "Closed: " << status;
    events::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
    std::stringstream ss;
    ss << "Error: " << ec << " : " << ec.message();
    events::log(ss.str());
  };

  client.start();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // messages received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::stringstream ss;
  ss << "Received " << store.size() << " messages: ";
  events::log(ss.str());

  for (int idx = 0; idx < store.size(); idx++)
  {
    events::log(store.at(idx));
  }
}
