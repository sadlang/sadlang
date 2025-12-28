# (AR) اختبار Walrus Operator / (EN) Test Walrus Operator

print("=== Test Walrus Operator (:=) ===")
print("\n")

# Test 1: Basic walrus in if condition
# (AR) اختبار بسيط: walrus في شرط if
print("Test 1: Basic walrus in if")
if ((x := 10) > 5) {
    print("x = ")
    print(x)
    print(" (should be 10)")
}
print("\n")

# Test 2: Walrus in while condition
# (AR) اختبار: walrus في حلقة while
print("Test 2: Walrus in while")
رقم count = 0
while ((y := count) < 3) {
    print("y = ")
    print(y)
    print("\n")
    count = count + 1
}
print("\n")

# Test 3: Walrus with function call
# (AR) اختبار: walrus مع استدعاء دالة
print("Test 3: Walrus with expression")
if ((z := 5 + 5) == 10) {
    print("z = ")
    print(z)
    print(" (should be 10)")
}
print("\n")

print("✅ All Walrus tests completed!")
