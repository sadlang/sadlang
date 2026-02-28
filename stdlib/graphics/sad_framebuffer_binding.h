// ═══════════════════════════════════════════════════════════════════════════════
// sad_framebuffer_binding.h — ربط نظام الإطار المخزني بلغة ص
// OS Framebuffer Binding for SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// الإطار المخزني / Framebuffer (GPU-Free)
// ═══════════════════════════════════════════════════════════════

/// إنشاء إطار مخزني / Create framebuffer
int framebuffer_create_impl(int width, int height);

/// تدمير إطار مخزني / Destroy framebuffer
void framebuffer_destroy_impl(int fbId);

/// الحصول على العرض / Get width
int framebuffer_width_impl(int fbId);

/// الحصول على الارتفاع / Get height
int framebuffer_height_impl(int fbId);

/// مسح بلون / Clear with color
void framebuffer_clear_impl(int fbId, int r, int g, int b);

/// تعيين بكسل / Set pixel
void framebuffer_set_pixel_impl(int fbId, int x, int y, int r, int g, int b);

/// الحصول على لون بكسل / Get pixel color (packed RGB)
int framebuffer_get_pixel_impl(int fbId, int x, int y);

/// رسم خط / Draw line (Bresenham)
void framebuffer_line_impl(int fbId, int x0, int y0, int x1, int y1, int r, int g, int b);

/// رسم مستطيل / Draw rectangle
void framebuffer_rect_impl(int fbId, int x, int y, int w, int h, int r, int g, int b, bool filled);

/// رسم دائرة / Draw circle (Midpoint)
void framebuffer_circle_impl(int fbId, int cx, int cy, int radius, int r, int g, int b, bool filled);

/// تبديل المخازن / Swap buffers (double buffering)
void framebuffer_swap_impl(int fbId);

/// تفعيل التخزين المزدوج / Enable double buffering
void framebuffer_double_buffer_impl(int fbId, bool enable);

/// الحصول على عدد البكسلات / Get total pixel count
int framebuffer_pixel_count_impl(int fbId);

/// ملء منطقة / Fill region
void framebuffer_fill_region_impl(int fbId, int x, int y, int w, int h, int r, int g, int b);

/// نسخ منطقة (blit) / Copy region
void framebuffer_blit_impl(int srcId, int dstId, int sx, int sy, int sw, int sh, int dx, int dy);

} // namespace graphics
} // namespace stdlib
} // namespace sad
