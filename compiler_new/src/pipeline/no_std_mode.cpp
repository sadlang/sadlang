/**
 * @file no_std_mode.cpp
 * @brief (AR) تنفيذ وضع #![بلا_مكتبة_قياسية] لبرمجة Bare-metal
 *        (EN) Implementation of #![no_std] mode for Bare-metal programming
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ██████╗  █████╗ ██████╗ ███████╗    ███╗   ███╗███████╗████████╗ █████╗ ██╗║
 * ║     ██╔══██╗██╔══██╗██╔══██╗██╔════╝    ████╗ ████║██╔════╝╚══██╔══╝██╔══██╗██║║
 * ║     ██████╔╝███████║██████╔╝█████╗      ██╔████╔██║█████╗     ██║   ███████║██║║
 * ║     ██╔══██╗██╔══██║██╔══██╗██╔══╝      ██║╚██╔╝██║██╔══╝     ██║   ██╔══██║██║║
 * ║     ██████╔╝██║  ██║██║  ██║███████╗    ██║ ╚═╝ ██║███████╗   ██║   ██║  ██║███║
 * ║     ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝    ╚═╝     ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚══╝
 * ║                                                                               ║
 * ║                     وضع بلا مكتبة قياسية - Bare Metal                          ║
 * ║                     لغة ص لبرمجة الأنظمة المدمجة                               ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يُمكّن لغة ص من العمل بدون مكتبة قياسية، وهو ضروري لـ:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │ 🖥️ كتابة أنظمة التشغيل (OS Kernels)                                    │
 *      │    - نواة نظام تشغيل عربية كاملة                                       │
 *      │    - bootloaders ومحملات الإقلاع                                        │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │ 🔌 برمجة المتحكمات الدقيقة (Microcontrollers)                           │
 *      │    - Arduino, STM32, ESP32                                              │
 *      │    - برامج التحكم الصناعي                                              │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │ 🚀 برامج UEFI و Bootloaders                                             │
 *      │    - برامج إقلاع مخصصة                                                 │
 *      │    - تطبيقات UEFI                                                       │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │ 📟 الأنظمة المدمجة (Embedded Systems)                                   │
 *      │    - أجهزة IoT                                                         │
 *      │    - أنظمة التحكم في الوقت الحقيقي                                     │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 *      الاستخدام في كود ص:
 *      @code
 *      #![بلا_مكتبة_قياسية]      // تعطيل المكتبة القياسية
 *      #![بلا_رئيسية]            // بدون دالة main الافتراضية
 *      
 *      #[معالج_ذعر]              // معالج الأخطاء الحرجة
 *      دالة عند_الذعر(معلومات: &معلومات_ذعر) -> ! {
 *          حلقة {}
 *      }
 *      
 *      #[نقطة_دخول]              // نقطة دخول مخصصة
 *      دالة _start() -> ! {
 *          // كود بدء التشغيل
 *          حلقة {}
 *      }
 *      @endcode
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2026-01-24
 * @version 1.0.0
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>

namespace sad {
namespace compiler {
namespace pipeline {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) ثوابت السمات العربية
//                    (EN) Arabic Attribute Constants
// ═══════════════════════════════════════════════════════════════════════════════

namespace سمات {
    // (AR) سمة تعطيل المكتبة القياسية
    // (EN) No standard library attribute
    const std::string بلا_مكتبة_قياسية = "بلا_مكتبة_قياسية";
    const std::string NO_STD = "no_std";
    
    // (AR) سمة تعطيل نقطة الدخول الافتراضية
    // (EN) No main attribute
    const std::string بلا_رئيسية = "بلا_رئيسية";
    const std::string NO_MAIN = "no_main";
    
    // (AR) سمة نقطة الدخول المخصصة
    // (EN) Custom entry point attribute
    const std::string نقطة_دخول = "نقطة_دخول";
    const std::string ENTRY_POINT = "entry_point";
    
    // (AR) سمة معالج الذعر
    // (EN) Panic handler attribute
    const std::string معالج_ذعر = "معالج_ذعر";
    const std::string PANIC_HANDLER = "panic_handler";
    
    // (AR) سمة معالج التخصيص
    // (EN) Alloc error handler attribute
    const std::string معالج_تخصيص = "معالج_تخصيص";
    const std::string ALLOC_ERROR_HANDLER = "alloc_error_handler";
    
    // (AR) سمة تعطيل فك المكدس
    // (EN) Abort on panic attribute
    const std::string إيقاف_عند_ذعر = "إيقاف_عند_ذعر";
    const std::string ABORT_ON_PANIC = "abort_on_panic";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) إعدادات وضع بلا مكتبة قياسية
//                    (EN) No-std Mode Configuration
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct NoStdConfig
 * @brief (AR) إعدادات وضع بلا مكتبة قياسية
 *        (EN) No-std mode configuration
 */
