#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) عيارُ البراهين: **بذرةٌ لا تحمرُّ حينَ يُكسَرُ ما تُثبِتُه ليست برهانًا**.

     فعلْنا هذا للحرّاسِ (يُحقَنُ عطبٌ فيُثبَتُ الاحمرار)، وهذا هو الشيءُ نفسُه
     مطبَّقًا على **البذور**. وبذرةٌ خضراءُ تُقرأُ برهانًا، ولا يُميِّزُ أحدٌ بينَ
     خضرةٍ تعني «العقدُ وُفِّيَ» وخضرةٍ تعني «لا عقدَ يُوفَّى».

     مِحقنتانِ مختلفتا الطبيعة:

       ① **عيارُ العقد** — يُفسَدُ `@expected` نفسُه. البذرةُ **يجبُ أن تحمرّ**.
          🔑 وهو **برهانٌ بشرطِ أن تكونَ المادّةُ نقيّة**: العقدُ صارَ يُخالِفُ
             المخرَجَ يقينًا، فخضرةٌ بعدَه تعني أنّ العقدَ لا يُقابَلُ بشيء.
          ⚠️ **والشرطُ ليس مجّانيًّا**: بذرةٌ مُبوَّبةٌ (`@posix_only` ·
             `@windows_only` · `@arch` · `@known_red`) يُعلِنُها العدّاءُ تخطّيًا
             أو حمراءَ معلومةً **ورمزُ خروجِه صفرٌ في الحالَين** — فهي طفرةٌ
             مكافئةٌ مضمونة. وقِيسَ في المادّةِ عشرونَ منها، فاستُبعِدت.

       ② **طفرةُ الشفرة** — يُبدَّلُ حرفيٌّ عدديٌّ في جسمِ البذرة. يجبُ أن تحمرَّ
          أيضًا (بفشلِ مقابلةٍ أو بخطأِ ترجمة — كلاهما كشف).
          ⚠️ **وهذا المجسُّ حدٌّ أعلى لا برهان**: طفرةٌ قد تكونُ **مكافئةً** فلا
             تُغيِّرُ المخرَجَ، فتُقرأُ البذرةُ «لا تكشف» وهي بريئة. فالعددُ
             الخارجُ منه **أكبرُ أو يساوي** العددَ الحقيقيّ، ويُقالُ كذلك.

     🔑 **والعيّنةُ حتميّةٌ لا عشوائيّة**: تُختارُ بخطوةٍ ثابتةٍ على ترتيبٍ مفروزٍ،
        فتشغيلتانِ بالوسائطِ نفسِها تُعطيانِ العيّنةَ نفسَها. وعشوائيّةٌ بلا بذرةٍ
        مثبَّتةٍ تجعلُ الرقمَ غيرَ قابلٍ لإعادةِ الإنتاج.

     الاستعمال:
       python scripts/codegen/calibrate_seed_proofs.py --sample 25
       python scripts/codegen/calibrate_seed_proofs.py --sample 25 --record --date=YYYY-MM-DD

(EN) Proof calibration: a seed that stays green when what it proves is broken is
     not a proof. Two mutators — corrupting the contract (sound: no equivalent
     mutants) and mutating a numeric literal (an upper bound: some mutants are
     equivalent). The sample is deterministic, never random.
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

sys.path.insert(0, str(Path(__file__).resolve().parent))

ROOT = Path(__file__).resolve().parents[2]
SEEDS = ROOT / "tests" / "behavior"
RUNNER = ROOT / "tests" / "runner.py"

