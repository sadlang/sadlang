#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: gen_grammar_lowers_to.py
الوصف: يشتقّ — بالدليل التجريبيّ لا بالادّعاء — أوپكودات SIR التي تخفض إليها كلُّ
       قاعدةٍ نحويّة، ويكتبها إلى language-truth/grammar/lowers_to.yaml.

(AR) المشكلة التي يحلّها: لا يوجد في مصدر الحقيقة ما يربط قاعدةً نحويّةً بما تخفض
     إليه، فيستحيل الحكم «هل تدعم الخلفيّةُ الأصليّة (بلا LLVM) هذه القاعدة؟» لأيّ
     قاعدةٍ على حدة. هذا الملفّ يجعل ذلك السؤال قابلًا للقياس.

(AR) المنهج — الاشتقاق بالتشغيل لا بالاستقراء اليدويّ:
       ① خطُّ الأساس: يُترجَم برنامجٌ أدنى (دالّة رئيسيّة فارغة) بـ«--أظهر-sir»،
          فتُجمَع أوپكودات السقالة التي يبثّها المولِّد لكلّ دالّةٍ مهما كانت.
       ② لكلّ قاعدة: تُترجَم عيّنةٌ من اختباراتها في tests/behavior/rules_matrix
          بـ«--أظهر-sir»، وتُجمَع أوپكوداتها.
       ③ lowers_to = اتّحادُ أوپكودات القاعدة **كاملًا** (سقالةً ومضمونًا)، لأنّ
          الحكمَ على الخلفيّة الأصليّة يلزمه كلُّ ما يحتاجه البرنامج فعلًا لا ما
          يميّز القاعدةَ فحسب؛ ويُدرَج beyond_baseline لنسبةِ ما تُضيفه القاعدة
          وحدَها إليها. طرحُ الأساس من الحكم يُخفي أنّ السقالةَ نفسَها قد لا تُخفَض.
     ما لم يُترجَم لا يُدّعى: القاعدة بلا اختباراتٍ ناجحةِ الترجمة تُوسَم
     «غير مشتقّة» صراحةً، ولا تُترك خانتها فارغةً موحيةً بالنجاح.

(AR) الحكم على الخلفيّة الأصليّة: القاعدة «مدعومة أصليًّا» على معماريّةٍ ما إن كان
     كلُّ أوپكودٍ في lowers_to لها مخفوضًا في تلك المعماريّة حسب
     language-truth/backend/sir_opcodes.yaml (حقل native_lowered).

الاستخدام:
    python scripts/codegen/gen_grammar_lowers_to.py --compiler build/bin/Release/sad-build.exe
    python scripts/codegen/gen_grammar_lowers_to.py --check     # يتحقّق من التحديث (CI)
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import io
import os
import re
import subprocess
import sys
import tempfile
from collections import defaultdict
from pathlib import Path

try:
    import yaml
except ImportError:
    print("❌ مطلوب PyYAML: pip install pyyaml", file=sys.stderr)
    sys.exit(2)

ROOT = Path(__file__).resolve().parent.parent.parent
GRAMMAR_DIR = ROOT / "language-truth" / "grammar"
RULES_MATRIX = ROOT / "tests" / "behavior" / "rules_matrix"
CATALOG = ROOT / "language-truth" / "backend" / "sir_opcodes.yaml"
OUT = GRAMMAR_DIR / "lowers_to.yaml"
EMIT_SIR_FLAG = "--أظهر-sir"

# (AR) عدد الاختبارات المأخوذة لكلّ قاعدة — سقفٌ مُعلَن لا صامت
SAMPLES_PER_RULE = 3
COMPILE_TIMEOUT = 90

_RE_SIR_SECTION = re.compile(r"=+\s*SIR\s*=+(.*)", re.S)
# (AR) المنمنمة في موضع التعليمة: إمّا بعد «= » أو في أوّل السطر
_RE_ASSIGN = re.compile(r"^\s*%%\S+\s*=\s*([a-z][a-z_.0-9]*)")
_RE_PLAIN = re.compile(r"^\s*([a-z][a-z_.0-9]*)\b")
# (AR) كلماتٌ ليست أوپكودات (تسميات وبنى نصّيّة في العرض)
_NOT_OPCODES = {"define", "class", "label", "entry", "declare", "global", "string"}


def _read(p: Path) -> str:
    return io.open(p, encoding="utf-8-sig").read()


