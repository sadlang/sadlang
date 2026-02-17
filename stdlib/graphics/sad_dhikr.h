// ═══════════════════════════════════════════════════════════════════════════════
// نظام الأذكار والتسبيح — لغة ص
// Dhikr & Tasbeeh System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// تسبيح إلكتروني متعدد العدادات مع أذكار مدمجة
// لا يوجد نظير لهذا في Flutter أو أي إطار عمل آخر!
//
// الميزات:
//   📿 عداد تسبيح رقمي بفتحات متعددة
//   🤲 مجموعات أذكار مدمجة (الصباح، المساء، بعد الصلاة)
//   🔄 تتبع الجلسات والأهداف اليومية
//   📊 إحصائيات الأذكار
//   🎵 اهتزاز عند كل عدّة (إذا كان مدعوماً)
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace sad { namespace dhikr {

// ═══════════════════════════════════════════════════════════════════
// الهياكل — Structures
// ═══════════════════════════════════════════════════════════════════

// نوع مجموعة الأذكار
enum class DhikrCollection {
    Morning,        // أذكار الصباح
    Evening,        // أذكار المساء
    AfterPrayer,    // أذكار بعد الصلاة
    BeforeSleep,    // أذكار النوم
    Custom          // مخصص
};

// ذكر واحد
struct Dhikr {
    std::string text;       // نص الذكر
    std::string reference;  // المرجع (حديث/آية)
    int targetCount;        // العدد المطلوب
    int currentCount;       // العدد الحالي
    std::string reward;     // الفضل/الثواب
};

// جلسة أذكار
struct DhikrSession {
    int sessionId;
    DhikrCollection collection;
    int totalCount;         // إجمالي التسبيحات
    int completedDhikrs;    // عدد الأذكار المكتملة
    int totalDhikrs;        // إجمالي الأذكار في المجموعة
    bool isComplete;        // هل اكتملت الجلسة؟
};

// إحصائيات
struct DhikrStats {
    int totalToday;         // إجمالي اليوم
    int totalAllTime;       // الإجمالي الكلي
    int sessionsToday;      // جلسات اليوم
    int longestStreak;      // أطول سلسلة متتالية
    int currentStreak;      // السلسلة الحالية
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── العدادات ───

// إنشاء عداد تسبيح جديد
int createCounter(const std::string& name, int target = 33);

// زيادة العداد بواحد
int increment(int counterId);

// إعادة تعيين العداد
void resetCounter(int counterId);

// الحصول على القيمة الحالية
int getCount(int counterId);

// الحصول على الهدف
int getTarget(int counterId);

// هل وصل العداد للهدف؟
bool isTargetReached(int counterId);

// ─── مجموعات الأذكار ───

// تحميل مجموعة أذكار مدمجة
int loadCollection(DhikrCollection collection);

// الحصول على الذكر الحالي في المجموعة
Dhikr getCurrentDhikr(int sessionId);

// تسبيح (زيادة الذكر الحالي والانتقال للتالي عند الاكتمال)
bool count(int sessionId);

// الانتقال للذكر التالي يدوياً
bool nextDhikr(int sessionId);

// الانتقال للذكر السابق
bool prevDhikr(int sessionId);

// الحصول على حالة الجلسة
DhikrSession getSessionInfo(int sessionId);

// هل اكتملت المجموعة؟
bool isCollectionComplete(int sessionId);

// ─── مجموعة مخصصة ───

// إنشاء مجموعة مخصصة
int createCustomCollection(const std::string& name);

// إضافة ذكر للمجموعة المخصصة
void addDhikrToCollection(int collectionId, const std::string& text,
                           int targetCount, const std::string& reference = "",
                           const std::string& reward = "");

// ─── الإحصائيات ───

// الحصول على الإحصائيات
DhikrStats getStats();

// إعادة ضبط إحصائيات اليوم
void resetDailyStats();

}} // namespace sad::dhikr
