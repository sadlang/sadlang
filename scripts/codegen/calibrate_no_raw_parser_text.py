#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ **«لا نصَّ تشخيصٍ خامٍّ في المحلّل النحويّ»**.

     الحارسُ يدَّعي أنّ كلَّ رسالةِ خطأٍ أو تحذيرٍ في `shared/parser/src`
     تُركَّبُ من كتالوجِ الأخطاءِ المولَّدِ من `language-truth/errors/*.yaml`،
     ولا تُكتَبُ حرفيًّا في الشيفرة. والنصُّ المكتوبُ يدويًّا يفقدُ رمزَ الخطأ،
     فلا يراه `sad-check --json` ولا الـLSP، ولا يُترجَمُ ولا يخضعُ لمستوى الشرح.

     وهو حارسٌ محكَمُ البناءِ: يفحصُ **البنيةَ** لا النصّ (فالبايتاتُ السداسيّةُ
     لا تُفلِت)، ويجرّدُ التعليقاتِ بترتيبِ بدائلَ يمنعُ `//` داخلَ سلسلةٍ من
     ابتلاعِ السطر. ولم يكن مُعايَرًا قطّ.

     🔑 **وما كشفَه العيارُ في الحارسِ نفسِه (٢٠٢٦-٠٩-٠٦):**

      ① **«لا يُزاد إليه أبدًا» كانت دعوى بلا قياس.** خطُّ الأساسِ يُخفِقُ على
         انحدارِ ملفٍّ عن رقمِه، لكنّ **رفعَ الرقمِ نفسِه** بابٌ مفتوح: تُكتَبُ
         خمسُ رسائلَ خامّةٍ ويُرفَعُ العددُ خمسًا فيخضرُّ الحارسُ في الحال —
         وهو عينُ ما وُجِدَ ليمنعَه. فصارَ للمجموعِ سقفٌ نازلٌ يُقاس.

      ② **والعمى كان يُقرَأُ تقدُّمًا.** لو عمِيَت المصارفُ (أُعيدت تسميةُ
         `error`/`consume`) أو خلا النطاقُ من ملفّاتٍ، لَخلَت المخالفاتُ كلُّها
         ولَطُبِعَ «⚠ كُنِست مواضعُ ولم يُنقَص خطُّ الأساس» — **دعوى إنجازٍ عن
         مسحٍ لم يقعْ**، ورمزُ ١ يقرؤه `x.py` حكمًا على المحتوى. فصارت أرضيّةُ
         عمًى على عددِ نداءاتِ المصارفِ وعطبُ آلةٍ برمزِ ٢ قبلَ كلِّ حكم.

(EN) Injection harness for the raw-parser-text guard: prove the ratchet bites
     in both directions, that the newly-added baseline ceiling is read, and
     that a blind scan now exits 2 instead of claiming progress it never made.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ** — والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import Harness, eol_to_lf, run, sub  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_no_raw_parser_text.py"
HARNESS = Path(__file__).resolve()
RECORD = (ROOT / "scripts" / "codegen" / "calibration"
          / "check_no_raw_parser_text.yaml")


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()


# ═══ أهدافُ الحقن ═══════════════════════════════════════════════════════
GUARD_REL = "scripts/codegen/check_no_raw_parser_text.py"
OOP = "shared/parser/src/declarations/parser_oop.cpp"
LF = chr(10)

# (AR) مرساةٌ حقيقيّةٌ في الشجرة: نداءُ كتالوجٍ سليمٌ يُحقَنُ قبلَه النثرُ الخامّ.
CATALOG_CALL = ("errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME,"
                ' {{"what_ar", "نوع الحقل"}').encode("utf-8")


def _before_call(extra: str) -> bytes:
    """(AR) يُدخِلُ سطرًا قبلَ نداءِ الكتالوجِ — داخلَ الجسمِ نفسِه."""
    return (extra + LF + "                ").encode("utf-8") + CATALOG_CALL


