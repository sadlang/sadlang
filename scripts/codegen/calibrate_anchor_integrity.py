#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ المرساة — **يُحقَنُ العطبُ ويُثبَتُ الاحمرار**.

     حارسٌ لم يُثبَتْ احمرارُه ليس حارسًا. وأغلبُ الأعطابِ المُدوَّنةِ في سجلِّ
     دروسِ هذا المستودعِ صورةٌ واحدةٌ مكرّرة: **أخضرُ لأنّه لا يستطيعُ أن يحمرّ**.

     مجسّاتٌ عدّتُها `len(PROBES)` وأرضيّتُها `MIN_PROBES` — ولا تُنثَرُ ههنا
     عددًا يبلى. وأربعةٌ منها **يجبُ أن تبقى خضراءَ** فهي اختباراتُ انحدار:
     ما لا يُقاسُ ينمو، وما يُقاسُ خطأً يُوقِفُ الدمجَ بلا سبب.

     🔑 **والبايتاتُ تُستعادُ ويُتحقَّقُ منها بـsha256** بعدَ كلِّ حقنة. وترفضُ
        المِحقنةُ القياسَ على **أرضيّةٍ ملوّثةٍ** بأثرِ تشغيلةٍ لم تُنهَ — وأثرُها
        **يُشتقُّ من المجسِّ نفسِه** لا يُكتَبُ نسخةً ثانيةً تبلى بأوّلِ تعديل.

(EN) Injection harness for the anchor-integrity guard: inject a defect, prove
     the guard reddens, restore the bytes exactly (sha256-verified). Four
     probes must stay GREEN — they are regression tests, not defects.
============================================================================
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ.** وقِيسَ لِمَ: ستُّ مِحقناتٍ
#      تقاسمَت ٢٣ دالّة، **ستَّ عشرةَ منها انجرفَت** إلى صورتَينِ فستّ —
#      ومنها شبكةُ أمانِ الانهيارِ نفسُها. والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import (  # noqa: E402
    CRLF, LF_, Harness, append, create, eol_to_lf, run, self_red, sub,
)

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_anchor_integrity.py"
HARNESS = Path(__file__).resolve()
RECORD = ROOT / "scripts" / "codegen" / "calibration" / "check_anchor_integrity.yaml"


def _run_guard():
    return _HARNESS.run_guard()


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()


# (AR) 🔑 **اسمُ العائلةِ يُشتقُّ من الحارسِ لا يُنسَخُ سلسلة.** كان المجسُّ ⑰
#      يقرأُ مرجعَه بـ`.get(u"أوپكوداتُ الخلفيّة", (0, 0))`، فإعادةُ تسميةٍ
#      مشروعةٌ في `FAMILIES` تُبقي الحارسَ أخضرَ و`_measure_baseline` ناجحًا
#      (الصفوفُ الأربعةُ الأخرى موجودة) ويعودُ `need` إلى ١ — أي **عينُ سلوكِ
#      ما قبلَ الرقعة، بلا رمزِ ٢ وبلا سطرِ تشخيصٍ واحد**. والرسوُّ ههنا على
#      `"backend"`: مجلَّدٌ في مصدرِ الحقيقةِ لا اسمَ عرضٍ يُترجَم.
#      ⚠️ **والاشتقاقُ كسولٌ لا عندَ مستوى الوحدة.** رميٌ عندَ الاستيرادِ
#         يُوقِفُ **جمعَ pytest كلَّه**: قِيسَ `Interrupted: 1 error during
#         collection` و**صفرٌ من ١٤٢ اختبارًا يُنفَّذ** — فتُفقَدُ تشخيصاتُ
#         ١٤١ اختبارًا لا صلةَ لها. ورمزُه ١ يخالفُ عُرفَ «عطبُ الآلةِ = ٢»
#         المتَّبَعَ في هذا الملفِّ نفسِه.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from check_anchor_integrity import FAMILIES as _FAMILIES  # noqa: E402


def _opcode_family() -> str:
    found = next((f[0] for f in _FAMILIES if f[1] == "backend"), None)
    if found is None:
        raise AssertionError(
            "لا عائلةَ مصدرُها 'backend' في FAMILIES — أُعيدَ ترتيبُ الحارسِ")
    return found


