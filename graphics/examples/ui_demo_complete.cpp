// ============================================================================
// ui_demo_complete.cpp - مثال كامل لجميع عناصر واجهة المستخدم
// Complete UI system demonstration
// ============================================================================
// الوصف: مثال شامل يوضح استخدام جميع عناصر واجهة المستخدم
// Description: Comprehensive example demonstrating all UI widgets
// ============================================================================

#include "../include/window/window.h"              // نافذة التطبيق / Application window
#include "../include/rendering/context.h"          // سياق الرسم / Rendering context
#include "../include/rendering/renderer2d.h"       // محرك الرسم 2D / 2D renderer
#include "../include/input/input_manager.h"        // إدارة الإدخال / Input management
#include "../include/resources/resource_manager.h" // إدارة الموارد / Resource management
#include "../include/ui/widget.h"                  // الفئة الأساسية للعناصر / Base widget class
#include "../include/ui/label.h"                   // عنصر التسمية / Label widget
#include "../include/ui/button.h"                  // عنصر الزر / Button widget
#include "../include/ui/textinput.h"               // عنصر إدخال النص / Text input widget
#include "../include/ui/checkbox.h"                // عنصر مربع الاختيار / Checkbox widget
#include "../include/ui/panel.h"                   // عنصر اللوحة / Panel widget
#include "../include/ui/slider.h"                  // عنصر المنزلق / Slider widget

#include <iostream>    // للطباعة / For printing
#include <memory>      // للمؤشرات الذكية / For smart pointers
#include <vector>      // للمصفوفات / For vectors
#include <string>      // للنصوص / For strings

// استخدام المساحات الاسمية / Using namespaces
using namespace sad::graphics;
using namespace sad::graphics::ui;

// ============================================================================
// حالة التطبيق / Application State
// ============================================================================
struct AppState {
    bool running;              // هل التطبيق يعمل؟ / Is application running?
    bool showHelp;             // هل نعرض المساعدة؟ / Show help?
    int clickCount;            // عدد النقرات / Click count
    std::string statusText;    // نص الحالة / Status text
    bool rememberMe;           // تذكرني / Remember me
    bool showPassword;         // إظهار كلمة المرور / Show password
    float volume;              // مستوى الصوت / Volume level
    float brightness;          // مستوى السطوع / Brightness level

    // البناء الافتراضي / Default constructor
    AppState() 
        : running(true)
        , showHelp(false)
        , clickCount(0)
        , statusText("مرحباً بك في العرض الكامل! / Welcome to Complete UI Demo!")
        , rememberMe(false)
        , showPassword(false)
        , volume(50.0f)
        , brightness(75.0f)
    {}
};

// ============================================================================
// تحديث واجهة المستخدم / Update UI
// ============================================================================
void UpdateUI(std::vector<std::shared_ptr<Widget>>& widgets, AppState& state) {
    // تحديث التسميات بناءً على الحالة / Update labels based on state
    
    // البحث عن تسمية العداد / Find counter label
    for (auto& widget : widgets) {
        auto label = std::dynamic_pointer_cast<Label>(widget);
        if (label && label->GetText().find("Clicks") != std::string::npos) {
            label->SetText("Clicks: " + std::to_string(state.clickCount));
        }
    }
}

