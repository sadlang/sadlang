/**
 * @file compiler_pipeline.h
 * @brief خط أنابيب المترجم الكامل / Complete Compiler Pipeline
 * @brief End-to-end compilation from source to bytecode execution
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 * 
 * @details
 * (AR) خط أنابيب كامل:
 *      المصدر → Lexer → Parser → AST → Optimizer → Bytecode Emitter → VM
 *      مع دعم حفظ/تحميل ملفات .sbc
 * 
 * (EN) Complete pipeline:
 *      Source → Lexer → Parser → AST → Optimizer → Bytecode Emitter → VM
 *      With .sbc file save/load support
 */

#pragma once

#include "../lexer/include/lexer.h"
#include "../parser/include/parser.h"
#include "../ast/include/ast.h"
#include "../optimizer/include/optimizer.h"
#include "../bytecode/include/bytecode_emitter.h"
#include "../bytecode/include/bytecode_file.h"
#include "../vm/include/vm.h"
#include "../vm/include/vm_gc.h"
#include "../runtime/include/runtime.h"

#include <string>
#include <memory>
#include <chrono>
#include <vector>

namespace Sad {

// ========================================
// Compilation Options - خيارات الترجمة
// ========================================

/**
 * @brief خيارات الترجمة / Compilation options
 */
struct CompilationOptions {
    // (AR) مستوى التحسين / (EN) Optimization level
    int optimizationLevel = 2;  // 0=none, 1=basic, 2=aggressive
    
    // (AR) معلومات التصحيح / (EN) Debug information
    bool includeDebugInfo = true;
    
    // (AR) التحقق من الأخطاء / (EN) Error checking
    bool strictMode = true;
    
    // (AR) تفعيل التحذيرات / (EN) Enable warnings
    bool showWarnings = true;
    
    // (AR) إحصائيات مفصلة / (EN) Verbose statistics
    bool verbose = false;
    
    // (AR) حفظ ملف .sbc / (EN) Save .sbc file
    bool saveBytecode = false;
    std::string bytecodeOutput = "";
    
    // (AR) إعدادات VM / (EN) VM settings
    VM::VMConfig vmConfig;
    
    // (AR) إعدادات GC / (EN) GC settings
    VM::GCConfig gcConfig;
};

// ========================================
// Compilation Result - نتيجة الترجمة
// ========================================

/**
 * @brief نتيجة الترجمة / Compilation result
 */
struct CompilationResult {
    bool success = false;
    std::string errorMessage;
    
    // (AR) الوقت المستغرق / (EN) Timing information
    double lexerTime = 0.0;
    double parserTime = 0.0;
    double optimizerTime = 0.0;
    double emitterTime = 0.0;
    double totalTime = 0.0;
    
    // (AR) الإحصائيات / (EN) Statistics
    size_t tokenCount = 0;
    size_t astNodeCount = 0;
    size_t bytecodeSize = 0;
    size_t constantsCount = 0;
    
    /**
     * @brief طباعة النتيجة / Print result
     */
    void print() const;
};

// ========================================
// Execution Result - نتيجة التنفيذ
// ========================================

/**
 * @brief نتيجة التنفيذ / Execution result
 */
struct ExecutionResult {
    bool success = false;
    std::string errorMessage;
    
    VM::Value returnValue;
    
    // (AR) الوقت المستغرق / (EN) Execution time
    double executionTime = 0.0;
    
    // (AR) إحصائيات VM / (EN) VM statistics
    size_t instructionsExecuted = 0;
    size_t gcCollections = 0;
    size_t peakMemoryUsage = 0;
    
    /**
     * @brief طباعة النتيجة / Print result
     */
    void print() const;
};

// ========================================
// Compiler Pipeline - خط أنابيب المترجم
// ========================================

/**
 * @brief خط أنابيب المترجم الكامل / Complete compiler pipeline
 */
class CompilerPipeline {
public:
    /**
     * @brief المُنشئ / Constructor
     * @param options خيارات الترجمة / Compilation options
     */
    explicit CompilerPipeline(const CompilationOptions& options = CompilationOptions{});
    
    /**
     * @brief المُدمِّر / Destructor
     */
    ~CompilerPipeline();
    
    // ========================================
    // Compilation Methods
    // ========================================
    
    /**
     * @brief ترجمة من نص مصدري / Compile from source text
     * @param source النص المصدري / Source code
     * @param filename اسم الملف (اختياري) / File name (optional)
     * @return نتيجة الترجمة / Compilation result
     */
    CompilationResult compile(const std::string& source, const std::string& filename = "<source>");
    
    /**
     * @brief ترجمة من ملف / Compile from file
     * @param filename اسم الملف / File name
     * @return نتيجة الترجمة / Compilation result
     */
    CompilationResult compileFile(const std::string& filename);
    
    /**
     * @brief تنفيذ البايت كود المترجم / Execute compiled bytecode
     * @return نتيجة التنفيذ / Execution result
     */
    ExecutionResult execute();
    
    /**
     * @brief ترجمة وتنفيذ / Compile and execute
     * @param source النص المصدري / Source code
     * @param filename اسم الملف (اختياري) / File name (optional)
     * @return نتيجة التنفيذ / Execution result
     */
    ExecutionResult compileAndExecute(const std::string& source, 
                                     const std::string& filename = "<source>");
    
    // ========================================
    // Bytecode File Operations
    // ========================================
    
