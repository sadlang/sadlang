/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: tashkeel_optimizer.cpp                                               ║
 * ║   الوصف: ضغط وتحسين التشكيل العربي                                         ║
 * ║   المهمة: T155 - Phase 14 (US12)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    ضغط التشكيل العربي لتوفير الذاكرة مع الحفاظ على القدرة على الاسترجاع
 *    Compress Arabic diacritics to save memory while preserving recoverability
 *
 * 📋 التشكيل العربي | Arabic Diacritics (Tashkeel):
 *
 *    ◆ فَتْحَة (Fathah)      U+064E  →  الحرف مع 'a'
 *    ◆ ضَمَّة (Dammah)       U+064F  →  الحرف مع 'u'
 *    ◆ كَسْرَة (Kasrah)      U+0650  →  الحرف مع 'i'
 *    ◆ سُكُون (Sukun)        U+0652  →  الحرف بدون حركة
 *    ◆ شَدَّة (Shaddah)      U+0651  →  تشديد الحرف
 *    ◆ تَنْوِين (Tanwin):
 *       - فتحتان U+064B → 'an'
 *       - ضمتان U+064C → 'un'
 *       - كسرتان U+064D → 'in'
 *
 * 🎯 استراتيجيات الضغط | Compression Strategies:
 *
 *    1️⃣ Run-Length Encoding للتشكيل المتكرر
 *    2️⃣ خريطة بتات للتشكيل (3 بتات لكل حرف)
 *    3️⃣ إزالة التشكيل مع خريطة استرجاع
 *    4️⃣ تشفير متغير الطول (Variable-length encoding)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/ADT/SmallVector.h>

#include <string>
#include <vector>
#include <bitset>
#include <optional>
#include <cstdint>
#include <cstring> // (AR) memcpy آمن بدلاً من strcpy / (EN) safe memcpy replaces strcpy
#include <unordered_map>

namespace sad {
namespace optimizer {

// ═══════════════════════════════════════════════════════════════════════════════
//                          تعريفات التشكيل | Tashkeel Definitions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 أنواع التشكيل | Tashkeel Types
 */
enum class TashkeelType : uint8_t {
    NONE      = 0,  // بدون تشكيل
    FATHA     = 1,  // فَتحة
    DAMMA     = 2,  // ضَمة
    KASRA     = 3,  // كَسرة
    SUKUN     = 4,  // سُكون
    SHADDA    = 5,  // شَدة
    FATHATAN  = 6,  // فتحتان (تنوين)
    DAMMATAN  = 7,  // ضمتان
    KASRATAN  = 8,  // كسرتان
    
    // مجموعات | Combinations
    SHADDA_FATHA    = 9,   // شدة + فتحة
    SHADDA_DAMMA    = 10,  // شدة + ضمة
    SHADDA_KASRA    = 11,  // شدة + كسرة
    SHADDA_FATHATAN = 12,  // شدة + فتحتان
    SHADDA_DAMMATAN = 13,  // شدة + ضمتان
    SHADDA_KASRATAN = 14,  // شدة + كسرتان
    
    SUPERSCRIPT_ALEF = 15  // ألف خنجرية
};

/**
 * 📋 Unicode codepoints للتشكيل
 */
namespace TashkeelCodepoints {
    constexpr uint32_t FATHA     = 0x064E;
    constexpr uint32_t DAMMA     = 0x064F;
    constexpr uint32_t KASRA     = 0x0650;
    constexpr uint32_t SHADDA    = 0x0651;
    constexpr uint32_t SUKUN     = 0x0652;
    constexpr uint32_t FATHATAN  = 0x064B;
    constexpr uint32_t DAMMATAN  = 0x064C;
    constexpr uint32_t KASRATAN  = 0x064D;
    constexpr uint32_t SUPERSCRIPT_ALEF = 0x0670;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                     تشكيل محرف واحد | Single Character Tashkeel
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 تشكيل حرف واحد | Character Tashkeel Info
 */
struct CharTashkeel {
    TashkeelType primary;    // التشكيل الأساسي
    TashkeelType secondary;  // تشكيل ثانوي (مثل شدة مع فتحة)
    bool hasSuperscriptAlef; // ألف خنجرية
    
    CharTashkeel()
        : primary(TashkeelType::NONE)
        , secondary(TashkeelType::NONE)
        , hasSuperscriptAlef(false)
    {}
    
    bool hasAny() const {
        return primary != TashkeelType::NONE || 
               secondary != TashkeelType::NONE ||
               hasSuperscriptAlef;
    }
    
