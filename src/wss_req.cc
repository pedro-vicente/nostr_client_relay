// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "client_wss.hpp"
#include <future>
#include <fstream>
#include<thread>
#include <strstream>
#include "nostri.h"
#include "log.hh"
#include "nostr.hh"
#include "uuid.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("nostro");
std::vector<std::string> store;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Nostro
// A Nostr client
// --sec <hex seckey> --content hello --kind 1
// --req --authors 4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b
/////////////////////////////////////////////////////////////////////////////////////////////////////

void usage()
{
  std::cout << "./nostro [OPTIONS]" << std::endl;
  std::cout << "[OPTIONS]:" << std::endl;
  std::cout << "  --uri <wss URI>      Wss URI to send" << std::endl;
  std::cout << "  --req                message is a request (REQ). EVENT parameters are ignored" << std::endl;
  std::cout << "REQ OPTIONS: These are for the REQ filter, per NIP-01" << std::endl;
  std::cout << "  --authors <string>   a list of pubkeys or prefixes" << std::endl;
  std::cout << "EVENT OPTIONS: These are to publish an EVENT, per NIP-01" << std::endl;
  std::cout << "  --content <string>   the content of the note" << std::endl;
  std::cout << "  --kind <number>      set kind" << std::endl;
  std::cout << "  --sec <hex seckey>   set the secret key for signing, otherwise one will be randomly generated" << std::endl;
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
  std::string authors;
  std::optional<std::string> seckey;
  int kind = 1;
  int is_req = 0;
  std::string json;

  comm::start_log();

  for (int i = 1; i < argc; ++i)
  {
    std::string arg = argv[i];
    if (arg == "--help")
    {
      usage();
      return 0;
    }
    else if (arg == "--req")
    {
      is_req = 1;
    }
    else if (arg == "--uri")
    {
      uri = argv[++i];
    }
    else if (arg == "--authors")
    {
      authors = argv[++i];
    }
    else if (arg == "--content")
    {
      content = argv[++i];
    }
    else if (arg == "--sec")
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
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  comm::log(uri);
  comm::log(content);
  if (seckey.has_value()) comm::log(seckey.value());
  comm::log(std::to_string(kind));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // generate the JSON message (REQ or EVENT)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (is_req)
  {
    std::string subscription_id = uuid::generate_uuid_v4();
    nostr::filter_t filter;
    if (authors.size())
    {
      filter.authors.push_back(authors);
      filter.kinds.push_back(nostr::kind_1);
    }
    json = nostr::make_request(subscription_id, filter);
    comm::json_to_file("nostro_request.json", json);
  }
  else
  {
    nostr::event_t ev;
    ev.content = content;
    ev.kind = kind;
    json = nostr::make_event(ev, seckey);
    comm::json_to_file("nostro_event.json", json);
  }

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
