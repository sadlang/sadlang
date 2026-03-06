import subprocess, sys, os

os.chdir(r"c:\s_lang\s-programming-language")
os.environ["PYTHONIOENCODING"] = "utf-8"

result = subprocess.run(
    [sys.executable, r"bee_os\boot\build_multimodule.py"],
    capture_output=True
)

out = (result.stdout + result.stderr).decode("utf-8", "replace")

with open(r"build\qemu_boot\build_errors.txt", "w", encoding="utf-8") as f:
    f.write(out)

# Count errors
errors = [l for l in out.split("\n") if "error" in l.lower() or "undefined" in l.lower()]
print(f"Exit: {result.returncode}, Errors: {len(errors)}")
for e in errors:
    print(e)
