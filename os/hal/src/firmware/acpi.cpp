// ===================================================================
// صدOS - تحليل جداول ACPI وشجرة الأجهزة (DeviceTree)
// المرحلة 52: دعم البرامج الثابتة (Firmware)
// ===================================================================

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace sad::os::hal {

// === هياكل جداول ACPI ===

/// ترويسة جدول ACPI القياسية
struct ACPITableHeader {
    char     signature[4];    // توقيع الجدول (4 أحرف)
    uint32_t length;          // طول الجدول الكامل بالبايت
    uint8_t  revision;        // رقم المراجعة
    uint8_t  checksum;        // المجموع التحققي
    char     oem_id[6];       // معرّف الشركة المصنّعة
    char     oem_table_id[8]; // معرّف جدول الشركة
    uint32_t oem_revision;    // مراجعة الشركة
    uint32_t creator_id;      // معرّف المُنشئ
    uint32_t creator_revision;// مراجعة المُنشئ
} __attribute__((packed));

/// مؤشر وصف جذر النظام (RSDP)
struct RSDP {
    char     signature[8];    // "RSD PTR " - توقيع RSDP
    uint8_t  checksum;        // المجموع التحققي
    char     oem_id[6];       // معرّف الشركة المصنّعة
    uint8_t  revision;        // 0 = ACPI 1.0, 2 = ACPI 2.0+
    uint32_t rsdt_address;    // عنوان جدول RSDT (32-بت)
    uint32_t length;          // طول الهيكل (للنسخة 2.0+)
    uint64_t xsdt_address;    // عنوان جدول XSDT (64-بت)
    uint8_t  ext_checksum;    // المجموع التحققي الموسع
    uint8_t  reserved[3];     // محجوز
} __attribute__((packed));

/// مدخل معالج محلي في جدول MADT
struct MADTLocalAPIC {
    uint8_t  type;            // نوع المدخل (0 = معالج محلي)
    uint8_t  length;          // طول المدخل
    uint8_t  processor_id;    // معرّف المعالج
    uint8_t  apic_id;         // معرّف APIC
    uint32_t flags;           // أعلام (بت 0 = مفعّل)
} __attribute__((packed));

/// عقدة في شجرة الأجهزة (DeviceTree)
struct DTNode {
    std::string name;                     // اسم العقدة
    std::vector<std::pair<std::string, std::vector<uint8_t>>> properties; // الخصائص
    std::vector<DTNode> children;         // العقد الفرعية
};

// === محلل ACPI ===

/// محلل جداول ACPI - يقرأ ويفسر جداول البرامج الثابتة
class ACPIParser {
public:
    /// تهيئة المحلل بالبحث عن RSDP في الذاكرة
    bool initialize() {
        rsdp_ = find_rsdp();
        if (!rsdp_) return false;
        parse_rsdt();   // تحليل جدول الجذر
        parse_madt();   // تحليل جدول وصف المقاطعات
        return true;
    }

    /// البحث عن جدول ACPI بتوقيعه
    const ACPITableHeader* find_table(const char* signature) const {
        for (auto* tbl : tables_) {
            if (std::memcmp(tbl->signature, signature, 4) == 0)
                return tbl;
        }
        return nullptr;
    }

    /// الحصول على عدد المعالجات المكتشفة
    size_t cpu_count() const { return local_apics_.size(); }

    /// الحصول على قائمة معالجات APIC المحلية
    const std::vector<MADTLocalAPIC>& local_apics() const { return local_apics_; }

private:
    const RSDP* rsdp_ = nullptr;
    std::vector<const ACPITableHeader*> tables_;
    std::vector<MADTLocalAPIC> local_apics_;

    /// البحث عن مؤشر RSDP في نطاقات الذاكرة المعروفة
    const RSDP* find_rsdp() {
        // البحث في EBDA (0x80000-0x9FFFF) والمنطقة 0xE0000-0xFFFFF
        return nullptr; // يعتمد على الذاكرة الفعلية
    }

    /// تحليل جدول وصف جذر النظام (RSDT/XSDT)
    void parse_rsdt() {
        if (!rsdp_) return;
        // قراءة مؤشرات الجداول من RSDT أو XSDT
    }

    /// تحليل جدول وصف المقاطعات المتعدد (MADT)
    void parse_madt() {
        auto* madt = find_table("APIC");
        if (!madt) return;
        // مسح مدخلات MADT واستخراج معالجات APIC
    }
};

// === محلل شجرة الأجهزة ===

/// محلل DeviceTree (FDT) - للمعماريات غير x86
class DeviceTreeParser {
public:
    /// تحليل شجرة الأجهزة المسطحة من عنوان في الذاكرة
    bool parse(const uint8_t* fdt_data, size_t size) {
        if (!fdt_data || size < 40) return false;
        // التحقق من الرقم السحري 0xD00DFEED
        uint32_t magic = read_be32(fdt_data);
        if (magic != 0xD00DFEED) return false;
        parse_nodes(fdt_data, size);
        return true;
    }

    /// البحث عن عقدة بمسارها
    const DTNode* find_node(const std::string& path) const {
        // بحث متعمق في الشجرة
        return find_in(root_, path);
    }

    const DTNode& root() const { return root_; }

private:
    DTNode root_{"/"};

    /// قراءة عدد صحيح 32-بت بتشفير Big-Endian
    uint32_t read_be32(const uint8_t* p) const {
        return (uint32_t(p[0]) << 24) | (uint32_t(p[1]) << 16) |
               (uint32_t(p[2]) << 8)  | uint32_t(p[3]);
    }

    /// تحليل عقد الشجرة من البيانات الخام
    void parse_nodes(const uint8_t* data, size_t size) {
        (void)data; (void)size;
        // تحليل هيكل FDT المسطح إلى شجرة عقد
    }

    /// بحث عن عقدة داخل شجرة فرعية
    const DTNode* find_in(const DTNode& node, const std::string& path) const {
        if (node.name == path) return &node;
        for (auto& child : node.children) {
            auto* found = find_in(child, path);
            if (found) return found;
        }
        return nullptr;
    }
};

} // namespace sad::os::hal
