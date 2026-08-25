#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسُ تكافؤِ المستقبِلِ العدميّ — يقيسُ العائلةَ كلَّها لا منفذًا منها
==============================================================================

(AR) لماذا وُلِد هذا الحارس (مقيسٌ لا مُخمَّن):

     في حملةِ النصِّ العدميّ جُعِلت `normalizeStringPtr` غلافًا على بابِ **العرض**،
     فمرّ بها نحوُ سبعةَ عشرَ منفذَ **عمليّة**، فصار «س.يحتوي("لا")» على خانةٍ
     عدميّةٍ يطبع **صحيح** — يبحثُ في لفظِ «لاشيء» فيجدُ «لا». أي أنّ انهيارًا
     مرئيًّا صار جوابًا كاذبًا لا يراه أحد. والاختبارُ الحارسُ القائمُ يومَها (٠٧٦)
     لم يكشفه، لأنّه يقفُ عند «.طول()» ويخرجُ قبل غيرها: حارسٌ يمسُّ منفذًا
     واحدًا من عائلةٍ لا يرى انحدارَ العائلة.

(AR) 🔑 فمبدأُ هذا الحارسِ أنّ قائمةَ المنافذِ لا تُكتَبُ هنا يدًا، بل تُقرأ من
     مصدرِ الحقيقة `language-truth/type_methods.yaml`. فطريقةٌ نصّيّةٌ تُضافُ غدًا
     تدخلُ القياسَ من تلقائها، ولا تجدُ قائمةً مكتوبةً تنسى نفسَها.

(AR) وما يُقاس ليس «هل ينهار المترجّم؟» بل **تطابقُ المحرّكَين**: يُشغَّل المفسّرُ
     والمترجّمُ على البرنامجِ نفسِه، ويُقارَنُ المخرَجُ والتشخيصُ حرفًا بحرف. لأنّ
     «لا انهيار» ليست شرطَ الصحّة — الجوابُ الكاذبُ لا ينهارُ أيضًا.

(AR) وصيغُ النداءِ تُكتشَفُ بالقياسِ لا بجدولٍ مكتوب: تُجرَّبُ على نصٍّ **حقيقيّ**،
     وتُقاسُ **كلُّ** صيغةٍ تنجحُ لا أوّلُها — لأنّ «جزء()» تنجحُ أوّلًا، فالوقوفُ
     عندها يترك «جزء(0، 2)» بلا قياسٍ أبدًا، وهي عينُ الصيغةِ التي كانت تُلفّق «لا»
     من لفظِ «لاشيء». أي أنّ حارسًا يقفُ عند أوّلِ صيغةٍ يعجزُ عن كشفِ الصنفِ
     الذي وُلِد له.

(EN) Null-receiver parity guard. The port list is READ from the SoT (type_methods.yaml),
     never hand-written, so a string method added tomorrow is probed automatically.
     What is measured is engine PARITY (interpreter vs compiler), not "absence of a
     crash": the silent-lie regression this guard exists for did not crash either.
     EVERY valid call shape is measured, not the first one that works.

الاستعمال / Usage:
    python scripts/codegen/check_null_receiver_parity.py [--بناء BUILD_DIR] [--تفصيل]
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
import tempfile
import uuid
from pathlib import Path

# (AR) مخرَجُ الطرفيّةِ على ويندوز صفحةُ رموزٍ لا تحمل العربيّة؛ فُرِض UTF-8.
# (EN) Force UTF-8 on Windows consoles.
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

REPO_ROOT = Path(__file__).resolve().parents[2]
SOT_TYPE_METHODS = REPO_ROOT / "language-truth" / "type_methods.yaml"

# (AR) نوعُ الهدفِ في مصدرِ الحقيقة — النصُّ وحدَه موضوعُ هذا الحارس.
# (EN) The SoT target-type token this guard scopes to.
SOT_TARGET_TYPE_STRING = "STRING"

# (AR) نصٌّ حقيقيٌّ لاكتشافِ صيغةِ النداء، ونصٌّ فرعيٌّ موجودٌ فيه.
# (EN) A real string for shape discovery, and a substring present in it.
PROBE_SUBJECT = "أهلا"
PROBE_SUBSTRING = "أه"

