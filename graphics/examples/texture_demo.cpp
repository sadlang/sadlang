// texture_demo.cpp - مثال توضيحي لنظام الـ Textures
// Texture system demonstration example
// يوضح كيفية تحميل واستخدام الصور كـ textures مع الرسم ثنائي الأبعاد
// Demonstrates how to load and use images as textures with 2D rendering

#include "../include/window/window.h"
#include "../include/rendering/context.h"
#include "../include/rendering/renderer2d.h"
#include "../include/input/input_manager.h"
#include "../include/texture/texture.h"
#include "../include/texture/image.h"

#include <iostream>
#include <memory>
#include <cmath>

using namespace sad::graphics;

// حالة التطبيق - Application state
struct AppState {
    bool showHelp = true;           // عرض التعليمات - show help
    bool running = true;            // التطبيق يعمل - application running
    
    // معلومات الصورة المحملة - loaded image info
    TextureRef testTexture;         // الـ texture الأساسي - main texture
    TextureRef whiteTexture;        // texture أبيض للاختبار - white texture for testing
    
    // موقع وحجم الصورة - image position and size
    Vec2 imagePos = Vec2(400.0f, 300.0f);
    Vec2 imageSize = Vec2(200.0f, 200.0f);
    float rotation = 0.0f;          // دوران الصورة - image rotation
    float scale = 1.0f;             // مقياس الصورة - image scale
    
    // لون الخلفية - background color
    Vec4 bgColor = Vec4(0.2f, 0.3f, 0.4f, 1.0f);
    
    // معلومات الأداء - performance info
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
    u32 frameCount = 0;
    float fps = 0.0f;
};

// معالجة الإدخال - Process input
void ProcessInput(InputManager& input, AppState& state, Window& window) {
    // الخروج - Exit
    if (input.IsKeyPressed(KeyCode::Escape)) {
        state.running = false;
    }
    
    // تبديل عرض التعليمات - Toggle help
    if (input.IsKeyPressed(KeyCode::H)) {
        state.showHelp = !state.showHelp;
    }
    
    // تحريك الصورة بالأسهم - Move image with arrows
    float speed = 200.0f * state.deltaTime;
    if (input.IsKeyHeld(KeyCode::Up)) state.imagePos.y += speed;
    if (input.IsKeyHeld(KeyCode::Down)) state.imagePos.y -= speed;
    if (input.IsKeyHeld(KeyCode::Left)) state.imagePos.x -= speed;
    if (input.IsKeyHeld(KeyCode::Right)) state.imagePos.x += speed;
    
    // دوران الصورة - Rotate image
    if (input.IsKeyHeld(KeyCode::Q)) state.rotation += 90.0f * state.deltaTime;
    if (input.IsKeyHeld(KeyCode::E)) state.rotation -= 90.0f * state.deltaTime;
    
    // تكبير/تصغير الصورة - Scale image
    if (input.IsKeyHeld(KeyCode::W)) state.scale += 1.0f * state.deltaTime;
    if (input.IsKeyHeld(KeyCode::S)) state.scale -= 1.0f * state.deltaTime;
    if (state.scale < 0.1f) state.scale = 0.1f;
    if (state.scale > 5.0f) state.scale = 5.0f;
    
    // إعادة تعيين - Reset
    if (input.IsKeyPressed(KeyCode::R)) {
        state.imagePos = Vec2(400.0f, 300.0f);
        state.rotation = 0.0f;
        state.scale = 1.0f;
    }
    
    // تغيير لون الخلفية - Change background color
    if (input.IsKeyPressed(KeyCode::Space)) {
        state.bgColor.x = static_cast<float>(rand()) / RAND_MAX;
        state.bgColor.y = static_cast<float>(rand()) / RAND_MAX;
        state.bgColor.z = static_cast<float>(rand()) / RAND_MAX;
    }
    
    // تحديث موقع الصورة بالماوس (عند الضغط) - Update image position with mouse (when pressed)
    if (input.IsMouseButtonHeld(MouseButton::Left)) {
        Vec2 mousePos = input.GetMousePosition();
        state.imagePos = mousePos;
    }
}

// رسم التعليمات - Render help text
void RenderHelp(Renderer2D& renderer) {
    // خلفية التعليمات - help background
    renderer.DrawFilledRect(
        Vec2(10.0f, 550.0f),
        Vec2(380.0f, 180.0f),
        Vec4(0.0f, 0.0f, 0.0f, 0.7f)
    );
    
    // ملاحظة: الرسم النصي سيُضاف لاحقاً مع نظام الخطوط
    // Note: Text rendering will be added later with font system
    // في الوقت الحالي، نعتمد على console output
    // For now, we rely on console output
}

