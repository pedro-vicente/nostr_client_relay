// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "uuid.hh"
#include <random>
#include <sstream>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// uuid
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace uuid
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> dis(0, 15);
  static std::uniform_int_distribution<> dis2(8, 11);

  std::string generate_uuid_v4()
  {
    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++)
    {
      ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++)
    {
      ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++)
    {
      ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++)
    {
      ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++)
    {
      ss << dis(gen);
    };
    std::string str = ss.str();
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
  }
}
