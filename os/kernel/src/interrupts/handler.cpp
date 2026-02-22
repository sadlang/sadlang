// ===================================================================
// معالج المقاطعات - صدOS
// handler.cpp - Interrupt Handler
// إعداد جدول واصفات المقاطعات وتسجيل معالجات ISR/IRQ
// المرحلة 52 - مشروع لغة صاد
// ===================================================================

#include <cstdint>
#include <cstddef>
#include <array>

namespace sad::os::kernel::interrupts {

// === ثوابت المقاطعات ===
static constexpr size_t IDT_ENTRIES       = 256;   // عدد مدخلات جدول المقاطعات
static constexpr size_t IRQ_BASE          = 32;    // بداية مقاطعات الأجهزة
static constexpr size_t TIMER_IRQ         = 0;     // مقاطعة المؤقت
static constexpr size_t KEYBOARD_IRQ      = 1;     // مقاطعة لوحة المفاتيح
static constexpr size_t SYSCALL_VECTOR    = 0x80;  // متجه استدعاء النظام

// === نوع بوابة المقاطعة ===
enum class GateType : uint8_t {
    Interrupt = 0x8E,  // بوابة مقاطعة (تعطل المقاطعات)
    Trap      = 0x8F,  // بوابة مصيدة (لا تعطل المقاطعات)
    Task      = 0x85   // بوابة مهمة
};

// === مدخل في جدول واصفات المقاطعات (IDT) - x86_64 ===
struct IDTEntry {
    uint16_t offset_low;       // البتات 0-15 من عنوان المعالج
    uint16_t selector;         // محدد مقطع الكود
    uint8_t  ist;              // جدول مكدس المقاطعة (0-7)
    uint8_t  type_attr;        // نوع البوابة والصفات
    uint16_t offset_mid;       // البتات 16-31
    uint32_t offset_high;      // البتات 32-63
    uint32_t reserved;         // محجوز (يجب أن يكون صفراً)

    // تعيين معالج المقاطعة
    void set_handler(uint64_t handler_addr, uint16_t code_selector, GateType type) {
        offset_low  = handler_addr & 0xFFFF;
        offset_mid  = (handler_addr >> 16) & 0xFFFF;
        offset_high = (handler_addr >> 32) & 0xFFFFFFFF;
        selector    = code_selector;
        ist         = 0;
        type_attr   = static_cast<uint8_t>(type);
        reserved    = 0;
    }
} __attribute__((packed));

// === مؤشر جدول المقاطعات ===
struct IDTPointer {
    uint16_t limit;    // حجم الجدول - 1
    uint64_t base;     // عنوان قاعدة الجدول
} __attribute__((packed));

// === إطار المقاطعة (يُدفع بواسطة المعالج) ===
struct InterruptFrame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t vector;           // رقم المقاطعة
    uint64_t error_code;       // رمز الخطأ (إن وجد)
    uint64_t rip;              // مؤشر التعليمة
    uint64_t cs;               // مقطع الكود
    uint64_t rflags;           // الأعلام
    uint64_t rsp;              // مؤشر المكدس
    uint64_t ss;               // مقطع المكدس
};

// === نوع دالة معالج المقاطعة ===
using ISRHandler = void(*)(InterruptFrame*);

// === إحصائيات المقاطعات ===
struct InterruptStats {
    uint64_t count;            // عدد مرات الحدوث
    uint64_t last_timestamp;   // آخر وقت حدوث
};

// === معالج المقاطعات الرئيسي ===
class InterruptHandler {
public:
    // تهيئة نظام المقاطعات
    bool initialize() {
        // مسح جدول المقاطعات
        for (auto& entry : idt_) {
            entry = {};
        }
        for (auto& handler : handlers_) {
            handler = nullptr;
        }
        for (auto& stat : stats_) {
            stat = {0, 0};
        }

        // تسجيل معالجات الاستثناءات (0-31)
        setup_exception_handlers();

        // تسجيل معالجات المقاطعات العامة (IRQ)
        setup_irq_handlers();

        // تسجيل معالج استدعاء النظام
        set_gate(SYSCALL_VECTOR, default_syscall_handler, GateType::Trap);

        // تحميل جدول المقاطعات
        load_idt();

        interrupts_enabled_ = true;
        return true;
    }

    // تسجيل معالج مقاطعة مخصص
    bool register_handler(uint8_t vector, ISRHandler handler) {
        if (vector >= IDT_ENTRIES || !handler) return false;
        handlers_[vector] = handler;
        return true;
    }

    // إلغاء تسجيل معالج
    void unregister_handler(uint8_t vector) {
        if (vector < IDT_ENTRIES) {
            handlers_[vector] = nullptr;
        }
    }

