/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: bidi_cache.cpp                                                       ║
 * ║   الوصف: تخزين مؤقت لحسابات النص ثنائي الاتجاه                             ║
 * ║   المهمة: T156 - Phase 14 (US12)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    تخزين نتائج تحليل BiDi للنصوص العربية لتجنب إعادة الحساب
 *    Cache BiDi analysis results for Arabic texts to avoid recomputation
 *
 * 📋 خوارزمية Unicode BiDi | Unicode BiDi Algorithm:
 *
 *    النص العربي يُكتب من اليمين لليسار (RTL)
 *    لكن الأرقام والنص الإنجليزي يُكتب من اليسار لليمين (LTR)
 *
 *    مثال | Example:
 *    المنطقي: "مرحباً 123 عالم"
 *    المرئي:  "ملاع 123 ًابحرم"  (مع تبديل اتجاه العرض)
 *
 * 🎯 استراتيجيات التخزين المؤقت | Caching Strategies:
 *
 *    1️⃣ تخزين مستويات BiDi للأحرف
 *    2️⃣ تخزين نتائج إعادة الترتيب
 *    3️⃣ تخزين معلومات الاتجاه لكل فقرة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/StringMap.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>
#include <cstdint>

namespace sad {
namespace optimizer {

// ═══════════════════════════════════════════════════════════════════════════════
//                          تعريفات BiDi | BiDi Definitions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 فئات BiDi | BiDi Categories (Unicode Bidirectional Categories)
 */
enum class BidiClass : uint8_t {
    // Strong types
    L = 0,   // Left-to-Right (Latin, etc.)
    R = 1,   // Right-to-Left (Hebrew)
    AL = 2,  // Arabic Letter
    
    // Weak types
    EN = 3,  // European Number
    ES = 4,  // European Number Separator
    ET = 5,  // European Number Terminator
    AN = 6,  // Arabic Number
    CS = 7,  // Common Number Separator
    NSM = 8, // Nonspacing Mark
    BN = 9,  // Boundary Neutral
    
    // Neutral types
    B = 10,  // Paragraph Separator
    S = 11,  // Segment Separator
    WS = 12, // Whitespace
    ON = 13, // Other Neutrals
    
    // Explicit formatting
    LRE = 14, // Left-to-Right Embedding
    LRO = 15, // Left-to-Right Override
    RLE = 16, // Right-to-Left Embedding
    RLO = 17, // Right-to-Left Override
    PDF = 18, // Pop Directional Format
    LRI = 19, // Left-to-Right Isolate
    RLI = 20, // Right-to-Left Isolate
    FSI = 21, // First Strong Isolate
    PDI = 22  // Pop Directional Isolate
};

/**
 * 📋 اتجاه النص | Text Direction
 */
enum class TextDirection : uint8_t {
    LTR = 0,      // يسار إلى يمين
    RTL = 1,      // يمين إلى يسار (العربية)
    MIXED = 2,    // مختلط
    NEUTRAL = 3   // محايد
};

/**
 * 📋 معلومات حرف BiDi | BiDi Character Info
 */
struct BidiCharInfo {
    uint32_t codepoint;        // نقطة الكود
    BidiClass bidiClass;       // فئة BiDi
    uint8_t embeddingLevel;    // مستوى التضمين (0-125)
    bool isMirrored;           // هل له شكل معكوس
    uint32_t mirrorCodepoint;  // نقطة الكود المعكوسة
    
    BidiCharInfo()
        : codepoint(0)
        , bidiClass(BidiClass::L)
        , embeddingLevel(0)
        , isMirrored(false)
        , mirrorCodepoint(0)
    {}
};

/**
 * 📋 نتيجة تحليل BiDi | BiDi Analysis Result
 */
struct BidiAnalysis {
    std::string originalText;              // النص الأصلي
    std::vector<BidiCharInfo> charInfo;    // معلومات كل حرف
    std::vector<size_t> visualOrder;       // ترتيب العرض
    TextDirection baseDirection;           // الاتجاه الأساسي
    uint8_t maxEmbeddingLevel;             // أعلى مستوى تضمين
    bool hasRTL;                           // يحتوي على RTL
    bool hasLTR;                           // يحتوي على LTR
    bool hasArabic;                        // يحتوي على عربي
    
    BidiAnalysis()
        : baseDirection(TextDirection::LTR)
        , maxEmbeddingLevel(0)
        , hasRTL(false)
        , hasLTR(false)
        , hasArabic(false)
    {}
    
