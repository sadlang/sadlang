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
from builtin_arity_extract import ArityCheck, arity_checks  # noqa: E402

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
    inline_sites: list[ArityCheck] = []
    inline_named: set[tuple[str, str]] = set()
    silent_sites: list[ArityCheck] = []
    for site in sites:
        # (AR) الرفضُ في الشرطِ يُعزَل قبلَ كلِّ حكم: ليس رقمًا حرفيًّا يُمرَّر
        #      إلى حارسٍ (فلا يُحمِّر)، وليس ختمًا من مصدرِ الحقيقةِ (فلا يُحتسَب
        #      مفروضًا). صنفٌ ثالثٌ يُعدّ ويُعلَن على حدة.
        if site.silent:
            silent_sites.append(site)
            continue
        if site.inline:
            inline_sites.append(site)
            inline_named.update(site.names)
            continue
        enforced.update(site.names)
        # (AR) ثابتُ الرتبةِ المُنتقى لأخٍ في الذراعِ نفسِها فرضٌ له باسمِه —
        #      وإغفالُه يُحمِّرُ الحارسَ بـ«إعلانٌ ميّت» على عقدٍ مفروضٍ فعلًا.
        enforced.update(site.also_constants)
        # (AR) وثابتُ الرتبةِ المُمرَّرُ إلى الحارسِ فرضٌ لصاحبِه **مهما كانت
        #      صيغةُ مقارنةِ الاسمِ في الذراع**: أذرعُ اللاتزامنِ تقارنُ بايتاتٍ
        #      مهرَّبةً لا ثوابتَ أسماءٍ، فلا يرى المستخرِجُ لها اسمًا — وقراءةُ
        #      ذلك «عقدًا لا يُطبَّق» تُحمِّرُ على حراسةٍ قائمةٍ فعلًا. وبقاءُ
        #      المقارنةِ بالبايتاتِ دَينٌ آخرُ يُسمّى على حدة.
        if site.constant:
            enforced.add(site.constant)
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
    if silent_sites:
        # (AR) 🔑 الذراعُ الخامسة: رفضٌ لا يسجّلُ خطأً. المصرّفُ يخرجُ بصفرٍ
        #      ويُنتجُ ثنائيًّا **بلا النداء** — سطرٌ في كودِ نواةٍ «يُنفَّذ»
        #      وهو غيرُ موجود. لا يُخفق فلا يُرى، وهو أخطرُ من الرقمِ الحرفيّ
        #      بمراتب: ذاك عقدٌ ينجرف، وهذا عملٌ يتبخّر.
        problems.append(
            f"أذرعُ رفضٍ لا تسجّلُ خطأً ({len(silent_sites)}) ⇒ النداءُ يتبخّرُ "
            "والمصرّفُ يخرجُ بصفر:\n    "
            + "\n    ".join(f"{s.file}:{s.line} — {s.silent}"
                            for s in silent_sites[:25]))

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

    # (AR) الدَّينُ يُقسَم قسمين لأنّهما مختلفان في الخطرِ وفي العلاج:
    #      ما يُرفَض برقمٍ محلّيّ (الرتبةُ مفروضةٌ، والعلاجُ رفعُ العددِ إلى
    #      مصدرِ الحقيقة)، وما لا يفرضُه أحدٌ (النداءُ يتبخّرُ صامتًا، والعلاجُ
    #      اشتقاقُ عقدٍ من الشيفرةِ لا اختراعُه). جمعُهما رقمًا واحدًا يُقرأ
    #      أسوأَ من الواقعِ ويخفي أيَّهما يلزمُ العملُ عليه أوّلًا.
    sealed_sites = [s for s in sites if not s.inline]
    inline_only = sorted(inline_named - declared.keys())
    print("حارس «رتبةُ المدمجِ من مصدرِ الحقيقة»:")
    print(f"  مُعلَنةٌ ومفروضة: {len(declared)} مدمجًا · مواضعُ فحص: "
          f"{len(sealed_sites)} (كلُّها بثوابتَ مُولَّدة)")
    print(f"  مفروضةٌ برقمٍ في الشرطِ لا من مصدرِ الحقيقة: {len(inline_only)} مدمجًا "
          f"في {len(inline_sites)} موضعَ رفض — الرتبةُ محروسةٌ والعددُ منفلت.")
    print(f"  لا يفرضُها أحد: {total - len(declared) - len(inline_only)} من {total} "
          "مدمجًا — النداءُ الناقصُ يمرّ صامتًا. لا يُخترَع لها عقد، بل يُشتقّ.")
    print("  ✓ لا رقمَ حرفيًّا، ولا ثابتًا غريبًا، ولا إعلانًا ميّتًا، ولا فرضًا "
          "غيرَ مُعلَن.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
