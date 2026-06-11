/**
 * @file ui_state_builtins.cpp
 * @brief (AR) دوال إدارة الحالة التفاعلية — عرّف / عيّن / اقرأ / راقب / تجميع
 * @brief (EN) Reactive state management builtins — define / set / get / observe / batch
 *
 * نظام الحالة يربط StateStore (من sad_ui/core) بالمفسر عبر UIBridge.
 * جميع الأسماء عربية فقط — بحسب اختيار المستخدم.
 *
 * الدوال المتوفرة:
 *   عرّف_حالة(اسم, قيمة)         — تعريف متغير حالة جديد
 *   عيّن_حالة(اسم, قيمة)         — تعيين قيمة (تعريف تلقائي إذا لم توجد)
 *   اقرأ_حالة(اسم)              — قراءة قيمة الحالة
 *   هل_توجد_حالة(اسم)           — فحص وجود حالة
 *   ابدأ_تجميع()               — بدء تجميع تحديثات (Batch)
 *   أنهِ_تجميع()               — إنهاء التجميع وإعادة البناء
 *   عرّف_محسوب(اسم, اعتمادات, دالة) — حالة محسوبة تلقائياً
 *   راقب_حالة(اسم, دالة)        — مراقبة تغيّرات حالة معينة
 *   أفرغ_الحالة()              — طباعة محتويات المخزن (تصحيح)
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "sad_ui/state.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>
#include "builtins/builtin_context.h"

namespace Sad {
namespace Interpreter {

// ═══════════════════════════════════════════════════════════════════════════════
// مراقب حالة يربط StateObserver بدالة لغة ص
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief مراقب يغلّف دالة مستخدم (لغة ص) لتُستدعى عند تغيّر الحالة
 *
 * عندما يتغير متغير حالة مُراقَب، يستدعي هذا المراقب
 * دالة المستخدم مع تمرير اسم الحالة والقيمة الجديدة كمعاملات.
 */
class SadFunctionObserver : public sad::ui::StateObserver {
public:
    SadFunctionObserver(Interpreter* interp, const std::string& funcName)
        : interpreter_(interp), funcName_(funcName) {}

    void onStateChanged(const std::string& stateName,
                         const std::any& newValue) override {
        if (!interpreter_) return;

        try {
            std::vector<Data::Value> args;
            args.push_back(Data::Value(stateName));

            // محاولة استخراج Data::Value من std::any
            try {
                auto val = std::any_cast<Data::Value>(newValue);
                args.push_back(val);
            } catch (const std::bad_any_cast&) {
                // إذا لم تكن Data::Value، نمرر لاشيء
                args.push_back(Data::Value());
            }

            interpreter_->callUserFunction(funcName_, args);
        } catch (const std::exception& e) {
            std::cerr << "خطأ في مراقب الحالة '" << stateName
                      << "': " << e.what() << std::endl;
        }
    }

private:
    Interpreter* interpreter_;
    std::string funcName_;
};

// ═══════════════════════════════════════════════════════════════════════════════
// تسجيل دوال الحالة
// ═══════════════════════════════════════════════════════════════════════════════

void registerUIStateBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // دالة مساعدة لإنشاء قائمة معاملات
    using FP = Data::FunctionParameter;

    // ─── عرّف_حالة(اسم, قيمة_أولية) ──────────────────────────────
    // تعريف متغير حالة جديد بقيمة أولية
    auto define_state_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2 || !args[0]->isString()) {
            throw std::runtime_error(
                "\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9() "
                "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xa7\xd8\xb3\xd9\x85 "
                "(\xd9\x86\xd8\xb5) \xd9\x88\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9 "
                "\xd8\xa3\xd9\x88\xd9\x84\xd9\x8a\xd8\xa9");
            // عرّف_حالة() يحتاج اسم (نص) وقيمة أولية
        }

        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>();

        const auto& name = args[0]->toString();
        auto& store = bridge->getStateStore();

        // تحديد نوع القيمة
        std::string valueType = "\xd8\xa3\xd9\x8a"; // أي
        if (args[1]->isInteger())   valueType = "\xd8\xb1\xd9\x82\xd9\x85"; // رقم
        else if (args[1]->isDouble())  valueType = "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a"; // عشري
        else if (args[1]->isString())  valueType = "\xd9\x86\xd8\xb5"; // نص
        else if (args[1]->isBoolean()) valueType = "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a"; // منطقي

        store.define(name, valueType, sad::ui::StateBindingType::State,
                     std::any(*args[1]));

        return std::make_shared<Data::Value>();
    };
    fm.defineBuiltInFunction(
        "\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85"), FP("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9")},
        define_state_fn); // عرّف_حالة
    fm.defineBuiltInFunction(
        "\xd8\xb9\xd8\xb1\xd9\x81_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85"), FP("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9")},
        define_state_fn); // عرف_حالة (بدون شدة)

    // ─── عيّن_حالة(اسم, قيمة) ──────────────────────────────────
    // تعيين قيمة حالة (تعريف تلقائي إذا لم تكن موجودة)
    auto set_state_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2 || !args[0]->isString()) {
            throw std::runtime_error(
                "\xd8\xb9\xd9\x8a\xd9\x91\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9() "
                "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xa7\xd8\xb3\xd9\x85 "
                "(\xd9\x86\xd8\xb5) \xd9\x88\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
            // عيّن_حالة() يحتاج اسم (نص) وقيمة
        }

        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>();

        const auto& name = args[0]->toString();
        auto& store = bridge->getStateStore();

        if (!store.has(name)) {
            // تعريف تلقائي إذا لم تكن معرّفة
            std::string valueType = "\xd8\xa3\xd9\x8a"; // أي
            if (args[1]->isInteger())   valueType = "\xd8\xb1\xd9\x82\xd9\x85";
            else if (args[1]->isDouble())  valueType = "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a";
            else if (args[1]->isString())  valueType = "\xd9\x86\xd8\xb5";
            else if (args[1]->isBoolean()) valueType = "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a";

            store.define(name, valueType, sad::ui::StateBindingType::State,
                         std::any(*args[1]));
        } else {
            store.set(name, std::any(*args[1]));
        }

        // التجميع الذكي: لا نعيد البناء فوراً
        // handleEvent() أو updateTimers() سيستدعي rebuildUI() بعد الانتهاء
        bridge->markStateDirty();

        return std::make_shared<Data::Value>();
    };
    fm.defineBuiltInFunction(
        "\xd8\xb9\xd9\x8a\xd9\x91\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85"), FP("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9")},
        set_state_fn); // عيّن_حالة
    fm.defineBuiltInFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85"), FP("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9")},
        set_state_fn); // عين_حالة (بدون شدة)

    // ─── اقرأ_حالة(اسم) ──────────────────────────────────────
    // قراءة قيمة حالة مخزّنة
    auto get_state_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        if (args.empty() || !args[0]->isString()) {
            throw std::runtime_error(
                "\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9() "
                "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xa7\xd8\xb3\xd9\x85 "
                "(\xd9\x86\xd8\xb5)");
            // اقرأ_حالة() يحتاج اسم (نص)
        }

        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>();

        const auto& name = args[0]->toString();
        auto& store = bridge->getStateStore();

        if (!store.has(name)) {
            return std::make_shared<Data::Value>(); // لاشيء
        }

        auto val = store.get(name);
        try {
            return std::make_shared<Data::Value>(std::any_cast<Data::Value>(val));
        } catch (const std::bad_any_cast&) {
            return std::make_shared<Data::Value>(); // نوع غير متوافق
        }
    };
    fm.defineBuiltInFunction(
        "\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85")},
        get_state_fn); // اقرأ_حالة

    // ─── هل_توجد_حالة(اسم) ──────────────────────────────────
    // فحص وجود حالة معرّفة
    auto has_state_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        if (args.empty() || !args[0]->isString()) {
            throw std::runtime_error(
                "\xd9\x87\xd9\x84_\xd8\xaa\xd9\x88\xd8\xac\xd8\xaf_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9() "
                "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xa7\xd8\xb3\xd9\x85 "
                "(\xd9\x86\xd8\xb5)");
            // هل_توجد_حالة() يحتاج اسم (نص)
        }

        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(false);

        return std::make_shared<Data::Value>(
            bridge->getStateStore().has(args[0]->toString()));
    };
    fm.defineBuiltInFunction(
        "\xd9\x87\xd9\x84_\xd8\xaa\xd9\x88\xd8\xac\xd8\xaf_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85")},
        has_state_fn); // هل_توجد_حالة

    // ─── ابدأ_تجميع() ────────────────────────────────────────
    // بدء تجميع التحديثات — لا يُخطر المراقبون حتى إنهاء التجميع
    auto begin_batch_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        auto* bridge = UIBridge::active();
        if (bridge) {
            bridge->getStateStore().beginBatch();
        }
        return std::make_shared<Data::Value>();
    };
    fm.defineBuiltInFunction(
        "\xd8\xa7\xd8\xa8\xd8\xaf\xd8\xa3_\xd8\xaa\xd8\xac\xd9\x85\xd9\x8a\xd8\xb9",
        {},
        begin_batch_fn); // ابدأ_تجميع

    // ─── أنهِ_تجميع() ────────────────────────────────────────
    // إنهاء التجميع + إعادة بناء الواجهة إذا تغيرت الحالة
    auto end_batch_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        auto* bridge = UIBridge::active();
        if (bridge) {
            bridge->getStateStore().endBatch();
            // إعادة البناء إذا تغيرت الحالة خلال التجميع
            if (bridge->isStateDirty()) {
                bridge->rebuildUI();
            }
        }
        return std::make_shared<Data::Value>();
    };
    fm.defineBuiltInFunction(
        "\xd8\xa3\xd9\x86\xd9\x87\xd9\x90_\xd8\xaa\xd8\xac\xd9\x85\xd9\x8a\xd8\xb9",
        {},
        end_batch_fn); // أنهِ_تجميع
    fm.defineBuiltInFunction(
        "\xd8\xa3\xd9\x86\xd9\x87_\xd8\xaa\xd8\xac\xd9\x85\xd9\x8a\xd8\xb9",
        {},
        end_batch_fn); // أنه_تجميع (بدون كسرة)

    // ─── عرّف_محسوب(اسم, [اعتمادات], دالة_الحساب) ──────────
    // تعريف حالة محسوبة تلقائياً من حالات أخرى
    auto define_computed_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 3 || !args[0]->isString() ||
            !args[1]->isArray() || !args[2]->isFunction()) {
            throw std::runtime_error(
                "\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81_\xd9\x85\xd8\xad\xd8\xb3\xd9\x88\xd8\xa8() "
                "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xa7\xd8\xb3\xd9\x85\xd8\x8c "
                "\xd9\x85\xd8\xb5\xd9\x81\xd9\x88\xd9\x81\xd8\xa9_\xd8\xa7\xd8\xb9\xd8\xaa\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa\xd8\x8c "
                "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9");
            // عرّف_محسوب() يحتاج اسم، مصفوفة_اعتمادات، دالة
        }

        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>();

        const auto& name = args[0]->toString();
        auto& store = bridge->getStateStore();

        // استخراج أسماء الاعتمادات
        std::unordered_set<std::string> deps;
        for (const auto& item : args[1]->toArrayRef()) {
            if (item.isString()) {
                deps.insert(item.toString());
            }
        }

        // حفظ مرجع الدالة واسمها
        auto funcRef = args[2]->toFunction();
        if (!funcRef) return std::make_shared<Data::Value>();

        std::string funcName = funcRef->registeredName;
        auto* interp = bridge->getInterpreter();

        // دالة الحساب — تستدعي دالة المستخدم وتُرجع النتيجة
        store.defineComputed(name, deps,
            [interp, funcName]() -> std::any {
                if (!interp) return {};
                try {
                    auto result = interp->callUserFunction(funcName, {});
                    return std::any(result);
                } catch (...) {
                    return {};
                }
            });

        return std::make_shared<Data::Value>();
    };
    fm.defineBuiltInFunction(
        "\xd8\xb9\xd8\xb1\xd9\x91\xd9\x81_\xd9\x85\xd8\xad\xd8\xb3\xd9\x88\xd8\xa8",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85"), FP("\xd8\xa7\xd8\xb9\xd8\xaa\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa"), FP("\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9")},
        define_computed_fn); // عرّف_محسوب
    fm.defineBuiltInFunction(
        "\xd8\xb9\xd8\xb1\xd9\x81_\xd9\x85\xd8\xad\xd8\xb3\xd9\x88\xd8\xa8",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85"), FP("\xd8\xa7\xd8\xb9\xd8\xaa\xd9\x85\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa"), FP("\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9")},
        define_computed_fn); // عرف_محسوب (بدون شدة)

    // ─── راقب_حالة(اسم, دالة_المراقبة) ─────────────────────
    // تسجيل دالة تُستدعى عند تغيّر حالة معينة
    auto observe_state_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        if (args.size() < 2 || !args[0]->isString() || !args[1]->isFunction()) {
            throw std::runtime_error(
                "\xd8\xb1\xd8\xa7\xd9\x82\xd8\xa8_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9() "
                "\xd9\x8a\xd8\xad\xd8\xaa\xd8\xa7\xd8\xac \xd8\xa7\xd8\xb3\xd9\x85 "
                "(\xd9\x86\xd8\xb5) \xd9\x88\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9");
            // راقب_حالة() يحتاج اسم (نص) ودالة
        }

        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>();

        const auto& name = args[0]->toString();
        auto funcRef = args[1]->toFunction();
        if (!funcRef) return std::make_shared<Data::Value>();

        // إنشاء مراقب يغلّف دالة المستخدم
        auto observer = std::make_shared<SadFunctionObserver>(
            bridge->getInterpreter(), funcRef->registeredName);

        // تسجيل المراقب في StateStore
        bridge->getStateStore().addObserver(name, observer);

        // الاحتفاظ بالمراقب (لمنع انتهاء صلاحية weak_ptr)
        bridge->keepObserver(observer);

        return std::make_shared<Data::Value>();
    };
    fm.defineBuiltInFunction(
        "\xd8\xb1\xd8\xa7\xd9\x82\xd8\xa8_\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {FP("\xd8\xa7\xd8\xb3\xd9\x85"), FP("\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9")},
        observe_state_fn); // راقب_حالة

    // ─── أفرغ_الحالة() ──────────────────────────────────────
    // طباعة محتويات مخزن الحالة (للتصحيح والتطوير)
    auto dump_state_fn = [](Sad::Interpreter::BuiltinContext &ctx)
        -> std::shared_ptr<Data::Value>
    {
                const auto &args = ctx.args(); (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string(""));

        auto dumpStr = bridge->getStateStore().dump();
        std::cout << dumpStr << std::endl;
        return std::make_shared<Data::Value>(dumpStr);
    };
    fm.defineBuiltInFunction(
        "\xd8\xa3\xd9\x81\xd8\xb1\xd8\xba_\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9",
        {},
        dump_state_fn); // أفرغ_الحالة
}

} // namespace Interpreter
} // namespace Sad
