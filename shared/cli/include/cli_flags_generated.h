// ============================================================================
// AUTO-GENERATED FROM language-truth/cli_flags.yaml — DO NOT EDIT MANUALLY
// (AR) ملف مولَّد تلقائياً. لإجراء تعديل، عدّل YAML وأعد التوليد (x.py gen).
// (EN) Auto-generated file. To modify, edit YAML and regenerate (x.py gen).
//
// (AR) نسخة الأعلام: 1.0
// ============================================================================

#pragma once

#include <cstddef>
#include <cstdint>

namespace sad {
namespace cli {

/**
 * @brief (AR) نوع العلم: منطقيّ (تطابق تامّ) أو قيميّ (الاسم أو الاسم=قيمة).
 * @brief (EN) Flag kind: bool (exact) or value (name or name=value).
 */
enum class FlagKind : std::uint8_t {
    Bool  = 0,
    Value = 1
};

/**
 * @brief (AR) وسم الإجراء الذي يميّز سلوك كلّ علم في مُوزِّع المحلِّل.
 * @brief (EN) Action tag discriminating each flag's behavior in the parser.
 */
enum class FlagAction : std::uint16_t {
    NoMain = 0,
    AbortOnPanic = 1,
    AllowAlloc = 2,
    LinkerScript = 3,
    EntryPoint = 4,
    ModuleMode = 5,
    LinkStatic = 6,
    OutShared = 7,
    Lto = 8,
    LtoNone = 9,
    Target = 10,
    EmitLlvm = 11,
    EmitBc = 12,
    EmitAst = 13,
    EmitAstJson = 14,
    EmitSir = 15,
    TimePasses = 16,
    Explain = 17,
    OutputLanguage = 18,
    ColorOn = 19,
    ColorOff = 20,
    Verbose = 21,
    BorrowCheckOn = 22,
    BorrowCheckOff = 23,
    DebugBorrow = 24,
    BorrowMsgArabic = 25,
    BorrowMsgEnglish = 26,
    TypeCheckOn = 27,
    TypeCheckOff = 28,
    DebugTypes = 29,
    StrictTypes = 30,
    EmitUi = 31,
    UiPlatform = 32,
    PlatformDesktop = 33,
    PlatformAndroid = 34,
    PlatformIos = 35,
    PlatformWeb = 36,
    EmitDocs = 37,
    DocsOut = 38,
    DocsProject = 39,
    DocsProjectName = 40,
    DocsFormat = 41,
    DocsExclude = 42,
    HelpRequested = 43,
    VersionRequested = 44,
    Freestanding = 45,
    Gc = 46,
    Ownership = 47,
    ProductionMode = 48,
    LearningMode = 49,
    AutoMode = 50,
    Suggestions = 51,
    DetectCycles = 52,
    MemoryLimit = 53,
    DebugMemory = 54,
    Security = 55,
    DebugSecurity = 56,
    StrictSecurity = 57,
    DebugMode = 58,
    DebugServer = 59,
    OptStats = 60,
    Profile = 61,
    ProfileFormat = 62,
    ProfileOutput = 63,
    ProfileTop = 64,
    HotReload = 65,
};

/**
 * @brief (AR) مواصفة علم واحد. كلّ الأسماء تأتي من المصدر الوحيد (لا سلاسل حرّة).
 * @brief (EN) A single flag spec. All names come from the single source of truth.
 */
struct FlagSpec {
    const char* canonical;   ///< (AR) الاسم العربيّ القانونيّ الوحيد (يبدأ بـ --)
    FlagKind    kind;        ///< (AR) منطقيّ أم قيميّ
    FlagAction  action;      ///< (AR) وسم الإجراء
    const char* value_hint;  ///< (AR) تلميح القيمة (فارغ للأعلام المنطقيّة)
    const char* desc_ar;     ///< (AR) الوصف العربيّ
    const char* desc_en;     ///< (EN) English description
    bool for_compiler;       ///< (AR) يستهلكه محلِّل المترجم (sad-build)
    bool for_interpreter;    ///< (AR) يستهلكه محلِّل المفسّر (sad-run)
    bool for_memory;         ///< (AR) يستهلكه ماسح سياسة الذاكرة المسبق (المحرّكان)
};

/**
 * @brief (AR) جدول كلّ الأعلام الطويلة (المصدر الوحيد).
 */
inline constexpr FlagSpec kFlags[] = {
    { "--بلا-رئيسية", FlagKind::Bool, FlagAction::NoMain, "", "تعطيل نقطة الدخول الافتراضية main (لتعريف _start مخصّص)", "Disable default main() entry point", true, false, false },
    { "--إجهاض-عند-الذعر", FlagKind::Bool, FlagAction::AbortOnPanic, "", "الإجهاض عند الذعر بدل فكّ المكدّس (بلا استثناءات C++)", "Abort on panic instead of stack unwinding", true, false, false },
    { "--اسمح-بالتخصيص", FlagKind::Bool, FlagAction::AllowAlloc, "", "السماح بالتخصيص الديناميكي في الوضع الحرّ (يتطلّب مُخصِّصًا مخصّصًا)", "Allow dynamic allocation in freestanding mode", true, false, false },
    { "--سكربت-الرابط", FlagKind::Value, FlagAction::LinkerScript, "ملفّ.ld", "مسار سكربت الرابط (.ld) لتعريف تخطيط الذاكرة", "Linker script path (.ld) for memory layout", true, false, false },
    { "--نقطة-الدخول", FlagKind::Value, FlagAction::EntryPoint, "اسم_الدالة", "اسم دالة نقطة الدخول المخصّصة", "Custom entry point function name", true, false, false },
    { "--وحدة", FlagKind::Bool, FlagAction::ModuleMode, "", "وضع الوحدة: يُعامَل الملفّ كمكتبة بلا نقطة دخول", "Module mode: treat file as a library without entry point", true, false, false },
    { "--ساكن", FlagKind::Bool, FlagAction::LinkStatic, "", "ربط ساكن للمكتبات", "Static linking", true, false, false },
    { "--مشترك", FlagKind::Bool, FlagAction::OutShared, "", "إخراج مكتبة مشتركة", "Emit a shared library", true, false, false },
    { "--تحسين-الربط", FlagKind::Value, FlagAction::Lto, "كامل|نحيف", "تحسين وقت الربط؛ بلا قيمة=كامل، أو =نحيف لبناء أسرع", "Link-time optimization; bare=full, or =thin for faster builds", true, false, false },
    { "--بلا-تحسين-ربط", FlagKind::Bool, FlagAction::LtoNone, "", "تعطيل تحسين وقت الربط", "Disable link-time optimization", true, false, false },
    { "--هدف", FlagKind::Value, FlagAction::Target, "ثالوث-الهدف", "ثالوث الهدف (مثل x86_64-unknown-none-elf)", "Target triple", true, false, false },
    { "--أظهر-llvm", FlagKind::Bool, FlagAction::EmitLlvm, "", "إظهار تمثيل LLVM الوسيط النصّيّ", "Emit textual LLVM IR", true, false, false },
    { "--أظهر-bc", FlagKind::Bool, FlagAction::EmitBc, "", "إظهار bitcode الخاصّ بـ LLVM", "Emit LLVM bitcode", true, false, false },
    { "--أظهر-الشجرة", FlagKind::Bool, FlagAction::EmitAst, "", "إظهار شجرة النحو المجرّدة (AST)", "Emit the abstract syntax tree", true, false, false },
    { "--أظهر-الشجرة-json", FlagKind::Bool, FlagAction::EmitAstJson, "", "إظهار شجرة النحو المجرّدة بصيغة JSON", "Emit the AST as JSON", true, false, false },
    { "--أظهر-sir", FlagKind::Bool, FlagAction::EmitSir, "", "إظهار التمثيل الوسيط للغة ص (SIR)", "Emit Sad IR (SIR)", true, false, false },
    { "--توقيت-المراحل", FlagKind::Bool, FlagAction::TimePasses, "", "قياس زمن مراحل الترجمة", "Time compilation passes", true, false, false },
    { "--اشرح", FlagKind::Value, FlagAction::Explain, "مستوى", "شرح الأخطاء؛ بلا قيمة=مفصّل، أو =مستوى محدّد", "Explain errors; bare=detailed, or =level", true, true, false },
    { "--لغة", FlagKind::Value, FlagAction::OutputLanguage, "ar|en", "لغة رسائل المترجم", "Compiler messages language", true, true, false },
    { "--ألوان", FlagKind::Bool, FlagAction::ColorOn, "", "تلوين التشخيصات", "Colorize diagnostics", true, false, false },
    { "--بلا-ألوان", FlagKind::Bool, FlagAction::ColorOff, "", "تعطيل تلوين التشخيصات", "Disable colorized diagnostics", true, false, false },
    { "--مُسهب", FlagKind::Bool, FlagAction::Verbose, "", "إخراج مُسهب", "Verbose output", true, false, false },
    { "--فحص-الاستعارة", FlagKind::Bool, FlagAction::BorrowCheckOn, "", "تفعيل فاحص الملكية والاستعارة", "Enable the ownership & borrow checker", true, true, false },
    { "--بلا-فحص-استعارة", FlagKind::Bool, FlagAction::BorrowCheckOff, "", "تعطيل فاحص الملكية والاستعارة", "Disable the borrow checker", true, false, false },
    { "--تنقيح-الاستعارة", FlagKind::Bool, FlagAction::DebugBorrow, "", "إخراج تنقيح فاحص الاستعارة", "Debug borrow-checker output", true, true, false },
    { "--استعارة-عربية", FlagKind::Bool, FlagAction::BorrowMsgArabic, "", "رسائل فاحص الاستعارة بالعربيّة", "Borrow-checker messages in Arabic", true, false, false },
    { "--استعارة-إنجليزية", FlagKind::Bool, FlagAction::BorrowMsgEnglish, "", "رسائل فاحص الاستعارة بالإنجليزيّة", "Borrow-checker messages in English", true, false, false },
    { "--فحص-الأنواع", FlagKind::Bool, FlagAction::TypeCheckOn, "", "تفعيل فحص الأنواع المتقدّم", "Enable advanced type checking", true, true, false },
    { "--بلا-فحص-أنواع", FlagKind::Bool, FlagAction::TypeCheckOff, "", "تعطيل فحص الأنواع المتقدّم", "Disable advanced type checking", true, false, false },
    { "--تنقيح-الأنواع", FlagKind::Bool, FlagAction::DebugTypes, "", "إخراج تنقيح فاحص الأنواع", "Debug type-checker output", true, true, false },
    { "--أنواع-صارمة", FlagKind::Bool, FlagAction::StrictTypes, "", "وضع صارم لفحص الأنواع", "Strict type-checking mode", true, true, false },
    { "--واجهات", FlagKind::Bool, FlagAction::EmitUi, "", "تفعيل وضع توليد الواجهات الرسومية", "Enable UI code generation", true, false, false },
    { "--منصة", FlagKind::Value, FlagAction::UiPlatform, "desktop|android|ios|web", "منصّة الواجهة الهدف (يُفعِّل وضع الواجهات)", "Target UI platform (enables UI mode)", true, false, false },
    { "--سطح-المكتب", FlagKind::Bool, FlagAction::PlatformDesktop, "", "واجهة سطح المكتب", "Desktop UI platform", true, false, false },
    { "--أندرويد", FlagKind::Bool, FlagAction::PlatformAndroid, "", "واجهة أندرويد (يضبط الهدف تلقائيًّا)", "Android UI platform (auto-sets target)", true, false, false },
    { "--ايفون", FlagKind::Bool, FlagAction::PlatformIos, "", "واجهة iOS", "iOS UI platform", true, false, false },
    { "--ويب", FlagKind::Bool, FlagAction::PlatformWeb, "", "واجهة الويب", "Web UI platform", true, false, false },
    { "--وثّق", FlagKind::Bool, FlagAction::EmitDocs, "", "استخراج توثيق Markdown من الشجرة (طباعة على المخرَج القياسيّ)", "Extract Markdown documentation (to stdout)", true, true, false },
    { "--وثّق-إلى", FlagKind::Value, FlagAction::DocsOut, "ملفّ", "كتابة التوثيق إلى ملفّ", "Write documentation to a file", true, true, false },
    { "--وثّق-مشروع", FlagKind::Value, FlagAction::DocsProject, "مجلّد", "توثيق مشروع متعدّد الملفّات", "Document a multi-file project", true, true, false },
    { "--اسم-المشروع", FlagKind::Value, FlagAction::DocsProjectName, "اسم", "اسم المشروع المعروض في عنوان التوثيق", "Project name shown in docs title", true, true, false },
    { "--صيغة-التوثيق", FlagKind::Value, FlagAction::DocsFormat, "markdown|json|html", "صيغة إخراج التوثيق", "Documentation output format", true, true, false },
    { "--استثنِ", FlagKind::Value, FlagAction::DocsExclude, "مسار", "استثناء مسار من التوثيق (يمكن تكراره)", "Exclude a path from docs (repeatable)", true, true, false },
    { "--مساعدة", FlagKind::Bool, FlagAction::HelpRequested, "", "عرض المساعدة", "Show help", true, true, false },
    { "--إصدار", FlagKind::Bool, FlagAction::VersionRequested, "", "عرض إصدار المترجم", "Show compiler version", true, true, false },
    { "--حرّ", FlagKind::Bool, FlagAction::Freestanding, "", "الوضع الحرّ: بلا مكتبة قياسية (يفرض وضع النواة: بلا جامع، ملكية صارمة)", "Freestanding: no standard library (forces kernel mode)", false, false, true },
    { "--جامع", FlagKind::Value, FlagAction::Gc, "استراتيجية", "جامع القمامة؛ بلا قيمة=وضع الجمع الكامل، أو =استراتيجية", "Garbage collector; bare=full GC mode, or =strategy", false, false, true },
    { "--ملكية", FlagKind::Value, FlagAction::Ownership, "off|warnings|strict|ultra", "مستوى فحص الملكية", "Ownership checking level", false, false, true },
    { "--إنتاج", FlagKind::Bool, FlagAction::ProductionMode, "", "وضع الإنتاج: ملكية صارمة (بلا جامع)", "Production mode: strict ownership (no GC)", false, false, true },
    { "--تعلم", FlagKind::Bool, FlagAction::LearningMode, "", "وضع التعلّم: رسائل تعليمية مفصّلة عن إدارة الذاكرة", "Learning mode: detailed educational memory messages", false, false, true },
    { "--تلقائي", FlagKind::Bool, FlagAction::AutoMode, "", "اكتشاف تلقائيّ لأفضل وضع ذاكرة", "Auto-detect the best memory mode", false, false, true },
    { "--اقتراحات", FlagKind::Bool, FlagAction::Suggestions, "", "تفعيل اقتراحات تحويل الملكية", "Enable ownership conversion suggestions", false, false, true },
    { "--كشف-دورات", FlagKind::Bool, FlagAction::DetectCycles, "", "تفعيل كشف دورات المراجع", "Enable reference-cycle detection", false, false, true },
    { "--حد-الذاكرة", FlagKind::Value, FlagAction::MemoryLimit, "ميغابايت", "حدّ ذاكرة الجامع بالميغابايت", "GC memory limit in megabytes", false, false, true },
    { "--تصحيح-الذاكرة", FlagKind::Bool, FlagAction::DebugMemory, "", "تفعيل رسائل تصحيح الذاكرة", "Enable memory debug messages", false, false, true },
    { "--أمان", FlagKind::Bool, FlagAction::Security, "", "تفعيل فاحص الأمان", "Enable the security checker", false, true, false },
    { "--تنقيح-الأمان", FlagKind::Bool, FlagAction::DebugSecurity, "", "إخراج تنقيح فاحص الأمان (يُفعّل الفحص)", "Debug security checker output (enables checking)", false, true, false },
    { "--أمان-صارم", FlagKind::Bool, FlagAction::StrictSecurity, "", "وضع صارم لفحص الأمان", "Strict security-checking mode", false, true, false },
    { "--تنقيح", FlagKind::Bool, FlagAction::DebugMode, "", "تفعيل وضع التنقيح", "Enable debug mode", false, true, false },
    { "--خادم-التنقيح", FlagKind::Bool, FlagAction::DebugServer, "", "تشغيل خادم التنقيح", "Run the debug server", false, true, false },
    { "--إحصاء-التحسين", FlagKind::Bool, FlagAction::OptStats, "", "عرض إحصاءات التحسين", "Show optimization statistics", false, true, false },
    { "--تنميط", FlagKind::Bool, FlagAction::Profile, "", "تفعيل مصحّح الأداء (التنميط)", "Enable the profiler", false, true, false },
    { "--صيغة-التنميط", FlagKind::Value, FlagAction::ProfileFormat, "صيغة", "صيغة إخراج التنميط", "Profiler output format", false, true, false },
    { "--مخرج-التنميط", FlagKind::Value, FlagAction::ProfileOutput, "ملفّ", "ملفّ إخراج التنميط", "Profiler output file", false, true, false },
    { "--أعلى-التنميط", FlagKind::Value, FlagAction::ProfileTop, "عدد", "عدد أعلى الدوالّ في تقرير التنميط", "Top-N functions in the profile report", false, true, false },
    { "--مراقبة", FlagKind::Bool, FlagAction::HotReload, "", "إعادة التحميل الساخن (مراقبة الملفّ)", "Hot reload (watch the file)", false, true, false },
};

/**
 * @brief (AR) عدد الأعلام (compile-time).
 */
inline constexpr std::size_t kFlagCount = 66;

// ────────────────────────────────────────────────────────────────────────────
// (AR) أعلام قصيرة قياسيّة (أعراف سلسلة أدوات، خارج التسمية العربيّة القانونيّة).
//      تُولَّد أسماؤها ثوابتَ كي لا تبقى سلاسل حرّة في المحلِّل.
// ────────────────────────────────────────────────────────────────────────────
namespace short_flags {
    inline constexpr const char* OptO0 = "-O0";
    inline constexpr const char* OptO1 = "-O1";
    inline constexpr const char* OptO2 = "-O2";
    inline constexpr const char* OptO3 = "-O3";
    inline constexpr const char* OptOs = "-Os";
    inline constexpr const char* OptOz = "-Oz";
    inline constexpr const char* CompileOnly = "-c";
    inline constexpr const char* EmitAsm = "-S";
    inline constexpr const char* DebugInfo = "-g";
    inline constexpr const char* VerboseShort = "-v";
    inline constexpr const char* WarningsAsErrors = "-Werror";
    inline constexpr const char* LibPathPrefix = "-L";
    inline constexpr const char* LibNamePrefix = "-l";
    inline constexpr const char* OutputFile = "-o";
    inline constexpr const char* TargetShort = "-T";
    inline constexpr const char* HelpShort = "-h";
} // namespace short_flags

/**
 * @brief (AR) مواصفة علم قصير (للمساعدة).
 */
struct ShortFlagSpec {
    const char* name;
    const char* desc_ar;
    const char* desc_en;
};

inline constexpr ShortFlagSpec kShortFlags[] = {
    { "-O0", "بلا تحسين", "No optimization" },
    { "-O1", "تحسين أساسيّ", "Basic optimization" },
    { "-O2", "تحسين عاديّ (افتراضيّ)", "Standard optimization (default)" },
    { "-O3", "تحسين أقصى", "Aggressive optimization" },
    { "-Os", "تحسين الحجم", "Size optimization" },
    { "-Oz", "تحسين الحجم الأقصى", "Aggressive size optimization" },
    { "-c", "ترجمة فقط (بلا ربط)", "Compile only (no linking)" },
    { "-S", "إنتاج تجميعة", "Emit assembly" },
    { "-g", "معلومات تنقيح", "Generate debug info" },
    { "-v", "إخراج مُسهب", "Verbose output" },
    { "-Werror", "التحذيرات كأخطاء", "Treat warnings as errors" },
    { "-L", "مسار بحث المكتبات (بادئة)", "Library search path (prefix)" },
    { "-l", "ربط مكتبة (بادئة)", "Link library (prefix)" },
    { "-o", "ملفّ الإخراج (يبتلع الوسيط التالي)", "Output file (consumes next arg)" },
    { "-T", "ثالوث الهدف (يبتلع الوسيط التالي)", "Target triple (consumes next arg)" },
    { "-h", "عرض المساعدة", "Show help" },
};

inline constexpr std::size_t kShortFlagCount = 16;

// ────────────────────────────────────────────────────────────────────────────
// (AR) قيم قانونيّة للأعلام القيميّة — ثوابت بدل السلاسل الحرّة.
// ────────────────────────────────────────────────────────────────────────────
namespace values {
    inline constexpr const char* LtoFull = "كامل";
    inline constexpr const char* LtoThin = "نحيف";
    inline constexpr const char* ExplainDefault = "detailed";
    inline constexpr const char* PlatformDesktop = "desktop";
    inline constexpr const char* PlatformAndroid = "android";
    inline constexpr const char* PlatformIos = "ios";
    inline constexpr const char* PlatformWeb = "web";
    inline constexpr const char* AndroidTargetTriple = "aarch64-linux-android24";
} // namespace values

// ────────────────────────────────────────────────────────────────────────────
// (AR) نصوص تشخيصات المحلِّل — ثوابت بدل السلاسل الحرّة.
// ────────────────────────────────────────────────────────────────────────────
namespace messages {
    inline constexpr const char* RequiresValue = " — يتطلّب قيمة / requires a value";
    inline constexpr const char* UnknownValue = ": قيمة غير معروفة / unknown value: ";
    inline constexpr const char* InvalidTargetTriple = "ثالوث هدف غير صالح / invalid target triple: ";
    inline constexpr const char* UnknownOptionPrefix = "خيار غير معروف / unknown option: ";
    inline constexpr const char* RequiresArgument = " — يتطلّب وسيطًا / requires an argument";
    inline constexpr const char* UnimplementedAction = ": إجراء غير مُنفَّذ في الموزِّع / unimplemented flag action";
    inline constexpr const char* FreestandingHostTargetHosted = "الوضع الحرّ بلا «--هدف»: افتُرض ثالوث المضيف (بلا واجهة نداءات نظام مبثوثة)، فلن تُبثّ جسور العتاد (ساعة CMOS، المنفذ التسلسليّ) ولا يصلح الناتج للمعدن العاري ولا لربط «-nostdlib». مرّر «--هدف=i686-unknown-elf» أو نظيره لبناء نواة / freestanding without «--هدف»: assumed the host triple (no inline syscall ABI), so hardware bridges (CMOS clock, serial port) are not emitted and the output is neither bare-metal nor -nostdlib capable. Pass «--هدف=i686-unknown-elf» or similar to build a kernel: ";
    inline constexpr const char* FreestandingHostTargetLinux = "الوضع الحرّ بلا «--هدف»: افتُرض لينكس نطاقَ مستخدم (تُبثّ نداءات النظام، ويُربط «-nostdlib»). إن قصدت نواة معدنيّة فمرّر «--هدف=i686-unknown-elf» أو نظيره / freestanding without «--هدف»: assumed Linux userspace (syscalls are emitted, -nostdlib links). For a bare-metal kernel pass «--هدف=i686-unknown-elf» or similar: ";
    inline constexpr const char* FreestandingBareMetalStubBridges = "الوضع الحرّ على معدن عارٍ بمعمارية بلا منافذ معزولة ولا نداء نظام مبثوث: جسرا الإخراج (المنفذ التسلسليّ) والوقت (الساعة) كعبان يعيدان لا شيء/صفرًا. عرِّف «__sad_serial_putc» و«time» في حزمة دعم اللوحة (BSP) وإلّا فلا مخرجات زمن التشغيل / freestanding on bare metal with no isolated ports and no inline syscall: the output (serial) and time (clock) bridges are stubs returning nothing/zero. Define «__sad_serial_putc» and «time» in a board-support package (BSP), otherwise there is no runtime output: ";
} // namespace messages

} // namespace cli
} // namespace sad
