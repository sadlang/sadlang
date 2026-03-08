/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: lockfile.cpp                                                         ║
 * ║   الوصف: إدارة ملف القفل (ص.قفل) للتبعيات                                   ║
 * ║   المهمة: T175 - Phase 16 (US14)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو ملف القفل؟
 * ──────────────────────────────────────
 *
 * ملف يحفظ النسخ الدقيقة لجميع التبعيات!
 *
 * 🔒 لماذا نحتاجه؟
 *
 *   بدون قفل:
 *   ص.toml: رسومات = "^1.0"
 *   
 *   يوم الاثنين: تُثبت رسومات@1.2.0 ✓ يعمل
 *   يوم الثلاثاء: تُثبت رسومات@1.3.0 ✗ خطأ!
 *
 *   مع القفل:
 *   ص.قفل: رسومات = "1.2.0" (دائماً!)
 *
 * 📋 محتوى ص.قفل:
 *
 *   [الحزم]
 *   رسومات = { نسخة = "1.2.0", تجزئة = "abc123..." }
 *   شبكة = { نسخة = "2.0.0", تجزئة = "def456..." }
 *
 * 💡 نصائح:
 *   • احفظ ص.قفل في Git
 *   • لا تحرره يدوياً
 *   • أعد توليده بـ: ص تثبيت --تحديث
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <optional>
#include <algorithm>

namespace fs = std::filesystem;

