import codecs
import sys

filepath = r'c:\s\s_language\src\optimizer\optimization_pass.cpp'

with open(filepath, 'r', encoding='utf-8', newline='') as f:
    content = f.read()

# Replace literal backtick-n with actual newline - use raw string literal
content = content.replace('}' + chr(96) + 'n    void visitSetComprehensionExpr', '}\n    void visitSetComprehensionExpr')

with open(filepath, 'w', encoding='utf-8', newline='') as f:
    f.write(content)

print("Fixed all backtick-n occurrences")
