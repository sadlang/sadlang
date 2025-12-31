// ==============================================================================
// text_demo.cpp - عرض توضيحي لنظام النصوص / Text System Demo
// ==============================================================================
// الوصف: مثال شامل يعرض إمكانيات نظام النصوص والخطوط
// Description: Comprehensive example showcasing text and font system capabilities
// ==============================================================================

#include "../include/window/window.h"           // نظام النوافذ / Window system
#include "../include/rendering/context.h"       // سياق الرسم / Render context
#include "../include/rendering/renderer2d.h"    // محرك الرسم 2D / 2D renderer
#include "../include/input/input_manager.h"     // إدارة المدخلات / Input management
#include "../include/text/font.h"               // نظام الخطوط / Font system

#include <iostream>                             // للإدخال/الإخراج / For I/O
#include <sstream>                              // للنصوص / For strings
#include <iomanip>                              // للتنسيق / For formatting
#include <chrono>                               // للوقت / For timing
#include <cmath>                                // للرياضيات / For math

// استخدام مساحات الأسماء / Using namespaces
using namespace SadGraphics;                    // مساحة أسماء الرسومات / Graphics namespace
using namespace sad::graphics;                  // مساحة أسماء sad / sad namespace

// ==============================================================================
// الثوابت / Constants
// ==============================================================================
constexpr int WINDOW_WIDTH = 1280;              // عرض النافذة / Window width
constexpr int WINDOW_HEIGHT = 720;              // ارتفاع النافذة / Window height
constexpr float TARGET_FPS = 60.0f;             // الإطارات المستهدفة / Target FPS
constexpr float FRAME_TIME = 1.0f / TARGET_FPS; // وقت الإطار / Frame time

// ==============================================================================
// بيانات العرض / Demo Data
// ==============================================================================

/// معلومات خط / Font info
struct FontInfo {
    FontRef font;                               // الخط / Font
    std::string name;                           // الاسم / Name
    float size;                                 // الحجم / Size
    bool loaded;                                // محمّل؟ / Loaded?
};

/// حالة التطبيق / Application state
struct AppState {
    bool running;                               // يعمل؟ / Running?
    bool showHelp;                              // عرض المساعدة؟ / Show help?
    bool showStats;                             // عرض الإحصائيات؟ / Show statistics?
    int selectedDemo;                           // العرض المختار / Selected demo
    
    // إحصائيات الأداء / Performance stats
    float fps;                                  // الإطارات في الثانية / Frames per second
    float frameTime;                            // وقت الإطار (ms) / Frame time (ms)
    int frameCount;                             // عداد الإطارات / Frame counter
    double lastTime;                            // آخر وقت / Last time
    double fpsTimer;                            // مؤقت FPS / FPS timer
    
    // الخطوط / Fonts
    std::vector<FontInfo> fonts;                // قائمة الخطوط / Font list
    int selectedFont;                           // الخط المختار / Selected font
    
    // النصوص التوضيحية / Demo texts
    std::string sampleText;                     // نص تجريبي / Sample text
    std::string arabicText;                     // نص عربي / Arabic text
    std::string englishText;                    // نص إنجليزي / English text
    
    // الألوان / Colors
    std::vector<Color> colors;                  // قائمة الألوان / Color list
    int selectedColor;                          // اللون المختار / Selected color
    
    // التحريك / Animation
    float animationTime;                        // وقت التحريك / Animation time
    float waveAmplitude;                        // سعة الموجة / Wave amplitude
    float waveFrequency;                        // تردد الموجة / Wave frequency
    
    /// المُنشئ / Constructor
    AppState()
        : running(true)                         // تشغيل / Running
        , showHelp(true)                        // عرض المساعدة / Show help
        , showStats(true)                       // عرض الإحصائيات / Show stats
        , selectedDemo(0)                       // العرض الأول / First demo
        , fps(0.0f)                             // FPS صفر / Zero FPS
        , frameTime(0.0f)                       // وقت صفر / Zero time
        , frameCount(0)                         // عداد صفر / Zero count
        , lastTime(0.0)                         // وقت صفر / Zero time
        , fpsTimer(0.0)                         // مؤقت صفر / Zero timer
        , selectedFont(0)                       // الخط الأول / First font
        , selectedColor(0)                      // اللون الأول / First color
        , animationTime(0.0f)                   // وقت صفر / Zero time
        , waveAmplitude(20.0f)                  // سعة 20 / Amplitude 20
        , waveFrequency(2.0f)                   // تردد 2 / Frequency 2
    {
        // النصوص التجريبية / Sample texts
        sampleText = "Hello, World! مرحبا بالعالم!";
        arabicText = "اللغة العربية جميلة ورائعة";
        englishText = "The quick brown fox jumps over the lazy dog";
        
        // الألوان / Colors
        colors = {
            Color::White,                       // أبيض / White
            Color::Red,                         // أحمر / Red
            Color::Green,                       // أخضر / Green
            Color::Blue,                        // أزرق / Blue
            Color::Yellow,                      // أصفر / Yellow
            Color::Magenta,                     // بنفسجي / Magenta
            Color::Cyan,                        // سماوي / Cyan
            Color(255, 128, 0, 255),           // برتقالي / Orange
        };
    }
};

