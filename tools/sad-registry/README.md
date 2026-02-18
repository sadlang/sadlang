# بسم الله الرحمن الرحيم
# سجل حزم لغة ص — Sad Language Package Registry

<div align="center">

**📦 المستودع الرسمي لحزم لغة البرمجة العربية «ص»**

[![Node.js](https://img.shields.io/badge/Node.js-18%2B-green)](https://nodejs.org)
[![License](https://img.shields.io/badge/License-MIT-blue)](LICENSE)
[![Tests](https://img.shields.io/badge/Tests-18%2F18-brightgreen)](#الاختبارات)

</div>

---

## 📋 نظرة عامة

سجل حزم لغة ص هو خادم مستودع حزم احترافي مبني خصيصاً للغة البرمجة العربية
«ص» (Sad). يوفر نظاماً متكاملاً لنشر وتوزيع وإدارة مكتبات لغة ص، مستوحى
من أنظمة مثل **crates.io** (Rust) و **npm** (Node.js) و **PyPI** (Python).

### ✨ الميزات الرئيسية

| الميزة | الوصف |
|--------|-------|
| 🔍 **بحث ذكي** | بحث بالعربية والإنجليزية في أسماء الحزم والأوصاف والكلمات المفتاحية |
| 📦 **نشر وتوزيع** | رفع الحزم كأرشيفات `.tar.gz` مع حساب SHA-256 تلقائي |
| 🔐 **مصادقة مزدوجة** | JWT tokens + API tokens (بادئة `sad_`) لدعم CLI والويب |
| 🌐 **واجهة ويب عربية** | موقع RTL كامل بخط Noto Kufi Arabic للتصفح والبحث |
| 📊 **إحصائيات حية** | عداد تنزيلات، حزم شائعة، أحدث الإضافات |
| 🏷️ **تصنيفات عربية** | 16 تصنيف (رياضيات، تشفير، ويب، إسلامي، مالي...) |
| 🛡️ **أمان** | Helmet, CORS, Rate Limiting, bcrypt (12 جولة) |
| 📐 **Semantic Versioning** | دعم كامل لـ semver مع caret `^` و tilde `~` |
| 🐳 **Docker جاهز** | Dockerfile + docker-compose مع Nginx عكسي |
| ⚡ **أداء عالي** | SQLite WAL mode + prepared statements + gzip compression |

### 🏗️ البنية المعمارية

```
┌─────────────────────────────────────────────────────────┐
│                     العملاء / Clients                    │
├──────────────────┬──────────────────┬───────────────────┤
│   sad-pkg CLI    │   واجهة الويب    │    API مباشر     │
│  (registry_      │   (متصفح)       │   (curl/أي عميل)  │
│   client.h)      │                  │                   │
└────────┬─────────┴────────┬─────────┴─────────┬─────────┘
         │                  │                   │
         ▼                  ▼                   ▼
┌─────────────────────────────────────────────────────────┐
│              Express.js Server (server.js)               │
├─────────────────────────────────────────────────────────┤
│  Middleware:  helmet │ cors │ compression │ rate-limit   │
├──────────────────┬──────────────────┬───────────────────┤
│  Routes:         │  Services:       │  Models:          │
│  ├─ api.js       │  ├─ package-     │  └─ database.js   │
│  ├─ packages.js  │  │  service.js   │     (SQLite +     │
│  ├─ auth.js      │  ├─ user-        │      WAL mode)    │
│  ├─ user.js      │  │  service.js   │                   │
│  ├─ stats.js     │  └─ stats-       │                   │
│  └─ web.js       │     service.js   │                   │
├──────────────────┴──────────────────┴───────────────────┤
│  Views (EJS RTL):  index │ search │ package │ 404 │ err │
├─────────────────────────────────────────────────────────┤
│  Storage:  ./data/registry.db  │  ./storage/packages/   │
└─────────────────────────────────────────────────────────┘
```

---

## 🚀 البدء السريع

### المتطلبات

| المتطلب | الإصدار | ملاحظة |
|---------|---------|--------|
| **Node.js** | 18+ (يُفضل 20) | وقت التشغيل |
| **npm** | 9+ | مدير حزم Node |
| **بناء أصلي** | Python 3 + make + g++ | مطلوب لـ better-sqlite3 |

> 💡 على Windows: تأكد من وجود أدوات البناء `npm install -g windows-build-tools`
> أو ثبّت Visual Studio Build Tools مع حزمة C++ Desktop.

### التثبيت

```bash
# 1. الانتقال لمجلد المشروع
cd tools/sad-registry

# 2. تثبيت التبعيات
npm install

# 3. إعداد ملف البيئة
cp .env.example .env
# عدّل .env حسب حاجتك (خاصة JWT_SECRET في الإنتاج)

# 4. بذر قاعدة البيانات ببيانات تجريبية (اختياري)
npm run seed

# 5. تشغيل الخادم في وضع التطوير (مع إعادة تحميل تلقائي)
npm run dev
```

الخادم يعمل الآن على **http://localhost:3000** 🎉

### التحقق من العمل

```bash
# اختبار API
curl http://localhost:3000/api/v1

# بحث عن حزمة
curl http://localhost:3000/api/v1/search?q=رياضيات

# إحصائيات المستودع
curl http://localhost:3000/api/v1/stats
```

---

## 📡 واجهة API الكاملة

### نقاط النهاية العامة (بدون مصادقة)

| الطريقة | المسار | الوصف | مثال |
|---------|--------|-------|------|
| `GET` | `/api/v1` | معلومات API والنقاط المتاحة | — |
| `GET` | `/api/v1/search?q=<نص>&limit=<ن>&category=<تصنيف>` | بحث في الحزم | `?q=رياضيات&limit=10` |
| `GET` | `/api/v1/packages/:name` | معلومات حزمة كاملة + إصداراتها | `/packages/sad-math` |
| `GET` | `/api/v1/packages/:name/versions` | قائمة الإصدارات فقط | `/packages/sad-math/versions` |
| `GET` | `/api/v1/packages/:name/:version` | تفاصيل إصدار محدد + تبعيات | `/packages/sad-math/1.2.0` |
| `GET` | `/api/v1/packages/:name/:version/download` | تنزيل أرشيف الحزمة | يرجع `.tar.gz` |
| `GET` | `/api/v1/stats` | إحصائيات المستودع الشاملة | — |

### نقاط النهاية المحمية (تتطلب مصادقة)

| الطريقة | المسار | الوصف | المصادقة |
|---------|--------|-------|---------|
| `POST` | `/api/v1/auth/register` | تسجيل مستخدم جديد | — |
| `POST` | `/api/v1/auth/login` | تسجيل دخول (يرجع JWT) | — |
| `POST` | `/api/v1/auth/token` | إنشاء رمز API جديد | JWT |
| `POST` | `/api/v1/packages/publish` | نشر حزمة جديدة | JWT أو API token |
| `DELETE` | `/api/v1/packages/:name/:version/yank` | سحب إصدار | JWT أو API token |
| `GET` | `/api/v1/user/packages` | حزم المستخدم الحالي | JWT أو API token |
| `GET` | `/api/v1/user/profile` | الملف الشخصي | JWT أو API token |

### أمثلة الاستخدام

#### تسجيل مستخدم جديد
```bash
curl -X POST http://localhost:3000/api/v1/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "username": "مطور",
    "email": "dev@example.com",
    "password": "كلمة_مرور_قوية_123",
    "display_name": "المطور أحمد"
  }'

# الاستجابة:
# {
#   "success": true,
#   "token": "eyJhbGci...",        ← JWT للوصول
#   "api_token": "sad_a1b2c3..."   ← رمز API لـ sad-pkg CLI
# }
```

#### نشر حزمة
```bash
# 1. إنشاء أرشيف الحزمة
tar -czf my-package-1.0.0.tar.gz src/ sad.toml

# 2. نشر الحزمة
curl -X POST http://localhost:3000/api/v1/packages/publish \
  -H "Authorization: Bearer sad_a1b2c3..." \
  -F "package=@my-package-1.0.0.tar.gz" \
  -F 'metadata={
    "name": "حزمتي",
    "version": "1.0.0",
    "description_ar": "وصف حزمتي بالعربية",
    "description": "My package description",
    "keywords": ["أداة", "مساعد"],
    "categories": ["cli"],
    "license": "MIT",
    "dependencies": { "sad-math": "^1.0.0" }
  }'
```

#### استخدام مع sad-pkg CLI
```bash
# تسجيل الدخول (يخزن الرمز محلياً)
sad-pkg login --token sad_a1b2c3...

# بحث
sad-pkg search رياضيات

# تثبيت حزمة
sad-pkg install sad-math

# نشر مشروعك
sad-pkg publish
```

---

## 🌐 واجهة الويب

الخادم يوفر واجهة ويب عربية كاملة (RTL) على نفس المنفذ:

| الصفحة | المسار | الوصف |
|--------|--------|-------|
| الرئيسية | `/` | بحث + إحصائيات + حزم شائعة + تصنيفات |
| البحث | `/search?q=...` | بحث متقدم مع تصفية بالتصنيفات |
| تفاصيل حزمة | `/packages/:name` | معلومات كاملة + إصدارات + تبعيات + أمر تثبيت |
| خطأ 404 | — | صفحة عربية عند عدم وجود الصفحة |

---

## 📁 هيكل المشروع

```
tools/sad-registry/
├── 📄 package.json           # تبعيات Node.js وسكربتات التشغيل
├── 📄 .env.example           # قالب متغيرات البيئة
├── 📄 Dockerfile             # بناء صورة Docker
├── 📄 docker-compose.yml     # تشغيل مع Nginx عكسي
│
├── 📂 src/                   # الكود المصدري الرئيسي
│   ├── 📄 server.js          # نقطة الدخول — إعداد Express وبدء الخادم
│   │
│   ├── 📂 models/            # طبقة البيانات
│   │   └── 📄 database.js    # SQLite schema + تهيئة + prepared statements
│   │
│   ├── 📂 routes/            # مسارات HTTP (thin controllers)
│   │   ├── 📄 api.js         # مسار جذر API + البحث
│   │   ├── 📄 packages.js    # CRUD الحزم + نشر + تنزيل + سحب
│   │   ├── 📄 auth.js        # تسجيل + دخول + رموز API
│   │   ├── 📄 user.js        # ملف شخصي + حزم المستخدم
│   │   ├── 📄 stats.js       # إحصائيات المستودع
│   │   └── 📄 web.js         # صفحات واجهة الويب (EJS)
│   │
│   ├── 📂 services/          # الطبقة المنطقية (business logic)
│   │   ├── 📄 package-service.js  # منطق الحزم (بحث/جلب/نشر/تنزيل)
│   │   ├── 📄 user-service.js     # منطق المستخدمين (تسجيل/دخول/رموز)
│   │   └── 📄 stats-service.js    # منطق الإحصائيات
│   │
│   ├── 📂 middleware/        # برمجيات وسيطة Express
│   │   ├── 📄 auth.js        # JWT + API token authentication
│   │   ├── 📄 errors.js      # معالجة 404 + 500 (API vs Web)
│   │   └── 📄 rate-limit.js  # حدود الطلبات (عام/نشر/مصادقة)
│   │
│   └── 📂 utils/             # أدوات مساعدة
│       └── 📄 validators.js  # تحقق من أسماء الحزم والإصدارات
│
├── 📂 views/                 # قوالب EJS (واجهة الويب)
│   ├── 📄 index.ejs          # الصفحة الرئيسية (بطل + شائعة + تصنيفات)
│   ├── 📄 search.ejs         # صفحة البحث + تصفية تصنيفات
│   ├── 📄 package.ejs        # تفاصيل حزمة (إصدارات + تبعيات + مالكين)
│   ├── 📄 404.ejs            # صفحة 404 عربية
│   ├── 📄 error.ejs          # صفحة خطأ عام
│   └── 📂 partials/
│       ├── 📄 header.ejs     # رأس مشترك (HTML head + navbar)
│       └── 📄 footer.ejs     # ذيل مشترك (روابط + حقوق + JS)
│
├── 📂 public/                # ملفات ثابتة
│   ├── 📂 css/
│   │   └── 📄 style.css      # أنماط CSS كاملة (RTL + Dark theme)
│   └── 📂 js/
│       └── 📄 main.js        # JavaScript للواجهة (نسخ أوامر التثبيت)
│
├── 📂 scripts/
│   └── 📄 seed.js            # بذر قاعدة البيانات ببيانات تجريبية
│
├── 📂 migrations/            # تطور قاعدة البيانات
│   ├── 📄 001_init.sql       # الجداول الأساسية
│   ├── 📄 002_improvements.sql
│   └── 📄 003_revolutionary.sql
│
├── 📂 tests/
│   └── 📄 api.test.js        # 18 اختبار Jest (API + Auth + Web)
│
├── 📂 docker/
│   └── 📄 nginx.conf         # إعدادات Nginx عكسي (SSL + Cache)
│
├── 📂 data/                  # قاعدة بيانات SQLite (مُولَّدة تلقائياً)
├── 📂 storage/               # ملفات الحزم المخزنة (tar.gz)
└── 📂 logs/                  # سجلات الخادم
```

---

## 🗄️ قاعدة البيانات

### الجداول

| الجدول | الوصف | الحقول الرئيسية |
|--------|-------|----------------|
| **users** | المستخدمون والحسابات | `username`, `email`, `password_hash`, `api_token`, `is_admin` |
| **packages** | الحزم المنشورة | `name`, `description`, `description_ar`, `owner_id`, `latest_version`, `total_downloads` |
| **versions** | إصدارات كل حزمة | `version`, `major/minor/patch`, `dependencies`, `checksum_sha256`, `archive_path` |
| **package_owners** | ملكية مشتركة للحزم | `package_id`, `user_id`, `role` (owner/maintainer) |
| **api_tokens** | رموز API متقدمة | `token_hash`, `scopes`, `expires_at` |
| **download_log** | سجل التنزيلات | `version_id`, `ip_address`, `user_agent`, `downloaded_at` |
| **categories** | تصنيفات الحزم | `slug`, `name_en`, `name_ar`, `icon` |

### التصنيفات المتضمنة

| الرمز | التصنيف | الوصف |
|-------|---------|-------|
| 🔢 | رياضيات (math) | حسابات رياضية وعددية |
| 📝 | معالجة نصوص (text) | معالجة النصوص والسلاسل |
| 🌐 | ويب (web) | أطر ويب وHTTP |
| 🔌 | شبكات (network) | بروتوكولات وأدوات شبكات |
| 🗃️ | قواعد بيانات (database) | محركات قواعد بيانات |
| 🔐 | تشفير (crypto) | تشفير وأمان |
| 🎨 | واجهات (ui) | أطر واجهات المستخدم |
| 🎮 | ألعاب (game) | محركات وأدوات ألعاب |
| 📊 | هياكل بيانات (data) | مجموعات وهياكل بيانات |
| 📁 | ملفات (io) | نظام ملفات ومدخلات/مخرجات |
| 📅 | تاريخ ووقت (date) | تقاويم (هجري وميلادي) |
| 🌍 | تدويل (i18n) | تدويل ودعم النص العربي |
| 🕌 | إسلامي (islamic) | صلاة، قرآن، تقويم هجري |
| 💰 | مالي (finance) | أدوات مالية وعملات |
| 🧪 | اختبار (testing) | أطر وأدوات اختبار |
| ⌨️ | سطر أوامر (cli) | أدوات سطر الأوامر |

---

## 🐳 النشر بـ Docker

### بناء وتشغيل سريع

```bash
# بناء الصورة
docker build -t sad-registry .

# تشغيل
docker run -d \
  --name sad-registry \
  -p 3000:3000 \
  -v sad-data:/app/data \
  -v sad-packages:/app/storage/packages \
  -e JWT_SECRET="سر-قوي-للإنتاج-$(openssl rand -hex 32)" \
  -e NODE_ENV=production \
  sad-registry
```

### مع Docker Compose (يشمل Nginx)

```bash
# تشغيل الخادم فقط
docker compose up -d

# تشغيل مع Nginx عكسي (HTTPS)
docker compose --profile with-nginx up -d
```

### متغيرات البيئة المهمة

| المتغير | الافتراضي | الوصف |
|---------|----------|-------|
| `PORT` | `3000` | منفذ الخادم |
| `NODE_ENV` | `development` | بيئة التشغيل |
| `DB_PATH` | `./data/registry.db` | مسار قاعدة البيانات |
| `STORAGE_PATH` | `./storage/packages` | مسار تخزين الأرشيفات |
| `JWT_SECRET` | ⚠️ غيّره في الإنتاج | سر توقيع JWT |
| `JWT_EXPIRY` | `30d` | مدة صلاحية الرمز |
| `BCRYPT_ROUNDS` | `12` | جولات تشفير كلمة المرور |
| `RATE_LIMIT_MAX_REQUESTS` | `100` | الحد الأقصى للطلبات/15 دقيقة |

---

## 🧪 الاختبارات

```bash
# تشغيل كل الاختبارات (18 اختبار)
npm test

# تشغيل مع تغطية الكود
npm test -- --coverage

# مراقبة مستمرة أثناء التطوير
npm run test:watch
```

### تغطية الاختبارات

| المجموعة | الاختبارات | الحالة |
|----------|-----------|--------|
| معلومات API | 1 | ✅ |
| تسجيل مستخدم | 3 | ✅ |
| تسجيل دخول | 2 | ✅ |
| بحث | 2 | ✅ |
| جلب حزمة | 2 | ✅ |
| حزم المستخدم | 3 | ✅ |
| نشر حزمة | 2 | ✅ |
| إحصائيات | 1 | ✅ |
| واجهة ويب | 2 | ✅ |
| **المجموع** | **18** | **✅ 100%** |

---

## 🔐 الأمان

| الطبقة | التقنية | الوصف |
|--------|---------|-------|
| **النقل** | HTTPS (Nginx + Let's Encrypt) | تشفير كل الاتصالات |
| **الرؤوس** | Helmet.js | CSP, XSS, Clickjacking protection |
| **المصادقة** | JWT + bcrypt | رموز موقعة + كلمات مرور مشفرة |
| **رموز API** | SHA-256 hashed | الرموز المتقدمة تُخزن كـ hash |
| **الحد** | express-rate-limit | 100 طلب/15 دقيقة (عام)، 10 نشر/ساعة |
| **الإدخال** | express-validator | تحقق من كل المدخلات |
| **الملفات** | multer + نوع + حجم | 50MB حد أقصى، أنواع `.tar.gz` فقط |
| **السلامة** | SHA-256 checksum | تحقق من سلامة كل حزمة منشورة |

---

## 🔄 التوافق مع sad-pkg CLI

هذا الخادم مصمم ليكون متوافقاً تماماً مع عميل `registry_client.h`
في أداة `sad-pkg`. العقد (API contract):

```
GET  /api/v1/search?q=<query>&limit=<n>           → {packages: [...]}
GET  /api/v1/packages/:name                        → {name, description, versions, ...}
GET  /api/v1/packages/:name/versions               → {versions: ["1.0.0", ...]}
GET  /api/v1/packages/:name/:version               → {dependencies, checksum, ...}
GET  /api/v1/packages/:name/:version/download      → binary (tar.gz)
POST /api/v1/packages/publish                      → multipart (file + metadata JSON)
POST /api/v1/auth/register                         → {token, api_token}
POST /api/v1/auth/login                            → {token, api_token}
GET  /api/v1/user/packages                         → {packages: [...]}
```

---

## 📈 خطة التطوير

### ✅ مكتمل (الإصدار 1.0)
- [x] خادم API كامل (بحث، نشر، تنزيل، مصادقة)
- [x] واجهة ويب عربية RTL
- [x] نظام تصنيفات عربي
- [x] Docker + Nginx
- [x] 18 اختبار ناجح
- [x] بذر بيانات تجريبية (10+ حزم)

### 🔜 قادم (الإصدار 1.1)
- [ ] إحصائيات تنزيلات يومية (مخطط بياني)
- [ ] ملف قفل `sad.lock` 
- [ ] تحقق من التوقيعات الرقمية
- [ ] webhook إشعارات عند النشر
- [ ] صفحة ملف شخصي للمطور
- [ ] RSS feed للحزم الجديدة

### 🔮 مستقبلي (الإصدار 2.0)
- [ ] مرآة CDN (Cloudflare R2)
- [ ] بحث نص كامل (FTS5)
- [ ] نظام تقييم ومراجعات
- [ ] فحص أمني تلقائي للحزم
- [ ] اتحاد مستودعات (Federation)
- [ ] تكامل مع GitHub Actions

---

## 📜 الترخيص

MIT — مفتوح المصدر للجميع.

## 🤝 المساهمة

نرحب بمساهمات المجتمع! افتح Issue أو Pull Request على GitHub.

---

<div align="center">

**صُنع بـ ❤️ للمبرمجين العرب — فريق لغة ص**

</div>

MIT
