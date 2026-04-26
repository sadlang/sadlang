/**
 * =============================================================================
 * ملف: llvm_kernel_sync_ir.cpp
 * الوصف: تنفيذ بدائيات المزامنة عبر LLVM IRBuilder (النسخة الجديدة)
 * المهمة: م-د04 - دمج LLVMKernelSyncCodegen
 * =============================================================================
 *
 * هذا الملف هو النسخة الجديدة التي تستخدم IRBuilder مباشرة
 * بدلاً من توليد نصوص IR كسلاسل نصية
 * الملف القديم llvm_kernel_sync.cpp يبقى كمرجع
 * =============================================================================
 */

#include "backend/llvm/llvm_kernel_sync.h"

#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════════
//                    المُنشئ
// ═══════════════════════════════════════════════════════════════════════════════

LLVMKernelSync::LLVMKernelSync(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

std::string LLVMKernelSync::nextLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(m_labelCounter++);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    القفل الدوراني (Spinlock)
// ═══════════════════════════════════════════════════════════════════════════════

llvm::StructType* LLVMKernelSync::getSpinlockType() {
    if (m_spinlockType) return m_spinlockType;
    
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    m_spinlockType = llvm::StructType::create(m_ctx, {i32Ty}, "SpinLock");
    return m_spinlockType;
}

void LLVMKernelSync::emitSpinlockInit(llvm::IRBuilder<>& builder,
                                        llvm::Value* lockPtr) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* zero = llvm::ConstantInt::get(i32Ty, 0);
    
    // الحصول على مؤشر حقل locked
    auto* fieldPtr = builder.CreateStructGEP(getSpinlockType(), lockPtr, 0,
                                              "lock_field");
    
    // تهيئة بالقيمة 0 (مفتوح)
    builder.CreateStore(zero, fieldPtr);
}

void LLVMKernelSync::emitSpinlockAcquire(llvm::IRBuilder<>& builder,
                                           llvm::Value* lockPtr,
                                           llvm::Function* parentFunc) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* zero = llvm::ConstantInt::get(i32Ty, 0);
    auto* one = llvm::ConstantInt::get(i32Ty, 1);
    
    // الحصول على مؤشر حقل locked
    auto* fieldPtr = builder.CreateStructGEP(getSpinlockType(), lockPtr, 0,
                                              "lock_field");
    
    // إنشاء الكتل
    std::string label = nextLabel("spinlock");
    auto* tryBB = llvm::BasicBlock::Create(m_ctx, label + "_try", parentFunc);
    auto* spinBB = llvm::BasicBlock::Create(m_ctx, label + "_spin", parentFunc);
    auto* acquiredBB = llvm::BasicBlock::Create(m_ctx, label + "_acquired", parentFunc);
    
    builder.CreateBr(tryBB);
    
    // ─── كتلة المحاولة: CAS(0 → 1) ───────────────────────────────────
    builder.SetInsertPoint(tryBB);
    auto* casResult = builder.CreateAtomicCmpXchg(
        fieldPtr, zero, one,
        llvm::MaybeAlign(4),
        llvm::AtomicOrdering::Acquire,
        llvm::AtomicOrdering::Monotonic);
    
    auto* success = builder.CreateExtractValue(casResult, 1, "cas_success");
    builder.CreateCondBr(success, acquiredBB, spinBB);
    
    // ─── كتلة الدوران: pause + قراءة استرخائية ─────────────────────────
    builder.SetInsertPoint(spinBB);
    
    // pause hint لتقليل استهلاك الطاقة وتجنب cache line bouncing
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* pauseTy = llvm::FunctionType::get(voidTy, false);
    auto* pauseAsm = llvm::InlineAsm::get(pauseTy, "pause", "",
                                            true, false,
                                            llvm::InlineAsm::AD_ATT);
    builder.CreateCall(pauseAsm);
    
    // قراءة استرخائية لتجنب ضغط الناقل
    auto* val = builder.CreateAlignedLoad(i32Ty, fieldPtr, llvm::MaybeAlign(4));
    // ضبط ترتيب القراءة
    if (auto* loadInst = llvm::dyn_cast<llvm::LoadInst>(val)) {
        loadInst->setAtomic(llvm::AtomicOrdering::Monotonic);
    }
    
    auto* isFree = builder.CreateICmpEQ(val, zero, "is_free");
    builder.CreateCondBr(isFree, tryBB, spinBB);
    
    // ─── ما بعد القفل ─────────────────────────────────────────────────
    builder.SetInsertPoint(acquiredBB);
}

