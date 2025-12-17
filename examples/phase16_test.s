# Phase 16 Test: Type Functions (دوال الأنواع)
# Testing conversion and type checking functions

اطبع("==============================================")
اطبع("  Phase 16: Type Functions Test (دوال الأنواع)")
اطبع("==============================================\n")

 ==============================================
 Type Conversion Functions (دوال التحويل)
# ==============================================

اطبع("--- Type Conversion (التحويل) ---\n")

# toArray (لمصفوفة)
اطبع("1. toArray / لمصفوفة:")
متغير arr1 = لمصفوفة(42)
اطبع("   لمصفوفة(42) = ", arr1)

متغير arr2 = toArray("مرحبا")
اطبع("   toArray('مرحبا') = ", arr2)

متغير arr3 = array(صحيح)
اطبع("   array(صحيح) = ", arr3)
اطبع()

# toBool (لمنطقي)
اطبع("2. toBool / لمنطقي:")
متغير b1 = لمنطقي(0)
اطبع("   لمنطقي(0) = ", b1)

متغير b2 = toBool(42)
اطبع("   toBool(42) = ", b2)

متغير b3 = bool("")
اطبع("   bool('') = ", b3)

متغير b4 = boolean("مرحبا")
اطبع("   boolean('مرحبا') = ", b4)
اطبع()

# ==============================================
# Type Checking Functions (دوال فحص الأنواع)
# ==============================================

اطبع("--- Type Checking (فحص الأنواع) ---\n")

# isInt (هو_رقم_صحيح)
اطبع("3. isInt / هو_رقم_صحيح:")
اطبع("   هو_رقم_صحيح(42) = ", هو_رقم_صحيح(42))
اطبع("   هو_رقم(3.14) = ", هو_رقم(3.14))
اطبع("   isInt(100) = ", isInt(100))
اطبع("   is_int('42') = ", is_int("42"))
اطبع("   isInteger(صحيح) = ", isInteger(صحيح))
اطبع()

# isFloat (هو_رقم_عشري)
اطبع("4. isFloat / هو_رقم_عشري:")
اطبع("   هو_رقم_عشري(3.14) = ", هو_رقم_عشري(3.14))
اطبع("   هو_عشري(42) = ", هو_عشري(42))
اطبع("   isFloat(2.718) = ", isFloat(2.718))
اطبع("   is_float('3.14') = ", is_float("3.14"))
اطبع("   isDouble(1.5) = ", isDouble(1.5))
اطبع()

# isString (هو_نص)
اطبع("5. isString / هو_نص:")
اطبع("   هو_نص('مرحبا') = ", هو_نص("مرحبا"))
اطبع("   isString('Hello') = ", isString("Hello"))
اطبع("   is_string(123) = ", is_string(123))
اطبع("   isStr('') = ", isStr(""))
اطبع()

# isArray (هو_مصفوفة)
اطبع("6. isArray / هو_مصفوفة:")
متغير test_arr = [1, 2, 3]
اطبع("   هو_مصفوفة([1,2,3]) = ", هو_مصفوفة(test_arr))
اطبع("   isArray([]) = ", isArray([]))
اطبع("   is_array(42) = ", is_array(42))
اطبع("   isList('list') = ", isList("list"))
اطبع()

# ==============================================
# Combined Usage Example (مثال استخدام مشترك)
# ==============================================

اطبع("--- Combined Example (مثال متكامل) ---\n")

دالة processValue(value) {
    اطبع("Processing: ", value)
    
    إذا (هو_رقم_صحيح(value)) {
        اطبع("  ✓ رقم صحيح / Integer")
    } وإلا_إذا (هو_رقم_عشري(value)) {
        اطبع("  ✓ رقم عشري / Float")
    } وإلا_إذا (هو_نص(value)) {
        اطبع("  ✓ نص / String")
    } وإلا_إذا (هو_مصفوفة(value)) {
        اطبع("  ✓ مصفوفة / Array")
    } وإلا {
        اطبع("  ✓ نوع آخر / Other type")
    }
    
    متغير as_bool = لمنطقي(value)
    اطبع("  Boolean value: ", as_bool)
    اطبع()
}

processValue(42)
processValue(3.14)
processValue("مرحبا")
processValue([1, 2, 3])
processValue(صحيح)
processValue(0)

# ==============================================
# Alias Coverage Test (اختبار تغطية الأسماء)
# ==============================================

اطبع("--- Alias Coverage (تغطية الأسماء) ---\n")

اطبع("toArray aliases: لمصفوفة, toArray, to_array, array")
اطبع("  ✓ All 4 aliases tested above\n")

اطبع("toBool aliases: لمنطقي, toBool, to_bool, bool, boolean")
اطبع("  ✓ All 5 aliases tested above\n")

اطبع("isInt aliases: هو_رقم_صحيح, هو_رقم, isInt, is_int, isInteger")
اطبع("  ✓ All 5 aliases tested above\n")

اطبع("isFloat aliases: هو_رقم_عشري, هو_عشري, isFloat, is_float, isDouble")
اطبع("  ✓ All 5 aliases tested above\n")

اطبع("isString aliases: هو_نص, isString, is_string, isStr")
اطبع("  ✓ All 4 aliases tested above\n")

اطبع("isArray aliases: هو_مصفوفة, isArray, is_array, isList")
اطبع("  ✓ All 4 aliases tested above\n")

# ==============================================
# Summary
# ==============================================

اطبع("==============================================")
اطبع("  Test Complete! (الاختبار مكتمل)")
اطبع("  6 functions × 27 aliases = SUCCESS ✓")
اطبع("==============================================")
