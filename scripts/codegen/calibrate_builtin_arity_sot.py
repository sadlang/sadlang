#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ **«رتبةُ المدمجِ تُعلَن مرّةً واحدةً في مصدرِ الحقيقة»**.

     الحارسُ يحرسُ من أربعةِ أطرافٍ (رقمٌ حرفيّ · ثابتٌ غريبٌ · إعلانٌ ميّتٌ ·
     فرضٌ غيرُ مُعلَن) وخامسٍ استُحدِثَ قبلَه (ذراعُ رفضٍ لا تسجّلُ خطأً
     فيتبخّرُ النداء). ولم يكن مُعايَرًا قطّ.

     🔑 **وما كشفَه العيارُ في الحارسِ نفسِه (٢٠٢٦-٠٩-٠٦):**

      ① **بابُ «لا يبلغُها المترجّم» كان يبتلعُ الإخفاق.** كان الجدولُ
         المُولَّدُ يُقرأُ تحتَ `if exists()`، فغيابُه يُفرِغُ الأهدافَ المبلوغةَ
         ويُعادُ **تصنيفُ** كلِّ عقدٍ إلى «يفرضُه المفسّرُ ولا يبلغُه
         المترجّم» — سطرُ إحصاءٍ لا شكوى. وقِيسَ حيًّا: بإخفاءِ
         `builtin_arity_generated.h` صارَ العددُ ٢٢ ⇐ ٩١ **والرمزُ صفر**.
         يتبخّرُ نصفُ الحراسةِ والحارسُ يشهدُ ✓. صارَ الجدولُ مرجعًا واجبًا،
         وصارَ لبابِ الهروبِ **سقفٌ نازلٌ** فلا يتّسعُ صامتًا.

      ② **و«يُطبَع بعددِه ولا يُبتَلَع» كان نصفَ الوعد.** الدَّينُ يُعلَنُ فعلًا
         (٧٦٧ من ١٣١٩) لكنّه **لا يُحاكَم**: مدمجٌ جديدٌ بلا فرضِ رتبةٍ يرفعُ
         العددَ ويمرُّ أخضر. فصارَ للديونِ الثلاثةِ سقوفٌ نازلة.

      ③ **ولا رمزَ ٢ في الحارسِ كلِّه.** مستخرِجٌ يعمى يجعلُ خمسَمئةِ إعلانٍ
         حيٍّ تُقرأُ «عقدًا لا يُطبَّق» — رفضٌ كاذبٌ عريضٌ برمزِ ١ يوجّهُ
         المُصلِحَ إلى مصدرِ الحقيقةِ والعطبُ في أداةِ القياس. وكذلك غيابُ
         `type_methods.yaml` كان **رجوعًا صامتًا** يُفرِغُ سطحًا ثانيًا كاملًا.

(EN) Injection harness for the builtin-arity SoT guard: prove all five content
     invariants and the three new descending ceilings bite, and that every
     unread-corpus path now exits 2 instead of degrading into a green tally.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ** — والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import Harness, eol_to_lf, run, sub  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_builtin_arity_sot.py"
HARNESS = Path(__file__).resolve()
RECORD = (ROOT / "scripts" / "codegen" / "calibration"
          / "check_builtin_arity_sot.yaml")


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()


# ═══ أهدافُ الحقن ═══════════════════════════════════════════════════════
GUARD_REL = "scripts/codegen/check_builtin_arity_sot.py"
EXTRACTOR = "scripts/codegen/builtin_arity_extract.py"
ARM = "compiler/src/frontend/builders/builtins_math.cpp"
SOT_MATH = "language-truth/builtins/math.yaml"
SOT_ASSERT = "language-truth/builtins/assertions.yaml"
LF = chr(10)

# (AR) مراسٍ حقيقيّةٌ في الشجرة — لا نصٌّ مُتخيَّل.
SQRT_CALL = ("if (!checkBuiltinArity(b_.errors_, funcName, Ar::Math::SQRT,"
             " argResults.size()))").encode("utf-8")
ARM_ENTRY = ("                std::vector<SIROperand> &argOperands)" + LF
             + "            {").encode("utf-8")
SQRT_ARITY = ("- cpp_id: SQRT" + LF + "  canonical: جذر" + LF
              + "  namespace: Math" + LF + "  arity:" + LF
              + "    min: 1" + LF + "    max: 1").encode("utf-8")
