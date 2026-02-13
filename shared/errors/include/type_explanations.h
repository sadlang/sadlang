/**
 * @file type_explanations.h
 * @brief (AR) شرح تعليمي لأخطاء عدم تطابق الأنواع
 *        (EN) Educational type mismatch error explanations
 * 
 * (AR) المهمة: T162 - Phase 15 (US13)
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                         أنواع البيانات | Data Types
// ═══════════════════════════════════════════════════════════════════════

enum class SadType {
    UNKNOWN,    ///< غير معروف
    VOID_T,     ///< عدم (لا شيء) - renamed to avoid conflict with macro
    NUMBER,     ///< عدد
    INTEGER,    ///< صحيح
    FLOAT,      ///< عشري
    STRING,     ///< نص
    BOOLEAN,    ///< منطقي
    ARRAY,      ///< مصفوفة
    LIST,       ///< قائمة
    MAP,        ///< قاموس
    OBJECT,     ///< كائن
    FUNCTION,   ///< دالة
    OPTIONAL,   ///< اختياري
    UNION,      ///< اتحاد
    ANY         ///< أي
};

struct TypeInfo {
    SadType type;
    std::string arabicName;
    std::string englishName;
    std::string description;
    std::string analogy;
    std::string examples;
    std::vector<std::string> validOperations;
    std::vector<SadType> canConvertTo;
    std::vector<SadType> canConvertFrom;
    
    TypeInfo() : type(SadType::UNKNOWN) {}
};

// ═══════════════════════════════════════════════════════════════════════
//                   قاعدة بيانات الأنواع | Type Database
// ═══════════════════════════════════════════════════════════════════════

class TypeDatabase {
public:
    TypeDatabase();
    const TypeInfo* getTypeInfo(SadType type) const;
    const TypeInfo* getTypeInfoByName(const std::string& name) const;
    
private:
    std::unordered_map<SadType, TypeInfo> types_;
    std::unordered_map<std::string, SadType> nameToType_;
    void initializeTypes();
};

// ═══════════════════════════════════════════════════════════════════════
//                    شارح الأنواع | Type Explainer
// ═══════════════════════════════════════════════════════════════════════

struct TypeMismatchExplanation {
    std::string summary;        ///< ملخص
    std::string summaryEn;
    std::string detailedExplanation;
    std::string detailedExplanationEn;
    std::string analogy;
    std::string conversionHint;
    std::string conversionHintEn;
    std::string codeExample;
    
    std::string format(bool useArabic = true) const;
};

class TypeExplainer {
public:
    TypeExplainer();

    /**
     * @brief (AR) شرح خطأ عدم تطابق الأنواع
     */
    TypeMismatchExplanation explainTypeMismatch(
        const std::string& expectedType,
        const std::string& actualType,
        const std::string& expression = ""
    );

    /**
     * @brief (AR) الحصول على وصف نوع
     */
    std::string getTypeDescription(const std::string& typeName, bool arabic = true);

    /**
     * @brief (AR) الحصول على اسم عربي للنوع
     */
    std::string typeToArabicName(SadType type);
    std::string typeToEnglishName(SadType type);

    /**
     * @brief (AR) هل يمكن التحويل بين نوعين؟
     */
    bool canConvert(SadType from, SadType to);

    /**
     * @brief (AR) الحصول على دالة التحويل
     */
    std::string getConversionFunction(SadType from, SadType to);

private:
    TypeDatabase db_;
};

} // namespace Errors
} // namespace Sad
