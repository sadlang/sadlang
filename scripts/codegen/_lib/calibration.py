#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) **قلبُ العيارِ المشترَك** — آليّةُ الحقنِ والاستعادةِ والسجلّ، مرّةً واحدة.

     العلّةُ المقيسة (٢٠٢٦-٠٩-٠٦): ستُّ مِحقناتٍ · ٣٦٥٠ سطرًا · ٢٣ دالّةً
     مشترَكة — **سبعٌ متطابقةٌ حرفًا وستَّ عشرةَ منجرفة**. `_is_date` بأربعِ
     صور؛ و`_journal_open`/`_journal_close` — شبكةُ أمانِ الانهيارِ نفسُها —
     بثلاثِ صورٍ لكلٍّ؛ و`_slack_bounds` بصورتَينِ لأنّ تحسينًا نزلَ في واحدةٍ
     وتُرِكَت ثلاثٌ. وهي عينُ الصورةِ المُدوَّنةِ في سجلِّ دروسِ هذا المستودع:
     «الرقعةُ تسدُّ في ملفٍّ وتتركُ الأخوات».

     🔑 **والأثرُ ليس جماليًّا**: كلفةُ المِحقنةِ الجديدةِ كانت ≈٥٠٠ سطرٍ
        منسوخ، وهي السببُ في أنّ خمسةً من ثلاثينَ حارسًا مُعايَرون. فالقلبُ
        المشترَكُ يجعلُ المِحقنةَ إعلانًا: حارسٌ · عنوانٌ · صفُّ مجسّات.

     وما يبقى في كلِّ مِحقنةٍ هو ما **يختلفُ** حقًّا: مراسي الحقنِ ونصوصُ
     الانتظارِ وقياسُ المرجعِ قبلَ الطفرة.

(EN) The shared calibration core: injection, verified restore, flight journal
     and record writing — spelled once. Measured: 6 harnesses shared 23
     functions, 16 of which had already drifted into 2-6 spellings each.
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

ROOT = Path(__file__).resolve().parents[3]

# (AR) ثابتانِ بدلَ محرَفَي هروب: تُنقَلُ الشفرةُ عبرَ صدفاتٍ تبتلعُ الخطَّ المائل.
CRLF = (chr(13) + chr(10)).encode("ascii")
LF_ = chr(10).encode("ascii")


# ═══ البصمةُ تُقاسُ على التمثيلِ الذي يملكُه git ═══════════════════════════
# (AR) `.gitattributes` يُعلِنُ `*.py text eol=lf`، فبصمةٌ على البايتاتِ الخامِّ
#      تُحمِّرُ كلَّ استنساخٍ نظيفٍ بحمرةٍ لا علاقةَ لها بالمحتوى.
def sha_bytes(blob: bytes) -> str:
    return hashlib.sha256((blob or b"").replace(CRLF, LF_)).hexdigest()


def sha_norm(path) -> str:
    return sha_bytes(Path(path).read_bytes())


def is_date(stamp: str) -> bool:
    """(AR) تاريخٌ فعليٌّ لا شكلٌ يُشبِهُه — و`\\d` في بايثون يوني‌كوديّ،
    فـ`٢٠٢٦-٠٩-٠٦` بالأرقامِ العربيّةِ كان يمرُّ على تعبيرٍ نمطيّ."""
    if not isinstance(stamp, str) or not stamp.isascii():
        return False
    try:
        date.fromisoformat(stamp)
    except ValueError:
        return False
    return True


def to_lf(blob: bytes) -> bytes:
    """(AR) لا يُغيِّرُ المحتوى — يُغيِّرُ تمثيلَ نهاياتِ الأسطرِ وحدَه.

    🔑 و`blob or b""` لازمٌ لا زينة: هدفٌ غيرُ موجودٍ يُمرَّرُ `None`. وكانت
       الخمسُ نسخًا منجرفةً بهذا الحرفِ بعينِه — إحداها تنهارُ عليه."""
    return (blob or b"").replace(CRLF, LF_)