def load_catalog() -> tuple[dict, dict, list]:
    """(AR) يُرجع (منمنمة→اسم، اسم→معماريّات الخفض، قائمة المعماريّات)."""
    if not CATALOG.exists():
        raise SystemExit(f"❌ مفقود: {CATALOG} — شغّل gen_sir_opcodes_yaml.py أوّلًا")
    data = yaml.safe_load(_read(CATALOG)) or {}
    by_mn: dict[str, str] = {}
    native: dict[str, list] = {}
    archs: set = set()
    for e in data.get("opcodes", []) or []:
        name = e["name"]
        mn = (e.get("mnemonic") or "").strip()
        if mn:
            by_mn.setdefault(mn, name)
        native[name] = list(e.get("native_lowered") or [])
        archs.update(native[name])
    return by_mn, native, sorted(archs)


def emit_sir(compiler: Path, src: Path, tmp: Path) -> str | None:
    """(AR) يُرجع نصّ قسم SIR، أو None إن أخفقت الترجمة."""
    try:
        proc = subprocess.run(
            [str(compiler.resolve()), str(src.resolve()), EMIT_SIR_FLAG],
            capture_output=True, timeout=COMPILE_TIMEOUT, cwd=str(tmp))
    except (subprocess.TimeoutExpired, OSError):
        return None
    out = (proc.stdout or b"").decode("utf-8", "replace")
    m = _RE_SIR_SECTION.search(out)
    return m.group(1) if m else None


def mnemonics_of(sir_text: str, by_mn: dict) -> set[str]:
    """(AR) أسماء الأوپكودات الظاهرة في نصّ SIR (المعروفة في الكتالوج فقط)."""
    found: set[str] = set()
    for line in sir_text.splitlines():
        line = line.split(";", 1)[0]
        m = _RE_ASSIGN.match(line) or _RE_PLAIN.match(line)
        if not m:
            continue
        mn = m.group(1)
        if mn in _NOT_OPCODES:
            continue
        name = by_mn.get(mn)
        if name:
            found.add(name)
    return found


_RE_RULE_TAG = re.compile(r"^#\s*@rule:?\s+(.+)$")


def extract_rule_ids(filepath: Path) -> list[str]:
    """(AR) معرّفاتُ القواعد من وسم `@rule` داخل الملفّ — لا من اسم مجلّده.

    (AR) نظيرُ `_extract_rule_ids` في check_grammar_conformance.py عمدًا: الاستنتاجُ
         من بادئة المسار يُعمي عن `_interactions/` (مجلّداتُها مسمّاةٌ
         `adv.ffi_extern_block__adv.ffi_linkage` بلا بادئة `gr.`) — خُمسُ الاختبارات
         تقريبًا — فيُسجَّل «لا اختبارات» سببًا كاذبًا لقاعدةٍ لها اختباراتٌ مطلقة.
    """
    ids: list[str] = []
    try:
        for i, line in enumerate(io.open(filepath, encoding="utf-8-sig")):
            if i >= 30:
                break
            m = _RE_RULE_TAG.match(line.strip())
            if m:
                ids.extend(r.strip() for r in re.split(r"[,،]", m.group(1)) if r.strip())
    except OSError:
        pass
    return ids


def rule_samples() -> dict[str, list[Path]]:
    """(AR) عيّنة اختباراتٍ إيجابيّةٍ لكلّ قاعدة (basic ثمّ edge؛ لا negative)."""
    out: dict[str, list[Path]] = defaultdict(list)
    if not RULES_MATRIX.is_dir():
        return out
    for tf in sorted(RULES_MATRIX.rglob("*.ص")):
        parts = tf.relative_to(RULES_MATRIX).parts
        if "negative" in parts:
            continue
        for rid in extract_rule_ids(tf):
            out[rid].append(tf)
    for rid in out:
        basic = [p for p in out[rid] if "basic" in p.parts]
        rest = [p for p in out[rid] if "basic" not in p.parts]
        out[rid] = (basic + rest)[:SAMPLES_PER_RULE]
    return out


def load_rule_ids() -> list[str]:
    ids: list[str] = []
    for yf in sorted(GRAMMAR_DIR.glob("*.yaml")):
        if yf.name.startswith("_"):
            continue
        data = yaml.safe_load(_read(yf)) or {}
        for prod in data.get("productions", []) or []:
            if prod.get("id"):
                ids.append(prod["id"])
    return ids


