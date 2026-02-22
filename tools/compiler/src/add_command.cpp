/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: add_command.cpp                                                      ║
 * ║   الوصف: أمر "ص أضف" لإضافة تبعيات للمشروع                                  ║
 * ║   المهمة: T172 - Phase 16 (US14)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو أمر "ص أضف"؟
 * ──────────────────────────────────────
 *
 * أمر لإضافة مكتبات (تبعيات) لمشروعك!
 *
 * 💻 الاستخدام:
 *   ص أضف رسومات              → إضافة أحدث نسخة
 *   ص أضف رسومات@1.2         → إضافة نسخة محددة
 *   ص أضف رسومات --تطوير     → تبعية تطوير فقط
 *
 * 📝 النتيجة في ص.toml:
 *   [المتطلبات]
 *   رسومات = "^1.2"
 *
 * 🔗 مصادر الحزم:
 *   • سجل سعد الرسمي (افتراضي)
 *   • رابط Git مباشر
 *   • مسار محلي
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <optional>

namespace fs = std::filesystem;

namespace sad {
namespace tools {

// ═══════════════════════════════════════════════════════════════════════════════
//                        مصادر الحزم | Package Sources
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 PackageSource - مصدر الحزمة
 */
enum class PackageSource {
    REGISTRY,   // سجل سعد الرسمي
    GIT,        // مستودع Git
    LOCAL,      // مسار محلي
    URL         // رابط مباشر
};

/**
 * 📋 DependencySpec - مواصفات التبعية
 */
struct DependencySpec {
    std::string name;           // اسم الحزمة
    std::string version;        // النسخة المطلوبة
    PackageSource source;       // المصدر
    std::string sourceUrl;      // رابط المصدر (إن وجد)
    bool isDev;                 // تبعية تطوير؟
    bool isOptional;            // اختياري؟
    std::vector<std::string> features;  // الميزات المطلوبة
    
