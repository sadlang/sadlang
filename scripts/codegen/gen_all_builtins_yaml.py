"""
يُولِّد ملفات language-truth/builtins/*.yaml الكاملة من البيانات المستخرجة من builtin_registry.h
"""
import yaml, pathlib, sys
sys.stdout.reconfigure(encoding='utf-8')

ROOT = pathlib.Path("language-truth/builtins")

def write_yaml(filename, namespace_group, description_ar, description_en, functions):
    doc = {
        "version": 1,
        "namespace_group": namespace_group,
        "description_ar": description_ar,
        "description_en": description_en,
        "functions": functions
    }
    out = ROOT / filename
    out.write_text(
        yaml.dump(doc, allow_unicode=True, default_flow_style=False, sort_keys=False, width=120),
        encoding='utf-8'
    )
    print(f"Written: {filename} ({len(functions)} functions)")

def fn(cpp_id, canonical, namespace, category, require_import, module, strategy, desc_ar, desc_en, params=None):
    return {
        "cpp_id": cpp_id,
        "canonical": canonical,
        "namespace": namespace,
        "category": category,
        "require_import": require_import,
        "module": module,
        "compiler_strategy": strategy,
        "since": "0.1.0",
        "status": "stable",
        "description_ar": desc_ar,
        "description_en": desc_en,
        "params": params or []
    }

MI = "MATH"
SI = "STRINGS"
BI = "BASICS"
AI = "ASSERTIONS"
MP = "MAPS"
SK = "SOCKETS"
NW = "NETWORK"
MF = "MODULE_FUNCTION"
IN = "INLINE"
RT = "RUNTIME_CALL"
LI = "LLVM_INTRINSIC"

# Math (33)
math_fns = [
    fn("SQRT",        "جذر",           "Math", MF, True, MI, LI, "الجذر التربيعي", "Square root"),
    fn("POWER",       "أس",            "Math", MF, True, MI, LI, "الأسّ", "Power"),
    fn("ABS",         "مطلق",          "Math", MF, True, MI, LI, "القيمة المطلقة", "Absolute value"),
    fn("ROUND",       "تقريب",         "Math", MF, True, MI, RT, "تقريب لأقرب صحيح", "Round"),
    fn("FLOOR",       "أرضية",         "Math", MF, True, MI, LI, "تقريب للأسفل", "Floor"),
    fn("CEIL",        "سقف",           "Math", MF, True, MI, LI, "تقريب للأعلى", "Ceil"),
    fn("SQUARE",      "مربع",          "Math", MF, True, MI, IN, "مربع العدد", "Square"),
    fn("MAX",         "أكبر",          "Math", MF, True, MI, IN, "الأكبر", "Maximum"),
    fn("MIN",         "أصغر",          "Math", MF, True, MI, IN, "الأصغر", "Minimum"),
    fn("SUM",         "جمع",           "Math", MF, True, MI, RT, "مجموع", "Sum"),
    fn("SIN",         "جيب",           "Math", MF, True, MI, LI, "جيب الزاوية", "Sine"),
    fn("COS",         "جتا",           "Math", MF, True, MI, LI, "جتا الزاوية", "Cosine"),
    fn("TAN",         "ظل",            "Math", MF, True, MI, LI, "ظل الزاوية", "Tangent"),
    fn("ASIN",        "معكوس_جيب",     "Math", MF, True, MI, LI, "جيب عكسي", "Arc sine"),
    fn("ACOS",        "معكوس_جتا",     "Math", MF, True, MI, LI, "جتا عكسي", "Arc cosine"),
    fn("ATAN",        "معكوس_ظل",      "Math", MF, True, MI, LI, "ظل عكسي", "Arc tangent"),
    fn("ATAN2",       "معكوس_ظل2",     "Math", MF, True, MI, LI, "ظل عكسي ثنائي", "Atan2"),
    fn("LOG",         "لوغ",           "Math", MF, True, MI, LI, "لوغاريتم طبيعي", "Natural log"),
    fn("LOG2",        "لوغ2",          "Math", MF, True, MI, LI, "لوغاريتم ثنائي", "Log base 2"),
    fn("LOG10",       "لوغ10",         "Math", MF, True, MI, LI, "لوغاريتم عشري", "Log base 10"),
    fn("EXP",         "أسي",           "Math", MF, True, MI, LI, "الدالة الأسية", "Exponential"),
    fn("PI",          "ط",             "Math", MF, True, MI, IN, "الثابت π", "Pi constant"),
    fn("PI_ALT",      "باي",           "Math", MF, True, MI, IN, "الثابت π (بديل)", "Pi alt"),
    fn("E",           "هـ",            "Math", MF, True, MI, IN, "الثابت e", "Euler number"),
    fn("RANDOM",      "عشوائي",        "Math", MF, True, MI, RT, "عدد عشوائي [0,1)", "Random"),
    fn("RANDOM_INT",  "عشوائي_صحيح",   "Math", MF, True, MI, RT, "صحيح عشوائي", "Random int"),
    fn("RANDOM_RANGE","عشوائي_نطاق",   "Math", MF, True, MI, RT, "عشوائي في نطاق", "Random range"),
    fn("RANDOM_FLOAT","عشوائي_عشري",   "Math", MF, True, MI, RT, "عشري عشوائي", "Random float"),
    fn("CLAMP",       "حصر",           "Math", MF, True, MI, RT, "حصر في نطاق", "Clamp"),
    fn("TRUNCATE",    "اقتطاع",        "Math", MF, True, MI, LI, "اقتطاع عشري", "Truncate"),
    fn("FMOD",        "باقي",          "Math", MF, True, MI, RT, "باقي عشري", "Float modulo"),
    fn("SIGN",        "إشارة",         "Math", MF, True, MI, RT, "إشارة العدد", "Sign"),
    fn("LERP",        "استيفاء_خطي",   "Math", MF, True, MI, RT, "استيفاء خطي", "Lerp"),
]
write_yaml("math.yaml", "Math", "وحدة رياضيات (33 دالة)", "Math module (33 functions)", math_fns)

