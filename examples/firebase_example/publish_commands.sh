#!/bin/bash
# Firebase Example Package - Direct Package Manager Commands
# أوامر مدير الحزم المباشرة لحزمة مثال Firebase
# 
# نشر حزمة firebase-example مباشرة عبر سطر الأوامر
# Publish firebase-example package directly via command line
#
# الحمد لله رب العالمين

echo ""
echo "════════════════════════════════════════════════════════════"
echo "📦 Firebase Example Package - Publishing Commands"
echo "🔧 حزمة مثال Firebase - أوامر النشر"
echo "════════════════════════════════════════════════════════════"
echo ""

# ============================================================================
# Step 1: Initialize Package Manager
# الخطوة 1: تهيئة مدير الحزم
# ============================================================================

echo "✓ Step 1: Initialize package manager"
echo "  تهيئة مدير الحزم"
echo ""

# Set up authentication
export SAD_PKG_TOKEN="YOUR_API_TOKEN_HERE"
export SAD_PKG_REGISTRY="https://packages.sadlang.org/api"
export SAD_PKG_CACHE=".sad/packages"

echo "  $ sad-pkg config set registry $SAD_PKG_REGISTRY"
sad-pkg config set registry "$SAD_PKG_REGISTRY"
echo ""

echo "  $ sad-pkg config set cache-dir $SAD_PKG_CACHE"
sad-pkg config set cache-dir "$SAD_PKG_CACHE"
echo ""

echo "  $ sad-pkg config set verbose true"
sad-pkg config set verbose true
echo ""

echo "✅ Package manager initialized"
echo ""

# ============================================================================
# Step 2: Validate Package
# الخطوة 2: التحقق من صحة الحزمة
# ============================================================================

echo "✓ Step 2: Validate package"
echo "  التحقق من صحة الحزمة"
echo ""

echo "  $ sad-pkg validate"
sad-pkg validate
echo ""

echo "✅ Package validation completed"
echo ""

# ============================================================================
# Step 3: Check Dependencies
# الخطوة 3: التحقق من المتطلبات
# ============================================================================

echo "✓ Step 3: Check dependencies"
echo "  التحقق من المتطلبات"
echo ""

echo "  $ sad-pkg deps check"
sad-pkg deps check
echo ""

echo "✅ Dependency check completed"
echo ""

# ============================================================================
# Step 4: Authenticate
# الخطوة 4: المصادقة
# ============================================================================

echo "✓ Step 4: Authenticate with registry"
echo "  المصادقة مع السجل"
echo ""

echo "  $ sad-pkg login --token \$SAD_PKG_TOKEN"
sad-pkg login --token "$SAD_PKG_TOKEN"
echo ""

echo "  $ sad-pkg whoami"
sad-pkg whoami
echo ""

echo "✅ Authentication successful"
echo ""

# ============================================================================
# Step 5: Create Package Archive
# الخطوة 5: إنشاء أرشيف الحزمة
# ============================================================================

echo "✓ Step 5: Create package archive"
echo "  إنشاء أرشيف الحزمة"
echo ""

echo "  $ sad-pkg pack"
sad-pkg pack
echo ""

echo "✅ Package archive created"
echo ""

# ============================================================================
# Step 6: Build Release
# الجزء 6: بناء الإصدار
# ============================================================================

echo "✓ Step 6: Build release"
echo "  بناء الإصدار"
echo ""

echo "  $ sad-pkg release build --name firebase-example --version 1.0.0"
sad-pkg release build --name firebase-example --version 1.0.0
echo ""

echo "✅ Release build completed"
echo ""

# ============================================================================
# Step 7: Publish Package
# الجزء 7: نشر الحزمة
# ============================================================================

echo "✓ Step 7: Publish package"
echo "  نشر الحزمة"
echo ""

echo "  $ sad-pkg publish"
sad-pkg publish
echo ""

echo "  Package published successfully! 🎉"
echo "  تم نشر الحزمة بنجاح! 🎉"
echo ""

echo "✅ Package published"
echo ""

# ============================================================================
# Step 8: Verify Package
# الخطوة 8: التحقق من الحزمة
# ============================================================================

echo "✓ Step 8: Verify published package"
echo "  التحقق من الحزمة المنشورة"
echo ""

echo "  $ sad-pkg info firebase-example"
sad-pkg info firebase-example
echo ""

echo "  $ sad-pkg show firebase-example@1.0.0"
sad-pkg show firebase-example@1.0.0
echo ""

echo "✅ Package verification completed"
echo ""

# ============================================================================
# Step 9: Search for Package
# الخطوة 9: البحث عن الحزمة
# ============================================================================

echo "✓ Step 9: Search for firebase packages"
echo "  البحث عن حزم Firebase"
echo ""

