"""
يُزيل الأسماء البديلة (string aliases) من شروط المقارنة، مُبقياً الثابت فقط.
يحوّل: if (m == TM::Array::PUSH || m == "أضف" || m == "ادفع")
إلى:   if (m == TM::Array::PUSH)

الاستخدام: python remove_aliases.py <file1> <file2> ...
"""
import re
import pathlib
import sys

sys.stdout.reconfigure(encoding='utf-8')

# نمط: ثابت (TM::/Bn::/Bs:: إلخ) متبوع بـ || VAR == "نص" (واحد أو أكثر)
# نلتقط الثابت ونحذف البدائل النصية
# VAR == NAMESPACE::...::CONST ( || VAR == "..." )+
ALIAS_PATTERN = re.compile(
    r'(\w+\s*==\s*(?:TM|Bn|Bs|Bm|Bb|Bss|Bmp|Bsk|Bhc|Bhs|Bnu|Basync|Bexc|Bffi|Bt)::[A-Za-z0-9_]+(?:::[A-Z0-9_]+)?)'
    r'((?:\s*\|\|\s*\w+\s*==\s*"[^"]*")+)'
)

def collapse(match):
    # نُبقي الثابت فقط، نحذف كل البدائل النصية
    return match.group(1)

def process(filepath):
    f = pathlib.Path(filepath)
    text = f.read_text(encoding='utf-8')
    new_text, count = ALIAS_PATTERN.subn(collapse, text)
    if count > 0:
        f.write_text(new_text, encoding='utf-8')
    print(f"  {filepath}: {count} conditions collapsed")
    return count

if __name__ == '__main__':
    total = 0
    for fp in sys.argv[1:]:
        total += process(fp)
    print(f"\nTotal: {total}")
