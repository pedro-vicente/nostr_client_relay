// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#include "follows.hh"

extern std::string pubkey;
extern std::vector<std::string> relays;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFollows
/////////////////////////////////////////////////////////////////////////////////////////////////////

ContainerFollows::ContainerFollows()
{
  this->setStyleClass("blue-box");

  auto container = std::make_unique<Wt::WContainerWidget>();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // options for table
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_check_raw = container->addNew<Wt::WCheckBox>("Raw message");
  m_check_raw->setChecked(true);
  m_check_raw->clicked().connect(this, &ContainerFollows::get_follows);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // table for events
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_table_messages = container->addWidget(std::make_unique<Wt::WTable>());
  m_table_messages->setStyleClass("table_messages");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get_follows
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  get_follows();

  this->addWidget(std::move(container));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFollows::get_follows
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ContainerFollows::get_follows()
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
    // get metadata returns an array of JSON events 
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> events;
    if (nostr::get_metadata(uri, pubkey, events) < 0)
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
          std::stringstream ss;
          ss << "follow." << row + 1 << ".json";
          comm::json_to_file(ss.str(), json);

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

            std::stringstream s;
            s << "content." << row + 1 << ".json";
            comm::json_to_file(s.str(), content);
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
//ContainerFollows::row_text
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ContainerFollows::row_text(const Wt::WString& s)
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