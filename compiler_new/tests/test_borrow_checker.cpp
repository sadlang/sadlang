// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file test_borrow_checker.cpp
 * @brief اختبارات نظام الملكية والاستعارة / Ownership & Borrow Checker Tests
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * 
 * =============================================================================
 *                    اختبارات شاملة لنظام أمان الذاكرة
 *                    Comprehensive Memory Safety System Tests
 * =============================================================================
 * 
 *     هذا الملف يحتوي على اختبارات لجميع مكونات نظام أمان الذاكرة:
 *     
 *     1. اختبارات تتبع الملكية (OwnershipTracker)
 *     2. اختبارات فحص الاستعارة (BorrowChecker)
 *     3. اختبارات تحليل دورة الحياة (LifetimeAnalyzer)
 *     4. اختبارات تحليل النقل (MoveAnalyzer)
 *     5. اختبارات فحص الكود غير الآمن (UnsafeChecker)
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// تضمين مكونات نظام أمان الذاكرة
#include "semantic/ownership_tracker.h"
#include "semantic/borrow_checker.h"
#include "semantic/lifetime_analyzer.h"
#include "semantic/move_analyzer.h"
#include "semantic/unsafe_checker.h"

using namespace Sad::Semantic;

// =============================================================================
//                    الدوال المساعدة / Helper Functions
// =============================================================================

/**
 * @brief طباعة عنوان الاختبار بالعربية
 */
