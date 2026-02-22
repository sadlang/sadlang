/**
 * ==========================================================================
 * ملف: sad_ui_desktop_demo.cpp
 * الوصف: تطبيق سطح المكتب التجريبي — ٨ شاشات
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * يستخدم مباشرة API الإطار الحقيقي (sad_ui_framework.h)
 * لعرض ٨ شاشات واجهة مستخدم تفاعلية.
 *
 * بناء:  cmake --build build --config Debug --target sad_desktop_demo
 * تشغيل: .\build\bin\Debug\sad_desktop_demo.exe
 */

#include "sad_ui_framework.h"
#include "sad_ui_internal.h"
#include "sad_navigator.h"
#include "sad_state.h"

#include <iostream>
#include <string>
#include <vector>

using namespace sad::ui;

// ═══════════════════════════════════════════════════════════════════════════
// الثيمة والألوان
// ═══════════════════════════════════════════════════════════════════════════

namespace colors {
    static const RGBA primary     = {41, 98, 255, 255};
    static const RGBA secondary   = {0, 200, 83, 255};
    static const RGBA background  = {245, 245, 248, 255};
    static const RGBA surface     = {255, 255, 255, 255};
    static const RGBA textPrimary = {30, 30, 30, 255};
    static const RGBA textSecondary={120, 120, 130, 255};
    static const RGBA white       = {255, 255, 255, 255};
    static const RGBA error       = {233, 30, 99, 255};
    static const RGBA purple      = {156, 39, 176, 255};
    static const RGBA orange      = {255, 152, 0, 255};
    static const RGBA cyan        = {3, 169, 244, 255};
    static const RGBA blueGrey    = {96, 125, 139, 255};
    static const RGBA divider     = {230, 230, 235, 255};
}

// ═══════════════════════════════════════════════════════════════════════════
// مساعدات البناء
// ═══════════════════════════════════════════════════════════════════════════

static int make_text(const std::string& content, RGBA color, float size = 20, bool bold_flag = false) {
    int w = widget_text(content);
    text_set_color(w, color.r, color.g, color.b);
    text_set_size(w, size);
    return w;
}

static int make_button(const std::string& label, RGBA bgColor, RGBA textColor = colors::white) {
    int w = widget_button(label);
    button_set_color(w, bgColor.r, bgColor.g, bgColor.b);
    text_set_color(w, textColor.r, textColor.g, textColor.b);
    widget_set_border_radius(w, 12);
    widget_set_padding(w, 14);
    return w;
}

static int make_nav_button(const std::string& label, RGBA bgColor, const std::string& route) {
    int w = make_button(label, bgColor);
    // Navigation handled in event loop via button name
    auto& widgetRef = g_widgets[w];
    widgetRef.widgetName = route;
    return w;
}

static int make_section_title(const std::string& title) {
    int w = make_text(title, colors::textPrimary, 22, true);
    return w;
}

static int make_card(int contentWidget) {
    int card = widget_card();
    widget_set_bg(card, colors::surface.r, colors::surface.g, colors::surface.b);
    widget_set_border_radius(card, 12);
    widget_set_padding(card, 16);
    widget_add_child(card, contentWidget);
    return card;
}