# (AR) صيغُ الوسائطِ المُجرَّبة. تُجرَّبُ كلُّها، وتُقاسُ كلُّ ناجحةٍ منها.
# (EN) Argument shapes tried; every one that works is measured.
ARGUMENT_SHAPES: list[str] = [
    "",
    f'"{PROBE_SUBSTRING}"',
    "1",
    f'"{PROBE_SUBSTRING}"، "{PROBE_SUBSTRING}"',
    "0، 1",
    f'"{PROBE_SUBSTRING}"، 1',
]

# (AR) برنامجُ القياس: خانةٌ نصّيّةٌ مُصرَّحةٌ عدمًا — وهي عينُ الحالةِ التي ولّدت
#      الانهيارَ ثمّ الكذبَ الصامت.
# (EN) The probe program: a declared string slot holding null.
NULL_SUBJECT_PROGRAM = "متغير نص س = لاشيء\nاطبع_سطر(س.{method}({args}))\n"
REAL_SUBJECT_PROGRAM = 'اطبع_سطر("{subject}".{method}({args}))\n'

# (AR) 🔑 أرضيّةُ القياس: أقلُّ عددِ منافذَ يجبُ أن تُقاسَ وتتطابقَ حتّى يُقبَلَ
#      الأخضر. القيمةُ مقيسةٌ لا مقدَّرة، وتُترَك دونَ المقيسِ اليومَ قليلًا كي لا
#      تُخفِقَ لتغييرٍ مشروعٍ في مصدرِ الحقيقة. وهبوطُها المفاجئُ إخفاقُ بيئةٍ أو
#      قياسٍ لا نجاحٌ — وهو عينُ الصنفِ الذي يجعلُ الأخضرَ يعني «لم يُقَس».
# (EN) 🔑 Measurement floor: the minimum number of ports that must be measured AND match
#      before green is accepted. A sudden drop is an environment or measurement failure.
MINIMUM_MEASURED_PORTS = 16

# (AR) رموزُ خروجٍ تعني انهيارًا لا رفعًا — على المنصّاتِ الثلاث.
# (EN) Exit codes that mean a crash, not a raise.
CRASH_EXIT_CODES = (139, 3221225477, -11)


class Verdict:
    """(AR) حكمُ منفذٍ واحد. / (EN) One port's verdict."""

    MATCH = "متطابق"
    DIVERGENT = "متباعد"
    NOT_IN_LANGUAGE = "غير_مدعوم_أصلًا"
    COMPILER_MISSING = "المترجّم_لا_يبنيه"


def read_string_methods(sot_path: Path) -> list[dict[str, object]]:
    """
    (AR) يقرأُ الطرائقَ النصّيّةَ من مصدرِ الحقيقةِ باسمِها **ورتبتِها**. القراءةُ
         سطريّةٌ عمدًا (لا PyYAML) لأنّ بقيّةَ حرّاسِ المستودعِ لا تفترضُ حزمةً
         خارجيّة.

    (AR) 🔑 والرتبةُ ليست زينةً ههنا: مُكتشِفُ الصيغِ كان يسألُ **المفسّرَ**
         أيُّ صيغةٍ تصحّ، والمفسّرُ متساهلٌ يبتلعُ الوسائطَ الزائدةَ صامتًا —
         فكان الحارسُ يقيسُ ٤٢ صيغةً ليست من اللغةِ أصلًا (`.عكس(٠، ١)` لطريقةٍ
         رتبتُها صفر). ولمّا صار المترجّمُ يفرضُ الرتبةَ انقلبت تلك الصيغُ
         «لا يبنيها المترجّم» فاحمرَّ الحارسُ على **صوابٍ** لا عطب. والمحرّكُ
         المتساهلُ مِعيارٌ رديءٌ لِما يصحّ؛ الصوابُ يُشتقُّ من مصدرِ الحقيقة.
    (EN) Reads string methods with their arity; line-wise on purpose. The arity is
         load-bearing: shape discovery used to ask the permissive interpreter what
         is valid, so 42 out-of-arity shapes were being measured as if they were
         language. Validity comes from the SoT, not from the lenient engine.
    """
    if not sot_path.exists():
        raise SystemExit(f"❌ مصدرُ الحقيقةِ مفقود / SoT missing: {sot_path}")

    text = sot_path.read_text(encoding="utf-8")
    methods: list[dict[str, object]] = []
    seen: set[str] = set()
    current: str | None = None
    minimum: int | None = None
    maximum: int | None = None
    variadic = False
    for raw in text.splitlines():
        line = raw.strip()
        if line.startswith("- target:"):
            current, minimum, maximum, variadic = None, None, None, False
        matched = re.match(r"^-?\s*method:\s*(\S+)\s*$", line)
        if matched:
            current = matched.group(1)
            minimum, maximum, variadic = None, None, False
            continue
        if current is not None:
            if re.match(r"^min:\s*(\d+)$", line):
                minimum = int(line.split(":", 1)[1])
            elif re.match(r"^max:\s*(\d+)$", line):
                maximum = int(line.split(":", 1)[1])
            elif line == "variadic: true":
                variadic = True
        if line.startswith("target_type:") and current is not None:
            if line.split(":", 1)[1].strip() == SOT_TARGET_TYPE_STRING:
                if current not in seen:
                    seen.add(current)
                    methods.append({
                        "name": current,
                        # (AR) بلا رتبةٍ مُعلَنةٍ لا يُخترَعُ حدّ: تُقاسُ كلُّ الصيغِ
                        #      كما كان قبلَ هذا الحارس. الأداةُ لا تخترعُ عقدًا.
                        "min": minimum,
                        "max": None if variadic else maximum,
                        "declared": minimum is not None,
                    })
            current, minimum, maximum, variadic = None, None, None, False
    return methods