# (AR) 🔑 **الثنائيُّ يُسمّى ولا يُترَكُ للمصادفة.** العدّاءُ يقعُ افتراضًا على
#      `build/bin/Debug/sad-build.exe`، وقد يكونُ **أقدمَ من شفرةِ المترجّم**
#      فيُنتِجُ حمرةً كاذبةً تُقرأُ عطبًا في الشجرة. (قِيسَ: ثنائيُّ Debug أقدمُ
#      من رقعةِ مترجّمٍ بستِّ ساعاتٍ فأحمرَّت بذرةٌ خضراءُ بأيِّ بناءٍ طازج.)
#      والدرسُ مُدوَّنٌ ثلاثَ مرّات: «أحمرُ محلّيٌّ وأخضرُ CI ⇒ اتّهمِ الأثر».
#      فيُمرَّرُ `--compiler` صراحةً، **وتُسجَّلُ بصمةُ الثنائيِّ في القياس** —
#      وإلّا فالرقمُ لا يُعادُ إنتاجُه ولا يُكذَّب.
DEFAULT_COMPILER = "build/bin/Release/sad-build.exe"
# (AR) 🔑 **قياسٌ لا عيار — فلا يسكنُ مجلَّدَ العيار.**
#      `calibration/` يحرسُه `check_calibration_fresh.py` ويشترطُ في كلِّ
#      ملفٍّ فيه حقلَي `guard`/`harness` وبصمتَيهما. وهذا الملفُّ **قياسٌ**
#      لا سجلُّ عيار: نتيجتُه تعتمدُ على ثنائيِّ المترجّمِ القائمِ لا على
#      بصمةِ شفرة. ووضعُه هناك جعلَه **سجلًّا يتيمًا** فحمِرَ الفوقيُّ —
#      وأمسكَ خطأً في التصميمِ لا في المحتوى.
RECORD_DIR = ROOT / "scripts" / "codegen" / "measurements"
RECORD = RECORD_DIR / "seed_proofs.yaml"

SKIP_PARTS = ("_archive",)
# (AR) 🔑 قارئُ العقدِ **يُورَثُ** من الحارسِ ولا يُكتَبُ ثالثةً: `[: \t]+`
#      صنفُ محارفَ لا تسلسل، فكان يقبلُ `@expected:` بفراغٍ زائلٍ حمولةً —
#      فيُلحِقُ المُطفِّرُ عقدًا صحيحًا حيثُ لا عقدَ **فيُصلِحُ ما جاءَ ليكشف**.
from check_seed_contract import _EXPECTED  # noqa: E402  (قارئٌ واحدٌ للعقد)
_SKIP = re.compile(r"^#[ \t]*@skip_compiler", re.M)
_NEGATIVE = re.compile(r"^#[ \t]*@expect(?:_compile)?_error[a-z_]*", re.M)
# (AR) 🔑 **بذرةٌ مُبوَّبةٌ قد تخرجُ بصفرٍ مهما أُفسِدَ عقدُها**: يُعلِنُها العدّاءُ
#      تخطّيًا (منصّةٌ أخرى · معماريّةٌ أخرى) أو حمراءَ معلومة، ورمزُ الخروجِ صفرٌ
#      في الحالَين. فهي حينَها طفرةٌ مكافئةٌ **مضمونةٌ لا محتملة**، وإدخالُها في
#      المادّةِ يجعلُ «عقدٌ مُفسَد» يتّهمُ بريئةً — ورمزُ الخروجِ يحكمُ عليه
#      فيصيرُ **رفضًا كاذبًا**.
#
#      ⚠️ **والاستبعادُ بالوسمِ أوسعُ من دعواه، ويُقالُ كما هو**: الوسمُ يُبوِّبُ
#         على منصّةٍ ولا يُبوِّبُ على أخرى — فـ`@windows_only` يُشغَّلُ ويُحاكَمُ
#         على ويندوز (مقيسًا: أربعُ بذورٍ) و`@arch: x86` على معالجٍ x86.
#         فالثمنُ **تغطيةٌ ضائعةٌ لا حكمٌ كاذب**: الاستبعادُ أحاديُّ الاتّجاهِ
#         يُصغِّرُ المادّةَ ولا يُنتِجُ اتّهامًا.
#      ولمَ بالوسمِ لا بحالةِ البذرةِ في تقريرِ العدّاء: الحالةُ أدقُّ (تستبعدُ
#      بالمقيسِ لا بالمتوقَّع وتلتقطُ وسمًا خامسًا يُولَدُ غدًا)، لكنّ مسارَ
#      التقريرِ ثابتٌ لا يقبلُ تجاوزًا فتشغيلتانِ متزامنتانِ تدهسُ إحداهما
#      الأخرى. والوسمُ **حتميٌّ عبرَ المنصّاتِ** فالعيّنةُ لا تتبدّلُ بمكانِ
#      التشغيل. (والانتقالُ إلى الحالةِ يلزمُه `--report-path` في العدّاءِ أوّلًا.)
_GATED = re.compile(r"^#[ \t]*@(posix_only|windows_only|arch|known_red)\b", re.M)
# (AR) حرفيٌّ عدديٌّ في **جسمِ** البذرةِ لا في تعليقٍ — والتعليقُ لا يُترجَم.
_NUMBER = re.compile(r"^(?!\s*#)(.*?)(?<![\w.])([0-9]+)(?![\w.])", re.M)


