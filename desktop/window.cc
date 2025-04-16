#include <QGuiApplication>
#include <QScreen>
#include <QScrollArea>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QDir>

#include "window.h"
#include "entry.h"
#include "store.h"
#include "feed.h"
#include "menu.h"
#include "home.h"

///////////////////////////////////////////////////////////////////////////////////////
// MainWindow
///////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow() : active_frame(1)
{
  populate();
}

///////////////////////////////////////////////////////////////////////////////////////
// populate
///////////////////////////////////////////////////////////////////////////////////////

void MainWindow::populate()
{
  auto container = new QWidget;

  //horizontal layout for menu and feed
  auto layout = new QHBoxLayout;
  container->setLayout(layout);

  Menu* menu = new Menu();

  menu->setStyleSheet("background-color: burlywood;");
  menu->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  layout->addWidget(menu, 20);

  if (active_frame == 1)
  {
    FrameFeed* feed = new FrameFeed();
    feed->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(feed, 80);
  }
  else if (active_frame == 0)
  {
    Home* home = new Home();
    home->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layout->addWidget(home, 80);
  }

  setCentralWidget(container);

}
