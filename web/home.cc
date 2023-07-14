// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "home.hh"

extern std::string pubkey;
extern std::vector<std::string> relays;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerHome
/////////////////////////////////////////////////////////////////////////////////////////////////////

ContainerHome::ContainerHome() : m_row(0)
{
  this->setStyleClass("blue-box");

  //first Nostr event to "nostr.pleb.network"
  const std::string event_id("d75d56b2141b12be96421fc5c913092cda06904208ef798b51a28f1c906bbab7");
  const std::string def_pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");

  std::string uri = relays.at(1);
  pubkey = def_pubkey;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //top container input
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container_top = std::make_unique<Wt::WContainerWidget>();

  container_top->addWidget(std::make_unique<Wt::WText>("Relay wss://"));
  m_edit_uri = container_top->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_uri->setText(uri);
  m_edit_uri->setWidth(200);
  m_edit_uri->setMargin(10, Wt::Side::Top | Wt::Side::Bottom | Wt::Side::Right);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //radion buttons, select EVENT or REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_button_message = std::make_shared<Wt::WButtonGroup>();
  Wt::WRadioButton* button;
  button = container_top->addWidget(std::make_unique<Wt::WRadioButton>("Event"));
  m_button_message->addButton(button);
  button = container_top->addWidget(std::make_unique<Wt::WRadioButton>("Request"));
  m_button_message->addButton(button);
  m_button_message->setSelectedButtonIndex(1);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //seckey
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Wt::WText* wtext_seckey = container_top->addWidget(std::make_unique<Wt::WText>("Private Key"));
  wtext_seckey->setMargin(20, Wt::Side::Left);
  wtext_seckey->setMargin(10, Wt::Side::Right);
  m_edit_seckey = container_top->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_seckey->setWidth(400);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //pubkey
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Wt::WText* wtext_pubkey = container_top->addWidget(std::make_unique<Wt::WText>("Public Key"));
  wtext_pubkey->setMargin(20, Wt::Side::Left);
  wtext_pubkey->setMargin(10, Wt::Side::Right);
  m_edit_pubkey = container_top->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_pubkey->setWidth(500);
  m_edit_pubkey->setText(pubkey);
  m_edit_pubkey->changed().connect([=]
    {
      pubkey = m_edit_pubkey->text().toUTF8();
    });

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //containers for EVENT and REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container_row = std::make_unique<Wt::WContainerWidget>();
  auto box_row = container_row->setLayout(std::make_unique<Wt::WHBoxLayout>());
  auto box_left = box_row->addLayout(std::make_unique<Wt::WVBoxLayout>());
  auto box_right = box_row->addLayout(std::make_unique<Wt::WVBoxLayout>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //EVENT
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto group_event = box_left->addWidget(std::make_unique<Wt::WGroupBox>("Event"));
  group_event->setStyleClass("col");
  group_event->addWidget(std::make_unique<Wt::WText>("Content"));
  group_event->addWidget(std::make_unique<Wt::WBreak>());

  m_area_content = group_event->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_content->setColumns(80);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto group_request = box_right->addWidget(std::make_unique<Wt::WGroupBox>("Request"));
  group_request->setStyleClass("col");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // kind
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Wt::WText* wtext_kind = group_request->addWidget(std::make_unique<Wt::WText>("Kind"));
  wtext_kind->setMargin(10, Wt::Side::Right);
  m_combo_kind = group_request->addNew<Wt::WComboBox>();
  m_combo_kind->addItem("1 Short Text Note");
  m_combo_kind->addItem("3 Contacts");
  m_combo_kind->setCurrentIndex(0);
  m_combo_kind->setMargin(10, Wt::Side::Bottom);
  m_combo_kind->changed().connect([=]
    {
      //1 Short Text Note 1
      //3 Contacts 2
      int index = m_combo_kind->currentIndex();
      if (index == 1)
      {
        m_edit_author->setText(m_edit_pubkey->text());
      }
    });

  group_request->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // event id 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  group_request->addWidget(std::make_unique<Wt::WText>("Event id"));
  group_request->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_event_id = group_request->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_event_id->setWidth(500);
  m_edit_event_id->setInline(false);
  m_edit_event_id->setMargin(10, Wt::Side::Bottom);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // author
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  group_request->addWidget(std::make_unique<Wt::WText>("Author"));
  group_request->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_author = group_request->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_author->setWidth(500);
  m_edit_author->setMargin(10, Wt::Side::Bottom);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //generated message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container_message = std::make_unique<Wt::WContainerWidget>();
  container_message->addWidget(std::make_unique<Wt::WText>("Message"));
  m_area_message = container_message->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_message->setInline(false);
  m_area_message->setColumns(150);
  m_area_message->setHeight(200);
  container_message->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //buttons
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto button_gen = container_message->addWidget(std::make_unique<Wt::WPushButton>("Generate message"));
  auto button_send = container_message->addWidget(std::make_unique<Wt::WPushButton>("Send message"));
  m_check_raw = container_message->addNew<Wt::WCheckBox>("Raw message");
  m_check_raw->setChecked(true);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //message received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_table_messages = container_message->addWidget(std::make_unique<Wt::WTable>());
  m_table_messages->setStyleClass("table_messages");

  button_send->clicked().connect(this, &ContainerHome::send_message);
  button_gen->clicked().connect(this, &ContainerHome::make_message);

  auto container = std::make_unique<Wt::WContainerWidget>();
  container->addWidget(std::move(container_top));
  container->addWidget(std::move(container_row));
  container->addWidget(std::move(container_message));
  this->addWidget(std::move(container));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerHome::send_message
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ContainerHome::send_message()
{
  Wt::WString uri = m_edit_uri->text();
  WssClient client(uri.toUTF8(), false);
  std::vector<std::string> store;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [&](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::stringstream ss;
    std::string str = in_message->string();
    ss << "Received: " << str;
    comm::log(ss.str());
    store.push_back(str);

    try
    {
      nlohmann::json js_message = nlohmann::json::parse(str);
      std::string json = js_message.dump();
      comm::json_to_file("on_message.json", json);

      //Relays can send 3 types of messages, which must also be JSON arrays, according to the following patterns:
      //["EVENT", <subscription_id>, <event JSON as defined above>], used to send events requested by clients.
      //["EOSE", <subscription_id>], used to indicate the end of stored events and the beginning of events newly received in real - time.
      //["NOTICE", <message>], used to send human - readable error messages or other things to clients.

      std::string message_type = js_message.at(0);
      if (message_type.compare("EVENT") == 0)
      {
        nostr::event_t ev;
        from_json(js_message.at(2), ev);
        comm::log("event received: " + ev.content);
        if (m_check_raw->isChecked() == false)
        {
          str = ev.content;
        }
      }
      else
      {
        connection->send_close(1000);
      }
    }
    catch (const std::exception& e)
    {
      comm::log(e.what());
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
    comm::log(ss.str());

    std::string message = m_area_message->text().toUTF8();

    ss.str(std::string());
    ss.clear();
    ss << "Sending: " << message;
    comm::log(ss.str());
    comm::json_to_file("on_open_message.json", message);

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
    comm::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
    std::stringstream ss;
    ss << "Error: " << ec << " : " << ec.message();
    comm::log(ss.str());
  };


  client.start();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // messages received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::stringstream ss;
  ss << "Received " << store.size() << " messages: ";
  comm::log(ss.str());

  for (int idx = 0; idx < store.size(); idx++)
  {
    comm::log(store.at(idx));
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerHome::make_message
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ContainerHome::make_message()
{
  struct args args = { 0 };
  struct nostr_event ev = { 0 };
  char* buf = (char*)malloc(102400);
  std::string json_message;

  //request or event
  int is_req = m_button_message->checkedId();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (is_req)
  {
    int index = m_combo_kind->currentIndex();
    int kind = 0;
    if (index == 0) kind = 1;
    else if (index == 1) kind = 3;

    //get event id
    std::string event_id = m_edit_event_id->text().toUTF8();

    //get author
    std::string author = m_edit_author->text().toUTF8();

    //get pubkey
    std::string pubkey = m_edit_pubkey->text().toUTF8();

    std::string subscription_id = uuid::generate_uuid_v4();

    if (event_id.size())
    {
      subscription_id = event_id;
    }

    nostr::filter_t filter;
    filter.kinds.push_back(kind);
    if (kind == nostr::kind_3) //contacts
    {
      filter.authors.push_back(pubkey);
      filter.limit = 1;
    }
    else if (kind == nostr::kind_1)
    {
      if (author.size()) filter.authors.push_back(author);
      filter.limit = 50;
    }

    json_message = nostr::make_request(subscription_id, filter);
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //EVENT
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  else
  {
    //get content
    Wt::WString content = m_area_content->text();
    args.content = strdup(content.toUTF8().c_str());

    //get seckey
    Wt::WString sec = m_edit_seckey->text();
    if (sec.toUTF8().size())
    {
      args.sec = strdup(sec.toUTF8().c_str());
    }

    //nostril generated JSON
    if (::make_message(&args, &ev, &buf) < 0)
    {
    }

    json_message = buf;
  }

  //format JSON to display
  try
  {
    nlohmann::json js_message = nlohmann::json::parse(json_message);
    std::string json_indent = js_message.dump(1);
    m_area_message->setText(json_indent);
    comm::json_to_file("send_message.json", json_indent);
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
  }

  free(buf);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerHome::row_text
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ContainerHome::row_text(const Wt::WString& s)
{
  std::string message = s.toUTF8();
  try
  {
    nlohmann::json js = nlohmann::json::parse(message);
    std::string json = js.dump(1);
    comm::json_to_file("row_text.json", json);
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
  }
}
