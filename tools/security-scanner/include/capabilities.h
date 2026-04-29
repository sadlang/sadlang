// ======================================================================
// capabilities.h - نظام القدرات / Capability System
// ======================================================================
// الوصف بالعربية:
//   نظام أمان مبني على القدرات لنظام النحلة
//   كل عملية خطيرة تتطلب قدرة محددة
//   مستوحى من seL4 capability-based security
//
// English Description:
//   Capability-based security system for BeeOS
//   Every dangerous operation requires a specific capability
//   Inspired by seL4 capability-based security
// ======================================================================

#ifndef SAD_CAPABILITIES_H
#define SAD_CAPABILITIES_H

#include <string>
#include <vector>
#include <cstdint>

namespace Sad {
namespace Security {

/**
 * @brief Capability types / أنواع القدرات
 * 
 * (AR) كل قدرة تمثل إذن للقيام بعملية معينة
 * (EN) Each capability represents permission for a specific operation
 */
enum class Capability : uint32_t {
    // ========== قدرات الذاكرة / Memory Capabilities ==========
    MemRead     = 0x0001,   ///< قراءة_ذاكرة / memory read
    MemWrite    = 0x0002,   ///< كتابة_ذاكرة / memory write
    MemExec     = 0x0004,   ///< تنفيذ_ذاكرة / memory execute
    MemAlloc    = 0x0008,   ///< تخصيص_ذاكرة / memory allocate
    
    // ========== قدرات الإدخال/الإخراج / I/O Capabilities ==========
    IoPort      = 0x0010,   ///< منفذ_إدخال / I/O port access
    IoFile      = 0x0020,   ///< ملف / file access
    IoNetwork   = 0x0040,   ///< شبكة / network access
    IoDisplay   = 0x0080,   ///< شاشة / display access
    
    // ========== قدرات النظام / System Capabilities ==========
    SysInterrupt = 0x0100,  ///< مقاطعة / interrupt handling
    SysProcess   = 0x0200,  ///< عملية / process management
    SysTime      = 0x0400,  ///< وقت / time access
    SysPower     = 0x0800,  ///< طاقة / power management
    
    // ========== قدرات الأمان / Security Capabilities ==========
    SecCapGrant  = 0x1000,  ///< منح_قدرة / grant capability
    SecCapRevoke = 0x2000,  ///< سحب_قدرة / revoke capability
    SecPriv      = 0x4000,  ///< امتياز / privileged operations
    
    // ========== تجميعات / Combinations ==========
    None         = 0x0000,  ///< لا_شيء / no capabilities
    All          = 0xFFFF   ///< الكل / all capabilities (kernel only)
};

/**
 * @brief Combine capabilities / دمج القدرات
 */
inline Capability operator|(Capability a, Capability b) {
    return static_cast<Capability>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline Capability operator&(Capability a, Capability b) {
    return static_cast<Capability>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool hasCapability(Capability set, Capability cap) {
    return (static_cast<uint32_t>(set) & static_cast<uint32_t>(cap)) != 0;
}

/**
 * @brief Capability token / رمز القدرة
 * 
 * (AR) رمز يمثل مجموعة من القدرات الممنوحة لوحدة معينة
 * (EN) Token representing a set of capabilities granted to a module
 * 
 * @example صيغة ص:
 * - #[قدرات(قراءة_ذاكرة، كتابة_ذاكرة)]
 * - وحدة مدير_الذاكرة { ... }
 */
struct CapabilityToken {
    std::string moduleName;     ///< Module name / اسم الوحدة
    Capability capabilities;    ///< Granted capabilities / القدرات الممنوحة
    bool isKernel;              ///< Kernel module? / وحدة نواة؟
    
    CapabilityToken(const std::string& mod, Capability caps, bool kernel = false)
        : moduleName(mod), capabilities(caps), isKernel(kernel) {}
    
    bool hasCapability(Capability cap) const {
        return Security::hasCapability(capabilities, cap);
    }
};

/**
 * @brief Capability checker / فاحص القدرات
 * 
 * (AR) يفحص في وقت الترجمة أن كل وحدة تستخدم فقط القدرات الممنوحة لها
 * (EN) Checks at compile time that each module only uses granted capabilities
 */
class CapabilityChecker {
public:
    CapabilityChecker() = default;
    
    /**
     * @brief Register module capabilities / تسجيل قدرات الوحدة
     */
    void registerModule(const CapabilityToken& token) {
        tokens_.push_back(token);
    }
    
    /**
     * @brief Check if module has capability / فحص قدرة الوحدة
     */
    bool checkModuleCapability(const std::string& module, Capability cap) const {
        for (const auto& token : tokens_) {
            if (token.moduleName == module) {
                return token.hasCapability(cap);
            }
        }
        return false;
    }
    
    /**
     * @brief Get required capability for operation
     * (AR) الحصول على القدرة المطلوبة لعملية معينة
     */
    static Capability requiredCapability(const std::string& operation) {
        if (operation == "قراءة_مؤشر" || operation == "ptr_read") 
            return Capability::MemRead;
        if (operation == "كتابة_مؤشر" || operation == "ptr_write") 
            return Capability::MemWrite;
        if (operation == "تخصيص" || operation == "alloc") 
            return Capability::MemAlloc;
        if (operation == "منفذ_إدخال" || operation == "io_port") 
            return Capability::IoPort;
        if (operation == "مقاطعة" || operation == "interrupt") 
            return Capability::SysInterrupt;
        if (operation == "شبكة" || operation == "network") 
            return Capability::IoNetwork;
        return Capability::None;
    }
    
    /**
     * @brief Get Arabic name for capability / الاسم العربي للقدرة
     */
    static std::string capabilityArabicName(Capability cap) {
        if (hasCapability(cap, Capability::MemRead)) return "قراءة_ذاكرة";
        if (hasCapability(cap, Capability::MemWrite)) return "كتابة_ذاكرة";
        if (hasCapability(cap, Capability::MemExec)) return "تنفيذ_ذاكرة";
        if (hasCapability(cap, Capability::MemAlloc)) return "تخصيص_ذاكرة";
        if (hasCapability(cap, Capability::IoPort)) return "منفذ_إدخال";
        if (hasCapability(cap, Capability::IoNetwork)) return "شبكة";
        if (hasCapability(cap, Capability::SysInterrupt)) return "مقاطعة";
        return "مجهول";
    }
    
private:
    std::vector<CapabilityToken> tokens_;
};

} // namespace Security
} // namespace Sad

#endif // SAD_CAPABILITIES_H