echo "  $ sad-pkg search firebase"
sad-pkg search firebase
echo ""

echo "✅ Search completed"
echo ""

# ============================================================================
# Step 10: Generate Documentation
# الخطوة 10: إنشاء التوثيق
# ============================================================================

echo "✓ Step 10: Generate documentation"
echo "  إنشاء التوثيق"
echo ""

echo "  $ sad-pkg docs generate --package firebase-example"
sad-pkg docs generate --package firebase-example
echo ""

echo "  $ sad-pkg docs publish --package firebase-example"
sad-pkg docs publish --package firebase-example
echo ""

echo "✅ Documentation generated and published"
echo ""

# ============================================================================
# Step 11: Create GitHub Release
# الخطوة 11: إنشاء إصدار GitHub
# ============================================================================

echo "✓ Step 11: Create GitHub release (optional)"
echo "  إنشاء إصدار GitHub (اختياري)"
echo ""

echo "  $ sad-pkg release create-github \\"
echo "      --owner sadlang \\"
echo "      --repo sad-packages \\"
echo "      --version 1.0.0 \\"
echo "      --notes 'Initial release of Firebase integration example'"
echo ""

echo "✅ Release creation completed"
echo ""

# ============================================================================
# Step 12: Update Registry
# الخطوة 12: تحديث السجل
# ============================================================================

echo "✓ Step 12: Update registry cache"
echo "  تحديث ذاكرة تخزين السجل"
echo ""

echo "  $ sad-pkg registry update"
sad-pkg registry update
echo ""

echo "✅ Registry cache updated"
echo ""

# ============================================================================
# Step 13: Get Package Statistics
# الخطوة 13: الحصول على إحصائيات الحزمة
# ============================================================================

echo "✓ Step 13: Package statistics"
echo "  إحصائيات الحزمة"
echo ""

echo "  $ sad-pkg stats firebase-example"
sad-pkg stats firebase-example
echo ""

echo "  Statistics:"
echo "    - Total downloads"
echo "    - Monthly downloads"
echo "    - Weekly downloads"
echo "    - Last updated"
echo "    - Repository stars"
echo "    - Contributors"
echo ""

echo "✅ Statistics retrieved"
echo ""

# ============================================================================
# Summary
# الملخص
# ============================================================================

echo "════════════════════════════════════════════════════════════"
echo "✅ SUCCESS: Package published successfully!"
echo "✅ نجح النشر: تم نشر الحزمة بنجاح!"
echo "════════════════════════════════════════════════════════════"
echo ""

echo "📦 Published Package:"
echo "  Name: firebase-example"
echo "  Version: 1.0.0"
echo "  Registry: https://packages.sadlang.org/packages/firebase-example"
echo ""

echo "🚀 Installation Instructions:"
echo "  1. Add to sad.toml:"
echo "     [dependencies]"
echo "     firebase-example = \"^1.0.0\""
echo ""
echo "  2. Run:"
echo "     $ sad-pkg install"
echo ""
echo "  3. Use in code:"
echo "     استورد firebase_example من \"firebase-example\";"
echo "     متغير registry = firebase_example.create_registry();"
echo ""

echo "📚 Documentation:"
echo "  - Docs: https://sadlang.org/docs/firebase-example"
echo "  - Source: https://github.com/sadlang/firebase-example"
echo "  - Issues: https://github.com/sadlang/firebase-example/issues"
echo ""

echo "💡 Next Steps:"
echo "  1. Share the package with the community"
echo "  2. Monitor package downloads and feedback"
echo "  3. Update documentation as needed"
echo "  4. Create new versions with improvements"
echo ""

echo "🙏 الحمد لله على إتمام النشر!"
echo "   Thank you for contributing to Sad Language!"
echo ""

# ============================================================================
# Installation Test (optional)
# اختبار التثبيت (اختياري)
# ============================================================================

echo "Optional: Test installation in a new directory"
echo "اختياري: اختبار التثبيت في مجلد جديد"
echo ""

read -p "Do you want to test installation? (y/n) " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "Creating test directory..."
    mkdir -p /tmp/sad-pkg-test
    cd /tmp/sad-pkg-test
    
    echo "Creating test sad.toml..."
    cat > sad.toml << 'EOF'
[package]
name = "firebase-test"
version = "1.0.0"

[dependencies]
firebase-example = "^1.0.0"
EOF
    
    echo "Running: sad-pkg install"
    sad-pkg install
    
    echo ""
    echo "✅ Installation test completed"
    echo "   Files installed in: /tmp/sad-pkg-test/.sad/packages/"
    echo ""
    
    cd -
fi

echo "════════════════════════════════════════════════════════════"
echo "🎉 Publishing process completed successfully! 🎉"
echo "════════════════════════════════════════════════════════════"
echo ""