namespace sad {
namespace pkg {

// ═══════════════════════════════════════════════════════════════════════════════
//                        تعريفات ملف القفل | Lockfile Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 LockedPackage - حزمة مقفلة
 */
struct LockedPackage {
    std::string name;           // اسم الحزمة
    std::string version;        // النسخة الدقيقة
    std::string checksum;       // التجزئة (SHA256)
    std::string source;         // المصدر (registry, git, path)
    std::string sourceUrl;      // رابط المصدر
    std::vector<std::string> dependencies;  // تبعيات الحزمة
    std::vector<std::string> features;      // الميزات المفعلة
};

/**
 * 📋 Lockfile - ملف القفل الكامل
 */
struct Lockfile {
    int version;                // نسخة صيغة الملف
    std::string generatedAt;    // تاريخ التوليد
    std::string sadVersion;     // نسخة سعد المستخدمة
    std::unordered_map<std::string, LockedPackage> packages;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        قارئ ملف القفل | Lockfile Reader
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📖 LockfileReader - قراءة ملف القفل
 */
class LockfileReader {
public:
    /**
     * 📄 قراءة ملف القفل
     */
    std::optional<Lockfile> read(const fs::path& path) {
        if (!fs::exists(path)) {
            return std::nullopt;
        }
        
        std::ifstream file(path);
        if (!file) {
            return std::nullopt;
        }
        
        Lockfile lockfile;
        lockfile.version = 1;
        
        std::string line;
        std::string currentSection;
        std::string currentPackage;
        
        while (std::getline(file, line)) {
            // تجاهل الأسطر الفارغة والتعليقات
            if (line.empty() || line[0] == '#') continue;
            
            // التحقق من القسم
            if (line[0] == '[') {
                size_t end = line.find(']');
                if (end != std::string::npos) {
                    currentSection = line.substr(1, end - 1);
                    
                    // التحقق من حزمة محددة
                    if (currentSection.starts_with("حزمة.") || 
                        currentSection.starts_with("package.")) {
                        currentPackage = currentSection.substr(currentSection.find('.') + 1);
                        lockfile.packages[currentPackage] = LockedPackage();
                        lockfile.packages[currentPackage].name = currentPackage;
                    }
                }
                continue;
            }
            
            // تحليل المفتاح والقيمة
            size_t eq = line.find('=');
            if (eq == std::string::npos) continue;
            
            std::string key = trim(line.substr(0, eq));
            std::string value = trim(line.substr(eq + 1));
            
            // إزالة علامات الاقتباس
            if (!value.empty() && value[0] == '"') {
                value = value.substr(1);
                if (!value.empty() && value.back() == '"') {
                    value.pop_back();
                }
            }
            
            // معالجة حسب القسم
            if (currentSection == "البيانات" || currentSection == "metadata") {
                if (key == "النسخة" || key == "version") {
                    lockfile.version = std::stoi(value);
                }
                else if (key == "تاريخ_التوليد" || key == "generated_at") {
                    lockfile.generatedAt = value;
                }
                else if (key == "نسخة_سعد" || key == "sad_version") {
                    lockfile.sadVersion = value;
                }
            }
            else if (!currentPackage.empty()) {
                auto& pkg = lockfile.packages[currentPackage];
                
                if (key == "النسخة" || key == "version") {
                    pkg.version = value;
                }
                else if (key == "التجزئة" || key == "checksum") {
                    pkg.checksum = value;
                }
                else if (key == "المصدر" || key == "source") {
                    pkg.source = value;
                }
                else if (key == "الرابط" || key == "source_url") {
                    pkg.sourceUrl = value;
                }
            }
        }
        
        return lockfile;
    }
    
private:
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t");
        return s.substr(start, end - start + 1);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        كاتب ملف القفل | Lockfile Writer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📝 LockfileWriter - كتابة ملف القفل
 */
class LockfileWriter {
public:
    /**
     * 💾 كتابة ملف القفل
     */
    bool write(const fs::path& path, const Lockfile& lockfile) {
        std::ofstream file(path);
        if (!file) {
            return false;
        }
        
        // رأس الملف
        file << "# ═══════════════════════════════════════════════════════════\n";
        file << "#              ملف القفل - لغة سعد\n";
        file << "#              Lockfile - Sad Language\n";
        file << "# ═══════════════════════════════════════════════════════════\n";
        file << "#\n";
        file << "# تحذير: هذا الملف يُولَّد تلقائياً. لا تحرره يدوياً!\n";
        file << "# Warning: This file is auto-generated. Do not edit manually!\n";
        file << "#\n";
        file << "# ═══════════════════════════════════════════════════════════\n\n";
        
        // البيانات الوصفية
        file << "[البيانات]\n";
        file << "النسخة = " << lockfile.version << "\n";
        file << "تاريخ_التوليد = \"" << getCurrentTimestamp() << "\"\n";
        file << "نسخة_سعد = \"" << (lockfile.sadVersion.empty() ? "0.1.0" : lockfile.sadVersion) << "\"\n";
        file << "\n";
        
        // الحزم المقفلة
        if (!lockfile.packages.empty()) {
            file << "# ═══════════════════════════════════════════════════════════\n";
            file << "#                       الحزم المقفلة\n";
            file << "# ═══════════════════════════════════════════════════════════\n\n";
            
            // ترتيب الحزم أبجدياً
            std::vector<std::string> sortedNames;
            for (const auto& [name, _] : lockfile.packages) {
                sortedNames.push_back(name);
            }
            std::sort(sortedNames.begin(), sortedNames.end());
            
            for (const auto& name : sortedNames) {
                const auto& pkg = lockfile.packages.at(name);
                
                file << "[حزمة." << name << "]\n";
                file << "النسخة = \"" << pkg.version << "\"\n";
                
                if (!pkg.checksum.empty()) {
                    file << "التجزئة = \"" << pkg.checksum << "\"\n";
                }
                
                if (!pkg.source.empty()) {
                    file << "المصدر = \"" << pkg.source << "\"\n";
                }
                
                if (!pkg.sourceUrl.empty()) {
                    file << "الرابط = \"" << pkg.sourceUrl << "\"\n";
                }
                
                if (!pkg.dependencies.empty()) {
                    file << "التبعيات = [";
                    for (size_t i = 0; i < pkg.dependencies.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << pkg.dependencies[i] << "\"";
                    }
                    file << "]\n";
                }
                
                file << "\n";
            }
        }
        
        return true;
    }
    
private:
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%dT%H:%M:%S");
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        مدير ملف القفل | Lockfile Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔐 LockfileManager - إدارة ملف القفل
 */
class LockfileManager {
public:
    /**
     * 📄 تحميل أو إنشاء ملف قفل
     */
    Lockfile loadOrCreate(const fs::path& projectPath) {
        fs::path lockPath = projectPath / "ص.قفل";
        
        if (!fs::exists(lockPath)) {
            lockPath = projectPath / "sad.lock";
        }
        
        LockfileReader reader;
        auto lockfile = reader.read(lockPath);
        
        if (lockfile) {
            return *lockfile;
        }
        
        // إنشاء ملف قفل جديد
        Lockfile newLock;
        newLock.version = 1;
        newLock.sadVersion = "0.1.0";
        return newLock;
    }
    
    /**
     * 💾 حفظ ملف القفل
     */
    bool save(const fs::path& projectPath, const Lockfile& lockfile) {
        fs::path lockPath = projectPath / "ص.قفل";
        
        LockfileWriter writer;
        return writer.write(lockPath, lockfile);
    }
    
    /**
     * ➕ إضافة حزمة للقفل
     */
    void addPackage(Lockfile& lockfile, const LockedPackage& pkg) {
        lockfile.packages[pkg.name] = pkg;
    }
    
    /**
     * ➖ إزالة حزمة من القفل
     */
    void removePackage(Lockfile& lockfile, const std::string& name) {
        lockfile.packages.erase(name);
    }
    
    /**
     * 🔍 البحث عن حزمة
     */
    const LockedPackage* getPackage(const Lockfile& lockfile, const std::string& name) {
        auto it = lockfile.packages.find(name);
        return it != lockfile.packages.end() ? &it->second : nullptr;
    }
    
