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

// --docroot=. --http-address=0.0.0.0 --http-port=80 
// --docroot=. --https-address=0.0.0.0 --ssl-certificate=server.crt --ssl-private-key=server.key --ssl-tmp-dh=dh2048.pem 

/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication
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
  void send();
  void generate();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication
/////////////////////////////////////////////////////////////////////////////////////////////////////

NostroApplication::NostroApplication(const Wt::WEnvironment& env)
  : WApplication(env)
{
  setTitle("Nostro");

  auto container = std::make_unique<Wt::WContainerWidget>();
  auto box_main = container->setLayout(std::make_unique<Wt::WHBoxLayout>());
  auto box_left = box_main->addLayout(std::make_unique<Wt::WVBoxLayout>());
  auto box_right = box_main->addLayout(std::make_unique<Wt::WVBoxLayout>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //input, left
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string local = "localhost:8080/nostr";
  std::string uri = "relay.damus.io";

  box_left->addWidget(std::make_unique<Wt::WText>("Relay Uri"));
  m_edit_uri = box_left->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_uri->setFocus();
  m_edit_uri->setText(uri);
  m_edit_uri->setWidth(200);

  box_left->addWidget(std::make_unique<Wt::WText>("Public Key"));
  m_edit_key = box_left->addWidget(std::make_unique<Wt::WLineEdit>());
  m_edit_key->setWidth(500);

  box_left->addWidget(std::make_unique<Wt::WBreak>());
  box_left->addWidget(std::make_unique<Wt::WText>("Content"));

  m_area_content = box_left->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_content->setInline(false);
  m_area_content->setColumns(100);
  m_area_content->resize(Wt::WLength::Auto, 200);
  m_area_content->setFocus();

  box_left->addWidget(std::make_unique<Wt::WBreak>());
  box_left->addWidget(std::make_unique<Wt::WText>("Event"));

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
  auto button_gen = hbox_buttons->addWidget(std::make_unique<Wt::WPushButton>("Generate event"));
  auto button_send = hbox_buttons->addWidget(std::make_unique<Wt::WPushButton>("Send event"));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //output, right
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_area_output = box_right->addWidget(std::make_unique<Wt::WTextArea>());
  m_area_output->setInline(false);
  m_area_output->setColumns(100);
  m_area_output->resize(Wt::WLength::Auto, 400);
  m_area_output->setReadOnly(true);

  button_send->clicked().connect(this, &NostroApplication::send);
  button_gen->clicked().connect(this, &NostroApplication::generate);
  root()->addWidget(std::move(container));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication::send
/////////////////////////////////////////////////////////////////////////////////////////////////////

void NostroApplication::send()
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
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<Wt::WApplication> create_application(const Wt::WEnvironment& env)
{
  return std::make_unique<NostroApplication>(env);
}

int main(int argc, char** argv)
{
  events::start_log();

  Wt::WServer server(argc, argv);
  server.addEntryPoint(Wt::EntryPointType::Application, create_application);
  server.run();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication::generate
/////////////////////////////////////////////////////////////////////////////////////////////////////

void NostroApplication::generate()
{
  std::string message = R"(["REQ", "RAND", {"kinds": [1], "limit": 2}])";

  struct args args = { 0 };
  struct nostr_event ev = { 0 };
  struct key key;
  secp256k1_context* ctx;
  if (!init_secp_context(&ctx))
    return;

  make_event_from_args(&ev, &args);

  //get content
  Wt::WString str = m_area_content->text();
  ev.content = strdup(str.toUTF8().c_str());

  //always envelop (send)
  args.flags |= HAS_ENVELOPE;

  //get key
  Wt::WString sec = m_edit_key->text();
  if (sec.toUTF8().size())
  {
    args.sec = strdup(sec.toUTF8().c_str());
  }

  if (args.sec)
  {
    if (!decode_key(ctx, args.sec, &key))
    {
      return;
    }
  }
  else
  {
    int* difficulty = NULL;
    if ((args.flags & HAS_DIFFICULTY) && (args.flags & HAS_MINE_PUBKEY))
    {
      difficulty = &args.difficulty;
    }

    if (!generate_key(ctx, &key, difficulty))
    {
      fprintf(stderr, "could not generate key\n");
      return;
    }
    fprintf(stderr, "secret_key ");
    print_hex(key.secret, sizeof(key.secret));
    fprintf(stderr, "\n");
  }

  if (args.flags & HAS_ENCRYPT)
  {
    int kind = args.flags & HAS_KIND ? args.kind : 4;
    if (!make_encrypted_dm(ctx, &key, &ev, args.encrypt_to, kind))
    {
      fprintf(stderr, "error making encrypted dm\n");
      return;
    }
  }

  // set the event's pubkey
  memcpy(ev.pubkey, key.pubkey, 32);

  if (args.flags & HAS_DIFFICULTY && !(args.flags & HAS_MINE_PUBKEY))
  {
    if (!mine_event(&ev, args.difficulty))
    {
      fprintf(stderr, "error when mining id\n");
      return;
    }
  }
  else
  {
    if (!generate_event_id(&ev))
    {
      fprintf(stderr, "could not generate event id\n");
      return;
    }
  }

  if (!sign_event(ctx, &key, &ev))
  {
    fprintf(stderr, "could not sign event\n");
    return;
  }

  if (!print_event(&ev, args.flags & HAS_ENVELOPE))
  {
    fprintf(stderr, "buffer too small\n");
    return;
  }

  std::ifstream ifs("nostril.json"); //generated in 'print_event'
  std::stringstream buf;
  buf << ifs.rdbuf();
  std::string out_message = buf.str();
  m_area_input->setText(out_message);
}
