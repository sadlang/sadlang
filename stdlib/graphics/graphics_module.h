// بسم الله الرحمن الرحيم
/**
 * @file graphics_module.h
 * @brief Graphics Module for Sad Language - وحدة الرسومات للغة ص
 * 
 * توفر دوال شاملة للرسومات وواجهات المستخدم
 * Provides comprehensive functions for graphics and UI
 * 
 * Categories / الفئات:
 * - Window Management (create, show, hide, close)
 * - Rendering (2D drawing, shapes, text, images)
 * - UI Widgets (Label, Button, TextInput, Checkbox, Panel, Slider)
 * - Event Handling (mouse, keyboard, window events)
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include "value.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace sad {
namespace stdlib {
namespace graphics {

// Note: All functions work with Sad::Data::Value directly
// Graphics objects are stored internally with integer IDs

// ============================================================================
// Window Management - إدارة النوافذ
// ============================================================================

/**
 * @brief Create new window - إنشاء نافذة جديدة
 * @return Window ID wrapped in Value / معرف النافذة
 */
int window_create_impl(const std::string& title, int width, int height);

/**
 * @brief Show window - عرض النافذة
 */
void window_show_impl(int windowId);

/**
 * @brief Hide window - إخفاء النافذة
 */
void window_hide_impl(int windowId);

/**
 * @brief Close window - إغلاق النافذة
 */
void window_close_impl(int windowId);

/**
 * @brief Check if window should close - فحص إذا يجب إغلاق النافذة
 */
bool window_should_close_impl(int windowId);

/**
 * @brief Poll window events - استقبال أحداث النافذة
 */
void window_poll_events_impl(int windowId);

/**
 * @brief Swap window buffers - تبديل buffers النافذة
 */
void window_swap_buffers_impl(int windowId);

// ============================================================================
// Renderer Management - إدارة الرسام
// ============================================================================

/**
 * @brief Create 2D renderer - إنشاء رسام ثنائي الأبعاد
 */
int renderer_create_impl(int windowId);

/**
 * @brief Begin rendering frame - بدء إطار الرسم
 */
void renderer_begin_frame_impl(int rendererId);

/**
 * @brief End rendering frame - إنهاء إطار الرسم
 */
void renderer_end_frame_impl(int rendererId);

/**
 * @brief Clear screen with color - مسح الشاشة بلون
 */
void renderer_clear_impl(int rendererId, int r, int g, int b, int a);

/**
 * @brief Draw line - رسم خط
 */
void renderer_draw_line_impl(int rendererId, float x1, float y1, float x2, float y2, 
                             int r, int g, int b, int a);

/**
 * @brief Draw rectangle - رسم مستطيل
 */
void renderer_draw_rect_impl(int rendererId, float x, float y, float width, float height,
                             int r, int g, int b, int a, bool filled);

/**
 * @brief Draw circle - رسم دائرة
 */
void renderer_draw_circle_impl(int rendererId, float x, float y, float radius,
                               int r, int g, int b, int a, bool filled);

/**
 * @brief Draw text - رسم نص
 */
void renderer_draw_text_impl(int rendererId, const std::string& text, float x, float y, 
                             float size, int r, int g, int b, int a);

// ============================================================================
// UI Widget Management - إدارة عناصر الواجهة
// ============================================================================

/**
 * @brief Create label widget - إنشاء تسمية
 */
int label_create_impl(const std::string& text, float x, float y, float width, float height);

/**
 * @brief Set label text - تعيين نص التسمية
 */
void label_set_text_impl(int labelId, const std::string& text);

/**
 * @brief Get label text - الحصول على نص التسمية
 */
std::string label_get_text_impl(int labelId);

/**
 * @brief Create button widget - إنشاء زر
 */
int button_create_impl(const std::string& text, float x, float y, float width, float height);

/**
 * @brief Check if button is clicked - فحص إذا تم النقر على الزر
 */
bool button_is_clicked_impl(int buttonId);

/**
 * @brief Set button text - تعيين نص الزر
 */
void button_set_text_impl(int buttonId, const std::string& text);

/**
 * @brief Create text input widget - إنشاء حقل نص
 */
int textinput_create_impl(const std::string& placeholder, float x, float y, 
                         float width, float height);

/**
 * @brief Get text input value - الحصول على قيمة حقل النص
 */
std::string textinput_get_value_impl(int inputId);

/**
 * @brief Set text input value - تعيين قيمة حقل النص
 */
void textinput_set_value_impl(int inputId, const std::string& value);

/**
 * @brief Create checkbox widget - إنشاء مربع اختيار
 */
int checkbox_create_impl(const std::string& label, float x, float y, 
                        float width, float height);

/**
 * @brief Check if checkbox is checked - فحص إذا كان مربع الاختيار محدداً
 */
bool checkbox_is_checked_impl(int checkboxId);

/**
 * @brief Set checkbox state - تعيين حالة مربع الاختيار
 */
void checkbox_set_checked_impl(int checkboxId, bool checked);

/**
 * @brief Draw widget - رسم عنصر واجهة
 */
void widget_draw_impl(int widgetId, int rendererId);

/**
 * @brief Update widget - تحديث عنصر واجهة
 */
void widget_update_impl(int widgetId, float deltaTime);

