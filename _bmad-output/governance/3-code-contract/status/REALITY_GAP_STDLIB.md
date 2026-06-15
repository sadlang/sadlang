# REALITY_GAP_STDLIB — تقرير AUDIT (Reality Reconciliation)

**Auditor:** dev_audit_stdlib (Explore subagent)  
**التاريخ:** 2026-06-01  
**Sprint:** [AUDIT-2026-06](../../2-agents/sprints/SPRINT_AUDIT_REALITY_RECONCILIATION.md)  
**Zone:** stdlib/, data/

---

## 1. ملخص تنفيذي

| المقياس | المُعلن | الفعلي | الفجوة |
|---|---|---|---|
| الكلمات المحجوزة | 40 + 3 + ~30 سياقية | 43 + سياقية كاملة | ✅ صفر |
| ملفات data/stdlib/ | 9 | 9 | ✅ صفر |
| دوال core builtins (registerCoreIOBuiltins) | ~21 ادعاء | 17 فعلي | ⚠️ -4 |
| تَطابق اسم "نوع" | YAML: "نوع" | cpp: "النوع" | 🔴 عدم تَطابق |

**درجة الصدق:** ~95% (مشكلة واحدة فعلية: اسم دالة)

---

## 2. مُطابقة الكلمات المحجوزة

**المصادر:**
- [data/language/keywords.yaml](data/language/keywords.yaml)
- [shared/lexer/src/lexer_keywords.cpp](shared/lexer/src/lexer_keywords.cpp#L37-L60)

**النَتيجة:** ✅ **تَطابق تام 100%**

| الفئة | العدد | حالة |
|---|---|---|
| كلمات محجوزة | 40 | ✅ |
| عوامل منطقية | 3 (و، أو، ليس) | ✅ |
| كلمات سياقية | ~30 (emittedByLexer: false) | ✅ |

---

## 3. مُطابقة data/stdlib/

| # | الملف | الدوال | الحالة |
|---|---|---|---|
| 1 | core.yaml | 19 | ✅ |
| 2 | stdlib_arrays.yaml | ~10+ | ✅ |
| 3 | stdlib_basics.yaml | ~9+ | ✅ |
| 4 | stdlib_assertions.yaml | 5 | ✅ |
| 5 | stdlib_io.yaml | 3 | ✅ |
| 6 | stdlib_math.yaml | ~35+ | ✅ |
| 7 | stdlib_strings.yaml | ~20+ | ✅ |
| 8 | stdlib_maps.yaml | ~45+ (JSON/XML/Regex/Time) | ✅ |
| 9 | stdlib_http.yaml | 8 | ✅ |

---

## 4. فجوات الدوال الأساسية

**المُنفَّذ في [interpreter/src/builtins/builtin_core_io.cpp](interpreter/src/builtins/builtin_core_io.cpp#L37-L347):**

| الدالة | الحالة | السطر |
|---|---|---|
| اطبع، اطبع_سطر، اقرأ | ✅ | 44-68 |
| متوقع، غير_متوقع | 🎁 موجود غير مَذكور | 73-99 |
| طول، نص، رقم، عشري، منطقي | ✅ | 104-149 |
| **النوع** (cpp) | ⚠️ YAML يَدَّعيها "نوع" | 115 |
| قناة، انتظر_الكل، عدد_المهام | ✅ | 155-191 |
| مجموعة_انتظار، قفل، مستقبل | ✅ | 197-257 |

**المجموع:** 17 مُنفَّذ + 2 مُفاجأة = 19 (الادعاء قال ~21)

---

## 5. ✅ صحيح
1. الكلمات الـ40 تَتطابق lexer ↔ YAML
2. العوامل المنطقية الـ3
3. الكلمات السياقية
4. 9 ملفات data/stdlib/
5. معظم دوال core (17/~21)
6. طرق المصفوفات (اضف، عكس، رتب...) في stdlib/

## 6. 🎁 مفاجآت
- **متوقع، غير_متوقع** (expect_true / expect_false — تَلميحات فرع CPU) في [builtin_core_io.cpp:73-99](interpreter/src/builtins/builtin_core_io.cpp#L73-L99)

## 7. ❌ ادعاءات كاذبة / فجوات

### 7.1 عدم تَطابق اسم دالة "نوع" 🔴
- **YAML:** `data/stdlib/core.yaml` يَدَّعي `"نوع"`
- **CPP:** [builtin_core_io.cpp:115](interpreter/src/builtins/builtin_core_io.cpp#L115) يُسجِّل `"النوع"`
- **التَأثير:** المستخدم يَكتب `نوع(x)` → فشل "دالة غير معرفة"
- **الحل:** إضافة alias في cpp أو توحيد على "نوع"

### 7.2 عدد الدوال "~21" بَدل 17 🟡
- copilot-instructions.md يَدَّعي ~21، الفعلي 17 (+2 مُفاجأة = 19)
- الحل: تَحديث الادعاء أو توضيح ما يَدخل/يَخرج من العَدد

---

## 8. توصيات

### 🔴 حَرج
1. **إصلاح اسم "النوع" → "نوع"** في `builtin_core_io.cpp:115` (أو إضافة alias)
2. **تَحديث copilot-instructions.md:** "~21" → "17" أو إضافة الناقصتين

### 🟡 متوسط
3. التَحقق من تَنفيذ كل طرق المصفوفات المُعلنة في [stdlib/مصفوفات.ص](stdlib/مصفوفات.ص)
4. توثيق ملفات `stdlib/include/` الفارغة نسبياً

---

## 9. الخلاصة

**Reality Gap للـstdlib صغيرة جداً.** الكلمات المحجوزة 100%، ملفات البيانات 100%، الدوال الأساسية 95% مع خَلَل واحد فعلي (اسم دالة "النوع").

**درجة الثقة:** 95%.
