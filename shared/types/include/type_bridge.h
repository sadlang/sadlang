/**
 * @file type_bridge.h
 * @brief (AR) طبقة الجسر بين نظام الأنواع الموحد (SadType) والنظام القديم (Value/ValueType)
 * @brief (EN) Bridge layer between unified type system (SadType) and legacy system (Value/ValueType)
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 * (AR) هذا الملف هو نقطة العبور الرسمية بين النظامين:
 *
 *      الكود الجديد يستخدم SadType/SadTypePtr حصرياً.
 *      عندما يحتاج الكود الجديد للتعامل مع كود قديم يستخدم Value/ValueType،
 *      يستخدم الدوال المعرّفة هنا للتحويل بين الأنظمة.
 *
 *      ADR-01 Phase 1: كل كود جديد يستخدم SadType — هذا الملف يوفر الجسر.
 *      ADR-01 Phase 2: ترحيل الكود القديم تدريجياً — هذا الملف يُسهّل الانتقال.
 *      ADR-01 Phase 3: حذف Value/ValueType — هذا الملف يُحذف أيضاً.
 *
 *      اتجاهات التحويل:
 *        SadType → Value        (عند تمرير أنواع جديدة لكود قديم)
 *        Value → SadType        (عند استقبال قيم من كود قديم في كود جديد)
 *        SadType → ValueType    (تحويل التعداد فقط)
 *        ValueType → SadType    (تحويل التعداد فقط)
 *        SadType → DataType     (للربط مع AST)
 *        DataType → SadType     (من AST إلى النظام الموحد)
 *
 * (EN) This file is the official crossing point between the two systems:
 *
 *      New code uses SadType/SadTypePtr exclusively.
 *      When new code needs to interact with legacy Value/ValueType code,
 *      it uses the functions defined here for conversion.
 *
 *      ADR-01 Phase 1: All new code uses SadType — this file provides the bridge.
 *      ADR-01 Phase 2: Gradual migration of old code — this file eases transition.
 *      ADR-01 Phase 3: Remove Value/ValueType — this file is removed too.
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * @author فريق تطوير لغة ص / S Language Development Team
 * @date يناير 2026 / January 2026
 * @version 1.0 — ADR-01 Phase 1
 */

#pragma once

#include "sad_type_system.h"

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) إلغاء تعريفات ماكروز ويندوز المتعارضة
// ═══════════════════════════════════════════════════════════════════════════════════
#ifdef VOID
#undef VOID
#endif
#ifdef ERROR
#undef ERROR
#endif

// ═══════════════════════════════════════════════════════════════════════════════════
// (AR) إعلانات مسبقة — لا نضمّن value.h هنا لتجنب التبعية الدائرية
//      التنفيذ الفعلي في type_bridge.cpp حيث نضمّن كلا الملفين
// (EN) Forward declarations — we don't include value.h here to avoid circular deps
//      Actual implementation in type_bridge.cpp where both headers are included
// ═══════════════════════════════════════════════════════════════════════════════════

namespace Sad
{
    namespace Data
    {
        class Value;
        enum class DataType;
    } // namespace Data
} // namespace Sad

namespace Sad
{
    namespace Types
    {

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ①: تحويلات SadType ↔ ValueType (تعدادات فقط — سريعة وخفيفة)
        //  Section ①: SadType ↔ ValueType conversions (enum-only — fast & lightweight)
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) تحويل SadTypeKind إلى ValueType
         * @brief (EN) Convert SadTypeKind to ValueType
         *
         * (AR) تحويل مباشر بين التعدادين. يُستخدم عندما يحتاج الكود الجديد
         *      إلى تزويد كود قديم بنوع ValueType.
         *      بعض أنواع SadType ليس لها مقابل في ValueType (مثل Generic, Union)
         *      — هذه تُحوَّل إلى VOID كقيمة افتراضية.
         *
         * (EN) Direct enum-to-enum conversion. Used when new code needs to provide
         *      legacy code with a ValueType.
         *      Some SadTypeKind values have no ValueType equivalent (e.g. Generic, Union)
         *      — these are converted to VOID as a fallback.
         *
         * @param kind نوع SadTypeKind
         * @return ValueType المقابل
         */
        SadTypeKind toValueType(SadTypeKind kind);

