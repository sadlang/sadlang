// ==============================================================================
// simple_demo.cpp - مثال بسيط لاستخدام مكتبة الرسومات
// Simple Graphics Library Demo
// ==============================================================================
// الوصف: مثال يوضح رسم أشكال هندسية والتفاعل مع لوحة المفاتيح والفأرة
// Description: Example showing geometric shape drawing and keyboard/mouse input
// ==============================================================================

#include "../include/window/window.h"
#include "../include/rendering/context.h"
#include "../include/rendering/renderer2d.h"
#include "../include/input/input_manager.h"
#include <SDL.h>

// إلغاء ماكرو ويندوز المتعارض / Undo conflicting Windows macros
#ifdef CreateWindow
#undef CreateWindow
#endif
#ifdef DrawText
#undef DrawText
#endif

#include <iostream>
#include <cmath>

using namespace sad::graphics;   // Window, Color, Point2D, WindowFlags, etc.
using namespace SadGraphics;     // RenderContext, Renderer2D, InputManager, KeyCode

// ==============================================================================
// ثوابت البرنامج / Program Constants
// ==============================================================================
constexpr int WINDOW_WIDTH  = 1024;
constexpr int WINDOW_HEIGHT = 768;
constexpr float MOVE_SPEED  = 300.0f;  // بكسل/ثانية / pixels per second

// ==============================================================================
// حالة البرنامج / Application State
// ==============================================================================
struct AppState {
    bool    running = true;
    Float32 time    = 0.0f;
    Point2D circlePos{WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
    Color   bgColor{0.08f, 0.08f, 0.12f, 1.0f};
    int     bgIndex = 0;
};

// ==============================================================================
// معالجة الإدخال / Process Input
// ==============================================================================
static void ProcessInput(InputManager& input, AppState& state, Float32 dt) {
    if (input.IsKeyPressed(KeyCode::Escape))
        state.running = false;

    // تحريك الدائرة بالأسهم / Move circle with arrows
    Float32 step = MOVE_SPEED * dt;
    if (input.IsKeyHeld(KeyCode::ArrowUp))    state.circlePos.y -= step;
    if (input.IsKeyHeld(KeyCode::ArrowDown))  state.circlePos.y += step;
    if (input.IsKeyHeld(KeyCode::ArrowLeft))  state.circlePos.x -= step;
    if (input.IsKeyHeld(KeyCode::ArrowRight)) state.circlePos.x += step;

    // تبديل لون الخلفية / Cycle background colour
    if (input.IsKeyPressed(KeyCode::Space)) {
        static const Color presets[] = {
            Color(0.08f, 0.08f, 0.12f),   // أزرق-أسود / blue-black
            Color(0.12f, 0.06f, 0.06f),   // أحمر-داكن / dark red
            Color(0.06f, 0.12f, 0.06f),   // أخضر-داكن / dark green
            Color(0.0f,  0.0f,  0.0f)     // أسود / black
        };
        state.bgIndex = (state.bgIndex + 1) % 4;
        state.bgColor = presets[state.bgIndex];
    }

    // تتبع الفأرة / Follow mouse
    if (input.IsMouseMoved()) {
        Point2D m = input.GetMousePosition();
        state.circlePos.x += (m.x - state.circlePos.x) * 0.08f;
        state.circlePos.y += (m.y - state.circlePos.y) * 0.08f;
    }
}

// ==============================================================================
// رسم الإطار / Render Frame
// ==============================================================================
static void RenderFrame(Renderer2D& renderer, const AppState& state,
                        const InputManager& input) {
    renderer.BeginFrame();
    renderer.Clear(state.bgColor);

    // --- شبكة خلفية / Background grid ---
    Color gridCol(0.15f, 0.15f, 0.20f);
    for (int x = 0; x < WINDOW_WIDTH; x += 50)
        renderer.DrawLine((Float32)x, 0, (Float32)x, (Float32)WINDOW_HEIGHT, gridCol, 1.0f);
    for (int y = 0; y < WINDOW_HEIGHT; y += 50)
        renderer.DrawLine(0, (Float32)y, (Float32)WINDOW_WIDTH, (Float32)y, gridCol, 1.0f);

    // --- مستطيلات / Rectangles ---
    renderer.DrawRect(50, 50, 200, 100, Color::Red, true);
    renderer.DrawRectOutline(50, 50, 200, 100, Color::Red, Color::Yellow, 2.0f);

    // مستطيل متحرك / Animated rectangle
    Float32 rx = 300.0f + std::sin(state.time * 2.0f) * 100.0f;
    Float32 ry = 80.0f  + std::cos(state.time * 3.0f) * 30.0f;
    renderer.DrawRect(rx, ry, 80, 60, Color::Green, true);

    // --- دوائر / Circles ---
    renderer.DrawCircle(550, 120, 50, Color::Blue, true);
    renderer.DrawCircle(550, 120, 52, Color::White, false);

    // دائرة متحركة / Animated orbit circle
    Float32 ox = 700.0f + std::cos(state.time) * 80.0f;
    Float32 oy = 120.0f + std::sin(state.time) * 80.0f;
    renderer.DrawCircle(ox, oy, 25, Color(1.0f, 0.5f, 0.0f), true);

    // --- خطوط دوارة / Spinning lines ---
    for (int i = 0; i < 12; i++) {
        float a = state.time + i * (6.2832f / 12.0f);
        Float32 lx = 150.0f + std::cos(a) * 80.0f;
        Float32 ly = 400.0f + std::sin(a) * 80.0f;
        renderer.DrawLine(150, 400, lx, ly, Color(0.0f, 1.0f, 0.5f), 1.5f);
    }

    // --- مثلث / Triangle ---
    Float32 tx = 400.0f + std::cos(state.time) * 15.0f;
    renderer.DrawTriangle(tx, 330, tx + 80, 440, tx - 80, 440,
                          Color::Yellow, true);

    // --- قطع ناقص / Ellipse ---
    renderer.DrawEllipse(700, 400, 80, 40, Color(0.5f, 0.0f, 1.0f), true);
    renderer.DrawEllipse(700, 400, 82, 42, Color::White, false);

    // --- شفافية تدريجية / Gradient opacity rectangles ---
    for (int i = 0; i < 6; i++) {
        Float32 alpha = 0.15f + i * 0.17f;
        renderer.DrawRect(50.0f + i * 55.0f, 550, 50, 35,
                          Color(1.0f, 0.0f, 0.5f, alpha), true);
    }

    // --- قوس دوار / Rotating arc ---
    renderer.DrawArc(WINDOW_WIDTH - 120.0f, WINDOW_HEIGHT - 120.0f, 50,
                     state.time, state.time + 3.14159f, Color::Magenta, 48);

    // --- الدائرة التفاعلية / Interactive circle ---
    Float32 pulse  = std::sin(state.time * 2.5f) * 8.0f;
    Float32 radius = 40.0f + pulse;
    Float32 cr = state.circlePos.x / WINDOW_WIDTH;   // أحمر حسب X / red by X
    Float32 cg = state.circlePos.y / WINDOW_HEIGHT;  // أخضر حسب Y / green by Y
    Color   circleColor(cr, cg, 0.6f);
    renderer.DrawCircle(state.circlePos.x, state.circlePos.y, radius, circleColor, true);
    renderer.DrawCircle(state.circlePos.x, state.circlePos.y, radius + 4, Color::White, false);

    // --- مؤشر الفأرة / Mouse cursor crosshair ---
    Point2D mp = input.GetMousePosition();
    renderer.DrawLine(mp.x - 12, mp.y, mp.x + 12, mp.y, Color::White, 1.0f);
    renderer.DrawLine(mp.x, mp.y - 12, mp.x, mp.y + 12, Color::White, 1.0f);

    // نقطة ضغط الفأرة / Mouse click dot
    if (input.IsMouseButtonHeld(MouseButton::Left))
        renderer.DrawCircle(mp.x, mp.y, 8, Color::Red, true);

    renderer.EndFrame();
}

// ==============================================================================
// الدالة الرئيسية / Main Function
// ==============================================================================
int main(int /*argc*/, char* /*argv*/[]) {

    std::cout << "\n=== مكتبة رسومات ص - مثال بسيط ===\n"
              << "=== Sad Graphics - Simple Demo  ===\n"
              << "ESC  = خروج / Exit\n"
              << "Arrows = تحريك الدائرة / Move circle\n"
              << "Space  = تبديل الخلفية / Cycle background\n"
              << "Mouse  = تتبع / Follow\n\n";

    // --- تهيئة SDL / Initialise SDL ---
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init فشل / failed: " << SDL_GetError() << '\n';
        return -1;
    }

