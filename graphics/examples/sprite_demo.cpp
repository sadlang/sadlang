// ==============================================================================
// sprite_demo.cpp - مثال متقدم لرسم الـ Sprites والصور
// Advanced sprite/image rendering demo with bouncing, rotation and tinting
// ==============================================================================

#include "../include/window/window.h"
#include "../include/rendering/context.h"
#include "../include/rendering/renderer2d.h"
#include "../include/input/input_manager.h"
#include "../include/texture/texture.h"
#include <SDL.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
#ifdef DrawText
#undef DrawText
#endif

#include <iostream>
#include <memory>
#include <cmath>
#include <vector>
#include <cstdlib>

using namespace sad::graphics;
using namespace SadGraphics;

// ==============================================================================
struct Sprite {
    TextureRef texture;
    Float32 x, y, w, h;
    Float32 rotation     = 0.0f;
    Float32 scale        = 1.0f;
    Color   tint         = Color::White;
    Float32 vx = 0, vy = 0;
    Float32 rotSpeed     = 0.0f;
};

struct AppState {
    bool running   = true;
    bool paused    = false;
    bool showGrid  = true;
    bool showBounds= false;
    Float32 time   = 0.0f;
    Float32 dt     = 0.0f;
    std::vector<Sprite> sprites;
    TextureRef defaultTex;
};

// ==============================================================================
static Sprite MakeSprite(const TextureRef& tex,
                          Float32 x, Float32 y, Float32 sz) {
    Sprite s;
    s.texture   = tex;
    s.x = x; s.y = y; s.w = sz; s.h = sz;
    s.vx        = ((rand() % 200) - 100) / 10.0f;
    s.vy        = ((rand() % 200) - 100) / 10.0f;
    s.rotSpeed  = ((rand() % 100) - 50) / 50.0f;
    return s;
}

static void UpdateSprite(Sprite& sp, Float32 dt, int W, int H) {
    sp.x += sp.vx * dt * 60.0f;
    sp.y += sp.vy * dt * 60.0f;
    sp.rotation += sp.rotSpeed * dt;
    Float32 hw = sp.w * sp.scale * 0.5f;
    Float32 hh = sp.h * sp.scale * 0.5f;
    if (sp.x - hw < 0 || sp.x + hw > W) { sp.vx = -sp.vx; sp.x = sp.x - hw < 0 ? hw : W - hw; }
    if (sp.y - hh < 0 || sp.y + hh > H) { sp.vy = -sp.vy; sp.y = sp.y - hh < 0 ? hh : H - hh; }
}

static void DrawSprite(Renderer2D& r, const Sprite& sp) {
    if (!sp.texture || !sp.texture->IsValid()) return;
    Float32 sw = sp.w * sp.scale, sh = sp.h * sp.scale;
    Float32 dx = sp.x - sw * 0.5f, dy = sp.y - sh * 0.5f;
    if (sp.rotation != 0.0f)
        r.DrawTextureRotated(sp.texture, dx, dy, sw, sh, sp.rotation, 0.5f, 0.5f, sp.tint);
    else
        r.DrawTexture(sp.texture, dx, dy, sw, sh, sp.tint);
}

// ==============================================================================
static void ProcessInput(InputManager& in, AppState& st, int W, int H) {
    if (in.IsKeyPressed(KeyCode::Escape)) st.running = false;
    if (in.IsKeyPressed(KeyCode::Space))  st.paused  = !st.paused;
    if (in.IsKeyPressed(KeyCode::G))      st.showGrid = !st.showGrid;
    if (in.IsKeyPressed(KeyCode::B))      st.showBounds = !st.showBounds;

    if (in.IsKeyPressed(KeyCode::A) && st.defaultTex) {
        Sprite sp = MakeSprite(st.defaultTex, W * 0.5f, H * 0.5f, 50.0f + rand() % 80);
        float r_ = (float)(rand() % 256) / 255.0f;
        float g_ = (float)(rand() % 256) / 255.0f;
        float b_ = (float)(rand() % 256) / 255.0f;
        sp.tint = Color(r_, g_, b_);
        st.sprites.push_back(sp);
        std::cout << "Sprites: " << st.sprites.size() << '\n';
    }
    if (in.IsKeyPressed(KeyCode::C)) { st.sprites.clear(); std::cout << "Cleared\n"; }
    if (in.IsKeyPressed(KeyCode::R) && !st.sprites.empty()) {
        auto& sp = st.sprites[rand() % st.sprites.size()];
        sp.tint = Color((float)(rand()%256)/255.0f, (float)(rand()%256)/255.0f, (float)(rand()%256)/255.0f);
    }
}

