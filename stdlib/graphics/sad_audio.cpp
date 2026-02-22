// ═══════════════════════════════════════════════════════════════════════════════
// نظام الصوت والموسيقى — لغة ص (التنفيذ)
// Audio & Sound System — SAD Language (Implementation)
// ═══════════════════════════════════════════════════════════════════════════════
//
// توليد صوت إجرائي بالكامل — بدون ملفات WAV خارجية
// يستخدم SDL2 Audio لتوليد وتشغيل الموجات الصوتية مباشرة
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_audio.h"

#include <SDL.h>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include <random>

// ─── تعريف الثابت π إذا لم يكن متوفراً ───
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace sad { namespace audio {

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة للنظام — متغيرات عامة داخلية
// ═══════════════════════════════════════════════════════════════════

static SDL_AudioDeviceID g_audioDevice = 0;   // معرّف جهاز الصوت
static int g_sampleRate = 44100;              // معدل العينات (هرتز)
static float g_masterVolume = 0.7f;           // مستوى الصوت الرئيسي
static bool g_initialized = false;            // هل تم تهيئة النظام؟

// مولّد أرقام عشوائية للضوضاء البيضاء
static std::mt19937 g_rng(std::random_device{}());
static std::uniform_int_distribution<int16_t> g_noiseDist(-16000, 16000);

// ═══════════════════════════════════════════════════════════════════
// تهيئة وإنهاء نظام الصوت
// ═══════════════════════════════════════════════════════════════════

// تهيئة نظام الصوت — يجب استدعاؤها قبل أي عملية صوتية
bool initAudio(int sampleRate, int channels) {
    // إذا كان مُهيّأ بالفعل، نرجع النجاح
    if (g_initialized) return true;

    // تهيئة نظام الصوت الفرعي في SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return false;
    }

    g_sampleRate = sampleRate;

    // إعداد مواصفات الصوت المطلوبة
    SDL_AudioSpec desired;
    SDL_memset(&desired, 0, sizeof(desired));
    desired.freq = sampleRate;            // معدل العينات
    desired.format = AUDIO_S16SYS;        // تنسيق 16-بت بإشارة
    desired.channels = (Uint8)channels;   // عدد القنوات (أحادي افتراضياً)
    desired.samples = 2048;               // حجم المخزن المؤقت
    desired.callback = nullptr;           // نستخدم SDL_QueueAudio بدل callback

    SDL_AudioSpec obtained;

    // فتح جهاز الصوت
    g_audioDevice = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (g_audioDevice == 0) {
        return false;
    }

    // بدء تشغيل الجهاز (إلغاء الإيقاف المؤقت)
    SDL_PauseAudioDevice(g_audioDevice, 0);

    g_initialized = true;
    return true;
}

// إنهاء نظام الصوت وتحرير الموارد
void shutdownAudio() {
    if (!g_initialized) return;

    // إغلاق جهاز الصوت
    if (g_audioDevice != 0) {
        SDL_CloseAudioDevice(g_audioDevice);
        g_audioDevice = 0;
    }

    // إنهاء النظام الفرعي للصوت
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    g_initialized = false;
}

// ═══════════════════════════════════════════════════════════════════
// توليد العينات الصوتية — قلب النظام
// ═══════════════════════════════════════════════════════════════════

// توليد عينات صوتية بموجة محددة مع غلاف ADSR
// المعاملات: التردد، المدة، مستوى الصوت، نوع الموجة
static std::vector<int16_t> generateSamples(float frequency, float duration,
                                             float volume, WaveType waveType) {
    // حساب عدد العينات المطلوبة
    int numSamples = static_cast<int>(g_sampleRate * duration);
    std::vector<int16_t> samples(numSamples);

    // ─── حساب نقاط غلاف الصوت ADSR ───
    // الهجوم (Attack): أول 5% — يرتفع الصوت تدريجياً
    int attackEnd   = static_cast<int>(numSamples * 0.05f);
    // الانحدار (Decay): التالية 10% — ينخفض قليلاً
    int decayEnd    = static_cast<int>(numSamples * 0.15f);
    // الاستدامة (Sustain): التالية 70% — مستوى ثابت
    int sustainEnd  = static_cast<int>(numSamples * 0.85f);
    // الإطلاق (Release): آخر 15% — يتلاشى الصوت
    // (من sustainEnd حتى numSamples)

    float sustainLevel = 0.8f; // مستوى الاستدامة

    for (int i = 0; i < numSamples; ++i) {
        float t = static_cast<float>(i);
        float sample = 0.0f;

        // ─── توليد شكل الموجة حسب النوع ───
        switch (waveType) {
            case WaveType::Sine: {
                // موجة جيبية: صوت ناعم ونقي
                sample = sinf(2.0f * (float)M_PI * frequency * t / g_sampleRate);
                break;
            }
            case WaveType::Square: {
                // موجة مربعة: إشارة ثنائية (+1 أو -1)
                float s = sinf(2.0f * (float)M_PI * frequency * t / g_sampleRate);
                sample = (s >= 0.0f) ? 1.0f : -1.0f;
                break;
            }
            case WaveType::Triangle: {
                // موجة مثلثية: خطية صاعدة ثم هابطة
                float phase = t * frequency / g_sampleRate;
                sample = 2.0f * fabsf(2.0f * (phase - floorf(phase + 0.5f))) - 1.0f;
                break;
            }
            case WaveType::Sawtooth: {
                // موجة منشارية: تصاعد خطي ثم قفز
                float phase = t * frequency / g_sampleRate;
                sample = 2.0f * (phase - floorf(phase)) - 1.0f;
                break;
            }
            case WaveType::Noise: {
                // ضوضاء بيضاء: قيم عشوائية
                sample = g_noiseDist(g_rng) / 16000.0f;
                break;
            }
        }

        // ─── تطبيق غلاف ADSR ───
        float envelope = 0.0f;
        if (i < attackEnd) {
            // مرحلة الهجوم — ارتفاع تدريجي من 0 إلى 1
            envelope = (attackEnd > 0) ? (float)i / attackEnd : 1.0f;
        } else if (i < decayEnd) {
            // مرحلة الانحدار — انخفاض من 1 إلى مستوى الاستدامة
            float decayLen = (float)(decayEnd - attackEnd);
            float decayPos = (float)(i - attackEnd);
            envelope = 1.0f - (1.0f - sustainLevel) * (decayPos / decayLen);
        } else if (i < sustainEnd) {
            // مرحلة الاستدامة — مستوى ثابت
            envelope = sustainLevel;
        } else {
            // مرحلة الإطلاق — تلاشي تدريجي إلى الصمت
            float releaseLen = (float)(numSamples - sustainEnd);
            float releasePos = (float)(i - sustainEnd);
            envelope = sustainLevel * (1.0f - releasePos / releaseLen);
        }

        // ─── تطبيق مستوى الصوت والغلاف وتحويل إلى int16 ───
        float finalSample = sample * volume * g_masterVolume * envelope;
        finalSample = std::clamp(finalSample, -1.0f, 1.0f);
        samples[i] = static_cast<int16_t>(finalSample * 32000.0f);
    }

    return samples;
}

// ═══════════════════════════════════════════════════════════════════
// تشغيل النغمات والأصوات
// ═══════════════════════════════════════════════════════════════════

// تشغيل نغمة واحدة بتردد ومدة ومستوى صوت ونوع موجة
void playTone(float frequency, float duration, float volume, int waveType) {
    if (!g_initialized) return;

    // تحويل الرقم الصحيح إلى نوع الموجة
    WaveType wt = WaveType::Sine;
    switch (waveType) {
        case 0: wt = WaveType::Sine;     break;
        case 1: wt = WaveType::Square;   break;
        case 2: wt = WaveType::Triangle; break;
        case 3: wt = WaveType::Sawtooth; break;
        case 4: wt = WaveType::Noise;    break;
        default: wt = WaveType::Sine;    break;
    }

    // توليد العينات وإرسالها لجهاز الصوت
    std::vector<int16_t> samples = generateSamples(frequency, duration, volume, wt);
    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// ═══════════════════════════════════════════════════════════════════
// تحويل اسم النوتة الموسيقية إلى تردد
// ═══════════════════════════════════════════════════════════════════

// جدول ترددات النوتات في الأوكتاف الرابع (الأساسي)
// C4=دو، D4=ري، E4=مي، F4=فا، G4=صول، A4=لا، B4=سي
static float noteNameToFrequency(const std::string& note) {
    if (note.empty()) return 440.0f;

    // استخراج اسم النوتة والأوكتاف
    char noteLetter = toupper(note[0]);
    int octave = 4; // الأوكتاف الافتراضي

    // البحث عن رقم الأوكتاف في النص
    for (size_t i = 1; i < note.size(); ++i) {
        if (note[i] >= '0' && note[i] <= '9') {
            octave = note[i] - '0';
            break;
        }
    }

    // ترددات النوتات الأساسية في الأوكتاف الرابع
    float baseFreq = 440.0f; // القيمة الافتراضية (لا)
    switch (noteLetter) {
        case 'C': baseFreq = 261.63f; break;  // دو
        case 'D': baseFreq = 293.66f; break;  // ري
        case 'E': baseFreq = 329.63f; break;  // مي
        case 'F': baseFreq = 349.23f; break;  // فا
        case 'G': baseFreq = 392.00f; break;  // صول
        case 'A': baseFreq = 440.00f; break;  // لا
        case 'B': baseFreq = 493.88f; break;  // سي
        default:  baseFreq = 440.00f; break;  // افتراضي: لا
    }

    // تعديل التردد حسب الأوكتاف بالنسبة للرابع
    // كل أوكتاف أعلى = ضعف التردد، أقل = نصفه
    int octaveDiff = octave - 4;
    if (octaveDiff > 0) {
        for (int i = 0; i < octaveDiff; ++i)
            baseFreq *= 2.0f;
    } else if (octaveDiff < 0) {
        for (int i = 0; i < -octaveDiff; ++i)
            baseFreq /= 2.0f;
    }

    return baseFreq;
}

// تشغيل نوتة موسيقية بالاسم — مثلاً "C4" أو "A5"
void playNote(const std::string& note, float duration, float volume) {
    if (!g_initialized) return;

    float freq = noteNameToFrequency(note);
    std::vector<int16_t> samples = generateSamples(freq, duration, volume, WaveType::Sine);
    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// تشغيل لحن — سلسلة من الترددات متتالية
void playMelody(const std::vector<float>& frequencies, float noteLength, float volume) {
    if (!g_initialized) return;

    // توليد وإضافة عينات كل نوتة بالتتابع
    for (float freq : frequencies) {
        if (freq <= 0.0f) {
            // تردد صفر أو سالب = صمت (سكتة موسيقية)
            int numSilence = static_cast<int>(g_sampleRate * noteLength);
            std::vector<int16_t> silence(numSilence, 0);
            SDL_QueueAudio(g_audioDevice, silence.data(),
                           static_cast<Uint32>(silence.size() * sizeof(int16_t)));
        } else {
            std::vector<int16_t> samples = generateSamples(freq, noteLength, volume, WaveType::Sine);
            SDL_QueueAudio(g_audioDevice, samples.data(),
                           static_cast<Uint32>(samples.size() * sizeof(int16_t)));
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// أصوات واجهة المستخدم — أصوات جاهزة للاستخدام الفوري
// ═══════════════════════════════════════════════════════════════════

// تشغيل صوت نقرة — قصير وحاد
void playClick() {
    if (!g_initialized) return;

    // موجة مربعة 1000 هرتز لمدة 0.05 ثانية — صوت نقرة واضح
    std::vector<int16_t> samples = generateSamples(1000.0f, 0.05f, 0.4f, WaveType::Square);
    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// تشغيل صوت نجاح — ثلاث نغمات صاعدة مبهجة
void playSuccess() {
    if (!g_initialized) return;

    // دو5 → مي5 → صول5 (تصاعد بهيج)
    float successNotes[] = { 523.0f, 659.0f, 784.0f };
    for (float freq : successNotes) {
        std::vector<int16_t> samples = generateSamples(freq, 0.1f, 0.5f, WaveType::Sine);
        SDL_QueueAudio(g_audioDevice, samples.data(),
                       static_cast<Uint32>(samples.size() * sizeof(int16_t)));
    }
}

// تشغيل صوت خطأ — نغمة منخفضة تنبيهية
void playError() {
    if (!g_initialized) return;

    // موجة مربعة 200 هرتز لمدة 0.3 ثانية — صوت مزعج يلفت الانتباه
    std::vector<int16_t> samples = generateSamples(200.0f, 0.3f, 0.5f, WaveType::Square);
    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// تشغيل صوت تحذير — نغمتان متناوبتان
void playWarning() {
    if (!g_initialized) return;

    // تناوب بين 800 و 600 هرتز — صوت إنذار
    float warningFreqs[] = { 800.0f, 600.0f, 800.0f, 600.0f };
    for (float freq : warningFreqs) {
        std::vector<int16_t> samples = generateSamples(freq, 0.1f, 0.45f, WaveType::Square);
        SDL_QueueAudio(g_audioDevice, samples.data(),
                       static_cast<Uint32>(samples.size() * sizeof(int16_t)));
    }
}

// تشغيل صوت إشعار — نغمة عالية قصيرة
void playNotification() {
    if (!g_initialized) return;

    // موجة جيبية 880 هرتز (لا5) لمدة 0.15 ثانية
    std::vector<int16_t> samples = generateSamples(880.0f, 0.15f, 0.5f, WaveType::Sine);
    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// تشغيل صوت واجهة مستخدم حسب النوع (للاستدعاء من SAD)
void playUISound(int soundType) {
    switch (soundType) {
        case 0: playClick();        break;  // نقرة
        case 1: playClick();        break;  // تمرير (نفس النقرة بصوت أخف)
        case 2: playSuccess();      break;  // نجاح
        case 3: playError();        break;  // خطأ
        case 4: playWarning();      break;  // تحذير
        case 5: playNotification(); break;  // إشعار
        case 6: playClick();        break;  // ضغطة مفتاح
        case 7: playSweepUp(300.0f, 600.0f, 0.15f); break; // انتقال
        default: playClick();       break;
    }
}

// ═══════════════════════════════════════════════════════════════════
// مؤثرات صوتية خاصة — أصوات مركّبة ومعقّدة
// ═══════════════════════════════════════════════════════════════════

// صوت مسح صاعد — التردد يرتفع تدريجياً من startFreq إلى endFreq
void playSweepUp(float startFreq, float endFreq, float duration) {
    if (!g_initialized) return;

    int numSamples = static_cast<int>(g_sampleRate * duration);
    std::vector<int16_t> samples(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        // حساب التردد الحالي بالاستيفاء الخطي
        float progress = static_cast<float>(i) / numSamples;
        float currentFreq = startFreq + (endFreq - startFreq) * progress;

        // توليد عينة موجة جيبية بالتردد المتغير
        float sample = sinf(2.0f * (float)M_PI * currentFreq *
                            static_cast<float>(i) / g_sampleRate);

        // تطبيق مستوى الصوت
        sample *= 0.5f * g_masterVolume;
        samples[i] = static_cast<int16_t>(std::clamp(sample, -1.0f, 1.0f) * 32000.0f);
    }

    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// صوت مسح هابط — التردد ينخفض تدريجياً من startFreq إلى endFreq
void playSweepDown(float startFreq, float endFreq, float duration) {
    if (!g_initialized) return;

    // نفس المنطق لكن بعكس الاتجاه (البداية أعلى من النهاية)
    int numSamples = static_cast<int>(g_sampleRate * duration);
    std::vector<int16_t> samples(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        float progress = static_cast<float>(i) / numSamples;
        float currentFreq = startFreq + (endFreq - startFreq) * progress;

        float sample = sinf(2.0f * (float)M_PI * currentFreq *
                            static_cast<float>(i) / g_sampleRate);

        // إضافة تلاشي تدريجي مع انخفاض التردد
        float fadeOut = 1.0f - progress * 0.3f;
        sample *= 0.5f * g_masterVolume * fadeOut;
        samples[i] = static_cast<int16_t>(std::clamp(sample, -1.0f, 1.0f) * 32000.0f);
    }

    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// صوت ارتداد — مسح هابط سريع يحاكي كرة ترتد
void playBounce() {
    if (!g_initialized) return;

    // مسح من 600 هرتز إلى 200 هرتز في 0.15 ثانية
    int numSamples = static_cast<int>(g_sampleRate * 0.15f);
    std::vector<int16_t> samples(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        float progress = static_cast<float>(i) / numSamples;
        // انخفاض متسارع في التردد (منحنى تربيعي)
        float currentFreq = 600.0f - 400.0f * progress * progress;

        float sample = sinf(2.0f * (float)M_PI * currentFreq *
                            static_cast<float>(i) / g_sampleRate);

        // تلاشي سريع في النهاية
        float envelope = 1.0f - progress;
        sample *= 0.5f * g_masterVolume * envelope;
        samples[i] = static_cast<int16_t>(std::clamp(sample, -1.0f, 1.0f) * 32000.0f);
    }

    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// صوت انفجار — دفقة ضوضاء مع تلاشي سريع
void playExplosionSound() {
    if (!g_initialized) return;

    // ضوضاء بيضاء لمدة 0.3 ثانية مع غلاف انحداري
    int numSamples = static_cast<int>(g_sampleRate * 0.3f);
    std::vector<int16_t> samples(numSamples);

    for (int i = 0; i < numSamples; ++i) {
        float progress = static_cast<float>(i) / numSamples;

        // ضوضاء عشوائية
        float noise = g_noiseDist(g_rng) / 16000.0f;

        // غلاف انحداري أسّي — يبدأ بقوة ويتلاشى سريعاً
        float decay = expf(-4.0f * progress);

        // إضافة مكون تردد منخفض للعمق
        float lowRumble = sinf(2.0f * (float)M_PI * 60.0f *
                               static_cast<float>(i) / g_sampleRate);

        float sample = (noise * 0.7f + lowRumble * 0.3f) * decay;
        sample *= 0.6f * g_masterVolume;
        samples[i] = static_cast<int16_t>(std::clamp(sample, -1.0f, 1.0f) * 32000.0f);
    }

    SDL_QueueAudio(g_audioDevice, samples.data(),
                   static_cast<Uint32>(samples.size() * sizeof(int16_t)));
}

// ═══════════════════════════════════════════════════════════════════
// نظام الإيقاعات — نبضات منتظمة بأنماط مختلفة
// ═══════════════════════════════════════════════════════════════════

// معرّف الإيقاع النشط (بسيط — إيقاع واحد في كل مرة)
static int g_activeBeatId = 0;
static bool g_beatActive = false;

// بدء نمط إيقاعي — يولّد عينات النمط ويضعها في الطابور
int startBeat(float bpm, int pattern) {
    if (!g_initialized) return -1;

    // إيقاف أي إيقاع نشط سابق
    g_beatActive = false;

    // حساب مدة النبضة الواحدة بالثواني
    float beatDuration = 60.0f / bpm;

    // أنماط إيقاعية مُعرَّفة مسبقاً
    std::vector<bool> beats;
    switch (pattern) {
        case 0: // نمط بسيط: نبض-صمت-نبض-صمت
            beats = { true, false, true, false, true, false, true, false };
            break;
        case 1: // نمط روك: نبض-صمت-صمت-نبض-نبض-صمت-صمت-نبض
            beats = { true, false, false, true, true, false, false, true };
            break;
        case 2: // نمط سريع: كل نبضة فعّالة
            beats = { true, true, true, true, true, true, true, true };
            break;
        default: // نمط افتراضي
            beats = { true, false, true, false };
            break;
    }

    // توليد العينات لكل نبضة في النمط (دورة واحدة)
    for (bool active : beats) {
        if (active) {
            // نبضة: موجة مربعة قصيرة حادة
            std::vector<int16_t> hit = generateSamples(
                150.0f, beatDuration * 0.3f, 0.5f, WaveType::Square);
            SDL_QueueAudio(g_audioDevice, hit.data(),
                           static_cast<Uint32>(hit.size() * sizeof(int16_t)));

            // الباقي صمت حتى النبضة التالية
            int silenceSamples = static_cast<int>(g_sampleRate * beatDuration * 0.7f);
            std::vector<int16_t> silence(silenceSamples, 0);
            SDL_QueueAudio(g_audioDevice, silence.data(),
                           static_cast<Uint32>(silence.size() * sizeof(int16_t)));
        } else {
            // صمت كامل لمدة النبضة
            int silenceSamples = static_cast<int>(g_sampleRate * beatDuration);
            std::vector<int16_t> silence(silenceSamples, 0);
            SDL_QueueAudio(g_audioDevice, silence.data(),
                           static_cast<Uint32>(silence.size() * sizeof(int16_t)));
        }
    }

    // إرجاع معرّف الإيقاع
    g_beatActive = true;
    g_activeBeatId++;
    return g_activeBeatId;
}

// إيقاف إيقاع محدد
void stopBeat(int beatId) {
    (void)beatId; // نتجاهل المعرّف حالياً — نوقف الكل
    g_beatActive = false;
    // مسح طابور الصوت لإيقاف فوري
    if (g_initialized && g_audioDevice != 0) {
        SDL_ClearQueuedAudio(g_audioDevice);
    }
}

// ═══════════════════════════════════════════════════════════════════
// التحكم العام بالصوت
// ═══════════════════════════════════════════════════════════════════

// تعيين مستوى الصوت الرئيسي (0.0 = صامت، 1.0 = أقصى)
void setMasterVolume(float volume) {
    g_masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

// الحصول على مستوى الصوت الرئيسي الحالي
float getMasterVolume() {
    return g_masterVolume;
}

// إيقاف جميع الأصوات فوراً — مسح طابور الصوت بالكامل
void stopAllSounds() {
    if (!g_initialized || g_audioDevice == 0) return;

    g_beatActive = false;
    SDL_ClearQueuedAudio(g_audioDevice);
}

// التحقق مما إذا كان هناك صوت قيد التشغيل حالياً
bool isPlaying() {
    if (!g_initialized || g_audioDevice == 0) return false;

    // إذا كان هناك بيانات في طابور الصوت، فالصوت يعمل
    Uint32 queued = SDL_GetQueuedAudioSize(g_audioDevice);
    return queued > 0;
}

}} // namespace sad::audio
