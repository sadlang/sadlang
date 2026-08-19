#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسُ سجلِّ الحمرةِ المقيسة — `tests/behavior/DECLARED_REDS.tsv`.

     🔑 **الدَّينُ يُقاس ولا ينمو.** المجلّدان `grammar_gaps` و`_regression`
     خارجَ كلِّ مستوًى يُشغّلُه CI، ففيهما اليومَ ٦١ ملفًّا أحمرَ لا يراها أحد.
     ولو صارت ٦٢ غدًا لَما لاحظَ أحد، ولو اخضرَّ أحدُها لَبقيَ موصوفًا عيبًا
     وهو مُصلَح. فالحارسُ يُخفِقُ في **الاتّجاهَين**:

       ① ملفٌّ في السجلِّ صار أخضرَ  ⇒ إخفاق: احذف صفَّه (العيبُ أُصلِح).
       ② ملفٌّ أحمرُ ليس في السجلِّ  ⇒ إخفاق: الدَّينُ نما صامتًا.
       ③ ملفٌّ تغيّرَ صنفُ إخفاقِه   ⇒ إخفاق: «فرقُ مخرَجٍ» صار انهيارًا
                                        عطبٌ آخرُ لم يُشخَّص، لا القدرُ المعلوم.
       ④ صفٌّ يشيرُ إلى ملفٍّ محذوف  ⇒ إخفاق: السجلُّ بَلِيَ.

     ⚠️ ولا يُقبَلُ حكمٌ إلّا من شوطٍ فعليّ: بلا `--measure` يفحصُ الحارسُ
     **بِنيةَ السجلِّ ووجودَ ملفّاتِه** فقط، ويُعلِنُ صراحةً أنّه لم يَقِسْ.
     فحصٌ يُعلِنُ أخضرَ عمّا لم يُشغِّلْه هو عينُ العطبِ الذي يحرسُه هذا الملفّ.

(EN) Guard for the measured-reds registry. grammar_gaps and _regression sit
     outside every CI level, so their 61 red files are invisible: a 62nd would
     go unnoticed and a fixed one would stay described as broken. The guard
     fails in BOTH directions — a row that turns green, a red that is absent, a
     changed failure kind, or a row pointing at a deleted file.

     ⚠️ Without --measure it validates only the registry's shape and that every
     listed file exists, and says so explicitly: a check that reports green on
     what it never ran is the very defect this file guards against.

الاستعمال / usage:
    python tests/system/harness/test_declared_reds_registry.py
    python tests/system/harness/test_declared_reds_registry.py --measure \
        --interp build/bin/sad-run --compiler build/bin/sad-build
