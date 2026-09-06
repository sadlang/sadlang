#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ **قرّاءِ وسومِ البذور** — العدّاءُ والحارسُ يقولانِ الشيءَ نفسَه.

     عقدُ البذرةِ يُكتَبُ وسمًا في تعليق (`@expected` · `@skip_compiler` ·
     `@expect_error` وأخواتُها · `@rule`)، ويقرؤه في هذه الشجرةِ **أنماطٌ
     مُصرَّفةٌ كثيرةٌ مكتوبةٌ باليد** في ملفّاتٍ شتّى — عدَدُها في ② أدناه لا
     ههنا، فعددٌ منثورٌ نسخةٌ ثانيةٌ من حقيقةٍ تُقاس. وليس فيها مَن يقيسُ
     اتّفاقَهم، فينجرفُ أحدُهم عن الآخرِ صامتًا.

     🔑 **والانجرافُ ههنا ليس نظريًّا.** سجلُّ دروسِ هذا المستودعِ يحملُ منه
        صورًا مقيسةً: ثلاثةُ قرّاءٍ أنتجوا ثلاثةَ أرقامٍ متناقضةٍ لعدَّادٍ واحد
        (١٦٤ · ١٦٥ · ١٦٩)؛ ونسخةٌ بـ`\\s` جعلت الحارسَ يقولُ ٧٧ والمقياسَ ٧٨؛
        و`utf-8` مقابلَ `utf-8-sig` جعلَ الحارسَ يرى عقدًا والعدّاءَ يتخطّى؛
        ونافذةُ ثلاثينَ سطرًا لم تكنْ عندَ الحارس. وكلُّها اكتُشِفَت **بعدَ**
        الإيداعِ لا قبلَه.

     وأربعةُ لامتغيِّرات:

       ① **خلافُ الحدّ**: يُستوردُ `tests/runner.py` — **مُنفِّذُ العقد** — ويُسألُ
          عن كلِّ بذرةٍ في الشجرة، ويُسألُ الحارسُ عنها، ويُقارَنُ الجوابان في
          ثلاثةِ أسئلةٍ: أمتخطّاةٌ هي؟ أذاتُ عقدٍ موجَب؟ أذاتُ عقدٍ سالب؟
          **سقفُه صفرٌ** — لا هامشَ لخلافٍ بين مَن يحرسُ ومَن يُنفِّذ.
          ولا يُغيَّرُ العدّاءُ ولا يُربَطُ بشيء: الحارسُ يستوردُه، لا العكس.

       ② **أنماطٌ مُصرَّفةٌ مكتوبةٌ باليد**: كلُّ نداءِ `re.*` بنمطٍ حرفيٍّ يذكرُ
          وسمَ بذرة. **سقفٌ نازلٌ وأرضيّةٌ مُعلَنة** — والحدُّ محدودٌ من طرفَيه:
          الانكماشُ **يُحمِّرُ** ولا يُقرأُ تقدّمًا. (وقِيسَ لِمَ: توسيعُ قائمةِ
          الاستثناءِ بكلمةٍ واحدةٍ أخفى أربعةَ أنماطٍ والبوّابةُ خضراء.)
          ⚠️ **والدعوى «أنماطٌ مُصرَّفة» لا «قرّاء»**: صورُ القراءةِ الأخرى
             (`startswith` · `in` · شظيّةٌ تُركَّبُ زمنَ التشغيل) تفلتُ منه —
             ويحرسُها ① بالحاصلِ لا بالهجاء.
          🔑 **وأرضيّتُه بنيويّة**: أكثرُ من نصفِ هذه الأنماطِ داخلَ العدّاءِ
             نفسِه، وقيدُ التصميمِ («يُستوردُ ولا يُغيَّر») يمنعُ توريثَها.
             فالنزولُ ممكنٌ إلى ما فوقَ ذلك لا إلى صفر، وقولُ غيرِ ذلك دَينٌ
             غيرُ قابلٍ للسداد.

       ③ **خلافُ قرّاءِ `@rule`**: كلُّ قارئٍ يُطبَّقُ **برايتِه وشكلِه** — لا
          بشكلٍ مُوحَّدٍ يُطبِّعُ الفروقَ — ويُقارَنُ ما استخرجَه بمرجعٍ واحد.
          **سقفُه صفر.** وكان يُصرَّفُ الجميعُ بـ`re.M` مهما كانت راياتُهم،
          وكان المفتاحُ **الهجاءَ** وحدَه فلا يُقارَنُ إلّا هجاءانِ مختلفان —
          وقِيسَ: نزعُ `re.M` من حارسِ CI حيٍّ لم يُحرِّكْ ساكنًا.

       ④ **ملفّاتٌ لا تُحلَّلُ نحويًّا**: تُتخطّى في المسحِ — فتُعَدُّ ويُعلَنُ
          عددُها بسقفٍ نازل. تخطٍّ صامتٌ يُنقِصُ ② بلا إشارة.

     🔑 **والمقيسُ حاصلُ القراءةِ لا هجاؤها** حيثُ أمكن. ثلاثُ قواعدَ متتاليةٍ
        في هذا المستودعِ حرسَت *صورةَ كتابةِ* القارئِ الثاني، والتُفَّ على
        ثلاثتِها بصورةٍ أخرى ثمّ باسمٍ آخرَ ثمّ بدالّةٍ أخرى من `re`.

     ⚠️ **ورمزُ الخروج**: ٢ عطبُ آلةٍ (لم يُقَسْ شيء) · ١ حكمٌ · ٠ أخضر.

