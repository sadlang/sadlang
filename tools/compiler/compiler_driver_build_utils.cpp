// ============================================================================
// compiler_driver_build_utils.cpp
// (AR) Build utilities: cleanup, sysroot, C compilation
// (EN) Build utilities: temp file cleanup, Android sysroot, C-to-obj compile
// ============================================================================

#include "compiler_driver.h"
#include "../../shared/utils/include/utf8_utils.h"
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#ifdef _WIN32
#include <windows.h>
#undef ERROR
#undef FATAL
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <algorithm>

namespace sad
{
    namespace driver
    {

        namespace
        {
            void append_unique_value(std::vector<std::string> &values, const std::string &value)
            {
                if (!value.empty() &&
                    std::find(values.begin(), values.end(), value) == values.end())
                {
                    values.push_back(value);
                }
            }

            bool has_library_file_in_dir(const std::filesystem::path &directory,
                                         const std::string &library_name)
            {
                if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
                {
                    return false;
                }

                return std::filesystem::exists(directory / (library_name + ".lib")) ||
                       std::filesystem::exists(directory / (library_name + ".a")) ||
                       std::filesystem::exists(directory / ("lib" + library_name + ".a"));
            }
        }

        void CompilerDriver::cleanup_temp_files()
        {
            for (const auto &file : temp_files_)
            {
                try
                {
                    std::filesystem::remove(file);
                }
                catch (...)
                {
                    // Ignore errors during cleanup
                }
            }
            temp_files_.clear();
        }

        void CompilerDriver::append_bundled_network_libraries(std::vector<std::string> &library_paths,
                                                              std::vector<std::string> &libraries) const
        {
            const auto exe_dir = get_executable_dir();
            const std::vector<std::filesystem::path> candidates = {
                exe_dir,
                exe_dir.parent_path(),
                exe_dir / ".." / "lib",
                exe_dir / ".." / ".." / "lib",
                exe_dir / ".." / ".." / "lib" / "Release",
                exe_dir / ".." / ".." / "lib" / "Debug",
                exe_dir / ".." / ".." / "lib" / "RelWithDebInfo",
                exe_dir / ".." / ".." / "lib" / "MinSizeRel"};

            bool found_http = false;
            bool found_network = false;
            bool found_websocket = false;

            for (const auto &candidate : candidates)
            {
                const auto normalized = std::filesystem::absolute(candidate).lexically_normal();
                const bool has_http = has_library_file_in_dir(normalized, "sad_http");
                const bool has_network = has_library_file_in_dir(normalized, "sad_network");
                const bool has_websocket = has_library_file_in_dir(normalized, "sad_websocket");

                if (!has_http && !has_network && !has_websocket)
                {
                    continue;
                }

                append_unique_value(library_paths, normalized.string());
                found_http = found_http || has_http;
                found_network = found_network || has_network;
                found_websocket = found_websocket || has_websocket;
            }

            if (found_http)
            {
                append_unique_value(libraries, "sad_http");
            }
            if (found_network)
            {
                append_unique_value(libraries, "sad_network");
            }
            // (AR) إضافة مكتبة WebSocket عند توفّرها — تحوي sad_ws_client_*, sad_ws_server_*
            // (EN) Add WebSocket library when available — contains sad_ws_client_*, sad_ws_server_*
            if (found_websocket)
            {
                append_unique_value(libraries, "sad_websocket");
            }

#ifdef _WIN32
            if (found_http || found_network || found_websocket)
            {
                append_unique_value(libraries, "ws2_32");
            }
#endif
        }

        std::string CompilerDriver::get_windows_clang_runtime_flag() const
        {
#ifdef _WIN32
            return options_.link_static ? "-fms-runtime-lib=static" : "-fms-runtime-lib=dll";
#else
            return "";
#endif
        }

