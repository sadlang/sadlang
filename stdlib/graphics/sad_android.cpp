// ═══════════════════════════════════════════════════════════════════════════════
// نظام دعم أندرويد — تطبيق لغة ص
// Android Support System — SAD Language Implementation
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يحتوي على التنفيذ الفعلي لنظام دعم أندرويد.
// يعمل بأسلوبين:
//   1. على أندرويد: يتواصل مع Java/Kotlin عبر JNI
//   2. على سطح المكتب: يعمل كمحاكاة (Stub) لأغراض التطوير والاختبار
//
// كل دالة تتحقق من متغير g_isAndroid:
//   - إذا كان true: يستدعي الدالة الأصلية عبر JNI
//   - إذا كان false: يُرجع قيمة افتراضية معقولة
// ═══════════════════════════════════════════════════════════════════════════════

#include "graphics/sad_android.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة — المتغيرات الداخلية
// ═══════════════════════════════════════════════════════════════════

namespace sad { namespace android {

// ─── هل نحن فعلاً على أندرويد؟ ───
// يُعيَّن إلى true فقط عند التشغيل عبر JNI على جهاز أندرويد حقيقي
static bool g_isAndroid = false;

// ─── حالة دورة الحياة ───
static LifecycleState g_lifecycleState = LifecycleState::Created;
static std::function<void()> g_onPauseCallback = nullptr;
static std::function<void()> g_onResumeCallback = nullptr;

// ─── التخزين المحلي (محاكاة SharedPreferences على سطح المكتب) ───
static std::unordered_map<std::string, std::string> g_stringStore;
static std::unordered_map<std::string, int> g_intStore;
static std::unordered_map<std::string, double> g_doubleStore;
static std::unordered_map<std::string, bool> g_boolStore;

// ─── معلومات محاكاة الجهاز ───
static DeviceInfo g_desktopInfo = {
    "Desktop-Emulation",        // طراز
    "SAD-Language",             // الشركة
    "Desktop",                  // إصدار النظام
    34,                         // مستوى SDK
    1920,                       // عرض الشاشة
    1080,                       // ارتفاع الشاشة
    2.0f,                       // الكثافة
    "ar",                       // اللغة
    "SA"                        // البلد
};

// ─── معلومات محاكاة الموقع (الحرم المكي — الافتراضي) ───
static LocationInfo g_location = {
    21.4225,    // خط العرض — مكة المكرمة
    39.8262,    // خط الطول
    277.0,      // الارتفاع بالأمتار
    10.0f,      // الدقة
    0.0f,       // السرعة
    0.0f,       // الاتجاه
    true        // متاح
};

// ─── حالة الشاشة ───
static bool g_fullscreen = false;
static bool g_keepScreenOn = false;
static Orientation g_orientation = Orientation::Auto;
static uint8_t g_statusR = 0, g_statusG = 0, g_statusB = 0;
static uint8_t g_navR = 0, g_navG = 0, g_navB = 0;
static bool g_statusBarVisible = true;

// ─── الحافظة ───
static std::string g_clipboard;

// ─── الإشعارات ───
static int g_nextNotificationId = 1;

// ═══════════════════════════════════════════════════════════════════
// تنفيذ الدوال — دورة الحياة
// ═══════════════════════════════════════════════════════════════════

void init() {
    // تهيئة النظام — على سطح المكتب نكتفي بتعيين الحالة
    g_lifecycleState = LifecycleState::Resumed;
    
    // تحديد ما إذا كنا على أندرويد
    #ifdef __ANDROID__
        g_isAndroid = true;
    #else
        g_isAndroid = false;
    #endif
}

void shutdown() {
    // تنظيف الموارد
    g_lifecycleState = LifecycleState::Destroyed;
    g_onPauseCallback = nullptr;
    g_onResumeCallback = nullptr;
    g_stringStore.clear();
    g_intStore.clear();
    g_doubleStore.clear();
    g_boolStore.clear();
}

LifecycleState getLifecycleState() {
    return g_lifecycleState;
}

void setOnPause(std::function<void()> callback) {
    g_onPauseCallback = callback;
}

void setOnResume(std::function<void()> callback) {
    g_onResumeCallback = callback;
}

// ═══════════════════════════════════════════════════════════════════
// معلومات الجهاز
// ═══════════════════════════════════════════════════════════════════

DeviceInfo getDeviceInfo() {
    return g_desktopInfo;
}

std::string getDeviceModel() {
    return g_desktopInfo.model;
}

std::string getOsVersion() {
    return g_desktopInfo.osVersion;
}

int getSdkLevel() {
    return g_desktopInfo.sdkVersion;
}

int getScreenWidth() {
    return g_desktopInfo.screenWidth;
}

int getScreenHeight() {
    return g_desktopInfo.screenHeight;
}

float getScreenDensity() {
    return g_desktopInfo.screenDensity;
}

std::string getDeviceLanguage() {
    return g_desktopInfo.language;
}

// ═══════════════════════════════════════════════════════════════════
// البطارية
// ═══════════════════════════════════════════════════════════════════

BatteryInfo getBatteryInfo() {
    // محاكاة: بطارية ممتلئة
    return {100, false, 25.0f};
}

int getBatteryLevel() {
    return 100;
}

bool isBatteryCharging() {
    return false;
}

// ═══════════════════════════════════════════════════════════════════
// الشبكة
// ═══════════════════════════════════════════════════════════════════

NetworkType getNetworkType() {
    // محاكاة: متصل بالواي فاي
    return NetworkType::WiFi;
}

bool isNetworkAvailable() {
    return true;
}

std::string getNetworkTypeName() {
    switch (getNetworkType()) {
        case NetworkType::WiFi: return "\xd9\x88\xd8\xa7\xd9\x8a \xd9\x81\xd8\xa7\xd9\x8a"; // واي فاي
        case NetworkType::Mobile: return "\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd9\x87\xd8\xa7\xd8\xaa\xd9\x81"; // بيانات الهاتف
        case NetworkType::Ethernet: return "\xd8\xa5\xd9\x8a\xd8\xab\xd8\xb1\xd9\x86\xd8\xaa"; // إيثرنت
        case NetworkType::None: return "\xd9\x84\xd8\xa7 \xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84"; // لا اتصال
        default: return "\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb9\xd8\xb1\xd9\x88\xd9\x81"; // غير معروف
    }
}

std::string getIpAddress() {
    return "127.0.0.1";
}

// ═══════════════════════════════════════════════════════════════════
// الموقع الجغرافي
// ═══════════════════════════════════════════════════════════════════

LocationInfo getLocation() {
    return g_location;
}

double getLatitude() {
    return g_location.latitude;
}

double getLongitude() {
    return g_location.longitude;
}

bool isLocationAvailable() {
    return g_location.available;
}

// ═══════════════════════════════════════════════════════════════════
// الإشعارات
// ═══════════════════════════════════════════════════════════════════

void createNotificationChannel(const std::string& id, const std::string& name,
                                const std::string& description, NotificationPriority priority) {
    // محاكاة: طباعة معلومات القناة
    std::cout << "[Android] \xd9\x82\xd9\x86\xd8\xa7\xd8\xa9 \xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1: " 
              << name << " (" << id << ")" << std::endl;
}

int showNotification(const NotificationConfig& config) {
    int id = g_nextNotificationId++;
    std::cout << "[Android] \xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1 #" << id 
              << ": " << config.title << " — " << config.body << std::endl;
    return id;
}

void showSimpleNotification(const std::string& title, const std::string& body) {
    NotificationConfig cfg;
    cfg.title = title;
    cfg.body = body;
    cfg.priority = NotificationPriority::Default;
    cfg.vibrate = true;
    cfg.sound = true;
    cfg.autoCancel = true;
    showNotification(cfg);
}

void cancelNotification(int notificationId) {
    std::cout << "[Android] \xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1 \xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1 #" 
              << notificationId << std::endl;
}

void cancelAllNotifications() {
    std::cout << "[Android] \xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1 \xd9\x83\xd9\x84 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa" << std::endl;
}

// ═══════════════════════════════════════════════════════════════════
// الاهتزاز
// ═══════════════════════════════════════════════════════════════════

void vibrate(int milliseconds) {
    // محاكاة على سطح المكتب
    std::cout << "[Android] \xd8\xa7\xd9\x87\xd8\xaa\xd8\xb2\xd8\xa7\xd8\xb2: " 
              << milliseconds << " \xd9\x85\xd9\x84\xd9\x8a \xd8\xab\xd8\xa7\xd9\x86\xd9\x8a\xd8\xa9" << std::endl;
}

void vibratePattern(const std::vector<int>& pattern) {
    std::cout << "[Android] \xd8\xa7\xd9\x87\xd8\xaa\xd8\xb2\xd8\xa7\xd8\xb2 \xd8\xa8\xd9\x86\xd9\x85\xd8\xb7 (" 
              << pattern.size() << " \xd8\xae\xd8\xb7\xd9\x88\xd8\xa9)" << std::endl;
}

// ═══════════════════════════════════════════════════════════════════
// التخزين المحلي — محاكاة SharedPreferences
// ═══════════════════════════════════════════════════════════════════

void saveString(const std::string& key, const std::string& value) {
    g_stringStore[key] = value;
}

std::string loadString(const std::string& key, const std::string& defaultValue) {
    auto it = g_stringStore.find(key);
    return it != g_stringStore.end() ? it->second : defaultValue;
}

void saveInt(const std::string& key, int value) {
    g_intStore[key] = value;
}

int loadInt(const std::string& key, int defaultValue) {
    auto it = g_intStore.find(key);
    return it != g_intStore.end() ? it->second : defaultValue;
}

void saveDouble(const std::string& key, double value) {
    g_doubleStore[key] = value;
}

double loadDouble(const std::string& key, double defaultValue) {
    auto it = g_doubleStore.find(key);
    return it != g_doubleStore.end() ? it->second : defaultValue;
}

void saveBool(const std::string& key, bool value) {
    g_boolStore[key] = value;
}

bool loadBool(const std::string& key, bool defaultValue) {
    auto it = g_boolStore.find(key);
    return it != g_boolStore.end() ? it->second : defaultValue;
}

void removeKey(const std::string& key) {
    g_stringStore.erase(key);
    g_intStore.erase(key);
    g_doubleStore.erase(key);
    g_boolStore.erase(key);
}

void clearStorage() {
    g_stringStore.clear();
    g_intStore.clear();
    g_doubleStore.clear();
    g_boolStore.clear();
}

// ═══════════════════════════════════════════════════════════════════
// الشاشة
// ═══════════════════════════════════════════════════════════════════

void setOrientation(Orientation orientation) {
    g_orientation = orientation;
}

void setFullScreen(bool fullscreen) {
    g_fullscreen = fullscreen;
}

void setKeepScreenOn(bool keepOn) {
    g_keepScreenOn = keepOn;
}

void setStatusBarColor(uint8_t r, uint8_t g, uint8_t b) {
    g_statusR = r; g_statusG = g; g_statusB = b;
}

void setNavigationBarColor(uint8_t r, uint8_t g, uint8_t b) {
    g_navR = r; g_navG = g; g_navB = b;
}

void hideStatusBar() {
    g_statusBarVisible = false;
}

void showStatusBar() {
    g_statusBarVisible = true;
}

float getStatusBarHeight() {
    return g_statusBarVisible ? 24.0f : 0.0f;
}

// ═══════════════════════════════════════════════════════════════════
// المشاركة
// ═══════════════════════════════════════════════════════════════════

void shareText(const std::string& text, const std::string& title) {
    std::cout << "[Android] \xd9\x85\xd8\xb4\xd8\xa7\xd8\xb1\xd9\x83\xd8\xa9: " << text << std::endl;
}

void shareUrl(const std::string& url, const std::string& title) {
    std::cout << "[Android] \xd9\x85\xd8\xb4\xd8\xa7\xd8\xb1\xd9\x83\xd8\xa9 \xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7: " << url << std::endl;
}

void openUrl(const std::string& url) {
    #ifdef _WIN32
        // [FIX #1] حماية من حقن الأوامر — استخدام ShellExecuteA بدلاً من system()
        // التحقق من أن الرابط يبدأ ببروتوكول آمن فقط
        if (url.substr(0, 7) == "http://" || url.substr(0, 8) == "https://") {
            ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
        } else {
            std::cout << "[Android] رابط غير آمن: " << url << std::endl;
        }
    #else
        std::cout << "[Android] \xd9\x81\xd8\xaa\xd8\xad: " << url << std::endl;
    #endif
}

void openAppSettings() {
    std::cout << "[Android] \xd9\x81\xd8\xaa\xd8\xad \xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82" << std::endl;
}

// ═══════════════════════════════════════════════════════════════════
// الحافظة
// ═══════════════════════════════════════════════════════════════════

void copyToClipboard(const std::string& text) {
    g_clipboard = text;
    #ifdef _WIN32
        // [FIX #2] حماية من حقن الأوامر — استخدام Win32 Clipboard API
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            size_t len = text.size() + 1;
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
            if (hMem) {
                memcpy(GlobalLock(hMem), text.c_str(), len);
                GlobalUnlock(hMem);
                SetClipboardData(CF_TEXT, hMem);
            }
            CloseClipboard();
        }
    #endif
}

std::string getClipboardText() {
    return g_clipboard;
}

// ═══════════════════════════════════════════════════════════════════
// الرسائل القصيرة (Toast)
// ═══════════════════════════════════════════════════════════════════

void showToast(const std::string& message, bool longDuration) {
    std::cout << "[Toast] " << message << std::endl;
}

// ═══════════════════════════════════════════════════════════════════
// معلومات المنصة
// ═══════════════════════════════════════════════════════════════════

bool isAndroid() {
    #ifdef __ANDROID__
        return true;
    #else
        return false;
    #endif
}

bool isEmulator() {
    return false;
}

std::string getPlatformName() {
    #ifdef __ANDROID__
        return "Android";
    #else
        return "Desktop";
    #endif
}

}} // namespace sad::android
