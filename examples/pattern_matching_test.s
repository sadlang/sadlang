# Pattern Matching Test (اختبار مطابقة الأنماط)
# Test 1: Basic literal matching
print("=== Test 1: Literal Matching ===")
var x = 1
match x {
    case 0: print("zero")
    case 1: print("واحد (one)")
    case 2: print("two")
نهاية

# Test 2: Variable binding
print("\n=== Test 2: Variable Binding ===")
var point = [10, 20]
match point {
    case [0, 0]: print("نقطة الأصل (origin)")
    case [x, 0]: print("على المحور السيني: ", x)
    case [x, y]: print("نقطة عند: x=", x, " y=", y)
نهاية

# Test 3: Guard conditions
print("\n=== Test 3: Guards ===")
var n = 5
match n {
    case x if x > 10: print("كبير: ", x)
    case x if x > 0: print("موجب: ", x)
    case x if x < 0: print("سالب: ", x)
    case 0: print("صفر")
نهاية

# Test 4: Wildcard pattern
print("\n=== Test 4: Wildcard ===")
var status = "unknown"
match status {
    case "ok": print("نجح")
    case "error": print("فشل")
    case _: print("حالة غير معروفة")
نهاية

# Test 5: Array matching
print("\n=== Test 5: Array Matching ===")
var items = [1, 2, 3]
match items 
    case []: print("فارغ")
    case [a]: print("عنصر واحد: ", a)
    case [a, b]: print("عنصران: ", a, ", ", b)
    case [a, b, c]: print("ثلاثة عناصر: ", a, ", ", b, ", ", c)
نهاية

print("\n✅ جميع الاختبارات نجحت!")
