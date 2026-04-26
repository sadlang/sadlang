/**
 * @file wasm32_target.cpp
 * @brief WebAssembly 32-bit target configuration for Sad compiler
 * 
 * هدف WebAssembly 32-bit لمترجم ص
 * يدعم الترجمة لتشغيل كود ص في المتصفحات
 */

#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <memory>

namespace sad {
namespace backend {
namespace targets {

/**
 * تكوين هدف WebAssembly
 * WebAssembly Target Configuration
 */
class Wasm32Target {
public:
    // ثوابت الهدف
    static constexpr const char* TRIPLE = "wasm32-unknown-unknown";
    static constexpr const char* TRIPLE_WASI = "wasm32-wasi";
    static constexpr const char* TRIPLE_EMSCRIPTEN = "wasm32-unknown-emscripten";
    static constexpr const char* CPU = "generic";
    static constexpr const char* FEATURES = "+simd128,+bulk-memory,+mutable-globals";
    
    /**
     * أنواع أهداف WASM المدعومة
     */
    enum class WasmEnvironment {
        Unknown,      // بدون بيئة محددة
        WASI,         // واجهة نظام WebAssembly
        Emscripten,   // بيئة Emscripten
        Browser       // مباشرة في المتصفح
    };
    
    /**
     * خيارات تحسين WASM
     */
    struct WasmOptimization {
        bool enableSIMD = true;           // تفعيل SIMD 128-bit
        bool enableBulkMemory = true;     // عمليات الذاكرة الجماعية
        bool enableTailCall = false;      // تحسين tail calls
        bool enableMultiValue = true;     // إرجاع قيم متعددة
        bool enableReferenceTypes = true; // أنواع المراجع
        bool enableGC = false;            // جمع القمامة (تجريبي)
        bool enableThreads = false;       // تعدد الخيوط
        bool enableExceptionHandling = false; // معالجة الاستثناءات
        int optimizationLevel = 2;        // مستوى التحسين (0-3)
        bool enableLTO = false;           // Link-Time Optimization
        bool stripDebug = false;          // إزالة معلومات التصحيح
        size_t stackSize = 1024 * 1024;   // حجم المكدس (1MB افتراضي)
        size_t initialMemory = 16;        // الذاكرة الأولية (صفحات 64KB)
        size_t maxMemory = 256;           // الحد الأقصى للذاكرة (صفحات)
    };
    
private:
    WasmEnvironment environment_;
    WasmOptimization options_;
    std::unique_ptr<llvm::TargetMachine> targetMachine_;
    bool initialized_ = false;
    
public:
    Wasm32Target(WasmEnvironment env = WasmEnvironment::Browser)
        : environment_(env) {}
    
    /**
     * تهيئة الهدف
     * @return true إذا نجحت التهيئة
     */
    bool initialize() {
        if (initialized_) return true;
        
        // تهيئة مكونات LLVM المطلوبة
        LLVMInitializeWebAssemblyTargetInfo();
        LLVMInitializeWebAssemblyTarget();
        LLVMInitializeWebAssemblyTargetMC();
        LLVMInitializeWebAssemblyAsmPrinter();
        LLVMInitializeWebAssemblyAsmParser();
        
        // الحصول على الهدف
        std::string error;
        const llvm::Target* target = llvm::TargetRegistry::lookupTarget(
            getTriple(), error);
        
        if (!target) {
            lastError_ = "فشل في العثور على هدف WebAssembly: " + error;
            return false;
        }
        
        // إنشاء خيارات الهدف
        llvm::TargetOptions targetOptions;
        targetOptions.EmitAddrsig = false;
        
        // تحديد مستوى التحسين
        llvm::CodeGenOpt::Level optLevel;
        switch (options_.optimizationLevel) {
            case 0: optLevel = llvm::CodeGenOpt::None; break;
            case 1: optLevel = llvm::CodeGenOpt::Less; break;
            case 2: optLevel = llvm::CodeGenOpt::Default; break;
            case 3: optLevel = llvm::CodeGenOpt::Aggressive; break;
            default: optLevel = llvm::CodeGenOpt::Default;
        }
        
        // إنشاء آلة الهدف
        targetMachine_.reset(target->createTargetMachine(
            getTriple(),
            CPU,
            getFeatureString(),
            targetOptions,
            llvm::Reloc::PIC_,
            llvm::CodeModel::Small,
            optLevel
        ));
        
        if (!targetMachine_) {
            lastError_ = "فشل في إنشاء آلة الهدف WebAssembly";
            return false;
        }
        
        initialized_ = true;
        return true;
    }
    
    /**
     * الحصول على Triple المناسب للبيئة
     */
    const char* getTriple() const {
        switch (environment_) {
            case WasmEnvironment::WASI:
                return TRIPLE_WASI;
            case WasmEnvironment::Emscripten:
                return TRIPLE_EMSCRIPTEN;
            default:
                return TRIPLE;
        }
    }
    
