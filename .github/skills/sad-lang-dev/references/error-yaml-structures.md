# بنى YAML للأخطاء الثلاث ومسار الهجرة V4→V5

> يوجد في المستودع **ثلاث بنى YAML مختلفة للأخطاء، لا تتطابق أيٌّ منها**. هذا الملف يشرح لأي
> نظام توثيق كلٌّ منها، ومسار الهجرة الرسمي. اقرأه قبل لمس أي ملف أخطاء YAML حتى لا تخلط الأنظمة.
> المصادر: `error_messages_yaml_migration.md` + ADR-DOCS-V4-001..004 في
> `_bmad-output/systems/living-documentation/`.

---

## البنية ١ — كتالوج رسائل الأخطاء (متداخلة) — النشط فعلياً

**أين:** `language-truth/errors/*.yaml` (V5، **المصدر النشط الموحَّد**، مربوط في CMake منذ EM-V5-1).
**`data/language/error_messages.yaml` (V4) حُذف** (EM-V5-3، 2026-06-10).

```yaml
# رأس الملف: version / category / description_ar / description_en / errors:
errors:
- code: LEX_INVALID_CHARACTER      # يطابق اسم ErrorCode بعد :: (رمزي، لا CAT_NNNN)
  id: LEX001                        # ^[A-Z]{2,3}\d{3}$
  category: lexical
  title:    { ar, en }
  brief:    { ar, en }              # الرسالة المختصرة + placeholders
  fix_hint: { ar, en }              # تلميح الإصلاح
  placeholders: [char]
  detailed: { ar, en }              # شرح وضع المعلّم
```

- **النظام:** كتالوج **رسائل** الأخطاء في وقت التشغيل.
- **الوظيفة:** يقرؤه `scripts/codegen/gen_error_messages.py` → يولّد `shared/errors/generated/` →
  **هو ما يصنع رسائل الأخطاء الفعلية للمفسر والمترجم**.
- **الأصل:** وُلِّد بـ `migrate_error_messages.py` من `error_codes.cpp` + `error_catalog_init.cpp`
  (لحلّ مشكلة 3 ملفات C++ يدوية + 177 خطأ من 203 بلا template غني).
- **هذا ما يطابقه المثال في [./error-system.md](./error-system.md) §3.**

## البنية ٢ — schema V5 المُطبَّع (مسطّحة) — الهدف المستقبلي

**أين:** `language-truth/_schemas/error.schema.json`.

```yaml
id: error.LEX_0001          # ^error\.[A-Z]{3}_\d{4}$
schema_version: "5.0"
code: LEX_0001              # ^[A-Z]{3}_\d{4}$  ← LEX_0001 لا LEX_INVALID_CHARACTER
category: lexical
message_ar / message_en     # مسطّحة — لا title/brief متداخلة
hint_ar / hint_en
since / status / severity
```

- **النظام:** الصيغة **المُطبَّعة المستهدفة** لكتالوج الرسائل تحت V5 الكامل.
- **تحديث (2026-06-09):** بعد حسم ق-فرعي-1 لصالح المتداخلة، **صُحِّح `error.schema.json` ليصف
  البنية المتداخلة** (الصيغة المسطّحة أعلاه أُلغيت). الملفات الثمانية (231 خطأً) تجتازه الآن بصفر
  مخالفات. لم تعد البنية ٢ متعارضة مع الملفات.

## البنية ٣ — كتالوج بيانات sadinfo (كيانات) — نظام مستقل

**أين:** `data/_schemas/error.schema.json` (v1) + مجلد لكل كيان `errors/error_e001/_index.yaml`.

```yaml
schema_version: 1
id: error.xxx               # ^error\.[a-z][a-z0-9_]*$
kind: error
code: E0001                 # ^E[0-9]{4}$  ← مفتاح ASCII للأدوات
name: "اسم العرض"           # قابل للـ override في i18n/*.yaml
category: lexer|parser|type|runtime|security|linker|io|concurrency
severity: error|warning|hint
owners: [@user]             # ملكية/مسؤولية
```

- **النظام:** **كتالوج بيانات أداة `sadinfo`** (الاستعلام/التصفّح/الموقع/i18n). وصفه صراحةً:
  «`_index.yaml` لرسائل الأخطاء، مرجع DATA_SCHEMA_CONTRACTS §4.3».
- **الوظيفة:** **ليس** رسائل وقت تشغيل، بل **بيانات وصفية** للفهرسة والتصفّح (`E0001` مفتاح للأدوات،
  `owners`/`severity`، اسم عرض قابل للترجمة). يخدمه `shared/sadinfo_core/` و`tools/sadinfo/`.
