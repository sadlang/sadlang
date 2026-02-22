/**
 * @file sad_hijri.cpp
 * @brief تنفيذ التقويم الهجري للغة البرمجة العربية ساد
 * 
 * هذا الملف يحتوي على التنفيذ الكامل لمكتبة التقويم الهجري
 * يستخدم الخوارزمية الكويتية/الجدولية للتحويل بين التقويمين الميلادي والهجري
 * 
 * الخوارزمية:
 *   ١. تحويل التاريخ الميلادي إلى رقم اليوم اليولياني
 *   ٢. تحويل رقم اليوم اليولياني إلى التاريخ الهجري باستخدام التقويم الجدولي
 * 
 * حقبة التقويم الهجري: رقم اليوم اليولياني 1948439.5
 * (الموافق 16 يوليو 622 ميلادي - هجرة النبي محمد ﷺ)
 * 
 * دورة الـ 30 سنة: السنوات الكبيسة هي 2، 5، 7، 10، 13، 16، 18، 21، 24، 26، 29
 * السنة العادية: 354 يومًا | السنة الكبيسة: 355 يومًا
 * الأشهر الفردية: 30 يومًا | الأشهر الزوجية: 29 يومًا
 * الشهر الثاني عشر في السنة الكبيسة: 30 يومًا
 * 
 * @author مطور لغة ساد
 * @date 2026
 */

#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include "graphics/sad_hijri.h"

