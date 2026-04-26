/**
 * =============================================================================
 * ملف: llvm_interrupt_handlers.h
 * الوصف: توليد معالجات المقاطعات وجدول IDT عبر LLVM IRBuilder
 * المهمة: م-د02 - تحويل InterruptCodeGenerator
 * =============================================================================
 *
 * يُولّد:
 *   - أغلفة ISR (Interrupt Service Routine wrappers)
 *   - جدول IDT (256 مُدخل لـ x86_64)
 *   - حفظ/استعادة السجلات
 *   - iretq للعودة من المقاطعة
 * =============================================================================
 */

#ifndef SAD_LLVM_INTERRUPT_HANDLERS_H
#define SAD_LLVM_INTERRUPT_HANDLERS_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace sad {
namespace codegen {

/**
 * مُدخل في جدول IDT
 */
struct IDTEntry {
    uint8_t vector;              // رقم المقاطعة (0-255)
    std::string handlerName;     // اسم دالة المعالج في كود المستخدم
    bool hasErrorCode;           // هل تدفع CPU كود خطأ؟
    bool isTrampoline;           // هل نحتاج trampoline wrapper؟
};

/**
 * إطار معالجة المقاطعة — يُمرَّر للمعالج
 */
struct InterruptFrameLayout {
    // ترتيب الحقول في المكدس (x86_64)
    // RIP, CS, RFLAGS, RSP, SS (يدفعها CPU)
    // + error code (بعض المقاطعات)
    // + سجلات عامة (يحفظها الـ wrapper)
    static constexpr int NUM_GP_REGS = 15;  // rax-r15 (ما عدا rsp)
};

/**
 * مُولّد معالجات المقاطعات
 */
class LLVMInterruptHandlers {
public:
    explicit LLVMInterruptHandlers(llvm::Module& module);
    
    /**
     * تسجيل معالج مقاطعة من سمة عربية
     * #[معالج_مقاطعة(14)] دالة خطأ_صفحة(إطار: &إطار_مقاطعة)
     */
    void registerHandler(uint8_t vector, const std::string& handlerFunc);
    
    /**
     * توليد كل أغلفة ISR + جدول IDT
     */
    void generateAll();
    
    /**
     * توليد غلاف ISR واحد
     */
    llvm::Function* generateISRWrapper(const IDTEntry& entry);
    
    /**
     * توليد جدول IDT كـ global constant
     */
    llvm::GlobalVariable* generateIDTTable();
    
    /**
     * توليد بنية إطار المقاطعة
     */
    llvm::StructType* getInterruptFrameType();

private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_ctx;
    
    std::vector<IDTEntry> m_entries;
    std::unordered_map<uint8_t, llvm::Function*> m_wrappers;
    llvm::StructType* m_frameType = nullptr;
    
    // هل المقاطعة تدفع كود خطأ تلقائياً؟
    static bool vectorHasErrorCode(uint8_t vector);
    
    // توليد حفظ السجلات
    void emitRegisterSave(llvm::IRBuilder<>& builder);
    void emitRegisterRestore(llvm::IRBuilder<>& builder);
    
    // توليد iretq
    void emitIRETQ(llvm::IRBuilder<>& builder);
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_INTERRUPT_HANDLERS_H
