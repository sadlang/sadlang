#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُولِّد arch_specific_opcodes_generated.h من
     language-truth/backend/arch_specific_opcodes.yaml
(EN) Generator for the architecture-constrained SIR opcode table.
----------------------------------------------------------------------------
(AR) الجدولُ يجيب سؤالًا واحدًا: هل خفضُ هذا الأوپكود يبثّ تعليماتٍ لا يفهمها
     إلّا معالجٌ من عائلةٍ بعينها؟ يستشيره `emitInstruction` قبل التوزيع،
     فيُبلِّغ خطأً مفهومًا بدل أن يخرج ثنائيٌّ لا يعمل — أو يُخفِق المُجمِّعُ
     برسالةٍ لا تدلّ على السبب.

     ويتحقّق المُولِّدُ أنّ كلَّ اسمٍ في الـYAML موجودٌ في كتالوج الأوپكودات
     المولَّد (`sir_opcodes.yaml`)، فالاسمُ الميّتُ يُخفِق التوليدَ لا يمرّ صامتًا.

الاستخدام:
    python gen_arch_specific_opcodes.py \\
        --yaml     language-truth/backend/arch_specific_opcodes.yaml \\
        --opcodes  language-truth/backend/sir_opcodes.yaml \\
        --out-h    compiler/include/backend/generated/arch_specific_opcodes_generated.h \\
        --quiet
============================================================================
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

try:
    import yaml
except ImportError:
    print("[gen_arch_specific_opcodes] FATAL: pyyaml not installed. pip install pyyaml",
          file=sys.stderr)
    sys.exit(1)

_THIS_DIR = Path(__file__).parent
_REPO_ROOT = _THIS_DIR.parents[1]
_DEFAULT_YAML = _REPO_ROOT / "language-truth" / "backend" / "arch_specific_opcodes.yaml"
_DEFAULT_OPCODES = _REPO_ROOT / "language-truth" / "backend" / "sir_opcodes.yaml"
_DEFAULT_OUT_H = (_REPO_ROOT / "compiler" / "include" / "backend" / "generated"
                  / "arch_specific_opcodes_generated.h")


# (AR) طرفيّةُ ويندوز بصفحةِ ترميزٍ ضيّقةٍ ترفع UnicodeEncodeError على الرسالة
#      العربيّة، فيسقط المُولِّدُ **بعد** كتابة الملفّ ⇒ خروجٌ غيرُ صفريٍّ يُقرأ
#      إخفاقَ توليدٍ وليس كذلك. الترميزُ يُضبَط صراحةً بدل الاعتماد على البيئة.
for _stream in (sys.stdout, sys.stderr):
    if hasattr(_stream, "reconfigure"):
        _stream.reconfigure(encoding="utf-8", errors="backslashreplace")


def _fatal(message: str) -> int:
    print(f"[gen_arch_specific_opcodes] FATAL: {message}", file=sys.stderr)
    return 1


def _load(path: Path) -> dict:
    with path.open(encoding="utf-8") as handle:
        return yaml.safe_load(handle)


