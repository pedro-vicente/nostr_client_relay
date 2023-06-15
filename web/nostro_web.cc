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
#include "message.hh"
#include "database.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("nostro_web");
std::vector<std::string> store;
std::vector<std::string> message_recv;
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
  std::shared_ptr<Wt::WButtonGroup> m_button_message;

  Wt::WTable* m_table_messages;
  int m_row;
  void send_message();
  void make_message();
  void row_text(const Wt::WString& s);
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

  auto container = std::make_unique<Wt::WContainerWidget>();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //input
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string local = "localhost:8080/nostr";
  std::vector<std::string> relay = { "relay.snort.social",
   "relay.damus.io",
   "nostr.pleb.network" };
  std::string uri = relay[2];

  container->addWidget(std::make_unique<Wt::WText>("Relay wss://"));
  m_edit_uri = container->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_uri->setText(uri);
  m_edit_uri->setWidth(200);
  m_edit_uri->setMargin(10, Wt::Side::Top | Wt::Side::Bottom | Wt::Side::Right);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //radion buttons, select EVENT or REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_button_message = std::make_shared<Wt::WButtonGroup>();
  Wt::WRadioButton* button;
  button = container->addWidget(std::make_unique<Wt::WRadioButton>("Event"));
  m_button_message->addButton(button);
  button = container->addWidget(std::make_unique<Wt::WRadioButton>("Request"));
  m_button_message->addButton(button);
  m_button_message->setSelectedButtonIndex(1);

  auto container_row = container->addWidget(std::make_unique<Wt::WContainerWidget>());
  container_row->setStyleClass("row");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //EVENT
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto group_event = container_row->addWidget(std::make_unique<Wt::WGroupBox>("Event"));
  if (mark_div) group_event->setStyleClass("col");

  group_event->addWidget(std::make_unique<Wt::WText>("Private Key"));
  group_event->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_key = group_event->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_key->setWidth(500);
  group_event->addWidget(std::make_unique<Wt::WBreak>());

  group_event->addWidget(std::make_unique<Wt::WText>("Content"));
  group_event->addWidget(std::make_unique<Wt::WBreak>());

  m_area_content = group_event->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_content->setColumns(80);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //REQ
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto group_request = container_row->addWidget(std::make_unique<Wt::WGroupBox>("Request"));
  if (mark_div) group_request->setStyleClass("col");

  group_request->addWidget(std::make_unique<Wt::WText>("Event id"));
  group_request->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_event_id = group_request->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_event_id->setWidth(500);
  if (mark_div) m_edit_event_id->setText("d75d56b2141b12be96421fc5c913092cda06904208ef798b51a28f1c906bbab7");

  group_request->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //generated message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  container->addWidget(std::make_unique<Wt::WText>("Message"));

  m_area_input = container->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_input->setInline(false);
  m_area_input->setColumns(200);

  container->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //buttons
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto button_gen = container->addWidget(std::make_unique<Wt::WPushButton>("Generate message"));
  auto button_send = container->addWidget(std::make_unique<Wt::WPushButton>("Send message"));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //message received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_table_messages = container->addWidget(std::make_unique<Wt::WTable>());
  m_table_messages->setStyleClass("table_messages");

  button_send->clicked().connect(this, &NostroApplication::send_message);
  button_gen->clicked().connect(this, &NostroApplication::make_message);
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
    ss << "Received: " << "\"" << str << "\"";
    events::log(ss.str());

    store.push_back(str);
    message_recv.push_back(str);

    std::string buf;
    for (int idx = 0; idx < message_recv.size(); idx++)
    {
      buf += message_recv.at(idx);
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
    std::string out_message = str.toUTF8();

    ss.str(std::string());
    ss.clear();
    ss << "Sending: \"" << out_message << "\"";
    events::log(ss.str());

    m_table_messages->clear();
    m_row = 0;

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

    message_recv.clear();
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
  char* json = (char*)malloc(102400);

  //get content
  Wt::WString content = m_area_content->text();
  args.content = strdup(content.toUTF8().c_str());

  //get key
  Wt::WString sec = m_edit_key->text();
  if (sec.toUTF8().size())
  {
    args.sec = strdup(sec.toUTF8().c_str());
  }

  int id = m_button_message->checkedId();
  if (id == 1) args.req = 1; else args.req = 0;

  //get event id
  Wt::WString event_id = m_edit_event_id->text();
  if (event_id.toUTF8().size() && id == 1)
  {
    args.event_id = strdup(event_id.toUTF8().c_str());
  }

  int rand_req = 0;
  if (rand_req)
  {
    args.rand_req = 1;
  }

  if (::make_message(&args, &ev, &json) < 0)
  {
  }

  std::string out_message = json;
  m_area_input->setText(out_message);
  free(json);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication::make_message
/////////////////////////////////////////////////////////////////////////////////////////////////////

void NostroApplication::row_text(const Wt::WString& s)
{


}