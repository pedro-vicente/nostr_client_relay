#include "feed.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFeed
/////////////////////////////////////////////////////////////////////////////////////////////////////

ContainerFeed::ContainerFeed() : m_row(0)
{
  this->setStyleClass("blue-box");

  auto app = dynamic_cast<NostroApplication*>(Wt::WApplication::instance());
  std::string uri = app->relays.at(1);
  std::vector<std::string> store;

  std::string subscription_id = uuid::generate_uuid_v4();
  nostr::filter_t filter;

  filter.authors.push_back(app->pubkey);
  filter.kinds.push_back(3);

  std::string message = nostr::make_request(subscription_id, filter);
  events::json_to_file("send_message.json", message);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //message received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container = std::make_unique<Wt::WContainerWidget>();
  m_table_messages = container->addWidget(std::make_unique<Wt::WTable>());
  m_table_messages->setStyleClass("table_messages");

  this->addWidget(std::move(container));

  WssClient client(uri, false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [&](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::stringstream ss;
    std::string str = in_message->string();
    ss << "Received: " << str;
    events::log(ss.str());
    store.push_back(str);

    try
    {
      nlohmann::json js_message = nlohmann::json::parse(str);
      std::string json = js_message.dump();
      events::json_to_file("on_message.json", json);

      //Relays can send 3 types of messages, which must also be JSON arrays, according to the following patterns:
      //["EVENT", <subscription_id>, <event JSON as defined above>], used to send events requested by clients.
      //["EOSE", <subscription_id>], used to indicate the end of stored events and the beginning of events newly received in real - time.
      //["NOTICE", <message>], used to send human - readable error messages or other things to clients.

      std::string message_type = js_message.at(0);
      if (message_type.compare("EVENT") == 0)
      {
        nostr::event_t ev;
        from_json(js_message.at(2), ev);
        events::log("event received: " + ev.content);
      }
      else if (message_type.compare("EOSE") == 0)
      {
        connection->send_close(1000);
      }
    }
    catch (const std::exception& e)
    {
      events::log(e.what());
    }

    Wt::WText* wtext = m_table_messages->elementAt(m_row, 0)->addNew<Wt::WText>(str);
    wtext->clicked().connect([=]()
      {
        row_text(wtext->text());
      });
    m_row++;

  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_open = [&](std::shared_ptr<WssClient::Connection> connection)
  {
    std::stringstream ss;
    ss << "Opened connection: HTTP " << connection.get()->http_version << " , code " << connection.get()->status_code;
    events::log(ss.str());

    ss.str(std::string());
    ss.clear();
    ss << "Sending: " << message;
    events::log(ss.str());
    events::json_to_file("on_open_message.json", message);

    m_table_messages->clear();
    m_row = 0;

    connection->send(message);
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFeed::row_text
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ContainerFeed::row_text(const Wt::WString& s)
{
  std::string message = s.toUTF8();
  try
  {
    nlohmann::json js = nlohmann::json::parse(message);
    std::string json = js.dump(1);
    events::json_to_file("row_text.json", json);
  }
  catch (const std::exception& e)
  {
    events::log(e.what());
  }
}
