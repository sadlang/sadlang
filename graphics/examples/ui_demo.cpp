// ============================================================================
// ui_demo.cpp - مثال تطبيقي لنظام واجهة المستخدم
// UI system demonstration example
// ============================================================================
// الوصف: مثال شامل يوضح استخدام عناصر واجهة المستخدم
// Description: Comprehensive example demonstrating UI widget usage
// ============================================================================

#include "../include/window/window.h"              // نافذة التطبيق / Application window
#include "../include/rendering/context.h"          // سياق الرسم / Rendering context
#include "../include/rendering/renderer2d.h"       // محرك الرسم 2D / 2D renderer
#include "../include/input/input_manager.h"        // إدارة الإدخال / Input management
#include "../include/resources/resource_manager.h" // إدارة الموارد / Resource management
#include "../include/ui/widget.h"                  // الفئة الأساسية للعناصر / Base widget class
#include "../include/ui/label.h"                   // عنصر التسمية / Label widget
#include "../include/ui/button.h"                  // عنصر الزر / Button widget

#include <iostream>    // للطباعة / For printing
#include <memory>      // للمؤشرات الذكية / For smart pointers
#include <vector>      // للمصفوفات / For vectors
#include <string>      // للنصوص / For strings

// استخدام المساحات الاسمية / Using namespaces
using namespace Graphics;
using namespace Graphics::UI;

// ============================================================================
// حالة التطبيق / Application State
// ============================================================================
struct AppState {
    bool running;           // هل التطبيق يعمل؟ / Is application running?
    bool showHelp;          // هل نعرض المساعدة؟ / Show help?
    int clickCount;         // عدد النقرات / Click count
    std::string statusText; // نص الحالة / Status text

    // البناء الافتراضي / Default constructor
    AppState() 
        : running(true)
        , showHelp(true)
        , clickCount(0)
        , statusText("Welcome to UI Demo!")
    {}
};

