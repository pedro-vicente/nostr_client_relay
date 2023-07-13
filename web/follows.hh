#ifndef WOSTRO_FOLLOWS_HH
#define WOSTRO_FOLLOWS_HH

#include "web.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFollows
/////////////////////////////////////////////////////////////////////////////////////////////////////

class ContainerFollows : public Wt::WContainerWidget
{
public:
  ContainerFollows();
  Wt::WCheckBox* m_check_raw;
  Wt::WTable* m_table_messages;
  void row_text(const Wt::WString& s);
  void get_follows();
};

#endif