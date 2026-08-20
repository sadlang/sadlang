// ════════════════════════════════════════════════════════════════════════════════
// ملف: llvm_bare_metal_linker.cpp
// File: llvm_bare_metal_linker.cpp
//
// الوصف: تنفيذ رابط bare-metal للنواة
// Description: Implementation of bare-metal linker for kernel
//
// المؤلف: Sad Compiler — Phase ز
// Author: Sad Compiler — Phase ز
// ════════════════════════════════════════════════════════════════════════════════

#include "backend/llvm/llvm_bare_metal_linker.h"
#include "utf8_utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace sad {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئ والمُدمّر / Constructor and Destructor
// ════════════════════════════════════════════════════════════════════════════════

LLVMBareMetalLinker::LLVMBareMetalLinker()
    : initialized_(false) {}

LLVMBareMetalLinker::~LLVMBareMetalLinker() = default;

// ════════════════════════════════════════════════════════════════════════════════
// التهيئة / Initialization
// ════════════════════════════════════════════════════════════════════════════════

bool LLVMBareMetalLinker::initialize(BareMetalTarget target) {
    options_.target = target;
    options_.target_triple = getDefaultTriple(target);
    initialized_ = true;
    return true;
}

bool LLVMBareMetalLinker::initialize(const std::string& target_triple) {
    options_.target_triple = target_triple;
    options_.target = targetFromTriple(target_triple);
    initialized_ = true;
    return true;
}

// ════════════════════════════════════════════════════════════════════════════════
// الإعدادات / Configuration
// ════════════════════════════════════════════════════════════════════════════════

void LLVMBareMetalLinker::setOutputFile(const std::string& path) {
    options_.output_file = path;
}

void LLVMBareMetalLinker::setOutputFormat(BareMetalOutputFormat format) {
    options_.output_format = format;
}

void LLVMBareMetalLinker::setLinkerScript(const std::string& path) {
    options_.linker_script = path;
    options_.auto_generate_script = false;
}

void LLVMBareMetalLinker::setEntryPoint(const std::string& entry) {
    options_.entry_point = entry;
}

void LLVMBareMetalLinker::setAutoGenerateScript(bool enable) {
    options_.auto_generate_script = enable;
}

void LLVMBareMetalLinker::setStripSymbols(bool strip) {
    options_.strip_symbols = strip;
}

void LLVMBareMetalLinker::setGCSections(bool gc) {
    options_.gc_sections = gc;
}

void LLVMBareMetalLinker::setGenerateMap(bool enable, const std::string& mapFile) {
    options_.generate_map = enable;
    options_.map_file = mapFile;
}

void LLVMBareMetalLinker::setVerbose(bool verbose) {
    options_.verbose = verbose;
}

void LLVMBareMetalLinker::addObjectFile(const std::string& path) {
    options_.object_files.push_back(path);
}

void LLVMBareMetalLinker::addObjectFiles(const std::vector<std::string>& paths) {
    options_.object_files.insert(options_.object_files.end(), paths.begin(), paths.end());
}

void LLVMBareMetalLinker::addStaticLibrary(const std::string& path) {
    options_.static_libraries.push_back(path);
}

void LLVMBareMetalLinker::addLibraryPath(const std::string& path) {
    options_.library_paths.push_back(path);
}

void LLVMBareMetalLinker::addLinkerFlag(const std::string& flag) {
    options_.extra_flags.push_back(flag);
}

// ════════════════════════════════════════════════════════════════════════════════
// الربط الرئيسي / Main Linking
// ════════════════════════════════════════════════════════════════════════════════

BareMetalLinkResult LLVMBareMetalLinker::link() {
    return link(options_);
}