def shape_arity(shape: str) -> int:
    """(AR) عددُ وسائطِ صيغةٍ نصّيّة. / (EN) Argument count of a call shape."""
    return 0 if not shape.strip() else shape.count("،") + 1


def run_program(binary: Path, source: Path, extra: list[str] | None = None) -> tuple[int, str]:
    """(AR) يُشغّلُ ثنائيًّا ويُعيدُ (رمزَ الخروج، المخرَجَ الموحَّد)."""
    command = [str(binary), str(source)] + (extra or [])
    try:
        completed = subprocess.run(
            command,
            capture_output=True,
            timeout=120,
            cwd=str(REPO_ROOT),
        )
    except subprocess.TimeoutExpired:
        return 124, "TIMEOUT"
    output = (completed.stdout + completed.stderr).decode("utf-8", errors="replace")
    return completed.returncode, output


ANSI_ESCAPE = re.compile(r"\x1b\[[0-9;]*[A-Za-z]")

# (AR) رأسُ التشخيصِ في المفسّر: «❌ error [RUN033]: ملف:سطر:عمود».
# (EN) The interpreter's diagnostic header.
DIAGNOSTIC_HEADER = re.compile(r"(?:error|خطأ)\s*\[(?P<code>[A-Z]{3}\d{3})\]")

# (AR) أسطرُ زخرفةِ تقريرِ التشخيصِ وأسطرُ تنقيحِ المترجّم — عرضٌ لا دلالة.
# (EN) Report chrome and compiler debug lines — presentation, not meaning.
CHROME_PREFIXES = ("│", "═", "─", "[DEBUG]", "⚠️", "❌", "📝", "🔍", "✅", "→", "[تحذير")


def strip_ansi(output: str) -> str:
    return ANSI_ESCAPE.sub("", output.replace("\r\n", "\n"))


def diagnostic_sentence(output: str) -> str | None:
    """
    (AR) يستخرجُ جملةَ التشخيصِ من تقريرِ المفسّر: السطرَ الذي يلي رأسَ الخطأ.
         وهي الجملةُ عينُها التي يطبعها المترجّمُ من الكتالوجِ نفسِه، فالمقارنةُ
         بها مقارنةُ **دلالةٍ** لا مقارنةُ صيغةِ عرضٍ — والصيغتان تختلفان عمدًا.
    (EN) Extracts the interpreter's diagnostic sentence (the line after the error
         header). It is the same catalog sentence the compiler prints, so comparing
         it compares MEANING, not the two engines' deliberately different chrome.
    """
    lines = strip_ansi(output).split("\n")
    for index, line in enumerate(lines):
        if DIAGNOSTIC_HEADER.search(line):
            for candidate in lines[index + 1 : index + 4]:
                text = candidate.lstrip("│ ").strip()
                if text:
                    return text
    return None


