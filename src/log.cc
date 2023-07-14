// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include <ctime>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "nlohmann/json.hpp"
#include "log.hh"

std::ofstream ofs_log;
extern std::string log_program_name;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//comm::start_log
//must be called at start of every program
//opens an output file stream for file generation
/////////////////////////////////////////////////////////////////////////////////////////////////////

void comm::start_log()
{
  ofs_log.open(log_program_name + ".log.txt", std::ofstream::trunc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//comm::log
//print program name and time only for standard output
//print full date and time for file
//time format
//http://en.cppreference.com/w/cpp/chrono/c/strftime
/////////////////////////////////////////////////////////////////////////////////////////////////////

void comm::log(const std::string& msg, bool to_file)
{
  std::cout << log_program_name << ":" << get_time_now("%X") << " " << msg << std::endl;
  if (to_file) ofs_log << get_time_now("%Y-%m-%d,%X") << " " << msg << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//comm::get_time_now
//return current time as string
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string comm::get_time_now(const std::string& time_format)
{
  char buf[80];
  std::time_t t = std::time(0); // get time now
  std::tm* now = std::localtime(&t);
  //http://en.cppreference.com/w/cpp/chrono/c/strftime
  strftime(buf, sizeof(buf), time_format.c_str(), now);
  std::string str(buf);
  return str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// comm::json_to_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

void comm::json_to_file(const std::string& name, const std::string& buf)
{
  try
  {
    nlohmann::json js_message = nlohmann::json::parse(buf);
    std::string json = js_message.dump(1);
    std::ofstream ofs;
    ofs.open(name, std::ofstream::out);
    ofs << json;
    ofs.close();

    std::cout << json << std::endl;
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// comm::to_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

void comm::to_file(const std::string& name, const std::string& buf)
{
  std::ofstream ofs;
  ofs.open(name, std::ofstream::out);
  ofs << buf;
  std::cout << buf << std::endl;
  ofs.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// comm::from_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

int comm::from_file(const std::string& name, std::string& buf)
{
  std::ifstream ifs(name);
  if (!ifs.is_open())
  {
    return -1;
  }
  std::stringstream ss;
  ss << ifs.rdbuf();
  buf = ss.str();

  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// comm::to_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

void comm::to_file(const std::string& name, const std::vector<std::string>& vec)
{
  std::ofstream ofs;
  ofs.open(name, std::ofstream::out);
  for (int idx = 0; idx < vec.size(); idx++)
  {
    ofs << vec.at(idx) << std::endl;
    std::cout << vec.at(idx) << std::endl;
  }

  ofs.close();
}

