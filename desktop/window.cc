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

///////////////////////////////////////////////////////////////////////////////////////
//MainWindow
///////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow()
{
  auto container = new QWidget;

  //horizontal layout for menu and feed
  auto layout = new QHBoxLayout;
  container->setLayout(layout);

  QFrame* frame1 = new QFrame();
  FrameFeed* frame2 = new FrameFeed();

  frame1->setStyleSheet("background-color: burlywood;");

  frame1->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  frame2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  layout->addWidget(frame1, 20);
  layout->addWidget(frame2, 80);

  setCentralWidget(container);
}