def program_output(output: str) -> str:
    """
    (AR) ما طبعه البرنامجُ نفسُه، دونَ زخرفةِ التقريرِ وأسطرِ التنقيح.
    (EN) What the program itself printed, minus report chrome and debug lines.
    """
    kept: list[str] = []
    for raw in strip_ansi(output).split("\n"):
        line = raw.strip()
        if not line or line.startswith(CHROME_PREFIXES):
            continue
        if line.startswith("(AR)") or line.startswith("(EN)"):
            continue
        if DIAGNOSTIC_HEADER.search(line):
            continue
        kept.append(line)
    return "\n".join(kept)


def discover_call_shapes(
    interpreter: Path, method: dict[str, object], workdir: Path
) -> list[str]:
    """
    (AR) يكتشفُ صيغَ النداءِ بالقياس: **كلَّ** صيغةٍ تنجحُ على نصٍّ حقيقيّ، لا أوّلَها —
         من بينِ ما تسمحُ به الرتبةُ المُعلَنةُ في مصدرِ الحقيقة. فالمفسّرُ يُسأَلُ
         عن الأنواعِ لا عن العدد: هو متساهلٌ في العددِ فلا يصلحُ مِعيارًا له.
         والقائمةُ الفارغةُ تعني أنّ اللغةَ لا تعرفُ هذه الطريقةَ اليومَ أصلًا.
    (EN) Measures EVERY working call shape within the SoT-declared arity. The
         interpreter is asked about types, not counts — it is lenient about counts.
    """
    name = str(method["name"])
    minimum = method["min"]
    maximum = method["max"]
    shapes: list[str] = []
    lenient: list[str] = []
    for shape in ARGUMENT_SHAPES:
        count = shape_arity(shape)
        out_of_arity = ((minimum is not None and count < int(minimum))
                        or (maximum is not None and count > int(maximum)))
        source = workdir / f"shape_{uuid.uuid4().hex[:8]}.ص"
        source.write_text(
            REAL_SUBJECT_PROGRAM.format(subject=PROBE_SUBJECT, method=name, args=shape),
            encoding="utf-8",
        )
        code, _ = run_program(interpreter, source)
        source.unlink(missing_ok=True)
        if code != 0:
            continue
        # (AR) صيغةٌ خارجَ الرتبةِ يقبلُها المفسّرُ **تباعدٌ قائم** لا شيءٌ يُحذَف:
        #      المترجّمُ يفرضُ رتبةَ طرائقِ الأنواعِ والمفسّرُ لا يفرضُها بعدُ.
        #      تُعَدُّ وتُعلَنُ ولا تُقاسُ عليها الأحكام — إذ ليست من اللغة.
        # (EN) An out-of-arity shape the interpreter accepts is a live divergence,
        #      not something to delete: the compiler enforces type-method arity and
        #      the interpreter does not yet. Counted and named, never judged upon.
        (lenient if out_of_arity else shapes).append(shape)
    method["lenient"] = lenient
    return shapes


def compile_and_run(compiler: Path, source: Path, workdir: Path) -> tuple[bool, int, str]:
    """
    (AR) يبني ثمّ يُشغّل. الرجوعُ: (هل بُني؟، رمزُ الخروج، المخرَج).
         وفشلُ **البناء** صنفٌ آخرُ غيرُ فشلِ التشغيل: الأوّلُ «غيرُ مُنفَّذٍ في
         المترجّم»، والثاني سلوكٌ يُقارَن. وخلطُهما يُخفي أيَّهما وقع.
    (EN) Build then run; a BUILD failure is a different class from a RUN failure.
    """
    exe = workdir / f"probe_{uuid.uuid4().hex[:8]}.exe"
    build_code, build_output = run_program(compiler, source, ["-o", str(exe)])
    if build_code != 0 or not exe.exists():
        return False, build_code, build_output
    try:
        completed = subprocess.run([str(exe)], capture_output=True, timeout=120)
        output = (completed.stdout + completed.stderr).decode("utf-8", errors="replace")
        return True, completed.returncode, output
    except subprocess.TimeoutExpired:
        return True, 124, "TIMEOUT"
    finally:
        exe.unlink(missing_ok=True)