def _git(*args: str) -> str:
    """(AR) مخرَجُ أمرِ git مقتطَعًا — وفراغٌ إن تعذَّر، فلا يُوقِفُ القياسَ."""
    try:
        proc = subprocess.run(["git", *args], cwd=str(ROOT), capture_output=True,
                              text=True, encoding="utf-8", timeout=30)
    except (OSError, subprocess.SubprocessError):
        return ""
    return (proc.stdout or "").strip() if proc.returncode == 0 else ""


def _sha_sample(seeds: list[Path]) -> str:
    """(AR) بصمةُ العيّنةِ: مسارٌ نسبيٌّ فبصمةُ بايتاتِه، بترتيبٍ ثابت."""
    digest = hashlib.sha256()
    for seed in seeds:
        digest.update(seed.relative_to(ROOT).as_posix().encode("utf-8"))
        digest.update(b"\x00")
        digest.update(hashlib.sha256(seed.read_bytes()).digest())
    return digest.hexdigest()


def _sha_file(path: Path) -> str:
    """(AR) بصمةُ الثنائيِّ — بلا تطبيعِ أسطرٍ فهو ثنائيٌّ لا نصّ."""
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _is_date(stamp: str) -> bool:
    if not stamp.isascii():
        return False
    try:
        date.fromisoformat(stamp)
    except ValueError:
        return False
    return True


def _candidates() -> list[Path]:
    """(AR) بذورٌ لها عقدٌ موجبٌ صريحٌ وتُشغَّل — وحدَها تصلحُ مادّةَ عيار."""
    out: list[Path] = []
    # (AR) الفرزُ بالمسارِ النصّيِّ لا بكائنِ `Path`: مقارنةُ `Path` على ويندوز
    #      بلا حساسيّةِ حالةٍ وعلى POSIX خامّة، فالترتيبانِ يختلفانِ والعيّنةُ
    #      «الحتميّةُ» لا تُعادُ إنتاجُها عبرَ المنصّات.
    for path in sorted(SEEDS.rglob("*.ص"), key=lambda p: p.as_posix()):
        if any(part in SKIP_PARTS for part in path.parts):
            continue
        text = path.read_text(encoding="utf-8-sig")
        if _SKIP.search(text) or _NEGATIVE.search(text) or _GATED.search(text):
            continue
        if _EXPECTED.search(text):
            out.append(path)
    return out


def _sample(rows: list[Path], size: int) -> list[Path]:
    """(AR) عيّنةٌ **حتميّةٌ** بخطوةٍ ثابتةٍ — لا عشوائيّةَ بلا بذرةٍ مثبَّتة."""
    if size >= len(rows):
        return rows
    stride = len(rows) / float(size)
    return [rows[int(i * stride)] for i in range(size)]


def _run_seed(path: Path, compiler: Path) -> int:
    """(AR) يُشغِّلُ العدّاءَ على بذرةٍ واحدةٍ ويعيدُ رمزَ الخروج.

    🔑 **بالمسارِ النسبيِّ لا بالاسمِ القاعديّ.** العدّاءُ يقبلُ الاسمَ ويحلُّه
       بـ`rglob` فيأخذُ **أوّلَ** مطابقة — وفي الشجرةِ ٤٦٣ اسمًا مكرَّرًا يمسُّ
       ١٠٤٦ ملفًّا (`023_extra_23.ص` ستَّ مرّات). فكانت المِحقنةُ تُطفِّرُ ملفًّا
       وتُشغِّلُ غيرَه، فتُعلِنُ بذرةً بريئةً «برهانًا أجوف».
       وهو عينُ الدرسِ المُدوَّن: **أداةُ القياسِ تُولِّدُ أعطابًا**.
    """
    env = dict(os.environ, PYTHONIOENCODING="utf-8", PYTHONUTF8="1")
    rel = path.resolve().relative_to(SEEDS.resolve()).as_posix()
    proc = subprocess.run([sys.executable, str(RUNNER), "--file", rel,
                           "--no-color", "--compiler", str(compiler)],
                          capture_output=True, text=True, encoding="utf-8",
                          cwd=str(ROOT), env=env, timeout=300)
    return proc.returncode


