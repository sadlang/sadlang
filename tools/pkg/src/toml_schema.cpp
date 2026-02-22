/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة ص - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: toml_schema.cpp                                                      ║
 * ║   الوصف: التحقق من صحة مخطط ص.toml                                          ║
 * ║   المهمة: T170 - Phase 16 (US14)                                            ║
 * ║   المؤلف: فريق ص | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هو التحقق من المخطط؟
 * ──────────────────────────────────────
 *
 * عندما تكتب ملف ص.toml، يجب أن يتبع قواعد معينة:
 *
 * ✅ صحيح:
 *   [الحزمة]
 *   الاسم = "مشروعي"      ← نص ✓
 *   النسخة = "1.0.0"      ← صيغة نسخة صحيحة ✓
 *
 * ❌ خطأ:
 *   [الحزمة]
 *   الاسم = 123           ← يجب أن يكون نص!
 *   النسخة = "abc"        ← صيغة نسخة خاطئة!
 *
 * 📋 المخطط يحدد:
 *   • ما الحقول المطلوبة والاختيارية
 *   • نوع كل حقل (نص، عدد، قائمة...)
 *   • القيم المسموحة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <sstream>
#include <regex>
#include <functional>
#include <optional>

namespace sad {
namespace pkg {

// ═══════════════════════════════════════════════════════════════════════════════
//                          أنواع المخطط | Schema Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 SchemaType - أنواع الحقول في المخطط
 */
enum class SchemaType {
    STRING,         // نص
    INTEGER,        // عدد صحيح
    FLOAT,          // عدد عشري
    BOOLEAN,        // منطقي
    ARRAY,          // قائمة
    TABLE,          // جدول
    SEMVER,         // نسخة دلالية
    EMAIL,          // بريد إلكتروني
    URL,            // رابط
    PATH,           // مسار ملف
    DEPENDENCY_SPEC // مواصفة تبعية
};

/**
 * 📝 ValidationError - خطأ في التحقق
 */
struct ValidationError {
    std::string path;       // المسار (مثل: الحزمة.الاسم)
    std::string messageAr;  // الرسالة بالعربية
    std::string messageEn;  // الرسالة بالإنجليزية
    bool isCritical;        // هل يمنع المتابعة؟
    
