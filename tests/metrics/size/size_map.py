# -*- coding: utf-8 -*-
"""
مِقياسُ الحجمِ الحتميّ (Deterministic Size Meter) — نظام tests/metrics/
═══════════════════════════════════════════════════════════════════════════

(AR) الفكرة: الزمنُ الجداريُّ يقيسُ حِملَ المُشغِّلِ بقدرِ ما يقيسُ الكود، فلا
     يحتملُ تمييزًا دقيقًا. أمّا **الحجمُ والرموزُ المجرورة** فحتميّان: نفسُ
     المصدرِ ⇒ نفسُ الرقمِ بايتًا ببايت، على أيِّ آلةٍ وفي أيِّ ساعة. فهما
     الأساسُ الصالحُ لحارسٍ يحمرُّ على وحدةٍ واحدةٍ من فرق.

     تقيسُ هذه الأداةُ لكلِّ برنامجٍ في المجموعةِ المرجعيّة:
       • **الرموزَ الخارجيّةَ المجرورة** — سطورُ `declare` في تمثيل LLVM.
         هذا هو المقياسُ الذي يكشفُ جرَّ `libm` وأخواتِه: مدمجٌ واحدٌ يُضيفُ
         `floor` فيجرُّ مكتبةً كاملةً إلى ثنائيٍّ لا يحتاجُها.
       • **حجمَ التمثيلِ الوسيط** — عددَ سطورِ IR المولّدة.
       • **حجمَ ELF الأصليّ بالبايت** — من كاتبِ ELF الخاصِّ بنا لا من رابطِ
         النظام، فالبايتاتُ بايتاتُنا وحدَنا.
       • **حالةَ الخلفيّةِ الأصليّة** لكلِّ برنامج — وانتقالُها من «مرفوض» إلى
         «مقبول» تقدُّمٌ مقيس، وانتقالُها العكسيُّ انحدارٌ يُفشِلُ البوّابة.

     🔑 **تثبيتُ الهدف:** كلُّ ترجمةٍ تمرُّ بـ`--هدف=x86_64-unknown-linux-gnu`
     مهما كان المضيف. فبلا تثبيتٍ يتغيّرُ `target triple` و`datalayout` بين
     العدّائين، فيحمرُّ الحارسُ على اختلافِ الآلةِ لا على اختلافِ الكود — وهي
     العلّةُ نفسُها التي أصابت بوّابةَ NFR حين قارنت زمنًا مطلقًا بأساسٍ
     عُويِرَ على جهازٍ آخر. **وقد قِيسَ الأثر:** مجموعةُ الرموزِ المجرورةِ
     خرجت متطابقةً بينَ ثالوثِ ويندوزَ وثالوثِ لينكسَ على المضيفِ نفسِه.

     ⚠️ ما لا يُقاسُ هنا عمدًا: حجمُ الثنائيِّ النهائيِّ بعدَ رابطِ النظام.
     ذاك يتبعُ إصدارَ الرابطِ ومكتباتِ المضيف، فحارسُه يقيسُ بيئةَ العدّاءِ
     لا لغتَنا. حين يلزمُ رقمُه فمكانُه لقطةٌ مؤرَّخةٌ على جهازٍ ثابت.

(EN) Deterministic size meter. For each reference program it records the
     dragged external symbols (LLVM `declare` lines — the metric that exposes
     a libm drag), the emitted IR line count, the native-backend ELF byte size
     (our own writer, not the system linker), and the native backend's
     accept/reject status per program. Every compile pins the target triple so
     the numbers describe the code, never the runner. Final linked binary size
     is deliberately out of scope: it tracks the host toolchain, not us.

الاستخدام / Usage:
    python tests/metrics/size/size_map.py --check  --compiler <sad-build>
    python tests/metrics/size/size_map.py --update --compiler <sad-build>
    python tests/metrics/size/size_map.py --self-test
"""