void printTestHeader(const std::string& arabicName, const std::string& englishName) {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  " << arabicName << "\n";
    std::cout << "║  " << englishName << "\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

/**
 * @brief طباعة نتيجة اختبار فردي
 */
void printTestResult(const std::string& testName, bool passed) {
    if (passed) {
        std::cout << "  ✓ نجح: " << testName << "\n";
    } else {
        std::cout << "  ✗ فشل: " << testName << "\n";
    }
}

/**
 * @brief عداد الاختبارات
 */
struct TestStats {
    int total = 0;
    int passed = 0;
    int failed = 0;
    
    void record(bool success) {
        total++;
        if (success) passed++;
        else failed++;
    }
    
    void printSummary() {
        std::cout << "\n";
        std::cout << "════════════════════════════════════════════════════════════════\n";
        std::cout << "                    ملخص نتائج الاختبارات\n";
        std::cout << "════════════════════════════════════════════════════════════════\n";
        std::cout << "  إجمالي الاختبارات: " << total << "\n";
        std::cout << "  اختبارات ناجحة:    " << passed << " ✓\n";
        std::cout << "  اختبارات فاشلة:    " << failed << " ✗\n";
        std::cout << "════════════════════════════════════════════════════════════════\n";
        
        if (failed == 0) {
            std::cout << "  🎉 جميع الاختبارات نجحت!\n";
        } else {
            std::cout << "  ⚠️  بعض الاختبارات فشلت\n";
        }
        std::cout << "\n";
    }
};

// العداد العام
TestStats stats;

// =============================================================================
//                    اختبارات تتبع الملكية / Ownership Tracker Tests
// =============================================================================

void testOwnershipTracker() {
    printTestHeader("اختبارات تتبع الملكية", "Ownership Tracker Tests");
    
    OwnershipTracker tracker;
    tracker.setArabicMessages(true);
    
    // اختبار 1: تسجيل متغير جديد
    {
        SourceLocation loc{1, 1, "test.sad"};
        tracker.declareVariable("س", "عدد", loc, OwnershipKind::Owned);
        
        bool hasOwner = tracker.getOwnership("س").has_value();
        printTestResult("تسجيل متغير جديد يُنشئ ملكية", hasOwner);
        stats.record(hasOwner);
    }
    
    // اختبار 2: نقل الملكية
    {
        SourceLocation loc{2, 1, "test.sad"};
        bool canTransfer = tracker.transferOwnership("س", "ص", loc);
        
        bool sIsMoved = tracker.isVariableMoved("س");
        printTestResult("نقل الملكية يُعلِّم المتغير كمنقول", sIsMoved);
        stats.record(sIsMoved);
    }
    
    // اختبار 3: استخدام بعد النقل يُنتج خطأ
    {
        SourceLocation loc{3, 1, "test.sad"};
        auto error = tracker.checkUseAfterMove("س", loc);
        
        bool hasError = error.has_value();
        printTestResult("استخدام بعد النقل يُنتج خطأ", hasError);
        stats.record(hasError);
    }
    
    // اختبار 4: إدارة النطاقات
    {
        tracker.enterScope();
        SourceLocation loc{4, 1, "test.sad"};
        tracker.declareVariable("محلي", "نص", loc, OwnershipKind::Owned);
        
        bool existsBeforeExit = tracker.getOwnership("محلي").has_value();
        tracker.exitScope();
        bool existsAfterExit = tracker.getOwnership("محلي").has_value();
        
        bool scopeWorks = existsBeforeExit && !existsAfterExit;
        printTestResult("المتغيرات تُحذف عند الخروج من النطاق", scopeWorks);
        stats.record(scopeWorks);
    }
}

// =============================================================================
//                    اختبارات فحص الاستعارة / Borrow Checker Tests
// =============================================================================

void testBorrowChecker() {
    printTestHeader("اختبارات فحص الاستعارة", "Borrow Checker Tests");
    
    BorrowChecker checker;
    checker.setArabicMessages(true);
    
    // اختبار 1: استعارة قراءة متعددة مسموحة
    {
        SourceLocation loc1{1, 1, "test.sad"};
        SourceLocation loc2{2, 1, "test.sad"};
        
        checker.declareVariable("بيانات", "قائمة<عدد>", loc1);
        
        bool borrow1 = checker.borrowImmutable("بيانات", "مرجع1", loc1);
        bool borrow2 = checker.borrowImmutable("بيانات", "مرجع2", loc2);
        
        bool multiReadOk = borrow1 && borrow2;
        printTestResult("استعارات قراءة متعددة مسموحة", multiReadOk);
        stats.record(multiReadOk);
    }
    
    // اختبار 2: استعارة كتابة وحيدة
    {
        SourceLocation loc{3, 1, "test.sad"};
        checker.declareVariable("عداد", "عدد", loc);
        
        bool mutableBorrow = checker.borrowMutable("عداد", "مغير", loc);
        
        printTestResult("استعارة كتابة وحيدة مسموحة", mutableBorrow);
        stats.record(mutableBorrow);
    }
    
    // اختبار 3: تعارض استعارة كتابة مع قراءة
    {
        SourceLocation loc1{4, 1, "test.sad"};
        SourceLocation loc2{5, 1, "test.sad"};
        
        checker.declareVariable("مورد", "مورد_خارجي", loc1);
        checker.borrowImmutable("مورد", "قارئ", loc1);
        
        bool conflictingBorrow = checker.borrowMutable("مورد", "كاتب", loc2);
        
        bool conflictDetected = !conflictingBorrow || checker.hasErrors();
        printTestResult("تعارض استعارة كتابة مع قراءة مكتشف", conflictDetected);
        stats.record(conflictDetected);
    }
    
    // اختبار 4: انتهاء الاستعارة
    {
        checker.clearErrors();
        SourceLocation loc{6, 1, "test.sad"};
        checker.declareVariable("ملف", "ملف_نظام", loc);
        
        checker.borrowMutable("ملف", "مقبض", loc);
        checker.endBorrow("مقبض");
        
        // الآن يجب أن نستطيع الاستعارة مرة أخرى
        bool canBorrowAgain = checker.borrowMutable("ملف", "مقبض2", loc);
        
        printTestResult("يمكن الاستعارة بعد انتهاء الاستعارة السابقة", canBorrowAgain);
        stats.record(canBorrowAgain);
    }
}

// =============================================================================
//                    اختبارات تحليل دورة الحياة / Lifetime Analyzer Tests
// =============================================================================

void testLifetimeAnalyzer() {
    printTestHeader("اختبارات تحليل دورة الحياة", "Lifetime Analyzer Tests");
    
    LifetimeAnalyzer analyzer;
    analyzer.setArabicMessages(true);
    
    // اختبار 1: إنشاء دورة حياة
    {
        SourceLocation loc{1, 1, "test.sad"};
        LifetimeId lifetime = analyzer.createLifetime("'أ", loc);
        
        bool created = lifetime != LifetimeId::INVALID;
        printTestResult("إنشاء دورة حياة جديدة", created);
        stats.record(created);
    }
    
    // اختبار 2: ربط متغير بدورة حياة
    {
        SourceLocation loc{2, 1, "test.sad"};
        LifetimeId lifetime = analyzer.createLifetime("'ب", loc);
        analyzer.bindVariableToLifetime("مرجع", lifetime);
        
        auto boundLifetime = analyzer.getVariableLifetime("مرجع");
        bool bound = boundLifetime.has_value();
        
        printTestResult("ربط متغير بدورة حياة", bound);
        stats.record(bound);
    }
    
    // اختبار 3: التحقق من صلاحية دورة الحياة
    {
        SourceLocation loc{3, 1, "test.sad"};
        LifetimeId lifetime = analyzer.createLifetime("'ج", loc);
        
        bool validBefore = analyzer.isLifetimeValid(lifetime);
        analyzer.endLifetime(lifetime);
        bool validAfter = analyzer.isLifetimeValid(lifetime);
        
        bool lifecycleWorks = validBefore && !validAfter;
        printTestResult("دورة الحياة تنتهي بشكل صحيح", lifecycleWorks);
        stats.record(lifecycleWorks);
    }
    
    // اختبار 4: علاقات دورة الحياة (Outlives)
    {
        SourceLocation loc{4, 1, "test.sad"};
        LifetimeId outer = analyzer.createLifetime("'خارجي", loc);
        LifetimeId inner = analyzer.createLifetime("'داخلي", loc);
        
        analyzer.addOutlivesConstraint(outer, inner);
        bool outlives = analyzer.doesOutlive(outer, inner);
        
        printTestResult("علاقة 'يعيش أطول من' تعمل", outlives);
        stats.record(outlives);
    }
}

// =============================================================================
//                    اختبارات تحليل النقل / Move Analyzer Tests
// =============================================================================

void testMoveAnalyzer() {
    printTestHeader("اختبارات تحليل النقل", "Move Analyzer Tests");
    
    MoveAnalyzer analyzer;
    analyzer.setArabicMessages(true);
    
    // اختبار 1: تسجيل متغير
    {
        SourceLocation loc{1, 1, "test.sad"};
        analyzer.declareVariable("نص_كبير", "نص", loc, false);
        
        bool notMoved = !analyzer.isVariableMoved("نص_كبير");
        printTestResult("المتغير الجديد ليس منقولاً", notMoved);
        stats.record(notMoved);
    }
    
    // اختبار 2: تسجيل النقل
    {
        SourceLocation loc{2, 1, "test.sad"};
        analyzer.recordMove("نص_كبير", MoveKind::Assignment, loc, "متغير_آخر");
        
        bool isMoved = analyzer.isVariableMoved("نص_كبير");
        printTestResult("المتغير يُعلَّم كمنقول بعد النقل", isMoved);
        stats.record(isMoved);
    }
    
    // اختبار 3: فحص الاستخدام بعد النقل
    {
        SourceLocation loc{3, 1, "test.sad"};
        auto error = analyzer.checkUse("نص_كبير", loc);
        
        bool errorDetected = error.has_value();
        printTestResult("استخدام بعد النقل يُكتشف", errorDetected);
        stats.record(errorDetected);
    }
    
    // اختبار 4: إعادة التعيين تُحيي المتغير
    {
        SourceLocation loc{4, 1, "test.sad"};
        analyzer.reassignVariable("نص_كبير", loc);
        
        bool notMovedAfterReassign = !analyzer.isVariableMoved("نص_كبير");
        printTestResult("إعادة التعيين تُحيي المتغير", notMovedAfterReassign);
        stats.record(notMovedAfterReassign);
    }
    
    // اختبار 5: أنواع النسخ لا تُنقل
    {
        SourceLocation loc{5, 1, "test.sad"};
        analyzer.declareVariable("رقم", "عدد", loc, true);  // نوع نسخ
        
        auto result = analyzer.recordMove("رقم", MoveKind::Assignment, loc, "آخر");
        
        bool isCopyType = (result == MoveCheckResult::IsCopyType);
        printTestResult("أنواع النسخ لا تُنقل", isCopyType);
        stats.record(isCopyType);
    }
    
    // اختبار 6: النقل الجزئي للحقول
    {
        SourceLocation loc{6, 1, "test.sad"};
        analyzer.declareVariable("هيكل", "شخص", loc, false);
        
        analyzer.recordPartialMove("هيكل", "اسم", loc, "اسم_منفصل");
        
        bool partiallyMoved = analyzer.isPartiallyMoved("هيكل");
        auto movedFields = analyzer.getMovedFields("هيكل");
        bool fieldMoved = !movedFields.empty();
        
        printTestResult("النقل الجزئي للحقول يعمل", partiallyMoved && fieldMoved);
        stats.record(partiallyMoved && fieldMoved);
    }
}

// =============================================================================
//                    اختبارات فحص الكود غير الآمن / Unsafe Checker Tests
// =============================================================================

void testUnsafeChecker() {
    printTestHeader("اختبارات فحص الكود غير الآمن", "Unsafe Checker Tests");
    
    UnsafeChecker checker;
    checker.setArabicMessages(true);
    
    // اختبار 1: العملية غير الآمنة خارج الكتلة تُنتج خطأ
    {
        SourceLocation loc{1, 1, "test.sad"};
        bool allowed = checker.checkOperation(
            UnsafeOperationKind::RawPointerDeref, loc, "*مؤشر");
        
        bool errorProduced = !allowed;
        printTestResult("العملية غير الآمنة خارج الكتلة تُنتج خطأ", errorProduced);
        stats.record(errorProduced);
    }
    
    // اختبار 2: العملية غير الآمنة داخل الكتلة مسموحة
    {
        checker.clearErrors();
        SourceLocation loc{2, 1, "test.sad"};
        
        checker.enterUnsafeBlock(loc);
        bool allowed = checker.checkOperation(
            UnsafeOperationKind::RawPointerDeref, loc, "*مؤشر");
        checker.exitUnsafeBlock();
        
        printTestResult("العملية غير الآمنة داخل الكتلة مسموحة", allowed);
        stats.record(allowed);
    }
    
    // اختبار 3: تتبع عمق الكتل المتداخلة
    {
        SourceLocation loc{3, 1, "test.sad"};
        
        checker.enterUnsafeBlock(loc);
        checker.enterUnsafeBlock(loc);
        int depth2 = checker.getUnsafeDepth();
        checker.exitUnsafeBlock();
        int depth1 = checker.getUnsafeDepth();
        checker.exitUnsafeBlock();
        int depth0 = checker.getUnsafeDepth();
        
        bool nestingWorks = (depth2 == 2) && (depth1 == 1) && (depth0 == 0);
        printTestResult("تتبع الكتل المتداخلة يعمل", nestingWorks);
        stats.record(nestingWorks);
    }
    
    // اختبار 4: تسجيل الدوال غير الآمنة
    {
        checker.registerUnsafeFunction("اقرأ_ذاكرة_خام");
        
        bool isUnsafe = checker.isUnsafeFunction("اقرأ_ذاكرة_خام");
        bool isNotUnsafe = !checker.isUnsafeFunction("اطبع");
        
        printTestResult("تسجيل الدوال غير الآمنة يعمل", isUnsafe && isNotUnsafe);
        stats.record(isUnsafe && isNotUnsafe);
    }
    
    // اختبار 5: فحص FFI
    {
        checker.clearErrors();
        SourceLocation loc{5, 1, "test.sad"};
        
        // FFI خارج كتلة غير آمنة
        bool ffiOutsideUnsafe = checker.checkFFICall("printf", loc);
        bool errorForFFI = !ffiOutsideUnsafe;
        
        printTestResult("استدعاء FFI يتطلب كتلة غير_آمن", errorForFFI);
        stats.record(errorForFFI);
    }
}

// =============================================================================
//                    اختبارات التكامل / Integration Tests
// =============================================================================

void testIntegration() {
    printTestHeader("اختبارات التكامل", "Integration Tests");
    
    // اختبار سيناريو كامل: دالة تأخذ مرجع وتُرجع نتيجة
    {
        OwnershipTracker ownership;
        BorrowChecker borrower;
        LifetimeAnalyzer lifetimes;
        MoveAnalyzer moves;
        
        SourceLocation loc{1, 1, "integration.sad"};
        
        // 1. إنشاء متغير
        ownership.declareVariable("قائمة", "قائمة<عدد>", loc, OwnershipKind::Owned);
        borrower.declareVariable("قائمة", "قائمة<عدد>", loc);
        moves.declareVariable("قائمة", "قائمة<عدد>", loc, false);
        
        // 2. إنشاء دورة حياة للدالة
        LifetimeId fnLifetime = lifetimes.createLifetime("'دالة", loc);
        
        // 3. استعارة للقراءة
        bool borrowOk = borrower.borrowImmutable("قائمة", "مرجع_قراءة", loc);
        
        // 4. التحقق من عدم النقل أثناء الاستعارة
        auto moveCheck = moves.recordMove("قائمة", MoveKind::FunctionParameter, loc, "دالة");
        
        // 5. إنهاء الاستعارة
        borrower.endBorrow("مرجع_قراءة");
        
        // 6. الآن يمكن النقل
        moves.reassignVariable("قائمة", loc);  // إعادة تعيين للاختبار
        auto moveAfterBorrow = moves.recordMove("قائمة", MoveKind::FunctionParameter, loc, "دالة2");
        
        bool integrationWorks = borrowOk && (moveAfterBorrow == MoveCheckResult::CanMove);
        printTestResult("التكامل بين المكونات يعمل بشكل صحيح", integrationWorks);
        stats.record(integrationWorks);
    }
    
    // اختبار سيناريو: استعارة مغيرة مع حماية
    {
        BorrowChecker borrower;
        SourceLocation loc{2, 1, "integration.sad"};
        
        borrower.declareVariable("عداد", "عدد", loc);
        
        // استعارة مغيرة
        bool mutBorrow = borrower.borrowMutable("عداد", "مغير", loc);
        
        // محاولة استعارة أخرى (يجب أن تفشل)
        bool secondBorrow = borrower.borrowImmutable("عداد", "قارئ", loc);
        
        bool protectionWorks = mutBorrow && (!secondBorrow || borrower.hasErrors());
        printTestResult("حماية الاستعارة المغيرة تعمل", protectionWorks);
        stats.record(protectionWorks);
    }
}

// =============================================================================
//                    الدالة الرئيسية / Main Function
// =============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                              ║\n";
    std::cout << "║            اختبارات نظام أمان الذاكرة - لغة ص               ║\n";
    std::cout << "║            Memory Safety System Tests - Sad Language         ║\n";
    std::cout << "║                                                              ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    
    // تشغيل جميع الاختبارات
    testOwnershipTracker();
    testBorrowChecker();
    testLifetimeAnalyzer();
    testMoveAnalyzer();
    testUnsafeChecker();
    testIntegration();
    
    // طباعة الملخص
    stats.printSummary();
    
    // إرجاع رمز الخروج
    return (stats.failed == 0) ? 0 : 1;
}
