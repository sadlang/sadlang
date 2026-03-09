/**
 * @file builtin_registry_part5.cpp
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

void registerBuiltinsPart5(Interpreter& interpreter) {
#ifdef HAS_GRAPHICS
    // ─── مسار_س / ai_get_path_x ───
    // الحصول على الإحداثي الأفقي (س) لنقطة معينة في المسار.
    // المعاملات: معرّف المسار، ورقم النقطة (الفهرس).
    auto ai_getPathX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getPathX(pathId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مسار_س", ai_getPathX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_path_x", ai_getPathX_func);

    // ─── مسار_ص / ai_get_path_y ───
    // الحصول على الإحداثي العمودي (ص) لنقطة معينة في المسار.
    // المعاملات: معرّف المسار، ورقم النقطة (الفهرس).
    auto ai_getPathY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getPathY(pathId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("مسار_ص", ai_getPathY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_path_y", ai_getPathY_func);

    // ─── هل_وجد_مسار / ai_is_path_found ───
    // التحقق من نجاح عملية إيجاد المسار. تُرجع 1 إذا تم إيجاد مسار
    // و0 إذا لم يكن هناك مسار ممكن (مثلاً: الهدف محاط بحواجز).
    // المعامل: معرّف المسار.
    auto ai_isPathFound_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>(sad::ai::isPathFound(pathId) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_وجد_مسار", ai_isPathFound_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_is_path_found", ai_isPathFound_func);

    // ─── احذف_مسار / ai_destroy_path ───
    // حذف المسار المحسوب وتحرير ذاكرته. يجب استدعاء هذه الدالة بعد
    // الانتهاء من استخدام المسار لتجنب تسرب الذاكرة.
    // المعامل: معرّف المسار.
    auto ai_destroyPath_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int pathId = (int)args[0]->toDouble();
        sad::ai::destroyPath(pathId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_مسار", ai_destroyPath_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_path", ai_destroyPath_func);

    // ─────────────────────────────────────────────────────────────────────
    // آلة الحالات المحدودة (Finite State Machine - FSM)
    // ─────────────────────────────────────────────────────────────────────
    // آلة الحالات تُستخدم لإدارة سلوك الكيانات في اللعبة. كل كيان
    // (عدو، NPC، إلخ) يكون في "حالة" واحدة في كل لحظة (مثل: خمول،
    // مطاردة، هجوم، هروب). الانتقال بين الحالات يحدث من خلال "محفزات".
    // ─────────────────────────────────────────────────────────────────────

    // ─── أنشئ_آلة_حالات / ai_create_fsm ───
    // إنشاء آلة حالات محدودة جديدة. تُرجع معرّف الآلة لاستخدامه
    // في إضافة الحالات والانتقالات وإطلاق المحفزات.
    auto ai_createFSM_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return std::make_shared<Data::Value>((double)sad::ai::createFSM());
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_آلة_حالات", ai_createFSM_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_fsm", ai_createFSM_func);

    // ─── احذف_آلة_حالات / ai_destroy_fsm ───
    // حذف آلة الحالات وتحرير ذاكرتها. المعامل: معرّف الآلة.
    auto ai_destroyFSM_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        sad::ai::destroyFSM(fsmId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_آلة_حالات", ai_destroyFSM_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_fsm", ai_destroyFSM_func);

    // ─── أضف_حالة / ai_add_state ───
    // إضافة حالة جديدة إلى آلة الحالات. كل حالة لها معرّف رقمي
    // واسم نصي (مثلاً: "خمول"، "مطاردة"، "هجوم").
    // المعاملات: معرّف الآلة، معرّف الحالة (رقم)، اسم الحالة (نص).
    auto ai_addState_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        int stateId = (int)args[1]->toDouble();
        std::string name = args[2]->toString();
        sad::ai::addState(fsmId, stateId, name);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_حالة", ai_addState_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_add_state", ai_addState_func);

    // ─── عيّن_حالة_أولية / ai_set_initial_state ───
    // تعيين الحالة التي تبدأ منها آلة الحالات. يجب استدعاء هذه الدالة
    // بعد إضافة الحالات وقبل البدء في إطلاق المحفزات.
    // المعاملات: معرّف الآلة، معرّف الحالة الأولية.
    auto ai_setInitialState_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        int stateId = (int)args[1]->toDouble();
        sad::ai::setInitialState(fsmId, stateId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_حالة_أولية", ai_setInitialState_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_initial_state", ai_setInitialState_func);

    // ─── أضف_انتقال / ai_add_transition ───
    // إضافة انتقال بين حالتين. عند إطلاق المحفز المحدد، إذا كانت الآلة
    // في الحالة "من"، تنتقل تلقائياً إلى الحالة "إلى".
    // المعاملات: معرّف الآلة، حالة المصدر، حالة الوجهة، اسم المحفز.
    auto ai_addTransition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        int from = (int)args[1]->toDouble();
        int to = (int)args[2]->toDouble();
        std::string trigger = args[3]->toString();
        sad::ai::addTransition(fsmId, from, to, trigger);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أضف_انتقال", ai_addTransition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_add_transition", ai_addTransition_func);

    // ─── أطلق_محفز / ai_fire_trigger ───
    // إطلاق محفز (trigger) في آلة الحالات. إذا كان هناك انتقال معرّف
    // من الحالة الحالية بهذا المحفز، يتم الانتقال وتُرجع 1 (نجاح).
    // إذا لم يكن هناك انتقال مناسب، تُرجع 0 (فشل).
    // المعاملات: معرّف الآلة، اسم المحفز.
    auto ai_fireTrigger_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        std::string trigger = args[1]->toString();
        return std::make_shared<Data::Value>(sad::ai::fireTrigger(fsmId, trigger) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أطلق_محفز", ai_fireTrigger_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_fire_trigger", ai_fireTrigger_func);

    // ─── الحالة_الحالية / ai_get_current_state ───
    // الحصول على المعرّف الرقمي للحالة الحالية في آلة الحالات.
    // مفيد للتحقق من حالة الكيان برمجياً (مثلاً: إذا كان في حالة الهجوم).
    // المعامل: معرّف الآلة.
    auto ai_getCurrentState_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getCurrentState(fsmId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("الحالة_الحالية", ai_getCurrentState_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_current_state", ai_getCurrentState_func);

    // ─── اسم_الحالة_الحالية / ai_get_current_state_name ───
    // الحصول على الاسم النصي للحالة الحالية (مثلاً: "مطاردة").
    // مفيد لعرض حالة الكيان على الشاشة أو لأغراض التصحيح.
    // المعامل: معرّف الآلة.
    auto ai_getCurrentStateName_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int fsmId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>(sad::ai::getCurrentStateName(fsmId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اسم_الحالة_الحالية", ai_getCurrentStateName_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_current_state_name", ai_getCurrentStateName_func);

    // ─────────────────────────────────────────────────────────────────────
    // سلوكيات التوجيه (Steering Behaviors)
    // ─────────────────────────────────────────────────────────────────────
    // سلوكيات التوجيه تُحرك الكيانات (أعداء، NPCs) بطريقة طبيعية وذكية.
    // كل كيان له موضع وسرعة وسرعة قصوى، ويمكنه تنفيذ سلوكيات مختلفة:
    //   - المطاردة (Seek): التوجه نحو هدف بأقصى سرعة
    //   - الهروب (Flee): الابتعاد عن تهديد بأقصى سرعة
    //   - التجوال (Wander): حركة عشوائية طبيعية
    //   - الاقتراب (Arrive): مطاردة مع تباطؤ تدريجي عند الاقتراب
    // ─────────────────────────────────────────────────────────────────────

    // ─── أنشئ_كيان / ai_create_agent ───
    // إنشاء كيان ذكي جديد (عدو، NPC، وحش، إلخ) في الموضع المحدد
    // مع سرعة قصوى. الكيان يمكنه تنفيذ سلوكيات التوجيه المختلفة.
    // المعاملات: الموضع الأفقي (س)، الموضع العمودي (ص)، السرعة القصوى.
    // تُرجع معرّف الكيان.
    auto ai_createAgent_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        float x = (float)args[0]->toDouble();
        float y = (float)args[1]->toDouble();
        float maxSpeed = (float)args[2]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::createAgent(x, y, maxSpeed));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_كيان", ai_createAgent_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_agent", ai_createAgent_func);

    // ─── احذف_كيان / ai_destroy_agent ───
    // حذف كيان وتحرير ذاكرته. المعامل: معرّف الكيان.
    auto ai_destroyAgent_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        sad::ai::destroyAgent(agentId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كيان", ai_destroyAgent_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_agent", ai_destroyAgent_func);

    // ─── عيّن_موضع_كيان / ai_set_agent_position ───
    // تعيين موضع الكيان يدوياً. مفيد لنقل الكيان فوراً إلى موضع جديد
    // (مثلاً: عند ظهوره لأول مرة أو إعادة تعيينه).
    // المعاملات: معرّف الكيان، الموضع الأفقي (س)، الموضع العمودي (ص).
    auto ai_setAgentPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::ai::setAgentPosition(agentId, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_كيان", ai_setAgentPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_agent_position", ai_setAgentPosition_func);

    // ─── موضع_كيان_س / ai_get_agent_x ───
    // الحصول على الموضع الأفقي (س) للكيان. مفيد لرسم الكيان أو
    // حساب المسافات. المعامل: معرّف الكيان.
    auto ai_getAgentX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getAgentX(agentId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كيان_س", ai_getAgentX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_agent_x", ai_getAgentX_func);

    // ─── موضع_كيان_ص / ai_get_agent_y ───
    // الحصول على الموضع العمودي (ص) للكيان. مفيد لرسم الكيان أو
    // حساب المسافات. المعامل: معرّف الكيان.
    auto ai_getAgentY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getAgentY(agentId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كيان_ص", ai_getAgentY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_agent_y", ai_getAgentY_func);

    // ─── طارد / ai_seek ───
    // سلوك المطاردة — يجعل الكيان يتجه نحو نقطة الهدف بأقصى سرعة.
    // هذا أبسط سلوك توجيه: الكيان يتحرك مباشرةً نحو الهدف بدون تباطؤ.
    // المعاملات: معرّف الكيان، موضع الهدف (س، ص)، الزمن المنقضي (dt).
    auto ai_seek_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float dt = (float)args[3]->toDouble();
        sad::ai::seek(agentId, tx, ty, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("طارد", ai_seek_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_seek", ai_seek_func);

    // ─── اهرب / ai_flee ───
    // سلوك الهروب — يجعل الكيان يبتعد عن نقطة التهديد بأقصى سرعة.
    // عكس المطاردة تماماً — مفيد لسلوك الخوف أو التراجع.
    // المعاملات: معرّف الكيان، موضع التهديد (س، ص)، الزمن المنقضي (dt).
    auto ai_flee_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float dt = (float)args[3]->toDouble();
        sad::ai::flee(agentId, tx, ty, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اهرب", ai_flee_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_flee", ai_flee_func);

    // ─── تجوّل / ai_wander ───
    // سلوك التجوال العشوائي — يجعل الكيان يتحرك بحركة عشوائية طبيعية
    // تشبه حركة الحيوانات في الطبيعة. لا يحتاج هدفاً محدداً.
    // المعاملات: معرّف الكيان، الزمن المنقضي (dt).
    auto ai_wander_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float dt = (float)args[1]->toDouble();
        sad::ai::wander(agentId, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تجوّل", ai_wander_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_wander", ai_wander_func);

    // ─── اقترب / ai_arrive ───
    // سلوك الاقتراب — مثل المطاردة ولكن مع تباطؤ تدريجي عند الاقتراب
    // من الهدف. يمنع الكيان من التجاوز ذهاباً وإياباً حول الهدف.
    // نصف قطر التباطؤ (slowRadius) يحدد متى يبدأ التباطؤ.
    // المعاملات: معرّف الكيان، موضع الهدف (س، ص)، نصف قطر التباطؤ، الزمن (dt).
    auto ai_arrive_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float slowR = (float)args[3]->toDouble();
        float dt = (float)args[4]->toDouble();
        sad::ai::arrive(agentId, tx, ty, slowR, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("اقترب", ai_arrive_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_arrive", ai_arrive_func);

    // ─── هل_يرى / ai_can_see ───
    // التحقق هل الكيان يمكنه رؤية النقطة المحددة (ضمن مدى رؤيته).
    // تُرجع 1 إذا كانت النقطة ضمن مدى الرؤية، و0 إذا كانت خارجه.
    // مفيد لتحديد متى يبدأ العدو في مطاردة اللاعب.
    // المعاملات: معرّف الكيان، موضع الهدف (س، ص).
    auto ai_canSee_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int agentId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        return std::make_shared<Data::Value>(sad::ai::canSee(agentId, tx, ty) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_يرى", ai_canSee_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_can_see", ai_canSee_func);

    // ─────────────────────────────────────────────────────────────────────
    // سلوك الأسراب (Flocking)
    // ─────────────────────────────────────────────────────────────────────
    // محاكاة حركة مجموعة من الكيانات (مثل سرب طيور أو مجموعة أسماك).
    // يعتمد على ثلاث قواعد بسيطة:
    //   1. الفصل (Separation): تجنب التصادم مع الجيران
    //   2. المحاذاة (Alignment): محاذاة الاتجاه مع الجيران
    //   3. التماسك (Cohesion): الانجذاب نحو مركز المجموعة
    // ─────────────────────────────────────────────────────────────────────

    // ─── أنشئ_سرب / ai_create_flock ───
    // إنشاء سرب جديد من الكيانات موزعين عشوائياً في المنطقة المحددة.
    // المعاملات: عدد أفراد السرب، منطقة التوزيع (س، ص، عرض، ارتفاع).
    // تُرجع معرّف السرب.
    auto ai_createFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int count = (int)args[0]->toDouble();
        float ax = (float)args[1]->toDouble();
        float ay = (float)args[2]->toDouble();
        float aw = (float)args[3]->toDouble();
        float ah = (float)args[4]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::createFlock(count, ax, ay, aw, ah));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_سرب", ai_createFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_create_flock", ai_createFlock_func);

    // ─── احذف_سرب / ai_destroy_flock ───
    // حذف السرب وتحرير ذاكرته. المعامل: معرّف السرب.
    auto ai_destroyFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        sad::ai::destroyFlock(flockId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_سرب", ai_destroyFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_destroy_flock", ai_destroyFlock_func);

    // ─── حدّث_سرب / ai_update_flock ───
    // تحديث حركة جميع أفراد السرب — يُطبق قواعد الفصل والمحاذاة والتماسك.
    // يجب استدعاء هذه الدالة في كل إطار (frame) لتحريك السرب.
    // المعاملات: معرّف السرب، الزمن المنقضي (dt).
    auto ai_updateFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        float dt = (float)args[1]->toDouble();
        sad::ai::updateFlock(flockId, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدّث_سرب", ai_updateFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_update_flock", ai_updateFlock_func);

    // ─── موضع_عنصر_سرب_س / ai_get_flock_member_x ───
    // الحصول على الموضع الأفقي (س) لعنصر معين في السرب.
    // المعاملات: معرّف السرب، رقم العنصر (الفهرس يبدأ من 0).
    auto ai_getFlockMemberX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getFlockMemberX(flockId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_عنصر_سرب_س", ai_getFlockMemberX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_flock_member_x", ai_getFlockMemberX_func);

    // ─── موضع_عنصر_سرب_ص / ai_get_flock_member_y ───
    // الحصول على الموضع العمودي (ص) لعنصر معين في السرب.
    // المعاملات: معرّف السرب، رقم العنصر (الفهرس يبدأ من 0).
    auto ai_getFlockMemberY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        int index = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getFlockMemberY(flockId, index));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_عنصر_سرب_ص", ai_getFlockMemberY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_flock_member_y", ai_getFlockMemberY_func);

    // ─── حجم_السرب / ai_get_flock_size ───
    // الحصول على عدد أفراد السرب. مفيد للتكرار على جميع الأفراد لرسمهم.
    // المعامل: معرّف السرب.
    auto ai_getFlockSize_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::ai::getFlockSize(flockId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حجم_السرب", ai_getFlockSize_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_get_flock_size", ai_getFlockSize_func);

    // ─── عيّن_هدف_سرب / ai_set_flock_target ───
    // تعيين نقطة هدف للسرب — يتحرك أفراد السرب تدريجياً نحو هذه النقطة
    // مع الحفاظ على سلوك السرب (الفصل والمحاذاة والتماسك).
    // المعاملات: معرّف السرب، موضع الهدف (س، ص).
    auto ai_setFlockTarget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int flockId = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::ai::setFlockTarget(flockId, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_هدف_سرب", ai_setFlockTarget_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_set_flock_target", ai_setFlockTarget_func);

    // ─── ارسم_سرب / ai_render_flock ───
    // رسم جميع أفراد السرب كنقاط ملونة على الشاشة. دالة مساعدة سريعة
    // لعرض السرب بدون كتابة حلقة رسم يدوية.
    // المعاملات: المُصيِّر (renderer)، معرّف السرب، اللون (أحمر، أخضر، أزرق).
    auto ai_renderFlock_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        void* renderer = (void*)(uintptr_t)(uint64_t)args[0]->toDouble();
        int flockId = (int)args[1]->toDouble();
        uint8_t r = (uint8_t)(int)args[2]->toDouble();
        uint8_t g = (uint8_t)(int)args[3]->toDouble();
        uint8_t b = (uint8_t)(int)args[4]->toDouble();
        sad::ai::renderFlock(renderer, flockId, r, g, b);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ارسم_سرب", ai_renderFlock_func);
    interpreter.getFunctionManager().registerBuiltinFunction("ai_render_flock", ai_renderFlock_func);

    // ═══════════════════════════════════════════════════════════════════════════
    // نظام الكاميرا ثنائية الأبعاد (2D Camera System) — sad::camera2d
    // ═══════════════════════════════════════════════════════════════════════════
    // نظام كاميرا متقدم يوفر تحكماً كاملاً بنافذة العرض في الألعاب ثنائية الأبعاد:
    //   - الموضع والتحريك: تعيين موضع الكاميرا أو تحريكها نسبياً
    //   - التكبير والتصغير (Zoom): مع حدود دنيا وقصوى
    //   - الدوران: تدوير الكاميرا حول مركزها
    //   - متابعة الهدف (Follow): مع تنعيم ومنطقة ميتة ونظر أمامي
    //   - حدود الكاميرا (Bounds): لمنع الخروج عن الخريطة
    //   - الاهتزاز (Screen Shake): تأثيرات اهتزاز بقوة ومدة محددة
    //   - الحركة السينمائية (Pan): تحريك سلس بين نقطتين
    //   - تحويل الإحداثيات: من العالم للشاشة والعكس
    // ═══════════════════════════════════════════════════════════════════════════

    // ─── أنشئ_كاميرا / cam2d_create ───
    // إنشاء كاميرا ثنائية أبعاد جديدة بأبعاد نافذة العرض المحددة.
    // عادةً تكون الأبعاد مساوية لحجم الشاشة (مثلاً: 800×600).
    // المعاملات: عرض نافذة العرض، ارتفاع نافذة العرض.
    // تُرجع معرّف الكاميرا لاستخدامه في الدوال الأخرى.
    auto cam2d_create_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int w = (int)args[0]->toDouble();
        int h = (int)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::createCamera(w, h));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أنشئ_كاميرا", cam2d_create_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_create", cam2d_create_func);

    // ─── احذف_كاميرا / cam2d_destroy ───
    // حذف الكاميرا وتحرير ذاكرتها. المعامل: معرّف الكاميرا.
    auto cam2d_destroy_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::destroyCamera(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("احذف_كاميرا", cam2d_destroy_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_destroy", cam2d_destroy_func);

    // ─── عيّن_موضع_كاميرا_2D / cam2d_set_position ───
    // تعيين موضع مركز الكاميرا مباشرةً. الكاميرا تعرض المنطقة المحيطة
    // بهذا الموضع. مفيد لنقل الكاميرا فوراً (بدون تنعيم).
    // المعاملات: معرّف الكاميرا، الموضع الأفقي (س)، الموضع العمودي (ص).
    auto cam2d_setPosition_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float x = (float)args[1]->toDouble();
        float y = (float)args[2]->toDouble();
        sad::camera2d::setCamPosition(camId, x, y);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_موضع_كاميرا_2D", cam2d_setPosition_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_position", cam2d_setPosition_func);

    // ─── موضع_كاميرا_س / cam2d_get_x ───
    // الحصول على الموضع الأفقي (س) لمركز الكاميرا.
    // المعامل: معرّف الكاميرا.
    auto cam2d_getX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamX(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كاميرا_س", cam2d_getX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_x", cam2d_getX_func);

    // ─── موضع_كاميرا_ص / cam2d_get_y ───
    // الحصول على الموضع العمودي (ص) لمركز الكاميرا.
    // المعامل: معرّف الكاميرا.
    auto cam2d_getY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamY(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("موضع_كاميرا_ص", cam2d_getY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_y", cam2d_getY_func);

    // ─── حرّك_كاميرا / cam2d_move ───
    // تحريك الكاميرا بمقدار نسبي من موضعها الحالي.
    // المعاملات: معرّف الكاميرا، الإزاحة الأفقية (dx)، الإزاحة العمودية (dy).
    auto cam2d_move_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float dx = (float)args[1]->toDouble();
        float dy = (float)args[2]->toDouble();
        sad::camera2d::moveCam(camId, dx, dy);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حرّك_كاميرا", cam2d_move_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_move", cam2d_move_func);

    // ─── عيّن_تكبير / cam2d_set_zoom ───
    // تعيين مستوى التكبير. القيمة 1.0 تعني العرض الطبيعي، 2.0 تعني
    // تكبير مضاعف (كل شيء يبدو أكبر)، 0.5 تعني تصغير (كل شيء أصغر).
    // المعاملات: معرّف الكاميرا، مستوى التكبير.
    auto cam2d_setZoom_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float zoom = (float)args[1]->toDouble();
        sad::camera2d::setCamZoom(camId, zoom);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_تكبير", cam2d_setZoom_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_zoom", cam2d_setZoom_func);

    // ─── تكبير_الكاميرا / cam2d_get_zoom ───
    // الحصول على مستوى التكبير الحالي للكاميرا. المعامل: معرّف الكاميرا.
    auto cam2d_getZoom_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamZoom(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تكبير_الكاميرا", cam2d_getZoom_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_zoom", cam2d_getZoom_func);

    // ─── حدود_تكبير / cam2d_set_zoom_limits ───
    // تعيين الحدود الدنيا والقصوى للتكبير. يمنع المستخدم من التكبير
    // أكثر من اللازم أو التصغير بشكل مفرط.
    // المعاملات: معرّف الكاميرا، الحد الأدنى، الحد الأقصى.
    auto cam2d_setZoomLimits_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float minZ = (float)args[1]->toDouble();
        float maxZ = (float)args[2]->toDouble();
        sad::camera2d::setCamZoomLimits(camId, minZ, maxZ);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدود_تكبير", cam2d_setZoomLimits_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_zoom_limits", cam2d_setZoomLimits_func);

    // ─── عيّن_دوران_كاميرا / cam2d_set_rotation ───
    // تعيين زاوية دوران الكاميرا (بالدرجات). الدوران يكون حول مركز الكاميرا.
    // مفيد لتأثيرات مثل ميلان الشاشة عند الضرر أو الانعطاف.
    // المعاملات: معرّف الكاميرا، الزاوية (بالدرجات).
    auto cam2d_setRotation_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float degrees = (float)args[1]->toDouble();
        sad::camera2d::setCamRotation(camId, degrees);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عيّن_دوران_كاميرا", cam2d_setRotation_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_rotation", cam2d_setRotation_func);

    // ─── دوران_الكاميرا / cam2d_get_rotation ───
    // الحصول على زاوية الدوران الحالية للكاميرا (بالدرجات).
    // المعامل: معرّف الكاميرا.
    auto cam2d_getRotation_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::getCamRotation(camId));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دوران_الكاميرا", cam2d_getRotation_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_get_rotation", cam2d_getRotation_func);

    // ─── تابع_هدف / cam2d_follow_target ───
    // جعل الكاميرا تتبع نقطة هدف تلقائياً مع تنعيم (smoothing).
    // قيمة التنعيم بين 0 و1: قريبة من 0 = بطيئة وسلسة، قريبة من 1 = سريعة.
    // يُستدعى كل إطار مع موضع اللاعب الحالي.
    // المعاملات: معرّف الكاميرا، موضع الهدف (س، ص)، معامل التنعيم.
    auto cam2d_followTarget_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float smoothing = (float)args[3]->toDouble();
        sad::camera2d::followTarget(camId, tx, ty, smoothing);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("تابع_هدف", cam2d_followTarget_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_follow_target", cam2d_followTarget_func);

    // ─── منطقة_ميتة / cam2d_set_dead_zone ───
    // تعيين المنطقة الميتة (Dead Zone) في وسط الشاشة. عندما يتحرك الهدف
    // داخل هذه المنطقة، الكاميرا لا تتحرك — فقط عند خروجه منها.
    // يجعل حركة الكاميرا أقل اضطراباً مع الحركات الصغيرة.
    // المعاملات: معرّف الكاميرا، عرض المنطقة، ارتفاع المنطقة.
    auto cam2d_setDeadZone_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float w = (float)args[1]->toDouble();
        float h = (float)args[2]->toDouble();
        sad::camera2d::setDeadZone(camId, w, h);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("منطقة_ميتة", cam2d_setDeadZone_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_dead_zone", cam2d_setDeadZone_func);

    // ─── نظر_أمامي / cam2d_set_look_ahead ───
    // تعيين مقدار النظر الأمامي — الكاميرا تنظر قليلاً في اتجاه حركة
    // الهدف لإعطاء اللاعب رؤية أفضل لما أمامه.
    // المعاملات: معرّف الكاميرا، مقدار النظر الأمامي (بالبكسل).
    auto cam2d_setLookAhead_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float amount = (float)args[1]->toDouble();
        sad::camera2d::setLookAhead(camId, amount);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("نظر_أمامي", cam2d_setLookAhead_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_look_ahead", cam2d_setLookAhead_func);

    // ─── حدود_كاميرا / cam2d_set_bounds ───
    // تعيين حدود الكاميرا — تمنع الكاميرا من عرض مناطق خارج الخريطة.
    // الحدود تُعرَّف بالإحداثيات: يسار، أعلى، يمين، أسفل.
    // المعاملات: معرّف الكاميرا، الحد الأيسر، العلوي، الأيمن، السفلي.
    auto cam2d_setBounds_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float l = (float)args[1]->toDouble();
        float t = (float)args[2]->toDouble();
        float r = (float)args[3]->toDouble();
        float b = (float)args[4]->toDouble();
        sad::camera2d::setCamBounds(camId, l, t, r, b);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدود_كاميرا", cam2d_setBounds_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_set_bounds", cam2d_setBounds_func);

    // ─── ألغ_حدود_كاميرا / cam2d_clear_bounds ───
    // إلغاء حدود الكاميرا — السماح لها بالتحرك بحرية بدون قيود.
    // المعامل: معرّف الكاميرا.
    auto cam2d_clearBounds_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::clearCamBounds(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("ألغ_حدود_كاميرا", cam2d_clearBounds_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_clear_bounds", cam2d_clearBounds_func);

    // ─── هزّ_الكاميرا / cam2d_shake ───
    // بدء تأثير اهتزاز الشاشة — يُستخدم عند الضربات القوية والانفجارات.
    // الاهتزاز يضعف تدريجياً خلال المدة المحددة.
    // المعاملات: معرّف الكاميرا، المدة (بالثواني)، الشدة (بالبكسل).
    auto cam2d_shake_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float duration = (float)args[1]->toDouble();
        float intensity = (float)args[2]->toDouble();
        sad::camera2d::shakeCamera(camId, duration, intensity);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هزّ_الكاميرا", cam2d_shake_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_shake", cam2d_shake_func);

    // ─── أوقف_الاهتزاز / cam2d_stop_shake ───
    // إيقاف الاهتزاز فوراً (بدون انتظار انتهاء المدة).
    // المعامل: معرّف الكاميرا.
    auto cam2d_stopShake_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        sad::camera2d::stopShake(camId);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أوقف_الاهتزاز", cam2d_stopShake_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_stop_shake", cam2d_stopShake_func);

    // ─── حرّك_نحو / cam2d_pan_to ───
    // تحريك الكاميرا بسلاسة من موضعها الحالي إلى نقطة الهدف خلال
    // المدة المحددة. مفيد للمشاهد السينمائية وعرض مناطق الخريطة.
    // المعاملات: معرّف الكاميرا، موضع الهدف (س، ص)، المدة (بالثواني).
    auto cam2d_panTo_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float tx = (float)args[1]->toDouble();
        float ty = (float)args[2]->toDouble();
        float duration = (float)args[3]->toDouble();
        sad::camera2d::panTo(camId, tx, ty, duration);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حرّك_نحو", cam2d_panTo_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_pan_to", cam2d_panTo_func);

    // ─── هل_تتحرك / cam2d_is_panning ───
    // التحقق هل الكاميرا في حالة حركة سينمائية (pan) حالياً.
    // تُرجع 1 إذا كانت تتحرك، و0 إذا انتهت الحركة.
    // المعامل: معرّف الكاميرا.
    auto cam2d_isPanning_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        return std::make_shared<Data::Value>(sad::camera2d::isPanning(camId) ? 1.0 : 0.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("هل_تتحرك", cam2d_isPanning_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_is_panning", cam2d_isPanning_func);

    // ─── حدّث_كاميرا / cam2d_update ───
    // تحديث الكاميرا — يجب استدعاء هذه الدالة في كل إطار.
    // تُعالج: متابعة الهدف، الاهتزاز، الحركة السينمائية، تطبيق الحدود.
    // المعاملات: معرّف الكاميرا، الزمن المنقضي (dt).
    auto cam2d_update_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float dt = (float)args[1]->toDouble();
        sad::camera2d::updateCamera(camId, dt);
        return std::make_shared<Data::Value>(1.0);
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حدّث_كاميرا", cam2d_update_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_update", cam2d_update_func);

    // ─── عالم_إلى_شاشة_س / cam2d_world_to_screen_x ───
    // تحويل إحداثي أفقي من نظام العالم إلى نظام الشاشة.
    // مفيد لمعرفة أين سيظهر كائن معين على الشاشة بعد تطبيق الكاميرا.
    // المعاملات: معرّف الكاميرا، الإحداثي الأفقي في العالم.
    auto cam2d_worldToScreenX_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float wx = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::worldToScreenX(camId, wx));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عالم_إلى_شاشة_س", cam2d_worldToScreenX_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_world_to_screen_x", cam2d_worldToScreenX_func);

    // ─── عالم_إلى_شاشة_ص / cam2d_world_to_screen_y ───
    // تحويل إحداثي عمودي من نظام العالم إلى نظام الشاشة.
    // المعاملات: معرّف الكاميرا، الإحداثي العمودي في العالم.
    auto cam2d_worldToScreenY_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        int camId = (int)args[0]->toDouble();
        float wy = (float)args[1]->toDouble();
        return std::make_shared<Data::Value>((double)sad::camera2d::worldToScreenY(camId, wy));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("عالم_إلى_شاشة_ص", cam2d_worldToScreenY_func);
    interpreter.getFunctionManager().registerBuiltinFunction("cam2d_world_to_screen_y", cam2d_worldToScreenY_func);

#endif // HAS_GRAPHICS
}

} // namespace Interpreter
} // namespace Sad
