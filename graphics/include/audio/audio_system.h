// ==============================================================================
// audio_system.h - نظام الصوت / Audio System
// ==============================================================================
// الوصف: نظام صوت متكامل يدعم تشغيل المؤثرات الصوتية والموسيقى
// Description: Complete audio system supporting sound effects and music playback
// ==============================================================================
// المميزات / Features:
//   - تشغيل المؤثرات الصوتية / Sound effects playback
//   - تشغيل الموسيقى الخلفية / Background music
//   - التحكم بالصوت / Volume control
//   - المجموعات الصوتية / Audio groups
//   - الصوت ثلاثي الأبعاد / 3D positional audio
//   - نظام الأولويات / Priority system
// ==============================================================================

#ifndef SAD_GRAPHICS_AUDIO_SYSTEM_H
#define SAD_GRAPHICS_AUDIO_SYSTEM_H

#include "../core/types.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

namespace sad {
namespace graphics {

// ==============================================================================
// معرّفات الصوت / Audio Identifiers
// ==============================================================================

/// معرّف المقطع الصوتي / Sound clip identifier
using SoundId = UInt32;

/// معرّف القناة / Channel identifier  
using ChannelId = UInt32;

/// معرّف المجموعة / Group identifier
using AudioGroupId = UInt32;

// ==============================================================================
// ثوابت نظام الصوت / Audio System Constants
// ==============================================================================

/// معرّف غير صالح / Invalid identifier
constexpr SoundId INVALID_SOUND_ID = 0;
constexpr ChannelId INVALID_CHANNEL_ID = 0;

/// الحد الأقصى للقنوات المتزامنة / Maximum simultaneous channels
constexpr Int32 MAX_AUDIO_CHANNELS = 32;

/// الحد الأقصى للمجموعات / Maximum audio groups
constexpr Int32 MAX_AUDIO_GROUPS = 16;

// ==============================================================================
// حالة الصوت / Audio State
// ==============================================================================

/// حالة تشغيل الصوت / Playback state
enum class AudioState {
    Stopped,        // متوقف / Stopped
    Playing,        // قيد التشغيل / Playing
    Paused,         // مؤقت / Paused
    FadingIn,       // يزداد / Fading in
    FadingOut       // يتلاشى / Fading out
};

// ==============================================================================
// إعدادات تشغيل الصوت / Playback Settings
// ==============================================================================

/// إعدادات تشغيل المقطع الصوتي / Sound playback settings
struct PlaybackSettings {
    Float32 volume = 1.0f;          // مستوى الصوت (0-1) / Volume level (0-1)
    Float32 pitch = 1.0f;           // طبقة الصوت / Pitch
    Float32 pan = 0.0f;             // التوازن (-1 يسار، 1 يمين) / Pan (-1 left, 1 right)
    bool loop = false;              // تكرار / Loop
    Int32 loopCount = -1;           // عدد التكرارات (-1 = لانهائي) / Loop count (-1 = infinite)
    Int32 priority = 0;             // الأولوية / Priority
    AudioGroupId group = 0;         // المجموعة / Group
    Float32 fadeInTime = 0.0f;      // وقت الظهور التدريجي / Fade in time
    Float32 delay = 0.0f;           // التأخير / Delay before playing
    
    /// الإعدادات الافتراضية / Default settings
    static PlaybackSettings Default() { return PlaybackSettings(); }
    
    /// إعدادات التكرار / Loop settings
    static PlaybackSettings Looped() {
        PlaybackSettings s;
        s.loop = true;
        return s;
    }
};

// ==============================================================================
// إعدادات الصوت المكاني (ثلاثي الأبعاد) / Spatial Audio Settings
// ==============================================================================

/// إعدادات الصوت المكاني / Spatial audio settings
struct SpatialSettings {
    Vec2 position;                  // موقع مصدر الصوت / Sound source position
    Float32 minDistance = 100.0f;   // المسافة الدنيا / Minimum distance
    Float32 maxDistance = 1000.0f;  // المسافة القصوى / Maximum distance
    Float32 rolloff = 1.0f;         // معامل التخفيف / Rolloff factor
    bool enabled = false;           // مفعّل / Enabled
    
