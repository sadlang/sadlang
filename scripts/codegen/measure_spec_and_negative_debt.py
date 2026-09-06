#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِقياسُ عدّادَي **ج** و**د** — الدَّينُ الذي كان بلا أمرٍ منشور.

     ستّةُ عدّاداتٍ منشورةٍ في `CLAUDE.md`، وأربعةٌ منها لها أمرٌ يقيسُها؛ و«ج»
     و«د» كانا نثرًا وحدَه — «دَينٌ مُقيَّدٌ لا سكوتٌ عنه» كما يقولُ نصُّها.
     ونثرٌ بلا أمرٍ يبلى: قِيسَ عندَ كتابةِ هذا الملفِّ أنّ «د» صارَ **٣٣١**
     والمنشورُ ٣٣٠، وأنّ تفصيلَه (٢٧٨) والمنشورَ (٢٧٧).

     **ج · مدخلاتٌ بلا مواصفة** — نطاقُه `language-truth/builtins/` بعينِه
     (١٢٠٥ مدخلًا)، وهو ما يقيسُه `check_builtin_engine_coverage.py`.
     🔑 و`language-truth/stdlib/` **خارجَ نطاقِه** — وقد كان ذلك مسكوتًا عنه،
        فيُطبَعُ ههنا صفًّا مستقلًّا: دَينُه مُسمًّى لا مخبوءٌ في عدَّادٍ آخر.

     **د · سوالبُ يتيمة** — بذرةٌ تحملُ `@expect_error` **ولا تحملُ** شقيقَيه
     المترجَمَين (`@expect_compile_error` · `@expect_error_compiled`). فهي
     سالبةٌ لا مقابلَ لها في مسارِ الترجمة. وتُصنَّفُ ثلاثًا: بلا نمطٍ أصلًا ·
     برمزِ خطأٍ من كتالوجِ SoT (يُوجَّه) · بنثرِ محرّكٍ (دَينٌ مُسمًّى).

     🔑 **والقرّاءُ مُورَّثون**: العدّاءُ (`tests/runner.py`) هو مَن يقرأُ الوسمَ،
        وشكلُ الرمزِ من `check_seed_contract.py`. ولا هجاءَ رابعٌ ههنا — وهو
        ما يحرسُه `check_seed_tag_readers.py`.

     ⚠️ **ولا سجلَّ عيارٍ مُودَعٌ لها بعدُ — دَينٌ مُسمًّى.** واسمُها يبدأُ
        بـ`measure_` فلا يمسحُها `check_calibration_fresh.py` (يمسحُ `check_*`
        وحدَها)، فصمتُه عنها **ليس تغطية**. وعيارُها اليومَ برهانُ عضٍّ خارجَ
        الشجرةِ (خمسُ حقناتٍ: أساسٌ أخضر · «د» يرتفعُ · «ج» ينقصُ · نطاقٌ
        مفقودٌ برمزِ ٢ · القرّاءُ مُورَّثون).

(EN) Measures counters C (SoT entries lacking a spec) and D (orphan negative
     seeds). Both were prose-only; prose without a command rots — D had.
     No committed calibration record yet: a named debt, not coverage.
