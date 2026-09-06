# -*- coding: utf-8 -*-
"""
============================================================================
(AR) اختباراتُ وحدةٍ لمِحقنةِ العيارِ وحارسِها الفوقيّ.

     🔑 **ولمَ هي موجودة:** المِحقنةُ `calibrate_builtin_coverage.py` أقوى من أيِّ
        اختبارِ وحدةٍ — تحقنُ عطبًا حقيقيًّا وتقيسُ احمرارَ الحارس. لكنّها
        **لا تعملُ في CI**: الشوطُ يفحصُ بصمتَها في السجلِّ ولا يُشغِّلُها.
        فلامتغيِّراتُها البنيويّةُ — تلك التي لا تحتاجُ حقنًا — تبقى بلا قياسٍ
        في المسارِ الوحيدِ الذي يُوقِفُ الدمج. وههنا موضعُها.

     وما يُقاسُ ههنا هو ما بلِيَ فعلًا في مراجعاتٍ خصميّةٍ متتالية:
       ① كلُّ مجسٍّ صامتِ الأثرِ **مُتتبَّعٌ** في الاشتقاق — وإلّا قاست المِحقنةُ
          فوقَ أثرِ نفسِها وأعلنت نجاحًا. (بلِيَ حين كان السجلُّ يدويًّا.)
       ② سِمةُ الأثرِ **دليلٌ** لا لفظٌ عابر.
       ③ البصمةُ **لا تتغيّرُ بنهاياتِ الأسطر** — وإلّا حمِرَ كلُّ استنساخٍ نظيف.
       ④ أرضيّةُ العمقِ في الطرفَين **لا تتباعد**.
       ⑤ التاريخُ تاريخٌ لا شكلٌ يُشبِهُه.

(EN) Unit tests for the calibration harness and its meta-guard. The harness
     itself is stronger, but it does not run in CI — only its fingerprint is
     checked — so its structural invariants are measured here instead.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

import pytest

CODEGEN = Path(__file__).resolve().parent
if str(CODEGEN) not in sys.path:
    sys.path.insert(0, str(CODEGEN))

import calibrate_anchor_integrity as anchor_harness  # noqa: E402
import calibrate_builtin_coverage as harness  # noqa: E402
import calibrate_seed_contract as seed_harness  # noqa: E402
import check_calibration_fresh as meta  # noqa: E402

# (AR) 🔑 **المِحقنتانِ معًا.** كان الاختبارُ يستوردُ واحدةً، فلامتغيِّراتُ الأخرى
#      بلا قياسٍ في CI — وهما لا تعملانِ في CI أصلًا، فهذا مساسُهما الوحيد.
#      و«الرقعةُ تسدُّ في ملفٍّ وتتركُ الأخوات» درسٌ مُدوَّن.
# (AR) 🔑 **ومِحقناتُ البوّابةِ تُشتقُّ ولا تُكتَبُ صفًّا.** صفٌّ يدويٌّ يبلى
#      في اتّجاهٍ واحد: مِحقنةٌ رابعةٌ تصلُ فتفلتُ من كلِّ لامتغيِّرٍ ههنا،
#      والاختبارُ يبقى أخضرَ لأنّه لا يعرفُ بوجودِها. والمعيارُ **نحويٌّ
#      وبنيويّ**: مِحقنةٌ لها `PROBES` و`MIN_PROBES` هي مِحقنةُ بوّابة.
def _gate_harnesses():
    import importlib

    found = []
    for path in sorted(CODEGEN.glob("calibrate_*.py")):
        module = importlib.import_module(path.stem)
        if hasattr(module, "PROBES") and hasattr(module, "MIN_PROBES"):
            found.append(module)
    assert found, "لا مِحقنةَ عيارٍ في الشجرة — الاختبارُ صارَ يحرسُ العدم"
    return tuple(found)


HARNESSES = _gate_harnesses()


# ═══ ① كلُّ مجسٍّ يُصرِّحُ بأثرِه ══════════════════════════════════════════
# (AR) 🔑 كان ههنا **قائمةُ بادئاتِ مساراتٍ يدويّة** يُقاسُ عليها «أهذا المجسُّ
#      صامت؟» — نسخةٌ ثانيةٌ من حقيقةٍ في المِحقنة، تبلى بلا صوتٍ عندَ نقلِ
#      ملفٍّ أو إضافةِ مجسّ. وقد بلِيَت فعلًا: مجسُّ بادئةِ BOM في مِحقنةِ العقدِ
#      كان **بلا تصريحِ أثر**، ومسارُ هدفِه يظهرُ في الاشتقاقِ لأنّ مجسًّا آخرَ
#      يُسجِّلُ المسارَ نفسَه — فبدا متتبَّعًا. وقِيسَ: قتلٌ في منتصفِه يتركُ بذرةً
#      **مُودَعةً** مُفسَدة، والحارسُ أخضرُ والعدّاءُ أخضرُ وهذا الاختبارُ أخضر.
#      فصارَ التصريحُ **واجبًا في المِحقنةِ نفسِها** (`_derive_residue` يرمي)،
#      وههنا يُقاسُ ما لا تستطيعُ هي قياسَه: **صدقُ** التصريح.


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_every_probe_declares_its_residue(harness):
    """(AR) صفٌّ لكلِّ مجسّ — **ويُقاسُ الرفضُ بالحقنِ لا بطولِ القائمة.**
    طولٌ يساوي طولًا تحصيلُ حاصلٍ بعدَ أن صارَ الاشتقافُ يرمي؛ فيُدَسُّ مجسٌّ
    بلا تصريحٍ ويُنتظَرُ الرفض."""
    assert len(harness._derive_residue()) == len(harness.PROBES), (
        "الاشتقاقُ يطوي مجسَّينِ في صفٍّ واحد — أحدُهما يركبُ تسجيلَ الآخر")

    def _undeclared(blob):
        return blob

    saved = harness.PROBES
    try:
        harness.PROBES = saved + (("زائفٌ بلا تصريح", "zz", _undeclared, 0, ""),)
        with pytest.raises(AssertionError):
            harness._derive_residue()
    finally:
        harness.PROBES = saved


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_a_green_probe_may_not_claim_the_gate_catches_it(harness):
    """(AR) 🔑 مجسٌّ يُنتظَرُ منه **رمزُ صفرٍ** لا يستطيعُ أن يزعمَ أنّ البوّابةَ
    تلتقطُ أثرَه: البوّابةُ خضراءُ بالتعريفِ حينَه. وهذا هو الزعمُ الذي لا
    يُكذِّبُه شيءٌ سواه."""
    liars = [entry[0] for entry in harness.PROBES
             if entry[3] == 0
             and getattr(entry[2], "residue", None) is harness._SELF_RED]
    assert not liars, (
        "مجسٌّ أخضرُ يزعمُ أنّ البوّابةَ تلتقطُ أثرَه — ولا شاهدَ عليه: %s" % liars)


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_no_trace_carries_a_reason(harness):
    """(AR) `_NO_TRACE` بابُ صمتٍ مشروع — فيُقفَلُ بتعليلٍ مكتوبٍ لا بإغفال."""
    for entry in harness.PROBES:
        if getattr(entry[2], "residue", None) is not harness._NO_TRACE:
            continue
        why = getattr(entry[2], "no_trace_reason", "")
        assert len(why) >= 40, (
            "صمتٌ بلا تعليلٍ كافٍ في %s: %r" % (entry[0], why))


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_derived_residue_is_not_empty(harness):
    assert harness._derive_residue(), "اشتقاقٌ فارغٌ = حارسُ تلوّثٍ لا يمكنُ أن يعضّ"


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_clean_tree_is_not_rejected(harness):
    # (AR) الرفضُ الكاذبُ أسوأُ من الانهيارِ الذي حلَّ محلَّه.
    assert harness._residue() == [], "رفضٌ كاذبٌ على شجرةٍ نظيفة"


# ═══ ② سِمةُ الأثرِ دليلٌ لا لفظٌ عابر ═══════════════════════════════════════
@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_residue_marks_are_evidence(harness):
    sentinels = (harness._CREATED, harness._SELF_RED, harness._NO_TRACE)
    for name, rel, mark in harness._derive_residue():
        if mark in sentinels:     # الدليلُ وجودُ ملفٍّ أو حمرةُ بوّابةٍ لا سِمة
            continue
        assert len(mark) >= harness.MIN_RESIDUE_MARK, (
            "سِمةُ أثرٍ أقصرُ من أن تكونَ دليلًا في %s (%s): %r" % (rel, name, mark))


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_append_refuses_a_blind_mark(harness):
    with pytest.raises(AssertionError):
        harness._append(b"\n// x\n")


# ═══ ③ البصمةُ لا تتغيّرُ بنهاياتِ الأسطر ══════════════════════════════════
# (AR) `.gitattributes` يفرضُ `*.py text eol=lf`، فما يُخرِجُه `git checkout`
#      بـLF مهما كانت نهاياتُ الأسطرِ على قرصِ الكاتب. وبصمةٌ على البايتاتِ
#      الخامِّ تُحمِّرُ كلَّ استنساخٍ نظيفٍ بحمرةٍ لا علاقةَ لها بالمحتوى.
@pytest.mark.parametrize("sha", tuple(m._sha_bytes for m in HARNESSES),
                         # (AR) والأسماءُ تُشتقُّ كالقائمة: صفٌّ يدويٌّ بثلاثةِ
                         #      أسماءٍ أوقفَ **جمعَ الملفِّ كلِّه** حينَ صارت
                         #      المِحقناتُ أربعًا (قِيسَ: `Interrupted`).
                         ids=lambda m: m.__name__)
def test_fingerprint_is_eol_invariant(sha):
    body = ("# -*- coding: utf-8 -*-" + chr(10) + "x = 1" + chr(10)).encode("utf-8")
    assert sha(body) == sha(body.replace(harness.LF_, harness.CRLF))


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_both_sides_share_one_convention(harness, tmp_path):
    # (AR) اتّفاقُ القراءةِ يجبُ أن يكونَ واحدًا في الطرفَين، وإلّا فالمقارنةُ
    #      بلا معنًى. والملفّانِ منفصلانِ عمدًا (الفوقيُّ لا يستوردُ مِحقنةً
    #      بعينِها)، فاتّفاقُهما **يُقاسُ** ولا يُوعَدُ به في تعليق.
    probe = tmp_path / "probe.py"
    probe.write_bytes(("a = 1" + chr(10) + "b = 2" + chr(10)).encode("utf-8"))
    lf_digest = (harness._sha_norm(probe), meta._sha_norm(probe))
    probe.write_bytes(probe.read_bytes().replace(harness.LF_, harness.CRLF))
    crlf_digest = (harness._sha_norm(probe), meta._sha_norm(probe))
    assert lf_digest[0] == lf_digest[1], "الطرفانِ لا يتّفقانِ على البصمة"
    assert lf_digest == crlf_digest, "البصمةُ تتغيّرُ بنهاياتِ الأسطر"


# ═══ ④ أرضيّةُ العمقِ في الطرفَين لا تتباعد ═════════════════════════════════
@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
def test_depth_floor_agrees_across_both_sides(harness):
    # (AR) الازدواجُ مقصود: لو استوردَ الفوقيُّ من المِحقنةِ لخفَّضَ تقليصُها
    #      أرضيّتَه ذاتيًّا — «حارسٌ طرفاه من أصلٍ واحد». فيُقاسُ التطابقُ ههنا.
    # (AR) الأرضيّةُ الجامعةُ في الفوقيِّ حدٌّ **أدنى** لكلِّ مِحقنة، لا مساواةٌ:
    #      مِحقنةٌ أعمقُ لا تُخالِفُه، ومِحقنةٌ أضحلُ منه تُرَدُّ.
    assert harness.MIN_PROBES >= meta.CEILING_MIN_PROBES
    assert len(harness.PROBES) >= harness.MIN_PROBES, (
        "مِحقنةٌ قُلِّصت تحتَ أرضيّتِها")


# ═══ ⑥ عيارُ البراهين — ثلاثُ رقعاتٍ كانت بلا حارس ═════════════════════════
# (AR) `calibrate_seed_proofs.py` لا مِحقنةَ له (يُشغِّلُ العدّاءَ فهو أبطأُ من
#      بوّابة)، فلامتغيِّراتُه البنيويّةُ تُقاسُ ههنا — وإلّا فأيُّ ردٍّ لرقعةٍ
#      منها يمرُّ صامتًا. وثلاثتُها بُرهِنَ أثرُها بالقياس.
import calibrate_seed_proofs as proofs  # noqa: E402


def test_pool_is_sorted_by_posix_path():
    """(AR) العيّنةُ حتميّةٌ **عبرَ المنصّات**: فرزُ كائنِ `Path` يختلفُ بين
    ويندوز (بلا حساسيّةِ حالة) وPOSIX — قِيسَ اختلافُ خمسةِ مواضعَ في عيّنةِ ١٤٩."""
    pool = proofs._candidates()
    assert [p.as_posix() for p in pool] == sorted(p.as_posix() for p in pool)


def test_sample_is_deterministic_and_spread():
    """(AR) خطوةٌ ثابتةٌ على ترتيبٍ مفروز — لا عشوائيّةَ بلا بذرةٍ مثبَّتة."""
    rows = list(range(100))
    assert proofs._sample(rows, 4) == [0, 25, 50, 75]
    assert proofs._sample(rows, 4) == proofs._sample(rows, 4)
    assert proofs._sample(rows, 200) == rows


def test_clean_tree_has_no_inflight_journal():
    """(AR) الرفضُ الكاذبُ أسوأُ من الانهيارِ: شجرةٌ نظيفةٌ لا تُوقِفُ القياس.

    🔑 و**بالمادّةِ**: `_residue()` بلا مادّةٍ يمسُّ السجلَّ وحدَه ولا يمسُّ
    ماسحَ السِّمة — وهو الكاشفُ الوحيدُ الذي ينجو من محوِ السجلّ. والمادّةُ
    مدفوعةُ الثمنِ سلفًا في أختِه، فالتمريرُ ههنا مجّانيّ.
    """
    assert proofs._residue(proofs._candidates()) == []


def test_gated_seeds_are_out_of_the_pool():
    """(AR) بذرةٌ يُبوِّبُها العدّاءُ تخرجُ بصفرٍ بلا محاكمة — طفرةٌ مكافئةٌ
    مضمونةٌ لا محتملة، فوجودُها في المادّةِ يجعلُ «البرهانَ» يتّهمُ بريئة."""
    gated = [p for p in proofs._candidates()
             if proofs._GATED.search(p.read_text(encoding="utf-8-sig"))]
    assert gated == [], "بذرةٌ مُبوَّبةٌ في مادّةِ العيار: %s" % gated[:3]


# (AR) 🔑 **الهويّةُ لا تُميِّزُ التوريثَ من النسخ.** `re.compile` يُخبِّئُ،
#      فنسخةٌ **بايتيّةٌ حرفيّةٌ** من سطرِ الحارسِ تُعطي الكائنَ نفسَه ويمرُّ
#      `is` صادقًا. وبُرهنَ بالحقن: استُبدِلَ الاستيرادُ بنسخةٍ حرفيّةٍ فمرَّ
#      الاختبارُ أخضر.
#
#      ⚠️ **ثمّ بُرهِنَ أنّ نفيَ `re.compile` هجاءٌ لا حقيقة.** كان التوكيدُ
#      يرفضُ صورةَ كتابةٍ واحدةً (`اسم = re.compile(...)`) فيفلتُ منه سبعٌ من
#      إحدى عشرةَ صورة. وقِيسَ نهايةً إلى نهاية: `_T = re.compile(...)` ثمّ
#      `SKIP_MARK = _T` في `measure_seed_contract_gap.py` ⇒ الاختباراتُ
#      **خضراءُ كلُّها**، والعدَّادُ الواحدُ برقمَين متناقضَين (أ ٥٤ ≠ ٧٧ ·
#      هـ ١٨٦ ≠ ١٦٥) — وهو حرفيًّا العطبُ (١٦٤ · ١٦٥ · ١٦٩) الذي وُجِدَ هذا
#      الاختبارُ لمنعِه.
#
#      ⚠️⚠️ **ثمّ بُرهِنَ أنّ قلبَ التوكيدِ لم يُخرِجْه من الهجاء.** انتقلت
#      المرساةُ من «صورةِ كتابةٍ واحدةٍ» إلى **قائمةِ أسماءٍ مكتوبةٍ باليد**،
#      وهي غيرُ محصاةٍ كسابقتها. وقِيسَ: قارئٌ ثانٍ **باسمٍ جديدٍ**
#      (`_TAG_READER`) في `measure_seed_contract_gap.py` — والاسمُ الموروثُ
#      باقٍ صحيحًا فيمرُّ فحصُ الهويّة — أعطى `142 passed` ورمزَ ٠ للبوّابة،
#      والحارسَ ٧٧ · ١٦٥ والمقياسَ **١٦ · ١٧٠**. العطبُ نفسُه، عبرَ الرقعةِ
#      التي جاءت لسدِّه. ومعها **رفضانِ كاذبان**: مولِّدٌ محضٌ صارَ «مستهلكًا»
#      لأنّ تعليقًا فيه يذكرُ اسمَ الحارس، ومتغيِّرٌ محلّيٌّ مشروعٌ فيه يُرفَض.
#
#      🔑 فالمرساةُ الآنَ **هجاءُ الوسمِ داخلَ نمطٍ مُصرَّف** — لا اسمُ
#      المتغيِّرِ ولا صورةُ الكتابة. ومفرداتُ الوسومِ **محدودةٌ ومشتقّةٌ من
#      الحارسِ نفسِه**، بخلافِ الأسماءِ التي لا تُحصى. والمستهلكُ **مَن يستوردُ
#      الحارسَ** (نحويًّا)، لا مَن يذكرُ اسمَه في تعليق.
CONTRACT_GUARD = "check_seed_contract"


def _guard_tag_vocabulary():
    """(AR) مفرداتُ الوسمِ من أنماطِ الحارسِ لا من هجاءٍ يدويٍّ ههنا."""
    import ast as _ast
    import re as _re

    source = (CODEGEN / (CONTRACT_GUARD + ".py")).read_text(encoding="utf-8")
    tree = _ast.parse(source)
    tags = set()
    for node in _ast.walk(tree):
        for text in _pattern_literals(node, _ast):
            tags.update(_re.findall(r"@[a-z_]+", text))
    # (AR) 🔑 **وثوابتُ الهجاءِ أيضًا، لا الأنماطُ المُصرَّفةُ وحدَها.** حينَ
    #      وُحِّدَ هجاءُ الوسمِ السالبِ في ثابتٍ نصّيٍّ (`NEGATIVE_TAG`)
    #      واستُورِدَ، اختفى `@expect` من المفرداتِ فورًا — فالنمطُ صارَ
    #      `re.compile(NEGATIVE_TAG, ...)` بلا سلسلةٍ حرفيّة. أي أنّ **إصلاحَ
    #      النسخِ كان يُعمي المرساةَ عن الوسمِ الذي وحّدَه**.
    for node in tree.body:
        if isinstance(node, (_ast.Assign, _ast.AnnAssign)):
            value = node.value
            if isinstance(value, _ast.Constant) and isinstance(value.value, str):
                tags.update(_re.findall(r"@[a-z_]+", value.value))
    assert tags, "لا وسمَ في أنماطِ الحارس — أُعيدَت صياغتُه فبطلَ المقياس"
    return tuple(sorted(tags))


# (AR) 🔑 **وأيُّ نداءِ `re.*` لا `compile` وحدَه.** كان المقيسُ `compile`
#      فقط، فقارئٌ ثانٍ بـ`re.search(r"^#[ \t]*@skip_compiler", text)` مرَّ
#      أخضرَ: `140 passed` ورمزُ صفرٍ للبوّابة، والحارسُ ٧٧ · ١٦٥ والمقياسُ
#      **١٦ · ١٦٩**. العطبُ (١٦٤ · ١٦٥ · ١٦٩) نفسُه، للمرّةِ الثالثة.
#      ⚠️ **ويُشترَطُ `Attribute` من `re`**: `compile()` المدمجةُ في بايثون
#         نداءٌ مشروعٌ تمامًا (`compile(src, "<x>", "exec")`)، وكان يُرفَضُ لو
#         حملَ مصدرُه وسمًا — رفضٌ كاذبٌ كامن.
RE_CALLS = ("compile", "search", "match", "fullmatch", "findall", "finditer",
            "sub", "subn", "split")


def _pattern_literals(node, ast_mod):
    """(AR) سلاسلُ النمطِ في نداءِ `re.*` — ولو رُكِّبَ النمطُ بالجمعِ أو
    مُرِّرَ وسيطًا مُسمًّى."""
    call = node
    if not isinstance(call, ast_mod.Call):
        return []
    func = call.func
    if not isinstance(func, ast_mod.Attribute) or func.attr not in RE_CALLS:
        return []
    source = call.args[0] if call.args else None
    for keyword in call.keywords:
        if keyword.arg == "pattern":
            source = keyword.value
    if source is None:
        return []
    return [child.value for child in ast_mod.walk(source)
            if isinstance(child, ast_mod.Constant) and isinstance(child.value, str)]


TAG_VOCABULARY = _guard_tag_vocabulary()


# (AR) 🔑 **وقائمةُ المستهلكينَ تُشتقُّ ولا تُكتَب.** كانت صفًّا حرفيًّا
#      بملفَّين — قائمةَ إذنٍ تبلى في اتّجاهٍ واحد: مستهلكٌ ثالثٌ يصلُ **بلا
#      قياس**، والاختبارُ يبقى أخضرَ لأنّه لا يعرفُ بوجودِه.
#      ⚠️ **ثمّ قِيسَ أنّ «ذِكرَ الاسمِ في النصّ» معيارٌ يُرقّي البريءَ متّهمًا**:
#         تعليقٌ واحدٌ يذكرُ اسمَ الحارسِ رقّى مولِّدًا محضًا (`gen_rules_matrix`)
#         إلى «مستهلك»، فصارَ متغيِّرٌ محلّيٌّ مشروعٌ فيه يُرفَضُ رفضًا كاذبًا.
#         والمعيارُ الآنَ **نحويٌّ**: مَن يستوردُ الحارسَ فعلًا. و`level == 0`
#         مقروءٌ صراحةً وإلّا مرَّ `from .check_seed_contract import …`.
def _reader_consumers():
    import ast as _ast

    consumers = []
    for path in sorted(CODEGEN.glob("*.py")):
        if path.name in (CONTRACT_GUARD + ".py", Path(__file__).name):
            continue
        tree = _ast.parse(path.read_text(encoding="utf-8"))
        # (AR) و`rsplit(".")[-1]` كان يقبلُ **أيَّ** حزمةٍ تنتهي بهذا الاسمِ
        #      (`vendor.pkg.check_seed_contract`) — قبولٌ كاذب. والوحدةُ
        #      بعينِها. و`import اسم` المجرَّدُ استهلاكٌ كذلك: كان يفلت.
        if any((isinstance(node, _ast.ImportFrom) and node.level == 0
                and node.module == CONTRACT_GUARD)
               or (isinstance(node, _ast.Import)
                   and any(a.name == CONTRACT_GUARD for a in node.names))
               for node in _ast.walk(tree)):
            consumers.append(path.name)
    assert consumers, (
        "لا مستهلكَ لقارئِ العقد — أُعيدَت تسميةُ الحارسِ أو زالَ، "
        "والاختبارُ صارَ يحرسُ العدم")
    return tuple(consumers)


READER_CONSUMERS = _reader_consumers()


@pytest.mark.parametrize("consumer", READER_CONSUMERS)
def test_no_consumer_compiles_its_own_tag_reader(consumer):
    """(AR) قارئُ الوسمِ **واحدٌ** لا نسخ: ثلاثةُ قرّاءٍ أنتجوا ثلاثةَ أرقامٍ
    متناقضةٍ لعدَّادٍ واحد (١٦٤ · ١٦٥ · ١٦٩)، ورابعٌ بـ`\\s` جعلَ الحارسَ ٧٧
    والمقياسَ ٧٨، وخامسٌ بإسنادٍ غيرِ مباشرٍ جعلَ أ ٥٤، وسادسٌ **باسمٍ جديدٍ**
    جعلَ أ ١٦ وهـ ١٧٠ والشوطَ كلَّه أخضر."""
    import ast as _ast

    tree = _ast.parse((CODEGEN / consumer).read_text(encoding="utf-8"))
    strayed = []
    for node in _ast.walk(tree):
        for text in _pattern_literals(node, _ast):
            hit = [tag for tag in TAG_VOCABULARY if tag in text]
            if hit:
                strayed.append("%s:%d %s ⇐ %r" % (consumer, node.lineno,
                                                  "·".join(hit), text[:60]))
    assert not strayed, (
        "نمطٌ مُصرَّفٌ يقرأُ وسمَ عقدٍ في ملفٍّ يستوردُ الحارس — والعدَّادُ "
        "الواحدُ لا يُقرَأُ بقارئَين: %s" % strayed)


def test_the_tag_rule_can_actually_redden():
    """(AR) 🔑 **والتوكيدُ يُصدَّقُ بإعادةِ عطبِه.** سابقاه كانا أخضرَينِ وهما
    عمياوان، فلا يُصدَّقُ توكيدٌ لم تُرَ حمرتُه — ولا يُصدَّقُ ما لم يُقَسْ
    أنّه لا يرفضُ البريء."""
    import ast as _ast

    assert "@expected" in TAG_VOCABULARY and "@skip_compiler" in TAG_VOCABULARY, (
        "مفرداتُ الوسمِ لم تُشتَقَّ من الحارس: %r" % (TAG_VOCABULARY,))

    def strays(source):
        tree = _ast.parse(source)
        return [text for node in _ast.walk(tree)
                for text in _pattern_literals(node, _ast)
                if any(tag in text for tag in TAG_VOCABULARY)]

    # (AR) والصورُ الستُّ الأخيرةُ **أفلتَت فعلًا** من نسخةٍ سابقةٍ من هذه
    #      القاعدة، فبقيَت شاهدةً عليها لا تنظيرًا.
    for source in ('_T = re.compile(r"^#[ \\t]*@skip_compiler")',
                   '_T = re.compile("^#" + r"[ \\t]*@expected")',
                   'X = regex.compile(r"@expect_error")',
                   'def f():\n    return re.compile(r"@expected:?\\s+(.+)")',
                   'x = re.search(r"^#[ \\t]*@skip_compiler", text)',
                   'x = re.match(r"@expected", text)',
                   'x = re.findall(r"@expect_error", text)',
                   'x = re.sub(r"@expected", "", text)',
                   'x = re.split(r"@skip_compiler", text)',
                   '_T = re.compile(pattern=r"@expected")'):
        assert strays(source), "نمطٌ يفلتُ من مرساةِ الوسم: %r" % source

    for source in ('_T = re.compile(r"^[0-9]+$")',
                   'lines.append(f"# @expected: {exp}")',
                   '_SKIP = "# @skip_compiler"',
                   'code = compile("# @expected 1", "<x>", "exec")',
                   'text = "@expected".join(parts)'):
        assert not strays(source), "رفضٌ كاذبٌ على صورةٍ مشروعة: %r" % source


@pytest.mark.parametrize("consumer", READER_CONSUMERS)
def test_inherited_readers_are_the_guard_objects(consumer):
    """(AR) والهويّةُ تُقاسُ أيضًا — شرطًا ثانيًا لا وحيدًا، **ولكلِّ مستهلك**:
    كان الفحصُ يقتصرُ على واحدٍ فيمرُّ انحرافُ الآخرِ صامتًا. 🔑 وأزواجُ
    الأسماءِ **تُشتقُّ من جملةِ الاستيرادِ نفسِها** ولا تُكتَبُ صفًّا: صفٌّ
    بملفَّين قائمةُ إذنٍ تبلى في اتّجاهٍ واحد — مستهلكٌ ثالثٌ يصلُ فتقيسُه
    مرساةُ الوسمِ ولا يقيسُه هذا."""
    import ast as _ast
    import importlib

    import check_seed_contract as contract_guard

    tree = _ast.parse((CODEGEN / consumer).read_text(encoding="utf-8"))
    pairs = {}
    for node in _ast.walk(tree):
        if isinstance(node, _ast.ImportFrom) and node.level == 0 \
                and node.module == CONTRACT_GUARD:
            for alias in node.names:
                pairs[alias.name] = alias.asname or alias.name
    if not pairs:
        # (AR) ومستهلكٌ بـ`import اسم` المجرَّدِ لا يُسمّي شيئًا فلا اسمَ
        #      يُقارَن — والوصولُ عبرَ الوحدةِ نفسِها هويّةٌ **بالبناء**.
        #      ويُشترَطُ أن يكونَ كذلك فعلًا لا أن يكونَ بلا استيرادٍ أصلًا.
        assert any(isinstance(node, _ast.Import)
                   and any(a.name == CONTRACT_GUARD for a in node.names)
                   for node in _ast.walk(tree)), (
            "مستهلكٌ بلا استيرادٍ يُقاسُ عليه: %s" % consumer)
        return

    module = importlib.import_module(consumer[:-len(".py")])
    for origin, alias in sorted(pairs.items()):
        assert getattr(module, alias) is getattr(contract_guard, origin), (
            "%s.%s ليس كائنَ %s.%s" % (consumer, alias, CONTRACT_GUARD, origin))


def test_the_two_counters_agree():
    """(AR) 🔑 **والحاصلُ يُقاسُ، لا الهجاءُ وحدَه.** ثلاثُ قواعدَ متتاليةٍ
    حرسَت *صورةَ كتابةِ* القارئِ الثاني، والتُفَّ على ثلاثتِها: صورةٌ أخرى ·
    اسمٌ آخر · دالّةٌ أخرى من `re`. وهذا التوكيدُ يقيسُ **تطابقَ العدَّادَين
    نفسَيهما** — فلا يُفلَتُ منه بهجاء، مهما كُتِبَ القارئُ الثاني."""
    import check_seed_contract as contract_guard
    import measure_seed_contract_gap as gap

    rows = contract_guard._seeds()
    skipped, no_contract, total = gap.classify()

    assert total == len(rows), (
        "مادّةُ القياسِ نفسُها مختلفة: الحارسُ %d · المقياسُ %d"
        % (len(rows), total))
    assert sum(skipped.values()) == sum(1 for r in rows if r["skip"]), (
        "العدّادُ «أ» برقمَين: الحارسُ %d · المقياسُ %d"
        % (sum(1 for r in rows if r["skip"]), sum(skipped.values())))
    assert sum(no_contract.values()) == sum(
        1 for r in rows
        if not r["skip"] and not r["expected"] and not r["negative"]), (
        "العدّادُ «هـ» برقمَين: الحارسُ %d · المقياسُ %d"
        % (sum(1 for r in rows
               if not r["skip"] and not r["expected"] and not r["negative"]),
           sum(no_contract.values())))


# ═══ ⑤ التاريخُ تاريخٌ لا شكلٌ يُشبِهُه ════════════════════════════════════
@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
@pytest.mark.parametrize("stamp", ["2026-09-05", "1999-01-01"])
def test_real_dates_accepted(harness, stamp):
    assert harness._is_date(stamp)


@pytest.mark.parametrize("harness", HARNESSES, ids=lambda m: m.__name__)
@pytest.mark.parametrize("stamp", [
    "٢٠٢٦-٠٩-٠٥",   # أرقامٌ عربيّةٌ هنديّة
    "۲۰۲۶-۰۹-۰۵",   # أرقامٌ فارسيّة
    "9999-99-99", "0000-00-00", "2026-9-5", "not-a-date", "",
])
def test_lookalikes_rejected(harness, stamp):
    assert not harness._is_date(stamp)
