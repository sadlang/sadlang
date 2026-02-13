// ملف: transformer.cpp
// الوصف: كتلة المحول مع شبكة التغذية الأمامية والاتصالات المتبقية وتكديس الطبقات
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <memory>
#include <cmath>
#include <cassert>
#include <functional>

namespace sad::ai {

struct TensorShape {
    size_t batch;
    size_t seq_len;
    size_t hidden_dim;
};

class FeedForwardNetwork {
    size_t hidden_dim_;
    size_t intermediate_dim_;
    std::vector<float> w_up_;
    std::vector<float> w_down_;
    std::vector<float> w_gate_;

public:
    FeedForwardNetwork(size_t hidden_dim, size_t intermediate_dim)
        : hidden_dim_(hidden_dim), intermediate_dim_(intermediate_dim) {
        w_up_.resize(hidden_dim * intermediate_dim, 0.01f);
        w_down_.resize(intermediate_dim * hidden_dim, 0.01f);
        w_gate_.resize(hidden_dim * intermediate_dim, 0.01f);
    }

    // تطبيق SwiGLU كدالة تنشيط
    std::vector<float> forward(const std::vector<float>& input) {
        std::vector<float> gate(intermediate_dim_, 0.0f);
        std::vector<float> up(intermediate_dim_, 0.0f);
        for (size_t i = 0; i < intermediate_dim_; ++i) {
            for (size_t j = 0; j < hidden_dim_; ++j) {
                gate[i] += input[j] * w_gate_[j * intermediate_dim_ + i];
                up[i] += input[j] * w_up_[j * intermediate_dim_ + i];
            }
            float silu = gate[i] / (1.0f + std::exp(-gate[i]));
            up[i] *= silu;
        }
        std::vector<float> output(hidden_dim_, 0.0f);
        for (size_t i = 0; i < hidden_dim_; ++i) {
            for (size_t j = 0; j < intermediate_dim_; ++j) {
                output[i] += up[j] * w_down_[j * hidden_dim_ + i];
            }
        }
        return output;
    }
};

class TransformerBlock {
    size_t hidden_dim_;
    size_t num_heads_;
    std::unique_ptr<FeedForwardNetwork> ffn_;
    std::vector<float> norm1_weight_;
    std::vector<float> norm2_weight_;
    float residual_scale_;

public:
    TransformerBlock(size_t hidden_dim, size_t num_heads, size_t intermediate_dim)
        : hidden_dim_(hidden_dim), num_heads_(num_heads), residual_scale_(1.0f) {
        ffn_ = std::make_unique<FeedForwardNetwork>(hidden_dim, intermediate_dim);
        norm1_weight_.resize(hidden_dim, 1.0f);
        norm2_weight_.resize(hidden_dim, 1.0f);
    }

    // الاتصال المتبقي مع التطبيع
    std::vector<float> residual_add(const std::vector<float>& x, const std::vector<float>& sublayer_out) {
        assert(x.size() == sublayer_out.size());
        std::vector<float> result(x.size());
        for (size_t i = 0; i < x.size(); ++i) {
            result[i] = x[i] + residual_scale_ * sublayer_out[i];
        }
        return result;
    }

    // تمرير أمامي عبر كتلة المحول
    std::vector<float> forward(const std::vector<float>& input) {
        auto normed = rms_norm(input, norm1_weight_);
        auto ffn_out = ffn_->forward(normed);
        auto after_ffn = residual_add(input, ffn_out);
        return rms_norm(after_ffn, norm2_weight_);
    }

    std::vector<float> rms_norm(const std::vector<float>& x, const std::vector<float>& w) {
        float sum_sq = 0.0f;
        for (auto v : x) sum_sq += v * v;
        float rms = std::sqrt(sum_sq / x.size() + 1e-6f);
        std::vector<float> out(x.size());
        for (size_t i = 0; i < x.size(); ++i) out[i] = (x[i] / rms) * w[i];
        return out;
    }

    void set_residual_scale(float s) { residual_scale_ = s; }
};

class TransformerStack {
    std::vector<std::unique_ptr<TransformerBlock>> layers_;
    size_t num_layers_;

public:
    TransformerStack(size_t num_layers, size_t hidden_dim, size_t num_heads, size_t inter_dim)
        : num_layers_(num_layers) {
        for (size_t i = 0; i < num_layers; ++i) {
            layers_.push_back(std::make_unique<TransformerBlock>(hidden_dim, num_heads, inter_dim));
        }
    }

    // تمرير عبر جميع الطبقات بالتسلسل
    std::vector<float> forward(std::vector<float> input) {
        for (auto& layer : layers_) {
            input = layer->forward(input);
        }
        return input;
    }

    size_t num_layers() const { return num_layers_; }
    size_t param_count() const { return layers_.size(); }
};

} // namespace sad::ai
