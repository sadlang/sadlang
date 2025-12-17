# الجدول الزمني للمشروع
# Project Timeline

**التاريخ / Date:** 2 ديسمبر 2025  
**الإصدار / Version:** 1.0

---

## 📅 الجدول الزمني الكامل / Complete Timeline

### (AR) العربية

**المدة الإجمالية:** 34-50 أسبوع (8-12 شهر)

---

## 📊 المراحل والجدول الزمني / Phases & Schedule

```
┌─────────────────────────────────────────────────────────────────────┐
│ Timeline: Sad Compiler Development (34-50 weeks)                    │
└─────────────────────────────────────────────────────────────────────┘

المرحلة 0: التحضير (1-2 أسابيع)
├── الأسبوع 1: توثيق المفسّر + تصميم SIR
└── الأسبوع 2: إعداد بنية المشروع + اختبارات

المرحلة 1: Frontend (4-6 أسابيع)
├── الأسبوع 3-4: Lexer & Parser محسّن
├── الأسبوع 5-6: Type Checker
└── الأسبوع 7-8: Semantic Analyzer

المرحلة 2: Middle-end (3-4 أسابيع)
├── الأسبوع 9-10: SIR Emitter
└── الأسبوع 11-12: Optimizer Passes

المرحلة 3: Bytecode Backend (4-6 أسابيع)
├── الأسبوع 13-14: Bytecode Format
├── الأسبوع 15-16: VM Implementation
└── الأسبوع 17-18: Runtime Library

المرحلة 4: LLVM Backend (8-12 أسابيع)
├── الأسبوع 19-22: LLVM IR Generator
├── الأسبوع 23-26: Optimization Integration
└── الأسبوع 27-30: Cross-compilation

المرحلة 5: Runtime & ABI (4-6 أسابيع)
├── الأسبوع 31-32: C ABI Integration
├── الأسبوع 33-34: FFI Implementation
└── الأسبوع 35-36: System Calls Wrapper

المرحلة 6: التحسينات (6-8 أسابيع)
├── الأسبوع 37-40: JIT Compilation
├── الأسبوع 41-42: LTO & PGO
└── الأسبوع 43-44: Advanced GC

المرحلة 7: الأدوات (4-6 أسابيع)
├── الأسبوع 45-46: LSP Implementation
├── الأسبوع 47-48: Debugger Integration
└── الأسبوع 49-50: Package Manager
```

---

## 🎯 المعالم الرئيسية / Major Milestones

### (AR) العربية

| الأسبوع | المعلم | الوصف | المخرجات |
|---------|--------|-------|----------|
| 2 | ✅ التحضير مكتمل | SIR مُصمم، بنية جاهزة | SIR spec، 20 tests |
| 8 | ✅ Frontend مكتمل | Parser & Type Checker | AST + Type info |
| 12 | ✅ Middle-end مكتمل | SIR generator + optimizer | Optimized SIR |
| 18 | ✅ Bytecode مكتمل | VM يشغّل Stage 1 | sadvm + libsad |
| 30 | ✅ LLVM مكتمل | Native executables | sadc --llvm |
| 36 | ✅ Runtime مكتمل | FFI & C ABI | libsad.so |
| 44 | ✅ Optimizations مكتملة | JIT + LTO | Performance 50x |
| 50 | ✅ v1.0 Release | جميع الميزات | Full compiler |

---

## 📈 خطة التسليم / Delivery Plan

### (AR) العربية

**النسخ / Releases:**

**Alpha (الأسبوع 18):**
- ✅ Bytecode VM يعمل
- ✅ يشغّل Stage 1 tests
- ⚠️ بدون LLVM backend
- 🎯 للاختبار الداخلي فقط

**Beta (الأسبوع 30):**
- ✅ LLVM Backend يعمل
- ✅ Native executables
- ✅ Cross-compilation
- 🎯 للاختبار العام

**RC (الأسبوع 44):**
- ✅ جميع التحسينات
- ✅ JIT + LTO
- ✅ Performance targets met
- 🎯 قبل الإطلاق النهائي

**v1.0 (الأسبوع 50):**
- ✅ جميع الميزات
- ✅ Documentation كاملة
- ✅ Stable API
- 🎯 الإطلاق الرسمي

---

## 🔄 التبعيات / Dependencies

```
Phase 0 (Prep)
    ↓
Phase 1 (Frontend) ←─────────┐
    ↓                         │
Phase 2 (Middle-end)          │
    ↓                         │
    ├─→ Phase 3 (Bytecode)    │
    │       ↓                 │
    │   Phase 5 (Runtime) ────┘
    │
    └─→ Phase 4 (LLVM)
            ↓
        Phase 6 (Optimizations)
            ↓
        Phase 7 (Tooling)
```

**يمكن التوازي:**
- Phase 3 (Bytecode) و Phase 4 (LLVM) بعد Phase 2
- Phase 5 (Runtime) مع Phase 3 أو 4
- Phase 7 (Tooling) في أي وقت بعد Phase 1

---

## 📊 توزيع الجهد / Effort Distribution

### (AR) العربية

| المرحلة | النسبة | الأسابيع | الأولوية |
|---------|--------|----------|----------|
| Frontend | 15% | 4-6 | P0 |
| Middle-end | 10% | 3-4 | P0 |
| Bytecode | 15% | 4-6 | P0 |
| LLVM | 30% | 8-12 | P1 |
| Runtime | 15% | 4-6 | P0 |
| Optimizations | 10% | 6-8 | P2 |
| Tooling | 5% | 4-6 | P2 |

---

## 🎓 فريق العمل المقترح / Suggested Team

### (AR) العربية

**الأدوار:**
- **مهندس Frontend:** Lexer, Parser, Type Checker
- **مهندس IR:** SIR, Optimizer
- **مهندس Backend:** Bytecode + LLVM
- **مهندس Runtime:** Memory, FFI, GC
- **مهندس QA:** Testing, CI/CD
- **Tech Lead:** Architecture, Review

**الحد الأدنى:** 2-3 مطورين بدوام كامل  
**الأمثل:** 4-6 مطورين

---

**السابق / Previous:** [المرحلة 7: الأدوات](10_phase7_tooling.md)  
**التالي / Next:** [الاختبارات](12_testing.md)
