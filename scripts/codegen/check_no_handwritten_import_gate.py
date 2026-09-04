# -*- coding: utf-8 -*-
"""(AR) حارسٌ: لا خريطةَ «اسمُ مدمَجةٍ ⇒ وحدة» مكتوبةً باليد في أيّ محرّك.

العلّةُ التاريخيّة: كانت لكلِّ محرّكٍ خريطتُه — `sir_builder_module_check.cpp`
في المصرّف (٨١ مدخلًا، بوّابةٌ تمنع النداء) و`builtin_registry.cpp` في المفسّر
(٤٠٠ سطر، اقتراحٌ في رسالة خطأ). فصار للسؤال الواحد ثلاثةُ أجوبةٍ متناقضة:
`جذر` رياضيّةٌ في SoT، ويطلب المصرِّفُ لها «نصوص»، ويقبلها المفسّرُ حرّةً.
والأثرُ أنّ ثلاثين مدمَجةً كانت تعمل مفسَّرةً ولا تُصرَّف البتّة.

فالبوّابةُ اليوم مولَّدةٌ وحدةً (`Sad::Builtins::IMPORT_GATE`) من
`require_import` + `module`. وهذا الحارسُ يمنع عودةَ الخريطةِ اليدويّة.

الكشف: حاويةٌ ساكنةٌ من نصٍّ إلى نصٍّ يظهر في قيمها اسمُ وحدةٍ قانونيّ
(«رياضيات» «نصوص» …) داخل ملفّ محرّك. الملفّاتُ المولَّدةُ مستثناة.
"""
import re
import sys
from pathlib import Path

# (AR) الطرفيّةُ قد تكون بترميزٍ غيرِ UTF-8، فتنهار رسالةُ النجاحِ العربيّةُ بـ
#      UnicodeEncodeError ويُبلَّغ فشلٌ كاذب. نظيرُ ما تفعله بقيّةُ الحرّاس.
# (EN) A non-UTF-8 console would make the Arabic success line raise
#      UnicodeEncodeError and report a false failure. Mirrors the other guards.
for _stream in (sys.stdout, sys.stderr):
    if hasattr(_stream, "reconfigure"):
        _stream.reconfigure(encoding="utf-8")

الجذر = Path(__file__).resolve().parents[2]
# (AR) 🔑 حُذف جذرُ المفسّر: زالَ من الشجر. والمسحُ يتخطّى المفقودَ
#      بـ`continue` فلا يُحمِّر — فكان الاسمُ دعوى «مقيسٌ» لا يقيسُها أحد.
# (EN) The interpreter root is removed: it left the tree. The scan `continue`s
#      past a missing root and never reddens, so the name was an unmeasured claim.
مسارات = [الجذر / "compiler" / "src", الجذر / "shared" / "builtins" / "src"]

نص_الوحدات = (الجذر / "shared/builtins/include/module_definitions.h").read_text(encoding="utf-8")
أسماء_الوحدات = sorted({م.group(1) for م in re.finditer(r"\{\"([^\"]+)\",\s*ModuleId::(\w+)\}", نص_الوحدات)
                        if م.group(2) != "NONE"})
if not أسماء_الوحدات:
    print("[حارس] ⚠️ تعذّر استخراجُ أسماءِ الوحدات من module_definitions.h — الحارسُ بلا معنًى", file=sys.stderr)
    sys.exit(2)

# (AR) خريطةٌ ساكنة: unordered_map/map من نصٍّ إلى نصّ
نمط_الخريطة = re.compile(r"static\s+const\s+std::(?:unordered_)?map\s*<\s*std::string\s*,\s*std::string\s*>")
نمط_الوحدة = re.compile("|".join(re.escape(س) for س in أسماء_الوحدات))

مخالفات = []
for جذر_مسار in مسارات:
    if not جذر_مسار.is_dir():
        continue
    for ملف in sorted(جذر_مسار.rglob("*.cpp")):
        if "generated" in ملف.parts:
            continue
        نص = ملف.read_text(encoding="utf-8", errors="replace")
        for مطابقة in نمط_الخريطة.finditer(نص):
            # (AR) افحص جسمَ التهيئةِ الذي يلي التصريح (٤٠٠٠ محرفٍ تكفي لأكبر خريطةٍ حُذفت)
            الجسم = نص[مطابقة.end(): مطابقة.end() + 4000]
            وحدة = نمط_الوحدة.search(الجسم)
            if وحدة:
                السطر = نص[: مطابقة.start()].count("\n") + 1
                مخالفات.append((ملف.relative_to(الجذر), السطر, وحدة.group(0)))

if مخالفات:
    print("[حارس] 🔴 خريطةُ «مدمَجة ⇒ وحدة» مكتوبةٌ باليد — البوّابةُ تُولَّد من مصدرِ الحقيقة:", file=sys.stderr)
    for ملف, سطر, وحدة in مخالفات:
        print(f"    {ملف}:{سطر}  (وجد اسمَ الوحدة «{وحدة}»)", file=sys.stderr)
    print("    الحلّ: عدّل `require_import`/`module` في language-truth/builtins/*.yaml", file=sys.stderr)
    print("           ثمّ `py -3.12 x.py gen`، واستعمل Sad::Builtins::importGateModuleNameFor.", file=sys.stderr)
    sys.exit(1)

print(f"[حارس] ✅ لا خريطةَ بوّابةٍ يدويّة ({len(أسماء_الوحدات)} اسمَ وحدةٍ فُحص)")
sys.exit(0)
