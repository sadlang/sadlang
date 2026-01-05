/*
 * ============================================================================
 * LLVM Compiler Pipeline - التنفيذ
 * LLVM Compiler Pipeline Implementation
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): January 4, 2026
 * ============================================================================
 */

#include "../include/llvm_compiler_pipeline.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace Sad {
namespace Compiler {
namespace LLVM {

// ============================================================================
// CompilationStats Implementation - تنفيذ الإحصائيات
// ============================================================================

void CompilationStats::print() const {
    std::cout << "\n========================================\n";
    std::cout << "إحصائيات الترجمة / Compilation Statistics\n";
    std::cout << "========================================\n\n";
    
    std::cout << "الأوقات / Timings:\n";
    std::cout << "  المحلل المعجمي / Lexer:        " << std::fixed << std::setprecision(2) << lexerTime << " ms\n";
    std::cout << "  المحلل النحوي / Parser:         " << parserTime << " ms\n";
    std::cout << "  فحص الأنواع / Type Checking:    " << typeCheckTime << " ms\n";
    std::cout << "  بناء SIR / SIR Building:        " << sirBuildTime << " ms\n";
    std::cout << "  توليد الكود / Code Generation:  " << codeGenTime << " ms\n";
    std::cout << "  التحسين / Optimization:         " << optimizationTime << " ms\n";
    std::cout << "  الإصدار / Emission:             " << emitTime << " ms\n";
    std::cout << "  ─────────────────────────────────\n";
    std::cout << "  المجموع / Total:                " << totalTime << " ms\n\n";
    
    std::cout << "الإحصائيات / Statistics:\n";
    std::cout << "  الرموز / Tokens:                " << tokenCount << "\n";
    std::cout << "  عقد AST / AST Nodes:            " << astNodeCount << "\n";
    std::cout << "  تعليمات SIR / SIR Instr.:      " << sirInstructionCount << "\n";
    std::cout << "  حجم LLVM IR / LLVM IR Size:     " << llvmIRSize << " bytes\n";
    std::cout << "  حجم Object / Object Size:       " << objectFileSize << " bytes\n";
    std::cout << "  حجم Executable / Exe Size:      " << executableSize << " bytes\n\n";
    
    std::cout << "المكونات / Components:\n";
    std::cout << "  الدوال / Functions:             " << functionsCount << "\n";
    std::cout << "  الأصناف / Classes:              " << classesCount << "\n";
    std::cout << "  متغيرات عامة / Globals:        " << globalVarsCount << "\n\n";
    
    if (totalTime > 0) {
        double ips = getInstructionsPerSecond();
        std::cout << "الأداء / Performance:\n";
        std::cout << "  السرعة / Speed:                 " << std::fixed << std::setprecision(2) 
                  << (ips / 1000.0) << " K instr/s\n";
    }
    
    std::cout << "========================================\n\n";
}

// ============================================================================
// CompilationResult Implementation - تنفيذ النتيجة
// ============================================================================

void CompilationResult::print() const {
    std::cout << "\n========================================\n";
    std::cout << "نتيجة الترجمة / Compilation Result\n";
    std::cout << "========================================\n\n";
    
    if (success) {
        std::cout << "الحالة / Status: ✅ نجحت / Success\n\n";
        
        if (!outputFile.empty()) {
            std::cout << "ملف الإخراج / Output File: " << outputFile << "\n";
        }
        if (!llvmIRFile.empty()) {
            std::cout << "ملف LLVM IR / LLVM IR File: " << llvmIRFile << "\n";
        }
        if (!assemblyFile.empty()) {
            std::cout << "ملف Assembly / Assembly File: " << assemblyFile << "\n";
        }
        if (!objectFile.empty()) {
            std::cout << "ملف Object / Object File: " << objectFile << "\n";
        }
        
        std::cout << "\n";
        stats.print();
        
        if (!warnings.empty()) {
            std::cout << "التحذيرات / Warnings (" << warnings.size() << "):\n";
            for (const auto& warning : warnings) {
                std::cout << "  ⚠️  " << warning << "\n";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "الحالة / Status: ❌ فشلت / Failed\n\n";
        std::cout << "الخطأ / Error: " << errorMessage << "\n\n";
        
        if (!errors.empty()) {
            std::cout << "الأخطاء / Errors (" << errors.size() << "):\n";
            for (const auto& error : errors) {
                std::cout << "  ❌ " << error << "\n";
            }
            std::cout << "\n";
        }
    }
    
    std::cout << "========================================\n\n";
}

// ============================================================================
// LLVMCompilerPipeline Implementation - تنفيذ Pipeline الرئيسي
// ============================================================================

/**
 * المنشئ / Constructor
 */
LLVMCompilerPipeline::LLVMCompilerPipeline(const Sad::Compiler::CompilerOptions& options)
    : options_(options)
    , initialized_(false)
    , hasErrors_(false)
{
}

/**
 * المنشئ الافتراضي / Default constructor
 */
LLVMCompilerPipeline::LLVMCompilerPipeline()
    : initialized_(false)
    , hasErrors_(false)
{
    // استخدام خيارات افتراضية / Use default options
}

/**
 * المدمر / Destructor
 */
LLVMCompilerPipeline::~LLVMCompilerPipeline() {
    cleanup();
}

/**
 * تهيئة Pipeline / Initialize pipeline
 */
bool LLVMCompilerPipeline::initialize() {
    if (initialized_) {
        return true; // Already initialized / مُهيأ مسبقاً
    }
    
    try {
        // تهيئة Code Generator مع خيارات / Initialize code generator with options
        codeGen_ = std::make_unique<Sad::LLVM::LLVMCodeGen>();
        
        // تهيئة LLVM مع الهدف الافتراضي / Initialize LLVM with default target
        std::string moduleName = "sad_module";
        if (!codeGen_->initialize(moduleName, options_.target_triple)) {
            logError("فشل تهيئة LLVM Code Generator / Failed to initialize LLVM Code Generator");
            return false;
        }
        
        // تعيين مستوى التحسين / Set optimization level
        codeGen_->setOptimizationLevel(options_.optimization_level);
        codeGen_->setAutoOptimize(options_.auto_optimize);
        
        if (options_.verbose) {
            std::cout << "[Pipeline] تمت التهيئة بنجاح / Initialized successfully\n";
            std::cout << "[Pipeline] الهدف / Target: " << options_.target_triple << "\n";
            std::cout << "[Pipeline] مستوى التحسين / Optimization: ";
            switch (options_.optimization_level) {
                case sad::OptimizationLevel::O0: std::cout << "O0 (No optimization)\n"; break;
                case sad::OptimizationLevel::O1: std::cout << "O1 (Basic)\n"; break;
                case sad::OptimizationLevel::O2: std::cout << "O2 (Standard)\n"; break;
                case sad::OptimizationLevel::O3: std::cout << "O3 (Aggressive)\n"; break;
                case sad::OptimizationLevel::Os: std::cout << "Os (Size)\n"; break;
                case sad::OptimizationLevel::Oz: std::cout << "Oz (Minimal size)\n"; break;
            }
        }
        
        initialized_ = true;
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في التهيئة / Initialization error: ") + e.what());
        return false;
    }
}

/**
 * ترجمة ملف / Compile file
 */
CompilationResult LLVMCompilerPipeline::compileFile(const std::string& sourceFile) {
    if (options_.verbose) {
        std::cout << "[Pipeline] قراءة الملف / Reading file: " << sourceFile << "\n";
    }
    
    // قراءة الملف / Read file
    std::string source = readFile(sourceFile);
    if (source.empty()) {
        CompilationResult result;
        result.success = false;
        result.errorMessage = "فشل قراءة الملف / Failed to read file: " + sourceFile;
        return result;
    }
    
    // ترجمة الكود / Compile source
    return compileSource(source, sourceFile);
}

/**
 * ترجمة كود مباشر / Compile source code
 */
CompilationResult LLVMCompilerPipeline::compileSource(const std::string& sourceCode, const std::string& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilationResult result;
    result.success = false;
    hasErrors_ = false;
    
    // التأكد من التهيئة / Ensure initialization
    if (!initialized_ && !initialize()) {
        result.errorMessage = "فشل تهيئة Pipeline / Failed to initialize pipeline";
        return result;
    }
    
    if (options_.verbose) {
        std::cout << "[Pipeline] بدء الترجمة / Starting compilation...\n";
    }
    
    // المرحلة 1: التحليل المعجمي / Stage 1: Lexical analysis
    auto stageStart = std::chrono::high_resolution_clock::now();
    if (!lexicalAnalysis(sourceCode, filename)) {
        result.errorMessage = "فشل التحليل المعجمي / Lexical analysis failed";
        result.errors = errors_;
        return result;
    }
    result.stats.lexerTime = measureTime(stageStart);
    result.stats.tokenCount = tokens_.size();
    
    // المرحلة 2: التحليل النحوي / Stage 2: Parsing
    stageStart = std::chrono::high_resolution_clock::now();
    if (!parsing()) {
        result.errorMessage = "فشل التحليل النحوي / Parsing failed";
        result.errors = errors_;
        return result;
    }
    result.stats.parserTime = measureTime(stageStart);
    result.stats.astNodeCount = ast_.size(); // تقريبي / Approximate
    
    // المرحلة 3: فحص الأنواع (اختياري) / Stage 3: Type checking (optional)
    if (options_.type_check_at_compile_time) {
        stageStart = std::chrono::high_resolution_clock::now();
        if (!typeChecking()) {
            result.errorMessage = "فشل فحص الأنواع / Type checking failed";
            result.errors = errors_;
            return result;
        }
        result.stats.typeCheckTime = measureTime(stageStart);
    }
    
    // المرحلة 4: بناء SIR / Stage 4: SIR building
    stageStart = std::chrono::high_resolution_clock::now();
    if (!sirBuilding()) {
        result.errorMessage = "فشل بناء SIR / SIR building failed";
        result.errors = errors_;
        return result;
    }
    result.stats.sirBuildTime = measureTime(stageStart);
    if (sirModule_) {
        result.stats.sirInstructionCount = sirModule_->getTotalInstructions();
        result.stats.functionsCount = sirModule_->getFunctionCount();
        result.stats.globalVarsCount = sirModule_->getGlobalCount();
    }
    
    // المرحلة 5: توليد LLVM IR / Stage 5: Code generation
    stageStart = std::chrono::high_resolution_clock::now();
    if (!codeGeneration()) {
        result.errorMessage = "فشل توليد الكود / Code generation failed";
        result.errors = errors_;
        return result;
    }
    result.stats.codeGenTime = measureTime(stageStart);
    
    // المرحلة 6: التحسين (إذا كان مفعلاً) / Stage 6: Optimization (if enabled)
    if (options_.isOptimizationEnabled()) {
        stageStart = std::chrono::high_resolution_clock::now();
        if (!optimization()) {
            // التحسين فشل لكن نواصل / Optimization failed but continue
            logWarning("فشل التحسين، الاستمرار بدون تحسين / Optimization failed, continuing without optimization");
        }
        result.stats.optimizationTime = measureTime(stageStart);
    }
    
    // حساب الوقت الإجمالي / Calculate total time
    result.stats.totalTime = measureTime(startTime);
    
    // حفظ الإخراج إذا طُلب / Save output if requested
    if (options_.emit_llvm_ir) {
        std::string irFile = options_.getLLVMIRFilename();
        if (emitLLVMIR(irFile)) {
            result.llvmIRFile = irFile;
        }
    }
    
    if (options_.emit_assembly) {
        std::string asmFile = options_.getAssemblyFilename();
        if (emitAssembly(asmFile)) {
            result.assemblyFile = asmFile;
        }
    }
    
    if (options_.emit_object) {
        std::string objFile = options_.getObjectFilename();
        if (emitObjectFile(objFile)) {
            result.objectFile = objFile;
            result.outputFile = objFile;
        }
    }
    
    // النجاح! / Success!
    result.success = true;
    result.warnings = warnings_;
    lastResult_ = result;
    
    if (options_.verbose || options_.print_compilation_stats) {
        result.print();
    }
    
    return result;
}

/**
 * المرحلة 1: التحليل المعجمي / Stage 1: Lexical analysis
 */
bool LLVMCompilerPipeline::lexicalAnalysis(const std::string& source, const std::string& filename) {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] المرحلة 1: التحليل المعجمي / Stage 1: Lexical analysis\n";
        }
        
        // إنشاء Lexer / Create lexer
        // Source: include/lexer/lexer_core.h:44 - LexerCore(const std::string& source)
        lexer_ = std::make_unique<Lexer::LexerCore>(source);
        
        // ملاحظة: لا نستدعي tokenize() هنا لأن Parser يحتاج Lexer
        // جديداً ليستخرج الرموز تدريجياً
        // Note: We don't call tokenize() here because Parser needs
        // fresh Lexer to extract tokens incrementally
        
        // لأغراض الإحصاء فقط، نحسب الرموز
        // For statistics only, count tokens
        Lexer::LexerCore tempLexer(source);
        tokens_ = tempLexer.tokenize();
        
        if (options_.verbose) {
            std::cout << "[Pipeline] تم استخراج " << tokens_.size() << " رمز / Extracted " << tokens_.size() << " tokens\n";
        }
        
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في التحليل المعجمي / Lexical analysis error: ") + e.what());
        return false;
    }
}

/**
 * المرحلة 2: التحليل النحوي / Stage 2: Parsing
 */
bool LLVMCompilerPipeline::parsing() {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] المرحلة 2: التحليل النحوي / Stage 2: Parsing\n";
        }
        
        // إنشاء Parser / Create parser
        parser_ = std::make_unique<Parser::ParserCore>(*lexer_);
        
        // تحليل البرنامج / Parse program
        ast_ = parser_->parseProgram();
        
        // التحقق من الأخطاء / Check for errors
        if (parser_->hasErrors()) {
            auto parserErrors = parser_->getErrors();
            for (const auto& error : parserErrors) {
                logError(error);
            }
            return false;
        }
        
        if (options_.verbose) {
            std::cout << "[Pipeline] تم بناء AST بنجاح / AST built successfully\n";
        }
        
        // طباعة AST إذا طُلب / Print AST if requested
        if (options_.print_ast) {
            std::cout << "\n=== AST ===\n";
            // TODO: طباعة AST / Print AST
            std::cout << "عدد العبارات / Statement count: " << ast_.size() << "\n\n";
        }
        
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في التحليل النحوي / Parsing error: ") + e.what());
        return false;
    }
}

