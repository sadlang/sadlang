// بسم الله الرحمن الرحيم
/**
 * @file main.sad
 * @description مثال كامل على استخدام Firebase مع لغة ص
 * Complete example of using Firebase with Sad Language
 */

استورد firebase من "firebase"؛

// ============================================================================
// إعدادات Firebase - Firebase Configuration
// ============================================================================

متغير config = {
    apiKey: "AIzaSyDxxxxxxxxxxxxxxxxxxx",
    projectId: "sad-lang-firebase",
    databaseURL: "https://sad-lang-firebase.firebaseio.com",
    storageBucket: "sad-lang-firebase.appspot.com",
    messagingSenderId: "123456789",
    appId: "1:123456789:web:xxxxxxxxxxxxxxxx"
}؛

// ============================================================================
// التطبيق الرئيسي - Main Application
// ============================================================================

دالة demo_firestore() {
    اطبع("\n📚 FIRESTORE DEMO - عرض Firestore")؛
    اطبع("=====================================\n")؛
    
    // الحصول على مرجع المجموعة
    متغير packages = app.firestore.collection("packages")؛
    
    // 1. إضافة حزمة جديدة
    اطبع("1. Adding new package...")؛
    متغير packageData = {
        name: "math-utils",
        version: "1.0.0",
        description: "Mathematical utilities",
        author: "Sad Team",
        downloads: 150,
        createdAt: current_timestamp()
    }؛
    
    متغير docId = packages.add(packageData)؛
    اطبع("   ✅ Package added: " + docId + "\n")؛
    
    // 2. الحصول على حزمة
    اطبع("2. Getting package...")؛
    متغير doc = packages.get(docId)؛
    اطبع("   ✅ Package retrieved: " + doc.id + "\n")؛
    
    // 3. تحديث حزمة
    اطبع("3. Updating package...")؛
    متغير updates = {
        downloads: 200,
        rating: 4.5
    }؛
    packages.update(docId, updates)؛
    اطبع("   ✅ Package updated\n")؛
    
    // 4. البحث عن حزم
    اطبع("4. Searching packages...")؛
    متغير results = packages.where("author", "==", "Sad Team")؛
    اطبع("   ✅ Found " + results.length + " packages\n")؛
    
    // 5. حذف حزمة
    اطبع("5. Deleting package...")؛
    packages.delete(docId)؛
    اطبع("   ✅ Package deleted\n")؛
}

دالة demo_storage() {
    اطبع("\n💾 STORAGE DEMO - عرض التخزين السحابي")؛
    اطبع("=====================================\n")؛
    
    // الحصول على مرجع التخزين
    متغير packagesStorage = app.storage.ref("packages")؛
    
    // 1. رفع ملف
    اطبع("1. Uploading package file...")؛
    متغير uploadPath = packagesStorage.upload("C:\\local\\math-utils-1.0.0.tar.gz")؛
    اطبع("   ✅ File uploaded: " + uploadPath + "\n")؛
    
    // 2. الحصول على رابط التحميل
    اطبع("2. Getting download URL...")؛
    متغير downloadUrl = packagesStorage.get_download_url()؛
    اطبع("   ✅ URL: " + downloadUrl + "\n")؛
    
    // 3. الحصول على بيانات الملف
    اطبع("3. Getting file metadata...")؛
    متغير metadata = packagesStorage.get_metadata()؛
    اطبع("   ✅ Metadata retrieved\n")؛
    
    // 4. تحميل ملف
    اطبع("4. Downloading file...")؛
    packagesStorage.download("C:\\downloads\\math-utils.tar.gz")؛
    اطبع("   ✅ File downloaded\n")؛
    
    // 5. حذف ملف
    اطبع("5. Deleting file...")؛
    packagesStorage.delete()؛
    اطبع("   ✅ File deleted\n")؛
}

دالة demo_auth() {
    اطبع("\n🔐 AUTHENTICATION DEMO - عرض المصادقة")؛
    اطبع("=====================================\n")؛
    
    // 1. التسجيل
    اطبع("1. Creating new account...")؛
    متغير user = app.auth.sign_up("developer@sadlang.org", "SecurePass123")؛
    
    اذا (user != null) {
        اطبع("   ✅ User created:")؛
        اطبع("      - UID: " + user.uid)؛
        اطبع("      - Email: " + user.email)؛
        اطبع("      - Verified: " + user.emailVerified + "\n")؛
    }
    
    // 2. تحديث الملف الشخصي
    اطبع("2. Updating profile...")؛
    app.auth.update_profile("Sad Developer", "https://example.com/photo.jpg")؛
    اطبع("   ✅ Profile updated\n")؛
    
    // 3. الحصول على المستخدم الحالي
    اطبع("3. Getting current user...")؛
    متغير currentUser = app.auth.get_current_user()؛
    
    اذا (currentUser != null) {
        اطبع("   ✅ Current user:")؛
        اطبع("      - Name: " + currentUser.displayName)؛
        اطبع("      - Email: " + currentUser.email + "\n")؛
    }
    
    // 4. إعادة تعيين كلمة المرور
    اطبع("4. Resetting password...")؛
    app.auth.reset_password("developer@sadlang.org")؛
    اطبع("   ✅ Reset email sent\n")؛
    
    // 5. تسجيل الخروج
    اطبع("5. Signing out...")؛
    app.auth.sign_out()؛
    اطبع("   ✅ User signed out\n")؛
}

