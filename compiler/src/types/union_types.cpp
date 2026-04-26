// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file union_types.cpp
 * @brief Arabic Union Type System Implementation
 */

#include "types/union_types.h"
#include <algorithm>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    Arabic Union Keywords (UTF-8 encoded)
// =============================================================================

namespace ArabicUnionKeywords {
    const char* const ITIHAD = "\xd8\xa7\xd8\xaa\xd8\xad\xd8\xa7\xd8\xaf"; // اتحاد
}

// =============================================================================
//                    UnionField Implementation
// =============================================================================

UnionField::UnionField(const std::string& name, TypePtr type, size_t index)
    : name_(name)
    , type_(type)
    , index_(index)
{
}

size_t UnionField::getSizeInBytes() const {
    // In future: get actual type size
    return 8;  // Default to 8 bytes
}

// =============================================================================
//                    UnionType Implementation
// =============================================================================

size_t UnionType::nextId_ = 0;

UnionType::UnionType(const std::string& arabicName, const std::string& englishName)
    : arabicName_(arabicName)
    , englishName_(englishName)
    , id_(nextId_++)
{
}

std::string UnionType::getName(bool arabic) const {
    return arabic ? arabicName_ : englishName_;
}

void UnionType::addField(const std::string& name, TypePtr type) {
    auto field = std::make_shared<UnionField>(name, type, fields_.size());
    fields_.push_back(field);
}

UnionFieldPtr UnionType::findField(const std::string& name) const {
    for (const auto& field : fields_) {
        if (field->getName() == name) {
            return field;
        }
    }
    return nullptr;
}

size_t UnionType::getSizeInBytes() const {
    size_t maxSize = 0;
    for (const auto& field : fields_) {
        maxSize = std::max(maxSize, field->getSizeInBytes());
    }
    return maxSize;
}

size_t UnionType::getAlignment() const {
    size_t maxAlign = 1;
    for (const auto& field : fields_) {
        // In future: get actual type alignment
        maxAlign = std::max(maxAlign, size_t(8));
    }
    return maxAlign;
}

// =============================================================================
//                    UnionRegistry Implementation
// =============================================================================

UnionRegistry& UnionRegistry::instance() {
    static UnionRegistry registry;
    return registry;
}

void UnionRegistry::registerUnion(UnionTypePtr unionType) {
    if (!unionType) return;
    
    unionsByName_[unionType->getArabicName()] = unionType;
    unionsByName_[unionType->getEnglishName()] = unionType;
}

UnionTypePtr UnionRegistry::findUnion(const std::string& name) const {
    auto it = unionsByName_.find(name);
    return (it != unionsByName_.end()) ? it->second : nullptr;
}

std::vector<UnionTypePtr> UnionRegistry::getAllUnions() const {
    std::vector<UnionTypePtr> result;
    std::unordered_set<size_t> seen;
    
    for (const auto& pair : unionsByName_) {
        if (seen.insert(pair.second->getId()).second) {
            result.push_back(pair.second);
        }
    }
    
    return result;
}

bool UnionRegistry::isUnionType(const std::string& name) const {
    return unionsByName_.count(name) > 0;
}

// =============================================================================
//                    UnionBuilder Implementation
// =============================================================================

UnionBuilder::UnionBuilder(const std::string& arabicName, const std::string& englishName)
    : union_(std::make_shared<UnionType>(arabicName, englishName))
{
}

UnionBuilder& UnionBuilder::field(const std::string& name, TypePtr type) {
    union_->addField(name, type);
    return *this;
}

UnionTypePtr UnionBuilder::build() {
    UnionRegistry::instance().registerUnion(union_);
    return union_;
}

} // namespace TypeSystem
} // namespace Sad
