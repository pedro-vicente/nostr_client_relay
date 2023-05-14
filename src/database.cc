#include <fstream>
#include "database.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::read
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<event_t> database::read()
{
  std::ifstream ifs("database.json");
  nlohmann::json js_database = nlohmann::json::parse(ifs);
  ifs.close();
  std::vector<event_t> database = js_database;
  return database;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::save
/////////////////////////////////////////////////////////////////////////////////////////////////////

int database::save(const std::vector<event_t>& database)
{
  nlohmann::json js_database = database;
  std::string json = js_database.dump(2);
  std::ofstream ofs;
  ofs.open("database.json", std::ofstream::out);
  ofs << json;
  ofs.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::append
/////////////////////////////////////////////////////////////////////////////////////////////////////

int database::append(const std::vector<event_t>& events)
{
  //read current database
  std::vector<event_t> database = database::read();

  //append events
  for (int idx = 0; idx < events.size(); idx++)
  {
    event_t ev = events.at(idx);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
//database::request
//NIP-01
//Upon receiving a REQ message, the relay SHOULD query its internal database and return events that match the filter, 
//then store that filter and send again all future events it receives to that same websocket until the websocket is closed. 
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<event_t> database::request(const filter_t& filter)
{
  std::vector<event_t> events;

  //read current database
  std::vector<event_t> database = database::read();

  //query
  for (int idx_dbs = 0; idx_dbs < database.size(); idx_dbs++)
  {
    event_t ev = database.at(idx_dbs);

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

  return events;
}
