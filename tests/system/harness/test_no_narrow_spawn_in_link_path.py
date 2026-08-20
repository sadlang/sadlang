#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(AR) حارسٌ: لا نداءَ عمليّةٍ ضيّقَ الترميزِ في مسارِ إنتاجِ التنفيذيّ.

🔑 لماذا: `std::system` و`CreateProcessA` تأخذان `char*`، فيُعيد ويندوز تفسيرَ
    بايتاتِ UTF-8 بترميزِ النظامِ (ACP). فمسارٌ عربيٌّ تحت ترميزٍ لاتينيٍّ يجعل
    الرابطَ **ينجح** ورمزُ عائدِه صفرٌ، ويكتب المخرَجَ باسمٍ مشوَّه. لا خطأَ ولا
    تشخيصَ — يغيب التنفيذيُّ عن موضعِه وحسب، فيُقرأ ذلك «لم يُنتج ملف تنفيذي».
    كُشِف حين قِيست مصفوفةُ القواعدِ على ويندوز لأوّلِ مرّة: ٩٣ ملفًّا اسمُه
    عربيٌّ أخفقت، و٢٩٢٧ اسمُها ASCII نجحت — والمجلّدُ العربيُّ لم يضرّ.

🔑 المرساة **فعلٌ** لا هجاءُ اسمٍ: المُحرَّمُ أن يعبر **مسارٌ مبنيٌّ** واجهةً
    ضيّقة. أمّا أمرُ استكشافٍ ثابتٌ بحرفيّةٍ ASCII واحدةٍ (`where clang.exe`)
    فلا يحمل مسارَ مستخدمٍ أصلًا، فيُسمَحُ به باشتقاقٍ لا بقائمةِ إذنٍ تبلى:
    الحكمُ هو «هل الوسيطُ حرفيّةٌ واحدةٌ ASCII بلا ضمٍّ؟» لا اسمُ الملفّ.

(EN) Guard: no narrow-encoding process spawn on the executable-production path.
    std::system/CreateProcessA take char*, so Windows reinterprets UTF-8 bytes
    through the ANSI codepage; an Arabic path under a Latin ACP makes the linker
    SUCCEED with exit code 0 while writing to a mangled name. The allowance for
    fixed ASCII probe commands is *derived* (single ASCII literal argument, no
    concatenation), not a hand-kept allowlist that would rot.

الاستعمال / Usage:
    python tests/system/harness/test_no_narrow_spawn_in_link_path.py
    rc=0 نظيف · rc=1 انتهاك · rc=2 الحارسُ نفسُه معطوب
