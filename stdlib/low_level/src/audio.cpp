/**
 * @file audio.cpp
 * @brief (AR) تنفيذ مدير الصوت — AC'97، HDA، PC Speaker
 *        محاكاة آمنة على سطح المكتب
 * @brief (EN) Audio Manager implementation — AC'97, HDA, PC Speaker
 *        Safe simulation on desktop
 */

#include "audio.h"
#include <sstream>
#include <cstring>
#include <algorithm>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) المُنشئ / (EN) Constructor
// ============================================================================
AudioManager::AudioManager() : pcSpeakerActive_(false) {}

// ============================================================================
// (AR) 1. اكتشاف وتهيئة / (EN) 1. Discovery & Init
// ============================================================================

int AudioManager::scanDevices() {
    devices_.clear();
    balances_.clear();
    recordedSamples_.clear();

    // (AR) جهاز HDA محاكى — Realtek ALC1220
    // (EN) Simulated HDA device — Realtek ALC1220
    AudioDeviceInfo hda;
    hda.id = 0;
    hda.deviceType = AudioDeviceType::HDA;
    hda.audioState = AudioState::FOUND;
    hda.model = "Realtek ALC1220 HD Audio";
    hda.vendorId = 0x10EC;
    hda.deviceId = 0x1220;
    hda.sampleRate = AudioConstants::SAMPLE_RATE_48000;
    hda.bitsPerSample = AudioConstants::BITS_16;
    hda.channels = AudioConstants::CHANNELS_STEREO;
    hda.volume = 75;
    hda.muted = false;
    hda.pciSlot = 0;
    devices_.push_back(hda);
    balances_.push_back(0);
    recordedSamples_.push_back(0);

    // (AR) جهاز AC'97 محاكى / (EN) Simulated AC'97
    AudioDeviceInfo ac97;
    ac97.id = 1;
    ac97.deviceType = AudioDeviceType::AC97;
    ac97.audioState = AudioState::FOUND;
    ac97.model = "Intel AC'97 Audio";
    ac97.vendorId = 0x8086;
    ac97.deviceId = 0x2415;
    ac97.sampleRate = AudioConstants::SAMPLE_RATE_44100;
    ac97.bitsPerSample = AudioConstants::BITS_16;
    ac97.channels = AudioConstants::CHANNELS_STEREO;
    ac97.volume = 50;
    ac97.muted = false;
    ac97.pciSlot = 1;
    devices_.push_back(ac97);
    balances_.push_back(0);
    recordedSamples_.push_back(0);

    // (AR) سماعة اللوحة / (EN) PC Speaker
    AudioDeviceInfo spk;
    spk.id = 2;
    spk.deviceType = AudioDeviceType::PC_SPEAKER;
    spk.audioState = AudioState::FOUND;
    spk.model = "PC Speaker (Beep)";
    spk.vendorId = 0;
    spk.deviceId = 0;
    spk.sampleRate = 0;
    spk.bitsPerSample = 0;
    spk.channels = 1;
    spk.volume = 100;
    spk.muted = false;
    spk.pciSlot = -1;
    devices_.push_back(spk);
    balances_.push_back(0);
    recordedSamples_.push_back(0);

    return Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size");
}

AudioDeviceInfo AudioManager::getDeviceInfo(int devId) const {
    if (devId >= 0 && devId < Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size"))
        return devices_[devId];
    AudioDeviceInfo empty;
    empty.id = -1;
    empty.deviceType = AudioDeviceType::NONE;
    empty.audioState = AudioState::NOT_FOUND;
    empty.model = "";
    empty.vendorId = 0;
    empty.deviceId = 0;
    empty.sampleRate = 0;
    empty.bitsPerSample = 0;
    empty.channels = 0;
    empty.volume = 0;
    empty.muted = true;
    empty.pciSlot = -1;
    return empty;
}

int AudioManager::initDevice(int devId) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    devices_[devId].audioState = AudioState::INITIALIZED;
    return 0;
}

std::string AudioManager::generateReport() const {
    std::ostringstream ss;
    ss << "=== \xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb5\xd9\x88\xd8\xaa / Audio Report ===\n";
    ss << "\xd8\xb9\xd8\xaf\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa3\xd8\xac\xd9\x87\xd8\xb2\xd8\xa9 / Devices: " << devices_.size() << "\n\n";
    for (const auto& d : devices_) {
        ss << "--- Device #" << d.id << " ---\n";
        ss << "  Model: " << d.model << "\n";
        ss << "  Type: ";
        switch (d.deviceType) {
            case AudioDeviceType::HDA: ss << "Intel HDA"; break;
            case AudioDeviceType::AC97: ss << "AC'97"; break;
            case AudioDeviceType::PC_SPEAKER: ss << "PC Speaker"; break;
            default: ss << "Unknown"; break;
        }
        ss << "\n";
        ss << "  Volume: " << d.volume << "%" << (d.muted ? " (MUTED)" : "") << "\n";
        ss << "  Sample Rate: " << d.sampleRate << " Hz\n\n";
    }
    return ss.str();
}

