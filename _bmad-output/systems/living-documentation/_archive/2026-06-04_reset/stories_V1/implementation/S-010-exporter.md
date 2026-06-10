# S-010: Exporter (json_schemas + web_bundle + lsp_cache)

> **Phase:** 4 | **Effort:** L | **Risk:** Low
> **Depends on:** S-009 | **Blocks:** S-015
> **Arch ref:** §3.5

---

## User Story

**As** الموقع، LSP، و JSON schema consumers،
**I want** export targets جاهزة من cache،
**So that** كل consumer يحصل على الـformat الأنسب له.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `Exporter::export(target, outputDir)` يدعم 3 targets في هذه القصة |
| AC-2 | `json_schemas` → ملفات `.schema.json` لكل category (للـIDEs) |
| AC-3 | `web_bundle` → JSON واحد محسَّن للموقع (`web_bundle.json` + chunks) |
| AC-4 | `lsp_cache` → binary compact format للـLSP startup السريع |
| AC-5 | كل target له subclass من `ExporterBase` |
| AC-6 | Golden tests مع expected files |
| AC-7 | export incremental: إذا الـcache لم يتغيَّر → skip |
| AC-8 | CLI: `sadinfo export --target=web_bundle --out=./build/web`

## Tasks

- [ ] T1: `ExporterBase` interface
- [ ] T2: `JsonSchemaExporter`
- [ ] T3: `WebBundleExporter` (مع chunking للأداء)
- [ ] T4: `LspCacheExporter` (binary format)
- [ ] T5: incremental detection (مقارنة merkle_root)
- [ ] T6: CLI command
- [ ] T7: Golden tests + expected fixtures
- [ ] T8: Integration tests

## File List

**جديد:**
- `shared/sadinfo_core/include/exporter.h`
- `shared/sadinfo_core/src/exporter/exporter_base.cpp`
- `shared/sadinfo_core/src/exporter/json_schema_exporter.cpp`
- `shared/sadinfo_core/src/exporter/web_bundle_exporter.cpp`
- `shared/sadinfo_core/src/exporter/lsp_cache_exporter.cpp`
- `tools/sadinfo/src/commands/export.cpp`
- `tests/sadinfo/golden/expected/web_bundle.json`
- `tests/sadinfo/golden/expected/builtin.schema.json`
- `tests/sadinfo/integration/exporter_tests.cpp`

## Definition of Done

- [ ] 3 targets تعمل
- [ ] Golden tests تمر
- [ ] الموقع يستهلك `web_bundle.json` بنجاح (smoke)
- [ ] LSP يستهلك `lsp_cache` بنجاح (smoke)

## Notes للمطوِّر

- باقي الـ5 targets (markdown_docs, pdf, vitepress_meta, validation_report, openapi) → stories لاحقة
- `lsp_cache` format: اقترح FlatBuffers أو custom binary بسيط (uint16 count + records)
- `web_bundle.json`: chunking للـcategories (chunk per category لـlazy loading)
