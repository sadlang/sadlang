# 🗺️ خريطة الاكتشاف الشاملة — لغة ص

> **تاريخ الإنشاء:** 2026-05-28
> **الهدف:** توثيق كل ملف في المشروع — اسم، تاريخ آخر تعديل، مسار، النظام التابع، الدوال العامة، الوظيفة
> **السبب:** قبل أي قرار حوكمة/تخطيط، يجب أن نعرف ماذا يوجد فعلاً
> **القاعدة:** قراءة فقط، لا تعديل لأي ملف خارج هذا المجلد

## 📂 المناطق العشر (Authority Regions)

| المنطقة | الملف | النطاق | الحالة |
|---|---|---|---|
| W1 | [A01_governance.md](A01_governance.md) | `_bmad-output/governance/1-policy/` + `_bmad-output/STATUS.md` | ✅ مكتمل (ملخص) |
| W2 | [A02_planning.md](A02_planning.md) | `_bmad-output/{planning-artifacts,docplan,codeRolePlan,stories,error_system,eroor_system,type_system,implementation-artifacts,test-artifacts,party-sessions,doc_plan}/` | ✅ مكتمل (تفصيلي) |
| W3 | [A03_bmad_config.md](A03_bmad_config.md) | `_bmad/` (bmb, bmm, cis, core, tea, _config) | ✅ مكتمل (ملخص) |
| W4 | [A04_documentation.md](A04_documentation.md) | `docs/` + `وثائق/` + `ملفات_التدريب/` + root .md | ✅ مكتمل (تفصيلي) |
| W5 | [A05_archived.md](A05_archived.md) | `archived/` + `_recovered/` + `_scratch/` + `_proj_pdf/` | ✅ مكتمل (ملخص) |
| W6 | [A06_github_config.md](A06_github_config.md) | `.github/` (workflows, skills, instructions, prompts) | ✅ مكتمل (تفصيلي) |
| W7 | [A07_shared_runtime.md](A07_shared_runtime.md) | `shared/` + `runtime/` | ✅ مكتمل (تفصيلي) |
| W8 | [A08_compiler_interpreter_vm.md](A08_compiler_interpreter_vm.md) | `compiler/` + `interpreter/` + `vm/` | ✅ مكتمل (ملخص) |
| W9 | [A09_libraries.md](A09_libraries.md) | `stdlib/` + `graphics/` + `network/` + `sad_ui/` + `platform/` | ✅ مكتمل (ملخص) |
| W10 | [A10_tools_tests_misc.md](A10_tools_tests_misc.md) | `tools/` + `tests/` + `examples/` + `scripts/` + `data/` + `cmake/` + `templates/` + `specs/` + `deployment/` + `distribution/` + `third_party/` + `website/` + root scripts | ✅ مكتمل (ملخص) |

## 📋 الفورمات الموحدة (لكل وكيل)

كل تقرير منطقة يجب أن يحوي:

### 1. ملخص المنطقة
- إجمالي الملفات
- إجمالي الأنواع (md, py, cpp, h, yaml, json, ...)
- أحدث تعديل / أقدم تعديل
- المساحة الإجمالية

### 2. الأنظمة المُكتشفة
قائمة بالأنظمة/المكونات/المعالم داخل المنطقة (مثل "نظام الحوكمة"، "نظام التخطيط"، "ADR-006b"، إلخ).

### 3. جدول الملفات التفصيلي
| الملف | المسار | آخر تعديل | الحجم | النظام التابع | الدوال العامة | الوظيفة | ملاحظات |
|---|---|---|---|---|---|---|---|
| ... | [path](path) | YYYY-MM-DD | KB | ... | `func1()`, `func2()` | ... | ... |

### 4. الملاحظات الاستراتيجية
- تكرارات مع مناطق أخرى
- تعارضات بين الملفات (نسختان متناقضتان)
- ملفات يدّعى أنها نشطة لكن تبدو مهجورة
- ملفات يدّعى أنها سلطة لكن لا تذكرها وثائق أخرى
- ثغرات (Story states لا تطابق الواقع)
- ملفات يجب أن تكون موجودة لكنها مفقودة

## 🚨 قاعدة لكل وكيل

> **يجب** التحقق من **آخر تاريخ تعديل** عبر `Get-Item <path> | Select-Object LastWriteTime` (PowerShell) أو ما يكافئها.
> **يجب** كتابة المسار كرابط Markdown يعمل: `[path/file.ext](path/file.ext)`.
> **ممنوع** تعديل أي ملف خارج هذا المجلد.
