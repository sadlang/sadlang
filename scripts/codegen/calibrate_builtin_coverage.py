#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ «الوعدُ المُعلَنُ يُقاسُ ولا يُدَّعى».

     حارسٌ لا يحمرُّ زينة. وهذه الأداةُ تُثبِتُ أنّه يحمرّ: تحقنُ عطبًا مقصودًا
     واحدًا في كلِّ مرّة، تُشغّلُ الحارس، تقيسُ رمزَ خروجِه ونصَّه، ثمّ **تستعيدُ
     البايتاتِ استعادةً تُبرهَنُ ببصمةِ sha256**.

     مجسّاتٌ عدّتُها `len(PROBES)` وأرضيّتُها `MIN_PROBES` — ولا تُنثَرُ ههنا
     عددًا يبلى. وأربعةٌ منها ليست تكرارًا لبعضِها:

       ④ **انحدارُ قدرة**: تُنزَعُ ذراعُ إرسالٍ **حقيقيّة** من المترجّم. وهذا
         المجسُّ وُلِدَ من مراجعةٍ خصميّةٍ أثبتَت أنّ نسخةً سابقةً من الحارسِ
         **تخسرُ صافيًا** قدرةَ كشفٍ كانت قائمة: حذفُ ثلاثَ عشرةَ ذراعَ WebSocket
         حقيقيّةً كان أحمرَ تحتَ الحارسِ القديمِ وأخضرَ تحتَ الجديد — لأنّ تلك
         الأذرعَ كانت موسومةً دَينًا كاذبًا. فالمجسُّ يحرسُ التصحيحَ لا الشفرةَ فقط.

       ⑥ **غشُّ التعليق** — يجبُ ألّا يحمرّ: تعليقٌ يذكرُ أسماءَ مدمجاتٍ مؤجَّلة.
         هجومٌ **أسقطَ مسبارًا سابقًا فعلًا** (٥٦٤ ← ٥٥٩ بسطرِ تعليقٍ واحد).

       ⑦ **الغسلُ بمجلَّدِ اختبار** — يجبُ ألّا يحمرّ ولا أن يُخفِضَ العدد: ملفٌّ
         يُسقَطُ في `compiler/tests/` يسمّي مدمجاتٍ مؤجَّلة. بُرهنَ أنّ ترويسةً
         واحدةً كانت تكفي لغسلِ ١٢٠٤ من ١٢٠٥ قبلَ استبعادِ مجلّداتِ الاختبار.

       ⑧ **مسبارٌ أعمى** — يجبُ أن يُنهيَ بالرمز 2 لا 1: عطبُ **آلةٍ** لا عطبُ
         **محتوى**، والخلطُ يجعلُ الأداةَ العمياءَ تُقرأُ «وجدَت عطبًا» ولم تنظرْ.

     🔑 و`--record` يُحدِّثُ سجلَّ العيارِ ومعه **بصمةُ الحارسِ وبصمةُ المِحقنةِ
        وقتَ العيار**. فإن تغيّرَ أيٌّ منهما ولم يُعَدِ العيارُ حمِرَ الحارسُ
        الفوقيُّ `check_calibration_fresh.py`.

(EN) Injection harness proving the coverage guard reddens. One defect at a time,
     byte-exact restore verified by sha256. Probe ④ is a capability-regression
     test born from an adversarial review; probes ⑥ and ⑦ must stay GREEN — they
     are regression tests for a comment-scanning defect and a test-directory
     laundering vector. Probe ⑧ must exit 2: a blind instrument is a tool fault.
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
GUARD = ROOT / "scripts" / "codegen" / "check_builtin_engine_coverage.py"
HARNESS = Path(__file__).resolve()
RECORD_DIR = ROOT / "scripts" / "codegen" / "calibration"
RECORD = RECORD_DIR / "check_builtin_engine_coverage.yaml"


