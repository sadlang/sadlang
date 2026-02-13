/**
 * @file smart_type_explanations.cpp
 * @brief (AR) تطبيق شرح أخطاء الأنواع - Sad::Errors namespace
 * (AR) المهمة: T162 - Phase 15 (US13)
 */

#include "type_explanations.h"
#include <sstream>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    TypeMismatchExplanation::format
// ═══════════════════════════════════════════════════════════════════════

std::string TypeMismatchExplanation::format(bool useArabic) const {
    std::ostringstream oss;
    oss << (useArabic ? summary : summaryEn) << "\n";
    if (!detailedExplanation.empty()) {
        oss << "\n" << (useArabic ? detailedExplanation : detailedExplanationEn) << "\n";
    }
    if (!analogy.empty()) {
        oss << "\n" << analogy << "\n";
    }
    if (!conversionHint.empty()) {
        oss << "\n" << (useArabic ? conversionHint : conversionHintEn) << "\n";
    }
    if (!codeExample.empty()) {
        oss << "\n" << codeExample << "\n";
    }
    return oss.str();
}

// ═══════════════════════════════════════════════════════════════════════
//                    TypeDatabase
// ═══════════════════════════════════════════════════════════════════════

TypeDatabase::TypeDatabase() { initializeTypes(); }

const TypeInfo* TypeDatabase::getTypeInfo(SadType type) const {
    auto it = types_.find(type);
    return it != types_.end() ? &it->second : nullptr;
}

const TypeInfo* TypeDatabase::getTypeInfoByName(const std::string& name) const {
    auto it = nameToType_.find(name);
    if (it != nameToType_.end()) return getTypeInfo(it->second);
    return nullptr;
}

void TypeDatabase::initializeTypes() {
    // Number
    {
        TypeInfo info;
        info.type = SadType::NUMBER;
        info.arabicName = "\xD8\xB9\xD8\xAF\xD8\xAF";
        info.englishName = "number";
        info.description = "A numeric value for calculations";
        info.validOperations = {"+", "-", "*", "/", "%", "**", "==", "!=", "<", ">", "<=", ">="};
        info.canConvertTo = {SadType::STRING, SadType::BOOLEAN, SadType::FLOAT, SadType::INTEGER};
        info.canConvertFrom = {SadType::STRING, SadType::BOOLEAN};
        types_[SadType::NUMBER] = info;
        nameToType_["\xD8\xB9\xD8\xAF\xD8\xAF"] = SadType::NUMBER;
        nameToType_["number"] = SadType::NUMBER;
    }
    // String
    {
        TypeInfo info;
        info.type = SadType::STRING;
        info.arabicName = "\xD9\x86\xD8\xB5";
        info.englishName = "string";
        info.description = "A sequence of characters";
        info.validOperations = {"+", "==", "!=", "<", ">", "[]"};
        info.canConvertTo = {SadType::NUMBER, SadType::BOOLEAN, SadType::LIST};
        info.canConvertFrom = {SadType::NUMBER, SadType::BOOLEAN, SadType::LIST, SadType::OBJECT};
        types_[SadType::STRING] = info;
        nameToType_["\xD9\x86\xD8\xB5"] = SadType::STRING;
        nameToType_["string"] = SadType::STRING;
    }
    // Boolean
    {
        TypeInfo info;
        info.type = SadType::BOOLEAN;
        info.arabicName = "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A";
        info.englishName = "boolean";
        info.description = "True or false value";
        info.validOperations = {"&&", "||", "!", "==", "!="};
        info.canConvertTo = {SadType::STRING, SadType::NUMBER};
        info.canConvertFrom = {SadType::STRING, SadType::NUMBER};
        types_[SadType::BOOLEAN] = info;
        nameToType_["\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A"] = SadType::BOOLEAN;
        nameToType_["boolean"] = SadType::BOOLEAN;
    }
    // List
    {
        TypeInfo info;
        info.type = SadType::LIST;
        info.arabicName = "\xD9\x82\xD8\xA7\xD8\xA6\xD9\x85\xD8\xA9";
        info.englishName = "list";
        info.description = "Ordered collection of elements";
        info.validOperations = {"+", "[]", "==", "!="};
        info.canConvertTo = {SadType::STRING, SadType::ARRAY};
        types_[SadType::LIST] = info;
        nameToType_["\xD9\x82\xD8\xA7\xD8\xA6\xD9\x85\xD8\xA9"] = SadType::LIST;
        nameToType_["list"] = SadType::LIST;
    }
    // Map
    {
        TypeInfo info;
        info.type = SadType::MAP;
        info.arabicName = "\xD9\x82\xD8\xA7\xD9\x85\xD9\x88\xD8\xB3";
        info.englishName = "map";
        info.description = "Key-value collection";
        info.validOperations = {"[]", "==", "!="};
        types_[SadType::MAP] = info;
        nameToType_["\xD9\x82\xD8\xA7\xD9\x85\xD9\x88\xD8\xB3"] = SadType::MAP;
        nameToType_["map"] = SadType::MAP;
    }
    // Integer
    {
        TypeInfo info;
        info.type = SadType::INTEGER;
        info.arabicName = "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD";
        info.englishName = "integer";
        info.validOperations = {"+", "-", "*", "/", "%", "==", "!=", "<", ">"};
        info.canConvertTo = {SadType::NUMBER, SadType::FLOAT, SadType::STRING};
        types_[SadType::INTEGER] = info;
        nameToType_["\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD"] = SadType::INTEGER;
        nameToType_["integer"] = SadType::INTEGER;
    }
    // Float
    {
        TypeInfo info;
        info.type = SadType::FLOAT;
        info.arabicName = "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A";
        info.englishName = "float";
        info.validOperations = {"+", "-", "*", "/", "==", "!=", "<", ">"};
        info.canConvertTo = {SadType::NUMBER, SadType::INTEGER, SadType::STRING};
        types_[SadType::FLOAT] = info;
        nameToType_["\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A"] = SadType::FLOAT;
        nameToType_["float"] = SadType::FLOAT;
    }
}

