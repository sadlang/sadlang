/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: arabic_normalization.cpp                                             ║
 * ║   الوصف: تطبيع Unicode NFC للنصوص العربية                                   ║
 * ║   المهمة: T158 - Phase 14 (US12)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    تطبيع النصوص العربية إلى صيغة NFC الموحدة
 *    Normalize Arabic texts to canonical NFC form
 *
 * 📋 أنواع التطبيع | Normalization Forms:
 *
 *    ◆ NFC (Canonical Decomposition, then Canonical Composition):
 *      الصيغة الأكثر استخداماً - يدمج الحروف المركبة
 *
 *    ◆ NFD (Canonical Decomposition):
 *      يفكك الحروف المركبة إلى أجزاء
 *
 *    ◆ NFKC (Compatibility Decomposition, then Canonical Composition):
 *      يحول الحروف المتوافقة ويدمج
 *
 *    ◆ NFKD (Compatibility Decomposition):
 *      يحول الحروف المتوافقة ويفكك
 *
 * 🎯 أمثلة التطبيع العربي | Arabic Normalization Examples:
 *
 *    ◆ أشكال العرض → الحروف الأساسية:
 *      ﻡ (U+FEE1) → م (U+0645)
 *
 *    ◆ لام-ألف → حرفين منفصلين:
 *      ﻻ (U+FEFB) → ل + ا
 *
 *    ◆ ترتيب التشكيل:
 *      حرف + شدة + فتحة (الترتيب القانوني)
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/ADT/StringMap.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <cstring> // (AR) memcpy آمن بدلاً من strcpy / (EN) safe memcpy replaces strcpy

namespace sad {
namespace optimizer {

// ═══════════════════════════════════════════════════════════════════════════════
//                          تعريفات | Definitions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 نوع التطبيع | Normalization Form
 */
enum class NormalizationForm {
    NFC,   // الصيغة المركبة القانونية (الافتراضية)
    NFD,   // الصيغة المفككة القانونية
    NFKC,  // الصيغة المركبة المتوافقة
    NFKD   // الصيغة المفككة المتوافقة
};

/**
 * 📋 فئة الدمج | Combining Class
 * 
 * ترتيب علامات التشكيل حسب معيار Unicode
 */
namespace CombiningClass {
    constexpr uint8_t NOT_REORDERED = 0;
    constexpr uint8_t OVERLAY = 1;
    constexpr uint8_t NUKTA = 7;
    constexpr uint8_t KANA_VOICING = 8;
    constexpr uint8_t VIRAMA = 9;
    
