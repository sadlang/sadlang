// ════════════════════════════════════════════════════════════════════════════════
// ملف: graphics_module_raylib.cpp
// وحدة الرسومات للغة ص - التكامل مع raylib
// Graphics Module for S Language - Integration with raylib
// ════════════════════════════════════════════════════════════════════════════════
// raylib مكتبة رسومات بسيطة وقوية وموثوقة / raylib is simple, powerful and reliable
// https://www.raylib.com/
// ════════════════════════════════════════════════════════════════════════════════

#include "graphics_module.h"
#include <iostream>
#include <map>
#include <string>
#include <cmath>

// ═══ تضمين raylib / Include raylib ═══
#include "raylib.h"

// ════════════════════════════════════════════════════════════════════════════════
// هياكل البيانات لإدارة الموارد / Data Structures for Resource Management
// ════════════════════════════════════════════════════════════════════════════════

// ══════════════════════════════════════════════════════════════════════════════
// بيانات النافذة / Window Data
// ══════════════════════════════════════════════════════════════════════════════
struct WindowData {
    std::string title;          // عنوان النافذة / Window title
    int width;                  // العرض / Width
    int height;                 // الارتفاع / Height
    bool is_open;               // هل النافذة مفتوحة / Is window open
    Color clear_color;          // لون المسح / Clear color
    
    WindowData() : width(800), height(600), is_open(false), clear_color(RAYWHITE) {}
};

// ══════════════════════════════════════════════════════════════════════════════
// خرائط لتخزين الموارد / Maps to Store Resources
// ══════════════════════════════════════════════════════════════════════════════
static std::map<int, WindowData> g_windows;        // خريطة النوافذ / Windows map
static int g_next_window_id = 1;                   // معرف النافذة التالي / Next window ID
static int g_current_window_id = -1;               // معرف النافذة الحالية / Current window ID

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة للألوان / Helper Functions for Colors
// ════════════════════════════════════════════════════════════════════════════════

