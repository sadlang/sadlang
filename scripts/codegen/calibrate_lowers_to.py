#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ **«ربطُ القواعدِ بأوپكوداتِ SIR»** — البوّابةُ الثالثة.

     الحارسُ يقيسُ خمسةَ لامتغيِّرات: طزاجةَ الكتالوجِ · تغطيةَ القواعدِ في
     `lowers_to.yaml` · مفرداتِ الأوپكوداتِ · انجرافَ `isel.yaml` عن تعدادِ
     المترجّمِ · ومطابقةَ المعماريّاتِ المخفوضةِ لقائمةِ `targets.yaml`.
     ولم يكن مُعايَرًا قطّ.

     🔑 **وما كشفَه العيارُ في الحارسِ نفسِه (٢٠٢٦-٠٩-٠٦):**

      ① **لامتغيِّراتُه كلُّها فروقُ مجموعات — وفرقُ خاليَينِ خالٍ.** قواعدُ
         صفرٌ ومداخلُ صفرٌ يُخرِجانِ «✅ الحارس سليم» ورمزَ صفر؛ وكتالوجٌ بلا
         أوپكودٍ واحدٍ يجعلُ فحصَ المفرداتِ يمرُّ **خاويًا**. فصارت أرضيّتا
         عمًى (قواعدُ · أوپكودات) وعطبُ آلةٍ برمزِ ٢ قبلَ كلِّ حكم.

      ② **والمرجعُ المفقودُ كان يخرجُ برمزِ ١.** كتالوجٌ أو `lowers_to` غيرُ
         موجودٍ يُقرأُ في `x.py` حكمًا على المحتوى، والعلاجُ إعادةُ توليدٍ لا
         إصلاحُ قاعدة.

      ③ **وقائمةُ دَينِ `isel` كانت تُقرأُ في اتّجاهٍ واحد.** اسمٌ خارجَها
         يُفشِل، أمّا بندٌ **بلِيَ** فيها (صارَ الاسمُ في التعدادِ أو زالَ ذِكرُه
         من `isel`) فيبقى صامتًا يُجيزُ عودةَ الانجرافِ غدًا.

      ④ **و«غيرُ المقيسِ» دَينٌ بلا سقف.** لا يُفشِلُ إلّا مع
         `--require-derived` — وهي ليست ما يشغّلُه `x.py`. فصارَ له سقفٌ نازلٌ
         يُقاسُ في كلِّ تشغيلة.

      ⑤ **ونصُّ الانجرافِ كان نصَّ الدَّينِ المُعلَنِ حرفًا** — جملةٌ واحدةٌ
         تُطبَعُ في الحمرةِ وفي الخضرةِ سواء، فمجسٌّ يرسو عليها يمرُّ على غيرِ
         ما يدَّعي. فصارَ للانجرافِ ذيلٌ يُميّزُه.

(EN) Injection harness for the lowers_to gate: prove all five content
     invariants bite, that the two new floors and the ceiling are read, and
     that an unread corpus exits 2 instead of passing as an empty difference.
============================================================================
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ** — والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import Harness, eol_to_lf, run, sub  # noqa: E402

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_lowers_to.py"
HARNESS = Path(__file__).resolve()
RECORD = ROOT / "scripts" / "codegen" / "calibration" / "check_lowers_to.yaml"


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()


# ═══ أهدافُ الحقن ═══════════════════════════════════════════════════════
GUARD_REL = "scripts/codegen/check_lowers_to.py"
GRAMMAR = "language-truth/grammar/00_program.yaml"
LOWERS = "language-truth/grammar/lowers_to.yaml"
TARGETS = "language-truth/backend/targets.yaml"
ISEL = "language-truth/backend/arm64/isel.yaml"
LF = chr(10)

# (AR) مراسٍ حقيقيّةٌ في الشجرة — لا نصٌّ مُتخيَّل.
PRODUCTIONS = ("productions:" + LF).encode("utf-8")
FIRST_ROW = ("  - id: gr.program.program" + LF + "    status: derived" + LF
             + "    lowers_to: [ADD_I64, ALLOC, BUILTIN_PRINT, LOAD,"
             " RET_VOID, STORE]").encode("utf-8")
