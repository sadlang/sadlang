// ملف: normalization.cpp
// الوصف: تنفيذات تطبيع الطبقة و RMSNorm
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <cmath>
#include <numeric>
#include <cassert>
#include <algorithm>

namespace sad::ai {

// ثابت صغير لمنع القسمة على صفر
constexpr float NORM_EPS = 1e-6f;

class LayerNorm {
    size_t dim_;
    float eps_;
    std::vector<float> gamma_;  // معاملات القياس
    std::vector<float> beta_;   // معاملات الإزاحة
    bool elementwise_affine_;

public:
    LayerNorm(size_t dim, float eps = NORM_EPS, bool affine = true)
        : dim_(dim), eps_(eps), elementwise_affine_(affine) {
        gamma_.resize(dim, 1.0f);
        beta_.resize(dim, 0.0f);
    }

    // حساب المتوسط لمتجه
    float compute_mean(const std::vector<float>& x) {
        float sum = std::accumulate(x.begin(), x.end(), 0.0f);
        return sum / static_cast<float>(x.size());
    }

    // حساب التباين لمتجه
    float compute_variance(const std::vector<float>& x, float mean) {
        float var = 0.0f;
        for (float v : x) {
            float diff = v - mean;
            var += diff * diff;
        }
        return var / static_cast<float>(x.size());
    }

    // التمرير الأمامي لتطبيع الطبقة
    std::vector<float> forward(const std::vector<float>& input) {
        assert(input.size() == dim_);
        float mean = compute_mean(input);
        float var = compute_variance(input, mean);
        float inv_std = 1.0f / std::sqrt(var + eps_);

        std::vector<float> output(dim_);
        for (size_t i = 0; i < dim_; ++i) {
            float normalized = (input[i] - mean) * inv_std;
            output[i] = elementwise_affine_ ? (gamma_[i] * normalized + beta_[i]) : normalized;
        }
        return output;
    }

    // تطبيع دُفعة كاملة
    std::vector<std::vector<float>> forward_batch(const std::vector<std::vector<float>>& batch) {
        std::vector<std::vector<float>> results;
        results.reserve(batch.size());
        for (const auto& x : batch) {
            results.push_back(forward(x));
        }
        return results;
    }

    void set_gamma(const std::vector<float>& g) { gamma_ = g; }
    void set_beta(const std::vector<float>& b) { beta_ = b; }
};

class RMSNorm {
    size_t dim_;
    float eps_;
    std::vector<float> weight_;

public:
    RMSNorm(size_t dim, float eps = NORM_EPS) : dim_(dim), eps_(eps) {
        weight_.resize(dim, 1.0f);
    }

    // حساب الجذر التربيعي لمتوسط المربعات
    float compute_rms(const std::vector<float>& x) {
        float sum_sq = 0.0f;
        for (float v : x) sum_sq += v * v;
        return std::sqrt(sum_sq / static_cast<float>(x.size()) + eps_);
    }

    // التمرير الأمامي لتطبيع RMS
    std::vector<float> forward(const std::vector<float>& input) {
        assert(input.size() == dim_);
        float rms = compute_rms(input);
        float inv_rms = 1.0f / rms;

        std::vector<float> output(dim_);
        for (size_t i = 0; i < dim_; ++i) {
            output[i] = input[i] * inv_rms * weight_[i];
        }
        return output;
    }

    // تطبيع مع تدرج مخصص للتدريب
    std::vector<float> forward_with_residual(const std::vector<float>& input,
                                              const std::vector<float>& residual) {
        assert(input.size() == dim_ && residual.size() == dim_);
        std::vector<float> fused(dim_);
        for (size_t i = 0; i < dim_; ++i) fused[i] = input[i] + residual[i];
        return forward(fused);
    }

    void set_weight(const std::vector<float>& w) { weight_ = w; }
    size_t dim() const { return dim_; }
};

} // namespace sad::ai
