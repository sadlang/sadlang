// ═══════════════════════════════════════════════════════════════════════════
// sad_android_apis.h - واجهات برمجة أندرويد الأصلية
// ═══════════════════════════════════════════════════════════════════════════
//
// يوفر هذا الملف ربط APIs أندرويد الأصلية لاستخدامها من كود ص
// يشمل: الكاميرا، الموقع، المستشعرات، التخزين، الشبكة، إلخ
//
// ═══════════════════════════════════════════════════════════════════════════

#ifndef SAD_ANDROID_APIS_H
#define SAD_ANDROID_APIS_H

#ifdef SAD_ANDROID

#include <jni.h>
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace sad {
namespace android {
namespace apis {

// ═══════════════════════════════════════════════════════════════════════════
// معلومات الجهاز
// ═══════════════════════════════════════════════════════════════════════════

struct DeviceInfo {
    std::string manufacturer;    // الشركة المصنعة
    std::string model;           // الموديل
    std::string androidVersion;  // إصدار أندرويد
    int sdkVersion;              // مستوى SDK
    std::string language;        // اللغة
    std::string country;         // البلد
    int screenWidth;             // عرض الشاشة
    int screenHeight;            // ارتفاع الشاشة
    float screenDensity;         // كثافة الشاشة
};

class DeviceAPI {
public:
    static DeviceInfo getDeviceInfo(JNIEnv* env, jobject context);
    static int getBatteryLevel(JNIEnv* env, jobject context);
    static bool isCharging(JNIEnv* env, jobject context);
    static bool isConnectedToNetwork(JNIEnv* env, jobject context);
    static std::string getNetworkType(JNIEnv* env, jobject context);
};

// ═══════════════════════════════════════════════════════════════════════════
// نظام الملفات
// ═══════════════════════════════════════════════════════════════════════════

class FileSystemAPI {
public:
    // المسارات
    static std::string getInternalDir(JNIEnv* env, jobject context);
    static std::string getExternalDir(JNIEnv* env, jobject context);
    static std::string getCacheDir(JNIEnv* env, jobject context);
    
    // عمليات الملفات
    static bool fileExists(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool deleteFile(const std::string& path);
    static bool copyFile(const std::string& src, const std::string& dst);
    
    static std::string readTextFile(const std::string& path);
    static bool writeTextFile(const std::string& path, const std::string& content);
    static std::vector<uint8_t> readBinaryFile(const std::string& path);
    static bool writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data);
    
    static std::vector<std::string> listDirectory(const std::string& path);
    static long long getFileSize(const std::string& path);
};

// ═══════════════════════════════════════════════════════════════════════════
// التخزين المحلي (SharedPreferences)
// ═══════════════════════════════════════════════════════════════════════════

class StorageAPI {
public:
    static void saveString(JNIEnv* env, jobject context, 
                          const std::string& key, const std::string& value);
    static std::string getString(JNIEnv* env, jobject context, 
                                const std::string& key, const std::string& defaultValue = "");
    
    static void saveInt(JNIEnv* env, jobject context, const std::string& key, int value);
    static int getInt(JNIEnv* env, jobject context, const std::string& key, int defaultValue = 0);
    
    static void saveFloat(JNIEnv* env, jobject context, const std::string& key, float value);
    static float getFloat(JNIEnv* env, jobject context, const std::string& key, float defaultValue = 0.0f);
    
    static void saveBool(JNIEnv* env, jobject context, const std::string& key, bool value);
    static bool getBool(JNIEnv* env, jobject context, const std::string& key, bool defaultValue = false);
    
    static bool contains(JNIEnv* env, jobject context, const std::string& key);
    static void remove(JNIEnv* env, jobject context, const std::string& key);
    static void clear(JNIEnv* env, jobject context);
};

// ═══════════════════════════════════════════════════════════════════════════
// الموقع الجغرافي
// ═══════════════════════════════════════════════════════════════════════════

struct Location {
    double latitude;      // خط العرض
    double longitude;     // خط الطول
    double altitude;      // الارتفاع
    float accuracy;       // الدقة (متر)
    float speed;          // السرعة (م/ث)
    float bearing;        // الاتجاه
    long long timestamp;  // الوقت
};

using LocationCallback = std::function<void(const Location&)>;
using LocationErrorCallback = std::function<void(const std::string&)>;

class LocationAPI {
public:
    static bool hasPermission(JNIEnv* env, jobject context);
    static void requestPermission(JNIEnv* env, jobject activity);
    
    static void getCurrentLocation(JNIEnv* env, jobject context,
                                   LocationCallback onSuccess,
                                   LocationErrorCallback onError);
    
    static void startLocationUpdates(JNIEnv* env, jobject context,
                                     int intervalMs,
                                     LocationCallback onUpdate);
    static void stopLocationUpdates(JNIEnv* env, jobject context);
    
    static double distanceBetween(double lat1, double lon1, double lat2, double lon2);
};

// ═══════════════════════════════════════════════════════════════════════════
// الكاميرا
// ═══════════════════════════════════════════════════════════════════════════

struct Photo {
    std::string path;
    int width;
    int height;
    std::vector<uint8_t> data;
};

using PhotoCallback = std::function<void(const Photo&)>;
using PhotoErrorCallback = std::function<void(const std::string&)>;

class CameraAPI {
public:
    static bool hasPermission(JNIEnv* env, jobject context);
    static void requestPermission(JNIEnv* env, jobject activity);
    
    static bool hasFrontCamera(JNIEnv* env, jobject context);
    static bool hasBackCamera(JNIEnv* env, jobject context);
    
    static void takePhoto(JNIEnv* env, jobject activity,
                         bool useFrontCamera,
                         PhotoCallback onSuccess,
                         PhotoErrorCallback onError);
    
    static void pickFromGallery(JNIEnv* env, jobject activity,
                               PhotoCallback onSuccess,
                               PhotoErrorCallback onError);
};

// ═══════════════════════════════════════════════════════════════════════════
// المستشعرات
// ═══════════════════════════════════════════════════════════════════════════

struct SensorData {
    float x, y, z;
    long long timestamp;
};

using SensorCallback = std::function<void(const SensorData&)>;

class SensorAPI {
public:
    // التسارع
    static bool hasAccelerometer(JNIEnv* env, jobject context);
    static void startAccelerometer(JNIEnv* env, jobject context, SensorCallback callback);
    static void stopAccelerometer(JNIEnv* env, jobject context);
    
    // الجيروسكوب
    static bool hasGyroscope(JNIEnv* env, jobject context);
    static void startGyroscope(JNIEnv* env, jobject context, SensorCallback callback);
    static void stopGyroscope(JNIEnv* env, jobject context);
    
    // البوصلة
    static bool hasCompass(JNIEnv* env, jobject context);
    static float getCompassHeading(JNIEnv* env, jobject context);
    
    // مستشعر الضوء
    static bool hasLightSensor(JNIEnv* env, jobject context);
    static float getLightLevel(JNIEnv* env, jobject context);
    
    // مستشعر القرب
    static bool hasProximitySensor(JNIEnv* env, jobject context);
    static float getProximity(JNIEnv* env, jobject context);
};

// ═══════════════════════════════════════════════════════════════════════════
// الإشعارات
// ═══════════════════════════════════════════════════════════════════════════

struct NotificationConfig {
    std::string title;
    std::string body;
    std::string icon;      // اسم الأيقونة
    std::string channelId;
    int priority = 0;      // -2 إلى 2
    bool autoCancel = true;
    std::map<std::string, std::string> data;
};

class NotificationAPI {
public:
    static bool hasPermission(JNIEnv* env, jobject context);
    static void requestPermission(JNIEnv* env, jobject activity);
    
    static void createChannel(JNIEnv* env, jobject context,
                             const std::string& channelId,
                             const std::string& channelName,
                             const std::string& description);
    
    static void show(JNIEnv* env, jobject context,
                    int id, const NotificationConfig& config);
    
    static void cancel(JNIEnv* env, jobject context, int id);
    static void cancelAll(JNIEnv* env, jobject context);
};

// ═══════════════════════════════════════════════════════════════════════════
// المشاركة
// ═══════════════════════════════════════════════════════════════════════════

class ShareAPI {
public:
    static void shareText(JNIEnv* env, jobject context,
                         const std::string& text,
                         const std::string& title = "");
    
    static void shareImage(JNIEnv* env, jobject context,
                          const std::string& imagePath,
                          const std::string& text = "");
    
    static void shareFile(JNIEnv* env, jobject context,
                         const std::string& filePath,
                         const std::string& mimeType);
};

// ═══════════════════════════════════════════════════════════════════════════
// الحافظة
// ═══════════════════════════════════════════════════════════════════════════

class ClipboardAPI {
public:
    static void copyText(JNIEnv* env, jobject context, const std::string& text);
    static std::string pasteText(JNIEnv* env, jobject context);
    static bool hasText(JNIEnv* env, jobject context);
};

// ═══════════════════════════════════════════════════════════════════════════
// الاهتزاز
// ═══════════════════════════════════════════════════════════════════════════

class VibrationAPI {
public:
    static bool hasVibrator(JNIEnv* env, jobject context);
    static void vibrate(JNIEnv* env, jobject context, int milliseconds);
    static void vibratePattern(JNIEnv* env, jobject context,
                              const std::vector<long>& pattern, int repeat = -1);
    static void cancel(JNIEnv* env, jobject context);
};

// ═══════════════════════════════════════════════════════════════════════════
// الصوت
// ═══════════════════════════════════════════════════════════════════════════

class AudioAPI {
public:
    // أصوات النظام
    static void playSystemSound(JNIEnv* env, jobject context, int soundType);
    
    // تشغيل ملفات الصوت
    static int playSound(JNIEnv* env, jobject context, const std::string& path, bool loop = false);
    static void stopSound(JNIEnv* env, jobject context, int playerId);
    static void pauseSound(JNIEnv* env, jobject context, int playerId);
    static void resumeSound(JNIEnv* env, jobject context, int playerId);
    static void setVolume(JNIEnv* env, jobject context, int playerId, float volume);
    
    // التحكم بمستوى الصوت
    static void setSystemVolume(JNIEnv* env, jobject context, int streamType, float volume);
    static float getSystemVolume(JNIEnv* env, jobject context, int streamType);
};

// ═══════════════════════════════════════════════════════════════════════════
// HTTP Client
// ═══════════════════════════════════════════════════════════════════════════

struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
};

using HttpCallback = std::function<void(const HttpResponse&)>;
using HttpErrorCallback = std::function<void(const std::string&)>;

class HttpAPI {
public:
    static void get(JNIEnv* env, jobject context,
                   const std::string& url,
                   const std::map<std::string, std::string>& headers,
                   HttpCallback onSuccess,
                   HttpErrorCallback onError);
    
    static void post(JNIEnv* env, jobject context,
                    const std::string& url,
                    const std::string& body,
                    const std::map<std::string, std::string>& headers,
                    HttpCallback onSuccess,
                    HttpErrorCallback onError);
    
    static void put(JNIEnv* env, jobject context,
                   const std::string& url,
                   const std::string& body,
                   const std::map<std::string, std::string>& headers,
                   HttpCallback onSuccess,
                   HttpErrorCallback onError);
    
    static void delete_(JNIEnv* env, jobject context,
                       const std::string& url,
                       const std::map<std::string, std::string>& headers,
                       HttpCallback onSuccess,
                       HttpErrorCallback onError);
};

// ═══════════════════════════════════════════════════════════════════════════
// Intent & Navigation
// ═══════════════════════════════════════════════════════════════════════════

class IntentAPI {
public:
    static void openUrl(JNIEnv* env, jobject context, const std::string& url);
    static void openEmail(JNIEnv* env, jobject context,
                         const std::string& to,
                         const std::string& subject = "",
                         const std::string& body = "");
    static void openPhone(JNIEnv* env, jobject context, const std::string& number);
    static void openMaps(JNIEnv* env, jobject context, double lat, double lon);
    static void openAppSettings(JNIEnv* env, jobject context);
};

} // namespace apis
} // namespace android
} // namespace sad

#endif // SAD_ANDROID
#endif // SAD_ANDROID_APIS_H
