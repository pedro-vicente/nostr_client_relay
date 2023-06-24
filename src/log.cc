#include <ctime>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "log.hh"

std::ofstream ofs_log;
extern std::string log_program_name;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//events::start_log
//must be called at start of every program
//opens an output file stream for file generation
/////////////////////////////////////////////////////////////////////////////////////////////////////

void events::start_log()
{
  ofs_log.open(log_program_name + ".log.txt", std::ofstream::trunc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//events::log
//print program name and time only for standard output
//print full date and time for file
//time format
//http://en.cppreference.com/w/cpp/chrono/c/strftime
/////////////////////////////////////////////////////////////////////////////////////////////////////

void events::log(const std::string& msg, bool to_file)
{
  std::cout << log_program_name << ":" << get_time_now("%X") << " " << msg << std::endl;
  if (to_file) ofs_log << get_time_now("%Y-%m-%d,%X") << " " << msg << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//events::get_time_now
//return current time as string
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string events::get_time_now(const std::string& time_format)
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
// events::json_to_file
/////////////////////////////////////////////////////////////////////////////////////////////////////

void events::json_to_file(const std::string& name, const std::string& buf)
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
    events::log(e.what());
  }
}
