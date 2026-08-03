// ============================================================================
// (AR) مولَّدٌ آليًّا من language-truth/backend/native_diagnostics.yaml — لا تُحرّره يدويًّا.
// (EN) AUTO-GENERATED from language-truth/backend/native_diagnostics.yaml — do not edit.
//      يُعاد توليده بـ`python x.py gen`؛ يحرسه `x.py gen --check`.
// ============================================================================
#ifndef SAD_NATIVE_DIAGNOSTICS_GENERATED_H
#define SAD_NATIVE_DIAGNOSTICS_GENERATED_H

#include <string>

namespace sad { namespace native { namespace diag {
    // ── format ──
    inline const std::string kVregSigil = "%";  // سِجِلُّ السجلّ الوهميّ في SIR (يسبق اسمَ المعامل/المعامِل)
    inline const std::string kFormSep = " ";  // فاصلٌ بين المنمنمة وصيغةِ الترميز في تشخيص «ترميزٌ مفقود»
    inline const std::string kNoImmField = ":no-imm-field";  // لا حقلَ فوريّ في صيغةِ الترميز المطلوبة (لاحقة)
    inline const std::string kUnsignedSuffix = "-unsigned=";  // لاحقةُ «معامِلٌ لا-موقَّع في مقارنةِ ترتيبٍ موقَّعة» (يسبقها موضعُ الفحص)

    // ── pure ──
    inline const std::string kMoveKind = "move-kind";  // نوعُ معامل MOVE غيرُ مدعوم
    inline const std::string kArrayGetBoxed = "array-get-boxed";  // ARRAY_GET لعنصرٍ معلَّب (elementType=Any) مؤجَّل
    inline const std::string kArraySetBoxed = "array-set-boxed";  // ARRAY_SET لعنصرٍ معلَّب (elementType=Any) مؤجَّل
    inline const std::string kArrayAppendBoxed = "array-append-boxed";  // BUILTIN_ARRAY_APPEND لعنصرٍ معلَّب (elementType=Any) مؤجَّل — يلزمه تعليبُ SadDyn
    inline const std::string kArrayNewDynamicSize = "array-new-dynamic-size";  // ARRAY_NEW بحجمٍ غيرِ ثابتٍ مؤجَّل
    inline const std::string kArrayNewNegative = "array-new-negative";  // ARRAY_NEW بطولٍ/سعةٍ سالبة
    inline const std::string kArrayConcatBoxed = "array-concat-boxed";  // ARRAY_CONCAT لمصفوفةٍ معلَّبة (elementType=Any) أو مختلفةِ النوعين مؤجَّل — يلزمه دمجُ مخزنِ الوسوم
    inline const std::string kEnumPayloadKind = "enum-payload-kind";  // ENUM_CONSTRUCT بحمولةٍ غيرِ عدديّةٍ محدَّدة (عشريّ/نصّ) مؤجَّل — يلزمه تعليبُ SadDyn الكامل
    inline const std::string kEnumPayloadDyn = "enum-payload-dyn";  // ENUM_GET_PAYLOAD بنتيجةٍ ديناميّة (Any) مؤجَّل — يلزمه بناءُ قيمةٍ ديناميّةٍ موسومة
    inline const std::string kObjectUnknownClass = "object-unknown-class";  // وصولُ حقلِ كائنٍ لصنفٍ غيرِ معروفٍ زمنَ الترجمة (تعذّر استنتاجُ الصنف)
    inline const std::string kObjectFieldLayout = "object-field-layout";  // حقلُ كائنٍ بتخطيطٍ غيرِ منتظم (bool/Any) أو حقلٌ غيرُ موجود مؤجَّل — يُدعَم الحقلُ ٨-بت فقط
    inline const std::string kObjectNoVtable = "object-no-vtable";  // نداءٌ افتراضيٌّ (OBJECT_CALL) لصنفٍ بلا جدولِ دوالّ (vtable) — الصنفُ @تمثيل_سي أو غيرُ مسجَّل
    inline const std::string kObjectMethodNoSlot = "object-method-no-slot";  // نداءٌ افتراضيٌّ لطريقةٍ لا خانةَ لها في جدولِ الدوالّ (باني/هادم أو اسمٌ مجهول)
    inline const std::string kVtableUnresolvedFn = "vtable-unresolved-fn";  // خانةُ جدولِ دوالٍّ تشيرُ لدالّةٍ غيرِ معرَّفةٍ زمنَ الترجمة — تعذّر ترقيعُ عنوانها
    inline const std::string kFuncAddrUnresolved = "func-addr-unresolved";  // عنوانُ دالّةٍ (إغلاقٌ/نداءٌ غيرُ مباشر) يشيرُ لدالّةٍ غيرِ معرَّفةٍ زمنَ الترجمة — تعذّر ترقيعُ عنوانها
    inline const std::string kCmpValue = "cmp-value";  // موضعُ فحصٍ: مقارنةٌ مُستعمَلةٌ قيمةً (لا مدموجةً في فرع)
    inline const std::string kCmpBranch = "cmp-branch";  // موضعُ فحصٍ: مقارنةٌ مدموجةٌ في فرع

