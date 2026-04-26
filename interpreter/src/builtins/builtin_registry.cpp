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
#include "../ui/ui_builtins.h"
#include "../ui/ui_module_builtins.h"
#include "../ui/widget_builtins.h" // (AR) دوال بناء عناصر الواجهة (نص، زر، عمود...)

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

        // ═════════════════════════════════════════════════════════════════
        // (AR) إعلانات خارجية — ملفات التسجيل حسب المحتوى
        // (EN) Extern declarations — registration files by content
        // ═════════════════════════════════════════════════════════════════
        extern void registerBuiltinsStrings(Interpreter &interpreter);
        extern void registerBuiltinsBasics(Interpreter &interpreter);
        extern void registerBuiltinsMath(Interpreter &interpreter);
        extern void registerBuiltinsAssertions(Interpreter &interpreter);
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
                {"أضف", "نصوص"},
                {"أزل", "نصوص"},
                {std::string(Bs::STR_LENGTH), "نصوص"},
                {std::string(Bs::TO_UPPER), "نصوص"},
                {std::string(Bs::TO_LOWER), "نصوص"},
                {std::string(Bs::SPLIT), "نصوص"},
                {std::string(Bs::JOIN), "نصوص"},
                {std::string(Bm::ABS), "نصوص"},
                {std::string(Bm::MAX), "نصوص"},
                {std::string(Bm::MIN), "نصوص"},
                {"جمع", "نصوص"},
                {std::string(Bm::SQRT), "نصوص"},
                {"حول_رقم", "نصوص"},
                {"لمصفوفة", "نصوص"},
                {"هو_رقم_صحيح", "نصوص"},
                {"هو_رقم_عشري", "نصوص"},
                {"هو_نص", "نصوص"},
                {"هو_مصفوفة", "نصوص"},
                {std::string(Bs::FIND), "نصوص"},
                {std::string(Bs::REPLACE), "نصوص"},
                {std::string(Bs::SUBSTRING), "نصوص"},
                {std::string(Bs::TRIM), "نصوص"},
                {std::string(Bs::STARTS_WITH), "نصوص"},
                {std::string(Bs::ENDS_WITH), "نصوص"},
                {std::string(Bs::CONTAINS), "نصوص"},
                {std::string(Ba::ADD), "نصوص"},
                {std::string(Ba::REMOVE), "نصوص"},
                {std::string(Ba::SIZE), "نصوص"},
                {std::string(Ba::ARRAY_CONTAINS), "نصوص"},
                {std::string(Ba::REVERSE), "نصوص"},
                {std::string(Ba::SORT), "نصوص"},
                {"أول", "نصوص"},
                {"آخر", "نصوص"},
                {std::string(Ba::SLICE), "نصوص"},
                {std::string(Bm::POWER), "نصوص"},
                {std::string(Bm::ROUND), "نصوص"},
                {std::string(Bm::FLOOR), "نصوص"},
                {std::string(Bm::CEIL), "نصوص"},
                {std::string(Bm::SQUARE), "نصوص"},

                // ─── أساسيات ───
                {std::string(Bb::EXIT), "أساسيات"},
                {std::string(Bb::ASSERT), "أساسيات"},
                {"اقرأ_أسطر", "أساسيات"},
                {"أضف_إلى_ملف", "أساسيات"},
                {"انسخ_ملف", "أساسيات"},
                {"انقل_ملف", "أساسيات"},
                {"احذف_ملف", "أساسيات"},
                {std::string(Bb::MKDIR), "أساسيات"},
                {"اسرد_مجلد", "أساسيات"},
                {"احذف_مجلد", "أساسيات"},
                {std::string(Bb::IS_FILE), "أساسيات"},
                {std::string(Bb::IS_DIR), "أساسيات"},
                {std::string(Bb::RANGE), "أساسيات"},
                {std::string(Bb::READ_FILE), "أساسيات"},
                {std::string(Bb::WRITE_FILE), "أساسيات"},
                {std::string(Bb::FILE_EXISTS), "أساسيات"},

                // ─── رياضيات ───
                // (AR) الأسماء الأساسية فقط — اسم واحد لكل دالة
                {std::string(Bm::LOG), "رياضيات"},
                {std::string(Bm::LOG10), "رياضيات"},
                {std::string(Bm::LOG2), "رياضيات"},
                {std::string(Bm::EXP), "رياضيات"},
                {std::string(Bm::CLAMP), "رياضيات"},
                {std::string(Bm::TRUNCATE), "رياضيات"},
                {std::string(Bm::FMOD), "رياضيات"},
                {std::string(Bm::SIGN), "رياضيات"},
                {std::string(Bm::PI), "رياضيات"},
                {std::string(Bm::E), "رياضيات"},
                {std::string(Bm::LERP), "رياضيات"},
                {std::string(Bm::RANDOM_RANGE), "رياضيات"},
                {std::string(Bm::RANDOM_FLOAT), "رياضيات"},
                {"احصل_بيئة", "رياضيات"},
                {"عيّن_بيئة", "رياضيات"},
                {"مجلد_حالي", "رياضيات"},
                {"غيّر_مجلد", "رياضيات"},
                {"نفّذ", "رياضيات"},
                {"منصة", "رياضيات"},
                {"نظام", "رياضيات"},
                {"طابع_زمن", "رياضيات"},
                {"ساعة", "رياضيات"},
                {"منفذ_اكتب", "رياضيات"},
                {"منفذ_اقرأ", "رياضيات"},
                {"ذاكرة_اكتب", "رياضيات"},
                {"ذاكرة_اقرأ", "رياضيات"},
                {"مقاطعة", "رياضيات"},
                {"توقف", "رياضيات"},
                {"تعطيل_مقاطعات", "رياضيات"},
                {"تفعيل_مقاطعات", "رياضيات"},
                {"شاشة_اكتب", "رياضيات"},
                {"شاشة_امسح", "رياضيات"},
                {"انسخ_ذاكرة", "رياضيات"},
                {"املأ_ذاكرة", "رياضيات"},
                {"تسلسل_هئ", "رياضيات"},
                {"تسلسل_ارسل", "رياضيات"},
                {"تسلسل_استقبل", "رياضيات"},
                {"تسلسل_جاهز", "رياضيات"},
                {"منفذ_رقمي_اكتب", "رياضيات"},
                {"منفذ_رقمي_اقرأ", "رياضيات"},
                {"حدد_وضع_منفذ", "رياضيات"},
                {"مؤقت_هئ", "رياضيات"},
                {"مؤقت_قراءة", "رياضيات"},
                {"مؤقت_انتظر", "رياضيات"},
                {"اعد_تشغيل", "رياضيات"},
                {"معرف_المعالج", "رياضيات"},
                {"عداد_الدورات", "رياضيات"},
                {"حاجز_ذاكرة", "رياضيات"},
                {"حاجز_قراءة", "رياضيات"},
                {"حاجز_كتابة", "رياضيات"},
                {"نقل_مباشر_هئ", "رياضيات"},
                {"نقل_مباشر_ابدأ", "رياضيات"},

                // ─── تأكيدات ───
                {"آمن", "تأكيدات"},
                {"ذعر", "تأكيدات"},
                {std::string(Bss::ASSERT_EQ), "تأكيدات"},
                {std::string(Bss::ASSERT_NEQ), "تأكيدات"},
                {std::string(Bss::ASSERT_TRUE), "تأكيدات"},
                {std::string(Bss::ASSERT_FALSE), "تأكيدات"},
                {"تأكد_لاشيء", "تأكيدات"},
                {"تأكد_ليس_لاشيء", "تأكيدات"},
                {"تأكد_أصغر", "تأكيدات"},
                {std::string(Bss::HASH), "تأكيدات"},
                {"شفّر", "تأكيدات"},
                {"فك_تشفير", "تأكيدات"},
                {"تأكد_نوع", "تأكيدات"},
                {"تأكد_أكبر", "تأكيدات"},
                {"نظّف", "تأكيدات"},
                {"عشوائي_آمن", "تأكيدات"},
                {"ترميز_64", "تأكيدات"},
                {"طباعة_تنسيق", "تأكيدات"},
                {"حجز", "تأكيدات"},
                {"حرر", "تأكيدات"},
                {"اعد_حجز", "تأكيدات"},
                {"حجز_صفري", "تأكيدات"},
                {"طول_نص_س", "تأكيدات"},
                {"انسخ_نص_س", "تأكيدات"},
                {"قارن_نص_س", "تأكيدات"},
                {"الحق_نص_س", "تأكيدات"},
                {"افتح_ملف_س", "تأكيدات"},
                {"اغلق_ملف_س", "تأكيدات"},
                {"اكتب_ملف_س", "تأكيدات"},
                {"اقرأ_ملف_س", "تأكيدات"},
                {"نفذ_امر", "تأكيدات"},
                {"قيمة_بيئة", "تأكيدات"},
                {"نص_لعدد", "تأكيدات"},
                {"نص_لعشري", "تأكيدات"},
                {"تنسيق_نص", "تأكيدات"},

                // ─── خرائط ───
                {"خريطة", "خرائط"},
                {"خريطة_احصل", "خرائط"},
                {"خريطة_عيّن", "خرائط"},
                {"خريطة_مفاتيح", "خرائط"},
                {"خريطة_قيم", "خرائط"},
                {"خريطة_تحتوي", "خرائط"},
                {"خريطة_احذف", "خرائط"},
                {"خريطة_حجم", "خرائط"},
                {"خريطة_عناصر", "خرائط"},
                {"خريطة_دمج", "خرائط"},
                {"تخطيط", "خرائط"},
                {"تصفية", "خرائط"},
                {"اختزال", "خرائط"},
                {"لكل_عنصر", "خرائط"},
                {"تخطيط_مسطح", "خرائط"},
                {"ضم", "خرائط"},
                {"أي_عنصر", "خرائط"},
                {"كل_العناصر", "خرائط"},
                {std::string(Bmp::NOW), "خرائط"},
                {"الآن_مللي", "خرائط"},
                {"صيغة_تاريخ", "خرائط"},
                {"مكونات_تاريخ", "خرائط"},
                {"سنة", "خرائط"},
                {"شهر", "خرائط"},
                {"يوم", "خرائط"},
                {"ساعة", "خرائط"},
                {"دقيقة", "خرائط"},
                {"ثانية", "خرائط"},
                {"يوم_الأسبوع", "خرائط"},
                {"فرق_وقت", "خرائط"},
                {std::string(Bmp::REGEX), "خرائط"},
                {"تعبير_بحث", "خرائط"},
                {"تعبير_استبدال", "خرائط"},
                {"تعبير_جد_الكل", "خرائط"},
                // (AR) دوال خرائط خاصة — نصوص ومسارات وتشفير
                {"تكرار_نص", "خرائط"},
                {"حشو_بداية", "خرائط"},
                {"حشو_نهاية", "خرائط"},
                {"عكس_نص", "خرائط"},
                {"رمز_حرف", "خرائط"},
                {"حرف_من_رمز", "خرائط"},
                {"عدّ", "خرائط"},
                {"تنسيق", "خرائط"},
                {"حجم_ملف", "خرائط"},
                {"ضم_مسار", "خرائط"},
                {"مجلد_مسار", "خرائط"},
                {"اسم_ملف", "خرائط"},
                {"امتداد_ملف", "خرائط"},
                {"ترميز_قاعدة64", "خرائط"},
                {"فك_قاعدة64", "خرائط"},
                {"ترميز_ست_عشري", "خرائط"},
                {"ترميز_عنوان", "خرائط"},
                {"فك_عنوان", "خرائط"},
                {"متغير_بيئة", "خرائط"},
                {"المجلد_الحالي", "خرائط"},
                {"خطأ", "خرائط"},
                {"فريد", "خرائط"},
                {"اتحاد", "خرائط"},
                {"تقاطع", "خرائط"},
                {"فرق", "خرائط"},
                {std::string(Bmp::JSON_PARSE), "خرائط"},
                {std::string(Bmp::JSON_STRINGIFY), "خرائط"},
                {"json_منسق", "خرائط"},
                {"حلل_xml", "خرائط"},
                {"حول_لـxml", "خرائط"},
                {"xml_منسق", "خرائط"},
                {"تسطيح", "خرائط"},
                {"تقطيع", "خرائط"},
                {"خذ", "خرائط"},
                {"اترك", "خرائط"},
                {"رقّم", "خرائط"},
                {"متوسط", "خرائط"},
                {"أصغر_المصفوفة", "خرائط"},
                {"أكبر_المصفوفة", "خرائط"},
                {"ترتيب_مصفوفة", "خرائط"},
                {"عكس_مصفوفة", "خرائط"},
                {"ربط", "خرائط"},
                {"ازل_تشكيل", "خرائط"},
                {"مقارنة_نص", "خرائط"},
                {"نص_يونيكود", "خرائط"},
                {"مصفوفة_جديدة", "خرائط"},
                {"مصفوفة_وحدة", "خرائط"},
                {"نطاق_مصفوفة", "خرائط"},
                {"أبعاد_مصفوفة", "خرائط"},
                {"قلب_محوري", "خرائط"},

                // ─── تزامن_متقدم ───
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x85\\xd9\\x87\\xd9\\x85\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd9\\x85\\xd9\\x87\\xd9\\x85\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xaa\\xd8\\xac", "تزامن_متقدم"},
                {"\\xd9\\x86\\xd9\\x88\\xd9\\x85_\\xd8\\xba\\xd9\\x8a\\xd8\\xb1_\\xd9\\x85\\xd8\\xaa\\xd8\\xb2\\xd8\\xa7\\xd9\\x85\\xd9\\x86", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x88\\xd9\\x81_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xad\\xd9\\x82\\xd9\\x82_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xad\\xd8\\xb5\\xd9\\x84_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xb1\\xd8\\xb3\\xd9\\x84_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xb1\\xd8\\xb3\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xba\\xd9\\x84\\xd9\\x82_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x81\\xd8\\xaa\\xd8\\xad_\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", "تزامن_متقدم"},
                {"\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xb6\\xd9\\x85_\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"\\xd8\\xad\\xd9\\x85\\xd9\\x84_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"\\xd8\\xae\\xd8\\xb2\\xd9\\x86_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xb6\\xd9\\x81_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"\\xd9\\x82\\xd8\\xa7\\xd8\\xb1\\xd9\\x86_\\xd9\\x88\\xd8\\xa8\\xd8\\xaf\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd8\\xa3\\xd9\\x8a", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xae\\xd8\\xaa\\xd8\\xb1_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd9\\x87\\xd9\\x88_\\xd9\\x85\\xd8\\xab\\xd9\\x8a\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd9\\x85\\xd8\\xab\\xd9\\x8a\\xd9\\x84_\\xd9\\x85\\xd9\\x86", "تزامن_متقدم"},
                {"\\xd9\\x86\\xd9\\x88\\xd8\\xb9_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd8\\xa7\\xd8\\xa6\\xd9\\x86", "تزامن_متقدم"},
                {"\\xd9\\x86\\xd9\\x88\\xd8\\xb9", "تزامن_متقدم"},
                {"\\xd8\\xad\\xd9\\x82\\xd9\\x88\\xd9\\x84_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd8\\xa7\\xd8\\xa6\\xd9\\x86", "تزامن_متقدم"},

                // ─── منصة ───
                {"\\xd8\\xa7\\xd9\\x84\\xd9\\x85\\xd9\\x86\\xd8\\xb5\\xd8\\xa9", "منصة"},
                {"\\xd9\\x87\\xd9\\x84_\\xd8\\xb3\\xd8\\xb7\\xd8\\xad_\\xd8\\xa7\\xd9\\x84\\xd9\\x85\\xd9\\x83\\xd8\\xaa\\xd8\\xa8", "منصة"},
                {"\\xd9\\x87\\xd9\\x84_\\xd8\\xa7\\xd9\\x86\\xd8\\xaf\\xd8\\xb1\\xd9\\x88\\xd9\\x8a\\xd8\\xaf", "منصة"},
                {"\\xd9\\x87\\xd9\\x84_\\xd9\\x88\\xd9\\x8a\\xd8\\xa8", "منصة"},

                // ─── مقابس (أسماء عربية دلالية) ───
                {std::string(Bsk::TCP_NEW), "مقابس"},
                {std::string(Bsk::TCP_CONNECT), "مقابس"},
                {std::string(Bsk::TCP_SEND), "مقابس"},
                {std::string(Bsk::TCP_RECV), "مقابس"},
                {std::string(Bsk::TCP_BIND), "مقابس"},
                {std::string(Bsk::TCP_LISTEN), "مقابس"},
                {std::string(Bsk::TCP_ACCEPT), "مقابس"},
                {std::string(Bsk::TCP_CLOSE), "مقابس"},
                {std::string(Bsk::TCP_CONNECTED), "مقابس"},
                {std::string(Bsk::TCP_REMOTE_ADDR), "مقابس"},
                {std::string(Bsk::UDP_NEW), "مقابس"},
                {std::string(Bsk::UDP_BIND), "مقابس"},
                {std::string(Bsk::UDP_SEND), "مقابس"},
                {std::string(Bsk::UDP_RECV), "مقابس"},
                {std::string(Bsk::UDP_CLOSE), "مقابس"},
                {std::string(Bsk::RECV_TIMEOUT), "مقابس"},
                {std::string(Bsk::SEND_TIMEOUT), "مقابس"},
                {std::string(Bsk::NO_DELAY), "مقابس"},
                {std::string(Bsk::LOCAL_PORT), "مقابس"},
                // ─── عميل HTTP ───
                {std::string(Bhc::NEW_CLIENT), "شبكة"},
                {std::string(Bhc::FREE_CLIENT), "شبكة"},
                {std::string(Bhc::GET), "شبكة"},
                {std::string(Bhc::POST), "شبكة"},
                {std::string(Bhc::PUT), "شبكة"},
                {std::string(Bhc::DELETE_REQ), "شبكة"},
                {std::string(Bhc::PATCH), "شبكة"},
                {std::string(Bhc::SET_BASE_URL), "شبكة"},
                {std::string(Bhc::SET_HEADER), "شبكة"},
                {std::string(Bhc::SET_TIMEOUT), "شبكة"},
                {std::string(Bhc::SET_BEARER), "شبكة"},
                {std::string(Bhc::LAST_ERROR), "شبكة"},
                {std::string(Bhc::IS_OK), "شبكة"},
                {std::string(Bhc::RESP_FREE), "شبكة"},
                {std::string(Bhc::RESP_STATUS), "شبكة"},
                {std::string(Bhc::RESP_BODY), "شبكة"},
                {std::string(Bhc::RESP_HEADER), "شبكة"},
                {std::string(Bhc::RESP_SUCCESS), "شبكة"},
                // ─── خادم HTTP ───
                {std::string(Bhs::NEW_SERVER), "شبكة"},
                {std::string(Bhs::FREE_SERVER), "شبكة"},
                {std::string(Bhs::ON_GET), "شبكة"},
                {std::string(Bhs::ON_POST), "شبكة"},
                {std::string(Bhs::ON_PUT), "شبكة"},
                {std::string(Bhs::ON_DELETE), "شبكة"},
                {std::string(Bhs::LISTEN), "شبكة"},
                {std::string(Bhs::STOP), "شبكة"},
                {std::string(Bhs::ENABLE_CORS), "شبكة"},
                {std::string(Bhs::REQ_METHOD), "شبكة"},
                {std::string(Bhs::REQ_PATH), "شبكة"},
                {std::string(Bhs::REQ_BODY), "شبكة"},
                {std::string(Bhs::REQ_HEADER), "شبكة"},
                {std::string(Bhs::REQ_QUERY), "شبكة"},
                {std::string(Bhs::RESP_SET_STATUS), "شبكة"},
                {std::string(Bhs::RESP_SET_BODY), "شبكة"},
                {std::string(Bhs::RESP_SET_JSON), "شبكة"},
                {std::string(Bhs::RESP_SET_HTML), "شبكة"},
                {std::string(Bhs::RESP_SET_HEADER), "شبكة"},
                // ─── أدوات الشبكة ───
                {std::string(Bnu::INIT), "شبكة"},
                {std::string(Bnu::CLEANUP), "شبكة"},
                {std::string(Bnu::LAST_ERROR_CODE), "شبكة"},
                {std::string(Bnu::LAST_ERROR_MSG), "شبكة"},
                {std::string(Bnu::ADDR_NEW), "شبكة"},
                {std::string(Bnu::ADDR_NEW_V6), "شبكة"},
                {std::string(Bnu::ADDR_FREE), "شبكة"},
                {std::string(Bnu::ADDR_IP), "شبكة"},
                {std::string(Bnu::ADDR_PORT), "شبكة"},
                {std::string(Bnu::ADDR_IS_V4), "شبكة"},
                {std::string(Bnu::ADDR_IS_V6), "شبكة"},
                {std::string(Bnu::ADDR_STR), "شبكة"},
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
            registry.registerModule({"مقابس",
                                     "وصلات TCP، مراسيل UDP، إعدادات الشبكة",
                                     [](Interpreter &interp)
                                     { registerBuiltinsSockets(interp); },
                                     {}});

            // ─── شبكة / Network HTTP (Part 40b) — HTTP client, server, utilities ───
            registry.registerModule({"شبكة",
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

            // ─── تشفير / Crypto (Part 7 crypto functions) — SHA-256, تشفير/فك_تشفير ───
            registry.registerModule({"\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // تشفير
                                     "SHA-256\xd8\x8c \xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1 CTR\xd8\x8c \xd9\x87\xd8\xa7\xd8\xb4",
                                     // SHA-256، تشفير CTR، هاش
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsAssertions(interp); // SHA-256, encrypt/decrypt, hash
                                     },
                                     {}});

            // ─── رسومات / Graphics — عناصر واجهة المستخدم التصريحية ───
            // (AR) يُسجِّل دوال بناء العناصر (~30 دالة) + محرك الواجهات عند `استورد رسومات`
            // (EN) Registers widget builder functions (~30) + UI engine on `استورد رسومات`
            registry.registerModule({"\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa", // رسومات
                                     "\xd8\xb9\xd9\x86\xd8\xa7\xd8\xb5\xd8\xb1 \xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb5\xd8\xb1\xd9\x8a\xd8\xad\xd9\x8a\xd8\xa9",
                                     // عناصر واجهة المستخدم التصريحية
                                     [](Interpreter &interp)
                                     {
                                         // (AR) تسجيل دوال بناء العناصر (نص، زر، عمود، ...)
                                         registerWidgetBuiltins(interp);
#if !defined(SAD_NO_SDL2) || defined(SAD_WASM_BUILD) || defined(SAD_PLATFORM_ANDROID)
                                         // (AR) تسجيل محرك الواجهات (تشغيل_تطبيق، التنقل، ...)
                                         registerUIBuiltins(interp);
#endif
                                     },
                                     {}});

            // ─── _محرك_واجهات / UI Engine — محرك واجهات داخلي ───
            // يسجل جميع الدوال عند استيراد _محرك_واجهات (تحميل كسول)
            registry.registerModule({"_\xd9\x85\xd8\xad\xd8\xb1\xd9\x83_\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa7\xd8\xaa", // _محرك_واجهات
                                     "\xd9\x85\xd8\xad\xd8\xb1\xd9\x83 \xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd8\xae\xd9\x84\xd9\x8a",
                                     // محرك واجهات المستخدم الداخلي
                                     [](Interpreter &interp)
                                     {
#if !defined(SAD_NO_SDL2) || defined(SAD_WASM_BUILD) || defined(SAD_PLATFORM_ANDROID)
                                         registerUIBuiltins(interp); // يسجل جميع الوحدات الفرعية
#endif
                                     },
                                     {}});

            // ─── وحدات الخدمات الفرعية (تحميل كسول مستقل) ───
            registry.registerModule({"_\xd8\xb5\xd9\x88\xd8\xaa",                                                 // _صوت
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb5\xd9\x88\xd8\xaa", // نظام الصوت
                                     [](Interpreter &interp)
                                     { registerUIAudioBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa",                                                 // _مؤقتات
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // نظام المؤقتات
                                     [](Interpreter &interp)
                                     { registerUITimerBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86",                                                 // _تخزين
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // نظام التخزين
                                     [](Interpreter &interp)
                                     { registerUIStorageBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9",                                                 // _شبكة
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", // نظام الشبكة
                                     [](Interpreter &interp)
                                     { registerUINetworkBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1",                                                 // _تشفير
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // نظام التشفير
                                     [](Interpreter &interp)
                                     { registerUICryptoBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9",                                                         // _منصة
                                     "\xd8\xae\xd8\xaf\xd9\x85\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", // خدمات المنصة
                                     [](Interpreter &interp)
                                     { registerUIPlatformBuiltins(interp); },
                                     {}});
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