import argparse
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

import yaml

for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8", errors="replace")
    except (AttributeError, ValueError):
        pass

HERE = Path(__file__).resolve().parent
PROJECT_ROOT = HERE.parents[2]
LEDGER_DIR = HERE / "ledger"
LEDGER_FILE = LEDGER_DIR / "size_latest.yaml"
HISTORY_FILE = LEDGER_DIR / "history.tsv"

# (AR) العَلَمانِ القانونيّان من مصدر الحقيقة (cli_flags.yaml).
FLAG_EMIT_LLVM = "--أظهر-llvm"
FLAG_NATIVE = "--خلفية-أصلية"
FLAG_TARGET = "--هدف"

# (AR) 🔑 الهدفُ المثبَّت: يُنتِزَعُ اختلافُ المضيفِ من القياس. لا يُغيَّرُ إلّا
#      بإعادةِ معايرةٍ كاملةٍ للسِّجلّ، لأنّ تغييرَه يُحرّكُ كلَّ رقمٍ فيه.
PINNED_TARGET = FLAG_TARGET + "=x86_64-unknown-linux-gnu"

# (AR) المجموعةُ المرجعيّة: مجلّداتٌ كاملةٌ لا قائمةَ أسماءٍ منثورة — فبرنامجٌ
#      جديدٌ يُضافُ إلى المجلّدِ يدخلُ القياسَ تلقائيًّا، ولا يبقى خارجَه صامتًا.
CORPUS_DIRS = [
    "compiler/benchmarks",
    "tests/behavior/P0_smoke",
]

# (AR) سطورُ رأسِ IR التي تصفُ الهدفَ لا الكود — تُستبعَدُ من العدّ.
IR_HEADER_RE = re.compile(r"^(; ModuleID|source_filename|target datalayout|target triple)")
DECLARE_RE = re.compile(r"^declare\s+[^@]*@([A-Za-z0-9_.$]+)")
DEFINE_RE = re.compile(r"^define\s")
GLOBAL_RE = re.compile(r"^@[A-Za-z0-9_.$]+\s*=")

COMPILE_TIMEOUT = 180


def _run(cmd, cwd):
    """(AR) يُشغّلُ المترجمَ ويُرجعُ (رمزُ الخروج). ضوضاءُ DEBUG تُبتلَع."""
    try:
        proc = subprocess.run(
            cmd,
            cwd=str(cwd),
            capture_output=True,
            timeout=COMPILE_TIMEOUT,
        )
        return proc.returncode
    except subprocess.TimeoutExpired:
        return -1


def corpus():
    """(AR) برامجُ المجموعةِ مرتّبةً بمسارٍ نسبيٍّ — الترتيبُ ثابتٌ فالقياسُ حتميّ."""
    found = []
    for rel in CORPUS_DIRS:
        base = PROJECT_ROOT / rel
        if not base.exists():
            continue
        for path in sorted(base.rglob("*.ص")):
            found.append(path.relative_to(PROJECT_ROOT).as_posix())
    return found


