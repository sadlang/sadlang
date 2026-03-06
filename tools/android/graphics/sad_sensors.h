#ifndef SAD_SENSORS_H
#define SAD_SENSORS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ─────────────────────────────────────────────────────────────────────────────
//  أنواع الحساسات
// ─────────────────────────────────────────────────────────────────────────────

typedef enum {
    SAD_SENSOR_ACCELEROMETER, // التسارع
    SAD_SENSOR_GYROSCOPE,     // الجيروسكوب
    SAD_SENSOR_MAGNETOMETER,  // المغناطيسية
    SAD_SENSOR_LIGHT,         // الإضاءة
    SAD_SENSOR_PROXIMITY      // التقارب
} SadSensorType;

typedef struct {
    float x;             // المحور السيني (X)
    float y;             // المحور الصادي (Y)
    float z;             // المحور العيني (Z)
    float value;         // قيمة مفردة للحساسات ذات البعد الواحد (إضاءة/تقارب)
    long long timestamp; // وقت القراءة
    bool isValid;        // هل القراءة صالحة؟
} SadSensorEvent;

// ─────────────────────────────────────────────────────────────────────────────
// دوال إدارة الحساسات الأساسية (تعمل مباشرة عبر Android NDK)
// ─────────────────────────────────────────────────────────────────────────────

/**
 * تهيئة نظام الحساسات.
 * @param androidApp مؤشر لسياق التطبيق (تُمرر عند بدء واجهة المستخدم)
 * @return true إذا نجحت التهيئة
 */
bool sadsensors_init(void* androidApp);

/**
 * تدمير نظام الحساسات وإيقاف التنصت
 */
void sadsensors_shutdown(void);

/**
 * تفعيل حساس معين للبدء في قراءة البيانات
 */
bool sadsensors_enable(SadSensorType type);

/**
 * تعطيل حساس معين لإيقاف قراءة البيانات وتوفير الطاقة
 */
void sadsensors_disable(SadSensorType type);

/**
 * قراءة آخر حالة مسجلة للحساس
 * @param outEvent هيكل يحفظ قراءة الحساس
 */
bool sadsensors_read(SadSensorType type, SadSensorEvent* outEvent);

// ─────────────────────────────────────────────────────────────────────────────
// دوال الموقع (GPS) - تتطلب JNI لأنها مدعومة في طبقة الـ Java/Kotlin بشكل أفضل
// ─────────────────────────────────────────────────────────────────────────────

typedef struct {
    double latitude;     // خط العرض
    double longitude;    // خط الطول
    double altitude;     // الارتفاع
    float accuracy;      // الدقة (بالمتر)
    long long timestamp; // الوقت
    bool isValid;        // الصلاحية
} SadLocation;

/** طلب صلاحيات الموقع الجغرافي */
bool sadgps_request_permissions(void);

/** تفعيل تحديد الموقع الجغرافي */
bool sadgps_start_updates(void);

/** إيقاف تحديد الموقع */
void sadgps_stop_updates(void);

/** الحصول على آخر موقع معروف */
bool sadgps_get_last_location(SadLocation* outLocation);

// ─────────────────────────────────────────────────────────────────────────────
// الكاميرا ومعالجة الصور
// ─────────────────────────────────────────────────────────────────────────────

typedef void (*SadCameraCallback)(const char* savedPath, bool success);

/**
 * إطلاق نية (Intent) لتطبيق الكاميرا الافتراضي والتقاط صورة والحصول على مسارها
 * @param outputPath المسار المراد فيه حفظ الصورة
 * @param callback دالة تُنادى فور الانتهاء من التقاط الصورة
 */
bool sadcamera_capture_image(const char* outputPath, SadCameraCallback callback);

#ifdef __cplusplus
}
#endif

#endif // SAD_SENSORS_H