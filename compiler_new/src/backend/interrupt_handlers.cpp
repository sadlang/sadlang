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
    const std::string مقاطعة = u8"مقاطعة";
    const std::string استثناء = u8"استثناء";
    const std::string معالج_خطأ = u8"معالج_خطأ";
    
    // (AR) خيارات السمة
    const std::string رقم = u8"رقم";
    const std::string اسم = u8"اسم";
    const std::string أولوية = u8"أولوية";
    const std::string عاري = u8"عاري";
    const std::string بدون_سياق = u8"بدون_سياق";
    
    // (AR) أنواع الاستثناءات
    const std::string قسمة_صفر = u8"قسمة_صفر";
    const std::string خطأ_صفحة = u8"خطأ_صفحة";
    const std::string حماية_عامة = u8"حماية_عامة";
    const std::string خطأ_شديد = u8"خطأ_شديد";
    const std::string تعليمة_غير_صالحة = u8"تعليمة_غير_صالحة";
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
class InterruptCodeGenerator {
public:
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        targetArch_ = arch;
    }
    
    /**
     * @brief (AR) توليد كود معالج المقاطعة
     *        (EN) Generate interrupt handler code
     * 
     * @param info (AR) معلومات المعالج (EN) Handler info
     * @param userCode (AR) كود المستخدم (EN) User code
     * @return (AR) كود LLVM IR (EN) LLVM IR code
     */
    std::string generateInterruptHandler(
        const InterruptInfo& info,
        const std::string& userCode
    ) {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"معالج المقاطعة: " << info.functionName << "\n";
        ir << "; Interrupt Handler: " << info.functionName << "\n";
        ir << "; Vector: " << info.vectorNumber << "\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        if (info.isNaked) {
            return generateNakedHandler(info, userCode);
        }
        
        if (isARM()) {
            return generateARMHandler(info, userCode);
        } else {
            return generateX86Handler(info, userCode);
        }
    }
    
    /**
     * @brief (AR) توليد جدول المتجهات
     *        (EN) Generate vector table
     */
    std::string generateVectorTable(
        const std::vector<InterruptInfo>& handlers,
        const std::string& defaultHandler
    ) {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"جدول متجهات المقاطعات" << "\n";
        ir << "; Interrupt Vector Table\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        if (isARM()) {
            return generateARMVectorTable(handlers, defaultHandler);
        } else {
            return generateX86VectorTable(handlers, defaultHandler);
        }
    }
    
