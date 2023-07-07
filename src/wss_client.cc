#include "client_wss.hpp"
#include <future>
#include <fstream> 
#include <strstream>
#include "nostri.h"
#include "log.hh"
#include "nostr.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("nostro");
std::vector<std::string> store;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Nostro
// A Nostr client
/////////////////////////////////////////////////////////////////////////////////////////////////////

void usage()
{
  std::cout << "./nostro [OPTIONS]" << std::endl;
  std::cout << " [OPTIONS]:" << std::endl;
  std::cout << "  --uri <wss URI>        Wss URI to send" << std::endl;
  std::cout << "  --content <string>     the content of the note" << std::endl;
  std::cout << "  --kind <number>        set kind" << std::endl;
  std::cout << "  --sec <hex seckey>     set the secret key for signing, otherwise one will be randomly generated" << std::endl;
  exit(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
// --uri relay.damus.io --content hello
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
  std::string relay_vostro = "localhost:8080";
  std::string uri;
  std::string content;
  std::optional<std::string> seckey;
  int kind = 1;

  comm::start_log();

  for (int i = 1; i < argc; ++i) 
  {
    std::string arg = argv[i];
    if (arg == "--help") 
    {
      usage();
      return 0;
    }
    else if (arg == "--uri") 
    {
      uri = argv[++i];
    }
    else if (arg == "--content")
    {
      content = argv[++i];
    }
    else if (arg == "--seckey")
    {
      seckey = argv[++i];
    }
    else if (arg == "--kind")
    {
      kind = std::stoi(argv[++i]);
    }
  }

  if (!uri.size())
  {
    uri = relay_vostro;
  }

  comm::log(uri);
  comm::log(content);
  if (seckey.has_value()) comm::log(seckey.value());
  comm::log(std::to_string(kind));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // generate the JSON message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  nostr::event_t ev;
  ev.content = content;
  ev.kind = kind;
  std::string json = nostr::make_event(ev, seckey);
  comm::json_to_file("nostro.json", json);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // relay to
  /////////////////////////////////////////////////////////////////////////////////////////////////////

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

}