    /// تفعيل الصوت المكاني / Enable spatial audio
    static SpatialSettings At(Float32 x, Float32 y) {
        SpatialSettings s;
        s.position = Vec2(x, y);
        s.enabled = true;
        return s;
    }
};

// ==============================================================================
// معلومات المقطع الصوتي / Sound Clip Info
// ==============================================================================

/// معلومات المقطع الصوتي المحمّل / Loaded sound clip info
struct SoundClipInfo {
    SoundId id = INVALID_SOUND_ID;
    std::string name;               // اسم المقطع / Clip name
    std::string filePath;           // مسار الملف / File path
    Float32 duration = 0.0f;        // المدة بالثواني / Duration in seconds
    Int32 channels = 0;             // عدد القنوات / Channel count
    Int32 sampleRate = 0;           // معدل العينات / Sample rate
    bool isStream = false;          // هل هو تدفق / Is streaming
};

// ==============================================================================
// معلومات القناة / Channel Info
// ==============================================================================

/// معلومات القناة الصوتية / Audio channel info
struct ChannelInfo {
    ChannelId id = INVALID_CHANNEL_ID;
    SoundId soundId = INVALID_SOUND_ID;
    AudioState state = AudioState::Stopped;
    Float32 volume = 1.0f;
    Float32 pitch = 1.0f;
    Float32 pan = 0.0f;
    Float32 position = 0.0f;        // الموقع الحالي بالثواني / Current position in seconds
    bool isMusic = false;           // هل هي موسيقى / Is music
};

// ==============================================================================
// المجموعة الصوتية / Audio Group
// ==============================================================================

/// مجموعة صوتية للتحكم المجمّع / Audio group for batch control
struct AudioGroup {
    AudioGroupId id = 0;
    std::string name;
    Float32 volume = 1.0f;          // مستوى صوت المجموعة / Group volume
    bool muted = false;             // هل المجموعة صامتة / Is muted
    bool paused = false;            // هل المجموعة متوقفة / Is paused
};

// ==============================================================================
// أنواع الاستدعاءات / Callback Types
// ==============================================================================

using AudioCallback = std::function<void(ChannelId channel)>;
using AudioCompleteCallback = std::function<void(ChannelId channel, SoundId soundId)>;

// ==============================================================================
// نظام الصوت الرئيسي / Main Audio System
// ==============================================================================

/// نظام الصوت الرئيسي / Main audio system class
class AudioSystem {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    /// مُنشئ افتراضي / Default constructor
    AudioSystem();
    
    /// المدمر / Destructor
    ~AudioSystem();
    
    /// منع النسخ / Prevent copying
    AudioSystem(const AudioSystem&) = delete;
    AudioSystem& operator=(const AudioSystem&) = delete;
    
    // ============================================================================
    // التهيئة والإغلاق / Initialization and Shutdown
    // ============================================================================
    
    /// تهيئة نظام الصوت / Initialize audio system
    /// @param sampleRate معدل العينات / Sample rate (default: 44100)
    /// @param channels عدد القنوات / Channel count (default: 2 stereo)
    /// @param bufferSize حجم المخزن المؤقت / Buffer size (default: 2048)
    /// @return نجاح التهيئة / Success
    bool Initialize(Int32 sampleRate = 44100, Int32 channels = 2, Int32 bufferSize = 2048);
    
    /// إغلاق نظام الصوت / Shutdown audio system
    void Shutdown();
    
    /// هل النظام مُهيأ / Is system initialized
    bool IsInitialized() const { return m_initialized; }
    
    // ============================================================================
    // تحميل الأصوات / Loading Sounds
    // ============================================================================
    
    /// تحميل مقطع صوتي / Load sound clip
    /// @param filePath مسار الملف / File path
    /// @param name اسم اختياري / Optional name
    /// @return معرّف المقطع / Sound ID
    SoundId LoadSound(const std::string& filePath, const std::string& name = "");
    
    /// تحميل موسيقى (تدفق) / Load music (streaming)
    /// @param filePath مسار الملف / File path
    /// @param name اسم اختياري / Optional name
    /// @return معرّف المقطع / Sound ID
    SoundId LoadMusic(const std::string& filePath, const std::string& name = "");
    
    /// تحرير مقطع صوتي / Unload sound clip
    /// @param soundId معرّف المقطع / Sound ID
    void UnloadSound(SoundId soundId);
    
    /// تحرير جميع الأصوات / Unload all sounds
    void UnloadAllSounds();
    