namespace sad {
namespace hijri {

// ============================================================
// ثوابت التقويم الهجري
// ============================================================

/// حقبة التقويم الهجري بحساب اليوم اليولياني
/// تمثل بداية أول يوم في التقويم الهجري (1 محرم سنة 1)
static const double HIJRI_EPOCH = 1948439.5;

/// السنوات الكبيسة في دورة الـ 30 سنة
/// هذه السنوات تحتوي على 355 يومًا بدلاً من 354
static const int LEAP_YEARS_IN_CYCLE[] = {2, 5, 7, 10, 13, 16, 18, 21, 24, 26, 29};
static const int LEAP_YEARS_COUNT = 11;

// ============================================================
// أسماء الأشهر الهجرية بالعربية والإنجليزية
// ============================================================

/// أسماء الأشهر الهجرية الاثني عشر باللغة العربية
static const std::string MONTH_NAMES_AR[] = {
    "",               // الفهرس صفر غير مستخدم - الأشهر تبدأ من 1
    "محرم",           // الشهر الأول - من الأشهر الحرم
    "صفر",            // الشهر الثاني
    "ربيع الأول",     // الشهر الثالث - شهر المولد النبوي
    "ربيع الثاني",    // الشهر الرابع
    "جمادى الأولى",   // الشهر الخامس
    "جمادى الآخرة",   // الشهر السادس
    "رجب",            // الشهر السابع - من الأشهر الحرم
    "شعبان",          // الشهر الثامن
    "رمضان",          // الشهر التاسع - شهر الصيام
    "شوال",           // الشهر العاشر - شهر عيد الفطر
    "ذو القعدة",      // الشهر الحادي عشر - من الأشهر الحرم
    "ذو الحجة"        // الشهر الثاني عشر - شهر الحج وعيد الأضحى
};

/// أسماء الأشهر الهجرية باللغة الإنجليزية (للتوافق الدولي)
static const std::string MONTH_NAMES_EN[] = {
    "",
    "Muharram",
    "Safar",
    "Rabi al-Awwal",
    "Rabi al-Thani",
    "Jumada al-Ula",
    "Jumada al-Akhirah",
    "Rajab",
    "Shaban",
    "Ramadan",
    "Shawwal",
    "Dhul-Qadah",
    "Dhul-Hijjah"
};

/// أسماء أيام الأسبوع باللغة العربية
/// تبدأ من الأحد (0) إلى السبت (6)
static const std::string DAY_NAMES_AR[] = {
    "الأحد",      // 0 - أول أيام الأسبوع
    "الإثنين",    // 1
    "الثلاثاء",   // 2
    "الأربعاء",   // 3
    "الخميس",     // 4
    "الجمعة",     // 5 - يوم الجمعة المبارك
    "السبت"       // 6 - آخر أيام الأسبوع
};

// ============================================================
// المناسبات الإسلامية على مدار السنة الهجرية
// ============================================================

/// قائمة المناسبات والأعياد الإسلامية المعروفة
/// تشمل الأعياد الرسمية والمناسبات الدينية المهمة
static const std::vector<IslamicEvent>& getIslamicEvents() {
    static const std::vector<IslamicEvent> events = {
        // === شهر محرم (الشهر الأول) ===
        {1, 1,   "رأس السنة الهجرية",        "Islamic New Year"},
        {1, 10,  "يوم عاشوراء",              "Day of Ashura"},

        // === شهر صفر (الشهر الثاني) ===
        {2, 20,  "أربعينية الإمام الحسين",    "Arbaeen of Imam Hussein"},

        // === شهر ربيع الأول (الشهر الثالث) ===
        {3, 1,   "بداية شهر ربيع الأول",     "Start of Rabi al-Awwal"},
        {3, 12,  "المولد النبوي الشريف",      "Prophet's Birthday (Mawlid)"},

        // === شهر رجب (الشهر السابع) ===
        {7, 1,   "بداية شهر رجب الحرام",     "Start of Rajab"},
        {7, 27,  "الإسراء والمعراج",          "Isra and Mi'raj"},

        // === شهر شعبان (الشهر الثامن) ===
        {8, 15,  "ليلة النصف من شعبان",       "Mid-Shaban Night"},

        // === شهر رمضان (الشهر التاسع) ===
        {9, 1,   "بداية شهر رمضان المبارك",   "Start of Ramadan"},
        {9, 17,  "ذكرى غزوة بدر الكبرى",     "Battle of Badr"},
        {9, 21,  "ذكرى فتح مكة المكرمة",     "Conquest of Mecca"},
        {9, 27,  "ليلة القدر (المرجحة)",      "Laylat al-Qadr (probable)"},

        // === شهر شوال (الشهر العاشر) ===
        {10, 1,  "عيد الفطر المبارك (اليوم الأول)",    "Eid al-Fitr (Day 1)"},
        {10, 2,  "عيد الفطر المبارك (اليوم الثاني)",   "Eid al-Fitr (Day 2)"},
        {10, 3,  "عيد الفطر المبارك (اليوم الثالث)",   "Eid al-Fitr (Day 3)"},

        // === شهر ذو القعدة (الشهر الحادي عشر) ===
        {11, 1,  "بداية شهر ذو القعدة الحرام", "Start of Dhul-Qadah"},

        // === شهر ذو الحجة (الشهر الثاني عشر) ===
        {12, 1,  "بداية شهر ذو الحجة",        "Start of Dhul-Hijjah"},
        {12, 8,  "يوم التروية",               "Day of Tarwiyah"},
        {12, 9,  "يوم عرفة",                  "Day of Arafah"},
        {12, 10, "عيد الأضحى المبارك (اليوم الأول)",    "Eid al-Adha (Day 1)"},
        {12, 11, "عيد الأضحى المبارك (اليوم الثاني)",   "Eid al-Adha (Day 2)"},
        {12, 12, "عيد الأضحى المبارك (اليوم الثالث)",   "Eid al-Adha (Day 3)"},
        {12, 13, "أيام التشريق (اليوم الأول)",  "Days of Tashreeq (Day 1)"}
    };
    return events;
}

// ============================================================
// دوال مساعدة داخلية
// ============================================================

/**
 * @brief تحويل التاريخ الميلادي إلى رقم اليوم اليولياني
 * 
 * يستخدم خوارزمية التحويل القياسية من التاريخ الميلادي
 * إلى رقم اليوم اليولياني (Julian Day Number)
 * 
 * @param year السنة الميلادية
 * @param month الشهر الميلادي (1-12)
 * @param day اليوم (1-31)
 * @return رقم اليوم اليولياني كعدد عشري
 */
static double gregorianToJDN(int year, int month, int day) {
    // تعديل الشهر والسنة وفق خوارزمية التحويل
    // إذا كان الشهر يناير أو فبراير، نعتبره شهر 13 أو 14 من السنة السابقة
    if (month <= 2) {
        year -= 1;
        month += 12;
    }

    // حساب معامل التصحيح الميلادي (فرق التقويم اليولياني والميلادي)
    int A = year / 100;
    int B = 2 - A + (A / 4);

    // حساب رقم اليوم اليولياني باستخدام الصيغة القياسية
    double jdn = std::floor(365.25 * (year + 4716))
               + std::floor(30.6001 * (month + 1))
               + day + B - 1524.5;

    return jdn;
}

/**
 * @brief تحويل رقم اليوم اليولياني إلى تاريخ ميلادي
 * 
 * العملية العكسية للتحويل من رقم اليوم اليولياني
 * إلى التاريخ الميلادي (السنة والشهر واليوم)
 * 
 * @param jdn رقم اليوم اليولياني
 * @return بنية التاريخ الميلادي
 */
static GregorianDate jdnToGregorian(double jdn) {
    // إضافة نصف يوم للتقريب الصحيح
    double z = std::floor(jdn + 0.5);
    double a = std::floor((z - 1867216.25) / 36524.25);
    double aa = z + 1 + a - std::floor(a / 4.0);
    double b = aa + 1524;
    double c = std::floor((b - 122.1) / 365.25);
    double d = std::floor(365.25 * c);
    double e = std::floor((b - d) / 30.6001);

    // حساب اليوم من الشهر
    int day = static_cast<int>(b - d - std::floor(30.6001 * e));

    // حساب الشهر مع مراعاة الأشهر التي تتجاوز 12
    int month;
    if (e < 14) {
        month = static_cast<int>(e) - 1;
    } else {
        month = static_cast<int>(e) - 13;
    }

    // حساب السنة مع مراعاة بداية السنة
    int year;
    if (month > 2) {
        year = static_cast<int>(c) - 4716;
    } else {
        year = static_cast<int>(c) - 4715;
    }

    GregorianDate result;
    result.year = year;
    result.month = month;
    result.day = day;
    return result;
}

// ============================================================
// التحقق من السنة الكبيسة الهجرية
// ============================================================

/**
 * @brief التحقق مما إذا كانت السنة الهجرية كبيسة
 * 
 * في التقويم الهجري الجدولي، الدورة تتكون من 30 سنة
 * منها 11 سنة كبيسة (355 يومًا) و19 سنة عادية (354 يومًا)
 * السنوات الكبيسة في الدورة هي: 2، 5، 7، 10، 13، 16، 18، 21، 24، 26، 29
 * 
 * @param hYear السنة الهجرية
 * @return صحيح إذا كانت السنة كبيسة
 */
bool isLeapYear(int hYear) {
    // حساب موقع السنة في دورة الـ 30 سنة
    int positionInCycle = ((hYear - 1) % 30) + 1;

    // التحقق مما إذا كان الموقع ضمن السنوات الكبيسة
    for (int i = 0; i < LEAP_YEARS_COUNT; ++i) {
        if (positionInCycle == LEAP_YEARS_IN_CYCLE[i]) {
            return true;
        }
    }
    return false;
}

// ============================================================
// حساب عدد أيام الشهر الهجري
// ============================================================

/**
 * @brief الحصول على عدد الأيام في شهر هجري معين
 * 
 * القاعدة العامة:
 *   - الأشهر الفردية (1، 3، 5، 7، 9، 11): 30 يومًا
 *   - الأشهر الزوجية (2، 4، 6، 8، 10، 12): 29 يومًا
 *   - الاستثناء: الشهر الثاني عشر (ذو الحجة) في السنة الكبيسة: 30 يومًا
 * 
 * @param hYear السنة الهجرية (لتحديد ما إذا كانت كبيسة)
 * @param hMonth الشهر الهجري (1-12)
 * @return عدد أيام الشهر
 */
int getDaysInMonth(int hYear, int hMonth) {
    // التحقق من صحة رقم الشهر
    if (hMonth < 1 || hMonth > 12) return 0;

    // الأشهر الفردية: 30 يومًا
    if (hMonth % 2 == 1) return 30;

    // الشهر الثاني عشر في السنة الكبيسة: 30 يومًا بدلاً من 29
    if (hMonth == 12 && isLeapYear(hYear)) return 30;

    // باقي الأشهر الزوجية: 29 يومًا
    return 29;
}

// ============================================================
// تحويل التاريخ الميلادي إلى هجري
// ============================================================

/**
 * @brief تحويل تاريخ ميلادي إلى تاريخ هجري
 * 
 * تستخدم الخوارزمية الكويتية/الجدولية:
 *   ١. تحويل التاريخ الميلادي إلى رقم اليوم اليولياني
 *   ٢. طرح حقبة التقويم الهجري للحصول على عدد الأيام منذ بداية التقويم
 *   ٣. حساب الدورات الكاملة (كل دورة 30 سنة = 10631 يومًا)
 *   ٤. حساب السنوات المتبقية داخل الدورة
 *   ٥. حساب الشهر واليوم
 * 
 * @param gYear السنة الميلادية
 * @param gMonth الشهر الميلادي (1-12)
 * @param gDay اليوم الميلادي (1-31)
 * @return بنية التاريخ الهجري
 */
HijriDate gregorianToHijri(int gYear, int gMonth, int gDay) {
    // الخطوة الأولى: تحويل التاريخ الميلادي إلى رقم اليوم اليولياني
    double jdn = gregorianToJDN(gYear, gMonth, gDay);

    // الخطوة الثانية: حساب عدد الأيام منذ حقبة التقويم الهجري
    double daysSinceEpoch = std::floor(jdn) - std::floor(HIJRI_EPOCH);

    // الخطوة الثالثة: حساب الدورات الكاملة
    // كل دورة = 30 سنة هجرية = 10631 يومًا
    int cycles = static_cast<int>(std::floor(daysSinceEpoch / 10631.0));
    int remainingDays = static_cast<int>(daysSinceEpoch - cycles * 10631);

    // الخطوة الرابعة: حساب السنوات داخل الدورة الحالية
    int yearInCycle = 0;
    for (int y = 1; y <= 30; ++y) {
        int daysInYear = 354;
        // التحقق مما إذا كانت هذه السنة كبيسة في الدورة
        for (int i = 0; i < LEAP_YEARS_COUNT; ++i) {
            if (y == LEAP_YEARS_IN_CYCLE[i]) {
                daysInYear = 355;
                break;
            }
        }
        if (remainingDays < daysInYear) {
            yearInCycle = y;
            break;
        }
        remainingDays -= daysInYear;
        if (y == 30) {
            yearInCycle = 30;
        }
    }

    // حساب السنة الهجرية الكاملة
    int hYear = cycles * 30 + yearInCycle;

    // الخطوة الخامسة: حساب الشهر واليوم
    int hMonth = 1;
    for (int m = 1; m <= 12; ++m) {
        int daysInMonth = getDaysInMonth(hYear, m);
        if (remainingDays < daysInMonth) {
            hMonth = m;
            break;
        }
        remainingDays -= daysInMonth;
        if (m == 12) {
            hMonth = 12;
        }
    }

    // اليوم يبدأ من 1 (وليس من صفر)
    int hDay = remainingDays + 1;

    HijriDate result;
    result.year = hYear;
    result.month = hMonth;
    result.day = hDay;
    return result;
}

// ============================================================
// تحويل التاريخ الهجري إلى ميلادي
// ============================================================

/**
 * @brief تحويل تاريخ هجري إلى تاريخ ميلادي
 * 
 * العملية العكسية للتحويل:
 *   ١. حساب عدد الأيام من بداية التقويم الهجري إلى التاريخ المحدد
 *   ٢. إضافة حقبة التقويم الهجري للحصول على رقم اليوم اليولياني
 *   ٣. تحويل رقم اليوم اليولياني إلى تاريخ ميلادي
 * 
 * @param hYear السنة الهجرية
 * @param hMonth الشهر الهجري (1-12)
 * @param hDay اليوم الهجري
 * @return بنية التاريخ الميلادي
 */
GregorianDate hijriToGregorian(int hYear, int hMonth, int hDay) {
    // حساب عدد الأيام للسنوات الكاملة السابقة
    // نحسب عدد الدورات الكاملة (30 سنة لكل دورة)
    int prevYears = hYear - 1;
    int fullCycles = prevYears / 30;
    int remainingYears = prevYears % 30;

    // كل دورة كاملة = 10631 يومًا
    long totalDays = fullCycles * 10631L;

    // إضافة أيام السنوات المتبقية في الدورة الحالية
    for (int y = 1; y <= remainingYears; ++y) {
        bool leap = false;
        for (int i = 0; i < LEAP_YEARS_COUNT; ++i) {
            if (y == LEAP_YEARS_IN_CYCLE[i]) {
                leap = true;
                break;
            }
        }
        totalDays += leap ? 355 : 354;
    }

    // إضافة أيام الأشهر الكاملة السابقة في السنة الحالية
    for (int m = 1; m < hMonth; ++m) {
        totalDays += getDaysInMonth(hYear, m);
    }

    // إضافة الأيام في الشهر الحالي (اليوم يبدأ من 1)
    totalDays += (hDay - 1);

    // تحويل إلى رقم اليوم اليولياني بإضافة الحقبة
    double jdn = totalDays + HIJRI_EPOCH;

    // تحويل رقم اليوم اليولياني إلى تاريخ ميلادي
    return jdnToGregorian(jdn);
}

// ============================================================
// أسماء الأشهر والأيام
// ============================================================

/**
 * @brief الحصول على اسم الشهر الهجري بالعربية
 * @param month رقم الشهر (1-12)
 * @return اسم الشهر بالعربية، أو نص فارغ إذا كان الرقم غير صحيح
 */
std::string getMonthNameAr(int month) {
    if (month < 1 || month > 12) return "";
    return MONTH_NAMES_AR[month];
}

/**
 * @brief الحصول على اسم الشهر الهجري بالإنجليزية
 * @param month رقم الشهر (1-12)
 * @return اسم الشهر بالإنجليزية
 */
std::string getMonthNameEn(int month) {
    if (month < 1 || month > 12) return "";
    return MONTH_NAMES_EN[month];
}

/**
 * @brief الحصول على اسم اليوم في الأسبوع بالعربية
 * @param dayOfWeek رقم اليوم (0=الأحد، 1=الإثنين، ... 6=السبت)
 * @return اسم اليوم بالعربية
 */
std::string getDayNameAr(int dayOfWeek) {
    // ضمان أن الرقم ضمن النطاق الصحيح (0-6)
    int idx = ((dayOfWeek % 7) + 7) % 7;
    return DAY_NAMES_AR[idx];
}

// ============================================================
// تنسيق التاريخ الهجري
// ============================================================

/**
 * @brief تنسيق التاريخ الهجري بالعربية
 * 
 * الصيغة: "اليوم شهر_بالعربية سنة هـ"
 * مثال: "12 ربيع الأول 1445 هـ"
 * 
 * @param hYear السنة الهجرية
 * @param hMonth الشهر الهجري
 * @param hDay اليوم الهجري
 * @return النص المنسق بالعربية
 */
std::string formatHijriAr(int hYear, int hMonth, int hDay) {
    std::ostringstream oss;
    oss << hDay << " " << getMonthNameAr(hMonth) << " " << hYear << " هـ";
    return oss.str();
}

/**
 * @brief تنسيق التاريخ الهجري بالإنجليزية
 * 
 * الصيغة: "day MonthName year AH"
 * مثال: "12 Rabi al-Awwal 1445 AH"
 * 
 * @param hYear السنة الهجرية
 * @param hMonth الشهر الهجري
 * @param hDay اليوم الهجري
 * @return النص المنسق بالإنجليزية
 */
std::string formatHijriEn(int hYear, int hMonth, int hDay) {
    std::ostringstream oss;
    oss << hDay << " " << getMonthNameEn(hMonth) << " " << hYear << " AH";
    return oss.str();
}

// ============================================================
// الحصول على التاريخ الهجري الحالي
// ============================================================

/**
 * @brief الحصول على التاريخ الهجري لليوم الحالي
 * 
 * يستخدم مكتبة <ctime> للحصول على التاريخ الميلادي الحالي
 * ثم يحوله إلى التاريخ الهجري باستخدام الخوارزمية الجدولية
 * 
 * @return بنية التاريخ الهجري لليوم
 */
HijriDate getToday() {
    // الحصول على الوقت الحالي من النظام
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);

