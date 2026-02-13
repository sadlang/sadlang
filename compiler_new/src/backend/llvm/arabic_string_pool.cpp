/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: arabic_string_pool.cpp                                               ║
 * ║   الوصف: تجميع النصوص العربية في مخزن واحد                                  ║
 * ║   المهمة: T154 - Phase 14 (US12)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    تحسين استخدام الذاكرة بتجميع النصوص العربية المتكررة
 *    Optimize memory usage by pooling repeated Arabic strings
 *
 * 📋 الميزات | Features:
 *
 *    1️⃣ تجميع السلاسل النصية | String Interning:
 *       "السلام عليكم" × 3 → مرجع واحد في الذاكرة
 *
 *    2️⃣ تطبيع Unicode قبل التجميع | Unicode Normalization:
 *       تأكد من أن النصوص المتساوية تُخزّن مرة واحدة
 *
 *    3️⃣ تحسين للنصوص الشائعة | Common String Optimization:
 *       كلمات مثل "دالة"، "دع"، "إذا" مخزنة مسبقاً
 *
 *    4️⃣ دعم UTF-8 و UTF-16 | Multi-encoding Support:
 *       تجميع للتشفيرات المختلفة
 *
 * 🎯 الاختبار المستقل | Independent Test:
 *    "السلام عليكم" × 3 تستخدم مرجع واحد في الذاكرة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Constants.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/Support/raw_ostream.h>

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>
#include <optional>
#include <cstring>

namespace sad {
namespace optimizer {

// ═══════════════════════════════════════════════════════════════════════════════
//                          تعريفات الأنواع | Type Definitions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 نوع ترميز النص | String Encoding Type
 */
enum class StringEncoding {
    UTF8,       // الترميز الافتراضي
    UTF16_LE,   // Windows
    UTF16_BE,   // Network byte order
    UTF32       // للأحرف المركبة
};

/**
 * 📋 معلومات النص المُجمَّع | Pooled String Info
 */
struct PooledStringInfo {
    std::string normalized;          // النص المُطبّع
    llvm::GlobalVariable* globalVar; // المتغير العام
    size_t useCount;                 // عدد الاستخدامات
    StringEncoding encoding;         // الترميز
    bool hasArabic;                  // يحتوي على عربي
    bool hasTashkeel;                // يحتوي على تشكيل
    size_t byteSize;                 // الحجم بالبايت
    size_t charCount;                // عدد الأحرف
    
    PooledStringInfo()
        : globalVar(nullptr)
        , useCount(0)
        , encoding(StringEncoding::UTF8)
        , hasArabic(false)
        , hasTashkeel(false)
        , byteSize(0)
        , charCount(0)
    {}
};

/**
 * 📋 إحصائيات المخزن | Pool Statistics
 */
struct PoolStatistics {
    size_t totalStrings;            // إجمالي النصوص
    size_t uniqueStrings;           // النصوص الفريدة
    size_t totalBytes;              // إجمالي البايتات
    size_t savedBytes;              // البايتات الموفرة
    size_t arabicStrings;           // النصوص العربية
    size_t tashkeelStrings;         // نصوص بتشكيل
    size_t commonHits;              // استخدامات الكلمات الشائعة
    
    PoolStatistics()
        : totalStrings(0)
        , uniqueStrings(0)
        , totalBytes(0)
        , savedBytes(0)
        , arabicStrings(0)
        , tashkeelStrings(0)
        , commonHits(0)
    {}
    