    DependencySpec()
        : source(PackageSource::REGISTRY)
        , isDev(false)
        , isOptional(false)
    {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        محلل التبعية | Dependency Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔍 DependencyParser - تحليل مواصفات التبعية
 */
class DependencyParser {
public:
    /**
     * 📖 تحليل سلسلة التبعية
     * الصيغ المدعومة:
     *   - اسم
     *   - اسم@نسخة
     *   - اسم@^نسخة
     *   - git:رابط
     *   - مسار:./محلي
     */
    static std::optional<DependencySpec> parse(const std::string& input) {
        DependencySpec spec;
        
        if (input.empty()) {
            return std::nullopt;
        }
        
        // التحقق من مصدر Git
        if (input.starts_with("git:") || input.starts_with("جيت:")) {
            spec.source = PackageSource::GIT;
            spec.sourceUrl = input.substr(input.find(':') + 1);
            spec.name = extractNameFromGitUrl(spec.sourceUrl);
            spec.version = "*";
            return spec;
        }
        
        // التحقق من مسار محلي
        if (input.starts_with("مسار:") || input.starts_with("path:")) {
            spec.source = PackageSource::LOCAL;
            spec.sourceUrl = input.substr(input.find(':') + 1);
            spec.name = fs::path(spec.sourceUrl).stem().string();
            spec.version = "*";
            return spec;
        }
        
        // صيغة سجل: اسم[@نسخة]
        size_t atPos = input.find('@');
        if (atPos != std::string::npos) {
            spec.name = input.substr(0, atPos);
            spec.version = input.substr(atPos + 1);
        } else {
            spec.name = input;
            spec.version = "*";  // أحدث نسخة
        }
        
        // التحقق من صحة النسخة
        if (!spec.version.empty() && spec.version != "*") {
            if (!isValidVersionSpec(spec.version)) {
                return std::nullopt;
            }
        }
        
        spec.source = PackageSource::REGISTRY;
        return spec;
    }
    
    /**
     * ✅ التحقق من صحة مواصفة النسخة
     */
    static bool isValidVersionSpec(const std::string& version) {
        if (version.empty() || version == "*") return true;
        
        // إزالة البادئات
        std::string v = version;
        if (v[0] == '^' || v[0] == '~' || v[0] == '>' || v[0] == '<' || v[0] == '=') {
            size_t start = 1;
            if (v.length() > 1 && v[1] == '=') start = 2;
            v = v.substr(start);
        }
        
        // التحقق من الصيغة
        static const std::regex versionRegex(
            R"(^(0|[1-9]\d*)(?:\.(0|[1-9]\d*))?(?:\.(0|[1-9]\d*))?)"
            R"((?:-([\w.-]+))?(?:\+([\w.-]+))?$)"
        );
        
        return std::regex_match(v, versionRegex);
    }
    
private:
    static std::string extractNameFromGitUrl(const std::string& url) {
        // استخراج اسم المستودع من الرابط
        fs::path p(url);
        std::string name = p.stem().string();
        
        // إزالة .git إن وجد
        if (name.ends_with(".git")) {
            name = name.substr(0, name.length() - 4);
        }
        
        return name;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        محرر TOML | TOML Editor
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📝 TomlEditor - تحرير ملف ص.toml
 */
class TomlEditor {
public:
    /**
     * 📄 قراءة الملف
     */
    bool load(const fs::path& path) {
        filePath_ = path;
        
        if (!fs::exists(path)) {
            return false;
        }
        
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        content_ = buffer.str();
        
        return true;
    }
    
    /**
     * 💾 حفظ الملف
     */
    bool save() {
        std::ofstream file(filePath_);
        if (!file) return false;
        
        file << content_;
        return true;
    }
    
    /**
     * ➕ إضافة تبعية
     */
    bool addDependency(const DependencySpec& dep) {
        std::string section = dep.isDev ? "[التطوير]" : "[المتطلبات]";
        std::string sectionEn = dep.isDev ? "[dev-dependencies]" : "[dependencies]";
        
        // البحث عن القسم
        size_t sectionPos = content_.find(section);
        if (sectionPos == std::string::npos) {
            sectionPos = content_.find(sectionEn);
        }
        
        if (sectionPos == std::string::npos) {
            // إضافة القسم في النهاية
            content_ += "\n" + section + "\n";
            sectionPos = content_.length();
        } else {
            // البحث عن نهاية القسم
            sectionPos = findSectionEnd(sectionPos);
        }
        
        // تنسيق قيمة التبعية
        std::string depValue = formatDependencyValue(dep);
        std::string line = dep.name + " = " + depValue + "\n";
        
        // التحقق من عدم وجود التبعية مسبقاً
        if (hasDependency(dep.name, dep.isDev)) {
            // تحديث التبعية الموجودة
            return updateDependency(dep);
        }
        
        // إضافة السطر
        content_.insert(sectionPos, line);
        
        return true;
    }
    
    /**
     * ➖ إزالة تبعية
     */
    bool removeDependency(const std::string& name, bool isDev) {
        std::string section = isDev ? "[التطوير]" : "[المتطلبات]";
        
        // البحث عن التبعية
        std::regex depRegex(name + R"(\s*=\s*.+\n?)");
        
        size_t sectionPos = content_.find(section);
        if (sectionPos == std::string::npos) {
            return false;
        }
        
        size_t sectionEnd = findNextSection(sectionPos + section.length());
        std::string sectionContent = content_.substr(sectionPos, sectionEnd - sectionPos);
        
        std::string newSection = std::regex_replace(sectionContent, depRegex, "");
        
        content_.replace(sectionPos, sectionEnd - sectionPos, newSection);
        
        return true;
    }
    
    /**
     * 🔍 التحقق من وجود تبعية
     */
    bool hasDependency(const std::string& name, bool isDev) {
        std::string section = isDev ? "[التطوير]" : "[المتطلبات]";
        
        size_t sectionPos = content_.find(section);
        if (sectionPos == std::string::npos) {
            return false;
        }
        
        size_t sectionEnd = findNextSection(sectionPos + section.length());
        std::string sectionContent = content_.substr(sectionPos, sectionEnd - sectionPos);
        
        return sectionContent.find(name) != std::string::npos;
    }
    
private:
    fs::path filePath_;
    std::string content_;
    
    size_t findSectionEnd(size_t start) {
        // البحث عن نهاية القسم (بداية القسم التالي أو نهاية الملف)
        size_t nextSection = findNextSection(start + 1);
        
        // إيجاد آخر سطر غير فارغ قبل القسم التالي
        size_t pos = nextSection;
        while (pos > start && (content_[pos - 1] == '\n' || content_[pos - 1] == ' ')) {
            pos--;
        }
        
        return pos + 1; // بعد السطر الجديد
    }
    
    size_t findNextSection(size_t start) {
        size_t pos = start;
        while (pos < content_.length()) {
            if (content_[pos] == '[' && (pos == 0 || content_[pos - 1] == '\n')) {
                return pos;
            }
            pos++;
        }
        return content_.length();
    }
    
    std::string formatDependencyValue(const DependencySpec& dep) {
        if (dep.source == PackageSource::GIT) {
            return "{ جيت = \"" + dep.sourceUrl + "\" }";
        }
        
        if (dep.source == PackageSource::LOCAL) {
            return "{ مسار = \"" + dep.sourceUrl + "\" }";
        }
        
        // قيمة بسيطة للسجل
        std::string version = dep.version;
        if (version == "*") {
            version = "^0.1";  // افتراضي
        } else if (version[0] != '^' && version[0] != '~' && 
                   version[0] != '>' && version[0] != '<' && version[0] != '=') {
            version = "^" + version;  // إضافة ^ افتراضياً
        }
        
        if (!dep.features.empty()) {
            std::string features;
            for (size_t i = 0; i < dep.features.size(); ++i) {
                if (i > 0) features += ", ";
                features += "\"" + dep.features[i] + "\"";
            }
            return "{ نسخة = \"" + version + "\", ميزات = [" + features + "] }";
        }
        
        return "\"" + version + "\"";
    }
    
    bool updateDependency(const DependencySpec& dep) {
        std::string section = dep.isDev ? "[التطوير]" : "[المتطلبات]";
        
        std::regex depRegex("(" + dep.name + R"(\s*=\s*))[^\n]+)");
        std::string replacement = "$1" + formatDependencyValue(dep);
        
        content_ = std::regex_replace(content_, depRegex, replacement);
        
        return true;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    سجل الحزم | Package Registry
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 PackageInfo - معلومات الحزمة من السجل
 */
struct PackageInfo {
    std::string name;
    std::string latestVersion;
    std::string description;
    std::vector<std::string> versions;
};

/**
 * 🌐 PackageRegistry - واجهة سجل الحزم
 */
class PackageRegistry {
public:
    /**
     * 🔍 البحث عن حزمة
     */
    std::optional<PackageInfo> lookup(const std::string& name) {
        // محاكاة بحث في السجل
        // في التطبيق الحقيقي، هذا سيتصل بخادم السجل
        
        // حزم وهمية للاختبار
        static std::unordered_map<std::string, PackageInfo> mockRegistry = {
            {"رسومات", {"رسومات", "1.5.0", "مكتبة الرسوم ثنائية الأبعاد", {"1.0.0", "1.2.0", "1.5.0"}}},
            {"شبكة", {"شبكة", "2.0.0", "مكتبة الشبكات والاتصال", {"1.0.0", "1.5.0", "2.0.0"}}},
            {"اختبار", {"اختبار", "0.5.0", "إطار اختبار", {"0.1.0", "0.3.0", "0.5.0"}}},
            {"json", {"json", "1.0.0", "محلل JSON", {"0.9.0", "1.0.0"}}},
            {"بيانات", {"بيانات", "3.0.0", "هياكل البيانات", {"1.0.0", "2.0.0", "3.0.0"}}}
        };
        
        auto it = mockRegistry.find(name);
        if (it != mockRegistry.end()) {
            return it->second;
        }
        
        return std::nullopt;
    }
    
    /**
     * 📋 قائمة النسخ المتاحة
     */
    std::vector<std::string> listVersions(const std::string& name) {
        auto info = lookup(name);
        if (info) {
            return info->versions;
        }
        return {};
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        منفذ الأمر | Command Executor
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🚀 AddCommandExecutor - منفذ أمر أضف
 */
class AddCommandExecutor {
public:
    struct Options {
        std::vector<std::string> packages;
        bool isDev;
        bool isOptional;
        bool dryRun;
        bool verbose;
        std::vector<std::string> features;
        
        Options() : isDev(false), isOptional(false), dryRun(false), verbose(false) {}
    };
    
    /**
     * 🎯 تنفيذ الأمر
     */
    int execute(const Options& options) {
        // البحث عن ص.toml
        fs::path tomlPath = findPackageToml();
        if (tomlPath.empty()) {
            printError("لم يُعثر على ملف ص.toml");
            printInfo("تأكد من أنك في مجلد مشروع سعد");
            return 1;
        }
        
        // تحميل الملف
        TomlEditor editor;
        if (!editor.load(tomlPath)) {
            printError("فشل قراءة ص.toml");
            return 1;
        }
        
        // معالجة كل حزمة
        int errors = 0;
        for (const auto& pkg : options.packages) {
            auto spec = DependencyParser::parse(pkg);
            if (!spec) {
                printError("صيغة غير صالحة: " + pkg);
                errors++;
                continue;
            }
            
            spec->isDev = options.isDev;
            spec->isOptional = options.isOptional;
            spec->features = options.features;
            
            // البحث في السجل لتأكيد وجود الحزمة
            if (spec->source == PackageSource::REGISTRY) {
                auto info = registry_.lookup(spec->name);
                if (!info) {
                    printError("الحزمة غير موجودة: " + spec->name);
                    errors++;
                    continue;
                }
                
                // استخدام أحدث نسخة إذا لم تُحدد
                if (spec->version == "*") {
                    spec->version = "^" + info->latestVersion;
                }
                
                if (options.verbose) {
                    printInfo("وُجدت: " + spec->name + " (" + info->latestVersion + ")");
                }
            }
            
            if (options.dryRun) {
                std::cout << "  [محاكاة] سيُضاف: " << spec->name << " = \"" << spec->version << "\"\n";
                continue;
            }
            
            // إضافة التبعية
            if (editor.addDependency(*spec)) {
                printSuccess(spec->name, spec->version, spec->isDev);
            } else {
                printError("فشل إضافة: " + spec->name);
                errors++;
            }
        }
        
        // حفظ الملف
        if (!options.dryRun && errors < options.packages.size()) {
            if (!editor.save()) {
                printError("فشل حفظ ص.toml");
                return 1;
            }
        }
        
        return errors > 0 ? 1 : 0;
    }
    
private:
    PackageRegistry registry_;
    
    fs::path findPackageToml() {
        fs::path current = fs::current_path();
        
        while (!current.empty()) {
            fs::path toml = current / "ص.toml";
            if (fs::exists(toml)) {
                return toml;
            }
            
            // جرب الاسم الإنجليزي
            toml = current / "sad.toml";
            if (fs::exists(toml)) {
                return toml;
            }
            
            if (current.parent_path() == current) {
                break;
            }
            current = current.parent_path();
        }
        
        return fs::path();
    }
    
    void printError(const std::string& msg) {
        std::cerr << "❌ " << msg << "\n";
    }
    
    void printInfo(const std::string& msg) {
        std::cout << "ℹ️  " << msg << "\n";
    }
    
    void printSuccess(const std::string& name, const std::string& version, bool isDev) {
        std::cout << "✅ أُضيفت: " << name << " (" << version << ")";
        if (isDev) std::cout << " [تطوير]";
        std::cout << "\n";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {
    int sad_add_package(
        const char** packages,
        int packageCount,
        int isDev,
        int dryRun
    ) {
        AddCommandExecutor::Options options;
        
        for (int i = 0; i < packageCount; ++i) {
            if (packages[i]) {
                options.packages.push_back(packages[i]);
            }
        }
        
        options.isDev = isDev != 0;
        options.dryRun = dryRun != 0;
        
        AddCommandExecutor executor;
        return executor.execute(options);
    }
    
    int sad_remove_package(const char* name, int isDev) {
        fs::path tomlPath = fs::current_path() / "ص.toml";
        if (!fs::exists(tomlPath)) {
            tomlPath = fs::current_path() / "sad.toml";
        }
        
        if (!fs::exists(tomlPath)) {
            return 1;
        }
        
        TomlEditor editor;
        if (!editor.load(tomlPath)) {
            return 1;
        }
        
        if (!editor.removeDependency(name ? name : "", isDev != 0)) {
            return 1;
        }
        
        return editor.save() ? 0 : 1;
    }
    
    int sad_has_package(const char* name, int isDev) {
        fs::path tomlPath = fs::current_path() / "ص.toml";
        if (!fs::exists(tomlPath)) {
            tomlPath = fs::current_path() / "sad.toml";
        }
        
        if (!fs::exists(tomlPath)) {
            return 0;
        }
        
        TomlEditor editor;
        if (!editor.load(tomlPath)) {
            return 0;
        }
        
        return editor.hasDependency(name ? name : "", isDev != 0) ? 1 : 0;
    }
}

} // namespace tools
} // namespace sad
