#include <string>
#include <vector>
#include <fstream> 
#include <iostream>
#include <sstream>
#include "uuid.hh"
#include "log.hh"
#include "nostr.hh"

std::string log_program_name("test_nostro");

/////////////////////////////////////////////////////////////////////////////////////////////////////
// prototypes
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_list(const std::string& file_name);
int read_lists();
int relay_all(const std::string& json);

std::vector<std::string> relays = { "eden.nostr.land",
  "nos.lol",
  "relay.snort.social",
  "relay.damus.io",
  "nostr.wine",
};

std::vector<std::string> list = { "list_01.txt",
"list_02.txt",
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  const std::string pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");

  events::start_log();

  std::vector<std::string> response;
  nostr::get_follows(relays.at(1), pubkey, response);

  std::vector<nostr::event_t> events;

  for (int idx = 0; idx < response.size(); idx++)
  {
    std::string message = response.at(idx);

    try
    {
      nlohmann::json js = nlohmann::json::parse(message);
      std::string type = js.at(0);
      if (type.compare("EVENT") == 0)
      {
        nostr::event_t ev;
        from_json(js.at(2), ev);
        events.push_back(ev);
        events::log("event received: " + ev.content);
        std::string json = js.dump();
        events::json_to_file("event_follow.json", json);
      }
    }
    catch (const std::exception& e)
    {
      events::log(e.what());
    }
  }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// read_lists
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_lists()
{
  for (int idx = 0; idx < list.size(); idx++)
  {
    if (read_list(list.at(idx)) < 0)
    {
      return -1;
    }
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// read_list
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_list(const std::string& file_name)
{
  std::ifstream ifs;
  ifs.open(file_name);
  if (!ifs.is_open())
  {
    return -1;
  }
  std::string json;
  while (std::getline(ifs, json))
  {
    std::cout << json << std::endl;
    nostr::Type Type = nostr::get_message_type(json);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // EVENT
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    if (nostr::Type::EVENT == Type)
    {
      std::string event_id;
      nostr::event_t ev;
      if (nostr::parse_event(json, event_id, ev) < 0)
      {
        assert(0);
      }
      std::cout << event_id << std::endl;
      std::cout << ev.content << std::endl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // REQ
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    else if (nostr::Type::REQ == Type)
    {
      std::string request_id;
      nostr::filter_t filter;
      if (nostr::parse_request(json, request_id, filter) < 0)
      {
        assert(0);
      }
      std::cout << request_id << std::endl;
      if (filter.authors.size())
      {
        for (int idx = 0; idx < filter.authors.size(); idx++)
        {
          std::cout << filter.authors.at(idx) << " ";
        }
        std::cout << std::endl;
      }
    }
  }
  ifs.close();
  return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// relay_all
/////////////////////////////////////////////////////////////////////////////////////////////////////

int relay_all(const std::string& json)
{
  for (int idx = 0; idx < relays.size(); idx++)
  {
    std::string uri = relays.at(idx);
    std::vector<std::string> store;
    if (nostr::relay_to(uri, json, store) < 0)
    {
    }
  }
  return 0;
}