def render(rows: list[dict], archs: list[str], baseline: set[str]) -> str:
    derived = [r for r in rows if r["status"] == "derived"]
    L: list[str] = []
    L.append("# ═══════════════════════════════════════════════════════════════════")
    L.append("# lowers_to.yaml — خفضُ كلّ قاعدةٍ نحويّةٍ إلى أوپكودات SIR")
    L.append("# ═══════════════════════════════════════════════════════════════════")
    L.append("# (AR) مُولَّد آليًّا بـscripts/codegen/gen_grammar_lowers_to.py — لا يُحرَّر يدويًّا.")
    L.append("#      الاشتقاق تجريبيّ: تُترجَم اختباراتُ القاعدة بـ«--أظهر-sir» فتُجمَع")
    L.append("#      أوپكوداتُها كاملةً في lowers_to (عليها يقوم الحكم الأصليّ)، ويُفرَز")
    L.append("#      beyond_baseline لما تُضيفه القاعدةُ فوق سقالةِ أيّ دالّة. وما لم")
    L.append("#      يُترجَم يُعلَن not_derived صراحةً — لا يُفترض نجاحه.")
    L.append("#")
    L.append("# (AR) native_ok: المعماريّات التي تخفض كلَّ أوپكودات القاعدة في الخلفيّة")
    L.append("#      الأصليّة بلا LLVM. native_missing: الأوپكودات المانعة لكلّ معماريّة.")
    L.append("# ═══════════════════════════════════════════════════════════════════")
    L.append("")
    L.append('version: "1.0"')
    L.append('description_ar: "ربط قواعد النحو بأوپكودات SIR — مُشتقّ بالتشغيل"')
    L.append('description_en: "Grammar rule to SIR opcode mapping — derived by execution"')
    L.append("")
    L.append("baseline_opcodes: [" + ", ".join(sorted(baseline)) + "]")
    L.append("")
    L.append("stats:")
    L.append(f"  rules_total: {len(rows)}")
    L.append(f"  derived: {len(derived)}")
    L.append(f"  not_derived: {len(rows) - len(derived)}")
    for a in archs:
        n = sum(1 for r in derived if a in r["native_ok"])
        L.append(f"  native_ok_{a}: {n}")
    L.append(f"  samples_per_rule: {SAMPLES_PER_RULE}")
    L.append("")
    L.append("rules:")
    for r in rows:
        L.append(f"  - id: {r['id']}")
        L.append(f"    status: {r['status']}")
        if r["status"] != "derived":
            L.append(f'    reason: "{r["reason"]}"')
            continue
        L.append("    lowers_to: [" + ", ".join(r["opcodes"]) + "]")
        L.append("    beyond_baseline: [" + ", ".join(r["beyond"]) + "]")
        L.append("    native_ok: [" + ", ".join(r["native_ok"]) + "]")
        for a in archs:
            miss = r["native_missing"].get(a) or []
            if miss:
                L.append(f"    native_missing_{a}: [" + ", ".join(miss) + "]")
        L.append("    evidence: [" + ", ".join(r["evidence"]) + "]")
    L.append("")
    return "\n".join(L)


