/**
 * @file builtin_registry.h
 * @brief (AR) السجل المركزي الموحّد لجميع الدوال المدمجة — مصدر الحقيقة الوحيد
 * @brief (EN) Unified central registry for all builtin functions — single source of truth
 *
 * @details
 * (AR) هذا هو الملف المركزي الذي يقرأ منه المفسر والمترجم معاً.
 *      كل دالة مدمجة في لغة ص لها إدخال واحد هنا يحدد:
 *      - اسمها العربي الوحيد (لا أسماء بديلة)
 *      - تصنيفها (IO أساسي، باني نوع، دالة وحدة)
 *      - الوحدة المطلوبة (إن وُجدت)
 *      - استراتيجية المترجم (inline، runtime call، LLVM intrinsic)
 *
 *      لكل دالة اسم واحد فقط بالعربية — لا أسماء بديلة ولا إنجليزية.
 *      أي دالة مدمجة جديدة يجب أن تُضاف هنا فقط.
 *
 * القرار المعماري: ADR-003 — توحيد المكتبات القياسية
 *
 * @author فريق تطوير لغة ص
 * @date 2025-07-19
 */

#pragma once

#include "builtin_categories.h"
#include "module_definitions.h"
#include <string_view>
#include <cstddef>
#include <array>

namespace Sad
{
    namespace Builtins
    {

        // ════════════════════════════════════════════════════════════════════
        // (AR) الأسماء القانونية لجميع الدوال المدمجة — مصدر الحقيقة الوحيد
        // (EN) Canonical names for all builtin functions — single source of truth
        //
        // (AR) القاعدة: كل دالة مدمجة لها اسم عربي واحد فقط.
        //      المفسر والمترجم يستخدمان هذه الثوابت بدلاً من نصوص حرفية.
        //      الأسماء هنا مخزنة بالشكل القانوني الذي يُنتجه الـ lexer بعد
        //      حذف التشكيل من المعرّفات العربية، لأن المطابقة وقت البناء
        //      والتنفيذ تتم على هذه الصيغة المجرّدة لا على الصيغة المزخرفة.
        //      لإضافة دالة جديدة: أضف الثابت هنا ثم استخدمه في المفسر والمترجم.
        // ════════════════════════════════════════════════════════════════════

        namespace Names
        {
            // ─────────── دوال IO أساسية (بدون استيراد) ───────────
            namespace Core
            {
                inline constexpr std::string_view PRINT = "اطبع";
                inline constexpr std::string_view PRINTLN = "اطبع_سطر";
                inline constexpr std::string_view READ = "اقرأ";
                inline constexpr std::string_view LENGTH = "طول";
                inline constexpr std::string_view TYPE = "نوع";
            }

            // ─────────── بانيات الأنواع (بدون استيراد) ───────────
            namespace TypeCtor
            {
                inline constexpr std::string_view TO_INT = "رقم";
                inline constexpr std::string_view TO_FLOAT = "عشري";
                inline constexpr std::string_view TO_STRING = "نص";
                inline constexpr std::string_view TO_BOOL = "منطقي";
            }

            // ─────────── دوال التزامن الأساسية (بدون استيراد) ───────────
            namespace Concurrency
            {
                inline constexpr std::string_view CHANNEL = "قناة";
                inline constexpr std::string_view WAIT_ALL = "انتظر_الكل";
                inline constexpr std::string_view TASK_COUNT = "عدد_المهام";
                inline constexpr std::string_view WAIT_GROUP = "مجموعة_انتظار";
                inline constexpr std::string_view MUTEX = "قفل";
                inline constexpr std::string_view FUTURE = "مستقبل";
            }

            // ─────────── وحدة رياضيات ───────────
            namespace Math
            {
                // أساسيات
                inline constexpr std::string_view SQRT = "جذر";
                inline constexpr std::string_view POWER = "أس";
                inline constexpr std::string_view ABS = "مطلق";
                inline constexpr std::string_view ROUND = "تقريب";
                inline constexpr std::string_view FLOOR = "أرضية";
                inline constexpr std::string_view CEIL = "سقف";
                inline constexpr std::string_view SQUARE = "مربع";
                inline constexpr std::string_view MAX = "أكبر";
                inline constexpr std::string_view MIN = "أصغر";
                inline constexpr std::string_view SUM = "جمع";

                // مثلثية
                inline constexpr std::string_view SIN = "جيب";
                inline constexpr std::string_view COS = "جتا";
                inline constexpr std::string_view TAN = "ظل";
                inline constexpr std::string_view ASIN = "معكوس_جيب";
                inline constexpr std::string_view ACOS = "معكوس_جتا";
                inline constexpr std::string_view ATAN = "معكوس_ظل";
                inline constexpr std::string_view ATAN2 = "معكوس_ظل2";

                // لوغاريتمية وأسية
                inline constexpr std::string_view LOG = "لوغ";
                inline constexpr std::string_view LOG2 = "لوغ2";
                inline constexpr std::string_view LOG10 = "لوغ10";
                inline constexpr std::string_view EXP = "أسي";

                // ثوابت
                inline constexpr std::string_view PI = "ط";
                inline constexpr std::string_view PI_ALT = "باي";
                inline constexpr std::string_view E = "هـ";

                // عشوائية
                inline constexpr std::string_view RANDOM = "عشوائي";
                inline constexpr std::string_view RANDOM_INT = "عشوائي_صحيح";
                inline constexpr std::string_view RANDOM_RANGE = "عشوائي_نطاق";
                inline constexpr std::string_view RANDOM_FLOAT = "عشوائي_عشري";

                // دوال إضافية (وحدة رياضيات فقط)
                inline constexpr std::string_view CLAMP = "حصر";
                inline constexpr std::string_view TRUNCATE = "اقتطاع";
                inline constexpr std::string_view FMOD = "باقي";
                inline constexpr std::string_view SIGN = "إشارة";
                inline constexpr std::string_view LERP = "استيفاء_خطي";
            }

            // ─────────── وحدة نصوص ───────────
            namespace Strings
            {
                inline constexpr std::string_view STR_LENGTH = "طول_نص";
                inline constexpr std::string_view CHAR_CODE = "رمز_حرف";
                inline constexpr std::string_view FIND = "بحث";
                inline constexpr std::string_view REPLACE = "استبدل";
                inline constexpr std::string_view SUBSTRING = "استخراج";
                inline constexpr std::string_view TRIM = "قص_أطراف";
                inline constexpr std::string_view TO_UPPER = "تحويل_كبير";
                inline constexpr std::string_view TO_LOWER = "تحويل_صغير";
                inline constexpr std::string_view SPLIT = "تقسيم";
                inline constexpr std::string_view JOIN = "دمج";
                inline constexpr std::string_view STARTS_WITH = "يبدأ_ب";
                inline constexpr std::string_view ENDS_WITH = "ينتهي_ب";
                inline constexpr std::string_view CONTAINS = "يحتوي_على";

                // ── (AR) دوال المصفوفات والقوائم (ضمن وحدة نصوص) ──
                inline constexpr std::string_view APPEND = "أضف";
                inline constexpr std::string_view REMOVE_ELEM = "أزل";

                // ── (AR) دوال تحويل الأنواع (ضمن وحدة نصوص) ──
                inline constexpr std::string_view CONVERT_TO_INT = "حول_رقم";
                inline constexpr std::string_view TO_ARRAY = "لمصفوفة";

                // ── (AR) دوال فحص الأنواع (ضمن وحدة نصوص) ──
                inline constexpr std::string_view IS_INT = "هو_رقم_صحيح";
                inline constexpr std::string_view IS_FLOAT = "هو_رقم_عشري";
                inline constexpr std::string_view IS_STRING = "هو_نص";
                inline constexpr std::string_view IS_ARRAY = "هو_مصفوفة";
            }

            // ─────────── عمليات المصفوفات (مدمجة — بدون استيراد) ───────────
            namespace Arrays
            {
                inline constexpr std::string_view ADD = "إضافة_عنصر";
                inline constexpr std::string_view REMOVE = "إزالة_عنصر";
                inline constexpr std::string_view SIZE = "حجم_مصفوفة";
                inline constexpr std::string_view INDEX_OF = "فهرس_مصفوفة";
                inline constexpr std::string_view ARRAY_CONTAINS = "يحتوي_عنصر";
                inline constexpr std::string_view REVERSE = "قلب";
                inline constexpr std::string_view SORT = "رتب";
                inline constexpr std::string_view FIRST = "الأول";
                inline constexpr std::string_view LAST = "الأخير";
                inline constexpr std::string_view SLICE = "شريحة";
            }

