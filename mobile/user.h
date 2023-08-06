#ifndef USER_HHH
#define USER_HHH

#include <QObject>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// User
/////////////////////////////////////////////////////////////////////////////////////////////////////

class User : public QObject
{
  Q_OBJECT
    Q_PROPERTY(QString user READ user WRITE set_user NOTIFY user_changed)

public:
  explicit User(QObject* parent = nullptr);
  QString user();
  void set_user(const QString& key);

signals:
  void user_changed();

private:
  QString m_user;
};

#endif
