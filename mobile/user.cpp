#include "user.h"
#include "model.h"

const std::string def_pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");

/////////////////////////////////////////////////////////////////////////////////////////////////////
// User
/////////////////////////////////////////////////////////////////////////////////////////////////////

User::User(QObject* parent)
  : QObject{ parent }
{
  m_user = QString::fromUtf8(def_pubkey.c_str());
}

QString User::user()
{
  return m_user;
}

void User::set_user(const QString& key)
{
  if (key == m_user)
    return;

  m_user = key;
  emit user_changed();
}