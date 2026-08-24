#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ======================================================================
# (AR) حارس «الاسم القانونيّ للمدمجة قابلٌ للنداء». المُشكِّل يجرّد التشكيلَ من
#      المعرّفات، بينما يفهرس السجلُّ المولَّد المدمجاتِ بالاسمِ القانونيِّ حرفيًّا.
#      فأيُّ اسمٍ قانونيٍّ يحملُ شدّةً (أو أيَّ حركةٍ ضمنَ مدى المُشكِّل)
#      **لا يمكن نداؤه أبدًا**: ما يكتبه المستخدمُ يصلُ مجرَّدًا فلا يطابقُ المفتاح.
#      قِيس (2026-08-24): ثلاثُ مدمجاتٍ معلَنةٍ `stable` في compiler_uefi.yaml
#      (apic_معرّف، apic_عطّل_pic، apic_وجّه_irq) كانت ترفضها الأماميّةُ بـ
#      «استدعاء دالة غير معرّفة». (والمفسّرُ لا يسجّلُها أصلًا — لاماداتُها معرَّفةٌ
#      في builtin_kernel_acpi.cpp بلا تسجيلٍ — دَينٌ مقيسٌ مستقلّ.) الحارسُ بنيويّ:
#      يقيسُ **فعلَ** التطبيعِ نفسَه (تجريد ما يجرّده المُشكِّل) لا هجاءَ اسمٍ بعينه.
# (EN) Guard: a builtin's canonical name must be callable. The lexer strips
#      diacritics from identifiers while the generated registry keys builtins by
#      the literal canonical name — so a canonical name carrying a shadda can
#      never be called. Measured: three `stable` builtins were unreachable.
# ======================================================================
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCOPE = ROOT / "language-truth" / "builtins"

try:
    import yaml
except ImportError:
    print("[builtins_canonical_callable] FATAL: pyyaml not installed", file=sys.stderr)
    sys.exit(2)

# (AR) المدى مأخوذٌ من المُشكِّلِ نفسِه لا من ظنٍّ: `lexer_core.cpp` يتخطّى
#      البايتَين 0xD9 0x8B–0x9F أي **U+064B–U+065F** كاملًا (فتشملُ الهمزةَ
#      العلويّةَ والسفليّةَ والمدّةَ فوقَ الحركاتِ والشدّة).
#      المقيسُ لا المفترَض (2026-08-24، برهانُ هويّةِ معرِّفٍ في المحرّك):
#      الحركاتُ 0x064B–0x065F (والشدّةُ 0x0651 منها) **تُجرَّد**: `سّين` و`سين`
#      المتغيّرُ نفسُه. أمّا التطويلُ 0x0640 فلا يُجرَّد: `سـين` و`سين` اسمان
#      مختلفان. فالحارسُ يقيسُ الصنفَ المبرهَنَ وحدَه ولا يوسِّع؛ والأسماءُ
#      الحاملةُ تطويلًا (`هـ` في math، و`اتصل_بـ` في sockets) صنفٌ آخرُ مقيسٌ
#      خارجَ مدى هذا الحارس ومسجَّلٌ في تقريرِ الموجة.
# (EN) Range taken from the lexer itself (lexer_core.cpp skips U+064B–U+065F).
#      Measured, not assumed: those marks ARE stripped from identifiers;
#      tatweel (0x0640) is NOT — the guard covers the proven class only.
def normalized(name: str) -> str:
    return "".join(ch for ch in name if not (0x064B <= ord(ch) <= 0x065F))


def main() -> int:
    # (AR) طرفيّةُ ويندوز قد تكون بترميزٍ محلّيّ يعجز عن العربيّة (نمطُ أخوات الحارس).
    # (EN) Windows consoles may use a local codepage that cannot encode Arabic.
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")
    offenders = []
    for path in sorted(SCOPE.glob("*.yaml")):
        try:
            doc = yaml.safe_load(path.read_text(encoding="utf-8"))
        except Exception as exc:  # noqa: BLE001 — تشخيص الملفّ المعطوب بعينه
            print(f"[builtins_canonical_callable] FATAL: {path.name}: {exc}", file=sys.stderr)
            return 2
        # (AR) بنيةُ ملفّاتِ المدمجات: خريطةٌ مفتاحُها `functions` (نمطُ الحارسِ
        #      الشقيقِ check_builtins_canonical_unique)، أو قائمةٌ مباشرةً.
        # (EN) Builtin files: a mapping keyed `functions`, or a bare list.
        entries = doc.get("functions", []) if isinstance(doc, dict) else doc
        if not isinstance(entries, list):
            continue
        for entry in entries:
            if not isinstance(entry, dict):
                continue
            canonical = entry.get("canonical")
            if not isinstance(canonical, str) or not canonical:
                continue
            if normalized(canonical) != canonical:
                offenders.append((path.name, entry.get("cpp_id", "?"), canonical))

    if offenders:
        print("[builtins_canonical_callable] ✗ أسماء قانونيّة غير قابلة للنداء "
              "(المُشكِّل يجرّد التشكيل من المعرّفات):", file=sys.stderr)
        for fname, cpp_id, canonical in offenders:
            print(f"    {fname}: {cpp_id}: {canonical} ⇒ يصل المصرّفَ "
                  f"{normalized(canonical)}", file=sys.stderr)
        print("    الإصلاح: اكتب الاسم القانونيّ مجرّدًا كما يصل المصرّف.", file=sys.stderr)
        return 1

    print("[builtins_canonical_callable] ✓ كلّ الأسماء القانونيّة قابلة للنداء")
    return 0


if __name__ == "__main__":
    sys.exit(main())
