// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file memory_mode_flag.cpp
 * @brief تنفيذ معالج أعلام وضع الذاكرة / Memory Mode Flag Handler
 *
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "memory/policy/memory_mode_flag.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib> // getenv
#include <map>

namespace Sad
{
    namespace Memory
    {

        // =============================================================================
        //                    البناء والتهيئة
        // =============================================================================

        MemoryModeFlag::MemoryModeFlag()
        {
            initializeFlags();
        }

        MemoryModeFlag::~MemoryModeFlag() = default;

        void MemoryModeFlag::initializeFlags()
        {
            // ==========================================================================
            // تعريفات الأعلام
            // ==========================================================================

            flagDefinitions_ = {
                // أعلام الوضع الرئيسي
                // (AR) ملاحظة: --تطوير/--development/--dev/-d و --مختلط/--hybrid/--mixed
                //      أُزيلت نهائياً في Phase E-3 توحيد الأعلام الثلاثة. استخدم --gc بديلاً.
                // (EN) Note: --تطوير/--development/--dev/-d and --مختلط/--hybrid/--mixed
                //      are removed in Phase E-3 unified flags. Use --gc instead.
                {"إنتاج",
                 "production",
                 "p",
                 "وضع الإنتاج مع نظام ملكية صارم (كـ Rust)",
                 false,
                 ""},
                {"تعلم",
                 "learn",
                 "l",
                 "وضع التعلم: رسائل تعليمية مفصلة عن إدارة الذاكرة",
                 false,
                 ""},
                {"تلقائي",
                 "auto",
                 "a",
                 "اكتشاف تلقائي لأفضل وضع بناءً على حجم المشروع",
                 false,
                 ""},

                // (AR) أعلام GC: --gc وحده يفعّل وضع gcDefaults (لا يستهلك قيمة).
                //      استخدم --gc=<strategy> أو --gc-strategy <s> لضبط الاستراتيجية.
                // (EN) GC flags: bare --gc enables gcDefaults mode (consumes no value).
                //      Use --gc=<strategy> or --gc-strategy <s> to set strategy.
                {
                    "gc",
                    "gc",
                    "",
                    "وضع GC الافتراضي (جامع قمامة + ملكية معطَّلة)",
                    false,
                    ""},
                {"gc-strategy",
                 "gc-strategy",
                 "",
                 "استراتيجية جامع القمامة: none|refcount|atomic|tracing|incremental",
                 true,
                 "refcount"},

                // أعلام الملكية
                // (AR) ملاحظة (إصلاح S-TS): أُزيل الاسم المختصر "o" — كان يتعارض مع علم
                //      الإخراج العام "-o" في مترجم sadc فيبتلعه ويمنع كتابة التنفيذيّ.
                //      مستوى الملكية يُستخدَم عبر --ownership / --ملكية فقط.
                // (EN) Fix (S-TS): removed short alias "o" — it collided with the universal
                //      output flag "-o" of the sadc compiler, swallowing it and its value so
                //      no executable was written. Use --ownership / --ملكية only.
                {
                    "ملكية",
                    "ownership",
                    "",
                    "مستوى فحص الملكية: off|warnings|strict|ultra",
                    true,
                    "warnings"},

                // أعلام إضافية
                {
                    "اقتراحات",
                    "suggestions",
                    "s",
                    "تفعيل اقتراحات تحويل الملكية",
                    false,
                    ""},
                {"كشف_دورات",
                 "detect-cycles",
                 "",
                 "تفعيل كشف دورات المراجع",
                 false,
                 ""},
                {"حد_ذاكرة",
                 "gc-memory-limit",
                 "",
                 "حد ذاكرة GC بالميغابايت",
                 true,
                 "1024"},
                {"تصحيح",
                 "debug-memory",
                 "",
                 "تفعيل رسائل تصحيح الذاكرة",
                 false,
                 ""}};

            // ==========================================================================
            // معالجات الأعلام
            // ==========================================================================

            // أعلام الوضع
            auto modeHandler = [this](MemoryModeSettings &s, const std::string &v)
            {
                handleModeFlag(s, v);
            };

            // (AR) أعلام وضع GC الافتراضي (--gc = كان --dev)
            // (EN) GC mode flags (--gc replaces deprecated --dev)
            flagHandlers_["--gc-mode"] = [](MemoryModeSettings &s, const std::string &)
            {
                s = MemoryModeSettings::gcDefaults();
            };
            // (AR) ملاحظة Phase E-3: أُزيلت معالجات --تطوير/--development/--dev/-d نهائياً.
            //      استخدم --gc بديلاً. أي استخدام لها سيُرفض كعَلَم غير معروف.
            // (EN) Phase E-3 cleanup: removed handlers for --تطوير/--development/--dev/-d.
            //      Use --gc instead. Any usage will be rejected as unknown flag.

            flagHandlers_["--إنتاج"] = [](MemoryModeSettings &s, const std::string &)
            {
                s = MemoryModeSettings::productionDefaults();
            };
            flagHandlers_["--production"] = flagHandlers_["--إنتاج"];
            flagHandlers_["--prod"] = flagHandlers_["--إنتاج"];
            flagHandlers_["--release"] = flagHandlers_["--إنتاج"];
            flagHandlers_["-p"] = flagHandlers_["--إنتاج"];

            // (AR) ملاحظة Phase E-3: أُزيلت معالجات --مختلط/--hybrid/--mixed نهائياً.
            //      استخدم --gc أو --learn بدلاً. أي استخدام سيُرفض كعَلَم غير معروف.
            // (EN) Phase E-3 cleanup: removed handlers for --مختلط/--hybrid/--mixed.
            //      Use --gc or --learn instead. Any usage will be rejected as unknown flag.

            flagHandlers_["--تعلم"] = [](MemoryModeSettings &s, const std::string &)
            {
                s = MemoryModeSettings::learningDefaults();
            };
            flagHandlers_["--learn"] = flagHandlers_["--تعلم"];
            flagHandlers_["--learning"] = flagHandlers_["--تعلم"];
            flagHandlers_["-l"] = flagHandlers_["--تعلم"];

            flagHandlers_["--تلقائي"] = [](MemoryModeSettings &s, const std::string &)
            {
                s.mode = MemoryMode::Auto;
            };
            flagHandlers_["--auto"] = flagHandlers_["--تلقائي"];
            flagHandlers_["-a"] = flagHandlers_["--تلقائي"];

            // (AR) أعلام GC: --gc بدون قيمة = gcDefaults() (وضع التطوير الكامل
            //      مع OwnershipMode::Disabled). --gc=<strategy> أو --gc-strategy <s>
            //      يضبط استراتيجية GC فقط دون تغيير الوضع.
            // (EN) GC flags: bare --gc => gcDefaults() (full dev mode with
            //      OwnershipMode::Disabled). --gc=<strategy> or --gc-strategy <s>
            //      tweaks GC strategy only without changing the mode.
            flagHandlers_["--gc"] = [this](MemoryModeSettings &s, const std::string &v)
            {
                if (v.empty())
                {
                    s = MemoryModeSettings::gcDefaults();
                }
                else
                {
                    handleGCFlag(s, v);
                }
            };
            flagHandlers_["--gc-strategy"] = [this](MemoryModeSettings &s, const std::string &v)
            {
                handleGCFlag(s, v);
            };

            // أعلام الملكية
            flagHandlers_["--ملكية"] = [this](MemoryModeSettings &s, const std::string &v)
            {
                handleOwnershipFlag(s, v);
            };
            flagHandlers_["--ownership"] = flagHandlers_["--ملكية"];
            // (AR) إصلاح S-TS: لا تُسجّل "-o" للملكية — تتعارض مع علم الإخراج العام في sadc.
            // (EN) Fix S-TS: do NOT register "-o" for ownership — collides with sadc's output flag.

            // أعلام إضافية
            flagHandlers_["--اقتراحات"] = [](MemoryModeSettings &s, const std::string &)
            {
                s.enableOwnershipSuggestions = true;
            };
            flagHandlers_["--suggestions"] = flagHandlers_["--اقتراحات"];
            flagHandlers_["-s"] = flagHandlers_["--اقتراحات"];

            flagHandlers_["--كشف_دورات"] = [](MemoryModeSettings &s, const std::string &)
            {
                s.enableCycleDetection = true;
            };
            flagHandlers_["--detect-cycles"] = flagHandlers_["--كشف_دورات"];

            flagHandlers_["--حد_ذاكرة"] = [](MemoryModeSettings &s, const std::string &v)
            {
                try
                {
                    s.gcMemoryLimitMB = std::stoul(v);
                }
                catch (...)
                {
                    // تجاهل القيم غير الصالحة
                }
            };
            flagHandlers_["--gc-memory-limit"] = flagHandlers_["--حد_ذاكرة"];

            flagHandlers_["--تصحيح"] = [](MemoryModeSettings &s, const std::string &)
            {
                s.debugMode = true;
            };
            flagHandlers_["--debug-memory"] = flagHandlers_["--تصحيح"];

            // (AR) علم بلا مكتبة قياسية → فرض وضع النواة (بلا GC، ملكية صارمة)
            // (EN) No-std flag → force kernel mode (no GC, strict ownership)
            flagHandlers_["--بلا-مكتبة-قياسية"] = [](MemoryModeSettings &s, const std::string &)
            {
                s = MemoryModeSettings::kernelDefaults();
            };
            flagHandlers_["--no-std"] = flagHandlers_["--بلا-مكتبة-قياسية"];
            flagHandlers_["--freestanding"] = flagHandlers_["--بلا-مكتبة-قياسية"];
            flagHandlers_["--kernel"] = flagHandlers_["--بلا-مكتبة-قياسية"];
            flagHandlers_["--نواة"] = flagHandlers_["--بلا-مكتبة-قياسية"];
        }