# Strings (21)
strings_fns = [
    fn("STR_LENGTH",    "طول_نص",        "Strings", MF, True, SI, RT, "طول النص", "String length"),
    fn("CHAR_CODE",     "رمز_حرف",       "Strings", MF, True, SI, RT, "رمز Unicode", "Char code"),
    fn("FIND",          "بحث",           "Strings", MF, True, SI, RT, "إيجاد نص فرعي", "Find"),
    fn("REPLACE",       "استبدل",        "Strings", MF, True, SI, RT, "استبدال", "Replace"),
    fn("SUBSTRING",     "استخراج",       "Strings", MF, True, SI, RT, "استخراج جزء", "Substring"),
    fn("TRIM",          "قص_أطراف",      "Strings", MF, True, SI, RT, "حذف مسافات", "Trim"),
    fn("TO_UPPER",      "تحويل_كبير",    "Strings", MF, True, SI, RT, "حروف كبيرة", "Uppercase"),
    fn("TO_LOWER",      "تحويل_صغير",    "Strings", MF, True, SI, RT, "حروف صغيرة", "Lowercase"),
    fn("SPLIT",         "تقسيم",         "Strings", MF, True, SI, RT, "تقسيم بفاصل", "Split"),
    fn("JOIN",          "دمج",           "Strings", MF, True, SI, RT, "دمج قائمة", "Join"),
    fn("STARTS_WITH",   "يبدأ_ب",        "Strings", MF, True, SI, RT, "يبدأ بـ", "Starts with"),
    fn("ENDS_WITH",     "ينتهي_ب",       "Strings", MF, True, SI, RT, "ينتهي بـ", "Ends with"),
    fn("CONTAINS",      "يحتوي_على",     "Strings", MF, True, SI, RT, "يحوي نص فرعي", "Contains"),
    fn("APPEND",        "أضف",           "Strings", MF, True, SI, RT, "إضافة لمصفوفة", "Append"),
    fn("REMOVE_ELEM",   "أزل",           "Strings", MF, True, SI, RT, "إزالة من مصفوفة", "Remove"),
    fn("CONVERT_TO_INT","حول_رقم",       "Strings", MF, True, SI, RT, "تحويل لرقم", "Parse number"),
    fn("TO_ARRAY",      "لمصفوفة",       "Strings", MF, True, SI, RT, "تحويل لمصفوفة", "To array"),
    fn("IS_INT",        "هو_رقم_صحيح",   "Strings", MF, True, SI, RT, "هل عدد صحيح؟", "Is integer"),
    fn("IS_FLOAT",      "هو_رقم_عشري",   "Strings", MF, True, SI, RT, "هل عدد عشري؟", "Is float"),
    fn("IS_STRING",     "هو_نص",         "Strings", MF, True, SI, RT, "هل نص؟", "Is string"),
    fn("IS_ARRAY",      "هو_مصفوفة",     "Strings", MF, True, SI, RT, "هل مصفوفة؟", "Is array"),
]
write_yaml("strings.yaml", "Strings", "وحدة نصوص (21 دالة)", "Strings module (21 functions)", strings_fns)

