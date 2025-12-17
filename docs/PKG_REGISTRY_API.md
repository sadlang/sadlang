# بسم الله الرحمن الرحيم

# Sad Language Package Registry API
# واجهة برمجة سجل حزم لغة ص

**Version / الإصدار:** 1.0.0  
**Last Updated / آخر تحديث:** December 11, 2025

## Overview / نظرة عامة

This document describes the REST API specification for the Sad Language Package Registry. The registry serves as a central repository for Sad Language packages, allowing developers to publish, discover, and install packages.

توضح هذه الوثيقة مواصفات واجهة REST API لسجل حزم لغة ص. يعمل السجل كمستودع مركزي لحزم لغة ص، مما يسمح للمطورين بنشر الحزم واكتشافها وتثبيتها.

## Base URL / عنوان الأساس

```
https://packages.sadlang.org/api/v1
```

## Authentication / المصادقة

Most endpoints require authentication using Bearer tokens:

معظم النقاط تتطلب مصادقة باستخدام رموز Bearer:

```http
Authorization: Bearer YOUR_API_TOKEN
```

## Endpoints / نقاط النهاية

### 1. Search Packages / البحث عن الحزم

البحث عن الحزم حسب الاسم أو الوصف

```http
GET /search?q={query}&limit={limit}
```

**Parameters / المعاملات:**
- `q` (required): Search query / استعلام البحث
- `limit` (optional): Maximum results (default: 10) / الحد الأقصى للنتائج

**Response / الاستجابة:**

```json
{
  "packages": [
    {
      "name": "http",
      "version": "1.2.0",
      "description": "HTTP client library",
      "description_ar": "مكتبة عميل HTTP",
      "downloads": 1523,
      "updated_at": "2025-12-10T10:30:00Z"
    }
  ],
  "total": 1,
  "page": 1
}
```

**Status Codes:**
- `200 OK`: Search successful
- `400 Bad Request`: Invalid query parameters
- `500 Internal Server Error`: Server error

---

### 2. Get Package Information / الحصول على معلومات الحزمة

الحصول على معلومات مفصلة عن حزمة معينة

```http
GET /packages/{package_name}
```

**Response / الاستجابة:**

```json
{
  "name": "http",
  "description": "HTTP client library for Sad Language",
  "description_ar": "مكتبة عميل HTTP للغة ص",
  "repository": "https://github.com/sad-lang/http",
  "license": "MIT",
  "authors": [
    "أحمد محمد <ahmad@example.com>"
  ],
  "versions": ["1.0.0", "1.1.0", "1.2.0"],
  "latest_version": "1.2.0",
  "downloads": 1523,
  "created_at": "2025-01-15T08:00:00Z",
  "updated_at": "2025-12-10T10:30:00Z"
}
```

**Status Codes:**
- `200 OK`: Package found
- `404 Not Found`: Package does not exist

---

### 3. Get Package Versions / الحصول على إصدارات الحزمة

الحصول على قائمة بجميع الإصدارات المتاحة للحزمة

```http
GET /packages/{package_name}/versions
```

**Response / الاستجابة:**

```json
{
  "name": "http",
  "versions": [
    "1.2.0",
    "1.1.0",
    "1.0.0",
    "0.9.0-beta"
  ]
}
```

**Status Codes:**
- `200 OK`: Versions retrieved
- `404 Not Found`: Package does not exist

---

### 4. Get Package Metadata / الحصول على بيانات الحزمة التفصيلية

الحصول على البيانات الكاملة لإصدار معين من الحزمة

```http
GET /packages/{package_name}/{version}
```

**Response / الاستجابة:**

```json
{
  "name": "http",
  "version": "1.2.0",
  "description": "HTTP client library",
  "description_ar": "مكتبة عميل HTTP",
  "authors": [
    "أحمد محمد <ahmad@example.com>"
  ],
  "license": "MIT",
  "repository": "https://github.com/sad-lang/http",
  "homepage": "https://sadlang.org/packages/http",
  "dependencies": {
    "json": "^2.1.0",
    "crypto": "~1.0.5"
  },
  "keywords": ["http", "client", "rest", "api"],
  "checksum": "sha256:a3f2c8b1d9e7f6a4c5b2d1e9f8a7b6c5d4e3f2a1b0c9d8e7f6a5b4c3d2e1f0",
  "tarball_url": "/api/v1/packages/http/1.2.0/download",
  "size": 45632,
  "published_at": "2025-12-10T10:30:00Z"
}
```

