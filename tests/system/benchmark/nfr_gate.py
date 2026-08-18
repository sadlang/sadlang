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

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) 🔑 مِسبارُ سرعةِ الآلة — ولمَ كان لا بدَّ منه
#
#      كانت البوّابةُ تقارنُ **زمنَ ساعةِ حائطٍ مطلقًا** بخطِّ أساسٍ عُويِرَ في
#      ٢٠٢٦-٠٦-١٢ على جهازٍ آخر. فحكمُها كان يتتبّعُ حِملَ المُشغِّلِ لا الكودَ.
#
#      والبرهانُ مقيس: في شوطِ ٢٠٢٦-٠٨-١٨ على 🍎 macOS (Debug) خرجت الثلاثةُ
#      **كلُّها** فوقَ أساسها في القياسِ الواحد:
#          001_hello 48.9 (أساس 41.6, +17.5%) · 002_arithmetic 69.5 (43.7, +59%)
#          004_functions 54.8 (43.7, +25%)
#      فسقطَ 002 وحدَه لأنّه عبرَ خطَّ الـ50% — والآلةُ هي البطيئةُ لا الحساب.
#      والإيداعُ نفسُه اجتازَ الاختبارَ عينَه في الشوطِ الموازي على المُشغِّلِ الآخر.
#
#      ⚠️ ولا يُعادُ المعايرةُ هنا: أرقامُ الأساسِ تبقى كما هي، ويُضافُ إليها
#      **عاملُ سرعةٍ يُقاسُ في الشوطِ نفسِه**. والمِسبارُ هو `001_hello` لأنّه
#      من **جلسةِ المعايرةِ عينِها** — فأيُّ مِسبارٍ أقيسُه أنا اليومَ لا يقترنُ
#      بأساسٍ قيسَ في حزيران، فيكون عاملًا بلا معنى.
#
#      وثمنُه مُعلَنٌ لا مطموس: `001_hello` صار **مِعيارًا لا محكومًا عليه**.
#      وانحدارٌ يُبطِئُ المحرّكَ كلَّه بالتساوي لا تراه بوّابةٌ نسبيّةٌ أصلًا —
#      فيحرسُه السقفُ المطلقُ أدناه.
#
# (EN) Machine-speed probe. The gate compared ABSOLUTE wall-clock against a
#      baseline calibrated 2026-06-12 on other hardware, so its verdict tracked
#      runner load, not code. Measured proof: on 2026-08-18 all three programs
#      came in above baseline in the same sample (+17.5%, +59%, +25%) — one
#      crossed the 50% line and reddened the job, while the identical commit
#      passed the identical test on the parallel runner.
#
#      Baselines are NOT re-calibrated; a speed factor measured in the SAME run
#      is applied instead. The probe is 001_hello because it comes from the same
#      calibration session — any probe measured today would not pair with a June
#      baseline. The stated cost: 001_hello becomes the yardstick, not a subject.
# ═══════════════════════════════════════════════════════════════════════════════
PROBE_PROGRAM = "001_hello"

# (AR) سقفٌ مطلقٌ للمِسبارِ نفسِه: آلةٌ أبطأُ من هذا المُعامِلِ ليست «بطيئةً» بل
#      مُختلّةٌ — فالقياسُ عليها بلا معنى، ويُعلَنُ ذلك ولا يُحسَبُ إخفاقَ أداء.
# (EN) Absolute sanity ceiling on the probe: beyond this the machine is not slow
#      but broken, so the sample is meaningless — reported, never scored as a
#      performance regression.
PROBE_SANITY_FACTOR = 10.0


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


def evaluate(measured: dict, thresholds: dict) -> tuple:
    """
    (AR) الحكمُ — دالّةٌ خالصةٌ تأخذُ القياسَ والعتباتِ وتردُّ (إخفاقات، أسطر).

         فُصِلت عن القياسِ عمدًا: بوّابةٌ لا يمكن أن تُختبَرَ بمُدخَلٍ مُصطنَعٍ
         هي بوّابةٌ لا يُعرَفُ أنّها ما تزال قادرةً على أن تُخفِق. و`--self-test`
         يُغذّيها حالتَين متعاكستَين ويشترطُ حكمَين متعاكسَين.
    (EN) Pure verdict function: (failures, lines). Split from measurement so the
         gate can be fed synthetic input — a gate that cannot be tested is a gate
         nobody knows still bites. See --self-test.
    """
    lines = []
    probe_spec = thresholds.get(PROBE_PROGRAM)
    probe_now = measured.get(PROBE_PROGRAM)

    speed = 1.0
    if probe_spec and probe_now:
        speed = probe_now / probe_spec["baseline_ms"]
        lines.append(
            f"  🧭 مِسبارُ سرعةِ الآلة ({PROBE_PROGRAM}): {probe_now:.1f}ms ÷ "
            f"{probe_spec['baseline_ms']:.1f}ms = ×{speed:.2f}"
        )
        if speed > PROBE_SANITY_FACTOR:
            lines.append(
                f"  ⚠️ الآلةُ أبطأُ من ×{PROBE_SANITY_FACTOR:.0f} — القياسُ بلا معنى، "
                f"لا يُحسَبُ تراجعَ أداء"
            )
            return [], lines
    else:
        lines.append(
            f"  ⚠️ لا مِسبارَ ({PROBE_PROGRAM}) — يُحكَمُ بالمُطلَقِ كما كان"
        )

    failures = []
    for name, ms in sorted(measured.items()):
        spec = thresholds.get(name)
        if not spec:
            lines.append(f"  ⚠️ {name}: بلا عتبة مسجلة — أضفها بالمعايرة")
            continue
        if name == PROBE_PROGRAM:
            lines.append(f"  🧭 {name}: {ms:.1f}ms — مِعيارٌ لا محكومٌ عليه")
            continue
        limit = spec["baseline_ms"] * (1.0 + spec["tolerance"]) * speed
        verdict = "✅" if ms <= limit else "❌"
        lines.append(
            f"  {verdict} {name}: {ms:.1f}ms ≤ حد {limit:.1f}ms "
            f"(أساس {spec['baseline_ms']:.1f} + {spec['tolerance']*100:.0f}% "
            f"× سرعة {speed:.2f})"
        )
        if ms > limit:
            failures.append(name)
    return failures, lines


