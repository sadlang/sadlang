// ═══════════════════════════════════════════════════════════════════════════════
// نظام دعم أندرويد — لغة ص
// Android Support System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام شامل لدعم منصة أندرويد يوفر:
//   📱 إدارة دورة حياة التطبيق (onCreate, onPause, onResume, onDestroy)
//   🔔 نظام الإشعارات (محلية + قنوات)
//   📍 خدمات الموقع الجغرافي (GPS)
//   📷 الوصول للكاميرا والمعرض
//   🔋 معلومات البطارية والشبكة
//   📳 الاهتزاز والمستشعرات
//   💾 التخزين المحلي (SharedPreferences + ملفات)
//   🔗 الربط العميق (Deep Links)
//   🎨 شريط الحالة وشريط التنقل
//   📲 مشاركة المحتوى والصور
//
// هذا الملف يعمل كطبقة تجريد (Abstraction Layer) — يستدعى من المفسر
// ويتواصل مع كود Java/Kotlin عبر JNI عند التشغيل على أندرويد
// أو يعمل كمحاكاة (Stub) عند التشغيل على سطح المكتب
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <map>

namespace sad { namespace android {

// ═══════════════════════════════════════════════════════════════════
// حالات دورة حياة التطبيق
// ═══════════════════════════════════════════════════════════════════
enum class LifecycleState {
    Created,    // تم الإنشاء
    Started,    // بدأ
    Resumed,    // استؤنف (في المقدمة)
    Paused,     // متوقف مؤقتاً (في الخلفية)
    Stopped,    // متوقف
    Destroyed   // تم التدمير
};

// ═══════════════════════════════════════════════════════════════════
// أهمية الإشعار
// ═══════════════════════════════════════════════════════════════════
enum class NotificationPriority {
    Low,        // منخفضة — لا يصدر صوت
    Default,    // عادية
    High,       // عالية — تظهر فوق الشاشة
    Urgent      // عاجلة — تصدر صوت واهتزاز
};

// ═══════════════════════════════════════════════════════════════════
// اتجاه الشاشة
// ═══════════════════════════════════════════════════════════════════
enum class Orientation {
    Portrait,       // عمودي
    Landscape,      // أفقي
    Auto,           // تلقائي (حسب المستشعر)
    PortraitLocked, // عمودي مقفل
    LandscapeLocked // أفقي مقفل
};

// ═══════════════════════════════════════════════════════════════════
// نوع الشبكة
// ═══════════════════════════════════════════════════════════════════
enum class NetworkType {
    None,       // لا اتصال
    WiFi,       // واي فاي
    Mobile,     // بيانات الهاتف
    Ethernet,   // إيثرنت
    Unknown     // غير معروف
};

// ═══════════════════════════════════════════════════════════════════
// معلومات الجهاز
// ═══════════════════════════════════════════════════════════════════
struct DeviceInfo {
    std::string model;          // طراز الجهاز (مثل: Samsung Galaxy S24)
    std::string manufacturer;   // الشركة المصنعة
    std::string osVersion;      // إصدار نظام التشغيل
    int sdkVersion;             // مستوى SDK
    int screenWidth;            // عرض الشاشة بالبكسل
    int screenHeight;           // ارتفاع الشاشة بالبكسل
    float screenDensity;        // كثافة الشاشة (DPI)
    std::string language;       // لغة الجهاز
    std::string country;        // بلد الجهاز
};

// ═══════════════════════════════════════════════════════════════════
// معلومات البطارية
// ═══════════════════════════════════════════════════════════════════
struct BatteryInfo {
    int level;              // مستوى البطارية (0-100)
    bool isCharging;        // هل يشحن؟
    float temperature;      // درجة الحرارة
};

// ═══════════════════════════════════════════════════════════════════
// معلومات الموقع الجغرافي
// ═══════════════════════════════════════════════════════════════════
struct LocationInfo {
    double latitude;        // خط العرض
    double longitude;       // خط الطول
    double altitude;        // الارتفاع بالأمتار
    float accuracy;         // الدقة بالأمتار
    float speed;            // السرعة (م/ث)
    float bearing;          // الاتجاه (درجات)
    bool available;         // هل المعلومات متاحة؟
};

// ═══════════════════════════════════════════════════════════════════
// إعدادات الإشعار
// ═══════════════════════════════════════════════════════════════════
struct NotificationConfig {
    std::string title;          // عنوان الإشعار
    std::string body;           // نص الإشعار
    std::string channelId;      // معرف القناة
    NotificationPriority priority;  // الأهمية
    bool vibrate;               // اهتزاز؟
    bool sound;                 // صوت؟
    bool autoCancel;            // يختفي عند الضغط؟
    int iconColor;              // لون الأيقونة (RGB مدمج)
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── دورة الحياة ───
void init();                                    // تهيئة النظام
void shutdown();                                // إيقاف النظام
LifecycleState getLifecycleState();             // الحصول على حالة التطبيق
void setOnPause(std::function<void()> callback);    // عند التوقف المؤقت
void setOnResume(std::function<void()> callback);   // عند الاستئناف

// ─── معلومات الجهاز ───
DeviceInfo getDeviceInfo();                     // معلومات شاملة عن الجهاز
std::string getDeviceModel();                   // طراز الجهاز
std::string getOsVersion();                     // إصدار النظام
int getSdkLevel();                              // مستوى SDK
int getScreenWidth();                           // عرض الشاشة
int getScreenHeight();                          // ارتفاع الشاشة
float getScreenDensity();                       // كثافة الشاشة
std::string getDeviceLanguage();                // لغة الجهاز

// ─── البطارية ───
BatteryInfo getBatteryInfo();                   // معلومات البطارية
int getBatteryLevel();                          // مستوى البطارية
bool isBatteryCharging();                       // هل يشحن؟

// ─── الشبكة ───
NetworkType getNetworkType();                   // نوع الشبكة
bool isNetworkAvailable();                      // هل الشبكة متوفرة؟
std::string getNetworkTypeName();               // اسم نوع الشبكة بالعربي
std::string getIpAddress();                     // عنوان IP

// ─── الموقع الجغرافي ───
LocationInfo getLocation();                     // الموقع الحالي
double getLatitude();                           // خط العرض
double getLongitude();                          // خط الطول
bool isLocationAvailable();                     // هل الموقع متاح؟

// ─── الإشعارات ───
void createNotificationChannel(const std::string& id, const std::string& name,
                                const std::string& description, NotificationPriority priority);
int showNotification(const NotificationConfig& config);     // عرض إشعار — يرجع المعرف
void showSimpleNotification(const std::string& title, const std::string& body);
void cancelNotification(int notificationId);                // إلغاء إشعار
void cancelAllNotifications();                              // إلغاء كل الإشعارات

// ─── الاهتزاز ───
void vibrate(int milliseconds = 200);           // اهتزاز بمدة محددة
void vibratePattern(const std::vector<int>& pattern);  // اهتزاز بنمط (تشغيل/إيقاف)

// ─── التخزين المحلي (SharedPreferences) ───
void saveString(const std::string& key, const std::string& value);
std::string loadString(const std::string& key, const std::string& defaultValue = "");
void saveInt(const std::string& key, int value);
int loadInt(const std::string& key, int defaultValue = 0);
void saveDouble(const std::string& key, double value);
double loadDouble(const std::string& key, double defaultValue = 0.0);
void saveBool(const std::string& key, bool value);
bool loadBool(const std::string& key, bool defaultValue = false);
void removeKey(const std::string& key);
void clearStorage();

// ─── الشاشة ───
void setOrientation(Orientation orientation);   // تعيين اتجاه الشاشة
void setFullScreen(bool fullscreen);            // ملء الشاشة
void setKeepScreenOn(bool keepOn);              // منع إطفاء الشاشة
void setStatusBarColor(uint8_t r, uint8_t g, uint8_t b);  // لون شريط الحالة
void setNavigationBarColor(uint8_t r, uint8_t g, uint8_t b);  // لون شريط التنقل
void hideStatusBar();                           // إخفاء شريط الحالة
void showStatusBar();                           // إظهار شريط الحالة
float getStatusBarHeight();                     // ارتفاع شريط الحالة

// ─── المشاركة ───
void shareText(const std::string& text, const std::string& title = "");  // مشاركة نص
void shareUrl(const std::string& url, const std::string& title = "");    // مشاركة رابط
void openUrl(const std::string& url);           // فتح رابط في المتصفح
void openAppSettings();                         // فتح إعدادات التطبيق

// ─── الحافظة ───
void copyToClipboard(const std::string& text);  // نسخ للحافظة
std::string getClipboardText();                 // لصق من الحافظة

// ─── الرسائل القصيرة (Toast) ───
void showToast(const std::string& message, bool longDuration = false);

// ─── هل نحن على أندرويد فعلاً؟ ───
bool isAndroid();                               // هل التطبيق يعمل على أندرويد؟
bool isEmulator();                              // هل على محاكي؟
std::string getPlatformName();                  // "Android" أو "Desktop"

}} // namespace sad::android