            // ─────────── وحدة أساسيات ───────────
            namespace Basics
            {
                inline constexpr std::string_view RANGE = "مدى";
                inline constexpr std::string_view EXIT = "خروج";
                inline constexpr std::string_view ASSERT = "تأكد";
                inline constexpr std::string_view READ_FILE = "اقرأ_ملف";
                inline constexpr std::string_view WRITE_FILE = "اكتب_ملف";
                inline constexpr std::string_view FILE_EXISTS = "هل_موجود";
                inline constexpr std::string_view IS_FILE = "هل_ملف";
                inline constexpr std::string_view IS_DIR = "هل_مجلد";
                inline constexpr std::string_view MKDIR = "أنشئ_مجلد";
                inline constexpr std::string_view DELETE_FILE = "احذف_ملف";
            }

            // ─────────── وحدة تأكيدات ───────────
            namespace Assertions
            {
                inline constexpr std::string_view ASSERT_EQ = "تأكد_يساوي";
                inline constexpr std::string_view ASSERT_NEQ = "تأكد_لا_يساوي";
                inline constexpr std::string_view ASSERT_TRUE = "تأكد_صحيح";
                inline constexpr std::string_view ASSERT_FALSE = "تأكد_خطأ";
                inline constexpr std::string_view HASH = "هاش";
            }

            // ─────────── وحدة خرائط ───────────
            namespace Maps
            {
                inline constexpr std::string_view JSON_PARSE = "تحليل_جيسون";
                inline constexpr std::string_view JSON_STRINGIFY = "نص_جيسون";
                inline constexpr std::string_view REGEX = "تعبير_مطابقة";
                inline constexpr std::string_view NOW = "الآن";
                inline constexpr std::string_view NOW_MS = "الآن_مللي";
                inline constexpr std::string_view DATE_FORMAT = "صيغة_تاريخ";
                inline constexpr std::string_view DATE_COMPONENTS = "مكونات_تاريخ";
                inline constexpr std::string_view YEAR = "سنة";
                inline constexpr std::string_view MONTH = "شهر";
                inline constexpr std::string_view DAY = "يوم";
                inline constexpr std::string_view HOUR = "ساعة";
                inline constexpr std::string_view MINUTE = "دقيقة";
                inline constexpr std::string_view SECOND = "ثانية";
                inline constexpr std::string_view WEEKDAY = "يوم_الأسبوع";
                inline constexpr std::string_view TIME_DIFF = "فرق_وقت";
                inline constexpr std::string_view FILE_SIZE = "حجم_ملف";
                inline constexpr std::string_view PATH_JOIN = "ضم_مسار";
                inline constexpr std::string_view PATH_DIRNAME = "مجلد_مسار";
                inline constexpr std::string_view PATH_BASENAME = "اسم_ملف";
                inline constexpr std::string_view PATH_EXT = "امتداد_ملف";
                inline constexpr std::string_view BASE64_ENCODE = "ترميز_قاعدة64";
                inline constexpr std::string_view BASE64_DECODE = "فك_قاعدة64";
                inline constexpr std::string_view HEX_ENCODE = "ترميز_ست_عشري";
                inline constexpr std::string_view URL_ENCODE = "ترميز_عنوان";
                inline constexpr std::string_view URL_DECODE = "فك_عنوان";
                inline constexpr std::string_view ENV_GET = "متغير_بيئة";
                inline constexpr std::string_view CWD = "المجلد_الحالي";
                inline constexpr std::string_view ERROR_FN = "خطأ";

                // ── (AR) دوال JSON/XML — تعريب كامل / Pure Arabic ──
                inline constexpr std::string_view JSON_PRETTY = "تنسيق_جيسون";
                inline constexpr std::string_view XML_PARSE = "تحليل_وسائم";
                inline constexpr std::string_view XML_STRINGIFY = "تحويل_وسائم";

                // ── (AR) دوال التعبيرات النمطية والنصوص ──
                inline constexpr std::string_view REGEX_SEARCH = "تعبير_بحث";
                inline constexpr std::string_view REGEX_REPLACE = "تعبير_استبدال";
                inline constexpr std::string_view REGEX_FIND_ALL = "تعبير_جد_الكل";
                inline constexpr std::string_view TEXT_REPEAT = "تكرار_نص";
                inline constexpr std::string_view PAD_START = "حشو_بداية";
                inline constexpr std::string_view PAD_END = "حشو_نهاية";
                inline constexpr std::string_view REVERSE_TEXT = "عكس_نص";
                inline constexpr std::string_view CHAR_CODE = "رمز_حرف";
                inline constexpr std::string_view FROM_CHAR_CODE = "حرف_من_رمز";
                inline constexpr std::string_view COUNT = "عد";
                inline constexpr std::string_view FORMAT = "تنسيق";
                inline constexpr std::string_view STRIP_DIACRITICS = "ازل_تشكيل";
                inline constexpr std::string_view COMPARE_TEXT = "مقارنة_نص";
                inline constexpr std::string_view UNICODE_CODEPOINTS = "نص_يونيكود";

                // ── (AR) دوال الخرائط الأساسية والدوال الوظيفية ──
                inline constexpr std::string_view MAP_CTOR = "خريطة";
                inline constexpr std::string_view MAP_GET = "خريطة_احصل";
                inline constexpr std::string_view MAP_SET = "خريطة_عين";
                inline constexpr std::string_view MAP_KEYS = "خريطة_مفاتيح";
                inline constexpr std::string_view MAP_VALUES = "خريطة_قيم";
                inline constexpr std::string_view MAP_HAS_KEY = "خريطة_تحتوي";
                inline constexpr std::string_view MAP_DELETE = "خريطة_احذف";
                inline constexpr std::string_view MAP_SIZE = "خريطة_حجم";
                inline constexpr std::string_view MAP_ENTRIES = "خريطة_عناصر";
                inline constexpr std::string_view MAP_MERGE = "خريطة_دمج";
                inline constexpr std::string_view MAP_FN = "تخطيط";
                inline constexpr std::string_view FILTER = "تصفية";
                inline constexpr std::string_view REDUCE = "اختزال";
                inline constexpr std::string_view FOR_EACH = "لكل_عنصر";
                inline constexpr std::string_view FLAT_MAP = "تخطيط_مسطح";
                inline constexpr std::string_view ZIP = "ضم";
                inline constexpr std::string_view ANY_ELEMENT = "أي_عنصر";
                inline constexpr std::string_view ALL_ELEMENTS = "كل_العناصر";
                inline constexpr std::string_view UNIQUE = "فريد";
                inline constexpr std::string_view UNION = "اتحاد";
                inline constexpr std::string_view INTERSECT = "تقاطع";
                inline constexpr std::string_view DIFFERENCE = "فرق";

                // ── (AR) دوال المصفوفات المتقدمة ──
                inline constexpr std::string_view FLATTEN = "تسطيح";
                inline constexpr std::string_view CHUNK = "تقطيع";
                inline constexpr std::string_view TAKE = "خذ";
                inline constexpr std::string_view DROP = "اترك";
                inline constexpr std::string_view ENUMERATE = "رقم";
                inline constexpr std::string_view AVERAGE = "متوسط";
                inline constexpr std::string_view MIN_ARRAY = "أصغر_المصفوفة";
                inline constexpr std::string_view MAX_ARRAY = "أكبر_المصفوفة";
                inline constexpr std::string_view SORT_ARRAY = "ترتيب_مصفوفة";
                inline constexpr std::string_view REVERSE_ARRAY = "عكس_مصفوفة";
                inline constexpr std::string_view JOIN = "ربط";
                inline constexpr std::string_view MATRIX_NEW = "مصفوفة_جديدة";
                inline constexpr std::string_view MATRIX_IDENTITY = "مصفوفة_وحدة";
                inline constexpr std::string_view ARANGE = "نطاق_مصفوفة";
                inline constexpr std::string_view SHAPE = "أبعاد_مصفوفة";
                inline constexpr std::string_view TRANSPOSE = "قلب_محوري";
            }

