#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
============================================================================
(AR) حارسُ «حجمُ النوعِ يُشتَقُّ ولا يُكتَب».

     قِيس قبل هذا الحارس: `@حجم` كان **جدولَين مكتوبَين باليدِ** بسلاسلَ
     عربيّةٍ خامّة — `if/else if` في المترجّمِ و`unordered_map` ساكنةً في
     المفسّر — فتباعدا في **٦ من ١١** نوعًا (خريطة ٤٨/٢٤ · بايت ٠/٨ ·
     حرف ٠/١ · أي ٧٢/٨ · فراغ ٠/٨ · عدم ٠/٨)، و**افتراضاهما يكذبان**:
     نوعٌ مجهولٌ يُخرِجُ ٨ في المترجّمِ و٠ في المفسّرِ بلا تشخيص.

 🔑  والحارسُ يقيسُ **جسمَ الدالّةِ نفسِه** لا الملفَّ كلَّه: الملفُّ فيه
     تشخيصاتٌ عربيّةٌ مشروعةٌ ودوالُّ أُخرى، فمسحٌ على الملفِّ يحمرُّ كذبًا
     ويُطفَأ. يُقتطَعُ الجسمُ بموازنةِ الأقواسِ من توقيعِ الدالّةِ، ثمّ:
       ① لا سلسلةَ عربيّةً فيه البتّة — أيُّ لفظِ نوعٍ فيه جدولٌ يعود؛
       ② ولا بدَّ من ذكرِ `sadTypeKindSizeBytes` — فالاشتقاقُ يُثبَتُ حضورًا
          لا غيابًا وحدَه، وإلّا مرَّ جسمٌ فارغٌ أخضرَ.

(EN) Guard: @حجم must derive its number from the generated SoT table. It
     measures the FUNCTION BODY (brace-balanced), not the whole file: the
     files carry legitimate Arabic diagnostics elsewhere, and a file-wide
     scan would go red falsely and be switched off.
============================================================================
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

# (AR) (المسار، توقيعُ الدالّة) — بابا `@حجم` في المحرّكَين.
SITES = (
    (ROOT / "compiler" / "src" / "frontend" / "builders" / "expression_lowlevel.cpp",
     "BuildResult ExpressionBuilder::buildExprSizeof(AST::SizeofExpr *sizeofExpr)"),
    (ROOT / "interpreter" / "src" / "visitors" / "expression_evaluator_overloads.cpp",
     "void ExpressionEvaluator::visitSizeofExpr(AST::SizeofExpr &expr)"),
)

REQUIRED_CALL = "sadTypeKindSizeBytes"

# (AR) سلسلةٌ فيها حرفٌ عربيٌّ واحدٌ على الأقلّ — وهي وحدَها المقيسة.
#      والتعليقاتُ تُنزَع أوّلًا فلا تُتّهم شروحُ العلّةِ بأنّها جدول.
_ARABIC_STRING = re.compile(r'"[^"\n]*[؀-ۿ][^"\n]*"')
_LINE_COMMENT = re.compile(r"//[^\n]*")
_BLOCK_COMMENT = re.compile(r"/\*.*?\*/", re.DOTALL)


def _body_of(text: str, signature: str, path: Path) -> str:
    """(AR) جسمُ الدالّةِ بموازنةِ الأقواسِ — لا بعددِ أسطرٍ يُخمَّن."""
    start = text.find(signature)
    if start < 0:
        raise SystemExit(
            f"✗ لم يُعثَر على توقيعِ الدالّةِ في {path.name}: {signature}\n"
            "  إن أُعيدت تسميتُها فحدِّثِ الحارسَ — حارسٌ لا يجدُ هدفَه لا يحرس."
        )
    open_brace = text.find("{", start)
    if open_brace < 0:
        raise SystemExit(f"✗ توقيعٌ بلا جسمٍ في {path.name}")
    depth = 0
    for i in range(open_brace, len(text)):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[open_brace:i + 1]
    raise SystemExit(f"✗ جسمٌ غيرُ مُغلَقٍ في {path.name}")


def main() -> int:
    sys.stdout.reconfigure(encoding="utf-8")
    print("حارس «حجمُ النوعِ يُشتَقُّ ولا يُكتَب»:")
    failed = False
    for path, signature in SITES:
        if not path.is_file():
            print(f"  ✗ ملفٌّ مفقود: {path}")
            failed = True
            continue
        body = _body_of(path.read_text(encoding="utf-8"), signature, path)
        stripped = _LINE_COMMENT.sub("", _BLOCK_COMMENT.sub("", body))
        literals = _ARABIC_STRING.findall(stripped)
        derives = REQUIRED_CALL in stripped
        status = "✓" if (not literals and derives) else "✗"
        print(f"  {status} {path.name} — يشتقّ: {'نعم' if derives else 'لا'}"
              f" · سلاسلُ عربيّةٌ في الجسم: {len(literals)}")
        if literals:
            for lit in literals[:8]:
                print(f"      · {lit}")
            print("    ⇐ جدولُ أحجامٍ مكتوبٌ باليدِ عاد. الحجمُ من"
                  " `language-truth/types.yaml` (حقل `size_bytes`) وحدَه.")
            failed = True
        if not derives:
            print(f"    ⇐ لا نداءَ لـ`{REQUIRED_CALL}` — الرقمُ لا يُشتَقُّ من مصدرِ الحقيقة.")
            failed = True
    if failed:
        return 1
    print("  ✓ المحرّكان يقرآن الجدولَ المولَّد، ولا جدولَ يدويًّا في أيٍّ منهما.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
