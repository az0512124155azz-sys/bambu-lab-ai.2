#ifndef slic3r_AIAssistantPanel_hpp_
#define slic3r_AIAssistantPanel_hpp_

#include <wx/frame.h>
#include <wx/string.h>
#include <array>
#include <vector>

class wxButton;
class wxCheckBox;
class wxChoice;
class wxListBox;
class wxNotebook;
class wxStaticText;
class wxTextCtrl;

namespace Slic3r::GUI {

class MainFrame;

class AIAssistantPanel final : public wxFrame
{
public:
    explicit AIAssistantPanel(MainFrame* owner);
    void toggle();
    void show_terminal();

    struct Triangle {
        std::array<double, 3> a;
        std::array<double, 3> b;
        std::array<double, 3> c;
    };

private:

    void build_ui();
    void style_button(wxButton* button, bool primary = false);
    void load_settings();
    void save_settings();
    void add_attachments();
    void add_share_rule();
    void submit_prompt();
    void detect_local_ollama();
    void detect_mcp_config(bool interactive = false);
    void choose_mcp_config();
    void send_provider_request(const wxString& prompt);
    void handle_ai_answer(const wxString& answer);
    void append_message(const wxString& sender, const wxString& message);
    void append_terminal(const wxString& line);
    void run_terminal_input();
    void execute_terminal_command(const wxString& command);
    bool set_slicer_setting(const wxString& key, const wxString& value);
    bool create_model_from_json(const wxString& json_text, const wxString& requested_name = {});
    bool write_and_import_stl(const std::vector<Triangle>& triangles, const wxString& name);

    MainFrame*            m_owner {nullptr};
    wxNotebook*           m_tabs {nullptr};
    wxChoice*             m_provider {nullptr};
    wxTextCtrl*           m_endpoint {nullptr};
    wxTextCtrl*           m_model {nullptr};
    wxTextCtrl*           m_api_key {nullptr};
    wxTextCtrl*           m_email {nullptr};
    wxTextCtrl*           m_calendar {nullptr};
    wxTextCtrl*           m_mcp_config {nullptr};
    wxStaticText*         m_provider_status {nullptr};
    wxStaticText*         m_mcp_status {nullptr};
    wxTextCtrl*           m_transcript {nullptr};
    wxTextCtrl*           m_prompt {nullptr};
    wxTextCtrl*           m_terminal_output {nullptr};
    wxTextCtrl*           m_terminal_input {nullptr};
    wxTextCtrl*           m_share_email {nullptr};
    wxChoice*             m_share_role {nullptr};
    wxListBox*            m_attachments {nullptr};
    wxListBox*            m_shares {nullptr};
    wxCheckBox*           m_monitor_print {nullptr};
    std::vector<wxString> m_attachment_paths;
    std::vector<wxString> m_ollama_models;
    std::vector<wxString> m_mcp_servers;
};

} // namespace Slic3r::GUI

#endif
