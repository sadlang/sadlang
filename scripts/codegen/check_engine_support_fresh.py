#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «سِجِلُّ قياسِ المحرّكَين يُغطّي ما هو معلَنٌ اليوم، ولم تتحرّك تحته
     طبقةُ الإرسال».

     المُخرَجُ `language-truth/_meta/builtin_engine_support.yaml` **لقطةُ قياسٍ**
     على ثنائيَّين مبنيَّين في لحظةٍ بعينها، ويُنشَرُ في التوثيقِ عمودَي «مترجّم»
     و«مفسّر». وأخطرُ ما يصيبُه ليس الخطأَ بل **التعفّنَ الصامت** بوجهَيه:

       ① تعفّنُ **الأسماء**: يُضافُ مدمَجٌ إلى مصدرِ الحقيقةِ ولا يُعادُ القياس،
          فتظهرُ صفحتُه في التوثيقِ بلا عمودٍ — أو أسوأ: يُحذَفُ مدمَجٌ ويبقى
          سطرُه في السِّجِلّ فيوثَّقُ ما لا وجودَ له.

       ② 🔑 تعفّنُ **الأحكام**: يُودَعُ إيداعٌ يصلُ عشراتِ المدمَجاتِ الموجودةِ
          أصلًا بذراعِ إرسالٍ في المترجّم، بلا إضافةِ اسمٍ واحدٍ جديد. عندئذٍ
          الأسماءُ كلُّها مغطّاة، والحارسُ الأوّلُ أخضرُ، والتوثيقُ يظلُّ يقول
          «لا يحلُّها المترجّم» شهورًا. فيُقاسُ الزمنُ أيضًا: أيُّ إيداعٍ مسَّ
          طبقةَ الإرسالِ **بعدَ** الإيداعِ المقيسِ يُحمِرُّ هذا الحارس.

     ⚠️ وحدُّ هذا الحارسِ مقصودٌ ومُعلَن: يقيسُ **التغطيةَ والطزاجة** لا
        **الصحّة** (أصادقٌ حكمُ كلِّ سطر). صدقُ الحكمِ لا يُعرَفُ إلّا بإعادةِ
        تشغيلِ المِجَسّ على ثنائيَّين مبنيَّين، وهو أثقلُ من بوّابةِ PR.

     العلاج عند الاحمرار:
         python scripts/codegen/probe_builtin_engines.py --build-dir build/bin/Release

(EN) Guards that the measured engine-support record covers exactly the set of
     builtins declared today, and that no dispatch-layer commit postdates the
     measured commit. Coverage and freshness — not per-row correctness.
