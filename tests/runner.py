#!/usr/bin/env python3
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: runner.py
الوصف: مُشغّل اختبارات التنفيذ المزدوج — يُقارن مخرجات المفسر والمترجم
المهمة: ADR-03 — تكافؤ 100% بين المفسر والمترجم

(AR) يُشغّل كل ملف .ص عبر المفسر (sad-run.exe) والمترجم (sad-build.exe) ويقارن المخرجات.
     الاختبار ينجح فقط إذا أنتج كلاهما نفس المخرج بالضبط.

(EN) Runs each .ص file through both interpreter and compiler, compares outputs.
     Test passes only when both produce identical output.

الاستخدام:
    python runner.py                          # تشغيل جميع الاختبارات
    python runner.py --level P0               # تشغيل مستوى P0 فقط
    python runner.py --dir core               # تشغيل مجلد محدد
    python runner.py --file 001_hello.ص       # تشغيل ملف واحد
    python runner.py --verbose                # طباعة تفاصيل
    python runner.py --report                 # إنشاء تقرير JSON
    python runner.py --freestanding           # (اختياريّ) تدقيق فجوات الوضع الحرّ --حرّ
    python runner.py --freestanding --report  # + تقرير JSON بالفجوات
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import json
import math
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
import uuid
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass, field, asdict
from enum import Enum
from pathlib import Path
from typing import Optional


def configure_utf8_console() -> None:
    """(AR) تهيئة UTF-8 للـ stdout/stderr على ويندوز وباقي الأنظمة.
    (EN) Configure UTF-8 for stdout/stderr on Windows and other platforms.
    """
    # (AR) فرض UTF-8 داخل العملية نفسها لتجنّب أخطاء cp1255/cp1252 في الطرفية.
    os.environ.setdefault("PYTHONIOENCODING", "utf-8")
    os.environ.setdefault("PYTHONUTF8", "1")

    try:
        if hasattr(sys.stdout, "reconfigure"):
            sys.stdout.reconfigure(encoding="utf-8", errors="replace")
        if hasattr(sys.stderr, "reconfigure"):
            sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    except Exception:
        # (AR) لا نوقف التنفيذ إذا تعذرت إعادة التهيئة؛ نستمر بإعدادات النظام.
        pass


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ①: الحالات والأنواع
# Part ①: Status types
# ═══════════════════════════════════════════════════════════════════════════════════

# (AR) سقفُ المهلةِ لكلِّ اختبار (ثانية). الوسمُ `@timeout` بالثواني — يُمرَّر كما هو
#      إلى subprocess.run — لكنّ عشرين اختبارَ شبكةٍ كُتبت بقيمِ مِلّي-ثانية (3000،
#      15000) فصارت مهلتُها ٥٠ دقيقةً إلى ٤ ساعات. حين علّق انهيارُ LLVM عمليّةَ
#      المصرِّف بقيت البوّابةُ معلَّقةً ٤٧ دقيقةً بلا مخرَجٍ ولا تشخيص. السقفُ يقصّ
#      القيمةَ ويصيح باسمِ الملفّ بدل أن يبتلعَ الخلطَ صامتًا: مهلةٌ مصمَّمةٌ لتمنعَ
#      التعليقَ لا يجوز أن تكون هي سببَه.
# (EN) Per-test timeout ceiling (seconds). The `@timeout` tag is in seconds — passed
#      straight to subprocess.run — but twenty network tests were written with
#      millisecond-scale values (3000, 15000), making their timeout 50 minutes to 4
#      hours. When an LLVM crash wedged the compiler process the gate hung for 47
#      minutes with no output and no diagnostic. The ceiling clamps the value and says
#      so, naming the file, rather than swallowing the unit confusion: a timeout meant
#      to prevent a hang must not become its cause.
MAX_TEST_TIMEOUT_SECONDS = 300


class Status(Enum):
    """(AR) حالة نتيجة الاختبار"""
    PASS = "PASS"                    # (AR) نجح — المخرجات متطابقة
    FAIL_OUTPUT = "FAIL_OUTPUT"      # (AR) فشل — المخرجات مختلفة
    FAIL_INTERP = "FAIL_INTERP"      # (AR) فشل — المفسر أعطى خطأ
    FAIL_COMPILE = "FAIL_COMPILE"    # (AR) فشل — المترجم فشل في الترجمة
    FAIL_RUNTIME = "FAIL_RUNTIME"    # (AR) فشل — الملف المُترجم تعطل عند التشغيل
    FAIL_TIMEOUT = "FAIL_TIMEOUT"    # (AR) فشل — تجاوز المهلة الزمنية
    SKIP = "SKIP"                    # (AR) تم التخطي — ميزة غير مدعومة


@dataclass
class TestMetadata:
    """(AR) البيانات الوصفية المُستخرجة من تعليقات الملف"""
    expected_output: list[str] = field(default_factory=list)
    requires: list[str] = field(default_factory=list)
    timeout: int = 30
    skip_compiler: bool = False
    skip_interpreter: bool = False
    posix_only: bool = False
    windows_only: bool = False
    description: str = ""
    priority: str = "P1"
    expect_error: str = ""  # (AR) إذا غير فارغ: الاختبار يتوقع خطأ يحتوي هذا النص
    # (AR) @expect_compile_error: اختبار سلبيّ للمترجم — يشغّل sad-build ويتوقّع فشل
    #      الترجمة (exit غير صفريّ)، مع نمط رسالة اختياريّ يُطابَق ضدّ stderr.
    #      التصميم: كلّ توجيه يحكم محرّكه — @expect_compile_error وحده ⇒ لا يشغَّل
    #      المفسّر (سالب مترجم فقط)؛ ومع @expect_error معًا ⇒ سالب مزدوج: يجب أن
    #      يُخطئ المفسّر (بنمطه) وأن يرفض المترجم الترجمة (بنمطه) ليمرّ الاختبار.
    # (EN) @expect_compile_error: compiler-negative test — runs sad-build expecting
    #      compilation failure (nonzero exit), with an optional message pattern
    #      matched against stderr. Design: each directive governs its own engine —
    #      alone ⇒ the interpreter is not run; combined with @expect_error ⇒ dual
    #      negative: the interpreter must error AND the compiler must reject.
    expect_compile_error: str = ""
    # (AR) @expect_error_compiled: سالب زمنيّ مترجَم — يجب أن تنجح الترجمة ثمّ
    #      يفشل الثنائيّ المُنتَج زمنيًّا (خروج ≠ 0) ومخرجاته تحوي النمط. يُكمِل
    #      @expect_error (الذي يغطّي المفسّر وحده) فيصنعان معًا سالبًا زمنيًّا
    #      مزدوجًا — شبكة أمان لحرّاس وقت التشغيل المترجَمة (RUN001/RUN009/RUN010)
    #      التي كانت بلا أيّ تغطية سلوكيّة.
    # (EN) @expect_error_compiled: compiled-runtime negative — compilation must
    #      succeed, then the produced binary must fail at runtime (nonzero exit)
    #      with output containing the pattern. Complements @expect_error (which
    #      covers the interpreter only), together forming a dual runtime
    #      negative — a safety net for compiled runtime guards
    #      (RUN001/RUN009/RUN010) that previously had no behavioral coverage.
    expect_error_compiled: str = ""
    stdin_data: str = ""    # (AR) إذا غير فارغ: يُمرَّر كـ stdin للمفسر والمترجم
    # ── وسوم الحتمية (ADR-004 / TEST-007) ──
    unordered: bool = False        # (AR) @unordered: يُفرز الخرج قبل المقارنة (التزامن — ترتيب غير حتمي)
    nondeterministic: bool = False # (AR) @nondeterministic: خرج لاحتمي مُثبَت — مقارنة كمجموعة لا تسلسل


@dataclass
class TestResult:
    """(AR) نتيجة اختبار واحد"""
    file: str
    status: Status
    interp_output: str = ""
    compiler_output: str = ""
    interp_time_ms: float = 0.0
    compiler_time_ms: float = 0.0
    error_message: str = ""
    metadata: Optional[TestMetadata] = None


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ②: قراءة البيانات الوصفية من ملفات .ص
# Part ②: Parse test metadata from .ص files
# ═══════════════════════════════════════════════════════════════════════════════════

# (AR) أنماط البيانات الوصفية في تعليقات أول 30 سطر
# (AR) جميع الأنماط تقبل كلا الصيغتين: @key: value (بنقطتين) و @key value (بدون نقطتين)
_RE_EXPECTED = re.compile(r"^#\s*@expected:?\s+(.+)$")
_RE_REQUIRES = re.compile(r"^#\s*@requires:?\s+(.+)$")
_RE_TIMEOUT = re.compile(r"^#\s*@timeout:?\s+(\d+)$")
_RE_SKIP_COMPILER = re.compile(r"^#\s*@skip_compiler\b")
_RE_SKIP_INTERP = re.compile(r"^#\s*@skip_interpreter\b")
_RE_EXPECT_ERROR = re.compile(r"^#\s*@expect_error:?\s*(.*)$")
_RE_EXPECT_COMPILE_ERROR = re.compile(r"^#\s*@expect_compile_error:?\s*(.*)$")
_RE_EXPECT_ERROR_COMPILED = re.compile(r"^#\s*@expect_error_compiled:?\s*(.*)$")
_RE_DESC = re.compile(r"^#\s*@description:?\s+(.+)$")
_RE_PRIORITY = re.compile(r"^#\s*@priority:?\s+(P[0-9]+(?:\.[\w.]+)?)$")
_RE_STDIN = re.compile(r"^#\s*@stdin_data:?\s+(.+)$")  # (AR) بيانات stdin للاختبارات التفاعلية
_RE_UNORDERED = re.compile(r"^#\s*@unordered\b")          # (AR) فرز الخرج قبل المقارنة
_RE_NONDET = re.compile(r"^#\s*@nondeterministic\b")      # (AR) خرج لاحتمي مُثبَت
# (AR) @posix_only: الاختبار يقيس بدائيّاتٍ دلالتُها POSIX (fork/exec، وصفات، أنابيب)،
#      وهي ترمي خطأً صريحًا على ويندوز عمدًا (RUN066) لا تُحاكى صامتةً. فيُتخطّى هناك
#      تخطّيًا **مرئيًّا** — لا يُشطب من العدّ ولا يُعدّ نجاحًا.
# (EN) @posix_only: gate for tests of POSIX-semantic primitives, which deliberately
#      throw on Windows rather than being silently emulated. Skipped visibly there.
_RE_POSIX_ONLY = re.compile(r"^#\s*@posix_only\b")
# (AR) @windows_only: نظيرُ ما سبق للاختباراتِ التي تحتاجُ برنامجًا ويندوزيًّا
#      (‏cmd.exe) بدل /bin/sh. الوسمانِ للبرنامجِ الخارجيِّ لا للبدائيّة نفسِها.
# (EN) @windows_only: the mirror gate, for tests needing a Windows program.
_RE_WINDOWS_ONLY = re.compile(r"^#\s*@windows_only\b")


