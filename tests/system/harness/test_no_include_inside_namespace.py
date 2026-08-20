#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسٌ: لا `#include` داخلَ فضاءِ أسماء.

🔑 لماذا: ترويسةُ النظامِ تُصادَر بأسرِها إلى فضاءِ الأسماءِ الذي ضُمَّت فيه، ولا
    يقف الأثرُ عند ملفِّها: حارسُ الضمِّ في الترويسةِ يجعل أوّلَ ضمٍّ هو الوحيدَ في
    وحدةِ الترجمةِ كلِّها، فيُبتلَع كلُّ ضمٍّ لاحقٍ في النطاقِ العامِّ **صامتًا**
    بحارسِه. فتغيب أسماءُ النظامِ عن النطاقِ العامّ لا بعطبٍ في المُنادي بل بحسبِ
    **ترتيبِ** الضمِّ عنده — ومُنادٍ سليمٌ يسقط لأنّ ضمًّا جديدًا سبقَ ضمَّه.

🔑 القياس: `<unistd.h>` كان مضمومًا داخلَ `sad::utf8` في `utf8_utils.h`. لم يظهر
    أثرُه حتّى ضُمَّت الترويسةُ في `compiler_driver_linker.cpp` **قبلَ** ضمِّه
    العامَّ لـ`<unistd.h>`، فصار `::getpid` و`::readlink` معدومَين في النطاقِ
    العامّ وسقط بناءُ لينكس. وماكُ وويندوزُ خضراوان في الشوطِ نفسِه: فرعُ ماكَ
    يضمُّ ترويسةً أخرى وويندوزُ لا يضمُّ POSIX أصلًا. أي أنّ منصّتَين من ثلاثٍ
    تُخفيان العطب.

🔑 المرساة **فعلٌ** لا هجاءُ سطرٍ: كلمةُ `namespace` تُعلِّق فتحًا، وأوّلُ `{`
    بعدَها يفتحه ولو بعدَ أسطر. أوّلُ صياغةٍ لهذا الحارسِ اشترطت `{` في سطرِ
    `namespace` نفسِه فعمِيت عن أسلوبِ آلمَن — وهو الغالبُ في هذه الشجرة — فمرَّ
    حقنٌ مقصودٌ أخضرَ. المُحرَّمُ أن يَظهرَ توجيهُ `#include` وعمقُ فضاءِ الأسماءِ
    أكبرُ من صفر، أيًّا كانت الترويسةُ وأيًّا كان شكلُ الأقواس.

🔑 والنصُّ الخامُّ يُطوى: مولِّدُ أندرويدَ يكتب شيفرةَ C داخلَ `R"( … )"`، وفيها
    `#include <stdio.h>` سطرًا **مولَّدًا** لا مضمومًا. عدُّه انتهاكًا يجعل
    الحارسَ يُحمِّر على نصٍّ لا على فعلٍ — فيُسكَت بقائمةِ إذنٍ ثمّ يموت.

(EN) Guard: no `#include` inside a namespace.
    A header included inside a namespace is captured into it whole, and the
    damage is not local: its include guard then makes that the ONLY inclusion in
    the translation unit, so a later global #include is silently swallowed.
    Global names vanish from global scope depending on include ORDER, so a
    correct caller breaks because some new include landed above its own.
    Measured: <unistd.h> inside sad::utf8 made ::getpid/::readlink undeclared
    and broke the Linux build while macOS and Windows stayed green.
    Raw string literals are skipped: generated C source is text, not an include.

الاستعمال / Usage:
    python tests/system/harness/test_no_include_inside_namespace.py
    rc=0 نظيف · rc=1 انتهاك · rc=2 الحارسُ نفسُه معطوب