============================================================================
"""
from __future__ import annotations

import argparse
import io
import sys
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
RUNNER = ROOT / "tests" / "runner.py"


def _load_runner():
    """(AR) 🔑 العدّاءُ يُحمَّلُ **بمسارِه** لا باسمِه: في الشجرةِ `runner.py`
    ثانٍ (`tests/system/docs/doc_gen/`)، وأيُّ `runner` أسبقُ في `sys.path`
    يُظلِّلُ المقصودَ فيُقاسُ غيرُه أو يُنهار."""
    import importlib.util

    if not RUNNER.is_file():
        raise AssertionError("العدّاءُ مفقود: %s" % RUNNER)
    spec = importlib.util.spec_from_file_location("sad_seed_runner", RUNNER)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module

from check_seed_contract import _seeds  # noqa: E402

TRUTH = ROOT / "language-truth"
SCOPES = ("builtins", "stdlib")


def measure_spec(scope: str):
    """(AR) مدخلاتُ نطاقٍ من SoT: كم بلا `returns`، كم بلا `params`، وكم معاملًا
    مكتوبَ النوع."""
    entries = returns = typed = 0
    # (AR) 🔑 **و«بلا params» كان يجمعُ ثلاثًا في واحدة.** قِيسَ: من ١٠٥٤
    #      «بلا params» في المدمجات **٧٢** مدخلًا `arity {min:0,max:0}` — أي
    #      دالّةٌ بلا معاملاتٍ حقًّا ومواصفتُها **تامّة**. فعدُّها دَينًا تحتَ
    #      عنوانِ «مدخلاتٌ بلا مواصفة» يُضخِّمُ العدَّادَ بما ليس منه.
    missing_key = empty_debt = empty_complete = 0
    directory = TRUTH / scope
    if not directory.is_dir():
        raise AssertionError("نطاقٌ مفقودٌ في مصدرِ الحقيقة: %s" % directory)
    for path in sorted(directory.glob("*.yaml")):
        doc = yaml.safe_load(io.open(str(path), encoding="utf-8")) or {}
        for item in (doc.get("functions") or []):
            if not isinstance(item, dict):
                continue
            entries += 1
            if not item.get("returns"):
                returns += 1
            arity = item.get("arity") or {}
            zero_arity = (isinstance(arity, dict)
                          and arity.get("min") == 0 and arity.get("max") == 0)
            if "params" not in item:
                missing_key += 1
            elif not item.get("params"):
                if zero_arity:
                    empty_complete += 1
                else:
                    empty_debt += 1
            else:
                typed += sum(1 for one in (item.get("params") or [])
                             if isinstance(one, dict) and one.get("type"))
    if not entries:
        raise AssertionError("صفرُ مدخلٍ في %s — لم يُقَسْ شيء" % scope)
    return entries, returns, missing_key, empty_debt, empty_complete, typed


def measure_orphan_negatives(executor):
    """(AR) سالبةٌ بلا مقابلٍ مترجَم — والقارئُ هو العدّاءُ نفسُه.

    🔑 و**التوجيهُ قرارُ العدّاءِ** (`_RE_ERROR_CODE` مطابقةً تامّة)، فيُورَّثُ
       قارئُه لا يُبنى سطرٌ مصطنعٌ يُطابَقُ عليه بهجاءِ حارسٍ آخر: قِيسَ أنّ
       حمولةً كـ`SEM001 وصفٌ نثريّ` تُصنَّفُ «رمزًا» عندَ الأداةِ و«نثرًا» عندَ
       العدّاءِ الذي يُنفِّذُ القرار (صفرُ حالةٍ في الشجرةِ اليوم — كامنٌ لا حيّ).
    """
    no_pattern = code = prose = 0
    for row in _seeds():
        path = ROOT / row["rel"]
        if not path.is_file():
            continue
        meta = executor.parse_metadata(path)
        if not meta.expect_error:
            continue
        if meta.expect_compile_error or meta.expect_error_compiled:
            continue          # لها مقابلٌ مترجَمٌ — ليست يتيمة
        payload = (meta.expect_error or "").strip()
        # (AR) `__ANY_ERROR__` يضعُه العدّاءُ آليًّا لوسمٍ بلا حمولة — فهو
        #      «بلا نمط» لا نمطًا. (ولا يُكتَبُ في بذرةٍ قطُّ، بنصِّ العدّاء.)
        if payload in ("", "__ANY_ERROR__"):
            no_pattern += 1
        elif executor._RE_ERROR_CODE.match(payload):
            code += 1
        else:
            prose += 1
    return no_pattern, code, prose


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    parser = argparse.ArgumentParser(add_help=True, description=__doc__)
    parser.add_argument("--ج", dest="expect_spec", type=int, default=None,
                        help="(AR) المتوقَّعُ لـ«ج» (بلا returns في المدمجات)"
                             " — يُخفِقُ إن خالفَه المقيس")
    parser.add_argument("--د", dest="expect_orphans", type=int, default=None,
                        help="(AR) المتوقَّعُ لـ«د» — يُخفِقُ إن خالفَه المقيس")
    args = parser.parse_args()

    try:
        executor = _load_runner()
    except Exception as exc:  # noqa: BLE001 — تعذُّرُ التحميلِ عطبُ آلة
        print("✗ عطبُ آلة: تعذَّرَ تحميلُ العدّاء: %s: %s"
              % (exc.__class__.__name__, exc))
        return 2

    print("═" * 74)
    print("  مِقياسُ عدّادَي ج و د — التعريفُ في هذا الملفِّ لا في نثرٍ خارجَه")
    print("═" * 74)

    rows = {}
    for scope in SCOPES:
        entries, returns, missing, debt, complete, typed = measure_spec(scope)
        rows[scope] = (entries, returns, missing, debt, complete, typed)
        print("  ج · %-9s %5d مدخلًا · بلا returns %5d · بلا params %5d"
              " (بلا مفتاحٍ %4d · فارغةٌ ودَينٌ %4d · فارغةٌ وتامّة %3d)"
              " · معاملٌ بنوع %3d"
              % (scope, entries, returns, missing + debt + complete,
                 missing, debt, complete, typed))
    print("      ⤷ «ج» المنشورُ نطاقُه `builtins` وحدَه؛ و`stdlib` دَينٌ مُسمًّى.")
    print("      ⤷ و«فارغةٌ وتامّة» ليست دَينًا: `arity {min:0,max:0}` — دالّةٌ"
          " بلا معاملاتٍ حقًّا.")

    no_pattern, code, prose = measure_orphan_negatives(executor)
    orphans = no_pattern + code + prose
    print("─" * 74)
    print("  د · سوالبُ يتيمة        %5d · بلا نمطٍ %4d · برمزِ SoT %4d"
          " · بنثرِ محرّكٍ %4d" % (orphans, no_pattern, code, prose))
    print("─" * 74)

    failures = []
    if args.expect_spec is not None and args.expect_spec != rows["builtins"][1]:
        failures.append("ج: المتوقَّع %d والمقيس %d"
                        % (args.expect_spec, rows["builtins"][1]))
    if args.expect_orphans is not None and args.expect_orphans != orphans:
        failures.append("د: المتوقَّع %d والمقيس %d" % (args.expect_orphans, orphans))
    for line in failures:
        print("  ❌ %s" % line)
    if failures:
        return 1
    print("  ✅ مقيسٌ — ولا عددَ منثورًا يُصدَّقُ بلا أمرِه.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — انهيارٌ ليس حكمًا
        sys.stdout.reconfigure(encoding="utf-8")
        print("✗ عطبُ آلةٍ — لم يُقَسْ شيء: %s: %s" % (exc.__class__.__name__, exc))
        raise SystemExit(2)
