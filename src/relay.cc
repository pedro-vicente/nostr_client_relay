#include <random>
#include <sstream>
#include <fstream> 
#include <iostream>
#include "client_wss.hpp"
#include "nlohmann/json.hpp"
#include "nostri.h"
#include "hex.h"
#include "log.hh"
#include "nostr.hh"
#include "uuid.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// make_response
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string relay::make_response(const std::string& msg)
{
  std::string json;

  json = msg;

  return json;
}