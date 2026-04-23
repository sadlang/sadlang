// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// compiler_driver_lld.cpp - ربط باستخدام LLD المدمج
// Embedded LLD Linker Support
// ═══════════════════════════════════════════════════════════════════════════
//
// الوصف (عربي):
// --------------
// هذا الملف يوفر دعم الربط باستخدام LLD المدمج داخل sadc.
// LLD هو رابط من مشروع LLVM يُضمّن كمكتبة داخل sadc
// لتوفير ربط مباشر بدون الحاجة لأي رابط خارجي.
//
// المحتويات:
//   1. أكواب بديلة لـ WindowsManifest (stubs) — مطلوبة بواسطة lldCOFF
//   2. دالة link_with_embedded_lld() — تستدعي LLD كمكتبة
//
// الملف بالكامل داخل حارس #ifdef HAS_EMBEDDED_LLD — لن يُترجم
// إذا لم يكن LLD متاحاً في بيئة البناء.
//
// Description (English):
// ----------------------
// This file provides embedded LLD linker support for sadc.
// LLD is an LLVM linker embedded as a library inside sadc
// to provide direct linking without any external linker dependency.
//
// Contents:
//   1. WindowsManifest stubs — required by lldCOFF
//   2. link_with_embedded_lld() — calls LLD as a library
//
// Entirely guarded by #ifdef HAS_EMBEDDED_LLD.
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"

#ifdef HAS_EMBEDDED_LLD

#include <lld/Common/Driver.h>
#include <llvm/Support/raw_ostream.h>

// (AR) تسجيل محركات LLD المدعومة — كل محرك يدعم صيغة ملفات مختلفة
// (EN) Register supported LLD drivers — each handles a different binary format
LLD_HAS_DRIVER(coff)  // Windows PE/COFF
LLD_HAS_DRIVER(elf)   // Linux ELF
LLD_HAS_DRIVER(mingw) // MinGW
LLD_HAS_DRIVER(macho) // macOS Mach-O
LLD_HAS_DRIVER(wasm)  // WebAssembly

// ────────────────────────────────────────────────────────────────────────
// (AR) أكواب بديلة لـ WindowsManifest — lldCOFF يحتاج هذه الدوال لدعم
//      دمج ملفات manifest، لكن sadc لا يستخدم هذه الميزة.
//      isAvailable() = false يخبر LLD بتجاوز دمج manifest.
// (EN) WindowsManifest stubs — lldCOFF references these for manifest
//      merging but sadc never uses this feature.
//      isAvailable() = false tells LLD to skip manifest merging.
// ────────────────────────────────────────────────────────────────────────
#ifdef _WIN32
#include <llvm/WindowsManifest/WindowsManifestMerger.h>
#include <llvm/Support/MemoryBuffer.h>

namespace llvm
{
    namespace windows_manifest
    {

        bool isAvailable() { return false; }

        char WindowsManifestError::ID = 0;

        WindowsManifestError::WindowsManifestError(const Twine &Msg)
            : Msg(Msg.str()) {}

        void WindowsManifestError::log(raw_ostream &OS) const
        {
            OS << Msg;
        }

        class WindowsManifestMerger::WindowsManifestMergerImpl
        {
        };

        WindowsManifestMerger::WindowsManifestMerger()
            : Impl(std::make_unique<WindowsManifestMergerImpl>()) {}

        WindowsManifestMerger::~WindowsManifestMerger() = default;

        Error WindowsManifestMerger::merge(MemoryBufferRef)
        {
            return make_error<WindowsManifestError>("manifest merging not available in sadc");
        }

        std::unique_ptr<MemoryBuffer> WindowsManifestMerger::getMergedManifest()
        {
            return nullptr;
        }

    }
} // namespace llvm::windows_manifest
#endif // _WIN32

#include <iostream>
#include <fstream>
#include <filesystem>

namespace sad
{
    namespace driver
    {

