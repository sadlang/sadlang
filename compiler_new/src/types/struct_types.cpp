// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file struct_types.cpp
 * @brief Arabic Struct Type System Implementation
 */

#include "types/struct_types.h"
#include <algorithm>
#include <sstream>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    Arabic Struct Keywords (UTF-8 encoded)
// =============================================================================

namespace ArabicStructKeywords {
    const char* const BUNYA     = "\xd8\xa8\xd9\x86\xd9\x8a\xd8\xa9";     // بنية
    const char* const HEIKAL    = "\xd9\x87\xd9\x8a\xd9\x83\xd9\x84";     // هيكل
    const char* const HAQLA     = "\xd8\xad\xd9\x82\xd9\x84";             // حقل
    const char* const AAM       = "\xd8\xb9\xd8\xa7\xd9\x85";             // عام
    const char* const KHASS     = "\xd8\xae\xd8\xa7\xd8\xb5";             // خاص
    const char* const MUTAHAREK = "\xd9\x85\xd8\xaa\xd8\xad\xd8\xb1\xd9\x83"; // متحرك
    const char* const THABET    = "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa";     // ثابت
}

// =============================================================================
//                    StructField Implementation
// =============================================================================

StructField::StructField(const std::string& name, TypePtr type, size_t index)
    : name_(name)
    , type_(type)
    , index_(index)
{
}

// =============================================================================
//                    StructType Implementation
// =============================================================================

size_t StructType::nextId_ = 0;

StructType::StructType(const std::string& arabicName, const std::string& englishName)
    : arabicName_(arabicName)
    , englishName_(englishName)
    , id_(nextId_++)
{
}

