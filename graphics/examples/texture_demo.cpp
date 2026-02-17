// ==============================================================================
// texture_demo.cpp - مثال توضيحي لنظام الـ Textures
// Texture system demonstration - loading, drawing, and transforming textures
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
#include <cstdlib>

using namespace sad::graphics;
using namespace SadGraphics;

// ==============================================================================
struct AppState {
    bool     running   = true;
    Float32  time      = 0.0f;
    Float32  dt        = 0.0f;

    TextureRef mainTex;
    TextureRef whiteTex;

    Float32 imgX      = 400.0f, imgY = 300.0f;
    Float32 imgW      = 200.0f, imgH = 200.0f;
    Float32 rotation  = 0.0f;
    Float32 scale     = 1.0f;
    Color   bgColor{0.2f, 0.3f, 0.4f, 1.0f};
};

// ==============================================================================
static void ProcessInput(InputManager& input, AppState& s) {
    if (input.IsKeyPressed(KeyCode::Escape)) s.running = false;

    Float32 spd = 250.0f * s.dt;
    if (input.IsKeyHeld(KeyCode::ArrowUp))    s.imgY -= spd;
    if (input.IsKeyHeld(KeyCode::ArrowDown))  s.imgY += spd;
    if (input.IsKeyHeld(KeyCode::ArrowLeft))  s.imgX -= spd;
    if (input.IsKeyHeld(KeyCode::ArrowRight)) s.imgX += spd;

    if (input.IsKeyHeld(KeyCode::Q)) s.rotation += 90.0f * s.dt;
    if (input.IsKeyHeld(KeyCode::E)) s.rotation -= 90.0f * s.dt;

    if (input.IsKeyHeld(KeyCode::W)) s.scale += s.dt;
    if (input.IsKeyHeld(KeyCode::S)) s.scale -= s.dt;
    if (s.scale < 0.1f) s.scale = 0.1f;
    if (s.scale > 5.0f) s.scale = 5.0f;

    if (input.IsKeyPressed(KeyCode::R)) {
        s.imgX = 400; s.imgY = 300; s.rotation = 0; s.scale = 1;
    }
    if (input.IsKeyPressed(KeyCode::Space)) {
        s.bgColor = Color((float)rand()/RAND_MAX,
                          (float)rand()/RAND_MAX,
                          (float)rand()/RAND_MAX);
    }
    if (input.IsMouseButtonHeld(MouseButton::Left)) {
        Point2D m = input.GetMousePosition();
        s.imgX = m.x; s.imgY = m.y;
    }
}

// ==============================================================================
static void RenderFrame(Renderer2D& renderer, const AppState& s) {
    renderer.BeginFrame();
    renderer.Clear(s.bgColor);

    // شبكة / Grid
    Color grid(0.3f, 0.3f, 0.3f, 0.3f);
    for (float x = 0; x < 1024; x += 50)
        renderer.DrawLine(x, 0, x, 768, grid, 1.0f);
    for (float y = 0; y < 768; y += 50)
        renderer.DrawLine(0, y, 1024, y, grid, 1.0f);

    // رسم texture مع دوران / Draw texture with rotation
    if (s.mainTex && s.mainTex->IsValid()) {
        Float32 fw = s.imgW * s.scale;
        Float32 fh = s.imgH * s.scale;

        // ظل / Shadow
        renderer.DrawRect(s.imgX + 5, s.imgY + 5, fw, fh,
                          Color(0, 0, 0, 0.3f), true);

        renderer.DrawTextureRotated(s.mainTex,
                                    s.imgX, s.imgY, fw, fh,
                                    s.rotation, 0.5f, 0.5f, Color::White);

        // إطار / Frame
        renderer.DrawRect(s.imgX, s.imgY, fw, fh, Color::White, false);
    }

    // texture أبيض مرجعي / Reference white texture
    if (s.whiteTex && s.whiteTex->IsValid())
        renderer.DrawTexture(s.whiteTex, 30, 30, 24, 24, Color::White);

    // رسم تأثيرات متنوعة / Various texture effects
    if (s.mainTex && s.mainTex->IsValid()) {
        // صف من نسخ بألوان مختلفة / Row of tinted copies
        Color tints[] = { Color::Red, Color::Green, Color::Blue,
                          Color::Yellow, Color::Cyan };
        for (int i = 0; i < 5; i++)
            renderer.DrawTexture(s.mainTex, 100.0f + i * 110.0f, 600, 90, 90, tints[i]);

        // جزء من الصورة / Texture region
        renderer.DrawTextureRegion(s.mainTex,
            700, 600, 120, 90,   // dest
            0, 0, 0.5f, 0.5f,   // src (top-left quarter)
            Color::White);
    }

    renderer.EndFrame();
}

// ==============================================================================
int main(int /*argc*/, char* /*argv*/[]) {
    std::cout << "\n=== Sad Graphics - Texture Demo ===\n"
              << "ESC=خروج  Arrows=تحريك  Q/E=دوران  W/S=تكبير  R=إعادة  Space=خلفية\n\n";

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init فشل: " << SDL_GetError() << '\n';
        return -1;
    }

    try {
        Window window("Sad Graphics - Texture Demo", 1024, 768, WindowFlags::Resizable);
        if (!window.IsOpen()) { SDL_Quit(); return -1; }

        RenderContext context(window.GetNativeWindow());
        if (!context.IsValid()) { SDL_Quit(); return -1; }

        std::cout << "OpenGL: " << context.GetVersion() << '\n';

        Renderer2D renderer(&context);
        InputManager input;
        AppState state;

        state.whiteTex = Texture::CreateWhite();
        state.mainTex  = Texture::CreateFromFile("test.png");
        if (!state.mainTex) {
            std::cout << "test.png غير موجود، استخدام texture أبيض\n";
            state.mainTex = Texture::CreateWhite();
        } else {
            state.imgW = (Float32)state.mainTex->GetWidth();
            state.imgH = (Float32)state.mainTex->GetHeight();
        }

        Uint32 last = SDL_GetTicks();
        while (state.running) {
            Uint32 now = SDL_GetTicks();
            state.dt = (now - last) / 1000.0f;
            last = now;
            state.time += state.dt;

            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_QUIT) state.running = false;
                input.ProcessEvent(ev);
            }
            input.Update();
            ProcessInput(input, state);
            RenderFrame(renderer, state);
            context.SwapBuffers();
        }

        renderer.PrintStats();
    } catch (const std::exception& e) {
        std::cerr << "خطأ: " << e.what() << '\n';
        SDL_Quit();
        return -1;
    }

    SDL_Quit();
    return 0;
}