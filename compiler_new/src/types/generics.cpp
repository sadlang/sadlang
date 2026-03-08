// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file generics.cpp
 * @brief تنفيذ نظام الأنواع المعممة / Generics System Implementation
 */

#include "types/generics.h"
#include "types/type.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    GenericParameter Implementation
// =============================================================================

GenericParameter::GenericParameter(const std::string& name, size_t index)
    : name_(name)
    , index_(index)
{
}

void GenericParameter::addTraitBound(TraitPtr trait) {
    bounds_.addTrait(trait);
}

bool GenericParameter::satisfiesBounds(TypePtr type, TraitRegistry& registry) const {
    return bounds_.isSatisfiedBy(type, registry);
}

void GenericParameter::setDefaultType(TypePtr defaultType) {
    defaultType_ = defaultType;
}

std::string GenericParameter::toString(bool arabic) const {
    std::stringstream ss;
    ss << name_;
    
    if (hasBounds()) {
        ss << ": " << bounds_.toString(arabic);
    }
    
    return ss.str();
}

// =============================================================================
//                    GenericType Implementation
// =============================================================================

GenericType::GenericType(const std::string& baseName)
    : baseName_(baseName)
{
}

void GenericType::addParameter(GenericParameterPtr param) {
    if (param) {
        parameters_.push_back(param);
    }
}

GenericParameterPtr GenericType::findParameter(const std::string& name) const {
    for (const auto& param : parameters_) {
        if (param->getName() == name) {
            return param;
        }
    }
    return nullptr;
}

GenericParameterPtr GenericType::getParameter(size_t index) const {
    return (index < parameters_.size()) ? parameters_[index] : nullptr;
}

GenericInstantiationPtr GenericType::instantiate(
    const std::vector<TypePtr>& typeArgs,
    TraitRegistry& registry) const 
{
    std::vector<std::string> errors;
    if (!validateTypeArgs(typeArgs, registry, errors)) {
        return nullptr;
    }
    
    // إنشاء تخصيص جديد مباشرة
    auto genericTypePtr = std::const_pointer_cast<GenericType>(shared_from_this());
    return std::make_shared<GenericInstantiation>(genericTypePtr, typeArgs);
}

bool GenericType::validateTypeArgs(
    const std::vector<TypePtr>& typeArgs,
    TraitRegistry& registry,
    std::vector<std::string>& errors) const 
{
    // التحقق من عدد المعاملات
    size_t requiredCount = 0;
    for (const auto& param : parameters_) {
        if (!param->hasDefault()) {
            ++requiredCount;
        }
    }
    
    if (typeArgs.size() < requiredCount) {
        std::stringstream ss;
        ss << "عدد الأنواع المحددة غير كافٍ: " << typeArgs.size() 
           << " بينما المطلوب " << requiredCount;
        errors.push_back(ss.str());
        return false;
    }
    
    if (typeArgs.size() > parameters_.size()) {
        std::stringstream ss;
        ss << "عدد الأنواع المحددة أكثر من المتوقع: " << typeArgs.size() 
           << " بينما الحد الأقصى " << parameters_.size();
        errors.push_back(ss.str());
        return false;
    }
    
    // التحقق من قيود كل معامل
    for (size_t i = 0; i < typeArgs.size(); ++i) {
        const auto& param = parameters_[i];
        const auto& typeArg = typeArgs[i];
        
        if (!param->satisfiesBounds(typeArg, registry)) {
            std::stringstream ss;
            ss << "النوع المحدد لـ " << param->getName() 
               << " لا يحقق القيود المطلوبة: " << param->getBounds().toString();
            errors.push_back(ss.str());
            return false;
        }
    }
    
    return true;
}

std::string GenericType::toString(bool arabic) const {
    std::stringstream ss;
    ss << baseName_ << "<";
    
    bool first = true;
    for (const auto& param : parameters_) {
        if (!first) ss << ", ";
        ss << param->toString(arabic);
        first = false;
    }
    
    ss << ">";
    return ss.str();
}

// =============================================================================
//                    GenericInstantiation Implementation
// =============================================================================

