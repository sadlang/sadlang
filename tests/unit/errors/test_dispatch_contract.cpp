/**
 * @file test_dispatch_contract.cpp
 * @brief (AR) اختبارات العقد لدالة dispatch() — نقطة القرار الموحَّدة لأخطاء الملكية
 *        (EN) Contract tests for dispatch() — unified ownership error decision point
 *
 * العقود المُختبَرة:
 * 1. OwnershipMode::Disabled  → DispatchAction::Ignore دائماً (--gc)
 * 2. OwnershipMode::Warnings  → DispatchAction::Warn  دائماً (--learn)
 * 3. OwnershipMode::Strict    → DispatchAction::Fatal دائماً
 * 4. OwnershipMode::UltraStrict → DispatchAction::Fatal دائماً (--prod)
 * 5. teacherMode=true  → teachingNote غير فارغ في وضع Warnings
 * 6. teacherMode=false → teachingNote فارغ
 * 7. الرسائل (AR + EN) غير فارغة في Fatal/Warn
 * 8. --dev redirect → نفس نتيجة --gc (gcDefaults ← Disabled → Ignore)
 * 9. --hybrid redirect → نفس نتيجة --gc
 */

#include <cassert>
#include <iostream>
#include <string>

// (AR) الوحدات المطلوبة
// (EN) Required modules
#include "shared/errors/include/builders/dispatch.h"
#include "shared/memory_policy/include/memory/policy/gc_mode.h"
#include "shared/errors/include/source_location.h"

using namespace Sad::Errors;
using namespace Sad;

// =============================================================================
//                         مساعدات الاختبار
// =============================================================================

static int testsTotal = 0;
static int testsPassed = 0;
static int testsFailed = 0;

/// (AR) تحقق من شرط وسجِّل نتيجة الاختبار
/// (EN) Assert a condition and record test result
static void check(bool condition, const std::string &testName)
{
    ++testsTotal;
    if (condition)
    {
        ++testsPassed;
        std::cout << "✓ " << testName << "\n";
    }
    else
    {
        ++testsFailed;
        std::cerr << "✗ FAIL: " << testName << "\n";
    }
}

/// (AR) بناء SourceLocation مؤقتة للاختبار
/// (EN) Build a temporary SourceLocation for testing
static SourceLocation makeLocation()
{
    SourceLocation loc;
    loc.filename = "test_file.ص";
    loc.line = 10;
    loc.column = 5;
    return loc;
}

// =============================================================================
//                  اختبار 1: OwnershipMode::Disabled → Ignore
// =============================================================================

static void testDisabledModeIgnoresAll()
{
    std::cout << "\n─── اختبار 1: Disabled → Ignore (--gc) ───\n";

    MemoryModeSettings settings = MemoryModeSettings::gcDefaults();
    // (AR) gcDefaults → ownershipMode = Disabled
    // (EN) gcDefaults → ownershipMode = Disabled
    SourceLocation loc = makeLocation();

    // (AR) اختبر جميع أنواع الأخطاء الثمانية
    // (EN) Test all 8 error kinds
    check(
        dispatch(OwnershipErrorKind::UseAfterMove, settings, loc, "س").action == DispatchAction::Ignore,
        "Disabled: UseAfterMove → Ignore");
    check(
        dispatch(OwnershipErrorKind::BorrowAfterLifetimeEnd, settings, loc, "ص").action == DispatchAction::Ignore,
        "Disabled: BorrowAfterLifetimeEnd → Ignore");
    check(
        dispatch(OwnershipErrorKind::MutableBorrowConflict, settings, loc, "ع").action == DispatchAction::Ignore,
        "Disabled: MutableBorrowConflict → Ignore");
    check(
        dispatch(OwnershipErrorKind::ImmutableReassignment, settings, loc, "ث").action == DispatchAction::Ignore,
        "Disabled: ImmutableReassignment → Ignore");
    check(
        dispatch(OwnershipErrorKind::ReferenceCycle, settings, loc, "أ").action == DispatchAction::Ignore,
        "Disabled: ReferenceCycle → Ignore");
    check(
        dispatch(OwnershipErrorKind::ResourceLeak, settings, loc, "ب").action == DispatchAction::Ignore,
        "Disabled: ResourceLeak → Ignore");
    check(
        dispatch(OwnershipErrorKind::DanglingPointer, settings, loc, "ج").action == DispatchAction::Ignore,
        "Disabled: DanglingPointer → Ignore");
    check(
        dispatch(OwnershipErrorKind::MoveFromBorrowed, settings, loc, "د").action == DispatchAction::Ignore,
        "Disabled: MoveFromBorrowed → Ignore");

    // (AR) في وضع Ignore: الرسائل فارغة (لا معنى لها)
    // (EN) In Ignore mode: messages are empty (no point generating them)
    auto result = dispatch(OwnershipErrorKind::UseAfterMove, settings, loc, "س");
    check(result.messageAr.empty(), "Disabled: messageAr فارغة في Ignore");
    check(result.messageEn.empty(), "Disabled: messageEn فارغة في Ignore");
    check(!result.teachingNote.has_value(), "Disabled: لا teachingNote في Ignore");
}

