// بسم الله الرحمن الرحيم
/**
 * @file publish_package.cpp
 * @brief نشر الحزمة على الخادم - Publish package to registry
 * 
 * مثال على كيفية رفع حزمة على سجل الحزم
 * Example of how to upload a package to the registry
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#include "../tools/pkg/registry_client.h"
#include "../tools/pkg/package.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace sad::pkg;

/**
 * رفع الحزمة يدوياً باستخدام HTTP POST
 * Upload package manually using HTTP POST
 */
bool upload_package_manual(const std::string& package_path, 
                          const std::string& registry_url,
                          const std::string& auth_token) {
    
    std::cout << "📦 رفع الحزمة - Uploading package..." << std::endl;
    std::cout << "   Package: " << package_path << std::endl;
    std::cout << "   Registry: " << registry_url << std::endl;
    
    // قراءة محتوى الملف
    std::ifstream file(package_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "❌ فشل فتح الملف - Failed to open file: " << package_path << std::endl;
        return false;
    }
    
    // قراءة البيانات
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string file_data = buffer.str();
    file.close();
    
    std::cout << "   File size: " << file_data.size() << " bytes" << std::endl;
    
    // TODO: إضافة multipart/form-data upload
    // For now, we'll use curl command directly
    std::cout << "\n⚠️  ملاحظة: رفع الحزم يتطلب تنفيذ multipart/form-data في RegistryClient" << std::endl;
    std::cout << "Note: Package upload requires multipart/form-data implementation in RegistryClient\n" << std::endl;
    
    // عرض أمر curl المطلوب
    std::cout << "💡 يمكنك رفع الحزمة يدوياً باستخدام curl:" << std::endl;
    std::cout << "You can upload the package manually using curl:\n" << std::endl;
    
    std::cout << "curl -X POST \\" << std::endl;
    std::cout << "  -H \"Authorization: Bearer " << auth_token << "\" \\" << std::endl;
    std::cout << "  -F \"package=@" << package_path << "\" \\" << std::endl;
    std::cout << "  " << registry_url << "/api/v1/packages/publish" << std::endl;
    
    return false;
}

/**
 * اختبار البحث عن الحزمة بعد الرفع
 * Test searching for package after upload
 */