# (AR) ثابتانِ بدلَ محرَفَي هروبٍ: تُنقَلُ الشفرةُ عبرَ صدفاتٍ تبتلعُ
#      الخطَّ المائلَ فتنكسرُ بصمتًا.
CRLF = (chr(13) + chr(10)).encode("ascii")
LF_ = chr(10).encode("ascii")


# ═══ البصمةُ تُقاسُ على التمثيلِ الذي يملكُه git ═══════════════════════════
# (AR) 🔑 **مرجعُ القياسِ يلائمُ مرجعَ البصمة.** `.gitattributes` يُعلِنُ
#      `*.py text eol=lf`، فما يُخرِجُه `git checkout` بـLF مهما كانت نهاياتُ
#      الأسطرِ على قرصِ الكاتب. وبصمةٌ على البايتاتِ الخامِّ تُسجَّلُ إذن على
#      تمثيلٍ **لا يملكُه المستودع**: سُجِّلت على CRLF فلم تطابقْ أيَّ استنساخٍ
#      نظيف — و`x.py gen --check` يحمرُّ في CI بحمرةٍ لا علاقةَ لها بالمحتوى.
#      (والدعوى تُقاسُ ولا تُنثَر: أكثرُ ملفّاتِ `scripts/codegen` بـLF على
#       القرصِ لأنّ `git checkout` هو ما كتبَها؛ والشاذُّ ما كُتِبَ محلّيًّا.
#       ولا يُثبَّتُ العددُ ههنا — أُثبِتَ مرّةً فبلِيَ في الجولةِ التالية.)
#      و`x.py` يحملُ `_norm` لهذا السببِ حرفيًّا منذُ قبل — ولم يُعَدْ استعمالُه.
#      ⚠️ والتطبيعُ **اتّفاقُ قراءةٍ** لا الحقيقةَ المقيسة، فيلزمُ أن يكونَ واحدًا
#         في الطرفَين. واتّفاقُهما **يُقاسُ** بالمجسِّ ⑩ لا يُوعَدُ به في تعليق.
def _sha_bytes(blob: bytes) -> str:
    return hashlib.sha256((blob or b"").replace(CRLF, LF_)).hexdigest()


def _sha_norm(path) -> str:
    """(AR) بصمةٌ على النصِّ بعدَ تطبيعِ نهاياتِ الأسطرِ إلى ما يُودِعُه git."""
    return _sha_bytes(path.read_bytes())


def _is_date(stamp: str) -> bool:
    """(AR) تاريخٌ فعليٌّ لا شكلٌ يُشبِهُه.

    🔑 و`\\d` في بايثون **يوني‌كوديّ**: قِيسَ أنّ `٢٠٢٦-٠٩-٠٥` بالأرقامِ العربيّةِ
       الهنديّةِ و`۲۰۲۶-۰۹-۰۵` بالفارسيّةِ و`9999-99-99` و`0000-00-00` تجتازُ
       `fullmatch(r"\\d{4}-\\d{2}-\\d{2}")` جميعًا. فشكلٌ يُشبِهُ التاريخَ ليس تاريخًا.
    """
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
    """(AR) عدّاءُ المجسِّ ⑩ — يُنفَّذُ والحارسُ مكتوبٌ بصورةِ LF.

    🔑 ولا يُشغِّلُ الحارسَ الفوقيَّ عمدًا: ذاك يقارنُ بالسجلِّ، والسجلُّ يُكتَبُ
       بهذه الدالّةِ نفسِها — فيدورُ المجسُّ على نفسِه ولا يبقى شاهدًا. يقيسُ
       **اللامتغيِّرَ مباشرةً**: بصمةُ الملفِّ بصورتَيه سواء. فإن سقطَ التطبيعُ
       عن هذا الطرفِ ظهرَ ههنا فورًا. وسقوطُه عن الطرفِ الآخرِ (الحارسِ الفوقيِّ)
       يُمسَكُ بغيرِ هذا: بصمةٌ مُسجَّلةٌ مطبَّعةٌ تُقابَلُ ببايتاتٍ خامٍّ ⇒ انجرافٌ
       أحمرُ على كلِّ قرصٍ بـCRLF.
    """
    # (AR) 🔑 يمرُّ بـ`_sha_norm` — **كاتبةِ السجلِّ بعينِها** — لا بـ`_sha_bytes`
    #      تحتَها. فحارسٌ يشهدُ لطبقةٍ أسفلَ التي تُكتَبُ منها الحقيقةُ يخضرُّ
    #      وإن تجاوزَتْها الطبقةُ العليا: قِيسَ في مراجعةٍ خصميّةٍ أنّ جعلَ
    #      `_sha_norm` تبصِمُ البايتاتِ الخامَّ مباشرةً يُبقي ⑩ أخضرَ ١٠/١٠.
    #      والاستعادةُ مضمونةٌ: `_probe` يُعيدُ البايتاتِ في `finally` ويتحقّقُ.
    blob = GUARD.read_bytes()
    as_lf = _sha_norm(GUARD)
    GUARD.write_bytes(blob.replace(LF_, CRLF))
    as_crlf = _sha_norm(GUARD)
    GUARD.write_bytes(blob)
    if as_lf == as_crlf:
        return 0, "بصمةٌ ثابتةٌ عبرَ نهاياتِ الأسطر: %s" % as_lf[:16]
    return 1, ("انجرافٌ بنهايةِ السطر: LF=%s ≠ CRLF=%s"
               % (as_lf[:16], as_crlf[:16]))


