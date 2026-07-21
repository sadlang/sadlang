#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ======================================================================
# (AR) حارس «لا مفاتيح خصائص خام» في رسوميّات SadUI كاملةً (features/graphics).
#      يفشل (خروج 1) إن كتب أيّ ملفّ `findProperty("literal")` بسلسلة خام بدل
#      ثابت SoT المولَّد `sad::ui::props::<ID>` (من language-truth/ui_props.yaml).
#      الحدّ نظيفٌ اليوم (القلب + كلّ الخلفيّات backends مُرحَّلة)؛ الحارس وقائيّ
#      يمنع انحدار المكتسب. يمسح النصّ الكامل (يلتقط الاستدعاء متعدّد الأسطر).
# (EN) Guard: no raw property-key literals anywhere in SadUI graphics. Fails if a
#      file writes findProperty("literal") instead of the generated SoT constant
#      sad::ui::props::<ID>. Clean today (core + all backends migrated); preventive.
# ======================================================================
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCOPE = ROOT / "features" / "graphics"
# أدلّة تُستثنى من المسح (بناء/وسائط خارجيّة لا مصدر SadUI).
EXCLUDE_DIRS = {"build", "build_release", "cmake-build-debug", "cmake-build-release", "third_party", "vendor"}
# نمط استدعاء findProperty بوسيط سلسلة نصّيّة حرفيّة (لا ثابت props::).
RAW = re.compile(r'findProperty\(\s*"')


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    if not SCOPE.is_dir():
        print("حارس مفاتيح الخصائص: المجلّد غير موجود: %s" % SCOPE)
        return 2
    violations = []
    files = [p for p in (sorted(SCOPE.rglob("*.cpp")) + sorted(SCOPE.rglob("*.h")))
             if not EXCLUDE_DIRS & set(p.parts)]
    for f in files:
        text = f.read_text(encoding="utf-8", errors="surrogateescape")
        # المطابقة على النصّ الكامل لا سطرًا-بسطر: `\s*` في النمط يطابق `\n`، فيُلتقط
        # نمط الاستدعاء المتعدّد الأسطر السائد (`findProperty(` ثمّ `"..."` بسطر تالٍ).
        for m in RAW.finditer(text):
            ln = text.count("\n", 0, m.start()) + 1
            snip = text[m.start():m.start() + 80].replace("\n", " ").strip()
            violations.append((f.relative_to(ROOT).as_posix(), ln, snip))
    print("حارس «لا مفاتيح خصائص خام» في رسوميّات SadUI:")
    print("  النطاق:   %s" % SCOPE.relative_to(ROOT).as_posix())
    print("  الملفّات: %d" % len(files))
    if violations:
        print("  ✗ خرق (%d) — استعمل ثابت SoT `sad::ui::props::<ID>` لا سلسلة خام:" % len(violations))
        for path, ln, snip in violations[:40]:
            print("     %s:%d  %s" % (path, ln, snip))
        if len(violations) > 40:
            print("     ... و%d أخرى" % (len(violations) - 40))
        print("  المصدر: language-truth/ui_props.yaml ⇒ prop_keys.h (x.py gen).")
        return 1
    print("  ✓ سليم — لا مفاتيح خصائص خام (كلّها ثوابت props:: المولَّدة من SoT).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
