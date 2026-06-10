# Story: UTM-6.7 — enforce_guards (حارس النزاهة)

**ID:** UTM-6.7  
**Status:** ✅ Done  
**Sprint:** UTM-6 (codegen meta-system) — الخطوة الأخيرة  
**Assigned:** Amelia (dev agent)

---

## الهدف

إضافة حارس نزاهة (enforce_guard) يمنع أي مطور من إضافة ثوابت يدوية من نوع
`inline constexpr std::string_view` إلى `builtin_registry.h`. الحارس يُشغَّل تلقائياً
عبر `gen_all.py` وبالتالي عبر cmake — أي خرق يفشل البناء.

---

## ACs (معايير القبول)

- [x] AC-1: `_check_no_manual_builtins(repo)` موجودة في `gen_all.py` وتتحقق من النمط `inline constexpr std::string_view`
- [x] AC-2: الحارس يتجاهل سطور التعليقات (يبدأ بـ `//` أو `#`)
- [x] AC-3: الحارس يُشغَّل تلقائياً بعد جميع المولِّدات في `main()` لـ gen_all.py
- [x] AC-4: الحارس ينجح على `builtin_registry.h` الحالي (0 ثوابت يدوية) ← يطبع `[guard] OK`
- [x] AC-5: الحارس يفشل (exit=1) عند وجود ثابت يدوي ← يطبع `[guard] FAIL` مع رقم السطر
- [x] AC-6: pytest 102/102 passed (كانت 96 قبل UTM-6.7)
- [x] AC-7: C++ build EXIT_CODE=0 (sad-run.exe يُبنى بنجاح)

---

## Tasks

- [x] T1: إضافة `_GUARD_PATTERN`, `_REGISTRY_H_REL`, `_check_no_manual_builtins()` في `gen_all.py`
- [x] T2: تكامل الحارس في `main()` كخطوة post-generation
- [x] T3: إضافة `TestUTM67` (6 اختبارات) في `test_gen_builtins.py`
- [x] T4: pytest 102/102 + C++ build EXIT_CODE=0

---

## الملفات المُعدَّلة

| الملف | التغيير |
|-------|---------|
| `scripts/codegen/gen_all.py` | إضافة `_check_no_manual_builtins()` + تكاملها في `main()` |
| `scripts/codegen/test_gen_builtins.py` | إضافة `TestUTM67` (6 اختبارات) |

---

## سلوك الحارس

```
# ✅ نجاح — لا ثوابت يدوية
[guard] OK: builtin_registry.h has 0 manual constants.

# ❌ فشل — وُجد ثابت يدوي في السطر 42
[guard] FAIL: builtin_registry.h contains 1 manual constant(s).
  All constants MUST be in data/language/builtins/*.yaml
  and auto-generated via gen_builtins.py.
  Offending lines:
  line 42: inline constexpr std::string_view MY_CONST = "قيمة";
```

---

## أرقام مفتاحية

| المقياس | القيمة |
|---------|--------|
| pytest | **102/102** |
| C++ build | **EXIT_CODE=0** |
| ثوابت يدوية في builtin_registry.h | **0** ✅ |
| تعليمات الحارس | **تتجاهل سطور `//` و `#`** |

---

## Dev Agent Record

**Amelia (dev agent):** UTM-6.7 منجز — الخطوة الأخيرة في UTM-6.

- T1 ✅: `_check_no_manual_builtins()` أُضيفت — تتحقق من النمط، تتجاهل التعليقات، تذكر رقم السطر
- T2 ✅: تكامل في `main()` — يُشغَّل بعد جميع المولِّدات، يضيف `enforce_guard` لقائمة الفشل
- T3 ✅: `TestUTM67` — 6 اختبارات شاملة (clean, fail, line_num, comment_ignore, nonexistent, gen_all_exit)
- T4 ✅: pytest 102/102 + C++ EXIT_CODE=0