// رسم معلومات الأداء - Render performance info
void RenderStats(Renderer2D& renderer, const AppState& state) {
    // خلفية المعلومات - stats background
    renderer.DrawFilledRect(
        Vec2(10.0f, 10.0f),
        Vec2(200.0f, 100.0f),
        Vec4(0.0f, 0.0f, 0.0f, 0.7f)
    );
}

// الرسم - Render
void Render(Renderer2D& renderer, const AppState& state, const Vec2& windowSize) {
    // تنظيف الشاشة - clear screen
    renderer.Clear(state.bgColor);
    
    // بداية الرسم - begin rendering
    renderer.Begin();
    
    // رسم شبكة للخلفية - draw background grid
    float gridSize = 50.0f;
    Vec4 gridColor(0.3f, 0.3f, 0.3f, 0.3f);
    
    for (float x = 0; x < windowSize.x; x += gridSize) {
        renderer.DrawLine(
            Vec2(x, 0.0f),
            Vec2(x, windowSize.y),
            gridColor,
            1.0f
        );
    }
    
    for (float y = 0; y < windowSize.y; y += gridSize) {
        renderer.DrawLine(
            Vec2(0.0f, y),
            Vec2(windowSize.x, y),
            gridColor,
            1.0f
        );
    }
    
    // رسم الصورة المحملة مع التحولات - draw loaded image with transformations
    if (state.testTexture && state.testTexture->IsValid()) {
        // حساب الحجم النهائي مع المقياس - calculate final size with scale
        Vec2 finalSize = state.imageSize * state.scale;
        
        // رسم ظل خفيف - draw subtle shadow
        Vec2 shadowOffset(5.0f, -5.0f);
        renderer.DrawFilledRect(
            state.imagePos + shadowOffset,
            finalSize,
            Vec4(0.0f, 0.0f, 0.0f, 0.3f),
            state.rotation
        );
        
        // رسم الصورة الحقيقية مع دوران - draw actual image with rotation
        renderer.DrawTextureRotated(
            state.testTexture,
            state.imagePos.x,
            state.imagePos.y,
            finalSize.x,
            finalSize.y,
            state.rotation,
            0.5f, 0.5f,  // مركز الدوران - rotation center
            Vec4(1.0f, 1.0f, 1.0f, 1.0f)  // لون أبيض (بدون تلوين) - white (no tint)
        );
        
        // رسم إطار حول الصورة - draw border around image
        renderer.DrawRect(
            state.imagePos,
            finalSize,
            Vec4(1.0f, 1.0f, 1.0f, 1.0f),
            3.0f,
            state.rotation
        );
    }
    
    // رسم texture أبيض في الزاوية - draw white texture in corner
    if (state.whiteTexture && state.whiteTexture->IsValid()) {
        renderer.DrawTexture(
            state.whiteTexture,
            50.0f, 50.0f,
            30.0f, 30.0f,
            Vec4(1.0f, 1.0f, 1.0f, 1.0f)
        );
    }
    
    // رسم معلومات الأداء - render performance stats
    RenderStats(renderer, state);
    
    // رسم التعليمات إذا كانت مفعّلة - render help if enabled
    if (state.showHelp) {
        RenderHelp(renderer);
    }
    
    // نهاية الرسم - end rendering
    renderer.End();
}

