# بسم الله الرحمن الرحيم

# Sad Language Package Registry - Dart Backend Example
# مثال على خادم سجل الحزم بلغة Dart

هذا مثال بسيط لخادم Backend مبني بـ Dart Shelf لسجل حزم لغة ص. يمكن استخدامه كنقطة بداية لبناء خادم كامل.

## Requirements / المتطلبات

```yaml
# pubspec.yaml
name: sad_registry_server
description: Package registry server for Sad Language
version: 1.0.0

environment:
  sdk: '>=3.0.0 <4.0.0'

dependencies:
  shelf: ^1.4.0
  shelf_router: ^1.1.0
  dart_jsonwebtoken: ^2.12.0
  postgres: ^2.7.0
  crypto: ^3.0.3
  path: ^1.8.3
  mime: ^1.0.4

dev_dependencies:
  lints: ^2.1.0
  test: ^1.24.0
```

## Project Structure / بنية المشروع

```
sad_registry_server/
├── bin/
│   └── server.dart           # Main server entry / نقطة الدخول الرئيسية
├── lib/
│   ├── api/
│   │   ├── packages_api.dart # Package endpoints / نقاط الحزم
│   │   ├── auth_api.dart     # Authentication / المصادقة
│   │   └── search_api.dart   # Search endpoints / نقاط البحث
│   ├── models/
│   │   ├── package.dart      # Package model / نموذج الحزمة
│   │   ├── user.dart         # User model / نموذج المستخدم
│   │   └── version.dart      # Version model / نموذج الإصدار
│   ├── services/
│   │   ├── db_service.dart   # Database service / خدمة قاعدة البيانات
│   │   ├── storage_service.dart # File storage / تخزين الملفات
│   │   └── auth_service.dart # Auth service / خدمة المصادقة
│   └── middleware/
│       ├── auth_middleware.dart  # Auth middleware / وسيط المصادقة
│       └── cors_middleware.dart  # CORS middleware / وسيط CORS
├── storage/               # Local file storage / تخزين الملفات المحلي
├── pubspec.yaml
└── README.md
```

## Implementation / التنفيذ

### 1. Main Server (bin/server.dart)

```dart
// بسم الله الرحمن الرحيم
import 'dart:io';
import 'package:shelf/shelf.dart';
import 'package:shelf/shelf_io.dart' as shelf_io;
import 'package:shelf_router/shelf_router.dart';
import '../lib/api/packages_api.dart';
import '../lib/api/auth_api.dart';
import '../lib/api/search_api.dart';
import '../lib/middleware/cors_middleware.dart';
import '../lib/middleware/auth_middleware.dart';

void main() async {
  // Initialize services
  print('Starting Sad Package Registry Server...');
  print('جاري تشغيل خادم سجل حزم لغة ص...');

  final app = Router();

  // Health check
  app.get('/health', (Request request) {
    return Response.ok('OK');
  });

  // API v1 routes
  app.mount('/api/v1/packages', PackagesApi().router);
  app.mount('/api/v1/search', SearchApi().router);
  app.mount('/api/v1/auth', AuthApi().router);
  app.mount('/api/v1/user', AuthApi().userRouter);

  // Create middleware pipeline
  final handler = Pipeline()
      .addMiddleware(corsMiddleware())
      .addMiddleware(logRequests())
      .addHandler(app);

  // Start server
  final port = int.parse(Platform.environment['PORT'] ?? '8080');
  final server = await shelf_io.serve(handler, '0.0.0.0', port);
  
  print('Server running on http://${server.address.host}:${server.port}');
  print('الخادم يعمل على http://${server.address.host}:${server.port}');
}
```

### 2. Package Model (lib/models/package.dart)