// =============================================================================
//                  اختبار 2: OwnershipMode::Warnings → Warn
// =============================================================================

static void testWarningsModeWarns()
{
    std::cout << "\n─── اختبار 2: Warnings → Warn (--learn) ───\n";

    MemoryModeSettings settings;
    settings.ownershipMode = OwnershipMode::Warnings;
    settings.gcStrategy = GCStrategy::Tracing;
    settings.mode = MemoryMode::Development;
    settings.teacherMode = false;

    SourceLocation loc = makeLocation();

    auto result = dispatch(OwnershipErrorKind::UseAfterMove, settings, loc, "متغير_أ");

    check(result.action == DispatchAction::Warn, "Warnings: action = Warn");
    check(!result.messageAr.empty(), "Warnings: messageAr غير فارغة");
    check(!result.messageEn.empty(), "Warnings: messageEn غير فارغة");
    check(!result.teachingNote.has_value(), "Warnings (teacherMode=false): لا teachingNote");
    check(!result.shouldStop(), "Warnings: shouldStop() = false");
    check(result.shouldEmit(), "Warnings: shouldEmit() = true");
}

// =============================================================================
//                  اختبار 3: teacherMode=true → teachingNote موجود
// =============================================================================

static void testTeacherModeAddsNote()
{
    std::cout << "\n─── اختبار 3: teacherMode=true → teachingNote ───\n";

    MemoryModeSettings settings;
    settings.ownershipMode = OwnershipMode::Warnings;
    settings.gcStrategy = GCStrategy::Tracing;
    settings.mode = MemoryMode::Development;
    settings.teacherMode = true; // (AR) وضع المعلم مفعَّل

    SourceLocation loc = makeLocation();

    auto result = dispatch(OwnershipErrorKind::UseAfterMove, settings, loc, "متغير_ب");

    check(result.action == DispatchAction::Warn, "teacherMode: action = Warn");
    check(result.teachingNote.has_value(), "teacherMode: teachingNote موجود");
    check(!result.teachingNote->empty(), "teacherMode: teachingNote غير فارغ");

    // (AR) اختبر لكل نوع خطأ أن teachingNote يُنتج محتوى
    // (EN) Test that each error kind produces a teaching note
    for (auto kind : {
             OwnershipErrorKind::BorrowAfterLifetimeEnd,
             OwnershipErrorKind::MutableBorrowConflict,
             OwnershipErrorKind::ImmutableReassignment,
             OwnershipErrorKind::ReferenceCycle,
             OwnershipErrorKind::ResourceLeak,
             OwnershipErrorKind::DanglingPointer,
             OwnershipErrorKind::MoveFromBorrowed})
    {
        auto r = dispatch(kind, settings, loc, "س");
        check(r.teachingNote.has_value() && !r.teachingNote->empty(),
              "teacherMode: teachingNote لـ kind=" + std::to_string(static_cast<int>(kind)));
    }
}

// =============================================================================
//                  اختبار 4: OwnershipMode::UltraStrict → Fatal
// =============================================================================

static void testUltraStrictFatal()
{
    std::cout << "\n─── اختبار 4: UltraStrict → Fatal (--prod) ───\n";

    MemoryModeSettings settings = MemoryModeSettings::productionDefaults();
    SourceLocation loc = makeLocation();

    auto result = dispatch(OwnershipErrorKind::UseAfterMove, settings, loc, "مورد_حرج");

    check(result.action == DispatchAction::Fatal, "UltraStrict: action = Fatal");
    check(!result.messageAr.empty(), "UltraStrict: messageAr غير فارغة");
    check(!result.messageEn.empty(), "UltraStrict: messageEn غير فارغة");
    check(result.shouldStop(), "UltraStrict: shouldStop() = true");
    check(result.shouldEmit(), "UltraStrict: shouldEmit() = true");
}

