#ifndef ENTRY_FEED_NOSTR_H
#define ENTRY_FEED_NOSTR_H

#include "nostro.h"
#include <QLabel>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
class QMediaPlayer;
class QAudioOutput;
class QNetworkReply;
class QLabel;
class QVBoxLayout;
QT_END_NAMESPACE

///////////////////////////////////////////////////////////////////////////////////////
// LabelEntry
///////////////////////////////////////////////////////////////////////////////////////

class FeedEntry : public QLabel
{
  Q_OBJECT
public:
  explicit FeedEntry(const nostr::event_t& nostr_event, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  QSize size_widget;

protected:
  QNetworkAccessManager m_manager;
  QMediaPlayer* m_player;
  QAudioOutput* m_audio;
  QLabel* m_label_image;
  QLabel* m_label_preview;
  QVBoxLayout* m_layout_right;

  int feed_height;
  int find_url(const QString& text_buffer, QString& url);
  const int feed_width = 800;

protected:
  bool event(QEvent* e);

signals:
  void finished_html();
protected slots:
  void on_finished_html();

signals:
  void finished_image();
protected slots:
  void on_finished_image();

signals:
  void finished_link_preview();
protected slots:
  void on_finished_link_preview();
};

#endif