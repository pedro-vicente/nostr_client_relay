#include "menu.h"
#include "window.h"
#include <QLabel>
#include <QVBoxLayout>

extern
MainWindow* main_window;

///////////////////////////////////////////////////////////////////////////////////////
// Menu
///////////////////////////////////////////////////////////////////////////////////////

Menu::Menu()
{
  auto layout = new QVBoxLayout;
  setLayout(layout);

  QLabel* home = new QLabel(this);
  connect(home, &QLabel::linkHovered, this, &Menu::home_hovered);
  connect(home, &QLabel::linkActivated, this, &Menu::home_activated);
  home->setText("<a href=\"whatever\">Home</a>");
  home->setOpenExternalLinks(false);

  QLabel* feed = new QLabel(this);
  connect(feed, &QLabel::linkHovered, this, &Menu::feed_hovered);
  connect(feed, &QLabel::linkActivated, this, &Menu::feed_activated);
  feed->setText("<a href=\"whatever\">Feed</a>");
  feed->setOpenExternalLinks(false);

  layout->addWidget(home);
  layout->addWidget(feed);
}

void Menu::home_hovered(const QString& link)
{

}

void Menu::home_activated(const QString& link)
{
  main_window->active_frame = 0;
  main_window->populate();
}

void Menu::feed_hovered(const QString& link)
{

}

void Menu::feed_activated(const QString& link)
{
  main_window->active_frame = 1;
  main_window->populate();
}

