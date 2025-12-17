/**
 * @file compiler_pipeline.cpp
 * @brief تطبيق خط أنابيب المترجم / Compiler Pipeline Implementation
 * @brief Implementation of end-to-end compilation and execution
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/compiler_pipeline.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace Sad {

// ========================================
// CompilationResult Implementation
// ========================================

void CompilationResult::print() const {
    std::cout << "\n========================================\n";
    std::cout << "نتيجة الترجمة / Compilation Result\n";
    std::cout << "========================================\n";
    
    if (success) {
        std::cout << "الحالة / Status: ✅ نجحت / Success\n\n";
        
        std::cout << "الوقت / Timing:\n";
        std::cout << "  المحلل اللفظي / Lexer:      " << formatTime(lexerTime) << "\n";
        std::cout << "  المحلل النحوي / Parser:     " << formatTime(parserTime) << "\n";
        std::cout << "  المُحسِّن / Optimizer:       " << formatTime(optimizerTime) << "\n";
        std::cout << "  مولد الكود / Emitter:       " << formatTime(emitterTime) << "\n";
        std::cout << "  المجموع / Total:            " << formatTime(totalTime) << "\n\n";
        
        std::cout << "الإحصائيات / Statistics:\n";
        std::cout << "  الرموز / Tokens:            " << tokenCount << "\n";
        std::cout << "  عقد AST / AST Nodes:        " << astNodeCount << "\n";
        std::cout << "  حجم البايت كود / Bytecode:  " << formatMemorySize(bytecodeSize) << "\n";
        std::cout << "  الثوابت / Constants:        " << constantsCount << "\n";
    } else {
        std::cout << "الحالة / Status: ❌ فشلت / Failed\n";
        std::cout << "الخطأ / Error: " << errorMessage << "\n";
    }
    
    std::cout << "========================================\n\n";
}

// ========================================
// ExecutionResult Implementation
// ========================================

void ExecutionResult::print() const {
    std::cout << "\n========================================\n";
    std::cout << "نتيجة التنفيذ / Execution Result\n";
    std::cout << "========================================\n";
    
    if (success) {
        std::cout << "الحالة / Status: ✅ نجحت / Success\n\n";
        
        std::cout << "القيمة المُرجَعة / Return Value: ";
        VM::printValue(returnValue);
        std::cout << "\n\n";
        
        std::cout << "الأداء / Performance:\n";
        std::cout << "  وقت التنفيذ / Execution Time: " << formatTime(executionTime) << "\n";
        std::cout << "  التعليمات المُنفَّذة / Instructions: " << instructionsExecuted << "\n";
        std::cout << "  دورات GC / GC Collections:   " << gcCollections << "\n";
        std::cout << "  ذروة الذاكرة / Peak Memory:  " << formatMemorySize(peakMemoryUsage) << "\n";
        
        // (AR) حساب سرعة التنفيذ / (EN) Calculate execution speed
        if (executionTime > 0) {
            double ips = instructionsExecuted / executionTime;
            std::cout << "  السرعة / Speed:              " << std::fixed << std::setprecision(2) 
                     << (ips / 1000000.0) << " MIPS\n";
        }
    } else {
        std::cout << "الحالة / Status: ❌ فشلت / Failed\n";
        std::cout << "الخطأ / Error: " << errorMessage << "\n";
    }
    
    std::cout << "========================================\n\n";
}

// ========================================
// CompilerPipeline Implementation
// ========================================

CompilerPipeline::CompilerPipeline(const CompilationOptions& options)
    : options_(options)
{
    initialize();
}

CompilerPipeline::~CompilerPipeline() {
    cleanup();
}

void CompilerPipeline::initialize() {
    // (AR) أنشئ المكونات / (EN) Create components
    lexer_ = std::make_unique<Lexer::Lexer>();
    parser_ = std::make_unique<Parser::Parser>();
    optimizer_ = std::make_unique<Optimizer::Optimizer>();
    emitter_ = std::make_unique<Bytecode::BytecodeEmitter>();
    
    // (AR) أنشئ VM و GC / (EN) Create VM and GC
    gc_ = std::make_unique<VM::GarbageCollector>(nullptr, options_.gcConfig);
    vm_ = std::make_unique<VM::VirtualMachine>(options_.vmConfig);
    
    // (AR) اربط GC مع VM / (EN) Connect GC with VM
    gc_ = std::make_unique<VM::GarbageCollector>(vm_.get(), options_.gcConfig);
    
    // (AR) سجّل دوال Runtime / (EN) Register runtime functions
    Runtime::registerAllRuntimeFunctions(vm_.get());
}

void CompilerPipeline::cleanup() {
    tokens_.clear();
    ast_.reset();
    bytecode_.clear();
    constants_.clear();
    errors_.clear();
    warnings_.clear();
}

CompilationResult CompilerPipeline::compile(const std::string& source, 
                                           const std::string& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilationResult result;
    result.success = false;
    
    if (options_.verbose) {
        std::cout << "[المترجم / Compiler] بدء الترجمة / Starting compilation...\n";
    }
    
    // (AR) المرحلة 1: التحليل اللفظي / (EN) Phase 1: Lexical analysis
    if (!lexicalAnalysis(source, filename)) {
        result.errorMessage = "فشل التحليل اللفظي / Lexical analysis failed";
        return result;
    }
    result.lexerTime = measureTime(startTime);
    result.tokenCount = tokens_.size();
    
    // (AR) المرحلة 2: التحليل النحوي / (EN) Phase 2: Parsing
    auto parseStart = std::chrono::high_resolution_clock::now();
    if (!parsing()) {
        result.errorMessage = "فشل التحليل النحوي / Parsing failed";
        return result;
    }
    result.parserTime = measureTime(parseStart);
    result.astNodeCount = ast_ ? ast_->countNodes() : 0;
    
    // (AR) المرحلة 3: التحسين / (EN) Phase 3: Optimization
    auto optimizeStart = std::chrono::high_resolution_clock::now();
    if (options_.optimizationLevel > 0 && !optimization()) {
        result.errorMessage = "فشل التحسين / Optimization failed";
        return result;
    }
    result.optimizerTime = measureTime(optimizeStart);
    
    // (AR) المرحلة 4: توليد البايت كود / (EN) Phase 4: Code generation
    auto emitStart = std::chrono::high_resolution_clock::now();
    if (!codeGeneration()) {
        result.errorMessage = "فشل توليد الكود / Code generation failed";
        return result;
    }
    result.emitterTime = measureTime(emitStart);
    result.bytecodeSize = bytecode_.size();
    result.constantsCount = constants_.size();
    
    result.totalTime = measureTime(startTime);
    result.success = true;
    
    lastCompilation_ = result;
    
    if (options_.verbose) {
        result.print();
    }
    
    // (AR) احفظ البايت كود إذا طُلب / (EN) Save bytecode if requested
    if (options_.saveBytecode && !options_.bytecodeOutput.empty()) {
        saveBytecode(options_.bytecodeOutput);
    }
    
    return result;
}

CompilationResult CompilerPipeline::compileFile(const std::string& filename) {
    if (options_.verbose) {
        std::cout << "[المترجم / Compiler] قراءة الملف / Reading file: " << filename << "\n";
    }
    
    std::string source = readFile(filename);
    if (source.empty()) {
        CompilationResult result;
        result.success = false;
        result.errorMessage = "فشل قراءة الملف / Failed to read file: " + filename;
        return result;
    }
    
    return compile(source, filename);
}

ExecutionResult CompilerPipeline::execute() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    ExecutionResult result;
    result.success = false;
    
    if (bytecode_.empty()) {
        result.errorMessage = "لا يوجد بايت كود للتنفيذ / No bytecode to execute";
        return result;
    }
    
    if (options_.verbose) {
        std::cout << "[VM] بدء التنفيذ / Starting execution...\n";
    }
    
    try {
        // (AR) حمّل البايت كود في VM / (EN) Load bytecode into VM
        vm_->loadBytecode(bytecode_.data(), bytecode_.size());
        vm_->loadConstants(constants_);
        
        // (AR) نفّذ / (EN) Execute
        result.returnValue = vm_->execute();
        result.success = true;
        
        result.executionTime = measureTime(startTime);
        result.instructionsExecuted = vm_->getInstructionCount();
        result.gcCollections = gc_->getStats().youngGenCollections + 
                              gc_->getStats().oldGenCollections;
        result.peakMemoryUsage = gc_->getStats().peakMemoryUsage;
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("خطأ في التنفيذ / Execution error: ") + e.what();
        result.success = false;
    }
    
    lastExecution_ = result;
    
    if (options_.verbose) {
        result.print();
    }
    
    return result;
}

ExecutionResult CompilerPipeline::compileAndExecute(const std::string& source, 
                                                   const std::string& filename) {
    CompilationResult compResult = compile(source, filename);
    
    if (!compResult.success) {
        ExecutionResult execResult;
        execResult.success = false;
        execResult.errorMessage = "فشلت الترجمة / Compilation failed: " + compResult.errorMessage;
        return execResult;
    }
    
    return execute();
}

bool CompilerPipeline::saveBytecode(const std::string& filename) {
    if (bytecode_.empty()) {
        std::cerr << "[خطأ / Error] لا يوجد بايت كود للحفظ / No bytecode to save\n";
        return false;
    }
    
    try {
        Bytecode::BytecodeWriter writer(filename);
        
        // (AR) أنشئ الترويسة / (EN) Create header
        Bytecode::FileHeader header;
        header.magic = Bytecode::MAGIC_NUMBER;
        header.versionMajor = Bytecode::FORMAT_VERSION_MAJOR;
        header.versionMinor = Bytecode::FORMAT_VERSION_MINOR;
        header.flags = options_.includeDebugInfo ? Bytecode::FLAG_DEBUG_INFO : 0;
        header.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        header.codeSize = static_cast<uint32_t>(bytecode_.size());
        header.constantsCount = static_cast<uint32_t>(constants_.size());
        header.symbolsCount = 0;
        
        // (AR) اكتب الأقسام / (EN) Write sections
        writer.writeHeader(header);
        writer.writeConstants(constants_);
        writer.writeCode(bytecode_.data(), bytecode_.size());
        
        if (options_.includeDebugInfo) {
            Bytecode::DebugInfo debug;
            
            // (AR) ملء معلومات التنقيح / (EN) Fill debug info
            debug.sourceFile = inputFile_;
            
            // (AR) نسخ معلومات الأسطر من الوحدة / (EN) Copy line info from module
            if (module_) {
                // يمكن إضافة معلومات أكثر تفصيلاً هنا
                // More detailed info can be added here
            }
            
            // (AR) تسجيل أسماء المتغيرات / (EN) Record variable names
            for (const auto& global : module_->getGlobals()) {
                debug.variableMap[global->name] = static_cast<uint32_t>(debug.localNames.size());
                debug.localNames.push_back(global->name);
            }
            
            writer.writeDebugInfo(debug);
        }
        
        writer.finalize();
        
        if (options_.verbose) {
            std::cout << "[المترجم / Compiler] تم حفظ البايت كود / Bytecode saved: " 
                     << filename << "\n";
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[خطأ / Error] فشل حفظ البايت كود / Failed to save bytecode: " 
                 << e.what() << "\n";
        return false;
    }
}

ExecutionResult CompilerPipeline::loadAndExecute(const std::string& filename) {
    ExecutionResult result;
    result.success = false;
    
    try {
        Bytecode::BytecodeLoader loader(filename);
        
        if (!loader.load()) {
            result.errorMessage = "فشل تحميل الملف / Failed to load file: " + loader.getError();
            return result;
        }
        
        if (!loader.verify()) {
            result.errorMessage = "فشل التحقق من الملف / File verification failed: " + loader.getError();
            return result;
        }
        
        // (AR) حمّل البيانات / (EN) Load data
        bytecode_ = loader.getCode();
        constants_ = loader.getConstants();
        
        if (options_.verbose) {
            std::cout << "[المترجم / Compiler] تم تحميل البايت كود / Bytecode loaded: " 
                     << filename << "\n";
            loader.getHeader().print();
        }
        
        // (AR) نفّذ / (EN) Execute
        return execute();
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("خطأ في التحميل / Load error: ") + e.what();
        return result;
    }
}

void CompilerPipeline::setOptions(const CompilationOptions& options) {
    options_ = options;
    
    // (AR) حدّث إعدادات VM و GC / (EN) Update VM and GC settings
    if (vm_) {
        vm_->setConfig(options_.vmConfig);
    }
    if (gc_) {
        gc_->setConfig(options_.gcConfig);
    }
}

void CompilerPipeline::printCompilationStats() const {
    if (lastCompilation_.success) {
        lastCompilation_.print();
    }
}

void CompilerPipeline::printExecutionStats() const {
    if (lastExecution_.success) {
        lastExecution_.print();
    }
}

void CompilerPipeline::printGCStats() const {
    if (gc_) {
        gc_->getStats().print();
    }
}

void CompilerPipeline::dumpBytecode() const {
    if (bytecode_.empty()) {
        std::cout << "[تفريغ / Dump] لا يوجد بايت كود / No bytecode\n";
        return;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "تفريغ البايت كود / Bytecode Dump\n";
    std::cout << "========================================\n";
    std::cout << "الحجم / Size: " << bytecode_.size() << " bytes\n\n";
    
    for (size_t i = 0; i < bytecode_.size(); ++i) {
        if (i % 16 == 0) {
            std::cout << std::setw(6) << std::setfill('0') << std::hex << i << ": ";
        }
        
        std::cout << std::setw(2) << std::setfill('0') << std::hex 
                 << static_cast<int>(bytecode_[i]) << " ";
        
        if ((i + 1) % 16 == 0) {
            std::cout << "\n";
        }
    }
    
    std::cout << std::dec << "\n========================================\n\n";
}

void CompilerPipeline::dumpAST() const {
    if (!ast_) {
        std::cout << "[تفريغ / Dump] لا يوجد AST / No AST\n";
        return;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "تفريغ AST / AST Dump\n";
    std::cout << "========================================\n";
    ast_->print();
    std::cout << "========================================\n\n";
}

// ========================================
// Internal Methods
// ========================================

bool CompilerPipeline::lexicalAnalysis(const std::string& source, 
                                      const std::string& filename) {
    try {
        tokens_ = lexer_->tokenize(source, filename);
        
        if (options_.verbose) {
            std::cout << "[المحلل اللفظي / Lexer] " << tokens_.size() 
                     << " رمز / tokens\n";
        }
        
        return !tokens_.empty();
        
    } catch (const std::exception& e) {
        logError("Lexer", e.what());
        return false;
    }
}

bool CompilerPipeline::parsing() {
    try {
        ast_ = parser_->parse(tokens_);
        
        if (options_.verbose) {
            std::cout << "[المحلل النحوي / Parser] " << ast_->countNodes() 
                     << " عقدة / nodes\n";
        }
        
        return ast_ != nullptr;
        
    } catch (const std::exception& e) {
        logError("Parser", e.what());
        return false;
    }
}

bool CompilerPipeline::optimization() {
    if (!ast_) return false;
    
    try {
        optimizer_->setLevel(options_.optimizationLevel);
        ast_ = optimizer_->optimize(std::move(ast_));
        
        if (options_.verbose) {
            std::cout << "[المُحسِّن / Optimizer] مستوى / level " 
                     << options_.optimizationLevel << "\n";
        }
        
        return ast_ != nullptr;
        
    } catch (const std::exception& e) {
        logError("Optimizer", e.what());
        return false;
    }
}

bool CompilerPipeline::codeGeneration() {
    if (!ast_) return false;
    
    try {
        emitter_->emit(ast_.get());
        bytecode_ = emitter_->getBytecode();
        constants_ = emitter_->getConstants();
        
        if (options_.verbose) {
            std::cout << "[مولد الكود / Emitter] " << bytecode_.size() 
                     << " بايت / bytes, " << constants_.size() 
                     << " ثابت / constants\n";
        }
        
        return !bytecode_.empty();
        
    } catch (const std::exception& e) {
        logError("Emitter", e.what());
        return false;
    }
}

void CompilerPipeline::logError(const std::string& phase, const std::string& message) {
    std::string error = "[" + phase + "] " + message;
    errors_.push_back(error);
    
    if (options_.verbose) {
        std::cerr << error << "\n";
    }
}

// ========================================
// Utility Functions
// ========================================

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[خطأ / Error] فشل فتح الملف / Failed to open file: " 
                 << filename << "\n";
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

double measureTime(std::chrono::high_resolution_clock::time_point start) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count() / 1000000.0; // Convert to seconds
}

std::string formatMemorySize(size_t bytes) {
    const char* units[] = { "B", "KB", "MB", "GB" };
    int unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit < 3) {
        size /= 1024.0;
        unit++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return oss.str();
}

std::string formatTime(double seconds) {
    if (seconds < 0.001) {
        return std::to_string(static_cast<int>(seconds * 1000000)) + " µs";
    } else if (seconds < 1.0) {
        return std::to_string(static_cast<int>(seconds * 1000)) + " ms";
    } else {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << seconds << " s";
        return oss.str();
    }
}

} // namespace Sad
