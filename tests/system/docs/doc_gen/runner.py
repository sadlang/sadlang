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
PROJECT_ROOT = SCRIPT_DIR.parents[3]

# (AR) حلّ مسار الثنائيّ عابرًا للمنصّات: على ويندوز تكون الثنائيّات في
#      build/bin/Debug/x.exe (مولّد متعدّد التهيئات)؛ على Linux/macOS في
#      build/bin/x بلا .exe وبلا مجلّد Debug (مولّد أحاديّ التهيئة).
# (EN) Cross-platform binary resolution (multi-config Windows vs single-config Unix).
def _resolve_binary(name: str) -> Path:
    bindir = PROJECT_ROOT / "build" / "bin"
    candidates = [
        bindir / "Debug" / f"{name}.exe",
        bindir / "Release" / f"{name}.exe",
        bindir / f"{name}.exe",
        bindir / name,
    ]
    for c in candidates:
        if c.exists():
            return c
    return candidates[0]

SAD = _resolve_binary("sad-run")
SADC = _resolve_binary("sad-build")
# (AR) المترجم اختياريّ: حين لا يُبنى (LLVM معطّل، مثل Windows CI) ننتقل إلى
#      وضع المفسّر-فقط (نتحقّق أنّ توليد sad للتوثيق يعمل دون مقارنة sadc).
# (EN) Compiler optional: when not built (LLVM off, e.g. Windows CI), fall back to
#      interpreter-only (verify sad doc-gen works without the sadc comparison).
SADC_AVAILABLE = SADC.exists()
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
        cmd.append(f"--وثّق-مشروع={case.input_path}")
    cmd.append(f"--صيغة-التوثيق={case.fmt}")
    cmd.append(f"--وثّق-إلى={out_path}")
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


# ═══════════════════════════════════════════════════════════════════════════════════
# (AR) م٢ (الفجوة ٤): التحقق من عقد JSON الوسيط — الملف المعياري للعقد هو
#      schema/docs_json.schema.json (JSON Schema draft-07). المدقق أدناه نسخة
#      مصغّرة مكتفية ذاتياً (بلا اعتماد jsonschema) تعكس الـschema نفسه؛ عند
#      تغيير العقد يُحدَّث الملفان معاً.
# (EN) M2 (gap 4): intermediate docs-JSON contract validation — the normative
#      contract is schema/docs_json.schema.json (draft-07). The validator below
#      is a self-contained mirror (no jsonschema dependency); update both
#      together on any contract change.
# ═══════════════════════════════════════════════════════════════════════════════════
DOCS_JSON_SCHEMA = SCRIPT_DIR / "schema" / "docs_json.schema.json"
_DECL_KINDS = {"function", "class", "enum", "struct", "trait", "constant", "variable", "namespace"}
_DECL_STR_FIELDS = ("summary", "description", "returns", "since", "version",
                    "author", "deprecated", "complexity", "threadSafety", "license")


def _validate_declaration(decl, path: str, errs: List[str]) -> None:
    """(AR) يتحقق من تصريح واحد وفق العقد (يتكرر داخل members للفضاءات)."""
    if not isinstance(decl, dict):
        errs.append(f"{path}: ليس كائناً")
        return
    kind = decl.get("kind")
    if kind not in _DECL_KINDS:
        errs.append(f"{path}.kind: قيمة غير معروفة {kind!r}")
        return
    if not isinstance(decl.get("name"), str):
        errs.append(f"{path}.name: مفقود أو ليس نصاً")
    if not isinstance(decl.get("summary"), str):
        errs.append(f"{path}.summary: مفقود أو ليس نصاً")
    if kind == "namespace":
        members = decl.get("members")
        if not isinstance(members, list):
            errs.append(f"{path}.members: مفقود أو ليس مصفوفة")
        else:
            for i, m in enumerate(members):
                _validate_declaration(m, f"{path}.members[{i}]", errs)
        extra = set(decl) - {"kind", "name", "summary", "members"}
        if extra:
            errs.append(f"{path}: حقول خارج العقد {sorted(extra)}")
        return
    for f in _DECL_STR_FIELDS:
        if not isinstance(decl.get(f), str):
            errs.append(f"{path}.{f}: مفقود أو ليس نصاً")
    if not isinstance(decl.get("experimental"), bool):
        errs.append(f"{path}.experimental: مفقود أو ليس منطقياً")
    params = decl.get("params")
    if not isinstance(params, list):
        errs.append(f"{path}.params: مفقود أو ليس مصفوفة")
    else:
        for i, p in enumerate(params):
            if not isinstance(p, dict) or set(p) != {"name", "description"} \
               or not isinstance(p.get("name"), str) or not isinstance(p.get("description"), str):
                errs.append(f"{path}.params[{i}]: يجب أن يكون {{name, description}} نصيين")
    extra = set(decl) - ({"kind", "name", "params", "experimental"} | set(_DECL_STR_FIELDS))
    if extra:
        errs.append(f"{path}: حقول خارج العقد {sorted(extra)}")