- **هو الـ schema الذي يستخدمه `validate_schemas.py` فعلياً** (يتحقق من `data/_schemas/` مقابل
  fixtures في `tests/sadinfo/fixtures/`).

---

## مسار الهجرة: من أي نظام إلى أي نظام

**الهجرة = V4 → V5 (نظام «Living Documentation»)** — موثّقة في
`_bmad-output/systems/living-documentation/` بقرارات ADR-DOCS-V4-001..004:

| المرحلة | الحالة |
|---|---|
| **V4-أصلي** | رسائل الأخطاء في **3 ملفات C++ يدوية** (`error_codes.h/.cpp` + `error_catalog_init.cpp`) — يكسر DRY و Open/Closed |
| **V4-yaml** | `migrate_error_messages.py` وحّدها في **ملف واحد ضخم** `data/language/error_messages.yaml` + codegen → C++. **(مُتقاعَد — حُذف في EM-V5-3)** |
| **V5-M0** | إنشاء `language-truth/` **بالتوازي** مع `data/language/` |
| **V5-M1** | توسعة إلى **14 نطاقاً**؛ **تقسيم** الأخطاء إلى ملفات فئات في `language-truth/errors/` |
| **V5 (الأخطاء، مُنجَز)** | المولّد يقرأ `language-truth/errors/` + مربوط CMake (EM-V5-1)؛ sadinfo إسقاط مُولَّد (EM-V5-2)؛ V4 محذوف (EM-V5-3). **متبقٍّ: تجميع المُولَّد في C++ (EM-3)** |

**جوهر تحوّل V5 (ADR-DOCS-V4-003):** التكامل مع **Python codegen → Generated C++** (بدل runtime
`yaml-cpp`)، و**بلا CLI** (ADR-V4-004). المصدر YAML واحد مُطبَّع يُولِّد C++ وقت البناء.

### العلاقة بين الأنظمة الثلاثة
- **١ و٢ توحّدا:** الصيغة المتداخلة (البنية ١) اعتُمدت رسمياً، و`error.schema.json` صُحِّح ليصفها
  (ق-فرعي-1) — لم تعد البنيتان متعارضتين. `language-truth/errors/` هو المصدر النشط، وV4 حُذف.
- **٣ صار إسقاطاً مُولَّداً:** كتالوج sadinfo يُولَّد الآن من `language-truth/` (EM-V5-2) — لا مصدر
  يدوي موازٍ.

---

## القرار المعماري المُعتمَد: مصدر حقيقة موحَّد

الأنظمة الثلاثة **ليست تصاميم متنافسة** — هي دَيْن هجرة. القرار محسوم في
`ADR-DOCS-V4-002` (authority: SoT) ومُعاد تأكيده في
`_bmad-output/systems/living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md`:

- **`language-truth/` (V5) هو المصدر الموحَّد المطلق**؛ كل أداة (اللغة، الموقع، LSP، المنسّق،
  الحزم، **sadinfo**) تستهلك **إسقاطاً مُولَّداً** منه — لا مصدراً يدوياً خاصاً بها.
- **محسوم (2026-06-09، ق-فرعي-2):** sadinfo **إسقاط مُولَّد** من `language-truth/` لا مصدر — اعتماده
  مصدراً = قلب التبعية (الحقول الغنية تُنقَل إلى المصدر ثم يُولَّد sadinfo).
- البنية ١ = V4 يُتقاعَد (M2)؛ البنية ٢ = الهدف الموحَّد؛ البنية ٣ = إسقاط مُولَّد.
- **محسوم (2026-06-09، ق-فرعي-1):** الصيغة الرسمية **المتداخلة** (`title/brief/fix_hint/detailed`).
  ✅ **مُنفَّذ:** صُحِّح `language-truth/_schemas/error.schema.json` ليصفها (231→75 يجتازون).
- ADR-005 `Accepted` (كل القرارات الفرعية الثلاثة محسومة).

## القاعدة العملية للمطوّر (بعد توحيد الأخطاء)

1. **لتغيير رسالة خطأ:** عدّل `language-truth/errors/<category>.yaml` (المصدر الموحَّد) ثم أعد البناء
   (هدف `sad_error_messages_codegen`) — راجع [./error-system.md](./error-system.md) §4.
2. **انسخ بنية خطأ شقيق** في نفس الملف (الصيغة المتداخلة المُعتمَدة).
3. **لا تحرّر `data/errors/` يدوياً** — إسقاط مُولَّد من المصدر (EM-V5-2).
4. **لا تخلط الأنظمة:** بيانات sadinfo (`E####`/`owners`) إسقاط، لا مصدر رسائل.
