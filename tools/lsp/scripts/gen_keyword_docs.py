#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مولّد أوصاف hover للكلمات المفتاحية — خاصّ بأداة LSP وحدها
(EN) Keyword hover-descriptions codegen — owned by the LSP tool only
----------------------------------------------------------------------------
(AR) يقرأ بيانات الأداة `tools/lsp/data/keyword_docs.yaml` (أوصاف عرضٍ مملوكة
     للأداة، ليست جزءًا من مصدر الحقيقة)، **ويستهلك** المعجم الرسميّ
     `language-truth/keywords.yaml` للتحقّق فقط — لا يعدّله إطلاقًا:
       • كلّ مفتاح وصف يجب أن يكون كلمةً رئيسية حقيقية في المعجم (محجوزة/عامل/
         سياقية)؛ مفتاح مجهول أو نوع مدمج ⇒ فشل البناء.
       • تُوسَّع الأسماء البديلة من المعجم تلقائيًّا (مثل «اذا» تأخذ وصف «إذا»).
     ثمّ يُصدر ترويسة header-only فيها keywordDocDescriptionAr(word).
(EN) Reads the tool's data (hover descriptions owned by the tool, NOT part of
     the SoT) and CONSUMES the official lexicon for validation only — never
     edits it. Every key must be a real primary keyword; aliases are expanded
     from the lexicon. Emits a header-only keywordDocDescriptionAr(word).
----------------------------------------------------------------------------
(AR) هذا الفصل يحقّق المبدأ: الأدوات تستهلك مصدر الحقيقة ولا توسّعه؛ معلومات
     الأداة تعيش في نظامها الداخليّ لا في ملفات اللغة القانونية.
