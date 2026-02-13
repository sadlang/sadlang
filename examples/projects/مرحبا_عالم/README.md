# 🌍 مرحباً بالعالم / Hello World

## الوصف / Description

أول برنامج تقليدي في أي لغة برمجة - طباعة "مرحباً بالعالم".

The traditional first program in any programming language - printing "Hello World".

## ملفات المشروع / Project Files

```
مرحبا_عالم/
├── README.md          # هذا الملف
├── main.s             # البرنامج الرئيسي
├── hello_arabic.s     # نسخة عربية
└── hello_fancy.s      # نسخة متقدمة مع تنسيق
```

## التشغيل / Running

```bash
# البرنامج الأساسي
sad main.s

# النسخة العربية
sad hello_arabic.s

# النسخة المتقدمة
sad hello_fancy.s
```

## ما ستتعلم / What You'll Learn

- ✅ كيفية كتابة دالة رئيسية
- ✅ طباعة النصوص على الشاشة
- ✅ استخدام التعليقات
- ✅ بنية البرنامج الأساسية

## الكود المصدري / Source Code

### main.s - البرنامج الأساسي

```sad
# بسم الله الرحمن الرحيم
# أول برنامج بلغة ص

دالة رئيسية()
    اطبع("مرحباً بالعالم! 🌍\n")
    اطبع("Hello World!\n")
    ارجع 0
نهاية
```

### النتيجة المتوقعة / Expected Output

```
مرحباً بالعالم! 🌍
Hello World!
```
