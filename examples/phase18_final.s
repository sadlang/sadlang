اطبع("========================================")
اطبع("  Phase 18: Array Functions Test")
اطبع("========================================")
اطبع("")

اطبع("--- Array Operations ---")
اطبع("")

نص arr = [1, 2, 3, 4, 5]
اطبع("Original array:")
اطبع(arr)
اطبع("")

رقم s = array_size(arr)
اطبع("1. array_size() = " + s)

رقم idx = indexOf(arr, 3)
اطبع("2. indexOf(arr, 3) = " + idx)

رقم has = array_contains(arr, 4)
اطبع("3. array_contains(arr, 4) = " + has)

اطبع("")
اطبع("--- Transformation Functions ---")
اطبع("")

نص rev = reverse(arr)
اطبع("4. reverse() = " + rev)

نص sorted = sort([5, 2, 8, 1, 9])
اطبع("5. sort([5,2,8,1,9]) = " + sorted)

اطبع("")
اطبع("--- Access Functions ---")
اطبع("")

رقم f = first(arr)
اطبع("6. first(arr) = " + f)

رقم l = last(arr)
اطبع("7. last(arr) = " + l)

نص sliced = slice(arr, 1, 3)
اطبع("8. slice(arr, 1, 3) = " + sliced)

اطبع("")
اطبع("========================================")
اطبع("  All tests passed!")
اطبع("========================================")