(EN) Guard for seed-tag readers: the runner (the contract's executor) and the
     guards must give the same answer on every seed. Imports tests/runner.py
     read-only. Ceiling 0 on disagreement, a two-sided bound on the number of
     hand-written compiled patterns, ceiling 0 on @rule reader divergence, and
     a descending ceiling on files skipped as unparseable.
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
# (AR) والثاني يُلحَقُ ولا يُقحَمُ في الصدر: `tests/` قد يحملُ اسمًا شائعًا،
#      وإقحامُه أوّلًا يُظلِّلُ للجلسةِ كلِّها لا لهذا الحارسِ وحدَه.
sys.path.append(str(ROOT / "tests"))

# ═══ الحدودُ المُعلَنة ═══════════════════════════════════════════════════════
CEILING_BOUNDARY = 0
CEILING_RULE_SPLIT = 0
CEILING_UNPARSED = 1
# (AR) 🔑 **الحدُّ محدودٌ من طرفَيه.** كان سقفًا وحدَه، فقِيسَ: توسيعُ قائمةِ
#      الاستثناءِ بكلمةٍ واحدةٍ أخفى أربعةَ أنماطٍ (٢٨ ← ٢٤) والحارسُ أخضر —
#      «انكماشُ المُعلَنِ يُقرأُ تقدّمًا». فالنزولُ يُعلَنُ ولا يُسرَق.
CEILING_HANDWRITTEN = 30
FLOOR_HANDWRITTEN = 30

# (AR) 🔑 مفرداتُ الوسمِ **تُشتقُّ** من أنماطِ العدّاءِ والحارسَينِ لا تُهجّى
#      ههنا: هجاءٌ رابعٌ ليدٍ رابعةٍ هو العطبُ الذي يحرسُه هذا الملفّ.
LEXICON_SOURCES = (
    ROOT / "tests" / "runner.py",
    ROOT / "scripts" / "codegen" / "check_seed_contract.py",
    ROOT / "scripts" / "codegen" / "check_anchor_integrity.py",
)
_TAG = re.compile(r"@[a-z_؀-ۿ]+")
# (AR) والوسمُ كلمةٌ تامّةٌ لا شظيّة: `len(tags) < 5` كان أرضيّةً لا تحرسُ —
#      قِيسَ أنّ تضييقَ `_TAG` يُنتِجُ معجمًا من شظايا (`@de` · `@e` · `@kno`)
#      و② تنكمشُ سبعةً والحارسُ أخضر.
_WHOLE_TAG = re.compile(r"@(?:[a-z_]{3,}|[؀-ۿ_]{3,})\Z")
FLOOR_LEXICON = 20

# (AR) 🔑 **استثناءُ الأرشيفِ يُورَثُ** كما في الشقيقتَين، ولا يُكتَبُ نسخةً
#      رابعة. وما دونَه (نواتجُ بناءٍ وبيئاتٌ) **ليس استثناءَ بذورٍ** فيُسمّى
#      باسمِه ولا يُخلَطُ به.
from check_seed_contract import SKIP_PARTS  # noqa: E402

BUILD_PARTS = ("build", ".venv", "__pycache__", "node_modules", ".git",
               "dist", "out")

RE_CALLS = ("compile", "search", "match", "fullmatch", "findall", "finditer",
            "sub", "subn", "split")


def _re_aliases(tree) -> set[str]:
    """(AR) أسماءُ وحدةِ `re` في هذا الملفِّ — و`from re import compile` كذلك."""
    names: set[str] = set()
    bare = False
    for node in ast.walk(tree):
        if isinstance(node, ast.Import):
            for alias in node.names:
                # (AR) و`regex` بديلٌ حرفيٌّ لـ`re` في بايثون — قارئٌ مكتوبٌ
                #      به قارئٌ كسائرِه، فلا يُستثنى بالاسم.
                if alias.name in ("re", "regex"):
                    names.add(alias.asname or alias.name)
        elif isinstance(node, ast.ImportFrom) and node.module == "re":
            for alias in node.names:
                if alias.name in RE_CALLS:
                    bare = True
    if bare:
        names.add("")          # نداءٌ مجرَّدٌ مستوردٌ من `re`
    return names


def _string_constants(tree) -> dict:
    """(AR) ثوابتُ الهجاءِ على مستوى الوحدةِ — يُتبَعُ إسنادُها إلى النمط."""
    out = {}
    for node in tree.body:
        if isinstance(node, (ast.Assign, ast.AnnAssign)):
            value = node.value
            if not (isinstance(value, ast.Constant) and isinstance(value.value, str)):
                continue
            targets = node.targets if isinstance(node, ast.Assign) else [node.target]
            for target in targets:
                if isinstance(target, ast.Name):
                    out[target.id] = value.value
    return out


def _pattern_literals(node, aliases=None, constants=None) -> list:
    """(AR) سلاسلُ النمطِ في نداءِ `re.*` — ولو رُكِّبَ بالجمعِ أو سُمّيَ وسيطُه
    أو أُسنِدَ إلى ثابتٍ على مستوى الوحدة.

    🔑 ويُشترَطُ أن يكونَ المُنادى **وحدةَ `re` بعينِها**: كان `func.attr`
       وحدَه، فـ`s.split("@expected")` — نصٌّ لا يقرأُ عقدًا — كان يُوقِفُ
       البوّابةَ (رفضٌ كاذبٌ مقيس)، و`compile()` المدمجةُ كذلك.
    """
    if not isinstance(node, ast.Call):
        return []
    func = node.func
    if isinstance(func, ast.Attribute):
        if func.attr not in RE_CALLS:
            return []
        owner = func.value
        if aliases is not None:
            if not (isinstance(owner, ast.Name) and owner.id in aliases):
                return []
    elif isinstance(func, ast.Name):
        if func.id not in RE_CALLS:
            return []
        if aliases is not None and "" not in aliases:
            return []
    else:
        return []
    source = node.args[0] if node.args else None
    for keyword in node.keywords:
        if keyword.arg == "pattern":
            source = keyword.value
    if source is None:
        return []
    found = [child.value for child in ast.walk(source)
             if isinstance(child, ast.Constant) and isinstance(child.value, str)]
    if constants:
        found += [constants[child.id] for child in ast.walk(source)
                  if isinstance(child, ast.Name) and child.id in constants]
    return found


def _multiline(node) -> bool:
    """(AR) أفي النداءِ رايةُ `re.M`؟ — الرايةُ من موضعِ النداءِ لا تُفترَض."""
    args = list(node.args[1:]) + [k.value for k in node.keywords if k.arg == "flags"]
    for arg in args:
        for child in ast.walk(arg):
            if isinstance(child, ast.Attribute) and child.attr in ("M", "MULTILINE"):
                return True
    return False


def _lexicon() -> tuple:
    """(AR) الوسومُ المعروفةُ — من أنماطِ مَن يقرؤها، لا من صفٍّ يدويّ."""
    tags = set()
    for path in LEXICON_SOURCES:
        if not path.is_file():
            raise AssertionError("مصدرُ معجمٍ مفقود: %s" % path)
        tree = ast.parse(path.read_text(encoding="utf-8"))
        aliases, constants = _re_aliases(tree), _string_constants(tree)
        for node in ast.walk(tree):
            for text in _pattern_literals(node, aliases, constants):
                tags.update(_TAG.findall(text))
        for value in constants.values():
            tags.update(_TAG.findall(value))
    # (AR) ⚠️ و`@expect` وسمٌ زائفٌ أنتجَه وقوفُ `_TAG` عندَ `(` في
    #      `@expect(?:_compile)?_error`. ويبقى: إسقاطُ كلِّ سابقةٍ صارمةٍ
    #      **أوسعُ من العطب** — قِيسَ أنّه يُسقِطُ معه `@expect_error` وهو وسمٌ
    #      حقيقيٌّ (سابقةُ `@expect_error_compiled`)، فينكمشُ المعجمُ ٢٠ ← ١٨.
    #      وأثرُ بقائِه توسيعُ المطابقةِ لا تضييقُها، فلا يُنقِصُ عدَّادًا.
    tags = {tag for tag in tags if _WHOLE_TAG.match(tag)}
    if len(tags) < FLOOR_LEXICON:
        raise AssertionError(
            "انكمشَ المعجمُ: %d < %d — قارئُ الوسمِ أعمى، فلا يُعلَنُ نجاح: %r"
            % (len(tags), FLOOR_LEXICON, sorted(tags)))
    return tuple(sorted(tags))


def _scan_files() -> list:
    return [path for path in sorted(ROOT.rglob("*.py"))
            if not any(part in SKIP_PARTS or part in BUILD_PARTS
                       for part in path.parts)]


def _handwritten(lexicon):
    """(AR) كلُّ نمطٍ مُصرَّفٍ يذكرُ وسمَ بذرة — والمتخطّى نحويًّا يُعَدُّ ويُسمّى."""
    with warnings.catch_warnings():
        # (AR) والكتمُ محصورٌ في هذا المسحِ ولا يُغيِّرُ حالةَ العمليّةِ كلِّها:
        #      `simplefilter` العارية تكتمُ تحذيرًا مشروعًا في اختبارٍ آخر.
        warnings.simplefilter("ignore", SyntaxWarning)
        found, unparsed, rules = [], [], []
        for path in _scan_files():
            rel = path.relative_to(ROOT).as_posix()
            try:
                tree = ast.parse(path.read_text(encoding="utf-8"))
            except (SyntaxError, UnicodeDecodeError, ValueError) as exc:
                unparsed.append("%s (%s)" % (rel, exc.__class__.__name__))
                continue
            aliases, constants = _re_aliases(tree), _string_constants(tree)
            for node in ast.walk(tree):
                texts = _pattern_literals(node, aliases, constants)
                if not texts:
                    continue
                joined = "".join(texts)
                hit = [tag for tag in lexicon if tag in joined]
                if hit:
                    found.append((rel, node.lineno, "·".join(hit)))
                    if "@rule" in joined:
                        rules.append((rel, node.lineno, joined, _multiline(node)))
    return found, unparsed, rules


# ═══ ③ كلُّ قارئٍ يُطبَّقُ برايتِه وشكلِه ═══════════════════════════════════
# (AR) 🔑 **مرجعُ المقارنةِ طرازُ العدّاء**: مطابقةٌ سطرًا سطرًا بعدَ `strip()`
#      داخلَ نافذةِ الوسوم — وهو ما يفعلُه `tests/runner.py` بالضبط. فيُقاسُ
#      كلُّ قارئٍ عليه ولو كان في الشجرةِ قارئٌ واحد، ولا يبقى اللامتغيِّرُ
#      رهنَ وجودِ هجاءَينِ مختلفَين (وكان كذلك: `len(patterns) > 1` كاذبٌ
#      دائمًا وهجاءُ الخمسةِ واحد ⇒ الكتلةُ ميّتةٌ والعيارُ يُباركُها ١٢/١٢).
def _rule_answer(pattern: str, multiline: bool, window_text: str):
    if multiline:
        found = re.compile(pattern, re.M).search(window_text)
    else:
        compiled = re.compile(pattern)
        found = None
        for line in window_text.split("\n"):
            found = compiled.match(line.strip())
            if found:
                break
    if not found:
        return None
    return found.group(1).strip() if found.groups() else ""


def _reference_rule_reader(rules):
    """(AR) المرجعُ: طرازُ العدّاءِ — مطابقةٌ سطرًا سطرًا بعدَ `strip()`."""
    for entry in rules:
        if not entry[3]:
            return entry
    return rules[0] if rules else None


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
        handwritten, unparsed, rules = _handwritten(lexicon)
        rows = guard._seeds()
        window = guard._runner_window()
    except AssertionError as exc:
        print("  ✗ عطبُ آلة: %s" % exc)
        return 2
    if not rows:
        print("  ✗ عطبُ آلة: لا بذرةَ مُسِحَت — لم يُقَسْ شيء")
        return 2

    print("  معجمُ الوسومِ المُشتقّ: %d (%s)" % (len(lexicon), " · ".join(lexicon)))

    # ── ① خلافُ الحدّ ────────────────────────────────────────────────────
    boundary = []
    measured = 0
    windows = {}
    for row in rows:
        path = ROOT / row["rel"]
        if not path.is_file():
            continue
        measured += 1
        text = io.open(path, encoding="utf-8-sig").read()
        windows[row["rel"]] = "\n".join(text.split("\n")[:window])
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
    rule_split = []
    reference = _reference_rule_reader(rules)
    if reference is None:
        print("  ✗ عطبُ آلة: لا قارئَ لـ@rule في الشجرة — لم يُقَسْ شيء")
        return 2
    ref_rel, ref_line, ref_pattern, ref_multi = reference
    ref_answer = {rel: _rule_answer(ref_pattern, ref_multi, text)
                  for rel, text in windows.items()}
    for rel, line, pattern, multiline in rules:
        if (rel, line) == (ref_rel, ref_line):
            continue
        for seed, text in windows.items():
            got = _rule_answer(pattern, multiline, text)
            if got != ref_answer[seed]:
                rule_split.append(
                    "%s:%d مقابل %s:%d — %s: %r مقابل %r"
                    % (rel, line, ref_rel, ref_line, seed, got, ref_answer[seed]))
                break

    print("─" * 74)
    rows_out = (
        ("① خلافُ الحدّ (العدّاء ↔ الحارس)", len(boundary), CEILING_BOUNDARY,
         "لا سقفَ له"),
        ("② أنماطٌ مُصرَّفةٌ مكتوبةٌ باليد", len(handwritten), CEILING_HANDWRITTEN,
         "نازل"),
        ("③ خلافُ قرّاءِ @rule", len(rule_split), CEILING_RULE_SPLIT,
         "لا سقفَ له"),
        ("④ ملفّاتٌ لا تُحلَّلُ نحويًّا", len(unparsed), CEILING_UNPARSED,
         "نازل"),
    )
    failures = []
    for title, value, ceiling, kind in rows_out:
        print("  %-38s %5d (السقف %d — %s)" % (title, value, ceiling, kind))
        if value > ceiling:
            failures.append(("✗ %s" % title, "%d > %d" % (value, ceiling)))
    shrunk = len(handwritten) < FLOOR_HANDWRITTEN
    if shrunk:
        failures.append(("✗ ② انكمشَ المُعلَن",
                         "%d < %d" % (len(handwritten), FLOOR_HANDWRITTEN)))
    print("  بذورٌ قِيسَت على الطرفَين: %d · قرّاءُ @rule: %d · المرجع: %s:%d"
          % (measured, len(rules), ref_rel, ref_line))

    if failures:
        print("─" * 74)
        for title, detail in failures:
            print("  %s: %s" % (title, detail))
        for line in boundary[:8]:
            print("      · %s" % line)
        for line in rule_split[:8]:
            print("      · %s" % line)
        for line in unparsed[:8]:
            print("      · %s" % line)
        if len(handwritten) > CEILING_HANDWRITTEN or shrunk:
            for rel, line, tags in handwritten[:8]:
                print("      · %s:%d %s" % (rel, line, tags))
        print("  ⤷ قارئانِ لوسمٍ واحدٍ يختلفانِ = عدَّادٌ واحدٌ برقمَين.")
        return 1

    print("  ✓ العدّاءُ والحارسُ متّفقانِ على كلِّ بذرة، وقرّاءُ @rule متّفقون،"
          " ولم ينمُ عددُ الأنماطِ ولم ينكمش.")
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
