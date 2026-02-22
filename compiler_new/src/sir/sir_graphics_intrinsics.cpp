/**
 * =============================================================================
 * ملف: sir_graphics_intrinsics.cpp
 * الوصف: تعليمات SIR الخاصة بنظام الرسومات (Graphics Intrinsics)
 * =============================================================================
 * 
 * 🎨 ربط نظام الرسومات مع SIR
 * ═════════════════════════════════════════
 * 
 * هذا الملف يربط دوال الرسومات في stdlib/graphics مع SIR.
 * يحول استدعاءات الدوال العربية إلى تعليمات SIR خاصة.
 * 
 * مثال:
 * ```
 * كود ص:
 *     متغير نافذة = انشاء_نافذة("برنامجي"، 800، 600)
 * 
 * SIR:
 *     %0 = استدعاء @sad_ui_create_window("برنامجي", 800, 600)
 * 
 * LLVM IR:
 *     %0 = call i8* @sad_ui_create_window(i8* @.str.0, i32 800, i32 600)
 * ```
 * 
 * =============================================================================
 */

#include "sir_types.h"
#include "sir_opcodes.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>

namespace sad::sir {

// =============================================================================
// تعريف أنواع دوال الرسومات
// Graphics Function Types
// =============================================================================

/**
 * @brief نوع دالة الرسومات
 */
enum class GraphicsFunctionType {
    // ═══ إدارة النوافذ ═══
    CreateWindow,       // انشاء_نافذة
    DestroyWindow,      // اغلاق_نافذة
    UpdateWindow,       // تحديث_نافذة
    SetWindowTitle,     // عنوان_نافذة
    SetWindowSize,      // حجم_نافذة
    SetWindowPosition,  // موقع_نافذة
    ShowWindow,         // اظهار_نافذة
    HideWindow,         // اخفاء_نافذة
    
    // ═══ إنشاء الويدجات ═══
    CreateWidget,       // انشاء_عنصر
    DestroyWidget,      // حذف_عنصر
    CloneWidget,        // نسخ_عنصر
    
    // ═══ خصائص الويدجات ═══
    SetWidgetProperty,  // عنصر_خاصية
    GetWidgetProperty,  // قراءة_خاصية
    SetWidgetPosition,  // عنصر_موقع
    SetWidgetSize,      // عنصر_حجم
    SetWidgetText,      // عنصر_نص
    SetWidgetColor,     // عنصر_لون
    SetWidgetVisible,   // عنصر_مرئي
    SetWidgetEnabled,   // عنصر_مفعل
    
    // ═══ التخطيط والأبناء ═══
    AddChild,           // اضافة_ابن
    RemoveChild,        // حذف_ابن
    GetChildren,        // ابناء
    GetParent,          // الاب
    
    // ═══ الأحداث ═══
    SetEventHandler,    // معالج_حدث
    WaitEvent,          // انتظار_حدث
    PollEvent,          // فحص_حدث
    
    // ═══ الرسم المباشر ═══
    DrawRect,           // ارسم_مربع
    DrawCircle,         // ارسم_دائرة
    DrawLine,           // ارسم_خط
    DrawText,           // ارسم_نص
    DrawImage,          // ارسم_صورة
    FillRect,           // املأ_مربع
    FillCircle,         // املأ_دائرة
    SetColor,           // حدد_لون
    Clear,              // امسح
    Present,            // اعرض
    
    // ═══ الويدجات المسبقة ═══
    CreateButton,       // زر
    CreateLabel,        // نص
    CreateTextField,    // حقل_نص
    CreateCheckbox,     // خانة_اختيار
    CreateSlider,       // منزلق
    CreateProgressBar,  // شريط_تقدم
    CreateList,         // قائمة
    CreateContainer,    // حاوية
    CreateColumn,       // عمود
    CreateRow,          // صف
    
