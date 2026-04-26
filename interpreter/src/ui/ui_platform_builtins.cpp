/**
 * @file ui_platform_builtins.cpp
 * @brief (AR) الأنظمة المنصّية: الموقع، الكاميرا، المستشعرات، المشاركة، الأذونات،
 *        السمات، التدويل، الوصولية، التخطيط المتجاوب، الطباعة، المقاييس الحيوية،
 *        الإيماءات، الخرائط، الرسم الحر
 * @brief (EN) Platform services: geo, camera, sensors, share, permissions, themes,
 *        i18n, accessibility, responsive, print, biometrics, gestures, maps, canvas
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

        void registerUIPlatformBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت المنصة
            namespace Bp = Builtins::Names::UIPlatform;

            // ═══ نظام 21: الرسوم المتحركة — محرك موحّد ═══
            // (EN) System 21: Unified Cross-Platform Animation Engine

            // حرّك(هدف، خاصية، من، إلى، مدة، [منحنى]) → معرّف
            auto start_anim_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 5)
                    return std::make_shared<Data::Value>(0.0);
                std::string target = args[0]->toString();
                std::string prop = args[1]->toString();
                double from = args[2]->toDouble();
                double to = args[3]->toDouble();
                int dur = static_cast<int>(args[4]->toDouble());
                std::string easing = args.size() > 5 ? args[5]->toString() : "linear";
                int id = bridge->startAnimation(target, prop, from, to, dur, easing);
                return std::make_shared<Data::Value>(static_cast<double>(id));
            };
            // عربي: حرّك (بالشدة وبدونها)
            fm.registerBuiltinFunction(std::string(Bp::ANIMATE), start_anim_fn); // حرّك

            // حركة_لون(هدف، خاصية، r1,g1,b1,a1, r2,g2,b2,a2, مدة، [منحنى]) → معرّف
            auto start_color_anim_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 11)
                    return std::make_shared<Data::Value>(0.0);
                std::string target = args[0]->toString();
                std::string prop = args[1]->toString();
                float fR = static_cast<float>(args[2]->toDouble());
                float fG = static_cast<float>(args[3]->toDouble());
                float fB = static_cast<float>(args[4]->toDouble());
                float fA = static_cast<float>(args[5]->toDouble());
                float tR = static_cast<float>(args[6]->toDouble());
                float tG = static_cast<float>(args[7]->toDouble());
                float tB = static_cast<float>(args[8]->toDouble());
                float tA = static_cast<float>(args[9]->toDouble());
                int dur = static_cast<int>(args[10]->toDouble());
                std::string easing = args.size() > 11 ? args[11]->toString() : "linear";
                int id = bridge->startColorAnimation(target, prop, fR, fG, fB, fA, tR, tG, tB, tA, dur, easing);
                return std::make_shared<Data::Value>(static_cast<double>(id));
            };
            fm.registerBuiltinFunction(std::string(Bp::COLOR_ANIM), start_color_anim_fn); // حركة_لون

            // أوقف_حركة(معرّف)
            auto stop_anim_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->stopAnimation(static_cast<int>(args[0]->toDouble()));
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::STOP_ANIM), stop_anim_fn); // أوقف_حركة

            // أوقف_حركات_عقدة(هدف)
            auto stop_node_anim_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->stopNodeAnimations(args[0]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::STOP_NODE_ANIM), stop_node_anim_fn); // أوقف_حركات_عقدة

            // إلغاء_كل_الحركات()
            auto cancel_all_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->stopAllAnimations();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::CANCEL_ALL_ANIM), cancel_all_fn); // إلغاء_كل_الحركات

            // حالة_حركة(معرّف) → "running"/"stopped"/"finished"/"not_found"
            auto anim_state_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string("not_found"));
                return std::make_shared<Data::Value>(bridge->getAnimationState(static_cast<int>(args[0]->toDouble())));
            };
            fm.registerBuiltinFunction(std::string(Bp::ANIM_STATE), anim_state_fn); // حالة_حركة

            // قيمة_حركة(هدف، خاصية، [قيمة_افتراضية]) → القيمة الحالية
            auto anim_value_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(0.0);
                double def = args.size() > 2 ? args[2]->toDouble() : 0.0;
                return std::make_shared<Data::Value>(bridge->getAnimatedValue(args[0]->toString(), args[1]->toString(), def));
            };
            fm.registerBuiltinFunction(std::string(Bp::ANIM_VALUE), anim_value_fn); // قيمة_حركة

            // حركات_نشطة() → منطقي
            auto has_active_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                return std::make_shared<Data::Value>(bridge ? bridge->hasActiveAnimations() : false);
            };
            fm.registerBuiltinFunction(std::string(Bp::HAS_ACTIVE_ANIM), has_active_fn); // حركات_نشطة

            // ═══ الموقع الجغرافي ═══
            auto geo_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                Data::Value::ArrayType arr;
                if (!bridge)
                {
                    arr.push_back(Data::Value(0.0));
                    arr.push_back(Data::Value(0.0));
                    return std::make_shared<Data::Value>(std::move(arr));
                }
                auto [lat, lng] = bridge->getLocation();
                arr.push_back(Data::Value(lat));
                arr.push_back(Data::Value(lng));
                return std::make_shared<Data::Value>(std::move(arr));
            };
            fm.registerBuiltinFunction(std::string(Bp::GEO_LOCATION), geo_fn);

            // ═══ الكاميرا ═══
            auto camera_open_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->openCamera());
            };
            fm.registerBuiltinFunction(std::string(Bp::CAMERA_OPEN), camera_open_fn);

            auto capture_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                std::string path = !args.empty() ? args[0]->toString() : "";
                return std::make_shared<Data::Value>(bridge->capturePhoto(path));
            };
            fm.registerBuiltinFunction(std::string(Bp::CAPTURE), capture_fn);

            // ═══ المستشعرات ═══
            auto sensor_read_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string("{}"));
                return std::make_shared<Data::Value>(bridge->readSensor(args[0]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bp::SENSOR_READ), sensor_read_fn);

            auto sensors_list_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string("[]"));
                return std::make_shared<Data::Value>(bridge->availableSensors());
            };
            fm.registerBuiltinFunction(std::string(Bp::SENSORS_LIST), sensors_list_fn);

            // ═══ المشاركة ═══
            auto share_text_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                std::string title = args.size() > 1 ? args[1]->toString() : "";
                return std::make_shared<Data::Value>(bridge->shareText(args[0]->toString(), title));
            };
            fm.registerBuiltinFunction(std::string(Bp::SHARE_TEXT), share_text_fn);

            auto share_file_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                std::string mime = args.size() > 1 ? args[1]->toString() : "";
                return std::make_shared<Data::Value>(bridge->shareFile(args[0]->toString(), mime));
            };
            fm.registerBuiltinFunction(std::string(Bp::SHARE_FILE), share_file_fn);

            // ═══ الأذونات ═══
            auto req_perm_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string("denied"));
                return std::make_shared<Data::Value>(bridge->requestPermission(args[0]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bp::REQUEST_PERM), req_perm_fn);

            auto check_perm_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->checkPermission(args[0]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bp::CHECK_PERM), check_perm_fn);

            // ═══ السمات ═══
            auto set_theme_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->setTheme(args[0]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::SET_THEME), set_theme_fn);

            auto get_theme_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->getTheme());
            };
            fm.registerBuiltinFunction(std::string(Bp::GET_THEME), get_theme_fn);

            auto sys_theme_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->getSystemTheme());
            };
            fm.registerBuiltinFunction(std::string(Bp::SYS_THEME), sys_theme_fn);

            // ═══ التدويل ═══
            auto translate_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->translate(args[0]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bp::TRANSLATE), translate_fn);

            auto set_locale_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->setLocale(args[0]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::SET_LOCALE), set_locale_fn);

            auto get_locale_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->getLocale());
            };
            fm.registerBuiltinFunction(std::string(Bp::GET_LOCALE), get_locale_fn);

            auto add_trans_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && args.size() >= 2)
                    bridge->addTranslation(args[0]->toString(), args[1]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::ADD_TRANSLATION), add_trans_fn);

            // ═══ الوصولية ═══
            auto screen_reader_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->screenReaderAnnounce(args[0]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::SCREEN_READER), screen_reader_fn);

            auto font_scale_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(1.0);
                return std::make_shared<Data::Value>(bridge->getSystemFontScale());
            };
            fm.registerBuiltinFunction(std::string(Bp::FONT_SCALE), font_scale_fn);

            // ═══ التخطيط المتجاوب ═══
            auto screen_w_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(0.0);
                return std::make_shared<Data::Value>(static_cast<double>(bridge->getScreenWidth()));
            };
            fm.registerBuiltinFunction(std::string(Bp::SCREEN_WIDTH), screen_w_fn);

            auto screen_h_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(0.0);
                return std::make_shared<Data::Value>(static_cast<double>(bridge->getScreenHeight()));
            };
            fm.registerBuiltinFunction(std::string(Bp::SCREEN_HEIGHT), screen_h_fn);

            auto density_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(96.0);
                return std::make_shared<Data::Value>(bridge->getPixelDensity());
            };
            fm.registerBuiltinFunction(std::string(Bp::SCREEN_DENSITY), density_fn);

            auto device_type_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string("unknown"));
                return std::make_shared<Data::Value>(bridge->getDeviceType());
            };
            fm.registerBuiltinFunction(std::string(Bp::DEVICE_TYPE), device_type_fn);

            // ═══ الطباعة ═══
            auto print_page_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                std::string title = args.size() > 1 ? args[1]->toString() : "";
                return std::make_shared<Data::Value>(bridge->printPage(args[0]->toString(), title));
            };
            fm.registerBuiltinFunction(std::string(Bp::PRINT_PAGE), print_page_fn);

            // ═══ المقاييس الحيوية ═══
            auto bio_auth_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(false);
                std::string reason = !args.empty() ? args[0]->toString() : "";
                return std::make_shared<Data::Value>(bridge->authenticateBiometric(reason));
            };
            fm.registerBuiltinFunction(std::string(Bp::BIO_AUTH), bio_auth_fn);

            auto bio_support_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->hasBiometricSupport());
            };
            fm.registerBuiltinFunction(std::string(Bp::BIO_SUPPORT), bio_support_fn);

            // ═══ الإيماءات المتقدمة ═══
            auto swipe_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->setSwipeHandler(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::ON_SWIPE), swipe_fn);

            auto pinch_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->setPinchHandler(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::ON_PINCH), pinch_fn);

            auto rotate_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->setRotateHandler(*args[0]);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::ON_ROTATE), rotate_fn);

            // ═══ الخرائط ═══
            auto create_map_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 3)
                    return std::make_shared<Data::Value>(0.0);
                int id = bridge->createMap(args[0]->toDouble(), args[1]->toDouble(), static_cast<int>(args[2]->toDouble()));
                return std::make_shared<Data::Value>(static_cast<double>(id));
            };
            fm.registerBuiltinFunction(std::string(Bp::CREATE_MAP), create_map_fn);

            auto add_marker_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 4)
                    return std::make_shared<Data::Value>();
                bridge->addMapMarker(static_cast<int>(args[0]->toDouble()),
                                     args[1]->toDouble(), args[2]->toDouble(), args[3]->toString());
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::ADD_MARKER), add_marker_fn);

            // ═══ الرسم الحر ═══
            auto create_canvas_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(0.0);
                int id = bridge->createCanvas(static_cast<int>(args[0]->toDouble()), static_cast<int>(args[1]->toDouble()));
                return std::make_shared<Data::Value>(static_cast<double>(id));
            };
            fm.registerBuiltinFunction(std::string(Bp::CREATE_CANVAS), create_canvas_fn);

            auto draw_line_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 6)
                    return std::make_shared<Data::Value>();
                int thick = args.size() > 6 ? static_cast<int>(args[6]->toDouble()) : 1;
                bridge->drawLine(static_cast<int>(args[0]->toDouble()),
                                 static_cast<int>(args[1]->toDouble()), static_cast<int>(args[2]->toDouble()),
                                 static_cast<int>(args[3]->toDouble()), static_cast<int>(args[4]->toDouble()),
                                 args[5]->toString(), thick);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::DRAW_LINE), draw_line_fn);

            auto draw_circle_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 5)
                    return std::make_shared<Data::Value>();
                bool fill = args.size() > 5 ? args[5]->toBool() : true;
                bridge->drawCircle(static_cast<int>(args[0]->toDouble()),
                                   static_cast<int>(args[1]->toDouble()), static_cast<int>(args[2]->toDouble()),
                                   static_cast<int>(args[3]->toDouble()), args[4]->toString(), fill);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::DRAW_CIRCLE), draw_circle_fn);

            auto draw_rect_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 6)
                    return std::make_shared<Data::Value>();
                bool fill = args.size() > 6 ? args[6]->toBool() : true;
                bridge->drawRect(static_cast<int>(args[0]->toDouble()),
                                 static_cast<int>(args[1]->toDouble()), static_cast<int>(args[2]->toDouble()),
                                 static_cast<int>(args[3]->toDouble()), static_cast<int>(args[4]->toDouble()),
                                 args[5]->toString(), fill);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::DRAW_RECT), draw_rect_fn);

            auto clear_canvas_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (bridge && !args.empty())
                    bridge->clearCanvas(static_cast<int>(args[0]->toDouble()));
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bp::CLEAR_CANVAS), clear_canvas_fn);
        }

    } // namespace Interpreter
} // namespace Sad
