// ==============================================================================
// audio_system.cpp - تنفيذ نظام الصوت / Audio System Implementation
// ==============================================================================
// الوصف: تنفيذ نظام الصوت باستخدام SDL2_mixer أو محرك صوت مخصص
// Description: Audio system implementation using SDL2_mixer or custom audio engine
// ==============================================================================

#include "../../include/audio/audio_system.h"
#include <algorithm>
#include <cmath>
#include <SDL.h>

// ملاحظة: يستخدم هذا التنفيذ واجهة SDL2 المدمجة للصوت
// Note: This implementation uses SDL2's built-in audio API for playback

namespace sad {
namespace graphics {

// ==============================================================================
// بيانات الصوت المحملة / Loaded Audio Buffer
// ==============================================================================

struct AudioBuffer {
    Uint8* data = nullptr;
    Uint32 length = 0;
    SDL_AudioSpec spec = {};
    bool converted = false;  // true = allocated by SDL_malloc, false = by SDL_LoadWAV
};

// ==============================================================================
// بيانات القناة الداخلية / Internal Channel Data
// ==============================================================================

struct InternalChannel {
    ChannelId id = INVALID_CHANNEL_ID;
    SoundId soundId = INVALID_SOUND_ID;
    AudioState state = AudioState::Stopped;
    PlaybackSettings settings;
    SpatialSettings spatial;
    Float32 currentPosition = 0.0f;
    Float32 fadeVolume = 1.0f;
    Float32 fadeSpeed = 0.0f;
    Float32 fadeTarget = 0.0f;
    Float32 delayRemaining = 0.0f;
    Int32 loopsRemaining = 0;
    Uint32 samplePosition = 0;
    Float32 spatialAttenuation = 1.0f;
    AudioCompleteCallback onComplete;
};

// ==============================================================================
// بيانات التنفيذ / Implementation Data
// ==============================================================================

struct AudioSystem::Impl {
    std::vector<InternalChannel> channels;
    std::unordered_map<SoundId, AudioBuffer> audioBuffers;
    SDL_AudioDeviceID deviceId = 0;
    SDL_AudioSpec obtainedSpec = {};
    AudioSystem* owner = nullptr;
    Int32 sampleRate = 44100;
    Int32 channelCount = 2;
    Int32 bufferSize = 2048;
    
    Impl() {
        channels.resize(MAX_AUDIO_CHANNELS);
    }
    
    InternalChannel* FindFreeChannel() {
        for (auto& ch : channels) {
            if (ch.state == AudioState::Stopped) {
                return &ch;
            }
        }
        return nullptr;
    }
    
    InternalChannel* GetChannel(ChannelId id) {
        for (auto& ch : channels) {
            if (ch.id == id && ch.state != AudioState::Stopped) {
                return &ch;
            }
        }
        return nullptr;
    }

