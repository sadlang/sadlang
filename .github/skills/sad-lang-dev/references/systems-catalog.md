# فهرس كل أنظمة لغة ص (خريطة شاملة)

> لا يمكن للمطوّر أن يقدّر أثر مهمته دون معرفة **كل** الأنظمة الموجودة. هذا الفهرس هو الخريطة
> الكاملة: لكل نظام موقعه، دوره، والمرجع الذي يشرحه. استخدمه في «تحليل الأثر»
> ([./delivery-checklist.md](./delivery-checklist.md) §أ).

## 1. أنظمة النواة المشتركة (`shared/`)

| النظام | المجلد | الدور | مرجع |
|--------|--------|------|------|
| المعجمي (Lexer) | `shared/lexer/` | تحويل المصدر إلى Tokens + جدول الكلمات المُولَّد | [architecture](./architecture.md) |
| النحوي (Parser) | `shared/parser/` | بناء AST؛ مقسّم `core/declarations/statements/specs/ui` | [architecture](./architecture.md), [adding-systems](./adding-systems.md) |
| الشجرة (AST) | `shared/ast/` | عقد الشجرة (كلها ترث `ASTNode`) | [adding-systems](./adding-systems.md) |
| نظام الأنواع | `shared/types/` | `Value`/`ValueType` (تشغيل) + `SadTypeKind` (ثابت) | [types-and-sir](./types-and-sir.md) |
| التحليل الدلالي | `shared/semantic/` | فحص الأنواع/النطاقات/الدلالة بعد التحليل النحوي | — |
| نظام الأخطاء | `shared/errors/` | الكتالوج، التشخيص، التلميحات، منع التتالي | [error-system](./error-system.md) |
| سجل الدوال المُولَّد | `shared/builtins/` | `generated/builtin_registry_generated.h` من YAML | [builtins-system](./builtins-system.md) |
| الوحدات | `shared/modules/` | حلّ الاستيراد/التصدير (`ModuleResolver`) | [builtins-system](./builtins-system.md) |
| الملكية (Ownership) | `shared/ownership/` | فحص الملكية/الاستعارة (compile-time) | [types-and-sir](./types-and-sir.md) |
| الملكية وقت التشغيل | `shared/ownership_runtime/` | دعم الملكية في وقت التشغيل | — |
| جامع القمامة (GC) | `shared/memory_gc/` | إدارة دورة حياة `ObjectInstance` (الكائنات مُدارة بـGC) | [types-and-sir](./types-and-sir.md) |
| سياسة الذاكرة | `shared/memory_policy/` | استراتيجيات/سياسات تخصيص الذاكرة | — |
| الأمان (Sandbox) | `shared/security/` | عزل/سياسات أمان التنفيذ | — |
| إعادة التحميل الساخن | `shared/hot_reload/` | تحديث الكود أثناء التشغيل | — |
| المُحلّل (Profiler) | `shared/profiler/` | قياس الأداء (استخدمه قبل التحسين — CW-30) | — |
| نواة sadinfo | `shared/sadinfo_core/` | المكتبة الخلفية لأداة `sadinfo` | [docs-yaml-system](./docs-yaml-system.md) |
| أدوات مساعدة | `shared/utils/` | مرافق مشتركة | — |

## 2. محرّكات التنفيذ

| النظام | المجلد | الدور | مرجع |
|--------|--------|------|------|
| المفسر | `interpreter/` | `core/`, `managers/`, `visitors/`, `builtins/`, `debug/`, `ui/` | [architecture](./architecture.md) |
| المترجم (sadc) | `compiler/` | `frontend/` (SIR), `sir_optimizer/`, `backend/` (LLVM), `types/`, `meta/` | [types-and-sir](./types-and-sir.md) |
| الآلة الافتراضية | `vm/` | بايت كود مرتبط بالمفسر | [architecture](./architecture.md) |
| وقت التشغيل | `runtime/` | ABI/FFI مستقل (`freestanding/`) + ربط VM | [architecture](./architecture.md) |

## 3. مصدر الحقيقة والتوليد

