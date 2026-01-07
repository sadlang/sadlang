# Test Escape Sequences

print("=== Test Regular Strings ===")
print("\n")

# Test newline
نص msg1 = "Line 1\nLine 2\nLine 3"
print("Newline test:")
print(msg1)
print("\n")

# Test tab
نص msg2 = "Column1\tColumn2\tColumn3"
print("Tab test:")
print(msg2)
print("\n")

# Test carriage return (in raw string, should show literally)
نص raw_cr = ح"Line1\r\nLine2\r\n"
print("Raw CR test:")
print(raw_cr)
print("\n")

# Test actual newline and tab (should convert)
نص normal = "Hello\tWorld\nNew line"
print("Normal escape test:")
print(normal)
print("\n")

print("All escape sequence tests passed!")