def parse_metadata(filepath: Path) -> TestMetadata:
    """(AR) استخراج البيانات الوصفية من تعليقات أول 30 سطر في الملف"""
    meta = TestMetadata()
    try:
        with open(filepath, "r", encoding="utf-8") as f:
            for i, line in enumerate(f):
                if i >= 30:
                    break
                line = line.strip()
                if not line.startswith("#"):
                    continue
                m = _RE_EXPECTED.match(line)
                if m:
                    # (AR) تحويل \n الحرفي إلى أسطر فعلية لدعم صيغة السطر الواحد
                    raw_val = m.group(1)
                    if r"\n" in raw_val:
                        meta.expected_output.extend(raw_val.split(r"\n"))
                    else:
                        meta.expected_output.append(raw_val)
                    continue
                m = _RE_REQUIRES.match(line)
                if m:
                    meta.requires.extend(r.strip() for r in m.group(1).split(","))
                    continue
                m = _RE_TIMEOUT.match(line)
                if m:
                    declared = int(m.group(1))
                    if declared > MAX_TEST_TIMEOUT_SECONDS:
                        # (AR) صيحةٌ باسمِ الملفّ: القيمةُ على الأرجح مِلّي-ثانيةً كُتبت
                        #      في وسمٍ بالثواني. نقصّ ولا نبتلع.
                        # (EN) Shout with the file name: the value is most likely
                        #      milliseconds written into a seconds tag. Clamp, do not swallow.
                        print(f"[runner] ⚠️ @timeout {declared}s > "
                              f"{MAX_TEST_TIMEOUT_SECONDS}s — قُصّ: {filepath.name} "
                              f"(الوسمُ بالثواني؛ أهي مِلّي-ثانية؟)", file=sys.stderr)
                        declared = MAX_TEST_TIMEOUT_SECONDS
                    meta.timeout = declared
                    continue
                if _RE_SKIP_COMPILER.match(line):
                    meta.skip_compiler = True
                    continue
                if _RE_SKIP_INTERP.match(line):
                    meta.skip_interpreter = True
                    continue
                m = _RE_EXPECT_COMPILE_ERROR.match(line)
                if m:
                    meta.expect_compile_error = m.group(1).strip() or "__ANY_ERROR__"
                    continue
                # (AR) يُفحص قبل @expect_error — البادئة المشتركة تجعل نمطه العامّ
                #      يلتهم «_compiled ...» لولا هذا الترتيب.
                # (EN) Checked before @expect_error — the shared prefix would make
                #      the generic pattern swallow "_compiled ..." otherwise.
                m = _RE_EXPECT_ERROR_COMPILED.match(line)
                if m:
                    meta.expect_error_compiled = m.group(1).strip() or "__ANY_ERROR__"
                    continue
                m = _RE_EXPECT_ERROR.match(line)
                if m:
                    meta.expect_error = m.group(1).strip() or "__ANY_ERROR__"
                    continue
                m = _RE_DESC.match(line)
                if m:
                    meta.description = m.group(1)
                    continue
                m = _RE_PRIORITY.match(line)
                if m:
                    meta.priority = m.group(1)
                    continue
                m = _RE_STDIN.match(line)
                if m:
                    # (AR) تحويل \n الحرفي إلى سطر جديد فعلي في stdin_data
                    meta.stdin_data = m.group(1).replace(r"\n", "\n")
                    continue
                if _RE_UNORDERED.match(line):
                    meta.unordered = True
                    continue
                if _RE_POSIX_ONLY.match(line):
                    meta.posix_only = True
                    continue
                if _RE_WINDOWS_ONLY.match(line):
                    meta.windows_only = True
                    continue
                if _RE_NONDET.match(line):
                    # (AR) @nondeterministic يستلزم فرز الخرج (مقارنة كمجموعة)
                    meta.nondeterministic = True
                    meta.unordered = True
                    continue
    except (OSError, UnicodeDecodeError):
        pass
    return meta


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ③: تنفيذ الاختبارات
# Part ③: Test execution
# ═══════════════════════════════════════════════════════════════════════════════════

def run_interpreter(sad_exe: Path, test_file: Path, timeout: int,
                    stdin_data: str = "", work_dir: Path = None) -> tuple[str, float, str]:
    """
    (AR) تشغيل ملف .ص عبر المفسر وإرجاع (المخرج، الوقت_بالمللي، رسالة_خطأ)
    (EN) Run .ص file via interpreter, return (output, time_ms, error_msg)

    (AR) `work_dir`: مجلّدُ عملٍ خاصٌّ بالتشغيلة. المصرَّفُ يعملُ في مجلّدٍ مؤقّتٍ
         منذ البداية (run_compiler)، والمفسّرُ كان يرثُ مجلّدَ المستدعي — أي
         **جذرَ المستودعِ عمليًّا**. فاختبارٌ يكتبُ مسارًا نسبيًّا كان يكتبُ في
         مكانين مختلفين بحسب المحرّك، ويخلّفُ أثرَه في شجرةِ المصدر. وهو الفرقُ
         الذي يمنعُ كتابةَ اختبارٍ **محمولٍ** للوصفاتِ أصلًا: `/tmp` ليس على
         ويندوز، والنسبيُّ لم يكن يعمل. توحيدُ المجلّدِ يفتحُ البابَ للاثنين.
    (EN) The compiled engine has always run in a private temp dir; the
         interpreter inherited the caller's — effectively the repo root. A
         relative path therefore meant two different places depending on the
         engine, which is precisely what made a portable descriptor test
         impossible: /tmp does not exist on Windows and relative did not work.
    """
    start = time.perf_counter()
    try:
        result = subprocess.run(
            [str(sad_exe), str(test_file)],
            capture_output=True,
            text=True,
            timeout=timeout,
            encoding="utf-8",
            errors="replace",
            input=stdin_data if stdin_data else None,
            cwd=str(work_dir) if work_dir else None,
        )
        elapsed = (time.perf_counter() - start) * 1000
        output = result.stdout.rstrip("\n")
        error = result.stderr.strip() if result.returncode != 0 else ""
        return output, elapsed, error
    except subprocess.TimeoutExpired:
        elapsed = (time.perf_counter() - start) * 1000
        return "", elapsed, "TIMEOUT"
    except Exception as e:
        elapsed = (time.perf_counter() - start) * 1000
        return "", elapsed, str(e)


def run_compiler(sadc_exe: Path, test_file: Path, temp_dir: Path, timeout: int,
                 stdin_data: str = "", capture_exit: bool = False) -> tuple[str, float, str]:
    """
    (AR) ترجمة ملف .ص عبر المترجم ثم تشغيل الملف المُنتج
    (EN) Compile .ص file, then run the produced executable
    
    (AR) خطوتان:
      1. sad-build test.ص -o test.exe
      2. ./test.exe
    """
    exe_name = test_file.stem + "_" + uuid.uuid4().hex[:8] + ".exe"

    # (AR) مجلد عمل فريد لكل ترجمة — لمنع تضارب الملفات المؤقتة (.o, .ll) عند التوازي
    work_dir = temp_dir / ("work_" + uuid.uuid4().hex[:8])
    work_dir.mkdir(parents=True, exist_ok=True)
    exe_path = work_dir / exe_name

    # (AR) تحويل المسارات لمطلقة — لأن cwd يغيّر السياق فالمسارات النسبية لن تعمل
    abs_sadc = Path(sadc_exe).resolve()
    abs_test = Path(test_file).resolve()
    abs_exe = exe_path.resolve()

    start = time.perf_counter()
    try:
        # (AR) خطوة 1: الترجمة
        compile_result = subprocess.run(
            [str(abs_sadc), str(abs_test), "-o", str(abs_exe)],
            capture_output=True,
            text=True,
            timeout=timeout,
            encoding="utf-8",
            errors="replace",
            cwd=str(work_dir),
        )
        if compile_result.returncode != 0:
            elapsed = (time.perf_counter() - start) * 1000
            # (AR) المصرّفُ يكتبُ تقريرَ التشخيصِ في stdout، وسقوطُه بإشارةٍ لا يكتبُ
            #      شيئًا في الاثنَين؛ فبـstderr وحدَه يظهرُ الإخفاقُ في CI بلا سبب.
            #      رمزُ الخروجِ يُذكَرُ دائمًا: السالبُ إشارةٌ (‑11 = SIGSEGV، ‑6 = إجهاض).
            # (EN) The compiler writes its diagnostic report to stdout, and a crash by
            #      signal writes to neither; stderr alone makes a CI failure causeless.
            #      Always report the exit code: negative means a signal.
            # (AR) ذيلُ stdout لا رأسُه: بناءُ التنقيحِ يسبقُ الخطأَ بمئاتِ أسطرِ [DEBUG].
            # (EN) stdout tail, not head: a Debug build precedes the error with
            #      hundreds of [DEBUG] lines.
            detail = compile_result.stderr.strip()
            if not detail:
                detail = "[ذيل stdout] " + compile_result.stdout.strip()[-600:]
            return "", elapsed, (f"COMPILE_ERROR (رمز {compile_result.returncode}): "
                                 f"{detail}")

        if not exe_path.exists():
            elapsed = (time.perf_counter() - start) * 1000
            return "", elapsed, "COMPILE_ERROR: لم يُنتج ملف تنفيذي"

        # (AR) خطوة 2: التشغيل
        run_result = subprocess.run(
            [str(exe_path)],
            capture_output=True,
            text=True,
            timeout=timeout,
            encoding="utf-8",
            errors="replace",
            input=stdin_data if stdin_data else None,
        )
        elapsed = (time.perf_counter() - start) * 1000
        output = run_result.stdout.rstrip("\n")
        error = run_result.stderr.strip() if run_result.returncode != 0 else ""
        # (AR) capture_exit (لسالب @expect_error_compiled الزمنيّ): حرّاس وقت
        #      التشغيل المترجَمة تطبع التشخيص على stdout وتخرج بـexit(1) وstderr
        #      فارغ — فبدون هذا الوسم يبدو الفشل نجاحًا. لا يغيّر المسار الافتراضيّ.
        # (EN) capture_exit (for the @expect_error_compiled runtime negative):
        #      compiled runtime guards print the diagnostic to stdout and exit(1)
        #      with an empty stderr — without this marker the failure looks like
        #      success. The default path is unchanged.
        if capture_exit and run_result.returncode != 0 and not error:
            error = f"RUNTIME_EXIT:{run_result.returncode}"
        return output, elapsed, error

    except subprocess.TimeoutExpired:
        elapsed = (time.perf_counter() - start) * 1000
        return "", elapsed, "TIMEOUT"
    except Exception as e:
        elapsed = (time.perf_counter() - start) * 1000
        return "", elapsed, str(e)
    finally:
        # (AR) تنظيف الملف التنفيذي المؤقت
        if exe_path.exists():
            try:
                exe_path.unlink()
            except OSError:
                pass
        # (AR) تنظيف مجلد العمل المؤقت
        if work_dir.exists():
            try:
                shutil.rmtree(work_dir, ignore_errors=True)
            except OSError:
                pass


FLOAT_EPSILON = 1e-9
_RE_NUMERIC = re.compile(r"^[+-]?\d+(?:\.\d+)?(?:[eE][+-]?\d+)?$")


def _lines_equal_with_float(a: str, b: str, eps: float = FLOAT_EPSILON) -> bool:
    """(AR) مقارنة سطرين مع تساهل عائم على الرموز الرقمية.
    (EN) Compare two lines with float tolerance on numeric tokens.
    يقارن نصّياً أولاً؛ ثم رمزاً برمز: الرقمية بـ epsilon والباقي نصّياً.
    """
    if a == b:
        return True
    tokens_a, tokens_b = a.split(), b.split()
    if len(tokens_a) != len(tokens_b):
        return False
    for ta, tb in zip(tokens_a, tokens_b):
        if ta == tb:
            continue
        if _RE_NUMERIC.match(ta) and _RE_NUMERIC.match(tb):
            try:
                if math.isclose(float(ta), float(tb), rel_tol=eps, abs_tol=eps):
                    continue
            except ValueError:
                pass
        return False
    return True


def compare_outputs(interp_out: str, compiler_out: str, meta: TestMetadata) -> bool:
    """(AR) مقارنة مخرجَي المفسر والمترجم وفق وسوم التطبيع.
    (EN) Compare interpreter/compiler outputs honoring normalization tags.

    - @unordered / @nondeterministic: تُفرز الأسطر قبل المقارنة (كمجموعة مرتّبة).
    - تساهل عائم بـ epsilon يُطبَّق دائماً على الرموز الرقمية.
    - بلا وسوم وبلا فروق عائمة: يكافئ المقارنة الحرفية تماماً (لا تغيير سلوكي).
    """
    a_lines = interp_out.split("\n")
    b_lines = compiler_out.split("\n")
    if meta and (meta.unordered or meta.nondeterministic):
        a_lines = sorted(a_lines)
        b_lines = sorted(b_lines)
    if len(a_lines) != len(b_lines):
        return False
    return all(_lines_equal_with_float(x, y) for x, y in zip(a_lines, b_lines))