        // =============================================================================
        //                    التحليل
        // =============================================================================

        FlagParseResult MemoryModeFlag::parse(int argc, char *argv[])
        {
            std::vector<std::string> args;
            for (int i = 1; i < argc; i++)
            {
                args.push_back(argv[i]);
            }
            return parse(args);
        }

        FlagParseResult MemoryModeFlag::parse(const std::vector<std::string> &args)
        {
            FlagParseResult result;
            result.success = true;
            result.settings = MemoryModeSettings::developmentDefaults(); // افتراضي

            // (AR) Phase E-3: قائمة الأعلام المُزالة نهائياً مع توجيه إلى البديل.
            //      عند رؤية أي منها يُسجَّل خطأ صارم ويفشل التحليل (success=false).
            // (EN) Phase E-3: hard-removed flags with redirect hints. Encountering
            //      any of them records a fatal error and parsing fails (success=false).
            static const std::map<std::string, std::string> deprecatedFlags = {
                {"--dev", "--gc"},
                {"--development", "--gc"},
                {"-d", "--gc"},
                {"--تطوير", "--gc"},
                {"--hybrid", "--gc (أو --learn)"},
                {"--mixed", "--gc (أو --learn)"},
                {"--مختلط", "--gc (أو --learn)"},
            };

            for (size_t i = 0; i < args.size(); i++)
            {
                const std::string &arg = args[i];

                // (AR) Phase E-3: رفض صارم للأعلام المُزالة (--dev/--hybrid وغيرها).
                // (EN) Phase E-3: hard rejection of removed flags (--dev/--hybrid etc.).
                {
                    std::string baseForCheck = arg;
                    auto eq = baseForCheck.find('=');
                    if (eq != std::string::npos)
                        baseForCheck = baseForCheck.substr(0, eq);
                    auto depIt = deprecatedFlags.find(baseForCheck);
                    if (depIt != deprecatedFlags.end())
                    {
                        result.success = false;
                        result.errors.push_back(
                            "العَلَم '" + baseForCheck + "' أُزيل نهائياً في Phase E-3. "
                                                       "استخدم '" +
                            depIt->second + "' بديلاً. / "
                                            "Flag '" +
                            baseForCheck + "' was removed in Phase E-3. "
                                           "Use '" +
                            depIt->second + "' instead.");
                        continue;
                    }
                }

                // تجاهل الأعلام غير المتعلقة بالذاكرة
                if (!isMemoryFlag(arg))
                {
                    result.remainingArgs.push_back(arg);
                    continue;
                }

                // البحث عن المعالج
                std::string baseArg = arg;
                std::string value;

                // التعامل مع صيغة --flag=value
                auto eqPos = arg.find('=');
                if (eqPos != std::string::npos)
                {
                    baseArg = arg.substr(0, eqPos);
                    value = arg.substr(eqPos + 1);
                }

                auto it = flagHandlers_.find(baseArg);
                if (it != flagHandlers_.end())
                {
                    // إذا كان العلم يحتاج قيمة ولم نجدها بعد '='
                    if (value.empty())
                    {
                        // فحص العلم التالي كقيمة
                        for (const auto &def : flagDefinitions_)
                        {
                            if (baseArg == "--" + def.longNameArabic ||
                                baseArg == "--" + def.longNameEnglish ||
                                baseArg == "-" + def.shortName)
                            {
                                if (def.hasValue && i + 1 < args.size())
                                {
                                    value = args[++i];
                                }
                                break;
                            }
                        }
                    }

                    try
                    {
                        it->second(result.settings, value);
                    }
                    catch (const std::exception &e)
                    {
                        result.warnings.push_back(
                            "تحذير: فشل معالجة العلم '" + arg + "': " + e.what());
                    }
                }
                else
                {
                    result.warnings.push_back(
                        "تحذير: علم غير معروف '" + arg + "'");
                    result.remainingArgs.push_back(arg);
                }
            }

            // تطبيق إعدادات البيئة (إذا لم يتم تحديد وضع صريح)
            applyEnvironmentSettings(result.settings);

            // ═══════════════════════════════════════════════════════════════════════
            // (AR) التحقق من تعارض no_std مع GC
            // (EN) Validate no_std / GC conflicts
            // ═══════════════════════════════════════════════════════════════════════

            bool isNoStd = false;
            for (const auto &arg : args)
            {
                if (arg == "--بلا-مكتبة-قياسية" || arg == "--no-std" ||
                    arg == "--freestanding" || arg == "--kernel" || arg == "--نواة")
                {
                    isNoStd = true;
                    break;
                }
            }
            // (AR) بلّغ المشغّل: هذه الأعلام تُستهلك هنا فلا تصل محلّله الرئيسيّ
            // (EN) Inform the driver: these flags are consumed here and never
            //      reach its main CLI parser
            result.noStdRequested = isNoStd;

            if (isNoStd)
            {
                // (AR) فرض: لا GC في وضع بلا مكتبة قياسية
                // (EN) Enforce: no GC in no_std mode
                if (result.settings.gcStrategy != GCStrategy::None)
                {
                    result.success = false;
                    result.warnings.push_back(
                        "خطأ: لا يمكن استخدام جامع القمامة في وضع بلا مكتبة قياسية\n"
                        "Error: Cannot use garbage collector in no_std mode");
                    // (AR) فرض الإعدادات الصحيحة
                    result.settings = MemoryModeSettings::kernelDefaults();
                }

                // (AR) التأكد من أن الملكية لا تُعطَّل
                if (result.settings.ownershipMode == OwnershipMode::Disabled)
                {
                    result.success = false;
                    result.warnings.push_back(
                        "خطأ: لا يمكن تعطيل نظام الملكية في وضع بلا مكتبة قياسية\n"
                        "Error: Cannot disable ownership in no_std mode");
                    result.settings.ownershipMode = OwnershipMode::UltraStrict;
                }
            }

            return result;
        }