void test_search_package(const std::string& package_name, 
                        const std::string& registry_url) {
    
    std::cout << "\n🔍 البحث عن الحزمة - Searching for package..." << std::endl;
    
    try {
        RegistryClient client(registry_url);
        auto results = client.search(package_name, 5);
        
        if (results.empty()) {
            std::cout << "❌ لم يتم العثور على الحزمة - Package not found" << std::endl;
        } else {
            std::cout << "✅ تم العثور على " << results.size() << " نتيجة - Found " << results.size() << " results:" << std::endl;
            for (const auto& name : results) {
                std::cout << "   - " << name << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "❌ خطأ في البحث - Search error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "بسم الله الرحمن الرحيم\n" << std::endl;
    std::cout << "=== نشر حزمة لغة ص - Sad Package Publisher ===" << std::endl;
    std::cout << "================================================\n" << std::endl;
    
    // الإعدادات الافتراضية
    std::string package_path = "C:\\s\\s_language\\examples\\math-utils-1.0.0.tar.gz";
    std::string registry_url = "http://localhost:8080"; // خادم محلي للاختبار
    std::string auth_token = "your-auth-token-here"; // يجب الحصول عليه من /api/v1/auth/login
    std::string package_name = "math-utils";
    
    // قراءة المعاملات من سطر الأوامر
    if (argc >= 2) {
        package_path = argv[1];
    }
    if (argc >= 3) {
        registry_url = argv[2];
    }
    if (argc >= 4) {
        auth_token = argv[3];
    }
    
    std::cout << "📋 معلومات النشر - Publishing Information:" << std::endl;
    std::cout << "   Package: " << package_path << std::endl;
    std::cout << "   Registry: " << registry_url << std::endl;
    std::cout << "   Auth Token: " << (auth_token.length() > 0 ? "***configured***" : "NOT SET") << std::endl;
    std::cout << std::endl;
    
    // التحقق من وجود الملف
    std::ifstream check_file(package_path);
    if (!check_file.good()) {
        std::cerr << "❌ خطأ: الملف غير موجود - Error: File not found" << std::endl;
        std::cerr << "   Path: " << package_path << std::endl;
        return 1;
    }
    check_file.close();
    
    std::cout << "✅ الملف موجود - File exists" << std::endl;
    
    // قراءة بيانات الحزمة من sad.toml
    std::cout << "\n📖 قراءة بيانات الحزمة - Reading package metadata..." << std::endl;
    try {
        Package pkg = Package::from_file("C:\\s\\s_language\\examples\\pkg_example\\sad.toml");
        std::cout << "   Name: " << pkg.name << std::endl;
        std::cout << "   Version: " << pkg.version.to_string() << std::endl;
        std::cout << "   Description: " << pkg.description << std::endl;
        std::cout << "   Authors: ";
        for (size_t i = 0; i < pkg.authors.size(); i++) {
            std::cout << pkg.authors[i];
            if (i < pkg.authors.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "   License: " << pkg.license << std::endl;
        package_name = pkg.name;
    } catch (const std::exception& e) {
        std::cerr << "⚠️  تحذير: فشل قراءة sad.toml - Warning: Failed to read sad.toml" << std::endl;
        std::cerr << "   Error: " << e.what() << std::endl;
    }
    
    // محاولة رفع الحزمة
    std::cout << "\n📤 محاولة رفع الحزمة - Attempting to upload package..." << std::endl;
    bool success = upload_package_manual(package_path, registry_url, auth_token);
    
    if (!success) {
        std::cout << "\n⚠️  لم يتم رفع الحزمة تلقائياً - Package not uploaded automatically" << std::endl;
        std::cout << "   يجب رفع الحزمة يدوياً باستخدام الأمر أعلاه" << std::endl;
        std::cout << "   Please upload the package manually using the command above\n" << std::endl;
        
        // عرض خطوات الرفع اليدوي
        std::cout << "📝 خطوات الرفع اليدوي - Manual Upload Steps:\n" << std::endl;
        std::cout << "1. تشغيل خادم السجل المحلي (Dart/Flutter server):" << std::endl;
        std::cout << "   cd C:\\kadahtech\\s_website\\s_module_server" << std::endl;
        std::cout << "   flutter run -d web-server --web-port=8080\n" << std::endl;
        
        std::cout << "2. الحصول على رمز المصادقة (Auth token):" << std::endl;
        std::cout << "   curl -X POST \\" << std::endl;
        std::cout << "     -H \"Content-Type: application/json\" \\" << std::endl;
        std::cout << "     -d '{\"username\":\"your_username\",\"password\":\"your_password\"}' \\" << std::endl;
        std::cout << "     " << registry_url << "/api/v1/auth/login\n" << std::endl;
        
        std::cout << "3. رفع الحزمة باستخدام الرمز:" << std::endl;
        std::cout << "   curl -X POST \\" << std::endl;
        std::cout << "     -H \"Authorization: Bearer YOUR_TOKEN\" \\" << std::endl;
        std::cout << "     -F \"package=@" << package_path << "\" \\" << std::endl;
        std::cout << "     " << registry_url << "/api/v1/packages/publish\n" << std::endl;
    }
    
    // محاولة البحث عن الحزمة (للتحقق من الرفع)
    std::cout << "🔄 محاولة البحث عن الحزمة في السجل..." << std::endl;
    std::cout << "Attempting to search for package in registry...\n" << std::endl;
    test_search_package(package_name, registry_url);
    
    std::cout << "\n================================================" << std::endl;
    std::cout << "الحمد لله رب العالمين" << std::endl;
    
    return 0;
}

// الحمد لله رب العالمين