ISEL_FIRST = ("patterns:" + LF + "  - sir: ADD_I64").encode("utf-8")
# (AR) وهدفٌ **مخطَّطٌ** يُرقّى «مخفوضًا» بلا مخفّض — الدعوى بلا سندٍ في
#      المترجّم. ويُختارُ المخطَّطُ لا المخفوضُ عمدًا: إعادةُ تسميةِ مخفوضٍ
#      تُشعِلُ لامتغيِّرَين (كذبُ الوسمِ ومعماريّةٌ خارجَ القائمة).
TARGET_PLANNED = ("  - id: armv7" + LF + '    name_ar: "ARMv7-A/Thumb-2"' + LF
                  + '    milestone: "م٧"' + LF + "    order: 4" + LF
                  + "    status: planned").encode("utf-8")
# (AR) وكتلةُ riscv64 بوسمِها: تُعادُ تسميتُها **وتُخفَّضُ إلى «مخطَّط» معًا**،
#      فتبقى الشكوى واحدةً — معماريّةٌ يخفضُها المترجّمُ خارجَ قائمةِ المستهدَفات.
TARGET_RISCV = ("  - id: riscv64" + LF
                + '    name_ar: "RISC-V RV64GC (+RV32)"' + LF
                + '    milestone: "م٦"' + LF + "    order: 3" + LF
                + "    status: lowered").encode("utf-8")


# (AR) 🔑 **والنصُّ المنتظَرُ وحدَه لا يكفي.** هذا الحارسُ يطبعُ تقريرًا كاملًا
#      في كلِّ تشغيلةٍ فيه أسطرُ دَينٍ مُعلَنٍ تُشبِهُ أسطرَ الشكوى، فالمرساةُ
#      نصُّ الخطأِ بعينِه — ويُنفى ما سواه.
_NO_ENTRY = "قاعدة بلا مدخل في lowers_to.yaml"
_EXTRA_ENTRY = "مدخلٌ لقاعدةٍ غير موجودة"
_UNKNOWN_OP = "أوپكودٌ غير معروفٍ في الكتالوج"
_ISEL_DRIFT = "⇐ انجرافٌ غيرُ مُعلَن"
_STALE_DEBT = "بندُ دَينٍ بالٍ في DECLARED_ISEL_DEBT"
_OVER_ND = "السقفُ ينحدرُ ولا يُرفَع"
_FAKE_TARGET = "ولا خفضَ أصليًّا له في المترجم"
_EXTRA_ARCH = "معماريّةٌ مخفوضةٌ خارج قائمة المستهدَفات"
_NO_TARGETS = "قائمةُ المنصّات مصدرُ حقيقةٍ إلزاميّ"
_NO_CAT = "شغّل gen_sir_opcodes_yaml.py"
_NO_LOW = "شغّل gen_grammar_lowers_to.py"
_BLIND_RULES = "قارئُ القواعدِ أعمى"
_BLIND_OPS = "الكتالوجُ لم يُقرأ"
_NO_ISEL = "لا ملفَّ isel.yaml في أيِّ خلفيّة"
# (AR) 🔑 **واللامتغيِّرُ الأوّلُ (طزاجةُ الكتالوج) كان بلا مجسٍّ وبلا مَرقَب.**
#      فمجسّانِ كانا يُشعِلانِه ويمرّانِ — لأنّ `gen_sir_opcodes_yaml.py --check`
#      يقرأُ `targets.yaml` و`isel` نفسَها التي يُطفِّرُها المجسّ. (كشفَته
#      مراجعةٌ خصميّة.) فصارَ مَرقَبًا مُعلَنًا، وله مجسّاه.
_FRESHNESS = "فشل فحصُ طزاجة الكتالوج"
_NO_GEN = "مفقود المولّد"
_EMPTY_TARGETS = "بلا هدفٍ واحد"
_ALL_MARKS = (_NO_ENTRY, _EXTRA_ENTRY, _UNKNOWN_OP, _ISEL_DRIFT, _STALE_DEBT,
              _OVER_ND, _FAKE_TARGET, _EXTRA_ARCH, _NO_TARGETS, _NO_CAT,
              _NO_LOW, _BLIND_RULES, _BLIND_OPS, _NO_ISEL, _FRESHNESS,
              _NO_GEN, _EMPTY_TARGETS)

