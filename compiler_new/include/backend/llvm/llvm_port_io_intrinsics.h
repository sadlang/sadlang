/**
 * =============================================================================
 * ملف: llvm_port_io_intrinsics.h
 * الوصف: توليد LLVM IR لعمليات إدخال/إخراج المنافذ
 * المهمة: م-ج03 - واجهة إدخال/إخراج المنافذ
 * =============================================================================
 *
 * يُولّد كود LLVM IR مُضمّن (inline always) لتعليمات x86:
 *   in al, dx    — قراءة 8-bit
 *   in ax, dx    — قراءة 16-bit
 *   in eax, dx   — قراءة 32-bit
 *   out dx, al   — كتابة 8-bit
 *   out dx, ax   — كتابة 16-bit
 *   out dx, eax  — كتابة 32-bit
 *
 * الدوال المُولَّدة تكون always_inline — بدون overhead لاستدعاء دالي.
 * =============================================================================
 */

#ifndef SAD_LLVM_PORT_IO_INTRINSICS_H
#define SAD_LLVM_PORT_IO_INTRINSICS_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/InlineAsm.h>

namespace sad {
namespace codegen {

/**
 * مُولّد تعليمات Port I/O لـ LLVM IR
 *
 * يُنشئ دوال مُضمّنة في LLVM Module:
 *   @sad_port_in_u8(i16 port) -> i8       alwaysinline
 *   @sad_port_in_u16(i16 port) -> i16     alwaysinline
 *   @sad_port_in_u32(i16 port) -> i32     alwaysinline
 *   @sad_port_out_u8(i16 port, i8 val)    alwaysinline
 *   @sad_port_out_u16(i16 port, i16 val)  alwaysinline
 *   @sad_port_out_u32(i16 port, i32 val)  alwaysinline
 */
class LLVMPortIOIntrinsics {
public:
    /**
     * إنشاء مع LLVM Module
     */
    explicit LLVMPortIOIntrinsics(llvm::Module& module);
    
    /**
     * تسجيل كل دوال Port I/O في الموديول
     * يجب استدعاؤها مرة واحدة عند بداية الترجمة
     */
    void registerAll();
    
    // ═══════════════════════════════════════════════════════════════════
    //                     توليد تعليمات القراءة
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * توليد قراءة 8-bit من منفذ
     * @param builder مُنشئ IR
     * @param port قيمة المنفذ (i16)
     * @return قيمة i8 المقروءة
     */
    llvm::Value* emitPortInU8(llvm::IRBuilder<>& builder, llvm::Value* port);
    
    /**
     * توليد قراءة 16-bit من منفذ
     */
    llvm::Value* emitPortInU16(llvm::IRBuilder<>& builder, llvm::Value* port);
    
    /**
     * توليد قراءة 32-bit من منفذ
     */
    llvm::Value* emitPortInU32(llvm::IRBuilder<>& builder, llvm::Value* port);
    
    // ═══════════════════════════════════════════════════════════════════
    //                     توليد تعليمات الكتابة
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * توليد كتابة 8-bit إلى منفذ
     * @param builder مُنشئ IR
     * @param port قيمة المنفذ (i16)
     * @param value القيمة للكتابة (i8)
     */
    void emitPortOutU8(llvm::IRBuilder<>& builder, llvm::Value* port, llvm::Value* value);
    
    /**
     * توليد كتابة 16-bit إلى منفذ
     */
    void emitPortOutU16(llvm::IRBuilder<>& builder, llvm::Value* port, llvm::Value* value);
    
    /**
     * توليد كتابة 32-bit إلى منفذ
     */
    void emitPortOutU32(llvm::IRBuilder<>& builder, llvm::Value* port, llvm::Value* value);
    
    // ═══════════════════════════════════════════════════════════════════
    //                     توليد I/O wait
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * توليد تأخير I/O (كتابة للمنفذ 0x80)
     */
    void emitIOWait(llvm::IRBuilder<>& builder);
    
private:
    llvm::Module& m_module;
    llvm::LLVMContext& m_context;
    
    // دوال مُسجّلة في الموديول
    llvm::Function* m_portInU8 = nullptr;
    llvm::Function* m_portInU16 = nullptr;
    llvm::Function* m_portInU32 = nullptr;
    llvm::Function* m_portOutU8 = nullptr;
    llvm::Function* m_portOutU16 = nullptr;
    llvm::Function* m_portOutU32 = nullptr;
    
    // إنشاء دوال فردية
    void createPortInFunction(const char* name, llvm::Type* resultType,
                              const char* asmStr, const char* constraints);
    void createPortOutFunction(const char* name, llvm::Type* valueType,
                               const char* asmStr, const char* constraints);
};

} // namespace codegen
} // namespace sad

#endif // SAD_LLVM_PORT_IO_INTRINSICS_H
