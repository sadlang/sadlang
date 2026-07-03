# أرشيف الاختبارات (NOT-A-SoT — للرجوع والتعدين فقط)

> **أُنشئ:** 2026-06-12 ضمن TEST-003/TEST-005 (نظام testing-system).
> قاعدة GR-04: **أرشفة لا حذف**. لا يُشغَّل أي شيء هنا من CI أو الراننر،
> ولا يُكتب اختبار جديد هنا أبداً — الأحياء في `unit/` و`behavior/` و`system/`.

## خريطة: أين ذهب/سيذهب كل مجلد (AC-02)

| المجلد المؤرشف | ملفات | التصنيف (جرد TEST-001) | الوجهة عند الدمج |
|---|---|---|---|
| `dual_execution_legacy/` | 380 .ص | التنظيم القديم قبل features | فُرز الحي منه إلى `behavior/sections` (TEST-003)؛ الفريد يُعدَّن لاحقاً |
| `compiler/` | 102 .ص | سلوكي للمترجم | `behavior/sections` حسب الميزة |
| `compiler_features/` | 50 .ص | ميزات المترجم | `behavior/sections` حسب الميزة |
| `compiler_rules/` | 29 .ص | مصفوفة قواعد | `behavior/rules_matrix` |
| `sad_examples/` | 255 .ص | أمثلة | `behavior/sections/12_تكامل` أو يبقى أرشيفاً |
| `root_cleanup/` | 145 .ص | مبعثر | فرز فردي |
| `ownership/` | 20 .ص | ملكية | `behavior/sections/08` + `unit/ownership` |
| `modules/` | 11 .ص + cpp معطّلة | استيراد | `behavior/sections/10_الاستيراد` (فجوة حرجة — أولوية) |
| `quick/` | 10 .ص | فحص سريع | `P0_smoke`/أقسام |
| `stdlib/` | 9 .ص | مكتبة | `behavior/sections/09` (الـcpp نُقلت إلى `unit/stdlib` في TEST-004) |
| `grammar/` | 8 .ص | نحو | `behavior/sections/01` |
| `type_system/` | 8 .ص | أنواع | `behavior/sections/02` + `unit/types` |
| `bare_metal/` | 4 .ص | منخفض المستوى | `behavior/sections/08` |
| `sad_tests/` | 3 .ص | عينات مكررة | يبقى أرشيفاً |
| `generics/`, `safety/`, `freestanding/` | 1 لكلٍّ | شظايا | فرز فردي |
| `async/`, `backends/`, `compat/`, `oop/`, `spec_rules/`, `stage1/` | شبه فارغة | بقايا بنيوية | تبقى أرشيفاً |
| `test_unicode_json.ص`, `test_unicode_stringify.ص` | 2 | سكربتات يدوية كانت بجذر tests | `behavior/sections/09` إن ثبتت صلاحيتها |

## أين الأحياء الآن؟ (بعد TEST-003/004/005)

```
tests/
├── runner.py + config.yaml + report_html.py   ← المشغّل الموحّد
├── behavior/    ← طبقة السلوك: P0_smoke + sections(12) + rules_matrix + _regression
├── unit/        ← طبقة الوحدة C++: 12+ مكوّناً + comprehensive (ctest -L Unit)
├── system/      ← طبقة النظام: hub, lsp, pkg, docs, benchmark, network(خلف علم), fuzzing
├── framework/   ← إطار اختبار C++ المشترك
└── _archive/    ← هذا المجلد
```

> **التعدين:** قبل دمج أي ملف من هنا في الأقسام: تأكد من صيغة `@expected`،
> شغّله عبر `python tests/runner.py --file <اسم>`, وحدّث `COVERAGE.md` للقسم الهدف.
