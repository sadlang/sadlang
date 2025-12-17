# 🚀 Firebase Integration with sad-pkg
# تكامل Firebase مع مدير الحزم

## نظرة عامة / Overview

هذا الدليل يشرح كيفية ربط `sad-pkg` (مدير الحزم) مع Firebase للحصول على:

✅ سجل حزم سحابي  
✅ تخزين آمن للملفات  
✅ مصادقة محمية  
✅ قاعدة بيانات موثوقة  

---

## 🔧 الإعدادات الأساسية / Basic Setup

### 1. إنشاء مشروع Firebase

```powershell
# تسجيل الدخول
firebase login

# إنشاء مشروع جديد
firebase create-project sad-lang-registry

# تعيين المشروع الحالي
firebase use sad-lang-registry
```

### 2. تفعيل الخدمات

```powershell
firebase firestore enable
firebase storage enable
firebase auth enable
firebase functions enable
```

### 3. الحصول على بيانات الاتصال

```
Firebase Console
  → Project Settings (⚙️)
  → Service Accounts
  → Generate New Private Key
  → Save as serviceAccountKey.json
```

---

## 📝 تحديث sad-pkg

### تعديل `tools/pkg/registry_client.h`

```cpp
// إضافة دعم Firebase
#include <firebase/app.h>
#include <firebase/firestore.h>
#include <firebase/storage.h>
#include <firebase/auth.h>

class FirebaseRegistryClient : public IPackageRegistry {
private:
    firebase::App* firebase_app;
    firebase::firestore::Firestore* firestore_db;
    firebase::storage::Storage* storage_bucket;
    firebase::auth::Auth* auth_obj;

public:
    FirebaseRegistryClient(const std::string& project_id) {
        // Initialize Firebase
        firebase::AppOptions options;
        options.set_api_key("YOUR_API_KEY");
        options.set_project_id(project_id);
        
        firebase_app = firebase::App::Create(options);
        firestore_db = firebase::firestore::Firestore::GetInstance(firebase_app);
        storage_bucket = firebase::storage::Storage::GetInstance(firebase_app);
        auth_obj = firebase::auth::Auth::GetInstance(firebase_app);
    }
    
    // Implement registry methods using Firebase
    std::vector<std::string> search(const std::string& query) override {
        // Query Firestore collection "packages"
        auto future = firestore_db->Collection("packages")
            .WhereArrayContains("keywords", query)
            .Get();
        
        // Process results...
    }
};
```

### تعديل `tools/pkg/cli.cpp`

```cpp
// استخدام Firebase في CLI
void PublishCommand::Execute(const std::vector<std::string>& args) {
    std::string project_id = config.get("firebase_project_id");
    
    FirebaseRegistryClient registry(project_id);
    
    // 1. التحقق من الحزمة
    Package pkg = Package::from_file("sad.toml");
    
    // 2. رفع الملف
    registry.upload_package(pkg, "path/to/package.tar.gz");
    
    // 3. حفظ البيانات
    registry.publish(pkg);
    
    std::cout << "✅ Package published successfully!" << std::endl;
}
```

---

## 🏗️ هيكل Firebase

### Collections

```
sad-lang-registry
├── packages/
│   ├── {packageId}/
│   │   ├── name: "math-utils"
│   │   ├── version: "1.0.0"
│   │   ├── description: "..."
│   │   ├── author: "uid"
│   │   ├── downloads: 150
│   │   ├── rating: 4.5
│   │   ├── archiveUrl: "gs://..."
│   │   ├── publishedAt: timestamp
│   │   └── dependencies: {...}
│   └── ...
│
├── users/
│   ├── {userId}/
│   │   ├── email: "dev@sadlang.org"
│   │   ├── displayName: "Developer"
│   │   ├── packages: ["pkg1", "pkg2"]
│   │   ├── joined: timestamp
│   │   └── lastLogin: timestamp
│   └── ...
│
├── downloads/
│   ├── {downloadId}/
│   │   ├── packageId: "math-utils"
│   │   ├── packageVersion: "1.0.0"
│   │   ├── userId: "uid"
│   │   ├── timestamp: timestamp
│   │   └── ipAddress: "..."
│   └── ...
│
└── reviews/
    ├── {reviewId}/
    │   ├── packageId: "math-utils"
    │   ├── userId: "uid"
    │   ├── rating: 5
    │   ├── comment: "Great library!"
    │   └── timestamp: timestamp
    └── ...
```

