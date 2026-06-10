---
id: S-V5-M1-010
title: "نطاق grammar_constructs — أبنية القواعد المتقدمة"
type: implementation-story
milestone: M1-DataPopulation
scope: grammar_constructs
entity_count: 25
codegen: gen_grammar.py (جديد ❌)
status: ready
estimate: L
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-010 — نطاق grammar_constructs ⭐

## القصة
بصفتي **فريق اللغة**، أريد **توثيق أبنية القواعد المتقدمة (عقود/تزامن/ماكرو/امتداد/عمر/async/ffi)** حتى **تشرح الأدوات الميزات المتقدمة بأمثلة صحيحة**.

## معايير القبول
- **Given** Schema grammar_construct جاهز (M0-003)
- **When** نكتب `language-truth/grammar_constructs.yaml` + ننشئ `gen_grammar.py`
- **Then** كل بناء له اسم + صيغة + مثال Given/When/Then + بناء أخضر

## المهام
- [ ] العقود: `يتطلب`/`يضمن`/`حيث`/`عقد`
- [ ] التزامن: `قناة`/`أطلق`/`اختر`/`أجّل`/`مجموعة_انتظار`/`قفل`/`مستقبل`
- [ ] الماكرو: `ماكرو`/استدعاء `!` (متغيرات + متغيّرة `...`)
- [ ] الامتداد: `امتداد`، تعليقات العمر: `<'أ>`
- [ ] async: `غير_متزامن`/`انتظر`، FFI: `خارجي`
- [ ] إنشاء `gen_grammar.py` + ربط في `codegen.cmake` + T1 + بناء أخضر

## المصدر في الكود
- `shared/parser/src/specs/{contracts,async,meta,ffi}/`
- `shared/parser/src/statements/parser_advanced.cpp`

## تعريف "تم"
YAML يطابق Schema + كل بناء له ID + مثال يُحاكي التنفيذ الفعلي + بناء أخضر.
