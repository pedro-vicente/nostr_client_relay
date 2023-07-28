#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QScreen>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//MainWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////

class MainWidget : public QWidget
{
  Q_OBJECT

public:
  MainWidget();
  void get_events();

private:
  QPushButton* m_button;
  QLineEdit* m_edit;
  QTableWidget* m_table;

private slots:
  void button_feed();
  void cell_selected(int row, int col);
};
