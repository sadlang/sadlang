// ===================================================================
// صدOS - طبقة تجريد العتاد (HAL) - معالج x86_64
// المرحلة 52: دعم معماريات متعددة
// ===================================================================

#include <cstdint>

namespace sad::os::hal::x86_64 {

// === هياكل البيانات الأساسية ===

/// مدخل في جدول الواصفات العامة (GDT)
struct GDTEntry {
    uint16_t limit_low;      // الحد الأدنى للقطعة
    uint16_t base_low;       // العنوان الأساسي - الجزء السفلي
    uint8_t  base_middle;    // العنوان الأساسي - الجزء الأوسط
    uint8_t  access;         // بايت الوصول
    uint8_t  granularity;    // الدقة والحد العلوي
    uint8_t  base_high;      // العنوان الأساسي - الجزء العلوي
} __attribute__((packed));

/// مدخل في جدول واصفات المقاطعات (IDT)
struct IDTEntry {
    uint16_t offset_low;     // إزاحة المعالج - الجزء السفلي
    uint16_t selector;       // محدد قطعة الكود
    uint8_t  ist;            // فهرس جدول المكدس
    uint8_t  type_attr;      // نوع البوابة وصلاحياتها
    uint16_t offset_mid;     // إزاحة المعالج - الجزء الأوسط
    uint32_t offset_high;    // إزاحة المعالج - الجزء العلوي
    uint32_t reserved;       // محجوز
} __attribute__((packed));

/// معلومات المعالج من تعليمة CPUID
struct CPUIDInfo {
    uint32_t eax, ebx, ecx, edx;
};

// === فئة المعالج x86_64 ===

/// فئة إدارة معالج x86_64 - تهيئة وإدارة المعالج
class X86CPU {
public:
    static constexpr int GDT_ENTRIES = 8;    // عدد مدخلات GDT
    static constexpr int IDT_ENTRIES = 256;  // عدد مدخلات IDT

    /// تهيئة المعالج الكاملة
    bool initialize() {
        setup_gdt();    // إعداد جدول الواصفات العامة
        setup_idt();    // إعداد جدول واصفات المقاطعات
        detect_features(); // اكتشاف ميزات المعالج
        initialized_ = true;
        return true;
    }

    /// قراءة سجل MSR (سجلات خاصة بالطراز)
    uint64_t read_msr(uint32_t msr_id) const {
        uint32_t low, high;
        // تعليمة rdmsr تقرأ السجل المحدد
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr_id));
        return (static_cast<uint64_t>(high) << 32) | low;
    }

    /// كتابة إلى سجل MSR
    void write_msr(uint32_t msr_id, uint64_t value) {
        uint32_t low = static_cast<uint32_t>(value);
        uint32_t high = static_cast<uint32_t>(value >> 32);
        asm volatile("wrmsr" : : "c"(msr_id), "a"(low), "d"(high));
    }

    /// تنفيذ تعليمة CPUID لاكتشاف ميزات المعالج
    CPUIDInfo cpuid(uint32_t leaf, uint32_t subleaf = 0) const {
        CPUIDInfo info{};
        asm volatile("cpuid"
            : "=a"(info.eax), "=b"(info.ebx), "=c"(info.ecx), "=d"(info.edx)
            : "a"(leaf), "c"(subleaf));
        return info;
    }

    /// قراءة سجل التحكم CR0
    uint64_t read_cr0() const {
        uint64_t val;
        asm volatile("mov %%cr0, %0" : "=r"(val));
        return val;
    }

    /// قراءة سجل التحكم CR3 (جدول الصفحات)
    uint64_t read_cr3() const {
        uint64_t val;
        asm volatile("mov %%cr3, %0" : "=r"(val));
        return val;
    }

    /// كتابة إلى سجل التحكم CR3 - تحديث جدول الصفحات
    void write_cr3(uint64_t value) {
        asm volatile("mov %0, %%cr3" : : "r"(value) : "memory");
    }

    bool has_sse() const { return features_sse_; }
    bool has_avx() const { return features_avx_; }

private:
    GDTEntry gdt_[GDT_ENTRIES]{};  // جدول الواصفات العامة
    IDTEntry idt_[IDT_ENTRIES]{};  // جدول واصفات المقاطعات
    bool initialized_ = false;
    bool features_sse_ = false;    // دعم تعليمات SSE
    bool features_avx_ = false;    // دعم تعليمات AVX

    /// إعداد جدول الواصفات العامة
    void setup_gdt() {
        gdt_[0] = {};                                          // المدخل الفارغ
        gdt_[1] = {0xFFFF, 0, 0, 0x9A, 0xAF, 0};            // قطعة كود النواة
        gdt_[2] = {0xFFFF, 0, 0, 0x92, 0xCF, 0};            // قطعة بيانات النواة
        gdt_[3] = {0xFFFF, 0, 0, 0xFA, 0xAF, 0};            // قطعة كود المستخدم
        gdt_[4] = {0xFFFF, 0, 0, 0xF2, 0xCF, 0};            // قطعة بيانات المستخدم
    }

    /// إعداد جدول واصفات المقاطعات
    void setup_idt() {
        for (int i = 0; i < IDT_ENTRIES; ++i) {
            idt_[i] = {};  // تهيئة افتراضية لكل مدخل
        }
    }

    /// اكتشاف ميزات المعالج عبر CPUID
    void detect_features() {
        auto info = cpuid(1);
        features_sse_ = (info.edx >> 25) & 1;  // فحص بت SSE
        features_avx_ = (info.ecx >> 28) & 1;  // فحص بت AVX
    }
};

} // namespace sad::os::hal::x86_64