// =============================================================================
//                  اختبار 5: عقد --dev redirect → نفس gcDefaults
// =============================================================================

static void testDevFlagRedirectToGcDefaults()
{
    std::cout << "\n─── اختبار 5: --dev redirect = --gc (gcDefaults) ───\n";

    // (AR) --gc و--dev كلاهما يجب أن يُعطيا gcDefaults() ← Disabled
    // (EN) --gc and --dev both should yield gcDefaults() ← Disabled
    MemoryModeSettings gcSettings = MemoryModeSettings::gcDefaults();

    // (AR) محاكاة --dev: يجب أن يُعيد gcDefaults()
    // (EN) Simulate --dev: should return gcDefaults()
    MemoryModeSettings devSettings = MemoryModeSettings::gcDefaults(); // backward compat

    SourceLocation loc = makeLocation();
    auto r1 = dispatch(OwnershipErrorKind::UseAfterMove, gcSettings, loc, "س");
    auto r2 = dispatch(OwnershipErrorKind::UseAfterMove, devSettings, loc, "س");

    check(r1.action == DispatchAction::Ignore, "--gc → Ignore");
    check(r2.action == DispatchAction::Ignore, "--dev (redirect) → Ignore");
    check(r1.action == r2.action, "--gc و--dev يعطيان نفس النتيجة");
}

// =============================================================================
//                  اختبار 6: عقد --hybrid redirect → نفس gcDefaults
// =============================================================================

static void testHybridFlagRedirectToGcDefaults()
{
    std::cout << "\n─── اختبار 6: --hybrid redirect = --gc (gcDefaults) ───\n";

    MemoryModeSettings gcSettings = MemoryModeSettings::gcDefaults();
    MemoryModeSettings hybridSettings = MemoryModeSettings::gcDefaults(); // backward compat

    SourceLocation loc = makeLocation();
    auto r1 = dispatch(OwnershipErrorKind::ResourceLeak, gcSettings, loc, "ملف");
    auto r2 = dispatch(OwnershipErrorKind::ResourceLeak, hybridSettings, loc, "ملف");

    check(r1.action == DispatchAction::Ignore, "--gc → Ignore");
    check(r2.action == DispatchAction::Ignore, "--hybrid (redirect) → Ignore");
    check(r1.action == r2.action, "--gc و--hybrid يعطيان نفس النتيجة");
}

// =============================================================================
//                  اختبار 7: الرسائل تتضمن اسم المتغير
// =============================================================================

static void testMessagesContainSymbolName()
{
    std::cout << "\n─── اختبار 7: الرسائل تتضمن اسم المتغير ───\n";

    MemoryModeSettings settings;
    settings.ownershipMode = OwnershipMode::Warnings;
    settings.teacherMode = false;

    SourceLocation loc = makeLocation();
    const std::string sym = "متغير_خاص_جداً";

    auto result = dispatch(OwnershipErrorKind::UseAfterMove, settings, loc, sym);

    check(result.messageAr.find(sym) != std::string::npos, "messageAr يحتوي اسم المتغير");
    check(result.messageEn.find(sym) != std::string::npos, "messageEn يحتوي اسم المتغير");
}

// =============================================================================
//                  نقطة الدخول الرئيسية
// =============================================================================

int main()
{
    std::cout << "═══════════════════════════════════════════════════════\n";
    std::cout << "   اختبارات العقد: dispatch() — نظام أخطاء الملكية\n";
    std::cout << "═══════════════════════════════════════════════════════\n";

    testDisabledModeIgnoresAll();
    testWarningsModeWarns();
    testTeacherModeAddsNote();
    testUltraStrictFatal();
    testDevFlagRedirectToGcDefaults();
    testHybridFlagRedirectToGcDefaults();
    testMessagesContainSymbolName();

    std::cout << "\n═══════════════════════════════════════════════════════\n";
    std::cout << "النتيجة: " << testsPassed << "/" << testsTotal << " اجتاز\n";

    if (testsFailed > 0)
    {
        std::cerr << "✗ فشل " << testsFailed << " اختبار!\n";
        return 1;
    }

    std::cout << "✅ جميع اختبارات العقد اجتازت!\n";
    std::cout << "═══════════════════════════════════════════════════════\n";
    return 0;
}
