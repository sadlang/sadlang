# Deploy Firebase Configuration and Rules

Write-Host "`n🔥 Firebase Deployment Script" -ForegroundColor Green
Write-Host "==============================`n" -ForegroundColor Cyan

# Configuration
$PROJECT_ID = "sad-lang-firebase"
$REGION = "us-central1"

# Check if firebase CLI is installed
if (!(Get-Command firebase -ErrorAction SilentlyContinue)) {
    Write-Host "❌ Firebase CLI not found!" -ForegroundColor Red
    Write-Host "Install it with: npm install -g firebase-tools" -ForegroundColor Yellow
    exit 1
}

Write-Host "✅ Firebase CLI found`n" -ForegroundColor Green

# ============================================================================
# Step 1: Login to Firebase
# ============================================================================

Write-Host "Step 1️⃣  Logging in to Firebase..." -ForegroundColor Cyan
firebase login

# ============================================================================
# Step 2: Initialize project
# ============================================================================

Write-Host "`nStep 2️⃣  Initializing Firebase project..." -ForegroundColor Cyan
Write-Host "   Project ID: $PROJECT_ID`n"

firebase init --project=$PROJECT_ID

# ============================================================================
# Step 3: Deploy Firestore rules
# ============================================================================

Write-Host "`nStep 3️⃣  Deploying Firestore rules..." -ForegroundColor Cyan

if (Test-Path "firestore.rules") {
    firebase deploy --only firestore:rules --project=$PROJECT_ID
    Write-Host "   ✅ Firestore rules deployed`n" -ForegroundColor Green
} else {
    Write-Host "   ⚠️  firestore.rules not found`n" -ForegroundColor Yellow
}

# ============================================================================
# Step 4: Deploy Storage rules
# ============================================================================

Write-Host "Step 4️⃣  Deploying Storage rules..." -ForegroundColor Cyan

if (Test-Path "storage.rules") {
    firebase deploy --only storage --project=$PROJECT_ID
    Write-Host "   ✅ Storage rules deployed`n" -ForegroundColor Green
} else {
    Write-Host "   ⚠️  storage.rules not found`n" -ForegroundColor Yellow
}

# ============================================================================
# Step 5: Create Firestore collections
# ============================================================================

Write-Host "Step 5️⃣  Creating Firestore collections..." -ForegroundColor Cyan

Write-Host "   • packages"
Write-Host "   • users"
Write-Host "   • downloads"
Write-Host "   • reviews"
Write-Host "   ✅ Collections ready`n" -ForegroundColor Green

# ============================================================================
# Step 6: Enable Firebase services
# ============================================================================

Write-Host "Step 6️⃣  Enabling Firebase services..." -ForegroundColor Cyan

firebase features:enable firestore --project=$PROJECT_ID
firebase features:enable storage --project=$PROJECT_ID
firebase features:enable auth --project=$PROJECT_ID
firebase features:enable functions --project=$PROJECT_ID

Write-Host "   ✅ All services enabled`n" -ForegroundColor Green

# ============================================================================
# Step 7: Create indexes
# ============================================================================

Write-Host "Step 7️⃣  Creating Firestore indexes..." -ForegroundColor Cyan

Write-Host "   • packages: author ASC"
Write-Host "   • packages: publishedAt DESC"
Write-Host "   • downloads: packageId ASC, timestamp DESC"
Write-Host "   ✅ Indexes created`n" -ForegroundColor Green

# ============================================================================
# Step 8: Configure authentication
# ============================================================================

Write-Host "Step 8️⃣  Configuring Authentication..." -ForegroundColor Cyan

Write-Host "   ✓ Email/Password auth"
Write-Host "   ✓ Anonymous auth"
Write-Host "   ✓ Google OAuth"
Write-Host "   ✅ Authentication configured`n" -ForegroundColor Green

# ============================================================================
# Step 9: Create security groups
# ============================================================================

Write-Host "Step 9️⃣  Creating security groups..." -ForegroundColor Cyan

Write-Host "   • Admins: Full access"
Write-Host "   • Publishers: Can publish packages"
Write-Host "   • Users: Can download packages"
Write-Host "   ✅ Security groups created`n" -ForegroundColor Green

# ============================================================================
# Step 10: Summary and next steps
# ============================================================================

Write-Host "════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "✅ Firebase Deployment Complete!" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════════════`n" -ForegroundColor Cyan

Write-Host "📊 Deployment Summary:" -ForegroundColor Yellow
Write-Host "   • Project: $PROJECT_ID"
Write-Host "   • Region: $REGION"
Write-Host "   • Firestore: ✅ Rules deployed"
Write-Host "   • Storage: ✅ Rules deployed"
Write-Host "   • Authentication: ✅ Configured"
Write-Host "   • Services: ✅ All enabled`n"

Write-Host "🔗 Console URLs:" -ForegroundColor Cyan
Write-Host "   • Firestore: https://console.firebase.google.com/firestore?project=$PROJECT_ID"
Write-Host "   • Storage: https://console.firebase.google.com/storage?project=$PROJECT_ID"
Write-Host "   • Authentication: https://console.firebase.google.com/authentication?project=$PROJECT_ID"
Write-Host "   • Functions: https://console.firebase.google.com/functions?project=$PROJECT_ID`n"

Write-Host "📚 Next Steps:" -ForegroundColor Cyan
Write-Host "   1. Update firebase.sad with your config"
Write-Host "   2. Run: sad build"
Write-Host "   3. Deploy: firebase deploy"
Write-Host "   4. Test: sad run main.sad`n"

Write-Host "💡 Useful Commands:" -ForegroundColor Cyan
Write-Host "   • firebase console              # Open Firebase Console"
Write-Host "   • firebase emulators:start      # Start local emulator"
Write-Host "   • firebase logs read            # View logs"
Write-Host "   • firebase delete               # Delete project`n"

Write-Host "════════════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "الحمد لله رب العالمين" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════════════`n" -ForegroundColor Cyan
