/*
 * ============================================================================
 * عمليات volatile والذاكرة المعيّنة (MMIO) — ملف الرأس
 * Volatile Operations & Memory-Mapped I/O — Header File
 * ============================================================================
 * 
 * م-أ03: ربط أنواع volatile والذاكرة المعيّنة
 * 
 * النواة تحتاج الوصول لسجلات الأجهزة عبر متطاير<ن> (volatile<T>).
 * يجب أن يُولد المترجم تعليمات volatile load/store في LLVM IR
 * لمنع أي تحسين (optimization) من حذف أو إعادة ترتيب العمليات.
 * 
 * المعادلات:
 *   *متطاير_مؤشر = قيمة  →  store volatile i32 %val, ptr %ptr
 *   متغير س = *متطاير_مؤشر  →  %x = load volatile i32, ptr %ptr
 * 
 * المؤلف: فريق مترجم لغة ص
 * التاريخ: فبراير 2026
 * ============================================================================
 */

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <memory>
#include <string>

namespace Sad {
namespace LLVM {

// ============================================================================
// نوع متطاير — يُغلّف أي نوع لجعل عمليات القراءة/الكتابة volatile
// Volatile wrapper type
// ============================================================================

/**
 * VolatileType — نوع مُغلّف يجعل كل عمليات الذاكرة volatile
 * 
 * عند استخدام متطاير<ع32> في كود ص:
 * - كل عملية قراءة تستخدم `load volatile`
 * - كل عملية كتابة تستخدم `store volatile`
 * - المحسن لا يحذف أو يعيد ترتيب أي عملية
 * 
 * هذا ضروري لسجلات الأجهزة (MMIO) لأن:
 * 1. القراءة قد تغيّر حالة الجهاز (side effect)
 * 2. الكتابة يجب أن تحدث بالترتيب الدقيق
 * 3. القراءة المتكررة قد تُرجع قيماً مختلفة
 */
class LLVMVolatileOps {
public:
    /**
     * المنشئ
     * @param builder بانٍ LLVM IR
     * @param context سياق LLVM
     */
    LLVMVolatileOps(llvm::IRBuilder<>& builder, llvm::LLVMContext& context);
    ~LLVMVolatileOps() = default;

    // ========================================================================
    // عمليات القراءة/الكتابة المتطايرة
    // ========================================================================

    /**
     * توليد volatile load — قراءة متطايرة من عنوان ذاكرة
     * 
     * @param ptr المؤشر الذي نقرأ منه
     * @param type نوع البيانات المُقرأة
     * @param name اسم القيمة الناتجة (اختياري)
     * @return القيمة المُحمّلة
     * 
     * يُولد: %name = load volatile <type>, ptr %ptr
     */
    llvm::Value* emitVolatileLoad(llvm::Value* ptr, llvm::Type* type,
                                  const std::string& name = "volatile_load");

    /**
     * توليد volatile store — كتابة متطايرة إلى عنوان ذاكرة
     * 
     * @param value القيمة المراد كتابتها
     * @param ptr المؤشر الذي نكتب إليه
     * @return تعليمة store الناتجة
     * 
     * يُولد: store volatile <type> %value, ptr %ptr
     */
    llvm::StoreInst* emitVolatileStore(llvm::Value* value, llvm::Value* ptr);

    // ========================================================================
    // عمليات MMIO — قراءة/كتابة سجلات الأجهزة
    // ========================================================================

    /**
     * قراءة سجل MMIO بحجم 8 بت
     * @param address العنوان الفيزيائي للسجل
     */
    llvm::Value* emitMMIO_Read8(llvm::Value* address);

    /**
     * قراءة سجل MMIO بحجم 16 بت
     */
    llvm::Value* emitMMIO_Read16(llvm::Value* address);

    /**
     * قراءة سجل MMIO بحجم 32 بت
     */
    llvm::Value* emitMMIO_Read32(llvm::Value* address);

    /**
     * قراءة سجل MMIO بحجم 64 بت
     */
    llvm::Value* emitMMIO_Read64(llvm::Value* address);

    /**
     * كتابة سجل MMIO بحجم 8 بت
     */
    llvm::StoreInst* emitMMIO_Write8(llvm::Value* address, llvm::Value* value);

    /**
     * كتابة سجل MMIO بحجم 16 بت
     */
    llvm::StoreInst* emitMMIO_Write16(llvm::Value* address, llvm::Value* value);

    /**
     * كتابة سجل MMIO بحجم 32 بت
     */
    llvm::StoreInst* emitMMIO_Write32(llvm::Value* address, llvm::Value* value);

    /**
     * كتابة سجل MMIO بحجم 64 بت
     */
    llvm::StoreInst* emitMMIO_Write64(llvm::Value* address, llvm::Value* value);

    // ========================================================================
    // حواجز الذاكرة — لضمان ترتيب العمليات
    // ========================================================================

    /**
     * حاجز ذاكرة — يمنع إعادة ترتيب القراءة/الكتابة
     * يُولد: fence seq_cst أو fence acquire/release
     */
    void emitMemoryFence(llvm::AtomicOrdering ordering = llvm::AtomicOrdering::SequentiallyConsistent);

    /**
     * فحص ما إذا كان النوع مُغلّفاً بمتطاير
     * يتحقق من وجود علامة volatile على النوع أو المتغير
     */
    static bool isVolatileType(const std::string& typeName);

private:
    llvm::IRBuilder<>& builder_;
    llvm::LLVMContext& context_;

    /**
     * تحويل عنوان عدد صحيح إلى مؤشر
     * يُستخدم مع عناوين MMIO الفيزيائية
     */
    llvm::Value* intToPtr(llvm::Value* address, llvm::Type* pointeeType);
};

} // namespace LLVM
} // namespace Sad