# (AR) 🔑 **ومُطفِّرُ نهاياتِ الأسطرِ يُورَثُ جاهزًا.** كلُّ مِحقنةٍ كانت
#      تُعيدُ هجاءَه لتُعلِّقَ عليه `no_trace` — خمسُ نسخٍ، اثنتانِ منها فقدَتا
#      `blob or b""`. فالتعليلُ واحدٌ لأنّ الحقيقةَ واحدة، والكائنُ واحد.
EOL_REASON = ("تطبيعُ نهاياتِ الأسطرِ لا يُبدِّلُ محتوًى، و`.gitattributes` "
              "يُعلِنُ `*.py text eol=lf` فالمُودَعُ بـLF أصلًا — فلا أثرَ "
              "دلاليًّا يبقى، والبصمةُ مطبَّعةٌ على الوجهَين.")


def eol_to_lf(blob: bytes) -> bytes:
    """(AR) مُطفِّرُ «نهاياتُ الأسطرِ لا تُبدِّلُ البصمة» — كائنٌ واحدٌ للجميع."""
    return to_lf(blob)


eol_to_lf.residue = None          # يُملأُ أدناه بعدَ تعريفِ الرايات


# ═══ الأثرُ يُشتقُّ من المجسِّ ولا يُكتَبُ مرّتَين ══════════════════════════
# (AR) 🔑 **الصمتُ يُصرَّحُ به ولا يُورَثُ بالإغفال.** مجسٌّ بلا تصريحِ أثرٍ
#      يبدو متتبَّعًا إن سجَّلَ مجسٌّ آخرُ المسارَ نفسَه بسِمةٍ أخرى — وقُيسَ:
#      قتلٌ في منتصفِه يتركُ بذرةً **مُودَعةً** مُفسَدةً، والحارسُ أخضرُ
#      والعدّاءُ أخضرُ وفحصُ الأثرِ أخضر. فالتصريحُ **لكلِّ مجسٍّ لا لكلِّ مسار**.
MIN_RESIDUE_MARK = 16
CREATED = object()      # ملفٌّ يُنشَأ — الدليلُ وجودُه
SELF_RED = object()     # الطفرةُ تُحمِّرُ الحارسَ بنفسِها — البوّابةُ تلتقطُها
NO_TRACE = object()     # لا أثرَ دلاليًّا — ويلزمُ تعليلٌ عندَ الإسناد


def sub(old: bytes, new: bytes, count: int = 1, residue: bool = False):
    def apply(blob: bytes) -> bytes:
        if blob is None or blob.count(old) < 1:
            raise AssertionError("المرساةُ غيرُ موجودة: %r" % old[:48])
        return blob.replace(old, new, count)
    apply.residue = new if residue else SELF_RED
    return apply


def append(extra: bytes):
    def apply(blob: bytes) -> bytes:
        return (blob or b"") + extra
    mark = extra.strip()
    if len(mark) < MIN_RESIDUE_MARK:
        raise AssertionError("سِمةُ أثرٍ أقصرُ من أن تكونَ دليلًا (%d < %d): %r"
                             % (len(mark), MIN_RESIDUE_MARK, mark))
    apply.residue = mark
    return apply


def create(body: bytes):
    def apply(_blob: bytes) -> bytes:
        return body
    apply.residue = CREATED
    return apply


def no_trace(reason: str):
    """(AR) صمتٌ **مُعلَّلٌ**: لا أثرَ دلاليًّا. والتعليلُ يُلزِمُ الكاتبَ ببرهانِه."""
    def wrap(fn):
        fn.residue = NO_TRACE
        fn.no_trace_reason = reason
        return fn
    return wrap


def self_red(fn):
    """(AR) الطفرةُ تُحمِّرُ الحارسَ بنفسِها — والبوّابةُ تلتقطُها إن بقيت."""
    fn.residue = SELF_RED
    return fn


eol_to_lf = no_trace(EOL_REASON)(eol_to_lf)