    // نقطة الدخول العامة لجميع المقاطعات
    void dispatch(InterruptFrame* frame) {
        uint8_t vector = static_cast<uint8_t>(frame->vector);

        // تحديث الإحصائيات
        stats_[vector].count++;
        stats_[vector].last_timestamp = read_timestamp();

        // استدعاء المعالج المسجل
        if (handlers_[vector]) {
            handlers_[vector](frame);
        } else {
            handle_unregistered(vector, frame);
        }

        // إرسال إشارة نهاية المقاطعة للأجهزة
        if (vector >= IRQ_BASE && vector < IRQ_BASE + 16) {
            send_eoi(vector - IRQ_BASE);
        }
    }

    // تفعيل/تعطيل المقاطعات
    void enable()  { interrupts_enabled_ = true;  /* sti */ }
    void disable() { interrupts_enabled_ = false; /* cli */ }
    bool is_enabled() const { return interrupts_enabled_; }

    // الحصول على إحصائيات مقاطعة
    InterruptStats get_stats(uint8_t vector) const {
        return (vector < IDT_ENTRIES) ? stats_[vector] : InterruptStats{0, 0};
    }

private:
    alignas(16) std::array<IDTEntry, IDT_ENTRIES> idt_;
    std::array<ISRHandler, IDT_ENTRIES> handlers_;
    std::array<InterruptStats, IDT_ENTRIES> stats_;
    IDTPointer idtr_;
    bool interrupts_enabled_ = false;

    // إعداد بوابة في جدول المقاطعات
    void set_gate(uint8_t vector, ISRHandler handler, GateType type) {
        uint64_t addr = reinterpret_cast<uint64_t>(handler);
        idt_[vector].set_handler(addr, 0x08, type); // 0x08 = محدد مقطع كود النواة
        handlers_[vector] = handler;
    }

    // تحميل جدول المقاطعات في المعالج
    void load_idt() {
        idtr_.limit = sizeof(idt_) - 1;
        idtr_.base = reinterpret_cast<uint64_t>(idt_.data());
        // __asm__ volatile("lidt %0" : : "m"(idtr_));
    }

    // إعداد معالجات الاستثناءات (قسمة على صفر، خطأ صفحة، إلخ)
    void setup_exception_handlers() {
        for (uint8_t i = 0; i < 32; i++) {
            set_gate(i, default_exception_handler, GateType::Trap);
        }
    }

    // إعداد معالجات مقاطعات الأجهزة
    void setup_irq_handlers() {
        // مقاطعة المؤقت (IRQ0)
        set_gate(IRQ_BASE + TIMER_IRQ, timer_handler, GateType::Interrupt);

        // مقاطعة لوحة المفاتيح (IRQ1)
        set_gate(IRQ_BASE + KEYBOARD_IRQ, keyboard_handler, GateType::Interrupt);

        // باقي مقاطعات الأجهزة
        for (uint8_t i = 2; i < 16; i++) {
            set_gate(IRQ_BASE + i, default_irq_handler, GateType::Interrupt);
        }
    }

    // إرسال إشارة نهاية المقاطعة (EOI)
    void send_eoi(uint8_t irq) {
        if (irq >= 8) {
            outb(0xA0, 0x20); // EOI للمتحكم الثانوي
        }
        outb(0x20, 0x20);     // EOI للمتحكم الرئيسي
    }

    uint64_t read_timestamp() { return 0; /* rdtsc */ }
    void outb(uint16_t, uint8_t) { /* كتابة لمنفذ إدخال/إخراج */ }

    // === المعالجات الافتراضية ===
    static void default_exception_handler(InterruptFrame* f) { /* معالج الاستثناء */ }
    static void default_irq_handler(InterruptFrame* f)       { /* معالج IRQ افتراضي */ }
    static void default_syscall_handler(InterruptFrame* f)    { /* معالج استدعاء النظام */ }

    // معالج المؤقت - يُستدعى كل نبضة مؤقت
    static void timer_handler(InterruptFrame* f) {
        // تحديث عداد النبضات
        // إعلام المجدول لاحتمال تبديل المهمة
    }

    // معالج لوحة المفاتيح
    static void keyboard_handler(InterruptFrame* f) {
        // قراءة رمز المفتاح من المنفذ 0x60
        // إضافته إلى مخزن لوحة المفاتيح المؤقت
    }

    void handle_unregistered(uint8_t vector, InterruptFrame*) {
        // مقاطعة بدون معالج مسجل - تسجيل تحذير
    }
};

} // namespace sad::os::kernel::interrupts
