#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ **«حجمُ النوعِ يُشتَقُّ ولا يُكتَب»** — يُحقَنُ الجدولُ
     اليدويُّ ويُثبَتُ الاحمرار.

     الحارسُ يدَّعي أنّ `@حجم` في المترجّمِ يقرأُ الجدولَ المولَّدَ من
     `language-truth/types.yaml` ولا يحملُ جدولًا مكتوبًا باليد. والدعوى بلا
     احمرارٍ مُثبَتٍ ليست حراسة — وهذا الحارسُ لم يكن مُعايَرًا قطّ.

     🔑 والمجسّاتُ تحقنُ **العطبَ الذي وقعَ فعلًا**: سلسلةُ `if/else if`
        بسلاسلَ عربيّةٍ خامّةٍ تباعدَت عن نظيرتِها في **٦ من ١١** نوعًا.
        وتحقنُ كذلك ما يجعلُ الحارسَ **رفضًا كاذبًا** لو أساءَ القياس: تعليقٌ
        عربيٌّ في الجسمِ · وسلسلةٌ عربيّةٌ **خارجَ** الجسمِ في الملفِّ نفسِه —
        وكلاهما يجبُ أن يبقى أخضرَ، وإلّا أُطفِئَ الحارسُ عندَ أوّلِ شكوى.

     🔑 **وعُويِرَ رمزُ الخروجِ كذلك**: تعذُّرُ بلوغِ جسمِ الدالّةِ عطبُ آلةٍ
        (رمزُ ٢) لا حكمٌ على المحتوى (رمزُ ١) — وكان يخرجُ ١، فحارسٌ لم يقرأْ
        جسمًا واحدًا يُقرأُ في CI «وجدَ جدولًا يدويًّا»، والعلاجانِ مختلفان.

(EN) Injection harness for the @حجم-derivation guard: inject a hand-written
     Arabic size table, prove the guard reddens, restore the bytes exactly.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ** — والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import CRLF, LF_, Harness, eol_to_lf, run, sub  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_sizeof_from_sot.py"
HARNESS = Path(__file__).resolve()
RECORD = ROOT / "scripts" / "codegen" / "calibration" / "check_sizeof_from_sot.yaml"


def _run_guard():
    return _HARNESS.run_guard()


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()


# ═══ أهدافُ الحقن ═══════════════════════════════════════════════════════
GUARD_REL = "scripts/codegen/check_sizeof_from_sot.py"
SITE = "compiler/src/frontend/builders/expression_lowlevel.cpp"
LF = chr(10)

# (AR) مرساةٌ داخلَ **جسمِ الدالّةِ** بعينِه — أوّلُ سطرٍ تنفيذيٍّ فيه.
IN_BODY = "const std::string &typeName = sizeofExpr->typeName;".encode("utf-8")
# (AR) ومرساةٌ **خارجَه**: سطرُ التوقيعِ يقعُ قبلَ القوسِ المفتوح.
SIGNATURE = ("BuildResult ExpressionBuilder::buildExprSizeof"
             "(AST::SizeofExpr *sizeofExpr)").encode("utf-8")
DERIVES = "::Sad::Types::sadTypeKindSizeBytes(kind)".encode("utf-8")


def _in_body(extra: str) -> bytes:
    """(AR) يُدخِلُ نصًّا في أوّلِ الجسمِ — فيُقاسُ ما يُقاسُ حقًّا."""
    return IN_BODY + b"  " + extra.encode("utf-8")




# (AR) 🔑 **والنصُّ المنتظَرُ وحدَه لا يكفي.** صفوفُ العدَّاداتِ تُطبَعُ في
#      كلِّ تشغيلةٍ خضراءَ كانت أو حمراء، فالمرساةُ **سطرُ العلاجِ** (`⇐ …`)
#      وهو لا يظهرُ إلّا عندَ الإخفاق. ويُنفى ما سواه: لولا النفيُ لَمرَّ مجسٌّ
#      على حمرةِ لامتغيِّرٍ آخر — وقد بُرهنَ ذلك في مِحقنةٍ شقيقةٍ بإعدامِ
#      لامتغيِّرٍ بالكاملِ وبقاءِ العيارِ ١٢/١٢ ورمزِ صفر.
_STRINGS = "⇐ جدولُ أحجامٍ مكتوبٌ باليدِ عاد"
_NO_CALL = "⇐ لا نداءَ لـ"
_NUMBERS = "⇐ عددٌ حرفيٌّ في جسمِ"
_NO_SITE = "لا موقعَ يُقاس"
_NO_FILE = "ملفٌّ مفقود"
_NO_SIGN = "لم يُعثَر على توقيعِ الدالّة"
_NO_BODY = "توقيعٌ بلا جسمٍ"
_UNCLOSED = "جسمٌ غيرُ مُغلَق"
_ALL_MARKS = (_STRINGS, _NO_CALL, _NUMBERS, _NO_SITE, _NO_FILE, _NO_SIGN,
              _NO_BODY, _UNCLOSED)


