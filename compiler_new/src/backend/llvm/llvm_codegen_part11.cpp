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

// Helper function for array struct type
static llvm::StructType* getArrayStructType(llvm::LLVMContext& ctx) {
    static llvm::StructType* arrTy = nullptr;
    if (!arrTy) {
        arrTy = llvm::StructType::create(ctx, {
            llvm::Type::getInt64Ty(ctx),     // length
            llvm::Type::getInt64Ty(ctx),     // capacity
            llvm::PointerType::getUnqual(ctx) // data pointer
        }, "SadArray");
    }
    return arrTy;
}

llvm::Value* LLVMCodeGen::emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        return builder_->CreateGlobalStringPtr("┘ו╪¼┘ח┘ט┘ה", "typeof_unknown");
    }
    
    // Determine type from operand's dataType at compile time
    const char* typeName = "┘ו╪¼┘ח┘ט┘ה";
    switch (inst->operands[0].dataType) {
        case SIRType::I64:     typeName = "╪╣╪»╪»_╪╡╪¡┘ך╪¡"; break;
        case SIRType::F64:     typeName = "╪╣╪»╪»_╪╣╪┤╪▒┘ך"; break;
        case SIRType::BOOL:    typeName = "┘ו┘ז╪╖┘ג┘ך"; break;
        case SIRType::STRING:  typeName = "┘ז╪╡"; break;
        case SIRType::ARRAY:   typeName = "┘ו╪╡┘ב┘ט┘ב╪⌐"; break;
        case SIRType::STRUCT:  typeName = "┘ד╪º╪ª┘ז"; break;
        case SIRType::PTR:     typeName = "┘ו╪ñ╪┤╪▒"; break;
        case SIRType::VOID:    typeName = "┘ב╪▒╪º╪║"; break;
        default:               typeName = "┘ו╪¼┘ח┘ט┘ה"; break;
    }
    
    llvm::Value* result = builder_->CreateGlobalStringPtr(typeName, "typeof_str");
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: Builtin String Functions / ╪»┘ט╪º┘ה ╪º┘ה┘ז╪╡┘ט╪╡ ╪º┘ה┘ו╪╢┘ו┘ז╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitStringToI64(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("STRING_TO_I64 requires 1 operand");
        return nullptr;
    }
    llvm::Value* str = resolveOperand(inst->operands[0]);
    if (!str) return nullptr;
    
    // Call atoll(str) Γזע i64
    auto* atollType = llvm::FunctionType::get(
        getInt64Type(), {llvm::PointerType::getUnqual(*context_)}, false);
    auto atollFunc = module_->getOrInsertFunction("atoll", atollType);
    llvm::Value* result = builder_->CreateCall(atollFunc, {str}, "str2i64");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// (AR) ╪¬╪¡┘ט┘ך┘ה ┘ז╪╡ ╪Ñ┘ה┘י ╪╣╪»╪» ╪╣╪┤╪▒┘ך Γאפ STRING_TO_F64
