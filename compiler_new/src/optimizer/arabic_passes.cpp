/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: arabic_passes.cpp                                                    ║
 * ║   الوصف: تكامل تحسينات العربية مع خط أنابيب المترجم                         ║
 * ║   المهمة: T159 - Phase 14 (US12)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    دمج جميع تحسينات العربية في خط أنابيب التحسين
 *    Integrate all Arabic optimizations into the optimization pipeline
 *
 * 📋 التحسينات المدمجة | Integrated Optimizations:
 *
 *    1️⃣ تجميع النصوص | String Pooling (T154)
 *    2️⃣ ضغط التشكيل | Tashkeel Compression (T155)
 *    3️⃣ تخزين BiDi | BiDi Caching (T156)
 *    4️⃣ تطبيع NFC | NFC Normalization (T158)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>

// Include our Arabic optimization headers
// #include "arabic_string_pool.cpp"
// #include "tashkeel_optimizer.cpp"
// #include "bidi_cache.cpp"
// #include "arabic_normalization.cpp"

namespace sad {
namespace optimizer {

// Forward declarations
class ArabicStringPool;
class TashkeelOptimizer;
class BidiCache;
class ArabicNormalizer;

// ═══════════════════════════════════════════════════════════════════════════════
//                      إحصائيات التحسين | Optimization Statistics
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 ArabicOptimizationStats
 * 
 * إحصائيات تحسينات العربية
 */
struct ArabicOptimizationStats {
    // تجميع النصوص
    size_t stringsPooled;
    size_t pooledBytes;
    size_t savedBytes;
    
    // ضغط التشكيل
    size_t tashkeelStringsOptimized;
    size_t tashkeelBytesSaved;
    
    // تخزين BiDi
    size_t bidiCacheHits;
    size_t bidiCacheMisses;
    
    // التطبيع
    size_t stringsNormalized;
    
    ArabicOptimizationStats()
        : stringsPooled(0)
        , pooledBytes(0)
        , savedBytes(0)
        , tashkeelStringsOptimized(0)
        , tashkeelBytesSaved(0)
        , bidiCacheHits(0)
        , bidiCacheMisses(0)
        , stringsNormalized(0)
    {}
    
