#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسُ المستقبِلِ العدميّ — يقيسُ العائلةَ كلَّها على قاعدةِ اللغة
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

(AR) 🔑 والمرجعُ **قاعدةٌ في اللغةِ لا مُخرَجُ محرّك**: `RUN003` في
     `language-truth/errors/runtime.yaml` يقولُ حرفيًّا إنّ «محاولةَ الوصولِ لخاصيّةٍ
     أو **استدعاءَ دالّةٍ** على قيمةِ لاشيء **ترفعُ هذا الخطأ»**. فالمقياسُ صار
     مطابقةَ المترجّمِ لهذه القاعدة، لا موافقتَه مُنفِّذًا آخر.

     وكان المرجعُ قبلَ اليومِ تكافؤَ المفسّرِ والمترجّم — وهو مرجعٌ هشٌّ مرّتين:
     يزولُ بزوالِ أحدِ المحرّكَين (وقد زال)، ويُصادِقُ على عقدٍ مكسورٍ ما دام
     المحرّكانِ مكسورَين معًا. والقاعدةُ لا تفعلُ أيًّا من الاثنَين.

     و«لا انهيار» ليست شرطَ الصحّة: الجوابُ الكاذبُ الصامتُ لا ينهارُ أيضًا،
     وهو الصنفُ الذي وُلِدَ له هذا الحارسُ أصلًا.

(AR) وصيغُ النداءِ تُكتشَفُ بالقياسِ لا بجدولٍ مكتوب: تُجرَّبُ على نصٍّ **حقيقيّ**،
     وتُقاسُ **كلُّ** صيغةٍ تنجحُ لا أوّلُها — لأنّ «جزء()» تنجحُ أوّلًا، فالوقوفُ
     عندها يترك «جزء(0، 2)» بلا قياسٍ أبدًا، وهي عينُ الصيغةِ التي كانت تُلفّق «لا»
     من لفظِ «لاشيء». أي أنّ حارسًا يقفُ عند أوّلِ صيغةٍ يعجزُ عن كشفِ الصنفِ
     الذي وُلِد له.

(EN) Null-receiver conformance guard. The port list is READ from the SoT
     (type_methods.yaml), never hand-written, so a string method added tomorrow is
     probed automatically. The oracle is a DECLARED LANGUAGE RULE (RUN003), not
     another engine's output: an engine reference dies with the engine and blesses a
     contract both engines break. "No crash" is not correctness — the silent-lie
     regression this guard exists for did not crash either. EVERY valid call shape
     is measured, not the first one that works.

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

# (AR) 🔑 رمزُ القاعدةِ المُعلَنة: نداءُ طريقةٍ على قيمةِ لاشيء يرفعُ RUN003
#      (`language-truth/errors/runtime.yaml`، الحقلُ `detailed.ar` نصًّا).
# (EN) The declared rule: calling a method on null raises RUN003.
SOT_NULL_RECEIVER_CODE = "RUN003"

# (AR) 🔑 سقفُ الدَّينِ التشخيصيّ. المترجّمُ اليومَ **يرفعُ** على كلِّ منفذٍ
#      (فلا كذبَ صامتًا) لكنّه يرفعُ بنثرٍ بلا رمزِ SoT. فالمخالفةُ مصنَّفةٌ صنفَين:
#        · صامتةٌ أو منهارة ⇒ إخفاقٌ فوريّ (وهو الصنفُ الذي وُلِدَ له الحارس)؛
#        · رافعةٌ بلا رمز  ⇒ دَينٌ معدودٌ بسقفٍ لا يُتجاوَز، فلا يتّسعُ صامتًا.
#      والسقفُ يُخفَّضُ كلّما أُرسِيَ منفذٌ على رمزِه، ولا يُرفَعُ إلّا بقرارٍ مُدوَّن.
# (EN) Diagnostic-debt ceiling. Today the compiler DOES raise at every port (no silent
#      lie) but without an SoT code. Violations are graded: silent/crashing fails
#      immediately; raising without a code is counted against a ceiling that must never
#      grow silently. Lower it as ports get anchored; never raise it without a record.
NO_SOT_CODE_CEILING = 19


