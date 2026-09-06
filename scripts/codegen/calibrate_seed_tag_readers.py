#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ **قرّاءِ وسومِ البذور** — يُحقَنُ الانجرافُ ويُثبَتُ
     الاحمرار.

     الحارسُ يدَّعي أنّ العدّاءَ (`tests/runner.py`) والحارسَ يقولانِ الشيءَ
     نفسَه عن كلِّ بذرة. والدعوى بلا احمرارٍ مُثبَتٍ ليست حراسة.

     🔑 والمجسّاتُ ههنا تحقنُ **انحرافاتٍ وقعَت فعلًا** في هذه الشجرة:
        `utf-8` مقابلَ `utf-8-sig` · نافذةٌ مثبَّتةٌ لا تتبعُ العدّاء · قارئٌ
        مكتوبٌ باليدٍ جديد · هجاءٌ ثانٍ لوسمٍ واحد · وقياسُ السالبِ بحقلٍ واحدٍ
        من ثلاثةٍ (وهو عطبٌ وقعَ في **أداةِ القياسِ نفسِها** ولفّقَ عشرَ حالاتِ
        خلافٍ لا وجودَ لها).

     🔑 **والبايتاتُ تُستعادُ ويُتحقَّقُ منها بـsha256** بعدَ كلِّ حقنة، وسجلُّ
        طيرانٍ يُفتَحُ قبلَ الطفرةِ ويُغلَقُ بعدَ استعادةٍ مُتحقَّقٍ منها.

(EN) Injection harness for the seed-tag reader guard: inject a real drift,
     prove the guard reddens, restore the bytes exactly (sha256-verified).
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
GUARD = ROOT / "scripts" / "codegen" / "check_seed_tag_readers.py"
HARNESS = Path(__file__).resolve()
RECORD_DIR = ROOT / "scripts" / "codegen" / "calibration"
RECORD = RECORD_DIR / "check_seed_tag_readers.yaml"

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


_HANDWRITTEN = re.compile("أنماطٌ مُصرَّفةٌ مكتوبةٌ باليد\\s+([0-9]+)")
_BASELINE: dict[str, int] = {}


def _measure_baseline() -> None:
    """(AR) عددُ القرّاءِ المقيسُ **قبلَ أوّلِ حقن** — يُطفَّرُ منه لا من المُعلَن."""
    code, out = _run_guard()
    match = _HANDWRITTEN.search(out)
    if code != 0 or not match:
        raise AssertionError(
            "تعذَّرَ قياسُ عددِ القرّاءِ قبلَ الحقن (رمز=%d) — لا عيارَ على مرجعٍ مجهول"
            % code)
    _BASELINE["handwritten"] = int(match.group(1))


def _no_trace(reason: str):
    """(AR) صمتٌ **مُعلَّلٌ**: لا أثرَ دلاليًّا. والتعليلُ يُلزِمُ الكاتبَ ببرهانِه."""
    def wrap(fn):
        fn.residue = _NO_TRACE
        fn.no_trace_reason = reason
        return fn
    return wrap


# ═══ أهدافُ الحقن ═══════════════════════════════════════════════════════
GUARD_REL = "scripts/codegen/check_seed_tag_readers.py"
CONTRACT_GUARD_REL = "scripts/codegen/check_seed_contract.py"
# (AR) قارئٌ مؤقّتٌ داخلَ نطاقِ المسحِ — لا خارجَه، وإلّا لم يكنْ ليُعَدَّ أصلًا.
PROBE_READER = "scripts/codegen/zz_tag_reader_probe.py"
NEW_SEED = "tests/behavior/rules_matrix/10_statements/zz_tag_probe.ص"
# (AR) ثوابتُ محرَفٍ بدلَ هروبٍ يبتلعُه نقلُ الشفرةِ عبرَ صدفة.
LF = chr(10)
TAB = "\\t".encode("ascii")


def _eol_to_lf(blob: bytes) -> bytes:
    """(AR) لا يُغيِّرُ المحتوى — يُغيِّرُ تمثيلَ نهاياتِ الأسطرِ وحدَه."""
    return blob.replace(CRLF, LF_)