// ═══════════════════════════════════════════════════════════════════════
//                    TypeExplainer
// ═══════════════════════════════════════════════════════════════════════

TypeExplainer::TypeExplainer() {}

std::string TypeExplainer::typeToArabicName(SadType type) {
    const TypeInfo* info = db_.getTypeInfo(type);
    return info ? info->arabicName : "\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x88\xD9\x81";
}

std::string TypeExplainer::typeToEnglishName(SadType type) {
    const TypeInfo* info = db_.getTypeInfo(type);
    return info ? info->englishName : "unknown";
}

std::string TypeExplainer::getTypeDescription(const std::string& typeName, bool arabic) {
    const TypeInfo* info = db_.getTypeInfoByName(typeName);
    if (!info) return arabic ? "\xD9\x86\xD9\x88\xD8\xB9 \xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x88\xD9\x81" : "Unknown type";
    return info->description;
}

bool TypeExplainer::canConvert(SadType from, SadType to) {
    const TypeInfo* info = db_.getTypeInfo(from);
    if (!info) return false;
    for (auto t : info->canConvertTo) {
        if (t == to) return true;
    }
    return false;
}

std::string TypeExplainer::getConversionFunction(SadType from, SadType to) {
    if (to == SadType::STRING) return "\xD9\x86\xD8\xB5()";
    if (to == SadType::NUMBER) return "\xD8\xB9\xD8\xAF\xD8\xAF()";
    if (to == SadType::BOOLEAN) return "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A()";
    if (to == SadType::INTEGER) return "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD()";
    if (to == SadType::FLOAT) return "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A()";
    return "";
}

TypeMismatchExplanation TypeExplainer::explainTypeMismatch(
    const std::string& expectedType,
    const std::string& actualType,
    const std::string& expression
) {
    TypeMismatchExplanation result;
    
    result.summary = "\xD8\xB9\xD8\xAF\xD9\x85 \xD8\xAA\xD8\xB7\xD8\xA7\xD8\xA8\xD9\x82 \xD8\xA7\xD9\x84\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9: \xD9\x85\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9 '" + expectedType + "' \xD9\x88\xD9\x84\xD9\x83\xD9\x86 \xD9\x88\xD8\xAC\xD8\xAF '" + actualType + "'";
    result.summaryEn = "Type mismatch: expected '" + expectedType + "' but found '" + actualType + "'";
    
    const TypeInfo* expInfo = db_.getTypeInfoByName(expectedType);
    const TypeInfo* actInfo = db_.getTypeInfoByName(actualType);
    
    if (expInfo && actInfo) {
        result.detailedExplanation = 
            "\xD8\xA7\xD9\x84\xD9\x86\xD9\x88\xD8\xB9 '" + expInfo->arabicName + "' \xD9\x84\xD8\xA7 \xD9\x8A\xD8\xAA\xD9\x88\xD8\xA7\xD9\x81\xD9\x82 \xD9\x85\xD8\xB9 \xD8\xA7\xD9\x84\xD9\x86\xD9\x88\xD8\xB9 '" + actInfo->arabicName + "'";
        result.detailedExplanationEn =
            "Type '" + expInfo->englishName + "' is not compatible with '" + actInfo->englishName + "'";
        
        bool convertible = canConvert(actInfo->type, expInfo->type);
        if (convertible) {
            std::string convFunc = getConversionFunction(actInfo->type, expInfo->type);
            result.conversionHint = "\xD9\x8A\xD9\x85\xD9\x83\xD9\x86\xD9\x83 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84 \xD8\xA8\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD8\xA7\xD9\x85: " + convFunc;
            result.conversionHintEn = "You can convert using: " + convFunc;
            if (!expression.empty()) {
                result.codeExample = convFunc.substr(0, convFunc.size()-1) + expression + ")";
            }
        }
    }
    
    return result;
}

} // namespace Errors
} // namespace Sad
