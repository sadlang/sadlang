"""يستخرج أسماء دوال النواة العربية (نصوص مباشرة) من ملفات builtin_kernel_*.cpp"""
import re, pathlib, sys, json
sys.stdout.reconfigure(encoding='utf-8')

# نمط: registerBuiltinFunction("...") — نلتقط أي محتوى ثم نُصفّي
PAT = re.compile(r'registerBuiltinFunction\("([^"]+)"')
BACKSL = chr(92)

def decode_hex(s):
    """فك hex escapes مع الإبقاء على ASCII الحرفي"""
    result = b''
    i = 0
    while i < len(s):
        if s[i] == BACKSL and i + 3 < len(s) and s[i+1] in ('x', 'X'):
            result += bytes.fromhex(s[i+2:i+4])
            i += 4
        else:
            result += s[i].encode('utf-8')
            i += 1
    return result.decode('utf-8', errors='replace')

def get_arabic(s):
    """يُرجع الاسم العربي المفكوك إن كان عربياً، وإلا None"""
    decoded = decode_hex(s)
    if any(ord(c) > 127 for c in decoded):
        return decoded
    return None

kernel_files = [
    'audio', 'cpu', 'gpu', 'memory', 'network', 'serial',
    'storage', 'usb', 'acpi', 'uefi', 'timers', 'threads'
]

all_names = {}
total = 0
for name in kernel_files:
    fp = pathlib.Path(f'interpreter/src/builtins/builtin_kernel_{name}.cpp')
    if not fp.exists():
        continue
    text = fp.read_text(encoding='utf-8')
    names = []
    for raw in PAT.findall(text):
        ar = get_arabic(raw)
        if ar:
            names.append(ar)
    all_names[name] = names
    total += len(names)
    print(f"{name}: {len(names)}")

print(f"\nTotal: {total}")

# حفظ القائمة
import tempfile, os
out = os.path.join(tempfile.gettempdir(), 'kernel_names.json')
pathlib.Path(out).write_text(
    json.dumps(all_names, ensure_ascii=False, indent=2), encoding='utf-8')
print(f"Saved: {out}")
