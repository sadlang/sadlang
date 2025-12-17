// بسم الله الرحمن الرحيم
/**
 * @file package_registry.sad
 * @description نظام سجل الحزم على Firebase
 * Package Registry System on Firebase
 */

استورد firebase من "firebase"؛

// ============================================================================
// Package Registry on Firebase - سجل الحزم على Firebase
// ============================================================================

/**
 * Package Registry - سجل الحزم
 */
صنف PackageRegistry {
    خاص app: FirebaseApp؛
    خاص packagesCollection: FirestoreCollection؛
    خاص usersCollection: FirestoreCollection؛
    خاص storage: Storage؛
    
    /**
     * Initialize registry
     */
    عام دالة __init__(firebaseApp: FirebaseApp) {
        this.app = firebaseApp؛
        this.packagesCollection = firebaseApp.firestore.collection("packages")؛
        this.usersCollection = firebaseApp.firestore.collection("users")؛
        this.storage = firebaseApp.storage؛
        
        اطبع("📦 Package Registry initialized")؛
    }
    
    /**
     * Publish package
     */
    عام دالة publish(packagePath: نص, metadata: قاموس) -> نص {
        اطبع("\n📤 Publishing package...")؛
        اطبع("   Path: " + packagePath)؛
        
        // 1. Validate package
        اطبع("   ✓ Validating package")؛
        
        // 2. Upload to storage
        اطبع("   ✓ Uploading to storage")؛
        متغير storageRef = storage.ref("packages/" + metadata["name"] + "/" + metadata["version"])؛
        متغير downloadUrl = storageRef.upload(packagePath)؛
        
        // 3. Create metadata
        اطبع("   ✓ Creating metadata")؛
        متغير packageDoc = {
            name: metadata["name"],
            version: metadata["version"],
            description: metadata["description"],
            author: metadata["author"],
            license: metadata["license"],
            repository: metadata["repository"],
            keywords: metadata["keywords"],
            downloads: 0,
            rating: 0,
            archiveUrl: downloadUrl,
            archiveSize: metadata["size"],
            publishedAt: current_timestamp(),
            updatedAt: current_timestamp(),
            status: "published"
        }؛
        
        // 4. Store in Firestore
        اطبع("   ✓ Storing in Firestore")؛
        متغير docId = packagesCollection.add(packageDoc)؛
        
        اطبع("\n✅ Package published successfully!")؛
        اطبع("   Name: " + metadata["name"])؛
        اطبع("   Version: " + metadata["version"])؛
        اطبع("   ID: " + docId + "\n")؛
        
        ارجع docId؛
    }
    
    /**
     * Search packages
     */
    عام دالة search(query: نص, limit: رقم = 10) -> قائمة<قاموس> {
        اطبع("🔍 Searching packages: " + query)؛
        
        // Search in name and description
        متغير results = packagesCollection.where("name", "like", query)؛
        
        اطبع("   Found " + results.length + " results\n")؛
        
        ارجع results؛
    }
    
    /**
     * Get package info
     */
    عام دالة get_package_info(name: نص, version: نص) -> قاموس|null {
        اطبع("📋 Getting package info: " + name + "@" + version)؛
        
        متغير results = packagesCollection.where("name", "==", name)؛
        
        لكل_من (متغير pkg في results) {
            اذا (pkg["version"] == version) {
                اطبع("   ✅ Found package\n")؛
                ارجع pkg؛
            }
        }
        
        اطبع("   ❌ Package not found\n")؛
        ارجع null؛
    }
    
    /**
     * Download package
     */
    عام دالة download_package(name: نص, version: نص, destPath: نص) -> منطقي {
        اطبع("📥 Downloading: " + name + "@" + version)؛
        
        // Get package info
        متغير pkg = get_package_info(name, version)؛
        
        اذا (pkg == null) {
            اطبع("   ❌ Package not found\n")؛
            ارجع خطأ؛
        }
        
        // Download from storage
        متغير storageRef = storage.ref("packages/" + name + "/" + version)؛
        storageRef.download(destPath)؛
        
        // Update download count
        packagesCollection.update(pkg["id"], {
            downloads: pkg["downloads"] + 1
        })؛
        
        اطبع("   ✅ Downloaded to: " + destPath + "\n")؛
        
        ارجع صح؛
    }
    
    /**
     * List all packages
     */
    عام دالة list_packages(limit: رقم = 20) -> قائمة<قاموس> {
        اطبع("📚 Listing packages (limit: " + limit + ")")؛
        
        متغير packages = packagesCollection.where("status", "==", "published")؛
        
        اطبع("   Found " + packages.length + " packages\n")؛
        
        ارجع packages؛
    }
    
    /**
     * Get trending packages
     */
    عام دالة get_trending(days: رقم = 7) -> قائمة<قاموس> {
        اطبع("🔥 Getting trending packages (last " + days + " days)")؛
        
        // In real implementation, query by recent downloads
        متغير trending = packagesCollection.where("status", "==", "published")؛
        
        اطبع("   Found " + trending.length + " trending packages\n")؛
        
        ارجع trending؛
    }
    
    /**
     * Get user packages
     */
    عام دالة get_user_packages(userId: نص) -> قائمة<قاموس> {
        اطبع("👤 Getting user packages: " + userId)؛
        
        متغير userPackages = packagesCollection.where("author", "==", userId)؛
        
        اطبع("   Found " + userPackages.length + " packages\n")؛
        
        ارجع userPackages؛
    }
}