| النظام | المجلد | الدور | مرجع |
|--------|--------|------|------|
| مصدر الحقيقة | `language-truth/` | YAML SoT لكل بيانات اللغة | [docs-yaml-system](./docs-yaml-system.md) |
| مولّدات الكود | `scripts/codegen/` | `gen_*.py` → C++ مُولَّد | [docs-yaml-system](./docs-yaml-system.md) |
| بقايا V4 | `data/language/keywords.yaml` | مصدر قديم (الكلمات فقط؛ أخطاء V4 حُذفت في EM-V5-3) — يُتقاعَد | [error-system](./error-system.md) |
| كتالوج sadinfo (مُولَّد) | `data/errors/` | إسقاط مُولَّد من `language-truth/errors/` (EM-V5-2) — لا يُحرَّر يدوياً | [error-yaml-structures](./error-yaml-structures.md) |

## 4. الأدوات (`tools/`) — كلها تستهلك `language-truth`

| الأداة | المجلد | الدور |
|--------|--------|------|
| LSP | `tools/lsp/` | خادم اللغة (تلوين/إكمال/تشخيص للمحرّرات) |
| المنسّق | `tools/formatter/` | تنسيق كود `.ص` |
| مدير الحزم | `tools/pkg/` | الحزم والاعتماديات |
| sadinfo | `tools/sadinfo/` | استعلام بيانات اللغة |
| واجهة المترجم | `tools/compiler/` | `compiler_driver_*` (واجهة sadc) |
| REPL | `tools/repl/` | الصدفة التفاعلية |
| الموزّع (Hub) | `tools/hub/` | `sad.exe` موزّع الأوامر (المفسر الفعلي `sad-run`) |
| أخرى | `tools/{analyze,check,profiler,wasm,apk_builder,installers,security-scanner}/` | تحليل/فحص/نشر |

> ⚠️ **أثر التموّج:** أي تعديل على `language-truth/` (كلمة/دالة/نوع/عامل) يَمَسّ **LSP + المنسّق
> + sadinfo + مدير الحزم + الموقع** لأنها تستهلك نفس المصدر. أدرِجها في تحليل الأثر.

## 5. المكتبة القياسية والمجالات

| النظام | المجلد | الدور |
|--------|--------|------|
| المكتبة القياسية | `stdlib/` | `core/io/math/string/network/crypto/json/xml/database/...` + مجالات عربية (`نص/ويب/إضافات`) |
| الرسوميات/الواجهات | `graphics/`, `sad_ui/` | SDL2/OpenGL + واجهات المستخدم |
| الشبكة | `network/` | طبقة الشبكة |
| المنصّة | `platform/` | كشف/تجريد المنصّة |
| الأنظمة المنخفضة | `stdlib/low_level/`, `stdlib/freestanding/`, `stdlib/embedded/` | kernel/OS/bare-metal (راجع مهارة `sad-os-coding`) |

## 6. الاختبار والبناء

| النظام | المجلد | الدور | مرجع |
|--------|--------|------|------|
| التنفيذ المزدوج | `tests/behavior/` | `runner.py` يقارن المفسر بالمترجم | [build-test](./build-test.md) |
| اختبارات C++ | `tests/unit/` (الشاملة في `unit/comprehensive/`) | وحدات + الحزمة الشاملة | [build-test](./build-test.md) |
| اختبارات sadinfo | `tests/sadinfo/` | تحقّق بيانات اللغة | [docs-yaml-system](./docs-yaml-system.md) |
| البناء | `cmake/` | `codegen.cmake`, `executables.cmake`, `llvm.cmake` | [build-test](./build-test.md) |

## 7. الحوكمة

| النظام | المجلد | مرجع |
|--------|--------|------|
| الحوكمة BMAD | `_bmad-output/governance/` + `_bmad-output/systems/` | [governance](./governance.md) |

---

## كيف تستخدم هذا الفهرس

1. **عند تحليل الأثر:** امسح الجدول واسأل «هل مهمتي تَمَسّ هذا النظام؟».
2. **العمود «مرجع» فارغ** يعني أن النظام غير مغطّى بمرجع تفصيلي بعد — اقرأ كوده مباشرةً
   (ابدأ بـ `README.md`/`include/` في مجلده) واتبع نمطه (CW-04, BF-25).
3. **عند الشك في موقع نظام:** هذا الفهرس هو نقطة البدء قبل `grep` عشوائي.
