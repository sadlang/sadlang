/**
 * @file builtin_registry_part3.cpp
 * @brief (AR) جزء من تسجيل الدوال المضمنة
 */

#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "type_functions.h"
#include "array_functions.h"
#include "other_functions.h"
#include "string/string_functions.h"
#include "math/math_functions.h"
#include "math/advanced_math.h"
#include "system/system_functions.h"
#include "filesystem/filesystem_module.h"
#include "graphics/graphics_module.h"
#include "graphics/sad_particles.h"
#include "graphics/sad_effects.h"
#include "graphics/sad_physics.h"
#include "graphics/sad_scene3d.h"
#include "graphics/sad_ai.h"
#include "graphics/sad_camera2d.h"
#include "graphics/sad_charts.h"
#include "graphics/sad_audio.h"
#include "graphics/sad_tilemap.h"
#include "graphics/sad_islamic_art.h"
#include "graphics/sad_dhikr.h"
#include "graphics/sad_qibla.h"
#include "graphics/sad_prayer.h"
#include "graphics/sad_hijri.h"
#include "graphics/sad_android.h"
#include "mobile/mobile_module.h"
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <random>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <set>
#include <regex>
#include <numeric>
#include <bitset>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <atomic>
#include <queue>
#include <unordered_map>
#include <condition_variable>

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsPart3(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS
    // Check checkbox state / فحص حالة مربع الاختيار
    auto checkbox_is_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.empty()) {
            throw std::runtime_error("(AR) مربع_اختيار_محدد: معامل مربع الاختيار مطلوب / (EN) checkbox_is_checked: Checkbox argument required");
        }
        
        int checkboxId = args[0]->toInt();
        bool checked = sad::stdlib::graphics::checkbox_is_checked_impl(checkboxId);
        return std::make_shared<Data::Value>(checked);
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع_اختيار_محدد", checkbox_is_checked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_is_checked", checkbox_is_checked_func);
    
    // Set checkbox state / تعيين حالة مربع الاختيار
    auto checkbox_set_checked_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) مربع_اختيار_تعيين_حالة: معاملان مطلوبان / (EN) checkbox_set_checked: 2 arguments required");
        }
        
        int checkboxId = args[0]->toInt();
        bool checked = args[1]->toBool();
        sad::stdlib::graphics::checkbox_set_checked_impl(checkboxId, checked);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("مربع_اختيار_تعيين_حالة", checkbox_set_checked_func);
    interpreter.getFunctionManager().registerBuiltinFunction("checkbox_set_checked", checkbox_set_checked_func);
    
    // Draw widget / رسم عنصر
    auto widget_draw_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) عنصر_رسم: معاملان مطلوبان (عنصر، رسام) / (EN) widget_draw: 2 arguments required (widget, renderer)");
        }
        
        int widgetId = args[0]->toInt();
        int rendererId = args[1]->toInt();
        sad::stdlib::graphics::widget_draw_impl(widgetId, rendererId);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عنصر_رسم", widget_draw_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_draw", widget_draw_func);
    
    // Update widget / تحديث عنصر
    auto widget_update_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 2) {
            throw std::runtime_error("(AR) عنصر_تحديث: معاملان مطلوبان (عنصر، زمن) / (EN) widget_update: 2 arguments required (widget, deltaTime)");
        }
        
        int widgetId = args[0]->toInt();
        float deltaTime = static_cast<float>(args[1]->toDouble());
        sad::stdlib::graphics::widget_update_impl(widgetId, deltaTime);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عنصر_تحديث", widget_update_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_update", widget_update_func);
    
    // Handle mouse event / معالجة حدث الفأرة
    auto widget_mouse_event_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        if (args.size() < 4) {
            throw std::runtime_error("(AR) عنصر_حدث_فأرة: 4 معاملات مطلوبة (عنصر، x، y، مضغوط) / (EN) widget_mouse_event: 4 arguments required (widget, x, y, pressed)");
        }
        
        int widgetId = args[0]->toInt();
        int x = args[1]->toInt();
        int y = args[2]->toInt();
        bool pressed = args[3]->toBool();
        
        sad::stdlib::graphics::widget_mouse_event_impl(widgetId, x, y, pressed);
        return std::make_shared<Data::Value>();
    };
    
    interpreter.getFunctionManager().registerBuiltinFunction("عنصر_حدث_فأرة", widget_mouse_event_func);
    interpreter.getFunctionManager().registerBuiltinFunction("widget_mouse_event", widget_mouse_event_func);
    
    // ╔═══════════════════════════════════════════════════════════════════════╗
    // ║                                                                       ║
    // ║   الموجة الأولى: أنظمة الرسومات المتقدمة                              ║
    // ║   Wave 1: Advanced Graphics Systems                                   ║
    // ║                                                                       ║
    // ║   تتضمن أربعة أنظمة فرعية قوية:                                       ║
    // ║     1) نظام الجسيمات (Particles) — نار، دخان، مطر، ثلج، انفجار       ║
    // ║     2) نظام التأثيرات البصرية (Effects) — ظل، توهج، تدرج، زجاج       ║
    // ║     3) محرك الفيزياء (Physics) — تصادم، جاذبية، نوابض                 ║
    // ║     4) المشاهد ثلاثية الأبعاد (Scene3D) — مكعب، كرة، كاميرا، إضاءة   ║
    // ║                                                                       ║
    // ║   كل دالة مسجلة باسم عربي وإنجليزي لتحقيق ثنائية اللغة الكاملة      ║
    // ║                                                                       ║
    // ╚═══════════════════════════════════════════════════════════════════════╝

    // ===================================================================
    // النظام 1: الجسيمات — sad::particles
    // ===================================================================
    //
    // نظام الجسيمات يوفر القدرة على إنشاء تأثيرات بصرية مذهلة مثل
    // النار والدخان والمطر والثلج والانفجارات والمزيد. كل مُصدِر جسيمات
    // يمكن تخصيصه بالكامل من حيث اللون والسرعة والجاذبية والشكل.
    // هذا النظام لا يوجد نظيره في Flutter على الإطلاق!
    // ===================================================================

    // ─── أنشئ_مصدر_جسيمات ───
    // إنشاء مُصدِر جسيمات جديد في الموضع (س، ص) المحدد.
    // المُصدِر هو المصدر الذي تنطلق منه الجسيمات — يمكنك إنشاء عدة مُصدِرات
    // في المشهد الواحد لتأثيرات مختلفة (مثلاً: نار هنا ودخان هناك).
    // يُرجع معرّف المُصدِر الذي يُستخدم في باقي الدوال للتحكم به.
    auto particles_createEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        int id = sad::particles::createEmitter(x, y);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مصدر_جسيمات", particles_createEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_create_emitter", particles_createEmitter_func);

    // ─── احذف_مصدر ───
    // حذف مُصدِر جسيمات بمعرّفه. عند الحذف تختفي جميع الجسيمات
    // المرتبطة به فوراً من المشهد ويُحرَّر الذاكرة المخصصة له.
    // يُستخدم عندما لا نعود بحاجة لتأثير معين (مثلاً: انتهاء الانفجار).
    auto particles_destroyEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::particles::destroyEmitter(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_مصدر", particles_destroyEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_destroy_emitter", particles_destroyEmitter_func);

    // ─── احذف_كل_المصادر ───
    // حذف جميع مُصدِرات الجسيمات الموجودة في المشهد دفعة واحدة.
    // مفيد عند الانتقال بين المشاهد أو عند إعادة تعيين اللعبة بالكامل
    // لتنظيف كل التأثيرات البصرية القديمة.
    auto particles_destroyAllEmitters_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::particles::destroyAllEmitters();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كل_المصادر", particles_destroyAllEmitters_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_destroy_all_emitters", particles_destroyAllEmitters_func);

    // ─── ابدأ_جسيمات ───
    // بدء إصدار الجسيمات من المُصدِر المحدد. بعد إنشاء المُصدِر يكون
    // متوقفاً بشكل افتراضي — يجب استدعاء هذه الدالة لبدء تدفق الجسيمات.
    // بمجرد البدء يستمر المُصدِر في إطلاق الجسيمات حسب معدل الإصدار المحدد.
    auto particles_startEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::particles::startEmitter(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ابدأ_جسيمات", particles_startEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_start_emitter", particles_startEmitter_func);

    // ─── أوقف_جسيمات ───
    // إيقاف إصدار جسيمات جديدة من المُصدِر المحدد. الجسيمات الموجودة
    // حالياً تستمر في حركتها حتى تنتهي أعمارها وتختفي تدريجياً.
    // يمكن إعادة تشغيل المُصدِر لاحقاً بالدالة ابدأ_جسيمات.
    auto particles_stopEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::particles::stopEmitter(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أوقف_جسيمات", particles_stopEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_stop_emitter", particles_stopEmitter_func);

    // ─── دفعة_جسيمات ───
    // إطلاق دفعة واحدة من الجسيمات بعدد محدد. على عكس الإصدار المستمر،
    // هذه الدالة تطلق كمية محددة مرة واحدة ثم تتوقف. مثالية لتأثيرات
    // مثل الانفجار أو ضربة السيف أو جمع عنصر في اللعبة.
    auto particles_burstEmitter_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        int count = (int)args[1]->toDouble();
        sad::particles::burstEmitter(id, count);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دفعة_جسيمات", particles_burstEmitter_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_burst_emitter", particles_burstEmitter_func);

    // ─── عيّن_معدل_إصدار ───
    // تعيين عدد الجسيمات التي يُصدرها المُصدِر في الثانية الواحدة.
    // القيم الأعلى تعطي تأثيراً أكثف (مثل نار شديدة) والقيم الأقل
    // تعطي تأثيراً أخف (مثل بخار خفيف). القيمة الافتراضية عادةً 50.
    auto particles_setEmitRate_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float rate = (float)args[1]->toDouble();
        sad::particles::setEmitRate(id, rate);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_معدل_إصدار", particles_setEmitRate_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_emit_rate", particles_setEmitRate_func);

    // ─── عيّن_جاذبية_جسيمات ───
    // تعيين قوة الجاذبية المؤثرة على جسيمات مُصدِر معين.
    // الجاذبية تسحب الجسيمات في الاتجاه المحدد — مثلاً:
    // (0, 100) تسحب للأسفل (للنار التي تصعد استخدم 0, -100)
    // (50, 0) تسحب لليمين (لمحاكاة الرياح الجانبية).
    auto particles_setGravity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float gx = (float)args[1]->toDouble();
        float gy = (float)args[2]->toDouble();
        sad::particles::setGravity(id, gx, gy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_جاذبية_جسيمات", particles_setGravity_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_gravity", particles_setGravity_func);

    // ─── عيّن_رياح_جسيمات ───
    // تعيين قوة الرياح المؤثرة على الجسيمات. الرياح تضيف قوة ثابتة
    // في الاتجاه المحدد، مما يجعل الجسيمات تنحرف. مثلاً: رياح (20, 0)
    // تجعل الدخان ينحرف لليمين، ورياح (0, -10) تجعل الثلج يطير للأعلى قليلاً.
    auto particles_setWind_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float wx = (float)args[1]->toDouble();
        float wy = (float)args[2]->toDouble();
        sad::particles::setWind(id, wx, wy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_رياح_جسيمات", particles_setWind_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_wind", particles_setWind_func);

    // ─── عيّن_لون_بداية ───
    // تعيين اللون الذي يبدأ به كل جسيم عند ولادته (RGBA).
    // الجسيمات تتدرج من لون البداية إلى لون النهاية خلال عمرها.
    // مثلاً: للنار نبدأ بأصفر مشرق (255, 200, 50) وننتهي بأحمر داكن.
    // المعامل الرابع (الشفافية) اختياري وقيمته الافتراضية 255 (معتم بالكامل).
    auto particles_setColorStart_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        uint8_t a = args.size() > 4 ? (uint8_t)args[4]->toDouble() : 255;
        sad::particles::setColorStart(id, r, g, b, a);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_بداية", particles_setColorStart_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_color_start", particles_setColorStart_func);

    // ─── عيّن_لون_نهاية ───
    // تعيين اللون الذي ينتهي به كل جسيم عند اقتراب موته (RGBA).
    // الجسيمات تتدرج تلقائياً من لون البداية إلى هذا اللون.
    // المعامل الرابع (الشفافية) اختياري وقيمته الافتراضية 0 (شفاف بالكامل)
    // مما يجعل الجسيم يتلاشى قبل اختفائه.
    auto particles_setColorEnd_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        uint8_t a = args.size() > 4 ? (uint8_t)args[4]->toDouble() : 0;
        sad::particles::setColorEnd(id, r, g, b, a);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_نهاية", particles_setColorEnd_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_color_end", particles_setColorEnd_func);

    // ─── عيّن_موضع_مصدر ───
    // تغيير موضع مُصدِر الجسيمات أثناء التشغيل. مفيد لجعل التأثير
    // يتبع كائناً متحركاً (مثل ذيل صاروخ أو شعلة في يد شخصية اللعبة).
    // الجسيمات الموجودة بالفعل لا تتأثر — فقط الجسيمات الجديدة تنطلق من الموضع الجديد.
    auto particles_setPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::particles::setPosition(id, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_مصدر", particles_setPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_set_position", particles_setPosition_func);

    // ─── عدد_الجسيمات ───
    // الحصول على عدد الجسيمات الحية حالياً في مُصدِر معين.
    // مفيد لمراقبة الأداء أو لتحديد متى انتهى تأثير معين
    // (عندما يصل العدد إلى صفر بعد إيقاف المُصدِر).
    auto particles_getParticleCount_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        int count = sad::particles::getParticleCount(id);
        return std::make_shared<Data::Value>((double)count);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عدد_الجسيمات", particles_getParticleCount_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_get_count", particles_getParticleCount_func);

    // ─── حدّث_جسيمات ───
    // تحديث فيزياء جميع الجسيمات ورسمها على المُصيِّر (renderer).
    // يجب استدعاء هذه الدالة في كل إطار (frame) من حلقة اللعبة الرئيسية.
    // المعامل الأول: مؤشر المُصيِّر (renderer) كقيمة رقمية.
    // المعامل الثاني: الزمن المنقضي منذ آخر إطار بالملي ثانية (مثلاً: 16.67 لـ 60 إطار/ثانية).
    // هذه الدالة تحدّث حركة الجسيمات وتطبّق الجاذبية والرياح ثم ترسمها.
    auto particles_updateAndRenderAll_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        double deltaMs = args[1]->toDouble();
        sad::particles::updateAndRenderAll(renderer, deltaMs);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدّث_جسيمات", particles_updateAndRenderAll_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_update_and_render", particles_updateAndRenderAll_func);

    // ─── التأثيرات الجاهزة (Presets) ───
    // هذه مجموعة من التأثيرات المُعدَّة مسبقاً والتي يمكن استخدامها بسطر واحد.
    // كل تأثير يُنشئ مُصدِر جسيمات مُهيَّأ تلقائياً بالإعدادات المثالية.
    // تُرجع جميعها معرّف المُصدِر للتحكم به لاحقاً.

    // ─── نار 🔥 ───
    // إنشاء تأثير نار واقعي في الموضع المحدد. اللهب يصعد للأعلى مع
    // تدرج من الأصفر المشرق إلى الأحمر الداكن. المعامل الثالث يتحكم
    // بشدة النار (1.0 = عادية، 2.0 = مضاعفة، 0.5 = خفيفة).
    auto particles_presetFire_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float intensity = args.size() > 2 ? (float)args[2]->toDouble() : 1.0f;
        int id = sad::particles::presetFire(x, y, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نار", particles_presetFire_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_fire", particles_presetFire_func);

    // ─── دخان 💨 ───
    // إنشاء تأثير دخان — سحابة رمادية تصعد ببطء وتتلاشى تدريجياً.
    // مثالي للبراكين والمداخن ومحركات السيارات والحرائق المنطفئة.
    // المعامل الثالث يتحكم بكثافة الدخان (عدد الجسيمات وحجمها).
    auto particles_presetSmoke_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float density = args.size() > 2 ? (float)args[2]->toDouble() : 1.0f;
        int id = sad::particles::presetSmoke(x, y, density);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دخان", particles_presetSmoke_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_smoke", particles_presetSmoke_func);

    // ─── مطر 🌧️ ───
    // إنشاء تأثير مطر يغطي عرض الشاشة المحدد. القطرات تتساقط
    // من الأعلى بسرعة عالية مع ميلان خفيف يحاكي الرياح.
    // المعامل الأول: عرض منطقة المطر. المعامل الثاني: شدة المطر.
    auto particles_presetRain_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float width = (float)args[0]->toDouble();
        float intensity = args.size() > 1 ? (float)args[1]->toDouble() : 1.0f;
        int id = sad::particles::presetRain(width, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مطر", particles_presetRain_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_rain", particles_presetRain_func);

    // ─── ثلج ❄️ ───
    // إنشاء تأثير ثلج — رقائق بيضاء تتساقط ببطء مع تمايل أفقي طفيف.
    // يعطي شعوراً بالشتاء والبرد. المعامل الأول: عرض منطقة الثلج.
    // المعامل الثاني: شدة التساقط (كلما زادت كلما زاد عدد الرقائق).
    auto particles_presetSnow_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float width = (float)args[0]->toDouble();
        float intensity = args.size() > 1 ? (float)args[1]->toDouble() : 1.0f;
        int id = sad::particles::presetSnow(width, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ثلج", particles_presetSnow_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_snow", particles_presetSnow_func);

    // ─── كونفيتي 🎊 ───
    // إنشاء تأثير كونفيتي احتفالي — قصاصات ملونة تتطاير في كل الاتجاهات.
    // مثالي لشاشات الفوز والاحتفالات. المعامل الثالث يحدد عدد القصاصات
    // (الافتراضي 100 قصاصة).
    auto particles_presetConfetti_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        int count = args.size() > 2 ? (int)args[2]->toDouble() : 100;
        int id = sad::particles::presetConfetti(x, y, count);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("كونفيتي", particles_presetConfetti_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_confetti", particles_presetConfetti_func);

    // ─── شرارات ✨ ───
    // إنشاء تأثير شرارات متألقة — نقاط مضيئة تظهر وتختفي في دائرة.
    // مثالي للعناصر السحرية والنجوم والجوائز. المعامل الثالث يحدد
    // نصف قطر الدائرة التي تظهر فيها الشرارات (الافتراضي 50 بكسل).
    auto particles_presetSparkle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float radius = args.size() > 2 ? (float)args[2]->toDouble() : 50.0f;
        int id = sad::particles::presetSparkle(x, y, radius);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("شرارات", particles_presetSparkle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_sparkle", particles_presetSparkle_func);

    // ─── انفجار 💥 ───
    // إنشاء تأثير انفجار — دفعة واحدة من الجسيمات تتطاير في كل الاتجاهات.
    // مثالي لتدمير الأعداء والانفجارات الكبيرة. المعامل الثالث يتحكم بقوة
    // الانفجار (سرعة الجسيمات وعددها). ينتهي تلقائياً بعد انطلاق الجسيمات.
    auto particles_presetExplosion_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float power = args.size() > 2 ? (float)args[2]->toDouble() : 1.0f;
        int id = sad::particles::presetExplosion(x, y, power);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("انفجار", particles_presetExplosion_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_explosion", particles_presetExplosion_func);

    // ─── فقاعات 🌊 ───
    // إنشاء تأثير فقاعات صاعدة — دوائر شفافة تصعد ببطء وتتمايل.
    // مثالي للمشاهد المائية وأحواض السمك والمشروبات الغازية.
    // المعامل الثالث يحدد معدل إنتاج الفقاعات في الثانية (الافتراضي 10).
    auto particles_presetBubbles_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float rate = args.size() > 2 ? (float)args[2]->toDouble() : 10.0f;
        int id = sad::particles::presetBubbles(x, y, rate);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فقاعات", particles_presetBubbles_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_bubbles", particles_presetBubbles_func);

    // ─── مسار_نجمي ⭐ ───
    // إنشاء تأثير مسار نجمي — جسيمات تتبع نقطة معينة (مثل مؤشر الفأرة).
    // كل جسيم يظهر خلف الحركة ويتلاشى تدريجياً مكوّناً ذيلاً لامعاً.
    // لتحريك المسار استخدم عيّن_موضع_مصدر لتحديث الموضع في كل إطار.
    auto particles_presetTrail_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        int id = sad::particles::presetTrail(x, y);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مسار_نجمي", particles_presetTrail_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_trail", particles_presetTrail_func);

    // ─── بتلات 🌸 ───
    // إنشاء تأثير بتلات أزهار متساقطة — أوراق زهرية تهبط ببطء مع
    // تمايل أنيق يحاكي سقوط أوراق الكرز اليابانية. يعطي إحساساً
    // رومانسياً وجميلاً. المعامل الأول: عرض المنطقة. الثاني: الشدة.
    auto particles_presetPetals_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float width = (float)args[0]->toDouble();
        float intensity = args.size() > 1 ? (float)args[1]->toDouble() : 1.0f;
        int id = sad::particles::presetPetals(width, intensity);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("بتلات", particles_presetPetals_func);
    interpreter.getFunctionManager().registerBuiltinFunction("particles_petals", particles_presetPetals_func);

    // ===================================================================
    // النظام 2: التأثيرات البصرية — sad::effects
    // ===================================================================
    //
    // نظام التأثيرات البصرية يوفر مجموعة غنية من التأثيرات التي لا
    // تتوفر في Flutter افتراضياً. يشمل الظلال الناعمة والتوهج
    // والتدرجات اللونية والزجاج الشفاف والنيومورفيزم وفلاتر الألوان
    // وتأثيرات التموج والحدود المتدرجة — كلها بأداء عالٍ فوق SDL2.
    // ===================================================================

    // ─── ارسم_ظل ───
    // رسم ظل ناعم خلف عنصر مستطيل. الظل يعطي إحساساً بالعمق والارتفاع.
    // المعاملات: المُصيِّر، موضع وأبعاد المستطيل (x, y, w, h)،
    // إزاحة الظل (أفقي، عمودي)، مقدار الضبابية، لون الظل (أحمر، أخضر، أزرق)،
    // وشفافية الظل (0.0 = شفاف تماماً، 1.0 = معتم تماماً).
    auto effects_drawShadowSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float offX = (float)args[5]->toDouble();
        float offY = (float)args[6]->toDouble();
        float blur = (float)args[7]->toDouble();
        uint8_t r = (uint8_t)args[8]->toDouble();
        uint8_t g = (uint8_t)args[9]->toDouble();
        uint8_t b = (uint8_t)args[10]->toDouble();
        float alpha = (float)args[11]->toDouble();
        sad::effects::drawShadowSimple(renderer, x, y, w, h, offX, offY, blur, r, g, b, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_ظل", effects_drawShadowSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_shadow", effects_drawShadowSimple_func);

    // ─── ارسم_توهج ───
    // رسم تأثير توهج (Glow) حول عنصر مستطيل. التوهج يعطي انطباعاً بأن
    // العنصر يشع ضوءاً. مثالي لإبراز الأزرار النشطة أو العناصر المهمة.
    // المعاملات: المُصيِّر، الموضع والأبعاد، نصف قطر التوهج، اللون (أحمر، أخضر، أزرق)،
    // وشدة التوهج (كلما زادت كلما كان التوهج أوضح).
    auto effects_drawGlowSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float radius = (float)args[5]->toDouble();
        uint8_t r = (uint8_t)args[6]->toDouble();
        uint8_t g = (uint8_t)args[7]->toDouble();
        uint8_t b = (uint8_t)args[8]->toDouble();
        float intensity = (float)args[9]->toDouble();
        sad::effects::drawGlowSimple(renderer, x, y, w, h, radius, r, g, b, intensity);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_توهج", effects_drawGlowSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_glow", effects_drawGlowSimple_func);

    // ─── ارسم_تدرج_افقي ───
    // رسم تدرج لوني أفقي (من اليسار إلى اليمين) داخل مستطيل.
    // التدرج ينتقل سلساً بين لونين. مثالي لخلفيات الأزرار والعناوين
    // والشرائط الزخرفية. المعاملات: المُصيِّر، الموضع والأبعاد،
    // اللون الأول (بداية التدرج)، اللون الثاني (نهاية التدرج)، والشفافية.
    auto effects_drawGradientH_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        uint8_t r1 = (uint8_t)args[5]->toDouble();
        uint8_t g1 = (uint8_t)args[6]->toDouble();
        uint8_t b1 = (uint8_t)args[7]->toDouble();
        uint8_t r2 = (uint8_t)args[8]->toDouble();
        uint8_t g2 = (uint8_t)args[9]->toDouble();
        uint8_t b2 = (uint8_t)args[10]->toDouble();
        float alpha = (float)args[11]->toDouble();
        sad::effects::drawGradientH(renderer, x, y, w, h, r1, g1, b1, r2, g2, b2, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تدرج_افقي", effects_drawGradientH_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_gradient_h", effects_drawGradientH_func);

    // ─── ارسم_تدرج_عمودي ───
    // رسم تدرج لوني عمودي (من الأعلى إلى الأسفل) داخل مستطيل.
    // مشابه للتدرج الأفقي لكن الاتجاه رأسي. مثالي لخلفيات السماء
    // (أزرق فاتح في الأعلى إلى أزرق غامق في الأسفل) أو لأشرطة التقدم.
    auto effects_drawGradientV_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        uint8_t r1 = (uint8_t)args[5]->toDouble();
        uint8_t g1 = (uint8_t)args[6]->toDouble();
        uint8_t b1 = (uint8_t)args[7]->toDouble();
        uint8_t r2 = (uint8_t)args[8]->toDouble();
        uint8_t g2 = (uint8_t)args[9]->toDouble();
        uint8_t b2 = (uint8_t)args[10]->toDouble();
        float alpha = (float)args[11]->toDouble();
        sad::effects::drawGradientV(renderer, x, y, w, h, r1, g1, b1, r2, g2, b2, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تدرج_عمودي", effects_drawGradientV_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_gradient_v", effects_drawGradientV_func);

    // ─── ارسم_تدرج_دائري ───
    // رسم تدرج لوني دائري (شعاعي) — اللون يتغير من المركز إلى الحافة.
    // مثالي لتأثيرات الإضاءة المركزية (بقعة ضوء) أو خلفيات دائرية جميلة.
    // المعاملات: المُصيِّر، مركز الدائرة (cx, cy)، نصف القطر، لونا البداية والنهاية، والشفافية.
    auto effects_drawGradientRadial_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int cx = (int)args[1]->toDouble();
        int cy = (int)args[2]->toDouble();
        float radius = (float)args[3]->toDouble();
        uint8_t r1 = (uint8_t)args[4]->toDouble();
        uint8_t g1 = (uint8_t)args[5]->toDouble();
        uint8_t b1 = (uint8_t)args[6]->toDouble();
        uint8_t r2 = (uint8_t)args[7]->toDouble();
        uint8_t g2 = (uint8_t)args[8]->toDouble();
        uint8_t b2 = (uint8_t)args[9]->toDouble();
        float alpha = (float)args[10]->toDouble();
        sad::effects::drawGradientRadial(renderer, cx, cy, radius, r1, g1, b1, r2, g2, b2, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تدرج_دائري", effects_drawGradientRadial_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_gradient_radial", effects_drawGradientRadial_func);

    // ─── ارسم_زجاج ───
    // رسم تأثير الزجاج الشفاف (Glass Morphism) — لوحة زجاجية مع ضبابية
    // وشفافية وصبغة لونية. هذا التأثير شائع في تصاميم واجهات iOS الحديثة
    // ويعطي مظهراً أنيقاً وعصرياً. المعاملات: المُصيِّر، الموضع والأبعاد،
    // مقدار الضبابية، الشفافية، ولون الصبغة (أحمر، أخضر، أزرق).
    auto effects_drawGlassPanelSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float blur = (float)args[5]->toDouble();
        float opacity = (float)args[6]->toDouble();
        uint8_t tR = (uint8_t)args[7]->toDouble();
        uint8_t tG = (uint8_t)args[8]->toDouble();
        uint8_t tB = (uint8_t)args[9]->toDouble();
        sad::effects::drawGlassPanelSimple(renderer, x, y, w, h, blur, opacity, tR, tG, tB);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_زجاج", effects_drawGlassPanelSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_glass", effects_drawGlassPanelSimple_func);

    // ─── ارسم_نيومورف ───
    // رسم تأثير النيومورفيزم (Neumorphism) — تصميم ثلاثي الأبعاد ناعم
    // يبدو كأن العنصر مطبوع في السطح نفسه. يتضمن ظلاً فاتحاً وظلاً داكناً
    // ليعطي إحساساً بالبروز أو الغؤور. المعاملات: المُصيِّر، الموضع والأبعاد،
    // عمق التأثير، لون الخلفية، وهل هو بارز (true) أم غائر (false).
    auto effects_drawNeumorphSimple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float depth = (float)args[5]->toDouble();
        uint8_t bgR = (uint8_t)args[6]->toDouble();
        uint8_t bgG = (uint8_t)args[7]->toDouble();
        uint8_t bgB = (uint8_t)args[8]->toDouble();
        bool convex = args[9]->toDouble() != 0.0;
        sad::effects::drawNeumorphSimple(renderer, x, y, w, h, depth, bgR, bgG, bgB, convex);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_نيومورف", effects_drawNeumorphSimple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_neumorph", effects_drawNeumorphSimple_func);

    // ─── فلتر_بني ───
    // تطبيق فلتر اللون البني القديم (Sepia) على منطقة مستطيلة من الشاشة.
    // يعطي الصورة مظهراً كلاسيكياً قديماً كصور القرن التاسع عشر.
    // المعامل الأخير يتحكم بشدة التأثير (0.0 = بلا تأثير، 1.0 = كامل).
    auto effects_applySepia_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float amount = args.size() > 5 ? (float)args[5]->toDouble() : 1.0f;
        sad::effects::applySepia(renderer, x, y, w, h, amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فلتر_بني", effects_applySepia_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_sepia", effects_applySepia_func);

    // ─── فلتر_رمادي ───
    // تطبيق فلتر التدرج الرمادي (Grayscale) على منطقة مستطيلة.
    // يحوّل كل الألوان إلى درجات الرمادي. مفيد لتأثيرات الموت في الألعاب
    // أو لإبراز عنصر ملون واحد بجعل باقي الشاشة رمادية.
    // المعامل الأخير يتحكم بشدة التأثير (0.0 = ألوان كاملة، 1.0 = رمادي تماماً).
    auto effects_applyGrayscale_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float amount = args.size() > 5 ? (float)args[5]->toDouble() : 1.0f;
        sad::effects::applyGrayscale(renderer, x, y, w, h, amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فلتر_رمادي", effects_applyGrayscale_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_grayscale", effects_applyGrayscale_func);

    // ─── فلتر_سطوع ───
    // تعديل سطوع منطقة مستطيلة من الشاشة. المعامل factor يحدد
    // مستوى السطوع: 1.0 = بلا تغيير، أكبر من 1.0 = أكثر سطوعاً،
    // أقل من 1.0 = أكثر عتمةً. مثلاً: 1.5 = أكثر سطوعاً بـ 50%، 0.5 = نصف السطوع.
    // مفيد لتأثيرات الوميض (flash) أو التعتيم عند فتح القوائم.
    auto effects_applyBrightness_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float factor = (float)args[5]->toDouble();
        sad::effects::applyBrightness(renderer, x, y, w, h, factor);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("فلتر_سطوع", effects_applyBrightness_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_brightness", effects_applyBrightness_func);

    // ─── ارسم_تموج ───
    // رسم تأثير تموج دائري (Ripple) — موجات دائرية تنتشر من نقطة مركزية.
    // مثالي لتأثيرات سقوط الحجر في الماء أو موجات الصوت أو الضغط على الشاشة.
    // المعاملات: المُصيِّر، مركز التموج (cx, cy)، نصف القطر، السعة (ارتفاع الموجة)،
    // الطور (لتحريك الموجة بمرور الوقت)، اللون، والشفافية.
    auto effects_drawRipple_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        float cx = (float)args[1]->toDouble();
        float cy = (float)args[2]->toDouble();
        float radius = (float)args[3]->toDouble();
        float amplitude = (float)args[4]->toDouble();
        float phase = (float)args[5]->toDouble();
        uint8_t r = (uint8_t)args[6]->toDouble();
        uint8_t g = (uint8_t)args[7]->toDouble();
        uint8_t b = (uint8_t)args[8]->toDouble();
        float alpha = (float)args[9]->toDouble();
        sad::effects::drawRipple(renderer, cx, cy, radius, amplitude, phase, r, g, b, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_تموج", effects_drawRipple_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_ripple", effects_drawRipple_func);

    // ─── ارسم_حافة ───
    // رسم تأثير الحافة المعتمة (Vignette) — تعتيم تدريجي عند حواف الشاشة.
    // يوجّه انتباه المشاهد إلى وسط الشاشة ويعطي مظهراً سينمائياً.
    // المعاملات: المُصيِّر، عرض وارتفاع الشاشة، نصف قطر المنطقة المضيئة،
    // نعومة الانتقال، وشدة التعتيم.
    auto effects_drawVignette_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int width = (int)args[1]->toDouble();
        int height = (int)args[2]->toDouble();
        float radius = (float)args[3]->toDouble();
        float softness = (float)args[4]->toDouble();
        float alpha = (float)args[5]->toDouble();
        sad::effects::drawVignette(renderer, width, height, radius, softness, alpha);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_حافة", effects_drawVignette_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_vignette", effects_drawVignette_func);

    // ─── ارسم_حد_متدرج ───
    // رسم حد (Border) بتدرج لوني حول مستطيل. الحد ينتقل سلساً بين لونين
    // مما يعطي مظهراً أنيقاً وعصرياً. مثالي لتزيين البطاقات والنوافذ والأزرار.
    // المعاملات: المُصيِّر، الموضع والأبعاد، سُمك الحد، لونا البداية والنهاية.
    auto effects_drawBorderGradient_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        int w = (int)args[3]->toDouble();
        int h = (int)args[4]->toDouble();
        float thickness = (float)args[5]->toDouble();
        uint8_t r1 = (uint8_t)args[6]->toDouble();
        uint8_t g1 = (uint8_t)args[7]->toDouble();
        uint8_t b1 = (uint8_t)args[8]->toDouble();
        uint8_t r2 = (uint8_t)args[9]->toDouble();
        uint8_t g2 = (uint8_t)args[10]->toDouble();
        uint8_t b2 = (uint8_t)args[11]->toDouble();
        sad::effects::drawBorderGradient(renderer, x, y, w, h, thickness, r1, g1, b1, r2, g2, b2);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_حد_متدرج", effects_drawBorderGradient_func);
    interpreter.getFunctionManager().registerBuiltinFunction("effects_draw_border_gradient", effects_drawBorderGradient_func);

#endif // HAS_GRAPHICS

}

} // namespace Interpreter
} // namespace Sad
