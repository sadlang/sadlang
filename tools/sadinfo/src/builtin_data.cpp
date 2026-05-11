// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/src/builtin_data.cpp
// (AR) جداول الحقيقة للدوال المدمجة auto-loaded (~21) والطرق المضمنة على
//      الأنواع. مرجع الأسماء العربية: shared/builtins/include/builtin_registry.h
//      (Names::Core, Names::TypeCtor, Names::Concurrency) — أسماء الطرق على
//      المصفوفات/النصوص/الخرائط/القنوات مأخوذة من قسم "الدوال المضمنة تلقائياً"
//      في .github/copilot-instructions.md، وهي مطابقة للسلوك في المفسر.
//
// (AR) القاعدة: لا تُغيّر ترتيب الإدخالات بدون تحديث الـ golden snapshots
//      في tests/sadinfo/golden/builtin_*.json. أي إضافة/تعديل = bump تطبيقي.
//
// (EN) Truth tables for auto-loaded built-in functions (~21) and methods on
//      host types. Source for Arabic names: shared/builtins registry.
//      DO NOT change entry order without updating the golden snapshots.
// ═══════════════════════════════════════════════════════════════════════════════

#include "builtin_data.h"

namespace Sad
{
    namespace Info
    {

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) ترجمة الفئات إلى ASCII
        // (EN) Category to ASCII string
        // ───────────────────────────────────────────────────────────────────────────────
        std::string_view categoryName(BuiltinCategory c)
        {
            switch (c)
            {
            case BuiltinCategory::Printing:    return "printing";
            case BuiltinCategory::Input:       return "input";
            case BuiltinCategory::Reflection:  return "reflection";
            case BuiltinCategory::TypeCtor:    return "type_ctor";
            case BuiltinCategory::Concurrency: return "concurrency";
            }
            return "unknown";
        }

