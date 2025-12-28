# Simple set comprehension test - English version
# Test with numbers list comprehension first

print("Test 1: List comprehension")
رقم list1 = [x for x in [0, 1, 2, 3, 4]]
print(list1)

print("\nTest 2: Set comprehension")
رقم set1 = {x for x in [0, 1, 2, 3, 4]}
print(set1)

print("\nTest 3: Set with duplicates removal")
رقم set2 = {x for x in [1, 1, 2, 2, 3, 3]}
print(set2)

print("\nTests complete!")
