/**
 * @file builtin_kernel_audio.cpp
 * @brief (AR) وحدة الصوت — تشغيل وإدارة الأجهزة الصوتية
 * @brief (EN) Audio module — sound device management
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "value.h"
#include "audio.h"

#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsKernelAudio(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // 1. اكتشاف وتهيئة / Discovery & Init
    // ═══════════════════════════════════════════════════════════════
    // 1. audio_scan / صوت_مسح
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().scanDevices());
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x85\xd8\xb3\xd8\xad", f);
    }

    // 2. audio_device_count / صوت_عدد_الأجهزة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().getDeviceCount());
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa7\xd8\xac\xd9\x87\xd8\xb2\xd8\xa9", f);
    }

    // 3. audio_device_model / صوت_موديل_الجهاز
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().getDeviceInfo(id).model);
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x85\xd9\x88\xd8\xaf\xd9\x8a\xd9\x84_\xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2", f);
    }

    // 4. audio_device_type / صوت_نوع_الجهاز
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                static_cast<int>(LowLevel::AudioManager::getInstance().getDeviceInfo(id).deviceType));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x86\xd9\x88\xd8\xb9_\xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2", f);
    }

    // 5. audio_init / صوت_تهيئة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().initDevice(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9", f);
    }

    // 6. audio_report / صوت_تقرير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().generateReport());
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 2. تشغيل / Playback
    // ═══════════════════════════════════════════════════════════════
    // 7. audio_play_tone / صوت_تشغيل_نغمة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int freq = args.size() > 1 ? args[1]->toInt() : 440;
            int dur = args.size() > 2 ? args[2]->toInt() : 100;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().playTone(id, freq, dur));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84_\xd9\x86\xd8\xba\xd9\x85\xd8\xa9", f);
    }

    // 8. audio_stop / صوت_ايقاف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().stopPlayback(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81", f);
    }

    // 9. audio_pause / صوت_ايقاف_مؤقت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().pausePlayback(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81_\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa", f);
    }

    // 10. audio_resume / صوت_استئناف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().resumePlayback(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xa6\xd9\x86\xd8\xa7\xd9\x81", f);
    }

    // 11. audio_is_playing / صوت_يعمل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::AudioManager::getInstance().isPlaying(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x8a\xd8\xb9\xd9\x85\xd9\x84", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 3. التحكم بالصوت / Volume Control
    // ═══════════════════════════════════════════════════════════════
    // 12. audio_set_volume / صوت_تعيين_مستوى
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int vol = args.size() > 1 ? args[1]->toInt() : 50;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().setVolume(id, vol));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89", f);
    }

    // 13. audio_get_volume / صوت_المستوى
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().getVolume(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd9\x88\xd9\x89", f);
    }

    // 14. audio_mute / صوت_كتم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            bool mute = args.size() > 1 ? (args[1]->toInt() != 0) : true;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().setMute(id, mute));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x83\xd8\xaa\xd9\x85", f);
    }

    // 15. audio_is_muted / صوت_مكتوم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::AudioManager::getInstance().isMuted(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x85\xd9\x83\xd8\xaa\xd9\x88\xd9\x85", f);
    }

    // 16. audio_set_balance / صوت_تعيين_ميزان
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int bal = args.size() > 1 ? args[1]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().setBalance(id, bal));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x85\xd9\x8a\xd8\xb2\xd8\xa7\xd9\x86", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 4. تسجيل / Recording
    // ═══════════════════════════════════════════════════════════════
    // 17. audio_start_record / صوت_بدء_تسجيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().startRecording(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa8\xd8\xaf\xd8\xa1_\xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84", f);
    }

    // 18. audio_stop_record / صوت_انهاء_تسجيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().stopRecording(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x86\xd9\x87\xd8\xa7\xd8\xa1_\xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84", f);
    }

    // 19. audio_is_recording / صوت_يسجل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::AudioManager::getInstance().isRecording(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd9\x8a\xd8\xb3\xd8\xac\xd9\x84", f);
    }

    // 20. audio_recorded_samples / صوت_العينات_المسجلة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().getRecordedSamples(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xb9\xd9\x8a\xd9\x86\xd8\xa7\xd8\xaa_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xac\xd9\x84\xd8\xa9", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 5. PC Speaker
    // ═══════════════════════════════════════════════════════════════
    // 21. audio_beep / صوت_صافرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int freq = args.size() > 0 ? args[0]->toInt() : 1000;
            int dur = args.size() > 1 ? args[1]->toInt() : 200;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().beep(freq, dur));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xb5\xd8\xa7\xd9\x81\xd8\xb1\xd8\xa9", f);
    }

    // 22. audio_beep_off / صوت_ايقاف_صافرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().beepOff());
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81_\xd8\xb5\xd8\xa7\xd9\x81\xd8\xb1\xd8\xa9", f);
    }

    // ═══════════════════════════════════════════════════════════════
    // 6. تهيئة / Config
    // ═══════════════════════════════════════════════════════════════
    // 23. audio_set_sample_rate / صوت_تعيين_معدل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int rate = args.size() > 1 ? args[1]->toInt() : 48000;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().setSampleRate(id, rate));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb9\xd8\xaf\xd9\x84", f);
    }

    // 24. audio_get_sample_rate / صوت_المعدل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().getSampleRate(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xaf\xd9\x84", f);
    }

    // 25. audio_set_channels / صوت_تعيين_قنوات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int ch = args.size() > 1 ? args[1]->toInt() : 2;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().setChannels(id, ch));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x82\xd9\x86\xd9\x88\xd8\xa7\xd8\xaa", f);
    }

    // 26. audio_get_channels / صوت_القنوات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().getChannels(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x84\xd9\x82\xd9\x86\xd9\x88\xd8\xa7\xd8\xaa", f);
    }

    // 27. audio_set_bits / صوت_تعيين_بتات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int bits = args.size() > 1 ? args[1]->toInt() : 16;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().setBitsPerSample(id, bits));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd8\xa8\xd8\xaa\xd8\xa7\xd8\xaa", f);
    }

    // 28. audio_get_bits / صوت_البتات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().getBitsPerSample(id));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xa8\xd8\xaa\xd8\xa7\xd8\xaa", f);
    }

    // 29. audio_silence / صوت_صمت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            int dur = args.size() > 1 ? args[1]->toInt() : 100;
            return std::make_shared<Data::Value>(LowLevel::AudioManager::getInstance().playSilence(id, dur));
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xb5\xd9\x85\xd8\xaa", f);
    }

    // 30. audio_reset / صوت_اعادة_تعيين
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            LowLevel::AudioManager::getInstance().reset();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("\xd8\xb5\xd9\x88\xd8\xaa_\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86", f);
    }

} // registerBuiltinsKernelAudio

} // namespace Interpreter
} // namespace Sad
