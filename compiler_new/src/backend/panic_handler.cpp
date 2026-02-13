/**
 * @file panic_handler.cpp
 * @brief (AR) معالج الذعر للأنظمة المدمجة
 *        (EN) Panic handler for embedded systems
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ██████╗  █████╗ ███╗   ██╗██╗ ██████╗                                    ║
 * ║     ██╔══██╗██╔══██╗████╗  ██║██║██╔════╝                                    ║
 * ║     ██████╔╝███████║██╔██╗ ██║██║██║                                         ║
 * ║     ██╔═══╝ ██╔══██║██║╚██╗██║██║██║                                         ║
 * ║     ██║     ██║  ██║██║ ╚████║██║╚██████╗                                    ║
 * ║     ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝ ╚═════╝                                    ║
 * ║                                                                               ║
 * ║         ██╗  ██╗ █████╗ ███╗   ██╗██████╗ ██╗     ███████╗██████╗            ║
 * ║         ██║  ██║██╔══██╗████╗  ██║██╔══██╗██║     ██╔════╝██╔══██╗           ║
 * ║         ███████║███████║██╔██╗ ██║██║  ██║██║     █████╗  ██████╔╝           ║
 * ║         ██╔══██║██╔══██║██║╚██╗██║██║  ██║██║     ██╔══╝  ██╔══██╗           ║
 * ║         ██║  ██║██║  ██║██║ ╚████║██████╔╝███████╗███████╗██║  ██║           ║
 * ║         ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝ ╚══════╝╚══════╝╚═╝  ╚═╝           ║
 * ║                                                                               ║
 * ║                     معالج الذعر - Panic Handler                               ║
 * ║                     لغة ص للتعامل مع الأخطاء الحرجة                          ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يدير معالجة الذعر (panic) في الأنظمة المدمجة:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     استراتيجيات معالجة الذعر                             │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    ┌──────────────┐    ┌──────────────┐    ┌──────────────┐            │
 *      │    │    إيقاف    │    │   طباعة    │    │  إعادة       │            │
 *      │    │    (Halt)   │    │  (Print)   │    │  تشغيل       │            │
 *      │    │             │    │            │    │  (Reset)     │            │
 *      │    │  حلقة {}    │    │  الرسالة  │    │  إعادة       │            │
 *      │    │  لا نهائية  │    │  + موقع   │    │  الإقلاع     │            │
 *      │    └──────────────┘    └──────────────┘    └──────────────┘            │
 *      │           │                   │                   │                    │
 *      │           └───────────────────┼───────────────────┘                    │
 *      │                               ▼                                        │
 *      │                    ┌──────────────────┐                                │
 *      │                    │  #[معالج_ذعر]   │                                │
 *      │                    │  المعالج المخصص │                                │
 *      │                    └──────────────────┘                                │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 *      الاستخدام في كود ص:
 *      @code
 *      #![بلا_مكتبة_قياسية]
 *      
 *      // معالج الذعر المخصص
 *      #[معالج_ذعر]
 *      دالة عند_الذعر(معلومات: &معلومات_ذعر) -> ! {
 *          // طباعة الرسالة عبر المنفذ التسلسلي
 *          اطبع_تسلسلي("ذعر: ")؛
 *          اطبع_تسلسلي(معلومات.رسالة)؛
 *          
 *          // طباعة الموقع
 *          إذا_دع بعض(موقع) = معلومات.موقع {
 *              اطبع_تسلسلي(" في ")؛
 *              اطبع_تسلسلي(موقع.ملف)؛
 *              اطبع_تسلسلي(":")؛
 *              اطبع_رقم(موقع.سطر)؛
 *          }
 *          
 *          // توقف نهائي
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
//                    (AR) استراتيجيات معالجة الذعر
//                    (EN) Panic Handling Strategies
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum PanicStrategy
 * @brief (AR) استراتيجيات معالجة الذعر
 *        (EN) Panic handling strategies
 */
enum class PanicStrategy {
    ABORT,              // (AR) إيقاف فوري (trap)
    HALT,               // (AR) حلقة لا نهائية
    RESET,              // (AR) إعادة تشغيل النظام
    UNWIND,             // (AR) فك المكدس (غير متاح في no_std)
    CUSTOM,             // (AR) معالج مخصص
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) هيكل معلومات الذعر
//                    (EN) Panic Information Structure
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct PanicLocation
 * @brief (AR) موقع حدوث الذعر
 *        (EN) Panic location
 */