        // ════════════════════════════════════════════════════════════════════════
        // (AR) ربط باستخدام LLD المدمج — الطريقة المفضلة عند توفر LLD
        // ════════════════════════════════════════════════════════════════════════
        // هذه الدالة تستدعي LLD كمكتبة لربط ملفات الكائن مباشرة من داخل sadc.
        //
        // على Windows: تستخدم lld-link (COFF mode) مع مكتبات MSVC
        // على Linux:   تستخدم ld.lld (ELF mode) مع CRT startup files
        // على macOS:   تستخدم ld64.lld (Mach-O mode)
        //
        // المعاملات:
        //   obj_path        — مسار ملف الكائن المُترجم (.obj/.o)
        //   runtime_obj_path — مسار ملف كائن المكتبة القياسية (يمكن أن يكون فارغاً)
        //   output_file     — مسار الملف التنفيذي الناتج (.exe أو بدون امتداد)
        // ════════════════════════════════════════════════════════════════════════
        // (EN) Link using embedded LLD — preferred method when LLD is available
        //
        // Calls LLD as a library to link object files directly from within sadc.
        //
        // On Windows: lld-link (COFF mode) with MSVC libraries
        // On Linux:   ld.lld (ELF mode) with CRT startup files
        // On macOS:   ld64.lld (Mach-O mode)
        // ════════════════════════════════════════════════════════════════════════
        bool CompilerDriver::link_with_embedded_lld(const std::string &obj_path,
                                                    const std::string &runtime_obj_path,
                                                    const std::string &output_file)
        {
            std::vector<std::string> arg_storage;
            std::vector<std::string> auto_library_paths;
            std::vector<std::string> auto_libraries;
            std::vector<std::string> windows_runtime_libraries;
            append_bundled_network_libraries(auto_library_paths, auto_libraries);
            append_windows_hosted_runtime_libraries(windows_runtime_libraries, true);

#ifdef _WIN32
            // ── Windows: COFF/PE mode (lld-link) ──
            arg_storage.push_back("lld-link");
            arg_storage.push_back("/OUT:" + output_file);
            arg_storage.push_back(obj_path);

            if (!runtime_obj_path.empty() && std::filesystem::exists(runtime_obj_path))
            {
                arg_storage.push_back(runtime_obj_path);
            }

            for (const auto &lib : windows_runtime_libraries)
            {
                arg_storage.push_back(lib);
            }

            // (AR) مكتبات المستخدم
            // (EN) User-specified libraries
            for (const auto &lib : options_.libraries)
            {
                arg_storage.push_back(lib + ".lib");
            }

            for (const auto &lib : auto_libraries)
            {
                arg_storage.push_back(lib + ".lib");
            }

            // (AR) مسارات المكتبات — MSVC + Windows SDK
            // (EN) Library paths — MSVC + Windows SDK
            auto msvc_paths = find_msvc_lib_paths();
            for (const auto &path : msvc_paths)
            {
                arg_storage.push_back("/LIBPATH:" + path);
            }
            for (const auto &path : auto_library_paths)
            {
                arg_storage.push_back("/LIBPATH:" + path);
            }
            for (const auto &path : options_.library_paths)
            {
                arg_storage.push_back("/LIBPATH:" + path);
            }

            arg_storage.push_back("/NOLOGO");
            arg_storage.push_back("/SUBSYSTEM:CONSOLE");

            if (options_.link_static)
            {
                arg_storage.push_back("/MT");
            }

#else
            // ── Linux/macOS: ELF mode (ld.lld) ──
            arg_storage.push_back("ld.lld");
            arg_storage.push_back("-o");
            arg_storage.push_back(output_file);
            arg_storage.push_back(obj_path);

            if (!runtime_obj_path.empty() && std::filesystem::exists(runtime_obj_path))
            {
                arg_storage.push_back(runtime_obj_path);
            }

            // (AR) ملفات بدء التشغيل CRT
            // (EN) CRT startup files
            for (const char *crt : {"/usr/lib/x86_64-linux-gnu/crt1.o",
                                    "/usr/lib/x86_64-linux-gnu/crti.o",
                                    "/usr/lib/x86_64-linux-gnu/crtn.o"})
            {
                if (std::filesystem::exists(crt))
                {
                    arg_storage.push_back(crt);
                }
            }

            // (AR) مسارات المكتبات
            // (EN) Library paths
            arg_storage.push_back("-L/usr/lib/x86_64-linux-gnu");
            arg_storage.push_back("-L/usr/lib");
            arg_storage.push_back("-lc");
            arg_storage.push_back("-lm");
            arg_storage.push_back("-dynamic-linker");
            arg_storage.push_back("/lib64/ld-linux-x86-64.so.2");

            for (const auto &path : options_.library_paths)
            {
                arg_storage.push_back("-L" + path);
            }
            for (const auto &lib : options_.libraries)
            {
                arg_storage.push_back("-l" + lib);
            }
#endif

            // (AR) بناء مصفوفة المؤشرات لـ LLD
            // (EN) Build pointer array for LLD
            std::vector<const char *> args;
            for (const auto &arg : arg_storage)
            {
                args.push_back(arg.c_str());
            }

            if (options_.verbose)
            {
                std::cerr << "\n  (AR) أمر LLD المدمج / (EN) Embedded LLD command:\n  ";
                for (const auto *a : args)
                {
                    std::cerr << a << " ";
                }
                std::cerr << "\n\n";
            }

            // (AR) استدعاء LLD كمكتبة
            // (EN) Call LLD as a library
            std::string lld_stdout_str;
            std::string lld_stderr_str;
            llvm::raw_string_ostream stdout_os(lld_stdout_str);
            llvm::raw_string_ostream stderr_os(lld_stderr_str);

#ifdef _WIN32
            lld::Result result = lld::lldMain(args, stdout_os, stderr_os,
                                              {{lld::WinLink, &lld::coff::link}});
#elif defined(__APPLE__)
            lld::Result result = lld::lldMain(args, stdout_os, stderr_os,
                                              {{lld::Darwin, &lld::macho::link}});
#else
            lld::Result result = lld::lldMain(args, stdout_os, stderr_os,
                                              {{lld::Gnu, &lld::elf::link}});
#endif

            stdout_os.flush();
            stderr_os.flush();

            if (result.retCode != 0)
            {
                if (options_.verbose)
                {
                    if (!lld_stdout_str.empty())
                        std::cerr << lld_stdout_str;
                    if (!lld_stderr_str.empty())
                        std::cerr << lld_stderr_str;
                }

                // (AR) لم ننجح — لكن لا نُبلغ عن خطأ فادح هنا
                //       سنترك الخطوات اللاحقة (clang/link.exe) تتولى الأمر
                // (EN) Failed — but don't report fatal here, let fallback methods handle it
                if (options_.verbose)
                {
                    std::cerr << "  (AR) LLD المدمج فشل (رمز: " << result.retCode << ")\n";
                    std::cerr << "  (EN) Embedded LLD failed (code: " << result.retCode << ")\n";
                }
                return false;
            }

            if (options_.verbose)
            {
                std::cerr << "  ✅ (AR) تم الربط بنجاح باستخدام LLD المدمج\n";
                std::cerr << "  ✅ (EN) Successfully linked using embedded LLD\n";
            }

            return true;
        }

    } // namespace driver
} // namespace sad

#endif // HAS_EMBEDDED_LLD
