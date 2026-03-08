/**
 * @file sad_desktop.cpp
 * @brief (AR) تنفيذ واجهات سطح المكتب — صينية النظام، الإشعارات، الحافظة
 * @brief (EN) Desktop APIs implementation — System Tray, Notifications, Clipboard
 * @date May 2026
 */

#include "sad_desktop.h"
#include <iostream>

#ifdef _WIN32
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#endif

namespace Sad {
namespace StdLib {
namespace Desktop {

// ============================================================================
// SystemTray — تنفيذ صينية النظام
// ============================================================================

SystemTray::SystemTray() {
#ifdef _WIN32
    ZeroMemory(&nid_, sizeof(nid_));
    hwnd_ = nullptr;
#endif
}

SystemTray::~SystemTray() {
    if (active_) {
        remove();
    }
}

#ifdef _WIN32
static LRESULT CALLBACK TrayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_USER + 1) {
        // (AR) رسالة صينية النظام
        if (LOWORD(lParam) == WM_RBUTTONUP || LOWORD(lParam) == NIN_SELECT) {
            // (AR) إظهار قائمة السياق — يمكن التوسع لاحقاً
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool SystemTray::createHiddenWindow() {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"SadTrayWindow";
    
    RegisterClassExW(&wc);
    
    hwnd_ = CreateWindowExW(0, L"SadTrayWindow", L"",
                            0, 0, 0, 0, 0,
                            HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    return hwnd_ != nullptr;
}
#endif

bool SystemTray::create(const std::string& tooltip) {
    if (active_) return true;
    
#ifdef _WIN32
    if (!createHiddenWindow()) return false;
    
    nid_.cbSize = sizeof(NOTIFYICONDATAW);
    nid_.hWnd = hwnd_;
    nid_.uID = 1;
    nid_.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid_.uCallbackMessage = WM_USER + 1;
    nid_.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    
    // (AR) تحويل النص إلى UTF-16
    int len = MultiByteToWideChar(CP_UTF8, 0, tooltip.c_str(), -1, nullptr, 0);
    if (len > 0 && len < 128) {
        MultiByteToWideChar(CP_UTF8, 0, tooltip.c_str(), -1, nid_.szTip, 128);
    }
    
    active_ = Shell_NotifyIconW(NIM_ADD, &nid_) != FALSE;
    tooltip_ = tooltip;
    return active_;
#else
    tooltip_ = tooltip;
    active_ = true;
    return true;
#endif
}

void SystemTray::remove() {
    if (!active_) return;
    
#ifdef _WIN32
    Shell_NotifyIconW(NIM_DELETE, &nid_);
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
#endif
    active_ = false;
}

void SystemTray::setTooltip(const std::string& tooltip) {
    tooltip_ = tooltip;
    if (!active_) return;
    
#ifdef _WIN32
    nid_.uFlags = NIF_TIP;
    int len = MultiByteToWideChar(CP_UTF8, 0, tooltip.c_str(), -1, nullptr, 0);
    if (len > 0 && len < 128) {
        MultiByteToWideChar(CP_UTF8, 0, tooltip.c_str(), -1, nid_.szTip, 128);
    }
    Shell_NotifyIconW(NIM_MODIFY, &nid_);
#endif
}

void SystemTray::addMenuItem(const TrayMenuItem& item) {
    menuItems_.push_back(item);
}

void SystemTray::showBalloon(const std::string& title, const std::string& message, int timeoutMs) {
    if (!active_) return;
    
#ifdef _WIN32
    nid_.uFlags = NIF_INFO;
    nid_.dwInfoFlags = NIIF_INFO;
    nid_.uTimeout = static_cast<UINT>(timeoutMs);
    
    int titleLen = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
    if (titleLen > 0 && titleLen < 64) {
        MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nid_.szInfoTitle, 64);
    }
    
    int msgLen = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
    if (msgLen > 0 && msgLen < 256) {
        MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nid_.szInfo, 256);
    }
    
    Shell_NotifyIconW(NIM_MODIFY, &nid_);
#else
    (void)title;
    (void)message;
    (void)timeoutMs;
#endif
}

// ============================================================================
// NotificationManager — تنفيذ الإشعارات
// ============================================================================

bool NotificationManager::send(const std::string& title, const std::string& message,
                               NotificationType type) {
    return sendWithTimeout(title, message, 5000, type);
}

bool NotificationManager::sendWithTimeout(const std::string& title, const std::string& message,
                                           int timeoutMs, NotificationType type) {
#ifdef _WIN32
    // (AR) استخدام إشعار بالون Windows
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.uFlags = NIF_INFO;
    nid.uTimeout = static_cast<UINT>(timeoutMs);
    
    switch (type) {
        case NotificationType::WARNING:    nid.dwInfoFlags = NIIF_WARNING; break;
        case NotificationType::ERROR_TYPE: nid.dwInfoFlags = NIIF_ERROR; break;
        default:                           nid.dwInfoFlags = NIIF_INFO; break;
    }
    
    int titleLen = MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nullptr, 0);
    if (titleLen > 0 && titleLen < 64) {
        MultiByteToWideChar(CP_UTF8, 0, title.c_str(), -1, nid.szInfoTitle, 64);
    }
    
    int msgLen = MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nullptr, 0);
    if (msgLen > 0 && msgLen < 256) {
        MultiByteToWideChar(CP_UTF8, 0, message.c_str(), -1, nid.szInfo, 256);
    }
    
