#ifndef NOSTRO_H
#define NOSTRO_H

#include <string>
#include <vector>
#include <ctime>

#undef HAVE_NOSTR

///////////////////////////////////////////////////////////////////////////////////////
// run GUI with or without nostr library (dummy JSON feed)
///////////////////////////////////////////////////////////////////////////////////////

const int show_color = 1;

#if defined HAVE_NOSTR
#include "client_wss.hpp"
#include "nostri.h"
#include "log.hh"
#include "database.hh"
#else
namespace nostr
{
  class event_t
  {
  public:
    event_t() : kind(1), created_at(std::time(0))
    {
    }
    std::string id;
    std::string pubkey;
    std::time_t created_at;
    int kind;
    std::vector<std::vector<std::string>> tags;
    std::string content;
    std::string sig;
  };
}
#endif

#endif
