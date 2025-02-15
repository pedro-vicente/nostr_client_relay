#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include "window.h"

void make_feed();

///////////////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  MainWindow window;
  QSize screen = QGuiApplication::primaryScreen()->size();
  window.resize(screen.width(), screen.height());
  window.showMaximized();
  return app.exec();
}

///////////////////////////////////////////////////////////////////////////////////////
// make_feed
///////////////////////////////////////////////////////////////////////////////////////

void make_feed()
{
  const QString lorem("Lorem ipsum dolor sit amet, consectetur adipiscing elit.\nAliquam aliquam consequat pellentesque. Donec mattis dapibus risus quis ultrices. Donec facilisis nulla neque, nec placerat odio blandit et.\n Vivamus iaculis consequat lacus et semper. Integer ornare id nisi sed interdum.\n Nulla varius pulvinar tortor, sit amet ullamcorper nulla gravida vel.Vestibulum vel sollicitudin leo \n");
  std::vector<QString> feed;

  //test emoji, jpeg, png, mp4, html
  QString entry_text(lorem);
  entry_text.append("\U0001F600");
  QString entry_image(lorem);
  entry_image.append("http://100.36.4.152/image/image.jpg");
  QString entry_movie(lorem);
  entry_movie.append("http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4 ");
  QString entry_html(lorem);
  entry_html.append("http://100.36.4.152/index.html ");

  feed.push_back(entry_html);
  feed.push_back(entry_text);
  feed.push_back(entry_image);
  feed.push_back(entry_movie);

  QFile file("feed.json");
  file.open(QIODevice::ReadWrite | QIODevice::Text);

  QJsonArray json_array;
  QJsonDocument json_doc;

  for (size_t idx = 0; idx < feed.size(); idx++)
  {
    QJsonObject json_object;
    json_object.insert("entry", feed.at(idx));
    json_array.append(json_object);
  }

  json_doc.setArray(json_array);
  file.write(json_doc.toJson());
  file.close();
}