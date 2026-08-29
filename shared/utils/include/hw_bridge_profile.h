// بسم الله الرحمن الرحيم
// ============================================================================
// hw_bridge_profile.h — تصنيفُ بيئةِ جسورِ العتاد، بلا LLVM
// hw_bridge_profile.h — hardware-bridge environment classification, LLVM-free
// ============================================================================
// (AR) هذه الترويسةُ موضعُ **القرارِ الوحيد**: أيُّ بيئةِ جسورٍ يقعُ فيها هدفٌ ما.
//      كانَ القرارُ قبلَها محبوسًا في `classifyHwBridgeProfile(const llvm::Triple&)`
//      داخلَ مولّدِ LLVM، فكانَ كلُّ مَن يحتاجُه — والسائقُ منهم — يجرُّ LLVM كاملةً
//      لأجلِ ثلاثةِ أسطرِ منطق. وبإخراجِه إلى هنا صارَ:
//        • مولّدُ LLVM يستدعيه بمُحوِّلٍ يترجمُ تعدادات `llvm::Triple` إلى رموزٍ
//          قانونيّة — **مطابقةُ أسماءٍ لا قرارٌ ثانٍ**؛
//        • والمترجمُ النحيلُ `sad-build-native` (بلا LLVM البتّة) يستدعيه بمُحوِّلٍ
//          نصّيٍّ يمسحُ مكوّناتِ الثالوث.
//      فالقرارُ نسخةٌ واحدةٌ، والمُحوِّلانِ اسميّانِ لا حُكميّان.
//
//      ⚠️ **لماذا المسحُ لا القراءةُ بالموضع؟** ثالوثٌ ثلاثيُّ المكوّنات مثل
//      «x86_64-linux-gnu» يُقرأُ مكوّنُه الثاني بائعًا لا نظامًا (وهو ما يفعلُه
//      `TargetTriple::parse` فعلًا: vendor="linux" وos="gnu")، فالقراءةُ بالموضعِ
//      تُصنِّفُ لينكسَ «معدنًا عاريًا» خطأً. ولذلك نمسحُ المكوّناتِ كلَّها بحثًا عن
//      اسمِ نظامٍ معروف — وهو ما يفعلُه `llvm::Triple::normalize` بأسلوبٍ آخر.
//
// (EN) This header is the *single decision site* for which hardware-bridge
//      environment a target falls into. The decision used to live inside
//      `classifyHwBridgeProfile(const llvm::Triple&)` in the LLVM code
//      generator, so every consumer — the compiler driver included — had to
//      drag all of LLVM in for three lines of logic. Hoisted here:
//        • the LLVM generator calls it through an adapter that maps
//          `llvm::Triple` enums to canonical tokens — a *name mapping*, not a
//          second decision;
//        • the thin `sad-build-native` compiler (no LLVM at all) calls it
//          through a textual adapter that scans the triple's components.
//      One decision, two purely nominal adapters.
//
//      ⚠️ *Why scan instead of reading by position?* A 3-component triple such
//      as "x86_64-linux-gnu" has its second field read as the *vendor*, not the
//      OS (which is exactly what `TargetTriple::parse` does: vendor="linux",
//      os="gnu"), so positional reading would misclassify Linux as bare metal.
//      We therefore scan every component for a known OS name — the same end
//      that `llvm::Triple::normalize` reaches by another route.
// ============================================================================

#ifndef SAD_SHARED_HW_BRIDGE_PROFILE_H
#define SAD_SHARED_HW_BRIDGE_PROFILE_H

#include <cstddef>
#include <string>

namespace sad
{
    namespace target
    {
        // ====================================================================
        // (AR) بيئةُ جسورِ العتاد — المميِّزُ الوحيدُ لكلِّ توليدٍ يلمسُ العتادَ أو
        //      نظامَ التشغيلِ في الوضعِ الحرّ. رايةُ «--حرّ» وحدَها لا تكفي: هي تصفُ
        //      **غيابَ المكتبةِ القياسيّة**، لا **حلقةَ الامتياز** ولا **المعماريّة**.
        // (EN) The hardware-bridge environment — the single discriminator for
        //      every emission touching hardware or the OS in freestanding mode.
        // ====================================================================
        enum class HwBridgeProfile
        {
            // (AR) معدنٌ عارٍ على x86/x86_64: الحلقة 0 — منافذُ الدخل/الخرج وcli/hlt متاحة.
            // (EN) Bare metal on x86/x86_64: ring 0 — port I/O and cli/hlt available.
            BareMetalPortIO,
            // (AR) معدنٌ عارٍ على معماريّةٍ بلا جسرٍ معروف (aarch64/riscv64/…):
            //      inb/outb لا وجودَ لها، والمنفذُ التسلسليُّ ذاكرةٌ خاصّةٌ باللوحة.
            // (EN) Bare metal on an architecture with no known bridge.
            BareMetalStub,
            // (AR) لينكس بمعماريّةٍ لها نداءُ نظامٍ مبثوث (x86_64: syscall، i386: int 0x80).
            // (EN) Linux on an architecture with an inline syscall.
            LinuxSyscall,
            // (AR) نظامٌ آخر (ويندوز/ماك) أو لينكس بمعماريّةٍ لا نبثُّ لها نداءَ نظام.
            // (EN) Another OS (Windows/macOS), or Linux on an arch with no syscall.
            HostedLibc
        };

