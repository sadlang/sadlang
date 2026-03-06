import sys
fname = sys.argv[1] if len(sys.argv) > 1 else 'test_out2.txt'
with open(fname, 'rb') as f:
    content = f.read().decode('utf-8', errors='replace')
lines = content.split('\n')
for line in lines:
    if '\u2717' in line or '\u2713' in line:
        sys.stdout.buffer.write((line + '\n').encode('utf-8'))