NEQ_ENTRY = ("- cpp_id: ASSERT_NEQ" + LF
             + "  canonical: تأكد_لا_يساوي").encode("utf-8")


# (AR) 🔑 **والنصُّ المنتظَرُ وحدَه لا يكفي.** سطورُ العدَّاداتِ تُطبَعُ في
#      الخضرةِ وحدَها، لكنّ شكاوى الحارسِ تتجاورُ في تشغيلةٍ واحدة — فمجسٌّ
#      يرسو على شكوى قد يمرُّ على حمرةِ شكوى أُخرى. ويُنفى ما سوى المُدَّعى.
_LITERALS = "أرقامًا حرفيّةً في مواضعِ الفحص"
_FOREIGN = "تقيسُ بثابتِ مدمجٍ آخر"
_DEAD = "لا تفرضُها ذراعٌ"
_UNMEASURED = "بلا `arity` في مصدرِ الحقيقة"
_SILENT = "أذرعُ رفضٍ لا تسجّلُ خطأً"
_OVER_UNENF = "لا يفرضُ رتبتَها أحد"
_OVER_INLINE = "رتبٌ مفروضةٌ برقمٍ في الشرط"
_OVER_UNREACH = "عقودٌ لا يبلغُها المترجّم"
_NO_TABLE = "جدولُ الرتبِ المُولَّدُ مفقود"
_NO_ROWS = "بلا صفٍّ واحد"
_NO_TM = "طرائقُ الأنواعِ مفقودة"
_NO_SOT = "لا ملفَّ مدمجاتٍ"
_NO_SITES = "مستخرِجُ الأذرعِ أعمى"
_BAD_ARITY = "إعلانُ رتبةٍ معطوبٌ"
_NO_REACH = "لا هدفَ تبلغُه أذرعُ الإرسال"
_ALL_MARKS = (_LITERALS, _FOREIGN, _DEAD, _UNMEASURED, _SILENT, _OVER_UNENF,
              _OVER_INLINE, _OVER_UNREACH, _NO_TABLE, _NO_ROWS, _NO_TM,
              _NO_SOT, _NO_SITES, _BAD_ARITY, _NO_REACH)

_CLEAN = "✓ لا رقمَ حرفيًّا"