    bool isMixed() const { return hasRTL && hasLTR; }
    bool isPureRTL() const { return hasRTL && !hasLTR; }
    bool isPureLTR() const { return hasLTR && !hasRTL; }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      محدد فئة BiDi | BiDi Class Resolver
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 BidiClassResolver
 * 
 * تحديد فئة BiDi لكل حرف
 */
class BidiClassResolver {
public:
    /**
     * تحديد فئة BiDi للـ codepoint
     */
    static BidiClass resolve(uint32_t cp) {
        // Arabic (U+0600 - U+06FF)
        if (cp >= 0x0600 && cp <= 0x06FF) {
            // Arabic-Indic digits are AN
            if (cp >= 0x0660 && cp <= 0x0669) return BidiClass::AN;
            // Extended Arabic-Indic digits
            if (cp >= 0x06F0 && cp <= 0x06F9) return BidiClass::EN;
            // Combining marks
            if (cp >= 0x064B && cp <= 0x065F) return BidiClass::NSM;
            if (cp == 0x0670) return BidiClass::NSM;
            // Most Arabic letters
            return BidiClass::AL;
        }
        
        // Arabic Supplement (U+0750 - U+077F)
        if (cp >= 0x0750 && cp <= 0x077F) {
            return BidiClass::AL;
        }
        
        // Arabic Extended-A (U+08A0 - U+08FF)
        if (cp >= 0x08A0 && cp <= 0x08FF) {
            return BidiClass::AL;
        }
        
        // Arabic Presentation Forms-A (U+FB50 - U+FDFF)
        if (cp >= 0xFB50 && cp <= 0xFDFF) {
            return BidiClass::AL;
        }
        
        // Arabic Presentation Forms-B (U+FE70 - U+FEFF)
        if (cp >= 0xFE70 && cp <= 0xFEFF) {
            return BidiClass::AL;
        }
        
        // Basic Latin (U+0000 - U+007F)
        if (cp < 0x0080) {
            if (cp >= 0x0041 && cp <= 0x005A) return BidiClass::L; // A-Z
            if (cp >= 0x0061 && cp <= 0x007A) return BidiClass::L; // a-z
            if (cp >= 0x0030 && cp <= 0x0039) return BidiClass::EN; // 0-9
            if (cp == 0x0020) return BidiClass::WS; // Space
            if (cp == 0x000A || cp == 0x000D) return BidiClass::B; // Newlines
            return BidiClass::ON; // Other
        }
        
        // Hebrew (U+0590 - U+05FF)
        if (cp >= 0x0590 && cp <= 0x05FF) {
            if (cp >= 0x0591 && cp <= 0x05BD) return BidiClass::NSM;
            return BidiClass::R;
        }
        
        // Default
        return BidiClass::L;
    }
    
    /**
     * التحقق من إمكانية العكس | Check if mirrored
     */
    static bool isMirrored(uint32_t cp) {
        // الأقواس والرموز التي لها أشكال معكوسة
        switch (cp) {
            case '(': case ')':
            case '[': case ']':
            case '{': case '}':
            case '<': case '>':
            case 0x00AB: case 0x00BB:  // « »
            case 0x2018: case 0x2019:  // ' '
            case 0x201C: case 0x201D:  // " "
            case 0x2039: case 0x203A:  // ‹ ›
                return true;
            default:
                return false;
        }
    }
    