            // ── (AR) دوال المقابس ──
            // ── (AR) مقابس TCP — اتصال مستمر ──
            // ── (EN) TCP Sockets — persistent connection ──
            namespace Sockets
            {
                inline constexpr std::string_view TCP_NEW = "أنشئ_وصلة";
                inline constexpr std::string_view TCP_CONNECT = "اتصل_بـ";
                inline constexpr std::string_view TCP_SEND = "أرسل_عبر_وصلة";
                inline constexpr std::string_view TCP_RECV = "استقبل_عبر_وصلة";
                inline constexpr std::string_view TCP_CLOSE = "أغلق_وصلة";
                inline constexpr std::string_view TCP_BIND = "خصص_منفذ";
                inline constexpr std::string_view TCP_LISTEN = "انتظر_اتصالات";
                inline constexpr std::string_view TCP_ACCEPT = "اقبل_متصل";
                inline constexpr std::string_view TCP_CONNECTED = "هل_متصلة";
                inline constexpr std::string_view TCP_REMOTE_ADDR = "عنوان_الطرف_الآخر";
                // ── (AR) مقابس UDP — رسائل بدون اتصال ──
                // ── (EN) UDP Sockets — connectionless messages ──
                inline constexpr std::string_view UDP_NEW = "أنشئ_مرسال";
                inline constexpr std::string_view UDP_BIND = "خصص_منفذ_رسائل";
                inline constexpr std::string_view UDP_SEND = "أرسل_رسالة";
                inline constexpr std::string_view UDP_RECV = "استقبل_رسالة";
                inline constexpr std::string_view UDP_CLOSE = "أغلق_مرسال";
                // ── (AR) إعدادات المقابس ──
                // ── (EN) Socket settings ──
                inline constexpr std::string_view RECV_TIMEOUT = "حدد_انتظار_الاستقبال";
                inline constexpr std::string_view SEND_TIMEOUT = "حدد_انتظار_الإرسال";
                inline constexpr std::string_view NO_DELAY = "أرسل_فورا";
                // ── (AR) معلومات المقبس المحلي ──
                // ── (EN) Local socket info ──
                inline constexpr std::string_view LOCAL_PORT = "منفذ_محلي";
            }

            // ── (AR) عميل HTTP — إرسال واستقبال طلبات الويب ──
            // ── (EN) HTTP Client — sending and receiving web requests ──
            namespace HttpClient
            {
                // ── إنشاء وتحرير العميل ──
                inline constexpr std::string_view NEW_CLIENT = "أنشئ_متصفح";
                inline constexpr std::string_view FREE_CLIENT = "أغلق_متصفح";
                // ── طلبات HTTP ──
                inline constexpr std::string_view GET = "اجلب";
                inline constexpr std::string_view POST = "أرسل";
                inline constexpr std::string_view PUT = "استبدل";
                inline constexpr std::string_view DELETE_REQ = "احذف_مورد";
                inline constexpr std::string_view PATCH = "عدل_مورد";
                // ── إعدادات العميل ──
                inline constexpr std::string_view SET_BASE_URL = "حدد_الموقع";
                inline constexpr std::string_view SET_HEADER = "أضف_ترويسة";
                inline constexpr std::string_view SET_TIMEOUT = "حدد_الانتظار";
                inline constexpr std::string_view SET_BEARER = "سجل_دخول_برمز";
                inline constexpr std::string_view LAST_ERROR = "سبب_الفشل";
                inline constexpr std::string_view IS_OK = "هل_نجح";
                // ── الاستجابة ──
                inline constexpr std::string_view RESP_FREE = "تجاهل_الرد";
                inline constexpr std::string_view RESP_STATUS = "رمز_الحالة";
                inline constexpr std::string_view RESP_BODY = "نص_الرد";
                inline constexpr std::string_view RESP_HEADER = "معلومة_الرد";
                inline constexpr std::string_view RESP_SUCCESS = "هل_الرد_ناجح";
            }

            // ── (AR) خادم HTTP — استقبال الطلبات ──
            // ── (EN) HTTP Server — handling incoming requests ──
            namespace HttpServer
            {
                inline constexpr std::string_view NEW_SERVER = "أنشئ_خادم";
                inline constexpr std::string_view FREE_SERVER = "أزل_خادم";
                inline constexpr std::string_view ON_GET = "عند_طلب_جلب";
                inline constexpr std::string_view ON_POST = "عند_طلب_إرسال";
                inline constexpr std::string_view ON_PUT = "عند_طلب_استبدال";
                inline constexpr std::string_view ON_DELETE = "عند_طلب_حذف";
                inline constexpr std::string_view LISTEN = "ابدأ_الاستماع";
                inline constexpr std::string_view STOP = "أوقف_الخادم";
                inline constexpr std::string_view ENABLE_CORS = "اسمح_بالوصول_الخارجي";
                // ── بيانات الطلب الوارد ──
                inline constexpr std::string_view REQ_METHOD = "نوع_الطلب";
                inline constexpr std::string_view REQ_PATH = "وجهة_الطلب";
                inline constexpr std::string_view REQ_BODY = "بيانات_الطلب";
                inline constexpr std::string_view REQ_HEADER = "معلومة_الطلب";
                inline constexpr std::string_view REQ_QUERY = "قيمة_من_الرابط";
                // ── بناء الاستجابة ──
                inline constexpr std::string_view RESP_SET_STATUS = "عين_حالة_الرد";
                inline constexpr std::string_view RESP_SET_BODY = "عين_نص_الرد";
                inline constexpr std::string_view RESP_SET_JSON = "عين_رد_جيسون";
                inline constexpr std::string_view RESP_SET_HTML = "عين_رد_صفحة";
                inline constexpr std::string_view RESP_SET_HEADER = "عين_ترويسة_الرد";
            }

            // ── (AR) أدوات الشبكة العامة ──
            // ── (EN) General network utilities ──
            namespace NetworkUtils
            {
                inline constexpr std::string_view INIT = "جهز_الشبكة";
                inline constexpr std::string_view CLEANUP = "نظف_الشبكة";
                inline constexpr std::string_view LAST_ERROR_CODE = "رمز_آخر_خطأ";
                inline constexpr std::string_view LAST_ERROR_MSG = "وصف_آخر_خطأ";
                // ── العناوين ──
                inline constexpr std::string_view ADDR_NEW = "عنوان";
                inline constexpr std::string_view ADDR_NEW_V6 = "عنوان_حديث";
                inline constexpr std::string_view ADDR_FREE = "حرر_عنوان";
                inline constexpr std::string_view ADDR_IP = "رقم_الجهاز";
                inline constexpr std::string_view ADDR_PORT = "رقم_المنفذ";
                inline constexpr std::string_view ADDR_IS_V4 = "هل_عنوان_قديم";
                inline constexpr std::string_view ADDR_IS_V6 = "هل_عنوان_حديث";
                inline constexpr std::string_view ADDR_STR = "العنوان_كنص";
            }

            // ── (AR) عميل WebSocket — اتصال ثنائي الاتجاه عبر ws:// ──
            // ── (EN) WebSocket Client — bidirectional ws:// connection ──
            namespace WebSocketClient
            {
                // ── دورة الحياة ──
                inline constexpr std::string_view NEW_CLIENT = "أنشئ_عميل_ويبسوكت";
                inline constexpr std::string_view FREE_CLIENT = "أزل_عميل_ويبسوكت";
                // ── الاتصال ──
                inline constexpr std::string_view CONNECT = "اتصل_بويبسوكت";
                inline constexpr std::string_view CLOSE = "أغلق_ويبسوكت";
                inline constexpr std::string_view IS_CONNECTED = "هل_متصل_ويبسوكت";
                // ── الإرسال والاستقبال ──
                inline constexpr std::string_view SEND = "أرسل_ويبسوكت";
                inline constexpr std::string_view PING = "نبض_ويبسوكت";
                inline constexpr std::string_view RECEIVE = "استقبل_ويبسوكت";
                inline constexpr std::string_view HAS_MESSAGE = "يوجد_رسالة_ويبسوكت";
                // ── معلومات ──
                inline constexpr std::string_view GET_URL = "رابط_ويبسوكت";
                inline constexpr std::string_view LAST_ERROR = "خطأ_ويبسوكت";
                // ── إعدادات ──
                inline constexpr std::string_view SET_RECV_TIMEOUT = "حدد_مهلة_استقبال_ويبسوكت";
                inline constexpr std::string_view SET_SEND_TIMEOUT = "حدد_مهلة_إرسال_ويبسوكت";
            }

