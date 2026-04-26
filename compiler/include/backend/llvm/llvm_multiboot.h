/**
 * =============================================================================
 * ملف: llvm_multiboot.h
 * الوصف: توليد ترويسة Multiboot2 كبيانات ثابتة عبر LLVM IRBuilder
 * المهمة: م-د05 - إضافة دعم ترويسة Multiboot2 / UEFI
 * =============================================================================
 *
 * ترويسة Multiboot2:
 *   - Magic: 0xE85250D6
 *   - Architecture: 0 (i386/protected mode)
 *   - Header length
 *   - Checksum: -(magic + arch + length)
 *   - Tags (اختياري): framebuffer, memory map, etc.
 * =============================================================================
 */

#ifndef SAD_LLVM_MULTIBOOT_H
#define SAD_LLVM_MULTIBOOT_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>
#include <vector>

namespace sad {
namespace codegen {

/**
 * أنواع علامات Multiboot2
 */
enum class Multiboot2TagType : uint16_t {
    END              = 0,
    INFORMATION_REQ  = 1,
    ADDRESS          = 2,
    ENTRY_ADDRESS    = 3,
    FLAGS            = 4,
    FRAMEBUFFER      = 5,
    MODULE_ALIGN     = 6,
    EFI_BS           = 7,
    ENTRY_ADDRESS_EFI32 = 8,
    ENTRY_ADDRESS_EFI64 = 9,
    RELOCATABLE      = 10,
};

/**
 * إعدادات Multiboot2
 */
struct Multiboot2Config {
    bool requestFramebuffer = false;
    uint32_t fbWidth = 1024;
    uint32_t fbHeight = 768;
    uint32_t fbDepth = 32;
    bool requestMemoryMap = true;
    bool moduleAlign = false;
};

/**
 * مولّد ترويسة Multiboot2
 */
class LLVMMultiboot {
public:
    explicit LLVMMultiboot(llvm::Module& module);
    
    /**
     * توليد ترويسة Multiboot2 كاملة في قسم .multiboot
     * #[إقلاع(multiboot2)]
     */
    llvm::GlobalVariable* generateHeader(const Multiboot2Config& config = {});
    
    /**
     * توليد علامة framebuffer
     */
    std::vector<uint32_t> buildFramebufferTag(uint32_t width,
                                               uint32_t height,
                                               uint32_t depth) const;
    
    /**
     * توليد علامة طلب معلومات
     */
    std::vector<uint32_t> buildInfoRequestTag(
        const std::vector<uint32_t>& requestedTags) const;

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    
    static constexpr uint32_t MULTIBOOT2_MAGIC    = 0xE85250D6;
    static constexpr uint32_t MULTIBOOT2_ARCH_X86 = 0;
    
    // حساب المجموع التحققي
    static uint32_t calculateChecksum(uint32_t magic, uint32_t arch,
                                       uint32_t length);
};

/**
 * مولّد نقطة دخول UEFI
 */
class LLVMUefiEntry {
public:
    explicit LLVMUefiEntry(llvm::Module& module);
    
    /**
     * توليد efi_main(ImageHandle, SystemTable) -> EFI_STATUS
     * #[إقلاع(uefi)]
     */
    llvm::Function* generateEfiMain();
    
    /**
     * توليد أنواع UEFI الأساسية
     */
    llvm::StructType* getEfiSystemTableType();
    llvm::StructType* getEfiSimpleTextOutputType();

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    
    llvm::StructType* m_systemTableType = nullptr;
    llvm::StructType* m_textOutputType = nullptr;
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_MULTIBOOT_H
