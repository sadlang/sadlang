"""
============================================================================
(AR) **تعدادُ حارسِ العدمِ داخلَ النطاق** — عدّادٌ تنازليٌّ يحرس التعايشَ.

     الدَّينُ الذي يحرسه: العقد (أ) ينقل «لاشيء» من حارسٍ **داخلَ** نطاقِ
     القيمة (`kSadNullSentinel`) إلى وسمٍ **خارجَه** (`%SadDyn`). وقد هاجرت
     أربعةُ أنواعٍ (منطقي · عشري · بايت · طبيعي64) وبقي `رقم؟`/`نص؟` إلى م‑هـ.

     🔑 **وما يحرسه هذا الملفّ ليس الهجرةَ بل ما بعدها**: بينما يتقلّص مسارُ
     الحارس، لا شيءَ في الشجرةِ يمنع **مُنتِجًا جديدًا** من أن يُضاف إليه.
     وإضافةُ موضعٍ جديدٍ لا تُخفق بناءً ولا اختبارَ سلوكٍ ولا تُغيّر مخرَجًا:
     تُوسّع دَينًا مُعلَنًا بصمتٍ فيصير سدُّه أبعدَ ممّا كان.

     ⚠️ **والرقمُ لا يُنقَل — يُقاس**. سجّلت الخطّةُ «مُنتِجي الحارسِ الـ١٤»،
     ويعطي التعدادُ الآليُّ **١٩ ملفًّا** — والفرقُ ليس خطأً في أحدِهما بل في
     **التعريف**: «مُنتِجٌ» بتصنيفٍ يدويٍّ غيرُ «موضعِ ذكرٍ غيرِ تعليقيّ».
     فالبوّابةُ تُثبِّت **التعريفَ الآليَّ مع الرقم**؛ ورقمٌ بلا تعريفٍ يُنتِج
     خلافًا لا يُحسَم، وعدّادٌ يُصنَّف يدويًّا ينجرف عن نفسِه بلا أن يُخفِق.

     ⚠️ **وثنائيُّ الاتّجاه عمدًا**: يُخفِق على الارتفاعِ (دَينٌ اتّسع) **وعلى
     الانخفاضِ** (دَينٌ سُدَّ). والثاني ليس عقوبةً على التقدّم: خريطةٌ تُترَك
     مرتفعةً بعد انخفاضٍ حقيقيٍّ تفقد حساسيّتَها، فيمرُّ مُنتِجٌ جديدٌ يُعيد
     الرقمَ إلى ما كان **دون أن تُخفِق**. وهو عينُ صنفِ «قائمةٍ تُوسَّع كلَّما
     أخفقت فتبتلع الشكلَ الجديدَ صامتة».

(EN) In-band null-sentinel census — a ratchet guarding the coexistence.
     Contract (a) moves null from an in-band sentinel to an out-of-band tag.
     While that path shrinks, nothing stops a NEW producer from being added:
     doing so fails no build, no behavioural test, and changes no output — it
     silently widens a declared debt. The number is measured, not quoted: the
     plan recorded "the 14 producers" while the mechanical census reports 19
     files — the gap is the DEFINITION, not an error, so the gate pins the
     definition together with the number. Deliberately bidirectional: it fails
     on a rise (debt widened) AND on a fall (debt paid), because a map left
     high after a real drop loses sensitivity and lets a new producer restore
     the old total without failing.
============================================================================
"""
from __future__ import annotations

import io
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]

