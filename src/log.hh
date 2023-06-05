#ifndef LOG_HH
#define LOG_HH

#include <string>

namespace events
{
  void start_log();
#ifdef _MSC_VER
  void log(const std::string& str, bool to_file = true);
#else
  void log(const std::string& str, bool to_file = false);
#endif
  std::string get_time_now(const std::string& time_format = "%X");
};

#endif

