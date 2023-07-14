// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "server_wss.hpp"
#include <future>

#include "log.hh"
#include "nostr.hh"
#include "database.hh"
#include "relay.hh"

using WssServer = SimpleWeb::SocketServer<SimpleWeb::WSS>;
std::string log_program_name("vostro");

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  comm::start_log();

  relay_t relay;

  WssServer server("server.crt", "server.key");
  server.config.port = 8080;
  auto& endpoint = server.endpoint["/"];

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  endpoint.on_message = [&](std::shared_ptr<WssServer::Connection> connection, std::shared_ptr<WssServer::InMessage> in_message)
  {
    std::stringstream ss;
    std::string msg = in_message->string();
    ss << "Server: Message received: \"" << msg << "\" from " << connection.get();
    comm::log(ss.str());

    std::string resp = relay.make_response(msg);

    connection->send(resp, [](const SimpleWeb::error_code& ec)
      {
        if (ec)
        {
          std::stringstream s;
          s << "Error sending message: " << ec << " : " << ec.message();
          comm::log(s.str());
        }
      });
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  endpoint.on_open = [](std::shared_ptr<WssServer::Connection> connection)
  {
    std::stringstream ss;
    ss << "Opened connection: " << connection.get();
    comm::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  endpoint.on_close = [](std::shared_ptr<WssServer::Connection> connection, int status, const std::string&)
  {
    std::stringstream ss;
    ss << "Closed connection: " << connection.get() << " code " << status;
    comm::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_handshake
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  endpoint.on_handshake = [](std::shared_ptr<WssServer::Connection>, SimpleWeb::CaseInsensitiveMultimap&)
  {
    return SimpleWeb::StatusCode::information_switching_protocols;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  endpoint.on_error = [](std::shared_ptr<WssServer::Connection> connection, const SimpleWeb::error_code& ec)
  {
    std::stringstream ss;
    ss << "Error: " << connection.get() << ". " << ec << " : " << ec.message() << std::endl;
    comm::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // start
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::promise<unsigned short> server_port;
  std::thread server_thread([&server, &server_port]()
    {
      server.start([&server_port](unsigned short port)
        {
          server_port.set_value(port);
        });
    });

  std::stringstream ss;
  ss << "Listening on port: " << server_port.get_future().get();
  comm::log(ss.str());
  server_thread.join();
}
