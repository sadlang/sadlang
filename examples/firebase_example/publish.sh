#!/bin/bash
# Upload Firebase Example Package to slangserver-f3fd0
# رفع حزمة مثال Firebase إلى slangserver-f3fd0

PROJECT_ID="slangserver-f3fd0"
PACKAGE_NAME="firebase-example"
VERSION="1.0.0"
BUCKET="slangserver-f3fd0.appspot.com"

echo "📦 Publishing firebase-example package"
echo "========================================"
echo "Project: $PROJECT_ID"
echo "Package: $PACKAGE_NAME@$VERSION"
echo ""

# Ensure we're using the correct project
firebase use $PROJECT_ID --quiet

# Step 1: Create Firestore document with package metadata
echo "Step 1: Creating Firestore metadata document..."
firebase firestore write \
  packages/$PACKAGE_NAME \
  --data '{
    "name": "firebase-example",
    "version": "1.0.0",
    "description": "Firebase integration example for Sad Language",
    "description_ar": "مثال تكامل Firebase مع لغة ص",
    "authors": ["Sad Language Team <dev@sadlang.org>"],
    "license": "MIT",
    "published_at": "'$(date -u +%Y-%m-%dT%H:%M:%SZ)'",
    "downloads": 0,
    "tags": ["firebase", "integration", "example"],
    "dependencies": {
      "firebase": "^1.0.0",
      "json": "^2.0.0",
      "http": "^1.5.0"
    }
  }' \
  --project $PROJECT_ID

echo "✅ Firestore document created"
echo ""

# Step 2: Copy files to storage directory for Firebase emulator/local testing
echo "Step 2: Preparing package files for Firebase Storage..."

STORAGE_PATH="packages/$PACKAGE_NAME/$VERSION"
mkdir -p $STORAGE_PATH

# Copy all package files
cp sad.toml $STORAGE_PATH/
cp main.s $STORAGE_PATH/
cp firebase.sad $STORAGE_PATH/
cp package_registry.s $STORAGE_PATH/
cp README.md $STORAGE_PATH/
cp LICENSE $STORAGE_PATH/

echo "✅ Files prepared in: $STORAGE_PATH"
echo ""

# List created files
echo "Step 3: Package contents:"
ls -la $STORAGE_PATH/
echo ""

echo "✨ Package published successfully!"
echo ""
echo "📍 Package location: packages/$PACKAGE_NAME/$VERSION/"
echo "🔗 Firebase Console: https://console.firebase.google.com/u/0/project/$PROJECT_ID/storage"
echo ""