============================================================================
"""
from __future__ import annotations

import subprocess
import sys
from pathlib import Path

import yaml

# (AR) قائمةُ مسارات الإرسالِ **تُستعارُ** من المِجَسِّ ولا تُنسَخ: نسختانِ
#      تنجرفان، فيحرسُ الحارسُ ملفّاتٍ غيرَ التي يقيسُها المِجَسّ.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from probe_builtin_engines import DISPATCH_PATHS  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"
RECORD = ROOT / "language-truth" / "_meta" / "builtin_engine_support.yaml"

# (AR) سقفانِ مُعلَنانِ لا صامتان: أسماءٌ تعذّرَ قياسُها، وأسماءٌ لم يُجِب عنها
#      الشكلُ الثاني في المعايرة. كلاهما مسمًّى في السِّجِلِّ اسمًا اسمًا، والسقفُ
#      يمنعُ أن تصيرَ السلّةُ مقبرةً يُلقى فيها ما يصعبُ قياسُه فيبقى الأخضرُ.
UNMEASURED_CEILING = 5
UNANSWERED_CEILING = 12


def declared() -> set:
    names = set()
    for path in sorted(SOT_DIR.glob("*.yaml")):
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            names.add(fn["canonical"])
    return names


def dispatch_commits_since(commit: str):
    """إيداعاتُ طبقةِ الإرسالِ بعدَ الإيداعِ المقيس، أو `None` إن تعذّر الحكم.

    `None` ليست براءةً: تُطبَعُ علّتُها ويُترَكُ الحكمُ معلَّقًا بدل ادّعاءِ
    نظافةٍ لم تُقَس.
    """
    # (AR) 🔑 الاستنساخُ الضحلُّ يُسمّى بذاتِه لا برمزِ git الخام. كان
    #      يُخفِقُ بـ«fatal: Invalid revision range» فيقرؤه الناظرُ عطبًا في
    #      الحارسِ لا نقصًا في مُدخَلِه. والعلاجُ يُكتَبُ مع العلّة:
    #      عمقُ 1 لا يحملُ الإيداعَ المقيسَ فلا يُسأَلُ عمّا بعده.
    try:
        shallow = subprocess.run(
            ["git", "-C", str(ROOT), "rev-parse", "--is-shallow-repository"],
            capture_output=True, timeout=30)
        if shallow.stdout.decode("utf-8", "replace").strip() == "true":
            return None, ("الاستنساخُ ضحلٌّ فلا يحملُ الإيداعَ المقيس. "
                          "العلاج: fetch-depth: 0 في خطوةِ checkout.")
    except Exception:                              # noqa: BLE001 — يُكمَلُ ويُحكَمُ أدناه
        pass
    try:
        proc = subprocess.run(
            ["git", "-C", str(ROOT), "log", "--oneline", f"{commit}..HEAD",
             "--", *DISPATCH_PATHS],
            capture_output=True, timeout=60)
    except Exception as exc:                       # noqa: BLE001 — يُسمّى ويُبلَّغ
        return None, f"تعذّر تشغيلُ git: {exc}"
    if proc.returncode != 0:
        err = proc.stderr.decode("utf-8", "replace").strip()[:160]
        return None, f"فشل git برمز {proc.returncode}: {err}"
    return [l for l in proc.stdout.decode("utf-8", "replace").splitlines() if l], ""


def check_side(calib: dict, key: str, label: str, problems: list) -> None:
    """معايرةُ محرّكٍ واحد: العدّادانِ متساويانِ **وكلاهما غيرُ صفر**.

    🔑 كان `present_sample` غيرَ مشروطٍ بأن يكونَ غيرَ صفر بينما `absent_sample`
    مشروط: سجلٌّ بعيّنةٍ فارغةٍ في جانبٍ يمرُّ أخضرَ وهو غيرُ مُعايَرٍ في ذلك
    الجانبِ إطلاقًا. أرضيّةُ الحكمِ لا تصحُّ بطرفٍ واحد.
    """
    side = calib.get(key) or {}
    if not side:
        problems.append(f"معايرةُ {label} غائبةٌ من السِّجِلّ")
        return
    for field in ("absent_sample", "present_sample"):
        if not side.get(field):
            problems.append(f"معايرةُ {label}: `{field}` صفرٌ أو غائب — "
                            "جانبٌ لم يُسأل عنه سؤالٌ ثانٍ")
    # (AR) «لم يُجِب» ليس «اتّفق»: عيّنةٌ تُفرَّغُ في سلّةِ الصمتِ تُخضِرُّ نسبةً
    #      على لا شيء. فيُحرَسُ الفرقُ بين المسحوبِ والمسؤولِ عنه فعلًا.
    silent = len(side.get("unanswered") or [])
    if silent > UNANSWERED_CEILING:
        problems.append(f"معايرةُ {label}: لم يُجِب الشكلُ الثاني عن {silent} "
                        f"اسمًا والسقفُ {UNANSWERED_CEILING}")
    for u in side.get("unanswered") or []:
        print(f"    ⚠ لم يُجِب: {u.get('canonical')} ({u.get('namespace')})")
    if side.get("absent_agreed") != side.get("absent_sample"):
        problems.append(f"معايرةُ {label}: جانبُ «غائب» "
                        f"{side.get('absent_agreed')}/{side.get('absent_sample')}")
    if side.get("present_agreed") != side.get("present_sample"):
        problems.append(f"معايرةُ {label}: جانبُ «موجود» "
                        f"{side.get('present_agreed')}/{side.get('present_sample')}")
    print(f"  معايرةُ {label}: «غائب» "
          f"{side.get('absent_agreed')}/{side.get('absent_sample')} "
          f"(سُحب {side.get('absent_drawn', '—')}) · «موجود» "
          f"{side.get('present_agreed')}/{side.get('present_sample')} "
          f"(سُحب {side.get('present_drawn', '—')}) · "
          f"الشكلُ الثاني: {side.get('second_form', '—')}")
    if side.get("strategy"):
        print(f"    استراتيجيّتها: {side['strategy']} — "
              f"{side.get('absent_namespaces', 0)} مجموعةً غائبةً و"
              f"{side.get('present_namespaces', 0)} موجودةً، "
              f"{side.get('per_namespace', 0)} من كلٍّ")
    # الخلافُ يُسمّى: عددٌ مجموعٌ يُخفي أنّ كلَّ المخالفينَ من مجموعةٍ واحدة.
    for d in side.get("disagreements") or []:
        print(f"    ⚠ خلافُ شكلَين: {d.get('canonical')} "
              f"({d.get('namespace')}) — المِجَسُّ قال «{d.get('probe')}»")


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    if not RECORD.exists():
        print(f"✗ سِجِلُّ القياس غير موجود: {RECORD}")
        print("  شغّل: python scripts/codegen/probe_builtin_engines.py")
        return 1

    doc = yaml.safe_load(RECORD.read_text(encoding="utf-8")) or {}
    rows = doc.get("functions") or []
    measured = {r["canonical"] for r in rows}
    sot = declared()
    problems: list = []

    # (AR) المُعلَنُ عجزُ قياسِه ليس «ناقصًا»: أُخرِج من `functions` عمدًا وسُمّي
    #      في `unmeasured`، فيعرضُه التوثيقُ «—». والخلطُ بينهما يجعلُ الصدقَ
    #      يبدو تعفّنًا. لكنّه محروسٌ بسقفٍ وبوجوبِ ذِكرِ سببٍ لكلِّ اسم.
    unmeasured = doc.get("unmeasured") or []
    unmeasured_names = {u.get("canonical") for u in unmeasured}
    if len(unmeasured) > UNMEASURED_CEILING:
        problems.append(f"تعذّر قياسُ {len(unmeasured)} اسمًا والسقفُ "
                        f"{UNMEASURED_CEILING}")
    for u in unmeasured:
        if not u.get("reason"):
            problems.append(f"اسمٌ في `unmeasured` بلا سبب: {u.get('canonical')}")

    missing = sorted(sot - measured - unmeasured_names)
    stale = sorted(measured - sot)     # مقيسٌ ولم يعُد معلَنًا ⇒ توثيقُ ما لا وجودَ له

    print("حارس «سِجِلُّ قياسِ المحرّكَين مُغطٍّ وطازج»:")
    print(f"  معلَنٌ في مصدر الحقيقة: {len(sot)} اسمًا قانونيًّا")
    print(f"  مقيسٌ في السِّجِلّ: {len(measured)}")
    for u in unmeasured:
        print(f"  ⚠ تعذّر قياسُه (يُعرَض «—» لا حكمًا): {u.get('canonical')} "
              f"[{u.get('engine')}] — {str(u.get('reason'))[:70]}")

    commit = doc.get("measured_commit")
    print(f"  قِيس على الإيداع: {commit or '—'}")
    if not commit:
        problems.append("الإيداعُ المقيسُ غيرُ مدوَّن — لا تُقاسُ طزاجةٌ بلا مرجع")
    else:
        late, why = dispatch_commits_since(commit)
        if late is None:
            problems.append(f"تعذّر قياسُ انجرافِ طبقةِ الإرسال — {why}")
        elif late:
            print(f"  ✗ بعدَ الإيداعِ المقيسِ {len(late)} إيداعًا مسَّ طبقةَ "
                  "الإرسال — أحكامُ السطورِ قد تعفّنت:")
            for line in late[:10]:
                print(f"      · {line}")
            problems.append("طبقةُ الإرسالِ تحرّكت بعدَ القياس")
        else:
            print("  ✓ لا إيداعَ مسَّ طبقةَ الإرسالِ بعدَ القياس")

    if doc.get("stale_binaries_allowed"):
        print("  ✗ السِّجِلُّ قِيس بعَلَمِ --allow-stale-binaries")
        problems.append("قياسٌ بثنائيٍّ أقدمَ من طبقةِ الإرسالِ لا يُنشَر")

    calib = doc.get("calibration") or {}
    check_side(calib, "interpreter", "المفسّر", problems)
    check_side(calib, "compiler", "المترجّم", problems)

    # (AR) 🔑 العدّاداتُ تُشتَقُّ من الصفوفِ وتُقارَنُ بالمكتوب: كانت تُطبَعُ من
    #      الوثيقةِ كحقيقةٍ بلا سندٍ، وترويسةُ «لا تحرّره بيد» غيرُ محروسة.
    counts = doc.get("counts") or {}
    derived = {
        "unmeasured": len(unmeasured),
        "declared": len(rows),
        "compiler": sum(1 for r in rows if r.get("compiler")),
        "interpreter": sum(1 for r in rows if r.get("interpreter")),
        "both": sum(1 for r in rows if r.get("compiler") and r.get("interpreter")),
        "interpreter_only": sum(1 for r in rows
                                if r.get("interpreter") and not r.get("compiler")),
        "compiler_only": sum(1 for r in rows
                             if r.get("compiler") and not r.get("interpreter")),
        "neither": sum(1 for r in rows
                       if not r.get("compiler") and not r.get("interpreter")),
    }
    for key, value in derived.items():
        if counts.get(key) != value:
            problems.append(f"عدّادٌ لا يطابقُ الصفوف: {key} مكتوبٌ "
                            f"{counts.get(key)} والمشتقُّ {value}")

    if len(measured) != len(rows):
        problems.append(f"صفوفٌ مكرّرةٌ لاسمٍ واحد: {len(rows)} صفًّا "
                        f"لـ{len(measured)} اسمًا")

    if missing:
        print(f"  ✗ معلَنٌ ولم يُقَس: {len(missing)} — "
              "سيظهر في التوثيق بلا عمودِ محرّك:")
        for name in missing[:15]:
            print(f"      · {name}")
        problems.append(f"معلَنٌ ولم يُقَس: {len(missing)}")
    if stale:
        print(f"  ✗ مقيسٌ ولم يعُد معلَنًا: {len(stale)} — "
              "سِجِلٌّ يوثّق ما حُذف:")
        for name in stale[:15]:
            print(f"      · {name}")
        problems.append(f"مقيسٌ ولم يعُد معلَنًا: {len(stale)}")

    if problems:
        for note in problems:
            print(f"  ✗ {note}")
        print("\n  العلاج: أعِد تشغيل المِجَسّ على ثنائيَّين محدَّثَين:")
        print("    python scripts/codegen/probe_builtin_engines.py"
              " --build-dir build/bin/Release")
        return 1

    print(f"  ✓ التغطيةُ تامّةٌ والقياسُ طازج — المحرّكان معًا: "
          f"{derived['both']} · المفسّر وحده: {derived['interpreter_only']} · "
          f"المترجّم وحده: {derived['compiler_only']} · "
          f"لا أحد: {derived['neither']}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
