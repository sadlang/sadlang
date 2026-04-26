// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file enum_types.cpp
 * @brief Arabic Enum Type System Implementation
 */

#include "types/enum_types.h"
#include <algorithm>
#include <sstream>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    Arabic Enum Keywords (UTF-8 encoded)
// =============================================================================

namespace ArabicEnumKeywords {
    const char* const TA3DAD    = "\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf";     // تعداد
    const char* const WUHDA     = "\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9";             // وحدة
    const char* const QEEMA     = "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9";             // قيمة
    const char* const IKHTYARI  = "\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1\xd9\x8a"; // اختياري
    const char* const LA_SHAY   = "\xd9\x84\xd8\xa7_\xd8\xb4\xd9\x8a\xd8\xa1";    // لا_شيء
    const char* const BA3D      = "\xd8\xa8\xd8\xb9\xd8\xb6";                      // بعض
    const char* const NATEEJA   = "\xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9";     // نتيجة
    const char* const NAJAH     = "\xd9\x86\xd8\xac\xd8\xa7\xd8\xad";             // نجاح
    const char* const KHATA     = "\xd8\xae\xd8\xb7\xd8\xa3";                      // خطأ
}

// =============================================================================
//                    EnumVariant Implementation
// =============================================================================

EnumVariant::EnumVariant(const std::string& name, VariantKind kind, int64_t discriminant)
    : name_(name)
    , kind_(kind)
    , discriminant_(discriminant)
{
}

EnumVariantPtr EnumVariant::createUnit(const std::string& name, int64_t discriminant) {
    return std::shared_ptr<EnumVariant>(
        new EnumVariant(name, VariantKind::Unit, discriminant));
}

EnumVariantPtr EnumVariant::createTuple(const std::string& name, int64_t discriminant,
                                         const std::vector<TypePtr>& fieldTypes) {
    auto variant = std::shared_ptr<EnumVariant>(
        new EnumVariant(name, VariantKind::Tuple, discriminant));
    variant->tupleTypes_ = fieldTypes;
    return variant;
}

EnumVariantPtr EnumVariant::createStruct(const std::string& name, int64_t discriminant,
                                          const std::vector<std::pair<std::string, TypePtr>>& fields) {
    auto variant = std::shared_ptr<EnumVariant>(
        new EnumVariant(name, VariantKind::Struct, discriminant));
    variant->structFields_ = fields;
    return variant;
}

size_t EnumVariant::getDataSize() const {
    switch (kind_) {
        case VariantKind::Unit:
            return 0;
        case VariantKind::Tuple: {
            size_t size = 0;
            for (const auto& type : tupleTypes_) {
                // In future: get actual type size
                size += 8;
            }
            return size;
        }
        case VariantKind::Struct: {
            size_t size = 0;
            for (const auto& field : structFields_) {
                // In future: get actual type size
                size += 8;
            }
            return size;
        }
    }
    return 0;
}

// =============================================================================
//                    EnumType Implementation
// =============================================================================

size_t EnumType::nextId_ = 0;

EnumType::EnumType(const std::string& arabicName, const std::string& englishName)
    : arabicName_(arabicName)
    , englishName_(englishName)
    , id_(nextId_++)
{
}

std::string EnumType::getName(bool arabic) const {
    std::string name = arabic ? arabicName_ : englishName_;
    
    if (!typeParameters_.empty()) {
        std::stringstream ss;
        ss << name << "<";
        bool first = true;
        for (const auto& param : typeParameters_) {
            if (!first) ss << ", ";
            ss << param->getName();
            first = false;
        }
        ss << ">";
        return ss.str();
    }
    
    return name;
}

void EnumType::addVariant(EnumVariantPtr variant) {
    if (variant) {
        variants_.push_back(variant);
    }
}

void EnumType::addUnitVariant(const std::string& name) {
    auto variant = EnumVariant::createUnit(name, nextDiscriminant_++);
    variants_.push_back(variant);
}

