import subprocess
import json
import threading

sad_exe = r"C:\s_lang\s-programming-language\build\bin\Debug\sad.exe"
test_file = r"C:\s_lang\s-programming-language\test_oop_debug.ص"

p = subprocess.Popen([sad_exe, "--debug-server", test_file], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, encoding='utf-8')

def read_out():
    while True:
        line = p.stdout.readline()
        if not line: break
        print(f"STDOUT: {line.strip()}")

def read_err():
    while True:
        line = p.stderr.readline()
        if not line: break
        print(f"STDERR: {line.strip()}")

threading.Thread(target=read_out, daemon=True).start()
threading.Thread(target=read_err, daemon=True).start()

reqs = [
    {"type": "request", "seq": 1, "command": "initialize", "arguments": {}},
    {"type": "request", "seq": 2, "command": "launch", "arguments": {"program": test_file, "stopOnEntry": True}},
    {"type": "request", "seq": 3, "command": "configurationDone", "arguments": {}},
]

import time
for r in reqs:
    cmd = json.dumps(r)
    print(f"SEND: {cmd}")
    try:
        p.stdin.write(cmd + "\n")
        p.stdin.flush()
    except Exception as e:
        print(f"Error sending: {e}")
        break
    time.sleep(0.5)

time.sleep(1)
print("Sending continue...")
try:
    p.stdin.write('{"type":"request", "seq":4, "command":"continue"}\n')
    p.stdin.flush()
except Exception as e:
    pass

time.sleep(1)
p.terminate()
