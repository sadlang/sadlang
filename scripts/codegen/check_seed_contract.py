#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «البذرةُ تُحاكَمُ على عقدٍ، والعقدُ يُسمّي ما يُنتظَر».

     بذرةٌ تُشغَّلُ ولا يُقابَلُ مخرَجُها بشيءٍ **تمرُّ دائمًا** — فهي سطرٌ في عدَّادٍ
     لا برهان. وأقوى صورةِ «أخضرَ لأنّه لا يستطيعُ أن يحمرّ» في شجرةِ البذور.

     أربعةُ لامتغيِّرات:

       ① **تُشغَّلُ بلا عقد**: ليست متخطّاةً، ولا `@expected`، ولا وسمَ سالب
          ⇒ تُشغَّلُ ولا تُحاكَمُ على شيء. سقفٌ **نازل**.

       ② **متخطّاة** (`@skip_compiler`): تُعَدُّ ولا تُشغَّل. سقفٌ **نازل**.
          والفرقُ عن ① جوهريّ: المتخطّاةُ **مُعلَنةٌ صراحةً**، والتي بلا عقدٍ
          تُوهِمُ بأنّها تُقاس.

       ③ **عقدانِ متناقضان**: `@expected` مع وسمٍ سالبٍ في بذرةٍ واحدة —
          «يُنتظَرُ هذا المخرَج» و«يُنتظَرُ أن تفشل» معًا ⇒ **صفر**.
          🔑 والواحدةُ التي كانت كذلك أثرُ عصرِ المحرّكَين: المترجّمُ يرفضُ
          بـSEM042 **والمفسّرُ يطبعُ 0**. والمفسّرُ محذوف، فبقيَ نصفُ العقدِ
          يُشيرُ إلى محرّكٍ زال.

       ④ **وسمٌ سالبٌ بلا رمزِ خطأ**: «تفشلُ بشيءٍ ما» لا «تفشلُ بـSEM042».
          عقدٌ ضعيفٌ يمرُّ على أيِّ فشلٍ ولو كان انهيارَ الأداة. سقفٌ **نازل**.

     🔑 **والهجاءُ الكاملُ يُقرأُ ولا يُفترَض.** الوسمُ السالبُ ثلاثةُ هجاءاتٍ في
        الشجرة (`@expect_error` · `@expect_compile_error` · `@expect_error_compiled`)،
        و`compile_` يقعُ **بينَ** `expect_` و`error` فلا لاحقةٌ تبلغُه. وقِيسَ أنّ
        `measure_seed_contract_gap.py` كان يعمى عنه فيعُدُّ **خمسَ بذورٍ لها عقدٌ
        سالبٌ صحيحٌ** ضمنَ «بلا عقد». والدرسُ مُدوَّن: حارسٌ يقرأُ أقلَّ ممّا في
        الشجرةِ يُعلِنُ سدَّ ثغرةٍ ويتركُها مفتوحة.

(EN) Seed-contract guard: a seed that runs but is judged on nothing always
     passes. Four invariants with descending ceilings; contradictory contracts
     are zero-tolerance. The negative mark has three committed spellings and all
     three are read — an infix `compile_` is unreachable by any suffix pattern.
