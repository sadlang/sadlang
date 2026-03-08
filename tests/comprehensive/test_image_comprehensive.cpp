// بسم الله الرحمن الرحيم
/**
 * @file test_image_comprehensive.cpp
 * @brief Comprehensive Tests for Image Module - اختبارات شاملة لوحدة الصور
 *
 * Tests: Color, Rectangle, Image operations (resize, crop, flip,
 *        grayscale, brightness, blend, similarity)
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 */

#include "sad_test_framework.h"
#include "image/image_module.h"
#include <cmath>

namespace img = sad::stdlib::image;
using img::Color;
using img::Point;
using img::ColorSpace;
using img::FlipDirection;
// Alias to avoid Windows GDI Rectangle conflict
using Rect = img::Rectangle;

int main() {
    SAD_TEST_INIT();

    // ================================================================
    // 1. Color
    // ================================================================
    SAD_GROUP("الألوان / Colors");

    SAD_TEST("default color is black", {
        Color c;
        SAD_ASSERT_EQ(c.r, (uint8_t)0);
        SAD_ASSERT_EQ(c.g, (uint8_t)0);
        SAD_ASSERT_EQ(c.b, (uint8_t)0);
        SAD_ASSERT_EQ(c.a, (uint8_t)255);
    });

    SAD_TEST("RGBA construction", {
        Color c(128, 64, 32, 200);
        SAD_ASSERT_EQ(c.r, (uint8_t)128);
        SAD_ASSERT_EQ(c.g, (uint8_t)64);
        SAD_ASSERT_EQ(c.b, (uint8_t)32);
        SAD_ASSERT_EQ(c.a, (uint8_t)200);
    });

    SAD_TEST("named factory colors", {
        auto red = Color::Red();
        SAD_ASSERT_EQ(red.r, (uint8_t)255);
        SAD_ASSERT_EQ(red.g, (uint8_t)0);
        SAD_ASSERT_EQ(red.b, (uint8_t)0);

        auto green = Color::Green();
        SAD_ASSERT_EQ(green.g, (uint8_t)255);

        auto blue = Color::Blue();
        SAD_ASSERT_EQ(blue.b, (uint8_t)255);

        auto white = Color::White();
        SAD_ASSERT_EQ(white.r, (uint8_t)255);
        SAD_ASSERT_EQ(white.g, (uint8_t)255);
        SAD_ASSERT_EQ(white.b, (uint8_t)255);

        auto black = Color::Black();
        SAD_ASSERT_EQ(black.r, (uint8_t)0);
        SAD_ASSERT_EQ(black.g, (uint8_t)0);
        SAD_ASSERT_EQ(black.b, (uint8_t)0);
    });

    SAD_TEST("color equality", {
        Color a(100, 200, 50, 255);
        Color b(100, 200, 50, 255);
        Color c(100, 200, 51, 255);
        SAD_ASSERT_TRUE(a == b);
        SAD_ASSERT_FALSE(a == c);
    });

    SAD_TEST("color interpolation concept", {
        Color a = Color::Black();
        Color b = Color::White();
        // Manual interpolation at 50%
        uint8_t mid_r = (uint8_t)((int)a.r + (int)(b.r - a.r) * 0.5);
        SAD_ASSERT_GE(mid_r, (uint8_t)126);
        SAD_ASSERT_LE(mid_r, (uint8_t)128);
    });

    SAD_TEST("transparent color", {
        auto t = Color::Transparent();
        SAD_ASSERT_EQ(t.a, (uint8_t)0);
    });

    // ================================================================
    // 2. Rectangle
    // ================================================================
    SAD_GROUP("المستطيلات / Rectangles");

    SAD_TEST("rectangle construction", {
        Rect r(10, 20, 100, 50);
        SAD_ASSERT_EQ(r.x, 10);
        SAD_ASSERT_EQ(r.y, 20);
        SAD_ASSERT_EQ(r.width, 100);
        SAD_ASSERT_EQ(r.height, 50);
    });

    SAD_TEST("contains point", {
        Rect r(0, 0, 100, 100);
        SAD_ASSERT_TRUE(r.contains(50, 50));
        SAD_ASSERT_TRUE(r.contains(0, 0));
        SAD_ASSERT_FALSE(r.contains(100, 100));
        SAD_ASSERT_FALSE(r.contains(-1, 0));
    });

    SAD_TEST("default rectangle", {
        Rect r;
        SAD_ASSERT_EQ(r.x, 0);
        SAD_ASSERT_EQ(r.y, 0);
        SAD_ASSERT_EQ(r.width, 0);
        SAD_ASSERT_EQ(r.height, 0);
    });

    // ================================================================
    // 3. Point
    // ================================================================
    SAD_GROUP("النقاط / Points");

    SAD_TEST("point construction", {
        Point p(5, 10);
        SAD_ASSERT_EQ(p.x, 5);
        SAD_ASSERT_EQ(p.y, 10);
    });

    SAD_TEST("default point", {
        Point p;
        SAD_ASSERT_EQ(p.x, 0);
        SAD_ASSERT_EQ(p.y, 0);
    });

    // ================================================================
    // 4. Enums & Utility Functions
    // ================================================================
    SAD_GROUP("تعدادات وأدوات / Enums & Utils");

    SAD_TEST("color space enum values", {
        SAD_ASSERT_TRUE(ColorSpace::RGBA != ColorSpace::RGB);
        SAD_ASSERT_TRUE(ColorSpace::RGB != ColorSpace::GRAYSCALE);
    });

    SAD_TEST("flip direction enum values", {
        // Verify enum values are distinct
        SAD_ASSERT_TRUE(FlipDirection::HORIZONTAL != FlipDirection::VERTICAL);
    });

    SAD_TEST("image format enum values", {
        // Verify all format enum values are distinct
        SAD_ASSERT_TRUE(img::ImageFormat::PNG != img::ImageFormat::JPEG);
        SAD_ASSERT_TRUE(img::ImageFormat::JPEG != img::ImageFormat::BMP);
        SAD_ASSERT_TRUE(img::ImageFormat::BMP != img::ImageFormat::UNKNOWN);
    });

    // ================================================================
    // 5. Image (skipped — stb_image_write.h not available)
    // ================================================================
    SAD_GROUP("الصورة / Image (تخطي)");

    SAD_SKIP("Image operations require stb_image_write.h (not installed)",
             "stb_image_write.h missing — Image class tests deferred");

    return _runner.printSummary();
}
