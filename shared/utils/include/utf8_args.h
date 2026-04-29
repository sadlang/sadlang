// ============================================================================
// (AR) أداة موحَّدة لمعالجة argv بـUTF-8 + ضبط الكونسول
//      مغلِّف رفيع فوق sad::utf8::get_utf8_args() (المرجع الواحد) مع إضافة:
//        - Sad::Utils::Utf8ArgvHolder: يعيد ربط argc/argv في مكانهما إلى UTF-8
//        - Sad::Utils::enableUtf8Console(): يضبط CP الكونسول
//        - مستعارات توافقية: Sad::HubUtil و Sad::Utils::getUtf8Args(argc,argv)
//
// (EN) Unified UTF-8 argv helper + console setup.
//      Thin wrapper over sad::utf8::get_utf8_args() (single source of truth) plus:
//        - Sad::Utils::Utf8ArgvHolder: rebinds argc/argv in-place to UTF-8
//        - Sad::Utils::enableUtf8Console(): sets console code page
//        - Backward-compat aliases: Sad::HubUtil + (argc,argv)-style getUtf8Args
//
// الاستخدام / Usage:
//   #include "utf8_args.h"
//   int main(int argc, char** argv) {
//       Sad::Utils::enableUtf8Console();
//       Sad::Utils::Utf8ArgvHolder _holder(argc, argv);
//       // الآن argv يحتوي UTF-8 على ويندوز — استمر بمنطقك العادي
//   }
// ============================================================================

#pragma once

#include "utf8_utils.h"

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Sad
{
    namespace Utils
    {

        // ----------------------------------------------------------------------------
        // (AR) جمع argv الكامل بـUTF-8 (مع سقوط احتياطي إلى argv الممرَّر)
        // (EN) Collect full argv as UTF-8 (with fallback to passed argv)
        // ----------------------------------------------------------------------------
        inline std::vector<std::string> getUtf8Args(int argcFallback, char **argvFallback)
        {
#ifdef _WIN32
            auto args = sad::utf8::get_utf8_args();
            if (!args.empty())
                return args;
#endif
            std::vector<std::string> out;
            out.reserve(static_cast<size_t>(argcFallback));
            for (int i = 0; i < argcFallback; ++i)
            {
                out.emplace_back(argvFallback && argvFallback[i] ? argvFallback[i] : "");
            }
            return out;
        }

        // ----------------------------------------------------------------------------
        // (AR) ضبط الكونسول لـUTF-8 (آمن على POSIX — no-op)
        // (EN) Set console to UTF-8 (safe no-op on POSIX)
        // ----------------------------------------------------------------------------
        inline void enableUtf8Console()
        {
#ifdef _WIN32
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
#endif
        }

        // ----------------------------------------------------------------------------
        // (AR) Utf8ArgvHolder: يعيد ربط argc/argv في مكانهما إلى UTF-8
        //      احفظه كمتغير محلي حتى نهاية main لإبقاء المخزن حياً.
        // (EN) Utf8ArgvHolder: rebinds argc/argv in-place to UTF-8.
        //      Keep as a local in main so storage outlives any argv use.
        // ----------------------------------------------------------------------------
        class Utf8ArgvHolder
        {
        public:
            Utf8ArgvHolder(int &argc, char **&argv)
            {
                args_ = getUtf8Args(argc, argv);
                ptrs_.resize(args_.size() + 1, nullptr);
                for (size_t i = 0; i < args_.size(); ++i)
                {
                    ptrs_[i] = const_cast<char *>(args_[i].c_str());
                }
                argc = static_cast<int>(args_.size());
                argv = ptrs_.data();
            }
            Utf8ArgvHolder(const Utf8ArgvHolder &) = delete;
            Utf8ArgvHolder &operator=(const Utf8ArgvHolder &) = delete;

            const std::vector<std::string> &args() const { return args_; }

        private:
            std::vector<std::string> args_;
            std::vector<char *> ptrs_;
        };

#ifdef _WIN32
        // (AR) معاد للاستخدام: تحويل سلسلة wchar_t إلى UTF-8
        // (EN) Helper: convert wchar_t string to UTF-8
        inline std::string wideToUtf8(const wchar_t *w)
        {
            if (!w)
                return {};
            int len = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
            if (len <= 0)
                return {};
            std::string out(static_cast<size_t>(len - 1), '\0');
            WideCharToMultiByte(CP_UTF8, 0, w, -1, out.data(), len, nullptr, nullptr);
            return out;
        }
#endif

    } // namespace Utils
} // namespace Sad

// ============================================================================
// (AR) مستعارات توافقية للاستخدام السابق Sad::HubUtil
// (EN) Backward-compat aliases for legacy Sad::HubUtil
// ============================================================================
namespace Sad
{
    namespace HubUtil
    {
        using Sad::Utils::getUtf8Args;
#ifdef _WIN32
        using Sad::Utils::wideToUtf8;
#endif
    } // namespace HubUtil
} // namespace Sad
