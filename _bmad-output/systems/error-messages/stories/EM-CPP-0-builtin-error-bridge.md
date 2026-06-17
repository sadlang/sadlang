# Story EM-CPP-0 — تقديم BuiltinContext (المُمكِّن)

**Epic**: [EPIC-EM-CPP](../epics/EPIC-EM-CPP-MIGRATION.md) | **النقاط**: 8 | **الأولوية**: حرجة | **الحالة**: جاهزة
**التبعيات**: لا شيء | **يحكمها**: [ADR-EM-CPP-1](../decisions/ADR-EM-CPP-1-BUILTIN-CONTEXT.md) | **المُسنَد إلى**: TBD

---

## الهدف
تقديم **`BuiltinContext`** كعقد استدعاء **وحيد** للدوال المضمنة — يحمل الوسائط + الموقع + واجهة
أخطاء الكتالوج (`ctx.error`). **استبدال نظيف بلا توافق خلفي** (اللغة غير منشورة — قرار المالك).

> **التصميم التفصيلي المعتمَد:** [BUILTIN_CONTEXT_DESIGN.md](../docs/BUILTIN_CONTEXT_DESIGN.md)
> (الواجهة الكاملة + `callNative(args,pos)` + overload التسجيل + 3 نقاط الاستدعاء + نمط الترحيل).

## التصميم (ADR-EM-CPP-1)
```cpp
class BuiltinContext {
    const std::vector<std::shared_ptr<Value>>& args() const;
    std::size_t argCount() const;
    const Sad::Lexer::Position& position() const;          // يضبطه الموزّع
    [[noreturn]] void error(ErrorCode, std::map<std::string,std::string> = {});  // كتالوج + موقع
    std::string_view functionName() const;                 // قابل للتوسّع
};
using BuiltinFn = std::function<std::shared_ptr<Value>(BuiltinContext&)>;
```

## التنفيذ (Tasks)
- [ ] **T1** — `builtin_context.h/.cpp`: صنف `BuiltinContext` (تعليق مزدوج اللغة).
  `error()` يستدعي `Sad::Errors::throwRuntime(code, position(), placeholders)`.
- [ ] **T2** — `FunctionDefinition`: تغيير نوع `nativeImplementation_` إلى `NativeCtx`
  (`function<ValuePtr(BuiltinContext&)>`) + `callNative(args, pos)` يبني السياق ويستدعيه.
  تغيير `registerBuiltinFunction` لقبول `NativeCtx`. **لا توقيع قديم.**
- [ ] **T3** — 3 نقاط الاستدعاء تمرّر الموقع: `calls_dispatch.cpp:526`, `interpreter_core.cpp:645`,
  `oop_string_map_methods.cpp:578` → `callNative(valuePtrs, node.position)`.
- [ ] **T4** — اختبار: دالة مضمنة بالتوقيع الجديد تستدعي `ctx.error(ErrorCode::RUN_TYPE_CHECK_FAILED)`
  → يُعرَض `[RUN036]` بموقع الاستدعاء الصحيح.

## ⚠️ وحدة ذرّية (قرار المالك: استبدال صارم بلا توافق خلفي)
بحذف التوقيع القديم، **كل الدوال المضمنة القائمة لن تُجمَّع** حتى تُحوَّل. لذلك **EM-CPP-0..6
وحدة دمج ذرّية واحدة** — البناء يبقى **أحمر** بينها ويُصبح **أخضر فقط بعد اكتمال EM-CPP-6**.
EM-CPP-0 لا يُدمَج وحده.

## معايير القبول (لـ EM-CPP-0 كجزء من الوحدة الذرّية)
- **Given** البنية الجديدة (`BuiltinContext` + `callNative(args,pos)` + تسجيل `NativeCtx` فقط)
- **When** تُستدعى دالة محوَّلة بالتوقيع الجديد
- **Then** `ctx.error` يُرندر من الكتالوج بموقع الاستدعاء (`[CODE]` + رسالة YAML) — بلا نص حر/حالة عامة
- **And** التوقيع القديم `(args)` **محذوف تماماً**
- **And** (على مستوى الوحدة الذرّية) البناء أخضر بعد EM-CPP-6

## تعريف "تم"
`BuiltinContext` + `callNative(args,pos)` + تسجيل `NativeCtx` فقط + 3 نقاط استدعاء مُحدَّثة، جاهزة
ليبني عليها EM-CPP-1..6. (الأخضر الشامل عند اكتمال الوحدة.)
