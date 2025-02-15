#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QDir>
#include "store.h"

///////////////////////////////////////////////////////////////////////////////////////
// read_database
///////////////////////////////////////////////////////////////////////////////////////

void read_database(std::vector<nostr::event_t>& database)
{
  ///////////////////////////////////////////////////////////////////////////////////////
  // read test JSON
  // HomeLocation: Mac: "~" Windows "C:/Users/<USER>"
  ///////////////////////////////////////////////////////////////////////////////////////

  QString database_location = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
  QDir dir(database_location);
  if (!dir.exists())
  {
    dir.mkpath(database_location);
  }
  if (!dir.exists("nostro"))
  {
    dir.mkdir("nostro");
  }
  dir.cd("nostro");
  database_location = dir.absoluteFilePath("feed.json");

  QFile file;
  file.setFileName(database_location);

  if (QFile::exists(database_location))
  {
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString buf = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(buf.toUtf8());
    QJsonArray json_array = doc.array();
    for (auto v : json_array)
    {
      QJsonObject obj = v.toObject();
      QString key = obj.keys().at(0);
      QString value = obj.value(key).toString();

      //make a dummy event
      nostr::event_t ev;
      ev.content = value.toStdString();

      database.push_back(ev);
    }
  }
  else
  {
    qDebug() << database_location;
  }
}