def probe_shape(
    interpreter: Path, compiler: Path, method: str, shape: str, workdir: Path
) -> dict[str, object]:
    """(AR) يقيسُ صيغةَ نداءٍ واحدةً ويُصدرُ حكمَها. / (EN) Measures one call shape."""
    port = f".{method}({shape})"
    source = workdir / f"null_{uuid.uuid4().hex[:8]}.ص"
    source.write_text(
        NULL_SUBJECT_PROGRAM.format(method=method, args=shape), encoding="utf-8"
    )
    try:
        interpreter_code, interpreter_output = run_program(interpreter, source)
        built, compiled_code, compiled_output = compile_and_run(compiler, source, workdir)
    finally:
        source.unlink(missing_ok=True)

    if not built:
        return {
            "port": port,
            "verdict": Verdict.COMPILER_MISSING,
            "detail": program_output(compiled_output)[:180],
        }

    interpreter_sentence = diagnostic_sentence(interpreter_output)
    interpreter_raised = interpreter_code != 0 and interpreter_sentence is not None
    compiled_raised = compiled_code != 0
    compiled_text = strip_ansi(compiled_output).replace("(AR) ", "").replace("(EN) ", "")

    # (AR) انهيارُ التجزئةِ «رفعٌ» في نظرِ رمزِ الخروجِ وحدَه، وهو أبعدُ ما يكون عنه.
    #      فيُسمّى باسمِه كي لا يُقرأ اختلافُ نصٍّ حيث وقع انهيار.
    # (EN) A segfault looks like "raised" to an exit code alone; name it as what it is.
    crash_note = " (انهيارُ تجزئة)" if compiled_code in CRASH_EXIT_CODES else ""

    if interpreter_raised and compiled_raised:
        if interpreter_sentence and interpreter_sentence in compiled_text:
            return {"port": port, "verdict": Verdict.MATCH, "detail": interpreter_sentence[:130]}
        return {
            "port": port,
            "verdict": Verdict.DIVERGENT,
            "detail": (
                "رفعَ المحرّكانِ بتشخيصَين مختلفَين — "
                f"المفسّر: «{(interpreter_sentence or '')[:90]}» ‖ "
                f"المترجّم[{compiled_code}]{crash_note}: "
                f"«{program_output(compiled_output)[:90]}»"
            ),
        }

    if not interpreter_raised and not compiled_raised:
        if program_output(interpreter_output) == program_output(compiled_output):
            return {
                "port": port,
                "verdict": Verdict.MATCH,
                "detail": program_output(interpreter_output)[:130],
            }
        return {
            "port": port,
            "verdict": Verdict.DIVERGENT,
            "detail": (
                f"مخرَجانِ مختلفان — المفسّر: «{program_output(interpreter_output)[:80]}» ‖ "
                f"المترجّم: «{program_output(compiled_output)[:80]}»"
            ),
        }

    # (AR) أحدُهما رفعَ والآخرُ لم يرفع — وهذا عينُ الصنفِ الذي وُلِد له الحارس:
    #      الجوابُ الكاذبُ الصامتُ من جهة، وانهيارُ التجزئةِ من الجهةِ الأخرى.
    # (EN) One raised and the other did not — exactly the class this guard exists for.
    which = "المفسّرُ وحدَه" if interpreter_raised else "المترجّمُ وحدَه"
    return {
        "port": port,
        "verdict": Verdict.DIVERGENT,
        "detail": (
            f"رفعَ {which} — المفسّر[{interpreter_code}]: "
            f"«{(interpreter_sentence or program_output(interpreter_output))[:80]}» ‖ "
            f"المترجّم[{compiled_code}]{crash_note}: «{program_output(compiled_output)[:80]}»"
        ),
    }