def classify_flakiness(sad_exe: Path, test_files: list, n: int, timeout: int) -> dict:
    """(AR) مصنّف الرفرفة (ADR-004 / TEST-007 T1.6).
    (EN) Flakiness classifier.

    يشغّل كل اختبار N مرة **بالمفسر وحده** ويصنّفه:
      - 'flaky'        : مخرجات مختلفة بين التشغيلات → لا-حتمية فعلية → مرشّح @nondeterministic.
      - 'deterministic': مخرجات ثابتة → أي فشل تكافؤ لاحق = خطأ حقيقي (لا يُقنَّع — BF-09).
    """
    flaky, deterministic = [], []
    for tf in test_files:
        meta = parse_metadata(tf)
        outs = set()
        for _ in range(n):
            out, _t, _e = run_interpreter(sad_exe, tf, meta.timeout or timeout,
                                          stdin_data=meta.stdin_data)
            outs.add(out)
        (flaky if len(outs) > 1 else deterministic).append(str(tf))
    return {"runs": n, "flaky": flaky, "deterministic": deterministic}


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ③-ب: تدقيق الوضع الحرّ (اختياريّ) — كشف فجوات الكود تحت --حرّ
# Part ③-b: Freestanding-mode audit (optional) — surface codegen gaps under --حرّ
# ═══════════════════════════════════════════════════════════════════════════════════
#
# (AR) وضعٌ اختياريّ (يُفعَّل بـ --الوضع-الحرّ / --freestanding) لا يشارك في التنفيذ
#      المزدوج الإلزاميّ. لكلّ ملف .ص نُصدِر تمثيل LLVM مرّتين عبر المترجم:
#        عاديّ:  sad-build ملف.ص --أظهر-llvm
#        حرّ:    sad-build ملف.ص --حرّ --أظهر-llvm
#      إصدار LLVM (لا ربط) يعزل فجوات توليد الكود عن قيود الربط/نقطة الدخول في
#      النواة. التصنيف:
#        سليم   : نجح الإصداران — لا فجوة.
#        فجوة   : نجح العاديّ وفشل الحرّ — فجوة خاصّة بالوضع الحرّ (المقصد).
#        غير مدعوم: فشل الإصداران — قيد سابق لا علاقة له بالوضع الحرّ.
#        شاذّ   : فشل العاديّ ونجح الحرّ — يُبلَّغ للاطّلاع.
# (EN) Optional mode (not part of the mandatory dual run). For each .ص file we emit
#      LLVM IR twice (normal vs --حرّ). Emitting IR (no link) isolates codegen gaps
#      from kernel link/entry constraints. A "gap" = normal OK but freestanding fails.

FREESTANDING_FLAG = "--حرّ"
EMIT_LLVM_FLAG = "--أظهر-llvm"
# (AR) وسم تشخيص SEM019 (المصدر: بوّابة سلامة الوضع الحرّ) — رفضٌ مقصود لا فجوة.
#      مطابقٌ حرفيًّا لِما يؤكّده حارس test_freestanding_builtin_gate.py.
# (EN) SEM019 diagnostic marker — an intended freestanding rejection, NOT a codegen
#      gap. Kept identical to what the freestanding gate test asserts.
SEM019_MARKER = "غير متاحة في الوضع الحرّ"


def emit_llvm_probe(sadc_exe: Path, test_file: Path, temp_dir: Path,
                    timeout: int, freestanding: bool) -> tuple[bool, str]:
    """(AR) يُصدِر LLVM IR لملفٍ (بلا ربط). يعيد (نجَح، رسالة_الخطأ_المختصرة).
    (EN) Emit LLVM IR for a file (no link). Returns (ok, short_error)."""
    # (AR) uuid كامل لتفادي تصادم عيد-الميلاد تحت توازٍ كثيف (كلّ مسبار مجلّده).
    work_dir = temp_dir / ("fs_" + uuid.uuid4().hex)
    work_dir.mkdir(parents=True, exist_ok=True)
    out_ll = (work_dir / (test_file.stem + ".ll")).resolve()
    cmd = [str(Path(sadc_exe).resolve()), str(Path(test_file).resolve()), EMIT_LLVM_FLAG]
    if freestanding:
        cmd.append(FREESTANDING_FLAG)
    cmd += ["-o", str(out_ll)]
    try:
        result = subprocess.run(
            cmd, capture_output=True, text=True, timeout=timeout,
            encoding="utf-8", errors="replace", cwd=str(work_dir),
        )
        if result.returncode != 0:
            msg = (result.stderr or result.stdout or "").strip()
            return False, msg[:400]
        if not out_ll.exists():
            return False, "لم يُنتج ملف LLVM"
        return True, ""
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT"
    except Exception as e:  # noqa: BLE001
        return False, str(e)
    finally:
        shutil.rmtree(work_dir, ignore_errors=True)


def audit_one_freestanding(sadc_exe: Path, test_file: Path, temp_dir: Path,
                           timeout: int) -> dict:
    """(AR) يدقّق ملفًّا واحدًا ويصنّفه:
        ok          : نجح الإصداران.
        gap         : عاديّ نجح · حرّ فشل بخطأ **غير** SEM019 ⇒ فجوة توليد حقيقيّة.
        intended    : عاديّ نجح · حرّ رُفض بـ SEM019 ⇒ قيدٌ مقصود لا عيب.
        unsupported : فشل الإصداران ⇒ خارج نطاق الوضع الحرّ.
        anomaly     : عاديّ فشل · حرّ نجح.
        inconclusive: مهلة على أيّ مسبار ⇒ لا حكم.
    (EN) Classify one file; a real 'gap' is a non-SEM019 freestanding failure."""
    normal_ok, normal_err = emit_llvm_probe(sadc_exe, test_file, temp_dir, timeout, freestanding=False)
    free_ok, free_err = emit_llvm_probe(sadc_exe, test_file, temp_dir, timeout, freestanding=True)
    if normal_err == "TIMEOUT" or free_err == "TIMEOUT":
        category = "inconclusive"
    elif normal_ok and free_ok:
        category = "ok"
    elif normal_ok and not free_ok:
        # (AR) رفض SEM019 مقصود ⇒ «قيد»؛ أيّ فشل آخر (ICE/verify) ⇒ «فجوة» حقيقيّة.
        category = "intended" if SEM019_MARKER in free_err else "gap"
    elif not normal_ok and not free_ok:
        category = "unsupported"
    else:
        category = "anomaly"
    return {
        "file": str(test_file),
        "category": category,
        "normal_ok": normal_ok,
        "free_ok": free_ok,
        "error": free_err if category in ("gap", "inconclusive") else "",
    }


def run_freestanding_audit(sadc_exe: Path, test_files: list, temp_dir: Path,
                           timeout: int, max_parallel: int, use_colors: bool,
                           verbose: bool) -> dict:
    """(AR) يشغّل تدقيق الوضع الحرّ عبر كلّ الملفّات ويطبع تقرير الفجوات.
    (EN) Run the freestanding audit over all files and print a gap report."""
    b = _BOLD if use_colors else ""
    r = _RESET if use_colors else ""
    g = _GREEN if use_colors else ""
    rd = _RED if use_colors else ""
    y = _YELLOW if use_colors else ""

    # (AR) استبعاد الاختبارات السالبة (@expect_error) وما يتخطّى المترجم — مصمَّمة
    #      لتفشل الترجمة عاديًّا، فتُضخّم «غير مدعوم» بلا صلة بالوضع الحرّ.
    # (EN) Exclude negative (@expect_error) and compiler-skipped tests — designed to
    #      fail normal compilation, so they'd inflate "unsupported" spuriously.
    audit_files: list[Path] = []
    excluded = 0
    for tf in test_files:
        meta = parse_metadata(tf)
        if (meta.expect_error or meta.expect_compile_error
                or meta.expect_error_compiled or meta.skip_compiler):
            excluded += 1
            continue
        audit_files.append(tf)

    print(f"\n{b}═══ تدقيق الوضع الحرّ ({FREESTANDING_FLAG}) — كشف فجوات الكود ═══{r}")
    print(f"  مترجم: {sadc_exe.name}")
    print(f"  ملفات:  {len(audit_files)}  (استُبعد {excluded} اختبارًا سالبًا/متخطّيًا للمترجم)")
    print(f"  الطريقة: إصدار LLVM عاديّ مقابل {FREESTANDING_FLAG} (بلا ربط)\n")

    audited: list[dict] = []
    if max_parallel > 1 and len(audit_files) > 1:
        with ThreadPoolExecutor(max_workers=max_parallel) as executor:
            futures = {
                executor.submit(audit_one_freestanding, sadc_exe, tf, temp_dir, timeout): tf
                for tf in audit_files
            }
            for future in as_completed(futures):
                audited.append(future.result())
    else:
        for tf in audit_files:
            audited.append(audit_one_freestanding(sadc_exe, tf, temp_dir, timeout))
    audited.sort(key=lambda x: x["file"])

    gaps = [a for a in audited if a["category"] == "gap"]
    intended = [a for a in audited if a["category"] == "intended"]
    unsupported = [a for a in audited if a["category"] == "unsupported"]
    anomalies = [a for a in audited if a["category"] == "anomaly"]
    inconclusive = [a for a in audited if a["category"] == "inconclusive"]
    ok = [a for a in audited if a["category"] == "ok"]
    normal_success = sum(1 for a in audited if a["normal_ok"])

    # (AR) حارس مصداقيّة: إن لم ينجح أيّ مسبار عاديّ، فالمترجم على الأرجح بلا LLVM
    #      (لا تُطبَع «لا فجوات» مطمئنةً بينما لم يُصدَّر شيء أصلًا).
    # (EN) Credibility guard: zero normal successes ⇒ compiler likely lacks LLVM;
    #      don't print a reassuring "no gaps" when nothing was emitted at all.
    if audited and normal_success == 0:
        print(f"{rd}{b}⚠️ فشل إصدار LLVM العاديّ لكلّ الملفّات — المترجم على الأرجح "
              f"بلا دعم LLVM. التدقيق غير ذي دلالة.{r}\n")
    elif gaps:
        print(f"{rd}{b}الفجوات الحقيقيّة (نجح عاديًّا · فشل حرًّا بخطأ غير SEM019): {len(gaps)}{r}")
        for a in gaps:
            print(f"  {rd}✗{r} {Path(a['file']).name}")
            if verbose and a["error"]:
                first_line = a["error"].splitlines()[0] if a["error"] else ""
                print(f"      {y}↳ {first_line}{r}")
    else:
        print(f"{g}{b}لا فجوات توليد حقيقيّة: كلّ إخفاق حرّ هو رفض SEM019 مقصود.{r}")

    print()
    print(f"{b}الملخّص:{r}")
    print(f"  {g}سليم:          {len(ok)}{r}")
    print(f"  {rd}فجوة حقيقيّة:   {len(gaps)}{r}")
    print(f"  {y}قيد مقصود:     {len(intended)}{r}  (رفض SEM019 — سلوك مصمَّم لا عيب)")
    print(f"  {y}غير مدعوم:     {len(unsupported)}{r}  (فشل عاديًّا أيضًا — خارج النطاق)")
    if anomalies:
        print(f"  {y}شاذّ:           {len(anomalies)}{r}  (فشل عاديًّا · نجح حرًّا)")
    if inconclusive:
        print(f"  {y}بلا حكم:       {len(inconclusive)}{r}  (مهلة على أحد المسبارين)")

    return {
        "flag": FREESTANDING_FLAG,
        "total": len(audited),
        "excluded": excluded,
        "normal_success": normal_success,
        "ok": [a["file"] for a in ok],
        "gaps": [{"file": a["file"], "error": a["error"]} for a in gaps],
        "intended": [a["file"] for a in intended],
        "unsupported": [a["file"] for a in unsupported],
        "anomalies": [a["file"] for a in anomalies],
        "inconclusive": [{"file": a["file"], "error": a["error"]} for a in inconclusive],
    }