        // ====================================================================
        // (AR) رمزُ المعماريّةِ القانونيّ. تعدادٌ مغلقٌ عمدًا: المُحوِّلانِ (LLVM والنصّيّ)
        //      يلتقيانِ عليه، فلا يمرُّ نصٌّ خامٌّ إلى موضعِ القرار.
        // (EN) Canonical architecture token. Deliberately a closed enum: both
        //      adapters meet on it, so no raw string reaches the decision site.
        // ====================================================================
        enum class ArchToken
        {
            X86,      // i386/i486/i586/i686/x86
            X86_64,   // x86_64/amd64/x64
            AArch64,  // aarch64/arm64/aarch64_be
            Arm,      // arm/armv7/thumb
            Riscv32,
            Riscv64,
            Other     // (AR) كلُّ ما عداها / (EN) everything else
        };

        // ====================================================================
        // (AR) رمزُ النظامِ القانونيّ. «مجهول» تعني هنا **معدنًا عاريًا**: ثالوثٌ بلا
        //      نظامٍ مذكور (aarch64) أو بنظامِ «none»/«elf»/«unknown» لا يدّعي نظامَ
        //      تشغيل، والوضعُ الحرُّ هو حالتُه الطبيعيّة.
        // (EN) Canonical OS token. "Unknown" means *bare metal* here.
        // ====================================================================
        enum class OsToken
        {
            Unknown,     // (AR) معدنٌ عارٍ / (EN) bare metal
            Linux,       // (AR) لينكس (وأندرويد نواتُه لينكس) / (EN) Linux (Android too)
            OtherHosted  // (AR) ويندوز/ماك/غيرهما / (EN) Windows/macOS/other
        };

        // ====================================================================
        // (AR) **القرار**. ثلاثةُ أسطرٍ لا رابع: نظامٌ مجهولٌ ⇒ معدنٌ (بمنافذَ أو
        //      بلا جسر)، ثمّ لينكس بمعماريّةٍ ذاتِ نداءٍ ⇒ نداءُ نظام، وإلّا مُستضاف.
        // (EN) *The* decision. Three lines and no fourth.
        // ====================================================================
        inline HwBridgeProfile classifyHwBridgeProfile(ArchToken arch, OsToken os)
        {
            if (os == OsToken::Unknown)
            {
                return (arch == ArchToken::X86 || arch == ArchToken::X86_64)
                           ? HwBridgeProfile::BareMetalPortIO
                           : HwBridgeProfile::BareMetalStub;
            }

            if (os == OsToken::Linux &&
                (arch == ArchToken::X86 || arch == ArchToken::X86_64))
            {
                return HwBridgeProfile::LinuxSyscall;
            }

            return HwBridgeProfile::HostedLibc;
        }

        namespace detail
        {
            // (AR) مقارنةٌ لا تبالي بحالةِ الحرف — الثالوثُ يُكتَبُ بحالاتٍ مختلطة.
            // (EN) Case-insensitive compare — triples arrive in mixed case.
            inline std::string toLowerAscii(const std::string &s)
            {
                std::string out = s;
                for (char &c : out)
                {
                    if (c >= 'A' && c <= 'Z')
                    {
                        c = static_cast<char>(c - 'A' + 'a');
                    }
                }
                return out;
            }

            inline bool startsWith(const std::string &s, const char *prefix)
            {
                const std::string p(prefix);
                return s.size() >= p.size() && s.compare(0, p.size(), p) == 0;
            }
        } // namespace detail

        // ====================================================================
        // (AR) المعماريّةُ من **المكوّنِ الأوّل** للثالوث — وموضعُها ثابتٌ في كلِّ
        //      الصيغِ فلا يحتاجُ مسحًا. الكُنى مُوحَّدة: arm64 كنيةُ aarch64، وamd64
        //      كنيةُ x86_64، وi686 كنيةُ x86 — وهو التوحيدُ نفسُه الذي يفعلُه
        //      `llvm::Triple::normalize`.
        // (EN) Architecture from the triple's *first* component — its position is
        //      fixed in every form, so no scan is needed. Aliases are unified.
        // ====================================================================
        inline ArchToken archTokenFromName(const std::string &rawArch)
        {
            const std::string a = detail::toLowerAscii(rawArch);

            if (a == "x86_64" || a == "amd64" || a == "x64" || a == "x86-64")
            {
                return ArchToken::X86_64;
            }
            if (a == "x86" || a == "i386" || a == "i486" || a == "i586" || a == "i686")
            {
                return ArchToken::X86;
            }
            if (a == "aarch64" || a == "arm64" || a == "aarch64_be" || a == "arm64_32")
            {
                return ArchToken::AArch64;
            }
            if (a == "riscv64")
            {
                return ArchToken::Riscv64;
            }
            if (a == "riscv32")
            {
                return ArchToken::Riscv32;
            }
            if (a == "arm" || a == "thumb" || detail::startsWith(a, "armv") ||
                detail::startsWith(a, "thumbv"))
            {
                return ArchToken::Arm;
            }

            return ArchToken::Other;
        }

