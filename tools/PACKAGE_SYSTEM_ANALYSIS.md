# تقرير تحليل نظام إدارة الحزم الشامل
# Comprehensive Package System Analysis Report

> **التاريخ:** التحليل يغطي جميع الملفات في `tools/pkg/` و `tools/sad-registry/`
> **النطاق:** عميل C++ + خادم Node.js + واجهة ويب

---

## ✅ الإصلاحات المنجزة

| # | الإصلاح | الملف | التفاصيل |
|---|---------|-------|----------|
| 1 | مسار النشر | `registry_client_v2.h` | `POST /api/v1/packages` → `POST /api/v1/packages/publish` |
| 2 | طريقة السحب | `registry_client_v2.h` | POST → DELETE مع fallback لـ POST |
| 3 | تحليل الإصدارات | `registry_client_v2.h` | يدعم صيغة المصفوفة `["1.0"]` والكائنات `[{version:"1.0"}]` |
| 4 | Checksum | `registry_client_v2.h` | من API معلومات الإصدار بدلاً من endpoint غير موجود |
| 5 | دعم DELETE | `http_client.h` | إضافة `request()` تدعم أي HTTP method |
| 6 | أمان JWT | `user-service.js` + `auth.js` | يرفض التشغيل بدون `JWT_SECRET` في الإنتاج |
| 7 | أمان CORS | `server.js` | مقيد في الإنتاج عبر `CORS_ORIGINS` |
| 8 | نموذج الدخول | `index.html` + `app.js` | إرسال `{username, password}` بدلاً من `{email, password}` |
| 9 | تصنيفات الواجهة | `index.html` | استخدام slugs إنجليزية (`network`, `math`...) تطابق الخادم |
| 10 | أمان api_token | `user-service.js` + `auth.js` | تخزين SHA-256 hash بدلاً من نص عادي |
| 11 | URL المستودع | `package_search.h` + `package_manager.cpp` | توحيد إلى `http://185.47.174.39:3000` |
| 12 | حذف v1 | `cli.cpp` + `registry_client.h` | ملفات محذوفة |
| 13 | حذف v3 | `src/registry_client.cpp` + `src/registry_client.hpp` | ملفات محذوفة |
| 14 | تنظيف كود ميت | `tools/pkg/src/` | حذف المجلد بالكامل (غير مبني في CMake) |

---

## الجزء الأول: تحليل عميل C++ (tools/pkg/)

### 1.1 نسختان من العميل

| الملف | النسخة | المكتبات | Registry URL الافتراضي |
|-------|--------|----------|----------------------|
| `registry_client.h` | v1 | libcurl + nlohmann/json | `http://185.47.174.39:3000` |
| `registry_client_v2.h` | v2 | WinHTTP (بدون تبعيات خارجية) | `http://185.47.174.39:3000` |
| `src/registry_client.cpp` | v3? | libcurl + nlohmann/json | `https://packages.sad-lang.org` |
| `include/package_search.h` | — | — | `https://registry.sad-pkg.org` |
| `src/package_manager.cpp` | — | — | `https://packages.sad-lang.org` |

**⛔ مشكلة حرجة #1:** ثلاثة عناوين مختلفة للمستودع في الكود!

### 1.2 أوامر CLI

#### v1 (`cli.cpp` ~620 سطر):
`init` · `install` · `add` · `remove` · `search` · `info` · `list` · `clean` · `publish` · `help`

#### v2 (`cli_v2.cpp` ~1900+ سطر):
كل أوامر v1 + `update` · `outdated` · `audit` · `verify` · `versions` · `yank` · `stats` · `register` · `login` · `logout` · `whoami` · `my-packages` · `token` · `config` · `doctor` · `--version`

**أسماء عربية مدعومة في v2:**
`أنشئ` · `ثبّت` · `أضف` · `أزل` · `حدّث` · `انشر` · `ابحث` · `معلومات` · `سجّل` · `دخول` · `اخرج` · `من_أنا` · `حزمي` · `مفتاح` · `إصدارات` · `سحب` · `إحصائيات` · `إعدادات` · `تدقيق` · `قديمة` · `تحقق` · `طبيب` · `قائمة` · `نظّف` · `مساعدة`

