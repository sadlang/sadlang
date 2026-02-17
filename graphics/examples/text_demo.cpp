// ==============================================================================
// text_demo.cpp - عرض توضيحي لنظام النصوص / Text System Demo
// ==============================================================================
// الوصف: مثال شامل يعرض إمكانيات نظام النصوص والخطوط
// Description: Comprehensive example showcasing text and font system capabilities
// ==============================================================================

#include "../include/window/window.h"
#include "../include/rendering/context.h"
#include "../include/rendering/renderer2d.h"
#include "../include/input/input_manager.h"
#include "../include/text/font.h"
#include "../include/text/arabic_text.h"
#include <SDL.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
#ifdef DrawText
#undef DrawText
#endif

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace sad::graphics;
using namespace SadGraphics;

// ==============================================================================
constexpr int W = 1280, H = 720;

struct FontInfo {
    FontRef font;
    std::string name;
    float size;
};

struct AppState {
    bool running       = true;
    int  selectedDemo  = 0;
    int  selectedFont  = 0;
    int  selectedColor = 0;
    Float32 time       = 0.0f;
    Float32 waveAmp    = 20.0f;
    Float32 waveFreq   = 2.0f;

    std::vector<FontInfo> fonts;
    std::vector<Color> colors = {
        Color::White, Color::Red, Color::Green, Color::Blue,
        Color::Yellow, Color::Magenta, Color::Cyan,
        Color(1.0f, 0.5f, 0.0f) // orange
    };

    std::string sample  = "Hello, World!";
    std::string arabic  = "\xD8\xA7\xD9\x84\xD9\x84\xD8\xBA\xD8\xA9 \xD8\xA7\xD9\x84\xD8\xB9\xD8\xB1\xD8\xA8\xD9\x8A\xD8\xA9 \xD8\xAC\xD9\x85\xD9\x8A\xD9\x84\xD8\xA9"; // اللغة العربية جميلة
    std::string english = "The quick brown fox jumps over the lazy dog";
    std::string sadLang = "\xD9\x84\xD8\xBA\xD8\xA9 \xD8\xB5"; // لغة ص
    std::string bismillah = "\xD8\xA8\xD8\xB3\xD9\x85 \xD8\xA7\xD9\x84\xD9\x84\xD9\x87"; // بسم الله
    std::string mixed = "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7 Hello \xD8\xB9\xD8\xA7\xD9\x84\xD9\x85"; // مرحبا Hello عالم
};

// ==============================================================================
static void LoadFonts(AppState& s) {
    const char* paths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/times.ttf",
        "C:/Windows/Fonts/cour.ttf",
        "C:/Windows/Fonts/verdana.ttf",
    };
    for (auto p : paths) {
        auto f = Font::CreateFromFile(p, 24.0f);
        if (f && f->IsValid()) {
            std::string name(p);
            auto pos = name.find_last_of("/\\");
            if (pos != std::string::npos) name = name.substr(pos + 1);
            s.fonts.push_back({f, name, 24.0f});
            std::cout << "Loaded: " << name << '\n';
        }
    }
    if (s.fonts.empty()) {
        auto df = Font::CreateDefault(24.0f);
        if (df && df->IsValid()) s.fonts.push_back({df, "default", 24.0f});
    }
    if (s.fonts.empty()) { std::cerr << "No fonts!\n"; s.running = false; }
}

static void HandleInput(InputManager& in, AppState& s) {
    if (in.IsKeyPressed(KeyCode::Escape)) s.running = false;
    if (in.IsKeyPressed(KeyCode::Num1))   s.selectedDemo = 0;
    if (in.IsKeyPressed(KeyCode::Num2))   s.selectedDemo = 1;
    if (in.IsKeyPressed(KeyCode::Num3))   s.selectedDemo = 2;
    if (in.IsKeyPressed(KeyCode::Num4))   s.selectedDemo = 3;
    if (in.IsKeyPressed(KeyCode::Num5))   s.selectedDemo = 4;
    if (in.IsKeyPressed(KeyCode::Num6))   s.selectedDemo = 5;
    if (in.IsKeyPressed(KeyCode::F) && !s.fonts.empty())
        s.selectedFont = (s.selectedFont + 1) % (int)s.fonts.size();
    if (in.IsKeyPressed(KeyCode::C))
        s.selectedColor = (s.selectedColor + 1) % (int)s.colors.size();
    if (in.IsKeyHeld(KeyCode::ArrowUp))   s.waveAmp += 10.0f * 0.016f;
    if (in.IsKeyHeld(KeyCode::ArrowDown)) { s.waveAmp -= 10.0f * 0.016f; if (s.waveAmp < 0) s.waveAmp = 0; }
}