    // Arabic combining classes
    constexpr uint8_t ARABIC_FATHATAN = 27;
    constexpr uint8_t ARABIC_DAMMATAN = 28;
    constexpr uint8_t ARABIC_KASRATAN = 29;
    constexpr uint8_t ARABIC_FATHA = 30;
    constexpr uint8_t ARABIC_DAMMA = 31;
    constexpr uint8_t ARABIC_KASRA = 32;
    constexpr uint8_t ARABIC_SHADDA = 33;
    constexpr uint8_t ARABIC_SUKUN = 34;
    constexpr uint8_t ARABIC_SUPERSCRIPT_ALEF = 35;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                     جداول التحويل | Conversion Tables
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 ArabicNormalizationTables
 * 
 * جداول تحويل أشكال العرض العربية إلى الأشكال الأساسية
 */
class ArabicNormalizationTables {
public:
    /**
     * تحويل أشكال العرض-B إلى الحروف الأساسية
     * Presentation Forms-B (U+FE70-U+FEFF) to basic Arabic
     */
    static std::vector<uint32_t> decomposePresBForm(uint32_t cp) {
        // جدول التحويل للأحرف الشائعة
        static const std::unordered_map<uint32_t, std::vector<uint32_t>> table = {
            // Isolated forms
            {0xFE80, {0x0621}},  // ء Hamza
            {0xFE81, {0x0622}}, {0xFE82, {0x0622}},  // آ Alef Madda
            {0xFE83, {0x0623}}, {0xFE84, {0x0623}},  // أ Alef Hamza Above
            {0xFE85, {0x0624}}, {0xFE86, {0x0624}},  // ؤ Waw Hamza
            {0xFE87, {0x0625}}, {0xFE88, {0x0625}},  // إ Alef Hamza Below
            {0xFE89, {0x0626}}, {0xFE8A, {0x0626}}, {0xFE8B, {0x0626}}, {0xFE8C, {0x0626}},  // ئ Yeh Hamza
            {0xFE8D, {0x0627}}, {0xFE8E, {0x0627}},  // ا Alef
            {0xFE8F, {0x0628}}, {0xFE90, {0x0628}}, {0xFE91, {0x0628}}, {0xFE92, {0x0628}},  // ب Beh
            {0xFE93, {0x0629}}, {0xFE94, {0x0629}},  // ة Teh Marbuta
            {0xFE95, {0x062A}}, {0xFE96, {0x062A}}, {0xFE97, {0x062A}}, {0xFE98, {0x062A}},  // ت Teh
            {0xFE99, {0x062B}}, {0xFE9A, {0x062B}}, {0xFE9B, {0x062B}}, {0xFE9C, {0x062B}},  // ث Theh
            {0xFE9D, {0x062C}}, {0xFE9E, {0x062C}}, {0xFE9F, {0x062C}}, {0xFEA0, {0x062C}},  // ج Jeem
            {0xFEA1, {0x062D}}, {0xFEA2, {0x062D}}, {0xFEA3, {0x062D}}, {0xFEA4, {0x062D}},  // ح Hah
            {0xFEA5, {0x062E}}, {0xFEA6, {0x062E}}, {0xFEA7, {0x062E}}, {0xFEA8, {0x062E}},  // خ Khah
            {0xFEA9, {0x062F}}, {0xFEAA, {0x062F}},  // د Dal
            {0xFEAB, {0x0630}}, {0xFEAC, {0x0630}},  // ذ Thal
            {0xFEAD, {0x0631}}, {0xFEAE, {0x0631}},  // ر Reh
            {0xFEAF, {0x0632}}, {0xFEB0, {0x0632}},  // ز Zain
            {0xFEB1, {0x0633}}, {0xFEB2, {0x0633}}, {0xFEB3, {0x0633}}, {0xFEB4, {0x0633}},  // س Seen
            {0xFEB5, {0x0634}}, {0xFEB6, {0x0634}}, {0xFEB7, {0x0634}}, {0xFEB8, {0x0634}},  // ش Sheen
            {0xFEB9, {0x0635}}, {0xFEBA, {0x0635}}, {0xFEBB, {0x0635}}, {0xFEBC, {0x0635}},  // ص Sad
            {0xFEBD, {0x0636}}, {0xFEBE, {0x0636}}, {0xFEBF, {0x0636}}, {0xFEC0, {0x0636}},  // ض Dad
            {0xFEC1, {0x0637}}, {0xFEC2, {0x0637}}, {0xFEC3, {0x0637}}, {0xFEC4, {0x0637}},  // ط Tah
            {0xFEC5, {0x0638}}, {0xFEC6, {0x0638}}, {0xFEC7, {0x0638}}, {0xFEC8, {0x0638}},  // ظ Zah
            {0xFEC9, {0x0639}}, {0xFECA, {0x0639}}, {0xFECB, {0x0639}}, {0xFECC, {0x0639}},  // ع Ain
            {0xFECD, {0x063A}}, {0xFECE, {0x063A}}, {0xFECF, {0x063A}}, {0xFED0, {0x063A}},  // غ Ghain
            {0xFED1, {0x0641}}, {0xFED2, {0x0641}}, {0xFED3, {0x0641}}, {0xFED4, {0x0641}},  // ف Feh
            {0xFED5, {0x0642}}, {0xFED6, {0x0642}}, {0xFED7, {0x0642}}, {0xFED8, {0x0642}},  // ق Qaf
            {0xFED9, {0x0643}}, {0xFEDA, {0x0643}}, {0xFEDB, {0x0643}}, {0xFEDC, {0x0643}},  // ك Kaf
            {0xFEDD, {0x0644}}, {0xFEDE, {0x0644}}, {0xFEDF, {0x0644}}, {0xFEE0, {0x0644}},  // ل Lam
            {0xFEE1, {0x0645}}, {0xFEE2, {0x0645}}, {0xFEE3, {0x0645}}, {0xFEE4, {0x0645}},  // م Meem
            {0xFEE5, {0x0646}}, {0xFEE6, {0x0646}}, {0xFEE7, {0x0646}}, {0xFEE8, {0x0646}},  // ن Noon
            {0xFEE9, {0x0647}}, {0xFEEA, {0x0647}}, {0xFEEB, {0x0647}}, {0xFEEC, {0x0647}},  // ه Heh
            {0xFEED, {0x0648}}, {0xFEEE, {0x0648}},  // و Waw
            {0xFEEF, {0x0649}}, {0xFEF0, {0x0649}},  // ى Alef Maksura
            {0xFEF1, {0x064A}}, {0xFEF2, {0x064A}}, {0xFEF3, {0x064A}}, {0xFEF4, {0x064A}},  // ي Yeh
            
            // Lam-Alef ligatures
            {0xFEF5, {0x0644, 0x0622}}, {0xFEF6, {0x0644, 0x0622}},  // لآ
            {0xFEF7, {0x0644, 0x0623}}, {0xFEF8, {0x0644, 0x0623}},  // لأ
            {0xFEF9, {0x0644, 0x0625}}, {0xFEFA, {0x0644, 0x0625}},  // لإ
            {0xFEFB, {0x0644, 0x0627}}, {0xFEFC, {0x0644, 0x0627}},  // لا
        };
        
        auto it = table.find(cp);
        if (it != table.end()) {
            return it->second;
        }
        
        return {cp};  // بدون تغيير
    }
    