        std::optional<MemoryModeSettings> MemoryModeFlag::parseFlag(const std::string &flag) const
        {
            std::string baseFlag = flag;
            std::string value;

            auto eqPos = flag.find('=');
            if (eqPos != std::string::npos)
            {
                baseFlag = flag.substr(0, eqPos);
                value = flag.substr(eqPos + 1);
            }

            auto it = flagHandlers_.find(baseFlag);
            if (it != flagHandlers_.end())
            {
                MemoryModeSettings settings;
                it->second(settings, value);
                return settings;
            }

            return std::nullopt;
        }

        // =============================================================================
        //                    المساعدة
        // =============================================================================

        std::string MemoryModeFlag::generateHelp(bool arabic) const
        {
            std::ostringstream help;

            if (arabic)
            {
                help << "\n";
                help << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
                help << "║                      أعلام وضع الذاكرة في لغة ص                               ║\n";
                help << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
                help << "\n";
                help << "  الأوضاع الرئيسية:\n";
                help << "  ─────────────────\n";
                help << "    --gc, --تطوير, --dev     وضع GC الافتراضي: جامع قمامة تلقائي (ملكية معطَّلة)\n";
                help << "                             مناسب للمبتدئين والتجارب السريعة\n";
                help << "\n";
                help << "    --إنتاج, --prod, -p      وضع الإنتاج: ملكية صارمة (كـ Rust)\n";
                help << "                             أقصى أداء مع أمان كامل في وقت الترجمة\n";
                help << "\n";
                help << "    --تعلم, --learn, -l      وضع التعلم: رسائل تعليمية مفصلة\n";
                help << "                             للتعلم التدريجي عن إدارة الذاكرة\n";
                help << "\n";
                help << "    --تلقائي, --auto, -a     اكتشاف تلقائي لأفضل وضع\n";
                help << "\n";
                help << "  خيارات متقدمة:\n";
                help << "  ─────────────────\n";
                help << "    --gc=STRATEGY            استراتيجية GC:\n";
                help << "                               none     - بدون GC (ملكية صرفة)\n";
                help << "                               refcount - عدّ مراجع (افتراضي للتطوير)\n";
                help << "                               atomic   - عدّ مراجع ذري (للمتزامن)\n";
                help << "                               tracing  - تتبع (Mark & Sweep)\n";
                help << "                               incremental - تدريجي\n";
                help << "\n";
                help << "    --ملكية=LEVEL            مستوى فحص الملكية:\n";
                help << "                               off      - إيقاف الفحص\n";
                help << "                               warnings - تحذيرات فقط (افتراضي)\n";
                help << "                               strict   - صارم (أخطاء)\n";
                help << "                               ultra    - صارم جداً (كـ Rust)\n";
                help << "\n";
                help << "    --اقتراحات, -s           تفعيل اقتراحات تحويل الملكية\n";
                help << "    --كشف_دورات              تفعيل كشف دورات المراجع\n";
                help << "    --حد_ذاكرة=MB            حد ذاكرة GC (افتراضي: 1024)\n";
                help << "    --تصحيح                  رسائل تصحيح الذاكرة\n";
                help << "\n";
                help << "  متغيرات البيئة:\n";
                help << "  ─────────────────\n";
                help << "    SAD_MEMORY_MODE          الوضع: gc|prod|learn|auto\n";
                help << "    SAD_GC_STRATEGY          استراتيجية GC\n";
                help << "    SAD_OWNERSHIP_LEVEL      مستوى الملكية\n";
                help << "\n";
                help << "  أمثلة:\n";
                help << "  ─────────────────\n";
                help << "    sadc --gc برنامجي.s                  # ترجمة بوضع GC الافتراضي\n";
                help << "    sadc --إنتاج --gc=none برنامجي.s    # ترجمة للإنتاج بدون GC\n";
                help << "    sadc --تعلم --اقتراحات برنامجي.s    # تعلم مع اقتراحات\n";
                help << "\n";
            }
            else
            {
                help << "\n";
                help << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
                help << "║                       Sad Language Memory Mode Flags                          ║\n";
                help << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
                help << "\n";
                help << "  Main Modes:\n";
                help << "  ───────────\n";
                help << "    --gc                      GC mode: automatic GC, no ownership checks (default)\n";
                help << "    --production, --prod, -p  Production mode: strict ownership (like Rust)\n";
                help << "    --learn, -l               Learning mode: detailed educational messages\n";
                help << "    --auto, -a                Auto-detect best mode\n";
                help << "\n";
                help << "  Advanced Options:\n";
                help << "  ─────────────────\n";
                help << "    --gc=STRATEGY             GC strategy: none|refcount|atomic|tracing|incremental\n";
                help << "    --ownership=LEVEL         Ownership level: off|warnings|strict|ultra\n";
                help << "    --suggestions, -s         Enable ownership suggestions\n";
                help << "    --detect-cycles           Enable reference cycle detection\n";
                help << "    --gc-memory-limit=MB      GC memory limit (default: 1024)\n";
                help << "    --debug-memory            Enable memory debug messages\n";
                help << "\n";
            }

            return help.str();
        }

