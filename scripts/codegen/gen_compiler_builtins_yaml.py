"""
يستخرج أسماء الدوال العربية المتبقية (غير المُحوَّلة) من ملفات المترجم،
ويُنشئ ملفات YAML لها بنطاقات مخصصة + cpp_id تسلسلي ثابت.
ثم يُحدِّث index + schema.
"""
import re, yaml, json, pathlib, sys
sys.stdout.reconfigure(encoding='utf-8')

# ملف المترجم → (namespace C++, prefix cpp_id, var)
FILES = {
    "builtins_io_utils":    ("CompilerIo",     "IO"),
    "builtins_os_core":     ("CompilerCpuCtl", "CPUCTL"),
    "builtins_os_hardware": ("CompilerHw",     "HW"),
    "builtins_os_system":   ("CompilerSys",    "SYS"),
    "builtins_security":    ("CompilerSec",    "SEC"),
    "builtins_simd":        ("CompilerSimd",   "SIMD"),
    "builtins_system":      ("CompilerMem",    "MEM"),
    "builtins_embedded":    ("CompilerEmbed",  "EMBED"),
}

COND = re.compile(r'funcName == "([^"]+)"')

def extract_unmapped(filepath):
    """يستخرج الأسماء العربية المتبقية كنصوص خام"""
    text = pathlib.Path(filepath).read_text(encoding='utf-8')
    names = []
    seen = set()
    for lit in COND.findall(text):
        if any(ord(c) > 127 for c in lit) and lit not in seen:
            seen.add(lit)
            names.append(lit)
    return names

index_add = []
schema_add = []

for name, (ns, prefix) in FILES.items():
    fp = f"compiler/src/frontend/builders/{name}.cpp"
    names = extract_unmapped(fp)
    if not names:
        continue
    functions = []
    for i, ar in enumerate(names):
        functions.append({
            "cpp_id": f"{prefix}_{i}",
            "canonical": ar,
            "namespace": ns,
            "category": "MODULE_FUNCTION",
            "require_import": True,
            "module": "NONE",
            "compiler_strategy": "INLINE",
            "since": "0.1.0",
            "status": "stable",
            "description_ar": ar,
            "description_en": f"compiler builtin ({name})",
            "params": [],
            "lookup_table": "name_only"
        })
    doc = {
        "version": 1,
        "namespace_group": ns,
        "description_ar": f"دوال مترجم {ns} ({len(functions)} دالة)",
        "description_en": f"Compiler builtins {ns} ({len(functions)})",
        "functions": functions
    }
    out_name = f"compiler_{name.replace('builtins_','')}.yaml"
    pathlib.Path(f"language-truth/builtins/{out_name}").write_text(
        yaml.dump(doc, allow_unicode=True, default_flow_style=False, sort_keys=False, width=120),
        encoding='utf-8')
    index_add.append(out_name)
    schema_add.append(ns)
    print(f"Written: {out_name} ({len(functions)} fns, ns={ns})")

# index
idx = pathlib.Path("language-truth/builtins/_index.yaml")
idata = yaml.safe_load(idx.read_text(encoding='utf-8'))
for fn in index_add:
    if fn not in idata['files']:
        idata['files'].append(fn)
idx.write_text(yaml.dump(idata, allow_unicode=True, default_flow_style=False, sort_keys=False), encoding='utf-8')

# schema
sp = pathlib.Path('language-truth/_schemas/builtin_function.schema.json')
schema = json.loads(sp.read_text(encoding='utf-8'))
# (AR) 🔑 الإلحاقُ الصامتُ ممنوع: كان هذا السطرُ يُوسِّعُ `enum` المجموعاتِ
#      بلا قرار، فصارت القائمةُ **للكتابةِ فقط** وخرج عنها ٣٥٥ إعلانًا.
#      واليومَ يحرسُها `check_builtins_schema.py` في الاتّجاهين، فإضافةُ
#      مجموعةٍ قرارٌ يُكتَبُ في المخطَّطِ بيدٍ ويُراجَعُ في الـPR.
missing = [ns for ns in schema_add
           if ns not in schema['properties']['namespace']['enum']]
if missing:
    raise SystemExit(
        "✗ مجموعاتٌ ليست في مخطَّطِ المدمَجات: " + ", ".join(sorted(set(missing)))
        + "\n  أضِفها عمدًا إلى language-truth/_schemas/builtin_function.schema.json"
        + "\n  ثمّ أعِد التشغيل. لا تُوسَّعُ القائمةُ آليًّا.")
sp.write_text(json.dumps(schema, ensure_ascii=False, indent=2), encoding='utf-8')
print("Updated index + schema")
