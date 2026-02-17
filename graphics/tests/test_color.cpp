// ==============================================================================
// test_color.cpp - اختبارات نظام الألوان / Color System Tests
// ==============================================================================
#include "catch.hpp"
#include "core/types.h"
#include <cmath>

using namespace sad::graphics;

// ============================================================================
// اختبارات المنشئات / Constructor Tests
// ============================================================================

TEST_CASE("Color default constructor - transparent black", "[color]") {
    Color c;
    REQUIRE(c.r == 0.0f);
    REQUIRE(c.g == 0.0f);
    REQUIRE(c.b == 0.0f);
    REQUIRE(c.a == 0.0f);
}

TEST_CASE("Color RGB constructor - alpha defaults to 1.0", "[color]") {
    Color c(0.5f, 0.6f, 0.7f);
    REQUIRE(c.r == Approx(0.5f));
    REQUIRE(c.g == Approx(0.6f));
    REQUIRE(c.b == Approx(0.7f));
    REQUIRE(c.a == Approx(1.0f));
}

TEST_CASE("Color RGBA constructor", "[color]") {
    Color c(0.1f, 0.2f, 0.3f, 0.4f);
    REQUIRE(c.r == Approx(0.1f));
    REQUIRE(c.g == Approx(0.2f));
    REQUIRE(c.b == Approx(0.3f));
    REQUIRE(c.a == Approx(0.4f));
}

// ============================================================================
// اختبارات FromBytes / FromBytes Tests
// ============================================================================

TEST_CASE("Color::FromBytes - basic conversion", "[color][frombytes]") {
    Color c = Color::FromBytes(255, 128, 0);
    REQUIRE(c.r == Approx(1.0f));
    REQUIRE(c.g == Approx(128.0f / 255.0f));
    REQUIRE(c.b == Approx(0.0f));
    REQUIRE(c.a == Approx(1.0f));  // default alpha = 255
}

TEST_CASE("Color::FromBytes - with alpha", "[color][frombytes]") {
    Color c = Color::FromBytes(255, 0, 0, 128);
    REQUIRE(c.r == Approx(1.0f));
    REQUIRE(c.g == Approx(0.0f));
    REQUIRE(c.b == Approx(0.0f));
    REQUIRE(c.a == Approx(128.0f / 255.0f));
}

TEST_CASE("Color::FromBytes - black", "[color][frombytes]") {
    Color c = Color::FromBytes(0, 0, 0);
    REQUIRE(c.r == Approx(0.0f));
    REQUIRE(c.g == Approx(0.0f));
    REQUIRE(c.b == Approx(0.0f));
    REQUIRE(c.a == Approx(1.0f));
}

TEST_CASE("Color::FromBytes - white", "[color][frombytes]") {
    Color c = Color::FromBytes(255, 255, 255);
    REQUIRE(c.r == Approx(1.0f));
    REQUIRE(c.g == Approx(1.0f));
    REQUIRE(c.b == Approx(1.0f));
    REQUIRE(c.a == Approx(1.0f));
}

// ============================================================================
// اختبارات الألوان المحددة مسبقاً / Predefined Color Tests
// ============================================================================

