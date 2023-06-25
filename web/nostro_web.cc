#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WServer.h>
#include <Wt/WTextArea.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WCheckBox.h>
#include <Wt/WGroupBox.h>
#include <Wt/WTable.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WTemplate.h>

#include "client_wss.hpp"
#include <future>
#include <fstream> 

#include "nostri.h"
#include "log.hh"
#include "nostr.hh"
#include "database.hh"

//first Nostr relayed event id (06/15/23) 
const std::string default_event_id("d75d56b2141b12be96421fc5c913092cda06904208ef798b51a28f1c906bbab7");
const std::string default_author("35d26e4690cbe1");
using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("nostro_web");
std::vector<std::string> store;
const int mark_div = 1;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication
// real event ids for testing REQ
// ./nostro --uri relay.snort.social --req --id 92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363
// ./nostro --uri relay.damus.io --req --rand
/////////////////////////////////////////////////////////////////////////////////////////////////////

class NostroApplication : public Wt::WApplication
{
public:
  NostroApplication(const Wt::WEnvironment& env);

private:
  Wt::WTextArea* m_area_content;
  Wt::WTextArea* m_area_input;
  Wt::WLineEdit* m_edit_uri;
  Wt::WLineEdit* m_edit_key;
  Wt::WLineEdit* m_edit_event_id;
  Wt::WLineEdit* m_edit_author;
  std::shared_ptr<Wt::WButtonGroup> m_button_message;

  Wt::WTable* m_table_messages;
  int m_row;
  void send_message();
  void make_message();
  void row_text(const Wt::WString& s);

  Wt::WCheckBox* m_check_raw;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
// --docroot=. --http-port=80 --http-address=0.0.0.0 
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<Wt::WApplication> create_application(const Wt::WEnvironment& env)
{
  return std::make_unique<NostroApplication>(env);
}

int main(int argc, char** argv)
{
  try
  {
    events::start_log();

    Wt::WServer server(argc, argv);
    server.addEntryPoint(Wt::EntryPointType::Application, create_application);
    server.run();
  }
  catch (std::exception& e)
  {
    events::log(e.what());
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication
// CSS layout: by default, the widget corresponds to a <div> tag.
/////////////////////////////////////////////////////////////////////////////////////////////////////

NostroApplication::NostroApplication(const Wt::WEnvironment& env)
  : WApplication(env), m_row(0)
{
  useStyleSheet("nostro.css");
  root()->setStyleClass("div_yellow_box");
  setTitle("Nostro");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //top container input
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container_top = std::make_unique<Wt::WContainerWidget>();

  std::string local = "localhost:8080/nostr";
  std::vector<std::string> relay = { "relay.snort.social",
   "relay.damus.io",
   "nostr.pleb.network" };
  std::string uri = relay[0];

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
  if (mark_div) group_event->setStyleClass("col");

  group_event->addWidget(std::make_unique<Wt::WText>("Private Key"));
  group_event->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_key = group_event->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_key->setWidth(400);
  group_event->addWidget(std::make_unique<Wt::WBreak>());

  group_event->addWidget(std::make_unique<Wt::WText>("Content"));
  group_event->addWidget(std::make_unique<Wt::WBreak>());

  m_area_content = group_event->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_content->setColumns(80);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto group_request = box_right->addWidget(std::make_unique<Wt::WGroupBox>("Request"));
  if (mark_div) group_request->setStyleClass("col");

  group_request->addWidget(std::make_unique<Wt::WText>("Event id"));
  group_request->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_event_id = group_request->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_event_id->setWidth(500);
  m_edit_event_id->setInline(false);
  if (mark_div) m_edit_event_id->setText(default_event_id);

  group_request->addWidget(std::make_unique<Wt::WText>("Author"));
  group_request->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_author = group_request->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_author->setWidth(400);
  if (mark_div) m_edit_author->setText(default_author);
  group_request->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //generated message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container_message = std::make_unique<Wt::WContainerWidget>();
  container_message->addWidget(std::make_unique<Wt::WText>("Message"));
  m_area_input = container_message->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_input->setInline(false);
  m_area_input->setColumns(150);
  m_area_input->setHeight(200);
  container_message->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //buttons
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto button_gen = container_message->addWidget(std::make_unique<Wt::WPushButton>("Generate message"));
  auto button_send = container_message->addWidget(std::make_unique<Wt::WPushButton>("Send message"));
  m_check_raw = container_message->addNew<Wt::WCheckBox>("Raw message");
  m_check_raw->setChecked(false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //message received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_table_messages = container_message->addWidget(std::make_unique<Wt::WTable>());
  m_table_messages->setStyleClass("table_messages");

  button_send->clicked().connect(this, &NostroApplication::send_message);
  button_gen->clicked().connect(this, &NostroApplication::make_message);

  auto container = std::make_unique<Wt::WContainerWidget>();
  container->addWidget(std::move(container_top));
  container->addWidget(std::move(container_row));
  container->addWidget(std::move(container_message));
  root()->addWidget(std::move(container));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication::send_message
/////////////////////////////////////////////////////////////////////////////////////////////////////

void NostroApplication::send_message()
{
  Wt::WString uri = m_edit_uri->text();
  WssClient client(uri.toUTF8(), false);

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
        if (m_check_raw->isChecked() == false) str = ev.content;
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

    Wt::WString str = m_area_input->text();
    std::string message = str.toUTF8();

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
//NostroApplication::make_message
/////////////////////////////////////////////////////////////////////////////////////////////////////

void NostroApplication::make_message()
{
  struct args args = { 0 };
  struct nostr_event ev = { 0 };
  char* buf = (char*)malloc(102400);

  //get content
  Wt::WString content = m_area_content->text();
  args.content = strdup(content.toUTF8().c_str());

  //get key
  Wt::WString sec = m_edit_key->text();
  if (sec.toUTF8().size())
  {
    args.sec = strdup(sec.toUTF8().c_str());
  }

  //request or event
  int is_req = m_button_message->checkedId();
  if (is_req == 1) args.req = 1; else args.req = 0;

  //get event id
  Wt::WString event_id = m_edit_event_id->text();
  if (event_id.toUTF8().size() && is_req == 1)
  {
    args.event_id = strdup(event_id.toUTF8().c_str());
  }

  //get event id
  Wt::WString author = m_edit_author->text();
  if (author.toUTF8().size() && is_req == 1)
  {
    args.author = strdup(author.toUTF8().c_str());
  }

  //random request 
  int rand_req = 0;
  if (rand_req && is_req == 1)
  {
    args.rand_req = 1;
  }

  //nostril generated JSON
  if (::make_message(&args, &ev, &buf) < 0)
  {
  }

  //format JSON to display
  try
  {
    nlohmann::json js_message = nlohmann::json::parse(buf);
    std::string json = js_message.dump(1);
    m_area_input->setText(json);
    events::json_to_file("send_message.json", json);
  }
  catch (const std::exception& e)
  {
    events::log(e.what());
  }

  free(buf);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication::make_message
/////////////////////////////////////////////////////////////////////////////////////////////////////

void NostroApplication::row_text(const Wt::WString& s)
{


}