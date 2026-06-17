# 🔨 ميثاق وكيل γ (Gamma) — Compiler / `sadc`

**النطاق:** المترجم الكامل من AST إلى ملف تنفيذي عبر LLVM 18.
**الحد الأقصى للستوريات المتوازية (WIP):** 2 (بسبب Release-only build batches)

---

## 1. الملكية

### مجلدات مملوكة (تعديل مباشر مسموح)
- `compiler/` (379 ملف، 127K loc) — أكبر مكون فردي في المشروع
  - `compiler/src/frontend/` — SIR Builder + Frontend Optimizer
  - `compiler/src/middle/` — passes متوسطة (DCE, CSE, ...)
  - `compiler/src/backend/` — LLVM CodeGen (20+ ملف بأسماء وصفية)
  - `compiler/src/sir_optimizer/` — تحسينات SIR
  - `compiler/src/meta/` — meta system (utm6)
  - `compiler/src/types/` — type inference للمترجم
  - `compiler/include/frontend/sir_types.h` — أنواع SIR (محروس G-06)
- `compiler/security/` — أمان المترجم (انتبه Mojibake في `compiler_security.cpp`)

### ملفات محروسة في نطاقي (تحتاج موافقة Saleh)
- `compiler/src/middle/dead_code_elimination_pass.cpp` (G-05) — blacklist pattern
- `compiler/src/frontend/sir_frontend_optimizer.cpp` (G-05) — يجب أن يطابق DCE
- `compiler/include/frontend/sir_types.h` (G-06) — enum `SIROpcode`

### مشترك (تنسيق مع γ Lead/Follow)
- `tools/compiler/` (واجهة `sadc` CLI — `compiler_driver_*.cpp`) — مشترك مع δ

### ممنوع تماماً
- `shared/` (نطاق α)
- `interpreter/`, `vm/`, `runtime/` (نطاق β)

---

## 2. ستوريات BACKLOG المُكلَّفة

| ID | العنوان | الدور | شريك |
|---|---|---|---|
| **B-004** | TypePtr Inference | Solo Lead | — |
| **B-003** | Union Types — codegen | Follow | α (Lead) |
| **B-005** | Template Constraints — codegen | Follow | α (Lead) |
| **B-006** | Dispatch Unification — codegen | Follow | β (Lead) **[GUARDED:G-04]** |

---

## 3. قواعد البناء — حرجة جداً

```powershell
# ⚠️ sadc RELEASE-ONLY (Debug يفشل بـ LNK2038 × 4222 mismatches)
cmake --build build --config Release --target sadc

# اختبار ملف
.\build\bin\Release\sadc.exe examples\test_simple.ص -o test.exe
.\test.exe
```

### لماذا Release-only؟
LLVM 18 libs بُنيت Release-only في النظام. ربط `sadc` Debug يخلط Debug runtime مع Release LLVM → 4222 LNK2038 errors.

---

## 4. قواعد حرجة

### 4.1 DCE Blacklist (G-05)
**النمط الحالي:** `default: return true` (= له side effect حتى يُثبت العكس)

```cpp
// الصح
bool hasSideEffect(SIROpcode op) {
    switch (op) {
        case SIROpcode::ADD: case SIROpcode::SUB: return false;  // pure
        // ... قائمة pure operations
        default: return true;  // افتراض آمن: له side effect
    }
}
```

**ممنوع:** عكسه إلى whitelist (`default: return false`) — يُعيد Fix #31/#39/#48 (حذف opcodes جديدة بصمت).

**عند إضافة opcode جديد:**
1. أضفه إلى `sir_types.h` (G-06)
2. **لا** تحتاج تعديل `hasSideEffect` — blacklist يحميه تلقائياً (default=true)
3. إن كان pure فعلاً، أضفه صراحة إلى case البيض

### 4.2 إضافة opcode SIR
1. تعديل `compiler/include/frontend/sir_types.h` (G-06) — يحتاج موافقة Saleh
2. تنفيذ build في `compiler/src/frontend/sir_builder_*.cpp`
3. تنفيذ codegen في `compiler/src/backend/llvm_codegen_*.cpp`
4. إن كان pure → أضفه لقائمة pure في `dead_code_elimination_pass.cpp` (G-05)
5. اختبار في `examples/` + `tests/comprehensive/`

### 4.3 ملفات backend بأسماء وصفية
- تجنب الأسماء القديمة `llvm_codegen_part1.cpp` إلخ
- الأسماء الحالية: `llvm_codegen_init.cpp`, `_memory_control.cpp`, `_output.cpp`, `_objects_arrays.cpp`, ...
- راجع `/memories/repo/compiler_fix_notes.md` للخريطة الكاملة

### 4.4 Mojibake في `compiler/security/compiler_security.cpp`
**حاجز معروف:** الملف يحوي تشفير CP862 (Hebrew DOS) من PowerShell سابق.
**لا تعدله** حتى يُكمل M-INFRA-05 (إصلاح الترميز).

### 4.5 يُمنع تماماً
- PowerShell `Out-File`/`Set-Content` على أي ملف يحوي عربي UTF-8 → استخدم Python `open(rb/wb)`
- تقسيم ملف backend دون نقل bytes كاملة (يفسد UTF-8)

---

## 5. بروتوكول Lead-Follow

كـ Follow لـ α (Union Types, Templates):
1. أنتظر RFC من α
2. أوقّع إن كان sound — أرفض إن كان يكسر DCE blacklist
3. أنفذ بعد تنفيذ α
4. اختبار تكامل في `tests/comprehensive/`

كـ Follow لـ β (Dispatch Unification):
1. أنتظر تصميم unified dispatch table
2. أحدّث codegen لاستدعاء dispatch الموحد
3. أحفظ Backward Compatibility لـ SIR opcodes

---

## 6. اختبارات إجبارية

- [ ] sadc يبني `examples/test_*.ص` بنجاح
- [ ] `comprehensive_tests` 100% (تشمل compiled tests)
- [ ] لا regression في LLVM IR (راجع `--emit-llvm`)
- [ ] قياس DCE قبل/بعد كل ستوري
