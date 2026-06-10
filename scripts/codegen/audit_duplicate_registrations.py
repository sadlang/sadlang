"""
يدقّق تكرار تسجيل الدوال المدمجة عبر ملفات المفسر:
يجد الأسماء القانونية (canonical) المُسجَّلة عبر registerBuiltinFunction في أكثر من ملف .cpp.

يربط الثوابت (Namespace::CPP_ID) بقيمتها العربية من الـ header المُولَّد،
ثم يجمع: canonical → {الملفات التي تسجلها}.
"""
import re, pathlib, sys, collections
sys.stdout.reconfigure(encoding='utf-8')

HEADER = pathlib.Path("shared/builtins/generated/builtin_registry_generated.h")


def build_const_map():
    """خريطة: 'Namespace::CPP_ID' → canonical (القيمة العربية)"""
    text = HEADER.read_text(encoding='utf-8')
    const_map = {}
    # نمر على كل namespace وكل ثابت
    # نتتبع المسار الكامل للـ namespace
    stack = []
    for line in text.split('\n'):
        nsm = re.search(r'namespace (\w+)', line)
        if nsm:
            stack.append(nsm.group(1))
            continue
        if '}' in line and stack:
            # تقريبي: كل } يُنهي namespace (يكفي للبنية المسطحة هنا)
            if '{' not in line:
                stack.pop()
                continue
        cm = re.search(r'(\w+)\s*=\s*"([^"]+)"', line)
        if cm and stack:
            cid, val = cm.group(1), cm.group(2)
            # المسار: آخر namespace ذي معنى (Core/Math/Kernel...) — نتخطى Sad/Builtins/Names
            meaningful = [s for s in stack if s not in ('Sad', 'Builtins', 'Names')]
            if meaningful:
                ns = meaningful[-1]
                const_map[f'{ns}::{cid}'] = val
    return const_map


def main():
    const_map = build_const_map()

    # alias → namespace في كل ملف
    reg_pat = re.compile(r'registerBuiltinFunction\(std::string\((\w+)::(\w+)\)')

    # canonical → {file → count}
    canon_files = collections.defaultdict(lambda: collections.defaultdict(int))

    for f in sorted(pathlib.Path('interpreter/src/builtins').glob('*.cpp')):
        text = f.read_text(encoding='utf-8', errors='replace')
        # alias map: 'Bm' → 'Math'
        aliases = {}
        for am in re.finditer(r'namespace (\w+)\s*=\s*(?:Sad::)?Builtins::Names::(\w+)', text):
            aliases[am.group(1)] = am.group(2)
        for rm in reg_pat.finditer(text):
            alias, cid = rm.group(1), rm.group(2)
            ns = aliases.get(alias, alias)
            canon = const_map.get(f'{ns}::{cid}')
            if canon:
                canon_files[canon][f.name] += 1

    # الأسماء المُسجَّلة في أكثر من ملف
    dups = {c: dict(fs) for c, fs in canon_files.items() if len(fs) > 1}
    print(f"=== تكرارات تسجيل عبر ملفات متعددة: {len(dups)} ===\n")
    for canon, files in sorted(dups.items()):
        print(f"  '{canon}':")
        for fn, cnt in files.items():
            print(f"      {fn} ({cnt})")
    print(f"\n=== الملخص ===")
    print(f"إجمالي الأسماء المُسجَّلة: {len(canon_files)}")
    print(f"المُكرَّرة عبر ملفات: {len(dups)}")


if __name__ == '__main__':
    main()
