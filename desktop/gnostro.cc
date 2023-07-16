#include "gnostro.hh"
#include "client_wss.hpp"
#include <future>
#include "nostri.h"
#include "log.hh"
#include "database.hh"

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
std::string log_program_name("gnostro");
wxSize frame_size;
const std::string def_pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");
void get_feed(const std::string& pubkey, const std::string& uri, std::vector<std::string>& response);

std::string pubkey;

std::vector<std::string> relays = { "eden.nostr.land",
"nos.lol",
"relay.snort.social",
"relay.damus.io",
"nostr.wine",
};

wxIMPLEMENT_APP(wxAppNostro);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//wxAppNostro::OnInit()
/////////////////////////////////////////////////////////////////////////////////////////////////////

bool wxAppNostro::OnInit()
{
  comm::start_log();

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

wxBEGIN_EVENT_TABLE(wxFrameMain, wxFrame)
EVT_MENU(wxID_EXIT, wxFrameMain::OnQuit)
EVT_MENU(wxID_ABOUT, wxFrameMain::OnAbout)
EVT_BUTTON(ID_BUTTON_SEND_EVENT, wxFrameMain::OnButtonSendEvent)
EVT_BUTTON(ID_BUTTON_GET_FEED, wxFrameMain::OnButtonGetFeed)
wxEND_EVENT_TABLE()

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
  m_pane_input->SetToolTip("Input");

  //empty panel
  wxPanel* panel = new wxPanel(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER);

  m_splitter->SplitVertically(m_pane_input, panel, 300);

#if defined DEV_MODE
  m_pane_input->SetBackgroundColour(wxColour(128, 128, 128));
  m_pane_output->SetBackgroundColour(wxColour(128, 128, 128));
#endif
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
//PanelInput::PanelInput
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(PanelInput, wxPanel)
wxEND_EVENT_TABLE()

PanelInput::PanelInput(wxWindow* parent) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER)
{
  wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);

  m_out_message = R"(["REQ", "RAND", {"kinds": [1], "limit": 3}])";

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //buttons
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxSizer* const sizer_btn = new wxBoxSizer(wxHORIZONTAL);
  sizer_btn->Add(new wxButton(this, ID_BUTTON_SEND_EVENT, wxT("Send"), wxDefaultPosition, wxDefaultSize), wxSizerFlags(0).Expand().Border());
  sizer_btn->Add(new wxButton(this, ID_BUTTON_GET_FEED, wxT("Get Feed"), wxDefaultPosition, wxDefaultSize), wxSizerFlags(0).Expand().Border());
  sizer->Add(sizer_btn);


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Relay
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxStaticBox* static_rel = new wxStaticBox(this, wxID_ANY, wxT("Relay"));
  wxSizer* const sizer_rel = new wxBoxSizer(wxHORIZONTAL);
  wxStaticBoxSizer* const sizer_static_rel = new wxStaticBoxSizer(static_rel, wxHORIZONTAL);
  sizer_static_rel->Add(new wxStaticText(this, wxID_ANY, wxEmptyString), wxSizerFlags(0).Expand().Border());

  m_text_relay = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(700, 40));
  m_text_relay->SetValue(relays.at(1));

  sizer_static_rel->Add(m_text_relay, wxSizerFlags(0).Expand().Border());
  sizer_rel->Add(sizer_static_rel);
  sizer->Add(sizer_rel, wxSizerFlags(0).Expand().Border());

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //Key
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxStaticBox* static_ids = new wxStaticBox(this, wxID_ANY, wxT("Pub Key"));
  wxSizer* const sizer_ids = new wxBoxSizer(wxHORIZONTAL);
  wxStaticBoxSizer* const sizer_static_ids = new wxStaticBoxSizer(static_ids, wxHORIZONTAL);
  sizer_static_ids->Add(new wxStaticText(this, wxID_ANY, wxEmptyString), wxSizerFlags(0).Expand().Border());

  m_text_key = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(700, 40));
  m_text_key->SetValue(def_pubkey);

  sizer_static_ids->Add(m_text_key, wxSizerFlags(0).Expand().Border());
  sizer_ids->Add(sizer_static_ids);
  sizer->Add(sizer_ids, wxSizerFlags(0).Expand().Border());

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

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //Content
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  wxStaticBox* static_cnt = new wxStaticBox(this, wxID_ANY, wxT("Message"));
  wxSizer* const sizer_cnt = new wxBoxSizer(wxHORIZONTAL);
  wxStaticBoxSizer* const sizer_static_cnt = new wxStaticBoxSizer(static_cnt, wxHORIZONTAL); 
  sizer_static_cnt->Add(new wxStaticText(this, wxID_ANY, wxEmptyString), wxSizerFlags(0).Expand().Border());
  sizer_static_cnt->Add(m_text_ctrl);
  sizer_cnt->Add(sizer_static_cnt);
  sizer->Add(sizer_cnt, wxSizerFlags(0).Expand().Border());

  SetSizerAndFit(sizer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxFrameMain::OnButtonSendEvent
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnButtonGetFeed(wxCommandEvent& event)
{
  PanelInput* panel = m_pane_input;
  std::string message = panel->m_text_ctrl->GetText().ToStdString();
  std::string uri = panel->m_text_relay->GetValue().ToStdString();
  pubkey = panel->m_text_key->GetValue().ToStdString();

  std::vector<std::string> response;
  get_feed(pubkey, uri, response);
  this->CreateGrid(response);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxFrameMain::OnButtonSendEvent
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::OnButtonSendEvent(wxCommandEvent& WXUNUSED(eve))
{
  PanelInput* panel = m_pane_input;
  std::string message = panel->m_text_ctrl->GetText().ToStdString();
  std::string uri = panel->m_text_relay->GetValue().ToStdString();
  pubkey = panel->m_text_key->GetValue().ToStdString();

  std::vector<std::string> store;

  WssClient client(uri, false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_message
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_message = [&](std::shared_ptr<WssClient::Connection> connection, std::shared_ptr<WssClient::InMessage> in_message)
  {
    std::stringstream ss;
    std::string str = in_message->string();
    ss << "Received: " << "\"" << str << "\"";
    comm::log(ss.str());
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
    comm::log(ss.str());

    ss.str(std::string());
    ss.clear();
    ss << "Sending: \"" << message << "\"";
    comm::log(ss.str());

    connection->send(message);
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_close
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_close = [](std::shared_ptr<WssClient::Connection>, int status, const std::string&)
  {
    std::stringstream ss;
    ss << "Closed: " << status;
    comm::log(ss.str());
  };

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // on_error
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  client.on_error = [](std::shared_ptr<WssClient::Connection>, const SimpleWeb::error_code& ec)
  {
    std::stringstream ss;
    ss << "Error: " << ec << " : " << ec.message();
    comm::log(ss.str());
  };

  client.start();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // messages received
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::stringstream ss;
  ss << "Received " << store.size() << " messages: ";
  comm::log(ss.str());

  for (int idx = 0; idx < store.size(); idx++)
  {
    comm::log(store.at(idx));
  }

  this->CreateGrid(store);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//PanelOutput::CreateGrid
/////////////////////////////////////////////////////////////////////////////////////////////////////

void wxFrameMain::CreateGrid(const std::vector<std::string>& store)
{
  m_pane_output = (PanelOutput*)m_splitter->GetWindow2();
  PanelOutput* panel = new PanelOutput(m_splitter, store);
  m_splitter->ReplaceWindow(m_pane_output, panel);
  m_pane_output->Destroy();
  m_pane_output = panel;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//PanelOutput::PanelOutput
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(PanelOutput, wxPanel)

wxEND_EVENT_TABLE()

PanelOutput::PanelOutput(wxWindow* parent, const std::vector<std::string>& store) :
  wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDOUBLE_BORDER)
{
  wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);

  m_grid = new GridResponse(this);
  size_t nbr_rows = store.size();
  m_grid->CreateGrid(nbr_rows, 1);
  m_grid->SetColLabelValue(0, "Response");
  m_grid->SetColSize(0, 1000);
  m_grid->EnableEditing(false);

  for (int idx = 0; idx < nbr_rows; idx++)
  {
    std::string msg = store.at(idx);
    wxString str(msg);
    wxLogDebug("%s", str.c_str());
    m_grid->SetRowSize(idx, 300);
    m_grid->SetCellValue(idx, 0, str);
  }

  sizer->Add(m_grid, wxSizerFlags(0).Expand().Border());

  SetSizerAndFit(sizer);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GridResponse::GridResponse
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(GridResponse, wxGrid)
EVT_GRID_SELECT_CELL(GridResponse::OnSelectCell)
wxEND_EVENT_TABLE()

GridResponse::GridResponse(wxWindow* parent) :
  wxGrid(parent, wxID_GRID_OUTPUT_PROCESS, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE),
  m_selected_row(0)
{
  AutoSize();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GridResponse::OnSelectCell
/////////////////////////////////////////////////////////////////////////////////////////////////////

void GridResponse::OnSelectCell(wxGridEvent& ev)
{
  m_selected_row = ev.GetRow();
  SelectRow(m_selected_row);
  if (GetNumberRows())
  {
  }
  ev.Skip();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//get_feed
/////////////////////////////////////////////////////////////////////////////////////////////////////

void get_feed(const std::string& pubkey, const std::string& uri, std::vector<std::string>& response)
{

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // get_follows returns an array of pubkeys 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<std::string> pubkeys;
  nostr::get_follows(uri, pubkey, pubkeys);
  comm::to_file("pubkeys.txt", pubkeys);

  for (int idx_key = 0; idx_key < pubkeys.size(); idx_key++)
  {
    std::string pubkey = pubkeys.at(idx_key);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // get feed returns an array of JSON events 
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> events;
    if (nostr::get_feed(uri, pubkey, events) < 0)
    {
    }

    int row = 0;
    for (int idx_eve = 0; idx_eve < events.size(); idx_eve++)
    {
      std::string message = events.at(idx_eve);
      try
      {
        nlohmann::json js = nlohmann::json::parse(message);
        std::string type = js.at(0);
        if (type.compare("EVENT") == 0)
        {
          nostr::event_t ev;
          from_json(js.at(2), ev);
          std::string json = js.dump(1);

          response.push_back(json);

          std::stringstream s;
          s << "follow." << row + 1 << ".json";
          comm::json_to_file(s.str(), json);
          row++;
        }
      }
      catch (const std::exception& e)
      {
        comm::log(e.what());
      }
    } //events
  } //pubkeys

}