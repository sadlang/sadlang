#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ **قرّاءِ وسومِ البذور** — العدّاءُ والحارسُ يقولانِ الشيءَ نفسَه.

     عقدُ البذرةِ يُكتَبُ وسمًا في تعليق (`@expected` · `@skip_compiler` ·
     `@expect_error` وأخواتُها · `@rule`)، ويقرؤه في هذه الشجرةِ **أنماطٌ
     مُصرَّفةٌ كثيرةٌ مكتوبةٌ باليد** في ملفّاتٍ شتّى — عدَدُها في ② أدناه لا
     ههنا. وليس فيها مَن يقيسُ اتّفاقَهم، فينجرفُ أحدُهم عن الآخرِ صامتًا.

     🔑 **والانجرافُ ههنا ليس نظريًّا.** ثلاثةُ قرّاءٍ أنتجوا ثلاثةَ أرقامٍ
        متناقضةٍ لعدَّادٍ واحد (١٦٤ · ١٦٥ · ١٦٩)؛ ونسخةٌ بـ`\\s` جعلت الحارسَ
        ٧٧ والمقياسَ ٧٨؛ ونافذةُ ثلاثينَ سطرًا لم تكنْ عندَ الحارس؛ **وأربعُ
        بذورٍ ذاتِ بادئةِ BOM كانت تُشخَّصُ «بلا وسم @rule» وهي تحملُه**.

     وخمسةُ لامتغيِّرات:

       ① **خلافُ الحدّ**: يُستوردُ `tests/runner.py` — **مُنفِّذُ العقد** — ويُسألُ
          عن كلِّ بذرةٍ في الشجرة، ويُسألُ الحارسُ عنها، ويُقارَنُ الجوابان في
          ثلاثةِ أسئلة. **سقفُه صفرٌ**. ولا يُغيَّرُ العدّاءُ: الحارسُ يستوردُه.
          ⚠️ وهو يقيسُ **طرفَي العقدِ المُسمَّيَينِ وحدَهما** — لا كلَّ قارئٍ
             في الشجرة. وصورُ القراءةِ غيرُ المُصرَّفةِ (`startswith` · `in`)
             خارجَ اللامتغيِّراتِ الخمسةِ كلِّها: **دَينٌ مُسمًّى**.

       ② **أنماطٌ مُصرَّفةٌ مكتوبةٌ باليد**: كلُّ نداءِ `re.*` بنمطٍ حرفيٍّ يذكرُ
          وسمَ بذرة. **سقفٌ نازلٌ وأرضيّةٌ مُعلَنة** — الانكماشُ يُحمِّرُ ولا
          يُقرأُ تقدّمًا. وأرضيّتُه بنيويّة: أكثرُ من نصفِها داخلَ العدّاءِ
          نفسِه، وقيدُ التصميمِ يمنعُ توريثَها.

       ③ **خلافُ قرّاءِ `@rule`**: كلُّ قارئٍ يُطبَّقُ **برايتِه وشكلِه** ويُقارَنُ
          بمرجعٍ على طرازِ العدّاء. **سقفُه صفر.**

       ④ **ملفّاتٌ لا تُحلَّلُ نحويًّا**: تُعَدُّ وتُسمّى — تخطٍّ صامتٌ يُنقِصُ ②.

       ⑤ **ترميزُ قارئِ الأسطر**: كلُّ `for … in …open(…)` على بذرةٍ يجبُ أن
          يفتحَ بـ`utf-8-sig`. **سقفُه صفر.** 🔑 وهذا هو **الوجهُ الثالثُ** من
          القارئِ (بعدَ النمطِ والنافذة) الذي يقولُ سجلُّ دروسِ هذا المستودعِ
          إنّ توريثَ واحدٍ منه يُخفي الانجرافَ في البقيّة — و③ وحدَه كان
          يعمى عنه: يقرأُ البذرةَ مرّةً بترميزٍ واحدٍ ثمّ يُعيدُ القرّاءَ على
          النصِّ المُطبَّعِ نفسِه، **فيُسوّي الفرقَ ثمّ يُعلِنُ الاتّفاق**.

     ⚠️ **ورمزُ الخروج**: ٢ عطبُ آلةٍ (لم يُقَسْ شيء) · ١ حكمٌ · ٠ أخضر.
        والانكماشُ حكمٌ لا عطبُ آلة: هو نتيجةُ عملٍ قد يكونُ مشروعًا، فيُقالُ
        له ماذا يفعلُ ولا يُتَّهَمُ بالعمى.