# ═══ سجلُّ الطيرانِ — شبكةٌ تعمُّ المجسّاتِ كلَّها ══════════════════════════
# (AR) 🔑 **`SELF_RED` دعوى تُكذَبُ في حالةٍ خضراءَ واقعيّة.** معناها «الطفرةُ
#      تُحمِّرُ الحارس»، وحمرتُها مشروطةٌ بأن يكونَ العدَّادُ على حدِّه: على
#      أرضيّةٍ فيها فجوةٌ **لا تُحمِّر**، فقتلٌ قاسٍ في منتصفِ المجسِّ يتركُ
#      الطفرةَ مُودَعةً وكلَّ فحصٍ أخضر. فالسجلُّ يُسمّي الملفَّ وبصمتَه **قبلَ**
#      الطفرةِ ويُمحى بعدَ استعادةٍ مُتحقَّقٍ منها — شبكةٌ لا تعتمدُ على تمييزِ
#      لفظٍ ولا على حمرةٍ مشروطة.
def git_dir() -> Path:
    """(AR) في شجرةٍ فرعيّةٍ يكونُ `.git` **ملفًّا**، فيُسأَلُ git ولا يُخمَّن."""
    try:
        proc = subprocess.run(["git", "rev-parse", "--absolute-git-dir"],
                              cwd=str(ROOT), capture_output=True, text=True,
                              encoding="utf-8", timeout=30)
        out = (proc.stdout or "").strip() if proc.returncode == 0 else ""
    except (OSError, subprocess.SubprocessError):
        out = ""
    return Path(out) if out else ROOT / ".git"