// النقطة الرئيسية للبرنامج - Main entry point
int main(int argc, char* argv[]) {
    std::cout << "=== Sad Graphics Library - Texture Demo ===" << std::endl;
    std::cout << "Arabic/English Comments - Professional Code Example" << std::endl;
    std::cout << std::endl;
    
    // إنشاء النافذة - Create window
    WindowSpec windowSpec;
    windowSpec.title = "Texture Demo - عرض الـ Textures";
    windowSpec.width = 800;
    windowSpec.height = 600;
    windowSpec.resizable = true;
    windowSpec.vSync = true;
    
    Window window(windowSpec);
    if (!window.IsOpen()) {
        std::cerr << "فشل في إنشاء النافذة / Failed to create window" << std::endl;
        return -1;
    }
    
    std::cout << "✓ تم إنشاء النافذة بنجاح / Window created successfully" << std::endl;
    
    // إنشاء سياق OpenGL - Create OpenGL context
    Context context(window);
    if (!context.IsValid()) {
        std::cerr << "فشل في إنشاء سياق OpenGL / Failed to create OpenGL context" << std::endl;
        return -1;
    }
    
    std::cout << "✓ تم إنشاء سياق OpenGL بنجاح / OpenGL context created successfully" << std::endl;
    
    // إنشاء المُرسم ثنائي الأبعاد - Create 2D renderer
    Renderer2D renderer;
    if (!renderer.Initialize()) {
        std::cerr << "فشل في تهيئة المُرسم / Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    std::cout << "✓ تم تهيئة المُرسم بنجاح / Renderer initialized successfully" << std::endl;
    
    // إنشاء مدير الإدخال - Create input manager
    InputManager input;
    
    std::cout << "✓ تم إنشاء مدير الإدخال / Input manager created" << std::endl;
    
    // إنشاء حالة التطبيق - Create application state
    AppState state;
    
    // إنشاء textures اختبارية - Create test textures
    state.whiteTexture = Texture::CreateWhite();
    std::cout << "✓ تم إنشاء texture أبيض / White texture created" << std::endl;
    
    // محاولة تحميل صورة اختبارية - Try to load a test image
    // ملاحظة: ضع صورة test.png في مجلد البرنامج للاختبار
    // Note: Place a test.png image in the program folder for testing
    state.testTexture = Texture::CreateFromFile("test.png", true, false);
    if (state.testTexture) {
        std::cout << "✓ تم تحميل test.png بنجاح / test.png loaded successfully" << std::endl;
        state.imageSize = Vec2(
            static_cast<float>(state.testTexture->GetWidth()),
            static_cast<float>(state.testTexture->GetHeight())
        );
    } else {
        std::cout << "⚠ لم يُعثر على test.png، استخدام texture افتراضي / test.png not found, using default texture" << std::endl;
        state.testTexture = Texture::CreateWhite();
        state.imageSize = Vec2(200.0f, 200.0f);
    }
    
    // طباعة التعليمات - Print instructions
    std::cout << "\n=== التحكم / Controls ===" << std::endl;
    std::cout << "  ESC       - خروج / Exit" << std::endl;
    std::cout << "  H         - إخفاء/إظهار التعليمات / Toggle help" << std::endl;
    std::cout << "  ↑↓←→      - تحريك الصورة / Move image" << std::endl;
    std::cout << "  Q/E       - دوران الصورة / Rotate image" << std::endl;
    std::cout << "  W/S       - تكبير/تصغير الصورة / Scale image" << std::endl;
    std::cout << "  R         - إعادة تعيين / Reset" << std::endl;
    std::cout << "  Space     - تغيير لون الخلفية / Change background" << std::endl;
    std::cout << "  Left Click - نقل الصورة للماوس / Move image to mouse" << std::endl;
    std::cout << std::endl;
    
    // حلقة التطبيق الرئيسية - Main application loop
    u32 lastTime = SDL_GetTicks();
    u32 fpsTimer = lastTime;
    u32 fpsFrames = 0;
    
    while (state.running && window.IsOpen()) {
        // حساب الوقت المنقضي - calculate delta time
        u32 currentTime = SDL_GetTicks();
        state.deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        state.totalTime += state.deltaTime;
        state.frameCount++;
        
        // حساب FPS - calculate FPS
        fpsFrames++;
        if (currentTime - fpsTimer >= 1000) {
            state.fps = fpsFrames / ((currentTime - fpsTimer) / 1000.0f);
            fpsFrames = 0;
            fpsTimer = currentTime;
        }
        
        // معالجة الأحداث - process events
        input.Update(); // تحديث الحالة السابقة - update previous state
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                state.running = false;
            }
            
            input.ProcessEvent(event); // معالجة الحدث - process event
        }
        
        // معالجة الإدخال - process input
        ProcessInput(input, state, window);
        
        // تحديث viewport عند تغيير حجم النافذة - update viewport on window resize
        i32 width, height;
        window.GetSize(width, height);
        Vec2 windowSize(static_cast<float>(width), static_cast<float>(height));
        renderer.SetViewport(0, 0, width, height);
        
        // الرسم - render
        Render(renderer, state, windowSize);
        
        // عرض النتيجة - present result
        window.SwapBuffers();
    }
    
    // تنظيف الموارد - cleanup resources
    std::cout << "\n=== الإحصائيات النهائية / Final Statistics ===" << std::endl;
    std::cout << "  إجمالي الإطارات / Total frames: " << state.frameCount << std::endl;
    std::cout << "  إجمالي الوقت / Total time: " << state.totalTime << " seconds" << std::endl;
    std::cout << "  متوسط FPS / Average FPS: " << (state.frameCount / state.totalTime) << std::endl;
    
    renderer.Shutdown();
    
    std::cout << "\nتم إغلاق التطبيق بنجاح / Application closed successfully" << std::endl;
    
    return 0;
}
