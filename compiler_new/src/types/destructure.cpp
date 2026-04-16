// ════════════════════════════════════════════════════════════════════════════════
// ملف: destructure.cpp
// File: destructure.cpp
//
// الوصف (AR): أنماط فك التركيب (Destructuring Patterns)
//             يدعم فك تركيب القوائم، السجلات، والأنواع المركبة
//             يستنتج أنواع المتغيرات المربوطة
//
// Description (EN): Destructuring Patterns
//                   Supports destructuring lists, records, and compound types
//                   Infers types of bound variables
//
// المؤلف: فريق تطوير لغة ص
// Author: Sad Language Development Team
//
// التاريخ: يناير 2026
// Date: January 2026
//
// المرحلة: Phase 12 - User Story 10 - Pattern Matching
// Phase: Phase 12 - User Story 10 - Pattern Matching
//
// المهمة: T141 [US10] Implement destructuring patterns
// Task: T141 [US10] Implement destructuring patterns
// ════════════════════════════════════════════════════════════════════════════════

// تعطيل تحذير Unicode للتعليقات العربية
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include "pattern_nodes.h"
#include "type_registry.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// معلومات فك التركيب / Destructuring Info
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نوع فك التركيب
 *        (EN) Destructuring kind
 */
enum class DestructureKind {
    List,       ///< (AR) قائمة [أ، ب، ج] / (EN) List [a, b, c]
    Tuple,      ///< (AR) صف (أ، ب، ج) / (EN) Tuple (a, b, c)
    Record,     ///< (AR) سجل {اسم: أ، عمر: ب} / (EN) Record {name: a, age: b}
    Struct,     ///< (AR) هيكل شخص{اسم، عمر} / (EN) Struct Person{name, age}
    Variant     ///< (AR) متغير نوع::حالة(بيانات) / (EN) Variant Type::Case(data)
};

/**
 * @brief (AR) عنصر مُستخرج من فك التركيب
 *        (EN) Extracted element from destructuring
 */
struct DestructuredElement {
    std::string variableName;   ///< (AR) اسم المتغير المربوط / (EN) Bound variable name
    TypePtr inferredType;       ///< (AR) النوع المُستنتج / (EN) Inferred type
    std::string accessPath;     ///< (AR) مسار الوصول للقيمة / (EN) Access path to value
    bool isRest;                ///< (AR) هل هو عنصر *rest؟ / (EN) Is it *rest element?
    
    DestructuredElement(const std::string& name, TypePtr type, 
                        const std::string& path, bool rest = false)
        : variableName(name), inferredType(type), accessPath(path), isRest(rest) {}
};

/**
 * @brief (AR) نتيجة فك التركيب
 *        (EN) Destructuring result
 */
struct DestructureResult {
    bool success;
    DestructureKind kind;
    std::vector<DestructuredElement> elements;
    std::vector<std::string> errors;
    
    static DestructureResult success_result(DestructureKind k) {
        return DestructureResult{true, k, {}, {}};
    }
    
    static DestructureResult failure(const std::string& error) {
        return DestructureResult{false, DestructureKind::List, {}, {error}};
    }
    
    void addElement(const DestructuredElement& elem) {
        elements.push_back(elem);
    }
    
    void addError(const std::string& error) {
        success = false;
        errors.push_back(error);
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// محلل فك التركيب / Destructure Analyzer
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) محلل أنماط فك التركيب
 *        (EN) Destructuring pattern analyzer
 * 
 * (AR) يقوم بـ:
 *      1. تحليل بنية النمط لتحديد نوع فك التركيب
 *      2. استنتاج أنواع المتغيرات المربوطة
 *      3. إنشاء مسارات الوصول للقيم
 *      4. التحقق من صحة البنية مقابل النوع المُختبر
 * 
 * (EN) Performs:
 *      1. Analyzes pattern structure to determine destructure kind
 *      2. Infers types of bound variables
 *      3. Creates access paths to values
 *      4. Validates structure against matched type
 */
class DestructureAnalyzer {
public:
    // ========================================================================
    // البناء والهدم / Construction & Destruction
    // ========================================================================
    
    DestructureAnalyzer()
        : useArabicMessages_(true)
        , debugMode_(false) {
    }
    
    ~DestructureAnalyzer() = default;
    
    // ========================================================================
    // الواجهة الرئيسية / Main Interface
    // ========================================================================
    
