/**
 * @file interrupt_handlers_codegen.cpp
 * @brief (AR) توليد كود المقاطعات وإدارتها
 * @brief (EN) Interrupt code generation and management
 * تم استخراج هذا الملف من interrupt_handlers.cpp وفقاً لقاعدة CW-05
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <sstream>
#include <cstdint>

// إصلاح تعارض ماكروات Windows مع أسماء المعددات
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
        ir << "; " << "معالج المقاطعة: " << info.functionName << "\n";
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
        ir << "; " << "جدول متجهات المقاطعات" << "\n";
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
        ir << "  ; " << "كود المستخدم المباشر (بدون prologue/epilogue)" << "\n";
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
            ir << "  ; " << "حفظ السجلات الإضافية (r4-r11)" << "\n";
            ir << "  ; Hardware automatically saves r0-r3, r12, lr, pc, xpsr\n";
            ir << "  %ctx = alloca [8 x i32]\n";
            ir << "  call void @__save_arm_context(ptr %ctx)\n\n";
        }
        
        ir << "  ; " << "كود المستخدم" << "\n";
        ir << userCode;
        ir << "\n";
        
        if (info.savesContext) {
            ir << "  ; " << "استعادة السجلات" << "\n";
            ir << "  call void @__restore_arm_context(ptr %ctx)\n";
        }
        
        if (info.noReturn) {
            ir << "  unreachable\n";
        } else {
            ir << "  ; " << "العودة من المقاطعة" << "\n";
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
            ir << "  ; " << "حفظ جميع السجلات" << "\n";
            ir << "  %ctx = alloca %X86ExceptionContext\n";
            ir << "  call void @__save_x86_context(ptr %ctx)\n\n";
        }
        
        ir << "  ; " << "كود المستخدم" << "\n";
        ir << userCode;
        ir << "\n";
        
        if (info.savesContext) {
            ir << "  ; " << "استعادة السجلات" << "\n";
            ir << "  call void @__restore_x86_context(ptr %ctx)\n";
        }
        
        if (info.noReturn) {
            ir << "  unreachable\n";
        } else {
            ir << "  ; " << "العودة من المقاطعة (iret)" << "\n";
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
        
        ir << "; " << "جدول متجهات ARM Cortex-M" << "\n";
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
        ir << "; " << "هيكل واصف IDT" << "\n";
        ir << "%IDTEntry = type { i16, i16, i8, i8, i16 }\n\n";
        
        ir << "; " << "جدول واصفات المقاطعات (IDT)" << "\n";
        ir << "@__idt = global [256 x %IDTEntry] zeroinitializer, section \".idt\"\n\n";
        
        // (AR) دالة تهيئة IDT
        ir << "; " << "دالة تهيئة جدول IDT" << "\n";
        ir << "define void @__init_idt() {\n";
        ir << "entry:\n";
        
        for (const auto& handler : handlers) {
            if (handler.vectorNumber >= 0 && handler.vectorNumber < 256) {
                ir << "  ; " << "تهيئة المتجه " << handler.vectorNumber << "\n";
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
        
        ir << "; " << "المعالج الافتراضي (حلقة لا نهائية)" << "\n";
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
                    "رقم المتجه " + std::to_string(vectorNum) + 
                    " خارج النطاق المسموح (0-255)"
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
        case InterruptKind::RESET: return "إعادة_تشغيل";
        case InterruptKind::NMI: return "مقاطعة_غير_قابلة_للإخفاء";
        case InterruptKind::HARD_FAULT: return "خطأ_شديد";
        case InterruptKind::MEM_MANAGE: return "إدارة_الذاكرة";
        case InterruptKind::BUS_FAULT: return "خطأ_الناقل";
        case InterruptKind::USAGE_FAULT: return "خطأ_الاستخدام";
        case InterruptKind::SYSTICK: return "مؤقت_النظام";
        case InterruptKind::TIMER: return "مؤقت";
        case InterruptKind::UART: return "منفذ_تسلسلي";
        case InterruptKind::GPIO: return "مدخل_مخرج_عام";
        case InterruptKind::DIVIDE_ERROR: return "قسمة_صفر";
        case InterruptKind::PAGE_FAULT: return "خطأ_صفحة";
        case InterruptKind::GENERAL_PROTECTION: return "حماية_عامة";
        default: return "مقاطعة";
    }
}


} // namespace backend
} // namespace compiler
} // namespace sad

