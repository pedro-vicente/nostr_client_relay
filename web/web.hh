// This file is part of 'Nostr_client_relay' 
// Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
// See file LICENSE for full license details.

#ifndef NOSTRO_WEB_HH
#define NOSTRO_WEB_HH

#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WServer.h>
#include <Wt/WTextArea.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WCheckBox.h>
#include <Wt/WGroupBox.h>
#include <Wt/WTable.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WMenu.h>
#include <Wt/WAnchor.h>
#include <Wt/WLink.h>

#include <future>
#include <fstream> 
#include <vector>
#include <string>
#include "client_wss.hpp"
#include "nostri.h"
#include "uuid.hh"
#include "log.hh"
#include "nostr.hh"
#include "database.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//NostroApplication
/////////////////////////////////////////////////////////////////////////////////////////////////////

class NostroApplication : public Wt::WApplication
{
public:
  NostroApplication(const Wt::WEnvironment& env);

  Wt::WMenu* m_menu;
  Wt::WHBoxLayout* m_layout;

  void selected();
};

#endif