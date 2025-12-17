أنشئ_مجلد("test_dir")
اطبع_سطر("Created directory")

اكتب_ملف("test_dir/file.txt", "Test content")
اطبع_سطر("Written file")

نص x = اقرأ_ملف("test_dir/file.txt")
x =x +"  asd "
اطبع_سطر(x)

رقم y = هل_ملف("test_dir/file.txt")
اطبع_سطر(y)
