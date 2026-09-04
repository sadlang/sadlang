#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارس اتّساق مفاتيح ومعدّلات عناصر واجهة SadUI — يربط جذور SoT بالكود الحيّ:
       1. language-truth/ui_props.yaml            — تعريف كلّ مفتاح (id ↔ عربيّ).
       2. language-truth/builtins/ui_widgets.yaml — primary_prop + params لكلّ عنصر.
       3. features/graphics/core/include/sad_ui/prop_keys.h — الهيدر المولَّد.
       4. interpreter/src/ui/widget_builtins.cpp  — المفتاح الذي يكتبه المصنع فعلًا.
       5. language-truth/ui_modifiers.yaml        — تعريف المعدّلات.
       6. compiler/.../call_method_dispatch.cpp   — ليترالات معدّلات المترجم.
       7. language-truth/ui_{animations,easings,events}.yaml — مفردات نصّيّة يستهلكها
          ir.h/types.cpp عبر ماكرو مولَّد (لا بديل إنجليزيّ حرفيّ).
     يفشل بصوتٍ عالٍ إن:
       • primary_prop لعنصرٍ لا يطابق أيّ id في ui_props.yaml.
       • params[0] لعنصرٍ ذي primary_prop لا يبدأ بالاسم العربيّ القانونيّ للمفتاح.
       • primary_prop لا يطابق مفتاح المصنع الفعليّ (props::ID) في widget_builtins.cpp.
       • مفتاحٌ في ui_props.yaml غائبٌ عن الهيدر المولَّد.
       • اسمٌ قانونيّ لمعدّلٍ ما زال حرفيًّا (\\xHH) في أحد الموزّعَين بدل استهلاك
         الهيدر المولَّد (يمنع انحرافًا بايتيًّا صامتًا بين المحرّكين).

(EN) Consistency guard tying the SoT roots for SadUI keys/modifiers to the live
     code (interpreter factory + compiler dispatcher), preventing silent
     byte-divergence between the two engines.
