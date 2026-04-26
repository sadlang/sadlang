// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file test_smart_ptr.cpp
 * @brief اختبارات المؤشرات الذكية / Smart Pointer Tests
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * 
 * =============================================================================
 *                    اختبارات عداد المراجع الذكي
 *                    Reference Counting Smart Pointer Tests
 * =============================================================================
 */

#include <iostream>
#include <cassert>
#include <string>
#include <thread>
#include <vector>

#include "runtime/smart_ptr.h"

using namespace Sad::Runtime;

// =============================================================================
//                    الدوال المساعدة / Helper Functions
// =============================================================================

int testsRun = 0;
int testsPassed = 0;

void printTestResult(const std::string& testName, bool passed) {
    testsRun++;
    if (passed) {
        testsPassed++;
        std::cout << "  ✓ نجح: " << testName << "\n";
    } else {
        std::cout << "  ✗ فشل: " << testName << "\n";
    }
}

// =============================================================================
//                    اختبارات Rc<T>
// =============================================================================

void testRc() {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات عم<T> (Rc<T>)                                       ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // اختبار 1: الإنشاء الأساسي
    {
        auto ptr = Rc<int>::create(42);
        bool valid = ptr && *ptr == 42;
        printTestResult("إنشاء عم<عدد> بقيمة 42", valid);
    }
    
    // اختبار 2: عدد المراجع
    {
        auto ptr = Rc<int>::create(10);
        bool countIs1 = ptr.useCount() == 1;
        printTestResult("العداد = 1 عند الإنشاء", countIs1);
    }
    
    // اختبار 3: الاستنساخ يزيد العداد
    {
        auto ptr = Rc<int>::create(10);
        auto clone = ptr.clone();
        bool countIs2 = ptr.useCount() == 2 && clone.useCount() == 2;
        printTestResult("الاستنساخ يزيد العداد لـ 2", countIs2);
    }
    
    // اختبار 4: التدمير ينقص العداد
    {
        auto ptr = Rc<int>::create(10);
        {
            auto clone = ptr.clone();
            // العداد = 2 هنا
        }
        // clone دُمِّر، العداد = 1
        bool countIs1Again = ptr.useCount() == 1;
        printTestResult("التدمير ينقص العداد", countIs1Again);
    }
    
    // اختبار 5: الوصول للقيمة
    {
        auto ptr = Rc<std::string>::create("مرحباً بالعالم");
        bool correctValue = *ptr == "مرحباً بالعالم";
        bool correctLength = ptr->length() == std::string("مرحباً بالعالم").length();
        printTestResult("الوصول للقيمة عبر * و ->", correctValue && correctLength);
    }
    
    // اختبار 6: unique()
    {
        auto ptr = Rc<int>::create(5);
        bool isUnique = ptr.unique();
        auto clone = ptr.clone();
        bool notUnique = !ptr.unique();
        printTestResult("unique() يعمل بشكل صحيح", isUnique && notUnique);
    }
    
    // اختبار 7: makeInPlace
    {
        auto ptr = Rc<std::vector<int>>::makeInPlace(3, 100);
        bool correctSize = ptr->size() == 3;
        bool correctValues = (*ptr)[0] == 100 && (*ptr)[2] == 100;
        printTestResult("makeInPlace يعمل", correctSize && correctValues);
    }
    
    // اختبار 8: تعيين النسخ
    {
        auto ptr1 = Rc<int>::create(1);
        auto ptr2 = Rc<int>::create(2);
        ptr1 = ptr2;
        bool sameValue = *ptr1 == 2;
        bool sameCount = ptr1.useCount() == 2;
        printTestResult("تعيين النسخ يعمل", sameValue && sameCount);
    }
    
    // اختبار 9: تعيين النقل
    {
        auto ptr1 = Rc<int>::create(1);
        auto ptr2 = Rc<int>::create(2);
        ptr1 = std::move(ptr2);
        bool movedValue = *ptr1 == 2;
        bool ptr2Empty = !ptr2;
        printTestResult("تعيين النقل يعمل", movedValue && ptr2Empty);
    }
}

// =============================================================================
//                    اختبارات Arc<T>
// =============================================================================

