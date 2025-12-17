/*
 * برنامج تشغيل الاختبارات الرئيسي / Main Test Runner
 * =====================================================
 * 
 * يقوم بتشغيل جميع اختبارات مكونات LLVM Backend
 * Runs all LLVM Backend component tests
 */

#include "test_framework.h"
#include <iostream>
#include <string>
#include <vector>

using namespace sad::testing;

// تصريحات دوال الاختبار الخارجية / External test function declarations
extern int run_llvm_linker_tests();

/**
 * عرض المساعدة / Display help
 */
void displayHelp() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  مُشغل اختبارات SadLang / SadLang Test Runner           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\nالاستخدام / Usage:\n";
    std::cout << "  test_runner [options]\n";
    std::cout << "\nالخيارات / Options:\n";
    std::cout << "  --help, -h          عرض هذه المساعدة / Display this help\n";
    std::cout << "  --verbose, -v       وضع مطوّل / Verbose mode\n";
    std::cout << "  --suite <name>      تشغيل مجموعة معينة / Run specific suite\n";
    std::cout << "  --list              عرض جميع المجموعات / List all suites\n";
    std::cout << "\nالمجموعات المتاحة / Available Suites:\n";
    std::cout << "  - LLVMLinker        اختبارات الرابط / Linker tests\n";
    std::cout << "  - LLVMOptimizer     اختبارات المُحسّن / Optimizer tests\n";
    std::cout << "  - LLVMTarget        اختبارات الهدف / Target tests\n";
    std::cout << "  - LLVMDebug         اختبارات التنقيح / Debug tests\n";
    std::cout << "  - LLVMRuntime       اختبارات وقت التشغيل / Runtime tests\n";
    std::cout << "  - LLVMIRGenerator   اختبارات مولد IR / IR generator tests\n";
    std::cout << "  - Integration       اختبارات التكامل / Integration tests\n";
    std::cout << std::endl;
}

/**
 * الدالة الرئيسية / Main function
 */
int main(int argc, char** argv) {
    bool verbose = true;
    std::string suite_filter;
    
    // معالجة الوسائط / Process arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            displayHelp();
            return 0;
        }
        else if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
        else if (arg == "--quiet" || arg == "-q") {
            verbose = false;
        }
        else if (arg == "--suite" && i + 1 < argc) {
            suite_filter = argv[++i];
        }
        else if (arg == "--list") {
            displayHelp();
            return 0;
        }
    }
    
    // طباعة الترويسة / Print header
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                           ║\n";
    std::cout << "║  بسم الله الرحمن الرحيم                                  ║\n";
    std::cout << "║                                                           ║\n";
    std::cout << "║  اختبارات SadLang LLVM Backend                           ║\n";
    std::cout << "║  SadLang LLVM Backend Test Suite                         ║\n";
    std::cout << "║                                                           ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << std::endl;
    
    int total_failures = 0;
    int total_tests = 0;
    int total_passed = 0;
    
    auto& manager = TestManager::instance();
    
    // تشغيل الاختبارات / Run tests
    if (suite_filter.empty() || suite_filter == "LLVMLinker") {
        std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "  اختبارات الرابط / Linker Tests\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        int failures = manager.runTests("LLVMLinker", verbose);
        const auto& results = manager.getResults();
        
        int passed = 0;
        for (const auto& result : results) {
            if (result.result == TestResult::Passed) {
                passed++;
            }
        }
        
        total_failures += failures;
        total_tests += results.size();
        total_passed += passed;
    }
    
    // المزيد من المجموعات سيتم إضافتها لاحقاً / More suites to be added later
    
    // طباعة النتائج النهائية / Print final results
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  النتائج النهائية / Final Results                        ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
    std::cout << "\nمجموع الاختبارات / Total Tests:     " << total_tests << std::endl;
    std::cout << "نجح / Passed:                       " << total_passed << std::endl;
    std::cout << "فشل / Failed:                       " << total_failures << std::endl;
    
    if (total_failures == 0) {
        std::cout << "\n✅ جميع الاختبارات نجحت! الحمد لله!" << std::endl;
        std::cout << "✅ All tests passed! Praise be to Allah!" << std::endl;
    } else {
        std::cout << "\n❌ بعض الاختبارات فشلت. يرجى المراجعة." << std::endl;
        std::cout << "❌ Some tests failed. Please review." << std::endl;
    }
    
    std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << std::endl;
    
    return total_failures;
}
