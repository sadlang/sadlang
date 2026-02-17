// ==============================================================================
// resource_demo.cpp - عرض توضيحي لنظام إدارة الموارد / Resource Management Demo
// ==============================================================================

#include "../include/window/window.h"
#include "../include/rendering/context.h"
#include "../include/rendering/renderer2d.h"
#include "../include/input/input_manager.h"
#include "../include/resources/resource_manager.h"
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

// ==============================================================================
struct AppState {
    bool running    = true;
    bool showStats  = true;
    bool showHelp   = true;
    int  selectedDemo = 0;
    Float32 time    = 0.0f;

    // إحصائيات الأداء / Performance stats
    float fps       = 0.0f;
    float frameTime = 0.0f;
    int   frameCount= 0;
    float fpsTimer  = 0.0f;

    // الموارد المحملة / Loaded resources
    std::vector<TextureRef>  textures;
    std::vector<FontRef>     fonts;
    std::vector<std::string> textureNames;
    std::vector<std::string> fontNames;
};

// ==============================================================================
static void InitSearchPaths() {
    auto& rm = ResourceManager::GetInstance();
    rm.AddSearchPath("assets/");
    rm.AddSearchPath("assets/textures/");
    rm.AddSearchPath("assets/fonts/");
    rm.AddSearchPath("C:/Windows/Fonts/");
    std::cout << "مسارات البحث جاهزة / Search paths ready\n";
}