        /**
         * @brief (AR) تحويل ValueType إلى SadTypeKind
         * @brief (EN) Convert ValueType to SadTypeKind
         *
         * (AR) تحويل عكسي. كل ValueType له مقابل مباشر في SadTypeKind.
         *
         * @param vt نوع ValueType
         * @return SadTypeKind المقابل
         */
        SadTypeKind fromValueType(SadTypeKind vt);

        // (AR) [S-TS-P2.5b] حُذفت دوال DataType (toDataType/fromDataType) — صفر مستهلك بعد
        //      ترحيل الـAST/المحلّل/الدلالات إلى SadTypeKind (P2.5a/P2). النظام الأفقر أُزيل.
        // (EN) [S-TS-P2.5b] DataType conversions removed — zero consumers after the AST/parser/
        //      semantic migration to SadTypeKind. The poorer representation is gone.

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ②: تحويلات SadTypePtr ↔ ValueType (نوع ذكي → تعداد والعكس)
        //  Section ②: SadTypePtr ↔ ValueType (smart type → enum and back)
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) تحويل SadTypePtr إلى ValueType (استخراج التعداد من النوع الذكي)
         * @brief (EN) Convert SadTypePtr to ValueType (extract enum from smart type)
         *
         * (AR) يستخرج SadTypeKind من المؤشر ثم يحوّله. إذا كان المؤشر null يُرجع VOID.
         *
         * @param type مؤشر النوع الذكي
         * @return ValueType المقابل
         */
        SadTypeKind sadTypeToValueType(const SadTypePtr &type);

        /**
         * @brief (AR) إنشاء SadTypePtr من ValueType
         * @brief (EN) Create SadTypePtr from ValueType
         *
         * (AR) يستخدم SadTypeRegistry لإنشاء النوع المناسب.
         *      للأنواع المركبة (ARRAY, MAP) يُنشئ نوعاً بدون معلومات عناصر.
         *
         * @param vt نوع ValueType
         * @return SadTypePtr نوع ذكي جديد
         */
        SadTypePtr sadTypeFromValueType(SadTypeKind vt);

        // (AR) [S-TS-P2.5b] حُذفت sadTypeToDataType/sadTypeFromDataType — صفر مستهلك.
        // (EN) [S-TS-P2.5b] sadTypeToDataType/sadTypeFromDataType removed — zero consumers.

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ③: تحويلات Value ↔ SadTypePtr (القيمة الكاملة ← النوع)
        //  Section ③: Value ↔ SadTypePtr (full value → type)
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) استنتاج SadTypePtr من قيمة Value موجودة
         * @brief (EN) Infer SadTypePtr from an existing Value instance
         *
         * (AR) يفحص نوع Value ويُنشئ SadTypePtr مناسباً:
         *      - للأنواع البدائية: يُرجع النوع المُخزَّن مسبقاً (interned)
         *      - للمصفوفات: يستنتج نوع العنصر من أول عنصر (إذا وُجد)
         *      - للخرائط: يُنشئ خريطة<نص، أي>
         *      - للكائنات: يُنشئ نوع صنف باسم الصنف الفعلي
         *      - للدوال: يُنشئ نوع دالة بدون معلومات معاملات
         *
         * (EN) Inspects Value type and creates appropriate SadTypePtr:
         *      - For primitives: returns pre-interned type
         *      - For arrays: infers element type from first element (if any)
         *      - For maps: creates Map<String, Any>
         *      - For objects: creates class type with actual class name
         *      - For functions: creates function type without param info
         *
         * @param value مرجع ثابت للقيمة
         * @return SadTypePtr النوع المُستنتَج
         */
        SadTypePtr inferSadType(const Data::Value &value);

        /**
         * @brief (AR) فحص هل القيمة Value متوافقة مع SadTypePtr المحدد
         * @brief (EN) Check if a Value is compatible with a specified SadTypePtr
         *
         * (AR) يُستخدم في فحص الأنواع وقت التشغيل: هل القيمة الممررة تتوافق مع النوع المتوقع؟
         *      يدعم: التوافق المباشر، التحويل الضمني (رقم→عشري)، أنواع اختيارية (T?)
         *
         * @param value القيمة للفحص
         * @param expectedType النوع المتوقع
         * @return true إذا كانت القيمة متوافقة
         */
        bool isValueCompatible(const Data::Value &value, const SadTypePtr &expectedType);

        /**
         * @brief (AR) الحصول على اسم النوع العربي من قيمة Value
         * @brief (EN) Get Arabic type name from a Value
         *
         * (AR) بديل لـ valueTypeToString() القديمة — يمر عبر SadType للحصول على اسم غني.
         *      مثال: بدلاً من "ARRAY" يُرجع "مصفوفة<رقم>" إذا أمكن استنتاج نوع العنصر.
         *
         * @param value القيمة
         * @return الاسم العربي الغني للنوع
         */
        std::string valueToArabicTypeName(const Data::Value &value);

        /**
         * @brief (AR) الحصول على اسم النوع الإنجليزي من قيمة Value
         * @brief (EN) Get English type name from a Value
         *
         * @param value القيمة
         * @return الاسم الإنجليزي الغني للنوع
         */
        std::string valueToEnglishTypeName(const Data::Value &value);

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  القسم ④: دوال مساعدة سريعة / Quick helper functions
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) هل نوعان SadType متساويان؟ (مقارنة آمنة مع null)
         * @brief (EN) Are two SadTypes equal? (null-safe comparison)
         */
        inline bool typesEqual(const SadTypePtr &a, const SadTypePtr &b)
        {
            if (!a && !b)
                return true;
            if (!a || !b)
                return false;
            return a->equals(b.get());
        }

        /**
         * @brief (AR) هل يمكن إسناد النوع المصدر إلى النوع الهدف؟ (مقارنة آمنة مع null)
         * @brief (EN) Can source type be assigned to target type? (null-safe check)
         */
        inline bool typeAssignableTo(const SadTypePtr &source, const SadTypePtr &target)
        {
            if (!source || !target)
                return true; // (AR) أنواع مجهولة — نسمح / (EN) unknown types — allow
            return source->isAssignableTo(target.get());
        }

        /**
         * @brief (AR) هل النوع هو أحد الأنواع الرقمية؟ (مقارنة آمنة مع null)
         * @brief (EN) Is the type one of the numeric types? (null-safe check)
         */
        inline bool isNumericType(const SadTypePtr &type)
        {
            return type && type->isNumeric();
        }

        /**
         * @brief (AR) هل النوع هو نوع نص؟ (مقارنة آمنة مع null)
         * @brief (EN) Is the type a string type? (null-safe check)
         */
        inline bool isStringType(const SadTypePtr &type)
        {
            return type && type->getKind() == SadTypeKind::String;
        }

        /**
         * @brief (AR) هل النوع بدائي؟ (مقارنة آمنة مع null)
         * @brief (EN) Is the type primitive? (null-safe check)
         */
        inline bool isPrimitiveType(const SadTypePtr &type)
        {
            return type && type->isPrimitive();
        }

        /**
         * @brief (AR) هل النوع مركب (مصفوفة/خريطة/صف)؟ (مقارنة آمنة مع null)
         * @brief (EN) Is the type composite (array/map/tuple)? (null-safe check)
         */
        inline bool isCompositeType(const SadTypePtr &type)
        {
            return type && type->isComposite();
        }

        /**
         * @brief (AR) هل النوع قابل للاستدعاء (دالة/إغلاق)؟ (مقارنة آمنة مع null)
         * @brief (EN) Is the type callable (function/closure)? (null-safe check)
         */
        inline bool isCallableType(const SadTypePtr &type)
        {
            return type && type->isCallable();
        }

        /**
         * @brief (AR) إنشاء وصف نصي مختصر لـ SadTypePtr (مع معالجة null)
         * @brief (EN) Create a short string description of SadTypePtr (null-safe)
         *
         * @param type المؤشر (يمكن أن يكون null)
         * @return وصف عربي قصير
         */
        inline std::string describeType(const SadTypePtr &type)
        {
            return type ? type->arabicName() : "مجهول";
        }

        /**
         * @brief (AR) إنشاء وصف نصي إنجليزي مختصر لـ SadTypePtr (مع معالجة null)
         * @brief (EN) Create a short English string description of SadTypePtr (null-safe)
         */
        inline std::string describeTypeEn(const SadTypePtr &type)
        {
            return type ? type->englishName() : "Unknown";
        }

    } // namespace Types
} // namespace Sad
