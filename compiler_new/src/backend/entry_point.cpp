/**
 * @file entry_point.cpp
 * @brief (AR) تنفيذ نقاط الدخول المخصصة للأنظمة المدمجة
 *        (EN) Custom entry point implementation for embedded systems
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ███╗   ██╗ ██████╗ ██████╗ ████████╗███████╗                              ║
 * ║     ████╗  ██║██╔═══██╗██╔══██╗╚══██╔══╝██╔════╝                              ║
 * ║     ██╔██╗ ██║██║   ██║██████╔╝   ██║   █████╗                                ║
 * ║     ██║╚██╗██║██║▄▄ ██║██╔══██╗   ██║   ██╔══╝                                ║
 * ║     ██║ ╚████║╚██████╔╝██║  ██║   ██║   ███████╗                              ║
 * ║     ╚═╝  ╚═══╝ ╚══▀▀═╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝                              ║
 * ║                                                                               ║
 * ║              ██████╗ ██╗   ██╗██╗  ██╗██╗   ██╗██╗                            ║
 * ║              ██╔══██╗██║   ██║██║  ██║██║   ██║██║                            ║
 * ║              ██║  ██║██║   ██║██║  ██║██║   ██║██║                            ║
 * ║              ██║  ██║██║   ██║██║  ██║██║   ██║██║                            ║
 * ║              ██████╔╝╚██████╔╝██╗  ██╗╚██████╔╝███████╗                       ║
 * ║              ╚═════╝  ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚══════╝                       ║
 * ║                                                                               ║
 * ║                    نقطة الدخول - Entry Point                                  ║
 * ║                    لغة ص للأنظمة المدمجة                                       ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يدير نقاط الدخول المخصصة لبرامج bare-metal:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     مراحل بدء التشغيل (Startup Stages)                   │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐        │
 *      │    │  Reset   │───►│  _start  │───►│  .bss    │───►│  .data   │        │
 *      │    │  Vector  │    │          │    │  clear   │    │  init    │        │
 *      │    └──────────┘    └──────────┘    └──────────┘    └──────────┘        │
 *      │                                                                 │        │
 *      │                                                                 ▼        │
 *      │    ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐        │
 *      │    │  HALT    │◄───│   main   │◄───│  global  │◄───│  stack   │        │
 *      │    │          │    │          │    │  ctors   │    │  setup   │        │
 *      │    └──────────┘    └──────────┘    └──────────┘    └──────────┘        │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 *      الاستخدام في كود ص:
 *      @code
 *      #![بلا_مكتبة_قياسية]
 *      #![بلا_رئيسية]
 *      
 *      #[نقطة_دخول]
 *      #[رابط(الاسم = "_start")]
 *      دالة البداية() -> ! {
 *          // تهيئة المعالج
 *          هيّئ_المكدس()؛
 *          امسح_bss()؛
 *          هيّئ_البيانات()؛
 *          
 *          // استدعاء البرنامج الرئيسي
 *          رئيسية()؛
 *          
 *          // لا يجب الوصول هنا
 *          حلقة { توقف() }
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
#include <functional>

namespace sad {
namespace compiler {
namespace backend {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع نقاط الدخول
//                    (EN) Entry Point Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum EntryPointKind
 * @brief (AR) أنواع نقاط الدخول المدعومة
 *        (EN) Supported entry point types
 */
enum class EntryPointKind {
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) نقاط الدخول القياسية
    // (EN) Standard entry points
    // ─────────────────────────────────────────────────────────────────────────
    
    MAIN,               // (AR) دالة main العادية
    NO_MAIN,            // (AR) بدون main (bare-metal)
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) نقاط دخول الأنظمة المدمجة
    // (EN) Embedded system entry points
    // ─────────────────────────────────────────────────────────────────────────
    
    RESET_HANDLER,      // (AR) معالج إعادة التشغيل
    ARDUINO_SETUP,      // (AR) دالة setup() لـ Arduino
    RTOS_TASK,          // (AR) مهمة RTOS
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) نقاط دخول خاصة
    // (EN) Special entry points
    // ─────────────────────────────────────────────────────────────────────────
    
    EFI_MAIN,           // (AR) نقطة دخول UEFI
    KERNEL_MAIN,        // (AR) نقطة دخول نواة نظام تشغيل
    BOOTLOADER,         // (AR) محمل الإقلاع
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) معلومات نقطة الدخول
//                    (EN) Entry Point Information
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct EntryPointInfo
 * @brief (AR) معلومات نقطة الدخول
 *        (EN) Entry point information
 */