**Status Codes:**
- `200 OK`: Metadata retrieved
- `404 Not Found`: Package or version does not exist

---

### 5. Download Package / تنزيل الحزمة

تنزيل ملف أرشيف الحزمة (.tar.gz)

```http
GET /packages/{package_name}/{version}/download
```

**Response:**
- Binary file (application/gzip)
- Headers include `Content-Disposition` with filename

**Status Codes:**
- `200 OK`: Download started
- `404 Not Found`: Package or version does not exist
- `410 Gone`: Package was yanked (unavailable)

---

### 6. Publish Package / نشر حزمة

نشر إصدار جديد من الحزمة (يتطلب مصادقة)

```http
POST /packages/publish
Authorization: Bearer YOUR_API_TOKEN
Content-Type: multipart/form-data
```

**Request Body:**
- `manifest`: Package manifest (sad.toml content as JSON or TOML)
- `package`: Package archive file (.tar.gz)

**Manifest Example:**

```json
{
  "name": "mylib",
  "version": "1.0.0",
  "description": "My awesome library",
  "description_ar": "مكتبتي الرائعة",
  "authors": ["محمد أحمد <mohammed@example.com>"],
  "license": "MIT",
  "repository": "https://github.com/user/mylib",
  "dependencies": {
    "http": "^1.2.0"
  }
}
```

**Response:**

```json
{
  "success": true,
  "message": "Package published successfully",
  "message_ar": "تم نشر الحزمة بنجاح",
  "package": {
    "name": "mylib",
    "version": "1.0.0",
    "url": "/api/v1/packages/mylib/1.0.0"
  }
}
```

**Status Codes:**
- `201 Created`: Package published successfully
- `400 Bad Request`: Invalid manifest or archive
- `401 Unauthorized`: Authentication required
- `409 Conflict`: Version already exists
- `413 Payload Too Large`: Package size exceeds limit

---

### 7. Get User Packages / الحصول على حزم المستخدم

الحصول على قائمة بجميع الحزم المنشورة من قبل المستخدم (يتطلب مصادقة)

```http
GET /user/packages
Authorization: Bearer YOUR_API_TOKEN
```

**Response:**

```json
{
  "packages": [
    {
      "name": "mylib",
      "latest_version": "1.0.0",
      "downloads": 45,
      "created_at": "2025-12-01T10:00:00Z"
    },
    {
      "name": "another-lib",
      "latest_version": "2.3.1",
      "downloads": 128,
      "created_at": "2025-11-15T14:30:00Z"
    }
  ]
}
```

**Status Codes:**
- `200 OK`: Packages retrieved
- `401 Unauthorized`: Authentication required

---

### 8. Yank Package Version / سحب إصدار الحزمة

جعل إصدار معين من الحزمة غير متاح للتثبيت (يتطلب مصادقة)

```http
DELETE /packages/{package_name}/{version}/yank
Authorization: Bearer YOUR_API_TOKEN
```

**Response:**

```json
{
  "success": true,
  "message": "Package version yanked successfully",
  "message_ar": "تم سحب إصدار الحزمة بنجاح"
}
```

**Status Codes:**
- `200 OK`: Version yanked
- `401 Unauthorized`: Authentication required
- `403 Forbidden`: Not package owner
- `404 Not Found`: Package or version does not exist

---

### 9. Unyank Package Version / إلغاء سحب إصدار الحزمة

إعادة إتاحة إصدار مسحوب للتثبيت

```http
PUT /packages/{package_name}/{version}/unyank
Authorization: Bearer YOUR_API_TOKEN
```

**Response:**

```json
{
  "success": true,
  "message": "Package version unyanked successfully",
  "message_ar": "تم إلغاء سحب إصدار الحزمة بنجاح"
}
```

**Status Codes:**
- `200 OK`: Version unyanked
- `401 Unauthorized`: Authentication required
- `403 Forbidden`: Not package owner
- `404 Not Found`: Package or version does not exist

---

### 10. Authentication - Login / تسجيل الدخول

الحصول على رمز مصادقة

```http
POST /auth/login
Content-Type: application/json
```

**Request:**

```json
{
  "username": "user@example.com",
  "password": "secure_password"
}
```

**Response:**