    // ── kind ──
    inline const std::string kArgKind = "arg-kind=";  // صنفُ معامل الوسيط غيرُ مدعوم
    inline const std::string kConstType = "const-type=";  // نوعُ الثابت غيرُ صحيحٍ (Integer فقط)
    inline const std::string kOperandKind = "operand-kind=";  // صنفُ المعامل غيرُ مدعوم
    inline const std::string kCmpBKind = "cmp-b-kind=";  // صنفُ المعامِل الثاني للمقارنة غيرُ مدعوم
    inline const std::string kCondKind = "cond-kind=";  // صنفُ معامِل الشرط غيرُ مدعوم
    inline const std::string kCondConstType = "cond-const-type=";  // نوعُ ثابتِ الشرط غيرُ صحيح
    inline const std::string kCmpValueUnsigned = "cmp-value-unsigned=";  // مقارنةٌ كقيمةٍ بمعامِلٍ لا-موقَّع (مؤجَّل)
    inline const std::string kCmpUnsignedType = "cmp-unsigned-type=";  // مقارنةُ فرعٍ بمعامِلٍ لا-موقَّع (مؤجَّل)

    // ── tag ──
    inline const std::string kAllocUnslotted = "alloc-unslotted:";  // ALLOC بلا خانةِ إطارٍ مخصَّصة
    inline const std::string kPhiUnslotted = "phi-unslotted:";  // PHI بلا خانةِ إطارٍ مخصَّصة (المسحُ المسبق أخفق)
    inline const std::string kAddrNonslot = "addr-nonslot:";  // ADDR لغيرِ متغيّرِ إطارٍ (عنوانُ مؤشّرِ حوضٍ غيرُ مدعوم)
    inline const std::string kRolVar = "rol-var:";  // ROL بمقدارٍ متغيّرٍ (المدعومُ ثابتٌ فقط؛ يتيمٌ سطحيًّا)
    inline const std::string kArgsGt6 = "args>6:";  // وسائطُ نداءٍ تتجاوز ٦ (AAPCS64 x0-x7؛ الحدُّ الحاليّ ٦)
    inline const std::string kArgsGt8 = "args>8:";  // وسائطُ نداءٍ تتجاوز ٨
    inline const std::string kArrayIndexImm12 = "array-index-imm12:";  // فهرسٌ ثابتٌ يتجاوز imm12 (٤٠٩٥) في عنونةِ عنصر
    inline const std::string kBoundsPanicRel8 = "bounds-panic-rel8:";  // إزاحةُ jb لكتلةِ هلعِ الحدّ خارج rel8
    inline const std::string kFrame = "frame:";  // حجمُ الإطار خارجَ مدى الترميز
    inline const std::string kBuiltinFloatMinMax = "builtin-float-minmax:";  // أصغر/أكبر بمعاملٍ عشريٍّ (مقارنةُ بتّاتٍ صحيحةٌ خطأٌ للسالب؛ يلزمه minsd/maxsd·fmin/fmax — دفعةٌ لاحقة)
    inline const std::string kLoadNonslot = "load-nonslot:";  // LOAD من غيرِ خانةِ إطار
    inline const std::string kNoBlocks = "no-blocks:";  // دالّةٌ بلا كتلٍ أساسيّة
    inline const std::string kParamsGt6 = "params>6:";  // معاملاتٌ تتجاوز ٦ (AAPCS64)
    inline const std::string kParamsGt8 = "params>8:";  // معاملاتٌ تتجاوز ٨ (SysV)
    inline const std::string kRel = "rel:";  // إزاحةٌ نسبيّةٌ خارجَ المدى
    inline const std::string kS32 = "s32:";  // فوريٌّ موقَّعٌ خارج [−2³¹, 2³¹)
    inline const std::string kShift = "shift:";  // مقدارُ إزاحةٍ خارجَ المدى المسموح
    inline const std::string kStoreNonslot = "store-nonslot:";  // STORE إلى غيرِ خانةِ إطار
    inline const std::string kStrlen = "strlen:";  // طولُ سلسلةٍ يتجاوز حدَّ movz (٦٥٥٣٥)
    inline const std::string kU16 = "u16:";  // فوريٌّ لا-موقَّعٌ يتجاوز ١٦ بتًّا (movz)
    inline const std::string kU32 = "u32:";  // فوريٌّ لا-موقَّعٌ يتجاوز ٣٢ بتًّا
    inline const std::string kUnaligned = "unaligned:";  // قيمةٌ غيرُ محاذاةٍ حيث تلزم المحاذاة
    inline const std::string kPool = "pool=";  // نفادُ حوضِ السجلّات (يتبعه حجمُ الحوض)

    // ── vreg ──
    inline const std::string kCmpBMemvar = "cmp-b-memvar:";  // معامِلُ المقارنةِ الثاني متغيّرُ ذاكرةٍ (مؤجَّل)
    inline const std::string kCondNotFusedCmp = "cond-not-fused-cmp:";  // شرطُ الفرعِ ليس مقارنةً مدموجة
    inline const std::string kPrintStrComputed = "print-str-computed:";  // طباعةُ سلسلةٍ محسوبةٍ (لا حرفيّة) مؤجَّلة
    inline const std::string kVregAliasesSlot = "vreg-aliases-slot:";  // سجلٌّ وهميٌّ يطابق اسمَ خانةِ إطار (تصادم)

    // ── helper ──
    inline const std::string kOpcode = "opcode=";  // الأوپكود المتضمَّن (يتبعه رقمُ SIROpcode)
    inline const std::string kBlocksZero = "blocks=0";  // الوحدةُ بلا أيّ كتلةٍ أساسيّة
    inline const std::string kNoRetTerminator = "no-ret-terminator";  // الكتلةُ الأخيرةُ بلا مُنهٍ RET
} } }  // sad::native::diag

#endif  // SAD_NATIVE_DIAGNOSTICS_GENERATED_H