// ============================================================================
// (AR) 2. تشغيل الصوت / (EN) 2. Playback
// ============================================================================

int AudioManager::playTone(int devId, int freqHz, int durationMs) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    if (devices_[devId].muted) return 0;
    (void)freqHz; (void)durationMs;
    devices_[devId].audioState = AudioState::PLAYING;
    return 0;
}

int AudioManager::playSilence(int devId, int durationMs) {
    return playTone(devId, 0, durationMs);
}

int AudioManager::stopPlayback(int devId) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    if (devices_[devId].audioState == AudioState::PLAYING ||
        devices_[devId].audioState == AudioState::PAUSED)
        devices_[devId].audioState = AudioState::INITIALIZED;
    return 0;
}

int AudioManager::pausePlayback(int devId) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    if (devices_[devId].audioState == AudioState::PLAYING)
        devices_[devId].audioState = AudioState::PAUSED;
    return 0;
}

int AudioManager::resumePlayback(int devId) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    if (devices_[devId].audioState == AudioState::PAUSED)
        devices_[devId].audioState = AudioState::PLAYING;
    return 0;
}

bool AudioManager::isPlaying(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return false;
    return devices_[devId].audioState == AudioState::PLAYING;
}

// ============================================================================
// (AR) 3. التحكم بالصوت / (EN) 3. Volume Control
// ============================================================================

int AudioManager::setVolume(int devId, int vol) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    devices_[devId].volume = std::max(0, std::min(vol, AudioConstants::MAX_VOLUME));
    return 0;
}

int AudioManager::getVolume(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    return devices_[devId].volume;
}

int AudioManager::setMute(int devId, bool mute) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    devices_[devId].muted = mute;
    return 0;
}

bool AudioManager::isMuted(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return true;
    return devices_[devId].muted;
}

int AudioManager::setBalance(int devId, int leftRight) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    balances_[devId] = std::max(-100, std::min(leftRight, 100));
    return 0;
}

// ============================================================================
// (AR) 4. تسجيل / (EN) 4. Recording
// ============================================================================

int AudioManager::startRecording(int devId) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    devices_[devId].audioState = AudioState::RECORDING;
    recordedSamples_[devId] = 0;
    return 0;
}

int AudioManager::stopRecording(int devId) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    if (devices_[devId].audioState == AudioState::RECORDING) {
        // (AR) محاكاة: 1000 عينة مسجلة / (EN) Simulation: 1000 samples recorded
        recordedSamples_[devId] = 1000;
        devices_[devId].audioState = AudioState::INITIALIZED;
    }
    return 0;
}

bool AudioManager::isRecording(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return false;
    return devices_[devId].audioState == AudioState::RECORDING;
}

int AudioManager::getRecordedSamples(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return 0;
    return recordedSamples_[devId];
}

// ============================================================================
// (AR) 5. PC Speaker / (EN) 5. PC Speaker
// ============================================================================

int AudioManager::beep(int freqHz, int durationMs) {
    (void)freqHz; (void)durationMs;
    pcSpeakerActive_ = true;
    return 0;
}

int AudioManager::beepOff() {
    pcSpeakerActive_ = false;
    return 0;
}

// ============================================================================
// (AR) 6. تهيئة / (EN) 6. Configuration
// ============================================================================

int AudioManager::setSampleRate(int devId, int rate) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    devices_[devId].sampleRate = rate;
    return 0;
}

int AudioManager::getSampleRate(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return 0;
    return devices_[devId].sampleRate;
}

int AudioManager::setChannels(int devId, int ch) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    devices_[devId].channels = std::max(1, std::min(ch, 8));
    return 0;
}

int AudioManager::getChannels(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return 0;
    return devices_[devId].channels;
}

int AudioManager::setBitsPerSample(int devId, int bits) {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return -1;
    devices_[devId].bitsPerSample = bits;
    return 0;
}

int AudioManager::getBitsPerSample(int devId) const {
    if (devId < 0 || devId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(devices_.size(), "audio_size")) return 0;
    return devices_[devId].bitsPerSample;
}

// ============================================================================
// (AR) إعادة تعيين / (EN) Reset
// ============================================================================

void AudioManager::reset() {
    devices_.clear();
    balances_.clear();
    recordedSamples_.clear();
    pcSpeakerActive_ = false;
}

} // namespace LowLevel
} // namespace Sad