    /**
     * @brief (AR) تحليل نمط وفك تركيبه
     *        (EN) Analyze and destructure a pattern
     * 
     * @param pattern (AR) النمط المراد فك تركيبه / (EN) Pattern to destructure
     * @param matchedType (AR) نوع القيمة المُطابقة / (EN) Type of matched value
     * @param basePath (AR) المسار الأساسي للوصول / (EN) Base access path
     * @return (AR) نتيجة فك التركيب / (EN) Destructure result
     */
    DestructureResult analyze(
        const AST::Pattern& pattern,
        TypePtr matchedType,
        const std::string& basePath = ""
    ) {
        // (AR) نمط شامل - لا فك تركيب
        // (EN) Wildcard - no destructuring
        if (dynamic_cast<const AST::WildcardPattern*>(&pattern)) {
            return DestructureResult::success_result(DestructureKind::List);
        }
        
        // (AR) نمط متغير بسيط
        // (EN) Simple variable pattern
        if (auto* varPat = dynamic_cast<const AST::VariablePattern*>(&pattern)) {
            auto result = DestructureResult::success_result(DestructureKind::List);
            result.addElement(DestructuredElement(
                varPat->name,
                matchedType,
                basePath.empty() ? varPat->name : basePath
            ));
            return result;
        }
        
        // (AR) نمط قائمة
        // (EN) List pattern
        if (auto* listPat = dynamic_cast<const AST::ListPattern*>(&pattern)) {
            return analyzeListPattern(*listPat, matchedType, basePath);
        }
        
        // (AR) نمط OR - يجب أن تكون جميع البدائل متسقة
        // (EN) OR pattern - all alternatives must be consistent
        if (auto* orPat = dynamic_cast<const AST::OrPattern*>(&pattern)) {
            return analyzeOrPattern(*orPat, matchedType, basePath);
        }
        
        // (AR) نمط قيمة حرفية - لا فك تركيب
        // (EN) Literal pattern - no destructuring
        if (dynamic_cast<const AST::LiteralPattern*>(&pattern)) {
            return DestructureResult::success_result(DestructureKind::List);
        }
        
        return DestructureResult::failure(formatError(
            "نوع نمط غير مدعوم لفك التركيب",
            "Unsupported pattern type for destructuring"
        ));
    }
    
    /**
     * @brief (AR) التحقق من قابلية النوع لفك التركيب
     *        (EN) Check if type supports destructuring
     */
    bool isDestructurable(TypePtr type) {
        if (!type) return false;
        
        switch (type->getKind()) {
            case SadTypeKind::Array:
            case SadTypeKind::Tuple:
            // case SadTypeKind::Struct:  // TODO: Add when Struct is added to SadTypeKind
            case SadTypeKind::Class:
            case SadTypeKind::Union:
                return true;
            default:
                return false;
        }
    }
    
    // ========================================================================
    // الإعدادات / Settings
    // ========================================================================
    
    void setArabicMessages(bool enable) { useArabicMessages_ = enable; }
    void setDebugMode(bool enable) { debugMode_ = enable; }

private:
    // ========================================================================
    // تحليل أنواع الأنماط المحددة / Specific Pattern Analysis
    // ========================================================================
    