SYMBOL = "kSadNullSentinel"
# (AR) 🔑 الجذورُ تشمل `shared` و`interpreter` **عمدًا وإن كان عددُهما صفرًا اليوم**.
#      وكانت مقصورةً على `compiler`/`tools`، فكان مُنتِجٌ جديدٌ يُضاف في المحلّلِ
#      أو في محلّلِ أمانِ العدمِ أو في نظامِ الأنواعِ **يعبر السقّاطةَ صامتًا** —
#      وهي الطبقاتُ التي عدّلتها هذه الحملةُ نفسُها.
#      ⚠️ وقِيس أنّ العددَ هناك **صفرٌ** بتعريفِ هذا الملفّ (الموضعانِ الوحيدانِ
#      سطرا تعليق)، فالتوسيعُ لا يُغيّر خريطةً ولا يُخفِق بوّابةً — وهذا بالضبطِ
#      وقتُ توسيعِ حارسٍ: **يوم يكون مجّانًا**، لا يوم يصير الثمنُ حمرةً تُناقَش.
# (EN) The roots deliberately include `shared` and `interpreter` even though their
#      count is ZERO today. Restricted to compiler/tools, a new producer added in the
#      parser, the null-safety analyzer or the type system — the very layers this
#      campaign touched — would pass the ratchet silently. Measured as zero under this
#      file's own definition (the only two mentions are comment lines), so widening
#      changes no map and fails no gate: which is exactly when a guard should be
#      widened — while it is free.
SCAN_ROOTS = ["compiler", "tools", "shared", "interpreter"]
SOURCE_SUFFIXES = (".cpp", ".h", ".hpp")

# (AR) 🔑 **تعريفُ الموضعِ المعدود، آليٌّ لا اجتهاديّ**: سطرٌ يذكر الرمزَ ولا
#      يبدأ (بعد تجريدِ الفراغ) بعلامةِ تعليق. تُستبعَد أسطرُ `//` و`*` و`/*`
#      فحسب — ولا يُحاوَل تمييزُ «مُنتِجٍ» من «مقارنة»: ذلك تصنيفٌ يحتاج تحليلًا
#      نحويًّا، وتقريبُه بنمطٍ نصّيٍّ يُنتِج ثقةً كاذبةً ويُخطئ في الاتّجاهين.
#      والمقصودُ هنا حجمُ سطحِ التعايشِ لا تشريحُه.
# (EN) The counted-site definition is mechanical, not judgemental: a line that
#      mentions the symbol and does not begin with a comment marker. No attempt
#      is made to separate "producer" from "comparison" — that needs real
#      parsing, and approximating it with a regex yields false confidence and
#      errs in both directions. What is measured is the SIZE of the coexistence
#      surface, not its anatomy.
COMMENT_PREFIXES = ("//", "*", "/*")

