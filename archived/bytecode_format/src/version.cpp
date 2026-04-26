/**
 * ==========================================================
 * ملف: version.cpp
 * الوصف: إدارة إصدارات Bytecode (1.0.0)
 * المهمة: T271 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🔢 ما هو Semantic Versioning؟
 * -----------------------------
 * نظام ترقيم الإصدارات يستخدم ثلاثة أرقام:
 * 
 *   MAJOR.MINOR.PATCH
 *   رئيسي.فرعي.تصحيحي
 * 
 * مثال: 1.2.3
 * - 1 = الإصدار الرئيسي
 * - 2 = الإصدار الفرعي
 * - 3 = إصدار التصحيح
 * 
 * 📊 متى يتغير كل رقم؟
 * ---------------------
 * 
 * 🔴 MAJOR (الرئيسي):
 *    يزيد عند تغييرات كبيرة غير متوافقة
 *    مثل: تغيير بنية الملف بالكامل
 *    
 * 🟡 MINOR (الفرعي):
 *    يزيد عند إضافة ميزات جديدة متوافقة
 *    مثل: إضافة تعليمات جديدة
 *    
 * 🟢 PATCH (التصحيحي):
 *    يزيد عند إصلاح أخطاء
 *    مثل: تصحيح خطأ في التشفير
 * 
 * 🤝 قواعد التوافق:
 * -----------------
 * - ملفات نفس MAJOR متوافقة (غالباً)
 * - MINOR أقل متوافق مع أعلى (backwards compatible)
 * - PATCH لا يؤثر على التوافق
 */

#include "format.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace sad {
namespace bytecode {

// ==========================================================
// 📌 فئة Version (الإصدار)
// ==========================================================

/**
 * 📊 فئة لإدارة إصدارات Bytecode
 */
class Version {
public:
    uint8_t major;      // الإصدار الرئيسي
    uint8_t minor;      // الإصدار الفرعي
    uint8_t patch;      // إصدار التصحيح
    
    // ═══════════════════════════════════════════════════════
    // البناة
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🏗️ إنشاء إصدار جديد
     */
    Version(uint8_t maj = VERSION_MAJOR, 
            uint8_t min = VERSION_MINOR, 
            uint8_t pat = VERSION_PATCH)
        : major(maj), minor(min), patch(pat) {}
    
    /**
     * 🏗️ إنشاء من string "1.2.3"
     */
    static Version parse(const std::string& str) {
        Version v(0, 0, 0);
        
        std::istringstream iss(str);
        char dot1, dot2;
        int maj, min, pat;
        
        if (iss >> maj >> dot1 >> min >> dot2 >> pat) {
            if (dot1 == '.' && dot2 == '.' &&
                maj >= 0 && maj <= 255 &&
                min >= 0 && min <= 255 &&
                pat >= 0 && pat <= 255) {
                v.major = static_cast<uint8_t>(maj);
                v.minor = static_cast<uint8_t>(min);
                v.patch = static_cast<uint8_t>(pat);
            }
        }
        
        return v;
    }
    