// ══════════════════════════════════════════════════════════════════════════════
// تحويل RGB إلى Color / Convert RGB to Color
// ══════════════════════════════════════════════════════════════════════════════
Color RGBToColor(int r, int g, int b, int a = 255) {
    return Color{
        static_cast<unsigned char>(r),
        static_cast<unsigned char>(g),
        static_cast<unsigned char>(b),
        static_cast<unsigned char>(a)
    };
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ دوال النوافذ / Window Functions Implementation
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء نافذة جديدة / Create a new window
// ══════════════════════════════════════════════════════════════════════════════
int window_create_impl(const std::string& title, int width, int height) {
    try {
        // التحقق من عدم وجود نافذة مفتوحة / Check no window already open
        if (g_current_window_id != -1) {
            std::cerr << "⚠ نافذة موجودة مسبقاً / Window already exists" << std::endl;
            return g_current_window_id;
        }
        
        // إنشاء النافذة / Create window
        InitWindow(width, height, title.c_str());
        
        if (!IsWindowReady()) {
            std::cerr << "❌ فشل إنشاء النافذة / Failed to create window" << std::endl;
            return -1;
        }
        
        SetTargetFPS(60);
        
        // حفظ بيانات النافذة / Save window data
        WindowData data;
        data.title = title;
        data.width = width;
        data.height = height;
        data.is_open = true;
        
        int id = g_next_window_id++;
        g_windows[id] = data;
        g_current_window_id = id;
        
        std::cout << "✓ نافذة جديدة / New window: " << width << "x" << height << std::endl;
        return id;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ خطأ / Error: " << e.what() << std::endl;
        return -1;
    }
}

bool window_show_impl(int window_id) {
    return window_id == g_current_window_id && g_windows[window_id].is_open;
}

bool window_hide_impl(int window_id) {
    std::cout << "⚠ raylib لا تدعم الإخفاء / raylib doesn't support hiding" << std::endl;
    return true;
}

bool window_close_impl(int window_id) {
    try {
        if (window_id != g_current_window_id) return false;
        
        CloseWindow();
        g_windows[window_id].is_open = false;
        g_current_window_id = -1;
        
        std::cout << "✓ تم الإغلاق / Closed" << std::endl;
        return true;
    } catch (...) {
        return false;
    }
}

bool window_poll_events_impl(int window_id) {
    if (window_id != g_current_window_id) return false;
    return !WindowShouldClose();
}

bool window_swap_buffers_impl(int window_id) {
    return true; // raylib تفعل هذا تلقائياً / raylib does this automatically
}

int window_get_width_impl(int window_id) {
    return GetScreenWidth();
}

int window_get_height_impl(int window_id) {
    return GetScreenHeight();
}

std::string window_get_title_impl(int window_id) {
    if (g_windows.find(window_id) != g_windows.end()) {
        return g_windows[window_id].title;
    }
    return "";
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ دوال الرسم / Renderer Functions Implementation
// ════════════════════════════════════════════════════════════════════════════════

int renderer_create_impl(int window_id) {
    if (window_id == g_current_window_id) {
        std::cout << "✓ رسام جاهز / Renderer ready" << std::endl;
        return window_id; // في raylib، الرسام مدمج مع النافذة / In raylib, renderer is built-in
    }
    return -1;
}

void renderer_clear_impl(int renderer_id, int r, int g, int b, int a) {
    BeginDrawing();
    ClearBackground(RGBToColor(r, g, b, a));
    EndDrawing();
}

void renderer_draw_line_impl(int renderer_id, int x1, int y1, int x2, int y2, 
                             int r, int g, int b, int a) {
    BeginDrawing();
    DrawLine(x1, y1, x2, y2, RGBToColor(r, g, b, a));
    EndDrawing();
}

void renderer_draw_rect_impl(int renderer_id, int x, int y, int w, int h, 
                             int r, int g, int b, int a, bool filled) {
    BeginDrawing();
    if (filled) {
        DrawRectangle(x, y, w, h, RGBToColor(r, g, b, a));
    } else {
        DrawRectangleLines(x, y, w, h, RGBToColor(r, g, b, a));
    }
    EndDrawing();
}

void renderer_draw_circle_impl(int renderer_id, int x, int y, int radius, 
                               int r, int g, int b, int a, bool filled) {
    BeginDrawing();
    if (filled) {
        DrawCircle(x, y, static_cast<float>(radius), RGBToColor(r, g, b, a));
    } else {
        DrawCircleLines(x, y, static_cast<float>(radius), RGBToColor(r, g, b, a));
    }
    EndDrawing();
}

void renderer_draw_triangle_impl(int renderer_id, int x1, int y1, int x2, int y2, 
                                 int x3, int y3, int r, int g, int b, int a) {
    BeginDrawing();
    DrawTriangle(
        Vector2{static_cast<float>(x1), static_cast<float>(y1)},
        Vector2{static_cast<float>(x2), static_cast<float>(y2)},
        Vector2{static_cast<float>(x3), static_cast<float>(y3)},
        RGBToColor(r, g, b, a)
    );
    EndDrawing();
}

void renderer_draw_text_impl(int renderer_id, const std::string& text, 
                             int x, int y, int size, int r, int g, int b, int a) {
    BeginDrawing();
    DrawText(text.c_str(), x, y, size, RGBToColor(r, g, b, a));
    EndDrawing();
}

void renderer_present_impl(int renderer_id) {
    // لا حاجة - raylib تفعل هذا تلقائياً / Not needed - raylib does this automatically
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال عناصر الواجهة / UI Widget Functions (Simplified)
// ════════════════════════════════════════════════════════════════════════════════

int widget_label_create_impl(const std::string& text, int x, int y) {
    std::cout << "✓ Label: " << text << " at (" << x << "," << y << ")" << std::endl;
    return 1;
}

int widget_button_create_impl(const std::string& text, int x, int y, int w, int h) {
    std::cout << "✓ Button: " << text << std::endl;
    return 1;
}

int widget_textinput_create_impl(const std::string& placeholder, int x, int y, int w, int h) {
    std::cout << "✓ TextInput at (" << x << "," << y << ")" << std::endl;
    return 1;
}

int widget_checkbox_create_impl(const std::string& label, int x, int y, bool checked) {
    std::cout << "✓ Checkbox: " << label << std::endl;
    return 1;
}

void widget_set_text_impl(int widget_id, const std::string& text) {}
std::string widget_get_text_impl(int widget_id) { return ""; }
void widget_set_position_impl(int widget_id, int x, int y) {}
void widget_set_size_impl(int widget_id, int w, int h) {}
void widget_set_visible_impl(int widget_id, bool visible) {}
bool widget_get_visible_impl(int widget_id) { return true; }
void widget_set_enabled_impl(int widget_id, bool enabled) {}
bool widget_get_enabled_impl(int widget_id) { return true; }
bool widget_is_clicked_impl(int widget_id) { return false; }
void widget_set_checked_impl(int widget_id, bool checked) {}
bool widget_get_checked_impl(int widget_id) { return false; }

} // namespace graphics
} // namespace stdlib
} // namespace sad
