/*
 * اختبار نظام كشف سلسلة الأدوات / Toolchain Detection Test
 * =========================================================
 */

#include <iostream>
#include "compiler/backends/llvm/toolchain_detection.h"

int main() {
    std::cout << "=== اختبار كشف سلسلة الأدوات / Toolchain Detection Test ===" << std::endl;
    std::cout << std::endl;
    
    // تهيئة مدير سلسلة الأدوات / Initialize toolchain manager
    auto& manager = sad::ToolchainManager::getInstance();
    
    if (manager.initialize()) {
        std::cout << "✓ تم تهيئة مدير سلسلة الأدوات بنجاح / Toolchain manager initialized successfully" << std::endl;
    } else {
        std::cout << "✗ فشل تهيئة مدير سلسلة الأدوات / Failed to initialize toolchain manager" << std::endl;
        return 1;
    }
    
    // الحصول على سلسلة الأدوات النشطة / Get active toolchain
    const auto* tc = manager.getActiveToolchain();
    
    if (tc) {
        std::cout << std::endl;
        std::cout << "سلسلة الأدوات النشطة / Active Toolchain:" << std::endl;
        std::cout << "  النوع / Type: " << tc->getTypeString() << std::endl;
        std::cout << "  الاسم / Name: " << tc->name << std::endl;
        std::cout << "  المسار / Path: " << tc->install_path << std::endl;
        std::cout << "  الإصدار / Version: " << tc->version.toString() << std::endl;
        
        std::cout << std::endl;
        std::cout << "الأدوات / Tools:" << std::endl;
        
        if (tc->compiler.available) {
            std::cout << "  المترجم / Compiler: " << tc->compiler.path << std::endl;
        }
        
        if (tc->linker.available) {
            std::cout << "  الرابط / Linker: " << tc->linker.path << std::endl;
        }
        
        if (tc->archiver.available) {
            std::cout << "  المؤرشف / Archiver: " << tc->archiver.path << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "مسارات المكتبات / Library Paths:" << std::endl;
        for (const auto& path : tc->library_paths) {
            std::cout << "  " << path << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "المكتبات الافتراضية / Default Libraries:" << std::endl;
        for (const auto& lib : tc->default_libs) {
            std::cout << "  " << lib << std::endl;
        }
        
        // اختبار بناء أمر الربط / Test building link command
        std::cout << std::endl;
        std::cout << "اختبار أمر الربط / Link Command Test:" << std::endl;
        std::string cmd = manager.buildLinkCommand({"test.o"}, "test.exe");
        std::cout << "  " << cmd << std::endl;
        
    } else {
        std::cout << "✗ لم يتم العثور على سلسلة أدوات / No toolchain found" << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "✓ اكتمل الاختبار بنجاح / Test completed successfully" << std::endl;
    
    return 0;
}
