/**
 * @file ui_audio_builtins.cpp
 * @brief (AR) نظام الصوت + التحكم بالمستوى
 * @brief (EN) Audio system + volume control
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <memory>
#include <string>
#include <vector>
#include "builtins/builtin_context.h"

namespace Sad
{
    namespace Interpreter
    {

        void registerUIAudioBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت الصوت
            namespace Ba = Builtins::Names::UIAudio;

            // شغّل_صوت(مسار، تكرار=خطأ) → معرّف
            auto play_sound_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(0.0);
                std::string path = args[0]->isString() ? args[0]->toString() : "";
                bool loop = (args.size() > 1 && args[1]->isBoolean()) ? args[1]->toBool() : false;
                int id = bridge->playSound(path, loop);
                return std::make_shared<Data::Value>(static_cast<double>(id));
            };
            fm.registerBuiltinFunction(std::string(Ba::PLAY_SOUND), play_sound_fn);

            // أوقف_صوت(معرّف)
            auto stop_sound_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                int id = args[0]->isNumeric() ? static_cast<int>(args[0]->toDouble()) : 0;
                bridge->stopSound(id);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Ba::STOP_SOUND), stop_sound_fn);

            // أوقف_كل_الأصوات()
            auto stop_all_sounds_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->stopAllSounds();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Ba::STOP_ALL), stop_all_sounds_fn);

            // مستوى_الصوت(قيمة) — set volume
            auto set_volume_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>();
                float vol = args[0]->isNumeric() ? static_cast<float>(args[0]->toDouble()) : 1.0f;
                bridge->setVolume(vol);
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Ba::SET_VOLUME), set_volume_fn);

            // احصل_مستوى_الصوت()
            auto get_volume_fn = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
            {
                const auto &args = ctx.args(); (void)args;
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(0.0);
                return std::make_shared<Data::Value>(static_cast<double>(bridge->getVolume()));
            };
            fm.registerBuiltinFunction(std::string(Ba::GET_VOLUME), get_volume_fn);
        }

    } // namespace Interpreter
} // namespace Sad