"""

import argparse
import io
import json
import os
import subprocess
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(
    os.path.abspath(__file__)))))
BEHAVIOR_DIR = os.path.join(REPO_ROOT, "tests", "behavior")
REGISTRY_PATH = os.path.join(BEHAVIOR_DIR, "DECLARED_REDS.tsv")
RUNNER_PATH = os.path.join(REPO_ROOT, "tests", "runner.py")
REPORT_PATH = os.path.join(REPO_ROOT, "build", "_dual_report.json")

# (AR) المجلّداتُ التي يغطّيها السجلُّ — وهي بعينِها ما لا يُشغّلُه أيُّ مستوًى.
# (EN) The directories the registry covers — exactly those no level runs.
COVERED_DIRS = ("grammar_gaps", "_regression")

SAD_SUFFIX = "ص"  # (AR) حرفُ الصاد — لاحقةُ ملفّاتِ ص

# ════════════════════════════════════════════════════════════════════════════
# (AR) 🔑 **طبقةُ الإخفاقِ تُحاكَم، وحرفُه يُدوَّنُ ولا يُحاكَم.**
#
#      السجلُّ يُولَّدُ على منصّةٍ وبناءٍ، ويُحاكَمُ في CI على منصّةٍ وبناءٍ آخرَين.
#      وقِيس ٢٠٢٦-٠٨-١٨ أنّ ثلاثةَ صفوفٍ مسجَّلةٍ `FAIL_TIMEOUT` **لا تتباطأ
#      أصلًا**: تشغيلُها مباشرةً يخرج بـ`buildrc=3` (‏`abort`) في أقلَّ من ثانية.
#      وإنّما حجبَها حوارُ تأكيدِ Debug على ويندوز حتّى انقضتِ المهلة. وعلى
#      لينكس/Release لا تأكيدَ أصلًا (‏NDEBUG) فالصنفُ يتبدّل حتمًا.
#
#      فالحكمُ بحرفِ الصنفِ يجعل الحارسَ يُخفِقُ على **فرقِ منصّةٍ** ويسمّيه
#      انحدارَ كود — وهو إنذارٌ كاذبٌ يُدرَّبُ الناسُ على تجاهُلِه، فيموتُ
#      الحارسُ حيًّا. وإلغاءُ فحصِ الصنفِ بالكلّيّةِ يُفرِّط في قيمةٍ حقيقيّة.
#
#      فالوسط: تُجمَعُ الأصنافُ في **طبقتَين مستقرّتَين عبرَ المنصّات**:
#        • `OUTPUT`  — المحرّكان أنتجا قيمتَين واختلفتا. خاصّيّةُ كودٍ لا بيئة.
#        • `ABORTED` — لم تُنتَجْ قيمةٌ أصلًا (ترجمةٌ فشلت · انهيار · مهلة ·
#                      خطأُ تشغيل). أيُّها بالضبط يتبعُ البناءَ والمنصّة.
#
#      فانتقالُ صفٍّ بين الطبقتَين حدثٌ يستحقُّ الإخفاق: «فرقُ مخرَجٍ» صار
#      انهيارًا **عطبٌ آخرُ لم يُشخَّص**. وتنقّلُه داخلَ `ABORTED` يُطبَع
#      ملاحظةً ولا يُخفِق.
# (EN) The failure LAYER is judged; its exact letter is recorded, not judged. The
#      registry is generated on one platform/build and judged on another. Measured:
#      three rows recorded FAIL_TIMEOUT do not hang at all — run directly they abort
#      (rc=3) in under a second; a Windows Debug assertion dialog held them to the
#      timeout, and under Linux/Release there is no assertion at all. Judging the
#      letter makes the guard fail on a PLATFORM difference and call it a code
#      regression — a false alarm people learn to ignore, which kills the guard while
#      it still looks alive. Dropping the check entirely forfeits real value. So kinds
#      collapse into two platform-stable layers: OUTPUT (both engines produced values
#      and they differed — a property of the code) and ABORTED (no value was produced:
#      compile failure, crash, timeout, runtime error — which one depends on the
#      build). Moving BETWEEN layers fails; moving within ABORTED prints a note.
# ════════════════════════════════════════════════════════════════════════════
FAIL_LAYER_OUTPUT = "OUTPUT"
FAIL_LAYER_ABORTED = "ABORTED"


def fail_layer(kind):
    """(AR) طبقةُ صنفِ الإخفاقِ — مستقرّةٌ عبرَ المنصّات."""
    return FAIL_LAYER_OUTPUT if kind == "FAIL_OUTPUT" else FAIL_LAYER_ABORTED


def read_registry():
    """(AR) يقرأ السجلَّ ويردُّ (خريطةَ مسار←صنف، قائمةَ أخطاءِ بِنية)."""
    if not os.path.exists(REGISTRY_PATH):
        return {}, ["السجلُّ غيرُ موجود: " + REGISTRY_PATH]

    entries, problems, seen_lines = {}, [], {}
    with io.open(REGISTRY_PATH, encoding="utf-8") as handle:
        for number, raw in enumerate(handle, start=1):
            line = raw.rstrip("\n").rstrip("\r")
            if not line.strip() or line.lstrip().startswith("#"):
                continue
            parts = line.split("\t")
            if len(parts) != 2:
                problems.append(
                    "السطر %d: عمودان مطلوبان بفاصلِ جدولةٍ لا %d" % (number, len(parts)))
                continue
            path, kind = parts[0].strip(), parts[1].strip()
            if not path.endswith("." + SAD_SUFFIX):
                problems.append("السطر %d: المسارُ لا ينتهي بلاحقةِ ص — %s" % (number, path))
                continue
            if not kind.startswith("FAIL_"):
                problems.append("السطر %d: صنفُ الإخفاقِ يجب أن يبدأ بـFAIL_ — %s" % (number, kind))
                continue
            if not path.startswith(COVERED_DIRS):
                problems.append(
                    "السطر %d: المسارُ خارجَ نطاقِ السجلّ (%s) — %s"
                    % (number, "، ".join(COVERED_DIRS), path))
                continue
            if path in entries:
                problems.append(
                    "السطر %d: مكرَّرٌ مع السطر %d — %s" % (number, seen_lines[path], path))
                continue
            entries[path] = kind
            seen_lines[path] = number
    return entries, problems


def check_files_exist(entries):
    """(AR) ④ صفٌّ يشيرُ إلى ملفٍّ محذوف ⇒ السجلُّ بَلِيَ."""
    missing = []
    for path in sorted(entries):
        if not os.path.exists(os.path.join(BEHAVIOR_DIR, path.replace("/", os.sep))):
            missing.append(path)
    return missing


def measure(interp, compiler, timeout):
    """(AR) يُشغّلُ المجلّدَين فعلًا ويردُّ خريطةَ مسار←صنفٍ للأحمرِ المقيس.

    (AR) 🔑 ويُقرأُ التقريرُ الآليُّ لا المخرَجُ النصّيّ: طباعةُ المُشغِّلِ تحمل
         الاسمَ المجرَّدَ وحدَه، والاسمُ المجرَّدُ **ليس هُويّة** — ملفّان
         باسمٍ واحدٍ في مجلّدَين مختلفَين يندمجان فيُخفي أحدُهما الآخر.
    """
    measured = {}
    for subdir in COVERED_DIRS:
        command = [sys.executable, RUNNER_PATH, "--dir", subdir,
                   "--timeout", str(timeout), "--report", "--no-color",
                   "--interp", interp]
        if compiler:
            command += ["--compiler", compiler]
        env = dict(os.environ)
        env["PYTHONIOENCODING"] = "utf-8"
        env["PYTHONUTF8"] = "1"

        # (AR) 🔑 يُحذَفُ التقريرُ **قبلَ** الشوط. فالمسارُ نفسُه تكتبُه خطواتٌ أخرى
        #      في الورقةِ (`--report` في مصفوفةِ القواعدِ والمستوى الكامل)، فلو
        #      أخفقَ إطلاقُ المُشغِّلِ هنا لَقُرِئ تقريرُ **خطوةٍ أخرى** وأُعلِن الحكمُ
        #      عليه. وهذا بعينِه العطبُ الذي بُنيَ هذا الملفُّ ليمنعَه: حكمٌ أخضرُ
        #      على ما لم يُشغَّل.
        # (EN) Delete the report BEFORE the run: other steps in the workflow write the
        #      same path, so a failed launch here would have the verdict read ANOTHER
        #      step's report — the very defect this file exists to prevent.
        if os.path.exists(REPORT_PATH):
            os.remove(REPORT_PATH)

        subprocess.run(command, cwd=REPO_ROOT, env=env,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if not os.path.exists(REPORT_PATH):
            raise SystemExit("❌ لم يُنتَج التقريرُ الآليّ (لم يُشغَّلْ شيء): " + REPORT_PATH)
        with io.open(REPORT_PATH, encoding="utf-8") as handle:
            report = json.load(handle)

        # (AR) ⚠️ وشوطٌ بصفرِ حالاتٍ ليس «كلُّها اخضرَّت» بل «لم يُقَسْ شيء».
        # (EN) A zero-case run is not "all green" — it is "nothing was measured".
        rows = report.get("tests", [])
        if not rows:
            raise SystemExit("❌ التقريرُ بلا حالاتٍ — المجلّدُ %s لم يُشغَّل" % subdir)
        print("  · %s: %d حالةً مقيسة" % (subdir, len(rows)))

        for row in rows:
            status = row.get("status", "")
            if not status.startswith("FAIL_"):
                continue
            absolute = os.path.abspath(row.get("file", ""))
            relative = os.path.relpath(absolute, BEHAVIOR_DIR).replace(os.sep, "/")
            measured[relative] = status
    return measured


def main():
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument("--measure", action="store_true",
                        help="(AR) شغِّلِ المجلّدَين فعلًا وقارِنْ — بلا هذا لا حكمَ على الحمرة")
    parser.add_argument("--interp", default="")
    parser.add_argument("--compiler", default="")
    parser.add_argument("--timeout", type=int, default=30)
    args = parser.parse_args()

    print("═" * 62)
    print("  حارسُ سجلِّ الحمرةِ المقيسة — DECLARED_REDS.tsv")
    print("═" * 62)

    entries, problems = read_registry()
    failures = list(problems)

    missing = check_files_exist(entries)
    for path in missing:
        failures.append("صفٌّ يشيرُ إلى ملفٍّ غيرِ موجود (السجلُّ بَلِيَ): " + path)

    print("  صفوفُ السجلّ: %d · أخطاءُ بِنية: %d · ملفّاتٌ مفقودة: %d"
          % (len(entries), len(problems), len(missing)))

    if not args.measure:
        # (AR) ⚠️ يُقالُ صراحةً: لم يُقَسْ شيءٌ من الحمرةِ نفسِها.
        print("  ⚠️ وضعُ البِنيةِ فقط — لم تُشغَّلْ أيُّ حالةٍ، فلا حكمَ على الحمرة.")
        print("     للحكمِ الكامل: --measure --interp <مسار> --compiler <مسار>")
    else:
        if not args.interp:
            raise SystemExit("❌ --measure يلزمُه --interp")
        measured = measure(args.interp, args.compiler, args.timeout)
        registry_paths = set(entries)
        measured_paths = set(measured)

        # ① اخضرَّ ملفٌّ مسجَّل
        for path in sorted(registry_paths - measured_paths):
            failures.append(
                "اخضرَّ وهو مسجَّلٌ أحمرَ — احذف صفَّه من السجلّ: %s (كان %s)"
                % (path, entries[path]))
        # ② احمرَّ ملفٌّ غيرُ مسجَّل
        for path in sorted(measured_paths - registry_paths):
            failures.append(
                "أحمرُ وليس في السجلّ — الدَّينُ نما صامتًا: %s (%s)"
                % (path, measured[path]))
        # ③ تغيّرَت طبقةُ الإخفاق (لا حرفُه — انظر fail_layer)
        drifted_within_layer = []
        for path in sorted(registry_paths & measured_paths):
            if entries[path] == measured[path]:
                continue
            if fail_layer(entries[path]) != fail_layer(measured[path]):
                failures.append(
                    "تغيّرَت طبقةُ الإخفاق — عطبٌ آخرُ لم يُشخَّص: %s (%s ← %s)"
                    % (path, entries[path], measured[path]))
            else:
                drifted_within_layer.append(
                    "%s (%s ← %s)" % (path, entries[path], measured[path]))
        if drifted_within_layer:
            # (AR) ملاحظةٌ لا إخفاق — ويُقالُ عددُها كي لا يمرَّ الانجرافُ صامتًا.
            # (EN) A note, not a failure — but counted so the drift is never silent.
            print("  ℹ️ تنقّلٌ داخلَ طبقةِ ABORTED (لا يُخفِق): %d"
                  % len(drifted_within_layer))
            for item in drifted_within_layer:
                print("     · " + item)
        print("  مقيسٌ أحمرَ: %d · مسجَّل: %d" % (len(measured_paths), len(registry_paths)))

    print("─" * 62)
    if failures:
        for item in failures:
            print("  ❌ " + item)
        print("─" * 62)
        print("  ❌ إخفاق: %d مخالفة" % len(failures))
        return 1
    print("  ✅ السجلُّ مطابق")
    return 0


if __name__ == "__main__":
    sys.exit(main())
