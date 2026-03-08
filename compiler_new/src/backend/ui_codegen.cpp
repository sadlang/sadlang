// ui_codegen.cpp — توليد واجهات أصلية للموبايل
#include <string>
#include <vector>
#include <sstream>

struct UiComponent {
    std::string name;
    std::string type; // "button", "text", "input", "column", "row", "image", "list"
    std::vector<std::pair<std::string, std::string>> properties;
    std::vector<UiComponent> children;
};

class UiCodegen {
public:
    virtual ~UiCodegen() = default;
    virtual std::string generate(const UiComponent& root) = 0;
    virtual std::string get_target_name() const = 0;
};

// Base implementation with shared logic
std::string indent(int level) {
    return std::string(level * 2, ' ');
}

std::string get_property(const UiComponent& comp, const std::string& key, const std::string& default_val = "") {
    for (const auto& [k, v] : comp.properties) {
        if (k == key) return v;
    }
    return default_val;
}

// Factory function
extern "C" {
    const char* ui_codegen_generate(const char* ast_json, const char* target) {
        // (AR) thread_local لأمان الخيوط / (EN) thread_local for thread safety
        thread_local std::string result;
        std::string tgt(target);
        // Parse JSON AST → UiComponent tree
        // Generate code for target platform
        result = "// Generated for " + tgt;
        return result.c_str();
    }
}