// ============================================================================
// إنشاء واجهة المستخدم / Create UI
// ============================================================================
void CreateUI(std::vector<std::shared_ptr<Widget>>& widgets, AppState& state) {
    // الحصول على مدير الموارد / Get resource manager
    auto& rm = ResourceManager::GetInstance();

    // تحميل خط / Load font
    auto font = rm.LoadFont("arial.ttf", 20.0f);
    auto smallFont = rm.LoadFont("arial.ttf", 16.0f);

    // ============================================================================
    // SECTION 1: العنوان الرئيسي / Main Title
    // ============================================================================
    auto title = std::make_shared<Label>("العرض الكامل لواجهة المستخدم / Complete UI Demo", font);
    title->SetPosition(50, 20);
    title->SetSize(1100, 50);
    title->SetAlignment(TextAlignment::MiddleCenter);
    title->SetTextColor(Color::White);
    
    WidgetStyle titleStyle;
    titleStyle.backgroundColor = Color(40, 90, 180);  // أزرق / Blue
    titleStyle.borderColor = Color::White;
    titleStyle.borderWidth = 3.0f;
    titleStyle.padding = 10.0f;
    title->SetStyle(titleStyle);
    widgets.push_back(title);

    // ============================================================================
    // SECTION 2: لوحة معلومات تسجيل الدخول / Login Info Panel
    // ============================================================================
    auto loginPanel = std::make_shared<Panel>(50, 90, 500, 300);
    loginPanel->SetLayout(PanelLayout::Vertical);
    loginPanel->SetSpacing(15.0f);
    loginPanel->SetContentPadding(20.0f);
    
    WidgetStyle panelStyle;
    panelStyle.backgroundColor = Color(240, 240, 240);
    panelStyle.borderColor = Color(180, 180, 180);
    panelStyle.borderWidth = 2.0f;
    loginPanel->SetStyle(panelStyle);
    widgets.push_back(loginPanel);

    // عنوان اللوحة / Panel title
    auto loginTitle = std::make_shared<Label>("تسجيل الدخول / Login", font);
    loginTitle->SetPosition(70, 110);
    loginTitle->SetSize(460, 35);
    loginTitle->SetAlignment(TextAlignment::MiddleLeft);
    loginTitle->SetTextColor(Color(40, 40, 40));
    widgets.push_back(loginTitle);

    // حقل اسم المستخدم / Username field
    auto usernameInput = std::make_shared<TextInput>();
    usernameInput->SetPosition(70, 155);
    usernameInput->SetSize(460, 40);
    usernameInput->SetPlaceholder("اسم المستخدم / Username");
    usernameInput->SetFont(smallFont);
    widgets.push_back(usernameInput);

    // حقل كلمة المرور / Password field
    auto passwordInput = std::make_shared<TextInput>();
    passwordInput->SetPosition(70, 210);
    passwordInput->SetSize(460, 40);
    passwordInput->SetPlaceholder("كلمة المرور / Password");
    passwordInput->SetFont(smallFont);
    passwordInput->SetPasswordMode(true);
    
    // ربط إظهار كلمة المرور / Connect show password
    passwordInput->SetOnTextChange([&state, passwordInput]() {
        if (state.showPassword) {
            passwordInput->SetPasswordMode(false);
        }
    });
    widgets.push_back(passwordInput);

    // خيار تذكرني / Remember me checkbox
    auto rememberCheckbox = std::make_shared<Checkbox>();
    rememberCheckbox->SetPosition(70, 265);
    rememberCheckbox->SetSize(200, 30);
    rememberCheckbox->SetLabel("تذكرني / Remember Me");
    rememberCheckbox->SetFont(smallFont);
    rememberCheckbox->SetOnCheckStateChange([&state](bool checked) {
        state.rememberMe = checked;
        state.statusText = checked ? "سيتم تذكرك / Will remember" : "لن يتم تذكرك / Won't remember";
        std::cout << state.statusText << std::endl;
    });
    widgets.push_back(rememberCheckbox);

    // خيار إظهار كلمة المرور / Show password checkbox
    auto showPasswordCheckbox = std::make_shared<Checkbox>();
    showPasswordCheckbox->SetPosition(300, 265);
    showPasswordCheckbox->SetSize(230, 30);
    showPasswordCheckbox->SetLabel("إظهار كلمة المرور / Show Password");
    showPasswordCheckbox->SetFont(smallFont);
    showPasswordCheckbox->SetOnCheckStateChange([&state, passwordInput](bool checked) {
        state.showPassword = checked;
        passwordInput->SetPasswordMode(!checked);
        state.statusText = checked ? "كلمة المرور ظاهرة / Password visible" : "كلمة المرور مخفية / Password hidden";
        std::cout << state.statusText << std::endl;
    });
    widgets.push_back(showPasswordCheckbox);

    // زر تسجيل الدخول / Login button
    auto loginButton = std::make_shared<Button>("تسجيل الدخول / Login");
    loginButton->SetPosition(70, 315);
    loginButton->SetSize(200, 50);
    loginButton->SetOnClick([&state, usernameInput, passwordInput]() {
        std::string username = usernameInput->GetText();
        std::string password = passwordInput->GetText();
        
        if (username.empty() || password.empty()) {
            state.statusText = "الرجاء ملء جميع الحقول / Please fill all fields";
        } else {
            state.statusText = "مرحباً " + username + " / Welcome " + username;
        }
        std::cout << state.statusText << std::endl;
    });
    widgets.push_back(loginButton);

    // زر إلغاء / Cancel button
    auto cancelButton = std::make_shared<Button>("إلغاء / Cancel");
    cancelButton->SetPosition(290, 315);
    cancelButton->SetSize(200, 50);
    
    // نمط الزر / Button style (red)
    WidgetStyle cancelStyle;
    cancelStyle.backgroundColor = Color(200, 50, 50);
    cancelButton->SetNormalStyle(cancelStyle);
    
    cancelButton->SetOnClick([usernameInput, passwordInput]() {
        usernameInput->SetText("");
        passwordInput->SetText("");
        std::cout << "تم الإلغاء / Cancelled" << std::endl;
    });
    widgets.push_back(cancelButton);

    // ============================================================================
    // SECTION 3: لوحة التحكم / Control Panel
    // ============================================================================
    auto controlPanel = std::make_shared<Panel>(600, 90, 550, 300);
    
    WidgetStyle controlPanelStyle;
    controlPanelStyle.backgroundColor = Color(245, 245, 250);
    controlPanelStyle.borderColor = Color(150, 150, 200);
    controlPanelStyle.borderWidth = 2.0f;
    controlPanel->SetStyle(controlPanelStyle);
    widgets.push_back(controlPanel);

    // عنوان لوحة التحكم / Control panel title
    auto controlTitle = std::make_shared<Label>("الإعدادات / Settings", font);
    controlTitle->SetPosition(620, 110);
    controlTitle->SetSize(510, 35);
    controlTitle->SetAlignment(TextAlignment::MiddleLeft);
    controlTitle->SetTextColor(Color(40, 40, 40));
    widgets.push_back(controlTitle);

    // تسمية مستوى الصوت / Volume label
    auto volumeLabel = std::make_shared<Label>("مستوى الصوت / Volume: 50%", smallFont);
    volumeLabel->SetPosition(620, 160);
    volumeLabel->SetSize(510, 30);
    volumeLabel->SetAlignment(TextAlignment::MiddleLeft);
    volumeLabel->SetTextColor(Color(60, 60, 60));
    widgets.push_back(volumeLabel);

    // منزلق مستوى الصوت / Volume slider
    auto volumeSlider = std::make_shared<Slider>(620, 200, 510, 30, 0.0f, 100.0f, 50.0f);
    volumeSlider->SetStepSize(1.0f);
    volumeSlider->SetShowValue(false);
    volumeSlider->SetOnValueChange([&state, volumeLabel](float value) {
        state.volume = value;
        volumeLabel->SetText("مستوى الصوت / Volume: " + std::to_string((int)value) + "%");
        state.statusText = "تم ضبط الصوت / Volume set to " + std::to_string((int)value) + "%";
    });
    widgets.push_back(volumeSlider);

    // تسمية مستوى السطوع / Brightness label
    auto brightnessLabel = std::make_shared<Label>("السطوع / Brightness: 75%", smallFont);
    brightnessLabel->SetPosition(620, 250);
    brightnessLabel->SetSize(510, 30);
    brightnessLabel->SetAlignment(TextAlignment::MiddleLeft);
    brightnessLabel->SetTextColor(Color(60, 60, 60));
    widgets.push_back(brightnessLabel);

    // منزلق السطوع / Brightness slider
    auto brightnessSlider = std::make_shared<Slider>(620, 290, 510, 30, 0.0f, 100.0f, 75.0f);
    brightnessSlider->SetStepSize(5.0f);
    brightnessSlider->SetShowValue(false);
    brightnessSlider->SetFillColor(Color(255, 200, 50));  // لون أصفر / Yellow color
    brightnessSlider->SetThumbColor(Color(255, 180, 0));
    brightnessSlider->SetOnValueChange([&state, brightnessLabel](float value) {
        state.brightness = value;
        brightnessLabel->SetText("السطوع / Brightness: " + std::to_string((int)value) + "%");
        state.statusText = "تم ضبط السطوع / Brightness set to " + std::to_string((int)value) + "%";
    });
    widgets.push_back(brightnessSlider);

    // زر إعادة الضبط / Reset button
    auto resetButton = std::make_shared<Button>("إعادة ضبط / Reset Settings");
    resetButton->SetPosition(620, 335);
    resetButton->SetSize(250, 45);
    resetButton->SetOnClick([&state, volumeSlider, brightnessSlider, volumeLabel, brightnessLabel]() {
        state.volume = 50.0f;
        state.brightness = 75.0f;
        volumeSlider->SetValue(50.0f);
        brightnessSlider->SetValue(75.0f);
        volumeLabel->SetText("مستوى الصوت / Volume: 50%");
        brightnessLabel->SetText("السطوع / Brightness: 75%");
        state.statusText = "تم إعادة الضبط / Settings reset";
        std::cout << state.statusText << std::endl;
    });
    widgets.push_back(resetButton);

    // زر التطبيق / Apply button
    auto applyButton = std::make_shared<Button>("تطبيق / Apply");
    applyButton->SetPosition(890, 335);
    applyButton->SetSize(240, 45);
    
    // نمط أخضر / Green style
    WidgetStyle applyStyle;
    applyStyle.backgroundColor = Color(50, 180, 50);
    applyButton->SetNormalStyle(applyStyle);
    
    applyButton->SetOnClick([&state]() {
        state.statusText = "تم التطبيق! / Applied! Volume: " + std::to_string((int)state.volume) + "%, Brightness: " + std::to_string((int)state.brightness) + "%";
        std::cout << state.statusText << std::endl;
    });
    widgets.push_back(applyButton);

    // ============================================================================
    // SECTION 4: لوحة الأزرار / Button Panel
    // ============================================================================
    auto buttonPanel = std::make_shared<Panel>(50, 410, 1100, 100);
    buttonPanel->SetLayout(PanelLayout::Horizontal);
    buttonPanel->SetSpacing(20.0f);
    buttonPanel->SetContentPadding(15.0f);
    
    WidgetStyle buttonPanelStyle;
    buttonPanelStyle.backgroundColor = Color(235, 235, 235);
    buttonPanelStyle.borderColor = Color(170, 170, 170);
    buttonPanelStyle.borderWidth = 2.0f;
    buttonPanel->SetStyle(buttonPanelStyle);
    widgets.push_back(buttonPanel);

    // أزرار ملونة / Colored buttons
    struct ButtonInfo {
        std::string text;
        Color color;
    };

    std::vector<ButtonInfo> buttonInfos = {
        {"أحمر / Red", Color(220, 60, 60)},
        {"أخضر / Green", Color(60, 200, 60)},
        {"أزرق / Blue", Color(60, 100, 220)},
        {"أصفر / Yellow", Color(220, 200, 40)},
        {"بنفسجي / Purple", Color(180, 60, 180)}
    };

    float buttonX = 65;
    for (const auto& info : buttonInfos) {
        auto colorButton = std::make_shared<Button>(info.text);
        colorButton->SetPosition(buttonX, 425);
        colorButton->SetSize(200, 70);
        
        WidgetStyle colorStyle;
        colorStyle.backgroundColor = info.color;
        colorButton->SetNormalStyle(colorStyle);
        
        colorButton->SetOnClick([&state, info]() {
            state.clickCount++;
            state.statusText = info.text + " clicked! Total: " + std::to_string(state.clickCount);
            std::cout << state.statusText << std::endl;
        });
        
        widgets.push_back(colorButton);
        buttonX += 220;
    }

    // ============================================================================
    // SECTION 5: شريط الحالة / Status Bar
    // ============================================================================
    auto statusBar = std::make_shared<Label>(state.statusText, smallFont);
    statusBar->SetPosition(50, 530);
    statusBar->SetSize(1100, 40);
    statusBar->SetAlignment(TextAlignment::MiddleLeft);
    statusBar->SetTextColor(Color::White);
    
    WidgetStyle statusStyle;
    statusStyle.backgroundColor = Color(60, 60, 60);
    statusStyle.borderColor = Color(40, 40, 40);
    statusStyle.borderWidth = 1.0f;
    statusStyle.padding = 10.0f;
    statusBar->SetStyle(statusStyle);
    widgets.push_back(statusBar);

    // ============================================================================
    // SECTION 6: نص المساعدة / Help Text
    // ============================================================================
    auto helpText = std::make_shared<Label>(
        "ESC = خروج / Exit | H = مساعدة / Help | TAB = انتقال / Switch Focus",
        smallFont
    );
    helpText->SetPosition(50, 585);
    helpText->SetSize(1100, 35);
    helpText->SetAlignment(TextAlignment::MiddleCenter);
    helpText->SetTextColor(Color(100, 100, 100));
    widgets.push_back(helpText);
}

