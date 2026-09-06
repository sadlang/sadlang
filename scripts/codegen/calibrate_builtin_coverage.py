#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مِحقنةُ عيارِ حارسِ «الوعدُ المُعلَنُ يُقاسُ ولا يُدَّعى».

     حارسٌ لا يحمرُّ زينة. وهذه الأداةُ تُثبِتُ أنّه يحمرّ: تحقنُ عطبًا مقصودًا
     واحدًا في كلِّ مرّة، تُشغّلُ الحارس، تقيسُ رمزَ خروجِه ونصَّه، ثمّ **تستعيدُ
     البايتاتِ استعادةً تُبرهَنُ ببصمةِ sha256**.

     مجسّاتٌ عدّتُها `len(PROBES)` وأرضيّتُها `MIN_PROBES` — ولا تُنثَرُ ههنا
     عددًا يبلى. وأربعةٌ منها ليست تكرارًا لبعضِها:

       ④ **انحدارُ قدرة**: تُنزَعُ ذراعُ إرسالٍ **حقيقيّة** من المترجّم. وهذا
         المجسُّ وُلِدَ من مراجعةٍ خصميّةٍ أثبتَت أنّ نسخةً سابقةً من الحارسِ
         **تخسرُ صافيًا** قدرةَ كشفٍ كانت قائمة: حذفُ ثلاثَ عشرةَ ذراعَ WebSocket
         حقيقيّةً كان أحمرَ تحتَ الحارسِ القديمِ وأخضرَ تحتَ الجديد — لأنّ تلك
         الأذرعَ كانت موسومةً دَينًا كاذبًا. فالمجسُّ يحرسُ التصحيحَ لا الشفرةَ فقط.

       ⑥ **غشُّ التعليق** — يجبُ ألّا يحمرّ: تعليقٌ يذكرُ أسماءَ مدمجاتٍ مؤجَّلة.
         هجومٌ **أسقطَ مسبارًا سابقًا فعلًا** (٥٦٤ ← ٥٥٩ بسطرِ تعليقٍ واحد).

       ⑦ **الغسلُ بمجلَّدِ اختبار** — يجبُ ألّا يحمرّ ولا أن يُخفِضَ العدد: ملفٌّ
         يُسقَطُ في `compiler/tests/` يسمّي مدمجاتٍ مؤجَّلة. بُرهنَ أنّ ترويسةً
         واحدةً كانت تكفي لغسلِ ١٢٠٤ من ١٢٠٥ قبلَ استبعادِ مجلّداتِ الاختبار.

       ⑧ **مسبارٌ أعمى** — يجبُ أن يُنهيَ بالرمز 2 لا 1: عطبُ **آلةٍ** لا عطبُ
         **محتوى**، والخلطُ يجعلُ الأداةَ العمياءَ تُقرأُ «وجدَت عطبًا» ولم تنظرْ.

     🔑 و`--record` يُحدِّثُ سجلَّ العيارِ ومعه **بصمةُ الحارسِ وبصمةُ المِحقنةِ
        وقتَ العيار**. فإن تغيّرَ أيٌّ منهما ولم يُعَدِ العيارُ حمِرَ الحارسُ
        الفوقيُّ `check_calibration_fresh.py`.

(EN) Injection harness proving the coverage guard reddens. One defect at a time,
     byte-exact restore verified by sha256. Probe ④ is a capability-regression
     test born from an adversarial review; probes ⑥ and ⑦ must stay GREEN — they
     are regression tests for a comment-scanning defect and a test-directory
     laundering vector. Probe ⑧ must exit 2: a blind instrument is a tool fault.
============================================================================
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
# (AR) 🔑 **آليّةُ العيارِ تُورَثُ ولا تُنسَخ.** وقِيسَ لِمَ: ستُّ مِحقناتٍ
#      تقاسمَت ٢٣ دالّة، **ستَّ عشرةَ منها انجرفَت** إلى صورتَينِ فستّ —
#      ومنها شبكةُ أمانِ الانهيارِ نفسُها. والتفصيلُ في ترويسةِ القلب.
from _lib.calibration import (  # noqa: E402
    CRLF, LF_, Harness, append, create, eol_to_lf, run, self_red, sub,
)

ROOT = Path(__file__).resolve().parents[2]
GUARD = ROOT / "scripts" / "codegen" / "check_builtin_engine_coverage.py"
HARNESS = Path(__file__).resolve()
RECORD = ROOT / "scripts" / "codegen" / "calibration" / "check_builtin_engine_coverage.yaml"


def _run_guard():
    return _HARNESS.run_guard()


def _run_eol_invariance():
    """(AR) مرجعٌ مُبكِّرٌ: `PROBES` يُبنى قبلَ `_HARNESS`، والنداءُ وقتَ التشغيل."""
    return _HARNESS.run_eol_invariance()



