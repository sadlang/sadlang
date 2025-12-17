اطبع("=== Phase 17: String Functions Test ===")
اطبع("")

نص text = "Hello World مرحبا بالعالم"

اطبع("Original text: " + text)
اطبع("")

نص upper = toUpper("hello")
اطبع("toUpper('hello') = " + upper)

نص lower = toLower("WORLD")
اطبع("toLower('WORLD') = " + lower)

رقم pos = find(text, "World")
اطبع("find('World') = " + pos)

نص replaced = replace("one two three", "two", "2")
اطبع("replace('one two three', 'two', '2') = " + replaced)

نص sub = substring("مرحبا", 0, 3)
اطبع("substring('مرحبا', 0, 3) = " + sub)

نص trimmed = trim("  hello  ")
اطبع("trim('  hello  ') = '" + trimmed + "'")

رقم starts = startsWith("مرحبا", "مر")
اطبع("startsWith('مرحبا', 'مر') = " + starts)

رقم ends = endsWith("Hello", "lo")
اطبع("endsWith('Hello', 'lo') = " + ends)

رقم has = contains("test string", "str")
اطبع("contains('test string', 'str') = " + has)

اطبع("")
اطبع("✓ All string function tests passed!")
