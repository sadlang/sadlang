---
id: S-BF-R01
title: "تحويل builtin_core_io.cpp — Core+TypeCtor+Concurrency"
type: refactor-story
epic: Epic-BF-Refactor
priority: P0
status: not-started
estimate: S
date: 2026-06-06
---

# S-BF-R01 — تحويل builtin_core_io.cpp

## القصة
بصفتي **مطوّر اللغة**، أريد **استبدال جميع النصوص العربية في `builtin_core_io.cpp` بثوابت مُولَّدة** حتى **يكون مصدر الحقيقة الوحيد لأسماء الدوال هو YAML**.

## المصدر
`interpreter/src/builtins/builtin_core_io.cpp`

## الدوال المستهدفة (15)
- Core: اطبع، اطبع_سطر، اقرأ، طول، نوع
- TypeCtor: رقم، عشري، نص، منطقي
- Concurrency: قناة، انتظر_الكل، عدد_المهام، مجموعة_انتظار، قفل، مستقبل

## المهام
- [ ] إضافة `#include "builtin_registry.h"` + `namespace Bn = Sad::Builtins::Names;`
- [ ] استبدال كل `registerBuiltinFunction("اطبع", ...)` → `registerBuiltinFunction(std::string(Bn::Core::PRINT), ...)`
- [ ] نفس الشيء لجميع الدوال الـ 15
- [ ] التحقق: `cmake --build build --target sad` أخضر
- [ ] التحقق: اختبار وظيفي — اطبع_سطر("مرحبا") يعمل

## تعريف "تم"
لا نص عربي مباشر في الملف + البناء أخضر + اختبار وظيفي يمر.
