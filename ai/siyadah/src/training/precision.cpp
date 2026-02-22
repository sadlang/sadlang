// ملف: precision.cpp
// الوصف: مدرب الدقة المختلطة مع FP16/BF16 وتدريج الخسارة وإلغاء تدريج التدرجات
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <cmath>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <cassert>

namespace sad::ai {

enum class PrecisionMode { FP32, FP16, BF16 };

// تمثيل FP16 مبسّط
struct FP16 {
    uint16_t bits;

    static FP16 from_float(float f) {
        FP16 h;
        uint32_t fbits;
        std::memcpy(&fbits, &f, sizeof(float));
        uint16_t sign = (fbits >> 16) & 0x8000;
        int exp = ((fbits >> 23) & 0xFF) - 127 + 15;
        uint16_t mantissa = (fbits >> 13) & 0x03FF;
        if (exp <= 0) { h.bits = sign; }
        else if (exp >= 31) { h.bits = sign | 0x7C00; }
        else { h.bits = sign | (exp << 10) | mantissa; }
        return h;
    }

    float to_float() const {
        uint16_t sign = (bits >> 15) & 1;
        uint16_t exp = (bits >> 10) & 0x1F;
        uint16_t mantissa = bits & 0x03FF;
        float result;
        if (exp == 0) result = std::ldexp(mantissa / 1024.0f, -14);
        else if (exp == 31) result = std::numeric_limits<float>::infinity();
        else result = std::ldexp(1.0f + mantissa / 1024.0f, exp - 15);
        return sign ? -result : result;
    }
};

struct LossScaleState {
    float scale = 65536.0f;
    float scale_factor = 2.0f;
    float backoff_factor = 0.5f;
    int growth_interval = 2000;
    int steps_since_last_growth = 0;
    int consecutive_good_steps = 0;
};

class MixedPrecisionTrainer {
    PrecisionMode mode_;
    LossScaleState scale_state_;
    std::vector<float> master_weights_;  // أوزان بدقة كاملة
    bool dynamic_scaling_;
    int overflow_count_;

public:
    MixedPrecisionTrainer(PrecisionMode mode = PrecisionMode::FP16, bool dynamic = true)
        : mode_(mode), dynamic_scaling_(dynamic), overflow_count_(0) {}

    // تحويل الأوزان من FP32 إلى الدقة المنخفضة
    std::vector<uint16_t> cast_to_low_precision(const std::vector<float>& weights) {
        std::vector<uint16_t> low(weights.size());
        for (size_t i = 0; i < weights.size(); ++i) {
            low[i] = FP16::from_float(weights[i]).bits;
        }
        return low;
    }

    // تحويل من الدقة المنخفضة إلى FP32
    std::vector<float> cast_to_fp32(const std::vector<uint16_t>& low) {
        std::vector<float> fp32(low.size());
        for (size_t i = 0; i < low.size(); ++i) {
            FP16 h; h.bits = low[i];
            fp32[i] = h.to_float();
        }
        return fp32;
    }

    // تدريج الخسارة لمنع تلاشي التدرجات
    float scale_loss(float loss) {
        return loss * scale_state_.scale;
    }

    // إلغاء تدريج التدرجات
    bool unscale_gradients(std::vector<float>& grads) {
        float inv_scale = 1.0f / scale_state_.scale;
        bool has_overflow = false;
        for (auto& g : grads) {
            g *= inv_scale;
            if (std::isinf(g) || std::isnan(g)) {
                has_overflow = true;
                g = 0.0f;
            }
        }
        return !has_overflow;
    }

    // تحديث مقياس الخسارة الديناميكي
    void update_scale(bool overflow) {
        if (overflow) {
            scale_state_.scale *= scale_state_.backoff_factor;
            scale_state_.consecutive_good_steps = 0;
            overflow_count_++;
        } else {
            scale_state_.consecutive_good_steps++;
            if (scale_state_.consecutive_good_steps >= scale_state_.growth_interval) {
                scale_state_.scale *= scale_state_.scale_factor;
                scale_state_.consecutive_good_steps = 0;
            }
        }
    }

    // خطوة تدريب بدقة مختلطة
    void step(std::vector<float>& params, std::vector<float>& grads, float lr) {
        bool valid = unscale_gradients(grads);
        update_scale(!valid);
        if (!valid) return;  // تخطي الخطوة عند الطفحان
        for (size_t i = 0; i < params.size(); ++i) {
            params[i] -= lr * grads[i];
        }
    }

    float current_scale() const { return scale_state_.scale; }
    PrecisionMode mode() const { return mode_; }
    int overflow_count() const { return overflow_count_; }
};

} // namespace sad::ai
