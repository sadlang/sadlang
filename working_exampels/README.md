# أمثلة لغة ص / Sad Language Examples

مجموعة شاملة من الأمثلة لاختبار جميع الدوال الداخلية في لغة ص
Comprehensive collection of examples testing all builtin functions in Sad Language

---

## 📁 هيكل المجلدات / Directory Structure

```
working_exampels/
├── 01_basics/          # أمثلة أساسية / Basic examples
├── 02_variables/       # المتغيرات / Variables
├── 03_strings/         # دوال النصوص / String functions
├── 04_math/            # دوال الرياضيات / Math functions
├── 05_arrays/          # دوال المصفوفات / Array functions
├── 06_files/           # دوال الملفات / File I/O functions
├── 07_utilities/       # دوال الأدوات / Utility functions
├── 08_advanced/        # أمثلة متقدمة / Advanced examples
└── 09_comprehensive/   # اختبارات شاملة / Comprehensive tests
```

---

## 🚀 كيفية التشغيل / How to Run

### المفسر / Interpreter:
```bash
sad.exe example.sad
```

### المترجم / Compiler:
```bash
sadc.exe example.sad --emit-llvm
```

---

## 📚 الأمثلة المتاحة / Available Examples

### 1. دوال النصوص / String Functions

#### `test_strings_simple.sad`
اختبار أساسي للدوال الأساسية
Basic test for fundamental functions
```sad
دالة رئيسية()
    اطبع(طول_نص("Hello"))
    اطبع_سطر(تحويل_كبير("test"))
نهاية
```

**الدوال المختبرة:**
- `طول_نص` - string length
- `تحويل_كبير` - to uppercase
- `تحويل_صغير` - to lowercase

#### `test_strings_advanced.sad`
اختبار متقدم لجميع دوال النصوص
Advanced test for all string functions
```sad
اطبع_سطر(استبدل("Hello World", "World", "Sad"))
اطبع_سطر(استخراج("Hello", 1, 3))
اطبع(يحتوي_على("Hello", "ll"))
```

**الدوال المختبرة:**
- `استبدل` - replace
- `استخراج` - substring
- `قص_أطراف` - trim
- `يبدأ_ب` - starts with
- `ينتهي_ب` - ends with
- `يحتوي_على` - contains
- `بحث` - find

---

### 2. دوال الرياضيات / Math Functions

#### `test_math_basic.sad`
عمليات رياضية أساسية
Basic math operations
```sad
دالة رئيسية()
    اطبع(جذر(16))
    اطبع(أس(2, 3))
    اطبع(مطلق(-5))
نهاية
```

**الدوال المختبرة:**
- `جذر` - square root
- `أس` - power
- `مطلق` - absolute
- `تقريب` - round
- `أرضية` - floor
- `سقف` - ceil

#### `test_math_trig.sad`
الدوال الثلاثية
Trigonometric functions
```sad
اطبع(جيب(0.523599))
اطبع(جيب_تمام(1.047198))
اطبع(ظل(0.785398))
اطبع(مربع(5))
```

**الدوال المختبرة:**
- `جيب` - sine
- `جيب_تمام` - cosine
- `ظل` - tangent
- `مربع` - square

---

### 3. دوال الملفات / File I/O Functions

#### `test_files_basic.sad`
عمليات الملفات الأساسية
Basic file operations
```sad
اكتب_ملف("test.txt", "محتوى")
اطبع(اقرأ_ملف("test.txt"))
أضف_إلى_ملف("test.txt", " إضافة")
احذف_ملف("test.txt")
```

**الدوال المختبرة:**
- `اقرأ_ملف` - read file
- `اكتب_ملف` - write file
- `أضف_إلى_ملف` - append to file
- `احذف_ملف` - delete file

#### `test_file_operations.sad`
نسخ ونقل الملفات
File copy and move operations
```sad
انسخ_ملف("original.txt", "copy.txt")
انقل_ملف("copy.txt", "moved.txt")
```

**الدوال المختبرة:**
- `انسخ_ملف` - copy file
- `انقل_ملف` - move file

#### `test_folders.sad`
إدارة المجلدات
Folder management
```sad
أنشئ_مجلد("test_folder")
اكتب_ملف("test_folder/file.txt", "محتوى")
```

**الدوال المختبرة:**
- `أنشئ_مجلد` - create directory

---

### 4. دوال الأدوات / Utility Functions

#### `test_utilities.sad`
دوال مساعدة
Helper functions
```sad
اطبع(عشوائي())
نم(1)
```

**الدوال المختبرة:**
- `عشوائي` - random number
- `نم` - sleep

