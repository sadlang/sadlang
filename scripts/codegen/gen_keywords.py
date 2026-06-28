#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُولِّد كود C++ من keywords.yaml — لغة ص
(EN) C++ codegen from keywords.yaml — Sad Language
----------------------------------------------------------------------------
(AR) يقرأ المعجم الرسمي للغة من YAML ويُنتج ملفين:
       - keywords_generated.h   : إعلانات بنية البيانات والـ APIs
       - keywords_generated.cpp : التهيئة الكاملة بكل الإدخالات
     يُستدعى من CMake وقت البناء عند تغير YAML.
(EN) Reads the official language lexicon from YAML and emits:
       - keywords_generated.h   : data structure declarations and APIs
       - keywords_generated.cpp : full table initialization
     Invoked by CMake at build time when YAML changes.
----------------------------------------------------------------------------
(AR) أي خطأ في YAML أو schema يُسبب فشل البناء بـ stderr مفصّل.
(EN) Any YAML/schema error fails the build with detailed stderr.
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
except ImportError as e:
    print("[gen_keywords] FATAL: PyYAML not installed.", file=sys.stderr)
    print("[gen_keywords] Install: pip install pyyaml jsonschema", file=sys.stderr)
    sys.exit(2)

try:
    from jsonschema import validate, ValidationError
except ImportError:
    print("[gen_keywords] FATAL: jsonschema not installed.", file=sys.stderr)
    print("[gen_keywords] Install: pip install pyyaml jsonschema", file=sys.stderr)
    sys.exit(2)


# =====================================================================
# (AR) تحويل اسم الفئة من YAML إلى enum value في C++
# (EN) Convert YAML category name to C++ enum value
# =====================================================================
CATEGORY_TO_ENUM = {
    "reserved":      "RESERVED",
    "operators":     "OPERATOR",
    "contextual":    "CONTEXTUAL",
    "builtin_types": "BUILTIN_TYPE",
}


def cpp_string_literal(s: str) -> str:
    """
    (AR) يحوّل سلسلة Python إلى literal C++ بأمان (يحفظ UTF-8).
    (EN) Convert Python string to safe C++ literal (preserves UTF-8).
    """
    # (AR) UTF-8 يُحفظ كما هو. نهرب فقط الأحرف الخطرة.
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
    """(AR) قراءة YAML بترميز UTF-8. (EN) Load YAML as UTF-8."""
    text = yaml_path.read_text(encoding="utf-8")
    data = yaml.safe_load(text)
    if not isinstance(data, dict):
        raise ValueError(f"YAML root must be a mapping, got {type(data).__name__}")
    return data


def load_schema(schema_path: Path) -> dict[str, Any]:
    """(AR) قراءة JSON Schema. (EN) Load JSON schema."""
    return json.loads(schema_path.read_text(encoding="utf-8"))


def validate_data(data: dict[str, Any], schema: dict[str, Any]) -> None:
    """
    (AR) يفحص بنية YAML ضد المخطط + فحوصات دلالية إضافية.
    (EN) Validate YAML structure against schema + extra semantic checks.
    """
    validate(data, schema)

    # (AR) فحص دلالي: كل كلمة (primary أو alias) فريدة عبر كل الفئات
    # (EN) Semantic check: every word (primary or alias) is unique across categories
    seen: dict[str, str] = {}
    for cat_name, cat in data["categories"].items():
        for entry in cat["keywords"]:
            words = [entry["word"]] + entry.get("aliases", [])
            for w in words:
                if w in seen:
                    raise ValueError(
                        f"Duplicate word '{w}' found in category '{cat_name}' "
                        f"(already in '{seen[w]}'). Each word must be unique."
                    )
                seen[w] = cat_name

    # (AR) فحص دلالي: كل tokenType فريد
    # (EN) Semantic check: every tokenType is unique
    seen_types: dict[str, str] = {}
    for cat_name, cat in data["categories"].items():
        for entry in cat["keywords"]:
            tt = entry["tokenType"]
            if tt in seen_types:
                raise ValueError(
                    f"Duplicate tokenType '{tt}' in category '{cat_name}' "
                    f"(already in '{seen_types[tt]}')."
                )
            seen_types[tt] = cat_name


# =====================================================================
# (AR) توليد الـ header
# (EN) Header generation
# =====================================================================
HEADER_TEMPLATE = '''// ============================================================================
// AUTO-GENERATED FROM language-truth/keywords.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل YAML وأعد البناء.
// (EN) Auto-generated file. To modify, edit YAML and rebuild.
// ============================================================================

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "token.h"

namespace Sad {{
namespace Lexer {{
namespace Generated {{

/**
 * @brief (AR) فئات الكلمات في معجم لغة ص
 * @brief (EN) Keyword categories in the Sad lexicon
 */
enum class KeywordCategory : std::uint8_t {{
    RESERVED     = 0,  ///< (AR) محجوزة دائماً، Lexer يُصدر KEYWORD_*
    OPERATOR     = 1,  ///< (AR) عوامل منطقية كلمات (و/أو/ليس)
    CONTEXTUAL   = 2,  ///< (AR) سياقية، Lexer يُصدر IDENTIFIER
    BUILTIN_TYPE = 3   ///< (AR) أسماء أنواع مدمجة، Lexer يُصدر IDENTIFIER
}};

/**
 * @brief (AR) إدخال واحد في المعجم
 * @brief (EN) Single lexicon entry
 */
struct KeywordEntry {{
    std::string                primaryWord;     ///< (AR) الكلمة الرئيسية
    TokenType                  type;            ///< (AR) نوع الرمز
    KeywordCategory            category;        ///< (AR) الفئة
    bool                       emittedByLexer;  ///< (AR) هل Lexer يُصدرها كرمز خاص؟
    std::vector<std::string>   aliases;         ///< (AR) أسماء بديلة (بدون تشكيل/همزة)
    std::vector<std::string>   roles;           ///< (AR) أدوار دلالية (block_opener, ...)
    std::string                english;         ///< (AR) المرادف الإنجليزي (للتوثيق)
}};

/**
 * @brief (AR) كل إدخالات المعجم
 * @brief (EN) Full lexicon entries
 */
const std::vector<KeywordEntry>& allEntries();

/**
 * @brief (AR) عدد الإدخالات الإجمالي (compile-time)
 * @brief (EN) Total entry count (compile-time)
 */
inline constexpr std::size_t kEntryCount = {entry_count};

}} // namespace Generated
}} // namespace Lexer
}} // namespace Sad
'''