```dart
// بسم الله الرحمن الرحيم
class Package {
  final String name;
  final String version;
  final String description;
  final String? descriptionAr;
  final List<String> authors;
  final String license;
  final String? repository;
  final String? homepage;
  final Map<String, String> dependencies;
  final List<String> keywords;
  final String checksum;
  final int size;
  final DateTime publishedAt;
  final bool isYanked;

  Package({
    required this.name,
    required this.version,
    required this.description,
    this.descriptionAr,
    required this.authors,
    required this.license,
    this.repository,
    this.homepage,
    required this.dependencies,
    required this.keywords,
    required this.checksum,
    required this.size,
    required this.publishedAt,
    this.isYanked = false,
  });

  Map<String, dynamic> toJson() => {
    'name': name,
    'version': version,
    'description': description,
    'description_ar': descriptionAr,
    'authors': authors,
    'license': license,
    'repository': repository,
    'homepage': homepage,
    'dependencies': dependencies,
    'keywords': keywords,
    'checksum': checksum,
    'size': size,
    'published_at': publishedAt.toIso8601String(),
    'is_yanked': isYanked,
  };

  factory Package.fromJson(Map<String, dynamic> json) => Package(
    name: json['name'],
    version: json['version'],
    description: json['description'],
    descriptionAr: json['description_ar'],
    authors: List<String>.from(json['authors']),
    license: json['license'],
    repository: json['repository'],
    homepage: json['homepage'],
    dependencies: Map<String, String>.from(json['dependencies'] ?? {}),
    keywords: List<String>.from(json['keywords'] ?? []),
    checksum: json['checksum'],
    size: json['size'],
    publishedAt: DateTime.parse(json['published_at']),
    isYanked: json['is_yanked'] ?? false,
  );
}
```

### 3. Packages API (lib/api/packages_api.dart)

