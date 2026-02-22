// ===================================================================
// صدOS - إطار عمل برامج تشغيل الأجهزة الموحّد
// المرحلة 52: نظام إدارة الأجهزة والتوصيل الساخن
// ===================================================================

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace sad::os::hal {

/// حالة الجهاز في النظام
enum class DeviceState {
    Unknown     = 0,  // غير معروف
    Detected    = 1,  // تم اكتشافه
    Initialized = 2,  // تمت تهيئته
    Running     = 3,  // يعمل
    Suspended   = 4,  // معلّق
    Error       = 5,  // خطأ
    Removed     = 6   // أُزيل
};

/// نوع ناقل الجهاز
enum class BusType {
    PCI,      // ناقل PCI/PCIe
    USB,      // ناقل USB
    I2C,      // ناقل I2C
    SPI,      // ناقل SPI
    MMIO,     // أجهزة معيّنة في الذاكرة
    Virtual   // أجهزة افتراضية
};

/// معلومات تعريف الجهاز
struct DeviceInfo {
    std::string name;          // اسم الجهاز
    uint32_t vendor_id;        // معرّف الشركة المصنّعة
    uint32_t device_id;        // معرّف الجهاز
    BusType bus;               // نوع الناقل
    uint64_t base_address;     // العنوان الأساسي
    uint32_t irq;              // رقم المقاطعة
};

// === الفئة الأساسية لبرنامج تشغيل الجهاز ===

/// فئة أساسية مجردة لجميع برامج التشغيل
class DeviceDriver {
public:
    virtual ~DeviceDriver() = default;

    /// تهيئة برنامج التشغيل والجهاز
    virtual bool init(const DeviceInfo& info) = 0;
    /// بدء تشغيل الجهاز
    virtual bool start() = 0;
    /// إيقاف الجهاز
    virtual void stop() = 0;
    /// تعليق الجهاز (لإدارة الطاقة)
    virtual bool suspend() { state_ = DeviceState::Suspended; return true; }
    /// استئناف عمل الجهاز
    virtual bool resume() { state_ = DeviceState::Running; return true; }

    DeviceState state() const { return state_; }
    const DeviceInfo& info() const { return info_; }

protected:
    DeviceState state_ = DeviceState::Unknown;
    DeviceInfo info_{};
};

// === مدير برامج التشغيل ===

/// مدير مركزي لاكتشاف وإدارة برامج التشغيل
class DriverManager {
public:
    using DriverFactory = std::function<std::unique_ptr<DeviceDriver>()>;
    using HotPlugCallback = std::function<void(const DeviceInfo&, bool /*added*/)>;

    /// تسجيل مصنع برنامج تشغيل لنوع جهاز محدد
    void register_driver(uint32_t vendor_id, uint32_t device_id,
                         DriverFactory factory) {
        uint64_t key = (static_cast<uint64_t>(vendor_id) << 32) | device_id;
        factories_[key] = std::move(factory);
    }

    /// اكتشاف الأجهزة المتصلة تلقائياً
    int discover_devices() {
        int found = 0;
        // مسح ناقل PCI للأجهزة المتصلة
        found += scan_pci_bus();
        // مسح أجهزة USB
        found += scan_usb_bus();
        return found;
    }

    /// معالجة حدث توصيل ساخن (إضافة أو إزالة جهاز)
    bool handle_hotplug(const DeviceInfo& dev, bool added) {
        if (added) {
            auto driver = find_driver(dev.vendor_id, dev.device_id);
            if (driver && driver->init(dev) && driver->start()) {
                active_drivers_.push_back(std::move(driver));
                notify_hotplug(dev, true);
                return true;
            }
        } else {
            // البحث عن برنامج التشغيل وإيقافه
            return remove_device(dev.device_id);
        }
        return false;
    }

    /// تسجيل مراقب لأحداث التوصيل الساخن
    void on_hotplug(HotPlugCallback cb) { hotplug_cbs_.push_back(std::move(cb)); }

    size_t active_count() const { return active_drivers_.size(); }

private:
    std::unordered_map<uint64_t, DriverFactory> factories_;
    std::vector<std::unique_ptr<DeviceDriver>> active_drivers_;
    std::vector<HotPlugCallback> hotplug_cbs_;

    /// إنشاء برنامج تشغيل مناسب للجهاز
    std::unique_ptr<DeviceDriver> find_driver(uint32_t vid, uint32_t did) {
        uint64_t key = (static_cast<uint64_t>(vid) << 32) | did;
        auto it = factories_.find(key);
        if (it != factories_.end()) return it->second();
        return nullptr;
    }

    /// إزالة جهاز وإيقاف برنامج تشغيله
    bool remove_device(uint32_t device_id) {
        for (auto it = active_drivers_.begin(); it != active_drivers_.end(); ++it) {
            if ((*it)->info().device_id == device_id) {
                (*it)->stop();
                active_drivers_.erase(it);
                return true;
            }
        }
        return false;
    }

    /// إشعار المراقبين بحدث التوصيل الساخن
    void notify_hotplug(const DeviceInfo& dev, bool added) {
        for (auto& cb : hotplug_cbs_) cb(dev, added);
    }

    int scan_pci_bus() { return 0; }  // مسح ناقل PCI
    int scan_usb_bus() { return 0; }  // مسح ناقل USB
};

} // namespace sad::os::hal
