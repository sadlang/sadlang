#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ عقدِ البذرة — يُحقَنُ العطبُ ويُثبَتُ الاحمرار.

     حارسٌ لم يُثبَتْ احمرارُه ليس حارسًا. ومجسّاتٌ عدّتُها `len(PROBES)`
     وأرضيّتُها `MIN_PROBES` — ولا تُنثَرُ ههنا عددًا يبلى. وأربعةٌ منها **يجبُ
     أن تبقى خضراءَ** فهي اختباراتُ انحدارٍ لا أعطاب.

     🔑 والبايتاتُ تُستعادُ ويُتحقَّقُ منها بـsha256، وتُرفَضُ الأرضيّةُ الملوّثةُ
        بأثرِ تشغيلةٍ لم تُنهَ — وأثرُها **يُشتقُّ من المجسِّ نفسِه**.

(EN) Injection harness for the seed-contract guard.
============================================================================
"""
from __future__ import annotations

import hashlib
import json
import os
import re
import subprocess
import sys
from datetime import date
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_seed_contract.py"
HARNESS = Path(__file__).resolve()
RECORD_DIR = ROOT / "scripts" / "codegen" / "calibration"
RECORD = RECORD_DIR / "check_seed_contract.yaml"

# (AR) ثابتانِ بدلَ محرَفَي هروب: تُنقَلُ الشفرةُ عبرَ صدفاتٍ تبتلعُ الخطَّ المائل.
CRLF = (chr(13) + chr(10)).encode("ascii")
LF_ = chr(10).encode("ascii")


# ═══ البصمةُ تُقاسُ على التمثيلِ الذي يملكُه git ═══════════════════════════
# (AR) `.gitattributes` يُعلِنُ `*.py text eol=lf`، فبصمةٌ على البايتاتِ الخامِّ
#      تُحمِّرُ كلَّ استنساخٍ نظيفٍ بحمرةٍ لا علاقةَ لها بالمحتوى.
def _sha_bytes(blob: bytes) -> str:
    return hashlib.sha256((blob or b"").replace(CRLF, LF_)).hexdigest()


def _sha_norm(path) -> str:
    return _sha_bytes(path.read_bytes())


def _is_date(stamp: str) -> bool:
    """(AR) تاريخٌ فعليٌّ لا شكلٌ يُشبِهُه — و`\\d` في بايثون يوني‌كوديّ."""
    if not stamp.isascii():
        return False
    try:
        date.fromisoformat(stamp)
    except ValueError:
        return False
    return True


def _run_guard() -> tuple[int, str]:
    env = dict(os.environ, PYTHONIOENCODING="utf-8", PYTHONUTF8="1")
    proc = subprocess.run([sys.executable, str(GUARD)], capture_output=True,
                          text=True, encoding="utf-8", cwd=str(ROOT), env=env)
    return proc.returncode, (proc.stdout or "") + (proc.stderr or "")


def _run_eol_invariance() -> tuple[int, str]:
    """(AR) يمرُّ بـ`_sha_norm` **كاتبةِ السجلِّ بعينِها** لا بطبقةٍ تحتَها."""
    blob = GUARD.read_bytes()
    as_lf = _sha_norm(GUARD)
    GUARD.write_bytes(blob.replace(LF_, CRLF))
    as_crlf = _sha_norm(GUARD)
    GUARD.write_bytes(blob)
    if as_lf == as_crlf:
        return 0, "بصمةٌ ثابتةٌ عبرَ نهاياتِ الأسطر: %s" % as_lf[:16]
    return 1, "انجرافٌ بنهايةِ السطر: LF=%s ≠ CRLF=%s" % (as_lf[:16], as_crlf[:16])


# ═══ الأثرُ يُشتقُّ من المجسِّ ولا يُكتَبُ مرّتَين ══════════════════════════
# (AR) 🔑 **الصمتُ يُصرَّحُ به ولا يُورَثُ بالإغفال.** كان `_bom_expected` بلا
#      `residue` **سهوًا**، ومسارُ هدفِه يظهرُ في الاشتقاقِ لأنّ مجسًّا آخرَ
#      يُسجِّلُ المسارَ نفسَه بسِمةٍ أخرى — فبدا متتبَّعًا وليس كذلك. وقُيسَ:
#      قتلٌ في منتصفِه يتركُ بذرةً **مُودَعةً** مُفسَدةً، والحارسُ أخضرُ والعدّاءُ
#      أخضرُ وفحصُ الأثرِ أخضر. فصارَ لكلِّ مجسٍّ تصريحٌ واجب، والاشتقاقُ
#      **لكلِّ مجسٍّ لا لكلِّ مسار**.
MIN_RESIDUE_MARK = 16
_CREATED = object()      # ملفٌّ يُنشَأ — الدليلُ وجودُه
_SELF_RED = object()     # الطفرةُ تُحمِّرُ الحارسَ بنفسِها — البوّابةُ تلتقطُها
_NO_TRACE = object()     # لا أثرَ دلاليًّا — ويلزمُ تعليلٌ عندَ الإسناد
_DECLARED = (_CREATED, _SELF_RED, _NO_TRACE)


def _sub(old: bytes, new: bytes, count: int = 1, residue: bool = False):
    def apply(blob: bytes) -> bytes:
        if blob is None or blob.count(old) < 1:
            raise AssertionError("المرساةُ غيرُ موجودة: %r" % old[:48])
        return blob.replace(old, new, count)
    apply.residue = new if residue else _SELF_RED
    return apply


def _append(extra: bytes):
    def apply(blob: bytes) -> bytes:
        return (blob or b"") + extra
    mark = extra.strip()
    if len(mark) < MIN_RESIDUE_MARK:
        raise AssertionError("سِمةُ أثرٍ أقصرُ من أن تكونَ دليلًا (%d < %d): %r"
                             % (len(mark), MIN_RESIDUE_MARK, mark))
    apply.residue = mark
    return apply


def _create(body: bytes):
    def apply(_blob: bytes) -> bytes:
        return body
    apply.residue = _CREATED
    return apply


_CONTRACTED = re.compile("بذورٌ ذاتُ عقدٍ\\s+([0-9]+)")
_BASELINE: dict[str, int] = {}


def _measure_baseline() -> None:
    """(AR) المتعاقَدُ المقيسُ **قبلَ أوّلِ حقن** — يُطفَّرُ منه لا من المُعلَن."""
    code, out = _run_guard()
    match = _CONTRACTED.search(out)
    if code != 0 or not match:
        raise AssertionError(
            "تعذَّرَ قياسُ المتعاقَدِ قبلَ الحقن (رمز=%d) — لا عيارَ على مرجعٍ مجهول"
            % code)
    _BASELINE["contracted"] = int(match.group(1))


def _self_red(fn):
    """(AR) طفرةٌ تُحمِّرُ الحارسَ بنفسِها: بقاؤها يُوقِفُ البوّابةَ فورًا."""
    fn.residue = _SELF_RED
    return fn


def _no_trace(reason: str):
    """(AR) صمتٌ **مُعلَّلٌ**: لا أثرَ دلاليًّا. والتعليلُ يُلزِمُ الكاتبَ ببرهانِه."""
    def wrap(fn):
        fn.residue = _NO_TRACE
        fn.no_trace_reason = reason
        return fn
    return wrap


SEED = "tests/behavior/rules_matrix/10_statements/gr.stmt.if/basic/001_true_cmp_gt.ص"
NEW_SEED = "tests/behavior/rules_matrix/10_statements/zz_contract_probe.ص"
# (AR) 🔑 **داخلَ نطاقِ المسحِ لا خارجَه.** كان `tests/_archive/…` — و`SEEDS`
#      هو `tests/behavior`، فالهدفُ لم يكنْ ليُمسَحَ أصلًا: المجسُّ يمرُّ ولو
#      حُذِفَ `SKIP_PARTS` كلُّه (قِيسَ: أخضرُ في الحالَين). ولا مجلَّدَ
#      `_archive` تحتَ `behavior` اليوم، فالبندُ كان ميّتًا وشاهدُه أعمى.
ARCHIVED = "tests/behavior/_archive/zz_contract_probe.ص"
EXPECTED_LINE = "# @expected".encode("utf-8")


@_self_red
def _bump_floor(blob: bytes) -> bytes:
    """(AR) يرفعُ أرضيّةَ المتعاقَدِ بواحدٍ — يُحاكي انكماشَ المجموعةِ بلا مساسٍ
    بالبذور. ويُقرأُ الرقمُ القائمُ ولا يُثبَّتُ، فلا تنكسرُ المرساةُ برفعٍ مشروع."""
    text = blob.decode("utf-8")
    anchor = "FLOOR_CONTRACTED = "
    at = text.index(anchor) + len(anchor)
    end = at
    while end < len(text) and text[end].isdigit():
        end += 1
    # (AR) المقيسُ + ١ لا المُعلَنُ + ١: الأرضيّةُ مصمَّمةٌ للصعود، فبذرةٌ
    #      متعاقَدةٌ جديدةٌ تفتحُ هامشًا (٤٣٠٤ فوقَ أرضيّةِ ٤٣٠٣) يبتلعُ
    #      رفعَ الواحدِ فلا يعضُّ المجسُّ وهو سليم.
    raised = max(int(text[at:end]), _BASELINE.get("contracted", 0)) + 1
    return (text[:at] + str(raised) + text[end:]).encode("utf-8")


_BOM_MARK = b"\xef\xbb\xbf" + "# @expected 1\n".encode("utf-8")


def _bom_expected(blob: bytes) -> bytes:
    """(AR) بادئةُ BOM قبلَ عقدٍ في السطرِ الأوّل — كانت تُبتلَعُ صامتة.

    وأثرُه **بايتاتُ البادئةِ نفسُها**: العدّاءُ لا يرى السطرَ (لا يبدأُ بـ`#`
    بعدَ `strip`) والحارسُ يراهُ عقدًا قائمًا سلفًا فلا يتغيّرُ عدّاد — فلا
    شاهدَ عليه غيرُ هذه السِّمة. وقِيسَ: **صفرُ** بذرةٍ في الشجرةِ تبدأُ بها.
    """
    body = blob[3:] if blob.startswith(b"\xef\xbb\xbf") else blob
    return _BOM_MARK + body


_bom_expected.residue = _BOM_MARK


@_no_trace("تطبيعُ نهاياتِ الأسطرِ لا يُغيِّرُ بصمةً مطبَّعةً ولا دلالةَ بايتٍ"
           " واحدٍ — وهو نصُّ اللامتغيِّرِ الذي يُثبِتُه هذا المجسُّ نفسُه.")
def _eol_to_lf(blob: bytes) -> bytes:
    return (blob or b"").replace(CRLF, LF_)


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    ("① بذرةٌ جديدةٌ بلا عقدٍ ترفعُ الدَّين",
     NEW_SEED, _create("اطبع_سطر(\"لا عقدَ لي\")\n".encode("utf-8")),
     1, "① تُشغَّلُ بلا عقد"),

    ("② بذرةٌ تفقدُ عقدَها",
     SEED, _sub(EXPECTED_LINE, "# (AR) نُزِعَ العقد".encode("utf-8")),
     1, "① تُشغَّلُ بلا عقد"),

    ("③ عقدانِ متناقضان — @expected مع سالب",
     SEED, _append("\n# @expect_error SEM001\n".encode("utf-8")),
     1, "③ عقدانِ متناقضان"),

    # (AR) الهجاءُ الثاني: `compile_` بينَ `expect_` و`error` فلا لاحقةٌ تبلغُه.
    #      ولولا قراءتِه لعُدَّت بذورُه «بلا عقد» — وهو ما وقعَ في المقياسِ القائم.
    ("④ الهجاءُ الثاني يُقرأُ عقدًا سالبًا",
     NEW_SEED, _create("# @expect_compile_error SEM042\nس = ١\n".encode("utf-8")),
     0, "③ عقدانِ متناقضان                      0"),

    ("⑤ تخطٍّ جديدٌ يرفعُ سقفَ المتخطّاة",
     NEW_SEED, _create("# @skip_compiler\n# @expected 1\nاطبع_سطر(\"1\")\n"
                       .encode("utf-8")),
     1, "② متخطّاة"),

    # (AR) وسمٌ سالبٌ بلا رمزٍ عقدٌ ضعيفٌ يمرُّ على أيِّ فشلٍ ولو كان انهيارَ الأداة.
    ("⑥ وسمٌ سالبٌ بلا رمزٍ يرفعُ الدَّينَ الضعيف",
     NEW_SEED, _create("# @expect_error\nس = ".encode("utf-8")),
     1, "④ وسمٌ سالبٌ بلا رمزِ خطأ"),

    # ═══ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    # (AR) 🔑 **ولا عددَ سقفٍ في النصِّ المنتظَر.** كان `"(السقف 165 — نازل)"`،
    #      وتعليقٌ فوقَه يزعمُ النجاةَ من هذا بعينِه. و١٦٥ سقفٌ **نازلٌ بالتعريف**
    #      فأوّلُ سدادِ دَينٍ مشروعٍ يُسقِطُ مجسَّينِ سليمَينِ ⇒ لا يُعادُ سجلُّ
    #      العيار ⇒ الحارسُ الفوقيُّ أحمرُ ⇒ **البوّابةُ تُقفَلُ على العملِ الذي
    #      وُجِدَ الحارسُ ليُنجِزَه**. والمِرساةُ الآنَ سطرُ اللامتغيِّرِ وحدَه.
    # (AR) 🔑 **على بذرةٍ يُنشِئُها المجسُّ لا على بذرةٍ لها عقدٌ سلفًا.**
    #      كان يُلحِقُ عقدًا ببذرةٍ متعاقَدةٍ فلا يتحرّكُ عدَّادٌ عندَ أيِّ قراءة —
    #      **شرطٌ لا يمكنُ أن يكونَ كاذبًا**. وبُرهنَ: ردُّ `utf-8-sig` إلى
    #      `utf-8` في الحارسِ يُبقيه ١٣/١٣. والآنَ العقدُ **وحدَه** خلفَ
    #      البادئة، فقارئٌ لا يقرؤها يعُدُّ البذرةَ «بلا عقد» فيحمرّ.
    ("⑦ بادئةُ BOM لا تبتلعُ العقد",
     NEW_SEED, _create(_BOM_MARK + "اطبع_سطر(\"1\")\n".encode("utf-8")),
     0, "✓ لم ينمُ دَينُ العقد"),

    ("⑧ الأرشيفُ متروكٌ — بذرةٌ بلا عقدٍ فيه لا تُحمِّر",
     ARCHIVED, _create("اطبع_سطر(\"أرشيفٌ بلا عقد\")\n".encode("utf-8")),
     0, "✓ لم ينمُ دَينُ العقد"),

    # (AR) 🔑 `parse_metadata` يقفُ عندَ ثلاثينَ سطرًا، والحارسُ كان يقرأُ الملفَّ
    #      كلَّه — فوسمٌ بعدَها **عقدٌ عندَ الحارسِ وعدمٌ عندَ مَن يُنفِّذُه**،
    #      والبذرةُ تُتخطّى صامتةً وقد عُدَّت متعاقَدة. قِيسَ اليومَ: صفرُ بذرةٍ
    #      كذلك — فالأرقامُ صحيحةٌ والثغرةُ كانت مفتوحةً للقادم.
    ("⑬ وسمٌ خارجَ نافذةِ العدّاءِ ليس عقدًا",
     NEW_SEED, _create(("# حشوٌ\n" * 32 + "# @expected 1\n"
                        + "اطبع_سطر(\"1\")\n").encode("utf-8")),
     1, "① تُشغَّلُ بلا عقد"),

    # (AR) وسمٌ في **جسمِ** الملفِّ ليس عقدًا — ولولا `^#` لكان ذكرُ الوسمِ في
    #      سلسلةٍ يُقرأُ عقدًا فيُخفَّضُ الدَّينُ بغشٍّ نصّيّ.
    # (AR) والنصُّ يحملُ `#` فيقيسَ **المرساةَ `^`** لا اشتراطَ `#`: بلا `#`
    #      كان يمرُّ ولو نُزِعَت المرساةُ من النمط (مقيس).
    ("⑨ نصٌّ في الجسمِ ليس عقدًا",
     NEW_SEED, _create("اطبع_سطر(\"# @expected 1\")\n".encode("utf-8")),
     1, "① تُشغَّلُ بلا عقد"),

    # (AR) 🔑 اللامتغيِّرُ ⑤ (أرضيّةُ المتعاقَد) كان **يُحاكَمُ عليه ولم يُثبَتْ
    #      احمرارُه قطُّ** — وهو نصُّ عقدِ هذه المِحقنةِ نفسِها. ورفعُ الأرضيّةِ
    #      بواحدٍ يُحاكي انكماشَ المجموعةِ المتعاقَدةِ بلا مساسٍ بالبذور.
    ("⑫ انكماشُ المتعاقَدِ يُحمِّر",
     "scripts/codegen/check_seed_contract.py",
     # (AR) والمرساةُ **اسمُ الثابتِ لا قيمتُه**: الأرضيّةُ مصمَّمةٌ للصعود،
     #      فقيمةٌ مثبَّتةٌ تنكسرُ بأوّلِ رفعٍ مشروع. و`residue=False`: سطرُ
     #      الأرضيّةِ ليس سِمةً مميّزةً، ووسمُه أثرًا يُنتِجُ حمرةً كاذبةً عندَ
     #      أوّلِ رفعٍ إلى القيمةِ التي يحقنُها المجسُّ نفسُه.
     _bump_floor,
     1, "انكمشَ المتعاقَد"),

    ("⑩ مسبارٌ أعمى — رمزُ عطبِ آلةٍ 2",
     "scripts/codegen/check_seed_contract.py",
     _sub(b'SKIP_PARTS = ("_archive",)',
          b'SKIP_PARTS = ("_archive", "behavior")', residue=True),
     2, "عيارُ الأداة:"),

    ("⑪ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     "scripts/codegen/check_seed_contract.py",
     _eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`).
MIN_PROBES = 13


def _derive_residue() -> tuple:
    """(AR) صفٌّ **لكلِّ مجسٍّ** لا لكلِّ مسار: مجسّانِ يشتركانِ في ملفٍّ واحدٍ
    بأثرَينِ مختلفَين، فالطيُّ على المسارِ يجعلُ أحدَهما يركبُ تسجيلَ الآخر."""
    out = []
    for entry in PROBES:
        name, path, mutate = entry[0], entry[1], entry[2]
        mark = getattr(mutate, "residue", None)
        if mark is None:
            raise AssertionError(
                "مجسٌّ بلا تصريحِ أثر: %s — يلزمُه سِمةٌ أو %s"
                % (name, "_CREATED/_SELF_RED/_NO_TRACE"))
        out.append((name, path, mark))
    return tuple(out)


def _residue() -> list[str]:
    """(AR) أثرُ تشغيلةٍ سابقةٍ لم تُنهَ. وجودُه ⇒ لا قياسَ، رمزُ ٢."""
    found: list[str] = _inflight()
    for name, rel, mark in _derive_residue():
        path = ROOT / rel
        if mark is _CREATED:
            if path.exists():
                found.append(f"{rel} — ملفُّ مجسٍّ باقٍ ({name})")
        elif mark in (_SELF_RED, _NO_TRACE):
            # (AR) `_SELF_RED` تلتقطُه البوّابةُ نفسُها (والمِحقنةُ تُعيدُ
            #      تشغيلَ الحارسِ بعدَ الاستعادةِ وتُوجِبُ رمزَ صفر)، و`_NO_TRACE`
            #      مُعلَّلٌ عندَ إسنادِه. فليس ههنا ما يُمسَح.
            continue
        elif path.is_file() and mark in path.read_bytes():
            found.append(f"{rel} — أثرُ حقنٍ باقٍ ({name}):"
                         f" {mark.decode('utf-8', 'replace')[:40]!r}")
    return found


# ═══ الحدُّ المشدودُ شرطُ عيارٍ ══════════════════════════════════════════════
# (AR) 🔑 **المجسُّ الذي «يجبُ أن يحمرّ» يحقنُ بندًا واحدًا** فيتجاوزُ السقفَ
#      بواحد — وهذا يعملُ فقط ما دامَ العدَّادُ **على** سقفِه بالضبط. وأوّلُ
#      سدادِ دَينٍ مشروعٍ يفتحُ فجوةً (١٦٤ تحتَ سقفِ ١٦٥) فتسقطُ خمسةُ مجسّاتٍ
#      سليمةٍ دفعةً واحدة ⇒ لا يُودَعُ سجلُّ العيارِ ⇒ الحارسُ الفوقيُّ أحمرُ
#      عندَ أوّلِ لمسةٍ ⇒ **البوّابةُ مقفلةٌ على العملِ الذي وُجِدَ الحارسُ
#      ليُنجِزَه**. ونزعُ العددِ من نصِّ المجسِّ عالجَ العَرَضَ لا السبب.
#      فالفجوةُ تُرفَضُ ههنا صراحةً **بمخرجٍ مسمًّى**: أنزِلِ السقفَ إلى المقيسِ
#      (أو ارفعِ الأرضيّة) ثمّ أعِدِ العيار — وهو السلوكُ الذي يفرضُه تصميمُ
#      «سقفٌ نازلٌ لا يُرفَع» أصلًا. ولا كلفةَ اليوم: الحدودُ الخمسةُ والعشرونَ
#      في الحرّاسِ الثلاثةِ **مشدودةٌ كلُّها** (مقيس).
# (AR) 🔑 **السقوفُ وحدَها.** الأرضيّةُ **مصمَّمةٌ للصعود**، فمطالبتُها
#      بالمساواةِ تُناقِضُ دلالتَها وتجعلُ **إضافةَ بذرةٍ واحدةٍ بعقد** — أعدى
#      عملٍ مشروعٍ في هذا المستودع — تُوقِفُ العيارَ برمزِ ٢؛ وقِيسَ فعلًا:
#      «مقيسٌ 4304 ≠ مُعلَنٌ 4303». والمجسُّ الذي يُحمِّرُ الأرضيّةَ يُطفِّرُها
#      من **المقيسِ + ١** فيعضُّ على أيِّ هامش، فلا حاجةَ إلى شدِّها.
_BOUND = re.compile(r"(\d+)\s*\((?:السقف|المسموح)\s*(\d+)")


def _slack_bounds() -> list[str]:
    """(AR) حدودٌ فيها فجوةٌ بينَ المقيسِ والمُعلَن — تمنعُ العيارَ ولا تُخفِقُه."""
    code, out = _run_guard()
    if code != 0:
        return ["الحارسُ ليس أخضرَ قبلَ العيار (رمز=%d)" % code]
    lines = [L for L in out.split(chr(10)) if _BOUND.search(L)]
    if not lines:
        return ["لم يُقرأْ سقفٌ واحدٌ من مخرَجِ الحارس — قارئُ السقوفِ أعمى"]
    slack = []
    for line in lines:
        for measured, bound in _BOUND.findall(line):
            if measured != bound:
                # (AR) ويُسمّى **السطرُ**: في حارسِ المرساةِ ستّةَ عشرَ حدًّا،
                #      فزوجُ أرقامٍ بلا اسمٍ لا يدلُّ على موضعِه.
                slack.append("%s  (مقيسٌ %s ≠ سقفٌ %s)"
                             % (line.strip(), measured, bound))
    return slack


# ═══ سجلُّ الطيرانِ — شبكةٌ تعمُّ المجسّاتِ كلَّها ══════════════════════════
# (AR) 🔑 **`_SELF_RED` كان دعوى تُكذَبُ في حالةٍ خضراءَ واقعيّة.** معناه «الطفرةُ
#      تُحمِّرُ الحارسَ فالبوّابةُ تلتقطُها»، ولذا كان `_residue()` يتخطّاها. لكنّ
#      حمرتَها مشروطةٌ بأن يكونَ العدَّادُ على سقفِه: على أرضيّةٍ فيها فجوةٌ
#      **لا تُحمِّرُ**، فقتلٌ قاسٍ في منتصفِ المجسِّ يتركُ الطفرةَ مُودَعةً
#      والحارسَ أخضرَ وكاشفَ الأثرِ يقولُ «نظيفة» (بُرهنَ بالحقن).
#      والسِّمةُ لا تصلحُ بديلًا لكلِّ مجسّ: نصُّ الاستبدالِ قد يكونُ لفظًا
#      مشروعًا يتكرّرُ مئاتِ المرّاتِ في الشجرة.
#      فالسجلُّ **يُسمّي البذرةَ وبصمتَها قبلَ الطفرةِ ويُمحى بعدَ استعادةٍ
#      مُتحقَّقٍ منها** — شبكةٌ واحدةٌ لا تعتمدُ على تمييزِ لفظٍ ولا على حمرةٍ
#      مشروطة. وهي النمطُ نفسُه المُودَعُ في `calibrate_seed_proofs.py`.


def _git_dir() -> Path:
    """(AR) في شجرةٍ فرعيّةٍ يكونُ `.git` **ملفًّا**، فيُسأَلُ git ولا يُخمَّن."""
    try:
        proc = subprocess.run(["git", "rev-parse", "--absolute-git-dir"],
                              cwd=str(ROOT), capture_output=True, text=True,
                              encoding="utf-8", timeout=30)
        out = (proc.stdout or "").strip() if proc.returncode == 0 else ""
    except (OSError, subprocess.SubprocessError):
        out = ""
    return Path(out) if out else ROOT / ".git"


JOURNAL = _git_dir() / ("_calibration_inflight_%s.json" % HARNESS.stem)


def _journal_open(rel: str, before: str | None) -> None:
    JOURNAL.parent.mkdir(parents=True, exist_ok=True)
    JOURNAL.write_text(json.dumps({"probe": rel, "sha256": before},
                                  ensure_ascii=False), encoding="utf-8")


def _journal_close() -> None:
    try:
        JOURNAL.unlink(missing_ok=True)
    except OSError:
        pass


def _inflight() -> list[str]:
    """(AR) أثرُ تشغيلةٍ قُتِلَت في منتصفِ مجسّ — تُسمّى البذرةُ بعينِها."""
    if not JOURNAL.is_file():
        return []
    try:
        rec = json.loads(JOURNAL.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return [f"{JOURNAL.name} — سجلُّ طيرانٍ لا يُقرأ"]
    rel, before = rec.get("probe"), rec.get("sha256")
    target = ROOT / str(rel)
    if before is None:
        # (AR) ملفٌّ كان يُنشَأ — بقاؤه أثر، وغيابُه استعادةٌ تمّت.
        if target.exists():
            return [f"{rel} — ملفُّ مجسٍّ باقٍ (سجلُّ طيران)"]
    elif not target.is_file():
        return [f"{rel} — الملفُّ مفقودٌ وسجلُّ الطيرانِ يذكرُه"]
    elif hashlib.sha256(target.read_bytes()).hexdigest() != before:
        return [f"{rel} — بقيَ **مُطفَّرًا**: بصمتُه تُخالِفُ سجلَّ الطيران"]
    _journal_close()          # استُعيدَ فعلًا وبقيَ السجلُّ وحدَه
    return []


def _probe(path: str, mutate, want_code: int, want_text: str,
           runner=None) -> tuple[bool, str]:
    target = ROOT / path
    existed = target.exists()
    original = target.read_bytes() if existed else None
    before = hashlib.sha256(original).hexdigest() if existed else None
    # (AR) والمجلَّدُ المُنشَأُ أثرٌ كالملفّ: هدفُ مجسِّ الأرشيفِ في مجلَّدٍ لا
    #      وجودَ له في الشجرة، فتركُه فارغًا يتركُ بصمةً لا يشتقُّها أحد.
    made_dir = not target.parent.is_dir()
    try:
        target.parent.mkdir(parents=True, exist_ok=True)
        _journal_open(path, before)
        target.write_bytes(mutate(original))
        code, out = (runner or _run_guard)()
    finally:
        # (AR) 🔑 **الإغلاقُ ههنا لا بعدَ الكتلة.** كان بعدَها، فأيُّ استثناءٍ
        #      داخلَ المجسّ (مرساةُ حقنٍ زالت بعملٍ مشروع) يتركُ السجلَّ
        #      مفتوحًا ببصمةِ لحظتِه — والاستعادةُ تمَّت في `finally` — فيصيرُ
        #      كلُّ تشغيلةٍ تاليةٍ **رفضًا كاذبًا على شجرةٍ نظيفةٍ بلا مخرج**.
        #      وقعَ فعلًا وأحمرَ `test_clean_tree_is_not_rejected`.
        if existed:
            target.write_bytes(original)          # type: ignore[arg-type]
            restored = hashlib.sha256(target.read_bytes()).hexdigest() == before
        else:
            target.unlink(missing_ok=True)
            if made_dir and target.parent.is_dir():
                try:
                    target.parent.rmdir()          # يخفقُ إن لم يكنْ فارغًا
                except OSError:
                    pass
            restored = not target.exists()
        if restored:
            _journal_close()
    if existed:
        if hashlib.sha256(target.read_bytes()).hexdigest() != before:
            raise AssertionError("لم تُستعَدِ البايتاتُ في " + path)
    elif target.exists():
        raise AssertionError("لم يُحذَفِ الملفُّ المؤقّت: " + path)
    ok = (code == want_code) and (want_text in out)
    detail = "رمز=%d (منتظَر %d)" % (code, want_code)
    if want_text not in out:
        detail += " · النصُّ المنتظَرُ غائب"
    return ok, detail


def _write_record(passed: int, stamp: str) -> None:
    RECORD_DIR.mkdir(parents=True, exist_ok=True)
    lines = [
        "# ═══════════════════════════════════════════════════════════════════",
        "# سجلُّ عيارٍ — مُشتَقٌّ آليًّا، لا يُحرَّرُ باليد.",
        "# ⚠️ إن تغيّرَ الحارسُ أو المِحقنةُ ولم يُعَدِ العيارُ حمِرَ",
        "#    check_calibration_fresh.py. وإعادتُه:",
        "#    python scripts/codegen/calibrate_seed_contract.py --record --date=YYYY-MM-DD",
        "# ═══════════════════════════════════════════════════════════════════",
        "version: 2",
        "guard: scripts/codegen/check_seed_contract.py",
        "harness: scripts/codegen/calibrate_seed_contract.py",
        "guard_sha256: %s" % _sha_norm(GUARD),
        "harness_sha256: %s" % _sha_norm(HARNESS),
        "calibrated_at: %s" % stamp,
        "probes_total: %d" % len(PROBES),
        "probes_passed: %d" % passed,
        "result: %d/%d" % (passed, len(PROBES)),
        "probes:",
    ]
    for entry in PROBES:
        name, path, _m, code, text = entry[:5]
        role = ("انحدار — يجبُ أن يبقى أخضر" if code == 0
                else "عطبُ آلة" if code == 2 else "يجبُ أن يحمرّ")
        lines.append('  - name: "%s"' % name)
        lines.append("    target: %s" % path)
        lines.append("    expect_exit: %d" % code)
        lines.append('    expect_text: "%s"' % text)
        lines.append('    role: "%s"' % role)
    RECORD.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    record = "--record" in sys.argv
    stamp = ""
    for arg in sys.argv[1:]:
        if arg.startswith("--date="):
            stamp = arg.split("=", 1)[1]
    if record and stamp and not _is_date(stamp):
        print("✗ --date شكلُه YYYY-MM-DD — و%r ليس تاريخًا." % stamp)
        return 2
    if record and not stamp:
        print("✗ --record يلزمُه --date=YYYY-MM-DD (لا يُقرأُ وقتُ النظام)")
        return 2

    print("عيارُ حارسِ «البذرةُ تُحاكَمُ على عقد» بالحقن:")
    print("%-50s %-9s %s" % ("المجسّ", "النتيجة", "التفصيل"))
    print("-" * 88)

    dirty = _residue()
    if dirty:
        print("✗ عطبُ آلة: أرضيّةٌ ملوّثةٌ بأثرِ تشغيلةٍ سابقةٍ لم تُنهَ:")
        for item in dirty:
            print("    · %s" % item)
        # (AR) والمخرجُ يُسمّى: السجلُّ في مجلَّدِ git فلا يبلغُه `git clean -xfd`.
        print("    ⤷ العلاج: استعِدِ الملفَّ المذكورَ من الإيداع، ثمّ احذفْ")
        print("      %s" % JOURNAL)
        # (AR) والمخرجُ يُسمّى: السجلُّ في مجلَّدِ git فلا يبلغُه `git clean -xfd`،
        #      و`git checkout` وحدَه يُتلِفُ تحريرًا مشروعًا ولا يمحوه.
        print("    ⤷ العلاج: استعِدِ الملفَّ المذكورَ من الإيداع، ثمّ احذفْ")
        print("      %s" % JOURNAL)
        return 2

    # (AR) 🔑 **حدٌّ فيه فجوةٌ يُوقِفُ العيارَ ولا يُخفِقُه.** مجسّاتُ «يجبُ أن
    #      تحمرّ» تحقنُ بندًا واحدًا، فحمرتُها مشروطةٌ بأن يكونَ العدَّادُ على
    #      حدِّه. والمخرجُ مُسمًّى: أنزِلِ السقفَ إلى المقيسِ (أو ارفعِ الأرضيّة)
    #      ثمّ أعِدِ العيار — وهو ما يفرضُه تصميمُ «نازلٌ لا يُرفَع» أصلًا.
    _measure_baseline()              # المرجعُ يُقاسُ قبلَ أوّلِ طفرة
    slack = _slack_bounds()          # سقوفٌ فقط — الأرضيّةُ تصعد
    if slack:
        print("✗ عطبُ آلة: سقفٌ فيه فجوةٌ — المجسّاتُ لا تعضُّ على هامش:")
        for item in slack:
            print("    · %s" % item)
        print("    ⤷ العلاج: أنزِلِ السقفَ إلى المقيسِ في الحارسِ ثمّ أعِدِ"
              " العيار — وهو ما يُوجِبُه «نازلٌ لا يُرفَع» أصلًا.")
        return 2

    if len(PROBES) < MIN_PROBES:
        print("✗ عطبُ آلة: %d مجسًّا < %d — مِحقنةٌ قُلِّصت، فلا تُعلِنُ نجاحًا."
              % (len(PROBES), MIN_PROBES))
        return 2

    passed = 0
    for entry in PROBES:
        name, path, mutate, code, text = entry[:5]
        ok, detail = _probe(path, mutate, code, text,
                            entry[5] if len(entry) > 5 else None)
        passed += 1 if ok else 0
        print("%-50s %-9s %s" % (name, "عضَّ" if ok else "لم يعضّ", detail))
    print("-" * 88)
    print("النتيجة: %d/%d" % (passed, len(PROBES)))

    code, out = _run_guard()
    tail = [ln for ln in out.strip().split("\n") if ln.strip()][-1:]
    print("وبعدَ الاستعادة: رمز=%d · %s" % (code, tail[0].strip() if tail else ""))

    if passed != len(PROBES) or code != 0:
        print("✗ لا يُودَعُ سجلُّ عيارٍ ناقص.")
        return 1
    if record:
        _write_record(passed, stamp)
        print("✓ أُودِعَ السجلُّ: %s" % RECORD.relative_to(ROOT).as_posix())
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
