#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارس حدود المفسّر — RFC sadlang-rfcs#10 (تصليب الحدود الداخليّة).

    الخلفية: في المرحلة 3 أُغلق بابا تسريب ترويسات المفسّر إلى نظام المترجم — تصدير
    `sad_shared` لمسارات `interpreter/include*` (أُزيل بعد نقل `class_manager.h` إلى
    `shared/types/`)، والكتلة العامّة `include_directories` في الجذر (نُقلت إلى
    `sad_interp PUBLIC`). الحدّ نظيفٌ **بنيويًّا** اليوم. هذا الحارس **وقائيّ**: يثبّت
    الوضع السليم ويفشل إن تسلّل اقترانٌ عَرَضيّ من نظام المترجم إلى ترويسات المفسّر
    (عبر مسار `interpreter/...` صريح، أو اسمٍ مكشوفٍ لو أُعيد فتحُ أيِّ بابٍ مستقبلًا).

    المرجع: docs/architecture/cmake-target-boundaries.md §5.

(EN) Interpreter-boundary guard. Fails if any compiler-subsystem source includes an
     interpreter-only header (directly via an `interpreter/...` path, or by a bare name
     that would resolve through a leaked PUBLIC include dir if one were reopened).
     Both leak doors were closed in phase 3; preventive guard against regression.
"""
import os, re, sys, io

# (AR) ضمان مخرجات UTF-8 على وحدات تحكّم ويندوز (cp125x) وفي CI سواء.
try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
except (AttributeError, ValueError):
    pass

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

INTERP_INC = os.path.join(ROOT, "interpreter", "include")
# (AR) جذور المسح = نظام المترجم وحده (أوضح حدّ؛ أدوات repl/profiler تستضيف المفسّر شرعًا)
SCAN_ROOTS = [
    os.path.join(ROOT, "compiler", "src"),
    os.path.join(ROOT, "compiler", "include"),
]
# (AR) مواضع تملك نسخها الشرعيّة من ترويسة بنفس الاسم ⇒ لا تُعدّ حصريّة للمفسّر
ELSEWHERE_INC = [
    os.path.join(ROOT, "compiler", "include"),
]
for base in ("shared",):
    for dirpath, _dirs, _files in os.walk(os.path.join(ROOT, base)):
        if os.path.basename(dirpath) == "include":
            ELSEWHERE_INC.append(dirpath)

HDR_EXT = (".h", ".hpp", ".hh", ".hxx")
SRC_EXT = (".cpp", ".cc", ".cxx", ".c", ".h", ".hpp", ".hh", ".hxx")
INC_RE = re.compile(r'^\s*#\s*include\s*[<"]([^">]+)[">]')


def collect_headers(base):
    """ترجع (مجموعة المسارات النسبيّة, مجموعة الأسماء)."""
    rels, names = set(), set()
    if not os.path.isdir(base):
        return rels, names
    for dp, _d, files in os.walk(base):
        for f in files:
            if f.endswith(HDR_EXT):
                rels.add(os.path.relpath(os.path.join(dp, f), base).replace("\\", "/"))
                names.add(f)
    return rels, names


def main():
    interp_rels, interp_names = collect_headers(INTERP_INC)
    # أسماء متوفّرة شرعًا في مكان آخر (نسخة خاصّة بالمترجم/المشترك)
    elsewhere_names = set()
    for inc in ELSEWHERE_INC:
        _r, n = collect_headers(inc)
        elsewhere_names |= n
    interp_exclusive = interp_names - elsewhere_names

    violations = []
    scanned = 0
    for root in SCAN_ROOTS:
        if not os.path.isdir(root):
            continue
        for dp, _d, files in os.walk(root):
            for f in files:
                if not f.endswith(SRC_EXT):
                    continue
                path = os.path.join(dp, f)
                scanned += 1
                try:
                    with io.open(path, encoding="utf-8", errors="replace") as fh:
                        for ln, line in enumerate(fh, 1):
                            m = INC_RE.match(line)
                            if not m:
                                continue
                            inc = m.group(1).replace("\\", "/")
                            b = inc.rsplit("/", 1)[-1]
                            rel = os.path.relpath(path, ROOT).replace("\\", "/")
                            if inc.startswith("interpreter/"):
                                violations.append((rel, ln, inc, "مسار صريح interpreter/"))
                            elif "/" in inc and inc in interp_rels:
                                violations.append((rel, ln, inc, "مسار نسبيّ يطابق ترويسة مفسّر"))
                            elif "/" not in inc and b in interp_exclusive:
                                violations.append((rel, ln, inc, "اسم حصريّ للمفسّر (يَحلّ عبر مسار التوافق المسرَّب)"))
                except OSError:
                    pass

    print("== حارس حدود المفسّر / interpreter-boundary guard ==")
    print(f"   ترويسات المفسّر: {len(interp_names)} (حصريّة: {len(interp_exclusive)})")
    print(f"   ملفّات نظام المترجم المفحوصة: {scanned}")
    if violations:
        print(f"\n[FAIL] {len(violations)} اختراق(ات) لحدّ المفسّر من نظام المترجم:\n")
        for rel, ln, inc, why in violations:
            print(f"  {rel}:{ln}  #include \"{inc}\"  ← {why}")
        print("\nنظام المترجم (sad-build) يجب ألّا يعتمد على ترويسات المفسّر (interpreter/).")
        print("راجع docs/architecture/cmake-target-boundaries.md §5.")
        return 1
    print("\n[OK] لا اختراق — حدّ المفسّر سليم (نظام المترجم لا يضمّن ترويسات المفسّر).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
