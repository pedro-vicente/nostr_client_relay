#include "message.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

void to_json(nlohmann::json& j, const event_t& s)
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

void from_json(const nlohmann::json& j, event_t& s)
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
//to_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

void to_json(nlohmann::json& j, const filter_t& s)
{
  j["ids"] = s.ids;
  j["authors"] = s.authors;
  j["kinds"] = s.kinds;
  j["#e"] = s._e;
  j["#p"] = s._p;
  j["since"] = s.since;
  j["until"] = s.until;
  j["limit"] = s.limit;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//from_json
/////////////////////////////////////////////////////////////////////////////////////////////////////

void from_json(const nlohmann::json& j, filter_t& s)
{
  j.at("ids").get_to(s.ids);
  j.at("authors").get_to(s.authors);
  j.at("kinds").get_to(s.kinds);
  j.at("#e").get_to(s._e);
  j.at("#p").get_to(s._p);
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