_CLEAN = "✅ الحارس سليم"


def _only(mark: str, also: tuple = ()):
    """(AR) ويُنفى ما سوى المُدَّعى — و`also` استثناءٌ **مُعلَّلٌ** لا مجاملة:
    مَرقَبٌ يشتعلُ **بالضرورةِ** مع المُدَّعى لأنّ الطفرةَ واحدةٌ تُقرأُ من
    قارئَين. وحضورُ المُدَّعى يبقى شرطًا (`want in out`) فلا يُفلِتُ المجسّ."""
    return (mark, tuple(other for other in _ALL_MARKS
                        if other != mark and other not in also))


def _green():
    return (_CLEAN, _ALL_MARKS)


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر[، عدّاءٌ])
PROBES = (
    # ═══ ① التغطيةُ في الاتّجاهَين ═══
    ("① قاعدةٌ جديدةٌ بلا مدخلٍ في lowers_to تُحمِّر",
     GRAMMAR, sub(PRODUCTIONS, PRODUCTIONS
                  + ("  - id: zz.probe.rule_without_lowering" + LF).encode("utf-8"),
                  residue=True),
     1, _only(_NO_ENTRY)),

    ("② مدخلٌ في lowers_to لقاعدةٍ لا وجودَ لها يُحمِّر",
     LOWERS, sub(FIRST_ROW,
                 ("  - id: zz.probe.row_without_rule" + LF
                  + "    status: derived" + LF
                  + "    lowers_to: [CALL]" + LF).encode("utf-8") + FIRST_ROW,
                 residue=True),
     1, _only(_EXTRA_ENTRY)),

    # ═══ ② المفردات: أوپكودٌ لا يعرفُه الكتالوج ═══
    ("③ أوپكودٌ غيرُ معروفٍ في مدخلِ خفضٍ يُحمِّر",
     LOWERS, sub(FIRST_ROW,
                 FIRST_ROW.replace(b"[ADD_I64,",
                                   b"[ZZ_PROBE_UNKNOWN_OPCODE, ADD_I64,"),
                 residue=True),
     1, _only(_UNKNOWN_OP)),

    # ═══ ③ انجرافُ isel — وقائمةُ الإذنِ في الاتّجاهَين ═══
    # (AR) ⚠️ والطزاجةُ تشتعلُ معه **بالضرورة**: المولِّدُ يقرأُ `isel` نفسَها
    #      التي يُطفِّرُها المجسّ، فلا يمكنُ فصلُهما بطفرةِ ملفٍّ واحد. والاستثناءُ
    #      مُعلَّلٌ، وحضورُ `_ISEL_DRIFT` يبقى شرطًا.
    ("④ اسمٌ في isel خارجَ تعدادِ المترجّمِ يُحمِّر",
     ISEL, sub(ISEL_FIRST, ("patterns:" + LF
                            + "  - sir: ZZ_PROBE_ISEL_DRIFT" + LF
                            + "  - sir: ADD_I64").encode("utf-8"), residue=True),
     1, _only(_ISEL_DRIFT, also=(_FRESHNESS,))),

    # (AR) 🔑 وبندٌ لا ينطبقُ اليومَ يُجيزُ عودةَ الانجرافِ غدًا — والقائمةُ
    #      تنكمشُ ولا تنمو. وكانت تُقرأُ في اتّجاهٍ واحدٍ فحسب.
    ("⑤ بندُ دَينٍ بالٍ في قائمةِ الإذنِ يُحمِّر",
     GUARD_REL, sub(b'DECLARED_ISEL_DEBT = {"CONST_I64"}',
                    b'DECLARED_ISEL_DEBT = {"CONST_I64", "ZZ_PROBE_STALE_DEBT"}',
                    residue=True),
     1, _only(_STALE_DEBT)),

    # ═══ ④ سقفُ «غيرِ المقيس» — اللامتغيِّرُ الذي استُحدِثَ بالعيار ═══
    ("⑥ قواعدُ غيرُ مقيسةٍ فوقَ سقفِها تُحمِّر",
     GUARD_REL, sub(b"CEILING_NOT_DERIVED = 2",
                    b"CEILING_NOT_DERIVED = 1  # zz_probe_lower_ceiling",
                    residue=True),
     1, _only(_OVER_ND)),

    # ═══ ⑤ المعماريّاتُ: الدعوى في مصدرِ الحقيقةِ يسنُدها المترجّمُ أو تُحمِّر ═══
    # (AR) وهدفٌ موسومٌ «مخفوض» بلا خفضٍ في المترجّمِ كذبٌ يُفشِلُ البوّابة.
    # (AR) ⚠️ ولا استثناءَ ههنا: قِيسَ أنّ الطزاجةَ **لا** تشتعلُ مع هذا
    #      المجسِّ ولا مع ⑮ — فاستثناءٌ غيرُ لازمٍ يُخفي ازدواجًا حقيقيًّا لو وقعَ
    #      غدًا. (وكان مكتوبًا خطأً أنّها تشتعلُ مع مجسّاتِ `targets.yaml` كلِّها.)
    ("⑦ هدفٌ موسومٌ lowered بلا خفضٍ يُحمِّر",
     TARGETS, sub(TARGET_PLANNED,
                  TARGET_PLANNED.replace(b"    status: planned",
                                         b"    status: lowered"
                                         b"  # zz_probe_fake_lowered"),
                  residue=True),
     1, _only(_FAKE_TARGET)),

    # (AR) والاتّجاهُ المقابل: معماريّةٌ يخفضُها المترجّمُ خارجَ قائمةِ
    #      المستهدَفات — بوّابةٌ تُقاسُ بما وُجِدَ صدفةً لا بما أُعلِن.
    ("⑧ معماريّةٌ مخفوضةٌ خارجَ قائمةِ المستهدَفاتِ تُحمِّر",
     TARGETS, sub(TARGET_RISCV,
                  TARGET_RISCV.replace(b"  - id: riscv64",
                                       b"  - id: zz_probe_renamed_riscv")
                  .replace(b"    status: lowered", b"    status: planned"),
                  residue=True),
     1, _only(_EXTRA_ARCH, also=(_FRESHNESS,))),

    # ═══ ⑥ عطبُ الآلةِ يُبلَغُ برمزِ ٢ ═══
    ("⑨ كتالوجُ الأوپكوداتِ مفقودٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'CATALOG = BACKEND_DIR / "sir_opcodes.yaml"',
                    b'CATALOG = BACKEND_DIR / "zz_probe_no_opcodes.yaml"',
                    residue=True),
     2, _only(_NO_CAT)),

    ("⑩ جدولُ الخفضِ مفقودٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'LOWERS = GRAMMAR_DIR / "lowers_to.yaml"',
                    b'LOWERS = GRAMMAR_DIR / "zz_probe_no_lowers.yaml"',
                    residue=True),
     2, _only(_NO_LOW)),

    # (AR) 🔑 **وفرقُ خاليَينِ خالٍ**: قواعدُ صفرٌ كانت تُخرِجُ «✅ الحارس سليم».
    ("⑪ قارئُ القواعدِ أعمى — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'GRAMMAR_DIR.glob("*.yaml")',
                    b'GRAMMAR_DIR.glob("*.zz_probe_no_such_suffix")',
                    residue=True),
     2, _only(_BLIND_RULES)),

    ("⑫ كتالوجٌ بلا أوپكودٍ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'cat.get("opcodes", []) or []',
                    b'cat.get("zz_probe_no_opcodes_key", []) or []',
                    residue=True),
     2, _only(_BLIND_OPS)),

    ("⑬ لا ملفَّ isel في أيِّ خلفيّةٍ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'BACKEND_DIR.glob("*/isel.yaml")',
                    b'BACKEND_DIR.glob("*/zz_probe_no_isel.yaml")',
                    residue=True),
     2, _only(_NO_ISEL)),

    # (AR) 🔑 وقائمةُ المنصّاتِ مصدرُ حقيقةٍ إلزاميّ — وغيابُها **عطبُ آلةٍ**
    #      لا حكمٌ على المحتوى. وكانت تخرجُ بـ١، والسجلُّ كادَ يُثبِّتُ ذلك عقدًا.
    ("⑭ قائمةُ المنصّاتِ مفقودةٌ — رمزُ آلةٍ ٢",
     GUARD_REL, sub(b'TARGETS = BACKEND_DIR / "targets.yaml"',
                    b'TARGETS = BACKEND_DIR / "zz_probe_no_targets.yaml"',
                    residue=True),
     2, _only(_NO_TARGETS)),

    ("⑮ قائمةُ المنصّاتِ بلا هدفٍ — رمزُ آلةٍ ٢",
     TARGETS, sub(b"targets:" + LF.encode("utf-8"),
                  b"targets: []  # zz_probe_empty_targets" + LF.encode("utf-8")
                  + b"zz_probe_old_targets:" + LF.encode("utf-8"),
                  residue=True),
     2, _only(_EMPTY_TARGETS)),

    # ═══ ⑦ اللامتغيِّرُ الأوّل: طزاجةُ الكتالوج ═══
    # (AR) 🔑 وكان مُعلَنًا في الترويسةِ بلا مجسٍّ ولا مَرقَب.
    ("⑯ فحصُ الطزاجةِ يُخفِقُ فيُحمِّر",
     GUARD_REL, sub('str(مولّد), "--check"'.encode("utf-8"),
                    'str(مولّد), "--zz-probe-bad-flag"'.encode("utf-8"),
                    residue=True),
     1, _only(_FRESHNESS)),

    ("⑰ مولِّدُ الكتالوجِ مفقودٌ فيُحمِّر",
     GUARD_REL, sub(b'with_name("gen_sir_opcodes_yaml.py")',
                    b'with_name("zz_probe_no_generator.py")', residue=True),
     1, _only(_NO_GEN)),

    # ═══ ⑧ اختبارُ انحدارٍ — يجبُ أن يبقى أخضر ═══
    # (AR) 🔑 **والتعليقُ في YAML ليس قاعدةً.** ولولا ذلك لَحمِرَ الحارسُ على
    #      مثالٍ مكتوبٍ شرحًا في ملفِّ قواعد — رفضٌ كاذبٌ يُطفَأُ به الحارس.
    ("⑱ قاعدةٌ في تعليقٍ لا تُحمِّر",
     GRAMMAR, sub(PRODUCTIONS, PRODUCTIONS
                  + ("#  - id: zz.probe.rule_in_a_comment" + LF).encode("utf-8"),
                  residue=True),
     0, _green()),

    ("⑲ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     GUARD_REL, eol_to_lf, 0, "بصمةٌ ثابتة", _run_eol_invariance),
)

# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`).
MIN_PROBES = 19


# ═══ المِحقنةُ إعلانٌ: حارسٌ · عنوانٌ · صفُّ مجسّات ═════════════════════════
_HARNESS = Harness(
    guard=GUARD,
    harness=HARNESS,
    record=RECORD,
    title="عيارُ حارسِ «ربطُ القواعدِ بأوپكوداتِ SIR» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    baseline=None,
    # (AR) 🔑 واللامتغيِّرُ الأوّلُ (الطزاجة) **مُفوَّضٌ بالكامل** إلى المولِّد،
    #      ومجسّاه ⑯/⑰ يرسوانِ على اسمِه ورايتِه — فيُبصَمُ مع الأهداف.
    depends=("scripts/codegen/gen_sir_opcodes_yaml.py",),
    # (AR) وأرضيّتاه أرضيّتا عمًى **دونَ نصفِ المقيسِ** عمدًا: القواعدُ
    #      والأوپكوداتُ تنمو وتُدمَجُ بعملٍ مشروع، وشدُّهما إلى المقيسِ يجعلُ
    #      أوّلَ إعادةِ صياغةٍ رفضًا كاذبًا. (والعددانِ في الحارسِ لا ههنا.)
    floors="slack",
    has_bounds=True,
)

if __name__ == "__main__":
    run(_HARNESS)
