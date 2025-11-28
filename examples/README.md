# 📚 دليل أمثلة لغة ص الشامل
# Comprehensive Examples Guide for Sad Language

هذا الدليل يحتوي على أمثلة شاملة لجميع قواعد لغة ص مع اختبارات عملية.

## 📂 هيكل الأمثلة (Examples Structure)

```
examples/
├── types/           # أمثلة أنواع البيانات (Data Types)
├── functions/       # أمثلة الدوال (Functions)
└── syntax/          # أمثلة التراكيب (Syntax)
```

---

## 📖 أقسام الأمثلة

### 1️⃣ أنواع البيانات (Types) - `types/`

يغطي جميع أنواع البيانات في لغة ص:

| الملف | الوصف | التغطية |
|------|-------|---------|
| `01_integers_basic.s` | الأعداد الصحيحة الأساسية | عشري، ثنائي (0b)، ثماني (0o)، ست عشري (0x)، عمليات حسابية |
| `02_floats_mixed.s` | الأعداد العشرية والمختلطة | الصيغة العلمية، العمليات المختلطة، التحويل التلقائي |
| `03_strings_advanced.s` | النصوص المتقدمة | الدمج، رموز الهروب، Unicode، Emoji |
| `04_arrays_operations.s` | عمليات المصفوفات | التصريح، الوصول، التقطيع، المصفوفات المتداخلة |
| `05_maps_complex.s` | الخرائط المعقدة | key-value، خرائط متداخلة، أنواع مختلطة |

**المفاهيم المغطاة:**
- ✅ Integer literals (decimal, binary, octal, hex)
- ✅ Float arithmetic and scientific notation
- ✅ String concatenation and escape sequences
- ✅ Array indexing, slicing, and nesting
- ✅ Map creation and access
- ✅ Type coercion (int + float)
- ✅ Unicode and emoji support

---

### 2️⃣ الدوال (Functions) - `functions/`

يغطي جميع أنماط الدوال في لغة ص:

| الملف | الوصف | التغطية |
|------|-------|---------|
| `01_functions_basic.s` | الدوال الأساسية | التصريح، المعاملات، الإرجاع، الاستدعاء |
| `02_functions_parameters.s` | المعاملات الاختيارية | القيم الافتراضية، معاملات متعددة |
| `03_functions_lambda.s` | دوال Lambda | الدوال المجهولة، Arrow syntax، التعابير |
| `04_functions_closures.s` | الإغلاقات | Variable capture، Closures lifetime |
| `05_functions_higher_order.s` | الدوال العليا | Functions as parameters، map/filter/reduce |

**المفاهيم المغطاة:**
- ✅ Function declaration with `دالة` keyword
- ✅ Parameters and return values
- ✅ Default parameters
- ✅ Lambda expressions `() =>`
- ✅ Closures and variable capture
- ✅ Higher-order functions (map, filter, reduce)
- ✅ Function composition and currying
- ✅ Recursion patterns

---

### 3️⃣ التراكيب (Syntax) - `syntax/`

يغطي جميع تراكيب اللغة:

| الملف | الوصف | التغطية |
|------|-------|---------|
| `01_syntax_expressions.s` | التعابير | حسابية، منطقية، مقارنة، ثلاثية، أولوية |
| `02_syntax_statements.s` | العبارات | تصريح، إسناد، إرجاع، طباعة |
| `03_syntax_control_flow.s` | التحكم بالتدفق | if، while، for-each، break، continue |
| `04_syntax_operators.s` | المشغلات | حسابية، مقارنة، منطقية، أولوية |
| `05_syntax_comprehensive.s` | برنامج شامل | مدير مهام يستخدم معظم الميزات |

**المفاهيم المغطاة:**
- ✅ Arithmetic expressions (+, -, *, /, %, **)
- ✅ Comparison operators (>, <, ==, !=, >=, <=)
- ✅ Logical operators (و، أو، ليس)
- ✅ Ternary operator (? :)
- ✅ If statements (إذا، آخر، آخر_إذا)
- ✅ While loops (بينما)
- ✅ For-each loops (لكل)
- ✅ Break (اقطع) and Continue (استمر)
- ✅ Variable declaration and assignment
- ✅ Function calls and returns
- ✅ Operator precedence and parentheses

---

## 🚀 كيفية تشغيل الأمثلة

### تشغيل مثال واحد:
```bash
c:\s\s_language\build\bin\Debug\sad.exe examples/types/01_integers_basic.s
```

