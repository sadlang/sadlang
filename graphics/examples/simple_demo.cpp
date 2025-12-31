// ==============================================================================
// simple_demo.cpp - مثال بسيط لاستخدام مكتبة الرسومات
// Simple Graphics Library Demo
// ==============================================================================
// الوصف: مثال يوضح رسم أشكال والتفاعل مع الإدخال
// Description: Example showing shape drawing and input interaction
// ==============================================================================

#include "../include/window/window.h"           // نظام النوافذ / Window system
#include "../include/rendering/context.h"       // سياق الرسم / Render context
#include "../include/rendering/renderer2d.h"    // محرك الرسم 2D / 2D renderer
#include "../include/input/input_manager.h"     // مدير الإدخال / Input manager
#include <iostream>                             // للطباعة / For printing
#include <cmath>                                // للدوال الرياضية / For math functions

using namespace SadGraphics;                    // استخدام مساحة الأسماء / Use namespace

// ==============================================================================
// ثوابت البرنامج / Program Constants
// ==============================================================================
const int WINDOW_WIDTH = 800;                   // عرض النافذة / Window width
const int WINDOW_HEIGHT = 600;                  // ارتفاع النافذة / Window height
const char* WINDOW_TITLE = "Sad Graphics Demo - مثال الرسومات";  // عنوان النافذة / Window title

// ==============================================================================
// متغيرات حالة البرنامج / Program State Variables
// ==============================================================================
struct AppState {
    bool running;                               // هل البرنامج يعمل / Is program running
    Float32 time;                               // الوقت المنقضي / Elapsed time
    Point2D circlePos;                          // موقع الدائرة / Circle position
    Color backgroundColor;                      // لون الخلفية / Background color
    bool showHelp;                              // عرض المساعدة / Show help
};

// ==============================================================================
// دالة طباعة المساعدة / Print Help Function
// ==============================================================================
void PrintHelp() {
    std::cout << "\n=== Sad Graphics Demo - Controls ===\n";
    std::cout << "ESC        - خروج / Exit\n";
    std::cout << "H          - إخفاء/إظهار المساعدة / Toggle help\n";
    std::cout << "Arrows     - تحريك الدائرة / Move circle\n";
    std::cout << "Space      - تغيير لون الخلفية / Change background\n";
    std::cout << "Left Click - رسم دائرة صغيرة / Draw small circle\n";
    std::cout << "Mouse Move - تحديث موقع الدائرة الكبيرة / Update big circle position\n";
    std::cout << "====================================\n\n";
}

// ==============================================================================
// دالة معالجة الإدخال / Process Input Function
// ==============================================================================
void ProcessInput(InputManager& input, AppState& state) {
    // التحقق من مفتاح الخروج / Check for exit key
    if (input.IsKeyPressed(KeyCode::Escape)) {  // مفتاح ESC / Escape key
        state.running = false;                  // إيقاف البرنامج / Stop program
        std::cout << "إنهاء البرنامج... / Exiting program...\n";
    }
    
    // التحقق من مفتاح المساعدة / Check for help key
    if (input.IsKeyPressed(KeyCode::H)) {       // مفتاح H / H key
        state.showHelp = !state.showHelp;       // عكس الحالة / Toggle state
        if (state.showHelp) {                   // إذا كان معروضاً / If showing
            PrintHelp();                        // طباعة المساعدة / Print help
        }
    }
    
    // تحريك الدائرة بالأسهم / Move circle with arrows
    const Float32 moveSpeed = 5.0f;             // سرعة الحركة / Movement speed
    if (input.IsKeyHeld(KeyCode::ArrowUp)) {    // سهم أعلى / Up arrow
        state.circlePos.y -= moveSpeed;         // تحريك للأعلى / Move up
    }
    if (input.IsKeyHeld(KeyCode::ArrowDown)) {  // سهم أسفل / Down arrow
        state.circlePos.y += moveSpeed;         // تحريك للأسفل / Move down
    }
    if (input.IsKeyHeld(KeyCode::ArrowLeft)) {  // سهم يسار / Left arrow
        state.circlePos.x -= moveSpeed;         // تحريك لليسار / Move left
    }
    if (input.IsKeyHeld(KeyCode::ArrowRight)) { // سهم يمين / Right arrow
        state.circlePos.x += moveSpeed;         // تحريك لليمين / Move right
    }
    
    // تغيير لون الخلفية / Change background color
    if (input.IsKeyPressed(KeyCode::Space)) {   // مفتاح المسافة / Space key
        // تدوير الألوان / Cycle colors
        if (state.backgroundColor == Color::Black) {
            state.backgroundColor = Color(30, 30, 40);  // رمادي داكن / Dark gray
        } else if (state.backgroundColor.r == 30) {
            state.backgroundColor = Color(20, 30, 50);  // أزرق داكن / Dark blue
        } else {
            state.backgroundColor = Color::Black;       // أسود / Black
        }
        std::cout << "تغيير لون الخلفية / Background color changed\n";
    }
    
    // تحديث موقع الدائرة الكبيرة مع الفأرة / Update big circle with mouse
    if (input.IsMouseMoved()) {                 // إذا تحركت الفأرة / If mouse moved
        Point2D mousePos = input.GetMousePosition();  // الحصول على الموقع / Get position
        // تحديث تدريجي / Gradual update
        state.circlePos.x += (mousePos.x - state.circlePos.x) * 0.1f;
        state.circlePos.y += (mousePos.y - state.circlePos.y) * 0.1f;
    }
}