    static void AudioCallback(void* userdata, Uint8* stream, int len);
};

// ==============================================================================
// استدعاء خلط الصوت / Audio Mixing Callback
// ==============================================================================

void AudioSystem::Impl::AudioCallback(void* userdata, Uint8* stream, int len) {
    auto* impl = static_cast<AudioSystem::Impl*>(userdata);
    SDL_memset(stream, 0, static_cast<size_t>(len));

    if (!impl->owner) return;
    AudioSystem* sys = impl->owner;
    if (sys->IsMasterMuted()) return;

    float masterVol = sys->GetMasterVolume();

    for (auto& ch : impl->channels) {
        if (ch.state != AudioState::Playing &&
            ch.state != AudioState::FadingIn &&
            ch.state != AudioState::FadingOut)
            continue;

        auto bufIt = impl->audioBuffers.find(ch.soundId);
        if (bufIt == impl->audioBuffers.end()) continue;

        const AudioBuffer& buf = bufIt->second;
        if (!buf.data || buf.length == 0) continue;

        // Handle end-of-buffer for non-looping sounds
        if (ch.samplePosition >= buf.length) {
            if (ch.settings.loop) {
                ch.samplePosition = 0;
            } else {
                continue;
            }
        }

        // Calculate effective volume
        float groupVol = 1.0f;
        if (ch.settings.group != 0) {
            const AudioGroup* grp = sys->GetGroup(ch.settings.group);
            if (grp) {
                if (grp->muted) continue;
                groupVol = grp->volume;
            }
        }

        float effectiveVol = ch.settings.volume * ch.fadeVolume * ch.spatialAttenuation * masterVol * groupVol;
        int sdlVol = static_cast<int>(effectiveVol * SDL_MIX_MAXVOLUME);
        if (sdlVol <= 0) {
            // Advance position even if silent
            ch.samplePosition += static_cast<Uint32>(len);
            if (ch.samplePosition >= buf.length && ch.settings.loop) {
                ch.samplePosition %= buf.length;
            }
            continue;
        }
        if (sdlVol > SDL_MIX_MAXVOLUME) sdlVol = SDL_MIX_MAXVOLUME;

        Uint32 remaining = buf.length - ch.samplePosition;
        Uint32 toMix = static_cast<Uint32>(len);

        if (toMix <= remaining) {
            // Enough data in buffer
            SDL_MixAudioFormat(stream, buf.data + ch.samplePosition,
                               impl->obtainedSpec.format, toMix, sdlVol);
            ch.samplePosition += toMix;
        } else {
            // Mix remainder, then loop if needed
            if (remaining > 0) {
                SDL_MixAudioFormat(stream, buf.data + ch.samplePosition,
                                   impl->obtainedSpec.format, remaining, sdlVol);
            }
            if (ch.settings.loop) {
                ch.samplePosition = 0;
                Uint32 mixed = remaining;
                while (mixed < toMix) {
                    Uint32 chunk = toMix - mixed;
                    if (chunk > buf.length) chunk = buf.length;
                    SDL_MixAudioFormat(stream + mixed, buf.data,
                                       impl->obtainedSpec.format, chunk, sdlVol);
                    ch.samplePosition = chunk % buf.length;
                    mixed += chunk;
                }
            } else {
                ch.samplePosition = buf.length; // mark as finished
            }
        }
    }
}

// ==============================================================================
// الإنشاء والتدمير / Construction and Destruction
// ==============================================================================

AudioSystem::AudioSystem()
    : m_impl(std::make_unique<Impl>())
{
    m_impl->owner = this;
}

AudioSystem::~AudioSystem() {
    Shutdown();
}

// ==============================================================================
// التهيئة والإغلاق / Initialization and Shutdown
// ==============================================================================

bool AudioSystem::Initialize(Int32 sampleRate, Int32 channels, Int32 bufferSize) {
    if (m_initialized) return true;
    
    m_impl->sampleRate = sampleRate;
    m_impl->channelCount = channels;
    m_impl->bufferSize = bufferSize;
    
    // تهيئة نظام الصوت SDL2 / Initialize SDL2 audio subsystem
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        return false;
    }

    SDL_AudioSpec want = {};
    want.freq = sampleRate;
    want.format = AUDIO_S16LSB;
    want.channels = static_cast<Uint8>(channels);
    want.samples = static_cast<Uint16>(bufferSize);
    want.callback = Impl::AudioCallback;
    want.userdata = m_impl.get();

