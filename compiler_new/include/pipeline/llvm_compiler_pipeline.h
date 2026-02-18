/*
 * ============================================================================
 * LLVM Compiler Pipeline - خط أنابيب مترجم LLVM
 * LLVM Compiler Pipeline Header
 * ============================================================================
 * 
 * الوصف (Description):
 *   خط أنابيب متكامل لترجمة كود لغة ص إلى native code باستخدام LLVM.
 *   يربط جميع مكونات المترجم: Parser → Type Checker → SIR → LLVM → Executable
 * 
 *   الجديد: دعم كامل لوضع Freestanding (بلا مكتبة قياسية)
 *   عند اكتشاف #![بلا_مكتبة_قياسية] يُفعَّل تلقائياً:
 *     - رفض رموز المكتبة القياسية
 *     - التحقق من وجود نقطة دخول ومعالج ذعر
 *     - توليد كود مناسب للـ bare-metal
 * 
 *   Integrated pipeline for compiling Sad code to native code using LLVM.
 *   Connects all compiler components: Parser → Type Checker → SIR → LLVM → Executable
 * 
 * المسار (Pipeline):
 *   .ص file → Lexer → Parser → AST → [فحص no_std] → (Type Checker) → SIR Builder → 
 *   [FreestandingCodeGen] → LLVM CodeGen → Optimizer → Object File → Linker → Executable
 * 
 * الاستخدام (Usage):
 *   ```cpp
 *   LLVMCompilerPipeline pipeline;
 *   pipeline.initialize();
 *   auto result = pipeline.compile("kernel.ص");
 *   if (result.success) {
 *       pipeline.emitExecutable("kernel.bin");
 *   }
 *   ```
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): January 4, 2026
 * الإصدار (Version): 1.1.0 — دعم Freestanding
 * ============================================================================
 */

#ifndef SAD_LLVM_COMPILER_PIPELINE_H
#define SAD_LLVM_COMPILER_PIPELINE_H

#include <memory>
#include <string>
#include <vector>
#include <chrono>

// Sad Components - مكونات لغة ص
// (AR) مسارات التضمين مُعدّلة لتعمل مع مسارات CMake
// (EN) Include paths adjusted to work with CMake include directories
#include "lexer_core.h"
#include "parser_core.h"
// (AR) تصريح مسبق لفاحص الأنواع — التنفيذ مُعطّل حالياً (قيد التطوير)
// (EN) Forward declaration for TypeChecker — implementation currently disabled (WIP)
namespace Sad { namespace Semantic { class TypeChecker; } }
#include "frontend/sir_builder.h"
#include "backend/llvm/llvm_codegen.h"
#include "backend/llvm/llvm_optimizer.h"
#include "pipeline/compiler_options.h"

// ─── دعم وضع Freestanding (بلا مكتبة قياسية) ──────────────────────────────
// هذه الرؤوس تُوفِّر:
//   - NoStdIntegration: فحص المصدر لسمات #![بلا_مكتبة_قياسية]
//   - NoStdModeManager: إدارة وضع no_std
//   - FreestandingCodeGen: توليد كود متوافق مع bare-metal
// ---
// Freestanding mode support headers provide:
//   - NoStdIntegration: scan source for #![no_std] attributes
//   - NoStdModeManager: manage no_std mode
//   - FreestandingCodeGen: generate bare-metal compatible code
// ─────────────────────────────────────────────────────────────────────────────
#include "pipeline/no_std_integration.h"
#include "pipeline/no_std_mode.h"
#include "pipeline/freestanding_codegen.h"

namespace Sad {
namespace Compiler {
namespace LLVM {

// ============================================================================
// Forward Declarations - التصريحات المسبقة
// ============================================================================

class LLVMCompilerPipeline;

// ============================================================================
// CompilationStats - إحصائيات الترجمة
// Compilation Statistics
// ============================================================================

/**
 * إحصائيات تفصيلية عن عملية الترجمة
 * Detailed compilation process statistics
 */
struct CompilationStats {
    // الأوقات / Timings
    double lexerTime = 0.0;           // وقت المحلل المعجمي / Lexer time (ms)
    double parserTime = 0.0;          // وقت المحلل النحوي / Parser time (ms)
    double typeCheckTime = 0.0;       // وقت فحص الأنواع / Type checking time (ms)
    double sirBuildTime = 0.0;        // وقت بناء SIR / SIR build time (ms)
    double codeGenTime = 0.0;         // وقت توليد الكود / Code generation time (ms)
    double optimizationTime = 0.0;    // وقت التحسين / Optimization time (ms)
    double emitTime = 0.0;            // وقت الإصدار / Emission time (ms)
    double totalTime = 0.0;           // الوقت الإجمالي / Total time (ms)
    
