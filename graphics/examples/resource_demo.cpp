// ==============================================================================
// resource_demo.cpp - عرض توضيحي لنظام إدارة الموارد / Resource Management Demo
// ==============================================================================
// الوصف: عرض شامل يوضح استخدام نظام إدارة الموارد
// Description: Comprehensive demo showing resource management system usage
// ==============================================================================

#include "../include/window/window.h"           // نظام النوافذ / Window system
#include "../include/rendering/context.h"       // سياق الرسم / Render context
#include "../include/rendering/renderer2d.h"    // محرك الرسم 2D / 2D renderer
#include "../include/input/input_manager.h"     // إدارة المدخلات / Input management
#include "../include/resources/resource_manager.h"  // مدير الموارد / Resource manager

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

// ==============================================================================
// حالة التطبيق / Application State
// ==============================================================================

/// حالة التطبيق / Application state
struct AppState {
    bool running;                               // يعمل؟ / Running?
    bool showStats;                             // عرض الإحصائيات؟ / Show stats?
    bool showHelp;                              // عرض المساعدة؟ / Show help?
    int selectedDemo;                           // العرض المختار / Selected demo
    
    // إحصائيات الأداء / Performance stats
    float fps;                                  // الإطارات في الثانية / FPS
    float frameTime;                            // وقت الإطار (ms) / Frame time (ms)
    int frameCount;                             // عداد الإطارات / Frame counter
    double lastTime;                            // آخر وقت / Last time
    double fpsTimer;                            // مؤقت FPS / FPS timer
    
    // الموارد المحملة / Loaded resources
    std::vector<TextureRef> textures;           // قائمة textures / Textures list
    std::vector<FontRef> fonts;                 // قائمة الخطوط / Fonts list
    
    // أسماء الموارد / Resource names
    std::vector<std::string> textureNames;      // أسماء textures / Texture names
    std::vector<std::string> fontNames;         // أسماء الخطوط / Font names
    
    // التحريك / Animation
    float animationTime;                        // وقت التحريك / Animation time
    
    /// المُنشئ / Constructor
    AppState()
        : running(true)                         // تشغيل / Running
        , showStats(true)                       // عرض الإحصائيات / Show stats
        , showHelp(true)                        // عرض المساعدة / Show help
        , selectedDemo(0)                       // العرض الأول / First demo
        , fps(0.0f)                             // FPS صفر / Zero FPS
        , frameTime(0.0f)                       // وقت صفر / Zero time
        , frameCount(0)                         // عداد صفر / Zero count
        , lastTime(0.0)                         // وقت صفر / Zero time
        , fpsTimer(0.0)                         // مؤقت صفر / Zero timer
        , animationTime(0.0f)                   // وقت صفر / Zero time
    {}
};

// ==============================================================================
// الدوال المساعدة / Helper Functions
// ==============================================================================

/// تهيئة مسارات البحث / Initialize search paths
void InitializeSearchPaths() {
    auto& rm = ResourceManager::GetInstance();  // الحصول على المدير / Get manager
    
    // إضافة مسارات البحث / Add search paths
    rm.AddSearchPath("assets/");               // مسار الأصول / Assets path
    rm.AddSearchPath("assets/textures/");      // مسار textures / Textures path
    rm.AddSearchPath("assets/fonts/");         // مسار الخطوط / Fonts path
    rm.AddSearchPath("C:/Windows/Fonts/");     // خطوط Windows / Windows fonts
    rm.AddSearchPath("/usr/share/fonts/truetype/dejavu/");  // خطوط Linux / Linux fonts
    
    std::cout << "(AR) تم تهيئة مسارات البحث / (EN) Search paths initialized" << std::endl;
}

