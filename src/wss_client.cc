#include "client_wss.hpp"
#include <future>
#include <fstream> 
#include <strstream>
#include "nostri.h"
#include "log.hh"
#include "nostr.hh"

#ifdef _MSC_VER
#pragma warning(disable: 6387)
#endif

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("nostro");
std::vector<std::string> store;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// usage examples
// ./nostro --uri relay.damus.io --content hello 
// ./nostro --content hello --sec 7f612229528369d91ddcaae527f097ab4c7cacd0058fa46d5857f74f88ad1a5e 
// ./nostro --content hello --mine-pubkey --pow 1 
// 
// relays 
// wss://relay.snort.social
// wss://relay.damus.io
// wss://eden.nostr.land
// wss://nostr-pub.wellorder.net
// wss://nos.lol
// 
// real event ids for testing REQ
// 92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363 @jack
// ./nostro --uri relay.snort.social --req --id 92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363
// ./nostro --uri relay.damus.io --req --rand
// ./nostro --uri nostr.pleb.network --req --id d75d56b2141b12be96421fc5c913092cda06904208ef798b51a28f1c906bbab7
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
  std::string relay_vostro = "localhost:8080/nostr";
  std::vector<std::string> relay = { "relay.snort.social",
    "relay.damus.io",
    "nostr.pleb.network" };
  char* buf = (char*)malloc(102400);
  struct args args = { 0 };
  struct nostr_event ev = { 0 };

  comm::start_log();

  if (!parse_args(argc, argv, &args, &ev))
  {
    usage();
    return 10;
  }

  if (make_message(&args, &ev, &buf) < 0)
  {
    return 0;
  }

  std::string uri(args.uri);

  //format JSON to display
  nlohmann::json js_message = nlohmann::json::parse(buf);
  std::string json = js_message.dump(1); //indent level
  comm::json_to_file("send_message.json", json);

  WssClient client(uri, false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::stringstream ss;
    std::string str = in_message->string();
    ss << "Received: " << str;
    comm::log(ss.str());
    store.push_back(str);
    comm::json_to_file("response.txt", str);

    connection->send_close(1000);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_open = [&](std::shared_ptr<WssClient::Connection> connection)
  {
    std::stringstream ss;
    ss << "Opened connection: HTTP " << connection.get()->http_version << " , code " << connection.get()->status_code;
    comm::log(ss.str());

    std::string message = json;
    ss.str(std::string());
    ss.clear();
    ss << "Sending: " << message;
    comm::log(ss.str());

    connection->send(message);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_close = [](std::shared_ptr<WssClient::Connection>, int status, const std::string&)
  {
    std::stringstream ss;
    ss << "Closed: " << status;
    comm::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
    std::stringstream ss;
    ss << "Error: " << ec << " : " << ec.message();
    comm::log(ss.str());
  };

  client.start();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // messages received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::stringstream ss;
  ss << "Received " << store.size() << " messages: ";
  comm::log(ss.str());

  for (int idx = 0; idx < store.size(); idx++)
  {
    comm::log(store.at(idx));
  }

  std::stringstream js;
  js << "[";
  for (int idx = 0; idx < store.size(); idx++)
  {
    js << store.at(idx);
    std::string c = (idx < store.size() - 1) ? " ,\n" : "\n";
    js << c;
  }
  js << "]";
  std::string s = js.rdbuf()->str();
  comm::json_to_file("response.json", s);

  free(buf);
}
