// ملف: position.cpp
// الوصف: ترميز الموقع: RoPE و ALiBi مع دعم الاتجاه من اليمين لليسار
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <cmath>
#include <cassert>
#include <algorithm>

namespace sad::ai {

// ثوابت الدائرة
constexpr float PI = 3.14159265358979f;
constexpr float BASE_THETA = 10000.0f;

enum class PositionType { Sinusoidal, RoPE, ALiBi };

class PositionEncoding {
    size_t max_seq_len_;
    size_t dim_;
    PositionType type_;
    std::vector<float> encoding_cache_;
    bool rtl_aware_;

public:
    PositionEncoding(size_t max_seq_len, size_t dim, PositionType type = PositionType::RoPE,
                     bool rtl = false)
        : max_seq_len_(max_seq_len), dim_(dim), type_(type), rtl_aware_(rtl) {
        precompute();
    }

    // حساب مسبق لقيم الترميز
    void precompute() {
        encoding_cache_.resize(max_seq_len_ * dim_, 0.0f);
        if (type_ == PositionType::Sinusoidal) {
            precompute_sinusoidal();
        }
    }

    // ترميز الموقع الجيبي الكلاسيكي
    void precompute_sinusoidal() {
        for (size_t pos = 0; pos < max_seq_len_; ++pos) {
            for (size_t i = 0; i < dim_; i += 2) {
                float angle = static_cast<float>(pos) / std::pow(BASE_THETA, static_cast<float>(i) / dim_);
                encoding_cache_[pos * dim_ + i] = std::sin(angle);
                if (i + 1 < dim_) encoding_cache_[pos * dim_ + i + 1] = std::cos(angle);
            }
        }
    }

    // تطبيق RoPE - ترميز الموقع الدوراني
    void apply_rope(std::vector<float>& q, std::vector<float>& k, size_t pos) {
        assert(q.size() == dim_ && k.size() == dim_);
        size_t effective_pos = rtl_aware_ ? (max_seq_len_ - 1 - pos) : pos;

        for (size_t i = 0; i < dim_; i += 2) {
            float theta = static_cast<float>(effective_pos) / std::pow(BASE_THETA, static_cast<float>(i) / dim_);
            float cos_t = std::cos(theta);
            float sin_t = std::sin(theta);

            float q0 = q[i], q1 = q[i + 1];
            q[i] = q0 * cos_t - q1 * sin_t;
            q[i + 1] = q0 * sin_t + q1 * cos_t;

            float k0 = k[i], k1 = k[i + 1];
            k[i] = k0 * cos_t - k1 * sin_t;
            k[i + 1] = k0 * sin_t + k1 * cos_t;
        }
    }

    // حساب انحياز ALiBi لرأس معين
    float compute_alibi_bias(size_t head_idx, size_t num_heads, size_t query_pos, size_t key_pos) {
        float slope = std::pow(2.0f, -8.0f * static_cast<float>(head_idx + 1) / num_heads);
        int distance;
        if (rtl_aware_) {
            distance = static_cast<int>(key_pos) - static_cast<int>(query_pos);
        } else {
            distance = static_cast<int>(query_pos) - static_cast<int>(key_pos);
        }
        return slope * static_cast<float>(distance);
    }

    // إنشاء مصفوفة انحياز ALiBi كاملة لرأس واحد
    std::vector<float> get_alibi_matrix(size_t head_idx, size_t num_heads, size_t seq_len) {
        std::vector<float> bias(seq_len * seq_len);
        for (size_t i = 0; i < seq_len; ++i) {
            for (size_t j = 0; j < seq_len; ++j) {
                bias[i * seq_len + j] = compute_alibi_bias(head_idx, num_heads, i, j);
            }
        }
        return bias;
    }

    // ترميز موقع واعٍ بالاتجاه من اليمين لليسار
    std::vector<float> get_rtl_position(size_t pos, size_t total_len) {
        std::vector<float> enc(dim_);
        size_t effective_pos = rtl_aware_ ? (total_len - 1 - pos) : pos;
        for (size_t i = 0; i < dim_; ++i) {
            enc[i] = encoding_cache_[effective_pos * dim_ + i];
        }
        return enc;
    }

    PositionType type() const { return type_; }
    bool is_rtl() const { return rtl_aware_; }
    size_t max_len() const { return max_seq_len_; }
};

} // namespace sad::ai