        std::string MemoryModeFlag::generateShortHelp(bool arabic) const
        {
            if (arabic)
            {
                return "الاستخدام: sadc [--gc|--إنتاج|--تعلم] [خيارات] ملف.s\n"
                       "استخدم --مساعدة_ذاكرة لمزيد من المعلومات";
            }
            else
            {
                return "Usage: sadc [--gc|--prod|--learn] [options] file.s\n"
                       "Use --memory-help for more information";
            }
        }

        void MemoryModeFlag::printHelp(bool arabic) const
        {
            std::cout << generateHelp(arabic);
        }

        // =============================================================================
        //                    التحقق
        // =============================================================================

        bool MemoryModeFlag::isMemoryFlag(const std::string &flag) const
        {
            // استخراج الجزء الأساسي (قبل =)
            std::string baseFlag = flag;
            auto eqPos = flag.find('=');
            if (eqPos != std::string::npos)
            {
                baseFlag = flag.substr(0, eqPos);
            }

            return flagHandlers_.find(baseFlag) != flagHandlers_.end();
        }

        std::vector<FlagDefinition> MemoryModeFlag::getSupportedFlags() const
        {
            return flagDefinitions_;
        }

        // =============================================================================
        //                    معالجات الأعلام الداخلية
        // =============================================================================

