// ======================================================================
// bee_format.h - صيغة الملف التنفيذي .نحلة / .bee Executable Format
// ======================================================================
// الوصف بالعربية:
//   صيغة ملف تنفيذي مخصصة لنظام النحلة بدل ELF/PE
//   تحمل: metadata عربية، قدرات، توقيع رقمي، ABI
//
// English Description:
//   Custom executable format for BeeOS instead of ELF/PE
//   Contains: Arabic metadata, capabilities, digital signature, ABI
// ======================================================================

#ifndef SAD_BEE_FORMAT_H
#define SAD_BEE_FORMAT_H

#include <string>
#include <vector>
#include <cstdint>
#include <array>

namespace Sad {
namespace BeeFormat {

// =========================================================================
// Magic number: "نحلة" in UTF-8 = D9 86 D8 AD D9 84 D8 A9
// (AR) الرقم السحري: "نحلة" بترميز UTF-8
// =========================================================================
constexpr std::array<uint8_t, 8> BEE_MAGIC = {
    0xD9, 0x86,  // ن
    0xD8, 0xAD,  // ح
    0xD9, 0x84,  // ل
    0xD8, 0xA9,  // ة
};

constexpr uint16_t BEE_VERSION_MAJOR = 1;
constexpr uint16_t BEE_VERSION_MINOR = 0;

/**
 * @brief Target architecture / بنية الهدف
 */
enum class TargetArch : uint16_t {
    X86_64  = 1,    ///< x86_64
    ARM64   = 2,    ///< ARM64/AArch64
    RISCV64 = 3,    ///< RISC-V 64-bit
    WASM    = 4     ///< WebAssembly
};

/**
 * @brief Executable type / نوع الملف التنفيذي
 */
enum class ExeType : uint16_t {
    Executable  = 1,    ///< تنفيذي / executable
    SharedLib   = 2,    ///< مكتبة_مشتركة / shared library
    StaticLib   = 3,    ///< مكتبة_ساكنة / static library
    KernelImage = 4,    ///< صورة_نواة / kernel image
    Driver      = 5,    ///< مشغّل / driver
    Module      = 6     ///< وحدة / loadable module
};

/**
 * @brief Section type / نوع القسم
 */
enum class SectionType : uint32_t {
    Code        = 0x01, ///< كود / code (.text)
    Data        = 0x02, ///< بيانات / data (.data)
    ReadOnly    = 0x03, ///< للقراءة / read-only (.rodata)
    BSS         = 0x04, ///< غير_مهيأ / uninitialized (.bss)
    Symbols     = 0x05, ///< رموز / symbol table
    StringTable = 0x06, ///< جدول_نصوص / string table
    Relocations = 0x07, ///< إزاحات / relocations
    Debug       = 0x08, ///< تنقيح / debug info
    Capabilities= 0x09, ///< قدرات / capability descriptors
    Signature   = 0x0A, ///< توقيع / digital signature
    Metadata    = 0x0B, ///< وصف / metadata (Arabic description)
    Init        = 0x0C, ///< تهيئة / initialization code
    Fini        = 0x0D  ///< إنهاء / finalization code
};

/**
 * @brief Bee file header (64 bytes) / رأس ملف النحلة
 * 
 * @example بنية الرأس:
 * | الحقل        | الحجم | الوصف                    |
 * |-------------|-------|--------------------------|
 * | magic       | 8     | "نحلة" UTF-8             |
 * | version     | 4     | الإصدار (major.minor)     |
 * | arch        | 2     | بنية المعالج              |
 * | type        | 2     | نوع الملف                 |
 * | entry_point | 8     | نقطة الدخول              |
 * | section_off | 8     | إزاحة جدول الأقسام       |
 * | section_cnt | 4     | عدد الأقسام              |
 * | flags       | 4     | أعلام                    |
 * | caps_off    | 8     | إزاحة القدرات            |
 * | sig_off     | 8     | إزاحة التوقيع            |
 * | reserved    | 8     | محجوز                    |
 * | المجموع     | 64    |                          |
 */
#pragma pack(push, 1)
struct BeeHeader {
    uint8_t magic[8];           ///< Magic: "نحلة" UTF-8
    uint16_t versionMajor;      ///< Major version / الإصدار الرئيسي
    uint16_t versionMinor;      ///< Minor version / الإصدار الفرعي
    uint16_t arch;              ///< Target arch / بنية الهدف
    uint16_t type;              ///< Executable type / نوع الملف
    uint64_t entryPoint;        ///< Entry point address / عنوان نقطة الدخول
    uint64_t sectionTableOffset;///< Section table offset / إزاحة جدول الأقسام
    uint32_t sectionCount;      ///< Section count / عدد الأقسام
    uint32_t flags;             ///< Flags / أعلام
    uint64_t capabilitiesOffset;///< Capabilities section offset / إزاحة القدرات
    uint64_t signatureOffset;   ///< Signature offset / إزاحة التوقيع
    uint8_t reserved[8];        ///< Reserved / محجوز
};
#pragma pack(pop)

static_assert(sizeof(BeeHeader) == 64, "BeeHeader must be 64 bytes");

/**
 * @brief Section header / رأس القسم
 */
#pragma pack(push, 1)
struct SectionHeader {
    uint32_t nameOffset;    ///< Name offset in string table / إزاحة الاسم
    uint32_t type;          ///< Section type / نوع القسم
    uint64_t virtualAddr;   ///< Virtual address / العنوان الافتراضي
    uint64_t fileOffset;    ///< File offset / الإزاحة في الملف
    uint64_t size;          ///< Section size / حجم القسم
    uint32_t alignment;     ///< Alignment / المحاذاة
    uint32_t flags;         ///< Section flags / أعلام القسم
};
#pragma pack(pop)

/**
 * @brief Section flags / أعلام القسم
 */
enum SectionFlags : uint32_t {
    SF_READ     = 0x01,     ///< قراءة / readable
    SF_WRITE    = 0x02,     ///< كتابة / writable
    SF_EXEC     = 0x04,     ///< تنفيذ / executable
    SF_ALLOC    = 0x08,     ///< تخصيص / allocate in memory
    SF_ARABIC   = 0x10      ///< عربي / contains Arabic text (RTL)
};

/**
 * @brief Bee file writer / كاتب ملف النحلة
 * 
 * (AR) ينشئ ملفات بصيغة .نحلة من كود LLVM IR المترجم
 * (EN) Creates .bee files from compiled LLVM IR code
 */
class BeeFileWriter {
public:
    BeeFileWriter(TargetArch arch, ExeType type)
        : arch_(arch), type_(type) {}
    