        std::string_view hostTypeName(BuiltinHostType h)
        {
            switch (h)
            {
            case BuiltinHostType::Array:   return "array";
            case BuiltinHostType::String:  return "string";
            case BuiltinHostType::Map:     return "map";
            case BuiltinHostType::Channel: return "channel";
            }
            return "unknown";
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) جدول الدوال المدمجة (~21) — auto-loaded بدون استيراد
        // (EN) Auto-loaded built-in functions table (~21)
        //
        // (AR) القائمة المُعتمدة (مطابقة لـ .github/copilot-instructions.md):
        //   إخراج (2):   اطبع، اطبع_سطر
        //   إدخال (1):   اقرأ
        //   انعكاس (2):  طول، نوع
        //   بانيات (4):  رقم، عشري، نص، منطقي
        //   تزامن (6):   قناة، انتظر_الكل، عدد_المهام، مجموعة_انتظار، قفل، مستقبل
        //   ─────────────────
        //   المجموع: 15
        //
        // (AR) ملاحظة: المهارات تذكر "~21" لأنها تشمل أسماء بديلة أو طرقاً
        //      مساعدة — نحن نتمسك بالأسماء القانونية (canonical) فقط.
        // ───────────────────────────────────────────────────────────────────────────────
        const std::vector<BuiltinFunction>& allBuiltinFunctions()
        {
            static const std::vector<BuiltinFunction> table = {
                // ── إخراج / Printing ──────────────────────────────────────────
                {
                    "print", "اطبع", "print",
                    BuiltinCategory::Printing, "فراغ",
                    "(AR) يطبع قيمة أو أكثر إلى الإخراج القياسي بدون سطر جديد.",
                    "(EN) Print one or more values to stdout without newline.",
                    {{"قيم", "values", "أي", false, true}},
                    "اطبع(\"س = \"، س)"
                },
                {
                    "println", "اطبع_سطر", "println",
                    BuiltinCategory::Printing, "فراغ",
                    "(AR) يطبع قيمة أو أكثر متبوعةً بسطر جديد.",
                    "(EN) Print one or more values followed by newline.",
                    {{"قيم", "values", "أي", false, true}},
                    "اطبع_سطر(\"مرحباً\")"
                },

                // ── إدخال / Input ─────────────────────────────────────────────
                {
                    "read", "اقرأ", "read",
                    BuiltinCategory::Input, "نص",
                    "(AR) يقرأ سطراً من الدخل القياسي مع رسالة اختيارية.",
                    "(EN) Read a line from stdin with optional prompt.",
                    {{"رسالة", "prompt", "نص", true, false}},
                    "متغير الاسم = اقرأ(\"اسمك؟ \")"
                },

                // ── انعكاس / Reflection ───────────────────────────────────────
                {
                    "length", "طول", "length",
                    BuiltinCategory::Reflection, "رقم",
                    "(AR) يرجع طول قيمة (نص، مصفوفة، خريطة).",
                    "(EN) Return length of a value (string, array, map).",
                    {{"قيمة", "value", "أي", false, false}},
                    "طول([1، 2، 3])"
                },
                {
                    "type", "نوع", "type",
                    BuiltinCategory::Reflection, "نص",
                    "(AR) يرجع اسم نوع القيمة كنص (مثال: \"رقم\"، \"نص\").",
                    "(EN) Return value type name as string (e.g. \"رقم\").",
                    {{"قيمة", "value", "أي", false, false}},
                    "نوع(42)  # \"رقم\""
                },

                // ── بانيات الأنواع / Type constructors ────────────────────────
                {
                    "to_int", "رقم", "to_int",
                    BuiltinCategory::TypeCtor, "رقم",
                    "(AR) يحوّل قيمة إلى رقم صحيح. يرمي خطأ إذا تعذّر.",
                    "(EN) Convert a value to integer. Throws on failure.",
                    {{"قيمة", "value", "أي", false, false}},
                    "رقم(\"42\")"
                },
                {
                    "to_float", "عشري", "to_float",
                    BuiltinCategory::TypeCtor, "عشري",
                    "(AR) يحوّل قيمة إلى رقم عشري.",
                    "(EN) Convert a value to floating-point number.",
                    {{"قيمة", "value", "أي", false, false}},
                    "عشري(\"3.14\")"
                },
                {
                    "to_string", "نص", "to_string",
                    BuiltinCategory::TypeCtor, "نص",
                    "(AR) يحوّل أي قيمة إلى تمثيلها النصي.",
                    "(EN) Convert any value to its string representation.",
                    {{"قيمة", "value", "أي", false, false}},
                    "نص(123)  # \"123\""
                },
                {
                    "to_bool", "منطقي", "to_bool",
                    BuiltinCategory::TypeCtor, "منطقي",
                    "(AR) يحوّل قيمة إلى منطقي (صحيح/خطأ).",
                    "(EN) Convert a value to boolean.",
                    {{"قيمة", "value", "أي", false, false}},
                    "منطقي(0)  # خطأ"
                },

                // ── التزامن / Concurrency ─────────────────────────────────────
                {
                    "channel", "قناة", "channel",
                    BuiltinCategory::Concurrency, "قناة",
                    "(AR) ينشئ قناة مخزَّنة بسعة محددة.",
                    "(EN) Create a buffered channel with given capacity.",
                    {{"السعة", "capacity", "رقم", false, false}},
                    "متغير ق = قناة(10)"
                },
                {
                    "wait_all", "انتظر_الكل", "wait_all",
                    BuiltinCategory::Concurrency, "فراغ",
                    "(AR) يحجب حتى تنتهي جميع الـ goroutines المُطلَقة.",
                    "(EN) Block until all spawned goroutines complete.",
                    {},
                    "انتظر_الكل()"
                },
                {
                    "task_count", "عدد_المهام", "task_count",
                    BuiltinCategory::Concurrency, "رقم",
                    "(AR) يرجع عدد الـ goroutines النشطة حالياً.",
                    "(EN) Return number of currently active goroutines.",
                    {},
                    "اطبع_سطر(عدد_المهام())"
                },
                {
                    "wait_group", "مجموعة_انتظار", "wait_group",
                    BuiltinCategory::Concurrency, "مجموعة_انتظار",
                    "(AR) ينشئ مجموعة انتظار للتزامن بين الخيوط.",
                    "(EN) Create a wait group for thread synchronization.",
                    {},
                    "متغير مج = مجموعة_انتظار()"
                },
                {
                    "mutex", "قفل", "mutex",
                    BuiltinCategory::Concurrency, "قفل",
                    "(AR) ينشئ قفلاً للحماية من السباقات (mutex).",
                    "(EN) Create a mutex lock to prevent data races.",
                    {},
                    "متغير ق_ف = قفل()"
                },
                {
                    "future", "مستقبل", "future",
                    BuiltinCategory::Concurrency, "مستقبل",
                    "(AR) ينشئ مستقبلاً يحمل قيمة تُحسب لاحقاً في goroutine.",
                    "(EN) Create a future holding a value computed later.",
                    {},
                    "متغير م = مستقبل()"
                },
            };
            return table;
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) جدول الطرق المضمنة على الأنواع — مطابق للمستند المرجعي
        // (EN) Built-in methods table — matches reference documentation
        //
        //   مصفوفات (~16): اضف، احذف_اخير، رتب، عكس، يحتوي، فهرس، شريحة،
        //                  خريطة، رشح، اختزل، لكل، اول، آخر، فارغة، الطول، مسطح
        //   نصوص (~10):    طول، يحتوي، يبدأ_بـ، ينتهي_بـ، تقسيم، استبدل،
        //                  استخراج، قص_أطراف، تحويل_كبير، تحويل_صغير
        //   خرائط (~8):    احصل، عيّن، احذف، يحتوي، مفاتيح، قيم، حجم، فارغة
        //   قنوات (~9):    أرسل، استقبل، حاول_ارسل، حاول_استقبل، الحجم،
        //                  السعة، فارغة، أغلق، مغلقة
        // ───────────────────────────────────────────────────────────────────────────────
        const std::vector<BuiltinMethod>& allBuiltinMethods()
        {
            using H = BuiltinHostType;
            static const std::vector<BuiltinMethod> table = {
                // ── Array methods ─────────────────────────────────────────────
                {"array.push",       H::Array, "اضف",        "push",
                 "فراغ", "(AR) يُضيف عنصراً إلى نهاية المصفوفة.", "(EN) Append element to end.",
                 {{"عنصر", "element", "أي", false, false}}},
                {"array.pop",        H::Array, "احذف_اخير", "pop",
                 "أي", "(AR) يحذف ويرجع آخر عنصر.", "(EN) Remove and return last element.", {}},
                {"array.sort",       H::Array, "رتب",        "sort",
                 "مصفوفة", "(AR) يرتب المصفوفة تصاعدياً.", "(EN) Sort array ascending.", {}},
                {"array.reverse",    H::Array, "عكس",        "reverse",
                 "مصفوفة", "(AR) يعكس ترتيب العناصر.", "(EN) Reverse element order.", {}},
                {"array.contains",   H::Array, "يحتوي",      "contains",
                 "منطقي", "(AR) يفحص وجود عنصر.", "(EN) Check element existence.",
                 {{"عنصر", "element", "أي", false, false}}},
                {"array.index_of",   H::Array, "فهرس",       "index_of",
                 "رقم", "(AR) يرجع فهرس عنصر أو -1.", "(EN) Return element index or -1.",
                 {{"عنصر", "element", "أي", false, false}}},
                {"array.slice",      H::Array, "شريحة",      "slice",
                 "مصفوفة", "(AR) يستخرج شريحة فرعية [بداية، نهاية).", "(EN) Extract subarray [start, end).",
                 {{"بداية", "start", "رقم", false, false},
                  {"نهاية", "end", "رقم", true, false}}},
                {"array.map",        H::Array, "خريطة",      "map",
                 "مصفوفة", "(AR) يطبق دالة على كل عنصر.", "(EN) Apply function to each element.",
                 {{"دالة", "fn", "أي", false, false}}},
                {"array.filter",     H::Array, "رشح",        "filter",
                 "مصفوفة", "(AR) يبقي العناصر التي تحقق الشرط.", "(EN) Keep elements matching predicate.",
                 {{"دالة", "fn", "أي", false, false}}},
                {"array.reduce",     H::Array, "اختزل",      "reduce",
                 "أي", "(AR) يجمع المصفوفة بقيمة واحدة.", "(EN) Reduce to single value.",
                 {{"دالة", "fn", "أي", false, false},
                  {"بدء", "init", "أي", true, false}}},
                {"array.for_each",   H::Array, "لكل",        "for_each",
                 "فراغ", "(AR) ينفّذ دالة على كل عنصر.", "(EN) Execute function for each element.",
                 {{"دالة", "fn", "أي", false, false}}},
                {"array.first",      H::Array, "اول",        "first",
                 "أي", "(AR) يرجع أول عنصر.", "(EN) Return first element.", {}},
                {"array.last",       H::Array, "آخر",        "last",
                 "أي", "(AR) يرجع آخر عنصر.", "(EN) Return last element.", {}},
                {"array.empty",      H::Array, "فارغة",      "empty",
                 "منطقي", "(AR) يفحص إن كانت فارغة.", "(EN) Check if empty.", {}},
                {"array.length",     H::Array, "الطول",      "length",
                 "رقم", "(AR) يرجع عدد العناصر.", "(EN) Return element count.", {}},
                {"array.flat",       H::Array, "مسطح",       "flat",
                 "مصفوفة", "(AR) يسطّح المصفوفات المتداخلة بمستوى واحد.", "(EN) Flatten one level deep.", {}},

                // ── String methods ────────────────────────────────────────────
                {"string.length",      H::String, "طول",          "length",
                 "رقم", "(AR) يرجع طول النص (محارف).", "(EN) Return string length (chars).", {}},
                {"string.contains",    H::String, "يحتوي",        "contains",
                 "منطقي", "(AR) يفحص وجود نص فرعي.", "(EN) Check substring existence.",
                 {{"نص_فرعي", "substr", "نص", false, false}}},
                {"string.starts_with", H::String, "يبدأ_بـ",     "starts_with",
                 "منطقي", "(AR) يفحص بداية النص.", "(EN) Check string prefix.",
                 {{"بادئة", "prefix", "نص", false, false}}},
                {"string.ends_with",   H::String, "ينتهي_بـ",    "ends_with",
                 "منطقي", "(AR) يفحص نهاية النص.", "(EN) Check string suffix.",
                 {{"لاحقة", "suffix", "نص", false, false}}},
                {"string.split",       H::String, "تقسيم",        "split",
                 "مصفوفة", "(AR) يقسّم النص حول فاصل.", "(EN) Split string around separator.",
                 {{"فاصل", "separator", "نص", false, false}}},
                {"string.replace",     H::String, "استبدل",       "replace",
                 "نص", "(AR) يستبدل كل نص فرعي ببديل.", "(EN) Replace all occurrences.",
                 {{"قديم", "old", "نص", false, false},
                  {"جديد", "new", "نص", false, false}}},
                {"string.substring",   H::String, "استخراج",      "substring",
                 "نص", "(AR) يستخرج نصاً فرعياً [بداية، نهاية).", "(EN) Extract substring [start, end).",
                 {{"بداية", "start", "رقم", false, false},
                  {"نهاية", "end", "رقم", true, false}}},
                {"string.trim",        H::String, "قص_أطراف",     "trim",
                 "نص", "(AR) يحذف المسافات من الأطراف.", "(EN) Trim whitespace from both ends.", {}},
                {"string.to_upper",    H::String, "تحويل_كبير",   "to_upper",
                 "نص", "(AR) يحوّل إلى حروف كبيرة (لاتيني).", "(EN) Convert to uppercase (Latin).", {}},
                {"string.to_lower",    H::String, "تحويل_صغير",   "to_lower",
                 "نص", "(AR) يحوّل إلى حروف صغيرة (لاتيني).", "(EN) Convert to lowercase (Latin).", {}},

                // ── Map methods ───────────────────────────────────────────────
                {"map.get",      H::Map, "احصل",   "get",
                 "أي", "(AR) يرجع قيمة المفتاح أو لاشيء.", "(EN) Get value by key or null.",
                 {{"مفتاح", "key", "أي", false, false}}},
                {"map.set",      H::Map, "عيّن",  "set",
                 "فراغ", "(AR) يعيّن قيمة لمفتاح.", "(EN) Set value for key.",
                 {{"مفتاح", "key", "أي", false, false},
                  {"قيمة", "value", "أي", false, false}}},
                {"map.delete",   H::Map, "احذف",   "delete",
                 "منطقي", "(AR) يحذف مفتاحاً ويرجع نجاح العملية.", "(EN) Delete key, return success.",
                 {{"مفتاح", "key", "أي", false, false}}},
                {"map.contains", H::Map, "يحتوي",  "contains",
                 "منطقي", "(AR) يفحص وجود مفتاح.", "(EN) Check key existence.",
                 {{"مفتاح", "key", "أي", false, false}}},
                {"map.keys",     H::Map, "مفاتيح", "keys",
                 "مصفوفة", "(AR) يرجع مصفوفة المفاتيح.", "(EN) Return array of keys.", {}},
                {"map.values",   H::Map, "قيم",    "values",
                 "مصفوفة", "(AR) يرجع مصفوفة القيم.", "(EN) Return array of values.", {}},
                {"map.size",     H::Map, "حجم",    "size",
                 "رقم", "(AR) يرجع عدد الأزواج.", "(EN) Return pair count.", {}},
                {"map.empty",    H::Map, "فارغة",  "empty",
                 "منطقي", "(AR) يفحص إن كانت فارغة.", "(EN) Check if empty.", {}},

                // ── Channel methods ───────────────────────────────────────────
                {"channel.send",       H::Channel, "أرسل",         "send",
                 "فراغ", "(AR) يرسل قيمة (يحجب إذا ممتلئة).", "(EN) Send value (blocks if full).",
                 {{"قيمة", "value", "أي", false, false}}},
                {"channel.receive",    H::Channel, "استقبل",       "receive",
                 "أي", "(AR) يستقبل قيمة (يحجب إذا فارغة).", "(EN) Receive value (blocks if empty).", {}},
                {"channel.try_send",   H::Channel, "حاول_ارسل",   "try_send",
                 "منطقي", "(AR) يحاول الإرسال بدون حجب.", "(EN) Try send without blocking.",
                 {{"قيمة", "value", "أي", false, false}}},
                {"channel.try_receive",H::Channel, "حاول_استقبل", "try_receive",
                 "أي", "(AR) يحاول الاستقبال بدون حجب.", "(EN) Try receive without blocking.", {}},
                {"channel.size",       H::Channel, "الحجم",        "size",
                 "رقم", "(AR) عدد العناصر الحالية.", "(EN) Current element count.", {}},
                {"channel.capacity",   H::Channel, "السعة",        "capacity",
                 "رقم", "(AR) السعة القصوى.", "(EN) Maximum capacity.", {}},
                {"channel.empty",      H::Channel, "فارغة",        "empty",
                 "منطقي", "(AR) يفحص إن كانت فارغة.", "(EN) Check if empty.", {}},
                {"channel.close",      H::Channel, "أغلق",         "close",
                 "فراغ", "(AR) يغلق القناة.", "(EN) Close the channel.", {}},
                {"channel.closed",     H::Channel, "مغلقة",        "closed",
                 "منطقي", "(AR) يفحص إن كانت مغلقة.", "(EN) Check if closed.", {}},
            };
            return table;
        }

    } // namespace Info
} // namespace Sad
