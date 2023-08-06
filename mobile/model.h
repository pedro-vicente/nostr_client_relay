#ifndef MODEL_FEED_H
#define MODEL_FEED_H

#include <QAbstractListModel>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Model
/////////////////////////////////////////////////////////////////////////////////////////////////////

class Model : public QAbstractListModel
{
  Q_OBJECT

public:
  Model();
  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  void populate();

private:
  QList<QString> m_data;
};

#endif 