            // ── (AR) خادم WebSocket — استقبال اتصالات وبث رسائل ──
            // ── (EN) WebSocket Server — accept connections, broadcast messages ──
            namespace WebSocketServer
            {
                // ── دورة الحياة ──
                inline constexpr std::string_view NEW_SERVER = "أنشئ_خادم_ويبسوكت";
                inline constexpr std::string_view FREE_SERVER = "أزل_خادم_ويبسوكت";
                // ── التشغيل ──
                inline constexpr std::string_view START = "ابدأ_خادم_ويبسوكت";
                inline constexpr std::string_view STOP = "أوقف_خادم_ويبسوكت";
                inline constexpr std::string_view IS_RUNNING = "هل_يعمل_خادم_ويبسوكت";
                // ── معلومات ──
                inline constexpr std::string_view CONN_COUNT = "عدد_اتصالات_ويبسوكت";
                inline constexpr std::string_view GET_PORT = "منفذ_خادم_ويبسوكت";
                // ── البث ──
                inline constexpr std::string_view BROADCAST = "بث_ويبسوكت";
                inline constexpr std::string_view BROADCAST_ROOM = "بث_لغرفة_ويبسوكت";
                // ── الغرف ──
                inline constexpr std::string_view ROOM_COUNT = "عدد_غرف_ويبسوكت";
                inline constexpr std::string_view ROOM_SIZE = "حجم_غرفة_ويبسوكت";
                // ── الإغلاق ──
                inline constexpr std::string_view CLOSE_ALL = "أغلق_كل_اتصالات_ويبسوكت";
            }

            // ─────────── وحدة المنصة — دوال النظام والبيئة ───────────
            namespace Platform
            {
                inline constexpr std::string_view GET_ENV = "احصل_بيئة";
                inline constexpr std::string_view SET_ENV = "عين_بيئة";
                inline constexpr std::string_view CUR_DIR = "مجلد_حالي";
                inline constexpr std::string_view CHANGE_DIR = "غير_مجلد";
                inline constexpr std::string_view EXECUTE = "نفذ";
                inline constexpr std::string_view PLATFORM_NAME = "منصة";
                inline constexpr std::string_view OS_NAME = "نظام";
                inline constexpr std::string_view TIMESTAMP = "طابع_زمن";
                inline constexpr std::string_view CLOCK = "ساعة";
            }

            // ─────────── دوال نواة نظام التشغيل (محاكاة في المفسر) ───────────
            namespace Kernel
            {
                // ── منافذ الإدخال/الإخراج ──
                inline constexpr std::string_view PORT_WRITE = "منفذ_اكتب";
                inline constexpr std::string_view PORT_READ = "منفذ_اقرأ";
                inline constexpr std::string_view MEM_WRITE = "ذاكرة_اكتب";
                inline constexpr std::string_view MEM_READ = "ذاكرة_اقرأ";
                // ── مقاطعات ──
                inline constexpr std::string_view INTERRUPT = "مقاطعة";
                inline constexpr std::string_view HALT = "توقف";
                inline constexpr std::string_view CLI = "تعطيل_مقاطعات";
                inline constexpr std::string_view STI = "تفعيل_مقاطعات";
                // ── VGA / ذاكرة ──
                inline constexpr std::string_view VGA_WRITE = "شاشة_اكتب";
                inline constexpr std::string_view VGA_CLEAR = "شاشة_امسح";
                inline constexpr std::string_view MEMCPY = "انسخ_ذاكرة";
                inline constexpr std::string_view MEMSET = "املأ_ذاكرة";
                // ── تسلسل (Serial) ──
                inline constexpr std::string_view SERIAL_INIT = "تسلسل_هئ";
                inline constexpr std::string_view SERIAL_SEND = "تسلسل_ارسل";
                inline constexpr std::string_view SERIAL_RECV = "تسلسل_استقبل";
                inline constexpr std::string_view SERIAL_READY = "تسلسل_جاهز";
                // ── GPIO ──
                inline constexpr std::string_view GPIO_WRITE = "منفذ_رقمي_اكتب";
                inline constexpr std::string_view GPIO_READ = "منفذ_رقمي_اقرأ";
                inline constexpr std::string_view GPIO_MODE = "حدد_وضع_منفذ";
                // ── مؤقت مضمن ──
                inline constexpr std::string_view EMBED_TIMER_INIT = "مؤقت_هئ";
                inline constexpr std::string_view EMBED_TIMER_READ = "مؤقت_قراءة";
                inline constexpr std::string_view EMBED_TIMER_WAIT = "مؤقت_انتظر";
                // ── تحكم بالمعالج ──
                inline constexpr std::string_view RESET = "اعد_تشغيل";
                inline constexpr std::string_view CPUID = "معرف_المعالج";
                inline constexpr std::string_view RDTSC = "عداد_الدورات";
                // ── حواجز ذاكرة ──
                inline constexpr std::string_view MFENCE = "حاجز_ذاكرة";
                inline constexpr std::string_view LFENCE = "حاجز_قراءة";
                inline constexpr std::string_view SFENCE = "حاجز_كتابة";
                // ── DMA ──
                inline constexpr std::string_view DMA_INIT = "نقل_مباشر_هئ";
                inline constexpr std::string_view DMA_START = "نقل_مباشر_ابدأ";
            }

            // ─────────── مؤقتات النظام (PIT/RTC/Stopwatch) ───────────
            namespace KernelTimers
            {
                // ── PIT ──
                inline constexpr std::string_view PIT_INIT = "مؤقت_تهيئة";
                inline constexpr std::string_view PIT_FREQ = "مؤقت_التردد";
                inline constexpr std::string_view PIT_COUNT = "مؤقت_العداد";
                inline constexpr std::string_view PIT_SET_FREQ = "مؤقت_تعيين_تردد";
                // ── RTC ──
                inline constexpr std::string_view RTC_SECONDS = "مؤقت_الثواني";
                inline constexpr std::string_view RTC_MINUTES = "مؤقت_الدقائق";
                inline constexpr std::string_view RTC_HOURS = "مؤقت_الساعات";
                inline constexpr std::string_view RTC_DAY = "مؤقت_اليوم";
                inline constexpr std::string_view RTC_MONTH = "مؤقت_الشهر";
                inline constexpr std::string_view RTC_YEAR = "مؤقت_السنة";
                inline constexpr std::string_view RTC_TIME = "مؤقت_الوقت";
                inline constexpr std::string_view RTC_DATE = "مؤقت_التاريخ";
                inline constexpr std::string_view RTC_ALARM = "مؤقت_منبه";
                // ── نظام ──
                inline constexpr std::string_view SYS_TICKS = "مؤقت_نبضات";
                inline constexpr std::string_view UPTIME_MS = "مؤقت_وقت_التشغيل";
                inline constexpr std::string_view UPTIME_SEC = "مؤقت_ثواني_التشغيل";
                inline constexpr std::string_view SLEEP = "مؤقت_انتظار";
                inline constexpr std::string_view TICK_RATE = "مؤقت_معدل_النبضات";
                inline constexpr std::string_view SET_TICK_RATE = "مؤقت_تعيين_معدل";
                inline constexpr std::string_view TICK = "مؤقت_نقرة";
                // ── ساعة إيقاف ──
                inline constexpr std::string_view SW_CREATE = "مؤقت_ساعة_جديدة";
                inline constexpr std::string_view SW_START = "مؤقت_ساعة_بدء";
                inline constexpr std::string_view SW_STOP = "مؤقت_ساعة_ايقاف";
                inline constexpr std::string_view SW_RESET = "مؤقت_ساعة_صفر";
                inline constexpr std::string_view SW_MS = "مؤقت_ساعة_ملي";
                inline constexpr std::string_view SW_RUNNING = "مؤقت_ساعة_تعمل";
                // ── تقرير ──
                inline constexpr std::string_view REPORT = "مؤقت_تقرير";
                inline constexpr std::string_view TIMER_RESET = "مؤقت_اعادة_تعيين";
                inline constexpr std::string_view RTC_READ = "مؤقت_قراءة_rtc";
                inline constexpr std::string_view PIT_ENABLED = "مؤقت_مفعل";
            }

