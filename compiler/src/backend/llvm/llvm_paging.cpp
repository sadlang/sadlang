/**
 * =============================================================================
 * ملف: llvm_paging.cpp
 * الوصف: تنفيذ توليد كود جداول الصفحات عبر LLVM IRBuilder
 * المهمة: م-د07
 * =============================================================================
 */

#include "backend/llvm/llvm_paging.h"

#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace sad {
namespace codegen {

LLVMPaging::LLVMPaging(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

// ═══════════════════════════════════════════════════════════════════════════════
//                    أنواع جداول الصفحات
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Type* LLVMPaging::getPageEntryType() {
    return llvm::Type::getInt64Ty(m_ctx);
}

llvm::ArrayType* LLVMPaging::getPageTableType() {
    if (m_pageTableType) return m_pageTableType;
    
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    m_pageTableType = llvm::ArrayType::get(i64Ty, ENTRIES_PER_TABLE);
    return m_pageTableType;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    إنشاء جداول ثابتة
// ═══════════════════════════════════════════════════════════════════════════════

llvm::GlobalVariable* LLVMPaging::createPML4Table(const std::string& name) {
    auto* tableTy = getPageTableType();
    auto* zeroInit = llvm::ConstantAggregateZero::get(tableTy);
    
    auto* pml4 = new llvm::GlobalVariable(
        m_module, tableTy, false, // ليس ثابتاً — سنملأه
        llvm::GlobalVariable::ExternalLinkage,
        zeroInit, name);
    
    pml4->setAlignment(llvm::Align(PAGE_SIZE));
    pml4->setSection(".page_tables");
    
    return pml4;
}

llvm::GlobalVariable* LLVMPaging::createEmptyPageTable(const std::string& name) {
    auto* tableTy = getPageTableType();
    auto* zeroInit = llvm::ConstantAggregateZero::get(tableTy);
    
    auto* table = new llvm::GlobalVariable(
        m_module, tableTy, false,
        llvm::GlobalVariable::InternalLinkage,
        zeroInit, name);
    
    table->setAlignment(llvm::Align(PAGE_SIZE));
    table->setSection(".page_tables");
    
    return table;
}

void LLVMPaging::createIdentityMapping(llvm::GlobalVariable* pml4,
                                         uint64_t sizeInBytes) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* tableTy = getPageTableType();
    
    // إنشاء PDPT
    auto* pdpt = createEmptyPageTable("boot_pdpt");
    
    if (sizeInBytes <= 2 * 1024 * 1024) {
        // ─── 2MB mapping باستخدام huge page ──────────────────────────
        auto* pd = createEmptyPageTable("boot_pd");
        
        // PD[0] = 0x0 | PRESENT | WRITABLE | HUGE_PAGE (2MB page)
        uint64_t pdEntryVal = 0x0 | PageFlags::PRESENT | PageFlags::WRITABLE
                              | PageFlags::HUGE_PAGE;
        
        // إنشاء المُدخلات كقيم ثابتة
        std::vector<llvm::Constant*> pdEntries(ENTRIES_PER_TABLE,
            llvm::ConstantInt::get(i64Ty, 0));
        pdEntries[0] = llvm::ConstantInt::get(i64Ty, pdEntryVal);
        auto* pdInit = llvm::ConstantArray::get(tableTy, pdEntries);
        pd->setInitializer(pdInit);
        
        // PDPT[0] = &PD | PRESENT | WRITABLE
        // ملاحظة: العنوان يُحسب وقت الربط
        // نستخدم ptrtoint لتحويل مؤشر PD إلى عدد صحيح
        auto* pdAddr = llvm::ConstantExpr::getPtrToInt(pd, i64Ty);
        // getAdd يعمل لأن العنوان محاذى للصفحة (البتات السفلية = 0)
        auto* pdptEntryVal = llvm::ConstantExpr::getAdd(
            pdAddr,
            llvm::ConstantInt::get(i64Ty,
                PageFlags::PRESENT | PageFlags::WRITABLE));
        
        std::vector<llvm::Constant*> pdptEntries(ENTRIES_PER_TABLE,
            llvm::ConstantInt::get(i64Ty, 0));
        pdptEntries[0] = pdptEntryVal;
        auto* pdptInit = llvm::ConstantArray::get(tableTy, pdptEntries);
        pdpt->setInitializer(pdptInit);
    } else {
        // ─── 1GB mapping باستخدام huge page في PDPT ─────────────────
        uint64_t pdptEntryVal = 0x0 | PageFlags::PRESENT | PageFlags::WRITABLE
                                | PageFlags::HUGE_PAGE;
        
        uint64_t numGBPages = (sizeInBytes + (1ULL << 30) - 1) >> 30;
        if (numGBPages > ENTRIES_PER_TABLE)
            numGBPages = ENTRIES_PER_TABLE;
        
        std::vector<llvm::Constant*> pdptEntries(ENTRIES_PER_TABLE,
            llvm::ConstantInt::get(i64Ty, 0));
        for (uint64_t i = 0; i < numGBPages; i++) {
            pdptEntries[i] = llvm::ConstantInt::get(i64Ty,
                (i * (1ULL << 30)) | pdptEntryVal);
        }
        auto* pdptInit = llvm::ConstantArray::get(tableTy, pdptEntries);
        pdpt->setInitializer(pdptInit);
    }
    
    // PML4[0] = &PDPT | PRESENT | WRITABLE
    auto* pdptAddr = llvm::ConstantExpr::getPtrToInt(pdpt, i64Ty);
    // getAdd يعمل لأن العنوان محاذى للصفحة (البتات السفلية = 0)
    auto* pml4EntryVal = llvm::ConstantExpr::getAdd(
        pdptAddr,
        llvm::ConstantInt::get(i64Ty,
            PageFlags::PRESENT | PageFlags::WRITABLE));
    
    std::vector<llvm::Constant*> pml4Entries(ENTRIES_PER_TABLE,
        llvm::ConstantInt::get(i64Ty, 0));
    pml4Entries[0] = pml4EntryVal;
    auto* pml4Init = llvm::ConstantArray::get(tableTy, pml4Entries);
    pml4->setInitializer(pml4Init);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    عمليات ديناميكية
// ═══════════════════════════════════════════════════════════════════════════════

void LLVMPaging::emitSetPageEntry(llvm::IRBuilder<>& builder,
                                    llvm::Value* tablePtr,
                                    llvm::Value* index,
                                    llvm::Value* physAddr,
                                    uint64_t flags) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    
    // حساب عنوان المُدخل: &table[index]
    auto* entryPtr = builder.CreateInBoundsGEP(
        getPageTableType(), tablePtr,
        {llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_ctx), 0), index},
        "entry_ptr");
    
