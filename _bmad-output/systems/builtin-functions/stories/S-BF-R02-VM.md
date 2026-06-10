---
id: S-BF-R02-VM
title: "تحويل الآلة الافتراضية (sad_vm) للثوابت المُولَّدة"
type: refactor-story
epic: Epic-BF-Refactor
priority: P1
status: completed
estimate: S
date: 2026-06-07
---

# S-BF-R02-VM — تحويل الآلة الافتراضية للثوابت

## القصة
بصفتي **مطوّر اللغة**، أريد **أن تستخدم الآلة الافتراضية (sad_vm) ثوابت `Names::*` المُولَّدة**
بدل النصوص العربية المباشرة، حتى **يكون مصدر أسماء الدوال موحَّداً عبر المسارات الثلاثة (مفسر/مترجم/VM)**.

## السياق (مُحقَّق بالأدلة)
- الـ VM تُسجِّل **11 دالة أصلية** بنص عربي مباشر في `vm/src/sad_vm_executor.cpp`.
  - دليل: `grep -cE 'سجّل_دالة_أصلية\("[^"]*[؀-ۿ]' vm/src/sad_vm_executor.cpp` → **11**
- بعضها له opcode مخصص (`عملية_اطبع = 0x90` في `vm/include/sad_vm_opcodes.h`).

## معايير القبول
- **Given** الـ header المُولَّد متاح (`builtin_registry.h`)
- **When** نستبدل كل `سجّل_دالة_أصلية("اطبع", …)` بـ `سجّل_دالة_أصلية(std::string(Bn::Core::PRINT), …)`
- **Then** `cmake --build build --target sad_vm` أخضر + الأسماء من YAML

## المهام
- [ ] إضافة `#include "builtin_registry.h"` + `namespace Bn = Sad::Builtins::Names;` في `sad_vm_executor.cpp`
- [ ] استبدال 11 تسجيل بالثوابت المقابلة:
  - اطبع→Core::PRINT، اطبع_سطر→Core::PRINTLN، ادخل→Core::READ
  - طول→Core::LENGTH، نوع→Core::TYPE
  - رقم→TypeCtor::TO_INT، عشري→TypeCtor::TO_FLOAT، نص→TypeCtor::TO_STRING
  - لنص/لرقم/لعشري → (تحقق من وجود ثوابت أو أضِفها لـ YAML)
- [ ] التحقق: opcodes (`عملية_اطبع`) تبقى كما هي (طبقة منفصلة عن أسماء التسجيل)
- [ ] بناء أخضر + اختبار `.ص` عبر الـ VM

## المصدر في الكود
- `vm/src/sad_vm_executor.cpp` (السطر 1736+ — `سجّل_الدوال_الأصلية`)
- `vm/include/sad_vm_opcodes.h` (opcodes الدوال)

## تعريف "تم"
صفر نص عربي في `سجّل_دالة_أصلية` + بناء أخضر + الأسماء من الثوابت المُولَّدة.