    /// الحصول على معلومات المقطع / Get sound info
    const SoundClipInfo* GetSoundInfo(SoundId soundId) const;
    
    /// البحث عن مقطع بالاسم / Find sound by name
    SoundId FindSound(const std::string& name) const;
    
    // ============================================================================
    // التشغيل / Playback
    // ============================================================================
    
    /// تشغيل مقطع صوتي / Play sound
    /// @param soundId معرّف المقطع / Sound ID
    /// @param settings إعدادات التشغيل / Playback settings
    /// @return معرّف القناة / Channel ID
    ChannelId Play(SoundId soundId, const PlaybackSettings& settings = PlaybackSettings::Default());
    
    /// تشغيل مقطع بالاسم / Play sound by name
    ChannelId Play(const std::string& name, const PlaybackSettings& settings = PlaybackSettings::Default());
    
    /// تشغيل صوت مكاني / Play spatial sound
    ChannelId PlaySpatial(SoundId soundId, const SpatialSettings& spatial, 
                          const PlaybackSettings& settings = PlaybackSettings::Default());
    
    /// تشغيل موسيقى / Play music
    ChannelId PlayMusic(SoundId soundId, const PlaybackSettings& settings = PlaybackSettings::Looped());
    
    // ============================================================================
    // التحكم بالقنوات / Channel Control
    // ============================================================================
    
    /// إيقاف قناة / Stop channel
    void Stop(ChannelId channel);
    
    /// إيقاف جميع القنوات / Stop all channels
    void StopAll();
    
    /// إيقاف مؤقت للقناة / Pause channel
    void Pause(ChannelId channel);
    
    /// إيقاف مؤقت للجميع / Pause all
    void PauseAll();
    
    /// استئناف قناة / Resume channel
    void Resume(ChannelId channel);
    
    /// استئناف الجميع / Resume all
    void ResumeAll();
    
    /// التلاشي التدريجي / Fade out
    void FadeOut(ChannelId channel, Float32 duration);
    
    /// الظهور التدريجي / Fade in
    void FadeIn(ChannelId channel, Float32 targetVolume, Float32 duration);
    
    /// التبديل التدريجي بين مقطعين / Cross fade
    void CrossFade(ChannelId from, ChannelId to, Float32 duration);
    
    // ============================================================================
    // إعدادات القنوات / Channel Settings
    // ============================================================================
    
    /// تعيين مستوى صوت القناة / Set channel volume
    void SetVolume(ChannelId channel, Float32 volume);
    
    /// الحصول على مستوى صوت القناة / Get channel volume
    Float32 GetVolume(ChannelId channel) const;
    
    /// تعيين طبقة الصوت / Set pitch
    void SetPitch(ChannelId channel, Float32 pitch);
    
    /// تعيين التوازن / Set pan
    void SetPan(ChannelId channel, Float32 pan);
    
    /// تعيين موقع التشغيل / Set playback position
    void SetPosition(ChannelId channel, Float32 seconds);
    
    /// الحصول على موقع التشغيل / Get playback position
    Float32 GetPosition(ChannelId channel) const;
    
    /// هل القناة قيد التشغيل / Is channel playing
    bool IsPlaying(ChannelId channel) const;
    
    /// الحصول على حالة القناة / Get channel state
    AudioState GetState(ChannelId channel) const;
    
    /// الحصول على معلومات القناة / Get channel info
    ChannelInfo GetChannelInfo(ChannelId channel) const;
    
    // ============================================================================
    // المجموعات الصوتية / Audio Groups
    // ============================================================================
    
    /// إنشاء مجموعة صوتية / Create audio group
    AudioGroupId CreateGroup(const std::string& name);
    
    /// تعيين مستوى صوت المجموعة / Set group volume
    void SetGroupVolume(AudioGroupId group, Float32 volume);
    
    /// كتم المجموعة / Mute group
    void MuteGroup(AudioGroupId group, bool muted = true);
    
    /// إيقاف مؤقت للمجموعة / Pause group
    void PauseGroup(AudioGroupId group, bool paused = true);
    
    /// إيقاف المجموعة / Stop group
    void StopGroup(AudioGroupId group);
    
    /// الحصول على معلومات المجموعة / Get group info
    const AudioGroup* GetGroup(AudioGroupId groupId) const;
    
