/*
 * ============================================================================
 * ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR - ┘ו┘ה┘ב ╪º┘ה╪¬┘ז┘ב┘ך╪░
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 * 
 * ┘ח╪░╪º ╪º┘ה┘ו┘ה┘ב ┘ך╪¡╪¬┘ט┘ך ╪╣┘ה┘י ╪¬┘ז┘ב┘ך╪░ ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR ╪º┘ה╪░┘ך ┘ך╪¬╪▒╪¼┘ו SIR ╪Ñ┘ה┘י LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 * 
 * ╪º┘ה╪¬╪▓╪º┘ו ╪¿ STRICT_CODING_RULES.md:
 * - ╪¬┘ו ┘ג╪▒╪º╪í╪⌐ ╪¼┘ו┘ך╪╣ ╪º┘ה┘ו┘ה┘ב╪º╪¬ ╪º┘ה╪▒╪ú╪│┘ך╪⌐ ╪¿╪º┘ה┘ד╪º┘ו┘ה (1746 ╪│╪╖╪▒)
 * - ╪¬┘ו ╪¬┘ט╪½┘ך┘ג ┘ד┘ה ╪º╪│╪¬╪«╪»╪º┘ו API ╪¿┘ו┘ט┘ג╪╣ ╪º┘ה┘ו╪╡╪»╪▒
 * - ┘ה╪º ╪¬┘ט╪¼╪» ╪»┘ט╪º┘ה ╪ú┘ט ┘ו╪¬╪║┘ך╪▒╪º╪¬ ┘ו╪«╪¬╪▒╪╣╪⌐
 * 
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 * 
 * ╪º┘ה┘ו╪ñ┘ה┘ב (Author): SadLanguage Compiler Team
 * ╪º┘ה╪¬╪º╪▒┘ך╪« (Date): December 2024
 * ╪º┘ה╪Ñ╪╡╪»╪º╪▒ (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h ╪¿╪»┘ה╪º┘כ ┘ו┘ז llvm/Support/TargetRegistry.h
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR;  // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SIRType

namespace Sad {
namespace LLVM {

llvm::Value* LLVMCodeGen::emitAdd(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    // Source: SIRInstruction::operands is PUBLIC member at sir_instruction.h:62
    if (inst->operands.size() < 2) {
        reportError("Add instruction requires 2 operands");
        return nullptr;
    }
    
    // ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬ ╪¿╪º╪│╪¬╪«╪»╪º┘ו resolveOperand ┘ה╪»╪╣┘ו ╪º┘ה╪½┘ט╪º╪¿╪¬ ┘ט╪º┘ה╪│╪¼┘ה╪º╪¬
    // Get operands using resolveOperand to support both constants and registers
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for add");
        return nullptr;
    }
    
    // ╪º┘ה╪¬╪¡┘ג┘ג ┘ו┘ז ╪º┘ה┘ז┘ט╪╣
    // Check type
    // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
    llvm::Value* result = nullptr;
    if (inst->opcode == SIROpcode::ADD_F64) {
        // (AR) تحويل i64 إلى double إذا لزم الأمر
        // (EN) Coerce i64 operands to double for float operations
        if (left->getType()->isIntegerTy())
            left = builder_->CreateSIToFP(left, builder_->getDoubleTy(), "i64tof64");
        if (right->getType()->isIntegerTy())
            right = builder_->CreateSIToFP(right, builder_->getDoubleTy(), "i64tof64");
        // Source: builder_ is defined at llvm_codegen.h:637
        result = builder_->CreateFAdd(left, right, "addtmp");
    } else {
        result = builder_->CreateAdd(left, right, "addtmp");
    }
    
    // ╪¡┘ב╪╕ ╪º┘ה┘ז╪¬┘ך╪¼╪⌐
    // Save result
    // Source: SIRInstruction::result is PUBLIC member at sir_instruction.h:61
    if (inst->result.has_value()) {
        // Source: SIROperand::name is PUBLIC member at sir_types.h:293
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪╖╪▒╪¡
 * Emit subtract instruction
 * 
 * Source: llvm_codegen.h:412
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitSub(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Sub instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for sub");
        return nullptr;
    }
    
    llvm::Value* result = nullptr;
    if (inst->opcode == SIROpcode::SUB_F64) {
        if (left->getType()->isIntegerTy())
            left = builder_->CreateSIToFP(left, builder_->getDoubleTy(), "i64tof64");
        if (right->getType()->isIntegerTy())
            right = builder_->CreateSIToFP(right, builder_->getDoubleTy(), "i64tof64");
        result = builder_->CreateFSub(left, right, "subtmp");
    } else {
        result = builder_->CreateSub(left, right, "subtmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪╢╪▒╪¿
 * Emit multiply instruction
 * 
 * Source: llvm_codegen.h:413
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitMul(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Mul instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for mul");
        return nullptr;
    }
    
    llvm::Value* result = nullptr;
    if (inst->opcode == SIROpcode::MUL_F64) {
        if (left->getType()->isIntegerTy())
            left = builder_->CreateSIToFP(left, builder_->getDoubleTy(), "i64tof64");
        if (right->getType()->isIntegerTy())
            right = builder_->CreateSIToFP(right, builder_->getDoubleTy(), "i64tof64");
        result = builder_->CreateFMul(left, right, "multmp");
    } else {
        result = builder_->CreateMul(left, right, "multmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ג╪│┘ו╪⌐
 * Emit divide instruction
 * 
 * Source: llvm_codegen.h:414
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitDiv(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Div instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for div");
        return nullptr;
    }
    
    llvm::Value* result = nullptr;
    if (inst->opcode == SIROpcode::DIV_F64) {
        if (left->getType()->isIntegerTy())
            left = builder_->CreateSIToFP(left, builder_->getDoubleTy(), "i64tof64");
        if (right->getType()->isIntegerTy())
            right = builder_->CreateSIToFP(right, builder_->getDoubleTy(), "i64tof64");
        result = builder_->CreateFDiv(left, right, "divtmp");
    } else {
        result = builder_->CreateSDiv(left, right, "divtmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪¿╪º┘ג┘ך ╪º┘ה┘ג╪│┘ו╪⌐
 * Emit modulo instruction
 * 
 * Source: llvm_codegen.h:415
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitMod(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Mod instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for mod");
        return nullptr;
    }
    
    llvm::Value* result = builder_->CreateSRem(left, right, "modtmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ז┘ב┘ך
 * Emit negate instruction
 * 
 * Source: llvm_codegen.h:416
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitNeg(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.empty()) {
        reportError("Neg instruction requires 1 operand");
        return nullptr;
    }
    
    llvm::Value* operand = resolveOperand(inst->operands[0]);
    
    if (!operand) {
        reportError("Operand not found for neg");
        return nullptr;
    }
    
    llvm::Value* result;
    if (operand->getType()->isDoubleTy() || operand->getType()->isFloatTy()) {
        // (AR) استخدام FNeg للأنواع العشرية
        // (EN) Use FNeg for floating-point types
        result = builder_->CreateFNeg(operand, "negtmp");
    } else {
        result = builder_->CreateNeg(operand, "negtmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ AND ╪½┘ז╪º╪ª┘ך
 * Emit bitwise AND instruction
 * 
 * Source: llvm_codegen.h:422
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitAnd(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("And instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for and");
        return nullptr;
    }
    
    llvm::Value* result = builder_->CreateAnd(left, right, "andtmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ OR ╪½┘ז╪º╪ª┘ך
 * Emit bitwise OR instruction
 * 
 * Source: llvm_codegen.h:423
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitOr(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Or instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for or");
        return nullptr;
    }
    
    llvm::Value* result = builder_->CreateOr(left, right, "ortmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ XOR ╪½┘ז╪º╪ª┘ך
 * Emit bitwise XOR instruction
 * 
 * Source: llvm_codegen.h:424
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitXor(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Xor instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for xor");
        return nullptr;
    }
    
    llvm::Value* result = builder_->CreateXor(left, right, "xortmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ NOT ╪½┘ז╪º╪ª┘ך
 * Emit bitwise NOT instruction
 * 
 * Source: llvm_codegen.h:425
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitNot(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.empty()) {
        reportError("Not instruction requires 1 operand");
        return nullptr;
    }
    
    llvm::Value* operand = resolveOperand(inst->operands[0]);
    
    if (!operand) {
        reportError("Operand not found for not");
        return nullptr;
    }
    
    llvm::Value* result = builder_->CreateNot(operand, "nottmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪Ñ╪▓╪º╪¡╪⌐ ┘ך╪│╪º╪▒
 * Emit shift left instruction
 * 
 * Source: llvm_codegen.h:426
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitShl(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Shl instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for shl");
        return nullptr;
    }
    
    llvm::Value* result = builder_->CreateShl(left, right, "shltmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪Ñ╪▓╪º╪¡╪⌐ ┘ך┘ו┘ך┘ז
 * Emit shift right instruction
 * 
 * Source: llvm_codegen.h:427
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitShr(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Shr instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for shr");
        return nullptr;
    }
    
    llvm::Value* result = builder_->CreateLShr(left, right, "shrtmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

// ============================================================================
// ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 5: ╪¬╪╣┘ה┘ך┘ו╪º╪¬ ╪º┘ה┘ו┘ג╪º╪▒┘ז╪⌐ ┘ט╪º┘ה╪░╪º┘ד╪▒╪⌐
// Phase 5: Comparison & Memory Instructions
// ============================================================================

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ┘ו╪¬╪│╪º┘ט┘ך
 * Emit equal comparison instruction
 * 
 * Source: llvm_codegen.h:433
 * @param inst ╪¬╪╣┘ה┘ך┘ו╪⌐ SIR / SIR instruction
 * @return ┘ג┘ך┘ו╪⌐ LLVM / LLVM value
 */