void LLVMKernelSync::emitSpinlockRelease(llvm::IRBuilder<>& builder,
                                           llvm::Value* lockPtr) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* zero = llvm::ConstantInt::get(i32Ty, 0);
    
    auto* fieldPtr = builder.CreateStructGEP(getSpinlockType(), lockPtr, 0,
                                              "lock_field");
    
    // store atomic release — يضمن أن كل الكتابات السابقة مرئية
    auto* storeInst = builder.CreateAlignedStore(zero, fieldPtr, llvm::MaybeAlign(4));
    if (auto* si = llvm::dyn_cast<llvm::StoreInst>(storeInst)) {
        si->setAtomic(llvm::AtomicOrdering::Release);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    قفل التذكرة (Ticket Lock)
// ═══════════════════════════════════════════════════════════════════════════════

llvm::StructType* LLVMKernelSync::getTicketLockType() {
    if (m_ticketLockType) return m_ticketLockType;
    
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    m_ticketLockType = llvm::StructType::create(
        m_ctx, {i32Ty, i32Ty}, "TicketLock");
    return m_ticketLockType;
}

void LLVMKernelSync::emitTicketLockInit(llvm::IRBuilder<>& builder,
                                          llvm::Value* lockPtr) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* zero = llvm::ConstantInt::get(i32Ty, 0);
    
    auto* nextPtr = builder.CreateStructGEP(getTicketLockType(), lockPtr, 0,
                                             "next_ticket");
    auto* servingPtr = builder.CreateStructGEP(getTicketLockType(), lockPtr, 1,
                                                "now_serving");
    
    builder.CreateStore(zero, nextPtr);
    builder.CreateStore(zero, servingPtr);
}

void LLVMKernelSync::emitTicketLockAcquire(llvm::IRBuilder<>& builder,
                                             llvm::Value* lockPtr,
                                             llvm::Function* parentFunc) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* one = llvm::ConstantInt::get(i32Ty, 1);
    
    auto* nextPtr = builder.CreateStructGEP(getTicketLockType(), lockPtr, 0,
                                             "next_ticket");
    auto* servingPtr = builder.CreateStructGEP(getTicketLockType(), lockPtr, 1,
                                                "now_serving");
    
    // الحصول على تذكرة: fetch_add(next_ticket, 1)
    auto* myTicket = builder.CreateAtomicRMW(
        llvm::AtomicRMWInst::Add, nextPtr, one,
        llvm::MaybeAlign(4),
        llvm::AtomicOrdering::Monotonic);
    myTicket->setName("my_ticket");
    
    // إنشاء كتل الانتظار
    std::string label = nextLabel("ticket");
    auto* waitBB = llvm::BasicBlock::Create(m_ctx, label + "_wait", parentFunc);
    auto* acquiredBB = llvm::BasicBlock::Create(m_ctx, label + "_acquired", parentFunc);
    
    builder.CreateBr(waitBB);
    
    // ─── حلقة الانتظار ──────────────────────────────────────────────
    builder.SetInsertPoint(waitBB);
    
    // قراءة now_serving
    auto* serving = builder.CreateAlignedLoad(i32Ty, servingPtr, llvm::MaybeAlign(4));
    if (auto* li = llvm::dyn_cast<llvm::LoadInst>(serving)) {
        li->setAtomic(llvm::AtomicOrdering::Acquire);
    }
    serving->setName("serving");
    
    auto* isMyTurn = builder.CreateICmpEQ(serving, myTicket, "is_my_turn");
    builder.CreateCondBr(isMyTurn, acquiredBB, waitBB);
    
    // ─── ما بعد القفل ─────────────────────────────────────────────────
    builder.SetInsertPoint(acquiredBB);
}

