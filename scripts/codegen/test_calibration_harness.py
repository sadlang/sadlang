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
HARNESSES = (harness, anchor_harness, seed_harness)


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
                         ids=("builtin", "anchor", "seed_contract"))
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
#      فالتوكيدُ **مقلوبٌ الآن**: لا يُعدَّدُ الممنوعُ (وهو لا يُحصى)، بل
#      يُشترَطُ المسموح — كلُّ ارتباطٍ لاسمِ قارئٍ يجبُ أن يكونَ `ImportFrom`
#      مصدرُه الحارسُ نفسُه، بأيِّ صورةِ كتابةٍ جاءَ الارتباط.
READER_NAMES = ("_EXPECTED", "_NEGATIVE", "_SKIP", "_NEG_CODE",
                "SKIP_MARK", "EXPECTED_MARK", "NEGATIVE_MARK", "SKIP_PARTS")
CONTRACT_GUARD = "check_seed_contract"


# (AR) 🔑 **وقائمةُ المستهلكينَ تُشتقُّ ولا تُكتَب.** كانت صفًّا حرفيًّا
#      بملفَّين — قائمةَ إذنٍ تبلى في اتّجاهٍ واحد: مستهلكٌ ثالثٌ يصلُ **بلا
#      قياس**، والاختبارُ يبقى أخضرَ لأنّه لا يعرفُ بوجودِه.
def _reader_consumers():
    consumers = []
    for path in sorted(CODEGEN.glob("*.py")):
        if path.name in (CONTRACT_GUARD + ".py", Path(__file__).name):
            continue
        if CONTRACT_GUARD in path.read_text(encoding="utf-8"):
            consumers.append(path.name)
    assert consumers, (
        "لا مستهلكَ لقارئِ العقد — أُعيدَت تسميةُ الحارسِ أو زالَ، "
        "والاختبارُ صارَ يحرسُ العدم")
    return tuple(consumers)


READER_CONSUMERS = _reader_consumers()


def _bound_names(tree, ast_mod):
    """(AR) كلُّ ارتباطٍ لاسمٍ من `READER_NAMES` — بأيِّ صورةِ كتابة."""
    found = []

    def _names(target):
        stack, out = [target], []
        while stack:
            cur = stack.pop()
            if isinstance(cur, (ast_mod.Tuple, ast_mod.List)):
                stack.extend(cur.elts)
            elif isinstance(cur, ast_mod.Starred):
                stack.append(cur.value)
            elif isinstance(cur, ast_mod.Name):
                out.append(cur.id)
        return out

    def _add(lineno, names, kind):
        for name in names:
            if name in READER_NAMES:
                found.append((lineno, name, kind))

    for node in ast_mod.walk(tree):
        if isinstance(node, ast_mod.ImportFrom):
            _add(node.lineno, [a.asname or a.name for a in node.names],
                 "from %s" % (node.module or "?"))
        elif isinstance(node, ast_mod.Import):
            _add(node.lineno, [a.asname or a.name.split(".")[0] for a in node.names],
                 "import")
        elif isinstance(node, ast_mod.Assign):
            for target in node.targets:
                _add(node.lineno, _names(target), "إسناد")
        elif isinstance(node, (ast_mod.AnnAssign, ast_mod.AugAssign)):
            _add(node.lineno, _names(node.target), "إسنادٌ موسومٌ أو مُراكَم")
        elif isinstance(node, ast_mod.NamedExpr):
            _add(node.lineno, _names(node.target), "إسنادٌ سائر")
        elif isinstance(node, (ast_mod.For, ast_mod.AsyncFor)):
            _add(node.lineno, _names(node.target), "متغيِّرُ دَوران")
        elif isinstance(node, ast_mod.With) or isinstance(node, ast_mod.AsyncWith):
            for item in node.items:
                if item.optional_vars is not None:
                    _add(node.lineno, _names(item.optional_vars), "with … as")
        elif isinstance(node, (ast_mod.FunctionDef, ast_mod.AsyncFunctionDef,
                               ast_mod.ClassDef)):
            _add(node.lineno, [node.name], "تعريف")
        # (AR) والارتباطُ الديناميُّ يُقاسُ بدقّةٍ لا بالتخمين: سلسلةٌ تُساوي
        #      اسمَ قارئٍ داخلَ `globals()[…] =` أو `setattr(…)` وحدَهما.
        elif isinstance(node, ast_mod.Call):
            func = node.func
            if isinstance(func, ast_mod.Name) and func.id == "setattr" \
                    and len(node.args) >= 2:
                key = node.args[1]
                if isinstance(key, ast_mod.Constant) and isinstance(key.value, str):
                    _add(node.lineno, [key.value], "setattr")
        elif isinstance(node, ast_mod.Subscript):
            value, key = node.value, node.slice
            dynamic = (isinstance(value, ast_mod.Call)
                       and isinstance(value.func, ast_mod.Name)
                       and value.func.id in ("globals", "vars"))
            if dynamic and isinstance(key, ast_mod.Constant) \
                    and isinstance(key.value, str):
                _add(node.lineno, [key.value], "globals()[…]")

    return found


