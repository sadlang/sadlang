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

#include <algorithm>

namespace Sad
{
    namespace Interpreter
    {

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
        extern void registerBuiltinsSadNet(Interpreter &interpreter);

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
                {"أضف", "نصوص"},
                {"append", "نصوص"},
                {"أزل", "نصوص"},
                {"remove", "نصوص"},
                {"نص_طول", "نصوص"},
                {"str_len", "نصوص"},
                {"أحرف_كبيرة", "نصوص"},
                {"upper", "نصوص"},
                {"أحرف_صغيرة", "نصوص"},
                {"lower", "نصوص"},
                {"قسم", "نصوص"},
                {"split", "نصوص"},
                {"صل", "نصوص"},
                {"join", "نصوص"},
                {"القيمة_المطلقة", "نصوص"},
                {"abs", "نصوص"},
                {"أكبر", "نصوص"},
                {"max", "نصوص"},
                {"أصغر", "نصوص"},
                {"min", "نصوص"},
                {"جمع", "نصوص"},
                {"الجذر", "نصوص"},
                {"sqrt", "نصوص"},
                {"حول_رقم", "نصوص"},
                {"string", "نصوص"},
                {"لمصفوفة", "نصوص"},
                {"toArray", "نصوص"},
                {"to_array", "نصوص"},
                {"toBool", "نصوص"},
                {"boolean", "نصوص"},
                {"هو_رقم_صحيح", "نصوص"},
                {"isInt", "نصوص"},
                {"is_int", "نصوص"},
                {"isInteger", "نصوص"},
                {"هو_رقم_عشري", "نصوص"},
                {"isFloat", "نصوص"},
                {"is_float", "نصوص"},
                {"isDouble", "نصوص"},
                {"هو_نص", "نصوص"},
                {"isString", "نصوص"},
                {"is_string", "نصوص"},
                {"isStr", "نصوص"},
                {"هو_مصفوفة", "نصوص"},
                {"isArray", "نصوص"},
                {"is_array", "نصوص"},
                {"isList", "نصوص"},
                {"طول_نص", "نصوص"},
                {"string_length", "نصوص"},
                {"str_length", "نصوص"},
                {"بحث", "نصوص"},
                {"find", "نصوص"},
                {"indexOf", "نصوص"},
                {"استبدل", "نصوص"},
                {"replace", "نصوص"},
                {"str_replace", "نصوص"},
                {"استخراج", "نصوص"},
                {"substring", "نصوص"},
                {"substr", "نصوص"},
                {"تحويل_صغير", "نصوص"},
                {"toLower", "نصوص"},
                {"lowercase", "نصوص"},
                {"تحويل_كبير", "نصوص"},
                {"toUpper", "نصوص"},
                {"uppercase", "نصوص"},
                {"قص_أطراف", "نصوص"},
                {"trim", "نصوص"},
                {"strip", "نصوص"},
                {"تقسيم", "نصوص"},
                {"split_string", "نصوص"},
                {"str_split", "نصوص"},
                {"دمج", "نصوص"},
                {"join_strings", "نصوص"},
                {"str_join", "نصوص"},
                {"يبدأ_ب", "نصوص"},
                {"startsWith", "نصوص"},
                {"starts_with", "نصوص"},
                {"ينتهي_ب", "نصوص"},
                {"endsWith", "نصوص"},
                {"ends_with", "نصوص"},
                {"يحتوي_على", "نصوص"},
                {"contains", "نصوص"},
                {"str_contains", "نصوص"},
                {"إضافة_عنصر", "نصوص"},
                {"array_append", "نصوص"},
                {"arr_append", "نصوص"},
                {"إزالة_عنصر", "نصوص"},
                {"array_pop", "نصوص"},
                {"arr_pop", "نصوص"},
                {"حجم_مصفوفة", "نصوص"},
                {"array_size", "نصوص"},
                {"arr_size", "نصوص"},
                {"array_indexOf", "نصوص"},
                {"findIndex", "نصوص"},
                {"يحتوي_عنصر", "نصوص"},
                {"array_contains", "نصوص"},
                {"arr_contains", "نصوص"},
                {"قلب", "نصوص"},
                {"reverse", "نصوص"},
                {"array_reverse", "نصوص"},
                {"فرز", "نصوص"},
                {"array_sort", "نصوص"},
                {"أول", "نصوص"},
                {"first", "نصوص"},
                {"array_first", "نصوص"},
                {"آخر", "نصوص"},
                {"last", "نصوص"},
                {"array_last", "نصوص"},
                {"شريحة", "نصوص"},
                {"slice", "نصوص"},
                {"array_slice", "نصوص"},
                {"جذر", "نصوص"},
                {"أس", "نصوص"},
                {"power", "نصوص"},
                {"pow", "نصوص"},
                {"مطلق", "نصوص"},
                {"absolute", "نصوص"},
                {"maximum", "نصوص"},
                {"أقصى", "نصوص"},
                {"minimum", "نصوص"},
                {"أدنى", "نصوص"},
                {"تقريب", "نصوص"},
                {"round", "نصوص"},
                {"أرضية", "نصوص"},
                {"floor", "نصوص"},
                {"سقف", "نصوص"},
                {"ceil", "نصوص"},
                {"ceiling", "نصوص"},
                {"مربع", "نصوص"},
                {"square", "نصوص"},

                // ─── أساسيات ───
                {"اخرج", "أساسيات"},
                {"exit", "أساسيات"},
                {"quit", "أساسيات"},
                {"تأكد", "أساسيات"},
                {"assert", "أساسيات"},
                {"اقرأ_أسطر", "أساسيات"},
                {"read_lines", "أساسيات"},
                {"أضف_إلى_ملف", "أساسيات"},
                {"append_to_file", "أساسيات"},
                {"انسخ_ملف", "أساسيات"},
                {"copy_file", "أساسيات"},
                {"انقل_ملف", "أساسيات"},
                {"move_file", "أساسيات"},
                {"احذف_ملف", "أساسيات"},
                {"delete_file", "أساسيات"},
                {"أنشئ_مجلد", "أساسيات"},
                {"create_directory", "أساسيات"},
                {"mkdir", "أساسيات"},
                {"اسرد_مجلد", "أساسيات"},
                {"list_directory", "أساسيات"},
                {"ls", "أساسيات"},
                {"احذف_مجلد", "أساسيات"},
                {"remove_directory", "أساسيات"},
                {"rmdir", "أساسيات"},
                {"هل_ملف", "أساسيات"},
                {"is_file", "أساسيات"},
                {"هل_مجلد", "أساسيات"},
                {"is_directory", "أساسيات"},
                {"is_dir", "أساسيات"},
                {"مدى", "أساسيات"},
                {"range", "أساسيات"},
                {"اقرأ_ملف", "أساسيات"},
                {"read_file", "أساسيات"},
                {"اكتب_ملف", "أساسيات"},
                {"write_file", "أساسيات"},
                {"هل_موجود", "أساسيات"},
                {"exists", "أساسيات"},

