#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ **«إحالةُ رمزٍ في القواعدِ لها وجودٌ في الكتالوج»**.

     الحارسُ يدَّعي أنّ كلَّ `terminal.ref` و`separator` في
     `language-truth/grammar/*.yaml` يُحَلُّ من أحدِ المصادرِ الثلاثةِ التي
     يُعلِنُها `_notation.yaml:terminal_ref_sources`. والدعوى بلا احمرارٍ
     مُثبَتٍ ليست حراسة — ولم يكن هذا الحارسُ مُعايَرًا قطّ.

     🔑 **وأوّلُ قياسٍ قبلَ أوّلِ مجسٍّ كشفَ أنّ مصدرَينِ من الثلاثةِ كانا
        ميّتَين**: قارئُ المعرّفاتِ تعبيرانِ نمطيّانِ على بدايةِ السطر،
        و`keywords.yaml` يكتبُ بنودَه خرائطَ سائلةً في سطرٍ واحدٍ بحقلِ
        `tokenType` — فالمقيسُ **معرّفٌ واحدٌ من ٢٥١**. فما تُجيزُه الوثيقةُ
        حرفًا (`ref: KW-CTX-003`) كان **رفضًا كاذبًا**، وهو أسوأُ من الانهيار.

     ومجسّاتُ هذه المِحقنةِ تحقنُ العطبَ في وجوهِه الثلاثة:
       · **حكمٌ على المحتوى** (رمز ١): إحالةٌ شبحيّةٌ في `ref` وفي `separator`.
       · **عطبُ آلةٍ** (رمز ٢): مصدرٌ ميّتٌ · ملفٌّ مفقودٌ في طرفَي القراءةِ
         (YAML وترويسةُ C) · إعلانٌ غائبٌ · بندٌ بلا حقلٍ · YAML لا يُحلَّلُ ·
         بنيةٌ غيرُ متوقَّعةٍ · قارئُ إحالاتٍ أعمى · مجلَّدٌ لا يُمسَح.
       · **وما يجبُ ألّا يُحمِّر**: الإحالةُ في تعليقٍ ليست إحالة، والإحالةُ
         التي لا يحلُّها إلّا `keywords.yaml` تمرُّ خضراء.

(EN) Injection harness for the grammar terminal-ref guard: prove it reddens on
     ghost refs, exits 2 (not 1) on every unread-corpus path, and does not
     falsely reject a commented example or a keywords-only identifier.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ** — والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import Harness, append, eol_to_lf, run, sub  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_grammar_terminal_refs.py"
HARNESS = Path(__file__).resolve()
RECORD = (ROOT / "scripts" / "codegen" / "calibration"
          / "check_grammar_terminal_refs.yaml")


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()


# ═══ أهدافُ الحقن ═══════════════════════════════════════════════════════
GUARD_REL = "scripts/codegen/check_grammar_terminal_refs.py"
GRAMMAR = "language-truth/grammar/60_advanced.yaml"
NOTATION = "language-truth/grammar/_notation.yaml"
KEYWORDS = "language-truth/keywords.yaml"
LF = chr(10)
BS = chr(92)
Q = chr(34)
R = chr(114)

# (AR) مراسٍ حقيقيّةٌ في الشجرة — لا نصٌّ مُتخيَّل.
REF_LINE = '- { kind: terminal, ref: TYPE_INTEGER, value: "رقم" }'.encode("utf-8")
SEP_LINE = "separator: ARABIC_COMMA".encode("utf-8")
SRC_KEYWORDS = "- source: keywords.yaml".encode("utf-8")
SRC_OPERATORS = "- source: operators.yaml".encode("utf-8")
SRC_HEADER = "- source: shared/lexer/include/token.h".encode("utf-8")
DECLARATION = "terminal_ref_sources:".encode("utf-8")


# (AR) ومراسي نمطِ الفاصلِ تُبنى بلا محرَفِ هروبٍ حرفيّ: الشفرةُ تُنقَلُ عبرَ
#      صدفاتٍ تبتلعُ الخطَّ المائلَ فتنكسرُ بصمتًا.
_SEP_ANCHOR = (R + Q + "^" + BS + "s*separator:").encode("utf-8")
_SEP_BLIND = (R + Q + "^" + BS
              + "s*zz_probe_blind_separator:").encode("utf-8")
_SEP_WIDENED = (R + Q + "[a-z_]*" + BS
                + "s*separator:").encode("utf-8")


def _ref(name: str) -> bytes:
    """(AR) يُبدِّلُ الإحالةَ في السطرِ المرساةِ وحدَه — والباقي كما هو."""
    return REF_LINE.replace(b"TYPE_INTEGER", name.encode("utf-8"))