void testArc() {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات عم_ذري<T> (Arc<T>)                                  ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // اختبار 1: الإنشاء الأساسي
    {
        auto ptr = Arc<int>::create(42);
        bool valid = ptr && *ptr == 42;
        printTestResult("إنشاء عم_ذري<عدد> بقيمة 42", valid);
    }
    
    // اختبار 2: الاستخدام بين الخيوط
    {
        auto shared = Arc<int>::create(0);
        std::vector<std::thread> threads;
        
        // إنشاء 10 خيوط تقرأ القيمة
        for (int i = 0; i < 10; i++) {
            threads.emplace_back([clone = shared.clone()]() {
                // قراءة فقط للاختبار
                volatile int val = *clone;
                (void)val;
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
        
        bool countCorrect = shared.useCount() == 1;
        printTestResult("الاستخدام الآمن بين الخيوط", countCorrect);
    }
    
    // اختبار 3: العداد الذري صحيح
    {
        auto ptr = Arc<int>::create(100);
        std::vector<Arc<int>> clones;
        
        for (int i = 0; i < 100; i++) {
            clones.push_back(ptr.clone());
        }
        
        bool countCorrect = ptr.useCount() == 101;
        
        clones.clear();
        bool countAfterClear = ptr.useCount() == 1;
        
        printTestResult("العداد الذري دقيق", countCorrect && countAfterClear);
    }
}

// =============================================================================
//                    اختبارات Weak<T>
// =============================================================================

void testWeak() {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات مرجع_ضعيف<T> (Weak<T>)                             ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // اختبار 1: إنشاء من Rc
    {
        auto strong = Rc<int>::create(42);
        auto weak = Weak<int>::fromRc(strong);
        
        bool notExpired = !weak.expired();
        bool correctCount = weak.useCount() == 1;
        printTestResult("إنشاء مرجع ضعيف من عم", notExpired && correctCount);
    }
    
    // اختبار 2: الترقية الناجحة
    {
        auto strong = Rc<int>::create(42);
        auto weak = Weak<int>::fromRc(strong);
        
        auto upgraded = weak.upgradeToRc();
        bool upgradeSuccess = upgraded && *upgraded == 42;
        bool countIncreased = strong.useCount() == 2;
        
        printTestResult("ترقية المرجع الضعيف تنجح", upgradeSuccess && countIncreased);
    }
    
    // اختبار 3: المرجع الضعيف لا يمنع الحذف
    {
        Weak<int> weak;
        {
            auto strong = Rc<int>::create(42);
            weak = Weak<int>::fromRc(strong);
        }
        // strong دُمِّر الآن
        
        bool isExpired = weak.expired();
        auto upgraded = weak.upgradeToRc();
        bool upgradeNull = !upgraded;
        
        printTestResult("المرجع الضعيف لا يمنع الحذف", isExpired && upgradeNull);
    }
    
    // اختبار 4: كسر الدورات المرجعية (مفهوم)
    {
        // سيناريو: عقدة تشير لنفسها عبر مرجع ضعيف
        struct Node {
            int value;
            Weak<Node> self;  // مرجع ضعيف لنفسها
        };
        
        auto node = Rc<Node>::makeInPlace();
        node->value = 123;
        node->self = Weak<Node>::fromRc(node);
        
        bool selfRefWorks = node->self.useCount() == 1;
        printTestResult("المرجع الضعيف يكسر الدورات المرجعية", selfRefWorks);
    }
    
    // اختبار 5: عدد المراجع الضعيفة
    {
        auto strong = Rc<int>::create(10);
        auto weak1 = Weak<int>::fromRc(strong);
        auto weak2 = Weak<int>::fromRc(strong);
        auto weak3 = Weak<int>::fromRc(strong);
        
        bool weakCount3 = weak1.weakCount() == 3;
        printTestResult("تتبع عدد المراجع الضعيفة", weakCount3);
    }
}

// =============================================================================
//                    اختبارات الأسماء العربية
// =============================================================================

void testArabicAliases() {
    std::cout << "\n╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  اختبارات الأسماء العربية                                     ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // اختبار 1: عم<T>
    {
        عم<int> س = عم<int>::create(42);
        bool works = س && *س == 42;
        printTestResult("عم<عدد> يعمل", works);
    }
    
    // اختبار 2: عم_ذري<T>
    {
        عم_ذري<std::string> ص = عم_ذري<std::string>::create("مرحباً");
        bool works = ص && *ص == "مرحباً";
        printTestResult("عم_ذري<نص> يعمل", works);
    }
    
    // اختبار 3: مرجع_ضعيف<T>
    {
        عم<int> قوي = عم<int>::create(100);
        مرجع_ضعيف<int> ضعيف = مرجع_ضعيف<int>::fromRc(قوي);
        bool works = !ضعيف.expired();
        printTestResult("مرجع_ضعيف<عدد> يعمل", works);
    }
}

// =============================================================================
//                    الدالة الرئيسية / Main Function
// =============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║         اختبارات المؤشرات الذكية - لغة ص                    ║\n";
    std::cout << "║         Smart Pointer Tests - Sad Language                   ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    testRc();
    testArc();
    testWeak();
    testArabicAliases();
    
    std::cout << "\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "                    ملخص نتائج الاختبارات\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    std::cout << "  إجمالي الاختبارات: " << testsRun << "\n";
    std::cout << "  اختبارات ناجحة:    " << testsPassed << " ✓\n";
    std::cout << "  اختبارات فاشلة:    " << (testsRun - testsPassed) << " ✗\n";
    std::cout << "════════════════════════════════════════════════════════════════\n";
    
    if (testsPassed == testsRun) {
        std::cout << "  🎉 جميع الاختبارات نجحت!\n";
    } else {
        std::cout << "  ⚠️  بعض الاختبارات فشلت\n";
    }
    std::cout << "\n";
    
    return (testsPassed == testsRun) ? 0 : 1;
}
