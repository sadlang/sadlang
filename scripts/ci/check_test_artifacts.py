#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""(AR) حارسُ آثارِ الاختبارات — لا يُترَكُ ملفٌّ في مجلّدِ العملِ بلا صاحب.

ما الذي يحرسه ولماذا (قِيسَ في ١٩ آب ٢٠٢٦):
    ستّةُ آثارٍ كانت راقدةً في جذرِ المستودعِ منذ ١٥ آب — ثلاثةٌ ظاهرةٌ في
    `git status` (`pipe_win.txt` و`redirect_win.txt` و`stderr_win.txt`)
    وثلاثةٌ **مخفيّةٌ** يبتلعها `*.tmp` في `.gitignore`. فالمُدرَكُ كان نصفَ
    الواقع: «مُتجاهَلٌ» ليس «غيرَ موجود».

    ومشغّلُ الاختباراتِ بريءٌ: `tests/runner.py` يعزلُ مجلّدَ عملٍ لكلِّ تشغيلةٍ
    ثمّ يمسحُه، فـCI لا يُلوِّثُ الجذرَ. الملوِّثُ هو **التشغيلُ اليدويُّ من
    الجذر** — وهو بالضبطِ كيف تُطوَّرُ هذه الاختبارات.

العقدُ المحروس — وهو على **الأثر** لا على الوسيلة:
    كلُّ اختبارٍ يذكرُ اسمَ ملفٍّ مجرّدًا (بامتدادٍ وبلا فاصلِ مسار) ويستدعي
    دالّةً منشئةً، يجب أن يُحقِّقَ أحدَ أمرَين:
      ١) يحذفَه صراحةً (`احذف_ملف` أو `احذف_مجلد`)، أو
      ٢) يُسمّيَه `*.tmp` — فيبتلعَه `.gitignore` بقصدٍ لا بمصادفة.
    والثاني مخرجُ الاختباراتِ السالبةِ وحدَها: ما يموتُ برميةٍ لا يبلغُ سطرَ
    التنظيفِ أصلًا، فاشتراطُ الحذفِ عليه اشتراطُ محال.

⚠️ الكشفُ عن **الاسمِ الحرفيِّ في أيِّ موضعٍ من الملفّ** لا عن وسيطِ النداء:
    مسحًا أوّلَ ربطَ الاسمَ بموضعِ النداءِ فأفلتت الثلاثةُ كلُّها — لأنّها
    تُسنِدُ الاسمَ إلى متغيّرٍ أوّلًا ثمّ تُمرِّرُ المتغيّر. الشكلُ الثاني
    نحويٌّ عاديٌّ لا حيلة، والمرساةُ يجب أن تسبقَه.

(EN) Test-artifact guard: any behaviour test naming a bare relative artifact
     must delete it, or name it *.tmp so .gitignore covers it deliberately.
     Detection is by literal anywhere in the file — binding the name to the
     call site let all three real offenders escape.
