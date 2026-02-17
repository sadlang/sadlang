// ============================================================================
// sad_qibla.cpp — حاسبة اتجاه القبلة للغة البرمجة ساد
// ============================================================================
// هذا الملف يحتوي على تنفيذ حاسبة اتجاه القبلة الشريفة
// يستخدم صيغة الدائرة العظمى لحساب الزاوية وصيغة هافرساين لحساب المسافة
// جميع الحسابات تعتمد على إحداثيات الكعبة المشرفة في مكة المكرمة
// ============================================================================

#include <cmath>
#include <string>
#include <sstream>
#include <SDL.h>
#include "graphics/sad_qibla.h"

// ============================================================================
// الثوابت الرياضية والجغرافية
// ============================================================================

namespace sad {
namespace qibla {

// إحداثيات الكعبة المشرفة في مكة المكرمة
// خط العرض: 21.4225 درجة شمالاً
// خط الطول: 39.8262 درجة شرقاً
static const double KAABA_LATITUDE  = 21.4225;
static const double KAABA_LONGITUDE = 39.8262;

// نصف قطر الأرض بالكيلومتر (القيمة المتوسطة)
static const double EARTH_RADIUS_KM = 6371.0;

// الثابت الرياضي باي (π)
static const double PI = 3.14159265358979323846;

// ============================================================================
// دوال مساعدة للتحويل بين الدرجات والراديان
// ============================================================================

/// تحويل من درجات إلى راديان
/// الصيغة: راديان = درجات × (π / 180)
static double toRadians(double degrees) {
    return degrees * (PI / 180.0);
}

/// تحويل من راديان إلى درجات
/// الصيغة: درجات = راديان × (180 / π)
static double toDegrees(double radians) {
    return radians * (180.0 / PI);
}

// ============================================================================
// حساب زاوية القبلة — صيغة الدائرة العظمى
// ============================================================================
// نستخدم صيغة الاتجاه (bearing) على الدائرة العظمى:
//   θ = atan2(sin(ΔL), cos(φ1)·sin(φ2) - sin(φ1)·cos(φ2)·cos(ΔL))
// حيث:
//   φ1 = خط عرض المستخدم (بالراديان)
//   φ2 = خط عرض الكعبة (بالراديان)
//   ΔL = خط طول الكعبة - خط طول المستخدم (بالراديان)
// النتيجة تكون بالدرجات من الشمال في اتجاه عقارب الساعة
// ============================================================================

double calculateAngle(double latitude, double longitude) {
    // تحويل الإحداثيات من درجات إلى راديان
    double phi1 = toRadians(latitude);          // خط عرض المستخدم
    double phi2 = toRadians(KAABA_LATITUDE);    // خط عرض الكعبة
    double deltaL = toRadians(KAABA_LONGITUDE - longitude); // فرق خط الطول

    // حساب مكونات صيغة الاتجاه
    // البسط: جيب فرق خط الطول
    double x = std::sin(deltaL);

    // المقام: جيب تمام عرض المستخدم × جيب عرض الكعبة
    //        ناقص جيب عرض المستخدم × جيب تمام عرض الكعبة × جيب تمام فرق الطول
    double y = std::cos(phi1) * std::sin(phi2)
             - std::sin(phi1) * std::cos(phi2) * std::cos(deltaL);

    // حساب الزاوية باستخدام دالة atan2
    // هذه الدالة تعطي الزاوية الصحيحة في جميع الأرباع
    double bearing = std::atan2(x, y);

    // تحويل من راديان إلى درجات
    double angle = toDegrees(bearing);

    // تطبيع الزاوية لتكون بين 0 و 360 درجة
    // الزاوية تُقاس من الشمال في اتجاه عقارب الساعة
    angle = std::fmod(angle + 360.0, 360.0);

    return angle;
}

// ============================================================================
// حساب المسافة إلى الكعبة — صيغة هافرساين
// ============================================================================
// صيغة هافرساين لحساب المسافة على سطح الكرة الأرضية:
//   a = sin²(Δφ/2) + cos(φ1) · cos(φ2) · sin²(Δλ/2)
//   c = 2 · atan2(√a, √(1-a))
//   d = R · c
// حيث:
//   R = نصف قطر الأرض (6371 كم)
//   Δφ = فرق خطوط العرض
//   Δλ = فرق خطوط الطول
// ============================================================================

double calculateDistance(double latitude, double longitude) {
    // تحويل الإحداثيات إلى راديان
    double phi1 = toRadians(latitude);
    double phi2 = toRadians(KAABA_LATITUDE);
    double deltaPhi = toRadians(KAABA_LATITUDE - latitude);     // فرق العرض
    double deltaLambda = toRadians(KAABA_LONGITUDE - longitude); // فرق الطول

    // حساب المعامل a في صيغة هافرساين
    // يتضمن مربع نصف جيب فرق العرض
    // مضافاً إليه حاصل ضرب أجيبة التمام في مربع نصف جيب فرق الطول
    double a = std::sin(deltaPhi / 2.0) * std::sin(deltaPhi / 2.0)
             + std::cos(phi1) * std::cos(phi2)
             * std::sin(deltaLambda / 2.0) * std::sin(deltaLambda / 2.0);

    // حساب المعامل c — الزاوية المركزية بين النقطتين
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

    // حساب المسافة النهائية بالكيلومتر
    // المسافة = نصف قطر الأرض × الزاوية المركزية
    double distance = EARTH_RADIUS_KM * c;

    return distance;
}

// ============================================================================
// تحديد الاتجاه بالنص العربي
// ============================================================================
// يتم تقسيم البوصلة إلى ثمانية اتجاهات رئيسية:
//   شمال (337.5° - 22.5°)
//   شمال شرق (22.5° - 67.5°)
//   شرق (67.5° - 112.5°)
//   جنوب شرق (112.5° - 157.5°)
//   جنوب (157.5° - 202.5°)
//   جنوب غرب (202.5° - 247.5°)
//   غرب (247.5° - 292.5°)
//   شمال غرب (292.5° - 337.5°)
// ============================================================================

std::string getDirectionText(double latitude, double longitude) {
    // حساب زاوية القبلة أولاً
    double angle = calculateAngle(latitude, longitude);

    // تحديد الاتجاه بناءً على الزاوية
    // نقسم الدائرة إلى 8 قطاعات، كل قطاع 45 درجة
    if (angle >= 337.5 || angle < 22.5) {
        return "\xd8\xb4\xd9\x85\xd8\xa7\xd9\x84";                          // شمال
    } else if (angle >= 22.5 && angle < 67.5) {
        return "\xd8\xb4\xd9\x85\xd8\xa7\xd9\x84 \xd8\xb4\xd8\xb1\xd9\x82"; // شمال شرق
    } else if (angle >= 67.5 && angle < 112.5) {
        return "\xd8\xb4\xd8\xb1\xd9\x82";                                  // شرق
    } else if (angle >= 112.5 && angle < 157.5) {
        return "\xd8\xac\xd9\x86\xd9\x88\xd8\xa8 \xd8\xb4\xd8\xb1\xd9\x82"; // جنوب شرق
    } else if (angle >= 157.5 && angle < 202.5) {
        return "\xd8\xac\xd9\x86\xd9\x88\xd8\xa8";                          // جنوب
    } else if (angle >= 202.5 && angle < 247.5) {
        return "\xd8\xac\xd9\x86\xd9\x88\xd8\xa8 \xd8\xba\xd8\xb1\xd8\xa8"; // جنوب غرب
    } else if (angle >= 247.5 && angle < 292.5) {
        return "\xd8\xba\xd8\xb1\xd8\xa8";                                  // غرب
    } else {
        return "\xd8\xb4\xd9\x85\xd8\xa7\xd9\x84 \xd8\xba\xd8\xb1\xd8\xa8"; // شمال غرب
    }
}

// ============================================================================
// عرض معلومات القبلة الشاملة
// ============================================================================
// يجمع جميع المعلومات: الزاوية، المسافة، والاتجاه في نص واحد
// ============================================================================

std::string getQiblaInfo(double latitude, double longitude) {
    // حساب جميع القيم المطلوبة
    double angle = calculateAngle(latitude, longitude);
    double distance = calculateDistance(latitude, longitude);
    std::string direction = getDirectionText(latitude, longitude);

    // بناء سلسلة المعلومات باستخدام مجرى النصوص
    std::ostringstream info;

    // عنوان المعلومات
    // "معلومات اتجاه القبلة"
    info << "\xe2\x95\x90\xe2\x95\x90\xe2\x95\x90 "
         << "\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa "
         << "\xd8\xa7\xd8\xaa\xd8\xac\xd8\xa7\xd9\x87 "
         << "\xd8\xa7\xd9\x84\xd9\x82\xd8\xa8\xd9\x84\xd8\xa9"
         << " \xe2\x95\x90\xe2\x95\x90\xe2\x95\x90\n";

    // "الموقع الحالي"
    info << "\xd8\xa7\xd9\x84\xd9\x85\xd9\x88\xd9\x82\xd8\xb9: "
         << latitude << "\xc2\xb0"
         << (latitude >= 0 ? "\xd8\xb4" : "\xd8\xac") // شمال أو جنوب
         << ", " << longitude << "\xc2\xb0"
         << (longitude >= 0 ? "\xd8\xb4\xd8\xb1\xd9\x82" : "\xd8\xba\xd8\xb1\xd8\xa8") // شرق أو غرب
         << "\n";

    // "زاوية القبلة"
    info << "\xd8\xb2\xd8\xa7\xd9\x88\xd9\x8a\xd8\xa9 "
         << "\xd8\xa7\xd9\x84\xd9\x82\xd8\xa8\xd9\x84\xd8\xa9: "
         << angle << "\xc2\xb0\n";

    // "الاتجاه"
    info << "\xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa\xd8\xac\xd8\xa7\xd9\x87: "
         << direction << "\n";

    // "المسافة إلى الكعبة"
    info << "\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xa7\xd9\x81\xd8\xa9 "
         << "\xd8\xa5\xd9\x84\xd9\x89 "
         << "\xd8\xa7\xd9\x84\xd9\x83\xd8\xb9\xd8\xa8\xd8\xa9: "
         << static_cast<int>(distance) << " \xd9\x83\xd9\x85\n";

    return info.str();
}

// ============================================================================
// دوال رسم مساعدة — رسم خط باستخدام خوارزمية بريسنهام
// ============================================================================
// لأن SDL2 لا يوفر دالة رسم خط مباشرة، نستخدم خوارزمية بريسنهام
// هذه الخوارزمية فعالة وتستخدم العمليات الصحيحة فقط
// ============================================================================

static void drawLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2) {
    // حساب الفروقات المطلقة بين النقطتين
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);