    // الإحصائيات / Statistics
    size_t tokenCount = 0;            // عدد الرموز / Token count
    size_t astNodeCount = 0;          // عدد عقد AST / AST node count
    size_t sirInstructionCount = 0;   // عدد تعليمات SIR / SIR instruction count
    size_t llvmIRSize = 0;            // حجم LLVM IR / LLVM IR size (bytes)
    size_t objectFileSize = 0;        // حجم ملف Object / Object file size (bytes)
    size_t executableSize = 0;        // حجم Executable / Executable size (bytes)
    
    // الإحصائيات المتقدمة / Advanced statistics
    size_t functionsCount = 0;        // عدد الدوال / Function count
    size_t classesCount = 0;          // عدد الأصناف / Class count
    size_t globalVarsCount = 0;       // عدد المتغيرات العامة / Global variable count
    
    /**
     * طباعة الإحصائيات / Print statistics
     */
    void print() const;
    
    /**
     * حساب السرعة بالتعليمات/ثانية / Calculate speed in instructions/second
     */
    double getInstructionsPerSecond() const {
        if (totalTime <= 0) return 0.0;
        return (sirInstructionCount * 1000.0) / totalTime;
    }
};

// ============================================================================
// CompilationResult - نتيجة الترجمة
// Compilation Result
// ============================================================================

/**
 * نتيجة عملية الترجمة مع معلومات النجاح/الفشل
 * Compilation result with success/failure information
 */
struct CompilationResult {
    bool success = false;              // هل نجحت الترجمة؟ / Was compilation successful?
    std::string errorMessage;          // رسالة الخطأ (إن وجدت) / Error message (if any)
    std::vector<std::string> errors;   // قائمة الأخطاء / Error list
    std::vector<std::string> warnings; // قائمة التحذيرات / Warning list
    
    CompilationStats stats;            // الإحصائيات / Statistics
    
    std::string outputFile;            // ملف الإخراج / Output file
    std::string llvmIRFile;            // ملف LLVM IR (اختياري) / LLVM IR file (optional)
    std::string assemblyFile;          // ملف Assembly (اختياري) / Assembly file (optional)
    std::string objectFile;            // ملف Object / Object file
    
    /**
     * طباعة النتيجة / Print result
     */
    void print() const;
};

// ============================================================================
// LLVMCompilerPipeline - خط أنابيب المترجم الرئيسي
// Main Compiler Pipeline
// ============================================================================

/**
 * خط أنابيب المترجم الرئيسي - يدير عملية الترجمة الكاملة
 * Main compiler pipeline - manages complete compilation process
 * 
 * يربط جميع المكونات:
 * Connects all components:
 * 1. Lexer: تحويل النص إلى رموز / Convert text to tokens
 * 2. Parser: بناء AST / Build AST
 * 3. Type Checker: التحقق من الأنواع (اختياري) / Type checking (optional)
 * 4. SIR Builder: بناء التمثيل الوسيط / Build intermediate representation
 * 5. LLVM CodeGen: توليد LLVM IR / Generate LLVM IR
 * 6. Optimizer: تحسين الكود / Optimize code
 * 7. Emitter: إصدار Object file / Emit object file
 * 8. Linker: ربط Executable / Link executable
 */
class LLVMCompilerPipeline {
public:
    /**
     * المنشئ / Constructor
     * @param options خيارات الترجمة / Compilation options
     */
    explicit LLVMCompilerPipeline(const Sad::Compiler::CompilerOptions& options);
    
    /**
     * المنشئ الافتراضي / Default constructor
     */
    LLVMCompilerPipeline();
    
    /**
     * المدمر / Destructor
     */
    ~LLVMCompilerPipeline();
    
    // ========================================================================
    // Public Interface - الواجهة العامة
    // ========================================================================
    
    /**
     * تهيئة Pipeline / Initialize pipeline
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize();
    
    /**
     * ترجمة ملف / Compile file
     * @param sourceFile مسار ملف المصدر / Source file path
     * @return نتيجة الترجمة / Compilation result
     */
    CompilationResult compileFile(const std::string& sourceFile);
    
    /**
     * ترجمة كود مباشر / Compile source code
     * @param sourceCode كود المصدر / Source code
     * @param filename اسم الملف (للأخطاء) / Filename (for errors)
     * @return نتيجة الترجمة / Compilation result
     */
    CompilationResult compileSource(const std::string& sourceCode, const std::string& filename = "<source>");
    