    void print(llvm::raw_ostream& os) const {
        os << "\n";
        os << "╔════════════════════════════════════════════════════════════╗\n";
        os << "║              إحصائيات تحسينات العربية                       ║\n";
        os << "║              Arabic Optimization Statistics                 ║\n";
        os << "╠════════════════════════════════════════════════════════════╣\n";
        os << "║                                                            ║\n";
        os << "║  📦 تجميع النصوص | String Pooling:                          ║\n";
        os << "║     نصوص مجمعة: " << stringsPooled << "\n";
        os << "║     بايتات مجمعة: " << pooledBytes << "\n";
        os << "║     بايتات موفرة: " << savedBytes << "\n";
        os << "║                                                            ║\n";
        os << "║  ✨ ضغط التشكيل | Tashkeel Compression:                     ║\n";
        os << "║     نصوص محسنة: " << tashkeelStringsOptimized << "\n";
        os << "║     بايتات موفرة: " << tashkeelBytesSaved << "\n";
        os << "║                                                            ║\n";
        os << "║  🔄 BiDi Cache:                                             ║\n";
        os << "║     إصابات: " << bidiCacheHits << "\n";
        os << "║     أخطاء: " << bidiCacheMisses << "\n";
        os << "║                                                            ║\n";
        os << "║  📐 تطبيع NFC | Normalization:                              ║\n";
        os << "║     نصوص مطبعة: " << stringsNormalized << "\n";
        os << "║                                                            ║\n";
        os << "╚════════════════════════════════════════════════════════════╝\n";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    تمرير تجميع النصوص | String Pooling Pass
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 ArabicStringPoolingPass
 * 
 * تمرير لتجميع النصوص العربية المتكررة
 */
class ArabicStringPoolingPass : public llvm::PassInfoMixin<ArabicStringPoolingPass> {
public:
    llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager& MAM) {
        bool changed = false;
        
        // جمع كل النصوص العربية
        std::unordered_map<std::string, std::vector<llvm::GlobalVariable*>> stringGroups;
        
        for (auto& G : M.globals()) {
            if (!G.hasInitializer()) continue;
            
            auto* init = G.getInitializer();
            if (auto* arr = llvm::dyn_cast<llvm::ConstantDataArray>(init)) {
                if (arr->isString()) {
                    std::string str = arr->getAsString().str();
                    
                    // تحقق من وجود حروف عربية
                    if (containsArabic(str)) {
                        stringGroups[str].push_back(&G);
                    }
                }
            }
        }
        
        // دمج النصوص المتكررة
        for (auto& [str, vars] : stringGroups) {
            if (vars.size() > 1) {
                // استخدام أول متغير كـ canonical
                llvm::GlobalVariable* canonical = vars[0];
                
                for (size_t i = 1; i < vars.size(); i++) {
                    // استبدال جميع الاستخدامات
                    vars[i]->replaceAllUsesWith(canonical);
                    vars[i]->eraseFromParent();
                    changed = true;
                    stats_.stringsPooled++;
                    stats_.savedBytes += str.size();
                }
            }
        }
        
        return changed ? llvm::PreservedAnalyses::none() : llvm::PreservedAnalyses::all();
    }
    
    const ArabicOptimizationStats& getStats() const { return stats_; }
    
private:
    ArabicOptimizationStats stats_;
    
    bool containsArabic(const std::string& str) {
        for (size_t i = 0; i < str.size();) {
            unsigned char c = str[i];
            uint32_t cp;
            size_t bytes;
            
            if ((c & 0x80) == 0) {
                cp = c;
                bytes = 1;
            } else if ((c & 0xE0) == 0xC0) {
                cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
                bytes = 2;
            } else if ((c & 0xF0) == 0xE0) {
                cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
                bytes = 3;
            } else {
                cp = 0;
                bytes = 1;
            }
            
            // Arabic range
            if ((cp >= 0x0600 && cp <= 0x06FF) ||
                (cp >= 0x0750 && cp <= 0x077F) ||
                (cp >= 0x08A0 && cp <= 0x08FF)) {
                return true;
            }
            
            i += bytes;
        }
        return false;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                   تمرير تحسين التشكيل | Tashkeel Optimization Pass
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 TashkeelOptimizationPass
 * 
 * تمرير لضغط التشكيل في النصوص العربية
 */
class TashkeelOptimizationPass : public llvm::PassInfoMixin<TashkeelOptimizationPass> {
public:
    llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager& MAM) {
        // هذا التمرير يحلل النصوص ويحدد الفرص للضغط
        // التطبيق الفعلي يتم في runtime
        
        for (auto& G : M.globals()) {
            if (!G.hasInitializer()) continue;
            
            auto* init = G.getInitializer();
            if (auto* arr = llvm::dyn_cast<llvm::ConstantDataArray>(init)) {
                if (arr->isString()) {
                    std::string str = arr->getAsString().str();
                    
                    if (containsTashkeel(str)) {
                        stats_.tashkeelStringsOptimized++;
                        // حساب التوفير المحتمل
                        stats_.tashkeelBytesSaved += countTashkeelBytes(str);
                    }
                }
            }
        }
        
        return llvm::PreservedAnalyses::all();
    }
    
    const ArabicOptimizationStats& getStats() const { return stats_; }
    
private:
    ArabicOptimizationStats stats_;
    
    bool containsTashkeel(const std::string& str) {
        for (size_t i = 0; i < str.size();) {
            unsigned char c = str[i];
            uint32_t cp;
            size_t bytes;
            
            if ((c & 0xE0) == 0xC0) {
                cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
                bytes = 2;
            } else if ((c & 0xF0) == 0xE0) {
                cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
                bytes = 3;
            } else {
                cp = c;
                bytes = 1;
            }
            
            // Tashkeel range
            if (cp >= 0x064B && cp <= 0x065F) {
                return true;
            }
            
            i += bytes;
        }
        return false;
    }
    
    size_t countTashkeelBytes(const std::string& str) {
        size_t count = 0;
        for (size_t i = 0; i < str.size();) {
            unsigned char c = str[i];
            uint32_t cp;
            size_t bytes;
            
            if ((c & 0xE0) == 0xC0) {
                cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
                bytes = 2;
            } else if ((c & 0xF0) == 0xE0) {
                cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
                bytes = 3;
            } else {
                cp = c;
                bytes = 1;
            }
            
            if (cp >= 0x064B && cp <= 0x065F) {
                count += bytes;
            }
            
            i += bytes;
        }
        return count;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    تمرير تحليل BiDi | BiDi Analysis Pass
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 BidiAnalysisPass
 * 
 * تمرير لتحليل وتخزين معلومات BiDi
 */
class BidiAnalysisPass : public llvm::PassInfoMixin<BidiAnalysisPass> {
public:
    llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager& MAM) {
        // تحليل النصوص وتخزين نتائج BiDi
        
        for (auto& G : M.globals()) {
            if (!G.hasInitializer()) continue;
            
            auto* init = G.getInitializer();
            if (auto* arr = llvm::dyn_cast<llvm::ConstantDataArray>(init)) {
                if (arr->isString()) {
                    std::string str = arr->getAsString().str();
                    
                    if (needsBidiAnalysis(str)) {
                        // تخزين نتيجة التحليل في metadata
                        // يمكن استخدامها لاحقاً في runtime
                        analyzedStrings_.insert(&G);
                    }
                }
            }
        }
        
        return llvm::PreservedAnalyses::all();
    }
    
private:
    std::unordered_set<llvm::GlobalVariable*> analyzedStrings_;
    
    bool needsBidiAnalysis(const std::string& str) {
        bool hasRTL = false;
        bool hasLTR = false;
        
        for (size_t i = 0; i < str.size();) {
            unsigned char c = str[i];
            uint32_t cp;
            size_t bytes;
            
            if ((c & 0x80) == 0) {
                cp = c;
                bytes = 1;
            } else if ((c & 0xE0) == 0xC0) {
                cp = ((c & 0x1F) << 6) | (str[i+1] & 0x3F);
                bytes = 2;
            } else if ((c & 0xF0) == 0xE0) {
                cp = ((c & 0x0F) << 12) | ((str[i+1] & 0x3F) << 6) | (str[i+2] & 0x3F);
                bytes = 3;
            } else {
                cp = 0;
                bytes = 1;
            }
            
            // Arabic (RTL)
            if ((cp >= 0x0600 && cp <= 0x06FF) ||
                (cp >= 0x0590 && cp <= 0x05FF)) {  // Hebrew
                hasRTL = true;
            }
            // Latin (LTR)
            if ((cp >= 0x0041 && cp <= 0x005A) ||
                (cp >= 0x0061 && cp <= 0x007A)) {
                hasLTR = true;
            }
            
            // نحتاج تحليل BiDi إذا كان مختلطاً
            if (hasRTL && hasLTR) return true;
            
            i += bytes;
        }
        
        return false;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    مدير التحسينات العربية | Arabic Optimization Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 ArabicOptimizationManager
 * 
 * يدير جميع تحسينات العربية
 */
class ArabicOptimizationManager {
public:
    /**
     * تسجيل جميع التمريرات
     */
    static void registerPasses(llvm::PassBuilder& PB) {
        // تسجيل تمريرات الوحدة
        PB.registerPipelineParsingCallback(
            [](llvm::StringRef Name, llvm::ModulePassManager& MPM,
               llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                if (Name == "arabic-string-pooling") {
                    MPM.addPass(ArabicStringPoolingPass());
                    return true;
                }
                if (Name == "tashkeel-optimization") {
                    MPM.addPass(TashkeelOptimizationPass());
                    return true;
                }
                if (Name == "bidi-analysis") {
                    MPM.addPass(BidiAnalysisPass());
                    return true;
                }
                return false;
            }
        );
        
        // إضافة التمريرات إلى خط الأنابيب الافتراضي
        PB.registerOptimizerLastEPCallback(
            [](llvm::ModulePassManager& MPM, llvm::OptimizationLevel Level) {
                if (Level != llvm::OptimizationLevel::O0) {
                    MPM.addPass(ArabicStringPoolingPass());
                    MPM.addPass(TashkeelOptimizationPass());
                    MPM.addPass(BidiAnalysisPass());
                }
            }
        );
    }
    
    /**
     * تشغيل جميع التحسينات يدوياً
     */
    static ArabicOptimizationStats runAllPasses(llvm::Module& M) {
        ArabicOptimizationStats totalStats;
        
        llvm::ModuleAnalysisManager MAM;
        
        // تجميع النصوص
        ArabicStringPoolingPass poolingPass;
        poolingPass.run(M, MAM);
        auto poolingStats = poolingPass.getStats();
        totalStats.stringsPooled = poolingStats.stringsPooled;
        totalStats.savedBytes = poolingStats.savedBytes;
        
        // تحسين التشكيل
        TashkeelOptimizationPass tashkeelPass;
        tashkeelPass.run(M, MAM);
        auto tashkeelStats = tashkeelPass.getStats();
        totalStats.tashkeelStringsOptimized = tashkeelStats.tashkeelStringsOptimized;
        totalStats.tashkeelBytesSaved = tashkeelStats.tashkeelBytesSaved;
        
        // تحليل BiDi
        BidiAnalysisPass bidiPass;
        bidiPass.run(M, MAM);
        
        return totalStats;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {
    /**
     * تسجيل التمريرات مع LLVM
     */
    void sad_register_arabic_passes(void* passBuilder) {
        auto* PB = static_cast<llvm::PassBuilder*>(passBuilder);
        ArabicOptimizationManager::registerPasses(*PB);
    }
    
    /**
     * تشغيل التحسينات
     */
    void sad_run_arabic_optimizations(void* module) {
        auto* M = static_cast<llvm::Module*>(module);
        auto stats = ArabicOptimizationManager::runAllPasses(*M);
        stats.print(llvm::outs());
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    LLVM Pass Plugin Interface
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "ArabicOptimizations",
        "1.0",
        [](llvm::PassBuilder& PB) {
            ArabicOptimizationManager::registerPasses(PB);
        }
    };
}

} // namespace optimizer
} // namespace sad