NET = "compiler/src/frontend/builders/builtins_network.cpp"
SADNET = "language-truth/builtins/sadnet.yaml"
CORE = "language-truth/builtins/core.yaml"
CORE_CPP = "compiler/src/frontend/builders/builtins_core.cpp"
LAUNDER = "compiler/tests/zz_calibration_launder_probe.h"




# ═══ المرجعُ يُقاسُ ولا يُنسَخ ═══════════════════════════════════════════════
# (AR) 🔑 كان النصُّ المنتظَرُ يحملُ **٥٩٩ مثبَّتًا** في ثلاثةِ مجسّات — وهو
#      `CEILING_DEFERRED` المُعلَنُ **نازلًا لا يُرفَع**. فأوّلُ سدادِ دَينٍ
#      مشروعٍ يُسقِطُ ثلاثةَ مجسّاتٍ سليمة ⇒ لا يُعادُ سجلُّ العيار ⇒ الحارسُ
#      الفوقيُّ أحمر ⇒ **البوّابةُ تُقفَلُ على العملِ الذي وُجِدَ الحارسُ
#      ليُنجِزَه**. وهو عينُ ما وُجِدَ في مِحقنةِ العقد، والتُقِطَ ههنا بأداةِ
#      برهانٍ لا بقراءة. والعددُ الآنَ **يُقاسُ من الحارسِ نفسِه قبلَ أوّلِ
#      حقن**، فلا نسخةَ ثانيةً تبلى.
_DEFERRED = re.compile("دَينُ تأجيل [(]intent: [^)]*[)]: ([0-9]+)")
_BASELINE: dict[str, int] = {}


def _measure_baseline() -> None:
    """(AR) يقيسُ الدَّينَ القائمَ من مخرَجِ الحارسِ **قبلَ** أوّلِ حقن."""
    code, out = _run_guard()
    match = _DEFERRED.search(out)
    if code != 0 or not match:
        raise AssertionError(
            "تعذَّرَ قياسُ المرجعِ قبلَ الحقن (رمز=%d) — لا عيارَ على مرجعٍ مجهول"
            % code)
    _BASELINE["deferred"] = int(match.group(1))


def _deferred_is(delta: int = 0):
    """(AR) نصٌّ منتظَرٌ **مؤجَّلُ التقويم**: يُبنى من المقيسِ لا من ثابت."""
    return lambda: "دَينُ تأجيل (intent: مؤجَّل): %d" % (_BASELINE["deferred"] + delta)


def _deferred_over_ceiling():
    """(AR) رسالةُ التجاوز: المقيسُ+١ فوقَ السقفِ — طرفاها مقيسانِ لا منقولان."""
    return lambda: "%d > %d" % (_BASELINE["deferred"] + 1, _BASELINE["deferred"])


