#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) توليدُ جدولِ نطاقاتِ المعاملاتِ من `language-truth/operators.yaml`.

     كان الجدولُ **مكتوبًا باليدِ في C++** داخل `TypeChecker::visitBinaryExpr`
     ومصدرُ الحقيقةِ لا يعرفُ منه شيئًا: لا حقلَ لنطاقِ المعاملاتِ في مخطَّطِ
     العواملِ إطلاقًا. فكان عقدُ اللغةِ — «ما الذي يقبلُه هذا العامل؟» — قائمًا
     في الشفرةِ وحدَها، لا يُقاسُ ولا يُراجَعُ ولا يُوثَّق.

     وأثرُه مقيسٌ لا مُدَّعًى (٢٠٢٦-٠٩-٠٣، ١٩٣ قياسًا): من العواملِ السبعةِ
     الحسابيّةِ الثنائيّةِ لم يكن يرفضُ شيئًا إلّا `**`. والبقيّةُ تقبلُ كلَّ
     تركيبةٍ وتُنتِجُ قمامةً أو تنهار — `10 - [1، 2، 3]` يطبعُ عنوانًا في
     الذاكرةِ برمزِ خروجٍ صفر، و`100 - "عشرة"` ينهارُ بإشارة.

     ⚠️ وقاعدةُ الحارسِ الحاكمة، وهي **ليست خيارًا لكلِّ عامل**:

         لا يُحكَمُ إلّا حين يكونُ الطرفانِ **كلاهما** من صنفٍ أعلنَه
         مصدرُ الحقيقة. وكلُّ ما عداه — `أي`، `عدم`، `فراغ`، `مجهول`،
         صنفٌ، تعدادٌ، اختياريّ، معاملُ دالّةٍ عارٍ — يمرُّ بلا حكم.

     وسببُ رفعِها إلى قاعدةٍ عامّةٍ مقيسٌ كذلك: الجدولُ اليدويُّ كان خلفَ
     `--أنواع-صارمة` ويُطلِقُ على `!isNumeric()`، و«لا أعرف» ليس عدديًّا —
     فكان يُحمِّرُ **٣٠ بذرةً صحيحة**، أبسطُها `ارجع أ + ب` في دالّةٍ
     بمعاملَين عاريَين. فالمِطلاقُ على **اليقين** لا على النوع.

(EN) Generates the operand-domain table from operators.yaml. The table used to
     be hand-written C++ inside TypeChecker::visitBinaryExpr while the SoT had
     no operand-domain field at all — the language contract lived in code only.
     Measured: of the seven binary arithmetic operators only `**` rejected
     anything; the rest accept every combination and produce garbage or crash.
     The governing rule, which is NOT a per-operator knob: judge only when BOTH
     operands are classified by the SoT; anything else passes unjudged.