    /**
     * الحصول على فئة الدمج للحرف
     */
    static uint8_t getCombiningClass(uint32_t cp) {
        // Arabic combining marks
        switch (cp) {
            case 0x064B: return CombiningClass::ARABIC_FATHATAN;
            case 0x064C: return CombiningClass::ARABIC_DAMMATAN;
            case 0x064D: return CombiningClass::ARABIC_KASRATAN;
            case 0x064E: return CombiningClass::ARABIC_FATHA;
            case 0x064F: return CombiningClass::ARABIC_DAMMA;
            case 0x0650: return CombiningClass::ARABIC_KASRA;
            case 0x0651: return CombiningClass::ARABIC_SHADDA;
            case 0x0652: return CombiningClass::ARABIC_SUKUN;
            case 0x0670: return CombiningClass::ARABIC_SUPERSCRIPT_ALEF;
            default: return CombiningClass::NOT_REORDERED;
        }
    }
    
    /**
     * هل الحرف علامة تشكيل
     */
    static bool isCombiningMark(uint32_t cp) {
        return getCombiningClass(cp) != CombiningClass::NOT_REORDERED;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        أدوات UTF-8 | UTF-8 Utilities
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 UTF8Utils
 */
class UTF8Utils {
public:
    /**
     * قراءة codepoint من UTF-8
     */
    static uint32_t readCodepoint(const char* str, size_t& bytesRead) {
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
    static std::string writeCodepoint(uint32_t cp) {
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
    
    /**
     * تحويل نص إلى قائمة codepoints
     */
    static std::vector<uint32_t> toCodepoints(const std::string& str) {
        std::vector<uint32_t> result;
        
        size_t i = 0;
        while (i < str.size()) {
            size_t bytesRead;
            uint32_t cp = readCodepoint(str.c_str() + i, bytesRead);
            result.push_back(cp);
            i += bytesRead;
        }
        
        return result;
    }
    
    /**
     * تحويل قائمة codepoints إلى نص
     */
    static std::string fromCodepoints(const std::vector<uint32_t>& codepoints) {
        std::string result;
        
        for (uint32_t cp : codepoints) {
            result += writeCodepoint(cp);
        }
        
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                       المطبّع | Normalizer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 ArabicNormalizer
 * 
 * تطبيع النصوص العربية
 */
class ArabicNormalizer {
public:
    /**
     * تطبيع إلى NFC
     */
    static std::string normalizeNFC(const std::string& input) {
        // الخطوة 1: التفكيك القانوني (NFD)
        std::vector<uint32_t> decomposed = decompose(input, false);
        
        // الخطوة 2: ترتيب علامات الدمج
        reorderCombiningMarks(decomposed);
        
        // الخطوة 3: التركيب القانوني
        std::vector<uint32_t> composed = compose(decomposed);
        
        return UTF8Utils::fromCodepoints(composed);
    }
    
    /**
     * تطبيع إلى NFD
     */
    static std::string normalizeNFD(const std::string& input) {
        std::vector<uint32_t> decomposed = decompose(input, false);
        reorderCombiningMarks(decomposed);
        return UTF8Utils::fromCodepoints(decomposed);
    }
    
    /**
     * تطبيع إلى NFKC
     */
    static std::string normalizeNFKC(const std::string& input) {
        std::vector<uint32_t> decomposed = decompose(input, true);  // compatibility
        reorderCombiningMarks(decomposed);
        std::vector<uint32_t> composed = compose(decomposed);
        return UTF8Utils::fromCodepoints(composed);
    }
    
    /**
     * تطبيع إلى NFKD
     */
    static std::string normalizeNFKD(const std::string& input) {
        std::vector<uint32_t> decomposed = decompose(input, true);  // compatibility
        reorderCombiningMarks(decomposed);
        return UTF8Utils::fromCodepoints(decomposed);
    }
    
    /**
     * التحقق من أن النص مطبّع
     */
    static bool isNormalized(const std::string& input, NormalizationForm form) {
        std::string normalized;
        
        switch (form) {
            case NormalizationForm::NFC:
                normalized = normalizeNFC(input);
                break;
            case NormalizationForm::NFD:
                normalized = normalizeNFD(input);
                break;
            case NormalizationForm::NFKC:
                normalized = normalizeNFKC(input);
                break;
            case NormalizationForm::NFKD:
                normalized = normalizeNFKD(input);
                break;
        }
        
        return input == normalized;
    }
    
private:
    /**
     * التفكيك | Decomposition
     */
    static std::vector<uint32_t> decompose(const std::string& input, bool compatibility) {
        std::vector<uint32_t> codepoints = UTF8Utils::toCodepoints(input);
        std::vector<uint32_t> result;
        
        for (uint32_t cp : codepoints) {
            // تحويل أشكال العرض
            if (cp >= 0xFE70 && cp <= 0xFEFF) {
                auto decomposed = ArabicNormalizationTables::decomposePresBForm(cp);
                result.insert(result.end(), decomposed.begin(), decomposed.end());
            }
            // تحويل أشكال العرض-A (للتوافقية)
            else if (compatibility && cp >= 0xFB50 && cp <= 0xFDFF) {
                auto decomposed = decomposePresAForm(cp);
                result.insert(result.end(), decomposed.begin(), decomposed.end());
            }
            else {
                result.push_back(cp);
            }
        }
        
        return result;
    }
    
    /**
     * تفكيك أشكال العرض-A
     */
    static std::vector<uint32_t> decomposePresAForm(uint32_t cp) {
        // تنفيذ مبسط - في الإنتاج نحتاج جدول كامل
        return {cp};
    }
    
    /**
     * ترتيب علامات الدمج | Reorder combining marks
     */
    static void reorderCombiningMarks(std::vector<uint32_t>& codepoints) {
        // ترتيب فقاعي مستقر حسب فئة الدمج
        for (size_t i = 1; i < codepoints.size(); i++) {
            uint8_t currentClass = ArabicNormalizationTables::getCombiningClass(codepoints[i]);
            
            if (currentClass == 0) continue;
            
            size_t j = i;
            while (j > 0) {
                uint8_t prevClass = ArabicNormalizationTables::getCombiningClass(codepoints[j-1]);
                
                if (prevClass == 0 || prevClass <= currentClass) {
                    break;
                }
                
                std::swap(codepoints[j], codepoints[j-1]);
                j--;
            }
        }
    }
    
    /**
     * التركيب | Composition
     */
    static std::vector<uint32_t> compose(const std::vector<uint32_t>& decomposed) {
        // تنفيذ مبسط - في الإنتاج نحتاج جدول تركيب كامل
        // للعربية معظم الحروف لا تتركب، لكن نحتاج معالجة خاصة
        
        std::vector<uint32_t> result;
        
        for (uint32_t cp : decomposed) {
            result.push_back(cp);
        }
        
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    مولد LLVM | LLVM Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 NormalizationCodegen
 * 
 * توليد كود LLVM للتطبيع
 */
class NormalizationCodegen {
private:
    llvm::LLVMContext& context;
    llvm::Module* module;
    llvm::IRBuilder<>& builder;
    
public:
    NormalizationCodegen(llvm::LLVMContext& ctx, llvm::Module* mod, llvm::IRBuilder<>& b)
        : context(ctx)
        , module(mod)
        , builder(b)
    {}
    
    /**
     * توليد استدعاء تطبيع
     */
    llvm::Value* generateNormalize(llvm::Value* strPtr, NormalizationForm form) {
        std::string funcName;
        
        switch (form) {
            case NormalizationForm::NFC:
                funcName = "sad_normalize_nfc";
                break;
            case NormalizationForm::NFD:
                funcName = "sad_normalize_nfd";
                break;
            case NormalizationForm::NFKC:
                funcName = "sad_normalize_nfkc";
                break;
            case NormalizationForm::NFKD:
                funcName = "sad_normalize_nfkd";
                break;
        }
        
        llvm::Function* func = module->getFunction(funcName);
        
        if (!func) {
            llvm::Type* ptrType = llvm::PointerType::get(context, 0);
            
            llvm::FunctionType* funcType = llvm::FunctionType::get(
                ptrType,
                {ptrType},
                false
            );
            
            func = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                funcName,
                module
            );
        }
        
        return builder.CreateCall(func, {strPtr}, "normalized");
    }
    
    /**
     * توليد فحص التطبيع
     */
    llvm::Value* generateIsNormalized(llvm::Value* strPtr, NormalizationForm form) {
        llvm::Function* func = module->getFunction("sad_is_normalized");
        
        if (!func) {
            llvm::Type* ptrType = llvm::PointerType::get(context, 0);
            llvm::Type* i1Type = llvm::Type::getInt1Ty(context);
            llvm::Type* i32Type = llvm::Type::getInt32Ty(context);
            
            llvm::FunctionType* funcType = llvm::FunctionType::get(
                i1Type,
                {ptrType, i32Type},
                false
            );
            
            func = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                "sad_is_normalized",
                module
            );
        }
        
        llvm::Value* formVal = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(context),
            static_cast<int>(form)
        );
        
        return builder.CreateCall(func, {strPtr, formVal}, "is_normalized");
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {
    /**
     * تطبيع NFC
     */
    char* sad_normalize_nfc(const char* input) {
        std::string result = ArabicNormalizer::normalizeNFC(input);
        const std::size_t bufSize = result.size() + 1;
        char* output = new char[bufSize];
        // (AR) memcpy آمن: الوجهة مخصصة بحجم دقيق + 1 لـ null / (EN) safe memcpy: dest sized exactly
        std::memcpy(output, result.c_str(), bufSize);
        return output;
    }
    
    /**
     * تطبيع NFD
     */
    char* sad_normalize_nfd(const char* input) {
        std::string result = ArabicNormalizer::normalizeNFD(input);
        const std::size_t bufSize = result.size() + 1;
        char* output = new char[bufSize];
        std::memcpy(output, result.c_str(), bufSize);
        return output;
    }
    
    /**
     * تطبيع NFKC
     */
    char* sad_normalize_nfkc(const char* input) {
        std::string result = ArabicNormalizer::normalizeNFKC(input);
        const std::size_t bufSize = result.size() + 1;
        char* output = new char[bufSize];
        std::memcpy(output, result.c_str(), bufSize);
        return output;
    }
    
    /**
     * تطبيع NFKD
     */
    char* sad_normalize_nfkd(const char* input) {
        std::string result = ArabicNormalizer::normalizeNFKD(input);
        const std::size_t bufSize = result.size() + 1;
        char* output = new char[bufSize];
        std::memcpy(output, result.c_str(), bufSize);
        return output;
    }
    
    /**
     * التحقق من التطبيع
     */
    int sad_is_normalized(const char* input, int form) {
        return ArabicNormalizer::isNormalized(
            input,
            static_cast<NormalizationForm>(form)
        ) ? 1 : 0;
    }
    
    /**
     * تحرير الذاكرة
     */
    void sad_free_normalized(char* str) {
        delete[] str;
    }
}

} // namespace optimizer
} // namespace sad