// ==============================================================================
static void Demo1(Renderer2D& r, AppState& s) {
    auto& font = s.fonts[s.selectedFont].font;
    auto& col  = s.colors[s.selectedColor];
    float y = 100;
    r.DrawText("Demo 1: Basic Text Rendering", font, 50, y, Color::Yellow); y += 60;
    r.DrawText("Standard: " + s.english, font, 50, y, col); y += 50;
    r.DrawText("Arabic: " + s.arabic, font, 50, y, col); y += 50;
    r.DrawText("Mixed: " + s.sample, font, 50, y, col); y += 60;
    for (int i = 0; i < std::min(3, (int)s.fonts.size()); i++) {
        r.DrawText("Font " + std::to_string(i+1) + ": " + s.fonts[i].name,
                   s.fonts[i].font, 50, y, col);
        y += 50;
    }
}

static void Demo2(Renderer2D& r, AppState& s) {
    auto& font = s.fonts[s.selectedFont].font;
    float cx = W / 2.0f, cy = H / 2.0f;
    r.DrawTextCentered("Demo 2: Text Alignment", font, cx, 100, Color::Yellow);
    r.DrawLine(cx, 0, cx, (float)H, Color(0.5f,0.5f,0.5f,0.5f), 1.0f);
    r.DrawLine(0, cy, (float)W, cy, Color(0.5f,0.5f,0.5f,0.5f), 1.0f);
    r.DrawTextAligned("Top-Left (0,0)",    font, cx, cy, 0.0f, 0.0f, Color::Red);
    r.DrawTextAligned("Top-Center (0.5,0)",font, cx, cy, 0.5f, 0.0f, Color::Green);
    r.DrawTextAligned("Top-Right (1,0)",   font, cx, cy, 1.0f, 0.0f, Color::Blue);
    r.DrawTextCentered("CENTER",           font, cx, cy, Color::Magenta);
    r.DrawTextAligned("Bottom-Left (0,1)", font, cx, cy, 0.0f, 1.0f, Color::Yellow);
    r.DrawTextAligned("Bottom-Right (1,1)",font, cx, cy, 1.0f, 1.0f, Color::Cyan);
}

static void Demo3(Renderer2D& r, AppState& s) {
    auto& font = s.fonts[s.selectedFont].font;
    r.DrawTextCentered("Demo 3: Text Colors", font, W/2.0f, 100, Color::Yellow);
    const char* names[] = {"White","Red","Green","Blue","Yellow","Magenta","Cyan","Orange"};
    float y = 200;
    for (size_t i = 0; i < s.colors.size(); i++) {
        r.DrawText(std::string(names[i]) + ": " + s.sample, font, 50, y, s.colors[i]);
        y += 50;
    }
}

static void Demo4(Renderer2D& r, AppState& s) {
    auto& font = s.fonts[s.selectedFont].font;
    r.DrawTextCentered("Demo 4: Animated Text Effects", font, W/2.0f, 50, Color::Yellow);

    // Wave
    std::string text = "WAVE EFFECT";
    float bx = 200, by = 200, cw = 30;
    for (size_t i = 0; i < text.size(); i++) {
        float off = std::sin(s.time * s.waveFreq + i * 0.5f) * s.waveAmp;
        float h = s.time + i * 0.1f;
        Color c(std::sin(h)*0.5f+0.5f, std::sin(h+2)*0.5f+0.5f, std::sin(h+4)*0.5f+0.5f);
        r.DrawText(std::string(1, text[i]), font, bx + i * cw, by + off, c);
    }

    // Pulse
    float alpha = std::sin(s.time * 2.0f) * 0.5f + 0.5f;
    r.DrawTextCentered("PULSE EFFECT", font, W/2.0f, 350, Color(1,1,1,alpha));

    // Rainbow
    text = "RAINBOW TEXT";
    cw = 40;
    float sx = (W - text.size() * cw) / 2;
    for (size_t i = 0; i < text.size(); i++) {
        float h = (s.time + i * 0.3f) * 2.0f;
        Color c(std::sin(h)*0.5f+0.5f, std::sin(h+2.094f)*0.5f+0.5f, std::sin(h+4.189f)*0.5f+0.5f);
        r.DrawText(std::string(1, text[i]), font, sx + i * cw, 500, c);
    }
}