دالة demo_realtime() {
    اطبع("\n🔄 REALTIME DATABASE DEMO - قاعدة البيانات الفورية")؛
    اطبع("=================================================\n")؛
    
    // الحصول على مرجع قاعدة البيانات
    متغير statsRef = app.database.ref("stats")؛
    
    // 1. تعيين قيمة
    اطبع("1. Setting stats...")؛
    متغير statsData = {
        totalPackages: 150,
        totalDownloads: 5000,
        activeUsers: 42,
        lastUpdate: current_timestamp()
    }؛
    statsRef.set(statsData)؛
    اطبع("   ✅ Stats set\n")؛
    
    // 2. الحصول على قيمة
    اطبع("2. Getting stats...")؛
    متغير stats = statsRef.get()؛
    اطبع("   ✅ Stats retrieved\n")؛
    
    // 3. تحديث قيمة
    اطبع("3. Updating stats...")؛
    statsRef.update({totalDownloads: 5100})؛
    اطبع("   ✅ Stats updated\n")؛
    
    // 4. الاستماع للتغييرات
    اطبع("4. Listening for changes...")؛
    statsRef.on_change(دالة(value) {
        اطبع("   📊 Stats changed: " + value)؛
    })؛
    اطبع("   ✅ Listener registered\n")؛
    
    // 5. حذف قيمة
    اطبع("5. Removing stats...")؛
    statsRef.remove()؛
    اطبع("   ✅ Stats removed\n")؛
}

دالة demo_functions() {
    اطبع("\n☁️  CLOUD FUNCTIONS DEMO - الدوال السحابية")؛
    اطبع("==========================================\n")؛
    
    // 1. حساب إحصائيات
    اطبع("1. Calling calculateStats function...")؛
    متغير result = app.functions.call("calculateStats", {
        startDate: "2024-01-01",
        endDate: "2024-12-31"
    })؛
    اطبع("   ✅ Function executed\n")؛
    
    // 2. إنشاء نسخة احتياطية
    اطبع("2. Calling backupDatabase function...")؛
    app.functions.call("backupDatabase", {
        collections: ["packages", "users"]
    })؛
    اطبع("   ✅ Backup started\n")؛
    
    // 3. إرسال بريد إلكتروني
    اطبع("3. Calling sendEmail function...")؛
    app.functions.call("sendEmail", {
        to: "user@example.com",
        subject: "Welcome to Sad Package Registry",
        template: "welcome"
    })؛
    اطبع("   ✅ Email sent\n")؛
}

دالة package_management_demo() {
    اطبع("\n📦 PACKAGE MANAGEMENT - إدارة الحزم")؛
    اطبع("====================================\n")؛
    
    متغير packages = app.firestore.collection("packages")؛
    متغير storage = app.storage.ref("packages")؛
    
    // 1. نشر حزمة جديدة
    اطبع("1. Publishing new package...")؛
    متغير packageInfo = {
        name: "http-client",
        version: "2.1.0",
        description: "HTTP client library for Sad Language",
        author: "Sad Team",
        license: "MIT",
        repository: "https://github.com/sad-lang/http-client",
        dependencies: {
            "json": "^1.0.0"
        },
        downloads: 0,
        rating: 0,
        createdAt: current_timestamp()
    }؛
    
    متغير pkgId = packages.add(packageInfo)؛
    اطبع("   ✅ Package created: " + pkgId + "\n")؛
    
    // 2. رفع ملف الحزمة
    اطبع("2. Uploading package archive...")؛
    storage.upload("C:\\builds\\http-client-2.1.0.tar.gz")؛
    اطبع("   ✅ Archive uploaded\n")؛
    
    // 3. تحديث معلومات الحزمة
    اطبع("3. Updating package info...")؛
    packages.update(pkgId, {
        archiveUrl: storage.get_download_url(),
        status: "published"
    })؛
    اطبع("   ✅ Package published\n")؛
    
    // 4. البحث عن الحزمة
    اطبع("4. Searching for package...")؛
    متغير found = packages.where("name", "==", "http-client")؛
    اطبع("   ✅ Found " + found.length + " results\n")؛
}

دالة main() {
    اطبع("بسم الله الرحمن الرحيم\n")؛
    اطبع("╔════════════════════════════════════════════════════════════╗")؛
    اطبع("║       Firebase Integration with Sad Language               ║")؛
    اطبع("║       تكامل Firebase مع لغة ص                            ║")؛
    اطبع("╚════════════════════════════════════════════════════════════╝\n")؛
    
    // 1. Initialize Firebase
    اطبع("🔥 INITIALIZING FIREBASE - تهيئة Firebase")؛
    اطبع("=========================================\n")؛
    
    متغير app = FirebaseApp(config)؛
    
    // 2. Run demos
    demo_firestore()؛
    demo_storage()؛
    demo_auth()؛
    demo_realtime()؛
    demo_functions()؛
    
    // 3. Complete example
    package_management_demo()؛
    
    // Summary
    اطبع("\n╔════════════════════════════════════════════════════════════╗")؛
    اطبع("║                  SUMMARY - الملخص                          ║")؛
    اطبع("╚════════════════════════════════════════════════════════════╝\n")؛
    
    اطبع("✅ Firestore:         Database operations complete")؛
    اطبع("✅ Storage:           File operations complete")؛
    اطبع("✅ Authentication:    Auth operations complete")؛
    اطبع("✅ Realtime DB:       Realtime operations complete")؛
    اطبع("✅ Cloud Functions:   Function calls complete")؛
    اطبع("✅ Package Mgmt:      Package management complete\n")؛
    
    اطبع("الحمد لله رب العالمين")؛
}

// تشغيل البرنامج
main()؛