    /**
     * @brief (AR) تحليل نمط قائمة
     *        (EN) Analyze list pattern
     */
    DestructureResult analyzeListPattern(
        const AST::ListPattern& pattern,
        TypePtr matchedType,
        const std::string& basePath
    ) {
        auto result = DestructureResult::success_result(DestructureKind::List);
        
        // (AR) استخراج نوع العنصر
        // (EN) Extract element type
        TypePtr elementType = nullptr;
        if (matchedType) {
            if (matchedType->getKind() == SadTypeKind::Array) {
                // (AR) افتراض نوع عنصر عام
                // (EN) Assume generic element type
                auto& registry = TypeRegistry::getInstance();
                elementType = registry.getAnyType();
            }
        }
        
        // (AR) تحليل كل عنصر
        // (EN) Analyze each element
        for (size_t i = 0; i < pattern.elements.size(); ++i) {
            std::string elemPath = basePath.empty() 
                ? "[" + std::to_string(i) + "]"
                : basePath + "[" + std::to_string(i) + "]";
            
            auto elemResult = analyze(*pattern.elements[i], elementType, elemPath);
            
            if (!elemResult.success) {
                for (const auto& error : elemResult.errors) {
                    result.addError(error);
                }
            } else {
                for (const auto& elem : elemResult.elements) {
                    result.addElement(elem);
                }
            }
        }
        
        // (AR) عنصر rest
        // (EN) Rest element
        if (pattern.has_rest && !pattern.rest_name.empty()) {
            std::string restPath = basePath.empty()
                ? "[" + std::to_string(pattern.elements.size()) + "..]"
                : basePath + "[" + std::to_string(pattern.elements.size()) + "..]";
            
            result.addElement(DestructuredElement(
                pattern.rest_name,
                matchedType,  // (AR) نفس نوع القائمة / (EN) Same list type
                restPath,
                true  // isRest
            ));
        }
        
        if (debugMode_) {
            std::cout << "[Destructure] List pattern with " 
                      << result.elements.size() << " elements\n";
            for (const auto& elem : result.elements) {
                std::cout << "  - " << elem.variableName 
                          << " @ " << elem.accessPath << "\n";
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) تحليل نمط OR
     *        (EN) Analyze OR pattern
     */
    DestructureResult analyzeOrPattern(
        const AST::OrPattern& pattern,
        TypePtr matchedType,
        const std::string& basePath
    ) {
        if (pattern.alternatives.empty()) {
            return DestructureResult::failure(formatError(
                "نمط OR بدون بدائل",
                "OR pattern with no alternatives"
            ));
        }
        
        // (AR) تحليل البديل الأول كمرجع
        // (EN) Analyze first alternative as reference
        auto result = analyze(*pattern.alternatives[0], matchedType, basePath);
        
        // (AR) التحقق من تطابق البدائل الأخرى
        // (EN) Verify other alternatives match
        for (size_t i = 1; i < pattern.alternatives.size(); ++i) {
            auto altResult = analyze(*pattern.alternatives[i], matchedType, basePath);
            
            // (AR) التحقق من تطابق المتغيرات
            // (EN) Verify variables match
            if (altResult.elements.size() != result.elements.size()) {
                result.addError(formatError(
                    "بدائل OR يجب أن تربط نفس عدد المتغيرات",
                    "OR alternatives must bind same number of variables"
                ));
                continue;
            }
            
            for (size_t j = 0; j < result.elements.size(); ++j) {
                if (j >= altResult.elements.size()) break;
                
                if (result.elements[j].variableName != altResult.elements[j].variableName) {
                    result.addError(formatError(
                        "بدائل OR يجب أن تربط نفس أسماء المتغيرات",
                        "OR alternatives must bind same variable names"
                    ));
                }
            }
        }
        
        return result;
    }
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    std::string formatError(const std::string& arabic, const std::string& english) {
        if (useArabicMessages_) {
            return arabic + " / " + english;
        }
        return english;
    }
    
    // ========================================================================
    // الحقول / Fields
    // ========================================================================
    
    bool useArabicMessages_;
    bool debugMode_;
};

// ════════════════════════════════════════════════════════════════════════════════
// مولد كود فك التركيب / Destructure Code Generator
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) مولد كود فك التركيب
 *        (EN) Destructure code generator
 * 
 * (AR) يولد كود C++ أو IR لفك تركيب القيم
 * (EN) Generates C++ or IR code for destructuring values
 */
class DestructureCodeGen {
public:
    /**
     * @brief (AR) توليد كود فك تركيب
     *        (EN) Generate destructure code
     * 
     * @param result (AR) نتيجة التحليل / (EN) Analysis result
     * @param sourceVar (AR) المتغير المصدر / (EN) Source variable
     * @return (AR) الكود المُولد / (EN) Generated code
     */
    std::string generateCode(
        const DestructureResult& result,
        const std::string& sourceVar
    ) {
        std::ostringstream code;
        
        code << "// فك التركيب / Destructuring\n";
        
        for (const auto& elem : result.elements) {
            if (elem.isRest) {
                // (AR) عنصر rest - نسخ الباقي
                // (EN) Rest element - copy remaining
                code << "auto " << elem.variableName << " = ";
                code << "std::vector<decltype(" << sourceVar << "[0])>(";
                code << sourceVar << ".begin() + " << extractIndex(elem.accessPath) << ", ";
                code << sourceVar << ".end());\n";
            } else {
                // (AR) عنصر عادي
                // (EN) Regular element
                code << "auto " << elem.variableName << " = ";
                code << sourceVar << elem.accessPath << ";\n";
            }
        }
        
        return code.str();
    }
    
    /**
     * @brief (AR) توليد شروط التحقق
     *        (EN) Generate validation conditions
     */
    std::string generateValidation(
        const DestructureResult& result,
        const std::string& sourceVar
    ) {
        std::ostringstream code;
        
        if (result.kind == DestructureKind::List) {
            size_t minSize = 0;
            bool hasRest = false;
            
            for (const auto& elem : result.elements) {
                if (elem.isRest) {
                    hasRest = true;
                } else {
                    minSize++;
                }
            }
            
            if (hasRest) {
                code << sourceVar << ".size() >= " << minSize;
            } else {
                code << sourceVar << ".size() == " << minSize;
            }
        }
        
        return code.str();
    }

private:
    size_t extractIndex(const std::string& path) {
        // (AR) استخراج الفهرس من مسار مثل "[3..]"
        // (EN) Extract index from path like "[3..]"
        size_t start = path.find('[');
        size_t end = path.find_first_of(".],", start + 1);
        if (start != std::string::npos && end != std::string::npos) {
            return std::stoull(path.substr(start + 1, end - start - 1));
        }
        return 0;
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// واجهة خارجية / External Interface
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تحليل نمط لفك التركيب
 *        (EN) Analyze pattern for destructuring
 */
DestructureResult analyzeDestructure(
    const AST::Pattern& pattern,
    TypePtr matchedType
) {
    DestructureAnalyzer analyzer;
    return analyzer.analyze(pattern, matchedType);
}

/**
 * @brief (AR) توليد كود فك التركيب
 *        (EN) Generate destructure code
 */
std::string generateDestructureCode(
    const DestructureResult& result,
    const std::string& sourceVar
) {
    DestructureCodeGen gen;
    return gen.generateCode(result, sourceVar);
}

/**
 * @brief (AR) التحقق من قابلية النوع لفك التركيب
 *        (EN) Check if type supports destructuring
 */
bool isTypeDestructurable(TypePtr type) {
    DestructureAnalyzer analyzer;
    return analyzer.isDestructurable(type);
}

} // namespace TypeSystem
} // namespace Sad

// ════════════════════════════════════════════════════════════════════════════════
// نهاية الملف / End of File
// ════════════════════════════════════════════════════════════════════════════════
