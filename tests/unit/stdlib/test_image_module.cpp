// بسم الله الرحمن الرحيم
/**
 * @file test_image_module.cpp
 * @brief Comprehensive Tests for Image Processing Module
 *        اختبارات شاملة لوحدة معالجة الصور
 *
 * Tests cover: Color, Rectangle, Point, Image (create, pixel ops,
 *              transforms, filters, color adjustments), format helpers
 *
 * @author ريم — مهندسة المكتبات والأدوات
 * @date March 2026
 * الحمد لله رب العالمين
 */

#include <gtest/gtest.h>
#include "image/image_module.h"

using namespace sad::stdlib::image;

// ============================================================================
// 1. Color Tests
//    اختبارات الألوان
// ============================================================================

TEST(ColorTest, DefaultConstruction) {
    Color c;
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
    EXPECT_EQ(c.a, 255);
}

TEST(ColorTest, RGBAConstruction) {
    Color c{128, 64, 32, 200};
    EXPECT_EQ(c.r, 128);
    EXPECT_EQ(c.g, 64);
    EXPECT_EQ(c.b, 32);
    EXPECT_EQ(c.a, 200);
}

TEST(ColorTest, StaticFactories) {
    Color black = Color::Black();
    EXPECT_EQ(black.r, 0);
    EXPECT_EQ(black.g, 0);
    EXPECT_EQ(black.b, 0);

    Color white = Color::White();
    EXPECT_EQ(white.r, 255);
    EXPECT_EQ(white.g, 255);
    EXPECT_EQ(white.b, 255);

    Color red = Color::Red();
    EXPECT_EQ(red.r, 255);
    EXPECT_EQ(red.g, 0);
    EXPECT_EQ(red.b, 0);

    Color green = Color::Green();
    EXPECT_EQ(green.g, 255);

    Color blue = Color::Blue();
    EXPECT_EQ(blue.b, 255);

    Color transparent = Color::Transparent();
    EXPECT_EQ(transparent.a, 0);
}

TEST(ColorTest, YellowCyanMagenta) {
    Color yellow = Color::Yellow();
    EXPECT_EQ(yellow.r, 255);
    EXPECT_EQ(yellow.g, 255);
    EXPECT_EQ(yellow.b, 0);

    Color cyan = Color::Cyan();
    EXPECT_EQ(cyan.r, 0);
    EXPECT_EQ(cyan.g, 255);
    EXPECT_EQ(cyan.b, 255);

    Color magenta = Color::Magenta();
    EXPECT_EQ(magenta.r, 255);
    EXPECT_EQ(magenta.g, 0);
    EXPECT_EQ(magenta.b, 255);
}

// ============================================================================
// 2. Rectangle & Point Tests
//    اختبارات المستطيل والنقطة
// ============================================================================

TEST(RectangleTest, Construction) {
    Rectangle r{10, 20, 100, 50};
    EXPECT_EQ(r.x, 10);
    EXPECT_EQ(r.y, 20);
    EXPECT_EQ(r.width, 100);
    EXPECT_EQ(r.height, 50);
}

TEST(RectangleTest, Contains) {
    Rectangle r{0, 0, 100, 100};
    EXPECT_TRUE(r.contains(50, 50));
    EXPECT_TRUE(r.contains(0, 0));
    EXPECT_TRUE(r.contains(99, 99));
    EXPECT_FALSE(r.contains(100, 100));
    EXPECT_FALSE(r.contains(-1, 0));
    EXPECT_FALSE(r.contains(0, -1));
}

TEST(PointTest, Construction) {
    Point p{15, 25};
    EXPECT_EQ(p.x, 15);
    EXPECT_EQ(p.y, 25);
}

// ============================================================================
// 3. Image Format & Enum Tests
//    اختبارات صيغ الصور والتعدادات
// ============================================================================

TEST(ImageEnumsTest, ImageFormatValues) {
    EXPECT_NE(ImageFormat::PNG, ImageFormat::JPEG);
    EXPECT_NE(ImageFormat::JPEG, ImageFormat::BMP);
    EXPECT_NE(ImageFormat::BMP, ImageFormat::UNKNOWN);
}

TEST(ImageEnumsTest, ColorSpaceValues) {
    EXPECT_NE(ColorSpace::RGB, ColorSpace::RGBA);
    EXPECT_NE(ColorSpace::RGB, ColorSpace::GRAYSCALE);
    EXPECT_NE(ColorSpace::BGR, ColorSpace::BGRA);
}

