#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
═══════════════════════════════════════════════════════════════════════════════════
ملف: gen_sir_opcodes_yaml.py
الوصف: يستخرج كتالوج أوپكودات SIR من مصدره الوحيد في المترجم ويكتبه إلى مصدر
       الحقيقة: language-truth/backend/sir_opcodes.yaml.

(AR) المشكلة التي يحلّها: كتالوج الأوپكودات كان محبوسًا في ترويسة C++، فلا يستطيع
     مصدر الحقيقة أن يشير إليه ولا أن يُتحقَّق من أيّ حقلٍ يذكر أوپكودًا. بلا كتالوجٍ
     في SoT، أيّ حقل «lowers_to» يبقى نصًّا حرًّا لا يحرسه شيء.

(AR) المصادر (تُقرأ ولا تُكتب):
       compiler/include/frontend/sir_types.h  ← enum class SIROpcode (الترتيب والأسماء)
       compiler/src/frontend/sir_types.cpp    ← sirOpcodeToString (المنمنمة النصّيّة)
     المخرَج مولَّد بالكامل — لا يُحرَّر يدويًّا.

(AR) التصنيف: الأوپكود «مخفوض أصليًّا» إن عالجته شيفرةُ الخفض في الخلفيّة الأصليّة
     (native/*_sir_lowering.h) — لا إن أعلنته جداول isel.yaml، فالجداول ما تزال
     بذرةَ م٠ (٣ أنماط) بينما الخفض الفعليّ مئاتُ الأسطر في C++. الحقلان يظهران
     منفصلَين ليبقى هذا الانجراف مرئيًّا لا مطموسًا.

الاستخدام:
    python scripts/codegen/gen_sir_opcodes_yaml.py            # يكتب الملفّ
    python scripts/codegen/gen_sir_opcodes_yaml.py --check    # يتحقّق فقط (CI)
═══════════════════════════════════════════════════════════════════════════════════
"""

import argparse
import io
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent.parent
HEADER = ROOT / "compiler" / "include" / "frontend" / "sir_types.h"
IMPL = ROOT / "compiler" / "src" / "frontend" / "sir_types.cpp"
BACKEND_DIR = ROOT / "language-truth" / "backend"
OUT = BACKEND_DIR / "sir_opcodes.yaml"
TARGETS = BACKEND_DIR / "targets.yaml"
# (AR) ملفٌّ مشتركٌ يحمل تحليلَ SIR محايدَ المعماريّة؛ يُمسَح مع مخفّضِ كلّ هدفٍ
#      كي لا تسقط معالجةٌ نُقلت إليه من الإحصاء صامتةً (م٤ ستنقل إليه المزيد).
SHARED_LOWERING = (ROOT / "compiler" / "include" / "backend" / "native" /
                   "sir_lowering_common.h")


def load_native_lowering() -> dict:
    """(AR) الخفض الأصليّ الفعليّ (بلا LLVM) لكلّ هدف — المسارات من مصدر الحقيقة
    ‏(backend/targets.yaml، الحقل lowering) لا مكتوبةً هنا. هدفٌ بلا حقل lowering
    ‏(مخطَّطٌ بعدُ) لا يدخل الإحصاء أصلًا."""
    out: dict = {}
    if not TARGETS.exists():
        raise SystemExit(f"❌ مفقود {TARGETS.relative_to(ROOT)} — قائمةُ الأهداف مصدرُ حقيقةٍ إلزاميّ")
    import yaml as _yaml
    data = _yaml.safe_load(_read(TARGETS)) or {}
    for t in (data.get("targets") or []):
        low = t.get("lowering")
        if not (t.get("id") and low):
            continue
        p = ROOT / low
        if not p.exists():
            raise SystemExit(f"❌ targets.yaml: {t['id']} يشير إلى مخفّضٍ غير موجود: {low}")
        out[t["id"]] = p
    if not out:
        raise SystemExit("❌ targets.yaml: لا هدفَ واحدٌ بحقل lowering — لا شيءَ يُقاس")
    return out

_RE_ENUM = re.compile(r"enum\s+class\s+SIROpcode[^{]*\{(.*?)\n\s*\};", re.S)
_RE_CASE = re.compile(r'case\s+SIROpcode::([A-Z][A-Z_0-9]*)\s*:\s*\n?\s*return\s+"([^"]*)"\s*;')
# (AR) عنوان القسم في الترويسة: «// N. اسم عربيّ / English (عدد)»
_RE_SECTION = re.compile(r"^\s*//\s*(\d+)\.\s*(.+?)\s*$", re.M)


def _read(p: Path) -> str:
    return io.open(p, encoding="utf-8-sig").read()


def extract_opcodes() -> list[dict]:
    """(AR) يُرجع قائمة الأوپكودات بترتيب التعداد مع منمنماتها وأقسامها."""
    header = _read(HEADER)
    m = _RE_ENUM.search(header)
    if not m:
        raise SystemExit("❌ لم يُعثر على enum class SIROpcode في " + str(HEADER))
    body = m.group(1)

    # (AR) القسم الحاليّ يُشتقّ من آخر عنوان «N. …» قبل الاسم
    section = "?"
    entries: list[dict] = []
    for line in body.splitlines():
        sec = _RE_SECTION.match(line)
        if sec:
            section = sec.group(2).strip()
            continue
        nm = re.match(r"\s*([A-Z][A-Z_0-9]*)\s*(?:=[^,]*)?,", line)
        if nm:
            entries.append({"name": nm.group(1), "section": section})

    # (AR) الاسم الأخير قد يخلو من الفاصلة
    tail = re.findall(r"^\s*([A-Z][A-Z_0-9]*)\s*$", body, re.M)
    known = {e["name"] for e in entries}
    for t in tail:
        if t not in known:
            entries.append({"name": t, "section": section})

    mnemonics = dict(_RE_CASE.findall(_read(IMPL)))
    for e in entries:
        e["mnemonic"] = mnemonics.get(e["name"], "")
    return entries


def isel_mnemonics() -> dict[str, set[str]]:
    """(AR) أسماء أوپكودات SIR المذكورة في جداول اختيار التعليمات لكلّ معماريّة."""
    out: dict[str, set[str]] = {}
    for isel in sorted(BACKEND_DIR.glob("*/isel.yaml")):
        arch = isel.parent.name
        out[arch] = set(re.findall(r"^\s*-\s*sir:\s*(\S+)", _read(isel), re.M))
    return out


def native_lowered() -> dict[str, set[str]]:
    """(AR) أوپكودات SIR التي تخفضها الخلفيّة الأصليّة فعلًا لكلّ معماريّة.

    (AR) تُقرأ من شيفرة الخفض نفسها لا من isel.yaml، لأنّ الجداول في مصدر
         الحقيقة ما تزال بذرةً (م٠) بينما الخفض الحقيقيّ مكتوبٌ في C++.
         الفارق بين المصدرين انجرافٌ مقصودُ الإظهار لا الإخفاء.
    """
    out: dict[str, set[str]] = {}
    مشترك = _read(SHARED_LOWERING) if SHARED_LOWERING.exists() else ""
    for arch, path in load_native_lowering().items():
        if path.exists():
            # (AR) الخفض = موضعُ معالجةٍ للأوپكود في شيفرة الخلفيّة. الشيفرة
            #      تعالجه بأحد شكلين، وإغفالُ أحدهما يُنقص العدّ زورًا:
            #        ① فرعُ switch:  case OP::BUILTIN_PRINT:
            #        ② مقارنةٌ صريحة: if (inst.opcode == sir::SIROpcode::BR)
            #      الفروعُ وحدَها كانت تُسقط BR/BR_COND (تُعالَجان بـif) فتظهر
            #      عشراتُ القواعد محجوبةً زورًا. والكنية «OP» = sir::SIROpcode.
            #      لا يُقبل مجرّدُ ذكرِ الاسم (قد يكون تعليقًا أو تشخيصَ عدمِ دعم).
            #      وتُضمّ الطبقةُ المشتركة إلى نصّ الهدف **إن كان يرثها فعلًا**:
            #      معالجةٌ نُقلت إليها تبقى خفضًا حقيقيًّا لكلّ وارثٍ لها. أمّا ضمُّها
            #      لهدفٍ لا يرثها فنسبةٌ كاذبة: هدفُ riscv64 (م٦) مخفّضٌ قائمٌ بذاته
            #      لا يمرّ بـLoweringDriver، فكان يُنسَب إليه EQ/NE/LT/LE/GT/GE — ستّةُ
            #      أوپكوداتٍ لا يعالجها — لمجرّد وجودِها في الطبقة المشتركة. الوراثةُ
            #      تُقاس من النصّ ولا تُفترَض: اشتقاقٌ صريحٌ من LoweringDriver.
            نص_الهدف = _read(path)
            # (AR) ع-٨ (مراجعةُ أميليا): المحكُّ كان نصَّ الملفِّ كلِّه، فتعليقٌ يذكر
            #      `LoweringDriver<Riscv64…>` كان يقلب الحكمَ ويعيد النسبةَ الكاذبة.
            #      المحكُّ الآن **اشتقاقٌ صريح** في رأسِ صنف، بعد نزعِ التعليقات.
            بلا_تعليقات = re.sub("//[^" + chr(10) + "]*", "", نص_الهدف)
            بلا_تعليقات = re.sub(r"/\*.*?\*/", "", بلا_تعليقات, flags=re.S)
            يرث_المشترك = bool(re.search(r":\s*public\s+(?:\w+::)*LoweringDriver\s*<", بلا_تعليقات))
            txt = نص_الهدف + ("\n" + مشترك if يرث_المشترك else "")
            qual = r"(?:OP|sir::SIROpcode|SIROpcode)"
            # (AR) اسمُ متغيّر الأوپكود يختلف (opcode/op/inst.opcode) فلا يُقيَّد.
            out[arch] = (set(re.findall(rf"case\s+{qual}::([A-Z][A-Z_0-9]*)\s*:", txt))
                         | set(re.findall(rf"==\s*{qual}::([A-Z][A-Z_0-9]*)", txt)))
        else:
            out[arch] = set()
    return out


def render(entries: list[dict], isel: dict[str, set[str]], native: dict[str, set[str]]) -> str:
    archs = sorted(set(isel) | set(native))
    covered = {a: isel.get(a, set()) for a in archs}
    nat = {a: native.get(a, set()) for a in archs}
    names = {e["name"] for e in entries}
    low = [e for e in entries if any(e["name"] in nat[a] for a in archs)]
    # (AR) انجراف: اسمٌ في isel.yaml لا وجود له في تعداد المترجم
    drift = sorted({n for a in archs for n in covered[a]} - names)

    L: list[str] = []
    L.append("# ═══════════════════════════════════════════════════════════════════")
    L.append("# sir_opcodes.yaml — كتالوج أوپكودات SIR (مصدر الحقيقة)")
    L.append("# ═══════════════════════════════════════════════════════════════════")
    L.append("# (AR) مُولَّد آليًّا بـscripts/codegen/gen_sir_opcodes_yaml.py — لا يُحرَّر يدويًّا.")
    L.append("#      المصدر: compiler/include/frontend/sir_types.h (التعداد)")
    L.append("#           + compiler/src/frontend/sir_types.cpp (المنمنمات)")
    L.append("#")
    L.append("# (AR) الغرض: يجعل أسماء الأوپكودات مفرداتٍ يحرسها مصدر الحقيقة، فيصير")
    L.append("#      حقل «lowers_to» في قواعد النحو قابلًا للتحقّق لا نصًّا حرًّا.")
    L.append("#")
    L.append("# (AR) الحقلان متمايزان عمدًا:")
    L.append("#      native_lowered ← المعماريّات التي تخفض الأوپكود فعلًا في شيفرة")
    L.append("#                        الخلفيّة الأصليّة (بلا LLVM) — القياس الصادق.")
    L.append("#      isel_declared  ← المعماريّات التي تعلن نمطًا له في backend/*/isel.yaml.")
    L.append("#      الفجوة بينهما تُظهر أنّ جداول SoT ما تزال بذرةً بينما الخفض في C++.")
    L.append("# ═══════════════════════════════════════════════════════════════════")
    L.append("")
    L.append('version: "1.0"')
    L.append('description_ar: "كتالوج أوپكودات SIR — الاسم، المنمنمة، والخفض الأصليّ"')
    L.append('description_en: "SIR opcode catalog — name, mnemonic, and native lowering"')
    L.append("")
    L.append("stats:")
    L.append(f"  total: {len(entries)}")
    L.append(f"  with_mnemonic: {sum(1 for e in entries if e['mnemonic'])}")
    L.append(f"  natively_lowered: {len(low)}")
    for a in archs:
        L.append(f"  native_lowered_{a}: {len(nat[a] & names)}")
    for a in archs:
        L.append(f"  isel_declared_{a}: {len(covered[a])}")
    L.append(f"  isel_names_not_in_enum: {len(drift)}")
    if drift:
        L.append("  isel_drift: [" + ", ".join(drift) + "]")
    L.append("")
    L.append("opcodes:")
    for e in entries:
        L.append(f"  - name: {e['name']}")
        L.append(f'    mnemonic: "{e["mnemonic"]}"')
        L.append(f'    section: "{e["section"]}"')
        L.append("    native_lowered: [" + ", ".join(a for a in archs if e["name"] in nat[a]) + "]")
        L.append("    isel_declared: [" + ", ".join(a for a in archs if e["name"] in covered[a]) + "]")
    L.append("")
    return "\n".join(L)


def main() -> int:
    ap = argparse.ArgumentParser(description="مولّد كتالوج أوپكودات SIR")
    ap.add_argument("--check", action="store_true", help="تحقّق فقط دون كتابة (CI)")
    args = ap.parse_args()
    os.environ.setdefault("PYTHONUTF8", "1")
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass

    entries = extract_opcodes()
    text = render(entries, isel_mnemonics(), native_lowered())

    if args.check:
        if not OUT.exists():
            print(f"❌ مفقود: {OUT}")
            return 1
        if _read(OUT) != text:
            print(f"❌ {OUT.name} غير محدَّث — أعِد التوليد بـgen_sir_opcodes_yaml.py")
            return 1
        print(f"✅ {OUT.name} محدَّث ({len(entries)} أوپكودًا)")
        return 0

    OUT.parent.mkdir(parents=True, exist_ok=True)
    io.open(OUT, "w", encoding="utf-8", newline="\n").write(text)
    print(f"✅ كُتب {OUT.relative_to(ROOT)} — {len(entries)} أوپكودًا، "
          f"{sum(1 for e in entries if e['mnemonic'])} بمنمنمة")
    return 0


if __name__ == "__main__":
    sys.exit(main())