```dart
// بسم الله الرحمن الرحيم
import 'dart:convert';
import 'dart:io';
import 'package:shelf/shelf.dart';
import 'package:shelf_router/shelf_router.dart';
import '../models/package.dart';
import '../services/db_service.dart';
import '../services/storage_service.dart';
import '../middleware/auth_middleware.dart';

class PackagesApi {
  final DbService _db = DbService();
  final StorageService _storage = StorageService();

  Router get router {
    final router = Router();

    // Get package info
    router.get('/<name>', _getPackage);

    // Get package versions
    router.get('/<name>/versions', _getVersions);

    // Get specific version metadata
    router.get('/<name>/<version>', _getPackageVersion);

    // Download package
    router.get('/<name>/<version>/download', _downloadPackage);

    // Publish package (requires auth)
    router.post('/publish', Pipeline()
        .addMiddleware(authMiddleware())
        .addHandler(_publishPackage));

    // Yank package version (requires auth)
    router.delete('/<name>/<version>/yank', Pipeline()
        .addMiddleware(authMiddleware())
        .addHandler(_yankPackage));

    // Unyank package version (requires auth)
    router.put('/<name>/<version>/unyank', Pipeline()
        .addMiddleware(authMiddleware())
        .addHandler(_unyankPackage));

    return router;
  }

  // Get package info
  Future<Response> _getPackage(Request request, String name) async {
    try {
      final package = await _db.getPackage(name);
      if (package == null) {
        return Response.notFound(jsonEncode({
          'success': false,
          'error': {
            'code': 'PACKAGE_NOT_FOUND',
            'message': 'Package not found',
            'message_ar': 'الحزمة غير موجودة'
          }
        }));
      }

      return Response.ok(
        jsonEncode(package.toJson()),
        headers: {'Content-Type': 'application/json'},
      );
    } catch (e) {
      return Response.internalServerError(
        body: jsonEncode({
          'success': false,
          'error': {
            'code': 'SERVER_ERROR',
            'message': e.toString(),
          }
        }),
      );
    }
  }

  // Get package versions
  Future<Response> _getVersions(Request request, String name) async {
    try {
      final versions = await _db.getPackageVersions(name);
      if (versions.isEmpty) {
        return Response.notFound(jsonEncode({
          'success': false,
          'error': {
            'code': 'PACKAGE_NOT_FOUND',
            'message': 'Package not found',
            'message_ar': 'الحزمة غير موجودة'
          }
        }));
      }

      return Response.ok(
        jsonEncode({
          'name': name,
          'versions': versions,
        }),
        headers: {'Content-Type': 'application/json'},
      );
    } catch (e) {
      return Response.internalServerError(
        body: jsonEncode({
          'success': false,
          'error': {
            'code': 'SERVER_ERROR',
            'message': e.toString(),
          }
        }),
      );
    }
  }

  // Get specific version metadata
  Future<Response> _getPackageVersion(Request request, String name, String version) async {
    try {
      final package = await _db.getPackageVersion(name, version);
      if (package == null) {
        return Response.notFound(jsonEncode({
          'success': false,
          'error': {
            'code': 'VERSION_NOT_FOUND',
            'message': 'Version not found',
            'message_ar': 'الإصدار غير موجود'
          }
        }));
      }

      return Response.ok(
        jsonEncode(package.toJson()),
        headers: {'Content-Type': 'application/json'},
      );
    } catch (e) {
      return Response.internalServerError(
        body: jsonEncode({
          'success': false,
          'error': {
            'code': 'SERVER_ERROR',
            'message': e.toString(),
          }
        }),
      );
    }
  }

  // Download package archive
  Future<Response> _downloadPackage(Request request, String name, String version) async {
    try {
      final package = await _db.getPackageVersion(name, version);
      if (package == null || package.isYanked) {
        return Response.notFound(jsonEncode({
          'success': false,
          'error': {
            'code': package?.isYanked == true ? 'PACKAGE_YANKED' : 'VERSION_NOT_FOUND',
            'message': 'Package version not available',
            'message_ar': 'إصدار الحزمة غير متاح'
          }
        }));
      }

      final file = await _storage.getPackageFile(name, version);
      if (file == null) {
        return Response.notFound('Package file not found');
      }

      final bytes = await file.readAsBytes();
      return Response.ok(
        bytes,
        headers: {
          'Content-Type': 'application/gzip',
          'Content-Disposition': 'attachment; filename="$name-$version.tar.gz"',
          'Content-Length': bytes.length.toString(),
        },
      );
    } catch (e) {
      return Response.internalServerError(
        body: jsonEncode({
          'success': false,
          'error': {
            'code': 'SERVER_ERROR',
            'message': e.toString(),
          }
        }),
      );
    }
  }

  // Publish package
  Future<Response> _publishPackage(Request request) async {
    try {
      // Parse multipart form data
      final contentType = request.headers['content-type'];
      if (contentType == null || !contentType.contains('multipart/form-data')) {
        return Response(400, body: jsonEncode({
          'success': false,
          'error': {
            'code': 'INVALID_REQUEST',
            'message': 'Expected multipart/form-data',
            'message_ar': 'متوقع multipart/form-data'
          }
        }));
      }

      // TODO: Parse multipart form data
      // Extract manifest and package file
      // Validate manifest
      // Check if version already exists
      // Save package file to storage
      // Save metadata to database
      // Return success response

      return Response(201, body: jsonEncode({
        'success': true,
        'message': 'Package published successfully',
        'message_ar': 'تم نشر الحزمة بنجاح',
      }));
    } catch (e) {
      return Response.internalServerError(
        body: jsonEncode({
          'success': false,
          'error': {
            'code': 'SERVER_ERROR',
            'message': e.toString(),
          }
        }),
      );
    }
  }

  // Yank package version
  Future<Response> _yankPackage(Request request, String name, String version) async {
    try {
      final userId = request.context['user_id'] as String;
      final success = await _db.yankPackage(name, version, userId);
      
      if (!success) {
        return Response.forbidden(jsonEncode({
          'success': false,
          'error': {
            'code': 'FORBIDDEN',
            'message': 'Not authorized to yank this package',
            'message_ar': 'غير مصرح بسحب هذه الحزمة'
          }
        }));
      }

      return Response.ok(jsonEncode({
        'success': true,
        'message': 'Package version yanked successfully',
        'message_ar': 'تم سحب إصدار الحزمة بنجاح',
      }));
    } catch (e) {
      return Response.internalServerError(
        body: jsonEncode({
          'success': false,
          'error': {
            'code': 'SERVER_ERROR',
            'message': e.toString(),
          }
        }),
      );
    }
  }

  // Unyank package version
  Future<Response> _unyankPackage(Request request, String name, String version) async {
    try {
      final userId = request.context['user_id'] as String;
      final success = await _db.unyankPackage(name, version, userId);
      
      if (!success) {
        return Response.forbidden(jsonEncode({
          'success': false,
          'error': {
            'code': 'FORBIDDEN',
            'message': 'Not authorized to unyank this package',
            'message_ar': 'غير مصرح بإلغاء سحب هذه الحزمة'
          }
        }));
      }

      return Response.ok(jsonEncode({
        'success': true,
        'message': 'Package version unyanked successfully',
        'message_ar': 'تم إلغاء سحب إصدار الحزمة بنجاح',
      }));
    } catch (e) {
      return Response.internalServerError(
        body: jsonEncode({
          'success': false,
          'error': {
            'code': 'SERVER_ERROR',
            'message': e.toString(),
          }
        }),
      );
    }
  }
}
```

### 4. CORS Middleware (lib/middleware/cors_middleware.dart)