/**
 * المرحلة 3: فحص الأنواع / Stage 3: Type checking (optional)
 */
bool LLVMCompilerPipeline::typeChecking() {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] المرحلة 3: فحص الأنواع / Stage 3: Type checking\n";
        }
        
        // TODO: تنفيذ فحص الأنواع / Implement type checking
        // هذه مرحلة اختيارية ويمكن تخطيها حالياً / This is optional and can be skipped for now
        
        if (options_.verbose) {
            std::cout << "[Pipeline] تم تخطي فحص الأنواع (قيد التطوير) / Type checking skipped (under development)\n";
        }
        
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في فحص الأنواع / Type checking error: ") + e.what());
        return false;
    }
}

/**
 * المرحلة 4: بناء SIR / Stage 4: SIR building
 */
bool LLVMCompilerPipeline::sirBuilding() {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] المرحلة 4: بناء SIR / Stage 4: SIR building\n";
        }
        
        // إنشاء SIR Builder / Create SIR builder
        sirBuilder_ = std::make_unique<SIR::SIRBuilder>();
        
        // بناء SIR Module من AST / Build SIR module from AST
        // Note: buildModule expects ProgramNode (StmtList)
        sirModule_ = sirBuilder_->buildModule(&ast_);
        
        if (!sirModule_) {
            logError("فشل بناء SIR Module / Failed to build SIR module");
            return false;
        }
        
        if (options_.verbose) {
            std::cout << "[Pipeline] تم بناء SIR بنجاح / SIR built successfully\n";
            std::cout << "[Pipeline]   الدوال / Functions: " << sirModule_->getFunctionCount() << "\n";
            std::cout << "[Pipeline]   التعليمات / Instructions: " << sirModule_->getTotalInstructions() << "\n";
        }
        
        // طباعة SIR إذا طُلب / Print SIR if requested
        if (options_.print_sir) {
            std::cout << "\n=== SIR ===\n";
            sirModule_->print();
            std::cout << "\n";
        }
        
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في بناء SIR / SIR building error: ") + e.what());
        return false;
    }
}