def validate_docs_json(p: Path) -> str:
    """(AR) يتحقق من ملف JSON مولَّد وفق العقد. يعيد "" عند السلامة أو وصف الخلل."""
    import json
    try:
        obj = json.loads(normalized_bytes(p).decode("utf-8"))
    except Exception as e:
        return f"JSON غير قابل للتحليل: {e}"
    errs: List[str] = []
    if not isinstance(obj, dict):
        return "الجذر ليس كائناً"
    if not isinstance(obj.get("file"), str):
        errs.append("file: مفقود أو ليس نصاً")
    decls = obj.get("declarations")
    if not isinstance(decls, list):
        errs.append("declarations: مفقود أو ليس مصفوفة")
    else:
        for i, d in enumerate(decls):
            _validate_declaration(d, f"declarations[{i}]", errs)
    extra = set(obj) - {"file", "declarations"}
    if extra:
        errs.append(f"الجذر: حقول خارج العقد {sorted(extra)}")
    return "؛ ".join(errs)


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

    # (AR) م٢ (الفجوة ٤): بوابة عقد JSON — مخرج json للملف المفرد يجب أن يطابق
    #      schema/docs_json.schema.json (عقد DocsExtractor::extractJson).
    #      ⚠️ فجوة قائمة خارج نطاق م٢: وضع المشروع (--وثّق-مشروع) يتجاهل صيغة json
    #      ويُخرج Markdown دائماً — لا يستدعي extractJson أصلاً، فالعقد لا يشمله بعد.
    # (EN) M2 (gap 4): JSON contract gate — single-file json output must match
    #      schema/docs_json.schema.json (the DocsExtractor::extractJson contract).
    #      Known pre-existing gap outside M2 scope: project mode ignores the json
    #      format and always emits Markdown (never calls extractJson).
    if case.fmt == "json" and case.kind == "single":
        contract_err = validate_docs_json(sad_out)
        if contract_err:
            return TestResult(case=case, passed=False,
                              error=f"sad json contract: {contract_err}",
                              sad_time_ms=t1)

    # (AR) لا مترجم: نكتفي بنجاح توليد المفسّر (وضع مفسّر-فقط).
    # (EN) No compiler: accept interpreter doc-gen success (interpreter-only mode).
    if not SADC_AVAILABLE:
        return TestResult(case=case, passed=True,
                          error="SKIP sadc: المترجم غير مبنيّ — مفسّر فقط",
                          sad_size=sad_out.stat().st_size, sad_time_ms=t1)

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
                extra_args=["--استثنِ=internal.ص"],
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
    if not SADC_AVAILABLE:
        # (AR) المترجم غير مبنيّ (LLVM معطّل، مثل Windows CI): وضع المفسّر-فقط —
        #      نتحقّق فقط أنّ توليد sad للتوثيق يعمل (انظر run_case). لا نفشل.
        # (EN) Compiler not built (LLVM off, e.g. Windows CI): interpreter-only mode.
        print(f"{C.YELLOW}⚠️ sadc غير مبنيّ — وضع المفسّر-فقط (تحقّق توليد sad){C.RESET}")

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
