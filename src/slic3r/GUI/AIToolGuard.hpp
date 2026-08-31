#ifndef slic3r_AIToolGuard_hpp_
#define slic3r_AIToolGuard_hpp_

#include <string>
#include <unordered_set>

namespace Slic3r::GUI {

enum class AIToolRisk { ReadOnly, SlicerMutation, PrinterMutation, Dangerous };

struct AIToolDecision {
    bool allowed {false};
    bool approval_required {true};
    std::string reason;
};

// Central deny-by-default policy. Network model output must pass this guard
// before any adapter is allowed to touch the slicer or a printer.
class AIToolGuard
{
public:
    AIToolDecision evaluate(const std::string& tool_name, bool user_approved) const;
    static AIToolRisk risk_for(const std::string& tool_name);
};

} // namespace Slic3r::GUI
#endif
