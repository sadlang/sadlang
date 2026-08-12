"""
============================================================================
(AR) حارسُ انجرافِ `language-truth/backend/arch_specific_opcodes.yaml`.

     الجدولُ يُعلن أيَّ أوپكودات SIR يبثّ خفضُها تعليماتٍ لا يفهمها إلّا معالجٌ
     من عائلةٍ بعينها. وإعلانٌ كهذا يبلى من طرفين، فيُحرَس من طرفين:

       (١) **اسمٌ ميّت**: أوپكودٌ في الجدول لا وجودَ له في كتالوج الأوپكودات
           ⇒ قيدٌ لا يُطبَّق أبدًا، وخضرةٌ بلا حراسة.
       (٢) **مُصدِرٌ يفلت**: مُصدِرٌ في شيفرة الخلفيّة يحوي أسمبليَّ x86 وأوپكودُه
           غيرُ مُعلَنٍ في الجدول ⇒ يُبَثُّ لأيّ هدفٍ بلا بوّابة. وهذا الطرفُ هو
           المهمّ: القوائمُ المكتوبةُ باليد تُصلَح مرّةً ثمّ يفلت منها التالي.

     العطبُ المقيس الذي ولّد الجدول (٢٠٢٦‑٠٨‑١٢): `عداد_الدورات()` بـ
     `--هدف=aarch64-unknown-elf` يخرج **بصفر** ويبثّ `rdtsc`؛ والأمرُ نفسُه على
     `i686-unknown-elf` — أي على المعماريّة التي تعمل عليها نواةُ النحلة اليوم.

     ⚠️ هذا الملفُّ **ساكن**: يقرأ الشيفرةَ والـYAML ولا يبني ولا يشغّل. فلا
     يتخطّى لغياب ثنائيٍّ، ولا يُخفي انجرافًا خلف تخطٍّ.
============================================================================
"""
from __future__ import annotations

import re
from pathlib import Path

import pytest

yaml = pytest.importorskip("yaml", reason="pyyaml مطلوب لقراءة مصدر الحقيقة")

ROOT = Path(__file__).resolve().parents[3]
SOT = ROOT / "language-truth" / "backend" / "arch_specific_opcodes.yaml"
OPCODE_CATALOG = ROOT / "language-truth" / "backend" / "sir_opcodes.yaml"
BUILDERS = ROOT / "compiler" / "src" / "backend" / "llvm" / "builders"

# (AR) منمنماتٌ لا تُنطَق إلّا على x86. قائمةٌ محافِظة: وجودُ إحداها في سلسلةٍ
#      داخل مُصدِرٍ شاهدٌ كافٍ على القيد؛ وغيابُها ليس شاهدًا على انتفائه.
#
#      ⚠️ **لا تُشتقّ من حقل `witness` عمدًا**، وإن بدت تكرارًا: الأداةُ لا يجوز
#      أن تستمدَّ بصرَها من المقيس. مُصدِرٌ جديدٌ يبثّ `xsetbv` — منمنمةً لا
#      يحملها أيُّ `witness` بعدُ — لن يراه حارسٌ مفرداتُه مشتقّةٌ من الجدول
#      الذي يحرسه، فتصير الخضرةُ بلا شاهد.
#
#      ⚠️ **بحدود كلمة** لا باحتواءٍ نصّيّ: `"inl" in "inline"` صحيحٌ نصًّا،
#      فمطابقةُ الاحتواء اتّهمت `emitCall` و`emitShl` وثلاثةَ مُصدِراتِ أقفالٍ
#      دوّارة بأنّها تبثّ منافذَ x86 — خمسةُ بلاغاتٍ كاذبةٍ من كلمة `inline`.
_X86_MNEMONICS = (
    "cli", "sti", "hlt", "cpuid", "rdtsc", "rdmsr", "wrmsr",
    "invlpg", "lgdt", "lidt", "sgdt", "sidt", "iret",
    "inb", "outb", "inw", "outw", "inl", "outl",
    "mfence", "lfence", "sfence", "wbinvd", "pause",
    "pushfl", "popfl", "pushfq", "popfq",
)
_X86_WITNESS_RE = tuple(
    (m, re.compile(rf'(?<![A-Za-z0-9_]){re.escape(m)}(?![A-Za-z0-9_])'))
    for m in _X86_MNEMONICS
) + (
    ("%cr", re.compile(r'%cr[0-8]?')),
    ("int $", re.compile(r'\bint \$')),
    ("rep stos", re.compile(r'\brep\s+stos')),
)

