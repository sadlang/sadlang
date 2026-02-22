/*
 * ============================================================================
 * عمليات volatile والذاكرة المعيّنة (MMIO) — ملف التنفيذ
 * Volatile Operations & Memory-Mapped I/O — Implementation File
 * ============================================================================
 * 
 * م-أ03: تنفيذ عمليات volatile load/store لدعم سجلات الأجهزة
 * 
 * المؤلف: فريق مترجم لغة ص
 * التاريخ: فبراير 2026
 * ============================================================================
 */

#include "llvm_volatile_ops.h"
#include <llvm/IR/Instructions.h>

namespace Sad {
namespace LLVM {

// ============================================================================
// المنشئ
// ============================================================================

LLVMVolatileOps::LLVMVolatileOps(llvm::IRBuilder<>& builder, llvm::LLVMContext& context)
    : builder_(builder)
    , context_(context)
{
}

// ============================================================================
// عمليات القراءة/الكتابة المتطايرة
// ============================================================================

/**
 * توليد volatile load
 * 
 * يُولد: %name = load volatile <type>, ptr %ptr
 * 
 * يُستخدم لقراءة سجلات الأجهزة حيث:
 * - كل قراءة تُولد تعليمة load فعلية (لا يتم حذفها بالتحسين)
 * - القراءات لا يُعاد ترتيبها بواسطة المحسن
 */
llvm::Value* LLVMVolatileOps::emitVolatileLoad(
    llvm::Value* ptr, llvm::Type* type, const std::string& name) {
    
    auto* loadInst = builder_.CreateLoad(type, ptr, name);
    loadInst->setVolatile(true);
    return loadInst;
}

/**
 * توليد volatile store
 * 
 * يُولد: store volatile <type> %value, ptr %ptr
 * 
 * يُستخدم لكتابة سجلات الأجهزة حيث:
 * - كل كتابة تُولد تعليمة store فعلية
 * - الكتابات تحدث بالترتيب الدقيق المحدد في الكود
 */
llvm::StoreInst* LLVMVolatileOps::emitVolatileStore(
    llvm::Value* value, llvm::Value* ptr) {
    
    auto* storeInst = builder_.CreateStore(value, ptr);
    storeInst->setVolatile(true);
    return storeInst;
}

// ============================================================================
// عمليات MMIO — القراءة
// ============================================================================

llvm::Value* LLVMVolatileOps::emitMMIO_Read8(llvm::Value* address) {
    auto* ptr = intToPtr(address, llvm::Type::getInt8Ty(context_));
    return emitVolatileLoad(ptr, llvm::Type::getInt8Ty(context_), "mmio_read8");
}

llvm::Value* LLVMVolatileOps::emitMMIO_Read16(llvm::Value* address) {
    auto* ptr = intToPtr(address, llvm::Type::getInt16Ty(context_));
    return emitVolatileLoad(ptr, llvm::Type::getInt16Ty(context_), "mmio_read16");
}

llvm::Value* LLVMVolatileOps::emitMMIO_Read32(llvm::Value* address) {
    auto* ptr = intToPtr(address, llvm::Type::getInt32Ty(context_));
    return emitVolatileLoad(ptr, llvm::Type::getInt32Ty(context_), "mmio_read32");
}

llvm::Value* LLVMVolatileOps::emitMMIO_Read64(llvm::Value* address) {
    auto* ptr = intToPtr(address, llvm::Type::getInt64Ty(context_));
    return emitVolatileLoad(ptr, llvm::Type::getInt64Ty(context_), "mmio_read64");
}

// ============================================================================
// عمليات MMIO — الكتابة
// ============================================================================

llvm::StoreInst* LLVMVolatileOps::emitMMIO_Write8(
    llvm::Value* address, llvm::Value* value) {
    auto* ptr = intToPtr(address, llvm::Type::getInt8Ty(context_));
    return emitVolatileStore(value, ptr);
}

llvm::StoreInst* LLVMVolatileOps::emitMMIO_Write16(
    llvm::Value* address, llvm::Value* value) {
    auto* ptr = intToPtr(address, llvm::Type::getInt16Ty(context_));
    return emitVolatileStore(value, ptr);
}

llvm::StoreInst* LLVMVolatileOps::emitMMIO_Write32(
    llvm::Value* address, llvm::Value* value) {
    auto* ptr = intToPtr(address, llvm::Type::getInt32Ty(context_));
    return emitVolatileStore(value, ptr);
}

llvm::StoreInst* LLVMVolatileOps::emitMMIO_Write64(
    llvm::Value* address, llvm::Value* value) {
    auto* ptr = intToPtr(address, llvm::Type::getInt64Ty(context_));
    return emitVolatileStore(value, ptr);
}

// ============================================================================
// حواجز الذاكرة
// ============================================================================

/**
 * حاجز ذاكرة — يمنع إعادة الترتيب
 * 
 * يُولد: fence <ordering>
 * 
 * الأنماط المدعومة:
 * - SequentiallyConsistent: أقوى ضمان (تسلسلي)
 * - Acquire: ضمان أن القراءات بعده لا تُقدَّم
 * - Release: ضمان أن الكتابات قبله لا تُؤخَّر
 * - AcquireRelease: ضمان acquire + release معاً
 */
void LLVMVolatileOps::emitMemoryFence(llvm::AtomicOrdering ordering) {
    builder_.CreateFence(ordering);
}

// ============================================================================
// فحص النوع المتطاير
// ============================================================================

/**
 * فحص ما إذا كان اسم النوع يحتوي على علامة volatile
 * 
 * الأنماط المعترف بها:
 * - "متطاير<ع32>" — الصيغة العربية
 * - "volatile<i32>" — الصيغة الإنجليزية
 * - "Volatile" — أي نوع يبدأ بـ Volatile
 */
bool LLVMVolatileOps::isVolatileType(const std::string& typeName) {
    // فحص الكلمات المفتاحية العربية والإنجليزية
    if (typeName.find("\xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd9\x8a\xd8\xb1") != std::string::npos) {
        return true;  // "متطاير" بالعربية (UTF-8)
    }
    if (typeName.find("volatile") != std::string::npos || 
        typeName.find("Volatile") != std::string::npos) {
        return true;
    }
    return false;
}

// ============================================================================
// دوال مساعدة
// ============================================================================

/**
 * تحويل عنوان عدد صحيح إلى مؤشر
 * 
 * يُستخدم لتحويل العنوان الفيزيائي (مثل 0xB8000 لشاشة VGA)
 * إلى مؤشر يمكن استخدامه في load/store
 * 
 * في LLVM 15+ مع المؤشرات الشفافة:
 *   inttoptr i64 %addr to ptr
 */
llvm::Value* LLVMVolatileOps::intToPtr(llvm::Value* address, llvm::Type* pointeeType) {
    // التحقق: إذا كان العنوان مؤشراً بالفعل، نُرجعه مباشرة
    if (address->getType()->isPointerTy()) {
        return address;
    }
    
    // LLVM 15+: مؤشرات شفافة — نحول int إلى ptr
    llvm::Type* ptrType = llvm::PointerType::get(context_, 0);
    return builder_.CreateIntToPtr(address, ptrType, "mmio_ptr");
}

} // namespace LLVM
} // namespace Sad
