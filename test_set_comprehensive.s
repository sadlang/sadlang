# Comprehensive Set Comprehension Test
# Testing various set comprehension scenarios

print("=== Set Comprehension Tests ===\n")

# Test 1: Basic set comprehension
print("Test 1: Basic set comprehension")
رقم s1 = {x for x in [1, 2, 3, 4, 5]}
print(s1)

# Test 2: Set with duplicates removed
print("\nTest 2: Removing duplicates")
رقم s2 = {x for x in [1, 1, 2, 2, 3, 3, 4, 4]}
print(s2)

# Test 3: Set with expression
print("\nTest 3: With expression (x * 2)")
رقم s3 = {x * 2 for x in [1, 2, 3, 4]}
print(s3)

# Test 4: Set with condition
print("\nTest 4: With condition (x > 2)")
رقم s4 = {x for x in [1, 2, 3, 4, 5] if x > 2}
print(s4)

# Test 5: Set with expression and condition
print("\nTest 5: Expression + condition")
رقم s5 = {x * x for x in [1, 2, 3, 4, 5] if x % 2 == 1}
print(s5)

# Test 6: Set from string-like list (if supported)
print("\nTest 6: Mixed duplicates")
رقم s6 = {x for x in [1, 2, 1, 3, 2, 4, 3]}
print(s6)

print("\n=== All tests complete! ===")
