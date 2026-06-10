"""
يُطبِّع أسماء الدوال/الطرق القانونية في ملفات YAML لتطابق مخرجات الـ lexer:
يحذف الحركات العربية (U+064B-U+065F: فتحة/ضمة/كسرة/شدة/سكون/تنوين)
يُبقي على التطويل (ـ U+0640) لأن الـ lexer لا يحذفه.

الاستخدام: python normalize_canonical.py
"""
import yaml
import pathlib
import sys

sys.stdout.reconfigure(encoding='utf-8')

# الحركات التي يحذفها الـ lexer: U+064B .. U+065F
HARAKAT = {chr(c) for c in range(0x064B, 0x0660)}


def strip_harakat(s):
    return ''.join(c for c in s if c not in HARAKAT)


def fix_file(path, name_keys):
    """name_keys: قائمة المفاتيح التي تحوي الاسم القانوني (canonical/method)"""
    data = yaml.safe_load(path.read_text(encoding='utf-8'))
    changed = 0
    # دوال/طرق
    items = data.get('methods') or data.get('functions') or []
    for it in items:
        for key in name_keys:
            if key in it:
                orig = it[key]
                norm = strip_harakat(orig)
                if norm != orig:
                    it[key] = norm
                    changed += 1
                    print(f"    {orig} -> {norm}")
    if changed:
        path.write_text(yaml.dump(data, allow_unicode=True, default_flow_style=False, sort_keys=False, width=120), encoding='utf-8')
    return changed


if __name__ == '__main__':
    root = pathlib.Path('language-truth')
    total = 0

    # type_methods.yaml
    tm = root / 'type_methods.yaml'
    if tm.exists():
        print(f"{tm}:")
        total += fix_file(tm, ['method'])

    # builtins/*.yaml
    for f in sorted((root / 'builtins').glob('*.yaml')):
        if f.name == '_index.yaml':
            continue
        print(f"{f}:")
        total += fix_file(f, ['canonical'])

    print(f"\nTotal normalized: {total}")