def _half_checked(rel_path: str, meta: "TestMetadata", interp_out: str,
                  interp_time: float, why: str) -> "TestResult":
    """(AR) سالبٌ مزدوجٌ بلا مترجم — نصفُه مفحوصٌ فعلًا.

    كان يُتخطّى كلُّه حين يغيب المترجم، والحقُّ أنّ نصفَه الأوّل (رفضُ المفسّر
    مع مطابقة النصّ) **قد فُحص بالفعل** قبل بلوغ هذا الموضع: لو لم يرمِ
    المفسّرُ أو رمى بنصٍّ آخرَ لكان الاختبارُ سقط أعلاه. فالتخطّي كان يرمي
    فحصًا تمَّ ويعلنُ «لم يُقَس شيء» — وهو غيرُ صحيح.

    وأثرُه ليس نظريًّا: على ويندوز لا يُبنى `sad-build` غالبًا (وهو يسقط
    محلّيًّا على برنامجٍ فارغ)، فكانت سوالبُ وحدة عمليات كلُّها تُتخطّى هناك
    ولا يُقاس من المفسّرِ شيء — أي أنّ المنصّةَ التي أُضيف لها التنفيذُ حديثًا
    هي بعينها التي لا تُقاس. والنصفُ المقيسُ يُعلَن نصفًا (‏«مفسّر فقط») لا
    يُقدَّم تكافؤًا مزدوجًا.

    (EN) A dual negative with no compiler: its interpreter half has already
    been verified above (a missing or mismatched error would have failed the
    test there), so skipping the whole thing discards a check that ran and
    reports "nothing measured", which is untrue. It is reported as an
    interpreter-only pass, never counted as dual parity.
    """
    if meta.expect_error:
        return TestResult(file=rel_path, status=Status.PASS,
                          interp_output=interp_out, interp_time_ms=interp_time,
                          metadata=meta,
                          error_message="نصفُ التكافؤ: المفسّر رفض — " + why)
    return TestResult(file=rel_path, status=Status.SKIP, metadata=meta,
                      error_message="تخطي: " + why)


def run_single_test(
    sad_exe: Path,
    sadc_exe: Path,
    test_file: Path,
    temp_dir: Path,
    default_timeout: int,
) -> TestResult:
    """(AR) تشغيل اختبار واحد عبر المفسر والمترجم ومقارنة المخرجات"""
    meta = parse_metadata(test_file)
    timeout = meta.timeout or default_timeout
    rel_path = str(test_file)

    # (AR) لا مترجم متاح (sadc_exe=None): وضع المفسّر-فقط — نفرض skip_compiler.
    #      هذا هو المقصد المُصرَّح في dual_tests.cmake (يُمرَّر --compiler فقط حين
    #      يوجد هدف sad-build؛ وإلا تُقارَن مخرجات المفسّر بـ@expected).
    # (EN) No compiler available (sadc_exe=None): interpreter-only mode — force
    #      skip_compiler. This matches the intent in dual_tests.cmake.
    if sadc_exe is None:
        meta.skip_compiler = True

    # (AR) بوّابة المنصّة: @posix_only على غير POSIX ⇒ تخطٍّ مرئيّ لا فشل.
    # (EN) Platform gate: @posix_only off POSIX ⇒ a visible skip, not a failure.
    if meta.posix_only and os.name != "posix":
        return TestResult(file=rel_path, status=Status.SKIP, metadata=meta,
                          error_message="تخطي: @posix_only على منصّة غير POSIX")
    # (AR) ونظيرُها: @windows_only. وُجِد لأنّ بعضَ اختباراتِ البدائيّاتِ تحتاج
    #      **برنامجًا خارجيًّا** (‏/bin/sh مقابل cmd.exe)، فالبرنامجُ لا المفهومُ
    #      هو ما يفترق. وما لا يحتاجُ برنامجًا صار محمولًا بلا وسمِ منصّةٍ أصلًا.
    # (EN) The mirror gate: some primitive tests need an actual external program
    #      (/bin/sh vs cmd.exe) — the program differs, not the concept. Tests that
    #      need no program carry no platform tag at all.
    if meta.windows_only and os.name != "nt":
        return TestResult(file=rel_path, status=Status.SKIP, metadata=meta,
                          error_message="تخطي: @windows_only على منصّة غير ويندوز")

    # (AR) التحقق من @skip
    if meta.skip_compiler and meta.skip_interpreter:
        return TestResult(file=rel_path, status=Status.SKIP, metadata=meta,
                          error_message="تخطي: skip_compiler + skip_interpreter")

    # (AR) تشغيل المفسر — سالبُ المترجم وحده (@expect_compile_error بلا @expect_error)
    #      لا يشغّل المفسّر: التوجيه يحكم محرّكه فقط (انظر تعليق TestMetadata).
    # (EN) Run interpreter — a compiler-only negative (@expect_compile_error without
    #      @expect_error) skips the interpreter: each directive governs its own engine.
    # (AR) تركيبة متناقضة: @expect_compile_error (الترجمة تفشل) مع
    #      @expect_error_compiled (الترجمة تنجح ويفشل الثنائيّ زمنيًّا) — لا
    #      يجتمعان؛ نرفض صراحةً بدل ترك الثاني شيفرة ميّتة صامتة.
    # (EN) Contradictory combo: @expect_compile_error (compilation must fail)
    #      with @expect_error_compiled (compilation must succeed then the binary
    #      fails at runtime) — mutually exclusive; reject explicitly instead of
    #      leaving the latter silently dead.
    if meta.expect_compile_error and meta.expect_error_compiled:
        return TestResult(file=rel_path, status=Status.FAIL_OUTPUT, metadata=meta,
                          error_message="توجيهان متناقضان: expect_compile_error مع expect_error_compiled")

    # (AR) كلّ توجيه يحكم محرّكه: سالبُ المترجم المنفرد (ترجمةً أو زمنيًّا) بلا
    #      @expect_error لا يشغّل المفسّر — وإلّا أُهملت نتيجته إهمالًا صامتًا
    #      (يقنّع انحدار مفسّر) أو أفشلته مهلته.
    # (EN) Each directive governs its own engine: a compiler-only negative
    #      (compile-time or runtime) without @expect_error skips the interpreter
    #      — otherwise its result is silently discarded (masking interpreter
    #      regressions) or its timeout spuriously fails the test.
    if meta.skip_interpreter or ((meta.expect_compile_error or meta.expect_error_compiled)
                                 and not meta.expect_error):
        interp_out, interp_time, interp_err = "", 0.0, ""
    else:
        interp_work = temp_dir / ("interp_" + uuid.uuid4().hex[:8])
        interp_work.mkdir(parents=True, exist_ok=True)
        try:
            interp_out, interp_time, interp_err = run_interpreter(
                sad_exe, Path(test_file).resolve(), timeout,
                stdin_data=meta.stdin_data, work_dir=interp_work)
        finally:
            shutil.rmtree(interp_work, ignore_errors=True)

    if interp_err == "TIMEOUT":
        return TestResult(file=rel_path, status=Status.FAIL_TIMEOUT,
                          interp_time_ms=interp_time, metadata=meta,
                          error_message="المفسر تجاوز المهلة")

    # ═══════════════════════════════════════════════════════════════
    # (AR) اختبارات سلبية: @expect_error — الاختبار يتوقع خطأ من المفسر
    # (EN) Negative tests: @expect_error — test expects interpreter error
    # ═══════════════════════════════════════════════════════════════
    if meta.expect_error:
        combined = interp_out + "\n" + (interp_err or "")
        if interp_err:
            # (AR) المفسر أعطى خطأ — نتحقق من محتوى الخطأ
            if meta.expect_error == "__ANY_ERROR__" or meta.expect_error in combined:
                # (AR) سالب مزدوج (@expect_error + @expect_compile_error أو
                #      @expect_error_compiled): لا نمرّر بعدُ — يجب أن يرفض المترجم
                #      ترجمةً أو زمنيًّا أيضًا (الكتلتان التاليتان).
                # (EN) Dual negative (@expect_error + @expect_compile_error or
                #      @expect_error_compiled): don't pass yet — the compiler must
                #      also reject, at compile time or at runtime (next blocks).
                if not meta.expect_compile_error and not meta.expect_error_compiled:
                    return TestResult(file=rel_path, status=Status.PASS,
                                      interp_output=interp_out, interp_time_ms=interp_time,
                                      metadata=meta)
            else:
                return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                                  interp_output=combined, interp_time_ms=interp_time,
                                  metadata=meta,
                                  error_message=f"خطأ لا يحتوي '{meta.expect_error}'")
        else:
            # (AR) المفسر لم يُعطِ خطأ — هذا فشل لاختبار سلبي
            return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                              interp_output=interp_out, interp_time_ms=interp_time,
                              metadata=meta,
                              error_message="اختبار سلبي: المفسر لم يُعطِ خطأ")

    # ═══════════════════════════════════════════════════════════════
    # (AR) اختبارات سلبية للمترجم: @expect_compile_error — يشغَّل sad-build
    #      ويُتوقَّع فشل الترجمة (COMPILE_ERROR)، مع مطابقة نمط اختياريّ ضدّ
    #      stderr. نجاح الترجمة = فشل الاختبار السلبيّ (شبكة أمان حقيقيّة
    #      لحرّاس الترجمة مثل SEM002/SEM005 — مراجعة أميليا م-4).
    # (EN) Compiler-negative tests: @expect_compile_error — run sad-build and
    #      expect compilation failure (COMPILE_ERROR), optionally matching a
    #      pattern against stderr. Successful compilation fails the negative
    #      test (a real safety net for compile-time guards — Amelia M-4).
    # ═══════════════════════════════════════════════════════════════
    if meta.expect_compile_error:
        if meta.skip_compiler:
            # (AR) لا مترجم متاحًا (وضع مفسّر-فقط) أو @skip_compiler صريح — لا
            #      يمكن فحص الرفض الترجميّ.
            # (EN) No compiler available (interpreter-only mode) or explicit
            #      @skip_compiler — the compile rejection cannot be checked.
            return _half_checked(rel_path, meta, interp_out, interp_time,
                                 "expect_compile_error بلا مترجم")
        compiler_out, compiler_time, compiler_err = run_compiler(
            sadc_exe, test_file, temp_dir, timeout, stdin_data=meta.stdin_data)
        if compiler_err == "TIMEOUT":
            return TestResult(file=rel_path, status=Status.FAIL_TIMEOUT,
                              interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                              metadata=meta, error_message="المترجم تجاوز المهلة")
        if compiler_err.startswith("COMPILE_ERROR"):
            if (meta.expect_compile_error == "__ANY_ERROR__"
                    or meta.expect_compile_error in compiler_err):
                return TestResult(file=rel_path, status=Status.PASS,
                                  interp_output=interp_out, interp_time_ms=interp_time,
                                  compiler_time_ms=compiler_time, metadata=meta)
            return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                              interp_output=interp_out, compiler_output=compiler_err,
                              interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                              metadata=meta,
                              error_message=f"خطأ الترجمة لا يحتوي '{meta.expect_compile_error}'")
        # (AR) المترجم لم يرفض — فشل الاختبار السلبيّ (سواء نجح التنفيذ أو انهار)
        # (EN) Compiler did not reject — the negative test fails (whether the
        #      produced binary ran or crashed)
        return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                          interp_output=interp_out, compiler_output=compiler_out,
                          interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                          metadata=meta,
                          error_message="اختبار سلبي: المترجم لم يرفض الترجمة")

    # ═══════════════════════════════════════════════════════════════
    # (AR) سالب زمنيّ مترجَم: @expect_error_compiled — يجب أن تنجح الترجمة ثمّ
    #      يفشل الثنائيّ زمنيًّا (خروج ≠ 0) ومخرجاته تحوي النمط. شبكة أمان
    #      لحرّاس وقت التشغيل المترجَمة (RUN001/RUN009/RUN010) التي كان الرنر
    #      يتركها بلا أيّ تغطية (السلبيّات كانت مفسّرًا حصرًا).
    # (EN) Compiled-runtime negative: @expect_error_compiled — compilation must
    #      succeed, then the binary must fail at runtime (nonzero exit) with
    #      output containing the pattern. Safety net for compiled runtime
    #      guards (RUN001/RUN009/RUN010) the runner previously never exercised
    #      (negatives were interpreter-only).
    # ═══════════════════════════════════════════════════════════════
    if meta.expect_error_compiled:
        if meta.skip_compiler:
            return _half_checked(rel_path, meta, interp_out, interp_time,
                                 "expect_error_compiled بلا مترجم")
        compiler_out, compiler_time, compiler_err = run_compiler(
            sadc_exe, test_file, temp_dir, timeout, stdin_data=meta.stdin_data,
            capture_exit=True)
        if compiler_err == "TIMEOUT":
            return TestResult(file=rel_path, status=Status.FAIL_TIMEOUT,
                              interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                              metadata=meta, error_message="المترجم تجاوز المهلة")
        if compiler_err.startswith("COMPILE_ERROR"):
            return TestResult(file=rel_path, status=Status.FAIL_COMPILE,
                              interp_output=interp_out, compiler_output=compiler_err,
                              interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                              metadata=meta,
                              error_message="سالب زمنيّ مترجَم: الترجمة يجب أن تنجح")
        if not compiler_err:
            return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                              interp_output=interp_out, compiler_output=compiler_out,
                              interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                              metadata=meta,
                              error_message="سالب زمنيّ مترجَم: الثنائيّ لم يفشل زمنيًّا")
        combined_c = compiler_out + "\n" + compiler_err
        if (meta.expect_error_compiled == "__ANY_ERROR__"
                or meta.expect_error_compiled in combined_c):
            return TestResult(file=rel_path, status=Status.PASS,
                              interp_output=interp_out, interp_time_ms=interp_time,
                              compiler_time_ms=compiler_time, metadata=meta)
        return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                          interp_output=interp_out, compiler_output=combined_c,
                          interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                          metadata=meta,
                          error_message=f"مخرجات الفشل الزمنيّ لا تحوي '{meta.expect_error_compiled}'")

    if interp_err and not meta.skip_interpreter:
        return TestResult(file=rel_path, status=Status.FAIL_INTERP,
                          interp_output=interp_out, interp_time_ms=interp_time,
                          metadata=meta, error_message=interp_err)

    # (AR) تشغيل المترجم
    if meta.skip_compiler:
        # (AR) إذا تم تخطي المترجم، نقارن مع @expected فقط
        if meta.expected_output:
            expected = "\n".join(meta.expected_output)
            if compare_outputs(interp_out, expected, meta):
                return TestResult(file=rel_path, status=Status.PASS,
                                  interp_output=interp_out, interp_time_ms=interp_time,
                                  metadata=meta)
            else:
                return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                                  interp_output=interp_out, interp_time_ms=interp_time,
                                  metadata=meta,
                                  error_message=f"المفسر ≠ المتوقع")
        return TestResult(file=rel_path, status=Status.SKIP, metadata=meta,
                          error_message="تخطي: skip_compiler بدون @expected")

    compiler_out, compiler_time, compiler_err = run_compiler(sadc_exe, test_file, temp_dir, timeout,
                                                               stdin_data=meta.stdin_data)

    if compiler_err == "TIMEOUT":
        return TestResult(file=rel_path, status=Status.FAIL_TIMEOUT,
                          interp_output=interp_out, interp_time_ms=interp_time,
                          compiler_time_ms=compiler_time, metadata=meta,
                          error_message="المترجم تجاوز المهلة")

    if compiler_err and compiler_err.startswith("COMPILE_ERROR"):
        return TestResult(file=rel_path, status=Status.FAIL_COMPILE,
                          interp_output=interp_out, interp_time_ms=interp_time,
                          compiler_time_ms=compiler_time, metadata=meta,
                          error_message=compiler_err)

    if compiler_err:
        return TestResult(file=rel_path, status=Status.FAIL_RUNTIME,
                          interp_output=interp_out, compiler_output=compiler_out,
                          interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                          metadata=meta, error_message=compiler_err)

    # (AR) إذا تم تخطي المفسّر (@skip_interpreter): نقارن مخرج المترجم بـ@expected فقط.
    #      ميزات خاصّة بالمترجم (قوالب/جنيسات، @تجميع، عناصر واجهة) لا يدعمها المفسّر بعد.
    # (EN) Interpreter skipped (@skip_interpreter): compare compiler output to @expected only.
    #      Compiler-only features (templates/generics, inline asm, UI widgets) the interpreter
    #      does not support yet.
    if meta.skip_interpreter:
        if meta.expected_output:
            expected = "\n".join(meta.expected_output)
            if compare_outputs(compiler_out, expected, meta):
                return TestResult(file=rel_path, status=Status.PASS,
                                  compiler_output=compiler_out, compiler_time_ms=compiler_time,
                                  metadata=meta)
            else:
                return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                                  compiler_output=compiler_out, compiler_time_ms=compiler_time,
                                  metadata=meta, error_message="المترجم ≠ المتوقع")
        return TestResult(file=rel_path, status=Status.SKIP, metadata=meta,
                          error_message="تخطي: skip_interpreter بدون @expected")

    # (AR) مقارنة المخرجات — عبر وضع التطبيع (ADR-004: فرز @unordered + تساهل عائم)
    if compare_outputs(interp_out, compiler_out, meta):
        # (AR) تحقق إضافي من @expected إن وُجد
        if meta.expected_output:
            expected = "\n".join(meta.expected_output)
            if not compare_outputs(interp_out, expected, meta):
                return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                                  interp_output=interp_out, compiler_output=compiler_out,
                                  interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                                  metadata=meta,
                                  error_message=f"متطابقان لكن ≠ المتوقع")
        return TestResult(file=rel_path, status=Status.PASS,
                          interp_output=interp_out, compiler_output=compiler_out,
                          interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                          metadata=meta)
    else:
        return TestResult(file=rel_path, status=Status.FAIL_OUTPUT,
                          interp_output=interp_out, compiler_output=compiler_out,
                          interp_time_ms=interp_time, compiler_time_ms=compiler_time,
                          metadata=meta,
                          error_message="مخرجات المفسر ≠ مخرجات المترجم")


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ④: جمع ملفات الاختبار وتشغيلها
# Part ④: Collect and run tests
# ═══════════════════════════════════════════════════════════════════════════════════

