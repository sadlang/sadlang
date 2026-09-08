// ============================================================================
// (AR) مولَّدٌ آليًّا من language-truth/backend/value_repr.yaml — لا تُحرّره يدويًّا.
// (EN) AUTO-GENERATED from language-truth/backend/value_repr.yaml — do not edit.
//      يُعاد توليده بـ`python x.py gen`؛ يحرسه `x.py gen --check`.
// ============================================================================
#ifndef SAD_VALUE_REPR_GENERATED_H
#define SAD_VALUE_REPR_GENERATED_H

#include <string>

namespace Sad { namespace Types { namespace repr {
    // ── dyn_kinds (SadDyn runtime type tags) ──
    inline constexpr long long kDynKindNull = 0;  // عدم (Null) — قيمةٌ فارغةٌ متمايزةٌ عن الصفر
    inline constexpr long long kDynKindInt = 1;  // عددٌ صحيح (i64)
    inline constexpr long long kDynKindFloat = 2;  // عشريّ (نمطُ بتّاتِ double في i64)
    inline constexpr long long kDynKindStr = 3;  // نصّ (مؤشّرٌ/واصفٌ حسب السياق)
    inline constexpr long long kDynKindBool = 4;  // منطقيّ (٠/١)
    inline constexpr long long kDynKindArray = 5;  // مصفوفة (مؤشّرٌ مُدار) — homogKind لناتجِ ZIP
    inline constexpr long long kDynKindMissing = 9;  // مفقود (Missing) — «لا قيمةَ هنا» (مفتاحُ خريطةٍ غائب)، متمايزٌ عن «عدم» القيمةِ الصريحة؛ كلاهما يُعرَض «لاشيء». 🔑 وكان اسمُه kDynKindVoid وحُذف اللفظُ لا السلوك: «فراغ» زالَ من اللغةِ (2026-09-07) وحلَّ محلَّه «خالي» نوعَ قيمةٍ كامل، وهذا الحارسُ الزمنيُّ **شيءٌ آخر** — غيابُ مفتاحٍ لا قيمةُ وحدة
    inline constexpr long long kDynKindUnit = 10;  // خالي (Unit) — قيمةُ الوحدة، القيمةُ الوحيدةُ لنوعِ `خالي`؛ تُعرَض «()». متمايزةٌ عن «مفقود» (لا قيمة) وعن «عدم» (قيمةٌ فارغةٌ صريحة)

    // ── map_layout (runtime map memory layout) ──
    inline constexpr long long kMapFieldCount = 0;  // فهرسُ حقلِ عددِ العناصرِ في الترويسة
    inline constexpr long long kMapFieldCapacity = 1;  // فهرسُ حقلِ السعة
    inline constexpr long long kMapFieldKeys = 2;  // فهرسُ حقلِ مؤشّرِ مصفوفةِ المفاتيح
    inline constexpr long long kMapFieldValues = 3;  // فهرسُ حقلِ مؤشّرِ مصفوفةِ القيم
    inline constexpr long long kMapFieldTypes = 4;  // فهرسُ حقلِ مؤشّرِ مصفوفةِ وسومِ الأنواع
    inline constexpr long long kMapHeaderBytes = 40;  // حجمُ الترويسة بالبايت (٥ حقولٍ × ٨)
    inline constexpr long long kMapSlotBytes = 8;  // حجمُ خانةِ المفتاح/القيمة/الوسم بالبايت
    inline constexpr long long kMapMinCapacity = 8;  // أدنى سعةٍ عند الإنشاء
    inline constexpr long long kMapGrowthFactor = 2;  // معاملُ التوسيعِ عند الامتلاء

    // ── display_texts (user-facing value display) ──
    inline const std::string kMissingTypeName = "مفقود";  // اسمُ النوعِ لوسمِ الغياب (Missing) — ما يُجيبُ به «نوع()» لمفتاحٍ غائبٍ من خريطة
    inline const std::string kUnitDisplay = "()";  // عرضُ قيمةِ الوحدة (خالي) — «()»، وهي الصيغةُ التي تُكتَبُ بها
    inline const std::string kNullDisplay = "لاشيء";  // عرضُ قيمةِ العدم/الفراغ (Null/Void) — يطابق Value::toString والمفسّر وdynToString
    inline const std::string kBoolTrueDisplay = "صحيح";  // عرضُ القيمةِ المنطقيّةِ الصادقة
    inline const std::string kBoolFalseDisplay = "خطأ";  // عرضُ القيمةِ المنطقيّةِ الكاذبة
    inline const std::string kObjectOpaqueDisplay = "<كائن>";  // عرضُ كائنٍ مُعتِمٍ داخلَ حاويةٍ مترجَمة — المفسّرُ أغنى (تباعُدٌ معلَن)
    inline const std::string kArrayOpen = "[";  // قوسُ فتحِ عرضِ المصفوفة
    inline const std::string kArrayClose = "]";  // قوسُ إغلاقِ عرضِ المصفوفة
    inline const std::string kArrayElemSep = ", ";  // فاصلُ عناصرِ عرضِ المصفوفة (لاتينيٌّ — يطابق المفسّر)
} } }  // Sad::Types::repr

#endif  // SAD_VALUE_REPR_GENERATED_H