---

## 🔐 الأمان / Security Rules

### Firestore Rules

```javascript
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    
    // Public read, authenticated write
    match /packages/{document=**} {
      allow read: if true;
      allow create: if request.auth != null &&
                       request.resource.data.author == request.auth.uid;
      allow update, delete: if request.auth != null &&
                              resource.data.author == request.auth.uid;
    }
    
    // User data protection
    match /users/{userId}/{document=**} {
      allow read: if request.auth != null && request.auth.uid == userId;
      allow write: if request.auth != null && request.auth.uid == userId;
    }
    
    // Downloads logging
    match /downloads/{document=**} {
      allow create: if request.auth != null;
      allow read: if request.auth.uid == resource.data.userId;
    }
  }
}
```

### Storage Rules

```javascript
rules_version = '2';
service firebase.storage {
  match /b/{bucket}/o {
    
    // Package archives - public read, auth write
    match /packages/{allPaths=**} {
      allow read: if true;
      allow write: if request.auth != null;
    }
    
    // User uploads
    match /users/{userId}/{allPaths=**} {
      allow read, write: if request.auth.uid == userId;
    }
  }
}
```

---

## 📦 تدفق العمل / Workflow

### لمنشر الحزم / For Package Publisher

```
1. Create sad.toml
   ├── name: "my-package"
   ├── version: "1.0.0"
   ├── author: "username"
   └── ...

2. sad-pkg login
   └── يتم المصادقة عبر Firebase Auth

3. sad-pkg publish
   ├── يتم رفع الملف إلى Storage
   ├── يتم حفظ البيانات في Firestore
   └── ✅ تم النشر

4. Update status
   ├── Updates in Firestore
   ├── Analytics tracking
   └── Notifications sent
```

### لمستخدم الحزم / For Package User

```
1. sad-pkg search "math"
   ├── Query Firestore
   ├── Return results
   └── Show options

2. sad-pkg add math-utils ^1.0.0
   ├── Add to sad.toml
   └── Resolve dependencies

3. sad-pkg install
   ├── Download from Storage
   ├── Extract archive
   ├── Install dependencies
   ├── Update Firestore counter
   └── ✅ Installation complete
```

---

## 🔍 الاستعلامات / Queries

### البحث عن الحزم

```sad
// في firebase.sad
دالة search_packages(query: نص) {
    متغير packages = app.firestore.collection("packages");
    
    // البحث بالاسم
    متغير byName = packages.where("name", "contains", query);
    
    // البحث بالكلمات المفتاحية
    متغير byKeywords = packages.where("keywords", "arrayContains", query);
    
    // ترتيب بعدد التحميلات
    متغير sorted = byName.orderBy("downloads", "desc");
    
    ارجع sorted;
}
```

### الحصول على حزم المستخدم

```sad
دالة get_user_packages(userId: نص) {
    متغير packages = app.firestore.collection("packages");
    
    متغير userPkgs = packages.where("author", "==", userId);
    
    ارجع userPkgs;
}
```

### تحديث عدد التحميلات

```sad
دالة increment_download_count(packageId: نص) {
    متغير packages = app.firestore.collection("packages");
    
    متغير pkg = packages.get(packageId);
    
    packages.update(packageId, {
        downloads: pkg["downloads"] + 1
    });
    
    // Log download
    متغير downloads = app.firestore.collection("downloads");
    downloads.add({
        packageId: packageId,
        userId: current_user.uid,
        timestamp: current_timestamp()
    });
}
```

---

## 🚀 خطوات النشر الكاملة / Complete Deployment

### 1. التحضير

```powershell
cd C:\s\s_language\examples\firebase_example

# تثبيت التبعيات
npm install -g firebase-tools
```

### 2. الإعدادات

```powershell
# تسجيل الدخول
firebase login

# اختيار المشروع
firebase use sad-lang-registry

# نشر القواعس
firebase deploy --only firestore:rules,storage
```

