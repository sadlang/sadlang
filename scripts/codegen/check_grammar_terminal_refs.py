# -*- coding: utf-8 -*-
"""(AR) حارسٌ: كلُّ `terminal.ref` في قواعد النحو له وجودٌ في كتالوج الرموز.

العلّةُ المقيسة (٢٠٢٦-٠٨-١٠): كانت `language-truth/grammar/*.yaml` تحيل في
٢٢ موضعًا إلى `OP_COMMA` وهو رمزٌ **لا وجودَ له** في أيٍّ من المصادر الثلاثة
التي يُعلنها `_notation.yaml` (`keywords.yaml` · `operators.yaml` ·
`shared/lexer/include/token.h`)؛ والرمزان الحقيقيّان `ARABIC_COMMA` و`COMMA`.
ومعها `EOF` (والصوابُ `END_OF_FILE`) و`KEYWORD_ITERATIONS`/`KEYWORD_SEED`
(وهما سياقيّتان تُطابَقان `IDENTIFIER` بقيمتِهما). ولم يكن ثمّة حارسٌ يقابل
هذه الإحالاتِ بالكتالوج، فكانت البوّابةُ خضراءَ والإحالاتُ شبحيّة — أخضرُ
تغطيةٍ لا أخضرَ سلامة. وأثرُ الشبحِ يقع على مستهلِكي SoT (مُبرِزُ نحوٍ، LSP،
مُحلِّلٌ مولَّد): يحلّ الإحالةَ فلا يجد شيئًا، أو يُصدِر الرسمَ العربيَّ وحدَه
فيرفض ما يقبله المُحلِّلُ الفعليّ.

الكشف: كلُّ `ref:` تحت `kind: terminal` وكلُّ `separator:` — يجب أن يكون
اسمَ رمزٍ في `token.h`، أو معرّفًا في `keywords.yaml`/`operators.yaml`.
"""
import re
import sys
from pathlib import Path

for _تيّار in (sys.stdout, sys.stderr):
    if hasattr(_تيّار, "reconfigure"):
        _تيّار.reconfigure(encoding="utf-8")

الجذر = Path(__file__).resolve().parents[2]
مسار_الرموز = الجذر / "shared" / "lexer" / "include" / "token.h"
مجلد_القواعد = الجذر / "language-truth" / "grammar"
مصادر_المعرّفات = [
    الجذر / "language-truth" / "keywords.yaml",
    الجذر / "language-truth" / "operators.yaml",
]


def اجمع_أسماء_الرموز() -> set:
    نصّ = مسار_الرموز.read_text(encoding="utf-8")
    return set(re.findall(r"^\s*([A-Z][A-Z0-9_]*)\s*(?:=\s*[^,]+)?,", نصّ, re.M))


def اجمع_المعرّفات() -> set:
    معرّفات = set()
    for مسار in مصادر_المعرّفات:
        if not مسار.exists():
            continue
        نصّ = مسار.read_text(encoding="utf-8")
        معرّفات |= set(re.findall(r"^\s*(?:-\s*)?id:\s*([A-Za-z0-9_\-]+)", نصّ, re.M))
        معرّفات |= set(re.findall(r"^\s*token_type:\s*([A-Z][A-Z0-9_]*)", نصّ, re.M))
    return معرّفات


def افحص() -> int:
    معروفة = اجمع_أسماء_الرموز() | اجمع_المعرّفات()
    if not معروفة:
        print("[حارس] ❌ تعذّر قراءة كتالوج الرموز — لا حكمَ بلا مرجع", file=sys.stderr)
        return 1
    مخالفات = []
    for ملفّ in sorted(مجلد_القواعد.glob("*.yaml")):
        for رقم, سطر in enumerate(ملفّ.read_text(encoding="utf-8").splitlines(), 1):
            إحالات = re.findall(r"kind:\s*terminal,\s*ref:\s*([A-Za-z_][A-Za-z0-9_\-]*)", سطر)
            إحالات += re.findall(r"^\s*separator:\s*([A-Za-z_][A-Za-z0-9_\-]*)", سطر)
            for إحالة in إحالات:
                if إحالة not in معروفة:
                    مخالفات.append((ملفّ.name, رقم, إحالة))
    if مخالفات:
        print("[حارس] ❌ إحالاتُ رموزٍ شبحيّة في قواعد النحو:", file=sys.stderr)
        for اسم, رقم, إحالة in مخالفات:
            print(f"  language-truth/grammar/{اسم}:{رقم} ⇐ {إحالة}", file=sys.stderr)
        print(
            "  الإحالةُ يجب أن تكون اسمَ رمزٍ في shared/lexer/include/token.h "
            "أو معرّفًا في keywords.yaml/operators.yaml (انظر terminal_ref_sources).",
            file=sys.stderr,
        )
        return 1
    print("[حارس] ✅ كلّ إحالات الرموز في قواعد النحو موجودة في الكتالوج")
    return 0


if __name__ == "__main__":
    sys.exit(افحص())