def _only(mark: str):
    return (mark, tuple(other for other in _ALL_MARKS if other != mark))


def _truncate_after_brace(blob: bytes) -> bytes:
    """(AR) يقطعُ الملفَّ عندَ أوّلِ قوسٍ بعدَ التوقيع، فيبقى الجسمُ غيرَ مُغلَق.

    🔑 وأوّلُ صياغةٍ للمجسِّ كانت **قوسًا زائدًا داخلَ الجسم**، فقِيسَ أنّها
       تقيسُ غيرَ ما تدَّعي: الموازنةُ تجدُ قوسًا مُغلِقًا لاحقًا في الملفِّ
       فتردُّ جسمًا **أوسعَ** فيه سلاسلُ دوالَّ أُخرى ⇒ رمزُ ١ لا ٢. والقطعُ
       عندَ الطرفِ وحدَه يجعلُ فرعَ «غيرُ مُغلَق» مبلوغًا حقًّا."""
    at = blob.find(SIGNATURE)
    if at < 0:
        raise AssertionError("المرساةُ غيرُ موجودة: توقيعُ الدالّة")
    brace = blob.find(b"{", at)
    if brace < 0:
        raise AssertionError("المرساةُ غيرُ موجودة: قوسُ الجسم")
    return blob[:brace + 1] + ("  // zz_probe_unclosed_body" + LF).encode("utf-8")


_truncate_after_brace.residue = b"zz_probe_unclosed_body"


def _truncate_at_signature(blob: bytes) -> bytes:
    """(AR) يقطعُ الملفَّ عندَ نهايةِ التوقيعِ فلا يبقى قوسُ جسمٍ أصلًا."""
    at = blob.find(SIGNATURE)
    if at < 0:
        raise AssertionError("المرساةُ غيرُ موجودة: توقيعُ الدالّة")
    return (blob[:at + len(SIGNATURE)]
            + ("  // zz_probe_no_body_at_all" + LF).encode("utf-8"))


_truncate_at_signature.residue = b"zz_probe_no_body_at_all"


