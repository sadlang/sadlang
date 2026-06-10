"""
يحوّل ملفات المترجم compiler/src/.../builtins_*.cpp لاستخدام ثوابت Names::*.
يبني خريطة عامة (اسم عربي → ثابت كامل) من builtin_registry_generated.h،
ثم لكل شرط funcName/name == "..." [|| ...]* يختار الثابت ويستبدل الكتلة كاملة.

الأسماء غير الموجودة في أي نطاق تُطبع كتحذير (تحتاج إضافة لـ YAML).

الاستخدام: python convert_compiler_builtins.py <file> [<var_name>]
  var_name: اسم المتغير في المقارنة (افتراضي: funcName)
"""
import re
import pathlib
import sys

sys.stdout.reconfigure(encoding='utf-8')

HEADER = pathlib.Path("shared/builtins/generated/builtin_registry_generated.h")


def build_global_map():
    """خريطة: اسم عربي → ثابت كامل (Bn::Namespace::CONST).
    عند التعارض، أول ظهور يفوز (نُفضّل النطاقات الأساسية)."""
    text = HEADER.read_text(encoding='utf-8')
    name_to_const = {}
    # نطاق Names فقط (نتخطى TypeMethods لأنها للطرق لا الدوال)
    names_block = re.search(r'namespace Names\s*\{(.*)\n        \} // namespace Names', text, re.DOTALL)
    block = names_block.group(1)
    # كل sub-namespace مباشر داخل Names (وداخله TypeMethods متشعّب — نتخطاه)
    for ns_match in re.finditer(r'namespace (\w+)\s*\{', block):
        ns = ns_match.group(1)
        if ns == 'TypeMethods':
            continue
        # محتوى هذا الـ namespace حتى الإغلاق المقابل
        start = ns_match.end()
        depth = 1
        i = start
        while i < len(block) and depth > 0:
            if block[i] == '{':
                depth += 1
            elif block[i] == '}':
                depth -= 1
            i += 1
        body = block[start:i]
        for cid, val in re.findall(r'(\w+)\s*=\s*"([^"]+)"', body):
            if cid.startswith('METHOD'):
                continue
            if val not in name_to_const:
                name_to_const[val] = f"Bn::{ns}::{cid}"
    return name_to_const


def convert(filepath, var='funcName'):
    name_map = build_global_map()
    f = pathlib.Path(filepath)
    text = f.read_text(encoding='utf-8')

    # نمط شرط متعدد الأسطر
    cond_pat = re.compile(
        re.escape(var) + r' == (?:"[^"]+"|Bn::\w+::\w+)'
        r'(?:\s*\|\|\s*\n?\s*' + re.escape(var) + r' == (?:"[^"]+"|Bn::\w+::\w+))*'
    )

    unmapped = set()

    def repl(match):
        cond = match.group(0)
        literals = re.findall(r'"([^"]+)"', cond)
        if not literals:
            return cond  # already constant
        for lit in literals:
            if lit in name_map:
                return f'{var} == {name_map[lit]}'
        # لا ثابت لأي اسم
        for lit in literals:
            if any(ord(c) > 127 for c in lit):
                unmapped.add(lit)
        return cond

    new_text, count = cond_pat.subn(repl, text)
    f.write_text(new_text, encoding='utf-8')
    print(f"  {filepath}: {count} conditions, unmapped: {sorted(unmapped)}")
    return unmapped


if __name__ == '__main__':
    filepath = sys.argv[1]
    var = sys.argv[2] if len(sys.argv) > 2 else 'funcName'
    convert(filepath, var)
