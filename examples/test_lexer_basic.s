# ======================================================================
# اختبار ميزات المحلل المعجمي - Lexer فقط
# Test Lexer features only - bypassing Parser
# ======================================================================

# اختبار أساسي للأرقام والنصوص العادية
# Basic test for numbers and regular strings

رقم bin = 0b1010
رقم oct = 0o17
رقم hex = 0xFF
رقم عربي = ١٢٣

نص normal = "Hello World"
نص unicode = "مرحبا \u0628\u0643"
نص hex_esc = "\x41\x42"

print(bin)
print(oct)
print(hex)
print(عربي)
print(normal)
print(unicode)
print(hex_esc)