# (AR) 🔑 **والنصُّ المنتظَرُ وحدَه لا يكفي.** صفوفُ العدَّاداتِ تُطبَعُ في كلِّ
#      تشغيلةٍ خضراءَ كانت أو حمراء، فالمرساةُ **سطرُ العلاجِ أو عطبِ الآلةِ**
#      وهو لا يظهرُ إلّا عندَ الإخفاق. ويُنفى ما سواه: لولا النفيُ لَمرَّ مجسٌّ
#      على حمرةِ لامتغيِّرٍ آخرَ فيُقرأَ عيارًا وهو لم يقِسْ ما ادَّعى.
_GHOST = "⇐ الإحالةُ يجب أن تكون اسمَ رمزٍ"
_DEAD = "لا يُساهِمُ باسمٍ واحد"
_MISSING = "ملفٌّ مفقود"
_NO_SRC = "غائبٌ أو خالٍ في _notation.yaml"
_BAD_ENTRY = "بندُ مصدرٍ بلا حقل"
_BAD_YAML = "لا يُحلَّلُ YAML"
_BAD_DOC = "بنيةُ `_notation.yaml` غيرُ متوقَّعة"
_FEW_FILES = "لم يُمسَحْ شيء"
_FEW_REFS = "قارئُ الإحالاتِ أعمى"
_FEW_SEPS = "قارئٌ لا يُبصِر"
_ALL_MARKS = (_GHOST, _DEAD, _MISSING, _NO_SRC, _BAD_ENTRY, _BAD_YAML,
              _BAD_DOC, _FEW_FILES, _FEW_REFS, _FEW_SEPS)

# (AR) وسطرُ السلامةِ مرساةُ الخضرةِ — ويُنفى معه كلُّ مَرقَبٍ أحمر.
_CLEAN = "✓ كلُّ إحالاتِ الرموزِ"


def _only(mark: str):
    return (mark, tuple(other for other in _ALL_MARKS if other != mark))


def _green():
    return (_CLEAN, _ALL_MARKS)


def _blank_keywords(_blob: bytes) -> bytes:
    """(AR) معجمٌ بلا معرّفٍ ولا نوعِ رمز — مصدرٌ مُعلَنٌ لا يُساهِمُ بشيء."""
    return ('version: "zz_probe_dead_source"' + LF).encode("utf-8")


_blank_keywords.residue = b'version: "zz_probe_dead_source"'


def _notation_as_list(_blob: bytes) -> bytes:
    """(AR) وثيقةُ اصطلاحٍ بنيتُها صفٌّ لا خريطة — عطبُ آلةٍ لا حكمُ محتوى."""
    return ("- zz_probe_notation_is_a_list" + LF).encode("utf-8")