# Arrays (10 — built-in, no import needed in some contexts)
arrays_fns = [
    fn("ADD",           "إضافة_عنصر",   "Arrays", MF, False, "NONE", RT, "إضافة عنصر", "Add element"),
    fn("REMOVE",        "إزالة_عنصر",   "Arrays", MF, False, "NONE", RT, "إزالة عنصر", "Remove element"),
    fn("SIZE",          "حجم_مصفوفة",   "Arrays", MF, False, "NONE", RT, "حجم", "Array size"),
    fn("INDEX_OF",      "فهرس_مصفوفة",  "Arrays", MF, False, "NONE", RT, "فهرس العنصر", "Index of"),
    fn("ARRAY_CONTAINS","يحتوي_عنصر",   "Arrays", MF, False, "NONE", RT, "يحوي العنصر؟", "Contains"),
    fn("REVERSE",       "قلب",          "Arrays", MF, False, "NONE", RT, "عكس الترتيب", "Reverse"),
    fn("SORT",          "رتب",          "Arrays", MF, False, "NONE", RT, "ترتيب", "Sort"),
    fn("FIRST",         "الأول",        "Arrays", MF, False, "NONE", RT, "أول عنصر", "First"),
    fn("LAST",          "الأخير",       "Arrays", MF, False, "NONE", RT, "آخر عنصر", "Last"),
    fn("SLICE",         "شريحة",        "Arrays", MF, False, "NONE", RT, "شريحة", "Slice"),
]
write_yaml("arrays.yaml", "Arrays", "عمليات مصفوفات مدمجة (10 دوال)", "Built-in array ops (10)", arrays_fns)

# Basics (10)
basics_fns = [
    fn("RANGE",       "مدى",         "Basics", MF, True, BI, RT, "نطاق أرقام", "Range"),
    fn("EXIT",        "خروج",        "Basics", MF, True, BI, RT, "إنهاء البرنامج", "Exit"),
    fn("ASSERT",      "تأكد",        "Basics", MF, True, BI, RT, "تأكيد شرط", "Assert"),
    fn("READ_FILE",   "اقرأ_ملف",    "Basics", MF, True, BI, RT, "قراءة ملف", "Read file"),
    fn("WRITE_FILE",  "اكتب_ملف",    "Basics", MF, True, BI, RT, "كتابة ملف", "Write file"),
    fn("FILE_EXISTS", "هل_موجود",    "Basics", MF, True, BI, RT, "هل موجود؟", "File exists"),
    fn("IS_FILE",     "هل_ملف",      "Basics", MF, True, BI, RT, "هل ملف؟", "Is file"),
    fn("IS_DIR",      "هل_مجلد",     "Basics", MF, True, BI, RT, "هل مجلد؟", "Is dir"),
    fn("MKDIR",       "أنشئ_مجلد",   "Basics", MF, True, BI, RT, "إنشاء مجلد", "Make dir"),
    fn("DELETE_FILE", "احذف_ملف",    "Basics", MF, True, BI, RT, "حذف ملف", "Delete file"),
]
write_yaml("basics.yaml", "Basics", "وحدة أساسيات (10 دوال)", "Basics module (10 functions)", basics_fns)

# Assertions (5)
assertions_fns = [
    fn("ASSERT_EQ",  "تأكد_يساوي",     "Assertions", MF, True, AI, RT, "يجب أن يتساوى", "Assert equal"),
    fn("ASSERT_NEQ", "تأكد_لا_يساوي",  "Assertions", MF, True, AI, RT, "يجب ألا يتساوى", "Assert NEQ"),
    fn("ASSERT_TRUE","تأكد_صحيح",       "Assertions", MF, True, AI, RT, "يجب صحيحاً", "Assert true"),
    fn("ASSERT_FALSE","تأكد_خطأ",       "Assertions", MF, True, AI, RT, "يجب خطأ", "Assert false"),
    fn("HASH",       "هاش",            "Assertions", MF, True, AI, RT, "هاش القيمة", "Hash value"),
]
write_yaml("assertions.yaml", "Assertions", "وحدة تأكيدات (5 دوال)", "Assertions module (5 functions)", assertions_fns)

