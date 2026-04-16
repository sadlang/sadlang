/**
 * @file interrupt_handlers.cpp
 * @brief (AR) تنفيذ معالجات المقاطعات للأنظمة المدمجة
 *        (EN) Interrupt handlers implementation for embedded systems
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ███╗   ███╗██╗   ██╗ █████╗ ████████╗ █████╗  █████╗  █████╗ ████████╗   ║
 * ║     ████╗ ████║██║   ██║██╔══██╗╚══██╔══╝██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝   ║
 * ║     ██╔████╔██║██║   ██║███████║   ██║   ███████║███████║███████║   ██║      ║
 * ║     ██║╚██╔╝██║██║   ██║██╔══██║   ██║   ██╔══██║██╔══██║██╔══██║   ██║      ║
 * ║     ██║ ╚═╝ ██║╚██████╔╝██║  ██║   ██║   ██║  ██║██║  ██║██║  ██║   ██║      ║
 * ║     ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝      ║
 * ║                                                                               ║
 * ║                     معالجات المقاطعات - Interrupt Handlers                    ║
 * ║                     لغة ص للأنظمة المدمجة والوقت الحقيقي                       ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يدير معالجات المقاطعات (Interrupt Handlers/ISRs):
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     أنواع المقاطعات (Interrupt Types)                    │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │  ┌─────────────────┐                    ┌─────────────────┐             │
 *      │  │ المقاطعات       │ ←────────────────→ │ الاستثناءات      │             │
 *      │  │ (Interrupts)    │                    │ (Exceptions)    │             │
 *      │  │                 │                    │                 │             │
 *      │  │ • Timer         │                    │ • Division by 0 │             │
 *      │  │ • UART          │                    │ • Page Fault    │             │
 *      │  │ • GPIO          │                    │ • Invalid Op    │             │
 *      │  │ • SPI/I2C       │                    │ • Breakpoint    │             │
 *      │  └─────────────────┘                    └─────────────────┘             │
 *      │           │                                      │                      │
 *      │           └──────────────┬───────────────────────┘                      │
 *      │                          ▼                                              │
 *      │              ┌─────────────────────┐                                    │
 *      │              │  جدول المتجهات      │                                    │
 *      │              │  (Vector Table)     │                                    │
 *      │              │                     │                                    │
 *      │              │  0x00: Reset        │                                    │
 *      │              │  0x04: NMI          │                                    │
 *      │              │  0x08: HardFault    │                                    │
 *      │              │  ...                │                                    │
 *      │              └─────────────────────┘                                    │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 *      الاستخدام في كود ص:
 *      @code
 *      #![بلا_مكتبة_قياسية]
 *      
 *      // معالج مقاطعة المؤقت
 *      #[مقاطعة(رقم = 15)]      // أو #[مقاطعة(اسم = "TIM2_IRQHandler")]
 *      دالة عند_تكة_المؤقت() {
 *          // زيادة العداد
 *          العداد += 1؛
 *          
 *          // مسح علم المقاطعة
 *          مسح_علم_المقاطعة(مقاطعة::مؤقت2)؛
 *      }
 *      
 *      // معالج استثناء القسمة على صفر
 *      #[استثناء(قسمة_صفر)]
 *      دالة عند_قسمة_صفر(سياق: &سياق_استثناء) -> ! {
 *          اطبع_خطأ("قسمة على صفر!")؛
 *          حلقة {}
 *      }
 *      @endcode
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <sstream>
#include <cstdint>

// إصلاح تعارض ماكروات Windows مع أسماء المعددات
// Fix Windows macro conflicts with enum names
#ifdef BREAKPOINT
#undef BREAKPOINT
#endif
#ifdef OVERFLOW
#undef OVERFLOW
#endif
#ifdef DEBUG
#undef DEBUG
#endif

namespace sad {
namespace compiler {
namespace backend {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع المقاطعات
//                    (EN) Interrupt Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum InterruptKind
 * @brief (AR) أنواع المقاطعات المدعومة
 *        (EN) Supported interrupt types
 */
