#ifndef MENU_WINDOW_H
#define MENU_WINDOW_H

#include <QFrame>
#include <QString>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

///////////////////////////////////////////////////////////////////////////////////////
// Menu
///////////////////////////////////////////////////////////////////////////////////////

class Menu : public QFrame
{
  Q_OBJECT
public:
  Menu();

private slots:
  void home_hovered(const QString& link);
  void home_activated(const QString& link);

  void feed_hovered(const QString& link);
  void feed_activated(const QString& link);
};

#endif