        void MemoryModeFlag::handleModeFlag(MemoryModeSettings &settings, const std::string &mode) const
        {
            // (AR) Phase E-3: --dev/--development/--hybrid/--mixed أُزيلت نهائياً.
            //      استخدم gc بدلاً من dev/development، و gc أو learn بدلاً من hybrid/mixed.
            // (EN) Phase E-3: --dev/--development/--hybrid/--mixed removed permanently.
            //      Use 'gc' instead of dev/development, 'gc' or 'learn' instead of hybrid/mixed.
            if (mode == "gc")
            {
                settings = MemoryModeSettings::gcDefaults();
            }
            else if (mode == "إنتاج" || mode == "prod" || mode == "production")
            {
                settings = MemoryModeSettings::productionDefaults();
            }
            else if (mode == "تعلم" || mode == "learn" || mode == "learning")
            {
                settings = MemoryModeSettings::learningDefaults();
            }
            else if (mode == "تلقائي" || mode == "auto")
            {
                settings.mode = MemoryMode::Auto;
            }
        }

        void MemoryModeFlag::handleGCFlag(MemoryModeSettings &settings, const std::string &strategy) const
        {
            if (strategy == "none" || strategy == "بدون")
            {
                settings.gcStrategy = GCStrategy::None;
            }
            else if (strategy == "refcount" || strategy == "عد_مراجع")
            {
                settings.gcStrategy = GCStrategy::ReferenceCounting;
            }
            else if (strategy == "atomic" || strategy == "ذري")
            {
                settings.gcStrategy = GCStrategy::AtomicReferenceCounting;
            }
            else if (strategy == "tracing" || strategy == "تتبع")
            {
                settings.gcStrategy = GCStrategy::Tracing;
            }
            else if (strategy == "incremental" || strategy == "تدريجي")
            {
                settings.gcStrategy = GCStrategy::Incremental;
            }
        }