struct NoStdConfig {
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الإعدادات الأساسية
    // (EN) Basic settings
    // ─────────────────────────────────────────────────────────────────────────
    
    bool noStdEnabled = false;          // (AR) هل تم تفعيل no_std؟
    bool noMainEnabled = false;         // (AR) هل تم تعطيل main؟
    bool abortOnPanic = false;          // (AR) إيقاف مباشر عند الذعر؟
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الدوال المطلوبة
    // (EN) Required functions
    // ─────────────────────────────────────────────────────────────────────────
    
    std::string entryPoint;             // (AR) اسم نقطة الدخول المخصصة
    std::string panicHandler;           // (AR) اسم معالج الذعر
    std::string allocErrorHandler;      // (AR) اسم معالج أخطاء التخصيص
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الميزات المسموحة
    // (EN) Allowed features
    // ─────────────────────────────────────────────────────────────────────────
    
    bool allowAlloc = false;            // (AR) السماح بالتخصيص الديناميكي
    bool allowFloat = true;             // (AR) السماح بأعداد الفاصلة العائمة
    bool allowAtomics = true;           // (AR) السماح بالعمليات الذرية
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الهدف
    // (EN) Target
    // ─────────────────────────────────────────────────────────────────────────
    
    std::string targetTriple;           // (AR) الهدف الثلاثي (مثل: x86_64-unknown-none)
    std::string linkerScript;           // (AR) مسار سكريبت الرابط
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع العناصر المطلوبة
//                    (EN) Required Item Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum RequiredItemKind
 * @brief (AR) أنواع العناصر المطلوبة في وضع no_std
 *        (EN) Required item types in no_std mode
 */
enum class RequiredItemKind {
    ENTRY_POINT,        // (AR) نقطة الدخول (EN) Entry point
    PANIC_HANDLER,      // (AR) معالج الذعر (EN) Panic handler
    ALLOC_ERROR,        // (AR) معالج خطأ التخصيص (EN) Alloc error handler
};

/**
 * @struct RequiredItem
 * @brief (AR) عنصر مطلوب في وضع no_std
 *        (EN) Required item in no_std mode
 */
struct RequiredItem {
    RequiredItemKind kind;              // (AR) نوع العنصر
    std::string expectedSignature;      // (AR) التوقيع المتوقع
    std::string arabicName;             // (AR) الاسم بالعربية
    std::string englishName;            // (AR) الاسم بالإنجليزية
    bool isProvided = false;            // (AR) هل تم توفيره؟
    std::string providedBy;             // (AR) اسم الدالة الموفرة
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل سمات no_std
//                    (EN) No-std Attribute Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class NoStdAttributeParser
 * @brief (AR) محلل سمات وضع بلا مكتبة قياسية
 *        (EN) No-std mode attribute parser
 */
class NoStdAttributeParser {
public:
    /**
     * @brief (AR) تحليل سمة crate-level
     *        (EN) Parse crate-level attribute
     * 
     * @param attributeName (AR) اسم السمة (EN) Attribute name
     * @param config (AR) الإعدادات للتحديث (EN) Config to update
     * @return (AR) هل تم التعرف على السمة؟ (EN) Was attribute recognized?
     */
    bool parseCrateAttribute(const std::string& attributeName, NoStdConfig& config) {
        // (AR) التحقق من السمات المعروفة
        // (EN) Check known attributes
        
        if (attributeName == سمات::بلا_مكتبة_قياسية || 
            attributeName == سمات::NO_STD) {
            config.noStdEnabled = true;
            return true;
        }
        
        if (attributeName == سمات::بلا_رئيسية || 
            attributeName == سمات::NO_MAIN) {
            config.noMainEnabled = true;
            return true;
        }
        
        if (attributeName == سمات::إيقاف_عند_ذعر || 
            attributeName == سمات::ABORT_ON_PANIC) {
            config.abortOnPanic = true;
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief (AR) تحليل سمة دالة
     *        (EN) Parse function attribute
     * 
     * @param attributeName (AR) اسم السمة (EN) Attribute name
     * @param functionName (AR) اسم الدالة (EN) Function name
     * @param config (AR) الإعدادات للتحديث (EN) Config to update
     * @return (AR) هل تم التعرف على السمة؟ (EN) Was attribute recognized?
     */
    bool parseFunctionAttribute(
        const std::string& attributeName,
        const std::string& functionName,
        NoStdConfig& config
    ) {
        if (attributeName == سمات::نقطة_دخول || 
            attributeName == سمات::ENTRY_POINT) {
            config.entryPoint = functionName;
            return true;
        }
        
        if (attributeName == سمات::معالج_ذعر || 
            attributeName == سمات::PANIC_HANDLER) {
            config.panicHandler = functionName;
            return true;
        }
        
        if (attributeName == سمات::معالج_تخصيص || 
            attributeName == سمات::ALLOC_ERROR_HANDLER) {
            config.allocErrorHandler = functionName;
            return true;
        }
        
        return false;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدقق وضع no_std
//                    (EN) No-std Mode Validator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct ValidationError
 * @brief (AR) خطأ تحقق
 *        (EN) Validation error
 */
struct ValidationError {
    std::string code;           // (AR) رمز الخطأ (EN) Error code
    std::string arabicMessage;  // (AR) الرسالة بالعربية
    std::string englishMessage; // (AR) الرسالة بالإنجليزية
    std::string hint;           // (AR) تلميح للإصلاح
};

/**
 * @class NoStdValidator
 * @brief (AR) مدقق متطلبات وضع no_std
 *        (EN) No-std mode requirements validator
 */
class NoStdValidator {
public:
    /**
     * @brief (AR) التحقق من الإعدادات
     *        (EN) Validate configuration
     * 
     * @param config (AR) الإعدادات للتحقق منها (EN) Config to validate
     * @return (AR) قائمة الأخطاء (EN) List of errors
     */
    std::vector<ValidationError> validate(const NoStdConfig& config) {
        std::vector<ValidationError> errors;
        
        if (!config.noStdEnabled) {
            // (AR) لا حاجة للتحقق إذا لم يكن no_std مفعلاً
            return errors;
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) التحقق من نقطة الدخول
        // (EN) Verify entry point
        // ─────────────────────────────────────────────────────────────────────
        
        if (config.noMainEnabled && config.entryPoint.empty()) {
            errors.push_back({
                "ص-EMB-001",
                "وضع بلا_رئيسية يتطلب تحديد نقطة_دخول",
                "no_main mode requires entry_point to be specified",
                "أضف #[نقطة_دخول] لدالة البداية"
            });
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) التحقق من معالج الذعر
        // (EN) Verify panic handler
        // ─────────────────────────────────────────────────────────────────────
        
        if (config.panicHandler.empty() && !config.abortOnPanic) {
            errors.push_back({
                "ص-EMB-002",
                "وضع بلا_مكتبة_قياسية يتطلب معالج_ذعر أو إيقاف_عند_ذعر",
                "no_std mode requires panic_handler or abort_on_panic",
                "أضف #[معالج_ذعر] لدالة معالجة الذعر أو #![إيقاف_عند_ذعر]"
            });
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) التحقق من معالج التخصيص (إذا كان التخصيص مسموحاً)
        // (EN) Verify alloc error handler (if alloc is allowed)
        // ─────────────────────────────────────────────────────────────────────
        
        if (config.allowAlloc && config.allocErrorHandler.empty()) {
            errors.push_back({
                "ص-EMB-003",
                "استخدام التخصيص الديناميكي يتطلب معالج_تخصيص",
                "Dynamic allocation requires alloc_error_handler",
                "أضف #[معالج_تخصيص] أو عطّل التخصيص الديناميكي"
            });
        }
        
        return errors;
    }
    
    /**
     * @brief (AR) التحقق من توقيع نقطة الدخول
     *        (EN) Validate entry point signature
     * 
     * @param signature (AR) التوقيع الفعلي (EN) Actual signature
     * @return (AR) هل التوقيع صحيح؟ (EN) Is signature valid?
     */
    bool validateEntryPointSignature(const std::string& signature) {
        // (AR) التوقيع المتوقع: دالة _start() -> !
        // (EN) Expected signature: fn _start() -> !
        
        // (AR) يجب أن تُرجع never type (!)
        // (EN) Must return never type (!)
        return signature.find("-> !") != std::string::npos ||
               signature.find("-> أبداً") != std::string::npos;
    }
    
    /**
     * @brief (AR) التحقق من توقيع معالج الذعر
     *        (EN) Validate panic handler signature
     */
    bool validatePanicHandlerSignature(const std::string& signature) {
        // (AR) التوقيع المتوقع: دالة(معلومات: &معلومات_ذعر) -> !
        // (EN) Expected: fn(&PanicInfo) -> !
        
        return (signature.find("معلومات_ذعر") != std::string::npos ||
                signature.find("PanicInfo") != std::string::npos) &&
               (signature.find("-> !") != std::string::npos ||
                signature.find("-> أبداً") != std::string::npos);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد كود no_std
//                    (EN) No-std Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class NoStdCodeGenerator
 * @brief (AR) مولّد كود خاص بـ no_std
 *        (EN) No-std specific code generator
 */
class NoStdCodeGenerator {
public:
    /**
     * @brief (AR) توليد كود بدء التشغيل
     *        (EN) Generate startup code
     * 
     * @param config (AR) الإعدادات (EN) Configuration
     * @return (AR) كود LLVM IR (EN) LLVM IR code
     */
    std::string generateStartupCode(const NoStdConfig& config) {
        std::ostringstream ir;
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) تعريف نقطة الدخول
        // (EN) Define entry point
        // ─────────────────────────────────────────────────────────────────────
        
        if (!config.entryPoint.empty()) {
            ir << "; " << "نقطة الدخول المخصصة" << "\n";
            ir << "; Custom entry point\n";
            ir << "define void @_start() {\n";
            ir << "entry:\n";
            
            // (AR) استدعاء نقطة الدخول المخصصة
            // (EN) Call custom entry point
            ir << "  call void @" << config.entryPoint << "()\n";
            
            // (AR) حلقة لا نهائية (يجب ألا نصل هنا)
            // (EN) Infinite loop (should not reach here)
            ir << "  br label %halt\n";
            ir << "halt:\n";
            ir << "  br label %halt\n";
            ir << "}\n\n";
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) توليد معالج الذعر الافتراضي إذا كان إيقاف_عند_ذعر
        // (EN) Generate default panic handler if abort_on_panic
        // ─────────────────────────────────────────────────────────────────────
        
        if (config.abortOnPanic) {
            ir << "; " << "معالج الذعر (إيقاف مباشر)" << "\n";
            ir << "; Panic handler (immediate abort)\n";
            ir << "define void @__sad_panic_handler(ptr %info) {\n";
            ir << "entry:\n";
            ir << "  call void @llvm.trap()\n";
            ir << "  unreachable\n";
            ir << "}\n\n";
            
            ir << "declare void @llvm.trap() noreturn nounwind\n\n";
        }
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود معالج الذعر
     *        (EN) Generate panic handler code
     */
    std::string generatePanicHandler(const NoStdConfig& config) {
        if (config.abortOnPanic) {
            return "";  // (AR) تم التوليد في generateStartupCode
        }
        
        std::ostringstream ir;
        
        ir << "; " << "معالج الذعر المخصص" << "\n";
        ir << "; Custom panic handler\n";
        ir << "define void @__sad_panic_handler(ptr %info) {\n";
        ir << "entry:\n";
        
        if (!config.panicHandler.empty()) {
            ir << "  call void @" << config.panicHandler << "(ptr %info)\n";
        }
        
        ir << "  unreachable\n";
        ir << "}\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد سكريبت الرابط الافتراضي
     *        (EN) Generate default linker script
     * 
     * @param config (AR) الإعدادات (EN) Configuration
     * @return (AR) محتوى السكريبت (EN) Script content
     */
    std::string generateDefaultLinkerScript(const NoStdConfig& config) {
        std::ostringstream script;
        
        script << "/* " << "سكريبت رابط لغة ص" << " */\n";
        script << "/* Sad Language Linker Script */\n\n";
        
        script << "ENTRY(_start)\n\n";
        
        script << "SECTIONS\n";
        script << "{\n";
        
        // (AR) قسم الكود
        // (EN) Code section
        script << "    /* " << "قسم الكود" << " */\n";
        script << "    .text : {\n";
        script << "        *(.text._start)\n";
        script << "        *(.text*)\n";
        script << "    }\n\n";
        
        // (AR) قسم البيانات للقراءة فقط
        // (EN) Read-only data section
        script << "    /* " << "بيانات للقراءة فقط" << " */\n";
        script << "    .rodata : {\n";
        script << "        *(.rodata*)\n";
        script << "    }\n\n";
        
        // (AR) قسم البيانات المهيأة
        // (EN) Initialized data section
        script << "    /* " << "بيانات مهيأة" << " */\n";
        script << "    .data : {\n";
        script << "        *(.data*)\n";
        script << "    }\n\n";
        
        // (AR) قسم BSS
        // (EN) BSS section
        script << "    /* " << "بيانات غير مهيأة" << " */\n";
        script << "    .bss : {\n";
        script << "        __bss_start = .;\n";
        script << "        *(.bss*)\n";
        script << "        *(COMMON)\n";
        script << "        __bss_end = .;\n";
        script << "    }\n\n";
        
        // (AR) المكدس
        // (EN) Stack
        script << "    /* " << "المكدس" << " */\n";
        script << "    . = ALIGN(4096);\n";
        script << "    __stack_bottom = .;\n";
        script << "    . += 0x4000;  /* 16KB stack */\n";
        script << "    __stack_top = .;\n";
        
        script << "}\n";
        
        return script.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير وضع no_std
//                    (EN) No-std Mode Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class NoStdModeManager
 * @brief (AR) مدير وضع بلا مكتبة قياسية
 *        (EN) No-std mode manager
 * 
 * @details
 * (AR) هذه الفئة تدير جميع جوانب وضع no_std:
 *      - تحليل السمات
 *      - التحقق من المتطلبات
 *      - توليد الكود
 *      - إعداد الرابط
 * 
 * (EN) This class manages all aspects of no_std mode:
 *      - Attribute parsing
 *      - Requirements validation
 *      - Code generation
 *      - Linker setup
 */
class NoStdModeManager {
public:
    NoStdModeManager() = default;
    
    /**
     * @brief (AR) تهيئة من سمات الحزمة
     *        (EN) Initialize from crate attributes
     */
    void initializeFromAttributes(const std::vector<std::string>& attributes) {
        for (const auto& attr : attributes) {
            parser_.parseCrateAttribute(attr, config_);
        }
    }
    
    /**
     * @brief (AR) تسجيل دالة بسمة
     *        (EN) Register function with attribute
     */
    void registerFunctionAttribute(
        const std::string& attribute,
        const std::string& functionName
    ) {
        parser_.parseFunctionAttribute(attribute, functionName, config_);
    }
    
    /**
     * @brief (AR) هل وضع no_std مفعّل؟
     *        (EN) Is no_std mode enabled?
     */
    bool isNoStdEnabled() const {
        return config_.noStdEnabled;
    }
    
    /**
     * @brief (AR) التحقق من صحة الإعدادات
     *        (EN) Validate configuration
     * 
     * @return (AR) قائمة الأخطاء (فارغة إذا كان كل شيء صحيح)
     *         (EN) List of errors (empty if all valid)
     */
    std::vector<ValidationError> validate() {
        return validator_.validate(config_);
    }
    
    /**
     * @brief (AR) الحصول على الإعدادات
     *        (EN) Get configuration
     */
    const NoStdConfig& getConfig() const {
        return config_;
    }
    
    /**
     * @brief (AR) تعديل الإعدادات
     *        (EN) Modify configuration
     */
    NoStdConfig& getConfig() {
        return config_;
    }
    
    /**
     * @brief (AR) توليد كود البدء
     *        (EN) Generate startup code
     */
    std::string generateStartupCode() {
        return codegen_.generateStartupCode(config_);
    }
    
    /**
     * @brief (AR) توليد سكريبت الرابط
     *        (EN) Generate linker script
     */
    std::string generateLinkerScript() {
        if (!config_.linkerScript.empty()) {
            return "";  // (AR) استخدام السكريبت المحدد
        }
        return codegen_.generateDefaultLinkerScript(config_);
    }
    
    /**
     * @brief (AR) الحصول على المكتبات المستبعدة
     *        (EN) Get excluded libraries
     */
    std::vector<std::string> getExcludedLibraries() const {
        if (!config_.noStdEnabled) {
            return {};
        }
        
        return {
            "std",      // (AR) المكتبة القياسية الكاملة
            "stdio",    // (AR) الإدخال/الإخراج
            "heap",     // (AR) إدارة الكومة
            "panic",    // (AR) معالج الذعر الافتراضي
            "thread",   // (AR) الخيوط
            "net",      // (AR) الشبكات
            "fs",       // (AR) نظام الملفات
        };
    }
    
    /**
     * @brief (AR) الحصول على المكتبات المتاحة
     *        (EN) Get available libraries
     */
    std::vector<std::string> getAvailableLibraries() const {
        if (!config_.noStdEnabled) {
            return {"std"};  // (AR) كل شيء متاح
        }
        
        // (AR) المكتبات الأساسية المتاحة دائماً
        // (EN) Core libraries always available
        std::vector<std::string> available = {
            "core",     // (AR) الأنواع الأساسية
            "intrinsics", // (AR) الدوال الجوهرية
        };
        
        if (config_.allowAlloc) {
            available.push_back("alloc");  // (AR) التخصيص الديناميكي
        }
        
        return available;
    }
    
    /**
     * @brief (AR) طباعة معلومات التكوين
     *        (EN) Print configuration info
     */
    void printConfigInfo(std::ostream& out) const {
        out << "\n╔═══════════════════════════════════════════════════════════════╗\n";
        out << "║              وضع بلا مكتبة قياسية (no_std)                     ║\n";
        out << "╠═══════════════════════════════════════════════════════════════╣\n";
        
        out << "║  بلا_مكتبة_قياسية: " 
            << (config_.noStdEnabled ? "✓ مفعّل" : "✗ معطّل") 
            << std::string(40 - (config_.noStdEnabled ? 10 : 10), ' ') << "║\n";
        
        out << "║  بلا_رئيسية:       " 
            << (config_.noMainEnabled ? "✓ مفعّل" : "✗ معطّل")
            << std::string(40 - (config_.noMainEnabled ? 10 : 10), ' ') << "║\n";
        
        out << "║  نقطة_دخول:        " 
            << (config_.entryPoint.empty() ? "(افتراضي)" : config_.entryPoint)
            << std::string(40 - (config_.entryPoint.empty() ? 10 : config_.entryPoint.length()), ' ') 
            << "║\n";
        
        out << "║  معالج_ذعر:        "
            << (config_.panicHandler.empty() ? 
                (config_.abortOnPanic ? "(إيقاف)" : "(مطلوب!)") : 
                config_.panicHandler)
            << std::string(40 - (config_.panicHandler.empty() ? 10 : config_.panicHandler.length()), ' ')
            << "║\n";
        
        out << "║  وضع_الذاكرة:      " << "ملكية صارمة (بلا GC)"
            << std::string(20, ' ') << "║\n";
        
        out << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    }
    
    /**
     * @brief (AR) ضبط وضع الذاكرة تلقائياً عند تفعيل no_std
     *        (EN) Auto-configure memory mode when no_std activated
     * 
     * @details
     * (AR) عند تفعيل #![بلا_مكتبة_قياسية]:
     *      1. يُفرض وضع الإنتاج (Production)
     *      2. يُعطَّل جامع القمامة (GCStrategy::None)
     *      3. يُفعَّل وضع الملكية الصارمة جداً (UltraStrict)
     *      4. تُعطَّل الاقتراحات وكشف الدورات
     *      
     *      هذا ضروري لأن النواة لا يمكنها الاعتماد على GC
     *      (لا يوجد وقت تشغيل لجمع القمامة في بيئة bare-metal)
     * 
     * (EN) When #![no_std] is activated:
     *      1. Force Production mode
     *      2. Disable GC (GCStrategy::None)
     *      3. Enable UltraStrict ownership
     *      4. Disable suggestions and cycle detection
     */
    void enforceMemoryModeForNoStd() {
        if (!config_.noStdEnabled) {
            return;  // (AR) لا شيء للفعل إذا لم يكن no_std مفعّلاً
        }
        
        // (AR) إعدادات الذاكرة لوضع النواة / bare-metal
        // (EN) Memory settings for kernel / bare-metal mode
        memoryMode_ = MemoryModeForNoStd::OwnershipStrict;
        gcEnabled_ = false;
        ownershipStrict_ = true;
    }
    
    /**
     * @brief (AR) هل جامع القمامة مسموح؟
     *        (EN) Is garbage collector allowed?
     */
    bool isGCAllowed() const {
        return !config_.noStdEnabled;  // (AR) لا GC في no_std
    }
    
    /**
     * @brief (AR) هل الملكية الصارمة مفروضة؟
     *        (EN) Is strict ownership enforced?
     */
    bool isStrictOwnershipEnforced() const {
        return config_.noStdEnabled;  // (AR) ملكية صارمة في no_std
    }
    
    /**
     * @brief (AR) التحقق من أن استدعاء GC غير مسموح في no_std
     *        (EN) Validate that GC calls are not allowed in no_std
     * 
     * @param functionName (AR) اسم الدالة (EN) Function name
     * @return (AR) رسالة خطأ أو سلسلة فارغة (EN) Error message or empty
     */
    std::string validateNoGCCall(const std::string& functionName) const {
        if (!config_.noStdEnabled) {
            return "";  // (AR) GC مسموح في الوضع العادي
        }
        
        // (AR) قائمة الدوال الممنوعة في وضع no_std
        // (EN) List of forbidden functions in no_std mode
        static const std::set<std::string> forbiddenGCFunctions = {
            "اجمع_قمامة",           // collect_garbage
            "فعّل_جامع_القمامة",      // enable_gc
            "عدّ_المراجع",           // ref_count
            "gc_collect",
            "gc_enable",
            "gc_disable",
            "ref_count",
        };
        
        if (forbiddenGCFunctions.count(functionName) > 0) {
            return "خطأ: لا يمكن استخدام '" + functionName + 
                   "' في وضع بلا مكتبة قياسية. "
                   "استخدم نظام الملكية بدلاً من جامع القمامة.\n"
                   "Error: Cannot use '" + functionName + 
                   "' in no_std mode. Use ownership instead of GC.";
        }
        
        return "";
    }
    
private:
    NoStdConfig config_;
    NoStdAttributeParser parser_;
    NoStdValidator validator_;
    NoStdCodeGenerator codegen_;
    
    // (AR) وضع الذاكرة في no_std
    // (EN) Memory mode in no_std
    enum class MemoryModeForNoStd {
        OwnershipStrict,    // (AR) ملكية صارمة (الافتراضي في no_std)
        OwnershipUltra      // (AR) ملكية صارمة جداً
    };
    MemoryModeForNoStd memoryMode_ = MemoryModeForNoStd::OwnershipStrict;
    bool gcEnabled_ = false;
    bool ownershipStrict_ = true;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) دوال مساعدة
//                    (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء مدير no_std من سطر الأوامر
 *        (EN) Create no_std manager from command line
 */
std::unique_ptr<NoStdModeManager> createNoStdManagerFromArgs(
    bool noStd,
    bool noMain,
    const std::string& target
) {
    auto manager = std::make_unique<NoStdModeManager>();
    auto& config = manager->getConfig();
    
    config.noStdEnabled = noStd;
    config.noMainEnabled = noMain;
    config.targetTriple = target;
    
    return manager;
}

/**
 * @brief (AR) التحقق مما إذا كان الهدف يتطلب no_std
 *        (EN) Check if target requires no_std
 */
bool targetRequiresNoStd(const std::string& target) {
    // (AR) الأهداف التي تتطلب no_std
    // (EN) Targets that require no_std
    static const std::set<std::string> noStdTargets = {
        "x86_64-unknown-none",
        "aarch64-unknown-none",
        "riscv64-unknown-none-elf",
        "thumbv7m-none-eabi",
        "thumbv7em-none-eabi",
        "avr-unknown-unknown",
    };
    
    return noStdTargets.count(target) > 0 ||
           target.find("-none") != std::string::npos ||
           target.find("-eabi") != std::string::npos;
}

} // namespace pipeline
} // namespace compiler
} // namespace sad
