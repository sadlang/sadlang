#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: runner.py — مُشغّل اختبارات استخراج التوثيق المزدوج (sad ↔ sadc)
═══════════════════════════════════════════════════════════════════════════════════

(AR) الفكرة:
    لكل ملف/مشروع اختبار، نُشغّل المفسر `sad` والمترجم `sadc` بنفس وسائط
    استخراج التوثيق (markdown / json / html / pdf). يجب أن يُنتجا مخرجاً
    متطابقاً بايتياً (ما عدا PDF حيث يُسمح بفرق metadata بسيط).

(EN) For every test input, run both `sad` and `sadc` with identical doc-gen
    flags and compare outputs. Byte-identical for md/json/html, small
    metadata-only diff allowed for PDF.

الاستخدام:
    python runner.py
    python runner.py --skip-pdf
    python runner.py --verbose
    python runner.py --filter inheritance
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import os
import subprocess
import sys
import shutil
import time
import hashlib
from pathlib import Path
from dataclasses import dataclass, field
from typing import List, Optional

# (AR) UTF-8 للطرفية على ويندوز
os.environ.setdefault("PYTHONIOENCODING", "utf-8")
os.environ.setdefault("PYTHONUTF8", "1")
try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

# (AR) ألوان ANSI
class C:
    RESET = "\033[0m"; BOLD = "\033[1m"; DIM = "\033[2m"
    GREEN = "\033[92m"; RED = "\033[91m"; YELLOW = "\033[93m"
    CYAN = "\033[96m"; BLUE = "\033[94m"; MAGENTA = "\033[95m"; GRAY = "\033[90m"

# (AR) جذر المشروع
SCRIPT_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = SCRIPT_DIR.parents[1]
SAD = PROJECT_ROOT / "build" / "bin" / "Debug" / "sad.exe"
SADC = PROJECT_ROOT / "build" / "bin" / "Release" / "sadc.exe"
TMP = SCRIPT_DIR / "_tmp"

# (AR) تسامح الفرق لـ PDF (timestamps في metadata قد تختلف ~1KB)
PDF_TOLERANCE_BYTES = 5 * 1024

# (AR) نتيجة اختبار واحد
@dataclass
class TestCase:
    name: str
    kind: str          # "single" أو "project"
    input_path: Path   # مسار الملف أو المجلد
    fmt: str           # markdown / json / html / pdf
    extra_args: List[str] = field(default_factory=list)
    skip_reason: str = ""

@dataclass
class TestResult:
    case: TestCase
    passed: bool
    sad_size: int = 0
    sadc_size: int = 0
    sad_hash: str = ""
    sadc_hash: str = ""
    sad_time_ms: float = 0.0
    sadc_time_ms: float = 0.0
    error: str = ""

# (AR) الامتدادات حسب الصيغة
EXT = {"markdown": "md", "json": "json", "html": "html", "pdf": "pdf"}


def hash_file(p: Path) -> str:
    """(AR) تجزئة sha256 لمحتوى الملف."""
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for chunk in iter(lambda: f.read(65536), b""):
            h.update(chunk)
    return h.hexdigest()


def normalized_bytes(p: Path) -> bytes:
    """(AR) قراءة الملف مع تطبيع نهايات الأسطر إلى LF فقط لأن sad (Debug)
    يكتب CRLF بينما sadc (Release) يكتب LF — والمحتوى المنطقي متطابق."""
    data = p.read_bytes()
    return data.replace(b"\r\n", b"\n").replace(b"\r", b"\n")


def hash_normalized(p: Path) -> str:
    """(AR) تجزئة sha256 بعد تطبيع نهايات الأسطر."""
    return hashlib.sha256(normalized_bytes(p)).hexdigest()


def run_doc_gen(exe: Path, case: TestCase, out_path: Path) -> tuple[bool, str, float]:
    """(AR) يُشغّل sad أو sadc مع وسائط استخراج التوثيق ويعيد (نجاح، خطأ، زمن)."""
    cmd = [str(exe)]
    if case.kind == "project":
        cmd.append(f"--docs-project={case.input_path}")
    cmd.append(f"--docs-format={case.fmt}")
    cmd.append(f"--docs-out={out_path}")
    cmd.extend(case.extra_args)
    if case.kind == "single":
        cmd.append(str(case.input_path))

    t0 = time.perf_counter()
    try:
        # (AR) ملاحظة: stderr قد يحوي ضوضاء headless من Edge — نتجاهلها
        r = subprocess.run(cmd, capture_output=True, timeout=60,
                           cwd=PROJECT_ROOT)
        dt = (time.perf_counter() - t0) * 1000
        if r.returncode != 0 and not out_path.exists():
            err = (r.stderr or b"").decode("utf-8", errors="replace")[:300]
            return False, f"exit={r.returncode}: {err}", dt
        if not out_path.exists() or out_path.stat().st_size == 0:
            return False, "no output file produced", dt
        return True, "", dt
    except subprocess.TimeoutExpired:
        return False, "timeout", (time.perf_counter() - t0) * 1000
    except Exception as e:
        return False, f"exception: {e}", (time.perf_counter() - t0) * 1000