TEST(ImageEnumsTest, InterpolationMethodValues) {
    EXPECT_NE(InterpolationMethod::NEAREST, InterpolationMethod::LINEAR);
    EXPECT_NE(InterpolationMethod::LINEAR, InterpolationMethod::CUBIC);
    EXPECT_NE(InterpolationMethod::CUBIC, InterpolationMethod::LANCZOS);
}

TEST(ImageEnumsTest, FlipDirectionValues) {
    EXPECT_NE(FlipDirection::HORIZONTAL, FlipDirection::VERTICAL);
    EXPECT_NE(FlipDirection::VERTICAL, FlipDirection::BOTH);
}

TEST(ImageEnumsTest, FilterTypeValues) {
    EXPECT_NE(FilterType::BLUR, FilterType::SHARPEN);
    EXPECT_NE(FilterType::GAUSSIAN_BLUR, FilterType::EDGE_DETECT);
}

TEST(ImageEnumsTest, FormatToString) {
    EXPECT_EQ(format_to_string(ImageFormat::PNG), "PNG");
    EXPECT_EQ(format_to_string(ImageFormat::JPEG), "JPEG");
    EXPECT_EQ(format_to_string(ImageFormat::BMP), "BMP");
}

TEST(ImageEnumsTest, ColorSpaceToString) {
    std::string rgb = color_space_to_string(ColorSpace::RGB);
    EXPECT_FALSE(rgb.empty());
}

// ============================================================================
// 4. Image Construction Tests
//    اختبارات إنشاء الصور
// ============================================================================

TEST(ImageTest, EmptyConstruction) {
    Image img;
    EXPECT_TRUE(img.empty());
    EXPECT_EQ(img.width(), 0u);
    EXPECT_EQ(img.height(), 0u);
}

TEST(ImageTest, SizedConstruction) {
    Image img(100, 50, ColorSpace::RGB);
    EXPECT_FALSE(img.empty());
    EXPECT_EQ(img.width(), 100u);
    EXPECT_EQ(img.height(), 50u);
    EXPECT_EQ(img.color_space(), ColorSpace::RGB);
    EXPECT_EQ(img.channels(), 3u);
}

TEST(ImageTest, RGBAConstruction) {
    Image img(64, 64, ColorSpace::RGBA);
    EXPECT_EQ(img.channels(), 4u);
    EXPECT_EQ(img.size(), 64u * 64u * 4u);
}

TEST(ImageTest, GrayscaleConstruction) {
    Image img(32, 32, ColorSpace::GRAYSCALE);
    EXPECT_EQ(img.channels(), 1u);
}

// ============================================================================
// 5. Pixel Operations Tests
//    اختبارات عمليات البكسل
// ============================================================================

TEST(ImageTest, SetAndGetPixel) {
    Image img(10, 10, ColorSpace::RGBA);
    Color red = Color::Red();

    img.set_pixel(5, 5, red);
    Color got = img.get_pixel(5, 5);
    EXPECT_EQ(got.r, 255);
    EXPECT_EQ(got.g, 0);
    EXPECT_EQ(got.b, 0);
    EXPECT_EQ(got.a, 255);
}

TEST(ImageTest, FillImage) {
    Image img(10, 10, ColorSpace::RGB);
    Color blue = Color::Blue();
    img.fill(blue);

    // Check a few pixels
    Color p1 = img.get_pixel(0, 0);
    EXPECT_EQ(p1.b, 255);
    EXPECT_EQ(p1.r, 0);

    Color p2 = img.get_pixel(9, 9);
    EXPECT_EQ(p2.b, 255);
}

TEST(ImageTest, DataPointer) {
    Image img(10, 10, ColorSpace::RGB);
    EXPECT_NE(img.data(), nullptr);
}

// ============================================================================
// 6. Image Transform Tests
//    اختبارات تحويلات الصور
// ============================================================================

TEST(ImageTest, Resize) {
    Image img(100, 100, ColorSpace::RGB);
    img.fill(Color::White());

    Image resized = img.resize(50, 50);
    EXPECT_EQ(resized.width(), 50u);
    EXPECT_EQ(resized.height(), 50u);
}

TEST(ImageTest, Crop) {
    Image img(100, 100, ColorSpace::RGB);
    img.fill(Color::Green());

    Image cropped = img.crop(10, 10, 50, 50);
    EXPECT_EQ(cropped.width(), 50u);
    EXPECT_EQ(cropped.height(), 50u);
}

TEST(ImageTest, CropWithRectangle) {
    Image img(100, 100, ColorSpace::RGB);
    Rectangle rect{0, 0, 30, 30};

    Image cropped = img.crop(rect);
    EXPECT_EQ(cropped.width(), 30u);
    EXPECT_EQ(cropped.height(), 30u);
}