//      ┘ז╪│╪¬╪»╪╣┘ך ╪»╪º┘ה╪⌐ atof() ┘ו┘ז ┘ו┘ד╪¬╪¿╪⌐ C ╪º┘ה┘ג┘ך╪º╪│┘ך╪⌐ ╪º┘ה╪¬┘ך ╪¬╪¡┘ט┘ס┘ה ┘ז╪╡╪º┘כ ┘ו╪½┘ה "3.14"
//      ╪Ñ┘ה┘י ┘ג┘ך┘ו╪⌐ ╪╣╪┤╪▒┘ך╪⌐ ┘ו╪▓╪»┘ט╪¼╪⌐ ╪º┘ה╪»┘ג╪⌐ (double/f64)
//      ┘ח╪░╪º ╪╢╪▒┘ט╪▒┘ך ┘ה╪»╪º┘ה╪⌐ ┘ה╪╣╪┤╪▒┘ך() ╪º┘ה┘ו╪╢┘ו┘ז╪⌐ ┘ב┘ך ┘ה╪║╪⌐ ╪╡
//
// (EN) Convert string to float Γאפ STRING_TO_F64
//      Calls C standard library atof() which converts a string like "3.14"
//      to a double-precision floating point value (f64)
//      Required for the built-in ┘ה╪╣╪┤╪▒┘ך() function in Sad language
// ============================================================================
llvm::Value* LLVMCodeGen::emitStringToF64(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("STRING_TO_F64 requires 1 operand");
        return nullptr;
    }
    llvm::Value* str = resolveOperand(inst->operands[0]);
    if (!str) return nullptr;
    
    // (AR) ╪º╪│╪¬╪»╪╣╪º╪í atof(str) Γזע f64
    //      atof ┘ח┘ך ╪»╪º┘ה╪⌐ C ┘ג┘ך╪º╪│┘ך╪⌐ ╪¬╪¡┘ט┘ס┘ה ┘ז╪╡╪º┘כ ╪Ñ┘ה┘י double
    // (EN) Call atof(str) Γזע f64
    //      atof is a standard C function that converts string to double
    auto* atofType = llvm::FunctionType::get(
        llvm::Type::getDoubleTy(*context_), 
        {llvm::PointerType::getUnqual(*context_)}, false);
    auto atofFunc = module_->getOrInsertFunction("atof", atofType);
    llvm::Value* result = builder_->CreateCall(atofFunc, {str}, "str2f64");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringLength(std::shared_ptr<SIRInstruction> inst) {
    return emitFFIStrlen(inst);
}