class Verdict:
    """(AR) حكمُ منفذٍ واحد على قاعدةِ اللغة. / (EN) One port's verdict vs the rule."""

    CONFORMS = "مطابقٌ_للقاعدة"
    SILENT = "أجابَ_صامتًا"
    CRASHED = "انهارَ"
    NO_CODE = "رفعَ_بلا_رمز"
    NOT_IN_LANGUAGE = "غير_مدعوم_أصلًا"
    COMPILER_MISSING = "المترجّم_لا_يبنيه"

    # (AR) المخالفاتُ الحاجبة: الصنفُ الذي وُلِدَ له الحارس. `NO_CODE` ليست منها
    #      وإنّما تُعَدُّ على سقفٍ — انظر NO_SOT_CODE_CEILING.
    # (EN) Blocking violations; NO_CODE is counted against a ceiling instead.
    BLOCKING = (SILENT, CRASHED)


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

# (AR) رأسُ التشخيصِ الحامِلُ رمزَ الكتالوج: «❌ error [RUN003]: ملف:سطر:عمود».
# (EN) The diagnostic header carrying the catalog code.
DIAGNOSTIC_HEADER = re.compile(r"(?:error|خطأ)\s*\[(?P<code>[A-Z]{3}\d{3})\]")

# (AR) أسطرُ زخرفةِ تقريرِ التشخيصِ وأسطرُ تنقيحِ المترجّم — عرضٌ لا دلالة.
# (EN) Report chrome and compiler debug lines — presentation, not meaning.
CHROME_PREFIXES = ("│", "═", "─", "[DEBUG]", "⚠️", "❌", "📝", "🔍", "✅", "→", "[تحذير")


def strip_ansi(output: str) -> str:
    return ANSI_ESCAPE.sub("", output.replace("\r\n", "\n"))


