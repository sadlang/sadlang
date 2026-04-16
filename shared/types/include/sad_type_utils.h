/**
 * @file sad_type_utils.h
 * @brief (AR) أدوات مساعدة لنظام الأنواع الموحد — ماكروز واختصارات للكود الجديد
 * @brief (EN) Utility helpers for the unified type system — macros and shortcuts for new code
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 * (AR) هذا الملف يوفر أدوات مختصرة لاستخدام SadType في الكود الجديد:
 *
 *      1. ماكروز الوصول السريع للأنواع البدائية (SAD_TYPE_INTEGER, SAD_TYPE_STRING, ...)
 *      2. ماكروز فحص النوع (SAD_IS_INTEGER, SAD_IS_STRING, ...)
 *      3. دوال مساعدة لإنشاء رسائل الأخطاء بالعربية
 *      4. أدوات مطابقة الأنماط (type matching/dispatch)
 *
 *      الهدف: تقليل الكود المكرر والتأكد من أن كل كود جديد يستخدم SadType.
 *
 * (EN) This file provides shortcuts for using SadType in new code:
 *      1. Quick-access macros for primitive types
 *      2. Type-checking macros
 *      3. Helper functions for Arabic error messages
 *      4. Type matching/dispatch utilities
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * @author فريق تطوير لغة ص / S Language Development Team
 * @date يناير 2026 / January 2026
 * @version 1.0 — ADR-01 Phase 1
 */

#pragma once

#include "sad_type_system.h"
#include <string>
#include <sstream>
#include <functional>

namespace Sad
{
    namespace Types
    {

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ①: اختصارات الوصول للأنواع البدائية (inline functions — أفضل من ماكروز)
        //  Section ①: Primitive type shortcuts (inline functions — better than macros)
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) الحصول السريع على أنواع بدائية مُخزنة مسبقاً (interned)
         * @brief (EN) Quick access to pre-interned primitive types
         *
         * (AR) هذه الدوال أسرع من SadTypeRegistry::instance().getX() لأنها:
         *      1. تُخزِّن المرجع المحلي لـ registry مرة واحدة
         *      2. استدعاءها مختصر — مثال: sadVoid() بدلاً من SadTypeRegistry::instance().getVoid()
         */
        inline SadTypePtr sadVoid() { return SadTypeRegistry::instance().getVoid(); }
        inline SadTypePtr sadInteger() { return SadTypeRegistry::instance().getInteger(); }
        inline SadTypePtr sadFloat() { return SadTypeRegistry::instance().getFloat(); }
        inline SadTypePtr sadBoolean() { return SadTypeRegistry::instance().getBoolean(); }
        inline SadTypePtr sadString() { return SadTypeRegistry::instance().getString(); }
        inline SadTypePtr sadByte() { return SadTypeRegistry::instance().getByte(); }
        inline SadTypePtr sadAny() { return SadTypeRegistry::instance().getAny(); }
        inline SadTypePtr sadNever() { return SadTypeRegistry::instance().getNever(); }
        inline SadTypePtr sadUnknown() { return SadTypeRegistry::instance().getUnknown(); }
        inline SadTypePtr sadError() { return SadTypeRegistry::instance().getError(); }

        // (AR) اختصارات الإنشاء / (EN) Creation shortcuts
        inline SadTypePtr sadArray(SadTypePtr elem = nullptr)
        {
            return SadTypeRegistry::instance().makeArray(std::move(elem));
        }

        inline SadTypePtr sadMap(SadTypePtr key = nullptr, SadTypePtr val = nullptr)
        {
            return SadTypeRegistry::instance().makeMap(std::move(key), std::move(val));
        }

        inline SadTypePtr sadTuple(std::vector<SadTypePtr> elems)
        {
            return SadTypeRegistry::instance().makeTuple(std::move(elems));
        }

        inline SadTypePtr sadFunction(std::vector<SadTypePtr> params, SadTypePtr ret = nullptr)
        {
            return SadTypeRegistry::instance().makeFunction(std::move(params), std::move(ret));
        }

        inline SadTypePtr sadClosure(std::vector<SadTypePtr> params, SadTypePtr ret = nullptr)
        {
            return SadTypeRegistry::instance().makeClosure(std::move(params), std::move(ret));
        }

        inline SadTypePtr sadClass(const std::string &name, SadTypePtr parent = nullptr)
        {
            return SadTypeRegistry::instance().getOrCreateClass(name, std::move(parent));
        }

        inline SadTypePtr sadOptional(SadTypePtr inner)
        {
            return SadTypeRegistry::instance().makeOptional(std::move(inner));
        }

        inline SadTypePtr sadUnion(std::vector<SadTypePtr> alts)
        {
            return SadTypeRegistry::instance().makeUnion(std::move(alts));
        }

        inline SadTypePtr sadReference(SadTypePtr pointee, bool isMutable = false)
        {
            return SadTypeRegistry::instance().makeReference(std::move(pointee), isMutable);
        }

        inline SadTypePtr sadFuture(SadTypePtr val = nullptr)
        {
            return SadTypeRegistry::instance().makeFuture(std::move(val));
        }

