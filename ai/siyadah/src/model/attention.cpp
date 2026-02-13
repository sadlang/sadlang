// ملف: attention.cpp
// الوصف: آلية الانتباه متعددة الرؤوس مع إسقاطات QKV والضرب النقطي المقيّس والإخفاء
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <cassert>

namespace sad::ai {

struct AttentionConfig {
    size_t hidden_dim = 768;
    size_t num_heads = 12;
    size_t head_dim = 64;
    float dropout_rate = 0.1f;
    bool use_causal_mask = true;
};

class MultiHeadAttention {
    AttentionConfig config_;
    std::vector<float> w_q_, w_k_, w_v_, w_o_;
    std::vector<float> bias_q_, bias_k_, bias_v_;

public:
    explicit MultiHeadAttention(const AttentionConfig& config) : config_(config) {
        size_t proj_size = config.hidden_dim * config.hidden_dim;
        w_q_.resize(proj_size, 0.02f);
        w_k_.resize(proj_size, 0.02f);
        w_v_.resize(proj_size, 0.02f);
        w_o_.resize(proj_size, 0.02f);
        bias_q_.resize(config.hidden_dim, 0.0f);
        bias_k_.resize(config.hidden_dim, 0.0f);
        bias_v_.resize(config.hidden_dim, 0.0f);
    }

    // إسقاط الاستعلام والمفتاح والقيمة
    std::vector<float> project(const std::vector<float>& input, const std::vector<float>& weight,
                               const std::vector<float>& bias) {
        size_t dim = config_.hidden_dim;
        std::vector<float> output(dim, 0.0f);
        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                output[i] += input[j] * weight[j * dim + i];
            }
            output[i] += bias[i];
        }
        return output;
    }

    // حساب الضرب النقطي المقيّس
    std::vector<float> scaled_dot_product(const std::vector<float>& q,
                                          const std::vector<float>& k,
                                          const std::vector<float>& v,
                                          size_t seq_len) {
        float scale = 1.0f / std::sqrt(static_cast<float>(config_.head_dim));
        size_t hd = config_.head_dim;
        std::vector<float> scores(seq_len * seq_len, 0.0f);

        for (size_t i = 0; i < seq_len; ++i) {
            for (size_t j = 0; j < seq_len; ++j) {
                float dot = 0.0f;
                for (size_t d = 0; d < hd; ++d) {
                    dot += q[i * hd + d] * k[j * hd + d];
                }
                scores[i * seq_len + j] = dot * scale;
            }
        }
        return scores;
    }

    // تطبيق قناع السببية لمنع الانتباه للمستقبل
    void apply_causal_mask(std::vector<float>& scores, size_t seq_len) {
        for (size_t i = 0; i < seq_len; ++i) {
            for (size_t j = i + 1; j < seq_len; ++j) {
                scores[i * seq_len + j] = -std::numeric_limits<float>::infinity();
            }
        }
    }

    // تطبيق softmax على صف من الدرجات
    void softmax_row(std::vector<float>& scores, size_t offset, size_t len) {
        float max_val = *std::max_element(scores.begin() + offset, scores.begin() + offset + len);
        float sum = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            scores[offset + i] = std::exp(scores[offset + i] - max_val);
            sum += scores[offset + i];
        }
        for (size_t i = 0; i < len; ++i) {
            scores[offset + i] /= (sum + 1e-9f);
        }
    }

    // التمرير الأمامي الكامل للانتباه متعدد الرؤوس
    std::vector<float> forward(const std::vector<float>& input, size_t seq_len) {
        auto q = project(input, w_q_, bias_q_);
        auto k = project(input, w_k_, bias_k_);
        auto v = project(input, w_v_, bias_v_);

        auto scores = scaled_dot_product(q, k, v, seq_len);
        if (config_.use_causal_mask) {
            apply_causal_mask(scores, seq_len);
        }
        for (size_t i = 0; i < seq_len; ++i) {
            softmax_row(scores, i * seq_len, seq_len);
        }

        // تجميع القيم الموزونة
        std::vector<float> output(config_.hidden_dim, 0.0f);
        for (size_t i = 0; i < config_.head_dim; ++i) {
            for (size_t j = 0; j < seq_len; ++j) {
                output[i] += scores[j] * v[j * config_.head_dim + i];
            }
        }
        return output;
    }

    size_t num_heads() const { return config_.num_heads; }
    size_t head_dim() const { return config_.head_dim; }
};

} // namespace sad::ai
