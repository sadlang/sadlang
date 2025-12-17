# Firebase Integration with Sad Language
# تكامل Firebase مع لغة ص

## 🔥 نظرة عامة / Overview

This example demonstrates how to use Firebase services directly from Sad Language:

- **Firestore** - قاعدة البيانات (Database)
- **Storage** - التخزين السحابي (Cloud Storage)
- **Authentication** - المصادقة (User Authentication)
- **Realtime Database** - قاعدة البيانات الفورية (Real-time Sync)
- **Cloud Functions** - الدوال السحابية (Serverless Functions)

## 📋 متطلبات / Requirements

```powershell
# 1. Create Firebase project
firebase create-project sad-lang-firebase

# 2. Install Firebase dependencies
sad-pkg add firebase ^1.0.0
sad-pkg add json ^2.0.0
sad-pkg add http ^1.5.0

# 3. Configure Firebase
firebase config:set projectId sad-lang-firebase
```

## ⚙️ الإعدادات / Configuration

### 1. Firebase Config

```sad
متغير config = {
    apiKey: "YOUR_API_KEY",
    projectId: "sad-lang-firebase",
    databaseURL: "https://sad-lang-firebase.firebaseio.com",
    storageBucket: "sad-lang-firebase.appspot.com",
    messagingSenderId: "YOUR_SENDER_ID",
    appId: "YOUR_APP_ID"
};
```

### 2. Initialize Firebase

```sad
متغير app = FirebaseApp(config);
```

## 🗄️ Firestore - قاعدة البيانات

### Create/Add Document

```sad
متغير packages = app.firestore.collection("packages");

متغير packageData = {
    name: "http-client",
    version: "2.1.0",
    author: "Sad Team"
};

متغير docId = packages.add(packageData);
```

### Read Document

```sad
متغير doc = packages.get("document_id");
```

### Update Document

```sad
packages.update("document_id", {
    version: "2.1.1",
    downloads: 150
});
```

### Delete Document

```sad
packages.delete("document_id");
```

### Query Documents

```sad
متغير results = packages.where("author", "==", "Sad Team");
متغير filtered = packages.where("version", ">", "1.0.0");
```

## 💾 Storage - التخزين السحابي

### Upload File

```sad
متغير storage = app.storage.ref("packages");
storage.upload("C:\\local\\file.tar.gz");
```

### Download File

```sad
storage.download("C:\\downloads\\file.tar.gz");
```

### Get Download URL

```sad
متغير url = storage.get_download_url();
```

### Get Metadata

```sad
متغير metadata = storage.get_metadata();
```

### Delete File

```sad
storage.delete();
```

## 🔐 Authentication - المصادقة

### Sign Up

```sad
متغير user = app.auth.sign_up("email@example.com", "password");
```

### Sign In

```sad
متغير user = app.auth.sign_in("email@example.com", "password");
```

### Get Current User

```sad
متغير currentUser = app.auth.get_current_user();
```

### Update Profile

```sad
app.auth.update_profile("Display Name", "https://example.com/photo.jpg");
```

### Reset Password

```sad
app.auth.reset_password("email@example.com");
```

### Sign Out

```sad
app.auth.sign_out();
```

## 🔄 Realtime Database - قاعدة البيانات الفورية

### Set Value

```sad
متغير statsRef = app.database.ref("stats");
statsRef.set({
    totalPackages: 150,
    totalDownloads: 5000
});
```

### Get Value

```sad
متغير stats = statsRef.get();
```

### Update Value

```sad
statsRef.update({totalDownloads: 5100});
```

### Listen for Changes

```sad
statsRef.on_change(دالة(value) {
    اطبع("Stats changed: " + value);
});
```

### Remove Value

```sad
statsRef.remove();
```

## ☁️ Cloud Functions - الدوال السحابية

### Call Function

```sad
متغير result = app.functions.call("calculateStats", {
    startDate: "2024-01-01",
    endDate: "2024-12-31"
});
```

## 🚀 تشغيل المثال / Running the Example

```powershell
# 1. Build the example
cd C:\s\s_language\examples\firebase_example
sad build

# 2. Run the example
sad run main.sad

# 3. Or use sad-pkg
sad-pkg run
```

## 📊 مخرجات متوقعة / Expected Output

```
🔥 Initializing Firebase...
   Project: sad-lang-firebase
   API Key: AIzaSyDx...

📚 FIRESTORE DEMO
1. Adding new package...
   ✅ Package added: doc_1702300800

2. Getting package...
   ✅ Package retrieved: doc_1702300800

... (more operations)
```

## 🔒 الأمان / Security

### Firestore Security Rules

```javascript
rules_version = '2';
service cloud.firestore {
  match /databases/{database}/documents {
    // Public read, authenticated write
    match /packages/{document=**} {
      allow read: if true;
      allow write: if request.auth != null;
    }
    
    // User-owned data
    match /users/{userId}/{document=**} {
      allow read, write: if request.auth.uid == userId;
    }
  }
}
```

### Storage Security Rules

```javascript
rules_version = '2';
service firebase.storage {
  match /b/{bucket}/o {
    // Public read, authenticated write
    match /packages/{allPaths=**} {
      allow read: if true;
      allow write: if request.auth != null;
    }
  }
}
```

## 📚 مزيد من الموارد / More Resources

- [Firebase Documentation](https://firebase.google.com/docs)
- [Sad Language Docs](https://sadlang.org/docs)
- [Package Manager Guide](../../docs/PKG_REGISTRY_INTEGRATION.md)

## 🤝 المساهمة / Contributing

Contributions are welcome! Please follow the Sad Language contribution guidelines.

---

**الحمد لله رب العالمين** ✨