# (AR) (اسمٌ، ملفٌّ، عطبٌ، رمزٌ منتظَر، نصٌّ منتظَر)
PROBES = (
    ("① وعدٌ كاذب — مؤجَّلٌ يُوسَمُ stable",
     SADNET, sub("  intent: مؤجَّل".encode("utf-8"), b"  status: stable"),
     1, "وعدٌ كاذب (stable بلا إرسال): 1"),

    # (AR) ولا عددَ في الاسمِ أيضًا: الاسمُ يُنسَخُ حرفيًّا إلى سجلِّ العيار،
    #      فعددٌ فيه نسخةٌ ثانيةٌ تبلى بأوّلِ نزلةٍ مشروعةٍ للسقف.
    ("② دَينُ التأجيلِ ينمو بندًا واحدًا فوقَ السقف",
     CORE, sub(b"  status: stable", "  intent: مؤجَّل".encode("utf-8")),
     1, _deferred_over_ceiling()),

    ("③ حقلٌ محذوف — هروبٌ من ① و②",
     SADNET, sub("  intent: مؤجَّل\n".encode("utf-8"), b""),
     1, "خارجَ التعداد): 1"),

    # (AR) ولا تُنزَعُ ههنا `Bwsc::NEW_CLIENT`: هي **قناريّةُ عيارِ الحارسِ نفسِه**
    #      (المجسُّ الخامسُ في `_calibrate`)، فنزعُها يُنهي بالرمز 2 لا 1 — عطبَ
    #      آلةٍ لا عطبَ محتوى. وقناريّةُ الأداةِ لا تصلحُ مادّةَ اختبارٍ لها.
    ("④ انحدارُ قدرة — تُنزَعُ ذراعُ إرسالٍ حقيقيّة",
     NET, sub(b"Bwsc::SEND", b"Bwsc::ZZ_REMOVED_ARM", 99, residue=True),
     1, "وعدٌ كاذب (stable بلا إرسال): 1"),

    ("⑤ الحقلانِ معًا — status وintent",
     SADNET, sub("  intent: مؤجَّل".encode("utf-8"),
                  "  intent: مؤجَّل\n  status: stable".encode("utf-8")),
     1, "خارجَ التعداد): 1"),

    # (AR) 🔑 **بابُ الهروبِ.** كان ③ يسألُ «أثمّةَ حقل؟» لا «أقيمتُه مُعلَنة؟»،
    #      و① يشترطُ `== "stable"` و② يشترطُ `== "مؤجَّل"` — فأيُّ قيمةٍ ثالثةٍ
    #      تُفلِتُ من الثلاثةِ معًا. وبُرهنَ بالحقن: ذراعٌ محذوفةٌ + `experimental`
    #      ⇒ أربعُ لامتغيِّراتٍ خضراءُ ورمزُ صفر، والبندُ يسقطُ من الميزانَين.
    #      و`experimental` ليست فرضًا بعيدًا: مخطَّطُ المستودعِ نفسُه يُبارِكُها
    #      (`stability: [stable, experimental, deprecated]`).
    ("⑨ قيمةٌ خارجَ التعداد — status: experimental",
     SADNET, sub("  intent: مؤجَّل".encode("utf-8"), b"  status: experimental"),
     1, "خارجَ التعداد): 1"),

    ("⑥ غشُّ التعليقِ — يجبُ ألّا يُخفِّضَ العدد",
     CORE_CPP, append(b"\n// NODE_NEW NODE_ID SHA256 DRAW_CIRCLE SCREEN_WIDTH\n"),
     0, _deferred_is()),

    ("⑦ الغسلُ بمجلَّدِ اختبار — يجبُ ألّا يُخفِّضَ العدد",
     LAUNDER, create("// غسلٌ مقصود\nnamespace Zz = Sad::Builtins::Names::SadNet;\n"
                      "static auto _a = Zz::NODE_NEW; static auto _b = Zz::NODE_ID;\n"
                      .encode("utf-8")),
     0, _deferred_is()),

    # (AR) 🔑 **مجسُّ انحدارٍ: البصمةُ لا تتغيّرُ بنهاياتِ الأسطر.** يُكتَبُ
    #      الحارسُ بصورتِه التي يُخرِجُها `git checkout` (LF) ويُنتظَرُ **أخضرُ**
    #      من الحارسِ الفوقيّ. ولولاه لعادَ العطبُ صامتًا بأوّلِ رقعةٍ تنسى
    #      التطبيع: كانت البصمةُ تُحسَبُ على CRLF فلا تطابقُ أيَّ استنساخٍ نظيف،
    #      والبوّابةُ تحمرُّ في CI بحمرةٍ لا علاقةَ لها بالمحتوى.
    #      وهو أيضًا **قياسُ اتّفاقِ الطرفَين** على اتّفاقِ القراءة: لو طبّعَ أحدُهما
    #      ولم يُطبِّعِ الآخرُ لَحمِرَ ههنا.
    ("⑩ البصمةُ لا تتغيّرُ بنهاياتِ الأسطرِ (LF)",
     "scripts/codegen/check_builtin_engine_coverage.py", eol_to_lf,
     0, "بصمةٌ ثابتة", _run_eol_invariance),

    ("⑧ مسبارٌ أعمى — رمزُ عطبِ آلةٍ 2",
     "scripts/codegen/check_builtin_engine_coverage.py",
     sub(b'AREAS = ("compiler", "shared", "runtime", "stdlib", "tools", '
          b'"features", "platform")',
          b'AREAS = ("platform",)', residue=True),
     2, "عيارُ الأداة:"),
)


# (AR) أرضيّةُ العمق. تُقرأُ في الحارسِ الفوقيِّ أيضًا (`CEILING_MIN_PROBES`)،
#      وههنا تمنعُ **المِحقنةَ نفسَها** من إعلانِ نجاحٍ بلا قياس: بـ`PROBES = ()`
#      كانت تُرجِعُ صفرًا وتكتبُ `0/0`، فالحارسُ الفوقيُّ وحدَه يمسكُها.
#      وأداةُ قياسٍ تُعلِنُ نجاحًا بلا أن تقيسَ هي عينُ ما تُنشَأُ لمنعِه.
MIN_PROBES = 10


# ═══ المِحقنةُ إعلانٌ: حارسٌ · عنوانٌ · صفُّ مجسّات ═════════════════════════
_HARNESS = Harness(
    guard=GUARD,
    harness=HARNESS,
    record=RECORD,
    title="عيارُ حارسِ «الوعدُ المُعلَنُ يُقاسُ ولا يُدَّعى» بالحقن:",
    probes=PROBES,
    min_probes=MIN_PROBES,
    baseline=_measure_baseline,
    floors="none",
)

if __name__ == "__main__":
    run(_HARNESS)