struct EntryPointInfo {
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) الهوية
    // (EN) Identity
    // ─────────────────────────────────────────────────────────────────────────
    
    std::string name;               // (AR) اسم الدالة في كود ص
    std::string linkName;           // (AR) الاسم في الرابط (مثل _start)
    EntryPointKind kind;            // (AR) نوع نقطة الدخول
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) التوقيع
    // (EN) Signature
    // ─────────────────────────────────────────────────────────────────────────
    
    std::string returnType;         // (AR) نوع الإرجاع (عادة !)
    std::vector<std::pair<std::string, std::string>> parameters;  // (AR) المعاملات
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) السمات
    // (EN) Attributes
    // ─────────────────────────────────────────────────────────────────────────
    
    bool isNaked = false;           // (AR) بدون prologue/epilogue
    bool noMangle = true;           // (AR) بدون تشويه الاسم
    bool noReturn = true;           // (AR) لا ترجع (!)
    std::string section;            // (AR) قسم مخصص (مثل .text._start)
    
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) التهيئة
    // (EN) Initialization
    // ─────────────────────────────────────────────────────────────────────────
    
    bool clearBss = true;           // (AR) مسح قسم .bss
    bool initData = true;           // (AR) تهيئة قسم .data
    bool setupStack = true;         // (AR) إعداد المكدس
    bool callGlobalCtors = true;    // (AR) استدعاء المُنشئات العامة
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) سمات نقطة الدخول العربية
//                    (EN) Arabic Entry Point Attributes
// ═══════════════════════════════════════════════════════════════════════════════

namespace سمات_الدخول {
    // (AR) السمة الرئيسية
    const std::string نقطة_دخول = u8"نقطة_دخول";
    const std::string entry_point = "entry_point";
    
    // (AR) سمات التكوين
    const std::string رابط = u8"رابط";                    // #[رابط(الاسم = "_start")]
    const std::string عاري = u8"عاري";                    // #[عاري] - بدون prologue/epilogue
    const std::string قسم = u8"قسم";                      // #[قسم(".text._start")]
    const std::string بدون_تشويه = u8"بدون_تشويه";        // #[بدون_تشويه]
    const std::string لا_يرجع = u8"لا_يرجع";              // #[لا_يرجع]
    
