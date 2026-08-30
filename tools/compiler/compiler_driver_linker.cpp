// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_linker.cpp - ربط ملفات الكائن لإنتاج ملفات تنفيذية
// Object File Linking — Producing Executables
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// هذا الملف يوفر دوال ربط ملفات الكائن (.obj/.o) لإنتاج ملفات تنفيذية.
//
// المحتويات:
//   1. invoke_linker()            — استدعاء الرابط العام (system linker)
//   2. get_temp_file()            — إنشاء ملفات مؤقتة
//   3. find_clang()               — البحث عن Clang في النظام
//   4. link_object_to_executable() — الدالة الرئيسية للربط (LLD + Clang + link.exe)
//
// الدوال المنقولة إلى ملفات مخصصة:
//   - link_with_embedded_lld()    → compiler_driver_lld.cpp
//   - link_android_executable()   → compiler_driver_android.cpp
//
// Description (English):
// ----------------------
// This file provides object file linking to produce executables.
//
// Contents:
//   1. invoke_linker()            — general system linker invocation
//   2. get_temp_file()            — temp file creation utility
//   3. find_clang()               — Clang discovery on the system
//   4. link_object_to_executable() — main linking function (LLD + Clang + link.exe)
//
// Functions moved to dedicated files:
//   - link_with_embedded_lld()    → compiler_driver_lld.cpp
//   - link_android_executable()   → compiler_driver_android.cpp
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"
#include "utf8_utils.h"
#include "cli_flags_generated.h"

// Windows API for paths
#ifdef _WIN32
#include <windows.h>
#else
// (AR) <unistd.h> لـ ::getpid على POSIX (المقابل لـ GetCurrentProcessId على Windows).
//      كان مفقودًا فيفشل بناء GCC؛ MSVC يستعمل الفرع الويندوزي فلا يحتاجه.
// (EN) <unistd.h> for ::getpid on POSIX (Windows uses GetCurrentProcessId). It was
//      missing, breaking the GCC build; MSVC uses the Windows branch.
#include <unistd.h>
#endif

// (AR) بيانات runtime المضمّنة — تم توليدها تلقائياً من sad_embedded_runtime.c
// (EN) Embedded runtime data — auto-generated from sad_embedded_runtime.c
#include "sad_embedded_runtime_data.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <cstdio>
#include <atomic>
#include <random>

// Undefine Windows macros that conflict with our code
#ifdef _WIN32
#undef CYAN
#undef YELLOW
#undef RED
#undef GREEN
#undef BLUE
#undef MAGENTA
#undef WHITE
#undef ERROR // Windows defines this
#undef FATAL // Windows might define this too
#endif

// ═══════════════════════════════════════════════════════════════════════════
// (AR) 🔑 **الـDLL المُورَّدةُ تُنسَخُ بجوارِ الثنائيِّ المُنتَج.**
//      برنامجُ واجهةٍ مترجَمٌ على ويندوز يستوردُ `SDL2.dll`، ومُحمِّلُ ويندوز
//      يبحثُ عنها في **مجلّدِ الثنائيِّ** أوّلًا ثمّ في مسارِ النظام. فبرنامجٌ
//      يُنقَلُ إلى أيِّ مجلّدٍ آخرَ **لا يبدأُ العملَ أصلًا**: يخرجُ بالرمز
//      `0xC0000135` (STATUS_DLL_NOT_FOUND) بلا حرفٍ واحدٍ في stdout ولا stderr.
//
//      وقد قِيسَ ذلك على CI: ستّةُ برامجِ واجهةٍ في ويندوز Debug تُترجَمُ
//      وتُربَطُ بنجاحٍ ثمّ تطبعُ لا شيء — والرمزُ `-1073741515` هو هو.
//      وخانةُ Release كانت خضراءَ **بالمصادفةِ لا بالصحّة**: `/O2` يُسقِطُ
//      المرجعَ إلى الراسمِ فيختفي استيرادُ `SDL2.dll` من الثنائيِّ رأسًا،
//      بينما `/Od` يُبقيه. أي أنّ قابليّةَ تشغيلِ البرنامجِ كانت معلَّقةً على
//      حادثةِ تحسين — وهو عطبُ منتَجٍ لا عَرَضُ تكامل: أيُّ مستخدمٍ يترجمُ
//      برنامجَ واجهةٍ وينقلُه يقعُ فيه.
// (EN) Copy vendored DLLs beside the produced binary. A compiled UI program on
//      Windows imports SDL2.dll, and the loader looks in the BINARY'S directory
//      first — so the program fails to start anywhere else, exiting with
//      0xC0000135 (STATUS_DLL_NOT_FOUND) and writing nothing to stdout/stderr.
//      Measured on CI: six UI programs link fine and print nothing, exit code
//      -1073741515. The Release cell was green BY ACCIDENT, not by correctness:
//      /O2 drops the renderer reference so the SDL2.dll import disappears
//      entirely, while /Od keeps it. Runnability hung on an optimization
//      accident — a product defect, not an integration symptom.
// ═══════════════════════════════════════════════════════════════════════════
namespace
{
#ifdef _WIN32
    std::filesystem::path find_dll_under(const std::filesystem::path &base,
                                         const std::string &dll_name)
    {
        std::error_code ec;
        if (base.empty() || !std::filesystem::is_directory(base, ec))
        {
            return {};
        }
        for (std::filesystem::recursive_directory_iterator it(
                 base, std::filesystem::directory_options::skip_permission_denied, ec),
             end;
             it != end; it.increment(ec))
        {
            if (ec)
            {
                break;
            }
            if (it->is_regular_file(ec) && it->path().filename().string() == dll_name)
            {
                return it->path();
            }
        }
        return {};
    }
#endif
} // namespace