def measure_program(compiler, rel, workdir):
    """(AR) يقيسُ برنامجًا واحدًا. يرجعُ (قياس، مشكلة-أو-None)."""
    src = PROJECT_ROOT / rel
    stem = "m%04d" % (abs(hash(rel)) % 10000)
    ll_out = workdir / (stem + ".ll")
    elf_out = workdir / (stem + ".elf")

    # (AR) 🔑 مسارُ LLVM — حالةٌ تُسجَّلُ لا عطبٌ يُوقِفُ القياس. فأوّلُ تشغيلٍ
    #      كشفَ أنّ ٣ من ١٠ برامجَ في `compiler/benchmarks/` لا تُترجَمُ أصلًا
    #      (SYN003 على `[جديد ...]`, وSYN014 على `امسك (…)`). لو أخفقت الأداةُ
    #      هناك لبقيت حمراءَ أبدًا ثمّ عُطِّلت؛ ولو ابتلعت الإخفاقَ صامتةً
    #      لقاست سبعةَ برامجَ وزعمت أنّها قاست عشرة. فالصوابُ: تسجيلُ الحالةِ
    #      عددًا مرئيًّا في السِّجلّ، وإفشالُ البوّابةِ عند أيِّ انتقالٍ فيها.
    code = _run(
        [str(compiler), str(src), FLAG_EMIT_LLVM, PINNED_TARGET, "-o", str(ll_out)],
        PROJECT_ROOT,
    )
    if code != 0 or not ll_out.exists():
        if ll_out.exists():
            ll_out.unlink()
        return (
            {
                "llvm_status": "مرفوض",
                "ir_lines": 0,
                "ir_defines": 0,
                "ir_globals": 0,
                "external_symbols": [],
                "native_status": "غير مقيس",
                "native_elf_bytes": 0,
            },
            None,
        )

    text = ll_out.read_text(encoding="utf-8", errors="replace")
    symbols = set()
    ir_lines = 0
    defines = 0
    globals_ = 0
    for line in text.splitlines():
        if not line.strip():
            continue
        if IR_HEADER_RE.match(line):
            continue
        ir_lines += 1
        match = DECLARE_RE.match(line)
        if match:
            symbols.add(match.group(1))
        elif DEFINE_RE.match(line):
            defines += 1
        elif GLOBAL_RE.match(line):
            globals_ += 1

    # (AR) الخلفيّةُ الأصليّة — رفضُها حالةٌ مسجّلةٌ لا عطب. اتّساعُ ما تقبلُه
    #      تقدُّمٌ مقيس؛ وضيقُه انحدارٌ يُفشِلُ البوّابةَ عبرَ مقارنةِ الحالة.
    native_code = _run(
        [str(compiler), str(src), FLAG_NATIVE, PINNED_TARGET, "-o", str(elf_out)],
        PROJECT_ROOT,
    )
    if native_code == 0 and elf_out.exists():
        native_status = "مقبول"
        native_bytes = elf_out.stat().st_size
    else:
        native_status = "مرفوض"
        native_bytes = 0

    for leftover in (ll_out, elf_out):
        if leftover.exists():
            leftover.unlink()

    return (
        {
            "llvm_status": "مقبول",
            "ir_lines": ir_lines,
            "ir_defines": defines,
            "ir_globals": globals_,
            "external_symbols": sorted(symbols),
            "native_status": native_status,
            "native_elf_bytes": native_bytes,
        },
        None,
    )


def measure_all(compiler):
    programs = corpus()
    if not programs:
        return None, ["المجموعةُ المرجعيّةُ فارغة — لا برنامجَ يُقاس"]
    facts = {"target": PINNED_TARGET, "programs": {}}
    problems = []
    workdir = Path(tempfile.mkdtemp(prefix="sad_size_"))
    try:
        for rel in programs:
            measured, problem = measure_program(compiler, rel, workdir)
            if problem:
                problems.append(problem)
                continue
            facts["programs"][rel] = measured
    finally:
        shutil.rmtree(workdir, ignore_errors=True)

    accepted = sum(
        1 for p in facts["programs"].values() if p["native_status"] == "مقبول"
    )
    llvm_ok = sum(1 for p in facts["programs"].values() if p["llvm_status"] == "مقبول")
    facts["totals"] = {
        "programs": len(facts["programs"]),
        "llvm_accepted": llvm_ok,
        "llvm_rejected": len(facts["programs"]) - llvm_ok,
        "native_accepted": accepted,
        "native_rejected": len(facts["programs"]) - accepted,
        "ir_lines": sum(p["ir_lines"] for p in facts["programs"].values()),
        "native_elf_bytes": sum(p["native_elf_bytes"] for p in facts["programs"].values()),
        "distinct_external_symbols": len(
            {s for p in facts["programs"].values() for s in p["external_symbols"]}
        ),
    }
    return facts, problems


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الحكمُ الخالص — بلا دخل/خرج، فيُبذَرُ في الاختبارِ الذاتيّ.
# ═══════════════════════════════════════════════════════════════════════════
NUMERIC_KEYS = ("ir_lines", "ir_defines", "ir_globals", "native_elf_bytes")