        void MemoryModeFlag::handleOwnershipFlag(MemoryModeSettings &settings, const std::string &level) const
        {
            if (level == "off" || level == "إيقاف")
            {
                settings.ownershipMode = OwnershipMode::Disabled;
            }
            else if (level == "warnings" || level == "تحذيرات")
            {
                settings.ownershipMode = OwnershipMode::Warnings;
            }
            else if (level == "strict" || level == "صارم")
            {
                settings.ownershipMode = OwnershipMode::Strict;
            }
            else if (level == "ultra" || level == "صارم_جداً")
            {
                settings.ownershipMode = OwnershipMode::UltraStrict;
            }
        }

        // =============================================================================
        //                    الدوال المساعدة
        // =============================================================================

        std::pair<std::vector<std::string>, std::vector<std::string>>
        extractMemoryFlags(const std::vector<std::string> &args)
        {
            MemoryModeFlag handler;
            std::vector<std::string> memoryFlags;
            std::vector<std::string> otherArgs;

            for (size_t i = 0; i < args.size(); i++)
            {
                const std::string &arg = args[i];

                if (handler.isMemoryFlag(arg))
                {
                    memoryFlags.push_back(arg);

                    // إذا كان العلم يأخذ قيمة، ضم القيمة التالية أيضاً
                    auto eqPos = arg.find('=');
                    if (eqPos == std::string::npos)
                    {
                        // التحقق إذا كان العلم يحتاج قيمة
                        for (const auto &def : handler.getSupportedFlags())
                        {
                            std::string fullArabic = "--" + def.longNameArabic;
                            std::string fullEnglish = "--" + def.longNameEnglish;
                            std::string shortFlag = "-" + def.shortName;

                            if ((arg == fullArabic || arg == fullEnglish ||
                                 (!def.shortName.empty() && arg == shortFlag)) &&
                                def.hasValue)
                            {
                                if (i + 1 < args.size())
                                {
                                    memoryFlags.push_back(args[++i]);
                                }
                                break;
                            }
                        }
                    }
                }
                else
                {
                    otherArgs.push_back(arg);
                }
            }

            return {memoryFlags, otherArgs};
        }