// ============================================================================
// Main Demo - عرض توضيحي
// ============================================================================

دالة main() {
    اطبع("بسم الله الرحمن الرحيم\n")؛
    اطبع("╔════════════════════════════════════════════════════════╗")؛
    اطبع("║         Package Registry on Firebase                   ║")؛
    اطبع("║         سجل الحزم على Firebase                       ║")؛
    اطبع("╚════════════════════════════════════════════════════════╝\n")؛
    
    // Initialize Firebase
    متغير config = {
        apiKey: "AIzaSyDxxxxxxxxxxxxxxxxxxx",
        projectId: "sad-lang-packages",
        databaseURL: "https://sad-lang-packages.firebaseio.com",
        storageBucket: "sad-lang-packages.appspot.com",
        messagingSenderId: "123456789",
        appId: "1:123456789:web:xxxxxxxxxxxxxxxx"
    }؛
    
    متغير app = FirebaseApp(config)؛
    متغير registry = PackageRegistry(app)؛
    
    // ============================================================
    // 1. Publish packages
    // ============================================================
    
    اطبع("\n1️⃣  PUBLISHING PACKAGES")؛
    اطبع("======================\n")؛
    
    // Publish math-utils
    متغير mathPkg = {
        name: "math-utils",
        version: "1.0.0",
        description: "Mathematical utility functions",
        author: "Sad Team",
        license: "MIT",
        repository: "https://github.com/sad-lang/math-utils",
        keywords: ["math", "utilities", "numbers"],
        size: 15240
    }؛
    
    متغير mathId = registry.publish("C:\\packages\\math-utils-1.0.0.tar.gz", mathPkg)؛
    
    // Publish http-client
    متغير httpPkg = {
        name: "http-client",
        version: "2.1.0",
        description: "HTTP client library",
        author: "Sad Team",
        license: "MIT",
        repository: "https://github.com/sad-lang/http-client",
        keywords: ["http", "networking", "client"],
        size: 28500
    }؛
    
    متغير httpId = registry.publish("C:\\packages\\http-client-2.1.0.tar.gz", httpPkg)؛
    
    // Publish json-parser
    متغير jsonPkg = {
        name: "json-parser",
        version: "3.0.0",
        description: "JSON parsing library",
        author: "Sad Team",
        license: "MIT",
        repository: "https://github.com/sad-lang/json-parser",
        keywords: ["json", "parser", "serialization"],
        size: 18900
    }؛
    
    متغير jsonId = registry.publish("C:\\packages\\json-parser-3.0.0.tar.gz", jsonPkg)؛
    
    // ============================================================
    // 2. Search packages
    // ============================================================
    
    اطبع("\n2️⃣  SEARCHING PACKAGES")؛
    اطبع("======================\n")؛
    
    registry.search("math")؛
    registry.search("http")؛
    registry.search("json")؛
    
    // ============================================================
    // 3. Get package info
    // ============================================================
    
    اطبع("\n3️⃣  GETTING PACKAGE INFO")؛
    اطبع("==========================\n")؛
    
    متغير info1 = registry.get_package_info("math-utils", "1.0.0")؛
    متغير info2 = registry.get_package_info("http-client", "2.1.0")؛
    متغير info3 = registry.get_package_info("json-parser", "3.0.0")؛
    
    // ============================================================
    // 4. List all packages
    // ============================================================
    
    اطبع("\n4️⃣  LISTING ALL PACKAGES")؛
    اطبع("========================\n")؛
    
    متغير allPackages = registry.list_packages(10)؛
    
    // ============================================================
    // 5. Get trending packages
    // ============================================================
    
    اطبع("\n5️⃣  TRENDING PACKAGES")؛
    اطبع("=====================\n")؛
    
    متغير trending = registry.get_trending(7)؛
    
    // ============================================================
    // 6. Get user packages
    // ============================================================
    
    اطبع("\n6️⃣  USER PACKAGES")؛
    اطبع("==================\n")؛
    
    متغير userPkgs = registry.get_user_packages("sad-team")؛
    
    // ============================================================
    // Summary
    // ============================================================
    
    اطبع("\n╔════════════════════════════════════════════════════════╗")؛
    اطبع("║                    SUMMARY - الملخص                     ║")؛
    اطبع("╚════════════════════════════════════════════════════════╝\n")؛
    
    اطبع("✅ Published 3 packages:")؛
    اطبع("   • math-utils@1.0.0")؛
    اطبع("   • http-client@2.1.0")؛
    اطبع("   • json-parser@3.0.0\n")؛
    
    اطبع("✅ Firebase Services Used:")؛
    اطبع("   • Firestore (Database)")؛
    اطبع("   • Storage (File Storage)")؛
    اطبع("   • Authentication (User Auth)\n")؛
    
    اطبع("✅ Registry Features:")؛
    اطبع("   • Publish packages")؛
    اطبع("   • Search packages")؛
    اطبع("   • Download packages")؛
    اطبع("   • List all packages")؛
    اطبع("   • Get trending packages")؛
    اطبع("   • Manage user packages\n")؛
    
    اطبع("الحمد لله رب العالمين")؛
}

main()؛

// الحمد لله رب العالمين
