// ملف: rlhf.cpp
// الوصف: مدرب RLHF مع نموذج المكافأة وتحديثات PPO وتعلم التفضيلات
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <functional>
#include <random>
#include <cassert>

namespace sad::ai {

struct RLHFConfig {
    float ppo_clip_range = 0.2f;
    float value_clip_range = 0.2f;
    float kl_coeff = 0.1f;
    float gamma = 0.99f;
    float lam = 0.95f;
    int ppo_epochs = 4;
    int batch_size = 64;
    float reward_baseline = 0.0f;
};

struct PreferencePair {
    std::vector<int> chosen_tokens;
    std::vector<int> rejected_tokens;
    std::string prompt;
    float margin = 0.0f;
};

class RewardModel {
    size_t hidden_dim_;
    std::vector<float> reward_head_;

public:
    explicit RewardModel(size_t hidden_dim) : hidden_dim_(hidden_dim) {
        reward_head_.resize(hidden_dim, 0.01f);
    }

    // حساب المكافأة لتسلسل مخفي
    float compute_reward(const std::vector<float>& hidden_state) {
        assert(hidden_state.size() == hidden_dim_);
        float reward = 0.0f;
        for (size_t i = 0; i < hidden_dim_; ++i) {
            reward += hidden_state[i] * reward_head_[i];
        }
        return std::tanh(reward);
    }

    // تدريب على أزواج التفضيل
    float preference_loss(float chosen_reward, float rejected_reward) {
        float diff = chosen_reward - rejected_reward;
        return -std::log(1.0f / (1.0f + std::exp(-diff)) + 1e-8f);
    }

    // تحديث الأوزان بناءً على خسارة التفضيل
    void update(const std::vector<float>& chosen_hidden,
                const std::vector<float>& rejected_hidden, float lr) {
        float r_c = compute_reward(chosen_hidden);
        float r_r = compute_reward(rejected_hidden);
        float sigmoid = 1.0f / (1.0f + std::exp(-(r_c - r_r)));
        float grad_scale = (sigmoid - 1.0f) * lr;

        for (size_t i = 0; i < hidden_dim_; ++i) {
            reward_head_[i] -= grad_scale * (chosen_hidden[i] - rejected_hidden[i]);
        }
    }
};

class PPOTrainer {
    RLHFConfig config_;
    std::vector<float> old_log_probs_;
    std::vector<float> values_;
    std::vector<float> advantages_;

public:
    explicit PPOTrainer(const RLHFConfig& config) : config_(config) {}

    // حساب الميزة المعممة (GAE)
    std::vector<float> compute_gae(const std::vector<float>& rewards,
                                    const std::vector<float>& values) {
        size_t T = rewards.size();
        std::vector<float> advantages(T, 0.0f);
        float gae = 0.0f;
        for (int t = static_cast<int>(T) - 1; t >= 0; --t) {
            float next_val = (t + 1 < static_cast<int>(T)) ? values[t + 1] : 0.0f;
            float delta = rewards[t] + config_.gamma * next_val - values[t];
            gae = delta + config_.gamma * config_.lam * gae;
            advantages[t] = gae;
        }
        return advantages;
    }

    // حساب خسارة PPO المقصوصة
    float ppo_loss(float log_prob, float old_log_prob, float advantage) {
        float ratio = std::exp(log_prob - old_log_prob);
        float clipped = std::clamp(ratio, 1.0f - config_.ppo_clip_range,
                                   1.0f + config_.ppo_clip_range);
        return -std::min(ratio * advantage, clipped * advantage);
    }

    // حساب عقوبة KL بين السياسة الحالية والمرجعية
    float kl_penalty(float log_prob, float ref_log_prob) {
        return config_.kl_coeff * (std::exp(log_prob - ref_log_prob) - 1.0f - (log_prob - ref_log_prob));
    }

    // خطوة تحديث PPO
    float update_step(const std::vector<float>& log_probs,
                      const std::vector<float>& old_log_probs,
                      const std::vector<float>& advantages) {
        float total_loss = 0.0f;
        for (size_t i = 0; i < log_probs.size(); ++i) {
            total_loss += ppo_loss(log_probs[i], old_log_probs[i], advantages[i]);
        }
        return total_loss / static_cast<float>(log_probs.size());
    }
};

class RLHFTrainer {
    RLHFConfig config_;
    RewardModel reward_model_;
    PPOTrainer ppo_trainer_;

public:
    RLHFTrainer(const RLHFConfig& config, size_t hidden_dim)
        : config_(config), reward_model_(hidden_dim), ppo_trainer_(config) {}

    // تدريب نموذج المكافأة على بيانات التفضيلات
    float train_reward_model(const std::vector<PreferencePair>& pairs,
                              const std::vector<std::vector<float>>& chosen_hiddens,
                              const std::vector<std::vector<float>>& rejected_hiddens, float lr) {
        float total_loss = 0.0f;
        for (size_t i = 0; i < pairs.size(); ++i) {
            float r_c = reward_model_.compute_reward(chosen_hiddens[i]);
            float r_r = reward_model_.compute_reward(rejected_hiddens[i]);
            total_loss += reward_model_.preference_loss(r_c, r_r);
            reward_model_.update(chosen_hiddens[i], rejected_hiddens[i], lr);
        }
        return total_loss / static_cast<float>(pairs.size());
    }

    RewardModel& reward_model() { return reward_model_; }
    PPOTrainer& ppo() { return ppo_trainer_; }
};

} // namespace sad::ai