    /**
     * إصدار LLVM IR إلى ملف / Emit LLVM IR to file
     * @param filename اسم الملف / Filename
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitLLVMIR(const std::string& filename);
    
    /**
     * إصدار Assembly إلى ملف / Emit assembly to file
     * @param filename اسم الملف / Filename
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitAssembly(const std::string& filename);
    
    /**
     * إصدار Object file / Emit object file
     * @param filename اسم الملف / Filename
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitObjectFile(const std::string& filename);
    
    /**
     * إصدار Executable / Emit executable
     * @param filename اسم الملف / Filename
     * @return true إذا نجح الإصدار / true if emission succeeded
     */
    bool emitExecutable(const std::string& filename);
    
    /**
     * الحصول على الوحدة الحالية / Get current module
     * @return مؤشر للوحدة / Pointer to module
     */
    llvm::Module* getCurrentModule();
    
    /**
     * الحصول على الخيارات / Get options
     * @return مرجع للخيارات / Reference to options
     */
    const Sad::Compiler::CompilerOptions& getOptions() const { return options_; }
    
    /**
     * تعيين الخيارات / Set options
     * @param options الخيارات الجديدة / New options
     */
    void setOptions(const Sad::Compiler::CompilerOptions& options) { options_ = options; }
    
    /**
     * الحصول على آخر نتيجة ترجمة / Get last compilation result
     * @return النتيجة / Result
     */
    const CompilationResult& getLastResult() const { return lastResult_; }
    
private:
    // ========================================================================
    // Pipeline Stages - مراحل Pipeline
    // ========================================================================
    
    /**
     * المرحلة 1: التحليل المعجمي / Stage 1: Lexical analysis
     * @param source الكود المصدري / Source code
     * @param filename اسم الملف / Filename
     * @return true إذا نجحت / true if succeeded
     */
    bool lexicalAnalysis(const std::string& source, const std::string& filename);
    
    /**
     * المرحلة 2: التحليل النحوي / Stage 2: Parsing
     * @return true إذا نجحت / true if succeeded
     */
    bool parsing();
    
    /**
     * المرحلة 3: فحص الأنواع / Stage 3: Type checking (optional)
     * @return true إذا نجحت / true if succeeded
     */
    bool typeChecking();
    
    /**
     * المرحلة 4: بناء SIR / Stage 4: SIR building
     * @return true إذا نجحت / true if succeeded
     */
    bool sirBuilding();
    
    /**
     * المرحلة 5: توليد LLVM IR / Stage 5: LLVM IR generation
     * @return true إذا نجحت / true if succeeded
     */
    bool codeGeneration();
    
    /**
     * المرحلة 6: التحسين / Stage 6: Optimization
     * @return true إذا نجحت / true if succeeded
     */
    bool optimization();
    
    /**
     * المرحلة 7: الإصدار / Stage 7: Emission
     * @param outputType نوع الإخراج / Output type
     * @param filename اسم الملف / Filename
     * @return true إذا نجحت / true if succeeded
     */
    bool emission(const std::string& outputType, const std::string& filename);

    // ─── مراحل Freestanding ────────────────────────────────────────────────

    /**
     * (AR) فحص الكود المصدري لسمات #![بلا_مكتبة_قياسية]
     *      يُشغَّل قبل مرحلة التحليل النحوي
     *      يُحدِّث options_.no_std و freestandingModeActive_
     *
     *      السمات التي يبحث عنها:
     *        #![بلا_مكتبة_قياسية]  أو  #![no_std]
     *        #![بلا_رئيسية]        أو  #![no_main]
     *        #![إيقاف_عند_ذعر]     أو  #![abort_on_panic]
     *        #[نقطة_دخول]          أو  #[entry_point]
     *        #[معالج_ذعر]          أو  #[panic_handler]
     *
     * @param sourceCode الكود المصدري للفحص
     * @return true دائماً (الفحص لا يُفشِل الترجمة، فقط يُعدِّل الحالة)
     *
     * (EN) Scan source code for #![no_std] attributes
     *      Runs before parsing stage
     *      Updates options_.no_std and freestandingModeActive_
     */
    bool scanForFreestandingAttributes(const std::string& sourceCode);

    /**
     * (AR) تهيئة وضع Freestanding بعد اكتشافه
     *      يُنشئ FreestandingCodeGen ويُعدِّه بالإعدادات المكتشفة
     *      يُسجِّل وحدة الترجمة لبدء التتبع
     *
     * @param filename اسم الملف المصدري (للتشخيص)
     * @return true إذا نجحت التهيئة
     *
     * (EN) Initialize freestanding mode after detection
     *      Creates FreestandingCodeGen with discovered settings
     */
    bool initializeFreestandingMode(const std::string& filename);

    /**
     * (AR) التحقق النهائي من اكتمال وحدة freestanding
     *      يتحقق من:
     *        ✓ وجود نقطة_دخول (#[نقطة_دخول] أو #[entry_point])
     *        ✓ وجود معالج_ذعر (#[معالج_ذعر] أو #[panic_handler])
     *        ✓ عدم وجود استخدامات محظورة لرموز المكتبة القياسية
     *
     *      يُشغَّل في نهاية مرحلة بناء SIR
     *
     * @return true إذا نجح التحقق
     *
     * (EN) Final validation of freestanding unit completeness
     */
    bool finalizeFreestandingUnit();

