#ifndef NOSTRO_WEB_FEED_HH
#define NOSTRO_WEB_FEED_HH

#include "web.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//ContainerFeed
/////////////////////////////////////////////////////////////////////////////////////////////////////

class ContainerFeed : public Wt::WContainerWidget
{
public:
  ContainerFeed();
  Wt::WCheckBox* m_check_raw;
  Wt::WTable* m_table_messages;
  void row_text(const Wt::WString& s);
  void get_follows();
};

#endif