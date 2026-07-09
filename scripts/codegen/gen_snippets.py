#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) مولِّد مقتطفات ص (VS Code snippets) من مصدر الحقيقة language-truth.
     يقرأ الكلمات المفتاحيّة الرسميّة (keywords.yaml) ويُنتج ‹sad.code-snippets›:
     قوالب البُنى الشائعة (دالة/رئيسية/إذا/لكل/بينما/طابق/بنية/تعداد/صنف/حاول-امسك/اختبر/…)
     **بكلماتها العربيّة المأخوذة من SoT** لتسريع الكتابة وتقليل أخطاء الإملاء. لا يوسّع SoT:
     الكلمات تُقرأ من keywords.yaml (لا تُكتب حرفيًّا)، وما عداها سقالة عرض (أقواس/مواضع إدراج).
(EN) Generates VS Code snippets for the Sad language from language-truth/keywords.yaml.
     The Arabic keywords come from SoT (never hardcoded); only the structural scaffolding
     (parentheses, tabstops, placeholder hints) is template. Mirrors gen_tmgrammar.py:
     LF-only output, a JSONC "generated" header, and an exact-compare --check drift guard.

الاستعمال: python scripts/codegen/gen_snippets.py [--check]
"""
import argparse
import json
import sys
from pathlib import Path

import yaml

# فرض UTF-8 على المخرجات (كونسول ويندوز قد يكون cp125x فيفشل مع العربيّة/الرموز ✅❌ — يُفسد
# رمز خروج --check). الملفّ نفسه يُكتب دومًا utf-8 صراحةً، هذا للطباعة فقط.
for _s in (sys.stdout, sys.stderr):
    try:
        _s.reconfigure(encoding="utf-8")
    except (AttributeError, ValueError):
        pass

# رأسا «مولَّد» (تعليقا JSONC — ملفّات .code-snippets تقبل التعليقات؛ كلّ مفتاح JSON = اسم مقتطف،
# فلا يمكن وضع مفتاح رأس نصّيّ كما في tmLanguage). يلتقطهما حارس محراب L0 كي لا يُحرَّر يدويًّا.
GENERATED_AR = "مولَّد من language-truth عبر scripts/codegen/gen_snippets.py — لا تُحرِّره يدويًّا."
GENERATED_EN = "Generated from language-truth by scripts/codegen/gen_snippets.py — do not edit."

# نصوص إرشاد مواضع الإدراج (tabstop placeholders) — بيانات محتوى المقتطف عربيّة-أوّلًا (لا منطق).
PH_NAME = "الاسم"
PH_PARAMS = "المعاملات"
PH_COND = "الشرط"
PH_ITEM = "عنصر"
PH_ITER = "المجموعة"
PH_VALUE = "القيمة"
PH_PATTERN = "النمط"
PH_ERR = "الخطأ"
PH_DESC = "الوصف"
PH_MODULE = "الوحدة"


def load_words(truth: Path) -> dict:
    """يبني {tokenType: word} من كلّ فئات keywords.yaml (مصدر الكلمات العربيّة). أوّل ظهور يفوز."""
    data = yaml.safe_load((truth / "keywords.yaml").read_text(encoding="utf-8"))
    words: dict[str, str] = {}
    for cat in (data.get("categories") or {}).values():
        for kw in (cat.get("keywords") or []):
            tok, word = kw.get("tokenType"), kw.get("word")
            if tok and word and tok not in words:
                words[tok] = word
    return words


def build_snippets(w: dict) -> dict:
    """يبني قاموس المقتطفات من كلمات SoT. قالبٌ يُتخطّى إن غاب أيّ رمز يعتمده (لا مقتطف بكلمة غائبة)."""
    snips: dict[str, dict] = {}

    def has(*toks: str) -> bool:
        return all(t in w for t in toks)

    def add(name: str, prefix: str, body: list[str], desc: str) -> None:
        snips[name] = {"prefix": prefix, "body": body, "description": desc}

    end = w.get("KEYWORD_END", "")

    # نقطة الدخول: دالة رئيسية() … نهاية
    if has("KEYWORD_FUNCTION", "KEYWORD_MAIN", "KEYWORD_END"):
        fn, mn = w["KEYWORD_FUNCTION"], w["KEYWORD_MAIN"]
        add(f"{fn} {mn}", mn, [f"{fn} {mn}()", "\t$0", end],
            "نقطة الدخول: دالّة رئيسيّة — main entry point")

    # تصريح دالّة: دالة الاسم(المعاملات) … نهاية
    if has("KEYWORD_FUNCTION", "KEYWORD_END"):
        fn = w["KEYWORD_FUNCTION"]
        add(fn, fn, [f"{fn} ${{1:{PH_NAME}}}(${{2:{PH_PARAMS}}})", "\t$0", end],
            "تصريح دالّة — function declaration")

    # شرط: إذا (الشرط) … نهاية
    if has("KEYWORD_IF", "KEYWORD_END"):
        kif = w["KEYWORD_IF"]
        add(kif, kif, [f"{kif} (${{1:{PH_COND}}})", "\t$0", end],
            "شرط — if statement")

    # شرط بديل: إذا (الشرط) … وإلا … نهاية
    if has("KEYWORD_IF", "KEYWORD_ELSE", "KEYWORD_END"):
        kif, kelse = w["KEYWORD_IF"], w["KEYWORD_ELSE"]
        add(f"{kif} {kelse}", kif,
            [f"{kif} (${{1:{PH_COND}}})", "\t$2", kelse, "\t$0", end],
            "شرط ببديل — if / else")

    # حلقة على مجموعة: لكل عنصر في المجموعة … نهاية (بلا أقواس — صيغة الأقواس مرفوضة)
    if has("KEYWORD_FOR", "KEYWORD_IN", "KEYWORD_END"):
        kfor, kin = w["KEYWORD_FOR"], w["KEYWORD_IN"]
        add(kfor, kfor,
            [f"{kfor} ${{1:{PH_ITEM}}} {kin} ${{2:{PH_ITER}}}", "\t$0", end],
            "حلقة على عناصر مجموعة — for-in loop")

    # حلقة شرطيّة: بينما (الشرط) … نهاية
    if has("KEYWORD_WHILE", "KEYWORD_END"):
        kwhile = w["KEYWORD_WHILE"]
        add(kwhile, kwhile, [f"{kwhile} (${{1:{PH_COND}}})", "\t$0", end],
            "حلقة شرطيّة — while loop")

    # مطابقة أنماط: طابق (القيمة) عندما النمط: … افتراضي: … نهاية
    if has("KEYWORD_MATCH", "KEYWORD_WHEN", "KEYWORD_DEFAULT", "KEYWORD_END"):
        km, kwhen, kdef = w["KEYWORD_MATCH"], w["KEYWORD_WHEN"], w["KEYWORD_DEFAULT"]
        add(km, km,
            [f"{km} (${{1:{PH_VALUE}}})", f"\t{kwhen} ${{2:{PH_PATTERN}}}:", "\t\t$3",
             f"\t{kdef}:", "\t\t$0", end],
            "مطابقة أنماط — match / when / default")

    # بنية: بنية الاسم … نهاية
    if has("KEYWORD_STRUCT", "KEYWORD_END"):
        kstruct = w["KEYWORD_STRUCT"]
        add(kstruct, kstruct, [f"{kstruct} ${{1:{PH_NAME}}}", "\t$0", end],
            "بنية — struct declaration")

    # تعداد: تعداد الاسم / عنصر … نهاية (عضوٌ واحدٌ إلزاميّ — repeat1 في gr.oop.enum؛
    # جسمٌ فارغ يُنتج خطأ تحليل، بخلاف بنية/صنف اللذين يقبلان جسمًا فارغًا)
    if has("KEYWORD_ENUM", "KEYWORD_END"):
        kenum = w["KEYWORD_ENUM"]
        add(kenum, kenum, [f"{kenum} ${{1:{PH_NAME}}}", f"\t${{0:{PH_ITEM}}}", end],
            "تعداد — enum declaration")

    # صنف: صنف الاسم … نهاية
    if has("KEYWORD_CLASS", "KEYWORD_END"):
        kclass = w["KEYWORD_CLASS"]
        add(kclass, kclass, [f"{kclass} ${{1:{PH_NAME}}}", "\t$0", end],
            "صنف — class declaration")

    # معالجة أخطاء: حاول … امسك الخطأ … نهاية (بلا أقواس — صيغة «امسك (اسم)» تُرفَض SYN001)
    if has("KEYWORD_TRY", "KEYWORD_CATCH", "KEYWORD_END"):
        ktry, kcatch = w["KEYWORD_TRY"], w["KEYWORD_CATCH"]
        add(f"{ktry} {kcatch}", ktry,
            [ktry, "\t$1", f"{kcatch} ${{2:{PH_ERR}}}", "\t$0", end],
            "معالجة أخطاء — try / catch")

    # اختبار: اختبر("الوصف") … نهاية (الأقواس حول الاسم إلزاميّة — gr.adv.property_test)
    if has("KEYWORD_TEST", "KEYWORD_END"):
        ktest = w["KEYWORD_TEST"]
        add(ktest, ktest, [f'{ktest}("${{1:{PH_DESC}}}")', "\t$0", end],
            "كتلة اختبار — test block")

    # متغيّر: متغير الاسم = القيمة
    if has("KEYWORD_VAR"):
        kvar = w["KEYWORD_VAR"]
        add(kvar, kvar, [f"{kvar} ${{1:{PH_NAME}}} = ${{0:{PH_VALUE}}}"],
            "تصريح متغيّر — variable declaration")

    # ثابت: ثابت الاسم = القيمة
    if has("KEYWORD_CONST"):
        kconst = w["KEYWORD_CONST"]
        add(kconst, kconst, [f"{kconst} ${{1:{PH_NAME}}} = ${{0:{PH_VALUE}}}"],
            "تصريح ثابت — constant declaration")

    # استيراد: استورد الوحدة
    if has("KEYWORD_IMPORT"):
        kimport = w["KEYWORD_IMPORT"]
        add(kimport, kimport, [f"{kimport} ${{0:{PH_MODULE}}}"],
            "استيراد وحدة — import")

    return snips


def render(truth: Path) -> str:
    words = load_words(truth)
    snips = build_snippets(words)
    header = f"// {GENERATED_AR}\n// {GENERATED_EN}\n"
    return header + json.dumps(snips, ensure_ascii=False, indent=2) + "\n"


def main() -> int:
    repo_root = Path(__file__).resolve().parents[2]
    ap = argparse.ArgumentParser(description="مولِّد مقتطفات ص (VS Code) من language-truth")
    ap.add_argument("--truth", type=Path, default=repo_root / "language-truth",
                    help="مجلّد مصدر الحقيقة (language-truth)")
    ap.add_argument("--out", type=Path,
                    default=repo_root / "editors" / "snippets" / "sad.code-snippets",
                    help="مسار ناتج المقتطفات")
    ap.add_argument("--check", action="store_true",
                    help="تحقّق فقط أنّ الناتج مطابق للمولَّد (لا كتابة) — لحارس الانجراف")
    args = ap.parse_args()

    text = render(args.truth)

    # نهايات أسطر LF حصرًا (يفرضها .gitattributes: eol=lf)؛ نُعطِّل ترجمة النظام (newline="")
    # في الكتابة والقراءة معًا كي لا ينجرف ناتج ويندوز (CRLF) وكي يلتقط --check أيّ CRLF يدويّ.
    # نستعمل open() الصريح لا read_text/write_text(newline=…) (معامل newline بـ3.13+، وCI قد يكون 3.12).
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
    n = text.count('"prefix":')
    print(f"✅ وُلِّد {args.out} ({n} مقتطفًا).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
