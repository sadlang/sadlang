import subprocess, sys, os

os.chdir(r"c:\s_lang\s-programming-language")
os.environ["PYTHONIOENCODING"] = "utf-8"

result = subprocess.run(
    [sys.executable, "bee_os/boot/build_multimodule.py"],
    capture_output=True, text=True, encoding="utf-8"
)

# Write stdout to file
with open("build/qemu_boot/build_log3.txt", "w", encoding="utf-8") as f:
    f.write(result.stdout)
    f.write("\n---STDERR---\n")
    f.write(result.stderr)

# Print summary
lines = result.stdout.split("\n")
for i, line in enumerate(lines):
    if "FAIL" in line or "error" in line or "undefined" in line or "FATAL" in line:
        start = max(0, i-1)
        end = min(len(lines), i+20)
        for j in range(start, end):
            print(lines[j])
        break

print(f"\nExit code: {result.returncode}")