_eol_to_lf = _no_trace(
    "تطبيعُ نهاياتِ الأسطرِ لا يُبدِّلُ محتوًى، و`.gitattributes` يُعلِنُ "
    "`*.py text eol=lf` فالمُودَعُ بـLF أصلًا — فلا أثرَ دلاليًّا يبقى."
)(_eol_to_lf)
# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
# (AR) 🔑 **النصُّ المنتظَرُ يجبُ ألّا يظهرَ إلّا عندَ الإخفاق.**
#      كان عنوانَ اللامتغيِّرِ نفسِه — وهو يُطبَعُ في **كلِّ** تشغيلةٍ خضراءَ
#      كانت أو حمراء. فبُرهنَ: إعدامُ اللامتغيِّرِ ③ بالكامل (`if False:`)
#      أبقى العيارَ **١٢/١٢ ورمزَ صفر** — أي أنّ العيارَ بارَكَ حارسًا فيه
#      لامتغيِّرٌ ميّت. والمرساةُ الآنَ سطرُ الإخفاقِ (`✗ ①`)، **ويُنفى ما
#      سواه** فيبرهنَ المجسُّ أنّ الحمرةَ من اللامتغيِّرِ الذي يدّعيه.
_ALL = ("✗ ①", "✗ ②", "✗ ③", "✗ ④", "✗ ⑤", "✗ ⑥", "✗ ⑦", "✗ ⑧")


def _only(mark):
    return (mark, tuple(other for other in _ALL if other != mark))