TEST_CASE("Predefined Color::Black", "[color][predefined]") {
    REQUIRE(Color::Black.r == Approx(0.0f));
    REQUIRE(Color::Black.g == Approx(0.0f));
    REQUIRE(Color::Black.b == Approx(0.0f));
    REQUIRE(Color::Black.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::White", "[color][predefined]") {
    REQUIRE(Color::White.r == Approx(1.0f));
    REQUIRE(Color::White.g == Approx(1.0f));
    REQUIRE(Color::White.b == Approx(1.0f));
    REQUIRE(Color::White.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::Red", "[color][predefined]") {
    REQUIRE(Color::Red.r == Approx(1.0f));
    REQUIRE(Color::Red.g == Approx(0.0f));
    REQUIRE(Color::Red.b == Approx(0.0f));
    REQUIRE(Color::Red.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::Green", "[color][predefined]") {
    REQUIRE(Color::Green.r == Approx(0.0f));
    REQUIRE(Color::Green.g == Approx(1.0f));
    REQUIRE(Color::Green.b == Approx(0.0f));
    REQUIRE(Color::Green.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::Blue", "[color][predefined]") {
    REQUIRE(Color::Blue.r == Approx(0.0f));
    REQUIRE(Color::Blue.g == Approx(0.0f));
    REQUIRE(Color::Blue.b == Approx(1.0f));
    REQUIRE(Color::Blue.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::Yellow", "[color][predefined]") {
    REQUIRE(Color::Yellow.r == Approx(1.0f));
    REQUIRE(Color::Yellow.g == Approx(1.0f));
    REQUIRE(Color::Yellow.b == Approx(0.0f));
    REQUIRE(Color::Yellow.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::Cyan", "[color][predefined]") {
    REQUIRE(Color::Cyan.r == Approx(0.0f));
    REQUIRE(Color::Cyan.g == Approx(1.0f));
    REQUIRE(Color::Cyan.b == Approx(1.0f));
    REQUIRE(Color::Cyan.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::Magenta", "[color][predefined]") {
    REQUIRE(Color::Magenta.r == Approx(1.0f));
    REQUIRE(Color::Magenta.g == Approx(0.0f));
    REQUIRE(Color::Magenta.b == Approx(1.0f));
    REQUIRE(Color::Magenta.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::Transparent", "[color][predefined]") {
    REQUIRE(Color::Transparent.r == Approx(0.0f));
    REQUIRE(Color::Transparent.g == Approx(0.0f));
    REQUIRE(Color::Transparent.b == Approx(0.0f));
    REQUIRE(Color::Transparent.a == Approx(0.0f));
}

TEST_CASE("Predefined Color::Gray", "[color][predefined]") {
    REQUIRE(Color::Gray.r == Approx(0.5f));
    REQUIRE(Color::Gray.g == Approx(0.5f));
    REQUIRE(Color::Gray.b == Approx(0.5f));
    REQUIRE(Color::Gray.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::LightGray", "[color][predefined]") {
    REQUIRE(Color::LightGray.r == Approx(0.75f));
    REQUIRE(Color::LightGray.g == Approx(0.75f));
    REQUIRE(Color::LightGray.b == Approx(0.75f));
    REQUIRE(Color::LightGray.a == Approx(1.0f));
}

TEST_CASE("Predefined Color::DarkGray", "[color][predefined]") {
    REQUIRE(Color::DarkGray.r == Approx(0.25f));
    REQUIRE(Color::DarkGray.g == Approx(0.25f));
    REQUIRE(Color::DarkGray.b == Approx(0.25f));
    REQUIRE(Color::DarkGray.a == Approx(1.0f));
}

// ============================================================================
// اختبارات الحالات الحدية / Edge Case Tests
// ============================================================================

TEST_CASE("Color values can exceed 0-1 range (HDR)", "[color][edge]") {
    Color c(2.0f, -0.5f, 1.5f, 0.0f);
    REQUIRE(c.r == Approx(2.0f));
    REQUIRE(c.g == Approx(-0.5f));
    REQUIRE(c.b == Approx(1.5f));
    REQUIRE(c.a == Approx(0.0f));
}

TEST_CASE("Color copy semantics", "[color][copy]") {
    Color original(0.1f, 0.2f, 0.3f, 0.4f);
    Color copy = original;
    REQUIRE(copy.r == Approx(original.r));
    REQUIRE(copy.g == Approx(original.g));
    REQUIRE(copy.b == Approx(original.b));
    REQUIRE(copy.a == Approx(original.a));
}

TEST_CASE("Color can be modified after creation", "[color][mutation]") {
    Color c(1.0f, 0.0f, 0.0f);
    c.r = 0.0f;
    c.g = 1.0f;
    REQUIRE(c.r == Approx(0.0f));
    REQUIRE(c.g == Approx(1.0f));
    REQUIRE(c.b == Approx(0.0f));
}
