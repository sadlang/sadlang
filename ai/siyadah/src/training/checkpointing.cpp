// ملف: checkpointing.cpp
// الوصف: نقاط تفتيش التدرجات مع تقسيم إلى شرائح وإعادة الحساب
// المشروع: سيادة - محرك الذكاء الاصطناعي للغة صاد
// المرحلة: 54 - الذكاء الاصطناعي

#include <vector>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <cassert>
#include <cmath>

namespace sad::ai {

// تمثيل شريحة نقطة تفتيش واحدة
struct CheckpointSegment {
    int segment_id;
    size_t start_layer;
    size_t end_layer;
    std::vector<float> saved_input;
    bool needs_recompute;

    CheckpointSegment(int id, size_t start, size_t end)
        : segment_id(id), start_layer(start), end_layer(end), needs_recompute(false) {}
};

// إحصائيات استخدام الذاكرة
struct MemoryStats {
    size_t peak_memory_bytes = 0;
    size_t saved_memory_bytes = 0;
    size_t recompute_count = 0;
    double recompute_overhead_ms = 0.0;
};

using LayerFn = std::function<std::vector<float>(const std::vector<float>&)>;

class GradientCheckpointing {
    std::vector<CheckpointSegment> segments_;
    std::vector<LayerFn> layer_fns_;
    size_t num_layers_;
    size_t segment_size_;
    MemoryStats stats_;
    bool enabled_;

public:
    GradientCheckpointing(size_t num_layers, size_t segment_size = 4)
        : num_layers_(num_layers), segment_size_(segment_size), enabled_(true) {
        create_segments();
    }

    // إنشاء شرائح نقاط التفتيش
    void create_segments() {
        segments_.clear();
        int seg_id = 0;
        for (size_t i = 0; i < num_layers_; i += segment_size_) {
            size_t end = std::min(i + segment_size_, num_layers_);
            segments_.emplace_back(seg_id++, i, end);
        }
    }

    // تسجيل دالة الطبقة
    void register_layer(LayerFn fn) {
        layer_fns_.push_back(std::move(fn));
    }

    // التمرير الأمامي مع حفظ نقاط التفتيش
    std::vector<float> forward(const std::vector<float>& input) {
        std::vector<float> current = input;

        for (auto& segment : segments_) {
            // حفظ المدخلات عند حدود الشريحة
            segment.saved_input = current;
            segment.needs_recompute = enabled_;

            for (size_t l = segment.start_layer; l < segment.end_layer; ++l) {
                if (l < layer_fns_.size()) {
                    current = layer_fns_[l](current);
                }
            }
        }
        return current;
    }

    // إعادة حساب التنشيطات لشريحة معينة أثناء التمرير الخلفي
    std::vector<std::vector<float>> recompute_segment(int segment_id) {
        assert(segment_id >= 0 && segment_id < static_cast<int>(segments_.size()));
        auto& seg = segments_[segment_id];
        stats_.recompute_count++;

        std::vector<std::vector<float>> activations;
        std::vector<float> current = seg.saved_input;
        activations.push_back(current);

        for (size_t l = seg.start_layer; l < seg.end_layer; ++l) {
            if (l < layer_fns_.size()) {
                current = layer_fns_[l](current);
                activations.push_back(current);
            }
        }
        return activations;
    }

    // حساب الذاكرة الموفّرة
    size_t estimate_memory_savings(size_t hidden_dim, size_t batch_size) const {
        size_t full_memory = num_layers_ * hidden_dim * batch_size * sizeof(float);
        size_t checkpoint_memory = segments_.size() * hidden_dim * batch_size * sizeof(float);
        return full_memory - checkpoint_memory;
    }

    // تحديث إحصائيات الذاكرة
    void update_stats(size_t hidden_dim, size_t batch_size) {
        stats_.saved_memory_bytes = estimate_memory_savings(hidden_dim, batch_size);
        stats_.peak_memory_bytes = segments_.size() * hidden_dim * batch_size * sizeof(float);
    }

    // تعيين حجم الشريحة وإعادة البناء
    void set_segment_size(size_t size) {
        segment_size_ = size;
        create_segments();
    }

    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }
    bool is_enabled() const { return enabled_; }
    size_t num_segments() const { return segments_.size(); }
    const MemoryStats& memory_stats() const { return stats_; }
};

} // namespace sad::ai
