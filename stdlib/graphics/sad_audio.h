// ═══════════════════════════════════════════════════════════════════════════════
// نظام الصوت والموسيقى — لغة ص
// Audio & Sound System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام صوت مدمج — Flutter يحتاج لمكتبات خارجية (audioplayers, just_audio)!
// هنا كل شيء مدمج وجاهز بسطر واحد.
//
// الميزات:
//   🔊 توليد أصوات إجرائية (Procedural Sound) — موجة جيبية، مربعة، مثلثية
//   🎵 تشغيل نغمات موسيقية (بالتردد أو اسم النوتة)
//   🥁 مولّد إيقاعات (Beat Generator) — أنماط إيقاعية جاهزة
//   🎚️ التحكم بالصوت — مستوى، تلاشي، إيقاف مؤقت
//   🔔 أصوات واجهة المستخدم — نقرة، تنبيه، نجاح، خطأ
//   📢 أصوات أذان إسلامية — تنبيه لأوقات الصلاة
//
// الآلية:
//   يستخدم SDL2 Audio مباشرة لتوليد الأصوات
//   بدون ملفات صوتية خارجية — كل شيء يُولَّد برمجياً
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace sad { namespace audio {

// ═══════════════════════════════════════════════════════════════════
// أنواع الموجات الصوتية — أساس توليد الصوت
// ═══════════════════════════════════════════════════════════════════

// نوع الموجة الصوتية — كل نوع له طابع صوتي مختلف
enum class WaveType {
    Sine,       // موجة جيبية — صوت ناعم ونقي (مثل الصفارة)
    Square,     // موجة مربعة — صوت حاد (مثل ألعاب الأتاري)
    Triangle,   // موجة مثلثية — وسط بين الجيبية والمربعة
    Sawtooth,   // موجة منشارية — صوت غني بالتوافقيات
    Noise       // ضوضاء بيضاء — صوت عشوائي (للمؤثرات)
};

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات
// ═══════════════════════════════════════════════════════════════════

// نغمة موسيقية واحدة
struct Tone {
    float frequency = 440.0f;     // التردد بالهرتز (440 = لا / A4)
    float duration = 0.5f;        // المدة بالثواني
    float volume = 0.5f;          // مستوى الصوت (0.0 إلى 1.0)
    WaveType waveType = WaveType::Sine;  // نوع الموجة
};

// ─── أصوات واجهة المستخدم الجاهزة ───
enum class UISound {
    Click,      // نقرة زر — صوت قصير
    Hover,      // تمرير فوق عنصر
    Success,    // نجاح عملية — صوت مبهج
    Error,      // خطأ — صوت تنبيهي
    Warning,    // تحذير
    Notification, // إشعار
    KeyPress,   // ضغطة مفتاح
    Swoosh      // انتقال بين صفحات
};

// نمط إيقاعي — سلسلة من النبضات
struct BeatPattern {
    std::vector<bool> beats;      // true = نبضة، false = صمت
    float bpm = 120.0f;           // عدد النبضات في الدقيقة
    float volume = 0.5f;          // مستوى الصوت
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── تهيئة وإنهاء نظام الصوت ───
bool initAudio(int sampleRate = 44100, int channels = 1);
void shutdownAudio();

// ─── تشغيل نغمة ───
// تشغيل نغمة بتردد ومدة وحجم ونوع موجة
void playTone(float frequency, float duration = 0.3f, float volume = 0.5f, int waveType = 0);

// تشغيل نوتة موسيقية بالاسم (C4, D4, E4, F4, G4, A4, B4, ...)
void playNote(const std::string& note, float duration = 0.3f, float volume = 0.5f);

// تشغيل سلسلة نغمات (لحن بسيط)
void playMelody(const std::vector<float>& frequencies, float noteLength = 0.2f, float volume = 0.5f);

// ─── أصوات واجهة المستخدم الجاهزة ───
void playUISound(int soundType);
void playClick();
void playSuccess();
void playError();
void playWarning();
void playNotification();

// ─── مؤثرات صوتية خاصة ───
// صوت صاعد (مثل تجميع نقاط في لعبة)
void playSweepUp(float startFreq, float endFreq, float duration = 0.3f);

// صوت هابط (مثل خسارة)
void playSweepDown(float startFreq, float endFreq, float duration = 0.3f);

// صوت ارتداد (bounce)
void playBounce();

// صوت انفجار (explosion)
void playExplosionSound();

// ─── الإيقاعات ───
// بدء نمط إيقاعي
int startBeat(float bpm, int pattern = 0);

// إيقاف الإيقاع
void stopBeat(int beatId);

// ─── التحكم العام ───
void setMasterVolume(float volume);
float getMasterVolume();
void stopAllSounds();
bool isPlaying();

}} // namespace sad::audio