# ═══ الأثرُ يُشتقُّ من المجسِّ ولا يُكتَبُ مرّتَين ═══════════════════════════
# (AR) 🔑 كان سجلُّ الأثرِ نسخةً ثانيةً باليدٍ لِما في `PROBES`: تُعادُ كتابةُ
#      المسارِ ونصِّ الحقنِ حرفيًّا. وبُرهنَ عماه: غُيِّرت كلمةٌ واحدةٌ في مجسِّ ⑥
#      ولم تُتبَعْ في السجلِّ ⇒ حُوكيت تشغيلةٌ مقتولةٌ فقاست المِحقنةُ **فوقَ
#      أثرِها** وأعلنت نجاحًا. فصارَ الأثرُ **سِمةً على المُصنِّعِ نفسِه**: ما
#      يُحقَنُ هو ما يُبحَثُ عنه، بلا نسخةٍ ثانيةٍ تبلى.
#      وما يُوسَمُ أثرًا هو ما **لا يُحمِّرُ الحارسَ**: حقنُ yaml يفضحُ نفسَه،
#      أمّا سطرُ تعليقٍ أو ملفٌّ في `tests/` فأثرُهما صامت. ولذا `residue=True`
#      يُطلَبُ صراحةً في `_sub` ولا يُفترَض — و`  intent: مؤجَّل` مثلًا نصٌّ
#      مشروعٌ في ٥٩٩ موضعًا، فوسمُه أثرًا **رفضٌ كاذبٌ** لا حراسة.
_CREATED = object()      # ملفٌّ يُنشَأ — الدليلُ وجودُه
# (AR) 🔑 **الصمتُ يُصرَّحُ به ولا يُورَثُ بالإغفال.** كان مجسٌّ بلا `residue`
#      **سهوًا** يبدو متتبَّعًا لأنّ مجسًّا آخرَ يُسجِّلُ المسارَ نفسَه بسِمةٍ
#      أخرى — والاشتقاقُ كان يطوي على المسار. فصارَ لكلِّ مجسٍّ تصريحٌ واجبٌ،
#      والاشتقاقُ **صفًّا لكلِّ مجسٍّ لا لكلِّ مسار**.
_SELF_RED = object()     # الطفرةُ تُحمِّرُ الحارسَ بنفسِها — البوّابةُ تلتقطُها
_NO_TRACE = object()     # لا أثرَ دلاليًّا — ويلزمُ تعليلٌ عندَ الإسناد


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