```json
{
  "success": true,
  "token": "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...",
  "expires_in": 86400,
  "user": {
    "id": "user-123",
    "username": "ahmad",
    "email": "user@example.com"
  }
}
```

**Status Codes:**
- `200 OK`: Login successful
- `401 Unauthorized`: Invalid credentials
- `429 Too Many Requests`: Rate limit exceeded

---

## Package Archive Format / صيغة أرشيف الحزمة

Packages must be compressed as `.tar.gz` archives with the following structure:

يجب ضغط الحزم كأرشيفات `.tar.gz` بالهيكل التالي:

```
package-1.0.0/
├── sad.toml           # Package manifest / ملف البيانات
├── README.md          # Documentation / التوثيق
├── LICENSE            # License file / ملف الترخيص
└── src/               # Source files / ملفات المصدر
    ├── main.s
    └── lib/
        └── helper.s
```

### sad.toml Format / صيغة ملف sad.toml

```toml
[package]
name = "mylib"
version = "1.0.0"
description = "My awesome library"
description_ar = "مكتبتي الرائعة"
authors = ["محمد أحمد <mohammed@example.com>"]
license = "MIT"
repository = "https://github.com/user/mylib"
homepage = "https://mylib.sadlang.org"
readme = "README.md"
keywords = ["web", "http", "client"]

[dependencies]
http = "^1.2.0"
json = "~2.1.0"

[dev-dependencies]
test = "^1.0.0"
```

---

## Error Responses / استجابات الخطأ

جميع الأخطاء تُعاد بصيغة JSON موحدة:

```json
{
  "success": false,
  "error": {
    "code": "PACKAGE_NOT_FOUND",
    "message": "Package 'invalid-package' not found",
    "message_ar": "الحزمة 'invalid-package' غير موجودة"
  }
}
```

### Error Codes / رموز الأخطاء

| Code | Description | الوصف |
|------|-------------|--------|
| `PACKAGE_NOT_FOUND` | Package does not exist | الحزمة غير موجودة |
| `VERSION_NOT_FOUND` | Version does not exist | الإصدار غير موجود |
| `VERSION_EXISTS` | Version already published | الإصدار منشور بالفعل |
| `INVALID_MANIFEST` | Invalid package manifest | ملف البيانات غير صالح |
| `INVALID_ARCHIVE` | Invalid package archive | الأرشيف غير صالح |
| `UNAUTHORIZED` | Authentication required | المصادقة مطلوبة |
| `FORBIDDEN` | Permission denied | الإذن مرفوض |
| `RATE_LIMIT_EXCEEDED` | Too many requests | طلبات كثيرة جداً |
| `SERVER_ERROR` | Internal server error | خطأ في الخادم |

---

## Rate Limiting / حد المعدل

API requests are rate-limited per IP address and per user:

طلبات API محدودة لكل عنوان IP ولكل مستخدم:

- **Anonymous requests:** 60 requests per hour
- **Authenticated requests:** 500 requests per hour

Response headers include rate limit information:

```http
X-RateLimit-Limit: 500
X-RateLimit-Remaining: 485
X-RateLimit-Reset: 1702300800
```

---

## Versioning / الإصدارات

The API uses semantic versioning in the URL path (`/api/v1/`). Breaking changes will result in a new version (e.g., `/api/v2/`).

تستخدم API الإصدار الدلالي في مسار URL (`/api/v1/`). التغييرات الكبيرة ستؤدي إلى إصدار جديد (مثل `/api/v2/`).

---

## Security / الأمان

### HTTPS Required / HTTPS مطلوب

All API requests must use HTTPS. HTTP requests will be redirected to HTTPS.

### Package Signatures / توقيعات الحزم

Packages should be signed using Ed25519 signatures. The signature is included in the package manifest and verified by the registry.

يجب توقيع الحزم باستخدام توقيعات Ed25519. يتم تضمين التوقيع في ملف البيانات والتحقق منه بواسطة السجل.

### Content Security / أمان المحتوى

All uploaded packages are scanned for:
- Malicious code / الشيفرة الخبيثة
- License compliance / الامتثال للترخيص
- Dependency security / أمان التبعيات

---

## Support / الدعم

For questions or issues, please contact:

للأسئلة أو المشاكل، يرجى الاتصال بـ:

- **Email:** support@sadlang.org
- **GitHub:** https://github.com/sad-lang/sad
- **Discord:** https://discord.gg/sadlang

---

الحمد لله رب العالمين
