# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project aims to follow Semantic Versioning.

## [1.0.0-beta.1] - 2026-03-08

### الجديد (Added)
- 🤖 **Android SDK كامل** — 35+ ملف Kotlin/JNI
  - SadInterpreter, SadStorage, SadAudio, SadNetwork, SadNotifications
  - SadNfc, SadHealth, SadMaps, SadBilling, SadFirebase
  - SadWorker, SadBluetooth, SadPermissions
  - Compose UI مع محرر كود ومحطة REPL
- 🍎 **iOS SDK** — SwiftUI مع CoreML و HealthKit
- 🌐 **WebAssembly** — مفسر ص في المتصفح
- 📱 **نظام الواجهات (UI Pipeline)** — توليد تلقائي لجميع المنصات
- 🎓 **إطار عمل الاختبارات العربي** — `اختبر` و `تأكد`
- 📚 **منهج تعليمي متكامل** — 8 أسابيع للمبتدئين

### تحسينات (Changed)
- تحسين أداء المفسر
- تحديث نظام الأنواع للدعم الكامل للـ OOP
- تحسين رسائل الخطأ بالعربية

### إصلاحات (Fixed)
- إصلاحات متعددة للثبات والتوافق
- حل مشاكل الملكية (Ownership) في الكائنات

## [Unreleased]
### Added
- Root governance and legal files: `LICENSE`, `CODE_OF_CONDUCT.md`, `SECURITY.md`.
- Annual-plan execution artifacts under docs and examples for the 2026 roadmap.
- LLVM Debug/Release mismatch guard in `cmake/llvm.cmake` with clear warnings.
- `analyzeExports()` implementation in `shared/modules/src/module.cpp` — now extracts exported symbols from AST.
- `checkConflicts()` implementation in `compiler_new/src/types/parallel_solver.cpp`.
- ✅ **أنواع جبرية (ADT)** — `تعداد` مع حقول بيانات، مدعوم في المفسر والمترجم (22 اختبار).
- ✅ **Result/Option + عامل انشر** — `نتيجة`/`اختياري` عبر ADT + `انشر` للإرجاع المبكر.
- ✅ **أسماء مستعارة للأنواع** — `نوع اسم = هدف`، مفسر + مترجم.
- ✅ **صفوف (Tuples)** — `(قيمة1، قيمة2)` + تفكيك + `.0 .1`، مفسر + مترجم (15 اختبار).
- ✅ **أصناف محكمة (Sealed Classes)** — `محكم صنف`، مفسر + مترجم + حماية عبر الملفات (10 اختبارات).
- ✅ **تأجيل مضمون (Defer)** — `أجّل` كلمة سياقية، LIFO، جملة/كتلة، مفسر + مترجم (10 اختبارات).

### Changed
- `README.md` refreshed with accurate structure, setup, and references.
- `docs/ANNUAL_PLAN_2026.md` updated with execution status, code quality report, and deliverables tracking.

### Fixed
- 15× `static std::string` → `thread_local` across C API functions for thread safety.
- 8× null pointer dereference checks added in C API boundary functions.
- 3× `reinterpret_cast<uintptr_t>` → `toString()` for semantic hash keys.
- `std::atomic<uint64_t>` for thread-safe actor ID counter in `actor_type.cpp`.
- Template return type substitution in `sir_builder_templates.cpp`.
- `TypeCheckError` struct-based error reporting in `type_checker.cpp`.
- Hash improvement with golden ratio bit mixing in `type_registry.cpp`.
- Bounds validation for constraint cache deserialization.
- `nightly.yml`: Fixed `$(date)` shell expansion in YAML fields → `steps.date.outputs`.
- Documentation links and repository-level project metadata consistency.

## [2.0.0] - 2026-03-06
### Added
- Stable Arabic-language tooling set around Sad Language:
  - `sad` interpreter
  - `sadc` compiler pipeline
  - LSP, formatter, package manager, and REPL toolchain
- Expanded comprehensive testing suites and regression coverage.

### Changed
- Significant updates across interpreter, compiler, stdlib modules, and docs.

### Fixed
- Multiple parser/interpreter/compiler regressions tracked in Sprint plans.

## [1.0.0] - 2025-01-01
### Added
- Initial public baseline of Sad Language core project structure.
