اطبع_سطر("=== Phase 22: Filesystem Functions Test ===")

# Test 1: Create directory
اطبع("Test create_directory: ")
أنشئ_مجلد("test_output")
اطبع_سطر("✓")

# Test 2: Write and read file
اطبع("Test write_file & read_file: ")
اكتب_ملف("test_output/test.txt", "Hello World!")
متغير محتوى = اقرأ_ملف("test_output/test.txt")
اطبع_سطر(محتوى)

# Test 3: Append to file
اطبع("Test append_to_file: ")
أضف_إلى_ملف("test_output/test.txt", "\nLine 2")
اطبع_سطر("✓")

# Test 4: Check if file exists
اطبع("Test file_exists: ")
متغير موجود = هل_موجود("test_output/test.txt")
اطبع_سطر(موجود)

# Test 5: Check if is_file
اطبع("Test is_file: ")
متغير ملف = هل_ملف("test_output/test.txt")
اطبع_سطر(ملف)

# Test 6: Check if is_directory
اطبع("Test is_directory: ")
متغير مجلد = هل_مجلد("test_output")
اطبع_سطر(مجلد)

# Test 7: List directory
اطبع("Test list_directory: ")
متغير قائمة = اسرد_مجلد("test_output")
اطبع_سطر(قائمة)

اطبع_سطر("=== All Filesystem Functions Work! ===")