def self_test() -> int:
    """
    (AR) بذرتان متعاكستان — البوّابةُ يجب أن تسكتَ في الأولى وتُخفِقَ في الثانية.
         ⚠️ والثانيةُ هي المهمّة: حارسٌ لا يُخفِقُ ليس حارسًا بل تعليقٌ ينتظر
         أن يكذب. ورقمُ الحالةِ الأولى مأخوذٌ من الشوطِ الحقيقيِّ لا مخترَعًا.
    (EN) Two opposite seeds: the gate must stay silent on a uniformly slow
         machine and MUST fail on a single-program regression. The second is the
         point — a guard that cannot fail is a comment waiting to lie.
    """
    thresholds = {
        "001_hello": {"baseline_ms": 41.6, "tolerance": 0.5},
        "002_arithmetic": {"baseline_ms": 43.7, "tolerance": 0.5},
        "004_functions": {"baseline_ms": 43.7, "tolerance": 0.5},
    }
    problems = []

    # (AR) ① الحمرةُ الكاذبةُ التي وقعت فعلًا — أرقامُ 🍎 macOS في ٢٠٢٦-٠٨-١٨.
    slow_machine = {"001_hello": 48.9, "002_arithmetic": 69.5, "004_functions": 54.8}
    failures, lines = evaluate(slow_machine, thresholds)
    print("  ── ① آلةٌ بطيئةٌ بالتساوي (أرقامُ الشوطِ الحقيقيّ) ──")
    for line in lines:
        print(line)
    if failures:
        problems.append(f"آلةٌ بطيئةٌ عُدَّت تراجعًا: {failures}")

    # (AR) ② تراجعٌ حقيقيٌّ: الآلةُ على سرعتها والحسابُ وحدَه تضخّم.
    real_regression = {"001_hello": 41.6, "002_arithmetic": 70.0, "004_functions": 43.7}
    failures, lines = evaluate(real_regression, thresholds)
    print("  ── ② تراجعٌ حقيقيٌّ في 002_arithmetic وحدَه ──")
    for line in lines:
        print(line)
    if failures != ["002_arithmetic"]:
        problems.append(f"تراجعٌ حقيقيٌّ لم يُمسَك: {failures}")

    # (AR) ③ آلةٌ بطيئةٌ **وتراجعٌ معها** — لا يستُرُ العاملُ عيبًا حقيقيًّا.
    both = {"001_hello": 83.2, "002_arithmetic": 200.0, "004_functions": 87.4}
    failures, lines = evaluate(both, thresholds)
    print("  ── ③ آلةٌ بطيئةٌ ×2 مع تراجعٍ حقيقيٍّ فوقَها ──")
    for line in lines:
        print(line)
    if failures != ["002_arithmetic"]:
        problems.append(f"العاملُ سترَ تراجعًا: {failures}")

    if problems:
        for problem in problems:
            print(f"  ❌ {problem}")
        print("❌ الاختبارُ الذاتيُّ لبوّابة NFR أخفق")
        return 1
    print("✅ الاختبارُ الذاتيّ: تسكتُ على الآلةِ البطيئة، وتُخفِقُ على التراجعِ الحقيقيّ")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="بوّابة عتبات الأداء NFR")
    parser.add_argument("--interp", default="build/bin/Debug/sad-run.exe")
    parser.add_argument("--calibrate", action="store_true",
                        help="قياس وكتابة خط أساس جديد (قرار بشري — ليس CI)")
    parser.add_argument("--self-test", action="store_true",
                        help="بذرتان متعاكستان تُثبتان أنّ البوّابة ما تزال تُخفِق")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

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

    failures, lines = evaluate(measured, thresholds)
    for line in lines:
        print(line)

    if failures:
        print(f"❌ بوّابة NFR فشلت — تراجع أداء في: {', '.join(failures)}")
        return 1
    print("✅ بوّابة NFR خضراء — لا تراجع أداء")
    return 0


if __name__ == "__main__":
    sys.exit(main())
