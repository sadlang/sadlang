"""
يحوّل ملفات builtin_kernel_*.cpp لاستخدام الثوابت المُولَّدة.
يقرأ الخريطة من kernel_*.yaml، يضيف include + alias، ويستبدل النصوص (عربي + hex).
"""
import yaml, pathlib, sys
sys.stdout.reconfigure(encoding='utf-8')

BACKSL = chr(92)

SUBSYS = {
    'cpu':     ('KernelCpu',     'Kcpu'),
    'audio':   ('KernelAudio',   'Kaudio'),
    'gpu':     ('KernelGpu',     'Kgpu'),
    'memory':  ('KernelMemory',  'Kmem'),
    'network': ('KernelNet',     'Knet'),
    'serial':  ('KernelSerial',  'Kserial'),
    'storage': ('KernelStorage', 'Kstorage'),
}


def hex_escaped(s):
    parts = []
    for ch in s:
        if ord(ch) < 128:
            parts.append(ch)
        else:
            for b in ch.encode('utf-8'):
                parts.append(BACKSL + 'x' + f'{b:02x}')
    return ''.join(parts)


def convert(subsys, ns, alias):
    yaml_path = pathlib.Path(f"language-truth/builtins/kernel_{subsys}.yaml")
    data = yaml.safe_load(yaml_path.read_text(encoding='utf-8'))
    # خريطة canonical → cpp_id
    name_to_id = {fn['canonical']: fn['cpp_id'] for fn in data['functions']}

    cpp_path = pathlib.Path(f"interpreter/src/builtins/builtin_kernel_{subsys}.cpp")
    text = cpp_path.read_text(encoding='utf-8')

    # 1) إضافة include + alias إن لم يوجدا
    if f"namespace {alias} =" not in text:
        # نضيف بعد أول #include
        marker = '#include "builtin_registry.h"'
        alias_line = f'\nnamespace {alias} = Sad::Builtins::Names::{ns};'
        if marker in text:
            text = text.replace(marker, marker + alias_line, 1)
        else:
            # أضف بعد أول سطر include
            lines = text.split('\n')
            for i, ln in enumerate(lines):
                if ln.startswith('#include'):
                    lines.insert(i+1, f'#include "builtin_registry.h"{alias_line}')
                    break
            text = '\n'.join(lines)

    # 2) استبدال التسجيلات (عربي مباشر + hex)
    count = 0
    for canonical, cpp_id in name_to_id.items():
        const = f'std::string({alias}::{cpp_id})'
        for literal in [canonical, hex_escaped(canonical)]:
            old = f'registerBuiltinFunction("{literal}",'
            new = f'registerBuiltinFunction({const},'
            n = text.count(old)
            if n > 0:
                text = text.replace(old, new)
                count += n

    cpp_path.write_text(text, encoding='utf-8')
    print(f"  {subsys}: {count} replacements")
    return count


if __name__ == '__main__':
    total = 0
    for subsys, (ns, alias) in SUBSYS.items():
        total += convert(subsys, ns, alias)
    print(f"\nTotal: {total}")