void EnumType::addTupleVariant(const std::string& name, const std::vector<TypePtr>& types) {
    auto variant = EnumVariant::createTuple(name, nextDiscriminant_++, types);
    variants_.push_back(variant);
}

void EnumType::addStructVariant(const std::string& name,
                                 const std::vector<std::pair<std::string, TypePtr>>& fields) {
    auto variant = EnumVariant::createStruct(name, nextDiscriminant_++, fields);
    variants_.push_back(variant);
}

EnumVariantPtr EnumType::findVariant(const std::string& name) const {
    for (const auto& variant : variants_) {
        if (variant->getName() == name) {
            return variant;
        }
    }
    return nullptr;
}

void EnumType::addTypeParameter(GenericParameterPtr param) {
    typeParameters_.push_back(param);
}

EnumTypePtr EnumType::instantiate(const std::vector<TypePtr>& typeArgs) const {
    if (typeArgs.size() != typeParameters_.size()) {
        return nullptr;
    }
    
    // Create instantiated name
    std::stringstream arabicSS, englishSS;
    arabicSS << arabicName_ << "<";
    englishSS << englishName_ << "<";
    
    bool first = true;
    for (size_t i = 0; i < typeArgs.size(); ++i) {
        if (!first) {
            arabicSS << ", ";
            englishSS << ", ";
        }
        // In future: use typeArgs[i]->getName()
        arabicSS << "T" << i;
        englishSS << "T" << i;
        first = false;
    }
    
    arabicSS << ">";
    englishSS << ">";
    
    auto instantiated = std::make_shared<EnumType>(arabicSS.str(), englishSS.str());
    
    // Copy variants (in future: substitute type parameters)
    for (const auto& variant : variants_) {
        instantiated->addVariant(variant);
    }
    
    instantiated->setRepr(repr_);
    
    return instantiated;
}

bool EnumType::isCLike() const {
    for (const auto& variant : variants_) {
        if (variant->hasData()) {
            return false;
        }
    }
    return true;
}

bool EnumType::isOptionLike() const {
    if (variants_.size() != 2) return false;
    
    bool hasNone = false;
    bool hasSome = false;
    
    for (const auto& variant : variants_) {
        if (variant->getKind() == VariantKind::Unit) {
            hasNone = true;
        } else if (variant->getKind() == VariantKind::Tuple &&
                   variant->getTupleTypes().size() == 1) {
            hasSome = true;
        }
    }
    
    return hasNone && hasSome;
}

bool EnumType::isResultLike() const {
    if (variants_.size() != 2) return false;
    
    bool hasOk = false;
    bool hasErr = false;
    
    for (const auto& variant : variants_) {
        if (variant->getKind() == VariantKind::Tuple &&
            variant->getTupleTypes().size() == 1) {
            if (!hasOk) hasOk = true;
            else hasErr = true;
        }
    }
    
    return hasOk && hasErr;
}

size_t EnumType::getSizeInBytes() const {
    if (variants_.empty()) return 0;
    
    // Size = discriminant + max variant data size
    size_t maxDataSize = 0;
    for (const auto& variant : variants_) {
        maxDataSize = std::max(maxDataSize, variant->getDataSize());
    }
    
    return getDiscriminantSize() + maxDataSize;
}

size_t EnumType::getDiscriminantSize() const {
    size_t variantCount = variants_.size();
    
    switch (repr_) {
        case EnumRepr::I8:
        case EnumRepr::U8:
            return 1;
        case EnumRepr::I16:
        case EnumRepr::U16:
            return 2;
        case EnumRepr::I32:
        case EnumRepr::U32:
            return 4;
        case EnumRepr::I64:
        case EnumRepr::U64:
            return 8;
        case EnumRepr::Auto:
            // Choose smallest size that fits
            if (variantCount <= 256) return 1;
            if (variantCount <= 65536) return 2;
            return 4;
    }
    
    return 4;
}

// =============================================================================
//                    EnumRegistry Implementation
// =============================================================================

