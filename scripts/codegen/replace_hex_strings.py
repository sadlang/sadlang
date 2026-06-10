"""
أداة عامة لاستبدال النصوص العربية المشفرة hex في ملف C++ بثوابت مُولَّدة.

الاستخدام:
    python replace_hex_strings.py <cpp_file> <namespace_alias> <map_json>

حيث map_json ملف JSON يربط الاسم العربي بـ cpp_id:
    {"استثناء_جديد": "EXC_NEW", ...}
"""
import pathlib, sys, json
sys.stdout.reconfigure(encoding='utf-8')


def to_hex_escaped(s: str) -> str:
    # (AR) الأحرف ASCII تبقى حرفية، غير ASCII تُشفَّر hex
    parts = []
    for ch in s:
        if ord(ch) < 128:
            parts.append(ch)
        else:
            for b in ch.encode('utf-8'):
                parts.append('\\' + 'x' + f'{b:02x}')
    return ''.join(parts)


def convert(cpp_file: str, ns_alias: str, name_map: dict) -> int:
    f = pathlib.Path(cpp_file)
    text = f.read_text(encoding='utf-8')
    count = 0
    for arabic, cpp_id in name_map.items():
        const = f'std::string({ns_alias}::{cpp_id})'
        hex_str = to_hex_escaped(arabic)
        # نجرب صيغتين: hex-encoded و نص عربي مباشر
        for literal in [hex_str, arabic]:
            old = 'registerBuiltinFunction("' + literal + '",'
            new = f'registerBuiltinFunction({const},'
            n = text.count(old)
            if n > 0:
                text = text.replace(old, new)
                count += n
                print(f"  '{arabic}': {n}")
    f.write_text(text, encoding='utf-8')
    return count


if __name__ == '__main__':
    cpp_file = sys.argv[1]
    ns_alias = sys.argv[2]
    map_file = sys.argv[3]
    name_map = json.loads(pathlib.Path(map_file).read_text(encoding='utf-8'))
    total = convert(cpp_file, ns_alias, name_map)
    print(f"\nTotal: {total}")
