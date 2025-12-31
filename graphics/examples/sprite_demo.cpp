// ==============================================================================
// sprite_demo.cpp - مثال متقدم لرسم الـ Sprites والصور
// Advanced sprite and image rendering demo
// ==============================================================================
// الوصف: يوضح كيفية رسم الصور والـ sprites باستخدام Renderer2D والـ Texture system
// Description: Demonstrates image and sprite rendering using Renderer2D and Texture system
// ==============================================================================

#include "../include/window/window.h"           // نظام النوافذ / Window system
#include "../include/rendering/context.h"       // سياق الرسم / Render context
#include "../include/rendering/renderer2d.h"    // محرك الرسم 2D / 2D rendering engine
#include "../include/input/input_manager.h"     // نظام الإدخال / Input system
#include "../include/texture/texture.h"         // نظام الـ textures / Texture system
#include "../include/texture/image.h"           // تحميل الصور / Image loading

#include <iostream>                             // للإدخال والإخراج / For I/O
#include <memory>                               // للمؤشرات الذكية / For smart pointers
#include <cmath>                                // للدوال الرياضية / For math functions
#include <vector>                               // للمصفوفات / For vectors

using namespace SadGraphics;                    // استخدام مساحة الأسماء / Using namespace

// ==============================================================================
// بيانات الـ Sprite / Sprite Data
// ==============================================================================
struct Sprite {
    sad::graphics::TextureRef texture;          // الـ texture / Texture
    Float32 x, y;                               // الموقع / Position
    Float32 width, height;                      // الأبعاد / Dimensions
    Float32 rotation;                           // الدوران (راديان) / Rotation (radians)
    Float32 scale;                              // المقياس / Scale
    Color tint;                                 // لون التلوين / Tint color
    Float32 velocityX, velocityY;               // السرعة / Velocity
    Float32 rotationSpeed;                      // سرعة الدوران / Rotation speed
};

// ==============================================================================
// حالة التطبيق / Application State
// ==============================================================================
struct AppState {
    bool running;                               // التطبيق يعمل / Application running
    bool showHelp;                              // عرض التعليمات / Show help
    bool paused;                                // مؤقت / Paused
    
    std::vector<Sprite> sprites;                // قائمة الـ sprites / Sprite list
    sad::graphics::TextureRef defaultTexture;   // الـ texture الافتراضي / Default texture
    
    Float32 time;                               // الوقت الإجمالي / Total time
    Float32 deltaTime;                          // وقت الإطار / Frame time
    UInt32 frameCount;                          // عدد الإطارات / Frame count
    Float32 fps;                                // الإطارات في الثانية / Frames per second
    
    // خيارات العرض / Display options
    bool showGrid;                              // عرض الشبكة / Show grid
    bool showBounds;                            // عرض الحدود / Show bounds
    
    AppState() 
        : running(true)                         // البدء بالتشغيل / Start running
        , showHelp(true)                        // عرض التعليمات في البداية / Show help initially
        , paused(false)                         // غير مؤقت / Not paused
        , time(0.0f)                            // الوقت صفر / Zero time
        , deltaTime(0.0f)                       // دلتا صفر / Zero delta
        , frameCount(0)                         // عدد الإطارات صفر / Zero frames
        , fps(0.0f)                             // FPS صفر / Zero FPS
        , showGrid(true)                        // عرض الشبكة / Show grid
        , showBounds(false)                     // إخفاء الحدود / Hide bounds
    {}
};

// ==============================================================================
// دوال مساعدة / Helper Functions
// ==============================================================================

/// إنشاء sprite جديد / Create new sprite
Sprite CreateSprite(const sad::graphics::TextureRef& texture, 
                    Float32 x, Float32 y, Float32 width, Float32 height) {
    Sprite sprite;                              // إنشاء sprite جديد / Create new sprite
    sprite.texture = texture;                   // تعيين الـ texture / Set texture
    sprite.x = x;                               // تعيين الموقع X / Set position X
    sprite.y = y;                               // تعيين الموقع Y / Set position Y
    sprite.width = width;                       // تعيين العرض / Set width
    sprite.height = height;                     // تعيين الارتفاع / Set height
    sprite.rotation = 0.0f;                     // بدون دوران / No rotation
    sprite.scale = 1.0f;                        // مقياس 1 / Scale 1
    sprite.tint = Color::White;                 // بدون تلوين / No tint
    
    // سرعة عشوائية / Random velocity
    sprite.velocityX = ((rand() % 200) - 100) / 10.0f;  // من -10 إلى 10 / From -10 to 10
    sprite.velocityY = ((rand() % 200) - 100) / 10.0f;  // من -10 إلى 10 / From -10 to 10
    sprite.rotationSpeed = ((rand() % 100) - 50) / 50.0f;  // من -1 إلى 1 / From -1 to 1
    
    return sprite;                              // إرجاع الـ sprite / Return sprite
}