    // القيمة: physAddr | flags
    auto* flagsVal = llvm::ConstantInt::get(i64Ty, flags);
    auto* entryVal = builder.CreateOr(physAddr, flagsVal, "entry_val");
    
    builder.CreateStore(entryVal, entryPtr);
}

llvm::Value* LLVMPaging::emitGetPageEntry(llvm::IRBuilder<>& builder,
                                            llvm::Value* tablePtr,
                                            llvm::Value* index) {
    auto* entryPtr = builder.CreateInBoundsGEP(
        getPageTableType(), tablePtr,
        {llvm::ConstantInt::get(llvm::Type::getInt32Ty(m_ctx), 0), index},
        "entry_ptr");
    
    return builder.CreateLoad(getPageEntryType(), entryPtr, "entry");
}

llvm::Value* LLVMPaging::emitExtractPhysAddr(llvm::IRBuilder<>& builder,
                                               llvm::Value* entry) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* mask = llvm::ConstantInt::get(i64Ty, ADDR_MASK);
    return builder.CreateAnd(entry, mask, "phys_addr");
}

llvm::Value* LLVMPaging::emitCheckFlag(llvm::IRBuilder<>& builder,
                                         llvm::Value* entry,
                                         uint64_t flag) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* flagVal = llvm::ConstantInt::get(i64Ty, flag);
    auto* masked = builder.CreateAnd(entry, flagVal);
    auto* zero = llvm::ConstantInt::get(i64Ty, 0);
    return builder.CreateICmpNE(masked, zero, "has_flag");
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    TLB و CR3
// ═══════════════════════════════════════════════════════════════════════════════

void LLVMPaging::emitInvalidatePage(llvm::IRBuilder<>& builder,
                                      llvm::Value* virtAddr) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "invlpg ($0)", "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm, {virtAddr});
}

// 🔴 دَينٌ مُعلَنٌ (٢٠٢٦-٠٨-١٢): هذا الملفُّ كلُّه يحمل العطبَ الذي سُدَّ في
//    `builders/platform/lowlevel_ops.cpp` — عرضُ سجلٍّ مثبَّتٌ i64 لا يتبع ثالوثَ
//    الهدف (`getTargetGprType`)، و`invlpg` بلا `~{memory}`. ولم يُسَدَّ هنا عمدًا:
//    مقيسٌ أنّ `LLVMPaging` **بلا نداءٍ واحدٍ** من خارج ملفّه، فمساراتُه ميّتة،
//    وهو خارجَ `emitInstruction` فلا تبلغه بوّابةُ المعماريّة كذلك.
//    الحسمُ (وصلُه أو حذفُه) رقعةٌ مستقلّةٌ بقياسها؛ وإصلاحُه بلا حسمٍ يشتري
//    شيفرةً صحيحةً لا يشغّلها أحد.
void LLVMPaging::emitFlushTLB(llvm::IRBuilder<>& builder) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    
    // قراءة CR3
    auto* readTy = llvm::FunctionType::get(i64Ty, false);
    auto* readAsm = llvm::InlineAsm::get(
        readTy, "mov %cr3, $0", "=r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    auto* cr3 = builder.CreateCall(readAsm, {}, "cr3");
    
    // إعادة كتابة CR3 (يُبطل كل TLB)
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* writeTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    auto* writeAsm = llvm::InlineAsm::get(
        writeTy, "mov $0, %cr3", "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    builder.CreateCall(writeAsm, {cr3});
}

void LLVMPaging::emitLoadPageTable(llvm::IRBuilder<>& builder,
                                     llvm::Value* pml4PhysAddr) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "mov $0, %cr3", "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm, {pml4PhysAddr});
}

void LLVMPaging::emitEnablePaging(llvm::IRBuilder<>& builder) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    
    // قراءة CR0
    auto* readTy = llvm::FunctionType::get(i64Ty, false);
    auto* readAsm = llvm::InlineAsm::get(
        readTy, "mov %cr0, $0", "=r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    auto* cr0 = builder.CreateCall(readAsm, {}, "cr0");
    
    // ضبط بت PG (بت 31)
    auto* pgBit = llvm::ConstantInt::get(i64Ty, 1ULL << 31);
    auto* newCR0 = builder.CreateOr(cr0, pgBit, "cr0_pg");
    
    // كتابة CR0
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* writeTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    auto* writeAsm = llvm::InlineAsm::get(
        writeTy, "mov $0, %cr0", "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    builder.CreateCall(writeAsm, {newCR0});
}

} // namespace codegen
} // namespace sad
