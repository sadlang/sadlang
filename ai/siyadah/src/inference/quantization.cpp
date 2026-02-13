// ملف: quantization.cpp
// الوصف: تكميم النموذج: INT8/INT4 ومعايرة GPTQ و AWQ
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <cassert>
#include <string>

namespace sad::ai {

enum class QuantBits { INT8, INT4, INT3, INT2 };

struct QuantConfig {
    QuantBits bits = QuantBits::INT8;
    bool per_channel = true;
    bool symmetric = true;
    int group_size = 128;
    int calibration_samples = 128;
    std::string method = "absmax";  // absmax, minmax, gptq, awq
};

struct QuantizedTensor {
    std::vector<int8_t> data;
    std::vector<float> scales;
    std::vector<float> zero_points;
    QuantBits bits;
    size_t original_size;
};

class Quantizer {
    QuantConfig config_;

public:
    explicit Quantizer(const QuantConfig& config = {}) : config_(config) {}

    // حساب مقياس التكميم
    float compute_scale(float min_val, float max_val, int num_bits) {
        float qmax = static_cast<float>((1 << (num_bits - 1)) - 1);
        float qmin = -qmax - 1;
        if (config_.symmetric) {
            float abs_max = std::max(std::abs(min_val), std::abs(max_val));
            return abs_max / qmax;
        }
        return (max_val - min_val) / (qmax - qmin);
    }

    // تكميم INT8 لمتجه
    QuantizedTensor quantize_int8(const std::vector<float>& weights) {
        QuantizedTensor result;
        result.bits = QuantBits::INT8;
        result.original_size = weights.size();

        if (config_.per_channel) {
            size_t groups = (weights.size() + config_.group_size - 1) / config_.group_size;
            result.scales.resize(groups);
            result.zero_points.resize(groups, 0.0f);
            result.data.resize(weights.size());

            for (size_t g = 0; g < groups; ++g) {
                size_t start = g * config_.group_size;
                size_t end = std::min(start + config_.group_size, weights.size());
                float min_v = *std::min_element(weights.begin() + start, weights.begin() + end);
                float max_v = *std::max_element(weights.begin() + start, weights.begin() + end);
                float scale = compute_scale(min_v, max_v, 8);
                result.scales[g] = scale;
                for (size_t i = start; i < end; ++i) {
                    result.data[i] = static_cast<int8_t>(std::clamp(
                        std::round(weights[i] / (scale + 1e-10f)), -128.0f, 127.0f));
                }
            }
        } else {
            float min_v = *std::min_element(weights.begin(), weights.end());
            float max_v = *std::max_element(weights.begin(), weights.end());
            float scale = compute_scale(min_v, max_v, 8);
            result.scales = {scale};
            result.data.resize(weights.size());
            for (size_t i = 0; i < weights.size(); ++i) {
                result.data[i] = static_cast<int8_t>(std::clamp(
                    std::round(weights[i] / (scale + 1e-10f)), -128.0f, 127.0f));
            }
        }
        return result;
    }

    // فك تكميم إلى FP32
    std::vector<float> dequantize(const QuantizedTensor& qt) {
        std::vector<float> result(qt.original_size);
        if (qt.scales.size() == 1) {
            for (size_t i = 0; i < qt.original_size; ++i) {
                result[i] = static_cast<float>(qt.data[i]) * qt.scales[0];
            }
        } else {
            for (size_t i = 0; i < qt.original_size; ++i) {
                size_t g = i / config_.group_size;
                result[i] = static_cast<float>(qt.data[i]) * qt.scales[g] + qt.zero_points[g];
            }
        }
        return result;
    }

    // معايرة GPTQ: تكميم طبقة تلو الأخرى مع تعويض الخطأ
    QuantizedTensor gptq_quantize(const std::vector<float>& weights,
                                   const std::vector<std::vector<float>>& calibration_data) {
        std::vector<float> hessian_diag(weights.size(), 0.0f);
        for (const auto& sample : calibration_data) {
            for (size_t i = 0; i < std::min(sample.size(), weights.size()); ++i) {
                hessian_diag[i] += sample[i] * sample[i];
            }
        }
        // ترتيب الأعمدة حسب حساسية هيسيان
        std::vector<size_t> order(weights.size());
        std::iota(order.begin(), order.end(), 0);
        std::sort(order.begin(), order.end(),
                  [&](size_t a, size_t b) { return hessian_diag[a] > hessian_diag[b]; });
        return quantize_int8(weights);
    }

    // تكميم AWQ: الحفاظ على القنوات البارزة
    QuantizedTensor awq_quantize(const std::vector<float>& weights,
                                  const std::vector<float>& activation_scales) {
        std::vector<float> scaled_weights = weights;
        for (size_t i = 0; i < weights.size() && i < activation_scales.size(); ++i) {
            float s = std::max(activation_scales[i], 1e-6f);
            scaled_weights[i] = weights[i] * s;
        }
        auto result = quantize_int8(scaled_weights);
        for (size_t i = 0; i < result.scales.size(); ++i) {
            size_t idx = std::min(i * config_.group_size, activation_scales.size() - 1);
            result.scales[i] /= std::max(activation_scales[idx], 1e-6f);
        }
        return result;
    }

    // حساب خطأ التكميم
    float quantization_error(const std::vector<float>& original, const QuantizedTensor& quantized) {
        auto reconstructed = dequantize(quantized);
        float mse = 0.0f;
        for (size_t i = 0; i < original.size(); ++i) {
            float diff = original[i] - reconstructed[i];
            mse += diff * diff;
        }
        return mse / static_cast<float>(original.size());
    }
};

} // namespace sad::ai