enum class InterruptKind {
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) مقاطعات المعالج (Processor Exceptions)
    // (EN) Processor exceptions
    // ─────────────────────────────────────────────────────────────────────────
    
    RESET,              // (AR) إعادة تشغيل
    NMI,                // (AR) مقاطعة غير قابلة للإخفاء
    HARD_FAULT,         // (AR) خطأ شديد
    MEM_MANAGE,         // (AR) خطأ إدارة الذاكرة
    BUS_FAULT,          // (AR) خطأ الناقل
    USAGE_FAULT,        // (AR) خطأ الاستخدام
    SVCALL,             // (AR) استدعاء المشرف
    DEBUG_MON,          // (AR) مراقب التصحيح
    PENDSV,             // (AR) خدمة معلقة
    SYSTICK,            // (AR) مؤقت النظام
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) مقاطعات خارجية (External Interrupts)
    // (EN) External interrupts
    // ─────────────────────────────────────────────────────────────────────────
    
    EXTERNAL,           // (AR) مقاطعة خارجية عامة
    TIMER,              // (AR) مؤقت
    UART,               // (AR) منفذ تسلسلي
    SPI,                // (AR) واجهة SPI
    I2C,                // (AR) واجهة I2C
    GPIO,               // (AR) مدخل/مخرج عام
    DMA,                // (AR) نقل مباشر للذاكرة
    ADC,                // (AR) محول تناظري-رقمي
    USB,                // (AR) ناقل USB
    CAN,                // (AR) ناقل CAN
    ETHERNET,           // (AR) إيثرنت
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) استثناءات x86
    // (EN) x86 Exceptions
    // ─────────────────────────────────────────────────────────────────────────
    
    DIVIDE_ERROR,       // (AR) قسمة على صفر
    DEBUG,              // (AR) تصحيح
    BREAKPOINT,         // (AR) نقطة توقف
    OVERFLOW,           // (AR) طفحان
    BOUND_RANGE,        // (AR) تجاوز الحدود
    INVALID_OPCODE,     // (AR) تعليمة غير صالحة
    DEVICE_NOT_AVAIL,   // (AR) جهاز غير متاح
    DOUBLE_FAULT,       // (AR) خطأ مزدوج
    INVALID_TSS,        // (AR) TSS غير صالح
    SEGMENT_NOT_PRESENT,// (AR) قطاع غير موجود
    STACK_SEGMENT,      // (AR) خطأ قطاع المكدس
    GENERAL_PROTECTION, // (AR) حماية عامة
    PAGE_FAULT,         // (AR) خطأ صفحة
    X87_FPU,            // (AR) خطأ FPU
    ALIGNMENT_CHECK,    // (AR) فحص المحاذاة
    MACHINE_CHECK,      // (AR) فحص الآلة
    SIMD_FP,            // (AR) خطأ SIMD
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) معلومات المقاطعة
//                    (EN) Interrupt Information
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct InterruptInfo
 * @brief (AR) معلومات معالج المقاطعة
 *        (EN) Interrupt handler information
 */
struct InterruptInfo {
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الهوية
    // (EN) Identity
    // ─────────────────────────────────────────────────────────────────────────
    
    std::string functionName;       // (AR) اسم الدالة في كود ص
    std::string linkName;           // (AR) الاسم في الرابط
    InterruptKind kind;             // (AR) نوع المقاطعة
    int32_t vectorNumber = -1;      // (AR) رقم المتجه (-1 = غير محدد)
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الخصائص
    // (EN) Properties
    // ─────────────────────────────────────────────────────────────────────────
    
    bool isNaked = false;           // (AR) بدون prologue/epilogue
    bool noReturn = false;          // (AR) لا ترجع (للاستثناءات)
    bool savesContext = true;       // (AR) تحفظ السياق تلقائياً
    int32_t priority = 0;           // (AR) الأولوية (للمعالجات التي تدعمها)
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) معاملات سياق الاستثناء (إن وجدت)
    // (EN) Exception context parameters (if any)
    // ─────────────────────────────────────────────────────────────────────────
    
    bool hasErrorCode = false;      // (AR) هل يوجد رمز خطأ؟
    std::string contextType;        // (AR) نوع هيكل السياق
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) سمات المقاطعات العربية
//                    (EN) Arabic Interrupt Attributes
// ═══════════════════════════════════════════════════════════════════════════════