                // ─── رياضيات ───
                {"لوغ", "رياضيات"},
                {"log", "رياضيات"},
                {"لوغاريتم", "رياضيات"},
                {"لوغ10", "رياضيات"},
                {"log10", "رياضيات"},
                {"لوغاريتم_عشري", "رياضيات"},
                {"لوغ2", "رياضيات"},
                {"log2", "رياضيات"},
                {"لوغاريتم_ثنائي", "رياضيات"},
                {"أس", "رياضيات"},
                {"exp", "رياضيات"},
                {"أُس", "رياضيات"},
                {"قيد", "رياضيات"},
                {"clamp", "رياضيات"},
                {"تقييد", "رياضيات"},
                {"اقتطاع", "رياضيات"},
                {"trunc", "رياضيات"},
                {"اقتطع", "رياضيات"},
                {"باقي", "رياضيات"},
                {"mod", "رياضيات"},
                {"باقي_القسمة", "رياضيات"},
                {"إشارة", "رياضيات"},
                {"sign", "رياضيات"},
                {"باي", "رياضيات"},
                {"pi", "رياضيات"},
                {"ط", "رياضيات"},
                {"إ", "رياضيات"},
                {"e_const", "رياضيات"},
                {"أولر", "رياضيات"},
                {"احصل_بيئة", "رياضيات"},
                {"عيّن_بيئة", "رياضيات"},
                {"setEnv", "رياضيات"},
                {"set_env", "رياضيات"},
                {"مجلد_حالي", "رياضيات"},
                {"currentDir", "رياضيات"},
                {"المجلد_الحالي", "رياضيات"},
                {"pwd", "رياضيات"},
                {"غيّر_مجلد", "رياضيات"},
                {"changeDir", "رياضيات"},
                {"cd", "رياضيات"},
                {"نفّذ", "رياضيات"},
                {"تنفذ", "رياضيات"},
                {"منصة", "رياضيات"},
                {"المنصة", "رياضيات"},
                {"نظام", "رياضيات"},
                {"osName", "رياضيات"},
                {"os", "رياضيات"},
                {"طابع_زمن", "رياضيات"},
                {"الطابع_الزمني", "رياضيات"},
                {"ساعة", "رياضيات"},
                {"clock", "رياضيات"},
                {"وقت_الساعة", "رياضيات"},
                {"منفذ_اكتب", "رياضيات"},
                {"منفذ_اقرأ", "رياضيات"},
                {"ذاكرة_اكتب", "رياضيات"},
                {"ذاكرة_اقرأ", "رياضيات"},
                {"مقاطعة", "رياضيات"},
                {"interrupt", "رياضيات"},
                {"int_call", "رياضيات"},
                {"توقف", "رياضيات"},
                {"تعطيل_مقاطعات", "رياضيات"},
                {"تفعيل_مقاطعات", "رياضيات"},
                {"شاشة_اكتب", "رياضيات"},
                {"vga_write", "رياضيات"},
                {"شاشة_امسح", "رياضيات"},
                {"vga_clear", "رياضيات"},
                {"انسخ_ذاكرة", "رياضيات"},
                {"املأ_ذاكرة", "رياضيات"},
                {"تسلسل_هئ", "رياضيات"},
                {"تسلسل_ارسل", "رياضيات"},
                {"تسلسل_استقبل", "رياضيات"},
                {"تسلسل_جاهز", "رياضيات"},
                {"منفذ_رقمي_اكتب", "رياضيات"},
                {"gpio_write", "رياضيات"},
                {"digital_write", "رياضيات"},
                {"منفذ_رقمي_اقرأ", "رياضيات"},
                {"gpio_read", "رياضيات"},
                {"digital_read", "رياضيات"},
                {"حدد_وضع_منفذ", "رياضيات"},
                {"gpio_mode", "رياضيات"},
                {"pin_mode", "رياضيات"},
                {"مؤقت_هئ", "رياضيات"},
                {"timer_init", "رياضيات"},
                {"مؤقت_قراءة", "رياضيات"},
                {"timer_read", "رياضيات"},
                {"مؤقت_انتظر", "رياضيات"},
                {"timer_wait", "رياضيات"},
                {"delay_us", "رياضيات"},
                {"اعد_تشغيل", "رياضيات"},
                {"reset", "رياضيات"},
                {"reboot", "رياضيات"},
                {"معرف_المعالج", "رياضيات"},
                {"cpu_id", "رياضيات"},
                {"cpuid", "رياضيات"},
                {"عداد_الدورات", "رياضيات"},
                {"حاجز_ذاكرة", "رياضيات"},
                {"memory_barrier", "رياضيات"},
                {"mfence", "رياضيات"},
                {"حاجز_قراءة", "رياضيات"},
                {"read_barrier", "رياضيات"},
                {"lfence", "رياضيات"},
                {"حاجز_كتابة", "رياضيات"},
                {"write_barrier", "رياضيات"},
                {"sfence", "رياضيات"},
                {"نقل_مباشر_هئ", "رياضيات"},
                {"dma_init", "رياضيات"},
                {"نقل_مباشر_ابدأ", "رياضيات"},
                {"dma_start", "رياضيات"},