# ═══ المُطفِّران ════════════════════════════════════════════════════════════
def _corrupt_contract(text: str) -> str | None:
    """(AR) يُفسِدُ `@expected` يقينًا: يُلحِقُ سطرًا لا يُنتِجُه أيُّ برنامج."""
    match = _EXPECTED.search(text)
    if not match:
        return None
    # (AR) و`"\\n"` خطٌّ مائلٌ ثمّ n لا سطرٌ جديد — عمدًا: العدّاءُ يقسمُ
    #      `@expected` على `r"\\n"` الحرفيِّ (`runner.py:326`)، ويقرأُ الوسمَ
    #      سطرًا سطرًا فلا يرى سطرًا حقيقيًّا لو أُدرِج. (قِيسَ وأُقِرّ.)
    # (AR) الإلحاقُ في **نهايةِ السطر**: القارئُ المورَّثُ من الحارسِ يقفُ
    #      عندَ بدايةِ الحمولةِ (تطلُّعٌ بلا التقاط) فلا مجموعةَ يُؤخَذُ منها موضع.
    line_end = text.find(chr(10), match.end())
    if line_end == -1:
        line_end = len(text)
    return text[:line_end] + chr(92) + "n" + RESIDUE_MARK + text[line_end:]


def _mutate_number(text: str) -> str | None:
    """(AR) يُبدِّلُ أوّلَ حرفيٍّ عدديٍّ في جسمِ البذرة (لا في تعليق)."""
    match = _NUMBER.search(text)
    if not match:
        return None
    start, end = match.span(2)
    return text[:start] + str(int(match.group(2)) + 1) + text[end:]


def _drop_print(text: str) -> str | None:
    """(AR) يحذفُ **آخرَ** سطرِ طباعةٍ في الجسم.

    🔑 وإشارتُه أعلى من طفرةِ العدد: حذفُ إخراجٍ يُغيِّرُ المخرَجَ بالبناءِ إلّا
       أن يكونَ السطرُ في شفرةٍ ميّتة. فالطفرةُ المكافئةُ ههنا نادرةٌ لا شائعة.
       (وطفرةُ العددِ أنتجت مكافئةً في أوّلِ تشغيلة: `ن = 3` ← `ن = 4` وكلاهما
        يسقطُ إلى `وإلا` فيطبعُ الشيءَ نفسَه — والبذرةُ بريئةٌ والمطفِّرُ ضعيف.)
    """
    lines = text.split(chr(10))
    for i in range(len(lines) - 1, -1, -1):
        stripped = lines[i].lstrip()
        if stripped.startswith("#") or "اطبع" not in stripped:
            continue
        return chr(10).join(lines[:i] + lines[i + 1:])
    return None


# (AR) 🔑 سِمةُ الأثر. و`finally` يحمي من الاستثناءِ ومن Ctrl-C ولا يحمي من
#      SIGKILL. وأثرُ العقدِ المُفسَدِ **صامتٌ تمامًا** إن سقطَ في `_regression/`
#      (لا يُشغِّلُه CI)، فبذرةٌ مُودَعةٌ تبقى مُفسَدةً بلا أن يقولَها أحد.
#      وأخواتُ هذه الأداةِ تملكُ فحصًا قبليًّا وهي كانت لا تملكُه.
RESIDUE_MARK = "ZZ_CONTRACT_CALIBRATION_MUST_FAIL"


# (AR) 🔑 **سجلُّ طيران**: يُكتَبُ اسمُ البذرةِ وبصمتُها الأصليّةُ **قبلَ** الطفرة،
#      ويُمحى بعدَ الاستعادةِ المُتحقَّقِ منها. فإن بقيَ فثمّةَ تشغيلةٌ لم تُنهَ
#      **وبذرةٌ مُودَعةٌ قد تكونُ مُفسَدة**، ويُسمّيها السجلُّ بعينِها.
#
#      ⚠️ ولا يُبحَثُ عن **سِمةٍ**: من المُطفِّراتِ الثلاثةِ واحدٌ يتركُ سِمةً،
#         و`_drop_print` يحذفُ سطرًا و`_mutate_number` يُبدِّلُ رقمًا — وكلاهما
#         بلا أثرٍ يُميَّز. وقتلٌ قاسٍ أثناءَ طفرةٍ **مكافئةٍ** (وهي ٤١ من ١٣٣
#         مقيسًا) يُودِعُ حرفيًّا مغلوطًا في بذرةٍ **تبقى خضراءَ إلى الأبد**.
#      ⚠️ ولا يُقاسُ بـ`git status`: ذاك يرفضُ كلَّ تعديلٍ غيرِ مُودَعٍ ولو كان
#         مشروعًا — **رفضٌ كاذبٌ** يمنعُ القياسَ على دفعةٍ تُصلِحُ بذرةً. (جُرِّبَ
#         فرفضَ رقعةً صحيحةً في هذه الدفعةِ نفسِها.) والسجلُّ يُسمّي **أثرَ
#         الأداةِ وحدَها**.
def _git_dir() -> Path:
    """(AR) 🔑 **مجلَّدُ git يُسأَلُ ولا يُخمَّن.** في شجرةٍ فرعيّةٍ (worktree)
    يكونُ `.git` **ملفًّا** لا مجلَّدًا، فـ`mkdir` يرمي `FileExistsError` ⇒ رمزُ
    ٢ عندَ أوّلِ طفرة: لا فسادَ لكن **لا قياسَ**، وسجلُّ الطيرانِ ميّتٌ في
    البيئةِ التي يستعملُها المستودعُ لتوازي الوكلاء. وأسوأُ منه أنّ فحصَ الأثرِ
    يقولُ «نظيف» هناك بلا تحذير."""
    out = _git("rev-parse", "--absolute-git-dir")
    return Path(out) if out else ROOT / ".git"


