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
    description: str = ""
    priority: str = "P1"
    expect_error: str = ""  # (AR) إذا غير فارغ: الاختبار يتوقع خطأ يحتوي هذا النص
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
_RE_DESC = re.compile(r"^#\s*@description:?\s+(.+)$")
_RE_PRIORITY = re.compile(r"^#\s*@priority:?\s+(P[0-9]+(?:\.[\w.]+)?)$")
_RE_STDIN = re.compile(r"^#\s*@stdin_data:?\s+(.+)$")  # (AR) بيانات stdin للاختبارات التفاعلية
_RE_UNORDERED = re.compile(r"^#\s*@unordered\b")          # (AR) فرز الخرج قبل المقارنة
_RE_NONDET = re.compile(r"^#\s*@nondeterministic\b")      # (AR) خرج لاحتمي مُثبَت


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
                    meta.timeout = int(m.group(1))
                    continue
                if _RE_SKIP_COMPILER.match(line):
                    meta.skip_compiler = True
                    continue
                if _RE_SKIP_INTERP.match(line):
                    meta.skip_interpreter = True
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
                    stdin_data: str = "") -> tuple[str, float, str]:
    """
    (AR) تشغيل ملف .ص عبر المفسر وإرجاع (المخرج، الوقت_بالمللي، رسالة_خطأ)
    (EN) Run .ص file via interpreter, return (output, time_ms, error_msg)
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
                 stdin_data: str = "") -> tuple[str, float, str]:
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
            return "", elapsed, f"COMPILE_ERROR: {compile_result.stderr.strip()}"

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


# ═══════════════════════════════════════════════════════════════════════════════════
# الجزء ③ب: وضع التطبيع والمقارنة (ADR-004 / TEST-007)
# Part ③b: Output normalization & comparison
# ═══════════════════════════════════════════════════════════════════════════════════
#
# (AR) المقارنة الحرفية للخرج تترفرف في حالتين أثبتهما الأساس المرجعي (baseline):
#   1. التزامن: ترتيب goroutines/القنوات غير حتمي → @unordered يفرز قبل المقارنة.
#   2. العشري: `/` يُرجع عشري دائماً → فروق تمثيل عائم → تساهل بـ epsilon.
# (EN) Literal comparison flakes on concurrency ordering and float representation.

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

    # (AR) التحقق من @skip
    if meta.skip_compiler and meta.skip_interpreter:
        return TestResult(file=rel_path, status=Status.SKIP, metadata=meta,
                          error_message="تخطي: skip_compiler + skip_interpreter")

    # (AR) تشغيل المفسر
    if meta.skip_interpreter:
        interp_out, interp_time, interp_err = "", 0.0, ""
    else:
        interp_out, interp_time, interp_err = run_interpreter(sad_exe, test_file, timeout,
                                                                   stdin_data=meta.stdin_data)

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
    """(AR) جمع جميع ملفات .ص في المجلدات المحددة"""
    files = []
    if subdirs:
        for sd in subdirs:
            d = tests_dir / sd
            if d.is_dir():
                files.extend(sorted(d.rglob("*.ص")))
    else:
        # (AR) جمع من جميع المجلدات الفرعية
        for d in sorted(tests_dir.iterdir()):
            if d.is_dir():
                files.extend(sorted(d.rglob("*.ص")))
        # (AR) + ملفات في الجذر
        files.extend(sorted(tests_dir.glob("*.ص")))
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

    if verbose and result.error_message:
        print(f"         ↳ {result.error_message}")
    if verbose and result.status == Status.FAIL_OUTPUT:
        print(f"         مفسر:  {result.interp_output[:100]!r}")
        print(f"         مترجم: {result.compiler_output[:100]!r}")


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
    sadc_exe = Path(args.compiler) if args.compiler else project_root / config["paths"]["compiler"]
    # (AR) موقع المشغّل مفصول عن موقع المحتوى: مجلد الاختبارات يأتي من
    #      config["paths"]["tests_dir"] (نسبيّ لجذر المشروع) — اكتمل الترحيل إلى
    #      tests/behavior في TEST-003 (القديم مؤرشف في tests/_archive/dual_execution_legacy).
    # (EN) Runner location decoupled from content: tests dir comes from config;
    #      migration to tests/behavior completed in TEST-003.
    tests_dir = project_root / config["paths"].get("tests_dir", "tests/behavior")
    temp_dir = project_root / config["execution"]["temp_dir"]
    timeout = args.timeout if args.timeout > 0 else config["execution"]["timeout_seconds"]
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

    # (AR) التحقق من الملفات التنفيذية
    if not sad_exe.exists():
        print(f"❌ المفسر غير موجود: {sad_exe}")
        sys.exit(1)
    if not sadc_exe.exists():
        print(f"❌ المترجم غير موجود: {sadc_exe}")
        sys.exit(1)

    # (AR) إنشاء مجلد مؤقت
    temp_dir.mkdir(parents=True, exist_ok=True)

    # (AR) جمع ملفات الاختبار
    if args.file:
        test_file = tests_dir / args.file
        if not test_file.exists():
            # (AR) محاولة البحث في المجلدات الفرعية
            for d in tests_dir.iterdir():
                if d.is_dir() and (d / args.file).exists():
                    test_file = d / args.file
                    break
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
    print(f"  مترجم: {sadc_exe.name}")
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