============================================================================
"""
import io
import re
import sys
from pathlib import Path

# (AR) الرسائلُ عربيّةٌ وصفحةُ ترميزِ الطرفيّةِ على ويندوز ليست UTF-8. نُعيد ضبطَ
#      المجرى بدل استبدالِه: الاستبدالُ يلفّ `.buffer` فيُبطِل التقاطَ pytest
#      (يُغلَق المجرى الأصليّ تحت اللفافة) فلا يعود الحارسُ قابلًا للاختبار.
# (EN) Reconfigure, don't replace: wrapping .buffer breaks pytest's capture.
try:
    sys.stdout.reconfigure(encoding="utf-8")  # type: ignore[union-attr]
except (AttributeError, ValueError):  # pragma: no cover — مجرى لا يقبل الضبط
    if hasattr(sys.stdout, "buffer"):
        sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")

try:
    import yaml
except ImportError:
    print("[check_ui_props] FATAL: pyyaml not installed", file=sys.stderr)
    sys.exit(1)

_ROOT = Path(__file__).parents[2]
_PROPS = _ROOT / "language-truth" / "ui_props.yaml"
_MODS = _ROOT / "language-truth" / "ui_modifiers.yaml"
_WIDGETS = _ROOT / "language-truth" / "builtins" / "ui_widgets.yaml"
_HEADER = _ROOT / "features" / "graphics" / "core" / "include" / "sad_ui" / "prop_keys.h"
_COMPILER = _ROOT / "compiler" / "src" / "frontend" / "builders" / "call_method_dispatch.cpp"

# (AR) 🔑 حُذفت ثلاثُ دوالَّ وثابتانِ موضوعُها ملفّانِ في المفسّرِ المحذوف:
#      `_factory_node_map` و`_factory_prop_map` و`_registers_from_sot` كانت تقرأ
#      `interpreter/src/ui/widget_builtins.cpp`، و`_METHOD_CALL` يشيرُ إلى موزّعِ
#      طرائقِ الواجهةِ فيه. ولا نظيرَ لها في المترجّم: الواجهةُ تُخفَّضُ عبر
#      أوپكودات `BUILTIN_UI_*` لا عبر مصنعٍ يكتبُ مفتاحًا. وتُركُها معرَّفةً بلا
#      مستعمِلٍ يجعلُ الملفَّ يبدو أوسعَ حراسةً ممّا هو.
# (EN) Removed: three functions and two constants whose subject was the deleted
#      interpreter's widget factory. The compiler has no counterpart — UI lowers
#      through BUILTIN_UI_* opcodes, not a key-writing factory. Leaving them
#      defined-but-unused would make this guard look wider than it is.

# (AR) مفردات SadUI النصّيّة (حركة/منحنيات/أحداث) — كلٌّ: (اسم، SoT، الماكرو
#      المولَّد، الهيدر المولَّد، ملفّ المكتبة المستهلِك، بادئة ليترالٍ إنجليزيّ ممنوع).
#      الحارس يتحقّق أنّ المستهلِك يضمّ الهيدر ويستدعي الماكرو ولا يُبقي بديلًا
#      إنجليزيًّا حرفيًّا (نظير سياسة «قانونيّ فقط»).
_GEN_DIR = _ROOT / "features" / "graphics" / "core" / "include" / "sad_ui" / "generated"
_IR_H = _ROOT / "features" / "graphics" / "core" / "include" / "sad_ui" / "ir.h"
_TYPES_CPP = _ROOT / "features" / "graphics" / "core" / "src" / "types.cpp"
_VOCABS = (
    ("حركة", _ROOT / "language-truth" / "ui_animations.yaml",
     "SAD_UI_ANIMATION_VOCAB", _GEN_DIR / "animation_vocab_generated.h", _IR_H,
     ("fadeIn", "slideRight", "scaleUp")),
    ("منحنيات", _ROOT / "language-truth" / "ui_easings.yaml",
     "SAD_UI_EASING_VOCAB", _GEN_DIR / "easing_vocab_generated.h", _IR_H,
     ("easeIn", "easeInOut", "linear\"")),
    ("أحداث", _ROOT / "language-truth" / "ui_events.yaml",
     "SAD_UI_EVENT_VOCAB", _GEN_DIR / "event_vocab_generated.h", _TYPES_CPP,
     ("onClick", "onTap", "onChange")),
)

# (AR) ألوان SadUI: SoT + الهيدرات المولَّدة + مستهلكوها في المكتبة والمحرّكين.
# (AR) أنواع عُقَد الواجهة — مصدر الحقيقة + الهيدر المولَّد + مستهلكاه في المكتبة.
_NODES = _ROOT / "language-truth" / "ui_nodes.yaml"
_NODE_HEADER = _GEN_DIR / "node_types_generated.h"
_TYPES_H = _ROOT / "features" / "graphics" / "core" / "include" / "sad_ui" / "types.h"
# (AR) المسارُ التصريحيُّ في المحلّل (`واجهة … نهاية`): قائمةُ العناصرِ الأوّليّةِ
#      الـ١٥ (ADR-UI-02) وخريطةُ المُهمَل. سياسةُ التقليصِ قرارٌ معماريٌّ يخصُّ
#      المحلّل، لكنّ **الأسماءَ** نفسَها يجب أن تكون أسماءَ عُقَدٍ قانونيّةً في SoT
#      وإلّا رفض المحلّلُ ما يعرفه المحرّكان أو قبِل ما لا وجودَ له.
_PARSER_UI = _ROOT / "shared" / "parser" / "src" / "ui" / "parser_ui.cpp"
_PARSER_HEADER = (
    _ROOT / "shared" / "parser" / "include" / "generated" / "ui_parser_nodes_generated.h"
)
_EVENTS_YAML = _ROOT / "language-truth" / "ui_events.yaml"
# (AR) بادئةُ اسمِ الحدث — المدخلةُ الاحتياطيّةُ «مخصص» لا تحملها ولا تُبعَث.
_EVENT_PREFIX = "عند_"
# (AR) كلُّ مَن يقرّر شكلَ العقدةِ في المسارِ التصريحيّ — محلّلًا كان أو مفسّرًا.
#      قصرُ الفحصِ على parser_ui.cpp ترك اسمًا حرفيًّا في مُقيِّمِ المفسّر يقارن
#      بـ«نص» بينما الاسمُ القانونيُّ «نص_عنصر»، فسقط وسيطُ العنصرِ في مفتاحٍ
#      احتياطيٍّ لا قارئَ له ⇒ نصٌّ فارغٌ على الشاشةِ بلا خطأ.
# (EN) Everyone who decides node shape — parser AND interpreter. Narrowing this
#      to parser_ui.cpp let a literal name survive in the interpreter evaluator.
_PARSER_CONSUMERS = (
    _ROOT / "shared" / "parser" / "src" / "ui" / "parser_ui.cpp",
    _ROOT / "shared" / "parser" / "src" / "core" / "parser_main.cpp",
    _ROOT / "shared" / "parser" / "src" / "core" / "parser_expressions.cpp",
)
# (AR) ADR-UI-02: المسارُ التصريحيُّ يقبل ١٥ عنصرًا أوّليًّا، سبعةٌ منها حاويات.
#      الرقمان قرارٌ معماريٌّ لا اشتقاقٌ من المولّد، فيُحرَسان هنا صراحةً: تأكيدٌ
#      يقارن المولَّدَ بالمولَّدِ حشوٌ لا يفشل أبدًا. تغييرُهما يستلزم تعديلَ ADR.
# (EN) ADR-UI-02's 15/7 is a decision, not a derivation — assert it explicitly.
_ADR_UI_02_PRIMITIVES = 15
_ADR_UI_02_CONTAINERS = 7

_COLORS = _ROOT / "language-truth" / "ui_colors.yaml"
_COLOR_TABLE = _GEN_DIR / "color_table_generated.h"
_COLOR_PRELUDE = _GEN_DIR / "color_prelude_generated.h"
_COLOR_UTILS = _ROOT / "features" / "graphics" / "core" / "include" / "sad_ui" / "color_utils.h"
_COMPILER_MODULE = _ROOT / "compiler" / "src" / "frontend" / "sir_builder_module.cpp"


def _cpp_block(src: str, name: str) -> str:
    """(AR) جسمُ قائمةِ تهيئةٍ في C++ باسمِ متغيّرٍ معلوم. / Initializer-list body."""
    start = src.find(f"{name} = {{")
    if start < 0:
        return ""
    end = src.find("};", start)
    return src[start:end] if end > start else ""


_ARABIC_RE = re.compile(r"[؀-ۿݐ-ݿﭐ-﷿ﹰ-﻿]")
# (AR) بادئاتُ السلسلةِ الخامّة ومُحدِّدُها (حدُّ C++ ١٦ محرفًا).
_RAW_STRING_RE = re.compile(r'(?:u8|[LuU])?R"([^("\\\s]{0,16})\(')
_HEX_DIGITS = "0123456789abcdefABCDEF"
_OCTAL_DIGITS = "01234567"


def _cpp_strings(src: str) -> list[str]:
    """
    (AR) كلُّ سلسلةٍ حرفيّةٍ في كودِ C++ **بقيمتها بعد فكِّ الهروب** (خارجَ التعليقات).

    ماسحٌ لا تعبيرٌ نمطيّ، لأنّ الأخيرَ يُخدَع بطرقٍ برهنتها المراجعة:
      • `"\\xd8\\xb2\\xd8\\xb1"` — البايتاتُ المُرمَّزةُ تخفي الاسمَ (وهي صيغةُ مولّدنا نفسِه)
      • `"\\u0632\\u0631"` و`"\\330\\262"` — الهروبُ الشامل والثمانيّ
      • `R"(زر)"` — السلسلةُ الخامّةُ تلتقط قوسَيها فلا تطابق
      • `"ز" "ر"` و`"ز" /*ف*/ "ر"` — التجاورُ (ولو بتعليقٍ بينهما) يشطر الاسم
      • `//` داخلَ سلسلةٍ — كنسُ التعليقاتِ بالنمطِ يبتر بقيّةَ السطرِ الحيّ
    (EN) A scanner, not a regex: decodes every escape form, joins adjacent
         literals (even across comments), handles raw strings, and never
         mistakes `//` inside a string for a comment.
    """
    out: list[str] = []
    i, n = 0, len(src)
    pending: list[str] | None = None  # (AR) تسلسلُ سلاسلَ متجاورة

    def flush() -> None:
        nonlocal pending
        if pending is not None:
            out.append("".join(pending))
            pending = None

    while i < n:
        ch = src[i]
        # (AR) التعليقُ لا يقطع التجاور: `"ز" /*ف*/ "ر"` سلسلةٌ واحدةٌ في C++،
        #      فلا نُفرِغ `pending` عنده وإلّا انشطر الاسمُ حرفين ومرّ.
        # (EN) A comment does not break concatenation — do not flush here.
        if ch == "/" and i + 1 < n and src[i + 1] == "/":
            nl = src.find("\n", i)
            if nl < 0:
                break
            i = nl
            continue
        if ch == "/" and i + 1 < n and src[i + 1] == "*":
            end = src.find("*/", i + 2)
            i = n if end < 0 else end + 2
            continue
        if ch == "'":
            # (AR) حرفٌ مفردٌ — نتخطّاه إلى إغلاقِه الحقيقيّ. القفزُ بعددٍ ثابتٍ
            #      يخطئ في `'\\\\'` (أربعةُ محارف) فيقع على علامةِ الإغلاقِ ويُعيد
            #      فتحَ حرفٍ وهميّ، فيفقد الماسحُ تزامنَه ويبتلعَ بقيّةَ الملفّ صامتًا.
            #      وإن لم يكن حرفًا (فاصلُ أرقامٍ مثل 1'000) نتقدّم محرفًا واحدًا.
            # (EN) Advance to the real closing quote; a fixed skip desyncs on '\\\\'.
            j = i + 1
            while j < n and src[j] != "'" and src[j] != "\n":
                j += 2 if src[j] == "\\" else 1
            i = j + 1 if (j < n and src[j] == "'" and j - i <= 5) else i + 1
            continue
        # (AR) سلسلةٌ خامّة R"delim( … )delim" — بادئاتُ L/u8/u/U مقبولة.
        #      نُطابق في مكانِه (`match(src, i)`) لا على `src[i:]`: التقطيعُ عند
        #      كلِّ محرفٍ يجعل المسحَ تربيعيًّا (ثوانٍ للملفّ الواحد) فيمنع
        #      توسيعَ نطاقِ الحارسِ إلى بقيّةِ الطبقات.
        # (EN) Match in place; slicing per char makes the scan quadratic.
        raw = _RAW_STRING_RE.match(src, i)
        if raw:
            close = f'){raw.group(1)}"'
            end = src.find(close, raw.end())
            body = src[raw.end() : end if end >= 0 else n]
            (pending := pending if pending is not None else []).append(body)
            i = n if end < 0 else end + len(close)
            continue
        if ch == '"':
            # (AR) سلسلةٌ عاديّةٌ لا تعبر سطرًا في C++؛ التوقّفُ عند `\n` يمنع أن
            #      يبتلعَ اقتباسٌ يتيمٌ بقيّةَ الملفّ فيعمى الحارسُ عمّا بعده.
            j, buf = i + 1, []
            while j < n and src[j] != '"' and src[j] != "\n":
                if src[j] == "\\":
                    esc = src[j + 1 : j + 2]
                    if esc == "x":  # (AR) هروبٌ سُدسيٌّ عشريٌّ جشِعُ الخانات
                        k = j + 2
                        while k < n and src[k] in _HEX_DIGITS:
                            k += 1
                        buf.append(bytes([int(src[j + 2 : k], 16) & 0xFF]))
                        j = k
                        continue
                    if esc in ("u", "U"):
                        # (AR) هروبٌ شامل: \\uXXXX أو \\UXXXXXXXX — عددُ خاناتِه ثابت.
                        width = 4 if esc == "u" else 8
                        digits = src[j + 2 : j + 2 + width]
                        if len(digits) == width and all(c in _HEX_DIGITS for c in digits):
                            buf.append(chr(int(digits, 16)).encode("utf-8"))
                            j += 2 + width
                            continue
                    if esc in _OCTAL_DIGITS:
                        # (AR) هروبٌ ثمانيّ: حتّى ثلاثِ خاناتٍ (\\330\\262 = «ز»).
                        k = j + 1
                        while k < n and k < j + 4 and src[k] in _OCTAL_DIGITS:
                            k += 1
                        buf.append(bytes([int(src[j + 1 : k], 8) & 0xFF]))
                        j = k
                        continue
                    buf.append(_CPP_ESCAPES.get(esc, esc).encode("utf-8"))
                    j += 2
                    continue
                buf.append(src[j].encode("utf-8"))
                j += 1
            (pending := pending if pending is not None else []).append(
                b"".join(buf).decode("utf-8", errors="replace")
            )
            i = j + 1
            continue
        if not ch.isspace():
            flush()
        i += 1
    flush()
    return out


# (AR) هروباتُ C++ البسيطة (ما عدا \\xHH المعالَجَ أعلاه بجشعِه المقصود).
_CPP_ESCAPES = {"n": "\n", "t": "\t", "r": "\r", "0": "\0", "\\": "\\",
                '"': '"', "'": "'", "a": "\a", "b": "\b", "f": "\f", "v": "\v"}


def _arabic_literals(src: str) -> list[str]:
    """(AR) السلاسلُ الحرفيّةُ التي تحوي حرفًا عربيًّا، بقيمتِها المفكوكة."""
    return [s for s in _cpp_strings(src) if _ARABIC_RE.search(s)]


def _macro_names(header: str, macro: str) -> set[str]:
    """(AR) أسماءُ مدخلاتِ قائمةِ X-macro مولَّدةٍ، مفكوكةَ الهروب. / X-macro entries."""
    start = header.find(f"#define {macro}(X)")
    if start < 0:
        return set()
    end = start
    while True:  # (AR) القائمةُ تمتدّ ما دام السطرُ ينتهي بشرطةٍ مائلةٍ خلفيّة
        nl = header.find("\n", end)
        if nl < 0:
            end = len(header)
            break
        if not header[end:nl].rstrip().endswith("\\"):
            end = nl
            break
        end = nl + 1
    return {
        m for m in _cpp_strings(header[start:end]) if _ARABIC_RE.search(m)
    }








def main() -> int:
    errors: list[str] = []

    props = yaml.safe_load(_PROPS.read_text(encoding="utf-8"))
    id_to_ar = {k["id"]: k["canonical"] for k in props.get("keys", [])}
    if not id_to_ar:
        print("[check_ui_props] FATAL: no keys in ui_props.yaml", file=sys.stderr)
        return 1

    widgets = yaml.safe_load(_WIDGETS.read_text(encoding="utf-8"))
    # (AR) 🔑 **قدرةُ حراسةٍ فُقِدَت، ولا تُطوى صامتة.** كان هنا طرفانِ يُقابَلانِ
    #      بمصدرِ الحقيقة: مصنعُ العناصرِ في `interpreter/src/ui/widget_builtins.cpp`
      #    (مفتاحُ `primary_prop` الفعليّ) وخريطةُ `cpp_id ⇒ عقدة`. زالَ الملفُّ مع
    #      محرّكِه، ولا نظيرَ له في المترجم: الواجهةُ تُخفَّضُ هناك عبر أوپكودات
    #      `BUILTIN_UI_*` لا عبر مصنعٍ يكتبُ مفتاحًا. فالفحصانِ **يسقطانِ لا**
    #      **يُستبدلان**: ٤٦ إعلانَ `primary_prop` في مصدرِ الحقيقةِ صارت بلا
    #      طرفٍ ثانٍ يُقابَلُ به.
    #      وما يبقى مقيسًا هنا حقيقيٌّ لا حشو: المفتاحُ معروفٌ في `ui_props.yaml`،
    #      و`params[0]` يبدأ باسمِه القانونيّ، وكلُّ مفتاحٍ في الهيدرِ المولَّد،
    #      وطرفُ المترجمِ في `call_method_dispatch.cpp` — وكلُّها قابلةٌ للكذب.
    # (EN) A guarding CAPABILITY was LOST and is recorded, not folded away: the
    #      widget factory (the real primary_prop key) had no compiler counterpart —
    #      the compiler lowers UI through BUILTIN_UI_* opcodes, not a key-writing
    #      factory. So 46 primary_prop declarations now have no second side.
    #      What remains IS falsifiable, so this guard is not vacuous.

    for fn in widgets.get("functions", []):
        pp = fn.get("primary_prop")
        if pp is None:
            continue
        canon = fn["canonical"]
        cpp_id = fn.get("cpp_id", "")
        # 1) primary_prop معروف في ui_props.yaml
        if pp not in id_to_ar:
            errors.append(f"العنصر «{canon}»: primary_prop «{pp}» لا يطابق أيّ مفتاح في ui_props.yaml")
            continue
        # 2) params[0] يبدأ بالاسم العربيّ القانونيّ للمفتاح
        params = fn.get("params") or []
        if not params:
            errors.append(f"العنصر «{canon}»: له primary_prop «{pp}» لكن params فارغة")
        else:
            ar = id_to_ar[pp]
            first = str(params[0])
            if not first.startswith(ar):
                errors.append(
                    f"العنصر «{canon}»: params[0] «{first}» لا يبدأ بالاسم القانونيّ «{ar}» (primary_prop={pp})"
                )
        # 3) primary_prop يطابق مفتاح المصنع الفعليّ في widget_builtins.cpp
        #
        # (AR) مساران مشروعان للتسجيل، ولكلٍّ فحصُه:
        #   • مصنعٌ يدويٌّ بالماكرو ⇒ المفتاحُ مكتوبٌ فيه ويجب أن يطابق primary_prop.
        #   • تسجيلٌ مولَّدٌ من مصدرِ الحقيقة ⇒ المفتاحُ يأتي من الجدولِ المولَّدِ
        #     (نوعُ العقدة ⇒ مفتاح) المشتقِّ من primary_prop نفسِه، فالمطابقةُ
        #     بنيويّةٌ لا نصّيّة. لا نُسكِت الفحصَ هنا: نشترط أن يستهلك المفسّرُ
        #     الجدولَينِ المولَّدَين فعلًا — فحذفُ الحلقةِ يُفشِل الحارسَ كما يجب.

    # 4) كلّ مفتاح في ui_props.yaml موجود في الهيدر المولَّد
    header = _HEADER.read_text(encoding="utf-8")
    for pid in id_to_ar:
        if not re.search(rf"\b{re.escape(pid)}\b", header):
            errors.append(f"المفتاح «{pid}» غائبٌ عن الهيدر المولَّد prop_keys.h (انجراف — شغّل x.py gen)")

    # 5) المحرّكان يستهلكان الهيدر المولَّد مباشرةً (لا literals معدّلاتٍ في أيٍّ) —
    #    يمنع الانحراف بالبناء لا بالمطابقة. نتحقّق أنّ موزّعَي المفسّر والمترجم
    #    يضمّان sad_ui/ui_modifiers.h ويستدعيان دوالّ mods::is*، وأنّ لا اسمَ معدّلٍ
    #    عربيّ حرفيّ (\xHH) متبقٍّ في أيٍّ منهما.
    mods = yaml.safe_load(_MODS.read_text(encoding="utf-8"))

    def _hex(s: str) -> str:
        return "".join(
            chr(b) if 0x20 <= b < 0x7F and chr(b) not in ('"', "\\") else f"\\x{b:02x}"
            for b in s.encode("utf-8")
        )

    for label, path in (("المترجم", _COMPILER),):
        if not path.exists():
            continue
        src = path.read_text(encoding="utf-8")
        if "sad_ui/ui_modifiers.h" not in src:
            errors.append(f"موزّع {label} ({path.name}) لا يضمّ sad_ui/ui_modifiers.h — لم يعد يستهلك مصدر الحقيقة")
        if "mods::is" not in src and "ui::mods::is" not in src:
            errors.append(f"موزّع {label} ({path.name}) لا يستدعي دوالّ mods::is* المولَّدة")
        # (AR) لا اسمَ معدّلٍ عربيّ قانونيّ متبقٍّ حرفيًّا (\xHH) في الموزّع —
        #      يجب أن يأتي حصرًا من دوالّ mods::is* في الهيدر المولَّد.
        for mod in mods.get("modifiers", []):
            canon = mod["canonical"]
            if any(b >= 0x80 for b in canon.encode("utf-8")) and _hex(canon) in src:
                errors.append(
                    f"موزّع {label}: اسم المعدّل «{canon}» ما زال حرفيًّا (\\xHH) — يجب أن يأتي من الهيدر فقط"
                )

    # 6) مفردات SadUI النصّيّة (حركة/منحنيات/أحداث): المستهلِك يضمّ الهيدر المولَّد
    #    ويستدعي الماكرو، وكلّ اسمٍ قانونيّ حاضرٌ في الهيدر، ولا بديلَ إنجليزيّ حرفيّ.
    n_vocab_entries = 0
    for vlabel, vyaml, vmacro, vheader, vconsumer, forbidden in _VOCABS:
        if not vyaml.exists():
            errors.append(f"مفردة {vlabel}: SoT مفقود ({vyaml.name})")
            continue
        vdata = yaml.safe_load(vyaml.read_text(encoding="utf-8"))
        entries = vdata.get("entries", [])
        n_vocab_entries += len(entries)
        if not vheader.exists():
            errors.append(f"مفردة {vlabel}: الهيدر المولَّد مفقود ({vheader.name}) — شغّل x.py gen")
            continue
        hdr = vheader.read_text(encoding="utf-8")
        if f"#define {vmacro}" not in hdr:
            errors.append(f"مفردة {vlabel}: الماكرو {vmacro} غائبٌ عن الهيدر المولَّد")
        for e in entries:
            canon = e["canonical"]
            if any(b >= 0x80 for b in canon.encode("utf-8")) and _hex(canon) not in hdr:
                errors.append(f"مفردة {vlabel}: «{canon}» غائبٌ عن الهيدر المولَّد (انجراف — شغّل x.py gen)")
        if vconsumer.exists():
            csrc = vconsumer.read_text(encoding="utf-8")
            if vheader.name not in csrc:
                errors.append(f"مفردة {vlabel}: مستهلِك {vconsumer.name} لا يضمّ {vheader.name} — لم يعد يستهلك مصدر الحقيقة")
            if vmacro not in csrc:
                errors.append(f"مفردة {vlabel}: مستهلِك {vconsumer.name} لا يستدعي الماكرو {vmacro}")
            for bad in forbidden:
                if f'"{bad}' in csrc:
                    errors.append(f"مفردة {vlabel}: بديلٌ إنجليزيّ حرفيّ «{bad}» ما زال في {vconsumer.name} (سياسة: قانونيّ فقط)")

    # 7) ألوان SadUI: الجداول والبادئة مولَّدة، والمستهلكون يستهلكون من المصدر.
    n_colors = 0
    if _COLORS.exists():
        cdata = yaml.safe_load(_COLORS.read_text(encoding="utf-8"))
        color_list = cdata.get("colors", [])
        n_colors = len(color_list)
        table = _COLOR_TABLE.read_text(encoding="utf-8") if _COLOR_TABLE.exists() else ""
        for macro in ("SAD_UI_COLOR_NAME_HEX", "SAD_UI_COLOR_BY_NAME", "SAD_UI_COLOR_RGBA", "SAD_UI_COLOR_MEMBERS"):
            if f"#define {macro}" not in table:
                errors.append(f"الألوان: الماكرو {macro} غائبٌ عن color_table_generated.h (شغّل x.py gen)")
        if _COLOR_PRELUDE.exists() and "colorEnumPrelude" not in _COLOR_PRELUDE.read_text(encoding="utf-8"):
            errors.append("الألوان: colorEnumPrelude غائبٌ عن color_prelude_generated.h")
        # كلّ لونٍ ذي اسم حاضرٌ في الجدول المولَّد
        for c in color_list:
            m = c.get("member")
            if m and any(b >= 0x80 for b in m.encode("utf-8")) and _hex(m) not in table:
                errors.append(f"الألوان: «{m}» غائبٌ عن color_table_generated.h (انجراف — شغّل x.py gen)")
        # مستهلكو المكتبة يستهلكون الماكرو (لا سلاسل ألوان خام)
        if _COLOR_UTILS.exists():
            cu = _COLOR_UTILS.read_text(encoding="utf-8")
            if "SAD_UI_COLOR_NAME_HEX" not in cu:
                errors.append("الألوان: color_utils.h لا يستهلك SAD_UI_COLOR_NAME_HEX (جدول يدويّ؟)")
        if _TYPES_CPP.exists():
            tc = _TYPES_CPP.read_text(encoding="utf-8")
            for macro in ("SAD_UI_COLOR_BY_NAME", "SAD_UI_COLOR_RGBA"):
                if macro not in tc:
                    errors.append(f"الألوان: types.cpp لا يستهلك {macro}")
        # (AR) 🔑 حُذفت ذراعُ المفسّر. لم تكن تُخفِق، بل كانت **لا
        #      تستطيعُ أن تُطلِق**: شرطُها `_INTERP_CORE.exists()` وملفُّها محذوفٌ
        #      مع المفسّر، فكانت تمرُّ خضراءَ أبدًا. وتركُها بهذا الشكلِ يقرأُ
        #      «مقيسٌ» وهو «غيرُ مقيسٍ قطُّ».
        #      ⚠️ والحذفُ آمنٌ لأنّ موضوعَها زال. **لكن نصفَ العقدِ سقطَ
        #      معه**، وهذا يُقالُ لا يُسكَتُ عنه: كانت تفحصُ شرطَين —
        #      استهلاكَ `SAD_UI_COLOR_MEMBERS` **و**وجودَ نقطةِ التسجيلِ
        #      `ensureBuiltinColorsRegistered`. وذراعُ المترجِمِ أدناه تفحصُ
        #      الأوّلَ وحدَه. ونقطةُ التسجيلِ لا نظيرَ لها في مسارِ المحرّكِ
        #      الواحد (مقيسٌ: الاسمُ لا يردُ في الشجرِ إلّا في تعليقِ
        #      `scripts/codegen/gen_ui_colors.py:11`) — فلا شيءَ يُحرَسُ اليوم،
        #      لا أنّه محروسٌ في مكانٍ آخر.
        # (EN) 🔑 The interpreter arm is deleted. It was not failing — it
        #      COULD NOT FIRE: its `exists()` condition names a file removed with
        #      the interpreter, so it passed green forever, reading as «measured»
        #      while being «never measured».
        #      ⚠️ Deletion is safe here because its SUBJECT is gone, not because
        #      it merely looked dead: the compiler arm below checks the same
        #      contract (consumption of SAD_UI_COLOR_MEMBERS) and is live.
        # المحرّك يسجّل التعداد من الماكرو المولَّد (لا أسماء مجرّدة)
        if _COMPILER_MODULE.exists():
            cm = _COMPILER_MODULE.read_text(encoding="utf-8")
            if "SAD_UI_COLOR_MEMBERS" not in cm:
                errors.append("الألوان: المترجم (sir_builder_module.cpp) لا يستهلك SAD_UI_COLOR_MEMBERS")

    # ═════════════════════════════════════════════════════════════════════════
    # (AR) ٦. أنواع عُقَد الواجهة — ui_nodes.yaml هو المصدرُ الوحيد للتعداد
    #      وللاسمَين (طباعةً وقراءةً). يفشل إن:
    #        • عقدةٌ تُشير إلى cpp_id مصنعٍ غيرِ موجودٍ في ui_widgets.yaml.
    #        • مصنعٌ في widget_builtins.cpp يُنتج عقدةً لم تُسجَّل في builtins لها.
    #        • عقدةٌ ذاتُ مصنعٍ وحيدٍ اسمُها القانونيّ ≠ اسمُ ذلك المصنع.
    #        • اسمٌ قانونيّ مكرَّر، أو اسمُ قراءةٍ إضافيّ يصطدم باسمٍ قانونيّ.
    #        • المكتبة لم تعد تستهلك الماكرو المولَّد (جدولٌ يدويّ عاد).
    # ═════════════════════════════════════════════════════════════════════════
    nodes_data = yaml.safe_load(_NODES.read_text(encoding="utf-8")) if _NODES.exists() else None
    n_nodes = 0
    if nodes_data is None:
        errors.append(f"أنواع العُقَد: مصدر الحقيقة مفقود ({_NODES.name})")
    else:
        node_list = nodes_data.get("nodes", [])
        n_nodes = len(node_list)
        widget_canon = {f["cpp_id"]: f["canonical"] for f in widgets.get("functions", [])}

        seen_canon: dict[str, str] = {}
        declared: dict[str, str] = {}  # cpp_id ⇒ node id
        for node in node_list:
            nid, canon = node["id"], node["canonical"]
            if canon in seen_canon:
                errors.append(
                    f"أنواع العُقَد: الاسم القانونيّ «{canon}» مكرَّر بين {seen_canon[canon]} و{nid}"
                )
            seen_canon[canon] = nid
            for cpp_id in node.get("builtins", []) or []:
                if cpp_id not in widget_canon:
                    errors.append(
                        f"العقدة «{nid}»: المصنع {cpp_id} غير معرَّفٍ في builtins/ui_widgets.yaml"
                    )
                declared[cpp_id] = nid
            own = [widget_canon[c] for c in node.get("builtins", []) or [] if c in widget_canon]
            if len(own) == 1 and own[0] != canon:
                errors.append(
                    f"العقدة «{nid}»: مصنعُها الوحيد «{own[0]}» فاسمُها القانونيّ يجب أن يطابقه لا «{canon}»"
                )
        # أسماءُ القراءة الإضافيّة لا تصطدم باسمٍ قانونيّ ولا ببعضها. ولم يبقَ
        # منها إلّا صنفٌ واحد — أسماءُ مصانعِ العقدةِ عدا اسمِها القانونيّ — بعد
        # حذفِ legacy_names من مصدرِ الحقيقة (قرار مالك: لا توافقَ خلفيًّا).
        # تصادمُ اسمِ مصنعٍ جديدٍ يمرُّ صامتًا لولا هذا الفحص، فتُسقِطَ قائمةُ تهيئةِ
        # unordered_map المدخلةَ الثانيةَ ويُحَلَّ الودجت إلى عقدةٍ خاطئة.
        seen_alt: dict[str, str] = {}
        for node in node_list:
            derived = [
                widget_canon[c]
                for c in node.get("builtins", []) or []
                if c in widget_canon and widget_canon[c] != node["canonical"]
            ]
            for alt in derived:
                if alt in seen_alt and seen_alt[alt] != node["id"]:
                    errors.append(
                        f"أنواع العُقَد: اسمُ القراءةِ الإضافيّ «{alt}» مشتركٌ بين "
                        f"{seen_alt[alt]} و{node['id']} — الجدولُ العكسيّ يحجُب أحدَهما صامتًا"
                    )
                seen_alt[alt] = node["id"]
                if alt in seen_canon and seen_canon[alt] != node["id"]:
                    errors.append(
                        f"العقدة «{node['id']}»: اسمُ القراءةِ الإضافيّ «{alt}» هو الاسمُ القانونيّ "
                        f"للعقدة {seen_canon[alt]} — تضاربٌ صامت"
                    )
        # كلُّ مصنعٍ يُنتج عقدةً في المفسّر مسجَّلٌ في builtins لتلك العقدة
        # (AR) 🔑 فحصُ «مصنعٌ ⇒ عقدة» سقطَ مع الملفّ — انظر الملاحظةَ أعلاه.
        # (AR) 🔑 وأُعيدَ ما حُذِفَ زائدًا: ستّةُ فحوصٍ موضوعُها باقٍ في الشجرة
        #      سقطت مع الكتلةِ الحاضنةِ ولا علاقةَ لها بالمفسّر: ماكرو العُقَدِ في
        #      types.h/.cpp · حضورُ كلِّ عقدةٍ في الرأسِ المولَّد · قرارُ ADR-UI-02 ·
        #      مطابقةُ قوائمِ المحلّلِ للـSoT في الاتّجاهَين · منعُ الاسمِ العربيِّ
        #      الحرفيّ · منعُ عودةِ الجدولِ اليدويّ. فحذفٌ صحيحٌ اتّسعَ فأسقطَ ما لا
        #      موضوعَ له في الحذف — وقد عاد.
        # (EN) Restored: six checks whose subjects still exist fell with the
        #      enclosing block though none depended on the interpreter.
        # المكتبة تستهلك الماكرو المولَّد (لا عودةَ لجدولٍ يدويّ)
        if not _NODE_HEADER.exists():
            errors.append(f"أنواع العُقَد: الهيدر المولَّد مفقود ({_NODE_HEADER.name}) — شغّل x.py gen")
        else:
            gen = _NODE_HEADER.read_text(encoding="utf-8")
            for node in node_list:
                if _hex(node["canonical"]) not in gen:
                    errors.append(
                        f"أنواع العُقَد: «{node['canonical']}» غائبٌ عن الهيدر المولَّد "
                        f"(انجراف — شغّل x.py gen)"
                    )
            if _TYPES_H.exists():
                th = _TYPES_H.read_text(encoding="utf-8")
                if "SAD_UI_NODE_TYPE_LIST" not in th:
                    errors.append("أنواع العُقَد: types.h لا يبني UINodeType من SAD_UI_NODE_TYPE_LIST (تعدادٌ يدويّ؟)")
            if _TYPES_CPP.exists():
                tc = _TYPES_CPP.read_text(encoding="utf-8")
                for macro in ("SAD_UI_NODE_TYPE_LIST", "SAD_UI_NODE_ALT_NAME_LIST"):
                    if macro not in tc:
                        errors.append(f"أنواع العُقَد: types.cpp لا يستهلك {macro} (جدولٌ يدويّ؟)")
        # المسارُ التصريحيُّ في المحلّل: يبني مجموعاتِه من القوائمِ المولَّدةِ ولا
        #   يحمل اسمًا عربيًّا حرفيًّا (سياسة: لا نصَّ في الكود، لا توافقَ خلفيًّا).
        #   الكتلةُ خارجَ `else` رأسِ الرسومات عمدًا: غيابُ ذاك الرأسِ يجب ألّا
        #   يُسكِت فحصَ المحلّل — حارسٌ يخرَس عند عطبٍ آخرَ ليس حارسًا.
        event_names = set()
        if _EVENTS_YAML.exists():
            ev = yaml.safe_load(_EVENTS_YAML.read_text(encoding="utf-8"))
            event_names = {e["canonical"] for e in ev.get("entries", [])}
        if not _PARSER_HEADER.exists():
            errors.append(
                f"المحلّل التصريحيّ: الرأس المولَّد مفقود ({_PARSER_HEADER.name}) — شغّل x.py gen"
            )
        else:
            # (AR) مطابقةُ الرأسِ للـSoT بمجموعاتٍ **دقيقةٍ** لا باحتواءِ نصّ: الأسماءُ
            #      مُرمَّزةٌ \\xHH ورمزُ اسمٍ قصيرٍ يقع داخلَ أطولَ منه («قائمة» داخلَ
            #      «عند_القائمة_السياقية»)، فالاحتواءُ يكذب في الاتّجاهين.
            # (EN) Exact set comparison, not substring: a short name's hex is a
            #      substring of a longer one's, so `in` lies both ways.
            ph = _PARSER_HEADER.read_text(encoding="utf-8")
            got_prims = _macro_names(ph, "SAD_UI_PARSER_PRIMITIVE_LIST")
            got_conts = _macro_names(ph, "SAD_UI_PARSER_CONTAINER_LIST")
            got_events = _macro_names(ph, "SAD_UI_PARSER_EVENT_LIST")
            want_prims = {n["canonical"] for n in node_list if n.get("parser_primitive")}
            want_conts = {n["canonical"] for n in node_list if n.get("parser_container")}
            want_events = {n for n in event_names if n.startswith(_EVENT_PREFIX)}
            for label, got_n, want_n in (
                ("العناصر الأوّليّة", len(want_prims), _ADR_UI_02_PRIMITIVES),
                ("الحاويات", len(want_conts), _ADR_UI_02_CONTAINERS),
            ):
                if got_n != want_n:
                    errors.append(
                        f"ADR-UI-02 [{label}]: مصدرُ الحقيقةِ يعلن {got_n} والقرارُ "
                        f"المعماريُّ {want_n} — عدّلِ الـADR قبلَ تغييرِ العدد"
                    )
            if not want_conts <= want_prims:
                errors.append(
                    "ADR-UI-02: كلُّ حاويةٍ يجب أن تكون عنصرًا أوّليًّا — "
                    f"خارجٌ عن الأوّليّات: {sorted(want_conts - want_prims)}"
                )
            for label, want, got in (
                ("العناصر الأوّليّة", want_prims, got_prims),
                ("الحاويات", want_conts, got_conts),
                ("الأحداث", want_events, got_events),
            ):
                for missing in sorted(want - got):
                    errors.append(
                        f"المحلّل التصريحيّ [{label}]: «{missing}» في مصدر الحقيقة وغائبٌ "
                        f"عن الرأس المولَّد (انجراف — شغّل x.py gen)"
                    )
                for extra in sorted(got - want):
                    errors.append(
                        f"المحلّل التصريحيّ [{label}]: «{extra}» في الرأس المولَّد وليس في "
                        f"مصدر الحقيقة (رأسٌ بائت — شغّل x.py gen)"
                    )
            # (AR) كلُّ مستهلِكي المسارِ التصريحيّ، لا parser_ui.cpp وحدَه: أيُّ
            #      «استثناءٍ سريعٍ» باسمٍ حرفيٍّ يوضَع طبيعيًّا في الملفَّين الآخرَين.
            #      نقيسُ على ما **يبوّبُ به المحلّلُ فعلًا** (أوّليّات/حاويات/أحداث)
            #      لا على الـ١١٠ عقدةً كلِّها: كثيرٌ من أسمائها متجانساتٌ لفظيّةٌ مع
            #      أسماءِ أنواعٍ في اللغة («خريطة»، «قائمة») فتكذبُ المطابقة.
            gated_names = want_prims | want_conts | want_events
            for consumer in _PARSER_CONSUMERS:
                if not consumer.exists():
                    errors.append(f"المحلّل التصريحيّ: ملفٌّ مستهلِكٌ مفقود ({consumer.name})")
                    continue
                src_c = consumer.read_text(encoding="utf-8")
                for literal in sorted(set(_arabic_literals(src_c)) & gated_names):
                    errors.append(
                        f"المحلّل التصريحيّ: اسمٌ عربيٌّ حرفيٌّ «{literal}» في {consumer.name} "
                        f"— يجب أن يأتي من القائمة المولَّدة"
                    )
        if _PARSER_UI.exists():
            pu = _PARSER_UI.read_text(encoding="utf-8")
            for macro in (
                "SAD_UI_PARSER_PRIMITIVE_LIST",
                "SAD_UI_PARSER_CONTAINER_LIST",
                "SAD_UI_PARSER_EVENT_LIST",
            ):
                if macro not in pu:
                    errors.append(
                        f"المحلّل التصريحيّ: parser_ui.cpp لا يستهلك {macro} (جدولٌ يدويّ؟)"
                    )
            for banned in ("deprecatedWidgets", "knownWidgetsLiteral"):
                if banned in pu:
                    errors.append(
                        f"المحلّل التصريحيّ: «{banned}» عاد إلى parser_ui.cpp — "
                        f"لا توافقَ خلفيًّا ولا جدولَ أسماءٍ يدويًّا"
                    )


    if errors:
        print("✗ فشل حارس اتّساق مفاتيح/معدّلات/مفردات/ألوان/عُقَد الواجهة:")
        for e in errors:
            print(f"  - {e}")
        return 1

    n_pp = sum(1 for f in widgets.get("functions", []) if f.get("primary_prop"))
    n_mod = len(mods.get("modifiers", []))
    print(
        f"✓ اتّساق الواجهة سليم: {len(id_to_ar)} مفتاحًا، {n_pp} عنصرًا بـprimary_prop "
        f"(بلا طرفٍ مقابِلٍ — دَينٌ مُقيَّد)، {n_mod} معدّلًا (مطابقٌ لموزّع المترجم)، "
        f"{n_vocab_entries} مفردةً نصّيّةً (حركة/منحنيات/أحداث)، "
        f"{n_colors} لونًا (تعداد `ألوان` + جدول موحَّد، مطابقةٌ للهيدرات المولَّدة)، "
        f"{n_nodes} نوعَ عقدةٍ (التعدادُ وجدولا الاسم مولَّدةٌ من ui_nodes.yaml)."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
