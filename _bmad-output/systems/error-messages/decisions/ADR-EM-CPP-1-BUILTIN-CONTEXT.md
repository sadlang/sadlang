---
id: ADR-EM-CPP-1
title: BuiltinContext — عقد استدعاء الدوال المضمنة (الأساس المستقبلي)
status: Accepted
date: 2026-06-10
deciders:
  - المالك (صالح)
  - Amelia (Dev)
authority: SoT
system: error-messages (يؤثر على builtin-functions)
relatedEpic: ../epics/EPIC-EM-CPP-MIGRATION.md
relatedGuide: ../docs/ERROR_SYSTEM_GUIDE.md
---

# ADR-EM-CPP-1 — BuiltinContext كعقد استدعاء الدوال المضمنة

## السياق
أثناء التخطيط لترحيل ~310 موقع رمي خام في C++ إلى نظام الكتالوج المُولَّد، تبيّن أن الدوال
المضمنة (254 موقعاً) **لا تستطيع استدعاء `throwRuntime`/`reportFromCatalog`** لأنها تتطلب
`Position`، وتوقيع الدالة المضمنة `std::function<Value(const vector<Value>& args)>`
(`function_manager.h:156`) **لا يحمل موقعاً ولا سياقاً**، ولا يوجد «موقع حالي» في المفسر.

طُرحت ثلاثة حلول: (أ) جسر استثناء `BuiltinError`، (ب) موقع thread-local، (ج) تمرير سياق للدوال.
المالك طلب **الحل الأمثل بعيد المدى بغضّ النظر عن التكلفة**.

## القرار
**تُعاد هيكلة عقد الدوال المضمنة لتأخذ `BuiltinContext&` بدل `(const vector<Value>& args)`.**

```cpp
// العقد الجديد:
using BuiltinFn = std::function<std::shared_ptr<Value>(BuiltinContext&)>;

class BuiltinContext {
public:
    // الوسائط (كما كانت)
    const std::vector<std::shared_ptr<Value>>& args() const;
    std::size_t argCount() const;

    // الموقع (call-site) — يضبطه الموزّع قبل الاستدعاء
    const Sad::Lexer::Position& position() const;

    // أخطاء من الكتالوج المُولَّد مباشرةً (لا نص حر، لا جسر)
    [[noreturn]] void error(ErrorCode code,
                            std::map<std::string,std::string> placeholders = {});

    // قابل للتوسّع لاحقاً: اسم الدالة، المفسر، GC، تتبّع المكدس...
    std::string_view functionName() const;
};
```

داخل الدالة المضمنة:
```cpp
auto cbrt = [](BuiltinContext& ctx) -> std::shared_ptr<Value> {
    if (ctx.argCount() != 1 || !ctx.args()[0]->isNumeric())
        ctx.error(ErrorCode::RUN_TYPE_CHECK_FAILED, {{"func","جذر_تكعيبي"}}); // كتالوج + موقع
    ...
};
```

## الأساس (لماذا هذا هو الأمثل)
- **النمط الصناعي القياسي:** Lua (`lua_State*`)، V8 (`FunctionCallbackInfo`)، Python C-API
  (`self`+`args`)، Rust (Context) — كلها تمرّر **سياقاً** لا وسائط مجرّدة.
- **قابل للتوسّع:** أي احتياج مستقبلي (اسم الدالة، فهرس الوسيط، القيم، المكدس، GC، المقيّم)
  يُضاف للسياق دون تغيير التواقيع ثانيةً.
- **بلا حالة عامة:** السياق مُمرَّر صراحةً (testable — CW-23)، بعكس thread-local.
- **موحّد:** `BuiltinContext` واجهة يمكن أن يُحقّقها المفسر **والمترجم** (sadc) → دوال مضمنة مشتركة.
- **يحلّ الأخطاء جذرياً:** `ctx.error(code, placeholders)` يستدعي مسار الكتالوج بموقع صحيح —
  بلا نص حر، بلا جسر، بلا thread-local.

## البدائل المرفوضة
- **جسر `BuiltinError`:** يحلّ الموقع فقط، يترك الدوال بلا سياق (دَيْن مستقبلي).
- **thread-local position:** حالة عامة تضرّ الاختبار وتُخفي التبعية.

## التبعات
- **إيجابية:** أساس صحيح ودائم لكل الدوال المضمنة + حلّ الأخطاء + توحيد مفسر/مترجم.
- **تكلفة:** إعادة هيكلة توقيع ~287 دالة مضمنة + التسجيل + الموزّع — **استبدال نظيف بلا توافق خلفي**
  (اللغة غير منشورة)؛ كل الوحدات تُحوَّل في دفعة الهجرة، الدمج بعد بناء أخضر شامل.

## استراتيجية الهجرة (استبدال نظيف — بلا توافق خلفي)
> **قرار المالك (2026-06-10):** اللغة **غير منشورة** → لا توافق خلفي. توقيع `(BuiltinContext&)`
> هو الوحيد (يُحذف `(args)` تماماً). كل الدوال تُحوَّل في دفعة الهجرة؛ لا shim انتقالي.
1. تقديم `BuiltinContext` + تغيير نوع التسجيل إلى `NativeCtx` فقط (`EM-CPP-0`).
2. تحويل كل وحدات الدوال المضمنة للتوقيع الجديد + `ctx.error` (`EM-CPP-1..6`، متتابعة، دمج بعد بناء أخضر شامل).

## نطاق المترجم (sadc)
`BuiltinContext` **خاص بالمفسر** (lambdas). المترجم يولّد LLVM IR لا lambdas. **الطبقة حسب الهدف
لا حسب مفسر/مترجم:**
- **أخطاء وقت الترجمة** (`reportError` ×11) → `reportFromCatalog(ErrorCode)` (Tier 1) — `EM-CPP-7`.
- **أخطاء الكود المُترجَم — هدف مُضيف** → **Tier 1 غني** (runtime مُضيف يرندر عبر `ErrorCatalog`، كالمفسر).
- **أخطاء الكود المُترجَم — freestanding** → **Tier 2** (`sad_panic` + جدول مُولَّد بلا STL).
- codegen يُصدر نداءً موحَّداً `sad_runtime_error(code,...)`؛ الـruntime المربوط يقرّر الغنى →
  ملحق **`EM-CPP-T2`**. التفصيل: `docs/BUILTIN_CONTEXT_DESIGN.md` §9 + الدليل §3-ب.

## الحالة
`Accepted` (2026-06-10). يُعاد تصميم `EPIC-EM-CPP` على أساسه. يُنفَّذ بدءاً بـ`EM-CPP-0`
(تقديم `BuiltinContext` + التوافق الخلفي).