# (AR) وخارجَ `build/`: ذاك يُمحى بـ`x.py clean` وبـ`git clean -xfd`، فكان قتلٌ
#      قاسٍ ثمّ تنظيفٌ يمحو **الكاشفَ ويُبقي البذرةَ مُفسَدة** — فتقيسُ الأداةُ
#      فوقَ أثرِها وتُعلِنُ النجاح. ومجلَّدُ git لا يُودَعُ ولا يُمحى بتنظيفٍ.
JOURNAL = _git_dir() / "_seed_proofs_inflight.json"


def _journal_open(path: Path, original: bytes) -> None:
    JOURNAL.parent.mkdir(parents=True, exist_ok=True)
    JOURNAL.write_text(json.dumps({
        "seed": path.relative_to(ROOT).as_posix(),
        "sha256": hashlib.sha256(original).hexdigest(),
    }, ensure_ascii=False), encoding="utf-8")


def _journal_close() -> None:
    JOURNAL.unlink(missing_ok=True)


def _residue(pool: list[Path] | None = None) -> list[str]:
    """(AR) أثرُ تشغيلةٍ سابقةٍ لم تُنهَ — بذرةٌ بقيت مُطفَّرة.

    كاشفانِ لا واحد: **السجلُّ** يُسمّي البذرةَ بعينِها، و**السِّمةُ** تنجو ولو
    مُحِيَ السجلُّ. وكانت السِّمةُ `RESIDUE_MARK` **بلا قارئٍ في المستودعِ كلِّه**
    — رقمٌ يُكتَبُ ولا يُقابَل، وهو عينُ ما رُقِّعَ في بصمةِ الثنائيّ.
    """
    found: list[str] = []
    if pool is not None:
        needle = RESIDUE_MARK.encode("ascii")
        found += [p.relative_to(ROOT).as_posix() + " — سِمةُ عقدٍ مُفسَدٍ باقية"
                  for p in pool if needle in p.read_bytes()]
    if not JOURNAL.is_file():
        return found
    try:
        rec = json.loads(JOURNAL.read_text(encoding="utf-8"))
    except (ValueError, OSError):
        # (AR) و`JOURNAL.name` لا `relative_to(ROOT)`: في شجرةٍ فرعيّةٍ يقعُ
        #      مجلَّدُ git **خارجَ** الجذر، فيرمي `relative_to` ⇒ تُستبدَلُ رسالةُ
        #      الأثرِ بخطأِ مسارٍ في البيئةِ التي كُتِبَ لها هذا السجلّ.
        return [f"{JOURNAL.name} — سجلٌّ لا يُقرأ"]
    seed = ROOT / rec.get("seed", "")
    if not seed.is_file():
        return [f"{rec.get('seed')} — بذرةٌ من سجلِّ الطيرانِ غيرُ موجودة"]
    if hashlib.sha256(seed.read_bytes()).hexdigest() == rec.get("sha256"):
        # (AR) استُعيدت فعلًا وبقيَ السجلُّ وحدَه — يُمحى ولا يُوقِفُ القياس.
        _journal_close()
        return found
    return found + [
        f"{rec['seed']} — بقيت **مُطفَّرة**: بصمتُها تُخالِفُ سجلَّ الطيران"]