def collect_tests(tests_dir: Path, subdirs: Optional[list[str]] = None) -> list[Path]:
    """(AR) جمع جميع ملفات .ص في المجلدات المحددة — عدا الوحدات المساعدة.

    (AR) اصطلاح: ملفٌّ اسمُه يبدأ بـ«_» **وحدةٌ مساعدة** تُستورَد ولا تُشغَّل بذاتها.

         العلّة: بعضُ الاختبارات تلزمها وحدةٌ تُستورَد (اختبارُ ما يفعله المصرّف
         بالوحدة المستوردة لا يُكتَب في ملفٍّ واحد). و«استورد» لا يقبل مسارًا
         نسبيًّا فلا تُخبَّأ في مجلَّدٍ فرعيّ، فكانت الوحدةُ تُجمَع اختبارًا وتُشغَّل
         وحدَها بلا «رئيسية» فتُحسَب فشلًا — فيُعاقَب الاختبارُ على أنّه احتاج
         وحدةً. اصطلاحٌ واحدٌ في الجامع أوضحُ من حيلةٍ في كلّ اختبار.
    (EN) Convention: a file whose name starts with "_" is a HELPER MODULE — it is
         imported by a test, never run as one. Some tests need an imported module
         (you cannot test what the compiler does to an imported module from a
         single file), and «استورد» takes no relative path so helpers cannot hide
         in a subdirectory. Without this they were collected, run without a main,
         and counted as failures — penalising a test for needing a helper.
    """
    def is_test(p: Path) -> bool:
        return not p.name.startswith("_")

    files = []
    if subdirs:
        for sd in subdirs:
            d = tests_dir / sd
            if d.is_dir():
                files.extend(sorted(f for f in d.rglob("*.ص") if is_test(f)))
    else:
        # (AR) جمع من جميع المجلدات الفرعية
        for d in sorted(tests_dir.iterdir()):
            if d.is_dir():
                files.extend(sorted(f for f in d.rglob("*.ص") if is_test(f)))
        # (AR) + ملفات في الجذر
        files.extend(sorted(f for f in tests_dir.glob("*.ص") if is_test(f)))
    return files


def load_config(config_path: Path) -> dict:
    """(AR) تحميل ملف config.yaml (بدون اعتمادية PyYAML — تحليل بسيط)
    (AR) ملاحظة: المفسر الافتراضي هو `sad-run.exe` (وليس `sad.exe`) لأن
         الأخير صار موزِّع أوامر بعد إعادة هيكلة الأدوات.
    (AR) المترجم هو `sad-build.exe` (هدف CMake `sad-build` بعد توحيد الأسماء —
         لم يَعُد اسم `sadc.exe` البائت مستعملاً). كلا المسارين من تهيئة Debug
         نفسها: أُصلح بناء المترجم في Debug جذريًا (cmake/llvm.cmake) فزال تعارض
         _ITERATOR_DEBUG_LEVEL — وبهذا تزول عثرتا «الثنائي البائت» و«مفسّر Debug +
         مترجم Release» معًا.
    """
    config = {
        "paths": {
            "interpreter": "build/bin/Debug/sad-run.exe",
            "compiler": "build/bin/Debug/sad-build.exe",
            "tests_dir": "tests/behavior",
        },
        "execution": {
            "timeout_seconds": 30,
            "max_parallel": 4,
            "temp_dir": "build/_dual_tmp",
        },
        "output": {
            "report_file": "build/_dual_report.json",
            "verbose": False,
            "colors": True,
        },
    }
    if not config_path.exists():
        return config

    # (AR) تحليل بسيط لـ YAML (المفاتيح الأساسية فقط)
    try:
        import yaml
        with open(config_path, "r", encoding="utf-8") as f:
            loaded = yaml.safe_load(f)
        if isinstance(loaded, dict):
            for section in ["paths", "execution", "output"]:
                if section in loaded and isinstance(loaded[section], dict):
                    config[section].update(loaded[section])
            # (AR) تحميل مستويات الاختبار
            if "levels" in loaded and isinstance(loaded["levels"], dict):
                config["levels"] = loaded["levels"]
            # (AR) تحميل أقسام الميزات المختصرة
            if "sections" in loaded and isinstance(loaded["sections"], dict):
                config["sections"] = loaded["sections"]
            # (AR) تحميل مُسبقات قوة CPU
            if "cpu_presets" in loaded and isinstance(loaded["cpu_presets"], dict):
                config["cpu_presets"] = loaded["cpu_presets"]
    except ImportError:
        # (AR) PyYAML غير متاح — نستخدم القيم الافتراضية
        pass
    except Exception:
        pass

    return config


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ⑤: العرض والتقارير
# Part ⑤: Display and reporting
# ═══════════════════════════════════════════════════════════════════════════════════

# (AR) ألوان ANSI
_GREEN = "\033[92m"
_RED = "\033[91m"
_YELLOW = "\033[93m"
_CYAN = "\033[96m"
_RESET = "\033[0m"
_BOLD = "\033[1m"

_STATUS_COLORS = {
    Status.PASS: _GREEN,
    Status.FAIL_OUTPUT: _RED,
    Status.FAIL_INTERP: _RED,
    Status.FAIL_COMPILE: _RED,
    Status.FAIL_RUNTIME: _RED,
    Status.FAIL_TIMEOUT: _YELLOW,
    Status.SKIP: _CYAN,
}