        bool applyEnvironmentSettings(MemoryModeSettings &settings)
        {
            bool applied = false;

            // SAD_MEMORY_MODE
            const char *modeEnv = std::getenv("SAD_MEMORY_MODE");
            if (modeEnv)
            {
                std::string mode(modeEnv);
                if (mode == "dev" || mode == "development")
                {
                    settings = MemoryModeSettings::developmentDefaults();
                    applied = true;
                }
                else if (mode == "prod" || mode == "production")
                {
                    settings = MemoryModeSettings::productionDefaults();
                    applied = true;
                }
                else if (mode == "hybrid")
                {
                    settings.mode = MemoryMode::Hybrid;
                    applied = true;
                }
                else if (mode == "learn" || mode == "learning")
                {
                    settings = MemoryModeSettings::learningDefaults();
                    applied = true;
                }
            }

            // SAD_GC_STRATEGY
            const char *gcEnv = std::getenv("SAD_GC_STRATEGY");
            if (gcEnv)
            {
                std::string strategy(gcEnv);
                if (strategy == "none")
                {
                    settings.gcStrategy = GCStrategy::None;
                    applied = true;
                }
                else if (strategy == "refcount")
                {
                    settings.gcStrategy = GCStrategy::ReferenceCounting;
                    applied = true;
                }
                else if (strategy == "atomic")
                {
                    settings.gcStrategy = GCStrategy::AtomicReferenceCounting;
                    applied = true;
                }
                else if (strategy == "tracing")
                {
                    settings.gcStrategy = GCStrategy::Tracing;
                    applied = true;
                }
            }

            // SAD_OWNERSHIP_LEVEL
            const char *ownershipEnv = std::getenv("SAD_OWNERSHIP_LEVEL");
            if (ownershipEnv)
            {
                std::string level(ownershipEnv);
                if (level == "off")
                {
                    settings.ownershipMode = OwnershipMode::Disabled;
                    applied = true;
                }
                else if (level == "warnings")
                {
                    settings.ownershipMode = OwnershipMode::Warnings;
                    applied = true;
                }
                else if (level == "strict")
                {
                    settings.ownershipMode = OwnershipMode::Strict;
                    applied = true;
                }
                else if (level == "ultra")
                {
                    settings.ownershipMode = OwnershipMode::UltraStrict;
                    applied = true;
                }
            }

            return applied;
        }

