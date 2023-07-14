// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "feed.hh"

extern std::string pubkey;
extern std::vector<std::string> relays;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFeed
/////////////////////////////////////////////////////////////////////////////////////////////////////

ContainerFeed::ContainerFeed()
{
  this->setStyleClass("blue-box");

  auto container = std::make_unique<Wt::WContainerWidget>();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // options for table
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_check_raw = container->addNew<Wt::WCheckBox>("Raw message");
  m_check_raw->setChecked(true);

  auto button_follows = container->addWidget(std::make_unique<Wt::WPushButton>("Get Feed"));
  button_follows->setInline(false);
  button_follows->clicked().connect(this, &ContainerFeed::get_feed);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // table for events
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_table_messages = container->addWidget(std::make_unique<Wt::WTable>());
  m_table_messages->setStyleClass("table_messages");

  this->addWidget(std::move(container));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFeed::get_feed
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ContainerFeed::get_feed()
{
  m_table_messages->clear();

  std::string uri = relays.at(1);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get follows returns an array of pubkeys
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<std::string> pubkeys;
  if (nostr::get_follows(uri, pubkey, pubkeys) < 0)
  {
  }

  comm::to_file("pubkeys.txt", pubkeys);

  int row = 0;
  for (int idx_key = 0; idx_key < pubkeys.size(); idx_key++)
  {
    std::string pubkey = pubkeys.at(idx_key);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // get feed returns an array of JSON events 
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> events;
    if (nostr::get_feed(uri, pubkey, events) < 0)
    {
    }

    for (int idx_eve = 0; idx_eve < events.size(); idx_eve++)
    {
      std::string message = events.at(idx_eve);
      try
      {
        nlohmann::json js = nlohmann::json::parse(message);
        std::string type = js.at(0);
        if (type.compare("EVENT") == 0)
        {
          nostr::event_t ev;
          from_json(js.at(2), ev);
          std::string json = js.dump(1);
          std::stringstream s;
          s << "follow." << row + 1 << ".json";
          comm::json_to_file(s.str(), json);

          /////////////////////////////////////////////////////////////////////////////////////////////////////
          // add complete JSON formatted message to HTML table or just contents
          /////////////////////////////////////////////////////////////////////////////////////////////////////

          Wt::WText* wtext;
          if (m_check_raw->isChecked() == true)
          {
            wtext = m_table_messages->elementAt(row, 0)->addNew<Wt::WText>(json);
          }
          else
          {
            std::string content = ev.content;
            wtext = m_table_messages->elementAt(row, 0)->addNew<Wt::WText>(pubkey);
            wtext = m_table_messages->elementAt(row, 1)->addNew<Wt::WText>(content);
          }


          wtext->setHeight(100);
          wtext->clicked().connect([=]()
            {
              row_text(wtext->text());
            });
          row++;
        }
      }
      catch (const std::exception& e)
      {
        comm::log(e.what());
      }
    } //events
  } //pubkeys

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
    comm::json_to_file("row_text.json", json);
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
  }
}

