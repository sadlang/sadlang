#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""(AR) نزعُ فحصِ الوجودِ عن أهدافِ LLVM التي لم تُشحَنْ في الحزمة.

لماذا (قِيسَ محلّيًّا ١٩ آب ٢٠٢٦ بفكِّ الحزمِ وبتشغيلِ cmake عليها):
    `LLVMExports.cmake` يُنهي نفسَه بحلقةٍ تتحقّقُ من **وجودِ ملفِّ كلِّ هدفٍ
    مُصدَّر**، وتُخفِقُ بـ`FATAL_ERROR` عند أوّلِ غياب. وحزمةُ conda-forge
    `llvmdev` لويندوز تُصدِّرُ ٢٧٣ مرجعًا وتشحنُ ٢٠٧ منها فقط:
      · ٦٦ تنفيذيًّا (`llvm-tblgen.exe` وإخوتُه) — تشحنُها `llvm-tools`
        في ٣٥٣ ﻣﺐ، وليس في المشروعِ ملفُّ `.td` واحدٌ يستدعيها.
      · و`LLVM-C.lib` و`LLVM-C.dll` — **لا تشحنُهما حزمةٌ واحدةٌ لويندوز**:
        `libllvm18` هناك حزمةُ بيانٍ حجمُها ٠٫١ ﻣﺐ لا مكتبةٌ مشتركة. فمطاردةُ
        الحزمِ لا تنتهي، وقد أنفقَت دورتَي CI قبلَ هذا القياس.

    والمشروعُ **يستثني `LLVM-C.lib` صراحةً** في `cmake/llvm.cmake` منذ زمنٍ
    (ربطُها يجعل `sad-build` معتمدًا على DLL وقتَ التشغيل — 0xC0000135).
    فالمانعُ ليس حاجةً إلينا بل حلقةُ تحقّقٍ في ملفِّ بائع.

ما يفعله — وهو **أقلُّ ما يكفي**:
    يُزيلُ تسجيلَ الفحصِ وحدَه (`_cmake_import_check_targets` وسطرَ
    `_cmake_import_check_files_for_*`) للأهدافِ الغائبةِ ملفّاتُها. ويُبقي
    تعريفَ الهدفِ كما هو: فمَن ربطَه يومًا يحصلُ على خطأِ رابطٍ **يُسمّيه**،
    لا على تخطٍّ صامت.

⚠️ الشرطُ اللاحقُ مقيسٌ لا مأمول: بعدَ النزعِ يُعادُ المسحُ ويُشترَطُ **صفرُ
    مرجعٍ غائب**. فإن أفلتَ شكلٌ نحويٌّ من المُحلِّلِ ظهرَ هنا حمرةً لا صمتًا.

(EN) Strip the existence-check registration for LLVM exported targets whose
     files the package never shipped. conda-forge's win-64 llvmdev exports 273
     file references and ships 207: 66 tool executables (in the 353 MB
     llvm-tools package; this project has no .td file) and LLVM-C.lib/.dll,
     which NO win-64 conda package ships — libllvm18 there is a 0.1 MB meta
     package. The project already excludes LLVM-C.lib explicitly. Only the
     check registration is removed; the imported target stays defined so a
     future consumer gets a named linker error, not a silent skip.
     Post-condition is measured: a re-scan must report zero missing.
"""
import io
import os
import re
import sys

نمط_ملفات = re.compile(
    r"^list\(APPEND _cmake_import_check_files_for_(\S+)\s+(.*?)\s*\)\s*$",
    re.M)
نمط_أهداف = re.compile(
    r"^list\(APPEND _cmake_import_check_targets\s+(\S+)\s*\)\s*$", re.M)
نمط_مسار = re.compile(r"\"([^\"]+)\"")


def مراجع(نص: str, بادئة: str) -> list:
    """(AR) [(اسم الهدف، المسار المُحَلّ)] لكلِّ مرجعٍ في ملفِّ تصدير."""
    خرج = []
    for مطابقة in نمط_ملفات.finditer(نص):
        for مسار in نمط_مسار.findall(مطابقة.group(2)):
            خرج.append((مطابقة.group(1),
                         مسار.replace("${_IMPORT_PREFIX}", بادئة)))
    return خرج


def جرّد(نص: str, غائبة: set) -> str:
    """(AR) نزعُ تسجيلِ الفحصِ عن الأهدافِ الغائبةِ وحدَها."""
    def أسقط_ملفات(مطابقة):
        return "" if مطابقة.group(1) in غائبة else مطابقة.group(0)

    def أسقط_هدفًا(مطابقة):
        return "" if مطابقة.group(1) in غائبة else مطابقة.group(0)

    نص = نمط_ملفات.sub(أسقط_ملفات, نص)
    نص = نمط_أهداف.sub(أسقط_هدفًا, نص)
    return نص


def عالج(بادئة: str) -> int:
    مجلد = os.path.join(بادئة, "lib", "cmake", "llvm")
    if not os.path.isdir(مجلد):
        print("::error::" + مجلد + " غيرُ موجود")
        return 1

    ملفات = sorted(اسم for اسم in os.listdir(مجلد)
                   if اسم.startswith("LLVMExports") and اسم.endswith(".cmake"))
    if not ملفات:
        print("::error::لا ملفَّ LLVMExports* في " + مجلد)
        return 1

    كل_المراجع, منزوعة = 0, []
    for اسم in ملفات:
        مسار = os.path.join(مجلد, اسم)
        نص = io.open(مسار, encoding="utf-8", errors="replace").read()
        قائمة = مراجع(نص, بادئة.replace(os.sep, "/"))
        كل_المراجع += len(قائمة)
        غائبة = {هدف for هدف, ملف in قائمة if not os.path.exists(ملف)}
        if not غائبة:
            continue
        منزوعة.extend(sorted(غائبة))
        io.open(مسار, "w", encoding="utf-8", newline="\n").write(جرّد(نص, غائبة))

    # ⚠️ (AR) صفرُ مرجعٍ يعني أنّ المُحلِّلَ عميٌّ لا أنّ الحزمةَ سليمة.
    # ⚠️ (EN) Zero references means a blind parser, not a healthy package.
    if كل_المراجع == 0:
        print("::error::صفرُ مرجعٍ في LLVMExports* — المُحلِّلُ نفسُه معطوب")
        return 1

    باقية = []
    for اسم in ملفات:
        نص = io.open(os.path.join(مجلد, اسم),
                     encoding="utf-8", errors="replace").read()
        باقية.extend(هدف for هدف, ملف in مراجع(نص, بادئة.replace(os.sep, "/"))
                     if not os.path.exists(ملف))

    print("مراجعُ التصدير: " + str(كل_المراجع) +
          " · أهدافٌ نُزِعَ فحصُها: " + str(len(منزوعة)))
    for هدف in منزوعة[:8]:
        print("   ✂️ " + هدف)
    if len(منزوعة) > 8:
        print("   … و" + str(len(منزوعة) - 8) + " هدفًا آخر")

    if باقية:
        print("::error::بقيَ " + str(len(باقية)) +
              " مرجعًا غائبًا بعدَ النزع: " + ", ".join(sorted(set(باقية))[:5]))
        return 1

    print("✅ صفرُ مرجعٍ غائبٍ بعدَ النزع — `find_package(LLVM CONFIG)` يمرّ")
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("::error::الاستعمال: strip_missing_llvm_exports.py <بادئةُ LLVM>")
        sys.exit(2)
    sys.exit(عالج(sys.argv[1]))
