// Simple Phase 16 Test
اطبع("Testing Type Functions")
اطبع()

// Test isInt
متغير x = 42
اطبع("isInt(42) = ", isInt(x))

// Test isFloat  
متغير y = 3.14
اطبع("isFloat(3.14) = ", isFloat(y))

// Test isString
متغير s = "hello"
اطبع("isString('hello') = ", isString(s))

// Test isArray
متغير arr = [1, 2, 3]
اطبع("isArray([1,2,3]) = ", isArray(arr))

// Test toBool
متغير b1 = toBool(0)
اطبع("toBool(0) = ", b1)

متغير b2 = toBool(42)
اطبع("toBool(42) = ", b2)

// Test toArray
متغير a = toArray(5)
اطبع("toArray(5) = ", a)

اطبع()
اطبع("All tests complete!")