(EN) Guard for seed-tag readers. Five invariants: runner/guard boundary,
     hand-written compiled patterns (two-sided bound), @rule reader divergence
     measured with each reader's own flags and shape, unparseable files, and
     the encoding of seed line-readers (utf-8-sig, because a BOM is not space).
============================================================================
"""
from __future__ import annotations

import ast
import importlib.util
import io
import re
import sys
import warnings
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

# ═══ الحدودُ المُعلَنة ═══════════════════════════════════════════════════════
CEILING_BOUNDARY = 0
CEILING_RULE_SPLIT = 0
CEILING_UNPARSED = 1
CEILING_ENCODING = 0
# (AR) 🔑 **الحدُّ محدودٌ من طرفَيه.** قِيسَ: توسيعُ قائمةِ الاستثناءِ بكلمةٍ
#      واحدةٍ أخفى أربعةَ أنماطٍ والحارسُ أخضر — «انكماشُ المُعلَنِ يُقرأُ تقدّمًا».
CEILING_HANDWRITTEN = 30
FLOOR_HANDWRITTEN = 30

LEXICON_SOURCES = (
    ROOT / "tests" / "runner.py",
    ROOT / "scripts" / "codegen" / "check_seed_contract.py",
    ROOT / "scripts" / "codegen" / "check_anchor_integrity.py",
)
RUNNER = ROOT / "tests" / "runner.py"
_TAG = re.compile(r"@[a-z_؀-ۿ]+")
_WHOLE_TAG = re.compile(r"@(?:[a-z_]{3,}|[؀-ۿ_]{3,})\Z")
FLOOR_LEXICON = 20
SEED_ENCODING = "utf-8-sig"

# (AR) 🔑 **استثناءُ الأرشيفِ يُورَثُ** كما في الشقيقتَين، ولا يُكتَبُ نسخةً رابعة.
from check_seed_contract import SKIP_PARTS  # noqa: E402

RE_CALLS = ("compile", "search", "match", "fullmatch", "findall", "finditer",
            "sub", "subn", "split")
RE_MODULES = ("re", "regex")
FLAG_BITS = {"M": re.M, "MULTILINE": re.M, "I": re.I, "IGNORECASE": re.I,
             "S": re.S, "DOTALL": re.S, "X": re.X, "VERBOSE": re.X,
             "A": re.A, "ASCII": re.A}
ALWAYS_SKIP = (".git", "__pycache__", ".venv", "node_modules")
FLOOR_IGNORED_DIRS = 8


def _ignored_parts():
    """(AR) 🔑 **مجلَّداتُ النواتجِ تُشتقُّ من `.gitignore`** لا تُهجّى صفًّا رابعًا.
    وقِيسَ لِمَ: صفٌّ يدويٌّ فيه `build` وحدَه تركَ ٣٦ ملفًّا (١٦٪ من المسح) من
    `build-tests/_deps/` داخلَ القياس — فملفٌّ في ناتجِ بناءٍ **غيرِ مُتتبَّع**
    كان يُحمِّرُ البوّابةَ، والعددُ المُعلَنُ عددَ آلةٍ لا عددَ مستودع."""
    path = ROOT / ".gitignore"
    if not path.is_file():
        raise AssertionError(".gitignore مفقود — لا يُشتقُّ نطاقُ المسح")
    exact, prefixes = set(ALWAYS_SKIP), set()
    for raw in path.read_text(encoding="utf-8", errors="replace").split("\n"):
        line = raw.strip()
        if not line or line.startswith(("#", "!")) or not line.endswith("/"):
            continue                      # مجلَّداتٌ فقط
        name = line.strip("/").split("/")[-1]
        if any(ch in name for ch in "*?["):
            head = re.split(r"[*?\[]", name)[0]
            if head:
                prefixes.add(head)
        elif name:
            exact.add(name)
    if len(exact) < FLOOR_IGNORED_DIRS:
        raise AssertionError(
            "نطاقُ التخطّي أصغرُ من أن يكونَ صحيحًا: %d < %d — %r"
            % (len(exact), FLOOR_IGNORED_DIRS, sorted(exact)))
    return frozenset(exact), tuple(sorted(prefixes))


IGNORED_EXACT, IGNORED_PREFIX = _ignored_parts()


def _skipped(part: str) -> bool:
    return (part in SKIP_PARTS or part in IGNORED_EXACT
            or part.startswith(IGNORED_PREFIX))


def _re_names(tree):
    """(AR) أسماءُ وحدةِ `re` وأسماءُ دوالِّها المستوردةِ مجرَّدةً.

    🔑 والاسمُ المجرَّدُ يُربَطُ **بالدالّةِ بعينِها**: رايةٌ عامّةٌ («استُوردَ
       شيءٌ من `re`») تجعلُ `compile()` المدمجةَ تُعَدُّ نمطًا (رفضٌ كاذب)
       وتُفوِّتُ `from re import compile as c` (قارئٌ حقيقيّ).
    """
    modules, bare = set(), {}
    for node in ast.walk(tree):
        if isinstance(node, ast.Import):
            for alias in node.names:
                if alias.name in RE_MODULES:
                    modules.add(alias.asname or alias.name)
        elif isinstance(node, ast.ImportFrom) and node.module in RE_MODULES:
            for alias in node.names:
                if alias.name in RE_CALLS:
                    bare[alias.asname or alias.name] = alias.name
    return modules, bare


def _string_constants(tree):
    """(AR) ثوابتُ الهجاءِ — في الوحدةِ كلِّها لا في جسمِها الأعلى وحدَه."""
    out = {}
    for node in ast.walk(tree):
        if not isinstance(node, (ast.Assign, ast.AnnAssign)):
            continue
        value = node.value
        if not (isinstance(value, ast.Constant) and isinstance(value.value, str)):
            continue
        targets = node.targets if isinstance(node, ast.Assign) else [node.target]
        for target in targets:
            if isinstance(target, ast.Name):
                out[target.id] = value.value
    return out


def _re_call(node, names):
    """(AR) أهو نداءُ `re.*`؟ يردُّ اسمَ الدالّةِ أو None."""
    if not isinstance(node, ast.Call):
        return None
    modules, bare = names
    func = node.func
    if isinstance(func, ast.Attribute):
        owner = func.value
        if func.attr in RE_CALLS and isinstance(owner, ast.Name) \
                and owner.id in modules:
            return func.attr
        return None
    if isinstance(func, ast.Name) and func.id in bare:
        return bare[func.id]
    return None


def _pattern_literals(node, names, constants) -> list:
    """(AR) سلاسلُ النمطِ في نداءِ `re.*` — ولو رُكِّبَ بالجمعِ أو سُمّيَ وسيطُه
    أو أُسنِدَ إلى ثابت."""
    if _re_call(node, names) is None:
        return []
    source = node.args[0] if node.args else None
    for keyword in node.keywords:
        if keyword.arg == "pattern":
            source = keyword.value
    if source is None:
        return []
    found = [child.value for child in ast.walk(source)
             if isinstance(child, ast.Constant) and isinstance(child.value, str)]
    found += [constants[child.id] for child in ast.walk(source)
              if isinstance(child, ast.Name) and child.id in constants]
    return found


def _flags(node, constants) -> int:
    """(AR) راياتُ النداءِ **كلُّها** لا `re.M` وحدَها: `re.X` تُبدِّلُ دلالةَ
    النمطِ تبديلًا تامًّا، وتصريفُه بدونِها يُلفِّقُ خلافًا على قارئٍ سليم."""
    value = 0
    candidates = list(node.args[1:]) + [k.value for k in node.keywords
                                        if k.arg == "flags"]
    for arg in candidates:
        for child in ast.walk(arg):
            if isinstance(child, ast.Attribute) and child.attr in FLAG_BITS:
                value |= FLAG_BITS[child.attr]
            elif isinstance(child, ast.Name) and child.id in FLAG_BITS:
                value |= FLAG_BITS[child.id]
    return value


def _lexicon():
    """(AR) الوسومُ المعروفةُ — من أنماطِ مَن يقرؤها، لا من صفٍّ يدويّ."""
    tags = set()
    for path in LEXICON_SOURCES:
        if not path.is_file():
            raise AssertionError("مصدرُ معجمٍ مفقود: %s" % path)
        tree = ast.parse(path.read_text(encoding="utf-8"))
        names, constants = _re_names(tree), _string_constants(tree)
        for node in ast.walk(tree):
            for text in _pattern_literals(node, names, constants):
                tags.update(_TAG.findall(text))
        for value in constants.values():
            tags.update(_TAG.findall(value))
    # (AR) ⚠️ و`@expect` وسمٌ زائفٌ أنتجَه وقوفُ `_TAG` عندَ `(`. ويبقى: إسقاطُ
    #      كلِّ سابقةٍ صارمةٍ **أوسعُ من العطب** — يُسقِطُ معه `@expect_error`
    #      وهو وسمٌ حقيقيّ. وأثرُ بقائِه توسيعُ المطابقةِ لا تضييقُها.
    return tuple(sorted(tag for tag in tags if _WHOLE_TAG.match(tag)))


def _scan_files() -> list:
    return [path for path in sorted(ROOT.rglob("*.py"))
            if not any(_skipped(part) for part in path.parts)]


def _seed_line_readers(tree, path_rel):
    """(AR) ⑤ كلُّ `for … in …open(…)` — قارئُ أسطرٍ، وترميزُه يُقرأُ لا يُفترَض."""
    found = []
    for node in ast.walk(tree):
        if not isinstance(node, (ast.For, ast.AsyncFor)):
            continue
        for child in ast.walk(node.iter):
            name = None
            if isinstance(child, ast.Call):
                func = child.func
                name = (func.attr if isinstance(func, ast.Attribute)
                        else getattr(func, "id", None))
            if name != "open":
                continue
            encoding = "utf-8"
            for keyword in child.keywords:
                if keyword.arg == "encoding" and isinstance(keyword.value, ast.Constant):
                    encoding = keyword.value.value
            found.append((path_rel, child.lineno, encoding))
    return found


def _handwritten(lexicon):
    """(AR) الأنماطُ المُصرَّفةُ · المتخطّى نحويًّا · قرّاءُ `@rule` · قرّاءُ الأسطر."""
    with warnings.catch_warnings():
        # (AR) والكتمُ محصورٌ في هذا المسحِ ولا يُغيِّرُ حالةَ العمليّةِ كلِّها.
        warnings.simplefilter("ignore", SyntaxWarning)
        found, unparsed, rules, readers = [], [], [], []
        for path in _scan_files():
            rel = path.relative_to(ROOT).as_posix()
            try:
                tree = ast.parse(path.read_text(encoding="utf-8"))
            except (SyntaxError, UnicodeDecodeError, ValueError) as exc:
                unparsed.append("%s (%s)" % (rel, exc.__class__.__name__))
                continue
            names, constants = _re_names(tree), _string_constants(tree)
            readers += _seed_line_readers(tree, rel)
            for node in ast.walk(tree):
                texts = _pattern_literals(node, names, constants)
                if not texts:
                    continue
                joined = "".join(texts)
                hit = [tag for tag in lexicon if tag in joined]
                if hit:
                    found.append((rel, node.lineno, "·".join(hit)))
                    if "@rule" in joined:
                        rules.append((rel, node.lineno, joined,
                                      _flags(node, constants)))
    return found, unparsed, rules, readers


def _rule_answer(pattern: str, flags: int, window_text: str):
    """(AR) القارئُ يُطبَّقُ برايتِه وشكلِه: بـ`re.M` بحثٌ على النصّ، وبدونِها
    مطابقةٌ سطرًا سطرًا بعدَ `strip()` — وهو ما تفعلُه الثلاثةُ في الشجرة."""
    if flags & re.M:
        found = re.compile(pattern, flags).search(window_text)
    else:
        compiled = re.compile(pattern, flags)
        found = None
        for line in window_text.split("\n"):
            found = compiled.match(line.strip())
            if found:
                break
    if not found:
        return None
    return found.group(1).strip() if found.groups() else ""


def _load_runner():
    """(AR) 🔑 العدّاءُ يُحمَّلُ **بمسارِه** لا باسمِه: في الشجرةِ `runner.py`
    ثانٍ (`tests/system/docs/doc_gen/`)، وأيُّ `runner` أسبقُ في `sys.path`
    يُظلِّلُ المقصودَ فيُقاسُ غيرُه أو يُنهارُ. (قِيسَ بدليلٍ مُصطنَع.)"""
    if not RUNNER.is_file():
        raise AssertionError("العدّاءُ مفقود: %s" % RUNNER)
    spec = importlib.util.spec_from_file_location("sad_seed_runner", RUNNER)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    print("حارس «قارئا العقدِ يقولانِ الشيءَ نفسَه»:")

    try:
        import check_seed_contract as guard
        executor = _load_runner()
    except Exception as exc:  # noqa: BLE001 — تعذُّرُ التحميلِ عطبُ آلة
        print("  ✗ عطبُ آلة: تعذَّرَ تحميلُ طرفَي القياس: %s: %s"
              % (exc.__class__.__name__, exc))
        return 2

    try:
        lexicon = _lexicon()
        handwritten, unparsed, rules, readers = _handwritten(lexicon)
        rows = guard._seeds()
        window = guard._runner_window()
    except AssertionError as exc:
        print("  ✗ عطبُ آلة: %s" % exc)
        return 2
    if not rows:
        print("  ✗ عطبُ آلة: لا بذرةَ مُسِحَت — لم يُقَسْ شيء")
        return 2
    if not rules:
        print("  ✗ عطبُ آلة: لا قارئَ لـ@rule في الشجرة — لم يُقَسْ شيء")
        return 2

    print("  معجمُ الوسومِ المُشتقّ: %d (%s)" % (len(lexicon), " · ".join(lexicon)))

    # ── ① خلافُ الحدّ ────────────────────────────────────────────────────
    boundary, measured, windows = [], 0, {}
    for row in rows:
        path = ROOT / row["rel"]
        if not path.is_file():
            continue
        measured += 1
        text = io.open(path, encoding=SEED_ENCODING).read()
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

    # ── ⑤ ترميزُ قارئِ الأسطر ────────────────────────────────────────────
    encoding_debt = ["%s:%d ⇐ %s" % (rel, line, enc)
                     for rel, line, enc in readers if enc != SEED_ENCODING]

    # ── ③ خلافُ قرّاءِ `@rule` ───────────────────────────────────────────
    reference = next((one for one in rules if not one[3] & re.M), None)
    if reference is None:
        # (AR) والمرجعُ **طرازُ العدّاء**؛ فغيابُ قارئٍ بلا `re.M` يعني أنّ
        #      المرجعَ صارَ من طرازٍ آخرَ صامتًا — لا يُقاسُ على مجهول.
        print("  ✗ عطبُ آلة: لا قارئَ لـ@rule على طرازِ العدّاءِ (بلا re.M)"
              " — لا مرجعَ للمقارنة")
        return 2
    ref_rel, ref_line, ref_pattern, ref_flags = reference
    ref_answer = {rel: _rule_answer(ref_pattern, ref_flags, text)
                  for rel, text in windows.items()}
    rule_split = []
    for rel, line, pattern, flags in rules:
        if (rel, line) == (ref_rel, ref_line):
            continue
        for seed, text in windows.items():
            got = _rule_answer(pattern, flags, text)
            if got != ref_answer[seed]:
                rule_split.append("%s:%d مقابل %s:%d — %s: %r مقابل %r"
                                  % (rel, line, ref_rel, ref_line, seed,
                                     got, ref_answer[seed]))
                break

    print("─" * 74)
    shrunk = len(handwritten) < FLOOR_HANDWRITTEN
    lexicon_short = len(lexicon) < FLOOR_LEXICON
    table = (
        ("①", "خلافُ الحدّ (العدّاء ↔ الحارس)", len(boundary),
         "(السقف %d — لا سقفَ له)" % CEILING_BOUNDARY,
         len(boundary) > CEILING_BOUNDARY, boundary),
        ("②", "أنماطٌ مُصرَّفةٌ مكتوبةٌ باليد", len(handwritten),
         "(السقف %d · الأرضيّة %d — نازل)" % (CEILING_HANDWRITTEN,
                                              FLOOR_HANDWRITTEN),
         len(handwritten) > CEILING_HANDWRITTEN or shrunk,
         ["%s:%d %s" % row for row in handwritten]),
        ("③", "خلافُ قرّاءِ @rule", len(rule_split),
         "(السقف %d — لا سقفَ له)" % CEILING_RULE_SPLIT,
         len(rule_split) > CEILING_RULE_SPLIT, rule_split),
        ("④", "ملفّاتٌ لا تُحلَّلُ نحويًّا", len(unparsed),
         "(السقف %d — نازل)" % CEILING_UNPARSED,
         len(unparsed) > CEILING_UNPARSED, unparsed),
        ("⑤", "قارئُ أسطرٍ بغيرِ utf-8-sig", len(encoding_debt),
         "(السقف %d — لا سقفَ له)" % CEILING_ENCODING,
         len(encoding_debt) > CEILING_ENCODING, encoding_debt),
    )
    for mark, title, value, bound, _bad, _detail in table:
        print("  %s %-34s %5d %s" % (mark, title, value, bound))
    print("  بذورٌ قِيسَت على الطرفَين: %d · قرّاءُ @rule: %d · قرّاءُ أسطرٍ: %d"
          " · المرجع: %s:%d"
          % (measured, len(rules), len(readers), ref_rel, ref_line))

    failed = [row for row in table if row[4]]
    if failed or lexicon_short:
        print("─" * 74)
        for mark, title, value, _bound, _bad, detail in failed:
            if mark == "②" and shrunk:
                print("  ✗ ② انكمشَ المُعلَن: %d < %d"
                      % (value, FLOOR_HANDWRITTEN))
                print("     ⤷ سُدَّ دَينٌ؟ أنزِلْ CEILING_HANDWRITTEN و"
                      "FLOOR_HANDWRITTEN معًا إلى %d ثمّ أعِدِ العيار." % value)
            else:
                print("  ✗ %s %s: %d يتجاوزُ حدَّه" % (mark, title, value))
                # (AR) وتفصيلُ اللامتغيِّرِ المُخفِقِ **وحدَه** يُطبَع: عرضُ
                #      صفوفِ غيرِه يُوهِمُ القارئَ بموضعِ العطب.
                for line in detail[:8]:
                    print("      · %s" % line)
        if lexicon_short:
            # (AR) 🔑 **الانكماشُ حكمٌ لا عطبُ آلة.** كان يُرفَعُ استثناءً
            #      فيُقرأُ رمزَ ٢ («لم يُقَسْ شيء») وقد قِيسَ كلُّ شيء، بتشخيصٍ
            #      يتّهمُ («قارئُ الوسمِ أعمى») ولا مخرجَ مُسمًّى. وتقاعدُ وسمٍ
            #      عملٌ مشروعٌ سمّاه هذا الملفُّ بنفسِه.
            print("  ✗ ⑥ انكمشَ معجمُ الوسوم: %d < %d"
                  % (len(lexicon), FLOOR_LEXICON))
            print("     ⤷ تقاعُدٌ مقصود؟ أنزِلْ FLOOR_LEXICON إلى %d في هذا"
                  " الملفِّ ثمّ أعِدِ العيار." % len(lexicon))
        return 1

    print("  ✓ العدّاءُ والحارسُ متّفقانِ على كلِّ بذرة، وقرّاءُ @rule متّفقون،"
          " والترميزُ واحد، ولم ينمُ عددُ الأنماطِ ولم ينكمش.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:  # noqa: BLE001
        sys.stdout.reconfigure(encoding="utf-8")
        print("✗ عطبُ آلةٍ — لم يُقَسْ شيء: %s: %s" % (exc.__class__.__name__, exc))
        raise SystemExit(2)
