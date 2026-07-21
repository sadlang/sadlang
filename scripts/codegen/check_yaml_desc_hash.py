#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ======================================================================
# (AR) حارس «# في وصف yaml غير مقتبس» في language-truth. حقل «description_*»
#      قيمته نثرٌ بشريّ؛ إن كان **غير مقتبس** وحوى « #» (مسافة ثمّ هاش) بترته YAML
#      تعليقًا صامتًا (رُصد حيًّا: وصف COLOR «...سداسيّة #RRGGBB...» بُتِر عند
#      «سداسيّة»، فانبتر تعليق prop_keys.h وصفحة docs المولَّدان). الحلّ: اقتباس
#      القيمة. يفشل (خروج 1) عند أيّ حقل وصف غير مقتبس فيه « #».
# (EN) Guard: no unquoted ` #` in language-truth description_* fields — YAML would
#      silently truncate the prose at the hash (treated as a comment). Quote it.
# ======================================================================
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCOPE = ROOT / "language-truth"
# حقل وصف: description_ar/description_en/description/desc، قيمته لا تبدأ باقتباس أو
# رمز كتلة (| > & * [ {)، وتحوي « #» (مسافة+هاش) ⇒ بترٌ صامت.
DESC = re.compile(r'^\s*(?:-\s*)?desc(?:ription)?(?:_\w+)?:\s+(?P<val>[^"\'|>&*\[\{#\s].*?)\s+#')


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    if not SCOPE.is_dir():
        print("حارس وصف yaml: المجلّد غير موجود: %s" % SCOPE)
        return 2
    files = sorted(SCOPE.rglob("*.yaml")) + sorted(SCOPE.rglob("*.yml"))
    breaches = []
    for f in files:
        for i, line in enumerate(f.read_text(encoding="utf-8", errors="surrogateescape").splitlines(), 1):
            if DESC.search(line):
                breaches.append((f.relative_to(ROOT).as_posix(), i, line.strip()[:90]))
    print("حارس «# في وصف yaml غير مقتبس»:")
    print("  النطاق:   %s" % SCOPE.relative_to(ROOT).as_posix())
    print("  الملفّات: %d" % len(files))
    if breaches:
        print("  ✗ خرق (%d) — اقتبِس القيمة (\"...\") كي لا يبتر YAML النصّ عند « #»:" % len(breaches))
        for path, ln, snip in breaches[:40]:
            print("     %s:%d  %s" % (path, ln, snip))
        if len(breaches) > 40:
            print("     ... و%d أخرى" % (len(breaches) - 40))
        return 1
    print("  ✓ سليم — لا « #» غير مقتبس في حقول الوصف (لا بتر صامت).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
