# طبقة الخلفيّة السياديّة الجدوليّة — `language-truth/backend/`

> مصدرُ الحقيقة لجداول الخلفيّة الأصليّة (بلا LLVM) متعدّدة المعماريّات.
> يُنفّذ **م٠** من [RFC الخلفيّة السياديّة متعدّدة المعماريّات](../../sadlang-rfcs) —
> الهيكلُ ومخطّطاتُ SoT وعقدُ واجهة `Target` (الحواجز).

## المبدأ الحاكم

**الخطُّ الخلفيُّ واحدٌ مشترك؛ والاختلافُ بين المعماريّات بياناتٌ في جداولٍ لكلّ ISA لا كودًا.**

```
SIR (~٢٥٠ أوپكود، مستقلّ عن ISA؛ ~٦٠ منخفضة يخفضها الخلفيّ، والبقيّة ⇒ call runtime)
      │  خفضٌ مشترك
      ▼
MIR (تعليماتٌ مجرّدة + سجلّاتٌ افتراضيّة)
      │  ← isel.yaml            [بيانات/ISA]
      ▼
اختيار التعليمات + تخصيص السجلّات (linear-scan) — خوارزميّةٌ مشتركة
      │  ← registers.yaml + abi/<arch>-<os>.yaml   [بيانات/ISA×نظام]
      ▼
محرّك الترميز العامّ — يُكتَب مرّةً (فرعان: fixed32 | variable)
      │  ← instructions.yaml (حقول البتّات)   [بيانات/ISA]
      ▼
كاتب ELF مشترك (~٨٥٪) ← e_machine + relocations   [بيانات/ISA]
```

## البنية

```
language-truth/backend/
  <arch>/
    instructions.yaml   ← جدول التعليمات (الترميز الثنائيّ)      [backend_encoding.schema.json]
    registers.yaml      ← جدول السجلّات (num/role/reserved)      [backend_register_file.schema.json]
    isel.yaml           ← أنماط اختيار SIR/MIR → منمنمات         [backend_isel.schema.json]
  abi/
    <arch>-<os>.yaml    ← اصطلاح النداء + نداء النظام            [backend_abi.schema.json]
```

المعماريّاتُ المستهدَفة (خمسٌ): `x86_64`, `arm64`, `riscv64` (+`riscv32`), `armv7`, `i686`.
م٠ يملأ جداولَ **x86_64** وحدها (الهدف المرجعيّ). كلُّ جدولٍ يُتحقَّق في CI عبر
`language-truth/tests/test_schema_validation.py`.

## عقد واجهة `Target` (الحاجز المعماريّ)

الحاجزُ الذي يمنع تسرّبَ افتراضاتِ معماريّةٍ بعينها إلى الخطّ المشترك: **كلُّ لمسةٍ خاصّةٍ بمعماريّةٍ
تمرّ عبر عقدٍ واحد** — إمّا استعلامٌ من جدول، وإمّا فرعُ عائلةٍ (`fixed32`/`variable`) في محرّكٍ مشترك.
لا يُكتب اسمُ سجلٍّ أو أوپكودٍ حرفيًّا في الخطّ المشترك. يُشتقّ `Target` من الجداول أعلاه:

| الاستعلام | المصدر |
|---|---|
| `ptr_width` / `word_bits` | `instructions.yaml: word_bits` |
| `register_file()` | `registers.yaml` |
| `isel_patterns()` | `isel.yaml` |
| `abi(os)` | `abi/<arch>-<os>.yaml` |
| `instruction_table()` | `instructions.yaml: instructions` |
| `encoding_family` | `instructions.yaml: encoding_family` ∈ {`fixed32`, `variable`} |
| `elf_machine` / `reloc_kinds` | `abi/<arch>-<os>.yaml: e_machine` (+ جدول relocations لاحقًا) |

> **ملاحظة م٠:** واجهة `Target` بوصفها كودَ C++ يستهلك هذه الجداول تُنفَّذ في **م١** (حين يستهلكها
> محرّكُ الترميز)، تفاديًا لكودٍ ميت. م٠ يُثبت **العقدَ بياناتٍ ومخطّطًا** — الحدَّ الفاصلَ بين المشترك
> وبيانات الـISA — بوصفه الحاجزَ التأسيسيّ.

## الحدّ الفاصل: مشترك مقابل بيانات لكلّ ISA

| مشترك (يُكتَب مرّةً) | بياناتٌ لكلّ ISA (جداول) |
|---|---|
| خفض SIR/MIR، تخصيص السجلّات (linear-scan)، محرّك الترميز، كاتب ELF (~٨٥٪) | التعليمات، السجلّات، أنماط الاختيار، الـABI، e_machine/relocations |

**المنطق الضيّق الخاصّ** (كودٌ صغيرٌ لكلّ عائلةٍ لا لكلّ ISA): بادئاتُ x86 (REX/ModRM/SIB) عبر
`encode_prefixes`/`encode_modrm_sib`؛ interworking في ARMv7؛ ضغطُ RISC-V-C.

## الأساس التقنيّ

`تقارير_مؤقته/لغة ص/no_llvm/`: `تقرير_استقلال_المترجم_عن_LLVM.md` +
`تصميم_طبقة_الترميز_الجدوليّة.md`. البوّابةُ الحرجة (من م١): اختبارٌ تفاضليٌّ بايت-ببايت ضدّ
`llvm-mc` — LLVM مصحِّحٌ مرجعيٌّ في CI فقط، لا تبعيّة إنتاج.