============================================================================
"""
from __future__ import annotations

import collections
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEEDS = ROOT / "tests" / "behavior"

# (AR) شجرةٌ محفوظةٌ لا تُشغَّل — استثناؤها جزءٌ من التعريفِ لا تفصيلُ تنفيذ،
#      وهو عُرفٌ قائمٌ في أربعةِ حرّاسَ قبلَ هذا.
SKIP_PARTS = ("_archive",)

# ═══ الوسومُ — بالهجاءِ الكاملِ وبفراغِ السطرِ الواحدِ لا `\s` ══════════════
# (AR) و`[ \t]` لا `\s`: في `re.M` يعبرُ `\s` نهايةَ السطرِ فيبتلعُ الوسمُ العاري
#      سطرَ الشفرةِ التالي حمولةً — وهو فخُّ رفضٍ كاذبٍ قِيسَ في حارسِ المرساة.
_SKIP = re.compile(r"^#[ \t]*@skip_compiler", re.M)
# (AR) 🔑 **شكلُ العقدِ يُشتَقُّ من العدّاءِ لا يُخترَعُ ههنا.**
#      `tests/runner.py` يشترطُ `@expected:?\s+(.+)` — أي **حمولةً**. وكان هذا
#      يقبلُ الوسمَ عاريًا (`# @expected:` بلا قيمة)، فبذرةٌ بنقطتَين تُسكِتُ
#      الحارسَ **والعدّاءُ يتخطّاها فلا تُشغَّلُ أصلًا**: خضرةٌ كاذبةٌ تلتفُّ على
#      أطروحةِ الحارسِ كلِّها بمحرفٍ واحد. وفي الشجرةِ بذرةٌ كذلك اليوم.
#      (والدرسُ مُدوَّن: حارسٌ يقرأُ غيرَ ما يقرؤه مَن يُنفِّذُ العقدَ يحرسُ وهمًا.)
_EXPECTED = re.compile(r"^#[ \t]*@expected:?[ \t]+(?=\S)", re.M)
_NEGATIVE = re.compile(r"^#[ \t]*@expect(?:_compile)?_error[a-z_]*", re.M)
# (AR) وشكلُ الرمزِ يُشتقُّ من عقدِ العدّاءِ (`tests/runner.py`) لا يُخترَعُ ههنا.
_NEG_CODE = re.compile(r"^#[ \t]*@expect(?:_compile)?_error[a-z_]*:?[ \t]+"
                       r"([A-Z]{2,4}[0-9]{3})(?![0-9])", re.M)

# ═══ السقوفُ المُودَعة — مُقاسةٌ في ٢٠٢٦-٠٩-٠٥ ═══════════════════════════════
# (AR) ① تُشغَّلُ بلا عقد. ١٦٩ ← ١٦٤ بقراءةِ الهجاءِ الذي كان المقياسُ القائمُ
#      يعمى عنه، ثمّ ١٦٤ ← ١٦٥ بردِّ **العقدِ الفارغ**: وسمٌ بلا حمولةٍ لا يراه
#      العدّاءُ عقدًا فيتخطّى البذرةَ — فعدُّه عقدًا كان يُخفي بذرةً لا تُشغَّل.
CEILING_NO_CONTRACT = 165

# (AR) ② متخطّاة. مُعلَنةٌ صراحةً فهي دَينٌ مقروءٌ لا وهمُ قياس.
CEILING_SKIPPED = 77

# (AR) 🔑 **أرضيّةُ المتعاقَدِ — تصعدُ ولا تهبط.** السقوفُ تحرسُ نموَّ الدَّينِ
#      وحدَه، **وانكماشُ المجموعةِ المتعاقَدةِ يُقرأُ سكونًا**: حذفُ مئتَي بذرةٍ
#      ذاتِ عقدٍ يُبقي ①②③④ كما هي فيخضرُّ الحارسُ ومئتا برهانٍ زالت.
#      وأختُه المُودَعةُ `check_anchor_integrity.py` تحرسُ هذا بـ`FLOOR_DECLARED`.
FLOOR_CONTRACTED = 4303

# (AR) ③ عقدانِ متناقضان — صفرٌ لا سقفَ له.
CEILING_CONTRADICTORY = 0

# (AR) ④ وسمٌ سالبٌ بلا رمز. عقدٌ يقولُ «تفشل» ولا يقولُ «بماذا»، فيمرُّ على
#      انهيارِ الأداةِ كما يمرُّ على الرفضِ المقصود. سقفٌ نازلٌ لا صفرٌ اليوم:
#      العددُ يُسمّى ثمّ ينزلُ بالعملِ، والصفرُ الآنَ كذبٌ يُسكَّنُ ولا يُصلَح.
CEILING_UNNAMED_FAILURE = 360


RUNNER = ROOT / "tests" / "runner.py"
# (AR) 🔑 **النافذةُ تُشتَقُّ من العدّاءِ ولا تُنسَخ.** `parse_metadata` يقفُ عندَ
#      حدٍّ ثابتٍ، فوسمٌ بعدَه **عقدٌ عندَ الحارسِ وعدمٌ عندَ مَن يُنفِّذُه** —
#      وهو عينُ ما تنهى عنه ترويسةُ هذا الملفّ: «حارسٌ يقرأُ غيرَ ما يقرؤه
#      مُنفِّذُ العقدِ يحرسُ وهمًا». طُوبِقَ النمطُ في الجولةِ الثالثةِ ولم
#      تُطابَقِ النافذة. ولو نُسِخَ العددُ لصارَ نسخةً ثانيةً تبلى بلا صوت.
_WINDOW = re.compile(r"def parse_metadata\b.*?if i >= (\d+):", re.S)


def _runner_window() -> int:
    """(AR) حدُّ الأسطرِ الذي يقرؤه العدّاء. تعذُّرُ اشتقاقِه **عطبُ آلة** لا
    حكمٌ: حارسٌ لا يعرفُ نافذةَ مَن يُنفِّذُ العقدَ لم يقِسْ شيئًا (رمزُ ٢)."""
    match = _WINDOW.search(RUNNER.read_text(encoding="utf-8"))
    if not match:
        raise AssertionError(
            "تعذَّرَ اشتقاقُ نافذةِ الوسومِ من %s — أتغيَّرَ `parse_metadata`؟"
            % RUNNER.relative_to(ROOT).as_posix())
    window = int(match.group(1))
    if not 1 <= window <= 10_000:
        raise AssertionError("نافذةٌ غيرُ معقولة: %d" % window)
    return window


def _seeds() -> list[dict]:
    rows: list[dict] = []
    window = _runner_window()
    for path in sorted(SEEDS.rglob("*.ص")):
        if any(part in SKIP_PARTS for part in path.parts):
            continue
        # (AR) `utf-8-sig` لا `utf-8`: بادئةُ BOM تسبقُ `#` فتُسقِطُ وسمَ السطرِ
        #      الأوّل. ولا `errors="replace"`: ملفٌّ لا يُفَكُّ **لم يُقَسْ**،
        #      فيصعدُ الاستثناءُ إلى غلافِ الرمز ٢.
        text = path.read_text(encoding="utf-8-sig")
        text = "\n".join(text.split("\n")[:window])
        rows.append({
            "rel": path.relative_to(ROOT).as_posix(),
            "top": path.relative_to(SEEDS).parts[0],
            "skip": bool(_SKIP.search(text)),
            "expected": bool(_EXPECTED.search(text)),
            "negative": bool(_NEGATIVE.search(text)),
            "code": bool(_NEG_CODE.search(text)),
        })
    return rows


# ═══ عيارُ الأداةِ نفسِها ═══════════════════════════════════════════════════
# (AR) قناريّةٌ سالبةٌ إلى جانبِ الموجبة: دعوى الوجودِ وحدَها يجتازُها حارسٌ
#      يزعمُ أنّ كلَّ بذرةٍ ذاتُ عقد.
def _calibrate(rows: list[dict]) -> list[str]:
    """(AR) خمسةُ مجسّاتٍ تُنفَّذُ في كلِّ تشغيل. إخفاقُ أحدِها = عطبُ آلة.

    🔑 ولا سادسَ لـ`_runner_window() < 1`: الاشتقاقُ يرمي سلفًا على أيِّ نافذةٍ
    خارجَ `1..10_000`، فكان بندًا **لا يمكنُ أن يحمرَّ بأيِّ مُدخَل** — عددٌ
    يُزيِّنُ «٦/٦» ويعُدُّ فرعًا ميّتًا. والاشتقاقُ نفسُه هو الجسّ: تعذُّرُه
    رمزُ ٢، ونتيجتُه تُطبَعُ في سطرِ العيارِ فتُقابَل.
    """
    fails: list[str] = []
    if len(rows) < 3000:
        fails.append(f"المسحُ عادَ بـ{len(rows)} بذرةً — والشجرةُ فيها آلاف")
    if not any(r["expected"] for r in rows):
        fails.append("لا بذرةَ ذاتَ `@expected` — قارئُ العقدِ أعمى")
    if not any(r["negative"] for r in rows):
        fails.append("لا بذرةَ ذاتَ وسمٍ سالب — قارئُ السالبِ أعمى")
    if not any(r["skip"] for r in rows):
        fails.append("لا بذرةَ متخطّاة — قارئُ التخطّي أعمى")
    # (AR) المجسُّ السالب: مستحيلٌ أن تحملَ كلُّ بذرةٍ عقدًا. وحارسٌ مُفرَغٌ
    #      يزعمُ ذلك يجتازُ دعاوى الوجودِ كلَّها.
    if all(r["expected"] or r["negative"] or r["skip"] for r in rows):
        fails.append("كلُّ بذرةٍ ذاتُ عقدٍ في القراءة — قارئٌ يوسِّعُ لا يقيس")
    return fails


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    rows = _seeds()

    probes = 5
    fails = _calibrate(rows)
    print("حارس «البذرةُ تُحاكَمُ على عقدٍ، والعقدُ يُسمّي ما يُنتظَر»:")
    if fails:
        print(f"  ✗ عيارُ الأداة: {probes - len(fails)}/{probes}"
              " — لا يُبنى على قراءتِها:")
        for msg in fails:
            print(f"      · {msg}")
        return 2

    live = [r for r in rows if not r["skip"]]
    skipped = [r for r in rows if r["skip"]]
    no_contract = [r for r in live if not r["expected"] and not r["negative"]]
    contradictory = [r for r in live if r["expected"] and r["negative"]]
    unnamed = [r for r in live if r["negative"] and not r["code"]]

    print(f"  عيارُ الأداة: {probes}/{probes} ✓"
          f" (بذورٌ ممسوحة: {len(rows)} · تُشغَّل: {len(live)}"
          f" · نافذةُ الوسوم: {_runner_window()} سطرًا — مشتقّةٌ من العدّاء)")

    contracted = len(live) - len(no_contract)
    floor_mark = "" if contracted >= FLOOR_CONTRACTED else "  ✗ انكمشَ المتعاقَد"
    print(f"  ⑤ بذورٌ ذاتُ عقدٍ                    {contracted:>5}"
          f" (الأرضيّة {FLOOR_CONTRACTED} — تصعدُ ولا تهبط){floor_mark}")

    checks = (
        ("① تُشغَّلُ بلا عقد", no_contract, CEILING_NO_CONTRACT, "نازل"),
        ("② متخطّاة (@skip_compiler)", skipped, CEILING_SKIPPED, "نازل"),
        ("③ عقدانِ متناقضان", contradictory, CEILING_CONTRADICTORY, "لا سقفَ له"),
        ("④ وسمٌ سالبٌ بلا رمزِ خطأ", unnamed, CEILING_UNNAMED_FAILURE, "نازل"),
    )
    failed = contracted < FLOOR_CONTRACTED
    for label, group, ceiling, kind in checks:
        mark = "" if len(group) <= ceiling else "  ✗"
        print(f"  {label:<34} {len(group):>5} (السقف {ceiling} — {kind}){mark}")
        if len(group) > ceiling:
            failed = True

    for label, group, ceiling, _kind in checks:
        if len(group) <= ceiling:
            continue
        print(f"  ✗ {label}: {len(group)} > {ceiling}")
        by_top = collections.Counter(r["top"] for r in group)
        print(f"      بالشجرة: {dict(by_top.most_common(5))}")
        for row in group[:8]:
            print(f"      · {row['rel']}")
        if len(group) > 8:
            print(f"      … و{len(group) - 8} غيرُها")

    if failed:
        print("  🔑 بذرةٌ تُشغَّلُ ولا يُقابَلُ مخرَجُها بشيءٍ تمرُّ دائمًا — فهي"
              " سطرٌ في عدَّادٍ لا برهان.")
        return 1
    print("  ✓ لم ينمُ دَينُ العقد، ولا عقدَ متناقضًا.")
    return 0


if __name__ == "__main__":
    # (AR) 🔑 انهيارٌ ليس حكمًا: بذرةٌ ليست UTF-8 **لم تُقَسْ**، فرمزُها ٢ لا ١.
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001 — كلُّ انهيارٍ عطبُ آلةٍ لا حكم
        sys.stdout.reconfigure(encoding="utf-8")
        print(f"  ✗ عطبُ آلةٍ — لم يُقَسْ شيء: {exc.__class__.__name__}: {exc}")
        raise SystemExit(2)
