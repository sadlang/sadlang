/**
 * @file mobile_module.cpp
 * @brief (AR) وحدة الهاتف المحمول — تنفيذ كامل
 * @brief (EN) Mobile module — full implementation
 * 
 * هذا الملف يوفر تنفيذ وظائف الهاتف المحمول للغة ص.
 * يدعم محاكاة Desktop وتوليد كود Kotlin/SwiftUI.
 */

#include "mobile/mobile_module.h"
#include <iostream>
#include <sstream>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <chrono>
#include <random>
#include <fstream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#endif

namespace sad {
namespace stdlib {
namespace mobile {

// =============================================================================
// Platform Detection
// =============================================================================

std::string detect_platform() {
#if defined(__ANDROID__)
    return "android";
#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    return "ios";
#elif defined(__APPLE__)
    return "macos";
#elif defined(_WIN32)
    return "windows";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown";
#endif
}

// =============================================================================
// DeviceInfo Implementation
// =============================================================================

DeviceInfo get_device_info_impl() {
    DeviceInfo info;
    info.platform = detect_platform();
    
#ifdef _WIN32
    // Windows desktop simulation
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    info.os_version = "Windows 10+";
    info.device_name = "Desktop (محاكاة)";
    
    // Get screen info
    HDC hdc = GetDC(NULL);
    if (hdc) {
        info.screen_width = GetDeviceCaps(hdc, HORZRES);
        info.screen_height = GetDeviceCaps(hdc, VERTRES);
        info.screen_density = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0;
        ReleaseDC(NULL, hdc);
    } else {
        info.screen_width = 1920;
        info.screen_height = 1080;
        info.screen_density = 1.0;
    }
    
    info.has_camera = true;  // Assume WebCam available
    info.has_gps = false;     // Desktop doesn't have GPS
    
#elif defined(__ANDROID__)
    // Will be filled by JNI at runtime
    info.os_version = "Android";
    info.device_name = "Android Device";
    info.screen_width = 1080;
    info.screen_height = 2400;
    info.screen_density = 2.5;
    info.has_camera = true;
    info.has_gps = true;
    
#elif defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    // Will be filled by Swift bridge at runtime
    info.os_version = "iOS";
    info.device_name = "iPhone";
    info.screen_width = 1170;
    info.screen_height = 2532;
    info.screen_density = 3.0;
    info.has_camera = true;
    info.has_gps = true;
    
#else
    // Generic desktop
    info.os_version = "Desktop";
    info.device_name = "محطة عمل";
    info.screen_width = 1920;
    info.screen_height = 1080;
    info.screen_density = 1.0;
    info.has_camera = false;
    info.has_gps = false;
#endif

    return info;
}

// =============================================================================
// UI Rendering - Jetpack Compose
// =============================================================================

std::string render_ui_element_to_compose_impl(const UIElement& elem, int indent) {
    std::string spaces(indent * 4, ' ');
    std::ostringstream ss;
    
    if (elem.type == "text") {
        ss << spaces << "Text(\n";
        ss << spaces << "    text = \"" << elem.text << "\",\n";
        ss << spaces << "    modifier = Modifier\n";
        ss << spaces << "        .semantics { contentDescription = \"" << elem.text << "\" }\n";
        ss << spaces << ")";
    }
    else if (elem.type == "button") {
        ss << spaces << "Button(\n";
        ss << spaces << "    onClick = { /* إجراء */ },\n";
        ss << spaces << "    modifier = Modifier.fillMaxWidth()\n";
        ss << spaces << ") {\n";
        ss << spaces << "    Text(\"" << elem.text << "\")\n";
        ss << spaces << "}";
    }
    else if (elem.type == "image") {
        ss << spaces << "Image(\n";
        ss << spaces << "    painter = painterResource(id = R.drawable." << elem.text << "),\n";
        ss << spaces << "    contentDescription = \"" << elem.text << "\",\n";
        ss << spaces << "    modifier = Modifier.size(200.dp)\n";
        ss << spaces << ")";
    }
    else if (elem.type == "column") {
        ss << spaces << "Column(\n";
        ss << spaces << "    modifier = Modifier\n";
        ss << spaces << "        .fillMaxWidth()\n";
        ss << spaces << "        .padding(16.dp),\n";
        ss << spaces << "    horizontalAlignment = Alignment.CenterHorizontally\n";
        ss << spaces << ") {\n";
        for (size_t i = 0; i < elem.children.size(); i++) {
            ss << render_ui_element_to_compose_impl(elem.children[i], indent + 1);
            if (i < elem.children.size() - 1) {
                ss << "\n" << spaces << "    Spacer(modifier = Modifier.height(8.dp))\n";
            }
        }
        ss << "\n" << spaces << "}";
    }
    else if (elem.type == "row") {
        ss << spaces << "Row(\n";
        ss << spaces << "    modifier = Modifier.fillMaxWidth(),\n";
        ss << spaces << "    horizontalArrangement = Arrangement.SpaceEvenly,\n";
        ss << spaces << "    verticalAlignment = Alignment.CenterVertically\n";
        ss << spaces << ") {\n";
        for (size_t i = 0; i < elem.children.size(); i++) {
            ss << render_ui_element_to_compose_impl(elem.children[i], indent + 1);
            if (i < elem.children.size() - 1) ss << "\n";
        }
        ss << "\n" << spaces << "}";
    }
    
    return ss.str();
}

// =============================================================================
// UI Rendering - SwiftUI
// =============================================================================

std::string render_ui_element_to_swiftui_impl(const UIElement& elem, int indent) {
    std::string spaces(indent * 4, ' ');
    std::ostringstream ss;
    
    if (elem.type == "text") {
        ss << spaces << "Text(\"" << elem.text << "\")\n";
        ss << spaces << "    .accessibilityLabel(\"" << elem.text << "\")";
    }
    else if (elem.type == "button") {
        ss << spaces << "Button(\"" << elem.text << "\") {\n";
        ss << spaces << "    // إجراء\n";
        ss << spaces << "}\n";
        ss << spaces << ".buttonStyle(.borderedProminent)\n";
        ss << spaces << ".frame(maxWidth: .infinity)";
    }
    else if (elem.type == "image") {
        ss << spaces << "Image(\"" << elem.text << "\")\n";
        ss << spaces << "    .resizable()\n";
        ss << spaces << "    .scaledToFit()\n";
        ss << spaces << "    .frame(width: 200, height: 200)";
    }
    else if (elem.type == "column") {
        ss << spaces << "VStack(spacing: 8) {\n";
        for (const auto& child : elem.children) {
            ss << render_ui_element_to_swiftui_impl(child, indent + 1) << "\n";
        }
        ss << spaces << "}\n";
        ss << spaces << ".padding()";
    }
    else if (elem.type == "row") {
        ss << spaces << "HStack(spacing: 8) {\n";
        for (const auto& child : elem.children) {
            ss << render_ui_element_to_swiftui_impl(child, indent + 1) << "\n";
        }
        ss << spaces << "}";
    }
    
    return ss.str();
}

// =============================================================================
// Location - Haversine Distance Calculator
// =============================================================================

double calculate_distance_impl(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Earth radius in kilometers
    const double RAD = M_PI / 180.0;
    
    double dLat = (lat2 - lat1) * RAD;
    double dLon = (lon2 - lon1) * RAD;
    
    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1 * RAD) * std::cos(lat2 * RAD) *
               std::sin(dLon / 2) * std::sin(dLon / 2);
    
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    
    return R * c; // Distance in kilometers
}

// =============================================================================
// Project Generation
// =============================================================================

bool create_mobile_project_impl(const std::string& name, const std::string& platform) {
    namespace fs = std::filesystem;
    
    try {
        fs::path project_dir = fs::current_path() / name;
        
        if (fs::exists(project_dir)) {
            std::cerr << "❌ المجلد موجود مسبقاً: " << name << std::endl;
            return false;
        }
        
        // Create directory structure
        if (platform == "android" || platform == "اندرويد" || platform == "both") {
            fs::create_directories(project_dir / "android" / "app" / "src" / "main" / "kotlin");
            fs::create_directories(project_dir / "android" / "app" / "src" / "main" / "res" / "values");
        }
        
        if (platform == "ios" || platform == "آيفون" || platform == "both") {
            fs::create_directories(project_dir / "ios" / name);
            fs::create_directories(project_dir / "ios" / name / "Assets.xcassets");
        }
        
        // Create shared directory
        fs::create_directories(project_dir / "shared" / "src");
        
        // Create main sad file
        std::ofstream main_file(project_dir / "shared" / "src" / "main.ص");
        main_file << "# " << name << " - تطبيق هاتف بلغة ص\n\n";
        main_file << "واجهة الرئيسية\n";
        main_file << "    @حالة العداد = 0\n\n";
        main_file << "    عرض\n";
        main_file << "        عمود\n";
        main_file << "            نص(\"مرحباً من " << name << "!\")\n";
        main_file << "            زر(\"اضغط هنا\") ← العداد += 1\n";
        main_file << "            نص(\"الضغطات: {العداد}\")\n";
        main_file << "        نهاية\n";
        main_file << "    نهاية\n";
        main_file << "نهاية\n";
        main_file.close();
        
        std::cout << "✅ تم إنشاء مشروع: " << name << std::endl;
        std::cout << "📁 الهيكل:\n";
        std::cout << "   " << name << "/\n";
        std::cout << "   ├── shared/src/main.ص\n";
        if (platform == "android" || platform == "اندرويد" || platform == "both") {
            std::cout << "   ├── android/\n";
        }
        if (platform == "ios" || platform == "آيفون" || platform == "both") {
            std::cout << "   └── ios/\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "❌ خطأ في إنشاء المشروع: " << e.what() << std::endl;
        return false;
    }
}

std::string generate_android_manifest_impl(const std::string& package_name, const std::string& app_name) {
    std::ostringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    ss << "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n";
    ss << "    package=\"" << package_name << "\">\n\n";
    ss << "    <uses-permission android:name=\"android.permission.INTERNET\" />\n";
    ss << "    <uses-permission android:name=\"android.permission.ACCESS_FINE_LOCATION\" />\n";
    ss << "    <uses-permission android:name=\"android.permission.CAMERA\" />\n\n";
    ss << "    <application\n";
    ss << "        android:allowBackup=\"true\"\n";
    ss << "        android:icon=\"@mipmap/ic_launcher\"\n";
    ss << "        android:label=\"" << app_name << "\"\n";
    ss << "        android:supportsRtl=\"true\"\n";
    ss << "        android:theme=\"@style/Theme.SadApp\">\n\n";
    ss << "        <activity\n";
    ss << "            android:name=\".MainActivity\"\n";
    ss << "            android:exported=\"true\"\n";
    ss << "            android:theme=\"@style/Theme.SadApp\">\n";
    ss << "            <intent-filter>\n";
    ss << "                <action android:name=\"android.intent.action.MAIN\" />\n";
    ss << "                <category android:name=\"android.intent.category.LAUNCHER\" />\n";
    ss << "            </intent-filter>\n";
    ss << "        </activity>\n";
    ss << "    </application>\n";
    ss << "</manifest>\n";
    return ss.str();
}

std::string generate_ios_plist_impl(const std::string& bundle_id, const std::string& app_name) {
    std::ostringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n";
    ss << "<plist version=\"1.0\">\n";
    ss << "<dict>\n";
    ss << "    <key>CFBundleDevelopmentRegion</key>\n";
    ss << "    <string>ar</string>\n";
    ss << "    <key>CFBundleExecutable</key>\n";
    ss << "    <string>$(EXECUTABLE_NAME)</string>\n";
    ss << "    <key>CFBundleIdentifier</key>\n";
    ss << "    <string>" << bundle_id << "</string>\n";
    ss << "    <key>CFBundleName</key>\n";
    ss << "    <string>" << app_name << "</string>\n";
    ss << "    <key>CFBundlePackageType</key>\n";
    ss << "    <string>APPL</string>\n";
    ss << "    <key>CFBundleShortVersionString</key>\n";
    ss << "    <string>1.0</string>\n";
    ss << "    <key>CFBundleVersion</key>\n";
    ss << "    <string>1</string>\n";
    ss << "    <key>LSRequiresIPhoneOS</key>\n";
    ss << "    <true/>\n";
    ss << "    <key>UILaunchStoryboardName</key>\n";
    ss << "    <string>LaunchScreen</string>\n";
    ss << "    <key>UIRequiredDeviceCapabilities</key>\n";
    ss << "    <array>\n";
    ss << "        <string>armv7</string>\n";
    ss << "    </array>\n";
    ss << "    <key>UISupportedInterfaceOrientations</key>\n";
    ss << "    <array>\n";
    ss << "        <string>UIInterfaceOrientationPortrait</string>\n";
    ss << "    </array>\n";
    ss << "    <key>NSCameraUsageDescription</key>\n";
    ss << "    <string>التطبيق يحتاج الوصول للكاميرا</string>\n";
    ss << "    <key>NSLocationWhenInUseUsageDescription</key>\n";
    ss << "    <string>التطبيق يحتاج الوصول للموقع</string>\n";
    ss << "</dict>\n";
    ss << "</plist>\n";
    return ss.str();
}

// =============================================================================
// Simulated Sensor Data (for desktop testing)
// =============================================================================

static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

SensorData get_simulated_sensor_data() {
    SensorData data;
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    data.x = dist(rng);
    data.y = dist(rng);
    data.z = 9.8 + dist(rng) * 0.1; // Simulate gravity on z-axis
    return data;
}

Location get_simulated_location() {
    Location loc;
    // Default: Riyadh, Saudi Arabia
    loc.latitude = 24.7136 + (rng() % 100 - 50) * 0.0001;
    loc.longitude = 46.6753 + (rng() % 100 - 50) * 0.0001;
    loc.altitude = 600.0;
    return loc;
}

// =============================================================================
// Public API Implementation (replacing inline stubs)
// =============================================================================

DeviceInfo get_device_info() {
    return get_device_info_impl();
}

std::string get_platform_name() {
    return detect_platform();
}

bool show_alert(const std::string& title, const std::string& message) {
#ifdef _WIN32
    int result = MessageBoxA(NULL, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
    return result == IDOK;
#else
    std::cout << "📢 [" << title << "]: " << message << std::endl;
    return true;
#endif
}

bool show_notification(const std::string& title, const std::string& message) {
    std::cout << "🔔 إشعار [" << title << "]: " << message << std::endl;
    return true;
}

bool show_toast(const std::string& message) {
    std::cout << "💬 " << message << std::endl;
    return true;
}

UIElement create_text(const std::string& t) {
    return {"text", t, {}};
}

UIElement create_button(const std::string& t) {
    return {"button", t, {}};
}

UIElement create_image(const std::string& t) {
    return {"image", t, {}};
}

UIElement create_column(const std::vector<UIElement>& c) {
    return {"column", "", c};
}

UIElement create_row(const std::vector<UIElement>& c) {
    return {"row", "", c};
}

std::string render_ui_element(const UIElement& e) {
    // Simple text representation
    std::ostringstream ss;
    ss << "[" << e.type << "]";
    if (!e.text.empty()) ss << " " << e.text;
    if (!e.children.empty()) {
        ss << " {";
        for (const auto& child : e.children) {
            ss << " " << render_ui_element(child);
        }
        ss << " }";
    }
    return ss.str();
}

std::string render_to_compose(const UIElement& e) {
    return render_ui_element_to_compose_impl(e, 0);
}

std::string render_to_swiftui(const UIElement& e) {
    return render_ui_element_to_swiftui_impl(e, 0);
}

std::string capture_photo() {
    std::cout << "📷 التقاط صورة... (محاكاة)" << std::endl;
#ifdef __ANDROID__
    // Will be implemented via JNI
    return "photo_captured.jpg";
#else
    return "simulated_photo.png";
#endif
}

bool is_camera_available() {
#ifdef __ANDROID__
    return true;
#elif defined(TARGET_OS_IPHONE)
    return true;
#elif defined(_WIN32)
    // Check for webcam on Windows (simplified)
    return true;
#else
    return false;
#endif
}

Location get_current_location() {
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
    // Real implementation via platform bridge
    Location loc;
    loc.latitude = 0;
    loc.longitude = 0;
    loc.altitude = 0;
    return loc;
#else
    return get_simulated_location();
#endif
}

double calculate_distance(double lat1, double lon1, double lat2, double lon2) {
    return calculate_distance_impl(lat1, lon1, lat2, lon2);
}

SensorData get_accelerometer() {
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
    SensorData data;
    // Real implementation via platform bridge
    return data;
#else
    return get_simulated_sensor_data();
#endif
}

SensorData get_gyroscope() {
#if defined(__ANDROID__) || defined(TARGET_OS_IPHONE)
    SensorData data;
    // Real implementation via platform bridge
    return data;
#else
    SensorData data = get_simulated_sensor_data();
    data.z = 0; // Gyroscope doesn't have gravity
    return data;
#endif
}

bool create_mobile_project(const std::string& name, const std::string& platform) {
    return create_mobile_project_impl(name, platform);
}

std::string generate_android_manifest(const std::string& package_name, const std::string& app_name) {
    return generate_android_manifest_impl(package_name, app_name);
}

std::string generate_ios_plist(const std::string& bundle_id, const std::string& app_name) {
    return generate_ios_plist_impl(bundle_id, app_name);
}

} // namespace mobile
} // namespace stdlib
} // namespace sad
