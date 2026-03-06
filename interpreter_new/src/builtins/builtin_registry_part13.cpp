/**
 * @file builtin_registry_part13.cpp
 * @brief (AR) تسجيل دوال دورة التطبيق، التنقل، إدارة الحالة، الأحداث، الثيمات
 * @brief (EN) Register app lifecycle, navigator, state management, events, themes
 */

#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

// ───── sad::ui / sad::nav / sad::state framework headers ─────
#include "graphics/sad_ui_framework.h"
#include "graphics/sad_navigator.h"
#include "graphics/sad_state.h"

// ───── Unified UI layer (cross-platform bridge) ─────
#include "ui/sad_ui_unified.h"
#include "ui/sad_ui_platform.h"
#include "ui/sad_ui_callbacks.h"

// Helper: route through unified backend if available, else fall back to sad::ui
namespace {
    inline bool useUnified() {
        return sad::unified::SadUI::instance().isInitialized();
    }
    inline sad::unified::SadUIBackend* UB() {
        return sad::unified::SadUI::instance().backend();
    }
}

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart13(Interpreter& interpreter) {

    // ═══════════════════════════════════════════════════════════════════
    // (AR) دورة حياة التطبيق / (EN) App Lifecycle
    // ═══════════════════════════════════════════════════════════════════

    // --- أنشئ_تطبيق / app_create ---
    auto app_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string title = args.size() > 0 ? args[0]->toString() : "تطبيق ص";
        int w = args.size() > 1 ? args[1]->toInt() : 800;
        int h = args.size() > 2 ? args[2]->toInt() : 600;
        int appId;
        if (useUnified() && UB()) {
            appId = UB()->appCreate(title, w, h);
        } else {
            appId = sad::ui::app_create(title, w, h);
        }
        return std::make_shared<Data::Value>(appId);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_create_func); // أنشئ_تطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_create", app_create_func);

    // --- التطبيق_يعمل / app_is_running ---
    auto app_is_running_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        if (useUnified() && UB()) {
            return std::make_shared<Data::Value>(UB()->appIsRunning(args[0]->toInt()));
        }
        return std::make_shared<Data::Value>(sad::ui::app_is_running(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82_\xd9\x8a\xd8\xb9\xd9\x85\xd9\x84", app_is_running_func); // التطبيق_يعمل
    interpreter.getFunctionManager().registerBuiltinFunction("app_is_running", app_is_running_func);

    // --- حدث_التطبيق / app_update ---
    auto app_update_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        if (useUnified() && UB()) {
            UB()->appUpdate(args[0]->toInt());
        } else {
            sad::ui::app_update(args[0]->toInt());
        }
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd8\xaf\xd8\xab_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_update_func); // حدث_التطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_update", app_update_func);

    // --- عين_محتوى_التطبيق / app_set_content ---
    auto app_set_content_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        if (useUnified() && UB()) {
            UB()->appSetContent(args[0]->toInt(), args[1]->toInt());
        } else {
            sad::ui::app_set_content(args[0]->toInt(), args[1]->toInt());
        }
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_set_content_func); // عين_محتوى_التطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_set_content", app_set_content_func);

    // --- عين_خلفية_التطبيق / app_set_bg_color ---
    auto app_set_bg_color_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) return std::make_shared<Data::Value>();
        int appId = args[0]->toInt();
        int r = args[1]->toInt(), g = args[2]->toInt(), b = args[3]->toInt();
        if (useUnified() && UB()) {
            UB()->appSetBgColor(appId, r, g, b);
        } else {
            sad::ui::app_set_bg_color(appId, r, g, b);
        }
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_set_bg_color_func); // عين_خلفية_التطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_set_bg_color", app_set_bg_color_func);

    // --- أغلق_التطبيق / app_close ---
    auto app_close_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        if (useUnified() && UB()) {
            UB()->appClose(args[0]->toInt());
        } else {
            sad::ui::app_close(args[0]->toInt());
        }
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xba\xd9\x84\xd9\x82_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82", app_close_func); // أغلق_التطبيق
    interpreter.getFunctionManager().registerBuiltinFunction("app_close", app_close_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) نظام التنقل / (EN) Navigator
    // ═══════════════════════════════════════════════════════════════════

    // --- انتقل / nav_push ---
    auto nav_push_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        std::string route = args[0]->toString();
        if (args.size() > 1) {
            // With args map — build from pairs: route, key1, val1, key2, val2...
            std::unordered_map<std::string, std::string> navArgs;
            for (size_t i = 1; i + 1 < args.size(); i += 2) {
                navArgs[args[i]->toString()] = args[i + 1]->toString();
            }
            return std::make_shared<Data::Value>(sad::nav::push(route, navArgs));
        }
        return std::make_shared<Data::Value>(sad::nav::push(route));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84", nav_push_func); // انتقل
    interpreter.getFunctionManager().registerBuiltinFunction("nav_push", nav_push_func);

    // --- ارجع / nav_pop ---
    auto nav_pop_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::pop());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9", nav_pop_func); // ارجع (note: this is for navigation, different from ارجع keyword)
    interpreter.getFunctionManager().registerBuiltinFunction("nav_pop", nav_pop_func);

    // --- يمكن_الرجوع / nav_can_pop ---
    auto nav_can_pop_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::canPop());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x8a\xd9\x85\xd9\x83\xd9\x86_\xd8\xa7\xd9\x84\xd8\xb1\xd8\xac\xd9\x88\xd8\xb9", nav_can_pop_func); // يمكن_الرجوع
    interpreter.getFunctionManager().registerBuiltinFunction("nav_can_pop", nav_can_pop_func);

    // --- المسار_الحالي / nav_current_route ---
    auto nav_current_route_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::currentRoute());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1_\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd9\x8a", nav_current_route_func); // المسار_الحالي
    interpreter.getFunctionManager().registerBuiltinFunction("nav_current_route", nav_current_route_func);

    // --- ودجت_الصفحة / nav_current_page_widget ---
    auto nav_current_page_widget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::currentPageWidget());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x88\xd8\xaf\xd8\xac\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xb5\xd9\x81\xd8\xad\xd8\xa9", nav_current_page_widget_func); // ودجت_الصفحة
    interpreter.getFunctionManager().registerBuiltinFunction("nav_current_page_widget", nav_current_page_widget_func);

    // --- انتقل_وامسح / nav_push_and_clear ---
    auto nav_push_and_clear_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::nav::pushAndClearAll(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84_\xd9\x88\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad", nav_push_and_clear_func); // انتقل_وامسح
    interpreter.getFunctionManager().registerBuiltinFunction("nav_push_and_clear", nav_push_and_clear_func);

    // --- ارجع_حتى / nav_pop_until ---
    auto nav_pop_until_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::nav::popUntil(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9_\xd8\xad\xd8\xaa\xd9\x89", nav_pop_until_func); // ارجع_حتى
    interpreter.getFunctionManager().registerBuiltinFunction("nav_pop_until", nav_pop_until_func);

    // --- استبدل_المسار / nav_replace ---
    auto nav_replace_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::nav::replace(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x84_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1", nav_replace_func); // استبدل_المسار
    interpreter.getFunctionManager().registerBuiltinFunction("nav_replace", nav_replace_func);

    // --- حجم_المكدس / nav_stack_size ---
    auto nav_stack_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::stackSize());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xad\xd8\xac\xd9\x85_\xd8\xa7\xd9\x84\xd9\x85\xd9\x83\xd8\xaf\xd8\xb3", nav_stack_size_func); // حجم_المكدس
    interpreter.getFunctionManager().registerBuiltinFunction("nav_stack_size", nav_stack_size_func);

    // --- اقرأ_بيانات_صفحة / nav_get_arg ---
    auto nav_get_arg_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        std::string key = args[0]->toString();
        std::string def = args.size() > 1 ? args[1]->toString() : "";
        return std::make_shared<Data::Value>(sad::nav::getArg(key, def));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xb5\xd9\x81\xd8\xad\xd8\xa9", nav_get_arg_func); // اقرأ_بيانات_صفحة
    interpreter.getFunctionManager().registerBuiltinFunction("nav_get_arg", nav_get_arg_func);

    // --- هل_المسار_موجود / nav_has_route ---
    auto nav_has_route_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::nav::hasRoute(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb1_\xd9\x85\xd9\x88\xd8\xac\xd9\x88\xd8\xaf", nav_has_route_func); // هل_المسار_موجود
    interpreter.getFunctionManager().registerBuiltinFunction("nav_has_route", nav_has_route_func);

    // --- امسح_التنقل / nav_clear_all ---
    auto nav_clear_all_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        sad::nav::clearAll();
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd9\x84\xd8\xaa\xd9\x86\xd9\x82\xd9\x84", nav_clear_all_func); // امسح_التنقل
    interpreter.getFunctionManager().registerBuiltinFunction("nav_clear_all", nav_clear_all_func);

    // --- أعد_التنقل / nav_reset ---
    auto nav_reset_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        sad::nav::reset();
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb9\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd9\x86\xd9\x82\xd9\x84", nav_reset_func); // أعد_التنقل
    interpreter.getFunctionManager().registerBuiltinFunction("nav_reset", nav_reset_func);

    // --- هل_ينتقل / nav_is_transitioning ---
    auto nav_is_transitioning_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::isTransitioning());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd9\x8a\xd9\x86\xd8\xaa\xd9\x82\xd9\x84", nav_is_transitioning_func); // هل_ينتقل
    interpreter.getFunctionManager().registerBuiltinFunction("nav_is_transitioning", nav_is_transitioning_func);

    // --- التبويب_الحالي / nav_current_tab ---
    auto nav_current_tab_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::currentTab());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8_\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd9\x8a", nav_current_tab_func); // التبويب_الحالي
    interpreter.getFunctionManager().registerBuiltinFunction("nav_current_tab", nav_current_tab_func);

    // --- بدل_تبويب / nav_switch_tab ---
    auto nav_switch_tab_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        sad::nav::switchTab(args[0]->toInt());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa8\xd8\xaf\xd9\x84_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8", nav_switch_tab_func); // بدل_تبويب
    interpreter.getFunctionManager().registerBuiltinFunction("nav_switch_tab", nav_switch_tab_func);

    // --- أنشئ_متنقل_تبويب / nav_create_tab_navigator ---
    auto nav_create_tab_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int tabCount = args.empty() ? 3 : args[0]->toInt();
        return std::make_shared<Data::Value>(sad::nav::createTabNavigator(tabCount));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd8\xaa\xd9\x86\xd9\x82\xd9\x84_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8", nav_create_tab_func); // أنشئ_متنقل_تبويب
    interpreter.getFunctionManager().registerBuiltinFunction("nav_create_tab_navigator", nav_create_tab_func);

    // --- أظهر_حوار_تنقل / nav_show_dialog ---
    auto nav_show_dialog_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        return std::make_shared<Data::Value>(sad::nav::showDialog(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb8\xd9\x87\xd8\xb1_\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84", nav_show_dialog_func); // أظهر_حوار_تنقل
    interpreter.getFunctionManager().registerBuiltinFunction("nav_show_dialog", nav_show_dialog_func);

    // --- أخف_حوار / nav_dismiss_dialog ---
    auto nav_dismiss_dialog_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        sad::nav::dismissDialog();
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xae\xd9\x81_\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1", nav_dismiss_dialog_func); // أخف_حوار
    interpreter.getFunctionManager().registerBuiltinFunction("nav_dismiss_dialog", nav_dismiss_dialog_func);

    // --- هل_الحوار_ظاهر / nav_is_dialog_showing ---
    auto nav_is_dialog_showing_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::nav::isDialogShowing());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1_\xd8\xb8\xd8\xa7\xd9\x87\xd8\xb1", nav_is_dialog_showing_func); // هل_حوار_ظاهر
    interpreter.getFunctionManager().registerBuiltinFunction("nav_is_dialog_showing", nav_is_dialog_showing_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) إدارة الحالة / (EN) State Management
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_حالة_رقم / state_set_number ---
    auto state_set_number_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::state::setState(args[0]->toString(), args[1]->toDouble());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xb1\xd9\x82\xd9\x85", state_set_number_func); // عين_حالة_رقم
    interpreter.getFunctionManager().registerBuiltinFunction("state_set_number", state_set_number_func);

    // --- عين_حالة_نص / state_set_text ---
    auto state_set_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::state::setState(args[0]->toString(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x86\xd8\xb5", state_set_text_func); // عين_حالة_نص
    interpreter.getFunctionManager().registerBuiltinFunction("state_set_text", state_set_text_func);

    // --- عين_حالة_منطقي / state_set_bool ---
    auto state_set_bool_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::state::setState(args[0]->toString(), args[1]->toBool());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", state_set_bool_func); // عين_حالة_منطقي
    interpreter.getFunctionManager().registerBuiltinFunction("state_set_bool", state_set_bool_func);

    // --- اقرأ_حالة_رقم / state_get_number ---
    auto state_get_number_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        std::string key = args[0]->toString();
        double def = args.size() > 1 ? args[1]->toDouble() : 0.0;
        return std::make_shared<Data::Value>(sad::state::getStateNumber(key, def));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd8\xb1\xd9\x82\xd9\x85", state_get_number_func); // اقرأ_حالة_رقم
    interpreter.getFunctionManager().registerBuiltinFunction("state_get_number", state_get_number_func);

    // --- اقرأ_حالة_نص / state_get_text ---
    auto state_get_text_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
        std::string key = args[0]->toString();
        std::string def = args.size() > 1 ? args[1]->toString() : "";
        return std::make_shared<Data::Value>(sad::state::getStateText(key, def));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x86\xd8\xb5", state_get_text_func); // اقرأ_حالة_نص
    interpreter.getFunctionManager().registerBuiltinFunction("state_get_text", state_get_text_func);

    // --- اقرأ_حالة_منطقي / state_get_bool ---
    auto state_get_bool_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        std::string key = args[0]->toString();
        bool def = args.size() > 1 ? args[1]->toBool() : false;
        return std::make_shared<Data::Value>(sad::state::getStateBool(key, def));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", state_get_bool_func); // اقرأ_حالة_منطقي
    interpreter.getFunctionManager().registerBuiltinFunction("state_get_bool", state_get_bool_func);

    // --- هل_حالة_موجودة / state_has ---
    auto state_has_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::state::hasState(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x87\xd9\x84_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9_\xd9\x85\xd9\x88\xd8\xac\xd9\x88\xd8\xaf\xd8\xa9", state_has_func); // هل_حالة_موجودة
    interpreter.getFunctionManager().registerBuiltinFunction("state_has", state_has_func);

    // --- احذف_حالة / state_remove ---
    auto state_remove_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>();
        sad::state::removeState(args[0]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xb0\xd9\x81_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", state_remove_func); // احذف_حالة
    interpreter.getFunctionManager().registerBuiltinFunction("state_remove", state_remove_func);

    // --- امسح_الحالات / state_clear_all ---
    auto state_clear_all_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        sad::state::clearAllState();
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd9\x85\xd8\xb3\xd8\xad_\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa", state_clear_all_func); // امسح_الحالات
    interpreter.getFunctionManager().registerBuiltinFunction("state_clear_all", state_clear_all_func);

    // --- زد_حالة / state_increment ---
    auto state_increment_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0.0);
        std::string key = args[0]->toString();
        double amount = args.size() > 1 ? args[1]->toDouble() : 1.0;
        return std::make_shared<Data::Value>(sad::state::incrementState(key, amount));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb2\xd8\xaf_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", state_increment_func); // زد_حالة
    interpreter.getFunctionManager().registerBuiltinFunction("state_increment", state_increment_func);

    // --- بدل_حالة / state_toggle ---
    auto state_toggle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(sad::state::toggleState(args[0]->toString()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa8\xd8\xaf\xd9\x84_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9", state_toggle_func); // بدل_حالة
    interpreter.getFunctionManager().registerBuiltinFunction("state_toggle", state_toggle_func);

    // --- عدد_الحالات / state_count ---
    auto state_count_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::state::getStateCount());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa", state_count_func); // عدد_الحالات
    interpreter.getFunctionManager().registerBuiltinFunction("state_count", state_count_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) التراجع والإعادة / (EN) Undo/Redo
    // ═══════════════════════════════════════════════════════════════════

    // --- فعل_التاريخ / state_enable_history ---
    auto state_enable_history_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        bool enable = args.empty() ? true : args[0]->toBool();
        sad::state::enableHistory(enable);
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x81\xd8\xb9\xd9\x84_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae", state_enable_history_func); // فعل_التاريخ
    interpreter.getFunctionManager().registerBuiltinFunction("state_enable_history", state_enable_history_func);

    // --- تراجع / state_undo ---
    auto state_undo_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::state::undo());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xaa\xd8\xb1\xd8\xa7\xd8\xac\xd8\xb9", state_undo_func); // تراجع
    interpreter.getFunctionManager().registerBuiltinFunction("state_undo", state_undo_func);

    // --- أعد / state_redo ---
    auto state_redo_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::state::redo());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa3\xd8\xb9\xd8\xaf", state_redo_func); // أعد
    interpreter.getFunctionManager().registerBuiltinFunction("state_redo", state_redo_func);

    // --- يمكن_التراجع / state_can_undo ---
    auto state_can_undo_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::state::canUndo());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x8a\xd9\x85\xd9\x83\xd9\x86_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd8\xa7\xd8\xac\xd8\xb9", state_can_undo_func); // يمكن_التراجع
    interpreter.getFunctionManager().registerBuiltinFunction("state_can_undo", state_can_undo_func);

    // --- يمكن_الإعادة / state_can_redo ---
    auto state_can_redo_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        return std::make_shared<Data::Value>(sad::state::canRedo());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd9\x8a\xd9\x85\xd9\x83\xd9\x86_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9", state_can_redo_func); // يمكن_الإعادة
    interpreter.getFunctionManager().registerBuiltinFunction("state_can_redo", state_can_redo_func);

    // --- احفظ_لقطة / state_save_snapshot ---
    auto state_save_snapshot_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string name = args.empty() ? "" : args[0]->toString();
        return std::make_shared<Data::Value>(sad::state::saveSnapshot(name));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xad\xd9\x81\xd8\xb8_\xd9\x84\xd9\x82\xd8\xb7\xd8\xa9", state_save_snapshot_func); // احفظ_لقطة
    interpreter.getFunctionManager().registerBuiltinFunction("state_save_snapshot", state_save_snapshot_func);

    // --- استعد_لقطة / state_restore_snapshot ---
    auto state_restore_snapshot_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(false);
        // Support both int ID and string name
        if (args[0]->getType() == Data::ValueType::STRING) {
            return std::make_shared<Data::Value>(sad::state::restoreSnapshot(args[0]->toString()));
        }
        return std::make_shared<Data::Value>(sad::state::restoreSnapshot(args[0]->toInt()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb9\xd8\xaf_\xd9\x84\xd9\x82\xd8\xb7\xd8\xa9", state_restore_snapshot_func); // استعد_لقطة
    interpreter.getFunctionManager().registerBuiltinFunction("state_restore_snapshot", state_restore_snapshot_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الثيمات / (EN) Themes
    // ═══════════════════════════════════════════════════════════════════

    // --- عين_ثيمة_نص / set_theme_str ---
    auto set_theme_str_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::setThemeProperty(args[0]->toString(), args[1]->toString());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xab\xd9\x8a\xd9\x85\xd8\xa9_\xd9\x86\xd8\xb5", set_theme_str_func); // عين_ثيمة_نص
    interpreter.getFunctionManager().registerBuiltinFunction("set_theme_str", set_theme_str_func);

    // --- عين_ثيمة_رقم / set_theme_num ---
    auto set_theme_num_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>();
        sad::ui::setThemeProperty(args[0]->toString(), args[1]->toDouble());
        return std::make_shared<Data::Value>();
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xab\xd9\x8a\xd9\x85\xd8\xa9_\xd8\xb1\xd9\x82\xd9\x85", set_theme_num_func); // عين_ثيمة_رقم
    interpreter.getFunctionManager().registerBuiltinFunction("set_theme_num", set_theme_num_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الأحداث / (EN) Events
    // ═══════════════════════════════════════════════════════════════════

    // Note: collectEvents returns complex struct — expose simplified versions

    // --- اجمع_الأحداث / collect_events ---
    // Returns number of events collected (simplified)
    auto collect_events_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        auto events = sad::ui::collectEvents(args[0]->toInt());
        return std::make_shared<Data::Value>(static_cast<int>(events.size()));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("\xd8\xa7\xd8\xac\xd9\x85\xd8\xb9_\xd8\xa7\xd9\x84\xd8\xa3\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab", collect_events_func); // اجمع_الأحداث
    interpreter.getFunctionManager().registerBuiltinFunction("collect_events", collect_events_func);

    // ═══════════════════════════════════════════════════════════════════
    // (AR) معالجة الإرجاعات — يفحص الأحداث ويستدعي الدوال المسجلة
    // (EN) Process callbacks — checks events and invokes registered functions
    // ═══════════════════════════════════════════════════════════════════

    // --- معالجة_الأحداث / process_callbacks ---
    auto process_callbacks_func = [&interpreter](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int callCount = 0;
        auto widgets = sad::callbacks::getRegisteredWidgets();
        for (int wid : widgets) {
            // فحص النقر
            std::string clickFn = sad::callbacks::getCallback(wid, "click");
            if (!clickFn.empty() && sad::ui::button_was_clicked(wid)) {
                try {
                    std::vector<Data::Value> callArgs;
                    callArgs.push_back(Data::Value(wid));
                    interpreter.callUserFunction(clickFn, callArgs);
                    callCount++;
                } catch (...) {
                    // تجاهل الأخطاء في الإرجاع لمنع تعطل الحلقة
                }
            }
        }
        return std::make_shared<Data::Value>(callCount);
    };
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xa3\xd8\xad\xd8\xaf\xd8\xa7\xd8\xab", process_callbacks_func); // معالجة_الأحداث
    interpreter.getFunctionManager().registerBuiltinFunction("process_callbacks", process_callbacks_func);

} // registerBuiltinsPart13

} // namespace Interpreter
} // namespace Sad