### 3. البناء والاختبار

```powershell
# بناء Sad
cd C:\s\s_language
cmake --build build --config Release

# تشغيل المثال
sad run examples\firebase_example\main.sad

# اختبار سجل الحزم
sad run examples\firebase_example\package_registry.sad
```

### 4. النشر النهائي

```powershell
# نشر كل شيء
firebase deploy

# التحقق من النشر
firebase console
```

---

## 📊 المراقبة والتحليلات / Monitoring

### تتبع الإحصائيات

```sad
دالة track_stats() {
    متغير statsRef = app.database.ref("stats");
    
    متغير stats = {
        totalPackages: get_total_packages_count(),
        totalDownloads: get_total_downloads_count(),
        activeUsers: get_active_users_count(),
        lastUpdate: current_timestamp()
    };
    
    statsRef.set(stats);
}
```

### عرض التقارير

```powershell
firebase analytics:list
firebase analytics:export
```

---

## 🔗 تكامل مع sad-pkg

### إضافة Firebase كخادم افتراضي

في `~/.sad/config.toml`:

```toml
[registry]
url = "https://sad-lang-registry.firebaseapp.com"
type = "firebase"
project_id = "sad-lang-registry"

[firebase]
api_key = "YOUR_API_KEY"
auth_domain = "sad-lang-registry.firebaseapp.com"
storage_bucket = "sad-lang-registry.appspot.com"
```

### استخدام في CLI

```powershell
# تسجيل دخول
sad-pkg login

# نشر حزمة
sad-pkg publish

# البحث
sad-pkg search math

# التثبيت
sad-pkg install math-utils
```

---

## ✅ قائمة التحقق / Checklist

- [ ] Firebase project created
- [ ] Services enabled (Firestore, Storage, Auth)
- [ ] Firestore rules deployed
- [ ] Storage rules deployed
- [ ] Service account key saved
- [ ] firebase.sad library installed
- [ ] sad-pkg configured
- [ ] sad-pkg login successful
- [ ] Package published to Firebase
- [ ] Package searchable
- [ ] Package downloadable
- [ ] Analytics tracking working
- [ ] Security rules tested

---

## 🎓 أمثلة متقدمة / Advanced Examples

### مثال 1: نشر حزمة متقدمة

```sad
دالة publish_advanced_package() {
    متغير app = FirebaseApp(config);
    متغير registry = PackageRegistry(app);
    
    // 1. بناء الحزمة
    متغير pkg = build_package("my-package");
    
    // 2. توقيع الحزمة
    متغير signature = sign_package(pkg);
    
    // 3. النشر
    متغير result = registry.publish(pkg.path, {
        name: pkg.name,
        version: pkg.version,
        signature: signature,
        changeLog: read_file("CHANGELOG.md")
    });
    
    // 4. الإشعار
    notify_users("New package: " + pkg.name);
}
```

### مثال 2: البحث المتقدم

```sad
دالة advanced_search(filters: قاموس) {
    متغير packages = app.firestore.collection("packages");
    
    متغير query = packages;
    
    // تصفية حسب الكاتب
    اذا (filters["author"]) {
        query = query.where("author", "==", filters["author"]);
    }
    
    // تصفية حسب الإصدار
    اذا (filters["minVersion"]) {
        query = query.where("version", ">=", filters["minVersion"]);
    }
    
    // تصفية حسب التقييم
    اذا (filters["minRating"]) {
        query = query.where("rating", ">=", filters["minRating"]);
    }
    
    // ترتيب النتائج
    متغير sorted = query.orderBy("downloads", "desc");
    
    ارجع sorted;
}
```

---

## 📚 مراجع / References

- [Firebase Documentation](https://firebase.google.com/docs)
- [Firebase Admin SDK](https://firebase.google.com/docs/admin/setup)
- [Firestore Security Rules](https://firebase.google.com/docs/firestore/security/start)
- [Storage Security Rules](https://firebase.google.com/docs/storage/security/start)
- [Sad Language Docs](https://sadlang.org/docs)

---

**الحمد لله رب العالمين** 🙏

تم الإنجاز: December 11, 2025
