# نظام Living Documentation — لغة ص

> **رسالة هذا النظام (سطر واحد):**  
> الأَوْثَق الرَسمي بأن **ما تَدعَمه لُغة ص هو الحقيقة المُطلَقة** عبر ملفات YAML قابلة للقِراءة من أي أداة، مع ضَمان أنها **تُطابق فِعلياً** ما يُنفِّذه المُفسِّر والمُترجم.

---

## الحالة الحالية (2026-06-05)

| العنصر | الإصدار | الحالة | المسار |
|---|---|---|---|
| **STRATEGY** | V5 | ✅ APPROVED + Codegen-aligned | [STRATEGY.md](STRATEGY.md) |
| **ARCHITECTURE** | V5 | ✅ APPROVED + Codegen-aligned | [ARCHITECTURE.md](ARCHITECTURE.md) |
| **ADR Approval** | V4-001 | ✅ Accepted | [decisions/ADR-DOCS-V4-001-APPROVAL.md](decisions/ADR-DOCS-V4-001-APPROVAL.md) |
| **ADR Unified Structure** | V4-002 | ⚠️ Partially Superseded | [decisions/ADR-DOCS-V4-002-UNIFIED-STRUCTURE.md](decisions/ADR-DOCS-V4-002-UNIFIED-STRUCTURE.md) |
| **ADR Codegen Integration** | V4-003 | ✅ Accepted (2026-06-05) | [decisions/ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md](decisions/ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md) |
| **ADR No CLI** | V4-004 | ✅ Accepted (2026-06-05) | [decisions/ADR-DOCS-V4-004-NO-CLI.md](decisions/ADR-DOCS-V4-004-NO-CLI.md) |
| **IMPLEMENTATION_PLAN** | V5 | ✅ APPROVED (18 ستوري M0→M3) | [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) |
| **5 ADRs إضافية (ND-V4)** | — | ⏳ مُخطَّطة (S-V4-M0-002) | `decisions/ADR-DOCS-V4-004..008` |
| **language-truth/** | — | ⏳ مُخطَّط (S-V4-M0-003) — روابط من `data/language/` أولاً | جذر مـشروع جديد |
| **بُنية قائمة (للتكامل)** | — | ✅ عاملة — سُتُوسَّع في M1 | `scripts/codegen/*.py` + `cmake/codegen.cmake` + `shared/lexer/generated/*` |

---

## ماذا نَفعل (و لا نَفعل)

### ✅ نَفعل (Scope = داخل)

- نَكتب YAML لكل كيان في اللُغة (~74 كلمة مفتاحية + تَوجيهات + دوال مُضمَّنة + …)
- **نَتَكامَل مَع `scripts/codegen/*.py` القائمة** لإنتاج C++ Generated:: وقت البَناء (صِفر I/O وقت التَشغيل)
- نَبني `libsadlangtruth` (C++ Wrapper نَظيف) للأَدوات C++ — **لا CLI** ([ADR-004](decisions/ADR-DOCS-V4-004-NO-CLI.md))
- نَضمن أن YAML **يُطابق Lexer + Parser فعلياً** عبر 4 اختبارات إلزامية
- نَكتب ADRs لكل قرار في النطاق

### ❌ لا نَفعل (Scope = خارج)

- لا نَكتب وثائق LSP أو Formatter أو Website (مسؤوليتهم!)
- لا نُحدَّد كيف تُخزَّن أي أداة وثائقها
- لا نُولَّد Tutorials أو Snippets أو محتوى تَعليمي
- لا نَتدخَّل في جودة ترجمة وثائق الأدوات
- **لا نُنشئ نِظام codegen مُوازِ — نَتَكامَل مَع القائم** (CW-19 DRY)

التَفاصيل الكاملة: [STRATEGY.md §1](STRATEGY.md)

---

## بنية المُجلَّد

```
living-documentation/
├── README.md                              ← أنت هنا
├── STRATEGY.md                            ← V5 APPROVED — السياسة الأم
├── ARCHITECTURE.md                        ← V5 APPROVED — تفصيل تقني
├── IMPLEMENTATION_PLAN.md                 ← V5 APPROVED — خطة M0–M3 (18 ستوري)
├── decisions/                             ← ADRs الفَعَّالة
│   ├── ADR-DOCS-V4-001-APPROVAL.md        ← الاعتماد الرسمي
│   └── ADR-DOCS-V4-002-UNIFIED-STRUCTURE  ← توحيد البنية والـ Schema
├── examples/                              ← أمثلة YAML تَوضيحية
└── _archive/                              ← تاريخ النظام (V1, V2, V3)
    ├── 2026-06-04_reset/                  ← V1, V2 (مَهجورة)
    └── 2026-06-04_v3_to_v4_pivot/         ← V3 + V4-DRAFT-1
```

> **مُلاحَظة GR-DOCS-CANONICAL:** الوثائق القانونية هي 3 ملفات في الجذر (`STRATEGY.md`, `ARCHITECTURE.md`, `IMPLEMENTATION_PLAN.md`). كل ما في `_archive/` للرجوع التاريخي فقط (`NOT-A-SoT`).

---

## كيف أَستَهلِك Truth كَفَريق أداة؟

أنت فَريق LSP أو Website أو Formatter أو Compiler؟ ها هي بَوابتك:

### الطريقة 1: واجهة C++ (مُوصى بها للأدوات المَكتوبة بـ C++)

```cpp
#include <sad/langtruth.h>

using namespace sad::langtruth;

// (V5) انتفاع من Generated::* وقت البَناء — 0ms وقت التَشغيل
auto& registry = Registry::instance();

// مثال: احصل على كلمة مفتاحية بالعربية
if (const auto* kw = registry.find_by_arabic("دالة")) {
    std::cout << kw->arabic << " = " << kw->english << "\n";
    // "دالة" = "function"
}

// مثال: استعرض كل الكلمات المحجوزة
for (const auto* kw : registry.by_subcategory("reserved")) {
    // افعل ما تَشاء بهذه البيانات
}
```

> **ملحوظة V5:** المكتبة تَعتمد على `Sad::Lexer::Generated::allEntries()` المُولَّد وقت البَناء — لا تُحمَّل YAML وقت التَشغيل أبداً. تَفاصيل في [ARCHITECTURE.md §2](ARCHITECTURE.md).

### الطريقة 2: YAML مُباشرة (للأَدوات بِلُغات أخرى)

> **لا CLI في V5** ([ADR-004](decisions/ADR-DOCS-V4-004-NO-CLI.md)) — الأَدوات غَير C++ تَقرأ YAML مُباشرةً.

#### Python (الأَسهل للسكربتات)

```python
import yaml
data = yaml.safe_load(open("language-truth/keywords.yaml"))
for category in data["categories"]:
    for entry in category["entries"]:
        print(entry["arabic"], "=", entry["english"])
```

#### Node.js (LSP، Website)

```javascript
const yaml = require('js-yaml');
const fs = require('fs');
const data = yaml.load(fs.readFileSync('language-truth/keywords.yaml', 'utf8'));
```

#### Bash (استِكشاف سَريع)

```bash
grep -A 15 "KW-FUNC-001" language-truth/keywords.yaml
```

#### CI Validation

```bash
python scripts/codegen/gen_keywords.py --validate-only
```

### الطريقة 3: مِثال هَيكَل YAML

```yaml
# language-truth/keywords.yaml (مُختَصَر)
categories:
  - name: reserved
    entries:
      - id: KW-FUNC-001
        schema_version: "1.0.0"
        arabic: دالة
        english: function
        since: "0.1.0"
        status: stable
        token_type: KEYWORD_FUNCTION
        description_short_ar: تَعريف دالة قابلة لإعادة الاستخدام
        description_short_en: Define a reusable function
        ends_with: نهاية
```

البِنية ثابتة عبر JSON Schema في `language-truth/_schemas/`.

> **لا نُلزِمك** بأي طريقة عَرض، تَخزين، أو معالجة. هذا قَرارك ككَريق أداة.

---

## كيف أَطلب تَغيير في Truth؟

1. **أَنشئ PR** يُعدِّل أو يُضيف YAML في `language-truth/`
2. **CI يُشغِّل 4 اختبارات** تلقائياً:
   - ✅ Schema Validation
   - ✅ Language Match (YAML = Lexer + Parser فعلياً)
   - ✅ Unique IDs
   - ✅ Required Fields
3. **مُراجَعة بَشَرية** من Language Team
4. **دَمج** إذا نَجح كل شيء

### متى يُرفَض الطلب؟

| الطلب | الحُكم |
|---|---|
| إضافة كلمة جديدة بدون تَنفيذها في Lexer أولاً | ❌ مَرفوض |
| تَغيير `arabic` لكلمة موجودة (Breaking) | ❌ مَرفوض بدون ADR |
| إضافة حقل غير مَوجود في Schema | ❌ مَرفوض |
| طَلب "اجعل Hover في LSP يَعرض كذا" | ❌ خارج نطاقنا — اطلبه من فَريق LSP |
| طَلب "أَضِف Snippet للموقع" | ❌ خارج نطاقنا — اطلبه من فَريق Website |

---

## القَواعد غير القابلة للتَفاوض (ND-V4)

تُلخَّص هنا، التَفاصيل في [STRATEGY.md §7](STRATEGY.md):

| القاعدة | الوصف |
|---|---|
| **ND-V4-1** | Truth يَحوي فقط دَعم اللُغة — لا أدوات |
| **ND-V4-2** | كل كيان لديه ID فريد عالمياً |
| **ND-V4-3** | كل YAML يُطابق JSON Schema |
| **ND-V4-4** | لا حقائق لُغوية مَكتوبة Hardcoded في الكود |
| **ND-V4-5** | فَريق اللُغة لا يَكتب/يُراجع وثائق الأدوات |
| **ND-V4-6** | اختبار "Truth = Lexer" إلزامي ودائم |
| **ND-V4-7** | كل خَطأ لديه `fix_suggestion_ar` و `fix_suggestion_en` |
| **ND-V4-8** | كل كيان لديه حقل `since` |

---

## التَواصل

- **المالك:** صالح
- **المُنفِّذ:** Amelia (Dev Agent)
- **آخر تَحديث:** 2026-06-05 (تَطبيق ADR-DOCS-V4-003 — V5 Codegen Integration)
- **آخر مُراجَعة حوكمة:** `VERIFICATION_REPORT_2026-05-30.md`
- **السبرنت:** Sprint #1 Execution Layer

للمساهمة، اطَّلع على [STRATEGY.md](STRATEGY.md) ثم [ARCHITECTURE.md](ARCHITECTURE.md) ثم [IMPLEMENTATION_PLAN.md](IMPLEMENTATION_PLAN.md) ثم [ADR-DOCS-V4-004](decisions/ADR-DOCS-V4-004-NO-CLI.md) ثم [ADR-DOCS-V4-003](decisions/ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md) ثم [ADR-DOCS-V4-002](decisions/ADR-DOCS-V4-002-UNIFIED-STRUCTURE.md).