    m_impl->deviceId = SDL_OpenAudioDevice(NULL, 0, &want, &m_impl->obtainedSpec, 0);
    if (m_impl->deviceId == 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    // بدء التشغيل / Start playback
    SDL_PauseAudioDevice(m_impl->deviceId, 0);

    m_initialized = true;
    return true;
}

void AudioSystem::Shutdown() {
    if (!m_initialized) return;

    StopAll();

    // إغلاق جهاز الصوت أولاً لإيقاف الاستدعاء / Close audio device first to stop callback
    if (m_impl->deviceId != 0) {
        SDL_CloseAudioDevice(m_impl->deviceId);
        m_impl->deviceId = 0;
    }

    UnloadAllSounds();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    m_initialized = false;
}

// ==============================================================================
// تحميل الأصوات / Loading Sounds
// ==============================================================================

SoundId AudioSystem::LoadSound(const std::string& filePath, const std::string& name) {
    if (!m_initialized) return INVALID_SOUND_ID;
    
    SoundClipInfo info;
    info.id = m_nextSoundId++;
    info.filePath = filePath;
    info.name = name.empty() ? filePath : name;
    info.isStream = false;
    
    // تحميل ملف WAV باستخدام SDL2 / Load WAV file using SDL2
    SDL_AudioSpec wavSpec;
    Uint8* wavData = nullptr;
    Uint32 wavLen = 0;

    if (!SDL_LoadWAV(filePath.c_str(), &wavSpec, &wavData, &wavLen)) {
        return INVALID_SOUND_ID;
    }

    // تحويل الصوت لمطابقة تنسيق الجهاز / Convert audio to match device format
    AudioBuffer buffer;
    if (wavSpec.freq != m_impl->obtainedSpec.freq ||
        wavSpec.format != m_impl->obtainedSpec.format ||
        wavSpec.channels != m_impl->obtainedSpec.channels) {

        SDL_AudioCVT cvt;
        int cvtResult = SDL_BuildAudioCVT(&cvt,
            wavSpec.format, wavSpec.channels, wavSpec.freq,
            m_impl->obtainedSpec.format, m_impl->obtainedSpec.channels,
            m_impl->obtainedSpec.freq);

        if (cvtResult < 0) {
            SDL_FreeWAV(wavData);
            return INVALID_SOUND_ID;
        }

        if (cvt.needed) {
            cvt.len = static_cast<int>(wavLen);
            cvt.buf = static_cast<Uint8*>(SDL_malloc(
                static_cast<size_t>(cvt.len) * static_cast<size_t>(cvt.len_mult)));
            if (!cvt.buf) {
                SDL_FreeWAV(wavData);
                return INVALID_SOUND_ID;
            }
            SDL_memcpy(cvt.buf, wavData, wavLen);
            SDL_ConvertAudio(&cvt);
            SDL_FreeWAV(wavData);

            buffer.data = cvt.buf;
            buffer.length = static_cast<Uint32>(cvt.len_cvt);
            buffer.spec = m_impl->obtainedSpec;
            buffer.converted = true;
        } else {
            buffer.data = wavData;
            buffer.length = wavLen;
            buffer.spec = wavSpec;
        }
    } else {
        buffer.data = wavData;
        buffer.length = wavLen;
        buffer.spec = wavSpec;
    }

    // حساب المدة الحقيقية / Calculate real duration
    int bytesPerSample = SDL_AUDIO_BITSIZE(m_impl->obtainedSpec.format) / 8;
    int frameSize = bytesPerSample * m_impl->obtainedSpec.channels;
    info.duration = (frameSize > 0)
        ? static_cast<Float32>(buffer.length) / (m_impl->obtainedSpec.freq * frameSize)
        : 0.0f;
    info.channels = m_impl->obtainedSpec.channels;
    info.sampleRate = m_impl->obtainedSpec.freq;

    m_impl->audioBuffers[info.id] = buffer;

    m_sounds[info.id] = info;
    m_soundsByName[info.name] = info.id;
    
    return info.id;
}

SoundId AudioSystem::LoadMusic(const std::string& filePath, const std::string& name) {
    if (!m_initialized) return INVALID_SOUND_ID;
    
    SoundClipInfo info;
    info.id = m_nextSoundId++;
    info.filePath = filePath;
    info.name = name.empty() ? filePath : name;
    info.isStream = true;
    
    // تحميل ملف موسيقى WAV باستخدام SDL2 / Load music WAV file using SDL2
    SDL_AudioSpec wavSpec;
    Uint8* wavData = nullptr;
    Uint32 wavLen = 0;

    if (!SDL_LoadWAV(filePath.c_str(), &wavSpec, &wavData, &wavLen)) {
        return INVALID_SOUND_ID;
    }

    // تحويل الصوت لمطابقة تنسيق الجهاز / Convert audio to match device format
    AudioBuffer buffer;
    if (wavSpec.freq != m_impl->obtainedSpec.freq ||
        wavSpec.format != m_impl->obtainedSpec.format ||
        wavSpec.channels != m_impl->obtainedSpec.channels) {

        SDL_AudioCVT cvt;
        int cvtResult = SDL_BuildAudioCVT(&cvt,
            wavSpec.format, wavSpec.channels, wavSpec.freq,
            m_impl->obtainedSpec.format, m_impl->obtainedSpec.channels,
            m_impl->obtainedSpec.freq);

        if (cvtResult < 0) {
            SDL_FreeWAV(wavData);
            return INVALID_SOUND_ID;
        }

        if (cvt.needed) {
            cvt.len = static_cast<int>(wavLen);
            cvt.buf = static_cast<Uint8*>(SDL_malloc(
                static_cast<size_t>(cvt.len) * static_cast<size_t>(cvt.len_mult)));
            if (!cvt.buf) {
                SDL_FreeWAV(wavData);
                return INVALID_SOUND_ID;
            }
            SDL_memcpy(cvt.buf, wavData, wavLen);
            SDL_ConvertAudio(&cvt);
            SDL_FreeWAV(wavData);

            buffer.data = cvt.buf;
            buffer.length = static_cast<Uint32>(cvt.len_cvt);
            buffer.spec = m_impl->obtainedSpec;
            buffer.converted = true;
        } else {
            buffer.data = wavData;
            buffer.length = wavLen;
            buffer.spec = wavSpec;
        }
    } else {
        buffer.data = wavData;
        buffer.length = wavLen;
        buffer.spec = wavSpec;
    }

    // حساب المدة الحقيقية / Calculate real duration
    int bytesPerSample = SDL_AUDIO_BITSIZE(m_impl->obtainedSpec.format) / 8;
    int frameSize = bytesPerSample * m_impl->obtainedSpec.channels;
    info.duration = (frameSize > 0)
        ? static_cast<Float32>(buffer.length) / (m_impl->obtainedSpec.freq * frameSize)
        : 0.0f;
    info.channels = m_impl->obtainedSpec.channels;
    info.sampleRate = m_impl->obtainedSpec.freq;

    m_impl->audioBuffers[info.id] = buffer;

    m_sounds[info.id] = info;
    m_soundsByName[info.name] = info.id;
    
    return info.id;
}

void AudioSystem::UnloadSound(SoundId soundId) {
    auto it = m_sounds.find(soundId);
    if (it != m_sounds.end()) {
        m_soundsByName.erase(it->second.name);

        // تحرير بيانات الصوت / Free audio buffer
        auto bufIt = m_impl->audioBuffers.find(soundId);
        if (bufIt != m_impl->audioBuffers.end()) {
            if (m_impl->deviceId != 0) SDL_LockAudioDevice(m_impl->deviceId);
            if (bufIt->second.data) {
                if (bufIt->second.converted) {
                    SDL_free(bufIt->second.data);
                } else {
                    SDL_FreeWAV(bufIt->second.data);
                }
            }
            m_impl->audioBuffers.erase(bufIt);
            if (m_impl->deviceId != 0) SDL_UnlockAudioDevice(m_impl->deviceId);
        }

        m_sounds.erase(it);
    }
}

void AudioSystem::UnloadAllSounds() {
    // تحرير جميع بيانات الصوت / Free all audio buffers
    if (m_impl->deviceId != 0) SDL_LockAudioDevice(m_impl->deviceId);
    for (auto& pair : m_impl->audioBuffers) {
        if (pair.second.data) {
            if (pair.second.converted) {
                SDL_free(pair.second.data);
            } else {
                SDL_FreeWAV(pair.second.data);
            }
            pair.second.data = nullptr;
        }
    }
    m_impl->audioBuffers.clear();
    if (m_impl->deviceId != 0) SDL_UnlockAudioDevice(m_impl->deviceId);

    m_sounds.clear();
    m_soundsByName.clear();
}

const SoundClipInfo* AudioSystem::GetSoundInfo(SoundId soundId) const {
    auto it = m_sounds.find(soundId);
    return (it != m_sounds.end()) ? &it->second : nullptr;
}

SoundId AudioSystem::FindSound(const std::string& name) const {
    auto it = m_soundsByName.find(name);
    return (it != m_soundsByName.end()) ? it->second : INVALID_SOUND_ID;
}

// ==============================================================================
// التشغيل / Playback
// ==============================================================================

ChannelId AudioSystem::Play(SoundId soundId, const PlaybackSettings& settings) {
    if (!m_initialized || soundId == INVALID_SOUND_ID) return INVALID_CHANNEL_ID;
    
    auto* soundInfo = GetSoundInfo(soundId);
    if (!soundInfo) return INVALID_CHANNEL_ID;
    
    auto* channel = m_impl->FindFreeChannel();
    if (!channel) {
        // لا توجد قناة فارغة، ابحث عن قناة ذات أولوية أقل
        // No free channel, find one with lower priority
        // TODO: تنفيذ نظام الأولويات / Implement priority system
        return INVALID_CHANNEL_ID;
    }
    
    SDL_LockAudioDevice(m_impl->deviceId);

    channel->id = m_nextChannelId++;
    channel->soundId = soundId;
    channel->settings = settings;
    channel->currentPosition = 0.0f;
    channel->samplePosition = 0;
    channel->spatialAttenuation = 1.0f;
    channel->loopsRemaining = settings.loopCount;
    channel->delayRemaining = settings.delay;
    
    if (settings.delay > 0.0f) {
        channel->state = AudioState::Stopped;
    } else if (settings.fadeInTime > 0.0f) {
        channel->state = AudioState::FadingIn;
        channel->fadeVolume = 0.0f;
        channel->fadeTarget = settings.volume;
        channel->fadeSpeed = settings.volume / settings.fadeInTime;
    } else {
        channel->state = AudioState::Playing;
        channel->fadeVolume = 1.0f;
    }

    SDL_UnlockAudioDevice(m_impl->deviceId);
    
    return channel->id;
}

ChannelId AudioSystem::Play(const std::string& name, const PlaybackSettings& settings) {
    SoundId id = FindSound(name);
    if (id == INVALID_SOUND_ID) return INVALID_CHANNEL_ID;
    return Play(id, settings);
}

ChannelId AudioSystem::PlaySpatial(SoundId soundId, const SpatialSettings& spatial, 
                                   const PlaybackSettings& settings) {
    ChannelId channel = Play(soundId, settings);
    if (channel != INVALID_CHANNEL_ID) {
        auto* ch = m_impl->GetChannel(channel);
        if (ch) {
            ch->spatial = spatial;
        }
    }
    return channel;
}

ChannelId AudioSystem::PlayMusic(SoundId soundId, const PlaybackSettings& settings) {
    PlaybackSettings musicSettings = settings;
    musicSettings.loop = true;
    return Play(soundId, musicSettings);
}

// ==============================================================================
// التحكم بالقنوات / Channel Control
// ==============================================================================

void AudioSystem::Stop(ChannelId channel) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch) {
        SDL_LockAudioDevice(m_impl->deviceId);
        ch->state = AudioState::Stopped;
        ch->id = INVALID_CHANNEL_ID;
        ch->samplePosition = 0;
        SDL_UnlockAudioDevice(m_impl->deviceId);
    }
}