============================================================================
"""

from __future__ import annotations

import argparse
import sys
import traceback
from pathlib import Path
from typing import Any

try:
    import yaml  # PyYAML
except ImportError:
    print("[gen_keyword_docs] FATAL: PyYAML not installed.", file=sys.stderr)
    print("[gen_keyword_docs] Install: pip install pyyaml", file=sys.stderr)
    sys.exit(2)


# (AR) علامات نهاية الجملة المقبولة في الوصف. (EN) Accepted sentence terminators.
SENTENCE_TERMINATORS = (".", "؟", "؛")  # . ؟ ؛


def load_yaml(path: Path) -> dict[str, Any]:
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"{path.name}: root must be a mapping, got {type(data).__name__}")
    return data


def collect_sot_keywords(sot_path: Path) -> tuple[dict[str, list[str]], set[str]]:
    """
    (AR) يستهلك المعجم الرسميّ ⇒ يُرجِع:
           - primary→aliases للكلمات غير المدمجة (محجوزة/عامل/سياقية)
           - مجموعة الكلمات المدمجة (لتشخيص أوضح عند خطأ المستخدم)
    (EN) Consumes the lexicon ⇒ returns primary→aliases for non-type keywords,
         plus the set of builtin-type words (for clearer diagnostics).
    """
    data = load_yaml(sot_path)
    cats = data.get("categories")
    if not isinstance(cats, dict) or not cats:
        raise ValueError(f"{sot_path.name}: 'categories' must be a non-empty mapping.")
    primary_to_aliases: dict[str, list[str]] = {}
    builtin_type_words: set[str] = set()
    for cat_name, cat in cats.items():
        entries = (cat or {}).get("keywords", []) if isinstance(cat, dict) else []
        for entry in entries:
            if not isinstance(entry, dict) or "word" not in entry:
                raise ValueError(
                    f"{sot_path.name}: keyword entry in '{cat_name}' lacks a 'word'."
                )
            word = entry["word"]
            if cat_name == "builtin_types":
                builtin_type_words.add(word)
                continue
            primary_to_aliases[word] = list(entry.get("aliases", []))
    return primary_to_aliases, builtin_type_words


def validate_descriptions(
    descriptions: dict[str, str],
    primary_to_aliases: dict[str, list[str]],
    builtin_type_words: set[str],
) -> None:
    """(AR) يفرض المبدأ: كلّ مفتاح كلمةٌ معجمية حقيقية + جودة الوصف."""
    for word, desc in descriptions.items():
        if word not in primary_to_aliases:
            if word in builtin_type_words:
                raise ValueError(
                    f"key '{word}' is a builtin type; its description belongs to "
                    f"types.yaml (surfaceTypeDescriptionAr), not to the LSP docs."
                )
            raise ValueError(
                f"key '{word}' is not a primary keyword in the lexicon "
                f"(language-truth/keywords.yaml). The tool consumes the SoT and "
                f"must not document non-keywords."
            )
        if not isinstance(desc, str) or not desc:
            raise ValueError(f"description for '{word}' must be a non-empty string.")
        if desc != desc.strip():
            raise ValueError(f"description for '{word}' has leading/trailing whitespace.")
        if "\n" in desc:
            raise ValueError(f"description for '{word}' must be a single line.")
        if not desc.endswith(SENTENCE_TERMINATORS):
            raise ValueError(
                f"description for '{word}' must end with a sentence terminator "
                f"(. ؟ ؛); got: {desc!r}"
            )


def cpp_string_literal(s: str) -> str:
    """
    (AR) literal C++ آمن يحفظ UTF-8 (codepoints تُكتب كبايتات UTF-8 عند الحفظ).
         يهرّب المحارف الخاصّة وأيّ محرف تحكّم C0 (< 0x20) كي لا يكسر الترويسة
         أو يُدخل سطرًا خفيًّا يهدّد حتميّة المولّد.
    (EN) Safe C++ literal preserving UTF-8; escapes specials and any C0 control
         char (< 0x20) so a stray control byte can't break the header or break
         generator determinism.
    """
    simple = {"\\": "\\\\", '"': '\\"', "\n": "\\n", "\r": "\\r", "\t": "\\t"}
    out = []
    for ch in s:
        if ch in simple:
            out.append(simple[ch])
        elif ord(ch) < 0x20:
            # (AR) محرف تحكّم C0 آخر (\f \v \0 …) ⇒ هروب سداسيّ عشريّ صريح.
            # (EN) Any other C0 control char ⇒ explicit hex escape.
            out.append(f"\\x{ord(ch):02x}")
        else:
            out.append(ch)
    return '"' + "".join(out) + '"'


HEADER_TEMPLATE = '''// ============================================================================
// AUTO-GENERATED FROM tools/lsp/data/keyword_docs.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد. لتعديل وصف، عدّل keyword_docs.yaml وأعد البناء.
// (EN) Auto-generated. To change a description, edit keyword_docs.yaml & rebuild.
//
// (AR) بيانات عرضٍ مملوكة لأداة LSP — ليست جزءًا من مصدر الحقيقة. مُتحقَّق منها
//      ضدّ language-truth/keywords.yaml وقت التوليد (كلّ مفتاح كلمةٌ معجمية).
// (EN) LSP-owned presentation data — NOT part of the SoT. Validated against the
//      lexicon at generation time (every key is a real keyword).
// ============================================================================

#pragma once

#include <string>
#include <unordered_map>

namespace sad {{
namespace lsp {{
namespace docs {{

/**
 * @brief (AR) وصف hover عربيّ موجز لكلمة مفتاحية (رئيسية أو بديلة). "" إن غابت.
 * @brief (EN) Short Arabic hover description for a keyword (primary or alias).
 *        Returns "" if the word is not documented.
 */
inline const std::string& keywordDocDescriptionAr(const std::string& word) {{
    static const std::string kEmpty;
    static const std::unordered_map<std::string, std::string> kDocs = {{
{rows}
    }};
    auto it = kDocs.find(word);
    return it != kDocs.end() ? it->second : kEmpty;
}}

}} // namespace docs
}} // namespace lsp
}} // namespace sad
'''


def emit_header(descriptions: dict[str, str],
                primary_to_aliases: dict[str, list[str]]) -> str:
    rows: list[str] = []
    # (AR) حارس تصادم المفاتيح: قائمة تهيئة unordered_map ذات مفتاحين متطابقين
    #      تُبقي الأوّل صمتًا، فقد يضيع وصفٌ بصمت. نرصده ونفشل البناء صراحةً.
    # (EN) Key-collision guard: a duplicate key in the map initializer list is
    #      silently dropped; detect it and fail the build instead.
    seen: dict[str, str] = {}

    def add_row(key: str, desc: str, desc_lit: str, origin: str) -> None:
        prev = seen.get(key)
        if prev is not None:
            raise ValueError(
                f"emitted key collision for '{key}' ({origin} vs {prev}); "
                f"a keyword and an alias (or two aliases) map to the same word."
            )
        seen[key] = origin
        rows.append(f"        {{ {cpp_string_literal(key)}, {desc_lit} }},")

    for word, desc in descriptions.items():
        desc_lit = cpp_string_literal(desc)
        add_row(word, desc, desc_lit, f"primary '{word}'")
        # (AR) توسيع الأسماء البديلة من المعجم لتأخذ الوصف نفسه.
        for alias in primary_to_aliases.get(word, []):
            add_row(alias, desc, desc_lit, f"alias of '{word}'")
    return HEADER_TEMPLATE.format(rows="\n".join(rows))


def write_if_changed(path: Path, content: str) -> bool:
    """(AR) كتابة ذكية تحفظ ctime وتمنع إعادة البناء بلا داعٍ."""
    if path.exists() and path.read_text(encoding="utf-8") == content:
        return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8", newline="\n")
    return True


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate LSP keyword hover docs header")
    ap.add_argument("--docs", required=True, type=Path, help="tools/lsp/data/keyword_docs.yaml")
    ap.add_argument("--sot",  required=True, type=Path, help="language-truth/keywords.yaml (read-only)")
    ap.add_argument("--out",  required=True, type=Path, help="output generated header path")
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    try:
        docs_data = load_yaml(args.docs)
        # (AR) حقل version موثَّق في البيانات؛ نفرض وجوده كنصّ كي لا يُحذف صمتًا
        #      ويبقى وعدًا متاحًا للتطوّر المستقبليّ للصيغة. (EN) Enforce a string
        #      'version' so the documented field can't silently rot.
        version = docs_data.get("version")
        if not isinstance(version, str) or not version.strip():
            raise ValueError("keyword_docs.yaml: 'version' must be a non-empty string.")
        descriptions = docs_data.get("descriptions", {})
        if not isinstance(descriptions, dict) or not descriptions:
            raise ValueError("keyword_docs.yaml: 'descriptions' must be a non-empty mapping.")

        primary_to_aliases, builtin_type_words = collect_sot_keywords(args.sot)
        validate_descriptions(descriptions, primary_to_aliases, builtin_type_words)

        header = emit_header(descriptions, primary_to_aliases)
        changed = write_if_changed(args.out, header)

        if not args.quiet:
            print(f"[gen_keyword_docs] OK: {len(descriptions)} descriptions, "
                  f"header={'updated' if changed else 'unchanged'}")
        return 0

    except (yaml.YAMLError, ValueError, KeyError) as e:
        print(f"[gen_keyword_docs] DATA ERROR: {e}", file=sys.stderr)
        return 1
    except Exception:
        print("[gen_keyword_docs] UNEXPECTED ERROR:", file=sys.stderr)
        traceback.print_exc(file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
