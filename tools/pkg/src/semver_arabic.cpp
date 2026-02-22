/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: semver_arabic.cpp                                                    ║
 * ║   الوصف: النسخة الدلالية بدعم عربي                                          ║
 * ║   المهمة: T174 - Phase 16 (US14)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هي النسخة الدلالية (Semantic Versioning)؟
 * ────────────────────────────────────────────────
 *
 * نظام لترقيم نسخ البرامج بطريقة واضحة ومفهومة.
 *
 * 📊 الصيغة: رئيسي.فرعي.تصحيح (Major.Minor.Patch)
 *
 *   مثال: 2.3.1
 *          │ │ └── تصحيح: إصلاح أخطاء صغيرة
 *          │ └──── فرعي: ميزات جديدة متوافقة
 *          └────── رئيسي: تغييرات كبيرة غير متوافقة
 *
 * 🔢 متى نزيد كل رقم؟
 *
 *   ⬆️ الرئيسي (1.0.0 → 2.0.0):
 *      عند تغييرات كسر التوافقية
 *      مثل: تغيير اسم دالة أساسية
 *
 *   ⬆️ الفرعي (1.0.0 → 1.1.0):
 *      عند إضافة ميزات جديدة متوافقة
 *      مثل: إضافة دالة جديدة
 *
 *   ⬆️ التصحيح (1.0.0 → 1.0.1):
 *      عند إصلاح أخطاء دون تغيير الواجهة
 *      مثل: إصلاح خطأ في حساب
 *
 * 📋 بادئات النطاقات:
 *   ^1.2.3  → متوافق مع 1.x.x (≥1.2.3, <2.0.0)
 *   ~1.2.3  → تحديثات تصحيح فقط (≥1.2.3, <1.3.0)
 *   >1.2.3  → أكبر من
 *   >=1.2.3 → أكبر من أو يساوي
 *   *       → أي نسخة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <optional>
#include <stdexcept>
#include <algorithm>

namespace sad {
namespace pkg {

// ═══════════════════════════════════════════════════════════════════════════════
//                        النسخة الدلالية | Semantic Version
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 SemVer - تمثيل النسخة الدلالية
 */
class SemVer {
public:
    int major;      // الرقم الرئيسي
    int minor;      // الرقم الفرعي
    int patch;      // رقم التصحيح
    std::string prerelease;  // إصدار تجريبي (مثل: alpha, beta)
    std::string build;       // معلومات البناء
    
    SemVer() : major(0), minor(0), patch(0) {}
    SemVer(int maj, int min, int pat) 
        : major(maj), minor(min), patch(pat) {}
    
    /**
     * 📖 تحليل نص النسخة
     */
    static std::optional<SemVer> parse(const std::string& versionStr) {
        static const std::regex semverRegex(
            R"(^v?(0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*))"
            R"((?:-((?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))"
            R"((?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?)"
            R"((?:\+([0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$)"
        );
        
        std::smatch match;
        if (!std::regex_match(versionStr, match, semverRegex)) {
            return std::nullopt;
        }
        
        SemVer ver;
        ver.major = std::stoi(match[1].str());
        ver.minor = std::stoi(match[2].str());
        ver.patch = std::stoi(match[3].str());
        
        if (match[4].matched) {
            ver.prerelease = match[4].str();
        }
        if (match[5].matched) {
            ver.build = match[5].str();
        }
        
        return ver;
    }
    