namespace سمات_المقاطعة {
    // (AR) السمات الرئيسية
    const std::string مقاطعة = "مقاطعة";
    const std::string استثناء = "استثناء";
    const std::string معالج_خطأ = "معالج_خطأ";
    
    // (AR) خيارات السمة
    const std::string رقم = "رقم";
    const std::string اسم = "اسم";
    const std::string أولوية = "أولوية";
    const std::string عاري = "عاري";
    const std::string بدون_سياق = "بدون_سياق";
    
    // (AR) أنواع الاستثناءات
    const std::string قسمة_صفر = "قسمة_صفر";
    const std::string خطأ_صفحة = "خطأ_صفحة";
    const std::string حماية_عامة = "حماية_عامة";
    const std::string خطأ_شديد = "خطأ_شديد";
    const std::string تعليمة_غير_صالحة = "تعليمة_غير_صالحة";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) هيكل سياق الاستثناء
//                    (EN) Exception Context Structure
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct ExceptionContext
 * @brief (AR) سياق الاستثناء (السجلات المحفوظة)
 *        (EN) Exception context (saved registers)
 */
struct ExceptionContext {
    // (AR) سجلات x86-64
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip;           // (AR) مؤشر التعليمات
    uint64_t rflags;        // (AR) أعلام المعالج
    uint64_t cs, ss;        // (AR) سجلات القطاع
    uint64_t errorCode;     // (AR) رمز الخطأ (إن وجد)
};

/**
 * @struct ARMExceptionContext
 * @brief (AR) سياق الاستثناء لـ ARM Cortex-M
 *        (EN) ARM Cortex-M exception context
 */
struct ARMExceptionContext {
    // (AR) السجلات المحفوظة تلقائياً بواسطة الأجهزة
    uint32_t r0, r1, r2, r3;
    uint32_t r12;
    uint32_t lr;            // (AR) سجل الرابط
    uint32_t pc;            // (AR) عداد البرنامج
    uint32_t xpsr;          // (AR) سجل حالة البرنامج
    
    // (AR) سجلات إضافية (يحفظها البرنامج)
    uint32_t r4, r5, r6, r7;
    uint32_t r8, r9, r10, r11;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل سمات المقاطعات
//                    (EN) Interrupt Attribute Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InterruptAttributeParser
 * @brief (AR) محلل سمات معالجات المقاطعات
 *        (EN) Interrupt handler attribute parser
 */
class InterruptAttributeParser {
public:
    /**
     * @brief (AR) تحليل سمة مقاطعة
     *        (EN) Parse interrupt attribute
     * 
     * @param attribute (AR) نص السمة (EN) Attribute text
     * @param value (AR) قيمة السمة (EN) Attribute value
     * @param info (AR) معلومات للتحديث (EN) Info to update
     */
    bool parseInterruptAttribute(
        const std::string& attribute,
        const std::string& value,
        InterruptInfo& info
    ) {
        // (AR) سمة #[مقاطعة]
        if (attribute == سمات_المقاطعة::مقاطعة || attribute == "interrupt") {
            info.kind = InterruptKind::EXTERNAL;
            return parseInterruptValue(value, info);
        }
        
        // (AR) سمة #[استثناء]
        if (attribute == سمات_المقاطعة::استثناء || attribute == "exception") {
            return parseExceptionValue(value, info);
        }
        
        // (AR) سمة #[عاري]
        if (attribute == سمات_المقاطعة::عاري || attribute == "naked") {
            info.isNaked = true;
            return true;
        }
        
        // (AR) سمة #[بدون_سياق]
        if (attribute == سمات_المقاطعة::بدون_سياق || attribute == "no_context") {
            info.savesContext = false;
            return true;
        }
        
        return false;
    }
    
private:
    /**
     * @brief (AR) تحليل قيمة سمة المقاطعة
     *        (EN) Parse interrupt attribute value
     */
    bool parseInterruptValue(const std::string& value, InterruptInfo& info) {
        // (AR) البحث عن رقم المتجه
        // (EN) Look for vector number
        size_t numPos = value.find(سمات_المقاطعة::رقم);
        if (numPos == std::string::npos) {
            numPos = value.find("number");
        }
        
        if (numPos != std::string::npos) {
            size_t eqPos = value.find('=', numPos);
            if (eqPos != std::string::npos) {
                std::string numStr;
                for (size_t i = eqPos + 1; i < value.length(); i++) {
                    if (std::isdigit(value[i])) {
                        numStr += value[i];
                    } else if (!std::isspace(value[i]) && numStr.length() > 0) {
                        break;
                    }
                }
                if (!numStr.empty()) {
                    info.vectorNumber = std::stoi(numStr);
                }
            }
        }
        
        // (AR) البحث عن اسم الرابط
        // (EN) Look for link name
        size_t namePos = value.find(سمات_المقاطعة::اسم);
        if (namePos == std::string::npos) {
            namePos = value.find("name");
        }
        
        if (namePos != std::string::npos) {
            size_t startQuote = value.find('"', namePos);
            size_t endQuote = value.find('"', startQuote + 1);
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                info.linkName = value.substr(startQuote + 1, endQuote - startQuote - 1);
            }
        }
        
