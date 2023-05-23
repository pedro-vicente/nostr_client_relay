#include "asio.hpp"
#include "log.hh"
#include "http.hh"
#include "message.hh"

using asio::ip::tcp;
std::string log_program_name("http_client");
int send(const std::string& host, const std::string& json);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
//Clients can send 3 types of messages, which must be JSON arrays, according to the following patterns:
//["EVENT", <event JSON as defined above>], used to publish events.
//["REQ", <subscription_id>, <filters JSON>...], used to request events and subscribe to new updates.
//["CLOSE", <subscription_id>], used to stop previous subscriptions.
//<subscription_id> is an arbitrary, non-empty string of max length 64 chars, that should be used to represent a subscription.
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  std::string json;
  std::string url_relay("127.0.0.1");
  events::start_log();

  //wait for server (test locally)
  std::this_thread::sleep_for(std::chrono::seconds(1));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //send an event
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  json = message::make_event(url_relay);
  if (send(url_relay, json) < 0)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //send a request
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  json = message::make_request();
  if (send(url_relay, json) < 0)
  {
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//send
/////////////////////////////////////////////////////////////////////////////////////////////////////

int send(const std::string& host, const std::string& json)
{
  std::string buf_http = http::make_post(host, json);

  try
  {
    asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::socket sock(io_service);
    asio::connect(sock, resolver.resolve(host, port_relay));
    asio::write(sock, asio::buffer(buf_http, buf_http.size()));
    std::stringstream s;
    s << "sent " << buf_http.size() << " bytes";
    events::log(s.str());

    //receive response
    http_msg_t http;
    if (http::parse(sock, http) < 0)
    {
      events::log("HTTP parse failed");
    }
    else
    {
      for (size_t idx = 0; idx < http.header.size(); idx++)
      {
        events::log("header: " + http.header.at(idx));
      }
      events::log("message: " + http.msg);
      std::string line = http.header.at(0);
      if (line.find("HTTP/1.1 200 OK\r") != std::string::npos)
      {
      }

      std::string json = http.msg;
      if (json.size())
      {
        nlohmann::json js_events = nlohmann::json::parse(json);

        //JSON contains an array of events
        std::vector<event_t> events = js_events;

        std::stringstream ss;
        ss << "received " << events.size() << " events";
        events::log(ss.str());
        for (size_t idx = 0; idx < events.size(); idx++)
        {
          events::log(events.at(idx).content);
        }
      }

      else
      {
        events::log("received empty message");  //from an event sent
      }
    }

    //close after receiving acknowledgement
    sock.shutdown(asio::ip::tcp::socket::shutdown_both);
    sock.close();
  }
  catch (std::exception& e)
  {
    events::log(std::string(e.what()));
  }

  return 0;
}

