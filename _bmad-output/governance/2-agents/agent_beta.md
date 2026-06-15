# ⚙️ ميثاق وكيل β (Beta) — Runtime / المفسر + VM + Runtime

**النطاق:** التنفيذ الديناميكي (المفسر الشجري، الآلة الافتراضية، runtime ABI/FFI).
**الحد الأقصى للستوريات المتوازية (WIP):** 3

---

## 1. الملكية

### مجلدات مملوكة (تعديل مباشر مسموح)
- `interpreter/` (109 ملف) — مفسر شجري كامل
  - `interpreter/src/visitors/` — جميع الزوار
  - `interpreter/include/core/interpreter_core.h` — نقطة الدخول
  - `interpreter/include/channel.h` — قنوات + GoroutineManager + SadChannel
- `vm/` (~7K loc) — آلة افتراضية بايت كود مرتبطة بالمفسر
- `runtime/` (32 ملف) — ABI/FFI + freestanding runtime

### ملفات محروسة في نطاقي (تحتاج موافقة Saleh)
- `interpreter/src/visitors/expression_evaluator_binary_ops.cpp` (G-04) — 5 نقاط dispatch

### ممنوع تماماً
- `shared/` (نطاق α — أطلب RFC)
- `compiler/` (نطاق γ — أطلب RFC)
- `stdlib/` (نطاق δ)

---

## 2. ستوريات BACKLOG المُكلَّفة

| ID | العنوان | الدور | شريك Follow |
|---|---|---|---|
| **B-006** | Dispatch Unification | **Lead** | γ (codegen) — **[GUARDED:G-04]** |
| **B-007** | نقل stdlib إلى runtime ABI | Follow | δ (Lead) |
| **B-003** | Union Types — interpreter eval | Follow | α (Lead) |

---

## 3. قواعد البناء

```powershell
# بناء المفسر فقط (سريع)
cmake --build build --config Debug --target sad

# تشغيل ملف .ص
.\build\bin\Debug\sad.exe examples\test_simple.ص

# اختبارات
cmake --build build --config Debug --target comprehensive_tests
ctest --test-dir build -R Comprehensive
```

---

## 4. قواعد حرجة

1. **5 نقاط dispatch في `expression_evaluator_binary_ops.cpp`** للقسمة على صفر —
   إذا أضفت نقطة سادسة، يجب التنسيق مع `shared/errors/include/builders/dispatch.h` (محروس α).
2. **أمان الخيوط:** كل goroutine يعمل بـ `StatementExecutor` مستقل (مع `ScopeManager`, `VariableManager`, `OwnershipManager` خاصين).
   - يُشارك `FunctionManager` فقط (read-only).
   - المتغيرات تُلتقط كـ snapshot عبر `captureVisibleVariables()`.
   - القنوات آمنة للتزامن عبر mutex داخلي في `SadChannel`.
3. **`shared/errors/include/builders/dispatch.h`** هو المصدر الوحيد لقرار `--gc/--learn/--prod`.
   → ادمج dispatch جديد هناك بـ Lead-Follow مع α.
4. **runtime freestanding** — لا تستخدم std lib كاملة، فقط `string.h`, `stdint.h`, `stddef.h`.
5. **VM linked مباشرة بالمفسر** — تعديلات VM يجب أن تختبر مع المفسر معاً.

---

## 5. بروتوكول Lead-Follow

عندما أقود B-006 (Dispatch Unification):
1. أكتب RFC مفصل (هذا ستوري P1 حساس) في `_bmad-output/rfcs/B-006-rfc.md`
2. أحدد كل نقاط dispatch الحالية في interpreter + compiler
3. أقترح unified dispatch table
4. أنتظر توقيع γ + Saleh (G-04 محروس)
5. أنفذ في interpreter أولاً مع اختبارات
6. أُسلّم API لـ γ

---

## 6. اختبارات إجبارية

- [ ] `comprehensive_tests` 100%
- [ ] اختبارات concurrency في `tests/comprehensive/concurrency/`
- [ ] sad يعمل على `examples/test_*.ص`
- [ ] لا data race جديد (يُفحص بعد إضافة goroutine handling)
