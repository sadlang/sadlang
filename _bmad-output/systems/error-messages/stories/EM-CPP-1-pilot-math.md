# Story EM-CPP-1 — بايلوت: ترحيل builtin_module_math.cpp

**Epic**: [EPIC-EM-CPP](../epics/EPIC-EM-CPP-MIGRATION.md) | **النقاط**: 3 | **الأولوية**: عالية | **الحالة**: محجوبة (تبعية EM-CPP-0)
**التبعيات**: EM-CPP-0 (الجسر) | **المُسنَد إلى**: TBD

---

## الهدف
ترحيل `interpreter/src/builtins/builtin_module_math.cpp` إلى **التوقيع الجديد `(BuiltinContext&)`**
واستبدال **21 موقع رمي خام** بـ`ctx.error(ErrorCode, placeholders)` — **بايلوت يُثبت النمط** قبل التوسعة.
(يحكمه [ADR-EM-CPP-1](../decisions/ADR-EM-CPP-1-BUILTIN-CONTEXT.md).)

## لماذا math أولاً؟
حجم متوسط (21)، أخطاء واضحة (نوع وسيط/نطاق)، رموز موجودة غالباً (`RUN_TYPE_CHECK_FAILED`,
`SEM_WRONG_ARG_COUNT`, `RUN_DIVISION_BY_ZERO`...).

## التنفيذ (Tasks)
- [ ] **T1** — جرد الـ21 موقعاً: لكل واحد، حدّد `ErrorCode` المناسب (موجود) أو سجّله كرمز ناقص.
- [ ] **T2** — للرموز الناقصة: أضِفها لـ`error_codes.h` + `language-truth/errors/runtime.yaml`
  (الإجراء: `ERROR_SYSTEM_GUIDE.md` §6) + أعد التوليد.
- [ ] **T3** — حوّل توقيع دوال math إلى `(BuiltinContext& ctx)` + استبدل كل
  `throw std::runtime_error("نص")` بـ`ctx.error(ErrorCode::X, {{"param", value}})` (النص في YAML).
- [ ] **T4** — البناء + اختبار سلوكي: استدعِ دالة math بوسيط خاطئ → يُعرَض `[CODE]` + الرسالة من الكتالوج.
- [ ] **T5** — تحقّق: `grep "throw std::runtime_error" builtin_module_math.cpp` = **0**.

## معايير القبول
- **Given** `builtin_module_math.cpp` فيه 21 رمياً خاماً
- **When** يكتمل الترحيل
- **Then** **0 `throw std::runtime_error`** في الملف
- **And** كل خطأ math يُعرَض من الكتالوج (`[CODE]` + رسالة YAML) بموقع الاستدعاء
- **And** البناء أخضر + لا تراجع في سلوك math
- **And** النمط موثّق ليُكرَّر في EM-CPP-2..6

## تعريف "تم"
math مُرحَّل بالكامل (0 خام) + النمط مُثبَت ومُوثَّق + اختبار سلوكي يمرّ.