# (AR) كلُّ سلسلةٍ حرفيّةٍ في جسم الدالّة — لا السلسلةُ التي تلي `InlineAsm::get(`.
#
#      ⚠️ أوّلُ صياغةٍ طابقت `InlineAsm::get(\w+, "…")` فأخطأت ثلاثةَ أشكالٍ شائعة:
#      نصٌّ مبنيٌّ في متغيّرٍ ثمّ مُمرَّرٌ (`asmStr` في اقرأ/اكتب منفذ)، ونصٌّ
#      مركَّبٌ بالجمع (`std::string(mnemonic) + " ($0)"` في lgdt/lidt)، ودالّةٌ
#      مساعدةٌ حرّةٌ بلا `::` فتُنسَب سلاسلُها إلى آخِرِ تعريفٍ قبلها. النتيجةُ
#      المقيسة: ستّةُ أوپكوداتٍ مُعلَنةٍ لا يراها الحارسُ — منها BUILTIN_RDTSC
#      نفسُه، الذي وُلِدت الرقعةُ لأجله — وسابعٌ «مغطًّى» بشاهدٍ منتحَل.
_STRING_LITERAL = re.compile(r'"((?:[^"\\]|\\.)*)"')
# (AR) التعليقاتُ تُمحى قبل المسح — بمسافاتٍ تحفظ الإزاحات فتبقى حدودُ الدوالّ
#      صحيحة. تعليقٌ يذكر منمنمةً بين علامتَي اقتباس (`خارجي("inl")`) ليس بثًّا،
#      واحتسابُه اتّهم `emitCall` ببثّ منفذِ x86. (نظيرُ درسِ عدِّ `افحص_`:
#      السطرُ الذي يذكر الشيءَ ليس السطرَ الذي يفعله.)
_COMMENT = re.compile(r'//[^\n]*|/\*.*?\*/', re.S)


def _strip_comments(text: str) -> str:
    return _COMMENT.sub(lambda m: re.sub(r'[^\n]', ' ', m.group(0)), text)


# (AR) هروبُ الأسطر والجدولة داخل السلسلة يُفكّ قبل المطابقة: نصُّ الأسمبلي
#      يُكتب `"cld\n\trep stosl"`، فحرفا `\` و`t` يلتصقان بـ`rep` فيمنعان حدَّ
#      الكلمة ⇒ `rep stos` لا يُطابَق وBUILTIN_MEM_FILL_32 يفلت.
def _unescape(literal: str) -> str:
    return literal.replace("\\n", "\n").replace("\\t", "\t").replace("\\r", "\r")


# (AR) نداءُ دالّةٍ مساعدةٍ داخل جسم مُصدِر — تُورَّث شواهدُها إليه.
_HELPER_CALL = re.compile(r'\b(emit\w+)\s*\(')
# (AR) بدايةُ دالّةٍ في ملفّ الخلفيّة: عضوٌ (`::emitX(`) أو حرّةٌ ساكنة (`static … emitX(`).
_FUNCTION_START = re.compile(r'(?:::|\bstatic\s+[\w:*&<>\s]*?\b)(emit\w+)\s*\(')
# (AR) ربطُ الأوپكود بمُصدِره: حالاتُ `case` **مكدَّسةٌ** كثيرًا
#      (`case A: case B: return cg_.emitX(inst);`) فتُلتقَط كلُّها لا الأولى.
#      و`cg_.` اختياريّةٌ عمدًا: التوزيعُ داخل صنفِ البناء نفسِه يُكتَب
#      `return emitX(inst);` في ٤٨ موضعًا، ولا يقع أيٌّ منها اليومَ في ملفٍّ
#      يبثّ أسمبليَّ x86 — فاشتراطُها كان يمرّ خضرةً وينكسر أوّلَ مُصدِرٍ جديد.
_DISPATCH = re.compile(
    r'((?:case\s+SIROpcode::\w+:\s*)+)[^;]{0,200}?return\s+(?:cg_\.)?(\w+)\(inst\);', re.S)
