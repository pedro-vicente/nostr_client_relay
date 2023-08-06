#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "user.h"
#include "model.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  QGuiApplication app(argc, argv);
  QQmlApplicationEngine engine;

  User* user = new User(&app);
  qmlRegisterSingletonInstance("User", 1, 0, "User", user);

  Model model;
  engine.rootContext()->setContextProperty("Model", &model);

  const QUrl url(u"qrc:/qml/main.qml"_qs);
  engine.load(url);

  return app.exec();
}