/// تحديث sprite / Update sprite
void UpdateSprite(Sprite& sprite, Float32 deltaTime, Int32 screenWidth, Int32 screenHeight) {
    if (sprite.velocityX == 0.0f && sprite.velocityY == 0.0f) {
        return;                                 // لا حركة / No movement
    }
    
    // تحديث الموقع / Update position
    sprite.x += sprite.velocityX * deltaTime * 60.0f;  // تحريك X / Move X
    sprite.y += sprite.velocityY * deltaTime * 60.0f;  // تحريك Y / Move Y
    
    // تحديث الدوران / Update rotation
    sprite.rotation += sprite.rotationSpeed * deltaTime;  // دوران / Rotate
    
    // الارتداد من الحواف / Bounce off edges
    Float32 halfWidth = sprite.width * sprite.scale * 0.5f;   // نصف العرض / Half width
    Float32 halfHeight = sprite.height * sprite.scale * 0.5f; // نصف الارتفاع / Half height
    
    if (sprite.x - halfWidth < 0.0f || sprite.x + halfWidth > screenWidth) {
        sprite.velocityX = -sprite.velocityX;   // عكس الاتجاه / Reverse direction
        sprite.x = sprite.x - halfWidth < 0.0f ? halfWidth : screenWidth - halfWidth;
    }
    
    if (sprite.y - halfHeight < 0.0f || sprite.y + halfHeight > screenHeight) {
        sprite.velocityY = -sprite.velocityY;   // عكس الاتجاه / Reverse direction
        sprite.y = sprite.y - halfHeight < 0.0f ? halfHeight : screenHeight - halfHeight;
    }
}

/// رسم sprite / Draw sprite
void DrawSprite(Renderer2D& renderer, const Sprite& sprite) {
    if (!sprite.texture || !sprite.texture->IsValid()) {
        return;                                 // texture غير صالح / Invalid texture
    }
    
    // حساب الأبعاد مع المقياس / Calculate dimensions with scale
    Float32 scaledWidth = sprite.width * sprite.scale;   // العرض المقاس / Scaled width
    Float32 scaledHeight = sprite.height * sprite.scale; // الارتفاع المقاس / Scaled height
    
    // حساب الموقع (مع مراعاة المركز) / Calculate position (accounting for center)
    Float32 drawX = sprite.x - scaledWidth * 0.5f;      // موقع الرسم X / Draw position X
    Float32 drawY = sprite.y - scaledHeight * 0.5f;     // موقع الرسم Y / Draw position Y
    
    // رسم مع دوران / Draw with rotation
    if (sprite.rotation != 0.0f) {
        renderer.DrawTextureRotated(
            sprite.texture,                     // الـ texture / Texture
            drawX, drawY,                       // الموقع / Position
            scaledWidth, scaledHeight,          // الأبعاد / Dimensions
            sprite.rotation,                    // الدوران / Rotation
            0.5f, 0.5f,                         // نقطة الدوران (المركز) / Rotation origin (center)
            sprite.tint                         // اللون / Tint
        );
    } else {
        renderer.DrawTexture(
            sprite.texture,                     // الـ texture / Texture
            drawX, drawY,                       // الموقع / Position
            scaledWidth, scaledHeight,          // الأبعاد / Dimensions
            sprite.tint                         // اللون / Tint
        );
    }
}

/// رسم حدود sprite / Draw sprite bounds
void DrawSpriteBounds(Renderer2D& renderer, const Sprite& sprite) {
    Float32 scaledWidth = sprite.width * sprite.scale;   // العرض المقاس / Scaled width
    Float32 scaledHeight = sprite.height * sprite.scale; // الارتفاع المقاس / Scaled height
    Float32 drawX = sprite.x - scaledWidth * 0.5f;      // موقع الرسم X / Draw position X
    Float32 drawY = sprite.y - scaledHeight * 0.5f;     // موقع الرسم Y / Draw position Y
    
    // رسم مستطيل الحدود / Draw bounding rectangle
    renderer.DrawRect(
        drawX, drawY,                           // الموقع / Position
        scaledWidth, scaledHeight,              // الأبعاد / Dimensions
        Color::Yellow,                          // لون أصفر / Yellow color
        false                                   // فارغ / Outline only
    );
    
    // رسم نقطة المركز / Draw center point
    renderer.DrawCircle(
        sprite.x, sprite.y,                     // المركز / Center
        3.0f,                                   // نصف القطر / Radius
        Color::Red,                             // لون أحمر / Red color
        true,                                   // ممتلئ / Filled
        8                                       // 8 أضلاع / 8 segments
    );
}

