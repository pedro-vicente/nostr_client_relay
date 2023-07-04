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
int get_follows();
int make_messages();

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

  make_messages();

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
      comm::json_to_file("event.json", json);
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
      comm::json_to_file("request.json", json);
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

/////////////////////////////////////////////////////////////////////////////////////////////////////
// get_follows
/////////////////////////////////////////////////////////////////////////////////////////////////////

int get_follows()
{
  const std::string pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");

  comm::start_log();

  std::vector<std::string> response;
  nostr::get_follows(relays.at(1), pubkey, response);

  std::vector<nostr::event_t> events;
  std::vector<std::string> follows;

  int row = 0;
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
        comm::log("event received: " + ev.content);

        std::string json = js.dump(1);
        std::stringstream s;
        s << "follow." << row + 1 << ".json";
        comm::json_to_file(s.str(), json);
        row++;

        follows.push_back(ev.pubkey);
      }
    }
    catch (const std::exception& e)
    {
      comm::log(e.what());
    }
  }

  comm::to_file("follows.txt", follows);

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// make_messages 
/////////////////////////////////////////////////////////////////////////////////////////////////////

int make_messages()
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // make_request
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  {
    const std::string pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");
    std::string subscription_id = "my_id";
    nostr::filter_t filter;
    filter.authors.push_back(pubkey);
    filter.kinds.push_back(1);
    filter.limit = 1;
    std::string json = nostr::make_request(subscription_id, filter);

    comm::json_to_file("request.json", json);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // parse_request
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  {
    std::string json = R"([
     "REQ",
     "34E8C71B-C0FB-4D6D-9CBB-694A091D6A2D",
     {
      "authors": [
       "4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b"
      ],
      "kinds": [
       1
      ],
      "limit": 1
     }
    ])";
    std::string request_id;
    nostr::filter_t filter;
    nostr::parse_request(json, request_id, filter);
    std::cout << request_id << std::endl;
    std::cout << filter.authors.at(0) << std::endl;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // parse_event
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  {
    std::string json = R"([
     "EVENT",
     "34E8C71B-C0FB-4D6D-9CBB-694A091D6A2D",
     {
      "content": "https://github.com/pedro-vicente/nostr_client_relay",
      "created_at": 1686887953,
      "id": "24d7deac8173f5e7ead51282106728ae39d44aa558ff3c5b3b236bece71684ef",
      "kind": 1,
      "pubkey": "4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b",
      "sig": "14d1b7ea0cd8f4275221c297d0d30f5afe2a87a13b743211ed23bcb5dac095498b2b34f645544e3c956577d6554d812fbf908ccb7857b2e920875046579a105c",
      "tags": []
     }
    ])";
    std::string event_id;
    nostr::event_t ev;
    nostr::parse_event(json, event_id, ev);
    std::cout << event_id << std::endl;
    std::cout << ev.content << std::endl;
  }


  {
    std::string json = R"([
     "REQ",
     "34E8C71B-C0FB-4D6D-9CBB-694A091D6A2D",
     {
      "authors": [
       "4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b"
      ],
      "kinds": [
       1
      ],
      "limit": 1
     }
    ])";
    std::string uri = "nos.lol";
    std::vector<std::string> response;
    nostr::relay_to(uri, json, response);
    comm::to_file("response.txt", response);
    for (int idx = 0; idx < response.size(); idx++)
    {
      std::string message = response.at(idx);
      std::cout << message << std::endl;
    }
  }


  {
    std::vector<std::string> response;
    std::string uri = "nos.lol";
    const std::string pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");
    nostr::get_follows(uri, pubkey, response);
    comm::to_file("response.txt", response);
    for (int idx = 0; idx < response.size(); idx++)
    {
      std::string message = response.at(idx);
      std::cout << message << std::endl;
    }
   
  }


  return 0;
}