# (AR) 🔑 **الأرضيّاتُ الخمسُ لا تُشَدُّ ويُطفَّرُ منها.** شرطُ «الحدِّ
#      المشدود» يقتصرُ على السقوفِ (فالأرضيّةُ مصمَّمةٌ للصعودِ وشدُّها يرفضُ
#      عملًا مشروعًا)، فبقيَ مجسُّ الأرضيّةِ ⑰ **يحذفُ بندًا واحدًا** وحمرتُه
#      مشروطةٌ بألّا يكونَ في الأرضيّةِ هامش. وقِيسَ: أرضيّةُ الأوپكوداتِ ٧٧٩
#      والمقيسُ ٧٨٠ ⇒ «⑰ لم يعضّ · 16/17» — والبوّابةُ خضراءُ لأنّها لا
#      تُشغِّلُ المِحقنات، فالشاهدُ يموتُ صامتًا. فالحذفُ الآنَ **بقدرِ الهامشِ
#      + ١** مقيسًا من الحارسِ قبلَ أوّلِ طفرة.
_ADDRESSES = re.compile("([^\n·]+?)\\s+عناوين:\\s+([0-9]+)\\s+\\(الأرضيّة\\s+([0-9]+)\\)")
_BASELINE: dict[str, tuple] = {}


def _measure_baseline() -> None:
    """(AR) عناوينُ كلِّ عائلةٍ وأرضيّتُها **قبلَ أوّلِ حقن**."""
    code, out = _run_guard()
    rows = _ADDRESSES.findall(out)
    if code != 0 or not rows:
        raise AssertionError(
            "تعذَّرَ قياسُ العناوينِ قبلَ الحقن (رمز=%d) — لا عيارَ على مرجعٍ مجهول"
            % code)
    for fam, addresses, floor in rows:
        _BASELINE[fam.strip()] = (int(addresses), int(floor))


def _shrink_opcodes(blob: bytes) -> bytes:
    """(AR) يحذفُ `BUILTIN_CLI` (وله توأمٌ في طبقةِ القيد) ومعه ما يبتلعُه
    هامشُ الأرضيّة — فيعضُّ المجسُّ سواءٌ أكانت مشدودةً أم لا."""
    text = blob.decode("utf-8")
    # (AR) والغيابُ يرمي ولا يستنُّ صفرًا: مرجعٌ مجهولٌ يعني مجسًّا أعمى.
    family = _opcode_family()
    if family not in _BASELINE:
        raise AssertionError(
            "لم يُقَسْ مرجعُ عائلةِ %s قبلَ الحقن — لا عيارَ على مرجعٍ مجهول"
            % family)
    addresses, floor = _BASELINE[family]
    need = max(1, addresses - floor + 1)
    head = "  - name: BUILTIN_CLI\n"
    if head not in text:
        raise AssertionError("المرساةُ غيرُ موجودة: BUILTIN_CLI")
    text = text.replace(head, "", 1)
    dropped = 1
    lines = text.split("\n")
    out, i = [], 0
    while i < len(lines):
        if dropped < need and lines[i].startswith("  - name: "):
            dropped += 1
            i += 1
            while i < len(lines) and not lines[i].startswith("  - name: ") \
                    and (lines[i].startswith("    ") or not lines[i].strip()):
                i += 1
            continue
        out.append(lines[i])
        i += 1
    return "\n".join(out).encode("utf-8")


SEED = "tests/behavior/rules_matrix/10_statements/gr.stmt.if/basic/001_true_cmp_gt.ص"
GRAMMAR = "language-truth/grammar/10_statements.yaml"
ARCHIVED = "tests/_archive/zz_anchor_probe.ص"
IF_ANCHOR = "# @rule: gr.stmt.if".encode("utf-8")

# (AR) 🔑 وبذرةُ المجسِّ ② **ليست أيَّ بذرة**: يلزمُها قاعدةٌ مرساتُها واحدةٌ
#      فقط، وإلّا لم يُيتِّمْها نزعُ مرساةٍ منها. (وقعتُ فيه أوّلَ مرّة: نُزِعت
#      مرساةُ `gr.stmt.if` فبقيت القاعدةُ مشهودةً ببذورٍ أخرى، ولم يعضَّ المجسّ
#      — **خللٌ في المجسِّ لا في الحارس**.) و`gr.adv.ffi_linkage` مقيسٌ في
#      ٢٠٢٦-٠٩-٠٥ أنّه الوحيدُ بمرساةٍ يتيمةِ العدد.
#      ولو زالت المرساةُ عن مكانِها لأَلقى `_sub` وانتهت المِحقنةُ بالرمز ٢
#      «لم تُقَسْ» — لا بمرورٍ صامت.
SOLE = ("tests/behavior/rules_matrix/_interactions/_generated/"
        "adv.ffi_extern_block__adv.ffi_linkage/001_ffi_block_linkage.ص")
SOLE_ANCHOR = "# @rule: gr.adv.ffi_extern_block, gr.adv.ffi_linkage".encode("utf-8")
STDLIB = "language-truth/stdlib/functions.yaml"
SIR = "language-truth/backend/sir_opcodes.yaml"
_BOM = b"\xef\xbb\xbf"


