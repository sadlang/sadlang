#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسٌ: عضوٌ مُصرَّحٌ به داخلَ حارسِ ترجمةٍ لا يُستعمَل خارجَه.

     العلّةُ المقيسة (١٩ آب ٢٠٢٦، الشوط 32285134745، ويندوز Release):

       compiler_driver_linker.cpp(686,38): error C3861:
           'find_msvc_lib_paths': identifier not found

     كان التصريحُ في `compiler_driver.h` والتعريفُ في
     `compiler_driver_build_utils.cpp` **داخلَ** `#ifdef HAS_EMBEDDED_LLD`،
     والنداءُ خارجَه تحتَ `#ifdef _WIN32` وحدَه. فحين يُطفأ الحارسُ — وهي تهيئةُ
     ويندوزَ على CI — **لا يُصرَّف المترجمُ أصلًا**. وظلَّ ذلك مستورًا لأنّ خطوةَ
     بنائه كانت تنتهي بـ`exit 0` غيرِ مشروط: أي أنّ عطبَ ترجمةٍ كاملًا عاش تحتَ
     علامةٍ خضراء.

     🔑 والعطبُ من صنفٍ لا يراه أيُّ بناءٍ **يُشغِّل الحارس**: يُصرَّف محلّيًّا
     ويُصرَّف على لينكس وماك، ولا يظهر إلّا في التهيئةِ التي يُطفأ فيها. فالحكمُ
     عليه ساكنٌ لا تجريبيّ: تُقرأ الأقسامُ المحروسةُ وتُقابَل بمواضعِ الاستعمال.

     الحكمُ في الاتّجاهَين:
       أ) عضوٌ مُصرَّحٌ به داخلَ الحارسِ ويُستعمَل خارجَه ⇒ عطبُ ترجمةٍ عند
          إطفائه (وهو ما وقع).
       ب) عضوٌ مُصرَّحٌ به خارجَ الحارسِ وتعريفُه داخلَه ⇒ عطبُ رابطٍ عند
          إطفائه — الوجهُ المقابلُ، وهو أهدأُ وأخبثُ لأنّه يمرُّ بالترجمة.

(EN) Guard: a member declared inside a compile-time guard must not be used
     outside it. Measured defect: find_msvc_lib_paths was declared and defined
     inside #ifdef HAS_EMBEDDED_LLD but called under #ifdef _WIN32 alone, so
     sad-build did not compile at all on Windows when LLD was off — invisible
     because the CI build step ended with an unconditional exit 0. Judged in
     both directions: declared-inside/used-outside (compile error) and
     declared-outside/defined-inside (link error).