GenericInstantiation::GenericInstantiation(
    GenericTypePtr genericType,
    const std::vector<TypePtr>& typeArgs)
    : genericType_(genericType)
    , typeArgs_(typeArgs)
{
}

TypePtr GenericInstantiation::getTypeArg(size_t index) const {
    return (index < typeArgs_.size()) ? typeArgs_[index] : nullptr;
}

std::string GenericInstantiation::getName(bool arabic) const {
    if (!genericType_) return "";
    
    std::stringstream ss;
    ss << genericType_->getBaseName() << "<";
    
    bool first = true;
    for (const auto& typeArg : typeArgs_) {
        if (!first) ss << ", ";
        // في المستقبل: استخدام typeArg->getName(arabic)
        ss << "نوع";
        first = false;
    }
    
    ss << ">";
    return ss.str();
}

std::string GenericInstantiation::getKey() const {
    if (!genericType_) return "";
    
    std::stringstream ss;
    ss << genericType_->getBaseName() << "<";
    
    bool first = true;
    for (const auto& typeArg : typeArgs_) {
        if (!first) ss << ",";
        // (AR) استخدام تمثيل النوع النصي بدلاً من عنوان الذاكرة
        // (EN) Use type string representation instead of memory address
        ss << (typeArg ? typeArg->toString() : "null");
        first = false;
    }
    
    ss << ">";
    return ss.str();
}

bool GenericInstantiation::equals(const GenericInstantiation& other) const {
    return getKey() == other.getKey();
}

// =============================================================================
//                    GenericsParser Implementation
// =============================================================================

ParsedGenericDecl GenericsParser::parse(const std::string& declaration) {
    ParsedGenericDecl result;
    
    // البحث عن < و >
    size_t openBracket = declaration.find('<');
    size_t closeBracket = declaration.rfind('>');
    
    if (openBracket == std::string::npos || closeBracket == std::string::npos) {
        result.isValid = false;
        result.errorMessage = "لم يتم العثور على أقواس المعممات <>"; 
        return result;
    }
    
    if (closeBracket <= openBracket) {
        result.isValid = false;
        result.errorMessage = "أقواس المعممات غير صحيحة";
        return result;
    }
    
    // استخراج الاسم الأساسي
    result.baseName = trim(declaration.substr(0, openBracket));
    
    // استخراج المعاملات
    std::string paramsStr = declaration.substr(
        openBracket + 1, 
        closeBracket - openBracket - 1
    );
    
    result.parameters = parseParameters(paramsStr);
    result.isValid = true;
    
    return result;
}

std::vector<GenericParameterPtr> GenericsParser::parseParameters(
    const std::string& paramsStr) 
{
    std::vector<GenericParameterPtr> result;
    auto parts = splitByComma(paramsStr);
    
    size_t index = 0;
    for (const auto& part : parts) {
        std::string trimmed = trim(part);
        if (trimmed.empty()) continue;
        
        // البحث عن :
        size_t colonPos = trimmed.find(':');
        std::string paramName;
        std::string boundsStr;
        
        if (colonPos != std::string::npos) {
            paramName = trim(trimmed.substr(0, colonPos));
            boundsStr = trim(trimmed.substr(colonPos + 1));
        } else {
            paramName = trimmed;
        }
        
        auto param = std::make_shared<GenericParameter>(paramName, index++);
        
        if (!boundsStr.empty()) {
            TraitBound bounds = parseBounds(boundsStr);
            for (const auto& trait : bounds.getTraits()) {
                param->addTraitBound(trait);
            }
        }
        
        result.push_back(param);
    }
    
    return result;
}

TraitBound GenericsParser::parseBounds(const std::string& boundsStr) {
    TraitBound result;
    auto parts = splitByPlus(boundsStr);
    
    auto& registry = TraitRegistry::instance();
    
    for (const auto& part : parts) {
        std::string traitName = trim(part);
        if (traitName.empty()) continue;
        
        TraitPtr trait = registry.findTrait(traitName);
        if (trait) {
            result.addTrait(trait);
        }
    }
    
    return result;
}