def drift_verdict(committed, derived):
    """(AR) خلافاتُ الحجمِ والرموزِ بين المُودَعِ والمقيس. فارغةٌ ⇒ لا انحدار."""
    drift = []
    old = (committed or {}).get("programs", {})
    new = (derived or {}).get("programs", {})

    old_target = (committed or {}).get("target")
    new_target = (derived or {}).get("target")
    if old_target != new_target:
        drift.append("الهدفُ المثبَّتُ تغيّر: %s ← %s" % (old_target, new_target))

    for rel in sorted(set(old) | set(new)):
        before = old.get(rel)
        after = new.get(rel)
        if before is None:
            drift.append("برنامجٌ جديدٌ غيرُ مُودَع: %s" % rel)
            continue
        if after is None:
            drift.append("برنامجٌ مُودَعٌ لم يُقَس: %s" % rel)
            continue
        for key in NUMERIC_KEYS:
            b, a = before.get(key), after.get(key)
            if b != a:
                drift.append("%s · %s: %s ← %s (%+d)" % (rel, key, b, a, (a or 0) - (b or 0)))
        if before.get("llvm_status") != after.get("llvm_status"):
            drift.append(
                "%s · حالةُ ترجمةِ IR: %s ← %s"
                % (rel, before.get("llvm_status"), after.get("llvm_status"))
            )
        if before.get("native_status") != after.get("native_status"):
            drift.append(
                "%s · حالةُ الخلفيّةِ الأصليّة: %s ← %s"
                % (rel, before.get("native_status"), after.get("native_status"))
            )
        b_syms = set(before.get("external_symbols") or [])
        a_syms = set(after.get("external_symbols") or [])
        for sym in sorted(a_syms - b_syms):
            drift.append("%s · 🔴 رمزٌ خارجيٌّ جُرَّ حديثًا: %s" % (rel, sym))
        for sym in sorted(b_syms - a_syms):
            drift.append("%s · 🟢 رمزٌ خارجيٌّ لم يعُد يُجَرّ: %s" % (rel, sym))
    return drift


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الإيداع
# ═══════════════════════════════════════════════════════════════════════════
def write_ledger(facts, stamp):
    LEDGER_DIR.mkdir(parents=True, exist_ok=True)
    header = (
        "# سِجلُّ الحجمِ الحتميِّ للغة ص — مقيسٌ لا مُصرَّح\n"
        "# ⚠️ لا يُحرَّرُ باليد: python tests/metrics/size/size_map.py --update --compiler <sad-build>\n"
        "# الهدفُ مثبَّتٌ عمدًا فلا يقيسُ الرقمُ آلةَ العدّاء\n"
        "# آخرُ قياس / last measurement: %s\n" % stamp
    )
    # (AR) سطرُ نهايةٍ LF صراحةً — انظر التعليلَ في مِقياسِ السطح.
    LEDGER_FILE.write_text(
        header + yaml.safe_dump(facts, allow_unicode=True, sort_keys=False, default_flow_style=False),
        encoding="utf-8",
        newline="\n",
    )

    t = facts["totals"]
    row = [
        stamp,
        str(t["programs"]),
        str(t["llvm_accepted"]),
        str(t["llvm_rejected"]),
        str(t["native_accepted"]),
        str(t["native_rejected"]),
        str(t["ir_lines"]),
        str(t["native_elf_bytes"]),
        str(t["distinct_external_symbols"]),
    ]
    if not HISTORY_FILE.exists():
        HISTORY_FILE.write_text(
            "\t".join(
                [
                    "date",
                    "programs",
                    "llvm_accepted",
                    "llvm_rejected",
                    "native_accepted",
                    "native_rejected",
                    "ir_lines",
                    "native_elf_bytes",
                    "distinct_external_symbols",
                ]
            )
            + "\n",
            encoding="utf-8",
            newline="\n",
        )
    line = "\t".join(row)
    existing = HISTORY_FILE.read_text(encoding="utf-8").splitlines()
    if existing and existing[-1] == line:
        return
    with open(HISTORY_FILE, "a", encoding="utf-8", newline="\n") as fh:
        fh.write(line + "\n")


