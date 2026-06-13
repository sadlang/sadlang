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
    lines.append("    } // namespace Types")
    lines.append("} // namespace Sad")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    ap = argparse.ArgumentParser(
        description="Generate SadTypeKind enum (C++) from types.yaml (Sad language)."
    )
    ap.add_argument("--yaml",   required=True, type=Path, help="path to types.yaml")
    ap.add_argument("--schema", required=True, type=Path, help="path to type.schema.json")
    ap.add_argument("--header", required=True, type=Path, help="output .h path")
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
