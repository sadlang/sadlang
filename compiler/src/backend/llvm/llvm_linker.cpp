/*
 * تنفيذ الرابط LLVM / LLVM Linker Implementation
 * ==============================================
 * 
 * تم التحسين بنظام كشف سلسلة الأدوات / Enhanced with Toolchain Detection System
 */

#include "llvm_linker.h"
#include "llvm_bare_metal_linker.h"
#include "toolchain_detection.h"
#include "utf8_utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace sad {

// الحصول على مرجع لمدير سلسلة الأدوات / Get reference to toolchain manager
static ToolchainManager& getToolchainManager() {
    return ToolchainManager::getInstance();
}

/**
 * المُنشئ / Constructor
 */
LLVMLinker::LLVMLinker()
    : initialized_(false),
      use_lld_(false) {
    // تهيئة مدير سلسلة الأدوات / Initialize toolchain manager
    getToolchainManager().initialize();
}

/**
 * المُدمر / Destructor
 */
LLVMLinker::~LLVMLinker() {
}

/**
 * تهيئة الرابط / Initialize linker
 */
bool LLVMLinker::initialize(const std::string& target_triple) {
    if (initialized_) {
        return true;
    }
    
    if (target_triple.empty()) {
        #ifdef _WIN32
            target_triple_ = "x86_64-pc-windows-msvc";
        #elif defined(__APPLE__)
            target_triple_ = "x86_64-apple-darwin";
        #else
            target_triple_ = "x86_64-unknown-linux-gnu";
        #endif
    } else {
        target_triple_ = target_triple;
    }
    
    options_.target_triple = target_triple_;
    
    // التحقق من توفر LLD / Check if LLD is available
    use_lld_ = isLLDAvailable();
    
    initialized_ = true;
    return true;
}

/**
 * تعيين نوع الربط / Set linking type
 */
void LLVMLinker::setLinkingType(LinkingType type) {
    options_.type = type;
}

/**
 * تعيين نمط الربط / Set linking mode
 */
void LLVMLinker::setLinkingMode(LinkingMode mode) {
    options_.mode = mode;
}

/**
 * تعيين مستوى LTO / Set LTO level
 */
void LLVMLinker::setLTOLevel(LTOLevel level) {
    options_.lto_level = level;
}

/**
 * تعيين ملف الإخراج / Set output file
 */
void LLVMLinker::setOutputFile(const std::string& filename) {
    options_.output_file = filename;
}

/**
 * تعيين نقطة الدخول / Set entry point
 */
void LLVMLinker::setEntryPoint(const std::string& entry_point) {
    options_.entry_point = entry_point;
}

/**
 * إضافة ملف كائن / Add object file
 */
void LLVMLinker::addObjectFile(const std::string& filename) {
    options_.object_files.push_back(filename);
}

/**
 * إضافة عدة ملفات كائنات / Add multiple object files
 */
void LLVMLinker::addObjectFiles(const std::vector<std::string>& filenames) {
    for (const auto& filename : filenames) {
        options_.object_files.push_back(filename);
    }
}

/**
 * إضافة مكتبة / Add library
 */
void LLVMLinker::addLibrary(const std::string& library) {
    options_.libraries.push_back(library);
}

/**
 * إضافة عدة مكتبات / Add multiple libraries
 */
void LLVMLinker::addLibraries(const std::vector<std::string>& libraries) {
    for (const auto& library : libraries) {
        options_.libraries.push_back(library);
    }
}

/**
 * إضافة مسار مكتبات / Add library path
 */
void LLVMLinker::addLibraryPath(const std::string& path) {
    options_.library_paths.push_back(path);
}

/**
 * إضافة عدة مسارات مكتبات / Add multiple library paths
 */
void LLVMLinker::addLibraryPaths(const std::vector<std::string>& paths) {
    for (const auto& path : paths) {
        options_.library_paths.push_back(path);
    }
}

/**
 * إضافة علم رابط / Add linker flag
 */
void LLVMLinker::addLinkerFlag(const std::string& flag) {
    options_.linker_flags.push_back(flag);
}

/**
 * تعيين إزالة الرموز / Set strip symbols
 */