    // تحديد اتجاه الخطوة (إيجابي أو سلبي)
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    // متغير الخطأ لخوارزمية بريسنهام
    int err = dx - dy;

    // رسم النقاط على طول الخط
    while (true) {
        SDL_RenderDrawPoint(renderer, x1, y1);

        // التحقق من الوصول إلى النقطة النهائية
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;

        // تعديل الإحداثي الأفقي
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        // تعديل الإحداثي العمودي
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// ============================================================================
// رسم دائرة باستخدام خوارزمية نقطة المنتصف
// ============================================================================
// خوارزمية فعالة لرسم الدوائر باستخدام تناظر الثمان نقاط
// ============================================================================

static void drawCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x; // متغير القرار لتحديد النقطة التالية

    // رسم النقاط المتناظرة في جميع الأرباع الثمانية
    while (y <= x) {
        // رسم ثمان نقاط متناظرة حول المركز
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);  // الربع الأول
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);  // الربع الثاني
        SDL_RenderDrawPoint(renderer, cx - y, cy + x);  // الربع الثالث
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);  // الربع الرابع
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);  // الربع الخامس
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);  // الربع السادس
        SDL_RenderDrawPoint(renderer, cx + y, cy - x);  // الربع السابع
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);  // الربع الثامن

        y++;

        // تحديث متغير القرار
        if (decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;
        } else {
            x--;
            decisionOver2 += 2 * (y - x) + 1;
        }
    }
}

