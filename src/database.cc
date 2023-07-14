// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include <fstream>
#include "log.hh"
#include "database.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::read
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<nostr::event_t> database::read()
{
  std::vector<nostr::event_t> database;

  try
  {
    std::ifstream ifs("database.json");
    nlohmann::json js_database = nlohmann::json::parse(ifs);
    ifs.close();
    database = js_database;
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
  }

  return database;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::save
/////////////////////////////////////////////////////////////////////////////////////////////////////

int database::save(const std::vector<nostr::event_t>& database)
{
  try
  {
    nlohmann::json js_database = database;
    std::string json = js_database.dump(2);
    std::ofstream ofs;
    ofs.open("database.json", std::ofstream::out);
    ofs << json;
    ofs.close();
    return 0;
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
    return -1;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::append
/////////////////////////////////////////////////////////////////////////////////////////////////////

int database::append(const std::vector<nostr::event_t>& events)
{
  try
  {
    //read current database
    std::vector<nostr::event_t> database = database::read();

    //append events
    for (int idx = 0; idx < events.size(); idx++)
    {
      nostr::event_t ev = events.at(idx);
      database.push_back(ev);
    }

    //save
    nlohmann::json js_database = database;
    std::string json = js_database.dump(2);
    std::ofstream ofs;
    ofs.open("database.json", std::ofstream::out);
    ofs << json;
    ofs.close();
    return 0;

  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
    return -1;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::request
//NIP-01
//Upon receiving a REQ message, the relay SHOULD query its internal database and return events that match the filter, 
//then store that filter and send again all future events it receives to that same websocket until the websocket is closed. 
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<nostr::event_t> database::request(const nostr::filter_t& filter)
{
  std::vector<nostr::event_t> events;

  try
  {
    //read current database
    std::vector<nostr::event_t> database = database::read();

    //query
    for (int idx_dbs = 0; idx_dbs < database.size(); idx_dbs++)
    {
      nostr::event_t ev = database.at(idx_dbs);

      //"authors" : <a list of pubkeys or prefixes, the pubkey of an event must be one of these>,
      //match with pubkey (only)
      for (int idx_aut = 0; idx_aut < filter.authors.size(); idx_aut++)
      {
        std::string author = filter.authors.at(idx_aut);
        if (ev.pubkey.compare(author) == 0)
        {
          events.push_back(ev);
          break;
        }
      }
    }
  }

  catch (const std::exception& e)
  {
    comm::log(e.what());
  }

  return events;
}