    /**
     * 🏗️ الإصدار الحالي للمترجم
     */
    static Version current() {
        return Version(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    }
    
    // ═══════════════════════════════════════════════════════
    // التحويلات
    // ═══════════════════════════════════════════════════════
    
    /**
     * 📝 تحويل إلى string
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << static_cast<int>(major) << "."
            << static_cast<int>(minor) << "."
            << static_cast<int>(patch);
        return oss.str();
    }
    
    /**
     * 📝 تحويل إلى string عربي
     */
    std::string toArabicString() const {
        std::ostringstream oss;
        oss << "الإصدار " << static_cast<int>(major) << "."
            << static_cast<int>(minor) << "."
            << static_cast<int>(patch);
        return oss.str();
    }
    
    /**
     * 🔢 تحويل إلى رقم واحد للمقارنة السريعة
     * 
     * الصيغة: MAJOR * 10000 + MINOR * 100 + PATCH
     */
    uint32_t toNumber() const {
        return static_cast<uint32_t>(major) * 10000 +
               static_cast<uint32_t>(minor) * 100 +
               static_cast<uint32_t>(patch);
    }
    
    // ═══════════════════════════════════════════════════════
    // المقارنات
    // ═══════════════════════════════════════════════════════
    
    bool operator==(const Version& other) const {
        return major == other.major && 
               minor == other.minor && 
               patch == other.patch;
    }
    
    bool operator!=(const Version& other) const {
        return !(*this == other);
    }
    
    bool operator<(const Version& other) const {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
    
    bool operator<=(const Version& other) const {
        return *this == other || *this < other;
    }
    
    bool operator>(const Version& other) const {
        return !(*this <= other);
    }
    
    bool operator>=(const Version& other) const {
        return !(*this < other);
    }
    
    // ═══════════════════════════════════════════════════════
    // التوافق
    // ═══════════════════════════════════════════════════════
    
    /**
     * 🤝 هل هذا الإصدار متوافق مع إصدار آخر؟
     * 
     * @param other الإصدار الآخر
     * @return true إذا كانا متوافقين
     * 
     * 📚 قواعد التوافق:
     * - MAJOR يجب أن يتطابق
     * - MINOR للملف يجب أن يكون ≤ MINOR للمفسر
     */
    bool isCompatibleWith(const Version& other) const {
        // الإصدار الرئيسي يجب أن يتطابق
        if (major != other.major) {
            return false;
        }
        
        // الإصدار الفرعي للملف يجب أن يكون أقل أو مساوي
        if (minor > other.minor) {
            return false;
        }
        
        return true;
    }
    
    /**
     * 🤝 هل متوافق مع الإصدار الحالي؟
     */
    bool isCompatibleWithCurrent() const {
        return isCompatibleWith(current());
    }
    
    /**
     * 📊 نوع عدم التوافق
     */
    enum class IncompatibilityReason {
        Compatible,         // متوافق
        MajorMismatch,      // الإصدار الرئيسي مختلف
        MinorTooNew,        // الإصدار الفرعي أحدث
        TooOld,             // قديم جداً
        TooNew              // جديد جداً
    };
    
    /**
     * 📊 سبب عدم التوافق
     */
    IncompatibilityReason getIncompatibilityReason(const Version& other) const {
        if (isCompatibleWith(other)) {
            return IncompatibilityReason::Compatible;
        }
        
        if (major != other.major) {
            if (major < other.major) {
                return IncompatibilityReason::TooOld;
            } else {
                return IncompatibilityReason::TooNew;
            }
        }
        
        if (minor > other.minor) {
            return IncompatibilityReason::MinorTooNew;
        }
        
        return IncompatibilityReason::MajorMismatch;
    }
    
    /**
     * 📝 رسالة عدم التوافق بالعربية
     */
    std::string getIncompatibilityMessage(const Version& other) const {
        auto reason = getIncompatibilityReason(other);
        
        switch (reason) {
            case IncompatibilityReason::Compatible:
                return "الإصداران متوافقان";
            case IncompatibilityReason::MajorMismatch:
                return "الإصدار الرئيسي مختلف - تحتاج ترقية المفسر أو إعادة ترجمة الملف";
            case IncompatibilityReason::MinorTooNew:
                return "الملف أحدث من المفسر - يرجى ترقية المفسر";
            case IncompatibilityReason::TooOld:
                return "الملف قديم جداً - يرجى إعادة ترجمته";
            case IncompatibilityReason::TooNew:
                return "الملف من إصدار مستقبلي - يرجى ترقية المفسر";
            default:
                return "خطأ غير معروف في التوافق";
        }
    }
    
    // ═══════════════════════════════════════════════════════
    // العمليات
    // ═══════════════════════════════════════════════════════
    
    /**
     * ⬆️ زيادة PATCH
     */
    Version bumpPatch() const {
        return Version(major, minor, patch + 1);
    }
    
    /**
     * ⬆️ زيادة MINOR (يصفّر PATCH)
     */
    Version bumpMinor() const {
        return Version(major, minor + 1, 0);
    }
    
    /**
     * ⬆️ زيادة MAJOR (يصفّر MINOR و PATCH)
     */
    Version bumpMajor() const {
        return Version(major + 1, 0, 0);
    }
};

// ==========================================================
// 📌 فئة VersionRange (نطاق الإصدارات)
// ==========================================================

/**
 * 📊 نطاق من الإصدارات
 * 
 * 📚 الاستخدام:
 * مثلاً: ">=1.0.0, <2.0.0" يعني أي إصدار من 1.0.0 حتى قبل 2.0.0
 */
class VersionRange {
public:
    Version min;        // الحد الأدنى
    Version max;        // الحد الأقصى
    bool minInclusive;  // هل الحد الأدنى مضمّن؟
    bool maxInclusive;  // هل الحد الأقصى مضمّن؟
    
    /**
     * 🏗️ إنشاء نطاق
     */
    VersionRange(const Version& minVer, const Version& maxVer,
                 bool minIncl = true, bool maxIncl = false)
        : min(minVer), max(maxVer), 
          minInclusive(minIncl), maxInclusive(maxIncl) {}
    
    /**
     * 🏗️ نطاق من إصدار واحد (exact match)
     */
    static VersionRange exact(const Version& ver) {
        return VersionRange(ver, ver, true, true);
    }
    
    /**
     * 🏗️ نطاق ">= version"
     */
    static VersionRange atLeast(const Version& ver) {
        return VersionRange(ver, Version(255, 255, 255), true, true);
    }
    
    /**
     * 🏗️ نطاق متوافق مع MAJOR معين
     * مثال: "^1.0.0" = من 1.0.0 إلى <2.0.0
     */
    static VersionRange compatible(const Version& ver) {
        Version maxVer(ver.major + 1, 0, 0);
        return VersionRange(ver, maxVer, true, false);
    }
    
    /**
     * 🔍 هل الإصدار ضمن النطاق؟
     */
    bool contains(const Version& ver) const {
        // فحص الحد الأدنى
        if (minInclusive) {
            if (ver < min) return false;
        } else {
            if (ver <= min) return false;
        }
        
        // فحص الحد الأقصى
        if (maxInclusive) {
            if (ver > max) return false;
        } else {
            if (ver >= max) return false;
        }
        
        return true;
    }
    
    /**
     * 📝 تحويل إلى string
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << (minInclusive ? ">=" : ">") << min.toString()
            << ", " << (maxInclusive ? "<=" : "<") << max.toString();
        return oss.str();
    }
};

// ==========================================================
// 📌 فئة VersionHeader (رأس الإصدار في الملف)
// ==========================================================

/**
 * 📄 رأس الإصدار في ملف Bytecode
 */
class VersionHeader {
public:
    Version version;            // الإصدار
    uint32_t timestamp;         // وقت الإنشاء (Unix timestamp)
    uint32_t compilerVersion;   // إصدار المترجم المستخدم
    uint8_t flags;              // أعلام إضافية
    
    /**
     * 🏗️ إنشاء رأس جديد
     */
    VersionHeader()
        : version(Version::current()),
          timestamp(0),
          compilerVersion(Version::current().toNumber()),
          flags(0) {}
    
    /**
     * 📊 حجم الرأس بالبايتات
     */
    static constexpr size_t SIZE = 12;
    
    /**
     * ✍️ كتابة الرأس
     */
    void write(uint8_t* output) const {
        output[0] = version.major;
        output[1] = version.minor;
        output[2] = version.patch;
        output[3] = flags;
        *reinterpret_cast<uint32_t*>(&output[4]) = toLittleEndian32(timestamp);
        *reinterpret_cast<uint32_t*>(&output[8]) = toLittleEndian32(compilerVersion);
    }
    
    /**
     * 📖 قراءة الرأس
     */
    static VersionHeader read(const uint8_t* input) {
        VersionHeader header;
        header.version.major = input[0];
        header.version.minor = input[1];
        header.version.patch = input[2];
        header.flags = input[3];
        header.timestamp = *reinterpret_cast<const uint32_t*>(&input[4]);
        header.compilerVersion = *reinterpret_cast<const uint32_t*>(&input[8]);
        return header;
    }
    
    /**
     * 🔍 التحقق من التوافق
     */
    bool isCompatible() const {
        return version.isCompatibleWithCurrent();
    }
    
    /**
     * 📝 وصف تفصيلي
     */
    std::string describe() const {
        std::ostringstream oss;
        
        oss << "═══════════════════════════════════\n";
        oss << "        معلومات الإصدار\n";
        oss << "═══════════════════════════════════\n\n";
        
        oss << "📊 إصدار Bytecode: " << version.toString() << "\n";
        oss << "🏗️ إصدار المترجم: " << (compilerVersion / 10000) << "."
            << ((compilerVersion / 100) % 100) << "."
            << (compilerVersion % 100) << "\n";
        
        if (timestamp > 0) {
            // تحويل timestamp إلى تاريخ (مبسط)
            oss << "📅 وقت الإنشاء: " << timestamp << " (Unix timestamp)\n";
        }
        
        oss << "\n🤝 التوافق: " 
            << (isCompatible() ? "✓ متوافق" : "✗ غير متوافق") << "\n";
        
        if (!isCompatible()) {
            oss << "⚠️ " << version.getIncompatibilityMessage(Version::current()) << "\n";
        }
        
        return oss.str();
    }
};

// ==========================================================
// 📌 فئة VersionChecker (فاحص الإصدارات)
// ==========================================================

/**
 * 🔍 فاحص الإصدارات
 */
class VersionChecker {
public:
    /**
     * 📊 نتيجة الفحص
     */
    struct CheckResult {
        bool compatible;        // هل متوافق؟
        Version fileVersion;    // إصدار الملف
        Version runtimeVersion; // إصدار المفسر
        std::string message;    // رسالة توضيحية
        
        /**
         * 📝 رسالة مفصلة
         */
        std::string detailedMessage() const {
            std::ostringstream oss;
            
            if (compatible) {
                oss << "✓ الملف متوافق\n";
                oss << "  إصدار الملف: " << fileVersion.toString() << "\n";
                oss << "  إصدار المفسر: " << runtimeVersion.toString() << "\n";
            } else {
                oss << "✗ الملف غير متوافق\n";
                oss << "  إصدار الملف: " << fileVersion.toString() << "\n";
                oss << "  إصدار المفسر: " << runtimeVersion.toString() << "\n";
                oss << "  السبب: " << message << "\n";
            }
            
            return oss.str();
        }
    };
    
    /**
     * 🔍 فحص إصدار الملف
     */
    static CheckResult check(const uint8_t* fileHeader, size_t length) {
        CheckResult result;
        result.runtimeVersion = Version::current();
        
        if (length < 3) {
            result.compatible = false;
            result.message = "الرأس قصير جداً";
            return result;
        }
        
        result.fileVersion = Version(fileHeader[0], fileHeader[1], fileHeader[2]);
        result.compatible = result.fileVersion.isCompatibleWithCurrent();
        
        if (!result.compatible) {
            result.message = result.fileVersion.getIncompatibilityMessage(result.runtimeVersion);
        } else {
            result.message = "الإصداران متوافقان";
        }
        
        return result;
    }
    
    /**
     * 🔍 فحص إصدار Version مباشرة
     */
    static CheckResult check(const Version& fileVersion) {
        CheckResult result;
        result.fileVersion = fileVersion;
        result.runtimeVersion = Version::current();
        result.compatible = fileVersion.isCompatibleWithCurrent();
        
        if (!result.compatible) {
            result.message = fileVersion.getIncompatibilityMessage(result.runtimeVersion);
        } else {
            result.message = "الإصداران متوافقان";
        }
        
        return result;
    }
};

// ==========================================================
// 📌 دوال مساعدة عامة
// ==========================================================

/**
 * 📊 الحصول على الإصدار الحالي كـ string
 */
std::string getCurrentVersionString() {
    return Version::current().toString();
}

/**
 * 📊 الحصول على الإصدار الحالي بالعربية
 */
std::string getCurrentVersionArabic() {
    return Version::current().toArabicString();
}

/**
 * 🔍 فحص سريع للتوافق
 */
bool isVersionCompatible(uint8_t major, uint8_t minor, uint8_t patch) {
    Version fileVersion(major, minor, patch);
    return fileVersion.isCompatibleWithCurrent();
}

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء كائن Version جديد
 */
void* sad_version_new(uint8_t major, uint8_t minor, uint8_t patch) {
    return new sad::bytecode::Version(major, minor, patch);
}

/**
 * تحرير كائن Version
 */
void sad_version_free(void* version) {
    delete static_cast<sad::bytecode::Version*>(version);
}

/**
 * الإصدار الحالي كـ string
 */
const char* sad_version_current_string(void) {
    static std::string str = sad::bytecode::getCurrentVersionString();
    return str.c_str();
}

/**
 * الإصدار الحالي بالعربية
 */
const char* sad_version_current_arabic(void) {
    static std::string str = sad::bytecode::getCurrentVersionArabic();
    return str.c_str();
}

/**
 * فحص التوافق
 */
int sad_version_is_compatible(uint8_t major, uint8_t minor, uint8_t patch) {
    return sad::bytecode::isVersionCompatible(major, minor, patch) ? 1 : 0;
}

/**
 * الحصول على MAJOR الحالي
 */
uint8_t sad_version_major(void) {
    return sad::bytecode::VERSION_MAJOR;
}

/**
 * الحصول على MINOR الحالي
 */
uint8_t sad_version_minor(void) {
    return sad::bytecode::VERSION_MINOR;
}

/**
 * الحصول على PATCH الحالي
 */
uint8_t sad_version_patch(void) {
    return sad::bytecode::VERSION_PATCH;
}

/**
 * مقارنة إصدارين
 * يرجع: -1 إذا v1 < v2, 0 إذا متساويين, 1 إذا v1 > v2
 */
int sad_version_compare(uint8_t major1, uint8_t minor1, uint8_t patch1,
                        uint8_t major2, uint8_t minor2, uint8_t patch2) {
    sad::bytecode::Version v1(major1, minor1, patch1);
    sad::bytecode::Version v2(major2, minor2, patch2);
    
    if (v1 < v2) return -1;
    if (v1 > v2) return 1;
    return 0;
}

} // extern "C"