def _sub(old: bytes, new: bytes, count: int = 1, residue: bool = False):
    def apply(blob: bytes) -> bytes:
        if blob is None or blob.count(old) < 1:
            raise AssertionError("المرساةُ غيرُ موجودة: %r" % old[:48])
        return blob.replace(old, new, count)
    apply.residue = new if residue else _SELF_RED
    return apply


# (AR) أدنى طولٍ لسِمةِ الأثر. وسِمةٌ من أربعةِ بايتاتٍ ليست دليلًا بل بذرةُ
#      **رفضٍ كاذب**: تُطابِقُ نصًّا مشروعًا في الشجرة. وسِمةٌ فارغةٌ (نصٌّ كلُّه
#      فراغ) تسقطُ من الاشتقاقِ **صامتةً** فيعمى السجلُّ عن مجسٍّ يترك أثرًا.
MIN_RESIDUE_MARK = 16


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


NET = "compiler/src/frontend/builders/builtins_network.cpp"
SADNET = "language-truth/builtins/sadnet.yaml"
CORE = "language-truth/builtins/core.yaml"
CORE_CPP = "compiler/src/frontend/builders/builtins_core.cpp"
LAUNDER = "compiler/tests/zz_calibration_launder_probe.h"

# (AR) بصماتُ التلوّث: أثرٌ لا يتركُه إلّا مجسٌّ لم يُنهِ استعادتَه.
#      🔑 و`finally` يحمي من الاستثناءِ ومن Ctrl-C، **ولا يحمي من SIGKILL** ولا
#      من انقطاعِ طاقة. وأثرُ المجسّاتِ ①③④⑤⑨ يُحمِّرُ الحارسَ فيُكشَف؛ أمّا ⑥
#      (سطرُ تعليقٍ) و⑦ (ملفٌّ في `compiler/tests/`) فأثرُهما **صامت** — الحارسُ
#      يبقى أخضرَ عليه. فالعلاجُ **رفضُ القياسِ على أرضيّةٍ ملوّثة** لا إخفاؤها
#      في `.gitignore`: أداةٌ تقيسُ فوقَ أثرِ نفسِها تُثبِتُ ما لا تعرف.
#      (والدرسُ مُدوَّن: «أداةُ قياسٍ لا تُنهى تُفسِدُ القياسَ التالي».)
def _derive_residue() -> tuple:
    """(AR) (اسمٌ، مسارٌ، تصريحُ أثرٍ) — صفٌّ **لكلِّ مجسٍّ لا لكلِّ مسار**.

    والطيُّ على المسارِ كان يجعلُ مجسًّا بلا تصريحٍ يركبُ تسجيلَ أخيه في الملفِّ
    نفسِه فيبدو متتبَّعًا وليس كذلك.
    """
    out = []
    for entry in PROBES:
        name, path, mutate = entry[0], entry[1], entry[2]
        mark = getattr(mutate, "residue", None)
        if mark is None:
            raise AssertionError(
                "مجسٌّ بلا تصريحِ أثر: %s — يلزمُه سِمةٌ أو"
                " _CREATED/_SELF_RED/_NO_TRACE" % name)
        out.append((name, path, mark))
    return tuple(out)


def _residue() -> list[str]:
    """(AR) أثرُ تشغيلةٍ سابقةٍ لم تُنهَ. وجودُه ⇒ لا قياسَ، رمزُ ٢.

    و`finally` يحمي من الاستثناءِ ومن Ctrl-C، **ولا يحمي من SIGKILL** ولا من
    انقطاعِ طاقة. وأداةٌ تقيسُ فوقَ أثرِ نفسِها تُثبِتُ ما لا تعرف.
    (والدرسُ مُدوَّن: «أداةُ قياسٍ لا تُنهى تُفسِدُ القياسَ التالي».)
    """
    found: list[str] = _inflight()
    for name, rel, mark in _derive_residue():
        path = ROOT / rel
        if mark is _CREATED:
            if path.exists():
                found.append(f"{rel} — ملفُّ مجسٍّ باقٍ ({name})")
        elif mark in (_SELF_RED, _NO_TRACE):
            # (AR) `_SELF_RED` تلتقطُه البوّابةُ نفسُها، و`_NO_TRACE` مُعلَّلٌ
            #      عندَ إسنادِه. فليس ههنا ما يُمسَح.
            continue
        elif path.is_file() and mark in path.read_bytes():
            found.append(f"{rel} — أثرُ حقنٍ باقٍ ({name}):"
                         f" {mark.decode('utf-8', 'replace')[:40]!r}")
    return found


