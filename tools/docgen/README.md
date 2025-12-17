# بسم الله الرحمن الرحيم

# Sad Documentation Generator / مولد التوثيق للغة Sad

**الحمد لله** - أداة احترافية لتوليد التوثيق التلقائي للغة Sad!

## ✨ الميزات / Features

- 📚 **استخراج تلقائي** - يستخرج التوثيق من تعليقات الكود
- 🌍 **ثنائي اللغة** - يدعم العربية والإنجليزية معاً
- 🎨 **صيغ متعددة** - HTML، Markdown، JSON
- 🔍 **بحث ذكي** - فهرس بحث تلقائي
- 📱 **تصميم متجاوب** - يعمل على جميع الأجهزة
- ⚡ **سريع وفعال** - معالجة سريعة للملفات الكبيرة

## 📖 صيغة التعليقات التوثيقية / Doc Comment Format

### تعليق بسيط / Simple Comment

```sad
# @brief وصف قصير / Short description
# @param x وصف المعامل / Parameter description
# @return وصف القيمة المرجعة / Return value description
دالة مثال(رقم x) {
    إرجاع x * ٢
}
```

### تعليق متعدد الأسطر / Multi-line Comment

```sad
###
# @brief حساب المساحة / Calculate area
# @brief_ar حساب مساحة المستطيل
# @description Calculates the area of a rectangle
#              given width and height.
# @description_ar يحسب مساحة المستطيل من العرض والطول
# @param عرض عرض المستطيل / Rectangle width
# @param طول طول المستطيل / Rectangle height
# @return المساحة / Area
# @example
#   رقم مساحة = حساب_مساحة(٥، ١٠)
#   اطبع(مساحة)  # سيطبع: ٥٠
# @since 1.0.0
# @author Your Name
###
دالة حساب_مساحة(رقم عرض، رقم طول) {
    إرجاع عرض * طول
}
```

## 🏷️ الوسوم المدعومة / Supported Tags

| الوسم / Tag | الوصف / Description |
|------------|---------------------|
| `@brief` | وصف قصير / Short description (English) |
| `@brief_ar` | وصف قصير بالعربية / Short description (Arabic) |
| `@description` | وصف مفصل / Detailed description (English) |
| `@description_ar` | وصف مفصل بالعربية / Detailed description (Arabic) |
| `@param <name> <desc>` | وصف معامل / Parameter description |
| `@return <desc>` | وصف القيمة المرجعة / Return value description |
| `@example` | مثال استخدام / Usage example |
| `@see <name>` | ارجع إلى / See also reference |
| `@since <version>` | منذ الإصدار / Since version |
| `@author <name>` | المؤلف / Author |
| `@deprecated` | مهمل / Deprecated |

## 🚀 الاستخدام / Usage

### بناء الأداة / Building the Tool

```bash
cd tools/docgen
mkdir build
cd build
cmake ..
cmake --build .
```

### توليد التوثيق / Generating Documentation

#### HTML (الصيغة الافتراضية)

```bash
# توليد HTML من مجلد src
saddoc src/

# تحديد مجلد الإخراج
saddoc -o docs/api src/

# مجلدات متعددة
saddoc src/ lib/ examples/
```

#### Markdown

```bash
saddoc -f markdown -o api_docs src/
```

#### JSON

```bash
saddoc -f json -o api.json src/
```

### خيارات سطر الأوامر / Command Line Options

```
-h, --help              عرض رسالة المساعدة / Show help message
-o, --output <dir>      مجلد الإخراج / Output directory (default: docs)
-f, --format <format>   صيغة الإخراج / Output format: html, markdown, json
-n, --name <name>       اسم المشروع / Project name
--name-ar <name>        اسم المشروع بالعربية / Project name in Arabic
-v, --version <ver>     إصدار المشروع / Project version
--verbose               إخراج مفصل / Verbose output
```

## 📁 هيكل الإخراج / Output Structure

### HTML

```
docs/
├── index.html          # الصفحة الرئيسية / Main page
├── style.css           # أنماط CSS / CSS styles
├── script.js           # JavaScript
├── function1.html      # صفحة دالة / Function page
├── function2.html
└── class1.html         # صفحة صنف / Class page
```

### Markdown

```
api_docs/
└── API.md             # ملف واحد يحتوي على كل التوثيق
                       # Single file with all documentation
```

## 🎯 مثال كامل / Complete Example

انظر إلى `examples/docs/example_with_docs.s` لمثال كامل على التوثيق.

See `examples/docs/example_with_docs.s` for a complete documentation example.

```bash
# توليد التوثيق من المثال
saddoc -n "Math Library" --name-ar "مكتبة الرياضيات" examples/docs/
```

## 🎨 تخصيص المظهر / Customizing the Theme

يمكنك تعديل ملف `style.css` الناتج لتخصيص مظهر التوثيق:

You can modify the generated `style.css` file to customize the documentation appearance:

- ألوان العناوين / Header colors
- خطوط النص / Text fonts
- تخطيط الصفحة / Page layout
- ألوان الكود / Code colors

## 📊 الإحصائيات / Statistics

- **docgen.h**: 522 سطر / lines
- **docgen.cpp**: 650+ سطر / lines  
- **main.cpp**: 200+ سطر / lines
- **المجموع / Total**: 1,400+ سطر / lines

## 🔧 API Reference

### DocExtractor

```cpp
DocExtractor extractor;
extractor.extract_from_file("path/to/file.s");
auto comments = extractor.get_comments();
```

### DocGenerator

```cpp
DocGenerator generator;
DocGenerator::Settings settings;
settings.project_name = "My Project";
settings.project_name_ar = "مشروعي";
generator.set_settings(settings);
generator.generate(comments, OutputFormat::HTML, "output_dir");
```

## 🤝 المساهمة / Contributing

نرحب بمساهماتكم! يرجى:

1. Fork المشروع
2. إنشاء فرع للميزة الجديدة
3. Commit التغييرات
4. Push إلى الفرع
5. فتح Pull Request

## 📄 الترخيص / License

هذا المشروع مفتوح المصدر ومتاح تحت ترخيص MIT.

This project is open source and available under the MIT License.

## 🌟 الحمد لله

**الحمد لله رب العالمين!**

مولد توثيق احترافي للغة Sad - يجمع بين العربية والإنجليزية بسلاسة! ✨