_STATUS_ICONS = {
    Status.PASS: "✅",
    Status.FAIL_OUTPUT: "❌",
    Status.FAIL_INTERP: "❌",
    Status.FAIL_COMPILE: "❌",
    Status.FAIL_RUNTIME: "❌",
    Status.FAIL_TIMEOUT: "⏰",
    Status.SKIP: "⏭️",
}


def print_result(result: TestResult, verbose: bool, use_colors: bool):
    """(AR) طباعة نتيجة اختبار واحد"""
    icon = _STATUS_ICONS.get(result.status, "?")
    name = Path(result.file).name
    c = _STATUS_COLORS.get(result.status, "") if use_colors else ""
    r = _RESET if use_colors else ""

    line = f"  {icon} {c}{result.status.value:14s}{r}  {name}"
    if result.interp_time_ms > 0 or result.compiler_time_ms > 0:
        line += f"  (مفسر: {result.interp_time_ms:.0f}ms"
        if result.compiler_time_ms > 0:
            line += f" | مترجم: {result.compiler_time_ms:.0f}ms"
        line += ")"
    print(line)

    # (AR) أخطاء الترجمة/التشغيل تُطبع دائمًا — نصّ الخطأ ضروريّ للتشخيص في CI.
    # (EN) Always surface compile/runtime errors — the message is essential for CI triage.
    if result.error_message and result.status in (Status.FAIL_COMPILE, Status.FAIL_RUNTIME):
        print(f"         ↳ {result.error_message[:800]}")
    elif verbose and result.error_message:
        print(f"         ↳ {result.error_message}")
    # (AR) اختلافُ المخرَجاتِ يُطبَعُ دائمًا كذلك: بدونه يظهرُ FAIL_OUTPUT في CI
    #      اسمًا بلا سبب، ولا يُشخَّصُ عيبٌ خاصٌّ بمنصّةٍ إلّا بإعادةِ تشغيلٍ يدويّة.
    # (EN) Always surface the output mismatch too: without it a CI FAIL_OUTPUT is a
    #      bare name, and a platform-specific defect cannot be triaged from the log.
    if result.status == Status.FAIL_OUTPUT:
        width = 400 if verbose else 200
        expected_joined = "\n".join(result.metadata.expected_output)
        print(f"         متوقّع: {expected_joined[:width]!r}")
        print(f"         مفسر:  {result.interp_output[:width]!r}")
        print(f"         مترجم: {result.compiler_output[:width]!r}")


def classify_results(results: list[TestResult]) -> dict:
    """
    (AR) تصنيف النتائج إلى: تكافؤ مزدوج (parity) ومفسر فقط (interpreter-only).
    (EN) Classify results into dual-parity vs interpreter-only categories.

    القواعد:
      - إذا الاختبار يحمل skip_compiler ونجح → interpreter_only_passed
      - إذا نجح بدون skip (مخرجات المفسر = المترجم) → dual_parity_passed
      - الفاشلة والمتخطاة تُصنف حسب نفس المنطق
    """
    dual_parity_passed = 0
    dual_parity_failed = 0
    interp_only_passed = 0
    interp_only_failed = 0
    skipped = 0

    for t in results:
        if t.status == Status.SKIP:
            skipped += 1
            continue

        is_interp_only = t.metadata and t.metadata.skip_compiler

        if t.status == Status.PASS:
            if is_interp_only:
                interp_only_passed += 1
            else:
                dual_parity_passed += 1
        elif t.status.value.startswith("FAIL"):
            if is_interp_only:
                interp_only_failed += 1
            else:
                dual_parity_failed += 1

    return {
        "dual_parity_passed": dual_parity_passed,
        "dual_parity_failed": dual_parity_failed,
        "interp_only_passed": interp_only_passed,
        "interp_only_failed": interp_only_failed,
        "skipped": skipped,
        "total": len(results),
        "total_passed": dual_parity_passed + interp_only_passed,
        "total_failed": dual_parity_failed + interp_only_failed,
    }


def print_summary(results: list[TestResult], use_colors: bool, elapsed_total: float):
    """(AR) طباعة ملخص النتائج مع فصل تكافؤ مزدوج عن مفسر فقط"""
    b = _BOLD if use_colors else ""
    r = _RESET if use_colors else ""
    g = _GREEN if use_colors else ""
    rd = _RED if use_colors else ""
    cy = _CYAN if use_colors else ""

    c = classify_results(results)
    total = c["total"]
    passed = c["total_passed"]
    failed = c["total_failed"]
    skipped = c["skipped"]

    print()
    print(f"{b}═══════════════════════════════════════════════════{r}")
    print(f"{b}  ملخص اختبارات التنفيذ المزدوج{r}")
    print(f"{b}═══════════════════════════════════════════════════{r}")
    print(f"  إجمالي: {total}")
    print(f"  {g}نجح:    {passed}{r}")
    if failed > 0:
        print(f"  {rd}فشل:    {failed}{r}")
    else:
        print(f"  فشل:    {failed}")
    print(f"  تخطي:   {skipped}")
    print()
    print(f"{b}  ── تفصيل النجاح ──{r}")
    print(f"  {g}تكافؤ مزدوج (مفسر+مترجم): {c['dual_parity_passed']}{r}")
    print(f"  {cy}مفسر فقط (@expected):      {c['interp_only_passed']}{r}")
    if c["dual_parity_failed"] > 0 or c["interp_only_failed"] > 0:
        print(f"{b}  ── تفصيل الفشل ──{r}")
        if c["dual_parity_failed"] > 0:
            print(f"  {rd}فشل تكافؤ:    {c['dual_parity_failed']}{r}")
        if c["interp_only_failed"] > 0:
            print(f"  {rd}فشل مفسر فقط: {c['interp_only_failed']}{r}")
    if total > 0:
        pct = (passed / total) * 100
        color = g if pct == 100 else rd
        print(f"  {color}النسبة:  {pct:.1f}%{r}")
    print(f"  الوقت:   {elapsed_total:.1f}s")
    print(f"{b}═══════════════════════════════════════════════════{r}")


def write_report(results: list[TestResult], report_path: Path, elapsed_total: float):
    """(AR) كتابة تقرير JSON مع فصل مؤشرات التكافؤ والمفسر"""
    c = classify_results(results)
    report = {
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "total": c["total"],
        "passed": c["total_passed"],
        "failed": c["total_failed"],
        "skipped": c["skipped"],
        "dual_parity_passed": c["dual_parity_passed"],
        "dual_parity_failed": c["dual_parity_failed"],
        "interp_only_passed": c["interp_only_passed"],
        "interp_only_failed": c["interp_only_failed"],
        "elapsed_seconds": round(elapsed_total, 2),
        "tests": [],
    }
    for r in results:
        is_interp_only = r.metadata and r.metadata.skip_compiler
        entry = {
            "file": r.file,
            "status": r.status.value,
            "mode": "interpreter_only" if is_interp_only else "dual_parity",
            "interp_time_ms": round(r.interp_time_ms, 1),
            "compiler_time_ms": round(r.compiler_time_ms, 1),
        }
        if r.error_message:
            entry["error"] = r.error_message
        if r.status == Status.FAIL_OUTPUT:
            entry["interp_output"] = r.interp_output[:500]
            entry["compiler_output"] = r.compiler_output[:500]
        report["tests"].append(entry)

    report_path.parent.mkdir(parents=True, exist_ok=True)
    with open(report_path, "w", encoding="utf-8") as f:
        json.dump(report, f, ensure_ascii=False, indent=2)


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ⑤ب: Performance Baselines — تسجيل ومقارنة أزمنة الاختبارات
# Part ⑤b: Performance baselines — record and compare test timing
# ═══════════════════════════════════════════════════════════════════════════════════

def save_baselines(results: list[TestResult], baselines_path: Path) -> None:
    """
    (AR) حفظ أزمنة تنفيذ الاختبارات كقيم مرجعية للمقارنة المستقبلية.
    (EN) Save test execution times as reference baselines for future comparison.

    الصيغة:
    {
      "version": 1,
      "timestamp": "ISO-8601",
      "baselines": {
        "001_hello.ص": {"interp_ms": 12.3, "compiler_ms": 456.7},
        ...
      }
    }
    """
    baselines: dict[str, dict] = {}
    for r in results:
        if r.status == Status.PASS:
            name = Path(r.file).name
            baselines[name] = {
                "interp_ms": round(r.interp_time_ms, 1),
                "compiler_ms": round(r.compiler_time_ms, 1),
            }

    data = {
        "version": 1,
        "timestamp": time.strftime("%Y-%m-%dT%H:%M:%S"),
        "note": "قيم مرجعية لأزمنة التنفيذ — تحذير عند تجاوز 2× القيمة المرجعية",
        "total_tests": len(baselines),
        "baselines": baselines,
    }

    baselines_path.parent.mkdir(parents=True, exist_ok=True)
    with open(baselines_path, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)


def check_baselines(
    results: list[TestResult],
    baselines_path: Path,
    threshold: float = 2.0,
    use_colors: bool = True,
) -> int:
    """
    (AR) مقارنة أزمنة الاختبارات مع القيم المرجعية.
         يُحذّر إذا تجاوز الزمن (threshold × القيمة المرجعية).
         يُرجع عدد التحذيرات.

    (EN) Compare test times against baselines.
         Warns if time exceeds (threshold × baseline).
         Returns warning count.
    """
    if not baselines_path.exists():
        print(f"⚠️  لا يوجد ملف baselines: {baselines_path}")
        print("     شغّل: python runner.py --save-baselines لإنشائه")
        return 0

    try:
        with open(baselines_path, "r", encoding="utf-8") as f:
            data = json.load(f)
    except (json.JSONDecodeError, OSError) as e:
        print(f"⚠️  خطأ في قراءة baselines: {e}")
        return 0

    saved = data.get("baselines", {})
    if not saved:
        return 0

    b = _BOLD if use_colors else ""
    r = _RESET if use_colors else ""
    y = _YELLOW if use_colors else ""
    g = _GREEN if use_colors else ""

    warnings = 0
    regressions: list[str] = []
    improvements: list[str] = []

    for result in results:
        if result.status != Status.PASS:
            continue
        name = Path(result.file).name
        if name not in saved:
            continue

        base = saved[name]
        base_interp = base.get("interp_ms", 0)
        base_comp = base.get("compiler_ms", 0)

        # (AR) فحص المفسر
        if base_interp > 0 and result.interp_time_ms > base_interp * threshold:
            ratio = result.interp_time_ms / base_interp
            regressions.append(
                f"  {y}⏱  {name} — مفسر: {result.interp_time_ms:.0f}ms "
                f"(مرجع: {base_interp:.0f}ms, ×{ratio:.1f}){r}"
            )
            warnings += 1

        # (AR) فحص المترجم (فقط إذا يوجد زمن مترجم فعلي)
        if base_comp > 50 and result.compiler_time_ms > base_comp * threshold:
            ratio = result.compiler_time_ms / base_comp
            regressions.append(
                f"  {y}⏱  {name} — مترجم: {result.compiler_time_ms:.0f}ms "
                f"(مرجع: {base_comp:.0f}ms, ×{ratio:.1f}){r}"
            )
            warnings += 1

        # (AR) رصد التحسينات الكبيرة (أسرع بـ 3× أو أكثر)
        if base_interp > 0 and result.interp_time_ms < base_interp / 3.0:
            ratio = base_interp / result.interp_time_ms
            improvements.append(
                f"  {g}🚀 {name} — مفسر أسرع ×{ratio:.1f} "
                f"({result.interp_time_ms:.0f}ms ← {base_interp:.0f}ms){r}"
            )

    print()
    print(f"{b}═══ مقارنة الأداء مع Baselines ═══{r}")
    print(f"  مرجع: {data.get('timestamp', 'غير معروف')} | {len(saved)} اختبار")

    if regressions:
        print(f"\n  {y}⚠️  تراجعات أداء (تجاوز {threshold}× المرجع):{r}")
        for msg in regressions:
            print(msg)
    else:
        print(f"\n  {g}✅ لا تراجعات أداء — الكود ضمن النطاق المقبول{r}")

    if improvements:
        print(f"\n  {g}تحسينات مُكتشفة:{r}")
        for msg in improvements:
            print(msg)

    print(f"{b}══════════════════════════════════{r}")
    return warnings


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ⑥: نقطة الدخول
# Part ⑥: Entry point
# ═══════════════════════════════════════════════════════════════════════════════════