/**
 * المرحلة 5: توليد LLVM IR / Stage 5: Code generation
 */
bool LLVMCompilerPipeline::codeGeneration() {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] المرحلة 5: توليد LLVM IR / Stage 5: Code generation\n";
        }
        
        // طباعة IR قبل التحسين إذا طُلب / Print IR before optimization if requested
        bool printBefore = options_.print_ir_before_opt;
        
        // توليد LLVM Module من SIR / Generate LLVM module from SIR
        // ملاحظة: generate() تتحقق من الـ module داخلياً قبل إرجاعه
        // Note: generate() verifies the module internally before returning
        llvmModule_ = codeGen_->generate(sirModule_);
        
        if (!llvmModule_) {
            logError("فشل توليد LLVM Module / Failed to generate LLVM module");
            return false;
        }
        
        // لا نستدعي verify() هنا لأن:
        // 1. generate() تتحقق داخلياً
        // 2. بعد generate()، module_ في codeGen_ يصبح null بسبب std::move
        // We don't call verify() here because:
        // 1. generate() verifies internally
        // 2. After generate(), module_ in codeGen_ becomes null due to std::move
        
        if (options_.verbose) {
            std::cout << "[Pipeline] تم توليد LLVM IR بنجاح / LLVM IR generated successfully\n";
        }
        
        // طباعة IR قبل التحسين / Print IR before optimization
        if (printBefore) {
            std::cout << "\n=== LLVM IR (قبل التحسين / Before Optimization) ===\n";
            llvmModule_->print(llvm::outs(), nullptr);
            std::cout << "\n";
        }
        
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في توليد الكود / Code generation error: ") + e.what());
        return false;
    }
}