def _emit_header(data: dict) -> str:
    families = {f["id"]: f for f in data["families"]}
    entries = sorted(data["opcodes"], key=lambda o: o["name"])

    lines: list[str] = []
    lines.append("// ============================================================================")
    lines.append("// (AR) مُولَّد آليًّا من language-truth/backend/arch_specific_opcodes.yaml")
    lines.append("//      لا يُحرَّر يدويًّا — عدِّل الـYAML ثمّ أعِد التوليد (`x.py gen`).")
    lines.append("// (EN) Auto-generated. Do not edit by hand.")
    lines.append("// ============================================================================")
    lines.append("#pragma once")
    lines.append("")
    lines.append("#include <array>")
    lines.append("#include <string_view>")
    lines.append("")
    lines.append("// (AR) تعدادُ الأوپكودات — المفتاحُ قيمةٌ لا نصّ.")
    lines.append("#include \"frontend/sir_types.h\"")
    lines.append("")
    lines.append("namespace Sad { namespace Backend {")
    lines.append("")
    lines.append("// (AR) قيدُ عائلةِ معالجٍ على خفضِ أوپكود.")
    lines.append("// (EN) A CPU-family constraint on an opcode's lowering.")
    lines.append("struct ArchConstrainedOpcode")
    lines.append("{")
    lines.append("    std::string_view opcode;   ///< (AR) اسمُ الأوپكود كما في SIROpcode")
    lines.append("    std::string_view familyId; ///< (AR) معرّفُ العائلة (x86…)")
    lines.append("    std::string_view familyAr; ///< (AR) اسمُ العائلة عربيًّا — لنصّ التشخيص")
    lines.append("    std::string_view witness;  ///< (AR) منمنمةٌ شاهدةٌ على القيد")
    lines.append("};")
    lines.append("")
    lines.append(f"inline constexpr std::array<ArchConstrainedOpcode, {len(entries)}> "
                 "ARCH_CONSTRAINED_OPCODES = {{")
    for entry in entries:
        family = families[entry["family"]]
        lines.append(f'    {{"{entry["name"]}", "{family["id"]}", '
                     f'"{family["name_ar"]}", "{entry["witness"]}"}},')
    lines.append("}};")
    lines.append("")

    for family in data["families"]:
        archs = family["llvm_archs"]
        const = family["id"].upper()
        lines.append(f"// (AR) أسماءُ llvm::Triple::ArchType المقبولةُ لعائلة {family['id']}.")
        lines.append(f"inline constexpr std::array<std::string_view, {len(archs)}> "
                     f"{const}_LLVM_ARCHS = {{{{")
        for arch in archs:
            lines.append(f'    "{arch}",')
        lines.append("}};")
        lines.append("")

    lines.append("// (AR) يعيد القيدَ إن كان الأوپكودُ مقيَّدًا، وإلّا nullptr.")
    lines.append("//")
    lines.append("//      ⚠️ المفتاحُ **قيمةُ التعداد** لا نصُّ الاسم. أوّلُ صياغةٍ طابقت")
    lines.append("//      اسمَ الأوپكود نصًّا عبر `sirOpcodeToString` — وتلك تعيد المنمنمة")
    lines.append("//      (`add.i64`) لا اسمَ التعداد، فلم تطابق ولا مرّةً واحدة: بوّابةٌ")
    lines.append("//      مبنيّةٌ تمامًا ولا تحرس شيئًا، وخضرةٌ بصفرِ تطابق. وبقيمة التعداد")
    lines.append("//      يصير الاسمُ الخاطئُ في مصدر الحقيقة **خطأَ ترجمة** لا صمتًا.")
    lines.append("// (EN) Keyed on the enum value, not the name: sirOpcodeToString returns")
    lines.append("//      the mnemonic, so a string match never fired. A wrong name in the")
    lines.append("//      SoT is now a compile error instead of a silent miss.")
    lines.append("inline const ArchConstrainedOpcode* findArchConstraint("
                 "::Sad::Compiler::SIR::SIROpcode opcode)")
    lines.append("{")
    lines.append("    using Op = ::Sad::Compiler::SIR::SIROpcode;")
    lines.append("    switch (opcode)")
    lines.append("    {")
    for index, entry in enumerate(entries):
        lines.append(f"    case Op::{entry['name']}: return &ARCH_CONSTRAINED_OPCODES[{index}];")
    lines.append("    default: return nullptr;")
    lines.append("    }")
    lines.append("}")
    lines.append("")
    lines.append("// (AR) هل معماريّةُ الهدف (باسم llvm::Triple) داخلَ العائلة المطلوبة؟")
    lines.append("inline bool archSatisfiesFamily(std::string_view familyId, std::string_view arch)")
    lines.append("{")
    for family in data["families"]:
        const = family["id"].upper()
        lines.append(f'    if (familyId == "{family["id"]}")')
        lines.append("    {")
        lines.append(f"        for (const auto& a : {const}_LLVM_ARCHS)")
        lines.append("            if (a == arch) return true;")
        lines.append("        return false;")
        lines.append("    }")
    lines.append("    // (AR) لا يُبلَغ: المُولِّدُ يُخفِق على أوپكودٍ يشير إلى عائلةٍ غيرِ")
    lines.append("    //      معلَنة، و`familyId` هنا يأتي دائمًا من `families`. و`false`")
    lines.append("    //      لا `true` كي يكون البلوغُ المستحيلُ **رفضًا صريحًا** إن وقع،")
    lines.append("    //      لا قيدًا يسقط صامتًا.")
    lines.append("    // (EN) Unreachable: the generator rejects an unknown family. false,")
    lines.append("    //      not true, so an impossible reach fails loudly.")
    lines.append("    return false;")
    lines.append("}")
    lines.append("")
    lines.append("} } // namespace Sad::Backend")
    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--yaml", type=Path, default=_DEFAULT_YAML,
                        help="مصدر الحقيقة: arch_specific_opcodes.yaml")
    parser.add_argument("--opcodes", type=Path, default=_DEFAULT_OPCODES,
                        help="كتالوج أوپكودات SIR للتحقّق من الأسماء")
    parser.add_argument("--out-h", type=Path, default=_DEFAULT_OUT_H,
                        help="الرأس المولَّد المراد كتابته")
    parser.add_argument("--quiet", action="store_true", help="كتم رسالة النجاح")
    args = parser.parse_args()

    data = _load(args.yaml)
    known = {o["name"] for o in _load(args.opcodes)["opcodes"]}
    families = {f["id"] for f in data["families"]}

    # (AR) اسمٌ لا وجودَ له في الكتالوج = قيدٌ على أوپكودٍ وهميٍّ لا يُطبَّق أبدًا.
    unknown = sorted({o["name"] for o in data["opcodes"]} - known)
    if unknown:
        return _fatal(f"أوپكوداتٌ غيرُ موجودةٍ في sir_opcodes.yaml: {unknown}")

    bad_family = sorted({o["name"] for o in data["opcodes"] if o["family"] not in families})
    if bad_family:
        return _fatal(f"عائلةٌ غيرُ معلَنةٍ لأوپكودات: {bad_family}")

    # (AR) أسماءُ المعماريّات تُقارَن بما يعيده `llvm::Triple::getArchTypeName`،
    #      وخطأٌ مطبعيٌّ فيها (`x86-64` بدل `x86_64`، أو `i686` بدل القانونيّ
    #      `i386`) **يُبطل القيدَ صامتًا** — البوّابةُ تُبنى وترفض المعماريّةَ التي
    #      يُفترَض أن تقبلها، أو تقبل ما يجب أن ترفضه. القائمةُ هنا مأخوذةٌ من
    #      تعداد LLVM؛ اسمٌ خارجها يُخفِق التوليد.
    known_archs = {
        "arm", "armeb", "aarch64", "aarch64_be", "aarch64_32", "arc", "avr",
        "bpfel", "bpfeb", "csky", "dxil", "hexagon", "loongarch32", "loongarch64",
        "m68k", "mips", "mipsel", "mips64", "mips64el", "msp430", "ppc", "ppcle",
        "ppc64", "ppc64le", "r600", "amdgcn", "riscv32", "riscv64", "sparc",
        "sparcv9", "sparcel", "systemz", "tce", "tcele", "thumb", "thumbeb",
        "i386", "x86_64", "xcore", "xtensa", "nvptx", "nvptx64", "le32", "le64",
        "amdil", "amdil64", "hsail", "hsail64", "spir", "spir64", "spirv",
        "spirv32", "spirv64", "kalimba", "shave", "lanai", "wasm32", "wasm64",
        "renderscript32", "renderscript64", "ve",
    }
    for family in data["families"]:
        unknown_arch = sorted(set(family["llvm_archs"]) - known_archs)
        if unknown_arch:
            return _fatal(
                f"أسماءُ معماريّاتٍ لا يعرفها llvm::Triple في العائلة «{family['id']}»: "
                f"{unknown_arch}. الاسمُ القانونيُّ لـi686 هو i386.")

    args.out_h.parent.mkdir(parents=True, exist_ok=True)
    args.out_h.write_text(_emit_header(data), encoding="utf-8")
    if not args.quiet:
        print(f"[gen_arch_specific_opcodes] {len(data['opcodes'])} أوپكودًا → {args.out_h}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