void AudioSystem::StopAll() {
    SDL_LockAudioDevice(m_impl->deviceId);
    for (auto& ch : m_impl->channels) {
        if (ch.state != AudioState::Stopped) {
            ch.state = AudioState::Stopped;
            ch.id = INVALID_CHANNEL_ID;
            ch.samplePosition = 0;
        }
    }
    SDL_UnlockAudioDevice(m_impl->deviceId);
}

void AudioSystem::Pause(ChannelId channel) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch && ch->state == AudioState::Playing) {
        ch->state = AudioState::Paused;
    }
}

void AudioSystem::PauseAll() {
    for (auto& ch : m_impl->channels) {
        if (ch.state == AudioState::Playing) {
            ch.state = AudioState::Paused;
        }
    }
}

void AudioSystem::Resume(ChannelId channel) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch && ch->state == AudioState::Paused) {
        ch->state = AudioState::Playing;
    }
}

void AudioSystem::ResumeAll() {
    for (auto& ch : m_impl->channels) {
        if (ch.state == AudioState::Paused) {
            ch.state = AudioState::Playing;
        }
    }
}

void AudioSystem::FadeOut(ChannelId channel, Float32 duration) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch && ch->state == AudioState::Playing) {
        ch->state = AudioState::FadingOut;
        ch->fadeTarget = 0.0f;
        ch->fadeSpeed = ch->fadeVolume / duration;
    }
}