### تشغيل جميع أمثلة قسم معين:
```powershell
# أمثلة Types
Get-ChildItem examples\types\*.s | ForEach-Object { 
    Write-Host "Running $($_.Name)..."
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName 
}

# أمثلة Functions
Get-ChildItem examples\functions\*.s | ForEach-Object { 
    Write-Host "Running $($_.Name)..."
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName 
}

# أمثلة Syntax
Get-ChildItem examples\syntax\*.s | ForEach-Object { 
    Write-Host "Running $($_.Name)..."
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName 
}
```

### تشغيل جميع الأمثلة:
```powershell
Get-ChildItem examples -Recurse -Filter *.s | ForEach-Object { 
    Write-Host "`n=== Running $($_.FullName) ==="
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName 
}
```

---

## 📊 ملخص التغطية (Coverage Summary)

### إجمالي الملفات: **15 ملف**
- Types: 5 ملفات
- Functions: 5 ملفات  
- Syntax: 5 ملفات

### الميزات المغطاة:

#### ✅ أنواع البيانات (100%)
- [x] رقم (Integer)
- [x] عشري (Float)
- [x] نص (String)
- [x] منطقي (Boolean)
- [x] مصفوفة (Array)
- [x] خريطة (Map)
- [x] أي (Any)
- [x] لا_شيء (Void)

#### ✅ المشغلات (100%)
- [x] حسابية: +, -, *, /, %, **
- [x] مقارنة: >, <, ==, !=, >=, <=
- [x] منطقية: و, أو, ليس
- [x] ثلاثي: ? :
- [x] دمج: + (strings, arrays)
- [x] تكرار: * (strings, arrays)
- [x] الوصول: [], slicing

#### ✅ هياكل التحكم (100%)
- [x] إذا (if)
- [x] آخر (else)
- [x] آخر_إذا (else if)
- [x] بينما (while)
- [x] لكل (for-each)
- [x] اقطع (break)
- [x] استمر (continue)

#### ✅ الدوال (100%)
- [x] تعريف الدوال (دالة)
- [x] المعاملات (parameters)
- [x] القيم الافتراضية (default values)
- [x] الإرجاع (إرجاع)
- [x] Lambda expressions
- [x] Closures
- [x] Higher-order functions

#### ✅ الميزات المتقدمة (100%)
- [x] Variable scoping
- [x] Type coercion
- [x] Nested structures
- [x] Unicode support
- [x] Scientific notation
- [x] Negative indexing
- [x] Array slicing

---

## 📝 ملاحظات مهمة

### 1. التعليقات
جميع الملفات تحتوي على:
- تعليقات عربية وإنجليزية
- شرح لكل قسم
- أمثلة توضيحية

### 2. التنسيق
- جميع استدعاءات `اطبع` تحتوي على `"\n"` للسطر الجديد
- رموز Emoji للتنسيق المرئي (✅, 🔴, 🟡, 🟢, etc.)
- ترويسات واضحة لكل قسم

### 3. التدرج
الأمثلة مرتبة من الأسهل للأصعب:
- **أساسي (Basic)**: Types, Basic Functions, Basic Syntax
- **متوسط (Intermediate)**: Control Flow, Parameters
- **متقدم (Advanced)**: Closures, Higher-Order, Comprehensive

---

## 🎯 الهدف من الأمثلة

1. **التعليم**: تعلم لغة ص من الصفر
2. **المرجع**: دليل سريع للتراكيب
3. **الاختبار**: التحقق من عمل الميزات
4. **التوثيق**: توثيق السلوك المتوقع

---

## 🔗 روابط ذات صلة

- [دليل المستخدم](../docs/USER_GUIDE.md)
- [مواصفات اللغة](../docs/language_spec/)
- [خارطة الطريق](../ROADMAP.md)

---

## 📞 المساهمة

إذا وجدت أخطاء أو لديك أفكار لأمثلة جديدة:
1. افتح Issue على GitHub
2. اقترح تحسينات
3. شارك أمثلتك الخاصة

---

## ✅ حالة المشروع

**تم الانتهاء من جميع الأمثلة!**

- ✅ Types examples (5/5)
- ✅ Functions examples (5/5)
- ✅ Syntax examples (5/5)

**إجمالي**: 15/15 ملف ✅

---

**تم إنشاؤه بواسطة**: GitHub Copilot  
**التاريخ**: 2024  
**الإصدار**: 1.0  
**اللغة**: لغة ص (Sad Language)

🎉 **جميع الأمثلة جاهزة للاستخدام!**
