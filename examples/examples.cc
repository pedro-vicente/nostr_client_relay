#include <string>
#include <vector>
#include <fstream> 
#include <iostream>
#include <sstream>
#include "uuid.hh"
#include "log.hh"
#include "nostr.hh"

std::string log_program_name("examples");

/////////////////////////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
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
    nostr::parse_relay_event(json, event_id, ev);
    std::cout << event_id << std::endl;
    std::cout << ev.content << std::endl;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // relay_to
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

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get_follows
  /////////////////////////////////////////////////////////////////////////////////////////////////////

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

  /////////////////////////////////////////////////////////////////////////////////////////////////////
 // make_event
 /////////////////////////////////////////////////////////////////////////////////////////////////////

  {
    std::optional<std::string> seckey;
    nostr::event_t ev;
    ev.content = "hello world";
    ev.kind = 1;
    std::string json = nostr::make_event(ev, seckey);

    comm::json_to_file("event.json", json);
  }

  return 0;
}