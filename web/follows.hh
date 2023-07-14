// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

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