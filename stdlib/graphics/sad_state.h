// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_state.h — نظام إدارة الحالة التفاعلي
// ─────────────────────────────────────────────────────────────────────────────
// يوفر نظام إدارة حالة متقدم مستوحى من Flutter/Redux:
//   - حالة عالمية (Global State) مع مفاتيح عربية
//   - حالة مكوّن (Component State) مرتبطة بودجت
//   - مراقبون (Observers) يُنبَّهون عند تغيّر الحالة
//   - حالة محسوبة (Computed State) تعتمد على حالات أخرى
//   - تاريخ الحالة (State History) للتراجع/الإعادة
//   - نقاط التفتيش (Snapshots) لحفظ واستعادة الحالة
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <any>
#include <variant>

namespace sad { namespace state {

// ═══════════════════════════════════════════════════════════════════
// أنواع قيم الحالة — يدعم: أرقام، نصوص، منطقية، قوائم، خرائط
// ═══════════════════════════════════════════════════════════════════
using StateValue = std::variant<
    double,                                          // رقم
    std::string,                                     // نص
    bool,                                            // منطقي
    std::vector<double>,                             // قائمة أرقام
    std::vector<std::string>,                        // قائمة نصوص
    std::unordered_map<std::string, double>,          // خريطة أرقام
    std::unordered_map<std::string, std::string>      // خريطة نصوص
>;

// أنواع قيم الحالة (للاستعلام)
enum class StateType {
    Number,         // رقم
    Text,           // نص
    Boolean,        // منطقي
    NumberList,     // قائمة_أرقام
    TextList,       // قائمة_نصوص
    NumberMap,      // خريطة_أرقام
    TextMap         // خريطة_نصوص
};

// ═══════════════════════════════════════════════════════════════════
// مراقب الحالة — يُستدعى عند تغيّر قيمة حالة معيّنة
// ═══════════════════════════════════════════════════════════════════
using StateObserverId = int;

// ═══════════════════════════════════════════════════════════════════
// سجل تغيير — لدعم التراجع (Undo) والإعادة (Redo)
// ═══════════════════════════════════════════════════════════════════
struct StateChange {
    std::string key;            // مفتاح الحالة
    StateValue oldValue;        // القيمة السابقة
    StateValue newValue;        // القيمة الجديدة
    double timestamp;           // وقت التغيير (بالمللي ثانية)
};

// ═══════════════════════════════════════════════════════════════════
// الحالة العالمية — مخزن مركزي لجميع بيانات التطبيق
// ═══════════════════════════════════════════════════════════════════

// ───── تعيين واسترجاع قيم الحالة ─────

// تعيين قيمة رقمية
void setState(const std::string& key, double value);

// تعيين قيمة نصية
void setState(const std::string& key, const std::string& value);

// تعيين قيمة منطقية
void setState(const std::string& key, bool value);

// تعيين قائمة أرقام
void setStateNumberList(const std::string& key, const std::vector<double>& list);

// تعيين قائمة نصوص
void setStateTextList(const std::string& key, const std::vector<std::string>& list);

// استرجاع قيمة رقمية (الافتراضي: 0)
double getStateNumber(const std::string& key, double defaultVal = 0.0);

// استرجاع قيمة نصية (الافتراضي: "")
std::string getStateText(const std::string& key, const std::string& defaultVal = "");

// استرجاع قيمة منطقية (الافتراضي: false)
bool getStateBool(const std::string& key, bool defaultVal = false);

// استرجاع قائمة أرقام
std::vector<double> getStateNumberList(const std::string& key);

// استرجاع قائمة نصوص
std::vector<std::string> getStateTextList(const std::string& key);

// هل المفتاح موجود في الحالة؟
bool hasState(const std::string& key);

// حذف مفتاح من الحالة
void removeState(const std::string& key);

// مسح جميع الحالات
void clearAllState();

// عدد الحالات المخزّنة
int getStateCount();

// سرد جميع مفاتيح الحالة
std::vector<std::string> getStateKeys();

// نوع قيمة مفتاح معيّن
StateType getStateType(const std::string& key);

// ───── تعديل قيم الحالة ─────

// زيادة قيمة رقمية (مفيد للعدادات)
double incrementState(const std::string& key, double amount = 1.0);

// إلحاق نص بنص موجود
void appendStateText(const std::string& key, const std::string& text);

// إضافة قيمة إلى قائمة أرقام
void pushStateNumber(const std::string& key, double value);

// إضافة نص إلى قائمة نصوص
void pushStateText(const std::string& key, const std::string& value);

// تبديل قيمة منطقية
bool toggleState(const std::string& key);

// ═══════════════════════════════════════════════════════════════════
// نظام المراقبين — إشعارات تفاعلية عند تغيّر الحالة
// ═══════════════════════════════════════════════════════════════════

// تسجيل مراقب يُستدعى عند تغيّر مفتاح معيّن
// يُرجع معرّف المراقب لإلغائه لاحقاً
StateObserverId watchState(const std::string& key,
                           std::function<void(const std::string& key, const StateValue& newVal)> callback);

// إلغاء مراقب بمعرّفه
void unwatchState(StateObserverId id);

// إلغاء جميع المراقبين لمفتاح معيّن
void unwatchAllForKey(const std::string& key);

// إلغاء جميع المراقبين
void unwatchAll();

// عدد المراقبين النشطين
int getWatcherCount();

// ═══════════════════════════════════════════════════════════════════
// التراجع والإعادة — تاريخ تغييرات الحالة (Undo/Redo)
// ═══════════════════════════════════════════════════════════════════

// تفعيل/تعطيل تتبع التاريخ (معطّل افتراضياً لتوفير الذاكرة)
void enableHistory(bool enable = true);

// هل يمكن التراجع؟
bool canUndo();

// هل يمكن الإعادة؟
bool canRedo();

// تراجع عن آخر تغيير
bool undo();

// إعادة آخر تغيير ملغي
bool redo();

// مسح سجل التاريخ
void clearHistory();

// عدد العمليات القابلة للتراجع
int getHistorySize();

// الحد الأقصى لسجل التاريخ
void setMaxHistorySize(int maxSize);

// ═══════════════════════════════════════════════════════════════════
// اللقطات (Snapshots) — حفظ واستعادة حالة كاملة
// ═══════════════════════════════════════════════════════════════════

// حفظ لقطة من الحالة الحالية وإرجاع معرّفها
int saveSnapshot(const std::string& name = "");

// استعادة حالة من لقطة
bool restoreSnapshot(int snapshotId);

// استعادة بالاسم
bool restoreSnapshot(const std::string& name);

// حذف لقطة
void deleteSnapshot(int snapshotId);

// عدد اللقطات المحفوظة
int getSnapshotCount();

}} // namespace sad::state
