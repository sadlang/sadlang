// ===================================================================
// صدOS - نظام التركيب المتوافق مع Wayland
// المرحلة 52: واجهة المستخدم الرسومية لنظام صدOS
// ===================================================================

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <mutex>

namespace sad::os::ui {

/// معرّف السطح الفريد
using SurfaceId = uint64_t;

/// تنسيق البكسل المدعوم
enum class PixelFormat : uint8_t {
    ARGB8888,   // اللون مع الشفافية
    XRGB8888,   // اللون بدون شفافية
    RGB565,     // لون مضغوط
    NV12        // فيديو YUV
};

/// مستطيل على الشاشة
struct Rect {
    int32_t x, y;
    uint32_t width, height;
};

/// سطح الرسم - الوحدة الأساسية للتركيب
class Surface {
public:
    SurfaceId id_;             // المعرّف الفريد
    Rect geometry_;            // الموقع والحجم
    PixelFormat format_;       // تنسيق البكسل
    uint32_t* buffer_;         // مخزن البكسلات
    bool visible_;             // هل السطح مرئي؟
    int32_t z_order_;          // ترتيب العمق
    float opacity_;            // مستوى الشفافية

    Surface(SurfaceId id, uint32_t w, uint32_t h)
        : id_(id), format_(PixelFormat::ARGB8888),
          buffer_(nullptr), visible_(true), z_order_(0), opacity_(1.0f) {
        geometry_ = {0, 0, w, h};
    }

    /// تحديث محتوى السطح من المخزن المؤقت
    void commit(const uint32_t* data, size_t size) {
        if (buffer_ && data) {
            std::memcpy(buffer_, data, size);
        }
    }

    /// هل النقطة داخل حدود السطح؟
    bool contains(int32_t px, int32_t py) const {
        return px >= geometry_.x && px < geometry_.x + (int32_t)geometry_.width &&
               py >= geometry_.y && py < geometry_.y + (int32_t)geometry_.height;
    }
};

/// جدولة الإطارات - مزامنة مع VSync
class FrameScheduler {
    uint32_t target_fps_;       // معدل الإطارات المستهدف
    uint64_t frame_count_;      // عدّاد الإطارات
    uint64_t last_frame_ns_;    // وقت آخر إطار بالنانو ثانية

public:
    explicit FrameScheduler(uint32_t fps = 60)
        : target_fps_(fps), frame_count_(0), last_frame_ns_(0) {}

    /// هل حان وقت الإطار التالي؟
    bool should_render(uint64_t current_ns) const {
        uint64_t interval = 1000000000ULL / target_fps_;
        return (current_ns - last_frame_ns_) >= interval;
    }

    /// تسجيل اكتمال الإطار
    void frame_done(uint64_t ns) {
        last_frame_ns_ = ns;
        frame_count_++;
    }
};

/// خط الأنابيب للتركيب - DRM/KMS
class CompositionPipeline {
    int drm_fd_;                // واصف ملف DRM
    uint32_t crtc_id_;          // معرّف وحدة التحكم بالعرض
    uint32_t connector_id_;     // معرّف الموصل

public:
    CompositionPipeline() : drm_fd_(-1), crtc_id_(0), connector_id_(0) {}

    /// تهيئة اتصال DRM/KMS
    bool initialize(const std::string& device_path) {
        // فتح جهاز DRM: /dev/dri/card0
        // drm_fd_ = open(device_path.c_str(), O_RDWR);
        return drm_fd_ >= 0;
    }

    /// تبديل المخزن المؤقت للعرض (page flip)
    bool page_flip(uint32_t framebuffer_id) {
        // drmModePageFlip(drm_fd_, crtc_id_, framebuffer_id, ...);
        return true;
    }
};

/// المُركّب الرئيسي - متوافق مع بروتوكول Wayland
class Compositor {
    std::vector<std::unique_ptr<Surface>> surfaces_;   // قائمة الأسطح
    FrameScheduler scheduler_;                          // مجدول الإطارات
    CompositionPipeline pipeline_;                      // خط أنابيب العرض
    std::mutex mutex_;                                  // قفل التزامن
    SurfaceId next_id_;                                 // المعرّف التالي

public:
    Compositor() : next_id_(1) {}

    /// إنشاء سطح جديد
    SurfaceId create_surface(uint32_t width, uint32_t height) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto id = next_id_++;
        surfaces_.push_back(std::make_unique<Surface>(id, width, height));
        return id;
    }

    /// تدمير سطح موجود
    void destroy_surface(SurfaceId id) {
        std::lock_guard<std::mutex> lock(mutex_);
        surfaces_.erase(
            std::remove_if(surfaces_.begin(), surfaces_.end(),
                [id](const auto& s) { return s->id_ == id; }),
            surfaces_.end()
        );
    }

    /// تركيب جميع الأسطح المرئية في الإطار النهائي
    void compose_frame() {
        std::lock_guard<std::mutex> lock(mutex_);
        // ترتيب الأسطح حسب العمق
        std::sort(surfaces_.begin(), surfaces_.end(),
            [](const auto& a, const auto& b) {
                return a->z_order_ < b->z_order_;
            });
        // رسم كل سطح مرئي بالترتيب
        for (const auto& surface : surfaces_) {
            if (surface->visible_) {
                // دمج السطح في المخزن النهائي
            }
        }
    }

    /// الحلقة الرئيسية للمُركّب
    void run() {
        pipeline_.initialize("/dev/dri/card0");
        // حلقة الأحداث: معالجة المدخلات، تركيب، عرض
    }
};

} // namespace sad::os::ui