@_no_trace("تطبيعُ نهاياتِ الأسطرِ لا يُغيِّرُ بصمةً مطبَّعةً ولا دلالةَ بايتٍ"
           " واحدٍ — وهو نصُّ اللامتغيِّرِ الذي يُثبِتُه هذا المجسُّ نفسُه.")
def _to_lf(blob: bytes) -> bytes:
    """(AR) صورةُ الملفِّ كما يُخرِجُها `git checkout` تحتَ `eol=lf`."""
    return (blob or b"").replace(CRLF, LF_)


# ═══ المرجعُ يُقاسُ ولا يُنسَخ ═══════════════════════════════════════════════
# (AR) 🔑 كان النصُّ المنتظَرُ يحملُ **٥٩٩ مثبَّتًا** في ثلاثةِ مجسّات — وهو
#      `CEILING_DEFERRED` المُعلَنُ **نازلًا لا يُرفَع**. فأوّلُ سدادِ دَينٍ
#      مشروعٍ يُسقِطُ ثلاثةَ مجسّاتٍ سليمة ⇒ لا يُعادُ سجلُّ العيار ⇒ الحارسُ
#      الفوقيُّ أحمر ⇒ **البوّابةُ تُقفَلُ على العملِ الذي وُجِدَ الحارسُ
#      ليُنجِزَه**. وهو عينُ ما وُجِدَ في مِحقنةِ العقد، والتُقِطَ ههنا بأداةِ
#      برهانٍ لا بقراءة. والعددُ الآنَ **يُقاسُ من الحارسِ نفسِه قبلَ أوّلِ
#      حقن**، فلا نسخةَ ثانيةً تبلى.
_DEFERRED = re.compile("دَينُ تأجيل [(]intent: [^)]*[)]: ([0-9]+)")
_BASELINE: dict[str, int] = {}


def _measure_baseline() -> None:
    """(AR) يقيسُ الدَّينَ القائمَ من مخرَجِ الحارسِ **قبلَ** أوّلِ حقن."""
    code, out = _run_guard()
    match = _DEFERRED.search(out)
    if code != 0 or not match:
        raise AssertionError(
            "تعذَّرَ قياسُ المرجعِ قبلَ الحقن (رمز=%d) — لا عيارَ على مرجعٍ مجهول"
            % code)
    _BASELINE["deferred"] = int(match.group(1))


def _deferred_is(delta: int = 0):
    """(AR) نصٌّ منتظَرٌ **مؤجَّلُ التقويم**: يُبنى من المقيسِ لا من ثابت."""
    return lambda: "دَينُ تأجيل (intent: مؤجَّل): %d" % (_BASELINE["deferred"] + delta)