@pytest.mark.parametrize("consumer", READER_CONSUMERS)
def test_contract_readers_are_inherited_not_rewritten(consumer):
    """(AR) قارئُ الوسمِ **واحدٌ** لا نسخ: ثلاثةُ قرّاءٍ أنتجوا ثلاثةَ أرقامٍ
    متناقضةٍ لعدَّادٍ واحد (١٦٤ · ١٦٥ · ١٦٩)، ونسخةٌ رابعةٌ بـ`\\s` جعلت
    الحارسَ يقولُ ٧٧ والمقياسَ ٧٨، وخامسةٌ بإسنادٍ غيرِ مباشرٍ جعلت أ ٥٤."""
    import ast as _ast

    tree = _ast.parse((CODEGEN / consumer).read_text(encoding="utf-8"))
    strayed = ["%s:%d %s (%s)" % (consumer, lineno, name, kind)
               for lineno, name, kind in _bound_names(tree, _ast)
               if kind != "from " + CONTRACT_GUARD]
    assert not strayed, (
        "ارتباطُ اسمِ قارئٍ من غيرِ توريثِ الحارس — والعدَّادُ الواحدُ لا "
        "يُقرَأُ بقارئَين: %s" % strayed)


def test_the_inheritance_assertion_can_actually_redden():
    """(AR) 🔑 **والتوكيدُ نفسُه يُصدَّقُ بإعادةِ عطبِه.** سابقُه كانَ أخضرَ
    ثلاثَ مرّاتٍ وهو أعمى، فلا يُصدَّقُ توكيدٌ لم يُرَ حمرتُه."""
    import ast as _ast

    for source in ('SKIP_MARK = re.compile("x")',
                   '_T = re.compile("x")\nSKIP_MARK = _T',
                   'SKIP_MARK: object = re.compile("x")',
                   'SKIP_MARK, _NEGATIVE = 1, 2',
                   'globals()["SKIP_MARK"] = 1',
                   'setattr(m, "SKIP_MARK", 1)',
                   'for SKIP_MARK in x: pass',
                   'def SKIP_MARK(): pass'):
        bound = _bound_names(_ast.parse(source), _ast)
        assert any(kind != "from " + CONTRACT_GUARD for _l, _n, kind in bound), (
            "صورةُ كتابةٍ تفلتُ من قارئِ الارتباطات: %r" % source)
    inherited = _bound_names(
        _ast.parse("from %s import _SKIP as SKIP_MARK" % CONTRACT_GUARD), _ast)
    assert inherited and all(kind == "from " + CONTRACT_GUARD
                             for _l, _n, kind in inherited), (
        "التوريثُ الصحيحُ يُرفَض — رفضٌ كاذب")


def test_inherited_readers_are_the_guard_objects():
    """(AR) والهويّةُ تُقاسُ أيضًا — شرطًا ثانيًا لا وحيدًا، **ولكِلا
    المستهلكَين**: كان الفحصُ يقتصرُ على `proofs` فيمرُّ انحرافُ الآخرِ
    صامتًا."""
    import importlib

    import check_seed_contract as contract_guard

    aliases = {
        "calibrate_seed_proofs": {"_EXPECTED": "_EXPECTED", "_SKIP": "_SKIP",
                                  "_NEGATIVE": "_NEGATIVE",
                                  "SKIP_PARTS": "SKIP_PARTS"},
        "measure_seed_contract_gap": {"_EXPECTED": "EXPECTED_MARK",
                                      "_SKIP": "SKIP_MARK",
                                      "_NEGATIVE": "NEGATIVE_MARK",
                                      "SKIP_PARTS": "EXCLUDED_DIRS"},
    }
    for module_name, pairs in aliases.items():
        module = importlib.import_module(module_name)
        for origin, alias in pairs.items():
            assert getattr(module, alias) is getattr(contract_guard, origin), (
                "%s.%s ليس كائنَ %s.%s" % (module_name, alias,
                                           CONTRACT_GUARD, origin))


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