BareMetalLinkResult LLVMBareMetalLinker::link(const BareMetalLinkOptions& options) {
    BareMetalLinkResult result;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // === الخطوة 1: التحقق من التهيئة / Step 1: Check initialization ===
    if (!initialized_) {
        result.errors.push_back("الرابط غير مُهيّأ / Linker not initialized");
        return result;
    }
    
    // === الخطوة 2: التحقق من الملفات / Step 2: Verify files ===
    if (!verifyFiles(options)) {
        result.errors.push_back("فشل التحقق من الملفات / File verification failed");
        return result;
    }
    
    // === الخطوة 3: توليد سكربت الربط / Step 3: Generate linker script ===
    std::string scriptPath = options.linker_script;
    if (scriptPath.empty() && options.auto_generate_script) {
        std::string script = generateLinkerScript(options);
        if (script.empty()) {
            result.errors.push_back("فشل توليد سكربت الربط / Failed to generate linker script");
            return result;
        }
        
        // حفظ السكربت المُولد / Save generated script
        scriptPath = options.output_file + ".ld";
        std::ofstream scriptFile(scriptPath);
        if (scriptFile.is_open()) {
            scriptFile << script;
            scriptFile.close();
            result.generated_script = script;
        } else {
            result.errors.push_back("فشل حفظ سكربت الربط / Failed to save linker script");
            return result;
        }
    }
    
    // === الخطوة 4: بناء سطر الأوامر / Step 4: Build command line ===
    BareMetalLinkOptions effectiveOpts = options;
    if (!scriptPath.empty()) {
        effectiveOpts.linker_script = scriptPath;
    }
    
    std::string command = buildCommandLine(effectiveOpts);
    
    if (options.verbose) {
        std::cout << "=== أمر الربط / Link command ===" << std::endl;
        std::cout << command << std::endl;
    }
    
    // === الخطوة 5: تنفيذ الربط / Step 5: Execute linking ===
    bool linkSuccess = executeLinker(command);
    
    if (!linkSuccess) {
        result.errors.push_back("فشل تنفيذ الرابط / Linker execution failed");
        auto endTime = std::chrono::high_resolution_clock::now();
        result.linking_time_ms = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        return result;
    }
    
    // === الخطوة 6: التحويل إلى الصيغة المطلوبة / Step 6: Convert to requested format ===
    std::string elfOutput = effectiveOpts.output_file;
    
    if (options.output_format == BareMetalOutputFormat::FlatBinary) {
        std::string binOutput = elfOutput;
        // تغيير الامتداد / Change extension
        auto dotPos = binOutput.rfind('.');
        if (dotPos != std::string::npos) {
            binOutput = binOutput.substr(0, dotPos) + ".bin";
        } else {
            binOutput += ".bin";
        }
        
        if (!convertToFlatBinary(elfOutput, binOutput)) {
            result.warnings.push_back("فشل التحويل إلى ثنائي خام / Failed to convert to flat binary");
        } else {
            result.output_file = binOutput;
        }
    } else if (options.output_format == BareMetalOutputFormat::HEX) {
        std::string hexOutput = elfOutput;
        auto dotPos = hexOutput.rfind('.');
        if (dotPos != std::string::npos) {
            hexOutput = hexOutput.substr(0, dotPos) + ".hex";
        } else {
            hexOutput += ".hex";
        }
        
        if (!convertToHex(elfOutput, hexOutput)) {
            result.warnings.push_back("فشل التحويل إلى Intel HEX / Failed to convert to Intel HEX");
        } else {
            result.output_file = hexOutput;
        }
    } else {
        result.output_file = elfOutput;
    }
    
    // === الخطوة 7: جمع المعلومات / Step 7: Collect info ===
    auto endTime = std::chrono::high_resolution_clock::now();
    result.linking_time_ms = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // الحصول على حجم الملف الناتج / Get output file size
    try {
        if (std::filesystem::exists(result.output_file)) {
            result.output_size = std::filesystem::file_size(result.output_file);
        }
    } catch (...) {
        // تجاهل / Ignore
    }
    
    result.success = true;
    
    if (options.verbose) {
        std::cout << "✓ الربط نجح / Linking succeeded" << std::endl;
        std::cout << "  الملف / File: " << result.output_file << std::endl;
        std::cout << "  الحجم / Size: " << result.output_size << " bytes" << std::endl;
        std::cout << "  الوقت / Time: " << result.linking_time_ms << " ms" << std::endl;
    }
    
    lastResult_ = result;
    return result;
}

