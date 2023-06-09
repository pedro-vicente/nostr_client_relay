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
  Wt::WTextArea* m_area_output;
  Wt::WLineEdit* m_edit_uri;
  Wt::WLineEdit* m_edit_key;
  Wt::WLineEdit* m_edit_event_id;
  Wt::WCheckBox* m_check_request;
  void send_message();
  void make_message();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
// --docroot=. --http-port=80 --http-address=0.0.0.0 
// --docroot=. --https-port=4430 --https-address=0.0.0.0 --ssl-certificate=server.crt --ssl-private-key=server.key --ssl-tmp-dh=dh2048.pem 
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
/////////////////////////////////////////////////////////////////////////////////////////////////////

NostroApplication::NostroApplication(const Wt::WEnvironment& env)
  : WApplication(env)
{
  setTitle("Nostro");
  const int width = 500;

  auto container = std::make_unique<Wt::WContainerWidget>();
  auto box_main = container->setLayout(std::make_unique<Wt::WHBoxLayout>());
  auto box_left = box_main->addLayout(std::make_unique<Wt::WVBoxLayout>());
  auto box_right = box_main->addLayout(std::make_unique<Wt::WVBoxLayout>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //input, left
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string local = "localhost:8080/nostr";
  std::string uri = "relay.snort.social";

  box_left->addWidget(std::make_unique<Wt::WText>("Relay Uri"));
  m_edit_uri = box_left->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_uri->setFocus();
  m_edit_uri->setText(uri);
  m_edit_uri->setWidth(200);

  auto group_event = box_left->addWidget(std::make_unique<Wt::WGroupBox>("Event"));

  group_event->addWidget(std::make_unique<Wt::WText>("Public Key"));
  group_event->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_key = group_event->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_key->setWidth(width);
  group_event->addWidget(std::make_unique<Wt::WBreak>());

  group_event->addWidget(std::make_unique<Wt::WText>("Content"));
  group_event->addWidget(std::make_unique<Wt::WBreak>());

  m_area_content = group_event->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_content->setInline(false);
  m_area_content->setColumns(100);
  m_area_content->resize(Wt::WLength::Auto, 100);
  m_area_content->setFocus();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //request
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto group_request = box_left->addWidget(std::make_unique<Wt::WGroupBox>("Request"));

  m_check_request = group_request->addWidget(std::make_unique<Wt::WCheckBox>("Request"));
  m_check_request->setChecked(true);
  group_request->addWidget(std::make_unique<Wt::WBreak>());

  group_request->addWidget(std::make_unique<Wt::WText>("Event id"));
  group_request->addWidget(std::make_unique<Wt::WBreak>());
  m_edit_event_id = group_request->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_event_id->setWidth(width);
  group_request->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //generated message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  box_left->addWidget(std::make_unique<Wt::WText>("Message"));

  m_area_input = box_left->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_input->setInline(false);
  m_area_input->setColumns(100);
  m_area_input->resize(Wt::WLength::Auto, 200);
  m_area_input->setFocus();

  box_left->addWidget(std::make_unique<Wt::WBreak>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //buttons
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto hbox_buttons = box_left->addLayout(std::make_unique<Wt::WHBoxLayout>());
  auto button_gen = hbox_buttons->addWidget(std::make_unique<Wt::WPushButton>("Generate message"));
  auto button_send = hbox_buttons->addWidget(std::make_unique<Wt::WPushButton>("Send message"));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //output, right
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_area_output = box_right->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_output->setInline(false);
  m_area_output->setColumns(100);
  m_area_output->resize(Wt::WLength::Auto, 600);
  m_area_output->setReadOnly(true);

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
    m_area_output->setText(buf);

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

  bool is_req = m_check_request->isChecked();
  if (is_req) args.req = 1; else args.req = 0;

  //get event id
  Wt::WString event_id = m_edit_event_id->text();
  if (event_id.toUTF8().size() && is_req == true)
  {
    args.event_id = strdup(event_id.toUTF8().c_str());
  }

  if (::make_message(&args, &ev, &json) < 0)
  {
  }

  std::string out_message = json;
  m_area_input->setText(out_message);
  free(json);
}
