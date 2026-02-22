// بسم الله الرحمن الرحيم
// ==============================================================================
// mobile_module.h - وحدة تطبيقات الهاتف للمفسر
// Mobile Module for the Interpreter
// ==============================================================================
//
// يوفر دوال مضمنة لتطبيقات الهاتف يمكن استدعاؤها من كود ص
//
// الدوال المتوفرة:
// ──────────────
//   هاتف_معلومات()              - معلومات نظام الهاتف
//   هاتف_رسالة(عنوان، نص)       - عرض رسالة تنبيه
//   هاتف_إشعار(عنوان، نص)       - إرسال إشعار
//   واجهة_نص(محتوى)            - إنشاء عنصر نصي
//   واجهة_زر(عنوان)             - إنشاء عنصر زر
//   واجهة_عمود(عناصر...)        - تخطيط عمودي
//   واجهة_صف(عناصر...)          - تخطيط أفقي
//   كاميرا_التقط()              - التقاط صورة
//   موقع_حالي()                - الحصول على الموقع الحالي
//   مستشعر_تسارع()             - قراءة مستشعر التسارع
//
// المؤلف: فريق تطوير لغة ص
// التاريخ: فبراير 2026
// ==============================================================================

#ifndef SAD_STDLIB_MOBILE_MODULE_H
#define SAD_STDLIB_MOBILE_MODULE_H

#include <string>
#include <vector>
#include <map>

namespace sad {
namespace stdlib {
namespace mobile {

// ==============================================================================
// أنواع عناصر الواجهة / UI Element Types
// ==============================================================================

enum class UIElementType {
    Text,       // نص
    Button,     // زر
    Image,      // صورة
    Input,      // حقل إدخال
    Column,     // عمود
    Row,        // صف
    List,       // قائمة
    Card,       // بطاقة
    Container,  // حاوية
    Spacer      // فاصل
};

struct UIElement {
    UIElementType type;
    std::string content;
    std::map<std::string, std::string> properties;
    std::vector<UIElement> children;
    
    UIElement() : type(UIElementType::Container) {}
    UIElement(UIElementType t, const std::string& c = "") : type(t), content(c) {}
};

// ==============================================================================
// معلومات الجهاز / Device Information
// ==============================================================================

struct DeviceInfo {
    std::string platform;       // أندرويد أو iOS
    std::string os_version;     // إصدار النظام
    std::string device_name;    // اسم الجهاز
    int screen_width;           // عرض الشاشة
    int screen_height;          // ارتفاع الشاشة
    double screen_density;      // كثافة الشاشة
    bool has_camera;            // هل يوجد كاميرا
    bool has_gps;               // هل يوجد GPS
    bool has_accelerometer;     // هل يوجد مستشعر تسارع
};

// ==============================================================================
// الموقع الجغرافي / Geolocation
// ==============================================================================

struct Location {
    double latitude;    // خط العرض
    double longitude;   // خط الطول
    double altitude;    // الارتفاع
    double accuracy;    // الدقة بالأمتار
};

// ==============================================================================
// بيانات المستشعرات / Sensor Data
// ==============================================================================

struct AccelerometerData {
    double x;
    double y;
    double z;
};

struct GyroscopeData {
    double x;
    double y;
    double z;
};

// ==============================================================================
// دوال وحدة الهاتف / Mobile Module Functions
// ==============================================================================

// معلومات الجهاز
DeviceInfo get_device_info();
std::string get_platform_name();

// رسائل وإشعارات
bool show_alert(const std::string& title, const std::string& message);
bool show_notification(const std::string& title, const std::string& message);
bool show_toast(const std::string& message);

// واجهة المستخدم التصريحية
UIElement create_text(const std::string& content);
UIElement create_button(const std::string& label);
UIElement create_image(const std::string& source);
UIElement create_input(const std::string& placeholder);
UIElement create_column(const std::vector<UIElement>& children);
UIElement create_row(const std::vector<UIElement>& children);
UIElement create_list(const std::vector<std::string>& items);
UIElement create_card(const std::string& title, const std::string& content);

// تحويل واجهة إلى نص
std::string render_ui_element(const UIElement& element, int indent = 0);
std::string render_to_compose(const UIElement& element, int indent = 0);
std::string render_to_swiftui(const UIElement& element, int indent = 0);

// الكاميرا
std::string capture_photo();
bool is_camera_available();

// الموقع
Location get_current_location();
double calculate_distance(double lat1, double lon1, double lat2, double lon2);

// المستشعرات
AccelerometerData get_accelerometer();
GyroscopeData get_gyroscope();

// مشروع الهاتف
bool create_mobile_project(const std::string& name, const std::string& platform);
std::string generate_android_manifest(const std::string& package_name, const std::string& app_name);
std::string generate_ios_plist(const std::string& app_name, const std::string& bundle_id);

} // namespace mobile
} // namespace stdlib
} // namespace sad

#endif // SAD_STDLIB_MOBILE_MODULE_H