        // ====================================================================
        // (AR) اسمُ نظامٍ واحدٍ ⇒ رمزُه، أو «ليسَ اسمَ نظامٍ» حين لا يكونُ المكوّنُ كذلك
        //      (بائعٌ مثل pc/unknown، أو بيئةٌ مثل gnu/musl، أو «none»/«elf»).
        // (EN) One component ⇒ its OS token, or "not an OS name here".
        // ====================================================================
        inline bool osTokenFromComponent(const std::string &rawComponent, OsToken &out)
        {
            const std::string c = detail::toLowerAscii(rawComponent);

            // (AR) لينكس — وأندرويد نواتُه لينكس، وllvm::Triple::isOSLinux تعدُّه لينكس.
            // (EN) Linux — Android's kernel is Linux and llvm counts it as Linux.
            if (detail::startsWith(c, "linux") || detail::startsWith(c, "android"))
            {
                out = OsToken::Linux;
                return true;
            }

            // (AR) أنظمةٌ مُستضيفةٌ أخرى — الاسمُ قد يحملُ إصدارًا («darwin23»).
            // (EN) Other hosted OSes — the name may carry a version ("darwin23").
            static const char *const kHostedPrefixes[] = {
                "windows", "win32", "mingw", "msvc", "cygwin", "darwin", "macos",
                "macosx", "ios", "tvos", "watchos", "freebsd", "netbsd", "openbsd",
                "dragonfly", "solaris", "haiku", "fuchsia", "wasi", "emscripten",
                "ps4", "ps5", "aix", "hurd", "zos"};
            for (const char *prefix : kHostedPrefixes)
            {
                if (detail::startsWith(c, prefix))
                {
                    out = OsToken::OtherHosted;
                    return true;
                }
            }

            return false;
        }

        // ====================================================================
        // (AR) النظامُ **بمسحِ المكوّناتِ كلِّها** بدءًا من الثاني. لا قراءةَ بالموضع:
        //      انظر تحذيرَ الترويسةِ أعلاه — «x86_64-linux-gnu» يضعُ لينكسَ في خانةِ
        //      البائع. وإن لم يُعثَر على اسمِ نظامٍ في أيِّ مكوّنٍ فالهدفُ معدنٌ عارٍ.
        // (EN) The OS by *scanning every component* from the second on.
        // ====================================================================
        inline OsToken osTokenFromTripleText(const std::string &tripleText)
        {
            bool first = true;
            std::size_t start = 0;

            for (;;)
            {
                const std::size_t dash = tripleText.find('-', start);
                const std::size_t end =
                    (dash == std::string::npos) ? tripleText.size() : dash;

                if (!first)
                {
                    OsToken found = OsToken::Unknown;
                    if (osTokenFromComponent(tripleText.substr(start, end - start), found))
                    {
                        return found;
                    }
                }
                first = false;

                if (dash == std::string::npos)
                {
                    break;
                }
                start = dash + 1;
            }

            return OsToken::Unknown;
        }

        // ====================================================================
        // (AR) المعماريّةُ من نصِّ الثالوث — المكوّنُ الأوّل.
        // (EN) Architecture from triple text — the first component.
        // ====================================================================
        inline ArchToken archTokenFromTripleText(const std::string &tripleText)
        {
            const std::size_t dash = tripleText.find('-');
            return archTokenFromName(
                dash == std::string::npos ? tripleText : tripleText.substr(0, dash));
        }

        // ====================================================================
        // (AR) الطريقُ المختصرُ للمستهلكِ النصّيّ (المترجمُ النحيلُ بلا LLVM).
        // (EN) The shorthand for textual consumers (the LLVM-free thin compiler).
        // ====================================================================
        inline HwBridgeProfile classifyHwBridgeProfileFromTripleText(
            const std::string &tripleText)
        {
            return classifyHwBridgeProfile(archTokenFromTripleText(tripleText),
                                           osTokenFromTripleText(tripleText));
        }

    } // namespace target
} // namespace sad

#endif // SAD_SHARED_HW_BRIDGE_PROFILE_H
