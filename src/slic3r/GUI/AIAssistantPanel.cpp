#include "AIAssistantPanel.hpp"

#include "GUI_App.hpp"
#include "MainFrame.hpp"
#include "slic3r/Utils/Http.hpp"
#include <nlohmann/json.hpp>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/listbox.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <thread>

namespace Slic3r::GUI {

namespace {
wxString config_value(const char* key, const wxString& fallback = {})
{
    const std::string value = wxGetApp().app_config->get("ai_assistant", key);
    return value.empty() ? fallback : wxString::FromUTF8(value);
}

void add_labelled(wxWindow* parent, wxSizer* sizer, const wxString& label, wxWindow* control)
{
    sizer->Add(new wxStaticText(parent, wxID_ANY, label), 0, wxLEFT | wxRIGHT | wxTOP, 8);
    sizer->Add(control, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
}
}

AIAssistantPanel::AIAssistantPanel(MainFrame* owner)
    : wxFrame(owner, wxID_ANY, "Bambu Studio AI", wxDefaultPosition, wxSize(430, 760),
              wxFRAME_TOOL_WINDOW | wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER),
      m_owner(owner)
{
    build_ui();
    load_settings();
    detect_local_ollama();
    Bind(wxEVT_CLOSE_WINDOW, [this](wxCloseEvent& event) {
        save_settings();
        Hide();
        event.Veto();
    });
}

void AIAssistantPanel::toggle()
{
    if (IsShown()) {
        Hide();
        return;
    }
    const wxRect owner = m_owner->GetScreenRect();
    const wxSize size = GetSize();
    SetPosition({owner.GetRight() - size.GetWidth(), owner.GetTop() + 48});
    Show();
    Raise();
    m_prompt->SetFocus();
}

void AIAssistantPanel::build_ui()
{
    auto* root = new wxBoxSizer(wxVERTICAL);
    auto* tabs = new wxNotebook(this, wxID_ANY);

    auto* chat = new wxPanel(tabs);
    auto* chat_sizer = new wxBoxSizer(wxVERTICAL);
    m_transcript = new wxTextCtrl(chat, wxID_ANY,
        "Bambu Studio AI ready. Printer-changing actions always require approval.\n",
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);
    m_attachments = new wxListBox(chat, wxID_ANY);
    auto* attach = new wxButton(chat, wxID_ANY, "Add images, video or documents...");
    m_prompt = new wxTextCtrl(chat, wxID_ANY, {}, wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE | wxTE_PROCESS_ENTER);
    auto* send = new wxButton(chat, wxID_ANY, "Send to AI");
    chat_sizer->Add(m_transcript, 1, wxEXPAND | wxALL, 8);
    chat_sizer->Add(new wxStaticText(chat, wxID_ANY, "Attachments"), 0, wxLEFT | wxRIGHT, 8);
    chat_sizer->Add(m_attachments, 0, wxEXPAND | wxALL, 8);
    chat_sizer->Add(attach, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
    chat_sizer->Add(m_prompt, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
    chat_sizer->Add(send, 0, wxEXPAND | wxALL, 8);
    chat->SetSizer(chat_sizer);

    auto* settings = new wxPanel(tabs);
    auto* settings_sizer = new wxBoxSizer(wxVERTICAL);
    m_provider = new wxChoice(settings, wxID_ANY);
    m_provider->Append("NVIDIA API");
    m_provider->Append("Ollama (local, when available)");
    m_endpoint = new wxTextCtrl(settings, wxID_ANY);
    m_model = new wxTextCtrl(settings, wxID_ANY);
    m_api_key = new wxTextCtrl(settings, wxID_ANY, {}, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    m_email = new wxTextCtrl(settings, wxID_ANY);
    m_calendar = new wxTextCtrl(settings, wxID_ANY);
    m_monitor_print = new wxCheckBox(settings, wxID_ANY, "Watch live printing and report anomalies");
    add_labelled(settings, settings_sizer, "AI provider", m_provider);
    add_labelled(settings, settings_sizer, "OpenAI-compatible endpoint", m_endpoint);
    add_labelled(settings, settings_sizer, "Model", m_model);
    add_labelled(settings, settings_sizer, "NVIDIA API key (session only; never committed)", m_api_key);
    add_labelled(settings, settings_sizer, "Completion notification email", m_email);
    add_labelled(settings, settings_sizer, "Calendar MCP connection name", m_calendar);
    settings_sizer->Add(m_monitor_print, 0, wxALL, 8);
    settings_sizer->Add(new wxStaticText(settings, wxID_ANY,
        "Ollama is selected automatically only after a successful local health check.\n"
        "Email and calendar actions require a separately authorized MCP connector."),
        0, wxEXPAND | wxALL, 8);
    auto* save = new wxButton(settings, wxID_ANY, "Save AI settings");
    settings_sizer->AddStretchSpacer();
    settings_sizer->Add(save, 0, wxEXPAND | wxALL, 8);
    settings->SetSizer(settings_sizer);

    auto* sharing = new wxPanel(tabs);
    auto* sharing_sizer = new wxBoxSizer(wxVERTICAL);
    m_share_email = new wxTextCtrl(sharing, wxID_ANY);
    m_share_role = new wxChoice(sharing, wxID_ANY);
    m_share_role->Append("View status only");
    m_share_role->Append("Start approved prints");
    m_share_role->Append("Manage slicer settings");
    m_share_role->Append("Full printer control");
    m_share_role->SetSelection(0);
    m_shares = new wxListBox(sharing, wxID_ANY);
    auto* add_share = new wxButton(sharing, wxID_ANY, "Add permission");
    add_labelled(sharing, sharing_sizer, "Person's email", m_share_email);
    add_labelled(sharing, sharing_sizer, "Permission", m_share_role);
    sharing_sizer->Add(add_share, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 8);
    sharing_sizer->Add(m_shares, 1, wxEXPAND | wxALL, 8);
    sharing_sizer->Add(new wxStaticText(sharing, wxID_ANY,
        "Sharing is deny-by-default. A remote service must authenticate invitations\n"
        "before these local rules can grant access."), 0, wxALL, 8);
    sharing->SetSizer(sharing_sizer);

    tabs->AddPage(chat, "Assistant");
    tabs->AddPage(settings, "AI settings");
    tabs->AddPage(sharing, "Sharing");
    root->Add(tabs, 1, wxEXPAND);
    SetSizer(root);

    attach->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { add_attachments(); });
    send->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { submit_prompt(); });
    save->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { save_settings(); });
    add_share->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { add_share_rule(); });
}

void AIAssistantPanel::load_settings()
{
    m_provider->SetSelection(config_value("provider", "nvidia") == "ollama" ? 1 : 0);
    m_endpoint->SetValue(config_value("endpoint", "https://integrate.api.nvidia.com/v1"));
    m_model->SetValue(config_value("model", "nvidia/nemotron-3-super-120b-a12b"));
    m_email->SetValue(config_value("notification_email"));
    m_calendar->SetValue(config_value("calendar_connection"));
    m_monitor_print->SetValue(config_value("monitor_print", "false") == "true");
}

void AIAssistantPanel::save_settings()
{
    auto* config = wxGetApp().app_config;
    config->set("ai_assistant", "provider", m_provider->GetSelection() == 1 ? "ollama" : "nvidia");
    config->set("ai_assistant", "endpoint", m_endpoint->GetValue().ToStdString());
    config->set("ai_assistant", "model", m_model->GetValue().ToStdString());
    config->set("ai_assistant", "notification_email", m_email->GetValue().ToStdString());
    config->set("ai_assistant", "calendar_connection", m_calendar->GetValue().ToStdString());
    config->set("ai_assistant", "monitor_print", m_monitor_print->GetValue() ? "true" : "false");
    config->save();
    append_message("System", "AI settings saved. The API key remains in memory for this session only.");
}

void AIAssistantPanel::add_attachments()
{
    wxFileDialog dialog(this, "Attach files", {}, {}, "All files (*.*)|*.*", wxFD_OPEN | wxFD_MULTIPLE);
    if (dialog.ShowModal() != wxID_OK)
        return;
    wxArrayString paths;
    dialog.GetPaths(paths);
    for (const auto& path : paths) {
        m_attachment_paths.push_back(path);
        m_attachments->Append(wxFileName(path).GetFullName());
    }
}

void AIAssistantPanel::add_share_rule()
{
    const wxString email = m_share_email->GetValue().Strip(wxString::both);
    if (!email.Contains("@")) {
        wxMessageBox("Enter a valid email address.", "Sharing", wxOK | wxICON_WARNING, this);
        return;
    }
    m_shares->Append(email + " — " + m_share_role->GetStringSelection());
    m_share_email->Clear();
}

void AIAssistantPanel::submit_prompt()
{
    const wxString prompt = m_prompt->GetValue().Strip(wxString::both);
    if (prompt.empty())
        return;
    append_message("You", prompt);
    m_prompt->Clear();
    append_message("System", "Contacting the configured provider...");
    send_provider_request(prompt);
}

void AIAssistantPanel::detect_local_ollama()
{
    std::thread([this]() {
        bool available = false;
        Slic3r::Http::get("http://127.0.0.1:11434/api/tags")
            .timeout_max(2)
            .on_complete([&available](std::string, unsigned status) { available = status >= 200 && status < 300; })
            .on_error([](std::string, std::string, unsigned) {})
            .perform_sync();
        if (available) {
            wxGetApp().CallAfter([this]() {
                m_provider->SetSelection(1);
                m_endpoint->SetValue("http://127.0.0.1:11434");
                if (m_model->GetValue().StartsWith("nvidia/"))
                    m_model->SetValue("llama3.2");
                append_message("System", "Local Ollama detected and selected automatically.");
            });
        }
    }).detach();
}

void AIAssistantPanel::send_provider_request(const wxString& prompt)
{
    const bool ollama = m_provider->GetSelection() == 1;
    const std::string endpoint = m_endpoint->GetValue().ToStdString();
    const std::string model = m_model->GetValue().ToStdString();
    const std::string key = m_api_key->GetValue().ToStdString();
    std::string enriched = prompt.ToStdString();
    if (!m_attachment_paths.empty()) {
        enriched += "\nAttached local files (request access before reading):";
        for (const auto& path : m_attachment_paths)
            enriched += "\n- " + path.ToStdString();
    }

    std::thread([this, ollama, endpoint, model, key, enriched]() {
        nlohmann::json body;
        std::string url;
        if (ollama) {
            url = endpoint + "/api/chat";
            body = {{"model", model}, {"stream", false},
                    {"messages", {{{"role", "system"}, {"content", "You are the Bambu Studio assistant. Never claim a printer action ran; propose allow-listed actions for explicit approval."}},
                                  {{"role", "user"}, {"content", enriched}}}}};
        } else {
            url = endpoint + "/chat/completions";
            body = {{"model", model}, {"stream", false},
                    {"messages", {{{"role", "system"}, {"content", "You are the Bambu Studio assistant. Never claim a printer action ran; propose allow-listed actions for explicit approval."}},
                                  {{"role", "user"}, {"content", enriched}}}}};
        }

        std::string response;
        std::string failure;
        Slic3r::Http http = Slic3r::Http::post(url);
        http.header("Content-Type", "application/json").timeout_max(90).set_post_body(body.dump());
        if (!ollama && !key.empty())
            http.header("Authorization", "Bearer " + key);
        http.on_complete([&response](std::string data, unsigned) { response = std::move(data); })
            .on_error([&failure](std::string data, std::string error, unsigned status) {
                failure = error + " (HTTP " + std::to_string(status) + ") " + data;
            }).perform_sync();

        wxString answer;
        if (!failure.empty()) {
            answer = "Provider error: " + wxString::FromUTF8(failure);
        } else {
            try {
                const auto json = nlohmann::json::parse(response);
                answer = wxString::FromUTF8(ollama
                    ? json.at("message").at("content").get<std::string>()
                    : json.at("choices").at(0).at("message").at("content").get<std::string>());
            } catch (const std::exception& e) {
                answer = "Invalid provider response: " + wxString::FromUTF8(e.what());
            }
        }
        wxGetApp().CallAfter([this, answer]() { append_message("AI", answer); });
    }).detach();
}

void AIAssistantPanel::append_message(const wxString& sender, const wxString& message)
{
    m_transcript->AppendText("\n" + sender + ": " + message + "\n");
}

} // namespace Slic3r::GUI
