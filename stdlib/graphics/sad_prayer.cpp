// ============================================================================
// sad_prayer.cpp — حاسبة مواقيت الصلاة للغة البرمجة العربية ساد
// ============================================================================
// هذا الملف يحتوي على التنفيذ الكامل لحساب مواقيت الصلاة الخمس
// باستخدام الحسابات الفلكية الدقيقة المعتمدة على موقع الشمس
// ============================================================================
// المؤلف: مشروع لغة ساد
// الوصف: حساب مواقيت الصلاة بناءً على الإحداثيات الجغرافية والتاريخ
//         مع دعم طرق حساب متعددة (أم القرى، رابطة العالم الإسلامي، إلخ)
// ============================================================================

#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include "graphics/sad_prayer.h"

namespace sad {
namespace prayer {

// ============================================================================
// الثوابت الرياضية والفلكية
// ============================================================================

// ثابت النسبة التقريبية (باي) — يُستخدم في تحويل الزوايا
static const double PI = 3.14159265358979323846;

// ============================================================================
// دوال مساعدة لتحويل الزوايا
// ============================================================================

/// تحويل الدرجات إلى راديان
/// الراديان = الدرجات × باي ÷ 180
static double degreesToRadians(double degrees) {
    return degrees * PI / 180.0;
}

/// تحويل الراديان إلى درجات
/// الدرجات = الراديان × 180 ÷ باي
static double radiansToDegrees(double radians) {
    return radians * 180.0 / PI;
}

/// حساب جيب الزاوية بالدرجات (بدلاً من الراديان)
static double sinDeg(double degrees) {
    return std::sin(degreesToRadians(degrees));
}

/// حساب جيب التمام بالدرجات
static double cosDeg(double degrees) {
    return std::cos(degreesToRadians(degrees));
}

/// حساب الظل بالدرجات
static double tanDeg(double degrees) {
    return std::tan(degreesToRadians(degrees));
}

/// حساب معكوس جيب التمام وإرجاع النتيجة بالدرجات
static double acosDeg(double value) {
    return radiansToDegrees(std::acos(value));
}

/// حساب معكوس الظل ذو المعاملين وإرجاع النتيجة بالدرجات
static double atan2Deg(double y, double x) {
    return radiansToDegrees(std::atan2(y, x));
}

/// تطبيع الزاوية لتكون ضمن النطاق [0، 360)
/// هذا ضروري لأن الحسابات الفلكية قد تنتج زوايا خارج هذا النطاق
static double fixAngle(double angle) {
    angle = std::fmod(angle, 360.0);
    if (angle < 0.0) angle += 360.0;
    return angle;
}

/// تطبيع الوقت ليكون ضمن النطاق [0، 24) ساعة
/// يضمن أن الوقت الناتج يقع ضمن يوم واحد
static double fixHour(double hour) {
    hour = std::fmod(hour, 24.0);
    if (hour < 0.0) hour += 24.0;
    return hour;
}

// ============================================================================
// حساب رقم اليوم اليولياني (Julian Day Number)
// ============================================================================
// رقم اليوم اليولياني هو عدد الأيام المنقضية منذ بداية التقويم اليولياني
// وهو أساسي في الحسابات الفلكية لأنه يوفر مرجعاً زمنياً موحداً
// ============================================================================

/// حساب رقم اليوم اليولياني من التاريخ الميلادي
/// المعادلة مأخوذة من الخوارزمية الفلكية المعتمدة
/// المدخلات: السنة، الشهر، اليوم (ميلادي)
/// المخرجات: رقم اليوم اليولياني (عدد عشري)
static double julianDay(int year, int month, int day) {
    // إذا كان الشهر يناير أو فبراير، نعتبره الشهر 13 أو 14 من السنة السابقة
    // هذا جزء من خوارزمية التحويل المعتمدة
    if (month <= 2) {
        year -= 1;
        month += 12;
    }

    // حساب معامل التصحيح الغريغوري
    // هذا التصحيح يعوض الفرق بين التقويم اليولياني والغريغوري
    double A = std::floor(static_cast<double>(year) / 100.0);
    double B = 2.0 - A + std::floor(A / 4.0);

    // المعادلة الرئيسية لحساب رقم اليوم اليولياني
    double JD = std::floor(365.25 * (year + 4716))
              + std::floor(30.6001 * (month + 1))
              + day + B - 1524.5;

    return JD;
}

// ============================================================================
// حساب موقع الشمس — الحسابات الفلكية الأساسية
// ============================================================================
// هذه الحسابات تحدد موقع الشمس في السماء بناءً على رقم اليوم اليولياني
// وهي ضرورية لتحديد أوقات الشروق والغروب وبالتالي مواقيت الصلاة
// ============================================================================

/// حساب الميل الشمسي (Sun Declination)
/// الميل الشمسي هو الزاوية بين أشعة الشمس ومستوى خط الاستواء
/// يتراوح بين -23.45° (في الانقلاب الشتوي) و+23.45° (في الانقلاب الصيفي)
/// المدخلات: رقم اليوم اليولياني
/// المخرجات: الميل الشمسي بالدرجات
static double sunDeclination(double jd) {
    // حساب عدد القرون اليوليانية منذ عصر J2000.0
    // J2000.0 هو الظهيرة في 1 يناير 2000 بالتوقيت العالمي
    double T = (jd - 2451545.0) / 36525.0;

    // حساب المتوسط الشاذ للشمس (Mean Anomaly)
    // هذه الزاوية تصف موقع الأرض في مدارها الإهليلجي
    double M = fixAngle(357.52911 + 35999.05029 * T - 0.0001537 * T * T);

    // حساب معادلة المركز (Equation of Center)
    // تصحيح يعوض عن كون مدار الأرض إهليلجياً وليس دائرياً
    double C = (1.9146 - 0.004817 * T - 0.000014 * T * T) * sinDeg(M)
             + (0.019993 - 0.000101 * T) * sinDeg(2.0 * M)
             + 0.00029 * sinDeg(3.0 * M);

    // حساب الطول الحقيقي للشمس (True Longitude)
    // المتوسط الهندسي للطول + معادلة المركز
    double L0 = fixAngle(280.46646 + 36000.76983 * T + 0.0003032 * T * T);
    double sunLong = fixAngle(L0 + C);

    // حساب ميل محور الأرض (Obliquity of the Ecliptic)
    // هذه الزاوية بين مستوى الكسوف ومستوى خط الاستواء السماوي
    double omega = 125.04 - 1934.136 * T;
    double epsilon = 23.439291 - 0.013004 * T
                   - 0.00000016 * T * T
                   + 0.00000050 * T * T * T;
    // تصحيح الترنح (Nutation correction)
    epsilon += 0.00256 * cosDeg(omega);

    // حساب الميل الشمسي النهائي باستخدام المعادلة الفلكية
    double declination = radiansToDegrees(
        std::asin(sinDeg(epsilon) * sinDeg(sunLong))
    );

    return declination;
}

/// حساب معادلة الوقت (Equation of Time)
/// معادلة الوقت هي الفرق بين الوقت الشمسي الحقيقي والوقت الشمسي المتوسط
/// هذا الفرق ينشأ بسبب:
///   1. ميل محور الأرض عن مستوى المدار
///   2. الشكل الإهليلجي لمدار الأرض
/// المدخلات: رقم اليوم اليولياني
/// المخرجات: الفرق بالدقائق
static double equationOfTime(double jd) {
    double T = (jd - 2451545.0) / 36525.0;

    // المتوسط الهندسي لطول الشمس
    double L0 = fixAngle(280.46646 + 36000.76983 * T + 0.0003032 * T * T);

    // المتوسط الشاذ للشمس
    double M = fixAngle(357.52911 + 35999.05029 * T - 0.0001537 * T * T);

    // انحراف مدار الأرض (Eccentricity)
    double e = 0.016708634 - 0.000042037 * T - 0.0000001267 * T * T;

    // ميل محور الأرض
    double epsilon = 23.439291 - 0.013004 * T;

    // حساب معادلة الوقت بطريقة تقريبية دقيقة
    double y = std::pow(tanDeg(epsilon / 2.0), 2.0);

    // المعادلة الرئيسية لحساب معادلة الوقت
    double eqTime = y * sinDeg(2.0 * L0)
                  - 2.0 * e * sinDeg(M)
                  + 4.0 * e * y * sinDeg(M) * cosDeg(2.0 * L0)
                  - 0.5 * y * y * sinDeg(4.0 * L0)
                  - 1.25 * e * e * sinDeg(2.0 * M);

    // التحويل من راديان إلى دقائق زمنية
    // كل راديان يساوي 229.18 دقيقة (360 درجة = 1440 دقيقة)
    return radiansToDegrees(eqTime) * 4.0;
}

// ============================================================================
// حساب وقت الظهيرة الشمسية (Solar Noon)
// ============================================================================

/// حساب وقت الظهيرة الشمسية — اللحظة التي تكون فيها الشمس في أعلى نقطة
/// هذا هو الأساس الذي تُحسب منه باقي مواقيت الصلاة
/// المدخلات: خط الطول، المنطقة الزمنية، معادلة الوقت بالدقائق
/// المخرجات: وقت الظهيرة بالساعات (نظام 24 ساعة)
static double computeNoon(double longitude, double timezone, double eqt) {
    // الظهيرة = 12 ساعة - معادلة الوقت - تصحيح خط الطول
    // كل درجة طول = 4 دقائق زمنية (360 درجة ÷ 24 ساعة × 60 دقيقة)
    return fixHour(12.0 - (eqt / 60.0) - (longitude / 15.0) + timezone);
}

// ============================================================================
// حساب زاوية الساعة (Hour Angle)
// ============================================================================

/// حساب زاوية الساعة لزاوية ارتفاع معينة
/// زاوية الساعة هي المسافة الزاوية التي تقطعها الشمس قبل أو بعد الظهيرة
/// المدخلات: الزاوية المطلوبة، خط العرض، الميل الشمسي
/// المخرجات: زاوية الساعة بالدرجات (أو NaN إذا كانت الشمس لا تصل لهذه الزاوية)
static double hourAngle(double angle, double latitude, double declination) {
    // المعادلة الفلكية لحساب زاوية الساعة
    // cos(HA) = [sin(a) - sin(lat) × sin(dec)] / [cos(lat) × cos(dec)]
    // حيث a = الزاوية المطلوبة، lat = خط العرض، dec = الميل الشمسي
    double cosHA = (sinDeg(angle) - sinDeg(latitude) * sinDeg(declination))
                 / (cosDeg(latitude) * cosDeg(declination));

    // التحقق من صحة القيمة — إذا كانت خارج النطاق [-1,1]
    // فهذا يعني أن الشمس لا تصل إلى هذه الزاوية في هذا الموقع
    if (cosHA > 1.0 || cosHA < -1.0) {
        return std::nan("");
    }

    return acosDeg(cosHA);
}

// ============================================================================
// الحصول على زوايا الفجر والعشاء حسب طريقة الحساب
// ============================================================================

/// إرجاع زاوية الفجر حسب طريقة الحساب المختارة
/// كل هيئة إسلامية تعتمد زاوية مختلفة بناءً على ملاحظاتها الفلكية
static double getFajrAngle(CalcMethod method) {
    switch (method) {
        case CalcMethod::UmmAlQura:
            // تقويم أم القرى — المعتمد في المملكة العربية السعودية
            return 18.5;
        case CalcMethod::MWL:
            // رابطة العالم الإسلامي
            return 18.0;
        case CalcMethod::ISNA:
            // الجمعية الإسلامية لأمريكا الشمالية
            return 15.0;
        case CalcMethod::Egypt:
            // الهيئة المصرية العامة للمساحة
            return 19.5;
        case CalcMethod::Karachi:
            // جامعة العلوم الإسلامية، كراتشي
            return 18.0;
        default:
            // القيمة الافتراضية — أم القرى
            return 18.5;
    }
}

/// إرجاع زاوية العشاء حسب طريقة الحساب المختارة
/// ملاحظة: تقويم أم القرى يستخدم 90 دقيقة بعد المغرب بدلاً من زاوية
/// لذلك نرجع 0 للإشارة إلى استخدام طريقة الفترة الزمنية
static double getIshaAngle(CalcMethod method) {
    switch (method) {
        case CalcMethod::UmmAlQura:
            // أم القرى: العشاء = 90 دقيقة بعد المغرب (لا تستخدم زاوية)
            return 0.0;
        case CalcMethod::MWL:
            return 17.0;
        case CalcMethod::ISNA:
            return 15.0;
        case CalcMethod::Egypt:
            return 17.5;
        case CalcMethod::Karachi:
            return 18.0;
        default:
            return 0.0;
    }
}

// ============================================================================
// الدالة الرئيسية لحساب مواقيت الصلاة
// ============================================================================

/// حساب مواقيت الصلاة الخمس بالإضافة إلى الشروق
/// هذه الدالة تجمع كل الحسابات الفلكية لتحديد المواقيت الدقيقة
///
/// المدخلات:
///   year, month, day — التاريخ الميلادي
///   latitude — خط العرض (موجب شمالاً، سالب جنوباً)
///   longitude — خط الطول (موجب شرقاً، سالب غرباً)
///   timezone — فرق التوقيت عن غرينتش بالساعات
///   method — طريقة الحساب (أم القرى، رابطة العالم الإسلامي، إلخ)
///   asrMethod — مذهب حساب العصر (شافعي أو حنفي)
///
/// المخرجات:
///   هيكل PrayerTimes يحتوي على الأوقات الستة بصيغة ساعات عشرية
PrayerTimes calculate(int year, int month, int day,
                      double latitude, double longitude, double timezone,
                      CalcMethod method, AsrMethod asrMethod) {

    PrayerTimes times;

    // ===== الخطوة 1: حساب رقم اليوم اليولياني =====
    double jd = julianDay(year, month, day);

    // ===== الخطوة 2: حساب الميل الشمسي ومعادلة الوقت =====
    // هذه القيم تتغير يومياً بشكل طفيف مع حركة الأرض حول الشمس
    double decl = sunDeclination(jd);
    double eqt  = equationOfTime(jd);

    // ===== الخطوة 3: حساب وقت الظهر (صلاة الظهر) =====
    // الظهر هو لحظة عبور الشمس لخط الزوال (أعلى نقطة في السماء)
    double noon = computeNoon(longitude, timezone, eqt);
    times.dhuhr = noon;

    // ===== الخطوة 4: حساب وقت الشروق =====
    // الشروق يحدث عندما يكون مركز الشمس على ارتفاع -0.8333°
    // القيمة -0.8333 تشمل:
    //   - نصف قطر قرص الشمس الظاهري (0.2667°)
    //   - الانكسار الجوي عند الأفق (0.5667°)
    double sunriseHA = hourAngle(-0.8333, latitude, decl);
    if (!std::isnan(sunriseHA)) {
        // الشروق = الظهر - زاوية الساعة (بالساعات)
        times.sunrise = noon - (sunriseHA / 15.0);
    } else {
        // في حالة الشمس القطبية (لا شروق)، نستخدم قيمة تقريبية
        times.sunrise = noon - 6.0;
    }

    // ===== الخطوة 5: حساب وقت المغرب (الغروب) =====
    // المغرب يحدث عندما يغرب قرص الشمس تحت الأفق
    // نفس زاوية الشروق ولكن بعد الظهيرة
    double sunsetHA = hourAngle(-0.8333, latitude, decl);
    if (!std::isnan(sunsetHA)) {
        // المغرب = الظهر + زاوية الساعة (بالساعات)
        times.maghrib = noon + (sunsetHA / 15.0);
    } else {
        times.maghrib = noon + 6.0;
    }

    // ===== الخطوة 6: حساب وقت الفجر =====
    // الفجر يبدأ عندما يظهر الضوء الأول في الأفق (الشفق الفلكي)
    // الزاوية تختلف حسب طريقة الحساب المعتمدة
    double fajrAngle = getFajrAngle(method);
    double fajrHA = hourAngle(-fajrAngle, latitude, decl);
    if (!std::isnan(fajrHA)) {
        times.fajr = noon - (fajrHA / 15.0);
    } else {
        // قيمة تقريبية: 90 دقيقة قبل الشروق
        times.fajr = times.sunrise - 1.5;
    }

    // ===== الخطوة 7: حساب وقت العصر =====
    // العصر يُحسب بناءً على طول ظل الجسم:
    //   - المذهب الشافعي: عندما يصبح ظل الجسم مساوياً لطوله + ظل الظهيرة
    //   - المذهب الحنفي: عندما يصبح ظل الجسم مساوياً لضعف طوله + ظل الظهيرة
    double shadowFactor = (asrMethod == AsrMethod::Hanafi) ? 2.0 : 1.0;

    // حساب الزاوية التي يكون عندها الظل بالطول المطلوب
    // المعادلة: زاوية العصر = arctan(1 / (معامل الظل + tan(|خط العرض - الميل|)))
    double asrAngle = radiansToDegrees(
        std::atan(1.0 / (shadowFactor + tanDeg(std::fabs(latitude - decl))))
    );

    double asrHA = hourAngle(asrAngle, latitude, decl);
    if (!std::isnan(asrHA)) {
        // العصر بعد الظهيرة
        times.asr = noon + (asrHA / 15.0);
    } else {
        // قيمة تقريبية: منتصف المسافة بين الظهر والمغرب
        times.asr = noon + (times.maghrib - noon) * 0.6667;
    }

    // ===== الخطوة 8: حساب وقت العشاء =====
    // العشاء يبدأ عند اختفاء الشفق الأحمر
    double ishaAngle = getIshaAngle(method);
    if (method == CalcMethod::UmmAlQura) {
        // طريقة أم القرى: العشاء = 90 دقيقة بعد المغرب
        // هذه الطريقة المعتمدة في المملكة العربية السعودية
        times.isha = times.maghrib + (90.0 / 60.0);
    } else {
        // باقي الطرق تستخدم زاوية محددة لحساب العشاء
        double ishaHA = hourAngle(-ishaAngle, latitude, decl);
        if (!std::isnan(ishaHA)) {
            times.isha = noon + (ishaHA / 15.0);
        } else {
            // قيمة تقريبية: 90 دقيقة بعد المغرب
            times.isha = times.maghrib + 1.5;
        }
    }

    // ===== الخطوة 9: تطبيع جميع الأوقات =====
    // نتأكد أن جميع الأوقات ضمن النطاق [0، 24) ساعة
    times.fajr    = fixHour(times.fajr);
    times.sunrise = fixHour(times.sunrise);
    times.dhuhr   = fixHour(times.dhuhr);
    times.asr     = fixHour(times.asr);
    times.maghrib = fixHour(times.maghrib);
    times.isha    = fixHour(times.isha);

    return times;
}

// ============================================================================
// دوال التنسيق — تحويل الأوقات الرقمية إلى نصوص مقروءة
// ============================================================================

/// تنسيق الوقت بنظام 24 ساعة (مثال: "05:23")
/// المدخلات: الوقت كعدد عشري (مثلاً 5.383 = الساعة 5:23)
/// المخرجات: نص بصيغة "سس:دد"
std::string formatTime(double time) {
    // تطبيع الوقت
    time = fixHour(time);

    // فصل الساعات والدقائق
    int hours = static_cast<int>(std::floor(time));
    int minutes = static_cast<int>(std::round((time - hours) * 60.0));

    // معالجة حالة تقريب الدقائق إلى 60
    if (minutes >= 60) {
        minutes = 0;
        hours = (hours + 1) % 24;
    }

    // بناء النص المنسق مع إضافة الأصفار البادئة
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours
        << ":" << std::setfill('0') << std::setw(2) << minutes;
    return oss.str();
}

/// تنسيق الوقت بنظام 12 ساعة مع علامة ص/م (مثال: "05:23 ص")
/// المدخلات: الوقت كعدد عشري
/// المخرجات: نص بصيغة "سس:دد ص" أو "سس:دد م"
std::string formatTime12(double time) {
    time = fixHour(time);

    int hours = static_cast<int>(std::floor(time));
    int minutes = static_cast<int>(std::round((time - hours) * 60.0));

    if (minutes >= 60) {
        minutes = 0;
        hours = (hours + 1) % 24;
    }

    // تحديد فترة اليوم: صباحاً (ص) أو مساءً (م)
    // ص = قبل الظهر، م = بعد الظهر
    std::string period = (hours < 12) ? "\xd8\xb5" : "\xd9\x85"; // ص أو م

    // تحويل إلى نظام 12 ساعة
    if (hours == 0) hours = 12;        // منتصف الليل = 12 ص
    else if (hours > 12) hours -= 12;  // بعد الظهر

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours
        << ":" << std::setfill('0') << std::setw(2) << minutes
        << " " << period;
    return oss.str();
}

// ============================================================================
// دوال الوصول — للحصول على أوقات وأسماء الصلوات
// ============================================================================

/// الحصول على وقت صلاة معينة بالفهرس
/// الفهرس: 0=الفجر، 1=الشروق، 2=الظهر، 3=العصر، 4=المغرب، 5=العشاء
/// المخرجات: الوقت منسقاً بنظام 24 ساعة
std::string getPrayerTime(const PrayerTimes& times, int index) {
    switch (index) {
        case 0: return formatTime(times.fajr);
        case 1: return formatTime(times.sunrise);
        case 2: return formatTime(times.dhuhr);
        case 3: return formatTime(times.asr);
        case 4: return formatTime(times.maghrib);
        case 5: return formatTime(times.isha);
        default:
            // فهرس غير صالح — إرجاع علامة خطأ
            return "--:--";
    }
}

/// الحصول على اسم الصلاة بالعربية حسب الفهرس
/// الأسماء مكتوبة بترميز UTF-8
std::string getPrayerName(int index) {
    switch (index) {
        case 0: return "\xd8\xa7\xd9\x84\xd9\x81\xd8\xac\xd8\xb1";       // الفجر
        case 1: return "\xd8\xa7\xd9\x84\xd8\xb4\xd8\xb1\xd9\x88\xd9\x82"; // الشروق
        case 2: return "\xd8\xa7\xd9\x84\xd8\xb8\xd9\x87\xd8\xb1";       // الظهر
        case 3: return "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xb5\xd8\xb1";       // العصر
        case 4: return "\xd8\xa7\xd9\x84\xd9\x85\xd8\xba\xd8\xb1\xd8\xa8"; // المغرب
        case 5: return "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xb4\xd8\xa7\xd8\xa1"; // العشاء
        default:
            return "\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb9\xd8\xb1\xd9\x88\xd9\x81"; // غير معروف
    }
}

/// الحصول على اسم طريقة الحساب بالعربية
std::string getMethodName(CalcMethod method) {
    switch (method) {
        case CalcMethod::UmmAlQura:
            // تقويم أم القرى — مكة المكرمة
            return "\xd8\xa3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x82\xd8\xb1\xd9\x89"; // أم القرى
        case CalcMethod::MWL:
            // رابطة العالم الإسلامي
            return "\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xb9\xd8\xa7\xd9\x84\xd9\x85 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xb3\xd9\x84\xd8\xa7\xd9\x85\xd9\x8a"; // رابطة العالم الإسلامي
        case CalcMethod::ISNA:
            // الجمعية الإسلامية لأمريكا الشمالية
            return "ISNA";
        case CalcMethod::Egypt:
            // الهيئة المصرية العامة للمساحة
            return "\xd8\xa7\xd9\x84\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb5\xd8\xb1\xd9\x8a\xd8\xa9"; // الهيئة المصرية
        case CalcMethod::Karachi:
            // جامعة العلوم الإسلامية — كراتشي
            return "\xd9\x83\xd8\xb1\xd8\xa7\xd8\xaa\xd8\xb4\xd9\x8a"; // كراتشي
        default:
            return "\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb9\xd8\xb1\xd9\x88\xd9\x81"; // غير معروف
    }
}

// ============================================================================
// دالة عرض جميع المواقيت — لتسهيل الطباعة والعرض
// ============================================================================

/// تنسيق جميع مواقيت الصلاة في نص واحد للعرض
/// المخرجات: نص متعدد الأسطر يحتوي على اسم كل صلاة ووقتها
/// الصيغة: "اسم_الصلاة: الوقت" لكل سطر
std::string getAllTimesFormatted(const PrayerTimes& times) {
    std::ostringstream oss;

    // عرض كل صلاة مع اسمها ووقتها
    // الترتيب: الفجر، الشروق، الظهر، العصر، المغرب، العشاء
    for (int i = 0; i < 6; ++i) {
        oss << getPrayerName(i) << ": " << getPrayerTime(times, i);
        // إضافة سطر جديد بعد كل صلاة ما عدا الأخيرة
        if (i < 5) {
            oss << "\n";
        }
    }

    return oss.str();
}

// ============================================================================
// نهاية ملف حاسبة مواقيت الصلاة
// بسم الله الرحمن الرحيم
// ============================================================================

} // نهاية فضاء الأسماء prayer
} // نهاية فضاء الأسماء sad
