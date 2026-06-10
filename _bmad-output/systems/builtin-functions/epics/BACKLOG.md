# Backlog — نظام Builtins Codegen

> آخر تحديث: 2026-06-07. الحالات مُحقَّقة بأدلة من الكود (GR-01).

## Epic-BF-001: إنشاء YAML الكامل للدوال المدمجة ✅

| Story | العنوان | الحالة |
|-------|---------|--------|
| S-BF-001 | YAML: Core + TypeCtor + Concurrency | ✅ |
| S-BF-002 | YAML: Math + Strings + Arrays | ✅ |
| S-BF-003 | YAML: Basics + Assertions + Maps | ✅ |
| S-BF-004 | YAML: Sockets + HTTP + WebSocket | ✅ |

> دليل: `ls language-truth/builtins/*.yaml | wc -l` → 41 ملف

## Epic-BF-002: gen_builtins_registry.py ✅

| Story | العنوان | الحالة |
|-------|---------|--------|
| S-BF-005 | كتابة gen_builtins_registry.py | ✅ |
| S-BF-006 | اختبارات gen_builtins_registry.py | ⚠️ (تحقق يدوي بالبناء) |

## Epic-BF-003: cmake + تكامل ✅

| Story | العنوان | الحالة |
|-------|---------|--------|
| S-BF-007 | cmake target + تحويل builtin_registry.h لـ wrapper | ✅ |

> دليل: `grep sad_builtin_registry_codegen cmake/codegen.cmake` + البناء أخضر

## Epic-BF-Refactor: ربط الكود بالثوابت (اسم واحد = ثابت)

| Story | العنوان | الحالة |
|-------|---------|--------|
| S-BF-R01 | تحويل builtin_core_io.cpp | ✅ |
| (متعدد) | المفسر بالكامل (builtin_*, oop_*, kernel_*) | ✅ |
| (متعدد) | المترجم بالكامل (builders/builtins_*, method_call_*) | ✅ |
| S-BF-R02-VM | تحويل الآلة الافتراضية (sad_vm) | ⏳ (11 تسجيل متبقٍّ) |

> دليل المفسر: `grep -rlE 'registerBuiltinFunction\("[؀-ۿ]' interpreter/src/builtins/` → 0 (كود)
> دليل المترجم: `grep -rlE 'funcName == "[؀-ۿ]' compiler/src/frontend/builders/` → 0

## خارج النطاق (أنظمة language-truth أخرى)

| البند | النظام المسؤول |
|-------|----------------|
| كلمات `باني`/`هدم` في backend | keywords.yaml |
| سمات LLVM (`مضمن_دائماً`) | attributes (مقترح) |
| عمليات ذرية (`تحميل`) | directives.yaml |
| أسماء الأنواع (`رقم`/`نص`) | types.yaml |
| جداول إكمال LSP | M2-Library |
