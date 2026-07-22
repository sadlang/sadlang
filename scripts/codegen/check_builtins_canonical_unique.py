#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ======================================================================
# (AR) حارس «تفرّد canonical عبر نطاقات المدمجات». مسجّل الدوالّ في المحرّكين
#      يفهرس المدمجات بالاسم القانونيّ وحده (registerBuiltinFunction يدهس
#      المفتاح نفسه بصمت)، فأيّ اسمين قانونيّين متطابقين في نطاقين مختلفين
#      يعني أنّ الفائز يتقرّر بترتيب الاستيراد بصمت — كما كاد يقع بين «ضم»
#      البتّيّة (KernelCpu) و«ضم» الأزواج (Maps::ZIP، صارت «زاوج»).
#      يجمع الحارس كلّ canonical من language-truth/builtins/*.yaml ويفشل
#      (خروج 1) عند أيّ تكرار جديد — بعد تجريد التشكيل والتطويل فقط (وهو ما
#      يفعله المُشكِّل بالمعرّفات)، دون طيّ الهمزات (تأكد ≠ تاكد لدى المُشكِّل).
#      التصادمات الموروثة قبل ولادة الحارس مقيَّدة في قائمة دَين صريحة
#      «تنكمش فقط»: سدّ تصادمًا ⇒ احذفه منها، وممنوع إضافة جديد إليها
#      إلّا بقرار مالك موثَّق.
# (EN) Guard: builtin canonical names must be unique across all namespaces.
#      The registry keys builtins by canonical name only, so a duplicate is
#      silently resolved by import order. Fails on any duplicate (after
#      diacritics/tatweel stripping only — what the lexer does) that is not
#      in the explicit shrink-only inherited-debt list below.
# ======================================================================
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCOPE = ROOT / "language-truth" / "builtins"

try:
    import yaml
except ImportError:
    print("[builtins_canonical_unique] FATAL: pyyaml not installed", file=sys.stderr)
    sys.exit(2)

# (AR) دَين موروث: تصادمات كانت قائمة يوم وُلد الحارس (2026-07-22)، كلّ قيد
#      «اسم مطبَّع ⇒ عدد التسجيلات المسموح». القائمة تنكمش فقط — سُدّ تصادم؟
#      احذف قيده. ظهور الاسم أقلّ من المسموح = القيد بائت ويجب حذفه (فشل).
# (EN) Inherited debt: collisions that predate the guard. Shrink-only.
INHERITED_DEBT = {
    "أرسل": 2,          # AsyncAdvanced/CHANNEL_SEND_ALT ⇄ HttpClient/POST
    "انسخ_ذاكرة": 2,    # Kernel/MEMCPY ⇄ KernelMemory/MEM_6
    "منصة": 2,          # FFI/FFI_PLATFORM ⇄ Platform/PLATFORM_NAME
    "ساعة": 2,          # Maps/HOUR ⇄ Platform/CLOCK
    "رمز_حرف": 2,       # Maps/CHAR_CODE ⇄ Strings/CHAR_CODE
    "استبدل": 3,        # HttpClient/PUT ⇄ Strings/REPLACE ⇄ UICore/REPLACE_PAGE
    "رقم": 2,           # Maps/ENUMERATE ⇄ TypeCtor/TO_INT
    "فك_تشفير": 2,      # Assertions/DECRYPT ⇄ UICrypto/DECRYPT
    "هاش": 2,           # Assertions/HASH ⇄ UICrypto/HASH_FN
    "حلل_جيسون": 2,     # Maps/JSON_PARSE_ALT ⇄ UINetwork/JSON_PARSE
    "اقرأ_ملف": 2,      # Basics/READ_FILE ⇄ UIIO/READ_FILE
    "اكتب_ملف": 2,      # Basics/WRITE_FILE ⇄ UIIO/WRITE_FILE
}


def strip_diacritics(s: str) -> str:
    """(AR) تجريد التشكيل والتطويل فقط — مرآة ما يفعله المُشكِّل بالمعرّفات
    (U+064B..U+0652 + الألف الخنجريّة U+0670 + التطويل U+0640)، بلا طيّ همزات.
    (EN) Strip diacritics/tatweel only — mirrors the lexer; no hamza folding."""
    return "".join(
        ch for ch in s
        if not (0x064B <= ord(ch) <= 0x0652 or ord(ch) in (0x0670, 0x0640))
    )


def _entries(path: Path):
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if isinstance(data, dict):
        data = data.get("functions", [])
    if isinstance(data, list):
        for item in data:
            if isinstance(item, dict) and "canonical" in item:
                yield item


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    if not SCOPE.is_dir():
        print("حارس تفرّد canonical: المجلّد غير موجود: %s" % SCOPE, file=sys.stderr)
        return 2
    occurrences = {}  # normalized -> [(canonical, namespace, file, cpp_id)]
    files = sorted(p for p in SCOPE.glob("*.yaml") if not p.name.startswith("_"))
    total = 0
    for f in files:
        rel = f.relative_to(ROOT).as_posix()
        for e in _entries(f):
            total += 1
            name = str(e["canonical"])
            occurrences.setdefault(strip_diacritics(name), []).append(
                (name, str(e.get("namespace", "?")), rel, str(e.get("cpp_id", "?")))
            )
    new_dups, stale_debt = [], []
    for key, occ in occurrences.items():
        allowed = INHERITED_DEBT.get(key, 1)
        if len(occ) > allowed:
            new_dups.append((key, occ))
    for key, allowed in INHERITED_DEBT.items():
        if len(occurrences.get(key, [])) < allowed:
            stale_debt.append((key, allowed, len(occurrences.get(key, []))))
    print("حارس «تفرّد canonical عبر نطاقات المدمجات»:")
    print("  النطاق:   %s" % SCOPE.relative_to(ROOT).as_posix())
    print("  الملفّات: %d · الأسماء: %d · دَين موروث مُعلَن: %d"
          % (len(files), total, len(INHERITED_DEBT)))
    rc = 0
    if new_dups:
        print("  ✗ تصادم جديد (%d) — الاسم القانونيّ يجب أن يتفرّد عبر كلّ النطاقات،"
              % len(new_dups))
        print("    وإلّا تقرّر الفائز بترتيب الاستيراد بصمت (المسجّل يدهس المفتاح):")
        for key, occ in new_dups:
            print("     «%s»:" % key)
            for (n, ns, rel, cid) in occ:
                print("        - «%s» (%s/%s) في %s" % (n, ns, cid, rel))
        rc = 1
    if stale_debt:
        print("  ✗ قيود دَين بائتة (%d) — سُدّ التصادم فاحذف قيده (القائمة تنكمش فقط):"
              % len(stale_debt))
        for key, allowed, actual in stale_debt:
            print("     «%s»: المسموح %d والموجود %d" % (key, allowed, actual))
        rc = 1
    if rc == 0:
        print("  ✓ سليم — لا تصادم جديدًا في الأسماء القانونيّة عبر النطاقات "
              "(بعد تجريد التشكيل، والدَّين الموروث مقيَّد لا يتّسع).")
    return rc


if __name__ == "__main__":
    sys.exit(main())