static void LoadEssentialResources(AppState& st) {
    auto& rm = ResourceManager::GetInstance();
    try {
        auto font = rm.LoadFont("arial.ttf", 20.0f);
        if (font) {
            st.fonts.push_back(font);
            st.fontNames.push_back("arial.ttf (20)");
            std::cout << "تم تحميل Arial / Arial loaded\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "خطأ تحميل خط / Font load error: " << e.what() << '\n';
    }
    std::cout << "عدد الخطوط / Fonts loaded: " << st.fonts.size() << '\n';
}

// ==============================================================================
static void HandleInput(AppState& st, InputManager& input) {
    if (input.IsKeyPressed(KeyCode::Escape)) st.running = false;
    if (input.IsKeyPressed(KeyCode::H))      st.showHelp  = !st.showHelp;
    if (input.IsKeyPressed(KeyCode::S))      st.showStats = !st.showStats;

    if (input.IsKeyPressed(KeyCode::Num1)) st.selectedDemo = 0;
    if (input.IsKeyPressed(KeyCode::Num2)) st.selectedDemo = 1;
    if (input.IsKeyPressed(KeyCode::Num3)) st.selectedDemo = 2;
    if (input.IsKeyPressed(KeyCode::Num4)) st.selectedDemo = 3;

    if (input.IsKeyPressed(KeyCode::L)) {
        auto& rm = ResourceManager::GetInstance();
        auto font = rm.LoadFont("times.ttf", 24.0f);
        if (font) {
            st.fonts.push_back(font);
            st.fontNames.push_back("times.ttf (24)");
            std::cout << "تم تحميل Times / Times loaded\n";
        }
    }

    if (input.IsKeyPressed(KeyCode::U)) {
        size_t n = ResourceManager::GetInstance().UnloadUnused();
        std::cout << "تفريغ " << n << " مورد / Unloaded " << n << " resources\n";
    }

    if (input.IsKeyPressed(KeyCode::P))
        ResourceManager::GetInstance().PrintStats();

    if (input.IsKeyPressed(KeyCode::C)) {
        st.textures.clear(); st.fonts.clear();
        st.textureNames.clear(); st.fontNames.clear();
        ResourceManager::GetInstance().UnloadAll();
        LoadEssentialResources(st);
    }
}

// ==============================================================================
// عروض توضيحية / Demo screens
// ==============================================================================

static void Demo1_BasicLoading(Renderer2D& r, AppState& st) {
    if (st.fonts.empty()) return;
    const auto& f = st.fonts[0];
    float y = 100, ls = 35;

    r.DrawText("Demo 1: Basic Resource Loading", f, 50, y, Color::Yellow);
    y += ls * 1.5f;
    r.DrawText("Resources cached on first load, reused automatically", f, 50, y, Color::White);
    y += ls;
    r.DrawText("Memory management with configurable limits", f, 50, y, Color::White);
    y += ls * 2;

    r.DrawText("Loaded Fonts:", f, 50, y, Color::Cyan);
    y += ls;
    for (size_t i = 0; i < st.fontNames.size(); ++i) {
        std::ostringstream o; o << "  " << (i+1) << ". " << st.fontNames[i];
        r.DrawText(o.str(), f, 70, y, Color::White);
        y += ls;
    }
    y += ls;
    r.DrawText("Loaded Textures:", f, 50, y, Color::Cyan);
    y += ls;
    if (st.textures.empty())
        r.DrawText("  (none)", f, 70, y, Color::Gray);
    else for (size_t i = 0; i < st.textureNames.size(); ++i) {
        std::ostringstream o; o << "  " << (i+1) << ". " << st.textureNames[i];
        r.DrawText(o.str(), f, 70, y, Color::White);
        y += ls;
    }
}

static void Demo2_MemoryStats(Renderer2D& r, AppState& st) {
    if (st.fonts.empty()) return;
    const auto& f = st.fonts[0];
    auto& rm = ResourceManager::GetInstance();
    const auto& s = rm.GetStats();
    float y = 100, ls = 35;
    std::ostringstream o;

    r.DrawText("Demo 2: Memory & Cache Statistics", f, 50, y, Color::Yellow);
    y += ls * 2;

    r.DrawText("Resource Count:", f, 50, y, Color::Cyan); y += ls * 1.2f;
    o << "  Loaded: " << s.loadedResources; r.DrawText(o.str(), f, 70, y, Color::White); y += ls;
    o.str(""); o << "  Cached: " << s.cachedResources; r.DrawText(o.str(), f, 70, y, Color::White); y += ls;
    o.str(""); o << "  Textures: " << s.textureCount; r.DrawText(o.str(), f, 70, y, Color::Green); y += ls;
    o.str(""); o << "  Fonts: " << s.fontCount; r.DrawText(o.str(), f, 70, y, Color::Green); y += ls;
    o.str(""); o << "  Shaders: " << s.shaderCount; r.DrawText(o.str(), f, 70, y, Color::Green); y += ls * 2;

    r.DrawText("Memory Usage:", f, 50, y, Color::Cyan); y += ls * 1.2f;
    float usedMB  = s.memoryUsed  / (1024.0f * 1024.0f);
    float limitMB = s.memoryLimit / (1024.0f * 1024.0f);
    float pct = s.memoryLimit > 0 ? s.memoryUsed * 100.0f / s.memoryLimit : 0;
    Color uc = pct < 50 ? Color::Green : pct < 80 ? Color::Yellow : Color::Red;

    o.str(""); o << "  Used: " << std::fixed << std::setprecision(2) << usedMB << " MB";
    r.DrawText(o.str(), f, 70, y, Color::White); y += ls;
    o.str(""); o << "  Limit: " << std::fixed << std::setprecision(2) << limitMB << " MB";
    r.DrawText(o.str(), f, 70, y, Color::White); y += ls;
    o.str(""); o << "  Usage: " << std::fixed << std::setprecision(1) << pct << "%";
    r.DrawText(o.str(), f, 70, y, uc); y += ls;

    // شريط التقدم / Progress bar
    r.DrawRect(70, y, 300, 20, Color(0.2f, 0.2f, 0.2f), true);
    r.DrawRect(70, y, 300 * (pct / 100.0f), 20, uc, true);
    r.DrawRect(70, y, 300, 20, Color::White, false);
}

static void Demo3_CachePerformance(Renderer2D& r, AppState& st) {
    if (st.fonts.empty()) return;
    const auto& f = st.fonts[0];
    const auto& s = ResourceManager::GetInstance().GetStats();
    float y = 100, ls = 35;
    std::ostringstream o;

    r.DrawText("Demo 3: Cache Performance", f, 50, y, Color::Yellow); y += ls * 2;
    r.DrawText("Cache Statistics:", f, 50, y, Color::Cyan); y += ls * 1.2f;
    o << "  Load Attempts: " << s.loadCount; r.DrawText(o.str(), f, 70, y, Color::White); y += ls;
    o.str(""); o << "  Cache Hits: " << s.cacheHits; r.DrawText(o.str(), f, 70, y, Color::Green); y += ls;
    o.str(""); o << "  Cache Misses: " << s.cacheMisses; r.DrawText(o.str(), f, 70, y, Color::Red); y += ls;

    float ratio = s.GetCacheHitRatio() * 100.0f;
    Color rc = ratio > 80 ? Color::Green : ratio > 50 ? Color::Yellow : Color::Red;
    o.str(""); o << "  Hit Ratio: " << std::fixed << std::setprecision(1) << ratio << "%";
    r.DrawText(o.str(), f, 70, y, rc); y += ls * 2;

    r.DrawText("Explanation:", f, 50, y, Color::Cyan); y += ls * 1.2f;
    r.DrawText("- Hit: found in cache (fast)", f, 70, y, Color::White); y += ls;
    r.DrawText("- Miss: loaded from disk (slow)", f, 70, y, Color::White); y += ls;
    r.DrawText("- Higher ratio = better performance", f, 70, y, Color::White); y += ls * 2;

    // رسم بياني بسيط / Simple chart
    if (s.cacheHits + s.cacheMisses > 0) {
        r.DrawCircle(200, 520, 80, Color::Green, true, 32);
        r.DrawText("Hits", f, 300, 490, Color::Green);
        r.DrawText("Misses", f, 300, 530, Color::Red);
    }
}

static void Demo4_AutoManagement(Renderer2D& r, AppState& st) {
    if (st.fonts.empty()) return;
    const auto& f = st.fonts[0];
    auto& rm = ResourceManager::GetInstance();
    float y = 100, ls = 35;
    std::ostringstream o;

    r.DrawText("Demo 4: Automatic Resource Management", f, 50, y, Color::Yellow); y += ls * 2;
    r.DrawText("Features:", f, 50, y, Color::Cyan); y += ls * 1.2f;
    r.DrawText("1. Auto-Caching on first load", f, 70, y, Color::White); y += ls;
    r.DrawText("2. Auto-Unload of unused resources", f, 70, y, Color::White); y += ls;
    r.DrawText("3. Memory limit enforcement", f, 70, y, Color::White); y += ls;
    r.DrawText("4. Thread-safe operations", f, 70, y, Color::White); y += ls * 2;

    r.DrawText("Current Settings:", f, 50, y, Color::Cyan); y += ls * 1.2f;
    o << "  Auto-Unload: " << (rm.IsAutoUnloadEnabled() ? "ON" : "OFF");
    r.DrawText(o.str(), f, 70, y, Color::White); y += ls;
    o.str(""); o << "  Mem Limit: " << (rm.GetMemoryLimit()/1024/1024) << " MB";
    r.DrawText(o.str(), f, 70, y, Color::White); y += ls;
    o.str(""); o << "  Res Limit: " << rm.GetResourceLimit();
    r.DrawText(o.str(), f, 70, y, Color::White); y += ls * 2;

    r.DrawText("Tips:", f, 50, y, Color::Cyan); y += ls * 1.2f;
    r.DrawText("- Use LoadTexture() for quick loading", f, 70, y, Color::LightGray); y += ls;
    r.DrawText("- Resources shared automatically", f, 70, y, Color::LightGray); y += ls;
    r.DrawText("- Manual unload only if needed", f, 70, y, Color::LightGray);
}

// ==============================================================================
static void ShowHelp(Renderer2D& r, const FontRef& f) {
    float x = 20, y = H - 260.0f, lh = 25;
    r.DrawRect(10, y - 10, 400, 260, Color(0,0,0,0.7f), true);
    r.DrawText("Controls (H to toggle):", f, x, y, Color::Yellow); y += lh;
    r.DrawText("ESC  - Exit",             f, x, y, Color::White); y += lh;
    r.DrawText("1-4  - Select Demo",      f, x, y, Color::White); y += lh;
    r.DrawText("S    - Toggle Stats",     f, x, y, Color::White); y += lh;
    r.DrawText("L    - Load Extra Font",  f, x, y, Color::White); y += lh;
    r.DrawText("U    - Unload Unused",    f, x, y, Color::White); y += lh;
    r.DrawText("P    - Print Stats",      f, x, y, Color::White); y += lh;
    r.DrawText("C    - Clear & Reload",   f, x, y, Color::White);
}

static void ShowStats(Renderer2D& r, const FontRef& f, const AppState& st) {
    float x = W - 300.0f, y = 20, lh = 25;
    r.DrawRect(x - 10, y - 10, 290, 110, Color(0,0,0,0.7f), true);
    r.DrawText("Performance:", f, x, y, Color::Yellow); y += lh;
    std::ostringstream o;
    o << "FPS: " << std::fixed << std::setprecision(1) << st.fps;
    r.DrawText(o.str(), f, x, y, Color::Green); y += lh;
    o.str(""); o << "Frame: " << std::setprecision(2) << st.frameTime << " ms";
    r.DrawText(o.str(), f, x, y, Color::Cyan); y += lh;
    o.str(""); o << "Demo: " << (st.selectedDemo + 1) << "/4";
    r.DrawText(o.str(), f, x, y, Color::White);
}

// ==============================================================================
int main(int /*argc*/, char* /*argv*/[]) {
    std::cout << "\n=== مدير الموارد / Resource Manager Demo ===\n\n";

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << '\n';
        return -1;
    }

    try {
        Window window("Resource Manager Demo - عرض إدارة الموارد", W, H,
                      WindowFlags::Resizable);
        if (!window.IsOpen()) { SDL_Quit(); return -1; }

        RenderContext context(window.GetNativeWindow());
        if (!context.IsValid()) { SDL_Quit(); return -1; }

        std::cout << "OpenGL: " << context.GetVersion() << '\n';

        Renderer2D renderer(&context);
        InputManager input;
        AppState state;

        InitSearchPaths();
        LoadEssentialResources(state);

        Uint32 last = SDL_GetTicks();

        while (state.running) {
            Uint32 now = SDL_GetTicks();
            float dt = (now - last) / 1000.0f;
            last = now;
            state.time += dt;

            // FPS counter
            state.frameCount++;
            state.fpsTimer += dt;
            if (state.fpsTimer >= 1.0f) {
                state.fps = state.frameCount / state.fpsTimer;
                state.frameTime = (state.fpsTimer / state.frameCount) * 1000.0f;
                state.frameCount = 0;
                state.fpsTimer = 0;
            }

            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) state.running = false;
                input.ProcessEvent(ev);
            }
            input.Update();

            HandleInput(state, input);
            ResourceManager::GetInstance().Update();

            renderer.BeginFrame();
            renderer.Clear(Color(0.08f, 0.08f, 0.12f));

            switch (state.selectedDemo) {
                case 0: Demo1_BasicLoading(renderer, state);     break;
                case 1: Demo2_MemoryStats(renderer, state);      break;
                case 2: Demo3_CachePerformance(renderer, state); break;
                case 3: Demo4_AutoManagement(renderer, state);   break;
            }

            if (!state.fonts.empty()) {
                if (state.showHelp)  ShowHelp(renderer, state.fonts[0]);
                if (state.showStats) ShowStats(renderer, state.fonts[0], state);
            }

            renderer.EndFrame();
            context.SwapBuffers();
        }

        std::cout << "\n=== إحصائيات / Stats ===\n";
        ResourceManager::GetInstance().PrintStats();
    } catch (const std::exception& e) {
        std::cerr << "خطأ / Error: " << e.what() << '\n';
        SDL_Quit();
        return -1;
    }

    SDL_Quit();
    std::cout << "انتهى / Done\n";
    return 0;
}
