#include "feed.hh"

extern std::string pubkey;
extern std::vector<std::string> relays;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFeed
/////////////////////////////////////////////////////////////////////////////////////////////////////

ContainerFeed::ContainerFeed()
{
  this->setStyleClass("blue-box");

  std::string uri = relays.at(1);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // table for events
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  auto container = std::make_unique<Wt::WContainerWidget>();
  m_table_messages = container->addWidget(std::make_unique<Wt::WTable>());
  m_table_messages->setStyleClass("table_messages");

  this->addWidget(std::move(container));

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get follows
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<nostr::event_t> events;
  std::vector<std::string> response;
  nostr::get_follows(uri, pubkey, response);
  int row = 0;

  for (int idx = 0; idx < response.size(); idx++)
  {
    std::string message = response.at(idx);
    try
    {
      nlohmann::json js = nlohmann::json::parse(message);
      std::string type = js.at(0);
      if (type.compare("EVENT") == 0)
      {
        nostr::event_t ev;
        from_json(js.at(2), ev);
        events.push_back(ev);
        std::string json = js.dump(1);
        std::stringstream s;
        s << "follow." << row + 1 << ".json";
        comm::json_to_file(s.str(), json);

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // add complete JSON formatted message to HTML table 
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        Wt::WText* wtext = m_table_messages->elementAt(row, 0)->addNew<Wt::WText>(json);
        wtext->setHeight(100);
        wtext->clicked().connect([=]()
          {
            row_text(wtext->text());
          });
        row++;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // parse tags
        /////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int idx = 0; idx < ev.tags.size(); idx++)
        {
          std::vector<std::string> tag = ev.tags.at(idx);
          std::string pubkey = tag.at(1);
        }
      }
    }
    catch (const std::exception& e)
    {
      comm::log(e.what());
    }
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
    comm::json_to_file("row_text.json", json);
  }
  catch (const std::exception& e)
  {
    comm::log(e.what());
  }

}

