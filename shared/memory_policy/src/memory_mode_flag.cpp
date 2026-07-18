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
#include "cli_flags_generated.h"
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


        // (AR) الاسم القانونيّ لعلمٍ من المصدر الوحيد — لا سلاسل حرّة في الكود.
        static const char *canonicalMemoryFlag(::sad::cli::FlagAction action)
        {
            for (std::size_t i = 0; i < ::sad::cli::kFlagCount; ++i)
            {
                if (::sad::cli::kFlags[i].action == action)
                {
                    return ::sad::cli::kFlags[i].canonical;
                }
            }
            return "";
        }

        void MemoryModeFlag::initializeFlags()
        {
            // ══════════════════════════════════════════════════════════════════════
            // (AR) تُبنى الأعلام كلّها من المصدر الوحيد (cli_flags_generated.h):
            //      عائلة `memory`. اسم عربيّ قانونيّ وحيد لكلّ مفهوم — لا مرادفات
            //      ولا اختصارات ولا توافق خلفيّ (أُلغيت 18 مرادفًا: --gc/--no-std/
            //      --freestanding/--kernel/--production/-p/--learn/-l/--auto/-a/…).
            //      الأسماء تأتي من الجدول؛ السلوك وحده يبقى هنا (switch على الإجراء).
            // (EN) All flags are built from the single source of truth (memory family).
            //      One canonical Arabic name per concept — no aliases, no back-compat.
            //      Names come from the table; only behavior lives here.
            // ══════════════════════════════════════════════════════════════════════
            using A = ::sad::cli::FlagAction;

            flagDefinitions_.clear();
            flagHandlers_.clear();

            for (std::size_t i = 0; i < ::sad::cli::kFlagCount; ++i)
            {
                const auto &spec = ::sad::cli::kFlags[i];
                if (!spec.for_memory)
                {
                    continue;
                }

                const bool hasValue = (spec.kind == ::sad::cli::FlagKind::Value);

                // (AR) التعريف (للمساعدة وللبحث عن قيمة في الوسيط التالي).
                //      لا اسم إنجليزيّ ولا مختصر: الاسم القانونيّ وحده.
                FlagDefinition def;
                def.longNameArabic = std::string(spec.canonical).substr(2); // بلا "--"
                def.longNameEnglish = def.longNameArabic;
                def.shortName = "";
                def.description = spec.desc_ar;
                def.hasValue = hasValue;
                def.defaultValue = spec.value_hint;
                flagDefinitions_.push_back(def);

                const A action = spec.action;
                flagHandlers_[spec.canonical] =
                    [this, action](MemoryModeSettings &s, const std::string &v)
                {
                    switch (action)
                    {
                    case A::Freestanding:
                        s = MemoryModeSettings::kernelDefaults();
                        break;
                    case A::Gc:
                        if (v.empty())
                        {
                            s = MemoryModeSettings::gcDefaults();
                        }
                        else
                        {
                            handleGCFlag(s, v);
                        }
                        break;
                    case A::Ownership:
                        handleOwnershipFlag(s, v);
                        break;
                    case A::ProductionMode:
                        s = MemoryModeSettings::productionDefaults();
                        break;
                    case A::LearningMode:
                        s = MemoryModeSettings::learningDefaults();
                        break;
                    case A::AutoMode:
                        s.mode = MemoryMode::Auto;
                        break;
                    case A::Suggestions:
                        s.enableOwnershipSuggestions = true;
                        break;
                    case A::DetectCycles:
                        s.enableCycleDetection = true;
                        break;
                    case A::MemoryLimit:
                        try
                        {
                            s.gcMemoryLimitMB = std::stoul(v);
                        }
                        catch (...)
                        {
                            // (AR) قيمة غير صالحة تُتجاهَل (سلوك موروث).
                        }
                        break;
                    case A::DebugMemory:
                        s.debugMode = true;
                        break;
                    default:
                        // (AR) علم عائلته memory بلا سلوك هنا ⇒ عيب في المصدر الوحيد.
                        break;
                    }
                };
            }
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
                    // (AR) القيمة تُمرَّر حصريًّا بصيغة «الاسم=قيمة» (كما في محلِّل
                    //      المترجم ووفق دلالة المصدر الوحيد). لا نبتلع الوسيط التالي:
                    //      الصيغة المجرّدة لعلمٍ قيميّ (مثل --جامع) لها معنًى قائم
                    //      بذاته (قيمة فارغة)، فابتلاع الوسيط التالي كان يلتهم اسم
                    //      الملفّ المصدر (انحدار). (EN) Values come only via name=value,
                    //      matching the compiler parser; never consume the next arg,
                    //      so bare value-flags keep their standalone meaning.
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
                if (arg == canonicalMemoryFlag(::sad::cli::FlagAction::Freestanding))
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
            // (AR) تُولَّد أعلام الذاكرة من المصدر الوحيد (عائلة memory): اسم عربيّ
            //      قانونيّ وحيد لكلّ مفهوم — بلا مرادفات ولا سلاسل حرّة.
            // (EN) Memory flags are generated from the single source of truth (memory
            //      family): one canonical Arabic name each — no aliases, no free strings.
            std::ostringstream help;
            help << "\n";
            for (std::size_t i = 0; i < ::sad::cli::kFlagCount; ++i)
            {
                const auto &spec = ::sad::cli::kFlags[i];
                if (!spec.for_memory)
                {
                    continue;
                }
                std::string name = spec.canonical;
                if (spec.kind == ::sad::cli::FlagKind::Value && spec.value_hint[0] != '\0')
                {
                    name += "=<";
                    name += spec.value_hint;
                    name += ">";
                }
                help << "    " << name << "\n"
                     << "        " << (arabic ? spec.desc_ar : spec.desc_en) << "\n";
            }
            help << "\n";
            return help.str();
        }

        std::string MemoryModeFlag::generateShortHelp(bool arabic) const
        {
            // (AR) أسماء قانونيّة من المصدر الوحيد. (EN) Canonical names from the SoT.
            const std::string gc = canonicalMemoryFlag(::sad::cli::FlagAction::Gc);
            const std::string prod = canonicalMemoryFlag(::sad::cli::FlagAction::ProductionMode);
            const std::string learn = canonicalMemoryFlag(::sad::cli::FlagAction::LearningMode);
            if (arabic)
            {
                return "الاستخدام: sad-build [" + gc + "|" + prod + "|" + learn + "] [خيارات] ملف.ص";
            }
            return "Usage: sad-build [" + gc + "|" + prod + "|" + learn + "] [options] file.ص";
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