            // ─────────── خيوط النظام (mutex/semaphore) ───────────
            namespace KernelThreads
            {
                // ── خيوط ──
                inline constexpr std::string_view THREAD_NEW = "خيط_جديد";
                inline constexpr std::string_view THREAD_START = "خيط_ابدأ";
                inline constexpr std::string_view THREAD_WAIT = "خيط_انتظر";
                inline constexpr std::string_view THREAD_STATUS = "خيط_حالة";
                inline constexpr std::string_view THREAD_SLEEP = "خيط_نوم";
                inline constexpr std::string_view THREAD_ID = "خيط_معرف";
                inline constexpr std::string_view THREAD_CPU_COUNT = "خيط_عدد_معالجات";
                inline constexpr std::string_view THREAD_LIST = "خيط_قائمة";
                inline constexpr std::string_view THREAD_DELETE = "خيط_احذف";
                inline constexpr std::string_view THREAD_YIELD = "خيط_تنازل";
                inline constexpr std::string_view THREAD_COUNT = "خيط_عدد";
                // ── أقفال ──
                inline constexpr std::string_view LOCK_NEW = "قفل_جديد";
                inline constexpr std::string_view LOCK_ACQUIRE = "قفل_أقفل";
                inline constexpr std::string_view LOCK_RELEASE = "قفل_حرر";
                inline constexpr std::string_view LOCK_TRY = "قفل_حاول";
                inline constexpr std::string_view LOCK_DELETE = "قفل_احذف";
                // ── إشارات ──
                inline constexpr std::string_view SEM_NEW = "إشارة_جديدة";
                inline constexpr std::string_view SEM_WAIT = "إشارة_انتظر";
                inline constexpr std::string_view SEM_POST = "إشارة_أطلق";
                inline constexpr std::string_view SEM_TRY = "إشارة_حاول";
            }

            // ─────────── لوحة المفاتيح والفأرة (USB) ───────────
            namespace KernelUSB
            {
                // ── لوحة مفاتيح ──
                inline constexpr std::string_view KB_INIT = "لوحة_تهيئة";
                inline constexpr std::string_view KB_POLL = "لوحة_استطلاع";
                inline constexpr std::string_view KB_KEY_PRESSED = "لوحة_مفتاح_مضغوط";
                inline constexpr std::string_view KB_LAST_KEY = "لوحة_آخر_مفتاح";
                inline constexpr std::string_view KB_LAST_SCAN = "لوحة_آخر_فحص";
                inline constexpr std::string_view KB_MODIFIERS = "لوحة_معدلات";
                inline constexpr std::string_view KB_TO_CHAR = "لوحة_إلى_حرف";
                inline constexpr std::string_view KB_CONNECTED = "لوحة_متصلة";
                inline constexpr std::string_view KB_CONSTANTS = "لوحة_ثابت";
                // ── فأرة ──
                inline constexpr std::string_view MOUSE_INIT = "فأرة_تهيئة";
                inline constexpr std::string_view MOUSE_POLL = "فأرة_استطلاع";
                inline constexpr std::string_view MOUSE_X = "فأرة_س";
                inline constexpr std::string_view MOUSE_Y = "فأرة_ص";
                inline constexpr std::string_view MOUSE_LEFT = "فأرة_يسار";
                inline constexpr std::string_view MOUSE_RIGHT = "فأرة_يمين";
                inline constexpr std::string_view MOUSE_MIDDLE = "فأرة_وسط";
                inline constexpr std::string_view MOUSE_WHEEL = "فأرة_عجلة";
                inline constexpr std::string_view MOUSE_CONNECTED = "فأرة_متصلة";
                inline constexpr std::string_view MOUSE_BOUNDS = "فأرة_حدود";
            }

            // ─────────── شبكة SadNet (P2P) ───────────
            namespace SadNet
            {
                inline constexpr std::string_view NODE_NEW = "عقدة_جديدة";
                inline constexpr std::string_view NODE_ID = "معرف_العقدة";
                inline constexpr std::string_view SHA256 = "تشفير_sha256";
                inline constexpr std::string_view RANDOM_BYTES = "بايتات_عشوائية";
                inline constexpr std::string_view STORE_FILE = "تخزين_ملف";
                inline constexpr std::string_view RETRIEVE_FILE = "استرجاع_ملف";
                inline constexpr std::string_view ENCRYPTED_MSG = "رسالة_مشفرة";
                inline constexpr std::string_view PROOF_OF_WORK = "إثبات_عمل";
                inline constexpr std::string_view PEER_REPUTATION = "سمعة_قرين";
                inline constexpr std::string_view PEER_BAN = "حظر_قرين";
                inline constexpr std::string_view PEER_TRUSTED = "قرين_موثوق";
                inline constexpr std::string_view IDENTITY_NEW = "هوية_جديدة";
                inline constexpr std::string_view BROADCAST_NEW = "بث_جديد";
                inline constexpr std::string_view BROADCAST_STOP = "بث_إيقاف";
                inline constexpr std::string_view NODE_DESTROY = "تدمير_عقدة";
            }

            // ─────────── واجهة المستخدم (UI) — عناصر ───────────
            namespace UIWidgets
            {
                inline constexpr std::string_view TEXT_WIDGET = "نص_عنصر";
                inline constexpr std::string_view IMAGE = "صورة";
                inline constexpr std::string_view ICON = "أيقونة";
                inline constexpr std::string_view BUTTON = "زر";
                inline constexpr std::string_view FAB = "زر_عائم";
                inline constexpr std::string_view TEXT_BUTTON = "زر_نصي";
                inline constexpr std::string_view ICON_BUTTON = "زر_أيقونة";
                inline constexpr std::string_view TEXT_FIELD = "حقل_نص";
                inline constexpr std::string_view TOGGLE = "مفتاح";
                inline constexpr std::string_view CHECKBOX = "خانة_اختيار";
                inline constexpr std::string_view SLIDER = "منزلق";
                inline constexpr std::string_view COLUMN = "عمود";
                inline constexpr std::string_view ROW = "صف";
                inline constexpr std::string_view STACK = "رصة";
                inline constexpr std::string_view GRID = "شبكة";
                inline constexpr std::string_view CENTER = "وسط";
                inline constexpr std::string_view PADDING = "حشوة";
                inline constexpr std::string_view ALIGN = "محاذاة";
                inline constexpr std::string_view EXPANDED = "موسع";
                inline constexpr std::string_view FLEXIBLE = "مرن";
                inline constexpr std::string_view SIZED_BOX = "مقاس";
                inline constexpr std::string_view SPACER = "فاصل";
                inline constexpr std::string_view DIVIDER = "فاصل_خط";
                inline constexpr std::string_view WRAP = "التفاف";
                inline constexpr std::string_view CONTAINER = "حاوية";
                inline constexpr std::string_view CARD = "بطاقة";
                inline constexpr std::string_view SCAFFOLD = "هيكل";
                inline constexpr std::string_view BOX = "صندوق";
                inline constexpr std::string_view SCROLL_VIEW = "عرض_تمرير";
                inline constexpr std::string_view APP_BAR = "شريط_تطبيق";
                inline constexpr std::string_view BOTTOM_NAV = "تنقل_سفلي";
                inline constexpr std::string_view DIALOG = "حوار";
                inline constexpr std::string_view SNACKBAR = "شريط_إشعار";
                inline constexpr std::string_view TOOLTIP = "تلميح";
                inline constexpr std::string_view PROGRESS = "شريط_تقدم";
                inline constexpr std::string_view LAZY_COLUMN = "عمود_كسول";
                inline constexpr std::string_view LAZY_ROW = "صف_كسول";
                inline constexpr std::string_view LIST_VIEW = "قائمة";
                inline constexpr std::string_view TEXT_AREA = "منطقة_نص";
                inline constexpr std::string_view DRAWER = "درج";
                inline constexpr std::string_view SAFE_AREA = "منطقة_آمنة";
                inline constexpr std::string_view SURFACE = "سطح";
            }

