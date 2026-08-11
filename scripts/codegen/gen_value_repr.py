"""
============================================================================
(AR) مُولِّد ثوابتِ تمثيلِ القيمة زمنَ التشغيل من SoT — لغة ص
(EN) Generator for runtime value-representation constants from SoT
----------------------------------------------------------------------------
(AR) يقرأ language-truth/backend/value_repr.yaml (مصدرُ الحقيقة لوسومِ SadDyn
     [DynKind] ونصوصِ عرضِ القيم [لاشيء/صحيح/خطأ]) ويُنتج هيدر C++ فيه:
       • ثوابتُ `inline constexpr long long` لوسومِ DynKind،
       • ثوابتُ `inline const std::string` لنصوصِ العرض،
     في namespace واحدٍ يستهلكه المفسّر (value.cpp) وLLVM (sad_dyn_repr)
     والخلفيّةُ الأصليّة (‎*_sir_lowering.h‎) ⇒ مصدرٌ واحدٌ يقتل صنفَ التباعُد.
     الهيدرُ مُلتزَمٌ في git (نمط go generate)؛ `x.py gen --check` حارسُ انجراف.
(EN) Reads the SoT value-representation YAML and emits a C++ header with
     inline constexpr DynKind tags + inline const std::string display texts,
     consumed by all three engines so no copy can drift.
============================================================================
"""
from __future__ import annotations

import argparse
import io
import re
import sys
from pathlib import Path
from typing import Any

try:
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8")
except Exception:  # pragma: no cover
    pass

try:
    import yaml
except ImportError:
    print("[gen_value_repr] FATAL: pyyaml not installed. pip install pyyaml", file=sys.stderr)
    sys.exit(1)

_IDENT_RE = re.compile(r"^k[A-Za-z0-9]+$")


def _fatal(msg: str) -> None:
    print(f"[gen_value_repr] FATAL: {msg}", file=sys.stderr)
    sys.exit(1)


def _cpp_escape(s: str) -> str:
    """(AR) يهرّب سلسلةً لحرفيّةِ C++."""
    return s.replace("\\", "\\\\").replace('"', '\\"')


def _emit(data: dict[str, Any]) -> str:
    ns = data.get("namespace")
    guard = data.get("guard")
    if not ns or not guard:
        _fatal("الـYAML يفتقر namespace أو guard")

    seen_names: set[str] = set()
    lines: list[str] = []

    def _check_name(name: str) -> None:
        if name is None:
            _fatal("مدخلٌ بلا name")
        if not _IDENT_RE.match(name):
            _fatal(f"اسمُ ثابتٍ غيرُ صالح (يجب kCamelCase): {name!r}")
        if name in seen_names:
            _fatal(f"اسمُ ثابتٍ مكرَّر: {name!r}")
        seen_names.add(name)

    # ── dyn_kinds (numeric tags) ──
    kinds = data.get("dyn_kinds") or []
    if kinds:
        lines.append("    // ── dyn_kinds (SadDyn runtime type tags) ──")
        seen_vals: set[int] = set()
        for e in kinds:
            name = e.get("name")
            _check_name(name)
            value = e.get("value")
            if not isinstance(value, int):
                _fatal(f"قيمةُ dyn_kind يجب أن تكون عددًا صحيحًا: {e!r}")
            if value in seen_vals:
                _fatal(f"قيمةُ dyn_kind مكرَّرة {value} عند {name!r}")
            seen_vals.add(value)
            ar = e.get("ar", "")
            comment = f"  // {ar}" if ar else ""
            lines.append(f"    inline constexpr long long {name} = {value};{comment}")
        lines.append("")

    # (AR) أقسامٌ عدديّةٌ إضافيّة: تخطيطُ الخريطةِ ووسومُ قيمِها. كلٌّ في فضاءِ
    #      ترقيمٍ مستقلّ ⇒ فحصُ التكرار داخلَ القسمِ لا عبرَه.
    # (EN) Extra numeric sections: map layout and map value tags. Each has its own
    #      numbering space, so duplicate checking is per-section, not global.
    for section, title in (("map_layout", "map_layout (runtime map memory layout)"),
                           ("map_value_tags", "map_value_tags (map slot value tags)")):
        entries = data.get(section) or []
        if not entries:
            continue
        lines.append(f"    // ── {title} ──")
        seen_sec: set[int] = set()
        for e in entries:
            name = e.get("name")
            _check_name(name)
            value = e.get("value")
            if not isinstance(value, int):
                _fatal(f"قيمةُ {section} يجب أن تكون عددًا صحيحًا: {e!r}")
            if section == "map_value_tags":
                if value in seen_sec:
                    _fatal(f"وسمٌ مكرَّر {value} عند {name!r}")
                seen_sec.add(value)
            ar = e.get("ar", "")
            comment = f"  // {ar}" if ar else ""
            lines.append(f"    inline constexpr long long {name} = {value};{comment}")
        lines.append("")

    # ── display_texts (user-facing value strings) ──
    texts = data.get("display_texts") or []
    if texts:
        lines.append("    // ── display_texts (user-facing value display) ──")
        seen_text_vals: dict[str, str] = {}
        for e in texts:
            name = e.get("name")
            _check_name(name)
            value = e.get("value")
            if not isinstance(value, str):
                _fatal(f"قيمةُ display_text يجب أن تكون نصًّا: {e!r}")
            if value in seen_text_vals:
                _fatal(f"نصُّ عرضٍ مكرَّر {value!r} بين {seen_text_vals[value]!r} و{name!r}")
            seen_text_vals[value] = name
            ar = e.get("ar", "")
            comment = f"  // {ar}" if ar else ""
            lines.append(f'    inline const std::string {name} = "{_cpp_escape(value)}";{comment}')
        lines.append("")

    body = "\n".join(lines).rstrip()
    parts = ns.split("::")
    open_ns = " ".join(f"namespace {p} {{" for p in parts)
    close_ns = " ".join("}" for _ in parts) + f"  // {ns}"

    return f"""// ============================================================================
// (AR) مولَّدٌ آليًّا من language-truth/backend/value_repr.yaml — لا تُحرّره يدويًّا.
// (EN) AUTO-GENERATED from language-truth/backend/value_repr.yaml — do not edit.
//      يُعاد توليده بـ`python x.py gen`؛ يحرسه `x.py gen --check`.
// ============================================================================
#ifndef {guard}
#define {guard}

#include <string>

{open_ns}
{body}
{close_ns}

#endif  // {guard}
"""


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate runtime value-representation constants from SoT.")
    ap.add_argument("--yaml", required=True, help="path to value_repr.yaml")
    ap.add_argument("--header", required=True, help="output C++ header path")
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    yaml_path = Path(args.yaml)
    if not yaml_path.is_file():
        _fatal(f"ملفُّ الـYAML غيرُ موجود: {yaml_path}")
    data = yaml.safe_load(yaml_path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        _fatal("جذرُ الـYAML يجب أن يكون خريطة")

    header = _emit(data)
    out = Path(args.header)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(header, encoding="utf-8", newline="\n")
    if not args.quiet:
        print(f"[gen_value_repr] wrote: {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
