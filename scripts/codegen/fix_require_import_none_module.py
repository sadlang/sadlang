# -*- coding: utf-8 -*-
"""(AR) إصلاحٌ مُلزَمٌ في مصدر الحقيقة: `module: NONE` ⇒ `require_import: false`.

العلّة: ٧٨٠ مدخلًا من ١١١٨ تحمل `require_import: true` مع `module: NONE` — وهو
تناقضٌ ذاتيّ: «يلزمه استيرادُ وحدة» + «لا ينتمي إلى وحدة». وما دامت البوّابةُ
تُولَّد من هذين الحقلين، فتوليدُ هذا التناقضِ حرفيًّا يجعل تلك المدمَجاتِ **غيرَ
قابلةٍ للنداء أبدًا**: لا وحدةَ تُستورَد لتحريرها. ومنها كتالوجُ SadUI كلُّه
وواجهاتُ النواة وUEFI وFFI.

فالإصلاحُ منطقيٌّ لا اجتهاديّ: لا وحدةَ ⇒ لا استيراد.

التحرير نصّيٌّ لا عبر تحميل YAML وإعادة كتابته: الملفّاتُ تحمل تعليقاتِ عقودٍ
(انظر رأس ui_widgets.yaml) تُفقَد في الدورة الكاملة.
"""
import sys
from pathlib import Path

الجذر = Path(__file__).resolve().parents[2]
مجلدات = [الجذر / "language-truth" / "builtins", الجذر / "language-truth" / "stdlib"]

بداية_مدخل = "- cpp_id:"
حقل_الوحدة_عدم = "module: NONE"
محجوب = "require_import: true"
حرّ = "require_import: false"


def أصلح_ملفًّا(المسار: Path) -> int:
    الأسطر = المسار.read_text(encoding="utf-8").splitlines(keepends=True)
    كتل, الكتلة_الجارية = [], []
    for سطر in الأسطر:
        if سطر.lstrip().startswith(بداية_مدخل) and الكتلة_الجارية:
            كتل.append(الكتلة_الجارية)
            الكتلة_الجارية = []
        الكتلة_الجارية.append(سطر)
    if الكتلة_الجارية:
        كتل.append(الكتلة_الجارية)

    عدد = 0
    for كتلة in كتل:
        if not any(حقل_الوحدة_عدم in س for س in كتلة):
            continue
        for فهرس, سطر in enumerate(كتلة):
            if سطر.strip() == محجوب:
                كتلة[فهرس] = سطر.replace(محجوب, حرّ)
                عدد += 1
    if عدد:
        المسار.write_text("".join(س for كتلة in كتل for س in كتلة), encoding="utf-8")
    return عدد


المجموع = 0
for مجلد in مجلدات:
    if not مجلد.is_dir():
        continue
    for ملف in sorted(مجلد.glob("*.yaml")):
        عدد = أصلح_ملفًّا(ملف)
        if عدد:
            print(f"  {ملف.relative_to(الجذر)}: {عدد}")
            المجموع += عدد

print(f"[إصلاح] مدخلاتٌ حُرِّرت (module: NONE ⇒ require_import: false): {المجموع}")
sys.exit(0)
