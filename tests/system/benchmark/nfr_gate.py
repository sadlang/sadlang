# -*- coding: utf-8 -*-
"""
بوّابة العتبات غير الوظيفية (NFR Gate) — نظام testing-system / TEST-005 AC-04
═══════════════════════════════════════════════════════════════════════════

(AR) الفكرة (ADR-004 + نقد Murat): «لا قيمة لاختبار أداء بلا عتبة رقمية
     تُفشِل عند التراجع». هذا السكربت:
       1. يقيس أزمنة تنفيذ برامج مرجعية بالمفسر (وسيط median لعدة تشغيلات —
          أمتن من المتوسط ضد القيم الشاذة).
       2. يقارنها بعتبات `nfr_thresholds.yaml` المجاورة: العتبة = خط أساس
          مقيس فعلياً × (1 + نسبة التسامح) — لا أرقام سحرية (CW-10).
       3. يخرج بكود 1 عند أي خرق → يفشل ctest (NFR_Gate).

     المعايرة: `python nfr_gate.py --calibrate` تقيس وتكتب خط الأساس الجديد
     في YAML (تُستخدم عمداً عند ترقية العتاد أو تحسين جوهري — قرار بشري،
     ليست خطوة CI).

(EN) NFR gate: measures median interpreter runtimes for reference programs and
     fails (exit 1) when any exceeds its calibrated threshold
     (baseline_ms × (1 + tolerance)). `--calibrate` re-measures and rewrites
     the baseline YAML — a deliberate human action, never a CI step.

الاستخدام / Usage:
    python tests/system/benchmark/nfr_gate.py            # الفحص (CI)
    python tests/system/benchmark/nfr_gate.py --calibrate
    python tests/system/benchmark/nfr_gate.py --interp build/bin/Debug/sad-run.exe
"""

import argparse
import statistics
import subprocess
import sys
import time
from pathlib import Path

# (AR) كونسول Windows قد يكون بترميز محلي (cp1255/cp1256) — نفرض UTF-8 للعربية
#      (نفس معالجة runner.py — errors='replace' يمنع الانهيار في أسوأ الأحوال)
# (EN) Force UTF-8 stdout for Arabic output on locale-encoded Windows consoles.
if hasattr(sys.stdout, "reconfigure"):
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")

# (AR) جذر المشروع: المجلد الذي يحوي tests/ (السكربت في tests/system/benchmark)
PROJECT_ROOT = Path(__file__).resolve().parents[3]
THRESHOLDS_FILE = Path(__file__).resolve().parent / "nfr_thresholds.yaml"

# (AR) البرامج المرجعية — من P0_smoke (مستقرة، تعمل في كل commit)
# (EN) Reference programs — from P0_smoke (stable, run on every commit)
BENCH_PROGRAMS = [
    "tests/behavior/P0_smoke/001_hello.ص",
    "tests/behavior/P0_smoke/002_arithmetic.ص",
    "tests/behavior/P0_smoke/004_functions.ص",
]

RUNS_PER_PROGRAM = 7   # (AR) عدد التشغيلات لكل برنامج (وسيط 7 يمتص الشواذ)
DEFAULT_TOLERANCE = 0.50  # (AR) تسامح افتراضي 50% فوق خط الأساس — يُضبط في YAML


def measure_median_ms(interp: Path, program: Path) -> float:
    """(AR) يقيس الوسيط بالمللي ثانية لتشغيل برنامج بالمفسر.
    (EN) Median wall-clock ms of running a program under the interpreter."""
    samples = []
    for _ in range(RUNS_PER_PROGRAM):
        start = time.perf_counter()
        proc = subprocess.run(
            [str(interp), str(program)],
            capture_output=True,
            timeout=60,
            cwd=str(PROJECT_ROOT),
        )
        elapsed_ms = (time.perf_counter() - start) * 1000.0
        if proc.returncode != 0:
            print(f"  ❌ فشل تنفيذ {program.name} (rc={proc.returncode}) — "
                  f"بوّابة NFR تتطلب برنامجاً أخضر أولاً")
            sys.exit(2)
        samples.append(elapsed_ms)
    return statistics.median(samples)


