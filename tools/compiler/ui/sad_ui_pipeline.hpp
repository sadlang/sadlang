// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: ui/sad_ui_pipeline.hpp
// الوصف: واجهة SadUI Pipeline (ملف مؤقت) / SadUI Pipeline Interface (stub)
// ═══════════════════════════════════════════════════════════════════════════════
// (AR) هذا ملف مؤقت لتمكين بناء sadc. سيُستبدل بالتنفيذ الكامل لاحقاً.
// (EN) This is a stub file to enable sadc builds. Will be replaced with
//      full implementation later.
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>
#include <map>

namespace sad { namespace ui { namespace ir {
struct TargetPlatform {
    static constexpr const char* Android = "android";
    static constexpr const char* IOS = "ios";
    static constexpr const char* Desktop = "desktop";
};
} } }

struct PipelineConfig {
    std::string platform;
    std::string output_dir;
    std::string app_name;
    bool verbose = false;
    bool validate = false;
    bool optimize = false;
    bool applyTheme = false;
};

struct GeneratedFile {
    std::string path;
    std::string content;
};

struct CodeGenResult {
    std::vector<std::string> errors;
    std::vector<GeneratedFile> files;
};

struct PipelineResult {
    bool success = false;
    std::string error_message;
    std::map<std::string, std::string> generated_files;
    std::vector<std::string> log;
    CodeGenResult codeGenResult;
};

class SadUIPipeline {
public:
    void setConfig(const PipelineConfig& config) { config_ = config; }
    
    PipelineResult buildFromSource(const std::string& source, const std::string& appName) {
        PipelineResult result;
        result.success = false;
        result.error_message = "SadUI Pipeline not yet implemented / نظام SadUI غير مُنفّذ بعد";
        result.codeGenResult.errors.push_back(result.error_message);
        return result;
    }
    
private:
    PipelineConfig config_;
};
