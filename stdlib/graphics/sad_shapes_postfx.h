// ═══════════════════════════════════════════════════════════════════════════════
// sad_shapes_postfx.h — ربط الأشكال المتقدمة والمعالجة البعدية بلغة ص
// Advanced Shapes & Post-Processing Binding for SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// الأشكال المتقدمة / Advanced Shapes
// ═══════════════════════════════════════════════════════════════

/// رسم منحنى بيزيه تربيعي / Draw quadratic Bezier curve
void shape_bezier_quad_impl(int rendererId, float x0, float y0, float x1, float y1,
                            float x2, float y2, int r, int g, int b, float thickness);

/// رسم منحنى بيزيه تكعيبي / Draw cubic Bezier curve
void shape_bezier_cubic_impl(int rendererId, float x0, float y0, float x1, float y1,
                             float x2, float y2, float x3, float y3,
                             int r, int g, int b, float thickness);

/// رسم مضلع منتظم / Draw regular polygon
void shape_polygon_impl(int rendererId, float cx, float cy, float radius, int sides,
                        int r, int g, int b, bool filled);

/// رسم نجمة / Draw star
void shape_star_impl(int rendererId, float cx, float cy, float outerR, float innerR,
                     int points, int r, int g, int b, bool filled);

/// رسم سهم / Draw arrow
void shape_arrow_impl(int rendererId, float x0, float y0, float x1, float y1,
                      float headSize, int r, int g, int b);

/// رسم قوس / Draw arc
void shape_arc_impl(int rendererId, float cx, float cy, float radius,
                    float startAngle, float endAngle, int r, int g, int b, float thickness);

/// رسم فطيرة / Draw pie
void shape_pie_impl(int rendererId, float cx, float cy, float radius,
                    float startAngle, float endAngle, int r, int g, int b, bool filled);

/// رسم حلقة / Draw ring
void shape_ring_impl(int rendererId, float cx, float cy, float innerR, float outerR,
                     int r, int g, int b);

/// رسم شبكة / Draw grid
void shape_grid_impl(int rendererId, float x, float y, float w, float h,
                     int cols, int rows, int r, int g, int b);

/// رسم قلب / Draw heart
void shape_heart_impl(int rendererId, float cx, float cy, float size,
                      int r, int g, int b, bool filled);

/// رسم علامة صح / Draw checkmark
void shape_checkmark_impl(int rendererId, float x, float y, float size,
                          int r, int g, int b, float thickness);

/// رسم X / Draw X mark
void shape_x_impl(int rendererId, float x, float y, float size,
                  int r, int g, int b, float thickness);

// ═══════════════════════════════════════════════════════════════
// المعالجة البعدية / Post-Processing Effects
// ═══════════════════════════════════════════════════════════════

/// إنشاء سلسلة تأثيرات / Create effect chain
int postfx_create_chain_impl();

/// تدمير سلسلة تأثيرات / Destroy effect chain
void postfx_destroy_chain_impl(int chainId);

/// إضافة تأثير توهج / Add bloom effect
int postfx_add_bloom_impl(int chainId, float threshold, float intensity, int passes);

/// إضافة تأثير ضبابية / Add blur effect
int postfx_add_blur_impl(int chainId, float radius, int samples);

/// إضافة تأثير فينييت / Add vignette effect
int postfx_add_vignette_impl(int chainId, float intensity, float radius);

/// إضافة تأثير تدرج ألوان / Add color grading effect
int postfx_add_color_grading_impl(int chainId, float brightness, float contrast, float saturation);

/// إضافة تأثير انحراف لوني / Add chromatic aberration effect
int postfx_add_chromatic_impl(int chainId, float intensity);

/// إضافة تأثير CRT / Add CRT effect
int postfx_add_crt_impl(int chainId, float curvature, float scanlines);

/// إضافة تأثير تدرج رمادي / Add grayscale effect
int postfx_add_grayscale_impl(int chainId);

/// إضافة تأثير سيبيا / Add sepia effect
int postfx_add_sepia_impl(int chainId);

/// إضافة تأثير عكس الألوان / Add invert effect
int postfx_add_invert_impl(int chainId);

/// إضافة تأثير تقطيع / Add pixelate effect
int postfx_add_pixelate_impl(int chainId, float pixelSize);

/// تفعيل/تعطيل تأثير / Enable/disable effect
void postfx_set_enabled_impl(int chainId, int effectIdx, bool enabled);

/// تعيين شدة التأثير / Set effect intensity
void postfx_set_intensity_impl(int chainId, int effectIdx, float intensity);

/// عدد التأثيرات / Get effect count
int postfx_effect_count_impl(int chainId);

/// إزالة تأثير / Remove effect
void postfx_remove_impl(int chainId, int effectIdx);

/// مسح السلسلة / Clear all effects
void postfx_clear_impl(int chainId);

} // namespace graphics
} // namespace stdlib
} // namespace sad
