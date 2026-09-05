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

import calibrate_builtin_coverage as harness  # noqa: E402
import check_calibration_fresh as meta  # noqa: E402


# ═══ ① كلُّ مجسٍّ صامتِ الأثرِ مُتتبَّع ═══════════════════════════════════════
# (AR) الأثرُ الصارخُ يُحمِّرُ الحارسَ بنفسِه فيُكشَف. والصامتُ لا يُكشَفُ إلّا
#      بالاشتقاق. وهذه المجسّاتُ تُعدَّلُ ملفًّا **لا يقرؤه الحارسُ من مصدرِ
#      الحقيقة**، فأثرُها لا يُحمِّرُ شيئًا: شفرةُ المترجّمِ، وملفُّ الاختبارِ
#      المُنشَأ، والحارسُ نفسُه.
SILENT_TARGETS = (
    "compiler/src/frontend/builders/",
    "compiler/tests/",
    "scripts/codegen/check_builtin_engine_coverage.py",
)


def _is_silent(target: str) -> bool:
    return any(target.startswith(prefix) for prefix in SILENT_TARGETS)


def test_every_silent_probe_is_tracked():
    derived = {rel for rel, _mark in harness._derive_residue()}
    missing = sorted(entry[1] for entry in harness.PROBES
                     if _is_silent(entry[1]) and entry[1] not in derived)
    assert not missing, (
        "مجسٌّ يترك أثرًا صامتًا ولا يُشتقُّ في سجلِّ الأثر — تقيسُ المِحقنةُ"
        " فوقَ أثرِ نفسِها: %s" % missing)


def test_derived_residue_is_not_empty():
    assert harness._derive_residue(), "اشتقاقٌ فارغٌ = حارسُ تلوّثٍ لا يمكنُ أن يعضّ"


def test_clean_tree_is_not_rejected():
    # (AR) الرفضُ الكاذبُ أسوأُ من الانهيارِ الذي حلَّ محلَّه.
    assert harness._residue() == [], "رفضٌ كاذبٌ على شجرةٍ نظيفة"


# ═══ ② سِمةُ الأثرِ دليلٌ لا لفظٌ عابر ═══════════════════════════════════════
def test_residue_marks_are_evidence():
    for rel, mark in harness._derive_residue():
        if mark is None:          # ملفٌّ يُنشَأ — وجودُه هو الدليل
            continue
        assert len(mark) >= harness.MIN_RESIDUE_MARK, (
            "سِمةُ أثرٍ أقصرُ من أن تكونَ دليلًا في %s: %r" % (rel, mark))


def test_append_refuses_a_blind_mark():
    with pytest.raises(AssertionError):
        harness._append(b"\n// x\n")


# ═══ ③ البصمةُ لا تتغيّرُ بنهاياتِ الأسطر ══════════════════════════════════
# (AR) `.gitattributes` يفرضُ `*.py text eol=lf`، فما يُخرِجُه `git checkout`
#      بـLF مهما كانت نهاياتُ الأسطرِ على قرصِ الكاتب. وبصمةٌ على البايتاتِ
#      الخامِّ تُحمِّرُ كلَّ استنساخٍ نظيفٍ بحمرةٍ لا علاقةَ لها بالمحتوى.
@pytest.mark.parametrize("sha", (harness._sha_bytes,))
def test_fingerprint_is_eol_invariant(sha):
    body = ("# -*- coding: utf-8 -*-" + chr(10) + "x = 1" + chr(10)).encode("utf-8")
    assert sha(body) == sha(body.replace(harness.LF_, harness.CRLF))


def test_both_sides_share_one_convention(tmp_path):
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
def test_depth_floor_agrees_across_both_sides():
    # (AR) الازدواجُ مقصود: لو استوردَ الفوقيُّ من المِحقنةِ لخفَّضَ تقليصُها
    #      أرضيّتَه ذاتيًّا — «حارسٌ طرفاه من أصلٍ واحد». فيُقاسُ التطابقُ ههنا.
    assert harness.MIN_PROBES == meta.CEILING_MIN_PROBES
    assert len(harness.PROBES) >= harness.MIN_PROBES, (
        "مِحقنةٌ قُلِّصت تحتَ أرضيّتِها")


# ═══ ⑤ التاريخُ تاريخٌ لا شكلٌ يُشبِهُه ════════════════════════════════════
@pytest.mark.parametrize("stamp", ["2026-09-05", "1999-01-01"])
def test_real_dates_accepted(stamp):
    assert harness._is_date(stamp)


@pytest.mark.parametrize("stamp", [
    "٢٠٢٦-٠٩-٠٥",   # أرقامٌ عربيّةٌ هنديّة
    "۲۰۲۶-۰۹-۰۵",   # أرقامٌ فارسيّة
    "9999-99-99", "0000-00-00", "2026-9-5", "not-a-date", "",
])
def test_lookalikes_rejected(stamp):
    assert not harness._is_date(stamp)