namespace sad
{
    namespace driver
    {

        bool CompilerDriver::invoke_linker(const std::vector<std::string> &objects,
                                           const std::string &output)
        {
            // ════════════════════════════════════════════════════════════════════════
            // (AR) دالة ربط الملفات الثنائية — تجمع ملفات الكائن في ملف واحد
            // ════════════════════════════════════════════════════════════════════════
            // هذه الدالة تُنفِّذ مرحلة الربط (Linking) التي تجمع:
            //   - ملفات الكائن (.obj/.o)   : الكود المُترجَم
            //   - مكتبات النظام (.lib/.a)  : وظائف قياسية
            //   - ملفات بدء التشغيل (CRT) : إعداد البيئة قبل main()
            //
            // في وضع Freestanding يتم تخطّي جميع المكتبات القياسية وملفات CRT،
            // لأن النواة/البرنامج المدمج يُعرِّف بيئته الخاصة من الصفر.
            // ════════════════════════════════════════════════════════════════════════
            // (EN) Linker invocation — combines object files into a single binary
            //
            // This function performs the Linking phase combining:
            //   - Object files (.obj/.o)   : compiled code
            //   - System libraries (.lib/.a): standard functions
            //   - C runtime files (CRT)    : environment setup before main()
            //
            // In Freestanding mode, all standard libraries and CRT files are
            // skipped — the kernel/embedded program defines its own environment.
            // ════════════════════════════════════════════════════════════════════════

            // Find system linker
            auto linker_opt = find_system_linker();
            if (!linker_opt)
            {
                diagnostics_.report_fatal("linker not found / الرابط غير موجود");
                return false;
            }

            std::string linker = *linker_opt;

            // Build linker command
            std::string command = linker;

            // Add output file
            command += " -o " + output;

            // Add object files
            for (const auto &obj : objects)
            {
                command += " " + obj;
            }

            // (AR) إضافة علامات وضع Freestanding قبل أي مكتبات
            // ════════════════════════════════════════════════════════════════════════
            // (AR) وضع Freestanding — علامات الرابط الضرورية:
            //
            //   -nostdlib
            //       لا تضمّن مكتبة C القياسية (libc.a / msvcrt.lib)
            //       ولا مكتبة C++ القياسية (libstdc++.a / msvcprt.lib)
            //       هذا يضمن عدم وجود أي كود خارجي غير متوقع في الثنائي
            //
            //   -nostartfiles
            //       لا تضمّن ملفات بدء تشغيل C (crt0.o, crti.o, crtn.o)
            //       هذه الملفات مسؤولة عن استدعاء main() وإعداد مكدّس C
            //       في وضع freestanding، الكود يبدأ مباشرة من رمز _start
            //
            //   -nodefaultlibs
            //       لا تُضف أي مكتبات افتراضية أخرى (libgcc, compiler-rt, ...)
            //       هذا يُعطي تحكماً كاملاً على ما يُرتبط بالثنائي
            //
            //   -e <نقطة_الدخول>
            //       تحديد نقطة البداية الفعلية للتنفيذ
            //       افتراضياً: _start (معيار لينكس)
            //       قابل للتغيير: kernel_main, boot_entry, ...
            //
            //   -T <سكريبت_الرابط>
            //       سكريبت يُحدّد تخطيط الذاكرة (Memory Layout):
            //       - أين تبدأ قسم النص (.text) في الذاكرة
            //       - أين تُوضع البيانات (.data, .bss)
            //       - عنوان التحميل لأنظمة التشغيل والبرامج المدمجة
            // ════════════════════════════════════════════════════════════════════════
            // (EN) Freestanding mode — required linker flags:
            //
            //   -nostdlib     : no C/C++ standard library
            //   -nostartfiles : no C startup files (crt0.o etc.)
            //   -nodefaultlibs: no default libraries (libgcc, compiler-rt)
            //   -e <entry>    : specify actual execution entry point
            //   -T <script>   : linker script for memory layout
            // ════════════════════════════════════════════════════════════════════════
            if (options_.freestanding)
            {
                // (AR) العلامات الثلاثة الأساسية لوضع freestanding
                // (EN) Three essential freestanding flags
                command += " -nostdlib -nostartfiles -nodefaultlibs";

                // (AR) تحديد نقطة الدخول — دائماً في وضع freestanding
                //      حتى _start الافتراضية يجب تحديدها صراحةً
                //      لأن بعض الرابطات قد تبحث عن main() افتراضياً
                // (EN) Always specify entry point in freestanding mode
                //      Because some linkers default to looking for main()
                if (!options_.freestanding_entry.empty())
                {
                    command += " -e " + options_.freestanding_entry;
                    if (options_.verbose)
                    {
                        std::cout << u8"  \u2699 [linker] \u0646\u0642\u0637\u0629 \u0627\u0644\u062f\u062e\u0648\u0644: "
                                  << options_.freestanding_entry << "\n";
                    }
                }

                // (AR) سكريبت الرابط — يُحدَّد هنا (لا يُكرَّر لاحقاً)
                //      يُوفِّر تحكماً كاملاً في تخطيط الذاكرة
                // (EN) Linker script — specified here (not duplicated below)
                if (!options_.linker_script.empty())
                {
                    command += " -T \"" + options_.linker_script + "\"";
                    if (options_.verbose)
                    {
                        std::cout << u8"  \u2699 [linker] \u0633\u0643\u0631\u064a\u0628\u062a \u0627\u0644\u0631\u0627\u0628\u0637: "
                                  << options_.linker_script << "\n";
                    }
                }

                // (AR) تحذير للمطور إذا لم يوجد سكريبت ربط في وضع freestanding
                //      بدون سكريبت، الرابط يستخدم التخطيط الافتراضي للنظام المضيف
                //      وهذا قد لا يكون صحيحاً للنواة أو الأنظمة المدمجة
                // (EN) Warn developer if no linker script in freestanding mode
                if (options_.linker_script.empty() && options_.verbose)
                {
                    std::cout << u8"  \u26a0 [freestanding] \u062a\u062d\u0630\u064a\u0631: \u0644\u0627 \u064a\u0648\u062c\u062f \u0633\u0643\u0631\u064a\u0628\u062a \u0631\u0627\u0628\u0637 (-T).\n"
                              << u8"     \u0633\u064a\u0633\u062a\u062e\u062f\u0645 \u0627\u0644\u0631\u0627\u0628\u0637 \u0627\u0644\u062a\u062e\u0637\u064a\u0637 \u0627\u0644\u0627\u0641\u062a\u0631\u0627\u0636\u064a.\n"
                              << u8"     \u0627\u0633\u062a\u062e\u062f\u0645 "
                              << canonical_flag(
                                     static_cast<std::uint16_t>(::sad::cli::FlagAction::LinkerScript))
                              << u8"=<\u0645\u0633\u0627\u0631> \u0644\u062a\u062d\u062f\u064a\u062f \u062a\u062e\u0637\u064a\u0637 \u0627\u0644\u0630\u0627\u0643\u0631\u0629.\n";
                }
            }
            else
            {
                // (AR) وضع عادي (غير freestanding) — إضافة الخيارات القياسية
                // (EN) Normal (non-freestanding) mode — add standard options

                // Add library paths
                for (const auto &path : options_.library_paths)
                {
                    command += " -L" + path;
                }

                // Add libraries
                for (const auto &lib : options_.libraries)
                {
                    command += " -l" + lib;
                }

                // Add static linking flag
                if (options_.link_static)
                {
                    command += " -static";
                }

                // Add linker script (normal mode)
                if (!options_.linker_script.empty())
                {
                    command += " -T " + options_.linker_script;
                }
            }

            // (AR) في وضع freestanding نضيف أيضاً مسارات/مكتبات المستخدم
            //      (مثلاً مكتبة runtime خاصة بالنواة)
            // (EN) In freestanding mode also add user-specified paths/libs
            //      (e.g., custom kernel runtime library)
            if (options_.freestanding)
            {
                for (const auto &path : options_.library_paths)
                {
                    command += " -L" + path;
                }
                for (const auto &lib : options_.libraries)
                {
                    command += " -l" + lib;
                }
            }

            // Execute linker
            if (options_.verbose)
            {
                std::cout << colors::CYAN << "Running: " << command << "\n"
                          << colors::RESET;
            }

            int result = sad::utf8::run_command(command);

            if (result != 0)
            {
                diagnostics_.report_fatal("linker failed / فشل الربط");
                return false;
            }

            return true;
        }

