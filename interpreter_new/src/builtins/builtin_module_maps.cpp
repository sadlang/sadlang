/**
 * @file builtin_module_maps.cpp
 * @brief (AR) وحدة الخرائط — ملف مُوجِّه يستدعي دوال التسجيل الفرعية
 * @brief (EN) Maps module — dispatcher that calls sub-registration functions
 *
 * @details
 * (AR) هذا الملف يستدعي دوال التسجيل من الملفات الفرعية الخمسة:
 *   - maps_core: منشئات + خرائط + برمجة وظيفية + مجموعات
 *   - maps_json_xml: JSON + XML
 *   - maps_arrays: مصفوفات متقدمة + matrix
 *   - maps_text: نصوص + regex + يونيكود
 *   - maps_utils: تاريخ + رياضيات + ملفات + تسلسل + بيئة + أخطاء
 *
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtins.h"
#include "interpreter_core.h"

// (AR) إلغاء ماكرو VOID الخاص بويندوز إن وُجد
#ifdef VOID
#undef VOID
#endif

namespace Sad
{
    namespace Interpreter
    {

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) إعلانات خارجية — دوال التسجيل الفرعية
        // (EN) Extern declarations — sub-registration functions
        // ═══════════════════════════════════════════════════════════════════════
        extern void registerBuiltinsMapsCore(Interpreter &interpreter);
        extern void registerBuiltinsMapsJsonXml(Interpreter &interpreter);
        extern void registerBuiltinsMapsArrays(Interpreter &interpreter);
        extern void registerBuiltinsMapsText(Interpreter &interpreter);
        extern void registerBuiltinsMapsUtils(Interpreter &interpreter);

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) دالة التسجيل الرئيسية — تستدعي الدوال الفرعية
        // (EN) Main registration — delegates to sub-functions
        // ═══════════════════════════════════════════════════════════════════════
        void registerBuiltinsMaps(Interpreter &interpreter)
        {
            // (AR) تسجيل الخرائط الأساسية (منشئات + خرائط + وظيفية + مجموعات)
            registerBuiltinsMapsCore(interpreter);

            // (AR) تسجيل JSON و XML
            registerBuiltinsMapsJsonXml(interpreter);

            // (AR) تسجيل المصفوفات المتقدمة والمصفوفات متعددة الأبعاد
            registerBuiltinsMapsArrays(interpreter);

            // (AR) تسجيل النصوص المتقدمة والتعبيرات النمطية واليونيكود
            registerBuiltinsMapsText(interpreter);

            // (AR) تسجيل الأدوات (تاريخ، رياضيات، ملفات، تسلسل، بيئة، أخطاء)
            registerBuiltinsMapsUtils(interpreter);
        }

    } // namespace Interpreter
} // namespace Sad