// ==============================================================================
// دالة الرسم / Render Function
// ==============================================================================
void Render(Renderer2D& renderer, const AppState& state, const InputManager& input) {
    // بدء الإطار / Begin frame
    renderer.BeginFrame();                      // بدء الإطار / Start frame
    
    // مسح الشاشة / Clear screen
    renderer.Clear(state.backgroundColor);      // مسح بلون الخلفية / Clear with background
    
    // رسم شبكة الخلفية / Draw background grid
    renderer.SetDrawColor(Color(50, 50, 60));   // لون رمادي خفيف / Light gray
    for (int x = 0; x < WINDOW_WIDTH; x += 50) {  // خطوط عمودية / Vertical lines
        renderer.DrawLine(x, 0, x, WINDOW_HEIGHT, Color(50, 50, 60), 1.0f);
    }
    for (int y = 0; y < WINDOW_HEIGHT; y += 50) {  // خطوط أفقية / Horizontal lines
        renderer.DrawLine(0, y, WINDOW_WIDTH, y, Color(50, 50, 60), 1.0f);
    }
    
    // رسم مستطيل ملون في الأعلى / Draw colored rectangle at top
    renderer.DrawRect(50, 50, 200, 100, Color::Red, true);
    renderer.DrawRectOutline(50, 50, 200, 100, Color::Red, Color::Yellow, 2.0f);
    
    // رسم مثلث / Draw triangle
    renderer.DrawTriangle(300, 100, 400, 50, 350, 150, Color::Green, true);
    
    // رسم قطع ناقص / Draw ellipse
    renderer.DrawEllipse(550, 100, 80, 50, Color::Cyan, false);
    
    // رسم الدائرة الكبيرة التفاعلية / Draw big interactive circle
    Float32 pulse = std::sin(state.time * 2.0f) * 10.0f;  // نبض / Pulse
    Float32 radius = 50.0f + pulse;             // نصف قطر متغير / Variable radius
    Color circleColor = Color::Blue;            // اللون الأساسي / Base color
    
    // تغيير اللون حسب الموقع / Change color based on position
    circleColor.r = static_cast<UInt8>(state.circlePos.x / WINDOW_WIDTH * 255);
    circleColor.g = static_cast<UInt8>(state.circlePos.y / WINDOW_HEIGHT * 255);
    
    renderer.DrawCircle(state.circlePos.x, state.circlePos.y, radius, circleColor, true);
    renderer.DrawCircle(state.circlePos.x, state.circlePos.y, radius + 5, Color::White, false);
    
    // رسم دوائر صغيرة عند موقع الفأرة عند الضغط / Draw small circles on mouse click
    if (input.IsMouseButtonHeld(MouseButton::Left)) {
        Point2D mousePos = input.GetMousePosition();
        renderer.DrawCircle(mousePos.x, mousePos.y, 10, Color::Red, true);
    }
    
    // رسم مؤشر الفأرة / Draw mouse cursor indicator
    Point2D mousePos = input.GetMousePosition();
    renderer.DrawLine(mousePos.x - 10, mousePos.y, mousePos.x + 10, mousePos.y, 
                     Color::White, 2.0f);
    renderer.DrawLine(mousePos.x, mousePos.y - 10, mousePos.x, mousePos.y + 10, 
                     Color::White, 2.0f);
    
    // رسم قوس دوار / Draw rotating arc
    Float32 arcAngle = state.time;              // زاوية دوران / Rotation angle
    renderer.DrawArc(WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100, 40,
                    arcAngle, arcAngle + 3.14159f, Color::Magenta, 32);
    
    // رسم مضلع / Draw polygon
    Point2D polygonPoints[] = {
        {100, WINDOW_HEIGHT - 150},
        {150, WINDOW_HEIGHT - 100},
        {200, WINDOW_HEIGHT - 120},
        {180, WINDOW_HEIGHT - 170},
        {120, WINDOW_HEIGHT - 180}
    };
    renderer.DrawPolygon(polygonPoints, 5, Color::Yellow, false);
    
    // إنهاء الإطار / End frame
    renderer.EndFrame();                        // إنهاء الإطار / Finish frame
}