def main():
    configure_utf8_console()

    parser = argparse.ArgumentParser(
        description="مُشغّل اختبارات التنفيذ المزدوج (المفسر + المترجم)"
    )
    parser.add_argument("--level",
                        help="مستوى الاختبار: P0، P0.متغيرات، P0.أنماط، P1، P2، P3، P4، full، match، oop، ...")
    parser.add_argument("--dir", help="مجلد فرعي محدد (مثل: core)")
    parser.add_argument("--file", help="ملف اختبار واحد")
    parser.add_argument("--verbose", "-v", action="store_true",
                        help="طباعة تفاصيل")
    parser.add_argument("--report", action="store_true",
                        help="إنشاء تقرير JSON")
    parser.add_argument("--html", nargs="?", const="auto",
                        help="إنشاء تقرير HTML (مسار اختياري، افتراضي: build/_dual_report.html)")
    parser.add_argument("--no-color", action="store_true",
                        help="بدون ألوان")
    parser.add_argument("--parallel", type=int, default=0,
                        help="عدد الخيوط المتوازية (0 = تسلسلي) — [قديم، استخدم --workers]")
    parser.add_argument("--workers", type=int, default=0,
                        help="عدد أنوية CPU للتشغيل المتوازي (1=تسلسلي، 0=من config)"
                             " — يتجاوز --parallel")
    parser.add_argument("--cpu",
                        choices=["sequential", "light", "moderate", "full", "max"],
                        help="مُسبق قوة CPU: sequential=1، light=2، moderate=4، full=8، max=أقصى")
    parser.add_argument("--section",
                        help="اسم قسم الميزات المختصر (مثل: متغيرات، أنواع، كائني، أنماط، أخطاء، ...)")
    parser.add_argument("--repeat", type=int, default=1,
                        help="عدد تكرارات التشغيل (burn-in). الافتراضي: 1")
    parser.add_argument("--classify", type=int, nargs="?", const=5, default=0,
                        metavar="N",
                        help="مصنّف الرفرفة (ADR-004/TEST-007): يشغّل كل اختبار N مرة "
                             "بالمفسر (الافتراضي 5)، ويميّز: لاحتمي (مرشّح @nondeterministic) "
                             "مقابل حتمي-فاشل (خطأ حقيقي يُصلَح لا يُقنَّع). لا يقارن المترجم.")
    parser.add_argument("--gate", action="store_true",
                        help="بوّابة قرار (ADR-004): يُصدر PASS/CONCERNS/FAIL. "
                             "FAIL إذا فشل دخان P0 أو هبطت النسبة تحت --gate-floor.")
    parser.add_argument("--gate-floor", type=float, default=None, metavar="PCT",
                        help="أدنى نسبة نجاح مقبولة للبوّابة (مثل 86.0). دونها = FAIL.")
    parser.add_argument("--timeout", type=int, default=0,
                        help="مهلة التنفيذ بالثواني (يتجاوز القيمة في config.yaml)")
    parser.add_argument("--interp", help="مسار المفسر")
    parser.add_argument("--compiler", help="مسار المترجم")
    parser.add_argument(
        "--save-baselines",
        action="store_true",
        help="حفظ أزمنة التنفيذ الحالية كقيم مرجعية (يُنشئ build/_perf_baselines.json)",
    )
    parser.add_argument(
        "--check-baselines",
        action="store_true",
        help="مقارنة الأزمنة مع القيم المرجعية المحفوظة، تحذير عند تجاوز 2×",
    )
    parser.add_argument(
        "--baselines-threshold",
        type=float,
        default=2.0,
        help="نسبة التجاوز المقبولة (الافتراضي: 2.0 = ضعف المرجع)",
    )
    parser.add_argument(
        "--freestanding", "--الوضع-الحرّ",
        dest="freestanding",
        action="store_true",
        help="(اختياريّ) تدقيق الوضع الحرّ: يكشف فجوات الكود عند --حرّ (إصدار LLVM عاديّ مقابل حرّ). لا يشارك في التنفيذ المزدوج الإلزاميّ.",
    )
    args = parser.parse_args()

    # (AR) تحديد جذر المشروع
    # (AR) بعد رفع المشغّل إلى جذر `tests/` (نظام testing-system، TEST-002):
    #      runner.py يعيش في tests/ → جذر المشروع هو المجلد الأب مباشرةً.
    # (EN) After lifting the runner to the `tests/` root, project root is the parent dir.
    runner_dir = Path(__file__).resolve().parent
    project_root = runner_dir.parent  # tests/../

    # (AR) تحميل الإعدادات
    config = load_config(runner_dir / "config.yaml")

    # (AR) المسارات
    sad_exe = Path(args.interp) if args.interp else project_root / config["paths"]["interpreter"]
    # (AR) المترجم اختياريّ: إن مُرِّر صراحةً نلتزمه (ونتحقّق من وجوده لاحقًا)؛
    #      وإلّا نستعمل الافتراضيّ فقط إن وُجد، وإلّا None ⇒ وضع المفسّر-فقط.
    # (EN) Compiler is optional: honor an explicit --compiler; otherwise use the
    #      default only if present, else None ⇒ interpreter-only mode.
    compiler_explicit = bool(args.compiler)
    sadc_exe = Path(args.compiler) if args.compiler else project_root / config["paths"]["compiler"]
    # (AR) موقع المشغّل مفصول عن موقع المحتوى: مجلد الاختبارات يأتي من
    #      config["paths"]["tests_dir"] (نسبيّ لجذر المشروع) — اكتمل الترحيل إلى
    #      tests/behavior في TEST-003 (القديم مؤرشف في tests/_archive/dual_execution_legacy).
    # (EN) Runner location decoupled from content: tests dir comes from config;
    #      migration to tests/behavior completed in TEST-003.
    tests_dir = project_root / config["paths"].get("tests_dir", "tests/behavior")
    temp_dir = project_root / config["execution"]["temp_dir"]
    # (AR) السقفُ نفسُه يسري على `--timeout` وعلى `timeout_seconds` في الإعداد، لا على
    #      الوسمِ وحدَه: سقفٌ يُلتفّ عليه من سطرِ الأوامر ليس سقفًا.
    # (EN) The same ceiling applies to `--timeout` and to the config's `timeout_seconds`,
    #      not only to the tag: a ceiling that can be bypassed from the command line is
    #      not a ceiling.
    timeout = args.timeout if args.timeout > 0 else config["execution"]["timeout_seconds"]
    if timeout > MAX_TEST_TIMEOUT_SECONDS:
        print(f"[runner] ⚠️ المهلةُ الافتراضيّة {timeout}s > "
              f"{MAX_TEST_TIMEOUT_SECONDS}s — قُصّت.", file=sys.stderr)
        timeout = MAX_TEST_TIMEOUT_SECONDS
    use_colors = not args.no_color and config["output"].get("colors", True)
    verbose = args.verbose or config["output"].get("verbose", False)

    # (AR) حل تعارض --workers / --cpu / --parallel بالأولوية:
    #      workers > cpu > parallel > config
    cpu_presets = config.get("cpu_presets", {
        "sequential": 1, "light": 2, "moderate": 4, "full": 8, "max": 0
    })
    if args.workers > 0:
        max_parallel = args.workers
    elif args.cpu:
        preset_val = cpu_presets.get(args.cpu, 4)
        if preset_val == 0:  # max = كل الأنوية
            import os as _os
            max_parallel = _os.cpu_count() or 4
        else:
            max_parallel = preset_val
    elif args.parallel > 0:
        max_parallel = args.parallel
    else:
        max_parallel = config["execution"]["max_parallel"]

    # (AR) حلّ مسار الثنائيّ عابرًا للمنصّات: مسارات config مكتوبة بصيغة ويندوز
    #      (build/bin/Debug/x.exe). على المولّدات أحاديّة التهيئة (Linux/macOS)
    #      تكون الثنائيّات في build/bin/x بلا لاحقة .exe وبلا مجلّد Debug/Release.
    #      نجرّب البدائل بالترتيب حتّى نجد الموجود.
    # (EN) Cross-platform binary resolution: config paths are Windows-style
    #      (multi-config). On single-config generators (Linux/macOS) the binary
    #      lives at build/bin/x with no .exe and no Debug/Release dir. Try fallbacks.
    def _resolve_binary(p: Path) -> Path:
        if p.exists():
            return p
        candidates = []
        stem_variants = [p.name]
        if p.suffix == ".exe":
            stem_variants.append(p.stem)  # بلا .exe
        for nm in stem_variants:
            # المسار كما هو لكن باسم بديل
            candidates.append(p.with_name(nm))
            # إزالة مقطع Debug/Release من المسار
            parts = [seg for seg in p.parent.parts if seg not in ("Debug", "Release")]
            if parts:
                candidates.append(Path(*parts) / nm)
        for c in candidates:
            if c.exists():
                return c
        return p

    sad_exe = _resolve_binary(sad_exe)
    sadc_exe = _resolve_binary(sadc_exe)

    # (AR) التحقق من المفسر — إلزاميّ دائمًا
    if not sad_exe.exists():
        print(f"❌ المفسر غير موجود: {sad_exe}")
        sys.exit(1)
    # (AR) المترجم: إن مُرِّر صراحةً (--compiler) فغيابه خطأ حقيقيّ ⇒ خروج.
    #      وإلّا (لم يُطلَب صراحةً، أي LLVM/sad-build غير مبنيّ) ننتقل إلى وضع
    #      المفسّر-فقط (sadc_exe=None) بدل الفشل — تشغيل بيئيّ لا عيب في الكود.
    # (EN) Compiler: if passed explicitly, its absence is a real error → exit.
    #      Otherwise (not requested — LLVM/sad-build not built) fall back to
    #      interpreter-only mode (sadc_exe=None) instead of failing.
    if not sadc_exe.exists():
        if compiler_explicit:
            print(f"❌ المترجم غير موجود: {sadc_exe}")
            sys.exit(1)
        print(f"⚠️ المترجم غير مبنيّ — وضع المفسّر-فقط (تُقارَن المخرجات بـ@expected)")
        sadc_exe = None

    # (AR) إنشاء مجلد مؤقت
    temp_dir.mkdir(parents=True, exist_ok=True)

    # (AR) جمع ملفات الاختبار
    if args.file:
        test_file = tests_dir / args.file
        if not test_file.exists():
            # (AR) محاولة البحث التكراري في كل المجلدات الفرعية
            #      يسمح بتمرير اسم الملف المجرّد فقط (كما في ctest) مهما كان عمقه
            # (EN) Recursive search across all subdirectories
            #      Allows passing the bare filename (as ctest does) regardless of depth
            matches = list(tests_dir.rglob(args.file))
            if matches:
                test_file = matches[0]
        if not test_file.exists():
            print(f"❌ الملف غير موجود: {args.file}")
            sys.exit(1)
        test_files = [test_file]
    elif args.section:
        # (AR) --section: تشغيل قسم ميزة محدد بالاسم المختصر
        sections_map = config.get("sections", {})
        if args.section not in sections_map:
            b = _BOLD if use_colors else ""
            r = _RESET if use_colors else ""
            print(f"❌ القسم '{args.section}' غير موجود في config.yaml.")
            print(f"\n{b}الأقسام المتاحة:{r}")
            for name, path in sections_map.items():
                print(f"  {name:<14} → {path}")
            sys.exit(1)
        test_files = collect_tests(tests_dir, [sections_map[args.section]])
    elif args.dir:
        test_files = collect_tests(tests_dir, [args.dir])
    elif args.level:
        # (AR) مستويات من config.yaml — يدعم P0.x والأسماء المخصصة
        level_config = config.get("levels", {}).get(args.level, {})
        if not level_config:
            b = _BOLD if use_colors else ""
            r = _RESET if use_colors else ""
            print(f"❌ المستوى '{args.level}' غير موجود في config.yaml.")
            print(f"\n{b}المستويات المتاحة:{r}")
            for name, lconf in config.get("levels", {}).items():
                desc = lconf.get("description", "") if isinstance(lconf, dict) else ""
                print(f"  {name:<20} — {desc}")
            print(f"\n{b}الأقسام المتاحة (--section):{r}")
            for name in config.get("sections", {}):
                print(f"  {name}")
            sys.exit(1)
        dirs = level_config.get("dirs", ["core"])
        test_files = collect_tests(tests_dir, dirs)
    else:
        test_files = collect_tests(tests_dir)

    if not test_files:
        print("⚠️ لا توجد ملفات اختبار")
        sys.exit(0)

    # ═══════════════════════════════════════════════════════════════
    # (AR) وضع المصنّف --classify (ADR-004 / TEST-007 T1.6)
    # ═══════════════════════════════════════════════════════════════
    if args.classify:
        b = _BOLD if use_colors else ""
        r = _RESET if use_colors else ""
        print(f"\n{b}═══ مصنّف الرفرفة ({args.classify}× بالمفسر) ═══{r}")
        print(f"  ملفات: {len(test_files)}\n")
        res = classify_flakiness(sad_exe, test_files, args.classify, timeout)
        print(f"{b}لاحتمي (مرشّح @nondeterministic): {len(res['flaky'])}{r}")
        for f in res["flaky"]:
            print(f"  🔀 {Path(f).name}")
        print(f"\n{b}حتمي (أي فشل تكافؤ = خطأ حقيقي): {len(res['deterministic'])}{r}")
        if args.report:
            out_path = project_root / "build" / "_classify_report.json"
            out_path.parent.mkdir(parents=True, exist_ok=True)
            with open(out_path, "w", encoding="utf-8") as fh:
                json.dump(res, fh, ensure_ascii=False, indent=2)
            print(f"\n📄 التقرير: {out_path}")
        sys.exit(0)

    # ═══════════════════════════════════════════════════════════════
    # (AR) وضع تدقيق الوضع الحرّ --الوضع-الحرّ (اختياريّ، مستقلّ عن التنفيذ المزدوج)
    # (EN) Freestanding-audit mode (optional, standalone from the dual run)
    # ═══════════════════════════════════════════════════════════════
    if args.freestanding:
        if sadc_exe is None:
            print("❌ تدقيق الوضع الحرّ يتطلّب المترجم (sad-build) — غير متاح.")
            sys.exit(2)
        audit = run_freestanding_audit(
            sadc_exe, test_files, temp_dir, timeout, max_parallel, use_colors, verbose,
        )
        if args.report:
            out_path = project_root / "build" / "_freestanding_audit.json"
            out_path.parent.mkdir(parents=True, exist_ok=True)
            with open(out_path, "w", encoding="utf-8") as fh:
                json.dump(audit, fh, ensure_ascii=False, indent=2)
            print(f"\n📄 التقرير: {out_path}")
        # (AR) وضع تشخيصيّ لا بوّابة: يخرج 0 دائمًا كي لا يكسر CI (المقصد كشفٌ لا فرض).
        # (EN) Diagnostic, not a gate: always exit 0 so it never breaks CI.
        sys.exit(0)

    # (AR) الطباعة الأولية
    b = _BOLD if use_colors else ""
    r = _RESET if use_colors else ""
    repeat_count = max(1, args.repeat)
    # (AR) وصف قوة CPU المُستخدمة
    if max_parallel <= 1:
        cpu_label = "تسلسلي (1 نواة)"
    elif args.cpu == "max":
        import os as _os2
        cpu_label = f"أقصى ({max_parallel} نواة من {_os2.cpu_count() or '?'})"
    else:
        cpu_label = f"{max_parallel} أنوية متوازية"

    print(f"\n{b}═══ اختبارات التنفيذ المزدوج ═══{r}")
    print(f"  مفسر:   {sad_exe.name}")
    print(f"  مترجم: {sadc_exe.name if sadc_exe else '(غير متاح — مفسّر فقط)'}")
    print(f"  ملفات:  {len(test_files)}")
    print(f"  CPU:    {cpu_label}")
    if args.level:
        level_desc = config.get("levels", {}).get(args.level, {}).get("description", "")
        if level_desc:
            print(f"  طبقة:   {args.level} — {level_desc}")
    if args.section:
        print(f"  قسم:    {args.section}")
    if repeat_count > 1:
        print(f"  تكرار:  {repeat_count}× (burn-in)")
    print()

    # (AR) تشغيل الاختبارات (مع دعم burn-in تكراري)
    results: list[TestResult] = []
    start_total = time.perf_counter()
    burn_in_failures = 0

    for iteration in range(repeat_count):
        if repeat_count > 1:
            print(f"\n{b}── الجولة {iteration + 1}/{repeat_count} ──{r}")

        iteration_results: list[TestResult] = []

        if max_parallel > 1 and len(test_files) > 1:
            # (AR) تشغيل متوازي
            with ThreadPoolExecutor(max_workers=max_parallel) as executor:
                futures = {
                    executor.submit(
                        run_single_test, sad_exe, sadc_exe, tf, temp_dir, timeout
                    ): tf
                    for tf in test_files
                }
                for future in as_completed(futures):
                    result = future.result()
                    iteration_results.append(result)
                    if repeat_count == 1:
                        print_result(result, verbose, use_colors)
            # (AR) إعادة ترتيب حسب اسم الملف
            iteration_results.sort(key=lambda x: x.file)
        else:
            # (AR) تشغيل تسلسلي
            for tf in test_files:
                result = run_single_test(sad_exe, sadc_exe, tf, temp_dir, timeout)
                iteration_results.append(result)
                if repeat_count == 1:
                    print_result(result, verbose, use_colors)

        # (AR) في وضع burn-in: إظهار ملخص مختصر لكل جولة
        if repeat_count > 1:
            iter_passed = sum(1 for t in iteration_results if t.status == Status.PASS)
            iter_failed = sum(1 for t in iteration_results if t.status.value.startswith("FAIL"))
            iter_total = len(iteration_results)
            g = _GREEN if use_colors else ""
            rd = _RED if use_colors else ""
            re = _RESET if use_colors else ""
            failed_names = [Path(t.file).name for t in iteration_results if t.status.value.startswith("FAIL")]
            if iter_failed > 0:
                burn_in_failures += iter_failed
                print(f"  {rd}الجولة {iteration + 1}: {iter_passed}/{iter_total} نجح | فشل: {', '.join(failed_names)}{re}")
            else:
                print(f"  {g}الجولة {iteration + 1}: {iter_passed}/{iter_total} نجح ✅{re}")

        # (AR) النتائج النهائية = آخر جولة (أو الوحيدة)
        if iteration == repeat_count - 1:
            results = iteration_results

    elapsed_total = time.perf_counter() - start_total

    # (AR) ملخص burn-in إن كان متعدد الجولات
    if repeat_count > 1:
        g = _GREEN if use_colors else ""
        rd = _RED if use_colors else ""
        re = _RESET if use_colors else ""
        print()
        if burn_in_failures == 0:
            print(f"{g}🔥 Burn-in نظيف: {repeat_count} جولة × {len(test_files)} اختبار = 0 فشل{re}")
        else:
            print(f"{rd}⚠️ Burn-in: {burn_in_failures} فشل إجمالي عبر {repeat_count} جولة{re}")

    # (AR) الملخص
    print_summary(results, use_colors, elapsed_total)

    # (AR) التقرير JSON
    if args.report:
        report_path = project_root / config["output"]["report_file"]
        write_report(results, report_path, elapsed_total)
        print(f"\n📄 التقرير: {report_path}")

    # (AR) تقرير HTML
    if args.html:
        from report_html import generate_html_report

        if args.html == "auto":
            html_path = project_root / "build" / "_dual_report.html"
        else:
            html_path = Path(args.html)

        # (AR) تحويل TestResult إلى dict لمولد HTML
        test_dicts = []
        for r in results:
            is_interp_only = r.metadata and r.metadata.skip_compiler
            test_dicts.append({
                "file": r.file,
                "status": r.status.value,
                "mode": "interpreter_only" if is_interp_only else "dual_parity",
                "interp_time_ms": round(r.interp_time_ms, 1),
                "compiler_time_ms": round(r.compiler_time_ms, 1),
                "error": r.error_message or "",
            })

        c = classify_results(results)

        generate_html_report(
            results=test_dicts,
            output_path=html_path,
            total=c["total"],
            passed=c["total_passed"],
            failed=c["total_failed"],
            skipped=c["skipped"],
            dual_parity_passed=c["dual_parity_passed"],
            interp_only_passed=c["interp_only_passed"],
            elapsed=elapsed_total,
        )
        print(f"\n🌐 تقرير HTML: {html_path}")

    # (AR) مسار ملف الـ baselines المرجعية
    baselines_path = project_root / "build" / "_perf_baselines.json"

    # (AR) حفظ الـ baselines — يُحفظ فقط الاختبارات التي نجحت
    if args.save_baselines:
        save_baselines(results, baselines_path)
        passed_count = sum(1 for t in results if t.status == Status.PASS)
        print(f"\n✅ تم حفظ Baselines: {passed_count} اختبار → {baselines_path}")

    # (AR) مقارنة الأداء مع الـ baselines المحفوظة
    if args.check_baselines:
        threshold = args.baselines_threshold
        warnings_count = check_baselines(results, baselines_path, threshold, use_colors)
        if warnings_count > 0:
            print(f"\n⚠️  {warnings_count} تحذير أداء — راجع الاختبارات أعلاه")

    # (AR) كود الخروج: 0 = كل شيء نجح أو تخطي، 1 = يوجد فشل (أو burn-in به فشل)
    failed = sum(1 for t in results if t.status.value.startswith("FAIL"))

    # ═══════════════════════════════════════════════════════════════
    # (AR) بوّابة القرار --gate (ADR-004 / TEST-007 T4)
    # (EN) Decision gate: PASS / CONCERNS / FAIL
    # ═══════════════════════════════════════════════════════════════
    if args.gate:
        total = len(results)
        passed = sum(1 for t in results if t.status == Status.PASS)
        rate = (passed / total * 100.0) if total else 0.0
        # (AR) فشل دخان P0 = حاجز فوري (أعلى مخاطرة)
        p0_failed = any(
            t.status.value.startswith("FAIL") and "P0_smoke" in str(t.file).replace("\\", "/")
            for t in results
        )
        floor_breached = args.gate_floor is not None and rate < args.gate_floor
        g = _GREEN if use_colors else ""
        rd = _RED if use_colors else ""
        y = _YELLOW if use_colors else ""
        re = _RESET if use_colors else ""
        if p0_failed or floor_breached or burn_in_failures > 0:
            verdict, color, code = "FAIL", rd, 1
            reason = ("فشل دخان P0" if p0_failed else
                      f"النسبة {rate:.1f}% < الحد {args.gate_floor}%" if floor_breached else
                      "رفرفة burn-in")
        elif failed > 0:
            verdict, color, code = "CONCERNS", y, 0
            reason = f"{failed} فشل ضمن الحد المقبول (بلا تراجع حرج)"
        else:
            verdict, color, code = "PASS", g, 0
            reason = "صفر فشل"
        print(f"\n{color}{_BOLD if use_colors else ''}بوّابة القرار: {verdict}{re} — {reason}")
        sys.exit(code)

    sys.exit(1 if (failed > 0 or burn_in_failures > 0) else 0)


if __name__ == "__main__":
    main()