"""
import re
import sys
from pathlib import Path

for _تيّار in (sys.stdout, sys.stderr):
    if hasattr(_تيّار, "reconfigure"):
        _تيّار.reconfigure(encoding="utf-8")

الجذر = Path(__file__).resolve().parents[2]
مجلد = الجذر / "tools" / "compiler"
الترويسة = مجلد / "compiler_driver.h"

# (AR) الحرّاسُ المحاكَمون. القائمةُ صريحةٌ لا مُستنتَجة: حارسُ منصّةٍ (`_WIN32`)
#      يُقصَد به الاختلافُ، أمّا حارسُ **ميزةٍ اختياريّة** فيُقصَد به أن يعمل
#      البرنامجُ بدونها — وهذا ما يجب أن يبقى صحيحًا.
# (EN) Explicit list: platform guards mean divergence on purpose; optional
#      feature guards mean the program must still build without them.
حرّاس_الميزات = ["HAS_EMBEDDED_LLD"]

نمط_موجّه = re.compile(r"^\s*#\s*(if|ifdef|ifndef|elif|else|endif)\b(.*)$")
نمط_تصريح = re.compile(
    r"^\s{8,}[A-Za-z_][\w:<>,\s\*&]*?\b([a-z_][a-z0-9_]*)\s*\([^;]*\)\s*(?:const\s*)?;")
نمط_تعليق = re.compile(r"^\s*(//|/\*|\*)")


def خريطة_الحراسة(نص: str, حارس: str) -> list:
    """(AR) لكلِّ سطرٍ: أهو داخلَ كتلةِ `حارس`؟ — بمكدّسٍ يحترم التداخل."""
    داخل, مكدّس, عمق = [], [], None
    for سطر in نص.splitlines():
        مطابقة = نمط_موجّه.match(سطر)
        if مطابقة:
            نوع, بقيّة = مطابقة.group(1), مطابقة.group(2)
            if نوع in ("if", "ifdef", "ifndef"):
                مكدّس.append(حارس in بقيّة)
                if حارس in بقيّة and عمق is None:
                    عمق = len(مكدّس)
            elif نوع == "endif" and مكدّس:
                if عمق == len(مكدّس):
                    عمق = None
                مكدّس.pop()
        داخل.append(عمق is not None)
    return داخل


def افحص(حارس: str) -> list:
    نص_الترويسة = الترويسة.read_text(encoding="utf-8", errors="replace")
    محروس = خريطة_الحراسة(نص_الترويسة, حارس)
    مُصرَّح_داخلًا, مُصرَّح_خارجًا = set(), set()
    for رقم, سطر in enumerate(نص_الترويسة.splitlines()):
        if نمط_تعليق.match(سطر):
            continue
        مطابقة = نمط_تصريح.match(سطر)
        if not مطابقة:
            continue
        (مُصرَّح_داخلًا if محروس[رقم] else مُصرَّح_خارجًا).add(مطابقة.group(1))

    مخالفات = []
    for مسار in sorted(مجلد.glob("*.cpp")):
        نص = مسار.read_text(encoding="utf-8", errors="replace")
        خريطة = خريطة_الحراسة(نص, حارس)
        for رقم, سطر in enumerate(نص.splitlines()):
            if نمط_تعليق.match(سطر):
                continue
            for عضو in مُصرَّح_داخلًا:
                if عضو + "(" in سطر and not خريطة[رقم]:
                    مخالفات.append(
                        ("أ", عضو, مسار.name, رقم + 1, سطر.strip()[:60],
                         "مُصرَّحٌ داخلَ %s ويُستعمَل خارجَه ⇒ عطبُ ترجمةٍ عند إطفائه"
                         % حارس))
            for عضو in مُصرَّح_خارجًا:
                if ("CompilerDriver::" + عضو + "(") in سطر and خريطة[رقم]:
                    مخالفات.append(
                        ("ب", عضو, مسار.name, رقم + 1, سطر.strip()[:60],
                         "مُصرَّحٌ خارجَ %s وتعريفُه داخلَه ⇒ عطبُ رابطٍ عند إطفائه"
                         % حارس))
    return مخالفات


def main() -> int:
    if not الترويسة.is_file():
        print("::error::%s غيرُ موجود" % الترويسة)
        return 1
    الكلّ = []
    for حارس in حرّاس_الميزات:
        الكلّ += افحص(حارس)
    if الكلّ:
        print("❌ حارسُ اتّساقِ الحراسة: %d مخالفة" % len(الكلّ))
        for صنف, عضو, ملفّ, رقم, سطر, سبب in الكلّ:
            print("   [%s] %s — %s:%d" % (صنف, عضو, ملفّ, رقم))
            print("       %s" % سطر)
            print("       %s" % سبب)
        print("   ⇦ العلاج: أخرِجِ العضوَ من الحارسِ إن لم يكن من شأنِ الميزة،")
        print("      أو احرسْ موضعَ الاستعمالِ بالحارسِ نفسِه — والأوّلُ أصحُّ حين")
        print("      يلزم الاستعمالُ في تهيئةٍ بلا الميزة.")
        return 1
    print("✅ حارسُ اتّساقِ الحراسة: كلُّ عضوٍ مُصرَّحٍ داخلَ %s يُستعمَل داخلَه، "
          "وكلُّ مُصرَّحٍ خارجَه يُعرَّف خارجَه"
          % "/".join(حرّاس_الميزات))
    return 0


if __name__ == "__main__":
    sys.exit(main())
