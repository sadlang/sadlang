/*
 * ============================================================================
 *  وحدة الصوت المنخفضة المستوى — لغة ص
 *  Low-Level Audio Module — Sad Language
 * ============================================================================
 *
 * @brief (AR) وحدة إدارة أجهزة الصوت بدون نظام تشغيل
 *             تدعم: AC'97، Intel HDA (High Definition Audio)
 *             تشغيل نغمات، تسجيل، تحكم بالصوت
 *
 * @brief (EN) Bare-metal audio management module
 *             Supports: AC'97, Intel HDA
 *             Tone playback, recording, volume control
 *
 * @details
 * (AR) هذه الوحدة توفر واجهة برمجية موحدة لأجهزة الصوت.
 *      على سطح المكتب: محاكاة آمنة.
 *      على النواة: وصول مباشر عبر PCI وMMIO.
 *
 *      الميزات:
 *        1. اكتشاف أجهزة الصوت — مسح PCI
 *        2. تشغيل نغمات — مولّد موجة جيبية بسيط
 *        3. التحكم بالصوت — مستوى، كتم، ميزان
 *        4. تسجيل — التقاط من الميكروفون (محاكاة)
 *        5. PC Speaker — نفخ مباشر في سماعة اللوحة الأم
 *
 * (EN) Unified audio device API. Simulated on desktop,
 *      direct hardware on bare-metal.
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_AUDIO_H
#define SAD_LOW_LEVEL_AUDIO_H

#include <cstdint>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) ثوابت الصوت / (EN) Audio Constants
// ============================================================================
namespace AudioConstants {
    constexpr int SAMPLE_RATE_44100 = 44100;
    constexpr int SAMPLE_RATE_48000 = 48000;
    constexpr int BITS_16 = 16;
    constexpr int BITS_24 = 24;
    constexpr int CHANNELS_MONO = 1;
    constexpr int CHANNELS_STEREO = 2;
    constexpr int MAX_VOLUME = 100;
    constexpr uint8_t PCI_CLASS_MULTIMEDIA = 0x04;
    constexpr uint8_t PCI_SUBCLASS_AUDIO = 0x01;
    constexpr uint8_t PCI_SUBCLASS_HDA = 0x03;
}

// ============================================================================
// (AR) نوع جهاز الصوت / (EN) Audio Device Type
// ============================================================================
enum class AudioDeviceType : uint8_t {
    NONE       = 0,  // (AR) لا يوجد / (EN) None
    AC97       = 1,  // (AR) AC'97 / (EN) AC'97
    HDA        = 2,  // (AR) Intel HDA / (EN) Intel HDA
    PC_SPEAKER = 3,  // (AR) سماعة اللوحة / (EN) PC Speaker
    UNKNOWN    = 255
};

// ============================================================================
// (AR) حالة جهاز الصوت / (EN) Audio Device State
// ============================================================================
enum class AudioState : uint8_t {
    NOT_FOUND    = 0,  // (AR) غير موجود / (EN) Not found
    FOUND        = 1,  // (AR) موجود / (EN) Found
    INITIALIZED  = 2,  // (AR) مهيأ / (EN) Initialized
    PLAYING      = 3,  // (AR) يعمل / (EN) Playing
    RECORDING    = 4,  // (AR) يسجل / (EN) Recording
    PAUSED       = 5,  // (AR) متوقف مؤقتاً / (EN) Paused
    ERROR_STATE  = 6   // (AR) خطأ / (EN) Error
};

// ============================================================================
// (AR) معلومات جهاز الصوت / (EN) Audio Device Info
// ============================================================================
struct AudioDeviceInfo {
    int id;
    AudioDeviceType deviceType;
    AudioState audioState;
    std::string model;
    uint16_t vendorId;
    uint16_t deviceId;
    int sampleRate;
    int bitsPerSample;
    int channels;
    int volume;         // (AR) 0-100 / (EN) 0-100
    bool muted;
    int pciSlot;
};

// ============================================================================
// (AR) مدير الصوت — نمط المفرد / (EN) Audio Manager — Singleton
// ============================================================================
class AudioManager {
public:
    static AudioManager& getInstance() {
        static AudioManager instance;
        return instance;
    }

    // ════════════════════════════════════════════════════════════════
    // (AR) 1. اكتشاف وتهيئة / (EN) 1. Discovery & Init
    // ════════════════════════════════════════════════════════════════
    int scanDevices();
    int getDeviceCount() const { return static_cast<int>(devices_.size()); }
    AudioDeviceInfo getDeviceInfo(int devId) const;
    int initDevice(int devId);
    std::string generateReport() const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 2. تشغيل الصوت / (EN) 2. Playback
    // ════════════════════════════════════════════════════════════════

    /// (AR) تشغيل نغمة (تردد هرتز، مدة مللي ثانية)
    /// (EN) Play tone (frequency Hz, duration ms)
    int playTone(int devId, int freqHz, int durationMs);

    /// (AR) تشغيل صمت / (EN) Play silence
    int playSilence(int devId, int durationMs);

    /// (AR) إيقاف التشغيل / (EN) Stop playback
    int stopPlayback(int devId);

    /// (AR) إيقاف مؤقت / (EN) Pause
    int pausePlayback(int devId);

    /// (AR) استئناف / (EN) Resume
    int resumePlayback(int devId);

    /// (AR) هل يعمل؟ / (EN) Is playing?
    bool isPlaying(int devId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 3. التحكم بالصوت / (EN) 3. Volume Control
    // ════════════════════════════════════════════════════════════════
    int setVolume(int devId, int vol);
    int getVolume(int devId) const;
    int setMute(int devId, bool mute);
    bool isMuted(int devId) const;
    int setBalance(int devId, int leftRight); // -100 يسار، 0 وسط، 100 يمين

    // ════════════════════════════════════════════════════════════════
    // (AR) 4. تسجيل / (EN) 4. Recording
    // ════════════════════════════════════════════════════════════════
    int startRecording(int devId);
    int stopRecording(int devId);
    bool isRecording(int devId) const;
    int getRecordedSamples(int devId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 5. PC Speaker / (EN) 5. PC Speaker (beep)
    // ════════════════════════════════════════════════════════════════
    int beep(int freqHz, int durationMs);
    int beepOff();

    // ════════════════════════════════════════════════════════════════
    // (AR) 6. تهيئة / (EN) 6. Configuration
    // ════════════════════════════════════════════════════════════════
    int setSampleRate(int devId, int rate);
    int getSampleRate(int devId) const;
    int setChannels(int devId, int ch);
    int getChannels(int devId) const;
    int setBitsPerSample(int devId, int bits);
    int getBitsPerSample(int devId) const;

    // (AR) إعادة تعيين / (EN) Reset
    void reset();

private:
    AudioManager();
    ~AudioManager() = default;
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::vector<AudioDeviceInfo> devices_;
    std::vector<int> balances_;          // (AR) ميزان لكل جهاز
    std::vector<int> recordedSamples_;   // (AR) عينات مسجلة
    bool pcSpeakerActive_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_AUDIO_H
