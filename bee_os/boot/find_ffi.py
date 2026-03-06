import re, os

files = [
    "ملفات_متقدمة_نحلة",
    "ملفات_أمان_نحلة",
    "ملفات_أنظمة_نحلة",
    "ملفات_ذاكرة_نحلة",
]

for fname in files:
    path = os.path.join("bee_os", "kernel", fname + ".ص")
    with open(path, "r", encoding="utf-8") as f:
        text = f.read()
    pattern = re.compile(r'خارجي\("([^"]+)"\)')
    matches = pattern.findall(text)
    print(f"\n=== {fname} ({len(matches)} FFI) ===")
    for m in matches:
        print(f"  {m}")
