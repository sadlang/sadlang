"""
يستبدل شروط مقارنة الطرق التي تستخدم نصوصاً عربية بثابت واحد.
يحوّل: if (m == "أدخل" || m == "ادخل")  →  if (m == TM::Array::INSERT)

يبني الخريطة من builtin_registry_generated.h (نطاق TypeMethods)،
ثم لكل كتلة شرط، يأخذ أول اسم له ثابت ويستبدل الكتلة كاملة.

الاستخدام: python collapse_method_conditions.py <namespace_sub> <file>
  namespace_sub: Array | String | Map | Channel | Any
"""
import re
import pathlib
import sys

sys.stdout.reconfigure(encoding='utf-8')

HEADER = pathlib.Path("shared/builtins/generated/builtin_registry_generated.h")


def build_map(sub):
    """خريطة: الاسم العربي → اسم الثابت لنطاق فرعي محدد"""
    text = HEADER.read_text(encoding='utf-8')
    m = re.search(r'namespace TypeMethods\s*\{(.*?)\n            \}', text, re.DOTALL)
    block = m.group(1)
    sub_m = re.search(r'namespace ' + sub + r'\s*\{(.*?)\n                \}', block, re.DOTALL)
    if not sub_m:
        return {}
    name_to_const = {}
    for cid, val in re.findall(r'(\w+)\s*=\s*"([^"]+)"', sub_m.group(1)):
        # نتجنب الثوابت العامة METHOD/METHOD_n (مكررة بلا معنى)
        name_to_const[val] = f"TM::{sub}::{cid}"
    return name_to_const


# نمط كتلة شرط: if (m == "..." [|| m == "..."]* )
COND_PATTERN = re.compile(r'if \(m == (?:"[^"]+"|TM::\w+::\w+)(?:\s*\|\|\s*m == (?:"[^"]+"|TM::\w+::\w+))*\)')


def process(sub, filepath):
    name_to_const = build_map(sub)
    f = pathlib.Path(filepath)
    text = f.read_text(encoding='utf-8')

    def replace_cond(match):
        cond = match.group(0)
        # استخرج كل الأسماء النصية في الشرط
        literals = re.findall(r'm == "([^"]+)"', cond)
        # ابحث عن أول اسم له ثابت
        for lit in literals:
            if lit in name_to_const:
                return f'if (m == {name_to_const[lit]})'
        # إن لم يوجد ثابت، أبقِ الكتلة كما هي
        return cond

    new_text, count = COND_PATTERN.subn(replace_cond, text)
    f.write_text(new_text, encoding='utf-8')
    print(f"  {filepath} [{sub}]: {count} conditions processed")


if __name__ == '__main__':
    sub = sys.argv[1]
    fp = sys.argv[2]
    process(sub, fp)