def _only(mark: str):
    return (mark, tuple(other for other in _ALL_MARKS if other != mark))


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    # ═══ ① الأطرافُ الخمسةُ للحكمِ على المحتوى ═══
    # (AR) وهو العطبُ الأصليّ: رتبةٌ مكتوبةٌ رقمًا في الذراعِ — نسخةٌ ثانيةٌ
    #      للحقيقةِ تنجرفُ بلا أن يحمرَّ شيء.
    ("① رقمٌ حرفيٌّ في موضعِ فحصٍ يُحمِّر",
     ARM, sub(SQRT_CALL, SQRT_CALL.replace(b"Ar::Math::SQRT", b"1, 1")
              + b"  // zz_probe_literal_arity", residue=True),
     1, _only(_LITERALS)),

    # (AR) والثابتُ الغريبُ أخفى من الرقم: رتبةٌ صحيحةُ الشكلِ خاطئةُ المرجع،
    #      تنجرفُ بصمتٍ حينَ تتغيّرُ رتبةُ المدمجِ الآخر.
    # (AR) 🔑 وسِمةُ الأثرِ **مميَّزةٌ عمدًا**: النصُّ المحقونُ ههنا هو بعينِه
    #      سطرُ ذراعِ `LOG` **المشروعُ القائمُ** في الملفِّ نفسِه، فاتّخاذُه
    #      سِمةً يجعلُ كاشفَ الأثرِ يُبلِّغُ عن شجرةٍ نظيفةٍ ويوقفُ العيارَ كلَّه.
    #      (وقعَ فعلًا في أوّلِ تشغيلةٍ لهذه المِحقنة، كما وقعَ في شقيقتِها.)
    ("② ذراعٌ تقيسُ بثابتِ مدمجٍ آخرَ تُحمِّر",
     ARM, sub(SQRT_CALL, SQRT_CALL.replace(b"Ar::Math::SQRT", b"Ar::Math::LOG")
              + b"  // zz_probe_foreign_constant", residue=True),
     1, _only(_FOREIGN)),

    # (AR) وإعلانٌ في مصدرِ الحقيقةِ لا تفرضُه ذراعٌ عقدٌ لا يُطبَّقُ أبدًا.
    ("③ إعلانُ رتبةٍ لا تفرضُه ذراعٌ يُحمِّر",
     SOT_ASSERT,
     sub(NEQ_ENTRY, NEQ_ENTRY + (LF + "  arity:" + LF + "    min: 2" + LF
                                 + "    max: 2  # zz_probe_dead_arity"
                                 ).encode("utf-8"), residue=True),
     1, _only(_DEAD)),

    # (AR) وفرضٌ بلا إعلانٍ يجعلُ الحقيقةَ في الشيفرةِ لا في مصدرِها.
    ("④ ذراعٌ تفرضُ رتبةً بلا إعلانٍ تُحمِّر",
     ARM,
     sub(SQRT_CALL,
         ("if (funcName == Bm::ZZ_PROBE_UNDECLARED)" + LF
          + "                    {" + LF
          + "                        if (!checkBuiltinArity(b_.errors_, funcName,"
          " Ar::Math::ZZ_PROBE_UNDECLARED, argResults.size()))" + LF
          + "                            return BuildResult(\"\","
          " SadTypeKind::Float);" + LF
          + "                    }" + LF
          + "                    ").encode("utf-8") + SQRT_CALL, residue=True),
     1, _only(_UNMEASURED)),

    # (AR) 🔑 والذراعُ الخامسة: رفضٌ لا يسجّلُ خطأً — النداءُ يتبخّرُ والمصرّفُ
    #      يخرجُ بصفرٍ ويُنتجُ ثنائيًّا **بلا العمل**. أخطرُ من الرقمِ الحرفيِّ
    #      بمراتب: ذاك عقدٌ ينجرف، وهذا عملٌ يختفي.
    ("⑤ ذراعُ رفضٍ لا تسجّلُ خطأً تُحمِّر",
     ARM,
     sub(ARM_ENTRY, ARM_ENTRY
         + (LF + "                if (argResults.size() < 1) { return"
            " BuildResult(); }  // zz_probe_silent_arm").encode("utf-8"),
         residue=True),
     1, _only(_SILENT)),

    # ═══ ② السقوفُ الثلاثةُ النازلةُ — وهي التي استُحدِثَت بالعيار ═══
    # (AR) والمجسُّ يُنزِلُ السقفَ لا يُنمّي الدَّين: نموُّ الدَّينِ يُغيِّرُ
    #      عدَّةَ عدّاداتٍ معًا فيُشعِلُ لامتغيِّرَين، وإنزالُ السقفِ يقيسُ
    #      **أنّه يُقرَأُ ويُقابَل** — وهو المُدَّعى.
    ("⑥ دَينُ «لا يفرضُها أحد» فوقَ سقفِه يُحمِّر",
     GUARD_REL, sub(b"CEILING_UNENFORCED = 767",
                    b"CEILING_UNENFORCED = 766  # zz_probe_lower_unenforced",
                    residue=True),
     1, _only(_OVER_UNENF)),

    ("⑦ دَينُ الرقمِ في الشرطِ فوقَ سقفِه يُحمِّر",
     GUARD_REL, sub(b"CEILING_INLINE_ONLY = 1",
                    b"CEILING_INLINE_ONLY = 0  # zz_probe_lower_inline",
                    residue=True),
     1, _only(_OVER_INLINE)),

    # (AR) 🔑 وهذا هو بابُ الهروبِ الذي كان يبتلعُ الإخفاقَ بلا سقف.
    ("⑧ بابُ «لا يبلغُها المترجّم» فوقَ سقفِه يُحمِّر",
     GUARD_REL, sub(b"CEILING_UNREACHABLE_METHODS = 22",
                    b"CEILING_UNREACHABLE_METHODS = 21  # zz_probe_lower_unreach",
                    residue=True),
     1, _only(_OVER_UNREACH)),

    # ═══ ③ عطبُ الآلةِ يُبلَغُ برمزِ ٢ ولا يتنكّرُ إحصاءً ═══
    # (AR) 🔑 وقِيسَ قبلَ العلاج: ٢٢ ⇐ ٩١ **ورمزُ صفر**.
    ("⑨ جدولُ الرتبِ المُولَّدُ مفقودٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'/ "builtin_arity_generated.h"',
                    b'/ "zz_probe_no_arity_table.h"', residue=True),
     2, _only(_NO_TABLE)),

    ("⑩ جدولُ الرتبِ بلا صفٍّ واحدٍ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'_TABLE_ROW = re.compile(r\'\\{"(\\w+)",',
                    b'_TABLE_ROW = re.compile(r\'zz_probe_no_row\\{"(\\w+)",',
                    residue=True),
     2, _only(_NO_ROWS)),

    ("⑪ قارئُ `lookup` أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'"TypeMethods::lookup(" not in text',
                    b'"zz_probe_no_such_lookup(" not in text', residue=True),
     2, _only(_NO_REACH)),

    # (AR) 🔑 وغيابُ طرائقِ الأنواعِ كان **رجوعًا صامتًا** يُفرِغُ سطحًا ثانيًا.
    ("⑫ طرائقُ الأنواعِ مفقودةٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'"language-truth" / "type_methods.yaml"',
                    b'"language-truth" / "zz_probe_no_type_methods.yaml"',
                    residue=True),
     2, _only(_NO_TM)),

    ("⑬ مصدرُ حقيقةِ المدمجاتِ فارغٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'"language-truth" / "builtins"',
                    b'"language-truth" / "zz_probe_no_builtins"', residue=True),
     2, _only(_NO_SOT)),

    # (AR) 🔑 ومستخرِجٌ أعمى كان يجعلُ خمسَمئةِ إعلانٍ حيٍّ «عقدًا لا يُطبَّق» —
    #      رفضٌ كاذبٌ عريضٌ برمزِ ١ يوجّهُ إلى الملفِّ الخطأ.
    ("⑭ مستخرِجُ الأذرعِ أعمى — رمزُ آلةٍ ٢",
     EXTRACTOR, sub(b'BUILDERS = ROOT / "compiler" / "src" / "frontend"'
                    b' / "builders"',
                    b'BUILDERS = ROOT / "compiler" / "zz_probe_no_builders"',
                    residue=True),
     2, _only(_NO_SITES)),

    # (AR) وحقلٌ ناقصٌ في إعلانٍ عطبُ آلةٍ يُسمّى، لا `KeyError` أثرًا رجعيًّا.
    ("⑮ إعلانُ رتبةٍ معطوبٌ في مصدرِ الحقيقةِ — رمزُ آلةٍ ٢",
     SOT_MATH, sub(SQRT_ARITY,
                   SQRT_ARITY.replace(b"    max: 1",
                                      "    max: زز_مجسّ".encode("utf-8")),
                   residue=True),
     2, _only(_BAD_ARITY)),

    # ═══ ④ اختبارُ انحدارٍ — يجبُ أن يبقى أخضر ═══
    # (AR) 🔑 **والذِّكرُ في تعليقٍ ليس فرضًا ولا رقمًا حرفيًّا.** ومستخرِجُ
    #      المواضعِ يمحو التعليقاتِ بمسافاتٍ تحفظُ المواضعَ لهذا السبب — ولولاه
    #      لَحمِرَ الحارسُ على شرحٍ مكتوبٍ في الذراعِ نفسِها: رفضٌ كاذبٌ يُطفَأُ
    #      به الحارسُ عندَ أوّلِ شكوى، وهو أسوأُ من غيابِه.
    #      (وكانت المِحقنةُ كلُّها حمراءَ الطرفِ بلا مجسٍّ أخضرَ واحد —
    #       كشفَته مراجعةٌ خصميّة.)
    ("⑯ رقمٌ حرفيٌّ في تعليقٍ لا يُحمِّر",
     ARM, sub(SQRT_CALL,
              b"// zz_probe_in_comment: checkBuiltinArity(b_.errors_,"
              b" funcName, 1, 1, argResults.size())" + LF.encode("utf-8")
              + b"                    " + SQRT_CALL, residue=True),
     0, (_CLEAN, _ALL_MARKS)),

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
    title="عيارُ حارسِ «رتبةُ المدمجِ من مصدرِ الحقيقة» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    baseline=None,
    # (AR) لا أرضيّةَ في مخرَجِه — ثلاثةُ سقوفٍ نازلةٍ وحدَها، وكلُّها مشدودةٌ
    #      اليومَ (١ · ٢٢ · ٧٦٧) ونزولُها بالعملِ لا بالقلم.
    floors="none",
    has_bounds=True,
)

if __name__ == "__main__":
    run(_HARNESS)
