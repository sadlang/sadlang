#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) طابعُ تفاصيلِ إخفاقاتِ التكافؤ من تقرير `runner.py --report`.

     🔑 **بوّابةٌ تُخفِقُ ولا تقولُ ما أخفقَ نصفُ بوّابة.** قِيس على PR #416
     (٢٠٢٦-٠٨-١٨): خطوةُ «مصفوفةُ القواعد» أخفقت على ٦ من ٣٠٠٢ على لينكس،
     ولم يحمل سجلُّ الشوطِ إلّا **أسماءَ** الملفّاتِ الستّة — لا المُخرَجَ ولا
     الفرقَ بين المحرّكَين. فتعذّر الحكمُ على السبب من على بُعد، وكان لا بدّ من
     شوطٍ ثانٍ لمجرّدِ **رؤيةِ** ما رآه الشوطُ الأوّل. والتقريرُ يحملُ المُخرَجَين
     أصلًا (`interp_output` و`compiler_output` في `write_report`)، فالنقصُ كان
     في العرضِ لا في القياس.

     ⚠️ ولا يُخفِقُ هذا الطابعُ بنفسِه أبدًا: هو أداةُ عرضٍ تعملُ **بعد** حكمٍ
     صدر، فلو أخفق لَحجبَ الحكمَ الذي جاء ليشرحَه.

(EN) Prints the details of parity failures from a `runner.py --report` JSON.
     A gate that fails without saying what failed is half a gate: measured on
     PR #416, the run log carried only the six file names, not the outputs, so
     a second run was needed merely to SEE what the first run had already seen.
     The report already carries both outputs; only the display was missing.
     This printer never fails on its own — it explains a verdict already given.
"""
import io
import json
import sys

# (AR) الخرجُ عربيٌّ دائمًا، ومحطّاتُ CI قد تُهيّئُ ترميزًا غيرَ UTF-8.
# (EN) The output is Arabic; CI hosts may default to a non-UTF-8 encoding.
if hasattr(sys.stdout, "buffer"):
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")

# (AR) حالاتٌ تُعَدُّ إخفاقًا — المصدرُ نفسُه الذي يحكمُ به `runner.py`:
#      كلُّ حالةٍ يبدأُ اسمُها بـ`FAIL`. و`KNOWN_RED` ليست منها عمدًا.
# (EN) Failure statuses — the same rule runner.py judges by: any status whose
#      name starts with FAIL. KNOWN_RED is deliberately not one of them.
FAIL_PREFIX = "FAIL"
MAX_SHOWN = 40


def main(argv):
    if len(argv) < 2:
        print("الاستعمال: print_parity_failures.py <مسار تقرير JSON>")
        return 0
    path = argv[1]
    try:
        with io.open(path, encoding="utf-8") as handle:
            report = json.load(handle)
    except (OSError, ValueError) as exc:
        print("ℹ️ تعذّرت قراءةُ التقرير (%s): %s" % (path, exc))
        return 0

    tests = report.get("tests", [])
    failures = [t for t in tests if str(t.get("status", "")).startswith(FAIL_PREFIX)]
    if not failures:
        print("ℹ️ لا إخفاقَ في التقرير — الحكمُ جاء من مكانٍ آخر.")
        return 0

    print("")
    print("══════════════════════════════════════════════════")
    print("  تفصيلُ الإخفاق: %d من %d" % (len(failures), report.get("total", len(tests))))
    print("══════════════════════════════════════════════════")

    for index, entry in enumerate(failures[:MAX_SHOWN], 1):
        print("")
        print("── [%d] %s" % (index, entry.get("file", "?")))
        print("   الحالة: %s" % entry.get("status", "?"))
        if entry.get("error"):
            print("   الخطأ: %s" % entry["error"])
        interp = entry.get("interp_output")
        compiled = entry.get("compiler_output")
        if interp is not None or compiled is not None:
            print("   ── مفسّر ──")
            for line in (interp or "").splitlines() or ["(فارغ)"]:
                print("   | %s" % line)
            print("   ── مترجم ──")
            for line in (compiled or "").splitlines() or ["(فارغ)"]:
                print("   | %s" % line)

    if len(failures) > MAX_SHOWN:
        print("")
        print("… و%d إخفاقًا آخرَ لم يُعرَض (السقف %d)." % (len(failures) - MAX_SHOWN, MAX_SHOWN))
    print("")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