# ═══════════════════════════════════════════════════════════════════════════
# (AR) الاختبارُ الذاتيّ — بذورٌ متعاكسة. ⚠️ لا يحتاجُ مترجمًا مبنيًّا: يُغذّي
#      الحكمَ الخالصَ بقياساتٍ مُصطنَعة. فبوّابةٌ تحتاجُ ترجمةً كي تُبرهِنَ عضّتَها
#      لا تُبرهِنُها على عدّاءٍ بلا مترجم.
# ═══════════════════════════════════════════════════════════════════════════
def self_test():
    def facts(progs, target=PINNED_TARGET):
        return {"target": target, "programs": progs}

    base_prog = {
        "llvm_status": "مقبول",
        "ir_lines": 100,
        "ir_defines": 4,
        "ir_globals": 2,
        "external_symbols": ["malloc", "printf"],
        "native_status": "مقبول",
        "native_elf_bytes": 512,
    }

    def mutate(**kw):
        clone = dict(base_prog)
        clone.update(kw)
        return facts({"a.ص": clone})

    base = facts({"a.ص": base_prog})
    cases = [
        ("تطابقٌ تامّ ⇒ سكوت", base, base, False),
        ("سطورُ IR نمت ⇒ إخفاق", base, mutate(ir_lines=101), True),
        ("سطورُ IR نقصت ⇒ إخفاق", base, mutate(ir_lines=99), True),
        ("حجمُ ELF نما ⇒ إخفاق", base, mutate(native_elf_bytes=520), True),
        (
            "🔴 رمزٌ خارجيٌّ جديد (جرُّ libm) ⇒ إخفاق",
            base,
            mutate(external_symbols=["floor", "malloc", "printf"]),
            True,
        ),
        (
            "🟢 رمزٌ خارجيٌّ زال ⇒ إخفاق (تحسُّنٌ يُودَعُ لا يُبتلَع)",
            base,
            mutate(external_symbols=["malloc"]),
            True,
        ),
        (
            "انحدارُ ترجمةِ IR مقبول⇒مرفوض ⇒ إخفاق",
            base,
            mutate(llvm_status="مرفوض", ir_lines=0, ir_defines=0, ir_globals=0,
                   external_symbols=[], native_status="غير مقيس", native_elf_bytes=0),
            True,
        ),
        (
            "انحدارُ الخلفيّةِ الأصليّة مقبول⇒مرفوض ⇒ إخفاق",
            base,
            mutate(native_status="مرفوض", native_elf_bytes=0),
            True,
        ),
        ("برنامجٌ جديدٌ في المجموعة ⇒ إخفاق", base, facts({"a.ص": base_prog, "b.ص": base_prog}), True),
        ("برنامجٌ اختفى ⇒ إخفاق", facts({"a.ص": base_prog, "b.ص": base_prog}), base, True),
        (
            "الهدفُ المثبَّتُ تغيّر ⇒ إخفاق (وإلّا قِيست الآلةُ لا الكود)",
            base,
            facts({"a.ص": base_prog}, target="--هدف=aarch64-unknown-linux-gnu"),
            True,
        ),
    ]

    ok = True
    for name, committed, derived, expect_drift in cases:
        drift = drift_verdict(committed, derived)
        got = bool(drift)
        if got != expect_drift:
            ok = False
        print("  %s %s — خلافات: %d" % ("✅" if got == expect_drift else "❌", name, len(drift)))

    # (AR) بذرةٌ على تثبيتِ الهدف: العَلَمُ يجبُ أن يبقى مثبَّتًا في الشيفرةِ نفسِها.
    if not PINNED_TARGET.startswith(FLAG_TARGET + "="):
        ok = False
        print("  ❌ الهدفُ غيرُ مثبَّتٍ — القياسُ سيتبعُ المضيف")
    else:
        print("  ✅ الهدفُ مثبَّتٌ في القياس: %s" % PINNED_TARGET)

    if not ok:
        print("❌ الاختبارُ الذاتيُّ لمِقياسِ الحجمِ أخفق")
        return 1
    print("✅ الاختبارُ الذاتيُّ لمِقياسِ الحجمِ اجتاز")
    return 0


