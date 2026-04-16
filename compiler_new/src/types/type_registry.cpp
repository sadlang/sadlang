// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// …„: type_registry.cpp
// File: type_registry.cpp
//
// ״§„ˆ״µ: ״×†״° ״³״¬„ …״±ƒ״² „״¬…״¹ ״§„״£†ˆ״§״¹
// Description: Implementation of central registry for all types
//
// ״§„…״₪„: Sad Language Type System
// Author: Sad Language Type System
//
// ״§„״×״§״±״®: 2 †״§״± 2026
// Date: January 2, 2026
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

#include "types/type_registry.h" // ״§״³״×״±״§״¯ ״§„״×״¹״± / Import definition
#include <functional>            // „€ std::hash / For std::hash

namespace Sad
{
    namespace TypeSystem
    {

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° Singleton Pattern
        // Singleton Pattern Implementation
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // ״§„״­״µˆ„ ״¹„‰ ״§„…״«„ ״§„ˆ״­״¯ / Get singleton instance
        TypeRegistry &TypeRegistry::getInstance()
        {
            // static …״­„ thread-safe  C++11+ / Local static is thread-safe in C++11+
            static TypeRegistry instance;

            // ״¥״±״¬״§״¹ ״§„…״«„ / Return instance
            return instance;
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° ״§„…†״´״¦ / Constructor Implementation
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // ״§„…†״´״¦ ״§„״®״§״µ / Private constructor
        TypeRegistry::TypeRegistry()
        {
            // ״×‡״¦״© ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© / Initialize primitive types
            initializePrimitiveTypes();
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° ״§„״×‡״¦״© / Initialization Implementation
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // ״×‡״¦״© cache ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© / Initialize primitive type cache
        void TypeRegistry::initializePrimitiveTypes()
        {
            // ״¥†״´״§״¡ ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© / Create primitive types
            void_type_ = std::make_shared<PrimitiveType>(SadTypeKind::Void);       // ״±״§״÷
            integer_type_ = std::make_shared<PrimitiveType>(SadTypeKind::Integer); // ״±‚…
            float_type_ = std::make_shared<PrimitiveType>(SadTypeKind::Float);     // ״¹״´״±
            boolean_type_ = std::make_shared<PrimitiveType>(SadTypeKind::Boolean); // …†״·‚
            string_type_ = std::make_shared<PrimitiveType>(SadTypeKind::String);   // †״µ

            // ״¥†״´״§״¡ ״§„״£†ˆ״§״¹ ״§„״®״§״µ״© / Create special types
            any_type_ = std::make_shared<PrimitiveType>(SadTypeKind::Any);         // ״£
            never_type_ = std::make_shared<PrimitiveType>(SadTypeKind::Never);     // ״£״¨״¯״§‹
            unknown_type_ = std::make_shared<PrimitiveType>(SadTypeKind::Unknown); // …״¬‡ˆ„

            // ״¥״¶״§״© ״§„״£†ˆ״§״¹ ״¥„‰ ״§„״®״±״·״© / Add types to map
            type_map_[hashType(void_type_)] = void_type_;
            type_map_[hashType(integer_type_)] = integer_type_;
            type_map_[hashType(float_type_)] = float_type_;
            type_map_[hashType(boolean_type_)] = boolean_type_;
            type_map_[hashType(string_type_)] = string_type_;
            type_map_[hashType(any_type_)] = any_type_;
            type_map_[hashType(never_type_)] = never_type_;
            type_map_[hashType(unknown_type_)] = unknown_type_;
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° ״§„״­״µˆ„ ״¹„‰ ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© / Primitive Type Getters Implementation
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ ״±״§״÷ / Get Void type
        TypePtr TypeRegistry::getVoidType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return void_type_;
        }

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ ״±‚… / Get Integer type
        TypePtr TypeRegistry::getIntegerType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return integer_type_;
        }

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ ״¹״´״± / Get Float type
        TypePtr TypeRegistry::getFloatType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return float_type_;
        }

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ …†״·‚ / Get Boolean type
        TypePtr TypeRegistry::getBooleanType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return boolean_type_;
        }

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ †״µ / Get String type
        TypePtr TypeRegistry::getStringType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return string_type_;
        }

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ Any / Get Any type
        TypePtr TypeRegistry::getAnyType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return any_type_;
        }

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ Never / Get Never type
        TypePtr TypeRegistry::getNeverType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return never_type_;
        }

        // ״§„״­״µˆ„ ״¹„‰ †ˆ״¹ Unknown / Get Unknown type
        TypePtr TypeRegistry::getUnknownType()
        {
            // ״¥״±״¬״§״¹ …† cache / Return from cache
            return unknown_type_;
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° Type Interning
        // Type Interning Implementation
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // ״§„״­״µˆ„ ״¹„‰ ״£ˆ ״¥†״´״§״¡ †ˆ״¹ ״¨״¯״§״¦ / Get or create primitive type
        TypePtr TypeRegistry::internPrimitiveType(SadTypeKind kind)
        {
            // ״§״³״×״®״¯״§… switch „„״£†ˆ״§״¹ ״§„…״®״×„״© / Use switch for different kinds
            switch (kind)
            {
            case SadTypeKind::Void:
                return getVoidType(); // ״±״§״÷
            case SadTypeKind::Integer:
                return getIntegerType(); // ״±‚…
            case SadTypeKind::Float:
                return getFloatType(); // ״¹״´״±
            case SadTypeKind::Boolean:
                return getBooleanType(); // …†״·‚
            case SadTypeKind::String:
                return getStringType(); // †״µ
            case SadTypeKind::Any:
                return getAnyType(); // ״£
            case SadTypeKind::Never:
                return getNeverType(); // ״£״¨״¯״§‹
            case SadTypeKind::Unknown:
                return getUnknownType(); // …״¬‡ˆ„
            case SadTypeKind::Class:
            case SadTypeKind::Trait:
                // (AR) أنواع معقدة — نُعيد Any كبديل مؤقت
                //      لأنها ليست بدائية ولا يمكن إنشاء PrimitiveType لها
                // (EN) Complex types — return Any as temporary placeholder
                //      since they are not primitive and PrimitiveType cannot handle them
                return getAnyType();
            default:
                // „„״£†ˆ״§״¹ ״§„״£״®״±‰״ ״§״³״×״®״¯… internType / For other types, use internType
                return internType(std::make_shared<PrimitiveType>(kind));
            }
        }

        // ״§„״­״µˆ„ ״¹„‰ ״£ˆ ״¥†״´״§״¡ †ˆ״¹ ״¹״§… / Get or create type with interning
        TypePtr TypeRegistry::internType(TypePtr type)
        {
            // ״§„״×״­‚‚ …† null / Check for null
            if (!type)
            {
                return nullptr; // ״¥״±״¬״§״¹ null ״¥״°״§ ƒ״§† ״§„…״¯״®„ null / Return null if input is null
            }

            // ‚„ mutex „„€ thread safety / Lock mutex for thread safety
            std::lock_guard<std::mutex> lock(mutex_);

            // ״­״³״§״¨ hash / Calculate hash
            size_t hash = hashType(type);

            // البحث في الخريطة / Search in map
            auto it = type_map_.find(hash);

            // إذا وُجد، إرجاع النوع الموجود / If found, return existing type
            if (it != type_map_.end())
            {
                return it->second; // إرجاع من cache / Return from cache
            }

            // إضافة نوع جديد / Add new type
            type_map_[hash] = type;

            // إرجاع النوع / Return type
            return type;
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° Hash / Hash Implementation
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // حساب hash لنوع / Calculate hash for a type
        size_t TypeRegistry::hashType(const TypePtr &type) const
        {
            // التحقق من null / Check for null
            if (!type)
            {
                return 0; // hash = 0 لـ null / hash = 0 for null
            }

            // بدء بـ hash نوع النوع / Start with kind hash
            size_t hash = std::hash<int>{}(static_cast<int>(type->getKind()));

            // للأنواع البدائية، النوع كافٍ / For primitive types, kind is sufficient
            if (type->isPrimitive() || type->isVoid() ||
                type->isAny() || type->isNever() || type->isUnknown())
            {
                return hash; // إرجاع hash النوع / Return kind hash
            }

            // للأنواع المركبة، نحتاج معلومات إضافية / For composite types, need more info
            // (AR) دمج معلومات إضافية في hash للأنواع المركبة باستخدام toString()
            // (EN) Mix additional info into hash for composite types via toString()
            std::string typeStr = type->toString();
            size_t strHash = std::hash<std::string>{}(typeStr);
            hash ^= strHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);

            // إرجاع hash / Return hash
            return hash;
        }

        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
        // ״×†״° ״§„״¥״­״µ״§״¦״§״× / Statistics Implementation
        // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

        // ״§„״­״µˆ„ ״¹„‰ ״¹״¯״¯ ״§„״£†ˆ״§״¹ ״§„…״³״¬„״© / Get number of registered types
        size_t TypeRegistry::getTypeCount() const
        {
            // ‚„ mutex / Lock mutex
            std::lock_guard<std::mutex> lock(mutex_);

            // ״¥״±״¬״§״¹ ״­״¬… ״§„״®״±״·״© / Return map size
            return type_map_.size();
        }

        // ״§„״­״µˆ„ ״¹„‰ ״¹״¯״¯ ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© / Get primitive type count
        size_t TypeRegistry::getPrimitiveTypeCount() const
        {
            // ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© ״§„…״¹״¯״© …״³״¨‚״§‹: 8 ״£†ˆ״§״¹ / Pre-cached primitive types: 8 types
            // (Void, Integer, Float, Boolean, String, Any, Never, Unknown)
            return 8;
        }

        // …״³״­ ״§„״³״¬„ („„״§״®״×״¨״§״±״§״× ‚״·) / Clear registry (for tests only)
        void TypeRegistry::clear()
        {
            // ‚„ mutex / Lock mutex
            std::lock_guard<std::mutex> lock(mutex_);

            // …״³״­ ״§„״®״±״·״© / Clear map
            type_map_.clear();

            // ״¥״¹״§״¯״© ״×‡״¦״© / Reinitialize
            initializePrimitiveTypes();
        }

        // ״¥״¹״§״¯״© ״×‡״¦״© ״§„״³״¬„ / Reinitialize registry
        void TypeRegistry::reset()
        {
            // ״§״³״×״¯״¹״§״¡ clear / Call clear
            clear();
        }

    } // namespace TypeSystem
} // namespace Sad