std::string StructType::getName(bool arabic) const {
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

void StructType::addField(const std::string& name, TypePtr type) {
    auto field = std::make_shared<StructField>(name, type, fields_.size());
    fields_.push_back(field);
    layoutCalculated_ = false;
}

StructFieldPtr StructType::findField(const std::string& name) const {
    for (const auto& field : fields_) {
        if (field->getName() == name) {
            return field;
        }
    }
    return nullptr;
}

bool StructType::hasField(const std::string& name) const {
    return findField(name) != nullptr;
}

void StructType::addTypeParameter(GenericParameterPtr param) {
    typeParameters_.push_back(param);
}

StructTypePtr StructType::instantiate(const std::vector<TypePtr>& typeArgs) const {
    if (typeArgs.size() != typeParameters_.size()) {
        return nullptr;
    }
    
    // Create instantiated name
    std::stringstream arabicSS, englishSS;
    arabicSS << arabicName_ << "<";
    englishSS << englishName_ << "<";
    
    bool first = true;
    for (const auto& arg : typeArgs) {
        if (!first) {
            arabicSS << ", ";
            englishSS << ", ";
        }
        // In future: use arg->getName()
        arabicSS << "T";
        englishSS << "T";
        first = false;
    }
    
    arabicSS << ">";
    englishSS << ">";
    
    auto instantiated = std::make_shared<StructType>(arabicSS.str(), englishSS.str());
    
    // Copy and substitute fields
    // In future: perform type substitution
    for (const auto& field : fields_) {
        instantiated->addField(field->getName(), field->getType());
    }
    
    instantiated->setLayout(layout_);
    instantiated->setCopy(isCopy_);
    instantiated->setTuple(isTuple_);
    
    // Copy traits
    for (const auto& trait : traits_) {
        instantiated->addTrait(trait);
    }
    
    return instantiated;
}

void StructType::addTrait(TraitPtr trait) {
    if (trait) {
        traits_.push_back(trait);
    }
}

bool StructType::implementsTrait(TraitPtr trait) const {
    if (!trait) return false;
    
    for (const auto& t : traits_) {
        if (t == trait || t->extendsTraitTransitive(trait.get())) {
            return true;
        }
    }
    return false;
}

size_t StructType::getSizeInBytes() const {
    if (!layoutCalculated_) {
        const_cast<StructType*>(this)->calculateLayout();
    }
    return cachedSize_;
}

size_t StructType::getAlignment() const {
    if (fields_.empty()) return 1;
    
    size_t maxAlign = 1;
    for (const auto& field : fields_) {
        // In future: get actual type alignment
        size_t fieldAlign = 8; // Default to 8 bytes
        maxAlign = std::max(maxAlign, fieldAlign);
    }
    
    switch (layout_) {
        case StructLayout::Packed:
            return 1;
        case StructLayout::C:
        case StructLayout::Auto:
            return maxAlign;
        case StructLayout::Repr32:
            return 4;
        case StructLayout::Repr64:
            return 8;
    }
    
    return maxAlign;
}

void StructType::calculateLayout() {
    if (fields_.empty()) {
        cachedSize_ = 0;
        layoutCalculated_ = true;
        return;
    }
    
    size_t currentOffset = 0;
    size_t maxAlign = getAlignment();
    
    for (auto& field : fields_) {
        // Get field size and alignment
        // In future: get from type system
        size_t fieldSize = 8;  // Default
        size_t fieldAlign = 8;
        
        if (layout_ != StructLayout::Packed) {
            // Apply alignment
            size_t padding = (fieldAlign - (currentOffset % fieldAlign)) % fieldAlign;
            currentOffset += padding;
        }
        
        field->setOffset(currentOffset);
        currentOffset += fieldSize;
    }
    
    // Final padding for array alignment
    if (layout_ != StructLayout::Packed) {
        size_t padding = (maxAlign - (currentOffset % maxAlign)) % maxAlign;
        currentOffset += padding;
    }
    
    cachedSize_ = currentOffset;
    layoutCalculated_ = true;
}

// =============================================================================
//                    StructRegistry Implementation
// =============================================================================

StructRegistry& StructRegistry::instance() {
    static StructRegistry registry;
    return registry;
}

void StructRegistry::registerStruct(StructTypePtr structType) {
    if (!structType) return;
    
    structsByName_[structType->getArabicName()] = structType;
    structsByName_[structType->getEnglishName()] = structType;
}

StructTypePtr StructRegistry::findStruct(const std::string& name) const {
    auto it = structsByName_.find(name);
    return (it != structsByName_.end()) ? it->second : nullptr;
}

std::vector<StructTypePtr> StructRegistry::getAllStructs() const {
    std::vector<StructTypePtr> result;
    std::unordered_set<size_t> seen;
    
    for (const auto& pair : structsByName_) {
        if (seen.insert(pair.second->getId()).second) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

bool StructRegistry::isStructType(const std::string& name) const {
    return structsByName_.count(name) > 0;
}

StructTypePtr StructRegistry::getOrCreateInstantiation(
    StructTypePtr genericStruct,
    const std::vector<TypePtr>& typeArgs)
{
    if (!genericStruct || !genericStruct->isGeneric()) {
        return genericStruct;
    }
    
    // Build key
    std::stringstream key;
    key << genericStruct->getId() << "<";
    bool first = true;
    for (const auto& arg : typeArgs) {
        if (!first) key << ",";
        key << reinterpret_cast<uintptr_t>(arg.get());
        first = false;
    }
    key << ">";
    
    std::string keyStr = key.str();
    
    // Check cache
    auto it = instantiations_.find(keyStr);
    if (it != instantiations_.end()) {
        return it->second;
    }
    
    // Create instantiation
    auto instantiated = genericStruct->instantiate(typeArgs);
    if (instantiated) {
        instantiations_[keyStr] = instantiated;
    }
    
    return instantiated;
}

void StructRegistry::clearInstantiationCache() {
    instantiations_.clear();
}

// =============================================================================
//                    StructBuilder Implementation
// =============================================================================

StructBuilder::StructBuilder(const std::string& arabicName, const std::string& englishName)
    : struct_(std::make_shared<StructType>(arabicName, englishName))
{
}

StructBuilder& StructBuilder::field(const std::string& name, TypePtr type) {
    struct_->addField(name, type);
    return *this;
}

StructBuilder& StructBuilder::generic(const std::string& paramName) {
    auto param = std::make_shared<GenericParameter>(paramName, genericIndex_++);
    struct_->addTypeParameter(param);
    return *this;
}

StructBuilder& StructBuilder::genericBounded(const std::string& paramName, TraitPtr bound) {
    auto param = std::make_shared<GenericParameter>(paramName, genericIndex_++);
    if (bound) {
        param->addTraitBound(bound);
    }
    struct_->addTypeParameter(param);
    return *this;
}

StructBuilder& StructBuilder::layout(StructLayout layout) {
    struct_->setLayout(layout);
    return *this;
}

StructBuilder& StructBuilder::copy() {
    struct_->setCopy(true);
    return *this;
}

StructBuilder& StructBuilder::tuple() {
    struct_->setTuple(true);
    return *this;
}

StructTypePtr StructBuilder::build() {
    struct_->calculateLayout();
    StructRegistry::instance().registerStruct(struct_);
    return struct_;
}

} // namespace TypeSystem
} // namespace Sad
