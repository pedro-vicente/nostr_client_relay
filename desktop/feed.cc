#include <QWidget>
#include <QHBoxLayout>
#include <QFrame>
#include <QMouseEvent>
#include <QColor>
#include <QString>
#include <QTextStream>
#include <QTextEdit>
#include <QPushButton>
#include <QPixmap>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QLabel>

#include "feed.h"
#include "entry.h"
#include "store.h"

std::string log_program_name("nostro");
const std::string def_pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");
std::vector<std::string> relays = { "eden.nostr.land", "nos.lol", "relay.snort.social", "relay.damus.io", "nostr.wine" };

///////////////////////////////////////////////////////////////////////////////////////
// FrameFeed
///////////////////////////////////////////////////////////////////////////////////////

FrameFeed::FrameFeed()
{
#if defined HAVE_NOSTR

  int get_remote = 0;

  comm::start_log();

  //read saved database 
  database = database::read();

  ///////////////////////////////////////////////////////////////////////////////////////
  // get remote
  ///////////////////////////////////////////////////////////////////////////////////////

  if (get_remote)
  {
    std::vector<std::string> events;
    nostr::get_events(def_pubkey, relays.at(1), events);

    int row = 0;
    for (int idx_eve = 0; idx_eve < events.size(); idx_eve++)
    {
      std::string message = events.at(idx_eve);
      try
      {
        nlohmann::json js_message = nlohmann::json::parse(message);
        std::string message_type = js_message.at(0);

        // Relays can send 5 types of messages, which must also be JSON arrays, according to the following patterns:
        // ["EVENT", <subscription_id>, <event JSON as defined above>], used to send events requested by clients.
        // ["OK", <event_id>, <true|false>, <message>], used to indicate acceptance or denial of an EVENT message.
        // ["EOSE", <subscription_id>], used to indicate the end of stored events and the beginning of events newly received in real-time.
        // ["CLOSED", <subscription_id>, <message>], used to indicate that a subscription was ended on the server side.
        // ["NOTICE", <message>], used to send human-readable error messages or other things to clients.

        if (message_type.compare("EVENT") == 0)
        {
          nostr::event_t ev;
          from_json(js_message.at(2), ev); //["EVENT", <subscription_id>, <event JSON as defined above>]
          comm::log("event received: " + ev.content, 1);

          //add event to list
          database.push_back(ev);

          std::string json = js_message.dump();
          std::stringstream s;
          s << "follow." << row + 1 << ".json";
          comm::json_to_file(s.str(), json);
        } //EVENT
      }
      catch (const std::exception& e)
      {
        comm::log(e.what(), 1);
      }
    } //events

    //save to database
    database::append(database);
  }
#else
  read_database(database);
#endif

  QVBoxLayout* layout_main = new QVBoxLayout;

  for (size_t idx = 0; idx < database.size(); idx++)
  {
    nostr::event_t ev = database.at(idx);
    qDebug() << "Entry " << idx + 1 << ": " << ev.content.c_str();
    FeedEntry* label = new FeedEntry(ev);
    label->setFixedSize(label->size_widget);
    layout_main->addWidget(label);
  }

  QWidget* widget_container = new QWidget;
  if (show_color)
  {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::darkGray);
    widget_container->setAutoFillBackground(true);
    widget_container->setPalette(pal);
  }
  widget_container->setLayout(layout_main);

  QScrollArea* scroll_area = new QScrollArea;
  scroll_area->setFrameShape(QFrame::NoFrame);
  scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll_area->setWidgetResizable(true);

  scroll_area->setWidget(widget_container);

  QVBoxLayout* layout_scroll = new QVBoxLayout;
  layout_scroll->setContentsMargins(0, 0, 0, 0);
  layout_scroll->addWidget(scroll_area);

  setLayout(layout_scroll);
}