@self_red
def _bom_invented(blob: bytes) -> bytes:
    """(AR) بادئةُ BOM + مرساةٌ مخترَعةٌ **في السطرِ الأوّل** — وهي الحالةُ التي
    كانت تُبتلَعُ صامتة. وتُنزَعُ بادئةٌ سابقةٌ إن وُجِدت لئلّا تتكرّر."""
    body = blob[len(_BOM):] if blob.startswith(_BOM) else blob
    return _BOM + "# @rule: gr.stmt.zz_bom_invented\n".encode("utf-8") + body


@self_red
def _dup_opcode(blob: bytes) -> bytes:
    """(AR) أوپكودٌ جديدٌ باسمٍ **موجودٍ** — يذوبُ في عنوانِ غيرِه."""
    text = blob.decode("utf-8")
    marker = "\n  - name: "
    at = text.index(marker)
    name = text[at + len(marker):].split("\n", 1)[0].strip()
    add = f"\n  - name: {name}\n    description_ar: بندٌ مُعلَنٌ باسمٍ موجود"
    return (text[:at] + add + text[at:]).encode("utf-8")



# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    ("① مرساةٌ يتيمة — وسمٌ يُسمّي قاعدةً مخترَعة",
     SEED, sub(IF_ANCHOR, "# @rule: gr.stmt.zz_invented".encode("utf-8")),
     1, "مرساةٌ يتيمة: 1"),

    ("② دَينُ الشهادةِ ينمو — تُنزَعُ آخرُ مرساةٍ لقاعدة",
     SOLE, sub(SOLE_ANCHOR, "# @rule: gr.adv.ffi_extern_block".encode("utf-8")),
     1, "بلا شهادة:    3"),

    # (AR) الصيغةُ المعمَّمةُ ليست نصًّا في وثيقةٍ — تُقاسُ. ولولا هذا لكان
    #      `@مُعلَن:` وسمًا ميّتًا يُكتَبُ ولا يقرؤه أحد.
    ("③ `@مُعلَن:` يُقرأُ فعلًا — مدمجٌ مخترَع",
     SEED, append("\n# @مُعلَن: bi.ZzNever.NEVER\n".encode("utf-8")),
     1, "bi.ZzNever.NEVER"),

    # (AR) وهذا ما لم يكن يُقاسُ قطُّ: الرمزُ كان علامةَ «بذرةٌ سالبة» فقط،
    #      فبذرةٌ تكتبُ `SEM999` تمرُّ صامتةً ولا يقابلُها الكتالوج.
    ("④ `@expect_error` يُقابَلُ بالكتالوج — SEM999",
     SEED, append("\n# @expect_error: SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) العنوانُ العاري ليس عنوانًا: ٢٦ معرِّفَ مدمجٍ يتكرّرُ عبرَ النطاقات.
    ("⑤ العنوانُ مؤهَّلٌ — معرِّفٌ عارٍ يُرَدُّ",
     SEED, append("\n# @مُعلَن: bi.ADD\n".encode("utf-8")),
     1, "bi.ADD"),

    ("⑥ مُعلَنٌ جديدٌ بلا شهادةٍ يرفعُ الدَّين",
     GRAMMAR, append("  - id: gr.stmt.zz_probe\n    lhs: {nonterminal: ZzProbe}\n"
                      "    ebnf: \"ZzProbe = 'zz' ;\"\n".encode("utf-8")),
     1, "gr.stmt.zz_probe"),

    # ═══ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    # (AR) `_archive` متروكٌ بالتصميم: شجرةٌ محفوظةٌ بلهجةٍ ماتت لا تُشغَّل.
    #      وهذا المجسُّ يُثبِتُ أنّ تركَها **مقصودٌ ومقيسٌ** لا سهوٌ في الكنس.
    ("⑦ الأرشيفُ متروكٌ — مرساةٌ يتيمةٌ فيه لا تُحمِّر",
     ARCHIVED, create("# @rule: gr.zz.archived_never\nاطبع_سطر(\"x\")\n"
                       .encode("utf-8")),
     0, "مرساةٌ يتيمة (ترسو على غيرِ مُعلَن): 0"),

    # (AR) وسمٌ في **جسمِ** الملفِّ لا في أوّلِ سطرٍ ليس مرساة. ولولا هذا لكان
    #      ذكرُ اسمِ قاعدةٍ في وصفٍ نثريٍّ يُقرأُ شهادةً.
    ("⑧ نصٌّ في الجسمِ ليس مرساةً — لا شهادةَ بالذِّكر",
     SEED, append("اطبع_سطر(\"@rule: gr.zz.in_body\")\n".encode("utf-8")),
     0, "مرساةٌ يتيمة (ترسو على غيرِ مُعلَن): 0"),

    # (AR) بادئةُ BOM تسبقُ `#` فلا يطابقُها `^#`. وفي الشجرةِ ٨٦ بذرةً
    #      ببادئةٍ، أربعٌ منها مرساتُها في السطرِ الأوّل. ولولا هذا المجسِّ لعادَ
    #      العطبُ بأوّلِ رقعةٍ تُعيدُ `utf-8` مكانَ `utf-8-sig`.
    ("⑪ بادئةُ BOM لا تبتلعُ المرساة",
     SEED, _bom_invented, 1, "مرساةٌ يتيمة: 1"),

    # (AR) الصيغةُ الخاليةُ من النقطتَين هي **الأغلبيّةُ المُودَعة** (١٣١ مقابل
    #      ١٠١). وكان الحارسُ يقرأُ أقلَّ من نصفِ ما يزعمُ حراستَه.
    ("⑫ `@expect_error` بلا نقطتَين يُقابَلُ بالكتالوج",
     SEED, append("\n# @expect_error SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) انكماشُ المُعلَنِ ليس تقدُّمًا: عائلةٌ تسقطُ صامتةً و`0 ≤ السقف`.
    ("⑬ عائلةٌ تنكمشُ — أرضيّةُ المُعلَن",
     STDLIB, sub(b"functions:", b"functions_ZZ_RENAMED:"),
     1, "انكمشَ المُعلَن"),

    # (AR) بندٌ مُعلَنٌ باسمٍ موجودٍ يذوبُ في عنوانِ غيرِه فلا يُعَدُّ ولا يُطالَب.
    ("⑭ معرِّفٌ مكرَّرٌ يبتلعُ بندًا — سقفُ التصادم",
     SIR, _dup_opcode, 1, "نما الذوبان"),

    # (AR) الهجاءُ الثاني: `compile_` **بين** `expect_` و`error`، فلا لاحقةٌ
    #      تبلغُه. ٧٥ موضعًا في الشجرة، وحارسانِ شقيقانِ يعدّانِه ندًّا.
    ("⑮ `@expect_compile_error` هجاءٌ ثانٍ يُقابَلُ بالكتالوج",
     SEED, append("\n# @expect_compile_error: SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) قائمةٌ برموزٍ — تناظرًا مع `@rule:`. ولولا انتزاعِ الرمزِ من صدرِ كلِّ
    #      جزءٍ لبقيَ الرمزُ الثاني غيرَ مقروءٍ صامتًا.
    ("⑯ قائمةُ رموزٍ — الثاني يُقرأُ كالأوّل",
     SEED, append("\n# @expect_error: SEM001, SEM999\n".encode("utf-8")),
     1, "err.SEM999"),

    # (AR) طبقةُ القيدِ مستثناةٌ من الإعلان، فحذفُ أوپكودٍ من `sir` له توأمٌ فيها
    #      **ينكمشُ فعلًا** وتعضُّ الأرضيّة. وقبلَ الاستثناءِ كان التوأمُ يملأُ
    #      العنوانَ الشاغرَ فيمرُّ الحذفُ صامتًا.
    ("⑰ حذفُ أوپكودٍ له توأمٌ في طبقةِ القيدِ يعضّ",
     SIR, self_red(_shrink_opcodes),
     1, "انكمشَ المُعلَن"),

    ("⑨ مسبارٌ أعمى — رمزُ عطبِ آلةٍ 2",
     "scripts/codegen/check_anchor_integrity.py",
     sub(b'SKIP_PARTS = ("_archive",)',
          b'SKIP_PARTS = ("_archive", "behavior", "unit")', residue=True),
     2, "عيارُ الأداة:"),

    ("⑩ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     "scripts/codegen/check_anchor_integrity.py",
     eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`)،
#      وههنا تمنعُ **المِحقنةَ نفسَها** من إعلانِ نجاحٍ بلا قياس: بـ`PROBES = ()`
#      تُرجِعُ صفرًا وتكتبُ `0/0`. وأداةُ قياسٍ تُعلِنُ نجاحًا بلا أن تقيسَ هي
#      عينُ ما تُنشَأُ لمنعِه.
MIN_PROBES = 17


# ═══ المِحقنةُ إعلانٌ: حارسٌ · عنوانٌ · صفُّ مجسّات ═════════════════════════
_HARNESS = Harness(
    guard=GUARD,
    harness=HARNESS,
    record=RECORD,
    title="عيارُ حارسِ «المُعلَنُ يعرفُ مَن يشهدُ له» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    baseline=_measure_baseline,
    floors="slack",
)

if __name__ == "__main__":
    run(_HARNESS)