    /**
     * ضغط إلى 4 بتات | Compress to 4 bits
     */
    uint8_t compress4Bit() const {
        // نستخدم 4 بتات لتمثيل أهم 16 حالة
        if (secondary == TashkeelType::SHADDA) {
            // شدة مع تشكيل آخر
            switch (primary) {
                case TashkeelType::FATHA:    return 9;
                case TashkeelType::DAMMA:    return 10;
                case TashkeelType::KASRA:    return 11;
                case TashkeelType::FATHATAN: return 12;
                case TashkeelType::DAMMATAN: return 13;
                case TashkeelType::KASRATAN: return 14;
                default: return 5;  // شدة فقط
            }
        }
        return static_cast<uint8_t>(primary);
    }
    
    /**
     * فك الضغط من 4 بتات | Decompress from 4 bits
     */
    static CharTashkeel decompress4Bit(uint8_t value) {
        CharTashkeel result;
        
        if (value >= 9 && value <= 14) {
            result.secondary = TashkeelType::SHADDA;
            switch (value) {
                case 9:  result.primary = TashkeelType::FATHA; break;
                case 10: result.primary = TashkeelType::DAMMA; break;
                case 11: result.primary = TashkeelType::KASRA; break;
                case 12: result.primary = TashkeelType::FATHATAN; break;
                case 13: result.primary = TashkeelType::DAMMATAN; break;
                case 14: result.primary = TashkeelType::KASRATAN; break;
            }
        } else if (value == 15) {
            result.hasSuperscriptAlef = true;
        } else {
            result.primary = static_cast<TashkeelType>(value);
        }
        
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     خريطة التشكيل المضغوطة | Compressed Tashkeel Map
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 CompressedTashkeelMap
 * 
 * خريطة مضغوطة للتشكيل - تخزن 2 حرف في كل بايت
 */
class CompressedTashkeelMap {
private:
    std::vector<uint8_t> data;
    size_t charCount;
    
public:
    CompressedTashkeelMap() : charCount(0) {}
    
    /**
     * إنشاء خريطة من قائمة تشكيل
     */
    void encode(const std::vector<CharTashkeel>& tashkeelList) {
        charCount = tashkeelList.size();
        
        // كل بايت يحتوي على تشكيل حرفين (4 بتات لكل حرف)
        size_t byteCount = (charCount + 1) / 2;
        data.resize(byteCount, 0);
        
        for (size_t i = 0; i < charCount; i++) {
            uint8_t compressed = tashkeelList[i].compress4Bit();
            size_t byteIndex = i / 2;
            
            if (i % 2 == 0) {
                // النصف العلوي
                data[byteIndex] |= (compressed << 4);
            } else {
                // النصف السفلي
                data[byteIndex] |= compressed;
            }
        }
    }
    
    /**
     * فك التشفير إلى قائمة تشكيل
     */
    std::vector<CharTashkeel> decode() const {
        std::vector<CharTashkeel> result;
        result.reserve(charCount);
        
        for (size_t i = 0; i < charCount; i++) {
            size_t byteIndex = i / 2;
            uint8_t compressed;
            
            if (i % 2 == 0) {
                compressed = (data[byteIndex] >> 4) & 0x0F;
            } else {
                compressed = data[byteIndex] & 0x0F;
            }
            
            result.push_back(CharTashkeel::decompress4Bit(compressed));
        }
        
        return result;
    }
    
    /**
     * الحصول على تشكيل حرف معين
     */
    CharTashkeel get(size_t index) const {
        if (index >= charCount) {
            return CharTashkeel();
        }
        
        size_t byteIndex = index / 2;
        uint8_t compressed;
        
        if (index % 2 == 0) {
            compressed = (data[byteIndex] >> 4) & 0x0F;
        } else {
            compressed = data[byteIndex] & 0x0F;
        }
        
        return CharTashkeel::decompress4Bit(compressed);
    }
    
    /**
     * حجم البيانات المضغوطة بالبايت
     */
    size_t byteSize() const { return data.size(); }
    
    /**
     * عدد الأحرف
     */
    size_t size() const { return charCount; }
    
    /**
     * نسبة الضغط
     */
    double compressionRatio() const {
        // بدون ضغط: حرف تشكيل = 3 بايت (UTF-8)
        // مع ضغط: 0.5 بايت لكل حرف
        if (charCount == 0) return 1.0;
        return (double)data.size() / (double)(charCount * 3);
    }
    
    /**
     * الحصول على البيانات الخام
     */
    const std::vector<uint8_t>& rawData() const { return data; }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        محلل التشكيل | Tashkeel Analyzer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 TashkeelAnalyzer
 * 
 * تحليل واستخراج التشكيل من النص العربي
 */
class TashkeelAnalyzer {
public:
    /**
     * التحقق إذا كان الـ codepoint تشكيلاً
     */
    static bool isTashkeel(uint32_t cp) {
        return (cp >= 0x064B && cp <= 0x0652) || 
               cp == 0x0670;
    }
    
    /**
     * تحويل codepoint إلى نوع تشكيل
     */
    static TashkeelType codepointToType(uint32_t cp) {
        switch (cp) {
            case TashkeelCodepoints::FATHA:    return TashkeelType::FATHA;
            case TashkeelCodepoints::DAMMA:    return TashkeelType::DAMMA;
            case TashkeelCodepoints::KASRA:    return TashkeelType::KASRA;
            case TashkeelCodepoints::SUKUN:    return TashkeelType::SUKUN;
            case TashkeelCodepoints::SHADDA:   return TashkeelType::SHADDA;
            case TashkeelCodepoints::FATHATAN: return TashkeelType::FATHATAN;
            case TashkeelCodepoints::DAMMATAN: return TashkeelType::DAMMATAN;
            case TashkeelCodepoints::KASRATAN: return TashkeelType::KASRATAN;
            case TashkeelCodepoints::SUPERSCRIPT_ALEF: return TashkeelType::SUPERSCRIPT_ALEF;
            default: return TashkeelType::NONE;
        }
    }
    
    /**
     * تحويل نوع تشكيل إلى codepoint
     */
    static uint32_t typeToCodepoint(TashkeelType type) {
        switch (type) {
            case TashkeelType::FATHA:    return TashkeelCodepoints::FATHA;
            case TashkeelType::DAMMA:    return TashkeelCodepoints::DAMMA;
            case TashkeelType::KASRA:    return TashkeelCodepoints::KASRA;
            case TashkeelType::SUKUN:    return TashkeelCodepoints::SUKUN;
            case TashkeelType::SHADDA:   return TashkeelCodepoints::SHADDA;
            case TashkeelType::FATHATAN: return TashkeelCodepoints::FATHATAN;
            case TashkeelType::DAMMATAN: return TashkeelCodepoints::DAMMATAN;
            case TashkeelType::KASRATAN: return TashkeelCodepoints::KASRATAN;
            case TashkeelType::SUPERSCRIPT_ALEF: return TashkeelCodepoints::SUPERSCRIPT_ALEF;
            default: return 0;
        }
    }
    
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
     * كتابة codepoint إلى UTF-8
     */
    static std::string writeUTF8(uint32_t cp) {
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
//                      محسّن التشكيل | Tashkeel Optimizer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 TashkeelOptimizer
 * 
 * المحسّن الرئيسي للتشكيل العربي
 * Main optimizer for Arabic diacritics
 */
class TashkeelOptimizer {
public:
    /**
     * 📋 نتيجة التحسين | Optimization Result
     */
    struct OptimizedString {
        std::string strippedText;        // النص بدون تشكيل
        CompressedTashkeelMap tashkeel;  // خريطة التشكيل المضغوطة
        size_t originalSize;             // الحجم الأصلي
        size_t optimizedSize;            // الحجم المحسّن
        
        double savings() const {
            if (originalSize == 0) return 0.0;
            return 1.0 - ((double)optimizedSize / (double)originalSize);
        }
    };
    
    /**
     * تحسين نص عربي | Optimize Arabic text
     * 
     * يفصل التشكيل عن الحروف ويضغط التشكيل
     */
    static OptimizedString optimize(const std::string& text) {
        OptimizedString result;
        result.originalSize = text.size();
        
        std::string stripped;
        std::vector<CharTashkeel> tashkeelList;
        
        CharTashkeel currentTashkeel;
        bool hasBaseChar = false;
        
        size_t i = 0;
        while (i < text.size()) {
            size_t bytesRead;
            uint32_t cp = TashkeelAnalyzer::readUTF8(text.c_str() + i, bytesRead);
            
            if (TashkeelAnalyzer::isTashkeel(cp)) {
                // تشكيل
                TashkeelType type = TashkeelAnalyzer::codepointToType(cp);
                
                if (type == TashkeelType::SHADDA) {
                    currentTashkeel.secondary = TashkeelType::SHADDA;
                } else if (type == TashkeelType::SUPERSCRIPT_ALEF) {
                    currentTashkeel.hasSuperscriptAlef = true;
                } else if (currentTashkeel.primary == TashkeelType::NONE) {
                    currentTashkeel.primary = type;
                }
            } else {
                // حرف عادي
                if (hasBaseChar) {
                    tashkeelList.push_back(currentTashkeel);
                }
                
                stripped += TashkeelAnalyzer::writeUTF8(cp);
                currentTashkeel = CharTashkeel();
                hasBaseChar = true;
            }
            
            i += bytesRead;
        }
        
        // إضافة تشكيل آخر حرف
        if (hasBaseChar) {
            tashkeelList.push_back(currentTashkeel);
        }
        
        result.strippedText = stripped;
        result.tashkeel.encode(tashkeelList);
        result.optimizedSize = stripped.size() + result.tashkeel.byteSize();
        
        return result;
    }
    
    /**
     * استرجاع النص الأصلي | Restore original text
     */
    static std::string restore(const OptimizedString& optimized) {
        std::string result;
        
        auto tashkeelList = optimized.tashkeel.decode();
        
        size_t charIndex = 0;
        size_t i = 0;
        
        while (i < optimized.strippedText.size()) {
            size_t bytesRead;
            uint32_t cp = TashkeelAnalyzer::readUTF8(
                optimized.strippedText.c_str() + i, bytesRead
            );
            
            // كتابة الحرف
            result += TashkeelAnalyzer::writeUTF8(cp);
            
            // إضافة التشكيل
            if (charIndex < tashkeelList.size()) {
                const auto& tashkeel = tashkeelList[charIndex];
                
                // الشدة أولاً إذا موجودة
                if (tashkeel.secondary == TashkeelType::SHADDA) {
                    result += TashkeelAnalyzer::writeUTF8(TashkeelCodepoints::SHADDA);
                }
                
                // التشكيل الأساسي
                if (tashkeel.primary != TashkeelType::NONE) {
                    uint32_t tashkeelCp = TashkeelAnalyzer::typeToCodepoint(tashkeel.primary);
                    if (tashkeelCp != 0) {
                        result += TashkeelAnalyzer::writeUTF8(tashkeelCp);
                    }
                }
                
                // الألف الخنجرية
                if (tashkeel.hasSuperscriptAlef) {
                    result += TashkeelAnalyzer::writeUTF8(TashkeelCodepoints::SUPERSCRIPT_ALEF);
                }
            }
            
            charIndex++;
            i += bytesRead;
        }
        
        return result;
    }
    
    /**
     * إزالة التشكيل فقط | Strip tashkeel only
     */
    static std::string stripTashkeel(const std::string& text) {
        std::string result;
        
        size_t i = 0;
        while (i < text.size()) {
            size_t bytesRead;
            uint32_t cp = TashkeelAnalyzer::readUTF8(text.c_str() + i, bytesRead);
            
            if (!TashkeelAnalyzer::isTashkeel(cp)) {
                result += TashkeelAnalyzer::writeUTF8(cp);
            }
            
            i += bytesRead;
        }
        
        return result;
    }
    
    /**
     * حساب عدد حروف التشكيل | Count tashkeel characters
     */
    static size_t countTashkeel(const std::string& text) {
        size_t count = 0;
        
        size_t i = 0;
        while (i < text.size()) {
            size_t bytesRead;
            uint32_t cp = TashkeelAnalyzer::readUTF8(text.c_str() + i, bytesRead);
            
            if (TashkeelAnalyzer::isTashkeel(cp)) {
                count++;
            }
            
            i += bytesRead;
        }
        
        return count;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    مولد LLVM للتشكيل | LLVM Tashkeel Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 TashkeelCodegen
 * 
 * توليد كود LLVM للتعامل مع التشكيل المحسّن
 */
class TashkeelCodegen {
private:
    llvm::LLVMContext& context;
    llvm::Module* module;
    llvm::IRBuilder<>& builder;
    
public:
    TashkeelCodegen(llvm::LLVMContext& ctx, llvm::Module* mod, llvm::IRBuilder<>& b)
        : context(ctx)
        , module(mod)
        , builder(b)
    {}
    
    /**
     * إنشاء ثابت للنص المحسّن | Create constant for optimized string
     */
    std::pair<llvm::GlobalVariable*, llvm::GlobalVariable*> createOptimizedString(
        const std::string& text
    ) {
        auto optimized = TashkeelOptimizer::optimize(text);
        
        // النص المجرد
        llvm::Constant* strippedConst = llvm::ConstantDataArray::getString(
            context, optimized.strippedText, true
        );
        
        auto* strippedVar = new llvm::GlobalVariable(
            *module,
            strippedConst->getType(),
            true,
            llvm::GlobalValue::PrivateLinkage,
            strippedConst,
            ".str.stripped"
        );
        
        // خريطة التشكيل
        auto& tashkeelData = optimized.tashkeel.rawData();
        llvm::Constant* tashkeelConst = llvm::ConstantDataArray::get(
            context,
            llvm::ArrayRef<uint8_t>(tashkeelData.data(), tashkeelData.size())
        );
        
        auto* tashkeelVar = new llvm::GlobalVariable(
            *module,
            tashkeelConst->getType(),
            true,
            llvm::GlobalValue::PrivateLinkage,
            tashkeelConst,
            ".tashkeel.map"
        );
        
        return {strippedVar, tashkeelVar};
    }
    
    /**
     * توليد استدعاء لاسترجاع النص الأصلي
     */
    llvm::Value* generateRestore(
        llvm::Value* strippedPtr,
        llvm::Value* tashkeelPtr,
        llvm::Value* tashkeelSize
    ) {
        // الحصول على دالة الاسترجاع أو إنشائها
        llvm::Function* restoreFunc = module->getFunction("sad_tashkeel_restore");
        
        if (!restoreFunc) {
            llvm::Type* ptrType = llvm::PointerType::get(context, 0);
            llvm::Type* i64Type = llvm::Type::getInt64Ty(context);
            
            llvm::FunctionType* funcType = llvm::FunctionType::get(
                ptrType,
                {ptrType, ptrType, i64Type},
                false
            );
            
            restoreFunc = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                "sad_tashkeel_restore",
                module
            );
        }
        
        return builder.CreateCall(
            restoreFunc,
            {strippedPtr, tashkeelPtr, tashkeelSize},
            "restored_str"
        );
    }
    
    /**
     * توليد دالة المقارنة بدون تشكيل
     */
    llvm::Value* generateCompareIgnoreTashkeel(
        llvm::Value* str1,
        llvm::Value* str2
    ) {
        llvm::Function* compareFunc = module->getFunction("sad_string_compare_no_tashkeel");
        
        if (!compareFunc) {
            llvm::Type* ptrType = llvm::PointerType::get(context, 0);
            llvm::Type* i32Type = llvm::Type::getInt32Ty(context);
            
            llvm::FunctionType* funcType = llvm::FunctionType::get(
                i32Type,
                {ptrType, ptrType},
                false
            );
            
            compareFunc = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                "sad_string_compare_no_tashkeel",
                module
            );
        }
        
        return builder.CreateCall(compareFunc, {str1, str2}, "cmp_result");
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * واجهة C للتكامل مع Runtime
 */
extern "C" {
    /**
     * إزالة التشكيل من نص
     */
    char* sad_strip_tashkeel(const char* text) {
        std::string result = TashkeelOptimizer::stripTashkeel(text);
        const std::size_t bufSize = result.size() + 1;
        char* output = new char[bufSize];
        // (AR) memcpy آمن: الوجهة بحجم دقيق / (EN) safe memcpy: dest sized exactly
        std::memcpy(output, result.c_str(), bufSize);
        return output;
    }
    
    /**
     * عدد حروف التشكيل
     */
    size_t sad_count_tashkeel(const char* text) {
        return TashkeelOptimizer::countTashkeel(text);
    }
    
    /**
     * استرجاع نص مع تشكيل من خريطة مضغوطة
     */
    char* sad_tashkeel_restore(
        const char* stripped,
        const uint8_t* tashkeelMap,
        size_t mapSize
    ) {
        // إعادة بناء OptimizedString
        TashkeelOptimizer::OptimizedString optimized;
        optimized.strippedText = stripped;
        
        // فك ترميز الخريطة
        std::vector<uint8_t> mapData(tashkeelMap, tashkeelMap + mapSize);
        // ... reconstruct CompressedTashkeelMap
        
        std::string result = TashkeelOptimizer::restore(optimized);
        const std::size_t bufSize = result.size() + 1;
        char* output = new char[bufSize];
        std::memcpy(output, result.c_str(), bufSize);
        return output;
    }
    
    /**
     * مقارنة نصين بتجاهل التشكيل
     */
    int sad_string_compare_no_tashkeel(const char* str1, const char* str2) {
        std::string s1 = TashkeelOptimizer::stripTashkeel(str1);
        std::string s2 = TashkeelOptimizer::stripTashkeel(str2);
        return s1.compare(s2);
    }
    
    /**
     * تحرير الذاكرة
     */
    void sad_free_string(char* str) {
        delete[] str;
    }
}

} // namespace optimizer
} // namespace sad