    /**
     * ✅ التحقق من التطابق مع ص.toml
     */
    struct IntegrityResult {
        bool valid;
        std::vector<std::string> missingPackages;
        std::vector<std::string> extraPackages;
        std::vector<std::string> versionMismatches;
    };
    
    IntegrityResult checkIntegrity(
        const Lockfile& lockfile,
        const std::unordered_map<std::string, std::string>& tomlDeps
    ) {
        IntegrityResult result;
        result.valid = true;
        
        // التحقق من الحزم المفقودة
        for (const auto& [name, _] : tomlDeps) {
            if (lockfile.packages.find(name) == lockfile.packages.end()) {
                result.missingPackages.push_back(name);
                result.valid = false;
            }
        }
        
        // التحقق من الحزم الزائدة
        for (const auto& [name, _] : lockfile.packages) {
            if (tomlDeps.find(name) == tomlDeps.end()) {
                result.extraPackages.push_back(name);
                // الحزم الزائدة لا تعني عدم الصلاحية (قد تكون تبعيات غير مباشرة)
            }
        }
        
        return result;
    }
    
    /**
     * 🔄 تحديث ملف القفل
     */
    void update(
        Lockfile& lockfile,
        const std::unordered_map<std::string, std::string>& resolvedDeps
    ) {
        for (const auto& [name, version] : resolvedDeps) {
            auto it = lockfile.packages.find(name);
            if (it != lockfile.packages.end()) {
                it->second.version = version;
            } else {
                LockedPackage pkg;
                pkg.name = name;
                pkg.version = version;
                pkg.source = "registry";
                lockfile.packages[name] = pkg;
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      حساب التجزئة | Checksum Calculator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔐 ChecksumCalculator - حساب تجزئة الملفات
 */
class ChecksumCalculator {
public:
    /**
     * 🔢 حساب تجزئة SHA256 (محاكاة)
     */
    static std::string calculate(const fs::path& filePath) {
        if (!fs::exists(filePath)) {
            return "";
        }
        
        // محاكاة حساب SHA256
        // في التطبيق الحقيقي، نستخدم OpenSSL أو مكتبة مشابهة
        
        std::ifstream file(filePath, std::ios::binary);
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        size_t hash = std::hash<std::string>{}(buffer.str());
        
        std::ostringstream oss;
        oss << "sha256:" << std::hex << std::setfill('0') << std::setw(64) << hash;
        return oss.str();
    }
    
    /**
     * ✅ التحقق من التجزئة
     */
    static bool verify(const fs::path& filePath, const std::string& expected) {
        std::string actual = calculate(filePath);
        return actual == expected;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::unique_ptr<LockfileManager> globalManager;
static Lockfile currentLockfile;

extern "C" {
    /**
     * 📄 تحميل ملف القفل
     */
    int sad_lockfile_load(const char* projectPath) {
        if (!globalManager) {
            globalManager = std::make_unique<LockfileManager>();
        }
        
        currentLockfile = globalManager->loadOrCreate(
            projectPath ? projectPath : "."
        );
        
        return 1;
    }
    
    /**
     * 💾 حفظ ملف القفل
     */
    int sad_lockfile_save(const char* projectPath) {
        if (!globalManager) return 0;
        
        return globalManager->save(
            projectPath ? projectPath : ".",
            currentLockfile
        ) ? 1 : 0;
    }
    
    /**
     * ➕ إضافة حزمة للقفل
     */
    int sad_lockfile_add(
        const char* name,
        const char* version,
        const char* checksum,
        const char* source
    ) {
        if (!globalManager || !name) return 0;
        
        LockedPackage pkg;
        pkg.name = name;
        pkg.version = version ? version : "";
        pkg.checksum = checksum ? checksum : "";
        pkg.source = source ? source : "registry";
        
        globalManager->addPackage(currentLockfile, pkg);
        return 1;
    }
    
    /**
     * 🔍 الحصول على نسخة حزمة مقفلة
     */
    const char* sad_lockfile_get_version(const char* name) {
        if (!globalManager || !name) return nullptr;
        
        const auto* pkg = globalManager->getPackage(currentLockfile, name);
        if (!pkg) return nullptr;
        
        static std::string result;
        result = pkg->version;
        return result.c_str();
    }
    
    /**
     * 📊 عدد الحزم المقفلة
     */
    int sad_lockfile_count() {
        return static_cast<int>(currentLockfile.packages.size());
    }
    
    /**
     * 🔢 حساب تجزئة ملف
     */
    const char* sad_checksum_calculate(const char* filePath) {
        if (!filePath) return nullptr;
        
        static std::string result;
        result = ChecksumCalculator::calculate(filePath);
        return result.c_str();
    }
    
    /**
     * ✅ التحقق من تجزئة ملف
     */
    int sad_checksum_verify(const char* filePath, const char* expected) {
        if (!filePath || !expected) return 0;
        return ChecksumCalculator::verify(filePath, expected) ? 1 : 0;
    }
}

} // namespace pkg
} // namespace sad