void AudioSystem::FadeIn(ChannelId channel, Float32 targetVolume, Float32 duration) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch) {
        ch->state = AudioState::FadingIn;
        ch->fadeTarget = targetVolume;
        ch->fadeSpeed = (targetVolume - ch->fadeVolume) / duration;
    }
}

void AudioSystem::CrossFade(ChannelId from, ChannelId to, Float32 duration) {
    FadeOut(from, duration);
    FadeIn(to, 1.0f, duration);
}

// ==============================================================================
// إعدادات القنوات / Channel Settings
// ==============================================================================

void AudioSystem::SetVolume(ChannelId channel, Float32 volume) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch) {
        ch->settings.volume = std::max(0.0f, std::min(1.0f, volume));
    }
}

Float32 AudioSystem::GetVolume(ChannelId channel) const {
    auto* ch = const_cast<AudioSystem*>(this)->m_impl->GetChannel(channel);
    return ch ? ch->settings.volume : 0.0f;
}

void AudioSystem::SetPitch(ChannelId channel, Float32 pitch) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch) {
        ch->settings.pitch = std::max(0.1f, std::min(4.0f, pitch));
    }
}

void AudioSystem::SetPan(ChannelId channel, Float32 pan) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch) {
        ch->settings.pan = std::max(-1.0f, std::min(1.0f, pan));
    }
}