---

### 5. أمثلة متقدمة / Advanced Examples

#### `text_processor.sad`
معالج نصوص متكامل
Complete text processor
```sad
اطبع(طول_نص("  Hello WORLD  "))
اطبع(قص_أطراف("  Hello WORLD  "))
اطبع(تحويل_صغير(قص_أطراف("  Hello WORLD  ")))
```

**المفاهيم:**
- تسلسل الدوال / Function chaining
- معالجة نصوص / Text processing
- تنظيف البيانات / Data cleaning

#### `scientific_calculator.sad`
حاسبة علمية
Scientific calculator
```sad
اطبع(جذر(16))
اطبع(أس(2, 10))
اطبع(أرضية(3.7) + سقف(2.3))
اطبع(مربع(12))
```

**المفاهيم:**
- عمليات رياضية معقدة / Complex math operations
- دمج الدوال / Function composition
- حسابات علمية / Scientific calculations

#### `file_manager.sad`
نظام إدارة ملفات
File management system
```sad
أنشئ_مجلد("data")
أنشئ_مجلد("backup")
اكتب_ملف("data/users.txt", "أحمد,محمد,علي")
انسخ_ملف("data/users.txt", "backup/users.txt")
```

**المفاهيم:**
- نظام ملفات / File system
- نسخ احتياطي / Backup system
- إدارة البيانات / Data management

---

### 6. اختبار شامل / Comprehensive Test

#### `full_test.sad`
اختبار شامل لجميع الدوال
Comprehensive test of all functions

يختبر:
- 10 دوال نصوص / string functions
- 10 دوال رياضيات / math functions
- 7 دوال ملفات / file I/O functions
- 3 دوال أدوات / utility functions

**النتيجة:** ✅ جميع الدوال تعمل

---

## 📊 إحصائيات / Statistics

| الفئة / Category | الأمثلة / Examples | الحالة / Status |
|------------------|-------------------|-----------------|
| String Functions | 2 | ✅ يعمل |
| Math Functions   | 2 | ✅ يعمل |
| File I/O         | 3 | ✅ يعمل |
| Utilities        | 1 | ✅ يعمل |
| Advanced         | 3 | ✅ يعمل |
| Comprehensive    | 1 | ✅ يعمل |
| **المجموع / Total** | **12** | **✅ 100%** |

---

## 🎯 أهداف التعلم / Learning Goals

### المبتدئين / Beginners:
- ✅ فهم أساسيات الدوال / Understanding function basics
- ✅ استخدام دوال النصوص / Using string functions
- ✅ العمليات الرياضية / Math operations

### المتوسطين / Intermediate:
- ✅ دمج الدوال / Function composition
- ✅ إدارة الملفات / File management
- ✅ معالجة البيانات / Data processing

### المتقدمين / Advanced:
- ✅ بناء أنظمة معقدة / Building complex systems
- ✅ تسلسل العمليات / Operation chaining
- ✅ تطبيقات عملية / Practical applications

---

## 📝 ملاحظات / Notes

### للمترجم / For Compiler:
- ⚠️ استخدم `--emit-llvm` لإخراج LLVM IR
- ⚠️ لا تستخدم التعليقات `//` (غير مدعومة)
- ⚠️ استخدم القيم المباشرة بدلاً من المتغيرات إذا واجهت مشاكل

### للمفسر / For Interpreter:
- ✅ جميع الأمثلة تعمل مباشرة
- ✅ دعم كامل للدوال المُنفذة
- ⏳ دوال المصفوفات قيد التطوير

---

## 🐛 مشاكل معروفة / Known Issues

1. **التعليقات**
   - ❌ `//` غير مدعومة
   - ✅ الحل: بدون تعليقات

2. **المتغيرات**
   - ⚠️ مشاكل في المترجم
   - ✅ الحل: استخدام القيم مباشرة

3. **دوال المصفوفات**
   - ⏳ قيد التطوير
   - ❌ غير متاحة حالياً

---

## 📖 مراجع إضافية / Additional Resources

- [TEST_RESULTS.md](TEST_RESULTS.md) - نتائج الاختبار التفصيلية
- [BUILTIN_FUNCTIONS_REPORT.md](../BUILTIN_FUNCTIONS_REPORT.md) - توثيق الدوال

---

## 🤝 المساهمة / Contributing

لإضافة أمثلة جديدة:
1. اتبع التسمية: `test_<category>_<name>.sad`
2. اختبر في المفسر والمترجم
3. وثق النتائج في TEST_RESULTS.md

---

**آخر تحديث:** 4 فبراير 2026
**Last Updated:** February 4, 2026