# Maps (46)
maps_fns = [
    fn("JSON_PARSE",      "تحليل_جيسون",   "Maps", MF, True, MP, RT, "تحليل JSON", "Parse JSON"),
    fn("JSON_STRINGIFY",  "نص_جيسون",      "Maps", MF, True, MP, RT, "تحويل لـ JSON", "JSON stringify"),
    fn("JSON_PRETTY",     "تنسيق_جيسون",   "Maps", MF, True, MP, RT, "JSON منسَّق", "Pretty JSON"),
    fn("XML_PARSE",       "تحليل_وسائم",   "Maps", MF, True, MP, RT, "تحليل XML", "Parse XML"),
    fn("XML_STRINGIFY",   "تحويل_وسائم",   "Maps", MF, True, MP, RT, "تحويل لـ XML", "XML stringify"),
    fn("REGEX",           "تعبير_مطابقة",  "Maps", MF, True, MP, RT, "مطابقة regex", "Regex match"),
    fn("REGEX_SEARCH",    "تعبير_بحث",     "Maps", MF, True, MP, RT, "بحث regex", "Regex search"),
    fn("REGEX_REPLACE",   "تعبير_استبدال", "Maps", MF, True, MP, RT, "استبدال regex", "Regex replace"),
    fn("REGEX_FIND_ALL",  "تعبير_جد_الكل", "Maps", MF, True, MP, RT, "كل تطابقات regex", "Regex find all"),
    fn("NOW",             "الآن",          "Maps", MF, True, MP, RT, "الوقت الحالي", "Current time"),
    fn("NOW_MS",          "الآن_مللي",     "Maps", MF, True, MP, RT, "الوقت بمللي ثانية", "Time ms"),
    fn("DATE_FORMAT",     "صيغة_تاريخ",    "Maps", MF, True, MP, RT, "تنسيق تاريخ", "Format date"),
    fn("DATE_COMPONENTS", "مكونات_تاريخ",  "Maps", MF, True, MP, RT, "مكونات التاريخ", "Date parts"),
    fn("YEAR",            "سنة",           "Maps", MF, True, MP, RT, "السنة", "Year"),
    fn("MONTH",           "شهر",           "Maps", MF, True, MP, RT, "الشهر", "Month"),
    fn("DAY",             "يوم",           "Maps", MF, True, MP, RT, "اليوم", "Day"),
    fn("HOUR",            "ساعة",          "Maps", MF, True, MP, RT, "الساعة", "Hour"),
    fn("MINUTE",          "دقيقة",         "Maps", MF, True, MP, RT, "الدقيقة", "Minute"),
    fn("SECOND",          "ثانية",         "Maps", MF, True, MP, RT, "الثانية", "Second"),
    fn("WEEKDAY",         "يوم_الأسبوع",   "Maps", MF, True, MP, RT, "يوم الأسبوع", "Weekday"),
    fn("TIME_DIFF",       "فرق_وقت",       "Maps", MF, True, MP, RT, "فرق وقت", "Time diff"),
    fn("FILE_SIZE",       "حجم_ملف",       "Maps", MF, True, MP, RT, "حجم ملف", "File size"),
    fn("PATH_JOIN",       "ضم_مسار",       "Maps", MF, True, MP, RT, "دمج مسار", "Join path"),
    fn("PATH_DIRNAME",    "مجلد_مسار",     "Maps", MF, True, MP, RT, "مجلد الملف", "Dirname"),
    fn("PATH_BASENAME",   "اسم_ملف",       "Maps", MF, True, MP, RT, "اسم الملف", "Basename"),
    fn("PATH_EXT",        "امتداد_ملف",    "Maps", MF, True, MP, RT, "امتداد الملف", "Extension"),
    fn("BASE64_ENCODE",   "ترميز_قاعدة64", "Maps", MF, True, MP, RT, "ترميز Base64", "Base64 encode"),
    fn("BASE64_DECODE",   "فك_قاعدة64",    "Maps", MF, True, MP, RT, "فك Base64", "Base64 decode"),
    fn("HEX_ENCODE",      "ترميز_ست_عشري", "Maps", MF, True, MP, RT, "ترميز سداسي", "Hex encode"),
    fn("URL_ENCODE",      "ترميز_عنوان",   "Maps", MF, True, MP, RT, "ترميز URL", "URL encode"),
    fn("URL_DECODE",      "فك_عنوان",      "Maps", MF, True, MP, RT, "فك URL", "URL decode"),
    fn("ENV_GET",         "متغير_بيئة",    "Maps", MF, True, MP, RT, "متغير بيئة", "Env var"),
    fn("CWD",             "المجلد_الحالي", "Maps", MF, True, MP, RT, "المجلد الحالي", "CWD"),
    fn("ERROR_FN",        "خطأ",           "Maps", MF, True, MP, RT, "إنشاء خطأ", "Create error"),
    fn("TEXT_REPEAT",     "تكرار_نص",      "Maps", MF, True, MP, RT, "تكرار نص", "Repeat text"),
    fn("PAD_START",       "حشو_بداية",     "Maps", MF, True, MP, RT, "حشو بداية", "Pad start"),
    fn("PAD_END",         "حشو_نهاية",     "Maps", MF, True, MP, RT, "حشو نهاية", "Pad end"),
    fn("REVERSE_TEXT",    "عكس_نص",        "Maps", MF, True, MP, RT, "عكس نص", "Reverse text"),
    fn("MAPS_CHAR_CODE",  "رمز_حرف",       "Maps", MF, True, MP, RT, "رمز حرف (خرائط)", "Char code"),
    fn("FROM_CHAR_CODE",  "حرف_من_رمز",    "Maps", MF, True, MP, RT, "حرف من رمز", "From char code"),
    fn("COUNT",           "عد",            "Maps", MF, True, MP, RT, "عد تكرارات", "Count"),
    fn("FORMAT",          "تنسيق",         "Maps", MF, True, MP, RT, "تنسيق نص", "Format"),
    fn("STRIP_DIACRITICS","ازل_تشكيل",     "Maps", MF, True, MP, RT, "إزالة تشكيل", "Strip diacritics"),
    fn("COMPARE_TEXT",    "مقارنة_نص",     "Maps", MF, True, MP, RT, "مقارنة نصين", "Compare text"),
    fn("UNICODE_CODEPOINTS","نص_يونيكود",  "Maps", MF, True, MP, RT, "نقاط Unicode", "Unicode pts"),
    fn("MAP_CTOR",        "خريطة",         "Maps", MF, True, MP, RT, "إنشاء خريطة", "Create map"),
]
write_yaml("maps.yaml", "Maps", "وحدة خرائط (46 دالة)", "Maps module (46 functions)", maps_fns)

