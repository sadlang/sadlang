// ═══════════════════════════════════════════════════════════════════════════════
// نظام التقويم الهجري — لغة ص
// Hijri Calendar System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// تحويل التواريخ بين التقويم الميلادي والهجري — مع دعم كامل للعربية
// لا يوجد نظير لهذا في Flutter أو أي إطار عمل آخر!
//
// يستخدم خوارزمية Kuwaiti لتحويل التواريخ
// مع جداول مدمجة للأشهر الهجرية والمناسبات الإسلامية
//
// الميزات:
//   📅 تحويل من ميلادي إلى هجري والعكس
//   🌙 أسماء الأشهر الهجرية بالعربية
//   📖 المناسبات والأعياد الإسلامية
//   🗓️ معرفة يوم الأسبوع بالعربية
//   🌑 يوم الشهر الهجري
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace sad { namespace hijri {

// ═══════════════════════════════════════════════════════════════════
// الهياكل — Structures
// ═══════════════════════════════════════════════════════════════════

// تاريخ هجري
struct HijriDate {
    int year;    // السنة الهجرية
    int month;   // الشهر (1-12)
    int day;     // اليوم
};

// تاريخ ميلادي
struct GregorianDate {
    int year;
    int month;
    int day;
};

// مناسبة إسلامية
struct IslamicEvent {
    int month;              // الشهر الهجري
    int day;                // اليوم
    std::string nameAr;     // الاسم بالعربية
    std::string nameEn;     // الاسم بالإنجليزية
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── التحويل ───

// تحويل من ميلادي إلى هجري
HijriDate gregorianToHijri(int gYear, int gMonth, int gDay);

// تحويل من هجري إلى ميلادي
GregorianDate hijriToGregorian(int hYear, int hMonth, int hDay);

// ─── أسماء الأشهر والأيام ───

// اسم الشهر الهجري بالعربية (1 = محرم، 12 = ذو الحجة)
std::string getMonthNameAr(int month);

// اسم الشهر الهجري بالإنجليزية
std::string getMonthNameEn(int month);

// اسم يوم الأسبوع بالعربية (0 = الأحد)
std::string getDayNameAr(int dayOfWeek);

// عدد أيام الشهر الهجري (29 أو 30)
int getDaysInMonth(int hYear, int hMonth);

// هل السنة الهجرية كبيسة؟
bool isLeapYear(int hYear);

// ─── التنسيق ───

// تاريخ هجري كنص عربي منسق
// مثال: "15 رمضان 1445"
std::string formatHijriAr(int hYear, int hMonth, int hDay);

// تاريخ هجري كنص إنجليزي منسق
std::string formatHijriEn(int hYear, int hMonth, int hDay);

// التاريخ الهجري اليوم
HijriDate getToday();

// تنسيق التاريخ الهجري اليوم
std::string getTodayFormatted();

// ─── المناسبات ───

// الحصول على المناسبات الإسلامية في شهر معين
std::vector<IslamicEvent> getEventsInMonth(int hMonth);

// هل اليوم فيه مناسبة؟
bool hasEvent(int hMonth, int hDay);

// اسم المناسبة (إن وجدت)
std::string getEventName(int hMonth, int hDay);

}} // namespace sad::hijri