struct PanicLocation {
    std::string file;       // (AR) اسم الملف
    int line;               // (AR) رقم السطر
    int column;             // (AR) رقم العمود
};

/**
 * @struct PanicInfo
 * @brief (AR) معلومات الذعر
 *        (EN) Panic information
 */
struct PanicInfo {
    std::string message;                        // (AR) رسالة الذعر
    std::optional<PanicLocation> location;      // (AR) موقع الذعر
    std::string panicType;                      // (AR) نوع الذعر
    bool canContinue = false;                   // (AR) هل يمكن الاستمرار؟
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) سمات معالج الذعر العربية
//                    (EN) Arabic Panic Handler Attributes
// ═══════════════════════════════════════════════════════════════════════════════

namespace سمات_الذعر {
    // (AR) السمة الرئيسية
    const std::string معالج_ذعر = u8"معالج_ذعر";
    const std::string إيقاف_عند_ذعر = u8"إيقاف_عند_ذعر";
    
    // (AR) أنواع الذعر
    const std::string تأكد_فشل = u8"تأكد_فشل";
    const std::string فهرس_خارج_الحدود = u8"فهرس_خارج_الحدود";
    const std::string لا_يمكن_فك_غلاف = u8"لا_يمكن_فك_غلاف";
    const std::string قسمة_صفر = u8"قسمة_صفر";
    const std::string طفحان_حسابي = u8"طفحان_حسابي";
    const std::string نفاد_الذاكرة = u8"نفاد_الذاكرة";
    const std::string مؤشر_فارغ = u8"مؤشر_فارغ";
    
    // (AR) حقول معلومات الذعر
    const std::string رسالة = u8"رسالة";
    const std::string موقع = u8"موقع";
    const std::string ملف = u8"ملف";
    const std::string سطر = u8"سطر";
    const std::string عمود = u8"عمود";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) معلومات معالج الذعر
//                    (EN) Panic Handler Information
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct PanicHandlerInfo
 * @brief (AR) معلومات معالج الذعر المسجل
 *        (EN) Registered panic handler information
 */
struct PanicHandlerInfo {
    std::string functionName;       // (AR) اسم الدالة
    std::string linkName;           // (AR) الاسم في الرابط
    PanicStrategy strategy;         // (AR) الاستراتيجية المستخدمة
    bool isDefault;                 // (AR) هل هو المعالج الافتراضي؟
    
    // (AR) خصائص المعالج
    bool printMessage = false;      // (AR) طباعة الرسالة
    bool printLocation = false;     // (AR) طباعة الموقع
    bool printBacktrace = false;    // (AR) طباعة تتبع المكدس
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد كود معالج الذعر
//                    (EN) Panic Handler Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class PanicHandlerCodeGenerator
 * @brief (AR) مولّد LLVM IR لمعالج الذعر
 *        (EN) LLVM IR generator for panic handler
 */
class PanicHandlerCodeGenerator {
public:
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        targetArch_ = arch;
    }
    
