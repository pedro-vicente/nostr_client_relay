// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "asio.hpp"
#include "log.hh"
#include "http.hh"
#include "nostr.hh"
#include "database.hh"

using asio::ip::tcp;
std::string log_program_name("http_relay");

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  const std::string port_relay("2000"); //listens on 2000
  unsigned short port_num = std::atoi(port_relay.c_str());
  comm::start_log();

  //a database is an array of events read and saved to filesystem in JSON format
  std::vector<nostr::event_t> database;

  //start with an empty database 
  database::save(database);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //network
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  asio::io_service io_service;
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port_num));
  acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  comm::log("Listening on port:" + std::to_string(port_num));

  while (1)
  {
    try
    {
      tcp::socket sock(io_service);
      acceptor.accept(sock);
      std::string client_ip = sock.remote_endpoint().address().to_string();

      http_msg_t http;
      if (http::parse(sock, http) < 0)
      {
        comm::log("HTTP parse failed");
      }

      comm::log("received POST from " + client_ip + " " + std::to_string(http.msg.size()) + " bytes");
      nlohmann::json js_message = nlohmann::json::parse(http.msg);
      std::string message_type = js_message.at(0);

      // Clients can send 3 types of messages, which must be JSON arrays, according to the following patterns:
      //["EVENT", <event JSON as defined above>], used to publish events.
      //["REQ", <subscription_id>, <filters JSON>...], used to request events and subscribe to new updates.
      //["CLOSE", <subscription_id>], used to stop previous subscriptions.

      if (message_type.compare("EVENT") == 0)
      {
        nostr::event_t ev;
        from_json(js_message.at(1), ev);
        comm::log("event received: " + ev.content);

        //add event to list
        database.push_back(ev);

        //save to database
        database::append(database);

        //send an empty response
        std::stringstream ss;
        std::string response("HTTP/1.1 200 OK\r\n\r\n");
        asio::write(sock, asio::buffer(response, response.size()));
        ss << "send response: " << client_ip;
        comm::log(ss.str());
      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //NIP-01
      //Upon receiving a REQ message, the relay SHOULD query its internal database and return events that match the filter, 
      //then store that filter and send again all future events it receives to that same websocket until the websocket is closed. 
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      else if (message_type.compare("REQ") == 0)
      {
        std::string subscription_id = js_message.at(1);
        nostr::filter_t filter;
        from_json(js_message.at(2), filter);
        comm::log("filter received: " + subscription_id);

        //query database, return events that match the filter
        std::vector<nostr::event_t> events = database::request(filter);

        nlohmann::json js_events = events;
        std::string json = js_events.dump(2);
        std::string response_http = http::make_post(client_ip, json);

        //send events to client
        asio::write(sock, asio::buffer(response_http, response_http.size()));
        std::stringstream ss;
        ss << "send response: " << client_ip;
        comm::log(ss.str());
      }
    }
    catch (std::exception& e)
    {
      comm::log(std::string(e.what()));
    }
  }

  return 0;
}