# Sockets (18)
sockets_fns = [
    fn("TCP_NEW",          "أنشئ_وصلة",              "Sockets", MF, True, SK, RT, "إنشاء وصلة TCP", "Create TCP"),
    fn("TCP_CONNECT",      "اتصل_بـ",                "Sockets", MF, True, SK, RT, "الاتصال بخادم", "TCP connect"),
    fn("TCP_SEND",         "أرسل_عبر_وصلة",          "Sockets", MF, True, SK, RT, "إرسال عبر TCP", "TCP send"),
    fn("TCP_RECV",         "استقبل_عبر_وصلة",        "Sockets", MF, True, SK, RT, "استقبال عبر TCP", "TCP recv"),
    fn("TCP_CLOSE",        "أغلق_وصلة",              "Sockets", MF, True, SK, RT, "إغلاق TCP", "TCP close"),
    fn("TCP_BIND",         "خصص_منفذ",               "Sockets", MF, True, SK, RT, "ربط منفذ", "TCP bind"),
    fn("TCP_LISTEN",       "انتظر_اتصالات",           "Sockets", MF, True, SK, RT, "الاستماع", "TCP listen"),
    fn("TCP_ACCEPT",       "اقبل_متصل",              "Sockets", MF, True, SK, RT, "قبول اتصال", "TCP accept"),
    fn("TCP_CONNECTED",    "هل_متصلة",               "Sockets", MF, True, SK, RT, "هل متصلة؟", "Is connected"),
    fn("TCP_REMOTE_ADDR",  "عنوان_الطرف_الآخر",       "Sockets", MF, True, SK, RT, "عنوان الطرف الآخر", "Remote addr"),
    fn("UDP_NEW",          "أنشئ_مرسال",             "Sockets", MF, True, SK, RT, "إنشاء UDP", "Create UDP"),
    fn("UDP_BIND",         "خصص_منفذ_رسائل",         "Sockets", MF, True, SK, RT, "ربط UDP", "UDP bind"),
    fn("UDP_SEND",         "أرسل_رسالة",             "Sockets", MF, True, SK, RT, "إرسال UDP", "UDP send"),
    fn("UDP_RECV",         "استقبل_رسالة",           "Sockets", MF, True, SK, RT, "استقبال UDP", "UDP recv"),
    fn("UDP_CLOSE",        "أغلق_مرسال",             "Sockets", MF, True, SK, RT, "إغلاق UDP", "UDP close"),
    fn("RECV_TIMEOUT",     "حدد_انتظار_الاستقبال",    "Sockets", MF, True, SK, RT, "مهلة الاستقبال", "Recv timeout"),
    fn("SEND_TIMEOUT",     "حدد_انتظار_الإرسال",      "Sockets", MF, True, SK, RT, "مهلة الإرسال", "Send timeout"),
    fn("NO_DELAY",         "أرسل_فورا",              "Sockets", MF, True, SK, RT, "إرسال فوري", "No delay"),
    fn("LOCAL_PORT",       "منفذ_محلي",              "Sockets", MF, True, SK, RT, "المنفذ المحلي", "Local port"),
]
write_yaml("sockets.yaml", "Sockets", "وحدة مقابس (19 دالة)", "Sockets module (19 functions)", sockets_fns)

