// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#ifndef LOG_HH
#define LOG_HH

#include <string>
#include <vector>

namespace comm
{
  void start_log();
  void log(const std::string& str, bool to_file = true);
  std::string get_time_now(const std::string& time_format = "%X");
  void json_to_file(const std::string& name, const std::string& buf);
  void to_file(const std::string& name, const std::string& buf);
  int from_file(const std::string& name, std::string& buf);
  void to_file(const std::string& name, const std::vector<std::string>& vec);
};

#endif