### 1.3 نقاط API المُستدعاة من العميل

#### v1 (registry_client.h):
| العملية | المسار المُستخدم | الصحيح على الخادم |
|---------|------------------|-------------------|
| بحث | `GET /api/v1/search?q=&limit=` | `/api/v1/packages/search?q=` |
| إصدارات | `GET /api/v1/packages/:name/versions` | ✓ (لكن الصيغة مختلفة) |
| بيانات | `GET /api/v1/packages/:name/:version` | ✓ |
| وجود | `GET /api/v1/packages/:name` | ✓ |
| تنزيل | `GET /api/v1/packages/:name/:version/download` | ✓ |
| حزم المستخدم | `GET /api/v1/user/packages` | ✓ |
| **نشر** | **Firebase Storage + Firestore** | **POST /api/v1/packages/publish** |

#### v2 (registry_client_v2.h):
| العملية | المسار المُستخدم | الصحيح على الخادم |
|---------|------------------|-------------------|
| بحث | `GET /api/v1/packages/search?q=&limit=` | ✓ |
| إصدارات | `GET /api/v1/packages/:name/versions` | ✓ (لكن الصيغة مختلفة) |
| تنزيل | `GET /api/v1/packages/:name/:version/download` | ✓ |
| checksum | `GET /api/v1/packages/:name/:version/checksum` | ❌ غير موجود! |
| **نشر** | **POST /api/v1/packages** (upload_file) | **POST /api/v1/packages/publish** |
| **سحب** | **POST /.../yank** | **DELETE /.../yank** |
| تسجيل دخول | `POST /api/v1/auth/login` | ✓ |
| تسجيل | `POST /api/v1/auth/register` | ✓ |
| ملف شخصي | `GET /api/v1/user/profile` | ✓ |
| حزمي | `GET /api/v1/user/packages` | ✓ |
| إنشاء رمز | `POST /api/v1/auth/token` | ✓ |
| إحصائيات | `GET /api/v1/stats` | ✓ |

#### src/registry_client.cpp (v3):
| العملية | المسار المُستخدم | الصحيح |
|---------|------------------|--------|
| بحث | `GET /api/v1/search?q=` | ❌ `/api/v1/packages/search` |
| تنزيل | `GET /api/v1/download/:name/:ver` | ❌ `/api/v1/packages/:name/:ver/download` |
| نشر | `POST /api/v1/packages` | ❌ `/api/v1/packages/publish` |
| تسجيل دخول | `POST /api/v1/login` | ❌ `/api/v1/auth/login` |

### 1.4 صيغة ملف الإعدادات (TOML)

| القسم (إنجليزي) | القسم (عربي) | الملف |
|-----------------|-------------|-------|
| `[package]` | `[حزمة]` / `[الحزمة]` | sad.toml / ص.toml |
| `[dependencies]` | `[تبعيات]` / `[المتطلبات]` | — |
| `[dev-dependencies]` | `[تبعيات_تطوير]` / `[التطوير]` | — |
| `[build-dependencies]` | — | — |
| `[build]` | `[البناء]` | — |

**⛔ مشكلة #2:** أسماء الأقسام العربية غير متسقة بين الملفات!
- `package_manager.cpp` يستخدم `[حزمة]` و `[تبعيات]`
- `arabic_toml_parser.cpp` يستخدم `[الحزمة]` و `[المتطلبات]`

---

## الجزء الثاني: تحليل خادم Node.js (tools/sad-registry/)

### 2.1 البنية
```
Express.js v4 + better-sqlite3 + JWT + bcrypt + multer
Node.js >= 18 (--experimental-sqlite)
المنفذ: 3000
```

### 2.2 مخطط قاعدة البيانات (SQLite)