    /**
     * الحصول على الشكل المعكوس | Get mirror codepoint
     */
    static uint32_t getMirror(uint32_t cp) {
        switch (cp) {
            case '(': return ')';
            case ')': return '(';
            case '[': return ']';
            case ']': return '[';
            case '{': return '}';
            case '}': return '{';
            case '<': return '>';
            case '>': return '<';
            case 0x00AB: return 0x00BB;
            case 0x00BB: return 0x00AB;
            case 0x2018: return 0x2019;
            case 0x2019: return 0x2018;
            case 0x201C: return 0x201D;
            case 0x201D: return 0x201C;
            case 0x2039: return 0x203A;
            case 0x203A: return 0x2039;
            default: return cp;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                          محلل BiDi | BiDi Analyzer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 BidiAnalyzer
 * 
 * تحليل النص وفق خوارزمية Unicode BiDi
 */
class BidiAnalyzer {
public:
    /**
     * قراءة codepoint من UTF-8
     */
    static uint32_t readUTF8(const char* str, size_t& bytesRead) {
        unsigned char c = str[0];
        
        if ((c & 0x80) == 0) {
            bytesRead = 1;
            return c;
        } else if ((c & 0xE0) == 0xC0) {
            bytesRead = 2;
            return ((c & 0x1F) << 6) | (str[1] & 0x3F);
        } else if ((c & 0xF0) == 0xE0) {
            bytesRead = 3;
            return ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
        } else if ((c & 0xF8) == 0xF0) {
            bytesRead = 4;
            return ((c & 0x07) << 18) | ((str[1] & 0x3F) << 12) | 
                   ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
        }
        
        bytesRead = 1;
        return 0xFFFD;
    }
    
    /**
     * تحليل نص كامل | Analyze full text
     */
    static BidiAnalysis analyze(const std::string& text) {
        BidiAnalysis result;
        result.originalText = text;
        
        // الخطوة 1: تحليل كل حرف
        size_t i = 0;
        while (i < text.size()) {
            size_t bytesRead;
            uint32_t cp = readUTF8(text.c_str() + i, bytesRead);
            
            BidiCharInfo info;
            info.codepoint = cp;
            info.bidiClass = BidiClassResolver::resolve(cp);
            info.isMirrored = BidiClassResolver::isMirrored(cp);
            info.mirrorCodepoint = BidiClassResolver::getMirror(cp);
            
            // تتبع أنواع النص
            if (info.bidiClass == BidiClass::AL || info.bidiClass == BidiClass::R) {
                result.hasRTL = true;
                if (info.bidiClass == BidiClass::AL) {
                    result.hasArabic = true;
                }
            } else if (info.bidiClass == BidiClass::L) {
                result.hasLTR = true;
            }
            
            result.charInfo.push_back(info);
            i += bytesRead;
        }
        
        // الخطوة 2: تحديد الاتجاه الأساسي
        result.baseDirection = determineBaseDirection(result.charInfo);
        
        // الخطوة 3: حساب مستويات التضمين
        computeEmbeddingLevels(result);
        
        // الخطوة 4: حساب ترتيب العرض
        computeVisualOrder(result);
        
        return result;
    }
    
private:
    /**
     * تحديد الاتجاه الأساسي | Determine base direction
     */
    static TextDirection determineBaseDirection(const std::vector<BidiCharInfo>& chars) {
        // البحث عن أول حرف قوي
        for (const auto& info : chars) {
            if (info.bidiClass == BidiClass::L) {
                return TextDirection::LTR;
            }
            if (info.bidiClass == BidiClass::R || info.bidiClass == BidiClass::AL) {
                return TextDirection::RTL;
            }
        }
        return TextDirection::LTR;  // الافتراضي
    }
    
    /**
     * حساب مستويات التضمين | Compute embedding levels
     */
    static void computeEmbeddingLevels(BidiAnalysis& analysis) {
        uint8_t baseLevel = (analysis.baseDirection == TextDirection::RTL) ? 1 : 0;
        
        // تنفيذ مبسط - في الإنتاج نستخدم الخوارزمية الكاملة
        for (auto& info : analysis.charInfo) {
            switch (info.bidiClass) {
                case BidiClass::L:
                    info.embeddingLevel = baseLevel;
                    break;
                    
                case BidiClass::R:
                case BidiClass::AL:
                    info.embeddingLevel = baseLevel | 1;  // RTL level
                    break;
                    
                case BidiClass::EN:
                case BidiClass::AN:
                    // الأرقام تأخذ مستوى يعتمد على السياق
                    info.embeddingLevel = baseLevel;
                    break;
                    
                default:
                    // الحروف المحايدة تأخذ مستوى الاتجاه الأساسي
                    info.embeddingLevel = baseLevel;
                    break;
            }
            
            if (info.embeddingLevel > analysis.maxEmbeddingLevel) {
                analysis.maxEmbeddingLevel = info.embeddingLevel;
            }
        }
    }
    
    /**
     * حساب ترتيب العرض | Compute visual order
     */
    static void computeVisualOrder(BidiAnalysis& analysis) {
        size_t n = analysis.charInfo.size();
        analysis.visualOrder.resize(n);
        
        // تهيئة بالترتيب المنطقي
        for (size_t i = 0; i < n; i++) {
            analysis.visualOrder[i] = i;
        }
        
        // عكس الأجزاء حسب مستوى التضمين
        // نبدأ من أعلى مستوى وننزل
        for (int level = analysis.maxEmbeddingLevel; level > 0; level--) {
            size_t runStart = 0;
            bool inRun = false;
            
            for (size_t i = 0; i <= n; i++) {
                bool currentAboveLevel = (i < n) && 
                    (analysis.charInfo[analysis.visualOrder[i]].embeddingLevel >= level);
                
                if (currentAboveLevel && !inRun) {
                    runStart = i;
                    inRun = true;
                } else if (!currentAboveLevel && inRun) {
                    // عكس هذا الجزء
                    reverseRange(analysis.visualOrder, runStart, i - 1);
                    inRun = false;
                }
            }
        }
    }
    
    /**
     * عكس مجموعة من الفهارس
     */
    static void reverseRange(std::vector<size_t>& arr, size_t start, size_t end) {
        while (start < end) {
            std::swap(arr[start], arr[end]);
            start++;
            end--;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                       التخزين المؤقت | BiDi Cache
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 BidiCache
 * 
 * تخزين مؤقت لنتائج تحليل BiDi
 */
class BidiCache {
private:
    // التخزين الرئيسي
    std::unordered_map<std::string, std::shared_ptr<BidiAnalysis>> cache;
    
    // إحصائيات
    size_t hits;
    size_t misses;
    size_t maxSize;
    
    // للحماية في multi-threading
    mutable std::mutex cacheMutex;
    
public:
    BidiCache(size_t maxCacheSize = 10000)
        : hits(0)
        , misses(0)
        , maxSize(maxCacheSize)
    {}
    
    /**
     * الحصول على تحليل أو إنشائه | Get or create analysis
     */
    std::shared_ptr<BidiAnalysis> getOrAnalyze(const std::string& text) {
        std::lock_guard<std::mutex> lock(cacheMutex);
        
        // البحث في الكاش
        auto it = cache.find(text);
        if (it != cache.end()) {
            hits++;
            return it->second;
        }
        
        misses++;
        
        // التحليل
        auto analysis = std::make_shared<BidiAnalysis>(BidiAnalyzer::analyze(text));
        
        // التخزين
        if (cache.size() >= maxSize) {
            evictOldest();
        }
        
        cache[text] = analysis;
        return analysis;
    }
    
    /**
     * التحقق السريع من الاتجاه | Quick direction check
     */
    TextDirection quickDirection(const std::string& text) {
        // تحليل سريع بدون تخزين
        bool hasRTL = false;
        bool hasLTR = false;
        
        size_t i = 0;
        while (i < text.size() && i < 100) {  // فحص أول 100 حرف فقط
            size_t bytesRead;
            uint32_t cp = BidiAnalyzer::readUTF8(text.c_str() + i, bytesRead);
            BidiClass bc = BidiClassResolver::resolve(cp);
            
            if (bc == BidiClass::AL || bc == BidiClass::R) {
                hasRTL = true;
            } else if (bc == BidiClass::L) {
                hasLTR = true;
            }
            
            if (hasRTL && hasLTR) {
                return TextDirection::MIXED;
            }
            
            i += bytesRead;
        }
        
        if (hasRTL && !hasLTR) return TextDirection::RTL;
        if (hasLTR && !hasRTL) return TextDirection::LTR;
        if (hasRTL && hasLTR) return TextDirection::MIXED;
        return TextDirection::NEUTRAL;
    }
    
    /**
     * مسح الكاش | Clear cache
     */
    void clear() {
        std::lock_guard<std::mutex> lock(cacheMutex);
        cache.clear();
        hits = 0;
        misses = 0;
    }
    
    /**
     * الإحصائيات | Statistics
     */
    double hitRate() const {
        std::lock_guard<std::mutex> lock(cacheMutex);
        size_t total = hits + misses;
        return total > 0 ? (double)hits / total : 0.0;
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(cacheMutex);
        return cache.size();
    }
    
private:
    /**
     * إزالة أقدم العناصر | Evict oldest entries
     */
    void evictOldest() {
        // تنفيذ بسيط - إزالة نصف الكاش
        size_t toRemove = cache.size() / 2;
        auto it = cache.begin();
        while (toRemove > 0 && it != cache.end()) {
            it = cache.erase(it);
            toRemove--;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                       مولد LLVM لـ BiDi | LLVM BiDi Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 BidiCodegen
 * 
 * توليد كود LLVM للتعامل مع BiDi
 */
class BidiCodegen {
private:
    llvm::LLVMContext& context;
    llvm::Module* module;
    llvm::IRBuilder<>& builder;
    
public:
    BidiCodegen(llvm::LLVMContext& ctx, llvm::Module* mod, llvm::IRBuilder<>& b)
        : context(ctx)
        , module(mod)
        , builder(b)
    {}
    
    /**
     * إنشاء ثابت لترتيب العرض | Create visual order constant
     */
    llvm::GlobalVariable* createVisualOrderConstant(const BidiAnalysis& analysis) {
        std::vector<uint32_t> order(analysis.visualOrder.begin(), analysis.visualOrder.end());
        
        llvm::ArrayType* arrayType = llvm::ArrayType::get(
            llvm::Type::getInt32Ty(context),
            order.size()
        );
        
        std::vector<llvm::Constant*> elements;
        for (uint32_t idx : order) {
            elements.push_back(llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(context), idx
            ));
        }
        
        llvm::Constant* arrayConst = llvm::ConstantArray::get(arrayType, elements);
        
        return new llvm::GlobalVariable(
            *module,
            arrayType,
            true,
            llvm::GlobalValue::PrivateLinkage,
            arrayConst,
            ".bidi.order"
        );
    }
    
    /**
     * إنشاء ثابت لمستويات التضمين | Create embedding levels constant
     */
    llvm::GlobalVariable* createEmbeddingLevelsConstant(const BidiAnalysis& analysis) {
        std::vector<uint8_t> levels;
        for (const auto& info : analysis.charInfo) {
            levels.push_back(info.embeddingLevel);
        }
        
        llvm::Constant* arrayConst = llvm::ConstantDataArray::get(
            context,
            llvm::ArrayRef<uint8_t>(levels)
        );
        
        return new llvm::GlobalVariable(
            *module,
            arrayConst->getType(),
            true,
            llvm::GlobalValue::PrivateLinkage,
            arrayConst,
            ".bidi.levels"
        );
    }
    
    /**
     * توليد استدعاء لإعادة ترتيب النص | Generate reorder call
     */
    llvm::Value* generateReorder(
        llvm::Value* textPtr,
        llvm::Value* orderPtr,
        llvm::Value* length
    ) {
        llvm::Function* reorderFunc = module->getFunction("sad_bidi_reorder");
        
        if (!reorderFunc) {
            llvm::Type* ptrType = llvm::PointerType::get(context, 0);
            llvm::Type* i64Type = llvm::Type::getInt64Ty(context);
            
            llvm::FunctionType* funcType = llvm::FunctionType::get(
                ptrType,
                {ptrType, ptrType, i64Type},
                false
            );
            
            reorderFunc = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                "sad_bidi_reorder",
                module
            );
        }
        
        return builder.CreateCall(
            reorderFunc,
            {textPtr, orderPtr, length},
            "reordered"
        );
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

// الكاش العام
static std::unique_ptr<BidiCache> globalBidiCache;

extern "C" {
    /**
     * تهيئة الكاش
     */
    void sad_bidi_cache_init(size_t maxSize) {
        globalBidiCache = std::make_unique<BidiCache>(maxSize);
    }
    
    /**
     * تحليل نص
     */
    int sad_bidi_analyze(const char* text) {
        if (!globalBidiCache) {
            globalBidiCache = std::make_unique<BidiCache>();
        }
        
        auto analysis = globalBidiCache->getOrAnalyze(text);
        return static_cast<int>(analysis->baseDirection);
    }
    
    /**
     * فحص سريع للاتجاه
     */
    int sad_bidi_quick_direction(const char* text) {
        if (!globalBidiCache) {
            globalBidiCache = std::make_unique<BidiCache>();
        }
        
        return static_cast<int>(globalBidiCache->quickDirection(text));
    }
    
    /**
     * إعادة ترتيب النص للعرض
     */
    char* sad_bidi_reorder(const char* text, const uint32_t* order, size_t length) {
        std::string result;
        
        // قراءة الأحرف
        std::vector<std::string> chars;
        size_t i = 0;
        while (i < std::strlen(text)) {
            size_t bytesRead;
            uint32_t cp = BidiAnalyzer::readUTF8(text + i, bytesRead);
            chars.push_back(std::string(text + i, bytesRead));
            i += bytesRead;
        }
        
        // إعادة الترتيب
        for (size_t j = 0; j < length && j < chars.size(); j++) {
            if (order[j] < chars.size()) {
                result += chars[order[j]];
            }
        }
        
        char* output = new char[result.size() + 1];
        std::strcpy(output, result.c_str());
        return output;
    }
    
    /**
     * مسح الكاش
     */
    void sad_bidi_cache_clear() {
        if (globalBidiCache) {
            globalBidiCache->clear();
        }
    }
    
    /**
     * نسبة الإصابة
     */
    double sad_bidi_cache_hit_rate() {
        if (globalBidiCache) {
            return globalBidiCache->hitRate();
        }
        return 0.0;
    }
}

} // namespace optimizer
} // namespace sad
