# 🌐 خادم ويب / Web Server

## الوصف / Description

خادم HTTP بسيط يوضح كيفية بناء تطبيقات الويب بلغة ص.

A simple HTTP server demonstrating how to build web applications in Sad Language.

## ملفات المشروع / Project Files

```
خادم_ويب/
├── README.md           # هذا الملف
├── main.s              # الخادم الرئيسي
├── src/
│   ├── router.s        # توجيه الطلبات
│   ├── handlers.s      # معالجات الطلبات
│   └── static.s        # الملفات الثابتة
├── public/             # الملفات العامة
│   ├── index.html
│   ├── style.css
│   └── script.js
└── config.s            # إعدادات الخادم
```

## الميزات / Features

- 🚀 خادم HTTP سريع وخفيف
- 📁 تقديم الملفات الثابتة
- 🛣️ نظام توجيه مرن
- 📝 دعم JSON
- 🔒 معالجة الأخطاء
- 📊 تسجيل الطلبات

## التشغيل / Running

```bash
sad main.s
# الخادم يعمل على http://localhost:8080
```

## API Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | `/` | الصفحة الرئيسية |
| GET | `/api/معلومات` | معلومات الخادم |
| GET | `/api/وقت` | الوقت الحالي |
| POST | `/api/رسالة` | إرسال رسالة |

## ما ستتعلم / What You'll Learn

- ✅ برمجة الشبكات
- ✅ بروتوكول HTTP
- ✅ معالجة الطلبات والاستجابات
- ✅ JSON
- ✅ الملفات الثابتة
- ✅ معالجة الأخطاء
