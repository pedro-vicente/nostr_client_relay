// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bv, Herent, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#ifndef HIGH_SCORES_WIDGET_H_
#define HIGH_SCORES_WIDGET_H_

#include <Wt/WContainerWidget.h>

class Session;

class HighScoresWidget : public Wt::WContainerWidget
{
public:
  explicit HighScoresWidget(Session *session);
  void update();

private:
  Session *session_ = nullptr;
};

#endif //HIGH_SCORES_WIDGET_H_
