// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// sad_state.cpp — تنفيذ نظام إدارة الحالة التفاعلي
// ─────────────────────────────────────────────────────────────────────────────
// يُنفّذ مخزن حالة مركزي مع مراقبين وتاريخ ولقطات
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_state.h"
#include <algorithm>
#include <chrono>
#include <iostream>

namespace sad { namespace state {

// ───── المتغيرات العالمية ─────

// المخزن الرئيسي: مفتاح → قيمة
static std::unordered_map<std::string, StateValue> g_store;

// المراقبون: معرّف → {مفتاح, دالة_استدعاء}
struct WatcherEntry {
    StateObserverId id;
    std::string key;
    std::function<void(const std::string&, const StateValue&)> callback;
};
static std::vector<WatcherEntry> g_watchers;
static StateObserverId g_nextWatcherId = 1;

// تاريخ التغييرات
static bool g_historyEnabled = false;
static std::vector<StateChange> g_undoStack;
static std::vector<StateChange> g_redoStack;
static int g_maxHistory = 100;

// اللقطات
struct Snapshot {
    int id;
    std::string name;
    std::unordered_map<std::string, StateValue> data;
};
static std::vector<Snapshot> g_snapshots;
static int g_nextSnapshotId = 1;

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

// إخطار جميع المراقبين المسجّلين لمفتاح معيّن
static void notifyWatchers(const std::string& key, const StateValue& newVal) {
    // نسخة لتجنب مشاكل التعديل أثناء التكرار
    auto watchersCopy = g_watchers;
    for (const auto& w : watchersCopy) {
        if (w.key == key || w.key == "*") {
            try {
                w.callback(key, newVal);
            } catch (...) {
                // تجاهل أخطاء دوال الاستدعاء
            }
        }
    }
}

// تسجيل تغيير في سجل التراجع
static void recordChange(const std::string& key, const StateValue& oldVal, const StateValue& newVal) {
    if (!g_historyEnabled) return;
    
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
    
    StateChange change;
    change.key = key;
    change.oldValue = oldVal;
    change.newValue = newVal;
    change.timestamp = static_cast<double>(now);
    
    g_undoStack.push_back(change);
    
    // حدّ التاريخ
    if ((int)g_undoStack.size() > g_maxHistory) {
        g_undoStack.erase(g_undoStack.begin());
    }
    
    // أي تغيير جديد يُبطل سلسلة الإعادة
    g_redoStack.clear();
}

// قيمة فارغة افتراضية
static StateValue defaultValue() {
    return StateValue(0.0);
}

// ═══════════════════════════════════════════════════════════════════
// تعيين قيم الحالة
// ═══════════════════════════════════════════════════════════════════

void setState(const std::string& key, double value) {
    StateValue oldVal = g_store.count(key) ? g_store[key] : defaultValue();
    StateValue newVal(value);
    g_store[key] = newVal;
    recordChange(key, oldVal, newVal);
    notifyWatchers(key, newVal);
}

void setState(const std::string& key, const std::string& value) {
    StateValue oldVal = g_store.count(key) ? g_store[key] : defaultValue();
    StateValue newVal(value);
    g_store[key] = newVal;
    recordChange(key, oldVal, newVal);
    notifyWatchers(key, newVal);
}

void setState(const std::string& key, bool value) {
    StateValue oldVal = g_store.count(key) ? g_store[key] : defaultValue();
    StateValue newVal(value);
    g_store[key] = newVal;
    recordChange(key, oldVal, newVal);
    notifyWatchers(key, newVal);
}

void setStateNumberList(const std::string& key, const std::vector<double>& list) {
    StateValue oldVal = g_store.count(key) ? g_store[key] : defaultValue();
    StateValue newVal(list);
    g_store[key] = newVal;
    recordChange(key, oldVal, newVal);
    notifyWatchers(key, newVal);
}

void setStateTextList(const std::string& key, const std::vector<std::string>& list) {
    StateValue oldVal = g_store.count(key) ? g_store[key] : defaultValue();
    StateValue newVal(list);
    g_store[key] = newVal;
    recordChange(key, oldVal, newVal);
    notifyWatchers(key, newVal);
}

// ═══════════════════════════════════════════════════════════════════
// استرجاع قيم الحالة
// ═══════════════════════════════════════════════════════════════════

double getStateNumber(const std::string& key, double defaultVal) {
    auto it = g_store.find(key);
    if (it == g_store.end()) return defaultVal;
    if (auto* v = std::get_if<double>(&it->second)) return *v;
    if (auto* v = std::get_if<bool>(&it->second)) return *v ? 1.0 : 0.0;
    return defaultVal;
}

std::string getStateText(const std::string& key, const std::string& defaultVal) {
    auto it = g_store.find(key);
    if (it == g_store.end()) return defaultVal;
    if (auto* v = std::get_if<std::string>(&it->second)) return *v;
    if (auto* v = std::get_if<double>(&it->second)) return std::to_string(*v);
    if (auto* v = std::get_if<bool>(&it->second)) return *v ? "true" : "false";
    return defaultVal;
}

bool getStateBool(const std::string& key, bool defaultVal) {
    auto it = g_store.find(key);
    if (it == g_store.end()) return defaultVal;
    if (auto* v = std::get_if<bool>(&it->second)) return *v;
    if (auto* v = std::get_if<double>(&it->second)) return *v != 0.0;
    if (auto* v = std::get_if<std::string>(&it->second)) return !v->empty();
    return defaultVal;
}

std::vector<double> getStateNumberList(const std::string& key) {
    auto it = g_store.find(key);
    if (it == g_store.end()) return {};
    if (auto* v = std::get_if<std::vector<double>>(&it->second)) return *v;
    return {};
}

std::vector<std::string> getStateTextList(const std::string& key) {
    auto it = g_store.find(key);
    if (it == g_store.end()) return {};
    if (auto* v = std::get_if<std::vector<std::string>>(&it->second)) return *v;
    return {};
}

bool hasState(const std::string& key) {
    return g_store.count(key) > 0;
}

void removeState(const std::string& key) {
    g_store.erase(key);
}

void clearAllState() {
    g_store.clear();
    g_watchers.clear();
    g_undoStack.clear();
    g_redoStack.clear();
}

int getStateCount() {
    return static_cast<int>(g_store.size());
}

std::vector<std::string> getStateKeys() {
    std::vector<std::string> keys;
    keys.reserve(g_store.size());
    for (const auto& [k, v] : g_store) {
        keys.push_back(k);
    }
    return keys;
}

StateType getStateType(const std::string& key) {
    auto it = g_store.find(key);
    if (it == g_store.end()) return StateType::Number;
    
    if (std::holds_alternative<double>(it->second)) return StateType::Number;
    if (std::holds_alternative<std::string>(it->second)) return StateType::Text;
    if (std::holds_alternative<bool>(it->second)) return StateType::Boolean;
    if (std::holds_alternative<std::vector<double>>(it->second)) return StateType::NumberList;
    if (std::holds_alternative<std::vector<std::string>>(it->second)) return StateType::TextList;
    if (std::holds_alternative<std::unordered_map<std::string, double>>(it->second)) return StateType::NumberMap;
    if (std::holds_alternative<std::unordered_map<std::string, std::string>>(it->second)) return StateType::TextMap;
    return StateType::Number;
}

// ═══════════════════════════════════════════════════════════════════
// تعديل الحالة
// ═══════════════════════════════════════════════════════════════════

double incrementState(const std::string& key, double amount) {
    double current = getStateNumber(key, 0.0);
    double newVal = current + amount;
    setState(key, newVal);
    return newVal;
}

void appendStateText(const std::string& key, const std::string& text) {
    std::string current = getStateText(key, "");
    setState(key, current + text);
}

void pushStateNumber(const std::string& key, double value) {
    auto list = getStateNumberList(key);
    list.push_back(value);
    setStateNumberList(key, list);
}

void pushStateText(const std::string& key, const std::string& value) {
    auto list = getStateTextList(key);
    list.push_back(value);
    setStateTextList(key, list);
}

bool toggleState(const std::string& key) {
    bool current = getStateBool(key, false);
    bool newVal = !current;
    setState(key, newVal);
    return newVal;
}

// ═══════════════════════════════════════════════════════════════════
// نظام المراقبين
// ═══════════════════════════════════════════════════════════════════

StateObserverId watchState(const std::string& key,
                           std::function<void(const std::string&, const StateValue&)> callback) {
    WatcherEntry entry;
    entry.id = g_nextWatcherId++;
    entry.key = key;
    entry.callback = callback;
    g_watchers.push_back(entry);
    return entry.id;
}

void unwatchState(StateObserverId id) {
    g_watchers.erase(
        std::remove_if(g_watchers.begin(), g_watchers.end(),
            [id](const WatcherEntry& w) { return w.id == id; }),
        g_watchers.end()
    );
}

void unwatchAllForKey(const std::string& key) {
    g_watchers.erase(
        std::remove_if(g_watchers.begin(), g_watchers.end(),
            [&key](const WatcherEntry& w) { return w.key == key; }),
        g_watchers.end()
    );
}

void unwatchAll() {
    g_watchers.clear();
}

int getWatcherCount() {
    return static_cast<int>(g_watchers.size());
}

// ═══════════════════════════════════════════════════════════════════
// التراجع والإعادة
// ═══════════════════════════════════════════════════════════════════

void enableHistory(bool enable) {
    g_historyEnabled = enable;
    if (!enable) {
        g_undoStack.clear();
        g_redoStack.clear();
    }
}

bool canUndo() {
    return !g_undoStack.empty();
}

bool canRedo() {
    return !g_redoStack.empty();
}

bool undo() {
    if (g_undoStack.empty()) return false;
    
    StateChange change = g_undoStack.back();
    g_undoStack.pop_back();
    
    // استعادة القيمة السابقة بدون تسجيل (تعطيل مؤقت)
    bool wasEnabled = g_historyEnabled;
    g_historyEnabled = false;
    g_store[change.key] = change.oldValue;
    notifyWatchers(change.key, change.oldValue);
    g_historyEnabled = wasEnabled;
    
    g_redoStack.push_back(change);
    return true;
}

bool redo() {
    if (g_redoStack.empty()) return false;
    
    StateChange change = g_redoStack.back();
    g_redoStack.pop_back();
    
    bool wasEnabled = g_historyEnabled;
    g_historyEnabled = false;
    g_store[change.key] = change.newValue;
    notifyWatchers(change.key, change.newValue);
    g_historyEnabled = wasEnabled;
    
    g_undoStack.push_back(change);
    return true;
}

void clearHistory() {
    g_undoStack.clear();
    g_redoStack.clear();
}

int getHistorySize() {
    return static_cast<int>(g_undoStack.size());
}

void setMaxHistorySize(int maxSize) {
    g_maxHistory = maxSize;
    while ((int)g_undoStack.size() > g_maxHistory) {
        g_undoStack.erase(g_undoStack.begin());
    }
}

// ═══════════════════════════════════════════════════════════════════
// اللقطات (Snapshots)
// ═══════════════════════════════════════════════════════════════════

int saveSnapshot(const std::string& name) {
    Snapshot snap;
    snap.id = g_nextSnapshotId++;
    snap.name = name;
    snap.data = g_store; // نسخة كاملة
    g_snapshots.push_back(snap);
    return snap.id;
}

bool restoreSnapshot(int snapshotId) {
    for (const auto& snap : g_snapshots) {
        if (snap.id == snapshotId) {
            g_store = snap.data;
            return true;
        }
    }
    return false;
}

bool restoreSnapshot(const std::string& name) {
    for (const auto& snap : g_snapshots) {
        if (snap.name == name) {
            g_store = snap.data;
            return true;
        }
    }
    return false;
}

void deleteSnapshot(int snapshotId) {
    g_snapshots.erase(
        std::remove_if(g_snapshots.begin(), g_snapshots.end(),
            [snapshotId](const Snapshot& s) { return s.id == snapshotId; }),
        g_snapshots.end()
    );
}

int getSnapshotCount() {
    return static_cast<int>(g_snapshots.size());
}

}} // namespace sad::state