void LLVMKernelSync::emitTicketLockRelease(llvm::IRBuilder<>& builder,
                                             llvm::Value* lockPtr) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* one = llvm::ConstantInt::get(i32Ty, 1);
    
    auto* servingPtr = builder.CreateStructGEP(getTicketLockType(), lockPtr, 1,
                                                "now_serving");
    
    // fetch_add(now_serving, 1) مع release ordering
    builder.CreateAtomicRMW(
        llvm::AtomicRMWInst::Add, servingPtr, one,
        llvm::MaybeAlign(4),
        llvm::AtomicOrdering::Release);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    حواجز الذاكرة
// ═══════════════════════════════════════════════════════════════════════════════

void LLVMKernelSync::emitFenceSeqCst(llvm::IRBuilder<>& builder) {
    builder.CreateFence(llvm::AtomicOrdering::SequentiallyConsistent);
}

void LLVMKernelSync::emitFenceAcquire(llvm::IRBuilder<>& builder) {
    builder.CreateFence(llvm::AtomicOrdering::Acquire);
}

void LLVMKernelSync::emitFenceRelease(llvm::IRBuilder<>& builder) {
    builder.CreateFence(llvm::AtomicOrdering::Release);
}

void LLVMKernelSync::emitFenceAcqRel(llvm::IRBuilder<>& builder) {
    builder.CreateFence(llvm::AtomicOrdering::AcquireRelease);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    عمليات ذرية
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Value* LLVMKernelSync::emitCompareAndSwap(
    llvm::IRBuilder<>& builder,
    llvm::Value* ptr,
    llvm::Value* expected,
    llvm::Value* desired) {
    
    auto* cas = builder.CreateAtomicCmpXchg(
        ptr, expected, desired,
        llvm::MaybeAlign(4),
        llvm::AtomicOrdering::AcquireRelease,
        llvm::AtomicOrdering::Monotonic);
    
    // إرجاع {القيمة القديمة, هل نجح؟}
    return cas;
}

llvm::Value* LLVMKernelSync::emitFetchAdd(
    llvm::IRBuilder<>& builder,
    llvm::Value* ptr,
    llvm::Value* val) {
    
    return builder.CreateAtomicRMW(
        llvm::AtomicRMWInst::Add, ptr, val,
        llvm::MaybeAlign(4),
        llvm::AtomicOrdering::AcquireRelease);
}

llvm::Value* LLVMKernelSync::emitAtomicLoad(
    llvm::IRBuilder<>& builder,
    llvm::Value* ptr) {
    
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* load = builder.CreateAlignedLoad(i32Ty, ptr, llvm::MaybeAlign(4));
    if (auto* li = llvm::dyn_cast<llvm::LoadInst>(load)) {
        li->setAtomic(llvm::AtomicOrdering::Acquire);
    }
    return load;
}

void LLVMKernelSync::emitAtomicStore(
    llvm::IRBuilder<>& builder,
    llvm::Value* ptr,
    llvm::Value* val) {
    
    auto* store = builder.CreateAlignedStore(val, ptr, llvm::MaybeAlign(4));
    if (auto* si = llvm::dyn_cast<llvm::StoreInst>(store)) {
        si->setAtomic(llvm::AtomicOrdering::Release);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    تعطيل/تفعيل المقاطعات
// ═══════════════════════════════════════════════════════════════════════════════

void LLVMKernelSync::emitDisableInterrupts(llvm::IRBuilder<>& builder) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, false);
    auto* asmInst = llvm::InlineAsm::get(funcTy, "cli", "",
                                          true, false,
                                          llvm::InlineAsm::AD_ATT);
    builder.CreateCall(asmInst);
}

void LLVMKernelSync::emitEnableInterrupts(llvm::IRBuilder<>& builder) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, false);
    auto* asmInst = llvm::InlineAsm::get(funcTy, "sti", "",
                                          true, false,
                                          llvm::InlineAsm::AD_ATT);
    builder.CreateCall(asmInst);
}

} // namespace codegen
} // namespace sad
