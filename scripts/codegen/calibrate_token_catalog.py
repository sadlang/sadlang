#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ **«اكتمالُ كتالوجِ المفردات»** — يُقابِلُ تعدادَ
     `KEYWORD_*` في `token.h` بسجلّاتِ `tokenType` في `keywords.yaml`.

     الحارسُ يدَّعي ثلاثةَ لامتغيِّراتٍ ورابعًا استُحدِثَ بالعيار:
       ① لا فجوةَ **جديدة**: رمزٌ في الترويسةِ غيرُ مُسجَّلٍ وغيرُ مُعلَنٍ دَينًا.
       ② لا بندَ دَينٍ **بائتًا**: سُجِّلَ في الكتالوجِ أو زالَ من الترويسة.
       ③ لا رمزَ **شبحٍ**: الكتالوجُ يشيرُ إلى ما لا وجودَ له في الترويسة.
       ④ **والدَّينُ ينكمشُ ولا ينمو** — سقفٌ نازلٌ استُحدِثَ ههنا.

     🔑 **وما كشفَه العيارُ في الحارسِ نفسِه (٢٠٢٦-٠٩-٠٦):**

      · **«القائمةُ تنكمشُ فقط» كانت دعوى بلا قياس.** الترويسةُ تقولُها منذُ
        اليومِ الأوّل، ولا شيءَ كان يمنعُ سطرًا جديدًا: يُعلَنُ الرمزُ غيرُ
        المُسجَّلِ دَينًا فيخضرُّ الحارسُ في الحال — بابُ هروبٍ مفتوحٌ من
        اللامتغيِّرِ الأوّلِ نفسِه. فصارَ لها **سقفٌ نازلٌ يُقاس**.

      · **وعددُها كان منثورًا فبلِيَ**: «مثبَّتة … ١٨ رمزًا» والمقيسُ **١٦** —
        خرجَ بندانِ بتسجيلٍ لغويٍّ وبقيَ النثرُ. نسخةٌ ثانيةٌ لحقيقةٍ تحتَها
        مباشرةً بلا حارس؛ حُذِفَ العددُ وصارَ يُشتَقّ.

      · **وعطبُ الآلةِ كان يخرجُ برمزِ ١ ويُغرِقُ تشخيصَه.** الاستخراجُ الفارغُ
        كان يُلحَقُ ببقيّةِ الشكاوى **بعدَها**، فترويسةٌ لا تُقرأُ تجعلُ
        `catalog − header` كلَّ الكتالوج: ثمانونَ سطرَ «رمزٌ شبح» عن رموزٍ
        سليمة، ورمزُ ١ يقرؤه `x.py` حكمًا على المحتوى. صارَ يُقالُ **أوّلًا**
        وبرمزِ ٢، ومعه غيابُ الملفَّينِ وعطبُ تحليلِ YAML.

(EN) Injection harness for the token-catalog guard: prove all four content
     invariants bite, that the newly-added shrink-only debt ceiling is read,
     and that every unread-corpus path exits 2 before it can print phantoms.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ** — والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import Harness, append, eol_to_lf, run, sub  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_token_catalog.py"
HARNESS = Path(__file__).resolve()
RECORD = ROOT / "scripts" / "codegen" / "calibration" / "check_token_catalog.yaml"


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()


# ═══ أهدافُ الحقن ═══════════════════════════════════════════════════════
GUARD_REL = "scripts/codegen/check_token_catalog.py"
HEADER = "shared/lexer/include/token.h"
CATALOG = "language-truth/keywords.yaml"
LF = chr(10)

# (AR) مراسٍ حقيقيّةٌ في الشجرة — لا نصٌّ مُتخيَّل.
ENUM_LINE = ("            KEYWORD_COMPTIME,  ///< وقت_ترجمة / comptime"
             " (تقييم وقت الترجمة - compile-time evaluation)").encode("utf-8")
