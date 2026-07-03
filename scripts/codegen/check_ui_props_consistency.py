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
_FACTORY = _ROOT / "interpreter" / "src" / "ui" / "widget_builtins.cpp"
_METHOD_CALL = _ROOT / "interpreter" / "src" / "ui" / "ui_widget_method_call.cpp"
_COMPILER = _ROOT / "compiler" / "src" / "frontend" / "builders" / "call_method_dispatch.cpp"

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
_COLORS = _ROOT / "language-truth" / "ui_colors.yaml"
_COLOR_TABLE = _GEN_DIR / "color_table_generated.h"
_COLOR_PRELUDE = _GEN_DIR / "color_prelude_generated.h"
_COLOR_UTILS = _ROOT / "features" / "graphics" / "core" / "include" / "sad_ui" / "color_utils.h"
_INTERP_CORE = _ROOT / "interpreter" / "src" / "core" / "interpreter_core.cpp"
_COMPILER_MODULE = _ROOT / "compiler" / "src" / "frontend" / "sir_builder_module.cpp"


def _factory_prop_map(src: str) -> dict[str, str]:
    """
    (AR) يستخرج خريطة cpp_id → props::ID من مصنع المفسّر عبر ربط متغيّر الدالّة
         بـMAKE_WIDGET_WITH_PROP_FN(<Node>, sad::ui::props::<ID>) ثمّ بتسجيله
         registerBuiltinFunction(std::string(Bw::<CPP_ID>), <var>).
    """
    var_to_id: dict[str, str] = {}
    for m in re.finditer(
        r"auto\s+(\w+)\s*=\s*MAKE_WIDGET_WITH_PROP_FN\([^,]+,\s*sad::ui::props::(\w+)\)",
        src,
    ):
        var_to_id[m.group(1)] = m.group(2)
    cpp_to_id: dict[str, str] = {}
    for m in re.finditer(
        r"registerBuiltinFunction\(std::string\(Bw::(\w+)\),\s*(\w+)\)", src
    ):
        cpp_id, var = m.group(1), m.group(2)
        if var in var_to_id:
            cpp_to_id[cpp_id] = var_to_id[var]
    # (AR) صندوق المقاس يكتب props::WIDTH مباشرةً (لا عبر الماكرو) — حالة خاصّة.
    if re.search(r"UINodeType::SizedBox", src) and "sad::ui::props::WIDTH" in src:
        cpp_to_id["SIZED_BOX"] = "WIDTH"
    return cpp_to_id


def main() -> int:
    errors: list[str] = []

    props = yaml.safe_load(_PROPS.read_text(encoding="utf-8"))
    id_to_ar = {k["id"]: k["canonical"] for k in props.get("keys", [])}
    if not id_to_ar:
        print("[check_ui_props] FATAL: no keys in ui_props.yaml", file=sys.stderr)
        return 1

    widgets = yaml.safe_load(_WIDGETS.read_text(encoding="utf-8"))
    factory_src = _FACTORY.read_text(encoding="utf-8")
    cpp_to_factory_id = _factory_prop_map(factory_src)

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
        factory_id = cpp_to_factory_id.get(cpp_id)
        if factory_id is None:
            errors.append(
                f"العنصر «{canon}» (cpp_id={cpp_id}): له primary_prop لكن لم يُعثَر على مفتاح المصنع في widget_builtins.cpp"
            )
        elif factory_id != pp:
            errors.append(
                f"العنصر «{canon}»: primary_prop «{pp}» يخالف مفتاح المصنع الفعليّ «props::{factory_id}»"
            )

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

    for label, path in (("المفسّر", _METHOD_CALL), ("المترجم", _COMPILER)):
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
        # المحرّكان يسجّلان التعداد من الماكرو المولَّد (لا أسماء مجرّدة)
        if _INTERP_CORE.exists():
            ic = _INTERP_CORE.read_text(encoding="utf-8")
            if "SAD_UI_COLOR_MEMBERS" not in ic or "ensureBuiltinColorsRegistered" not in ic:
                errors.append("الألوان: المفسّر (interpreter_core.cpp) لا يسجّل تعداد الألوان من SAD_UI_COLOR_MEMBERS")
        if _COMPILER_MODULE.exists():
            cm = _COMPILER_MODULE.read_text(encoding="utf-8")
            if "SAD_UI_COLOR_MEMBERS" not in cm:
                errors.append("الألوان: المترجم (sir_builder_module.cpp) لا يستهلك SAD_UI_COLOR_MEMBERS")

    if errors:
        print("✗ فشل حارس اتّساق مفاتيح/معدّلات/مفردات/ألوان الواجهة:")
        for e in errors:
            print(f"  - {e}")
        return 1

    n_pp = sum(1 for f in widgets.get("functions", []) if f.get("primary_prop"))
    n_mod = len(mods.get("modifiers", []))
    print(
        f"✓ اتّساق الواجهة سليم: {len(id_to_ar)} مفتاحًا، {n_pp} عنصرًا بـprimary_prop "
        f"(مطابقٌ للمصنع)، {n_mod} معدّلًا (مطابقٌ لموزّعَي المحرّكين)، "
        f"{n_vocab_entries} مفردةً نصّيّةً (حركة/منحنيات/أحداث)، "
        f"{n_colors} لونًا (تعداد `ألوان` + جدول موحَّد، مطابقةٌ للهيدرات المولَّدة)."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
