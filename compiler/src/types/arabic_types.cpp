// Disable Unicode warning for Arabic strings
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file arabic_types.cpp
 * @brief Arabic Type System Implementation
 */

#include "types/arabic_types.h"
#include <algorithm>

namespace Sad {
namespace TypeSystem {

// =============================================================================
//                    Arabic Type Name Constants (UTF-8 encoded)
// =============================================================================

namespace ArabicTypeNames {
    // Unsigned integers
    const char* const HA8   = "\xd8\xad" "8";   // ح8
    const char* const HA16  = "\xd8\xad" "16";  // ح16
    const char* const HA32  = "\xd8\xad" "32";  // ح32
    const char* const HA64  = "\xd8\xad" "64";  // ح64
    
    // Signed integers
    const char* const AIN8  = "\xd8\xb9" "8";   // ع8
    const char* const AIN16 = "\xd8\xb9" "16";  // ع16
    const char* const AIN32 = "\xd8\xb9" "32";  // ع32
    const char* const AIN64 = "\xd8\xb9" "64";  // ع64
    const char* const RAQM  = "\xd8\xb1\xd9\x82\xd9\x85";  // رقم
    
    // Floating point
    const char* const AASHRI32 = "\xd8\xb9\xd8\xb4" "32";  // عش32
    const char* const AASHRI64 = "\xd8\xb9\xd8\xb4" "64";  // عش64
    const char* const ASHRI    = "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a";  // عشري
    
