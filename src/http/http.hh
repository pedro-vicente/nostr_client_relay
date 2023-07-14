// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#ifndef HTTP_HH
#define HTTP_HH

#include <string>
#include <vector>
#include "asio.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http_msg_t
//utility class to store various HTTP fields
/////////////////////////////////////////////////////////////////////////////////////////////////////

class http_msg_t
{
public:
  http_msg_t() :content_size(0) {};
  size_t content_size;
  std::string method; //POST or GET
  std::string url;
  std::vector<std::string> header; //store header lines
  std::string msg; //POST message, usually JSON
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//prototypes
/////////////////////////////////////////////////////////////////////////////////////////////////////

namespace http
{
  int parse(asio::ip::tcp::socket& sock, http_msg_t& http);
  std::string make_post(const std::string& host, const std::string& json);
  std::string get_method(const std::string& str_header);
  std::string get_url(const std::string& str_header);
};

#endif