def sub_bytes(old: bytes, new: bytes):
    """(AR) استبدالٌ بسِمةِ أثرٍ **مُشتقّةٍ من النصِّ المحقون** — كـ`sub` سواء،
    ويُفرَدُ ليُقرأَ النصُّ متعدِّدَ الأسطرِ في موضعِه."""
    return sub(old, new.replace(b"\\n", LF.encode("utf-8")), residue=True)


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    # ═══ ① جدولٌ عربيٌّ مكتوبٌ باليدِ يعود ═══
    # (AR) وهو العطبُ الذي وقعَ فعلًا: سلسلةُ `if/else if` بأسماءِ الأنواعِ
    #      العربيّةِ نصًّا — نسخةٌ ثانيةٌ من جدولِ الأنواعِ تُباعِدُ عن الأصل.
    ("① سلسلةٌ عربيّةٌ في جسمِ الدالّة",
     SITE,
     # (AR) وبلا عددٍ حرفيّ: الحقنةُ الأولى كانت `return 48;` فأشعلَت
     #      اللامتغيِّرَينِ معًا، ونفيُ «ما سوى» ردَّها — والمجسُّ يجبُ أن
     #      يعضَّ على **واحدٍ** يدَّعيه لا على اثنَين.
     sub(IN_BODY,
         _in_body('if (typeName == "خريطة") return kSadTypeSizeUnknown;'),
         residue=True),
     1, _only(_STRINGS)),

    # (AR) وحرفٌ عربيٌّ **واحدٌ** يكفي — والمرساةُ صنفُ المحرَفِ لا لفظٌ بعينِه.
    ("② حرفٌ عربيٌّ واحدٌ في سلسلةٍ يكفي",
     SITE,
     sub(IN_BODY, _in_body('const char *zz_probe_one = "ص";'), residue=True),
     1, _only(_STRINGS)),

    ("③ سلسلتانِ تُعَدّانِ اثنتَينِ لا واحدة",
     SITE,
     sub(IN_BODY, _in_body('const char *zz_a = "أ"; const char *zz_b = "ب";'),
         residue=True),
     1, _only(_STRINGS)),

    # ═══ ② الاشتقاقُ يُثبَتُ حضورًا لا غيابًا وحدَه ═══
    # (AR) 🔑 ولولا هذا الشرطُ لمرَّ **جسمٌ فارغٌ** أخضرَ: لا سلسلةَ فيه ولا
    #      اشتقاق. والمجسُّ ينزعُ النداءَ ويُبقي الباقي.
    ("④ نزعُ نداءِ الاشتقاقِ يُحمِّر",
     SITE,
     # (AR) 🔑 وسِمةُ الأثرِ **مميَّزةٌ عمدًا**: `kSadTypeSizeUnknown` وحدَه
     #      لفظٌ **مشروعٌ قائمٌ** في الموضعِ نفسِه، فاتّخاذُه سِمةً يجعلُ كاشفَ
     #      الأثرِ يُبلِّغُ عن شجرةٍ نظيفةٍ — رفضٌ كاذبٌ يُوقِفُ العيارَ كلَّه.
     #      (وقعَ فعلًا في أوّلِ تشغيلةٍ لهذه المِحقنة.)
     sub(DERIVES, "::Sad::Types::kSadTypeSizeUnknown"
         " /* zz_probe_no_derive */".encode("utf-8"), residue=True),
     1, _only(_NO_CALL)),

    # (AR) والاسمُ المطلوبُ في الحارسِ طرفٌ ثانٍ للعقدِ نفسِه: انجرافُه عن
    #      المترجّمِ يجبُ أن يُحمِّرَ لا أن يمرَّ صامتًا.
    ("⑤ اسمُ النداءِ المطلوبِ ينجرفُ عن المترجّم",
     GUARD_REL,
     sub('REQUIRED_CALL = "sadTypeKindSizeBytes"'.encode("utf-8"),
         'REQUIRED_CALL = "sadTypeKindSizeBytesZZ"'.encode("utf-8"),
         residue=True),
     1, _only(_NO_CALL)),

    # ═══ عطبُ الآلةِ يُبلَغُ برمزِ ٢ لا ١ ═══
    # (AR) 🔑 وكان يخرجُ **١** — و`x.py` يقرؤه حكمًا على المحتوى. فحارسٌ لم
    #      يقرأْ جسمًا واحدًا كان يُقرأُ «وجدَ جدولًا يدويًّا»، والعلاجانِ
    #      مختلفانِ تمامًا: تحديثُ توقيعٍ في الحارسِ مقابلَ حذفِ جدولٍ من المترجّم.
    ("⑥ توقيعٌ أُعيدت تسميتُه — رمزُ آلةٍ ٢",
     SITE,
     sub(SIGNATURE, SIGNATURE.replace(b"buildExprSizeof", b"buildExprSizeofZZ"),
         residue=True),
     2, _only(_NO_SIGN)),

    ("⑦ موقعٌ مفقودٌ — رمزُ آلةٍ ٢",
     GUARD_REL,
     sub('"expression_lowlevel.cpp"'.encode("utf-8"),
         '"zz_no_such_file.cpp"'.encode("utf-8"), residue=True),
     2, _only(_NO_FILE)),

    ("⑧ جسمٌ غيرُ مُغلَقٍ — رمزُ آلةٍ ٢",
     SITE, _truncate_after_brace, 2, _only(_UNCLOSED)),

    # ═══ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    # (AR) 🔑 **والتعليقُ العربيُّ ليس جدولًا.** لولا نزعِ التعليقاتِ لَحمِرَ
    #      الحارسُ على شرحِ العلّةِ المكتوبِ في الموضعِ نفسِه — رفضٌ كاذبٌ
    #      يُطفَأُ به الحارسُ عندَ أوّلِ شكوى، وهو أسوأُ من غيابِه.
    ("⑨ تعليقٌ عربيٌّ سطريٌّ في الجسمِ لا يُحمِّر",
     SITE,
     sub(IN_BODY, _in_body('// zz_probe: "خريطة" 48' + LF), residue=True),
     0, "لا جدولَ يدويًّا فيه"),

    ("⑩ تعليقٌ عربيٌّ كتليٌّ في الجسمِ لا يُحمِّر",
     SITE,
     sub(IN_BODY, _in_body('/* zz_probe "ص" */'), residue=True),
     0, "لا جدولَ يدويًّا فيه"),

    # (AR) 🔑 **والمقيسُ جسمُ الدالّةِ لا الملفّ.** الملفُّ فيه تشخيصاتٌ
    #      عربيّةٌ مشروعةٌ ودوالُّ أُخرى، فمسحٌ على الملفِّ كلِّه يحمرُّ كذبًا.
    ("⑪ سلسلةٌ عربيّةٌ خارجَ الجسمِ لا تُحمِّر",
     SITE,
     sub(SIGNATURE,
         ('static const char *zz_probe_outside = "ص";' + LF
          + "            ").encode("utf-8") + SIGNATURE, residue=True),
     0, "لا جدولَ يدويًّا فيه"),

    ("⑫ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     GUARD_REL, eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),

    # ═══ ما لم يكن مبلوغًا قطّ ═══
    # (AR) 🔑 **صفرُ موقعٍ كان يُخرِجُ رمزَ صفرٍ وسطرَ نجاح** — حارسٌ أخضرُ إلى
    #      الأبد. والصفُّ نزلَ فعلًا من موقعَينِ إلى واحدٍ حينَ زالَ محرّكُ
    #      المفسّر، فالإزالةُ التاليةُ كانت ستُفرِغُه بلا صوت.
    ("⑬ صفُّ المواقعِ خالٍ — رمزُ آلةٍ ٢",
     GUARD_REL,
     sub_bytes(b"SITES = (", b"SITES = ()  # zz_probe_empty_sites\nZZ_OLD = ("),
     2, _only(_NO_SITE)),

    # (AR) 🔑 **والجدولُ يعودُ بلا حرفٍ عربيٍّ واحد**: `case Map: return 48;`
    #      يمرُّ على شرطِ السلاسلِ ويحملُ ذِكرًا ميّتًا للنداءِ فيمرُّ على الثاني.
    ("⑭ جدولٌ بأعدادٍ حرفيّةٍ بلا عربيّةٍ يُحمِّر",
     SITE,
     sub(IN_BODY, _in_body("if (kind == SadTypeKind::Map) return 48;"),
         residue=True),
     1, _only(_NUMBERS)),

    # (AR) 🔑 **والذِّكرُ ليس نداءً**: الاسمُ حاضرٌ في تعليقٍ أو في شرحٍ ولا
    #      نداءَ له — وكان الشرطُ عضويّةً نصّيّةً فيمرّ.
    ("⑮ ذِكرُ الاسمِ بلا نداءٍ ليس اشتقاقًا",
     SITE,
     sub(DERIVES,
         ("::Sad::Types::kSadTypeSizeUnknown"
          " /* zz_probe_mention_only sadTypeKindSizeBytes */").encode("utf-8"),
         residue=True),
     1, _only(_NO_CALL)),

    # (AR) وفرعُ «توقيعٌ بلا جسمٍ» كان مسلكَ عطبٍ بلا مجسّ.
    ("⑯ توقيعٌ بلا جسمٍ — رمزُ آلةٍ ٢",
     SITE, _truncate_at_signature, 2, _only(_NO_BODY)),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`).
MIN_PROBES = 16


# ═══ المِحقنةُ إعلانٌ: حارسٌ · عنوانٌ · صفُّ مجسّات ═════════════════════════
_HARNESS = Harness(
    guard=GUARD,
    harness=HARNESS,
    record=RECORD,
    title="عيارُ حارسِ «حجمُ النوعِ يُشتَقُّ ولا يُكتَب» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    # (AR) ولا مرجعَ يُقاسُ قبلَ الحقن: حدودُ هذا الحارسِ صفريّةٌ بطبعِها
    #      (لا سقفَ ينزلُ ولا أرضيّةَ تصعد)، فلا عددَ يُطفَّرُ منه.
    baseline=None,
    floors="none",
    # (AR) وصارَ له سقفٌ واحد (`CEILING_LITERAL_NUMBERS`) بعدَ أن أُضيفَ
    #      شرطُ «لا عددَ حرفيًّا في الجسم». 🔑 **والإعلانُ التقطَ بلاهُ بنفسِه**:
    #      كان `has_bounds=False` فأخفقَ العيارُ بـ«يُعلِنُ بلا حدودٍ ومخرَجُه
    #      يحملُ حدًّا» — وهو المقصودُ من قياسِ الإعلانِ في الاتّجاهَين.
    has_bounds=True,
)

if __name__ == "__main__":
    run(_HARNESS)