def load_thresholds() -> dict:
    """(AR) قراءة YAML بدون اعتماد خارجي (صيغة مسطّحة: مفتاح: قيمة)."""
    data = {}
    if not THRESHOLDS_FILE.exists():
        return data
    current = None
    for line in THRESHOLDS_FILE.read_text(encoding="utf-8").splitlines():
        stripped = line.split("#", 1)[0].rstrip()
        if not stripped:
            continue
        if not stripped.startswith(" ") and stripped.endswith(":"):
            current = stripped[:-1].strip()
            data[current] = {}
        elif current and ":" in stripped:
            key, val = stripped.split(":", 1)
            data[current][key.strip()] = float(val.strip())
    return data


def write_thresholds(measured: dict) -> None:
    lines = [
        "# عتبات NFR — خط أساس مقيس فعلياً (GR-01) — يُعاد توليده بـ --calibrate فقط",
        "# NFR thresholds — actually-measured baseline; regenerate via --calibrate only",
        f"# آخر معايرة / last calibration: {time.strftime('%Y-%m-%d %H:%M')}",
        f"# عدد التشغيلات لكل قياس / runs per sample: {RUNS_PER_PROGRAM} (median)",
        "",
    ]
    for name, ms in measured.items():
        lines.append(f"{name}:")
        lines.append(f"  baseline_ms: {ms:.1f}")
        lines.append(f"  tolerance: {DEFAULT_TOLERANCE}")
        lines.append("")
    THRESHOLDS_FILE.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description="بوّابة عتبات الأداء NFR")
    parser.add_argument("--interp", default="build/bin/Debug/sad-run.exe")
    parser.add_argument("--calibrate", action="store_true",
                        help="قياس وكتابة خط أساس جديد (قرار بشري — ليس CI)")
    args = parser.parse_args()

    interp = PROJECT_ROOT / args.interp
    if not interp.exists():
        print(f"⚠️ المفسر غير موجود: {interp} — تخطٍّ (ليس فشلاً)")
        return 0

    print("═══ بوّابة NFR — قياس الوسيط لكل برنامج مرجعي ═══")
    measured = {}
    for rel in BENCH_PROGRAMS:
        program = PROJECT_ROOT / rel
        if not program.exists():
            print(f"  ❌ برنامج مرجعي مفقود: {rel}")
            return 2
        ms = measure_median_ms(interp, program)
        measured[program.stem] = ms
        print(f"  📏 {program.name}: median = {ms:.1f}ms")

    if args.calibrate:
        write_thresholds(measured)
        print(f"✅ كُتب خط الأساس في {THRESHOLDS_FILE.name}")
        return 0

    thresholds = load_thresholds()
    if not thresholds:
        print("❌ لا ملف عتبات — شغّل --calibrate أولاً واعتمده في git")
        return 2

    failures = []
    for name, ms in measured.items():
        spec = thresholds.get(name)
        if not spec:
            print(f"  ⚠️ {name}: بلا عتبة مسجلة — أضفها بالمعايرة")
            continue
        limit = spec["baseline_ms"] * (1.0 + spec["tolerance"])
        verdict = "✅" if ms <= limit else "❌"
        print(f"  {verdict} {name}: {ms:.1f}ms ≤ حد {limit:.1f}ms "
              f"(أساس {spec['baseline_ms']:.1f} + {spec['tolerance']*100:.0f}%)")
        if ms > limit:
            failures.append(name)

    if failures:
        print(f"❌ بوّابة NFR فشلت — تراجع أداء في: {', '.join(failures)}")
        return 1
    print("✅ بوّابة NFR خضراء — لا تراجع أداء")
    return 0


if __name__ == "__main__":
    sys.exit(main())
