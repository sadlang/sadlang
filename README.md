# 🇸🇦 لغة ص - لغة برمجة عربية حديثة

<div dir="rtl">

## نظرة عامة

**لغة ص** هي لغة برمجة عربية حديثة تهدف إلى تسهيل تعلم البرمجة للمتحدثين بالعربية. تدعم اللغة البرمجة الكائنية، نظام رسومات متقدم، وتوفر بيئة سهلة وبسيطة للمبتدئين.

### ✨ المميزات الرئيسية

- 🌍 **دعم كامل للغة العربية**: جميع الكلمات المفتاحية والمتغيرات بالعربية
- 🎨 **نظام رسومات متقدم**: بناء واجهات مستخدم بطريقة سهلة وحديثة
- 🧩 **البرمجة الكائنية**: دعم كامل للأصناف والوراثة
- 📚 **مكتبات قياسية غنية**: رياضيات، إدخال/إخراج، نصوص، وقت، رسومات
- 🚀 **سهلة التعلم**: صممت خصيصاً للمبتدئين
- 🔧 **قابلة للتوسع**: نظام مكتبات مرن

### 📋 المتطلبات

- **CMake** 3.15 أو أحدث
- **مترجم C++** يدعم معيار C++17
  - Visual Studio 2019 أو أحدث (Windows)
  - GCC 7+ أو Clang 5+ (Linux/macOS)

### 🔨 البناء والتثبيت

#### Windows

</div>

```bash
# 1. إنشاء مجلد البناء
mkdir build
cd build

# 2. إنشاء ملفات البناء
cmake ..

# 3. بناء المشروع
cmake --build . --config Release

# 4. تشغيل البرنامج
.\bin\Release\sad.exe
```

<div dir="rtl">

#### Linux/macOS

</div>

```bash
# 1. إنشاء مجلد البناء
mkdir build && cd build

# 2. إنشاء ملفات البناء
cmake ..

# 3. بناء المشروع
make -j4

# 4. تشغيل البرنامج
./bin/sad
```

<div dir="rtl">

### 🎯 أمثلة الاستخدام

#### مثال 1: مرحباً بالعالم

</div>

```sad
اطبع("مرحباً بالعالم!")
```

<div dir="rtl">

#### مثال 2: المتغيرات والعمليات الحسابية

</div>

```sad
رقم س = 10
رقم ص = 20
رقم النتيجة = س + ص
اطبع("النتيجة: " + النتيجة)
```

<div dir="rtl">

#### مثال 3: الدوال

</div>

```sad
دالة جمع(رقم أ، رقم ب) -> رقم {
    إرجاع أ + ب
}

رقم النتيجة = جمع(5، 3)
اطبع("5 + 3 = " + النتيجة)
```

<div dir="rtl">

#### مثال 4: الشروط والحلقات

</div>

```sad
رقم العدد = 10

إذا العدد > 5 {
    اطبع("العدد أكبر من 5")
} وإلا {
    اطبع("العدد أصغر من أو يساوي 5")
}

بينما العدد > 0 {
    اطبع("العدد: " + العدد)
    العدد = العدد - 1
}
```

<div dir="rtl">

#### مثال 5: البرمجة الكائنية

</div>

```sad
صنف شخص {
    خاص:
        نص الاسم
        رقم العمر
    
    عام:
        دالة منشئ(نص اسم، رقم عمر) {
            هذا.الاسم = اسم
            هذا.العمر = عمر
        }
        
        دالة عرف() {
            اطبع("الاسم: " + هذا.الاسم)
            اطبع("العمر: " + هذا.العمر)
        }
}

شخص أحمد = جديد شخص("أحمد"، 25)
أحمد.عرف()
```

<div dir="rtl">

#### مثال 6: الواجهات الرسومية

</div>