    // Other types
    const char* const MANTIQY = "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a";  // منطقي
    const char* const HARF    = "\xd8\xad\xd8\xb1\xd9\x81";  // حرف
    const char* const BYTE    = "\xd8\xa8\xd8\xa7\xd9\x8a\xd8\xaa";  // بايت
    const char* const FARAGH  = "\xd9\x81\xd8\xb1\xd8\xa7\xd8\xba";  // فراغ
    const char* const ABADAN  = "\xd8\xa3\xd8\xa8\xd8\xaf\xd8\xa7\xd9\x8b";  // أبداً
}

// =============================================================================
//                    PrimitiveTypeRegistry Implementation
// =============================================================================

PrimitiveTypeRegistry& PrimitiveTypeRegistry::instance() {
    static PrimitiveTypeRegistry registry;
    return registry;
}

PrimitiveTypeRegistry::PrimitiveTypeRegistry() {
    initializeTypes();
}

void PrimitiveTypeRegistry::initializeTypes() {
    types_.clear();
    arabicNameIndex_.clear();
    englishNameIndex_.clear();
    
    // Reserve space for all types
    types_.reserve(static_cast<size_t>(PrimitiveKind::COUNT));
    
    // Helper lambda to add a type
    auto addType = [this](PrimitiveKind kind, const char* arabic, const char* english,
                          size_t size, bool isSigned, bool isInt, bool isFloat) {
        PrimitiveTypeInfo info;
        info.kind = kind;
        info.arabicName = arabic;
        info.englishName = english;
        info.sizeInBytes = size;
        info.alignment = size > 0 ? size : 1;
        info.isSigned = isSigned;
        info.isInteger = isInt;
        info.isFloating = isFloat;
        info.isNumeric = isInt || isFloat;
        
        size_t index = types_.size();
        types_.push_back(info);
        
        arabicNameIndex_[arabic] = index;
        englishNameIndex_[english] = index;
    };
    
    // Void type
    addType(PrimitiveKind::Void, ArabicTypeNames::FARAGH, "void", 0, false, false, false);
    
    // Never type
    addType(PrimitiveKind::Never, ArabicTypeNames::ABADAN, "never", 0, false, false, false);
    
    // Boolean
    addType(PrimitiveKind::Bool, ArabicTypeNames::MANTIQY, "bool", 1, false, false, false);
    
    // Character
    addType(PrimitiveKind::Char, ArabicTypeNames::HARF, "char", 4, false, false, false);
    
    // Byte
    addType(PrimitiveKind::Byte, ArabicTypeNames::BYTE, "byte", 1, false, true, false);
    
    // Unsigned integers
    addType(PrimitiveKind::UInt8,  ArabicTypeNames::HA8,  "u8",  1, false, true, false);
    addType(PrimitiveKind::UInt16, ArabicTypeNames::HA16, "u16", 2, false, true, false);
    addType(PrimitiveKind::UInt32, ArabicTypeNames::HA32, "u32", 4, false, true, false);
    addType(PrimitiveKind::UInt64, ArabicTypeNames::HA64, "u64", 8, false, true, false);
    
    // Signed integers
    addType(PrimitiveKind::Int8,  ArabicTypeNames::AIN8,  "i8",  1, true, true, false);
    addType(PrimitiveKind::Int16, ArabicTypeNames::AIN16, "i16", 2, true, true, false);
    addType(PrimitiveKind::Int32, ArabicTypeNames::AIN32, "i32", 4, true, true, false);
    addType(PrimitiveKind::Int64, ArabicTypeNames::AIN64, "i64", 8, true, true, false);
    
    // Also add "raqm" as alias for i64
    arabicNameIndex_[ArabicTypeNames::RAQM] = englishNameIndex_["i64"];
    englishNameIndex_["int"] = englishNameIndex_["i64"];
    
    // Floating point
    addType(PrimitiveKind::Float32, ArabicTypeNames::AASHRI32, "f32", 4, true, false, true);
    addType(PrimitiveKind::Float64, ArabicTypeNames::AASHRI64, "f64", 8, true, false, true);
    
    // Also add "ashri" as alias for f64
    arabicNameIndex_[ArabicTypeNames::ASHRI] = englishNameIndex_["f64"];
    englishNameIndex_["float"] = englishNameIndex_["f32"];
    englishNameIndex_["double"] = englishNameIndex_["f64"];
}

const PrimitiveTypeInfo* PrimitiveTypeRegistry::getInfo(PrimitiveKind kind) const {
    for (const auto& info : types_) {
        if (info.kind == kind) {
            return &info;
        }
    }
    return nullptr;
}

const PrimitiveTypeInfo* PrimitiveTypeRegistry::getInfoByArabicName(const std::string& name) const {
    auto it = arabicNameIndex_.find(name);
    if (it != arabicNameIndex_.end() && it->second < types_.size()) {
        return &types_[it->second];
    }
    return nullptr;
}

const PrimitiveTypeInfo* PrimitiveTypeRegistry::getInfoByEnglishName(const std::string& name) const {
    auto it = englishNameIndex_.find(name);
    if (it != englishNameIndex_.end() && it->second < types_.size()) {
        return &types_[it->second];
    }
    return nullptr;
}

bool PrimitiveTypeRegistry::isPrimitiveName(const std::string& name) const {
    return arabicNameIndex_.count(name) > 0 || englishNameIndex_.count(name) > 0;
}

PrimitiveKind PrimitiveTypeRegistry::getKindByName(const std::string& name) const {
    auto it = arabicNameIndex_.find(name);
    if (it != arabicNameIndex_.end() && it->second < types_.size()) {
        return types_[it->second].kind;
    }
    
    it = englishNameIndex_.find(name);
    if (it != englishNameIndex_.end() && it->second < types_.size()) {
        return types_[it->second].kind;
    }
    
    return PrimitiveKind::Unknown;
}

std::string PrimitiveTypeRegistry::primitiveKindToArabic(PrimitiveKind kind) {
    const auto* info = instance().getInfo(kind);
    return info ? info->arabicName : "";
}

std::string PrimitiveTypeRegistry::primitiveKindToEnglish(PrimitiveKind kind) {
    const auto* info = instance().getInfo(kind);
    return info ? info->englishName : "";
}

size_t PrimitiveTypeRegistry::primitiveKindSizeInBytes(PrimitiveKind kind) {
    const auto* info = instance().getInfo(kind);
    return info ? info->sizeInBytes : 0;
}

bool PrimitiveTypeRegistry::isImplicitConversionSafe(PrimitiveKind from, PrimitiveKind to) {
    if (from == to) return true;
    
    const auto* fromInfo = instance().getInfo(from);
    const auto* toInfo = instance().getInfo(to);
    
    if (!fromInfo || !toInfo) return false;
    
    // Same category (both int or both float)
    if (fromInfo->isInteger && toInfo->isInteger) {
        // Widening is always safe
        if (toInfo->sizeInBytes > fromInfo->sizeInBytes) {
            // Unsigned to signed of larger size is safe
            if (!fromInfo->isSigned && toInfo->isSigned) {
                return toInfo->sizeInBytes > fromInfo->sizeInBytes;
            }
            // Same signedness widening is safe
            return fromInfo->isSigned == toInfo->isSigned;
        }
    }
    
    if (fromInfo->isFloating && toInfo->isFloating) {
        // f32 -> f64 is safe
        return toInfo->sizeInBytes >= fromInfo->sizeInBytes;
    }
    
    // Integer to float is generally safe for small integers
    if (fromInfo->isInteger && toInfo->isFloating) {
        // i32/u32 -> f64 is safe
        // i64/u64 -> f64 may lose precision
        if (toInfo->kind == PrimitiveKind::Float64) {
            return fromInfo->sizeInBytes <= 4;
        }
        if (toInfo->kind == PrimitiveKind::Float32) {
            return fromInfo->sizeInBytes <= 2;
        }
    }
    
    return false;
}

PrimitiveKind PrimitiveTypeRegistry::getWiderType(PrimitiveKind a, PrimitiveKind b) {
    if (a == b) return a;
    
    const auto* aInfo = instance().getInfo(a);
    const auto* bInfo = instance().getInfo(b);
    
    if (!aInfo || !bInfo) return PrimitiveKind::Unknown;
    
    // Float takes precedence over integer
    if (aInfo->isFloating && !bInfo->isFloating) return a;
    if (bInfo->isFloating && !aInfo->isFloating) return b;
    
    // Both floating - return wider
    if (aInfo->isFloating && bInfo->isFloating) {
        return aInfo->sizeInBytes >= bInfo->sizeInBytes ? a : b;
    }
    
    // Both integers
    if (aInfo->isInteger && bInfo->isInteger) {
        // If both same signedness, return wider
        if (aInfo->isSigned == bInfo->isSigned) {
            return aInfo->sizeInBytes >= bInfo->sizeInBytes ? a : b;
        }
        
        // Mixed signedness - prefer signed if it can hold both ranges
        if (aInfo->isSigned && aInfo->sizeInBytes > bInfo->sizeInBytes) return a;
        if (bInfo->isSigned && bInfo->sizeInBytes > aInfo->sizeInBytes) return b;
        
        // Otherwise widen to next signed type
        size_t maxSize = std::max(aInfo->sizeInBytes, bInfo->sizeInBytes);
        if (maxSize < 8) {
            // Widen to signed type of double size
            switch (maxSize * 2) {
                case 2: return PrimitiveKind::Int16;
                case 4: return PrimitiveKind::Int32;
                case 8: return PrimitiveKind::Int64;
            }
        }
        
        // Fall back to i64
        return PrimitiveKind::Int64;
    }
    
    return PrimitiveKind::Unknown;
}

} // namespace TypeSystem
} // namespace Sad
