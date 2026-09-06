#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) الحارسُ الفوقيّ: «عيارٌ قديمٌ لحارسٍ جديدٍ ليس عيارًا».

     أغلبُ الأعطابِ المُدوَّنةِ في سجلِّ دروسِ هذا المستودعِ صورةٌ واحدةٌ مكرّرة:
     **حارسٌ اخضرَّ لأنّه لا يستطيعُ أن يحمرّ**. والعلاجُ أن يُحقَنَ فيه عطبٌ
     مقصودٌ فيُثبَتَ احمرارُه — ثمّ **أن يُحرَسَ العيارُ نفسُه**، وإلّا صارَ نثرًا
     في تعليقٍ يبلى بأوّلِ تعديل.

     أربعةُ لامتغيِّراتٍ تُقاس:

       ① **انجرافُ البصمة**: سجلٌّ يحملُ `guard_sha256` أو `harness_sha256` لملفٍّ
          تغيّرَ بعدَه ⇒ أحمر. فالحارسُ الذي عُويِرَ ليس الحارسَ الذي يعملُ اليوم،
          والمِحقنةُ التي أثبتت ليست المِحقنةَ القائمة.

       ② **سجلٌّ ناقصُ النتيجة**: `probes_passed` أقلُّ من `probes_total` ⇒ أحمر،
          **أو `probes_total` دونَ الأرضيّةِ المُودَعة** ⇒ أحمر أيضًا. فالشرطُ
          الأوّلُ وحدَه يُرضى بحذفِ ما يُقاس: تُقلَّصُ المِحقنةُ إلى مجسٍّ واحدٍ
          فيصيرُ `1/1` «كاملًا». والأرضيّةُ تنزلُ ولا تُرفَع.
          🔑 وبدونِ هذا يصيرُ السجلُّ **دعوى لا قياسًا**: يكفي أن يُكتَبَ ملفٌّ
          بيدٍ فيه بصمةٌ صحيحةٌ و`result: 0/0` ليُقرأَ الحارسُ «مُعايَرًا» ولم
          يُنفَّذْ مجسٌّ واحد. (بُرهنَ بالحقنِ في مراجعةٍ خصميّة.)

       ③ **سجلٌّ يتيم**: يشيرُ إلى حارسٍ أو مِحقنةٍ غيرِ موجودة ⇒ أحمر. شاهدةٌ
          لميّتٍ تُوهِمُ بتغطيةٍ لا وجودَ لها.

       ④ **الحرّاسُ بلا سجلّ**: سقفٌ **نازل**. ولم يُجعَلْ صفرًا لأنّ الصفرَ اليومَ
          كذبٌ يُسكَّنُ ولا يُصلَح: العددُ يُسمّى ثمّ ينزلُ بالعمل.

     ⚠️ **ولا يُعايِرُ هذا الحارسُ نفسَه**: يستثني ملفَّه من العدِّ، فهو الوحيدُ
        الذي لا يُثبِتُ النظامُ احمرارَه. وعيارُه من خارجِه — بحقنٍ يدويٍّ مُدوَّنٍ
        في مراجعة. والعددُ المطبوعُ «حرّاسُ الشجرة» لا يعدُّه.

(EN) Meta-guard: a stale calibration is not a calibration. Fails on fingerprint
     drift (guard or harness), on a record whose probes did not all pass (else a
     hand-written record with a valid hash and `0/0` reads as calibrated), on a
     record pointing at a missing file, and when uncalibrated guards grow past
     the committed, descending ceiling. It does not calibrate itself.
