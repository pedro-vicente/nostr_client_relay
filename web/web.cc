// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "web.hh"
#include "home.hh"
#include "feed.hh"
#include "follows.hh"

std::string log_program_name("wostro");

std::string pubkey;

std::vector<std::string> relays = { "eden.nostr.land",
"nos.lol",
"relay.snort.social",
"relay.damus.io",
"nostr.wine",
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
    comm::start_log();
    Wt::WServer server(argc, argv);
    server.addEntryPoint(Wt::EntryPointType::Application, create_application);
    server.run();
  }
  catch (std::exception& e)
  {
    comm::log(e.what());
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication
// CSS layout: by default, the widget corresponds to a <div> tag.
/////////////////////////////////////////////////////////////////////////////////////////////////////

NostroApplication::NostroApplication(const Wt::WEnvironment& env)
  : WApplication(env)
{
  useStyleSheet("nostro.css");
  setTitle("Nostro");
  root()->setStyleClass("yellow-box");

  auto container = std::make_unique<Wt::WContainerWidget>();
  m_layout = container->setLayout(std::make_unique<Wt::WHBoxLayout>());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //menu
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container_menu = std::make_unique<Wt::WContainerWidget>();
  m_menu = container_menu->addNew<Wt::WMenu>();
  m_menu->setStyleClass("green-box");
  m_menu->setWidth(100);
  m_menu->addItem("Home", std::make_unique<Wt::WAnchor>(Wt::WLink(Wt::LinkType::InternalPath, "home")));
  m_menu->addItem("Feed", std::make_unique<Wt::WAnchor>(Wt::WLink(Wt::LinkType::InternalPath, "feed")));
  m_menu->addItem("Follows", std::make_unique<Wt::WAnchor>(Wt::WLink(Wt::LinkType::InternalPath, "follows")));
  m_menu->itemSelected().connect(this, &NostroApplication::selected);
  m_layout->addWidget(std::move(container_menu));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //home
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container_home = std::make_unique<ContainerHome>();
  m_layout->addWidget(std::move(container_home), 1);

  root()->addWidget(std::move(container));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//selected
/////////////////////////////////////////////////////////////////////////////////////////////////////

void NostroApplication::selected()
{
  Wt::WLayoutItem* item = m_layout->itemAt(1);
  m_layout->removeItem(item);
  int index = m_menu->currentIndex();
  std::unique_ptr<Wt::WContainerWidget> container;
  switch (index)
  {
  case 0: container = std::make_unique<ContainerHome>(); break;
  case 1: container = std::make_unique<ContainerFeed>(); break;
  case 2: container = std::make_unique<ContainerFollows>(); break;
  }
  m_layout->addWidget(std::move(container), 1);
}