/**
 * المرحلة 6: التحسين / Stage 6: Optimization
 */
bool LLVMCompilerPipeline::optimization() {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] المرحلة 6: التحسين / Stage 6: Optimization\n";
        }
        
        // التحسين يتم تلقائياً في codeGen->generate() إذا كان auto_optimize مفعلاً
        // Optimization happens automatically in codeGen->generate() if auto_optimize is enabled
        
        // طباعة IR بعد التحسين إذا طُلب / Print IR after optimization if requested
        if (options_.print_ir_after_opt) {
            std::cout << "\n=== LLVM IR (بعد التحسين / After Optimization) ===\n";
            llvmModule_->print(llvm::outs(), nullptr);
            std::cout << "\n";
        }
        
        // طباعة إحصائيات التحسين إذا طُلب / Print optimization stats if requested
        if (options_.print_optimization_stats) {
            // TODO: الحصول على إحصائيات التحسين من codeGen
            // Get optimization stats from codeGen
            if (options_.verbose) {
                std::cout << "[Pipeline] إحصائيات التحسين متاحة / Optimization stats available\n";
            }
        }
        
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في التحسين / Optimization error: ") + e.what());
        return false;
    }
}

/**
 * إصدار LLVM IR إلى ملف / Emit LLVM IR to file
 */