# ══════════════════════════════════════════════════════════════════════════
# (AR) الخريطةُ المُثبَّتة — تُحدَّث **عمدًا** مع كلِّ مرحلةٍ تُغيّر العدد،
#      ويُسجَّل مسوّغُ التغيير. حالُها اليوم: بعد م‑ب/م‑ج/م‑د وم‑ز وسدِّ منافذِ
#      الحاويةِ العدميّة، وقبل م‑هـ التي تُهاجِر `رقم؟`/`نص؟` وهي المرحلةُ
#      التي يُتوقَّع أن تُنقِص أكثرَ الأرقام.
#
# ── سجلُّ التحديثات ────────────────────────────────────────────────────────
#
# (AR) [١] ١٩ ملفًّا · ٤٩ موضعًا — حالُ التثبيتِ الأوّل (م‑و).
#
# (AR) [٢] ٢٢ ملفًّا · ٥٤ موضعًا — وهذا التحديثُ. والبوّابةُ **أخفقت أوّلًا
#      فأدّت عملَها**؛ ويُسجَّل مسوّغُ كلِّ حركةٍ لأنّ رقمًا يتحرّك بلا مسوّغٍ
#      يجعل التحديثَ طقسًا يُؤدّى عند كلِّ حمرةٍ فيبتلع المُنتِجَ الحقيقيّ:
#
#        ↓ arith_type_conv.cpp        ٤ ⇒ ٢   نسختانِ من فحصِ العدمِ حُذِفتا
#                                            وحُوِّلتا نداءً للبابِ الواحد.
#        ↑ llvm_codegen_string_safety ٤ ⇒ ٥   **البابُ الواحدُ** نفسُه: الفحصُ
#                                            انتقل إلى هنا، فالارتفاعُ هنا هو
#                                            عينُ الانخفاضِ هناك — تركيزٌ لا
#                                            توسُّع. وهو ما لا يراه المجموعُ
#                                            وحدَه، ولذلك تُقاس الملفّاتُ لا
#                                            المجموعُ فقط.
#        ↑ expression_nullsafety.cpp  ٤ ⇒ ٧   `buildExprOptionalMethodCall`
#                                            الجديدة: النداءُ الآمنُ يُرجِع
#                                            العدمَ، و`رقم؟`/`نص؟` ما زالا
#                                            على الحارسِ داخلَ النطاق. فهذه
#                                            زيادةٌ **حقيقيّةٌ** في الدَّينِ
#                                            تُدفَع اليومَ وتُستردُّ في م‑هـ.
#        + class_main.cpp             ٠ ⇒ ١   [م‑ز] القيمةُ الافتراضيّةُ للحقلِ
#        + mem_alloca.cpp             ٠ ⇒ ١   العدميِّ، ومُطبِّقاها التوأمان.
#        + objects_arrays_ops.cpp     ٠ ⇒ ١   ثلاثةُ مواضعَ لنوعٍ واحدٍ يُسدَّد
#                                            كلُّها في م‑هـ دفعةً واحدة.
#
#      🔑 والدرسُ المُسجَّل: **المجموعُ وحدَه يُخفي التركيز**. لو قِيس العددُ
#      إجماليًّا لَبدا +٥ زيادةً خالصة، والحقيقةُ أنّ منها نقلًا (‑٢/+١) وأنّ
#      الزيادةَ الصافيةَ الحقيقيّةَ ستّةُ مواضعَ لسببَين مُسمَّيَين.
# (EN) The pinned map — updated DELIBERATELY, with the reason recorded. The
#      gate fired first and did its job. Every move is justified because a
#      number that moves without a reason turns the update into a ritual
#      performed on every red, which then swallows the real producer.
#      Note the concentration: -2 in arith_type_conv and +1 in the single door
#      are the SAME check moving, not new debt — visible only because files are
#      counted, not just the total.
# ══════════════════════════════════════════════════════════════════════════
CENSUS = {
    "compiler/include/frontend/sir_constants.h": 2,
    "compiler/include/frontend/sir_module.h": 3,
    "compiler/src/backend/llvm/llvm_codegen_string_safety.cpp": 5,
    "compiler/src/backend/llvm/sad_dyn_repr.cpp": 2,
    "compiler/src/backend/llvm/builders/arithmetic/arith_cmp.cpp": 3,
    "compiler/src/backend/llvm/builders/arithmetic/arith_main.cpp": 2,
    "compiler/src/backend/llvm/builders/arithmetic/arith_type_conv.cpp": 2,
    "compiler/src/backend/llvm/builders/arithmetic/cf_branch_call.cpp": 1,
    "compiler/src/backend/llvm/builders/builtins/io_builtins_ops.cpp": 1,
    "compiler/src/backend/llvm/builders/collections/string_ops.cpp": 3,
    "compiler/src/backend/llvm/builders/memory/mem_alloca.cpp": 1,
    "compiler/src/backend/llvm/builders/oop/objects_arrays_ops.cpp": 1,
    "compiler/src/backend/llvm/builders/collections/string_ops_ops.cpp": 3,
    "compiler/src/backend/llvm/builders/directives/channels_ops.cpp": 5,
    "compiler/src/backend/llvm/builders/platform/ui_ops.cpp": 2,
    "compiler/src/frontend/sir_builder_match_patterns.cpp": 1,
    "compiler/src/frontend/builders/class_main.cpp": 1,
    "compiler/src/frontend/builders/expression_basic.cpp": 2,
    "compiler/src/frontend/builders/expression_binary_op.cpp": 2,
    "compiler/src/frontend/builders/expression_nullsafety.cpp": 7,
    "compiler/src/frontend/builders/statement_assign_if.cpp": 4,
    "compiler/src/frontend/builders/statement_concurrency.cpp": 1,
}


