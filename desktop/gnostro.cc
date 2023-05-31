#include "wx/wxprec.h"
#include "wx/wx.h"
#include "sample.xpm"
#include "wx/splitter.h"
#include <wx/display.h>
#include "wx/stc/stc.h"

#include "client_wss.hpp"
#include <future>
#include "nostri.h"
#include "log.hh"
#include "message.hh"
#include "database.hh"

// nostr developer
// 0b889799eab6851ecd93aca010dbd85cc9aacd51b3569aa02f933b3ad23d6b24

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("gnostro");
std::vector<std::string> store;
wxSize frame_size;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//widget IDs
/////////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
  ID_BUTTON_SEND_EVENT = wxID_HIGHEST + 1,
  ID_TEXT_EVENT
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxPanelInput
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxPanelInput : public wxPanel
{
public:
  wxPanelInput(wxWindow* parent);
  void OnButtonSendEvent(wxCommandEvent& event);

protected:
  wxStyledTextCtrl* m_text_ctrl;
  std::string m_out_message;

  DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//PanelInput
/////////////////////////////////////////////////////////////////////////////////////////////////////

class PanelInput : public wxPanel
{
public:
  PanelInput(wxWindow* parent);

private:
  DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//PanelOutput
/////////////////////////////////////////////////////////////////////////////////////////////////////

class PanelOutput : public wxPanel
{
public:
  PanelOutput(wxWindow* parent);

private:
  DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppNostro
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxAppNostro : public wxApp
{
public:
  virtual bool OnInit() wxOVERRIDE;
};

wxIMPLEMENT_APP(wxAppNostro);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

class wxFrameMain : public wxFrame
{
public:
  wxFrameMain(const wxString& title, wxPoint position, wxSize size);
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);

  wxSplitterWindow* m_splitter;
  wxWindow* m_pane_input, * m_pane_output;

private:
  wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxFrameMain, wxFrame)
EVT_MENU(wxID_EXIT, wxFrameMain::OnQuit)
EVT_MENU(wxID_ABOUT, wxFrameMain::OnAbout)
wxEND_EVENT_TABLE()

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppNostro::OnInit()
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool wxAppNostro::OnInit()
{
  events::start_log();

  if (!wxApp::OnInit())
  {
    return false;
  }
  wxDisplay display;
  wxRect r = display.GetClientArea();
  wxPoint position(100, 100);
  wxSize size;
  size.x = 2 * r.width / 3;
  size.y = 2 * r.height / 3;
  frame_size = size;
  wxFrameMain* frame = new wxFrameMain("Nostro", position, size);
  frame->Show(true);
  frame->Maximize();
  return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::wxFrameMain
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxFrameMain::wxFrameMain(const wxString& title, wxPoint position, wxSize size)
  : wxFrame(NULL, wxID_ANY, title, position, size)
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //setup GUI
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  SetIcon(wxICON(sample));
  wxMenu* menu_file = new wxMenu;
  menu_file->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit this program");
  wxMenu* menu_help = new wxMenu;
  menu_help->Append(wxID_ABOUT, "&About\tF1", "Show about dialog");
  wxMenuBar* menu_bar = new wxMenuBar();
  menu_bar->Append(menu_file, "&File");
  menu_bar->Append(menu_help, "&Help");
  SetMenuBar(menu_bar);
  CreateStatusBar(2);
  SetStatusText("Ready");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //2 panes, input, output
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  m_splitter = new wxSplitterWindow(this);

  // If you use non-zero gravity you must initialize the splitter with its
  // correct initial size, otherwise it will change the sash position by a
  // huge amount when it's resized from its initial default size to its real
  // size when the frame lays it out. This wouldn't be necessary if default
  // zero gravity were used (although it would do no harm either).
  m_splitter->SetSize(GetClientSize());
  m_splitter->SetSashGravity(1.0);
  m_splitter->SetSashInvisible(true);

  m_pane_input = new PanelInput(m_splitter);
  m_pane_input->SetBackgroundColour(wxColour(128, 128, 128));
  m_pane_input->SetToolTip("Input");

  m_pane_output = new PanelOutput(m_splitter);
  m_pane_output->SetBackgroundColour(wxColour(128, 128, 128));
  m_pane_output->SetToolTip("Output");

  m_splitter->SplitVertically(m_pane_input, m_pane_output, 500);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnQuit
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnQuit(wxCommandEvent& WXUNUSED(event))
{
  Close(true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxFrameMain::OnAbout
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnAbout(wxCommandEvent& WXUNUSED(event))
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxPanelInput
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(wxPanelInput, wxPanel)
EVT_BUTTON(ID_BUTTON_SEND_EVENT, wxPanelInput::OnButtonSendEvent)
wxEND_EVENT_TABLE()

wxPanelInput::wxPanelInput(wxWindow* parent) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER)
{
  m_out_message = R"(["REQ", "RAND", {"kinds": [1], "limit": 3}])";

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //populate input
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
  wxSizer* const sizer_row_1 = new wxBoxSizer(wxHORIZONTAL);
  wxSizer* const sizer_row_2 = new wxBoxSizer(wxHORIZONTAL);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // text event input
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxSize size_text = frame_size / 2;
  wxStyledTextCtrl* text;
  text = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, size_text);
  text->StyleClearAll();
  text->SetUseHorizontalScrollBar(false);
  text->SetMarginWidth(1, 30);
  text->SetMarginType(1, wxSTC_MARGIN_NUMBER);
  text->StyleSetSpec(wxSTC_STYLE_LINENUMBER, "fore:#000000,back:#FFFFFF");
  text->SetText(m_out_message);
  text->SetLexer(wxSTC_LEX_JSON);
  text->StyleSetForeground(wxSTC_MSSQL_COMMENT, wxColour(255, 0, 0));
  text->StyleSetForeground(wxSTC_MSSQL_LINE_COMMENT, wxColour(255, 0, 0));
  text->StyleSetForeground(wxSTC_MSSQL_STRING, wxColour(255, 0, 0));
  text->StyleSetForeground(wxSTC_MSSQL_NUMBER, wxColour(255, 0, 0));
  text->StyleSetForeground(wxSTC_MSSQL_OPERATOR, wxColour(255, 0, 0));
  text->StyleSetForeground(wxSTC_MSSQL_IDENTIFIER, wxColour(0, 150, 0));
  text->StyleSetForeground(wxSTC_MSSQL_VARIABLE, wxColour(0, 150, 0));
  text->StyleSetForeground(wxSTC_MSSQL_COLUMN_NAME, wxColour(0, 0, 255));
  text->StyleSetForeground(wxSTC_MSSQL_STATEMENT, wxColour(0, 0, 150));
  text->StyleSetForeground(wxSTC_MSSQL_DATATYPE, wxColour(150, 150, 150));
  text->StyleSetForeground(wxSTC_MSSQL_SYSTABLE, wxColour(150, 150, 150));
  text->StyleSetForeground(wxSTC_MSSQL_GLOBAL_VARIABLE, wxColour(150, 150, 150));
  text->StyleSetForeground(wxSTC_MSSQL_FUNCTION, wxColour(150, 150, 150));
  text->StyleSetForeground(wxSTC_MSSQL_STORED_PROCEDURE, wxColour(150, 150, 150));
  text->StyleSetForeground(wxSTC_MSSQL_DEFAULT_PREF_DATATYPE, wxColour(150, 150, 150));
  text->StyleSetForeground(wxSTC_MSSQL_COLUMN_NAME_2, wxColour(0, 0, 255));
  m_text_ctrl = text;

  sizer_row_2->Add(m_text_ctrl), wxSizerFlags(0).Expand().Border();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // button send event
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxSizer* const sizer_btn_rep = new wxBoxSizer(wxHORIZONTAL);
  wxSize size_button(169, 44);
  size_button *= 2;
  wxButton* b = new wxButton(this,
    ID_BUTTON_SEND_EVENT,
    wxT("Send Event "),
    wxDefaultPosition,
    size_button);
  sizer_btn_rep->Add(b, wxSizerFlags(0).Expand().Border());
  sizer_row_1->Add(sizer_btn_rep);

  //sizers
  sizer->Add(sizer_row_1);
  sizer->Add(sizer_row_2);
  SetSizer(sizer);
  SetClientSize(GetBestSize());
  SetSizeHints(GetSize());

  m_out_message = R"(["REQ", "RAND", {"kinds": [1], "limit": 3}])";
  m_text_ctrl->SetValue(m_out_message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxPanelInput::OnButtonSendEvent
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxPanelInput::OnButtonSendEvent(wxCommandEvent& WXUNUSED(eve))
{
  // wss://relay.snort.social
  // wss://relay.damus.io
  // wss://eden.nostr.land
  // wss://nostr-pub.wellorder.net
  // wss://nos.lol

  // default uri
  std::string uri = "localhost:8080/nostr";

  WssClient client(uri, false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::stringstream ss;
    std::string str = in_message->string();
    ss << "Received: " << "\"" << str << "\"";
    events::log(ss.str());
    store.push_back(str);

    connection->send_close(1000);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //on_open
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_open = [&](std::shared_ptr<WssClient::Connection> connection)
  {
    std::stringstream ss;
    ss << "Opened connection: HTTP " << connection.get()->http_version << " , code " << connection.get()->status_code;
    events::log(ss.str());

    m_text_ctrl->SetValue(m_out_message);

    ss.str(std::string());
    ss.clear();
    ss << "Sending: \"" << m_out_message << "\"";
    events::log(ss.str());

    connection->send(m_out_message);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_close = [](std::shared_ptr<WssClient::Connection>, int status, const std::string&)
  {
    std::stringstream ss;
    ss << "Closed: " << status;
    events::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
    std::stringstream ss;
    ss << "Error: " << ec << " : " << ec.message();
    events::log(ss.str());
  };

  client.start();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // messages received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::stringstream ss;
  ss << "Received " << store.size() << " messages: ";
  events::log(ss.str());

  for (int idx = 0; idx < store.size(); idx++)
  {
    events::log(store.at(idx));
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//PanelInput::PanelInput
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(PanelInput, wxPanel)

wxEND_EVENT_TABLE()

PanelInput::PanelInput(wxWindow* parent) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER)
{
  wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //buttons
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxSizer* const sizer_btn = new wxBoxSizer(wxHORIZONTAL);
  sizer_btn->Add(new wxButton(this, ID_BUTTON_SEND_EVENT, wxT("Send"), wxDefaultPosition, wxDefaultSize), wxSizerFlags(0).Expand().Border());
  sizer->Add(sizer_btn);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //Key
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxStaticBox* static_ids = new wxStaticBox(this, wxID_ANY, wxT("Key"));
  wxSizer* const sizer_ids = new wxBoxSizer(wxHORIZONTAL);
  wxStaticBoxSizer* const sizer_static_ids = new wxStaticBoxSizer(static_ids, wxHORIZONTAL);
  sizer_static_ids->Add(new wxStaticText(this, wxID_ANY, wxEmptyString), wxSizerFlags(0).Expand().Border());
  sizer_static_ids->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(700, 40)), wxSizerFlags(0).Expand().Border());
  sizer_ids->Add(sizer_static_ids);
  sizer->Add(sizer_ids, wxSizerFlags(0).Expand().Border());


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //Content
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxStaticBox* static_cnt = new wxStaticBox(this, wxID_ANY, wxT("Content"));
  wxSizer* const sizer_cnt = new wxBoxSizer(wxHORIZONTAL);
  wxStaticBoxSizer* const sizer_static_cnt = new wxStaticBoxSizer(static_cnt, wxHORIZONTAL);
  sizer_static_cnt->Add(new wxStaticText(this, wxID_ANY, wxEmptyString), wxSizerFlags(0).Expand().Border());
  sizer_static_cnt->Add(new wxTextCtrl(this, ID_TEXT_EVENT, wxEmptyString, wxDefaultPosition, wxSize(1000, 700)), wxSizerFlags(0).Expand().Border());
  sizer_cnt->Add(sizer_static_cnt);
  sizer->Add(sizer_cnt, wxSizerFlags(0).Expand().Border());



  SetSizerAndFit(sizer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//PanelOutput::PanelOutput
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(PanelOutput, wxPanel)

wxEND_EVENT_TABLE()

PanelOutput::PanelOutput(wxWindow* parent) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER)
{
  wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
}