"""
import io
import re
import sys
from pathlib import Path

جذر_الاختبارات = Path("tests")

دوال_منشئة = (
    "اكتب_ملف", "أضف_إلى_ملف", "افتح_وصفا", "افتح_ملف",
    "انسخ_ملف", "انقل_ملف", "أنشئ_مجلد",
)
دوال_حاذفة = ("احذف_ملف", "احذف_مجلد")

منشئة = re.compile("|".join(دوال_منشئة))
حاذفة = re.compile("|".join(دوال_حاذفة))

# (AR) اسمٌ مجرّدٌ: امتدادٌ لاتينيٌّ قصيرٌ ولا فاصلَ مسارٍ ولا حرفَ سواقة.
# ⚠️ (AR) الاسمُ لا فراغَ فيه، وامتدادُه لاتينيٌّ **صرفٌ** لا رقمَ فيه.
#      بدونِ ذلك كان `اطبع("الإصدار 1.2")` يُقرأُ أثرًا اسمُه «الإصدار 1.2»،
#      و«3.14» مثلَه. وهذا الحارسُ في `code-quality` وهي `needs:` للمنصّاتِ
#      الثلاث — فمطابقةٌ زائفةٌ واحدةٌ في ٣٩٣١ ملفًّا **توقفُ CI كلَّه**.
# ⚠️ (EN) No whitespace and a purely alphabetic extension: a decimal in
#      any string used to read as an artifact name, and this guard gates
#      every platform job through `needs:`.
اسم_مجرد = re.compile(r"\"([^\"/\:*?<>|\s]+\.[A-Za-z]{1,5})\"")

# (AR) توجيهاتُ السالب: ما يُتوقَّعُ منه رميةٌ لا يبلغُ سطرَ التنظيف.
توجيه_سالب = re.compile(r"@expect_error\b|@expect_compile_error\b|@expect_error_compiled\b")

امتداد_متجاهل = ".tmp"
امتدادات_مصدر = (".ص", ".sad")


def يُحذَف(تنفيذيّ: str, اسم: str) -> bool:
    """(AR) أيُحذَفُ هذا الاسمُ بعينِه — حرفيًّا أو عبرَ متغيّرٍ أُسنِدَ إليه؟"""
    حاذفات = "|".join(دوال_حاذفة)
    if re.search(r"(?:%s)\s*\(\s*\"%s\""
                 % (حاذفات, re.escape(اسم)), تنفيذيّ):
        return True
    # (AR) الشكلُ الثاني: `متغير م = "x.txt"` ثمّ `احذف_ملف(م)`. وهو الشكلُ
    #      الذي أفلتَ المسحَ الأوّلَ كلَّه، فلا يُترَكُ هنا.
    # (EN) The second form — assign then pass the variable — is the one that
    #      escaped the first sweep entirely.
    for مسند in re.finditer(r"([^\s=]+)\s*=\s*\"%s\"" % re.escape(اسم), تنفيذيّ):
        if re.search(r"(?:%s)\s*\(\s*%s\s*[),]"
                     % (حاذفات, re.escape(مسند.group(1))), تنفيذيّ):
            return True
    return False


def اجمع_المخالفات() -> list:
    مخالفات = []
    for ملف in sorted(جذر_الاختبارات.rglob("*.ص")):
        نص = io.open(ملف, encoding="utf-8", errors="replace").read()
        # ⚠️ (AR) الطبقةُ واحدةٌ في الجهتَين: الأسماءُ **والتنظيفُ** يُقرآن من
        #      الأسطرِ القابلةِ للتنفيذِ وحدَها. وقبلَها كان الاسمُ يُجمَعُ من نصِّ
        #      الملفِّ كلِّه، فيُحمَّرُ ملفٌّ ذُكِرَ فيه اسمُ ملفٍّ **في شرحٍ** وهو
        #      ينظّفُ نفسَه فعلًا؛ وكان `احذف_ملف` داخلَ تعليقٍ يُرضي الحارس.
        #      قراءةُ الجهتَين من طبقةٍ واحدةٍ تُسقِطُ الوجهَين معًا.
        # ⚠️ (EN) One layer for both sides: names and cleanup are read from
        #      executable lines only. Mixing layers produced a false red on a
        #      file that does clean up, and a false green on a commented-out
        #      delete.
        تنفيذيّ = "\n".join(x for x in نص.split("\n")
                          if not x.lstrip().startswith("#"))
        if not منشئة.search(تنفيذيّ):
            continue
        أسماء = sorted({م.group(1) for م in اسم_مجرد.finditer(تنفيذيّ)
                        if not م.group(1).endswith(امتدادات_مصدر)})
        if not أسماء:
            continue
        سالب = bool(توجيه_سالب.search(نص))
        for اسم in أسماء:
            # (AR) و`*.tmp` مخرجُ **السالبِ وحدَه** كما تقول الوثيقة: ما يموتُ
            #      برميةٍ لا يبلغُ سطرَ التنظيفِ أصلًا. وموجبٌ يُسمّي أثرَه `.tmp`
            #      كان ينجو بلا تنظيف — عقدٌ مكتوبٌ غيرُ منفَّذ.
            # (EN) The *.tmp escape hatch belongs to negative tests only.
            if سالب and اسم.endswith(امتداد_متجاهل):
                continue
            if يُحذَف(تنفيذيّ, اسم):
                continue
            سبب = ("اختبارٌ سالبٌ يموتُ برميةٍ فلا يبلغُ التنظيف — سمِّه "
                   "`*.tmp`" if سالب else
                   "يُنشئُ ولا يحذف — أضف `احذف_ملف` قبل آخرِ سطرِ طباعة")
            مخالفات.append((ملف.as_posix(), اسم, سبب))
    return مخالفات


def main() -> int:
    if not جذر_الاختبارات.is_dir():
        print("::error::مجلّدُ " + جذر_الاختبارات.as_posix() + " غيرُ موجود")
        return 1

    مخالفات = اجمع_المخالفات()
    if not مخالفات:
        print("✅ حارسُ آثارِ الاختبارات: لا اختبارَ يترك أثرًا بلا صاحب")
        return 0

    print("❌ حارسُ آثارِ الاختبارات — " + str(len(مخالفات)) + " أثرًا بلا صاحب:")
    for ملف, اسم, سبب in مخالفات:
        print("   " + ملف)
        print("      الأثر: " + اسم + "  ⇒  " + سبب)
        print("::error file=" + ملف + "::أثرُ اختبارٍ بلا تنظيف: " + اسم)
    return 1


if __name__ == "__main__":
    sys.exit(main())
