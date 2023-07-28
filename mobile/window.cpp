#include "window.h"
#include "client_wss.hpp"
#include <future>
#include "nostri.h"
#include "log.hh"
#include "database.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("mostro");
const std::string def_pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");
std::string pubkey;
std::vector<std::string> relays = { "eden.nostr.land",
"nos.lol",
"relay.snort.social",
"relay.damus.io",
"nostr.wine",
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MainWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////

MainWidget::MainWidget()
{
  QScreen* screen = QGuiApplication::primaryScreen();
  QRect  screenGeometry = screen->geometry();
  int height = screenGeometry.height();
  int width = screenGeometry.width();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Key
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  QVBoxLayout* layout_edit = new QVBoxLayout;
  m_edit = new QLineEdit(this);
  QString str = QString::fromStdString(def_pubkey.c_str());
  m_edit->setMinimumWidth(400);
  m_edit->setPlaceholderText(str);
  layout_edit->addWidget(m_edit);
  QGroupBox* group_key = new QGroupBox("Pub Key");
  group_key->setLayout(layout_edit);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Feed
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  QVBoxLayout* layout_feed = new QVBoxLayout;
  m_button = new QPushButton("Get", this);
  connect(m_button, &QPushButton::released, this, &MainWidget::button_feed);
  layout_feed->addWidget(m_button);
  QGroupBox* group_feed = new QGroupBox("Feed");
  group_feed->setLayout(layout_feed);

  m_table = new QTableWidget(this);
  m_table->horizontalHeader()->hide();
  m_table->resizeRowsToContents();
  m_table->setColumnCount(1);
  m_table->setColumnWidth(0, width);
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_table->setSelectionMode(QAbstractItemView::SingleSelection);
  m_table->setShowGrid(true);
  m_table->setStyleSheet("QTableView {selection-background-color: grey;}");
  connect(m_table, SIGNAL(cellClicked(int, int)), this, SLOT(cell_selected(int, int)));
  layout_feed->addWidget(m_table);


  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(group_key);
  layout->addWidget(group_feed);
  setLayout(layout);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//button_feed
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWidget::button_feed()
{
  get_events();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//get_events
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWidget::get_events()
{
  std::string uri = relays.at(1);
  pubkey = def_pubkey;
  std::vector<std::string> response;
  nostr::get_events(pubkey, uri, response);

  std::stringstream ss;
  ss << "Received " << response.size() << " messages: ";
  comm::log(ss.str());

  m_table->clear();
  m_table->setRowCount(response.size());

  for (int idx = 0; idx < response.size(); idx++)
  {
    QString str = QString::fromStdString(response.at(idx));
    m_table->setItem(idx, 0, new QTableWidgetItem(str));

    comm::log(response.at(idx));
  }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//cell_selected
/////////////////////////////////////////////////////////////////////////////////////////////////////

void MainWidget::cell_selected(int row, int col)
{

}