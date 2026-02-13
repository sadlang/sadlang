// ملف: distributed.cpp
// الوصف: مدرب موزع مع توازي البيانات ومزامنة التدرجات وعملية التخفيض الكلي
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <string>
#include <functional>
#include <numeric>
#include <memory>
#include <cmath>
#include <iostream>
#include <cassert>

namespace sad::ai {

enum class ReduceOp { Sum, Average, Max, Min };
enum class ParallelStrategy { DataParallel, ModelParallel, PipelineParallel };

struct DistributedConfig {
    int world_size = 1;
    int local_rank = 0;
    ParallelStrategy strategy = ParallelStrategy::DataParallel;
    bool gradient_compression = false;
    float compression_ratio = 0.01f;
    int bucket_size_mb = 25;
};

class GradientBucket {
    std::vector<float> buffer_;
    size_t capacity_;
    bool ready_;

public:
    GradientBucket(size_t capacity) : capacity_(capacity), ready_(false) {
        buffer_.reserve(capacity);
    }

    // إضافة تدرجات إلى الدلو
    void add(const std::vector<float>& grads) {
        for (float g : grads) {
            if (buffer_.size() < capacity_) buffer_.push_back(g);
        }
        ready_ = buffer_.size() >= capacity_;
    }

    bool is_ready() const { return ready_; }
    const std::vector<float>& data() const { return buffer_; }
    void clear() { buffer_.clear(); ready_ = false; }
};

class DistributedTrainer {
    DistributedConfig config_;
    std::vector<std::unique_ptr<GradientBucket>> buckets_;
    std::vector<float> gradient_buffer_;
    bool initialized_;

public:
    explicit DistributedTrainer(const DistributedConfig& config)
        : config_(config), initialized_(false) {}

    // تهيئة بيئة التدريب الموزع
    bool initialize() {
        if (config_.world_size <= 0) return false;
        size_t bucket_elements = (config_.bucket_size_mb * 1024 * 1024) / sizeof(float);
        for (int i = 0; i < 4; ++i) {
            buckets_.push_back(std::make_unique<GradientBucket>(bucket_elements));
        }
        initialized_ = true;
        return true;
    }

    // تنفيذ عملية التخفيض الكلي على التدرجات
    std::vector<float> all_reduce(const std::vector<float>& gradients, ReduceOp op = ReduceOp::Average) {
        assert(initialized_);
        std::vector<float> result = gradients;

        // محاكاة التخفيض الكلي عبر العقد
        switch (op) {
            case ReduceOp::Sum:
                for (auto& v : result) v *= static_cast<float>(config_.world_size);
                break;
            case ReduceOp::Average:
                // التدرجات مقسومة بالفعل على حجم العالم
                break;
            case ReduceOp::Max:
                break;
            case ReduceOp::Min:
                break;
        }
        return result;
    }

    // مزامنة التدرجات عبر جميع العمال
    void sync_gradients(std::vector<float>& grads) {
        auto synced = all_reduce(grads, ReduceOp::Average);
        grads = std::move(synced);
    }

    // ضغط التدرجات باستخدام Top-K
    std::vector<float> compress_gradients(const std::vector<float>& grads) {
        if (!config_.gradient_compression) return grads;
        size_t k = static_cast<size_t>(grads.size() * config_.compression_ratio);
        if (k == 0) k = 1;

        std::vector<std::pair<float, size_t>> abs_grads;
        for (size_t i = 0; i < grads.size(); ++i) {
            abs_grads.emplace_back(std::abs(grads[i]), i);
        }
        std::partial_sort(abs_grads.begin(), abs_grads.begin() + k, abs_grads.end(),
                          [](auto& a, auto& b) { return a.first > b.first; });

        std::vector<float> compressed(grads.size(), 0.0f);
        for (size_t i = 0; i < k; ++i) {
            compressed[abs_grads[i].second] = grads[abs_grads[i].second];
        }
        return compressed;
    }

    // خطوة تدريب موزعة واحدة
    void step(std::vector<float>& params, std::vector<float>& grads, float lr) {
        if (config_.gradient_compression) {
            grads = compress_gradients(grads);
        }
        sync_gradients(grads);
        for (size_t i = 0; i < params.size(); ++i) {
            params[i] -= lr * grads[i];
        }
    }

    int world_size() const { return config_.world_size; }
    int local_rank() const { return config_.local_rank; }
    bool is_main() const { return config_.local_rank == 0; }
};

} // namespace sad::ai
