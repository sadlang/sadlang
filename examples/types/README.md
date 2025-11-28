# 📊 أمثلة أنواع البيانات / Data Types Examples

هذا المجلد يحتوي على أمثلة اختبار لنظام الأنواع في لغة ص.

This folder contains test examples for the type system in Sad language.

## 📋 قائمة الملفات / File List

| # | الملف | الوصف | الصعوبة |
|---|-------|-------|---------|
| 1 | `01_integers_basic.s` | أرقام صحيحة بسيطة | سهل |
| 2 | `02_floats_mixed.s` | أرقام عشرية وخلط | متوسط |
| 3 | `03_strings_advanced.s` | سلاسل نصية متقدمة | متوسط |
| 4 | `04_arrays_operations.s` | عمليات المصفوفات | متقدم |
| 5 | `05_maps_complex.s` | خرائط معقدة | متقدم |

## 🎯 تغطية القواعد / Rules Coverage

- ✅ Integer (رقم): decimal, binary, octal, hex
- ✅ Float (عشري): scientific notation, mixed arithmetic
- ✅ String (نص): concatenation, f-strings, multiline
- ✅ Array (مصفوفة): literals, indexing, methods
- ✅ Map (خريطة): key-value pairs, operations
- ✅ Boolean (منطقي): true/false, logical operations
- ✅ None (لاشيء): null values

## 🚀 كيفية التشغيل / How to Run

```powershell
# تشغيل مثال واحد / Run single example
c:\s\s_language\build\bin\Debug\sad.exe .\01_integers_basic.s

# تشغيل جميع الأمثلة / Run all examples
Get-ChildItem *.s | ForEach-Object { 
    Write-Host "Running: $($_.Name)"
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName
}
```

## 📖 مرجع القواعد / Rules Reference

راجع `docs/language_spec/rules/01_types.md` للتفاصيل الكاملة.

See `docs/language_spec/rules/01_types.md` for complete details.
