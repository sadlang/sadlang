#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) مُولِّد نحو TextMate للغة ص من مصدر الحقيقة (language-truth)
(EN) TextMate grammar generator for Sad language from language-truth SoT
----------------------------------------------------------------------------
(AR) يقرأ المعجم الرسميّ (keywords/types/operators/directives) ويُنتج ملفّ
       sad.tmLanguage.json — قواعد الإبراز التي تستهلكها المحرّرات (محراب،
       إضافات VS Code). الهدف: الإبراز يتبع اللغة آليًّا فلا ينجرف يدويًّا.
     محراب «يستهلك مصدر الحقيقة لا يكرّره» — هذا الملفّ ناتج مولَّد لا يُحرَّر.
(EN) Reads the official lexicon (keywords/types/operators/directives) and emits
       sad.tmLanguage.json — the highlighting grammar consumed by editors
       (Mihrab, VS Code extensions). Highlighting follows the language
       automatically — no hand-maintained drift. Generated, do not edit.
----------------------------------------------------------------------------
(AR) حدود الكلمات العربيّة: \\b غير موثوق في Oniguruma مع العربيّة، فنستعمل
       نظرة خلفيّة/أماميّة سالبة على صنف محارف المُعرِّف.
(EN) Arabic word boundaries: \\b is unreliable in Oniguruma for Arabic, so we
       use negative look-behind/ahead against the identifier char class.