                // ─── تأكيدات ───
                {"آمن", "تأكيدات"},
                {"is_safe", "تأكيدات"},
                {"ذعر", "تأكيدات"},
                {"panic", "تأكيدات"},
                {"تأكد_يساوي", "تأكيدات"},
                {"assert_eq", "تأكيدات"},
                {"تأكد_لا_يساوي", "تأكيدات"},
                {"assert_not_equal", "تأكيدات"},
                {"assert_ne", "تأكيدات"},
                {"تأكد_صحيح", "تأكيدات"},
                {"assert_true", "تأكيدات"},
                {"تأكد_خطأ", "تأكيدات"},
                {"assert_false", "تأكيدات"},
                {"تأكد_لاشيء", "تأكيدات"},
                {"assert_null", "تأكيدات"},
                {"تأكد_ليس_لاشيء", "تأكيدات"},
                {"assert_not_null", "تأكيدات"},
                {"assert_gt", "تأكيدات"},
                {"تأكد_أصغر", "تأكيدات"},
                {"assert_less", "تأكيدات"},
                {"assert_lt", "تأكيدات"},
                {"هاش", "تأكيدات"},
                {"hash", "تأكيدات"},
                {"sha256", "تأكيدات"},
                {"شفّر", "تأكيدات"},
                {"encrypt", "تأكيدات"},
                {"فك_تشفير", "تأكيدات"},
                {"decrypt", "تأكيدات"},
                {"تأكد_نوع", "تأكيدات"},
                {"assert_type", "تأكيدات"},
                {"assert_equal", "تأكيدات"},
                {"تأكد_أكبر", "تأكيدات"},
                {"assert_greater", "تأكيدات"},
                {"نظّف", "تأكيدات"},
                {"sanitize", "تأكيدات"},
                {"عشوائي_آمن", "تأكيدات"},
                {"secure_random", "تأكيدات"},
                {"ترميز_64", "تأكيدات"},
                {"base64_encode", "تأكيدات"},
                {"طباعة_تنسيق", "تأكيدات"},
                {"printf", "تأكيدات"},
                {"c_printf", "تأكيدات"},
                {"حجز", "تأكيدات"},
                {"malloc", "تأكيدات"},
                {"c_malloc", "تأكيدات"},
                {"حرر", "تأكيدات"},
                {"free", "تأكيدات"},
                {"c_free", "تأكيدات"},
                {"اعد_حجز", "تأكيدات"},
                {"realloc", "تأكيدات"},
                {"c_realloc", "تأكيدات"},
                {"حجز_صفري", "تأكيدات"},
                {"calloc", "تأكيدات"},
                {"c_calloc", "تأكيدات"},
                {"طول_نص_س", "تأكيدات"},
                {"strlen", "تأكيدات"},
                {"c_strlen", "تأكيدات"},
                {"انسخ_نص_س", "تأكيدات"},
                {"strcpy", "تأكيدات"},
                {"c_strcpy", "تأكيدات"},
                {"قارن_نص_س", "تأكيدات"},
                {"strcmp", "تأكيدات"},
                {"c_strcmp", "تأكيدات"},
                {"الحق_نص_س", "تأكيدات"},
                {"strcat", "تأكيدات"},
                {"c_strcat", "تأكيدات"},
                {"افتح_ملف_س", "تأكيدات"},
                {"fopen", "تأكيدات"},
                {"c_fopen", "تأكيدات"},
                {"اغلق_ملف_س", "تأكيدات"},
                {"fclose", "تأكيدات"},
                {"c_fclose", "تأكيدات"},
                {"اكتب_ملف_س", "تأكيدات"},
                {"fputs", "تأكيدات"},
                {"c_fputs", "تأكيدات"},
                {"اقرأ_ملف_س", "تأكيدات"},
                {"fgets", "تأكيدات"},
                {"c_fgets", "تأكيدات"},
                {"نفذ_امر", "تأكيدات"},
                {"system", "تأكيدات"},
                {"c_system", "تأكيدات"},
                {"قيمة_بيئة", "تأكيدات"},
                {"getenv", "تأكيدات"},
                {"c_getenv", "تأكيدات"},
                {"نص_لعدد", "تأكيدات"},
                {"atoi", "تأكيدات"},
                {"c_atoi", "تأكيدات"},
                {"نص_لعشري", "تأكيدات"},
                {"atof", "تأكيدات"},
                {"c_atof", "تأكيدات"},
                {"تنسيق_نص", "تأكيدات"},
                {"snprintf", "تأكيدات"},
                {"c_snprintf", "تأكيدات"},

