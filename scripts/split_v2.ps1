# سكريبت تقسيم v2 - بدون دوال
$src = "c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_helpers.cpp"
$dir = "c:\s_lang\s-programming-language\compiler_new\src\frontend"

# أولاً استعادة الملف الأصلي من النسخة المقصوصة + الملفين الجديدين
# helpers الآن 1102 سطر (0-1096 + namespace close)
# نحتاج إعادة تجميعه

$helpers = [IO.File]::ReadAllLines($src)
Write-Host "Current helpers: $($helpers.Count)"

# الملف الأصلي مقصوص بالفعل إلى 1102 سطر
# لكن الملفات الجديدة فارغة! 
# نحتاج إعادة بناء من git

# لنفحص: هل الملف الأصلي في git؟
Set-Location "c:\s_lang\s-programming-language"
git checkout -- "compiler_new/src/frontend/sir_builder_helpers.cpp" 2>&1
Write-Host "Restored from git"

$all = [IO.File]::ReadAllLines($src)
Write-Host "Restored helpers: $($all.Count)"