_notation_as_list.residue = b"zz_probe_notation_is_a_list"


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    # ═══ ① حكمٌ على المحتوى: الإحالةُ الشبحيّة ═══
    # (AR) وهو العطبُ الذي وقعَ فعلًا: `OP_COMMA` في ٢٢ موضعًا ولا وجودَ له.
    ("① إحالةٌ شبحيّةٌ في `ref` تُحمِّر",
     GRAMMAR, sub(REF_LINE, _ref("ZZ_GHOST_TERMINAL"), residue=True),
     1, _only(_GHOST)),

    # (AR) و`separator` طرفٌ ثانٍ للقاعدةِ نفسِها — مسحُه فرعٌ مستقلٌّ في
    #      القارئ، وسقوطُه يمرُّ صامتًا لولا مجسٌّ عليه.
    ("② إحالةٌ شبحيّةٌ في `separator` تُحمِّر",
     GRAMMAR, sub(SEP_LINE, b"separator: ZZ_GHOST_SEPARATOR", residue=True),
     1, _only(_GHOST)),

    # ═══ ② عطبُ الآلةِ: المصدرُ المُعلَنُ الميّت ═══
    # (AR) 🔑 وهو العطبُ الذي كان قائمًا: مصدرانِ يردّانِ اسمًا واحدًا من ٢٥١،
    #      فتُقرأُ الوثيقةُ ثلاثةَ مصادرَ والقياسُ مصدرٌ واحد.
    ("③ مصدرٌ مُعلَنٌ لا يُساهِمُ باسمٍ — رمزُ آلةٍ ٢",
     KEYWORDS, _blank_keywords, 2, _only(_DEAD)),

    # (AR) وطرفا القراءةِ فرعانِ مختلفانِ في الشفرة: YAML وترويسةُ C — ولكلٍّ
    #      مجسُّه، فسقوطُ أحدِهما لا يُغطّيه الآخر.
    ("④ مصدرُ YAML مفقودٌ — رمزُ آلةٍ ٢",
     NOTATION, sub(SRC_OPERATORS, b"- source: zz_no_operators.yaml",
                   residue=True),
     2, _only(_MISSING)),

    ("⑤ ترويسةُ الرموزِ مفقودةٌ — رمزُ آلةٍ ٢",
     NOTATION, sub(SRC_HEADER,
                   b"- source: shared/lexer/include/zz_no_token.h",
                   residue=True),
     2, _only(_MISSING)),

    # ═══ ③ الإعلانُ نفسُه: غيابُه وعطبُه ═══
    # (AR) 🔑 والمصادرُ **تُشتَقُّ** من الوثيقةِ ولا تُهجّى في الحارس، فغيابُ
    #      الإعلانِ عمًى لا سلامة.
    ("⑥ إعلانُ المصادرِ غائبٌ — رمزُ آلةٍ ٢",
     NOTATION, sub(DECLARATION, b"zz_probe_no_sources_declared:", residue=True),
     2, _only(_NO_SRC)),

    ("⑦ بندُ مصدرٍ بلا حقل `source` — رمزُ آلةٍ ٢",
     NOTATION, sub(SRC_KEYWORDS, b"- zz_probe_wrong_key: keywords.yaml",
                   residue=True),
     2, _only(_BAD_ENTRY)),

    ("⑧ وثيقةُ الاصطلاحِ لا تُحلَّلُ — رمزُ آلةٍ ٢",
     NOTATION, append((LF + "zz_probe_bad_yaml: [unclosed" + LF).encode("utf-8")),
     2, _only(_BAD_YAML)),

    ("⑨ بنيةُ وثيقةِ الاصطلاحِ غيرُ متوقَّعةٍ — رمزُ آلةٍ ٢",
     NOTATION, _notation_as_list, 2, _only(_BAD_DOC)),

    # ═══ ④ العمى: قارئٌ لا يقرأُ ومجلَّدٌ لا يُمسَح ═══
    # (AR) 🔑 **وصفرُ إحالةٍ كان يُقرأُ ✅ ورمزَ صفر.** صيغةُ YAML تتحوّلُ من
    #      السائلةِ إلى الكتليّةِ فيعمى النمطُ، والحارسُ يشهدُ بالسلامة.
    ("⑩ نمطُ الإحالاتِ أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b're.compile(r"kind:', b're.compile(r"zz_probe_blind_kind:',
                    residue=True),
     2, _only(_FEW_REFS)),

    # (AR) ومجلَّدٌ أُعيدت تسميتُه يردُّ صفرَ ملفٍّ — وصفرٌ ليس سلامة.
    ("⑪ لا ملفَّ قواعدَ يُمسَحُ — رمزُ آلةٍ ٢",
     GUARD_REL, sub('مجلد_القواعد.glob("*.yaml")'.encode("utf-8"),
                    'مجلد_القواعد.glob("*.zz_probe_no_such_suffix")'
                    .encode("utf-8"), residue=True),
     2, _only(_FEW_FILES)),

    # ═══ ⑤ اختباراتُ انحدارٍ — يجبُ أن تبقى خضراء ═══
    # (AR) 🔑 **والإحالةُ في تعليقٍ ليست إحالة.** و`_notation.yaml` موضعُ
    #      المثالِ بطبعِه، فالفخُّ قائمٌ لا نظريّ: مثالٌ مكتوبٌ شرحًا كان
    #      سيصيرُ رفضًا كاذبًا يُطفَأُ به الحارسُ عندَ أوّلِ شكوى.
    ("⑫ إحالةٌ شبحيّةٌ في تعليقٍ لا تُحمِّر",
     GRAMMAR,
     sub(REF_LINE,
         ("# مثالٌ للشرح: { kind: terminal, ref: ZZ_GHOST_IN_COMMENT }" + LF
          + "              ").encode("utf-8") + REF_LINE, residue=True),
     0, _green()),

    # (AR) 🔑 **وإحالةٌ لا يحلُّها إلّا `keywords.yaml` تمرُّ خضراء** — وهذا
    #      المجسُّ يعضُّ من طرفَين: إن عادَ القارئُ إلى تعبيرَيه النمطيَّينِ
    #      صارَ المعجمُ مصدرًا ميّتًا (رمز ٢)، وإن سقطَ حقلُ `id` وحدَه صارت
    #      الإحالةُ شبحيّةً (رمز ١) — وكلاهما يُخفِقُ ههنا.
    # (AR) 🔑 وسِمةُ الأثرِ تحملُ وسمَ `zz` **وإن كانت القيمةُ نصًّا عربيًّا
    #      مشروعَ الشكل**: السِّمةُ ههنا سطرُ إحالةٍ صالحٌ تمامًا، فلو كُتِبَ
    #      نظيرُه يومًا في هذا الملفِّ لأبلغَ كاشفُ الأثرِ عن «أرضيّةٍ ملوّثةٍ»
    #      على شجرةٍ نظيفةٍ وأوقفَ العيارَ كلَّه. (كشفَته مراجعةٌ خصميّة.)
    ("⑬ إحالةٌ من معرّفاتِ المعجمِ وحدَها لا تُحمِّر",
     GRAMMAR, sub(REF_LINE,
                  _ref("KW-CTX-003").replace('value: "رقم"'.encode("utf-8"),
                                             b'value: "zz_probe_kw_only"'),
                  residue=True),
     0, _green()),

    # (AR) 🔑 **وقارئُ الفواصلِ طرفٌ ثانٍ لا يحرسُه مجموعُ الإحالات**: الفواصلُ
    #      واحدٌ بالمئة، فموتُه كاملًا يُبقي المجموعَ فوقَ أرضيّتِه ولا يُرى.
    # (AR) 🔑 وأرضيّةُ الفواصلِ استُبدِلَت بعيارٍ ذاتيّ: خمسةٌ لا غير، فأرضيّةٌ
    #      عليها إمّا عمياءُ (لو جُمِعَت) وإمّا رفضٌ كاذبٌ عندَ أوّلِ إعادةِ صياغة.
    ("⑭ قارئُ الفواصلِ لا يُبصِر — رمزُ آلةٍ ٢",
     GUARD_REL, sub(_SEP_ANCHOR, _SEP_BLIND, residue=True),
     2, _only(_FEW_SEPS)),

    # (AR) 🔑 **والمشيُ المُنطَّقُ حامِلٌ لا زينة**: كسرُ نطاقِه يُفرِغُ المعجمَ
    #      فيصيرُ مصدرًا ميّتًا — وهو ما يُثبِتُ أنّ التنطيقَ يُقرَأُ فعلًا.
    ("⑮ نطاقُ مشيِ المعجمِ مكسورٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub('وثيقة.get("categories")'.encode("utf-8"),
                    'وثيقة.get("zz_probe_no_categories")'.encode("utf-8"),
                    residue=True),
     2, _only(_DEAD)),

    # (AR) 🔑 **والعيّنةُ السالبةُ تمنعُ الرضا بنمطٍ وُسِّعَ حتّى طابقَ كلَّ شيء**
    #      — وهو عمًى من الجهةِ الأخرى: يُحمِّرُ على كلِّ حقلٍ ينتهي بـ`separator`.
    ("⑯ قارئٌ وُسِّعَ حتّى رأى سالبتَه — رمزُ آلةٍ ٢",
     GUARD_REL, sub(_SEP_ANCHOR, _SEP_WIDENED, residue=True),
     2, _only(_FEW_SEPS)),

    # (AR) 🔑 **وشكلُ المخطَّطِ يُتحقَّقُ منه ولا يُفترَض**: `categories` صفًّا
    #      بدلَ خريطةٍ كان يُخرِجُ `AttributeError` غيرَ ملتقَطٍ ⇒ رمزُ ١، وهو
    #      عينُ العطبِ الذي يُعلِنُ الحارسُ أنّه سدَّه. (كشفَته مراجعةٌ خصميّة.)
    ("⑰ `categories` صفٌّ لا خريطةٌ — رمزُ آلةٍ ٢ لا انهيارٌ برمزِ ١",
     KEYWORDS, sub("categories:".encode("utf-8"),
                   ("categories:" + LF + "  - zz_probe_categories_is_a_list"
                    + LF + "zz_probe_old_categories:").encode("utf-8"),
                   residue=True),
     2, _only(_DEAD)),

    ("⑱ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     GUARD_REL, eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`).
MIN_PROBES = 18


# ═══ المِحقنةُ إعلانٌ: حارسٌ · عنوانٌ · صفُّ مجسّات ═════════════════════════
_HARNESS = Harness(
    guard=GUARD,
    harness=HARNESS,
    record=RECORD,
    title="عيارُ حارسِ «إحالةُ رمزٍ في القواعدِ لها وجودٌ في الكتالوج» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    baseline=None,
    # (AR) 🔑 **أرضيّاتُه أرضيّاتُ عمًى لا مسطرةُ تغطيةٍ فلا تُشَدّ.** القواعدُ
    #      تُصاغُ وتُدمَجُ فيرتفعُ عددُ الإحالاتِ وينخفضُ بعملٍ مشروع، وشدُّ
    #      الأرضيّةِ إلى المقيسِ يجعلُ أوّلَ إعادةِ صياغةٍ رفضًا كاذبًا — وهو
    #      عينُ ما وقعَ في حارسِ المرساةِ حينَ وحَّدَ القلبُ قارئَ الحدود.
    floors="slack",
    has_bounds=True,
)

if __name__ == "__main__":
    run(_HARNESS)
