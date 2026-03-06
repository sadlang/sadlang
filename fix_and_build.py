import subprocess, sys, os

os.chdir(r"c:\s_lang\s-programming-language")
SADC = r"build\bin\Release\sadc.exe"

# Recompile the two fixed files
for mod in ["ملفات_أنظمة_نحلة", "ملفات_متقدمة_نحلة"]:
    src = os.path.join("bee_os", "kernel", mod + ".ص")
    out = os.path.join("bee_os", "kernel", mod + ".ll")
    cmd = [SADC, "--freestanding", "--no-main", "--emit-llvm", src, "-o", out]
    print(f"Compiling {mod}...")
    r = subprocess.run(cmd, capture_output=True)
    stderr = r.stderr.decode("utf-8", "replace")
    if os.path.exists(out) and os.path.getsize(out) > 0:
        print(f"  OK ({os.path.getsize(out)} bytes)")
    else:
        print(f"  FAIL!")
        print(stderr[-500:])

# Now run full build
print("\n=== Running full build ===")
env = os.environ.copy()
env["PYTHONIOENCODING"] = "utf-8"
r = subprocess.run(
    [sys.executable, r"bee_os\boot\build_multimodule.py"],
    capture_output=True, env=env
)
out = (r.stdout + r.stderr).decode("utf-8", "replace")

with open(r"build\qemu_boot\build_log_final.txt", "w", encoding="utf-8") as f:
    f.write(out)

if r.returncode == 0:
    print("BUILD SUCCESS!")
    # Print last 20 lines
    for line in out.split("\n")[-20:]:
        print(line)
else:
    print(f"BUILD FAILED (exit={r.returncode})")
    # Print error lines
    for line in out.split("\n"):
        if "error" in line.lower() or "undefined" in line.lower() or "FATAL" in line or "FAIL" in line:
            print(line)
    # Print the full error context
    lines = out.split("\n")
    for i, line in enumerate(lines):
        if "FAIL" in line:
            start = max(0, i-1)
            end = min(len(lines), i+25)
            print("\n--- Error Context ---")
            for j in range(start, end):
                print(lines[j])
            break
