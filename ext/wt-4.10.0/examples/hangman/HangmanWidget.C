/*
 * Copyright (C) 2011 Emweb bv, Herent, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#include "HangmanWidget.h"

#include <Wt/WBreak.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WAny.h>

#include "Session.h"
#include "Dictionary.h"
#include "WordWidget.h"
#include "ImagesWidget.h"
#include "LettersWidget.h"

using namespace Wt;

namespace {
  const int MaxGuesses = 9;
}

HangmanWidget::HangmanWidget(const std::string &name)
  : name_(name)
{
  setContentAlignment(AlignmentFlag::Center);

  title_ = addNew<WText>(tr("hangman.readyToPlay"));

  word_ = addNew<WordWidget>();
  statusText_ = addNew<WText>();
  images_ = addNew<ImagesWidget>(MaxGuesses);

  letters_ = addNew<LettersWidget>();
  letters_->letterPushed().connect(this, &HangmanWidget::registerGuess);

  language_ = addNew<WComboBox>();
  language_->addItem(tr("hangman.englishWords").arg(18957));
  language_->addItem(tr("hangman.dutchWords").arg(1688));

  addNew<WBreak>();

  newGameButton_ = addNew<WPushButton>(tr("hangman.newGame"));
  newGameButton_->clicked().connect(this, &HangmanWidget::newGame);

  letters_->hide();
}

void HangmanWidget::newGame()
{
  WString title(tr("hangman.guessTheWord"));
  title_->setText(title.arg(name_));

  language_->hide();
  newGameButton_->hide();

  /*
   * Choose a new secret word and reset the game
   */
  auto dictionary = static_cast<Dictionary>(language_->currentIndex());
  word_->init(randomWord(dictionary));
  letters_->reset();
  badGuesses_ = 0;
  images_->showImage(badGuesses_);
  statusText_->setText("");
}

void HangmanWidget::registerGuess(char c)
{
  if (badGuesses_ < MaxGuesses) {
    bool correct = word_->guess(c);

    if (!correct) {
      ++badGuesses_;
      images_->showImage(badGuesses_);
    }
  }

  if (badGuesses_ == MaxGuesses) {
    WString status = tr("hangman.youHang");
    statusText_->setText(status.arg(word_->word()));

    letters_->hide();
    language_->show();
    newGameButton_->show();

    scoreUpdated_.emit(-10);
  } else if (word_->won()) {
    statusText_->setText(tr("hangman.youWin"));
    images_->showImage(ImagesWidget::HURRAY);

    letters_->hide();
    language_->show();
    newGameButton_->show();

    scoreUpdated_.emit(20 - badGuesses_);
  }
}