void LLVMLinker::setStripSymbols(bool strip) {
    options_.strip_symbols = strip;
}

/**
 * تعيين إنشاء ملف خريطة / Set generate map file
 */
void LLVMLinker::setGenerateMap(bool generate, const std::string& map_file) {
    options_.generate_map = generate;
    if (!map_file.empty()) {
        options_.map_file = map_file;
    } else if (generate && !options_.output_file.empty()) {
        options_.map_file = options_.output_file + ".map";
    }
}

/**
 * تعيين بدون مكتبة قياسية / Set no stdlib
 */
void LLVMLinker::setNoStdlib(bool no_stdlib) {
    options_.no_stdlib = no_stdlib;
}

/**
 * تعيين بدون مكتبات افتراضية / Set no default libs
 */
void LLVMLinker::setNoDefaultLibs(bool no_default_libs) {
    options_.no_default_libs = no_default_libs;
}

/**
 * تعيين مطوّل / Set verbose
 */
void LLVMLinker::setVerbose(bool verbose) {
    options_.verbose = verbose;
}

/**
 * الربط / Link
 */
bool LLVMLinker::link() {
    return link(options_);
}

/**
 * الربط مع خيارات مخصصة / Link with custom options
 */
bool LLVMLinker::link(const LinkingOptions& options) {
    if (!initialized_) {
        std::cerr << "خطأ: الرابط غير مهيأ / Error: Linker not initialized" << std::endl;
        return false;
    }
    
    // التحقق من الملفات / Verify files
    if (!verifyFiles(options)) {
        std::cerr << "خطأ: التحقق من الملفات فشل / Error: File verification failed" << std::endl;
        return false;
    }
    
    // إضافة المكتبات الافتراضية / Add default libraries
    LinkingOptions opts = options;
    if (!opts.no_default_libs) {
        addDefaultLibraries(opts);
    }
    
    // === فحص هدف bare-metal / Check for bare-metal target ===
    if (LLVMBareMetalLinker::isBareMetalTriple(target_triple_) || opts.no_stdlib) {
        // استخدام رابط bare-metal / Use bare-metal linker
        LLVMBareMetalLinker bmLinker;
        bmLinker.initialize(target_triple_);
        bmLinker.setOutputFile(opts.output_file);
        bmLinker.setVerbose(opts.verbose);
        bmLinker.setStripSymbols(opts.strip_symbols);
        bmLinker.setGenerateMap(opts.generate_map);
        
        if (!opts.entry_point.empty()) {
            bmLinker.setEntryPoint(opts.entry_point);
        }
        
        for (const auto& obj : opts.object_files) {
            bmLinker.addObjectFile(obj);
        }
        for (const auto& flag : opts.linker_flags) {
            bmLinker.addLinkerFlag(flag);
        }
        
        auto bmResult = bmLinker.link();
        
        info_.output_size = bmResult.output_size;
        info_.linking_time_ms = bmResult.linking_time_ms;
        info_.warnings = bmResult.warnings;
        info_.errors = bmResult.errors;
        
        return bmResult.success;
    }
    
    // بدء قياس الوقت / Start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // إعادة تعيين المعلومات / Reset info
    info_ = LinkingInfo();
    
    bool success = false;
    
    // محاولة استخدام LLD أولاً / Try using LLD first
    if (use_lld_) {
        success = useLLD(opts);
    }
    
    // إذا فشل LLD، استخدم رابط النظام / If LLD fails, use system linker
    if (!success) {
        success = useSystemLinker(opts);
    }
    
    // حساب وقت الربط / Calculate linking time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    info_.linking_time_ms = duration.count() / 1000.0;
    
    // الحصول على حجم الملف المخرج / Get output file size
    if (success && !opts.output_file.empty()) {
        std::ifstream file(opts.output_file, std::ios::binary | std::ios::ate);
        if (file.is_open()) {
            info_.output_size = file.tellg();
            file.close();
        }
    }
    
    if (opts.verbose) {
        printLinkingInfo();
    }
    
    return success;
}

/**
 * الحصول على معلومات الربط / Get linking information
 */
const LinkingInfo& LLVMLinker::getLinkingInfo() const {
    return info_;
}

/**
 * طباعة معلومات الربط / Print linking information
 */