// ============================================================================
// رسم خط سميك — لرسم سهم القبلة بشكل واضح
// ============================================================================
// نرسم عدة خطوط متوازية لإنشاء خط سميك
// السُمك يتم عن طريق إزاحة الخط في الاتجاه العمودي
// ============================================================================

static void drawThickLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int thickness) {
    // حساب المتجه العمودي على الخط
    double dx = static_cast<double>(x2 - x1);
    double dy = static_cast<double>(y2 - y1);
    double length = std::sqrt(dx * dx + dy * dy);

    // تجنب القسمة على صفر
    if (length < 0.001) return;

    // المتجه العمودي الموحد
    double nx = -dy / length;
    double ny = dx / length;

    // رسم خطوط متوازية لتشكيل السمك المطلوب
    int half = thickness / 2;
    for (int i = -half; i <= half; i++) {
        int offsetX = static_cast<int>(nx * i);
        int offsetY = static_cast<int>(ny * i);
        drawLine(renderer, x1 + offsetX, y1 + offsetY, x2 + offsetX, y2 + offsetY);
    }
}

// ============================================================================
// رسم دائرة مملوءة — لرسم نقطة الكعبة ونقطة المركز
// ============================================================================

static void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius) {
    // رسم خطوط أفقية لملء الدائرة
    for (int y = -radius; y <= radius; y++) {
        // حساب عرض الخط الأفقي عند هذا الارتفاع
        int halfWidth = static_cast<int>(std::sqrt(
            static_cast<double>(radius * radius - y * y)));
        // رسم خط أفقي من اليسار إلى اليمين
        SDL_RenderDrawLine(renderer, cx - halfWidth, cy + y, cx + halfWidth, cy + y);
    }
}

