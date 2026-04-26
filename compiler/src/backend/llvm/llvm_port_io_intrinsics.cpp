/**
 * =============================================================================
 * ملف: llvm_port_io_intrinsics.cpp
 * الوصف: تنفيذ توليد LLVM IR لعمليات Port I/O
 * المهمة: م-ج03 - واجهة إدخال/إخراج المنافذ
 * =============================================================================
 *
 * يُولّد دوال LLVM IR تحتوي تجميع مُضمّن (inline asm) لتعليمات x86.
 * كل دالة تكون مُعلّمة بـ alwaysinline — المترجم يُدمجها في المستدعي.
 *
 * مثال على الـ IR المُولَّد لـ port_in_u8:
 *
 *   define i8 @sad_port_in_u8(i16 %port) alwaysinline {
 *     %result = call i8 asm sideeffect "inb $1, $0",
 *                    "={al},{dx}"(i16 %port)
 *     ret i8 %result
 *   }
 * =============================================================================
 */

#include "backend/llvm/llvm_port_io_intrinsics.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Attributes.h>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════════
//                           إنشاء وتسجيل
// ═══════════════════════════════════════════════════════════════════════════════

LLVMPortIOIntrinsics::LLVMPortIOIntrinsics(llvm::Module& module)
    : m_module(module)
    , m_context(module.getContext()) {}