static int make_divider() {
    int d = widget_divider();
    return d;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ١: الرئيسية — قائمة التنقل
// ═══════════════════════════════════════════════════════════════════════════

static int build_home(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 24);

    // العنوان
    int title = make_text("\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xb5 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xac\xd8\xb1\xd9\x8a\xd8\xa8\xd9\x8a",
                          colors::primary, 28, true);  // تطبيق ص التجريبي
    widget_add_child(root, title);

    int spacer1 = widget_spacer(8);
    widget_add_child(root, spacer1);

    int desc = make_text("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb1 \xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9 \xd9\x84\xd9\x84\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb9\xd8\xb1\xd8\xa7\xd8\xb6",
                         colors::textSecondary, 16);  // اختر شاشة للاستعراض
    widget_add_child(root, desc);

    int spacer2 = widget_spacer(20);
    widget_add_child(root, spacer2);

    // أزرار التنقل
    struct NavItem { std::string label; RGBA color; std::string route; };
    std::vector<NavItem> items = {
        {"\xd8\xa7\xd9\x84\xd9\x85\xd9\x84\xd9\x81 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xae\xd8\xb5\xd9\x8a", colors::primary, "profile"},   // الملف الشخصي
        {"\xd8\xa7\xd9\x84\xd9\x85\xd9\x87\xd8\xa7\xd9\x85", colors::secondary, "tasks"},       // المهام
        {"\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa", colors::purple, "settings"},    // الإعدادات
        {"\xd8\xa7\xd9\x84\xd9\x85\xd8\xad\xd8\xa7\xd8\xaf\xd8\xab\xd8\xa7\xd8\xaa", colors::orange, "chat"},       // المحادثات
        {"\xd8\xa7\xd9\x84\xd8\xb7\xd9\x82\xd8\xb3", colors::cyan, "weather"},            // الطقس
        {"\xd8\xa7\xd9\x84\xd9\x85\xd8\xaa\xd8\xac\xd8\xb1", colors::error, "store"},           // المتجر
        {"\xd8\xad\xd9\x88\xd9\x84", colors::blueGrey, "about"},             // حول
    };

    for (const auto& item : items) {
        int btn = make_nav_button(item.label, item.color, item.route);
        widget_add_child(root, btn);
        int sp = widget_spacer(8);
        widget_add_child(root, sp);
    }

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ٢: الملف الشخصي
// ═══════════════════════════════════════════════════════════════════════════

static int build_profile(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 24);

    // زر رجوع
    int backBtn = make_button("\xe2\x86\x90 \xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", colors::blueGrey);  // ← رجوع
    g_widgets[backBtn].widgetName = "back";
    widget_add_child(root, backBtn);

    int sp1 = widget_spacer(16);
    widget_add_child(root, sp1);

    // الأفاتار
    int avatar = widget_container();
    widget_set_bg(avatar, colors::primary.r, colors::primary.g, colors::primary.b);
    widget_set_width(avatar, 80);
    widget_set_height(avatar, 80);
    widget_set_border_radius(avatar, 40);
    int avatarText = make_text("\xd8\xa3", colors::white, 32);  // أ
    widget_add_child(avatar, avatarText);
    widget_add_child(root, avatar);

    int sp2 = widget_spacer(16);
    widget_add_child(root, sp2);

    // الاسم
    int nameLabel = make_text("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85:", colors::textSecondary, 14);  // الاسم:
    widget_add_child(root, nameLabel);

    int nameField = widget_textfield("\xd8\xa3\xd8\xad\xd9\x85\xd8\xaf \xd8\xa7\xd9\x84\xd8\xae\xd8\xa7\xd9\x84\xd8\xaf\xd9\x8a");  // أحمد الخالدي
    widget_set_border(nameField, 1, 200, 200, 210);
    widget_set_border_radius(nameField, 8);
    widget_set_padding(nameField, 12);
    g_widgets[nameField].widgetName = "name_field";
    widget_add_child(root, nameField);

    int sp3 = widget_spacer(12);
    widget_add_child(root, sp3);

    // البريد
    int emailLabel = make_text("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xb1\xd9\x8a\xd8\xaf:", colors::textSecondary, 14);  // البريد:
    widget_add_child(root, emailLabel);

    int emailField = widget_textfield("ahmad@sad-lang.org");
    widget_set_border(emailField, 1, 200, 200, 210);
    widget_set_border_radius(emailField, 8);
    widget_set_padding(emailField, 12);
    g_widgets[emailField].widgetName = "email_field";
    widget_add_child(root, emailField);

    int sp4 = widget_spacer(20);
    widget_add_child(root, sp4);

    // زر الحفظ
    int saveBtn = make_button("\xd8\xad\xd9\x81\xd8\xb8 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaa", colors::primary);  // حفظ التغييرات
    g_widgets[saveBtn].widgetName = "save";
    widget_add_child(root, saveBtn);

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ٣: المهام
// ═══════════════════════════════════════════════════════════════════════════

static int build_tasks(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 24);

    int backBtn = make_button("\xe2\x86\x90 \xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", colors::blueGrey);
    g_widgets[backBtn].widgetName = "back";
    widget_add_child(root, backBtn);

    int sp1 = widget_spacer(16);
    widget_add_child(root, sp1);

    int title = make_section_title("\xd8\xa7\xd9\x84\xd9\x85\xd9\x87\xd8\xa7\xd9\x85");  // المهام
    widget_add_child(root, title);

    int sp2 = widget_spacer(12);
    widget_add_child(root, sp2);

    // إدخال مهمة جديدة
    int inputRow = widget_row();
    int taskInput = widget_textfield("\xd9\x85\xd9\x87\xd9\x85\xd8\xa9 \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9...");  // مهمة جديدة...
    widget_set_border(taskInput, 1, 200, 200, 210);
    widget_set_border_radius(taskInput, 8);
    g_widgets[taskInput].widgetName = "task_input";
    widget_add_child(inputRow, taskInput);
    int addBtn = make_button("+", colors::secondary);
    g_widgets[addBtn].widgetName = "add_task";
    widget_add_child(inputRow, addBtn);
    widget_add_child(root, inputRow);

    int sp3 = widget_spacer(16);
    widget_add_child(root, sp3);

    // قائمة المهام
    std::vector<std::string> tasks = {
        "\xd9\x85\xd8\xb1\xd8\xa7\xd8\xac\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x83\xd9\x88\xd8\xaf",              // مراجعة الكود
        "\xd9\x83\xd8\xaa\xd8\xa7\xd8\xa8\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa",  // كتابة الاختبارات
        "\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xab \xd8\xa7\xd9\x84\xd9\x88\xd8\xab\xd8\xa7\xd8\xa6\xd9\x82",      // تحديث الوثائق
    };

    for (const auto& task : tasks) {
        int card = widget_card();
        widget_set_bg(card, colors::surface.r, colors::surface.g, colors::surface.b);
        widget_set_border_radius(card, 10);
        widget_set_padding(card, 14);

        int row = widget_row();
        int checkbox = widget_checkbox(task);
        widget_add_child(row, checkbox);
        int label = make_text(task, colors::textPrimary, 16);
        widget_add_child(row, label);
        widget_add_child(card, row);

        widget_add_child(root, card);

        int sp = widget_spacer(8);
        widget_add_child(root, sp);
    }

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ٤: الإعدادات
// ═══════════════════════════════════════════════════════════════════════════

static int build_settings(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 24);

    int backBtn = make_button("\xe2\x86\x90 \xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", colors::blueGrey);
    g_widgets[backBtn].widgetName = "back";
    widget_add_child(root, backBtn);

    int sp1 = widget_spacer(16);
    widget_add_child(root, sp1);

    int title = make_section_title("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa");  // الإعدادات
    widget_add_child(root, title);

    int sp2 = widget_spacer(16);
    widget_add_child(root, sp2);

    // الوضع الداكن
    {
        int card = widget_card();
        widget_set_bg(card, colors::surface.r, colors::surface.g, colors::surface.b);
        widget_set_border_radius(card, 12);
        widget_set_padding(card, 16);

        int row = widget_row();
        int label = make_text("\xd8\xa7\xd9\x84\xd9\x88\xd8\xb6\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd9\x83\xd9\x86", colors::textPrimary, 16);  // الوضع الداكن
        widget_add_child(row, label);
        int sw = widget_switch("\xd8\xa7\xd9\x84\xd9\x88\xd8\xb6\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xaf\xd8\xa7\xd9\x83\xd9\x86");
        g_widgets[sw].widgetName = "dark_mode";
        widget_add_child(row, sw);
        widget_add_child(card, row);
        widget_add_child(root, card);
    }

    int sp3 = widget_spacer(12);
    widget_add_child(root, sp3);

    // الإشعارات
    {
        int card = widget_card();
        widget_set_bg(card, colors::surface.r, colors::surface.g, colors::surface.b);
        widget_set_border_radius(card, 12);
        widget_set_padding(card, 16);

        int row = widget_row();
        int label = make_text("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa", colors::textPrimary, 16);  // الإشعارات
        widget_add_child(row, label);
        int sw = widget_switch("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1\xd8\xa7\xd8\xaa");
        g_widgets[sw].widgetName = "notifications";
        widget_set_checked(sw, true);
        widget_add_child(row, sw);
        widget_add_child(card, row);
        widget_add_child(root, card);
    }

    int sp4 = widget_spacer(12);
    widget_add_child(root, sp4);

    // حجم الخط
    {
        int card = widget_card();
        widget_set_bg(card, colors::surface.r, colors::surface.g, colors::surface.b);
        widget_set_border_radius(card, 12);
        widget_set_padding(card, 16);

        int col = widget_column();
        int label = make_text("\xd8\xad\xd8\xac\xd9\x85 \xd8\xa7\xd9\x84\xd8\xae\xd8\xb7", colors::textPrimary, 16);  // حجم الخط
        widget_add_child(col, label);
        int slider = widget_slider(10, 30);
        widget_set_value(slider, 16);
        g_widgets[slider].widgetName = "font_size";
        widget_add_child(col, slider);
        widget_add_child(card, col);
        widget_add_child(root, card);
    }

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ٥: المحادثات
// ═══════════════════════════════════════════════════════════════════════════

static int build_chat(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 16);

    int backBtn = make_button("\xe2\x86\x90 \xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", colors::blueGrey);
    g_widgets[backBtn].widgetName = "back";
    widget_add_child(root, backBtn);

    int sp1 = widget_spacer(12);
    widget_add_child(root, sp1);

    int title = make_section_title("\xd8\xa7\xd9\x84\xd9\x85\xd8\xad\xd8\xa7\xd8\xaf\xd8\xab\xd8\xa7\xd8\xaa");  // المحادثات
    widget_add_child(root, title);

    int sp2 = widget_spacer(12);
    widget_add_child(root, sp2);

    // الرسائل
    struct Message { std::string text; bool isMine; };
    std::vector<Message> messages = {
        {"\xd9\x85\xd8\xb1\xd8\xad\xd8\xa8\xd8\xa7\xd9\x8b! \xd9\x83\xd9\x8a\xd9\x81 \xd8\xad\xd8\xa7\xd9\x84\xd9\x83\xd8\x9f", false},    // مرحباً! كيف حالك؟
        {"\xd8\xa7\xd9\x84\xd8\xad\xd9\x85\xd8\xaf \xd9\x84\xd9\x84\xd9\x87\xd8\x8c \xd8\xa8\xd8\xae\xd9\x8a\xd8\xb1", true},              // الحمد لله، بخير
        {"\xd9\x87\xd9\x84 \xd8\xa7\xd9\x86\xd8\xaa\xd9\x87\xd9\x8a\xd8\xaa \xd9\x85\xd9\x86 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb4\xd8\xb1\xd9\x88\xd8\xb9\xd8\x9f", false},   // هل انتهيت من المشروع؟
        {"\xd9\x86\xd8\xb9\xd9\x85\xd8\x8c \xd8\xa7\xd9\x84\xd8\xad\xd9\x85\xd8\xaf \xd9\x84\xd9\x84\xd9\x87 \xd8\xaa\xd9\x85 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad!", true},  // نعم، الحمد لله تم بنجاح!
        {"\xd9\x85\xd9\x85\xd8\xaa\xd8\xa7\xd8\xb2! \xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83 \xd8\xa7\xd9\x84\xd9\x84\xd9\x87 \xd9\x81\xd9\x8a\xd9\x83", false},   // ممتاز! بارك الله فيك
    };

    int scrollArea = widget_scrollview();
    int msgCol = widget_column();

    for (const auto& msg : messages) {
        int bubble = widget_container();
        widget_set_bg(bubble, msg.isMine ? colors::primary.r : colors::surface.r,
                              msg.isMine ? colors::primary.g : colors::surface.g,
                              msg.isMine ? colors::primary.b : colors::surface.b);
        widget_set_border_radius(bubble, 16);
        widget_set_padding(bubble, 12);

        int msgText = make_text(msg.text, msg.isMine ? colors::white : colors::textPrimary, 15);
        widget_add_child(bubble, msgText);
        widget_add_child(msgCol, bubble);

        int sp = widget_spacer(6);
        widget_add_child(msgCol, sp);
    }

    widget_add_child(scrollArea, msgCol);
    widget_add_child(root, scrollArea);

    int sp3 = widget_spacer(12);
    widget_add_child(root, sp3);

    // صف الإدخال
    int inputRow = widget_row();
    int chatInput = widget_textfield("\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8 \xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9...");  // اكتب رسالة...
    widget_set_border(chatInput, 1, 200, 200, 210);
    widget_set_border_radius(chatInput, 20);
    widget_set_padding(chatInput, 10);
    g_widgets[chatInput].widgetName = "chat_input";
    widget_add_child(inputRow, chatInput);

    int sendBtn = make_button("\xd8\xa5\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84", colors::primary);  // إرسال
    g_widgets[sendBtn].widgetName = "send";
    widget_add_child(inputRow, sendBtn);
    widget_add_child(root, inputRow);

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ٦: الطقس
// ═══════════════════════════════════════════════════════════════════════════

static int build_weather(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 24);

    int backBtn = make_button("\xe2\x86\x90 \xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", colors::blueGrey);
    g_widgets[backBtn].widgetName = "back";
    widget_add_child(root, backBtn);

    int sp1 = widget_spacer(16);
    widget_add_child(root, sp1);

    int title = make_section_title("\xd8\xa7\xd9\x84\xd8\xb7\xd9\x82\xd8\xb3");  // الطقس
    widget_add_child(root, title);

    int sp2 = widget_spacer(12);
    widget_add_child(root, sp2);

    // بطاقة الطقس الرئيسية
    int weatherCard = widget_card();
    widget_set_bg(weatherCard, colors::cyan.r, colors::cyan.g, colors::cyan.b);
    widget_set_border_radius(weatherCard, 20);
    widget_set_padding(weatherCard, 24);

    int wcCol = widget_column();
    int city = make_text("\xd8\xa7\xd9\x84\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6", colors::white, 22);  // الرياض
    widget_add_child(wcCol, city);

    int temp = make_text("32" "\xc2\xb0" "C", colors::white, 48);  // 32°C
    widget_add_child(wcCol, temp);

    int condition = make_text("\xd9\x85\xd8\xb4\xd9\x85\xd8\xb3 \xd8\xb5\xd8\xa7\xd9\x81\xd9\x8d", colors::white, 18);  // مشمس صافٍ
    widget_add_child(wcCol, condition);

    widget_add_child(weatherCard, wcCol);
    widget_add_child(root, weatherCard);

    int sp3 = widget_spacer(16);
    widget_add_child(root, sp3);

    // تفاصيل
    int detailsTitle = make_text("\xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb5\xd9\x8a\xd9\x84", colors::textPrimary, 18);  // التفاصيل
    widget_add_child(root, detailsTitle);

    int sp4 = widget_spacer(8);
    widget_add_child(root, sp4);

    struct Detail { std::string label; std::string value; };
    std::vector<Detail> details = {
        {"\xd8\xa7\xd9\x84\xd8\xb1\xd8\xb7\xd9\x88\xd8\xa8\xd8\xa9", "25%"},       // الرطوبة
        {"\xd8\xa7\xd9\x84\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xad", "15 km/h"},      // الرياح
        {"\xd8\xa7\xd9\x84\xd8\xb6\xd8\xba\xd8\xb7", "1013 hPa"},     // الضغط
        {"\xd8\xa7\xd9\x84\xd8\xb1\xd8\xa4\xd9\x8a\xd8\xa9", "10 km"},        // الرؤية
    };

    for (const auto& d : details) {
        int row = widget_row();
        int lbl = make_text(d.label, colors::textSecondary, 15);
        widget_add_child(row, lbl);
        int val = make_text(d.value, colors::textPrimary, 15);
        widget_add_child(row, val);

        int card = widget_card();
        widget_set_bg(card, colors::surface.r, colors::surface.g, colors::surface.b);
        widget_set_border_radius(card, 10);
        widget_set_padding(card, 12);
        widget_add_child(card, row);
        widget_add_child(root, card);

        int sp = widget_spacer(6);
        widget_add_child(root, sp);
    }

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ٧: المتجر
// ═══════════════════════════════════════════════════════════════════════════

static int build_store(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 20);

    int backBtn = make_button("\xe2\x86\x90 \xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", colors::blueGrey);
    g_widgets[backBtn].widgetName = "back";
    widget_add_child(root, backBtn);

    int sp1 = widget_spacer(12);
    widget_add_child(root, sp1);

    int title = make_section_title("\xd8\xa7\xd9\x84\xd9\x85\xd8\xaa\xd8\xac\xd8\xb1");  // المتجر
    widget_add_child(root, title);

    int sp2 = widget_spacer(8);
    widget_add_child(root, sp2);

    // البحث
    int searchField = widget_textfield("\xd8\xa7\xd8\xa8\xd8\xad\xd8\xab...");  // ابحث...
    widget_set_border(searchField, 1, 200, 200, 210);
    widget_set_border_radius(searchField, 20);
    widget_set_padding(searchField, 10);
    g_widgets[searchField].widgetName = "search";
    widget_add_child(root, searchField);

    int sp3 = widget_spacer(12);
    widget_add_child(root, sp3);

    // شبكة المنتجات
    struct Product { std::string name; std::string price; RGBA tagColor; };
    std::vector<Product> products = {
        {"\xd9\x83\xd8\xaa\xd8\xa7\xd8\xa8 C++", "59 \xd8\xb1.\xd8\xb3", colors::primary},               // كتاب C++
        {"\xd9\x83\xd8\xaa\xd8\xa7\xd8\xa8 \xd9\x84\xd8\xba\xd8\xa9 \xd8\xb5", "49 \xd8\xb1.\xd8\xb3", colors::secondary},  // كتاب لغة ص
        {"\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa9 SDL2", "99 \xd8\xb1.\xd8\xb3", colors::purple},                 // دورة SDL2
        {"\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa9 LLVM", "129 \xd8\xb1.\xd8\xb3", colors::orange},               // دورة LLVM
    };

    int grid = widget_gridview(2);
    for (const auto& prod : products) {
        int card = widget_card();
        widget_set_bg(card, colors::surface.r, colors::surface.g, colors::surface.b);
        widget_set_border_radius(card, 14);
        widget_set_padding(card, 14);

        int col = widget_column();

        // Tag ملون
        int tag = widget_container();
        widget_set_bg(tag, prod.tagColor.r, prod.tagColor.g, prod.tagColor.b);
        widget_set_height(tag, 6);
        widget_set_border_radius(tag, 3);
        widget_add_child(col, tag);

        int sp = widget_spacer(8);
        widget_add_child(col, sp);

        int name = make_text(prod.name, colors::textPrimary, 15);
        widget_add_child(col, name);

        int sp2 = widget_spacer(4);
        widget_add_child(col, sp2);

        int price = make_text(prod.price, colors::primary, 16);
        widget_add_child(col, price);

        int sp3 = widget_spacer(8);
        widget_add_child(col, sp3);

        int buyBtn = make_button("\xd8\xb4\xd8\xb1\xd8\xa7\xd8\xa1", colors::primary);  // شراء
        widget_add_child(col, buyBtn);

        widget_add_child(card, col);
        widget_add_child(grid, card);
    }
    widget_add_child(root, grid);

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الشاشة ٨: حول
// ═══════════════════════════════════════════════════════════════════════════

static int build_about(const std::unordered_map<std::string, std::string>& args) {
    int root = widget_column();
    widget_set_bg(root, colors::background.r, colors::background.g, colors::background.b);
    widget_set_padding(root, 24);

    int backBtn = make_button("\xe2\x86\x90 \xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", colors::blueGrey);
    g_widgets[backBtn].widgetName = "back";
    widget_add_child(root, backBtn);

    int sp1 = widget_spacer(20);
    widget_add_child(root, sp1);

    // الشعار
    int logo = widget_container();
    widget_set_bg(logo, colors::primary.r, colors::primary.g, colors::primary.b);
    widget_set_width(logo, 80);
    widget_set_height(logo, 80);
    widget_set_border_radius(logo, 16);
    int logoText = make_text("\xd8\xb5", colors::white, 40);  // ص
    widget_add_child(logo, logoText);
    widget_add_child(root, logo);

    int sp2 = widget_spacer(16);
    widget_add_child(root, sp2);

    int appName = make_text("\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xb5 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xac\xd8\xb1\xd9\x8a\xd8\xa8\xd9\x8a",
                            colors::textPrimary, 24);  // تطبيق ص التجريبي
    widget_add_child(root, appName);

    int version = make_text("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb5\xd8\xaf\xd8\xa7\xd8\xb1 1.0.0", colors::textSecondary, 14);  // الإصدار 1.0.0
    widget_add_child(root, version);

    int sp3 = widget_spacer(20);
    widget_add_child(root, sp3);

    int divider1 = make_divider();
    widget_add_child(root, divider1);

    int sp4 = widget_spacer(16);
    widget_add_child(root, sp4);

    // معلومات
    struct Info { std::string label; std::string value; };
    std::vector<Info> infos = {
        {"\xd8\xa7\xd9\x84\xd9\x85\xd8\xb7\xd9\x88\xd8\xb1", "\xd9\x81\xd8\xb1\xd9\x8a\xd9\x82 \xd9\x84\xd8\xba\xd8\xa9 \xd8\xb5"},   // المطور / فريق لغة ص
        {"\xd8\xa7\xd9\x84\xd9\x84\xd8\xba\xd8\xa9", "C++17 + SDL2"},            // اللغة
        {"\xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xb5\xd8\xa9", "\xd8\xb3\xd8\xb7\xd8\xad \xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaa\xd8\xa8"}, // المنصة / سطح المكتب
        {"\xd8\xa7\xd9\x84\xd8\xb1\xd8\xae\xd8\xb5\xd8\xa9", "MIT"},             // الرخصة
    };

    for (const auto& info : infos) {
        int row = widget_row();
        int lbl = make_text(info.label + ":", colors::textSecondary, 14);
        widget_add_child(row, lbl);
        int val = make_text(info.value, colors::textPrimary, 14);
        widget_add_child(row, val);
        widget_add_child(root, row);

        int sp = widget_spacer(8);
        widget_add_child(root, sp);
    }

    int sp5 = widget_spacer(20);
    widget_add_child(root, sp5);

    int copyright = make_text("\xc2\xa9 2024 \xd9\x84\xd8\xba\xd8\xa9 \xd8\xb5 - \xd8\xac\xd9\x85\xd9\x8a\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xad\xd9\x82\xd9\x88\xd9\x82 \xd9\x85\xd8\xad\xd9\x81\xd9\x88\xd8\xb8\xd8\xa9",
                              colors::textSecondary, 12);  // © 2024 لغة ص - جميع الحقوق محفوظة
    widget_add_child(root, copyright);

    return root;
}

// ═══════════════════════════════════════════════════════════════════════════
// الدالة الرئيسية
// ═══════════════════════════════════════════════════════════════════════════

int main(int argc, char* argv[]) {
    std::cout << "=== SadUI Desktop Demo ===" << std::endl;
    std::cout << "  8 screens, Arabic UI, SDL2 backend" << std::endl;

    // إنشاء التطبيق
    int appId = app_create("\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xb5 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xac\xd8\xb1\xd9\x8a\xd8\xa8\xd9\x8a", 420, 750);  // تطبيق ص التجريبي
    if (appId <= 0) {
        std::cerr << "  Failed to create app!" << std::endl;
        return 1;
    }

    app_set_bg_color(appId, colors::background.r, colors::background.g, colors::background.b);
    std::cout << "  App created (id=" << appId << ")" << std::endl;

    // تسجيل الشاشات
    sad::nav::registerRoute("home", build_home);
    sad::nav::registerRoute("profile", build_profile);
    sad::nav::registerRoute("tasks", build_tasks);
    sad::nav::registerRoute("settings", build_settings);
    sad::nav::registerRoute("chat", build_chat);
    sad::nav::registerRoute("weather", build_weather);
    sad::nav::registerRoute("store", build_store);
    sad::nav::registerRoute("about", build_about);

    std::cout << "  8 routes registered" << std::endl;

    // عرض الشاشة الرئيسية
    sad::nav::push("home");
    int currentWidget = sad::nav::currentPageWidget();
    if (currentWidget > 0) {
        app_set_content(appId, currentWidget);
    }

    std::cout << "  Starting event loop..." << std::endl;

    // حلقة التشغيل
    while (app_is_running(appId)) {
        app_update(appId);

        // التحقق من أزرار التنقل
        for (auto& [id, w] : g_widgets) {
            if (w.widgetName.empty()) continue;

            if (button_was_clicked(id)) {
                if (w.widgetName == "back") {
                    if (sad::nav::canPop()) {
                        sad::nav::pop();
                        int newWidget = sad::nav::currentPageWidget();
                        if (newWidget > 0) {
                            app_set_content(appId, newWidget);
                        }
                    }
                } else {
                    // الانتقال إلى الشاشة
                    sad::nav::push(w.widgetName);
                    int newWidget = sad::nav::currentPageWidget();
                    if (newWidget > 0) {
                        app_set_content(appId, newWidget);
                    }
                }
            }
        }
    }

    // التنظيف
    app_close(appId);
    std::cout << "  App closed." << std::endl;
    std::cout << "=== Done ===" << std::endl;
    return 0;
}
