#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «رتبةُ المدمجِ تُعلَن مرّةً واحدةً في مصدرِ الحقيقة».

     يُحرَس من أربعةِ أطراف:

       (١) **رقمٌ حرفيّ**: موضعُ فحصٍ يمرّرُ عددًا مكتوبًا في الشيفرة ⇒ نسخةٌ
           ثانيةٌ للحقيقةِ تنجرفُ بلا أن يحمرَّ شيء. المطلوبُ ثابتٌ مُولَّد.
       (٢) **ثابتٌ غريب**: ذراعٌ تحرسُ أسماءً وتقيسُ بثابتِ مدمجٍ **آخرَ** ⇒
           رتبةٌ صحيحةُ الشكلِ خاطئةُ المرجع، وهي أخفى من الرقمِ الحرفيّ.
       (٣) **إعلانٌ ميّت**: `arity` في مصدرِ الحقيقةِ لا تفرضُه ذراعٌ ⇒ عقدٌ
           لا يُطبَّق أبدًا وخضرةٌ بلا حراسة.
       (٤) **فرضٌ غيرُ مُعلَن**: ذراعٌ تفرضُ رتبةً لمدمجٍ بلا `arity` في مصدرِ
           الحقيقة ⇒ الحقيقةُ في الشيفرةِ لا في مصدرها.

     والدَّينُ المتبقّي (مدمجاتٌ لا رتبةَ مفروضةً لها أصلًا) **يُطبَع بعددِه**
     ولا يُبتَلَع: صمتُ الحارسِ عن غيرِ المقيسِ يُقرَأ تغطيةً وهو ليس بها.
(EN) Guard: builtin arity is declared once in SoT — no literals, no foreign
     constants, no dead declarations, no unmeasured enforcement. Debt printed.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

import yaml

sys.path.insert(0, str(Path(__file__).resolve().parent))
from builtin_arity_extract import arity_checks  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
SOT_DIR = ROOT / "language-truth" / "builtins"


def _sot_arities() -> tuple[dict[tuple[str, str], tuple[int, int]], int]:
    table: dict[tuple[str, str], tuple[int, int]] = {}
    total = 0
    for path in sorted(SOT_DIR.glob("*.yaml")):
        doc = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
        for fn in doc.get("functions") or []:
            total += 1
            arity = fn.get("arity")
            if arity:
                table[(fn["namespace"], fn["cpp_id"])] = (
                    int(arity["min"]), int(arity["max"]))
    return table, total


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    declared, total = _sot_arities()
    sites = arity_checks()

    literals: list[str] = []
    foreign: list[str] = []
    enforced: set[tuple[str, str]] = set()
    for site in sites:
        enforced.update(site.names)
        if site.literal:
            literals.append(f"{site.file}:{site.line}")
            continue
        # (AR) الثابتُ يجب أن يكون لأحدِ أسماءِ الذراعِ نفسِها — وإلّا فالرتبةُ
        #      تُقرأ من مدمجٍ آخر، فتنجرفُ بصمتٍ حين تتغيّرُ رتبةُ ذاك.
        if site.constant and site.names and site.constant not in site.names:
            names = "، ".join(f"{ns}::{cid}" for ns, cid in site.names)
            foreign.append(
                f"{site.file}:{site.line} — الذراعُ تحرسُ [{names}] وتقيسُ بثابتِ "
                f"{site.constant[0]}::{site.constant[1]}")

    problems: list[str] = []
    if literals:
        problems.append(
            "رتبٌ مكتوبةٌ أرقامًا حرفيّةً في مواضعِ الفحصِ (نسخةٌ ثانيةٌ للحقيقة):\n    "
            + "\n    ".join(literals))
    if foreign:
        problems.append(
            "مواضعُ تقيسُ بثابتِ مدمجٍ آخر:\n    " + "\n    ".join(foreign))

    dead = sorted(declared.keys() - enforced)
    if dead:
        problems.append(
            f"إعلاناتُ `arity` لا تفرضُها ذراعٌ ({len(dead)}) ⇒ عقدٌ لا يُطبَّق:\n    "
            + "\n    ".join(f"{ns}::{cid}" for ns, cid in dead[:20]))

    unmeasured = sorted(enforced - declared.keys())
    if unmeasured:
        problems.append(
            f"أذرعٌ تفرضُ رتبةً بلا `arity` في مصدرِ الحقيقة ({len(unmeasured)}):\n    "
            + "\n    ".join(f"{ns}::{cid}" for ns, cid in unmeasured[:20]))

    if problems:
        print("حارس «رتبةُ المدمجِ من مصدرِ الحقيقة»: ✗")
        for problem in problems:
            print("  " + problem)
        return 1

    print("حارس «رتبةُ المدمجِ من مصدرِ الحقيقة»:")
    print(f"  مُعلَنةٌ ومفروضة: {len(declared)} مدمجًا · مواضعُ فحص: {len(sites)}"
          " (كلُّها بثوابتَ مُولَّدة)")
    print(f"  دَينٌ معلَن: {total - len(declared)} من {total} مدمجًا لا رتبةَ "
          "مفروضةً لها في الأماميّة بعدُ — تُختَم عند فرضِها.")
    print("  ✓ لا رقمَ حرفيًّا، ولا ثابتًا غريبًا، ولا إعلانًا ميّتًا، ولا فرضًا "
          "غيرَ مُعلَن.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