def _measure_census() -> "dict[str, int]":
    measured: "dict[str, int]" = {}
    for root_name in SCAN_ROOTS:
        root = ROOT / root_name
        if not root.is_dir():
            continue
        for dirpath, _dirnames, filenames in os.walk(root):
            for filename in filenames:
                if not filename.endswith(SOURCE_SUFFIXES):
                    continue
                path = Path(dirpath) / filename
                try:
                    text = io.open(path, encoding="utf-8", errors="replace").read()
                except OSError:
                    continue
                if SYMBOL not in text:
                    continue
                count = 0
                for line in text.split("\n"):
                    stripped = line.strip()
                    if stripped.startswith(COMMENT_PREFIXES):
                        continue
                    if SYMBOL in line:
                        count += 1
                if count:
                    relative = path.relative_to(ROOT).as_posix()
                    measured[relative] = count
    return measured


def test_sentinel_census_matches_pinned_map():
    """(AR) خريطةُ التعايشِ مطابِقةٌ لِما ثُبِّت — ارتفاعًا وانخفاضًا."""
    measured = _measure_census()

    # (AR) ① المسحُ التقط شيئًا. خريطةٌ فارغةٌ تجعل ② تأكيدًا خاويًا يمرُّ على
    #      أيِّ شجرة: تغيُّرُ اسمِ الرمزِ أو جذورِ المسحِ يُفرِغ النتيجةَ صامتًا.
    # (EN) A zero-sized census would make the comparison vacuous.
    assert measured, (
        "لم يُلتقط أيُّ موضعٍ لِـ%s في %s.\n"
        "وهذا إخفاقٌ لا نجاح: إمّا تغيّر اسمُ الرمز، وإمّا تغيّرت جذورُ المسح، "
        "وإمّا تبدّلت لواحقُ الملفّات — والتأكيدُ على خريطةٍ فارغةٍ يمرُّ على "
        "أيِّ شجرةٍ مهما كان فيها." % (SYMBOL, "، ".join(SCAN_ROOTS))
    )

    added = {p: n for p, n in measured.items() if p not in CENSUS}
    removed = {p: n for p, n in CENSUS.items() if p not in measured}
    changed = {
        p: (CENSUS[p], measured[p])
        for p in sorted(set(CENSUS) & set(measured))
        if CENSUS[p] != measured[p]
    }

    if not (added or removed or changed):
        return

    pinned_total = sum(CENSUS.values())
    measured_total = sum(measured.values())
    direction = (
        "ارتفع" if measured_total > pinned_total
        else "انخفض" if measured_total < pinned_total
        else "لم يتغيّر مجموعُه وتغيّر توزيعُه"
    )

    lines = [
        "خريطةُ حارسِ العدمِ داخلَ النطاق لم تعد مطابِقةً للمُثبَّت — %s "
        "(%d ⇒ %d موضعًا · %d ⇒ %d ملفًّا)."
        % (direction, pinned_total, measured_total, len(CENSUS), len(measured)),
    ]
    if added:
        lines.append("🔴 ملفّاتٌ **جديدةٌ** تُنتِج الحارس: " +
                     "، ".join("%s (%d)" % item for item in sorted(added.items())))
    if removed:
        lines.append("✅ ملفّاتٌ لم تعد تُنتِجه: " +
                     "، ".join("%s (كان %d)" % item for item in sorted(removed.items())))
    if changed:
        lines.append("↕ ملفّاتٌ تبدّل عددُها: " +
                     "، ".join("%s %d⇒%d" % (p, a, b) for p, (a, b) in changed.items()))

    lines.append(
        "🔑 الارتفاعُ **يُرفَض ما لم يُبرَّر**: مُنتِجٌ جديدٌ للحارسِ لا يُخفق "
        "بناءً ولا سلوكًا ولا يُغيّر مخرَجًا — يُوسّع دَينًا مُعلَنًا بصمتٍ فيصير "
        "سدُّه أبعد."
    )
    lines.append(
        "✅ والانخفاضُ **تقدُّمٌ يُسجَّل لا يُبتلَع**: حدِّث CENSUS في هذا الملفّ "
        "عمدًا واذكر المرحلةَ التي أنقصته. خريطةٌ تُترَك مرتفعةً بعد انخفاضٍ "
        "حقيقيٍّ تفقد حساسيّتَها، فيمرُّ مُنتِجٌ جديدٌ يُعيد الرقمَ إلى ما كان "
        "**دون أن تُخفِق**."
    )
    assert False, "\n".join(lines)
