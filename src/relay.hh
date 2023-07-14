// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#ifndef RELAY_HH
#define RELAY_HH

#include <string>
#include "database.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// relay_t
// manage relay database
// a database is an array of events read and saved to filesystem in JSON format
/////////////////////////////////////////////////////////////////////////////////////////////////////

class relay_t
{
public:
  relay_t();
  std::vector<nostr::event_t> database;
  std::string make_response(const std::string& msg);
};

#endif