"""
يستخرج جميع ثوابت string_view من builtin_registry.h ويُنشئ ملفات YAML
لكل namespace لم يُغطَّ بعد.

الاستخدام:
    python extract_builtins_from_header.py
"""
import re, yaml, pathlib, sys
sys.stdout.reconfigure(encoding='utf-8')

HEADER = pathlib.Path("shared/builtins/include/builtin_registry.h")
BUILTINS_DIR = pathlib.Path("language-truth/builtins")

# النطاقات المُغطَّاة بالفعل
COVERED = {
    "Core", "TypeCtor", "Concurrency",
    "Math", "Strings", "Arrays", "Basics", "Assertions", "Maps",
    "Sockets", "HttpClient", "HttpServer", "NetworkUtils", "WebSocketClient"
}

# قراءة الـ header
text = HEADER.read_text(encoding='utf-8', errors='replace')

# استخراج namespace blocks
ns_pattern = re.compile(
    r'namespace\s+(\w+)\s*\{([^}]+?)\}',
    re.DOTALL
)

# استخراج constexpr string_view
entry_pattern = re.compile(
    r'inline\s+constexpr\s+std::string_view\s+(\w+)\s*=\s*"([^"]+)"\s*;'
)

# جمع النطاقات
all_ns: dict[str, list[tuple[str, str]]] = {}

for ns_match in ns_pattern.finditer(text):
    ns_name = ns_match.group(1)
    ns_body = ns_match.group(2)

    if ns_name in ("Sad", "Builtins", "Names"):
        continue

    entries = entry_pattern.findall(ns_body)
    if entries:
        all_ns.setdefault(ns_name, []).extend(entries)

# طباعة النطاقات المكتشفة
print(f"Discovered namespaces: {len(all_ns)}")
for ns, entries in all_ns.items():
    covered = "✅" if ns in COVERED else "❌"
    print(f"  {covered} {ns}: {len(entries)} entries")

# توليد YAML للنطاقات غير المُغطَّاة
def make_fn(cpp_id: str, canonical: str, namespace: str) -> dict:
    return {
        "cpp_id": cpp_id,
        "canonical": canonical,
        "namespace": namespace,
        "category": "MODULE_FUNCTION",
        "require_import": True,
        "module": "NONE",  # سيتم تحديثه لاحقاً
        "compiler_strategy": "RUNTIME_CALL",
        "since": "0.1.0",
        "status": "stable",
        "description_ar": canonical,
        "description_en": canonical,
        "params": []
    }

# تجميع النطاقات المتشابهة
GROUPS = {
    "kernel": ["Kernel", "KernelTimers", "KernelThreads", "KernelUSB"],
    "sadnet": ["SadNet"],
    "platform": ["Platform"],
    "ui_widgets": ["UIWidgets", "UICore"],
    "ui_platform": ["UIAudio", "UICrypto", "UINetwork", "UIIO", "UIDialog",
                    "UITimer", "UIStorage", "UIPlatform", "UIDevice"],
}

# تحديد المجموعة لكل namespace
def get_group(ns: str) -> str | None:
    for group, namespaces in GROUPS.items():
        if ns in namespaces:
            return group
    return None

# كتابة ملفات YAML
new_files: dict[str, list] = {}
for ns, entries in all_ns.items():
    if ns in COVERED:
        continue
    group = get_group(ns)
    if group is None:
        group = ns.lower()

    if group not in new_files:
        new_files[group] = []
    for cpp_id, canonical in entries:
        new_files[group].append(make_fn(cpp_id, canonical, ns))

print(f"\nGenerating {len(new_files)} new YAML files:")
for group_name, fns in sorted(new_files.items()):
    yaml_path = BUILTINS_DIR / f"{group_name}.yaml"
    doc = {
        "version": 1,
        "namespace_group": group_name,
        "description_ar": f"وحدة {group_name} ({len(fns)} دالة)",
        "description_en": f"{group_name} module ({len(fns)} functions)",
        "functions": fns
    }
    yaml_path.write_text(
        yaml.dump(doc, allow_unicode=True, default_flow_style=False, sort_keys=False, width=120),
        encoding='utf-8'
    )
    print(f"  Written: {group_name}.yaml ({len(fns)} functions)")

print(f"\nDone. Update _index.yaml to include the new files.")