def _deferred_over_ceiling():
    """(AR) رسالةُ التجاوز: المقيسُ+١ فوقَ السقفِ — طرفاها مقيسانِ لا منقولان."""
    return lambda: "%d > %d" % (_BASELINE["deferred"] + 1, _BASELINE["deferred"])


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر)
PROBES = (
    ("① وعدٌ كاذب — مؤجَّلٌ يُوسَمُ stable",
     SADNET, _sub("  intent: مؤجَّل".encode("utf-8"), b"  status: stable"),
     1, "وعدٌ كاذب (stable بلا إرسال): 1"),

    # (AR) ولا عددَ في الاسمِ أيضًا: الاسمُ يُنسَخُ حرفيًّا إلى سجلِّ العيار،
    #      فعددٌ فيه نسخةٌ ثانيةٌ تبلى بأوّلِ نزلةٍ مشروعةٍ للسقف.
    ("② دَينُ التأجيلِ ينمو بندًا واحدًا فوقَ السقف",
     CORE, _sub(b"  status: stable", "  intent: مؤجَّل".encode("utf-8")),
     1, _deferred_over_ceiling()),

    ("③ حقلٌ محذوف — هروبٌ من ① و②",
     SADNET, _sub("  intent: مؤجَّل\n".encode("utf-8"), b""),
     1, "خارجَ التعداد): 1"),

    # (AR) ولا تُنزَعُ ههنا `Bwsc::NEW_CLIENT`: هي **قناريّةُ عيارِ الحارسِ نفسِه**
    #      (المجسُّ الخامسُ في `_calibrate`)، فنزعُها يُنهي بالرمز 2 لا 1 — عطبَ
    #      آلةٍ لا عطبَ محتوى. وقناريّةُ الأداةِ لا تصلحُ مادّةَ اختبارٍ لها.
    ("④ انحدارُ قدرة — تُنزَعُ ذراعُ إرسالٍ حقيقيّة",
     NET, _sub(b"Bwsc::SEND", b"Bwsc::ZZ_REMOVED_ARM", 99, residue=True),
     1, "وعدٌ كاذب (stable بلا إرسال): 1"),

    ("⑤ الحقلانِ معًا — status وintent",
     SADNET, _sub("  intent: مؤجَّل".encode("utf-8"),
                  "  intent: مؤجَّل\n  status: stable".encode("utf-8")),
     1, "خارجَ التعداد): 1"),

    # (AR) 🔑 **بابُ الهروبِ.** كان ③ يسألُ «أثمّةَ حقل؟» لا «أقيمتُه مُعلَنة؟»،
    #      و① يشترطُ `== "stable"` و② يشترطُ `== "مؤجَّل"` — فأيُّ قيمةٍ ثالثةٍ
    #      تُفلِتُ من الثلاثةِ معًا. وبُرهنَ بالحقن: ذراعٌ محذوفةٌ + `experimental`
    #      ⇒ أربعُ لامتغيِّراتٍ خضراءُ ورمزُ صفر، والبندُ يسقطُ من الميزانَين.
    #      و`experimental` ليست فرضًا بعيدًا: مخطَّطُ المستودعِ نفسُه يُبارِكُها
    #      (`stability: [stable, experimental, deprecated]`).
    ("⑨ قيمةٌ خارجَ التعداد — status: experimental",
     SADNET, _sub("  intent: مؤجَّل".encode("utf-8"), b"  status: experimental"),
     1, "خارجَ التعداد): 1"),

    ("⑥ غشُّ التعليقِ — يجبُ ألّا يُخفِّضَ العدد",
     CORE_CPP, _append(b"\n// NODE_NEW NODE_ID SHA256 DRAW_CIRCLE SCREEN_WIDTH\n"),
     0, _deferred_is()),

    ("⑦ الغسلُ بمجلَّدِ اختبار — يجبُ ألّا يُخفِّضَ العدد",
     LAUNDER, _create("// غسلٌ مقصود\nnamespace Zz = Sad::Builtins::Names::SadNet;\n"
                      "static auto _a = Zz::NODE_NEW; static auto _b = Zz::NODE_ID;\n"
                      .encode("utf-8")),
     0, _deferred_is()),

    # (AR) 🔑 **مجسُّ انحدارٍ: البصمةُ لا تتغيّرُ بنهاياتِ الأسطر.** يُكتَبُ
    #      الحارسُ بصورتِه التي يُخرِجُها `git checkout` (LF) ويُنتظَرُ **أخضرُ**
    #      من الحارسِ الفوقيّ. ولولاه لعادَ العطبُ صامتًا بأوّلِ رقعةٍ تنسى
    #      التطبيع: كانت البصمةُ تُحسَبُ على CRLF فلا تطابقُ أيَّ استنساخٍ نظيف،
    #      والبوّابةُ تحمرُّ في CI بحمرةٍ لا علاقةَ لها بالمحتوى.
    #      وهو أيضًا **قياسُ اتّفاقِ الطرفَين** على اتّفاقِ القراءة: لو طبّعَ أحدُهما
    #      ولم يُطبِّعِ الآخرُ لَحمِرَ ههنا.
    ("⑩ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     "scripts/codegen/check_builtin_engine_coverage.py", _to_lf,
     0, "بصمةٌ ثابتة", _run_eol_invariance),

    ("⑧ مسبارٌ أعمى — رمزُ عطبِ آلةٍ 2",
     "scripts/codegen/check_builtin_engine_coverage.py",
     _sub(b'AREAS = ("compiler", "shared", "runtime", "stdlib", "tools", '
          b'"features", "platform")',
          b'AREAS = ("platform",)', residue=True),
     2, "عيارُ الأداة:"),
)


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
_BOUND = re.compile(r"(\d+)\s*\((?:السقف|الأرضيّة|المسموح)\s*(\d+)")