    /**
     * (AR) فحص الرمز في وضع freestanding
     *      يُشغَّل عند مصادفة رمز أثناء بناء SIR
     *
     * @param symbolName اسم الرمز المراد فحصه
     * @return true إذا كان الرمز مقبولاً
     *
     * (EN) Check symbol in freestanding mode
     */
    bool checkFreestandingSymbol(const std::string& symbolName);
    
    // ========================================================================
    // Helper Functions - الدوال المساعدة
    // ========================================================================
    
    /**
     * قراءة ملف / Read file
     * @param filename اسم الملف / Filename
     * @return محتوى الملف / File content
     */
    std::string readFile(const std::string& filename);
    
    /**
     * تنظيف الموارد / Cleanup resources
     */
    void cleanup();
    
    /**
     * قياس الوقت / Measure time
     * @param start نقطة البداية / Start point
     * @return الوقت بالميلي ثانية / Time in milliseconds
     */
    double measureTime(const std::chrono::high_resolution_clock::time_point& start) const;
    
    /**
     * تسجيل خطأ / Log error
     * @param message رسالة الخطأ / Error message
     */
    void logError(const std::string& message);
    
    /**
     * تسجيل تحذير / Log warning
     * @param message رسالة التحذير / Warning message
     */
    void logWarning(const std::string& message);
    
    // ========================================================================
    // Member Variables - المتغيرات الأعضاء
    // ========================================================================
    
    // الخيارات / Options
    Sad::Compiler::CompilerOptions options_;
    
    // المكونات / Components
    std::unique_ptr<Lexer::LexerCore> lexer_;
    std::unique_ptr<Parser::ParserCore> parser_;
    // (AR) فاحص الأنواع — مُعطّل حالياً (قيد التطوير)
    // (EN) Type checker — currently disabled (under development)
    // std::unique_ptr<Sad::Semantic::TypeChecker> typeChecker_;
    std::unique_ptr<SIR::SIRBuilder> sirBuilder_;
    std::unique_ptr<Sad::LLVM::LLVMCodeGen> codeGen_;

    // ─── مكونات وضع Freestanding ────────────────────────────────────────────
    // هذه المكونات تُنشأ فقط عند تفعيل وضع no_std
    // (تلقائياً عند اكتشاف #![بلا_مكتبة_قياسية] في الكود المصدري)
    //
    // Freestanding mode components — created only when no_std is active
    // (auto-activated when #![no_std] is detected in source)
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * (AR) نتيجة فحص المصدر لسمات no_std
     *      تحتوي: هل وُجد #![بلا_مكتبة_قياسية]؟ اسم نقطة الدخول؟ المعالج؟
     * (EN) Result of scanning source for no_std attributes
     */
    sad::compiler::pipeline::NoStdScanResult noStdScanResult_;

    /**
     * (AR) إعدادات وضع no_std المُعدَّة من نتيجة الفحص
     *      تُمرَّر لـ FreestandingCodeGen
     * (EN) no_std configuration built from scan result
     */
    sad::compiler::pipeline::NoStdConfig noStdConfig_;

    /**
     * (AR) مولّد الكود الخاص بوضع freestanding
     *      يتولى:
     *        - توليد تسميات كتل LLVM IR الصحيحة
     *        - التحقق من الأنواع والرموز المحظورة
     *        - تتبع اكتمال الوحدة (نقطة_دخول، معالج_ذعر)
     * (EN) Code generator for freestanding mode
     */
    std::unique_ptr<sad::compiler::freestanding::FreestandingCodeGen> freestandingCodeGen_;

    /**
     * (AR) هل وضع freestanding مُفعَّل للوحدة الحالية؟
     *      يُحدَّث بعد مرحلة فحص المصدر
     * (EN) Is freestanding mode active for the current unit?
     */
    bool freestandingModeActive_ = false;
    
    // البيانات المؤقتة / Temporary data
    std::vector<Lexer::Token> tokens_;
    AST::StmtList ast_;
    std::shared_ptr<SIR::SIRModule> sirModule_;
    std::unique_ptr<llvm::Module> llvmModule_;
    
    // النتائج / Results
    CompilationResult lastResult_;
    
    // Error and warning tracking / تتبع الأخطاء والتحذيرات
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // الحالة / State
    bool initialized_ = false;
    bool hasErrors_ = false;
};

} // namespace LLVM
} // namespace Compiler
} // namespace Sad

#endif // SAD_LLVM_COMPILER_PIPELINE_H