# HttpClient (17)
http_client_fns = [
    fn("NEW_CLIENT",   "أنشئ_متصفح",           "HttpClient", MF, True, NW, RT, "إنشاء عميل HTTP", "New HTTP client"),
    fn("FREE_CLIENT",  "أغلق_متصفح",           "HttpClient", MF, True, NW, RT, "تحرير العميل", "Free client"),
    fn("GET",          "اجلب",                 "HttpClient", MF, True, NW, RT, "طلب GET", "HTTP GET"),
    fn("POST",         "أرسل",                 "HttpClient", MF, True, NW, RT, "طلب POST", "HTTP POST"),
    fn("PUT",          "استبدل",               "HttpClient", MF, True, NW, RT, "طلب PUT", "HTTP PUT"),
    fn("DELETE_REQ",   "احذف_مورد",            "HttpClient", MF, True, NW, RT, "طلب DELETE", "HTTP DELETE"),
    fn("PATCH",        "عدل_مورد",             "HttpClient", MF, True, NW, RT, "طلب PATCH", "HTTP PATCH"),
    fn("SET_BASE_URL", "حدد_الموقع",           "HttpClient", MF, True, NW, RT, "تحديد الموقع", "Set base URL"),
    fn("SET_HEADER",   "أضف_ترويسة",           "HttpClient", MF, True, NW, RT, "إضافة ترويسة", "Set header"),
    fn("SET_TIMEOUT",  "حدد_الانتظار",         "HttpClient", MF, True, NW, RT, "تحديد المهلة", "Set timeout"),
    fn("SET_BEARER",   "سجل_دخول_برمز",        "HttpClient", MF, True, NW, RT, "Bearer token", "Set bearer"),
    fn("LAST_ERROR",   "سبب_الفشل",            "HttpClient", MF, True, NW, RT, "آخر خطأ", "Last error"),
    fn("IS_OK",        "هل_نجح",               "HttpClient", MF, True, NW, RT, "هل نجح؟", "Is OK"),
    fn("RESP_FREE",    "تجاهل_الرد",           "HttpClient", MF, True, NW, RT, "تجاهل الرد", "Free response"),
    fn("RESP_STATUS",  "رمز_الحالة",           "HttpClient", MF, True, NW, RT, "رمز الحالة", "Status code"),
    fn("RESP_BODY",    "نص_الرد",              "HttpClient", MF, True, NW, RT, "نص الرد", "Response body"),
    fn("RESP_HEADER",  "معلومة_الرد",          "HttpClient", MF, True, NW, RT, "ترويسة الرد", "Response header"),
    fn("RESP_SUCCESS", "هل_الرد_ناجح",         "HttpClient", MF, True, NW, RT, "هل الرد ناجح؟", "Response success"),
]
write_yaml("http_client.yaml", "HttpClient", "وحدة شبكة عميل HTTP (18 دالة)", "HTTP client module (18 functions)", http_client_fns)