    /**
     * بناء سلسلة الميزات من الخيارات
     */
    std::string getFeatureString() const {
        std::string features;
        
        if (options_.enableSIMD) features += "+simd128,";
        if (options_.enableBulkMemory) features += "+bulk-memory,";
        if (options_.enableTailCall) features += "+tail-call,";
        if (options_.enableMultiValue) features += "+multivalue,";
        if (options_.enableReferenceTypes) features += "+reference-types,";
        if (options_.enableGC) features += "+gc,";
        if (options_.enableThreads) features += "+atomics,";
        if (options_.enableExceptionHandling) features += "+exception-handling,";
        
        features += "+mutable-globals,+sign-ext";
        
        return features;
    }
    
    /**
     * ترجمة وحدة LLVM إلى ملف WASM
     * @param module وحدة LLVM
     * @param outputPath مسار الملف الناتج
     * @return true إذا نجحت الترجمة
     */
    bool compile(llvm::Module& module, const std::string& outputPath) {
        if (!initialized_ && !initialize()) {
            return false;
        }
        
        // تطبيق تخطيط البيانات
        module.setDataLayout(targetMachine_->createDataLayout());
        module.setTargetTriple(getTriple());
        
        // فتح ملف الإخراج
        std::error_code errorCode;
        llvm::raw_fd_ostream outputFile(outputPath, errorCode, llvm::sys::fs::OF_None);
        
        if (errorCode) {
            lastError_ = "فشل في فتح ملف الإخراج: " + errorCode.message();
            return false;
        }
        
        // إنشاء مدير التمريرات
        llvm::legacy::PassManager passManager;
        
        // إضافة تمرير توليد الكود
        if (targetMachine_->addPassesToEmitFile(
                passManager, outputFile, nullptr,
                llvm::CGFT_ObjectFile)) {
            lastError_ = "الهدف لا يدعم توليد ملفات الكائنات";
            return false;
        }
        
        // تشغيل التمريرات
        passManager.run(module);
        outputFile.flush();
        
        return true;
    }
    
    /**
     * توليد ملف JavaScript مساعد
     * @param moduleName اسم الوحدة
     * @param outputPath مسار ملف JS الناتج
     */
    bool generateJSGlue(const std::string& moduleName, 
                        const std::string& outputPath,
                        const std::vector<std::string>& exports) {
        std::error_code errorCode;
        llvm::raw_fd_ostream jsFile(outputPath, errorCode, llvm::sys::fs::OF_None);
        
        if (errorCode) {
            lastError_ = "فشل في إنشاء ملف JS: " + errorCode.message();
            return false;
        }
        
        // توليد كود JS
        jsFile << "// ملف JavaScript مُولَّد تلقائياً بواسطة مترجم ص\n";
        jsFile << "// Auto-generated JavaScript glue for Sad WebAssembly module\n\n";
        
        jsFile << "const " << moduleName << " = (async () => {\n";
        jsFile << "  const memory = new WebAssembly.Memory({ initial: " 
               << options_.initialMemory << ", maximum: " 
               << options_.maxMemory << " });\n\n";
        
        // دوال الاستيراد
        jsFile << "  const importObject = {\n";
        jsFile << "    env: {\n";
        jsFile << "      memory: memory,\n";
        jsFile << "      // دوال الطباعة\n";
        jsFile << "      اطبع: (ptr) => console.log(readString(ptr)),\n";
        jsFile << "      اطبع_رقم: (n) => console.log(n),\n";
        jsFile << "      // دوال DOM\n";
        jsFile << "      __sad_dom_query: (selector) => document.querySelector(readString(selector)),\n";
        jsFile << "      __sad_dom_create: (tag) => document.createElement(readString(tag)),\n";
        jsFile << "      // دوال الشبكة\n";
        jsFile << "      __sad_fetch: async (url) => fetch(readString(url)),\n";
        jsFile << "    },\n";
        jsFile << "    wasi_snapshot_preview1: {\n";
        jsFile << "      fd_write: () => {},\n";
        jsFile << "      fd_close: () => {},\n";
        jsFile << "      fd_seek: () => {},\n";
        jsFile << "      proc_exit: (code) => { throw new Error('Exit: ' + code); }\n";
        jsFile << "    }\n";
        jsFile << "  };\n\n";
        
        // تحميل WASM
        jsFile << "  const wasmPath = './" << moduleName << ".wasm';\n";
        jsFile << "  const { instance } = await WebAssembly.instantiateStreaming(\n";
        jsFile << "    fetch(wasmPath), importObject\n";
        jsFile << "  );\n\n";
        
        // دوال مساعدة
        jsFile << "  // دالة قراءة النص من الذاكرة\n";
        jsFile << "  function readString(ptr) {\n";
        jsFile << "    const mem = new Uint8Array(memory.buffer);\n";
        jsFile << "    let end = ptr;\n";
        jsFile << "    while (mem[end] !== 0) end++;\n";
        jsFile << "    return new TextDecoder('utf-8').decode(mem.slice(ptr, end));\n";
        jsFile << "  }\n\n";
        
        jsFile << "  // دالة كتابة النص في الذاكرة\n";
        jsFile << "  function writeString(str, ptr) {\n";
        jsFile << "    const mem = new Uint8Array(memory.buffer);\n";
        jsFile << "    const encoder = new TextEncoder();\n";
        jsFile << "    const bytes = encoder.encode(str);\n";
        jsFile << "    mem.set(bytes, ptr);\n";
        jsFile << "    mem[ptr + bytes.length] = 0;\n";
        jsFile << "    return bytes.length;\n";
        jsFile << "  }\n\n";
        
        // تصدير الدوال
        jsFile << "  return {\n";
        jsFile << "    instance,\n";
        jsFile << "    memory,\n";
        jsFile << "    readString,\n";
        jsFile << "    writeString,\n";
        for (const auto& exp : exports) {
            jsFile << "    " << exp << ": instance.exports." << exp << ",\n";
        }
        jsFile << "  };\n";
        jsFile << "})();\n";
        
        return true;
    }
    