        void CompilerDriver::append_windows_hosted_runtime_libraries(std::vector<std::string> &libraries,
                                                                     bool include_cpp_runtime) const
        {
#ifdef _WIN32
            if (options_.link_static)
            {
                if (include_cpp_runtime)
                {
                    append_unique_value(libraries, "libcpmt.lib");
                }
                append_unique_value(libraries, "libcmt.lib");
                append_unique_value(libraries, "libvcruntime.lib");
                append_unique_value(libraries, "libucrt.lib");
            }
            else
            {
                if (include_cpp_runtime)
                {
                    append_unique_value(libraries, "msvcprt.lib");
                }
                append_unique_value(libraries, "msvcrt.lib");
                append_unique_value(libraries, "vcruntime.lib");
                append_unique_value(libraries, "ucrt.lib");
            }

            append_unique_value(libraries, "oldnames.lib");
            append_unique_value(libraries, "legacy_stdio_definitions.lib");
            append_unique_value(libraries, "kernel32.lib");
#else
            (void)libraries;
            (void)include_cpp_runtime;
#endif
        }

        // ============================================================================
        // (AR) البحث عن أداة Clang في النظام
        // ============================================================================
        // نبحث عن Clang في عدة أماكن:
        //   1. المسارات المعروفة (C:\LLVM\bin على ويندوز)
        //   2. متغير البيئة PATH
        //   3. بجوار الملف التنفيذي sadc.exe
        //   4. مسارات Visual Studio المعروفة
        // ============================================================================
        // (AR) البحث عن أداة Clang الخاصة بـ Android NDK
        // ============================================================================
        // تبحث عن clang داخل Android NDK لاستخدامه في الترجمة المتقاطعة.
        // تبحث في:
        //   1. متغيرات البيئة: ANDROID_NDK_HOME, ANDROID_NDK, ANDROID_SDK_ROOT
        //   2. المسارات الشائعة على كل نظام تشغيل
        //   3. تتحقق من وجود clang في مسار toolchains/llvm/prebuilt
        // ============================================================================
        // (EN) Find Android NDK Clang for cross-compilation
        // Searches environment variables and common paths for NDK clang.
        // ============================================================================
        std::optional<std::string> CompilerDriver::find_android_ndk_clang()
        {
            namespace fs = std::filesystem;

// (AR) تحديد وسم المضيف (host tag) حسب نظام التشغيل الحالي
// (EN) Determine host tag based on current OS
#ifdef _WIN32
            const std::string host_tag = "windows-x86_64";
            const std::string clang_name = "clang.exe";
#elif defined(__APPLE__)
            const std::string host_tag = "darwin-x86_64";
            const std::string clang_name = "clang";
#else
            const std::string host_tag = "linux-x86_64";
            const std::string clang_name = "clang";
#endif

            // (AR) جمع مسارات NDK المحتملة
            // (EN) Collect candidate NDK paths
            std::vector<std::string> ndk_candidates;

            // (AR) 1. متغيرات البيئة
            const char *env_vars[] = {"ANDROID_NDK_HOME", "ANDROID_NDK", "NDK_HOME"};
            for (const auto &var : env_vars)
            {
                const char *val = std::getenv(var);
                if (val && std::strlen(val) > 0)
                {
                    ndk_candidates.push_back(val);
                }
            }

            // (AR) 2. عبر ANDROID_SDK_ROOT أو ANDROID_HOME
            const char *sdk_vars[] = {"ANDROID_SDK_ROOT", "ANDROID_HOME"};
            for (const auto &var : sdk_vars)
            {
                const char *val = std::getenv(var);
                if (val && std::strlen(val) > 0)
                {
                    std::string sdk_path = val;
                    // (AR) ابحث عن أحدث إصدار NDK
                    fs::path ndk_dir = fs::path(sdk_path) / "ndk";
                    if (fs::exists(ndk_dir) && fs::is_directory(ndk_dir))
                    {
                        std::string latest_version;
                        for (const auto &entry : fs::directory_iterator(ndk_dir))
                        {
                            if (entry.is_directory())
                            {
                                std::string name = entry.path().filename().string();
                                if (name > latest_version)
                                {
                                    latest_version = name;
                                }
                            }
                        }
                        if (!latest_version.empty())
                        {
                            ndk_candidates.push_back((ndk_dir / latest_version).string());
                        }
                    }
                }
            }

// (AR) 3. المسارات الشائعة
#ifdef _WIN32
            const char *localappdata = std::getenv("LOCALAPPDATA");
            if (localappdata)
            {
                fs::path sdk_ndk = fs::path(localappdata) / "Android" / "Sdk" / "ndk";
                if (fs::exists(sdk_ndk) && fs::is_directory(sdk_ndk))
                {
                    std::string latest;
                    for (const auto &entry : fs::directory_iterator(sdk_ndk))
                    {
                        if (entry.is_directory())
                        {
                            std::string name = entry.path().filename().string();
                            if (name > latest)
                                latest = name;
                        }
                    }
                    if (!latest.empty())
                    {
                        ndk_candidates.push_back((sdk_ndk / latest).string());
                    }
                }
            }
            ndk_candidates.push_back("C:\\Android\\ndk");
            ndk_candidates.push_back("C:\\android-ndk");
#else
            ndk_candidates.push_back("/opt/android-ndk");
            const char *home = std::getenv("HOME");
            if (home)
            {
                ndk_candidates.push_back(std::string(home) + "/Android/Sdk/ndk");
                ndk_candidates.push_back(std::string(home) + "/android-ndk");
            }
#endif

            // (AR) البحث عن clang في كل مسار NDK
            // (EN) Search for clang in each NDK candidate
            for (const auto &ndk_path : ndk_candidates)
            {
                fs::path toolchain = fs::path(ndk_path) / "toolchains" / "llvm" / "prebuilt" / host_tag / "bin";
                fs::path clang_path = toolchain / clang_name;

                if (fs::exists(clang_path))
                {
                    if (options_.verbose)
                    {
                        std::cerr << "  تم العثور على Android NDK clang: " << clang_path.string() << "\n";
                        std::cerr << "  Found Android NDK clang at: " << clang_path.string() << "\n";
                    }
                    return clang_path.string();
                }
            }

            if (options_.verbose)
            {
                std::cerr << "  تحذير: لم يتم العثور على Android NDK clang\n";
                std::cerr << "  Warning: Android NDK clang not found\n";
                std::cerr << "  عيّن ANDROID_NDK_HOME أو ثبّت Android NDK\n";
            }
            return std::nullopt;
        }

