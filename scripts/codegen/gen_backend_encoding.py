"""
============================================================================
(AR) مُولِّد جدول ترميز الخلفيّة السياديّة (x86-64، عائلة variable) من SoT — لغة ص
(EN) Generator for the sovereign backend encoding table (x86-64 variable family)
----------------------------------------------------------------------------
(AR) يقرأ language-truth/backend/<arch>/instructions.yaml (مصدر الحقيقة الوحيد
     لترميز التعليمات) ويُنتج هيدر C++ يحوي جدولَ EncSpec + دالّةَ بحث
     lookupEncSpec(المنمنمة، الصيغة). فيصبح المحرّكُ (x86_variable_encoder.h)
     منطقًا صرفًا يقرأ بياناتِه من الـYAML — تحقيقُ مبدأ «table-driven» في RFC
     الخلفيّة السياديّة: الاختلافُ بين التعليمات بياناتٌ لا كود.

     الهيدر مُلتزَمٌ في git (نمط go generate)؛ `x.py gen --check` حارسُ انجرافٍ
     يفشل إن لم يَعُد مطابقًا لِما يولّده الـYAML (بوّابة CI «فحص توليد الكود»).

(EN) Reads the SoT instruction-encoding YAML and emits a C++ header with an
     EncSpec table + lookupEncSpec(mnemonic, form). The encoder header becomes
     pure logic reading its data from YAML — realizing the table-driven principle.
============================================================================
"""
from __future__ import annotations

import argparse
import io
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
    print("[gen_backend_encoding] FATAL: pyyaml not installed. pip install pyyaml", file=sys.stderr)
    sys.exit(1)

_THIS_DIR = Path(__file__).parent
sys.path.insert(0, str(_THIS_DIR))
try:
    from _lib.emit import cpp_string_literal, write_if_changed  # type: ignore
except Exception:  # pragma: no cover
    def cpp_string_literal(s: str) -> str:  # type: ignore
        return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'

    def write_if_changed(path: Any = None, content: Any = None) -> bool:  # type: ignore
        p = Path(path)
        if p.exists() and p.read_text(encoding="utf-8") == content:
            return False
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(content, encoding="utf-8")
        return True

try:
    from _lib.loader import load_schema, validate_schema  # type: ignore
    _HAVE_VALIDATION = True
except Exception:  # pragma: no cover
    _HAVE_VALIDATION = False


def _op_index(ref: Any) -> int:
    """(AR) يحوّل مرجعَ معاملٍ «opN» إلى فهرسٍ صحيح. / (EN) 'opN' -> N."""
    if isinstance(ref, int):
        return -1  # (AR) عددٌ ثابت (امتداد أوپكود) لا مرجعُ معامل
    s = str(ref)
    if s.startswith("op"):
        return int(s[2:])
    raise ValueError(f"مرجع معامل غير صالح / invalid operand ref: {ref!r}")


def _first_reg_operand(operands: list[dict[str, Any]]) -> int:
    """(AR) فهرسُ أوّل معاملٍ من نوع سجلّ (لـ opcode_reg_add). / (EN) index of first reg operand."""
    for i, op in enumerate(operands):
        if op.get("kind") == "reg":
            return i
    return -1


def _emit_encspec(form: dict[str, Any]) -> str:
    """(AR) يبني EncSpec عبر lambda تُنفَّذ فورًا (تضبط الحقولَ اللازمة، والباقي افتراضيّ).
       C++17 لا يدعم المُهيّئات المُعيَّنة، فنبني بجُملٍ داخل []{...}().
    (EN) Builds an EncSpec via an immediately-invoked lambda (only the needed
       fields set; the rest keep their defaults). Avoids C++20 designated inits.
    """
    enc = form.get("encode", {})
    operands = form.get("operands", [])
    stmts: list[str] = []

    if enc.get("rex_w"):
        stmts.append("s.rex_w = true;")

    opcode = enc.get("opcode", [])
    opcode_lit = "{" + ", ".join(f"0x{b:02X}" for b in opcode) + "}"
    stmts.append(f"s.opcode = {opcode_lit};")

    if enc.get("opcode_reg_add"):
        stmts.append("s.opcode_reg_add = true;")
        stmts.append(f"s.opcode_reg_op = {_first_reg_operand(operands)};")

    modrm = enc.get("modrm")
    if modrm is not None:
        reg = modrm.get("reg")
        rm = modrm.get("rm")
        stmts.append("s.modrm.present = true;")
        # (AR) حقل reg اختياريّ (المخطّط يُلزم mod فقط): عددٌ ⇒ امتداد أوپكود، أو «opN» ⇒ معامل.
        if reg is not None:
            if isinstance(reg, int):
                stmts.append(f"s.modrm.reg_ext = {reg};")
            else:
                stmts.append(f"s.modrm.reg_op = {_op_index(reg)};")
        if rm is not None:
            stmts.append(f"s.modrm.rm_op = {_op_index(rm)};")

    imm = enc.get("immediate")
    if imm is not None:
        stmts.append(f"s.imm_op = {int(imm['operand'])};")
        stmts.append(f"s.imm_bits = {int(imm['bits'])};")

    body = " ".join(stmts)
    return "[]{ sad::native::x86::EncSpec s; " + body + " return s; }()"