bool LLVMCompilerPipeline::emitLLVMIR(const std::string& filename) {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] إصدار LLVM IR إلى / Emitting LLVM IR to: " << filename << "\n";
        }
        
        if (!llvmModule_) {
            logError("لا توجد وحدة LLVM / No LLVM module available");
            return false;
        }
        
        return codeGen_->emitToFile(filename);
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في إصدار LLVM IR / LLVM IR emission error: ") + e.what());
        return false;
    }
}

/**
 * إصدار Assembly / Emit assembly
 */
bool LLVMCompilerPipeline::emitAssembly(const std::string& filename) {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] إصدار Assembly إلى / Emitting assembly to: " << filename << "\n";
        }
        
        if (!llvmModule_) {
            logError("لا توجد وحدة LLVM / No LLVM module available");
            return false;
        }
        
        // استخدام الوحدة المحلية llvmModule_ بدلاً من codeGen_->module_
        // Use local llvmModule_ instead of codeGen_->module_
        return codeGen_->emitAssembly(filename, llvmModule_.get());
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في إصدار Assembly / Assembly emission error: ") + e.what());
        return false;
    }
}

/**
 * إصدار Object file / Emit object file
 */
bool LLVMCompilerPipeline::emitObjectFile(const std::string& filename) {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] إصدار Object file إلى / Emitting object file to: " << filename << "\n";
        }
        
        if (!llvmModule_) {
            logError("لا توجد وحدة LLVM / No LLVM module available");
            return false;
        }
        
        // استخدام الوحدة المحلية llvmModule_ بدلاً من codeGen_->module_ 
        // لأن generate() نقلت الملكية بـ std::move
        // Use local llvmModule_ instead of codeGen_->module_
        // because generate() moved ownership with std::move
        return codeGen_->emitObjectFile(filename, llvmModule_.get());
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في إصدار Object file / Object file emission error: ") + e.what());
        return false;
    }
}

