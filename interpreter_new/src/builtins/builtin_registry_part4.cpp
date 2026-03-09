/**
 * @file builtin_registry_part4.cpp
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

void registerBuiltinsPart4(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS
    // ===================================================================
    // النظام 3: محرك الفيزياء — sad::physics
    // ===================================================================
    //
    // محرك فيزياء ثنائي الأبعاد متكامل يوفر محاكاة فيزيائية واقعية.
    // يدعم الأجسام الصلبة (دوائر ومستطيلات)، كشف التصادمات،
    // الجاذبية والاحتكاك والارتداد، القوى والاندفاعات، القيود (النوابض)،
    // والرسم التصحيحي. يمكن محاكاة حتى 500 جسم بأداء ممتاز.
    // Flutter ليس لديه أي محرك فيزياء مدمج!
    // ===================================================================

    // ─── أنشئ_دائرة_فيزيائية ───
    // إنشاء جسم فيزيائي دائري الشكل في الموضع (x, y) مع نصف القطر والكتلة المحددين.
    // الجسم الدائري مثالي لمحاكاة الكرات والعملات والفقاعات والقذائف.
    // يتأثر بالجاذبية والتصادمات تلقائياً. يُرجع معرّف الجسم للتحكم به.
    auto physics_createCircle_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float radius = (float)args[2]->toDouble();
        float mass = args.size() > 3 ? (float)args[3]->toDouble() : 1.0f;
        int id = sad::physics::createCircle(x, y, radius, mass);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_دائرة_فيزيائية", physics_createCircle_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_circle", physics_createCircle_func);

    // ─── أنشئ_صندوق_فيزيائي ───
    // إنشاء جسم فيزيائي مستطيل الشكل (صندوق) في الموضع (x, y) مع العرض والارتفاع والكتلة.
    // الصندوق مثالي لمحاكاة الصناديق والطوب والبلوكات والمنصات المتحركة.
    // يتأثر بالجاذبية والتصادمات ويتفاعل مع الأجسام الأخرى. يُرجع معرّف الجسم.
    auto physics_createBox_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float w = (float)args[2]->toDouble();
        float h = (float)args[3]->toDouble();
        float mass = args.size() > 4 ? (float)args[4]->toDouble() : 1.0f;
        int id = sad::physics::createBox(x, y, w, h, mass);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_صندوق_فيزيائي", physics_createBox_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_box", physics_createBox_func);

    // ─── أنشئ_منصة ───
    // إنشاء منصة ثابتة (Static Platform) لا تتحرك ولا تتأثر بالقوى.
    // المنصات الثابتة تُستخدم كأرضيات وجدران وحواجز وسلالم.
    // الأجسام المتحركة ترتد عنها لكنها هي لا تتزحزح أبداً.
    auto physics_createStaticPlatform_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float w = (float)args[2]->toDouble();
        float h = (float)args[3]->toDouble();
        int id = sad::physics::createStaticPlatform(x, y, w, h);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_منصة", physics_createStaticPlatform_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_platform", physics_createStaticPlatform_func);

    // ─── احذف_جسم ───
    // حذف جسم فيزيائي من العالم بمعرّفه. يختفي الجسم فوراً
    // ولا يعود يؤثر في التصادمات أو الفيزياء.
    auto physics_destroyBody_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::physics::destroyBody(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_جسم", physics_destroyBody_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_destroy_body", physics_destroyBody_func);

    // ─── عيّن_موضع_فيزيائي ───
    // نقل جسم فيزيائي مباشرة إلى موضع جديد (x, y).
    // تحذير: هذا يتجاوز الفيزياء — الجسم ينتقل فوراً بدون تصادمات.
    // يُستخدم عادةً لإعادة وضع الجسم بعد السقوط أو عند بدء مرحلة جديدة.
    auto physics_setPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::physics::setPosition(id, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_فيزيائي", physics_setPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_position", physics_setPosition_func);

    // ─── عيّن_سرعة ───
    // تعيين سرعة جسم فيزيائي مباشرة (vx, vy بالبكسل/ثانية).
    // مفيد لإطلاق قذيفة أو قفز شخصية أو تحريك جسم بسرعة محددة.
    // القيمة الموجبة في vy تعني الحركة للأسفل والسالبة للأعلى.
    auto physics_setVelocity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float vx = (float)args[1]->toDouble();
        float vy = (float)args[2]->toDouble();
        sad::physics::setVelocity(id, vx, vy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_سرعة", physics_setVelocity_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_velocity", physics_setVelocity_func);

    // ─── عيّن_ارتداد ───
    // تعيين معامل الارتداد (Restitution) لجسم فيزيائي.
    // القيمة 0 = لا ارتداد (الجسم يتوقف عند الاصطدام مثل الطين).
    // القيمة 1 = ارتداد مرن كامل (مثل كرة مطاطية).
    // القيم بين 0 و1 تعطي ارتداداً جزئياً (مثل كرة القدم ≈ 0.6).
    auto physics_setRestitution_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float rest = (float)args[1]->toDouble();
        sad::physics::setRestitution(id, rest);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_ارتداد", physics_setRestitution_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_restitution", physics_setRestitution_func);

    // ─── عيّن_احتكاك ───
    // تعيين معامل الاحتكاك (Friction) لجسم فيزيائي.
    // القيمة 0 = سطح زلق (مثل الجليد — لا يبطئ الحركة).
    // القيمة 1 = سطح خشن جداً (يوقف الحركة بسرعة).
    // مفيد لمحاكاة أنواع مختلفة من الأسطح في الألعاب.
    auto physics_setFriction_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float fric = (float)args[1]->toDouble();
        sad::physics::setFriction(id, fric);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_احتكاك", physics_setFriction_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_friction", physics_setFriction_func);

    // ─── عيّن_لون_جسم ───
    // تعيين لون عرض جسم فيزيائي عند رسمه بالدالة التصحيحية.
    // مفيد للتمييز بين أنواع الأجسام (مثلاً: أعداء بالأحمر، لاعب بالأزرق).
    auto physics_setBodyColor_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        sad::physics::setBodyColor(id, r, g, b);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_جسم", physics_setBodyColor_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_body_color", physics_setBodyColor_func);

    // ─── موضع_س_فيزيائي ───
    // الحصول على الإحداثي الأفقي (X) لجسم فيزيائي.
    // يُستخدم لمعرفة أين يقع الجسم حالياً في العالم الفيزيائي
    // لرسمه في المكان الصحيح أو لاتخاذ قرارات بناءً على موضعه.
    auto physics_getX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = sad::physics::getX(id);
        return std::make_shared<Data::Value>((double)x);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_س_فيزيائي", physics_getX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_get_x", physics_getX_func);

    // ─── موضع_ص_فيزيائي ───
    // الحصول على الإحداثي الرأسي (Y) لجسم فيزيائي.
    // مع موضع_س_فيزيائي يمكنك معرفة الموضع الكامل للجسم في العالم.
    // لاحظ أن المحور Y موجب للأسفل (كما هو معتاد في الرسوميات ثنائية الأبعاد).
    auto physics_getY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float y = sad::physics::getY(id);
        return std::make_shared<Data::Value>((double)y);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_ص_فيزيائي", physics_getY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_get_y", physics_getY_func);

    // ─── طبّق_قوة ───
    // تطبيق قوة مستمرة على جسم فيزيائي (بالنيوتن تقريباً).
    // القوة تُضاف إلى القوى الأخرى وتُطبَّق في الإطار التالي.
    // مثل الرياح أو محرك صاروخ — قوة مستمرة ما دام يتم استدعاؤها.
    // لقفزة أو ضربة مفاجئة استخدم طبّق_اندفاع بدلاً من هذه الدالة.
    auto physics_applyForce_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float fx = (float)args[1]->toDouble();
        float fy = (float)args[2]->toDouble();
        sad::physics::applyForce(id, fx, fy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طبّق_قوة", physics_applyForce_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_apply_force", physics_applyForce_func);

    // ─── طبّق_اندفاع ───
    // تطبيق اندفاع فوري (Impulse) على جسم فيزيائي — تغيير مباشر وفوري في السرعة.
    // على عكس القوة المستمرة، الاندفاع يحدث مرة واحدة فقط.
    // مثالي للقفز والرمي والضربات والانفجارات والنطّات.
    // القيمة السالبة في iy تعني اندفاع للأعلى (قفزة).
    auto physics_applyImpulse_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float ix = (float)args[1]->toDouble();
        float iy = (float)args[2]->toDouble();
        sad::physics::applyImpulse(id, ix, iy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طبّق_اندفاع", physics_applyImpulse_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_apply_impulse", physics_applyImpulse_func);

    // ─── عيّن_جاذبية_العالم ───
    // تعيين الجاذبية العامة لعالم الفيزياء بأكمله.
    // الجاذبية تؤثر على جميع الأجسام المتحركة (Dynamic).
    // القيمة الافتراضية عادةً (0, 980) لمحاكاة جاذبية الأرض (9.8 م/ث² × 100 بكسل/متر).
    // يمكن تغييرها لمحاكاة القمر (0, 160) أو الفضاء (0, 0) أو حتى عكسها (0, -500).
    auto physics_setWorldGravity_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float gx = (float)args[0]->toDouble();
        float gy = (float)args[1]->toDouble();
        sad::physics::setWorldGravity(gx, gy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_جاذبية_العالم", physics_setWorldGravity_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_world_gravity", physics_setWorldGravity_func);

    // ─── عيّن_حدود_العالم ───
    // تعيين حدود العالم الفيزيائي (المنطقة التي تحتوي الأجسام).
    // الأجسام التي تخرج من هذه الحدود تُعاد إلى الداخل (ترتد عن الجدران).
    // المعاملات: الحد الأيسر، العلوي، الأيمن، السفلي.
    // مفيد لمنع الأجسام من السقوط إلى ما لا نهاية.
    auto physics_setWorldBounds_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float left = (float)args[0]->toDouble();
        float top = (float)args[1]->toDouble();
        float right = (float)args[2]->toDouble();
        float bottom = (float)args[3]->toDouble();
        sad::physics::setWorldBounds(left, top, right, bottom);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_حدود_العالم", physics_setWorldBounds_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_set_world_bounds", physics_setWorldBounds_func);

    // ─── أنشئ_نابض ───
    // إنشاء قيد نابض (Spring Constraint) بين جسمين فيزيائيين.
    // النابض يحاول إبقاء المسافة بين الجسمين عند الطول المحدد —
    // إذا ابتعدا يسحبهما وإذا اقتربا يبعدهما. مثالي لمحاكاة الحبال
    // والجسور المعلقة والسلاسل وأجهزة الزنبرك.
    // المعاملات: معرّفا الجسمين، طول النابض، الصلابة (0-1)، التخميد (0-1).
    auto physics_createSpringConstraint_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int bodyA = (int)args[0]->toDouble();
        int bodyB = (int)args[1]->toDouble();
        float length = (float)args[2]->toDouble();
        float stiffness = args.size() > 3 ? (float)args[3]->toDouble() : 0.3f;
        float damping = args.size() > 4 ? (float)args[4]->toDouble() : 0.1f;
        int id = sad::physics::createSpringConstraint(bodyA, bodyB, length, stiffness, damping);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_نابض", physics_createSpringConstraint_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_create_spring", physics_createSpringConstraint_func);

    // ─── هل_يتصادم ───
    // فحص هل جسمان فيزيائيان يتصادمان (يتلامسان) حالياً.
    // يُرجع 1 (صحيح) إذا كان هناك تصادم، و0 (خطأ) إذا لم يكن.
    // مفيد لكشف وصول الكرة للهدف، أو ملامسة اللاعب لعدو، أو جمع عنصر.
    auto physics_isColliding_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int idA = (int)args[0]->toDouble();
        int idB = (int)args[1]->toDouble();
        bool colliding = sad::physics::isColliding(idA, idB);
        return std::make_shared<Data::Value>(colliding ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_يتصادم", physics_isColliding_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_is_colliding", physics_isColliding_func);

    // ─── عدد_الأجسام ───
    // الحصول على العدد الإجمالي للأجسام الفيزيائية الموجودة في العالم.
    // مفيد لمراقبة الأداء أو لعرض إحصائيات أو للتحقق من فراغ العالم.
    auto physics_getBodyCount_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int count = sad::physics::getBodyCount();
        return std::make_shared<Data::Value>((double)count);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عدد_الأجسام", physics_getBodyCount_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_get_body_count", physics_getBodyCount_func);

    // ─── خطوة_فيزيائية ───
    // تقديم المحاكاة الفيزيائية خطوة واحدة للأمام بالزمن المحدد (بالملي ثانية).
    // يجب استدعاء هذه الدالة في كل إطار من حلقة اللعبة الرئيسية.
    // هي التي تحرّك الأجسام وتكشف التصادمات وتطبّق الجاذبية والقوى.
    // مثال: خطوة_فيزيائية(16.67) لـ 60 إطار في الثانية.
    auto physics_step_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float dt = (float)args[0]->toDouble();
        sad::physics::step(dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("خطوة_فيزيائية", physics_step_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_step", physics_step_func);

    // ─── ارسم_فيزياء ───
    // رسم جميع الأجسام الفيزيائية بأشكالها الهندسية وألوانها على المُصيِّر.
    // هذه الدالة تصحيحية (Debug) — ترسم الدوائر والمستطيلات والقيود.
    // مفيدة أثناء التطوير لرؤية مواضع الأجسام وحدود التصادم.
    // المعامل: مؤشر المُصيِّر (renderer) كقيمة رقمية.
    auto physics_debugRender_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        sad::physics::debugRender(renderer);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_فيزياء", physics_debugRender_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_debug_render", physics_debugRender_func);

    // ─── أعد_تعيين_العالم ───
    // إعادة تعيين عالم الفيزياء بالكامل — حذف كل الأجسام والقيود
    // وإعادة كل الإعدادات إلى قيمها الافتراضية. يُستخدم عند بدء
    // مرحلة جديدة أو إعادة تشغيل اللعبة أو تحميل مشهد جديد.
    auto physics_resetWorld_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::physics::resetWorld();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أعد_تعيين_العالم", physics_resetWorld_func);
    interpreter.getFunctionManager().registerBuiltinFunction("physics_reset_world", physics_resetWorld_func);

    // ===================================================================
    // النظام 4: المشاهد ثلاثية الأبعاد — sad::scene3d
    // ===================================================================
    //
    // نظام رسم ثلاثي الأبعاد كامل فوق SDL2 — Flutter لا يدعم 3D إطلاقاً!
    // يتضمن إسقاطاً منظورياً، إضاءة اتجاهية ومحيطية، أشكالاً أساسية
    // (مكعب، كرة، مخروط، أسطوانة، مستوى)، كاميرا قابلة للتحريك والتدوير،
    // ورسم شبكي ومحاور إحداثية. يمكن عرض حتى 1000 كائن ثلاثي الأبعاد.
    // ===================================================================

    // ─── أنشئ_مكعب ───
    // إنشاء مكعب ثلاثي الأبعاد في الموضع (x, y, z) بالحجم المحدد.
    // المكعب هو أبسط الأشكال ثلاثية الأبعاد — 6 وجوه متساوية.
    // مثالي لتمثيل المباني والصناديق والأرضيات والجدران في مشاهد 3D.
    // المعامل الرابع (الحجم) اختياري وقيمته الافتراضية 1.0.
    auto scene3d_createCube_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float size = args.size() > 3 ? (float)args[3]->toDouble() : 1.0f;
        int id = sad::scene3d::createCube(x, y, z, size);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مكعب", scene3d_createCube_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_cube", scene3d_createCube_func);

    // ─── أنشئ_كرة ───
    // إنشاء كرة ثلاثية الأبعاد في الموضع (x, y, z) بنصف القطر وعدد الأقسام.
    // الكرة مكونة من مثلثات — كلما زاد عدد الأقسام (segments) كلما بدت أكثر نعومة
    // لكن تكلفة الرسم تزيد. القيمة الافتراضية 12 قسماً تعطي نتيجة جيدة.
    // مثالية لتمثيل الكواكب والكرات والجزيئات والنقاط المضيئة.
    auto scene3d_createSphere_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float radius = args.size() > 3 ? (float)args[3]->toDouble() : 0.5f;
        int segments = args.size() > 4 ? (int)args[4]->toDouble() : 12;
        int id = sad::scene3d::createSphere(x, y, z, radius, segments);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_كرة", scene3d_createSphere_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_sphere", scene3d_createSphere_func);

    // ─── أنشئ_مخروط ───
    // إنشاء مخروط ثلاثي الأبعاد في الموضع (x, y, z) بنصف القطر والارتفاع.
    // المخروط شكل هندسي قاعدته دائرة ورأسه نقطة. مثالي لتمثيل
    // الأشجار (أقماع خضراء) والأسهم والمؤشرات والقمع والمباني المخروطية.
    auto scene3d_createCone_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float radius = args.size() > 3 ? (float)args[3]->toDouble() : 0.5f;
        float height = args.size() > 4 ? (float)args[4]->toDouble() : 1.0f;
        int id = sad::scene3d::createCone(x, y, z, radius, height);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مخروط", scene3d_createCone_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_cone", scene3d_createCone_func);

    // ─── أنشئ_أسطوانة ───
    // إنشاء أسطوانة ثلاثية الأبعاد في الموضع (x, y, z) بنصف القطر والارتفاع.
    // الأسطوانة شكل ذو قاعدتين دائريتين وجسم مستقيم. مثالية لتمثيل
    // الأعمدة والأنابيب وجذوع الأشجار والعملات المعدنية (مع ارتفاع صغير).
    auto scene3d_createCylinder_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float radius = args.size() > 3 ? (float)args[3]->toDouble() : 0.5f;
        float height = args.size() > 4 ? (float)args[4]->toDouble() : 1.0f;
        int id = sad::scene3d::createCylinder(x, y, z, radius, height);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_أسطوانة", scene3d_createCylinder_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_cylinder", scene3d_createCylinder_func);

    // ─── أنشئ_مستوى ───
    // إنشاء مستوى (Plane) ثلاثي الأبعاد — سطح مسطح أفقي.
    // يُستخدم كأرضية للمشهد أو كسطح ماء أو كقاعدة للكائنات.
    // المعامل الرابع يحدد حجم المستوى (الافتراضي 5.0 وحدات).
    auto scene3d_createPlane_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        float size = args.size() > 3 ? (float)args[3]->toDouble() : 5.0f;
        int id = sad::scene3d::createPlane(x, y, z, size);
        return std::make_shared<Data::Value>((double)id);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_مستوى", scene3d_createPlane_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_create_plane", scene3d_createPlane_func);

    // ─── احذف_كائن_3D ───
    // حذف كائن ثلاثي الأبعاد من المشهد بمعرّفه.
    // الكائن يختفي فوراً ولا يُرسم بعد ذلك.
    auto scene3d_destroyObject_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        sad::scene3d::destroyObject(id);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كائن_3D", scene3d_destroyObject_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_destroy_object", scene3d_destroyObject_func);

    // ─── عيّن_موضع_3D ───
    // تعيين موضع كائن ثلاثي الأبعاد مباشرة إلى الإحداثيات (x, y, z).
    // المحور X أفقي (يمين/يسار)، المحور Y رأسي (أعلى/أسفل)،
    // المحور Z عمق (بعيد/قريب من الكاميرا). يُستخدم لتحريك الكائنات في المشهد.
    auto scene3d_setObjectPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        float z = (float)args[3]->toDouble();
        sad::scene3d::setObjectPosition(id, x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_3D", scene3d_setObjectPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_position", scene3d_setObjectPosition_func);

    // ─── عيّن_دوران_3D ───
    // تعيين دوران كائن ثلاثي الأبعاد حول المحاور الثلاثة (بالدرجات).
    // rx = الدوران حول المحور X (ميلان أمامي/خلفي).
    // ry = الدوران حول المحور Y (دوران يمين/يسار — الأكثر شيوعاً).
    // rz = الدوران حول المحور Z (إمالة جانبية).
    auto scene3d_setObjectRotation_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float rx = (float)args[1]->toDouble();
        float ry = (float)args[2]->toDouble();
        float rz = (float)args[3]->toDouble();
        sad::scene3d::setObjectRotation(id, rx, ry, rz);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_دوران_3D", scene3d_setObjectRotation_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_rotation", scene3d_setObjectRotation_func);

    // ─── عيّن_مقياس_3D ───
    // تعيين مقياس (حجم) كائن ثلاثي الأبعاد على المحاور الثلاثة.
    // (1, 1, 1) = الحجم الطبيعي. (2, 2, 2) = ضعف الحجم في كل الاتجاهات.
    // يمكن مط الكائن بتغيير محور واحد فقط مثل (1, 2, 1) لمضاعفة الارتفاع.
    auto scene3d_setObjectScale_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float sx = (float)args[1]->toDouble();
        float sy = (float)args[2]->toDouble();
        float sz = (float)args[3]->toDouble();
        sad::scene3d::setObjectScale(id, sx, sy, sz);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_مقياس_3D", scene3d_setObjectScale_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_scale", scene3d_setObjectScale_func);

    // ─── عيّن_لون_3D ───
    // تعيين لون كائن ثلاثي الأبعاد (RGBA). اللون يؤثر على كيفية ظهور
    // الكائن عند تطبيق الإضاءة عليه. المعامل الخامس (الشفافية) اختياري
    // وقيمته الافتراضية 255 (معتم بالكامل). قيم أقل تجعل الكائن شبه شفاف.
    auto scene3d_setObjectColor_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        uint8_t r = (uint8_t)args[1]->toDouble();
        uint8_t g = (uint8_t)args[2]->toDouble();
        uint8_t b = (uint8_t)args[3]->toDouble();
        uint8_t a = args.size() > 4 ? (uint8_t)args[4]->toDouble() : 255;
        sad::scene3d::setObjectColor(id, r, g, b, a);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_لون_3D", scene3d_setObjectColor_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_color", scene3d_setObjectColor_func);

    // ─── دوّر_كائن ───
    // إضافة دوران تدريجي لكائن ثلاثي الأبعاد (بالدرجات).
    // على عكس عيّن_دوران_3D الذي يحدد الدوران المطلق، هذه الدالة
    // تضيف الدوران المحدد للدوران الحالي. مثالية لتدوير الكائنات باستمرار
    // في كل إطار (مثل كوكب يدور حول نفسه أو عملة تتقلب).
    auto scene3d_rotateObject_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int id = (int)args[0]->toDouble();
        float dx = (float)args[1]->toDouble();
        float dy = (float)args[2]->toDouble();
        float dz = (float)args[3]->toDouble();
        sad::scene3d::rotateObject(id, dx, dy, dz);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دوّر_كائن", scene3d_rotateObject_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_rotate_object", scene3d_rotateObject_func);

    // ─── عيّن_كاميرا ───
    // تعيين موضع الكاميرا في الفضاء ثلاثي الأبعاد.
    // الكاميرا هي "العين" التي ننظر من خلالها للمشهد.
    // الموضع الافتراضي (0, 2, -5) — مرتفعة قليلاً ومبتعدة عن المركز.
    // غيّر z لتقريب/إبعاد الكاميرا، وy لرفعها/خفضها.
    auto scene3d_setCameraPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        sad::scene3d::setCameraPosition(x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_كاميرا", scene3d_setCameraPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_camera_position", scene3d_setCameraPosition_func);

    // ─── عيّن_هدف_كاميرا ───
    // تعيين النقطة التي تنظر إليها الكاميرا في الفضاء ثلاثي الأبعاد.
    // الكاميرا دائماً موجهة نحو هذه النقطة. الهدف الافتراضي (0, 0, 0) — المركز.
    // غيّر الهدف لتوجيه الكاميرا نحو كائن معين أو منطقة محددة.
    auto scene3d_setCameraTarget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        sad::scene3d::setCameraTarget(x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_هدف_كاميرا", scene3d_setCameraTarget_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_camera_target", scene3d_setCameraTarget_func);

    // ─── دوّر_كاميرا ───
    // تدوير الكاميرا حول هدفها (Orbit) — كأنك تدور حول كائن لرؤيته من كل الزوايا.
    // المعامل الأول: الزاوية الأفقية (يمين/يسار). الثاني: الزاوية الرأسية (أعلى/أسفل).
    // الثالث: المسافة من الهدف (القرب/البعد). مثالي للمعاينة ثلاثية الأبعاد.
    auto scene3d_orbitCamera_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float angleH = (float)args[0]->toDouble();
        float angleV = (float)args[1]->toDouble();
        float distance = (float)args[2]->toDouble();
        sad::scene3d::orbitCamera(angleH, angleV, distance);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دوّر_كاميرا", scene3d_orbitCamera_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_orbit_camera", scene3d_orbitCamera_func);

    // ─── عيّن_إضاءة ───
    // تعيين اتجاه الضوء الرئيسي (الاتجاهي) في المشهد ثلاثي الأبعاد.
    // الضوء الاتجاهي يشبه ضوء الشمس — يأتي من اتجاه واحد ويؤثر على
    // كل الكائنات بنفس الطريقة. الاتجاه الافتراضي (-0.5, -1, 0.5)
    // يمثل ضوءاً يأتي من الأعلى اليسار. غيّره لتغيير مظهر الإضاءة.
    auto scene3d_setLightDirection_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float z = (float)args[2]->toDouble();
        sad::scene3d::setLightDirection(x, y, z);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_إضاءة", scene3d_setLightDirection_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_light_direction", scene3d_setLightDirection_func);

    // ─── عيّن_إضاءة_محيطية ───
    // تعيين مستوى الإضاءة المحيطية (Ambient Light) في المشهد.
    // الإضاءة المحيطية هي الضوء الذي يصل لكل مكان بالتساوي بدون اتجاه محدد.
    // القيمة 0 = ظلام تام (فقط الضوء الاتجاهي يعمل).
    // القيمة 1 = إضاءة كاملة (لا ظلال — كل الوجوه مضاءة بالتساوي).
    // القيمة الافتراضية 0.3 تعطي توازناً جيداً بين الإضاءة والظلال.
    auto scene3d_setAmbientLight_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float amount = (float)args[0]->toDouble();
        sad::scene3d::setAmbientLight(amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_إضاءة_محيطية", scene3d_setAmbientLight_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_set_ambient_light", scene3d_setAmbientLight_func);

    // ─── ارسم_3D ───
    // رسم المشهد ثلاثي الأبعاد بالكامل على المُصيِّر (renderer).
    // هذه هي الدالة الرئيسية التي تُستدعى في كل إطار لعرض المشهد.
    // تقوم بتطبيق الإسقاط المنظوري والإضاءة وترتيب العمق ورسم كل الكائنات.
    // المعاملات: المُصيِّر، عرض الشاشة، ارتفاع الشاشة.
    auto scene3d_render_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int w = (int)args[1]->toDouble();
        int h = (int)args[2]->toDouble();
        sad::scene3d::render(renderer, w, h);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_3D", scene3d_render_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_render", scene3d_render_func);

    // ─── ارسم_شبكة ───
    // رسم شبكة أرضية (Grid) في المشهد ثلاثي الأبعاد — خطوط متقاطعة على
    // المستوى الأفقي تُساعد في تقدير المسافات والمواضع. مفيدة جداً أثناء
    // التطوير لرؤية "أرضية" المشهد. المعاملات: المُصيِّر، أبعاد الشاشة،
    // حجم الشبكة الكلي، وعدد التقسيمات (كلما زاد كلما كانت الشبكة أدق).
    auto scene3d_drawGrid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int w = (int)args[1]->toDouble();
        int h = (int)args[2]->toDouble();
        float size = (float)args[3]->toDouble();
        int divs = (int)args[4]->toDouble();
        sad::scene3d::drawGrid(renderer, w, h, size, divs);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_شبكة", scene3d_drawGrid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_draw_grid", scene3d_drawGrid_func);

    // ─── ارسم_محاور ───
    // رسم المحاور الإحداثية الثلاثة (X أحمر، Y أخضر، Z أزرق) في المشهد.
    // تُساعد في فهم اتجاهات المحاور والتنقل في الفضاء ثلاثي الأبعاد.
    // المعامل الرابع يحدد طول المحاور (بالوحدات ثلاثية الأبعاد).
    auto scene3d_drawAxes_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int w = (int)args[1]->toDouble();
        int h = (int)args[2]->toDouble();
        float length = (float)args[3]->toDouble();
        sad::scene3d::drawAxes(renderer, w, h, length);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_محاور", scene3d_drawAxes_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_draw_axes", scene3d_drawAxes_func);

    // ─── أعد_تعيين_المشهد ───
    // إعادة تعيين المشهد ثلاثي الأبعاد بالكامل — حذف كل الكائنات وإعادة
    // الكاميرا والإضاءة إلى قيمها الافتراضية. يُستخدم عند الانتقال
    // لمشهد جديد أو إعادة بناء المشهد من الصفر.
    auto scene3d_resetScene_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        sad::scene3d::resetScene();
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أعد_تعيين_المشهد", scene3d_resetScene_func);
    interpreter.getFunctionManager().registerBuiltinFunction("scene3d_reset_scene", scene3d_resetScene_func);

    // ═══════════════════════════════════════════════════════════════════════════
    // نظام الذكاء الاصطناعي للألعاب (Game AI System) — sad::ai
    // ═══════════════════════════════════════════════════════════════════════════
    // هذا النظام يقدم أدوات ذكاء اصطناعي متكاملة للألعاب تشمل:
    //   - إيجاد المسار (A* Pathfinding): إنشاء شبكة وتحديد المسار الأقصر
    //   - آلة الحالات المحدودة (FSM): إدارة حالات الكيانات والانتقالات بينها
    //   - سلوكيات التوجيه (Steering): مطاردة، هروب، تجوال، اقتراب
    //   - سلوك الأسراب (Flocking): محاكاة حركة مجموعات مثل الطيور والأسماك
    // كل هذه الميزات مدمجة بدون أي مكتبة خارجية!
    // ═══════════════════════════════════════════════════════════════════════════

    // ─── أنشئ_شبكة_مسار / ai_create_grid ───
    // إنشاء شبكة ثنائية الأبعاد لنظام إيجاد المسار (A* Pathfinding).
    // الشبكة هي أساس كل عمليات إيجاد المسار — كل خلية يمكن أن تكون
    // قابلة للمشي أو حاجزاً. المعاملات: العرض والارتفاع (عدد الخلايا).
    // تُرجع معرّف الشبكة لاستخدامه في الدوال الأخرى.
    auto ai_createGrid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int w = (int)args[0]->toDouble();
        int h = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::createGrid(w, h));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_شبكة_مسار", ai_createGrid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_grid", ai_createGrid_func);

    // ─── احذف_شبكة_مسار / ai_destroy_grid ───
    // حذف شبكة المسار وتحرير ذاكرتها. يجب استدعاء هذه الدالة عند الانتهاء
    // من استخدام الشبكة لتجنب تسرب الذاكرة. المعامل: معرّف الشبكة.
    auto ai_destroyGrid_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        sad::ai::destroyGrid(gridId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_شبكة_مسار", ai_destroyGrid_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_grid", ai_destroyGrid_func);

    // ─── عيّن_قابلية_المشي / ai_set_walkable ───
    // تعيين خلية معينة في الشبكة كقابلة للمشي أو كحاجز (جدار).
    // هذا يحدد أين يمكن للكيانات أن تمر وأين لا يمكنها.
    // المعاملات: معرّف الشبكة، إحداثيات الخلية (س، ص)، وقيمة منطقية (1=ممر، 0=حاجز).
    auto ai_setWalkable_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        bool walkable = args[3]->toDouble() != 0.0;
        sad::ai::setWalkable(gridId, x, y, walkable);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_قابلية_المشي", ai_setWalkable_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_walkable", ai_setWalkable_func);

    // ─── عيّن_تكلفة / ai_set_cost ───
    // تعيين تكلفة المرور عبر خلية معينة. الخلايا ذات التكلفة الأعلى
    // يتم تجنبها في المسار — مثلاً: الرمال أبطأ من الطريق المعبد.
    // المعاملات: معرّف الشبكة، إحداثيات الخلية (س، ص)، والتكلفة (عدد عشري).
    auto ai_setCost_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        int x = (int)args[1]->toDouble();
        int y = (int)args[2]->toDouble();
        float cost = (float)args[3]->toDouble();
        sad::ai::setCost(gridId, x, y, cost);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_تكلفة", ai_setCost_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_cost", ai_setCost_func);

    // ─── جد_مسار / ai_find_path ───
    // إيجاد أقصر مسار بين نقطتين على الشبكة باستخدام خوارزمية A*.
    // هذه هي الدالة الأساسية في نظام إيجاد المسار — تأخذ نقطة البداية
    // ونقطة النهاية وتُرجع معرّف المسار الذي يمكن قراءة نقاطه لاحقاً.
    // المعاملات: معرّف الشبكة، بداية (س، ص)، نهاية (س، ص).
    auto ai_findPath_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int gridId = (int)args[0]->toDouble();
        int sx = (int)args[1]->toDouble();
        int sy = (int)args[2]->toDouble();
        int ex = (int)args[3]->toDouble();
        int ey = (int)args[4]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::findPath(gridId, sx, sy, ex, ey));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("جد_مسار", ai_findPath_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_find_path", ai_findPath_func);

    // ─── طول_المسار / ai_get_path_length ───
    // الحصول على عدد النقاط في المسار المحسوب. يُستخدم للتكرار على
    // نقاط المسار واحدة تلو الأخرى (مع مسار_س ومسار_ص).
    // المعامل: معرّف المسار.
    auto ai_getPathLength_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getPathLength(pathId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طول_المسار", ai_getPathLength_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_path_length", ai_getPathLength_func);

#endif // HAS_GRAPHICS
}

} // namespace Interpreter
} // namespace Sad