private:
    std::string targetArch_ = "x86_64";
    
    bool isARM() const {
        return targetArch_.find("arm") != std::string::npos ||
               targetArch_.find("thumb") != std::string::npos ||
               targetArch_.find("aarch") != std::string::npos;
    }
    
    /**
     * @brief (AR) توليد معالج عاري (naked)
     *        (EN) Generate naked handler
     */
    std::string generateNakedHandler(
        const InterruptInfo& info,
        const std::string& userCode
    ) {
        std::ostringstream ir;
        
        std::string attrs = "naked noinline";
        if (info.noReturn) {
            attrs += " noreturn";
        }
        
        ir << "define void @" << info.linkName << "() " << attrs << " {\n";
        ir << "entry:\n";
        ir << "  ; " << u8"كود المستخدم المباشر (بدون prologue/epilogue)" << "\n";
        ir << userCode;
        
        if (info.noReturn) {
            ir << "  unreachable\n";
        } else {
            ir << "  ret void\n";
        }
        
        ir << "}\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد معالج ARM Cortex-M
     *        (EN) Generate ARM Cortex-M handler
     */
    std::string generateARMHandler(
        const InterruptInfo& info,
        const std::string& userCode
    ) {
        std::ostringstream ir;
        
        // (AR) نوع سياق ARM
        ir << "%ARMExceptionContext = type { ";
        ir << "i32, i32, i32, i32, ";  // r0-r3
        ir << "i32, i32, i32, i32 ";    // r12, lr, pc, xpsr
        ir << "}\n\n";
        
        // (AR) الدالة
        std::string attrs = "noinline";
        if (info.noReturn) {
            attrs += " noreturn";
        }
        
        ir << "define void @" << info.linkName << "()";
        ir << " " << attrs << " section \".text.isr\" {\n";
        ir << "entry:\n";
        
        if (info.savesContext) {
            ir << "  ; " << u8"حفظ السجلات الإضافية (r4-r11)" << "\n";
            ir << "  ; Hardware automatically saves r0-r3, r12, lr, pc, xpsr\n";
            ir << "  %ctx = alloca [8 x i32]\n";
            ir << "  call void @__save_arm_context(ptr %ctx)\n\n";
        }
        
        ir << "  ; " << u8"كود المستخدم" << "\n";
        ir << userCode;
        ir << "\n";
        
        if (info.savesContext) {
            ir << "  ; " << u8"استعادة السجلات" << "\n";
            ir << "  call void @__restore_arm_context(ptr %ctx)\n";
        }
        
        if (info.noReturn) {
            ir << "  unreachable\n";
        } else {
            ir << "  ; " << u8"العودة من المقاطعة" << "\n";
            ir << "  ret void\n";
        }
        
        ir << "}\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد معالج x86/x86-64
     *        (EN) Generate x86/x86-64 handler
     */
    std::string generateX86Handler(
        const InterruptInfo& info,
        const std::string& userCode
    ) {
        std::ostringstream ir;
        
        // (AR) نوع سياق x86
        ir << "%X86ExceptionContext = type { ";
        ir << "i64, i64, i64, i64, ";     // rax, rbx, rcx, rdx
        ir << "i64, i64, i64, i64, ";     // rsi, rdi, rbp, rsp
        ir << "i64, i64, i64, i64, ";     // r8-r11
        ir << "i64, i64, i64, i64, ";     // r12-r15
        ir << "i64, i64, ";               // rip, rflags
        ir << "i64, i64";                 // cs, ss
        if (info.hasErrorCode) {
            ir << ", i64";                // error code
        }
        ir << " }\n\n";
        
        // (AR) الدالة
        std::string attrs = "noinline";
        if (info.noReturn) {
            attrs += " noreturn";
        }
        
        ir << "define void @" << info.linkName << "()";
        ir << " " << attrs << " section \".text.isr\" {\n";
        ir << "entry:\n";
        
        if (info.savesContext) {
            ir << "  ; " << u8"حفظ جميع السجلات" << "\n";
            ir << "  %ctx = alloca %X86ExceptionContext\n";
            ir << "  call void @__save_x86_context(ptr %ctx)\n\n";
        }
        
        ir << "  ; " << u8"كود المستخدم" << "\n";
        ir << userCode;
        ir << "\n";
        
        if (info.savesContext) {
            ir << "  ; " << u8"استعادة السجلات" << "\n";
            ir << "  call void @__restore_x86_context(ptr %ctx)\n";
        }
        
        if (info.noReturn) {
            ir << "  unreachable\n";
        } else {
            ir << "  ; " << u8"العودة من المقاطعة (iret)" << "\n";
            ir << "  call void @llvm.x86.iret()\n";
            ir << "  ret void\n";
        }
        
        ir << "}\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد جدول متجهات ARM
     *        (EN) Generate ARM vector table
     */
    std::string generateARMVectorTable(
        const std::vector<InterruptInfo>& handlers,
        const std::string& defaultHandler
    ) {
        std::ostringstream ir;
        
        ir << "; " << u8"جدول متجهات ARM Cortex-M" << "\n";
        ir << "@__vectors = global [256 x ptr] [\n";
        
        // (AR) أول 16 متجه هي استثناءات النظام
        std::vector<std::string> systemVectors = {
            "@__stack_top",             // 0: Initial Stack Pointer
            "@Reset_Handler",           // 1: Reset
            "@NMI_Handler",             // 2: NMI
            "@HardFault_Handler",       // 3: Hard Fault
            "@MemManage_Handler",       // 4: Memory Management
            "@BusFault_Handler",        // 5: Bus Fault
            "@UsageFault_Handler",      // 6: Usage Fault
            "null", "null", "null", "null",  // 7-10: Reserved
            "@SVC_Handler",             // 11: SVCall
            "@DebugMon_Handler",        // 12: Debug Monitor
            "null",                     // 13: Reserved
            "@PendSV_Handler",          // 14: PendSV
            "@SysTick_Handler"          // 15: SysTick
        };
        
        for (size_t i = 0; i < 256; i++) {
            ir << "  ";
            
            if (i < systemVectors.size()) {
                ir << "ptr " << systemVectors[i];
            } else {
                // (AR) البحث عن معالج مخصص
                bool found = false;
                for (const auto& handler : handlers) {
                    if (handler.vectorNumber == static_cast<int32_t>(i)) {
                        ir << "ptr @" << handler.linkName;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    ir << "ptr @" << defaultHandler;
                }
            }
            
            if (i < 255) {
                ir << ",";
            }
            
            ir << "  ; " << i << "\n";
        }
        
        ir << "], section \".isr_vector\"\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد جدول متجهات x86
     *        (EN) Generate x86 vector table (IDT)
     */
    std::string generateX86VectorTable(
        const std::vector<InterruptInfo>& handlers,
        const std::string& defaultHandler
    ) {
        std::ostringstream ir;
        
        // (AR) هيكل واصف IDT
        ir << "; " << u8"هيكل واصف IDT" << "\n";
        ir << "%IDTEntry = type { i16, i16, i8, i8, i16 }\n\n";
        
        ir << "; " << u8"جدول واصفات المقاطعات (IDT)" << "\n";
        ir << "@__idt = global [256 x %IDTEntry] zeroinitializer, section \".idt\"\n\n";
        
        // (AR) دالة تهيئة IDT
        ir << "; " << u8"دالة تهيئة جدول IDT" << "\n";
        ir << "define void @__init_idt() {\n";
        ir << "entry:\n";
        
        for (const auto& handler : handlers) {
            if (handler.vectorNumber >= 0 && handler.vectorNumber < 256) {
                ir << "  ; " << u8"تهيئة المتجه " << handler.vectorNumber << "\n";
                ir << "  call void @__set_idt_entry(i32 " << handler.vectorNumber;
                ir << ", ptr @" << handler.linkName;
                ir << ", i8 0x8E)  ; Present, DPL=0, Interrupt Gate\n";
            }
        }
        
        ir << "  ret void\n";
        ir << "}\n\n";
        
        return ir.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير المقاطعات
//                    (EN) Interrupt Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InterruptManager
 * @brief (AR) مدير معالجات المقاطعات
 *        (EN) Interrupt handlers manager
 */
class InterruptManager {
public:
    InterruptManager() = default;
    
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        codeGen_.setTargetArch(arch);
        targetArch_ = arch;
    }
    
    /**
     * @brief (AR) تسجيل معالج مقاطعة
     *        (EN) Register interrupt handler
     */
    void registerHandler(const InterruptInfo& info) {
        handlers_[info.vectorNumber] = info;
    }
    
    /**
     * @brief (AR) الحصول على معالج بحسب رقم المتجه
     *        (EN) Get handler by vector number
     */
    std::optional<InterruptInfo> getHandler(int32_t vectorNumber) const {
        auto it = handlers_.find(vectorNumber);
        if (it != handlers_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * @brief (AR) الحصول على جميع المعالجات
     *        (EN) Get all handlers
     */
    std::vector<InterruptInfo> getAllHandlers() const {
        std::vector<InterruptInfo> result;
        for (const auto& [num, info] : handlers_) {
            result.push_back(info);
        }
        return result;
    }
    
    /**
     * @brief (AR) توليد كود جميع المعالجات
     *        (EN) Generate code for all handlers
     */
    std::string generateAllHandlerCode(
        const std::map<int32_t, std::string>& userCode
    ) {
        std::ostringstream ir;
        
        for (const auto& [vectorNum, info] : handlers_) {
            auto codeIt = userCode.find(vectorNum);
            std::string code = (codeIt != userCode.end()) ? codeIt->second : "";
            ir << codeGen_.generateInterruptHandler(info, code);
        }
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد جدول المتجهات
     *        (EN) Generate vector table
     */
    std::string generateVectorTable() {
        return codeGen_.generateVectorTable(
            getAllHandlers(),
            "Default_Handler"
        );
    }
    
    /**
     * @brief (AR) توليد معالج افتراضي
     *        (EN) Generate default handler
     */
    std::string generateDefaultHandler() {
        std::ostringstream ir;
        
        ir << "; " << u8"المعالج الافتراضي (حلقة لا نهائية)" << "\n";
        ir << "; Default handler (infinite loop)\n";
        ir << "define void @Default_Handler() noreturn noinline section \".text.isr\" {\n";
        ir << "entry:\n";
        ir << "  br label %loop\n";
        ir << "loop:\n";
        ir << "  br label %loop\n";
        ir << "}\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) التحقق من صحة المعالجات
     *        (EN) Validate handlers
     */
    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        // (AR) التحقق من تعارض أرقام المتجهات
        // (هذا لا يحدث بسبب استخدام map، لكن نتحقق من النطاق)
        for (const auto& [vectorNum, info] : handlers_) {
            if (vectorNum < 0 || vectorNum >= 256) {
                errors.push_back(
                    u8"رقم المتجه " + std::to_string(vectorNum) + 
                    u8" خارج النطاق المسموح (0-255)"
                );
            }
        }
        
        return errors;
    }
    
private:
    std::map<int32_t, InterruptInfo> handlers_;
    InterruptCodeGenerator codeGen_;
    std::string targetArch_ = "x86_64";
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) دوال مساعدة
//                    (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء معلومات معالج مقاطعة
 *        (EN) Create interrupt handler info
 */
InterruptInfo createInterruptHandlerInfo(
    const std::string& functionName,
    int32_t vectorNumber,
    InterruptKind kind
) {
    InterruptInfo info;
    info.functionName = functionName;
    info.linkName = functionName + "_Handler";
    info.vectorNumber = vectorNumber;
    info.kind = kind;
    info.savesContext = true;
    
    return info;
}

/**
 * @brief (AR) إنشاء معلومات معالج استثناء
 *        (EN) Create exception handler info
 */
InterruptInfo createExceptionHandlerInfo(
    const std::string& functionName,
    InterruptKind kind,
    bool hasErrorCode
) {
    InterruptInfo info;
    info.functionName = functionName;
    info.kind = kind;
    info.hasErrorCode = hasErrorCode;
    info.noReturn = true;
    info.savesContext = true;
    
    // (AR) تحديد رقم المتجه بناءً على النوع
    switch (kind) {
        case InterruptKind::DIVIDE_ERROR:
            info.vectorNumber = 0;
            info.linkName = "DivideError_Handler";
            break;
        case InterruptKind::PAGE_FAULT:
            info.vectorNumber = 14;
            info.linkName = "PageFault_Handler";
            break;
        case InterruptKind::GENERAL_PROTECTION:
            info.vectorNumber = 13;
            info.linkName = "GeneralProtection_Handler";
            break;
        case InterruptKind::DOUBLE_FAULT:
            info.vectorNumber = 8;
            info.linkName = "DoubleFault_Handler";
            break;
        default:
            info.linkName = functionName + "_Handler";
            break;
    }
    
    return info;
}

/**
 * @brief (AR) الحصول على اسم نوع المقاطعة بالعربية
 *        (EN) Get interrupt type name in Arabic
 */
std::string getInterruptKindNameArabic(InterruptKind kind) {
    switch (kind) {
        case InterruptKind::RESET: return u8"إعادة_تشغيل";
        case InterruptKind::NMI: return u8"مقاطعة_غير_قابلة_للإخفاء";
        case InterruptKind::HARD_FAULT: return u8"خطأ_شديد";
        case InterruptKind::MEM_MANAGE: return u8"إدارة_الذاكرة";
        case InterruptKind::BUS_FAULT: return u8"خطأ_الناقل";
        case InterruptKind::USAGE_FAULT: return u8"خطأ_الاستخدام";
        case InterruptKind::SYSTICK: return u8"مؤقت_النظام";
        case InterruptKind::TIMER: return u8"مؤقت";
        case InterruptKind::UART: return u8"منفذ_تسلسلي";
        case InterruptKind::GPIO: return u8"مدخل_مخرج_عام";
        case InterruptKind::DIVIDE_ERROR: return u8"قسمة_صفر";
        case InterruptKind::PAGE_FAULT: return u8"خطأ_صفحة";
        case InterruptKind::GENERAL_PROTECTION: return u8"حماية_عامة";
        default: return u8"مقاطعة";
    }
}

} // namespace backend
} // namespace compiler
} // namespace sad