MUTATORS = (
    ("عقدٌ مُفسَد", _corrupt_contract, "برهان"),
    ("إخراجٌ محذوف", _drop_print, "شبهُ برهان"),
    ("طفرةُ عددٍ", _mutate_number, "حدٌّ أعلى"),
)


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    size = 25
    compiler = ROOT / DEFAULT_COMPILER
    record = "--record" in sys.argv
    stamp = ""
    for arg in sys.argv[1:]:
        if arg.startswith("--sample="):
            size = int(arg.split("=", 1)[1])
        elif arg == "--sample":
            idx = sys.argv.index(arg)
            size = int(sys.argv[idx + 1])
        elif arg.startswith("--compiler="):
            compiler = Path(arg.split("=", 1)[1])
            if not compiler.is_absolute():
                compiler = ROOT / compiler
        elif arg.startswith("--date="):
            stamp = arg.split("=", 1)[1]
    if record and (not stamp or not _is_date(stamp)):
        print("✗ --record يلزمُه --date=YYYY-MM-DD تاريخًا فعليًّا")
        return 2

    print("عيارُ البراهين — «بذرةٌ لا تحمرُّ حينَ يُكسَرُ ما تُثبِتُه ليست برهانًا»:")
    pool = _candidates()
    dirty = _residue(pool)
    if dirty:
        print("  ✗ عطبُ آلة: أرضيّةٌ ملوّثةٌ بأثرِ تشغيلةٍ سابقةٍ لم تُنهَ —"
              " لا يُقاسُ فوقَه، وبذرةٌ مُودَعةٌ باقيةٌ مُفسَدة:")
        for rel in dirty[:6]:
            print(f"      · {rel}")
        print("  نظِّفْ ثمّ أعِدْ: git checkout -- <الملفّ>")
        return 2
    if not compiler.is_file():
        print(f"  ✗ عطبُ آلة: لا ثنائيَّ في {compiler.relative_to(ROOT).as_posix()}"
              " — يُمرَّرُ بـ--compiler=<مسار>")
        return 2
    print(f"  الثنائيّ: {compiler.relative_to(ROOT).as_posix()}"
          f" · بصمة {_sha_file(compiler)[:16]}")
    chosen = _sample(pool, size)
    print(f"  مادّةُ العيار: {len(pool)} بذرةً ذاتَ عقدٍ موجبٍ · العيّنةُ الحتميّة:"
          f" {len(chosen)}")
    if len(pool) < 100:
        print(f"  ✗ عطبُ آلة: مادّةُ العيارِ {len(pool)} — والشجرةُ فيها آلاف.")
        return 2

    # (AR) 🔑 **عيارُ الأداةِ قبلَ القياس**: كلُّ بذرةٍ في العيّنةِ **خضراءُ اليومَ**
    #      بلا طفرة. وإلّا فالخضرةُ التي نقيسُ فقدَها ليست موجودةً أصلًا، وكلُّ
    #      «لم تحمرّ» يصيرُ بلا معنًى. (وبهذا يُمسَكُ خطأُ توجيهِ المسارِ الذي
    #      جعلَ المِحقنةَ تُطفِّرُ ملفًّا وتُشغِّلُ غيرَه.)
    healthy, unhealthy = [], []
    for path in chosen:
        (healthy if _run_seed(path, compiler) == 0 else unhealthy).append(path)
    if unhealthy:
        print(f"  · مستبعَدةٌ (ليست خضراءَ قبلَ الطفرة): {len(unhealthy)}"
              " — لا يُقاسُ فقدُ خضرةٍ لا وجودَ لها")
        for path in unhealthy[:4]:
            print(f"      · {path.relative_to(ROOT).as_posix()}")
    # (AR) أرضيّةٌ لا صفرٌ: لو أبقى ثنائيٌّ بائدٌ بذرةً واحدةً خضراءَ لأعلنت
    #      الأداةُ المُطفِّرَ سليمًا على عيّنةِ **واحد** ورجعت بصفر.
    # (AR) أرضيّةٌ **نسبيّةٌ** لا مطلقة: المطلقةُ تمنعُ عيّنةً صغيرةً مشروعةً
    #      (٣ &lt; ٥) وهو رفضٌ كاذب، والنسبيّةُ تعملُ في كلِّ حجمٍ وتبقى حارسة.
    floor = max(1, (len(chosen) * 2 + 2) // 3)
    if len(healthy) < floor:
        print(f"  ✗ عطبُ آلة: {len(healthy)} خضراءَ فقط من {len(chosen)}"
              f" (الأرضيّة {floor}) — الثنائيُّ مشبوهٌ لا الشجرة.")
        return 2
    print(f"  عيارُ الأداة: {len(healthy)}/{len(chosen)} خضراءُ قبلَ الطفرة ✓")
    chosen = healthy

    print()
    print("%-46s %-11s %-13s %s"
          % ("البذرة", "عقدٌ مُفسَد", "إخراجٌ محذوف", "طفرةُ عدد"))
    print("-" * 88)
    results: dict[str, list[str]] = {name: [] for name, _m, _k in MUTATORS}
    skipped: dict[str, int] = {name: 0 for name, _m, _k in MUTATORS}

    for path in chosen:
        original = path.read_bytes()
        before = hashlib.sha256(original).hexdigest()
        text = original.decode("utf-8-sig")
        marks = []
        for name, mutate, _kind in MUTATORS:
            mutated = mutate(text)
            if mutated is None:
                skipped[name] += 1
                marks.append("—")
                continue
            try:
                _journal_open(path, original)
                path.write_bytes(mutated.encode("utf-8"))
                code = _run_seed(path, compiler)
            finally:
                path.write_bytes(original)
            if hashlib.sha256(path.read_bytes()).hexdigest() != before:
                raise AssertionError("لم تُستعَدِ البايتاتُ في %s" % path.name)
            _journal_close()
            if code == 0:
                results[name].append(path.relative_to(ROOT).as_posix())
                marks.append("✗ خضراء")
            else:
                marks.append("عضَّت")
        print("%-46s %-11s %-13s %s"
              % (path.name[:46], marks[0], marks[1], marks[2]))

    print("-" * 88)
    total = len(chosen)
    print()
    for name, _m, kind in MUTATORS:
        vacuous = results[name]
        ran = total - skipped[name]
        print(f"  {name:<12} عُويِرَ {ran:>3} · **لم تحمرَّ**: {len(vacuous):>3}"
              f"  ({kind})")
        for rel in vacuous[:6]:
            print(f"      · {rel}")
        if len(vacuous) > 6:
            print(f"      … و{len(vacuous) - 6} غيرُها")

    print()
    print("  🔑 «عقدٌ مُفسَد» **برهانٌ**: العقدُ صارَ يُخالِفُ المخرَجَ يقينًا، فخضرةٌ")
    print("     بعدَه تعني أنّ العقدَ لا يُقابَلُ بشيء.")
    print("  · «إخراجٌ محذوف» **شبهُ برهان**: حذفُ إخراجٍ يُغيِّرُ المخرَجَ بالبناءِ")
    print("     إلّا أن يكونَ في شفرةٍ ميّتة.")
    print("  ⚠️ «طفرةُ عدد» **حدٌّ أعلى**: طفرةٌ قد تكونُ مكافئةً فلا تُغيِّرُ المخرَج،")
    print("     فتُقرأُ البذرةُ «لا تكشف» وهي بريئة. (قِيسَ مثالٌ في أوّلِ تشغيلة:")
    print("     `ن = 3` ← `ن = 4` وكلاهما يسقطُ إلى `وإلا`.)")

    if record:
        RECORD_DIR.mkdir(parents=True, exist_ok=True)
        lines = [
            "# ═══════════════════════════════════════════════════════════════",
            "# قياسُ عيارِ البراهين — مُشتَقٌّ آليًّا، لا يُحرَّرُ باليد.",
            "# ⚠️ ليس بصمةً: نتيجتُه تعتمدُ على ثنائيِّ المترجّمِ القائم، فيُعادُ",
            "#    قياسُه عندَ الحاجةِ ولا يُحرَسُ بانجرافِ بصمة.",
            "# ═══════════════════════════════════════════════════════════════",
            "version: 1",
            "measured_at: %s" % stamp,
            "compiler: %s" % compiler.relative_to(ROOT).as_posix(),
            "compiler_sha256: %s" % _sha_file(compiler),
            # (AR) 🔑 **بصمةُ الثنائيِّ وحدَها لا يقرؤها أحدٌ ولا يبلغُها أحد:**
            #      لا مستودعَ للثنائيّات، فالرقمُ يُكتَبُ ولا يُقابَل. فيُسجَّلُ
            #      معه **أصلُه**: أيُّ إيداعٍ بُنِيَ منه، وأكانت الشجرةُ نظيفةً.
            #      فيصيرُ القياسُ قابلًا لإعادةِ الإنتاجِ (يُعادُ البناءُ من ذلك
            #      الإيداع) و**قابلًا للتكذيب** — وهما شرطا الرقمِ المُعلَن.
            "source_head: %s" % _git("rev-parse", "HEAD"),
            # (AR) والنطاقُ يشملُ **البذورَ** أيضًا: هي مادّةُ القياسِ لا خلفيّتُه،
            #      وكان الحقلُ يقولُ `no` وبذرةٌ مُعدَّلةٌ غيرُ مُودَعة.
            "source_dirty: %s" % (
                "yes" if _git("status", "--porcelain", "--", "compiler", "shared",
                              "tests") else "no"),
            "pool: %d" % len(pool),
            # (AR) 🔑 **الحجمُ المطلوبُ لا المقيسُ وحدَه.** كان يُسجَّلُ العددُ
            #      **بعدَ** تصفيةِ الحمراءِ قبلَ الطفرة، فلا يُعادُ به إنتاجُ
            #      القياس: `--sample=N` يختارُ بخطوةٍ ثابتة، وتغييرُ N بواحدٍ
            #      يُبدِّلُ **١٤٢ من ١٤٩** بذرةً (قِيسَ). فتشغيلتانِ بحجمَينِ
            #      متجاورَينِ تقيسانِ مجموعتَينِ متباينتَين، وفرقُ نتيجتِهما
            #      **تباينُ عيّنةٍ لا انحدار**. والحجمُ المطلوبُ يُسجَّلُ ههنا،
            #      والبصمةُ أدناه تُثبِّتُ **أيَّ نصٍّ** قِيسَ بعينِه.
            "sample_requested: %d" % size,
            "sample_measured: %d" % len(chosen),
            # (AR) 🔑 **وبصمةُ العيّنةِ المقيسةِ نفسِها.** `source_dirty` وحدَه
            #      يقولُ «الشجرةُ فيها تحريرٌ ما» ولا يقولُ **أيَّ نصٍّ قِيس** —
            #      فسجلٌّ سليمُ الشكلِ يبقى غيرَ قابلٍ للتكذيبِ إن حُرِّرت بذرة
            #      بعدَه. وهذه البصمةُ تُقابَلُ بإعادةِ حسابِها على العيّنةِ
            #      نفسِها: تطابقٌ ⇒ الرقمُ عن هذا النصِّ بعينِه.
            "sample_sha256: %s" % _sha_sample(chosen),
            # (AR) 🔑 **والمستبعَداتُ تُسمّى.** البصمةُ وحدَها تقولُ «تغيَّرَ شيء»
            #      ولا تقولُ ماذا، والعددُ يقولُ «١٩٧» ولا يقولُ **أيّ** ١٩٧.
            #      وبتسميةِ المستبعَداتِ تصيرُ المجموعةُ مشتقّةً كاملةً:
            #      `_sample(_candidates(), sample_requested)` ناقصَ هؤلاء.
            # (AR) مفتاحانِ لا مفتاحٌ واحد: عددٌ قياسيٌّ تلاه تسلسلٌ أعمقَ
            #      إزاحةً يلتحمُ نصًّا واحدًا فلا يُقرأُ عددًا ولا يُجرَدُ قائمة.
            "excluded_not_green: %d" % len(unhealthy),
            "excluded:" if unhealthy else "excluded: []",
            *["  - %s" % p.relative_to(ROOT).as_posix() for p in unhealthy],
            # (AR) وأمرُ إعادةِ الإنتاجِ حرفيًّا — لا يُعادُ بناؤه من الحقول.
            "reproduce: python scripts/codegen/calibrate_seed_proofs.py"
            " --record --date=%s --sample=%d" % (stamp, size),
            "mutators:",
        ]
        for name, _m, kind in MUTATORS:
            lines.append('  - name: "%s"' % name)
            lines.append("    kind: %s" % {"برهان": "proof",
                                            "شبهُ برهان": "near_proof"}.get(
                kind, "upper_bound"))
            lines.append("    calibrated: %d" % (total - skipped[name]))
            lines.append("    stayed_green: %d" % len(results[name]))
        RECORD.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")
        print("\n✓ أُودِعَ القياس: %s" % RECORD.relative_to(ROOT).as_posix())

    # (AR) رمزُ الخروجِ يحكمُ على **البرهانِ** وحدَه؛ الحدُّ الأعلى يُبلَّغُ ولا
    #      يُحاكَمُ عليه — فرفضٌ على طفرةٍ قد تكونُ مكافئةً **رفضٌ كاذب**.
    return 1 if results["عقدٌ مُفسَد"] else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