def main() -> int:
    ap = argparse.ArgumentParser(description="مشتقّ خفض القواعد إلى أوپكودات SIR")
    ap.add_argument("--compiler", default="build/bin/Release/sad-build.exe",
                    help="مسار المترجم (sad-build)")
    ap.add_argument("--check", action="store_true",
                    help="تحقّق من تحديث الملفّ دون إعادة اشتقاقٍ كامل (CI)")
    ap.add_argument("--only", help="اشتقاق قاعدةٍ واحدةٍ فقط (للتجريب)")
    ap.add_argument("--recompute", action="store_true",
                    help="أعِد حسابَ الحكم الأصليّ من الكتالوج دون إعادة ترجمةٍ "
                         "(يُستعمل حين يتغيّر الكتالوج لا الاختبارات)")
    args = ap.parse_args()
    os.environ.setdefault("PYTHONUTF8", "1")
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    if args.check:
        if not OUT.exists():
            print(f"❌ مفقود: {OUT.name} — شغّل gen_grammar_lowers_to.py")
            return 1
        data = yaml.safe_load(_read(OUT)) or {}
        ids = set(load_rule_ids())
        have = {r["id"] for r in data.get("rules", []) or []}
        missing = sorted(ids - have)
        extra = sorted(have - ids)
        if missing or extra:
            for i in missing:
                print(f"  ✗ قاعدة بلا اشتقاق: {i}")
            for i in extra:
                print(f"  ✗ اشتقاق لقاعدةٍ غير موجودة: {i}")
            return 1
        print(f"✅ {OUT.name} يغطّي {len(ids)} قاعدة")
        return 0

    if args.recompute:
        if not OUT.exists():
            print(f"❌ مفقود: {OUT.name} — لا شيء يُعاد حسابه", file=sys.stderr)
            return 2
        _by_mn, native, archs = load_catalog()
        data = yaml.safe_load(_read(OUT)) or {}
        baseline = set(data.get("baseline_opcodes") or [])
        rows: list[dict] = []
        for r in data.get("rules", []) or []:
            if r.get("status") != "derived":
                rows.append({"id": r["id"], "status": r.get("status", "not_derived"),
                             "reason": r.get("reason", "")})
                continue
            ops = set(r.get("lowers_to") or [])
            miss = {a: sorted(o for o in ops if a not in native.get(o, [])) for a in archs}
            rows.append({
                "id": r["id"], "status": "derived",
                "opcodes": sorted(ops), "beyond": sorted(ops - baseline),
                "native_ok": [a for a in archs if not miss[a]],
                "native_missing": miss,
                "evidence": list(r.get("evidence") or []),
            })
        io.open(OUT, "w", encoding="utf-8", newline="\n").write(
            render(rows, archs, baseline))
        d = sum(1 for r in rows if r["status"] == "derived")
        for a in archs:
            n = sum(1 for r in rows if r["status"] == "derived" and a in r["native_ok"])
            print(f"  {a}: {n}/{d}")
        print(f"✅ أُعيد حسابُ {OUT.relative_to(ROOT)} من الكتالوج ({d} قاعدةً مُشتقّة)")
        return 0

    compiler = Path(args.compiler)
    if not compiler.exists():
        print(f"❌ المترجم غير موجود: {compiler}", file=sys.stderr)
        return 2

    by_mn, native, archs = load_catalog()
    samples = rule_samples()
    ids = load_rule_ids()
    if args.only:
        ids = [i for i in ids if i == args.only]

    with tempfile.TemporaryDirectory(prefix="sad_lowers_") as td:
        tmp = Path(td)
        base_src = tmp / "خط_الأساس.ص"
        io.open(base_src, "w", encoding="utf-8", newline="\n").write(
            "دالة رئيسية()\nنهاية\n")
        base_sir = emit_sir(compiler, base_src, tmp)
        if base_sir is None:
            print("❌ تعذّر اشتقاق خطّ الأساس — المترجم لا يبثّ SIR", file=sys.stderr)
            return 2
        baseline = mnemonics_of(base_sir, by_mn)
        print(f"▸ خطّ الأساس: {len(baseline)} أوپكودًا من السقالة")

        rows: list[dict] = []
        for n, rid in enumerate(ids, 1):
            tests = samples.get(rid) or []
            if not tests:
                rows.append({"id": rid, "status": "not_derived",
                             "reason": "لا اختبارات إيجابيّة في rules_matrix"})
                print(f"  [{n}/{len(ids)}] {rid} — لا اختبارات")
                continue
            ops: set[str] = set()
            ok_files: list[str] = []
            for tf in tests:
                sir = emit_sir(compiler, tf, tmp)
                if sir is None:
                    continue
                ops |= mnemonics_of(sir, by_mn)
                ok_files.append(tf.name)
            if not ok_files:
                rows.append({"id": rid, "status": "not_derived",
                             "reason": "أخفقت ترجمةُ كلّ العيّنات — القاعدة غير مدعومةٍ في المترجم"})
                print(f"  [{n}/{len(ids)}] {rid} — أخفقت الترجمة")
                continue
            miss = {a: sorted(o for o in ops if a not in native.get(o, [])) for a in archs}
            rows.append({
                "id": rid, "status": "derived",
                "opcodes": sorted(ops),
                "beyond": sorted(ops - baseline),
                "native_ok": [a for a in archs if not miss[a]],
                "native_missing": miss,
                "evidence": ok_files,
            })
            print(f"  [{n}/{len(ids)}] {rid} — {len(ops)} أوپكودًا "
                  f"(أصليّ: {', '.join(rows[-1]['native_ok']) or 'لا شيء'})")

    text = render(rows, archs, baseline)
    io.open(OUT, "w", encoding="utf-8", newline="\n").write(text)
    d = sum(1 for r in rows if r["status"] == "derived")
    print(f"\n✅ كُتب {OUT.relative_to(ROOT)} — {d}/{len(rows)} قاعدةً مُشتقّة")
    return 0


if __name__ == "__main__":
    sys.exit(main())