============================================================================
"""
from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any

sys.path.insert(0, str(Path(__file__).resolve().parent))

try:
    from _lib import load_yaml, load_schema, validate_schema, write_if_changed
    from jsonschema import ValidationError
except ImportError:  # pragma: no cover
    print("[gen_operator_domains] FATAL: pyyaml/jsonschema not installed.", file=sys.stderr)
    sys.exit(2)

ROOT = Path(__file__).resolve().parents[2]

# (AR) اسمُ الرمزِ يُشتَقُّ من المعرِّفِ باطّرادٍ ثمّ **يُتحقَّقُ من وجودِه** في
#      `token.h`. والاشتقاقُ وحدَه لا يكفي: قاعدةُ تسميةٍ لا يحرسُها أحدٌ تنجرفُ
#      صامتةً يومَ يُعادُ تسميةُ رمزٍ، فيسقطُ الحارسُ عن عاملٍ بلا أن يحمرَّ شيء.
# (EN) The token name is derived from the id, then VERIFIED to exist in token.h.
TOKEN_HEADER = ROOT / "shared" / "lexer" / "include" / "token.h"


def token_name_for(op_id: str) -> str:
    """(AR) `op.floor_divide` ⇒ `OP_FLOOR_DIVIDE`."""
    if not op_id.startswith("op."):
        raise SystemExit(f"[gen_operator_domains] معرّف عامل غير متوقّع: {op_id}")
    return "OP_" + op_id[len("op."):].upper()


def declared_token_names() -> set[str]:
    text = TOKEN_HEADER.read_text(encoding="utf-8")
    names: set[str] = set()
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped.startswith("OP_"):
            continue
        head = stripped.split(",")[0].split("=")[0].strip()
        if head.replace("_", "").isalnum():
            names.add(head)
    return names


def type_id_to_kind(types_doc: dict[str, Any]) -> dict[str, str]:
    """(AR) `type.string` ⇒ `String` — من `types.yaml` لا من جدولٍ هنا."""
    return {t["id"]: t["kind"] for t in types_doc.get("types", [])}


def class_predicate(name: str, spec: dict[str, Any], id_to_kind: dict[str, str]) -> str:
    """(AR) شرطُ انتماءِ نوعٍ إلى صنفٍ — مشتقٌّ من إعلانِ الصنفِ لا مكتوبٌ هنا."""
    if "derive" in spec:
        # (AR) `types.yaml:numeric` — الحقلُ المُعلَنُ نفسُه، عبرَ المحمولِ المولَّدِ منه.
        if spec["derive"] != "types.yaml:numeric":
            raise SystemExit(
                f"[gen_operator_domains] اشتقاقٌ غير معروف للصنف «{name}»: {spec['derive']}")
        return "sadTypeKindIsNumeric(kind)"
    kinds = []
    for type_id in spec.get("types", []):
        if type_id not in id_to_kind:
            raise SystemExit(
                f"[gen_operator_domains] الصنف «{name}» يشير إلى نوعٍ غير معلَن: {type_id}")
        kinds.append(id_to_kind[type_id])
    if not kinds:
        raise SystemExit(f"[gen_operator_domains] الصنف «{name}» بلا أنواع")
    return " || ".join(f"kind == SadTypeKind::{k}" for k in kinds)


def as_list(value: Any) -> list[str]:
    return list(value) if isinstance(value, list) else [value]


def emit(ops: list[dict[str, Any]], classes: dict[str, Any],
         id_to_kind: dict[str, str]) -> str:
    guarded = [o for o in ops if "operands" in o]
    known = declared_token_names()

    lines: list[str] = []
    add = lines.append

    add("// ============================================================================")
    add("// AUTO-GENERATED from language-truth/operators.yaml — DO NOT EDIT MANUALLY")
    add("// (AR) مُولَّد تلقائياً من language-truth/operators.yaml — لا تعدّله يدوياً")
    add("// (EN) Auto-generated by scripts/codegen/gen_operator_domains.py")
    add("//")
    add("// (AR) نطاقاتُ معاملاتِ العوامل. والقاعدةُ الحاكمةُ — وليست خيارًا لكلِّ")
    add("//      عامل — أن لا يُحكَمَ إلّا حين يكونُ الطرفانِ **كلاهما** مصنَّفَين في")
    add("//      مصدرِ الحقيقة. فما لا يُصنَّف (`أي`، `عدم`، `فراغ`، `مجهول`، صنفٌ،")
    add("//      تعدادٌ، اختياريّ، معاملٌ عارٍ) يمرُّ بلا حكم — لأنّ حارسًا يقرأُ")
    add("//      «لا أعرف» حكمًا يتّهمُ برامجَ صحيحة (مقيسٌ: ٣٠ بذرة).")
    add("// (EN) Operand domains. Judge ONLY when both sides are classified by the")
    add("//      SoT; anything unclassified passes unjudged.")
    add("// ============================================================================")
    add("")
    add("#pragma once")
    add("")
    add('#include "sad_type_kind_generated.h"')
    add('#include "token.h"')
    add("")
    add("namespace Sad")
    add("{")
    add("    namespace Types")
    add("    {")
    add("        namespace OperandDomains")
    add("        {")
    add("            using ::Sad::Lexer::TokenType;")
    add("")

    # ─── محمولاتُ الأصناف ───
    for name in sorted(classes):
        pred = class_predicate(name, classes[name], id_to_kind)
        add(f"            /// (AR) صنف «{name}» — من operand_classes في مصدر الحقيقة.")
        add(f"            inline constexpr bool inClass_{name}(SadTypeKind kind)")
        add("            {")
        add(f"                return {pred};")
        add("            }")
        add("")

    # ─── أهو مصنَّفٌ أصلًا؟ ───
    add("            /// (AR) أَلِهذا النوعِ صنفٌ مُعلَن؟ وما لا صنفَ له لا يُحكَمُ عليه.")
    add("            /// (EN) Is this kind classified at all? Unclassified kinds are never judged.")
    add("            inline constexpr bool isClassified(SadTypeKind kind)")
    add("            {")
    add("                return " + "\n                       || ".join(
        f"inClass_{n}(kind)" for n in sorted(classes)) + ";")
    add("            }")
    add("")

    # ─── أللعاملِ حارسٌ؟ ───
    add("            /// (AR) أللعاملِ نطاقٌ مُعلَنٌ في مصدرِ الحقيقة؟ فما لا نطاقَ له لا حارسَ له.")
    add("            inline constexpr bool hasDomain(TokenType op)")
    add("            {")
    add("                switch (op)")
    add("                {")
    for o in guarded:
        tok = token_name_for(o["id"])
        if tok not in known:
            raise SystemExit(
                f"[gen_operator_domains] الرمز «{tok}» المشتقّ من «{o['id']}» "
                f"غير معلَن في token.h")
        add(f"                case TokenType::{tok}: // {o['symbol']}")
    add("                    return true;")
    add("                default:")
    add("                    return false;")
    add("                }")
    add("            }")
    add("")

    # ─── التوقيعات ───
    add("            /// (AR) أيقبلُ العاملُ هذين الطرفَين؟ يُسأَلُ بعدَ `isClassified` للطرفَين.")
    add("            /// (EN) Does the operator accept this pair? Ask only after isClassified.")
    add("            inline constexpr bool accepts(TokenType op, SadTypeKind left, SadTypeKind right)")
    add("            {")
    add("                switch (op)")
    add("                {")
    for o in guarded:
        tok = token_name_for(o["id"])
        add(f"                case TokenType::{tok}: // {o['symbol']} — {o['name_ar']}")
        add("                    return")
        sigs = o["operands"]["accepts"]
        for i, sig in enumerate(sigs):
            left_terms = " || ".join(f"inClass_{c}(left)" for c in as_list(sig["left"]))
            right_terms = " || ".join(f"inClass_{c}(right)" for c in as_list(sig["right"]))
            joiner = "       " if i == 0 else "    || "
            add(f"                        {joiner}(({left_terms})")
            add(f"                            && ({right_terms}))")
        add("                        ;")
    add("                default:")
    add("                    return true; // (AR) بلا نطاقٍ مُعلَن ⇒ بلا حارس")
    add("                }")
    add("            }")
    add("")

    # ─── رمزُ العاملِ للتشخيص ───
    add("            /// (AR) رمزُ العاملِ كما أعلنَه مصدرُ الحقيقة — لا كما يهجّيه المُشخِّص.")
    add("            ///      وبه يُملأُ نائبُ `{op}` في الرسالة، فالرمزُ نسخةٌ واحدةٌ لا نسختان.")
    add("            /// (EN) The operator's symbol as the SoT declares it, filling the")
    add("            ///      `{op}` placeholder so the spelling lives in one place only.")
    add("            inline constexpr const char *symbolOf(TokenType op)")
    add("            {")
    add("                switch (op)")
    add("                {")
    for o in guarded:
        tok = token_name_for(o["id"])
        add(f'                case TokenType::{tok}: return "{o["symbol"]}";')
    add('                default: return "";')
    add("                }")
    add("            }")
    add("")

    # ─── الحكمُ الواحد ───
    add("            /// (AR) الحكمُ في موضعٍ واحد: يُرفَضُ فقط ما هو **مقطوعٌ** بخروجِه")
    add("            ///      عن النطاق — عاملٌ محروسٌ، وطرفانِ مصنَّفانِ كلاهما، ولا")
    add("            ///      توقيعَ يوافقهما. وما سوى ذلك يمرّ.")
    add("            /// (EN) The single verdict: reject only what is CERTAINLY out of domain.")
    add("            inline constexpr bool violates(TokenType op, SadTypeKind left, SadTypeKind right)")
    add("            {")
    add("                return hasDomain(op)")
    add("                       && isClassified(left)")
    add("                       && isClassified(right)")
    add("                       && !accepts(op, left, right);")
    add("            }")
    add("")
    add("        } // namespace OperandDomains")
    add("    } // namespace Types")
    add("} // namespace Sad")
    add("")
    return "\n".join(lines)


def main() -> int:
    ap = argparse.ArgumentParser(description="Generate operand-domain table from SoT")
    ap.add_argument("--yaml", required=True, type=Path, help="path to operators.yaml")
    ap.add_argument("--schema", required=True, type=Path, help="path to operator.schema.json")
    ap.add_argument("--types", required=True, type=Path, help="path to types.yaml")
    ap.add_argument("--header", required=True, type=Path, help="output .h path")
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    doc = load_yaml(args.yaml)
    schema = load_schema(args.schema)
    ops = doc.get("operators") or []

    # (AR) المصادقةُ هنا **حمّالة**: مخطَّطُ العواملِ عاشَ سنينَ بلا مُصادِقٍ واحد
    #      (قائمةُ الأزواجِ في اختبارِ المخطَّطاتِ فارغةٌ منذ M0)، فانجرفت البياناتُ
    #      عنه حتّى صارَ ٤٣ مدخلًا من ٤٣ مخالِفًا. ووصلُها بمولِّدٍ يجري في
    #      `x.py gen --check` يجعلُ الانجرافَ يحمرُّ في بوّابةٍ قائمةٍ لا في
    #      اختبارٍ لا يُشغَّل.
    # (EN) Validation here is LOAD-BEARING: the operator schema had no validator at
    #      all, so the data drifted to 43/43 violations. Wiring it into a generator
    #      that runs under `x.py gen --check` makes drift redden in a live gate.
    for entry in ops:
        try:
            validate_schema(entry, schema)
        except ValidationError as exc:
            print(f"[gen_operator_domains] مخالفة مخطّط في «{entry.get('id')}»: {exc.message}",
                  file=sys.stderr)
            return 1

    classes = doc.get("operand_classes") or {}
    if not classes:
        print("[gen_operator_domains] FATAL: operand_classes غائبة من operators.yaml",
              file=sys.stderr)
        return 1

    types_doc = load_yaml(args.types)
    id_to_kind = type_id_to_kind(types_doc)

    # (AR) كلُّ صنفٍ يُذكَرُ في توقيعٍ لا بدّ أن يكونَ مُعلَنًا — وإلّا صمتَ الحارسُ
    #      عن توقيعٍ يظنُّ كاتبُه أنّه أعلنَه.
    for o in ops:
        for sig in (o.get("operands") or {}).get("accepts", []):
            for side in ("left", "right"):
                for c in as_list(sig[side]):
                    if c not in classes:
                        print(f"[gen_operator_domains] «{o['id']}» يذكر صنفًا غير معلَن: {c}",
                              file=sys.stderr)
                        return 1

    content = emit(ops, classes, id_to_kind)
    changed = write_if_changed(args.header, content)
    if not args.quiet:
        guarded = sum(1 for o in ops if "operands" in o)
        print(f"[gen_operator_domains] {len(ops)} عاملًا، {guarded} محروسًا "
              f"⇒ {args.header} ({'كُتب' if changed else 'بلا تغيير'})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