// ============================================================================
// رسم رأس السهم — مثلث عند نهاية سهم القبلة
// ============================================================================

static void drawArrowHead(SDL_Renderer* renderer, int tipX, int tipY,
                          double angleRad, int size) {
    // حساب النقطتين الخلفيتين لرأس السهم
    // زاوية الانفتاح: 25 درجة من كل جانب
    double spreadAngle = toRadians(25.0);

    // النقطة اليسرى لرأس السهم
    int leftX = tipX - static_cast<int>(size * std::sin(angleRad - spreadAngle));
    int leftY = tipY + static_cast<int>(size * std::cos(angleRad - spreadAngle));

    // النقطة اليمنى لرأس السهم
    int rightX = tipX - static_cast<int>(size * std::sin(angleRad + spreadAngle));
    int rightY = tipY + static_cast<int>(size * std::cos(angleRad + spreadAngle));

    // رسم خطوط رأس السهم بسمك مناسب
    drawThickLine(renderer, tipX, tipY, leftX, leftY, 3);
    drawThickLine(renderer, tipX, tipY, rightX, rightY, 3);
    drawThickLine(renderer, leftX, leftY, rightX, rightY, 2);
}

// ============================================================================
// رسم بوصلة القبلة الرئيسية
// ============================================================================
// هذه الدالة ترسم بوصلة كاملة تتضمن:
// 1. الدائرة الخارجية للبوصلة
// 2. دائرة داخلية للزخرفة
// 3. علامات الاتجاهات الأصلية الأربعة (شمال، جنوب، شرق، غرب)
// 4. علامات الدرجات كل 30 درجة
// 5. سهم القبلة العريض باللون المميز
// 6. رمز الكعبة المشرفة عند طرف السهم
// 7. نقطة المركز
//
// المعاملات:
//   renderer — مؤشر SDL_Renderer للرسم
//   cx, cy — إحداثيات مركز البوصلة
//   radius — نصف قطر البوصلة بالبكسل
//   qiblaAngle — زاوية القبلة بالدرجات من الشمال
//   deviceHeading — اتجاه الجهاز الحالي (للدوران) بالدرجات
// ============================================================================