"""

import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]

# (AR) جذورُ الشيفرةِ الأصليّةِ وحدَها — لا شيفرةَ طرفٍ ثالثٍ ولا مجلّدَ بناء.
# (EN) First-party roots only — no third-party code, no build directories.
SOURCE_ROOTS = ["shared", "compiler", "interpreter", "tools", "apps", "runtime"]
SOURCE_SUFFIXES = {".h", ".hpp", ".hh", ".cpp", ".cc", ".cxx", ".inc"}

# (AR) أرضيّةٌ تُحصي **الملفّاتِ الممسوحة** لا الانتهاكات: لو رثّت الجذورُ باسمٍ
#      تبدّل لمسح الحارسُ صفرًا ومرَّ أخضرَ إلى الأبد. حارسٌ لا يرى شيئًا لا
#      يُحمِّر أبدًا.
# (EN) The floor counts SCANNED FILES, not violations: if a root is renamed the
#      guard would scan nothing and pass forever. A guard that sees nothing can
#      never fire.
MIN_SCANNED_FILES = 700

# (AR) `namespace` تُعلِّق فتحًا وأوّلُ `{` بعدَها يفتحه — ولو بعدَ أسطر. و`;` أو
#      `=` تُلغي التعليقَ فلا يُحسَب `using namespace X;` ولا الاسمُ البديلُ فتحًا.
# (EN) `namespace` arms an opening; the next `{` opens it, even lines later.
#      A `;` or `=` disarms it, so `using namespace X;` and aliases never count.
_WORD = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
_INCLUDE = re.compile(r"^\s*#\s*include\b")
_RAW_OPEN = re.compile(r"(?:u8|u|U|L)?R\"([^ ()\\\t]*)\(")
_QUOTE_MARKS = (chr(34), chr(39))


def _strip_code(line: str, in_block_comment: bool, raw_delimiter):
    """(AR) يُفرِغ التعليقاتِ والنصوصَ — العاديَّ منها والخامَّ — فلا تُحسَب أقواسُها."""
    out = []
    index = 0
    length = len(line)
    while index < length:
        if raw_delimiter is not None:
            closer = ")" + raw_delimiter + chr(34)
            end = line.find(closer, index)
            if end == -1:
                return "".join(out), in_block_comment, raw_delimiter
            raw_delimiter = None
            index = end + len(closer)
            continue
        if in_block_comment:
            end = line.find("*/", index)
            if end == -1:
                return "".join(out), True, None
            in_block_comment = False
            index = end + 2
            continue
        char = line[index]
        pair = line[index:index + 2]
        if pair == "/*":
            in_block_comment = True
            index += 2
            continue
        if pair == "//":
            break
        if char in "RuUL":
            previous = line[index - 1] if index else " "
            if not (previous.isalnum() or previous == "_"):
                opener = _RAW_OPEN.match(line, index)
                if opener is not None:
                    raw_delimiter = opener.group(1)
                    index = opener.end()
                    continue
        if char in _QUOTE_MARKS:
            quote = char
            index += 1
            while index < length:
                if line[index] == "\\":
                    index += 2
                    continue
                if line[index] == quote:
                    index += 1
                    break
                index += 1
            continue
        out.append(char)
        index += 1
    return "".join(out), in_block_comment, raw_delimiter


def scan_file(path: Path):
    """(AR) يُعيد قائمةَ (رقمُ السطرِ، نصُّه) لكلِّ ضمٍّ داخلَ فضاءِ أسماء."""
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        return []
    findings = []
    depth = 0                 # عمقُ الأقواسِ الكلّيّ / total brace depth
    namespace_depths = []     # أعماقُ فتحِ فضاءاتِ الأسماء / namespace open depths
    pending_namespace = False
    in_block_comment = False
    raw_delimiter = None
    for number, raw_line in enumerate(text.splitlines(), start=1):
        # (AR) الحكمُ بحالةِ **مطلعِ** السطر: ضمٌّ داخلَ نصٍّ خامٍّ أو تعليقٍ كتلةٍ
        #      سطرٌ مكتوبٌ لا فعلٌ مُنفَّذ.
        # (EN) Judge by the state at line START: an include inside a raw string
        #      or a block comment is written text, not an executed act.
        quoted = raw_delimiter is not None or in_block_comment
        code, in_block_comment, raw_delimiter = _strip_code(
            raw_line, in_block_comment, raw_delimiter)
        if namespace_depths and not quoted and _INCLUDE.match(raw_line):
            findings.append((number, raw_line.strip()))
        # (AR) توجيهُ المعالجِ لا يُغيّر العمق — و`#define` قد يحمل قوسًا وحيدًا.
        # (EN) Preprocessor lines never change depth; a #define may hold a brace.
        if code.lstrip().startswith("#"):
            continue
        index = 0
        length = len(code)
        while index < length:
            char = code[index]
            if char == "{":
                depth += 1
                if pending_namespace:
                    namespace_depths.append(depth - 1)
                    pending_namespace = False
                index += 1
            elif char == "}":
                depth -= 1
                while namespace_depths and depth <= namespace_depths[-1]:
                    namespace_depths.pop()
                index += 1
            elif char in ";=":
                pending_namespace = False
                index += 1
            elif char.isalpha() or char == "_":
                word = _WORD.match(code, index)
                if word is None:
                    index += 1
                    continue
                if word.group(0) == "namespace":
                    pending_namespace = True
                index = word.end()
            else:
                index += 1
    return findings


def main() -> int:
    scanned = 0
    violations = []
    missing_roots = []

    for root_name in SOURCE_ROOTS:
        root = REPO_ROOT / root_name
        if not root.is_dir():
            missing_roots.append(root_name)
            continue
        for path in sorted(root.rglob("*")):
            if not path.is_file() or path.suffix.lower() not in SOURCE_SUFFIXES:
                continue
            scanned += 1
            for number, line in scan_file(path):
                relative = path.relative_to(REPO_ROOT).as_posix()
                violations.append((relative, number, line))

    if missing_roots:
        print("❌ الحارسُ يشير إلى جذورٍ غيرِ موجودة / guard points at missing roots:")
        for root_name in missing_roots:
            print(f"   - {root_name}")
        print("   حدِّثْ SOURCE_ROOTS — لا تُسكِتِ الحارسَ بحذفِ صفِّه.")
        return 2

    if scanned < MIN_SCANNED_FILES:
        print(f"❌ الماسحُ رأى {scanned} ملفًّا فقط (الأرضيّة {MIN_SCANNED_FILES}).")
        print("   المِرساةُ انزاحت عن شجرةِ المصدر — حارسٌ لا يرى شيئًا لا يُحمِّر.")
        return 2

    if violations:
        print("❌ ضمٌّ داخلَ فضاءِ أسماء / #include inside a namespace:")
        for relative, number, line in violations:
            print(f"   {relative}:{number}")
            print(f"      {line}")
        print()
        print("   الإصلاح: انقلِ الضمَّ إلى رأسِ الملفِّ خارجَ كلِّ فضاءِ أسماء،")
        print("   وكرِّرْ شرطَ المنصّةِ هناك إن لزم.")
        print("   Fix: hoist the include to file scope, outside every namespace,")
        print("   repeating the platform condition there if needed.")
        return 1

    print(f"✅ لا ضمَّ داخلَ فضاءِ أسماء — {scanned} ملفًّا ممسوحًا "
          f"في {len(SOURCE_ROOTS)} جذورٍ.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