| الجدول | الحقول الرئيسية |
|--------|----------------|
| `users` | id, username, email, password_hash, display_name, api_token, is_active, is_admin |
| `packages` | id, name, description, description_ar, owner_id, latest_version, total_downloads, is_yanked |
| `versions` | id, package_id, version, major/minor/patch, dependencies (JSON), checksum_sha256, archive_path, archive_size, is_yanked |
| `package_owners` | package_id, user_id, role |
| `api_tokens` | user_id, name, token_hash, scopes (JSON), expires_at |
| `download_log` | version_id, ip_address, user_agent, downloaded_at |
| `categories` | slug, name_ar, name_en, icon (16 تصنيف مُبذَر) |

### 2.3 نقاط API الكاملة

#### المصادقة (`/api/v1/auth/`):
| الطريقة | المسار | الوصف | المصادقة |
|---------|--------|-------|----------|
| POST | `/register` | تسجيل حساب | لا |
| POST | `/login` | دخول (username/email + password → JWT + api_token) | لا |
| POST | `/token` | إنشاء رمز API | JWT/api_token |

#### الحزم (`/api/v1/packages/`):
| الطريقة | المسار | الوصف | المصادقة |
|---------|--------|-------|----------|
| GET | `/search?q=&limit=&offset=&category=` | بحث | لا |
| GET | `/suggest?q=&limit=` | اقتراحات | لا |
| GET | `/popular?limit=` | الأكثر تنزيلاً | لا |
| GET | `/recent?limit=` | أحدث الحزم | لا |
| GET | `/featured?limit=` | مميزة (= popular) | لا |
| GET | `/:name` | معلومات حزمة | لا |
| GET | `/:name/versions` | قائمة إصدارات | لا |
| GET | `/:name/:version` | معلومات إصدار | لا |
| GET | `/:name/:version/download` | تنزيل `.tar.gz` | لا |
| POST | `/publish` | نشر (multipart: `package` + `metadata`) | JWT/api_token |
| DELETE | `/:name/:version/yank` | سحب إصدار | JWT/api_token |

#### المستخدم (`/api/v1/user/`):
| الطريقة | المسار | الوصف |
|---------|--------|-------|
| GET | `/profile` | الملف الشخصي |
| GET | `/packages` | حزم المستخدم |

#### الإحصائيات (`/api/v1/stats/`):
| GET | `/` | إحصائيات شاملة |
| GET | `/downloads?days=` | تنزيلات يومية |
| GET | `/categories` | التصنيفات |

#### عام:
| GET | `/api/v1/` | معلومات API |
| GET | `/api/v1/health` | فحص صحة |
| GET | `/api/v1/errors` | قائمة رموز الأخطاء |

### 2.4 نظام المصادقة
1. **JWT (Bearer):** `Authorization: Bearer <jwt_token>` — صلاحية 30 يوم
2. **API Token:** `Authorization: sad_xxxx` — يُبحث أولاً في `api_tokens` (بالهاش)، ثم في `users.api_token` (نص صريح)

### 2.5 تقييد الطلبات (Rate Limiting)
- عام: 200 طلب / 15 دقيقة
- نشر: 10 طلبات / ساعة
- مصادقة: 20 محاولة / 15 دقيقة

### 2.6 نظام الأخطاء
28 رمز خطأ مُعرّف مع رسائل عربية وإنجليزية + شرح + حل مقترح:
`AUTH_001-005` · `REG_001-005` · `PKG_001-008` · `PUB_001-004` · `VAL_001-002` · `RATE_001-003` · `GEN_001-004` · `TKN_001-002` · `SRC_001`

---

## الجزء الثالث: التعارضات الحرجة بين العميل والخادم

### ⛔ تعارض 1: مسار البحث
| المكون | المسار |
|--------|--------|
| الخادم | `GET /api/v1/packages/search?q=` |
| v1 client | `GET /api/v1/search?q=` ← **خطأ 404** |
| v2 client | `GET /api/v1/packages/search?q=` ← ✓ |
| src/registry_client.cpp | `GET /api/v1/search?q=` ← **خطأ 404** |

