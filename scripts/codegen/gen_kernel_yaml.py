"""
يُولِّد ملفات YAML لدوال النواة من الأسماء المستخرجة، بـ cpp_id تسلسلي ثابت.
كل subsystem له namespace خاص (KernelCpu, KernelAudio, ...).
"""
import json, yaml, pathlib, sys, tempfile, os
sys.stdout.reconfigure(encoding='utf-8')

names_file = os.path.join(tempfile.gettempdir(), 'kernel_names.json')
all_names = json.loads(pathlib.Path(names_file).read_text(encoding='utf-8'))

# subsystem → (namespace C++, prefix cpp_id, module)
SUBSYS = {
    'cpu':     ('KernelCpu',     'CPU',     'NONE'),
    'audio':   ('KernelAudio',   'AUDIO',   'NONE'),
    'gpu':     ('KernelGpu',     'GPU',     'NONE'),
    'memory':  ('KernelMemory',  'MEM',     'NONE'),
    'network': ('KernelNet',     'NET',     'NONE'),
    'serial':  ('KernelSerial',  'SERIAL',  'NONE'),
    'storage': ('KernelStorage', 'STORAGE', 'NONE'),
}

index_additions = []
ns_additions = []

for subsys, names in all_names.items():
    if subsys not in SUBSYS or not names:
        continue
    ns, prefix, module = SUBSYS[subsys]
    ns_additions.append(ns)

    functions = []
    seen = set()
    counter = 0
    for ar in names:
        cpp_id = f"{prefix}_{counter}"
        counter += 1
        functions.append({
            "cpp_id": cpp_id,
            "canonical": ar,
            "namespace": ns,
            "category": "MODULE_FUNCTION",
            "require_import": True,
            "module": module,
            "compiler_strategy": "RUNTIME_CALL",
            "since": "0.1.0",
            "status": "stable",
            "description_ar": ar,
            "description_en": f"kernel {subsys} function",
            "params": [],
            "lookup_table": "name_only"
        })

    doc = {
        "version": 1,
        "namespace_group": ns,
        "description_ar": f"وحدة نواة {subsys} ({len(functions)} دالة)",
        "description_en": f"Kernel {subsys} module ({len(functions)} functions)",
        "functions": functions
    }
    fname = f"kernel_{subsys}.yaml"
    out = pathlib.Path(f"language-truth/builtins/{fname}")
    out.write_text(yaml.dump(doc, allow_unicode=True, default_flow_style=False, sort_keys=False, width=120), encoding='utf-8')
    index_additions.append(fname)
    print(f"Written: {fname} ({len(functions)} functions, ns={ns})")

print(f"\nNamespaces to add to schema: {ns_additions}")
print(f"Files to add to index: {index_additions}")

# تحديث index
idx = pathlib.Path("language-truth/builtins/_index.yaml")
idata = yaml.safe_load(idx.read_text(encoding='utf-8'))
for fn in index_additions:
    if fn not in idata['files']:
        idata['files'].append(fn)
idx.write_text(yaml.dump(idata, allow_unicode=True, default_flow_style=False, sort_keys=False), encoding='utf-8')

# تحديث schema
sp = pathlib.Path('language-truth/_schemas/builtin_function.schema.json')
schema = json.loads(sp.read_text(encoding='utf-8'))
ns_enum = schema['properties']['namespace']['enum']
for ns in ns_additions:
    if ns not in ns_enum:
        ns_enum.append(ns)
sp.write_text(json.dumps(schema, ensure_ascii=False, indent=2), encoding='utf-8')
print("Updated index + schema")