TEST(ImageTest, Flip) {
    Image img(10, 10, ColorSpace::RGB);
    img.set_pixel(0, 0, Color::Red());

    Image flipped = img.flip(FlipDirection::HORIZONTAL);
    EXPECT_EQ(flipped.width(), 10u);
    EXPECT_EQ(flipped.height(), 10u);
    // After horizontal flip, (0,0) should now be at (9,0)
    Color c = flipped.get_pixel(9, 0);
    EXPECT_EQ(c.r, 255);
}

// ============================================================================
// 7. Color Operations Tests
//    اختبارات عمليات الألوان
// ============================================================================

TEST(ImageTest, Grayscale) {
    Image img(10, 10, ColorSpace::RGB);
    img.fill(Color::Red());

    Image gray = img.grayscale();
    EXPECT_EQ(gray.color_space(), ColorSpace::GRAYSCALE);
    EXPECT_EQ(gray.channels(), 1u);
}

TEST(ImageTest, Invert) {
    Image img(10, 10, ColorSpace::RGB);
    img.fill(Color{100, 150, 200, 255});

    Image inverted = img.invert();
    Color p = inverted.get_pixel(0, 0);
    EXPECT_EQ(p.r, 155);
    EXPECT_EQ(p.g, 105);
    EXPECT_EQ(p.b, 55);
}

TEST(ImageTest, AdjustBrightness) {
    Image img(10, 10, ColorSpace::RGB);
    img.fill(Color{100, 100, 100, 255});

    Image brighter = img.adjust_brightness(50);
    Color p = brighter.get_pixel(0, 0);
    EXPECT_EQ(p.r, 150);
}

TEST(ImageTest, Clone) {
    Image img(10, 10, ColorSpace::RGB);
    img.fill(Color::Red());

    Image cloned = img.clone();
    EXPECT_EQ(cloned.width(), img.width());
    EXPECT_EQ(cloned.height(), img.height());

    Color p = cloned.get_pixel(0, 0);
    EXPECT_EQ(p.r, 255);
}

// ============================================================================
// 8. Image Statistics Tests
//    اختبارات إحصائيات الصورة
// ============================================================================

TEST(ImageTest, AverageColor) {
    Image img(10, 10, ColorSpace::RGB);
    img.fill(Color{100, 100, 100, 255});

    Color avg = img.average_color();
    EXPECT_EQ(avg.r, 100);
    EXPECT_EQ(avg.g, 100);
    EXPECT_EQ(avg.b, 100);
}

TEST(ImageTest, Brightness) {
    Image img(10, 10, ColorSpace::RGB);
    img.fill(Color::White());
    double b = img.brightness();
    EXPECT_NEAR(b, 1.0, 0.01);

    Image dark(10, 10, ColorSpace::RGB);
    dark.fill(Color::Black());
    double d = dark.brightness();
    EXPECT_NEAR(d, 0.0, 0.01);
}

// ============================================================================
// 9. Edge Cases
//    حالات الحافة
// ============================================================================

TEST(ImageEdgeCaseTest, ZeroSizeImage) {
    Image img;
    EXPECT_TRUE(img.empty());
    EXPECT_EQ(img.width(), 0u);
    EXPECT_EQ(img.height(), 0u);
}

TEST(ImageEdgeCaseTest, SinglePixelImage) {
    Image img(1, 1, ColorSpace::RGBA);
    img.set_pixel(0, 0, Color::Red());
    Color p = img.get_pixel(0, 0);
    EXPECT_EQ(p.r, 255);
}

TEST(ImageEdgeCaseTest, SimilarImages) {
    Image img1(10, 10, ColorSpace::RGB);
    Image img2(10, 10, ColorSpace::RGB);
    img1.fill(Color::Red());
    img2.fill(Color::Red());
    EXPECT_TRUE(img1.is_similar(img2, 0.01));
}

TEST(ImageEdgeCaseTest, DifferentImages) {
    Image img1(10, 10, ColorSpace::RGB);
    Image img2(10, 10, ColorSpace::RGB);
    img1.fill(Color::Red());
    img2.fill(Color::Blue());
    EXPECT_FALSE(img1.is_similar(img2, 0.01));
}

TEST(ImageEdgeCaseTest, InterpolateColor) {
    Color c = interpolate_color(Color::Black(), Color::White(), 0.5);
    // Midpoint should be gray
    EXPECT_NEAR(c.r, 128, 2);
    EXPECT_NEAR(c.g, 128, 2);
    EXPECT_NEAR(c.b, 128, 2);
}