    /**
     * @brief (AR) توليد تعريف هيكل معلومات الذعر
     *        (EN) Generate panic info structure definition
     */
    std::string generatePanicInfoStruct() {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"هيكل معلومات الذعر" << "\n";
        ir << "; Panic Info Structure\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) هيكل الموقع
        ir << "; " << u8"هيكل موقع الذعر" << "\n";
        ir << "%PanicLocation = type {\n";
        ir << "    ptr,      ; " << u8"اسم الملف (file)" << "\n";
        ir << "    i32,      ; " << u8"رقم السطر (line)" << "\n";
        ir << "    i32       ; " << u8"رقم العمود (column)" << "\n";
        ir << "}\n\n";
        
        // (AR) هيكل معلومات الذعر
        ir << "; " << u8"هيكل معلومات الذعر" << "\n";
        ir << "%PanicInfo = type {\n";
        ir << "    ptr,              ; " << u8"رسالة الذعر (message)" << "\n";
        ir << "    i64,              ; " << u8"طول الرسالة (message_len)" << "\n";
        ir << "    i1,               ; " << u8"هل يوجد موقع؟ (has_location)" << "\n";
        ir << "    %PanicLocation    ; " << u8"الموقع (location)" << "\n";
        ir << "}\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد معالج الذعر الافتراضي
     *        (EN) Generate default panic handler
     */
    std::string generateDefaultPanicHandler(PanicStrategy strategy) {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"معالج الذعر الافتراضي" << "\n";
        ir << "; Default Panic Handler\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        ir << "define void @__sad_panic_handler(ptr %info) noreturn {\n";
        ir << "entry:\n";
        
        switch (strategy) {
            case PanicStrategy::ABORT:
                ir << "  ; " << u8"استراتيجية: إيقاف فوري (trap)" << "\n";
                ir << "  call void @llvm.trap()\n";
                ir << "  unreachable\n";
                break;
                
            case PanicStrategy::HALT:
                ir << "  ; " << u8"استراتيجية: حلقة لا نهائية" << "\n";
                ir << "  br label %halt_loop\n";
                ir << "halt_loop:\n";
                ir << "  br label %halt_loop\n";
                break;
                
            case PanicStrategy::RESET:
                ir << "  ; " << u8"استراتيجية: إعادة تشغيل" << "\n";
                ir << "  call void @__system_reset()\n";
                ir << "  unreachable\n";
                break;
                
            default:
                ir << "  call void @llvm.trap()\n";
                ir << "  unreachable\n";
                break;
        }
        
        ir << "}\n\n";
        
        // (AR) إعلان llvm.trap
        ir << "declare void @llvm.trap() noreturn nounwind\n\n";
        
        if (strategy == PanicStrategy::RESET) {
            ir << "; " << u8"دالة إعادة تشغيل النظام (يجب توفيرها)" << "\n";
            ir << "declare void @__system_reset() noreturn\n\n";
        }
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد wrapper لمعالج الذعر المخصص
     *        (EN) Generate wrapper for custom panic handler
     */
    std::string generateCustomPanicHandlerWrapper(const PanicHandlerInfo& info) {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"غلاف معالج الذعر المخصص" << "\n";
        ir << "; Custom Panic Handler Wrapper\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) إعلان المعالج المخصص
        ir << "; " << u8"المعالج المخصص" << "\n";
        ir << "declare void @" << info.functionName << "(ptr) noreturn\n\n";
        
        // (AR) الغلاف
        ir << "define void @__sad_panic_handler(ptr %info) noreturn {\n";
        ir << "entry:\n";
        ir << "  ; " << u8"استدعاء المعالج المخصص" << "\n";
        ir << "  call void @" << info.functionName << "(ptr %info)\n";
        ir << "  ; " << u8"في حالة العودة (لا يجب أن يحدث)" << "\n";
        ir << "  call void @llvm.trap()\n";
        ir << "  unreachable\n";
        ir << "}\n\n";
        
        ir << "declare void @llvm.trap() noreturn nounwind\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد دالة panic!
     *        (EN) Generate panic! function
     */
    std::string generatePanicMacro() {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"دالة ذعر!" << "\n";
        ir << "; panic! function\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) دالة ذعر مع رسالة فقط
        ir << "; " << u8"ذعر مع رسالة" << "\n";
        ir << "define void @__sad_panic_message(ptr %msg, i64 %len) noreturn {\n";
        ir << "entry:\n";
        ir << "  %info = alloca %PanicInfo\n";
        ir << "  %msg_ptr = getelementptr %PanicInfo, ptr %info, i32 0, i32 0\n";
        ir << "  store ptr %msg, ptr %msg_ptr\n";
        ir << "  %len_ptr = getelementptr %PanicInfo, ptr %info, i32 0, i32 1\n";
        ir << "  store i64 %len, ptr %len_ptr\n";
        ir << "  %has_loc = getelementptr %PanicInfo, ptr %info, i32 0, i32 2\n";
        ir << "  store i1 false, ptr %has_loc\n";
        ir << "  call void @__sad_panic_handler(ptr %info)\n";
        ir << "  unreachable\n";
        ir << "}\n\n";
        
        // (AR) دالة ذعر مع موقع
        ir << "; " << u8"ذعر مع موقع" << "\n";
        ir << "define void @__sad_panic_location(ptr %msg, i64 %len, ptr %file, i32 %line, i32 %col) noreturn {\n";
        ir << "entry:\n";
        ir << "  %info = alloca %PanicInfo\n";
        ir << "  ; " << u8"تعبئة الرسالة" << "\n";
        ir << "  %msg_ptr = getelementptr %PanicInfo, ptr %info, i32 0, i32 0\n";
        ir << "  store ptr %msg, ptr %msg_ptr\n";
        ir << "  %len_ptr = getelementptr %PanicInfo, ptr %info, i32 0, i32 1\n";
        ir << "  store i64 %len, ptr %len_ptr\n";
        ir << "  ; " << u8"تعبئة الموقع" << "\n";
        ir << "  %has_loc = getelementptr %PanicInfo, ptr %info, i32 0, i32 2\n";
        ir << "  store i1 true, ptr %has_loc\n";
        ir << "  %loc_file = getelementptr %PanicInfo, ptr %info, i32 0, i32 3, i32 0\n";
        ir << "  store ptr %file, ptr %loc_file\n";
        ir << "  %loc_line = getelementptr %PanicInfo, ptr %info, i32 0, i32 3, i32 1\n";
        ir << "  store i32 %line, ptr %loc_line\n";
        ir << "  %loc_col = getelementptr %PanicInfo, ptr %info, i32 0, i32 3, i32 2\n";
        ir << "  store i32 %col, ptr %loc_col\n";
        ir << "  call void @__sad_panic_handler(ptr %info)\n";
        ir << "  unreachable\n";
        ir << "}\n\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد دوال التأكيد
     *        (EN) Generate assertion functions
     */
    std::string generateAssertFunctions() {
        std::ostringstream ir;
        
        ir << "; ═══════════════════════════════════════════════════════════════\n";
        ir << "; " << u8"دوال التأكيد" << "\n";
        ir << "; Assertion Functions\n";
        ir << "; ═══════════════════════════════════════════════════════════════\n\n";
        
        // (AR) تأكد! - assert
        ir << "; " << u8"تأكد!(شرط)" << "\n";
        ir << "@__assert_msg = private constant [20 x i8] c\"assertion failed!\\00\\00\"\n\n";
        
        ir << "define void @__sad_assert(i1 %cond, ptr %file, i32 %line) {\n";
        ir << "entry:\n";
        ir << "  br i1 %cond, label %ok, label %fail\n";
        ir << "ok:\n";
        ir << "  ret void\n";
        ir << "fail:\n";
        ir << "  call void @__sad_panic_location(ptr @__assert_msg, i64 17, ptr %file, i32 %line, i32 0)\n";
        ir << "  unreachable\n";
        ir << "}\n\n";
        
        // (AR) تأكد_مساواة! - assert_eq
        ir << "; " << u8"تأكد_مساواة!(أ، ب)" << "\n";
        ir << "@__assert_eq_msg = private constant [30 x i8] c\"assertion failed: a != b\\00\\00\\00\\00\\00\"\n\n";
        
        ir << "define void @__sad_assert_eq_i64(i64 %a, i64 %b, ptr %file, i32 %line) {\n";
        ir << "entry:\n";
        ir << "  %eq = icmp eq i64 %a, %b\n";
        ir << "  br i1 %eq, label %ok, label %fail\n";
        ir << "ok:\n";
        ir << "  ret void\n";
        ir << "fail:\n";
        ir << "  call void @__sad_panic_location(ptr @__assert_eq_msg, i64 24, ptr %file, i32 %line, i32 0)\n";
        ir << "  unreachable\n";
        ir << "}\n\n";
        
        // (AR) debug_assert - في Debug فقط
        ir << "; " << u8"تأكد_تصحيح! (في Debug فقط)" << "\n";
        ir << "define void @__sad_debug_assert(i1 %cond, ptr %file, i32 %line) {\n";
        ir << "entry:\n";
        ir << "  ; " << u8"في Release، هذه الدالة فارغة" << "\n";
        ir << "  ; In Release, this function is empty\n";
        ir << "  br i1 %cond, label %ok, label %fail\n";
        ir << "ok:\n";
        ir << "  ret void\n";
        ir << "fail:\n";
        ir << "  call void @__sad_panic_location(ptr @__assert_msg, i64 17, ptr %file, i32 %line, i32 0)\n";
        ir << "  unreachable\n";
        ir << "}\n\n";
        
        return ir.str();
    }
    
private:
    std::string targetArch_ = "x86_64";
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير معالج الذعر
//                    (EN) Panic Handler Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class PanicHandlerManager
 * @brief (AR) مدير معالج الذعر
 *        (EN) Panic handler manager
 */
class PanicHandlerManager {
public:
    PanicHandlerManager() {
        // (AR) الاستراتيجية الافتراضية هي الإيقاف الفوري
        strategy_ = PanicStrategy::ABORT;
    }
    
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        codeGen_.setTargetArch(arch);
    }
    
    /**
     * @brief (AR) تحديد استراتيجية الذعر
     *        (EN) Set panic strategy
     */
    void setStrategy(PanicStrategy strategy) {
        strategy_ = strategy;
    }
    
    /**
     * @brief (AR) تسجيل معالج ذعر مخصص
     *        (EN) Register custom panic handler
     */
    void registerCustomHandler(const PanicHandlerInfo& info) {
        customHandler_ = info;
        hasCustomHandler_ = true;
    }
    
    /**
     * @brief (AR) هل يوجد معالج مخصص؟
     *        (EN) Is there a custom handler?
     */
    bool hasCustomHandler() const {
        return hasCustomHandler_;
    }
    
    /**
     * @brief (AR) توليد كود معالج الذعر الكامل
     *        (EN) Generate complete panic handler code
     */
    std::string generatePanicRuntime() {
        std::ostringstream ir;
        
        // (AR) هيكل معلومات الذعر
        ir << codeGen_.generatePanicInfoStruct();
        
        // (AR) معالج الذعر
        if (hasCustomHandler_) {
            ir << codeGen_.generateCustomPanicHandlerWrapper(customHandler_);
        } else {
            ir << codeGen_.generateDefaultPanicHandler(strategy_);
        }
        
        // (AR) دوال panic
        ir << codeGen_.generatePanicMacro();
        
        // (AR) دوال التأكيد
        ir << codeGen_.generateAssertFunctions();
        
        return ir.str();
    }
    
    /**
     * @brief (AR) التحقق من صحة التكوين
     *        (EN) Validate configuration
     */
    std::vector<std::string> validate() const {
        std::vector<std::string> errors;
        
        if (hasCustomHandler_) {
            if (customHandler_.functionName.empty()) {
                errors.push_back(u8"معالج الذعر المخصص لا يملك اسم دالة");
            }
        }
        
        return errors;
    }
    
private:
    PanicStrategy strategy_;
    PanicHandlerInfo customHandler_;
    bool hasCustomHandler_ = false;
    PanicHandlerCodeGenerator codeGen_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) دوال مساعدة
//                    (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء معلومات معالج ذعر
 *        (EN) Create panic handler info
 */
PanicHandlerInfo createPanicHandlerInfo(
    const std::string& functionName,
    PanicStrategy strategy
) {
    PanicHandlerInfo info;
    info.functionName = functionName;
    info.linkName = functionName;
    info.strategy = strategy;
    info.isDefault = false;
    
    return info;
}

/**
 * @brief (AR) الحصول على اسم استراتيجية الذعر بالعربية
 *        (EN) Get panic strategy name in Arabic
 */
std::string getPanicStrategyNameArabic(PanicStrategy strategy) {
    switch (strategy) {
        case PanicStrategy::ABORT: return u8"إيقاف_فوري";
        case PanicStrategy::HALT: return u8"حلقة_لا_نهائية";
        case PanicStrategy::RESET: return u8"إعادة_تشغيل";
        case PanicStrategy::UNWIND: return u8"فك_المكدس";
        case PanicStrategy::CUSTOM: return u8"مخصص";
        default: return u8"غير_معروف";
    }
}

/**
 * @brief (AR) تحليل سمة استراتيجية الذعر
 *        (EN) Parse panic strategy attribute
 */
std::optional<PanicStrategy> parsePanicStrategyAttribute(const std::string& value) {
    if (value == u8"إيقاف" || value == "abort") {
        return PanicStrategy::ABORT;
    }
    if (value == u8"توقف" || value == "halt") {
        return PanicStrategy::HALT;
    }
    if (value == u8"إعادة_تشغيل" || value == "reset") {
        return PanicStrategy::RESET;
    }
    if (value == u8"فك" || value == "unwind") {
        return PanicStrategy::UNWIND;
    }
    
    return std::nullopt;
}

} // namespace backend
} // namespace compiler
} // namespace sad