============================================================================
"""
from __future__ import annotations

import hashlib
import subprocess
import sys
from pathlib import Path

import yaml

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
def _sha_norm(path) -> str:
    """(AR) بصمةٌ على النصِّ بعدَ تطبيعِ نهاياتِ الأسطرِ إلى ما يُودِعُه git."""
    return hashlib.sha256(path.read_bytes().replace(CRLF, LF_)).hexdigest()


ROOT = Path(__file__).resolve().parents[2]
CODEGEN = ROOT / "scripts" / "codegen"
CI = ROOT / "scripts" / "ci"
RECORDS = CODEGEN / "calibration"

# (AR) السقفُ المُودَعُ للحرّاسِ بلا عيار — **ينزلُ ولا يُرفَع**.
#      🔑 ولا يُنثَرُ العددُ المُقاسُ ههنا: يُطبَعُ في كلِّ تشغيلةٍ ويتغيّرُ بوصولِ
#      حارسٍ ووصولِ سجلٍّ معًا — فنسخةٌ باليدِ تبلى بأوّلِ منهما. (وقد بلِيَت:
#      كُتِبَ «٢٧ حارسًا وواحدٌ مُعايَر» ثمّ صارا ٢٨ واثنَين.)
#      وخمسةٌ منها تحملُ عيارًا **نثرًا في تعليقٍ** لا سجلًّا — والنثرُ لا يُحتسَب:
#      لا يُعادُ تقييمُه حين يتغيّرُ الحارس، وهو عينُ ما يمنعُه هذا الملفّ.
# (AR) 🔑 **السقفُ ينزلُ بالعملِ لا بالقلم — وهذا الملفُّ شهدَ نقضَ قاعدتِه.**
#      أُنزِلَ ٢٦ ← ٢٥ في إيداعٍ لم يُودِعْ سجلَّ العيارِ الرابعَ الذي يُبرِّرُه
#      (كان تعديلَ شجرةِ عملٍ لجلسةٍ أخرى، ابتلعَه `git commit -- <مسارات>`
#      لأنّه يُودِعُ **محتوى شجرةِ العمل** لتلك المسارات لا ما رُحِّلَ وحدَه).
#      والمقيسُ على شجرةِ ذلك الإيداعِ نفسِها: `بلا سجلِّ عيار: 26 (السقف 25)`
#      ⇒ رمز ١ بتشخيصٍ **كاذب** («حارسٌ جديدٌ يصلُ ومعه سجلُّ عيارِه») — رفضٌ
#      كاذبٌ يوقفُ البوّابةَ على كلِّ استنساخٍ نظيف. يُنزَّلُ حينَ يُودَعُ سجلُّه.
CEILING_UNCALIBRATED = 26

# (AR) 🔑 **أرضيّةُ عمقِ العيار — تنزلُ المِحقنةُ إليها ولا تحتها.**
#      لولاها لكانَ تقليصُ المِحقنةِ من ثمانيةِ مجسّاتٍ إلى واحدٍ ثمّ `--record`
#      طريقًا مشروعًا إلى سجلٍّ يقولُ `1/1` **أخضر** — فشرطُ «النتيجةُ كاملة»
#      يُرضى بحذفِ ما يُقاس. كاملٌ من واحدٍ ليس كاملًا.
#      وتنزلُ إلى عددِ مجسّاتِ المِحقنةِ القائمةِ ولا تحتَه — ولا يُنثَرُ العددُ
#      ههنا نسخةً ثانيةً: قيمتُه في السطرِ التالي وحدَه.
CEILING_MIN_PROBES = 10


def _key(path: Path) -> str:
    """(AR) هويّةُ الحارسِ **مسارُه** لا اسمُه القاعديّ.

    🔑 وبالاسمِ القاعديِّ كان تصادمُ الأسماءِ يُزوِّرُ عيارًا: حارسٌ في
       `scripts/ci/` يحملُ اسمَ حارسٍ مُعايَرٍ في `scripts/codegen/` يُقرأُ
       «مُعايَرًا» وهو لم يُعايَرْ قطّ. (بُرهنَ بالحقنِ في مراجعةٍ خصميّة.)
    """
    return path.resolve().relative_to(ROOT).as_posix()


# (AR) 🔑 **والسجلُّ غيرُ المُودَعِ ليس من الشجرة.** كان المسحُ `glob` على
#      القرصِ وحدَه، فسجلُّ عيارٍ قيدَ الإنشاءِ عندَ أحدِهم يُحمِّرُ البوّابةَ
#      **محلّيًّا ولا يراه CI** — حمرةٌ لا يُسكِتُها صاحبُها إلّا بإيداعِ عملٍ
#      ناقص، وهي صورةُ الرفضِ الكاذبِ عينُها. وقِيسَت حيّةً: سجلُّ حارسٍ آخرَ
#      قيدَ الإنشاءِ (٦ مجسّاتٍ) أوقفَ بوّابةَ عملٍ لا صلةَ له به.
#      وفي CI كلُّ ما في الشجرةِ مُودَعٌ، فالمقيسُ هناك لا يتبدَّل.
#      ⚠️ ولا يُسكَتُ عن تعذُّرِ التصفية: إن لم يُجَبْ `git` **يُقالُ ذلك سطرًا
#         مسمًّى** ويُمسَحُ القرصُ كما كان — سقوطٌ صامتٌ إلى السلوكِ القديمِ هو
#         العطبُ الذي يُدوِّنُه سجلُّ دروسِ هذا المستودعِ مرارًا.
#      🔑 **والمسارُ يُشتقُّ من `RECORDS` لا يُهجّى.** كان مكتوبًا باليدِ
#         نسخةً ثانية، و`git ls-files -- <مسارٌ غيرُ موجود>` يردُّ **رمزَ ٠
#         ومخرَجًا فارغًا** — فلا يلتقطُه `check=True`، وتسقطُ السجلّاتُ كلُّها
#         صامتةً. وقِيسَ الأثر: `سجلّاتُ عيار: 0` ثمّ `بلا سجلِّ عيار: 29 > 26`
#         بتشخيصٍ **كاذب** («حارسٌ جديدٌ يصلُ») ورمزِ ١ — رفضٌ كاذبٌ شاملٌ في
#         CI. والهامشُ صفرٌ اليوم، فأيُّ إخفاقٍ في التصفيةِ يُحمِّرُ فورًا.
#      🔑 **وطرفا الكسرِ من أصلٍ واحد.** كانت السجلّاتُ تُصفّى والحرّاسُ لا
#         يُصفَّون، فحارسٌ جديدٌ غيرُ مُودَعٍ يُحسَبُ ويُغفَلُ سجلُّه — وهي عينُ
#         الحمرةِ المحلّيّةِ التي لا يراها CI، مَنقولةً من المقامِ إلى البسط.
def _tracked(paths: list[Path], scope: list[Path]) -> tuple[list[Path], str]:
    args = [p.relative_to(ROOT).as_posix() for p in scope]
    try:
        out = subprocess.run(["git", "-C", str(ROOT), "ls-files", "-z", "--"] + args,
                             capture_output=True, check=True).stdout.decode("utf-8")
    except (OSError, subprocess.CalledProcessError) as exc:
        return paths, "  ⚠️ تعذَّرَ سؤالُ git (%s) — مُسِحَ القرصُ بلا تصفية" % exc
    known = {(ROOT / rel).resolve() for rel in out.split("\0") if rel}
    kept = [p for p in paths if p.resolve() in known]
    return kept, ""


def _guard_scope() -> list[Path]:
    return [CODEGEN] + ([CI] if CI.is_dir() else [])


def _guards() -> list[Path]:
    found = sorted(CODEGEN.glob("check_*.py"))
    if CI.is_dir():
        found += sorted(CI.glob("check_*.py"))
    return [p for p in found if p.resolve() != Path(__file__).resolve()]


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    all_guards = _guards()
    on_disk = sorted(RECORDS.glob("*.yaml")) if RECORDS.is_dir() else []
    guards, note = _tracked(all_guards, _guard_scope())
    records, rec_note = _tracked(on_disk, [RECORDS])
    note = note or rec_note

    print("حارس «عيارٌ قديمٌ لحارسٍ جديدٍ ليس عيارًا»:")
    print(f"  حرّاسُ الشجرة: {len(guards)} · سجلّاتُ عيار: {len(records)}")
    if note:
        print(note)
    else:
        # (AR) 🔑 **والطرفانِ يُسمَّيانِ منفصلَين.** عددٌ واحدٌ يجمعُهما لا يقولُ
        #      للقارئِ أيُّهما سقط، والحكمُ يختلفُ اختلافًا تامًّا بينهما.
        dropped = (len(all_guards) - len(guards), len(on_disk) - len(records))
        if any(dropped):
            print("  · غيرُ مُودَعٍ أُغفِلَ — حرّاسٌ %d · سجلّاتٌ %d (لا يراه CI)"
                  % dropped)

    # (AR) 🔑 **وصفرُ حارسٍ ليس نجاحًا.** التصفيةُ بـgit تسقطُ إلى «لا شيء» حينَ
    #      يردُّ `ls-files` رمزَ ٠ ومخرَجًا فارغًا — في نسخةٍ مُصدَّرةٍ بلا
    #      `.git`، أو فهرسٍ فارغ. وقِيسَ: `حرّاسُ الشجرة: 0 · سجلّاتُ عيار: 0`
    #      ثمّ `✓ كلُّ سجلٍّ يطابقُ …` **ورمزُ صفر** — أخضرُ لأنّ الشرطَ لا
    #      يمكنُ أن يكونَ كاذبًا. والدرسُ مكتوبٌ في `scripts/run_tests.ps1`.
    if all_guards and not guards:
        print("  ✗ عطبُ آلة: التصفيةُ أسقطت الحرّاسَ كلَّهم (%d) — لم يُقَسْ شيء"
              % len(all_guards))
        return 2
    if not all_guards:
        print("  ✗ عطبُ آلة: لا حارسَ في الشجرة — لم يُقَسْ شيء")
        return 2

    drifted: list[str] = []
    orphaned: list[str] = []
    incomplete: list[str] = []
    calibrated: set[str] = set()

    for rec in records:
        rel = rec.relative_to(ROOT).as_posix()
        try:
            doc = yaml.safe_load(rec.read_text(encoding="utf-8")) or {}
        except yaml.YAMLError as exc:
            orphaned.append(f"{rel} — لا يُحلَّلُ YAML: {exc.__class__.__name__}")
            continue
        if not isinstance(doc, dict):
            orphaned.append(f"{rel} — بنيةٌ غيرُ متوقَّعة")
            continue

        guard_rel = doc.get("guard")
        guard_path = (ROOT / guard_rel) if guard_rel else None
        if not guard_rel or not guard_path.is_file():
            orphaned.append(f"{rel} ← حارسٌ غيرُ موجود: {guard_rel or '(بلا حقل guard)'}")
            continue

        # ② النتيجةُ كاملةٌ أوّلًا — سجلٌّ لم تُنفَّذْ مجسّاتُه دعوى لا قياس.
        total = doc.get("probes_total")
        passed = doc.get("probes_passed")
        if not isinstance(total, int) or not isinstance(passed, int) or total < 1:
            incomplete.append(f"{rel} — بلا probes_total/probes_passed صحيحَين")
            continue
        if passed != total:
            incomplete.append(f"{rel} — {passed}/{total}: عيارٌ ناقصٌ لا يُحتسَب")
            continue

        # ① البصماتُ: الحارسُ والمِحقنةُ معًا.
        drift = False
        for key, path_key in (("guard_sha256", "guard"), ("harness_sha256", "harness")):
            target_rel = doc.get(path_key)
            recorded = doc.get(key)
            # (AR) الغيابُ وحدَه «بلا حقل». وقيمةٌ موجودةٌ خاطئةٌ **انجرافٌ** يُسمّى
            #      باسمِه. و`not recorded` كان يخلطُ بينهما: بصمةٌ كلُّها أرقامٌ
            #      (`0000`) يحلُّها YAML عددًا صفرًا فتُقرأُ «بلا حقل» — تشخيصٌ
            #      يُرسِلُ القارئَ إلى العلاجِ الخطأ. (قِيسَ في مراجعةٍ خصميّة.)
            if recorded is None or not str(recorded).strip():
                incomplete.append(f"{rel} — بلا {key}")
                drift = True
                break
            target = ROOT / target_rel if target_rel else None
            if target is None or not target.is_file():
                orphaned.append(f"{rel} ← {path_key} غيرُ موجود: {target_rel}")
                drift = True
                break
            actual = _sha_norm(target)
            if actual != str(recorded):
                drifted.append(f"{target_rel}\n        مُسجَّل: {recorded}"
                               f"\n        فعليّ:  {actual}"
                               f"  (عُويِرَ في {doc.get('calibrated_at', '؟')})")
                drift = True
                break
        if drift:
            continue

        if total < CEILING_MIN_PROBES:
            incomplete.append(f"{rel} — {total} مجسًّا < {CEILING_MIN_PROBES}:"
                              " مِحقنةٌ قُلِّصت، وكاملٌ من واحدٍ ليس كاملًا")
            continue
        # (AR) 🔑 السجلُّ يُثبِتُ **هويّةَ ملفَّين**، ولا يُثبِتُ وحدَه أنّ الثاني
        #      يمسُّ الأوّل. فسجلٌّ يُنسَخُ لحارسٍ جديدٍ ويُحدَّثُ مسارُه وبصمتُه
        #      وتُنسى مِحقنتُه كان يجعلُه «مُعايَرًا» بمِحقنةِ غيرِه — بابُ `0/0`
        #      نفسُه من جهةٍ أخرى. (بُرهنَ بالحقنِ في مراجعةٍ خصميّة.)
        #      وأرخصُ رباطٍ يُقاس: أن تُسمّي المِحقنةُ حارسَها في بايتاتِها.
        # (AR) 🔑 ولا يقبلُ سجلًّا لنفسِه: هو مستثنًى من `_guards()` عمدًا (لا
        #      يُعايِرُ نفسَه)، فسجلٌّ باسمِه كان يُطبَعُ «✓ مُعايَر» ولا ينزلُ به
        #      عددٌ ولا يُقاسُ به شيء — **دعوى مطبوعةٌ بلا مقابل**.
        if guard_path.resolve() == Path(__file__).resolve():
            orphaned.append(f"{rel} — سجلٌّ لهذا الحارسِ نفسِه: وهو لا يُعايِرُ"
                            " نفسَه، فسجلُّه دعوى لا قياس")
            continue
        harness_path = ROOT / doc["harness"]
        if guard_path.name.encode("utf-8") not in harness_path.read_bytes():
            orphaned.append(f"{rel} — المِحقنةُ لا تذكرُ {guard_path.name}:"
                            " سجلٌّ يربطُ ملفَّين لا يمسُّ أحدُهما الآخر")
            continue
        calibrated.add(_key(guard_path))
        print(f"  ✓ {guard_rel} — {passed}/{total}"
              f" · عُويِرَ في {doc.get('calibrated_at', '؟')}")

    uncalibrated = [_key(g) for g in guards if _key(g) not in calibrated]
    print(f"  بلا سجلِّ عيار: {len(uncalibrated)}"
          f" (السقف {CEILING_UNCALIBRATED} — نازلٌ لا يُرفَع)")

    failed = False
    if drifted:
        print("  ✗ عيارٌ بائتٌ — الملفُّ تغيّرَ ولم يُعَدْ عيارُه:")
        for item in drifted:
            print(f"      · {item}")
        print("      أعِدْه: python <المِحقنةُ المُسمّاةُ في السجلّ>"
              " --record --date=YYYY-MM-DD")
        failed = True
    if incomplete:
        print("  ✗ سجلٌّ لا يُثبِتُ عيارًا:")
        for item in incomplete:
            print(f"      · {item}")
        failed = True
    if orphaned:
        print("  ✗ سجلٌّ يتيم:")
        for item in orphaned:
            print(f"      · {item}")
        failed = True
    if len(uncalibrated) > CEILING_UNCALIBRATED:
        print(f"  ✗ نما عددُ الحرّاسِ بلا عيار: {len(uncalibrated)} >"
              f" {CEILING_UNCALIBRATED} — حارسٌ جديدٌ يصلُ ومعه سجلُّ عيارِه.")
        failed = True

    if failed:
        return 1
    print("  ✓ كلُّ سجلٍّ يطابقُ بصمتَي حارسِه ومِحقنتِه ونتيجتُه كاملة.")
    return 0


if __name__ == "__main__":
    # (AR) 🔑 انهيارٌ ليس حكمًا — كأختِه في حارسِ التغطية. سجلٌّ يُشيرُ إلى مسارٍ
    #      خارجَ جذرِ المستودعِ كان يُلقي `ValueError` خامًّا ويخرجُ بـ١ فيُقرأَ
    #      «فشلُ حارس» (عطبُ محتوًى). والرمزُ ١ محجوزٌ لحكمٍ صدرَ عن قياسٍ تمّ.
    #      (والدرسُ مُدوَّن: «الرقعةُ تسدُّ في ملفٍّ وتتركُ الأخوات».)
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"  ✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