def probe_method(
    interpreter: Path, compiler: Path, method: dict[str, object], workdir: Path
) -> list[dict[str, object]]:
    """
    (AR) يقيسُ **كلَّ** صيغِ نداءِ الطريقةِ التي تصحُّ في اللغة، لا صيغةً منها.
    (EN) Measures EVERY valid call shape of the method, not one of them.
    """
    name = str(method["name"])
    shapes = discover_call_shapes(interpreter, method, workdir)
    if not shapes:
        return [{"port": f".{name}()", "verdict": Verdict.NOT_IN_LANGUAGE, "detail": ""}]
    return [probe_shape(interpreter, compiler, name, shape, workdir) for shape in shapes]


def resolve_binary(build_dir: Path, stem: str) -> Path:
    """
    (AR) يجدُ الثنائيَّ عبر المولّداتِ متعدّدةِ التهيئةِ وأحاديّتِها معًا.
    (EN) Resolves the binary across multi-config and single-config generators.
    """
    candidates = [
        build_dir / "bin" / "Debug" / f"{stem}.exe",
        build_dir / "bin" / "Release" / f"{stem}.exe",
        build_dir / "bin" / f"{stem}.exe",
        build_dir / "bin" / stem,
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    joined = "\n  ".join(str(c) for c in candidates)
    raise SystemExit(f"❌ لم يُعثَر على {stem} تحت {build_dir} / binary not found:\n  {joined}")


def main() -> int:
    parser = argparse.ArgumentParser(description="حارسُ تكافؤِ المستقبِلِ العدميّ")
    parser.add_argument("--بناء", dest="build_dir", default="build")
    parser.add_argument("--تفصيل", dest="verbose", action="store_true")
    args = parser.parse_args()

    build_dir = (REPO_ROOT / args.build_dir).resolve()
    interpreter = resolve_binary(build_dir, "sad-run")
    compiler = resolve_binary(build_dir, "sad-build")

    methods = read_string_methods(SOT_TYPE_METHODS)
    if not methods:
        # (AR) قائمةٌ فارغةٌ تعني أنّ القراءةَ انكسرت، لا أنّ اللغةَ بلا طرائق.
        # (EN) An empty list means the reader broke, not that the language is empty.
        print("❌ لم تُقرأ أيُّ طريقةٍ نصّيّةٍ من مصدرِ الحقيقة — القارئُ مكسور.")
        return 1

    # (AR) ما لا رتبةَ له يُعَدُّ ويُذكَر: صمتُ الأداةِ عمّا لم تحدَّهُ يُقرأ
    #      «حُدَّ الكلُّ»، وهو عينُ الادّعاءِ الذي تحرسُ منه هذه الأداة.
    # (EN) Methods without a declared arity are counted and named, never silent.
    undeclared = [str(m["name"]) for m in methods if not m["declared"]]
    print(f"🔍 طرائقُ العائلةِ من مصدرِ الحقيقة: {len(methods)}"
          f" · بلا رتبةٍ مُعلَنة: {len(undeclared)}")
    print(f"   المفسّر:  {interpreter}")
    print(f"   المترجّم: {compiler}")
    if undeclared:
        print("   بلا حدٍّ من مصدرِ الحقيقة (تُقاسُ صيغُها كلُّها): "
              + "، ".join(undeclared))
    print("")

    marks = {
        Verdict.MATCH: "✅",
        Verdict.DIVERGENT: "❌",
        Verdict.NOT_IN_LANGUAGE: "⚪",
        Verdict.COMPILER_MISSING: "⚠️",
    }

    results: list[dict[str, object]] = []
    with tempfile.TemporaryDirectory(prefix="sad_null_parity_") as temp:
        workdir = Path(temp)
        for method in methods:
            for result in probe_method(interpreter, compiler, method, workdir):
                results.append(result)
                print(f"  {marks[str(result['verdict'])]} {result['port']}: {result['verdict']}")
                if args.verbose and result["detail"]:
                    print(f"       {result['detail']}")

    divergent = [r for r in results if r["verdict"] == Verdict.DIVERGENT]
    unsupported = [r for r in results if r["verdict"] == Verdict.NOT_IN_LANGUAGE]
    uncompiled = [r for r in results if r["verdict"] == Verdict.COMPILER_MISSING]
    matched = [r for r in results if r["verdict"] == Verdict.MATCH]

    print("")
    print("═" * 70)
    print(f"  متطابق: {len(matched)}   متباعد: {len(divergent)}")
    print(f"  خارجَ اللغةِ اليوم: {len(unsupported)}   لا يبنيه المترجّم: {len(uncompiled)}")
    lenient_total = sum(len(m.get("lenient", [])) for m in methods)
    if lenient_total:
        print(f"  يقبلُها المفسّرُ خارجَ الرتبةِ المُعلَنة: {lenient_total} صيغة")
    print("═" * 70)

    # (AR) ما لم يُقَس يُذكَر: بوّابةٌ تصمتُ عمّا أسقطته تُقرأ «غطّيتُ الكلَّ».
    # (EN) No silent caps: what was skipped is named, or green reads as "all covered".
    if unsupported:
        print("")
        print("⚪ خارجَ اللغةِ اليومَ (لا يقبلُها المفسّرُ على نصٍّ حقيقيّ):")
        print("   " + "، ".join(str(r["port"]) for r in unsupported))
    if uncompiled:
        print("")
        print("⚠️ لا يبنيها المترجّمُ أصلًا:")
        for entry in uncompiled:
            print(f"   • {entry['port']}: {entry['detail']}")

    exit_code = 0

    # ════════════════════════════════════════════════════════════════════
    # (AR) 🔑 أرضيّةُ القياس. بلاها يجتازُ الحارسُ وهو لم يقِسْ شيئًا: بُرهِن
    #      بالتشغيل أنّ مفسّرًا ومترجّمًا مزيَّفَين يُعطيان «متطابق: ٠ · متباعد: ٠»
    #      ثمّ سطرَ النجاحِ ورمزَ الخروجِ صفرًا. وأرجحُ ما يقعُ على CI ليس تباعدًا
    #      بل عطبُ بيئةٍ (هدفٌ لا يُبنى، رابطٌ مفقود) يُصنّفُ كلَّ منفذٍ «لا يبنيه
    #      المترجّم» ⇒ خضرةٌ فارغة. وقيمةُ هذا الحارسِ كلُّها في حُمرته.
    # (EN) 🔑 Measurement floor. Without it the guard passes having measured nothing —
    #      proven by running it against fake binaries. The likeliest CI failure is not
    #      divergence but a broken environment, which would otherwise read as green.
    # ════════════════════════════════════════════════════════════════════
    if len(matched) < MINIMUM_MEASURED_PORTS:
        print("")
        print(f"❌ لم يُقَس إلّا {len(matched)} منفذًا والأرضيّةُ {MINIMUM_MEASURED_PORTS}.")
        print("   هذا عطبُ بيئةٍ أو قياسٍ لا براءةَ سلوك: المحرّكانِ لم يُشغَّلا كما يجب.")
        print("   (EN) Too few ports measured — an environment failure, not a pass.")
        exit_code = 1

    # (AR) «لا يبنيه المترجّم» ليس حكمًا محايدًا: صيغةٌ يقبلُها المفسّرُ ويرفضُ
    #      المترجّمُ بناءَها تباعُدٌ بذاتها، وهي أيضًا القناعُ الذي يلبَسُه عطبُ
    #      البيئة. فتُفشِلُ الحارسَ ولا تُطبَعُ وحدَها.
    # (EN) "Compiler won't build it" is itself a divergence — and the mask an
    #      environment failure wears. It fails the guard rather than only printing.
    if uncompiled:
        exit_code = 1

    if divergent:
        print("")
        print("❌ تباعُدٌ في المستقبِلِ العدميّ:")
        for entry in divergent:
            print(f"   • {entry['port']}")
            print(f"     {entry['detail']}")
        print("")
        print("(AR) المطلوبُ أن يُطابِقَ المترجّمُ المفسّرَ — لا أن «لا ينهار».")
        print("     منفذُ العمليّةِ يمرُّ بـ`normalizeStringPtr` (فيرفعُ RUN033)،")
        print("     والوسيطُ غيرُ المستقبِلِ يمرُّ بـ`normalizeStringArgPtr` (فيُعرَض).")
        exit_code = 1

    if exit_code == 0:
        print("")
        print(f"✅ {len(matched)} منفذًا مقيسًا، كلُّها متطابقةٌ بين المحرّكَين.")
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
