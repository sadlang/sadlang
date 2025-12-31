// ============================================================================
// types.cpp - تطبيق الأنواع الأساسية
// Core Types Implementation
// ============================================================================

#include "core/types.h"

namespace sad {
namespace graphics {

// ============================================================================
// تعريف الألوان المعرفة مسبقاً / Predefined colors definition
// ============================================================================

const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);       // أسود / Black
const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);       // أبيض / White
const Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);         // أحمر / Red
const Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);       // أخضر / Green
const Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);        // أزرق / Blue
const Color Color::Yellow(1.0f, 1.0f, 0.0f, 1.0f);      // أصفر / Yellow
const Color Color::Cyan(0.0f, 1.0f, 1.0f, 1.0f);        // سماوي / Cyan
const Color Color::Magenta(1.0f, 0.0f, 1.0f, 1.0f);     // أرجواني / Magenta
const Color Color::Transparent(0.0f, 0.0f, 0.0f, 0.0f); // شفاف / Transparent

} // namespace graphics
} // namespace sad