BANNER = (
    "// ============================================================================\n"
    "// (AR) مولَّدٌ آليًّا من language-truth/backend/x86_64/instructions.yaml — لا تُحرّره.\n"
    "//      أعِد توليدَه بـ: python x.py gen  (وحارسُ الانجراف: python x.py gen --check).\n"
    "// (EN) AUTO-GENERATED from the SoT instruction-encoding YAML. DO NOT EDIT.\n"
    "//      Regenerate with: python x.py gen   (drift guard: python x.py gen --check).\n"
    "// ============================================================================\n"
)


def generate(yaml_path: Path, header_path: Path, schema_path: Path | None) -> None:
    data = yaml.safe_load(yaml_path.read_text(encoding="utf-8"))
    if schema_path and _HAVE_VALIDATION:
        try:
            validate_schema(data, load_schema(schema_path))
        except Exception as e:  # pragma: no cover
            print(f"[gen_backend_encoding] schema validation failed: {e}", file=sys.stderr)
            sys.exit(1)

    arch = data["architecture"]
    rows: list[str] = []
    for mnemonic, forms in data["instructions"].items():
        for form in forms:
            spec = _emit_encspec(form)
            rows.append(
                f"    {{ {cpp_string_literal(mnemonic)}, "
                f"{cpp_string_literal(form['form'])}, {spec} }},"
            )

    lines: list[str] = []
    lines.append(BANNER)
    lines.append("#ifndef SAD_NATIVE_X86_64_ENCODING_GENERATED_H")
    lines.append("#define SAD_NATIVE_X86_64_ENCODING_GENERATED_H")
    lines.append("")
    lines.append('#include "backend/native/x86_variable_encoder.h"')
    lines.append("")
    lines.append("#include <string>")
    lines.append("#include <vector>")
    lines.append("")
    lines.append("namespace sad { namespace native { namespace x86 {")
    lines.append("")
    lines.append(f"// (AR) جدول ترميز {arch} المولَّد من SoT (منمنمة، صيغة، EncSpec).")
    lines.append("struct GenEncEntry { std::string mnemonic; std::string form; EncSpec spec; };")
    lines.append("")
    lines.append("inline const std::vector<GenEncEntry> &encodingTable()")
    lines.append("{")
    lines.append("    static const std::vector<GenEncEntry> kTable = {")
    lines.extend(rows)
    lines.append("    };")
    lines.append("    return kTable;")
    lines.append("}")
    lines.append("")
    lines.append("// (AR) بحثٌ عن مواصفة الترميز بالمنمنمة والصيغة؛ يعيد nullptr إن لم تُوجد.")
    lines.append("// (EN) look up an encoding spec by mnemonic + form; nullptr if absent.")
    lines.append("inline const EncSpec *lookupEncSpec(const std::string &mnemonic, const std::string &form)")
    lines.append("{")
    lines.append("    for (const auto &e : encodingTable())")
    lines.append("        if (e.mnemonic == mnemonic && e.form == form)")
    lines.append("            return &e.spec;")
    lines.append("    return nullptr;")
    lines.append("}")
    lines.append("")
    lines.append("}}} // namespace sad::native::x86")
    lines.append("")
    lines.append("#endif // SAD_NATIVE_X86_64_ENCODING_GENERATED_H")

    content = "\n".join(lines) + "\n"
    changed = write_if_changed(header_path, content)
    if not (schema_path and "--quiet" in sys.argv):
        print(f"[gen_backend_encoding] {'wrote' if changed else 'unchanged'}: {header_path}")


def main() -> None:
    ap = argparse.ArgumentParser(description="Generate the sovereign backend encoding table header.")
    ap.add_argument("--yaml", required=True)
    ap.add_argument("--schema", default=None)
    ap.add_argument("--header", required=True)
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()
    generate(Path(args.yaml), Path(args.header),
             Path(args.schema) if args.schema else None)


if __name__ == "__main__":
    main()
