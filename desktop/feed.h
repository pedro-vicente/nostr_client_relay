#ifndef FEED_WINDOW_H
#define FEED_WINDOW_H

#include <QFrame>
#include "nostr.hh"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

///////////////////////////////////////////////////////////////////////////////////////
// FrameFeed
///////////////////////////////////////////////////////////////////////////////////////

class FrameFeed : public QFrame
{
public:
  FrameFeed();

  //a database is an array of events read and saved to filesystem in JSON format
  std::vector<nostr::event_t> database;

  Q_OBJECT
};


#endif
