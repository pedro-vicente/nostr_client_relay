#ifndef DATABASE_HH
#define DATABASE_HH

#include <string>
#include <vector>
#include "message.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//a database is an array of events read and saved to filesystem in JSON format
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace database
{
  std::vector<event_t> read();
  std::vector<event_t> request(const filter_t& filter);
  int save(const std::vector<event_t>& database);
  int append(const std::vector<event_t>& database);
}


#endif
