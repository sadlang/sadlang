# المرحلة 5: تحليل stdlib

## 📅 التاريخ: 8 يناير 2026

---

## 📊 تحليل التكرار في HTTP

### stdlib/http/ (3 ملفات)

| الملف | الوصف |
|-------|-------|
| http_builtins.cpp | دوال HTTP المدمجة |
| http_module.cpp | وحدة HTTP |
| http_module.h | header لوحدة HTTP |

### stdlib/network/http/ (7 ملفات)

| الملف | الوصف |
|-------|-------|
| CMakeLists.txt | ملف بناء |
| http_base.cpp | قاعدة HTTP |
| http_client.cpp | عميل HTTP |
| http_cookie.cpp | إدارة Cookies |
| http_request.cpp | طلبات HTTP |
| http_response.cpp | استجابات HTTP |
| http_server.cpp | خادم HTTP |
| http_url.cpp | معالجة URLs |

---

## 🎯 قرار التوحيد

### الخيار المقترح: دمج في stdlib/network/http/

**السبب:**
- stdlib/network/http/ أكثر اكتمالاً (7 ملفات vs 3 ملفات)
- يحتوي على CMakeLists.txt
- يتبع هيكل منطقي (client, server, request, response)

### خطوات التوحيد:

1. نسخ ملفات stdlib/http/ إلى stdlib/network/http/
2. تحديث CMakeLists.txt لـ network/http
3. حذف stdlib/http/ القديم

---

## 📊 تحليل stdlib/modules/

يحتوي فقط على:
- PHASE5_INTEGRATION_PLAN.md

**القرار:** يمكن حذفه أو الإبقاء عليه كوثائق

---

## ✅ الإجراءات المنفذة

لتجنب كسر البناء، سنقوم بـ:
1. توثيق التكرار
2. عدم حذف الملفات القديمة الآن
3. ترك القرار للمطور

---

## 📁 الهيكل الموصى به لـ stdlib

```
stdlib/
├── core/           ✅ جيد - لا تغيير
├── crypto/         ✅ جيد - لا تغيير
├── database/       ✅ جيد - لا تغيير
├── filesystem/     ✅ جيد - لا تغيير
├── graphics/       ✅ جيد - لا تغيير
├── image/          ✅ جيد - لا تغيير
├── io/             ✅ جيد - لا تغيير
├── json/           ✅ جيد - لا تغيير
├── math/           ✅ جيد - لا تغيير
├── string/         ✅ جيد - لا تغيير
├── xml/            ✅ جيد - لا تغيير
│
├── network/        ⚠️ يحتاج تنظيف
│   ├── http/       ← دمج stdlib/http/ هنا
│   ├── websocket/
│   └── ...
│
├── http/           ⚠️ مكرر - للحذف لاحقاً
│
└── modules/        ⚠️ للحذف (يحتوي وثائق فقط)
```

---

## ✅ الحالة: تحليل مكتمل

نظراً لمخاطر كسر البناء، لن نقوم بحذف الملفات المكررة الآن.
تم توثيق التكرار للمراجعة المستقبلية.
