/**
 * @file sad_desktop.h
 * @brief (AR) واجهات برمجة سطح المكتب — صينية النظام، الإشعارات، الحافظة
 * @brief (EN) Desktop APIs — System Tray, Notifications, Clipboard
 * @date May 2026
 * @version 1.0
 * 
 * توفر هذه المكتبة واجهات عربية لأدوات سطح المكتب:
 * - صينية النظام (System Tray)
 * - الإشعارات (Notifications)
 * - الحافظة (Clipboard) - متقدمة
 * - معلومات الشاشة (Display Info)
 */

#ifndef SAD_DESKTOP_H
#define SAD_DESKTOP_H

#include "../../shared/types/include/value.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#undef VOID
#endif

namespace Sad {
namespace StdLib {
namespace Desktop {

// ============================================================================
// صينية النظام / System Tray
// ============================================================================

/**
 * @brief (AR) عنصر في قائمة صينية النظام
 * @brief (EN) System tray menu item
 */
struct TrayMenuItem {
    std::string label;                         ///< النص المعروض / Display text
    std::string id;                            ///< معرّف العنصر / Item identifier
    bool enabled = true;                       ///< مفعّل / Enabled
    bool separator = false;                    ///< فاصل / Is separator
    std::function<void()> callback;            ///< دالة الاستجابة / Callback
};

/**
 * @class SystemTray
 * @brief (AR) إدارة أيقونة صينية النظام
 * @brief (EN) System tray icon management
 */
class SystemTray {
public:
    SystemTray();
    ~SystemTray();

    /// (AR) إنشاء أيقونة في صينية النظام / Create tray icon
    bool create(const std::string& tooltip);
    
    /// (AR) إزالة أيقونة صينية النظام / Remove tray icon
    void remove();
    
    /// (AR) تعيين نص التلميح / Set tooltip text
    void setTooltip(const std::string& tooltip);
    
    /// (AR) إضافة عنصر للقائمة / Add menu item
    void addMenuItem(const TrayMenuItem& item);
    
    /// (AR) إظهار بالون إشعار / Show balloon notification
    void showBalloon(const std::string& title, const std::string& message, int timeoutMs = 3000);
    
    /// (AR) هل الصينية نشطة؟ / Is tray active?
    bool isActive() const { return active_; }

private:
    bool active_ = false;
    std::string tooltip_;
    std::vector<TrayMenuItem> menuItems_;
    
#ifdef _WIN32
    NOTIFYICONDATAW nid_;
    HWND hwnd_;
    bool createHiddenWindow();
#endif
};

// ============================================================================
// الإشعارات / Notifications
// ============================================================================

/**
 * @enum NotificationType
 * @brief (AR) نوع الإشعار / Notification severity level
 */
enum class NotificationType {
    INFO,       ///< معلومات / Information
    WARNING,    ///< تحذير / Warning
    ERROR_TYPE, ///< خطأ / Error
    SUCCESS     ///< نجاح / Success
};

/**
 * @class NotificationManager
 * @brief (AR) مدير إشعارات سطح المكتب
 * @brief (EN) Desktop notification manager
 */
class NotificationManager {
public:
    /// (AR) إرسال إشعار / Send notification
    static bool send(const std::string& title,
                     const std::string& message,
                     NotificationType type = NotificationType::INFO);
    
    /// (AR) إرسال إشعار مع مهلة / Send notification with timeout
    static bool sendWithTimeout(const std::string& title,
                                const std::string& message,
                                int timeoutMs,
                                NotificationType type = NotificationType::INFO);
};

// ============================================================================
// الحافظة المتقدمة / Advanced Clipboard
// ============================================================================

/**
 * @class Clipboard
 * @brief (AR) عمليات الحافظة المتقدمة
 * @brief (EN) Advanced clipboard operations
 */
class Clipboard {
public:
    /// (AR) نسخ نص إلى الحافظة / Copy text to clipboard
    static bool copyText(const std::string& text);
    
    /// (AR) لصق نص من الحافظة / Paste text from clipboard
    static std::string pasteText();
    
    /// (AR) هل تحتوي الحافظة على نص؟ / Does clipboard contain text?
    static bool hasText();
    
    /// (AR) مسح الحافظة / Clear clipboard
    static bool clear();
};

// ============================================================================
// معلومات الشاشة / Display Info
// ============================================================================

/**
 * @struct DisplayInfo
 * @brief (AR) معلومات الشاشة
 * @brief (EN) Display information
 */
struct DisplayInfo {
    int width = 0;               ///< عرض الشاشة / Screen width
    int height = 0;              ///< ارتفاع الشاشة / Screen height
    int dpi = 96;                ///< كثافة النقاط / DPI
    double scaleFactor = 1.0;    ///< معامل التحجيم / Scale factor
};

/**
 * @class Display
 * @brief (AR) استعلام معلومات الشاشة
 * @brief (EN) Query display information
 */
class Display {
public:
    /// (AR) الحصول على معلومات الشاشة الرئيسية
    static DisplayInfo getPrimaryDisplay();
    
    /// (AR) عدد الشاشات المتصلة
    static int getDisplayCount();
};

// ============================================================================
// دوال مدمجة للمفسر / Built-in functions for interpreter
// ============================================================================

/// صينية النظام / System Tray functions
Data::Value tray_create(const std::vector<Data::Value>& args);
Data::Value tray_remove(const std::vector<Data::Value>& args);
Data::Value tray_tooltip(const std::vector<Data::Value>& args);
Data::Value tray_notify(const std::vector<Data::Value>& args);

/// الإشعارات / Notification functions
Data::Value notify_send(const std::vector<Data::Value>& args);

/// الحافظة / Clipboard functions
Data::Value clipboard_copy(const std::vector<Data::Value>& args);
Data::Value clipboard_paste(const std::vector<Data::Value>& args);
Data::Value clipboard_has_text(const std::vector<Data::Value>& args);
Data::Value clipboard_clear(const std::vector<Data::Value>& args);

/// الشاشة / Display functions
Data::Value display_info(const std::vector<Data::Value>& args);
Data::Value display_count(const std::vector<Data::Value>& args);

} // namespace Desktop
} // namespace StdLib
} // namespace Sad

#endif // SAD_DESKTOP_H