# HttpServer (19) - from builtin_registry.h lines 361-382
http_server_fns = [
    fn("NEW_SERVER",      "أنشئ_خادم",              "HttpServer", MF, True, NW, RT, "إنشاء خادم HTTP", "New server"),
    fn("FREE_SERVER",     "أزل_خادم",               "HttpServer", MF, True, NW, RT, "تحرير الخادم", "Free server"),
    fn("ON_GET",          "عند_طلب_جلب",             "HttpServer", MF, True, NW, RT, "معالج GET", "On GET"),
    fn("ON_POST",         "عند_طلب_إرسال",           "HttpServer", MF, True, NW, RT, "معالج POST", "On POST"),
    fn("ON_PUT",          "عند_طلب_استبدال",         "HttpServer", MF, True, NW, RT, "معالج PUT", "On PUT"),
    fn("ON_DELETE",       "عند_طلب_حذف",             "HttpServer", MF, True, NW, RT, "معالج DELETE", "On DELETE"),
    fn("LISTEN",          "ابدأ_الاستماع",            "HttpServer", MF, True, NW, RT, "بدء الاستماع", "Start listen"),
    fn("STOP",            "أوقف_الخادم",             "HttpServer", MF, True, NW, RT, "إيقاف الخادم", "Stop server"),
    fn("ENABLE_CORS",     "اسمح_بالوصول_الخارجي",    "HttpServer", MF, True, NW, RT, "تفعيل CORS", "Enable CORS"),
    fn("REQ_METHOD",      "نوع_الطلب",               "HttpServer", MF, True, NW, RT, "نوع الطلب", "Request method"),
    fn("REQ_PATH",        "وجهة_الطلب",              "HttpServer", MF, True, NW, RT, "مسار الطلب", "Request path"),
    fn("REQ_BODY",        "بيانات_الطلب",             "HttpServer", MF, True, NW, RT, "بيانات الطلب", "Request body"),
    fn("REQ_HEADER",      "معلومة_الطلب",             "HttpServer", MF, True, NW, RT, "ترويسة الطلب", "Request header"),
    fn("REQ_QUERY",       "قيمة_من_الرابط",           "HttpServer", MF, True, NW, RT, "معامل URL", "Query param"),
    fn("RESP_SET_STATUS", "عين_حالة_الرد",            "HttpServer", MF, True, NW, RT, "تعيين حالة الرد", "Set status"),
    fn("RESP_SET_BODY",   "عين_نص_الرد",              "HttpServer", MF, True, NW, RT, "تعيين نص الرد", "Set body"),
    fn("RESP_SET_JSON",   "عين_رد_جيسون",             "HttpServer", MF, True, NW, RT, "رد JSON", "Set JSON body"),
    fn("RESP_SET_HTML",   "عين_رد_صفحة",              "HttpServer", MF, True, NW, RT, "رد HTML", "Set HTML body"),
    fn("RESP_SET_HEADER", "عين_ترويسة_الرد",          "HttpServer", MF, True, NW, RT, "تعيين ترويسة", "Set resp header"),
]
write_yaml("http_server.yaml", "HttpServer", "وحدة شبكة خادم HTTP (19 دالة)", "HTTP server module (19 functions)", http_server_fns)

# NetworkUtils (12)
network_utils_fns = [
    fn("INIT",            "جهز_الشبكة",        "NetworkUtils", MF, True, NW, RT, "تهيئة الشبكة", "Init network"),
    fn("CLEANUP",         "نظف_الشبكة",        "NetworkUtils", MF, True, NW, RT, "تنظيف الشبكة", "Cleanup network"),
    fn("LAST_ERROR_CODE", "رمز_آخر_خطأ",       "NetworkUtils", MF, True, NW, RT, "رمز آخر خطأ", "Last error code"),
    fn("LAST_ERROR_MSG",  "وصف_آخر_خطأ",       "NetworkUtils", MF, True, NW, RT, "وصف آخر خطأ", "Last error msg"),
    fn("ADDR_NEW",        "عنوان",             "NetworkUtils", MF, True, NW, RT, "إنشاء عنوان IPv4", "New address"),
    fn("ADDR_NEW_V6",     "عنوان_حديث",        "NetworkUtils", MF, True, NW, RT, "إنشاء عنوان IPv6", "New IPv6 addr"),
    fn("ADDR_FREE",       "حرر_عنوان",         "NetworkUtils", MF, True, NW, RT, "تحرير عنوان", "Free address"),
    fn("ADDR_IP",         "رقم_الجهاز",        "NetworkUtils", MF, True, NW, RT, "عنوان IP", "IP address"),
    fn("ADDR_PORT",       "رقم_المنفذ",        "NetworkUtils", MF, True, NW, RT, "رقم المنفذ", "Port number"),
    fn("ADDR_IS_V4",      "هل_عنوان_قديم",     "NetworkUtils", MF, True, NW, RT, "هل IPv4؟", "Is IPv4"),
    fn("ADDR_IS_V6",      "هل_عنوان_حديث",     "NetworkUtils", MF, True, NW, RT, "هل IPv6؟", "Is IPv6"),
    fn("ADDR_STR",        "العنوان_كنص",       "NetworkUtils", MF, True, NW, RT, "العنوان كنص", "Address as string"),
]
write_yaml("network_utils.yaml", "NetworkUtils", "وحدة شبكة أدوات (12 دالة)", "Network utils (12 functions)", network_utils_fns)