def emit_header(data: dict[str, Any]) -> str:
    total = sum(len(c["keywords"]) for c in data["categories"].values())
    return HEADER_TEMPLATE.format(entry_count=total)


# =====================================================================
# (AR) توليد الـ source
# (EN) Source generation
# =====================================================================
SOURCE_TEMPLATE = '''// ============================================================================
// AUTO-GENERATED FROM language-truth/keywords.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل YAML وأعد البناء.
// (EN) Auto-generated file. To modify, edit YAML and rebuild.
//
// (AR) نسخة المعجم: {version}
// (EN) Lexicon version: {version}
// ============================================================================

#include "keywords_generated.h"

namespace Sad {{
namespace Lexer {{
namespace Generated {{

const std::vector<KeywordEntry>& allEntries() {{
    static const std::vector<KeywordEntry> entries = {{
{rows}
    }};
    return entries;
}}

}} // namespace Generated
}} // namespace Lexer
}} // namespace Sad
'''


def vector_literal(items: list[str]) -> str:
    """(AR) {{"a","b","c"}} أو {{}}. (EN) C++ vector init list."""
    if not items:
        return "{}"
    inner = ", ".join(cpp_string_literal(x) for x in items)
    return "{" + inner + "}"


def emit_source(data: dict[str, Any]) -> str:
    rows: list[str] = []
    for cat_name, cat in data["categories"].items():
        cat_enum = CATEGORY_TO_ENUM[cat_name]
        emitted = "true" if cat["emittedByLexer"] else "false"
        rows.append(f"        // ─── {cat_name} ({len(cat['keywords'])}) ───")
        for entry in cat["keywords"]:
            word_lit  = cpp_string_literal(entry["word"])
            tt        = "TokenType::" + entry["tokenType"]
            cat_lit   = "KeywordCategory::" + cat_enum
            aliases   = vector_literal(entry.get("aliases", []))
            roles     = vector_literal(entry.get("roles", []))
            english   = cpp_string_literal(entry.get("english", ""))
            rows.append(
                f"        {{ {word_lit}, {tt}, {cat_lit}, {emitted}, "
                f"{aliases}, {roles}, {english} }},"
            )
    return SOURCE_TEMPLATE.format(version=data["version"], rows="\n".join(rows))


# =====================================================================
# (AR) كتابة ذكية: لا تُحدِّث الملف إن لم يتغير المحتوى (يحفظ ctime)
# (EN) Smart write: only update if content changed (preserves ctime)
# =====================================================================
def write_if_changed(path: Path, content: str) -> bool:
    if path.exists():
        existing = path.read_text(encoding="utf-8")
        if existing == content:
            return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")
    return True


# =====================================================================
# (AR) نقطة الدخول
# (EN) Entry point
# =====================================================================
def main() -> int:
    ap = argparse.ArgumentParser(
        description="Generate Sad lexicon C++ from keywords.yaml"
    )
    ap.add_argument("--yaml",   required=True, type=Path, help="path to keywords.yaml")
    ap.add_argument("--schema", required=True, type=Path, help="path to keywords.schema.json")
    ap.add_argument("--header", required=True, type=Path, help="output .h path")
    ap.add_argument("--source", required=True, type=Path, help="output .cpp path")
    ap.add_argument("--quiet",  action="store_true", help="suppress info output")
    args = ap.parse_args()

    try:
        data   = load_yaml(args.yaml)
        schema = load_schema(args.schema)
        validate_data(data, schema)

        header_text = emit_header(data)
        source_text = emit_source(data)

        h_changed = write_if_changed(args.header, header_text)
        c_changed = write_if_changed(args.source, source_text)

        if not args.quiet:
            total = sum(len(c["keywords"]) for c in data["categories"].values())
            print(
                f"[gen_keywords] OK: {total} entries, "
                f"header={'updated' if h_changed else 'unchanged'}, "
                f"source={'updated' if c_changed else 'unchanged'}"
            )
        return 0

    except ValidationError as e:
        print(f"[gen_keywords] SCHEMA ERROR: {e.message}", file=sys.stderr)
        print(f"[gen_keywords] at path: {'/'.join(map(str, e.absolute_path))}", file=sys.stderr)
        return 1
    except (yaml.YAMLError, ValueError) as e:
        print(f"[gen_keywords] DATA ERROR: {e}", file=sys.stderr)
        return 1
    except Exception:
        print("[gen_keywords] UNEXPECTED ERROR:", file=sys.stderr)
        traceback.print_exc(file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
