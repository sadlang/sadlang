---
id: S-V5-M3-004
title: "T5 — تغطية قنوات التوثيق الخمس"
type: implementation-story
milestone: M3-Quality
scope: validation-t5
status: ready
estimate: L
depends_on: [S-V5-M3-003, S-V5-M1-012]
date: 2026-06-05
governed_by: [ADR-004]
---

# S-V5-M3-004 — T5 Doc Channels Coverage

## القصة
بصفتي **بوابة جودة**، أريد **إثبات أن قنوات التوثيق الخمس تقرأ من Truth فقط** حتى **لا توجد قناة تختلق محتوى خارج الحقيقة (GR-01)**.

## معايير القبول
- **Given** المُصيّرات الخمسة (lsp/man/repl/tutorial/vitepress) موجودة
- **When** نشغّل T5 + حارس المصدر
- **Then** كل مُصيّر يقرأ من Truth + لا مصدر خارجي + كل رابط تعليمي صالح

## المهام
- [ ] `test_channels_source.py`: كل renderer مصدره Truth فقط
- [ ] حارس `check_docs_source_guard.py`: لا قراءة من خارج `language-truth/`
- [ ] فحص الروابط: كل مرجع درس/تمرين ← كيان موجود
- [ ] تغطية القنوات الخمس: lsp, man, repl, tutorial, vitepress
- [ ] دمج في CI + بناء أخضر

## المصدر في الكود
- `scripts/codegen/renderers/{render_lsp,render_man,render_repl,render_tutorial,render_vitepress}.py`
- `scripts/codegen/check_docs_source_guard.py`

## تعريف "تم"
كل القنوات الخمس تقرأ من Truth + الحارس يفشل على مصدر خارجي + روابط صالحة.
