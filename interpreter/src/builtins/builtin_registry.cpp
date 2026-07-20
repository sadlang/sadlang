/**
 * @file builtin_registry.cpp
 * @brief (AR) نظام التسجيل الكسول — يسجل الوحدات الأساسية فقط عند البدء
 *        والوحدات الأخرى عند الاستيراد
 * @brief (EN) Lazy registration system — registers only core modules at startup,
 *        other modules on import
 *
 * @details
 * (AR) الوحدات المُضمّنة:
 *   - الأساسية (Core)       → Parts 1-2, 6, 8, 10 — تُسجَّل دائماً
 *   - نظام_منخفض (OS)       → Parts 19-22          — عند `استورد نظام_منخفض`
 *   - usb                   → Part 23               — عند `استورد usb`
 *   - محرك (Game Engine)    → Parts 29-34           — عند `استورد محرك`
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_module_registry.h"
#include "function_manager.h"
#include "ui/ui_eval_bridge.h" // (AR) م2-أ: مزوّدو وحدات خارجيّون؛ بادئة ui/ لتحلّ في sad_core وsad_interpreter

// (AR) السجل المركزي الموحّد — ADR-003
// (EN) Unified central registry — ADR-003
#include "builtin_registry.h"

#include <algorithm>

namespace Sad
{
    namespace Interpreter
    {

        // (AR) اختصارات لأسماء الدوال المركزية
        namespace Bs = Builtins::Names::Strings;
        namespace Ba = Builtins::Names::Arrays;
        namespace Bm = Builtins::Names::Math;
        namespace Bb = Builtins::Names::Basics;
        namespace Bss = Builtins::Names::Assertions;
        namespace Bmp = Builtins::Names::Maps;
        namespace Bsk = Builtins::Names::Sockets;
        namespace Bhc = Builtins::Names::HttpClient;
        namespace Bhs = Builtins::Names::HttpServer;
        namespace Bnu = Builtins::Names::NetworkUtils;
        // (AR) ثوابت أسماء الوحدات — مُشتقَّة من module_definitions.h
        // (EN) Module name constants — derived from module_definitions.h
        using Builtins::ModuleId;
        using Builtins::getCanonicalModuleName;
        static const std::string kModStrings    = std::string(Builtins::getCanonicalModuleName(ModuleId::STRINGS));
        static const std::string kModMath       = std::string(Builtins::getCanonicalModuleName(ModuleId::MATH));
        static const std::string kModBasics     = std::string(Builtins::getCanonicalModuleName(ModuleId::BASICS));
        static const std::string kModAssertions = std::string(Builtins::getCanonicalModuleName(ModuleId::ASSERTIONS));
        static const std::string kModCrypto     = std::string(Builtins::getCanonicalModuleName(ModuleId::CRYPTO));
        static const std::string kModMaps       = std::string(Builtins::getCanonicalModuleName(ModuleId::MAPS));
        static const std::string kModAsync      = std::string(Builtins::getCanonicalModuleName(ModuleId::ASYNC));
        static const std::string kModPlatform   = std::string(Builtins::getCanonicalModuleName(ModuleId::PLATFORM));
        static const std::string kModNetwork    = std::string(Builtins::getCanonicalModuleName(ModuleId::NETWORK));
        static const std::string kModSockets    = std::string(Builtins::getCanonicalModuleName(ModuleId::SOCKETS));

        // ═════════════════════════════════════════════════════════════════
        // (AR) إعلانات خارجية — ملفات التسجيل حسب المحتوى
        // (EN) Extern declarations — registration files by content
        // ═════════════════════════════════════════════════════════════════
        extern void registerBuiltinsStrings(Interpreter &interpreter);
        extern void registerBuiltinsBasics(Interpreter &interpreter);
        extern void registerBuiltinsMath(Interpreter &interpreter);
        extern void registerBuiltinsAssertions(Interpreter &interpreter);
        extern void registerBuiltinsCrypto(Interpreter &interpreter);
        extern void registerBuiltinsAsync(Interpreter &interpreter);
        extern void registerBuiltinsMaps(Interpreter &interpreter);
        extern void registerBuiltinsKernelCPU(Interpreter &interpreter);
        extern void registerBuiltinsKernelUEFI(Interpreter &interpreter);
        extern void registerBuiltinsKernelACPI(Interpreter &interpreter);
        extern void registerBuiltinsKernelGPU(Interpreter &interpreter);
        extern void registerBuiltinsKernelUSB(Interpreter &interpreter);
        extern void registerBuiltinsKernelStorage(Interpreter &interpreter);
        extern void registerBuiltinsKernelNetwork(Interpreter &interpreter);
        extern void registerBuiltinsKernelAudio(Interpreter &interpreter);
        extern void registerBuiltinsKernelTimers(Interpreter &interpreter);
        extern void registerBuiltinsKernelSerial(Interpreter &interpreter);
        extern void registerBuiltinsKernelMemory(Interpreter &interpreter);
        extern void registerBuiltinsKernelThreads(Interpreter &interpreter);
        extern void registerBuiltinsFFI(Interpreter &interpreter);
        extern void registerBuiltinsExceptions(Interpreter &interpreter);
        extern void registerBuiltinsPlatform(Interpreter &interpreter);
        extern void registerBuiltinsSockets(Interpreter &interpreter);
        extern void registerBuiltinsHttp(Interpreter &interpreter);
        extern void registerBuiltinsSadNet(Interpreter &interpreter);
        extern void registerBuiltinsWebSocket(Interpreter &interpreter);

        // ═════════════════════════════════════════════════════════════════
        // BuiltinModuleRegistry::loadModule — implemented here because it
        // needs FunctionManager (which requires interpreter_core.h)
        // ═════════════════════════════════════════════════════════════════
        bool BuiltinModuleRegistry::loadModule(const std::string &name)
        {
            std::string canonical = resolveAlias(name);
            if (loadedModules_.count(canonical) > 0)
                return true;

            auto it = modules_.find(canonical);
            if (it == modules_.end() || !interpreter_)
                return false;

            // (AR) تفعيل تتبع التسجيل لالتقاط جميع الدوال المسجلة فعلاً
            //      بدلاً من الاعتماد على فرق أسماء الدوال (diff) الذي يفشل
            //      عندما تكون الدالة مسجلة مسبقاً بواسطة StandardLibraryManager
            // (EN) Enable registration tracking to capture ALL actually registered
            //      functions, instead of relying on name diff which fails when
            //      functions are pre-registered by StandardLibraryManager
            auto &fm = interpreter_->getFunctionManager();
            fm.startRegistrationTracking();

            // (AR) استدعاء دالة التسجيل
            // (EN) Call the registration function
            it->second.registrar(*interpreter_);

            // (AR) الحصول على جميع الدوال المسجلة أثناء التتبع (بدون تكرار)
            // (EN) Get all functions registered during tracking (deduplicated)
            it->second.exportedFunctions = fm.stopRegistrationTracking();

            loadedModules_.insert(canonical);
            return true;
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) اقتراح الوحدة المناسبة لدالة غير مستوردة
        // (EN) Suggest the appropriate module for an unimported function
        // ═════════════════════════════════════════════════════════════════
        std::string BuiltinModuleRegistry::suggestModuleForFunction(const std::string &funcName) const
        {
            // (AR) خريطة ثابتة: اسم الدالة → اسم الوحدة — تُولَّد تلقائياً من ملفات التسجيل
            // (EN) Static map: function name → module name — auto-generated from registration files
            static const std::unordered_map<std::string, std::string> funcToModule = {
                // ─── نصوص ───
                // (AR) الأسماء الأساسية فقط — اسم واحد لكل دالة
                {"أضف", kModStrings},
                {"أزل", kModStrings},
                {std::string(Bs::STR_LENGTH), kModStrings},
                {std::string(Bs::TO_UPPER), kModStrings},
                {std::string(Bs::TO_LOWER), kModStrings},
                {std::string(Bs::SPLIT), kModStrings},
                {std::string(Bs::JOIN), kModStrings},
                {std::string(Bm::ABS), kModStrings},
                {std::string(Bm::MAX), kModStrings},
                {std::string(Bm::MIN), kModStrings},
                {"جمع", kModStrings},
                {std::string(Bm::SQRT), kModStrings},
                {"حول_رقم", kModStrings},
                {"لمصفوفة", kModStrings},
                {"هو_رقم_صحيح", kModStrings},
                {"هو_رقم_عشري", kModStrings},
                {"هو_نص", kModStrings},
                {"هو_مصفوفة", kModStrings},
                {std::string(Bs::FIND), kModStrings},
                {std::string(Bs::REPLACE), kModStrings},
                {std::string(Bs::SUBSTRING), kModStrings},
                {std::string(Bs::TRIM), kModStrings},
                {std::string(Bs::STARTS_WITH), kModStrings},
                {std::string(Bs::ENDS_WITH), kModStrings},
                {std::string(Bs::CONTAINS), kModStrings},
                {std::string(Ba::ADD), kModStrings},
                {std::string(Ba::REMOVE), kModStrings},
                {std::string(Ba::SIZE), kModStrings},
                {std::string(Ba::ARRAY_CONTAINS), kModStrings},
                {std::string(Ba::REVERSE), kModStrings},
                {std::string(Ba::SORT), kModStrings},
                {"أول", kModStrings},
                {"آخر", kModStrings},
                {std::string(Ba::SLICE), kModStrings},
                {std::string(Bm::POWER), kModStrings},
                {std::string(Bm::ROUND), kModStrings},
                {std::string(Bm::FLOOR), kModStrings},
                {std::string(Bm::CEIL), kModStrings},
                {std::string(Bm::SQUARE), kModStrings},

                // ─── أساسيات ───
                {std::string(Bb::EXIT), kModBasics},
                {std::string(Bb::ASSERT), kModBasics},
                {"اقرأ_أسطر", kModBasics},
                {"أضف_إلى_ملف", kModBasics},
                {"انسخ_ملف", kModBasics},
                {"انقل_ملف", kModBasics},
                {"احذف_ملف", kModBasics},
                {std::string(Bb::MKDIR), kModBasics},
                {"اسرد_مجلد", kModBasics},
                {"احذف_مجلد", kModBasics},
                {std::string(Bb::IS_FILE), kModBasics},
                {std::string(Bb::IS_DIR), kModBasics},
                {std::string(Bb::RANGE), kModBasics},
                {std::string(Bb::READ_FILE), kModBasics},
                {std::string(Bb::WRITE_FILE), kModBasics},
                {std::string(Bb::FILE_EXISTS), kModBasics},

                // ─── رياضيات ───
                // (AR) الأسماء الأساسية فقط — اسم واحد لكل دالة
                {std::string(Bm::LOG), kModMath},
                {std::string(Bm::LOG10), kModMath},
                {std::string(Bm::LOG2), kModMath},
                {std::string(Bm::EXP), kModMath},
                {std::string(Bm::CLAMP), kModMath},
                {std::string(Bm::TRUNCATE), kModMath},
                {std::string(Bm::FMOD), kModMath},
                {std::string(Bm::SIGN), kModMath},
                {std::string(Bm::PI), kModMath},
                {std::string(Bm::E), kModMath},
                {std::string(Bm::LERP), kModMath},
                {std::string(Bm::RANDOM_RANGE), kModMath},
                {std::string(Bm::RANDOM_FLOAT), kModMath},
                {"احصل_بيئة", kModMath},
                {"عيّن_بيئة", kModMath},
                {"مجلد_حالي", kModMath},
                {"غيّر_مجلد", kModMath},
                {"نفّذ", kModMath},
                {kModPlatform, kModMath},
                {"نظام", kModMath},
                {"طابع_زمن", kModMath},
                {"ساعة", kModMath},
                {"منفذ_اكتب", kModMath},
                {"منفذ_اقرأ", kModMath},
                {"ذاكرة_اكتب", kModMath},
                {"ذاكرة_اقرأ", kModMath},
                {"مقاطعة", kModMath},
                {"توقف", kModMath},
                {"تعطيل_مقاطعات", kModMath},
                {"تفعيل_مقاطعات", kModMath},
                {"شاشة_اكتب", kModMath},
                {"شاشة_امسح", kModMath},
                {"انسخ_ذاكرة", kModMath},
                {"املأ_ذاكرة", kModMath},
                {"تسلسل_هئ", kModMath},
                {"تسلسل_ارسل", kModMath},
                {"تسلسل_استقبل", kModMath},
                {"تسلسل_جاهز", kModMath},
                {"منفذ_رقمي_اكتب", kModMath},
                {"منفذ_رقمي_اقرأ", kModMath},
                {"حدد_وضع_منفذ", kModMath},
                {"مؤقت_هئ", kModMath},
                {"مؤقت_قراءة", kModMath},
                {"مؤقت_انتظر", kModMath},
                {"اعد_تشغيل", kModMath},
                {"معرف_المعالج", kModMath},
                {"عداد_الدورات", kModMath},
                {"حاجز_ذاكرة", kModMath},
                {"حاجز_قراءة", kModMath},
                {"حاجز_كتابة", kModMath},
                {"نقل_مباشر_هئ", kModMath},
                {"نقل_مباشر_ابدأ", kModMath},

                // ─── تأكيدات ───
                {"آمن", kModAssertions},
                {"ذعر", kModAssertions},
                {std::string(Bss::ASSERT_EQ), kModAssertions},
                {std::string(Bss::ASSERT_NEQ), kModAssertions},
                {std::string(Bss::ASSERT_TRUE), kModAssertions},
                {std::string(Bss::ASSERT_FALSE), kModAssertions},
                {"تأكد_لاشيء", kModAssertions},
                {"تأكد_ليس_لاشيء", kModAssertions},
                {"تأكد_أصغر", kModAssertions},
                {std::string(Bss::HASH), kModAssertions},
                {"شفّر", kModAssertions},
                {"فك_تشفير", kModAssertions},
                {"تأكد_نوع", kModAssertions},
                {"تأكد_أكبر", kModAssertions},
                {"نظّف", kModAssertions},
                {"عشوائي_آمن", kModAssertions},
                {"ترميز_64", kModAssertions},
                {"بلايك3", kModCrypto},
                {"هاش_مفتاح", kModCrypto},
                {"اشتق_مفتاح_مرور", kModCrypto},
                {"اشتق_مفتاح", kModCrypto},
                {"طباعة_تنسيق", kModAssertions},
                {"حجز", kModAssertions},
                {"حرر", kModAssertions},
                {"اعد_حجز", kModAssertions},
                {"حجز_صفري", kModAssertions},
                {"طول_نص_س", kModAssertions},
                {"انسخ_نص_س", kModAssertions},
                {"قارن_نص_س", kModAssertions},
                {"الحق_نص_س", kModAssertions},
                {"افتح_ملف_س", kModAssertions},
                {"اغلق_ملف_س", kModAssertions},
                {"اكتب_ملف_س", kModAssertions},
                {"اقرأ_ملف_س", kModAssertions},
                {"نفذ_امر", kModAssertions},
                {"قيمة_بيئة", kModAssertions},
                {"نص_لعدد", kModAssertions},
                {"نص_لعشري", kModAssertions},
                {"تنسيق_نص", kModAssertions},

                // ─── خرائط ───
                {"خريطة", kModMaps},
                {"خريطة_احصل", kModMaps},
                {"خريطة_عيّن", kModMaps},
                {"خريطة_مفاتيح", kModMaps},
                {"خريطة_قيم", kModMaps},
                {"خريطة_تحتوي", kModMaps},
                {"خريطة_احذف", kModMaps},
                {"خريطة_حجم", kModMaps},
                {"خريطة_عناصر", kModMaps},
                {"خريطة_دمج", kModMaps},
                {"تخطيط", kModMaps},
                {"تصفية", kModMaps},
                {"اختزال", kModMaps},
                {"لكل_عنصر", kModMaps},
                {"تخطيط_مسطح", kModMaps},
                {"ضم", kModMaps},
                {"أي_عنصر", kModMaps},
                {"كل_العناصر", kModMaps},
                {std::string(Bmp::NOW), kModMaps},
                {"الآن_مللي", kModMaps},
                {"صيغة_تاريخ", kModMaps},
                {"مكونات_تاريخ", kModMaps},
                {"سنة", kModMaps},
                {"شهر", kModMaps},
                {"يوم", kModMaps},
                {"ساعة", kModMaps},
                {"دقيقة", kModMaps},
                {"ثانية", kModMaps},
                {"يوم_الأسبوع", kModMaps},
                {"فرق_وقت", kModMaps},
                {std::string(Bmp::REGEX), kModMaps},
                {"تعبير_بحث", kModMaps},
                {"تعبير_استبدال", kModMaps},
                {"تعبير_جد_الكل", kModMaps},
                // (AR) دوال خرائط خاصة — نصوص ومسارات وتشفير
                {"تكرار_نص", kModMaps},
                {"حشو_بداية", kModMaps},
                {"حشو_نهاية", kModMaps},
                {"عكس_نص", kModMaps},
                {"رمز_حرف", kModMaps},
                {"حرف_من_رمز", kModMaps},
                {"عدّ", kModMaps},
                {"تنسيق", kModMaps},
                {"حجم_ملف", kModMaps},
                {"ضم_مسار", kModMaps},
                {"مجلد_مسار", kModMaps},
                {"اسم_ملف", kModMaps},
                {"امتداد_ملف", kModMaps},
                {"ترميز_قاعدة64", kModMaps},
                {"فك_قاعدة64", kModMaps},
                {"ترميز_ست_عشري", kModMaps},
                {"ترميز_عنوان", kModMaps},
                {"فك_عنوان", kModMaps},
                {"متغير_بيئة", kModMaps},
                {"المجلد_الحالي", kModMaps},
                {"خطأ", kModMaps},
                {"فريد", kModMaps},
                {"اتحاد", kModMaps},
                {"تقاطع", kModMaps},
                {"فرق", kModMaps},
                {std::string(Bmp::JSON_PARSE), kModMaps},
                {std::string(Bmp::JSON_STRINGIFY), kModMaps},
                {"json_منسق", kModMaps},
                {"حلل_xml", kModMaps},
                {"حول_لـxml", kModMaps},
                {"xml_منسق", kModMaps},
                {"تسطيح", kModMaps},
                {"تقطيع", kModMaps},
                {"خذ", kModMaps},
                {"اترك", kModMaps},
                {"رقّم", kModMaps},
                {"متوسط", kModMaps},
                {"أصغر_المصفوفة", kModMaps},
                {"أكبر_المصفوفة", kModMaps},
                {"ترتيب_مصفوفة", kModMaps},
                {"عكس_مصفوفة", kModMaps},
                {"ربط", kModMaps},
                {"ازل_تشكيل", kModMaps},
                {"مقارنة_نص", kModMaps},
                {"نص_يونيكود", kModMaps},
                {"مصفوفة_جديدة", kModMaps},
                {"مصفوفة_وحدة", kModMaps},
                {"نطاق_مصفوفة", kModMaps},
                {"أبعاد_مصفوفة", kModMaps},
                {"قلب_محوري", kModMaps},

                // ─── تزامن_متقدم ───
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x85\\xd9\\x87\\xd9\\x85\\xd8\\xa9", kModAsync},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd9\\x85\\xd9\\x87\\xd9\\x85\\xd8\\xa9", kModAsync},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1", kModAsync},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xaa\\xd8\\xac", kModAsync},
                {"\\xd9\\x86\\xd9\\x88\\xd9\\x85_\\xd8\\xba\\xd9\\x8a\\xd8\\xb1_\\xd9\\x85\\xd8\\xaa\\xd8\\xb2\\xd8\\xa7\\xd9\\x85\\xd9\\x86", kModAsync},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", kModAsync},
                {"\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", kModAsync},
                {"\\xd8\\xa3\\xd9\\x88\\xd9\\x81_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", kModAsync},
                {"\\xd8\\xad\\xd9\\x82\\xd9\\x82_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", kModAsync},
                {"\\xd8\\xa7\\xd8\\xad\\xd8\\xb5\\xd9\\x84_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", kModAsync},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", kModAsync},
                {"\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", kModAsync},
                {"\\xd8\\xa3\\xd8\\xb1\\xd8\\xb3\\xd9\\x84_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", kModAsync},
                {"\\xd8\\xa3\\xd8\\xb1\\xd8\\xb3\\xd9\\x84", kModAsync},
                {"\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", kModAsync},
                {"\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", kModAsync},
                {"\\xd8\\xa3\\xd8\\xba\\xd9\\x84\\xd9\\x82_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", kModAsync},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x82\\xd9\\x81\\xd9\\x84", kModAsync},
                {"\\xd9\\x82\\xd9\\x81\\xd9\\x84", kModAsync},
                {"\\xd8\\xa7\\xd9\\x82\\xd9\\x81\\xd9\\x84", kModAsync},
                {"\\xd8\\xa7\\xd9\\x81\\xd8\\xaa\\xd8\\xad_\\xd9\\x82\\xd9\\x81\\xd9\\x84", kModAsync},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", kModAsync},
                {"\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", kModAsync},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xb6\\xd9\\x85_\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", kModAsync},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", kModAsync},
                {"\\xd8\\xad\\xd9\\x85\\xd9\\x84_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", kModAsync},
                {"\\xd8\\xae\\xd8\\xb2\\xd9\\x86_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", kModAsync},
                {"\\xd8\\xa3\\xd8\\xb6\\xd9\\x81_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", kModAsync},
                {"\\xd9\\x82\\xd8\\xa7\\xd8\\xb1\\xd9\\x86_\\xd9\\x88\\xd8\\xa8\\xd8\\xaf\\xd9\\x84", kModAsync},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd9\\x84", kModAsync},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd8\\xa3\\xd9\\x8a", kModAsync},
                {"\\xd8\\xa7\\xd8\\xae\\xd8\\xaa\\xd8\\xb1_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", kModAsync},
                {"\\xd9\\x87\\xd9\\x88_\\xd9\\x85\\xd8\\xab\\xd9\\x8a\\xd9\\x84", kModAsync},
                {"\\xd9\\x85\\xd8\\xab\\xd9\\x8a\\xd9\\x84_\\xd9\\x85\\xd9\\x86", kModAsync},
                {"\\xd9\\x86\\xd9\\x88\\xd8\\xb9_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd8\\xa7\\xd8\\xa6\\xd9\\x86", kModAsync},
                {"\\xd9\\x86\\xd9\\x88\\xd8\\xb9", kModAsync},
                {"\\xd8\\xad\\xd9\\x82\\xd9\\x88\\xd9\\x84_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd8\\xa7\\xd8\\xa6\\xd9\\x86", kModAsync},

                // ─── منصة ───
                {"\\xd8\\xa7\\xd9\\x84\\xd9\\x85\\xd9\\x86\\xd8\\xb5\\xd8\\xa9", kModPlatform},
                {"\\xd9\\x87\\xd9\\x84_\\xd8\\xb3\\xd8\\xb7\\xd8\\xad_\\xd8\\xa7\\xd9\\x84\\xd9\\x85\\xd9\\x83\\xd8\\xaa\\xd8\\xa8", kModPlatform},
                {"\\xd9\\x87\\xd9\\x84_\\xd8\\xa7\\xd9\\x86\\xd8\\xaf\\xd8\\xb1\\xd9\\x88\\xd9\\x8a\\xd8\\xaf", kModPlatform},
                {"\\xd9\\x87\\xd9\\x84_\\xd9\\x88\\xd9\\x8a\\xd8\\xa8", kModPlatform},

                // ─── مقابس (أسماء عربية دلالية) ───
                {std::string(Bsk::TCP_NEW), kModSockets},
                {std::string(Bsk::TCP_CONNECT), kModSockets},
                {std::string(Bsk::TCP_SEND), kModSockets},
                {std::string(Bsk::TCP_RECV), kModSockets},
                {std::string(Bsk::TCP_BIND), kModSockets},
                {std::string(Bsk::TCP_LISTEN), kModSockets},
                {std::string(Bsk::TCP_ACCEPT), kModSockets},
                {std::string(Bsk::TCP_CLOSE), kModSockets},
                {std::string(Bsk::TCP_CONNECTED), kModSockets},
                {std::string(Bsk::TCP_REMOTE_ADDR), kModSockets},
                {std::string(Bsk::UDP_NEW), kModSockets},
                {std::string(Bsk::UDP_BIND), kModSockets},
                {std::string(Bsk::UDP_SEND), kModSockets},
                {std::string(Bsk::UDP_RECV), kModSockets},
                {std::string(Bsk::UDP_CLOSE), kModSockets},
                {std::string(Bsk::RECV_TIMEOUT), kModSockets},
                {std::string(Bsk::SEND_TIMEOUT), kModSockets},
                {std::string(Bsk::NO_DELAY), kModSockets},
                {std::string(Bsk::LOCAL_PORT), kModSockets},
                // ─── عميل HTTP ───
                {std::string(Bhc::NEW_CLIENT), kModNetwork},
                {std::string(Bhc::FREE_CLIENT), kModNetwork},
                {std::string(Bhc::GET), kModNetwork},
                {std::string(Bhc::POST), kModNetwork},
                {std::string(Bhc::PUT), kModNetwork},
                {std::string(Bhc::DELETE_REQ), kModNetwork},
                {std::string(Bhc::PATCH), kModNetwork},
                {std::string(Bhc::SET_BASE_URL), kModNetwork},
                {std::string(Bhc::SET_HEADER), kModNetwork},
                {std::string(Bhc::SET_TIMEOUT), kModNetwork},
                {std::string(Bhc::SET_BEARER), kModNetwork},
                {std::string(Bhc::LAST_ERROR), kModNetwork},
                {std::string(Bhc::IS_OK), kModNetwork},
                {std::string(Bhc::RESP_FREE), kModNetwork},
                {std::string(Bhc::RESP_STATUS), kModNetwork},
                {std::string(Bhc::RESP_BODY), kModNetwork},
                {std::string(Bhc::RESP_HEADER), kModNetwork},
                {std::string(Bhc::RESP_SUCCESS), kModNetwork},
                // ─── خادم HTTP ───
                {std::string(Bhs::NEW_SERVER), kModNetwork},
                {std::string(Bhs::FREE_SERVER), kModNetwork},
                {std::string(Bhs::ON_GET), kModNetwork},
                {std::string(Bhs::ON_POST), kModNetwork},
                {std::string(Bhs::ON_PUT), kModNetwork},
                {std::string(Bhs::ON_DELETE), kModNetwork},
                {std::string(Bhs::LISTEN), kModNetwork},
                {std::string(Bhs::STOP), kModNetwork},
                {std::string(Bhs::ENABLE_CORS), kModNetwork},
                {std::string(Bhs::REQ_METHOD), kModNetwork},
                {std::string(Bhs::REQ_PATH), kModNetwork},
                {std::string(Bhs::REQ_BODY), kModNetwork},
                {std::string(Bhs::REQ_HEADER), kModNetwork},
                {std::string(Bhs::REQ_QUERY), kModNetwork},
                {std::string(Bhs::RESP_SET_STATUS), kModNetwork},
                {std::string(Bhs::RESP_SET_BODY), kModNetwork},
                {std::string(Bhs::RESP_SET_JSON), kModNetwork},
                {std::string(Bhs::RESP_SET_HTML), kModNetwork},
                {std::string(Bhs::RESP_SET_HEADER), kModNetwork},
                // ─── أدوات الشبكة ───
                {std::string(Bnu::INIT), kModNetwork},
                {std::string(Bnu::CLEANUP), kModNetwork},
                {std::string(Bnu::LAST_ERROR_CODE), kModNetwork},
                {std::string(Bnu::LAST_ERROR_MSG), kModNetwork},
                {std::string(Bnu::ADDR_NEW), kModNetwork},
                {std::string(Bnu::ADDR_NEW_V6), kModNetwork},
                {std::string(Bnu::ADDR_FREE), kModNetwork},
                {std::string(Bnu::ADDR_IP), kModNetwork},
                {std::string(Bnu::ADDR_PORT), kModNetwork},
                {std::string(Bnu::ADDR_IS_V4), kModNetwork},
                {std::string(Bnu::ADDR_IS_V6), kModNetwork},
                {std::string(Bnu::ADDR_STR), kModNetwork},
            };

            auto it = funcToModule.find(funcName);
            if (it != funcToModule.end())
            {
                return it->second;
            }

            // (AR) فحص السجل المركزي الموحّد — ADR-003
            //      يدعم الدوال المعرّفة في builtin_registry.h (60+ دالة)
            //      ويدعم البحث بالأسماء البديلة (الإنجليزية والعربية)
            // (EN) Check unified central registry — ADR-003
            //      Supports functions defined in builtin_registry.h (60+)
            //      and supports alias lookup (English and Arabic)
            auto suggestion = Sad::Builtins::suggestModuleForFunction(
                std::string_view(funcName));
            if (!suggestion.empty())
            {
                return std::string(suggestion);
            }

            return "";
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) دالة حرة: اقتراح الوحدة لدالة — يستدعيها expression_evaluator_calls
        // (EN) Free function: suggest module for function — called by expression_evaluator_calls
        // ═════════════════════════════════════════════════════════════════
        std::string suggestModuleForUnimportedFunction(const std::string &funcName)
        {
            return BuiltinModuleRegistry::getInstance().suggestModuleForFunction(funcName);
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) تسجيل الوحدات المُضمّنة في السجل المركزي
        // (EN) Register builtin modules in the central registry
        // ═════════════════════════════════════════════════════════════════
        static void registerBuiltinModules()
        {
            auto &registry = BuiltinModuleRegistry::getInstance();

            // ═══════════════════════════════════════════════════════════════
            // (AR) الوحدات الأساسية — متاحة دائماً، تُحمَّل كسولاً في الوضع الصارم
            // (EN) Core modules — always available, lazy-loaded in strict mode
            // ═══════════════════════════════════════════════════════════════

            // ─── نصوص / Strings (Part 1) — string ops, basic math, type checks ───
            registry.registerModule({"\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5", // نصوص

                                     "\xd8\xaf\xd9\x88\xd8\xa7\xd9\x84 \xd8\xa7\xd9\x84\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5 \xd9\x88\xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa7\xd8\xb3\xd9\x84",
                                     // دوال النصوص والسلاسل
                                     [](Interpreter &interp)
                                     { registerBuiltinsStrings(interp); },
                                     {}});

            // ─── أساسيات / Basics (Part 2) — exit, assert, range, file I/O basics ───
            registry.registerModule({"\xd8\xa3\xd8\xb3\xd8\xa7\xd8\xb3\xd9\x8a\xd8\xa7\xd8\xaa", // أساسيات

                                     "\xd8\xae\xd8\xb1\xd9\x88\xd8\xac\xd8\x8c \xd9\x85\xd8\xaf\xd9\x89\xd8\x8c \xd8\xaa\xd8\xa3\xd9\x83\xd8\xaf\xd8\x8c \xd9\x85\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa",
                                     // خروج، مدى، تأكد، ملفات
                                     [](Interpreter &interp)
                                     { registerBuiltinsBasics(interp); },
                                     {}});

            // ─── رياضيات / Math (Part 6) — log, clamp, embedded IO ───
            registry.registerModule({"\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa7\xd8\xaa", // رياضيات
                                     "\xd9\x84\xd9\x88\xd8\xba\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xaa\xd9\x85\xd8\x8c \xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd8\xaf\xd8\x8c \xd8\xaf\xd9\x88\xd8\xa7\xd9\x84 \xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa9",
                                     // لوغاريتم، تقييد، دوال رياضية
                                     [](Interpreter &interp)
                                     { registerBuiltinsMath(interp); },
                                     {}});

            // ─── تأكيدات / Assertions (Part 7) — assert, hash, crypto, FFI ───
            registry.registerModule({"\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf\xd8\xa7\xd8\xaa", // تأكيدات
                                     "\xd8\xaa\xd8\xa3\xd9\x83\xd8\xaf\xd8\x8c \xd9\x87\xd8\xa7\xd8\xb4\xd8\x8c \xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1\xd8\x8c FFI",
                                     // تأكد، هاش، تشفير، FFI
                                     [](Interpreter &interp)
                                     { registerBuiltinsAssertions(interp); },
                                     {}});

            // ─── تزامن / Async (Part 8) — spawn, await, channels, threads, atomics ───
            registry.registerModule({"\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // تزامن_متقدم

                                     "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7\xd8\x8c \xd9\x82\xd9\x86\xd9\x88\xd8\xa7\xd8\xaa\xd8\x8c \xd8\xb0\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xaa\xd8\x8c OOP",
                                     // خيوط، قنوات، ذريات، OOP
                                     [](Interpreter &interp)
                                     { registerBuiltinsAsync(interp); },
                                     {}});

            // ─── خرائط / Maps & Advanced (Part 10) — maps, functional, date, regex, JSON ───
            registry.registerModule({"\xd8\xae\xd8\xb1\xd8\xa7\xd8\xa6\xd8\xb7", // خرائط

                                     "\xd8\xae\xd8\xb1\xd8\xa7\xd8\xa6\xd8\xb7\xd8\x8c JSON\xd8\x8c \xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae\xd8\x8c \xd8\xaa\xd8\xb9\xd8\xa8\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa \xd9\x86\xd9\x85\xd8\xb7\xd9\x8a\xd8\xa9",
                                     // خرائط، JSON، تاريخ، تعبيرات نمطية
                                     [](Interpreter &interp)
                                     { registerBuiltinsMaps(interp); },
                                     {}});

            // ─── منصة / Platform (Part 39) — platform detection ───
            registry.registerModule({"\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", // منصة

                                     "\xd9\x83\xd8\xb4\xd9\x81 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9 \xd9\x88\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84",
                                     // كشف المنصة ونظام التشغيل
                                     [](Interpreter &interp)
                                     { registerBuiltinsPlatform(interp); },
                                     {}});

            // ─── مقابس / Sockets (Part 40) — TCP, UDP ───
            registry.registerModule({kModSockets,
                                     "وصلات TCP، مراسيل UDP، إعدادات الشبكة",
                                     [](Interpreter &interp)
                                     { registerBuiltinsSockets(interp); },
                                     {}});

            // ─── شبكة / Network HTTP (Part 40b) — HTTP client, server, utilities ───
            registry.registerModule({kModNetwork,
                                     "عميل HTTP، خادم HTTP، عناوين، أدوات شبكة",
                                     [](Interpreter &interp)
                                     { registerBuiltinsHttp(interp); },
                                     {}});

            // ─── ويبسوكت / WebSocket (Part 40c) — bidirectional WS client + server ───
            registry.registerModule({"ويبسوكت",
                                     "عميل وخادم WebSocket، بث، غرف",
                                     [](Interpreter &interp)
                                     { registerBuiltinsWebSocket(interp); },
                                     {}});

            // ─── شبكة_لامركزية / SadNet Decentralized Network (Part 41) ───
            registry.registerModule({"\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x84\xd8\xa7\xd9\x85\xd8\xb1\xd9\x83\xd8\xb2\xd9\x8a\xd8\xa9", // شبكة_لامركزية
                                     "DHT\xd8\x8c E2EE\xd8\x8c \xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86 \xd9\x85\xd9\x88\xd8\xb2\xd8\xb9\xd8\x8c CDN\xd8\x8c DNS\xd8\x8c \xd8\xa8\xd8\xab",
                                     // DHT، E2EE، تخزين موزع، CDN، DNS، بث
                                     [](Interpreter &interp)
                                     { registerBuiltinsSadNet(interp); },
                                     {}});

            // ═══════════════════════════════════════════════════════════════
            // (AR) وحدات النظام المنخفض — تُحمَّل عند الاستيراد فقط
            // (EN) Low-level system modules — loaded on import only
            // ═══════════════════════════════════════════════════════════════

            // ─── نظام_منخفض / Low-Level OS (Parts 19, 20, 21, 22) ───
            registry.registerModule({"\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85_\xd9\x85\xd9\x86\xd8\xae\xd9\x81\xd8\xb6", // نظام_منخفض
                                     "CPU\xd8\x8c APIC\xd8\x8c UEFI\xd8\x8c ACPI\xd8\x8c GPU \xd8\xa8\xd8\xaf\xd9\x88\xd9\x86 \xd9\x86\xd8\xb8\xd8\xa7\xd9\x85",
                                     // CPU، APIC، UEFI، ACPI، GPU بدون نظام
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelCPU(interp);
                                         registerBuiltinsKernelUEFI(interp);
                                         registerBuiltinsKernelACPI(interp);
                                         registerBuiltinsKernelGPU(interp);
                                     },
                                     {}});

            // ─── يو_إس_بي ───
            registry.registerModule({"\xd9\x8a\xd9\x88_\xd8\xa5\xd8\xb3_\xd8\xa8\xd9\x8a", // يو_إس_بي
                                     "USB/HID\xd8\x8c \xd9\x84\xd9\x88\xd8\xad\xd8\xa9 \xd9\x85\xd9\x81\xd8\xa7\xd8\xaa\xd9\x8a\xd8\xad\xd8\x8c \xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9",
                                     // USB/HID، لوحة مفاتيح، فأرة
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelUSB(interp);
                                     },
                                     {}});

            // ─── تخزين / Storage (Part 24) — NVMe, AHCI, S.M.A.R.T ───
            registry.registerModule({"\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // تخزين
                                     "NVMe\xd8\x8c AHCI\xd8\x8c S.M.A.R.T\xd8\x8c \xd8\xaa\xd9\x82\xd8\xb3\xd9\x8a\xd9\x85\xd8\xa7\xd8\xaa",
                                     // NVMe، AHCI، S.M.A.R.T، تقسيمات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelStorage(interp);
                                     },
                                     {}});

            // ─── شبكة_منخفضة / Low-Level Network Stack (Part 25) — Ethernet, ARP, IP, UDP ───
            registry.registerModule({"شبكة_منخفضة", // شبكة_منخفضة
                                     "Ethernet\xd8\x8c ARP\xd8\x8c IPv4\xd8\x8c ICMP\xd8\x8c UDP",
                                     // Ethernet، ARP، IPv4، ICMP، UDP
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelNetwork(interp);
                                     },
                                     {}});

            // ─── صوت / Audio (Part 26) — AC'97, HDA, PC Speaker ───
            registry.registerModule({"\xd8\xb5\xd9\x88\xd8\xaa", // صوت
                                     "AC'97\xd8\x8c HDA\xd8\x8c PC Speaker\xd8\x8c \xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84",
                                     // AC'97، HDA، PC Speaker، تسجيل
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelAudio(interp);
                                     },
                                     {}});

            // ─── مؤقتات / Timers (Part 27) — PIT, RTC, Stopwatch ───
            registry.registerModule({"\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // مؤقتات
                                     "PIT 8254\xd8\x8c RTC\xd8\x8c \xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9 \xd8\xa5\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81",
                                     // PIT 8254، RTC، ساعة إيقاف
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelTimers(interp);
                                     },
                                     {}});

            // ─── تسلسلي / Serial UART (Part 28) — COM1-COM4 ───
            registry.registerModule({"\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a", // تسلسلي
                                     "UART 16550\xd8\x8c COM1-COM4\xd8\x8c loopback",
                                     // UART 16550، COM1-COM4، loopback
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelSerial(interp);
                                     },
                                     {}});

            // ─── ذاكرة / Memory (Part 35) — Allocator, GC ───
            registry.registerModule({"\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", // ذاكرة
                                     "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5 \xd8\xa7\xd9\x84\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9\xd8\x8c \xd8\xac\xd8\xa7\xd9\x85\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x82\xd9\x85\xd8\xa7\xd9\x85\xd8\xa9\xd8\x8c \xd8\xa5\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa",
                                     // مخصص الذاكرة، جامع القمامة، إحصائيات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelMemory(interp);
                                     },
                                     {}});

            // ─── خيوط / Threads (Part 36) — Thread, Mutex, Semaphore ───
            registry.registerModule({"\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7", // خيوط
                                     "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7\xd8\x8c \xd8\xa3\xd9\x82\xd9\x81\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa \xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86",
                                     // خيوط، أقفال، إشارات تزامن
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelThreads(interp);
                                     },
                                     {}});

            // ─── واجهة_خارجية / FFI (Part 37) — Dynamic library loading ───
            registry.registerModule({"\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9_\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a\xd8\xa9", // واجهة_خارجية
                                     "FFI\xd8\x8c \xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84 \xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xaa C\xd8\x8c \xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1 \xd8\xaf\xd9\x88\xd8\xa7\xd9\x84",
                                     // FFI، تحميل مكتبات C، استدعاء دوال
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsFFI(interp);
                                     },
                                     {}});

            // ─── استثناءات / Exceptions & Sandbox (Part 38) ───
            registry.registerModule({"\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa", // استثناءات
                                     "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa\xd8\x8c \xd8\xaa\xd8\xaa\xd8\xa8\xd8\xb9 \xd9\x85\xd9\x83\xd8\xaf\xd8\xb3\xd8\x8c \xd8\xa3\xd8\xb0\xd9\x88\xd9\x86\xd8\xa7\xd8\xaa",
                                     // استثناءات، تتبع مكدس، أذونات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsExceptions(interp);
                                     },
                                     {}});

            // ─── مقابس / Sockets (Part 40) — TCP & UDP عالي المستوى ───
            registry.registerModule({"\xd9\x85\xd9\x82\xd8\xa7\xd8\xa8\xd8\xb3", // مقابس
                                     "TCP\xd8\x8c UDP\xd8\x8c \xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa5\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84",
                                     // TCP، UDP، اتصال، إرسال، استقبال
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsSockets(interp); // TCP & UDP high-level sockets
                                     },
                                     {}});

            // ─── تشفير / Crypto (Part 7 crypto + Part 41 BLAKE3) — SHA-256, تشفير/فك_تشفير، بلايك3 ───
            registry.registerModule({"\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // تشفير
                                     "SHA-256\xd8\x8c \xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1 CTR\xd8\x8c \xd9\x87\xd8\xa7\xd8\xb4\xd8\x8c BLAKE3",
                                     // SHA-256، تشفير CTR، هاش، BLAKE3
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsAssertions(interp); // SHA-256, encrypt/decrypt, hash
                                         registerBuiltinsCrypto(interp);     // BLAKE3, keyed hash
                                     },
                                     {}});

            // ═══════════════════════════════════════════════════════════════
            // (AR) م2-أ (sadlang-rfcs#10): وحدات الواجهات (رسومات/_صوت/...) لم تَعُد
            //      مُسجَّلة هنا؛ يسجّلها sad_graphics_bridge عبر مزوّد خارجيّ. القلب لا يعرف
            //      sad_ui. يثبّت sad-run المزوّد قبل إنشاء المفسّر.
            // (EN) Phase 2-A: UI modules are no longer registered here; sad_graphics_bridge
            //      registers them via an external provider. The core does not know sad_ui.
            // ═══════════════════════════════════════════════════════════════
            for (auto &provider : externalModuleProviders())
                provider();
        }

        // ═════════════════════════════════════════════════════════════════
        // (AR) تعريف دالة التسجيل المصغّر
        // (EN) Forward declaration for core-only registration
        // ═════════════════════════════════════════════════════════════════
        extern void registerCoreIOBuiltins(Interpreter &interpreter);

        // ═════════════════════════════════════════════════════════════════
        // (AR) نقطة الدخول الرئيسية: تسجيل الدوال الأساسية + تهيئة سجل الوحدات
        // (EN) Main entry point: register core functions + initialize module registry
        // ═════════════════════════════════════════════════════════════════
        void registerBuiltinFunctions(Interpreter &interpreter)
        {
            // ═══ (AR) الأساسية — I/O وتحويل الأنواع فقط ═══
            // ═══ (EN) Core — I/O and type conversion only ═══
            registerCoreIOBuiltins(interpreter); // اطبع، اقرأ، طول، نوع، نص، رقم، عشري

            // ═══════════════════════════════════════════════════════════════
            // (AR) جميع الوحدات الأخرى تُحمَّل عند الاستيراد فقط
            // (EN) All other modules are loaded on import only
            //
            // الوحدات المتاحة للاستيراد:
            //   استورد نصوص         — دوال النصوص والسلاسل (Part 1)
            //   استورد أساسيات       — خروج، مدى، تأكد، ملفات (Part 2)
            //   استورد رياضيات       — دوال رياضية (Part 6)
            //   استورد تأكيدات       — تأكد، هاش، تشفير، FFI (Part 7)
            //   استورد تزامن_متقدم   — خيوط، قنوات، ذريات (Part 8)
            //   استورد خرائط         — خرائط، JSON، تاريخ، regex (Part 10)
            //   استورد منصة          — كشف المنصة (Part 39)
            //   استورد مقابس          — وصلات TCP، مراسيل UDP (Part 40)
            //   استورد شبكة           — عميل HTTP، خادم HTTP، أدوات شبكة (Part 40b)
            // ═══════════════════════════════════════════════════════════════
            auto &registry = BuiltinModuleRegistry::getInstance();
            registry.setInterpreter(&interpreter);
            registerBuiltinModules();
        }

    } // namespace Interpreter
} // namespace Sad