# WebSocketClient (14)
ws_fns = [
    fn("NEW_CLIENT",      "أنشئ_عميل_ويبسوكت",        "WebSocketClient", MF, True, NW, RT, "إنشاء عميل WebSocket", "New WS client"),
    fn("FREE_CLIENT",     "أزل_عميل_ويبسوكت",         "WebSocketClient", MF, True, NW, RT, "تحرير العميل", "Free WS client"),
    fn("CONNECT",         "اتصل_بويبسوكت",            "WebSocketClient", MF, True, NW, RT, "الاتصال", "WS connect"),
    fn("CLOSE",           "أغلق_ويبسوكت",             "WebSocketClient", MF, True, NW, RT, "إغلاق الاتصال", "WS close"),
    fn("IS_CONNECTED",    "هل_متصل_ويبسوكت",          "WebSocketClient", MF, True, NW, RT, "هل متصل؟", "WS connected"),
    fn("SEND",            "أرسل_ويبسوكت",             "WebSocketClient", MF, True, NW, RT, "إرسال رسالة", "WS send"),
    fn("PING",            "نبض_ويبسوكت",              "WebSocketClient", MF, True, NW, RT, "إرسال ping", "WS ping"),
    fn("RECEIVE",         "استقبل_ويبسوكت",           "WebSocketClient", MF, True, NW, RT, "استقبال رسالة", "WS receive"),
    fn("HAS_MESSAGE",     "يوجد_رسالة_ويبسوكت",       "WebSocketClient", MF, True, NW, RT, "يوجد رسالة؟", "WS has msg"),
    fn("GET_URL",         "رابط_ويبسوكت",             "WebSocketClient", MF, True, NW, RT, "الرابط", "WS URL"),
    fn("LAST_ERROR",      "خطأ_ويبسوكت",              "WebSocketClient", MF, True, NW, RT, "آخر خطأ", "WS last error"),
    fn("SET_RECV_TIMEOUT","حدد_مهلة_استقبال_ويبسوكت", "WebSocketClient", MF, True, NW, RT, "مهلة الاستقبال", "WS recv timeout"),
    fn("SET_SEND_TIMEOUT","حدد_مهلة_إرسال_ويبسوكت",  "WebSocketClient", MF, True, NW, RT, "مهلة الإرسال", "WS send timeout"),
]
write_yaml("websocket_client.yaml", "WebSocketClient", "وحدة شبكة WebSocket (13 دالة)", "WebSocket client (13 functions)", ws_fns)

# Update _index.yaml
index = {
    "description_ar": "فهرس ملفات builtins/",
    "description_en": "Index of builtins/ files",
    "files": [
        "core.yaml",
        "types.yaml",
        "concurrency.yaml",
        "math.yaml",
        "strings.yaml",
        "arrays.yaml",
        "basics.yaml",
        "assertions.yaml",
        "maps.yaml",
        "sockets.yaml",
        "http_client.yaml",
        "http_server.yaml",
        "network_utils.yaml",
        "websocket_client.yaml",
    ]
}
(ROOT / "_index.yaml").write_text(
    yaml.dump(index, allow_unicode=True, default_flow_style=False, sort_keys=False),
    encoding='utf-8'
)
print("Written: _index.yaml")
print()
print("Summary:")
total = 3+4+6+33+21+10+10+5+46+19+18+19+12+13
print(f"  Total functions: {total}")