// ============================================================================
// إنشاء واجهة المستخدم / Create UI
// ============================================================================
void CreateUI(std::vector<std::shared_ptr<Widget>>& widgets, AppState& state) {
    // الحصول على مدير الموارد / Get resource manager
    auto& rm = ResourceManager::GetInstance();

    // تحميل خط / Load font
    auto font = rm.LoadFont("arial.ttf", 20.0f);

    // ============================================================================
    // 1. عنوان رئيسي / Main title
    // ============================================================================
    auto title = std::make_shared<Label>("UI System Demo", font);  // إنشاء تسمية / Create label
    title->SetPosition(320, 20);                                    // ضبط الموقع / Set position
    title->SetSize(560, 50);                                        // ضبط الحجم / Set size
    title->SetAlignment(TextAlignment::MiddleCenter);               // محاذاة للمركز / Center alignment
    title->SetTextColor(Color::White);                              // لون أبيض / White color
    
    // نمط العنوان / Title style
    WidgetStyle titleStyle;
    titleStyle.backgroundColor = Color(50, 50, 150);  // خلفية زرقاء داكنة / Dark blue background
    titleStyle.borderColor = Color::White;            // حدود بيضاء / White border
    titleStyle.borderWidth = 3.0f;                    // عرض الحدود / Border width
    titleStyle.padding = 10.0f;                       // مسافة داخلية / Padding
    title->SetStyle(titleStyle);
    
    widgets.push_back(title);  // إضافة للقائمة / Add to list

    // ============================================================================
    // 2. زر النقر / Click button
    // ============================================================================
    auto clickButton = std::make_shared<Button>("Click Me!");  // إنشاء زر / Create button
    clickButton->SetPosition(440, 100);                        // ضبط الموقع / Set position
    clickButton->SetSize(200, 50);                             // ضبط الحجم / Set size
    
    // دالة عند النقر / Click callback
    clickButton->SetOnClick([&state]() {
        state.clickCount++;  // زيادة العداد / Increment counter
        state.statusText = "Button clicked " + std::to_string(state.clickCount) + " times!";
        std::cout << state.statusText << std::endl;  // طباعة / Print
    });

    widgets.push_back(clickButton);  // إضافة للقائمة / Add to list

    // ============================================================================
    // 3. تسمية عداد النقرات / Click counter label
    // ============================================================================
    auto counterLabel = std::make_shared<Label>("Clicks: 0");  // إنشاء تسمية / Create label
    counterLabel->SetPosition(440, 170);                       // ضبط الموقع / Set position
    counterLabel->SetSize(200, 40);                            // ضبط الحجم / Set size
    counterLabel->SetAlignment(TextAlignment::MiddleCenter);   // محاذاة للمركز / Center alignment
    counterLabel->SetTextColor(Color::Black);                  // لون أسود / Black color
    
    // نمط العداد / Counter style
    WidgetStyle counterStyle;
    counterStyle.backgroundColor = Color(220, 220, 220);  // خلفية رمادية فاتحة / Light gray background
    counterStyle.borderColor = Color::Black;              // حدود سوداء / Black border
    counterStyle.borderWidth = 2.0f;                      // عرض الحدود / Border width
    counterLabel->SetStyle(counterStyle);
    
    widgets.push_back(counterLabel);  // إضافة للقائمة / Add to list

    // ============================================================================
    // 4. زر إعادة الضبط / Reset button
    // ============================================================================
    auto resetButton = std::make_shared<Button>("Reset Counter");  // إنشاء زر / Create button
    resetButton->SetPosition(440, 230);                            // ضبط الموقع / Set position
    resetButton->SetSize(200, 50);                                 // ضبط الحجم / Set size
    
    // دالة عند النقر / Click callback
    resetButton->SetOnClick([&state]() {
        state.clickCount = 0;  // إعادة العداد / Reset counter
        state.statusText = "Counter reset!";
        std::cout << state.statusText << std::endl;  // طباعة / Print
    });

    widgets.push_back(resetButton);  // إضافة للقائمة / Add to list

    // ============================================================================
    // 5. أزرار ملونة / Colored buttons
    // ============================================================================
    
    // زر أحمر / Red button
    auto redButton = std::make_shared<Button>("Red");  // إنشاء زر / Create button
    redButton->SetPosition(100, 350);                  // ضبط الموقع / Set position
    redButton->SetSize(150, 50);                       // ضبط الحجم / Set size
    
    // نمط أحمر / Red style
    WidgetStyle redStyle = redButton->GetStyle();
    redStyle.backgroundColor = Color(200, 50, 50);  // خلفية حمراء / Red background
    redStyle.foregroundColor = Color::White;        // نص أبيض / White text
    redButton->SetNormalStyle(redStyle);
    
    redButton->SetOnClick([&state]() {
        state.statusText = "Red button clicked!";
        std::cout << state.statusText << std::endl;
    });
    
    widgets.push_back(redButton);

    // زر أخضر / Green button
    auto greenButton = std::make_shared<Button>("Green");  // إنشاء زر / Create button
    greenButton->SetPosition(280, 350);                    // ضبط الموقع / Set position
    greenButton->SetSize(150, 50);                         // ضبط الحجم / Set size
    
    // نمط أخضر / Green style
    WidgetStyle greenStyle = greenButton->GetStyle();
    greenStyle.backgroundColor = Color(50, 200, 50);  // خلفية خضراء / Green background
    greenStyle.foregroundColor = Color::White;        // نص أبيض / White text
    greenButton->SetNormalStyle(greenStyle);
    
    greenButton->SetOnClick([&state]() {
        state.statusText = "Green button clicked!";
        std::cout << state.statusText << std::endl;
    });
    
    widgets.push_back(greenButton);

    // زر أزرق / Blue button
    auto blueButton = std::make_shared<Button>("Blue");  // إنشاء زر / Create button
    blueButton->SetPosition(460, 350);                   // ضبط الموقع / Set position
    blueButton->SetSize(150, 50);                        // ضبط الحجم / Set size
    
    // نمط أزرق / Blue style
    WidgetStyle blueStyle = blueButton->GetStyle();
    blueStyle.backgroundColor = Color(50, 50, 200);  // خلفية زرقاء / Blue background
    blueStyle.foregroundColor = Color::White;        // نص أبيض / White text
    blueButton->SetNormalStyle(blueStyle);
    
    blueButton->SetOnClick([&state]() {
        state.statusText = "Blue button clicked!";
        std::cout << state.statusText << std::endl;
    });
    
    widgets.push_back(blueButton);

    // زر أصفر / Yellow button
    auto yellowButton = std::make_shared<Button>("Yellow");  // إنشاء زر / Create button
    yellowButton->SetPosition(640, 350);                     // ضبط الموقع / Set position
    yellowButton->SetSize(150, 50);                          // ضبط الحجم / Set size
    
    // نمط أصفر / Yellow style
    WidgetStyle yellowStyle = yellowButton->GetStyle();
    yellowStyle.backgroundColor = Color(220, 220, 50);  // خلفية صفراء / Yellow background
    yellowStyle.foregroundColor = Color::Black;         // نص أسود / Black text
    yellowButton->SetNormalStyle(yellowStyle);
    
    yellowButton->SetOnClick([&state]() {
        state.statusText = "Yellow button clicked!";
        std::cout << state.statusText << std::endl;
    });
    
    widgets.push_back(yellowButton);

    // ============================================================================
    // 6. شريط الحالة / Status bar
    // ============================================================================
    auto statusLabel = std::make_shared<Label>(state.statusText);  // إنشاء تسمية / Create label
    statusLabel->SetPosition(50, 520);                             // ضبط الموقع / Set position
    statusLabel->SetSize(1180, 40);                                // ضبط الحجم / Set size
    statusLabel->SetAlignment(TextAlignment::MiddleLeft);          // محاذاة لليسار / Left alignment
    statusLabel->SetTextColor(Color::White);                       // لون أبيض / White color
    
    // نمط شريط الحالة / Status bar style
    WidgetStyle statusStyle;
    statusStyle.backgroundColor = Color(80, 80, 80);  // خلفية رمادية داكنة / Dark gray background
    statusStyle.borderColor = Color::Black;           // حدود سوداء / Black border
    statusStyle.borderWidth = 2.0f;                   // عرض الحدود / Border width
    statusStyle.padding = 10.0f;                      // مسافة داخلية / Padding
    statusLabel->SetStyle(statusStyle);
    
    widgets.push_back(statusLabel);  // إضافة للقائمة / Add to list

    // ============================================================================
    // 7. معلومات المساعدة / Help info
    // ============================================================================
    auto helpLabel = std::make_shared<Label>("Press H to toggle help | ESC to exit");  // إنشاء تسمية / Create label
    helpLabel->SetPosition(50, 580);                                                   // ضبط الموقع / Set position
    helpLabel->SetSize(1180, 30);                                                      // ضبط الحجم / Set size
    helpLabel->SetAlignment(TextAlignment::MiddleCenter);                              // محاذاة للمركز / Center alignment
    helpLabel->SetTextColor(Color(180, 180, 180));                                     // لون رمادي فاتح / Light gray
    
    widgets.push_back(helpLabel);  // إضافة للقائمة / Add to list
}