        // ============================================================================
        // (AR) الحصول على مسار sysroot لـ Android NDK
        // ============================================================================
        // (EN) Get Android NDK sysroot path from clang path
        // ============================================================================

        std::string CompilerDriver::get_android_sysroot(const std::string &ndk_clang)
        {
            namespace fs = std::filesystem;
            // clang يكون في: <ndk>/toolchains/llvm/prebuilt/<host>/bin/clang
            // sysroot يكون في: <ndk>/toolchains/llvm/prebuilt/<host>/sysroot
            fs::path clang_dir = fs::path(ndk_clang).parent_path(); // bin/
            fs::path prebuilt_dir = clang_dir.parent_path();        // <host>/
            fs::path sysroot = prebuilt_dir / "sysroot";
            return sysroot.string();
        }

// ============================================================================
// (AR) تنفيذ الرابط المدمج LLD
// ============================================================================
// (EN) Embedded LLD Linker Implementation
// ============================================================================
#ifdef HAS_EMBEDDED_LLD

        // ────────────────────────────────────────────────────────────────────────
        // (AR) البحث عن مترجم C في النظام
        // (EN) Find a C compiler on the system (clang, cl.exe, or gcc)
        // ────────────────────────────────────────────────────────────────────────
        std::optional<std::string> CompilerDriver::find_c_compiler()
        {
            // (AR) جرب clang أولاً — لديه أفضل توافق مع كود sadc
            // (EN) Try clang first — best compatibility with sadc-generated code
            auto clang = find_clang();
            if (clang)
                return clang;

#ifdef _WIN32
            // (AR) جرب cl.exe عبر أمر where
            // (EN) Try cl.exe via where command
            if (std::system("where cl.exe > nul 2>&1") == 0)
            {
                FILE *pipe = _popen("where cl.exe 2>nul", "r");
                if (pipe)
                {
                    char buffer[512];
                    if (fgets(buffer, sizeof(buffer), pipe))
                    {
                        std::string result(buffer);
                        while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
                            result.pop_back();
                        _pclose(pipe);
                        if (!result.empty() && std::filesystem::exists(result))
                        {
                            if (options_.verbose)
                            {
                                std::cerr << "  Found C compiler (cl.exe): " << result << "\n";
                            }
                            return result;
                        }
                    }
                    else
                    {
                        _pclose(pipe);
                    }
                }
            }

            // (AR) جرب gcc من MinGW/w64devkit
            // (EN) Try gcc from MinGW/w64devkit
            for (const char *gcc_path : {
                     "C:\\w64devkit\\bin\\gcc.exe",
                     "C:\\mingw64\\bin\\gcc.exe",
                     "C:\\msys64\\mingw64\\bin\\gcc.exe",
                     "C:\\msys64\\ucrt64\\bin\\gcc.exe"})
            {
                if (std::filesystem::exists(gcc_path))
                {
                    if (options_.verbose)
                    {
                        std::cerr << "  Found C compiler (gcc): " << gcc_path << "\n";
                    }
                    return std::string(gcc_path);
                }
            }

            if (std::system("where gcc.exe > nul 2>&1") == 0)
            {
                return std::string("gcc.exe");
            }

#else
            // (AR) لينكس/ماك — cc أو gcc متوفران دائماً تقريباً
            // (EN) Linux/macOS — cc or gcc are almost always available
            for (const char *comp : {"cc", "gcc", "clang"})
            {
                std::string cmd = std::string("which ") + comp + " > /dev/null 2>&1";
                if (std::system(cmd.c_str()) == 0)
                {
                    if (options_.verbose)
                    {
                        std::cerr << "  Found C compiler: " << comp << "\n";
                    }
                    return std::string(comp);
                }
            }
#endif

            return std::nullopt;
        }