// ==============================================================================
// الدالة الرئيسية / Main Function
// ==============================================================================
int main(int argc, char* argv[]) {
    // طباعة رسالة البداية / Print startup message
    std::cout << "\n=== Sad Graphics Library Demo ===\n";
    std::cout << "مثال مكتبة الرسومات للغة ص\n";
    std::cout << "=================================\n\n";
    
    // تهيئة SDL / Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {       // تهيئة نظام الفيديو / Initialize video
        std::cerr << "فشل تهيئة SDL / SDL Init failed: " << SDL_GetError() << "\n";
        return -1;                              // إنهاء بخطأ / Exit with error
    }
    
    // تهيئة مدير النوافذ / Initialize window manager
    WindowManager& winManager = WindowManager::GetInstance();
    if (winManager.Initialize() != Result::Success) {
        std::cerr << "فشل تهيئة مدير النوافذ / WindowManager init failed\n";
        SDL_Quit();                             // تنظيف SDL / Cleanup SDL
        return -1;
    }
    
    // إنشاء النافذة / Create window
    WindowHandle windowHandle = winManager.CreateWindow(
        WINDOW_TITLE,                           // العنوان / Title
        WINDOW_WIDTH,                           // العرض / Width
        WINDOW_HEIGHT,                          // الارتفاع / Height
        WindowFlags::Shown | WindowFlags::OpenGL  // الخيارات / Flags
    );
    
    if (windowHandle == 0) {                    // التحقق من النجاح / Check success
        std::cerr << "فشل إنشاء النافذة / Window creation failed\n";
        winManager.Shutdown();                  // تنظيف / Cleanup
        SDL_Quit();
        return -1;
    }
    
    Window* window = winManager.GetWindow(windowHandle);  // الحصول على النافذة / Get window
    
    // إنشاء سياق الرسم / Create render context
    ContextOptions contextOpts;                 // خيارات السياق / Context options
    contextOpts.vsync = true;                   // تفعيل VSync / Enable VSync
    RenderContext context(window->GetNativeWindow(), contextOpts);
    
    if (!context.IsValid()) {                   // التحقق من الصلاحية / Check validity
        std::cerr << "فشل إنشاء سياق الرسم / RenderContext creation failed\n";
        winManager.Shutdown();
        SDL_Quit();
        return -1;
    }
    
    // طباعة معلومات OpenGL / Print OpenGL info
    std::cout << "OpenGL Version: " << context.GetVersion() << "\n";
    std::cout << "Renderer: " << context.GetRenderer() << "\n\n";
    
    // إنشاء محرك الرسم / Create renderer
    Renderer2D renderer(&context);              // إنشاء المحرك / Create renderer
    
    // إنشاء مدير الإدخال / Create input manager
    InputManager input;                         // مدير الإدخال / Input manager
    
    // تهيئة حالة البرنامج / Initialize app state
    AppState state;                             // حالة البرنامج / App state
    state.running = true;                       // البرنامج يعمل / Program running
    state.time = 0.0f;                          // بداية الوقت / Start time
    state.circlePos = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};  // في الوسط / Center
    state.backgroundColor = Color::Black;       // خلفية سوداء / Black background
    state.showHelp = true;                      // عرض المساعدة / Show help
    
    // طباعة المساعدة / Print help
    PrintHelp();                                // عرض التعليمات / Show instructions
    
    // الحلقة الرئيسية / Main loop
    Uint32 lastTime = SDL_GetTicks();           // الوقت السابق / Last time
    
    while (state.running) {                     // طالما البرنامج يعمل / While running
        // حساب الوقت / Calculate time
        Uint32 currentTime = SDL_GetTicks();    // الوقت الحالي / Current time
        Float32 deltaTime = (currentTime - lastTime) / 1000.0f;  // الفرق الزمني / Time delta
        lastTime = currentTime;                 // تحديث الوقت / Update time
        state.time += deltaTime;                // تحديث الوقت الكلي / Update total time
        
        // معالجة أحداث SDL / Process SDL events
        SDL_Event event;                        // حدث SDL / SDL event
        while (SDL_PollEvent(&event)) {         // المرور على الأحداث / Poll events
            if (event.type == SDL_QUIT) {       // حدث إغلاق / Quit event
                state.running = false;          // إيقاف البرنامج / Stop program
            }
            
            // معالجة الإدخال / Process input
            input.ProcessEvent(event);          // معالجة الحدث / Process event
        }
        
        // تحديث حالة الإدخال / Update input state
        input.Update();                         // تحديث / Update
        
        // معالجة الإدخال / Process input
        ProcessInput(input, state);             // معالجة المدخلات / Process inputs
        
        // الرسم / Render
        Render(renderer, state, input);         // رسم الإطار / Render frame
        
        // عرض النتيجة / Display result
        context.SwapBuffers();                  // تبديل المخازن / Swap buffers
    }
    
    // طباعة الإحصائيات النهائية / Print final statistics
    std::cout << "\n=== الإحصائيات النهائية / Final Statistics ===\n";
    renderer.PrintStats();                      // طباعة إحصائيات الرسم / Print render stats
    std::cout << "الوقت الكلي / Total Time: " << state.time << " ثانية / seconds\n";
    
    // التنظيف / Cleanup
    winManager.Shutdown();                      // إغلاق مدير النوافذ / Shutdown window manager
    SDL_Quit();                                 // إنهاء SDL / Quit SDL
    
    std::cout << "\nإنهاء البرنامج بنجاح / Program ended successfully\n";
    return 0;                                   // إنهاء ناجح / Successful exit
}
