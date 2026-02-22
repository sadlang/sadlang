/**
 * ==========================================================================
 * ملف: ui_demo_app.hpp
 * الوصف: تطبيق تجريبي متقدم بـ ٨ واجهات مستخدم لاختبار نظام SadUI
 * ==========================================================================
 * بسم الله الرحمن الرحيم
 *
 * يُنشئ هذا الملف تطبيقًا تجريبيًا كاملاً يتضمن ٨ شاشات:
 * 1. الشاشة الرئيسية — لوحة تحكم بقائمة الشاشات
 * 2. شاشة الملف الشخصي — معلومات المستخدم + تعديل
 * 3. شاشة المهام — قائمة مهام تفاعلية
 * 4. شاشة الإعدادات — تبديلات ومنزلقات
 * 5. شاشة المحادثات — واجهة دردشة
 * 6. شاشة الطقس — عرض بيانات جو
 * 7. شاشة المتجر — بطاقات منتجات
 * 8. شاشة حول التطبيق — معلومات ونسخة
 */

#ifndef SAD_UI_DEMO_APP_HPP
#define SAD_UI_DEMO_APP_HPP

#include "ui/sad_ui_ir.hpp"
#include <memory>
#include <string>

namespace sad {
namespace ui {
namespace demo {

using namespace sad::ui::ir;

/**
 * @brief إنشاء تطبيق تجريبي متقدم بـ ٨ واجهات
 */
inline AppIR createDemoApp() {
    AppIR app;
    app.appName = "تطبيق_ص_التجريبي";
    app.version = "1.0.0";
    app.description = "تطبيق تجريبي متقدم بـ ٨ واجهات مستخدم";
    app.initialScreenName = "الرئيسية";
    app.targetPlatform = TargetPlatform::Desktop;

    // إعدادات سطح المكتب
    AppIR::DesktopConfig deskCfg;
    deskCfg.windowWidth = 420;
    deskCfg.windowHeight = 750;
    app.desktopConfig = deskCfg;

    // ═══════════════════════════════════════════════════════════════════
    // الثيمة / Theme
    // ═══════════════════════════════════════════════════════════════════
    app.theme.primaryColor    = {41, 98, 255, 1.0f};     // أزرق ملكي
    app.theme.secondaryColor  = {0, 200, 83, 1.0f};      // أخضر
    app.theme.backgroundColor = {250, 250, 252, 1.0f};   // أبيض فاتح
    app.theme.surfaceColor    = {255, 255, 255, 1.0f};   // أبيض
    app.theme.errorColor      = {213, 0, 0, 1.0f};       // أحمر
    app.theme.textPrimary     = {30, 30, 30, 1.0f};      // أسود تقريبًا
    app.theme.textSecondary   = {120, 120, 130, 1.0f};   // رمادي
    app.theme.fontSizeTitle   = 28.0f;
    app.theme.fontSizeBody    = 16.0f;
    app.theme.fontSizeCaption = 12.0f;
    app.theme.borderRadiusMD  = 12.0f;
    app.theme.spacingMD       = 16.0f;
    app.theme.isRTL           = true;
    app.theme.isDarkMode      = false;

    // ═══════════════════════════════════════════════════════════════════
    // 1. الشاشة الرئيسية — لوحة تحكم
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "الرئيسية";
        screen.routeName = "الرئيسية";

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "main_root";
        root->style.padding = {24, 24, 24, 24};
        root->style.width = Dimension::fill();
        root->style.height = Dimension::fill();
        root->style.backgroundColor = app.theme.backgroundColor;

        // عنوان
        auto title = std::make_shared<UINodeIR>();
        title->type = WidgetType::Text;
        title->tag = "main_title";
        title->properties["content"] = std::string("مرحبًا بك في تطبيق ص");
        title->style.foregroundColor = app.theme.primaryColor;
        title->style.padding = {0, 0, 20, 0};
        root->children.push_back(title);

        // وصف
        auto desc = std::make_shared<UINodeIR>();
        desc->type = WidgetType::Text;
        desc->tag = "main_desc";
        desc->properties["content"] = std::string("تطبيق تجريبي متقدم بـ ٨ واجهات مستخدم يعمل على كل المنصات");
        desc->style.foregroundColor = app.theme.textSecondary;
        desc->style.padding = {0, 0, 30, 0};
        root->children.push_back(desc);

        // أزرار التنقل (٧ أزرار للشاشات الأخرى)
        struct NavItem { std::string label; std::string target; Color color; };
        NavItem items[] = {
            {"👤 الملف الشخصي",    "الملف_الشخصي",   {41, 98, 255, 1.0f}},
            {"📋 قائمة المهام",     "المهام",          {0, 200, 83, 1.0f}},
            {"⚙️ الإعدادات",       "الإعدادات",       {156, 39, 176, 1.0f}},
            {"💬 المحادثات",       "المحادثات",       {255, 152, 0, 1.0f}},
            {"🌤️ الطقس",          "الطقس",           {3, 169, 244, 1.0f}},
            {"🛒 المتجر",          "المتجر",          {233, 30, 99, 1.0f}},
            {"ℹ️ حول التطبيق",    "حول",             {96, 125, 139, 1.0f}},
        };

        for (const auto& item : items) {
            auto btn = std::make_shared<UINodeIR>();
            btn->type = WidgetType::Button;
            btn->properties["content"] = item.label;
            btn->style.backgroundColor = item.color;
            btn->style.foregroundColor = {255, 255, 255, 1.0f};
            btn->style.padding = {14, 20, 14, 20};
            btn->style.borderRadius = 12.0f;
            btn->style.width = Dimension::fill();
            btn->style.margin = {0, 0, 10, 0};

            NavigationActionIR nav;
            nav.type = NavigationActionType::Push;
            nav.targetRoute = item.target;
            btn->navAction = nav;

            root->children.push_back(btn);
        }

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    // ═══════════════════════════════════════════════════════════════════
    // 2. شاشة الملف الشخصي
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "الملف_الشخصي";
        screen.routeName = "الملف_الشخصي";

        // ربط الحالة
        StateBindingIR nameState;
        nameState.name = "الاسم";
        nameState.kind = StateBindingKind::Signal;
        nameState.valueType = StateValueType::Text;
        nameState.initialValue = "أحمد الخالدي";
        screen.stateBindings.push_back(nameState);

        StateBindingIR emailState;
        emailState.name = "البريد";
        emailState.kind = StateBindingKind::Signal;
        emailState.valueType = StateValueType::Text;
        emailState.initialValue = "ahmad@sad-lang.org";
        screen.stateBindings.push_back(emailState);

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "profile_root";
        root->style.padding = {24, 24, 24, 24};
        root->style.width = Dimension::fill();
        root->style.backgroundColor = app.theme.backgroundColor;

        // أيقونة المستخدم (دائرة)
        auto avatar = std::make_shared<UINodeIR>();
        avatar->type = WidgetType::Container;
        avatar->tag = "avatar";
        avatar->style.width = Dimension::points(100);
        avatar->style.height = Dimension::points(100);
        avatar->style.borderRadius = 50.0f;
        avatar->style.backgroundColor = app.theme.primaryColor;
        avatar->style.margin = {0, 0, 20, 0};

        auto avatarText = std::make_shared<UINodeIR>();
        avatarText->type = WidgetType::Text;
        avatarText->properties["content"] = std::string("أ");
        avatarText->style.foregroundColor = {255, 255, 255, 1.0f};
        avatar->children.push_back(avatarText);
        root->children.push_back(avatar);

        // حقل الاسم
        auto nameLabel = std::make_shared<UINodeIR>();
        nameLabel->type = WidgetType::Text;
        nameLabel->properties["content"] = std::string("الاسم الكامل:");
        nameLabel->style.foregroundColor = app.theme.textSecondary;
        nameLabel->style.padding = {0, 0, 6, 0};
        root->children.push_back(nameLabel);

        auto nameField = std::make_shared<UINodeIR>();
        nameField->type = WidgetType::TextField;
        nameField->tag = "name_field";
        nameField->properties["placeholder"] = std::string("أدخل اسمك");
        nameField->style.padding = {12, 16, 12, 16};
        nameField->style.borderRadius = 8.0f;
        nameField->style.borderWidth = 1.0f;
        nameField->style.borderColor = {200, 200, 210, 1.0f};
        nameField->style.width = Dimension::fill();
        nameField->style.margin = {0, 0, 16, 0};

        StateBindingIR nameBind;
        nameBind.name = "الاسم";
        nameBind.kind = StateBindingKind::Binding;
        nameBind.valueType = StateValueType::Text;
        nameField->stateBinding = nameBind;
        root->children.push_back(nameField);

        // حقل البريد
        auto emailLabel = std::make_shared<UINodeIR>();
        emailLabel->type = WidgetType::Text;
        emailLabel->properties["content"] = std::string("البريد الإلكتروني:");
        emailLabel->style.foregroundColor = app.theme.textSecondary;
        emailLabel->style.padding = {0, 0, 6, 0};
        root->children.push_back(emailLabel);

        auto emailField = std::make_shared<UINodeIR>();
        emailField->type = WidgetType::TextField;
        emailField->tag = "email_field";
        emailField->properties["placeholder"] = std::string("بريدك@example.com");
        emailField->style.padding = {12, 16, 12, 16};
        emailField->style.borderRadius = 8.0f;
        emailField->style.borderWidth = 1.0f;
        emailField->style.borderColor = {200, 200, 210, 1.0f};
        emailField->style.width = Dimension::fill();
        emailField->style.margin = {0, 0, 24, 0};
        root->children.push_back(emailField);

        // زر حفظ
        auto saveBtn = std::make_shared<UINodeIR>();
        saveBtn->type = WidgetType::Button;
        saveBtn->tag = "save_btn";
        saveBtn->properties["content"] = std::string("حفظ التغييرات");
        saveBtn->style.backgroundColor = app.theme.primaryColor;
        saveBtn->style.foregroundColor = {255, 255, 255, 1.0f};
        saveBtn->style.padding = {14, 24, 14, 24};
        saveBtn->style.borderRadius = 12.0f;
        saveBtn->style.width = Dimension::fill();

        EventHandlerIR saveEvent;
        saveEvent.type = EventType::Tap;
        saveEvent.handlerName = "حفظ_الملف";
        saveBtn->eventHandlers.push_back(saveEvent);
        root->children.push_back(saveBtn);

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    // ═══════════════════════════════════════════════════════════════════
    // 3. شاشة المهام — قائمة مهام تفاعلية
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "المهام";
        screen.routeName = "المهام";

        StateBindingIR taskList;
        taskList.name = "المهام";
        taskList.kind = StateBindingKind::Signal;
        taskList.valueType = StateValueType::TextList;
        taskList.initialValue = "مراجعة الكود,كتابة الاختبارات,تحديث الوثائق";
        screen.stateBindings.push_back(taskList);

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "tasks_root";
        root->style.padding = {16, 16, 16, 16};
        root->style.width = Dimension::fill();
        root->style.height = Dimension::fill();
        root->style.backgroundColor = app.theme.backgroundColor;

        // عنوان
        auto title = std::make_shared<UINodeIR>();
        title->type = WidgetType::Text;
        title->properties["content"] = std::string("📋 مهامك اليوم");
        title->style.foregroundColor = app.theme.textPrimary;
        title->style.padding = {0, 0, 16, 0};
        root->children.push_back(title);

        // حقل إضافة مهمة
        auto addRow = std::make_shared<UINodeIR>();
        addRow->type = WidgetType::Row;
        addRow->tag = "add_row";
        addRow->style.width = Dimension::fill();
        addRow->style.margin = {0, 0, 16, 0};

        auto taskInput = std::make_shared<UINodeIR>();
        taskInput->type = WidgetType::TextField;
        taskInput->tag = "task_input";
        taskInput->properties["placeholder"] = std::string("مهمة جديدة...");
        taskInput->style.padding = {10, 14, 10, 14};
        taskInput->style.borderRadius = 8.0f;
        taskInput->style.borderWidth = 1.0f;
        taskInput->style.borderColor = {200, 200, 210, 1.0f};
        taskInput->style.width = Dimension::fill();
        addRow->children.push_back(taskInput);

        auto addBtn = std::make_shared<UINodeIR>();
        addBtn->type = WidgetType::Button;
        addBtn->tag = "add_btn";
        addBtn->properties["content"] = std::string("أضف");
        addBtn->style.backgroundColor = app.theme.secondaryColor;
        addBtn->style.foregroundColor = {255, 255, 255, 1.0f};
        addBtn->style.padding = {10, 18, 10, 18};
        addBtn->style.borderRadius = 8.0f;
        addBtn->style.margin = {0, 0, 0, 10};
        addRow->children.push_back(addBtn);
        root->children.push_back(addRow);

        // عناصر المهام (نموذجية)
        std::string tasks[] = {"مراجعة الكود ✓", "كتابة الاختبارات", "تحديث الوثائق", "إصلاح الأخطاء", "تصميم الواجهة"};
        for (const auto& t : tasks) {
            auto card = std::make_shared<UINodeIR>();
            card->type = WidgetType::Card;
            card->style.padding = {14, 16, 14, 16};
            card->style.borderRadius = 10.0f;
            card->style.backgroundColor = app.theme.surfaceColor;
            card->style.width = Dimension::fill();
            card->style.margin = {0, 0, 8, 0};
            card->style.shadow = Shadow{Color(0, 0, 0, 0.1f), 0, 2, 4};

            auto taskText = std::make_shared<UINodeIR>();
            taskText->type = WidgetType::Text;
            taskText->properties["content"] = t;
            taskText->style.foregroundColor = app.theme.textPrimary;
            card->children.push_back(taskText);
            root->children.push_back(card);
        }

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    // ═══════════════════════════════════════════════════════════════════
    // 4. شاشة الإعدادات — تبديلات ومنزلقات
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "الإعدادات";
        screen.routeName = "الإعدادات";

        StateBindingIR darkMode;
        darkMode.name = "الوضع_الداكن";
        darkMode.kind = StateBindingKind::Signal;
        darkMode.valueType = StateValueType::Boolean;
        darkMode.initialValue = "false";
        screen.stateBindings.push_back(darkMode);

        StateBindingIR notifications;
        notifications.name = "الإشعارات";
        notifications.kind = StateBindingKind::Signal;
        notifications.valueType = StateValueType::Boolean;
        notifications.initialValue = "true";
        screen.stateBindings.push_back(notifications);

        StateBindingIR fontSize;
        fontSize.name = "حجم_الخط";
        fontSize.kind = StateBindingKind::Signal;
        fontSize.valueType = StateValueType::Number;
        fontSize.initialValue = "16";
        screen.stateBindings.push_back(fontSize);

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "settings_root";
        root->style.padding = {24, 24, 24, 24};
        root->style.width = Dimension::fill();
        root->style.backgroundColor = app.theme.backgroundColor;

        auto title = std::make_shared<UINodeIR>();
        title->type = WidgetType::Text;
        title->properties["content"] = std::string("⚙️ الإعدادات");
        title->style.foregroundColor = app.theme.textPrimary;
        title->style.padding = {0, 0, 24, 0};
        root->children.push_back(title);

        // إعداد: الوضع الداكن
        auto darkRow = std::make_shared<UINodeIR>();
        darkRow->type = WidgetType::Row;
        darkRow->style.width = Dimension::fill();
        darkRow->style.padding = {14, 0, 14, 0};
        darkRow->style.margin = {0, 0, 10, 0};
        {
            auto label = std::make_shared<UINodeIR>();
            label->type = WidgetType::Text;
            label->properties["content"] = std::string("🌙 الوضع الداكن");
            label->style.foregroundColor = app.theme.textPrimary;
            darkRow->children.push_back(label);

            auto toggle = std::make_shared<UINodeIR>();
            toggle->type = WidgetType::Toggle;
            toggle->tag = "dark_toggle";
            StateBindingIR bind;
            bind.name = "الوضع_الداكن";
            bind.kind = StateBindingKind::Binding;
            bind.valueType = StateValueType::Boolean;
            toggle->stateBinding = bind;
            darkRow->children.push_back(toggle);
        }
        root->children.push_back(darkRow);

        // إعداد: الإشعارات
        auto notifRow = std::make_shared<UINodeIR>();
        notifRow->type = WidgetType::Row;
        notifRow->style.width = Dimension::fill();
        notifRow->style.padding = {14, 0, 14, 0};
        notifRow->style.margin = {0, 0, 10, 0};
        {
            auto label = std::make_shared<UINodeIR>();
            label->type = WidgetType::Text;
            label->properties["content"] = std::string("🔔 الإشعارات");
            label->style.foregroundColor = app.theme.textPrimary;
            notifRow->children.push_back(label);

            auto toggle = std::make_shared<UINodeIR>();
            toggle->type = WidgetType::Toggle;
            toggle->tag = "notif_toggle";
            StateBindingIR bind;
            bind.name = "الإشعارات";
            bind.kind = StateBindingKind::Binding;
            bind.valueType = StateValueType::Boolean;
            toggle->stateBinding = bind;
            notifRow->children.push_back(toggle);
        }
        root->children.push_back(notifRow);

        // شريط الصوت
        auto divider = std::make_shared<UINodeIR>();
        divider->type = WidgetType::Divider;
        divider->style.margin = {16, 0, 16, 0};
        root->children.push_back(divider);

        auto fontLabel = std::make_shared<UINodeIR>();
        fontLabel->type = WidgetType::Text;
        fontLabel->properties["content"] = std::string("📏 حجم الخط");
        fontLabel->style.foregroundColor = app.theme.textPrimary;
        fontLabel->style.padding = {0, 0, 10, 0};
        root->children.push_back(fontLabel);

        auto slider = std::make_shared<UINodeIR>();
        slider->type = WidgetType::Slider;
        slider->tag = "font_slider";
        slider->style.width = Dimension::fill();
        StateBindingIR fontBind;
        fontBind.name = "حجم_الخط";
        fontBind.kind = StateBindingKind::Binding;
        fontBind.valueType = StateValueType::Number;
        slider->stateBinding = fontBind;
        root->children.push_back(slider);

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    // ═══════════════════════════════════════════════════════════════════
    // 5. شاشة المحادثات — واجهة دردشة
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "المحادثات";
        screen.routeName = "المحادثات";

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "chat_root";
        root->style.padding = {16, 16, 16, 16};
        root->style.width = Dimension::fill();
        root->style.height = Dimension::fill();
        root->style.backgroundColor = app.theme.backgroundColor;

        // عنوان
        auto header = std::make_shared<UINodeIR>();
        header->type = WidgetType::Text;
        header->properties["content"] = std::string("💬 المحادثات");
        header->style.foregroundColor = app.theme.textPrimary;
        header->style.padding = {0, 0, 16, 0};
        root->children.push_back(header);

        // رسائل نموذجية
        struct Msg { std::string sender; std::string text; bool isMe; };
        Msg msgs[] = {
            {"خالد", "مرحبًا، كيف حالك؟", false},
            {"أنت", "الحمد لله بخير، ماذا عندك؟", true},
            {"خالد", "هل أنهيت مشروع لغة ص؟", false},
            {"أنت", "نعم! نظام الواجهات جاهز 🎉", true},
            {"خالد", "ممتاز! أريد تجربته", false},
        };

        for (const auto& m : msgs) {
            auto msgCard = std::make_shared<UINodeIR>();
            msgCard->type = WidgetType::Card;
            msgCard->style.padding = {10, 14, 10, 14};
            msgCard->style.borderRadius = 16.0f;
            msgCard->style.margin = {0, 0, 8, 0};
            msgCard->style.width = Dimension::points(280);

            if (m.isMe) {
                msgCard->style.backgroundColor = app.theme.primaryColor;
            } else {
                msgCard->style.backgroundColor = {235, 235, 240, 1.0f};
            }

            auto senderText = std::make_shared<UINodeIR>();
            senderText->type = WidgetType::Text;
            senderText->properties["content"] = m.sender;
            senderText->style.foregroundColor = m.isMe
                ? Color{200, 210, 255, 1.0f}
                : app.theme.textSecondary;
            msgCard->children.push_back(senderText);

            auto msgText = std::make_shared<UINodeIR>();
            msgText->type = WidgetType::Text;
            msgText->properties["content"] = m.text;
            msgText->style.foregroundColor = m.isMe
                ? Color{255, 255, 255, 1.0f}
                : app.theme.textPrimary;
            msgCard->children.push_back(msgText);

            root->children.push_back(msgCard);
        }

        // منطقة الإدخال
        auto inputRow = std::make_shared<UINodeIR>();
        inputRow->type = WidgetType::Row;
        inputRow->tag = "chat_input_row";
        inputRow->style.width = Dimension::fill();
        inputRow->style.margin = {16, 0, 0, 0};

        auto chatInput = std::make_shared<UINodeIR>();
        chatInput->type = WidgetType::TextField;
        chatInput->tag = "chat_input";
        chatInput->properties["placeholder"] = std::string("اكتب رسالتك...");
        chatInput->style.padding = {10, 14, 10, 14};
        chatInput->style.borderRadius = 20.0f;
        chatInput->style.borderWidth = 1.0f;
        chatInput->style.borderColor = {200, 200, 210, 1.0f};
        chatInput->style.width = Dimension::fill();
        inputRow->children.push_back(chatInput);

        auto sendBtn = std::make_shared<UINodeIR>();
        sendBtn->type = WidgetType::Button;
        sendBtn->tag = "send_btn";
        sendBtn->properties["content"] = std::string("إرسال");
        sendBtn->style.backgroundColor = app.theme.primaryColor;
        sendBtn->style.foregroundColor = {255, 255, 255, 1.0f};
        sendBtn->style.padding = {10, 18, 10, 18};
        sendBtn->style.borderRadius = 20.0f;
        sendBtn->style.margin = {0, 0, 0, 10};

        EventHandlerIR sendEvt;
        sendEvt.type = EventType::Tap;
        sendEvt.handlerName = "إرسال_رسالة";
        sendBtn->eventHandlers.push_back(sendEvt);
        inputRow->children.push_back(sendBtn);

        root->children.push_back(inputRow);

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    // ═══════════════════════════════════════════════════════════════════
    // 6. شاشة الطقس — عرض بيانات جو
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "الطقس";
        screen.routeName = "الطقس";

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "weather_root";
        root->style.padding = {24, 24, 24, 24};
        root->style.width = Dimension::fill();
        root->style.height = Dimension::fill();
        root->style.backgroundColor = {25, 118, 210, 1.0f}; // أزرق سماوي

        // درجة الحرارة الكبيرة
        auto temp = std::make_shared<UINodeIR>();
        temp->type = WidgetType::Text;
        temp->tag = "temperature";
        temp->properties["content"] = std::string("٢٨°م");
        temp->style.foregroundColor = {255, 255, 255, 1.0f};
        temp->style.padding = {0, 0, 8, 0};
        root->children.push_back(temp);

        auto city = std::make_shared<UINodeIR>();
        city->type = WidgetType::Text;
        city->properties["content"] = std::string("الرياض، المملكة العربية السعودية");
        city->style.foregroundColor = {200, 220, 255, 1.0f};
        city->style.padding = {0, 0, 4, 0};
        root->children.push_back(city);

        auto condition = std::make_shared<UINodeIR>();
        condition->type = WidgetType::Text;
        condition->properties["content"] = std::string("☀️ مشمس — صافٍ");
        condition->style.foregroundColor = {255, 255, 255, 1.0f};
        condition->style.padding = {0, 0, 30, 0};
        root->children.push_back(condition);

        // تفاصيل الطقس (صف بطاقات)
        auto detailsRow = std::make_shared<UINodeIR>();
        detailsRow->type = WidgetType::Row;
        detailsRow->style.width = Dimension::fill();

        struct WeatherDetail { std::string icon; std::string label; std::string value; };
        WeatherDetail details[] = {
            {"💧", "الرطوبة",   "25%"},
            {"💨", "الرياح",    "15 كم/س"},
            {"🌡️", "الضغط",    "1013 hPa"},
        };

        for (const auto& d : details) {
            auto card = std::make_shared<UINodeIR>();
            card->type = WidgetType::Card;
            card->style.padding = {12, 14, 12, 14};
            card->style.borderRadius = 12.0f;
            card->style.backgroundColor = {255, 255, 255, 0.2f};
            card->style.margin = {0, 8, 0, 0};

            auto icon = std::make_shared<UINodeIR>();
            icon->type = WidgetType::Text;
            icon->properties["content"] = d.icon;
            card->children.push_back(icon);

            auto label = std::make_shared<UINodeIR>();
            label->type = WidgetType::Text;
            label->properties["content"] = d.label;
            label->style.foregroundColor = {200, 220, 255, 1.0f};
            card->children.push_back(label);

            auto val = std::make_shared<UINodeIR>();
            val->type = WidgetType::Text;
            val->properties["content"] = d.value;
            val->style.foregroundColor = {255, 255, 255, 1.0f};
            card->children.push_back(val);

            detailsRow->children.push_back(card);
        }
        root->children.push_back(detailsRow);

        // توقعات الأيام القادمة
        auto forecastTitle = std::make_shared<UINodeIR>();
        forecastTitle->type = WidgetType::Text;
        forecastTitle->properties["content"] = std::string("التوقعات القادمة");
        forecastTitle->style.foregroundColor = {255, 255, 255, 1.0f};
        forecastTitle->style.padding = {24, 0, 12, 0};
        root->children.push_back(forecastTitle);

        struct Forecast { std::string day; std::string icon; std::string high; std::string low; };
        Forecast forecasts[] = {
            {"الأحد",    "☀️", "30°", "22°"},
            {"الاثنين",  "⛅", "28°", "20°"},
            {"الثلاثاء", "🌧️", "24°", "18°"},
        };

        for (const auto& f : forecasts) {
            auto fRow = std::make_shared<UINodeIR>();
            fRow->type = WidgetType::Row;
            fRow->style.width = Dimension::fill();
            fRow->style.padding = {10, 0, 10, 0};

            auto dayText = std::make_shared<UINodeIR>();
            dayText->type = WidgetType::Text;
            dayText->properties["content"] = f.day;
            dayText->style.foregroundColor = {255, 255, 255, 1.0f};
            fRow->children.push_back(dayText);

            auto iconText = std::make_shared<UINodeIR>();
            iconText->type = WidgetType::Text;
            iconText->properties["content"] = f.icon;
            fRow->children.push_back(iconText);

            auto temps = std::make_shared<UINodeIR>();
            temps->type = WidgetType::Text;
            temps->properties["content"] = f.high + " / " + f.low;
            temps->style.foregroundColor = {200, 220, 255, 1.0f};
            fRow->children.push_back(temps);

            root->children.push_back(fRow);
        }

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    // ═══════════════════════════════════════════════════════════════════
    // 7. شاشة المتجر — بطاقات منتجات
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "المتجر";
        screen.routeName = "المتجر";

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "store_root";
        root->style.padding = {16, 16, 16, 16};
        root->style.width = Dimension::fill();
        root->style.height = Dimension::fill();
        root->style.backgroundColor = app.theme.backgroundColor;

        auto title = std::make_shared<UINodeIR>();
        title->type = WidgetType::Text;
        title->properties["content"] = std::string("🛒 المتجر");
        title->style.foregroundColor = app.theme.textPrimary;
        title->style.padding = {0, 0, 20, 0};
        root->children.push_back(title);

        // بحث
        auto searchField = std::make_shared<UINodeIR>();
        searchField->type = WidgetType::TextField;
        searchField->tag = "search_field";
        searchField->properties["placeholder"] = std::string("🔍 ابحث عن منتج...");
        searchField->style.padding = {12, 16, 12, 16};
        searchField->style.borderRadius = 24.0f;
        searchField->style.backgroundColor = {240, 240, 245, 1.0f};
        searchField->style.width = Dimension::fill();
        searchField->style.margin = {0, 0, 20, 0};
        root->children.push_back(searchField);

        // شبكة المنتجات
        auto grid = std::make_shared<UINodeIR>();
        grid->type = WidgetType::Grid;
        grid->tag = "products_grid";
        grid->style.width = Dimension::fill();

        struct Product { std::string name; std::string price; std::string emoji; Color color; };
        Product products[] = {
            {"كتاب برمجة ص",   "٤٩ ر.س",   "📖", {66, 133, 244, 1.0f}},
            {"حاسوب محمول",    "٣٩٩٩ ر.س", "💻", {52, 168, 83, 1.0f}},
            {"سماعات لاسلكية", "١٩٩ ر.س",  "🎧", {234, 67, 53, 1.0f}},
            {"لوحة مفاتيح",   "٢٤٩ ر.س",  "⌨️", {251, 188, 5, 1.0f}},
            {"ساعة ذكية",     "١٢٩٩ ر.س", "⌚", {156, 39, 176, 1.0f}},
            {"شاشة عرض",     "٢٤٩٩ ر.س", "🖥️", {0, 150, 136, 1.0f}},
        };

        for (const auto& p : products) {
            auto card = std::make_shared<UINodeIR>();
            card->type = WidgetType::Card;
            card->style.padding = {16, 16, 16, 16};
            card->style.borderRadius = 14.0f;
            card->style.backgroundColor = app.theme.surfaceColor;
            card->style.margin = {0, 8, 10, 0};

            auto emoji = std::make_shared<UINodeIR>();
            emoji->type = WidgetType::Text;
            emoji->properties["content"] = p.emoji;
            emoji->style.padding = {0, 0, 8, 0};
            card->children.push_back(emoji);

            auto name = std::make_shared<UINodeIR>();
            name->type = WidgetType::Text;
            name->properties["content"] = p.name;
            name->style.foregroundColor = app.theme.textPrimary;
            card->children.push_back(name);

            auto price = std::make_shared<UINodeIR>();
            price->type = WidgetType::Text;
            price->properties["content"] = p.price;
            price->style.foregroundColor = p.color;
            price->style.padding = {6, 0, 10, 0};
            card->children.push_back(price);

            auto buyBtn = std::make_shared<UINodeIR>();
            buyBtn->type = WidgetType::Button;
            buyBtn->properties["content"] = std::string("أضف للسلة");
            buyBtn->style.backgroundColor = p.color;
            buyBtn->style.foregroundColor = {255, 255, 255, 1.0f};
            buyBtn->style.padding = {8, 14, 8, 14};
            buyBtn->style.borderRadius = 8.0f;
            buyBtn->style.width = Dimension::fill();

            EventHandlerIR buyEvt;
            buyEvt.type = EventType::Tap;
            buyEvt.handlerName = "أضف_للسلة";
            buyBtn->eventHandlers.push_back(buyEvt);
            card->children.push_back(buyBtn);

            grid->children.push_back(card);
        }
        root->children.push_back(grid);

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    // ═══════════════════════════════════════════════════════════════════
    // 8. شاشة حول التطبيق
    // ═══════════════════════════════════════════════════════════════════
    {
        ScreenIR screen;
        screen.name = "حول";
        screen.routeName = "حول";

        auto root = std::make_shared<UINodeIR>();
        root->type = WidgetType::Column;
        root->tag = "about_root";
        root->style.padding = {32, 24, 32, 24};
        root->style.width = Dimension::fill();
        root->style.height = Dimension::fill();
        root->style.backgroundColor = app.theme.backgroundColor;

        // شعار
        auto logo = std::make_shared<UINodeIR>();
        logo->type = WidgetType::Container;
        logo->tag = "logo";
        logo->style.width = Dimension::points(80);
        logo->style.height = Dimension::points(80);
        logo->style.borderRadius = 20.0f;
        logo->style.backgroundColor = app.theme.primaryColor;
        logo->style.margin = {0, 0, 20, 0};

        auto logoText = std::make_shared<UINodeIR>();
        logoText->type = WidgetType::Text;
        logoText->properties["content"] = std::string("ص");
        logoText->style.foregroundColor = {255, 255, 255, 1.0f};
        logo->children.push_back(logoText);
        root->children.push_back(logo);

        // اسم التطبيق
        auto appNameText = std::make_shared<UINodeIR>();
        appNameText->type = WidgetType::Text;
        appNameText->properties["content"] = std::string("تطبيق ص التجريبي");
        appNameText->style.foregroundColor = app.theme.textPrimary;
        appNameText->style.padding = {0, 0, 6, 0};
        root->children.push_back(appNameText);

        auto version = std::make_shared<UINodeIR>();
        version->type = WidgetType::Text;
        version->properties["content"] = std::string("الإصدار 1.0.0");
        version->style.foregroundColor = app.theme.textSecondary;
        version->style.padding = {0, 0, 24, 0};
        root->children.push_back(version);

        // معلومات
        struct InfoLine { std::string emoji; std::string text; };
        InfoLine infos[] = {
            {"🌍", "لغة ص — أول لغة برمجة عربية حديثة"},
            {"🖥️", "واجهات رسومية تعمل على كل المنصات"},
            {"📱", "سطح المكتب، أندرويد، iOS، والويب"},
            {"⚡", "أداء أصلي بفضل LLVM و SDL2"},
            {"🔒", "نظام ملكية واقتراض متقدم"},
            {"📦", "مدير حزم مدمج"},
        };

        for (const auto& info : infos) {
            auto row = std::make_shared<UINodeIR>();
            row->type = WidgetType::Row;
            row->style.width = Dimension::fill();
            row->style.padding = {10, 0, 10, 0};

            auto emoji = std::make_shared<UINodeIR>();
            emoji->type = WidgetType::Text;
            emoji->properties["content"] = info.emoji;
            emoji->style.margin = {0, 12, 0, 0};
            row->children.push_back(emoji);

            auto text = std::make_shared<UINodeIR>();
            text->type = WidgetType::Text;
            text->properties["content"] = info.text;
            text->style.foregroundColor = app.theme.textPrimary;
            row->children.push_back(text);

            root->children.push_back(row);
        }

        // حقوق
        auto copyright = std::make_shared<UINodeIR>();
        copyright->type = WidgetType::Text;
        copyright->properties["content"] = std::string("حقوق النشر © 2024-2026 فريق لغة ص");
        copyright->style.foregroundColor = app.theme.textSecondary;
        copyright->style.padding = {30, 0, 0, 0};
        root->children.push_back(copyright);

        screen.rootNode = root;
        app.screens.push_back(std::move(screen));
    }

    return app;
}

} // namespace demo
} // namespace ui
} // namespace sad

#endif // SAD_UI_DEMO_APP_HPP