    std::string format(bool useArabic = true) const {
        std::ostringstream oss;
        oss << "❌ " << path << ": ";
        oss << (useArabic ? messageAr : messageEn);
        if (isCritical) oss << " [حرج]";
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        تعريف حقل المخطط | Schema Field Definition
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 SchemaField - تعريف حقل في المخطط
 */
struct SchemaField {
    std::string nameAr;         // الاسم بالعربية
    std::string nameEn;         // الاسم بالإنجليزية
    SchemaType type;            // النوع
    bool required;              // مطلوب؟
    std::string descriptionAr;  // الوصف بالعربية
    std::string descriptionEn;  // الوصف بالإنجليزية
    std::optional<std::string> defaultValue;  // القيمة الافتراضية
    std::vector<std::string> allowedValues;   // القيم المسموحة (إن وجدت)
    std::function<bool(const std::string&)> customValidator;  // مُحقق مخصص
    
    SchemaField() : type(SchemaType::STRING), required(false) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        مُحققات القيم | Value Validators
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ✅ ValueValidators - مُحققات القيم المختلفة
 */
class ValueValidators {
public:
    /**
     * 🔢 التحقق من النسخة الدلالية
     * صيغة: major.minor.patch (مثل: 1.2.3)
     */
    static bool isValidSemver(const std::string& version) {
        // النسخة الدلالية: X.Y.Z أو X.Y.Z-prerelease أو X.Y.Z+build
        static const std::regex semverRegex(
            R"(^(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*))"
            R"((?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))"
            R"((?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?)"
            R"((?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$)"
        );
        return std::regex_match(version, semverRegex);
    }
    
    /**
     * 📧 التحقق من البريد الإلكتروني
     */
    static bool isValidEmail(const std::string& email) {
        static const std::regex emailRegex(
            R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"
        );
        return std::regex_match(email, emailRegex);
    }
    
    /**
     * 🔗 التحقق من الرابط
     */
    static bool isValidUrl(const std::string& url) {
        static const std::regex urlRegex(
            R"(^https?://[^\s/$.?#].[^\s]*$)"
        );
        return std::regex_match(url, urlRegex);
    }
    
    /**
     * 📁 التحقق من اسم الحزمة
     * يجب أن يكون: حروف عربية أو إنجليزية، أرقام، شرطة سفلية
     */
    static bool isValidPackageName(const std::string& name) {
        if (name.empty() || name.length() > 64) return false;
        
        // التحقق من عدم البدء برقم
        char first = name[0];
        if (first >= '0' && first <= '9') return false;
        
        // التحقق من الحروف المسموحة
        for (size_t i = 0; i < name.length(); ) {
            unsigned char c = name[i];
            
            // ASCII: حروف، أرقام، شرطة سفلية، شرطة
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '_' || c == '-') {
                i++;
                continue;
            }
            
            // UTF-8: حروف عربية
            if (c >= 0xD8 && c <= 0xDB) {
                // تخطي بايتات UTF-8 الإضافية
                if (i + 1 < name.length()) {
                    i += 2;
                    continue;
                }
            }
            
            return false;
        }
        
        return true;
    }
    
    /**
     * 📦 التحقق من مواصفة التبعية
     * صيغ مدعومة: "1.0", "^1.0", "~1.0", ">=1.0", "*"
     */
    static bool isValidDependencySpec(const std::string& spec) {
        if (spec.empty()) return false;
        if (spec == "*") return true;
        
        // إزالة البادئة
        std::string version = spec;
        if (version[0] == '^' || version[0] == '~' ||
            version[0] == '>' || version[0] == '<' || version[0] == '=') {
            size_t start = 1;
            if (version.length() > 1 && version[1] == '=') start = 2;
            version = version.substr(start);
        }
        
        // التحقق من النسخة
        // صيغة مبسطة: X أو X.Y أو X.Y.Z
        static const std::regex versionRegex(
            R"(^(0|[1-9]\d*)(?:\.(0|[1-9]\d*))?(?:\.(0|[1-9]\d*))?$)"
        );
        return std::regex_match(version, versionRegex);
    }
    
    /**
     * 📜 التحقق من الرخصة
     */
    static bool isValidLicense(const std::string& license) {
        static const std::unordered_set<std::string> validLicenses = {
            "MIT", "Apache-2.0", "GPL-3.0", "GPL-2.0", "BSD-3-Clause",
            "BSD-2-Clause", "ISC", "MPL-2.0", "LGPL-3.0", "LGPL-2.1",
            "Unlicense", "CC0-1.0", "WTFPL", "Zlib", "BSL-1.0",
            // أسماء عربية
            "رخصة_حرة", "ملكية_عامة", "بدون_قيود"
        };
        return validLicenses.count(license) > 0;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        مخطط ص.toml | Package Schema
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 PackageSchema - مخطط ملف ص.toml الكامل
 */
class PackageSchema {
public:
    PackageSchema() {
        initializeSchema();
    }
    
    /**
     * ✅ التحقق من صحة الجدول
     */
    std::vector<ValidationError> validate(
        const std::unordered_map<std::string, std::string>& flatValues
    ) {
        std::vector<ValidationError> errors;
        
        // التحقق من الحقول المطلوبة
        for (const auto& [path, field] : schema_) {
            if (field.required) {
                auto it = flatValues.find(path);
                auto itAr = flatValues.find(field.nameAr);
                
                if (it == flatValues.end() && itAr == flatValues.end()) {
                    errors.push_back({
                        path,
                        "الحقل '" + field.nameAr + "' مطلوب",
                        "Field '" + field.nameEn + "' is required",
                        true
                    });
                }
            }
        }
        
        // التحقق من قيم الحقول
        for (const auto& [path, value] : flatValues) {
            auto it = schema_.find(path);
            if (it == schema_.end()) {
                // محاولة البحث بالاسم العربي
                for (const auto& [schemaPath, field] : schema_) {
                    if (field.nameAr == path || schemaPath.ends_with("." + path)) {
                        it = schema_.find(schemaPath);
                        break;
                    }
                }
            }
            
            if (it != schema_.end()) {
                auto fieldErrors = validateField(path, value, it->second);
                errors.insert(errors.end(), fieldErrors.begin(), fieldErrors.end());
            }
        }
        
        return errors;
    }
    
    /**
     * 📖 الحصول على وصف حقل
     */
    std::string getFieldDescription(const std::string& path, bool arabic = true) const {
        auto it = schema_.find(path);
        if (it != schema_.end()) {
            return arabic ? it->second.descriptionAr : it->second.descriptionEn;
        }
        return "";
    }
    
    /**
     * 📋 الحصول على الحقول المطلوبة
     */
    std::vector<std::string> getRequiredFields() const {
        std::vector<std::string> required;
        for (const auto& [path, field] : schema_) {
            if (field.required) {
                required.push_back(path);
            }
        }
        return required;
    }
    
private:
    std::unordered_map<std::string, SchemaField> schema_;
    
    void initializeSchema() {
        // ═══════════════════════════════════════════════════════════════
        //                     قسم الحزمة [الحزمة]
        // ═══════════════════════════════════════════════════════════════
        {
            SchemaField field;
            field.nameAr = "الاسم";
            field.nameEn = "name";
            field.type = SchemaType::STRING;
            field.required = true;
            field.descriptionAr = "اسم الحزمة الفريد";
            field.descriptionEn = "Unique package name";
            field.customValidator = ValueValidators::isValidPackageName;
            schema_["package.name"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "النسخة";
            field.nameEn = "version";
            field.type = SchemaType::SEMVER;
            field.required = true;
            field.descriptionAr = "نسخة الحزمة (صيغة: X.Y.Z)";
            field.descriptionEn = "Package version (format: X.Y.Z)";
            field.customValidator = ValueValidators::isValidSemver;
            schema_["package.version"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "الوصف";
            field.nameEn = "description";
            field.type = SchemaType::STRING;
            field.required = false;
            field.descriptionAr = "وصف مختصر للحزمة";
            field.descriptionEn = "Brief package description";
            schema_["package.description"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "المؤلف";
            field.nameEn = "author";
            field.type = SchemaType::STRING;
            field.required = false;
            field.descriptionAr = "اسم المؤلف (اختياري: <بريد>)";
            field.descriptionEn = "Author name (optional: <email>)";
            schema_["package.author"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "المؤلفون";
            field.nameEn = "authors";
            field.type = SchemaType::ARRAY;
            field.required = false;
            field.descriptionAr = "قائمة المؤلفين";
            field.descriptionEn = "List of authors";
            schema_["package.authors"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "الرخصة";
            field.nameEn = "license";
            field.type = SchemaType::STRING;
            field.required = false;
            field.defaultValue = "MIT";
            field.descriptionAr = "نوع الرخصة (مثل: MIT, Apache-2.0)";
            field.descriptionEn = "License type (e.g., MIT, Apache-2.0)";
            field.customValidator = ValueValidators::isValidLicense;
            schema_["package.license"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "المستودع";
            field.nameEn = "repository";
            field.type = SchemaType::URL;
            field.required = false;
            field.descriptionAr = "رابط مستودع الكود المصدري";
            field.descriptionEn = "Source code repository URL";
            field.customValidator = ValueValidators::isValidUrl;
            schema_["package.repository"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "الصفحة";
            field.nameEn = "homepage";
            field.type = SchemaType::URL;
            field.required = false;
            field.descriptionAr = "رابط الصفحة الرئيسية";
            field.descriptionEn = "Homepage URL";
            field.customValidator = ValueValidators::isValidUrl;
            schema_["package.homepage"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "التوثيق";
            field.nameEn = "documentation";
            field.type = SchemaType::URL;
            field.required = false;
            field.descriptionAr = "رابط التوثيق";
            field.descriptionEn = "Documentation URL";
            field.customValidator = ValueValidators::isValidUrl;
            schema_["package.documentation"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "الكلمات_المفتاحية";
            field.nameEn = "keywords";
            field.type = SchemaType::ARRAY;
            field.required = false;
            field.descriptionAr = "كلمات مفتاحية للبحث";
            field.descriptionEn = "Keywords for search";
            schema_["package.keywords"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "الفئات";
            field.nameEn = "categories";
            field.type = SchemaType::ARRAY;
            field.required = false;
            field.descriptionAr = "فئات الحزمة";
            field.descriptionEn = "Package categories";
            schema_["package.categories"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "الحد_الأدنى_ص";
            field.nameEn = "sad_minimum";
            field.type = SchemaType::SEMVER;
            field.required = false;
            field.descriptionAr = "الحد الأدنى لنسخة ص المطلوبة";
            field.descriptionEn = "Minimum required Sad version";
            field.customValidator = ValueValidators::isValidSemver;
            schema_["package.sad_minimum"] = field;
        }
        
        // ═══════════════════════════════════════════════════════════════
        //                  قسم البناء [البناء]
        // ═══════════════════════════════════════════════════════════════
        {
            SchemaField field;
            field.nameAr = "الهدف";
            field.nameEn = "target";
            field.type = SchemaType::STRING;
            field.required = false;
            field.allowedValues = {"تنفيذي", "مكتبة", "executable", "library"};
            field.defaultValue = "تنفيذي";
            field.descriptionAr = "نوع الناتج: تنفيذي أو مكتبة";
            field.descriptionEn = "Output type: executable or library";
            schema_["build.target"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "المدخل";
            field.nameEn = "entry";
            field.type = SchemaType::PATH;
            field.required = false;
            field.defaultValue = "src/main.ص";
            field.descriptionAr = "ملف نقطة الدخول";
            field.descriptionEn = "Entry point file";
            schema_["build.entry"] = field;
        }
        
        {
            SchemaField field;
            field.nameAr = "المخرج";
            field.nameEn = "output";
            field.type = SchemaType::PATH;
            field.required = false;
            field.descriptionAr = "مسار ملف الإخراج";
            field.descriptionEn = "Output file path";
            schema_["build.output"] = field;
        }
    }
    
    std::vector<ValidationError> validateField(
        const std::string& path,
        const std::string& value,
        const SchemaField& field
    ) {
        std::vector<ValidationError> errors;
        
        // التحقق من القيم المسموحة
        if (!field.allowedValues.empty()) {
            bool found = false;
            for (const auto& allowed : field.allowedValues) {
                if (value == allowed) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::string allowedStr;
                for (size_t i = 0; i < field.allowedValues.size(); ++i) {
                    if (i > 0) allowedStr += ", ";
                    allowedStr += field.allowedValues[i];
                }
                errors.push_back({
                    path,
                    "قيمة غير مسموحة. القيم المتاحة: " + allowedStr,
                    "Invalid value. Allowed values: " + allowedStr,
                    true
                });
            }
        }
        
        // التحقق المخصص
        if (field.customValidator && !value.empty()) {
            if (!field.customValidator(value)) {
                errors.push_back({
                    path,
                    "قيمة غير صالحة لـ " + field.nameAr,
                    "Invalid value for " + field.nameEn,
                    true
                });
            }
        }
        
        return errors;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     مُنشئ ص.toml | Package.toml Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📝 PackageTomlGenerator - إنشاء ملفات ص.toml
 */
class PackageTomlGenerator {
public:
    /**
     * 📄 إنشاء ص.toml أساسي
     */
    static std::string generateBasic(
        const std::string& name,
        const std::string& version = "0.1.0",
        const std::string& description = ""
    ) {
        std::ostringstream toml;
        
        toml << "# ═══════════════════════════════════════════════════════════\n";
        toml << "#              ملف إعدادات الحزمة - لغة ص\n";
        toml << "#              Package Configuration - Sad Language\n";
        toml << "# ═══════════════════════════════════════════════════════════\n\n";
        
        toml << "[الحزمة]\n";
        toml << "الاسم = \"" << name << "\"\n";
        toml << "النسخة = \"" << version << "\"\n";
        
        if (!description.empty()) {
            toml << "الوصف = \"" << description << "\"\n";
        } else {
            toml << "الوصف = \"وصف المشروع\"\n";
        }
        
        toml << "# المؤلف = \"اسمك <بريدك@مثال.com>\"\n";
        toml << "# الرخصة = \"MIT\"\n";
        toml << "\n";
        
        toml << "# ═══════════════════════════════════════════════════════════\n";
        toml << "#                          المتطلبات\n";
        toml << "# ═══════════════════════════════════════════════════════════\n\n";
        
        toml << "[المتطلبات]\n";
        toml << "# أضف التبعيات هنا\n";
        toml << "# مثال: رسومات = \"^1.0\"\n";
        toml << "\n";
        
        toml << "[التطوير]\n";
        toml << "# تبعيات التطوير فقط\n";
        toml << "# مثال: اختبار = \"0.3\"\n";
        
        return toml.str();
    }
    
    /**
     * 📄 إنشاء ص.toml كامل
     */
    static std::string generateFull(
        const std::string& name,
        const std::string& version,
        const std::string& description,
        const std::string& author,
        const std::string& license,
        const std::vector<std::pair<std::string, std::string>>& dependencies
    ) {
        std::ostringstream toml;
        
        toml << "# ═══════════════════════════════════════════════════════════\n";
        toml << "#              ملف إعدادات الحزمة - لغة ص\n";
        toml << "# ═══════════════════════════════════════════════════════════\n\n";
        
        toml << "[الحزمة]\n";
        toml << "الاسم = \"" << name << "\"\n";
        toml << "النسخة = \"" << version << "\"\n";
        toml << "الوصف = \"" << description << "\"\n";
        
        if (!author.empty()) {
            toml << "المؤلف = \"" << author << "\"\n";
        }
        
        if (!license.empty()) {
            toml << "الرخصة = \"" << license << "\"\n";
        }
        
        toml << "\n[المتطلبات]\n";
        for (const auto& [dep, ver] : dependencies) {
            toml << dep << " = \"" << ver << "\"\n";
        }
        
        return toml.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::unique_ptr<PackageSchema> globalSchema;
static std::vector<ValidationError> lastValidationErrors;

extern "C" {
    void sad_schema_init() {
        if (!globalSchema) {
            globalSchema = std::make_unique<PackageSchema>();
        }
    }
    
    int sad_schema_validate(const char** keys, const char** values, int count) {
        if (!globalSchema) sad_schema_init();
        
        std::unordered_map<std::string, std::string> flatValues;
        for (int i = 0; i < count; ++i) {
            if (keys[i] && values[i]) {
                flatValues[keys[i]] = values[i];
            }
        }
        
        lastValidationErrors = globalSchema->validate(flatValues);
        return lastValidationErrors.empty() ? 1 : 0;
    }
    
    int sad_schema_get_error_count() {
        return static_cast<int>(lastValidationErrors.size());
    }
    
    const char* sad_schema_get_error(int index) {
        if (index < 0 || index >= static_cast<int>(lastValidationErrors.size())) {
            return nullptr;
        }
        static std::string result;
        result = lastValidationErrors[index].format(true);
        return result.c_str();
    }
    
    const char* sad_schema_generate_basic(const char* name, const char* version) {
        static std::string result;
        result = PackageTomlGenerator::generateBasic(
            name ? name : "مشروعي",
            version ? version : "0.1.0"
        );
        return result.c_str();
    }
    
    int sad_schema_is_valid_semver(const char* version) {
        return version && ValueValidators::isValidSemver(version) ? 1 : 0;
    }
    
    int sad_schema_is_valid_package_name(const char* name) {
        return name && ValueValidators::isValidPackageName(name) ? 1 : 0;
    }
}

} // namespace pkg
} // namespace sad
