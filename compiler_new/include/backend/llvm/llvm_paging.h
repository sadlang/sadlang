/**
 * =============================================================================
 * ملف: llvm_paging.h
 * الوصف: توليد كود جداول الصفحات وتعليمات إدارة الذاكرة الافتراضية
 * المهمة: م-د07 - إضافة دعم جداول الصفحات (Paging Support)
 * =============================================================================
 *
 * 4-Level Paging (x86_64):
 *   PML4 → PDPT → PD → PT → Physical Page (4KB)
 *
 * بنية مُدخل الصفحة (64-بت):
 *   بت 0:  Present (P)
 *   بت 1:  Read/Write (RW)
 *   بت 2:  User/Supervisor (US)
 *   بت 3:  Page-Level Write-Through (PWT)
 *   بت 4:  Page-Level Cache Disable (PCD)
 *   بت 5:  Accessed (A)
 *   بت 6:  Dirty (D)
 *   بت 7:  Page Size / PAT (PS)
 *   بت 8:  Global (G)
 *   بت 12-51: Physical Address
 *   بت 63: No Execute (NX)
 * =============================================================================
 */

#ifndef SAD_LLVM_PAGING_H
#define SAD_LLVM_PAGING_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>

namespace sad {
namespace codegen {

/**
 * أعلام مُدخل الصفحة
 */
namespace PageFlags {
    constexpr uint64_t PRESENT       = 1ULL << 0;
    constexpr uint64_t WRITABLE      = 1ULL << 1;
    constexpr uint64_t USER          = 1ULL << 2;
    constexpr uint64_t WRITE_THROUGH = 1ULL << 3;
    constexpr uint64_t CACHE_DISABLE = 1ULL << 4;
    constexpr uint64_t ACCESSED      = 1ULL << 5;
    constexpr uint64_t DIRTY         = 1ULL << 6;
    constexpr uint64_t HUGE_PAGE     = 1ULL << 7;
    constexpr uint64_t GLOBAL        = 1ULL << 8;
    constexpr uint64_t NO_EXECUTE    = 1ULL << 63;
    
    // مجموعات مُعرّفة مسبقاً
    constexpr uint64_t KERNEL_CODE   = PRESENT;
    constexpr uint64_t KERNEL_DATA   = PRESENT | WRITABLE | NO_EXECUTE;
    constexpr uint64_t KERNEL_RODATA = PRESENT | NO_EXECUTE;
    constexpr uint64_t USER_CODE     = PRESENT | USER;
    constexpr uint64_t USER_DATA     = PRESENT | WRITABLE | USER | NO_EXECUTE;
    constexpr uint64_t MMIO          = PRESENT | WRITABLE | CACHE_DISABLE
                                       | NO_EXECUTE;
}

/**
 * مستويات جداول الصفحات
 */
enum class PageLevel : unsigned {
    PML4  = 4,  // Page Map Level 4
    PDPT  = 3,  // Page Directory Pointer Table
    PD    = 2,  // Page Directory
    PT    = 1,  // Page Table
};

/**
 * مولّد كود جداول الصفحات عبر LLVM IRBuilder
 */
class LLVMPaging {
public:
    explicit LLVMPaging(llvm::Module& module);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    أنواع جداول الصفحات
    // ═══════════════════════════════════════════════════════════════════════
    
    /** نوع مُدخل الصفحة: i64 */
    llvm::Type* getPageEntryType();
    
    /** نوع جدول الصفحات: [512 x i64] (4KB) */
    llvm::ArrayType* getPageTableType();
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    إنشاء جداول ثابتة
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * إنشاء جدول PML4 كمتغير عام في قسم .page_tables
     */
    llvm::GlobalVariable* createPML4Table(const std::string& name = "pml4");
    
    /**
     * إنشاء جدول صفحات فارغ (512 مُدخل صفري)
     */
    llvm::GlobalVariable* createEmptyPageTable(const std::string& name);
    
    /**
     * إنشاء identity mapping أساسي (أول 2MB أو 1GB)
     * - يربط العناوين الافتراضية بنفس العناوين الفيزيائية
     * - ضروري عند تفعيل الترحيل لأول مرة
     */
    void createIdentityMapping(llvm::GlobalVariable* pml4,
                                uint64_t sizeInBytes = 2 * 1024 * 1024);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    عمليات ديناميكية (وقت التشغيل)
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * توليد كود لتعيين مُدخل في جدول الصفحات
     * table[index] = physAddr | flags
     */
    void emitSetPageEntry(llvm::IRBuilder<>& builder,
                           llvm::Value* tablePtr,
                           llvm::Value* index,
                           llvm::Value* physAddr,
                           uint64_t flags);
    
    /**
     * توليد كود لقراءة مُدخل من جدول الصفحات
     */
    llvm::Value* emitGetPageEntry(llvm::IRBuilder<>& builder,
                                   llvm::Value* tablePtr,
                                   llvm::Value* index);
    
    /**
     * توليد كود لاستخراج العنوان الفيزيائي من المُدخل
     * (entry & 0x000FFFFFFFFFF000)
     */
    llvm::Value* emitExtractPhysAddr(llvm::IRBuilder<>& builder,
                                      llvm::Value* entry);
    
    /**
     * توليد كود لفحص علم معين في المُدخل
     * (entry & flagBit) != 0
     */
    llvm::Value* emitCheckFlag(llvm::IRBuilder<>& builder,
                                llvm::Value* entry,
                                uint64_t flag);
    
    // ═══════════════════════════════════════════════════════════════════════
    //                    تعليمات TLB وتحميل CR3
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * إبطال صفحة واحدة في TLB
     * invlpg [address]
     */
    void emitInvalidatePage(llvm::IRBuilder<>& builder, llvm::Value* virtAddr);
    
    /**
     * إبطال كامل TLB (بإعادة تحميل CR3)
     * mov rax, cr3; mov cr3, rax
     */
    void emitFlushTLB(llvm::IRBuilder<>& builder);
    
    /**
     * تحميل جدول صفحات جديد في CR3
     * mov cr3, physAddr
     */
    void emitLoadPageTable(llvm::IRBuilder<>& builder, llvm::Value* pml4PhysAddr);
    
    /**
     * تفعيل الترحيل (ضبط CR0.PG)
     */
    void emitEnablePaging(llvm::IRBuilder<>& builder);

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    
    llvm::ArrayType* m_pageTableType = nullptr;
    
    static constexpr uint64_t PAGE_SIZE = 4096;
    static constexpr unsigned ENTRIES_PER_TABLE = 512;
    static constexpr uint64_t ADDR_MASK = 0x000FFFFFFFFFF000ULL;
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_PAGING_H