/**
 * إصدار Executable / Emit executable
 */
bool LLVMCompilerPipeline::emitExecutable(const std::string& filename) {
    try {
        if (options_.verbose) {
            std::cout << "[Pipeline] إنشاء Executable: " << filename << "\n";
        }
        
        // أولاً: إنشاء object file / First: Create object file
        std::string objFile = filename + ".o";
        if (!emitObjectFile(objFile)) {
            return false;
        }
        
        // ثانياً: استدعاء Linker / Second: Invoke linker
        // TODO: تنفيذ Linker / Implement linker
        if (options_.verbose) {
            std::cout << "[Pipeline] Linker قيد التطوير / Linker under development\n";
            std::cout << "[Pipeline] Object file جاهز / Object file ready: " << objFile << "\n";
        }
        
        return true;
    }
    catch (const std::exception& e) {
        logError(std::string("خطأ في إنشاء Executable / Executable creation error: ") + e.what());
        return false;
    }
}

/**
 * الحصول على الوحدة الحالية / Get current module
 */
llvm::Module* LLVMCompilerPipeline::getCurrentModule() {
    return llvmModule_.get();
}

// ============================================================================
// Helper Functions - الدوال المساعدة
// ============================================================================

/**
 * قراءة ملف / Read file
 */
std::string LLVMCompilerPipeline::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        logError("فشل فتح الملف / Failed to open file: " + filename);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * تنظيف الموارد / Cleanup resources
 */
void LLVMCompilerPipeline::cleanup() {
    tokens_.clear();
    ast_.clear();
    sirModule_.reset();
    llvmModule_.reset();
    errors_.clear();
    warnings_.clear();
}

/**
 * قياس الوقت / Measure time
 */
double LLVMCompilerPipeline::measureTime(const std::chrono::high_resolution_clock::time_point& start) const {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1000.0; // تحويل إلى ميلي ثانية / Convert to milliseconds
}

/**
 * تسجيل خطأ / Log error
 */
void LLVMCompilerPipeline::logError(const std::string& message) {
    errors_.push_back(message);
    hasErrors_ = true;
    
    if (options_.verbose) {
        std::cerr << "[Pipeline Error] " << message << "\n";
    }
}

/**
 * تسجيل تحذير / Log warning
 */
void LLVMCompilerPipeline::logWarning(const std::string& message) {
    warnings_.push_back(message);
    
    if (options_.verbose) {
        std::cout << "[Pipeline Warning] " << message << "\n";
    }
}

} // namespace LLVM
} // namespace Compiler
} // namespace Sad