// ==============================================================================
// معالجة الإدخال / Input Processing
// ==============================================================================

/// معالجة إدخال لوحة المفاتيح / Process keyboard input
void ProcessInput(InputManager& inputManager, AppState& state, Int32 screenWidth, Int32 screenHeight) {
    // الخروج / Exit
    if (inputManager.IsKeyPressed(KeyCode::Escape)) {
        state.running = false;                  // إيقاف التطبيق / Stop application
    }
    
    // تبديل التعليمات / Toggle help
    if (inputManager.IsKeyPressed(KeyCode::H)) {
        state.showHelp = !state.showHelp;       // تبديل العرض / Toggle display
    }
    
    // تبديل الإيقاف المؤقت / Toggle pause
    if (inputManager.IsKeyPressed(KeyCode::Space)) {
        state.paused = !state.paused;           // تبديل الإيقاف / Toggle pause
    }
    
    // تبديل الشبكة / Toggle grid
    if (inputManager.IsKeyPressed(KeyCode::G)) {
        state.showGrid = !state.showGrid;       // تبديل الشبكة / Toggle grid
    }
    
    // تبديل الحدود / Toggle bounds
    if (inputManager.IsKeyPressed(KeyCode::B)) {
        state.showBounds = !state.showBounds;   // تبديل الحدود / Toggle bounds
    }
    
    // إضافة sprite جديد / Add new sprite
    if (inputManager.IsKeyPressed(KeyCode::A) && state.defaultTexture) {
        Float32 x = screenWidth * 0.5f;         // مركز الشاشة X / Screen center X
        Float32 y = screenHeight * 0.5f;        // مركز الشاشة Y / Screen center Y
        Float32 size = 50.0f + (rand() % 100);  // حجم عشوائي / Random size
        
        Sprite sprite = CreateSprite(state.defaultTexture, x, y, size, size);
        state.sprites.push_back(sprite);        // إضافة للقائمة / Add to list
        
        std::cout << "(AR) تم إضافة sprite جديد (العدد: " << state.sprites.size() << ")" << std::endl;
        std::cout << "(EN) Added new sprite (total: " << state.sprites.size() << ")" << std::endl;
    }
    
    // مسح جميع الـ sprites / Clear all sprites
    if (inputManager.IsKeyPressed(KeyCode::C)) {
        state.sprites.clear();                  // مسح القائمة / Clear list
        std::cout << "(AR) تم مسح جميع الـ sprites" << std::endl;
        std::cout << "(EN) Cleared all sprites" << std::endl;
    }
    
    // تغيير لون sprite عشوائي / Change random sprite color
    if (inputManager.IsKeyPressed(KeyCode::R) && !state.sprites.empty()) {
        Int32 index = rand() % state.sprites.size();  // اختيار عشوائي / Random selection
        state.sprites[index].tint = Color(
            rand() % 256,                       // أحمر عشوائي / Random red
            rand() % 256,                       // أخضر عشوائي / Random green
            rand() % 256,                       // أزرق عشوائي / Random blue
            255                                 // شفافية كاملة / Full alpha
        );
    }
}

// ==============================================================================
// الرسم / Rendering
// ==============================================================================

/// رسم الشبكة / Draw grid
void DrawGrid(Renderer2D& renderer, Int32 screenWidth, Int32 screenHeight) {
    const Float32 gridSize = 50.0f;             // حجم الخلية / Cell size
    const Color gridColor(128, 128, 128, 64);  // لون رمادي شفاف / Transparent gray
    
    // خطوط عمودية / Vertical lines
    for (Float32 x = 0.0f; x <= screenWidth; x += gridSize) {
        renderer.DrawLine(
            x, 0.0f,                            // نقطة البداية / Start point
            x, (Float32)screenHeight,           // نقطة النهاية / End point
            gridColor,                          // اللون / Color
            1.0f                                // السمك / Thickness
        );
    }
    
    // خطوط أفقية / Horizontal lines
    for (Float32 y = 0.0f; y <= screenHeight; y += gridSize) {
        renderer.DrawLine(
            0.0f, y,                            // نقطة البداية / Start point
            (Float32)screenWidth, y,            // نقطة النهاية / End point
            gridColor,                          // اللون / Color
            1.0f                                // السمك / Thickness
        );
    }
}