        // (AR) البحث عن الأولوية
        // (EN) Look for priority
        size_t prioPos = value.find(سمات_المقاطعة::أولوية);
        if (prioPos == std::string::npos) {
            prioPos = value.find("priority");
        }
        
        if (prioPos != std::string::npos) {
            size_t eqPos = value.find('=', prioPos);
            if (eqPos != std::string::npos) {
                std::string prioStr;
                for (size_t i = eqPos + 1; i < value.length(); i++) {
                    if (std::isdigit(value[i]) || value[i] == '-') {
                        prioStr += value[i];
                    } else if (!std::isspace(value[i]) && prioStr.length() > 0) {
                        break;
                    }
                }
                if (!prioStr.empty()) {
                    info.priority = std::stoi(prioStr);
                }
            }
        }
        
        return true;
    }
    
    /**
     * @brief (AR) تحليل قيمة سمة الاستثناء
     *        (EN) Parse exception attribute value
     */
    bool parseExceptionValue(const std::string& value, InterruptInfo& info) {
        info.noReturn = true;  // (AR) معالجات الاستثناءات عادةً لا ترجع
        
        // (AR) تحديد نوع الاستثناء
        if (value.find(سمات_المقاطعة::قسمة_صفر) != std::string::npos ||
            value.find("divide_error") != std::string::npos) {
            info.kind = InterruptKind::DIVIDE_ERROR;
            info.vectorNumber = 0;
            info.hasErrorCode = false;
        }
        else if (value.find(سمات_المقاطعة::خطأ_صفحة) != std::string::npos ||
                 value.find("page_fault") != std::string::npos) {
            info.kind = InterruptKind::PAGE_FAULT;
            info.vectorNumber = 14;
            info.hasErrorCode = true;
        }
        else if (value.find(سمات_المقاطعة::حماية_عامة) != std::string::npos ||
                 value.find("general_protection") != std::string::npos) {
            info.kind = InterruptKind::GENERAL_PROTECTION;
            info.vectorNumber = 13;
            info.hasErrorCode = true;
        }
        else if (value.find(سمات_المقاطعة::خطأ_شديد) != std::string::npos ||
                 value.find("hard_fault") != std::string::npos) {
            info.kind = InterruptKind::HARD_FAULT;
            info.vectorNumber = 3;  // (AR) في ARM
            info.hasErrorCode = false;
        }
        else if (value.find(سمات_المقاطعة::تعليمة_غير_صالحة) != std::string::npos ||
                 value.find("invalid_opcode") != std::string::npos) {
            info.kind = InterruptKind::INVALID_OPCODE;
            info.vectorNumber = 6;
            info.hasErrorCode = false;
        }
        
        return true;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد كود المقاطعات
//                    (EN) Interrupt Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InterruptCodeGenerator
 * @brief (AR) مولّد LLVM IR لمعالجات المقاطعات
 *        (EN) LLVM IR generator for interrupt handlers
 */

} // namespace backend
} // namespace compiler
} // namespace sad