============================================================================
"""
from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any

# فرض UTF-8 على المخرجات (كونسول ويندوز قد يكون cp125x فيفشل مع العربيّة/الإيموجي).
for _s in (sys.stdout, sys.stderr):
    try:
        _s.reconfigure(encoding="utf-8")
    except (AttributeError, ValueError):
        pass

try:
    import yaml
except ImportError:  # pragma: no cover
    print("خطأ: PyYAML مطلوب (pip install pyyaml)", file=sys.stderr)
    sys.exit(2)

# صنف محارف المُعرِّف (حرف/رقم/شرطة سفليّة) — للحدود السالبة حول الكلمات.
IDENT = r"[\p{L}\p{Nd}_]"
# حدّ كلمة عربيّ-آمن: ليس قبله ولا بعده محرف مُعرِّف.
BOUND_OPEN = rf"(?<!{IDENT})"
BOUND_CLOSE = rf"(?!{IDENT})"


def load_yaml(path: Path) -> dict[str, Any]:
    if not path.is_file():
        print(f"خطأ: ملفّ مصدر الحقيقة غير موجود: {path}", file=sys.stderr)
        sys.exit(2)
    try:
        data = yaml.safe_load(path.read_text(encoding="utf-8"))
    except yaml.YAMLError as e:  # pragma: no cover
        print(f"خطأ: فشل تحليل YAML في {path}: {e}", file=sys.stderr)
        sys.exit(2)
    if data is None:
        return {}
    if not isinstance(data, dict):
        print(f"خطأ: جذر {path} يجب أن يكون خريطة (mapping).", file=sys.stderr)
        sys.exit(2)
    return data


def _alt(words: list[str]) -> str:
    """بديل regex من كلمات، الأطول أوّلًا (مطابقة جشعة صحيحة)، مع تهريب."""
    uniq = sorted({w for w in words if w}, key=lambda w: (-len(w), w))
    return "|".join(re.escape(w) for w in uniq)


def _keyword_match(words: list[str]) -> str:
    return f"{BOUND_OPEN}(?:{_alt(words)}){BOUND_CLOSE}"


def collect_keywords(kw: dict[str, Any]) -> dict[str, list[str]]:
    """يجمع الكلمات من فئات keywords.yaml حسب الدور الدلاليّ للإبراز."""
    cats = kw.get("categories", {})
    out: dict[str, list[str]] = {
        "control": [],      # تحكّم التدفّق + معالجة الأخطاء + مطابقة
        "declaration": [],  # دالة/صنف/بنية/تعداد/... (فاتحات كتل تعريف)
        "modifier": [],     # عام/خاص/... ومُعدِّلات
        "operator_word": [],# و/أو/ليس
        "constant": [],     # صحيح/خطأ/عدم
        "other": [],        # سياقيّة وبقيّة المحجوزة
    }

    def words_of(item: dict[str, Any]) -> list[str]:
        return [item["word"], *item.get("aliases", [])]

    DECL_TOKENS = {
        "KEYWORD_FUNCTION", "KEYWORD_CLASS", "KEYWORD_STRUCT", "KEYWORD_ENUM",
        "KEYWORD_CONSTRUCTOR", "KEYWORD_INHERITS",
    }
    MODIFIER_TOKENS = {
        "KEYWORD_PUBLIC", "KEYWORD_PRIVATE", "KEYWORD_PROTECTED",
        "KEYWORD_STATIC", "KEYWORD_CONST", "KEYWORD_MUT", "KEYWORD_FINAL",
    }
    # ملاحظة: مصدر الحقيقة يَسِم القيم الحرفيّة بـLITERAL_* لا KEYWORD_* — نطابقهما معًا
    # ليبقى الكشف صامدًا لو غُيِّر العُرف لاحقًا.
    CONST_TOKENS = {
        "LITERAL_TRUE", "LITERAL_FALSE", "LITERAL_NULL", "LITERAL_NONE",
        "KEYWORD_TRUE", "KEYWORD_FALSE", "KEYWORD_NULL", "KEYWORD_NONE",
    }

    def route(tok: str, ws: list[str]) -> None:
        # العوامل الكلميّة (OP_*) تُبرَز كعوامل أينما وردت (محجوزة أو سياقيّة).
        if tok.startswith("OP_"):
            out["operator_word"] += ws
        elif tok in DECL_TOKENS:
            out["declaration"] += ws
        elif tok in MODIFIER_TOKENS:
            out["modifier"] += ws
        elif tok in CONST_TOKENS:
            out["constant"] += ws
        else:
            out["control"] += ws

    for item in cats.get("reserved", {}).get("keywords", []):
        route(item.get("tokenType", ""), words_of(item))

    for item in cats.get("operators", {}).get("keywords", []):
        out["operator_word"] += words_of(item)

    for item in cats.get("contextual", {}).get("keywords", []):
        tok = item.get("tokenType", "")
        ws = words_of(item)
        if tok.startswith("OP_"):
            out["operator_word"] += ws
        else:
            out["other"] += ws

    return out


def collect_types(kw: dict[str, Any], types: dict[str, Any]) -> list[str]:
    """أسماء الأنواع: builtin_types من keywords + الأنواع السطحيّة من types.yaml."""
    names: list[str] = []
    for item in kw.get("categories", {}).get("builtin_types", {}).get("keywords", []):
        names.append(item["word"])
        names += item.get("aliases", [])
    for t in types.get("types", []) if isinstance(types.get("types"), list) else []:
        if t.get("surface") and t.get("word"):
            names.append(t["word"])
            names += t.get("aliases", [])
    return names


def collect_operator_symbols(ops: dict[str, Any]) -> list[str]:
    """العوامل الرمزيّة فقط؛ الكلميّة (و/أو/ليس/في/مؤكد) تُعالَج كـoperator_word.

    نستبعد أيّ رمز يحوي حرفًا (لاتينيًّا أو عربيًّا) أو رقمًا — لكنّنا نُبقي علامات
    الترقيم العربيّة كالاستفهام «؟» (عوامل «؟ :»/«؟؟»/«؟.»). الرمز المركَّب بفراغ
    («؟ :» الشَّرطيّ الثلاثيّ) يُقسَّم إلى قطعه (؟ ثمّ :) لأنّها رموز منفصلة في
    الشيفرة الفعليّة، لا متلاصقة.
    """
    out: list[str] = []
    for o in ops.get("operators", []):
        sym = o.get("symbol", "")
        if not sym:
            continue
        # نتجاهل الكلميّة: أيّ رمز فيه حرف أو رقم (يُعالَج كـoperator_word).
        if any(ch.isalpha() or ch.isdigit() for ch in sym):
            continue
        # رمز مركَّب بفراغ ⇒ قطع منفصلة (الترقيم لا يتلاصق في المصدر الفعليّ).
        for piece in sym.split():
            if piece:
                out.append(piece)
    return out


def collect_directive_words(dirs: dict[str, Any]) -> list[str]:
    """يستخرج اسم التوجيه بعد @ من symbol مثل «@حجم(نوع)» ⇒ «حجم»."""
    words: list[str] = []
    for d in dirs.get("directives", []):
        sym = d.get("symbol", "")
        # symbol مثل «@حجم(نوع)» ⇒ نأخذ ما بعد @ حتى أوّل قوس/فراغ. (re القياسيّ لا يدعم \p
        # فنفصل يدويًّا.) نتجنّب maxsplit الموضعيّ (مُهمَل في بايثون الحديث).
        if sym.startswith("@"):
            name = re.split(r"[(\s]", sym[1:].strip(), maxsplit=1)[0]
            if name:
                words.append(name)
    return words


def build_grammar(truth: Path) -> dict[str, Any]:
    kw = load_yaml(truth / "keywords.yaml")
    types = load_yaml(truth / "types.yaml")
    ops = load_yaml(truth / "operators.yaml")
    dirs = load_yaml(truth / "directives.yaml")

    groups = collect_keywords(kw)
    type_names = collect_types(kw, types)
    op_symbols = collect_operator_symbols(ops)
    directive_words = collect_directive_words(dirs)

    repository: dict[str, Any] = {}

    # تعليقات: مطابِقة لـgrammar/70_lexical.yaml (gr.lex.comment):
    #   سطر «#»، كتلة «#* *#»، توثيق سطر «##»، توثيق كتلة «#** **#».
    # ملاحظة ترتيب: الكتل (الأطول بادئةً) أوّلًا كي لا يبتلعها نمط السطر «#».
    repository["comments"] = {
        "patterns": [
            {"name": "comment.block.documentation.sad",
             "begin": r"#\*\*", "end": r"\*\*#"},
            {"name": "comment.block.sad",
             "begin": r"#\*", "end": r"\*#"},
            {"name": "comment.line.documentation.sad",
             "match": r"##.*$"},
            {"name": "comment.line.number-sign.sad",
             "match": r"#.*$"},
        ]
    }
    # استيفاء «{تعبير}» داخل f"...": نعاود الدخول إلى أنماط اللغة كي يُبرَز
    # المُحتوى (كلمات/أنواع/أرقام/عوامل) كأيّ تعبير عاديّ. نُضمِّن فقط الأقسام
    # المُولَّدة فعلًا (types/operators مشروطان) كي لا نُحيل على قسم مفقود.
    interp_includes: list[dict[str, str]] = [
        {"include": "#keywords"},
        {"include": "#numbers"},
    ]
    if type_names:
        interp_includes.append({"include": "#types"})
    if op_symbols:
        interp_includes.append({"include": "#operators"})

    # نصوص (gr.lex.string/raw_string/fstring): «"..."» مع هروب، خام «r"..."» بلا
    # هروب، منسَّق «f"...{تعبير}..."» مع إبراز الاستيفاء {...}.
    # ترتيب الهروب «\\.» قبل بدء الاستيفاء كي تُعامَل «\{» كحرف مهرَّب لا فاتحة استيفاء.
    repository["strings"] = {
        "patterns": [
            {"name": "string.quoted.raw.sad",
             "begin": r'(?<![\p{L}\p{Nd}_])r"', "end": r'"'},
            {"name": "string.quoted.interpolated.sad",
             "begin": r'(?<![\p{L}\p{Nd}_])f"', "end": r'"',
             "patterns": [
                 {"name": "constant.character.escape.sad", "match": r"\\."},
                 {"name": "meta.interpolation.sad", "begin": r"\{", "end": r"\}",
                  "patterns": interp_includes},
             ]},
            {"name": "string.quoted.double.sad", "begin": r'"', "end": r'"',
             "patterns": [{"name": "constant.character.escape.sad", "match": r"\\."}]},
        ]
    }
    # أعمار (gr.lex.lifetime): «'مُعرِّف» — قبل النصوص لا يلتبس (فاصلة عليا مفردة).
    repository["lifetimes"] = {
        "patterns": [
            {"name": "storage.modifier.lifetime.sad",
             "match": rf"'{IDENT}+"},
        ]
    }
    # أرقام (gr.lex.integer/double): عشريّ/سداسيّ‑عشريّ/ثنائيّ/ثمانيّ مع فاصل «_»،
    # وعشريّ بأُسّ اختياريّ. أرقام عربيّة (٠-٩) ولاتينيّة.
    DIGITS = r"[\d٠-٩]"
    repository["numbers"] = {
        "patterns": [
            # سداسيّ/ثنائيّ/ثمانيّ (لاتينيّ فقط حسب المعجم).
            {"name": "constant.numeric.hex.sad",
             "match": r"(?<![\p{L}\p{Nd}_])0[xX][0-9a-fA-F_]+"},
            {"name": "constant.numeric.binary.sad",
             "match": r"(?<![\p{L}\p{Nd}_])0[bB][01_]+"},
            {"name": "constant.numeric.octal.sad",
             "match": r"(?<![\p{L}\p{Nd}_])0[oO][0-7_]+"},
            # عشريّ مع كسر/أُسّ اختياريّ وفاصل «_».
            {"name": "constant.numeric.decimal.sad",
             "match": (rf"(?<![\p{{L}}\p{{Nd}}_]){DIGITS}(?:{DIGITS}|_)*"
                       rf"(?:\.{DIGITS}(?:{DIGITS}|_)*)?"
                       r"(?:[eE][+\-]?[\d٠-٩]+)?")},
        ]
    }
    repository["keywords"] = {"patterns": []}
    kw_pat = repository["keywords"]["patterns"]
    if groups["control"]:
        kw_pat.append({"name": "keyword.control.sad", "match": _keyword_match(groups["control"])})
    if groups["declaration"]:
        kw_pat.append({"name": "storage.type.sad", "match": _keyword_match(groups["declaration"])})
    if groups["modifier"]:
        kw_pat.append({"name": "storage.modifier.sad", "match": _keyword_match(groups["modifier"])})
    if groups["operator_word"]:
        kw_pat.append({"name": "keyword.operator.word.sad", "match": _keyword_match(groups["operator_word"])})
    if groups["constant"]:
        kw_pat.append({"name": "constant.language.sad", "match": _keyword_match(groups["constant"])})
    if groups["other"]:
        kw_pat.append({"name": "keyword.other.sad", "match": _keyword_match(groups["other"])})

    if type_names:
        repository["types"] = {
            "patterns": [{"name": "support.type.sad", "match": _keyword_match(type_names)}]
        }
    if directive_words:
        repository["directives"] = {
            "patterns": [{"name": "keyword.control.directive.sad",
                          "match": rf"@(?:{_alt(directive_words)}){BOUND_CLOSE}"}]
        }
    if op_symbols:
        repository["operators"] = {
            "patterns": [{"name": "keyword.operator.sad", "match": _alt(op_symbols)}]
        }

    includes = [
        {"include": "#comments"},
        {"include": "#strings"},
        {"include": "#lifetimes"},
        {"include": "#numbers"},
        {"include": "#directives"} if directive_words else None,
        {"include": "#keywords"},
        {"include": "#types"} if type_names else None,
        {"include": "#operators"} if op_symbols else None,
    ]
    includes = [i for i in includes if i]

    return {
        "$schema": "https://raw.githubusercontent.com/martinring/tmlanguage/master/tmlanguage.json",
        "_generated": "مولَّد من language-truth عبر scripts/codegen/gen_tmgrammar.py — لا تُحرِّره يدويًّا.",
        "name": "Sad",
        "scopeName": "source.sad",
        "patterns": includes,
        "repository": repository,
    }


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    ap = argparse.ArgumentParser(description="مولِّد نحو TextMate للغة ص من language-truth")
    ap.add_argument("--truth", type=Path, default=repo_root / "language-truth",
                    help="مجلّد مصدر الحقيقة (language-truth)")
    ap.add_argument("--out", type=Path,
                    default=repo_root / "editors" / "grammars" / "sad.tmLanguage.json",
                    help="مسار ناتج نحو TextMate")
    ap.add_argument("--check", action="store_true",
                    help="تحقّق فقط أنّ الناتج مطابق للمولَّد (لا كتابة) — لحارس الانجراف")
    args = ap.parse_args()

    grammar = build_grammar(args.truth)
    text = json.dumps(grammar, ensure_ascii=False, indent=2) + "\n"

    # نهايات أسطر LF حصرًا (يفرضها .gitattributes: eol=lf). نُعطِّل ترجمة النظام
    # (newline="") في الكتابة والقراءة معًا كي:
    #  • لا ينجرف ناتج ويندوز (CRLF) عن المُلتزَم المُسوَّى (LF)؛
    #  • يلتقط --check أيّ تحرير يدويّ يُدخِل CRLF بدل أن يُسوّيه صمتًا قبل المقارنة.
    # نستعمل open() الصريح لا Path.read_text/write_text(newline=...) لأنّ معامل
    # newline أُضيف إليهما في Python 3.13 فقط (CI قد يكون 3.12).
    if args.check:
        if not args.out.is_file():
            print(f"❌ الناتج غير موجود: {args.out} — شغّل المولِّد.", file=sys.stderr)
            return 1
        with open(args.out, "r", encoding="utf-8", newline="") as f:
            current = f.read()
        if current != text:
            print(f"❌ {args.out} منجرف عن language-truth — أعد توليده.", file=sys.stderr)
            return 1
        print(f"✅ {args.out.name} متطابق مع language-truth.")
        return 0

    args.out.parent.mkdir(parents=True, exist_ok=True)
    with open(args.out, "w", encoding="utf-8", newline="") as f:
        f.write(text)
    print(f"✅ وُلِّد {args.out} ({len(grammar['repository'])} مجموعة نمط).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
