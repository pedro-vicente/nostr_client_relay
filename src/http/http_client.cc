// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include <fstream>
#include <sstream>
#include "asio.hpp"
#include "log.hh"
#include "http.hh"
#include "nostr.hh"

using asio::ip::tcp;
std::string log_program_name("http_client");
int send(const std::string& host, const std::string& json);
std::string hex_string(int len, int to_lower = 0);
std::string make_event();
std::string make_request();

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
//nostr protocol with HTTP transport; data (signatures) are simulated 
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
  comm::start_log();

  //wait for server (test locally)
#if defined _MSC_VER
  std::this_thread::sleep_for(std::chrono::seconds(1));
#endif

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //send an event
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  json = make_event();
  if (send(url_relay, json) < 0)
  {
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //send a request
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  json = make_request();
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
  const std::string port_relay("2000"); //listens on 2000
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
    comm::log(s.str());

    //receive response
    http_msg_t http;
    if (http::parse(sock, http) < 0)
    {
      comm::log("HTTP parse failed");
    }
    else
    {
      for (size_t idx = 0; idx < http.header.size(); idx++)
      {
        comm::log("header: " + http.header.at(idx));
      }
      comm::log("message: " + http.msg);
      std::string line = http.header.at(0);
      if (line.find("HTTP/1.1 200 OK\r") != std::string::npos)
      {
      }

      std::string json = http.msg;
      if (json.size())
      {
        nlohmann::json js_events = nlohmann::json::parse(json);

        //JSON contains an array of events
        std::vector<nostr::event_t> events = js_events;

        std::stringstream ss;
        ss << "received " << events.size() << " events";
        comm::log(ss.str());
        for (size_t idx = 0; idx < events.size(); idx++)
        {
          comm::log(events.at(idx).content);
        }
      }

      else
      {
        comm::log("received empty message");  //from an event sent
      }
    }

    //close after receiving acknowledgement
    sock.shutdown(asio::ip::tcp::socket::shutdown_both);
    sock.close();
  }
  catch (std::exception& e)
  {
    comm::log(std::string(e.what()));
  }

  return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//hex_string
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string hex_string(int len, int to_lower)
{
  std::string hex;
  char hex_characters[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
  for (int idx = 0; idx < len; idx++)
  {
    hex.append(1, hex_characters[rand() % 16]);
  }
  if (to_lower)
  {
    transform(hex.begin(), hex.end(), hex.begin(), ::tolower);
  }
  return hex;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//make_event
//["EVENT", <event JSON as defined above>], used to publish events.
//The only object type that exists is the event, which has the following format on the wire:
//"id": <32-bytes lowercase hex - encoded sha256 of the serialized event data>
//"pubkey" : <32-bytes lowercase hex - encoded public key of the event creator>,
//"created_at" : <unix timestamp in seconds>,
//"kind" : <integer>,
//"tags" : [
//["e", <32-bytes hex of the id of another event>, <recommended relay URL>],
//["p", <32-bytes hex of a pubkey>, <recommended relay URL>],
//],
//"content" : <arbitrary string>,
//"sig" : <64-bytes hex of the signature of the sha256 hash of the serialized event data, which is the same as the "id" field>
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string make_event()
{
  const std::string subscription_id("subscription_1");
  const std::string pubkey("b2b41e936821bb48572c2295ec5f1741");
  std::string id = hex_string(32, 1);
  std::string json;
  srand(time(0));
  std::vector<std::vector<std::string>> tags;

  //"tags"
  //["e", <32-bytes hex of the id of another event>, <recommended relay URL>],
  std::vector<std::string> e;
  e.push_back("e");
  std::stringstream es;
  es << id;
  e.push_back(es.str());

  //["p", <32-bytes hex of a pubkey>, <recommended relay URL>],
  std::vector<std::string> p;
  p.push_back("p");
  std::stringstream ep;
  ep << pubkey;
  p.push_back(ep.str());

  tags.push_back(e);
  tags.push_back(p);

  nostr::event_t ev;

  //"id": <32-bytes lowercase hex - encoded sha256 of the serialized event data>
  ev.id = id;

  //"pubkey" : <32-bytes lowercase hex - encoded public key of the event creator>,
  ev.pubkey = pubkey;

  //"created_at" : <unix timestamp in seconds>,
  ev.created_at = std::time(0);

  //"kind" : <integer>,
  ev.kind = 1;

  ev.tags = tags;
  ev.content = "some great content";
  ev.sig = hex_string(64);

  nlohmann::json js_ev;
  to_json(js_ev, ev);
  nlohmann::json js_EVENT = nlohmann::json::array({ "EVENT", js_ev });
  json = js_EVENT.dump(2);

  std::ofstream ofs;
  ofs.open("event.json", std::ofstream::out);
  ofs << json;
  ofs.close();

  return json;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//make_request
//["REQ", <subscription_id>, <filters JSON>...], used to request events and subscribe to new updates.
//<subscription_id> is an arbitrary, non-empty string of max length 64 chars, that should be used to represent a subscription.
//<filters> is a JSON object that determines what events will be sent in that subscription, it can have the following attributes:
//"ids": <a list of event ids or prefixes>,
//"authors" : <a list of pubkeys or prefixes, the pubkey of an event must be one of these>,
//"kinds" : <a list of a kind numbers>,
//"#e" : <a list of event ids that are referenced in an "e" tag>,
//"#p" : <a list of pubkeys that are referenced in a "p" tag>,
//"since" : <an integer unix timestamp, events must be newer than this to pass>,
//"until" : <an integer unix timestamp, events must be older than this to pass>,
//"limit" : <maximum number of events to be returned in the initial query>
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string make_request()
{
  const std::string subscription_id("subscription_1");
  const std::string pubkey("b2b41e936821bb48572c2295ec5f1741");
  std::string id = hex_string(32, 1);
  nostr::filter_t filter;

  //"ids": <a list of event ids or prefixes>,
  std::vector<std::string> ids;
  ids.push_back(id); //generated in a event creation
  filter.ids = ids;

  //"authors" : <a list of pubkeys or prefixes, the pubkey of an event must be one of these>,
  std::vector<std::string> authors;
  authors.push_back(pubkey);
  filter.authors = authors;

  //"kinds" : <a list of a kind numbers>,
  std::vector<int> kinds;
  kinds.push_back(nostr::kind_1);
  filter.kinds = kinds;

  //"#e" : <a list of event ids that are referenced in an "e" tag>,
  std::vector<std::string> _e;
  _e.push_back(id);
  filter._e = _e;

  //"#p" : <a list of pubkeys that are referenced in a "p" tag>,
  std::vector<std::string> _p;
  _p.push_back(pubkey);
  filter._p = _p;

  //"since" : <an integer unix timestamp, events must be newer than this to pass>,
  std::time_t since = 1643873040;
  filter.since = since;

  //"until" : <an integer unix timestamp, events must be older than this to pass>,
  filter.until = std::time(0);

  //"limit" : <maximum number of events to be returned in the initial query>
  filter.limit = 100;

  //<subscription_id> is an arbitrary, non - empty string of max length 64 chars, that should be used to represent a subscription.
  nlohmann::json js_subscription_id = subscription_id;

  nlohmann::json js_fil;
  to_json(js_fil, filter);
  nlohmann::json js_req = nlohmann::json::array({ "REQ", js_subscription_id, js_fil });
  std::string json = js_req.dump(2);

  std::ofstream ofs;
  ofs.open("request.json", std::ofstream::out);
  ofs << json;
  ofs.close();

  return json;
}