### ⛔ تعارض 2: صيغة استجابة الإصدارات
| المكون | الصيغة المتوقعة |
|--------|----------------|
| الخادم يُرسل | `{success: true, name: "x", versions: [{version: "1.0.0", downloads: 5, ...}]}` |
| v1 يتوقع | `{versions: ["1.0.0", "1.1.0"]}` (مصفوفة نصوص) |
| v2 يتوقع | `{versions: ["1.0.0"]}` أو `{versions: [{version}]}` حسب الـ parser |

العميل v1 سيفشل لأنه يتوقع مصفوفة نصوص لا مصفوفة كائنات.
العميل v2 **قد** ينجح إذا كان SimpleJson يقرأ `v.str_val` من كائن — لكن هذا غير مؤكد.

### ⛔ تعارض 3: مسار النشر
| المكون | المسار |
|--------|--------|
| الخادم | `POST /api/v1/packages/publish` (multipart: field=`package`) |
| v1 | Firebase Storage (لا يستخدم الخادم أصلاً!) |
| v2 | `POST /api/v1/packages` (upload_file) ← **خطأ 404** |
| src/registry_client.cpp | `POST /api/v1/packages` ← **خطأ 404** |

### ⛔ تعارض 4: مسار التنزيل
| المكون | المسار |
|--------|--------|
| الخادم | `GET /api/v1/packages/:name/:version/download` |
| v1 | `GET /api/v1/packages/:name/:version/download` ← ✓ |
| v2 | `GET /api/v1/packages/:name/:version/download` ← ✓ |
| src/registry_client.cpp | `GET /api/v1/download/:name/:ver` ← **خطأ 404** |

### ⛔ تعارض 5: Checksum
| المكون | الآلية |
|--------|--------|
| الخادم | يُرسل `X-Checksum-SHA256` كترويسة HTTP مع ملف التنزيل |
| v2 | يطلب `GET /api/v1/packages/:name/:version/checksum` ← **نقطة غير موجودة!** |

### ⛔ تعارض 6: سحب الإصدار (Yank)
| المكون | الطريقة |
|--------|---------|
| الخادم | `DELETE /api/v1/packages/:name/:version/yank` |
| v2 | `POST /api/v1/packages/:name/:version/yank` |

### ⛔ تعارض 7: مسار تسجيل الدخول
| المكون | المسار |
|--------|--------|
| الخادم | `POST /api/v1/auth/login` |
| v2 | `POST /api/v1/auth/login` ← ✓ |
| src/registry_client.cpp | `POST /api/v1/login` ← **خطأ 404** |

### ⛔ تعارض 8: صيغة الاستجابة العامة
الخادم يُغلّف كل شيء بـ `{success: true/false, ...}`. بعض عملاء C++ لا يتحققون من حقل `success`.

---

## الجزء الرابع: مشاكل واجهة الويب

### 4.1 نظامان للعرض
- **EJS Templates** في `views/` (يُقدَّم من مسارات `web.js`): `/`, `/search`, `/packages/:name`
- **HTML ثابت** في `web/`: `index.html`, `search.html`, `package.html`, `stats.html`, `admin.html`, إلخ
- النظامان **يتداخلان** ولا أحد يعرف أيهما المُستخدم فعلاً

### 4.2 تسجيل الدخول
نموذج تسجيل الدخول في HTML يُرسل `email` + `password` لكن الخادم يتوقع `username` + `password`. حقل `authUsername` مخفي افتراضياً (`style="display:none"`).

### 4.3 فلاتر البحث بدون تنفيذ
أزرار الفلترة (الأكثر صلة، الأكثر تنزيلاً، الأحدث، أبجدي) في `search.html` ليس لها أي حدث JavaScript.

### 4.4 التقييمات/المراجعات بدون خادم
صفحة الحزمة تعرض تبويب "التقييمات" مع نموذج إرسال تقييم ونجوم — لكن **لا يوجد أي مسار API أو خدمة للتقييمات على الخادم**.

