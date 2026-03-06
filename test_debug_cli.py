import subprocess
import json
import sys
import time

sad_exe = r"C:\s_lang\s-programming-language\build\bin\Debug\sad.exe"
test_file = r"C:\s_lang\s-programming-language\test_oop_debug.ص"

print("Starting debug server...")
p = subprocess.Popen([sad_exe, "--debug-server", test_file], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

requests = [
    {"type": "request", "seq": 1, "command": "initialize", "arguments": {}},
    {"type": "request", "seq": 2, "command": "launch", "arguments": {"program": test_file, "stopOnEntry": True}},
    {"type": "request", "seq": 3, "command": "setBreakpoints", "arguments": {"source": {"path": test_file}, "breakpoints": [{"line": 19}]}},
    {"type": "request", "seq": 4, "command": "configurationDone", "arguments": {}},
]

for req in requests:
    cmd = json.dumps(req)
    print(f"-> {cmd}")
    p.stdin.write(cmd + "\n")
    p.stdin.flush()
    time.sleep(0.2)

# Read lines
print("Reading responses...")
for _ in range(10):
    line = p.stdout.readline()
    if line:
        print(f"<- {line.strip()}")
    else:
        break

p.terminate()
print("Test completed.")