DEBT_ENUM = ("            KEYWORD_MUT_PTR,   ///< *متغير / mut_ptr"
             " (مؤشر متغير - mutable pointer)").encode("utf-8")
CATALOG_LINE = ('      - { id: "KW-CTX-041", subcategory: "low_level",'
                ' since: "1.0.0", word: "تجميع", tokenType: KEYWORD_ASM,'
                " english: asm, roles: [block_opener] }").encode("utf-8")
# (AR) وذيلُ التعدادِ: آخرُ عنصرٍ بلا فاصلةٍ ثمّ قوسُ الإغلاق — وهو الموضعُ
#      الذي وُضِعَ من أجلِه بديلُ `}` في نمطِ الحارس.
ENUM_TAIL = ("            WHITESPACE   ///< مسافة (عادة تُتجاهل) / whitespace"
             + LF + "        };").encode("utf-8")
NEW_ENUM_TAIL = (
    "            WHITESPACE,  ///< مسافة (عادة تُتجاهل) / whitespace" + LF
    + "            KEYWORD_ZZ_PROBE_LAST   ///< zz_probe_last_enumerator" + LF
    + "        };").encode("utf-8")


def _after_enum(extra: str) -> bytes:
    """(AR) يُدخِلُ عنصرَ تعدادٍ **بعدَ** مرساةٍ قائمة — داخلَ التعدادِ نفسِه."""
    return ENUM_LINE + (LF + "            " + extra).encode("utf-8")


def _after_catalog(extra: str) -> bytes:
    """(AR) يُدخِلُ بندَ كتالوجٍ بعدَ مرساةٍ قائمة — في القائمةِ نفسِها."""
    return CATALOG_LINE + (LF + "      " + extra).encode("utf-8")


# (AR) 🔑 **والنصُّ المنتظَرُ وحدَه لا يكفي.** سطرُ العدَّاداتِ يُطبَعُ في كلِّ
#      تشغيلةٍ خضراءَ كانت أو حمراء، فالمرساةُ **سطرُ الشكوى** وحدَه — ويُنفى
#      ما سواه، وإلّا مرَّ مجسٌّ على حمرةِ لامتغيِّرٍ آخرَ فقُرِئَ عيارًا.
_NEW_GAP = "رمز جديد في token.h غير مُسجَّل"
_STALE_CATALOGED = "أصبح مُسجَّلًا في الكتالوج"
_STALE_GONE = "لم يعُد مُعرَّفًا في token.h"
_PHANTOM = "رمز شبح في الكتالوج"
_OVER_CEIL = "فوقَ السقفِ"
_BLIND_HDR = "تغيّرَ شكلُ التعداد"
_BLIND_CAT = "تغيّرَ المخطّط"
_NO_HDR = "ترويسةُ الرموزِ مفقودة"
_NO_CAT = "كتالوجُ المفرداتِ مفقود"
_BAD_YAML = "لا يُحلَّلُ YAML"
_ALL_MARKS = (_NEW_GAP, _STALE_CATALOGED, _STALE_GONE, _PHANTOM, _OVER_CEIL,
              _BLIND_HDR, _BLIND_CAT, _NO_HDR, _NO_CAT, _BAD_YAML)

_CLEAN = "لا فجوة جديدة"


def _only(mark: str):
    return (mark, tuple(other for other in _ALL_MARKS if other != mark))


def _green():
    return (_CLEAN, _ALL_MARKS)


def _unparsable_catalog(_blob: bytes) -> bytes:
    """(AR) كتالوجٌ لا يُحلَّل — عطبُ آلةٍ لا حكمُ محتوى."""
    return ("zz_probe_catalog_bad_yaml: [unclosed" + LF).encode("utf-8")