    /**
     * 📝 تحويل لنص
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << major << "." << minor << "." << patch;
        if (!prerelease.empty()) {
            oss << "-" << prerelease;
        }
        if (!build.empty()) {
            oss << "+" << build;
        }
        return oss.str();
    }
    
    /**
     * 📝 تحويل لنص عربي
     */
    std::string toArabicString() const {
        std::ostringstream oss;
        oss << major << "." << minor << "." << patch;
        if (!prerelease.empty()) {
            // ترجمة الإصدارات التجريبية الشائعة
            std::string arabicPre = prerelease;
            if (prerelease == "alpha") arabicPre = "ألفا";
            else if (prerelease == "beta") arabicPre = "بيتا";
            else if (prerelease == "rc") arabicPre = "مرشح";
            oss << "-" << arabicPre;
        }
        return oss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                     عمليات المقارنة
    // ═══════════════════════════════════════════════════════════════
    
    bool operator==(const SemVer& other) const {
        return major == other.major && 
               minor == other.minor && 
               patch == other.patch &&
               prerelease == other.prerelease;
    }
    
    bool operator!=(const SemVer& other) const {
        return !(*this == other);
    }
    
    bool operator<(const SemVer& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        if (patch != other.patch) return patch < other.patch;
        
        // النسخة التجريبية أقل من النسخة الرسمية
        if (prerelease.empty() && !other.prerelease.empty()) return false;
        if (!prerelease.empty() && other.prerelease.empty()) return true;
        
        return prerelease < other.prerelease;
    }
    
    bool operator<=(const SemVer& other) const {
        return *this == other || *this < other;
    }
    
    bool operator>(const SemVer& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const SemVer& other) const {
        return !(*this < other);
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                     عمليات الزيادة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * ⬆️ زيادة الرقم الرئيسي
     */
    SemVer bumpMajor() const {
        return SemVer(major + 1, 0, 0);
    }
    
    /**
     * ⬆️ زيادة الرقم الفرعي
     */
    SemVer bumpMinor() const {
        return SemVer(major, minor + 1, 0);
    }
    
    /**
     * ⬆️ زيادة رقم التصحيح
     */
    SemVer bumpPatch() const {
        return SemVer(major, minor, patch + 1);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        نطاق النسخ | Version Range
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 VersionRange - نطاق النسخ المقبولة
 */
class VersionRange {
public:
    enum class Operator {
        EXACT,          // =1.2.3
        CARET,          // ^1.2.3
        TILDE,          // ~1.2.3
        GREATER,        // >1.2.3
        GREATER_EQ,     // >=1.2.3
        LESS,           // <1.2.3
        LESS_EQ,        // <=1.2.3
        ANY             // *
    };
    
    VersionRange() : op_(Operator::ANY) {}
    VersionRange(Operator op, const SemVer& ver) : op_(op), version_(ver) {}
    
    /**
     * 📖 تحليل نطاق النسخ
     */
    static std::optional<VersionRange> parse(const std::string& rangeStr) {
        if (rangeStr.empty() || rangeStr == "*") {
            return VersionRange(Operator::ANY, SemVer());
        }
        
        std::string str = rangeStr;
        Operator op = Operator::EXACT;
        
        // تحديد المعامل
        if (str[0] == '^') {
            op = Operator::CARET;
            str = str.substr(1);
        }
        else if (str[0] == '~') {
            op = Operator::TILDE;
            str = str.substr(1);
        }
        else if (str.starts_with(">=")) {
            op = Operator::GREATER_EQ;
            str = str.substr(2);
        }
        else if (str.starts_with("<=")) {
            op = Operator::LESS_EQ;
            str = str.substr(2);
        }
        else if (str[0] == '>') {
            op = Operator::GREATER;
            str = str.substr(1);
        }
        else if (str[0] == '<') {
            op = Operator::LESS;
            str = str.substr(1);
        }
        else if (str[0] == '=') {
            op = Operator::EXACT;
            str = str.substr(1);
        }
        
        // تحليل النسخة
        auto ver = SemVer::parse(str);
        if (!ver) {
            // محاولة تحليل نسخة جزئية (مثل: 1.2)
            if (str.find('.') == str.rfind('.')) {
                str += ".0";
                ver = SemVer::parse(str);
            }
            if (!ver) {
                return std::nullopt;
            }
        }
        
        return VersionRange(op, *ver);
    }
    
    /**
     * ✅ التحقق من توافق نسخة
     */
    bool satisfies(const SemVer& ver) const {
        switch (op_) {
            case Operator::ANY:
                return true;
                
            case Operator::EXACT:
                return ver == version_;
                
            case Operator::CARET:
                // ^1.2.3 = >=1.2.3 and <2.0.0
                // ^0.2.3 = >=0.2.3 and <0.3.0
                // ^0.0.3 = >=0.0.3 and <0.0.4
                if (ver < version_) return false;
                if (version_.major == 0) {
                    if (version_.minor == 0) {
                        return ver.major == 0 && ver.minor == 0 && 
                               ver.patch == version_.patch;
                    }
                    return ver.major == 0 && ver.minor == version_.minor;
                }
                return ver.major == version_.major;
                
            case Operator::TILDE:
                // ~1.2.3 = >=1.2.3 and <1.3.0
                if (ver < version_) return false;
                return ver.major == version_.major && 
                       ver.minor == version_.minor;
                
            case Operator::GREATER:
                return ver > version_;
                
            case Operator::GREATER_EQ:
                return ver >= version_;
                
            case Operator::LESS:
                return ver < version_;
                
            case Operator::LESS_EQ:
                return ver <= version_;
        }
        
        return false;
    }
    
    /**
     * 📝 تحويل لنص
     */
    std::string toString() const {
        std::string prefix;
        switch (op_) {
            case Operator::ANY: return "*";
            case Operator::EXACT: prefix = ""; break;
            case Operator::CARET: prefix = "^"; break;
            case Operator::TILDE: prefix = "~"; break;
            case Operator::GREATER: prefix = ">"; break;
            case Operator::GREATER_EQ: prefix = ">="; break;
            case Operator::LESS: prefix = "<"; break;
            case Operator::LESS_EQ: prefix = "<="; break;
        }
        return prefix + version_.toString();
    }
    
    /**
     * 📝 تحويل لنص عربي
     */
    std::string toArabicString() const {
        std::string prefix;
        switch (op_) {
            case Operator::ANY: return "أي نسخة";
            case Operator::EXACT: prefix = "بالضبط "; break;
            case Operator::CARET: prefix = "متوافق مع "; break;
            case Operator::TILDE: prefix = "تصحيحات "; break;
            case Operator::GREATER: prefix = "أكبر من "; break;
            case Operator::GREATER_EQ: prefix = "أكبر من أو يساوي "; break;
            case Operator::LESS: prefix = "أصغر من "; break;
            case Operator::LESS_EQ: prefix = "أصغر من أو يساوي "; break;
        }
        return prefix + version_.toString();
    }
    
private:
    Operator op_;
    SemVer version_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     محلل النسخ المركب | Composite Range
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 CompositeRange - نطاق مركب (مثل: >=1.0 <2.0)
 */
class CompositeRange {
public:
    /**
     * 📖 تحليل نطاق مركب
     */
    static std::optional<CompositeRange> parse(const std::string& rangeStr) {
        CompositeRange result;
        
        // تقسيم على المسافات أو الفواصل
        std::vector<std::string> parts;
        std::istringstream iss(rangeStr);
        std::string part;
        
        while (iss >> part) {
            if (part == "," || part == "&&" || part == "و") continue;
            parts.push_back(part);
        }
        
        // إذا كانت القائمة فارغة، تعامل كـ "*"
        if (parts.empty()) {
            auto range = VersionRange::parse("*");
            if (range) result.ranges_.push_back(*range);
            return result;
        }
        
        for (const auto& p : parts) {
            auto range = VersionRange::parse(p);
            if (!range) {
                return std::nullopt;
            }
            result.ranges_.push_back(*range);
        }
        
        return result;
    }
    
    /**
     * ✅ التحقق من توافق نسخة
     */
    bool satisfies(const SemVer& ver) const {
        for (const auto& range : ranges_) {
            if (!range.satisfies(ver)) {
                return false;
            }
        }
        return true;
    }
    
    /**
     * 🔍 إيجاد أفضل نسخة متوافقة
     */
    std::optional<SemVer> findBestMatch(const std::vector<SemVer>& available) const {
        std::optional<SemVer> best;
        
        for (const auto& ver : available) {
            if (satisfies(ver)) {
                if (!best || ver > *best) {
                    best = ver;
                }
            }
        }
        
        return best;
    }
    
private:
    std::vector<VersionRange> ranges_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {
    /**
     * 📖 تحليل نسخة
     */
    int sad_semver_parse(
        const char* versionStr,
        int* major, int* minor, int* patch
    ) {
        auto ver = SemVer::parse(versionStr ? versionStr : "");
        if (!ver) return 0;
        
        if (major) *major = ver->major;
        if (minor) *minor = ver->minor;
        if (patch) *patch = ver->patch;
        
        return 1;
    }
    
    /**
     * 🔄 مقارنة نسختين
     */
    int sad_semver_compare(const char* v1, const char* v2) {
        auto ver1 = SemVer::parse(v1 ? v1 : "");
        auto ver2 = SemVer::parse(v2 ? v2 : "");
        
        if (!ver1 || !ver2) return 0;
        
        if (*ver1 < *ver2) return -1;
        if (*ver1 > *ver2) return 1;
        return 0;
    }
    
    /**
     * ✅ التحقق من توافق نسخة مع نطاق
     */
    int sad_semver_satisfies(const char* version, const char* range) {
        auto ver = SemVer::parse(version ? version : "");
        auto rng = CompositeRange::parse(range ? range : "*");
        
        if (!ver || !rng) return 0;
        
        return rng->satisfies(*ver) ? 1 : 0;
    }
    
    /**
     * ⬆️ زيادة النسخة
     */
    const char* sad_semver_bump(const char* version, const char* type) {
        auto ver = SemVer::parse(version ? version : "0.0.0");
        if (!ver) return nullptr;
        
        static std::string result;
        std::string bumpType = type ? type : "patch";
        
        SemVer newVer;
        if (bumpType == "major" || bumpType == "رئيسي") {
            newVer = ver->bumpMajor();
        }
        else if (bumpType == "minor" || bumpType == "فرعي") {
            newVer = ver->bumpMinor();
        }
        else {
            newVer = ver->bumpPatch();
        }
        
        result = newVer.toString();
        return result.c_str();
    }
    
    /**
     * 📝 تحويل نسخة لنص عربي
     */
    const char* sad_semver_to_arabic(const char* version) {
        auto ver = SemVer::parse(version ? version : "");
        if (!ver) return nullptr;
        
        static std::string result;
        result = ver->toArabicString();
        return result.c_str();
    }
    
    /**
     * ✅ التحقق من صحة صيغة النسخة
     */
    int sad_semver_is_valid(const char* version) {
        return SemVer::parse(version ? version : "").has_value() ? 1 : 0;
    }
}

} // namespace pkg
} // namespace sad