    /**
     * @brief حفظ البايت كود إلى ملف / Save bytecode to file
     * @param filename اسم الملف / File name
     * @return true إذا نجحت / true if successful
     */
    bool saveBytecode(const std::string& filename);
    
    /**
     * @brief تحميل وتنفيذ ملف بايت كود / Load and execute bytecode file
     * @param filename اسم الملف / File name
     * @return نتيجة التنفيذ / Execution result
     */
    ExecutionResult loadAndExecute(const std::string& filename);
    
    // ========================================
    // Access Methods
    // ========================================
    
    /**
     * @brief الحصول على AST / Get AST
     * @return مؤشر للـ AST / Pointer to AST
     */
    AST::Program* getAST() const { return ast_.get(); }
    
    /**
     * @brief الحصول على البايت كود / Get bytecode
     * @return البايت كود / Bytecode
     */
    const std::vector<uint8_t>& getBytecode() const { return bytecode_; }
    
    /**
     * @brief الحصول على الثوابت / Get constants
     * @return الثوابت / Constants
     */
    const std::vector<VM::Value>& getConstants() const { return constants_; }
    
    /**
     * @brief الحصول على VM / Get VM
     * @return مؤشر للـ VM / Pointer to VM
     */
    VM::VirtualMachine* getVM() const { return vm_.get(); }
    
    /**
     * @brief الحصول على GC / Get GC
     * @return مؤشر للـ GC / Pointer to GC
     */
    VM::GarbageCollector* getGC() const { return gc_.get(); }
    
    /**
     * @brief الحصول على الخيارات / Get options
     * @return الخيارات / Options
     */
    const CompilationOptions& getOptions() const { return options_; }
    
    /**
     * @brief تحديث الخيارات / Update options
     * @param options الخيارات الجديدة / New options
     */
    void setOptions(const CompilationOptions& options);
    
    // ========================================
    // Statistics & Diagnostics
    // ========================================
    
    /**
     * @brief طباعة إحصائيات الترجمة / Print compilation statistics
     */
    void printCompilationStats() const;
    
    /**
     * @brief طباعة إحصائيات التنفيذ / Print execution statistics
     */
    void printExecutionStats() const;
    
    /**
     * @brief طباعة إحصائيات GC / Print GC statistics
     */
    void printGCStats() const;
    
    /**
     * @brief تفريغ البايت كود / Dump bytecode
     */
    void dumpBytecode() const;
    
    /**
     * @brief تفريغ AST / Dump AST
     */
    void dumpAST() const;
    
private:
    // ========================================
    // Internal Methods
    // ========================================
    
    /**
     * @brief تهيئة المكونات / Initialize components
     */
    void initialize();
    
    /**
     * @brief تنظيف الموارد / Cleanup resources
     */
    void cleanup();
    
    /**
     * @brief المرحلة 1: التحليل اللفظي / Phase 1: Lexical analysis
     */
    bool lexicalAnalysis(const std::string& source, const std::string& filename);
    
    /**
     * @brief المرحلة 2: التحليل النحوي / Phase 2: Parsing
     */
    bool parsing();
    
    /**
     * @brief المرحلة 3: التحسين / Phase 3: Optimization
     */
    bool optimization();
    
    /**
     * @brief المرحلة 4: توليد البايت كود / Phase 4: Bytecode generation
     */
    bool codeGeneration();
    
    /**
     * @brief تسجيل خطأ / Log error
     */
    void logError(const std::string& phase, const std::string& message);
    
    // ========================================
    // Member Variables
    // ========================================
    
    CompilationOptions options_;
    
    // (AR) المكونات / (EN) Components
    std::unique_ptr<Lexer::Lexer> lexer_;
    std::unique_ptr<Parser::Parser> parser_;
    std::unique_ptr<Optimizer::Optimizer> optimizer_;
    std::unique_ptr<Bytecode::BytecodeEmitter> emitter_;
    std::unique_ptr<VM::VirtualMachine> vm_;
    std::unique_ptr<VM::GarbageCollector> gc_;
    
    // (AR) البيانات / (EN) Data
    std::vector<Lexer::Token> tokens_;
    std::unique_ptr<AST::Program> ast_;
    std::vector<uint8_t> bytecode_;
    std::vector<VM::Value> constants_;
    
    // (AR) النتائج / (EN) Results
    CompilationResult lastCompilation_;
    ExecutionResult lastExecution_;
    
    // (AR) الأخطاء / (EN) Errors
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

// ========================================
// Utility Functions - دوال مساعدة
// ========================================

/**
 * @brief قراءة ملف نصي / Read text file
 * @param filename اسم الملف / File name
 * @return محتوى الملف / File contents
 */
std::string readFile(const std::string& filename);

/**
 * @brief قياس الوقت / Measure time
 * @param start وقت البداية / Start time
 * @return الوقت بالثواني / Time in seconds
 */
double measureTime(std::chrono::high_resolution_clock::time_point start);

/**
 * @brief تنسيق حجم الذاكرة / Format memory size
 * @param bytes الحجم بالبايت / Size in bytes
 * @return نص منسّق / Formatted string
 */
std::string formatMemorySize(size_t bytes);

/**
 * @brief تنسيق الوقت / Format time
 * @param seconds الوقت بالثواني / Time in seconds
 * @return نص منسّق / Formatted string
 */
std::string formatTime(double seconds);

} // namespace Sad
