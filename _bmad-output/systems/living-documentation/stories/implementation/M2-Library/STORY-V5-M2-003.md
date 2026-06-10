---
id: S-V5-M2-003
title: "توسعة codegen.cmake لكل النطاقات الـ14"
type: implementation-story
milestone: M2-Library
scope: build-integration
status: ready
estimate: M
depends_on: [S-V5-M2-002]
date: 2026-06-05
governed_by: [ADR-004, ADR-003]
---

# S-V5-M2-003 — توسعة codegen.cmake

## القصة
بصفتي **نظام البناء**، أريد **هدف codegen لكل النطاقات الـ14 + هدف شامل** حتى **يُولَّد كل C++ تلقائياً من Truth في كل بناء**.

## معايير القبول
- **Given** كل `gen_*.py` للنطاقات موجود (M1)
- **When** نوسّع `cmake/codegen.cmake`
- **Then** هدف لكل نطاق + هدف `sad_langtruth_codegen_all` يجمعها + بناء أخضر

## المهام
- [ ] إضافة custom command/target لكل نطاق (keywords...expr_constructs)
- [ ] هدف تجميعي `sad_langtruth_codegen_all` يعتمد على الكل
- [ ] ربط الإخراج المولَّد بمكتبة `sad_core`/`libsadlangtruth`
- [ ] تشغيل تلقائي عند تغيّر YAML المصدر

## المصدر في الكود
- `cmake/codegen.cmake`

## تعريف "تم"
كل هدف يُولّد C++ صحيح + الهدف الشامل يبني الكل + بناء أخضر نظيف.