    try {
        // --- إنشاء النافذة / Create window ---
        Window window("مكتبة رسومات ص - مثال بسيط  |  Sad Graphics Demo",
                      WINDOW_WIDTH, WINDOW_HEIGHT, WindowFlags::Resizable);

        if (!window.IsOpen()) {
            std::cerr << "فشل إنشاء النافذة / Window creation failed\n";
            SDL_Quit();
            return -1;
        }

        // --- سياق OpenGL / OpenGL context ---
        RenderContext context(window.GetNativeWindow());
        if (!context.IsValid()) {
            std::cerr << "فشل سياق OpenGL / OpenGL context failed\n";
            SDL_Quit();
            return -1;
        }

        std::cout << "OpenGL: " << context.GetVersion() << '\n'
                  << "GPU   : " << context.GetRenderer() << "\n\n";

        // --- محرك الرسم / Renderer ---
        Renderer2D renderer(&context);

        // --- مدير الإدخال / Input manager ---
        InputManager input;

        // --- حالة البرنامج / State ---
        AppState state;
        Uint32 lastTick = SDL_GetTicks();

        // ==========  حلقة اللعبة الرئيسية / Main loop  ==========
        while (state.running) {
            // حساب الوقت / Timing
            Uint32 now = SDL_GetTicks();
            Float32 dt = (now - lastTick) / 1000.0f;
            lastTick = now;
            state.time += dt;

            // أحداث SDL / SDL events
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT)
                    state.running = false;
                input.ProcessEvent(ev);
            }

            // تحديث الإدخال / Update input
            input.Update();

            // معالجة الإدخال / Process input
            ProcessInput(input, state, dt);

            // رسم / Render
            RenderFrame(renderer, state, input);

            // عرض / Present
            context.SwapBuffers();
        }

        // إحصائيات / Stats
        std::cout << "\n=== إحصائيات / Stats ===\n";
        renderer.PrintStats();
        std::cout << "الوقت الكلي / Total time: " << state.time << "s\n";

    } catch (const std::exception& e) {
        std::cerr << "خطأ / Error: " << e.what() << '\n';
        SDL_Quit();
        return -1;
    }

    SDL_Quit();
    std::cout << "تم الخروج بنجاح / Exited OK\n";
    return 0;
}