void drawCompass(void* rendererPtr, int cx, int cy, int radius,
                double qiblaAngle, double deviceHeading) {
    // تحويل المؤشر العام إلى مؤشر SDL_Renderer
    SDL_Renderer* renderer = static_cast<SDL_Renderer*>(rendererPtr);
    if (!renderer) return; // التحقق من صلاحية المؤشر

    // ================================================================
    // الخطوة الأولى: رسم الدائرة الخارجية للبوصلة
    // ================================================================
    // اللون: أبيض فاتح للإطار الخارجي
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    drawCircle(renderer, cx, cy, radius);
    drawCircle(renderer, cx, cy, radius - 1); // دائرة مزدوجة للسمك

    // دائرة داخلية زخرفية
    SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
    drawCircle(renderer, cx, cy, radius - 10);

    // ================================================================
    // الخطوة الثانية: رسم علامات الدرجات على محيط البوصلة
    // ================================================================
    // حساب فرق الزاوية بسبب اتجاه الجهاز
    double headingRad = toRadians(deviceHeading);

    // رسم علامة كل 10 درجات
    for (int deg = 0; deg < 360; deg += 10) {
        double angleRad = toRadians(static_cast<double>(deg)) - headingRad;

        // تحديد طول العلامة حسب الدرجة
        int tickInner;
        if (deg % 90 == 0) {
            tickInner = radius - 25; // علامات الاتجاهات الرئيسية أطول
        } else if (deg % 30 == 0) {
            tickInner = radius - 18; // علامات كل 30 درجة متوسطة
        } else {
            tickInner = radius - 12; // علامات صغيرة كل 10 درجات
        }

        int tickOuter = radius - 5;

        // حساب نقطتي بداية ونهاية العلامة
        int outerX = cx + static_cast<int>(tickOuter * std::sin(angleRad));
        int outerY = cy - static_cast<int>(tickOuter * std::cos(angleRad));
        int innerX = cx + static_cast<int>(tickInner * std::sin(angleRad));
        int innerY = cy - static_cast<int>(tickInner * std::cos(angleRad));

        // اللون: رمادي للعلامات العادية، أبيض للرئيسية
        if (deg % 90 == 0) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        }

        drawLine(renderer, innerX, innerY, outerX, outerY);
    }

    // ================================================================
    // الخطوة الثالثة: رسم خطوط الاتجاهات الأربعة الرئيسية
    // ================================================================
    // خطوط رفيعة من المركز إلى الحافة للاتجاهات الأصلية

    int crossLen = radius - 30; // طول خطوط الاتجاهات

    // الشمال — خط أحمر خفيف (لتمييز الشمال)
    {
        double northRad = -headingRad; // الشمال عند 0 درجة
        int nx = cx + static_cast<int>(crossLen * std::sin(northRad));
        int ny = cy - static_cast<int>(crossLen * std::cos(northRad));
        SDL_SetRenderDrawColor(renderer, 200, 80, 80, 255);
        drawThickLine(renderer, cx, cy, nx, ny, 2);

        // رسم مثلث صغير عند طرف الشمال
        int tipLen = radius - 15;
        int tipX = cx + static_cast<int>(tipLen * std::sin(northRad));
        int tipY = cy - static_cast<int>(tipLen * std::cos(northRad));
        drawFilledCircle(renderer, tipX, tipY, 4);
    }

    // الجنوب — خط رمادي
    {
        double southRad = toRadians(180.0) - headingRad;
        int sx = cx + static_cast<int>(crossLen * std::sin(southRad));
        int sy = cy - static_cast<int>(crossLen * std::cos(southRad));
        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        drawLine(renderer, cx, cy, sx, sy);
    }

    // الشرق — خط رمادي
    {
        double eastRad = toRadians(90.0) - headingRad;
        int ex = cx + static_cast<int>(crossLen * std::sin(eastRad));
        int ey = cy - static_cast<int>(crossLen * std::cos(eastRad));
        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        drawLine(renderer, cx, cy, ex, ey);
    }

    // الغرب — خط رمادي
    {
        double westRad = toRadians(270.0) - headingRad;
        int wx = cx + static_cast<int>(crossLen * std::sin(westRad));
        int wy = cy - static_cast<int>(crossLen * std::cos(westRad));
        SDL_SetRenderDrawColor(renderer, 160, 160, 160, 255);
        drawLine(renderer, cx, cy, wx, wy);
    }

    // ================================================================
    // الخطوة الرابعة: رسم سهم القبلة — العنصر الرئيسي
    // ================================================================
    // سهم عريض باللون الأخضر يشير إلى اتجاه القبلة
    // يأخذ في الاعتبار اتجاه الجهاز الحالي

    double qiblaRad = toRadians(qiblaAngle) - headingRad;

    // حساب نقطة طرف سهم القبلة
    int arrowLen = radius - 20;
    int arrowTipX = cx + static_cast<int>(arrowLen * std::sin(qiblaRad));
    int arrowTipY = cy - static_cast<int>(arrowLen * std::cos(qiblaRad));

    // رسم جسم السهم — خط سميك باللون الأخضر الإسلامي
    // اللون الأخضر: يرمز إلى الإسلام والسكينة
    SDL_SetRenderDrawColor(renderer, 0, 180, 80, 255);
    drawThickLine(renderer, cx, cy, arrowTipX, arrowTipY, 4);

    // رسم رأس السهم عند الطرف
    SDL_SetRenderDrawColor(renderer, 0, 200, 90, 255);
    drawArrowHead(renderer, arrowTipX, arrowTipY, qiblaRad, 18);

    // ================================================================
    // الخطوة الخامسة: رسم رمز الكعبة المشرفة عند طرف السهم
    // ================================================================
    // مربع صغير يمثل الكعبة المشرفة باللون الذهبي

    // حساب موقع رمز الكعبة — أبعد قليلاً من طرف السهم
    int kaabaLen = radius - 8;
    int kaabaX = cx + static_cast<int>(kaabaLen * std::sin(qiblaRad));
    int kaabaY = cy - static_cast<int>(kaabaLen * std::cos(qiblaRad));

    // رسم مربع الكعبة — مربع ذهبي صغير
    int kaabaSize = 5;
    SDL_SetRenderDrawColor(renderer, 218, 165, 32, 255); // ذهبي
    SDL_Rect kaabaRect = {
        kaabaX - kaabaSize, kaabaY - kaabaSize,
        kaabaSize * 2, kaabaSize * 2
    };
    SDL_RenderFillRect(renderer, &kaabaRect);

    // إطار أسود حول رمز الكعبة
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderDrawRect(renderer, &kaabaRect);

    // ================================================================
    // الخطوة السادسة: رسم نقطة المركز
    // ================================================================
    // نقطة صغيرة في مركز البوصلة تمثل موقع المستخدم
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawFilledCircle(renderer, cx, cy, 4);

    // حلقة حول نقطة المركز
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    drawCircle(renderer, cx, cy, 6);

    // ================================================================
    // الخطوة السابعة: رسم حرف الشمال "ش" بنقاط
    // ================================================================
    // نرسم نقطة حمراء عند موقع الشمال على الدائرة الخارجية
    {
        double northRad = -headingRad;
        int markDist = radius + 8;
        int northMarkX = cx + static_cast<int>(markDist * std::sin(northRad));
        int northMarkY = cy - static_cast<int>(markDist * std::cos(northRad));

        // نقطة حمراء كبيرة تشير إلى الشمال
        SDL_SetRenderDrawColor(renderer, 220, 50, 50, 255);
        drawFilledCircle(renderer, northMarkX, northMarkY, 5);
    }
}