void LLVMLinker::printLinkingInfo() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "معلومات الربط / Linking Information" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "عدد الرموز / Total Symbols: " << info_.total_symbols << std::endl;
    std::cout << "رموز غير معرّفة / Undefined Symbols: " << info_.undefined_symbols << std::endl;
    std::cout << "حجم الإخراج / Output Size: " << info_.output_size << " bytes" << std::endl;
    std::cout << "وقت الربط / Linking Time: " << info_.linking_time_ms << " ms" << std::endl;
    
    if (!info_.warnings.empty()) {
        std::cout << "\nالتحذيرات / Warnings:" << std::endl;
        for (const auto& warning : info_.warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    }
    
    if (!info_.errors.empty()) {
        std::cout << "\nالأخطاء / Errors:" << std::endl;
        for (const auto& error : info_.errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    std::cout << "========================================\n" << std::endl;
}

/**
 * الحصول على أعلام الرابط الافتراضية / Get default linker flags
 */
std::vector<std::string> LLVMLinker::getDefaultLinkerFlags(const std::string& target_triple) {
    std::vector<std::string> flags;
    
    if (target_triple.find("windows") != std::string::npos) {
        // Windows
        if (target_triple.find("msvc") != std::string::npos) {
            flags.push_back("/SUBSYSTEM:CONSOLE");
            flags.push_back("/MACHINE:X64");
        } else {
            // MinGW
            flags.push_back("-Wl,--subsystem,console");
        }
    } else if (target_triple.find("darwin") != std::string::npos) {
        // macOS
        flags.push_back("-arch");
        flags.push_back("x86_64");
    } else {
        // Linux and others
        flags.push_back("-pie");
    }
    
    return flags;
}

/**
 * الحصول على المكتبات الافتراضية / Get default libraries
 */
std::vector<std::string> LLVMLinker::getDefaultLibraries(const std::string& target_triple) {
    std::vector<std::string> libraries;
    
    if (target_triple.find("windows") != std::string::npos) {
        // Windows
        if (target_triple.find("msvc") != std::string::npos) {
            libraries.push_back("msvcrt");
            libraries.push_back("kernel32");
            libraries.push_back("user32");
        } else {
            // MinGW
            libraries.push_back("mingw32");
            libraries.push_back("gcc");
            libraries.push_back("kernel32");
        }
    } else if (target_triple.find("darwin") != std::string::npos) {
        // macOS
        libraries.push_back("System");
    } else {
        // Linux and others
        libraries.push_back("c");
        libraries.push_back("m");
        libraries.push_back("pthread");
        libraries.push_back("dl");
    }
    
    return libraries;
}

/**
 * التحقق من توفر LLD / Check if LLD is available
 */
bool LLVMLinker::isLLDAvailable() {
    // محاولة تشغيل lld / Try to run lld
    #ifdef _WIN32
        int result = std::system("lld --version >nul 2>&1");
    #else
        int result = std::system("lld --version >/dev/null 2>&1");
    #endif
    
    return result == 0;
}

/**
 * بناء سطر أوامر الرابط / Build linker command line
 */
std::string LLVMLinker::buildLinkerCommandLine(const LinkingOptions& options) {
    std::ostringstream cmd;
    
    bool is_windows = target_triple_.find("windows") != std::string::npos;
    bool is_msvc = target_triple_.find("msvc") != std::string::npos;
    
    if (is_windows && is_msvc) {
        // MSVC linker - استخدام سلسلة الأدوات المكتشفة / Using detected toolchain
        std::string linker_path = "link.exe";
        std::vector<std::string> lib_paths;
        
        auto& tcm = getToolchainManager();
        const auto* tc = tcm.getActiveToolchain();
        if (tc && tc->type == ToolchainType::MSVC && tc->linker.available) {
            linker_path = tc->linker.path;
            lib_paths = tc->library_paths;
        }
        
        cmd << "\"" << linker_path << "\"";
        cmd << " /NOLOGO";
        
        // نوع الإخراج / Output type
        if (options.type == LinkingType::SharedLibrary) {
            cmd << " /DLL";
        }
        
        // ملف الإخراج / Output file
        if (!options.output_file.empty()) {
            cmd << " /OUT:\"" << options.output_file << "\"";
        }
        
        // ملفات الكائنات / Object files
        for (const auto& obj : options.object_files) {
            cmd << " \"" << obj << "\"";
        }
        
        // مسارات المكتبات من سلسلة الأدوات / Library paths from toolchain
        for (const auto& path : lib_paths) {
            cmd << " /LIBPATH:\"" << path << "\"";
        }
        
        // مسارات المكتبات المخصصة / Custom library paths
        for (const auto& path : options.library_paths) {
            cmd << " /LIBPATH:\"" << path << "\"";
        }
        
        // المكتبات / Libraries
        for (const auto& lib : options.libraries) {
            // إضافة .lib إذا لم تكن موجودة / Add .lib if not present
            if (lib.find(".lib") == std::string::npos) {
                cmd << " " << lib << ".lib";
            } else {
                cmd << " " << lib;
            }
        }
        
        // إزالة الرموز / Strip symbols
        if (options.strip_symbols) {
            cmd << " /DEBUG:NONE";
        }
        
        // ملف الخريطة / Map file
        if (options.generate_map) {
            cmd << " /MAP:\"" << (options.map_file.empty() ? options.output_file + ".map" : options.map_file) << "\"";
        }
        
        // الأعلام الإضافية / Additional flags
        for (const auto& flag : options.linker_flags) {
            cmd << " " << flag;
        }
        
    } else {
        // Unix-like linker (ld, lld, clang)
        if (use_lld_) {
            cmd << "ld.lld";
        } else {
            cmd << "clang";
        }
        
        // نوع الإخراج / Output type
        if (options.type == LinkingType::SharedLibrary) {
            cmd << " -shared";
        }
        
        // نمط الربط / Linking mode
        if (options.mode == LinkingMode::Static) {
            cmd << " -static";
        } else if (options.mode == LinkingMode::PIE) {
            cmd << " -pie";
        }
        
        // ملف الإخراج / Output file
        if (!options.output_file.empty()) {
            cmd << " -o " << options.output_file;
        }
        
        // ملفات الكائنات / Object files
        for (const auto& obj : options.object_files) {
            cmd << " " << obj;
        }
        
        // مسارات المكتبات / Library paths
        for (const auto& path : options.library_paths) {
            cmd << " -L" << path;
        }
        
        // المكتبات / Libraries
        for (const auto& lib : options.libraries) {
            cmd << " -l" << lib;
        }
        
        // LTO
        if (options.lto_level == LTOLevel::Thin) {
            cmd << " -flto=thin";
        } else if (options.lto_level == LTOLevel::Full) {
            cmd << " -flto";
        }
        
        // إزالة الرموز / Strip symbols
        if (options.strip_symbols) {
            cmd << " -s";
        }
        
        // ملف الخريطة / Map file
        if (options.generate_map) {
            std::string map_file = options.map_file.empty() ? options.output_file + ".map" : options.map_file;
            cmd << " -Wl,-Map," << map_file;
        }
        
        // بدون مكتبة قياسية / No stdlib
        if (options.no_stdlib) {
            cmd << " -nostdlib";
        }
        
        // نقطة الدخول / Entry point
        if (!options.entry_point.empty() && options.entry_point != "main") {
            cmd << " -e " << options.entry_point;
        }
        
        // الأعلام الإضافية / Additional flags
        for (const auto& flag : options.linker_flags) {
            cmd << " " << flag;
        }
    }
    
    return cmd.str();
}

/**
 * تنفيذ الرابط / Execute linker
 */
bool LLVMLinker::executeLinker(const std::string& command) {
    if (options_.verbose) {
        std::cout << "تشغيل الرابط / Running linker: " << command << std::endl;
    }
    
#ifdef _WIN32
    // 🔑 (AR) النسخةُ العريضةُ لا الضيّقة: `CreateProcessA` تُعيد تفسيرَ بايتاتِ
    //      UTF-8 بترميزِ النظامِ (ACP)، فمسارٌ عربيٌّ تحت ترميزٍ لاتينيٍّ يجعل
    //      الرابطَ **ينجح** ويكتب المخرَجَ باسمٍ مشوَّه — بلا خطأٍ ولا رمزِ عائد.
    // 🔑 (EN) Wide, not narrow: CreateProcessA reinterprets the UTF-8 bytes via
    //      the system ANSI codepage, so an Arabic path under a Latin codepage
    //      makes the linker SUCCEED while writing to a mangled name — with no
    //      error and no exit code to show for it.
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    // نسخ الأمر لأن CreateProcess يحتاج لمؤشر قابل للتعديل
    // Copy command because CreateProcess needs modifiable pointer
    std::wstring cmd_copy = sad::utf8::to_wstring(command);
    
    if (!CreateProcessW(
        NULL,                           // No module name (use command line)
        &cmd_copy[0],                   // Command line
        NULL,                           // Process handle not inheritable
        NULL,                           // Thread handle not inheritable
        FALSE,                          // Set handle inheritance to FALSE
        0,                              // No creation flags
        NULL,                           // Use parent's environment block
        NULL,                           // Use parent's starting directory
        &si,                            // Pointer to STARTUPINFO structure
        &pi                             // Pointer to PROCESS_INFORMATION structure
    )) {
        DWORD error = GetLastError();
        info_.errors.push_back("فشل تشغيل الرابط (CreateProcess error: " + std::to_string(error) + ") / Failed to start linker");
        return false;
    }
    
    // انتظار اكتمال العملية / Wait for process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    // الحصول على كود الخروج / Get exit code
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    
    // إغلاق المقابض / Close handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    
    if (exitCode != 0) {
        info_.errors.push_back("الرابط خرج بكود / Linker exited with code: " + std::to_string(exitCode));
        return false;
    }
    
    return true;
#else
    int result = sad::utf8::run_command(command);
    
    if (result != 0) {
        info_.errors.push_back("Linker exited with code " + std::to_string(result));
        return false;
    }
    
    return true;
#endif
}

/**
 * استخدام LLD مباشرة / Use LLD directly
 */
bool LLVMLinker::useLLD(const LinkingOptions& options) {
    std::string command = buildLinkerCommandLine(options);
    return executeLinker(command);
}

/**
 * استخدام رابط النظام / Use system linker
 */
bool LLVMLinker::useSystemLinker(const LinkingOptions& options) {
    std::string command = buildLinkerCommandLine(options);
    return executeLinker(command);
}

/**
 * جمع المعلومات من الإخراج / Collect information from output
 */
void LLVMLinker::collectLinkingInfo(const std::string& output) {
    // تحليل إخراج الرابط لجمع المعلومات / Parse linker output to collect info
    // هذه دالة قابلة للتوسيع / This function is extensible
}

/**
 * إضافة المكتبات الافتراضية / Add default libraries
 */
void LLVMLinker::addDefaultLibraries(LinkingOptions& options) {
    auto default_libs = getDefaultLibraries(target_triple_);
    
    for (const auto& lib : default_libs) {
        // تحقق من عدم وجود المكتبة مسبقاً / Check if library doesn't already exist
        if (std::find(options.libraries.begin(), options.libraries.end(), lib) == options.libraries.end()) {
            options.libraries.push_back(lib);
        }
    }
    
    auto default_flags = getDefaultLinkerFlags(target_triple_);
    
    for (const auto& flag : default_flags) {
        // تحقق من عدم وجود العلم مسبقاً / Check if flag doesn't already exist
        if (std::find(options.linker_flags.begin(), options.linker_flags.end(), flag) == options.linker_flags.end()) {
            options.linker_flags.push_back(flag);
        }
    }
}

/**
 * التحقق من الملفات / Verify files
 */
bool LLVMLinker::verifyFiles(const LinkingOptions& options) {
    // التحقق من وجود ملفات الكائنات / Verify object files exist
    for (const auto& obj_file : options.object_files) {
        std::ifstream file(obj_file);
        if (!file.good()) {
            info_.errors.push_back("Object file not found: " + obj_file);
            return false;
        }
    }
    
    // التحقق من تحديد ملف الإخراج / Verify output file is specified
    if (options.output_file.empty()) {
        info_.errors.push_back("Output file not specified");
        return false;
    }
    
    return true;
}

} // namespace sad