            // ─────────── واجهة المستخدم — نواة ───────────
            namespace UICore
            {
                inline constexpr std::string_view ENGINE = "_محرك_واجهات";
                inline constexpr std::string_view RUN_APP = "تشغيل_تطبيق";
                inline constexpr std::string_view PRINT_TREE = "طباعة_شجرة";
                inline constexpr std::string_view NAVIGATE = "انتقل";
                inline constexpr std::string_view NAVIGATE_TRANSITION = "انتقل_بتحريك";
                inline constexpr std::string_view NAVIGATE_EXIT_TRANSITION = "انتقل_بتحريك_كامل";
                inline constexpr std::string_view BACK = "عودة";
                inline constexpr std::string_view BACK_TRANSITION = "عودة_بتحريك";
                inline constexpr std::string_view BACK_TO_ROOT = "عودة_للبداية";
                inline constexpr std::string_view REPLACE_PAGE = "استبدل";
                inline constexpr std::string_view TOGGLE_THEME = "تبديل_الثيم";
                inline constexpr std::string_view DARK_MODE = "وضع_داكن";
                inline constexpr std::string_view LIGHT_MODE = "وضع_فاتح";
                inline constexpr std::string_view IS_DARK = "هل_داكن";
                inline constexpr std::string_view UPDATE_STATE = "تحديث_حالة";
                inline constexpr std::string_view SET_TITLE = "عنوان_النافذة";
                inline constexpr std::string_view PAGE_COUNT = "عدد_الصفحات";
                inline constexpr std::string_view CLOSE_WINDOW = "أغلق_النافذة";
                inline constexpr std::string_view SET_STATE = "عين_الحالة";
                inline constexpr std::string_view GEN_WEB = "توليد_ويب";
            }

            // ─────────── واجهة المستخدم — صوت ───────────
            namespace UIAudio
            {
                inline constexpr std::string_view PLAY_SOUND = "شغل_صوت";
                inline constexpr std::string_view STOP_SOUND = "أوقف_صوت";
                inline constexpr std::string_view STOP_ALL = "أوقف_كل_الأصوات";
                inline constexpr std::string_view SET_VOLUME = "مستوى_الصوت";
                inline constexpr std::string_view GET_VOLUME = "احصل_مستوى_الصوت";
            }

            // ─────────── واجهة المستخدم — تشفير وقاعدة بيانات ───────────
            namespace UICrypto
            {
                inline constexpr std::string_view ENCRYPT = "شفر_نص";
                inline constexpr std::string_view DECRYPT = "فك_تشفير";
                inline constexpr std::string_view HASH_FN = "هاش";
                inline constexpr std::string_view DB_OPEN = "افتح_قاعدة";
                inline constexpr std::string_view DB_EXEC = "نفذ_استعلام";
                inline constexpr std::string_view DB_CLOSE = "أغلق_قاعدة";
            }

            // ─────────── واجهة المستخدم — شبكة ───────────
            namespace UINetwork
            {
                inline constexpr std::string_view HTTP_REQUEST = "طلب_شبكة";
                inline constexpr std::string_view IS_ONLINE = "هل_متصل";
                inline constexpr std::string_view JSON_PARSE = "حلل_جيسون";
                inline constexpr std::string_view JSON_STRINGIFY = "صدر_جيسون";
            }

            // ─────────── واجهة المستخدم — ملفات ───────────
            namespace UIIO
            {
                inline constexpr std::string_view ON_DROP = "عند_الإسقاط";
                inline constexpr std::string_view SCREENSHOT = "لقطة_شاشة";
                inline constexpr std::string_view READ_FILE = "اقرأ_ملف";
                inline constexpr std::string_view WRITE_FILE = "اكتب_ملف";
                inline constexpr std::string_view FILE_EXISTS = "هل_ملف_موجود";
                inline constexpr std::string_view FILE_PICKER = "اختر_ملف";
                inline constexpr std::string_view SAVE_PICKER = "حفظ_ملف";
            }

            // ─────────── واجهة المستخدم — حوارات ───────────
            namespace UIDialog
            {
                inline constexpr std::string_view ALERT = "حوار_تنبيه";
                inline constexpr std::string_view CONFIRM = "حوار_تأكيد";
                inline constexpr std::string_view SHOW_SNACKBAR = "أظهر_رسالة";
                inline constexpr std::string_view NOTIFICATION = "أظهر_إشعار";
            }

            // ─────────── واجهة المستخدم — مؤقتات ───────────
            namespace UITimer
            {
                inline constexpr std::string_view TIMEOUT = "بعد_مدة";
                inline constexpr std::string_view INTERVAL = "كل_مدة";
                inline constexpr std::string_view CANCEL_TIMER = "أوقف_مؤقت";
            }

            // ─────────── واجهة المستخدم — تخزين ───────────
            namespace UIStorage
            {
                inline constexpr std::string_view COPY = "انسخ";
                inline constexpr std::string_view PASTE = "الصق";
                inline constexpr std::string_view HAS_CLIPBOARD = "هل_يوجد_نص_منسوخ";
                inline constexpr std::string_view STORE = "خزن";
                inline constexpr std::string_view RETRIEVE = "استرجع";
                inline constexpr std::string_view REMOVE_STORED = "احذف_مخزن";
                inline constexpr std::string_view CLEAR_STORE = "امسح_المخزن";
            }

            // ─────────── واجهة المستخدم — منصة ───────────
            namespace UIPlatform
            {
                // ── حركات ──
                inline constexpr std::string_view ANIMATE = "حرك";
                inline constexpr std::string_view COLOR_ANIM = "حركة_لون";
                inline constexpr std::string_view STOP_ANIM = "أوقف_حركة";
                inline constexpr std::string_view STOP_NODE_ANIM = "أوقف_حركات_عقدة";
                inline constexpr std::string_view CANCEL_ALL_ANIM = "إلغاء_كل_الحركات";
                inline constexpr std::string_view ANIM_STATE = "حالة_حركة";
                inline constexpr std::string_view ANIM_VALUE = "قيمة_حركة";
                inline constexpr std::string_view HAS_ACTIVE_ANIM = "حركات_نشطة";
                // ── جغرافيا وكاميرا ──
                inline constexpr std::string_view GEO_LOCATION = "احصل_موقع";
                inline constexpr std::string_view CAMERA_OPEN = "افتح_كاميرا";
                inline constexpr std::string_view CAPTURE = "التقط_صورة";
                inline constexpr std::string_view SENSOR_READ = "قراءة_مستشعر";
                inline constexpr std::string_view SENSORS_LIST = "المستشعرات_المتاحة";
                // ── مشاركة ──
                inline constexpr std::string_view SHARE_TEXT = "شارك_نص";
                inline constexpr std::string_view SHARE_FILE = "شارك_ملف";
                // ── أذونات ──
                inline constexpr std::string_view REQUEST_PERM = "اطلب_اذن";
                inline constexpr std::string_view CHECK_PERM = "تحقق_اذن";
                // ── سمات وترجمة ──
                inline constexpr std::string_view SET_THEME = "عين_سمة";
                inline constexpr std::string_view GET_THEME = "احصل_سمة";
                inline constexpr std::string_view SYS_THEME = "سمة_النظام";
                inline constexpr std::string_view TRANSLATE = "ترجم";
                inline constexpr std::string_view SET_LOCALE = "عين_لغة";
                inline constexpr std::string_view GET_LOCALE = "اللغة_الحالية";
                inline constexpr std::string_view ADD_TRANSLATION = "أضف_ترجمة";
                // ── إمكانية الوصول ──
                inline constexpr std::string_view SCREEN_READER = "اقرأ_شاشة";
                inline constexpr std::string_view FONT_SCALE = "حجم_خط_النظام";
                // ── شاشة ──
                inline constexpr std::string_view SCREEN_WIDTH = "عرض_الشاشة";
                inline constexpr std::string_view SCREEN_HEIGHT = "ارتفاع_الشاشة";
                inline constexpr std::string_view SCREEN_DENSITY = "كثافة_الشاشة";
                inline constexpr std::string_view DEVICE_TYPE = "نوع_الجهاز";
                // ── طباعة ومصادقة ──
                inline constexpr std::string_view PRINT_PAGE = "اطبع_صفحة";
                inline constexpr std::string_view BIO_AUTH = "مصادقة_حيوية";
                inline constexpr std::string_view BIO_SUPPORT = "دعم_حيوي";
                // ── إيماءات ──
                inline constexpr std::string_view ON_SWIPE = "عند_سحب";
                inline constexpr std::string_view ON_PINCH = "عند_قرص";
                inline constexpr std::string_view ON_ROTATE = "عند_تدوير";
                // ── خريطة ولوحة رسم ──
                inline constexpr std::string_view CREATE_MAP = "أنشئ_خريطة";
                inline constexpr std::string_view ADD_MARKER = "أضف_علامة";
                inline constexpr std::string_view CREATE_CANVAS = "أنشئ_لوحة";
                inline constexpr std::string_view DRAW_LINE = "ارسم_خط";
                inline constexpr std::string_view DRAW_CIRCLE = "ارسم_دائرة";
                inline constexpr std::string_view DRAW_RECT = "ارسم_مستطيل";
                inline constexpr std::string_view CLEAR_CANVAS = "امسح_لوحة";
            }

