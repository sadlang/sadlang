#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «سِجِلُّ قياسِ المحرّكَين يُغطّي ما هو معلَنٌ اليوم».

     المُخرَجُ `language-truth/_meta/builtin_engine_support.yaml` **لقطةُ قياسٍ**
     على ثنائيَّين مبنيَّين في لحظةٍ بعينها، ويُنشَرُ في التوثيقِ عمودَي «مترجّم»
     و«مفسّر». وأخطرُ ما يصيبُه ليس الخطأَ بل **التعفّنَ الصامت**: يُضافُ مدمَجٌ
     إلى مصدرِ الحقيقةِ ولا يُعادُ القياس، فتظهرُ صفحتُه في التوثيقِ بلا عمودٍ —
     أو أسوأ: يُحذَفُ مدمَجٌ ويبقى سطرُه في السِّجِلّ فيوثَّقُ ما لا وجودَ له.

     ⚠️ وحدُّ هذا الحارسِ مقصودٌ ومُعلَن: يقيسُ **التغطية** (أيُّ الأسماءِ
        مذكورة) لا **الصحّة** (أصادقٌ حكمُ كلِّ سطر). صدقُ الحكمِ لا يُعرَفُ إلّا
        بإعادةِ تشغيلِ المِجَسّ على ثنائيَّين مبنيَّين، وهو أثقلُ من بوّابةِ PR.
        فيُقالُ ما يُقاس: هذا يحرسُ التغطيةَ وحدَها، ولا يُدَّعى أنّه يحرسُ أكثر.

     العلاج عند الاحمرار:
         python scripts/codegen/probe_builtin_engines.py --build-dir build/bin/Release

(EN) Guards that the measured engine-support record covers exactly the set of
     builtins declared today. Coverage only — not per-row correctness.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"
RECORD = ROOT / "language-truth" / "_meta" / "builtin_engine_support.yaml"


def declared() -> set:
    names = set()
    for path in sorted(SOT_DIR.glob("*.yaml")):
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            names.add(fn["canonical"])
    return names


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

    missing = sorted(sot - measured)   # معلَنٌ ولم يُقَس ⇒ عمودٌ فارغٌ في التوثيق
    stale = sorted(measured - sot)     # مقيسٌ ولم يعُد معلَنًا ⇒ توثيقُ ما لا وجودَ له

    calib = doc.get("calibration") or {}
    ok_calib = (calib.get("absent_agreed") == calib.get("absent_sample")
                and calib.get("present_agreed") == calib.get("present_sample")
                and calib.get("absent_sample"))

    print("حارس «سِجِلُّ قياسِ المحرّكَين مُغطٍّ»:")
    print(f"  معلَنٌ في مصدر الحقيقة: {len(sot)}")
    print(f"  مقيسٌ في السِّجِلّ: {len(measured)}")
    print(f"  قِيس على الإيداع: {doc.get('measured_commit', '—')}")
    print(f"  المعايرة مدوَّنةٌ وسليمة: {'نعم' if ok_calib else 'لا'}")

    failed = False
    if missing:
        print(f"  ✗ معلَنٌ ولم يُقَس: {len(missing)} — "
              "سيظهر في التوثيق بلا عمودِ محرّك:")
        for name in missing[:15]:
            print(f"      · {name}")
        failed = True
    if stale:
        print(f"  ✗ مقيسٌ ولم يعُد معلَنًا: {len(stale)} — "
              "سِجِلٌّ يوثّق ما حُذف:")
        for name in stale[:15]:
            print(f"      · {name}")
        failed = True
    if not ok_calib:
        print("  ✗ المعايرةُ غائبةٌ أو مختلَّةٌ في السِّجِلّ — "
              "رقمٌ من أداةٍ لم تُعايَر لا يُنشَر.")
        failed = True

    if failed:
        print("\n  العلاج: أعِد تشغيل المِجَسّ على ثنائيَّين محدَّثَين:")
        print("    python scripts/codegen/probe_builtin_engines.py"
              " --build-dir build/bin/Release")
        return 1

    counts = doc.get("counts") or {}
    print(f"  ✓ التغطيةُ تامّة — المحرّكان معًا: {counts.get('both', '—')} · "
          f"المفسّر وحده: {counts.get('interpreter_only', '—')} · "
          f"المترجّم وحده: {counts.get('compiler_only', '—')} · "
          f"لا أحد: {counts.get('neither', '—')}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
