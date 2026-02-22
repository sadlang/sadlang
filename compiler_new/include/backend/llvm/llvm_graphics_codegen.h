/**
 * =============================================================================
 * ملف: llvm_graphics_codegen.h
 * الوصف: رأس ملف توليد كود LLVM IR لنظام الرسومات
 * =============================================================================
 * 
 * 🎨 نظام الرسومات مع LLVM
 * ═════════════════════════════════════════
 * 
 * يوفر هذا الملف واجهة لتوليد كود LLVM IR لنظام الرسومات.
 * يدعم:
 * - Windows (PE/EXE)
 * - Linux (ELF)
 * - macOS (Mach-O)
 * 
 * الاستخدام:
 * ```cpp
 * #include "llvm_graphics_codegen.h"
 * 
 * using namespace Sad::LLVM::Graphics;
 * 
 * LLVMGraphicsCodeGen codegen;
 * codegen.initialize("my_app", "x86_64-pc-windows-msvc");
 * codegen.generateAll("تطبيقي", 800, 600);
 * codegen.verify();
 * codegen.emitToFile("my_app.ll");
 * ```
 * 
 * =============================================================================
 */

#ifndef SAD_LLVM_GRAPHICS_CODEGEN_H
#define SAD_LLVM_GRAPHICS_CODEGEN_H

#ifdef HAS_LLVM

#include <memory>
#include <string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

namespace Sad {
namespace LLVM {
namespace Graphics {

// Forward declarations
class GraphicsTypeGenerator;
class SDL2Declarations;
class GraphicsRuntimeGenerator;
class GraphicsProgramGenerator;

// =============================================================================
// LLVMGraphicsCodeGen — مولد كود الرسومات الرئيسي
// =============================================================================

/**
 * @brief مولد كود LLVM للرسومات
 * 
 * (AR) يترجم كود الرسومات من لغة ص إلى LLVM IR
 *      ثم يولد ملفات تنفيذية أصلية لجميع المنصات.
 * 
 * (EN) Translates graphics code from Sad language to LLVM IR
 *      then generates native executables for all platforms.
 * 
 * الميزات:
 * - دعم SDL2 الكامل
 * - توليد دوال Runtime تلقائياً
 * - دعم جميع أنظمة التشغيل
 */
class LLVMGraphicsCodeGen {
public:
    LLVMGraphicsCodeGen();
    ~LLVMGraphicsCodeGen();
    
    // منع النسخ
    LLVMGraphicsCodeGen(const LLVMGraphicsCodeGen&) = delete;
    LLVMGraphicsCodeGen& operator=(const LLVMGraphicsCodeGen&) = delete;
    
    // ═══════════════════════════════════════════════════════════════════════
    // التهيئة / Initialization
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تهيئة المولد
     * @param moduleName اسم الوحدة / Module name
     * @param targetTriple المنصة المستهدفة / Target platform
     * 
     * أمثلة للمنصات:
     * - "x86_64-pc-windows-msvc"   (Windows 64-bit)
     * - "x86_64-pc-linux-gnu"      (Linux 64-bit)
     * - "x86_64-apple-macosx"      (macOS 64-bit)
     * - "aarch64-linux-gnu"        (Linux ARM64)
     * - "aarch64-apple-macosx"     (macOS ARM64)
     */
    void initialize(const std::string& moduleName,
                   const std::string& targetTriple = "");
    
    // ═══════════════════════════════════════════════════════════════════════
    // توليد الكود / Code Generation
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توليد كل كود الرسومات
     * @param windowTitle عنوان النافذة / Window title
     * @param width عرض النافذة / Window width
     * @param height ارتفاع النافذة / Window height
     */
    void generateAll(const std::string& windowTitle = "Sad Application",
                    int width = 800, int height = 600);
    
    /**
     * @brief توليد الأنواع فقط
     */
    void generateTypes();
    
    /**
     * @brief توليد تعريفات SDL2 فقط
     */
    void generateSDLDeclarations();
    
    /**
     * @brief توليد دوال Runtime فقط
     */
    void generateRuntimeFunctions();
    
    /**
     * @brief توليد دالة main فقط
     */
    void generateMainFunction(const std::string& windowTitle,
                             int width, int height);
    
    // ═══════════════════════════════════════════════════════════════════════
    // الويدجات / Widgets
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief إضافة كود ويدجت
     * @param widgetType نوع الويدجت
     * @param properties خصائص الويدجت
     */
    void addWidget(int widgetType,
                   const std::unordered_map<std::string, std::string>& properties);
    
    /**
     * @brief إضافة معالج حدث
     * @param widgetId معرف الويدجت
     * @param eventType نوع الحدث
     * @param handlerCode كود المعالج
     */
    void addEventHandler(int widgetId, int eventType,
                        const std::string& handlerCode);
    
    // ═══════════════════════════════════════════════════════════════════════
    // التحقق والإخراج / Verification & Output
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief التحقق من صحة الوحدة
     * @return true إذا كانت صحيحة
     */
    bool verify();
    
    /**
     * @brief حفظ LLVM IR إلى ملف
     * @param filename اسم الملف
     * @return true إذا نجح الحفظ
     */
    bool emitToFile(const std::string& filename);
    
    /**
     * @brief حفظ كود object إلى ملف
     * @param filename اسم الملف (.o أو .obj)
     * @return true إذا نجح الحفظ
     */
    bool emitObjectFile(const std::string& filename);
    
    /**
     * @brief حفظ ملف تنفيذي
     * @param filename اسم الملف (.exe أو بدون امتداد)
     * @return true إذا نجح الحفظ
     */
    bool emitExecutable(const std::string& filename);
    
    /**
     * @brief الحصول على LLVM IR كنص
     * @return نص LLVM IR
     */
    std::string getIRString();
    
    /**
     * @brief طباعة LLVM IR إلى stdout
     */
    void dump();
    
    // ═══════════════════════════════════════════════════════════════════════
    // الوصول للمكونات / Component Access
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief الحصول على الوحدة
     */
    llvm::Module* getModule();
    
    /**
     * @brief الحصول على السياق
     */
    llvm::LLVMContext* getContext();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// =============================================================================
// دوال مساعدة / Helper Functions
// =============================================================================

/**
 * @brief الحصول على target triple للمنصة الحالية
 */
std::string getHostTargetTriple();

/**
 * @brief التحقق من توفر SDL2
 */
bool isSDL2Available();

/**
 * @brief الحصول على مسار مكتبة SDL2
 */
std::string getSDL2LibraryPath();

} // namespace Graphics
} // namespace LLVM
} // namespace Sad

#endif // HAS_LLVM

#endif // SAD_LLVM_GRAPHICS_CODEGEN_H
