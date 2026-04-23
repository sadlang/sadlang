/**
 * @file module_definitions.h
 * @brief (AR) تعريف جميع الوحدات المدمجة وأسمائها المستعارة — مصدر الحقيقة الوحيد
 * @brief (EN) Definition of all builtin modules and their aliases — single source of truth
 *
 * @details
 * (AR) هذا الملف يعرّف جميع الوحدات المدمجة في لغة ص مع:
 *      - الاسم الأساسي (canonical name)
 *      - الأسماء المستعارة (aliases) — عربية وإنجليزية
 *      - المعرّف الرقمي (module ID) لكل وحدة
 *
 *      أي تعديل على أسماء الوحدات أو إضافة وحدة جديدة يجب أن يتم هنا فقط.
 *      المفسر والمترجم يقرآن من هذا الملف.
 *
 * (EN) This file defines all builtin modules in S Language with:
 *      - Canonical name
 *      - Aliases (Arabic and English)
 *      - Numeric module ID
 *
 *      Any changes to module names or new modules must be done here only.
 *      Both interpreter and compiler read from this file.
 *
 * القرار المعماري: ADR-003 — توحيد المكتبات القياسية
 *
 * @author فريق تطوير لغة ص / Sad Language Development Team
 * @date 2025-07-19
 */

#pragma once

#include <string_view>
#include <cstdint>
#include <cstddef>
#include <array>

namespace Sad
{
    namespace Builtins
    {

        // ════════════════════════════════════════════════════════════════════
        // (AR) معرّفات الوحدات — ثوابت وقت الترجمة
        // (EN) Module IDs — compile-time constants
        // ════════════════════════════════════════════════════════════════════

        enum class ModuleId : uint8_t
        {
            NONE = 0,       /// (AR) لا وحدة مطلوبة (core) / (EN) No module required (core)
            MATH = 1,       /// (AR) رياضيات / (EN) Math
            STRINGS = 2,    /// (AR) نصوص / (EN) Strings
            BASICS = 3,     /// (AR) أساسيات / (EN) Basics
            ASYNC = 4,      /// (AR) تزامن / (EN) Async/Concurrency
            ASSERTIONS = 5, /// (AR) تأكيدات / (EN) Assertions
            MAPS = 6,       /// (AR) خرائط / (EN) Maps
            PLATFORM = 7,   /// (AR) منصة / (EN) Platform
            NETWORK = 8,    /// (AR) شبكة / (EN) Network
            CRYPTO = 9,     /// (AR) تشفير / (EN) Crypto
            SOCKETS = 10,   /// (AR) مقابس / (EN) Sockets

            _MODULE_COUNT /// (AR) عدد الوحدات (للتحقق) / (EN) Module count (for validation)
        };

        // ════════════════════════════════════════════════════════════════════
        // (AR) تعريف اسم مستعار لوحدة — يربط اسماً بمعرّف الوحدة
        // (EN) Module alias definition — maps a name to a module ID
        // ════════════════════════════════════════════════════════════════════

        struct ModuleAlias
        {
            std::string_view name; /// (AR) الاسم (عربي أو إنجليزي) / (EN) Name (Arabic or English)
            ModuleId moduleId;     /// (AR) معرّف الوحدة المرتبطة / (EN) Associated module ID
        };

        // ════════════════════════════════════════════════════════════════════
        // (AR) جدول الأسماء المستعارة — constexpr — مصدر الحقيقة الوحيد
        //      يُستخدم في المفسر (BuiltinModuleRegistry) والمترجم (sir_builder_module_check)
        // (EN) Alias table — constexpr — single source of truth
        //      Used by interpreter (BuiltinModuleRegistry) and compiler (sir_builder_module_check)
        // ════════════════════════════════════════════════════════════════════

        inline const std::array<ModuleAlias, 10> MODULE_ALIASES = {{
            // ─── اسم واحد فقط لكل وحدة — لا أسماء بديلة ───
            {"رياضيات", ModuleId::MATH},
            {"نصوص", ModuleId::STRINGS},
            {"أساسيات", ModuleId::BASICS},
            {"تزامن_متقدم", ModuleId::ASYNC},
            {"تأكيدات", ModuleId::ASSERTIONS},
            {"خرائط", ModuleId::MAPS},
            {"منصة", ModuleId::PLATFORM},
            {"شبكة", ModuleId::NETWORK},
            {"تشفير", ModuleId::CRYPTO},
            {"مقابس", ModuleId::SOCKETS},
        }};

        // ════════════════════════════════════════════════════════════════════
        // (AR) دالة بحث عن الوحدة بالاسم — constexpr
        // (EN) Module lookup by name — constexpr
        // ════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) حل اسم إلى معرّف وحدة. يُرجع NONE إذا لم يُعثر عليه.
         * @brief (EN) Resolve a name to a module ID. Returns NONE if not found.
         */
        inline ModuleId resolveModuleId(std::string_view name)
        {
            for (const auto &alias : MODULE_ALIASES)
            {
                if (alias.name == name)
                    return alias.moduleId;
            }
            return ModuleId::NONE;
        }

        /**
         * @brief (AR) الحصول على الاسم الأساسي (canonical) لوحدة
         * @brief (EN) Get canonical name for a module
         */
        inline constexpr std::string_view getCanonicalModuleName(ModuleId id)
        {
            switch (id)
            {
            case ModuleId::MATH:
                return "رياضيات";
            case ModuleId::STRINGS:
                return "نصوص";
            case ModuleId::BASICS:
                return "أساسيات";
            case ModuleId::ASYNC:
                return "تزامن_متقدم";
            case ModuleId::ASSERTIONS:
                return "تأكيدات";
            case ModuleId::MAPS:
                return "خرائط";
            case ModuleId::PLATFORM:
                return "منصة";
            case ModuleId::NETWORK:
                return "شبكة";
            case ModuleId::CRYPTO:
                return "تشفير";
            case ModuleId::SOCKETS:
                return "مقابس";
            default:
                return "";
            }
        }

        /**
         * @brief (AR) هل الاسم المعطى وحدة مدمجة معروفة؟
         * @brief (EN) Is the given name a known builtin module?
         */
        inline bool isKnownModule(std::string_view name)
        {
            return resolveModuleId(name) != ModuleId::NONE;
        }

    } // namespace Builtins
} // namespace Sad
