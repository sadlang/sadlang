#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُولِّد كود C++ من types.yaml — لغة ص
(EN) C++ codegen from types.yaml — Sad Language
----------------------------------------------------------------------------
(AR) يقرأ مصدر الحقيقة للأنواع (types.yaml) ويُنتج:
       - sad_type_kind_generated.h : تعداد SadTypeKind الموحَّد (كل القيم)
     ⚠️ الترتيب حرج: القيم تُولَّد بترتيب القائمة (توافق ثنائي/تسلسل).
     يُستدعى من CMake وقت البناء عند تغيّر YAML.
(EN) Reads the type SoT (types.yaml) and emits the unified SadTypeKind enum.
     ORDER IS CRITICAL — emitted in list order (binary/serialization compat).
----------------------------------------------------------------------------
(AR) أي خطأ في YAML أو schema يُسبب فشل البناء بـ stderr مفصّل.
(EN) Any YAML/schema error fails the build with detailed stderr.
============================================================================
"""

from __future__ import annotations

import argparse
import sys
import traceback
from pathlib import Path
from typing import Any

# (AR) [S-TS-P11] إعادة ضبط ترميز الإخراج إلى UTF-8 — لمنع انهيار طباعة الرموز (✓)
#      والعربية على وحدات تحكّم Windows (cp1252) أثناء استدعاء CMake للمولّد.
# (EN) [S-TS-P11] Force UTF-8 output so the ✓ glyph and Arabic don't crash on
#      Windows cp1252 consoles when CMake invokes the generator.
for _stream in (sys.stdout, sys.stderr):
    try:
        _stream.reconfigure(encoding="utf-8", errors="replace")  # type: ignore[attr-defined]
    except Exception:
        pass

# (AR) إضافة مجلد codegen للمسار لاستيراد المكتبة المشتركة _lib
# (EN) Add codegen dir to path for the shared _lib import
_CODEGEN_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(_CODEGEN_DIR))

try:
    from _lib import (  # type: ignore
        load_yaml,
        load_schema,
        validate_schema,
        write_if_changed,
        validate_no_duplicates,
    )
except ImportError as e:
    print(f"[gen_types] FATAL: cannot import _lib helpers: {e}", file=sys.stderr)
    sys.exit(2)

try:
    from jsonschema import ValidationError
except ImportError:
    print("[gen_types] FATAL: jsonschema not installed.", file=sys.stderr)
    print("[gen_types] Install: pip install pyyaml jsonschema", file=sys.stderr)
    sys.exit(2)


# =====================================================================
# (AR) عناوين الأقسام لكل فئة (عربي/إنجليزي) — للتعليقات فقط
# (EN) Section headers per category — for comments only
# =====================================================================
CATEGORY_SECTION = {
    "primitive":       ("أنواع بدائية", "Primitive types"),
    "sized_primitive": ("أنواع بدائية بحجم محدد (للمترجم)", "Sized primitives (for compiler)"),
    "composite":       ("أنواع مركبة", "Composite types"),
    "oop":             ("أنواع كائنية", "OOP types"),
    "function":        ("أنواع وظيفية", "Function types"),
    "advanced":        ("أنواع متقدمة", "Advanced types"),
    "pointer":         ("مؤشرات ومراجع", "Pointers and references"),
    "special":         ("أنواع خاصة", "Special types"),
    "async":           ("أنواع غير متزامنة", "Async types"),
    "graphics":        ("أنواع الرسومات", "Graphics types"),
    "simd":            ("متجهات SIMD", "SIMD vectors"),
}


def emit_header(types: list[dict[str, Any]]) -> str:
    """
    (AR) يُنتج محتوى sad_type_kind_generated.h من قائمة الأنواع المُرتّبة.
    (EN) Emits sad_type_kind_generated.h content from the ordered type list.
    """
    lines: list[str] = []
    lines.append("// ============================================================================")
    lines.append("// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY")
    lines.append("// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل types.yaml وأعد البناء.")
    lines.append("// (EN) Auto-generated file. To modify, edit types.yaml and rebuild.")
    lines.append("// ============================================================================")
    lines.append("")
    lines.append("#pragma once")
    lines.append("")
    lines.append("#include <array>")
    lines.append("#include <string_view>")
    lines.append("")
    lines.append("namespace Sad")
    lines.append("{")
    lines.append("    namespace Types")
    lines.append("    {")
    lines.append("")
    lines.append("        /**")
    lines.append("         * @brief (AR) التعداد الموحد لكل أنواع لغة ص — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Unified type kind enum — generated from types.yaml")
    lines.append("         *")
    lines.append(f"         * (AR) عدد القيم: {len(types)} — الترتيب حرج (توافق ثنائي).")
    lines.append(f"         * (EN) {len(types)} values — order is critical (binary compat).")
    lines.append("         */")
    lines.append("        enum class SadTypeKind : int")
    lines.append("        {")

    prev_cat: str | None = None
    # (AR) أطول اسم kind لمحاذاة التعليقات / (EN) longest kind for comment alignment
    max_kind = max(len(t["kind"]) for t in types)

    for t in types:
        cat = t["category"]
        if cat != prev_cat:
            ar, en = CATEGORY_SECTION.get(cat, (cat, cat))
            lines.append(f"            // ─── {ar} / {en} ───")
            prev_cat = cat
        kind = t["kind"]
        word = t.get("word", "")
        name_en = t.get("name_en", "")
        pad = " " * (max_kind - len(kind))
        lines.append(f"            {kind},{pad} ///< {word} / {name_en}")

    lines.append("        };")
    lines.append("")

    # ========================================================================
    # (AR) دالة الاسم العربي لـنوع() — مصدر حقيقة واحد للمحرّكين (مفسّر + مترجم).
    #      تُرجع typeof_ar إن وُجد، وإلا word، وإلا «مجهول». تُرمَّز القيم بـ\xHH
    #      (بايتات UTF-8) لضمان صحّتها في MSVC بغضّ النظر عن ترميز الملف المصدر.
    # (EN) Arabic name for نوع()/typeof — single source of truth for BOTH engines.
    #      Returns typeof_ar if set, else word, else «مجهول». Names are emitted as
    #      \xHH UTF-8 byte escapes so MSVC reads them correctly regardless of file
    #      encoding (mirrors the previous hand-written interpreter literals).
    # ========================================================================
    def hex_escape(s: str) -> str:
        return "".join(f"\\x{b:02x}" for b in s.encode("utf-8"))

    unknown = hex_escape("مجهول")
    lines.append("        /**")
    lines.append("         * @brief (AR) الاسم العربي الذي تُرجعه نوع() لنوعٍ ما — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Arabic name returned by نوع()/typeof for a kind — generated")
    lines.append("         */")
    lines.append("        inline const char *sadTypeKindArabicName(SadTypeKind kind)")
    lines.append("        {")
    lines.append("            switch (kind)")
    lines.append("            {")
    for t in types:
        name = t.get("typeof_ar") or t.get("word") or "مجهول"
        lines.append(f'            case SadTypeKind::{t["kind"]}: return "{hex_escape(name)}"; // {name}')
    lines.append(f'            default: return "{unknown}"; // مجهول')
    lines.append("            }")
    lines.append("        }")
    lines.append("")

    # ========================================================================
    # (AR) أسماء الأنواع السطحية (surface:true) — المرئيّة في كود المستخدم.
    #      للأدوات (LSP: تلوينها كأنواع، الإكمال). مشتقّة من types.yaml لا تهريد.
    # (EN) Surface type names (surface:true) — user-visible in source. For tooling
    #      (LSP type coloring/completion). Derived from types.yaml, not hardcoded.
    # ========================================================================
    surface = [t for t in types if t.get("surface") is True]
    lines.append("        // ─── أسماء الأنواع السطحية / Surface type names ───")
    lines.append("        /**")
    lines.append("         * @brief (AR) أسماء الأنواع السطحية (surface:true) — مُولَّدة من types.yaml")
    lines.append("         * @brief (EN) Surface type names (surface:true) — generated from types.yaml")
    lines.append("         *")
    lines.append(f"         * (AR) العدد: {len(surface)} — يستهلكها مزوّد الرموز الدلاليّة لتلوينها أنواعًا.")
    lines.append(f"         * (EN) {len(surface)} names — consumed by the semantic-tokens provider.")
    lines.append("         */")
    lines.append(f"        inline constexpr std::array<std::string_view, {len(surface)}> SURFACE_TYPE_NAMES = {{{{")
    for t in surface:
        name = t.get("word") or "?"
        lines.append(f'            "{hex_escape(name)}", // {name}')
    lines.append("        }};")
    lines.append("")

    # ========================================================================
    # (AR) وصف النوع السطحيّ بالعربية (description_ar) بحسب الكلمة — للأدوات
    #      (LSP: تلميح hover). يُرجِع الوصف من types.yaml أو "" إن لم يكن سطحيًّا.
    #      بهذا يكون وصف الأنواع في التلميح مشتقًّا من مصدر الحقيقة لا مهرَّدًا.
    # (EN) Arabic description of a surface type by word (from description_ar) — for
    #      tooling (LSP hover). Returns "" for non-surface words. Keeps hover type
    #      descriptions sourced from the SoT instead of hand-edited.
    # ========================================================================
    lines.append("        /**")
    lines.append("         * @brief (AR) وصف النوع السطحيّ بالعربية بحسب كلمته — مُولَّد من types.yaml")
    lines.append("         * @brief (EN) Arabic description of a surface type by its word — generated")
    lines.append("         */")
    lines.append("        inline const char *surfaceTypeDescriptionAr(std::string_view word)")
    lines.append("        {")
    for t in surface:
        name = t.get("word") or "?"
        desc = t.get("description_ar", "")
        lines.append(f'            if (word == "{hex_escape(name)}") return "{hex_escape(desc)}"; // {name}')
    lines.append('            return "";')
    lines.append("        }")
    lines.append("")
    lines.append("    } // namespace Types")
    lines.append("} // namespace Sad")
    lines.append("")
    return "\n".join(lines)


# =====================================================================
# (AR) مُولِّد تخطيط البُنى المضمَّنة ذات الحقول (حدث…) — مصدر حقيقة واحد
#      يستهلكه: المترجم (هيكل + POD الجسر)، وقت التشغيل (تعبئة POD من EventData).
# (EN) Built-in field-bearing struct layout generator (حدث…) — single SoT
#      consumed by: the compiler (struct + bridge POD) and the runtime (POD fill).
# =====================================================================
# (AR) خانة ABI لكلّ نوع حقل — تحدّد نوع خانة POD وخانة LLVM
# (EN) ABI slot per field kind — determines POD member type and LLVM slot type
_KIND_C_TYPE = {
    "Float":   "double",
    "Integer": "int64_t",
    "Boolean": "int64_t",  # (AR) منطقيّ يُنقل كـ i64 (أأمن ABI) ثمّ يُقتطع i1 في الـthunk
    "String":  "const char *",
}
_KIND_ABI_SLOT = {
    "Float":   "F64",
    "Integer": "I64",
    "Boolean": "I64",
    "String":  "PTR",
}
_EVENT_TYPE_SOURCE = "__type"  # (AR) مصدر خاصّ: نوع الحدث المحقون (ليس عضو EventData)


def _pod_member(source: str) -> str:
    # (AR) اسم عضو POD: نظير EventData الإنجليزيّ، وeventType لنوع الحدث المحقون
    return "eventType" if source == _EVENT_TYPE_SOURCE else source


def emit_layout_header(types: list[dict[str, Any]]) -> str:
    """
    (AR) يُنتج sad_event_layout_generated.h من الأنواع التي تحمل `fields`.
    (EN) Emits sad_event_layout_generated.h from types carrying `fields`.
    """
    def hex_escape(s: str) -> str:
        return "".join(f"\\x{b:02x}" for b in s.encode("utf-8"))

    # (AR) اليوم: حدث فقط يحمل حقولًا — لكن المولّد عامّ لأيّ نوع بحقول
    field_types = [t for t in types if isinstance(t.get("fields"), list) and t["fields"]]
    event = next((t for t in field_types if t["kind"] == "Event"), None)

    L: list[str] = []
    L.append("// ============================================================================")
    L.append("// AUTO-GENERATED FROM language-truth/types.yaml — DO NOT EDIT MANUALLY")
    L.append("// (AR) تخطيط البُنى المضمَّنة (حدث). عدّل types.yaml (حقول حدث) وأعد البناء.")
    L.append("// (EN) Built-in struct layouts (حدث). To modify, edit types.yaml and rebuild.")
    L.append("// ============================================================================")
    L.append("")
    L.append("#pragma once")
    L.append("")
    L.append("#include <array>")
    L.append("#include <cstdint>")
    L.append("#include <string_view>")
    L.append('#include "sad_type_kind_generated.h"')
    L.append("")
    L.append("namespace Sad")
    L.append("{")
    L.append("    namespace Types")
    L.append("    {")
    L.append("        namespace EventLayout")
    L.append("        {")

    if event is None:
        # (AR) لا نوع حدث بحقول — رأسٌ فارغ صالح (لا يكسر البناء)
        L.append("            // (AR) لا حقول معرّفة لنوع حدث في types.yaml.")
        L.append("        } // namespace EventLayout")
        L.append("    } // namespace Types")
        L.append("} // namespace Sad")
        L.append("")
        return "\n".join(L)

    fields = event["fields"]
    n = len(fields)

    # ─── خانة ABI للجسر / bridge ABI slot ───
    L.append("            // (AR) خانة الجسر ثلاثيّة الأنواع (تبسيط ABI): i64/double/مؤشّر")
    L.append("            // (EN) Tri-typed bridge slot (ABI simplification): i64/double/pointer")
    L.append("            enum class AbiSlot : int { I64, F64, PTR };")
    L.append("")

    # ─── بنية POD الجسر (وقت التشغيل ← thunk المترجم) ───
    L.append("            // (AR) POD الجسر: يملؤه وقت التشغيل من EventData ويقرؤه thunk المترجم")
    L.append("            // (EN) Bridge POD: filled by runtime from EventData, read by compiler thunk")
    L.append("            struct SadEventPod")
    L.append("            {")
    for f in fields:
        c_type = _KIND_C_TYPE[f["kind"]]
        member = _pod_member(f["source"])
        sep = "" if c_type.endswith("*") else " "
        L.append(f"                {c_type}{sep}{member} = {{}}; // {f['name']} ← {f['source']}")
    L.append("            };")
    L.append("")

    # ─── جدول الحقول (اسم عربيّ + نوع + خانة ABI) للمترجم/الأدوات ───
    L.append("            // (AR) وصف حقل: الاسم العربيّ + SadTypeKind + خانة ABI — بترتيب البنية")
    L.append("            // (EN) Field descriptor: Arabic name + SadTypeKind + ABI slot — struct order")
    L.append("            struct SadEventField")
    L.append("            {")
    L.append("                std::string_view nameUtf8; // (AR) الاسم العربيّ (مفتاح خريطة المفسّر/حقل بنية المترجم)")
    L.append("                std::string_view source;   // (AR) عضو EventData الإنجليزيّ المصدر (أو __type لنوع الحدث)")
    L.append("                SadTypeKind      kind;")
    L.append("                AbiSlot          abi;")
    L.append("            };")
    L.append("")
    L.append(f"            inline constexpr std::array<SadEventField, {n}> SAD_EVENT_FIELDS = {{{{")
    for f in fields:
        nm = hex_escape(f["name"])
        L.append(
            f'                SadEventField{{ "{nm}", "{f["source"]}", SadTypeKind::{f["kind"]}, '
            f'AbiSlot::{_KIND_ABI_SLOT[f["kind"]]} }}, // {f["name"]} ← {f["source"]}'
        )
    L.append("            }};")
    L.append("")
    L.append(f"            inline constexpr int SAD_EVENT_FIELD_COUNT = {n};")
    L.append("")
    # ─── اسم البنية بلغة ص (مصدر الحقيقة: word) — لمزامنة صنف المترجم المضمَّن ───
    L.append("            // (AR) اسم البنية بلغة ص (من types.yaml: word) — يُشتقّ منه المترجم")
    L.append("            //      صنفَ «حدث» المضمَّن فلا يُكتب الاسم حرفيًّا في كود المترجم.")
    L.append("            // (EN) Sad-language struct name (types.yaml: word) — the compiler")
    L.append("            //      derives its built-in event class from this (no literal in code).")
    L.append(
        f'            inline constexpr std::string_view SAD_EVENT_STRUCT_NAME = '
        f'"{hex_escape(event["word"])}"; // {event["word"]}'
    )
    L.append("")

    # ─── تعبئة POD من EventData (وقت التشغيل فقط) خلف حارس ───
    L.append("            // (AR) تعبئة POD من EventData — للتضمين في وقت التشغيل فقط:")
    L.append("            //      عرّف SAD_EVENT_POD_WITH_EVENTDATA وضمّن types.h قبل هذا الرأس.")
    L.append("            // (EN) Fill POD from EventData — runtime-side only: define")
    L.append("            //      SAD_EVENT_POD_WITH_EVENTDATA and include types.h before this header.")
    L.append("#ifdef SAD_EVENT_POD_WITH_EVENTDATA")
    L.append("            inline void sadFillEventPod(const ::sad::ui::EventData &e,")
    L.append("                                        int64_t eventType, SadEventPod &pod)")
    L.append("            {")
    for f in fields:
        src = f["source"]
        member = _pod_member(src)
        if src == _EVENT_TYPE_SOURCE:
            L.append(f"                pod.{member} = eventType;")
        elif f["kind"] == "Boolean":
            L.append(f"                pod.{member} = e.{src} ? 1 : 0;")
        elif f["kind"] == "String":
            L.append(f"                pod.{member} = e.{src}.c_str();")
        elif f["kind"] == "Integer":
            L.append(f"                pod.{member} = static_cast<int64_t>(e.{src});")
        else:  # Float
            L.append(f"                pod.{member} = static_cast<double>(e.{src});")
    L.append("            }")
    L.append("#endif // SAD_EVENT_POD_WITH_EVENTDATA")
    L.append("")
    L.append("        } // namespace EventLayout")
    L.append("    } // namespace Types")
    L.append("} // namespace Sad")
    L.append("")
    return "\n".join(L)


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Generate SadTypeKind enum (C++) from types.yaml (Sad language)."
    )
    ap.add_argument("--yaml",   required=True, type=Path, help="path to types.yaml")
    ap.add_argument("--schema", required=True, type=Path, help="path to type.schema.json")
    ap.add_argument("--header", required=True, type=Path, help="output .h path")
    ap.add_argument("--layout-header", type=Path, default=None,
                    help="output .h path for built-in struct layouts (حدث) — optional")
    ap.add_argument("--quiet",  action="store_true", help="suppress info output")
    args = ap.parse_args()

    try:
        data = load_yaml(args.yaml)
        schema = load_schema(args.schema)

        types = data.get("types")
        if not isinstance(types, list) or not types:
            raise ValueError("types.yaml must contain a non-empty 'types' list")

        # (AR) فحص المخطّط لكل إدخال / (EN) schema-validate each entry
        for i, entry in enumerate(types):
            try:
                validate_schema(entry, schema)
            except ValidationError as e:
                raise ValidationError(
                    f"types[{i}] (id={entry.get('id','?')}): {e.message}"
                ) from e

        # (AR) لا تكرار في kind ولا id / (EN) no duplicate kinds or ids
        validate_no_duplicates(types, key="kind", context="types.yaml kind")
        validate_no_duplicates(types, key="id", context="types.yaml id")

        header_text = emit_header(types)
        changed = write_if_changed(args.header, header_text)

        if not args.quiet:
            state = "written" if changed else "unchanged"
            print(f"[gen_types] ✓ {len(types)} نوعًا → {args.header.name} ({state})")

        # (AR) رأس تخطيط البُنى المضمَّنة (حدث) — اختياريّ
        # (EN) Built-in struct layout header (حدث) — optional
        if args.layout_header is not None:
            layout_text = emit_layout_header(types)
            changed2 = write_if_changed(args.layout_header, layout_text)
            if not args.quiet:
                state2 = "written" if changed2 else "unchanged"
                print(f"[gen_types] ✓ تخطيط حدث → {args.layout_header.name} ({state2})")
        return 0

    except (ValidationError, ValueError) as e:
        print(f"[gen_types] FATAL: validation error: {e}", file=sys.stderr)
        return 1
    except Exception as e:  # noqa: BLE001
        print(f"[gen_types] FATAL: {e}", file=sys.stderr)
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())
