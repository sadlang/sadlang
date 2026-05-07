/**
 * @file runtime_arithmetic_catalog.cpp
 * @brief (AR) قوالب أخطاء وقت التشغيل الحسابية: قسمة على صفر، باقي،
 *             قسمة صحيحة، تجاوز عددي.
 * @brief (EN) Arithmetic runtime error templates: division by zero,
 *             modulo, floor division, numeric overflow.
 *
 * (AR) Phase 4 — كل القوالب تستخدم placeholders فقط ({a}, {b}, ...).
 *      المفسر يُمرّر القيم الديناميكية، لا يُمرّر نص الرسالة.
 */

#include "runtime_template_helpers.h"

namespace Sad
{
    namespace Errors
    {

        void registerArithmeticRuntimeTemplates(ErrorCatalog &catalog)
        {
            using EC = ErrorCode;
            using Catalog::makeRT;

            // (AR) قسمة فاصلة عائمة على صفر — يُغطي 2 موقع
            catalog.registerTemplate(EC::RUN_DIVISION_BY_ZERO, makeRT(
                                                                   "RUN001",
                                                                   "قسمة على صفر", "Division by zero",
                                                                   "لا يمكن قسمة {a} على صفر",
                                                                   "Cannot divide {a} by zero",
                                                                   "تحقّق أن المقسوم عليه ≠ 0 قبل العملية",
                                                                   "Verify divisor ≠ 0 before the operation",
                                                                   "القسمة على صفر غير معرَّفة رياضياً. وقت التشغيل لا يستطيع إنتاج قيمة فيتوقّف.",
                                                                   "Division by zero is mathematically undefined. Runtime cannot produce a value and halts.",
                                                                   "إذا (ع != 0)\n   متغير ن = س / ع\nنهاية"));

            // (AR) قسمة صحيحة على صفر — يُغطي 2 موقع
            catalog.registerTemplate(EC::RUN_FLOOR_DIVISION_BY_ZERO, makeRT(
                                                                         "RUN009",
                                                                         "قسمة صحيحة على صفر", "Floor division by zero",
                                                                         "لا يمكن إجراء قسمة صحيحة لـ {a} على صفر",
                                                                         "Cannot floor-divide {a} by zero",
                                                                         "تحقّق من المقسوم عليه قبل استخدام عامل القسمة الصحيحة //",
                                                                         "Verify divisor before using the // operator",
                                                                         "القسمة الصحيحة (//) تُنتج خارجاً صحيحاً، لكن القسمة على صفر تظل غير معرَّفة.",
                                                                         "Floor division (//) produces an integer quotient, but division by zero remains undefined."));

            // (AR) باقي القسمة على صفر — يُغطي 2 موقع
            catalog.registerTemplate(EC::RUN_MODULO_BY_ZERO, makeRT(
                                                                 "RUN010",
                                                                 "باقي القسمة على صفر", "Modulo by zero",
                                                                 "لا يمكن حساب باقي قسمة {a} على صفر",
                                                                 "Cannot compute {a} modulo zero",
                                                                 "تحقّق من المعامل الثاني قبل استخدام %",
                                                                 "Verify the second operand before using %",
                                                                 "عامل % يحسب باقي القسمة، وهو غير معرَّف عندما يكون المقسوم عليه صفراً.",
                                                                 "The % operator computes the remainder, which is undefined when divisor is zero."));

            // (AR) تجاوز عددي — احتياطي
            catalog.registerTemplate(EC::RUN_NUMERIC_OVERFLOW, makeRT(
                                                                   "RUN011",
                                                                   "تجاوز عددي", "Numeric overflow",
                                                                   "تجاوزت العملية على {a} و {b} نطاق نوع رقم",
                                                                   "Operation on {a} and {b} exceeded رقم range",
                                                                   "استخدم نوعاً أوسع أو راجع الحدود",
                                                                   "Use a wider type or review bounds"));
        }

    } // namespace Errors
} // namespace Sad