    /**
     * توليد ملف HTML للاختبار
     */
    bool generateTestHTML(const std::string& moduleName,
                          const std::string& outputPath) {
        std::error_code errorCode;
        llvm::raw_fd_ostream htmlFile(outputPath, errorCode, llvm::sys::fs::OF_None);
        
        if (errorCode) return false;
        
        htmlFile << "<!DOCTYPE html>\n";
        htmlFile << "<html dir=\"rtl\" lang=\"ar\">\n";
        htmlFile << "<head>\n";
        htmlFile << "  <meta charset=\"UTF-8\">\n";
        htmlFile << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        htmlFile << "  <title>تطبيق ص - " << moduleName << "</title>\n";
        htmlFile << "  <style>\n";
        htmlFile << "    body { font-family: 'Segoe UI', Tahoma, sans-serif; margin: 20px; }\n";
        htmlFile << "    #output { background: #f5f5f5; padding: 15px; border-radius: 8px; }\n";
        htmlFile << "  </style>\n";
        htmlFile << "</head>\n";
        htmlFile << "<body>\n";
        htmlFile << "  <h1>تطبيق " << moduleName << "</h1>\n";
        htmlFile << "  <div id=\"app\"></div>\n";
        htmlFile << "  <div id=\"output\"><pre></pre></div>\n";
        htmlFile << "  <script src=\"" << moduleName << ".js\"></script>\n";
        htmlFile << "  <script>\n";
        htmlFile << "    " << moduleName << ".then(mod => {\n";
        htmlFile << "      if (mod.instance.exports.رئيسية) {\n";
        htmlFile << "        mod.instance.exports.رئيسية();\n";
        htmlFile << "      } else if (mod.instance.exports.main) {\n";
        htmlFile << "        mod.instance.exports.main();\n";
        htmlFile << "      }\n";
        htmlFile << "    });\n";
        htmlFile << "  </script>\n";
        htmlFile << "</body>\n";
        htmlFile << "</html>\n";
        
        return true;
    }
    
    // إعدادات
    void setEnvironment(WasmEnvironment env) { environment_ = env; }
    WasmEnvironment getEnvironment() const { return environment_; }
    
    void setOptions(const WasmOptimization& opts) { options_ = opts; }
    WasmOptimization& getOptions() { return options_; }
    const WasmOptimization& getOptions() const { return options_; }
    
    llvm::TargetMachine* getTargetMachine() { return targetMachine_.get(); }
    
    const std::string& getLastError() const { return lastError_; }
    
private:
    std::string lastError_;
};

/**
 * مساعد لإنشاء هدف WASM من سطر الأوامر
 */
std::unique_ptr<Wasm32Target> createWasmTargetFromArgs(
    const std::string& envStr,
    int optLevel = 2,
    bool enableSIMD = true) {
    
    auto target = std::make_unique<Wasm32Target>();
    
    // تحديد البيئة
    if (envStr == "wasi" || envStr == "واسي") {
        target->setEnvironment(Wasm32Target::WasmEnvironment::WASI);
    } else if (envStr == "emscripten") {
        target->setEnvironment(Wasm32Target::WasmEnvironment::Emscripten);
    } else {
        target->setEnvironment(Wasm32Target::WasmEnvironment::Browser);
    }
    
    // تطبيق الخيارات
    auto& opts = target->getOptions();
    opts.optimizationLevel = optLevel;
    opts.enableSIMD = enableSIMD;
    
    return target;
}

} // namespace targets
} // namespace backend
} // namespace sad
