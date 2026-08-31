#include "AIToolGuard.hpp"

namespace Slic3r::GUI {

AIToolRisk AIToolGuard::risk_for(const std::string& name)
{
    static const std::unordered_set<std::string> read_only = {
        "get_project_summary", "get_printer_status", "get_slice_settings", "capture_camera_frame"
    };
    static const std::unordered_set<std::string> slicer = {
        "set_slice_setting", "select_filament_colour", "import_model", "slice_plate"
    };
    static const std::unordered_set<std::string> printer = {
        "send_print", "pause_print", "resume_print", "cancel_print", "set_printer_temperature"
    };
    if (read_only.find(name) != read_only.end()) return AIToolRisk::ReadOnly;
    if (slicer.find(name) != slicer.end()) return AIToolRisk::SlicerMutation;
    if (printer.find(name) != printer.end()) return AIToolRisk::PrinterMutation;
    return AIToolRisk::Dangerous;
}

AIToolDecision AIToolGuard::evaluate(const std::string& name, bool approved) const
{
    switch (risk_for(name)) {
    case AIToolRisk::ReadOnly:
        return {true, false, "Read-only action"};
    case AIToolRisk::SlicerMutation:
    case AIToolRisk::PrinterMutation:
        return approved ? AIToolDecision{true, true, "Explicitly approved"}
                        : AIToolDecision{false, true, "User approval required"};
    case AIToolRisk::Dangerous:
        return {false, true, "Unknown or prohibited tool"};
    }
    return {false, true, "Denied by default"};
}

} // namespace Slic3r::GUI