_CASE_NAME = re.compile(r'case\s+SIROpcode::(\w+):')


def _sot() -> dict:
    return yaml.safe_load(SOT.read_text(encoding="utf-8"))


def _declared_opcodes() -> set[str]:
    return {entry["name"] for entry in _sot()["opcodes"]}


def _x86_emitters() -> dict[str, set[str]]:
    """(AR) كلُّ مُصدِرٍ يحوي سلسلةً فيها منمنمةُ x86 ⇒ {اسمُ المُصدِر: المنمنمات}.

    النطاقُ جسمُ الدالّة كاملًا (من تعريفٍ إلى الذي يليه) لا نداءُ `InlineAsm::get`
    وحدَه — فالنصُّ قد يُبنى في متغيّرٍ أو يُركَّب بالجمع قبل تمريره.
    """
    found: dict[str, set[str]] = {}
    calls: dict[str, set[str]] = {}
    for path in sorted(BUILDERS.rglob("*.cpp")):
        text = _strip_comments(path.read_text(encoding="utf-8"))
        starts = [(m.start(), m.group(1)) for m in _FUNCTION_START.finditer(text)]
        if not starts:
            continue
        bounds = [(pos, name, starts[i + 1][0] if i + 1 < len(starts) else len(text))
                  for i, (pos, name) in enumerate(starts)]
        for start, name, end in bounds:
            body = text[start:end]
            for literal in _STRING_LITERAL.finditer(body):
                hits = {witness for witness, pattern in _X86_WITNESS_RE
                        if pattern.search(_unescape(literal.group(1)))}
                if hits:
                    found.setdefault(name, set()).update(hits)
            callees = {c for c in _HELPER_CALL.findall(body) if c != name}
            if callees:
                calls.setdefault(name, set()).update(callees)

    # (AR) توريثٌ عبر النداء حتّى الثبات: مُصدِرٌ لا سلسلةَ فيه لكنّه ينادي دالّةً
    #      مساعدةً تبثّ x86 مقيَّدٌ مثلَها. `مؤقت_انتظر` صار كذلك بعد توحيد قراءةِ
    #      عدّاد الدورات في مساعدةٍ واحدة — ولولا التوريثِ لسقط من التغطية بسبب
    #      إصلاحِ تكرارٍ، أي لعاقَبَنا الحارسُ على تحسينٍ صحيح.
    changed = True
    while changed:
        changed = False
        for caller, callees in calls.items():
            inherited: set[str] = set()
            for callee in callees:
                inherited |= found.get(callee, set())
            if inherited - found.get(caller, set()):
                found.setdefault(caller, set()).update(inherited)
                changed = True
    return found


def _opcodes_of_emitter() -> dict[str, set[str]]:
    """(AR) عكسُ جدولِ التوزيع: اسمُ المُصدِر ⇒ **كلُّ** الأوپكودات التي تُوزَّع إليه."""
    mapping: dict[str, set[str]] = {}
    for path in sorted(BUILDERS.rglob("instr_*.cpp")):
        text = _strip_comments(path.read_text(encoding="utf-8"))
        for cases, emitter in _DISPATCH.findall(text):
            mapping.setdefault(emitter, set()).update(_CASE_NAME.findall(cases))
    return mapping


def test_sot_file_exists():
    assert SOT.exists(), f"مصدرُ الحقيقة مفقود: {SOT}"


def test_every_declared_opcode_exists_in_catalog():
    """(AR) الطرفُ الأوّل: لا اسمَ ميّتًا في الجدول."""
    catalog = {o["name"] for o in yaml.safe_load(
        OPCODE_CATALOG.read_text(encoding="utf-8"))["opcodes"]}
    unknown = sorted(_declared_opcodes() - catalog)
    assert not unknown, (
        "أوپكوداتٌ مُعلَنةٌ في arch_specific_opcodes.yaml لا وجودَ لها في "
        f"sir_opcodes.yaml ⇒ قيدٌ لا يُطبَّق أبدًا: {unknown}")