    /**
     * @brief Set entry point / تعيين نقطة الدخول
     */
    void setEntryPoint(uint64_t addr) { entryPoint_ = addr; }
    
    /**
     * @brief Add a section / إضافة قسم
     */
    void addSection(const std::string& name, SectionType type,
                    const std::vector<uint8_t>& data, uint32_t flags) {
        sections_.push_back({name, type, data, flags});
    }
    
    /**
     * @brief Write to file / الكتابة لملف
     * @return true on success / صحيح عند النجاح
     */
    bool writeToFile(const std::string& path) const;
    
    /**
     * @brief Get total file size estimate / تقدير حجم الملف
     */
    size_t estimateSize() const {
        size_t size = sizeof(BeeHeader);
        for (const auto& s : sections_) {
            size += sizeof(SectionHeader) + s.data.size();
        }
        return size;
    }
    
private:
    struct SectionInfo {
        std::string name;
        SectionType type;
        std::vector<uint8_t> data;
        uint32_t flags;
    };
    
    TargetArch arch_;
    ExeType type_;
    uint64_t entryPoint_ = 0;
    std::vector<SectionInfo> sections_;
};

/**
 * @brief Bee file reader / قارئ ملف النحلة
 * 
 * (AR) يقرأ ملفات .نحلة ويستخرج الأقسام والمعلومات
 * (EN) Reads .bee files and extracts sections and info
 */
class BeeFileReader {
public:
    BeeFileReader() = default;
    
    /**
     * @brief Read from file / القراءة من ملف
     */
    bool readFromFile(const std::string& path);
    
    /**
     * @brief Get header / الحصول على الرأس
     */
    const BeeHeader& getHeader() const { return header_; }
    
    /**
     * @brief Validate magic number / التحقق من الرقم السحري
     */
    bool isValidBeeFile() const {
        for (int i = 0; i < 8; i++) {
            if (header_.magic[i] != BEE_MAGIC[i]) return false;
        }
        return true;
    }
    
    /**
     * @brief Get Arabic description / الحصول على الوصف العربي
     */
    std::string getArabicDescription() const {
        std::string desc = "ملف نحلة:\n";
        desc += "  الإصدار: " + std::to_string(header_.versionMajor) + 
                "." + std::to_string(header_.versionMinor) + "\n";
        desc += "  البنية: ";
        switch (static_cast<TargetArch>(header_.arch)) {
            case TargetArch::X86_64: desc += "x86_64"; break;
            case TargetArch::ARM64: desc += "ARM64"; break;
            case TargetArch::RISCV64: desc += "RISC-V 64"; break;
            case TargetArch::WASM: desc += "WebAssembly"; break;
        }
        desc += "\n  الأقسام: " + std::to_string(header_.sectionCount);
        return desc;
    }
    
private:
    BeeHeader header_{};
};

} // namespace BeeFormat
} // namespace Sad

#endif // SAD_BEE_FORMAT_H