### 4.5 التصنيفات
روابط التصنيفات في `index.html` تستخدم أسماء عربية (`category=شبكات`) لكن الخادم يخزّن التصنيفات بـ `slug` إنجليزي (`web`, `network`, `math`).

### 4.6 صفحات بدون تنفيذ
- `admin.html` — لوحة الإدارة (لا يوجد API للإدارة)
- `notifications.html` — الإشعارات (لا يوجد نظام إشعارات)
- `organizations.html` — المنظمات (لا يوجد دعم)
- `security.html` — الأمان (لا يوجد تنفيذ)

---

## الجزء الخامس: قائمة جميع المشاكل والتحسينات المرقّمة

### 🔴 مشاكل حرجة (تمنع العمل)

1. **مسار البحث في v1 خاطئ:** `/api/v1/search` بدلاً من `/api/v1/packages/search` — البحث لا يعمل
2. **مسار النشر خاطئ في v2:** `POST /api/v1/packages` بدلاً من `/api/v1/packages/publish` — النشر لا يعمل
3. **v1 ينشر عبر Firebase:** لا يستخدم خادم المستودع أصلاً — النشر لا يعمل مع الخادم الحالي
4. **صيغة الإصدارات مختلفة:** v1 يتوقع مصفوفة نصوص، الخادم يُرسل مصفوفة كائنات — التثبيت لا يعمل في v1
5. **نقطة checksum غير موجودة:** v2 يطلب `/checksum` لكن الخادم يُرسل الـ checksum كترويسة
6. **طريقة السحب مختلفة:** v2 يستخدم POST، الخادم يتوقع DELETE
7. **src/registry_client.cpp مسارات خاطئة بالكامل:** 4 مسارات من 6 خاطئة
8. **JWT_SECRET مشفّر في الكود:** `'sad-registry-default-secret-change-me'` — أي شخص يقرأ الكود يمكنه تزوير رموز
9. **ثلاثة عناوين مستودع مختلفة:** لا يوجد مصدر حقيقة واحد

### 🟠 مشاكل أمنية

10. **CORS مفتوح للجميع:** `origin: '*'` يسمح لأي موقع بالوصول
11. **api_token مخزّن كنص صريح** في جدول `users` — تسريب قاعدة البيانات يكشف كل الرموز
12. **بيانات الاعتماد محفوظة كنص JSON عادي** في `credentials.json` محلياً
13. **لا يوجد تحقق من HTTPS:** عميل v1/v2 يقبل HTTP عادي
14. **لا يوجد تشفير لملفات الحزم:** الملفات تُخزن كما هي في `storage/packages/`
15. **Helmet مع CSP معطل:** `contentSecurityPolicy: false`
16. **كلمة المرور 8 أحرف فقط:** بدون متطلبات تعقيد (أرقام/رموز)

### 🟡 مشاكل بنيوية

17. **ثلاث نسخ من RegistryClient:** `registry_client.h`, `registry_client_v2.h`, `src/registry_client.cpp` — تكرار هائل
18. **ثلاث نسخ من SemanticVersion:** في `package.h`, `src/package_manager.cpp`, `src/semver_arabic.cpp`
19. **ثلاث نسخ من DependencyResolver:** في `dependency_resolver.h`, `src/dependency_resolver.cpp`, `src/dependency_resolver.hpp`
20. **ثلاث نسخ من TOML Parser:** في `toml_parser.h`, `src/arabic_toml_parser.cpp`, `src/toml_schema.cpp`
21. **أسماء الأقسام العربية غير متسقة:** `[حزمة]` vs `[الحزمة]`, `[تبعيات]` vs `[المتطلبات]`
22. **نظامان للعرض على الويب:** EJS + HTML ثابت يتداخلان
23. **cli.cpp و cli_v2.cpp ملفان منفصلان:** لا إعادة استخدام للكود بينهما
24. **package_search.h يحتوي كيانات HTML:** `&lt;` بدلاً من `<` (ملف تالف/مُشفّر)
25. **مجلد src/ خارج التصميم الرئيسي:** الملفات في `tools/pkg/src/` لا تُستخدم من cli.cpp/cli_v2.cpp

