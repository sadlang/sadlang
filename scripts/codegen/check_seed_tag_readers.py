#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ **قرّاءِ وسومِ البذور** — العدّاءُ والحارسُ يقولانِ الشيءَ نفسَه.

     عقدُ البذرةِ يُكتَبُ وسمًا في تعليق (`@expected` · `@skip_compiler` ·
     `@expect_error` وأخواتُها · `@rule`)، ويقرؤه في هذه الشجرةِ **قرّاءٌ
     كثيرون مكتوبونَ باليد** في ملفّاتٍ شتّى — عدَدُهم في ② أدناه لا ههنا،
     فعددٌ منثورٌ نسخةٌ ثانيةٌ من حقيقةٍ تُقاس. وليس فيها مَن يقيسُ اتّفاقَهم،
     فينجرفُ أحدُهم عن الآخرِ صامتًا.

     🔑 **والانجرافُ ههنا ليس نظريًّا.** سجلُّ دروسِ هذا المستودعِ يحملُ منه
        صورًا مقيسةً: ثلاثةُ قرّاءٍ أنتجوا ثلاثةَ أرقامٍ متناقضةٍ لعدَّادٍ واحد
        (١٦٤ · ١٦٥ · ١٦٩)؛ ونسخةٌ بـ`\\s` جعلت الحارسَ يقولُ ٧٧ والمقياسَ ٧٨؛
        و`utf-8` مقابلَ `utf-8-sig` جعلَ الحارسَ يرى عقدًا والعدّاءَ يتخطّى؛
        ونافذةُ ثلاثينَ سطرًا لم تكنْ عندَ الحارس. وكلُّها اكتُشِفَت **بعدَ**
        الإيداعِ لا قبلَه.

     وثلاثةُ لامتغيِّرات:

       ① **خلافُ الحدّ**: يُستوردُ `tests/runner.py` — **مُنفِّذُ العقد** — ويُسألُ
          عن كلِّ بذرةٍ في الشجرة، ويُسألُ الحارسُ عنها، ويُقارَنُ الجوابان في
          ثلاثةِ أسئلةٍ: أمتخطّاةٌ هي؟ أذاتُ عقدٍ موجَب؟ أذاتُ عقدٍ سالب؟
          **سقفُه صفرٌ** — لا هامشَ لخلافٍ بين مَن يحرسُ ومَن يُنفِّذ.
          ولا يُغيَّرُ العدّاءُ ولا يُربَطُ بشيء: الحارسُ يستوردُه، لا العكس.

       ② **قرّاءٌ مكتوبونَ باليد**: كلُّ نمطٍ مُصرَّفٍ في الشجرةِ يذكرُ وسمَ
          بذرة. سقفٌ **نازل**: يُهبَطُ به كلّما وُرِّثَ قارئٌ، ولا يُرفَع.
          (وليس صفرًا لأنّ الصفرَ اليومَ كذبٌ يُسكَّنُ: العدّاءُ مكتبةٌ قياسيّةٌ
          خالصةٌ يُستدعى اثنتَي عشرةَ مرّةً في CI، وربطُه بـ`scripts/codegen`
          مخاطرةٌ لا سدّ — فالعددُ يُسمّى ويُخفَّضُ بالعمل.)

       ③ **خلافُ قرّاءِ `@rule`**: لوسمٍ واحدٍ قرّاءُ عدّة. يُطبَّقُ كلُّ
          **هجاءٍ متمايزٍ** منهم على المادّةِ نفسِها ويُقارَنُ ما استخرجَه.
          **سقفُه صفر.** (وهجاءٌ واحدٌ اليومَ — والصفُّ في المخرَجِ يقولُه.)

     🔑 **والمقيسُ حاصلُ القراءةِ لا هجاؤها.** ثلاثُ قواعدَ متتاليةٍ في هذا
        المستودعِ حرسَت *صورةَ كتابةِ* القارئِ الثاني، والتُفَّ على ثلاثتِها
        بصورةٍ أخرى ثمّ باسمٍ آخرَ ثمّ بدالّةٍ أخرى من `re`. فما يُقاسُ ههنا
        هو **الجوابُ** الذي يُعطيه كلُّ قارئٍ على بذرةٍ بعينِها.

     ⚠️ **ورمزُ الخروج**: ٢ عطبُ آلةٍ (لم يُقَسْ شيء) · ١ حكمٌ · ٠ أخضر.