    // استخراج التاريخ الميلادي
    // ملاحظة: tm_year يبدأ من 1900، و tm_mon يبدأ من 0
    int gYear = localTime->tm_year + 1900;
    int gMonth = localTime->tm_mon + 1;
    int gDay = localTime->tm_mday;

    // تحويل التاريخ الميلادي إلى هجري
    return gregorianToHijri(gYear, gMonth, gDay);
}

/**
 * @brief الحصول على التاريخ الهجري لليوم بصيغة نصية عربية منسقة
 * 
 * يُرجع التاريخ الهجري الحالي بالتنسيق العربي
 * مثال: "15 رمضان 1447 هـ"
 * 
 * @return النص المنسق للتاريخ الهجري الحالي
 */
std::string getTodayFormatted() {
    HijriDate today = getToday();
    return formatHijriAr(today.year, today.month, today.day);
}

// ============================================================
// البحث عن المناسبات الإسلامية
// ============================================================

/**
 * @brief الحصول على جميع المناسبات في شهر هجري معين
 * 
 * يبحث في قائمة المناسبات الإسلامية عن جميع المناسبات
 * التي تقع في الشهر المحدد ويُرجعها في قائمة
 * 
 * @param hMonth رقم الشهر الهجري (1-12)
 * @return قائمة بالمناسبات الإسلامية في الشهر المحدد
 */
std::vector<IslamicEvent> getEventsInMonth(int hMonth) {
    std::vector<IslamicEvent> result;
    const auto& allEvents = getIslamicEvents();

    // البحث عن جميع المناسبات في الشهر المطلوب
    for (const auto& event : allEvents) {
        if (event.month == hMonth) {
            result.push_back(event);
        }
    }

    return result;
}

/**
 * @brief التحقق مما إذا كان هناك مناسبة في تاريخ هجري معين
 * 
 * يبحث في قائمة المناسبات عن تطابق الشهر واليوم
 * 
 * @param hMonth رقم الشهر الهجري (1-12)
 * @param hDay رقم اليوم (1-30)
 * @return صحيح إذا وُجدت مناسبة في هذا التاريخ
 */
bool hasEvent(int hMonth, int hDay) {
    const auto& allEvents = getIslamicEvents();

    // البحث عن أي مناسبة تطابق الشهر واليوم
    for (const auto& event : allEvents) {
        if (event.month == hMonth && event.day == hDay) {
            return true;
        }
    }

    return false;
}

/**
 * @brief الحصول على اسم المناسبة في تاريخ هجري معين
 * 
 * يبحث عن أول مناسبة تطابق الشهر واليوم المحددين
 * ويُرجع اسمها بالعربية
 * 
 * إذا كان هناك أكثر من مناسبة في نفس اليوم، يُرجع الأولى فقط
 * 
 * @param hMonth رقم الشهر الهجري (1-12)
 * @param hDay رقم اليوم (1-30)
 * @return اسم المناسبة بالعربية، أو نص فارغ إذا لم تُوجد مناسبة
 */
std::string getEventName(int hMonth, int hDay) {
    const auto& allEvents = getIslamicEvents();

    // البحث عن المناسبة المطابقة وإرجاع اسمها بالعربية
    for (const auto& event : allEvents) {
        if (event.month == hMonth && event.day == hDay) {
            return event.nameAr;
        }
    }

    // لم يتم العثور على مناسبة في هذا التاريخ
    return "";
}

} // نهاية فضاء الأسماء hijri
} // نهاية فضاء الأسماء sad