void LLVMPortIOIntrinsics::registerAll() {
    auto* i8Ty = llvm::Type::getInt8Ty(m_context);
    auto* i16Ty = llvm::Type::getInt16Ty(m_context);
    auto* i32Ty = llvm::Type::getInt32Ty(m_context);
    auto* voidTy = llvm::Type::getVoidTy(m_context);
    
    // ─── دوال القراءة (Port In) ─────────────────────────────────────
    
    // i8 @sad_port_in_u8(i16 port) — in al, dx
    {
        auto* funcTy = llvm::FunctionType::get(i8Ty, {i16Ty}, false);
        m_portInU8 = llvm::Function::Create(
            funcTy, llvm::Function::ExternalLinkage,
            "sad_port_in_u8", m_module);
        m_portInU8->addFnAttr(llvm::Attribute::AlwaysInline);
        m_portInU8->addFnAttr(llvm::Attribute::NoUnwind);
        m_portInU8->setDoesNotThrow();
        
        auto* bb = llvm::BasicBlock::Create(m_context, "entry", m_portInU8);
        llvm::IRBuilder<> builder(bb);
        
        auto* asmTy = llvm::FunctionType::get(i8Ty, {i16Ty}, false);
        auto* inlineAsm = llvm::InlineAsm::get(
            asmTy, "inb $1, $0", "={al},{dx}", true);
        
        auto* result = builder.CreateCall(inlineAsm, {m_portInU8->getArg(0)});
        builder.CreateRet(result);
    }
    
    // i16 @sad_port_in_u16(i16 port) — in ax, dx
    {
        auto* funcTy = llvm::FunctionType::get(i16Ty, {i16Ty}, false);
        m_portInU16 = llvm::Function::Create(
            funcTy, llvm::Function::ExternalLinkage,
            "sad_port_in_u16", m_module);
        m_portInU16->addFnAttr(llvm::Attribute::AlwaysInline);
        m_portInU16->addFnAttr(llvm::Attribute::NoUnwind);
        
        auto* bb = llvm::BasicBlock::Create(m_context, "entry", m_portInU16);
        llvm::IRBuilder<> builder(bb);
        
        auto* asmTy = llvm::FunctionType::get(i16Ty, {i16Ty}, false);
        auto* inlineAsm = llvm::InlineAsm::get(
            asmTy, "inw $1, $0", "={ax},{dx}", true);
        
        auto* result = builder.CreateCall(inlineAsm, {m_portInU16->getArg(0)});
        builder.CreateRet(result);
    }
    
    // i32 @sad_port_in_u32(i16 port) — in eax, dx
    {
        auto* funcTy = llvm::FunctionType::get(i32Ty, {i16Ty}, false);
        m_portInU32 = llvm::Function::Create(
            funcTy, llvm::Function::ExternalLinkage,
            "sad_port_in_u32", m_module);
        m_portInU32->addFnAttr(llvm::Attribute::AlwaysInline);
        m_portInU32->addFnAttr(llvm::Attribute::NoUnwind);
        
        auto* bb = llvm::BasicBlock::Create(m_context, "entry", m_portInU32);
        llvm::IRBuilder<> builder(bb);
        
        auto* asmTy = llvm::FunctionType::get(i32Ty, {i16Ty}, false);
        auto* inlineAsm = llvm::InlineAsm::get(
            asmTy, "inl $1, $0", "={eax},{dx}", true);
        
        auto* result = builder.CreateCall(inlineAsm, {m_portInU32->getArg(0)});
        builder.CreateRet(result);
    }
    
    // ─── دوال الكتابة (Port Out) ────────────────────────────────────
    
    // void @sad_port_out_u8(i16 port, i8 val) — out dx, al
    {
        auto* funcTy = llvm::FunctionType::get(voidTy, {i16Ty, i8Ty}, false);
        m_portOutU8 = llvm::Function::Create(
            funcTy, llvm::Function::ExternalLinkage,
            "sad_port_out_u8", m_module);
        m_portOutU8->addFnAttr(llvm::Attribute::AlwaysInline);
        m_portOutU8->addFnAttr(llvm::Attribute::NoUnwind);
        
        auto* bb = llvm::BasicBlock::Create(m_context, "entry", m_portOutU8);
        llvm::IRBuilder<> builder(bb);
        
        auto* asmTy = llvm::FunctionType::get(voidTy, {i8Ty, i16Ty}, false);
        auto* inlineAsm = llvm::InlineAsm::get(
            asmTy, "outb $0, $1", "{al},{dx}", true);
        
        builder.CreateCall(inlineAsm, {m_portOutU8->getArg(1), m_portOutU8->getArg(0)});
        builder.CreateRetVoid();
    }
    
    // void @sad_port_out_u16(i16 port, i16 val) — out dx, ax
    {
        auto* funcTy = llvm::FunctionType::get(voidTy, {i16Ty, i16Ty}, false);
        m_portOutU16 = llvm::Function::Create(
            funcTy, llvm::Function::ExternalLinkage,
            "sad_port_out_u16", m_module);
        m_portOutU16->addFnAttr(llvm::Attribute::AlwaysInline);
        m_portOutU16->addFnAttr(llvm::Attribute::NoUnwind);
        
        auto* bb = llvm::BasicBlock::Create(m_context, "entry", m_portOutU16);
        llvm::IRBuilder<> builder(bb);
        
        auto* asmTy = llvm::FunctionType::get(voidTy, {i16Ty, i16Ty}, false);
        auto* inlineAsm = llvm::InlineAsm::get(
            asmTy, "outw $0, $1", "{ax},{dx}", true);
        
        builder.CreateCall(inlineAsm, {m_portOutU16->getArg(1), m_portOutU16->getArg(0)});
        builder.CreateRetVoid();
    }
    
    // void @sad_port_out_u32(i16 port, i32 val) — out dx, eax
    {
        auto* funcTy = llvm::FunctionType::get(voidTy, {i16Ty, i32Ty}, false);
        m_portOutU32 = llvm::Function::Create(
            funcTy, llvm::Function::ExternalLinkage,
            "sad_port_out_u32", m_module);
        m_portOutU32->addFnAttr(llvm::Attribute::AlwaysInline);
        m_portOutU32->addFnAttr(llvm::Attribute::NoUnwind);
        
        auto* bb = llvm::BasicBlock::Create(m_context, "entry", m_portOutU32);
        llvm::IRBuilder<> builder(bb);
        
        auto* asmTy = llvm::FunctionType::get(voidTy, {i32Ty, i16Ty}, false);
        auto* inlineAsm = llvm::InlineAsm::get(
            asmTy, "outl $0, $1", "{eax},{dx}", true);
        
        builder.CreateCall(inlineAsm, {m_portOutU32->getArg(1), m_portOutU32->getArg(0)});
        builder.CreateRetVoid();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//                     توليد تعليمات (للاستدعاء المباشر)
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Value* LLVMPortIOIntrinsics::emitPortInU8(
    llvm::IRBuilder<>& builder, llvm::Value* port) {
    
    if (m_portInU8) {
        return builder.CreateCall(m_portInU8, {port}, "port_in_u8");
    }
    
    // Fallback: inline asm مباشر
    auto* i8Ty = llvm::Type::getInt8Ty(m_context);
    auto* i16Ty = llvm::Type::getInt16Ty(m_context);
    auto* asmTy = llvm::FunctionType::get(i8Ty, {i16Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, "inb $1, $0", "={al},{dx}", true);
    return builder.CreateCall(inlineAsm, {port}, "port_in_u8");
}

llvm::Value* LLVMPortIOIntrinsics::emitPortInU16(
    llvm::IRBuilder<>& builder, llvm::Value* port) {
    
    if (m_portInU16) {
        return builder.CreateCall(m_portInU16, {port}, "port_in_u16");
    }
    
    auto* i16Ty = llvm::Type::getInt16Ty(m_context);
    auto* asmTy = llvm::FunctionType::get(i16Ty, {i16Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, "inw $1, $0", "={ax},{dx}", true);
    return builder.CreateCall(inlineAsm, {port}, "port_in_u16");
}

llvm::Value* LLVMPortIOIntrinsics::emitPortInU32(
    llvm::IRBuilder<>& builder, llvm::Value* port) {
    
    if (m_portInU32) {
        return builder.CreateCall(m_portInU32, {port}, "port_in_u32");
    }
    
    auto* i16Ty = llvm::Type::getInt16Ty(m_context);
    auto* i32Ty = llvm::Type::getInt32Ty(m_context);
    auto* asmTy = llvm::FunctionType::get(i32Ty, {i16Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, "inl $1, $0", "={eax},{dx}", true);
    return builder.CreateCall(inlineAsm, {port}, "port_in_u32");
}

void LLVMPortIOIntrinsics::emitPortOutU8(
    llvm::IRBuilder<>& builder, llvm::Value* port, llvm::Value* value) {
    
    if (m_portOutU8) {
        builder.CreateCall(m_portOutU8, {port, value});
        return;
    }
    
    auto* i8Ty = llvm::Type::getInt8Ty(m_context);
    auto* i16Ty = llvm::Type::getInt16Ty(m_context);
    auto* voidTy = llvm::Type::getVoidTy(m_context);
    auto* asmTy = llvm::FunctionType::get(voidTy, {i8Ty, i16Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, "outb $0, $1", "{al},{dx}", true);
    builder.CreateCall(inlineAsm, {value, port});
}

void LLVMPortIOIntrinsics::emitPortOutU16(
    llvm::IRBuilder<>& builder, llvm::Value* port, llvm::Value* value) {
    
    if (m_portOutU16) {
        builder.CreateCall(m_portOutU16, {port, value});
        return;
    }
    
    auto* i16Ty = llvm::Type::getInt16Ty(m_context);
    auto* voidTy = llvm::Type::getVoidTy(m_context);
    auto* asmTy = llvm::FunctionType::get(voidTy, {i16Ty, i16Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, "outw $0, $1", "{ax},{dx}", true);
    builder.CreateCall(inlineAsm, {value, port});
}

void LLVMPortIOIntrinsics::emitPortOutU32(
    llvm::IRBuilder<>& builder, llvm::Value* port, llvm::Value* value) {
    
    if (m_portOutU32) {
        builder.CreateCall(m_portOutU32, {port, value});
        return;
    }
    
    auto* i16Ty = llvm::Type::getInt16Ty(m_context);
    auto* i32Ty = llvm::Type::getInt32Ty(m_context);
    auto* voidTy = llvm::Type::getVoidTy(m_context);
    auto* asmTy = llvm::FunctionType::get(voidTy, {i32Ty, i16Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, "outl $0, $1", "{eax},{dx}", true);
    builder.CreateCall(inlineAsm, {value, port});
}

void LLVMPortIOIntrinsics::emitIOWait(llvm::IRBuilder<>& builder) {
    auto* i8Ty = llvm::Type::getInt8Ty(m_context);
    auto* voidTy = llvm::Type::getVoidTy(m_context);
    
    auto* asmTy = llvm::FunctionType::get(voidTy, {i8Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, "outb $0, $$0x80", "{al}", true);
    
    auto* zero = llvm::ConstantInt::get(i8Ty, 0);
    builder.CreateCall(inlineAsm, {zero});
}

} // namespace codegen
} // namespace sad
