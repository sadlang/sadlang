// (AR) تنفيذ عارض المساعدة
// (EN) Help renderer implementation

#include "hub/help_renderer.h"

#include <algorithm>
// (AR) <cstdint> صراحةً لـ SIZE_MAX — GCC/Clang لا يضمّانه ضمنيًا (يعمل على MSVC).
// (EN) Explicit <cstdint> for SIZE_MAX — not pulled transitively on GCC/Clang.
#include <cstdint>
#include <sstream>

namespace Sad
{
    namespace Hub
    {

        // ============================================================================
        HelpRenderer::HelpRenderer(const ToolRegistry &registry)
            : registry_(registry) {}

        // ============================================================================
        // (AR) المساعدة الرئيسية / (EN) Main help
        // ============================================================================
        std::string HelpRenderer::renderMainHelp(const std::string &hubVersion) const
        {
            std::ostringstream os;
            os << "sad - مركز أدوات لغة ص (Sad Language Tool Hub) v" << hubVersion << "\n\n";

            os << "الاستخدام / Usage:\n";
            os << "  sad <أمر> [خيارات...] [ملف.ص]\n";
            os << "  sad <command> [options...] [file.sad]\n\n";

            os << "الأدوات المتوفرة / Available tools:\n";
            if (registry_.all().empty())
            {
                os << "  (AR) لم تُكتشف أي أداة! تأكد أن sad-*.exe في PATH\n";
                os << "  (EN) no tools discovered! ensure sad-*.exe is in PATH\n";
            }
            else
            {
                // (AR) عرض الأدوات الشائعة أولاً ثم الباقي
                // (EN) Show common tools first, then the rest
                const std::vector<std::string> commonOrder = {
                    "run", "build", "check", "fmt", "repl"};
                std::vector<ToolDescriptor> common, others;
                for (const auto &t : registry_.all())
                {
                    if (std::find(commonOrder.begin(), commonOrder.end(), t.name) != commonOrder.end())
                    {
                        common.push_back(t);
                    }
                    else
                    {
                        others.push_back(t);
                    }
                }
                std::sort(common.begin(), common.end(),
                          [&](const ToolDescriptor &a, const ToolDescriptor &b)
                          {
                              auto ai = std::find(commonOrder.begin(), commonOrder.end(), a.name);
                              auto bi = std::find(commonOrder.begin(), commonOrder.end(), b.name);
                              return ai < bi;
                          });

                auto printTool = [&](const ToolDescriptor &t)
                {
                    os << "  sad " << t.name;
                    for (size_t i = t.name.size(); i < 12; ++i)
                        os << ' ';
                    if (!t.description.empty())
                        os << " - " << t.description;
                    os << "\n";
                };

                if (!common.empty())
                {
                    os << "  (الأوامر اليومية / common):\n";
                    for (const auto &t : common)
                        printTool(t);
                }
                if (!others.empty())
                {
                    os << "\n  (أدوات إضافية / extras):\n";
                    for (const auto &t : others)
                        printTool(t);
                }
            }

            os << "\nأوامر مدمجة / Built-in commands:\n";
            os << "  sad --help, -h        " << "عرض هذه الرسالة / show this message\n";
            os << "  sad --version, -v     " << "الإصدار / version info\n";
            os << "  sad --list            " << "قائمة الأدوات بمساراتها / list tools with paths\n";

            os << "\nأمثلة / Examples:\n";
            os << "  sad run hello.ص              # تشغيل بالمفسّر / run with interpreter\n";
            os << "  sad build hello.ص -o app.exe # ترجمة إلى تنفيذي / compile to executable\n";
            os << "  sad check hello.ص            # فحص ملكية وأنواع / ownership & type check\n";
            os << "  sad fmt hello.ص              # تنسيق الكود / format code\n";

            return os.str();
        }

        // ============================================================================
        std::string HelpRenderer::renderToolList() const
        {
            std::ostringstream os;
            os << "الأدوات المكتشَفة / Discovered tools (" << registry_.all().size() << "):\n\n";

            if (registry_.all().empty())
            {
                os << "  (AR) لا توجد أدوات. مسارات البحث:\n";
                os << "  (EN) no tools. searched paths:\n";
                for (const auto &p : registry_.searchedPaths())
                {
                    os << "    - " << p << "\n";
                }
                return os.str();
            }

            for (const auto &t : registry_.all())
            {
                os << "  sad-" << t.name;
                for (size_t i = t.name.size(); i < 14; ++i)
                    os << ' ';
                if (t.isBuiltin)
                {
                    os << " [مدمج / builtin]";
                }
                else
                {
                    os << " " << t.executable;
                }
                os << "\n";
            }
            return os.str();
        }

        // ============================================================================
        std::string HelpRenderer::renderVersion(const std::string &hubVersion) const
        {
            std::ostringstream os;
            os << "sad " << hubVersion << "\n";
            os << "(AR) مركز أدوات لغة ص — Sad Language Tool Hub\n";
            os << "(AR) أدوات مكتشَفة: " << registry_.all().size() << "\n";
            os << "(EN) discovered tools: " << registry_.all().size() << "\n";
            return os.str();
        }

        // ============================================================================
        // (AR) مسافة Levenshtein لاقتراح أقرب أمر
        // (EN) Levenshtein distance for nearest-command suggestions
        // ============================================================================

        namespace
        {

            size_t levenshtein(const std::string &a, const std::string &b)
            {
                const size_t m = a.size(), n = b.size();
                if (m == 0)
                    return n;
                if (n == 0)
                    return m;

                std::vector<std::vector<size_t>> dp(m + 1, std::vector<size_t>(n + 1));
                for (size_t i = 0; i <= m; ++i)
                    dp[i][0] = i;
                for (size_t j = 0; j <= n; ++j)
                    dp[0][j] = j;

                for (size_t i = 1; i <= m; ++i)
                {
                    for (size_t j = 1; j <= n; ++j)
                    {
                        size_t cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
                        dp[i][j] = std::min({
                            dp[i - 1][j] + 1,       // delete
                            dp[i][j - 1] + 1,       // insert
                            dp[i - 1][j - 1] + cost // substitute
                        });
                    }
                }
                return dp[m][n];
            }

        } // anonymous

        std::string HelpRenderer::findClosestMatch(const std::string &given) const
        {
            std::string best;
            size_t bestDist = SIZE_MAX;
            for (const auto &t : registry_.all())
            {
                size_t d = levenshtein(given, t.name);
                if (d < bestDist)
                {
                    bestDist = d;
                    best = t.name;
                }
            }
            // (AR) لا تقترح إذا كانت المسافة كبيرة جداً
            // (EN) Don't suggest if distance too large
            if (bestDist > 3 || bestDist > given.size() / 2 + 1)
                return "";
            return best;
        }

        // ============================================================================
        std::string HelpRenderer::renderUnknownCommand(const std::string &given) const
        {
            std::ostringstream os;
            os << "(AR) خطأ: أمر غير معروف '" << given << "'\n";
            os << "(EN) error: unknown command '" << given << "'\n";

            std::string suggestion = findClosestMatch(given);
            if (!suggestion.empty())
            {
                os << "\n(AR) هل تقصد: sad " << suggestion << "؟\n";
                os << "(EN) did you mean: sad " << suggestion << "?\n";
            }

            os << "\n(AR) شغّل 'sad --help' لعرض الأوامر المتوفرة\n";
            os << "(EN) run 'sad --help' to see available commands\n";
            return os.str();
        }

    } // namespace Hub
} // namespace Sad
