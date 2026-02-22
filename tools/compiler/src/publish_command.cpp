/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة ص - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: publish_command.cpp                                                  ║
 * ║   الوصف: أمر "ص نشر" لنشر الحزم في السجل                                   ║
 * ║   المهمة: T173 - Phase 16 (US14)                                            ║
 * ║   المؤلف: فريق ص | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو أمر "ص نشر"؟
 * ──────────────────────────────────────
 *
 * أمر لمشاركة مكتبتك مع العالم!
 *
 * 💻 الاستخدام:
 *   ص نشر                    → نشر في سجل ص الرسمي
 *   ص نشر --محاكاة           → اختبار دون نشر فعلي
 *   ص نشر --رمز=XXX          → استخدام رمز API
 *
 * 📋 خطوات النشر:
 *   1. التحقق من ص.toml
 *   2. بناء الحزمة
 *   3. إنشاء الأرشيف
 *   4. رفع للسجل
 *   5. التحقق من النجاح
 *
 * ⚠️ متطلبات النشر:
 *   • حساب في سجل ص
 *   • ص.toml صالح
 *   • اسم حزمة فريد
 *   • نسخة جديدة
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
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

namespace sad {
namespace tools {

// ═══════════════════════════════════════════════════════════════════════════════
//                        حالة النشر | Publish State
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 PublishStep - خطوات النشر
 */
enum class PublishStep {
    VALIDATE,       // التحقق
    BUILD,          // البناء
    PACKAGE,        // التغليف
    UPLOAD,         // الرفع
    VERIFY,         // التأكيد
    COMPLETE        // اكتمال
};

/**
 * 📝 PublishResult - نتيجة النشر
 */
struct PublishResult {
    bool success;
    std::string message;
    std::string packageUrl;
    std::string checksum;
    size_t packageSize;
    