    double savingsPercent() const {
        if (totalBytes == 0) return 0.0;
        return (double)savedBytes / (double)(totalBytes + savedBytes) * 100.0;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     أدوات Unicode | Unicode Utilities
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 UnicodeAnalyzer
 * 
 * تحليل وتطبيع النصوص العربية
 */
class UnicodeAnalyzer {
public:
    /**
     * التحقق من أن الحرف عربي | Check if character is Arabic
     */
    static bool isArabicChar(uint32_t codepoint) {
        // Arabic block: U+0600 - U+06FF
        // Arabic Supplement: U+0750 - U+077F
        // Arabic Extended-A: U+08A0 - U+08FF
        // Arabic Presentation Forms-A: U+FB50 - U+FDFF
        // Arabic Presentation Forms-B: U+FE70 - U+FEFF
        
        return (codepoint >= 0x0600 && codepoint <= 0x06FF) ||
               (codepoint >= 0x0750 && codepoint <= 0x077F) ||
               (codepoint >= 0x08A0 && codepoint <= 0x08FF) ||
               (codepoint >= 0xFB50 && codepoint <= 0xFDFF) ||
               (codepoint >= 0xFE70 && codepoint <= 0xFEFF);
    }
    
    /**
     * التحقق من التشكيل | Check if diacritical mark (tashkeel)
     */
    static bool isTashkeel(uint32_t codepoint) {
        // Arabic Fathah, Dammah, Kasrah, etc.
        // U+064B - U+065F: Arabic diacritics
        // U+0670: Arabic superscript alef
        // U+06D6 - U+06ED: Arabic small letters and other marks
        
        return (codepoint >= 0x064B && codepoint <= 0x065F) ||
               (codepoint == 0x0670) ||
               (codepoint >= 0x06D6 && codepoint <= 0x06ED);
    }
    
    /**
     * قراءة codepoint من UTF-8 | Read codepoint from UTF-8
     */
    static uint32_t readUTF8Codepoint(const char* str, size_t& bytesRead) {
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
        return 0xFFFD; // Replacement character
    }
    
    /**
     * تحليل النص | Analyze string
     */
    static void analyzeString(
        const std::string& str,
        bool& hasArabic,
        bool& hasTashkeel,
        size_t& charCount
    ) {
        hasArabic = false;
        hasTashkeel = false;
        charCount = 0;
        
        size_t i = 0;
        while (i < str.size()) {
            size_t bytesRead;
            uint32_t cp = readUTF8Codepoint(str.c_str() + i, bytesRead);
            
            if (isArabicChar(cp)) {
                hasArabic = true;
            }
            if (isTashkeel(cp)) {
                hasTashkeel = true;
            }
            
            charCount++;
            i += bytesRead;
        }
    }
    
    /**
     * تطبيع NFC مبسط | Simple NFC normalization
     * 
     * ملاحظة: في الإنتاج نستخدم ICU أو مكتبة Unicode كاملة
     */
    static std::string normalizeNFC(const std::string& str) {
        // تنفيذ مبسط - في الإنتاج نستخدم ICU
        // هنا نقوم بتطبيع بسيط للأحرف العربية
        
        std::string result;
        result.reserve(str.size());
        
        size_t i = 0;
        while (i < str.size()) {
            size_t bytesRead;
            uint32_t cp = readUTF8Codepoint(str.c_str() + i, bytesRead);
            
            // تحويل أشكال العرض إلى الأشكال الأساسية
            cp = normalizeArabicPresentation(cp);
            
            // كتابة النتيجة
            result += codepointToUTF8(cp);
            i += bytesRead;
        }
        
        return result;
    }
    
    /**
     * تطبيع أشكال العرض العربية
     */
    static uint32_t normalizeArabicPresentation(uint32_t cp) {
        // تحويل أشكال العرض (Presentation Forms) إلى الأشكال الأساسية
        // هذا تنفيذ مبسط للأحرف الشائعة
        
        // Presentation Forms-B to basic Arabic
        if (cp >= 0xFE70 && cp <= 0xFEFF) {
            // جدول تحويل مبسط
            static const struct { uint32_t from; uint32_t to; } conversions[] = {
                {0xFE80, 0x0621},  // Hamza
                {0xFE81, 0x0622}, {0xFE82, 0x0622},  // Alef with madda
                {0xFE83, 0x0623}, {0xFE84, 0x0623},  // Alef with hamza above
                {0xFE85, 0x0624}, {0xFE86, 0x0624},  // Waw with hamza
                {0xFE87, 0x0625}, {0xFE88, 0x0625},  // Alef with hamza below
                {0xFE89, 0x0626}, {0xFE8A, 0x0626}, {0xFE8B, 0x0626}, {0xFE8C, 0x0626},  // Yeh with hamza
                {0xFE8D, 0x0627}, {0xFE8E, 0x0627},  // Alef
                {0xFE8F, 0x0628}, {0xFE90, 0x0628}, {0xFE91, 0x0628}, {0xFE92, 0x0628},  // Beh
                // ... المزيد من التحويلات
            };
            
            for (const auto& conv : conversions) {
                if (cp == conv.from) return conv.to;
            }
        }
        
        return cp;
    }
    
    /**
     * تحويل codepoint إلى UTF-8
     */
    static std::string codepointToUTF8(uint32_t cp) {
        std::string result;
        
        if (cp < 0x80) {
            result += static_cast<char>(cp);
        } else if (cp < 0x800) {
            result += static_cast<char>(0xC0 | (cp >> 6));
            result += static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            result += static_cast<char>(0xE0 | (cp >> 12));
            result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            result += static_cast<char>(0xF0 | (cp >> 18));
            result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (cp & 0x3F));
        }
        
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     مخزن النصوص العربية | Arabic String Pool
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 ArabicStringPool
 * 
 * مخزن للنصوص العربية مع تجميع وتحسين
 * Pool for Arabic strings with interning and optimization
 */
class ArabicStringPool {
private:
    llvm::LLVMContext& context;
    llvm::Module* module;
    
    // المخزن الرئيسي | Main pool
    std::unordered_map<std::string, PooledStringInfo> pool;
    
    // الكلمات الشائعة المحملة مسبقاً | Pre-loaded common words
    std::unordered_map<std::string, llvm::GlobalVariable*> commonStrings;
    
    // إحصائيات | Statistics
    PoolStatistics stats;
    
    // للحماية في multi-threading
    mutable std::mutex poolMutex;
    
    // عداد للأسماء الفريدة
    int stringCounter;
    
public:
    ArabicStringPool(llvm::LLVMContext& ctx, llvm::Module* mod)
        : context(ctx)
        , module(mod)
        , stringCounter(0)
    {
        initializeCommonStrings();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    التجميع الرئيسي | Main Pooling
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على نص من المخزن أو إنشائه | Get or create pooled string
     */
    llvm::GlobalVariable* getOrCreateString(const std::string& str) {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        // تطبيع النص
        std::string normalized = UnicodeAnalyzer::normalizeNFC(str);
        
        // البحث في المخزن
        auto it = pool.find(normalized);
        if (it != pool.end()) {
            // موجود - زيادة العداد
            it->second.useCount++;
            stats.savedBytes += it->second.byteSize;
            return it->second.globalVar;
        }
        
        // التحقق من الكلمات الشائعة
        auto commonIt = commonStrings.find(normalized);
        if (commonIt != commonStrings.end()) {
            stats.commonHits++;
            return commonIt->second;
        }
        
        // إنشاء جديد
        return createPooledString(normalized, str);
    }
    
    /**
     * إنشاء نص جديد في المخزن
     */
    llvm::GlobalVariable* createPooledString(
        const std::string& normalized,
        const std::string& original
    ) {
        // تحليل النص
        bool hasArabic, hasTashkeel;
        size_t charCount;
        UnicodeAnalyzer::analyzeString(normalized, hasArabic, hasTashkeel, charCount);
        
        // إنشاء الثابت
        llvm::Constant* strConstant = llvm::ConstantDataArray::getString(
            context, normalized, true /* null-terminated */
        );
        
        // إنشاء المتغير العام
        std::string varName = generateStringName(hasArabic);
        
        llvm::GlobalVariable* globalVar = new llvm::GlobalVariable(
            *module,
            strConstant->getType(),
            true,  // isConstant
            llvm::GlobalValue::PrivateLinkage,
            strConstant,
            varName
        );
        
        // تعيين محاذاة مناسبة
        globalVar->setAlignment(llvm::Align(8));
        
        // تخزين المعلومات
        PooledStringInfo info;
        info.normalized = normalized;
        info.globalVar = globalVar;
        info.useCount = 1;
        info.encoding = StringEncoding::UTF8;
        info.hasArabic = hasArabic;
        info.hasTashkeel = hasTashkeel;
        info.byteSize = normalized.size() + 1;  // +1 for null terminator
        info.charCount = charCount;
        
        pool[normalized] = info;
        
        // تحديث الإحصائيات
        stats.totalStrings++;
        stats.uniqueStrings++;
        stats.totalBytes += info.byteSize;
        if (hasArabic) stats.arabicStrings++;
        if (hasTashkeel) stats.tashkeelStrings++;
        
        return globalVar;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    الكلمات الشائعة | Common Words
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تهيئة الكلمات العربية الشائعة | Initialize common Arabic words
     */
    void initializeCommonStrings() {
        // الكلمات المفتاحية الشائعة في لغة سعد
        static const char* commonArabicStrings[] = {
            // كلمات مفتاحية | Keywords
            "دالة",           // function
            "دع",            // let
            "متغير",          // var/mutable
            "إذا",           // if
            "وإلا",          // else
            "طالما",          // while
            "لكل",           // for
            "في",            // in
            "أرجع",          // return
            "صحيح",          // true
            "خطأ",           // false
            "فارغ",          // null/none
            "بعض",           // some
            "نتيجة",          // result
            "نجاح",          // success
            "فشل",           // failure
            
            // أنواع شائعة | Common types
            "عدد_صحيح",       // int
            "عدد_عشري",       // float
            "نص",            // string
            "منطقي",          // bool
            "قائمة",          // list
            "خريطة",          // map
            "مجموعة",         // set
            
            // رسائل شائعة | Common messages
            "السلام عليكم",    // Peace be upon you (most common greeting)
            "مرحباً",         // Hello
            "خطأ",           // Error
            "تحذير",          // Warning
            "نجاح",          // Success
            
            // عمليات | Operations
            "حوّل",          // map
            "صفّي",          // filter
            "اجمع",          // collect
            "طيّ",           // fold
            
            nullptr
        };
        
        for (int i = 0; commonArabicStrings[i] != nullptr; i++) {
            std::string str = commonArabicStrings[i];
            std::string normalized = UnicodeAnalyzer::normalizeNFC(str);
            
            llvm::Constant* constant = llvm::ConstantDataArray::getString(
                context, normalized, true
            );
            
            std::string varName = ".str.common." + std::to_string(i);
            
            llvm::GlobalVariable* globalVar = new llvm::GlobalVariable(
                *module,
                constant->getType(),
                true,
                llvm::GlobalValue::PrivateLinkage,
                constant,
                varName
            );
            
            globalVar->setAlignment(llvm::Align(8));
            commonStrings[normalized] = globalVar;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    البحث والاستعلام | Lookup and Query
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * البحث عن نص في المخزن | Lookup string in pool
     */
    std::optional<llvm::GlobalVariable*> lookup(const std::string& str) const {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        std::string normalized = UnicodeAnalyzer::normalizeNFC(str);
        
        auto it = pool.find(normalized);
        if (it != pool.end()) {
            return it->second.globalVar;
        }
        
        auto commonIt = commonStrings.find(normalized);
        if (commonIt != commonStrings.end()) {
            return commonIt->second;
        }
        
        return std::nullopt;
    }
    
    /**
     * التحقق من وجود النص | Check if string exists
     */
    bool contains(const std::string& str) const {
        return lookup(str).has_value();
    }
    
    /**
     * الحصول على معلومات النص | Get string info
     */
    std::optional<PooledStringInfo> getInfo(const std::string& str) const {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        std::string normalized = UnicodeAnalyzer::normalizeNFC(str);
        
        auto it = pool.find(normalized);
        if (it != pool.end()) {
            return it->second;
        }
        
        return std::nullopt;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    الإحصائيات والتقارير | Statistics and Reports
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الإحصائيات | Get statistics
     */
    PoolStatistics getStatistics() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return stats;
    }
    
    /**
     * طباعة تقرير | Print report
     */
    void printReport(llvm::raw_ostream& os) const {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        os << "\n";
        os << "╔════════════════════════════════════════════════════════════╗\n";
        os << "║              تقرير مخزن النصوص العربية                      ║\n";
        os << "║              Arabic String Pool Report                     ║\n";
        os << "╠════════════════════════════════════════════════════════════╣\n";
        os << "║                                                            ║\n";
        os << "║  إجمالي النصوص | Total Strings:     " << stats.totalStrings << "\n";
        os << "║  نصوص فريدة | Unique Strings:       " << stats.uniqueStrings << "\n";
        os << "║  نصوص عربية | Arabic Strings:       " << stats.arabicStrings << "\n";
        os << "║  نصوص بتشكيل | With Tashkeel:       " << stats.tashkeelStrings << "\n";
        os << "║                                                            ║\n";
        os << "║  إجمالي البايتات | Total Bytes:     " << stats.totalBytes << "\n";
        os << "║  بايتات موفرة | Saved Bytes:        " << stats.savedBytes << "\n";
        os << "║  نسبة التوفير | Savings:            " << stats.savingsPercent() << "%\n";
        os << "║                                                            ║\n";
        os << "║  استخدام الشائعة | Common Hits:     " << stats.commonHits << "\n";
        os << "║                                                            ║\n";
        os << "╚════════════════════════════════════════════════════════════╝\n";
    }
    
    /**
     * الحصول على النصوص الأكثر استخداماً
     */
    std::vector<std::pair<std::string, size_t>> getMostUsedStrings(size_t count) const {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        std::vector<std::pair<std::string, size_t>> result;
        
        for (const auto& entry : pool) {
            result.emplace_back(entry.first, entry.second.useCount);
        }
        
        // ترتيب تنازلي
        std::sort(result.begin(), result.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });
        
        if (result.size() > count) {
            result.resize(count);
        }
        
        return result;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    التنظيف والصيانة | Cleanup and Maintenance
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تنظيف النصوص غير المستخدمة | Cleanup unused strings
     * 
     * ملاحظة: يجب استدعاؤها بحذر - فقط بعد انتهاء التوليد
     */
    void cleanup() {
        std::lock_guard<std::mutex> lock(poolMutex);
        
        // إزالة النصوص التي استُخدمت مرة واحدة فقط وحجمها صغير
        const size_t minUsefulSize = 16;  // نصوص أصغر من هذا لا تستحق التجميع
        
        std::vector<std::string> toRemove;
        
        for (const auto& entry : pool) {
            if (entry.second.useCount == 1 && 
                entry.second.byteSize < minUsefulSize &&
                !entry.second.hasArabic) {
                toRemove.push_back(entry.first);
            }
        }
        
        for (const auto& key : toRemove) {
            // لا نحذف المتغير العام لأنه قد يكون مستخدماً
            // فقط نزيله من المخزن
            pool.erase(key);
        }
    }
    
    /**
     * مسح المخزن | Clear pool
     */
    void clear() {
        std::lock_guard<std::mutex> lock(poolMutex);
        pool.clear();
        stats = PoolStatistics();
    }
    
    /**
     * الحجم | Size
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(poolMutex);
        return pool.size();
    }
    
private:
    /**
     * توليد اسم للنص | Generate string name
     */
    std::string generateStringName(bool hasArabic) {
        if (hasArabic) {
            return ".str.ar." + std::to_string(stringCounter++);
        }
        return ".str." + std::to_string(stringCounter++);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    مدير تجميع النصوص | String Pool Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 StringPoolManager
 * 
 * مدير عام للتكامل مع المترجم
 */
class StringPoolManager {
private:
    static std::unique_ptr<ArabicStringPool> globalPool;
    
public:
    /**
     * تهيئة المدير | Initialize manager
     */
    static void initialize(llvm::LLVMContext& ctx, llvm::Module* mod) {
        globalPool = std::make_unique<ArabicStringPool>(ctx, mod);
    }
    
    /**
     * الحصول على المخزن | Get pool
     */
    static ArabicStringPool* getPool() {
        return globalPool.get();
    }
    
    /**
     * تجميع نص | Pool string
     */
    static llvm::GlobalVariable* poolString(const std::string& str) {
        if (!globalPool) return nullptr;
        return globalPool->getOrCreateString(str);
    }
    
    /**
     * إنهاء وطباعة التقرير | Finalize and print report
     */
    static void finalize(llvm::raw_ostream& os) {
        if (globalPool) {
            globalPool->printReport(os);
        }
    }
    
    /**
     * تنظيف | Cleanup
     */
    static void cleanup() {
        globalPool.reset();
    }
};

// تعريف المتغير الثابت
std::unique_ptr<ArabicStringPool> StringPoolManager::globalPool = nullptr;

// ═══════════════════════════════════════════════════════════════════════════════
//                         واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * واجهة C للتكامل | C Interface for integration
 */
extern "C" {
    void* sad_string_pool_create(void* llvm_context, void* module) {
        auto* ctx = static_cast<llvm::LLVMContext*>(llvm_context);
        auto* mod = static_cast<llvm::Module*>(module);
        return new ArabicStringPool(*ctx, mod);
    }
    
    void sad_string_pool_destroy(void* pool) {
        delete static_cast<ArabicStringPool*>(pool);
    }
    
    void* sad_string_pool_get(void* pool, const char* str) {
        auto* p = static_cast<ArabicStringPool*>(pool);
        return p->getOrCreateString(str);
    }
    
    int sad_string_pool_contains(void* pool, const char* str) {
        auto* p = static_cast<ArabicStringPool*>(pool);
        return p->contains(str) ? 1 : 0;
    }
    
    size_t sad_string_pool_size(void* pool) {
        auto* p = static_cast<ArabicStringPool*>(pool);
        return p->size();
    }
}

} // namespace optimizer
} // namespace sad