def diagnostic_sentence(output: str) -> str | None:
    """
    (AR) يستخرجُ جملةَ التشخيصِ من التقرير: السطرَ الذي يلي رأسَ الخطأ. وتُعرَض
         للقارئِ لا لِيُحكَمَ بها — فالحكمُ معقودٌ على **رمزِ** الكتالوجِ وحدَه،
         إذ النثرُ صيغةُ عرضٍ تتبدّلُ والرمزُ عقدٌ لا يتبدّل.
    (EN) Extracts the diagnostic sentence (the line after the error header). It is
         shown, never judged upon: the verdict rests on the catalog CODE alone,
         since prose is presentation and the code is the contract.
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


def diagnostic_prose(output: str) -> str:
    """
    (AR) جملةُ التشخيصِ كما طبعها المترجّمُ فعلًا — أوّلُ سطرِ «(AR)» منها.
         و`program_output` يُسقِطُها عمدًا لأنّها ليست مخرَجَ البرنامج، فيلزمُ
         بابٌ ثانٍ لعرضِ الدَّينِ التشخيصيّ: دَينٌ يُعرَضُ فارغًا لا يُصلَح.
    (EN) The diagnostic sentence as the compiler actually printed it. program_output
         drops it on purpose (it is not program output), so the debt report needs its
         own door: a debt shown as an empty string is a debt nobody can fix.
    """
    for raw in strip_ansi(output).splitlines():
        line = raw.strip()
        if line.startswith("(AR)"):
            return line[4:].strip()
    for raw in strip_ansi(output).splitlines():
        line = raw.strip()
        if line and not line.startswith(CHROME_PREFIXES):
            return line
    return ""


def discover_call_shapes(
    compiler: Path, method: dict[str, object], workdir: Path
) -> list[str]:
    """
    (AR) يكتشفُ صيغَ النداءِ بالقياس: **كلَّ** صيغةٍ تنجحُ على نصٍّ حقيقيّ، لا أوّلَها —
         من بينِ ما تسمحُ به الرتبةُ المُعلَنةُ في مصدرِ الحقيقة. والقائمةُ الفارغةُ
         تعني أنّ اللغةَ لا تعرفُ هذه الطريقةَ اليومَ أصلًا.
    (EN) Measures EVERY working call shape within the SoT-declared arity.
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
        built, code, _ = compile_and_run(compiler, source, workdir)
        source.unlink(missing_ok=True)
        if not built or code != 0:
            continue
        # (AR) صيغةٌ خارجَ الرتبةِ المُعلَنةِ يقبلُها المترجّمُ **مخالفةٌ لمصدرِ
        #      الحقيقة** لا شيءٌ يُحذَف: الرتبةُ مكتوبةٌ في `type_methods.yaml`،
        #      وقبولُ ما يجاوزُها توسيعٌ للغةِ من الأداةِ لا من اللغة.
        #      تُعَدُّ وتُعلَنُ ولا يُقاسُ عليها المستقبِلُ العدميّ — موضوعُها آخر.
        # (EN) An out-of-arity shape the compiler accepts violates the SoT-declared
        #      arity — counted and named, but judged separately from null-receiver.
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
    compiler: Path, method: str, shape: str, workdir: Path
) -> dict[str, object]:
    """
    (AR) يقيسُ صيغةَ نداءٍ واحدةً على قاعدةِ اللغة (RUN003) ويُصدرُ حكمَها.
    (EN) Measures one call shape against the declared rule (RUN003).
    """
    port = f".{method}({shape})"
    source = workdir / f"null_{uuid.uuid4().hex[:8]}.ص"
    source.write_text(
        NULL_SUBJECT_PROGRAM.format(method=method, args=shape), encoding="utf-8"
    )
    try:
        built, code, output = compile_and_run(compiler, source, workdir)
    finally:
        source.unlink(missing_ok=True)

    if not built:
        return {
            "port": port,
            "verdict": Verdict.COMPILER_MISSING,
            "detail": program_output(output)[:180],
        }

    # (AR) انهيارُ التجزئةِ «رفعٌ» في نظرِ رمزِ الخروجِ وحدَه، وهو أبعدُ ما يكونُ عنه.
    # (EN) A segfault looks like "raised" to an exit code alone; it is not one.
    if code in CRASH_EXIT_CODES:
        return {
            "port": port,
            "verdict": Verdict.CRASHED,
            "detail": f"انهيارُ تجزئة [{code}] — والقاعدةُ توجبُ رفعَ "
                      f"{SOT_NULL_RECEIVER_CODE}",
        }

    # (AR) 🔑 رمزُ خروجٍ صفرٌ على مستقبِلٍ عدميٍّ هو **الجوابُ الكاذبُ الصامت**:
    #      البرنامجُ أجابَ حيثُ توجبُ القاعدةُ أن يرفع. وهو الصنفُ الذي وُلِدَ له
    #      هذا الحارسُ («س.يحتوي("لا")» على خانةٍ عدميّةٍ يطبعُ «صحيح»).
    # (EN) Exit 0 on a null receiver is the silent lie the guard was born for.
    if code == 0:
        return {
            "port": port,
            "verdict": Verdict.SILENT,
            "detail": f"أجابَ «{program_output(output)[:90]}» ورمزُ الخروجِ صفر — "
                      f"والقاعدةُ توجبُ رفعَ {SOT_NULL_RECEIVER_CODE}",
        }

    sentence = diagnostic_sentence(output)
    text = strip_ansi(output)
    if SOT_NULL_RECEIVER_CODE in text:
        return {
            "port": port,
            "verdict": Verdict.CONFORMS,
            "detail": (sentence or program_output(output))[:130],
        }

    # (AR) رفعَ — وهذا يسدُّ بابَ الكذبِ الصامت — لكن بلا رمزِ القاعدة. دَينٌ
    #      تشخيصيٌّ معدودٌ لا مخالفةٌ حاجبة، إذ السلوكُ صحيحٌ والتشخيصُ ناقص.
    # (EN) It raised (so no silent lie) but without the rule's code: counted debt.
    return {
        "port": port,
        "verdict": Verdict.NO_CODE,
        "detail": f"رفعَ [{code}] بلا {SOT_NULL_RECEIVER_CODE}: "
                  f"«{(sentence or diagnostic_prose(output))[:90]}»",
    }


