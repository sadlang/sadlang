// ===================================================================
// صدOS - نظام إدارة الطاقة
// المرحلة 52: حالات السكون وتدرج تردد المعالج
// ===================================================================

#include <cstdint>
#include <string>
#include <vector>
#include <functional>

namespace sad::os::hal {

/// حالات السكون وفق معيار ACPI (S0-S5)
enum class SleepState {
    S0 = 0,  // عامل بالكامل (أداء كامل)
    S1 = 1,  // سكون خفيف (المعالج متوقف، الذاكرة نشطة)
    S2 = 2,  // سكون أعمق (ذاكرة المعالج المؤقتة مُفرّغة)
    S3 = 3,  // تعليق إلى الذاكرة (RAM فقط نشطة)
    S4 = 4,  // سبات (الحالة محفوظة على القرص)
    S5 = 5   // إيقاف كامل
};

/// سياسة تدرج تردد المعالج
enum class FreqPolicy {
    Performance  = 0,  // أقصى أداء دائماً
    PowerSave    = 1,  // توفير طاقة أقصى
    OnDemand     = 2,  // تعديل حسب الحمل
    Conservative = 3   // تعديل تدريجي محافظ
};

/// مصدر إيقاظ النظام من السكون
struct WakeSource {
    std::string name;       // اسم المصدر (لوحة مفاتيح، شبكة، مؤقت)
    uint32_t irq;           // رقم المقاطعة
    bool enabled;           // هل مصدر الإيقاظ مفعّل؟
    uint64_t wake_count;    // عدد مرات الإيقاظ
};

/// معلومات تردد المعالج الحالية
struct CPUFreqInfo {
    uint64_t current_freq_khz;  // التردد الحالي بالكيلوهرتز
    uint64_t min_freq_khz;      // أدنى تردد ممكن
    uint64_t max_freq_khz;      // أقصى تردد ممكن
    FreqPolicy policy;           // السياسة المطبقة
};

// === مدير الطاقة ===

/// مدير مركزي لإدارة طاقة النظام وتردد المعالج
class PowerManager {
public:
    using StateChangeCallback = std::function<void(SleepState, SleepState)>;

    /// تهيئة نظام إدارة الطاقة
    bool initialize() {
        detect_capabilities();  // اكتشاف إمكانيات الطاقة
        setup_wake_sources();   // إعداد مصادر الإيقاظ
        freq_info_.policy = FreqPolicy::OnDemand;
        initialized_ = true;
        return true;
    }

    /// الانتقال إلى حالة سكون محددة
    bool enter_sleep_state(SleepState target) {
        if (target == current_state_) return true;
        if (!validate_transition(current_state_, target)) return false;

        // إشعار المراقبين قبل الانتقال
        SleepState old = current_state_;
        notify_state_change(old, target);

        switch (target) {
            case SleepState::S1: suspend_cpus_light(); break;
            case SleepState::S3: suspend_to_ram();     break;
            case SleepState::S4: hibernate_to_disk();  break;
            case SleepState::S5: full_shutdown();       break;
            default: break;
        }
        current_state_ = target;
        return true;
    }

    /// ضبط سياسة تدرج تردد المعالج
    void set_freq_policy(FreqPolicy policy) {
        freq_info_.policy = policy;
        apply_freq_policy();
    }

    /// ضبط تردد المعالج يدوياً (بالكيلوهرتز)
    bool set_cpu_frequency(uint64_t freq_khz) {
        if (freq_khz < freq_info_.min_freq_khz ||
            freq_khz > freq_info_.max_freq_khz) return false;
        freq_info_.current_freq_khz = freq_khz;
        return true;
    }

    /// تفعيل أو تعطيل مصدر إيقاظ
    bool configure_wake_source(const std::string& name, bool enable) {
        for (auto& ws : wake_sources_) {
            if (ws.name == name) { ws.enabled = enable; return true; }
        }
        return false;
    }

    /// تسجيل مراقب لتغييرات حالة الطاقة
    void on_state_change(StateChangeCallback cb) {
        callbacks_.push_back(std::move(cb));
    }

    SleepState current_state() const { return current_state_; }
    const CPUFreqInfo& freq_info() const { return freq_info_; }
    const std::vector<WakeSource>& wake_sources() const { return wake_sources_; }

private:
    SleepState current_state_ = SleepState::S0;
    CPUFreqInfo freq_info_{2400000, 800000, 4500000, FreqPolicy::OnDemand};
    std::vector<WakeSource> wake_sources_;
    std::vector<StateChangeCallback> callbacks_;
    bool initialized_ = false;

    /// التحقق من صحة انتقال حالة الطاقة
    bool validate_transition(SleepState from, SleepState to) const {
        // لا يمكن الانتقال من S5 (إيقاف) إلا عبر إعادة التشغيل
        if (from == SleepState::S5) return false;
        (void)to;
        return true;
    }

    void detect_capabilities() {
        // اكتشاف حالات السكون المدعومة من العتاد
    }

    void setup_wake_sources() {
        wake_sources_.push_back({"لوحة_المفاتيح", 1, true, 0});
        wake_sources_.push_back({"الشبكة", 11, false, 0});
        wake_sources_.push_back({"المؤقت", 0, true, 0});
    }

    void apply_freq_policy() { /* تطبيق السياسة على العتاد */ }
    void suspend_cpus_light() { /* سكون خفيف */ }
    void suspend_to_ram()     { /* تعليق إلى الذاكرة */ }
    void hibernate_to_disk()  { /* سبات إلى القرص */ }
    void full_shutdown()      { /* إيقاف كامل */ }

    void notify_state_change(SleepState from, SleepState to) {
        for (auto& cb : callbacks_) cb(from, to);
    }
};

} // namespace sad::os::hal
