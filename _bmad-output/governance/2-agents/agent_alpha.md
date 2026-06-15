# 🌍 ميثاق وكيل α (Alpha) — Frontend / النواة المشتركة

**النطاق:** نواة اللغة المشتركة بين المفسر والمترجم.
**الحد الأقصى للستوريات المتوازية (WIP):** 2

---

## 1. الملكية

### مجلدات مملوكة (تعديل مباشر مسموح)
- `shared/lexer/` — تحليل معجمي + جدول الكلمات
- `shared/parser/` — تحليل نحوي + بناء AST
- `shared/ast/` — تعريفات عقد AST + visitor pattern (باستثناء المحروس)
- `shared/types/` — نظام الأنواع + `Value`
- `shared/errors/` — أنواع الأخطاء + builders (باستثناء `dispatch.h`)
- `shared/ownership/` — فحص الملكية + lifetimes
- `shared/semantic/` — التحليل الدلالي
- `shared/profiler/` — أداء التحليل
- `shared/sadinfo_core/` — معلومات وقت الترجمة
- `shared/builtins/` — تسجيل الدوال المضمنة
- `shared/memory_policy/` — سياسات الذاكرة
- `shared/runtime/` — runtime types المشتركة
- `shared/UI/` — مكونات UI داخلية للنواة

### ملفات محروسة في نطاقي (تحتاج موافقة Saleh)
- `shared/lexer/include/token.h` (G-01)
- `shared/lexer/src/lexer_keywords.cpp` (G-01)
- `shared/types/include/value.h` (G-02)
- `shared/ast/include/ast.h` + `ast_visitor.h` (G-03)
- `shared/errors/include/builders/dispatch.h` (G-04)

### ممنوع تماماً
- أي مجلد خارج `shared/` (يجب التنسيق Lead-Follow)
- `_bmad/`, `_bmad-output/`, `.github/copilot-instructions.md`

---

## 2. ستوريات BACKLOG المُكلَّفة

| ID | العنوان | الدور | شريك Follow |
|---|---|---|---|
| **B-003** | Union Types | **Lead** | γ (codegen), β (interpreter eval) |
| **B-005** | Template Constraints | **Lead** | γ (codegen) |
| **B-008** | Architecture Refactor #4 | Solo | — |

---

## 3. قواعد البناء

```powershell
# بناء سريع للتحقق (Debug عادي للنواة)
cmake --build build --config Debug --target sad_core

# اختبارات شاملة بعد كل تغيير
cmake --build build --config Debug --target comprehensive_tests
ctest --test-dir build -R Comprehensive
```

---

## 4. قواعد حرجة

1. **`BorrowChecker` و `ASTPrinter`** يرثان `ASTVisitor` مباشرة (لا عبر `BaseASTVisitor`).
   → أي virtual جديد بدون default يكسر البناء فوراً.
2. **عقد الكلمات الـ40** ثابت — توسيع `lexer_keywords.cpp` يحتاج موافقة Saleh.
3. **ملفات C++ تحوي عربي UTF-8** — Python `open(rb/wb)` فقط، لا PowerShell.
4. **تعليقات API ثنائية اللغة** (AR/EN brief) — إلزامية لكل دالة عامة.

---

## 5. بروتوكول Lead-Follow

عندما أقود ستوري (B-003, B-005):
1. أكتب RFC في `_bmad-output/rfcs/B-XXX-rfc.md` (≤2 صفحة)
2. أنتظر توقيع γ + β (إن وُجدا)
3. أنفذ الجزء الخاص بي أولاً مع وحدات اختبار
4. أُسلّم API stub لـ Follow
5. أراجع تكامل Follow في `tests/comprehensive/`

---

## 6. اختبارات إجبارية قبل إغلاق أي ستوري

- [ ] `comprehensive_tests` تمر 100%
- [ ] `parser_tests` لا تتراجع
- [ ] sad (المفسر) يعمل على `examples/test_*.ص`
- [ ] sadc (المترجم) يبني `examples/` بـ Release
- [ ] لا تحذيرات جديدة من المترجم