void AudioSystem::SetPosition(ChannelId channel, Float32 seconds) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch) {
        ch->currentPosition = std::max(0.0f, seconds);

        // تحديث موقع العينة / Update sample position to match
        int bytesPerSample = SDL_AUDIO_BITSIZE(m_impl->obtainedSpec.format) / 8;
        int frameSize = bytesPerSample * m_impl->obtainedSpec.channels;
        if (frameSize > 0 && m_impl->deviceId != 0) {
            SDL_LockAudioDevice(m_impl->deviceId);
            ch->samplePosition = static_cast<Uint32>(
                ch->currentPosition * m_impl->obtainedSpec.freq * frameSize);
            SDL_UnlockAudioDevice(m_impl->deviceId);
        }
    }
}

Float32 AudioSystem::GetPosition(ChannelId channel) const {
    auto* ch = const_cast<AudioSystem*>(this)->m_impl->GetChannel(channel);
    return ch ? ch->currentPosition : 0.0f;
}

bool AudioSystem::IsPlaying(ChannelId channel) const {
    auto* ch = const_cast<AudioSystem*>(this)->m_impl->GetChannel(channel);
    return ch && (ch->state == AudioState::Playing || 
                  ch->state == AudioState::FadingIn ||
                  ch->state == AudioState::FadingOut);
}

AudioState AudioSystem::GetState(ChannelId channel) const {
    auto* ch = const_cast<AudioSystem*>(this)->m_impl->GetChannel(channel);
    return ch ? ch->state : AudioState::Stopped;
}

ChannelInfo AudioSystem::GetChannelInfo(ChannelId channel) const {
    ChannelInfo info;
    auto* ch = const_cast<AudioSystem*>(this)->m_impl->GetChannel(channel);
    if (ch) {
        info.id = ch->id;
        info.soundId = ch->soundId;
        info.state = ch->state;
        info.volume = ch->settings.volume * ch->fadeVolume;
        info.pitch = ch->settings.pitch;
        info.pan = ch->settings.pan;
        info.position = ch->currentPosition;
    }
    return info;
}

