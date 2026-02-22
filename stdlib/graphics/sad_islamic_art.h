// ═══════════════════════════════════════════════════════════════════════════════
// نظام الزخارف الإسلامية — لغة ص
// Islamic Geometric Art System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// رسم الأنماط الهندسية الإسلامية حسابياً — بدون صور!
// لا يوجد نظير لهذا في Flutter أو أي إطار عمل آخر!
//
// الميزات:
//   ⭐ أنماط نجمية (6، 8، 10، 12 رؤوس)
//   🔷 فسيفساء هندسية متكررة (Tessellation)
//   🌿 زخارف نباتية (Arabesque)
//   🖼️ إطارات زخرفية وحدود
//   🌈 أنظمة ألوان إسلامية تقليدية
//   📐 تحكم كامل بالحجم والتعقيد واللون
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace sad { namespace islamic_art {

// ═══════════════════════════════════════════════════════════════════
// الألوان الإسلامية الكلاسيكية
// ═══════════════════════════════════════════════════════════════════

struct ArtColor {
    uint8_t r, g, b, a;
};

// أنظمة الألوان المحددة مسبقاً
enum class ColorScheme {
    OttomanBlue,    // أزرق عثماني — أزرق كوبالت، ذهبي، أبيض
    MoorishGreen,   // أخضر أندلسي — أخضر زمردي، أبيض، ذهبي
    PersianRose,    // وردي فارسي — وردي، فيروزي، ذهبي
    MarrakechGold,  // ذهبي مراكشي — ذهبي، بني، كريمي
    IznikCeramic,   // سيراميك إزنيك — أبيض، أزرق كوبالت، أحمر
    MamlukRed,      // أحمر مملوكي — أحمر، ذهبي، أسود
    Custom          // مخصص — يحدده المستخدم
};

// ═══════════════════════════════════════════════════════════════════
// أنواع الأنماط
// ═══════════════════════════════════════════════════════════════════

// نوع النمط النجمي
enum class StarPattern {
    SixPointed,     // نجمة سداسية (نجمة داود / المسدس)
    EightPointed,   // نجمة ثمانية (الأكثر شيوعاً في الزخارف الإسلامية)
    TenPointed,     // نجمة عشارية
    TwelvePointed   // نجمة اثنا عشرية  
};

// نوع الفسيفساء
enum class TessellationType {
    Hexagonal,      // فسيفساء سداسية
    Octagonal,      // فسيفساء ثمانية
    Girih,          // أنماط كريه (5 أشكال أساسية)
    Muqarnas        // مقرنصات (تقريب ثنائي الأبعاد)
};

// نوع الإطار الزخرفي
enum class BorderType {
    GeometricBand,  // شريط هندسي متكرر
    Interlaced,     // شريط متشابك
    Kufic,          // كتابة كوفية مزخرفة مبسطة
    Floral          // زخرفة نباتية
};

// ═══════════════════════════════════════════════════════════════════
// إعدادات الرسم
// ═══════════════════════════════════════════════════════════════════

struct PatternConfig {
    int x, y;           // موقع البداية
    int width, height;  // حجم المنطقة
    int complexity;     // مستوى التعقيد (1-5)
    int lineWidth;      // سمك الخطوط
    ColorScheme scheme; // نظام الألوان
    
    // ألوان مخصصة (تُستخدم عند scheme == Custom)
    ArtColor primaryColor;
    ArtColor secondaryColor;
    ArtColor backgroundColor;
    ArtColor accentColor;
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── الأنماط النجمية ───

// رسم نمط نجمي
void drawStarPattern(void* renderer, StarPattern type,
                     int cx, int cy, int radius,
                     ColorScheme scheme = ColorScheme::OttomanBlue,
                     int lineWidth = 2);

// رسم شبكة من الأنماط النجمية (فسيفساء)
void drawStarGrid(void* renderer, StarPattern type,
                  int x, int y, int width, int height,
                  int cellSize, ColorScheme scheme = ColorScheme::OttomanBlue,
                  int lineWidth = 2);

// ─── الفسيفساء ───

// رسم فسيفساء هندسية
void drawTessellation(void* renderer, TessellationType type,
                      int x, int y, int width, int height,
                      int cellSize, ColorScheme scheme = ColorScheme::MoorishGreen,
                      int lineWidth = 2);

// ─── الإطارات والحدود ───

// رسم إطار زخرفي حول منطقة
void drawBorder(void* renderer, BorderType type,
                int x, int y, int width, int height,
                int borderWidth = 20,
                ColorScheme scheme = ColorScheme::MarrakechGold,
                int lineWidth = 2);

// ─── دوال مساعدة ───

// الحصول على ألوان نظام لون معين
void getSchemeColors(ColorScheme scheme,
                     ArtColor& primary, ArtColor& secondary,
                     ArtColor& background, ArtColor& accent);

// رسم نجمة واحدة (للاستخدام المفرد)
void drawSingleStar(void* renderer, int points, 
                    int cx, int cy, int outerR, int innerR,
                    ArtColor fillColor, ArtColor lineColor,
                    int lineWidth = 2);

// رسم زخرفة نباتية مبسطة (أرابيسك)
void drawArabesque(void* renderer,
                   int x, int y, int width, int height,
                   ColorScheme scheme = ColorScheme::PersianRose,
                   int complexity = 3);

// رسم خلفية مزخرفة كاملة
void drawIslamicBackground(void* renderer,
                           int width, int height,
                           ColorScheme scheme = ColorScheme::OttomanBlue,
                           int patternSize = 60);

}} // namespace sad::islamic_art
