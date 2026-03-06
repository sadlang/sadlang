import re

for fname in ["ملفات_متقدمة_نحلة", "ملفات_أمان_نحلة", "ملفات_أنظمة_نحلة", "ملفات_ذاكرة_نحلة"]:
    ll_path = f"build/qemu_boot/{fname}_elf.ll"
    with open(ll_path, "r", encoding="utf-8") as f:
        text = f.read()
    declares = re.findall(r'declare[^@]*@"?([^\s"(]+)"?', text)
    print(f"\n=== {fname} declares ===")
    for d in sorted(set(declares)):
        print(f"  {d}")
