#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) يولّدُ أسماءَ التوجيهاتِ (@) من `language-truth/directives.yaml`.

     وُلد هذا المولّدُ من حاجةٍ مقيسة: تشخيصُ `@حجم` على نوعٍ لا حجمَ له
     يحتاجُ اسمَ التوجيهِ نصًّا، وكتابتُه سلسلةً خامّةً في المترجّمِ وأخرى
     في المفسّرِ هي بعينِها العلّةُ التي وُلد `size_bytes` لسدِّها: حقيقةٌ
     واحدةٌ تسكنُ مواضعَ كثيرةً فتنجرف. فالاسمُ يُقرأُ من مصدرِ الحقيقةِ
     كما يُقرأُ الحجمُ منه.

(EN) Generates directive (@) names from language-truth/directives.yaml, so a
     diagnostic naming a directive cites the SoT rather than a raw literal.
============================================================================
"""
from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

try:
    import yaml
except ImportError:  # pragma: no cover
    print("[gen_directives] FATAL: pyyaml not installed. pip install pyyaml", file=sys.stderr)
    raise SystemExit(1)

_ID_RE = re.compile(r"^directive\.([a-z][a-z0-9_]*)$")


def _fatal(msg: str) -> None:
    print(f"[gen_directives] FATAL: {msg}", file=sys.stderr)
    raise SystemExit(1)


def _hex_escape(s: str) -> str:
    """(AR) بايتات UTF-8 بـ\\xHH — يقرؤها MSVC صحيحةً مهما كان ترميزُ الملفّ."""
    return "".join(f"\\x{b:02x}" for b in s.encode("utf-8"))


def _emit(directives: list[dict]) -> str:
    lines: list[str] = []
    seen: set[str] = set()
    for entry in directives:
        raw_id = entry.get("id")
        match = _ID_RE.match(str(raw_id))
        if not match:
            _fatal(f"معرّفُ توجيهٍ غيرُ صالح: {raw_id!r}")
        const = match.group(1).upper()
        if const in seen:
            _fatal(f"اسمُ ثابتٍ مكرَّر: {const!r}")
        seen.add(const)
        name_ar = entry.get("name_ar")
        if not name_ar:
            _fatal(f"توجيهٌ بلا name_ar: {raw_id!r}")
        lines.append(
            f'            inline constexpr std::string_view {const} = '
            f'"{_hex_escape(name_ar)}";  // {name_ar}'
        )
    body = "\n".join(lines)
    return f"""// ============================================================================
// (AR) مولَّدٌ آليًّا من language-truth/directives.yaml — لا تُحرّره يدويًّا.
// (EN) AUTO-GENERATED from language-truth/directives.yaml — do not edit.
//      يُعاد توليده بـ`python x.py gen`؛ يحرسه `x.py gen --check`.
// ============================================================================
#ifndef SAD_DIRECTIVE_NAMES_GENERATED_H
#define SAD_DIRECTIVE_NAMES_GENERATED_H

#include <string_view>

namespace Sad
{{
    namespace Directives
    {{
        // (AR) أسماءُ التوجيهاتِ العربيّةُ كما تُعرَضُ في التشخيص.
        // (EN) Arabic directive names as shown in diagnostics.
        namespace Names
        {{
{body}
        }}  // namespace Names
    }}  // namespace Directives
}}  // namespace Sad

#endif  // SAD_DIRECTIVE_NAMES_GENERATED_H
"""


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate directive names from SoT.")
    ap.add_argument("--yaml", required=True)
    ap.add_argument("--header", required=True)
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    path = Path(args.yaml)
    if not path.is_file():
        _fatal(f"ملفُّ الـYAML غيرُ موجود: {path}")
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        _fatal("جذرُ الـYAML يجب أن يكون خريطة")
    directives = data.get("directives") or []
    if not directives:
        _fatal("لا توجيهاتٍ في مصدرِ الحقيقة")

    out = Path(args.header)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(_emit(directives), encoding="utf-8", newline="\n")
    if not args.quiet:
        print(f"[gen_directives] wrote: {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
