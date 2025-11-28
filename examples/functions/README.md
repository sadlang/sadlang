# 🔧 أمثلة الدوال / Functions Examples

هذا المجلد يحتوي على أمثلة اختبار لنظام الدوال في لغة ص.

This folder contains test examples for the function system in Sad language.

## 📋 قائمة الملفات / File List

| # | الملف | الوصف | الصعوبة |
|---|-------|-------|---------|
| 1 | `01_functions_basic.s` | دوال بسيطة | سهل |
| 2 | `02_functions_parameters.s` | معاملات ومعاملات افتراضية | متوسط |
| 3 | `03_functions_lambda.s` | دوال مجهولة (Lambda) | متوسط |
| 4 | `04_functions_closures.s` | إغلاقات | متقدم |
| 5 | `05_functions_higher_order.s` | دوال عليا | متقدم |

## 🎯 تغطية القواعد / Rules Coverage

- ✅ Basic Functions: declaration, parameters, return
- ✅ Default Parameters: optional values
- ✅ Lambda Expressions: anonymous functions
- ✅ Closures: variable capture
- ✅ Higher-Order Functions: functions as arguments/return values
- ✅ Function Overloading (planned)
- ✅ Variadic Functions (planned)

## 🚀 كيفية التشغيل / How to Run

```powershell
# تشغيل مثال واحد / Run single example
c:\s\s_language\build\bin\Debug\sad.exe .\01_functions_basic.s

# تشغيل جميع الأمثلة / Run all examples
Get-ChildItem *.s | ForEach-Object { 
    Write-Host "Running: $($_.Name)"
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName
}
```

## 📖 مرجع القواعد / Rules Reference

راجع `docs/language_spec/rules/02_functions.md` للتفاصيل الكاملة.

See `docs/language_spec/rules/02_functions.md` for complete details.
