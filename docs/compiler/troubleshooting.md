# استكشاف أخطاء sadc

## مشكلة Debug Linking مع LLVM
- السبب: عدم توافق Build Type بين LLVM وsadc.
- الحل العملي الحالي: بناء `sadc` على Release عندما تكون LLVM مبنية Release.

## نصائح عامة
- تحقق من `ENABLE_LLVM_BACKEND`.
- راجع مخرجات CMake قبل البناء.