/**
 * @brief Handle mouse event for widget - معالجة حدث فأرة لعنصر
 */
void widget_mouse_event_impl(int widgetId, int x, int y, bool pressed);

// ============================================================================
// Input Handling - معالجة المدخلات
// ============================================================================

/**
 * @brief Check if a key is currently pressed - فحص إذا كان مفتاح مضغوطًا
 * @param keycode SDL scancode (e.g., 26='W', 22='S', 4='A', 7='D', etc.)
 */
bool key_is_pressed_impl(int keycode);

/**
 * @brief Get mouse X position - الحصول على موقع الفأرة الأفقي
 */
int mouse_get_x_impl();

/**
 * @brief Get mouse Y position - الحصول على موقع الفأرة العمودي
 */
int mouse_get_y_impl();

/**
 * @brief Check if mouse button is pressed - فحص إذا كان زر الفأرة مضغوطًا
 * @param button 1=left, 2=middle, 3=right
 */
bool mouse_button_pressed_impl(int button);

// ============================================================================
// Texture / Image Management - إدارة الصور والنسيج
// ============================================================================

/**
 * @brief Load texture from file - تحميل نسيج من ملف
 * @return Texture ID or -1 on failure
 */
int texture_load_impl(const std::string& filePath);

/**
 * @brief Draw texture on renderer - رسم نسيج على الرسام
 */
void texture_draw_impl(int rendererId, int textureId, float x, float y, float width, float height);

/**
 * @brief Unload texture - تحرير النسيج
 */
void texture_unload_impl(int textureId);

// ============================================================================
// Audio System - نظام الصوت
// ============================================================================

/**
 * @brief Initialize audio system - تهيئة نظام الصوت
 */
bool audio_init_impl();

/**
 * @brief Load a sound file - تحميل ملف صوتي
 * @return Sound ID or -1 on failure
 */
int sound_load_impl(const std::string& filePath, const std::string& name);

/**
 * @brief Play a loaded sound - تشغيل صوت محمّل
 * @return Channel ID
 */
int sound_play_impl(int soundId);

/**
 * @brief Stop a playing sound - إيقاف صوت
 */
void sound_stop_impl(int channelId);

/**
 * @brief Set sound volume - تعيين مستوى الصوت
 */
void sound_set_volume_impl(int channelId, float volume);

/**
 * @brief Set master volume - تعيين مستوى الصوت الرئيسي
 */
void audio_set_master_volume_impl(float volume);

// ============================================================================
// Drawing Shapes - رسم الأشكال
// ============================================================================

/**
 * @brief Draw a triangle - رسم مثلث
 */
void renderer_draw_triangle_impl(int rendererId, float x1, float y1, 
                                  float x2, float y2, float x3, float y3,
                                  int r, int g, int b, int a, bool filled);

/**
 * @brief Draw a rounded rectangle - رسم مستطيل دائري الحواف
 */
void renderer_draw_rounded_rect_impl(int rendererId, float x, float y, float w, float h,
                                      float radius, int r, int g, int b, int a, bool filled);

// ============================================================================
// Extended Drawing - رسم متقدم
// ============================================================================

void renderer_draw_ellipse_impl(int rendererId, float cx, float cy, float rx, float ry,
                                int r, int g, int b, int a, bool filled);

void renderer_draw_arc_impl(int rendererId, float cx, float cy, float radius,
                            float startAngle, float endAngle, int r, int g, int b, int a);

void renderer_draw_rect_outline_impl(int rendererId, float x, float y, float w, float h,
                                      int fillR, int fillG, int fillB, int fillA,
                                      int outR, int outG, int outB, int outA,
                                      float outlineThickness);

// ============================================================================
// Transformation - التحويلات
// ============================================================================

void renderer_push_transform_impl(int rendererId);
void renderer_pop_transform_impl(int rendererId);
void renderer_translate_impl(int rendererId, float x, float y);
void renderer_rotate_impl(int rendererId, float angle);
void renderer_scale_impl(int rendererId, float sx, float sy);
void renderer_reset_transform_impl(int rendererId);

// ============================================================================
// Renderer Settings - إعدادات الرسام
// ============================================================================

void renderer_set_draw_color_impl(int rendererId, int r, int g, int b, int a);
void renderer_set_line_thickness_impl(int rendererId, float thickness);
void renderer_set_viewport_impl(int rendererId, int x, int y, int w, int h);
void renderer_set_projection_impl(int rendererId, float left, float right, float bottom, float top);

// ============================================================================
// Window Properties - خصائص النافذة
// ============================================================================

/**
 * @brief Get window width - الحصول على عرض النافذة
 */
int window_get_width_impl(int windowId);

/**
 * @brief Get window height - الحصول على ارتفاع النافذة
 */
int window_get_height_impl(int windowId);

/**
 * @brief Set window title - تعيين عنوان النافذة
 */
void window_set_title_impl(int windowId, const std::string& title);

// ============================================================================
// Time Utilities - أدوات الوقت
// ============================================================================

/**
 * @brief Get current tick count (milliseconds since SDL init) - الحصول على الوقت الحالي
 */
int get_ticks_impl();

} // namespace graphics
} // namespace stdlib
} // namespace sad
