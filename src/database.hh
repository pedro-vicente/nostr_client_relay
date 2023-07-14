// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#ifndef DATABASE_HH
#define DATABASE_HH

#include <string>
#include <vector>
#include "nostr.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//a database is an array of events read and saved to filesystem in JSON format
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace database
{
  std::vector<nostr::event_t> read();
  std::vector<nostr::event_t> request(const nostr::filter_t& filter);
  int save(const std::vector<nostr::event_t>& database);
  int append(const std::vector<nostr::event_t>& database);
}


#endif