EnumRegistry& EnumRegistry::instance() {
    static EnumRegistry registry;
    return registry;
}

EnumRegistry::EnumRegistry() {
    initializeCoreEnums();
}

void EnumRegistry::registerEnum(EnumTypePtr enumType) {
    if (!enumType) return;
    
    enumsByName_[enumType->getArabicName()] = enumType;
    enumsByName_[enumType->getEnglishName()] = enumType;
}

EnumTypePtr EnumRegistry::findEnum(const std::string& name) const {
    auto it = enumsByName_.find(name);
    return (it != enumsByName_.end()) ? it->second : nullptr;
}

std::vector<EnumTypePtr> EnumRegistry::getAllEnums() const {
    std::vector<EnumTypePtr> result;
    std::unordered_set<size_t> seen;
    
    for (const auto& pair : enumsByName_) {
        if (seen.insert(pair.second->getId()).second) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

bool EnumRegistry::isEnumType(const std::string& name) const {
    return enumsByName_.count(name) > 0;
}

void EnumRegistry::initializeCoreEnums() {
    optionType_ = createOptionType();
    resultType_ = createResultType();
    
    registerEnum(optionType_);
    registerEnum(resultType_);
}

// =============================================================================
//                    EnumBuilder Implementation
// =============================================================================

EnumBuilder::EnumBuilder(const std::string& arabicName, const std::string& englishName)
    : enum_(std::make_shared<EnumType>(arabicName, englishName))
{
}

EnumBuilder& EnumBuilder::unit(const std::string& name) {
    enum_->addUnitVariant(name);
    return *this;
}

EnumBuilder& EnumBuilder::tuple(const std::string& name, std::initializer_list<TypePtr> types) {
    enum_->addTupleVariant(name, std::vector<TypePtr>(types));
    return *this;
}

EnumBuilder& EnumBuilder::structVariant(const std::string& name,
                                         std::initializer_list<std::pair<std::string, TypePtr>> fields) {
    enum_->addStructVariant(name, std::vector<std::pair<std::string, TypePtr>>(fields));
    return *this;
}

EnumBuilder& EnumBuilder::generic(const std::string& paramName) {
    auto param = std::make_shared<GenericParameter>(paramName, genericIndex_++);
    enum_->addTypeParameter(param);
    return *this;
}

EnumBuilder& EnumBuilder::repr(EnumRepr r) {
    enum_->setRepr(r);
    return *this;
}

EnumTypePtr EnumBuilder::build() {
    EnumRegistry::instance().registerEnum(enum_);
    return enum_;
}

// =============================================================================
//                    Helper Functions
// =============================================================================

EnumTypePtr createOptionType() {
    auto option = std::make_shared<EnumType>(
        ArabicEnumKeywords::IKHTYARI,  // اختياري
        "Option"
    );
    
    // Add generic parameter T
    auto paramT = std::make_shared<GenericParameter>("T", 0);
    option->addTypeParameter(paramT);
    
    // Add variants: None, Some(T)
    option->addUnitVariant(ArabicEnumKeywords::LA_SHAY);  // لا_شيء
    option->addTupleVariant(ArabicEnumKeywords::BA3D, {nullptr});  // بعض(T)
    
    return option;
}

EnumTypePtr createResultType() {
    auto result = std::make_shared<EnumType>(
        ArabicEnumKeywords::NATEEJA,  // نتيجة
        "Result"
    );
    
    // Add generic parameters T, E
    auto paramT = std::make_shared<GenericParameter>("T", 0);
    auto paramE = std::make_shared<GenericParameter>("E", 1);
    result->addTypeParameter(paramT);
    result->addTypeParameter(paramE);
    
    // Add variants: Ok(T), Err(E)
    result->addTupleVariant(ArabicEnumKeywords::NAJAH, {nullptr});  // نجاح(T)
    result->addTupleVariant(ArabicEnumKeywords::KHATA, {nullptr});  // خطأ(E)
    
    return result;
}

} // namespace TypeSystem
} // namespace Sad