                // ─── خرائط ───
                {"خريطة", "خرائط"},
                {"map", "خرائط"},
                {"خريطة_احصل", "خرائط"},
                {"map_get", "خرائط"},
                {"خريطة_عيّن", "خرائط"},
                {"map_set", "خرائط"},
                {"خريطة_مفاتيح", "خرائط"},
                {"map_keys", "خرائط"},
                {"خريطة_قيم", "خرائط"},
                {"map_values", "خرائط"},
                {"خريطة_تحتوي", "خرائط"},
                {"map_has_key", "خرائط"},
                {"خريطة_احذف", "خرائط"},
                {"map_delete", "خرائط"},
                {"خريطة_حجم", "خرائط"},
                {"map_size", "خرائط"},
                {"خريطة_عناصر", "خرائط"},
                {"map_entries", "خرائط"},
                {"خريطة_دمج", "خرائط"},
                {"map_merge", "خرائط"},
                {"تخطيط", "خرائط"},
                {"تصفية", "خرائط"},
                {"filter", "خرائط"},
                {"اختزال", "خرائط"},
                {"reduce", "خرائط"},
                {"لكل_عنصر", "خرائط"},
                {"forEach", "خرائط"},
                {"for_each", "خرائط"},
                {"تخطيط_مسطح", "خرائط"},
                {"flatMap", "خرائط"},
                {"flat_map", "خرائط"},
                {"ضم", "خرائط"},
                {"zip", "خرائط"},
                {"أي_عنصر", "خرائط"},
                {"any", "خرائط"},
                {"كل_العناصر", "خرائط"},
                {"all", "خرائط"},
                {"الآن", "خرائط"},
                {"now", "خرائط"},
                {"الآن_مللي", "خرائط"},
                {"now_ms", "خرائط"},
                {"صيغة_تاريخ", "خرائط"},
                {"date_format", "خرائط"},
                {"مكونات_تاريخ", "خرائط"},
                {"date_components", "خرائط"},
                {"سنة", "خرائط"},
                {"year", "خرائط"},
                {"شهر", "خرائط"},
                {"month", "خرائط"},
                {"يوم", "خرائط"},
                {"day", "خرائط"},
                {"ساعة", "خرائط"},
                {"hour", "خرائط"},
                {"دقيقة", "خرائط"},
                {"minute", "خرائط"},
                {"ثانية", "خرائط"},
                {"second", "خرائط"},
                {"يوم_الأسبوع", "خرائط"},
                {"weekday", "خرائط"},
                {"فرق_وقت", "خرائط"},
                {"time_diff", "خرائط"},
                {"تعبير_مطابقة", "خرائط"},
                {"regex_match", "خرائط"},
                {"تعبير_بحث", "خرائط"},
                {"regex_search", "خرائط"},
                {"تعبير_استبدال", "خرائط"},
                {"regex_replace", "خرائط"},
                {"تعبير_جد_الكل", "خرائط"},
                {"regex_find_all", "خرائط"},
                {"ط", "خرائط"},
                {"PI", "خرائط"},
                {"هـ", "خرائط"},
                {"E", "خرائط"},
                {"لوغاريتم", "خرائط"},
                {"لوغاريتم2", "خرائط"},
                {"لوغاريتم10", "خرائط"},
                {"أسي", "خرائط"},
                {"جيب", "خرائط"},
                {"sin", "خرائط"},
                {"جيب_تمام", "خرائط"},
                {"cos", "خرائط"},
                {"ظل", "خرائط"},
                {"tan", "خرائط"},
                {"جيب_عكسي", "خرائط"},
                {"asin", "خرائط"},
                {"جيب_تمام_عكسي", "خرائط"},
                {"acos", "خرائط"},
                {"ظل_عكسي2", "خرائط"},
                {"atan2", "خرائط"},
                {"حصر", "خرائط"},
                {"استيفاء_خطي", "خرائط"},
                {"lerp", "خرائط"},
                {"عشوائي_نطاق", "خرائط"},
                {"random_range", "خرائط"},
                {"عشوائي_عشري", "خرائط"},
                {"random_float", "خرائط"},
                {"تكرار_نص", "خرائط"},
                {"repeat", "خرائط"},
                {"حشو_بداية", "خرائط"},
                {"padStart", "خرائط"},
                {"pad_start", "خرائط"},
                {"حشو_نهاية", "خرائط"},
                {"padEnd", "خرائط"},
                {"pad_end", "خرائط"},
                {"عكس_نص", "خرائط"},
                {"reverse_string", "خرائط"},
                {"رمز_حرف", "خرائط"},
                {"charCodeAt", "خرائط"},
                {"char_code_at", "خرائط"},
                {"حرف_من_رمز", "خرائط"},
                {"fromCharCode", "خرائط"},
                {"from_char_code", "خرائط"},
                {"عدّ", "خرائط"},
                {"count", "خرائط"},
                {"تنسيق", "خرائط"},
                {"format", "خرائط"},
                {"حجم_ملف", "خرائط"},
                {"file_size", "خرائط"},
                {"ضم_مسار", "خرائط"},
                {"path_join", "خرائط"},
                {"مجلد_مسار", "خرائط"},
                {"path_dirname", "خرائط"},
                {"اسم_ملف", "خرائط"},
                {"path_basename", "خرائط"},
                {"امتداد_ملف", "خرائط"},
                {"path_extension", "خرائط"},
                {"ترميز_قاعدة64", "خرائط"},
                {"فك_قاعدة64", "خرائط"},
                {"base64_decode", "خرائط"},
                {"ترميز_ست_عشري", "خرائط"},
                {"hex_encode", "خرائط"},
                {"ترميز_عنوان", "خرائط"},
                {"url_encode", "خرائط"},
                {"فك_عنوان", "خرائط"},
                {"url_decode", "خرائط"},
                {"متغير_بيئة", "خرائط"},
                {"env_get", "خرائط"},
                {"المجلد_الحالي", "خرائط"},
                {"cwd", "خرائط"},
                {"خطأ", "خرائط"},
                {"error", "خرائط"},
                {"فريد", "خرائط"},
                {"unique", "خرائط"},
                {"اتحاد", "خرائط"},
                {"union_arr", "خرائط"},
                {"تقاطع", "خرائط"},
                {"intersect", "خرائط"},
                {"فرق", "خرائط"},
                {"difference", "خرائط"},
                {"تحليل_جيسون", "خرائط"},
                {"json_parse", "خرائط"},
                {"نص_جيسون", "خرائط"},
                {"json_stringify", "خرائط"},
                {"json_منسق", "خرائط"},
                {"json_pretty", "خرائط"},
                {"json_minify", "خرائط"},
                {"json_parse_value", "خرائط"},
                {"حلل_xml", "خرائط"},
                {"xml_parse", "خرائط"},
                {"حول_لـxml", "خرائط"},
                {"xml_stringify", "خرائط"},
                {"xml_minify", "خرائط"},
                {"xml_منسق", "خرائط"},
                {"xml_pretty", "خرائط"},
                {"تسطيح", "خرائط"},
                {"flatten", "خرائط"},
                {"تقطيع", "خرائط"},
                {"chunk", "خرائط"},
                {"خذ", "خرائط"},
                {"take", "خرائط"},
                {"اترك", "خرائط"},
                {"drop", "خرائط"},
                {"رقّم", "خرائط"},
                {"enumerate", "خرائط"},
                {"sum_array", "خرائط"},
                {"متوسط", "خرائط"},
                {"average", "خرائط"},
                {"أصغر_المصفوفة", "خرائط"},
                {"min_array", "خرائط"},
                {"أكبر_المصفوفة", "خرائط"},
                {"max_array", "خرائط"},
                {"ترتيب_مصفوفة", "خرائط"},
                {"sort_array", "خرائط"},
                {"عكس_مصفوفة", "خرائط"},
                {"reverse_array", "خرائط"},
                {"ربط", "خرائط"},
                {"join_array", "خرائط"},
                {"ازل_تشكيل", "خرائط"},
                {"strip_diacritics", "خرائط"},
                {"مقارنة_نص", "خرائط"},
                {"compare_text", "خرائط"},
                {"نص_يونيكود", "خرائط"},
                {"unicode_codepoints", "خرائط"},
                {"مصفوفة_جديدة", "خرائط"},
                {"matrix_new", "خرائط"},
                {"matrix", "خرائط"},
                {"مصفوفة_وحدة", "خرائط"},
                {"identity_matrix", "خرائط"},
                {"نطاق_مصفوفة", "خرائط"},
                {"arange", "خرائط"},
                {"أبعاد_مصفوفة", "خرائط"},
                {"shape", "خرائط"},
                {"قلب_محوري", "خرائط"},
                {"transpose", "خرائط"},