const BareMetalLinkResult& LLVMBareMetalLinker::getLastResult() const {
    return lastResult_;
}

// ════════════════════════════════════════════════════════════════════════════════
// بناء سطر الأوامر / Build Command Line
// ════════════════════════════════════════════════════════════════════════════════

std::string LLVMBareMetalLinker::buildCommandLine(const BareMetalLinkOptions& opts) {
    std::ostringstream cmd;
    
    // استخدام ld.lld إذا متاح، وإلا clang / Use ld.lld if available, else clang
    // نفضل LLD لأنه يدعم كل الأهداف / Prefer LLD as it supports all targets
    cmd << "ld.lld";
    
    // لا مكتبات قياسية أبداً / Never link standard libraries
    cmd << " -nostdlib";
    
    // نقطة الدخول / Entry point
    if (!opts.entry_point.empty()) {
        cmd << " -e " << opts.entry_point;
    }
    
    // سكربت الربط / Linker script
    if (!opts.linker_script.empty()) {
        cmd << " -T \"" << opts.linker_script << "\"";
    }
    
    // ملف الإخراج / Output file
    cmd << " -o \"" << opts.output_file << "\"";
    
    // حذف الأقسام غير المستخدمة / Remove unused sections
    if (opts.gc_sections) {
        cmd << " --gc-sections";
    }
    
    // إزالة الرموز / Strip symbols
    if (opts.strip_symbols) {
        cmd << " -s";
    }
    
    // ملف الخريطة / Map file
    if (opts.generate_map) {
        std::string mapPath = opts.map_file;
        if (mapPath.empty()) {
            mapPath = opts.output_file + ".map";
        }
        cmd << " -Map \"" << mapPath << "\"";
    }
    
    // قابل لإعادة التموضع / Relocatable
    if (opts.relocatable) {
        cmd << " -r";
    }
    
    // مسارات المكتبات / Library paths
    for (const auto& path : opts.library_paths) {
        cmd << " -L \"" << path << "\"";
    }
    
    // ملفات الكائنات / Object files
    for (const auto& obj : opts.object_files) {
        cmd << " \"" << obj << "\"";
    }
    
    // المكتبات الثابتة / Static libraries
    for (const auto& lib : opts.static_libraries) {
        cmd << " \"" << lib << "\"";
    }
    
    // العلامات الإضافية / Extra flags
    for (const auto& flag : opts.extra_flags) {
        cmd << " " << flag;
    }
    
    return cmd.str();
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الرابط / Execute Linker
// ════════════════════════════════════════════════════════════════════════════════

bool LLVMBareMetalLinker::executeLinker(const std::string& command) {
#ifdef _WIN32
    // تنفيذ على Windows باستخدام CreateProcess
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    // 🔑 (AR) عريضةٌ لا ضيّقة: الضيّقةُ تُشوّه المسارَ العربيَّ وتنجح صامتةً.
    // 🔑 (EN) Wide, not narrow: the narrow one mangles Arabic paths silently.
    std::wstring cmdCopy = sad::utf8::to_wstring(command);
    
    if (!CreateProcessW(
        NULL,
        cmdCopy.data(),
        NULL, NULL, FALSE,
        CREATE_NO_WINDOW,
        NULL, NULL,
        &si, &pi)) {
        
        // محاولة مع clang كبديل / Try clang as fallback
        std::string fallback = "clang -target " + options_.target_triple 
            + " -nostdlib -fuse-ld=lld " + command.substr(6);  // skip "ld.lld"
        cmdCopy = sad::utf8::to_wstring(fallback);
        
        if (!CreateProcessW(
            NULL,
            cmdCopy.data(),
            NULL, NULL, FALSE,
            CREATE_NO_WINDOW,
            NULL, NULL,
            &si, &pi)) {
            return false;
        }
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    return exitCode == 0;
#else
    // تنفيذ على Unix باستخدام system()
    int ret = sad::utf8::run_command(command);
    return WIFEXITED(ret) && WEXITSTATUS(ret) == 0;
#endif
}

// ════════════════════════════════════════════════════════════════════════════════
// توليد سكربت الربط / Generate Linker Script
// ════════════════════════════════════════════════════════════════════════════════

std::string LLVMBareMetalLinker::generateLinkerScript(const BareMetalLinkOptions& opts) {
    std::ostringstream script;
    
    // ترويسة / Header
    script << "/* سكربت ربط مُولد بواسطة مترجم ص */\n";
    script << "/* Auto-generated linker script by Sad compiler */\n\n";
    
    // نقطة الدخول / Entry point
    script << "ENTRY(" << opts.entry_point << ")\n\n";
    
    switch (opts.target) {
        case BareMetalTarget::X86_64_Bare:
        case BareMetalTarget::X86_32_Bare: {
            // === x86 Bare-Metal Kernel ===
            uint64_t loadAddr = (opts.target == BareMetalTarget::X86_64_Bare) 
                                 ? 0x100000 : 0x100000;  // 1MB
            
            script << "SECTIONS\n{\n";
            script << "    . = " << "0x" << std::hex << loadAddr << std::dec << ";\n\n";
            
            // قسم Multiboot / Multiboot section
            script << "    .multiboot ALIGN(4) : {\n";
            script << "        KEEP(*(.multiboot))\n";
            script << "    }\n\n";
            
            // قسم الكود / Text section
            script << "    .text ALIGN(4096) : {\n";
            script << "        __text_start = .;\n";
            script << "        *(.text .text.*)\n";
            script << "        __text_end = .;\n";
            script << "    }\n\n";
            
            // قسم القراءة فقط / Read-only data
            script << "    .rodata ALIGN(4096) : {\n";
            script << "        __rodata_start = .;\n";
            script << "        *(.rodata .rodata.*)\n";
            script << "        __rodata_end = .;\n";
            script << "    }\n\n";
            
            // قسم البيانات / Data section
            script << "    .data ALIGN(4096) : {\n";
            script << "        __data_start = .;\n";
            script << "        *(.data .data.*)\n";
            script << "        __data_end = .;\n";
            script << "    }\n\n";
            
            // قسم BSS / BSS section
            script << "    .bss ALIGN(4096) : {\n";
            script << "        __bss_start = .;\n";
            script << "        *(.bss .bss.*)\n";
            script << "        *(COMMON)\n";
            script << "        __bss_end = .;\n";
            script << "    }\n\n";
            
            // جداول المقاطعات / Interrupt tables
            script << "    .interrupt_table ALIGN(4096) : {\n";
            script << "        KEEP(*(.interrupt_table))\n";
            script << "    }\n\n";
            
            // جداول الصفحات / Page tables
            script << "    .page_tables ALIGN(4096) : {\n";
            script << "        KEEP(*(.page_tables))\n";
            script << "    }\n\n";
            
            // المكدس / Stack
            script << "    . = ALIGN(16);\n";
            script << "    __stack_bottom = .;\n";
            script << "    . = . + 0x4000; /* 16KB stack */\n";
            script << "    __stack_top = .;\n\n";
            
            // نهاية النواة / Kernel end
            script << "    __kernel_end = .;\n";
            script << "}\n";
            break;
        }
        
        case BareMetalTarget::ARM_CortexM: {
            // === ARM Cortex-M ===
            script << "MEMORY\n{\n";
            script << "    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K\n";
            script << "    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K\n";
            script << "}\n\n";
            
            script << "SECTIONS\n{\n";
            
            // متجه ISR / ISR vector
            script << "    .isr_vector : {\n";
            script << "        KEEP(*(.isr_vector))\n";
            script << "    } > FLASH\n\n";
            
            script << "    .text : {\n";
            script << "        *(.text .text.*)\n";
            script << "    } > FLASH\n\n";
            
            script << "    .rodata : {\n";
            script << "        *(.rodata .rodata.*)\n";
            script << "    } > FLASH\n\n";
            
            script << "    __data_load = LOADADDR(.data);\n";
            script << "    .data : {\n";
            script << "        __data_start = .;\n";
            script << "        *(.data .data.*)\n";
            script << "        __data_end = .;\n";
            script << "    } > RAM AT > FLASH\n\n";
            
            script << "    .bss : {\n";
            script << "        __bss_start = .;\n";
            script << "        *(.bss .bss.*)\n";
            script << "        *(COMMON)\n";
            script << "        __bss_end = .;\n";
            script << "    } > RAM\n\n";
            
            script << "    . = ALIGN(8);\n";
            script << "    __stack_top = ORIGIN(RAM) + LENGTH(RAM);\n";
            script << "}\n";
            break;
        }
        
        case BareMetalTarget::AARCH64_Bare:
        case BareMetalTarget::ARM_CortexA: {
            // === AArch64 / Cortex-A ===
            script << "SECTIONS\n{\n";
            script << "    . = 0x80000; /* Raspberry Pi 3/4 load address */\n\n";
            script << "    .text : { *(.text .text.*) }\n";
            script << "    .rodata : { *(.rodata .rodata.*) }\n";
            script << "    .data : { *(.data .data.*) }\n";
            script << "    .bss : {\n";
            script << "        __bss_start = .;\n";
            script << "        *(.bss .bss.*)\n";
            script << "        *(COMMON)\n";
            script << "        __bss_end = .;\n";
            script << "    }\n";
            script << "    __kernel_end = .;\n";
            script << "}\n";
            break;
        }
        
        case BareMetalTarget::RISCV32_Bare:
        case BareMetalTarget::RISCV64_Bare: {
            // === RISC-V ===
            script << "SECTIONS\n{\n";
            script << "    . = 0x80000000;\n\n";
            script << "    .text : { *(.text .text.*) }\n";
            script << "    .rodata : { *(.rodata .rodata.*) }\n";
            script << "    .data : { *(.data .data.*) }\n";
            script << "    .bss : {\n";
            script << "        __bss_start = .;\n";
            script << "        *(.bss .bss.*)\n";
            script << "        __bss_end = .;\n";
            script << "    }\n";
            script << "    . = ALIGN(16);\n";
            script << "    . = . + 0x4000;\n";
            script << "    __stack_top = .;\n";
            script << "}\n";
            break;
        }
    }
    
    return script.str();
}

// ════════════════════════════════════════════════════════════════════════════════
// التحويلات / Conversions
// ════════════════════════════════════════════════════════════════════════════════

bool LLVMBareMetalLinker::convertToFlatBinary(const std::string& elfPath, const std::string& binPath) {
    // استخدام llvm-objcopy / Use llvm-objcopy
    std::ostringstream cmd;
    cmd << "llvm-objcopy -O binary \"" << elfPath << "\" \"" << binPath << "\"";
    
    int ret = sad::utf8::run_command(cmd.str());
    return ret == 0;
}

bool LLVMBareMetalLinker::convertToHex(const std::string& elfPath, const std::string& hexPath) {
    // استخدام llvm-objcopy / Use llvm-objcopy
    std::ostringstream cmd;
    cmd << "llvm-objcopy -O ihex \"" << elfPath << "\" \"" << hexPath << "\"";
    
    int ret = sad::utf8::run_command(cmd.str());
    return ret == 0;
}

// ════════════════════════════════════════════════════════════════════════════════
// التحقق من الملفات / File Verification
// ════════════════════════════════════════════════════════════════════════════════

bool LLVMBareMetalLinker::verifyFiles(const BareMetalLinkOptions& opts) {
    if (opts.object_files.empty()) {
        if (options_.verbose) {
            std::cerr << "✗ لا توجد ملفات كائنات / No object files" << std::endl;
        }
        return false;
    }
    
    for (const auto& obj : opts.object_files) {
        if (!std::filesystem::exists(obj)) {
            if (options_.verbose) {
                std::cerr << "✗ ملف غير موجود / File not found: " << obj << std::endl;
            }
            return false;
        }
    }
    
    // التحقق من سكربت الربط إذا مُوفّر / Check linker script if provided
    if (!opts.linker_script.empty() && !std::filesystem::exists(opts.linker_script)) {
        if (options_.verbose) {
            std::cerr << "✗ سكربت الربط غير موجود / Linker script not found: " 
                      << opts.linker_script << std::endl;
        }
        return false;
    }
    
    return true;
}

// ════════════════════════════════════════════════════════════════════════════════
// الدوال الثابتة / Static Functions
// ════════════════════════════════════════════════════════════════════════════════

BareMetalTarget LLVMBareMetalLinker::targetFromTriple(const std::string& triple) {
    // x86_64 bare-metal
    if (triple.find("x86_64") != std::string::npos && 
        (triple.find("none") != std::string::npos || triple.find("bare") != std::string::npos ||
         triple.find("elf") != std::string::npos)) {
        return BareMetalTarget::X86_64_Bare;
    }
    
    // i686 bare-metal
    if ((triple.find("i686") != std::string::npos || triple.find("i386") != std::string::npos) &&
        (triple.find("none") != std::string::npos || triple.find("bare") != std::string::npos ||
         triple.find("elf") != std::string::npos)) {
        return BareMetalTarget::X86_32_Bare;
    }
    
    // ARM Cortex-M (Thumb)
    if (triple.find("thumb") != std::string::npos || 
        (triple.find("arm") != std::string::npos && triple.find("none-eabi") != std::string::npos)) {
        return BareMetalTarget::ARM_CortexM;
    }
    
    // AArch64
    if (triple.find("aarch64") != std::string::npos && triple.find("none") != std::string::npos) {
        return BareMetalTarget::AARCH64_Bare;
    }
    
    // RISC-V 64
    if (triple.find("riscv64") != std::string::npos) {
        return BareMetalTarget::RISCV64_Bare;
    }
    
    // RISC-V 32
    if (triple.find("riscv32") != std::string::npos) {
        return BareMetalTarget::RISCV32_Bare;
    }
    
    // ARM Cortex-A
    if (triple.find("arm") != std::string::npos) {
        return BareMetalTarget::ARM_CortexA;
    }
    
    // الافتراضي / Default
    return BareMetalTarget::X86_64_Bare;
}

bool LLVMBareMetalLinker::isBareMetalTriple(const std::string& triple) {
    // الأهداف bare-metal تحتوي عادةً "none" أو "bare" أو "elf" بدون نظام
    // Bare-metal targets typically contain "none", "bare", or "elf" without OS
    if (triple.find("none") != std::string::npos) return true;
    if (triple.find("bare") != std::string::npos) return true;
    if (triple.find("unknown-elf") != std::string::npos) return true;
    if (triple.find("none-eabi") != std::string::npos) return true;
    return false;
}

std::string LLVMBareMetalLinker::getDefaultTriple(BareMetalTarget target) {
    switch (target) {
        case BareMetalTarget::X86_64_Bare:  return "x86_64-unknown-none-elf";
        case BareMetalTarget::X86_32_Bare:  return "i686-unknown-none-elf";
        case BareMetalTarget::ARM_CortexM:  return "thumbv7em-none-eabi";
        case BareMetalTarget::ARM_CortexA:  return "armv7-unknown-none-eabi";
        case BareMetalTarget::RISCV32_Bare: return "riscv32-unknown-none-elf";
        case BareMetalTarget::RISCV64_Bare: return "riscv64-unknown-none-elf";
        case BareMetalTarget::AARCH64_Bare: return "aarch64-unknown-none-elf";
    }
    return "x86_64-unknown-none-elf";
}

} // namespace sad
