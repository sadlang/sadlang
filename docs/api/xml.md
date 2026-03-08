# وثائق API — وحدة xml

> المسار: `stdlib/xml/`
> مساحة الأسماء: `sad::stdlib::xml`

## نظرة عامة
توفر الوحدة نموذج DOM مبسط: سمات، عناصر، مستند، ومحلل XML.

## الملفات الأساسية
- `xml/xml_module.h`
- `xml/xml_module.cpp`

## API رئيسية
- `class XmlAttribute`
  - `name/value`, `set_name`, `set_value`, `namespace_uri`
- `class XmlElement`
  - `set_attribute`, `get_attribute`, `has_attribute`, `remove_attribute`
  - `set_text`, `append_text`, `text`
  - `append_child`, `children`, `first_child`, `clear_children`
- `class XmlDocument`
  - `set_root`, `root`, `set_version`, `set_encoding`, `validate`
- `class XmlParser`
  - `parse(xmlText)`

## نقطة الدخول الموحّدة في المفسر
> الواجهات التالية مفعّلة في builtins لاستخدام XML مباشرة من كود `.ص`.
> ملاحظة تشغيل: يلزم `استورد محرك` قبل الاستخدام لأن التسجيل يتم ضمن وحدة المحرك.

- `حلل_xml(نص, خيارات?)` أو `xml_parse(...)` -> كائن/خريطة
- `حول_لـxml(قيمة, خيارات?)` أو `xml_stringify(...)` -> نص XML
- `xml_منسق(قيمة, مسافة=2)` أو `xml_pretty(...)` -> نص XML منسق
- `xml_مصغر(قيمة)` أو `xml_minify(...)` -> نص XML مضغوط

### ملاحظات خيارات XML
- `preserve_whitespace`: `صحيح/خطأ`
- `ignore_comments`: `صحيح/خطأ`
- `pretty`, `indent`, `omit_declaration` في stringify

## مثال سريع
```cpp
using namespace sad::stdlib::xml;
XmlDocument doc = XmlParser::parse("<root id=\"1\"></root>");
auto id = doc.root().get_attribute("id");
```
