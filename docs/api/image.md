# وثائق API — وحدة image

> المسار: `stdlib/image/`
> مساحة الأسماء: `sad::stdlib::image`

## نظرة عامة
الوحدة توفر هياكل ألوان/نقاط/مستطيلات، وأنواع صور وتحويلات بصرية (resize/crop/filters/flip).

## الملفات الأساسية
- `image/image_module.h`
- `image/image_module.cpp`

## أهم الأنواع
- `struct Color`
  - `Black()`, `White()`, `Red()`, `Green()`, `Blue()`, `Transparent()`
- `struct Rectangle`
- `struct Point`
- `class Image`
- `enum class ImageFormat`
- `enum class ColorSpace`
- `enum class InterpolationMethod`
- `enum class FlipDirection`
- `enum class FilterType`

## وظائف مساعدة
- `format_to_string(ImageFormat)`
- `color_space_to_string(ColorSpace)`
- `interpolate_color(a, b, t)`

## مثال سريع
```cpp
using namespace sad::stdlib::image;
Image img(100, 100, ColorSpace::RGBA);
img.fill(Color::Red());
auto small = img.resize(50, 50);
```

## ملاحظات
- بعض ميزات التحميل/الحفظ تعتمد على headers خارجية (`stb_image*`).
- على Windows قد يحدث تعارض اسم `Rectangle` مع GDI API إذا لم يتم التأهيل الصحيح للاسم.
