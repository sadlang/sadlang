// ===================================================================
// صدOS - مدير النوافذ بدعم RTL أولاً
// المرحلة 52: واجهة المستخدم - تخطيط عربي أصيل
// ===================================================================

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <map>

namespace sad::os::ui {

/// اتجاه التخطيط
enum class LayoutDirection : uint8_t {
    RTL,    // من اليمين لليسار (الافتراضي)
    LTR     // من اليسار لليمين
};

/// وضع النافذة
enum class WindowMode : uint8_t {
    FLOATING,       // عائمة حرة
    TILED,          // مبلّطة
    MAXIMIZED,      // مكبّرة
    FULLSCREEN      // ملء الشاشة
};

/// خصائص النافذة
struct WindowProperties {
    std::u32string title;          // عنوان النافذة (يونيكود كامل)
    int32_t x, y;                  // الموقع
    uint32_t width, height;        // الأبعاد
    WindowMode mode;               // وضع العرض
    LayoutDirection direction;     // اتجاه المحتوى
    bool decorated;                // هل لها إطار؟
    float opacity;                 // الشفافية
};

/// مساحة العمل الافتراضية
class Workspace {
    uint32_t id_;                                  // رقم المساحة
    std::u32string name_;                          // اسم المساحة بالعربية
    std::vector<uint32_t> window_ids_;             // نوافذ هذه المساحة
    LayoutDirection layout_dir_;                   // اتجاه التبليط

public:
    Workspace(uint32_t id, const std::u32string& name)
        : id_(id), name_(name), layout_dir_(LayoutDirection::RTL) {}

    /// إضافة نافذة للمساحة
    void add_window(uint32_t win_id) {
        window_ids_.push_back(win_id);
    }

    /// إزالة نافذة من المساحة
    void remove_window(uint32_t win_id) {
        window_ids_.erase(
            std::remove(window_ids_.begin(), window_ids_.end(), win_id),
            window_ids_.end()
        );
    }

    /// إعادة ترتيب النوافذ حسب التخطيط RTL
    void retile(uint32_t screen_w, uint32_t screen_h) {
        if (window_ids_.empty()) return;
        uint32_t count = window_ids_.size();
        uint32_t tile_w = screen_w / count;
        // في RTL: النافذة الأولى على اليمين
        for (uint32_t i = 0; i < count; i++) {
            uint32_t x_pos = (layout_dir_ == LayoutDirection::RTL)
                ? screen_w - (i + 1) * tile_w
                : i * tile_w;
            // تطبيق الموقع الجديد على النافذة
            (void)x_pos;
        }
    }

    uint32_t id() const { return id_; }
    size_t window_count() const { return window_ids_.size(); }
};

/// عنصر واجهة عربي أساسي
class ArabicWidget {
protected:
    std::u32string label_;         // النص بالعربية
    int32_t x_, y_;                // الموقع
    uint32_t w_, h_;               // الأبعاد
    LayoutDirection dir_;          // اتجاه النص

public:
    ArabicWidget(const std::u32string& label, uint32_t w, uint32_t h)
        : label_(label), x_(0), y_(0), w_(w), h_(h),
          dir_(LayoutDirection::RTL) {}

    virtual ~ArabicWidget() = default;

    /// رسم العنصر - يُعاد تعريفه في الأصناف الفرعية
    virtual void render() = 0;
};

/// زر بواجهة عربية
class ArabicButton : public ArabicWidget {
    std::function<void()> on_click_;   // دالة النقر

public:
    ArabicButton(const std::u32string& label, uint32_t w, uint32_t h)
        : ArabicWidget(label, w, h) {}

    void set_on_click(std::function<void()> handler) {
        on_click_ = std::move(handler);
    }

    void render() override {
        // رسم الزر مع محاذاة النص لليمين (RTL)
    }
};

/// مدير النوافذ الرئيسي - RTL أولاً
class RTLWindowManager {
    std::map<uint32_t, WindowProperties> windows_;     // النوافذ المُدارة
    std::vector<std::unique_ptr<Workspace>> workspaces_; // مساحات العمل
    uint32_t active_workspace_;                         // المساحة النشطة
    uint32_t next_window_id_;                           // معرّف النافذة التالي
    LayoutDirection global_direction_;                  // الاتجاه العام

public:
    RTLWindowManager()
        : active_workspace_(0), next_window_id_(1),
          global_direction_(LayoutDirection::RTL) {
        // إنشاء مساحة العمل الافتراضية
        workspaces_.push_back(
            std::make_unique<Workspace>(0, U"سطح المكتب"));
    }

    /// إنشاء نافذة جديدة
    uint32_t create_window(const std::u32string& title, uint32_t w, uint32_t h) {
        uint32_t id = next_window_id_++;
        WindowProperties props{title, 0, 0, w, h,
            WindowMode::FLOATING, global_direction_, true, 1.0f};
        windows_[id] = props;
        workspaces_[active_workspace_]->add_window(id);
        return id;
    }

    /// تبديل وضع النافذة (عائمة / مبلّطة)
    void toggle_mode(uint32_t win_id) {
        auto it = windows_.find(win_id);
        if (it == windows_.end()) return;
        auto& mode = it->second.mode;
        mode = (mode == WindowMode::FLOATING)
            ? WindowMode::TILED : WindowMode::FLOATING;
    }

    /// التبديل إلى مساحة عمل أخرى
    void switch_workspace(uint32_t ws_id) {
        if (ws_id < workspaces_.size()) {
            active_workspace_ = ws_id;
        }
    }
};

} // namespace sad::os::ui