def _stamp():
    try:
        out = subprocess.run(
            ["git", "log", "-1", "--format=%cs"],
            cwd=str(PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=10,
        )
        if out.returncode == 0 and out.stdout.strip():
            return out.stdout.strip()
    except (OSError, subprocess.SubprocessError):
        pass
    return "unknown"


def main():
    parser = argparse.ArgumentParser(description="مِقياسُ الحجمِ الحتميِّ للغة ص")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("--check", action="store_true", help="مقارنةُ المقيسِ بالمُودَع (CI)")
    group.add_argument("--update", action="store_true", help="إعادةُ القياسِ والإيداع")
    group.add_argument("--self-test", action="store_true", help="بذورُ الحكمِ المتعاكسة")
    parser.add_argument("--compiler", help="مسارُ sad-build")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    if not args.compiler or not Path(args.compiler).exists():
        print("❌ لا مترجمَ — مرِّر --compiler <sad-build>")
        return 1

    print("═══ مِقياسُ الحجمِ الحتميّ — الهدفُ مثبَّت: %s ═══" % PINNED_TARGET)
    facts, problems = measure_all(Path(args.compiler))
    if problems:
        print("❌ إخفاقُ قياس — %d مشكلة:" % len(problems))
        for p in problems:
            print("   • %s" % p)
        return 1

    t = facts["totals"]
    print(
        "  📏 %d برنامجًا · IR %d سطرًا · ELF %d بايت · %d رمزًا خارجيًّا متمايزًا"
        % (t["programs"], t["ir_lines"], t["native_elf_bytes"], t["distinct_external_symbols"])
    )
    print(
        "  ⚙️ ترجمةُ IR: %d مقبولًا · %d مرفوضًا   |   الخلفيّةُ الأصليّة: %d مقبولًا · %d مرفوضًا"
        % (t["llvm_accepted"], t["llvm_rejected"], t["native_accepted"], t["native_rejected"])
    )

    stamp = _stamp()
    if args.update:
        write_ledger(facts, stamp)
        print("  💾 أُودِعَ السِّجلّ: %s" % LEDGER_FILE)
        return 0

    if not LEDGER_FILE.exists():
        print("❌ لا سِجلَّ مُودَعًا — شغّل --update أوّلًا")
        return 1
    with open(LEDGER_FILE, encoding="utf-8") as fh:
        committed = yaml.safe_load(fh)
    drift = drift_verdict(committed, facts)
    if drift:
        print("❌ انحرافٌ عن السِّجلِّ المُودَع — %d خلافًا:" % len(drift))
        for d in drift:
            print("   • %s" % d)
        print("   ↳ إن كان التغييرُ مقصودًا: --update ثمّ أودِع السِّجلَّ مع التغيير")
        return 1
    print("✅ لا انحراف — الحجمُ والرموزُ تطابقُ السِّجلَّ المُودَع")
    return 0


if __name__ == "__main__":
    sys.exit(main())