def compare_outputs(fmt: str, sad_path: Path, sadc_path: Path) -> tuple[bool, str]:
    """(AR) يقارن الملفين حسب الصيغة. يعيد (متطابق، سبب)."""
    if fmt == "pdf":
        # (AR) PDF: نقبل فرق metadata — نقارن الحجم بتسامح
        ds = sad_path.stat().st_size
        dc = sadc_path.stat().st_size
        diff = abs(ds - dc)
        if diff > PDF_TOLERANCE_BYTES:
            return False, f"pdf size diff {diff} > {PDF_TOLERANCE_BYTES}"
        return True, ""
    # (AR) ما سواه: تطابق بايتي بعد تطبيع نهايات الأسطر (CRLF↔LF)
    h1 = hash_normalized(sad_path)
    h2 = hash_normalized(sadc_path)
    if h1 != h2:
        return False, (f"normalized hash mismatch ({h1[:8]} vs {h2[:8]}, "
                       f"raw sizes {sad_path.stat().st_size}/{sadc_path.stat().st_size})")
    return True, ""


def run_case(case: TestCase) -> TestResult:
    """(AR) يُشغّل حالة اختبار واحدة (نفس الإدخال على sad و sadc + المقارنة)."""
    if case.skip_reason:
        return TestResult(case=case, passed=True, error="SKIP: " + case.skip_reason)

    ext = EXT[case.fmt]
    sad_out = TMP / f"{case.name}__sad.{ext}"
    sadc_out = TMP / f"{case.name}__sadc.{ext}"
    for p in (sad_out, sadc_out):
        if p.exists():
            p.unlink()

    ok1, err1, t1 = run_doc_gen(SAD, case, sad_out)
    if not ok1:
        return TestResult(case=case, passed=False,
                          error=f"sad failed: {err1}", sad_time_ms=t1)

    ok2, err2, t2 = run_doc_gen(SADC, case, sadc_out)
    if not ok2:
        return TestResult(case=case, passed=False,
                          error=f"sadc failed: {err2}",
                          sad_time_ms=t1, sadc_time_ms=t2)

    same, why = compare_outputs(case.fmt, sad_out, sadc_out)
    res = TestResult(
        case=case, passed=same,
        sad_size=sad_out.stat().st_size,
        sadc_size=sadc_out.stat().st_size,
        sad_hash=hash_normalized(sad_out)[:12] if case.fmt != "pdf" else "",
        sadc_hash=hash_normalized(sadc_out)[:12] if case.fmt != "pdf" else "",
        sad_time_ms=t1, sadc_time_ms=t2,
        error=why,
    )
    return res


def discover_cases(skip_pdf: bool, name_filter: Optional[str]) -> List[TestCase]:
    """(AR) يبني قائمة الاختبارات من inputs/single و inputs/projects."""
    cases: List[TestCase] = []
    formats = ["markdown", "json", "html"]
    if not skip_pdf:
        formats.append("pdf")

    # (AR) ملفات مفردة
    single_dir = SCRIPT_DIR / "inputs" / "single"
    for f in sorted(single_dir.glob("*.ص")):
        base = f.stem
        for fmt in formats:
            name = f"single__{base}__{fmt}"
            if name_filter and name_filter not in name:
                continue
            cases.append(TestCase(name=name, kind="single",
                                  input_path=f, fmt=fmt))

    # (AR) مشاريع
    proj_dir = SCRIPT_DIR / "inputs" / "projects"
    for d in sorted(proj_dir.iterdir()):
        if not d.is_dir():
            continue
        for fmt in formats:
            name = f"project__{d.name}__{fmt}"
            if name_filter and name_filter not in name:
                continue
            cases.append(TestCase(name=name, kind="project",
                                  input_path=d, fmt=fmt))

    # (AR) مشروع zoo مع استثناء internal.ص
    zoo = proj_dir / "zoo"
    if zoo.exists():
        for fmt in formats:
            name = f"project__zoo_excl__{fmt}"
            if name_filter and name_filter not in name:
                continue
            cases.append(TestCase(
                name=name, kind="project", input_path=zoo, fmt=fmt,
                extra_args=["--docs-exclude=internal.ص"],
            ))

    return cases


