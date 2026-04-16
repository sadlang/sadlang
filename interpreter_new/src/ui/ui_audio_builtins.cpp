/**
 * @file ui_audio_builtins.cpp
 * @brief (AR) نظام الصوت + التحكم بالمستوى
 * @brief (EN) Audio system + volume control
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerUIAudioBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // شغّل_صوت(مسار، تكرار=خطأ) → معرّف
    auto play_sound_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(0.0);
        std::string path = args[0]->isString() ? args[0]->toString() : "";
        bool loop = (args.size() > 1 && args[1]->isBoolean()) ? args[1]->toBool() : false;
        int id = bridge->playSound(path, loop);
        return std::make_shared<Data::Value>(static_cast<double>(id));
    };
    fm.registerBuiltinFunction("\xd8\xb4\xd8\xba\xd9\x91\xd9\x84_\xd8\xb5\xd9\x88\xd8\xaa", play_sound_fn);
    fm.registerBuiltinFunction("\xd8\xb4\xd8\xba\xd9\x84_\xd8\xb5\xd9\x88\xd8\xaa", play_sound_fn);
    fm.registerBuiltinFunction("playSound", play_sound_fn);

    // أوقف_صوت(معرّف)
    auto stop_sound_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        int id = args[0]->isNumeric() ? static_cast<int>(args[0]->toDouble()) : 0;
        bridge->stopSound(id);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa3\xd9\x88\xd9\x82\xd9\x81_\xd8\xb5\xd9\x88\xd8\xaa", stop_sound_fn);
    fm.registerBuiltinFunction("stopSound", stop_sound_fn);

    // أوقف_كل_الأصوات()
    auto stop_all_sounds_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (bridge) bridge->stopAllSounds();
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa3\xd9\x88\xd9\x82\xd9\x81_\xd9\x83\xd9\x84_\xd8\xa7\xd9\x84\xd8\xa3\xd8\xb5\xd9\x88\xd8\xa7\xd8\xaa", stop_all_sounds_fn);
    fm.registerBuiltinFunction("stopAllSounds", stop_all_sounds_fn);

    // مستوى_الصوت(قيمة) — set volume
    auto set_volume_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>();
        float vol = args[0]->isNumeric() ? static_cast<float>(args[0]->toDouble()) : 1.0f;
        bridge->setVolume(vol);
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89_\xd8\xa7\xd9\x84\xd8\xb5\xd9\x88\xd8\xaa", set_volume_fn);
    fm.registerBuiltinFunction("setVolume", set_volume_fn);

    // احصل_مستوى_الصوت()
    auto get_volume_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(0.0);
        return std::make_shared<Data::Value>(static_cast<double>(bridge->getVolume()));
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89_\xd8\xa7\xd9\x84\xd8\xb5\xd9\x88\xd8\xaa", get_volume_fn);
    fm.registerBuiltinFunction("getVolume", get_volume_fn);
}

} // namespace Interpreter
} // namespace Sad