/// تحميل الموارد الأساسية / Load essential resources
void LoadEssentialResources(AppState& state) {
    std::cout << "(AR) تحميل الموارد الأساسية... / (EN) Loading essential resources..." << std::endl;
    
    auto& rm = ResourceManager::GetInstance();  // الحصول على المدير / Get manager
    
    // تحميل خط افتراضي / Load default font
    try {
        auto font = rm.LoadFont("arial.ttf", 20.0f);
        if (font) {                             // إذا نجح / If successful
            state.fonts.push_back(font);        // إضافة / Add
            state.fontNames.push_back("arial.ttf (20)");  // حفظ الاسم / Store name
            std::cout << "(AR) تم تحميل خط Arial / (EN) Arial font loaded" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "(AR) خطأ في تحميل خط: " << e.what() 
                 << " / (EN) Error loading font: " << e.what() << std::endl;
    }
    
    // ملاحظة: يمكن إضافة textures هنا إذا كانت متاحة
    // Note: Can add textures here if available
    
    std::cout << "(AR) تم تحميل " << state.fonts.size() << " خط"
             << " / (EN) Loaded " << state.fonts.size() << " font(s)" << std::endl;
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
    
    // تحديث مدير الموارد / Update resource manager
    ResourceManager::GetInstance().Update();    // استدعاء التحديث / Call update
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
    
    // 1-4 لتبديل العروض / 1-4 to switch demos
    if (input.IsKeyPressed(SDLK_1)) state.selectedDemo = 0;
    if (input.IsKeyPressed(SDLK_2)) state.selectedDemo = 1;
    if (input.IsKeyPressed(SDLK_3)) state.selectedDemo = 2;
    if (input.IsKeyPressed(SDLK_4)) state.selectedDemo = 3;
    
    // L لتحميل موارد إضافية / L to load additional resources
    if (input.IsKeyPressed(SDLK_l)) {
        std::cout << "(AR) تحميل موارد إضافية... / (EN) Loading additional resources..." << std::endl;
        
        auto& rm = ResourceManager::GetInstance();
        
        // محاولة تحميل خط آخر / Try loading another font
        auto font = rm.LoadFont("times.ttf", 24.0f);
        if (font) {
            state.fonts.push_back(font);
            state.fontNames.push_back("times.ttf (24)");
            std::cout << "(AR) تم تحميل خط Times / (EN) Times font loaded" << std::endl;
        }
    }
    
    // U لتفريغ موارد غير مستخدمة / U to unload unused resources
    if (input.IsKeyPressed(SDLK_u)) {
        std::cout << "(AR) تفريغ موارد غير مستخدمة... / (EN) Unloading unused resources..." << std::endl;
        
        auto& rm = ResourceManager::GetInstance();
        size_t count = rm.UnloadUnused();
        
        std::cout << "(AR) تم تفريغ " << count << " مورد"
                 << " / (EN) Unloaded " << count << " resources" << std::endl;
    }
    
    // P لطباعة الإحصائيات / P to print statistics
    if (input.IsKeyPressed(SDLK_p)) {
        ResourceManager::GetInstance().PrintStats();
    }
    
    // C لمسح جميع الموارد / C to clear all resources
    if (input.IsKeyPressed(SDLK_c)) {
        std::cout << "(AR) تحذير: مسح جميع الموارد! / (EN) Warning: Clearing all resources!" << std::endl;
        
        state.textures.clear();             // مسح textures / Clear textures
        state.fonts.clear();                // مسح fonts / Clear fonts
        state.textureNames.clear();         // مسح الأسماء / Clear names
        state.fontNames.clear();            // مسح الأسماء / Clear names
        
        ResourceManager::GetInstance().UnloadAll();  // تفريغ الكل / Unload all
        
        // إعادة تحميل الأساسية / Reload essential
        LoadEssentialResources(state);
    }
}

// ==============================================================================
// العروض التوضيحية / Demos
// ==============================================================================

/// عرض 1: تحميل أساسي / Demo 1: Basic Loading
void Demo1_BasicLoading(Renderer2D& renderer, AppState& state) {
    if (state.fonts.empty()) return;         // إذا لم توجد خطوط / If no fonts
    
    const FontRef& font = state.fonts[0];    // الخط الأول / First font
    
    float y = 100.0f;                        // موقع Y البداية / Start Y position
    float lineSpacing = 35.0f;               // المسافة بين الأسطر / Line spacing
    
    // عنوان العرض / Demo title
    renderer.DrawText("Demo 1: Basic Resource Loading", font, 50, y, Color::Yellow);
    y += lineSpacing * 1.5f;
    
    // معلومات / Information
    renderer.DrawText("Resource Manager handles automatic loading and caching", 
                     font, 50, y, Color::White);
    y += lineSpacing;
    
    renderer.DrawText("Resources are loaded only once and cached for reuse", 
                     font, 50, y, Color::White);
    y += lineSpacing;
    
    renderer.DrawText("Memory management is automatic with configurable limits", 
                     font, 50, y, Color::White);
    y += lineSpacing * 2;
    
    // قائمة الموارد المحملة / List of loaded resources
    renderer.DrawText("Loaded Resources:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.5f;
    
    // عرض الخطوط / Display fonts
    renderer.DrawText("Fonts:", font, 70, y, Color::Green);
    y += lineSpacing;
    
    for (size_t i = 0; i < state.fontNames.size(); ++i) {
        std::ostringstream oss;
        oss << "  " << (i + 1) << ". " << state.fontNames[i];
        renderer.DrawText(oss.str(), font, 90, y, Color::White);
        y += lineSpacing;
    }
    
    y += lineSpacing;
    
    // عرض textures / Display textures
    renderer.DrawText("Textures:", font, 70, y, Color::Green);
    y += lineSpacing;
    
    if (state.textures.empty()) {
        renderer.DrawText("  (none loaded)", font, 90, y, Color(150, 150, 150));
    } else {
        for (size_t i = 0; i < state.textureNames.size(); ++i) {
            std::ostringstream oss;
            oss << "  " << (i + 1) << ". " << state.textureNames[i];
            renderer.DrawText(oss.str(), font, 90, y, Color::White);
            y += lineSpacing;
        }
    }
}

/// عرض 2: إحصائيات الذاكرة / Demo 2: Memory Statistics
void Demo2_MemoryStats(Renderer2D& renderer, AppState& state) {
    if (state.fonts.empty()) return;         // إذا لم توجد خطوط / If no fonts
    
    const FontRef& font = state.fonts[0];    // الخط الأول / First font
    auto& rm = ResourceManager::GetInstance();  // مدير الموارد / Resource manager
    const auto& stats = rm.GetStats();       // الإحصائيات / Statistics
    
    float y = 100.0f;                        // موقع Y / Y position
    float lineSpacing = 35.0f;               // المسافة / Spacing
    
    // عنوان / Title
    renderer.DrawText("Demo 2: Memory & Cache Statistics", font, 50, y, Color::Yellow);
    y += lineSpacing * 2;
    
    // إحصائيات الموارد / Resource statistics
    renderer.DrawText("Resource Count:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.2f;
    
    std::ostringstream oss;
    
    oss << "  Total Resources: " << stats.loadedResources;
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Cached Resources: " << stats.cachedResources;
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Textures: " << stats.textureCount;
    renderer.DrawText(oss.str(), font, 70, y, Color::Green);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Fonts: " << stats.fontCount;
    renderer.DrawText(oss.str(), font, 70, y, Color::Green);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Shaders: " << stats.shaderCount;
    renderer.DrawText(oss.str(), font, 70, y, Color::Green);
    y += lineSpacing * 2;
    
    // إحصائيات الذاكرة / Memory statistics
    renderer.DrawText("Memory Usage:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.2f;
    
    float memoryUsedMB = stats.memoryUsed / 1024.0f / 1024.0f;
    float memoryLimitMB = stats.memoryLimit / 1024.0f / 1024.0f;
    float memoryPercent = (stats.memoryUsed * 100.0f) / stats.memoryLimit;
    
    oss.str("");
    oss << "  Used: " << std::fixed << std::setprecision(2) << memoryUsedMB << " MB";
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Limit: " << std::fixed << std::setprecision(2) << memoryLimitMB << " MB";
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Usage: " << std::fixed << std::setprecision(1) << memoryPercent << "%";
    Color usageColor = memoryPercent < 50 ? Color::Green : 
                      memoryPercent < 80 ? Color::Yellow : Color::Red;
    renderer.DrawText(oss.str(), font, 70, y, usageColor);
    y += lineSpacing;
    
    // رسم شريط التقدم / Draw progress bar
    float barX = 70.0f;
    float barWidth = 300.0f;
    float barHeight = 20.0f;
    
    // خلفية / Background
    renderer.DrawRect(barX, y, barWidth, barHeight, Color(50, 50, 50), true);
    
    // التقدم / Progress
    float progressWidth = barWidth * (memoryPercent / 100.0f);
    renderer.DrawRect(barX, y, progressWidth, barHeight, usageColor, true);
    
    // حدود / Border
    renderer.DrawRect(barX, y, barWidth, barHeight, Color::White, false);
}

/// عرض 3: أداء Cache / Demo 3: Cache Performance
void Demo3_CachePerformance(Renderer2D& renderer, AppState& state) {
    if (state.fonts.empty()) return;         // إذا لم توجد خطوط / If no fonts
    
    const FontRef& font = state.fonts[0];    // الخط الأول / First font
    auto& rm = ResourceManager::GetInstance();  // مدير الموارد / Resource manager
    const auto& stats = rm.GetStats();       // الإحصائيات / Statistics
    
    float y = 100.0f;                        // موقع Y / Y position
    float lineSpacing = 35.0f;               // المسافة / Spacing
    
    // عنوان / Title
    renderer.DrawText("Demo 3: Cache Performance", font, 50, y, Color::Yellow);
    y += lineSpacing * 2;
    
    // إحصائيات الأداء / Performance statistics
    renderer.DrawText("Cache Statistics:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.2f;
    
    std::ostringstream oss;
    
    oss << "  Total Load Attempts: " << stats.loadCount;
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Cache Hits: " << stats.cacheHits;
    renderer.DrawText(oss.str(), font, 70, y, Color::Green);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Cache Misses: " << stats.cacheMisses;
    renderer.DrawText(oss.str(), font, 70, y, Color::Red);
    y += lineSpacing;
    
    float hitRatio = stats.GetCacheHitRatio() * 100.0f;
    oss.str("");
    oss << "  Hit Ratio: " << std::fixed << std::setprecision(1) << hitRatio << "%";
    Color ratioColor = hitRatio > 80 ? Color::Green :
                      hitRatio > 50 ? Color::Yellow : Color::Red;
    renderer.DrawText(oss.str(), font, 70, y, ratioColor);
    y += lineSpacing * 2;
    
    // شرح / Explanation
    renderer.DrawText("Cache Performance Explained:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.2f;
    
    renderer.DrawText("- Cache Hit: Resource found in cache (fast)", 
                     font, 70, y, Color::White);
    y += lineSpacing;
    
    renderer.DrawText("- Cache Miss: Resource loaded from disk (slow)", 
                     font, 70, y, Color::White);
    y += lineSpacing;
    
    renderer.DrawText("- Higher hit ratio = Better performance", 
                     font, 70, y, Color::White);
    y += lineSpacing * 2;
    
    // رسم رسم بياني دائري بسيط / Draw simple pie chart
    float centerX = 200.0f;
    float centerY = 500.0f;
    float radius = 80.0f;
    
    if (stats.cacheHits + stats.cacheMisses > 0) {
        float ratio = (float)stats.cacheHits / (stats.cacheHits + stats.cacheMisses);
        float angle = ratio * 360.0f * (3.14159f / 180.0f);  // تحويل لراديان / Convert to radians
        
        // رسم دائرة hits / Draw hits circle
        renderer.DrawCircle(centerX, centerY, radius, Color::Green, true, 32);
        
        // رسم قطاع misses / Draw misses sector
        if (stats.cacheMisses > 0) {
            // ملاحظة: يحتاج تطبيق DrawArc أو DrawPieSlice
            // Note: Needs DrawArc or DrawPieSlice implementation
        }
        
        // تسميات / Labels
        renderer.DrawText("Hits", font, centerX + radius + 20, centerY - 40, Color::Green);
        renderer.DrawText("Misses", font, centerX + radius + 20, centerY + 10, Color::Red);
    }
}

/// عرض 4: إدارة تلقائية / Demo 4: Automatic Management
void Demo4_AutoManagement(Renderer2D& renderer, AppState& state) {
    if (state.fonts.empty()) return;         // إذا لم توجد خطوط / If no fonts
    
    const FontRef& font = state.fonts[0];    // الخط الأول / First font
    auto& rm = ResourceManager::GetInstance();  // مدير الموارد / Resource manager
    
    float y = 100.0f;                        // موقع Y / Y position
    float lineSpacing = 35.0f;               // المسافة / Spacing
    
    // عنوان / Title
    renderer.DrawText("Demo 4: Automatic Resource Management", font, 50, y, Color::Yellow);
    y += lineSpacing * 2;
    
    // الميزات / Features
    renderer.DrawText("Automatic Features:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.2f;
    
    renderer.DrawText("1. Auto-Caching: Resources cached on first load", 
                     font, 70, y, Color::White);
    y += lineSpacing;
    
    renderer.DrawText("2. Auto-Unload: Unused resources freed automatically", 
                     font, 70, y, Color::White);
    y += lineSpacing;
    
    renderer.DrawText("3. Memory Limit: Automatic when limit reached", 
                     font, 70, y, Color::White);
    y += lineSpacing;
    
    renderer.DrawText("4. Thread-Safe: Safe to use from multiple threads", 
                     font, 70, y, Color::White);
    y += lineSpacing * 2;
    
    // الإعدادات الحالية / Current settings
    renderer.DrawText("Current Settings:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.2f;
    
    std::ostringstream oss;
    
    oss << "  Auto-Unload: " << (rm.IsAutoUnloadEnabled() ? "Enabled" : "Disabled");
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Memory Limit: " << (rm.GetMemoryLimit() / 1024 / 1024) << " MB";
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing;
    
    oss.str("");
    oss << "  Resource Limit: " << rm.GetResourceLimit();
    renderer.DrawText(oss.str(), font, 70, y, Color::White);
    y += lineSpacing * 2;
    
    // نصائح / Tips
    renderer.DrawText("Tips:", font, 50, y, Color::Cyan);
    y += lineSpacing * 1.2f;
    
    renderer.DrawText("- Use LoadTexture() for quick loading", 
                     font, 70, y, Color(200, 200, 200));
    y += lineSpacing;
    
    renderer.DrawText("- Resources are shared automatically", 
                     font, 70, y, Color(200, 200, 200));
    y += lineSpacing;
    
    renderer.DrawText("- Manual unload only if needed", 
                     font, 70, y, Color(200, 200, 200));
}

// ==============================================================================
// عرض المساعدة والإحصائيات / Show Help and Stats
// ==============================================================================

/// عرض المساعدة / Show help
void ShowHelp(Renderer2D& renderer, const FontRef& font) {
    float x = 20.0f;                            // موقع X / X position
    float y = WINDOW_HEIGHT - 280.0f;          // موقع Y / Y position
    float lineHeight = 25.0f;                   // ارتفاع السطر / Line height
    
    // خلفية شبه شفافة / Semi-transparent background
    renderer.DrawRect(10, y - 10, 500, 290, Color(0, 0, 0, 180), true);
    
    // عنوان / Title
    renderer.DrawText("Controls (H to toggle):", font, x, y, Color::Yellow);
    y += lineHeight;
    
    // المفاتيح / Keys
    renderer.DrawText("ESC - Exit", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("1-4 - Select Demo", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("S - Toggle Stats", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("L - Load Additional Resources", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("U - Unload Unused Resources", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("P - Print Statistics to Console", font, x, y, Color::White);
    y += lineHeight;
    renderer.DrawText("C - Clear All Resources", font, x, y, Color::White);
}

/// عرض الإحصائيات / Show statistics
void ShowStats(Renderer2D& renderer, const FontRef& font, const AppState& state) {
    float x = WINDOW_WIDTH - 320.0f;           // موقع X / X position
    float y = 20.0f;                            // موقع Y / Y position
    float lineHeight = 25.0f;                   // ارتفاع السطر / Line height
    
    // خلفية / Background
    renderer.DrawRect(x - 10, y - 10, 310, 130, Color(0, 0, 0, 180), true);
    
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
    oss << "Demo: " << (state.selectedDemo + 1) << "/4";
    renderer.DrawText(oss.str(), font, x, y, Color::White);
}

// ==============================================================================
// الدالة الرئيسية / Main Function
// ==============================================================================

int main(int argc, char* argv[]) {
    std::cout << "=====================================" << std::endl;
    std::cout << "(AR) عرض نظام إدارة الموارد" << std::endl;
    std::cout << "(EN) Resource Management System Demo" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // إنشاء النافذة / Create window
    Window window("Resource Manager Demo - عرض إدارة الموارد", WINDOW_WIDTH, WINDOW_HEIGHT);
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
    
    // تهيئة نظام الموارد / Initialize resource system
    InitializeSearchPaths();                    // مسارات البحث / Search paths
    LoadEssentialResources(state);              // تحميل الموارد / Load resources
    
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
            case 0: Demo1_BasicLoading(renderer, state); break;
            case 1: Demo2_MemoryStats(renderer, state); break;
            case 2: Demo3_CachePerformance(renderer, state); break;
            case 3: Demo4_AutoManagement(renderer, state); break;
        }
        
        // رسم المساعدة والإحصائيات / Draw help and stats
        if (state.fonts.size() > 0) {           // إذا كانت هناك خطوط / If fonts available
            if (state.showHelp) {
                ShowHelp(renderer, state.fonts[0]);
            }
            if (state.showStats) {
                ShowStats(renderer, state.fonts[0], state);
            }
        }
        
        // إنهاء الرسم / End rendering
        renderer.EndFrame();
        context.SwapBuffers();                  // تبديل المخازن / Swap buffers
    }
    
    std::cout << "(AR) إنهاء العرض / (EN) Demo finished" << std::endl;
    
    // طباعة الإحصائيات النهائية / Print final statistics
    ResourceManager::GetInstance().PrintStats();
    
    return 0;                                   // نجاح / Success
}