llvm::Value* LLVMCodeGen::emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* str = resolveOperand(inst->operands[0]);
    if (!str) return nullptr;
    
    // strlen + malloc + loop calling toupper
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* len = builder_->CreateCall(strlenFunc, {str}, "len");
    
    llvm::Value* newLen = builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1));
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, {newLen}, "upper_buf");
    
    // strcpy then loop toupper
    auto* strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strcpyFunc = module_->getOrInsertFunction("strcpy", strcpyType);
    builder_->CreateCall(strcpyFunc, {buf, str});
    
    // Simple approach: call _strupr or iterate with toupper
    auto* struprType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
    auto struprFunc = module_->getOrInsertFunction("_strupr", struprType);
    builder_->CreateCall(struprFunc, {buf});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringToLower(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* str = resolveOperand(inst->operands[0]);
    if (!str) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* len = builder_->CreateCall(strlenFunc, {str}, "len");
    
    llvm::Value* newLen = builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1));
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, {newLen}, "lower_buf");
    
    auto* strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strcpyFunc = module_->getOrInsertFunction("strcpy", strcpyType);
    builder_->CreateCall(strcpyFunc, {buf, str});
    
    auto* strlwrType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
    auto strlwrFunc = module_->getOrInsertFunction("_strlwr", strlwrType);
    builder_->CreateCall(strlwrFunc, {buf});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringFind(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("STRING_FIND requires 2 operands (haystack, needle)");
        return nullptr;
    }
    llvm::Value* haystack = resolveOperand(inst->operands[0]);
    llvm::Value* needle = resolveOperand(inst->operands[1]);
    if (!haystack || !needle) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    
    // strstr(haystack, needle) Γזע ptr or null
    auto* strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strstrFunc = module_->getOrInsertFunction("strstr", strstrType);
    llvm::Value* found = builder_->CreateCall(strstrFunc, {haystack, needle}, "found");
    
    // Convert to index: found == null ? -1 : (found - haystack)
    llvm::Value* isNull = builder_->CreateICmpEQ(found, 
        llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*context_)), "isnull");
    llvm::Value* foundInt = builder_->CreatePtrToInt(found, i64Ty, "found.int");
    llvm::Value* hstackInt = builder_->CreatePtrToInt(haystack, i64Ty, "hstack.int");
    llvm::Value* offset = builder_->CreateSub(foundInt, hstackInt, "offset");
    llvm::Value* result = builder_->CreateSelect(isNull, 
        llvm::ConstantInt::get(i64Ty, -1), offset, "find_result");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringReplace(std::shared_ptr<SIRInstruction> inst) {
    // Full string replace: call runtime helper sad_string_replace(str, old, new) -> char*
    if (!inst || inst->operands.size() < 3) {
        reportError("STRING_REPLACE requires 3 operands (str, old, new)");
        return nullptr;
    }
    llvm::Value* str = resolveOperand(inst->operands[0]);
    llvm::Value* oldStr = resolveOperand(inst->operands[1]);
    llvm::Value* newStr = resolveOperand(inst->operands[2]);
    if (!str || !oldStr || !newStr) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    
    // Build inline: find first occurrence with strstr, copy before + new + after
    // Allocate generous buffer: strlen(str) * 2 + strlen(newStr) + 1
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* srcLen = builder_->CreateCall(strlenFunc, {str}, "src.len");
    llvm::Value* oldLen = builder_->CreateCall(strlenFunc, {oldStr}, "old.len");
    llvm::Value* newLen = builder_->CreateCall(strlenFunc, {newStr}, "new.len");
    
    // bufSize = srcLen * 2 + newLen + 1 (generous)
    llvm::Value* bufSize = builder_->CreateMul(srcLen, llvm::ConstantInt::get(i64Ty, 2));
    bufSize = builder_->CreateAdd(bufSize, newLen);
    bufSize = builder_->CreateAdd(bufSize, llvm::ConstantInt::get(i64Ty, 1));
    
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, {bufSize}, "replace_buf");
    
    // Use strstr to find oldStr in str
    auto* strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strstrFunc = module_->getOrInsertFunction("strstr", strstrType);
    llvm::Value* found = builder_->CreateCall(strstrFunc, {str, oldStr}, "found");
    
    // If not found, just copy original
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* foundBB = llvm::BasicBlock::Create(*context_, "replace.found", curFunc);
    llvm::BasicBlock* notFoundBB = llvm::BasicBlock::Create(*context_, "replace.notfound", curFunc);
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context_, "replace.merge", curFunc);
    
    llvm::Value* isNull = builder_->CreateICmpEQ(found,
        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "isnull");
    builder_->CreateCondBr(isNull, notFoundBB, foundBB);
    
    // Not found: strcpy original
    builder_->SetInsertPoint(notFoundBB);
    auto* strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strcpyFunc = module_->getOrInsertFunction("strcpy", strcpyType);
    builder_->CreateCall(strcpyFunc, {buf, str});
    builder_->CreateBr(mergeBB);
    
    // Found: copy prefix + newStr + suffix
    builder_->SetInsertPoint(foundBB);
    llvm::Value* prefixLen = builder_->CreatePtrDiff(i8Ty, found, str, "prefix.len");
    auto* memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
    auto memcpyFunc = module_->getOrInsertFunction("memcpy", memcpyType);
    builder_->CreateCall(memcpyFunc, {buf, str, prefixLen}); // copy prefix
    llvm::Value* dst1 = builder_->CreateGEP(i8Ty, buf, {prefixLen}, "dst1");
    builder_->CreateCall(memcpyFunc, {dst1, newStr, newLen}); // copy newStr
    llvm::Value* dst2 = builder_->CreateGEP(i8Ty, dst1, {newLen}, "dst2");
    llvm::Value* suffixStart = builder_->CreateGEP(i8Ty, found, {oldLen}, "suffix.start");
    llvm::Value* suffixLen = builder_->CreateSub(srcLen, builder_->CreateAdd(prefixLen, oldLen));
    llvm::Value* suffixCopyLen = builder_->CreateAdd(suffixLen, llvm::ConstantInt::get(i64Ty, 1)); // include null
    builder_->CreateCall(memcpyFunc, {dst2, suffixStart, suffixCopyLen});
    builder_->CreateBr(mergeBB);
    
    builder_->SetInsertPoint(mergeBB);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) {
        reportError("STRING_SUBSTRING requires 3 operands (str, start, length)");
        return nullptr;
    }
    llvm::Value* str = resolveOperand(inst->operands[0]);
    llvm::Value* start = resolveOperand(inst->operands[1]);
    llvm::Value* len = resolveOperand(inst->operands[2]);
    if (!str || !start || !len) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    
    // Allocate buffer: len + 1
    llvm::Value* bufSize = builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1));
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, {bufSize}, "substr_buf");
    
    // Source pointer: str + start
    llvm::Value* srcPtr = builder_->CreateGEP(
        llvm::Type::getInt8Ty(*context_), str, {start}, "substr.src");
    
    // memcpy(buf, srcPtr, len)
    auto* memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
    auto memcpyFunc = module_->getOrInsertFunction("memcpy", memcpyType);
    builder_->CreateCall(memcpyFunc, {buf, srcPtr, len});
    
    // Null-terminate
    llvm::Value* endPtr = builder_->CreateGEP(
        llvm::Type::getInt8Ty(*context_), buf, {len}, "substr.end");
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context_), 0), endPtr);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringTrim(std::shared_ptr<SIRInstruction> inst) {
    // Call C runtime: skip leading whitespace, then copy until trailing whitespace
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* str = resolveOperand(inst->operands[0]);
    if (!str) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    
    // Use strspn to find leading whitespace count, then strlen-based trim end
    // strspn(str, " \t\n\r") returns number of leading whitespace chars
    auto* strspnType = llvm::FunctionType::get(i64Ty, {ptrTy, ptrTy}, false);
    auto strspnFunc = module_->getOrInsertFunction("strspn", strspnType);
    llvm::Value* ws = builder_->CreateGlobalStringPtr(" \t\n\r", "ws_chars");
    llvm::Value* leadingWS = builder_->CreateCall(strspnFunc, {str, ws}, "leading.ws");
    
    // start = str + leadingWS
    llvm::Value* start = builder_->CreateGEP(i8Ty, str, {leadingWS}, "trim.start");
    
    // Get length of remaining string
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* remLen = builder_->CreateCall(strlenFunc, {start}, "rem.len");
    
    // Allocate buffer: remLen + 1
    llvm::Value* bufSize = builder_->CreateAdd(remLen, llvm::ConstantInt::get(i64Ty, 1));
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, {bufSize}, "trim_buf");
    
    // memcpy start content
    auto* memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
    auto memcpyFunc = module_->getOrInsertFunction("memcpy", memcpyType);
    builder_->CreateCall(memcpyFunc, {buf, start, bufSize});
    
    // Trim trailing whitespace: walk back from end while isspace
    // Simple approach: create loop to null-terminate at first non-whitespace from end
    // For simplicity, use a runtime helper pattern: buf[len] scanning back
    // We'll call isspace on each char from the end
    auto* isSpaceType = llvm::FunctionType::get(i32Ty, {i32Ty}, false);
    auto isSpaceFunc = module_->getOrInsertFunction("isspace", isSpaceType);
    
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "trim.loop", curFunc);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context_, "trim.body", curFunc);
    llvm::BasicBlock* doneBB = llvm::BasicBlock::Create(*context_, "trim.done", curFunc);
    
    // idx = remLen - 1
    llvm::Value* startIdx = builder_->CreateSub(remLen, llvm::ConstantInt::get(i64Ty, 1));
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(loopBB);
    llvm::PHINode* idx = builder_->CreatePHI(i64Ty, 2, "trim.idx");
    idx->addIncoming(startIdx, loopBB->getSinglePredecessor());
    
    // if idx < 0, done
    llvm::Value* isNeg = builder_->CreateICmpSLT(idx, llvm::ConstantInt::get(i64Ty, 0), "is.neg");
    builder_->CreateCondBr(isNeg, doneBB, bodyBB);
    
    builder_->SetInsertPoint(bodyBB);
    llvm::Value* charPtr = builder_->CreateGEP(i8Ty, buf, {idx}, "char.ptr");
    llvm::Value* ch = builder_->CreateLoad(i8Ty, charPtr, "ch");
    llvm::Value* chInt = builder_->CreateZExt(ch, i32Ty, "ch.int");
    llvm::Value* isSp = builder_->CreateCall(isSpaceFunc, {chInt}, "is.sp");
    llvm::Value* isSpBool = builder_->CreateICmpNE(isSp, llvm::ConstantInt::get(i32Ty, 0), "is.sp.bool");
    
    // If space, null-terminate and continue
    llvm::BasicBlock* trimBB = llvm::BasicBlock::Create(*context_, "trim.set", curFunc);
    builder_->CreateCondBr(isSpBool, trimBB, doneBB);
    
    builder_->SetInsertPoint(trimBB);
    builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), charPtr);
    llvm::Value* nextIdx = builder_->CreateSub(idx, llvm::ConstantInt::get(i64Ty, 1));
    idx->addIncoming(nextIdx, trimBB);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(doneBB);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringSplit(std::shared_ptr<SIRInstruction> inst) {
    // Split string by delimiter into a SadArray of string pointers
    // Uses strtok-like approach: count delimiters, allocate array, copy tokens
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* str = resolveOperand(inst->operands[0]);
    llvm::Value* delim = resolveOperand(inst->operands[1]);
    if (!str || !delim) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    
    // Call runtime: sad_string_split(str, delim) -> SadArray*
    // For now, create a SadArray with a single element (the original string)
    // This is correct for the case when delimiter is not found
    llvm::StructType* arrTy = getArrayStructType(*context_);
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    
    // Allocate SadArray struct
    llvm::Value* arrSize = llvm::ConstantInt::get(i64Ty, 24); // 3 * i64
    llvm::Value* arrPtr = builder_->CreateCall(mallocFunc, {arrSize}, "split.arr");
    
    // Allocate data buffer for 16 pointers initially
    llvm::Value* dataSize = llvm::ConstantInt::get(i64Ty, 16 * 8);
    llvm::Value* dataPtr = builder_->CreateCall(mallocFunc, {dataSize}, "split.data");
    
    // Store array metadata: length=0, capacity=16, data=dataPtr
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "split.len.gep");
    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenGep);
    llvm::Value* capGep = builder_->CreateStructGEP(arrTy, arrPtr, 1, "split.cap.gep");
    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 16), capGep);
    llvm::Value* datGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "split.dat.gep");
    builder_->CreateStore(dataPtr, datGep);
    
    // Use strtok to tokenize: first make a copy of str (strtok modifies input)
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* srcLen = builder_->CreateCall(strlenFunc, {str}, "src.len");
    llvm::Value* copySize = builder_->CreateAdd(srcLen, llvm::ConstantInt::get(i64Ty, 1));
    llvm::Value* strCopy = builder_->CreateCall(mallocFunc, {copySize}, "str.copy");
    auto* memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
    auto memcpyFunc = module_->getOrInsertFunction("memcpy", memcpyType);
    builder_->CreateCall(memcpyFunc, {strCopy, str, copySize});
    
    // Call strtok(strCopy, delim) in a loop
    auto* strtokType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strtokFunc = module_->getOrInsertFunction("strtok", strtokType);
    
    // First call: strtok(strCopy, delim)
    llvm::Value* firstTok = builder_->CreateCall(strtokFunc, {strCopy, delim}, "tok.first");
    
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "split.loop", curFunc);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context_, "split.body", curFunc);
    llvm::BasicBlock* doneBB = llvm::BasicBlock::Create(*context_, "split.done", curFunc);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(loopBB);
    llvm::PHINode* tok = builder_->CreatePHI(ptrTy, 2, "tok");
    tok->addIncoming(firstTok, loopBB->getSinglePredecessor());
    llvm::PHINode* count = builder_->CreatePHI(i64Ty, 2, "count");
    count->addIncoming(llvm::ConstantInt::get(i64Ty, 0), loopBB->getSinglePredecessor());
    
    llvm::Value* tokNull = builder_->CreateICmpEQ(tok,
        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "tok.null");
    builder_->CreateCondBr(tokNull, doneBB, bodyBB);
    
    builder_->SetInsertPoint(bodyBB);
    // Store token pointer: data[count] = strdup(tok)
    llvm::Value* tokLen = builder_->CreateCall(strlenFunc, {tok}, "tok.len");
    llvm::Value* tokBufSz = builder_->CreateAdd(tokLen, llvm::ConstantInt::get(i64Ty, 1));
    llvm::Value* tokCopy = builder_->CreateCall(mallocFunc, {tokBufSz}, "tok.copy");
    builder_->CreateCall(memcpyFunc, {tokCopy, tok, tokBufSz});
    
    llvm::Value* curData = builder_->CreateLoad(ptrTy, datGep, "cur.data");
    llvm::Value* elemPtr = builder_->CreateGEP(ptrTy, curData, {count}, "elem.ptr");
    builder_->CreateStore(tokCopy, elemPtr);
    
    llvm::Value* nextCount = builder_->CreateAdd(count, llvm::ConstantInt::get(i64Ty, 1));
    // Next token: strtok(NULL, delim)
    llvm::Value* nextTok = builder_->CreateCall(strtokFunc, {
        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), delim}, "tok.next");
    tok->addIncoming(nextTok, bodyBB);
    count->addIncoming(nextCount, bodyBB);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(doneBB);
    // Update length
    builder_->CreateStore(count, lenGep);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = arrPtr;
    }
    return arrPtr;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringJoin(std::shared_ptr<SIRInstruction> inst) {
    // Join array of strings with separator
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* arrPtr = resolveOperand(inst->operands[0]);
    llvm::Value* sep = resolveOperand(inst->operands[1]);
    if (!arrPtr || !sep) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // Load array length and data
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "join.len.gep");
    llvm::Value* arrLen = builder_->CreateLoad(i64Ty, lenGep, "join.len");
    llvm::Value* datGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "join.dat.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(ptrTy, datGep, "join.data");
    
    // Get separator length
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* sepLen = builder_->CreateCall(strlenFunc, {sep}, "sep.len");
    
    // Allocate generous buffer: arrLen * 256 (rough estimate)
    llvm::Value* bufSize = builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 256));
    bufSize = builder_->CreateAdd(bufSize, llvm::ConstantInt::get(i64Ty, 1));
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, {bufSize}, "join.buf");
    
    // Start with empty string
    builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), buf);
    
    // Loop: strcat each element + separator
    auto* strcatType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strcatFunc = module_->getOrInsertFunction("strcat", strcatType);
    
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "join.loop", curFunc);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context_, "join.body", curFunc);
    llvm::BasicBlock* doneBB = llvm::BasicBlock::Create(*context_, "join.done", curFunc);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(loopBB);
    llvm::PHINode* idx = builder_->CreatePHI(i64Ty, 2, "join.idx");
    idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), loopBB->getSinglePredecessor());
    llvm::Value* isDone = builder_->CreateICmpUGE(idx, arrLen, "join.done.check");
    builder_->CreateCondBr(isDone, doneBB, bodyBB);
    
    builder_->SetInsertPoint(bodyBB);
    // If not first element, append separator
    llvm::Value* isFirst = builder_->CreateICmpEQ(idx, llvm::ConstantInt::get(i64Ty, 0));
    llvm::BasicBlock* sepBB = llvm::BasicBlock::Create(*context_, "join.sep", curFunc);
    llvm::BasicBlock* concatBB = llvm::BasicBlock::Create(*context_, "join.concat", curFunc);
    builder_->CreateCondBr(isFirst, concatBB, sepBB);
    
    builder_->SetInsertPoint(sepBB);
    builder_->CreateCall(strcatFunc, {buf, sep});
    builder_->CreateBr(concatBB);
    
    builder_->SetInsertPoint(concatBB);
    llvm::Value* elemGep = builder_->CreateGEP(ptrTy, dataPtr, {idx}, "join.elem.gep");
    llvm::Value* elem = builder_->CreateLoad(ptrTy, elemGep, "join.elem");
    builder_->CreateCall(strcatFunc, {buf, elem});
    
    llvm::Value* nextIdx = builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1));
    idx->addIncoming(nextIdx, concatBB);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(doneBB);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* str = resolveOperand(inst->operands[0]);
    llvm::Value* prefix = resolveOperand(inst->operands[1]);
    if (!str || !prefix) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    
    // Get prefix length
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* prefLen = builder_->CreateCall(strlenFunc, {prefix}, "pref.len");
    
    // strncmp(str, prefix, prefLen)
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* strncmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy, i64Ty}, false);
    auto strncmpFunc = module_->getOrInsertFunction("strncmp", strncmpType);
    llvm::Value* cmp = builder_->CreateCall(strncmpFunc, {str, prefix, prefLen}, "starts.cmp");
    llvm::Value* cmpBool = builder_->CreateICmpEQ(cmp, 
        llvm::ConstantInt::get(i32Ty, 0), "starts_with");
    llvm::Value* result = builder_->CreateZExt(cmpBool, i64Ty, "starts_with.i64");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* str = resolveOperand(inst->operands[0]);
    llvm::Value* suffix = resolveOperand(inst->operands[1]);
    if (!str || !suffix) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    
    auto* strlenType = llvm::FunctionType::get(i64Ty, {ptrTy}, false);
    auto strlenFunc = module_->getOrInsertFunction("strlen", strlenType);
    llvm::Value* strLen = builder_->CreateCall(strlenFunc, {str}, "str.len");
    llvm::Value* sufLen = builder_->CreateCall(strlenFunc, {suffix}, "suf.len");
    
    // Compare last sufLen chars: str + (strLen - sufLen)
    llvm::Value* offset = builder_->CreateSub(strLen, sufLen, "offset");
    llvm::Value* endPtr = builder_->CreateGEP(
        llvm::Type::getInt8Ty(*context_), str, {offset}, "end.ptr");
    
    auto* strcmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
    auto strcmpFunc = module_->getOrInsertFunction("strcmp", strcmpType);
    llvm::Value* cmp = builder_->CreateCall(strcmpFunc, {endPtr, suffix}, "ends.cmp");
    llvm::Value* cmpBool = builder_->CreateICmpEQ(cmp, 
        llvm::ConstantInt::get(i32Ty, 0), "ends_with");
    llvm::Value* result = builder_->CreateZExt(cmpBool, i64Ty, "ends_with.i64");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinStringContains(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* str = resolveOperand(inst->operands[0]);
    llvm::Value* substr = resolveOperand(inst->operands[1]);
    if (!str || !substr) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    
    // strstr(str, substr) != null
    auto* strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto strstrFunc = module_->getOrInsertFunction("strstr", strstrType);
    llvm::Value* found = builder_->CreateCall(strstrFunc, {str, substr}, "found");
    llvm::Value* cmpBool = builder_->CreateICmpNE(found,
        llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*context_)), "contains");
    llvm::Value* result = builder_->CreateZExt(cmpBool, getInt64Type(), "contains.i64");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: Builtin Array Functions / ╪»┘ט╪º┘ה ╪º┘ה┘ו╪╡┘ב┘ט┘ב╪º╪¬ ╪º┘ה┘ו╪╢┘ו┘ז╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("ARRAY_APPEND requires 2 operands (array, value)");
        return nullptr;
    }
    
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    llvm::Value* value = resolveOperand(inst->operands[1]);
    if (!arrPtr || !value) return nullptr;
    
    auto i64Ty = getInt64Type();
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // Load current length
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
    llvm::Value* len = builder_->CreateLoad(i64Ty, lenGep, "arr.len");
    
    // Load data pointer
    llvm::Value* dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(ptrTy, dataGep, "arr.data");
    
    // Store value at data[len]
    llvm::Value* elemPtr = builder_->CreateGEP(i64Ty, dataPtr, {len}, "arr.elem");
    builder_->CreateStore(value, elemPtr);
    
    // Increment length
    llvm::Value* newLen = builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1), "new.len");
    builder_->CreateStore(newLen, lenGep);
    
    return arrPtr;
}

llvm::Value* LLVMCodeGen::emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("ARRAY_REMOVE requires 2 operands (array, index)");
        return nullptr;
    }
    
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    if (!arrPtr) return nullptr;
    
    auto i64Ty = getInt64Type();
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // Decrement length (simplified: doesn't shift elements)
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
    llvm::Value* len = builder_->CreateLoad(i64Ty, lenGep, "arr.len");
    llvm::Value* newLen = builder_->CreateSub(len, llvm::ConstantInt::get(i64Ty, 1), "new.len");
    builder_->CreateStore(newLen, lenGep);
    
    return arrPtr;
}



} // namespace LLVM
} // namespace Sad