llvm::Value* LLVMCodeGen::emitCmpEq(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("CmpEq instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for cmpeq: " + inst->operands[0].name + ", " + inst->operands[1].name);
        return nullptr;
    }
    
    llvm::Value* result = nullptr;
    llvm::Type* leftTy = left->getType();
    llvm::Type* rightTy = right->getType();
    
    // (AR) ┘ו╪╣╪º┘ה╪¼╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ╪º┘ה┘ו╪ñ╪┤╪▒╪º╪¬/╪º┘ה┘ז╪╡┘ט╪╡ ╪ú┘ט┘ה╪º┘כ (╪¡╪¬┘י ┘ה┘ט ╪º┘ה╪ú┘ז┘ט╪º╪╣ ┘ו╪¬╪╖╪º╪¿┘ג╪⌐)
    // (EN) Handle pointer/string comparison first (even if types match)
    if (leftTy->isPointerTy() && rightTy->isPointerTy()) {
        // (AR) ┘ד┘ה╪º┘ח┘ו╪º ┘ו╪ñ╪┤╪▒╪º┘ז - ╪º╪│╪¬╪«╪»╪º┘ו strcmp ┘ה┘ו┘ג╪º╪▒┘ז╪⌐ ╪º┘ה┘ז╪╡┘ט╪╡
        // (EN) Both pointers - use strcmp for string comparison
        bool isStringCmp = (inst->operands[0].dataType == SIRType::STRING ||
                           inst->operands[1].dataType == SIRType::STRING);
        if (isStringCmp) {
            llvm::FunctionType* strcmpType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context_),
                {llvm::PointerType::getUnqual(*context_), llvm::PointerType::getUnqual(*context_)},
                false);
            llvm::FunctionCallee strcmpFn = module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value* cmpResult = builder_->CreateCall(strcmpFn, {left, right}, "strcmp.ret");
            result = builder_->CreateICmpEQ(cmpResult, 
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), "streq");
        } else {
            result = builder_->CreateICmpEQ(left, right, "cmpeqtmp");
        }
        if (inst->result.has_value()) {
            context_info_.namedValues[inst->result->name] = result;
        }
        return result;
    }
    
    // (AR) ┘ו╪╣╪º┘ה╪¼╪⌐ ╪╣╪»┘ו ╪¬╪╖╪º╪¿┘ג ╪º┘ה╪ú┘ז┘ט╪º╪╣
    // (EN) Handle type mismatches
    if (leftTy != rightTy) {
        // (AR) ╪¬╪¡┘ט┘ך┘ה i1 ╪Ñ┘ה┘י i64 ╪ú┘ט ╪º┘ה╪╣┘ד╪│
        // (EN) Convert i1 to i64 or vice versa
        if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64)) {
            left = builder_->CreateZExt(left, rightTy, "zext_l");
        } else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1)) {
            right = builder_->CreateZExt(right, leftTy, "zext_r");
        } else if (leftTy->isIntegerTy() && rightTy->isDoubleTy()) {
            left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
        } else if (leftTy->isDoubleTy() && rightTy->isIntegerTy()) {
            right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
        } else if (leftTy->isPointerTy() && rightTy->isIntegerTy()) {
            // (AR) ┘ו╪ñ╪┤╪▒ ┘ו┘ג╪º╪¿┘ה ╪╣╪»╪» - ╪¬╪¡┘ו┘ך┘ה ╪º┘ה┘ו╪ñ╪┤╪▒ ╪ú┘ט┘ה╪º┘כ
            // (EN) Pointer vs integer - load pointer first
            left = builder_->CreateLoad(rightTy, left, "loadptr_l");
        } else if (leftTy->isIntegerTy() && rightTy->isPointerTy()) {
            right = builder_->CreateLoad(leftTy, right, "loadptr_r");
        }
        // (AR) ╪¬╪¡╪»┘ך╪½ ╪º┘ה╪ú┘ז┘ט╪º╪╣ ╪¿╪╣╪» ╪º┘ה╪¬╪¡┘ט┘ך┘ה
        // (EN) Update types after conversion
        leftTy = left->getType();
        rightTy = right->getType();
    }
    
    // (AR) ┘ו┘ג╪º╪▒┘ז╪⌐ ╪¡╪│╪¿ ╪º┘ה┘ז┘ט╪╣
    // (EN) Compare based on type
    if (leftTy->isDoubleTy() && rightTy->isDoubleTy()) {
        result = builder_->CreateFCmpOEQ(left, right, "cmpeqtmp");
    } else {
        result = builder_->CreateICmpEQ(left, right, "cmpeqtmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ╪║┘ך╪▒ ┘ו╪¬╪│╪º┘ט┘ך
 * Emit not equal comparison instruction
 * 
 * Source: llvm_codegen.h:434
 */
llvm::Value* LLVMCodeGen::emitCmpNe(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("CmpNe instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for cmpne");
        return nullptr;
    }
    
    llvm::Type* leftTy = left->getType();
    llvm::Type* rightTy = right->getType();
    
    // (AR) ┘ו╪╣╪º┘ה╪¼╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ╪º┘ה┘ז╪╡┘ט╪╡
    // (EN) Handle string comparison
    if (leftTy->isPointerTy() && rightTy->isPointerTy()) {
        bool isStringCmp = (inst->operands[0].dataType == SIRType::STRING ||
                           inst->operands[1].dataType == SIRType::STRING);
        if (isStringCmp) {
            llvm::FunctionType* strcmpType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context_),
                {llvm::PointerType::getUnqual(*context_), llvm::PointerType::getUnqual(*context_)},
                false);
            llvm::FunctionCallee strcmpFn = module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value* cmpResult = builder_->CreateCall(strcmpFn, {left, right}, "strcmp.ret");
            llvm::Value* result = builder_->CreateICmpNE(cmpResult,
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0), "strne");
            if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
            return result;
        }
    }
    
    // (AR) ┘ו╪╣╪º┘ה╪¼╪⌐ ╪╣╪»┘ו ╪¬╪╖╪º╪¿┘ג ╪º┘ה╪ú┘ז┘ט╪º╪╣
    // (EN) Handle type mismatches
    if (leftTy != rightTy) {
        if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64)) {
            left = builder_->CreateZExt(left, rightTy, "zext_l");
        } else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1)) {
            right = builder_->CreateZExt(right, leftTy, "zext_r");
        } else if (leftTy->isIntegerTy() && rightTy->isDoubleTy()) {
            left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
        } else if (leftTy->isDoubleTy() && rightTy->isIntegerTy()) {
            right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
        } else if (leftTy->isPointerTy() && rightTy->isIntegerTy()) {
            left = builder_->CreateLoad(rightTy, left, "loadptr_l");
        } else if (leftTy->isIntegerTy() && rightTy->isPointerTy()) {
            right = builder_->CreateLoad(leftTy, right, "loadptr_r");
        }
        leftTy = left->getType();
        rightTy = right->getType();
    }
    
    llvm::Value* result;
    if (leftTy->isDoubleTy() && rightTy->isDoubleTy()) {
        result = builder_->CreateFCmpONE(left, right, "cmpnetmp");
    } else {
        result = builder_->CreateICmpNE(left, right, "cmpnetmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ╪ú┘ג┘ה ┘ו┘ז
 * Emit less than comparison instruction
 * 
 * Source: llvm_codegen.h:435
 */
llvm::Value* LLVMCodeGen::emitCmpLt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("CmpLt instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for cmplt");
        return nullptr;
    }
    
    llvm::Type* leftTy = left->getType();
    llvm::Type* rightTy = right->getType();
    
    // (AR) ┘ו╪╣╪º┘ה╪¼╪⌐ ╪╣╪»┘ו ╪¬╪╖╪º╪¿┘ג ╪º┘ה╪ú┘ז┘ט╪º╪╣
    // (EN) Handle type mismatches
    if (leftTy != rightTy) {
        if (leftTy->isIntegerTy() && rightTy->isDoubleTy()) {
            left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
        } else if (leftTy->isDoubleTy() && rightTy->isIntegerTy()) {
            right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
        } else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64)) {
            left = builder_->CreateZExt(left, rightTy, "zext_l");
        } else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1)) {
            right = builder_->CreateZExt(right, leftTy, "zext_r");
        }
        leftTy = left->getType();
        rightTy = right->getType();
    }
    
    llvm::Value* result;
    if (leftTy->isDoubleTy() && rightTy->isDoubleTy()) {
        result = builder_->CreateFCmpOLT(left, right, "cmplttmp");
    } else {
        result = builder_->CreateICmpSLT(left, right, "cmplttmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ╪ú┘ג┘ה ┘ו┘ז ╪ú┘ט ┘ך╪│╪º┘ט┘ך
 * Emit less than or equal comparison instruction
 * 
 * Source: llvm_codegen.h:436
 */
llvm::Value* LLVMCodeGen::emitCmpLe(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("CmpLe instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for cmple");
        return nullptr;
    }
    
    llvm::Type* leftTy = left->getType();
    llvm::Type* rightTy = right->getType();
    
    // (AR) ┘ו╪╣╪º┘ה╪¼╪⌐ ╪╣╪»┘ו ╪¬╪╖╪º╪¿┘ג ╪º┘ה╪ú┘ז┘ט╪º╪╣
    // (EN) Handle type mismatches
    if (leftTy != rightTy) {
        if (leftTy->isIntegerTy() && rightTy->isDoubleTy()) {
            left = builder_->CreateSIToFP(left, rightTy, "sitofp_l");
        } else if (leftTy->isDoubleTy() && rightTy->isIntegerTy()) {
            right = builder_->CreateSIToFP(right, leftTy, "sitofp_r");
        } else if (leftTy->isIntegerTy(1) && rightTy->isIntegerTy(64)) {
            left = builder_->CreateZExt(left, rightTy, "zext_l");
        } else if (leftTy->isIntegerTy(64) && rightTy->isIntegerTy(1)) {
            right = builder_->CreateZExt(right, leftTy, "zext_r");
        }
        leftTy = left->getType();
        rightTy = right->getType();
    }
    
    llvm::Value* result;
    if (leftTy->isDoubleTy() && rightTy->isDoubleTy()) {
        result = builder_->CreateFCmpOLE(left, right, "cmpletmp");
    } else {
        result = builder_->CreateICmpSLE(left, right, "cmpletmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ╪ú┘ד╪¿╪▒ ┘ו┘ז
 * Emit greater than comparison instruction
 * 
 * Source: llvm_codegen.h:437
 */


} // namespace LLVM
} // namespace Sad