# (AR) 🔑 **والنصُّ المنتظَرُ وحدَه لا يكفي.** سطرُ العدَّاداتِ يُطبَعُ في كلِّ
#      تشغيلة، فالمرساةُ سطرُ الشكوى وحدَه — ويُنفى ما سواه.
_REGRESSION = "✗ انحدار"
_FALSE_PROGRESS = "كُنِست مواضعُ ولم يُنقَص خطّ الأساس"
_OVER_CEIL = "ينحدرُ ولا يُرفَع: ارفعِ"
_NO_FILES = "لا ملفَّ مصدرٍ في النطاقِ"
_BLIND = "قارئٌ لا يُبصِر"
_FEW_CONSUME = "النطاقُ المقروءُ ليس محلّلًا نحويًّا"
_NO_SCOPE = "المجلّد غير موجود"
_ALL_MARKS = (_REGRESSION, _FALSE_PROGRESS, _OVER_CEIL, _NO_FILES, _BLIND,
              _FEW_CONSUME, _NO_SCOPE)

_CLEAN = "✓ لا انحدار"


def _only(mark: str):
    return (mark, tuple(other for other in _ALL_MARKS if other != mark))


def _green():
    return (_CLEAN, _ALL_MARKS)


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    # ═══ ① الانحدار: رسالةٌ خامّةٌ جديدةٌ في ملفٍّ بلغَ سقفَه ═══
    ("① نصٌّ عربيٌّ خامٌّ في مصرفِ `error` يُحمِّر",
     OOP, sub(CATALOG_CALL,
              _before_call('error("زز مجسّ: نصٌّ خامٌّ لا رمزَ له");'),
              residue=True),
     1, _only(_REGRESSION)),

    # (AR) 🔑 **والبنيةُ لا النصّ**: موضعٌ في الشجرةِ كُتِبَ بايتاتٍ سداسيّةً
    #      هربًا من مشكلةِ ترميزٍ فيُفلِتُ من أيِّ بحثٍ حرفيّ. والمجسُّ يحقنُ
    #      الصورةَ عينَها — لا حرفَ عربيًّا واحدًا في الشيفرة.
    ("② نصٌّ عربيٌّ مكتوبٌ بايتاتٍ سداسيّةً لا يُفلِت",
     OOP, sub(CATALOG_CALL,
              _before_call(r'error("\xD9\x86\xD8\xB5\xD9\x91 zz_probe_hex");'),
              residue=True),
     1, _only(_REGRESSION)),

    # (AR) و`consume(نوع، "رسالة")` هو المصرفُ الذي كُنِسَت منه ٩٧ رسالة،
    #      ونثرُه في الوسيطِ **الثاني** فلا يراه فاحصُ الوسيطِ الأوّل.
    ("③ نثرٌ في الوسيطِ الثاني لـ`consume` لا يُفلِت",
     OOP, sub(CATALOG_CALL,
              _before_call('consume(TT::SEMICOLON, "زز مجسّ في consume");'),
              residue=True),
     1, _only(_REGRESSION)),

    # (AR) و`std::cerr` لافتةٌ يدويّةٌ بلا تسجيلٍ مركزيّ — لا رمزَ خروجٍ يحملُها.
    ("④ لافتةُ `std::cerr` خامّةٌ تُحمِّر",
     OOP, sub(CATALOG_CALL,
              _before_call('std::cerr << "زز مجسّ لافتة";'), residue=True),
     1, _only(_REGRESSION)),

    # ═══ ② السقّاطةُ تعضُّ في الاتّجاهِ الآخرِ أيضًا ═══
    # (AR) وموضعٌ كُنِسَ ولم يُنقَصْ خطُّ الأساسِ يُحمِّرُ كذلك — وإلّا بقيَ
    #      الرقمُ رصيدًا يُسحَبُ منه لاحقًا بلا احمرار.
    ("⑤ رقمُ خطِّ أساسٍ أعلى من المقيسِ يُحمِّر",
     GUARD_REL, sub(b'"shared/parser/src/ui/parser_ui.cpp": 19,',
                    b'"shared/parser/src/ui/parser_ui.cpp": 19,'
                    + LF.encode("utf-8")
                    + b'    "shared/parser/src/core/parser_helpers.cpp": 11,'
                      b'  # zz_probe_stale_baseline', residue=True),
     1, _only(_FALSE_PROGRESS)),

    # ═══ ③ السقفُ النازلُ — اللامتغيِّرُ الذي استُحدِثَ بالعيار ═══
    # (AR) 🔑 والمجسُّ يُنزِلُ السقفَ لا يُنمّي خطَّ الأساس: نموُّه يُشعِلُ
    #      «انحدارًا» معه، وإنزالُ السقفِ يقيسُ **أنّه يُقرَأُ ويُقابَل**.
    ("⑥ خطُّ الأساسِ فوقَ سقفِه يُحمِّر",
     GUARD_REL, sub(b"CEILING_BASELINE_TOTAL = 37",
                    b"CEILING_BASELINE_TOTAL = 36  # zz_probe_lower_ceiling",
                    residue=True),
     1, _only(_OVER_CEIL)),

    # ═══ ④ العمى يُبلَغُ برمزِ ٢ ولا يتنكّرُ إنجازًا ═══
    # (AR) 🔑 وقِيسَ: بلا هذا الفرعِ كانت المصارفُ العمياءُ تُطبَعُ «كُنِست
    #      مواضعُ» — دعوى تقدُّمٍ عن مسحٍ لم يقعْ أصلًا.
    # (AR) 🔑 **ولكلِّ قارئٍ مجسُّه.** أرضيّةٌ على مجموعِ النداءاتِ كانت
    #      تُخفي موتَ قارئَين: `consume` وحدَه ١٥٧ فيبقى المجموعُ فوقَها بينما
    #      يعمى `error` و`cerr` معًا (١٨ مخالفةً من ٣٧). فصارَ العيارُ ذاتيًّا
    #      لكلِّ قارئ، ولكلِّ قارئٍ مجسٌّ يُعمّيه.
    ("⑦ قارئُ `consume` أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(rb'(?<![A-Za-z0-9_])consume\s*\(\s*[^,()]*',
                    rb'(?<![A-Za-z0-9_])zz_probe_blind_consume\s*\(\s*[^,()]*',
                    residue=True),
     2, _only(_BLIND)),

    ("⑧ قارئُ مصارفِ التشخيصِ أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'SINKS = ("error", "errorAt"',
                    b'SINKS = ("zz_probe_blind_error", "errorAt"', residue=True),
     2, _only(_BLIND)),

    ("⑨ قارئُ `std::cerr` أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(rb"_CERR = re.compile(r'std::cerr",
                    rb"_CERR = re.compile(r'zz_probe_blind_cerr",
                    residue=True),
     2, _only(_BLIND)),

    # (AR) 🔑 **والعيّنةُ السالبةُ تمنعُ الرضا بنمطٍ وُسِّعَ حتّى طابقَ كلَّ شيء**
    #      — وهو عمًى من الجهةِ الأخرى: يُحمِّرُ على كلِّ نداءٍ سليم.
    ("⑩ قارئٌ وُسِّعَ حتّى رأى سالبتَه — رمزُ آلةٍ ٢",
     GUARD_REL, sub(rb"_CERR = re.compile(r'std::cerr\s*<<",
                    rb"_CERR = re.compile(r'std::c\w+\s*<<  # zz_probe_widened",
                    residue=True),
     2, _only(_BLIND)),

    # (AR) وأرضيّةُ الحضورِ البنيويِّ طرفٌ ثالث: مجلَّدٌ يُقرأُ ولا `consume` فيه.
    ("⑪ نداءاتُ `consume` دونَ أرضيّتِها — رمزُ آلةٍ ٢",
     GUARD_REL, sub(rb"_ANY_CONSUME = re.compile(r'(?<![A-Za-z0-9_])consume",
                    rb"_ANY_CONSUME = re.compile(r'(?<![A-Za-z0-9_])zz_no_consume",
                    residue=True),
     2, _only(_FEW_CONSUME)),

    ("⑫ لا ملفَّ مصدرٍ في النطاقِ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'SCOPE.rglob("*.cpp")',
                    b'SCOPE.rglob("*.zz_probe_no_such_suffix")', residue=True),
     2, _only(_NO_FILES)),

    ("⑬ نطاقُ المحلّلِ مفقودٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'SCOPE = ROOT / "shared" / "parser" / "src"',
                    b'SCOPE = ROOT / "shared" / "zz_probe_no_parser"',
                    residue=True),
     2, _only(_NO_SCOPE)),

    # ═══ ⑤ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    # (AR) 🔑 **والنداءُ في تعليقٍ ليس نداءً** — ولولا تجريدُ التعليقاتِ لَحمِرَ
    #      الحارسُ على مثالٍ مكتوبٍ شرحًا، وهو رفضٌ كاذبٌ يُطفَأُ به الحارس.
    ("⑭ مثالٌ في تعليقٍ لا يُحمِّر",
     OOP, sub(CATALOG_CALL,
              _before_call('// error("زز مجسّ في تعليق");'), residue=True),
     0, _green()),

    # (AR) 🔑 **و`//` داخلَ سلسلةٍ ليست تعليقًا.** وترتيبُ البدائلِ في مجرّدِ
    #      التعليقاتِ هو الحارسُ على ذلك: بغيرِه يُمحى بقيّةُ السطرِ ويُفلِتُ
    #      الخرقُ الذي بعدَه. والمجسُّ يضعُ الخرقَ **خلفَ** رابطٍ في سلسلة.
    ("⑮ خرقٌ بعدَ `//` داخلَ سلسلةٍ لا يُفلِت",
     OOP, sub(CATALOG_CALL,
              _before_call('const char *zz = "انظر http://x"; '
                           'error("زز مجسّ خلفَ الرابط");'), residue=True),
     1, _only(_REGRESSION)),

    # (AR) وفاصلٌ أو رمزٌ تزيينيٌّ ليس نثرًا — وعدُّه خرقًا يُحمِّرُ على سليم.
    ("⑯ سلسلةٌ بلا حرفٍ هجائيٍّ ليست نثرًا",
     OOP, sub(CATALOG_CALL, _before_call('error("── :: ──");'), residue=True),
     0, _green()),

    ("⑰ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     GUARD_REL, eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`).
MIN_PROBES = 17


# ═══ المِحقنةُ إعلانٌ: حارسٌ · عنوانٌ · صفُّ مجسّات ═════════════════════════
_HARNESS = Harness(
    guard=GUARD,
    harness=HARNESS,
    record=RECORD,
    title="عيارُ حارسِ «لا نصَّ تشخيصٍ خامٍّ في المحلّل» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    baseline=None,
    # (AR) وملفّاتٌ يقرؤها الحارسُ في كلِّ تشغيلةٍ ولا يُطفِّرُها مجسّ
    #      تبقى بلا بصمةٍ لولا هذا الإعلان — والدائرةُ تُغلَق.
    depends=("scripts/codegen/_lib/guard_exit.py",),
    # (AR) وأرضيّتُه أرضيّةُ عمًى لا مسطرةُ تغطية: عددُ نداءاتِ المصارفِ يرتفعُ
    #      وينخفضُ بكلِّ كنسٍ مشروع، فشدُّها إلى المقيسِ يجعلُ أوّلَ كنسةٍ
    #      رفضًا كاذبًا — وهو نقيضُ ما وُجِدَ له الحارس.
    floors="slack",
    has_bounds=True,
)

if __name__ == "__main__":
    run(_HARNESS)