// ==============================================================================
// الدوال المساعدة / Helper Functions
// ==============================================================================

/// تحميل الخطوط / Load fonts
void LoadFonts(AppState& state) {
    std::cout << "(AR) تحميل الخطوط... / (EN) Loading fonts..." << std::endl;
    
    // قائمة الخطوط للتحميل / List of fonts to load
    std::vector<std::pair<std::string, float>> fontPaths = {
        {"C:/Windows/Fonts/arial.ttf", 24.0f},              // Arial
        {"C:/Windows/Fonts/times.ttf", 24.0f},              // Times New Roman
        {"C:/Windows/Fonts/cour.ttf", 24.0f},               // Courier New
        {"C:/Windows/Fonts/verdana.ttf", 24.0f},            // Verdana
        {"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24.0f},  // Linux
    };
    
    // محاولة تحميل كل خط / Try loading each font
    for (const auto& [path, size] : fontPaths) {
        FontInfo info;                          // معلومات الخط / Font info
        info.font = Font::CreateFromFile(path, size);  // محاولة التحميل / Try loading
        info.size = size;                       // حفظ الحجم / Store size
        info.loaded = (info.font != nullptr && info.font->IsValid());  // التحقق / Check
        
        // استخراج اسم الخط من المسار / Extract font name from path
        size_t lastSlash = path.find_last_of("/\\");
        info.name = (lastSlash != std::string::npos) 
                   ? path.substr(lastSlash + 1) 
                   : path;
        
        if (info.loaded) {                      // إذا نجح التحميل / If loaded successfully
            std::cout << "(AR) تم تحميل: " << info.name 
                     << " / (EN) Loaded: " << info.name << std::endl;
            state.fonts.push_back(info);        // إضافة للقائمة / Add to list
        }
    }
    
    // التحقق من وجود خطوط / Check if fonts loaded
    if (state.fonts.empty()) {                  // إذا لم يُحمَّل أي خط / If no fonts loaded
        std::cerr << "(AR) فشل تحميل أي خط! / (EN) Failed to load any fonts!" << std::endl;
        state.running = false;                  // إيقاف التطبيق / Stop application
    } else {
        std::cout << "(AR) تم تحميل " << state.fonts.size() << " خط(وط)"
                 << " / (EN) Loaded " << state.fonts.size() << " font(s)" << std::endl;
    }
}

/// تحديث حالة التطبيق / Update application state
void UpdateState(AppState& state, float deltaTime) {
    // تحديث وقت التحريك / Update animation time
    state.animationTime += deltaTime;           // زيادة الوقت / Increment time
    
    // تحديث عداد الإطارات / Update frame counter
    state.frameCount++;                         // زيادة العداد / Increment counter
    state.fpsTimer += deltaTime;                // زيادة المؤقت / Increment timer
    
    // حساب FPS كل ثانية / Calculate FPS every second
    if (state.fpsTimer >= 1.0) {                // كل ثانية / Every second
        state.fps = state.frameCount / state.fpsTimer;  // حساب FPS / Calculate FPS
        state.frameTime = (state.fpsTimer / state.frameCount) * 1000.0f;  // ms
        state.frameCount = 0;                   // إعادة تعيين / Reset
        state.fpsTimer = 0.0;                   // إعادة تعيين / Reset
    }
}

/// معالجة المدخلات / Handle input
void HandleInput(AppState& state, InputManager& input) {
    // ESC للخروج / ESC to exit
    if (input.IsKeyPressed(SDLK_ESCAPE)) {
        state.running = false;                  // إيقاف / Stop
    }
    
    // H لعرض/إخفاء المساعدة / H to toggle help
    if (input.IsKeyPressed(SDLK_h)) {
        state.showHelp = !state.showHelp;       // تبديل / Toggle
    }
    
    // S لعرض/إخفاء الإحصائيات / S to toggle stats
    if (input.IsKeyPressed(SDLK_s)) {
        state.showStats = !state.showStats;     // تبديل / Toggle
    }
    
    // 1-5 لتبديل العروض / 1-5 to switch demos
    if (input.IsKeyPressed(SDLK_1)) state.selectedDemo = 0;
    if (input.IsKeyPressed(SDLK_2)) state.selectedDemo = 1;
    if (input.IsKeyPressed(SDLK_3)) state.selectedDemo = 2;
    if (input.IsKeyPressed(SDLK_4)) state.selectedDemo = 3;
    if (input.IsKeyPressed(SDLK_5)) state.selectedDemo = 4;
    
    // F لتبديل الخط / F to cycle font
    if (input.IsKeyPressed(SDLK_f)) {
        state.selectedFont = (state.selectedFont + 1) % state.fonts.size();
    }
    
    // C لتبديل اللون / C to cycle color
    if (input.IsKeyPressed(SDLK_c)) {
        state.selectedColor = (state.selectedColor + 1) % state.colors.size();
    }
    
    // سهم لأعلى/لأسفل لتعديل السعة / Up/Down to adjust amplitude
    if (input.IsKeyDown(SDLK_UP)) {
        state.waveAmplitude += 10.0f * 0.016f;  // زيادة السعة / Increase amplitude
    }
    if (input.IsKeyDown(SDLK_DOWN)) {
        state.waveAmplitude -= 10.0f * 0.016f;  // تقليل السعة / Decrease amplitude
        if (state.waveAmplitude < 0.0f) state.waveAmplitude = 0.0f;
    }
}

// ==============================================================================
// العروض التوضيحية / Demos
// ==============================================================================

/// عرض 1: نص أساسي / Demo 1: Basic Text
void Demo1_BasicText(Renderer2D& renderer, AppState& state) {
    const FontRef& font = state.fonts[state.selectedFont].font;
    const Color& color = state.colors[state.selectedColor];
    
    float y = 100.0f;                           // موقع Y البداية / Start Y position
    float lineSpacing = 50.0f;                  // المسافة بين الأسطر / Line spacing
    
    // عنوان العرض / Demo title
    renderer.DrawText("Demo 1: Basic Text Rendering", font, 50, y, Color::Yellow);
    y += lineSpacing * 1.5f;
    
    // نص عادي / Normal text
    renderer.DrawText("Standard text: " + state.englishText, font, 50, y, color);
    y += lineSpacing;
    
    // نص عربي / Arabic text
    renderer.DrawText("Arabic text: " + state.arabicText, font, 50, y, color);
    y += lineSpacing;
    
    // نص مختلط / Mixed text
    renderer.DrawText("Mixed: " + state.sampleText, font, 50, y, color);
    y += lineSpacing;
    
    // أحجام مختلفة / Different sizes
    for (int i = 0; i < std::min(3, (int)state.fonts.size()); ++i) {
        std::ostringstream oss;
        oss << "Font " << (i + 1) << ": " << state.fonts[i].name;
        renderer.DrawText(oss.str(), state.fonts[i].font, 50, y, color);
        y += lineSpacing;
    }
}

/// عرض 2: محاذاة النصوص / Demo 2: Text Alignment
void Demo2_Alignment(Renderer2D& renderer, AppState& state) {
    const FontRef& font = state.fonts[state.selectedFont].font;
    
    float centerX = WINDOW_WIDTH / 2.0f;        // مركز X / Center X
    float centerY = WINDOW_HEIGHT / 2.0f;       // مركز Y / Center Y
    
    // عنوان / Title
    renderer.DrawTextCentered("Demo 2: Text Alignment", font, 
                             centerX, 100, Color::Yellow);
    
    // رسم خطوط مرجعية / Draw reference lines
    renderer.DrawLine(centerX, 0, centerX, WINDOW_HEIGHT, Color(128, 128, 128, 128), 2.0f);
    renderer.DrawLine(0, centerY, WINDOW_WIDTH, centerY, Color(128, 128, 128, 128), 2.0f);
    
    // محاذاة مختلفة / Different alignments
    renderer.DrawTextAligned("Top-Left (0,0)", font, 
                            centerX, centerY, 0.0f, 0.0f, Color::Red);
    
    renderer.DrawTextAligned("Top-Center (0.5,0)", font, 
                            centerX, centerY, 0.5f, 0.0f, Color::Green);
    
    renderer.DrawTextAligned("Top-Right (1,0)", font, 
                            centerX, centerY, 1.0f, 0.0f, Color::Blue);
    
    renderer.DrawTextAligned("Center-Left (0,0.5)", font, 
                            centerX, centerY, 0.0f, 0.5f, Color::Yellow);
    
    renderer.DrawTextCentered("CENTER (0.5,0.5)", font, 
                             centerX, centerY, Color::Magenta);
    
    renderer.DrawTextAligned("Center-Right (1,0.5)", font, 
                            centerX, centerY, 1.0f, 0.5f, Color::Cyan);
    
    renderer.DrawTextAligned("Bottom-Left (0,1)", font, 
                            centerX, centerY, 0.0f, 1.0f, Color(255, 128, 0, 255));
    
    renderer.DrawTextAligned("Bottom-Center (0.5,1)", font, 
                            centerX, centerY, 0.5f, 1.0f, Color(128, 255, 0, 255));
    
    renderer.DrawTextAligned("Bottom-Right (1,1)", font, 
                            centerX, centerY, 1.0f, 1.0f, Color(255, 0, 128, 255));
}

/// عرض 3: الألوان / Demo 3: Colors
void Demo3_Colors(Renderer2D& renderer, AppState& state) {
    const FontRef& font = state.fonts[state.selectedFont].font;
    
    // عنوان / Title
    renderer.DrawTextCentered("Demo 3: Text Colors", font, 
                             WINDOW_WIDTH / 2, 100, Color::Yellow);
    
    float y = 200.0f;                           // موقع البداية / Start position
    float spacing = 50.0f;                      // المسافة / Spacing
    
    // عرض جميع الألوان / Show all colors
    const char* colorNames[] = {
        "White", "Red", "Green", "Blue", 
        "Yellow", "Magenta", "Cyan", "Orange"
    };
    
    for (size_t i = 0; i < state.colors.size(); ++i) {
        std::ostringstream oss;
        oss << colorNames[i] << ": " << state.sampleText;
        renderer.DrawText(oss.str(), font, 50, y, state.colors[i]);
        y += spacing;
    }
}

/// عرض 4: تأثيرات متحركة / Demo 4: Animated Effects
void Demo4_AnimatedEffects(Renderer2D& renderer, AppState& state) {
    const FontRef& font = state.fonts[state.selectedFont].font;
    
    // عنوان / Title
    renderer.DrawTextCentered("Demo 4: Animated Text Effects", font, 
                             WINDOW_WIDTH / 2, 50, Color::Yellow);
    
    std::string text = "WAVE EFFECT";
    float baseX = 200.0f;                       // موقع X الأساسي / Base X
    float baseY = 200.0f;                       // موقع Y الأساسي / Base Y
    
    // تأثير موجة / Wave effect
    float charWidth = 30.0f;                    // عرض الحرف التقريبي / Approx char width
    for (size_t i = 0; i < text.length(); ++i) {
        float x = baseX + i * charWidth;        // موقع X / X position
        float offset = std::sin(state.animationTime * state.waveFrequency + i * 0.5f) 
                      * state.waveAmplitude;    // إزاحة الموجة / Wave offset
        float y = baseY + offset;               // موقع Y مع الموجة / Y with wave
        
        // لون متدرج / Gradient color
        float hue = (state.animationTime + i * 0.1f);
        u8 r = static_cast<u8>(std::sin(hue) * 127 + 128);
        u8 g = static_cast<u8>(std::sin(hue + 2.0f) * 127 + 128);
        u8 b = static_cast<u8>(std::sin(hue + 4.0f) * 127 + 128);
        Color color(r, g, b, 255);
        
        std::string charStr(1, text[i]);        // حرف واحد / Single char
        renderer.DrawText(charStr, font, x, y, color);
    }
    
    // تأثير نبض / Pulse effect
    baseY = 350.0f;
    text = "PULSE EFFECT";
    float scale = 1.0f + std::sin(state.animationTime * 3.0f) * 0.3f;  // نبض / Pulse
    u8 alpha = static_cast<u8>(std::sin(state.animationTime * 2.0f) * 127 + 128);
    renderer.DrawTextCentered(text, font, WINDOW_WIDTH / 2, baseY, 
                             Color(255, 255, 255, alpha));
    
    // تأثير قوس قزح / Rainbow effect
    baseY = 500.0f;
    text = "RAINBOW TEXT";
    charWidth = 40.0f;
    float startX = (WINDOW_WIDTH - text.length() * charWidth) / 2;
    
    for (size_t i = 0; i < text.length(); ++i) {
        float hue = (state.animationTime + i * 0.3f) * 2.0f;
        u8 r = static_cast<u8>(std::sin(hue) * 127 + 128);
        u8 g = static_cast<u8>(std::sin(hue + 2.094f) * 127 + 128);
        u8 b = static_cast<u8>(std::sin(hue + 4.189f) * 127 + 128);
        
        std::string charStr(1, text[i]);
        renderer.DrawText(charStr, font, startX + i * charWidth, baseY, 
                         Color(r, g, b, 255));
    }
}

/// عرض 5: قياس النصوص / Demo 5: Text Measurement
void Demo5_TextMeasurement(Renderer2D& renderer, AppState& state) {
    const FontRef& font = state.fonts[state.selectedFont].font;
    
    // عنوان / Title
    renderer.DrawTextCentered("Demo 5: Text Measurement & Bounding Boxes", font, 
                             WINDOW_WIDTH / 2, 50, Color::Yellow);
    
    // نص تجريبي / Sample text
    std::string text = state.sampleText;
    float x = 100.0f;
    float y = 200.0f;
    
    // قياس النص / Measure text
    float width, height;
    font->MeasureText(text, width, height);
    
    // رسم صندوق محيط / Draw bounding box
    renderer.DrawRect(x - 5, y - 5, width + 10, height + 10, 
                     Color(64, 64, 64, 128), true);
    renderer.DrawRect(x - 5, y - 5, width + 10, height + 10, 
                     Color::White, false);
    
    // رسم النص / Draw text
    renderer.DrawText(text, font, x, y, Color::White);
    
    // عرض المعلومات / Display info
    std::ostringstream info;
    info << "Width: " << std::fixed << std::setprecision(2) << width << " px";
    renderer.DrawText(info.str(), font, x, y + height + 30, Color::Cyan);
    
    info.str("");
    info << "Height: " << height << " px";
    renderer.DrawText(info.str(), font, x, y + height + 60, Color::Cyan);
    
    info.str("");
    info << "Font Size: " << font->GetFontSize() << " px";
    renderer.DrawText(info.str(), font, x, y + height + 90, Color::Cyan);
    
    info.str("");
    info << "Line Height: " << font->GetLineHeight() << " px";
    renderer.DrawText(info.str(), font, x, y + height + 120, Color::Cyan);
}

// ==============================================================================
// عرض المساعدة والإحصائيات / Show Help and Stats
// ==============================================================================

/// عرض المساعدة / Show help
void ShowHelp(Renderer2D& renderer, const FontRef& font) {
    float x = 20.0f;                            // موقع X / X position
    float y = WINDOW_HEIGHT - 200.0f;          // موقع Y / Y position
    float lineHeight = 25.0f;                   // ارتفاع السطر / Line height
    
    // خلفية شبه شفافة / Semi-transparent background
    renderer.DrawRect(10, y - 10, 400, 210, Color(0, 0, 0, 180), true);
    
    // عنوان / Title
    renderer.DrawText("Controls (H to toggle):", font, x, y, Color::Yellow);
    y += lineHeight;
    
    // المفاتيح / Keys
    renderer.DrawText("ESC - Exit", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("1-5 - Select Demo", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("F - Cycle Font", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("C - Cycle Color", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("S - Toggle Stats", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("UP/DOWN - Adjust Wave", font, x, y, Color::White);
}

/// عرض الإحصائيات / Show statistics
void ShowStats(Renderer2D& renderer, const FontRef& font, const AppState& state) {
    float x = WINDOW_WIDTH - 320.0f;           // موقع X / X position
    float y = 20.0f;                            // موقع Y / Y position
    float lineHeight = 25.0f;                   // ارتفاع السطر / Line height
    
    // خلفية / Background
    renderer.DrawRect(x - 10, y - 10, 310, 180, Color(0, 0, 0, 180), true);
    
    // العنوان / Title
    renderer.DrawText("Performance Stats:", font, x, y, Color::Yellow);
    y += lineHeight;
    
    // الإحصائيات / Statistics
    std::ostringstream oss;
    
    oss << "FPS: " << std::fixed << std::setprecision(1) << state.fps;
    renderer.DrawText(oss.str(), font, x, y, Color::Green);
    y += lineHeight;
    
    oss.str("");
    oss << "Frame Time: " << std::setprecision(2) << state.frameTime << " ms";
    renderer.DrawText(oss.str(), font, x, y, Color::Cyan);
    y += lineHeight;
    
    oss.str("");
    oss << "Demo: " << (state.selectedDemo + 1) << "/5";
    renderer.DrawText(oss.str(), font, x, y, Color::White);
    y += lineHeight;
    
    oss.str("");
    oss << "Font: " << state.fonts[state.selectedFont].name;
    renderer.DrawText(oss.str(), font, x, y, Color::White);
    y += lineHeight;
    
    oss.str("");
    oss << "Color: " << (state.selectedColor + 1) << "/" << state.colors.size();
    renderer.DrawText(oss.str(), font, x, y, Color::White);
}

// ==============================================================================
// الدالة الرئيسية / Main Function
// ==============================================================================

int main(int argc, char* argv[]) {
    std::cout << "=====================================" << std::endl;
    std::cout << "(AR) عرض نظام النصوص والخطوط" << std::endl;
    std::cout << "(EN) Text and Font System Demo" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // إنشاء النافذة / Create window
    Window window("Text Demo - عرض النصوص", WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!window.IsValid()) {                    // التحقق من الصحة / Check validity
        std::cerr << "(AR) فشل إنشاء النافذة! / (EN) Failed to create window!" << std::endl;
        return 1;                               // خطأ / Error
    }
    
    // إنشاء سياق الرسم / Create render context
    RenderContext context(&window);
    if (!context.IsValid()) {                   // التحقق من الصحة / Check validity
        std::cerr << "(AR) فشل إنشاء سياق الرسم! / (EN) Failed to create context!" << std::endl;
        return 1;                               // خطأ / Error
    }
    
    // إنشاء محرك الرسم / Create renderer
    Renderer2D renderer(&context);
    
    // إنشاء مدير المدخلات / Create input manager
    InputManager input;
    
    // حالة التطبيق / Application state
    AppState state;
    
    // تحميل الخطوط / Load fonts
    LoadFonts(state);
    if (!state.running) {                       // إذا فشل التحميل / If loading failed
        return 1;                               // خروج / Exit
    }
    
    // الحلقة الرئيسية / Main loop
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    
    while (state.running) {                     // حلقة العرض / Render loop
        // حساب دلتا الوقت / Calculate delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        float dt = deltaTime.count();           // دلتا بالثواني / Delta in seconds
        
        // معالجة الأحداث / Process events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {       // حدث إغلاق / Quit event
                state.running = false;          // إيقاف / Stop
            }
            input.ProcessEvent(&event);         // معالجة الحدث / Process event
        }
        
        // معالجة المدخلات / Handle input
        HandleInput(state, input);
        
        // تحديث الحالة / Update state
        UpdateState(state, dt);
        
        // تحديث المدخلات / Update input
        input.Update();
        
        // بدء الرسم / Begin rendering
        renderer.BeginFrame();
        renderer.Clear(Color(20, 20, 30, 255)); // خلفية داكنة / Dark background
        
        // رسم العرض المختار / Draw selected demo
        switch (state.selectedDemo) {
            case 0: Demo1_BasicText(renderer, state); break;
            case 1: Demo2_Alignment(renderer, state); break;
            case 2: Demo3_Colors(renderer, state); break;
            case 3: Demo4_AnimatedEffects(renderer, state); break;
            case 4: Demo5_TextMeasurement(renderer, state); break;
        }
        
        // رسم المساعدة والإحصائيات / Draw help and stats
        if (state.showHelp) {
            ShowHelp(renderer, state.fonts[0].font);
        }
        if (state.showStats) {
            ShowStats(renderer, state.fonts[0].font, state);
        }
        
        // إنهاء الرسم / End rendering
        renderer.EndFrame();
        context.SwapBuffers();                  // تبديل المخازن / Swap buffers
    }
    
    std::cout << "(AR) إنهاء العرض / (EN) Demo finished" << std::endl;
    return 0;                                   // نجاح / Success
}