// ============================================================================
// رسم بوصلة القبلة الكاملة — الدالة المُيسَّرة
// ============================================================================
// هذه الدالة تجمع بين حساب الزاوية ورسم البوصلة
// تأخذ إحداثيات المستخدم وتحسب الاتجاه تلقائياً ثم ترسم البوصلة
//
// المعاملات:
//   renderer — مؤشر SDL_Renderer للرسم
//   cx, cy — إحداثيات مركز البوصلة على الشاشة
//   radius — نصف قطر البوصلة بالبكسل
//   latitude — خط عرض المستخدم بالدرجات
//   longitude — خط طول المستخدم بالدرجات
//   deviceHeading — اتجاه الجهاز الحالي بالدرجات (اختياري، الافتراضي 0)
// ============================================================================

void drawQiblaCompass(void* renderer, int cx, int cy, int radius,
                     double latitude, double longitude, double deviceHeading) {
    // حساب زاوية القبلة من موقع المستخدم
    double qiblaAngle = calculateAngle(latitude, longitude);

    // استدعاء دالة رسم البوصلة مع الزاوية المحسوبة
    drawCompass(renderer, cx, cy, radius, qiblaAngle, deviceHeading);
}

} // نهاية فضاء الأسماء qibla
} // نهاية فضاء الأسماء sad

// ============================================================================
// نهاية الملف — حاسبة اتجاه القبلة
// الحمد لله رب العالمين
// ============================================================================
