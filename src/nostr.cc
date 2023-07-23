// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include <random>
#include <sstream>
#include <fstream> 
#include <iostream>
#include "client_wss.hpp"
#include "nlohmann/json.hpp"

#include "nostri.h"
#include "hex.h"
#include "log.hh"
#include "nostr.hh"
#include "uuid.hh"

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
    comm::log(e.what());
    return nostr::Type::UNKNOWN;
  }
  return nostr::Type::UNKNOWN;
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
    comm::log(e.what());
  }
  return json;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_client_event
// note: relay events do not have a subscription string at array index 1)
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::parse_client_event(const std::string& json, nostr::event_t& ev)
{
  try
  {
    //all Nostr messages are JSON arrays
    nlohmann::json js_message = nlohmann::json::parse(json);

    std::string type = js_message.at(0);
    if (type.compare("EVENT") == 0)
    {
      from_json(js_message.at(1), ev);
      return 0;
    }
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// parse_relay_event
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::parse_relay_event(const std::string& json, std::string& subscription_id, nostr::event_t& ev)
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
    comm::log(e.what());
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
    comm::log(e.what());
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// relay_to
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::relay_to(const std::string& uri, const std::string& json, std::vector<std::string>& store)
{
  WssClient client(uri, false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [&](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::string str = in_message->string();
    store.push_back(str);
    connection->send_close(1000);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_open = [&](std::shared_ptr<WssClient::Connection> connection)
  {
    connection->send(json);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_close = [](std::shared_ptr<WssClient::Connection>, int status, const std::string&)
  {
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
  };

  client.start();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// make_event
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string nostr::make_event(nostr::event_t& ev, const std::optional<std::string>& seckey)
{
  std::string json;
  struct key key;
  secp256k1_context* ctx;

  if (!init_secp_context(&ctx))
  {
    return json;
  }

  if (seckey.has_value())
  {
    std::string sec = seckey.value();
    if (!decode_key(ctx, sec.c_str(), &key))
    {
      return json;
    }
  }
  else
  {
    int* difficulty = NULL;
    if (!generate_key(ctx, &key, difficulty))
    {
      comm::log("could not generate key");
      return json;
    }
    fprintf(stderr, "secret_key ");
    print_hex(key.secret, sizeof(key.secret));
    fprintf(stderr, "\n");
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // generate event ID, signature, pubkey
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  nostr_event nev = { 0 };
  char pubkey[65];
  char id[65];
  char sig[129];
  nev.created_at = ev.created_at;
  nev.content = ev.content.c_str();
  nev.kind = ev.kind;
  memcpy(nev.pubkey, key.pubkey, 32);

  if (!generate_event_id(&nev))
  {
    comm::log("could not generate event id");
    return json;
  }

  if (!sign_event(ctx, &key, &nev))
  {
    comm::log("could not sign event");
    return json;
  }

  if (hex_encode(nev.id, sizeof(nev.id), id, sizeof(id)) < 0)
  {
    comm::log("could not encode event id");
    return json;
  }
  if (hex_encode(nev.pubkey, sizeof(nev.pubkey), pubkey, sizeof(pubkey)) < 0)
  {
    comm::log("could not encode event pubkey");
    return json;
  }
  if (hex_encode(nev.sig, sizeof(nev.sig), sig, sizeof(sig)) < 0)
  {
    comm::log("could not encode event signature");
    return json;
  }

  ev.pubkey = std::string(pubkey);
  ev.id = std::string(id);
  ev.sig = std::string(sig);
  nlohmann::json js;
  to_json(js, ev);
  //envelop the JSON event object 
  nlohmann::json js_ev = nlohmann::json::array({ "EVENT", js });
  json = js_ev.dump();
  return json;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// get_follows returns a list of pubkeys
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::get_follows(const std::string& uri, const std::string& pubkey, std::vector<std::string>& pubkeys)
{
  std::vector<std::string> responses;
  std::vector< nostr::event_t> subs_events;

  std::string subscription_id = uuid::generate_uuid_v4();
  nostr::filter_t filter;
  filter.authors.push_back(pubkey);
  filter.kinds.push_back(nostr::kind_3);
  filter.limit = 1;
  std::string json = nostr::make_request(subscription_id, filter);
  comm::json_to_file("follows_request.json", json);
  if (nostr::relay_to(uri, json, responses) < 0)
  {
  }

  for (int idx = 0; idx < responses.size(); idx++)
  {
    std::string message = responses.at(idx);

    try
    {
      nlohmann::json js = nlohmann::json::parse(message);
      std::string type = js.at(0);
      if (type.compare("EVENT") == 0)
      {
        nostr::event_t ev;
        from_json(js.at(2), ev);
        subs_events.push_back(ev);
        comm::json_to_file("follows_event.json", message);
      }
    }
    catch (const std::exception& e)
    {
      comm::log(e.what());
    }
  }

  if (!subs_events.size())
  {
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get follows info (1 event only should be returned)
  // A special event with kind 3, meaning "contact list" is defined as having a list of p tags, 
  // one for each of the followed/known profiles one is following.
  // Each tag entry should contain the key for the profile, a relay URL where events from that key can be found
  // (can be set to an empty string if not needed), and a local name(or "petname") 
  // for that profile(can also be set to an empty string or not provided), 
  // i.e., ["p", <32 - bytes hex key>, <main relay URL>, <petname>].
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  nostr::event_t ev = subs_events.at(0);
  for (int idx = 0; idx < ev.tags.size(); idx++)
  {
    std::vector<std::string> tag = ev.tags.at(idx);
    if (tag.size() > 1 && tag.at(0).compare("p") == 0)
    {
      std::string pubkey = tag.at(1);
      pubkeys.push_back(pubkey);
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// get_feed
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::get_feed(const std::string& uri, const std::string& pubkey, std::vector<std::string>& response)
{
  std::string subscription_id = uuid::generate_uuid_v4();
  nostr::filter_t filter;
  filter.authors.push_back(pubkey);
  filter.kinds.push_back(nostr::kind_1);
  filter.limit = 2;
  std::string json = nostr::make_request(subscription_id, filter);
  comm::json_to_file("req_feed.json", json);

  std::vector<std::string> messages;
  if (nostr::relay_to(uri, json, messages) < 0)
  {
  }

  for (int idx = 0; idx < messages.size(); idx++)
  {
    std::string message = messages.at(idx);
    try
    {
      nlohmann::json js = nlohmann::json::parse(message);
      std::string type = js.at(0);
      if (type.compare("EVENT") == 0)
      {
        nostr::event_t ev;
        from_json(js.at(2), ev);
        response.push_back(message);
        comm::json_to_file("follows_event.json", message);
      }
    }
    catch (const std::exception& e)
    {
      comm::log(e.what());
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// get_metadata
/////////////////////////////////////////////////////////////////////////////////////////////////////

int nostr::get_metadata(const std::string& uri, const std::string& pubkey, std::vector<std::string>& response)
{
  std::string subscription_id = uuid::generate_uuid_v4();
  nostr::filter_t filter;
  filter.authors.push_back(pubkey);
  filter.kinds.push_back(nostr::kind_0);
  std::string json = nostr::make_request(subscription_id, filter);
  comm::json_to_file("req_metadata.json", json);

  std::vector<std::string> messages;
  if (nostr::relay_to(uri, json, messages) < 0)
  {
  }

  for (int idx = 0; idx < messages.size(); idx++)
  {
    std::string message = messages.at(idx);
    response.push_back(message);
  }

  return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//get_events
/////////////////////////////////////////////////////////////////////////////////////////////////////

void nostr::get_events(const std::string& pubkey, const std::string& uri, std::vector<std::string>& response)
{

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get_follows returns an array of pubkeys 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<std::string> pubkeys;
  nostr::get_follows(uri, pubkey, pubkeys);
  comm::to_file("pubkeys.txt", pubkeys);

  for (int idx_key = 0; idx_key < pubkeys.size(); idx_key++)
  {
    std::string pubkey = pubkeys.at(idx_key);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // get feed returns an array of JSON events 
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> events;
    if (nostr::get_feed(uri, pubkey, events) < 0)
    {
    }

    int row = 0;
    for (int idx_eve = 0; idx_eve < events.size(); idx_eve++)
    {
      std::string message = events.at(idx_eve);
      try
      {
        nlohmann::json js = nlohmann::json::parse(message);
        std::string type = js.at(0);
        if (type.compare("EVENT") == 0)
        {
          nostr::event_t ev;
          from_json(js.at(2), ev);
          std::string json = js.dump(1);

          response.push_back(json);

          std::stringstream s;
          s << "follow." << row + 1 << ".json";
          comm::json_to_file(s.str(), json);
          row++;
        }
      }
      catch (const std::exception& e)
      {
        comm::log(e.what());
      }
    } //events
  } //pubkeys

}