// ============================================================================
// تحديث واجهة المستخدم / Update UI
// ============================================================================
void UpdateUI(std::vector<std::shared_ptr<Widget>>& widgets, AppState& state, float deltaTime) {
    // تحديث جميع العناصر / Update all widgets
    for (auto& widget : widgets) {
        if (widget) {
            widget->Update(deltaTime);  // تحديث العنصر / Update widget
        }
    }

    // تحديث تسمية العداد / Update counter label
    if (widgets.size() >= 3) {
        auto counterLabel = std::dynamic_pointer_cast<Label>(widgets[2]);  // الحصول على التسمية / Get label
        if (counterLabel) {
            counterLabel->SetText("Clicks: " + std::to_string(state.clickCount));  // تحديث النص / Update text
        }
    }

    // تحديث شريط الحالة / Update status bar
    if (widgets.size() >= 9) {
        auto statusLabel = std::dynamic_pointer_cast<Label>(widgets[8]);  // الحصول على التسمية / Get label
        if (statusLabel) {
            statusLabel->SetText(state.statusText);  // تحديث النص / Update text
        }
    }
}

// ============================================================================
// معالجة الإدخال / Handle Input
// ============================================================================
void HandleInput(InputManager& input, std::vector<std::shared_ptr<Widget>>& widgets, AppState& state) {
    // الخروج / Exit
    if (input.IsKeyPressed(KeyCode::Escape)) {
        state.running = false;  // إيقاف التطبيق / Stop application
    }

    // تبديل المساعدة / Toggle help
    if (input.IsKeyPressed(KeyCode::H)) {
        state.showHelp = !state.showHelp;  // تبديل / Toggle
        state.statusText = state.showHelp ? "Help shown" : "Help hidden";
    }

    // الحصول على موقع الفأرة / Get mouse position
    int mouseX, mouseY;
    input.GetMousePosition(mouseX, mouseY);

    // معالجة حركة الفأرة / Handle mouse move
    for (auto& widget : widgets) {
        if (widget) {
            widget->OnMouseMove(mouseX, mouseY);  // إرسال حدث الحركة / Send move event
        }
    }

    // معالجة أزرار الفأرة / Handle mouse buttons
    if (input.IsMouseButtonPressed(MouseButton::Left)) {
        // ضغط زر الفأرة / Mouse button pressed
        for (auto& widget : widgets) {
            if (widget) {
                widget->OnMouseDown(mouseX, mouseY, 0);  // إرسال حدث الضغط / Send press event
            }
        }
    }

    if (input.IsMouseButtonReleased(MouseButton::Left)) {
        // رفع زر الفأرة / Mouse button released
        for (auto& widget : widgets) {
            if (widget) {
                widget->OnMouseUp(mouseX, mouseY, 0);  // إرسال حدث الرفع / Send release event
            }
        }
    }
}

