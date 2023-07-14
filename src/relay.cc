// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include <random>
#include <sstream>
#include <fstream> 
#include <iostream>
#include "nlohmann/json.hpp"
#include "nostri.h"
#include "hex.h"

#include "relay.hh"
#include "log.hh"
#include "nostr.hh"
#include "uuid.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// relay_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

relay_t::relay_t()
{
  // read current database
  database = database::read();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// make_response
// Logic to send a JSON response to client
// Clients can send 3 types of messages, which must be JSON arrays, according to the following patterns:
// ["EVENT", <event JSON as defined above>], used to publish events.
// ["REQ", <subscription_id>, <filters JSON>...], used to request events and subscribe to new updates.
// ["CLOSE", <subscription_id>], used to stop previous subscriptions.
// Relays can send 3 types of messages, which must also be JSON arrays, according to the following patterns:
// ["EVENT", <subscription_id>, <event JSON as defined above>], used to send events requested by clients.
// ["EOSE", <subscription_id>], used to indicate the end of stored events and the beginning of events newly received in real-time.
// ["NOTICE", <message>], used to send human-readable error messages or other things to clients.
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string relay_t::make_response(const std::string& msg)
{
  std::string json;

  comm::json_to_file("relay_message.json", msg);

  nostr::Type Type = nostr::get_message_type(msg);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // EVENT
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (nostr::Type::EVENT == Type)
  {
    nostr::event_t ev;
    if (nostr::parse_client_event(msg, ev) < 0)
    {
    }
    nlohmann::json js = nlohmann::json::array({ "EOSE", ev.id });

    //add event to list
    database.push_back(ev);

    //save to database
    database::append(database);

    json = js.dump();
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  else if (nostr::Type::REQ == Type)
  {
    std::string request_id;
    nostr::filter_t filter;

    if (nostr::parse_request(msg, request_id, filter) < 0)
    {
    }
  
    //query database, return events that match the filter
    std::vector<nostr::event_t> events = database::request(filter);

    nlohmann::json js;
    if (events.size())
    {
      js = nlohmann::json::array({ "EVENT", request_id, events.at(0)});
    }
    else
    {
      js = nlohmann::json::array({ "EOSE", request_id });
    }
    json = js.dump();
  }

  else
  {
    nlohmann::json js = nlohmann::json::array({ "NOTICE", "Error" });
    json = js.dump();
  }


  comm::json_to_file("relay_response.json", json);
  return json;
}


