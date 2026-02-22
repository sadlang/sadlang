// ===================================================================
// مدير الذاكرة الافتراضية - صدOS
// vmm.cpp - Virtual Memory Manager
// إدارة جداول الصفحات رباعية المستويات مع دعم النسخ عند الكتابة
// المرحلة 52 - مشروع لغة صاد
// ===================================================================

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace sad::os::kernel::memory {

// === ثوابت الذاكرة الافتراضية ===
static constexpr uint64_t PAGE_SIZE        = 4096;          // حجم الصفحة: 4 كيلوبايت
static constexpr uint64_t PAGE_TABLE_ENTRIES = 512;         // عدد المدخلات في كل مستوى
static constexpr uint64_t KERNEL_BASE      = 0xFFFF800000000000ULL; // قاعدة فضاء النواة

// === أعلام الصفحة ===
enum PageFlags : uint64_t {
    Present       = 1ULL << 0,    // الصفحة موجودة
    Writable      = 1ULL << 1,    // قابلة للكتابة
    UserAccess    = 1ULL << 2,    // قابلة للوصول من فضاء المستخدم
    WriteThrough  = 1ULL << 3,    // كتابة مباشرة للذاكرة المخبئية
    CacheDisable  = 1ULL << 4,    // تعطيل الذاكرة المخبئية
    Accessed      = 1ULL << 5,    // تم الوصول إليها
    Dirty         = 1ULL << 6,    // تم تعديلها
    HugePage      = 1ULL << 7,    // صفحة كبيرة (2 ميغا)
    Global        = 1ULL << 8,    // صفحة عامة
    CopyOnWrite   = 1ULL << 9,    // نسخ عند الكتابة (بت مخصص)
    NoExecute     = 1ULL << 63    // غير قابلة للتنفيذ
};

// === مدخل جدول الصفحات ===
struct PageTableEntry {
    uint64_t value = 0;

    bool is_present() const     { return value & Present; }
    bool is_writable() const    { return value & Writable; }
    bool is_cow() const         { return value & CopyOnWrite; }
    uint64_t address() const    { return value & 0x000FFFFFFFFFF000ULL; }

    void set(uint64_t addr, uint64_t flags) {
        value = (addr & 0x000FFFFFFFFFF000ULL) | flags;
    }
    void clear() { value = 0; }
};

// === جدول الصفحات (مستوى واحد) ===
struct alignas(PAGE_SIZE) PageTable {
    PageTableEntry entries[PAGE_TABLE_ENTRIES];
};

// === نوع خطأ الصفحة ===
enum class PageFaultType : uint8_t {
    NotPresent,        // الصفحة غير موجودة
    ProtectionViolation, // انتهاك الحماية
    CopyOnWrite,       // نسخ عند الكتابة
    StackOverflow      // تجاوز المكدس
};

// === مدير الذاكرة الافتراضية ===
class VirtualMemoryManager {
public:
    // تهيئة مدير الذاكرة الافتراضية
    bool initialize() {
        pml4_ = allocate_page_table();
        if (!pml4_) return false;
        aslr_seed_ = 0xDEADBEEF; // بذرة عشوائية مبدئية
        mapped_pages_ = 0;
        return true;
    }

    // تعيين صفحة افتراضية إلى فيزيائية (4 مستويات)
    bool map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
        if (!pml4_) return false;

        uint64_t pml4_idx = (virtual_addr >> 39) & 0x1FF;   // المستوى 4
        uint64_t pdpt_idx = (virtual_addr >> 30) & 0x1FF;   // المستوى 3
        uint64_t pd_idx   = (virtual_addr >> 21) & 0x1FF;   // المستوى 2
        uint64_t pt_idx   = (virtual_addr >> 12) & 0x1FF;   // المستوى 1

        // التنقل عبر المستويات مع إنشاء جداول عند الحاجة
        auto* pdpt = ensure_table(pml4_, pml4_idx);
        if (!pdpt) return false;
        auto* pd = ensure_table(pdpt, pdpt_idx);
        if (!pd) return false;
        auto* pt = ensure_table(pd, pd_idx);
        if (!pt) return false;

        pt->entries[pt_idx].set(physical_addr, flags | Present);
        mapped_pages_++;
        return true;
    }

    // إلغاء تعيين صفحة
    void unmap_page(uint64_t virtual_addr) {
        auto* entry = walk_page_tables(virtual_addr);
        if (entry && entry->is_present()) {
            entry->clear();
            invalidate_tlb(virtual_addr);
            if (mapped_pages_ > 0) mapped_pages_--;
        }
    }

    // معالج خطأ الصفحة
    bool handle_page_fault(uint64_t fault_addr, uint64_t error_code) {
        auto type = classify_fault(fault_addr, error_code);

        switch (type) {
            case PageFaultType::CopyOnWrite:
                return handle_cow_fault(fault_addr);
            case PageFaultType::NotPresent:
                return handle_demand_paging(fault_addr);
            case PageFaultType::StackOverflow:
                return expand_stack(fault_addr);
            case PageFaultType::ProtectionViolation:
                return false; // إنهاء العملية
        }
        return false;
    }

    // تعيين ذاكرة مع دعم ASLR
    uint64_t mmap_with_aslr(uint64_t size, uint64_t flags) {
        uint64_t base = generate_aslr_address(size);
        uint64_t pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

        for (uint64_t i = 0; i < pages; i++) {
            uint64_t vaddr = base + i * PAGE_SIZE;
            uint64_t paddr = allocate_physical_page();
            if (!map_page(vaddr, paddr, flags)) return 0;
        }
        return base;
    }

    // تفعيل النسخ عند الكتابة لنطاق ذاكرة
    bool enable_cow(uint64_t start, uint64_t size) {
        uint64_t pages = size / PAGE_SIZE;
        for (uint64_t i = 0; i < pages; i++) {
            auto* entry = walk_page_tables(start + i * PAGE_SIZE);
            if (entry && entry->is_present()) {
                entry->value &= ~Writable;       // إزالة إذن الكتابة
                entry->value |= CopyOnWrite;      // تعليم كنسخ عند الكتابة
            }
        }
        return true;
    }

    uint64_t total_mapped_pages() const { return mapped_pages_; }

private:
    PageTable* pml4_ = nullptr;
    uint64_t aslr_seed_;
    uint64_t mapped_pages_ = 0;

    PageTable* allocate_page_table()                     { return new PageTable{}; }
    uint64_t allocate_physical_page()                    { return 0; /* من مدير الإطارات */ }
    void invalidate_tlb(uint64_t)                        { /* تعليمة invlpg */ }

    PageTable* ensure_table(PageTable* parent, uint64_t index) {
        if (!parent->entries[index].is_present()) {
            auto* table = allocate_page_table();
            if (!table) return nullptr;
            parent->entries[index].set(reinterpret_cast<uint64_t>(table),
                                       Present | Writable | UserAccess);
        }
        return reinterpret_cast<PageTable*>(parent->entries[index].address());
    }

    PageTableEntry* walk_page_tables(uint64_t vaddr) {
        if (!pml4_) return nullptr;
        auto& pml4e = pml4_->entries[(vaddr >> 39) & 0x1FF];
        if (!pml4e.is_present()) return nullptr;
        auto* pdpt = reinterpret_cast<PageTable*>(pml4e.address());
        auto& pdpte = pdpt->entries[(vaddr >> 30) & 0x1FF];
        if (!pdpte.is_present()) return nullptr;
        auto* pd = reinterpret_cast<PageTable*>(pdpte.address());
        auto& pde = pd->entries[(vaddr >> 21) & 0x1FF];
        if (!pde.is_present()) return nullptr;
        auto* pt = reinterpret_cast<PageTable*>(pde.address());
        return &pt->entries[(vaddr >> 12) & 0x1FF];
    }

    PageFaultType classify_fault(uint64_t addr, uint64_t error_code) {
        if (error_code & 0x1) {
            auto* e = walk_page_tables(addr);
            if (e && e->is_cow()) return PageFaultType::CopyOnWrite;
            return PageFaultType::ProtectionViolation;
        }
        return PageFaultType::NotPresent;
    }

    bool handle_cow_fault(uint64_t addr) {
        auto* entry = walk_page_tables(addr);
        if (!entry) return false;
        uint64_t new_page = allocate_physical_page();
        // نسخ محتويات الصفحة القديمة إلى الجديدة
        entry->set(new_page, Present | Writable | UserAccess);
        invalidate_tlb(addr);
        return true;
    }

    bool handle_demand_paging(uint64_t addr) {
        uint64_t page = allocate_physical_page();
        return map_page(addr & ~(PAGE_SIZE - 1), page, Present | Writable | UserAccess);
    }

    bool expand_stack(uint64_t) { return true; }

    uint64_t generate_aslr_address(uint64_t size) {
        aslr_seed_ = aslr_seed_ * 6364136223846793005ULL + 1442695040888963407ULL;
        uint64_t offset = (aslr_seed_ >> 16) & 0xFFFFF000ULL;
        return 0x00007F0000000000ULL + offset;
    }
};

} // namespace sad::os::kernel::memory
