#ifndef MESSAGE_HH
#define MESSAGE_HH

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

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
  std::vector<std::string> ids;
  std::vector<std::string> authors;
  std::vector<int> kinds;
  std::vector<std::string> _e;
  std::vector<std::string> _p;
  std::time_t since;
  std::time_t until;
  size_t limit;
};

void to_json(nlohmann::json& j, const filter_t& s);
void from_json(const nlohmann::json& j, filter_t& s);



#endif