```sad
صنف تطبيقي يرث من عنصر_بلا_حالة {
    دالة ارسم() -> عنصر {
        إرجاع نافذة(
            العنوان: "تطبيقي الأول"،
            العرض: 800،
            الارتفاع: 600،
            الطفل: عمود(
                الأطفال: [
                    نص("مرحباً بك في لغة ص!")،
                    زر(
                        النص: "اضغط هنا"،
                        عند_الضغط: دالة() {
                            اطبع("تم الضغط على الزر!")
                        }
                    )
                ]
            )
        )
    }
}

نافذة_التطبيق(جديد تطبيقي())
```

<div dir="rtl">

### 📂 هيكل المشروع

</div>

```
s_language/
├── include/              # ملفات الترويسة (.h)
│   ├── lexer/           # المحلل المعجمي
│   ├── parser/          # المحلل النحوي
│   ├── interpreter/     # المفسر التنفيذي
│   ├── data/            # إدارة البيانات
│   ├── stdlib/          # المكتبات القياسية
│   ├── errors/          # إدارة الأخطاء
│   ├── debug/           # أدوات التنقيح
│   └── utils/           # أدوات مساعدة
├── src/                 # ملفات المصدر (.cpp)
│   ├── lexer/
│   ├── parser/
│   ├── interpreter/
│   ├── data/
│   ├── stdlib/
│   └── main.cpp
├── tests/               # الاختبارات
├── examples/            # أمثلة الاستخدام
├── docs/                # التوثيق
├── build/               # ملفات البناء
└── CMakeLists.txt       # ملف البناء الرئيسي
```

<div dir="rtl">

### 🧪 تشغيل الاختبارات

</div>

```bash
cd build
ctest --output-on-failure
```

<div dir="rtl">

### 📖 التوثيق

- [دليل المستخدم](docs/tutorials/getting_started.md)
- [مرجع API](docs/api/reference.md)
- [المعمارية التقنية](docs/architecture/design.md)

### 🤝 المساهمة

نرحب بمساهماتكم! يرجى قراءة [دليل المساهمة](CONTRIBUTING.md) قبل البدء.

### 📝 الترخيص

هذا المشروع مرخص تحت [رخصة MIT](LICENSE).

### 👥 الفريق

- **المطور الرئيسي**: فريق لغة ص
- **البريد الإلكتروني**: contact@sadlang.org
- **الموقع**: https://sadlang.org

### 🌟 الدعم

إذا أعجبك المشروع، لا تنسى إعطاءه نجمة ⭐ على GitHub!

</div>

---

# 🇬🇧 Sad - Modern Arabic Programming Language

## Overview

**Sad Language** is a modern Arabic programming language designed to make programming accessible to Arabic speakers. It supports object-oriented programming, an advanced graphics system, and provides an easy and simple environment for beginners.

### ✨ Key Features

- 🌍 **Full Arabic Support**: All keywords and variables in Arabic
- 🎨 **Advanced Graphics System**: Build user interfaces easily and modernly
- 🧩 **Object-Oriented Programming**: Full support for classes and inheritance
- 📚 **Rich Standard Libraries**: Math, I/O, strings, time, graphics
- 🚀 **Easy to Learn**: Designed specifically for beginners
- 🔧 **Extensible**: Flexible library system

### 📋 Requirements

- **CMake** 3.15 or newer
- **C++ Compiler** supporting C++17 standard
  - Visual Studio 2019 or newer (Windows)
  - GCC 7+ or Clang 5+ (Linux/macOS)

### 🔨 Build and Installation

See the Arabic section above for build instructions.

### 📖 Documentation

- [User Guide](docs/tutorials/getting_started.md)
- [API Reference](docs/api/reference.md)
- [Technical Architecture](docs/architecture/design.md)

### 🤝 Contributing

We welcome your contributions! Please read the [Contributing Guide](CONTRIBUTING.md) before starting.

### 📝 License

This project is licensed under the [MIT License](LICENSE).

### 👥 Team

- **Lead Developer**: Sad Language Team
- **Email**: contact@sadlang.org
- **Website**: https://sadlang.org

### 🌟 Support

If you like the project, don't forget to give it a star ⭐ on GitHub!

---

**Made with ❤️ for the Arabic developer community**

**صُنع بـ ❤️ لمجتمع المطورين العرب**