        std::filesystem::path CompilerDriver::get_temp_file(const std::string &suffix)
        {
            // (AR) إنشاء ملف مؤقت بإسم فريد — آمن للتزامن
            //      نستخدم عداد ذري + معرف العملية + وقت عالي الدقة
            //      لمنع تضارب الأسماء عند تشغيل عدة نسخ من sadc بالتوازي
            // (EN) Create temp file with unique name — concurrency-safe
            //      Uses atomic counter + PID + high-resolution time
            //      to prevent name collisions when running multiple sadc in parallel
            static std::atomic<uint64_t> counter{0};
            auto now = std::chrono::steady_clock::now().time_since_epoch().count();
            auto pid =
#ifdef _WIN32
                static_cast<uint64_t>(GetCurrentProcessId());
#else
                static_cast<uint64_t>(::getpid());
#endif
            uint64_t id = counter.fetch_add(1, std::memory_order_relaxed);
            auto temp_dir = std::filesystem::temp_directory_path();
            auto temp_file = temp_dir / ("sadc_" + std::to_string(pid) + "_" + std::to_string(now) + "_" + std::to_string(id) + suffix);
            return temp_file;
        }

        //
        // Clang ضروري لربط ملفات الكائن وإنتاج ملفات تنفيذية
        // لأنه يعرف تلقائياً أين توجد مكتبات النظام ومكتبات C
        // ============================================================================
        // (EN) Find Clang tool on the system
        // Clang is needed to link object files into executables because it
        // automatically knows where system libraries and C libraries are located.
        // ============================================================================
        std::optional<std::string> CompilerDriver::find_clang()
        {
            // (AR) قائمة المسارات المرشحة للبحث عن clang
            // (EN) Candidate paths to search for clang
            std::vector<std::string> candidates;

#ifdef _WIN32
            // (AR) المسارات المعروفة على ويندوز
            // (EN) Known Windows paths — common LLVM installation locations
            candidates.push_back("C:\\LLVM\\bin\\clang.exe");
            candidates.push_back("C:\\Program Files\\LLVM\\bin\\clang.exe");
            candidates.push_back("C:\\Program Files (x86)\\LLVM\\bin\\clang.exe");
            candidates.push_back("C:\\llvm_dev\\LLVM\\bin\\clang.exe");
            candidates.push_back("C:\\LLVM-18\\bin\\clang.exe");
            candidates.push_back("C:\\LLVM-17\\bin\\clang.exe");
            candidates.push_back("C:\\LLVM-16\\bin\\clang.exe");

            // (AR) البحث في محركات الأقراص الشائعة
            // (EN) Search common drive locations
            for (const char drive : {'D', 'E'})
            {
                std::string base = std::string(1, drive) + ":\\";
                for (const char *name : {"LLVM\\bin\\clang.exe", "llvm_dev\\LLVM\\bin\\clang.exe",
                                         "LLVM-18\\bin\\clang.exe", "Program Files\\LLVM\\bin\\clang.exe"})
                {
                    candidates.push_back(base + name);
                }
            }

            // (AR) البحث بجوار sadc.exe - قد يكون clang مثبتاً في نفس المجلد
            // (EN) Search next to sadc.exe - clang might be installed alongside
            auto exe_dir = get_executable_dir();
            candidates.push_back((exe_dir / "clang.exe").string());
            candidates.push_back((exe_dir.parent_path() / "clang.exe").string());

            // (AR) البحث في متغيرات البيئة المتعددة
            // (EN) Search in multiple environment variables
            for (const char *env_name : {"LLVM_DIR", "LLVM_PATH", "LLVM_ROOT",
                                         "LLVM_HOME", "LLVM_INSTALL_DIR"})
            {
                const char *env_val = std::getenv(env_name);
                if (env_val)
                {
                    candidates.push_back(std::string(env_val) + "\\bin\\clang.exe");
                    // (AR) بعض المتغيرات تشير إلى lib/cmake/llvm — نحتاج الجذر
                    // (EN) Some variables point to lib/cmake/llvm — navigate to root
                    std::filesystem::path env_path(env_val);
                    if (env_path.filename() == "llvm")
                    {
                        candidates.push_back((env_path.parent_path().parent_path().parent_path() / "bin" / "clang.exe").string());
                    }
                }
            }

            // (AR) فحص كل مرشح
            // (EN) Check each candidate
            for (const auto &path : candidates)
            {
                if (std::filesystem::exists(path))
                {
                    if (options_.verbose)
                    {
                        std::cerr << "  تم العثور على clang: " << path << "\n";
                        std::cerr << "  Found clang at: " << path << "\n";
                    }
                    return path;
                }
            }

            // (AR) البحث في PATH باستخدام أمر where (ويندوز)
            // (EN) Search in PATH using 'where' command (Windows)
            if (std::system("where clang.exe > nul 2>&1") == 0)
            {
                // (AR) clang موجود في PATH - استخرج المسار الكامل
                // (EN) clang is in PATH - extract full path
                FILE *pipe = _popen("where clang.exe 2>nul", "r");
                if (pipe)
                {
                    char buffer[512];
                    if (fgets(buffer, sizeof(buffer), pipe))
                    {
                        std::string result(buffer);
                        // (AR) إزالة أحرف السطر الجديد
                        // (EN) Remove trailing newline/whitespace
                        while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' '))
                        {
                            result.pop_back();
                        }
                        _pclose(pipe);
                        if (!result.empty() && std::filesystem::exists(result))
                        {
                            if (options_.verbose)
                            {
                                std::cerr << "  تم العثور على clang في PATH: " << result << "\n";
                            }
                            return result;
                        }
                    }
                    _pclose(pipe);
                }
            }

#else
            // (AR) البحث على لينكس/ماك
            // (EN) Search on Linux/Mac
            candidates.push_back("/usr/bin/clang");
            candidates.push_back("/usr/local/bin/clang");
            candidates.push_back("/usr/lib/llvm-18/bin/clang");
            candidates.push_back("/usr/lib/llvm-17/bin/clang");
            candidates.push_back("/usr/lib/llvm-16/bin/clang");