class Journal:
    """(AR) سجلُّ طيرانٍ واحدٌ لكلِّ مِحقنة — في مجلَّدِ git فلا يبلغُه
    `git clean -xfd` ولا يُودَعُ سهوًا."""

    def __init__(self, stem: str) -> None:
        self.path = git_dir() / ("_calibration_inflight_%s.json" % stem)

    def open(self, rel: str, before: str | None) -> None:
        self.path.parent.mkdir(parents=True, exist_ok=True)
        self.path.write_text(json.dumps({"probe": rel, "sha256": before},
                                        ensure_ascii=False), encoding="utf-8")

    def close(self) -> None:
        try:
            self.path.unlink(missing_ok=True)
        except OSError:
            pass

    def inflight(self) -> list[str]:
        """(AR) أثرُ تشغيلةٍ قُتِلَت في منتصفِ مجسّ — يُسمّى الملفُّ بعينِه."""
        if not self.path.is_file():
            return []
        try:
            rec = json.loads(self.path.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            return ["%s — سجلُّ طيرانٍ لا يُقرأ" % self.path.name]
        rel, before = rec.get("probe"), rec.get("sha256")
        target = ROOT / str(rel)
        if before is None:
            # (AR) ملفٌّ كان يُنشَأ — بقاؤه أثر، وغيابُه استعادةٌ تمّت.
            if target.exists():
                return ["%s — ملفُّ مجسٍّ باقٍ (سجلُّ طيران)" % rel]
        elif not target.is_file():
            return ["%s — الملفُّ مفقودٌ وسجلُّ الطيرانِ يذكرُه" % rel]
        elif hashlib.sha256(target.read_bytes()).hexdigest() != before:
            return ["%s — بقيَ **مُطفَّرًا**: بصمتُه تُخالِفُ سجلَّ الطيران" % rel]
        self.close()          # استُعيدَ فعلًا وبقيَ السجلُّ وحدَه
        return []


# ═══ قارئُ الحدودِ — سقفٌ وأرضيّةٌ في صورتَيهما ════════════════════════════
# (AR) 🔑 **حدٌّ فيه فجوةٌ يُوقِفُ العيارَ ولا يُخفِقُه.** مجسّاتُ «يجبُ أن
#      تحمرّ» تحقنُ بندًا واحدًا، فحمرتُها مشروطةٌ بأن يكونَ العدَّادُ على حدِّه.
#      والفجوةُ في **الأرضيّةِ** مرَّت مرّةً بـ«١٥/١٥ ورمزِ صفر» لأنّ القارئَ
#      كان يشترطُ لفظَ «السقف»؛ ومرَّت ثانيةً في أرضيّةٍ **بلا سقفٍ أصلًا**
#      لأنّه لم يعرفْ صورتَها. فالصورُ الثلاثُ تُقرَأُ ههنا مرّةً واحدةً للجميع.
# (AR) و«المسموح» لفظُ سقفٍ عندَ حارسٍ آخرَ ولفظُ «السقف» عندَ ثالث — والقلبُ
#      يقرأُ اللفظَين: قارئٌ يعرفُ هجاءَ حارسٍ واحدٍ يعمى عن حدودِ إخوتِه صامتًا.
_BOUND = re.compile(r"(\d+)\s*\((?:السقف|المسموح)\s*(\d+)")
_FLOOR = re.compile(r"(\d+)\s*\((?:السقف|المسموح)\s*\d+\s*·\s*الأرضيّة\s*(\d+)")
# (AR) ولا يُشترَطُ قوسٌ مباشرٌ بعدَ الرقم: حارسٌ يكتبُ «(الأرضيّة N — تصعدُ
#      ولا تهبط)» كان يسقطُ من القراءةِ **بالصدفةِ الهجائيّةِ** لا بإعلانٍ.
_FLOOR_ONLY = re.compile(r"(\d+)\s*\(الأرضيّة\s*(\d+)")


class Harness:
    """(AR) مِحقنةُ عيارٍ: تُعلِنُ حارسَها ومجسّاتِها، ويتولّى القلبُ الباقي."""

    def __init__(self, *, guard: Path, harness: Path, record: Path,
                 title: str, probes: tuple, min_probes: int,
                 baseline=None, floors: str = "none",
                 has_bounds: bool = True, version: int = 3) -> None:
        self.guard = Path(guard)
        self.harness = Path(harness)
        self.record = Path(record)
        self.title = title
        self.probes = probes
        self.min_probes = min_probes
        # (AR) قياسُ المرجعِ **قبلَ أوّلِ طفرة**: يُطفَّرُ من المقيسِ لا من
        #      المُعلَن، وإلّا صارَ المجسُّ يقيسُ الهجاءَ الذي يُطفِّرُه.
        self.baseline = baseline
        # (AR) 🔑 **والأرضيّاتُ ثلاثةُ أصنافٍ لا صنفان، والإعلانُ يُحاكَمُ.**
        #      وقِيسَ لِمَ: التوحيدُ شدَّ أرضيّاتِ حارسِ المرساةِ الخمسَ — وهي
        #      **مصمَّمةٌ للصعود** (ترويسةُ مِحقنتِها تقولُ ذلك حرفًا، ومُطفِّرُها
        #      يحسبُ `addresses - floor + 1` ليعضَّ فوقَ الهامش). فأوّلُ قاعدةٍ
        #      أو رمزِ خطأٍ يُضافُ كان سيُوقِفُ عيارَها برمزِ ٢ بلا مخرَجٍ إلّا
        #      رفعَ الأرضيّة — أي **قفلُ البوّابةِ على العملِ الذي وُجِدَ الحارسُ
        #      ليُنجِزَه**. والصنفُ يُعلَنُ ويُقاسُ:
        #        · "clamp"  الأرضيّةُ مشدودةٌ ويجبُ أن تُقرَأ (وإلّا فالقارئُ أعمى)
        #        · "slack"  مصمَّمةٌ للصعودِ فلا تُقاسُ ههنا — ويُطفَّرُ من المقيس
        #        · "none"   لا أرضيّةَ في المخرَجِ أصلًا؛ وظهورُ واحدةٍ إخفاقٌ
        if floors not in ("clamp", "slack", "none"):
            raise AssertionError("إعلانُ أرضيّةٍ غيرُ معروف: %r" % floors)
        self.floors = floors
        # (AR) وحارسٌ حدودُه صفريّةٌ بطبعِها (لا سقفَ ينزلُ ولا أرضيّةَ تصعد)
        #      يُعلِنُ ذلك — **والإعلانُ يُقاسُ في الاتّجاهَين**: إن ظهرَ حدٌّ
        #      في مخرَجِه أخفقَ العيار، فلا يصيرُ الإعلانُ بابًا لتخطّي الشدّ.
        self.has_bounds = has_bounds
        self.version = version
        self.journal = Journal(self.harness.stem)
        # (AR) و`baseline=None` إعلانٌ يُقاسُ كذلك: نصُّ الانتظارِ **القابلُ
        #      للنداء** هو وحدَه ما يلزمُه مرجعٌ مقيسٌ قبلَ الطفرة، فوجودُه بلا
        #      مرجعٍ عطبُ إعلانٍ يُقالُ عندَ البناءِ لا `KeyError` عندَ المجسّ.
        if baseline is None:
            needy = [entry[0] for entry in probes
                     if callable(entry[4] if not isinstance(entry[4], tuple)
                                 else entry[4][0])]
            if needy:
                raise AssertionError(
                    "مجسّاتٌ تنتظرُ مرجعًا مقيسًا و`baseline=None`: %s" % needy)

    # ── تشغيلُ الحارس ────────────────────────────────────────────────────
    def run_guard(self) -> tuple[int, str]:
        env = dict(os.environ, PYTHONIOENCODING="utf-8", PYTHONUTF8="1")
        proc = subprocess.run([sys.executable, str(self.guard)],
                              capture_output=True, text=True, encoding="utf-8",
                              cwd=str(ROOT), env=env)
        return proc.returncode, (proc.stdout or "") + (proc.stderr or "")

    def run_eol_invariance(self) -> tuple[int, str]:
        """(AR) يمرُّ بـ`sha_norm` **كاتبةِ السجلِّ بعينِها** لا بطبقةٍ تحتَها."""
        blob = self.guard.read_bytes()
        as_lf = sha_norm(self.guard)
        self.guard.write_bytes(blob.replace(LF_, CRLF))
        as_crlf = sha_norm(self.guard)
        self.guard.write_bytes(blob)
        if as_lf == as_crlf:
            return 0, "بصمةٌ ثابتةٌ عبرَ نهاياتِ الأسطر: %s" % as_lf[:16]
        return 1, "انجرافٌ بنهايةِ السطر: LF=%s ≠ CRLF=%s" % (as_lf[:16],
                                                              as_crlf[:16])

    # ── الحدودُ المشدودة ─────────────────────────────────────────────────
    def slack_bounds(self) -> list[str]:
        code, out = self.run_guard()
        if code != 0:
            return ["الحارسُ ليس أخضرَ قبلَ العيار (رمز=%d)" % code]
        lines = [L for L in out.split(chr(10))
                 if _BOUND.search(L) or _FLOOR_ONLY.search(L)]
        if not self.has_bounds:
            return ([] if not lines else
                    ["الحارسُ يُعلِنُ «بلا حدود» ومخرَجُه يحملُ %d حدًّا: %s"
                     % (len(lines), lines[0].strip())])
        if not lines:
            return ["لم يُقرأْ حدٌّ واحدٌ من مخرَجِ الحارس — قارئُ الحدودِ أعمى"]
        floor_lines = [L for L in lines
                       if _FLOOR.search(L) or _FLOOR_ONLY.search(L)]
        if self.floors == "none":
            return ([] if not floor_lines else
                    ["الحارسُ يُعلِنُ «بلا أرضيّات» ومخرَجُه يحملُ %d: %s"
                     % (len(floor_lines), floor_lines[0].strip())])
        if self.floors == "clamp" and not floor_lines:
            return ["لم تُقرأْ أرضيّةٌ واحدة — صفوفُها خارجَ المرساة"]
        slack = []
        for line in lines:
            for measured, bound in _BOUND.findall(line):
                if measured != bound:
                    # (AR) ويُسمّى **السطرُ**: زوجُ أرقامٍ بلا اسمٍ لا يدلُّ
                    #      على أيِّ لامتغيِّرٍ فيه الفجوة.
                    slack.append("%s  (مقيسٌ %s ≠ سقفٌ %s)"
                                 % (line.strip(), measured, bound))
            if self.floors != "clamp":
                continue
            for pattern in (_FLOOR, _FLOOR_ONLY):
                for measured, floor in pattern.findall(line):
                    if measured != floor:
                        slack.append("%s  (مقيسٌ %s ≠ أرضيّةٌ %s)"
                                     % (line.strip(), measured, floor))
        return slack

    # ── الأثرُ يُشتقُّ من المجسّات ─────────────────────────────────────────
    def derive_residue(self) -> tuple:
        marks = []
        for entry in self.probes:
            name, rel, mutate = entry[0], entry[1], entry[2]
            mark = getattr(mutate, "residue", None)
            if mark is None:
                raise AssertionError(
                    "مجسٌّ بلا تصريحِ أثر: %s — صرِّحْ به أو علِّلْ صمتَه" % name)
            marks.append((name, rel, mark))
        return tuple(marks)

    def residue(self) -> list[str]:
        dirty = self.journal.inflight()
        for name, rel, mark in self.derive_residue():
            target = ROOT / rel
            if mark is CREATED:
                if target.exists():
                    dirty.append("%s — ملفُّ مجسٍّ باقٍ (%s)" % (rel, name))
                continue
            if mark in (SELF_RED, NO_TRACE) or not target.is_file():
                continue
            if mark in target.read_bytes():
                # (AR) والسِّمةُ تُطبَع: اسمُ المجسِّ يقولُ **أيَّ** أثرٍ، والبايتاتُ
                #      تقولُ **ما** يُبحَثُ عنه — والمُصلِحُ يحتاجُ الاثنَين.
                dirty.append("%s — أثرُ طفرةٍ باقٍ (%s): %r"
                             % (rel, name, mark[:40]))
        return dirty

    # ── المجسُّ: حقنٌ ثمّ استعادةٌ مُتحقَّقٌ منها ─────────────────────────
    def probe(self, path: str, mutate, want_code: int, want_text,
              runner=None) -> tuple[bool, str]:
        target = ROOT / path
        existed = target.exists()
        original = target.read_bytes() if existed else None
        before = hashlib.sha256(original).hexdigest() if existed else None
        # (AR) والمجلَّدُ المُنشَأُ أثرٌ كالملفّ: هدفُ مجسٍّ قد يقعُ في مجلَّدٍ
        #      لا وجودَ له في الشجرة، فتركُه فارغًا يتركُ بصمةً لا يشتقُّها أحد.
        # (AR) و`mkdir(parents=True)` قد يُنشئُ **عدّةَ** مستوياتٍ، فتُحصى
        #      قبلَ الإنشاءِ وتُزالُ كلُّها — لا مستوًى واحدًا يترك الباقيَ أثرًا.
        made_dirs = []
        probe_dir = target.parent
        while not probe_dir.is_dir() and probe_dir != ROOT:
            made_dirs.append(probe_dir)
            probe_dir = probe_dir.parent
        try:
            target.parent.mkdir(parents=True, exist_ok=True)
            self.journal.open(path, before)
            target.write_bytes(mutate(original))
            code, out = (runner or self.run_guard)()
        finally:
            # (AR) 🔑 **الإغلاقُ ههنا لا بعدَ الكتلة.** كان بعدَها، فأيُّ
            #      استثناءٍ داخلَ المجسّ (مرساةُ حقنٍ زالت بعملٍ مشروع) يتركُ
            #      السجلَّ مفتوحًا ببصمةِ لحظتِه — والاستعادةُ تمَّت في
            #      `finally` — فيصيرُ كلُّ تشغيلةٍ تاليةٍ **رفضًا كاذبًا على
            #      شجرةٍ نظيفةٍ بلا مخرج**. وقعَ فعلًا وأحمرَ اختبارًا.
            if existed:
                target.write_bytes(original)      # type: ignore[arg-type]
                restored = (hashlib.sha256(target.read_bytes()).hexdigest()
                            == before)
            else:
                target.unlink(missing_ok=True)
                for stale in made_dirs:            # من الأعمقِ إلى الأعلى
                    try:
                        stale.rmdir()              # يخفقُ إن لم يكنْ فارغًا
                    except OSError:
                        break
                restored = not target.exists()
            if restored:
                self.journal.close()
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
        if callable(want):
            want = want()
        ok = (code == want_code) and (want in out)
        detail = "رمز=%d (منتظَر %d)" % (code, want_code)
        if want not in out:
            detail += " · النصُّ المنتظَرُ غائب"
        stray = [mark for mark in forbidden if mark in out]
        if stray:
            ok = False
            detail += " · حمرةٌ من لامتغيِّرٍ آخر: %s" % " ".join(stray)
        return ok, detail

    # ── السجلّ ───────────────────────────────────────────────────────────
    def write_record(self, passed: int, stamp: str) -> None:
        self.record.parent.mkdir(parents=True, exist_ok=True)
        harness_rel = self.harness.relative_to(ROOT).as_posix()
        guard_rel = self.guard.relative_to(ROOT).as_posix()
        lines = [
            "# ═══════════════════════════════════════════════════════════════════",
            "# سجلُّ عيارٍ — مُشتَقٌّ آليًّا، لا يُحرَّرُ باليد.",
            "# ⚠️ إن تغيّرَ الحارسُ أو المِحقنةُ أو هدفُ مجسٍّ ولم يُعَدِ العيارُ",
            "#    حمِرَ check_calibration_fresh.py. وإعادتُه:",
            "#    python %s --record --date=YYYY-MM-DD" % harness_rel,
            "# ═══════════════════════════════════════════════════════════════════",
            "version: %d" % self.version,
            "guard: %s" % guard_rel,
            "harness: %s" % harness_rel,
            "guard_sha256: %s" % sha_norm(self.guard),
            "harness_sha256: %s" % sha_norm(self.harness),
            "calibrated_at: %s" % stamp,
            "probes_total: %d" % len(self.probes),
            "probes_passed: %d" % passed,
            "result: %d/%d" % (passed, len(self.probes)),
        ]
        # (AR) 🔑 **وأهدافُ المجسّاتِ الثالثةُ تُبصَمُ كذلك.** السجلُّ كان يبصمُ
        #      الحارسَ والمِحقنةَ وحدَهما، ومراسي بعضِ المجسّاتِ نصوصٌ تعيشُ في
        #      ملفٍّ **ثالث**. فإعادةُ صياغةِ ذلك السطرِ هناك لا تُحرِّكُ
        #      البصمتَين، فيبقى السجلُّ «مُعايَرًا» ومجسٌّ صارَ لا ينطبقُ أصلًا.
        #      والمعدومُ يُعلَنُ لا يُسكَتُ عنه: هدفٌ يُنشِئُه المجسُّ نفسُه لا
        #      وجودَ له قبلَ التشغيل.
        # (AR) 🔑 **والقلبُ المشترَكُ في الصفِّ كذلك.** التعميمُ جعلَه ملفًّا
        #      رابعًا تعتمدُ عليه كلُّ مِحقنة، والسجلُّ يبصمُ الحارسَ والمِحقنةَ
        #      وحدَهما — فتبديلُ سطرٍ ههنا كان سيُبطِلُ عيارَ الخمسةِ جميعًا
        #      **بلا أن يحمرَّ شيء**. وهو عينُ الثقبِ الذي سدَّته بصماتُ
        #      الأهدافِ الثالثة، يُعيدُه التعميمُ من بابٍ آخرَ إن أُغفِل.
        core_rel = Path(__file__).resolve().relative_to(ROOT).as_posix()
        third = sorted(({entry[1] for entry in self.probes} | {core_rel})
                       - {guard_rel, harness_rel})
        lines.append("targets_sha256:")
        for rel in third:
            path = ROOT / rel
            lines.append("  %s: %s"
                         % (rel, sha_norm(path) if path.is_file() else "(معدوم)"))
        lines.append("probes:")
        for entry in self.probes:
            name, rel, _mutate, code, text = entry[:5]
            role = ("انحدار — يجبُ أن يبقى أخضر" if code == 0
                    else "عطبُ آلة" if code == 2 else "يجبُ أن يحمرّ")
            want = text[0] if isinstance(text, tuple) else text
            if callable(want):
                want = want()
            lines.append('  - name: "%s"' % name)
            lines.append("    target: %s" % rel)
            lines.append("    expect_exit: %d" % code)
            lines.append('    expect_text: "%s"' % want)
            lines.append('    role: "%s"' % role)
        self.record.write_text("\n".join(lines) + "\n", encoding="utf-8",
                               newline="\n")

    # ── المدخل ───────────────────────────────────────────────────────────
    def main(self, argv: list[str] | None = None) -> int:
        sys.stdout.reconfigure(encoding="utf-8")
        argv = list(sys.argv[1:] if argv is None else argv)
        record = "--record" in argv
        stamp = ""
        for arg in argv:
            if arg.startswith("--date="):
                stamp = arg.split("=", 1)[1]
        if record and stamp and not is_date(stamp):
            print("✗ --date شكلُه YYYY-MM-DD — و%r ليس تاريخًا." % stamp)
            return 2
        if record and not stamp:
            print("✗ --record يلزمُه --date=YYYY-MM-DD (لا يُقرأُ وقتُ النظام)")
            return 2

        print(self.title)
        print("%-50s %-9s %s" % ("المجسّ", "النتيجة", "التفصيل"))
        print("-" * 88)

        dirty = self.residue()
        if dirty:
            print("✗ عطبُ آلة: أرضيّةٌ ملوّثةٌ بأثرِ تشغيلةٍ سابقةٍ لم تُنهَ:")
            for item in dirty:
                print("    · %s" % item)
            # (AR) والمخرجُ يُسمّى: السجلُّ في مجلَّدِ git فلا يبلغُه
            #      `git clean -xfd`، و`git checkout` وحدَه يُتلِفُ تحريرًا
            #      مشروعًا ولا يمحوه.
            print("    ⤷ العلاج: استعِدِ الملفَّ المذكورَ من الإيداع، ثمّ احذفْ")
            print("      %s" % self.journal.path)
            return 2

        if self.baseline is not None:
            self.baseline()          # المرجعُ يُقاسُ قبلَ أوّلِ طفرة
        slack = self.slack_bounds()
        if slack:
            print("✗ عطبُ آلة: حدٌّ فيه فجوةٌ — المجسّاتُ لا تعضُّ على هامش:")
            for item in slack:
                print("    · %s" % item)
            print("    ⤷ العلاج: أنزِلِ السقفَ إلى المقيسِ (أو ارفعِ الأرضيّة)"
                  " في الحارسِ ثمّ أعِدِ العيار — وهو ما يُوجِبُه «نازلٌ لا"
                  " يُرفَع» أصلًا.")
            return 2

        if len(self.probes) < self.min_probes:
            print("✗ عطبُ آلة: %d مجسًّا < %d — مِحقنةٌ قُلِّصت، فلا تُعلِنُ نجاحًا."
                  % (len(self.probes), self.min_probes))
            return 2

        passed = 0
        for entry in self.probes:
            name, path, mutate, code, text = entry[:5]
            ok, detail = self.probe(path, mutate, code, text,
                                    entry[5] if len(entry) > 5 else None)
            passed += 1 if ok else 0
            print("%-50s %-9s %s" % (name, "عضَّ" if ok else "لم يعضّ", detail))
        print("-" * 88)
        print("النتيجة: %d/%d" % (passed, len(self.probes)))

        code, out = self.run_guard()
        tail = [ln for ln in out.strip().split("\n") if ln.strip()][-1:]
        print("وبعدَ الاستعادة: رمز=%d · %s"
              % (code, tail[0].strip() if tail else ""))

        if passed != len(self.probes) or code != 0:
            print("✗ لا يُودَعُ سجلُّ عيارٍ ناقص.")
            return 1
        if record:
            self.write_record(passed, stamp)
            print("✓ أُودِعَ السجلُّ: %s"
                  % self.record.relative_to(ROOT).as_posix())
        return 0


def run(harness: Harness) -> None:
    """(AR) المدخلُ الموحَّد — وكلُّ انهيارٍ **عطبُ آلةٍ لا حكم** (رمزُ ٢)."""
    try:
        raise SystemExit(harness.main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001
        sys.stdout.reconfigure(encoding="utf-8")
        print("✗ عطبُ آلةٍ — لم يُقَسْ شيء: %s: %s"
              % (exc.__class__.__name__, exc))
        raise SystemExit(2)
