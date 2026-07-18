#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُولِّد كود C++ من cli_flags.yaml — أعلام مترجم لغة ص
(EN) C++ codegen from cli_flags.yaml — Sad compiler CLI flags
----------------------------------------------------------------------------
(AR) يقرأ المصدر الوحيد لأعلام المترجم الطويلة ويُنتج هيدرًا واحدًا:
       cli_flags_generated.h : تعداد الإجراءات + جدول المواصفات (constexpr)
     يُستهلَك من tools/compiler/compiler_driver_cli.cpp لإلغاء السلاسل
     الحرّة والمرادفات. يُحرَس بـ x.py gen --check.
(EN) Reads the single source of truth for the compiler's long flags and emits
     one header (action enum + constexpr spec table) consumed by the driver.
----------------------------------------------------------------------------
(AR) أي خطأ في YAML أو schema يُسبب فشل التوليد بـ stderr مفصّل.
============================================================================
"""

from __future__ import annotations

import argparse
import json
import sys
import traceback
from pathlib import Path
from typing import Any

try:
    import yaml  # PyYAML
except ImportError:
    print("[gen_cli_flags] FATAL: PyYAML not installed.", file=sys.stderr)
    print("[gen_cli_flags] Install: pip install pyyaml jsonschema", file=sys.stderr)
    sys.exit(2)

try:
    from jsonschema import validate, ValidationError
except ImportError:
    print("[gen_cli_flags] FATAL: jsonschema not installed.", file=sys.stderr)
    print("[gen_cli_flags] Install: pip install pyyaml jsonschema", file=sys.stderr)
    sys.exit(2)


def cpp_string_literal(s: str) -> str:
    """(AR) يحوّل سلسلة Python إلى literal C++ بأمان (يحفظ UTF-8)."""
    out = []
    for ch in s:
        if ch == '\\':
            out.append('\\\\')
        elif ch == '"':
            out.append('\\"')
        elif ch == '\n':
            out.append('\\n')
        elif ch == '\r':
            out.append('\\r')
        elif ch == '\t':
            out.append('\\t')
        else:
            out.append(ch)
    return '"' + ''.join(out) + '"'


def load_yaml(yaml_path: Path) -> dict[str, Any]:
    text = yaml_path.read_text(encoding="utf-8")
    data = yaml.safe_load(text)
    if not isinstance(data, dict):
        raise ValueError(f"YAML root must be a mapping, got {type(data).__name__}")
    return data


def load_schema(schema_path: Path) -> dict[str, Any]:
    return json.loads(schema_path.read_text(encoding="utf-8"))


def validate_data(data: dict[str, Any], schema: dict[str, Any]) -> None:
    """(AR) يفحص البنية ضدّ المخطط + فحوصات تفرّد دلاليّة (لا مرادفات)."""
    validate(data, schema)

    seen_id: dict[str, bool] = {}
    seen_canonical: dict[str, str] = {}
    seen_action: dict[str, str] = {}
    for f in data["flags"]:
        fid = f["id"]
        if fid in seen_id:
            raise ValueError(f"Duplicate flag id '{fid}'.")
        seen_id[fid] = True

        canon = f["canonical"]
        if canon in seen_canonical:
            raise ValueError(
                f"Duplicate canonical flag '{canon}' (ids '{seen_canonical[canon]}' "
                f"and '{fid}'). Each flag must have exactly ONE canonical name."
            )
        seen_canonical[canon] = fid

        action = f["action"]
        if action in seen_action:
            raise ValueError(
                f"Duplicate action '{action}' (ids '{seen_action[action]}' and '{fid}')."
            )
        seen_action[action] = fid

    # (AR) تفرّد أسماء الثوابت داخل كلّ عائلة + تفرّد أسماء الأعلام القصيرة.
    for family in ("short_flags", "value_constants", "messages"):
        seen_const: set[str] = set()
        for item in data.get(family, []):
            c = item["const"]
            if c in seen_const:
                raise ValueError(f"Duplicate const '{c}' in '{family}'.")
            seen_const.add(c)

    seen_short: set[str] = set()
    for s in data.get("short_flags", []):
        if s["name"] in seen_short:
            raise ValueError(f"Duplicate short flag '{s['name']}'.")
        seen_short.add(s["name"])


HEADER_TEMPLATE = '''// ============================================================================
// AUTO-GENERATED FROM language-truth/cli_flags.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل YAML وأعد التوليد (x.py gen).
// (EN) Auto-generated file. To modify, edit YAML and regenerate (x.py gen).
//
// (AR) نسخة الأعلام: {version}
// ============================================================================

#pragma once

#include <cstddef>
#include <cstdint>

namespace sad {{
namespace cli {{

/**
 * @brief (AR) نوع العلم: منطقيّ (تطابق تامّ) أو قيميّ (الاسم أو الاسم=قيمة).
 * @brief (EN) Flag kind: bool (exact) or value (name or name=value).
 */
enum class FlagKind : std::uint8_t {{
    Bool  = 0,
    Value = 1
}};

/**
 * @brief (AR) وسم الإجراء الذي يميّز سلوك كلّ علم في مُوزِّع المحلِّل.
 * @brief (EN) Action tag discriminating each flag's behavior in the parser.
 */
enum class FlagAction : std::uint16_t {{
{action_enum}
}};

/**
 * @brief (AR) مواصفة علم واحد. كلّ الأسماء تأتي من المصدر الوحيد (لا سلاسل حرّة).
 * @brief (EN) A single flag spec. All names come from the single source of truth.
 */
struct FlagSpec {{
    const char* canonical;   ///< (AR) الاسم العربيّ القانونيّ الوحيد (يبدأ بـ --)
    FlagKind    kind;        ///< (AR) منطقيّ أم قيميّ
    FlagAction  action;      ///< (AR) وسم الإجراء
    const char* value_hint;  ///< (AR) تلميح القيمة (فارغ للأعلام المنطقيّة)
    const char* desc_ar;     ///< (AR) الوصف العربيّ
    const char* desc_en;     ///< (EN) English description
    bool for_compiler;       ///< (AR) يستهلكه محلِّل المترجم (sad-build)
    bool for_interpreter;    ///< (AR) يستهلكه محلِّل المفسّر (sad-run)
    bool for_memory;         ///< (AR) يستهلكه ماسح سياسة الذاكرة المسبق (المحرّكان)
}};

/**
 * @brief (AR) جدول كلّ الأعلام الطويلة (المصدر الوحيد).
 */
inline constexpr FlagSpec kFlags[] = {{
{rows}
}};

/**
 * @brief (AR) عدد الأعلام (compile-time).
 */
inline constexpr std::size_t kFlagCount = {count};

// ────────────────────────────────────────────────────────────────────────────
// (AR) أعلام قصيرة قياسيّة (أعراف سلسلة أدوات، خارج التسمية العربيّة القانونيّة).
//      تُولَّد أسماؤها ثوابتَ كي لا تبقى سلاسل حرّة في المحلِّل.
// ────────────────────────────────────────────────────────────────────────────
namespace short_flags {{
{short_consts}
}} // namespace short_flags

/**
 * @brief (AR) مواصفة علم قصير (للمساعدة).
 */
struct ShortFlagSpec {{
    const char* name;
    const char* desc_ar;
    const char* desc_en;
}};

inline constexpr ShortFlagSpec kShortFlags[] = {{
{short_rows}
}};

inline constexpr std::size_t kShortFlagCount = {short_count};

// ────────────────────────────────────────────────────────────────────────────
// (AR) قيم قانونيّة للأعلام القيميّة — ثوابت بدل السلاسل الحرّة.
// ────────────────────────────────────────────────────────────────────────────
namespace values {{
{value_consts}
}} // namespace values

// ────────────────────────────────────────────────────────────────────────────
// (AR) نصوص تشخيصات المحلِّل — ثوابت بدل السلاسل الحرّة.
// ────────────────────────────────────────────────────────────────────────────
namespace messages {{
{message_consts}
}} // namespace messages

}} // namespace cli
}} // namespace sad
'''


def emit_header(data: dict[str, Any]) -> str:
    flags = data["flags"]

    # (AR) تعداد الإجراءات — قيمة صريحة مرقّمة للثبات.
    action_lines: list[str] = []
    for idx, f in enumerate(flags):
        action_lines.append(f"    {f['action']} = {idx},")
    action_enum = "\n".join(action_lines)

    rows: list[str] = []
    for f in flags:
        canon = cpp_string_literal(f["canonical"])
        kind = "FlagKind::Bool" if f["kind"] == "bool" else "FlagKind::Value"
        action = "FlagAction::" + f["action"]
        hint = cpp_string_literal(f.get("value_hint", ""))
        d_ar = cpp_string_literal(f["desc_ar"])
        d_en = cpp_string_literal(f["desc_en"])
        eng = f["engines"]
        fc = "true" if "compiler" in eng else "false"
        fi = "true" if "interpreter" in eng else "false"
        fm = "true" if "memory" in eng else "false"
        rows.append(
            f"    {{ {canon}, {kind}, {action}, {hint}, {d_ar}, {d_en}, {fc}, {fi}, {fm} }},"
        )

    shorts = data.get("short_flags", [])
    short_consts = "\n".join(
        f"    inline constexpr const char* {s['const']} = {cpp_string_literal(s['name'])};"
        for s in shorts
    )
    short_rows = "\n".join(
        f"    {{ {cpp_string_literal(s['name'])}, {cpp_string_literal(s['desc_ar'])}, "
        f"{cpp_string_literal(s['desc_en'])} }},"
        for s in shorts
    )

    value_consts = "\n".join(
        f"    inline constexpr const char* {v['const']} = {cpp_string_literal(v['value'])};"
        for v in data.get("value_constants", [])
    )
    message_consts = "\n".join(
        f"    inline constexpr const char* {m['const']} = {cpp_string_literal(m['text'])};"
        for m in data.get("messages", [])
    )

    return HEADER_TEMPLATE.format(
        version=data["version"],
        action_enum=action_enum,
        rows="\n".join(rows),
        count=len(flags),
        short_consts=short_consts,
        short_rows=short_rows,
        short_count=len(shorts),
        value_consts=value_consts,
        message_consts=message_consts,
    )


def write_if_changed(path: Path, content: str) -> bool:
    if path.exists():
        existing = path.read_text(encoding="utf-8")
        if existing == content:
            return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")
    return True


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate Sad compiler CLI flags C++ header")
    ap.add_argument("--yaml", required=True, type=Path, help="path to cli_flags.yaml")
    ap.add_argument("--schema", required=True, type=Path, help="path to cli_flags.schema.json")
    ap.add_argument("--header", required=True, type=Path, help="output .h path")
    ap.add_argument("--quiet", action="store_true", help="suppress info output")
    args = ap.parse_args()

    try:
        data = load_yaml(args.yaml)
        schema = load_schema(args.schema)
        validate_data(data, schema)

        header_text = emit_header(data)
        changed = write_if_changed(args.header, header_text)

        if not args.quiet:
            print(
                f"[gen_cli_flags] OK: {len(data['flags'])} flags, "
                f"header={'updated' if changed else 'unchanged'}"
            )
        return 0

    except ValidationError as e:
        print(f"[gen_cli_flags] SCHEMA ERROR: {e.message}", file=sys.stderr)
        print(f"[gen_cli_flags] at path: {'/'.join(map(str, e.absolute_path))}", file=sys.stderr)
        return 1
    except (yaml.YAMLError, ValueError) as e:
        print(f"[gen_cli_flags] DATA ERROR: {e}", file=sys.stderr)
        return 1
    except Exception:
        print("[gen_cli_flags] UNEXPECTED ERROR:", file=sys.stderr)
        traceback.print_exc(file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