            auto exe_dir = get_executable_dir();
            candidates.push_back((exe_dir / "clang").string());

            for (const auto &path : candidates)
            {
                if (std::filesystem::exists(path))
                {
                    if (options_.verbose)
                    {
                        std::cerr << "  Found clang at: " << path << "\n";
                    }
                    return path;
                }
            }

            // (AR) البحث في PATH على لينكس/ماك
            // (EN) Search in PATH on Linux/Mac
            if (std::system("which clang > /dev/null 2>&1") == 0)
            {
                FILE *pipe = popen("which clang 2>/dev/null", "r");
                if (pipe)
                {
                    char buffer[512];
                    if (fgets(buffer, sizeof(buffer), pipe))
                    {
                        std::string result(buffer);
                        while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
                        {
                            result.pop_back();
                        }
                        pclose(pipe);
                        if (!result.empty())
                            return result;
                    }
                    pclose(pipe);
                }
            }
#endif

            return std::nullopt;
        }

        // ============================================================================
        // (AR) ربط ملف كائن مع مكتبة وقت التشغيل لإنتاج ملف تنفيذي
        // ============================================================================
        // هذه الدالة تنفذ المرحلة الأخيرة من الترجمة: أخذ ملف الكائن (.obj)
        // وربطه مع المكتبات اللازمة لإنتاج ملف تنفيذي (.exe).
        //
        // الاستراتيجية:
        //   1. نبحث عن clang أولاً (الطريقة المفضلة)
        //   2. إذا لم نجد clang، نستخدم link.exe (MSVC)
        //   3. نبحث عن مكتبة وقت التشغيل (runtime) التي تحتوي على
        //      دوال مثل sad_llvm_input, sad_llvm_input_int
        //   4. إذا لم نجد المكتبة، ننشئ ملف runtime مؤقت ونترجمه مع البرنامج
        //
        // ملاحظة مهمة: دوال الطباعة (اطبع) تستخدم printf مباشرة في LLVM IR
        // لذا لا تحتاج لمكتبة runtime. لكن دوال الإدخال (اقرأ) تحتاج لها.
        // ============================================================================
        // (EN) Link object file with runtime library to produce executable
        //
        // Strategy:
        //   1. Try clang first (preferred - handles libraries automatically)
        //   2. Fall back to link.exe (MSVC) if clang not found
        //   3. Look for runtime library with functions like sad_llvm_input*
        //   4. If no library found, create minimal runtime temp file & compile
        //
        // Note: Print functions use printf directly in LLVM IR, so no runtime
        // needed for basic output. But input functions (اقرأ) need runtime.
        // ============================================================================

        // (AR) نسخُ الـDLL المُورَّدةِ بجوارِ الثنائيِّ المُنتَج — انظرَ التعليقَ
        //      المطوّلَ فوقَ `find_dll_under`. تُستدعى بعدَ كلِّ ربطٍ ناجحٍ في
        //      المسارات الثلاثةِ (LLD المدمج · clang · link.exe): ثلاثُ نسخٍ من
        //      «نجح الربط»، وسدُّ واحدةٍ منها يتركُ الأخرَيَينِ تُنتجانِ ثنائيًّا
        //      لا يعمل.
        // (EN) Copy vendored DLLs next to the produced binary — see the long
        //      comment above find_dll_under. Called from all THREE success paths
        //      (embedded LLD, clang, link.exe): sealing one would leave the other
        //      two producing a binary that cannot start.
        void CompilerDriver::copy_vendored_runtime_dlls(const std::string &output_file) const
        {
#ifdef _WIN32
            // (AR) الشرطُ هو الشرطُ نفسُه الذي ربطَ به: إن لم تُلتقَط مكتبةُ
            //      الرسومِ فلا استيرادَ لـSDL2 ولا داعيَ لنسخِ شيء.
            // (EN) Gate on exactly what the link gated on: no graphics runtime
            //      picked up ⇒ no SDL2 import ⇒ nothing to copy.
            std::vector<std::string> probe_paths;
            std::vector<std::string> probe_libs;
            append_bundled_network_libraries(probe_paths, probe_libs);
            const bool links_ui =
                std::find(probe_libs.begin(), probe_libs.end(), "sad_graphics_runtime") != probe_libs.end();
            if (!links_ui)
            {
                return;
            }

            std::error_code ec;
            // (AR) 🔑 `sad::utf8::make_path` لا `std::filesystem::path` مباشرةً:
            //      بناءُ المسارِ من نصٍّ **ضيّقٍ** على MSVC يُحوِّلُ بترميزِ النظامِ
            //      لا بـUTF‑8، فمسارُ مخرَجٍ عربيٌّ يُطلِقُ توكيدًا في بناءِ Debug
            //      (`0x80000003`) — وNDEBUG يُخفيه فيمرُّ في Release. وهي لغةٌ
            //      أسماءُ مخرَجاتِها عربيّةٌ بطبيعتِها، فالمسارُ الضيّقُ هنا عطبٌ
            //      لا احتياط. قِيسَ: `-o رقم1.exe` يسقط، و`-o a1.exe` يمرّ.
            // (EN) Use sad::utf8::make_path, never a narrow std::filesystem::path:
            //      MSVC converts narrow strings via the system codepage, not UTF-8,
            //      so an Arabic output path trips a Debug assertion (0x80000003)
            //      that NDEBUG hides in Release. Measured: -o رقم1.exe aborts,
            //      -o a1.exe passes.
            const auto out_dir = std::filesystem::absolute(
                                     sad::utf8::make_path(output_file), ec)
                                     .parent_path();
            if (ec || out_dir.empty())
            {
                return;
            }

            const auto compiler_dir = std::filesystem::absolute(get_executable_dir(), ec).lexically_normal();
            std::filesystem::path vendor;
            if (!ec)
            {
                static const std::filesystem::path kVendorRelative =
                    std::filesystem::path("features") / "graphics" / "third_party";
                for (auto dir = compiler_dir; !dir.empty(); dir = dir.parent_path())
                {
                    std::error_code probe_ec;
                    if (std::filesystem::is_directory(dir / kVendorRelative, probe_ec))
                    {
                        vendor = dir / kVendorRelative;
                        break;
                    }
                    if (dir == dir.parent_path())
                    {
                        break;
                    }
                }
            }

            // (AR) SDL2_ttf اختياريّةٌ فعلًا (المُورَّداتُ قد تخلو منها ويُحذَّرُ
            //      عندَ الربط)، وSDL2 ليست اختياريّة — لكن لا يُحمَّرُ هنا: الربطُ
            //      نجحَ فعلًا، وسقوطُ النسخِ يُعلَنُ تحذيرًا لا يُفشِلُ ترجمةً تمّت.
            // (EN) SDL2_ttf really is optional; SDL2 is not — but this never fails
            //      the build: the link already succeeded, so a failed copy warns.
            for (const char *dll_name : {"SDL2.dll", "SDL2_ttf.dll"})
            {
                const auto target = out_dir / dll_name;
                std::error_code exists_ec;
                if (std::filesystem::exists(target, exists_ec))
                {
                    continue;
                }
                auto source = compiler_dir.empty() ? std::filesystem::path{}
                                                   : compiler_dir / dll_name;
                std::error_code src_ec;
                if (source.empty() || !std::filesystem::exists(source, src_ec))
                {
                    source = find_dll_under(vendor, dll_name);
                }
                if (source.empty())
                {
                    continue;
                }
                std::error_code copy_ec;
                std::filesystem::copy_file(source, target,
                                           std::filesystem::copy_options::overwrite_existing, copy_ec);
                if (copy_ec && options_.verbose)
                {
                    std::cerr << "  تحذير: تعذّر نسخ " << dll_name << " بجوار المخرَج ("
                              << copy_ec.message() << ")\n";
                    std::cerr << "  Warning: could not copy " << dll_name
                              << " next to the output (" << copy_ec.message() << ")\n";
                }
                else if (!copy_ec && options_.verbose)
                {
                    std::cerr << "  نُسِخَت " << dll_name << " بجوار المخرَج / copied next to output\n";
                }
            }
#else
            (void)output_file;
#endif
        }

        bool CompilerDriver::link_object_to_executable(const std::string &obj_path,
                                                       const std::string &output_file,
                                                       llvm::Module *module)
        {
            // ================================================================
            // (AR) فحص: هل الهدف هو أندرويد؟
            // (EN) Check: Is the target Android?
            // ================================================================
            if (options_.target.is_android())
            {
                return link_android_executable(obj_path, output_file, module);
            }

#ifdef HAS_EMBEDDED_LLD
            // ================================================================
            // (AR) الخطوة 1: محاولة استخدام LLD المدمج (الطريقة المفضلة)
            // ════════════════════════════════════════════════════════════════
            // LLD مدمج داخل sadc — لا حاجة لأي رابط خارجي.
            // الخطوات:
            //   1. كتابة ملف runtime .c إلى مجلد مؤقت
            //   2. البحث عن مترجم C (clang, cl.exe, gcc) لترجمة runtime
            //   3. ترجمة runtime.c → runtime.obj
            //   4. استدعاء LLD المدمج لربط user.obj + runtime.obj → .exe
            // ════════════════════════════════════════════════════════════════
            // (EN) Step 1: Try embedded LLD linker (preferred method)
            // LLD is embedded inside sadc — no external linker needed.
            // Steps:
            //   1. Write runtime .c to temp directory
            //   2. Find a C compiler (clang, cl.exe, gcc) to compile runtime
            //   3. Compile runtime.c → runtime.obj
            //   4. Call embedded LLD to link user.obj + runtime.obj → .exe
            // ================================================================
            {
                if (options_.verbose)
                {
                    std::cerr << "\n  (AR) محاولة استخدام LLD المدمج...\n";
                    std::cerr << "  (EN) Trying embedded LLD linker...\n\n";
                }

                // (AR) زمنُ التشغيلِ من المخزونِ المفتوحِ بالمحتوى — يُصرَّف مرّةً
                //      لا مرّةً لكلِّ ترجمة (انظر get_cached_runtime_object).
                // (EN) Runtime from the content-keyed cache — compiled once, not once
                //      per compilation (see get_cached_runtime_object).
                std::string runtime_obj_path;
                if (auto c_compiler = find_c_compiler())
                {
                    if (auto cached = get_cached_runtime_object(*c_compiler))
                    {
                        runtime_obj_path = *cached;
                        if (options_.verbose)
                        {
                            std::cerr << "  (AR) كائنُ زمنِ التشغيل جاهز\n";
                            std::cerr << "  (EN) Runtime object ready\n";
                        }
                    }
                }

                // (AR) استدعاء LLD المدمج
                // (EN) Call embedded LLD
                if (link_with_embedded_lld(obj_path, runtime_obj_path, output_file))
                {
                    copy_vendored_runtime_dlls(output_file);
                    return true;
                }

                if (options_.verbose)
                {
                    std::cerr << "  (AR) فشل LLD المدمج، جاري المحاولة بالطرق البديلة...\n";
                    std::cerr << "  (EN) Embedded LLD failed, trying fallback methods...\n";
                }
            }
#endif

            // ================================================================
            // (AR) الخطوة 2: البحث عن أداة الربط (clang أو link.exe) — احتياط
            // (EN) Step 2: Find linker tool (clang or link.exe) — fallback
            // ================================================================
            auto clang_opt = find_clang();

            if (clang_opt)
            {
                // ============================================================
                // (AR) الطريقة المفضلة: استخدام Clang كمحرك للربط
                // ============================================================
                // Clang هو الخيار الأفضل لأنه:
                //   - يعرف أين توجد مكتبات C القياسية (printf, scanf, ...)
                //   - يتعامل مع اختلافات النظام تلقائياً (MSVC vs MinGW)
                //   - يضيف مكتبات بدء التشغيل (CRT startup) تلقائياً
                //   - يدعم إضافة ملفات مصدر مباشرة (للـ runtime)
                // ============================================================
                // (EN) Preferred method: Use Clang as linker driver
                // Clang automatically handles system libraries, CRT startup,
                // and platform differences.
                // ============================================================
                std::string clang = *clang_opt;
                std::vector<std::string> auto_library_paths;
                std::vector<std::string> auto_libraries;
                std::vector<std::string> windows_runtime_libraries;
                append_bundled_network_libraries(auto_library_paths, auto_libraries);
                append_windows_hosted_runtime_libraries(windows_runtime_libraries, true);

                // (AR) بناء أمر الربط الأساسي
                // (EN) Build basic link command
                // (AR) ملاحظة: على ويندوز، system() يستخدم cmd.exe الذي له قواعد
                //       اقتباس معقدة. نغلف الأمر بالكامل بعلامات اقتباس خارجية.
                // (EN) Note: On Windows, system() uses cmd.exe which has complex
                //       quoting rules. We wrap the entire command in outer quotes.
                std::string command = "\"" + clang + "\" \"" + obj_path + "\"";
                command += " -o \"" + output_file + "\"";
                // (AR) تعطيل تحذيرات الدوال المهملة (scanf, fopen, ...)
                // (EN) Suppress deprecation warnings (scanf, fopen, ...)
                command += " -w";
#ifdef _WIN32
                const std::string runtime_flag = get_windows_clang_runtime_flag();
                if (!runtime_flag.empty())
                {
                    command += " " + runtime_flag;
                }
#endif

                // ============================================================
                // (AR) إنشاء ملف runtime مؤقت بلغة C خالصة
                // ============================================================
                // نستخدم ملف C بسيط (ليس C++) لتجنب مشاكل توافق إصدارات
                // Clang مع مكتبات MSVC القياسية (مثل <vector>).
                // هذا الملف يحتوي على الدوال الأساسية فقط:
                //   - دوال الإدخال (اقرأ): sad_llvm_input, sad_llvm_input_int, ...
                //   - دوال الذاكرة البسيطة: sad_llvm_alloc, sad_llvm_free
                // ملاحظة: دوال الطباعة (اطبع) تستخدم printf مباشرة في LLVM IR
                // ============================================================
                // (EN) Create minimal pure-C runtime temp file
                // We use pure C (not C++) to avoid Clang/MSVC STL version issues.
                // ============================================================
                // (AR) 🔑 كائنٌ مخزونٌ لا مصدرٌ يُعاد تصريفه: تمريرُ ملفِّ الـC هنا
                //      كان يجعل clang **يُصرِّفُ ١٧٠ كيلوبايتَ في كلِّ ربط** (~٧٠٠ ms
                //      مقيسة). المخزونُ مفتوحُه محتوى، فالناتجُ هو الناتجُ نفسُه.
                //      وإن أخفق المخزونُ لأيِّ سبب، يُرَدُّ إلى المصدرِ كما كان —
                //      تسريعٌ لا شرطُ صحّة، ولا مسارَ يفقد زمنَ التشغيلِ صامتًا.
                // (EN) Pass a cached object, not source: handing the .c to clang made it
                //      recompile 170 KB on every link (~700 ms measured). Falls back to
                //      the source path if caching fails — a speed-up, never a precondition.
                bool runtime_attached = false;
#ifdef HAS_EMBEDDED_LLD
                if (auto rt_compiler = find_c_compiler())
                {
                    if (auto cached_rt = get_cached_runtime_object(*rt_compiler))
                    {
                        command += " \"" + *cached_rt + "\"";
                        runtime_attached = true;
                        if (options_.verbose)
                        {
                            std::cerr << "  كائنُ زمنِ التشغيل من المخزون: " << *cached_rt << "\n";
                            std::cerr << "  Runtime object from cache: " << *cached_rt << "\n";
                        }
                    }
                }
#endif
                if (!runtime_attached)
                {
                    auto temp_runtime = get_temp_file(".c");
                    temp_files_.push_back(temp_runtime);

                    std::ofstream rt_file(temp_runtime);
                    if (rt_file.is_open())
                    {
                        // (AR) كتابة البيانات المضمّنة من الهيدر المُولّد تلقائياً
                        // (EN) Write embedded data from auto-generated header
                        rt_file.write(sad_embedded_runtime_data, sad_embedded_runtime_size);
                        rt_file.close();
                        command += " \"" + temp_runtime.string() + "\"";

                        if (options_.verbose)
                        {
                            std::cerr << "  استخدام runtime مؤقت: " << temp_runtime.string() << "\n";
                            std::cerr << "  Using embedded runtime: " << temp_runtime.string() << "\n";
                        }
                    }
                }

                for (const auto &path : auto_library_paths)
                {
                    command += " -L\"" + path + "\"";
                }

#ifdef _WIN32
                // (AR) مسارات مكتبات MSVC/SDK. كان هذا الموضع يقرأ `LIB` وحدَه، فإن
                //      شُغّل المترجم من صدَفةٍ عاديّة (لا «موجِّه أوامر المطوّر») بقي
                //      بلا مسارٍ واحد ⇒ فشل ربطِ كلّ برنامجِ واجهة. نستعمل هنا
                //      find_msvc_lib_paths() — وهي التي يستعملها مسارُ lld أصلًا —
                //      فتقرأ `LIB` ثمّ تكتشف المسارات بـvswhere وWindows Kits عند غيابه
                //      ⇒ مصدرُ اكتشافٍ واحدٌ للمسارَين لا نسختان تنحرفان.
                // (EN) MSVC/SDK library paths. This site used to read `LIB` only, so
                //      outside a Developer Command Prompt it produced no -L at all and
                //      every UI link failed. Reuse find_msvc_lib_paths() (already used by
                //      the lld path): `LIB` first, then vswhere + Windows Kits discovery.
                for (const auto &p : find_msvc_lib_paths())
                {
                    command += " -L\"" + p + "\"";
                }
#endif

                // (AR) إضافة مسارات المكتبات المحددة من المستخدم
                // (EN) Add user-specified library paths
                for (const auto &path : options_.library_paths)
                {
                    command += " -L\"" + path + "\"";
                }

                // (AR) إضافة المكتبات المحددة من المستخدم
                // (EN) Add user-specified libraries
                // (AR) (Amelia مراجعة٣) الحلقتان تمرّان بـclang_library_flag مثلَ
                //      حلقةِ CRT أدناه: مستخدمٌ يمرّر `--library sad_http.lib` كان
                //      يحصل على `-lsad_http.lib` ⇒ فشلُ ربطٍ مُربِك.
                // (EN) Route user/auto libs through the same normaliser as the CRT
                //      loop below; `foo.lib` must become `-lfoo`, not `-lfoo.lib`.
                for (const auto &lib : options_.libraries)
                {
                    command += " " + clang_library_flag(lib);
                }

                for (const auto &lib : auto_libraries)
                {
                    command += " " + clang_library_flag(lib);
                }

#ifdef _WIN32
                for (const auto &lib : windows_runtime_libraries)
                {
                    // (AR) بصيغة `-l` لا موضعيًّا — انظر clang_library_flag: الموضعيُّ
                    //      لا يُبحَث عنه في مسارات -L فيفشل دائمًا مهما ضُبِط LIB.
                    command += " " + clang_library_flag(lib);
                }
#else
                // (AR) نظيرُ ويندوزَ على POSIX — libm/libpthread ومكتبةُ C++ القياسيّة.
                //      يُلحَقُ **بعدَ** الأرشيفاتِ المضمَّنةِ لأنّ رابطَ ELF أحاديُّ المرور:
                //      المكتبةُ التي تُشبعُ رموزًا يجبُ أن تلي مَن يطلبُها.
                // (EN) POSIX counterpart of the Windows CRT block. Appended *after* the
                //      bundled archives: the ELF linker is single-pass, so a library that
                //      satisfies symbols must follow the objects that reference them.
                //      ولا نطلبُ مكتبةَ C++ إلّا حين رُبِط أرشيفٌ مضمَّنٌ فعلًا: هي
                //      وحدَها وحداتُ الترجمةِ بـC++ هنا (كودُنا المولَّدُ ومكتبةُ وقتِ
                //      التشغيلِ المؤقّتةُ كلاهما C خالص)، فبرنامجُ «مرحبا» على صورةٍ
                //      فيها clang بلا libstdc++-dev يبقى قابلًا للربطِ كما كان.
                // (EN) Request the C++ runtime only when a bundled archive was actually
                //      linked — those are the only C++ TUs here (our generated code and
                //      the temp runtime are pure C), so a hello-world on an image with
                //      clang but no libstdc++-dev keeps linking as before.
                std::vector<std::string> posix_runtime_libraries;
                append_posix_hosted_runtime_libraries(posix_runtime_libraries,
                                                      !auto_libraries.empty());
                for (const auto &lib : posix_runtime_libraries)
                {
                    command += " " + clang_library_flag(lib);
                }
#endif

                // (AR) إضافة علم الربط الثابت إذا طُلب
                // (EN) Add static linking flag if requested
                if (options_.link_static)
                {
                    command += " -static";
                }

                if (options_.verbose)
                {
                    std::cerr << "\n  أمر الربط / Link command:\n  " << command << "\n\n";
                }

// (AR) تنفيذ أمر الربط
// (EN) Execute link command
// (AR) على ويندوز، cmd.exe يحتاج لتغليف الأمر بالكامل بعلامات اقتباس
//       إضافية عندما يحتوي المسار التنفيذي على علامات اقتباس
// (EN) On Windows, cmd.exe needs the entire command wrapped in quotes
//       when the executable path contains quotes
#ifdef _WIN32
                int result = sad::utf8::run_command("\"" + command + "\"");
#else
                int result = sad::utf8::run_command(command);
#endif

                if (result != 0)
                {
                    diagnostics_.report_fatal(
                        "فشل ربط الملف التنفيذي (رمز الخطأ: " + std::to_string(result) + ")\n"
                                                                                         "Failed to link executable (error code: " +
                        std::to_string(result) + ")\n"
                                                 "تأكد من تثبيت clang وأنه يعمل بشكل صحيح\n"
                                                 "Make sure clang is installed and working correctly");
                    return false;
                }

                copy_vendored_runtime_dlls(output_file);
                return true;
            }
            else
            {
// ============================================================
// (AR) الطريقة البديلة: استخدام link.exe (MSVC) مباشرة
// ============================================================
// إذا لم نجد clang، نحاول استخدام link.exe من Visual Studio
// هذا أعقد قليلاً لأننا نحتاج لتحديد مكتبات C يدوياً
// ============================================================
// (EN) Fallback: Use link.exe (MSVC) directly
// More complex since we need to specify C libraries manually
// ============================================================
#ifdef _WIN32
                // (AR) حاول العثور على link.exe
                // (EN) Try to find link.exe
                if (std::system("where link.exe > nul 2>&1") == 0)
                {
                    std::vector<std::string> auto_library_paths;
                    std::vector<std::string> auto_libraries;
                    std::vector<std::string> windows_runtime_libraries;
                    append_bundled_network_libraries(auto_library_paths, auto_libraries);
                    append_windows_hosted_runtime_libraries(windows_runtime_libraries, true);

                    // (AR) بناء أمر link.exe
                    // (EN) Build link.exe command
                    std::string command = "link.exe /OUT:\"" + output_file + "\"";
                    command += " \"" + obj_path + "\"";

                    for (const auto &lib : windows_runtime_libraries)
                    {
                        command += " " + lib;
                    }

                    for (const auto &path : auto_library_paths)
                    {
                        command += " /LIBPATH:\"" + path + "\"";
                    }

                    // (AR) إضافة مسارات المكتبات
                    for (const auto &path : options_.library_paths)
                    {
                        command += " /LIBPATH:\"" + path + "\"";
                    }

                    for (const auto &lib : options_.libraries)
                    {
                        command += " " + lib + ".lib";
                    }

                    for (const auto &lib : auto_libraries)
                    {
                        command += " " + lib + ".lib";
                    }

                    command += " /NOLOGO /SUBSYSTEM:CONSOLE";

                    if (options_.verbose)
                    {
                        std::cerr << "  أمر الربط / Link command: " << command << "\n";
                    }

                    int result = sad::utf8::run_command(command);
                    if (result != 0)
                    {
                        diagnostics_.report_fatal(
                            "فشل الربط بـ link.exe (رمز الخطأ: " + std::to_string(result) + ")\n"
                                                                                            "Linking with link.exe failed (error code: " +
                            std::to_string(result) + ")");
                        return false;
                    }
                    copy_vendored_runtime_dlls(output_file);
                    return true;
                }
#endif

                // (AR) لم نجد أي أداة ربط
                // (EN) No linker found
                diagnostics_.report_fatal(
                    "لم يتم العثور على أداة ربط (clang أو link.exe)\n"
                    "No linker found (clang or link.exe)\n"
                    "قم بتثبيت LLVM/Clang من https://llvm.org/\n"
                    "Install LLVM/Clang from https://llvm.org/");
                return false;
            }
        }

    } // namespace driver
} // namespace sad