/// رسم معلومات الأداء / Draw performance info
void DrawStats(Renderer2D& renderer, const AppState& state) {
    // خلفية المعلومات / Info background
    renderer.DrawRect(
        10.0f, 10.0f,                           // الموقع / Position
        220.0f, 120.0f,                         // الأبعاد / Dimensions
        Color(0, 0, 0, 180),                    // لون أسود شفاف / Transparent black
        true                                    // ممتلئ / Filled
    );
    
    // إطار المعلومات / Info border
    renderer.DrawRect(
        10.0f, 10.0f,                           // الموقع / Position
        220.0f, 120.0f,                         // الأبعاد / Dimensions
        Color::White,                           // لون أبيض / White
        false                                   // فارغ / Outline
    );
    
    // ملاحظة: النص سيُضاف مع نظام الخطوط
    // Note: Text will be added with font system
}

/// رسم التعليمات / Draw help
void DrawHelp(Renderer2D& renderer, Int32 screenHeight) {
    // خلفية التعليمات / Help background
    renderer.DrawRect(
        10.0f, screenHeight - 210.0f,           // الموقع / Position
        300.0f, 200.0f,                         // الأبعاد / Dimensions
        Color(0, 0, 0, 200),                    // لون أسود شفاف / Transparent black
        true                                    // ممتلئ / Filled
    );
    
    // إطار التعليمات / Help border
    renderer.DrawRect(
        10.0f, screenHeight - 210.0f,           // الموقع / Position
        300.0f, 200.0f,                         // الأبعاد / Dimensions
        Color::Cyan,                            // لون سماوي / Cyan
        false                                   // فارغ / Outline
    );
    
    // ملاحظة: التعليمات النصية ستُضاف مع نظام الخطوط
    // Note: Text instructions will be added with font system
}

