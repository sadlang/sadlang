// ملف: engine.cpp
// الوصف: محرك الاستدلال مع التجميع وأخذ العينات top-k/top-p ودرجة الحرارة
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <random>
#include <queue>
#include <functional>
#include <memory>
#include <cassert>

namespace sad::ai {

struct SamplingConfig {
    float temperature = 1.0f;
    int top_k = 50;
    float top_p = 0.9f;
    float repetition_penalty = 1.1f;
    int max_tokens = 512;
    int seed = -1;
    std::vector<int> stop_tokens;
};

struct InferenceRequest {
    std::vector<int> token_ids;
    SamplingConfig sampling;
    std::string request_id;
    int priority = 0;
};

struct InferenceResult {
    std::vector<int> generated_tokens;
    std::vector<float> token_logprobs;
    std::string request_id;
    bool finished = false;
};

class InferenceEngine {
    SamplingConfig default_config_;
    std::mt19937 rng_;
    std::vector<InferenceRequest> batch_;
    int max_batch_size_;

public:
    InferenceEngine(int max_batch_size = 32, int seed = 42)
        : max_batch_size_(max_batch_size), rng_(seed) {}

    // تطبيق درجة الحرارة على اللوغاريتمات
    void apply_temperature(std::vector<float>& logits, float temperature) {
        if (temperature <= 0.0f) return;
        float inv_temp = 1.0f / temperature;
        for (auto& l : logits) l *= inv_temp;
    }

    // عقوبة التكرار
    void apply_repetition_penalty(std::vector<float>& logits,
                                   const std::vector<int>& generated, float penalty) {
        for (int tok : generated) {
            if (tok >= 0 && tok < static_cast<int>(logits.size())) {
                logits[tok] = logits[tok] > 0 ? logits[tok] / penalty : logits[tok] * penalty;
            }
        }
    }

    // أخذ عينات Top-K
    int sample_top_k(std::vector<float>& logits, int k) {
        std::vector<std::pair<float, int>> scored;
        for (int i = 0; i < static_cast<int>(logits.size()); ++i) {
            scored.emplace_back(logits[i], i);
        }
        std::partial_sort(scored.begin(), scored.begin() + std::min(k, (int)scored.size()),
                          scored.end(), [](auto& a, auto& b) { return a.first > b.first; });
        scored.resize(std::min(k, (int)scored.size()));
        return sample_from_candidates(scored);
    }

    // أخذ عينات Top-P (أخذ عينات النواة)
    int sample_top_p(std::vector<float>& logits, float p) {
        softmax(logits);
        std::vector<std::pair<float, int>> scored;
        for (int i = 0; i < static_cast<int>(logits.size()); ++i) {
            scored.emplace_back(logits[i], i);
        }
        std::sort(scored.begin(), scored.end(), [](auto& a, auto& b) { return a.first > b.first; });

        float cumsum = 0.0f;
        std::vector<std::pair<float, int>> candidates;
        for (auto& [prob, idx] : scored) {
            cumsum += prob;
            candidates.emplace_back(prob, idx);
            if (cumsum >= p) break;
        }
        return sample_from_candidates(candidates);
    }

    // أخذ عينة من قائمة المرشحين
    int sample_from_candidates(const std::vector<std::pair<float, int>>& candidates) {
        std::vector<float> probs;
        for (auto& [p, _] : candidates) probs.push_back(p);
        softmax(probs);
        std::discrete_distribution<int> dist(probs.begin(), probs.end());
        return candidates[dist(rng_)].second;
    }

    // تطبيق softmax
    void softmax(std::vector<float>& logits) {
        float max_val = *std::max_element(logits.begin(), logits.end());
        float sum = 0.0f;
        for (auto& l : logits) { l = std::exp(l - max_val); sum += l; }
        for (auto& l : logits) l /= (sum + 1e-9f);
    }

    // توليد رمز واحد
    int generate_token(std::vector<float> logits, const SamplingConfig& config,
                       const std::vector<int>& context) {
        apply_repetition_penalty(logits, context, config.repetition_penalty);
        apply_temperature(logits, config.temperature);
        if (config.top_k > 0) return sample_top_k(logits, config.top_k);
        return sample_top_p(logits, config.top_p);
    }

    // إضافة طلب إلى الدُّفعة
    bool add_request(const InferenceRequest& req) {
        if (static_cast<int>(batch_.size()) >= max_batch_size_) return false;
        batch_.push_back(req);
        return true;
    }

    // التحقق من شروط التوقف
    bool should_stop(int token, const SamplingConfig& config, int generated_count) {
        if (generated_count >= config.max_tokens) return true;
        for (int stop : config.stop_tokens) {
            if (token == stop) return true;
        }
        return false;
    }

    size_t batch_size() const { return batch_.size(); }
    void clear_batch() { batch_.clear(); }
};

} // namespace sad::ai