    // (AR) سمات التهيئة
    const std::string بدون_تهيئة_bss = u8"بدون_تهيئة_bss";
    const std::string بدون_تهيئة_بيانات = u8"بدون_تهيئة_بيانات";
    const std::string بدون_مكدس = u8"بدون_مكدس";
    const std::string بدون_منشئات = u8"بدون_منشئات";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل سمات نقطة الدخول
//                    (EN) Entry Point Attribute Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class EntryPointAttributeParser
 * @brief (AR) محلل سمات نقطة الدخول
 *        (EN) Entry point attribute parser
 */
class EntryPointAttributeParser {
public:
    /**
     * @brief (AR) تحليل سمة نقطة الدخول
     *        (EN) Parse entry point attribute
     * 
     * @param attribute (AR) نص السمة الكامل (EN) Full attribute text
     * @param info (AR) معلومات للتحديث (EN) Info to update
     * @return (AR) هل تم التعرف؟ (EN) Was it recognized?
     */
    bool parseAttribute(const std::string& attribute, EntryPointInfo& info) {
        // (AR) التحقق من سمة نقطة_دخول الأساسية
        if (attribute == سمات_الدخول::نقطة_دخول || 
            attribute == سمات_الدخول::entry_point) {
            info.kind = EntryPointKind::NO_MAIN;
            info.noReturn = true;
            return true;
        }
        
        // (AR) سمة عاري (naked)
        if (attribute == سمات_الدخول::عاري || attribute == "naked") {
            info.isNaked = true;
            return true;
        }
        
        // (AR) سمة بدون_تشويه
        if (attribute == سمات_الدخول::بدون_تشويه || attribute == "no_mangle") {
            info.noMangle = true;
            return true;
        }
        
        // (AR) سمة لا_يرجع
        if (attribute == سمات_الدخول::لا_يرجع || attribute == "no_return") {
            info.noReturn = true;
            info.returnType = "!";
            return true;
        }
        
        // (AR) سمات تعطيل التهيئة
        if (attribute == سمات_الدخول::بدون_تهيئة_bss) {
            info.clearBss = false;
            return true;
        }
        if (attribute == سمات_الدخول::بدون_تهيئة_بيانات) {
            info.initData = false;
            return true;
        }
        if (attribute == سمات_الدخول::بدون_مكدس) {
            info.setupStack = false;
            return true;
        }
        if (attribute == سمات_الدخول::بدون_منشئات) {
            info.callGlobalCtors = false;
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief (AR) تحليل سمة رابط
     *        (EN) Parse link attribute
     * 
     * @param value (AR) قيمة السمة (EN) Attribute value
     * @param info (AR) معلومات للتحديث (EN) Info to update
     * 
     * @example #[رابط(الاسم = "_start")] -> linkName = "_start"
     */
    bool parseLinkAttribute(const std::string& value, EntryPointInfo& info) {
        // (AR) البحث عن الاسم = "..."
        size_t namePos = value.find(u8"الاسم");
        if (namePos == std::string::npos) {
            namePos = value.find("name");
        }
        
        if (namePos != std::string::npos) {
            size_t startQuote = value.find('"', namePos);
            size_t endQuote = value.find('"', startQuote + 1);
            
            if (startQuote != std::string::npos && endQuote != std::string::npos) {
                info.linkName = value.substr(startQuote + 1, endQuote - startQuote - 1);
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * @brief (AR) تحليل سمة قسم
     *        (EN) Parse section attribute
     * 
     * @example #[قسم(".text._start")] -> section = ".text._start"
     */
    bool parseSectionAttribute(const std::string& value, EntryPointInfo& info) {
        size_t startQuote = value.find('"');
        size_t endQuote = value.find('"', startQuote + 1);
        
        if (startQuote != std::string::npos && endQuote != std::string::npos) {
            info.section = value.substr(startQuote + 1, endQuote - startQuote - 1);
            return true;
        }
        
        return false;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد كود نقطة الدخول
//                    (EN) Entry Point Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class EntryPointCodeGenerator
 * @brief (AR) مولّد LLVM IR لنقاط الدخول
 *        (EN) LLVM IR generator for entry points
 */
class EntryPointCodeGenerator {
public:
    /**
     * @brief (AR) توليد كود التهيئة (startup code)
     *        (EN) Generate initialization code (startup code)
     * 
     * @param info (AR) معلومات نقطة الدخول (EN) Entry point info
     * @return (AR) كود LLVM IR (EN) LLVM IR code
     */
    std::string generateStartupCode(const EntryPointInfo& info) {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"كود بدء التشغيل لـ " << info.name << "\n";
        ir << "; Startup code for " << info.name << "\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) تعريفات خارجية
        ir << generateExternDeclarations(info);
        
        // (AR) نقطة الدخول الفعلية
        ir << generateEntryPoint(info);
        
        return ir.str();
    }
    
private:
    /**
     * @brief (AR) توليد التعريفات الخارجية
     *        (EN) Generate external declarations
     */
    std::string generateExternDeclarations(const EntryPointInfo& info) {
        std::ostringstream ir;
        
        ir << "; " << u8"رموز خارجية من سكريبت الرابط" << "\n";
        ir << "; External symbols from linker script\n";
        
        if (info.clearBss) {
            ir << "@__bss_start = external global i8\n";
            ir << "@__bss_end = external global i8\n";
        }
        
        if (info.initData) {
            ir << "@__data_start = external global i8\n";
            ir << "@__data_end = external global i8\n";
            ir << "@__data_load = external global i8\n";
        }
        
        if (info.setupStack) {
            ir << "@__stack_top = external global i8\n";
        }
        
        if (info.callGlobalCtors) {
            ir << "@__init_array_start = external global ptr\n";
            ir << "@__init_array_end = external global ptr\n";
        }
        
        ir << "\n";
        
        // (AR) الدالة الرئيسية المستخدم
        ir << "; " << u8"الدالة الرئيسية" << "\n";
        ir << "declare void @" << info.name << "() noreturn\n\n";
        
        // (AR) دوال LLVM intrinsics
        ir << "; LLVM Intrinsics\n";
        ir << "declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)\n";
        ir << "declare void @llvm.memcpy.p0.p0.i64(ptr, ptr, i64, i1)\n";
        ir << "declare void @llvm.trap() noreturn nounwind\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد نقطة الدخول
     *        (EN) Generate entry point
     */
    std::string generateEntryPoint(const EntryPointInfo& info) {
        std::ostringstream ir;
        
        ir << "; " << u8"نقطة الدخول: " << info.linkName << "\n";
        ir << "; Entry point: " << info.linkName << "\n";
        
        // (AR) سمات الدالة
        std::string attributes = "noreturn";
        if (info.isNaked) {
            attributes += " naked";
        }
        
        // (AR) بداية الدالة
        ir << "define void @" << info.linkName << "() " << attributes;
        
        if (!info.section.empty()) {
            ir << " section \"" << info.section << "\"";
        }
        
        ir << " {\n";
        ir << "entry:\n";
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) المرحلة 1: إعداد المكدس
        // (EN) Stage 1: Stack setup
        // ─────────────────────────────────────────────────────────────────────
        
        if (info.setupStack && !info.isNaked) {
            ir << "  ; " << u8"المرحلة 1: إعداد المكدس" << "\n";
            ir << "  ; Stage 1: Stack setup\n";
            ir << "  ; (Stack pointer is typically set by hardware or bootloader)\n\n";
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) المرحلة 2: مسح قسم .bss
        // (EN) Stage 2: Clear .bss section
        // ─────────────────────────────────────────────────────────────────────
        
        if (info.clearBss && !info.isNaked) {
            ir << "  ; " << u8"المرحلة 2: مسح قسم .bss" << "\n";
            ir << "  ; Stage 2: Clear .bss section\n";
            ir << "  %bss_start = ptrtoint ptr @__bss_start to i64\n";
            ir << "  %bss_end = ptrtoint ptr @__bss_end to i64\n";
            ir << "  %bss_size = sub i64 %bss_end, %bss_start\n";
            ir << "  call void @llvm.memset.p0.i64(ptr @__bss_start, i8 0, i64 %bss_size, i1 false)\n\n";
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) المرحلة 3: تهيئة قسم .data
        // (EN) Stage 3: Initialize .data section
        // ─────────────────────────────────────────────────────────────────────
        
        if (info.initData && !info.isNaked) {
            ir << "  ; " << u8"المرحلة 3: تهيئة قسم .data" << "\n";
            ir << "  ; Stage 3: Initialize .data section\n";
            ir << "  %data_start = ptrtoint ptr @__data_start to i64\n";
            ir << "  %data_end = ptrtoint ptr @__data_end to i64\n";
            ir << "  %data_size = sub i64 %data_end, %data_start\n";
            ir << "  call void @llvm.memcpy.p0.p0.i64(ptr @__data_start, ptr @__data_load, i64 %data_size, i1 false)\n\n";
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) المرحلة 4: استدعاء المُنشئات العامة
        // (EN) Stage 4: Call global constructors
        // ─────────────────────────────────────────────────────────────────────
        
        if (info.callGlobalCtors && !info.isNaked) {
            ir << "  ; " << u8"المرحلة 4: استدعاء المُنشئات العامة" << "\n";
            ir << "  ; Stage 4: Call global constructors\n";
            ir << "  br label %ctor_loop\n\n";
            
            ir << "ctor_loop:\n";
            ir << "  %ctor_ptr = phi ptr [ @__init_array_start, %entry ], [ %ctor_next, %ctor_call ]\n";
            ir << "  %ctor_end_ptr = ptrtoint ptr @__init_array_end to i64\n";
            ir << "  %ctor_cur_ptr = ptrtoint ptr %ctor_ptr to i64\n";
            ir << "  %ctor_done = icmp uge i64 %ctor_cur_ptr, %ctor_end_ptr\n";
            ir << "  br i1 %ctor_done, label %call_main, label %ctor_call\n\n";
            
            ir << "ctor_call:\n";
            ir << "  %ctor_fn = load ptr, ptr %ctor_ptr\n";
            ir << "  call void %ctor_fn()\n";
            ir << "  %ctor_next = getelementptr ptr, ptr %ctor_ptr, i32 1\n";
            ir << "  br label %ctor_loop\n\n";
            
            ir << "call_main:\n";
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) المرحلة 5: استدعاء الدالة الرئيسية
        // (EN) Stage 5: Call main function
        // ─────────────────────────────────────────────────────────────────────
        
        ir << "  ; " << u8"المرحلة 5: استدعاء الدالة الرئيسية" << "\n";
        ir << "  ; Stage 5: Call main function\n";
        ir << "  call void @" << info.name << "()\n\n";
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) المرحلة 6: التوقف
        // (EN) Stage 6: Halt
        // ─────────────────────────────────────────────────────────────────────
        
        ir << "  ; " << u8"المرحلة 6: التوقف (لا يجب الوصول هنا)" << "\n";
        ir << "  ; Stage 6: Halt (should not reach here)\n";
        ir << "  br label %halt\n\n";
        
        ir << "halt:\n";
        ir << "  call void @llvm.trap()\n";
        ir << "  unreachable\n";
        
        ir << "}\n\n";
        
        return ir.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد نقطة دخول Arduino
//                    (EN) Arduino Entry Point Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class ArduinoEntryPointGenerator
 * @brief (AR) مولّد نقطة دخول خاصة بـ Arduino
 *        (EN) Arduino-specific entry point generator
 */
class ArduinoEntryPointGenerator {
public:
    /**
     * @brief (AR) توليد نقطة دخول Arduino
     *        (EN) Generate Arduino entry point
     * 
     * @details
     * (AR) بنية برنامج Arduino في ص:
     *      @code
     *      #![بلا_مكتبة_قياسية]
     *      استخدم أردوينو::*؛
     *      
     *      #[إعداد]                // setup()
     *      دالة إعداد() {
     *          هيّئ_المنفذ(٩٦٠٠)؛
     *      }
     *      
     *      #[حلقة]                 // loop()
     *      دالة حلقة() {
     *          اكتب_خط("مرحباً!")؛
     *          تأخير(١٠٠٠)؛
     *      }
     *      @endcode
     */
    std::string generateArduinoMain(
        const std::string& setupFunction,
        const std::string& loopFunction
    ) {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"نقطة دخول Arduino" << "\n";
        ir << "; Arduino Entry Point\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) التصريحات
        ir << "declare void @" << setupFunction << "()\n";
        ir << "declare void @" << loopFunction << "()\n";
        ir << "declare void @__arduino_init()\n\n";
        
        // (AR) نقطة الدخول
        ir << "; " << u8"دالة main الخاصة بـ Arduino" << "\n";
        ir << "define i32 @main() {\n";
        ir << "entry:\n";
        
        // (AR) تهيئة Arduino
        ir << "  ; " << u8"تهيئة الأجهزة" << "\n";
        ir << "  call void @__arduino_init()\n\n";
        
        // (AR) استدعاء setup()
        ir << "  ; " << u8"استدعاء دالة الإعداد" << "\n";
        ir << "  call void @" << setupFunction << "()\n\n";
        
        // (AR) حلقة loop() اللانهائية
        ir << "  ; " << u8"الحلقة الرئيسية اللانهائية" << "\n";
        ir << "  br label %loop\n\n";
        
        ir << "loop:\n";
        ir << "  call void @" << loopFunction << "()\n";
        ir << "  br label %loop\n";
        ir << "}\n\n";
        
        return ir.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير نقاط الدخول
//                    (EN) Entry Point Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class EntryPointManager
 * @brief (AR) مدير نقاط الدخول للمشروع
 *        (EN) Entry point manager for the project
 */
class EntryPointManager {
public:
    EntryPointManager() = default;
    
    /**
     * @brief (AR) تسجيل نقطة دخول
     *        (EN) Register entry point
     */
    void registerEntryPoint(const EntryPointInfo& info) {
        entryPoints_[info.name] = info;
        
        // (AR) تحديد نقطة الدخول الرئيسية
        if (info.kind == EntryPointKind::NO_MAIN || 
            info.kind == EntryPointKind::RESET_HANDLER ||
            info.kind == EntryPointKind::KERNEL_MAIN) {
            primaryEntryPoint_ = info.name;
        }
    }
    
    /**
     * @brief (AR) الحصول على نقطة الدخول الرئيسية
     *        (EN) Get primary entry point
     */
    std::optional<EntryPointInfo> getPrimaryEntryPoint() const {
        if (primaryEntryPoint_.empty()) {
            return std::nullopt;
        }
        
        auto it = entryPoints_.find(primaryEntryPoint_);
        if (it != entryPoints_.end()) {
            return it->second;
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief (AR) هل يوجد نقطة دخول مخصصة؟
     *        (EN) Is there a custom entry point?
     */
    bool hasCustomEntryPoint() const {
        return !primaryEntryPoint_.empty();
    }
    
    /**
     * @brief (AR) توليد كود البدء
     *        (EN) Generate startup code
     */
    std::string generateStartupCode() {
        auto primary = getPrimaryEntryPoint();
        if (!primary) {
            return "";
        }
        
        return codeGen_.generateStartupCode(*primary);
    }
    
    /**
     * @brief (AR) الحصول على جميع نقاط الدخول
     *        (EN) Get all entry points
     */
    const std::map<std::string, EntryPointInfo>& getEntryPoints() const {
        return entryPoints_;
    }
    
    /**
     * @brief (AR) التحقق من صحة نقاط الدخول
     *        (EN) Validate entry points
     */
    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        // (AR) التحقق من وجود نقطة دخول واحدة على الأقل
        if (entryPoints_.empty()) {
            errors.push_back(u8"لا توجد نقطة دخول محددة");
        }
        
        // (AR) التحقق من نقاط الدخول المتعددة
        int primaryCount = 0;
        for (const auto& [name, info] : entryPoints_) {
            if (info.kind == EntryPointKind::NO_MAIN ||
                info.kind == EntryPointKind::RESET_HANDLER) {
                primaryCount++;
            }
            
            // (AR) التحقق من نوع الإرجاع
            if (info.noReturn && info.returnType != "!" && 
                info.returnType.find(u8"أبداً") == std::string::npos) {
                errors.push_back(u8"نقطة الدخول '" + name + u8"' يجب أن ترجع نوع 'أبداً' (!)");
            }
        }
        
        if (primaryCount > 1) {
            errors.push_back(u8"يوجد أكثر من نقطة دخول رئيسية");
        }
        
        return errors;
    }
    
private:
    std::map<std::string, EntryPointInfo> entryPoints_;
    std::string primaryEntryPoint_;
    EntryPointCodeGenerator codeGen_;
    EntryPointAttributeParser parser_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) دوال مساعدة
//                    (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء معلومات نقطة دخول افتراضية
 *        (EN) Create default entry point info
 */
EntryPointInfo createDefaultEntryPointInfo(const std::string& name) {
    EntryPointInfo info;
    info.name = name;
    info.linkName = "_start";
    info.kind = EntryPointKind::NO_MAIN;
    info.returnType = "!";
    info.noReturn = true;
    info.noMangle = true;
    info.section = ".text._start";
    info.clearBss = true;
    info.initData = true;
    info.setupStack = true;
    info.callGlobalCtors = true;
    
    return info;
}

/**
 * @brief (AR) إنشاء معلومات نقطة دخول Arduino
 *        (EN) Create Arduino entry point info
 */
EntryPointInfo createArduinoEntryPointInfo(
    const std::string& setupName,
    const std::string& loopName
) {
    EntryPointInfo info;
    info.name = "main";
    info.linkName = "main";
    info.kind = EntryPointKind::ARDUINO_SETUP;
    info.returnType = "i32";
    info.noReturn = false;
    info.noMangle = true;
    info.clearBss = false;  // (AR) يتم بواسطة Arduino runtime
    info.initData = false;
    info.setupStack = false;
    info.callGlobalCtors = false;
    
    // (AR) تخزين أسماء setup و loop
    info.parameters.push_back({"setup", setupName});
    info.parameters.push_back({"loop", loopName});
    
    return info;
}

/**
 * @brief (AR) إنشاء معلومات نقطة دخول نواة
 *        (EN) Create kernel entry point info
 */
EntryPointInfo createKernelEntryPointInfo(const std::string& name) {
    EntryPointInfo info = createDefaultEntryPointInfo(name);
    info.kind = EntryPointKind::KERNEL_MAIN;
    info.linkName = "_kernel_start";
    info.section = ".text.boot";
    
    // (AR) النواة تدير التهيئة بنفسها عادةً
    info.callGlobalCtors = false;
    
    return info;
}

} // namespace backend
} // namespace compiler
} // namespace sad
