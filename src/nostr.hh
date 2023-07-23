// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#ifndef NOSTR_HH
#define NOSTR_HH

#include <string>
#include <vector>
#include <optional>
#include "nlohmann/json.hpp"

namespace nostr
{

  enum 
  {
    kind_0 = 0, // Metadata 
    kind_1 = 1, // Short Text Note
    kind_3 = 3, // Contacts
  };


  //event
  const std::string ids("ids");
  const std::string authors("authors");
  const std::string kinds("kinds");

  //filter
  const std::string id("id");
  const std::string pubkey("pubkey");
  const std::string created_at("created_at");
  const std::string kind("kind");
  const std::string tags("tags");
  const std::string content("content");
  const std::string sig("sig");

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

  class event_t
  {
  public:
    event_t() : kind(1), created_at(std::time(0))
    {
    }
    std::string id;
    std::string pubkey;
    std::time_t created_at;
    int kind;
    std::vector<std::vector<std::string>> tags;
    std::string content;
    std::string sig;
  };

  void to_json(nlohmann::json& j, const event_t& s);
  void from_json(const nlohmann::json& j, event_t& s);

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

  class filter_t
  {
  public:
    filter_t() : limit(1), since(0), until(std::time(0))
    {
    };
    std::vector<std::string> ids;
    std::vector<std::string> authors;
    std::vector<int> kinds;
    std::vector<std::string> _e;
    std::vector<std::string> _p;
    std::time_t since;
    std::time_t until;
    size_t limit;
  };

  void to_json(nlohmann::json& j, const filter_t& s, bool with_time = false);
  void from_json(const nlohmann::json& j, filter_t& s);

  //Clients can send 3 types of messages, which must be JSON arrays, according to the following patterns:
  //["EVENT", <event JSON as defined above>], used to publish events.
  //["REQ", <subscription_id>, <filters JSON>...], used to request events and subscribe to new updates.
  //["CLOSE", <subscription_id>], used to stop previous subscriptions.

  //Relays can send 3 types of messages, which must also be JSON arrays, according to the following patterns:
  //["EVENT", <subscription_id>, <event JSON as defined above>], used to send events requested by clients.
  //["EOSE", <subscription_id>], used to indicate the end of stored events and the beginning of events newly received in real - time.
  //["NOTICE", <message>], used to send human - readable error messages or other things to clients.

  enum class Type
  {
    EVENT,
    REQ,
    CLOSE,
    EOSE,
    NOTICE,
    UNKNOWN
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // public API
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Type get_message_type(const std::string& json);
  std::string make_request(const std::string& subscription_id, const nostr::filter_t& filter);
  std::string make_event(nostr::event_t& ev, const std::optional<std::string>& seckey);
  int parse_client_event(const std::string& json, nostr::event_t& ev);
  int parse_relay_event(const std::string& json, std::string& event_id, nostr::event_t& ev);
  int parse_request(const std::string& json, std::string& request_id, nostr::filter_t& filter);
  int relay_to(const std::string& uri, const std::string& json, std::vector<std::string>& store);
  int get_follows(const std::string& uri, const std::string& pubkey, std::vector<std::string>& pubkeys);
  int get_feed(const std::string& uri, const std::string& pubkey, std::vector<std::string>& response);
  int get_metadata(const std::string& uri, const std::string& pubkey, std::vector<std::string>& response);

  void get_events(const std::string& pubkey, const std::string& uri, std::vector<std::string>& response);
}


#endif