    // ═══ دوال مساعدة ═══
    ParseColor,         // تحليل_لون
    GetScreenSize,      // حجم_الشاشة
    GetMousePosition,   // موقع_الفأرة
    IsKeyPressed,       // مفتاح_مضغوط
    
    Unknown
};

// =============================================================================
// جدول ربط الأسماء العربية بأنواع الدوال
// Arabic Names to Function Types Mapping
// =============================================================================

/**
 * @brief جدول ربط الأسماء العربية
 */
class GraphicsFunctionTable {
public:
    static GraphicsFunctionTable& getInstance() {
        static GraphicsFunctionTable instance;
        return instance;
    }
    
    /**
     * @brief البحث عن نوع الدالة بالاسم العربي
     */
    std::optional<GraphicsFunctionType> lookup(const std::string& name) const {
        auto it = nameToType_.find(name);
        if (it != nameToType_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * @brief الحصول على اسم دالة Runtime
     */
    std::string getRuntimeName(GraphicsFunctionType type) const {
        auto it = typeToRuntime_.find(type);
        if (it != typeToRuntime_.end()) {
            return it->second;
        }
        return "";
    }
    
    /**
     * @brief هل هذه دالة رسومات؟
     */
    bool isGraphicsFunction(const std::string& name) const {
        return nameToType_.find(name) != nameToType_.end();
    }

private:
    GraphicsFunctionTable() {
        initialize();
    }
    
    void initialize() {
        // ═══ إدارة النوافذ ═══
        addMapping("انشاء_نافذة", GraphicsFunctionType::CreateWindow, "sad_ui_create_window");
        addMapping("اغلاق_نافذة", GraphicsFunctionType::DestroyWindow, "sad_ui_destroy_window");
        addMapping("تحديث_نافذة", GraphicsFunctionType::UpdateWindow, "sad_ui_update_window");
        addMapping("عنوان_نافذة", GraphicsFunctionType::SetWindowTitle, "sad_ui_set_window_title");
        addMapping("حجم_نافذة", GraphicsFunctionType::SetWindowSize, "sad_ui_set_window_size");
        addMapping("موقع_نافذة", GraphicsFunctionType::SetWindowPosition, "sad_ui_set_window_position");
        addMapping("اظهار_نافذة", GraphicsFunctionType::ShowWindow, "sad_ui_show_window");
        addMapping("اخفاء_نافذة", GraphicsFunctionType::HideWindow, "sad_ui_hide_window");
        
        // ═══ إنشاء الويدجات ═══
        addMapping("انشاء_عنصر", GraphicsFunctionType::CreateWidget, "sad_ui_create_widget");
        addMapping("حذف_عنصر", GraphicsFunctionType::DestroyWidget, "sad_ui_destroy_widget");
        addMapping("نسخ_عنصر", GraphicsFunctionType::CloneWidget, "sad_ui_clone_widget");
        
        // ═══ خصائص الويدجات ═══
        addMapping("عنصر_خاصية", GraphicsFunctionType::SetWidgetProperty, "sad_ui_set_property");
        addMapping("قراءة_خاصية", GraphicsFunctionType::GetWidgetProperty, "sad_ui_get_property");
        addMapping("عنصر_موقع", GraphicsFunctionType::SetWidgetPosition, "sad_ui_set_position");
        addMapping("عنصر_حجم", GraphicsFunctionType::SetWidgetSize, "sad_ui_set_size");
        addMapping("عنصر_نص", GraphicsFunctionType::SetWidgetText, "sad_ui_set_text");
        addMapping("عنصر_لون", GraphicsFunctionType::SetWidgetColor, "sad_ui_set_color");
        addMapping("عنصر_مرئي", GraphicsFunctionType::SetWidgetVisible, "sad_ui_set_visible");
        addMapping("عنصر_مفعل", GraphicsFunctionType::SetWidgetEnabled, "sad_ui_set_enabled");
        
        // ═══ التخطيط والأبناء ═══
        addMapping("اضافة_ابن", GraphicsFunctionType::AddChild, "sad_ui_add_child");
        addMapping("حذف_ابن", GraphicsFunctionType::RemoveChild, "sad_ui_remove_child");
        addMapping("ابناء", GraphicsFunctionType::GetChildren, "sad_ui_get_children");
        addMapping("الاب", GraphicsFunctionType::GetParent, "sad_ui_get_parent");
        
        // ═══ الأحداث ═══
        addMapping("معالج_حدث", GraphicsFunctionType::SetEventHandler, "sad_ui_set_handler");
        addMapping("انتظار_حدث", GraphicsFunctionType::WaitEvent, "sad_ui_wait_event");
        addMapping("فحص_حدث", GraphicsFunctionType::PollEvent, "sad_ui_poll_event");
        
        // ═══ الرسم المباشر ═══
        addMapping("ارسم_مربع", GraphicsFunctionType::DrawRect, "sad_ui_draw_rect");
        addMapping("ارسم_دائرة", GraphicsFunctionType::DrawCircle, "sad_ui_draw_circle");
        addMapping("ارسم_خط", GraphicsFunctionType::DrawLine, "sad_ui_draw_line");
        addMapping("ارسم_نص", GraphicsFunctionType::DrawText, "sad_ui_draw_text");
        addMapping("ارسم_صورة", GraphicsFunctionType::DrawImage, "sad_ui_draw_image");
        addMapping("املأ_مربع", GraphicsFunctionType::FillRect, "sad_ui_fill_rect");
        addMapping("املأ_دائرة", GraphicsFunctionType::FillCircle, "sad_ui_fill_circle");
        addMapping("حدد_لون", GraphicsFunctionType::SetColor, "sad_ui_set_draw_color");
        addMapping("امسح", GraphicsFunctionType::Clear, "sad_ui_clear");
        addMapping("اعرض", GraphicsFunctionType::Present, "sad_ui_present");
        
        // ═══ الويدجات المسبقة ═══
        addMapping("زر", GraphicsFunctionType::CreateButton, "sad_ui_button");
        addMapping("نص", GraphicsFunctionType::CreateLabel, "sad_ui_label");
        addMapping("حقل_نص", GraphicsFunctionType::CreateTextField, "sad_ui_textfield");
        addMapping("خانة_اختيار", GraphicsFunctionType::CreateCheckbox, "sad_ui_checkbox");
        addMapping("منزلق", GraphicsFunctionType::CreateSlider, "sad_ui_slider");
        addMapping("شريط_تقدم", GraphicsFunctionType::CreateProgressBar, "sad_ui_progressbar");
        addMapping("قائمة", GraphicsFunctionType::CreateList, "sad_ui_list");
        addMapping("حاوية", GraphicsFunctionType::CreateContainer, "sad_ui_container");
        addMapping("عمود", GraphicsFunctionType::CreateColumn, "sad_ui_column");
        addMapping("صف", GraphicsFunctionType::CreateRow, "sad_ui_row");
        
        // ═══ دوال مساعدة ═══
        addMapping("تحليل_لون", GraphicsFunctionType::ParseColor, "sad_ui_parse_color");
        addMapping("حجم_الشاشة", GraphicsFunctionType::GetScreenSize, "sad_ui_get_screen_size");
        addMapping("موقع_الفأرة", GraphicsFunctionType::GetMousePosition, "sad_ui_get_mouse_pos");
        addMapping("مفتاح_مضغوط", GraphicsFunctionType::IsKeyPressed, "sad_ui_is_key_pressed");
        
        // ═══ أسماء إنجليزية بديلة ═══
        addMapping("create_window", GraphicsFunctionType::CreateWindow, "sad_ui_create_window");
        addMapping("destroy_window", GraphicsFunctionType::DestroyWindow, "sad_ui_destroy_window");
        addMapping("create_widget", GraphicsFunctionType::CreateWidget, "sad_ui_create_widget");
        addMapping("set_property", GraphicsFunctionType::SetWidgetProperty, "sad_ui_set_property");
        addMapping("button", GraphicsFunctionType::CreateButton, "sad_ui_button");
        addMapping("label", GraphicsFunctionType::CreateLabel, "sad_ui_label");
        addMapping("column", GraphicsFunctionType::CreateColumn, "sad_ui_column");
        addMapping("row", GraphicsFunctionType::CreateRow, "sad_ui_row");
    }
    
    void addMapping(const std::string& arabicName, 
                   GraphicsFunctionType type,
                   const std::string& runtimeName) {
        nameToType_[arabicName] = type;
        typeToRuntime_[type] = runtimeName;
    }
    
    std::unordered_map<std::string, GraphicsFunctionType> nameToType_;
    std::unordered_map<GraphicsFunctionType, std::string> typeToRuntime_;
};

// =============================================================================
// توليد تعليمات SIR للرسومات
// SIR Graphics Instructions Generator
// =============================================================================

/**
 * @brief مولد تعليمات SIR للرسومات
 */
class GraphicsSIRGenerator {
public:
    /**
     * @brief تحويل استدعاء دالة رسومات إلى تعليمة SIR
     */
    static std::optional<SirInstruction> generateInstruction(
        const std::string& functionName,
        const std::vector<size_t>& argIds,
        size_t resultId
    ) {
        auto& table = GraphicsFunctionTable::getInstance();
        
        auto funcType = table.lookup(functionName);
        if (!funcType.has_value()) {
            return std::nullopt;
        }
        
        std::string runtimeName = table.getRuntimeName(*funcType);
        
        // إنشاء تعليمة استدعاء
        SirInstruction inst;
        inst.opcode = SirOpcode::استدعاء; // CALL
        inst.result = resultId;
        
        // إضافة اسم الدالة كمعامل
        inst.operands.push_back(SirOperand::makeSymbol(runtimeName));
        
        // إضافة المعاملات
        for (size_t argId : argIds) {
            inst.operands.push_back(SirOperand::makeValue(argId));
        }
        
        return inst;
    }
    
    /**
     * @brief توليد كود تهيئة الرسومات
     */
    static std::vector<SirInstruction> generateInitCode() {
        std::vector<SirInstruction> instructions;
        
        // استدعاء sad_ui_init()
        SirInstruction init;
        init.opcode = SirOpcode::استدعاء;
        init.operands.push_back(SirOperand::makeSymbol("sad_ui_init"));
        instructions.push_back(init);
        
        return instructions;
    }
    
    /**
     * @brief توليد كود إنهاء الرسومات
     */
    static std::vector<SirInstruction> generateCleanupCode() {
        std::vector<SirInstruction> instructions;
        
        // استدعاء sad_ui_quit()
        SirInstruction quit;
        quit.opcode = SirOpcode::استدعاء;
        quit.operands.push_back(SirOperand::makeSymbol("sad_ui_quit"));
        instructions.push_back(quit);
        
        return instructions;
    }
};

// =============================================================================
// تحويل AST إلى SIR للرسومات
// AST to SIR Conversion for Graphics
// =============================================================================

/**
 * @brief زائر AST لتحويل الرسومات
 */
class GraphicsASTVisitor {
public:
    /**
     * @brief هل هذا استدعاء دالة رسومات؟
     */
    static bool isGraphicsCall(const std::string& functionName) {
        return GraphicsFunctionTable::getInstance().isGraphicsFunction(functionName);
    }
    
    /**
     * @brief تحويل استدعاء رسومات إلى SIR
     */
    static std::vector<SirInstruction> convertGraphicsCall(
        const std::string& functionName,
        const std::vector<size_t>& argIds,
        size_t resultId
    ) {
        std::vector<SirInstruction> result;
        
        auto inst = GraphicsSIRGenerator::generateInstruction(
            functionName, argIds, resultId);
        
        if (inst.has_value()) {
            result.push_back(*inst);
        }
        
        return result;
    }
};

} // namespace sad::sir