            // ─────────── واجهة المستخدم — جهاز ───────────
            namespace UIDevice
            {
                inline constexpr std::string_view FULLSCREEN = "شاشة_كاملة";
                inline constexpr std::string_view IS_FULLSCREEN = "هل_شاشة_كاملة";
                inline constexpr std::string_view WINDOW_SIZE = "أبعاد_النافذة";
                inline constexpr std::string_view MOUSE_POS = "موقع_الفأرة";
                inline constexpr std::string_view DISPLAY_SCALE = "مقياس_العرض";
                inline constexpr std::string_view DEVICE_OS_NAME = "اسم_النظام";
                inline constexpr std::string_view CPU_COUNT = "عدد_المعالجات";
                inline constexpr std::string_view RAM_SIZE = "حجم_الذاكرة";
                inline constexpr std::string_view SYS_LANG = "لغة_النظام";
                inline constexpr std::string_view CURSOR_STYLE = "المؤشر";
                inline constexpr std::string_view SHOW_CURSOR = "أظهر_المؤشر";
                inline constexpr std::string_view OPEN_URL = "فتح_رابط";
                inline constexpr std::string_view DEVICE_TIMESTAMP = "الطابع_الزمني";
                inline constexpr std::string_view DATETIME = "التاريخ_والوقت";
                inline constexpr std::string_view VIBRATE = "اهتزاز";
                inline constexpr std::string_view LOG_FN = "سجل";
                inline constexpr std::string_view BATTERY_LEVEL = "مستوى_البطارية";
                inline constexpr std::string_view IS_CHARGING = "هل_يشحن";
            }

        } // namespace Names

        // ════════════════════════════════════════════════════════════════════
        // (AR) إدخال دالة مدمجة — كل دالة لها اسم واحد فقط
        // (EN) Builtin function entry — each function has exactly one name
        // ════════════════════════════════════════════════════════════════════

        struct BuiltinEntry
        {
            /// (AR) الاسم العربي الوحيد — لا أسماء بديلة
            /// (EN) The sole Arabic name — no aliases
            std::string_view canonicalName;

            /// (AR) تصنيف الدالة — يحدد هل تحتاج استيراد أم لا
            /// (EN) Function category — determines import requirement
            BuiltinCategory category;

            /// (AR) الوحدة المطلوبة (NONE إذا لا تحتاج استيراد)
            /// (EN) Required module (NONE if no import needed)
            ModuleId requiredModule;

            /// (AR) استراتيجية المترجم — كيف يولّد كود LLVM
            /// (EN) Compiler strategy — how to generate LLVM code
            CompilerStrategy compilerStrategy;
        };

        // ════════════════════════════════════════════════════════════════════
        //                    الطبقة 1: IO أساسي (بدون استيراد)
        //                    Layer 1: Core IO (no import)
        // ════════════════════════════════════════════════════════════════════

        inline constexpr std::array<BuiltinEntry, 3> CORE_IO_BUILTINS = {{
            {Names::Core::PRINT, BuiltinCategory::CORE_IO, ModuleId::NONE, CompilerStrategy::INLINE},
            {Names::Core::PRINTLN, BuiltinCategory::CORE_IO, ModuleId::NONE, CompilerStrategy::INLINE},
            {Names::Core::READ, BuiltinCategory::CORE_IO, ModuleId::NONE, CompilerStrategy::INLINE},
        }};

        // ════════════════════════════════════════════════════════════════════
        //                الطبقة 2: بانيات الأنواع (بدون استيراد)
        //                Layer 2: Type Constructors (no import)
        // ════════════════════════════════════════════════════════════════════

        inline constexpr std::array<BuiltinEntry, 4> TYPE_CONSTRUCTOR_BUILTINS = {{
            {Names::TypeCtor::TO_INT, BuiltinCategory::TYPE_CONSTRUCTOR, ModuleId::NONE, CompilerStrategy::INLINE},
            {Names::TypeCtor::TO_FLOAT, BuiltinCategory::TYPE_CONSTRUCTOR, ModuleId::NONE, CompilerStrategy::INLINE},
            {Names::TypeCtor::TO_STRING, BuiltinCategory::TYPE_CONSTRUCTOR, ModuleId::NONE, CompilerStrategy::INLINE},
            {Names::TypeCtor::TO_BOOL, BuiltinCategory::TYPE_CONSTRUCTOR, ModuleId::NONE, CompilerStrategy::INLINE},
        }};

        // ════════════════════════════════════════════════════════════════════
        //             الطبقة 3: دوال الوحدات (تحتاج استيراد)
        //             Layer 3: Module Functions (require import)
        // ════════════════════════════════════════════════════════════════════

        inline constexpr std::array<BuiltinEntry, 68> MODULE_FUNCTION_BUILTINS = {{
            // ═══════════════════════════════════════════════════════════
            // وحدة رياضيات (ModuleId::MATH) — 31 دالة
            // ═══════════════════════════════════════════════════════════
            {Names::Math::SQRT, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::POWER, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::ABS, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::ROUND, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::FLOOR, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::CEIL, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::SIN, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::COS, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::TAN, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::ASIN, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::ACOS, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::ATAN, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::ATAN2, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::LOG, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::LOG2, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::LOG10, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::EXP, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::PI, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::INLINE},
            {Names::Math::PI_ALT, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::INLINE},
            {Names::Math::E, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::INLINE},
            {Names::Math::RANDOM, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::RANDOM_INT, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::SQUARE, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::INLINE},
            {Names::Math::MAX, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::INLINE},
            {Names::Math::MIN, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::INLINE},
            {Names::Math::SUM, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::CLAMP, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::TRUNCATE, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::LLVM_INTRINSIC},
            {Names::Math::FMOD, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::SIGN, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::LERP, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::RANDOM_RANGE, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},
            {Names::Math::RANDOM_FLOAT, BuiltinCategory::MODULE_FUNCTION, ModuleId::MATH, CompilerStrategy::RUNTIME_CALL},

            // ═══════════════════════════════════════════════════════════
            // وحدة نصوص (ModuleId::STRINGS) — 13 دالة
            // ═══════════════════════════════════════════════════════════
            {Names::Strings::STR_LENGTH, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::CHAR_CODE, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::FIND, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::REPLACE, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::SUBSTRING, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::TRIM, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::TO_UPPER, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::TO_LOWER, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::SPLIT, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::JOIN, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::STARTS_WITH, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::ENDS_WITH, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},
            {Names::Strings::CONTAINS, BuiltinCategory::MODULE_FUNCTION, ModuleId::STRINGS, CompilerStrategy::RUNTIME_CALL},

            // ═══════════════════════════════════════════════════════════
            // وحدة أساسيات (ModuleId::BASICS)
            // ═══════════════════════════════════════════════════════════
            {Names::Basics::RANGE, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::EXIT, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::ASSERT, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::READ_FILE, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::WRITE_FILE, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::FILE_EXISTS, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::IS_FILE, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::IS_DIR, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::MKDIR, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},
            {Names::Basics::DELETE_FILE, BuiltinCategory::MODULE_FUNCTION, ModuleId::BASICS, CompilerStrategy::RUNTIME_CALL},

            // ═══════════════════════════════════════════════════════════
            // وحدة تأكيدات (ModuleId::ASSERTIONS)
            // ═══════════════════════════════════════════════════════════
            {Names::Assertions::ASSERT_EQ, BuiltinCategory::MODULE_FUNCTION, ModuleId::ASSERTIONS, CompilerStrategy::RUNTIME_CALL},
            {Names::Assertions::ASSERT_NEQ, BuiltinCategory::MODULE_FUNCTION, ModuleId::ASSERTIONS, CompilerStrategy::RUNTIME_CALL},
            {Names::Assertions::ASSERT_TRUE, BuiltinCategory::MODULE_FUNCTION, ModuleId::ASSERTIONS, CompilerStrategy::RUNTIME_CALL},
            {Names::Assertions::ASSERT_FALSE, BuiltinCategory::MODULE_FUNCTION, ModuleId::ASSERTIONS, CompilerStrategy::RUNTIME_CALL},
            {Names::Assertions::HASH, BuiltinCategory::MODULE_FUNCTION, ModuleId::ASSERTIONS, CompilerStrategy::RUNTIME_CALL},

            // ═══════════════════════════════════════════════════════════
            // وحدة خرائط (ModuleId::MAPS)
            // ═══════════════════════════════════════════════════════════
            {Names::Maps::JSON_PARSE, BuiltinCategory::MODULE_FUNCTION, ModuleId::MAPS, CompilerStrategy::RUNTIME_CALL},
            {Names::Maps::JSON_STRINGIFY, BuiltinCategory::MODULE_FUNCTION, ModuleId::MAPS, CompilerStrategy::RUNTIME_CALL},
            {Names::Maps::REGEX, BuiltinCategory::MODULE_FUNCTION, ModuleId::MAPS, CompilerStrategy::RUNTIME_CALL},
            {Names::Maps::NOW, BuiltinCategory::MODULE_FUNCTION, ModuleId::MAPS, CompilerStrategy::RUNTIME_CALL},
        }};