                // ─── تزامن_متقدم ───
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x85\\xd9\\x87\\xd9\\x85\\xd8\\xa9", "تزامن_متقدم"},
                {"spawn", "تزامن_متقدم"},
                {"async_spawn", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd9\\x85\\xd9\\x87\\xd9\\x85\\xd8\\xa9", "تزامن_متقدم"},
                {"await", "تزامن_متقدم"},
                {"async_await", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xaa\\xd8\\xac", "تزامن_متقدم"},
                {"yield", "تزامن_متقدم"},
                {"async_yield", "تزامن_متقدم"},
                {"\\xd9\\x86\\xd9\\x88\\xd9\\x85_\\xd8\\xba\\xd9\\x8a\\xd8\\xb1_\\xd9\\x85\\xd8\\xaa\\xd8\\xb2\\xd8\\xa7\\xd9\\x85\\xd9\\x86", "تزامن_متقدم"},
                {"async_sleep", "تزامن_متقدم"},
                {"sleep_async", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"create_future", "تزامن_متقدم"},
                {"\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x88\\xd9\\x81_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"resolve_future", "تزامن_متقدم"},
                {"\\xd8\\xad\\xd9\\x82\\xd9\\x82_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xad\\xd8\\xb5\\xd9\\x84_\\xd9\\x85\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"get_future", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"create_channel", "تزامن_متقدم"},
                {"\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xb1\\xd8\\xb3\\xd9\\x84_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"channel_send", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xb1\\xd8\\xb3\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"channel_recv", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xba\\xd9\\x84\\xd9\\x82_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"channel_close", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"create_mutex", "تزامن_متقدم"},
                {"\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x81\\xd8\\xaa\\xd8\\xad_\\xd9\\x82\\xd9\\x81\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", "تزامن_متقدم"},
                {"thread_spawn", "تزامن_متقدم"},
                {"\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xb6\\xd9\\x85_\\xd8\\xae\\xd9\\x8a\\xd8\\xb7", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd9\\x86\\xd8\\xb4\\xd8\\xa6_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"create_atomic", "تزامن_متقدم"},
                {"\\xd8\\xad\\xd9\\x85\\xd9\\x84_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"atomic_load", "تزامن_متقدم"},
                {"\\xd8\\xae\\xd8\\xb2\\xd9\\x86_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"atomic_store", "تزامن_متقدم"},
                {"\\xd8\\xa3\\xd8\\xb6\\xd9\\x81_\\xd8\\xb0\\xd8\\xb1\\xd9\\x8a", "تزامن_متقدم"},
                {"atomic_add", "تزامن_متقدم"},
                {"\\xd9\\x82\\xd8\\xa7\\xd8\\xb1\\xd9\\x86_\\xd9\\x88\\xd8\\xa8\\xd8\\xaf\\xd9\\x84", "تزامن_متقدم"},
                {"compare_and_swap", "تزامن_متقدم"},
                {"cas", "تزامن_متقدم"},
                {"atomic_cas", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd9\\x86\\xd8\\xaa\\xd8\\xb8\\xd8\\xb1_\\xd8\\xa3\\xd9\\x8a", "تزامن_متقدم"},
                {"wait_any", "تزامن_متقدم"},
                {"\\xd8\\xa7\\xd8\\xae\\xd8\\xaa\\xd8\\xb1_\\xd9\\x82\\xd9\\x86\\xd8\\xa7\\xd8\\xa9", "تزامن_متقدم"},
                {"select", "تزامن_متقدم"},
                {"channel_select", "تزامن_متقدم"},
                {"\\xd9\\x87\\xd9\\x88_\\xd9\\x85\\xd8\\xab\\xd9\\x8a\\xd9\\x84", "تزامن_متقدم"},
                {"\\xd9\\x85\\xd8\\xab\\xd9\\x8a\\xd9\\x84_\\xd9\\x85\\xd9\\x86", "تزامن_متقدم"},
                {"instanceof", "تزامن_متقدم"},
                {"\\xd9\\x86\\xd9\\x88\\xd8\\xb9_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd8\\xa7\\xd8\\xa6\\xd9\\x86", "تزامن_متقدم"},
                {"\\xd9\\x86\\xd9\\x88\\xd8\\xb9", "تزامن_متقدم"},
                {"classof", "تزامن_متقدم"},
                {"\\xd8\\xad\\xd9\\x82\\xd9\\x88\\xd9\\x84_\\xd8\\xa7\\xd9\\x84\\xd9\\x83\\xd8\\xa7\\xd8\\xa6\\xd9\\x86", "تزامن_متقدم"},
                {"fields", "تزامن_متقدم"},

                // ─── منصة ───
                {"\\xd8\\xa7\\xd9\\x84\\xd9\\x85\\xd9\\x86\\xd8\\xb5\\xd8\\xa9", "منصة"},
                {"\\xd9\\x87\\xd9\\x84_\\xd8\\xb3\\xd8\\xb7\\xd8\\xad_\\xd8\\xa7\\xd9\\x84\\xd9\\x85\\xd9\\x83\\xd8\\xaa\\xd8\\xa8", "منصة"},
                {"\\xd9\\x87\\xd9\\x84_\\xd8\\xa7\\xd9\\x86\\xd8\\xaf\\xd8\\xb1\\xd9\\x88\\xd9\\x8a\\xd8\\xaf", "منصة"},
                {"\\xd9\\x87\\xd9\\x84_\\xd9\\x88\\xd9\\x8a\\xd8\\xa8", "منصة"},

                // ─── مقابس ───
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_tcp_\\xd8\\xac\\xd8\\xaf\\xd9\\x8a\\xd8\\xaf", "مقابس"},
                {"tcp_new", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xa7\\xd8\\xaa\\xd8\\xb5\\xd8\\xa7\\xd9\\x84", "مقابس"},
                {"tcp_connect", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xa7\\xd8\\xb1\\xd8\\xb3\\xd8\\xa7\\xd9\\x84", "مقابس"},
                {"tcp_send", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd8\\xa7\\xd9\\x84", "مقابس"},
                {"tcp_recv", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xb1\\xd8\\xa8\\xd8\\xb7", "مقابس"},
                {"tcp_bind", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x85\\xd8\\xa7\\xd8\\xb9", "مقابس"},
                {"tcp_listen", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd9\\x82\\xd8\\xa8\\xd9\\x88\\xd9\\x84", "مقابس"},
                {"tcp_accept", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xa7\\xd8\\xba\\xd9\\x84\\xd8\\xa7\\xd9\\x82", "مقابس"},
                {"tcp_close", "مقابس"},
                {"socket_close", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd9\\x85\\xd8\\xaa\\xd8\\xb5\\xd9\\x84", "مقابس"},
                {"tcp_is_connected", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xb9\\xd9\\x86\\xd9\\x88\\xd8\\xa7\\xd9\\x86_\\xd8\\xa8\\xd8\\xb9\\xd9\\x8a\\xd8\\xaf", "مقابس"},
                {"tcp_peer_address", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_udp_\\xd8\\xac\\xd8\\xaf\\xd9\\x8a\\xd8\\xaf", "مقابس"},
                {"udp_new", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_udp_\\xd8\\xb1\\xd8\\xa8\\xd8\\xb7", "مقابس"},
                {"udp_bind", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_udp_\\xd8\\xa7\\xd8\\xb1\\xd8\\xb3\\xd8\\xa7\\xd9\\x84", "مقابس"},
                {"udp_send_to", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_udp_\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd8\\xa7\\xd9\\x84", "مقابس"},
                {"udp_recv_from", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_udp_\\xd8\\xa7\\xd8\\xba\\xd9\\x84\\xd8\\xa7\\xd9\\x82", "مقابس"},
                {"udp_close", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd9\\x85\\xd9\\x87\\xd9\\x84\\xd8\\xa9_\\xd8\\xa7\\xd8\\xb3\\xd8\\xaa\\xd9\\x82\\xd8\\xa8\\xd8\\xa7\\xd9\\x84", "مقابس"},
                {"socket_recv_timeout", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd9\\x85\\xd9\\x87\\xd9\\x84\\xd8\\xa9_\\xd8\\xa7\\xd8\\xb1\\xd8\\xb3\\xd8\\xa7\\xd9\\x84", "مقابس"},
                {"socket_send_timeout", "مقابس"},
                {"\\xd9\\x85\\xd9\\x82\\xd8\\xa8\\xd8\\xb3_\\xd8\\xa8\\xd8\\xaf\\xd9\\x88\\xd9\\x86_\\xd8\\xaa\\xd8\\xa3\\xd8\\xae\\xd9\\x8a\\xd8\\xb1", "مقابس"},
                {"socket_nodelay", "مقابس"},
            };

            auto it = funcToModule.find(funcName);
            if (it != funcToModule.end())
            {
                return it->second;
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
                                     {"strings", "string", "str",
                                      "\xd8\xb3\xd9\x84\xd8\xa7\xd8\xb3\xd9\x84"}, // سلاسل
                                     "\xd8\xaf\xd9\x88\xd8\xa7\xd9\x84 \xd8\xa7\xd9\x84\xd9\x86\xd8\xb5\xd9\x88\xd8\xb5 \xd9\x88\xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa7\xd8\xb3\xd9\x84",
                                     // دوال النصوص والسلاسل
                                     [](Interpreter &interp)
                                     { registerBuiltinsStrings(interp); },
                                     {}});

            // ─── أساسيات / Basics (Part 2) — exit, assert, range, file I/O basics ───
            registry.registerModule({"\xd8\xa3\xd8\xb3\xd8\xa7\xd8\xb3\xd9\x8a\xd8\xa7\xd8\xaa", // أساسيات
                                     {"basics", "basic", "base",
                                      "\xd9\x85\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa"}, // ملفات
                                     "\xd8\xae\xd8\xb1\xd9\x88\xd8\xac\xd8\x8c \xd9\x85\xd8\xaf\xd9\x89\xd8\x8c \xd8\xaa\xd8\xa3\xd9\x83\xd8\xaf\xd8\x8c \xd9\x85\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa",
                                     // خروج، مدى، تأكد، ملفات
                                     [](Interpreter &interp)
                                     { registerBuiltinsBasics(interp); },
                                     {}});

            // ─── رياضيات / Math (Part 6) — log, clamp, embedded IO ───
            registry.registerModule({"\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa7\xd8\xaa", // رياضيات
                                     {"math", "mathematics",
                                      "\xd8\xad\xd8\xb3\xd8\xa7\xd8\xa8"}, // حساب
                                     "\xd9\x84\xd9\x88\xd8\xba\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xaa\xd9\x85\xd8\x8c \xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd8\xaf\xd8\x8c \xd8\xaf\xd9\x88\xd8\xa7\xd9\x84 \xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa9",
                                     // لوغاريتم، تقييد، دوال رياضية
                                     [](Interpreter &interp)
                                     { registerBuiltinsMath(interp); },
                                     {}});

            // ─── تأكيدات / Assertions (Part 7) — assert, hash, crypto, FFI ───
            registry.registerModule({"\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf\xd8\xa7\xd8\xaa", // تأكيدات
                                     {"assertions", "assert", "testing",
                                      "\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1"}, // اختبار
                                     "\xd8\xaa\xd8\xa3\xd9\x83\xd8\xaf\xd8\x8c \xd9\x87\xd8\xa7\xd8\xb4\xd8\x8c \xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1\xd8\x8c FFI",
                                     // تأكد، هاش، تشفير، FFI
                                     [](Interpreter &interp)
                                     { registerBuiltinsAssertions(interp); },
                                     {}});

            // ─── تزامن / Async (Part 8) — spawn, await, channels, threads, atomics ───
            registry.registerModule({"\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // تزامن_متقدم
                                     {"async", "concurrency",
                                      "\xd8\xaa\xd9\x88\xd8\xa7\xd8\xb2\xd9\x8a"}, // توازي
                                     "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7\xd8\x8c \xd9\x82\xd9\x86\xd9\x88\xd8\xa7\xd8\xaa\xd8\x8c \xd8\xb0\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xaa\xd8\x8c OOP",
                                     // خيوط، قنوات، ذريات، OOP
                                     [](Interpreter &interp)
                                     { registerBuiltinsAsync(interp); },
                                     {}});

            // ─── خرائط / Maps & Advanced (Part 10) — maps, functional, date, regex, JSON ───
            registry.registerModule({"\xd8\xae\xd8\xb1\xd8\xa7\xd8\xa6\xd8\xb7", // خرائط
                                     {"maps", "functional", "json", "xml", "engine",
                                      "\xd9\x85\xd8\xad\xd8\xb1\xd9\x83",                                                   // محرك
                                      "\xd8\xaf\xd9\x88\xd8\xa7\xd9\x84_\xd9\x85\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85\xd8\xa9"}, // دوال_متقدمة
                                     "\xd8\xae\xd8\xb1\xd8\xa7\xd8\xa6\xd8\xb7\xd8\x8c JSON\xd8\x8c \xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae\xd8\x8c \xd8\xaa\xd8\xb9\xd8\xa8\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa \xd9\x86\xd9\x85\xd8\xb7\xd9\x8a\xd8\xa9",
                                     // خرائط، JSON، تاريخ، تعبيرات نمطية
                                     [](Interpreter &interp)
                                     { registerBuiltinsMaps(interp); },
                                     {}});

            // ─── منصة / Platform (Part 39) — platform detection ───
            registry.registerModule({"\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", // منصة
                                     {"platform",
                                      "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85_\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84"}, // نظام_تشغيل
                                     "\xd9\x83\xd8\xb4\xd9\x81 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9 \xd9\x88\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84",
                                     // كشف المنصة ونظام التشغيل
                                     [](Interpreter &interp)
                                     { registerBuiltinsPlatform(interp); },
                                     {}});

            // ─── شبكة_عالية / Network High-Level (Part 40) — sockets, HTTP ───
            registry.registerModule({"\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd8\xb9\xd8\xa7\xd9\x84\xd9\x8a\xd8\xa9", // شبكة_عالية
                                     {"http", "sockets_hl",
                                      "\xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa"}, // اتصالات
                                     "TCP\xd8\x8c HTTP\xd8\x8c \xd9\x85\xd9\x82\xd8\xa7\xd8\xa8\xd8\xb3 \xd8\xb9\xd8\xa7\xd9\x84\xd9\x8a\xd8\xa9",
                                     // TCP، HTTP، مقابس عالية
                                     [](Interpreter &interp)
                                     { registerBuiltinsSockets(interp); },
                                     {}});

            // ─── شبكة_لامركزية / SadNet Decentralized Network (Part 41) ───
            registry.registerModule({"\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x84\xd8\xa7\xd9\x85\xd8\xb1\xd9\x83\xd8\xb2\xd9\x8a\xd8\xa9", // شبكة_لامركزية
                                     {"sadnet", "p2p", "decentralized",
                                      "\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x85\xd9\x88\xd8\xb2\xd8\xb9\xd8\xa9"}, // شبكة_موزعة
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
                                     {"lowlevel", "os", "kernel",
                                      "\xd9\x86\xd9\x88\xd8\xa7\xd8\xa9"}, // نواة
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

            // ─── usb ───
            registry.registerModule({"usb",
                                     {"\xd9\x8a\xd9\x88_\xd8\xa5\xd8\xb3_\xd8\xa8\xd9\x8a", // يو_إس_بي
                                      "hid",
                                      "\xd8\xa3\xd8\xac\xd9\x87\xd8\xb2\xd8\xa9_\xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84"}, // أجهزة_إدخال
                                     "USB/HID\xd8\x8c \xd9\x84\xd9\x88\xd8\xad\xd8\xa9 \xd9\x85\xd9\x81\xd8\xa7\xd8\xaa\xd9\x8a\xd8\xad\xd8\x8c \xd9\x81\xd8\xa3\xd8\xb1\xd8\xa9",
                                     // USB/HID، لوحة مفاتيح، فأرة
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelUSB(interp);
                                     },
                                     {}});

            // ─── تخزين / Storage (Part 24) — NVMe, AHCI, S.M.A.R.T ───
            registry.registerModule({"\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // تخزين
                                     {"storage", "nvme", "ahci",
                                      "\xd8\xa3\xd9\x82\xd8\xb1\xd8\xa7\xd8\xb5"}, // أقراص
                                     "NVMe\xd8\x8c AHCI\xd8\x8c S.M.A.R.T\xd8\x8c \xd8\xaa\xd9\x82\xd8\xb3\xd9\x8a\xd9\x85\xd8\xa7\xd8\xaa",
                                     // NVMe، AHCI، S.M.A.R.T، تقسيمات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelStorage(interp);
                                     },
                                     {}});

            // ─── شبكة / Network Stack (Part 25) — Ethernet, ARP, IP, UDP ───
            registry.registerModule({"\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", // شبكة
                                     {"network", "net",
                                      "\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9_\xd9\x85\xd9\x86\xd8\xae\xd9\x81\xd8\xb6\xd8\xa9"}, // شبكة_منخفضة
                                     "Ethernet\xd8\x8c ARP\xd8\x8c IPv4\xd8\x8c ICMP\xd8\x8c UDP",
                                     // Ethernet، ARP، IPv4، ICMP، UDP
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelNetwork(interp);
                                     },
                                     {}});

            // ─── صوت / Audio (Part 26) — AC'97, HDA, PC Speaker ───
            registry.registerModule({"\xd8\xb5\xd9\x88\xd8\xaa", // صوت
                                     {"audio",
                                      "\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x85\xd9\x86\xd8\xae\xd9\x81\xd8\xb6"}, // صوت_منخفض
                                     "AC'97\xd8\x8c HDA\xd8\x8c PC Speaker\xd8\x8c \xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84",
                                     // AC'97، HDA، PC Speaker، تسجيل
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelAudio(interp);
                                     },
                                     {}});

            // ─── مؤقتات / Timers (Part 27) — PIT, RTC, Stopwatch ───
            registry.registerModule({"\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // مؤقتات
                                     {"timers", "timer", "rtc",
                                      "\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9"}, // ساعة
                                     "PIT 8254\xd8\x8c RTC\xd8\x8c \xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9 \xd8\xa5\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81",
                                     // PIT 8254، RTC، ساعة إيقاف
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelTimers(interp);
                                     },
                                     {}});

            // ─── تسلسلي / Serial UART (Part 28) — COM1-COM4 ───
            registry.registerModule({"\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a", // تسلسلي
                                     {"serial", "uart", "com",
                                      "\xd9\x85\xd9\x86\xd8\xa7\xd9\x81\xd8\xb0_\xd8\xaa\xd8\xb3\xd9\x84\xd8\xb3\xd9\x84\xd9\x8a\xd8\xa9"}, // منافذ_تسلسلية
                                     "UART 16550\xd8\x8c COM1-COM4\xd8\x8c loopback",
                                     // UART 16550، COM1-COM4، loopback
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelSerial(interp);
                                     },
                                     {}});

            // ─── ذاكرة / Memory (Part 35) — Allocator, GC ───
            registry.registerModule({"\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9", // ذاكرة
                                     {"memory", "mem",
                                      "\xd8\xaa\xd8\xae\xd8\xb5\xd9\x8a\xd8\xb5",  // تخصيص
                                      "\xd9\x82\xd9\x85\xd8\xa7\xd9\x85\xd8\xa9"}, // قمامة
                                     "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5 \xd8\xa7\xd9\x84\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9\xd8\x8c \xd8\xac\xd8\xa7\xd9\x85\xd8\xb9 \xd8\xa7\xd9\x84\xd9\x82\xd9\x85\xd8\xa7\xd9\x85\xd8\xa9\xd8\x8c \xd8\xa5\xd8\xad\xd8\xb5\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa7\xd8\xaa",
                                     // مخصص الذاكرة، جامع القمامة، إحصائيات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelMemory(interp);
                                     },
                                     {}});

            // ─── خيوط / Threads (Part 36) — Thread, Mutex, Semaphore ───
            registry.registerModule({"\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7", // خيوط
                                     {"threads", "thread",
                                      "\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86",  // تزامن
                                      "\xd8\xa3\xd9\x82\xd9\x81\xd8\xa7\xd9\x84"}, // أقفال
                                     "\xd8\xae\xd9\x8a\xd9\x88\xd8\xb7\xd8\x8c \xd8\xa3\xd9\x82\xd9\x81\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa5\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa \xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86",
                                     // خيوط، أقفال، إشارات تزامن
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsKernelThreads(interp);
                                     },
                                     {}});

            // ─── واجهة_خارجية / FFI (Part 37) — Dynamic library loading ───
            registry.registerModule({"\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9_\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a\xd8\xa9", // واجهة_خارجية
                                     {"ffi", "foreign",
                                      "\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xaa_\xd8\xae\xd8\xa7\xd8\xb1\xd8\xac\xd9\x8a\xd8\xa9"}, // مكتبات_خارجية
                                     "FFI\xd8\x8c \xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84 \xd9\x85\xd9\x83\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xaa C\xd8\x8c \xd8\xa7\xd8\xb3\xd8\xaa\xd8\xaf\xd8\xb9\xd8\xa7\xd8\xa1 \xd8\xaf\xd9\x88\xd8\xa7\xd9\x84",
                                     // FFI، تحميل مكتبات C، استدعاء دوال
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsFFI(interp);
                                     },
                                     {}});

            // ─── استثناءات / Exceptions & Sandbox (Part 38) ───
            registry.registerModule({"\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa", // استثناءات
                                     {"exceptions", "errors",
                                      "\xd8\xa3\xd9\x85\xd8\xa7\xd9\x86", // أمان
                                      "sandbox"},
                                     "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1\xd8\xa7\xd8\xaa\xd8\x8c \xd8\xaa\xd8\xaa\xd8\xa8\xd8\xb9 \xd9\x85\xd9\x83\xd8\xaf\xd8\xb3\xd8\x8c \xd8\xa3\xd8\xb0\xd9\x88\xd9\x86\xd8\xa7\xd8\xaa",
                                     // استثناءات، تتبع مكدس، أذونات
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsExceptions(interp);
                                     },
                                     {}});

            // ─── مقابس / Sockets (Part 40) — TCP & UDP عالي المستوى ───
            registry.registerModule({"\xd9\x85\xd9\x82\xd8\xa7\xd8\xa8\xd8\xb3", // مقابس
                                     {"sockets", "socket", "tcp", "udp",
                                      "\xd9\x85\xd9\x82\xd8\xa8\xd8\xb3"}, // مقبس
                                     "TCP\xd8\x8c UDP\xd8\x8c \xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa5\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\x8c \xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd8\xa7\xd9\x84",
                                     // TCP، UDP، اتصال، إرسال، استقبال
                                     [](Interpreter &interp)
                                     {
                                         registerBuiltinsSockets(interp); // TCP & UDP high-level sockets
                                     },
                                     {}});

            // ─── تشفير / Crypto (Part 7 crypto functions) — SHA-256, تشفير/فك_تشفير ───
            registry.registerModule({"\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // تشفير
                                     {"crypto", "encryption",
                                      "\xd8\xa3\xd9\x85\xd8\xa7\xd9\x86_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa"}, // أمان_بيانات
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
            registry.registerModule({"\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa",                      // رسومات
                                     {"graphics", "\xd8\xb1\xd8\xb3\xd9\x88\xd9\x85\xd9\x8a\xd8\xa7\xd8\xaa", // رسوميات
                                      "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa7\xd8\xaa"},                    // واجهات
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
                                     {"_ui_engine"},
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
            registry.registerModule({"_\xd8\xb5\xd9\x88\xd8\xaa", // _صوت
                                     {"_audio"},
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb5\xd9\x88\xd8\xaa", // نظام الصوت
                                     [](Interpreter &interp)
                                     { registerUIAudioBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // _مؤقتات
                                     {"_timers"},
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa", // نظام المؤقتات
                                     [](Interpreter &interp)
                                     { registerUITimerBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // _تخزين
                                     {"_storage"},
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xae\xd8\xb2\xd9\x8a\xd9\x86", // نظام التخزين
                                     [](Interpreter &interp)
                                     { registerUIStorageBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", // _شبكة
                                     {"_network"},
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", // نظام الشبكة
                                     [](Interpreter &interp)
                                     { registerUINetworkBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // _تشفير
                                     {"_crypto"},
                                     "\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", // نظام التشفير
                                     [](Interpreter &interp)
                                     { registerUICryptoBuiltins(interp); },
                                     {}});

            registry.registerModule({"_\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", // _منصة
                                     {"_platform"},
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
            //   استورد شبكة_عالية    — TCP، HTTP، مقابس (Part 40)
            // ═══════════════════════════════════════════════════════════════
            auto &registry = BuiltinModuleRegistry::getInstance();
            registry.setInterpreter(&interpreter);
            registerBuiltinModules();
        }

    } // namespace Interpreter
} // namespace Sad