def probe_method(
    compiler: Path, method: dict[str, object], workdir: Path
) -> list[dict[str, object]]:
    """
    (AR) يقيسُ **كلَّ** صيغِ نداءِ الطريقةِ التي تصحُّ في اللغة، لا صيغةً منها.
    (EN) Measures EVERY valid call shape of the method, not one of them.
    """
    name = str(method["name"])
    shapes = discover_call_shapes(compiler, method, workdir)
    if not shapes:
        return [{"port": f".{name}()", "verdict": Verdict.NOT_IN_LANGUAGE, "detail": ""}]
    return [probe_shape(compiler, name, shape, workdir) for shape in shapes]


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
    parser = argparse.ArgumentParser(
        description="حارسُ المستقبِلِ العدميّ — على قاعدةِ اللغةِ RUN003")
    parser.add_argument("--بناء", dest="build_dir", default="build")
    parser.add_argument("--تفصيل", dest="verbose", action="store_true")
    args = parser.parse_args()

    build_dir = (REPO_ROOT / args.build_dir).resolve()
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
    print(f"   المترجّم: {compiler}")
    print(f"   القاعدةُ المرجع: {SOT_NULL_RECEIVER_CODE}"
          f" (language-truth/errors/runtime.yaml)")
    if undeclared:
        print("   بلا حدٍّ من مصدرِ الحقيقة (تُقاسُ صيغُها كلُّها): "
              + "، ".join(undeclared))
    print("")

    marks = {
        Verdict.CONFORMS: "✅",
        Verdict.SILENT: "❌",
        Verdict.CRASHED: "❌",
        Verdict.NO_CODE: "🟡",
        Verdict.NOT_IN_LANGUAGE: "⚪",
        Verdict.COMPILER_MISSING: "⚠️",
    }

    results: list[dict[str, object]] = []
    with tempfile.TemporaryDirectory(prefix="sad_null_parity_") as temp:
        workdir = Path(temp)
        for method in methods:
            for result in probe_method(compiler, method, workdir):
                results.append(result)
                print(f"  {marks[str(result['verdict'])]} {result['port']}: {result['verdict']}")
                if args.verbose and result["detail"]:
                    print(f"       {result['detail']}")

    blocking = [r for r in results if r["verdict"] in Verdict.BLOCKING]
    no_code = [r for r in results if r["verdict"] == Verdict.NO_CODE]
    unsupported = [r for r in results if r["verdict"] == Verdict.NOT_IN_LANGUAGE]
    uncompiled = [r for r in results if r["verdict"] == Verdict.COMPILER_MISSING]
    conforming = [r for r in results if r["verdict"] == Verdict.CONFORMS]

    # (AR) المقيسُ فعلًا = ما بلغَ حكمًا سلوكيًّا، مطابقًا كان أو مخالفًا. وما لم
    #      يُبنَ ليس مقيسًا، وعدُّه في الأرضيّةِ يجعلُ عطبَ البيئةِ يشتري خُضرة.
    # (EN) "Measured" = reached a behavioural verdict; unbuilt ports are not measured.
    measured = len(conforming) + len(blocking) + len(no_code)

    print("")
    print("═" * 70)
    print(f"  مطابقٌ للقاعدة: {len(conforming)}   مخالفٌ حاجب: {len(blocking)}")
    print(f"  رفعَ بلا {SOT_NULL_RECEIVER_CODE}: {len(no_code)}"
          f" (السقف {NO_SOT_CODE_CEILING})")
    print(f"  خارجَ اللغةِ اليوم: {len(unsupported)}   لا يبنيه المترجّم: {len(uncompiled)}")
    lenient_total = sum(len(m.get("lenient", [])) for m in methods)
    if lenient_total:
        print(f"  يقبلُها المترجّمُ خارجَ الرتبةِ المُعلَنة في مصدرِ الحقيقة: "
              f"{lenient_total} صيغة")
    print("═" * 70)

    # (AR) ما لم يُقَس يُذكَر: بوّابةٌ تصمتُ عمّا أسقطته تُقرأ «غطّيتُ الكلَّ».
    # (EN) No silent caps: what was skipped is named, or green reads as "all covered".
    if unsupported:
        print("")
        print("⚪ خارجَ اللغةِ اليومَ (لا يبنيها المترجّمُ على نصٍّ حقيقيّ):")
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
    if measured < MINIMUM_MEASURED_PORTS:
        print("")
        print(f"❌ لم يُقَس إلّا {measured} منفذًا والأرضيّةُ {MINIMUM_MEASURED_PORTS}.")
        print("   هذا عطبُ بيئةٍ أو قياسٍ لا براءةَ سلوك: المترجّمُ لم يُشغَّلْ كما يجب.")
        print("   (EN) Too few ports measured — an environment failure, not a pass.")
        exit_code = 1

    # (AR) «لا يبنيه المترجّم» ليس حكمًا محايدًا: صيغةٌ يقبلُها المفسّرُ ويرفضُ
    #      المترجّمُ بناءَها تباعُدٌ بذاتها، وهي أيضًا القناعُ الذي يلبَسُه عطبُ
    #      البيئة. فتُفشِلُ الحارسَ ولا تُطبَعُ وحدَها.
    # (EN) "Compiler won't build it" is itself a divergence — and the mask an
    #      environment failure wears. It fails the guard rather than only printing.
    if uncompiled:
        exit_code = 1

    if blocking:
        print("")
        print("❌ مخالفةٌ حاجبةٌ للقاعدة — أجابَ أو انهارَ حيثُ يجبُ أن يرفع:")
        for entry in blocking:
            print(f"   • {entry['port']}  [{entry['verdict']}]")
            print(f"     {entry['detail']}")
        print("")
        print(f"(AR) القاعدةُ {SOT_NULL_RECEIVER_CODE} في `language-truth/errors/"
              f"runtime.yaml`: نداءُ طريقةٍ على قيمةِ لاشيء **يرفعُ** هذا الخطأ.")
        print("     والجوابُ الصامتُ أخبثُ من الانهيار: لا يراه أحد.")
        exit_code = 1

    # (AR) 🔑 الدَّينُ التشخيصيُّ لا يُفشِلُ بذاتِه، لكنّه **لا يتّسعُ صامتًا**:
    #      تجاوُزُ السقفِ إخفاقٌ، لأنّ اتّساعَ دَينٍ بلا قرارٍ انحدارٌ لا حالةٌ راهنة.
    # (EN) The diagnostic debt does not fail on its own, but it may never grow
    #      silently: exceeding the ceiling is a regression, not a status quo.
    if no_code:
        print("")
        print(f"🟡 رفعَ بلا رمزِ {SOT_NULL_RECEIVER_CODE} "
              f"({len(no_code)} منفذًا · السقف {NO_SOT_CODE_CEILING}):")
        for entry in no_code:
            print(f"   • {entry['port']}: {entry['detail']}")
    if len(no_code) > NO_SOT_CODE_CEILING:
        print("")
        print(f"❌ الدَّينُ التشخيصيُّ اتّسعَ: {len(no_code)} > {NO_SOT_CODE_CEILING}.")
        print("   ارسِ المنفذَ الجديدَ على رمزِه، أو ارفعِ السقفَ بقرارٍ مُدوَّن.")
        exit_code = 1

    if exit_code == 0:
        print("")
        print(f"✅ {measured} منفذًا مقيسًا على القاعدة: "
              f"{len(conforming)} مطابقٌ · {len(no_code)} دَينٌ تشخيصيٌّ داخلَ السقف.")
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