### 🔵 ميزات ناقصة

26. **لا يوجد نظام تقييمات/مراجعات:** الواجهة تعرضه لكن لا يوجد API
27. **لا يوجد نظام إشعارات:** صفحة notifications.html بدون تنفيذ
28. **لا يوجد دعم منظمات:** صفحة organizations.html بدون تنفيذ
29. **لا يوجد لوحة إدارة:** صفحة admin.html بدون API
30. **لا يوجد نظام أمان:** صفحة security.html بدون تنفيذ
31. **لا يوجد استخراج tar.gz في v1:** TODO في cli.cpp
32. **PubGrub غير مُنفّذ:** TODO في dependency_resolver.cpp
33. **فلاتر البحث بدون تنفيذ:** أزرار الفرز في search.html لا تعمل
34. **لا يوجد بحث autocomplete حقيقي:** `/suggest` موجود في الخادم لكن الواجهة لا تستخدمه
35. **لا يوجد تحديث الحزم (update):** الأمر موجود في v2 CLI لكن التنفيذ ناقص
36. **لا يوجد تدقيق أمني (audit):** الأمر موجود في v2 CLI لكن بدون مصدر بيانات
37. **Deflate decompression غير مُنفّذ في ZipExtractor:** TODO في registry_client_v2.h (يكتب المضغوط كما هو)
38. **ChecksumCalculator في lockfile.cpp يستخدم std::hash (محاكاة):** وليس SHA256 حقيقي

### 🟣 تحسينات مقترحة

39. **توحيد عنوان المستودع:** ملف إعدادات مركزي واحد
40. **توحيد RegistryClient:** نسخة واحدة بواجهة IPackageRegistry
41. **توحيد SemanticVersion:** استخدام semver_arabic.cpp كمصدر وحيد
42. **توحيد TOML Parser:** دمج arabic_toml_parser.cpp مع toml_parser.h
43. **توحيد أسماء الأقسام العربية:** قاموس واحد مشترك
44. **إزالة src/registry_client.cpp:** مسارات كلها خاطئة، لا يُستخدم
45. **إزالة Firebase من v1:** استبداله بالنشر عبر HTTP
46. **إضافة نقطة checksum للخادم:** أو قراءة الترويسة `X-Checksum-SHA256` في v2
47. **توحيد طريقة السحب:** استخدام DELETE على كلا الجانبين
48. **إضافة تحقق HTTPS:** رفض HTTP في الإنتاج
49. **تشفير api_token في قاعدة البيانات:** تخزين الهاش فقط
50. **تعيين JWT_SECRET من متغير بيئي:** مع إيقاف التشغيل إذا لم يُعيَّن
51. **تقييد CORS:** تحديد النطاقات المسموحة
52. **إصلاح تسجيل الدخول في الواجهة:** إرسال username بدلاً من email
53. **إصلاح روابط التصنيفات:** استخدام slug إنجليزي أو إضافة دعم عربي
54. **حذف صفحات بدون تنفيذ:** أو إضافة تنفيذ لها
55. **إضافة Swagger/OpenAPI:** توثيق API تلقائي
56. **إضافة اختبارات تكامل:** بين العميل والخادم
57. **إضافة CI/CD:** بناء وتشغيل الاختبارات تلقائياً
58. **إضافة pagination لحزم المستخدم:** الخادم يُرجع كل الحزم دفعة واحدة
59. **إضافة دعم .sadignore:** لاستثناء ملفات من النشر
60. **إضافة نظام webhook:** لإشعار الخدمات الخارجية عند النشر
61. **إصلاح ZipExtractor:** إضافة zlib decompression للملفات المضغوطة بـ Deflate
62. **استخدام OpenSSL SHA256 في lockfile.cpp:** بدلاً من std::hash