        // ────────────────────────────────────────────────────────────────────────
        // (AR) ترجمة ملف C إلى ملف كائن
        // (EN) Compile a C source file to an object file
        // ────────────────────────────────────────────────────────────────────────

        bool CompilerDriver::compile_c_to_obj(const std::string &c_source_path,
                                              const std::string &obj_output_path,
                                              const std::string &c_compiler)
        {
            std::string command;

            // (AR) كشف نوع المترجم — cl.exe يستخدم بنية أوامر مختلفة
            // (EN) Detect compiler type — cl.exe uses different command syntax
            bool is_msvc = (c_compiler.find("cl.exe") != std::string::npos ||
                            c_compiler.find("cl.EXE") != std::string::npos);
            bool is_clang = (c_compiler.find("clang") != std::string::npos ||
                             c_compiler.find("CLANG") != std::string::npos);

            if (is_msvc)
            {
                command = "\"" + c_compiler + "\" /c /O2 /TC /W0 /nologo";
                command += options_.link_static ? " /MT" : " /MD";
                command += " /Fo\"" + obj_output_path + "\"";
                command += " \"" + c_source_path + "\"";
            }
            else
            {
                command = "\"" + c_compiler + "\" -c -O2 -w";
#ifdef _WIN32
                if (is_clang)
                {
                    const std::string runtime_flag = get_windows_clang_runtime_flag();
                    if (!runtime_flag.empty())
                    {
                        command += " " + runtime_flag;
                    }
                }
#endif
                command += " -o \"" + obj_output_path + "\"";
                command += " \"" + c_source_path + "\"";
            }

            if (options_.verbose)
            {
                std::cerr << "  (AR) أمر ترجمة runtime: " << command << "\n";
                std::cerr << "  (EN) Runtime compile command: " << command << "\n";
            }

#ifdef _WIN32
            int result = std::system(("\"" + command + "\"").c_str());
#else
            int result = std::system(command.c_str());
#endif

            return result == 0;
        }

