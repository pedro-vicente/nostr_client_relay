#include "asio.hpp"
#include "log.hh"
#include "http.hh"
#include "message.hh"
#include "database.hh"

using asio::ip::tcp;
std::string log_program_name("relay");

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  unsigned short port_num = std::atoi(port_relay.c_str());
  events::start_log();

  //a database is an array of events read and saved to filesystem in JSON format
  std::vector<event_t> database;

  //start with an empty database 
  database::save(database);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //network
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  asio::io_service io_service;
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port_num));
  acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  events::log("Listening on port:" + std::to_string(port_num));

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
        events::log("HTTP parse failed");
      }

      events::log("received POST from " + client_ip + " " + std::to_string(http.msg.size()) + " bytes");
      nlohmann::json js_message = nlohmann::json::parse(http.msg);
      std::string message_type = js_message.at(0);

      // Clients can send 3 types of messages, which must be JSON arrays, according to the following patterns:
      //["EVENT", <event JSON as defined above>], used to publish events.
      //["REQ", <subscription_id>, <filters JSON>...], used to request events and subscribe to new updates.
      //["CLOSE", <subscription_id>], used to stop previous subscriptions.

      if (message_type.compare("EVENT") == 0)
      {
        event_t ev;
        from_json(js_message.at(1), ev);
        events::log("event received: " + ev.content);

        //add event to list
        database.push_back(ev);

        //save to database
        database::append(database);

        //send an empty response
        std::stringstream ss;
        std::string response("HTTP/1.1 200 OK\r\n\r\n");
        asio::write(sock, asio::buffer(response, response.size()));
        ss << "send response: " << client_ip;
        events::log(ss.str());
      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //NIP-01
      //Upon receiving a REQ message, the relay SHOULD query its internal database and return events that match the filter, 
      //then store that filter and send again all future events it receives to that same websocket until the websocket is closed. 
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      else if (message_type.compare("REQ") == 0)
      {
        std::string subscription_id = js_message.at(1);
        filter_t filter;
        from_json(js_message.at(2), filter);
        events::log("filter received: " + subscription_id);

        //query database, return events that match the filter
        std::vector<event_t> events = database::request(filter);

        nlohmann::json js_events = events;
        std::string json = js_events.dump(2);
        std::string response_http = http::make_post(client_ip, json);

        //send events to client
        asio::write(sock, asio::buffer(response_http, response_http.size()));
        std::stringstream ss;
        ss << "send response: " << client_ip;
        events::log(ss.str());
      }
    }
    catch (std::exception& e)
    {
      events::log(std::string(e.what()));
    }
  }

  return 0;
}