def print_result(r: TestResult, verbose: bool):
    """(AR) يطبع نتيجة اختبار بسطر ملوّن."""
    status_color = C.GREEN if r.passed else C.RED
    status = "✓ PASS" if r.passed else "✗ FAIL"
    if r.error.startswith("SKIP"):
        status_color = C.YELLOW
        status = "⊘ SKIP"
    fmt_color = {"markdown": C.CYAN, "json": C.BLUE, "html": C.MAGENTA, "pdf": C.YELLOW}.get(r.case.fmt, C.GRAY)
    line = f"{status_color}{status:8}{C.RESET} {fmt_color}[{r.case.fmt:8}]{C.RESET} {r.case.name}"
    if r.passed and r.case.fmt != "pdf":
        line += f" {C.DIM}({r.sad_size}B, {r.sad_time_ms:.0f}+{r.sadc_time_ms:.0f}ms){C.RESET}"
    elif r.passed and r.case.fmt == "pdf":
        diff = abs(r.sad_size - r.sadc_size)
        line += f" {C.DIM}(sad={r.sad_size}B sadc={r.sadc_size}B Δ={diff}B){C.RESET}"
    if r.error and not r.error.startswith("SKIP"):
        line += f"\n  {C.RED}└─ {r.error}{C.RESET}"
    print(line)
    if verbose and r.passed:
        print(f"  {C.DIM}sad_hash={r.sad_hash} sadc_hash={r.sadc_hash}{C.RESET}")


def main():
    ap = argparse.ArgumentParser(description="اختبارات استخراج التوثيق المزدوج")
    ap.add_argument("--skip-pdf", action="store_true", help="تخطي اختبارات PDF (بطيئة وتحتاج Edge/Chrome)")
    ap.add_argument("--verbose", "-v", action="store_true", help="طباعة تفاصيل")
    ap.add_argument("--filter", default=None, help="تصفية بأسم جزئي")
    ap.add_argument("--keep-tmp", action="store_true", help="إبقاء _tmp بعد الانتهاء")
    args = ap.parse_args()

    # (AR) فحوصات أولية
    if not SAD.exists():
        print(f"{C.RED}❌ sad.exe غير موجود: {SAD}{C.RESET}")
        return 2
    if not SADC.exists():
        print(f"{C.RED}❌ sadc.exe غير موجود: {SADC}{C.RESET}")
        return 2

    # (AR) تنظيف وتجهيز _tmp
    if TMP.exists():
        shutil.rmtree(TMP)
    TMP.mkdir(parents=True, exist_ok=True)

    cases = discover_cases(args.skip_pdf, args.filter)
    print(f"{C.BOLD}{C.CYAN}═══════════════════════════════════════════════════════════════════{C.RESET}")
    print(f"{C.BOLD}اختبارات استخراج التوثيق المزدوج (sad ↔ sadc){C.RESET}")
    print(f"{C.DIM}عدد الحالات: {len(cases)} | sad: {SAD.name} | sadc: {SADC.name}{C.RESET}")
    print(f"{C.BOLD}{C.CYAN}═══════════════════════════════════════════════════════════════════{C.RESET}\n")

    results: List[TestResult] = []
    t_start = time.perf_counter()
    for i, c in enumerate(cases, 1):
        print(f"{C.DIM}[{i:3}/{len(cases)}]{C.RESET} ", end="")
        r = run_case(c)
        results.append(r)
        print_result(r, args.verbose)

    t_total = time.perf_counter() - t_start

    # (AR) ملخص
    passed = sum(1 for r in results if r.passed)
    failed = sum(1 for r in results if not r.passed and not r.error.startswith("SKIP"))
    skipped = sum(1 for r in results if r.error.startswith("SKIP"))
    total = len(results)

    print(f"\n{C.BOLD}{C.CYAN}═══════════════════════════════════════════════════════════════════{C.RESET}")
    print(f"{C.BOLD}الملخص:{C.RESET}")
    print(f"  {C.GREEN}✓ نجح:    {passed:3}/{total}{C.RESET}")
    if skipped:
        print(f"  {C.YELLOW}⊘ تخطى:   {skipped:3}/{total}{C.RESET}")
    if failed:
        print(f"  {C.RED}✗ فشل:    {failed:3}/{total}{C.RESET}")
    print(f"  {C.DIM}⏱  المدة:  {t_total:.2f}s{C.RESET}")

    # (AR) طبقة المخططات
    by_fmt = {}
    for r in results:
        by_fmt.setdefault(r.case.fmt, [0, 0])
        idx = 0 if r.passed else 1
        by_fmt[r.case.fmt][idx] += 1
    print(f"\n{C.BOLD}حسب الصيغة:{C.RESET}")
    for fmt, (p, f) in by_fmt.items():
        c = C.GREEN if f == 0 else C.YELLOW if p > 0 else C.RED
        print(f"  {c}{fmt:10} {p}/{p+f}{C.RESET}")

    if failed:
        print(f"\n{C.BOLD}{C.RED}الاختبارات الفاشلة:{C.RESET}")
        for r in results:
            if not r.passed and not r.error.startswith("SKIP"):
                print(f"  {C.RED}✗{C.RESET} {r.case.name}: {r.error}")

    # (AR) تنظيف
    if not args.keep_tmp and failed == 0:
        shutil.rmtree(TMP, ignore_errors=True)
    elif args.keep_tmp or failed > 0:
        print(f"\n{C.DIM}مخرجات الاختبار محفوظة في: {TMP}{C.RESET}")

    print(f"{C.BOLD}{C.CYAN}═══════════════════════════════════════════════════════════════════{C.RESET}")
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