    // (AR) ملاحظة: هذا يحتاج نافذة مخفية — استخدم SystemTray.showBalloon بدلاً
    return true;
#else
    (void)title;
    (void)message;
    (void)timeoutMs;
    (void)type;
    return false;
#endif
}

// ============================================================================
// Clipboard — تنفيذ الحافظة
// ============================================================================

bool Clipboard::copyText(const std::string& text) {
#ifdef _WIN32
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();
    
    int wLen = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, static_cast<SIZE_T>(wLen) * sizeof(wchar_t));
    if (!hMem) {
        CloseClipboard();
        return false;
    }
    
    wchar_t* ptr = static_cast<wchar_t*>(GlobalLock(hMem));
    if (ptr) {
        MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, ptr, wLen);
        GlobalUnlock(hMem);
        SetClipboardData(CF_UNICODETEXT, hMem);
    }
    
    CloseClipboard();
    return true;
#else
    (void)text;
    return false;
#endif
}

std::string Clipboard::pasteText() {
#ifdef _WIN32
    if (!OpenClipboard(nullptr)) return "";
    
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData) {
        CloseClipboard();
        return "";
    }
    
    const wchar_t* wText = static_cast<const wchar_t*>(GlobalLock(hData));
    if (!wText) {
        CloseClipboard();
        return "";
    }
    
    int len = WideCharToMultiByte(CP_UTF8, 0, wText, -1, nullptr, 0, nullptr, nullptr);
    std::string result(static_cast<size_t>(len - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wText, -1, &result[0], len, nullptr, nullptr);
    
    GlobalUnlock(hData);
    CloseClipboard();
    return result;
#else
    return "";
#endif
}

bool Clipboard::hasText() {
#ifdef _WIN32
    return IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE;
#else
    return false;
#endif
}

bool Clipboard::clear() {
#ifdef _WIN32
    if (!OpenClipboard(nullptr)) return false;
    EmptyClipboard();
    CloseClipboard();
    return true;
#else
    return false;
#endif
}

// ============================================================================
// Display — تنفيذ معلومات الشاشة
// ============================================================================

DisplayInfo Display::getPrimaryDisplay() {
    DisplayInfo info;
#ifdef _WIN32
    info.width = GetSystemMetrics(SM_CXSCREEN);
    info.height = GetSystemMetrics(SM_CYSCREEN);
    
    HDC hdc = GetDC(nullptr);
    if (hdc) {
        info.dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        info.scaleFactor = static_cast<double>(info.dpi) / 96.0;
        ReleaseDC(nullptr, hdc);
    }
#endif
    return info;
}

int Display::getDisplayCount() {
#ifdef _WIN32
    return GetSystemMetrics(SM_CMONITORS);
#else
    return 1;
#endif
}

// ============================================================================
// دوال المفسر المدمجة / Interpreter Built-in Functions
// ============================================================================

// (AR) كائن صينية مشترك يُستخدم من الدوال المدمجة
static std::unique_ptr<SystemTray> g_tray;

Data::Value tray_create(const std::vector<Data::Value>& args) {
    std::string tooltip = "لغة ص";
    if (!args.empty() && args[0].getType() == Data::ValueType::STRING) {
        tooltip = args[0].getString();
    }
    if (!g_tray) g_tray = std::make_unique<SystemTray>();
    return Data::Value(g_tray->create(tooltip));
}

Data::Value tray_remove(const std::vector<Data::Value>& args) {
    (void)args;
    if (g_tray) g_tray->remove();
    return Data::Value();
}

Data::Value tray_tooltip(const std::vector<Data::Value>& args) {
    if (!args.empty() && args[0].getType() == Data::ValueType::STRING && g_tray) {
        g_tray->setTooltip(args[0].getString());
    }
    return Data::Value();
}

Data::Value tray_notify(const std::vector<Data::Value>& args) {
    if (args.size() >= 2 && g_tray) {
        std::string title = args[0].getType() == Data::ValueType::STRING ? args[0].getString() : "";
        std::string msg = args[1].getType() == Data::ValueType::STRING ? args[1].getString() : "";
        int timeout = args.size() >= 3 ? static_cast<int>(args[2].getNumber()) : 3000;
        g_tray->showBalloon(title, msg, timeout);
    }
    return Data::Value();
}

Data::Value notify_send(const std::vector<Data::Value>& args) {
    if (args.size() >= 2) {
        std::string title = args[0].getType() == Data::ValueType::STRING ? args[0].getString() : "";
        std::string msg = args[1].getType() == Data::ValueType::STRING ? args[1].getString() : "";
        return Data::Value(NotificationManager::send(title, msg));
    }
    return Data::Value(false);
}

Data::Value clipboard_copy(const std::vector<Data::Value>& args) {
    if (!args.empty() && args[0].getType() == Data::ValueType::STRING) {
        return Data::Value(Clipboard::copyText(args[0].getString()));
    }
    return Data::Value(false);
}

Data::Value clipboard_paste(const std::vector<Data::Value>& args) {
    (void)args;
    return Data::Value(Clipboard::pasteText());
}

Data::Value clipboard_has_text(const std::vector<Data::Value>& args) {
    (void)args;
    return Data::Value(Clipboard::hasText());
}

Data::Value clipboard_clear(const std::vector<Data::Value>& args) {
    (void)args;
    return Data::Value(Clipboard::clear());
}

Data::Value display_info(const std::vector<Data::Value>& args) {
    (void)args;
    auto info = Display::getPrimaryDisplay();
    // (AR) إرجاع كنص وصفي — يمكن التوسع لإرجاع كائن
    return Data::Value(
        std::to_string(info.width) + "x" + std::to_string(info.height) + 
        " DPI:" + std::to_string(info.dpi) +
        " Scale:" + std::to_string(info.scaleFactor)
    );
}

Data::Value display_count(const std::vector<Data::Value>& args) {
    (void)args;
    return Data::Value(static_cast<double>(Display::getDisplayCount()));
}

} // namespace Desktop
} // namespace StdLib
} // namespace Sad
