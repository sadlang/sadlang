اطبع("Testing Type Functions")
اطبع()

رقم x = 42
اطبع("isInt(42) = ", isInt(x))

متغير y = 3.14
اطبع("isFloat(3.14) = ", isFloat(y))

متغير s = "hello"
اطبع("isString('hello') = ", isString(s))

متغير arr = [1, 2, 3]
اطبع("isArray([1,2,3]) = ", isArray(arr))

متغير b1 = toBool(0)
اطبع("toBool(0) = ", b1)

متغير b2 = toBool(42)
اطبع("toBool(42) = ", b2)

متغير a = toArray(5)
اطبع("toArray(5) = ", a)

اطبع()
اطبع("All tests complete!")