// ============================================================================
// الدالة الرئيسية / Main Function
// ============================================================================
int main() {
    std::cout << "=== UI Demo Starting ===" << std::endl;  // طباعة بداية / Print start

    try {
        // ============================================================================
        // إنشاء النافذة / Create Window
        // ============================================================================
        Window window("UI System Demo", 1280, 720);  // نافذة 1280×720 / 1280×720 window
        if (!window.IsOpen()) {
            std::cerr << "Failed to create window!" << std::endl;  // فشل إنشاء النافذة / Window creation failed
            return 1;
        }

        // ============================================================================
        // إنشاء السياق والمحرك / Create Context and Renderer
        // ============================================================================
        RenderContext context(&window);                      // سياق الرسم / Rendering context
        Renderer2D renderer(&context);                       // محرك الرسم 2D / 2D renderer
        InputManager& input = InputManager::GetInstance();   // مدير الإدخال / Input manager

        // ============================================================================
        // تهيئة إدارة الموارد / Initialize Resource Manager
        // ============================================================================
        auto& rm = ResourceManager::GetInstance();
        rm.AddSearchPath("assets/fonts/");           // إضافة مسار الخطوط / Add fonts path
        rm.AddSearchPath("C:/Windows/Fonts/");      // إضافة مسار خطوط Windows / Add Windows fonts path

        // ============================================================================
        // إنشاء حالة التطبيق وواجهة المستخدم / Create App State and UI
        // ============================================================================
        AppState state;                               // حالة التطبيق / App state
        std::vector<std::shared_ptr<Widget>> widgets; // قائمة العناصر / Widget list
        CreateUI(widgets, state);                     // إنشاء الواجهة / Create UI

        std::cout << "UI created with " << widgets.size() << " widgets" << std::endl;  // طباعة / Print

        // ============================================================================
        // حلقة التطبيق الرئيسية / Main Application Loop
        // ============================================================================
        float lastTime = 0.0f;  // الوقت السابق / Last time
        
        while (state.running && window.IsOpen()) {
            // حساب الوقت / Calculate time
            float currentTime = SDL_GetTicks() / 1000.0f;  // الوقت الحالي بالثواني / Current time in seconds
            float deltaTime = currentTime - lastTime;       // الفرق الزمني / Time delta
            lastTime = currentTime;                         // تحديث الوقت / Update time

            // معالجة الأحداث / Process events
            window.PollEvents();
            input.Update();

            // معالجة الإدخال / Handle input
            HandleInput(input, widgets, state);

            // تحديث الواجهة / Update UI
            UpdateUI(widgets, state, deltaTime);

            // الرسم / Rendering
            renderer.BeginFrame();                   // بداية الإطار / Begin frame
            renderer.Clear(Color(30, 30, 40));       // مسح بلون رمادي داكن / Clear with dark gray

            // رسم جميع العناصر / Draw all widgets
            for (auto& widget : widgets) {
                if (widget) {
                    widget->Draw(&renderer);  // رسم العنصر / Draw widget
                }
            }

            renderer.EndFrame();                     // نهاية الإطار / End frame
            context.SwapBuffers();                   // عرض الإطار / Display frame
        }

        std::cout << "=== UI Demo Finished ===" << std::endl;  // طباعة النهاية / Print end
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;  // طباعة الخطأ / Print error
        return 1;
    }

    return 0;  // نجاح / Success
}
