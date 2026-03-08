// ============================================================================
// ui_demo.cpp - مثال تطبيقي لنظام واجهة المستخدم
// UI system demonstration example
// ============================================================================

#include "../include/window/window.h"
#include "../include/rendering/context.h"
#include "../include/rendering/renderer2d.h"
#include "../include/input/input_manager.h"
#include "../include/resources/resource_manager.h"
#include "../include/ui/widget.h"
#include "../include/ui/label.h"
#include "../include/ui/button.h"
#include <SDL.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
#ifdef DrawText
#undef DrawText
#endif

#include <iostream>
#include <memory>
#include <vector>
#include <string>

using namespace sad::graphics;    // Window, Color, types, ResourceManager
using namespace SadGraphics;      // RenderContext, Renderer2D, InputManager, KeyCode
using namespace sad::graphics::ui;     // Widget, Label, Button, WidgetStyle, TextAlignment

// ============================================================================
constexpr int W = 1280, H = 720;

struct AppState {
    bool running    = true;
    bool showHelp   = true;
    int  clickCount = 0;
    std::string statusText = "Welcome to UI Demo!";
};

// ============================================================================
// إنشاء واجهة المستخدم / Create UI
// ============================================================================
static void CreateUI(std::vector<std::shared_ptr<Widget>>& widgets, AppState& state) {
    auto& rm = ResourceManager::GetInstance();
    auto font = rm.LoadFont("arial.ttf", 20.0f);

    // --- 0: عنوان رئيسي / Title ---
    {
        auto lbl = std::make_shared<Label>("UI System Demo", font);
        lbl->SetPosition(360, 20);
        lbl->SetSize(560, 50);
        lbl->SetAlignment(TextAlignment::MiddleCenter);
        lbl->SetTextColor(Color::White);
        WidgetStyle s;
        s.backgroundColor = Color(0.2f, 0.2f, 0.6f);
        s.borderColor     = Color::White;
        s.borderWidth     = 3.0f;
        s.padding         = 10.0f;
        lbl->SetStyle(s);
        widgets.push_back(lbl);
    }

    // --- 1: زر نقر / Click button ---
    {
        auto btn = std::make_shared<Button>("Click Me!");
        btn->SetPosition(440, 100);
        btn->SetSize(200, 50);
        btn->SetOnClick([&state]() {
            state.clickCount++;
            state.statusText = "Button clicked " + std::to_string(state.clickCount) + " times!";
            std::cout << state.statusText << '\n';
        });
        widgets.push_back(btn);
    }

    // --- 2: عداد النقرات / Counter label ---
    {
        auto lbl = std::make_shared<Label>("Clicks: 0", font);
        lbl->SetPosition(440, 170);
        lbl->SetSize(200, 40);
        lbl->SetAlignment(TextAlignment::MiddleCenter);
        lbl->SetTextColor(Color::Black);
        WidgetStyle s;
        s.backgroundColor = Color(0.86f, 0.86f, 0.86f);
        s.borderColor     = Color::Black;
        s.borderWidth     = 2.0f;
        lbl->SetStyle(s);
        widgets.push_back(lbl);
    }

    // --- 3: زر إعادة الضبط / Reset button ---
    {
        auto btn = std::make_shared<Button>("Reset Counter");
        btn->SetPosition(440, 230);
        btn->SetSize(200, 50);
        btn->SetOnClick([&state]() {
            state.clickCount = 0;
            state.statusText = "Counter reset!";
            std::cout << state.statusText << '\n';
        });
        widgets.push_back(btn);
    }

    // --- 4-7: أزرار ملونة / Coloured buttons ---
    struct ColorBtn { const char* name; Color bg; Color fg; float x; };
    ColorBtn colors[] = {
        {"Red",    Color(0.78f, 0.2f, 0.2f),  Color::White, 100},
        {"Green",  Color(0.2f, 0.78f, 0.2f),  Color::White, 280},
        {"Blue",   Color(0.2f, 0.2f, 0.78f),  Color::White, 460},
        {"Yellow", Color(0.86f, 0.86f, 0.2f), Color::Black, 640},
    };
    for (auto& c : colors) {
        auto btn = std::make_shared<Button>(c.name);
        btn->SetPosition(c.x, 350);
        btn->SetSize(150, 50);
        WidgetStyle s;
        s.backgroundColor = c.bg;
        s.foregroundColor  = c.fg;
        btn->SetNormalStyle(s);
        btn->SetOnClick([&state, n = std::string(c.name)]() {
            state.statusText = n + " button clicked!";
            std::cout << state.statusText << '\n';
        });
        widgets.push_back(btn);
    }

    // --- 8: شريط الحالة / Status bar ---
    {
        auto lbl = std::make_shared<Label>(state.statusText, font);
        lbl->SetPosition(50, 520);
        lbl->SetSize(1180, 40);
        lbl->SetAlignment(TextAlignment::MiddleLeft);
        lbl->SetTextColor(Color::White);
        WidgetStyle s;
        s.backgroundColor = Color(0.31f, 0.31f, 0.31f);
        s.borderColor     = Color::Black;
        s.borderWidth     = 2.0f;
        s.padding         = 10.0f;
        lbl->SetStyle(s);
        widgets.push_back(lbl);
    }

    // --- 9: سطر المساعدة / Help line ---
    {
        auto lbl = std::make_shared<Label>("Press H to toggle help | ESC to exit", font);
        lbl->SetPosition(50, 580);
        lbl->SetSize(1180, 30);
        lbl->SetAlignment(TextAlignment::MiddleCenter);
        lbl->SetTextColor(Color(0.7f, 0.7f, 0.7f));
        widgets.push_back(lbl);
    }
}

