# ======================================================================
# test_advanced_numbers.s - اختبار الأرقام المتقدمة
# Test Advanced Number Formats
# ======================================================================
# الوصف:
#   اختبار شامل لجميع صيغ الأرقام المتقدمة الجديدة:
#   - Binary (0b, 0ثن)
#   - Octal (0o, 0ع)
#   - Hexadecimal (0x, 0س)
#   - Underscores في الأرقام (1_000_000)
#   - الأرقام العربية (٠-٩)
# ======================================================================

# ============ Binary Numbers (الأرقام الثنائية) ============

# صيغة إنجليزية (English format)
رقم binary1 = 0b1010
اطبع("Binary 0b1010 = ", binary1)

رقم binary2 = 0b0001_1111
اطبع("Binary with underscores 0b0001_1111 = ", binary2)

# صيغة عربية (Arabic format)
رقم binary3 = 0ثن1010
اطبع("Arabic binary 0ثن1010 = ", binary3)

رقم binary4 = 0ثن0001_1111
اطبع("Arabic binary with underscores 0ثن0001_1111 = ", binary4)

# ============ Octal Numbers (الأرقام الثمانية) ============

# صيغة إنجليزية
رقم octal1 = 0o17
اطبع("Octal 0o17 = ", octal1)

رقم octal2 = 0o755
اطبع("Octal 0o755 = ", octal2)

رقم octal3 = 0o1_777
اطبع("Octal with underscores 0o1_777 = ", octal3)

# صيغة عربية
رقم octal4 = 0ع17
اطبع("Arabic octal 0ع17 = ", octal4)

رقم octal5 = 0ع755
اطبع("Arabic octal 0ع755 = ", octal5)

# ============ Hexadecimal Numbers (الأرقام الست عشرية) ============

# صيغة إنجليزية
رقم hex1 = 0xFF
اطبع("Hex 0xFF = ", hex1)

رقم hex2 = 0x1A2B
اطبع("Hex 0x1A2B = ", hex2)

رقم hex3 = 0xDEAD_BEEF
اطبع("Hex with underscores 0xDEAD_BEEF = ", hex3)

# صيغة عربية
رقم hex4 = 0س15
اطبع("Arabic hex 0س15 = ", hex4)

رقم hex5 = 0س1A2B
اطبع("Arabic hex 0س1A2B = ", hex5)

# ============ Regular Numbers with Underscores ============

رقم big_number = 1_000_000
اطبع("Big number 1_000_000 = ", big_number)

رقم formatted_number = 123_456_789
اطبع("Formatted number 123_456_789 = ", formatted_number)

عشري pi_formatted = 3.141_592_653
اطبع("Pi formatted 3.141_592_653 = ", pi_formatted)

# ============ Arabic Digits (الأرقام العربية) ============

# أرقام عربية عادية
رقم arabic_num1 = ٤٢
اطبع("Arabic digits ٤٢ = ", arabic_num1)

رقم arabic_num2 = ١٢٣٤٥
اطبع("Arabic digits ١٢٣٤٥ = ", arabic_num2)

# أرقام عربية عشرية
عشري arabic_decimal = ٣.١٤
اطبع("Arabic decimal ٣.١٤ = ", arabic_decimal)

# ============ Mixed Tests ============

# خليط بين الأرقام العربية والإنجليزية (Lexer يحولها كلها للإنجليزية)
رقم mixed = 12٣45
اطبع("Mixed 12٣45 = ", mixed)

# ============ Summary ============

اطبع("")
اطبع("=== All Advanced Number Tests Complete ===")
اطبع("✓ Binary (0b, 0ثن)")
اطبع("✓ Octal (0o, 0ع)")
اطبع("✓ Hexadecimal (0x, 0س)")
اطبع("✓ Underscores (1_000_000)")
اطبع("✓ Arabic digits (٠-٩)")
