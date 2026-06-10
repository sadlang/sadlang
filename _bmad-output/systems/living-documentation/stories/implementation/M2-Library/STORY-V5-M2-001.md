---
id: S-V5-M2-001
title: "تصميم واجهة libsadlangtruth العامة"
type: implementation-story
milestone: M2-Library
scope: api-design
status: ready
estimate: M
depends_on: [S-V5-M1-001]
date: 2026-06-05
governed_by: [ADR-004, ADR-003]
---

# S-V5-M2-001 — تصميم API libsadlangtruth

## القصة
بصفتي **مستهلِك للحقيقة اللغوية (LSP/Compiler/Docs)**، أريد **واجهة C++ ثابتة وثنائية اللغة للوصول إلى كل الكيانات** حتى **أقرأ الحقيقة دون yaml-cpp ولا I/O وقت التشغيل**.

## معايير القبول
- **Given** نطاقات M1 معرَّفة
- **When** نصمّم header `shared/langtruth/include/sad/langtruth.h`
- **Then** الواجهة تكشف Views للقراءة فقط + بحث O(1) + توثيق ثنائي اللغة

## المهام
- [ ] تصميم `KeywordView`/`BuiltinView`/`TypeMethodView`/... (POD/string_view للقراءة)
- [ ] تصميم `Registry`: `find(id)`، `get(id)`، `getAll(scope)`
- [ ] تثبيت ABI: لا تبعية على yaml-cpp في الواجهة العامة
- [ ] توثيق `@brief (AR)` + `@brief (EN)` لكل API عام (CW-08)

## المصدر في الكود
- جديد: `shared/langtruth/include/sad/langtruth.h`

## تعريف "تم"
Header يُترجم بلا أخطاء + كل API موثَّق ثنائياً + لا تبعية yaml-cpp عامة.
