/**
 * =============================================================================
 * ملف: llvm_multiboot.cpp
 * الوصف: تنفيذ توليد ترويسة Multiboot2 عبر LLVM IRBuilder
 * المهمة: م-د05
 * =============================================================================
 */

#include "backend/llvm/llvm_multiboot.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <numeric>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════════
//                    Multiboot2
// ═══════════════════════════════════════════════════════════════════════════════

LLVMMultiboot::LLVMMultiboot(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

uint32_t LLVMMultiboot::calculateChecksum(uint32_t magic, uint32_t arch,
                                            uint32_t length) {
    return -(magic + arch + length);
}

llvm::GlobalVariable* LLVMMultiboot::generateHeader(
    const Multiboot2Config& config) {
    
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* i16Ty = llvm::Type::getInt16Ty(m_ctx);
    
    // بناء البيانات الخام للترويسة
    std::vector<uint32_t> headerData;
    
    // ─── الترويسة الأساسية (16 بايت) ────────────────────────────────
    // سنُحدّث الطول والـ checksum لاحقاً
    headerData.push_back(MULTIBOOT2_MAGIC);        // magic
    headerData.push_back(MULTIBOOT2_ARCH_X86);     // architecture
    headerData.push_back(0);                        // header_length (مؤقت)
    headerData.push_back(0);                        // checksum (مؤقت)
    
    // ─── علامات اختيارية ──────────────────────────────────────────────
    
    // علامة framebuffer
    if (config.requestFramebuffer) {
        auto fbTag = buildFramebufferTag(config.fbWidth, config.fbHeight,
                                          config.fbDepth);
        headerData.insert(headerData.end(), fbTag.begin(), fbTag.end());
    }
    
    // علامة طلب معلومات (memory map)
    if (config.requestMemoryMap) {
        std::vector<uint32_t> requests = {6}; // MULTIBOOT_TAG_TYPE_MMAP
        auto infoTag = buildInfoRequestTag(requests);
        headerData.insert(headerData.end(), infoTag.begin(), infoTag.end());
    }
    
    // علامة module alignment
    if (config.moduleAlign) {
        // type=6, flags=0, size=8
        headerData.push_back((0 << 16) | static_cast<uint32_t>(
                              Multiboot2TagType::MODULE_ALIGN));
        headerData.push_back(8);
    }
    
    // ─── علامة النهاية (8 بايت) ──────────────────────────────────────
    headerData.push_back(0); // type=0, flags=0
    headerData.push_back(8); // size=8
    
    // تحديث الطول والـ checksum
    uint32_t headerLength = static_cast<uint32_t>(headerData.size() * 4);
    headerData[2] = headerLength;
    headerData[3] = calculateChecksum(MULTIBOOT2_MAGIC, MULTIBOOT2_ARCH_X86,
                                       headerLength);
    
    // ─── إنشاء المتغير العام ─────────────────────────────────────────
    auto* arrayTy = llvm::ArrayType::get(i32Ty, headerData.size());
    
    std::vector<llvm::Constant*> values;
    values.reserve(headerData.size());
    for (auto val : headerData) {
        values.push_back(llvm::ConstantInt::get(i32Ty, val));
    }
    
    auto* init = llvm::ConstantArray::get(arrayTy, values);
    
    auto* header = new llvm::GlobalVariable(
        m_module, arrayTy, true,
        llvm::GlobalVariable::ExternalLinkage,
        init, "multiboot2_header");
    
    header->setAlignment(llvm::Align(8));
    header->setSection(".multiboot");
    
    return header;
}

std::vector<uint32_t> LLVMMultiboot::buildFramebufferTag(
    uint32_t width, uint32_t height, uint32_t depth) const {
    
    std::vector<uint32_t> tag;
    // type=5, flags=0
    tag.push_back((0 << 16) | static_cast<uint32_t>(
                   Multiboot2TagType::FRAMEBUFFER));
    tag.push_back(20);     // size = 20 bytes
    tag.push_back(width);  // width
    tag.push_back(height); // height
    tag.push_back(depth);  // depth
    return tag;
}

std::vector<uint32_t> LLVMMultiboot::buildInfoRequestTag(
    const std::vector<uint32_t>& requestedTags) const {
    
    std::vector<uint32_t> tag;
    uint32_t size = 8 + static_cast<uint32_t>(requestedTags.size() * 4);
    
    // type=1, flags=0
    tag.push_back((0 << 16) | static_cast<uint32_t>(
                   Multiboot2TagType::INFORMATION_REQ));
    tag.push_back(size);
    
    for (auto t : requestedTags) {
        tag.push_back(t);
    }
    
    // محاذاة إلى 8 بايت
    while ((tag.size() * 4) % 8 != 0) {
        tag.push_back(0);
    }
    
    return tag;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    UEFI Entry
// ═══════════════════════════════════════════════════════════════════════════════

LLVMUefiEntry::LLVMUefiEntry(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

llvm::StructType* LLVMUefiEntry::getEfiSystemTableType() {
    if (m_systemTableType) return m_systemTableType;
    
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(m_ctx);
    
    // EFI_SYSTEM_TABLE (مبسّط):
    // { Hdr(i64), FirmwareVendor(ptr), FirmwareRevision(i64),
    //   ConIn(ptr), ConOut(ptr), StdErr(ptr), RuntimeServices(ptr),
    //   BootServices(ptr), ... }
    m_systemTableType = llvm::StructType::create(m_ctx, {
        i64Ty,  // Hdr.Signature
        ptrTy,  // FirmwareVendor
        i64Ty,  // FirmwareRevision
        ptrTy,  // ConsoleInHandle
        ptrTy,  // ConIn
        ptrTy,  // ConsoleOutHandle
        ptrTy,  // ConOut (EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL*)
        ptrTy,  // StandardErrorHandle
        ptrTy,  // StdErr
        ptrTy,  // RuntimeServices
        ptrTy,  // BootServices
    }, "EFI_SYSTEM_TABLE");
    
    return m_systemTableType;
}

llvm::Function* LLVMUefiEntry::generateEfiMain() {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(m_ctx);
    
    // EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
    // EFI_HANDLE = void*, EFI_STATUS = uint64_t
    auto* funcTy = llvm::FunctionType::get(i64Ty, {ptrTy, ptrTy}, false);
    
    auto* efiMain = llvm::Function::Create(
        funcTy, llvm::Function::ExternalLinkage,
        "efi_main", m_module);
    
    // تسمية المعاملات
    auto args = efiMain->arg_begin();
    args->setName("ImageHandle");
    ++args;
    args->setName("SystemTable");
    
    // سمات UEFI calling convention
    efiMain->setCallingConv(llvm::CallingConv::Win64);
    efiMain->setSection(".text.efi");
    
    // إنشاء الكتلة الأساسية — المستخدم سيملأها
    auto* entry = llvm::BasicBlock::Create(m_ctx, "entry", efiMain);
    llvm::IRBuilder<> builder(entry);
    
    // EFI_SUCCESS = 0
    builder.CreateRet(llvm::ConstantInt::get(i64Ty, 0));
    
    return efiMain;
}

} // namespace codegen
} // namespace sad
