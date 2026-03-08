#pragma once
/**
 * @file mobile_module.h
 * @brief (AR) وحدة الهاتف المحمول — واجهة كاملة
 * @brief (EN) Mobile module — full interface for mobile development
 * 
 * توفر هذه الوحدة دوال للتعامل مع:
 * - معلومات الجهاز (Device Info)
 * - الإشعارات والتنبيهات (Alerts/Notifications)
 * - عناصر واجهة المستخدم (UI Elements)
 * - توليد كود Kotlin Compose و SwiftUI
 * - الكاميرا (Camera)
 * - الموقع الجغرافي (Location)
 * - المستشعرات (Sensors)
 * - إنشاء مشاريع الهاتف (Project Generation)
 */

#include <string>
#include <vector>

namespace sad {
namespace stdlib {
namespace mobile {

// =============================================================================
// Device Information / معلومات الجهاز
// =============================================================================

/**
 * @brief (AR) بنية معلومات الجهاز
 * @brief (EN) Device information structure
 */
struct DeviceInfo {
    std::string platform = "unknown";   ///< android, ios, windows, macos, linux
    std::string os_version = "0.0";     ///< إصدار نظام التشغيل
    std::string device_name = "unknown"; ///< اسم الجهاز
    int screen_width = 0;               ///< عرض الشاشة بالبكسل
    int screen_height = 0;              ///< ارتفاع الشاشة بالبكسل
    double screen_density = 1.0;        ///< كثافة الشاشة (DPI ratio)
    bool has_camera = false;            ///< هل الكاميرا متوفرة؟
    bool has_gps = false;               ///< هل GPS متوفر؟
};

/**
 * @brief (AR) الحصول على معلومات الجهاز الحالي
 * @brief (EN) Get current device information
 */
DeviceInfo get_device_info();

/**
 * @brief (AR) الحصول على اسم المنصة فقط
 * @brief (EN) Get platform name only
 */
std::string get_platform_name();

// =============================================================================
// Alerts & Notifications / التنبيهات والإشعارات
// =============================================================================

/**
 * @brief (AR) عرض تنبيه (alert) للمستخدم
 * @brief (EN) Show alert dialog to user
 */
bool show_alert(const std::string& title, const std::string& message = "");

/**
 * @brief (AR) عرض إشعار (notification)
 * @brief (EN) Show notification
 */
bool show_notification(const std::string& title, const std::string& message = "");

/**
 * @brief (AR) عرض رسالة سريعة (toast)
 * @brief (EN) Show toast message
 */
bool show_toast(const std::string& message);

// =============================================================================
// UI Elements / عناصر واجهة المستخدم
// =============================================================================

/**
 * @brief (AR) بنية عنصر واجهة المستخدم
 * @brief (EN) UI Element structure
 */
struct UIElement {
    std::string type;                  ///< text, button, image, column, row
    std::string text;                  ///< النص أو اسم المورد
    std::vector<UIElement> children;   ///< العناصر الفرعية
};

/**
 * @brief (AR) إنشاء عنصر نص
 * @brief (EN) Create text element
 */
UIElement create_text(const std::string& text);

/**
 * @brief (AR) إنشاء عنصر زر
 * @brief (EN) Create button element
 */
UIElement create_button(const std::string& text);

/**
 * @brief (AR) إنشاء عنصر صورة
 * @brief (EN) Create image element
 */
UIElement create_image(const std::string& resource_name);

/**
 * @brief (AR) إنشاء عمود (تخطيط عمودي)
 * @brief (EN) Create column (vertical layout)
 */
UIElement create_column(const std::vector<UIElement>& children);

/**
 * @brief (AR) إنشاء صف (تخطيط أفقي)
 * @brief (EN) Create row (horizontal layout)
 */
UIElement create_row(const std::vector<UIElement>& children);

/**
 * @brief (AR) تحويل عنصر UI لنص (للتصحيح)
 * @brief (EN) Render UI element to text (for debugging)
 */
std::string render_ui_element(const UIElement& element);

/**
 * @brief (AR) توليد كود Jetpack Compose
 * @brief (EN) Generate Jetpack Compose code
 */
std::string render_to_compose(const UIElement& element);

/**
 * @brief (AR) توليد كود SwiftUI
 * @brief (EN) Generate SwiftUI code
 */
std::string render_to_swiftui(const UIElement& element);

// =============================================================================
// Camera / الكاميرا
// =============================================================================

/**
 * @brief (AR) التقاط صورة
 * @brief (EN) Capture photo
 * @return مسار الصورة الملتقطة أو "" في حالة الفشل
 */
std::string capture_photo();

/**
 * @brief (AR) التحقق من توفر الكاميرا
 * @brief (EN) Check if camera is available
 */
bool is_camera_available();

// =============================================================================
// Location / الموقع الجغرافي
// =============================================================================

/**
 * @brief (AR) بنية الموقع الجغرافي
 * @brief (EN) Location structure
 */
struct Location {
    double latitude = 0.0;   ///< خط العرض
    double longitude = 0.0;  ///< خط الطول
    double altitude = 0.0;   ///< الارتفاع بالمتر
};

/**
 * @brief (AR) الحصول على الموقع الحالي
 * @brief (EN) Get current location
 */
Location get_current_location();

/**
 * @brief (AR) حساب المسافة بين نقطتين (بالكيلومتر)
 * @brief (EN) Calculate distance between two points (in km)
 * @details Uses Haversine formula
 */
double calculate_distance(double lat1, double lon1, double lat2, double lon2);

// =============================================================================
// Sensors / المستشعرات
// =============================================================================

/**
 * @brief (AR) بنية بيانات المستشعر
 * @brief (EN) Sensor data structure
 */
struct SensorData {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

/**
 * @brief (AR) قراءة مستشعر التسارع
 * @brief (EN) Read accelerometer sensor
 */
SensorData get_accelerometer();

/**
 * @brief (AR) قراءة الجيروسكوب
 * @brief (EN) Read gyroscope sensor
 */
SensorData get_gyroscope();

// =============================================================================
// Project Generation / إنشاء المشاريع
// =============================================================================

/**
 * @brief (AR) إنشاء مشروع هاتف جديد
 * @brief (EN) Create new mobile project
 * @param name اسم المشروع
 * @param platform المنصة: "android", "ios", "both", "اندرويد", "آيفون"
 */
bool create_mobile_project(const std::string& name, const std::string& platform);

/**
 * @brief (AR) توليد AndroidManifest.xml
 * @brief (EN) Generate AndroidManifest.xml
 */
std::string generate_android_manifest(const std::string& package_name, const std::string& app_name);

/**
 * @brief (AR) توليد Info.plist لـ iOS
 * @brief (EN) Generate iOS Info.plist
 */
std::string generate_ios_plist(const std::string& bundle_id, const std::string& app_name);

} // namespace mobile
} // namespace stdlib
} // namespace sad