_unparsable_catalog.residue = b"zz_probe_catalog_bad_yaml"


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    # ═══ ① الفجوةُ الجديدة — وهي الدَّينُ الذي وُجِدَ الحارسُ ليمنعَ اتّساعَه ═══
    ("① رمزٌ جديدٌ في التعدادِ بلا تسجيلٍ ولا إعلانِ دَينٍ يُحمِّر",
     HEADER, sub(ENUM_LINE, _after_enum("KEYWORD_ZZ_PROBE_NEW,"), residue=True),
     1, _only(_NEW_GAP)),

    # ═══ ② بندُ الدَّينِ البائت — بوجهَيه ═══
    # (AR) وجهٌ أوّل: سُجِّلَ في الكتالوجِ فوجبَ حذفُه من القائمة.
    ("② بندُ دَينٍ صارَ مُسجَّلًا في الكتالوجِ يُحمِّر",
     CATALOG,
     sub(CATALOG_LINE,
         _after_catalog('- { id: "ZZ-PROBE-01", word: "زز",'
                        " tokenType: KEYWORD_LET, english: zz }"),
         residue=True),
     1, _only(_STALE_CATALOGED)),

    # (AR) ووجهٌ ثانٍ: زالَ من الترويسةِ فوجبَ حذفُه كذلك. والتعليقُ **بدلَ**
    #      إعادةِ التسميةِ عمدًا: إعادةُ التسميةِ تُنشئُ رمزًا جديدًا فتُشعِلُ
    #      لامتغيِّرًا ثانيًا، والمجسُّ يجبُ أن يعضَّ على واحدٍ يدَّعيه.
    ("③ بندُ دَينٍ زالَ من التعدادِ يُحمِّر",
     HEADER, sub(DEBT_ENUM, b"//" + DEBT_ENUM.lstrip(), residue=True),
     1, _only(_STALE_GONE)),

    # ═══ ③ الرمزُ الشبح ═══
    ("④ رمزٌ في الكتالوجِ لا وجودَ له في التعدادِ يُحمِّر",
     CATALOG,
     sub(CATALOG_LINE,
         _after_catalog('- { id: "ZZ-PROBE-02", word: "شبح",'
                        " tokenType: KEYWORD_ZZ_PROBE_PHANTOM, english: zz }"),
         residue=True),
     1, _only(_PHANTOM)),

    # ═══ ④ السقفُ النازلُ — اللامتغيِّرُ الذي استُحدِثَ بالعيار ═══
    # (AR) 🔑 والمجسُّ يُنزِلُ **السقفَ** لا يُنمّي القائمة: كلُّ رموزِ الترويسةِ
    #      غيرِ المُسجَّلةِ مُعلَنةٌ دَينًا اليوم، فأيُّ بندٍ يُضافُ يكونُ بائتًا
    #      فيُشعِلُ لامتغيِّرًا ثانيًا. وإنزالُ السقفِ يقيسُ **أنّه يُقرَأُ
    #      ويُقابَل** — وهو المُدَّعى.
    ("⑤ الدَّينُ فوقَ سقفِه يُحمِّر",
     GUARD_REL, sub(b"CEILING_INHERITED_DEBT = 16",
                    b"CEILING_INHERITED_DEBT = 15  # zz_probe_lower_ceiling",
                    residue=True),
     1, _only(_OVER_CEIL)),

    # ═══ ⑤ عطبُ الآلةِ يُبلَغُ برمزِ ٢ قبلَ أن يُغرِقَ تشخيصَه ═══
    # (AR) 🔑 وكان يخرجُ **١** ومعه ثمانونَ سطرَ «رمزٌ شبح» عن رموزٍ سليمة.
    ("⑥ قارئُ التعدادِ أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'KEYWORD_PREFIX = "KEYWORD_"',
                    b'KEYWORD_PREFIX = "ZZ_PROBE_BLIND_KEYWORD_"', residue=True),
     2, _only(_BLIND_HDR)),

    ("⑦ قارئُ الكتالوجِ أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'node.get("tokenType")',
                    b'node.get("zz_probe_blind_tokenType")', residue=True),
     2, _only(_BLIND_CAT)),

    ("⑧ ترويسةُ الرموزِ مفقودةٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'"include" / "token.h"',
                    b'"include" / "zz_probe_no_token.h"', residue=True),
     2, _only(_NO_HDR)),

    ("⑨ كتالوجُ المفرداتِ مفقودٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'"language-truth" / "keywords.yaml"',
                    b'"language-truth" / "zz_probe_no_keywords.yaml"',
                    residue=True),
     2, _only(_NO_CAT)),

    ("⑩ كتالوجُ المفرداتِ لا يُحلَّلُ — رمزُ آلةٍ ٢",
     CATALOG, _unparsable_catalog, 2, _only(_BAD_YAML)),

    # ═══ ⑥ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    # (AR) 🔑 **والذِّكرُ في تعليقٍ ليس عنصرَ تعداد.** ولولا إسقاطُ التعليقاتِ
    #      لَحمِرَ الحارسُ على شرحٍ مكتوبٍ في الترويسةِ نفسِها — رفضٌ كاذبٌ
    #      يُطفَأُ به الحارسُ عندَ أوّلِ شكوى.
    ("⑪ رمزٌ في تعليقٍ سطريٍّ ليس عنصرَ تعدادٍ",
     HEADER, sub(ENUM_LINE, _after_enum("// KEYWORD_ZZ_PROBE_IN_LINE,"),
                 residue=True),
     0, _green()),

    ("⑫ رمزٌ في تعليقٍ كتليٍّ ليس عنصرَ تعدادٍ",
     HEADER, sub(ENUM_LINE, _after_enum("/* KEYWORD_ZZ_PROBE_IN_BLOCK, */"),
                 residue=True),
     0, _green()),

    # (AR) 🔑 **وآخرُ عنصرٍ في تعدادِ C++ يجوزُ بلا فاصلةٍ لاحقة** — ولهذا
    #      يقبلُ نمطُ الحارسِ قوسَ الإغلاقِ بديلًا عن الفاصلة، وترويسةُ الحارسِ
    #      تقولُ ذلك حرفًا. والدعوى بلا مجسٍّ نثرٌ: هذا المجسُّ يضعُ الرمزَ في
    #      **الموضعِ الأخيرِ حقًّا** (قبلَ `};` بلا فاصلة) فيُقاسُ البديلُ.
    #      (وأوّلُ صياغةٍ وضعَته في الوسطِ فمرَّ أخضرَ — والمجسُّ كان يقيسُ
    #       غيرَ ما يدَّعي: عنصرٌ بلا فاصلةٍ في الوسطِ ليس C++ صحيحةً أصلًا.)
    ("⑬ عنصرٌ جديدٌ في الموضعِ الأخيرِ قبلَ `};` لا يُفلِتُ",
     HEADER, sub(ENUM_TAIL, NEW_ENUM_TAIL, residue=True),
     1, _only(_NEW_GAP)),

    ("⑭ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     GUARD_REL, eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`).
MIN_PROBES = 14


# ═══ المِحقنةُ إعلانٌ: حارسٌ · عنوانٌ · صفُّ مجسّات ═════════════════════════
_HARNESS = Harness(
    guard=GUARD,
    harness=HARNESS,
    record=RECORD,
    title="عيارُ حارسِ «اكتمالُ كتالوجِ المفردات» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    baseline=None,
    # (AR) وملفّاتٌ يقرؤها الحارسُ في كلِّ تشغيلةٍ ولا يُطفِّرُها مجسّ
    #      تبقى بلا بصمةٍ لولا هذا الإعلان — والدائرةُ تُغلَق.
    depends=("scripts/codegen/_lib/guard_exit.py",),
    # (AR) لا أرضيّةَ في مخرَجِه — سقفُ الدَّينِ وحدَه، وهو **مشدودٌ** اليومَ
    #      (١٦ = ١٦) ونزولُه بالعملِ لا بالقلم.
    floors="none",
    has_bounds=True,
)

if __name__ == "__main__":
    run(_HARNESS)
