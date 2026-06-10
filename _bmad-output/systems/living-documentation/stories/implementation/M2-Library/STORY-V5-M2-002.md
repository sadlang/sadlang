---
id: S-V5-M2-002
title: "تنفيذ Wrapper libsadlangtruth"
type: implementation-story
milestone: M2-Library
scope: wrapper-impl
status: ready
estimate: L
depends_on: [S-V5-M2-001]
date: 2026-06-05
governed_by: [ADR-004, ADR-003]
---

# S-V5-M2-002 — تنفيذ Wrapper

## القصة
بصفتي **المكتبة**، أريد **تنفيذ المُحوّلات فوق C++ Generated** حتى **توفّر بحثاً O(1) صفر I/O مع تهيئة كسولة آمنة للخيوط**.

## معايير القبول
- **Given** الواجهة مصمَّمة (M2-001) + كود Generated موجود
- **When** نكتب `keyword_adapter.cpp`/`builtin_adapter.cpp`/`error_adapter.cpp`
- **Then** كل بحث O(1) عبر `unordered_map` + تهيئة `std::call_once` + صفر قراءة ملفات

## المهام
- [ ] `keyword_adapter.cpp` يبني فهرس من `Sad::Lexer::Generated::*`
- [ ] `builtin_adapter.cpp` من `Sad::Builtins::Generated::*`
- [ ] `error_adapter.cpp` للرسائل
- [ ] تهيئة كسولة `std::call_once` (CW-16 RAII)
- [ ] لا I/O وقت التشغيل (تحقُّق بفحص الكود)

## المصدر في الكود
- جديد: `shared/langtruth/src/*_adapter.cpp`

## تعريف "تم"
كل المُحوّلات تُترجم + بحث O(1) مُختبَر + صفر I/O + بناء أخضر.