---

## الجزء السادس: ملخص الأولويات

### المستوى 1 — إصلاح فوري (يمنع الاستخدام):
- إصلاح مسارات API في العميل (#1, #2, #5, #6, #7)
- إصلاح صيغة استجابة الإصدارات (#4)
- إزالة Firebase من v1 (#3)
- إصلاح JWT_SECRET (#8)

### المستوى 2 — توحيد البنية:
- توحيد عنوان المستودع (#9, #39)
- توحيد العميل (#17, #40)
- توحيد SemanticVersion (#18, #41)
- حذف الكود المكرر (#25, #44)

### المستوى 3 — أمان:
- إصلاح CORS (#10, #51)
- تشفير الرموز (#11, #49)
- HTTPS فقط (#13, #48)
- CSP (#15)

### المستوى 4 — ميزات:
- واجهة ويب مكتملة (#26-34)
- PubGrub resolver (#32)
- أدوات التدقيق (#36)

---

## ملحق: خريطة الملفات

```
tools/pkg/
├── cli.cpp                    # CLI v1 (~620 سطر)
├── cli_v2.cpp                 # CLI v2 (~1900+ سطر)
├── registry_client.h          # v1: libcurl + nlohmann/json
├── registry_client_v2.h       # v2: WinHTTP + SimpleJson + SHA256 + ZIP
├── http_client.h              # WinHTTP wrapper
├── package.h                  # Package, Version, Dependency
├── dependency_resolver.h      # IPackageRegistry, DependencyResolver (header)
├── toml_parser.h              # TOML parser (header)
├── include/
│   └── package_search.h       # Search engine interfaces (HTML-encoded!)
└── src/
    ├── registry_client.cpp    # v3 RegistryClient (مسارات خاطئة)
    ├── registry_client.hpp
    ├── package_manager.cpp    # PackageManager + Arabic TOML
    ├── package_manager.hpp
    ├── dependency_resolver.cpp # Another DependencyResolver
    ├── dependency_resolver.hpp
    ├── semver_arabic.cpp      # SemVer with Arabic + extern C
    ├── arabic_toml_parser.cpp # Arabic TOML parser
    ├── toml_schema.cpp        # TOML schema validation
    └── lockfile.cpp           # Lockfile manager (ص.قفل)

tools/sad-registry/
├── package.json               # Express + better-sqlite3 + JWT
├── src/
│   ├── server.js              # Main server (Express)
│   ├── routes/
│   │   ├── api.js             # Router hub + /health + /errors
│   │   ├── packages.js        # CRUD + publish + download + yank
│   │   ├── auth.js            # register + login + token
│   │   ├── user.js            # profile + packages
│   │   ├── stats.js           # statistics
│   │   └── web.js             # EJS pages (/, /search, /packages/:name)
│   ├── services/
│   │   ├── package-service.js # Search, info, publish, download, yank
│   │   ├── user-service.js    # Register, login, profile, tokens
│   │   └── stats-service.js   # Summary, popular, newest, categories
│   ├── models/
│   │   └── database.js        # SQLite schema (7 tables)
│   ├── middleware/
│   │   ├── auth.js            # JWT + API token verification
│   │   ├── errors.js          # 404, JSON, upload, global handlers
│   │   └── rate-limit.js      # General, publish, auth limiters
│   └── utils/
│       ├── error-codes.js     # 28 error codes (AR+EN)
│       └── validators.js      # Name, version, username, email, password
└── web/
    ├── app.js                 # Frontend JS (API calls, search, render)
    ├── style.css              # CSS
    ├── index.html             # Homepage
    ├── search.html            # Search page
    ├── package.html           # Package details
    ├── stats.html             # Statistics (unimplemented)
    ├── admin.html             # Admin panel (unimplemented)
    ├── notifications.html     # Notifications (unimplemented)
    ├── organizations.html     # Organizations (unimplemented)
    └── security.html          # Security (unimplemented)
```
