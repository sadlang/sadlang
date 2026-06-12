// ════════════════════════════════════════════════════════════════════════════════
// ملف: test_harness.cpp
// File: test_harness.cpp
//
// الوصف: تنفيذ إطار اختبار bare-metal — ترجمة ← ربط ← QEMU ← فحص
// Description: Bare-metal test harness implementation — compile → link → QEMU → verify
//
// المؤلف: Sad Compiler — Phase ح
// Author: Sad Compiler — Phase ح
// ════════════════════════════════════════════════════════════════════════════════

#include "test_harness.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <array>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#endif

namespace sad {
namespace test {

// ════════════════════════════════════════════════════════════════════════════════
// QEMUConfig — الحصول على اسم QEMU / Get QEMU binary name
// ════════════════════════════════════════════════════════════════════════════════

std::string QEMUConfig::getQEMUBinary() const {
    switch (arch) {
        case Arch::X86_64:  return "qemu-system-x86_64";
        case Arch::ARM:     return "qemu-system-arm";
        case Arch::AArch64: return "qemu-system-aarch64";
        case Arch::RISCV64: return "qemu-system-riscv64";
    }
    return "qemu-system-x86_64";
}

// ════════════════════════════════════════════════════════════════════════════════
// TestReport — طباعة التقرير / Print report
// ════════════════════════════════════════════════════════════════════════════════

void TestReport::print() const {
    std::cout << "\n";
    std::cout << u8"══════════════════════════════════════════════════════\n";
    std::cout << u8"     تقرير اختبارات Bare-Metal / Bare-Metal Test Report\n";
    std::cout << u8"══════════════════════════════════════════════════════\n\n";
    
    for (const auto& result : results) {
        std::string statusStr;
        switch (result.status) {
            case TestStatus::Pass:         statusStr = u8"✓ نجاح / PASS"; break;
            case TestStatus::Fail:         statusStr = u8"✗ فشل / FAIL"; break;
            case TestStatus::Timeout:      statusStr = u8"⏱ انتهاء الوقت / TIMEOUT"; break;
            case TestStatus::CompileError: statusStr = u8"⚠ خطأ ترجمة / COMPILE ERROR"; break;
            case TestStatus::LinkError:    statusStr = u8"⚠ خطأ ربط / LINK ERROR"; break;
            case TestStatus::QEMUError:    statusStr = u8"⚠ خطأ QEMU / QEMU ERROR"; break;
            case TestStatus::Skipped:      statusStr = u8"⊘ تم التخطي / SKIPPED"; break;
        }
        
        std::cout << "  " << statusStr << " — " << result.testName;
        if (result.durationMs > 0) {
            std::cout << " (" << result.durationMs << " ms)";
        }
        if (result.kernelSize > 0) {
            std::cout << " [" << result.kernelSize << " bytes]";
        }
        std::cout << "\n";
        
        if (result.status == TestStatus::Fail && !result.errorMessage.empty()) {
            std::cout << "    " << result.errorMessage << "\n";
        }
    }
    
    std::cout << u8"\n──────────────────────────────────────────────────────\n";
    std::cout << u8"  الإجمالي / Total: " << totalTests << "\n";
    std::cout << u8"  نجح / Passed: " << passedTests << "\n";
    std::cout << u8"  فشل / Failed: " << failedTests << "\n";
    std::cout << u8"  تم التخطي / Skipped: " << skippedTests << "\n";
    std::cout << u8"  الوقت الكلي / Total time: " << totalDurationMs << " ms\n";
    std::cout << u8"══════════════════════════════════════════════════════\n";
    
    if (allPassed()) {
        std::cout << u8"\n  ✓✓✓ جميع الاختبارات نجحت / ALL TESTS PASSED ✓✓✓\n\n";
    } else {
        std::cout << u8"\n  ✗✗✗ بعض الاختبارات فشلت / SOME TESTS FAILED ✗✗✗\n\n";
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// BareMetalTestHarness — المُنشئ / Constructor
// ════════════════════════════════════════════════════════════════════════════════

BareMetalTestHarness::BareMetalTestHarness(const std::string& compilerPath)
    : compilerPath_(compilerPath), outputDir_("build/test_bare_metal") {}

// ════════════════════════════════════════════════════════════════════════════════
// الإعدادات / Configuration
// ════════════════════════════════════════════════════════════════════════════════

void BareMetalTestHarness::setQEMUConfig(const QEMUConfig& config) {
    qemuConfig_ = config;
}

void BareMetalTestHarness::setOutputDir(const std::string& dir) {
    outputDir_ = dir;
}

void BareMetalTestHarness::setVerbose(bool verbose) {
    verbose_ = verbose;
}

// ════════════════════════════════════════════════════════════════════════════════
// تسجيل الاختبارات / Register Tests
// ════════════════════════════════════════════════════════════════════════════════

void BareMetalTestHarness::addTest(const BareMetalTestCase& testCase) {
    tests_.push_back(testCase);
}

void BareMetalTestHarness::addTestDirectory(const std::string& dir, 
                                              const std::string& expectedOutputExt) {
    namespace fs = std::filesystem;
    
    if (!fs::exists(dir)) return;
    
    for (const auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".ص" || entry.path().extension() == u8".ص") {
            BareMetalTestCase tc;
            tc.name = entry.path().stem().string();
            tc.sourceFile = entry.path().string();
            
            // البحث عن ملف الإخراج المتوقع / Look for expected output file
            auto expectedFile = entry.path();
            expectedFile.replace_extension(expectedOutputExt);
            if (fs::exists(expectedFile)) {
                std::ifstream ifs(expectedFile);
                tc.expectedOutput = std::string(std::istreambuf_iterator<char>(ifs),
                                                  std::istreambuf_iterator<char>());
            }
            
            addTest(tc);
        }
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// التشغيل / Execution
// ════════════════════════════════════════════════════════════════════════════════

TestReport BareMetalTestHarness::runAll() {
    TestReport report;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // إنشاء مجلد الإخراج / Create output directory
    std::filesystem::create_directories(outputDir_);
    
    report.totalTests = static_cast<int>(tests_.size());
    
    for (const auto& testCase : tests_) {
        if (verbose_) {
            std::cout << u8"\n▶ تشغيل / Running: " << testCase.name << "...\n";
        }
        
        TestResult result = runTest(testCase);
        report.results.push_back(result);
        
        switch (result.status) {
            case TestStatus::Pass:    report.passedTests++; break;
            case TestStatus::Skipped: report.skippedTests++; break;
            default:                  report.failedTests++; break;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    report.totalDurationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    return report;
}

TestResult BareMetalTestHarness::runTest(const BareMetalTestCase& testCase) {
    TestResult result;
    result.testName = testCase.name;
    result.expectedOutput = testCase.expectedOutput;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // === الخطوة 1: التحقق من الملف / Step 1: Check source file ===
    if (!std::filesystem::exists(testCase.sourceFile)) {
        result.status = TestStatus::Fail;
        result.errorMessage = u8"ملف المصدر غير موجود / Source file not found: " + testCase.sourceFile;
        return result;
    }
    
    // === الخطوة 2: الترجمة / Step 2: Compile ===
    std::string objectFile;
    std::string compilerOutput;
    
    if (!compileSource(testCase, objectFile, compilerOutput)) {
        result.compilerOutput = compilerOutput;
        if (testCase.expectCompileError) {
            result.status = TestStatus::Pass;
        } else {
            result.status = TestStatus::CompileError;
            result.errorMessage = u8"فشل الترجمة / Compilation failed";
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        result.durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return result;
    }
    
    // === الخطوة 3: الربط / Step 3: Link ===
    std::string elfFile;
    std::string linkOutput;
    
    if (!linkObject(objectFile, testCase, elfFile, linkOutput)) {
        result.status = TestStatus::LinkError;
        result.errorMessage = u8"فشل الربط / Linking failed: " + linkOutput;
        auto endTime = std::chrono::high_resolution_clock::now();
        result.durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return result;
    }
    
    // حجم النواة / Kernel size
    try {
        result.kernelSize = std::filesystem::file_size(elfFile);
    } catch (...) {}
    
    // === الخطوة 4: التشغيل على QEMU / Step 4: Run on QEMU ===
    if (!isQEMUAvailable()) {
        result.status = TestStatus::Skipped;
        result.errorMessage = u8"QEMU غير متاح / QEMU not available";
        auto endTime = std::chrono::high_resolution_clock::now();
        result.durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return result;
    }
    
    std::string serialOutput;
    if (!runOnQEMU(elfFile, testCase, serialOutput, testCase.timeoutSeconds)) {
        result.status = TestStatus::QEMUError;
        result.errorMessage = u8"خطأ QEMU / QEMU error";
        auto endTime = std::chrono::high_resolution_clock::now();
        result.durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return result;
    }
    
    result.serialOutput = serialOutput;
    
    // === الخطوة 5: مقارنة الإخراج / Step 5: Compare output ===
    if (!testCase.expectedOutput.empty()) {
        if (compareOutput(serialOutput, testCase.expectedOutput)) {
            result.status = TestStatus::Pass;
        } else {
            result.status = TestStatus::Fail;
            result.errorMessage = u8"الإخراج لا يطابق المتوقع / Output doesn't match expected";
        }
    } else {
        // لا يوجد إخراج متوقع — نجاح إذا لم يتعطل
        result.status = TestStatus::Pass;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    return result;
}

// ════════════════════════════════════════════════════════════════════════════════
// الخطوات الداخلية / Internal Steps
// ════════════════════════════════════════════════════════════════════════════════

bool BareMetalTestHarness::compileSource(const BareMetalTestCase& testCase,
                                           std::string& outputFile,
                                           std::string& compilerOutput) {
    outputFile = outputDir_ + "/" + testCase.name + ".o";
    
    std::ostringstream cmd;
    cmd << "\"" << compilerPath_ << "\"";
    cmd << " --بلا-مكتبة-قياسية";
    cmd << " --هدف " << testCase.target;
    cmd << " -c";  // ترجمة فقط / Compile only
    cmd << " -o \"" << outputFile << "\"";
    
    for (const auto& flag : testCase.compilerFlags) {
        cmd << " " << flag;
    }
    
    cmd << " \"" << testCase.sourceFile << "\"";
    cmd << " 2>&1";
    
    if (verbose_) {
        std::cout << "  " << u8"ترجمة / Compile: " << cmd.str() << "\n";
    }
    
    // تنفيذ وجمع الإخراج / Execute and capture output
    std::array<char, 4096> buffer;
    std::string result;
    
#ifdef _WIN32
    FILE* pipe = _popen(cmd.str().c_str(), "r");
#else
    FILE* pipe = popen(cmd.str().c_str(), "r");
#endif
    
    if (!pipe) {
        compilerOutput = u8"فشل تشغيل المترجم / Failed to run compiler";
        return false;
    }
    
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }
    
#ifdef _WIN32
    int exitCode = _pclose(pipe);
#else
    int exitCode = pclose(pipe);
#endif
    
    compilerOutput = result;
    return exitCode == 0;
}

bool BareMetalTestHarness::linkObject(const std::string& objectFile,
                                        const BareMetalTestCase& testCase,
                                        std::string& elfFile,
                                        std::string& linkOutput) {
    elfFile = outputDir_ + "/" + testCase.name + ".elf";
    
    std::ostringstream cmd;
    cmd << "ld.lld -nostdlib";
    cmd << " -e _start";
    cmd << " --gc-sections";
    cmd << " -o \"" << elfFile << "\"";
    
    if (!testCase.linkerScript.empty()) {
        cmd << " -T \"" << testCase.linkerScript << "\"";
    }
    
    cmd << " \"" << objectFile << "\"";
    cmd << " 2>&1";
    
    if (verbose_) {
        std::cout << "  " << u8"ربط / Link: " << cmd.str() << "\n";
    }
    
    std::array<char, 4096> buffer;
    std::string result;
    
#ifdef _WIN32
    FILE* pipe = _popen(cmd.str().c_str(), "r");
#else
    FILE* pipe = popen(cmd.str().c_str(), "r");
#endif
    
    if (!pipe) {
        linkOutput = u8"فشل تشغيل الرابط / Failed to run linker";
        return false;
    }
    
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }
    
#ifdef _WIN32
    int exitCode = _pclose(pipe);
#else
    int exitCode = pclose(pipe);
#endif
    
    linkOutput = result;
    return exitCode == 0;
}

bool BareMetalTestHarness::runOnQEMU(const std::string& kernelFile,
                                       const BareMetalTestCase& testCase,
                                       std::string& serialOutput,
                                       int timeoutSeconds) {
    (void)testCase;  // للاستخدام المستقبلي / For future use
    
    std::ostringstream cmd;
    cmd << qemuConfig_.getQEMUBinary();
    cmd << " -kernel \"" << kernelFile << "\"";
    cmd << " -m " << qemuConfig_.memoryMB;
    cmd << " -machine " << qemuConfig_.machine;
    
    if (qemuConfig_.noGraphic) {
        cmd << " -nographic";
    }
    
    if (qemuConfig_.serialToStdio) {
        cmd << " -serial " << qemuConfig_.serialDevice;
    }
    
    // الخروج عند إيقاف المعالج / Exit on CPU halt
    cmd << " -no-reboot";
    cmd << " -device isa-debug-exit,iobase=0xf4,iosize=0x04";
    
    if (!qemuConfig_.extraArgs.empty()) {
        cmd << " " << qemuConfig_.extraArgs;
    }
    
    cmd << " 2>&1";
    
    if (verbose_) {
        std::cout << "  " << u8"QEMU: " << cmd.str() << "\n";
    }
    
    // تشغيل مع مهلة / Run with timeout
    std::array<char, 4096> buffer;
    std::string result;
    
#ifdef _WIN32
    // على Windows: استخدام CreateProcess مع timeout
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    
    HANDLE hReadPipe, hWritePipe;
    CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
    SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);
    
    si.hStdOutput = hWritePipe;
    si.hStdError = hWritePipe;
    si.dwFlags |= STARTF_USESTDHANDLES;
    
    std::string cmdStr = cmd.str();
    
    if (!CreateProcessA(NULL, cmdStr.data(), NULL, NULL, TRUE,
                         CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        serialOutput = u8"فشل تشغيل QEMU / Failed to start QEMU";
        CloseHandle(hReadPipe);
        CloseHandle(hWritePipe);
        return false;
    }
    
    CloseHandle(hWritePipe);
    
    DWORD waitResult = WaitForSingleObject(pi.hProcess, timeoutSeconds * 1000);
    
    if (waitResult == WAIT_TIMEOUT) {
        TerminateProcess(pi.hProcess, 1);
        serialOutput = u8"انتهاء الوقت / Timeout";
        CloseHandle(hReadPipe);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return false;
    }
    
    // قراءة الإخراج / Read output
    DWORD bytesRead;
    while (ReadFile(hReadPipe, buffer.data(), static_cast<DWORD>(buffer.size()) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result += buffer.data();
    }
    
    CloseHandle(hReadPipe);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
#else
    // على Unix: استخدام popen مع alarm
    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) {
        serialOutput = u8"فشل تشغيل QEMU / Failed to start QEMU";
        return false;
    }
    
    // TODO: إضافة timeout بـ alarm() أو select()
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        result += buffer.data();
    }
    
    pclose(pipe);
#endif
    
    serialOutput = result;
    return true;
}

bool BareMetalTestHarness::compareOutput(const std::string& actual, const std::string& expected) {
    // تنظيف المسافات البيضاء / Clean whitespace
    auto trim = [](std::string s) -> std::string {
        // إزالة المسافات من البداية والنهاية / Trim leading and trailing whitespace
        auto start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    };
    
    return trim(actual).find(trim(expected)) != std::string::npos;
}

// ════════════════════════════════════════════════════════════════════════════════
// التحقق من التوفر / Availability Checks
// ════════════════════════════════════════════════════════════════════════════════

bool BareMetalTestHarness::isQEMUAvailable() const {
    std::string cmd = qemuConfig_.getQEMUBinary() + " --version";
    
#ifdef _WIN32
    cmd += " >nul 2>&1";
#else
    cmd += " >/dev/null 2>&1";
#endif
    
    return std::system(cmd.c_str()) == 0;
}

bool BareMetalTestHarness::isCompilerAvailable() const {
    return std::filesystem::exists(compilerPath_);
}

} // namespace test
} // namespace sad