static void Demo5_Arabic(Renderer2D& r, AppState& s) {
    auto& font = s.fonts[s.selectedFont].font;
    r.DrawTextCentered("Demo 5: Arabic Text Rendering", font, W/2.0f, 50, Color::Yellow);

    float y = 130;
    // عنوان القسم / Section title
    r.DrawText("Arabic Shaping + RTL:", font, 50, y, Color::Cyan); y += 50;

    // عرض النص العربي بالتشكيل / Arabic text with shaping
    r.DrawTextArabic(s.arabic, font, W - 50, y, Color::White); y += 50;
    r.DrawTextArabic(s.sadLang, font, W - 50, y, Color::Green); y += 50;
    r.DrawTextArabic(s.bismillah, font, W - 50, y, Color(1.0f, 0.85f, 0.0f)); y += 70;

    // DrawTextAuto - كشف تلقائي / Auto detect
    r.DrawText("Auto-detect direction (DrawTextAuto):", font, 50, y, Color::Cyan); y += 50;
    r.DrawTextAuto(s.mixed, font, 50, y, Color::White); y += 50;
    r.DrawTextAuto(s.english, font, 50, y, Color::White); y += 50;
    r.DrawTextAuto(s.arabic, font, 50, y, Color::White); y += 70;

    // الأبجدية العربية / Arabic alphabet
    r.DrawText("Arabic Alphabet Shaping:", font, 50, y, Color::Cyan); y += 50;
    std::string alph = "\xD8\xA7\xD8\xA8\xD8\xAA\xD8\xAB\xD8\xAC\xD8\xAD\xD8\xAE\xD8\xAF\xD8\xB0\xD8\xB1\xD8\xB2\xD8\xB3\xD8\xB4\xD8\xB5\xD8\xB6\xD8\xB7\xD8\xB8\xD8\xB9\xD8\xBA\xD9\x81\xD9\x82\xD9\x83\xD9\x84\xD9\x85\xD9\x86\xD9\x87\xD9\x88\xD9\x8A";
    r.DrawTextArabic(alph, font, W - 50, y, Color::Yellow); y += 50;

    // ألوان متعددة / Multiple colors
    r.DrawText("Colored Arabic:", font, 50, y, Color::Cyan); y += 50;
    r.DrawTextArabic(s.sadLang, font, 650, y, Color::Red);
    r.DrawTextArabic(s.sadLang, font, 750, y, Color::Green);
    r.DrawTextArabic(s.sadLang, font, 850, y, Color::Blue);
}

static void Demo6(Renderer2D& r, AppState& s) {
    auto& font = s.fonts[s.selectedFont].font;
    r.DrawTextCentered("Demo 5: Text Measurement", font, W/2.0f, 50, Color::Yellow);

    float tw, th;
    font->MeasureText(s.sample, tw, th);
    float x = 100, y = 200;
    r.DrawRect(x-5, y-5, tw+10, th+10, Color(0.25f,0.25f,0.25f,0.5f), true);
    r.DrawRect(x-5, y-5, tw+10, th+10, Color::White, false);
    r.DrawText(s.sample, font, x, y, Color::White);

    std::ostringstream o; o << "Width: " << std::fixed << std::setprecision(1) << tw << " px";
    r.DrawText(o.str(), font, x, y+th+30, Color::Cyan);
    o.str(""); o << "Height: " << th << " px";
    r.DrawText(o.str(), font, x, y+th+60, Color::Cyan);
    o.str(""); o << "Font Size: " << font->GetFontSize();
    r.DrawText(o.str(), font, x, y+th+90, Color::Cyan);
    o.str(""); o << "Line Height: " << font->GetLineHeight();
    r.DrawText(o.str(), font, x, y+th+120, Color::Cyan);
}

// ==============================================================================
int main(int /*argc*/, char* /*argv*/[]) {
    std::cout << "\n=== Sad Graphics - Text Demo ===\n"
              << "1-6=Demo  F=Font  C=Color  Up/Down=Wave  ESC=Exit\n"
              << "5=Arabic Text  6=Text Measurement\n\n";

    if (SDL_Init(SDL_INIT_VIDEO) < 0) { std::cerr << "SDL: " << SDL_GetError() << '\n'; return -1; }

    try {
        Window window("Sad Graphics - Text Demo", W, H, WindowFlags::Resizable);
        if (!window.IsOpen()) { SDL_Quit(); return -1; }

        RenderContext context(window.GetNativeWindow());
        if (!context.IsValid()) { SDL_Quit(); return -1; }

        Renderer2D renderer(&context);
        InputManager input;
        AppState state;
        LoadFonts(state);
        if (!state.running) { SDL_Quit(); return -1; }

        Uint32 last = SDL_GetTicks();
        while (state.running) {
            Uint32 now = SDL_GetTicks();
            state.time += (now - last) / 1000.0f;
            last = now;

            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) state.running = false;
                input.ProcessEvent(ev);
            }
            input.Update();
            HandleInput(input, state);

            renderer.BeginFrame();
            renderer.Clear(Color(0.08f, 0.08f, 0.12f));
            switch (state.selectedDemo) {
                case 0: Demo1(renderer, state); break;
                case 1: Demo2(renderer, state); break;
                case 2: Demo3(renderer, state); break;
                case 3: Demo4(renderer, state); break;
                case 4: Demo5_Arabic(renderer, state); break;
                case 5: Demo6(renderer, state); break;
            }
            renderer.EndFrame();
            context.SwapBuffers();
        }

        renderer.PrintStats();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        SDL_Quit(); return -1;
    }
    SDL_Quit();
    return 0;
}