    /// البحث عن مجموعة بالاسم / Find group by name
    AudioGroupId FindGroup(const std::string& name) const;
    
    // ============================================================================
    // التحكم العام / Global Control
    // ============================================================================
    
    /// تعيين مستوى الصوت الرئيسي / Set master volume
    void SetMasterVolume(Float32 volume);
    
    /// الحصول على مستوى الصوت الرئيسي / Get master volume
    Float32 GetMasterVolume() const { return m_masterVolume; }
    
    /// كتم الصوت الرئيسي / Mute master
    void SetMasterMute(bool muted);
    
    /// هل الصوت مكتوم / Is master muted
    bool IsMasterMuted() const { return m_masterMuted; }
    
    // ============================================================================
    // الصوت المكاني / Spatial Audio
    // ============================================================================
    
    /// تعيين موقع المستمع / Set listener position
    void SetListenerPosition(Float32 x, Float32 y);
    void SetListenerPosition(const Vec2& position);
    
    /// الحصول على موقع المستمع / Get listener position
    Vec2 GetListenerPosition() const { return m_listenerPosition; }
    
    /// تحديث موقع مصدر الصوت / Update sound source position
    void SetSourcePosition(ChannelId channel, Float32 x, Float32 y);
    void SetSourcePosition(ChannelId channel, const Vec2& position);
    
    // ============================================================================
    // الاستدعاءات / Callbacks
    // ============================================================================
    
    /// تعيين استدعاء عند انتهاء التشغيل / Set completion callback
    void SetOnComplete(ChannelId channel, AudioCompleteCallback callback);
    
    /// تعيين استدعاء عام عند انتهاء أي صوت / Set global completion callback
    void SetGlobalOnComplete(AudioCompleteCallback callback);
    
    // ============================================================================
    // التحديث / Update
    // ============================================================================
    
    /// تحديث نظام الصوت (يجب استدعاؤه كل إطار)
    /// Update audio system (must be called every frame)
    void Update(Float32 deltaTime);
    
    // ============================================================================
    // الإحصائيات / Statistics
    // ============================================================================
    
    /// عدد الأصوات المحملة / Loaded sound count
    Int32 GetLoadedSoundCount() const;
    
    /// عدد القنوات النشطة / Active channel count
    Int32 GetActiveChannelCount() const;
    
    /// الحصول على نمط واحد / Get singleton instance
    static AudioSystem& GetInstance();

private:
    // ============================================================================
    // البيانات الداخلية / Internal Data
    // ============================================================================
    
    bool m_initialized = false;
    Float32 m_masterVolume = 1.0f;
    bool m_masterMuted = false;
    Vec2 m_listenerPosition;
    
    /// بيانات التنفيذ الخاصة / Private implementation data
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    AudioCompleteCallback m_globalOnComplete;
    
    /// معرّف تالي للأصوات / Next sound ID
    SoundId m_nextSoundId = 1;
    
    /// معرّف تالي للقنوات / Next channel ID
    ChannelId m_nextChannelId = 1;
    
    /// معرّف تالي للمجموعات / Next group ID
    AudioGroupId m_nextGroupId = 1;
    
    /// خريطة المقاطع الصوتية / Sound clips map
    std::unordered_map<SoundId, SoundClipInfo> m_sounds;
    std::unordered_map<std::string, SoundId> m_soundsByName;
    
    /// خريطة المجموعات / Groups map
    std::unordered_map<AudioGroupId, AudioGroup> m_groups;
    std::unordered_map<std::string, AudioGroupId> m_groupsByName;
};

// ==============================================================================
// دوال مساعدة عامة / Global Helper Functions
// ==============================================================================

/// تشغيل صوت سريع باستخدام النمط الواحد
/// Quick play using singleton
inline ChannelId PlaySound(SoundId soundId, Float32 volume = 1.0f) {
    PlaybackSettings settings;
    settings.volume = volume;
    return AudioSystem::GetInstance().Play(soundId, settings);
}

/// تشغيل صوت بالاسم
/// Play sound by name
inline ChannelId PlaySound(const std::string& name, Float32 volume = 1.0f) {
    PlaybackSettings settings;
    settings.volume = volume;
    return AudioSystem::GetInstance().Play(name, settings);
}

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_AUDIO_SYSTEM_H
