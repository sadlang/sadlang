"""
يستخرج أسماء الدوال العربية من ملفات C++ التي تستخدم hex-encoding
"""
import pathlib, sys, re

sys.stdout.reconfigure(encoding='utf-8')

BACKSLASH = '\\'

def decode_cpp_str(s: str) -> str:
    """فك تشفير نص C++ يحتوي hex escapes مثل \\xNN"""
    result = b''
    i = 0
    s_bytes = s.encode('latin-1', errors='replace')
    n = len(s_bytes)
    while i < n:
        if s_bytes[i] == ord(BACKSLASH) and i + 3 < n and s_bytes[i+1] in (ord('x'), ord('X')):
            hex_part = s_bytes[i+2:i+4].decode('ascii', errors='replace')
            try:
                result += bytes.fromhex(hex_part)
            except ValueError:
                result += s_bytes[i:i+4]
            i += 4
        elif s_bytes[i] == ord(BACKSLASH) and i + 1 < n:
            # escape sequences أخرى
            result += bytes([s_bytes[i+1]])
            i += 2
        else:
            result += bytes([s_bytes[i]])
            i += 1
    return result.decode('utf-8', errors='replace')


def extract_arabic_names(filepath: str) -> list:
    text = pathlib.Path(filepath).read_text(encoding='utf-8', errors='replace')
    PAT = re.compile(r'registerBuiltinFunction\("([^"]+)"')
    seen = set()
    names = []
    for match in PAT.finditer(text):
        raw = match.group(1)
        decoded = decode_cpp_str(raw)
        if decoded not in seen and any(ord(c) > 127 for c in decoded):
            seen.add(decoded)
            names.append(decoded)
    return names


if __name__ == '__main__':
    import sys
    files = sys.argv[1:] if len(sys.argv) > 1 else [
        'interpreter/src/builtins/builtin_module_async.cpp',
    ]
    for f in files:
        names = extract_arabic_names(f)
        print(f"\n=== {f} ({len(names)}) ===")
        for n in names:
            print(f"  '{n}'")
