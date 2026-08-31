#ifndef slic3r_AIAssistantPanel_hpp_
#define slic3r_AIAssistantPanel_hpp_

#include <wx/frame.h>
#include <wx/string.h>
#include <vector>

class wxCheckBox;
class wxChoice;
class wxListBox;
class wxTextCtrl;

namespace Slic3r::GUI {

class MainFrame;

// A separate tool window keeps the existing slicer layout untouched.  Printer
// mutations are intentionally not executed directly: they are converted into
// allow-listed proposals and require an explicit user approval.
class AIAssistantPanel final : public wxFrame
{
public:
    explicit AIAssistantPanel(MainFrame* owner);
    void toggle();

private:
    void build_ui();
    void load_settings();
    void save_settings();
    void add_attachments();
    void add_share_rule();
    void submit_prompt();
    void detect_local_ollama();
    void send_provider_request(const wxString& prompt);
    void append_message(const wxString& sender, const wxString& message);

    MainFrame*              m_owner {nullptr};
    wxChoice*               m_provider {nullptr};
    wxTextCtrl*             m_endpoint {nullptr};
    wxTextCtrl*             m_model {nullptr};
    wxTextCtrl*             m_api_key {nullptr};
    wxTextCtrl*             m_email {nullptr};
    wxTextCtrl*             m_calendar {nullptr};
    wxTextCtrl*             m_transcript {nullptr};
    wxTextCtrl*             m_prompt {nullptr};
    wxTextCtrl*             m_share_email {nullptr};
    wxChoice*               m_share_role {nullptr};
    wxListBox*              m_attachments {nullptr};
    wxListBox*              m_shares {nullptr};
    wxCheckBox*             m_monitor_print {nullptr};
    std::vector<wxString>   m_attachment_paths;
};

} // namespace Slic3r::GUI

#endif