std::vector<std::string> GenericsParser::splitByComma(const std::string& str) {
    std::vector<std::string> result;
    std::string current;
    int depth = 0;
    
    for (char c : str) {
        if (c == '<') {
            ++depth;
            current += c;
        } else if (c == '>') {
            --depth;
            current += c;
        } else if (c == ',' && depth == 0) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

std::vector<std::string> GenericsParser::splitByPlus(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, '+')) {
        result.push_back(item);
    }
    
    return result;
}

std::string GenericsParser::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();
    
    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }
    
    return str.substr(start, end - start);
}

// =============================================================================
//                    Monomorphizer Implementation
// =============================================================================

Monomorphizer& Monomorphizer::instance() {
    static Monomorphizer instance;
    return instance;
}

GenericInstantiationPtr Monomorphizer::getOrCreate(
    GenericTypePtr genericType,
    const std::vector<TypePtr>& typeArgs,
    TraitRegistry& registry)
{
    if (!genericType) return nullptr;
    
    // إنشاء مفتاح مؤقت للبحث
    auto tempInst = std::make_shared<GenericInstantiation>(genericType, typeArgs);
    std::string key = tempInst->getKey();
    
    // البحث عن تخصيص موجود
    auto it = instantiations_.find(key);
    if (it != instantiations_.end()) {
        return it->second;
    }
    
    // التحقق من القيود
    std::vector<std::string> errors;
    if (!genericType->validateTypeArgs(typeArgs, registry, errors)) {
        return nullptr;
    }
    
    // إنشاء وتخزين التخصيص
    instantiations_[key] = tempInst;
    return tempInst;
}

GenericInstantiationPtr Monomorphizer::find(const std::string& key) const {
    auto it = instantiations_.find(key);
    return (it != instantiations_.end()) ? it->second : nullptr;
}

void Monomorphizer::clear() {
    instantiations_.clear();
}

// =============================================================================
//                    Helper Functions Implementation
// =============================================================================

std::pair<std::string, std::vector<std::string>> 
extractTypeParameters(const std::string& typeName) 
{
    std::pair<std::string, std::vector<std::string>> result;
    
    size_t openBracket = typeName.find('<');
    if (openBracket == std::string::npos) {
        result.first = typeName;
        return result;
    }
    
    size_t closeBracket = typeName.rfind('>');
    if (closeBracket == std::string::npos || closeBracket <= openBracket) {
        result.first = typeName;
        return result;
    }
    
    result.first = typeName.substr(0, openBracket);
    
    std::string paramsStr = typeName.substr(
        openBracket + 1, 
        closeBracket - openBracket - 1
    );
    
    // تقسيم بالفاصلة مع مراعاة المعممات المتداخلة
    std::string current;
    int depth = 0;
    
    for (char c : paramsStr) {
        if (c == '<') {
            ++depth;
            current += c;
        } else if (c == '>') {
            --depth;
            current += c;
        } else if (c == ',' && depth == 0) {
            // تقليم المسافات
            size_t start = current.find_first_not_of(" \t");
            size_t end = current.find_last_not_of(" \t");
            if (start != std::string::npos) {
                result.second.push_back(current.substr(start, end - start + 1));
            }
            current.clear();
        } else {
            current += c;
        }
    }
    
    // إضافة آخر معامل
    size_t start = current.find_first_not_of(" \t");
    size_t end = current.find_last_not_of(" \t");
    if (start != std::string::npos) {
        result.second.push_back(current.substr(start, end - start + 1));
    }
    
    return result;
}

std::string buildInstantiatedTypeName(
    const std::string& baseName,
    const std::vector<std::string>& typeArgNames)
{
    if (typeArgNames.empty()) {
        return baseName;
    }
    
    std::stringstream ss;
    ss << baseName << "<";
    
    bool first = true;
    for (const auto& name : typeArgNames) {
        if (!first) ss << ", ";
        ss << name;
        first = false;
    }
    
    ss << ">";
    return ss.str();
}

bool isGenericTypeName(const std::string& typeName) {
    return typeName.find('<') != std::string::npos && 
           typeName.find('>') != std::string::npos;
}

} // namespace TypeSystem
} // namespace Sad