PROBES = (
    # ═══ ① خلافُ الحدّ ═══
    # (AR) 🔑 **وسمٌ مُزاحٌ بفراغ**: العدّاءُ يُطابِقُ بعدَ `line.strip()`
    #      فيراهُ عقدًا، والحارسُ مرساتُه `^#` فلا يراه. وجهٌ ثالثٌ من قارئٍ
    #      واحدٍ (بعدَ النمطِ والنافذة) لا يقيسُه شيءٌ سواه.
    #      ⚠️ وكان ههنا مجسُّ «نزعِ utf-8-sig» فقِيسَ أنّه **لا يمكنُ أن
    #         يحمرّ**: صفرُ بذرةٍ يتغيّرُ جوابُها به اليوم.
    ("① وسمٌ مُزاحٌ: العدّاءُ يراهُ والحارسُ لا",
     NEW_SEED,
     _create(("   # @expected 1" + LF + 'اطبع_سطر("1")' + LF).encode("utf-8")),
     1, _only("✗ ①")),

    # (AR) والمرساةُ على **سطرِ النمطِ** لا على أوّلِ ذكرٍ للوسم: أوّلُ ذكرٍ
    #      في ترويسةِ الملفِّ (تعليقٌ لا يُنفَّذ)، فالطفرةُ كانت تُصيبُ نصًّا
    #      ميّتًا والمجسُّ يمرُّ أخضرَ وهو أعمى (قِيسَ).
    ("② قارئُ التخطّي ينجرفُ عن العدّاء",
     CONTRACT_GUARD_REL,
     _sub(b'@skip_compiler"', b'@skip_compilerZZ"', residue=True),
     1, _only("✗ ①")),

    ("③ قارئُ العقدِ يشترطُ النقطتَين",
     CONTRACT_GUARD_REL,
     _sub(b"@expected:?[ " + TAB + b"]+(?=", b"@expected:[ " + TAB + b"]+(?=",
          residue=True),
     1, _only("✗ ①")),

    # (AR) نافذةُ الحارسِ تُثبَّتُ فلا تتبعُ العدّاء — وهو عينُ الانجرافِ الذي
    #      جعلَ وسمًا في السطرِ ٤٠ عقدًا عندَ الحارسِ وعدمًا عندَ العدّاء.
    ("④ نافذةُ الحارسِ تُثبَّتُ ولا تتبعُ العدّاء",
     CONTRACT_GUARD_REL,
     _sub(b"    return window", b"    return 3  # noqa", residue=True),
     1, _only("✗ ①")),

    # (AR) السالبُ عندَ العدّاءِ **ثلاثةُ حقولٍ** لا واحد. وقياسُه بحقلٍ واحدٍ
    #      لفّقَ عشرَ حالاتِ خلافٍ لا وجودَ لها — فالمجسُّ يحرسُ أداةَ القياس.
    ("⑤ السالبُ يُقاسُ بحقلٍ واحدٍ من ثلاثة",
     GUARD_REL,
     _sub(b"or meta.expect_error_compiled)",
          b"or False)  # zz_probe_single_field", residue=True),
     1, _only("✗ ①")),

    # ═══ ② عددُ الأنماطِ — من طرفَيه ═══
    ("⑥ نمطٌ جديدٌ مكتوبٌ باليدِ يرفعُ السقف",
     PROBE_READER,
     _create(("# -*- coding: utf-8 -*-" + LF + "import re" + LF
              + '_X = re.compile(r"^#\\s*@expected:?\\s+(.+)$")' + LF).encode("utf-8")),
     1, _only("✗ ②")),

    # (AR) 🔑 **والانكماشُ يُحمِّرُ ولا يُقرأُ تقدّمًا**: قِيسَ أنّ توسيعَ قائمةِ
    #      الاستثناءِ بكلمةٍ واحدةٍ يُخفي أربعةَ أنماطٍ والحارسُ أخضر.
    # (AR) ⚠️ ويُسمَحُ لـ⑧ وحدَه أن يُرافِقَ ②: توسيعُ نطاقِ التخطّي يُخفي
    #      **الأنماطَ والقرّاءَ معًا** بالضرورة (في `scripts/ci` كلاهما)،
    #      فحمرةُ الأرضيّتَينِ إنذارانِ صحيحانِ لا تلوُّثُ مجسّ. وما سواهما
    #      منفيٌّ كما في كلِّ مجسّ.
    ("⑦ استثناءٌ أوسعُ يُخفي أنماطًا",
     GUARD_REL,
     _sub(b'ALWAYS_SKIP = (".git"', b'ALWAYS_SKIP = ("ci", ".git"',
          residue=True),
     1, ("✗ ②", tuple(m for m in _ALL if m not in ("✗ ②", "✗ ⑧")))),

    # ═══ ③ خلافُ قرّاءِ @rule ═══
    # (AR) 🔑 وكان هذا اللامتغيِّرُ **ميّتًا**: المفتاحُ الهجاءُ وحدَه وهجاءُ
    #      الخمسةِ واحد ⇒ `len(patterns) > 1` كاذبٌ دائمًا. والآنَ كلُّ قارئٍ
    #      يُقاسُ بموضعِه على مرجعٍ واحد، فينشقُّ أوّلُ هجاءٍ يختلف.
    ("⑧ هجاءُ @rule ينشقُّ عن المرجع",
     "scripts/codegen/gen_rules_matrix.py",
     # (AR) ومرساةٌ بلا خطٍّ مائل: الهروبُ يُبتلَعُ عبرَ مستويَي توليدٍ
     #      وصدفة، فينكسرُ صامتًا. و`@rule:?` فريدٌ في الملفِّ (مقيس).
     _sub(b"@rule:?", b"@rule[ ]+  # zz_probe_rule_split ", residue=True),
     1, _only("✗ ③")),

    # ═══ ④ ملفٌّ لا يُحلَّلُ نحويًّا ═══
    ("⑨ ملفٌّ لا يُحلَّلُ نحويًّا يُعَدُّ ويُحمِّر",
     PROBE_READER,
     _create(("# -*- coding: utf-8 -*-" + LF + "X = <NODE:12>()" + LF).encode("utf-8")),
     1, _only("✗ ④")),

    # ═══ عطبُ آلةٍ — رمزُ ٢ ═══
    # (AR) والمرساةُ نصُّ الاستثناءِ بعينِه لا لفظُ «عطبُ آلة» العامّ: هو في
    #      كلِّ مسارِ رمزِ ٢، فمجسٌّ عليه يرضى بعطبٍ آخرَ تمامًا (قِيسَ).
    ("⑩ مصدرُ معجمٍ مفقود — رمزُ آلةٍ ٢",
     GUARD_REL,
     _sub(b'ROOT / "tests" / "runner.py",', b'ROOT / "tests" / "zz_absent.py",',
          residue=True),
     2, ("مصدرُ معجمٍ مفقود", ("انكمشَ معجم", "لا مرجعَ للمقارنة",
                              "تعذَّرَ تحميلُ طرفَي القياس"))),

    # (AR) 🔑 **وتشويهٌ جزئيٌّ يُحمِّرُ كذلك**: كان الشرطُ `len(tags) < 5`
    #      فمعجمٌ من شظايا (`@de` · `@e` · `@kno`) يمرُّ و② تنكمشُ سبعةً
    #      والحارسُ أخضر. والأرضيّةُ الآنَ على المعجمِ كلِّه.
    # (AR) 🔑 **والانكماشُ حكمٌ لا عطبُ آلة**: كان يُرفَعُ استثناءً فيُقرأُ
    #      رمزَ ٢ («لم يُقَسْ شيء») وقد قِيسَ كلُّ شيء، بتشخيصٍ يتّهمُ ولا
    #      مخرجَ مُسمًّى — وتقاعدُ وسمٍ عملٌ مشروعٌ سمّاه الحارسُ بنفسِه.
    #      ⚠️ والطفرةُ **رفعُ الأرضيّةِ** لا تضييقُ `_TAG`: تضييقُه يبتلعُ
    #         `@rule` نفسَه (`u` خارجَ المدى) فيصيرُ العطبُ «لا قارئَ لـ@rule»
    #         ورمزَ ٢ — أي أنّ المجسَّ يقيسُ غيرَ اللامتغيِّرِ الذي يدّعيه.
    ("⑪ انكماشُ المعجمِ يُحمِّرُ ⑥ حكمًا لا آلةً",
     GUARD_REL,
     _sub(b"FLOOR_LEXICON = 20", b"FLOOR_LEXICON = 21", residue=True),
     1, _only("✗ ⑥")),

    # ═══ ⑤ ترميزُ قارئِ الأسطر ═══
    # (AR) الوجهُ الثالثُ من القارئِ بعدَ النمطِ والنافذة. وردُّ `utf-8` أعادَ
    #      أربعَ بذورٍ ذاتِ بادئةِ BOM إلى «بلا وسم @rule» وهي تحملُه.
    ("⑯ قارئُ أسطرٍ يعودُ إلى utf-8",
     "scripts/codegen/gen_rules_matrix.py",
     _sub(b'enumerate(open(filepath, encoding="utf-8-sig",',
          b'enumerate(open(filepath, encoding="utf-8",', residue=True),
     1, _only("✗ ⑤")),

    # ═══ ط‑١ · غيابُ المرجعِ عطبُ آلةٍ لا سكوت ═══
    # (AR) `_reference` كان يرتدُّ إلى `rules[0]` — فلو زالَ آخرُ قارئٍ بلا
    #      `re.M` لصارَ المرجعُ من طرازٍ آخرَ **صامتًا**، وذهبَت دعوى «مرجعٌ
    #      على طرازِ العدّاء» بلا تشخيص.
    #      والطفرةُ في الحارسِ نفسِه: القرّاءُ بلا `re.M` ثلاثةٌ في الشجرة،
    #      فإسكاتُهم يلزمُه ثلاثُ طفراتٍ والمجسُّ يحقنُ ملفًّا واحدًا. والمقصودُ
    #      إثباتُ أنّ فرعَ «لا مرجع» **حيٌّ ومُسمًّى** لا ميّتٌ لا يُبلَغ.
    ("⑰ لا مرجعَ على طرازِ العدّاء — رمزُ آلةٍ ٢",
     GUARD_REL,
     _sub(b"if not one[3] & re.M", b"if one[3] & re.M and False",
          residue=True),
     2, ("لا مرجعَ للمقارنة", ("مصدرُ معجمٍ مفقود",))),

    # ═══ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    ("⑫ بذرةٌ متعاقَدةٌ جديدةٌ لا تُحمِّر",
     NEW_SEED,
     _create(("# @expected 1" + LF + 'اطبع_سطر("1")' + LF).encode("utf-8")),
     0, "متّفقانِ على كلِّ بذرة"),

    # (AR) 🔑 **النافذةُ مشترَكةٌ فعلًا**: تضييقُها في العدّاءِ يتبعُه الحارسُ
    #      فيبقى الخلافُ صفرًا. ولولا الاشتقاقُ لانفصلا ههنا.
    ("⑬ تضييقُ نافذةِ العدّاءِ يتبعُه الحارس",
     "tests/runner.py",
     _sub(b"if i >= 30:", b"if i >= 4:  # zz_probe_runner_window", residue=True),
     0, "متّفقانِ على كلِّ بذرة"),

    # (AR) 🔑 **ولا رفضَ كاذبًا على `str.split`**: كان `func.attr` وحدَه
    #      يُفحَص، فنصٌّ لا يقرأُ عقدًا (`s.split("@expected")`) يُوقِفُ
    #      البوّابةَ بتشخيصٍ خاطئ (قِيسَ: ② 29 > 28).
    ("⑭ `str.split` على وسمٍ ليس قارئًا",
     PROBE_READER,
     _create(("# -*- coding: utf-8 -*-" + LF
              + 'def f(s): return s.split("@expected")' + LF).encode("utf-8")),
     0, "متّفقانِ على كلِّ بذرة"),

    ("⑮ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     GUARD_REL, _eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),

    # ═══ ⑤ الشكلُ الشقيقُ — وفيه مُنفِّذُ العقدِ نفسُه ═══
    # (AR) 🔑 **وهذا هو المجسُّ الذي لم يكنْ موجودًا فبقيَ الثقبُ حيًّا.**
    #      كان ⑤ يقيسُ `for … in open(…)` وحدَه، فعمِيَ عن
    #      `with open(…) as f: for … in f` — وهو شكلُ `tests/runner.py:323`،
    #      أي **مُنفِّذُ العقدِ**. فرُدَّ إلى `utf-8` ⇒ ⑤ صفرٌ و③ صفرٌ و① صفرٌ،
    #      والبوّابةُ خضراءُ والعقدُ يُبتلَعُ في أوّلِ بذرةٍ ببادئة.
    ("⑱ الشكلُ الشقيقُ `with open` يعودُ إلى utf-8",
     "tests/runner.py",
     _sub(b'with open(filepath, "r", encoding="utf-8-sig") as f:',
          b'with open(filepath, "r", encoding="utf-8") as f:', residue=True),
     1, _only("✗ ⑤")),

    # ═══ ⑧ أرضيّةُ تغطيةِ ⑤ ═══
    # (AR) صفرُ قارئٍ يُقرأُ «صفرَ مخالفة» — والأرضيّةُ هي ما يمنعُ ذلك.
    ("⑲ أرضيّةُ قرّاءِ نصِّ البذرةِ تعضّ",
     GUARD_REL,
     _sub(b"FLOOR_SEED_READERS = 7", b"FLOOR_SEED_READERS = 8", residue=True),
     1, _only("✗ ⑧")),

    # ═══ ⑦ أرضيّةُ نطاقِ التخطّي ═══
    # (AR) أرضيّةٌ أنشأتها رقعةُ «النطاقُ يُشتقُّ من `.gitignore`» ونُسِيَ أن
    #      تُقاس: كانت ٨ والمشتقُّ ٢٧ — فجوةُ ١٩ لا تحمرُّ إلّا بعدَ انهيارِ
    #      سبعينَ بالمئةِ من الملفّ.
    ("⑳ أرضيّةُ نطاقِ التخطّي تعضّ",
     GUARD_REL,
     _sub(b"FLOOR_IGNORED_DIRS = 27", b"FLOOR_IGNORED_DIRS = 28", residue=True),
     1, _only("✗ ⑦")),

    # ═══ رمزُ الآلةِ يُبلَغُ من طبقةِ الاشتقاق ═══
    # (AR) 🔑 كان `_ignored_parts()` يُنفَّذُ عندَ **الاستيراد**، فغيابُ مصدرِه
    #      يُخرِجُ رمزَ ١ وتتبُّعًا خامًّا — و`x.py` يقرأُ ١ حكمًا على المحتوى.
    #      فأداةٌ لم تقِسْ شيئًا كانت تُقرأُ «وجدَت انجرافَ قرّاء».
    ("㉑ غيابُ مصدرِ النطاقِ — رمزُ آلةٍ ٢",
     GUARD_REL,
     _sub(b'path = ROOT / ".gitignore"',
          b'path = ROOT / ".gitignore_zz_probe"', residue=True),
     2, (".gitignore مفقود", ("مصدرُ معجمٍ مفقود", "لا مرجعَ للمقارنة"))),

    # ═══ البادئةُ للمجلَّداتِ وحدَها ═══
    # (AR) 🔑 ملفٌّ **متتبَّعٌ** اسمُه يبدأُ ببادئةِ بناءٍ كان يسقطُ من المسحِ
    #      كلِّه (قِيسَ: `distribution/android/build-android.py`). والمجسُّ
    #      يُنشِئُ نظيرَه بنمطٍ جديد: يُعَدُّ الآنَ (② ٣١ > ٣٠)، ولو رُدَّت
    #      البادئةُ إلى كلِّ الأجزاءِ لاختفى ولم يعضَّ هذا المجسّ.
    ("㉒ ملفٌّ ببادئةِ بناءٍ في اسمِه يُمسَحُ لا يُتخطّى",
     "scripts/codegen/build_zz_tag_probe.py",
     _create(("# -*- coding: utf-8 -*-" + LF
              + "import re" + LF
              + 'ZZ = re.compile(r"^#[ ]*@expected:?[ ]+(.+)$")' + LF
              ).encode("utf-8")),
     1, _only("✗ ②")),
)
# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`).
MIN_PROBES = 22


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
#      سدادِ دَينٍ مشروعٍ يفتحُ فجوةً فتسقطُ مجسّاتٌ سليمةٌ دفعةً واحدة
#      ⇒ لا يُودَعُ سجلُّ العيارِ ⇒ الحارسُ الفوقيُّ أحمرُ
#      عندَ أوّلِ لمسةٍ ⇒ **البوّابةُ مقفلةٌ على العملِ الذي وُجِدَ الحارسُ
#      ليُنجِزَه**. ونزعُ العددِ من نصِّ المجسِّ عالجَ العَرَضَ لا السبب.
#      فالفجوةُ تُرفَضُ ههنا صراحةً **بمخرجٍ مسمًّى**: أنزِلِ السقفَ إلى المقيسِ
#      (أو ارفعِ الأرضيّة) ثمّ أعِدِ العيار — وهو السلوكُ الذي يفرضُه تصميمُ
#      «سقفٌ نازلٌ لا يُرفَع» أصلًا. والشدُّ يُقاسُ في **كلِّ تشغيلةٍ** بـ
#      `_slack_bounds()` ولا يُنثَرُ ههنا عددًا: عددٌ منثورٌ نسخةٌ ثانيةٌ من
#      حقيقةٍ تُقاسُ وتبلى بأوّلِ حدٍّ يُضاف. 🔑 **وقد بلِيَ واحدٌ ههنا فعلًا**:
#      كُتِبَ أنّ «الحدودَ كلَّها مشدودة» ثمّ قُصِرَ هذا القارئُ على السقوفِ
#      في السطرِ التالي مباشرةً — فصارَ النصُّ يعِدُ بما لا يقيسُه.
# (AR) 🔑 **السقوفُ وحدَها.** والأرضيّةُ في هذا الحارسِ (`FLOOR_HANDWRITTEN`)
#      ليست هدفَ طفرةٍ ولا تُطبَعُ في صفٍّ بهذا الشكل، فلا يبلغُها هذا
#      القارئُ أصلًا — وهي محدودةٌ بالمساواةِ مع سقفِها عمدًا: الانكماشُ
#      يُحمِّرُ ولا يُقرأُ تقدّمًا. و«المسموح» لفظُ حارسٍ آخرَ لا يطبعُه هذا
#      (مقيس: صفرُ ذكرٍ له في مخرَجِه) فنُزِعَ من المرساة.
_BOUND = re.compile(r"(\d+)\s*\(السقف\s*(\d+)")
# (AR) 🔑 **والأرضيّةُ تُقرَأُ كالسقف.** كانت خارجَ المرساةِ فمرَّت فجوةُ واحدٍ
#      فيها بـ**١٥/١٥ ورمزِ صفر** — أي سجلُّ عيارٍ كاملٌ يُبارِكُ أرضيّةً لم
#      تعُدْ تعضُّ على اختفاءِ نمط. (قِيسَ بالحقن.)
_FLOOR = re.compile(r"(\d+)\s*\(السقف\s*\d+\s*·\s*الأرضيّة\s*(\d+)")
# (AR) 🔑 **وأرضيّةٌ بلا سقفٍ أرضيّةٌ كذلك.** لامتغيِّرا التغطيةِ (⑦ نطاقُ
#      التخطّي · ⑧ قرّاءُ نصِّ البذرة) يُطبَعانِ بصيغةِ «N (الأرضيّة M)» بلا
#      سقفٍ — وقارئٌ يشترطُ لفظَ «السقف» لا يبلغُهما، فتمرُّ فيهما الفجوةُ
#      كما مرَّت في أختِهما قبلَ صفٍّ واحد.
_FLOOR_ONLY = re.compile(r"(\d+)\s*\(الأرضيّة\s*(\d+)\)")