// ============================================================================
// تحديث الواجهة / Update UI
// ============================================================================
static void UpdateUI(std::vector<std::shared_ptr<Widget>>& widgets,
                     AppState& state, float dt) {
    for (auto& w : widgets) if (w) w->Update(dt);

    // تحديث العداد / Update counter label (index 2)
    if (widgets.size() > 2)
        if (auto lbl = std::dynamic_pointer_cast<Label>(widgets[2]))
            lbl->SetText("Clicks: " + std::to_string(state.clickCount));

    // تحديث شريط الحالة / Update status bar (index 8)
    if (widgets.size() > 8)
        if (auto lbl = std::dynamic_pointer_cast<Label>(widgets[8]))
            lbl->SetText(state.statusText);
}

// ============================================================================
// معالجة الإدخال / Handle Input
// ============================================================================
static void HandleInput(InputManager& input,
                        std::vector<std::shared_ptr<Widget>>& widgets,
                        AppState& state) {
    if (input.IsKeyPressed(KeyCode::Escape)) state.running = false;
    if (input.IsKeyPressed(KeyCode::H)) {
        state.showHelp = !state.showHelp;
        state.statusText = state.showHelp ? "Help shown" : "Help hidden";
    }

    Point2D mp = input.GetMousePosition();
    int mx = (int)mp.x, my = (int)mp.y;

    for (auto& w : widgets) if (w) w->OnMouseMove(mx, my);

    if (input.IsMouseButtonPressed(MouseButton::Left))
        for (auto& w : widgets) if (w) w->OnMouseDown(mx, my, 0);

    if (input.IsMouseButtonReleased(MouseButton::Left))
        for (auto& w : widgets) if (w) w->OnMouseUp(mx, my, 0);
}

// ============================================================================
// الدالة الرئيسية / Main Function
// ============================================================================
int main(int /*argc*/, char* /*argv*/[]) {
    std::cout << "=== UI Demo ===\n";

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << '\n';
        return -1;
    }

    try {
        Window window("UI System Demo - واجهة المستخدم", W, H, WindowFlags::Resizable);
        if (!window.IsOpen()) { SDL_Quit(); return -1; }

        RenderContext context(window.GetNativeWindow());
        if (!context.IsValid()) { SDL_Quit(); return -1; }

        Renderer2D renderer(&context);
        InputManager input;

        // تهيئة الموارد / Init resources
        auto& rm = ResourceManager::GetInstance();
        rm.AddSearchPath("assets/fonts/");
        rm.AddSearchPath("C:/Windows/Fonts/");

        AppState state;
        std::vector<std::shared_ptr<Widget>> widgets;
        CreateUI(widgets, state);
        std::cout << "Widgets: " << widgets.size() << '\n';

        Uint32 last = SDL_GetTicks();

        while (state.running) {
            Uint32 now = SDL_GetTicks();
            float dt = (now - last) / 1000.0f;
            last = now;

            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) state.running = false;
                input.ProcessEvent(ev);
            }
            input.Update();

            HandleInput(input, widgets, state);
            UpdateUI(widgets, state, dt);

            renderer.BeginFrame();
            renderer.Clear(Color(0.12f, 0.12f, 0.16f));

            for (auto& w : widgets) if (w) w->Draw(&renderer);

            renderer.EndFrame();
            context.SwapBuffers();
        }

        std::cout << "=== UI Demo Finished ===\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        SDL_Quit();
        return -1;
    }

    SDL_Quit();
    return 0;
}