        // ════════════════════════════════════════════════════════════════════
        //              الطبقة 4: طرق الأنواع المدمجة (بدون استيراد)
        //              Layer 4: Built-in Type Methods (no import)
        // ════════════════════════════════════════════════════════════════════

        struct TypeMethodEntry
        {
            std::string_view methodName; /// (AR) اسم الطريقة / (EN) Method name
            TargetType targetType;       /// (AR) نوع الكائن المستهدف / (EN) Target object type
        };

        inline constexpr std::array<TypeMethodEntry, 32> TYPE_METHOD_BUILTINS = {{
            // ─── طرق المصفوفات ───
            {"اضف", TargetType::ARRAY},
            {"احذف_الأخير", TargetType::ARRAY},
            {"رتب", TargetType::ARRAY},
            {"اعكس", TargetType::ARRAY},
            {"يحتوي", TargetType::ARRAY},
            {"فهرس", TargetType::ARRAY},
            {"شريحة", TargetType::ARRAY},
            {"خريطة", TargetType::ARRAY},
            {"رشح", TargetType::ARRAY},
            {"اختزل", TargetType::ARRAY},
            {"لكل", TargetType::ARRAY},
            {"الأول", TargetType::ARRAY},
            {"الأخير", TargetType::ARRAY},
            {"فارغة", TargetType::ARRAY},
            {"الطول", TargetType::ARRAY},
            {"مسطح", TargetType::ARRAY},

            // ─── طرق النصوص ───
            {"طول", TargetType::STRING},
            {"يحتوي", TargetType::STRING},
            {"يبدأ_بـ", TargetType::STRING},
            {"ينتهي_بـ", TargetType::STRING},
            {"تقسيم", TargetType::STRING},
            {"استبدل", TargetType::STRING},
            {"استخراج", TargetType::STRING},
            {"قص_أطراف", TargetType::STRING},
            {"تحويل_كبير", TargetType::STRING},
            {"تحويل_صغير", TargetType::STRING},

            // ─── طرق الخرائط ───
            {"احصل", TargetType::MAP},
            {"عين", TargetType::MAP},
            {"احذف", TargetType::MAP},
            {"مفاتيح", TargetType::MAP},
            {"قيم", TargetType::MAP},
            {"حجم", TargetType::MAP},

            // ─── طرق مشتركة لجميع الأنواع ───
            // (AR) .الطول() و .النوع() يعملان على أي نوع يدعمها
            // (EN) .الطول() and .النوع() work on any supporting type
        }};

        // ════════════════════════════════════════════════════════════════════
        // (AR) دوال البحث في السجل — وقت التشغيل
        // (EN) Registry lookup functions — runtime
        // ════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) البحث عن دالة بالاسم في جميع الطبقات
         * @brief (EN) Look up a function by name across all layers
         * @return مؤشر إلى الإدخال أو nullptr / Pointer to entry or nullptr
         */
        inline const BuiltinEntry *findBuiltinByName(std::string_view name)
        {
            // (AR) البحث في IO أساسي
            for (const auto &entry : CORE_IO_BUILTINS)
            {
                if (entry.canonicalName == name)
                    return &entry;
            }

            // (AR) البحث في بانيات الأنواع
            for (const auto &entry : TYPE_CONSTRUCTOR_BUILTINS)
            {
                if (entry.canonicalName == name)
                    return &entry;
            }

            // (AR) البحث في دوال الوحدات
            for (const auto &entry : MODULE_FUNCTION_BUILTINS)
            {
                if (entry.canonicalName == name)
                    return &entry;
            }

            return nullptr;
        }

        /**
         * @brief (AR) هل الدالة متاحة بدون استيراد؟
         * @brief (EN) Is the function available without import?
         */
        inline bool isAvailableWithoutImport(std::string_view name)
        {
            const auto *entry = findBuiltinByName(name);
            if (!entry)
                return false;
            return entry->category == BuiltinCategory::CORE_IO ||
                   entry->category == BuiltinCategory::TYPE_CONSTRUCTOR;
        }

        /**
         * @brief (AR) ما الوحدة المطلوبة لهذه الدالة؟
         * @brief (EN) What module is required for this function?
         * @return معرّف الوحدة أو NONE / Module ID or NONE
         */
        inline ModuleId getRequiredModule(std::string_view name)
        {
            const auto *entry = findBuiltinByName(name);
            if (!entry)
                return ModuleId::NONE;
            return entry->requiredModule;
        }

        /**
         * @brief (AR) اقتراح الوحدة المناسبة لدالة غير مستوردة (لرسائل الخطأ)
         * @brief (EN) Suggest the appropriate module for an unimported function (for error messages)
         * @return اسم الوحدة أو فراغ / Module name or empty
         */
        inline std::string_view suggestModuleForFunction(std::string_view name)
        {
            const auto *entry = findBuiltinByName(name);
            if (!entry || entry->requiredModule == ModuleId::NONE)
                return "";
            return getCanonicalModuleName(entry->requiredModule);
        }

        // ════════════════════════════════════════════════════════════════════
        // (AR) تحقق وقت الترجمة — static_assert
        // (EN) Compile-time validation — static_assert
        // ════════════════════════════════════════════════════════════════════

        /// (AR) التحقق من أن السجل ليس فارغاً
        /// (EN) Verify registry is not empty
        static_assert(CORE_IO_BUILTINS.size() == 3,
                      "CORE_IO_BUILTINS must have exactly 3 entries (اطبع, اطبع_سطر, اقرأ)");

        static_assert(TYPE_CONSTRUCTOR_BUILTINS.size() == 4,
                      "TYPE_CONSTRUCTOR_BUILTINS must have exactly 4 entries (رقم, عشري, نص, منطقي)");

        /// (AR) التحقق من أن عدد الوحدات يتطابق مع التعداد
        /// (EN) Verify module count matches enum
        static_assert(static_cast<uint8_t>(ModuleId::_MODULE_COUNT) == 11,
                      "ModuleId count mismatch — update MODULE_ALIASES when adding new modules");

    } // namespace Builtins
} // namespace Sad
