from pathlib import Path

p = Path("compiler/include/frontend/sir_builder.h")
text = p.read_text(encoding="utf-8")
lines = text.split("\n")

# Step 2
inc_line = '#include "sir_builder_context.h"'
if inc_line not in text:
    for i, ln in enumerate(lines):
        if ln.strip() == '#include "sir_constants.h"':
            lines.insert(i + 1, inc_line)
            break

# Step 3
target_old = "            class SIRBuilder"
target_new = "            class SIRBuilder : public SIRBuilderContext"
for i, ln in enumerate(lines):
    if ln == target_old:
        lines[i] = target_new
        break

# Step 4
marker = "const std::vector<std::string> &getErrors() const { return errors_; }"
anchor = -1
for i, ln in enumerate(lines):
    if marker in ln:
        anchor = i
        break
assert anchor >= 0, "anchor not found"

priv_idx = -1
for i in range(anchor + 1, len(lines)):
    if "private:" in lines[i]:
        priv_idx = i
        break
assert priv_idx >= 0, "private not found"

end_line = "                std::unordered_map<std::string, ModuleSIRArtifacts> moduleCache_;"
end_idx = -1
for i in range(priv_idx, len(lines)):
    if lines[i] == end_line:
        end_idx = i
        break
assert end_idx >= 0, "end line not found"

replacement = '''            private:
                // ==================================================================
                // الحالة الداخلية / Internal State
                // ==================================================================
                //
                // (AR) Phase 6 — Step 1: جميع حقول الحالة والـstructs الداخلية
                //      (FinallyContext, SIRBuilderContextState, NamespaceMemberInfo,
                //      CaptureInfo, ModuleSIRArtifacts) ودوال saveContext/restoreContext
                //      نُقلت إلى struct SIRBuilderContext في sir_builder_context.h.
                //      SIRBuilder الآن يرث منها (`class SIRBuilder : public SIRBuilderContext`)
                //      فيبقى الوصول المباشر للحقول من جميع ملفات sir_builder_*.cpp يعمل
                //      دون أي تغيير.
                // (EN) Phase 6 — Step 1: all state fields and inner structs were moved
                //      to struct SIRBuilderContext in sir_builder_context.h. SIRBuilder
                //      inherits from it; direct field access keeps working unchanged.
                // ==================================================================
'''.split("\n")

new_lines = lines[:priv_idx] + replacement + lines[end_idx + 1:]
new_text = "\n".join(new_lines)
p.write_text(new_text, encoding="utf-8")

size = p.stat().st_size
final_lines = new_text.split("\n")
class_ln = next((i+1 for i,l in enumerate(final_lines) if l.startswith("            class SIRBuilder : public SIRBuilderContext")), -1)
private_ln = next((i+1 for i,l in enumerate(final_lines) if i+1 > class_ln and l.strip() == "private:"), -1)
print(f"size={size}")
print(f"class_line={class_ln}")
print(f"private_line={private_ln}")