"""

import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[3]

# (AR) الملفّاتُ التي تبني أمرًا ثمّ تُطلِقه في مسارِ إنتاجِ التنفيذيّ/المكتبة.
# (EN) Files that build a command and spawn it on the executable/library path.
SCANNED_FILES = [
    "tools/compiler/compiler_driver_linker.cpp",
    "tools/compiler/compiler_driver_backend.cpp",
    "tools/compiler/compiler_driver_android_linker.cpp",
    "tools/compiler/compiler_driver_build_utils.cpp",
    "compiler/src/backend/llvm/llvm_linker.cpp",
    "compiler/src/backend/llvm/llvm_bare_metal_linker.cpp",
]

# (AR) واجهاتُ الإطلاقِ الضيّقة. `CreateProcessA` ضيّقةٌ دائمًا مهما كان وسيطُها،
#      لأنّ سطرَ الأمرِ عندها عازلُ `char` لا حرفيّة.
# (EN) Narrow spawn APIs. CreateProcessA is always narrow whatever the argument,
#      because its command line is a char buffer, never a literal.
NARROW_CALLS = [
    "std::system",
    "CreateProcessA",
    "_popen",
    "ShellExecuteA",
    "WinExec",
]
ALWAYS_NARROW = {"CreateProcessA", "ShellExecuteA", "WinExec"}

# (AR) أرضيّةُ عدٍّ تُحصى على **كلِّ** إطلاقٍ — ضيّقِه وعريضِه معًا. لو عُدَّ
#      الضيّقُ وحدَه لهبطت الأرضيّةُ كلّما أُصلح موضعٌ، فينقلب نجاحُ الإصلاحِ
#      إنذارًا كاذبًا. أمّا هبوطُ المجموعِ فيعني أنّ المِرساةَ انزاحت عن مسارِ
#      الربطِ نفسِه — لا أنّ الشجرةَ نظُفت. حارسٌ لا يرى شيئًا لا يُحمِّر أبدًا.
# (EN) The floor counts EVERY spawn, narrow and wide alike. Counting only the
#      narrow ones would lower the floor with each fix, turning a successful
#      repair into a false alarm. A drop in the total means the anchor slipped
#      off the link path itself. A guard that sees nothing can never fire.
SAFE_CALLS = [
    "sad::utf8::run_command",
    "sad::utf8::open_pipe_read",
    "CreateProcessW",
]
MIN_SPAWN_SITES = 12


def _extract_argument(text: str, open_paren: int) -> str:
    """(AR) نصُّ الوسائطِ حتّى القوسِ المُطابِق. (EN) Argument text to the match."""
    depth = 0
    in_string = False
    escaped = False
    for index in range(open_paren, len(text)):
        char = text[index]
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                in_string = False
            continue
        if char == '"':
            in_string = True
        elif char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return text[open_paren + 1:index]
    return text[open_paren + 1:]


_SINGLE_LITERAL = re.compile(r'^\s*"((?:[^"\\]|\\.)*)"\s*(?:,\s*"(?:[^"\\]|\\.)*"\s*)?$')


def _is_fixed_ascii_probe(argument: str) -> bool:
    """(AR) وسيطٌ حرفيٌّ واحدٌ ASCII بلا ضمٍّ ولا متغيّر — أمرُ استكشافٍ ثابت."""
    match = _SINGLE_LITERAL.match(argument)
    if not match:
        return False
    return all(ord(character) < 128 for character in argument)


def main() -> int:
    violations = []
    call_sites = 0
    safe_sites = 0
    missing = []

    for relative in SCANNED_FILES:
        path = REPO_ROOT / relative
        if not path.is_file():
            missing.append(relative)
            continue
        source = path.read_text(encoding="utf-8")
        lines = source.splitlines()
        for safe in SAFE_CALLS:
            safe_sites += source.count(safe + "(")
        for api in NARROW_CALLS:
            start = 0
            while True:
                found = source.find(api + "(", start)
                if found == -1:
                    break
                start = found + 1
                # (AR) تخطّي الذِّكرِ داخلَ تعليقٍ أو نصّ — نقيس النداءَ لا الكلمة.
                line_number = source.count("\n", 0, found) + 1
                line = lines[line_number - 1] if line_number <= len(lines) else ""
                stripped = line.lstrip()
                if stripped.startswith("//") or stripped.startswith("*"):
                    continue
                before = source[found - 1] if found else " "
                if before.isalnum() or before in "_:" and source[found - 2:found] != "::":
                    continue
                call_sites += 1
                argument = _extract_argument(source, found + len(api))
                if api in ALWAYS_NARROW:
                    violations.append((relative, line_number, api, line.strip()))
                elif not _is_fixed_ascii_probe(argument):
                    violations.append((relative, line_number, api, line.strip()))

    if missing:
        print("❌ الحارسُ يشير إلى ملفّاتٍ غيرِ موجودة / guard points at missing files:")
        for relative in missing:
            print(f"   - {relative}")
        print("   حدِّثْ SCANNED_FILES أو أعِدِ الملفَّ — لا تُسكِتِ الحارسَ بحذفِ صفّه.")
        return 2

    spawn_sites = call_sites + safe_sites
    if spawn_sites < MIN_SPAWN_SITES:
        print(f"❌ الماسحُ وجد {spawn_sites} إطلاقًا فقط (الأرضيّة {MIN_SPAWN_SITES}):"
              f" {call_sites} ضيّقًا و{safe_sites} آمنًا.")
        print("   المِرساةُ انزاحت عن مسارِ الربط — حارسٌ لا يرى شيئًا لا يُحمِّر.")
        return 2

    if violations:
        print("❌ نداءُ عمليّةٍ ضيّقُ الترميزِ في مسارِ إنتاجِ التنفيذيّ:")
        for relative, line_number, api, line in violations:
            print(f"   {relative}:{line_number}  [{api}]")
            print(f"      {line}")
        print()
        print("   الإصلاح: sad::utf8::run_command(...) للأوامر، وCreateProcessW")
        print("   مع sad::utf8::to_wstring(...) للإطلاقِ المباشر.")
        print("   Fix: sad::utf8::run_command(...) for commands, CreateProcessW")
        print("   with sad::utf8::to_wstring(...) for direct spawns.")
        return 1

    print(f"✅ لا نداءَ ضيّقًا في مسارِ الربط — {spawn_sites} إطلاقًا مفحوصًا "
          f"في {len(SCANNED_FILES)} ملفًّا.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
