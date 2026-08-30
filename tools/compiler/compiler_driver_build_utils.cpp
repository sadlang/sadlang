// ============================================================================
// compiler_driver_build_utils.cpp
// (AR) أدوات البناء: تنظيف الملفّات المؤقّتة، sysroot أندرويد، ترجمة C،
//      وكشف/إلحاق مكتبات وقت التشغيل المُورَّدة (الشبكة + الواجهات SadUI).
// (EN) Build utilities: temp file cleanup, Android sysroot, C-to-obj compile,
//      and detection/append of bundled runtime libs (network + SadUI).
// ============================================================================

#include "compiler_driver.h"
#include "utf8_utils.h"
#include "sad_embedded_runtime_data.h" // (AR) بايتاتُ زمنِ التشغيل — مفتاحُ المخزون

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
// (AR) 🔑 ضمٌّ صريحٌ لا يعتمدُ على وسيط. كانت `std::strlen` تصلُ هنا **عرَضًا**
//      عبرَ ترويسةِ LLVM مضمومةٍ بلا استعمالِ `llvm::` واحد؛ فلمّا حُذفت — وهي
//      «ميّتةٌ» بمقياسِ الاستعمالِ المباشر — سقطَ معها مورِّدُها الخفيّ. وMSVC
//      يُمرِّرُها من ترويسةٍ أخرى فلا يُحمِرُّ، وlibstdc++ لا يفعل: عطبٌ يخصُّ
//      المضيفَ وحدَه. عدُّ الاستعمالِ يقيسُ التبعيّةَ **الواجهيّة** لا ما تُورِّدُه
//      الترويسةُ نقلًا؛ والعلاجُ أن يُصرَّحَ بالمورِّدِ لا أن يُورَّثَ.
// (EN) Explicit include, no reliance on a middleman. std::strlen used to arrive
//      here transitively through an LLVM header that had zero direct llvm:: uses;
//      removing that "dead" include removed its hidden supplier too. MSVC still
//      supplies it from another header, libstdc++ does not — a host-only break.
//      Counting uses measures the interface dependency, not what a header
//      transitively supplies.
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <string_view>

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

            // (AR) يصعد آباء «start» بحثًا عن مجلّد الموردات المُورَّدة
            //      (features/graphics/third_party). كان الجذر يُفترض على بُعد
            //      ثلاثة مستوياتٍ بالضبط (build/bin/<Config>/) فينكسر لأيّ تخطيطٍ
            //      آخر — ومنه dist/<Config>/ حيث كان الثنائيّ المُثبَّت يفشل
            //      بـSDL_* غير معرّفة. نبدأ بمجلّد الثنائيّ نفسه كي تكفيَ حزمةٌ
            //      تشحن الموارد بجواره. يُعيد مسارًا فارغًا إن لم يوجد.
            // (EN) Walk «start»'s ancestors for features/graphics/third_party. The
            //      root used to be assumed exactly three levels up, which breaks for
            //      any other layout (dist/<Config>/ among them). Empty if not found.
            std::filesystem::path find_vendored_third_party_dir(const std::filesystem::path &start)
            {
                static const std::filesystem::path kVendorRelative =
                    std::filesystem::path("features") / "graphics" / "third_party";

                for (auto dir = start; !dir.empty(); dir = dir.parent_path())
                {
                    std::error_code ec;
                    if (std::filesystem::is_directory(dir / kVendorRelative, ec))
                    {
                        return dir / kVendorRelative;
                    }
                    if (dir == dir.parent_path()) // (AR) بلغنا جذر القرص
                    {
                        break;
                    }
                }
                return {};
            }

            // (AR) يبحث تنازليًّا تحت «base» عن مجلّد x64 يحوي «<lib>.lib»، ويُعيد مجلّده.
            //      يُستعمل لإيجاد المكتبات المُورَّدة (SDL2/SDL2_ttf) دون تثبيت رقم
            //      الإصدار في المسار (features/graphics/third_party/SDL2/SDL2-<ver>/lib/x64).
            // (EN) Recursively find an x64 dir under «base» containing «<lib>.lib» and
            //      return that dir. Locates vendored libs (SDL2/SDL2_ttf) without
            //      hardcoding the version segment in the path.
            std::filesystem::path find_vendored_x64_lib_dir(const std::filesystem::path &base,
                                                            const std::string &library_name)
            {
                std::error_code ec;
                if (!std::filesystem::exists(base, ec))
                {
                    return {};
                }
                const std::string target = library_name + ".lib";
                // (AR) حدّ عمقٍ احتياطيّ: التخطيط الفعليّ هو
                //      <base>/SDL2/SDL2-<ver>/lib/x64 (عمق ~4)؛ الحدّ يمنع مسحًا
                //      غير محدود لو احتوى الشجرة على ارتباطات/أعشاش عميقة.
                // (EN) Defensive depth cap: real layout is
                //      <base>/SDL2/SDL2-<ver>/lib/x64 (depth ~4); the cap prevents
                //      an unbounded scan if the tree has deep nests/links.
                constexpr int kMaxSearchDepth = 8;
                for (auto it = std::filesystem::recursive_directory_iterator(
                         base, std::filesystem::directory_options::skip_permission_denied, ec);
                     it != std::filesystem::recursive_directory_iterator();
                     it.increment(ec))
                {
                    if (ec)
                    {
                        break;
                    }
                    if (it.depth() >= kMaxSearchDepth)
                    {
                        it.disable_recursion_pending();
                    }
                    if (it->is_regular_file(ec) && it->path().filename() == target)
                    {
                        // (AR) نفحص مجلّد الملفّ (لا المسار الكامل) بحثًا عن «x64» لتمييزه
                        //      عن نظيره x86 (كلاهما موجود: lib/x64 وlib/x86)، وتفاديًا
                        //      لمطابقة كاذبة لو احتوى اسم الملفّ نفسه «x64».
                        // (EN) Inspect the file's directory (not the whole path) for «x64»
                        //      to distinguish it from the sibling x86 dir (both exist:
                        //      lib/x64, lib/x86) and avoid a false match on the filename.
                        const std::string dir = it->path().parent_path().string();
                        if (dir.find("x64") != std::string::npos || dir.find("X64") != std::string::npos)
                        {
                            return it->path().parent_path();
                        }
                    }
                }
                return {};
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
            // (AR) مكتبة وقت تشغيل الواجهات: sad_graphics_runtime (الجسر C) تعتمد على sad_graphics (IR/تخطيط).
            //      بدونها تبقى رموز sad_button/sad_text/sad_column غير معرّفة عند ربط
            //      برامج SadUI المترجمة (إغلاق P0-3/أ-4).
            // (EN) UI runtime: sad_graphics_runtime (C bridge) depends on sad_graphics (IR/layout).
            //      Without them, compiled SadUI programs fail linking with undefined
            //      sad_button/sad_text/sad_column (P0-3/A-4 closure).
            bool found_ui = false;

            // (AR) الوضع الحرّ (--حرّ): نستهدف خلفيّة fb0/evdev لا SDL2. نكشف/نربط
            //      المتغيّر الحرّ (sad_graphics_runtime_freestanding + sad_graphics_freestanding)
            //      ولا نُدخِل SDL2 إطلاقًا، فلا يستورد الثنائيّ SDL2.
            // (EN) Freestanding mode (--حرّ): target fb0/evdev, not SDL2. Detect/link the
            //      freestanding variant and never pull SDL2 in.
            const bool freestanding = options_.freestanding;
            const char *const ui_runtime_lib =
                freestanding ? "sad_graphics_runtime_freestanding" : "sad_graphics_runtime";
            const char *const ui_graphics_lib =
                freestanding ? "sad_graphics_freestanding" : "sad_graphics";

            for (const auto &candidate : candidates)
            {
                const auto normalized = std::filesystem::absolute(candidate).lexically_normal();
                const bool has_http = has_library_file_in_dir(normalized, "sad_http");
                const bool has_network = has_library_file_in_dir(normalized, "sad_network");
                const bool has_websocket = has_library_file_in_dir(normalized, "sad_websocket");
                // (AR) نكشف جسر وقت التشغيل (الحرّ أو المستضاف)؛ مكتبة الرسم مُجمَّعة معه.
                // (EN) Detect the UI runtime bridge (freestanding or hosted); its graphics lib is co-located.
                const bool has_ui = has_library_file_in_dir(normalized, ui_runtime_lib);

                if (!has_http && !has_network && !has_websocket && !has_ui)
                {
                    continue;
                }

                append_unique_value(library_paths, normalized.string());
                found_http = found_http || has_http;
                found_network = found_network || has_network;
                found_websocket = found_websocket || has_websocket;
                found_ui = found_ui || has_ui;
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

            // (AR) مكتبات الواجهات: الترتيب مهمّ — sad_graphics_runtime قبل sad_graphics (تعتمد عليها).
            //      الرابط يُسقِط الأعضاء غير المُشار إليها، فلا ضرر على البرامج غير الرسوميّة.
            // (EN) UI libs: order matters — sad_graphics_runtime before sad_graphics (its dependency).
            //      The linker drops unreferenced members, so non-UI programs are unaffected.
            if (found_ui)
            {
                // (AR) الترتيب مهمّ — جسر وقت التشغيل قبل مكتبة الرسم (يعتمد عليها).
                append_unique_value(libraries, ui_runtime_lib);
                append_unique_value(libraries, ui_graphics_lib);

                // (AR) الوضع الحرّ لا يربط SDL2 إطلاقًا (خلفيّة fb0/evdev ذاتيّة الاكتفاء).
                // (EN) Freestanding never links SDL2 (self-contained fb0/evdev backend).
#ifdef _WIN32
                if (!freestanding)
                {
                // (AR) sad_graphics مُصرَّفة مع SDL2 (SAD_UI_USE_SDL2)، ووحدة ترجمة الجسر
                //      sad_ui_runtime.cpp تشير للراسم فتُدخِل رموز SDL_*/TTF_* عبوريًّا
                //      حتى في المسار بلا رأس. نربط SDL2 وSDL2_ttf المُورَّدتين.
                // (EN) sad_graphics is built with SDL2, and the bridge TU references the
                //      renderer, so SDL_*/TTF_* are pulled transitively even headless.
                //      Link the vendored SDL2 + SDL2_ttf import libs.
                std::error_code exe_dir_ec;
                const auto exe_dir =
                    std::filesystem::absolute(get_executable_dir(), exe_dir_ec).lexically_normal();
                const auto third_party = find_vendored_third_party_dir(exe_dir);

                // (AR) (Amelia مراجعة٣) غيابُ المجلّد يعني فشلَ ربطٍ **يقينيًّا**
                //      بـSDL_* غير معرّفة؛ فالتحذيرُ هنا غيرُ مشروطٍ بـverbose،
                //      ويذكر أين بحثنا كي لا يواجه المستخدمُ رسالةَ رابطٍ خامّة.
                // (EN) A missing dir means a certain link failure, so warn
                //      unconditionally and say where we looked.
                if (exe_dir_ec)
                {
                    std::cerr << "  تحذير: تعذّر تحديد مسار ثنائيّ المترجم ("
                              << exe_dir_ec.message()
                              << ") فلم نبحث عن موردات الرسوميّات؛ قد يفشل الربط بـSDL_*.\n";
                    std::cerr << "  Warning: could not resolve the compiler binary path ("
                              << exe_dir_ec.message() << "); vendored graphics libs not searched.\n";
                }
                else if (third_party.empty())
                {
                    std::cerr << "  تحذير: لم يُعثر على موردات الرسوميّات المُورَّدة "
                                 "(features/graphics/third_party) بدءًا من "
                              << exe_dir.string() << " وصعودًا؛ قد يفشل الربط بـSDL_* غير معرّفة.\n";
                    std::cerr << "  Warning: vendored graphics third_party not found walking up from "
                              << exe_dir.string() << "; link may fail with undefined SDL_*.\n";
                }

                const auto sdl2_dir = find_vendored_x64_lib_dir(third_party, "SDL2");
                if (!sdl2_dir.empty())
                {
                    append_unique_value(library_paths, sdl2_dir.string());
                    append_unique_value(libraries, "SDL2");
                }
                else if (options_.verbose && !third_party.empty())
                {
                    // (AR) لم نجد SDL2.lib المُورَّدة؛ قد يفشل الربط بـSDL_* غير معرّفة.
                    //      (المجلّد المفقود كلّيًّا حُذّر عنه أعلاه بلا شرط.)
                    // (EN) Vendored SDL2.lib not found; link may fail with undefined SDL_*.
                    std::cerr << "  تحذير: لم يُعثر على SDL2.lib (x64) تحت " << third_party.string() << "\n";
                    std::cerr << "  Warning: SDL2.lib (x64) not found under " << third_party.string() << "\n";
                }
                const auto sdl2_ttf_dir = find_vendored_x64_lib_dir(third_party, "SDL2_ttf");
                if (!sdl2_ttf_dir.empty())
                {
                    append_unique_value(library_paths, sdl2_ttf_dir.string());
                    append_unique_value(libraries, "SDL2_ttf");
                }
                else if (options_.verbose)
                {
                    // (AR) لم نجد SDL2_ttf.lib المُورَّدة؛ قد يفشل الربط بـTTF_* غير معرّفة
                    //      إن استعمل البرنامج النصوص المنسّقة.
                    // (EN) Vendored SDL2_ttf.lib not found; link may fail with undefined
                    //      TTF_* if the program uses styled text.
                    std::cerr << "  تحذير: لم يُعثر على SDL2_ttf.lib (x64) تحت " << third_party.string() << "\n";
                    std::cerr << "  Warning: SDL2_ttf.lib (x64) not found under " << third_party.string() << "\n";
                }
                } // if (!freestanding)
#endif

#if !defined(_WIN32) && defined(SAD_UI_HOST_SDL2)
                // (AR) نظيرُ الكتلةِ أعلاه على POSIX. مكتبةُ الرسوماتِ بُنيت بـSDL2
                //      (SAD_UI_HOST_SDL2 يُعرَّفُ وقتَ تهيئةِ CMake حين تُعثَرُ عليها)،
                //      ووحدةُ sad_ui_runtime.cpp تشيرُ إلى DesktopRenderer وDesktopWindow
                //      تحتَ SAD_UI_USE_SDL2 — وهي وحدةُ ترجمةٍ واحدةٌ لا تتجزّأ، فأيُّ
                //      برنامجٍ يستدعي `زر` أو `طباعة_شجرة` يسحبُها كاملةً ومعها رموزُ
                //      SDL_*. بدونَ هذه الكتلةِ يفشلُ الربطُ بـundefined SDL_CreateWindow.
                //      الوضعُ الحرُّ مستثنًى كما على ويندوز: خلفيّةُ fb0/evdev لا SDL2.
                // (EN) POSIX counterpart of the block above. The graphics library was
                //      built against SDL2, and the runtime TU is monolithic, so every UI
                //      program drags SDL_* in. Freestanding is excluded (fb0/evdev).
                if (!freestanding)
                {
                    constexpr const char *kSdl2Library = "SDL2";
                    constexpr const char *kSdl2TtfLibrary = "SDL2_ttf";
#ifdef SAD_UI_HOST_SDL2_LIB_DIR
                    // (AR) مسارٌ ملموسٌ حين لا يكونُ ضمنَ مساراتِ البحثِ الافتراضيّة
                    //      (هومبرو على ماك مثلًا: ‎/opt/homebrew/lib‎).
                    const std::string sdl2_dir = SAD_UI_HOST_SDL2_LIB_DIR;
                    if (!sdl2_dir.empty())
                    {
                        append_unique_value(library_paths, sdl2_dir);
                    }
#endif
                    append_unique_value(libraries, kSdl2Library);
#ifdef SAD_UI_HOST_SDL2_TTF
                    append_unique_value(libraries, kSdl2TtfLibrary);
#else
                    (void)kSdl2TtfLibrary;
#endif
                }
#endif
            }

#ifdef _WIN32
            if (found_http || found_network || found_websocket)
            {
                append_unique_value(libraries, "ws2_32");
            }
#endif
        }

        // ═══════════════════════════════════════════════════════════════════
        // (AR) 🔑 **مكتبةُ التشغيلِ تُشتَقُّ من هذا الثنائيِّ نفسِه، لا تُفترَض.**
        //      كان السائقُ يمرّرُ `-fms-runtime-lib=dll` دائمًا — أي CRT
        //      **الإصداريَّ** — بينما مكتباتُ المشروعِ التي يربطُها في بناءِ
        //      Debug مُصرَّفةٌ بـ`/MDd`. فالنتيجةُ ربطٌ يسقطُ برموزٍ لا وجودَ
        //      لها في CRT الإصداريّ: `__imp__CrtDbgReport` و`__imp__calloc_dbg`.
        //      قِيسَ: بناءُ Debug عاديٌّ على ويندوز ⇒ **كلُّ** برنامجٍ يستوردُ
        //      مكتبةً من مكتباتِ المشروعِ لا يُربَط.
        //
        //      والحقيقةُ المطلوبةُ ليست علمَ بناءٍ جديدًا يُمرَّرُ ويُنسى: هذا
        //      الثنائيُّ **مُصرَّفٌ بالمكتباتِ نفسِها وفي التكوينِ نفسِه**، فـ
        //      `_DEBUG` فيه هو بعينِه جوابُ السؤال. وMSVC يُعرِّفُه حصرًا مع
        //      `/MDd` و`/MTd`. ووضعُ التوافقِ يفرضُ `/U_DEBUG` فيُجيبُ صحيحًا
        //      أيضًا: مكتباتٌ إصداريّةٌ ⇐ عَلَمٌ إصداريّ.
        // (EN) Derive the runtime library from THIS binary, never assume it. The
        //      driver always passed -fms-runtime-lib=dll — the RELEASE CRT —
        //      while the project libraries it links are built /MDd in a Debug
        //      build, so every generated program failed to link with
        //      __imp__CrtDbgReport / __imp__calloc_dbg. The needed fact is not a
        //      new build flag to pass and forget: this binary is compiled with
        //      those very libraries in that very config, so its own _DEBUG is
        //      the answer. MSVC defines it exactly for /MDd and /MTd, and the
        //      release-compat mode forces /U_DEBUG — which answers correctly too.
        // ═══════════════════════════════════════════════════════════════════
        std::string CompilerDriver::get_windows_clang_runtime_flag() const
        {
#ifdef _WIN32
#ifdef _DEBUG
            return options_.link_static ? "-fms-runtime-lib=static_dbg"
                                        : "-fms-runtime-lib=dll_dbg";
#else
            return options_.link_static ? "-fms-runtime-lib=static" : "-fms-runtime-lib=dll";
#endif
#else
            return "";
#endif
        }

        void CompilerDriver::append_windows_hosted_runtime_libraries(std::vector<std::string> &libraries,
                                                                     bool include_cpp_runtime) const
        {
#ifdef _WIN32
            // (AR) أسماءُ CRT تتبع العَلَمَ أعلاه حرفًا بحرف: خلطُ `msvcrt` مع
            //      مكتباتٍ بُنِيَت على `msvcrtd` يُنتج رموزًا مفقودةً أو نسختَي
            //      كومةٍ في عمليّةٍ واحدة. والمصدرُ واحد: `_DEBUG` لهذا الثنائيّ.
            // (EN) The CRT names follow the flag above exactly: mixing msvcrt with
            //      libraries built against msvcrtd yields missing symbols or two
            //      heaps in one process. One source of truth: this binary's _DEBUG.
#ifdef _DEBUG
            if (options_.link_static)
            {
                if (include_cpp_runtime)
                {
                    append_unique_value(libraries, "libcpmtd.lib");
                }
                append_unique_value(libraries, "libcmtd.lib");
                append_unique_value(libraries, "libvcruntimed.lib");
                append_unique_value(libraries, "libucrtd.lib");
            }
            else
            {
                if (include_cpp_runtime)
                {
                    append_unique_value(libraries, "msvcprtd.lib");
                }
                append_unique_value(libraries, "msvcrtd.lib");
                append_unique_value(libraries, "vcruntimed.lib");
                append_unique_value(libraries, "ucrtd.lib");
            }
#else
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
#endif

            append_unique_value(libraries, "oldnames.lib");
            append_unique_value(libraries, "legacy_stdio_definitions.lib");
            append_unique_value(libraries, "kernel32.lib");
#else
            (void)libraries;
            (void)include_cpp_runtime;
#endif
        }

        void CompilerDriver::append_posix_hosted_runtime_libraries(std::vector<std::string> &libraries,
                                                                    bool include_cpp_runtime) const
        {
#ifndef _WIN32
            // (AR) دوالُّ الرياضيّاتِ في libm والخيوطُ في libpthread — وكلاهما خارجَ
            //      libc الافتراضيّةِ بخلافِ CRT ويندوز.
            // (EN) Math lives in libm and threads in libpthread — outside default libc.
            constexpr const char *kMathLibrary = "m";
            constexpr const char *kThreadLibrary = "pthread";

            // (AR) مكتبةُ C++ القياسيّة: المكتباتُ المضمَّنةُ التي نربطُها
            //      (sad_graphics_runtime وsad_graphics وsad_http…) وحداتُ ترجمةٍ
            //      بـC++، فتشيرُ إلى `std::basic_ostringstream` و`std::exception`
            //      وغيرِها. ويندوزُ يربطُها منذُ البداية (msvcprt/libcpmt في
            //      append_windows_hosted_runtime_libraries) بينما بقيَ POSIX بلا
            //      نظير ⇒ كلُّ برنامجٍ يستوردُ «رسومات» يفشلُ ربطُه على لينكس/ماك
            //      بـundefined reference إلى رموزِ `std::`. سائقُ الربطِ هنا هو
            //      `clang`/`cc` (سائقُ C) فلا يضمُّها تلقائيًّا كما يفعلُ `clang++`.
            // (EN) C++ standard library: the bundled archives we link are C++ TUs
            //      referencing std:: symbols. Windows has linked its C++ runtime all
            //      along; POSIX had no counterpart, so every UI program failed to link
            //      there. Our link driver is the C driver (`clang`/`cc`), which does
            //      not pull the C++ runtime in the way `clang++` would.
#ifdef __APPLE__
            // (AR) ماك: libc++ (LLVM) هي القياسيّةُ الافتراضيّة.
            constexpr const char *kCppStandardLibrary = "c++";
#else
            // (AR) لينكس: libstdc++ (GNU) — وهي ما بُنيت به الأرشيفاتُ نفسُها
            //      (رموزُ `std::__cxx11::` في سجلِّ الإخفاقِ تدلُّ عليها).
            constexpr const char *kCppStandardLibrary = "stdc++";
#endif

            // (AR) الترتيبُ مقصود: مكتبةُ C++ **قبلَ** libm/libpthread. رابطُ ELF
            //      أحاديُّ المرورِ على الأرشيفاتِ الساكنة، وأعضاءُ libstdc++ نفسِها
            //      تستدعي pow/sqrt — فلو جاءت بعدَ `-lm` انتهى مسحُ libm.a قبلَ أن
            //      تُطلَبَ رموزُه ⇒ undefined reference to 'pow' تحتَ `--static`.
            //      (Amelia مراجعة: العكسُ يمرُّ ديناميًّا بـDT_NEEDED ويسقطُ ساكنًا.)
            // (EN) Order matters: the C++ runtime precedes libm/libpthread. libstdc++
            //      members themselves call pow/sqrt; with `-lm` first, its archive scan
            //      is over before those references appear ⇒ static links break.
            if (include_cpp_runtime)
            {
                append_unique_value(libraries, kCppStandardLibrary);
            }
            append_unique_value(libraries, kMathLibrary);
            append_unique_value(libraries, kThreadLibrary);
#else
            (void)libraries;
            (void)include_cpp_runtime;
#endif
        }

        std::string CompilerDriver::clang_library_flag(const std::string &library)
        {
            // (AR) سائقُ clang (لا clang-cl) يعامل `msvcrt.lib` **وسيطًا موضعيًّا** أي
            //      ملفَّ دخلٍ يُقاس في مجلّد العمل وحدَه، ولا يبحث عنه في مسارات `-L`
            //      إطلاقًا ⇒ «no such file or directory: 'msvcrt.lib'» مهما ضُبِط LIB.
            //      الصيغةُ `-lmsvcrt` هي وحدَها التي تمرّ على مسارات البحث.
            //      (مسارُ link.exe بخلافه: الوسيطُ الموضعيُّ صحيحٌ له ويُحَلّ بـ/LIBPATH.)
            // (EN) The clang driver treats `foo.lib` as a positional input file (CWD
            //      only) and never searches -L for it; only `-lfoo` honours -L.
            //      (Amelia مراجعة٣) المقارنةُ غيرُ حسّاسةٍ للحالة: ويندوز يقبل
            //      `FOO.LIB` فلولا ذلك لخرج `-lFOO.LIB`.
            constexpr std::string_view kLibSuffix = ".lib";
            std::string stem = library;
            if (stem.size() > kLibSuffix.size())
            {
                std::string tail = stem.substr(stem.size() - kLibSuffix.size());
                std::transform(tail.begin(), tail.end(), tail.begin(),
                               [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                if (tail == kLibSuffix)
                {
                    stem.resize(stem.size() - kLibSuffix.size());
                }
            }
            return "-l" + stem;
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
                if (sad::utf8::run_command(cmd) == 0)
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

            // (AR) SAD_FREESTANDING يُميَّز فقط عند --freestanding: يُبقي
            //      sad_embedded_runtime.c على مسار العشوائيّة القديم (srand/rand
            //      عبر libc فقط، بلا نداء نظام تشغيل) لأهداف الوضع الحرّ التي قد
            //      لا يوجد تحتها نظام تشغيل مضيف أصلًا (كنواة sad-os). الأهداف
            //      المستضافة (الافتراضي) تحصل على CSPRNG حقيقيّ — انظر
            //      sad_crypto_os_random في sad_embedded_runtime.c.
            // (EN) SAD_FREESTANDING is defined only for --freestanding builds, so
            //      sad_embedded_runtime.c keeps the old libc-only rand() path on
            //      that target (may have no host OS at all, e.g. sad-os kernel).
            //      Hosted targets (default) get a real OS-backed CSPRNG.
            const std::string freestanding_define = options_.freestanding ? "SAD_FREESTANDING" : "";

            if (is_msvc)
            {
                command = "\"" + c_compiler + "\" /c /O2 /TC /W0 /nologo";
                command += options_.link_static ? " /MT" : " /MD";
                if (!freestanding_define.empty())
                    command += " /D" + freestanding_define;
                command += " /Fo\"" + obj_output_path + "\"";
                command += " \"" + c_source_path + "\"";
            }
            else
            {
                command = "\"" + c_compiler + "\" -c -O2 -w";
                if (!freestanding_define.empty())
                    command += " -D" + freestanding_define;
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
            int result = sad::utf8::run_command("\"" + command + "\"");
#else
            int result = sad::utf8::run_command(command);
#endif

            return result == 0;
        }

        // ════════════════════════════════════════════════════════════════════════
        // (AR) كائنُ زمنِ التشغيلِ مُصرَّفًا مرّةً ومخزَّنًا — التفصيلُ في الترويسة.
        // (EN) Compile-once, cached runtime object — rationale in the header.
        // ════════════════════════════════════════════════════════════════════════
        std::optional<std::string> CompilerDriver::get_cached_runtime_object(
            const std::string &c_compiler)
        {
            namespace fs = std::filesystem;

            // (AR) بصمةُ FNV-1a على بايتاتِ زمنِ التشغيل + كلِّ ما يُغيّر ناتجَ
            //      التصريف. المفتاحُ محتوًى، فلا إبطالَ يدويّ.
            // (EN) FNV-1a over the runtime bytes plus everything that changes codegen.
            uint64_t hash = 1469598103934665603ULL;
            auto mix = [&hash](const char *data, size_t len) {
                for (size_t i = 0; i < len; ++i)
                {
                    hash ^= static_cast<unsigned char>(data[i]);
                    hash *= 1099511628211ULL;
                }
            };
            mix(sad_embedded_runtime_data, sad_embedded_runtime_size);
            mix(c_compiler.c_str(), c_compiler.size());
            const std::string triple = options_.target.to_string();
            mix(triple.c_str(), triple.size());
            const char flags[2] = {options_.freestanding ? '1' : '0',
                                   options_.link_static ? '1' : '0'};
            mix(flags, sizeof(flags));

#ifdef _WIN32
            const std::string obj_suffix = ".obj";
#else
            const std::string obj_suffix = ".o";
#endif
            std::ostringstream key;
            key << "sad_rt_" << std::hex << hash << obj_suffix;

            std::error_code ec;
            const fs::path cache_dir = fs::temp_directory_path(ec) / "sad_runtime_cache";
            if (ec)
                return std::nullopt;
            fs::create_directories(cache_dir, ec); // (AR) الوجودُ المسبقُ ليس خطأً
            const fs::path cached = cache_dir / key.str();

            // (AR) إصابةٌ: الملفُّ موجودٌ وغيرُ فارغ. والفراغُ يُرفَض عمدًا — كائنٌ
            //      بطولِ صفرٍ يجتاز `exists` ثمّ يُفشِل الربطَ برسالةٍ لا تدلّ عليه.
            // (EN) Hit: present and non-empty. A zero-length object would pass
            //      `exists` and then fail the link with an unrelated-looking error.
            if (fs::exists(cached, ec) && fs::file_size(cached, ec) > 0 && !ec)
            {
                if (options_.verbose)
                {
                    std::cerr << "  (AR) زمنُ التشغيلِ من المخزون: " << cached.string() << "\n";
                    std::cerr << "  (EN) Runtime from cache: " << cached.string() << "\n";
                }
                return cached.string();
            }

            // (AR) إخفاق: صرِّف إلى اسمٍ فريدٍ ثمّ انقُله ذرّيًّا إلى اسمِ المفتاح.
            // (EN) Miss: compile to a unique name, then move it into place atomically.
            auto temp_c = get_temp_file(".c");
            temp_files_.push_back(temp_c);
            {
                std::ofstream rt(temp_c, std::ios::binary);
                if (!rt.is_open())
                    return std::nullopt;
                rt.write(sad_embedded_runtime_data, sad_embedded_runtime_size);
            }

            auto temp_obj = get_temp_file(obj_suffix);
            if (!compile_c_to_obj(temp_c.string(), temp_obj.string(), c_compiler))
            {
                temp_files_.push_back(temp_obj);
                return std::nullopt;
            }

            fs::rename(temp_obj, cached, ec);
            if (!ec)
                return cached.string();

            // (AR) خسِرنا السباقَ أو تعذّر النقل. إن وضعه غيرُنا فهو صالحٌ (المفتاحُ
            //      محتوًى فالناتجان متطابقان)؛ وإلّا فاستعمل المؤقّتَ ولا تُخفِق —
            //      المخزونُ تسريعٌ لا شرطُ صحّة.
            // (EN) Lost the race or rename failed: use whoever won (same content key),
            //      else fall back to the temp object. The cache is a speed-up, not a
            //      correctness precondition.
            if (fs::exists(cached, ec) && fs::file_size(cached, ec) > 0 && !ec)
            {
                fs::remove(temp_obj, ec);
                return cached.string();
            }
            temp_files_.push_back(temp_obj);
            return temp_obj.string();
        }

#endif // HAS_EMBEDDED_LLD

        // ────────────────────────────────────────────────────────────────────────
        // (AR) البحث عن مسارات مكتبات MSVC و Windows SDK
        // 🔑 خارجَ حارسِ LLD: ينادي هذه الدالّةَ مسارُ الربطِ بمترجمِ C تحتَ
        //    `_WIN32` وحدَه، وبقاؤها داخلَ الحارسِ كان يمنع ترجمةَ `sad-build`
        //    على ويندوزَ كلَّها عند إطفائه (C3861 في compiler_driver_linker.cpp).
        // (EN) Find MSVC and Windows SDK library paths for linking.
        // 🔑 Outside the LLD guard: the C-compiler link path calls it under
        //    _WIN32 alone, and keeping it inside broke the whole Windows build.
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

            FILE *pipe = sad::utf8::open_pipe_read(vswhere + " 2>nul");
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

    } // namespace driver
} // namespace sad