def test_every_declared_family_is_defined():
    data = _sot()
    families = {f["id"] for f in data["families"]}
    orphans = sorted({o["name"] for o in data["opcodes"] if o["family"] not in families})
    assert not orphans, f"أوپكوداتٌ تشير إلى عائلةٍ غيرِ معلَنة: {orphans}"


def test_no_x86_emitter_escapes_the_declaration():
    """(AR) الطرفُ الثاني — الأهمّ: لا مُصدِرَ x86 خارجَ الجدول.

    مُصدِرٌ جديدٌ يبثّ `outb` ولا يُعلَن هنا يُبَثُّ لـaarch64 بخروجٍ صفريّ.
    """
    emitters = _x86_emitters()
    assert emitters, (
        "لم يُعثَر على أيّ مُصدِرٍ يبثّ أسمبليَّ x86 — التعبيرُ المنتظمُ انكسر "
        "على الأرجح، والاختبارُ صار تأكيدًا بصفرِ شاهد")

    by_opcodes = _opcodes_of_emitter()
    declared = _declared_opcodes()
    escaped: list[str] = []
    covered: set[str] = set()
    for emitter, witnesses in sorted(emitters.items()):
        opcodes = by_opcodes.get(emitter)
        if not opcodes:
            # (AR) مُصدِرٌ بلا حالةِ توزيعٍ مقروءة: قد يكون دالّةً مساعدةً يناديها
            #      مُصدِرٌ آخرُ مُعلَنٌ أصلًا (فلا بأس)، وقد يكون بابًا. لا يُحتسَب
            #      تغطيةً — والتأكيدُ على العدد أدناه هو ما يمسك الفرقَ إن كان بابًا.
            continue
        for opcode in sorted(opcodes):
            if opcode in declared:
                covered.add(opcode)
            else:
                escaped.append(f"{emitter} → {opcode} — {sorted(witnesses)}")
    assert not escaped, (
        "مُصدِراتٌ تبثّ أسمبليَّ x86 وأوپكودُها غيرُ مُعلَنٍ في "
        "language-truth/backend/arch_specific_opcodes.yaml ⇒ تُبَثُّ لأيّ هدفٍ "
        "بلا بوّابة:\n  " + "\n  ".join(escaped))

    # (AR) والطرفُ المكمّل: كلُّ **مُعلَنٍ** يجب أن يراه الحارس. بدونه يبقى الحارسُ
    #      أخضرَ وهو أعمى عن جزءٍ ممّا يُعلنه — وقد كان يرى أربعةً وعشرين من ثلاثين
    #      حينها، ومنها BUILTIN_RDTSC. الفرقُ يُطبَع كي لا يُقرأ الصمتُ تغطية.
    unseen = sorted(declared - covered)
    assert not unseen, (
        f"الحارسُ لا يرى {len(unseen)} أوپكودًا مُعلَنًا (يرى {len(covered)} من "
        f"{len(declared)}) ⇒ حذفُ أيٍّ منها من مصدر الحقيقة يمرّ أخضر: {unseen}")


def test_generated_header_matches_sot():
    """(AR) الرأسُ المولَّد يحمل كلَّ ما في الـYAML — وإلّا فالبناءُ يقرأ جدولًا بائتًا."""
    header = (ROOT / "compiler" / "include" / "backend" / "generated"
              / "arch_specific_opcodes_generated.h")
    assert header.exists(), (
        f"الرأسُ المولَّد مفقود: {header} — شغّل `x.py gen`")
    text = header.read_text(encoding="utf-8")
    missing = sorted(op for op in _declared_opcodes() if f'"{op}"' not in text)
    assert not missing, (
        f"الرأسُ المولَّد بائتٌ عن مصدر الحقيقة — ينقصه: {missing}. شغّل `x.py gen`")
