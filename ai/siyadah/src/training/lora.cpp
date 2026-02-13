// ملف: lora.cpp
// الوصف: محوّل LoRA مع مصفوفات منخفضة الرتبة واختيار الرتبة والدمج وفك الدمج
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <cmath>
#include <random>
#include <cassert>

namespace sad::ai {

struct LoRAConfig {
    int rank = 8;
    float alpha = 16.0f;
    float dropout = 0.0f;
    std::vector<std::string> target_modules = {"q_proj", "v_proj"};
    bool fan_in_fan_out = false;
};

class LoRALayer {
    size_t in_dim_;
    size_t out_dim_;
    int rank_;
    float alpha_;
    float scaling_;
    std::vector<float> lora_a_;  // مصفوفة A منخفضة الرتبة
    std::vector<float> lora_b_;  // مصفوفة B منخفضة الرتبة
    std::vector<float> original_weight_;
    bool merged_;

public:
    LoRALayer(size_t in_dim, size_t out_dim, int rank, float alpha)
        : in_dim_(in_dim), out_dim_(out_dim), rank_(rank), alpha_(alpha), merged_(false) {
        scaling_ = alpha_ / static_cast<float>(rank_);
        initialize_matrices();
    }

    // تهيئة مصفوفات LoRA بتوزيع كاوسي
    void initialize_matrices() {
        std::mt19937 gen(42);
        std::normal_distribution<float> dist(0.0f, 1.0f / std::sqrt(static_cast<float>(rank_)));
        lora_a_.resize(in_dim_ * rank_);
        for (auto& v : lora_a_) v = dist(gen);
        lora_b_.resize(rank_ * out_dim_, 0.0f);  // تهيئة بأصفار
    }

    // التمرير الأمامي: x * (W + s*B*A)
    std::vector<float> forward(const std::vector<float>& input,
                               const std::vector<float>& base_output) {
        if (merged_) return base_output;

        // حساب x * A
        std::vector<float> intermediate(rank_, 0.0f);
        for (int r = 0; r < rank_; ++r) {
            for (size_t i = 0; i < in_dim_; ++i) {
                intermediate[r] += input[i] * lora_a_[i * rank_ + r];
            }
        }

        // حساب (x * A) * B * scaling
        std::vector<float> output = base_output;
        for (size_t o = 0; o < out_dim_; ++o) {
            float lora_out = 0.0f;
            for (int r = 0; r < rank_; ++r) {
                lora_out += intermediate[r] * lora_b_[r * out_dim_ + o];
            }
            output[o] += lora_out * scaling_;
        }
        return output;
    }

    // دمج أوزان LoRA في الأوزان الأصلية
    void merge(std::vector<float>& weight) {
        assert(weight.size() == in_dim_ * out_dim_);
        if (merged_) return;
        for (size_t i = 0; i < in_dim_; ++i) {
            for (size_t o = 0; o < out_dim_; ++o) {
                float delta = 0.0f;
                for (int r = 0; r < rank_; ++r) {
                    delta += lora_a_[i * rank_ + r] * lora_b_[r * out_dim_ + o];
                }
                weight[i * out_dim_ + o] += delta * scaling_;
            }
        }
        merged_ = true;
    }

    // فك دمج أوزان LoRA من الأوزان
    void unmerge(std::vector<float>& weight) {
        assert(weight.size() == in_dim_ * out_dim_);
        if (!merged_) return;
        for (size_t i = 0; i < in_dim_; ++i) {
            for (size_t o = 0; o < out_dim_; ++o) {
                float delta = 0.0f;
                for (int r = 0; r < rank_; ++r) {
                    delta += lora_a_[i * rank_ + r] * lora_b_[r * out_dim_ + o];
                }
                weight[i * out_dim_ + o] -= delta * scaling_;
            }
        }
        merged_ = false;
    }

    bool is_merged() const { return merged_; }
    int rank() const { return rank_; }
    size_t param_count() const { return in_dim_ * rank_ + rank_ * out_dim_; }
};

class LoRAAdapter {
    LoRAConfig config_;
    std::unordered_map<std::string, std::unique_ptr<LoRALayer>> layers_;

public:
    explicit LoRAAdapter(const LoRAConfig& config) : config_(config) {}

    // إضافة طبقة LoRA لوحدة مستهدفة
    void add_layer(const std::string& name, size_t in_dim, size_t out_dim) {
        layers_[name] = std::make_unique<LoRALayer>(in_dim, out_dim, config_.rank, config_.alpha);
    }

    // اختيار الرتبة المثلى بناءً على أهمية الوحدة
    int select_rank(const std::string& module_name, float importance_score) {
        int base_rank = config_.rank;
        if (importance_score > 0.8f) return base_rank * 2;
        if (importance_score < 0.3f) return std::max(1, base_rank / 2);
        return base_rank;
    }

    size_t total_params() const {
        size_t total = 0;
        for (const auto& [name, layer] : layers_) total += layer->param_count();
        return total;
    }

    bool has_layer(const std::string& name) const { return layers_.count(name) > 0; }
};

} // namespace sad::ai