(EN) Guard for seed-tag readers: the runner (the contract's executor) and the
     guards must give the same answer on every seed. Imports tests/runner.py
     read-only and compares. Ceiling 0 on disagreement, descending ceiling on
     the number of hand-written readers, ceiling 0 on @rule reader divergence.
============================================================================
"""
from __future__ import annotations

import ast
import io
import re
import sys
import warnings
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(ROOT / "tests"))

# ═══ الحدودُ المُعلَنة ═══════════════════════════════════════════════════════
# (AR) ① و③ صفرانِ: خلافٌ بين قارئَي وسمٍ واحدٍ عطبٌ لا دَين.
CEILING_BOUNDARY = 0
CEILING_RULE_SPLIT = 0
# (AR) ② سقفٌ **نازلٌ لا يُرفَع** — يُهبَطُ به كلّما وُرِّثَ قارئ.
CEILING_HANDWRITTEN = 28

# (AR) 🔑 مفرداتُ الوسمِ **تُشتقُّ** من أنماطِ العدّاءِ والحارسَينِ لا تُهجّى
#      ههنا: هجاءٌ رابعٌ ليدٍ رابعةٍ هو العطبُ الذي يحرسُه هذا الملفّ.
LEXICON_SOURCES = (
    ROOT / "tests" / "runner.py",
    ROOT / "scripts" / "codegen" / "check_seed_contract.py",
    ROOT / "scripts" / "codegen" / "check_anchor_integrity.py",
)
_TAG = re.compile(r"@[a-z_؀-ۿ]+")

SKIP_PARTS = ("_archive", "build", ".venv", "__pycache__", "node_modules",
              ".git", "dist", "out")


def _pattern_literals(node) -> list[str]:
    """(AR) سلاسلُ النمطِ في نداءِ `re.*` — ولو رُكِّبَ بالجمعِ أو سُمّيَ وسيطُه."""
    if not isinstance(node, ast.Call):
        return []
    func = node.func
    if not isinstance(func, ast.Attribute) or func.attr not in (
            "compile", "search", "match", "fullmatch", "findall", "finditer",
            "sub", "subn", "split"):
        return []
    source = node.args[0] if node.args else None
    for keyword in node.keywords:
        if keyword.arg == "pattern":
            source = keyword.value
    if source is None:
        return []
    return [child.value for child in ast.walk(source)
            if isinstance(child, ast.Constant) and isinstance(child.value, str)]


def _lexicon() -> tuple[str, ...]:
    """(AR) الوسومُ المعروفةُ — من أنماطِ مَن يقرؤها، لا من صفٍّ يدويّ."""
    tags: set[str] = set()
    for path in LEXICON_SOURCES:
        if not path.is_file():
            raise AssertionError("مصدرُ معجمٍ مفقود: %s" % path)
        tree = ast.parse(path.read_text(encoding="utf-8"))
        for node in ast.walk(tree):
            for text in _pattern_literals(node):
                tags.update(_TAG.findall(text))
        for node in tree.body:
            if isinstance(node, (ast.Assign, ast.AnnAssign)):
                value = node.value
                if isinstance(value, ast.Constant) and isinstance(value.value, str):
                    tags.update(_TAG.findall(value.value))
    if len(tags) < 5:
        raise AssertionError("معجمٌ أصغرُ من أن يكونَ صحيحًا: %r" % sorted(tags))
    return tuple(sorted(tags))


def _handwritten(lexicon) -> list[tuple[str, int, str]]:
    """(AR) كلُّ نمطٍ مُصرَّفٍ في الشجرةِ يذكرُ وسمَ بذرة."""
    # (AR) وتحذيراتُ الهروبِ من ملفّاتٍ لا نملكُها تُكتَمُ: مخرَجُ حارسٍ يُقرأُ
    #      في CI لا يُلوَّثُ بضجيجٍ عن ملفٍّ لا يقيسُه.
    warnings.simplefilter("ignore", SyntaxWarning)
    found: list[tuple[str, int, str]] = []
    for path in sorted(ROOT.rglob("*.py")):
        if any(part in SKIP_PARTS for part in path.parts):
            continue
        try:
            tree = ast.parse(path.read_text(encoding="utf-8"))
        except (SyntaxError, UnicodeDecodeError, ValueError):
            # (AR) قوالبُ توليدٍ وملفّاتٌ غيرُ صالحةٍ نحويًّا — تُتخطّى ولا
            #      تُوقِفُ القياس، فليست قرّاءَ يُنفَّذون.
            continue
        for node in ast.walk(tree):
            for text in _pattern_literals(node):
                hit = [tag for tag in lexicon if tag in text]
                if hit:
                    found.append((path.relative_to(ROOT).as_posix(),
                                  node.lineno, "·".join(hit)))
                    break
    return found


def _rule_readers(handwritten) -> list[str]:
    """(AR) أنماطُ `@rule` الحرفيّةُ — كلُّ نسخةٍ على حدة."""
    patterns: set[str] = set()
    for rel, _line, _tags in handwritten:
        path = ROOT / rel
        try:
            tree = ast.parse(path.read_text(encoding="utf-8"))
        except (SyntaxError, UnicodeDecodeError, ValueError):
            continue
        for node in ast.walk(tree):
            for text in _pattern_literals(node):
                if "@rule" in text:
                    patterns.add(text)
    return sorted(patterns)


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    print("حارس «قارئا العقدِ يقولانِ الشيءَ نفسَه»:")

    try:
        import check_seed_contract as guard
        import runner as executor
    except Exception as exc:  # noqa: BLE001 — تعذُّرُ الاستيرادِ عطبُ آلة
        print("  ✗ عطبُ آلة: تعذَّرَ استيرادُ طرفَي القياس: %s" % exc)
        return 2

    try:
        lexicon = _lexicon()
        handwritten = _handwritten(lexicon)
        rows = guard._seeds()
    except AssertionError as exc:
        print("  ✗ عطبُ آلة: %s" % exc)
        return 2
    if not rows:
        print("  ✗ عطبُ آلة: لا بذرةَ مُسِحَت — لم يُقَسْ شيء")
        return 2

    print("  معجمُ الوسومِ المُشتقّ: %d (%s)"
          % (len(lexicon), " · ".join(lexicon)))

    # ── ① خلافُ الحدّ ────────────────────────────────────────────────────
    boundary: list[str] = []
    measured = 0
    for row in rows:
        path = ROOT / row["rel"]
        if not path.is_file():
            continue
        measured += 1
        meta = executor.parse_metadata(path)
        # (AR) والسالبُ **ثلاثةُ حقولٍ** عندَ العدّاءِ لا حقلٌ واحد — وقياسُه
        #      بحقلٍ واحدٍ يُلفِّقُ عشرَ حالاتِ خلافٍ لا وجودَ لها. (قِيسَ.)
        negative = bool(meta.expect_error or meta.expect_compile_error
                        or meta.expect_error_compiled)
        for label, said_guard, said_runner in (
                ("متخطّاة", row["skip"], bool(meta.skip_compiler)),
                ("عقدٌ موجَب", row["expected"], bool(meta.expected_output)),
                ("عقدٌ سالب", row["negative"], negative)):
            if said_guard != said_runner:
                boundary.append("%s — %s: الحارس=%s · العدّاء=%s"
                                % (row["rel"], label, said_guard, said_runner))
    if not measured:
        print("  ✗ عطبُ آلة: صفرُ بذرةٍ قِيسَت على الطرفَين — لم يُقَسْ شيء")
        return 2

    # ── ③ خلافُ قرّاءِ `@rule` ───────────────────────────────────────────
    rule_patterns = _rule_readers(handwritten)
    rule_split: list[str] = []
    if len(rule_patterns) > 1:
        answers = []
        for pattern in rule_patterns:
            compiled = re.compile(pattern, re.M)
            got = {}
            for row in rows:
                path = ROOT / row["rel"]
                if not path.is_file():
                    continue
                text = io.open(path, encoding="utf-8-sig").read()
                text = "\n".join(text.split("\n")[:guard._runner_window()])
                found = compiled.search(text)
                if found:
                    got[row["rel"]] = (found.group(1).strip()
                                       if found.groups() else "")
            answers.append((pattern, got))
        first_pattern, first = answers[0]
        for pattern, got in answers[1:]:
            keys = set(got) ^ set(first)
            for key in sorted(keys)[:5]:
                rule_split.append("%s ⇐ %r مقابل %r" % (key, pattern,
                                                        first_pattern))
            for key in sorted(set(got) & set(first)):
                if got[key] != first[key]:
                    rule_split.append("%s: %r مقابل %r"
                                      % (key, got[key], first[key]))

    print("─" * 74)
    rows_out = (
        ("① خلافُ الحدّ (العدّاء ↔ الحارس)", len(boundary), CEILING_BOUNDARY,
         "لا سقفَ له"),
        ("② قرّاءٌ مكتوبونَ باليد", len(handwritten), CEILING_HANDWRITTEN,
         "نازل"),
        ("③ خلافُ قرّاءِ @rule", len(rule_split), CEILING_RULE_SPLIT,
         "لا سقفَ له"),
    )
    failures = []
    for title, value, ceiling, kind in rows_out:
        print("  %-38s %5d (السقف %d — %s)" % (title, value, ceiling, kind))
        if value > ceiling:
            failures.append((title, value, ceiling))
    print("  بذورٌ قِيسَت على الطرفَين: %d · قرّاءُ @rule: %d"
          % (measured, len(rule_patterns)))

    if failures:
        print("─" * 74)
        for title, value, ceiling in failures:
            print("  ✗ %s: %d > %d" % (title, value, ceiling))
        for line in boundary[:10]:
            print("      · %s" % line)
        for line in rule_split[:10]:
            print("      · %s" % line)
        if len(handwritten) > CEILING_HANDWRITTEN:
            for rel, line, tags in handwritten[:10]:
                print("      · %s:%d %s" % (rel, line, tags))
        print("  ⤷ قارئانِ لوسمٍ واحدٍ يختلفانِ = عدَّادٌ واحدٌ برقمَين.")
        return 1

    print("  ✓ العدّاءُ والحارسُ متّفقانِ على كلِّ بذرة، ولم ينمُ عددُ القرّاء.")
    return 0


if __name__ == "__main__":
    # (AR) 🔑 انهيارٌ ليس حكمًا: رمزُه ٢ لا ١.
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001
        sys.stdout.reconfigure(encoding="utf-8")
        print("✗ عطبُ آلةٍ — لم يُقَسْ شيء: %s: %s" % (exc.__class__.__name__, exc))
        raise SystemExit(2)