```dart
// بسم الله الرحمن الرحيم
import 'package:shelf/shelf.dart';

Middleware corsMiddleware() {
  return (Handler handler) {
    return (Request request) async {
      // Handle preflight requests
      if (request.method == 'OPTIONS') {
        return Response.ok('', headers: _corsHeaders);
      }

      // Handle actual request
      final response = await handler(request);
      return response.change(headers: _corsHeaders);
    };
  };
}

const _corsHeaders = {
  'Access-Control-Allow-Origin': '*',
  'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, OPTIONS',
  'Access-Control-Allow-Headers': 'Origin, Content-Type, Authorization',
};
```

### 5. Auth Middleware (lib/middleware/auth_middleware.dart)

```dart
// بسم الله الرحمن الرحيم
import 'package:shelf/shelf.dart';
import 'package:dart_jsonwebtoken/dart_jsonwebtoken.dart';
import 'dart:convert';

Middleware authMiddleware() {
  return (Handler handler) {
    return (Request request) async {
      final authHeader = request.headers['authorization'];
      
      if (authHeader == null || !authHeader.startsWith('Bearer ')) {
        return Response(401, body: jsonEncode({
          'success': false,
          'error': {
            'code': 'UNAUTHORIZED',
            'message': 'Authentication required',
            'message_ar': 'المصادقة مطلوبة'
          }
        }));
      }

      final token = authHeader.substring(7);
      
      try {
        final jwt = JWT.verify(token, SecretKey('YOUR_SECRET_KEY'));
        final payload = jwt.payload as Map<String, dynamic>;
        
        // Add user info to request context
        return await handler(request.change(context: {
          'user_id': payload['user_id'],
          'username': payload['username'],
        }));
      } catch (e) {
        return Response(401, body: jsonEncode({
          'success': false,
          'error': {
            'code': 'INVALID_TOKEN',
            'message': 'Invalid or expired token',
            'message_ar': 'رمز غير صالح أو منتهي الصلاحية'
          }
        }));
      }
    };
  };
}
```

## Running the Server / تشغيل الخادم

```bash
# Install dependencies
dart pub get

# Run in development mode
dart run bin/server.dart

# Build for production
dart compile exe bin/server.dart -o server

# Run production build
./server
```

## Environment Variables / متغيرات البيئة

```bash
PORT=8080                    # Server port / منفذ الخادم
DATABASE_URL=postgres://...  # PostgreSQL connection / اتصال PostgreSQL
JWT_SECRET=your_secret_key   # JWT secret / مفتاح JWT السري
STORAGE_PATH=./storage       # Package storage path / مسار تخزين الحزم
```

## Database Schema / مخطط قاعدة البيانات

```sql
-- بسم الله الرحمن الرحيم

-- Users table / جدول المستخدمين
CREATE TABLE users (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    username VARCHAR(255) UNIQUE NOT NULL,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Packages table / جدول الحزم
CREATE TABLE packages (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name VARCHAR(255) UNIQUE NOT NULL,
    description TEXT,
    description_ar TEXT,
    repository VARCHAR(500),
    homepage VARCHAR(500),
    license VARCHAR(100),
    owner_id UUID REFERENCES users(id),
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- Package versions table / جدول إصدارات الحزم
CREATE TABLE package_versions (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    package_id UUID REFERENCES packages(id),
    version VARCHAR(50) NOT NULL,
    checksum VARCHAR(100) NOT NULL,
    size INTEGER NOT NULL,
    dependencies JSONB DEFAULT '{}',
    keywords TEXT[] DEFAULT '{}',
    authors TEXT[] DEFAULT '{}',
    is_yanked BOOLEAN DEFAULT FALSE,
    published_at TIMESTAMP DEFAULT NOW(),
    UNIQUE(package_id, version)
);

-- Downloads table / جدول التنزيلات
CREATE TABLE downloads (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    package_id UUID REFERENCES packages(id),
    version_id UUID REFERENCES package_versions(id),
    ip_address VARCHAR(45),
    downloaded_at TIMESTAMP DEFAULT NOW()
);

-- Create indexes / إنشاء الفهارس
CREATE INDEX idx_packages_name ON packages(name);
CREATE INDEX idx_package_versions_package_id ON package_versions(package_id);
CREATE INDEX idx_downloads_package_id ON downloads(package_id);
```

---

الحمد لله رب العالمين