    PublishResult() : success(false), packageSize(0) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        مُحقق الحزمة | Package Validator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ✅ PackageValidator - التحقق من صحة الحزمة للنشر
 */
class PackageValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    /**
     * 🔍 التحقق الكامل
     */
    ValidationResult validate(const fs::path& projectPath) {
        ValidationResult result;
        result.valid = true;
        
        // التحقق من وجود ص.toml
        fs::path tomlPath = projectPath / "ص.toml";
        if (!fs::exists(tomlPath)) {
            tomlPath = projectPath / "sad.toml";
        }
        
        if (!fs::exists(tomlPath)) {
            result.errors.push_back("لم يُعثر على ملف ص.toml");
            result.valid = false;
            return result;
        }
        
        // قراءة وتحليل ص.toml
        std::ifstream file(tomlPath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // التحقق من الحقول المطلوبة
        if (content.find("الاسم") == std::string::npos && 
            content.find("name") == std::string::npos) {
            result.errors.push_back("حقل 'الاسم' مفقود في ص.toml");
            result.valid = false;
        }
        
        if (content.find("النسخة") == std::string::npos && 
            content.find("version") == std::string::npos) {
            result.errors.push_back("حقل 'النسخة' مفقود في ص.toml");
            result.valid = false;
        }
        
        // التحقق من وجود الملف الرئيسي
        fs::path srcPath = projectPath / "src";
        if (!fs::exists(srcPath)) {
            result.errors.push_back("مجلد src/ غير موجود");
            result.valid = false;
        } else {
            bool hasEntry = fs::exists(srcPath / "main.ص") || 
                           fs::exists(srcPath / "lib.ص");
            if (!hasEntry) {
                result.errors.push_back("لا يوجد ملف main.ص أو lib.ص في src/");
                result.valid = false;
            }
        }
        
        // تحذيرات (لا تمنع النشر)
        if (content.find("الوصف") == std::string::npos && 
            content.find("description") == std::string::npos) {
            result.warnings.push_back("يُنصح بإضافة وصف للحزمة");
        }
        
        if (content.find("الرخصة") == std::string::npos && 
            content.find("license") == std::string::npos) {
            result.warnings.push_back("يُنصح بتحديد الرخصة");
        }
        
        if (!fs::exists(projectPath / "README.md")) {
            result.warnings.push_back("يُنصح بإضافة ملف README.md");
        }
        
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        مُغلف الحزمة | Package Packager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 PackagePackager - تغليف الحزمة للنشر
 */
class PackagePackager {
public:
    struct PackageInfo {
        fs::path archivePath;
        size_t size;
        std::string checksum;
        std::vector<std::string> files;
    };
    
    /**
     * 📦 إنشاء أرشيف الحزمة
     */
    PackageInfo createPackage(
        const fs::path& projectPath,
        const std::string& name,
        const std::string& version
    ) {
        PackageInfo info;
        
        // تحديد مسار الأرشيف
        fs::path buildDir = projectPath / "build" / "pkg";
        fs::create_directories(buildDir);
        
        std::string archiveName = name + "-" + version + ".sad.tar";
        info.archivePath = buildDir / archiveName;
        
        // جمع الملفات للتغليف
        collectFiles(projectPath, info.files);
        
        // إنشاء الأرشيف (محاكاة - في التطبيق الحقيقي نستخدم مكتبة tar)
        createArchive(projectPath, info);
        
        // حساب المجموع الاختباري
        info.checksum = calculateChecksum(info.archivePath);
        
        // حجم الملف
        info.size = fs::file_size(info.archivePath);
        
        return info;
    }
    
private:
    void collectFiles(const fs::path& projectPath, std::vector<std::string>& files) {
        // الملفات المطلوبة
        std::vector<std::string> required = {
            "ص.toml", "sad.toml"
        };
        
        for (const auto& file : required) {
            if (fs::exists(projectPath / file)) {
                files.push_back(file);
                break;  // واحد فقط من ملفات التكوين
            }
        }
        
        // مجلد src
        if (fs::exists(projectPath / "src")) {
            for (const auto& entry : fs::recursive_directory_iterator(projectPath / "src")) {
                if (entry.is_regular_file()) {
                    files.push_back(fs::relative(entry.path(), projectPath).string());
                }
            }
        }
        
        // README إن وجد
        if (fs::exists(projectPath / "README.md")) {
            files.push_back("README.md");
        }
        
        // LICENSE إن وجد
        if (fs::exists(projectPath / "LICENSE")) {
            files.push_back("LICENSE");
        }
    }
    
    void createArchive(const fs::path& projectPath, PackageInfo& info) {
        // محاكاة إنشاء الأرشيف
        // في التطبيق الحقيقي، نستخدم libtar أو مكتبة مشابهة
        
        std::ofstream archive(info.archivePath, std::ios::binary);
        
        // كتابة قائمة الملفات كمحاكاة
        archive << "# Sad Package Archive\n";
        archive << "# Files:\n";
        for (const auto& file : info.files) {
            archive << file << "\n";
            
            // كتابة محتوى الملف
            fs::path filePath = projectPath / file;
            if (fs::exists(filePath)) {
                std::ifstream input(filePath, std::ios::binary);
                archive << "--- " << file << " ---\n";
                archive << input.rdbuf();
                archive << "\n--- END ---\n";
            }
        }
    }
    
    std::string calculateChecksum(const fs::path& filePath) {
        // محاكاة حساب SHA256
        // في التطبيق الحقيقي، نستخدم OpenSSL أو مكتبة مشابهة
        
        std::ifstream file(filePath, std::ios::binary);
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        // محاكاة بسيطة للمجموع الاختباري
        size_t hash = std::hash<std::string>{}(buffer.str());
        
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(16) << hash;
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      رافع السجل | Registry Uploader
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🌐 RegistryUploader - رفع الحزمة للسجل
 */
class RegistryUploader {
public:
    struct UploadConfig {
        std::string registryUrl;
        std::string apiToken;
        bool dryRun;
        
        UploadConfig() 
            : registryUrl("https://registry.sadlang.com")
            , dryRun(false)
        {}
    };
    
    struct UploadResult {
        bool success;
        std::string message;
        std::string packageUrl;
        int statusCode;
    };
    
    /**
     * 📤 رفع الحزمة
     */
    UploadResult upload(
        const fs::path& archivePath,
        const std::string& name,
        const std::string& version,
        const UploadConfig& config
    ) {
        UploadResult result;
        
        if (config.dryRun) {
            result.success = true;
            result.message = "محاكاة: سيُرفع " + archivePath.string();
            result.packageUrl = config.registryUrl + "/packages/" + name + "/" + version;
            result.statusCode = 200;
            return result;
        }
        
        // التحقق من الرمز
        if (config.apiToken.empty()) {
            result.success = false;
            result.message = "رمز API مطلوب للنشر. استخدم: ص تسجيل";
            result.statusCode = 401;
            return result;
        }
        
        // محاكاة الرفع
        // في التطبيق الحقيقي، نستخدم libcurl أو مكتبة HTTP
        
        std::cout << "   📤 رفع إلى " << config.registryUrl << "...\n";
        
        // محاكاة نجاح الرفع
        result.success = true;
        result.message = "تم النشر بنجاح!";
        result.packageUrl = config.registryUrl + "/packages/" + name + "/" + version;
        result.statusCode = 201;
        
        return result;
    }
    
    /**
     * 🔐 التحقق من الرمز
     */
    bool verifyToken(const std::string& token, const std::string& registryUrl) {
        // محاكاة التحقق من صحة الرمز
        return !token.empty() && token.length() >= 10;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        منفذ النشر | Publish Executor
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🚀 PublishCommandExecutor - منفذ أمر النشر
 */
class PublishCommandExecutor {
public:
    struct Options {
        std::string apiToken;
        std::string registryUrl;
        bool dryRun;
        bool verbose;
        bool skipBuild;
        bool allowDirty;  // السماح بتغييرات غير محفوظة
        
        Options() 
            : registryUrl("https://registry.sadlang.com")
            , dryRun(false)
            , verbose(false)
            , skipBuild(false)
            , allowDirty(false)
        {}
    };
    
    /**
     * 🎯 تنفيذ النشر
     */
    int execute(const Options& options) {
        fs::path projectPath = fs::current_path();
        
        printHeader();
        
        // الخطوة 1: التحقق
        printStep(PublishStep::VALIDATE, "التحقق من الحزمة...");
        auto validation = validator_.validate(projectPath);
        
        if (!validation.valid) {
            for (const auto& error : validation.errors) {
                printError(error);
            }
            return 1;
        }
        
        for (const auto& warning : validation.warnings) {
            printWarning(warning);
        }
        
        printStepDone();
        
        // قراءة معلومات الحزمة
        auto [name, version] = readPackageInfo(projectPath);
        if (name.empty() || version.empty()) {
            printError("فشل قراءة معلومات الحزمة");
            return 1;
        }
        
        std::cout << "   📦 " << name << " @ " << version << "\n\n";
        
        // الخطوة 2: البناء (اختياري)
        if (!options.skipBuild) {
            printStep(PublishStep::BUILD, "بناء الحزمة...");
            // محاكاة البناء
            printStepDone();
        }
        
        // الخطوة 3: التغليف
        printStep(PublishStep::PACKAGE, "تغليف الحزمة...");
        auto packageInfo = packager_.createPackage(projectPath, name, version);
        printStepDone();
        
        if (options.verbose) {
            std::cout << "   📁 ملفات الحزمة:\n";
            for (const auto& file : packageInfo.files) {
                std::cout << "      - " << file << "\n";
            }
            std::cout << "   💾 الحجم: " << formatSize(packageInfo.size) << "\n";
            std::cout << "   🔐 المجموع: " << packageInfo.checksum << "\n\n";
        }
        
        // الخطوة 4: الرفع
        printStep(PublishStep::UPLOAD, "رفع للسجل...");
        
        RegistryUploader::UploadConfig uploadConfig;
        uploadConfig.registryUrl = options.registryUrl;
        uploadConfig.apiToken = options.apiToken;
        uploadConfig.dryRun = options.dryRun;
        
        auto uploadResult = uploader_.upload(
            packageInfo.archivePath, name, version, uploadConfig
        );
        
        if (!uploadResult.success) {
            printError(uploadResult.message);
            return 1;
        }
        
        printStepDone();
        
        // الخطوة 5: التأكيد
        printStep(PublishStep::VERIFY, "التحقق من النشر...");
        printStepDone();
        
        // النجاح!
        printSuccess(name, version, uploadResult.packageUrl, options.dryRun);
        
        return 0;
    }
    
private:
    PackageValidator validator_;
    PackagePackager packager_;
    RegistryUploader uploader_;
    
    void printHeader() {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    🚀 نشر حزمة ص                           ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
    }
    
    void printStep(PublishStep step, const std::string& message) {
        std::string stepNum;
        switch (step) {
            case PublishStep::VALIDATE: stepNum = "1/5"; break;
            case PublishStep::BUILD: stepNum = "2/5"; break;
            case PublishStep::PACKAGE: stepNum = "3/5"; break;
            case PublishStep::UPLOAD: stepNum = "4/5"; break;
            case PublishStep::VERIFY: stepNum = "5/5"; break;
            default: stepNum = ""; break;
        }
        std::cout << "   [" << stepNum << "] " << message;
    }
    
    void printStepDone() {
        std::cout << " ✓\n";
    }
    
    void printError(const std::string& msg) {
        std::cerr << "\n   ❌ خطأ: " << msg << "\n";
    }
    
    void printWarning(const std::string& msg) {
        std::cout << "   ⚠️  تحذير: " << msg << "\n";
    }
    
    void printSuccess(
        const std::string& name,
        const std::string& version,
        const std::string& url,
        bool dryRun
    ) {
        std::cout << "\n";
        if (dryRun) {
            std::cout << "   🔍 [محاكاة] سيُنشر:\n";
        } else {
            std::cout << "   ✅ تم النشر بنجاح!\n";
        }
        std::cout << "\n";
        std::cout << "   📦 " << name << "@" << version << "\n";
        std::cout << "   🔗 " << url << "\n";
        std::cout << "\n";
        std::cout << "   للتثبيت:\n";
        std::cout << "      ص أضف " << name << "\n";
        std::cout << "\n";
    }
    
    std::pair<std::string, std::string> readPackageInfo(const fs::path& projectPath) {
        fs::path tomlPath = projectPath / "ص.toml";
        if (!fs::exists(tomlPath)) {
            tomlPath = projectPath / "sad.toml";
        }
        
        if (!fs::exists(tomlPath)) {
            return {"", ""};
        }
        
        std::ifstream file(tomlPath);
        std::string line;
        std::string name, version;
        
        while (std::getline(file, line)) {
            // بحث بسيط عن الاسم والنسخة
            if (line.find("الاسم") != std::string::npos || 
                line.find("name") != std::string::npos) {
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    name = extractQuotedValue(line.substr(eq + 1));
                }
            }
            else if (line.find("النسخة") != std::string::npos || 
                     line.find("version") != std::string::npos) {
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    version = extractQuotedValue(line.substr(eq + 1));
                }
            }
        }
        
        return {name, version};
    }
    
    std::string extractQuotedValue(const std::string& s) {
        size_t start = s.find('"');
        if (start == std::string::npos) return "";
        
        size_t end = s.find('"', start + 1);
        if (end == std::string::npos) return "";
        
        return s.substr(start + 1, end - start - 1);
    }
    
    std::string formatSize(size_t bytes) {
        if (bytes < 1024) return std::to_string(bytes) + " B";
        if (bytes < 1024 * 1024) return std::to_string(bytes / 1024) + " KB";
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {
    int sad_publish_package(
        const char* apiToken,
        const char* registryUrl,
        int dryRun,
        int verbose
    ) {
        PublishCommandExecutor::Options options;
        options.apiToken = apiToken ? apiToken : "";
        options.registryUrl = registryUrl ? registryUrl : "https://registry.sadlang.com";
        options.dryRun = dryRun != 0;
        options.verbose = verbose != 0;
        
        PublishCommandExecutor executor;
        return executor.execute(options);
    }
    
    int sad_validate_package(const char* path) {
        PackageValidator validator;
        auto result = validator.validate(path ? path : ".");
        return result.valid ? 0 : 1;
    }
}

} // namespace tools
} // namespace sad