        inline SadTypePtr sadGenerator(SadTypePtr yield = nullptr)
        {
            return SadTypeRegistry::instance().makeGenerator(std::move(yield));
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ②: فحص الأنواع (type guards) — inline مع معالجة null
        //  Section ②: Type guards — inline with null handling
        //
        // █████████████████████████████████████████████████████████████████████████████████

        // (AR) فحص SadTypeKind — استخدام: if (sadIs(type, SadTypeKind::Integer)) ...
        inline bool sadIs(const SadTypePtr &type, SadTypeKind expected)
        {
            return type && type->getKind() == expected;
        }

        // (AR) فحص متعدد — استخدام: if (sadIsAnyOf(type, SadTypeKind::Integer, SadTypeKind::Float)) ...
        template <typename... Kinds>
        inline bool sadIsAnyOf(const SadTypePtr &type, Kinds... kinds)
        {
            if (!type)
                return false;
            auto k = type->getKind();
            return ((k == kinds) || ...);
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ③: رسائل أخطاء الأنواع بالعربية
        //  Section ③: Arabic type error messages
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) إنشاء رسالة خطأ عدم تطابق أنواع
         * @brief (EN) Create type mismatch error message
         *
         * @param expected النوع المتوقع
         * @param actual النوع الفعلي
         * @param context سياق إضافي (اختياري)
         * @return رسالة خطأ بالعربية
         */
        inline std::string makeTypeMismatchError(const SadTypePtr &expected, const SadTypePtr &actual,
                                                 const std::string &context = "")
        {
            std::ostringstream oss;
            oss << "خطأ في النوع: ";
            if (!context.empty())
                oss << context << " — ";
            oss << "متوقع '" << (expected ? expected->arabicName() : "مجهول") << "'";
            oss << " ولكن وُجد '" << (actual ? actual->arabicName() : "مجهول") << "'";
            return oss.str();
        }

        /**
         * @brief (AR) إنشاء رسالة خطأ عملية غير مدعومة على نوع
         * @brief (EN) Create unsupported operation error message
         *
         * @param operation اسم العملية
         * @param type النوع
         * @return رسالة خطأ بالعربية
         */
        inline std::string makeUnsupportedOpError(const std::string &operation, const SadTypePtr &type)
        {
            std::ostringstream oss;
            oss << "خطأ: العملية '" << operation << "' غير مدعومة على النوع '"
                << (type ? type->arabicName() : "مجهول") << "'";
            return oss.str();
        }

        /**
         * @brief (AR) إنشاء رسالة خطأ تحويل نوع
         * @brief (EN) Create type conversion error message
         *
         * @param from النوع المصدر
         * @param to النوع الهدف
         * @return رسالة خطأ بالعربية
         */
        inline std::string makeConversionError(const SadTypePtr &from, const SadTypePtr &to)
        {
            std::ostringstream oss;
            oss << "خطأ: لا يمكن تحويل '" << (from ? from->arabicName() : "مجهول")
                << "' إلى '" << (to ? to->arabicName() : "مجهول") << "'";
            return oss.str();
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ④: نمط مطابقة الأنواع (Type Dispatch Pattern)
        //  Section ④: Type matching/dispatch pattern
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) إرسال إلى دالة بناءً على نوع SadType (نمط Visitor خفيف)
         * @brief (EN) Dispatch to a handler based on SadType kind (lightweight Visitor)
         *
         * (AR) استخدام:
         *   dispatchOnType(type, {
         *       { SadTypeKind::Integer, [&]() { ... } },
         *       { SadTypeKind::String,  [&]() { ... } },
         *   }, [&]() { // افتراضي ... });
         *
         * @param type النوع للمطابقة
         * @param handlers جدول المعالجات (kind → handler)
         * @param defaultHandler المعالج الافتراضي (اختياري)
         */
        inline void dispatchOnType(
            const SadTypePtr &type,
            const std::initializer_list<std::pair<SadTypeKind, std::function<void()>>> &handlers,
            const std::function<void()> &defaultHandler = nullptr)
        {
            if (!type)
            {
                if (defaultHandler)
                    defaultHandler();
                return;
            }

            auto kind = type->getKind();
            for (const auto &[k, handler] : handlers)
            {
                if (k == kind)
                {
                    handler();
                    return;
                }
            }

            if (defaultHandler)
                defaultHandler();
        }

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ⑤: تحويلات مفيدة بين SadTypeKind والنص
        //  Section ⑤: Useful SadTypeKind ↔ string conversions
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) الحصول على اسم النوع من kind مباشرة (عربي)
         * @brief (EN) Get type name from kind directly (Arabic)
         */
        inline std::string kindToArabic(SadTypeKind kind) { return sadTypeKindToArabic(kind); }

        /**
         * @brief (AR) الحصول على اسم النوع من kind مباشرة (إنجليزي)
         * @brief (EN) Get type name from kind directly (English)
         */
        inline std::string kindToEnglish(SadTypeKind kind) { return sadTypeKindToEnglish(kind); }

        /**
         * @brief (AR) تعيين النوع الافتراضي — يُرجع النوع المُعطى أو القيمة الافتراضية
         * @brief (EN) Default type — return given type or fallback
         *
         * (AR) مفيد عندما يكون النوع اختيارياً:
         *      SadTypePtr t = typeOrDefault(maybeNull, sadInteger());
         */
        inline SadTypePtr typeOrDefault(const SadTypePtr &type, const SadTypePtr &defaultType)
        {
            return type ? type : defaultType;
        }

    } // namespace Types
} // namespace Sad