        std::optional<MemoryModeSettings> readConfigFile(const std::string &configPath)
        {
            std::ifstream file(configPath);
            if (!file.is_open())
            {
                return std::nullopt;
            }

            MemoryModeSettings settings;
            std::string line;

            while (std::getline(file, line))
            {
                // تجاهل التعليقات والأسطر الفارغة
                if (line.empty() || line[0] == '#' || line[0] == ';')
                {
                    continue;
                }

                // تحليل key=value
                auto eqPos = line.find('=');
                if (eqPos == std::string::npos)
                {
                    continue;
                }

                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);

                // إزالة المسافات
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                if (key == "mode" || key == "وضع")
                {
                    if (value == "development" || value == "تطوير")
                    {
                        settings = MemoryModeSettings::developmentDefaults();
                    }
                    else if (value == "production" || value == "إنتاج")
                    {
                        settings = MemoryModeSettings::productionDefaults();
                    }
                }
                else if (key == "gc_strategy" || key == "استراتيجية_gc")
                {
                    if (value == "none")
                        settings.gcStrategy = GCStrategy::None;
                    else if (value == "refcount")
                        settings.gcStrategy = GCStrategy::ReferenceCounting;
                    else if (value == "atomic")
                        settings.gcStrategy = GCStrategy::AtomicReferenceCounting;
                }
                else if (key == "ownership" || key == "ملكية")
                {
                    if (value == "off")
                        settings.ownershipMode = OwnershipMode::Disabled;
                    else if (value == "warnings")
                        settings.ownershipMode = OwnershipMode::Warnings;
                    else if (value == "strict")
                        settings.ownershipMode = OwnershipMode::Strict;
                    else if (value == "ultra")
                        settings.ownershipMode = OwnershipMode::UltraStrict;
                }
                else if (key == "gc_memory_limit" || key == "حد_ذاكرة")
                {
                    try
                    {
                        settings.gcMemoryLimitMB = std::stoul(value);
                    }
                    catch (const std::invalid_argument &)
                    {
                        // قيمة غير رقمية - استخدام القيمة الافتراضية
                        // settings.gcMemoryLimitMB يبقى بدون تغيير
                    }
                    catch (const std::out_of_range &)
                    {
                        // قيمة خارج النطاق - استخدام القيمة الافتراضية
                    }
                }
                else if (key == "suggestions" || key == "اقتراحات")
                {
                    settings.enableOwnershipSuggestions = (value == "true" || value == "1" || value == "نعم");
                }
                else if (key == "cycle_detection" || key == "كشف_دورات")
                {
                    settings.enableCycleDetection = (value == "true" || value == "1" || value == "نعم");
                }
                else if (key == "debug" || key == "تصحيح")
                {
                    settings.debugMode = (value == "true" || value == "1" || value == "نعم");
                }
                else if (key == "teacher" || key == "معلم")
                {
                    settings.teacherMode = (value == "true" || value == "1" || value == "نعم");
                }
            }

            return settings;
        }

    } // namespace Memory
} // namespace Sad
