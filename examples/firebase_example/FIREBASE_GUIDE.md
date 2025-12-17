# 🔥 Firebase Integration Guide for Sad Language
# دليل تكامل Firebase مع لغة ص

## 📚 Table of Contents / جدول المحتويات

1. [Getting Started](#getting-started)
2. [Firebase Setup](#firebase-setup)
3. [Sad Language Firebase Library](#sad-language-firebase-library)
4. [Examples](#examples)
5. [Best Practices](#best-practices)
6. [Troubleshooting](#troubleshooting)

---

## 🚀 Getting Started / البدء السريع

### Prerequisites / المتطلبات

```powershell
# 1. Node.js and npm
node --version  # v16+
npm --version

# 2. Firebase CLI
npm install -g firebase-tools

# 3. Sad Language Compiler
sad --version

# 4. Sad Package Manager
sad-pkg --version
```

### Installation / التثبيت

```powershell
# 1. Create Firebase project
cd C:\s\s_language\examples\firebase_example

# 2. Initialize Firebase
firebase init

# 3. Login to Firebase
firebase login

# 4. Install Sad Firebase package
sad-pkg add firebase ^1.0.0
```

---

## 🔥 Firebase Setup / إعداد Firebase

### Step 1: Create Firebase Project

```powershell
# Via Firebase Console
# 1. Go to https://console.firebase.google.com
# 2. Click "Create Project"
# 3. Enter project name: "sad-lang-firebase"
# 4. Enable Google Analytics
# 5. Create project
```

### Step 2: Get Firebase Config

```javascript
// Copy from Firebase Console > Project Settings
const firebaseConfig = {
    apiKey: "AIzaSyDxxxxxxxxxxxxxxxxxxx",
    authDomain: "sad-lang-firebase.firebaseapp.com",
    projectId: "sad-lang-firebase",
    storageBucket: "sad-lang-firebase.appspot.com",
    messagingSenderId: "123456789",
    appId: "1:123456789:web:xxxxxxxxxxxxxxxx"
};
```

### Step 3: Enable Services

```powershell
# Enable Firestore
firebase firestore enable

# Enable Storage
firebase storage enable

# Enable Authentication
firebase auth enable

# Enable Functions
firebase functions enable
```

---

## 📚 Sad Language Firebase Library / مكتبة Firebase لغة ص

### Module Structure / هيكل الوحدة

```
firebase.sad
├── FirebaseConfig        # Configuration type
├── Firestore            # Database
│   ├── FirestoreCollection
│   └── FirestoreDoc
├── Storage              # Cloud Storage
│   └── StorageRef
├── Auth                 # Authentication
│   └── User
├── RealtimeDatabase     # Real-time Sync
│   └── DatabaseRef
└── CloudFunctions       # Serverless Functions
```

### Import and Initialize / الاستيراد والتهيئة

```sad
استورد firebase من "firebase";

متغير config = {
    apiKey: "YOUR_API_KEY",
    projectId: "sad-lang-firebase",
    databaseURL: "https://sad-lang-firebase.firebaseio.com",
    storageBucket: "sad-lang-firebase.appspot.com",
    messagingSenderId: "YOUR_SENDER_ID",
    appId: "YOUR_APP_ID"
};

متغير app = FirebaseApp(config);
```

---

## 💡 Examples / أمثلة

### Example 1: Firestore Operations

```sad
// Get collection
متغير packages = app.firestore.collection("packages");

// Create document
متغير newPkg = {
    name: "math-utils",
    version: "1.0.0",
    author: "Sad Team"
};

متغير docId = packages.add(newPkg);

// Read document
متغير pkg = packages.get(docId);

// Update document
packages.update(docId, {version: "1.0.1"});

// Delete document
packages.delete(docId);

// Query documents
متغير results = packages.where("author", "==", "Sad Team");
```

### Example 2: Storage Operations

```sad
// Get storage reference
متغير storage = app.storage.ref("packages");

// Upload file
storage.upload("C:\\local\\math-utils-1.0.0.tar.gz");

// Get download URL
متغير url = storage.get_download_url();

// Download file
storage.download("C:\\downloads\\math-utils.tar.gz");

// Get metadata
متغير meta = storage.get_metadata();

// Delete file
storage.delete();
```

### Example 3: Authentication

```sad
// Sign up
متغير user = app.auth.sign_up("dev@sadlang.org", "password123");

// Sign in
متغير user = app.auth.sign_in("dev@sadlang.org", "password123");

// Get current user
متغير currentUser = app.auth.get_current_user();

// Update profile
app.auth.update_profile("Display Name", "https://example.com/photo.jpg");

// Reset password
app.auth.reset_password("dev@sadlang.org");

// Sign out
app.auth.sign_out();
```

### Example 4: Realtime Database

```sad
// Get reference
متغير statsRef = app.database.ref("stats");

// Set value
statsRef.set({
    totalPackages: 150,
    totalDownloads: 5000
});

// Get value
متغير stats = statsRef.get();

// Update value
statsRef.update({totalDownloads: 5100});

// Listen for changes
statsRef.on_change(دالة(value) {
    اطبع("Stats changed: " + value);
});

// Remove value
statsRef.remove();
```

### Example 5: Cloud Functions

```sad
// Call function
متغير result = app.functions.call("calculateStats", {
    startDate: "2024-01-01",
    endDate: "2024-12-31"
});

// Call with multiple parameters
app.functions.call("sendEmail", {
    to: "user@example.com",
    subject: "Welcome",
    template: "welcome"
});
```

### Example 6: Package Registry

```sad
استورد firebase من "firebase";

متغير app = FirebaseApp(config);
متغير registry = PackageRegistry(app);

// Publish package
متغير metadata = {
    name: "math-utils",
    version: "1.0.0",
    description: "Math utilities",
    author: "Sad Team",
    license: "MIT"
};

registry.publish("C:\\packages\\math-utils-1.0.0.tar.gz", metadata);

// Search packages
متغير results = registry.search("math");

// Get package info
متغير info = registry.get_package_info("math-utils", "1.0.0");

// Download package
registry.download_package("math-utils", "1.0.0", "C:\\local\\");

// List all packages
متغير allPkgs = registry.list_packages();

// Get trending
متغير trending = registry.get_trending(7);

// Get user packages
متغير userPkgs = registry.get_user_packages("sad-team");
```

---

## ✅ Best Practices / أفضل الممارسات

### 1. Security / الأمان

```sad
// ✅ DO: Validate input
دالة validate_package(pkg: قاموس) -> منطقي {
    اذا (pkg["name"] == "") {
        اطبع("Error: Package name required");
        ارجع خطأ;
    }
    
    اذا (pkg["version"] == "") {
        اطبع("Error: Package version required");
        ارجع خطأ;
    }
    
    ارجع صح;
}

// ❌ DON'T: Skip validation
```

### 2. Error Handling / معالجة الأخطاء

```sad
// ✅ DO: Handle errors
حاول {
    متغير user = app.auth.sign_in(email, password);
} خطأ {
    اطبع("Login failed: " + error);
    ارجع null;
}

// ❌ DON'T: Ignore errors
```

### 3. Performance / الأداء

```sad
// ✅ DO: Use collections efficiently
متغير packages = app.firestore.collection("packages");
متغير active = packages.where("status", "==", "published");

// ❌ DON'T: Load all documents
```

### 4. Rate Limiting / تحديد المعدل

```sad
// ✅ DO: Implement rate limiting
متغير lastRequest = 0;
متغير minInterval = 1000; // 1 second

دالة safe_api_call(func: دالة) {
    متغير now = current_timestamp();
    اذا (now - lastRequest < minInterval) {
        اطبع("Rate limit: Please wait");
        ارجع false;
    }
    
    lastRequest = now;
    ارجع func();
}
```

---

## 🔧 Deployment / النشر

### Deploy to Firebase Hosting

```powershell
# 1. Build your Sad application
sad build

# 2. Deploy to Firebase
firebase deploy --only hosting

# 3. View deployment
firebase open hosting
```

### Deploy Functions

```powershell
# 1. Create functions directory
mkdir functions

# 2. Deploy functions
firebase deploy --only functions

# 3. View function logs
firebase functions:log
```

---

## 🐛 Troubleshooting / استكشاف الأخطاء

### Issue 1: Connection Failed

```
Error: CURL request failed: Could not resolve hostname
```

**Solution:**
```powershell
# Check internet connection
ping firebase.google.com

# Check firewall settings
# Check VPN connection
```

### Issue 2: Authentication Failed

```
Error: Invalid authentication credentials
```

**Solution:**
```powershell
# Verify API key in config
# Check Firebase Console > Authentication settings
# Ensure email/password auth is enabled
```

### Issue 3: Storage Upload Failed

```
Error: Permission denied: User does not have permission to write
```

**Solution:**
```javascript
// Update Firebase Storage Rules
rules_version = '2';
service firebase.storage {
  match /b/{bucket}/o {
    match /packages/{allPaths=**} {
      allow read: if true;
      allow write: if request.auth != null;
    }
  }
}
```

### Issue 4: Firestore Quota Exceeded

```
Error: Quota exceeded for Firestore
```

**Solution:**
```powershell
# 1. Check Firebase Console > Usage
# 2. Upgrade plan if needed
# 3. Optimize queries to reduce reads
# 4. Implement caching
```

---

## 📊 Firestore Structure / هيكل Firestore

```
sad-lang-firebase
└── Collections
    ├── packages
    │   ├── Documents
    │   │   ├── name (string)
    │   │   ├── version (string)
    │   │   ├── description (string)
    │   │   ├── author (string)
    │   │   ├── downloads (number)
    │   │   ├── rating (number)
    │   │   ├── archiveUrl (string)
    │   │   ├── publishedAt (timestamp)
    │   │   └── updatedAt (timestamp)
    │   └── Indexes
    │       ├── author ASC
    │       └── publishedAt DESC
    │
    ├── users
    │   ├── Documents
    │   │   ├── email (string)
    │   │   ├── displayName (string)
    │   │   ├── photoURL (string)
    │   │   ├── packages (array)
    │   │   ├── createdAt (timestamp)
    │   │   └── lastLogin (timestamp)
    │   └── Indexes
    │       └── email (unique)
    │
    └── downloads
        ├── Documents
        │   ├── packageId (string)
        │   ├── userId (string)
        │   ├── timestamp (timestamp)
        │   └── ipAddress (string)
        └── Indexes
            ├── packageId ASC, timestamp DESC
            └── userId ASC, timestamp DESC
```

---

## 📈 Performance Metrics / مقاييس الأداء

```sad
// Track performance
دالة track_performance(operation: نص) {
    متغير start = current_timestamp();
    
    // ... operation ...
    
    متغير duration = current_timestamp() - start;
    اطبع("Operation: " + operation);
    اطبع("Duration: " + duration + "ms");
}
```

---

## 🔗 Related Documentation / التوثيق المرتبط

- [Firebase Documentation](https://firebase.google.com/docs)
- [Sad Language Documentation](https://sadlang.org)
- [Package Registry Guide](../docs/PKG_REGISTRY_INTEGRATION.md)
- [Firebase Security Rules](https://firebase.google.com/docs/rules)

---

## 📝 License / الترخيص

This example is licensed under MIT License.

---

**الحمد لله رب العالمين** 🙏

Last Updated: December 11, 2025
