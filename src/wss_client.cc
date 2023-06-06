#include "client_wss.hpp"
#include <future>
#include <fstream> 
#include <strstream>
#include "nostri.h"
#include "log.hh"
#include "message.hh"
#include "database.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("nostro");
std::vector<std::string> store;
void save(const std::string& name, const std::string& buf);

/////////////////////////////////////////////////////////////////////////////////////////////////////
// usage examples
// ./nostro --uri relay.damus.io --content hello 
// ./nostro --content hello --sec 7f612229528369d91ddcaae527f097ab4c7cacd0058fa46d5857f74f88ad1a5e 
// ./nostro --content hello --mine-pubkey --pow 1 
// 
// relays 
// wss://relay.snort.social
// wss://relay.damus.io
// wss://eden.nostr.land
// wss://nostr-pub.wellorder.net
// wss://nos.lol
// 
// real event ids for testing REQ
// 92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363 @jack
// ./nostro --uri relay.snort.social --req --id 92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363
// ./nostro --uri relay.damus.io --req --rand
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
  std::string message = R"(["REQ", "RAND", {"kinds": [1], "limit": 2}])";
  char* buf = (char*)malloc(102400);
  char* url = (char*)malloc(1024);
  events::start_log();

  if (make_message(argc, argv, &buf, &url) < 0)
  {
    return 0;
  }

  std::string uri = url;
  std::string json = buf;

  save("message.json", json);

  WssClient client(uri, false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::stringstream ss;
    std::string str = in_message->string();
    ss << "Received: " << "\"" << str << "\"";
    events::log(ss.str());
    store.push_back(str);
    save("response.txt", str);

    connection->send_close(1000);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_open = [&](std::shared_ptr<WssClient::Connection> connection)
  {
    std::stringstream ss;
    ss << "Opened connection: HTTP " << connection.get()->http_version << " , code " << connection.get()->status_code;
    events::log(ss.str());

    std::string out_message = json;
    ss.str(std::string());
    ss.clear();
    ss << "Sending: \"" << out_message << "\"";
    events::log(ss.str());

    connection->send(out_message);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_close = [](std::shared_ptr<WssClient::Connection>, int status, const std::string&)
  {
    std::stringstream ss;
    ss << "Closed: " << status;
    events::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
    std::stringstream ss;
    ss << "Error: " << ec << " : " << ec.message();
    events::log(ss.str());
  };

  client.start();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // messages received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::stringstream ss;
  ss << "Received " << store.size() << " messages: ";
  events::log(ss.str());

  for (int idx = 0; idx < store.size(); idx++)
  {
    events::log(store.at(idx));
  }

  std::stringstream js;
  js << "[";
  for (int idx = 0; idx < store.size(); idx++)
  {
    js << store.at(idx);
    std::string c = (idx < store.size() - 1) ? " ,\n" : "\n";
    js << c;
  }
  js << "]";
  std::string s = js.rdbuf()->str();
  save("response.json", s);

  free(url);
  free(buf);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// save
/////////////////////////////////////////////////////////////////////////////////////////////////////

void save(const std::string& name, const std::string& buf)
{
  std::ofstream ofs;
  ofs.open(name, std::ofstream::out);
  ofs << buf;
  ofs.close();
}
