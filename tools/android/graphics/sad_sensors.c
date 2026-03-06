#include "sad_sensors.h"
#include <android/sensor.h>
#include <android/looper.h>
#include <android/log.h>
#include <stddef.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SadSensors", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "SadSensors", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "SadSensors", __VA_ARGS__))

static ASensorManager* sensorManager = NULL;
static const ASensor* accelerometerSensor = NULL;
static const ASensor* gyroscopeSensor = NULL;
static const ASensor* magnetometerSensor = NULL;
static const ASensor* lightSensor = NULL;
static const ASensor* proximitySensor = NULL;
static ASensorEventQueue* sensorEventQueue = NULL;

static SadSensorEvent lastAccel = {0};
static SadSensorEvent lastGyro = {0};
static SadSensorEvent lastMag = {0};
static SadSensorEvent lastLight = {0};
static SadSensorEvent lastProx = {0};

bool sadsensors_init(void* androidApp) {
    if (sensorManager != NULL) return true; // Already initialized

    // Try to get ASensorManager instance. Works perfectly on API < 26.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    sensorManager = ASensorManager_getInstance();
#pragma clang diagnostic pop

    if (!sensorManager) {
        LOGE("Failed to get ASensorManager");
        return false;
    }

    accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    gyroscopeSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);
    magnetometerSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_MAGNETIC_FIELD);
    lightSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_LIGHT);
    proximitySensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_PROXIMITY);

    ALooper* looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    if (!looper) {
        LOGE("Failed to get ALooper");
        return false;
    }

    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, 3, NULL, NULL);
    if (!sensorEventQueue) {
        LOGE("Failed to create ASensorEventQueue");
        return false;
    }

    LOGI("Sensors subsystem initialized (NDK core sensors).");
    return true;
}

void sadsensors_shutdown(void) {
    if (sensorManager && sensorEventQueue) {
        ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
        sensorEventQueue = NULL;
    }
    sensorManager = NULL;
}

bool sadsensors_enable(SadSensorType type) {
    if (!sensorEventQueue) return false;
    
    const ASensor* target = NULL;
    switch(type) {
        case SAD_SENSOR_ACCELEROMETER: target = accelerometerSensor; break;
        case SAD_SENSOR_GYROSCOPE: target = gyroscopeSensor; break;
        case SAD_SENSOR_MAGNETOMETER: target = magnetometerSensor; break;
        case SAD_SENSOR_LIGHT: target = lightSensor; break;
        case SAD_SENSOR_PROXIMITY: target = proximitySensor; break;
    }

    if (target) {
        ASensorEventQueue_enableSensor(sensorEventQueue, target);
        // Set sample rate to ~60Hz (measured in microseconds)
        ASensorEventQueue_setEventRate(sensorEventQueue, target, 1000000 / 60);
        return true;
    }
    return false;
}

void sadsensors_disable(SadSensorType type) {
    if (!sensorEventQueue) return;
    
    const ASensor* target = NULL;
    switch(type) {
        case SAD_SENSOR_ACCELEROMETER: target = accelerometerSensor; break;
        case SAD_SENSOR_GYROSCOPE: target = gyroscopeSensor; break;
        case SAD_SENSOR_MAGNETOMETER: target = magnetometerSensor; break;
        case SAD_SENSOR_LIGHT: target = lightSensor; break;
        case SAD_SENSOR_PROXIMITY: target = proximitySensor; break;
    }

    if (target) {
        ASensorEventQueue_disableSensor(sensorEventQueue, target);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// تحديث البيانات من نظام أندرويد بشكل غير متزامن
// ─────────────────────────────────────────────────────────────────────────────
static void update_events_queue() {
    if (!sensorEventQueue) return;
    ASensorEvent event;
    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0) {
        if (event.type == ASENSOR_TYPE_ACCELEROMETER) {
            lastAccel.x = event.acceleration.x;
            lastAccel.y = event.acceleration.y;
            lastAccel.z = event.acceleration.z;
            lastAccel.timestamp = event.timestamp;
            lastAccel.isValid = true;
        } else if (event.type == ASENSOR_TYPE_GYROSCOPE) {
            lastGyro.x = event.vector.x;
            lastGyro.y = event.vector.y;
            lastGyro.z = event.vector.z;
            lastGyro.timestamp = event.timestamp;
            lastGyro.isValid = true;
        } else if (event.type == ASENSOR_TYPE_MAGNETIC_FIELD) {
            lastMag.x = event.magnetic.x;
            lastMag.y = event.magnetic.y;
            lastMag.z = event.magnetic.z;
            lastMag.timestamp = event.timestamp;
            lastMag.isValid = true;
        } else if (event.type == ASENSOR_TYPE_LIGHT) {
            lastLight.value = event.light;
            lastLight.timestamp = event.timestamp;
            lastLight.isValid = true;
        } else if (event.type == ASENSOR_TYPE_PROXIMITY) {
            lastProx.value = event.distance;
            lastProx.timestamp = event.timestamp;
            lastProx.isValid = true;
        }
    }
}

bool sadsensors_read(SadSensorType type, SadSensorEvent* outEvent) {
    if (!outEvent) return false;
    update_events_queue(); // استنزاف الأحداث المعلقة

    SadSensorEvent* source = NULL;
    switch(type) {
        case SAD_SENSOR_ACCELEROMETER: source = &lastAccel; break;
        case SAD_SENSOR_GYROSCOPE: source = &lastGyro; break;
        case SAD_SENSOR_MAGNETOMETER: source = &lastMag; break;
        case SAD_SENSOR_LIGHT: source = &lastLight; break;
        case SAD_SENSOR_PROXIMITY: source = &lastProx; break;
    }

    if (source && source->isValid) {
        *outEvent = *source;
        return true;
    }
    
    outEvent->isValid = false;
    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// دوال تحديد الموقع والكاميرا (متروكة للـ JNI)
// ─────────────────────────────────────────────────────────────────────────────

bool sadgps_request_permissions(void) {
    LOGW("sadgps_request_permissions will be implemented in JVM/Kotlin bridge layer.");
    return false;
}

bool sadgps_start_updates(void) {
    LOGW("sadgps_start_updates not mapped to JNI yet.");
    return false;
}

void sadgps_stop_updates(void) {
    LOGW("sadgps_stop_updates not mapped to JNI yet.");
}

bool sadgps_get_last_location(SadLocation* outLocation) {
    if(outLocation) outLocation->isValid = false;
    return false;
}

bool sadcamera_capture_image(const char* outputPath, SadCameraCallback callback) {
    LOGW("sadcamera_capture_image will trigger an Intent via UI bridge later.");
    return false;
}
