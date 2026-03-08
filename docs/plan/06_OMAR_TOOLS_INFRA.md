# عمر — الأدوات والبنية التحتية (Tools & Infrastructure)

> الدور: مهندس الأدوات والبنية التحتية
> المسؤولية: LSP, Formatter, Package Manager, REPL, CI/CD, Testing, VS Code Extension, Debugger
> الملف المرجعي: `docs/plan/01_STRATEGIC_PLAN.md`

---

## الملكية الحصرية

```
tools/
├── lsp/                         ← خادم LSP (اكتمالات، تشخيصات، hover، ...)
│   ├── src/
│   │   ├── providers/           ← 24 provider (completion, diagnostics, hover...)
│   │   ├── server/              ← LSP server core
│   │   └── protocol/           ← LSP 3.17 protocol
│   └── tests/
├── formatter/                   ← sad-fmt
│   ├── src/
│   └── config/                  ← sad-fmt.toml
├── pkg/                         ← sad-pkg (مدير الحزم)
│   ├── src/
│   │   ├── registry/           ← Package registry client
│   │   ├── resolver/           ← Dependency resolver
│   │   └── installer/          ← Package installer
│   └── registry-server/        ← Registry backend (API)
├── repl/                        ← sad-repl
│   ├── src/
│   └── history/
├── vscode-extension/            ← VS Code Extension
│   ├── src/
│   │   ├── extension.ts        ← Extension entry
│   │   ├── language/           ← Language client
│   │   └── debug/              ← Debug adapter
│   ├── syntaxes/               ← TextMate grammar
│   └── package.json
├── debugger/                    ← sad-debug (DAP - Debug Adapter Protocol)
│   ├── src/
│   │   ├── dap_server.cpp      ← DAP server
│   │   ├── breakpoints.cpp     ← Breakpoint management
│   │   ├── stepping.cpp        ← Step over/into/out
│   │   └── variables.cpp       ← Variable inspection
│   └── tests/
├── linter/                      ← sad-lint
│   ├── src/
│   │   ├── rules/              ← Lint rules
│   │   └── config/             ← .sadlint.toml
│   └── tests/
├── profiler/                    ← sad-profile
│   ├── src/
│   └── output/                 ← Flame graphs
└── docgen/                      ← sad-doc (API docs generator)
    ├── src/
    └── templates/

tests/                           ← إطار الاختبار
├── framework/                   ← Test framework بلغة ص
├── comprehensive/               ← 900+ اختبار شامل
├── regression/                  ← اختبارات انحدار
├── stdlib_tests/                ← اختبارات المكتبة القياسية
├── integration/                 ← اختبارات تكاملية
└── performance/                 ← اختبارات أداء

.github/workflows/               ← CI/CD
├── ci.yml                       ← بناء + اختبار
├── release.yml                  ← إصدارات تلقائية
├── nightly.yml                  ← بناء ليلي
└── security.yml                 ← فحص أمني
```

### القاعدة الذهبية
> عمر لا يعدّل: `compiler_new/`، `interpreter_new/`، `graphics/`، `platform/`
> يقرأ `shared/` لبناء أدوات تحليل (AST, tokens) لكن لا يعدّله

---

## 2026 — التفصيل الشهري

---

### الربع الأول (مارس — مايو 2026): "تأسيس الأدوات + CI"

#### مارس 2026

| الأسبوع | المهمة | الملفات | المُخرج | معيار القبول |
|---------|--------|---------|---------|-------------|
| 1 | إصلاح CI: nightly.yml + release.yml | `.github/workflows/*.yml` | كل workflows تمر بدون أخطاء | ✅ green on 3 platforms |
| 2 | CI: إضافة matrix build (Win/Linux/macOS) | `.github/workflows/ci.yml` | Matrix: 3 OS × 2 config | 6 builds pass |
| 3 | أول GitHub Release تلقائي | `.github/workflows/release.yml` | `git tag v0.5.0` → artifacts | .zip artifacts منشورة |
| 4 | LSP: حذف legacy files + cleanup | `tools/lsp/` | حذف ملفات .legacy، تنظيف | 0 legacy files |

