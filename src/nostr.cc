#include <random>
#include <sstream>
#include <fstream> 
#include <iostream>
#include "client_wss.hpp"
#include "nlohmann/json.hpp"
#include "log.hh"
#include "nostr.hh"
using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//event_t
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

void nostr::to_json(nlohmann::json& j, const nostr::event_t& s)
{
  j["id"] = s.id;
  j["pubkey"] = s.pubkey;
  j["created_at"] = s.created_at;
  j["kind"] = s.kind;
  j["tags"] = s.tags;
  j["content"] = s.content;
  j["sig"] = s.sig;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//from_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

void nostr::from_json(const nlohmann::json& j, nostr::event_t& s)
{
  j.at("id").get_to(s.id);
  j.at("pubkey").get_to(s.pubkey);
  j.at("created_at").get_to(s.created_at);
  j.at("kind").get_to(s.kind);
  j.at("tags").get_to(s.tags);
  j.at("content").get_to(s.content);
  j.at("sig").get_to(s.sig);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//filter_t
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

void nostr::to_json(nlohmann::json& j, const nostr::filter_t& s, bool with_time)
{
  if (s.ids.size())
  {
    j["ids"] = s.ids;
  }
  if (s.authors.size())
  {
    j["authors"] = s.authors;
  }
  j["kinds"] = s.kinds;
  if (s._e.size())
  {
    j["#e"] = s._e;
  }
  if (s._p.size())
  {
    j["#p"] = s._p;
  }
  if (with_time)
  {
    j["since"] = s.since;
    j["until"] = s.until;
  }
  j["limit"] = s.limit;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//from_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

void nostr::from_json(const nlohmann::json& j, nostr::filter_t& s)
{
  if (j.contains("ids"))
  {
    j.at("ids").get_to(s.ids);
  }
  if (j.contains("authors"))
  {
    j.at("authors").get_to(s.authors);
  }
  if (j.contains("kinds"))
  {
    j.at("kinds").get_to(s.kinds);
  }
  if (j.contains("#e"))
  {
    j.at("#e").get_to(s._e);
  }
  if (j.contains("#p"))
  {
    j.at("#p").get_to(s._p);
  }
  if (j.contains("since"))
  {
    j.at("since").get_to(s.since);
  }
  if (j.contains("until"))
  {
    j.at("until").get_to(s.until);
  }
  if (j.contains("limit"))
  {
    j.at("limit").get_to(s.limit);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//make_request
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string nostr::make_request(const std::string& subscription_id, const nostr::filter_t& filter)
{
  std::string json;
  try
  {
    nlohmann::json js_subscription_id = subscription_id;
    nlohmann::json js_filter;
    to_json(js_filter, filter);
    nlohmann::json js_req = nlohmann::json::array({ "REQ", js_subscription_id, js_filter });
    json = js_req.dump();
  }
  catch (const std::exception& e)
  {
    events::log(e.what());
  }
  return json;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//get_message_type
/////////////////////////////////////////////////////////////////////////////////////////////////////

nostr::Type nostr::get_message_type(const std::string& json)
{
  try
  {
    //all Nostr messages are JSON arrays
    nlohmann::json js_message = nlohmann::json::parse(json);

    //Relays can send 3 types of messages, which must also be JSON arrays, according to the following patterns:
    //["EVENT", <subscription_id>, <event JSON as defined above>], used to send events requested by clients.
    //["EOSE", <subscription_id>], used to indicate the end of stored events and the beginning of events newly received in real - time.
    //["NOTICE", <message>], used to send human - readable error messages or other things to clients.

    std::string type = js_message.at(0);
    if (type.compare("EVENT") == 0)
    {
      return nostr::Type::EVENT;
    }
    else if (type.compare("REQ") == 0)
    {
      return nostr::Type::REQ;
    }
    else if (type.compare("CLOSE") == 0)
    {
      return nostr::Type::CLOSE;
    }
    else if (type.compare("EOSE") == 0)
    {
      return nostr::Type::EOSE;
    }
    else if (type.compare("NOTICE") == 0)
    {
      return nostr::Type::NOTICE;
    }
  }
  catch (const std::exception& e)
  {
    events::log(e.what());
    return nostr::Type::UNKNOWN;
  }
  return nostr::Type::UNKNOWN;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//parse_event
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::parse_event(const std::string& json, std::string& subscription_id, nostr::event_t& ev)
{
  try
  {
    //all Nostr messages are JSON arrays
    nlohmann::json js_message = nlohmann::json::parse(json);

    std::string type = js_message.at(0);
    if (type.compare("EVENT") == 0)
    {
      subscription_id = js_message.at(1);
      from_json(js_message.at(2), ev);
      return 0;
    }
  }
  catch (const std::exception& e)
  {
    events::log(e.what());
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//parse_request
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::parse_request(const std::string& json, std::string& request_id, nostr::filter_t& filter)
{
  try
  {
    //all Nostr messages are JSON arrays
    nlohmann::json js_message = nlohmann::json::parse(json);

    std::string type = js_message.at(0);
    if (type.compare("REQ") == 0)
    {
      request_id = js_message.at(1);
      from_json(js_message.at(2), filter);
      return 0;
    }
  }
  catch (const std::exception& e)
  {
    events::log(e.what());
  }
  return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// relay_to
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::relay_to(const std::string& uri, const std::string& json)
{
  WssClient client(uri, false);
  size_t count = 0;
  std::ofstream log;
  log.open("messages.txt", std::ofstream::trunc);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [&](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::string str = in_message->string();
    log << "Received: " << str << std::endl;

    std::stringstream ss;
    count++;
    ss << "message_" << std::to_string(count) << ".json";
    events::json_to_file(ss.str(), str);

  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_open = [&](std::shared_ptr<WssClient::Connection> connection)
  {
    std::string out_message = json;
    log << "Sending: " << out_message << std::endl;

    connection->send(out_message);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_close = [&](std::shared_ptr<WssClient::Connection>, int status, const std::string&)
  {
    log << "Closed: " << status << std::endl;
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [&](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
    log << "Error: " << ec << " : " << ec.message() << std::endl;
  };

  client.start();
  return 0;
}