// ==============================================================================
// المجموعات الصوتية / Audio Groups
// ==============================================================================

AudioGroupId AudioSystem::CreateGroup(const std::string& name) {
    AudioGroup group;
    group.id = m_nextGroupId++;
    group.name = name;
    
    m_groups[group.id] = group;
    m_groupsByName[name] = group.id;
    
    return group.id;
}

void AudioSystem::SetGroupVolume(AudioGroupId group, Float32 volume) {
    auto it = m_groups.find(group);
    if (it != m_groups.end()) {
        it->second.volume = std::max(0.0f, std::min(1.0f, volume));
    }
}

void AudioSystem::MuteGroup(AudioGroupId group, bool muted) {
    auto it = m_groups.find(group);
    if (it != m_groups.end()) {
        it->second.muted = muted;
    }
}

void AudioSystem::PauseGroup(AudioGroupId group, bool paused) {
    auto it = m_groups.find(group);
    if (it != m_groups.end()) {
        it->second.paused = paused;
        
        for (auto& ch : m_impl->channels) {
            if (ch.settings.group == group) {
                if (paused && ch.state == AudioState::Playing) {
                    ch.state = AudioState::Paused;
                } else if (!paused && ch.state == AudioState::Paused) {
                    ch.state = AudioState::Playing;
                }
            }
        }
    }
}

void AudioSystem::StopGroup(AudioGroupId group) {
    for (auto& ch : m_impl->channels) {
        if (ch.settings.group == group && ch.state != AudioState::Stopped) {
            ch.state = AudioState::Stopped;
            ch.id = INVALID_CHANNEL_ID;
        }
    }
}

const AudioGroup* AudioSystem::GetGroup(AudioGroupId groupId) const {
    auto it = m_groups.find(groupId);
    return (it != m_groups.end()) ? &it->second : nullptr;
}

AudioGroupId AudioSystem::FindGroup(const std::string& name) const {
    auto it = m_groupsByName.find(name);
    return (it != m_groupsByName.end()) ? it->second : 0;
}

// ==============================================================================
// التحكم العام / Global Control
// ==============================================================================

void AudioSystem::SetMasterVolume(Float32 volume) {
    m_masterVolume = std::max(0.0f, std::min(1.0f, volume));
}

void AudioSystem::SetMasterMute(bool muted) {
    m_masterMuted = muted;
}

// ==============================================================================
// الصوت المكاني / Spatial Audio
// ==============================================================================

void AudioSystem::SetListenerPosition(Float32 x, Float32 y) {
    m_listenerPosition = Vec2(x, y);
}

void AudioSystem::SetListenerPosition(const Vec2& position) {
    m_listenerPosition = position;
}

void AudioSystem::SetSourcePosition(ChannelId channel, Float32 x, Float32 y) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch && ch->spatial.enabled) {
        ch->spatial.position = Vec2(x, y);
    }
}

void AudioSystem::SetSourcePosition(ChannelId channel, const Vec2& position) {
    SetSourcePosition(channel, position.x, position.y);
}

// ==============================================================================
// الاستدعاءات / Callbacks
// ==============================================================================

void AudioSystem::SetOnComplete(ChannelId channel, AudioCompleteCallback callback) {
    auto* ch = m_impl->GetChannel(channel);
    if (ch) {
        ch->onComplete = callback;
    }
}

void AudioSystem::SetGlobalOnComplete(AudioCompleteCallback callback) {
    m_globalOnComplete = callback;
}

// ==============================================================================
// التحديث / Update
// ==============================================================================

