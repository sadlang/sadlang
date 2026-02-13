// ════════════════════════════════════════════════════════════════════════════════
// ملف: test_harness.h
// File: test_harness.h
//
// الوصف: إطار اختبار bare-metal مع QEMU — ترجمة ← تشغيل ← فحص إخراج تسلسلي
// Description: Bare-metal test harness with QEMU — compile → run → check serial output
//
// المؤلف: Sad Compiler — Phase ح
// Author: Sad Compiler — Phase ح
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_TEST_BARE_METAL_HARNESS_H
#define SAD_TEST_BARE_METAL_HARNESS_H

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <map>

namespace sad {
namespace test {

// ════════════════════════════════════════════════════════════════════════════════
// نتيجة اختبار واحد / Single Test Result
// ════════════════════════════════════════════════════════════════════════════════

enum class TestStatus {
    Pass,           // نجاح / Pass
    Fail,           // فشل / Fail
    Timeout,        // انتهاء الوقت / Timeout
    CompileError,   // خطأ ترجمة / Compile error
    LinkError,      // خطأ ربط / Link error
    QEMUError,      // خطأ QEMU / QEMU error
    Skipped         // تم التخطي / Skipped
};

struct TestResult {
    std::string testName;
    TestStatus status = TestStatus::Fail;
    std::string serialOutput;      // إخراج المنفذ التسلسلي / Serial port output
    std::string expectedOutput;    // الإخراج المتوقع / Expected output
    std::string compilerOutput;    // إخراج المترجم / Compiler output
    std::string errorMessage;      // رسالة الخطأ / Error message
    double durationMs = 0.0;       // المدة بالملي ثانية / Duration in ms
    size_t kernelSize = 0;         // حجم النواة / Kernel size
};

// ════════════════════════════════════════════════════════════════════════════════
// تقرير الاختبارات / Test Report
// ════════════════════════════════════════════════════════════════════════════════

struct TestReport {
    std::vector<TestResult> results;
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    int skippedTests = 0;
    double totalDurationMs = 0.0;
    
    // طباعة التقرير / Print report
    void print() const;
    
    // التحقق من نجاح الكل / Check all passed
    bool allPassed() const { return failedTests == 0; }
};

// ════════════════════════════════════════════════════════════════════════════════
// إعدادات QEMU / QEMU Configuration
// ════════════════════════════════════════════════════════════════════════════════

struct QEMUConfig {
    std::string qemuPath = "qemu-system-x86_64";  // مسار QEMU / QEMU path
    std::string machine = "q35";                   // نوع الآلة / Machine type
    int memoryMB = 128;                            // الذاكرة بالميجا / Memory in MB
    int timeoutSeconds = 10;                       // المهلة بالثواني / Timeout in seconds
    bool noGraphic = true;                         // بدون رسومات / No graphics
    bool serialToStdio = true;                     // الإخراج التسلسلي لـ stdio
    std::string serialDevice = "stdio";            // جهاز تسلسلي / Serial device
    std::string extraArgs;                         // معاملات إضافية / Extra arguments
    
    // معمارية الهدف / Target architecture
    enum class Arch { X86_64, ARM, AArch64, RISCV64 } arch = Arch::X86_64;
    
    // الحصول على اسم QEMU للمعمارية / Get QEMU name for architecture
    std::string getQEMUBinary() const;
};

// ════════════════════════════════════════════════════════════════════════════════
// إعدادات اختبار واحد / Single Test Configuration
// ════════════════════════════════════════════════════════════════════════════════

struct BareMetalTestCase {
    std::string name;              // اسم الاختبار / Test name
    std::string sourceFile;        // ملف المصدر .ص / Source file .sad
    std::string expectedOutput;    // الإخراج المتوقع / Expected serial output
    std::string linkerScript;      // سكربت ربط مخصص (اختياري) / Custom linker script (optional)
    std::string target = "x86_64-unknown-none-elf";  // الهدف / Target
    int timeoutSeconds = 10;       // المهلة / Timeout
    bool expectCompileError = false; // نتوقع خطأ ترجمة / Expect compile error
    std::vector<std::string> compilerFlags;  // علامات مترجم إضافية / Extra compiler flags
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: BareMetalTestHarness
// Class: BareMetalTestHarness
//
// الوصف: إطار اختبار يترجم ملفات ص في وضع bare-metal ويشغلها على QEMU
// Description: Test framework that compiles .sad files in bare-metal mode and runs them on QEMU
// ════════════════════════════════════════════════════════════════════════════════

class BareMetalTestHarness {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئ / Constructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    explicit BareMetalTestHarness(const std::string& compilerPath);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإعدادات / Configuration
    // ═══════════════════════════════════════════════════════════════════════════
    
    void setQEMUConfig(const QEMUConfig& config);
    void setOutputDir(const std::string& dir);
    void setVerbose(bool verbose);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل الاختبارات / Register Tests
    // ═══════════════════════════════════════════════════════════════════════════
    
    void addTest(const BareMetalTestCase& testCase);
    void addTestDirectory(const std::string& dir, const std::string& expectedOutputExt = ".expected");
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التشغيل / Execution
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تشغيل جميع الاختبارات / Run all tests
    TestReport runAll();
    
    // تشغيل اختبار واحد / Run single test
    TestResult runTest(const BareMetalTestCase& testCase);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // مساعدات / Helpers
    // ═══════════════════════════════════════════════════════════════════════════
    
    // التحقق من توفر QEMU / Check QEMU availability
    bool isQEMUAvailable() const;
    
    // التحقق من توفر المترجم / Check compiler availability
    bool isCompilerAvailable() const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الخطوات الداخلية / Internal Steps
    // ═══════════════════════════════════════════════════════════════════════════
    
    // ترجمة ملف المصدر / Compile source file
    bool compileSource(const BareMetalTestCase& testCase, 
                       std::string& outputFile,
                       std::string& compilerOutput);
    
    // ربط الملف / Link the file
    bool linkObject(const std::string& objectFile, 
                     const BareMetalTestCase& testCase,
                     std::string& elfFile,
                     std::string& linkOutput);
    
    // تشغيل على QEMU / Run on QEMU
    bool runOnQEMU(const std::string& kernelFile, 
                    const BareMetalTestCase& testCase,
                    std::string& serialOutput,
                    int timeoutSeconds);
    
    // مقارنة الإخراج / Compare output
    bool compareOutput(const std::string& actual, const std::string& expected);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء / Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string compilerPath_;
    std::string outputDir_;
    QEMUConfig qemuConfig_;
    std::vector<BareMetalTestCase> tests_;
    bool verbose_ = false;
};

} // namespace test
} // namespace sad

#endif // SAD_TEST_BARE_METAL_HARNESS_H