def _slack_bounds() -> list[str]:
    """(AR) حدودٌ فيها فجوةٌ بينَ المقيسِ والمُعلَن — تمنعُ العيارَ ولا تُخفِقُه."""
    code, out = _run_guard()
    if code != 0:
        return ["الحارسُ ليس أخضرَ قبلَ العيار (رمز=%d)" % code]
    lines = [L for L in out.split(chr(10))
             if _BOUND.search(L) or _FLOOR_ONLY.search(L)]
    if not lines:
        return ["لم يُقرأْ حدٌّ واحدٌ من مخرَجِ الحارس — قارئُ الحدودِ أعمى"]
    if not [L for L in lines if _FLOOR_ONLY.search(L)]:
        return ["لم تُقرأْ أرضيّةٌ مفردةٌ واحدة — صفّا ⑦ و⑧ خارجَ المرساة"]
    slack = []
    for line in lines:
        for measured, bound in _BOUND.findall(line):
            if measured != bound:
                # (AR) ويُسمّى **السطرُ**: زوجُ أرقامٍ بلا اسمٍ لا يدلُّ على
                #      أيِّ لامتغيِّرٍ فيه الفجوة.
                slack.append("%s  (مقيسٌ %s ≠ سقفٌ %s)"
                             % (line.strip(), measured, bound))
        for measured, floor in _FLOOR.findall(line):
            if measured != floor:
                slack.append("%s  (مقيسٌ %s ≠ أرضيّةٌ %s)"
                             % (line.strip(), measured, floor))
        for measured, floor in _FLOOR_ONLY.findall(line):
            if measured != floor:
                slack.append("%s  (مقيسٌ %s ≠ أرضيّةٌ %s)"
                             % (line.strip(), measured, floor))
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
    # (AR) 🔑 **ويُنفى ما سوى اللامتغيِّرِ المُدَّعى.** عنوانُ اللامتغيِّرِ
    #      يُطبَعُ في كلِّ تشغيلة، فمرساةٌ عليه لا تُميِّزُ حمرةً من خضرة —
    #      وبُرهنَ: إعدامُ لامتغيِّرٍ بالكاملِ أبقى العيارَ ١٢/١٢ ورمزَ صفر.
    want, forbidden = (want_text if isinstance(want_text, tuple)
                       else (want_text, ()))
    ok = (code == want_code) and (want in out)
    detail = "رمز=%d (منتظَر %d)" % (code, want_code)
    if want not in out:
        detail += " · النصُّ المنتظَرُ غائب"
    stray = [mark for mark in forbidden if mark in out]
    if stray:
        ok = False
        detail += " · حمرةٌ من لامتغيِّرٍ آخر: %s" % " ".join(stray)
    return ok, detail