void AudioSystem::Update(Float32 deltaTime) {
    if (!m_initialized) return;
    
    for (auto& ch : m_impl->channels) {
        if (ch.state == AudioState::Stopped) continue;
        
        // معالجة التأخير / Handle delay
        if (ch.delayRemaining > 0.0f) {
            ch.delayRemaining -= deltaTime;
            if (ch.delayRemaining <= 0.0f) {
                ch.state = AudioState::Playing;
            }
            continue;
        }
        
        // معالجة التلاشي / Handle fading
        if (ch.state == AudioState::FadingIn) {
            ch.fadeVolume += ch.fadeSpeed * deltaTime;
            if (ch.fadeVolume >= ch.fadeTarget) {
                ch.fadeVolume = ch.fadeTarget;
                ch.state = AudioState::Playing;
            }
        } else if (ch.state == AudioState::FadingOut) {
            ch.fadeVolume -= ch.fadeSpeed * deltaTime;
            if (ch.fadeVolume <= 0.0f) {
                ch.fadeVolume = 0.0f;
                ch.state = AudioState::Stopped;
                ch.id = INVALID_CHANNEL_ID;
                
                // استدعاء عند الانتهاء / Call on complete
                if (ch.onComplete) {
                    ch.onComplete(ch.id, ch.soundId);
                }
                if (m_globalOnComplete) {
                    m_globalOnComplete(ch.id, ch.soundId);
                }
                continue;
            }
        }
        
        // تحديث موقع التشغيل / Update playback position
        if (ch.state == AudioState::Playing || ch.state == AudioState::FadingIn || 
            ch.state == AudioState::FadingOut) {
            ch.currentPosition += deltaTime * ch.settings.pitch;
            
            // التحقق من انتهاء المقطع / Check if clip ended
            auto* soundInfo = GetSoundInfo(ch.soundId);
            if (soundInfo && ch.currentPosition >= soundInfo->duration) {
                if (ch.settings.loop) {
                    ch.currentPosition = 0.0f;
                    if (ch.loopsRemaining > 0) {
                        ch.loopsRemaining--;
                        if (ch.loopsRemaining == 0) {
                            ch.settings.loop = false;
                        }
                    }
                } else {
                    ch.state = AudioState::Stopped;
                    ch.id = INVALID_CHANNEL_ID;
                    
                    if (ch.onComplete) {
                        ch.onComplete(ch.id, ch.soundId);
                    }
                    if (m_globalOnComplete) {
                        m_globalOnComplete(ch.id, ch.soundId);
                    }
                }
            }
        }
        
        // حساب الصوت المكاني / Calculate spatial audio
        if (ch.spatial.enabled) {
            Float32 dx = ch.spatial.position.x - m_listenerPosition.x;
            Float32 dy = ch.spatial.position.y - m_listenerPosition.y;
            Float32 distance = std::sqrt(dx * dx + dy * dy);
            
            // حساب التخفيف / Calculate attenuation
            Float32 attenuation = 1.0f;
            if (distance > ch.spatial.minDistance) {
                if (distance >= ch.spatial.maxDistance) {
                    attenuation = 0.0f;
                } else {
                    Float32 range = ch.spatial.maxDistance - ch.spatial.minDistance;
                    attenuation = 1.0f - std::pow((distance - ch.spatial.minDistance) / range, 
                                                   ch.spatial.rolloff);
                    attenuation = std::max(0.0f, attenuation);
                }
            }
            
            // حساب التوازن / Calculate panning
            Float32 pan = 0.0f;
            if (distance > 0.001f) {
                pan = dx / distance;
                pan = std::max(-1.0f, std::min(1.0f, pan));
            }
            
            // تطبيق التخفيف والتوازن / Apply attenuation and panning
            ch.spatialAttenuation = attenuation;
            ch.settings.pan = pan;
        }
    }
}

// ==============================================================================
// الإحصائيات / Statistics
// ==============================================================================

Int32 AudioSystem::GetLoadedSoundCount() const {
    return static_cast<Int32>(m_sounds.size());
}

Int32 AudioSystem::GetActiveChannelCount() const {
    Int32 count = 0;
    for (const auto& ch : m_impl->channels) {
        if (ch.state != AudioState::Stopped) count++;
    }
    return count;
}

// ==============================================================================
// النمط الواحد / Singleton
// ==============================================================================

AudioSystem& AudioSystem::GetInstance() {
    static AudioSystem instance;
    return instance;
}

} // namespace graphics
} // namespace sad