// ==============================================================================
static void RenderFrame(Renderer2D& renderer, const AppState& st, int W, int H) {
    renderer.BeginFrame();
    renderer.Clear(Color(0.12f, 0.12f, 0.16f));

    // شبكة / Grid
    if (st.showGrid) {
        Color gc(0.2f, 0.2f, 0.25f, 0.5f);
        for (float x = 0; x < W; x += 50) renderer.DrawLine(x, 0, x, (float)H, gc, 1.0f);
        for (float y = 0; y < H; y += 50) renderer.DrawLine(0, y, (float)W, y, gc, 1.0f);
    }

    // رسم الـ sprites / Draw sprites
    for (auto& sp : st.sprites) {
        DrawSprite(renderer, sp);
        if (st.showBounds) {
            Float32 sw = sp.w * sp.scale, sh = sp.h * sp.scale;
            renderer.DrawRect(sp.x - sw*0.5f, sp.y - sh*0.5f, sw, sh, Color::Yellow, false);
            renderer.DrawCircle(sp.x, sp.y, 3, Color::Red, true, 8);
        }
    }

    // لوحة معلومات / Info panel
    renderer.DrawRect(10, 10, 200, 50, Color(0,0,0,0.7f), true);
    renderer.DrawRect(10, 10, 200, 50, Color::White, false);

    renderer.EndFrame();
}

// ==============================================================================
int main(int /*argc*/, char* /*argv*/[]) {
    std::cout << "\n=== Sad Graphics - Sprite Demo ===\n"
              << "A=إضافة C=مسح R=لون Space=إيقاف G=شبكة B=حدود ESC=خروج\n\n";

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init: " << SDL_GetError() << '\n'; return -1;
    }

    try {
        constexpr int W = 1024, H = 768;
        Window window("Sad Graphics - Sprite Demo", W, H, WindowFlags::Resizable);
        if (!window.IsOpen()) { SDL_Quit(); return -1; }

        RenderContext context(window.GetNativeWindow());
        if (!context.IsValid()) { SDL_Quit(); return -1; }

        Renderer2D renderer(&context);
        InputManager input;
        AppState state;

        state.defaultTex = Texture::CreateWhite();

        // 5 sprites ابتدائية / 5 initial sprites
        for (int i = 0; i < 5; i++) {
            Float32 sz = 50.0f + i * 20.0f;
            Sprite sp = MakeSprite(state.defaultTex, 200.0f + i * 150, 300.0f + (i%2)*100, sz);
            sp.tint = Color(0.2f + i*0.15f, 0.4f + i*0.12f, 0.8f - i*0.12f);
            state.sprites.push_back(sp);
        }

        Uint32 last = SDL_GetTicks();
        while (state.running) {
            Uint32 now = SDL_GetTicks();
            state.dt = (now - last) / 1000.0f;
            last = now; state.time += state.dt;

            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) state.running = false;
                input.ProcessEvent(ev);
            }
            input.Update();
            ProcessInput(input, state, W, H);

            if (!state.paused)
                for (auto& sp : state.sprites)
                    UpdateSprite(sp, state.dt, W, H);

            RenderFrame(renderer, state, W, H);
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