def _write_record(passed: int, stamp: str) -> None:
    RECORD_DIR.mkdir(parents=True, exist_ok=True)
    lines = [
        "# ═══════════════════════════════════════════════════════════════════",
        "# سجلُّ عيارٍ — مُشتَقٌّ آليًّا، لا يُحرَّرُ باليد.",
        "# ⚠️ إن تغيّرَ الحارسُ أو المِحقنةُ ولم يُعَدِ العيارُ حمِرَ",
        "#    check_calibration_fresh.py. وإعادتُه:",
        "#    python scripts/codegen/calibrate_seed_tag_readers.py --record --date=YYYY-MM-DD",
        "# ═══════════════════════════════════════════════════════════════════",
        "version: 2",
        "guard: scripts/codegen/check_seed_tag_readers.py",
        "harness: scripts/codegen/calibrate_seed_tag_readers.py",
        "guard_sha256: %s" % _sha_norm(GUARD),
        "harness_sha256: %s" % _sha_norm(HARNESS),
        "calibrated_at: %s" % stamp,
        "probes_total: %d" % len(PROBES),
        "probes_passed: %d" % passed,
        "result: %d/%d" % (passed, len(PROBES)),
    ]
    # (AR) 🔑 **وأهدافُ المجسّاتِ الثالثةُ تُبصَمُ كذلك.** السجلُّ كان يبصمُ
    #      الحارسَ والمِحقنةَ وحدَهما، ومراسي بعضِ المجسّاتِ نصوصٌ تعيشُ في
    #      ملفٍّ **ثالثٍ** (`gen_rules_matrix.py` · `tests/runner.py` …).
    #      فإعادةُ صياغةِ ذلك السطرِ هناك لا تُحرِّكُ البصمتَين، فيبقى
    #      `check_calibration_fresh` أخضرَ ويطبعُ «مُعايَر» بينما المجسُّ صارَ
    #      **لا ينطبقُ أصلًا** — ولا شيءَ يُوجِبُ إعادةَ تشغيلِه ليُكتشَف.
    #      والمعدومُ ههنا يُعلَنُ لا يُسكَتُ عنه: هدفٌ يُنشِئُه المجسُّ نفسُه
    #      (`_create`) لا وجودَ له قبلَ التشغيل، فيُبصَمُ بـ`(معدوم)`.
    #      (EN) Fingerprint third-party probe targets too: a reworded anchor in a
    #      third file leaves both recorded hashes intact, so a probe that can no
    #      longer apply keeps reading as "calibrated".
    third = sorted({entry[1] for entry in PROBES}
                   - {GUARD.relative_to(ROOT).as_posix(),
                      HARNESS.relative_to(ROOT).as_posix()})
    lines.append("targets_sha256:")
    for rel in third:
        path = ROOT / rel
        lines.append("  %s: %s"
                     % (rel, _sha_norm(path) if path.is_file() else "(معدوم)"))
    lines.append("probes:")
    for entry in PROBES:
        name, path, _m, code, text = entry[:5]
        role = ("انحدار — يجبُ أن يبقى أخضر" if code == 0
                else "عطبُ آلة" if code == 2 else "يجبُ أن يحمرّ")
        lines.append('  - name: "%s"' % name)
        lines.append("    target: %s" % path)
        lines.append("    expect_exit: %d" % code)
        lines.append('    expect_text: "%s"'
                     % (text[0] if isinstance(text, tuple) else text))
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

    print("عيارُ حارسِ «قارئا العقدِ يقولانِ الشيءَ نفسَه» بالحقن:")
    print("%-50s %-9s %s" % ("المجسّ", "النتيجة", "التفصيل"))
    print("-" * 88)

    dirty = _residue()
    if dirty:
        print("✗ عطبُ آلة: أرضيّةٌ ملوّثةٌ بأثرِ تشغيلةٍ سابقةٍ لم تُنهَ:")
        for item in dirty:
            print("    · %s" % item)
        # (AR) والمخرجُ يُسمّى: السجلُّ في مجلَّدِ git فلا يبلغُه `git clean -xfd`.
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