def _slack_bounds() -> list[str]:
    """(AR) حدودٌ فيها فجوةٌ بينَ المقيسِ والمُعلَن — تمنعُ العيارَ ولا تُخفِقُه."""
    code, out = _run_guard()
    if code != 0:
        return ["الحارسُ ليس أخضرَ قبلَ العيار (رمز=%d)" % code]
    pairs = _BOUND.findall(out)
    if not pairs:
        return ["لم يُقرأْ حدٌّ واحدٌ من مخرَجِ الحارس — قارئُ الحدودِ أعمى"]
    return ["مقيسٌ %s ≠ مُعلَنٌ %s" % (m, b) for m, b in pairs if m != b]


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
    try:
        target.parent.mkdir(parents=True, exist_ok=True)
        _journal_open(path, before)
        target.write_bytes(mutate(original))
        code, out = (runner or _run_guard)()
    finally:
        if existed:
            target.write_bytes(original)          # type: ignore[arg-type]
        else:
            target.unlink(missing_ok=True)
    if existed:
        if hashlib.sha256(target.read_bytes()).hexdigest() != before:
            raise AssertionError("لم تُستعَدِ البايتاتُ في " + path)
    elif target.exists():
        raise AssertionError("لم يُحذَفِ الملفُّ المؤقّت: " + path)
    # (AR) ولا يُمحى السجلُّ إلّا بعدَ استعادةٍ مُتحقَّقٍ منها بالبصمةِ أو بالغياب.
    _journal_close()
    # (AR) نصٌّ مؤجَّلُ التقويمِ يُبنى من المرجعِ المقيسِ — لا ثابتَ منسوخ.
    want_text = want_text() if callable(want_text) else want_text
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
        "#    python scripts/codegen/calibrate_builtin_coverage.py --record --date=YYYY-MM-DD",
        "# ═══════════════════════════════════════════════════════════════════",
        "version: 2",
        "guard: scripts/codegen/check_builtin_engine_coverage.py",
        "harness: scripts/codegen/calibrate_builtin_coverage.py",
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
        # (AR) ويُقوَّمُ المؤجَّلُ قبلَ الإيداعِ: سجلٌّ يحملُ `<function …>`
        #      نصٌّ لا يُقابِلُه شيء.
        lines.append('    expect_text: "%s"' % (text() if callable(text) else text))
        lines.append('    role: "%s"' % role)
    RECORD.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`)،
#      وههنا تمنعُ **المِحقنةَ نفسَها** من إعلانِ نجاحٍ بلا قياس: بـ`PROBES = ()`
#      كانت تُرجِعُ صفرًا وتكتبُ `0/0`، فالحارسُ الفوقيُّ وحدَه يمسكُها.
#      وأداةُ قياسٍ تُعلِنُ نجاحًا بلا أن تقيسَ هي عينُ ما تُنشَأُ لمنعِه.
MIN_PROBES = 10


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    record = "--record" in sys.argv
    stamp = ""
    for arg in sys.argv[1:]:
        if arg.startswith("--date="):
            stamp = arg.split("=", 1)[1]
    if record and stamp and not _is_date(stamp):
        print("✗ --date شكلُه YYYY-MM-DD — و%r ليس تاريخًا. سجلٌّ يحملُ نصًّا"
              " مكانَ تاريخٍ لا يُقالُ عنه متى عُويِر." % stamp)
        return 2
    if record and not stamp:
        print("✗ --record يلزمُه --date=YYYY-MM-DD (لا يُقرأُ وقتُ النظامِ"
              " لئلّا يتغيّرَ السجلُّ بلا عمل)")
        return 2

    print("عيارُ حارسِ «الوعدُ المُعلَنُ يُقاسُ ولا يُدَّعى» بالحقن:")
    print("%-50s %-9s %s" % ("المجسّ", "النتيجة", "التفصيل"))
    print("-" * 88)
    dirty = _residue()
    if dirty:
        print("✗ عطبُ آلة: أرضيّةٌ ملوّثةٌ بأثرِ تشغيلةٍ سابقةٍ لم تُنهَ —"
              " لا يُقاسُ فوقَه:")
        for item in dirty:
            print("    · %s" % item)
        print("  نظِّفْ ثمّ أعِدْ: git checkout -- <الملفّ> · احذفِ الملفَّ الباقي.")
        return 2

    # (AR) 🔑 **حدٌّ فيه فجوةٌ يُوقِفُ العيارَ ولا يُخفِقُه.** مجسّاتُ «يجبُ أن
    #      تحمرّ» تحقنُ بندًا واحدًا، فحمرتُها مشروطةٌ بأن يكونَ العدَّادُ على
    #      حدِّه. والمخرجُ مُسمًّى: أنزِلِ السقفَ إلى المقيسِ (أو ارفعِ الأرضيّة)
    #      ثمّ أعِدِ العيار — وهو ما يفرضُه تصميمُ «نازلٌ لا يُرفَع» أصلًا.
    slack = _slack_bounds()
    if slack:
        print("✗ عطبُ آلة: حدٌّ فيه فجوةٌ — المجسّاتُ لا تعضُّ على هامش:")
        for item in slack:
            print("    · %s" % item)
        print("    ⤷ العلاج: أنزِلِ السقفَ إلى المقيسِ (أو ارفعِ الأرضيّة)"
              " في الحارس، ثمّ أعِدِ العيار.")
        return 2

    if len(PROBES) < MIN_PROBES:
        print("✗ عطبُ آلة: %d مجسًّا < %d — مِحقنةٌ قُلِّصت، فلا تُعلِنُ نجاحًا."
              % (len(PROBES), MIN_PROBES))
        return 2

    # (AR) المرجعُ يُقاسُ **قبلَ** أوّلِ طفرةٍ — بعدَها يكونُ قياسًا فوقَ أثرٍ.
    _measure_baseline()

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
    tail = out.strip().splitlines()[-1].strip() if out.strip() else ""
    print("وبعدَ الاستعادة: رمز=%d · %s" % (code, tail))

    if passed != len(PROBES) or code != 0:
        print("✗ لا يُودَعُ سجلُّ عيارٍ ناقص.")
        return 1
    if record:
        _write_record(passed, stamp)
        print("✓ أُودِعَ السجلُّ: %s" % RECORD.relative_to(ROOT).as_posix())
    return 0


if __name__ == "__main__":
    # (AR) 🔑 كأختَيها. انهيارٌ في المِحقنةِ ليس حكمًا على المحتوى: مرساةُ حقنٍ
    #      اختفَتْ، أو ملفُّ مجسٍّ غيرُ مقروء ⇒ **لم تُقَسْ**، فرمزُها ٢ لا ١.
    #      (والدرسُ مُدوَّن: «الرقعةُ تسدُّ في ملفٍّ وتتركُ الأخوات» — سُدَّ
    #       الحارسانِ في جولتَين وبقيت هذه حتّى الرابعة.)
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