**المُخرج الشهري**: CI/CD يعمل على 3 منصات + أول GitHub Release

#### أبريل 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | VS Code Extension: نشر في Marketplace | `tools/vscode-extension/` | Extension منشورة + متاحة للتثبيت |
| 2 | VS Code: تحسين syntax highlighting | `tools/vscode-extension/syntaxes/` | تلوين 100% من الكلمات المفتاحية |
| 3 | VS Code: snippets + code actions | `tools/vscode-extension/src/` | 20 snippet + 5 code actions |
| 4 | LSP: تحسين hover documentation | `tools/lsp/src/providers/hover_provider.cpp` | Hover يعرض نوع + وصف + مثال |

**المُخرج الشهري**: VS Code Extension منشورة مع syntax + snippets + hover

#### مايو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Formatter: دعم كامل للكتل | `tools/formatter/src/` | if/while/for/class/function formatting |
| 2 | Formatter: دعم OOP + pattern matching | `tools/formatter/src/` | أصناف + طابق + سمات |
| 3 | Formatter: تخصيص (sad-fmt.toml) | `tools/formatter/config/` | indent_size, line_length, brace_style |
| 4 | Formatter: اختبارات + integration | `tools/formatter/tests/` | 50 حالة اختبار، 90% coverage |

**المُخرج الشهري**: Formatter يغطي 90% من الحالات مع ملف تخصيص

**معايير القبول Q1:**
- [ ] CI/CD: 3 platforms × 2 configs = 6 builds pass
- [ ] أول GitHub Release منشورة
- [ ] VS Code Extension في Marketplace مع 10+ installs
- [ ] Formatter يغطي 90% من الحالات
- [ ] 0 legacy files في LSP

---

### الربع الثاني (يونيو — أغسطس 2026): "أدوات ناضجة"

#### يونيو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | LSP: Completion Provider محسّن | `tools/lsp/src/providers/completion_provider.cpp` | إكمال ذكي: متغيرات + دوال + أنواع |
| 2 | LSP: Diagnostics في الوقت الفعلي | `tools/lsp/src/providers/diagnostics_provider.cpp` | أخطاء فورية أثناء الكتابة |
| 3 | LSP: Go to Definition + References | `tools/lsp/src/providers/definition_provider.cpp` | F12 + Shift+F12 يعملان |
| 4 | LSP: Signature Help | `tools/lsp/src/providers/signature_provider.cpp` | عرض معاملات الدالة أثناء الكتابة |

#### يوليو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Package Registry: server design | `tools/pkg/registry-server/` | REST API تصميم + endpoints |
| 2 | Package Registry: server implementation | `tools/pkg/registry-server/src/` | Node.js/Python server + SQLite DB |
| 3 | sad-pkg: publish + install | `tools/pkg/src/` | `sad-pkg publish` + `sad-pkg install` |
| 4 | 10 حزم رسمية أولية | `tools/pkg/official-packages/` | math, json, http, crypto, utils... |

#### أغسطس 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Linter: بنية أساسية | `tools/linter/src/` | Engine + rule loading |
| 2 | Linter: 10 قواعد أولية | `tools/linter/src/rules/` | unused vars, naming, complexity... |
| 3 | Linter: integration مع LSP | `tools/linter/src/lsp_integration.cpp` | أخطاء lint في VS Code |
| 4 | REPL: تحسين (history + multiline) | `tools/repl/src/` | Arrow keys + multiline + autocomplete |

**معايير القبول Q2:**
- [ ] LSP: Completion + Diagnostics + GoTo + Signature Help
- [ ] Package Registry v1 يعمل
- [ ] `sad-pkg install/publish` يعملان
- [ ] 10 حزم رسمية
- [ ] Linter مع 10 قواعد
- [ ] REPL مع history + multiline

---

### الربع الثالث (سبتمبر — نوفمبر 2026): "اختبارات + Debugger"

#### سبتمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Test Framework بلغة ص | `tests/framework/` | `اختبر "وصف" ... تأكد(...) نهاية` |
| 2 | Test Framework: assertions + reporting | `tests/framework/` | `تأكد_يساوي`, `تأكد_خطأ`, HTML report |
| 3 | Test Framework: test discovery | `tests/framework/` | اكتشاف تلقائي لملفات .ص |
| 4 | Coverage tool أساسي | `tests/framework/coverage/` | تقرير تغطية أسطر |

#### أكتوبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | LSP: Rename Symbol | `tools/lsp/src/providers/rename_provider.cpp` | F2 rename عبر كل الملفات |
| 2 | LSP: Extract Function | `tools/lsp/src/providers/refactoring_provider.cpp` | استخراج كود إلى دالة |
| 3 | LSP: Code Lens + Inlay Hints | `tools/lsp/src/providers/` | عدد المراجع + أنواع مستنتجة |
| 4 | Profiler أساسي | `tools/profiler/src/` | `sad --profile script.ص` → تقرير |

#### نوفمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Debugger (DAP): architecture | `tools/debugger/src/dap_server.cpp` | DAP server يستقبل اتصالات |
| 2 | Debugger: breakpoints + stepping | `tools/debugger/src/breakpoints.cpp`, `stepping.cpp` | توقف + خطوة |
| 3 | Debugger: variable inspection | `tools/debugger/src/variables.cpp` | عرض قيم المتغيرات |
| 4 | VS Code: launch.json + debug integration | `tools/vscode-extension/src/debug/` | Debug من VS Code مباشرة |

**معايير القبول Q3:**
- [ ] Test Framework بلغة ص مع assertions + reporting
- [ ] Coverage tool يعمل
- [ ] LSP: Rename + Extract Function + Code Lens
- [ ] Profiler ينتج تقرير أداء
- [ ] Debugger (DAP) مع breakpoints + stepping + variables
- [ ] VS Code debugging يعمل

---

### الربع الرابع (ديسمبر 2026): "v1.0 Tools"

#### ديسمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Windows installer: كل الأدوات | `distribution/` | sad + sadc + sad-fmt + sad-pkg + sad-lint |
| 2 | Security audit: OWASP review | `.github/workflows/security.yml` | فحص أمني تلقائي |
| 3 | VS Code Extension v2.0 | `tools/vscode-extension/` | كل الميزات + dark theme + RTL |
| 4 | v1.0 release preparation | `tools/`, `tests/` | Smoke tests مكتملة |

**معايير القبول Q4:**
- [ ] كل الأدوات في installer واحد
- [ ] Security audit pass
- [ ] VS Code Extension v2.0 منشورة
- [ ] 1500+ اختبار (100% pass)

---

## 2027 — التفصيل الشهري

---

### الربع الأول (يناير — مارس 2027): "Debugger + Profiler"

#### يناير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | DAP v2: conditional breakpoints | `tools/debugger/src/` | شروط للتوقف + watchpoints |
| 3-4 | DAP v2: call stack + threads | `tools/debugger/src/` | عرض الاستدعاءات + خيوط |

#### فبراير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Profiler: flame graph | `tools/profiler/src/` | SVG flame graph + CLI output |
| 3-4 | Profiler: memory profiling | `tools/profiler/src/memory/` | تتبع استهلاك الذاكرة |

#### مارس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | LSP v2: workspace symbols | `tools/lsp/src/` | بحث في كل workspace |
| 3-4 | LSP v2: semantic tokens | `tools/lsp/src/` | تلوين دلالي متقدم |

**معايير القبول Q1 2027:**
- [ ] DAP: conditional breakpoints + call stack + threads
- [ ] Profiler: flame graph + memory profiling
- [ ] LSP v2: workspace symbols + semantic tokens

---

### الربع الثاني (أبريل — يونيو 2027): "تكامل IDEs + Linter متقدم"

#### أبريل 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Linter: 20 قاعدة إضافية | `tools/linter/src/rules/` | complexity, security, style... |
| 3-4 | Linter: auto-fix | `tools/linter/src/fixer/` | إصلاح تلقائي للمشاكل |

#### مايو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | IntelliJ Plugin أساسي | `tools/intellij-plugin/` | Syntax + LSP client |
| 3-4 | Sublime Text package | `tools/sublime-text/` | Syntax + build system |

