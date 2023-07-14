// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include <iostream>
#include <fstream>
#include "http.hh"
#include "log.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http::make_post
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string http::make_post(const std::string& host, const std::string& json)
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //construct HTTP
  //Only used HTTP field is "Content-Length"
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::stringstream ss;
  ss << "POST / HTTP/1.1\r\n";
  ss << "Host: " << host << "\r\n";
  ss << "Connection: close\r\n";
  ss << "Content-Type: application/json\r\n";
  ss << "Content-Length: " << std::to_string(json.size()) << "\r\n";
  ss << "\r\n"; //terminate HTTP headers
  ss << json;
  std::string buf = ss.str();
  return buf;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http::get_method
//return HTTP method as a string (e.g GET, POST)
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string http::get_method(const std::string& header)
{
  std::string method;
  std::string s = header.substr(0, 4);
  if (s.compare("HTTP") == 0)
  {
    return method; //this is a response (HTTP/1.1 200 OK)
  }
  size_t pos = header.find(" ");
  if (pos == std::string::npos)
  {
    comm::log("HTTP bad format");
    return method;
  }
  method = header.substr(0, pos);
  return method;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http::get_url
//return url as a string (e.g returns /resourceurl from the header, GET /resourceurl HTTP/1.1)
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string http::get_url(const std::string& header)
{
  std::string url;
  std::string s = header.substr(0, 4);
  if (s.compare("HTTP") == 0)
  {
    return url; //this is a response (HTTP/1.1 200 OK)
  }
  size_t pos = header.find(" ");
  size_t ver_pos = header.find(" HTTP");
  if (pos == std::string::npos)
  {
    comm::log("HTTP bad format");
    return url;
  }
  if (pos == ver_pos)
  {
    return url;
  }
  url = header.substr(pos + 1, ver_pos - pos - 1);
  return url;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//http::parse
//read incoming HTTP from a socket and export to 'http_msg_t'
//Note: reads HTTP header 'Content-Length'
/////////////////////////////////////////////////////////////////////////////////////////////////////

int http::parse(asio::ip::tcp::socket& sock, http_msg_t& http)
{
  std::string line;
  asio::streambuf buf;
  std::stringstream ss;
  asio::error_code error;
  size_t content_size = 0;
  size_t read_left = 0;
  try
  {
    //read until end of HTTP header
    //Note:: after a successful read_until operation, the streambuf may contain additional data 
    //beyond the delimiter. An application will typically leave that data in the streambuf for a subsequent 
    //read_until operation to examine.

    asio::read_until(sock, buf, "\r\n\r\n");
    std::istream stream(&buf);
    while (std::getline(stream, line) && line != "\r")
    {
      http.header.push_back(line);
    }

    //store method and url
    line = http.header.at(0);
    http.method = http::get_method(line);
    http.url = http::get_url(line);

    //find 'Content-Length'
    for (int idx = 0; idx < http.header.size(); idx++)
    {
      line = http.header.at(idx);
      if (line.find("Content-Length: ") != std::string::npos)
      {
        size_t start = line.find(":");
        start += 2; //space
        size_t end = line.find("\r");
        std::string s = line.substr(start, end - 1);
        try
        {
          content_size = std::atoi(s.c_str());
        }
        catch (std::invalid_argument&)
        {
          comm::log("invalid Content-Length");
          return -1;
        }
        http.content_size = content_size;
      }
    }

    if (http.content_size == 0)
    {
      //nothing to read; not a POST; must be an acknowledgement response 
      return 0;
    }

    //read end of message left
    //dump whatever content we already have
    if (buf.size() > 0)
    {
      ss << &buf;
      std::string s = ss.str();
      read_left = content_size - s.size();
    }
    else
    {
      read_left = content_size;
    }

    //asio::read reads exact number of bytes
    size_t recv = asio::read(sock, buf, asio::transfer_exactly(read_left));
    ss << &buf;
  }
  catch (std::exception& e)
  {
    comm::log(e.what());
    return -1;
  }
  http.msg = ss.str();
  return 0;
}
