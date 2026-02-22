// ===================================================================
// صدOS - طبقة تجريد العتاد (HAL) - معالج ARM64
// المرحلة 52: دعم معمارية ARM64 (AArch64)
// ===================================================================

#include <cstdint>
#include <array>

namespace sad::os::hal::arm64 {

// === ثوابت مستويات الاستثناء ===
enum class ExceptionLevel : uint8_t {
    EL0 = 0,  // مستوى التطبيقات
    EL1 = 1,  // مستوى نواة نظام التشغيل
    EL2 = 2,  // مستوى المراقب الافتراضي (Hypervisor)
    EL3 = 3   // مستوى المراقب الآمن (Secure Monitor)
};

/// أنواع المقاطعات في GIC
enum class InterruptType : uint8_t {
    SGI  = 0,  // مقاطعة مولّدة برمجياً
    PPI  = 1,  // مقاطعة خاصة بالمعالج
    SPI  = 2,  // مقاطعة مشتركة بين المعالجات
    LPI  = 3   // مقاطعة محلية للطرفيات
};

/// معلومات مقاطعة في وحدة التحكم العامة
struct GICInterrupt {
    uint32_t id;           // معرّف المقاطعة
    InterruptType type;    // نوع المقاطعة
    uint8_t priority;      // أولوية المقاطعة (0 = الأعلى)
    uint8_t target_cpu;    // المعالج المستهدف
    bool enabled;          // هل المقاطعة مفعّلة؟
};

// === فئة معالج ARM64 ===

/// إدارة معالج ARM64 - سجلات النظام ووحدة المقاطعات
class ARM64CPU {
public:
    static constexpr int MAX_INTERRUPTS = 1024;  // أقصى عدد مقاطعات GIC
    static constexpr int MAX_CPUS = 8;           // أقصى عدد أنوية

    /// تهيئة المعالج ووحدة المقاطعات
    bool initialize() {
        current_el_ = read_exception_level();
        setup_exception_vectors();  // إعداد جدول الاستثناءات
        init_gic();                 // تهيئة وحدة التحكم بالمقاطعات
        initialized_ = true;
        return true;
    }

    /// قراءة مستوى الاستثناء الحالي
    ExceptionLevel read_exception_level() const {
        uint64_t current_el;
        asm volatile("mrs %0, CurrentEL" : "=r"(current_el));
        return static_cast<ExceptionLevel>((current_el >> 2) & 0x3);
    }

    /// قراءة سجل نظام عام باستخدام اسمه الرقمي
    uint64_t read_system_register(const char* /*reg_name*/) const {
        // في التنفيذ الفعلي يُستخدم mrs مع السجل المطلوب
        uint64_t val = 0;
        return val;
    }

    /// قراءة عداد الدورات (Performance Counter)
    uint64_t read_cycle_counter() const {
        uint64_t val;
        asm volatile("mrs %0, PMCCNTR_EL0" : "=r"(val));
        return val;
    }

    /// تفعيل مقاطعة في وحدة التحكم العامة GIC
    bool gic_enable_interrupt(uint32_t irq_id, uint8_t priority, uint8_t cpu) {
        if (irq_id >= MAX_INTERRUPTS) return false;
        interrupts_[irq_id] = {irq_id, classify_interrupt(irq_id),
                               priority, cpu, true};
        gic_write_enable(irq_id);   // كتابة بت التفعيل
        gic_set_priority(irq_id, priority);
        return true;
    }

    /// تعطيل مقاطعة في GIC
    void gic_disable_interrupt(uint32_t irq_id) {
        if (irq_id < MAX_INTERRUPTS) {
            interrupts_[irq_id].enabled = false;
            gic_write_disable(irq_id);
        }
    }

    /// إرسال مقاطعة برمجية SGI إلى معالج آخر
    void send_sgi(uint8_t sgi_id, uint8_t target_cpu) {
        if (sgi_id > 15 || target_cpu >= MAX_CPUS) return;
        uint64_t val = (static_cast<uint64_t>(target_cpu) << 16) | sgi_id;
        (void)val; // يُكتب إلى سجل ICC_SGI1R_EL1
    }

    ExceptionLevel current_level() const { return current_el_; }

private:
    ExceptionLevel current_el_ = ExceptionLevel::EL1;
    bool initialized_ = false;
    std::array<GICInterrupt, MAX_INTERRUPTS> interrupts_{};

    /// تصنيف نوع المقاطعة حسب رقمها
    InterruptType classify_interrupt(uint32_t id) const {
        if (id < 16)  return InterruptType::SGI;
        if (id < 32)  return InterruptType::PPI;
        if (id < 1020) return InterruptType::SPI;
        return InterruptType::LPI;
    }

    /// إعداد جدول متجهات الاستثناءات
    void setup_exception_vectors() {
        // يُحمّل عنوان جدول المتجهات إلى VBAR_EL1
    }

    /// تهيئة وحدة التحكم العامة بالمقاطعات
    void init_gic() {
        // تهيئة الموزع (Distributor) وواجهة المعالج
    }

    void gic_write_enable(uint32_t irq_id)  { (void)irq_id; }
    void gic_write_disable(uint32_t irq_id) { (void)irq_id; }
    void gic_set_priority(uint32_t irq_id, uint8_t pri) {
        (void)irq_id; (void)pri;
    }
};

} // namespace sad::os::hal::arm64