#### يونيو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Neovim/Vim plugin | `tools/vim-sad/` | TreeSitter grammar + LSP |
| 3-4 | IDE matrix testing | `tools/` | كل الـ IDEs تعمل مع LSP |

**معايير القبول Q2 2027:**
- [ ] Linter: 30 قاعدة + auto-fix
- [ ] IntelliJ + Sublime + Vim plugins
- [ ] كل IDEs تتصل بـ LSP

---

### الربع الثالث (يوليو — سبتمبر 2027): "Ecosystem Maturity"

#### يوليو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | DAP v3: hot reload | `tools/debugger/src/` | تعديل كود أثناء التشغيل |
| 3-4 | DAP v3: remote debugging | `tools/debugger/src/` | Debug عن بعد (Android/iOS) |

#### أغسطس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Registry: 200+ حزمة | `tools/pkg/` | فهرس بحث + تصنيفات + تقييمات |
| 3-4 | Registry: security scanning | `tools/pkg/registry-server/` | فحص أمني تلقائي للحزم |

#### سبتمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Security audit: كل الأدوات | `tools/` | OWASP audit + dependency audit |
| 3-4 | Automated CI/CD: release pipeline كامل | `.github/workflows/` | tag → build → test → publish |

**معايير القبول Q3 2027:**
- [ ] Hot reload + remote debugging
- [ ] 200+ حزمة مع security scanning
- [ ] Full security audit pass
- [ ] Automated release pipeline

---

### الربع الرابع (أكتوبر — ديسمبر 2027): "v2.0 Tools"

#### أكتوبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | Automated CI: كل منصة + كل target | `.github/workflows/` | Matrix: 3 OS × 5 targets |
| 3-4 | docgen: API docs من كود ص | `tools/docgen/src/` | `sad-doc` → HTML/Markdown |

#### نوفمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | VS Code Extension v3.0 | `tools/vscode-extension/` | Debugger + Profiler + RTL + themes |
| 3-4 | Test: 5000+ اختبار | `tests/` | كل الميزات × كل المنصات |

#### ديسمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | v2.0 smoke tests | `tests/` | كل الأدوات تعمل |
| 3-4 | 🎉 v2.0 Tools release | `tools/` | إطلاق رسمي |

**معايير القبول Q4 2027:**
- [ ] VS Code Extension v3.0
- [ ] 5000+ اختبار (100% pass)
- [ ] docgen يولّد API docs
- [ ] Full CI/CD matrix
- [ ] v2.0 Tools stable

---

## الأدوات والتقنيات

| الأداة | الغرض | الإصدار |
|--------|--------|---------|
| LSP 3.17 | Language Server Protocol | 3.17 |
| DAP | Debug Adapter Protocol | 1.x |
| Node.js | VS Code Extension + Registry | 18+ |
| TypeScript | VS Code Extension | 5.x |
| SQLite | Registry database | 3.x |
| GitHub Actions | CI/CD | latest |
| OWASP ZAP | Security scanning | latest |
| TreeSitter | Vim/Neovim grammar | latest |

---

## ملخص المُخرجات

```
2026:
  ✅ CI/CD: 3 platforms × 2 configs — green
  ✅ VS Code Extension v2.0 في Marketplace
  ✅ Formatter: 90% coverage + configurable
  ✅ LSP: Completion + Diagnostics + GoTo + Rename + Refactoring
  ✅ Package Registry v1 مع 50+ حزمة
  ✅ Linter: 10 قواعد
  ✅ REPL: history + multiline + autocomplete
  ✅ Debugger (DAP): breakpoints + stepping + variables
  ✅ Test Framework بلغة ص
  ✅ Security audit
  ✅ 1500+ اختبار

2027:
  ✅ DAP v3: hot reload + remote debug
  ✅ Profiler: flame graph + memory
  ✅ Linter: 30 قواعد + auto-fix
  ✅ IntelliJ + Sublime + Vim plugins
  ✅ Registry: 200+ حزمة + security scanning
  ✅ docgen → API docs
  ✅ VS Code Extension v3.0
  ✅ 5000+ اختبار
  ✅ Full CI/CD pipeline
  ✅ v2.0 Tools stable
```
