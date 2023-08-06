#include "model.h"
#include <QMultiMap>

#include "client_wss.hpp"
#include <future>
#include "nostri.h"
#include "log.hh"
#include "database.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("mostro");
const std::string def_pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");
std::string pubkey;
std::vector<std::string> relays = { "eden.nostr.land",
"nos.lol",
"relay.snort.social",
"relay.damus.io",
"nostr.wine",
};


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Model
/////////////////////////////////////////////////////////////////////////////////////////////////////

Model::Model()
{
  populate();
}

int Model::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return m_data.count();
}

int Model::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1;
}

QVariant Model::data(const QModelIndex& index, int role) const
{
  if (index.row() < 0 || index.row() >= m_data.count())
    return QVariant();

  switch (role) {
  case Qt::DisplayRole:
    return m_data.at(index.row());

  default:
    break;
  }

  return QVariant();
}

void Model::populate()
{
  std::string uri = relays.at(1);
  pubkey = def_pubkey;
  std::vector<std::string> response;
  nostr::get_events(pubkey, uri, response);

  for (int idx = 0; idx < response.size(); ++idx)
  {
    nlohmann::json js_message = nlohmann::json::parse(response.at(idx));
    std::string json = js_message.dump(1);
    QString str = QString::fromUtf8(json.c_str());
    m_data.push_back(str);
  }

}