        // ────────────────────────────────────────────────────────────────────────
        // (AR) البحث عن مسارات مكتبات MSVC و Windows SDK
        // (EN) Find MSVC and Windows SDK library paths for linking
        // ────────────────────────────────────────────────────────────────────────
        std::vector<std::string> CompilerDriver::find_msvc_lib_paths()
        {
            std::vector<std::string> paths;

#ifdef _WIN32
            // (AR) الطريقة 1: متغير البيئة LIB (يُعيَّن في Developer Command Prompt)
            // (EN) Method 1: LIB environment variable (set in Developer Command Prompt)
            const char *lib_env = std::getenv("LIB");
            if (lib_env)
            {
                std::istringstream iss(lib_env);
                std::string path;
                while (std::getline(iss, path, ';'))
                {
                    if (!path.empty() && std::filesystem::exists(path))
                    {
                        paths.push_back(path);
                    }
                }
                if (!paths.empty())
                    return paths;
            }

            // (AR) الطريقة 2: استخدام vswhere للعثور على Visual Studio
            // (EN) Method 2: Use vswhere to find Visual Studio installation
            std::string vswhere = "\"C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe\"";
            vswhere += " -latest -property installationPath";

            FILE *pipe = _popen((vswhere + " 2>nul").c_str(), "r");
            if (pipe)
            {
                char buffer[512];
                std::string vs_path;
                if (fgets(buffer, sizeof(buffer), pipe))
                {
                    vs_path = buffer;
                    while (!vs_path.empty() && (vs_path.back() == '\n' || vs_path.back() == '\r'))
                        vs_path.pop_back();
                }
                _pclose(pipe);

                if (!vs_path.empty())
                {
                    // (AR) البحث عن مكتبات MSVC
                    // (EN) Find MSVC libraries
                    std::string msvc_base = vs_path + "\\VC\\Tools\\MSVC";
                    if (std::filesystem::exists(msvc_base))
                    {
                        // (AR) العثور على أحدث إصدار
                        // (EN) Find latest version
                        std::string latest_version;
                        for (auto &entry : std::filesystem::directory_iterator(msvc_base))
                        {
                            if (entry.is_directory())
                            {
                                std::string ver = entry.path().filename().string();
                                if (latest_version.empty() || ver > latest_version)
                                {
                                    latest_version = ver;
                                }
                            }
                        }
                        if (!latest_version.empty())
                        {
                            std::string lib_path = msvc_base + "\\" + latest_version + "\\lib\\x64";
                            if (std::filesystem::exists(lib_path))
                            {
                                paths.push_back(lib_path);
                            }
                        }
                    }
                }
            }

            // (AR) الطريقة 3: البحث عن Windows SDK
            // (EN) Method 3: Find Windows SDK
            std::string sdk_base = "C:\\Program Files (x86)\\Windows Kits\\10\\Lib";
            if (std::filesystem::exists(sdk_base))
            {
                std::string latest_version;
                for (auto &entry : std::filesystem::directory_iterator(sdk_base))
                {
                    if (entry.is_directory())
                    {
                        std::string ver = entry.path().filename().string();
                        if (ver.find("10.0.") == 0)
                        {
                            if (latest_version.empty() || ver > latest_version)
                            {
                                latest_version = ver;
                            }
                        }
                    }
                }
                if (!latest_version.empty())
                {
                    std::string ucrt_path = sdk_base + "\\" + latest_version + "\\ucrt\\x64";
                    std::string um_path = sdk_base + "\\" + latest_version + "\\um\\x64";

                    if (std::filesystem::exists(ucrt_path))
                        paths.push_back(ucrt_path);
                    if (std::filesystem::exists(um_path))
                        paths.push_back(um_path);
                }
            }
#endif

            return paths;
        }

        // ────────────────────────────────────────────────────────────────────────
        // (AR) ربط ملف كائن باستخدام LLD المدمج
        // ════════════════════════════════════════════════════════════════════════
        // يستخدم LLD كمكتبة (بدلاً من عملية خارجية):
        //   - ويندوز: lld::coff::link() — ينتج ملفات PE/COFF
        //   - لينكس:  lld::elf::link()  — ينتج ملفات ELF
        //   - ماك:    lld::macho::link() — ينتج ملفات Mach-O
        // ════════════════════════════════════════════════════════════════════════
        // (EN) Link object file using embedded LLD
        // Uses LLD as a library (instead of external process):
        //   - Windows: lld::coff::link() — produces PE/COFF executables
        //   - Linux:   lld::elf::link()  — produces ELF executables
        //   - macOS:   lld::macho::link() — produces Mach-O executables
        // ────────────────────────────────────────────────────────────────────────

#endif // HAS_EMBEDDED_LLD

    } // namespace driver
} // namespace sad