// ============================================================================
// الدالة الرئيسية / Main function
// ============================================================================
int main(int argc, char* argv[]) {
    std::cout << "==================================================" << std::endl;
    std::cout << "العرض الكامل لواجهة المستخدم / Complete UI Demo" << std::endl;
    std::cout << "==================================================" << std::endl;

    // إنشاء النافذة / Create window
    Window window("Complete UI Demo - العرض الكامل", 1200, 650);
    if (!window.Initialize()) {
        std::cerr << "فشل في إنشاء النافذة / Failed to create window" << std::endl;
        return -1;
    }

    // إنشاء السياق / Create context
    RenderContext context;
    if (!context.Initialize()) {
        std::cerr << "فشل في إنشاء السياق / Failed to create context" << std::endl;
        return -1;
    }

    // إنشاء المحرك / Create renderer
    Renderer2D renderer;
    if (!renderer.Initialize()) {
        std::cerr << "فشل في إنشاء المحرك / Failed to create renderer" << std::endl;
        return -1;
    }

    // إنشاء مدير الإدخال / Create input manager
    InputManager inputManager;

    // حالة التطبيق / Application state
    AppState state;

    // قائمة العناصر / Widget list
    std::vector<std::shared_ptr<Widget>> widgets;

    // إنشاء واجهة المستخدم / Create UI
    CreateUI(widgets, state);

    std::cout << "تم إنشاء " << widgets.size() << " عنصر / Created " << widgets.size() << " widgets" << std::endl;
    std::cout << "اضغط ESC للخروج / Press ESC to exit" << std::endl;
    std::cout << "اضغط H للمساعدة / Press H for help" << std::endl;

    // الحلقة الرئيسية / Main loop
    while (state.running) {
        // معالجة الأحداث / Process events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // تحديث مدير الإدخال / Update input manager
            inputManager.ProcessEvent(event);

            // معالجة أحداث النافذة / Handle window events
            if (event.type == SDL_QUIT) {
                state.running = false;
            }

            // معالجة المفاتيح / Handle keys
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    state.running = false;  // خروج / Exit
                } else if (event.key.keysym.sym == SDLK_h) {
                    state.showHelp = !state.showHelp;  // تبديل المساعدة / Toggle help
                }

                // تمرير الحدث للعناصر / Pass event to widgets
                for (auto& widget : widgets) {
                    widget->OnKeyDown(event.key.keysym.sym);
                }
            }

            // معالجة حركة الفأرة / Handle mouse motion
            if (event.type == SDL_MOUSEMOTION) {
                for (auto& widget : widgets) {
                    widget->OnMouseMove(event.motion.x, event.motion.y);
                }
            }

            // معالجة أزرار الفأرة / Handle mouse buttons
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                for (auto& widget : widgets) {
                    widget->OnMouseDown(event.button.x, event.button.y, event.button.button);
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                for (auto& widget : widgets) {
                    widget->OnMouseUp(event.button.x, event.button.y, event.button.button);
                }
            }

            // معالجة إدخال النص / Handle text input
            if (event.type == SDL_TEXTINPUT) {
                for (auto& widget : widgets) {
                    widget->OnTextInput(event.text.text);
                }
            }
        }

        // تحديث العناصر / Update widgets
        float deltaTime = 0.016f;  // ~60 FPS
        for (auto& widget : widgets) {
            widget->Update(deltaTime);
        }

        // تحديث واجهة المستخدم / Update UI
        UpdateUI(widgets, state);

        // مسح الشاشة / Clear screen
        renderer.Clear(Color(250, 250, 255));  // لون فاتح جداً / Very light color

        // رسم العناصر / Draw widgets
        for (auto& widget : widgets) {
            widget->Draw(&renderer);
        }

        // عرض الإطار / Present frame
        renderer.Present();
        window.SwapBuffers();
    }

    std::cout << "==================================================" << std::endl;
    std::cout << "شكراً لاستخدامك / Thank you for using!" << std::endl;
    std::cout << "إجمالي النقرات / Total clicks: " << state.clickCount << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
