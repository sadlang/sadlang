"""
============================================================================
(AR) مُولِّد ثوابتِ وسومِ تشخيصِ التخفيض الأصليّ (الخلفيّة السياديّة) من SoT — لغة ص
(EN) Generator for native-lowering diagnostic tag constants (sovereign backend)
----------------------------------------------------------------------------
(AR) يقرأ language-truth/backend/native_diagnostics.yaml (مصدرُ الحقيقة لوسومِ
     {detail} التي يمرّرها المخفّضان لـfail()) ويُنتج هيدر C++ فيه ثوابتُ
     `inline const std::string` في namespace sad::native::diag. فيصبح المخفّضان
     (sir_native_lowering.h / arm64_sir_lowering.h) بلا حرفيّاتِ وسومٍ خام —
     كلُّ وسمٍ ثابتٌ مسمّى مولَّدٌ من SoT (اتّساقًا مع مبدأ «لا نصوصٌ خام»).

     الهيدرُ مُلتزَمٌ في git (نمط go generate)؛ `x.py gen --check` حارسُ انجراف.
(EN) Reads the SoT diagnostic-tag YAML and emits a C++ header of inline const
     std::string constants in namespace sad::native::diag, so the lowerers carry
     no raw tag literals — every tag is a named, SoT-generated constant.
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
    print("[gen_native_diagnostics] FATAL: pyyaml not installed. pip install pyyaml", file=sys.stderr)
    sys.exit(1)

# (AR) الأقسامُ في الـYAML التي تحوي قوائمَ الوسوم (بترتيبٍ مستقرٍّ للمخرَج).
_SECTIONS = ("format", "pure", "kind", "tag", "vreg", "helper")
_IDENT_RE = re.compile(r"^k[A-Za-z0-9]+$")


def _fatal(msg: str) -> None:
    print(f"[gen_native_diagnostics] FATAL: {msg}", file=sys.stderr)
    sys.exit(1)


def _cpp_escape(s: str) -> str:
    """(AR) يهرّب سلسلةً لحرفيّةِ C++ (اقتباسٌ ورجعةٌ خلفيّة)."""
    return s.replace("\\", "\\\\").replace('"', '\\"')


def _emit(data: dict[str, Any]) -> str:
    ns = data.get("namespace")
    guard = data.get("guard")
    if not ns or not guard:
        _fatal("الـYAML يفتقر namespace أو guard")

    seen_names: set[str] = set()
    seen_values: dict[str, str] = {}
    lines: list[str] = []

    for section in _SECTIONS:
        entries = data.get(section) or []
        if not entries:
            continue
        lines.append(f"    // ── {section} ──")
        for e in entries:
            name = e.get("name")
            value = e.get("value")
            ar = e.get("ar", "")
            if name is None or value is None:
                _fatal(f"مدخلٌ في «{section}» بلا name/value: {e!r}")
            if not _IDENT_RE.match(name):
                _fatal(f"اسمُ ثابتٍ غيرُ صالح (يجب kCamelCase): {name!r}")
            if name in seen_names:
                _fatal(f"اسمُ ثابتٍ مكرَّر: {name!r}")
            if value in seen_values:
                _fatal(f"قيمةٌ مكرَّرة {value!r} بين {seen_values[value]!r} و{name!r}")
            seen_names.add(name)
            seen_values[value] = name
            comment = f"  // {ar}" if ar else ""
            lines.append(f'    inline const std::string {name} = "{_cpp_escape(value)}";{comment}')
        lines.append("")

    body = "\n".join(lines).rstrip()
    parts = ns.split("::")  # sad::native::diag
    open_ns = " ".join(f"namespace {p} {{" for p in parts)
    close_ns = " ".join("}" for _ in parts) + f"  // {ns}"

    return f"""// ============================================================================
// (AR) مولَّدٌ آليًّا من language-truth/backend/native_diagnostics.yaml — لا تُحرّره يدويًّا.
// (EN) AUTO-GENERATED from language-truth/backend/native_diagnostics.yaml — do not edit.
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
    ap = argparse.ArgumentParser(description="Generate native-lowering diagnostic tag constants from SoT.")
    ap.add_argument("--yaml", required=True, help="path to native_diagnostics.yaml")
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
        print(f"[gen_native_diagnostics] wrote: {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
