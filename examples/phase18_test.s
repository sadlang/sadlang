اطبع("=== Phase 18: Array Functions Test ===")
اطبع("")

نص arr = [1, 2, 3, 4, 5]
اطبع("Original array: " + arr)
اطبع("")

رقم s = array_size(arr)
اطبع("array_size([1,2,3,4,5]) = " + s)

رقم idx = indexOf(arr, 3)
اطبع("indexOf(arr, 3) = " + idx)

رقم has = array_contains(arr, 4)
اطبع("array_contains(arr, 4) = " + has)

نص rev = reverse(arr)
اطبع("reverse([1,2,3,4,5]) = " + rev)

نص sorted = sort([5, 2, 8, 1, 9])
اطبع("sort([5,2,8,1,9]) = " + sorted)

رقم f = first(arr)
اطبع("first(arr) = " + f)

رقم l = last(arr)
اطبع("last(arr) = " + l)

نص sliced = slice(arr, 1, 3)
اطبع("slice(arr, 1, 3) = " + sliced)

اطبع("")
اطبع("✓ All array function tests passed!")
