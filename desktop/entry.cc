#include <QEvent>
#include <QHoverEvent>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QVideoWidget>
#include <QNetworkReply>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QString>
#include <sstream>
#include <fstream>
#include "entry.h"

///////////////////////////////////////////////////////////////////////////////////////
// find_url
// find if message has any URL
///////////////////////////////////////////////////////////////////////////////////////

int FeedEntry::find_url(const QString& content_buffer, QString& url)
{
  std::vector<QString> protocols = { "https://", "http://" };

  for (int idx = 0; idx < protocols.size(); idx++)
  {
    qsizetype pos = content_buffer.indexOf(protocols.at(idx));
    if (pos != -1)
    {
      qsizetype end = content_buffer.indexOf(" ", pos);
      if (end == -1)
      {
        end = content_buffer.size();
      }
      qsizetype len = end - pos;
      url = content_buffer.sliced(pos, len);
      return 1;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//FeedEntry::event
///////////////////////////////////////////////////////////////////////////////////////

bool FeedEntry::event(QEvent* e)
{
  if (m_player == nullptr)
  {
    return QWidget::event(e);
  }
  switch (e->type())
  {
  case QEvent::HoverEnter:
    m_player->play();
    m_audio->setVolume(10);
    return true;
    break;
  case QEvent::HoverLeave:
    m_player->pause();
    m_audio->setVolume(0);
    return true;
    break;
  case QEvent::HoverMove:
    return true;
    break;
  default:
    break;
  }
  return QWidget::event(e);
}

///////////////////////////////////////////////////////////////////////////////////////
// FeedEntry
///////////////////////////////////////////////////////////////////////////////////////

FeedEntry::FeedEntry(const nostr::event_t& nostr_event, QWidget* parent, Qt::WindowFlags f)
  : QLabel(parent, f),
  m_player(nullptr),
  m_audio(nullptr),
  m_label_image(nullptr),
  m_label_preview(nullptr)
{
  setAttribute(Qt::WA_Hover);
  QColor color = QWidget::palette().color(QWidget::backgroundRole());
  int r = color.red() + 30;
  int g = color.green() + 30;
  int b = color.blue() + 30;
  int a = color.alpha();
  std::stringstream s;
  s << ":hover{background:rgba(" << r << "," << g << "," << b << "," << a << ")}";
  setStyleSheet(QString::fromStdString(s.str()));

  const int feed_text_extra_height = 30;
  const int left_panel_width = 40;
  int right_panel_width = feed_width - left_panel_width;

  ///////////////////////////////////////////////////////////////////////////////////////
  // widget right (header, message text, images/media, <link preview>) )
  ///////////////////////////////////////////////////////////////////////////////////////

  m_layout_right = new QVBoxLayout();
  QWidget* widget_right = new QWidget(this);
  widget_right->setLayout(m_layout_right);

  ///////////////////////////////////////////////////////////////////////////////////////
  // widget header (user, date)
  ///////////////////////////////////////////////////////////////////////////////////////

  const int height_header = 30;
  QWidget* widget_header = new QWidget();
  const QSize size_header(right_panel_width, height_header);
  widget_header->setFixedSize(size_header);
  QHBoxLayout* layout_header = new QHBoxLayout();
  widget_header->setLayout(layout_header);
  if (show_color)
  {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::darkYellow);
    widget_header->setAutoFillBackground(true);
    widget_header->setPalette(pal);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // widget date
  ///////////////////////////////////////////////////////////////////////////////////////

  QLabel* widget_date = new QLabel();
  const QSize size_date(100, height_header);
  widget_date->setFixedSize(size_date);
  if (show_color)
  {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::darkRed);
    widget_date->setAutoFillBackground(true);
    widget_date->setPalette(pal);
  }
  std::time_t created_at = nostr_event.created_at;
  widget_date->setText("4/8/2024");

  ///////////////////////////////////////////////////////////////////////////////////////
  // widget user
  ///////////////////////////////////////////////////////////////////////////////////////

  QLabel* widget_user = new QLabel();
  const QSize size_user(200, height_header);
  widget_user->setFixedSize(size_user);
  if (show_color)
  {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::green);
    widget_user->setAutoFillBackground(true);
    widget_user->setPalette(pal);
  }
  QString pubkey = QString::fromStdString(nostr_event.pubkey);
  widget_user->setText(pubkey);

  //add to layout
  layout_header->addWidget(widget_user);
  layout_header->addWidget(widget_date);
  m_layout_right->addWidget(widget_header);

  ///////////////////////////////////////////////////////////////////////////////////////
  // widget text 
  ///////////////////////////////////////////////////////////////////////////////////////

  QString content_buffer = QString::fromStdString(nostr_event.content);
#ifdef _DEBUG
  std::string str = content_buffer.toStdString();
#endif

  QTextEdit* widget_text = new QTextEdit();
  widget_text->setText(content_buffer);
  widget_text->setReadOnly(true);
  widget_text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QFont font = widget_text->currentFont();
  QFontMetrics font_metrics(font);
  QSize size_metrics = font_metrics.size(Qt::TextIncludeTrailingSpaces, content_buffer);

  ///////////////////////////////////////////////////////////////////////////////////////
  // height of text widget
  ///////////////////////////////////////////////////////////////////////////////////////

  feed_height = size_metrics.height() + feed_text_extra_height;
  const QSize size_text(right_panel_width, feed_height);
  widget_text->setFixedSize(size_text);

  feed_height += height_header;

  if (show_color)
  {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::red);
    widget_text->setAutoFillBackground(true);
    widget_text->setPalette(pal);
  }

  //add to layout
  m_layout_right->addWidget(widget_text);

  ///////////////////////////////////////////////////////////////////////////////////////
  // find images/media
  ///////////////////////////////////////////////////////////////////////////////////////

  QString url;
  int has_url = find_url(content_buffer, url);

  if (has_url)
  {
    qDebug() << "url: " << url;
    QUrl url_media(url);

    ///////////////////////////////////////////////////////////////////////////////////////
    // image
    ///////////////////////////////////////////////////////////////////////////////////////

    if (url.contains(".png", Qt::CaseInsensitive)
      || url.contains(".jpg", Qt::CaseInsensitive)
      || url.contains(".webp", Qt::CaseInsensitive)
      || url.contains(".svg", Qt::CaseInsensitive))
    {
      QNetworkRequest request(url_media);
      QNetworkReply* reply = m_manager.get(request);
      connect(reply, &QNetworkReply::finished, this, &FeedEntry::on_finished_image);

      m_label_image = new QLabel();
      const QSize label_size(feed_width, 300);
      feed_height += 300;
      m_label_image->setFixedSize(label_size);

      //add to layout
      m_layout_right->addWidget(m_label_image);
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // movie
    ///////////////////////////////////////////////////////////////////////////////////////

    else if (url.contains(".mp4", Qt::CaseInsensitive))
    {
      m_player = new QMediaPlayer;
      m_audio = new QAudioOutput;
      m_player->setAudioOutput(m_audio);
      m_player->setSource(url_media);
      QVideoWidget* video = new QVideoWidget();
      m_player->metaData().value(QMediaMetaData::Resolution).toSize();
      video->setFixedSize(400, 300);
      m_player->setVideoOutput(video);
      m_audio->setVolume(0);

      m_player->play();

      feed_height += 300;

      //add to layout
      m_layout_right->addWidget(video);
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // html, insert link and get HTML for preview
    ///////////////////////////////////////////////////////////////////////////////////////

    else if (url.contains(".htm", Qt::CaseInsensitive) || url.contains(".html", Qt::CaseInsensitive))
    {
      //make a request to extract HTML
      QNetworkRequest request(url_media);
      QNetworkReply* reply = m_manager.get(request);
      connect(reply, &QNetworkReply::finished, this, &FeedEntry::on_finished_html);

      //display link URL 
      QLabel* label = new QLabel();
      const QSize label_size(right_panel_width, 100);
      feed_height += 100;
      label->setFixedSize(label_size);
      std::stringstream s;
      s << "<a href=\"" << url.toStdString() << "\">" << url.toStdString() << "</a>";
      QString link = QString::fromStdString(s.str());
      label->setText(link);
      label->setTextFormat(Qt::RichText);
      label->setTextInteractionFlags(Qt::TextBrowserInteraction);
      label->setOpenExternalLinks(true);

      //add
      m_layout_right->addWidget(label);
    }
  }


  ///////////////////////////////////////////////////////////////////////////////////////
  // widget left (user ID)
  ///////////////////////////////////////////////////////////////////////////////////////

  QLabel* widget_left = new QLabel();
  const QSize size_left(left_panel_width, feed_height);
  widget_left->setFixedSize(size_left);
  QString avatar = QString::fromStdString(nostr_event.pubkey);
  avatar = "avatar";
  widget_left->setText(avatar);

  if (show_color)
  {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::darkCyan);
    widget_left->setAutoFillBackground(true);
    widget_left->setPalette(pal);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // main widget
  ///////////////////////////////////////////////////////////////////////////////////////

  //add 'this' as parent 
  QWidget* widget_main = new QWidget(this);
  size_widget = QSize(feed_width, feed_height);
  QHBoxLayout* layout_main = new QHBoxLayout();
  widget_main->setLayout(layout_main);
  layout_main->addWidget(widget_left);
  layout_main->addWidget(widget_right);
  if (show_color)
  {
    QPalette pal = QPalette();
    pal.setColor(QPalette::Window, Qt::darkBlue);
    widget_main->setAutoFillBackground(true);
    widget_main->setPalette(pal);
  }
  widget_main->show();
}

///////////////////////////////////////////////////////////////////////////////////////
//FeedEntry::on_finished_html
///////////////////////////////////////////////////////////////////////////////////////

void FeedEntry::on_finished_html()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  qDebug() << "on_finished_html(): " << reply->request().url().toString();
  reply->deleteLater();

  QNetworkRequest request = reply->request();
  QUrl url = request.url();
  QString path = url.path();
  QString url_path = url.url();
  QString url_main = url_path.remove(path);

  QByteArray data = reply->readAll();

  QString html = QString(data);

  std::vector<QString> img_tag;

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // parse HTML
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  if (img_tag.size())
  {
    QString image_url = url_main + "/";
    image_url += QString(img_tag.at(0));
    QUrl url_image(image_url);

    m_label_preview = new QLabel();
    const QSize label_size(feed_width, 100);
    m_label_preview->setFixedSize(label_size);

    //add
    m_layout_right->addWidget(m_label_preview);

    QNetworkRequest request(url_image);
    QNetworkReply* reply = m_manager.get(request);
    connect(reply, &QNetworkReply::finished, this, &FeedEntry::on_finished_link_preview);
  }

  emit finished_html();
}

///////////////////////////////////////////////////////////////////////////////////////
//FeedEntry::on_finished_link_preview
///////////////////////////////////////////////////////////////////////////////////////

void FeedEntry::on_finished_link_preview()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  qDebug() << "on_finished_link_preview(): " << reply->request().url().toString();
  reply->deleteLater();

  assert(m_label_preview != nullptr);

  QByteArray data = reply->readAll();
  QImage image;
  image.loadFromData(data);
  m_label_preview->setPixmap(QPixmap::fromImage(image));
  m_label_preview->setFixedSize(image.width(), image.height());

  emit finished_link_preview();
}

///////////////////////////////////////////////////////////////////////////////////////
//FeedEntry::on_finished_image
///////////////////////////////////////////////////////////////////////////////////////

void FeedEntry::on_finished_image()
{
  QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
  qDebug() << "on_finished_image(): " << reply->request().url().toString();
  reply->deleteLater();

  assert(m_label_image != nullptr);

  QByteArray data = reply->readAll();

  QImage image;
  image.loadFromData(data);
  m_label_image->setPixmap(QPixmap::fromImage(image));
  m_label_image->setFixedSize(image.width(), image.height());

  emit finished_image();
}