// ==============================================================================
// الدالة الرئيسية / Main Function
// ==============================================================================

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "(AR) مثال الـ Sprites المتقدم" << std::endl;
    std::cout << "(EN) Advanced Sprite Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // إنشاء النافذة / Create window
    Window window("(AR) مثال الـ Sprites / (EN) Sprite Demo", 1024, 768);
    if (!window.IsValid()) {
        std::cerr << "(AR) فشل إنشاء النافذة! / (EN) Failed to create window!" << std::endl;
        return -1;
    }
    
    // إنشاء سياق الرسم / Create render context
    RenderContext context(&window);
    if (!context.IsValid()) {
        std::cerr << "(AR) فشل إنشاء سياق الرسم! / (EN) Failed to create render context!" << std::endl;
        return -1;
    }
    
    // إنشاء محرك الرسم 2D / Create 2D renderer
    Renderer2D renderer(&context);
    
    // إنشاء مدير الإدخال / Create input manager
    InputManager inputManager(&window);
    
    // إنشاء حالة التطبيق / Create application state
    AppState state;
    
    // تحميل texture افتراضي (أبيض) / Load default texture (white)
    state.defaultTexture = sad::graphics::Texture::CreateDefault(sad::graphics::TextureType::White);
    
    if (!state.defaultTexture || !state.defaultTexture->IsValid()) {
        std::cerr << "(AR) فشل إنشاء الـ texture الافتراضي! / (EN) Failed to create default texture!" << std::endl;
        return -1;
    }
    
    // إنشاء بعض الـ sprites الابتدائية / Create some initial sprites
    Int32 screenWidth, screenHeight;
    SDL_GetWindowSize(window.GetNativeHandle(), &screenWidth, &screenHeight);
    
    for (int i = 0; i < 5; ++i) {
        Float32 x = 200.0f + i * 150.0f;        // موقع X / X position
        Float32 y = 300.0f + (i % 2) * 100.0f;  // موقع Y / Y position
        Float32 size = 50.0f + i * 20.0f;       // الحجم / Size
        
        Sprite sprite = CreateSprite(state.defaultTexture, x, y, size, size);
        sprite.tint = Color(
            50 + i * 40,                        // أحمر / Red
            100 + i * 30,                       // أخضر / Green
            200 - i * 30,                       // أزرق / Blue
            255                                 // الشفافية / Alpha
        );
        state.sprites.push_back(sprite);        // إضافة للقائمة / Add to list
    }
    
    std::cout << "\n(AR) تم إنشاء " << state.sprites.size() << " sprites ابتدائية" << std::endl;
    std::cout << "(EN) Created " << state.sprites.size() << " initial sprites\n" << std::endl;
    
    // طباعة التعليمات / Print instructions
    std::cout << "(AR) التحكم / (EN) Controls:" << std::endl;
    std::cout << "  ESC       - (AR) الخروج / (EN) Exit" << std::endl;
    std::cout << "  H         - (AR) تبديل التعليمات / (EN) Toggle help" << std::endl;
    std::cout << "  SPACE     - (AR) إيقاف/استئناف / (EN) Pause/Resume" << std::endl;
    std::cout << "  A         - (AR) إضافة sprite / (EN) Add sprite" << std::endl;
    std::cout << "  C         - (AR) مسح الكل / (EN) Clear all" << std::endl;
    std::cout << "  R         - (AR) تلوين عشوائي / (EN) Random color" << std::endl;
    std::cout << "  G         - (AR) تبديل الشبكة / (EN) Toggle grid" << std::endl;
    std::cout << "  B         - (AR) تبديل الحدود / (EN) Toggle bounds" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // حلقة اللعبة الرئيسية / Main game loop
    Uint32 lastTime = SDL_GetTicks();            // الوقت السابق / Previous time
    
    while (state.running) {
        // حساب deltaTime / Calculate deltaTime
        Uint32 currentTime = SDL_GetTicks();     // الوقت الحالي / Current time
        state.deltaTime = (currentTime - lastTime) / 1000.0f;  // التحويل للثواني / Convert to seconds
        lastTime = currentTime;                  // تحديث الوقت / Update time
        
        // تحديث الوقت الإجمالي / Update total time
        state.time += state.deltaTime;           // إضافة الدلتا / Add delta
        state.frameCount++;                      // زيادة عدد الإطارات / Increment frame count
        
        // حساب FPS كل ثانية / Calculate FPS every second
        if (state.time >= 1.0f) {
            state.fps = state.frameCount / state.time;  // FPS = الإطارات / الوقت / FPS = frames / time
            state.frameCount = 0;                // إعادة التعيين / Reset
            state.time = 0.0f;                   // إعادة التعيين / Reset
        }
        
        // معالجة أحداث النافذة / Process window events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            window.ProcessEvent(event);          // معالجة حدث النافذة / Process window event
            inputManager.ProcessEvent(event);    // معالجة حدث الإدخال / Process input event
            
            if (event.type == SDL_QUIT) {
                state.running = false;           // إيقاف التطبيق / Stop application
            }
        }
        
        // معالجة الإدخال / Process input
        inputManager.Update();                   // تحديث مدير الإدخال / Update input manager
        ProcessInput(inputManager, state, screenWidth, screenHeight);
        
        // تحديث الـ sprites / Update sprites
        if (!state.paused) {
            for (Sprite& sprite : state.sprites) {
                UpdateSprite(sprite, state.deltaTime, screenWidth, screenHeight);
            }
        }
        
        // الرسم / Rendering
        renderer.BeginFrame();                   // بداية الإطار / Begin frame
        renderer.Clear(Color(30, 30, 40, 255));  // مسح الشاشة / Clear screen
        
        // رسم الشبكة / Draw grid
        if (state.showGrid) {
            DrawGrid(renderer, screenWidth, screenHeight);
        }
        
        // رسم جميع الـ sprites / Draw all sprites
        for (const Sprite& sprite : state.sprites) {
            DrawSprite(renderer, sprite);        // رسم sprite / Draw sprite
            
            if (state.showBounds) {
                DrawSpriteBounds(renderer, sprite);  // رسم الحدود / Draw bounds
            }
        }
        
        // رسم معلومات الأداء / Draw performance info
        DrawStats(renderer, state);
        
        // رسم التعليمات / Draw help
        if (state.showHelp) {
            DrawHelp(renderer, screenHeight);
        }
        
        renderer.EndFrame();                     // نهاية الإطار / End frame
        
        // عرض النتيجة / Present result
        context.Present();                       // عرض على الشاشة / Present to screen
        
        // طباعة معلومات الأداء كل 60 إطار / Print performance info every 60 frames
        if (state.frameCount % 60 == 0) {
            const RenderStats& stats = renderer.GetStats();
            std::cout << "(AR) الإطارات: " << state.fps 
                     << " | الرسومات: " << stats.drawCalls
                     << " | الرؤوس: " << stats.vertices
                     << " | الـ sprites: " << state.sprites.size() << std::endl;
        }
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "(AR) تم إنهاء التطبيق بنجاح" << std::endl;
    std::cout << "(EN) Application terminated successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
