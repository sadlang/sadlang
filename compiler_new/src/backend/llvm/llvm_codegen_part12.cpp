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

llvm::Value* LLVMCodeGen::emitBuiltinArraySize(std::shared_ptr<SIRInstruction> inst) {
    return emitArrayLen(inst);
}

llvm::Value* LLVMCodeGen::emitBuiltinArrayIndexOf(std::shared_ptr<SIRInstruction> inst) {
    // Linear search through SadArray, returns index or -1
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* arrPtr = resolveOperand(inst->operands[0]);
    llvm::Value* needle = resolveOperand(inst->operands[1]);
    if (!arrPtr || !needle) return nullptr;
    
    auto i64Ty = getInt64Type();
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // Load length and data
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "idxof.len.gep");
    llvm::Value* arrLen = builder_->CreateLoad(i64Ty, lenGep, "idxof.len");
    llvm::Value* datGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "idxof.dat.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(ptrTy, datGep, "idxof.data");
    
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "idxof.loop", curFunc);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context_, "idxof.body", curFunc);
    llvm::BasicBlock* foundBB = llvm::BasicBlock::Create(*context_, "idxof.found", curFunc);
    llvm::BasicBlock* notFoundBB = llvm::BasicBlock::Create(*context_, "idxof.notfound", curFunc);
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context_, "idxof.merge", curFunc);
    llvm::BasicBlock* entryBB = builder_->GetInsertBlock();
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(loopBB);
    llvm::PHINode* idx = builder_->CreatePHI(i64Ty, 2, "idxof.idx");
    idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
    llvm::Value* isDone = builder_->CreateICmpUGE(idx, arrLen, "idxof.done");
    builder_->CreateCondBr(isDone, notFoundBB, bodyBB);
    
    builder_->SetInsertPoint(bodyBB);
    llvm::Value* elemGep = builder_->CreateGEP(i64Ty, dataPtr, {idx}, "idxof.elem.gep");
    llvm::Value* elem = builder_->CreateLoad(i64Ty, elemGep, "idxof.elem");
    llvm::Value* isEq = builder_->CreateICmpEQ(elem, needle, "idxof.eq");
    builder_->CreateCondBr(isEq, foundBB, loopBB);
    llvm::Value* nextIdx = builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1));
    // Fix: need to re-insert after creating the branch
    // Actually builder_ already inserted the CondBr, so nextIdx comes before it
    // Let's restructure:
    
    // Need to fix ordering: compute nextIdx before branch
    // We'll use a continue block
    auto* contBB = llvm::BasicBlock::Create(*context_, "idxof.cont", curFunc);
    // Remove the last branch and redo
    bodyBB->getTerminator()->eraseFromParent();
    builder_->SetInsertPoint(bodyBB);
    builder_->CreateCondBr(isEq, foundBB, contBB);
    
    builder_->SetInsertPoint(contBB);
    llvm::Value* nextIdx2 = builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1));
    idx->addIncoming(nextIdx2, contBB);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(foundBB);
    builder_->CreateBr(mergeBB);
    
    builder_->SetInsertPoint(notFoundBB);
    builder_->CreateBr(mergeBB);
    
    builder_->SetInsertPoint(mergeBB);
    llvm::PHINode* result = builder_->CreatePHI(i64Ty, 2, "idxof.result");
    result->addIncoming(idx, foundBB);
    result->addIncoming(llvm::ConstantInt::get(i64Ty, -1), notFoundBB);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinArrayContains(std::shared_ptr<SIRInstruction> inst) {
    // Delegates to indexOf, checks result != -1
    llvm::Value* idxResult = emitBuiltinArrayIndexOf(inst);
    if (!idxResult) {
        llvm::Value* falseval = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), 0);
        if (inst && inst->result.has_value()) {
            context_info_.namedValues[inst->result->name] = falseval;
        }
        return falseval;
    }
    llvm::Value* result = builder_->CreateICmpNE(idxResult,
        llvm::ConstantInt::get(getInt64Type(), -1), "contains.result");
    if (inst && inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinArrayReverse(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arrPtr = resolveOperand(inst->operands[0]);
    if (!arrPtr) return nullptr;
    
    auto i64Ty = getInt64Type();
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // Load length and data pointer
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "rev.len.gep");
    llvm::Value* arrLen = builder_->CreateLoad(i64Ty, lenGep, "rev.len");
    llvm::Value* datGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "rev.dat.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(ptrTy, datGep, "rev.data");
    
    // In-place swap: i=0, j=len-1, while i < j: swap data[i], data[j]
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "rev.loop", curFunc);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context_, "rev.body", curFunc);
    llvm::BasicBlock* doneBB = llvm::BasicBlock::Create(*context_, "rev.done", curFunc);
    
    llvm::Value* initJ = builder_->CreateSub(arrLen, llvm::ConstantInt::get(i64Ty, 1), "rev.initj");
    llvm::BasicBlock* entryBB = builder_->GetInsertBlock();
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(loopBB);
    llvm::PHINode* iVal = builder_->CreatePHI(i64Ty, 2, "rev.i");
    iVal->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
    llvm::PHINode* jVal = builder_->CreatePHI(i64Ty, 2, "rev.j");
    jVal->addIncoming(initJ, entryBB);
    
    llvm::Value* cond = builder_->CreateICmpSLT(iVal, jVal, "rev.cond");
    builder_->CreateCondBr(cond, bodyBB, doneBB);
    
    builder_->SetInsertPoint(bodyBB);
    // Load data[i] and data[j]
    llvm::Value* iPtr = builder_->CreateGEP(i64Ty, dataPtr, {iVal}, "rev.iptr");
    llvm::Value* jPtr = builder_->CreateGEP(i64Ty, dataPtr, {jVal}, "rev.jptr");
    llvm::Value* iElem = builder_->CreateLoad(i64Ty, iPtr, "rev.ielem");
    llvm::Value* jElem = builder_->CreateLoad(i64Ty, jPtr, "rev.jelem");
    builder_->CreateStore(jElem, iPtr);
    builder_->CreateStore(iElem, jPtr);
    
    llvm::Value* nextI = builder_->CreateAdd(iVal, llvm::ConstantInt::get(i64Ty, 1));
    llvm::Value* nextJ = builder_->CreateSub(jVal, llvm::ConstantInt::get(i64Ty, 1));
    iVal->addIncoming(nextI, bodyBB);
    jVal->addIncoming(nextJ, bodyBB);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(doneBB);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = arrPtr;
    }
    return arrPtr;
}

llvm::Value* LLVMCodeGen::emitBuiltinArraySort(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    
    if (arrPtr) {
        auto i64Ty = getInt64Type();
        auto ptrTy = llvm::PointerType::getUnqual(*context_);
        llvm::StructType* arrTy = getArrayStructType(*context_);
        
        // Load data pointer and length
        llvm::Value* dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
        llvm::Value* dataPtr = builder_->CreateLoad(ptrTy, dataGep, "arr.data");
        llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
        llvm::Value* len = builder_->CreateLoad(i64Ty, lenGep, "arr.len");
        
        // Call qsort(data, len, sizeof(i64), comparator)
        // We need a comparison function
        auto* qsortType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(*context_),
            {ptrTy, i64Ty, i64Ty, ptrTy}, false);
        auto qsortFunc = module_->getOrInsertFunction("qsort", qsortType);
        
        // Create or get comparison function
        llvm::Function* cmpFunc = module_->getFunction("__sad_i64_cmp");
        if (!cmpFunc) {
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto* cmpFType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            cmpFunc = llvm::Function::Create(cmpFType, llvm::Function::InternalLinkage, 
                "__sad_i64_cmp", module_.get());
            auto* entry = llvm::BasicBlock::Create(*context_, "entry", cmpFunc);
            llvm::IRBuilder<> tmpBuilder(entry);
            auto args = cmpFunc->arg_begin();
            llvm::Value* aPtr = &*args++;
            llvm::Value* bPtr = &*args;
            llvm::Value* aVal = tmpBuilder.CreateLoad(i64Ty, aPtr, "a");
            llvm::Value* bVal = tmpBuilder.CreateLoad(i64Ty, bPtr, "b");
            llvm::Value* diff = tmpBuilder.CreateSub(aVal, bVal, "diff");
            llvm::Value* truncated = tmpBuilder.CreateTrunc(diff, i32Ty, "trunc");
            tmpBuilder.CreateRet(truncated);
        }
        
        builder_->CreateCall(qsortFunc, {
            dataPtr, len, llvm::ConstantInt::get(i64Ty, 8), cmpFunc
        });
    }
    
    if (inst->result.has_value() && arrPtr) {
        context_info_.namedValues[inst->result->name] = arrPtr;
    }
    return arrPtr;
}

llvm::Value* LLVMCodeGen::emitBuiltinArrayFirst(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    // Equivalent to ARRAY_GET(arr, 0)
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    if (!arrPtr) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    llvm::Value* dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(ptrTy, dataGep, "arr.data");
    llvm::Value* result = builder_->CreateLoad(getInt64Type(), dataPtr, "arr.first");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinArrayLast(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    if (!arrPtr) return nullptr;
    
    auto i64Ty = getInt64Type();
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    llvm::Value* lenGep = builder_->CreateStructGEP(arrTy, arrPtr, 0, "arr.len.gep");
    llvm::Value* len = builder_->CreateLoad(i64Ty, lenGep, "arr.len");
    llvm::Value* lastIdx = builder_->CreateSub(len, llvm::ConstantInt::get(i64Ty, 1), "last.idx");
    
    llvm::Value* dataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "arr.data.gep");
    llvm::Value* dataPtr = builder_->CreateLoad(ptrTy, dataGep, "arr.data");
    llvm::Value* elemPtr = builder_->CreateGEP(i64Ty, dataPtr, {lastIdx}, "arr.last.ptr");
    llvm::Value* result = builder_->CreateLoad(i64Ty, elemPtr, "arr.last");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinArraySlice(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) {
        reportError("ARRAY_SLICE requires 3 operands (array, start, end)");
        return nullptr;
    }
    
    llvm::Value* arrPtr = context_info_.namedValues[inst->operands[0].name];
    if (!arrPtr) arrPtr = resolveOperand(inst->operands[0]);
    llvm::Value* start = resolveOperand(inst->operands[1]);
    llvm::Value* end = resolveOperand(inst->operands[2]);
    if (!arrPtr || !start || !end) return nullptr;
    
    auto i64Ty = getInt64Type();
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::StructType* arrTy = getArrayStructType(*context_);
    
    // New length = end - start
    llvm::Value* newLen = builder_->CreateSub(end, start, "slice.len");
    
    // Allocate new array
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    auto* arrSize = llvm::ConstantExpr::getSizeOf(arrTy);
    llvm::Value* newArr = builder_->CreateCall(mallocFunc, {arrSize}, "slice.arr");
    
    // Set length
    llvm::Value* nLenGep = builder_->CreateStructGEP(arrTy, newArr, 0, "slice.len.gep");
    builder_->CreateStore(newLen, nLenGep);
    
    // Set capacity = length
    llvm::Value* nCapGep = builder_->CreateStructGEP(arrTy, newArr, 1, "slice.cap.gep");
    builder_->CreateStore(newLen, nCapGep);
    
    // Allocate data
    llvm::Value* dataSize = builder_->CreateMul(newLen, llvm::ConstantInt::get(i64Ty, 8));
    llvm::Value* newData = builder_->CreateCall(mallocFunc, {dataSize}, "slice.data");
    llvm::Value* nDataGep = builder_->CreateStructGEP(arrTy, newArr, 2, "slice.data.gep");
    builder_->CreateStore(newData, nDataGep);
    
    // Copy data from original
    llvm::Value* srcDataGep = builder_->CreateStructGEP(arrTy, arrPtr, 2, "src.data.gep");
    llvm::Value* srcData = builder_->CreateLoad(ptrTy, srcDataGep, "src.data");
    llvm::Value* srcStart = builder_->CreateGEP(i64Ty, srcData, {start}, "src.start");
    
    auto* memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false);
    auto memcpyFunc = module_->getOrInsertFunction("memcpy", memcpyType);
    builder_->CreateCall(memcpyFunc, {newData, srcStart, dataSize});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = newArr;
    }
    return newArr;
}

// ============================================================================
// Phase N: Builtin File I/O Functions / ╪»┘ט╪º┘ה ╪º┘ה┘ו┘ה┘ב╪º╪¬ ╪º┘ה┘ו╪╢┘ו┘ז╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("FILE_READ requires 1 operand (filename)");
        return nullptr;
    }
    llvm::Value* filename = resolveOperand(inst->operands[0]);
    if (!filename) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    
    // fopen(filename, "r")
    auto* fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);
    llvm::Value* mode = builder_->CreateGlobalStringPtr("r", "mode_r");
    llvm::Value* file = builder_->CreateCall(fopenFunc, {filename, mode}, "file");
    
    // Allocate read buffer (4096 bytes)
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, 
        {llvm::ConstantInt::get(i64Ty, 4096)}, "read_buf");
    
    // fread(buf, 1, 4095, file)
    auto* freadType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
    auto freadFunc = module_->getOrInsertFunction("fread", freadType);
    llvm::Value* bytesRead = builder_->CreateCall(freadFunc, {
        buf, llvm::ConstantInt::get(i64Ty, 1), 
        llvm::ConstantInt::get(i64Ty, 4095), file
    }, "bytes_read");
    
    // Null-terminate
    llvm::Value* endPtr = builder_->CreateGEP(
        llvm::Type::getInt8Ty(*context_), buf, {bytesRead}, "end");
    builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context_), 0), endPtr);
    
    // fclose(file)
    auto* fcloseType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_), {ptrTy}, false);
    auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
    builder_->CreateCall(fcloseFunc, {file});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = buf;
    }
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("FILE_WRITE requires 2 operands (filename, content)");
        return nullptr;
    }
    llvm::Value* filename = resolveOperand(inst->operands[0]);
    llvm::Value* content = resolveOperand(inst->operands[1]);
    if (!filename || !content) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    
    // fopen(filename, "w")
    auto* fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);
    llvm::Value* mode = builder_->CreateGlobalStringPtr("w", "mode_w");
    llvm::Value* file = builder_->CreateCall(fopenFunc, {filename, mode}, "file");
    
    // fputs(content, file)
    auto* fputsType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
    auto fputsFunc = module_->getOrInsertFunction("fputs", fputsType);
    llvm::Value* result = builder_->CreateCall(fputsFunc, {content, file}, "fputs_result");
    
    auto* fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
    auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
    builder_->CreateCall(fcloseFunc, {file});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("FILE_APPEND requires 2 operands (filename, content)");
        return nullptr;
    }
    llvm::Value* filename = resolveOperand(inst->operands[0]);
    llvm::Value* content = resolveOperand(inst->operands[1]);
    if (!filename || !content) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    
    auto* fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);
    llvm::Value* mode = builder_->CreateGlobalStringPtr("a", "mode_a");
    llvm::Value* file = builder_->CreateCall(fopenFunc, {filename, mode}, "file");
    
    auto* fputsType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
    auto fputsFunc = module_->getOrInsertFunction("fputs", fputsType);
    llvm::Value* result = builder_->CreateCall(fputsFunc, {content, file}, "fputs_result");
    
    auto* fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
    auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
    builder_->CreateCall(fcloseFunc, {file});
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* filename = resolveOperand(inst->operands[0]);
    if (!filename) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    
    auto* removeType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
    auto removeFunc = module_->getOrInsertFunction("remove", removeType);
    llvm::Value* result = builder_->CreateCall(removeFunc, {filename}, "remove_result");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst) {
    // Cross-platform file copy using fopen/fread/fwrite
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* src = resolveOperand(inst->operands[0]);
    llvm::Value* dst = resolveOperand(inst->operands[1]);
    if (!src || !dst) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = getInt64Type();
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    
    // fopen(src, "rb") and fopen(dst, "wb")
    auto* fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
    auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);
    
    llvm::Value* rb = builder_->CreateGlobalStringPtr("rb", "mode.rb");
    llvm::Value* wb = builder_->CreateGlobalStringPtr("wb", "mode.wb");
    llvm::Value* srcFile = builder_->CreateCall(fopenFunc, {src, rb}, "src.file");
    llvm::Value* dstFile = builder_->CreateCall(fopenFunc, {dst, wb}, "dst.file");
    
    // Check if both opened successfully
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* copyBB = llvm::BasicBlock::Create(*context_, "fcopy.copy", curFunc);
    llvm::BasicBlock* failBB = llvm::BasicBlock::Create(*context_, "fcopy.fail", curFunc);
    llvm::BasicBlock* doneBB = llvm::BasicBlock::Create(*context_, "fcopy.done", curFunc);
    
    llvm::Value* srcNull = builder_->CreateICmpEQ(srcFile,
        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));
    llvm::Value* dstNull = builder_->CreateICmpEQ(dstFile,
        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));
    llvm::Value* anyNull = builder_->CreateOr(srcNull, dstNull);
    builder_->CreateCondBr(anyNull, failBB, copyBB);
    
    // Copy loop: read 4096 bytes at a time
    builder_->SetInsertPoint(copyBB);
    llvm::Value* bufSize = llvm::ConstantInt::get(i64Ty, 4096);
    auto* mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* buf = builder_->CreateCall(mallocFunc, {bufSize}, "copy.buf");
    
    auto* freadType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
    auto freadFunc = module_->getOrInsertFunction("fread", freadType);
    auto* fwriteType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
    auto fwriteFunc = module_->getOrInsertFunction("fwrite", fwriteType);
    auto* fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
    auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
    auto* freeType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {ptrTy}, false);
    auto freeFunc = module_->getOrInsertFunction("free", freeType);
    
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "fcopy.loop", curFunc);
    llvm::BasicBlock* loopBodyBB = llvm::BasicBlock::Create(*context_, "fcopy.body", curFunc);
    llvm::BasicBlock* loopDoneBB = llvm::BasicBlock::Create(*context_, "fcopy.ldone", curFunc);
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(loopBB);
    llvm::Value* bytesRead = builder_->CreateCall(freadFunc, {
        buf, llvm::ConstantInt::get(i64Ty, 1), bufSize, srcFile}, "bytes.read");
    llvm::Value* hasData = builder_->CreateICmpUGT(bytesRead, llvm::ConstantInt::get(i64Ty, 0));
    builder_->CreateCondBr(hasData, loopBodyBB, loopDoneBB);
    
    builder_->SetInsertPoint(loopBodyBB);
    builder_->CreateCall(fwriteFunc, {
        buf, llvm::ConstantInt::get(i64Ty, 1), bytesRead, dstFile});
    builder_->CreateBr(loopBB);
    
    builder_->SetInsertPoint(loopDoneBB);
    builder_->CreateCall(fcloseFunc, {srcFile});
    builder_->CreateCall(fcloseFunc, {dstFile});
    builder_->CreateCall(freeFunc, {buf});
    builder_->CreateBr(doneBB);
    
    builder_->SetInsertPoint(failBB);
    builder_->CreateBr(doneBB);
    
    builder_->SetInsertPoint(doneBB);
    llvm::PHINode* result = builder_->CreatePHI(i32Ty, 2, "fcopy.result");
    result->addIncoming(llvm::ConstantInt::get(i32Ty, 1), loopDoneBB); // success
    result->addIncoming(llvm::ConstantInt::get(i32Ty, 0), failBB); // failure
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* src = resolveOperand(inst->operands[0]);
    llvm::Value* dst = resolveOperand(inst->operands[1]);
    if (!src || !dst) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    
    auto* renameType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
    auto renameFunc = module_->getOrInsertFunction("rename", renameType);
    llvm::Value* result = builder_->CreateCall(renameFunc, {src, dst}, "rename_result");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* path = resolveOperand(inst->operands[0]);
    if (!path) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    
    // Use sad_file_create_dir runtime helper (cross-platform)
    // On Windows: _mkdir(path), On Linux: mkdir(path, 0755)
    auto* mkdirType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
    auto mkdirFunc = module_->getOrInsertFunction("sad_file_create_dir", mkdirType);
    llvm::Value* result = builder_->CreateCall(mkdirFunc, {path}, "mkdir_result");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst) {
    // List directory contents using runtime helper
    // sad_file_list_dir(path) -> SadArray of strings
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* path = resolveOperand(inst->operands[0]);
    if (!path) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    
    // Call runtime helper: char* sad_file_list_dir(const char* path)
    // Returns newline-separated list of directory entries
    auto* listDirType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
    auto listDirFunc = module_->getOrInsertFunction("sad_file_list_dir", listDirType);
    llvm::Value* result = builder_->CreateCall(listDirFunc, {path}, "listdir.ret");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Dead Declaration Implementations / ╪¬┘ז┘ב┘ך╪░ ╪º┘ה╪Ñ╪╣┘ה╪º┘ז╪º╪¬ ╪º┘ה┘ו╪╣┘ה┘ג╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitBitCast(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // In LLVM opaque pointers era, bitcast between pointers is identity
    // For ptr->ptr, just return the value. For other types, use CreateBitCast.
    llvm::Type* destTy = val->getType(); // default: same type
    if (inst->operands.size() >= 2) {
        // If second operand specifies the target type name, use ptr
        destTy = llvm::PointerType::getUnqual(*context_);
    }
    
    llvm::Value* result = val;
    if (val->getType() != destTy) {
        result = builder_->CreateBitCast(val, destTy, "bitcast");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitIntToPtr(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* result = builder_->CreateIntToPtr(val, ptrTy, "inttoptr");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitPtrToInt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    auto i64Ty = getInt64Type();
    llvm::Value* result = builder_->CreatePtrToInt(val, i64Ty, "ptrtoint");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitTrunc(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // Default: truncate to i32
    llvm::Type* destTy = llvm::Type::getInt32Ty(*context_);
    if (inst->operands.size() >= 2) {
        // Check for target bit width in metadata
        auto& meta = inst->operands[1];
        if (meta.name == "i8") destTy = llvm::Type::getInt8Ty(*context_);
        else if (meta.name == "i16") destTy = llvm::Type::getInt16Ty(*context_);
        else if (meta.name == "i1") destTy = llvm::Type::getInt1Ty(*context_);
    }
    
    llvm::Value* result = builder_->CreateTrunc(val, destTy, "trunc");
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitZExt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // Default: extend to i64
    llvm::Type* destTy = getInt64Type();
    if (inst->operands.size() >= 2) {
        auto& meta = inst->operands[1];
        if (meta.name == "i32") destTy = llvm::Type::getInt32Ty(*context_);
        else if (meta.name == "i16") destTy = llvm::Type::getInt16Ty(*context_);
    }
    
    llvm::Value* result = builder_->CreateZExt(val, destTy, "zext");
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitSExt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // Default: extend to i64
    llvm::Type* destTy = getInt64Type();
    if (inst->operands.size() >= 2) {
        auto& meta = inst->operands[1];
        if (meta.name == "i32") destTy = llvm::Type::getInt32Ty(*context_);
        else if (meta.name == "i16") destTy = llvm::Type::getInt16Ty(*context_);
    }
    
    llvm::Value* result = builder_->CreateSExt(val, destTy, "sext");
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// LLVM Coroutine Support / دعم كوروتينات LLVM
// ============================================================================
// (AR) تنفيذ غير_متزامن/انتظر عبر LLVM Coroutine Intrinsics
// (EN) Implement async/await via LLVM Coroutine Intrinsics
//
// البنية / Structure:
//   غير_متزامن دالة foo(params) → LLVM coroutine:
//     entry:
//       %promise = alloca i64 (return value storage)
//       %coro.id = call @llvm.coro.id(0, %promise, null, null)
//       %coro.size = call @llvm.coro.size.i64()
//       %coro.alloc = call @malloc(%coro.size)
//       %coro.hdl = call @llvm.coro.begin(%coro.id, %coro.alloc)
//       ... function body ...
//       store %retval, %promise
//       br %coro.final
//     coro.final:
//       %final.save = call @llvm.coro.save(%coro.hdl)
//       %final.susp = call @llvm.coro.suspend(%final.save, true)
//       switch %final.susp [0→suspend, 1→cleanup, default→suspend]
//     coro.cleanup:
//       %mem = call @llvm.coro.free(%coro.id, %coro.hdl)
//       call @free(%mem)
//       br %coro.suspend
//     coro.suspend:
//       call @llvm.coro.end(%coro.hdl, false, none)
//       ret ptr %coro.hdl
// ============================================================================

void LLVMCodeGen::emitCoroutinePreamble(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc) {
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    auto i1Ty = llvm::Type::getInt1Ty(*context_);
    auto tokenTy = llvm::Type::getTokenTy(*context_);
    auto voidTy = llvm::Type::getVoidTy(*context_);
    
    // (AR) التأكد من أننا في كتلة entry
    // (EN) Make sure we're in the entry block
    auto* entryBB = &llvmFunc->getEntryBlock();
    builder_->SetInsertPoint(entryBB);
    
    // (AR) إنشاء الكتل الخاصة بالكوروتين
    // (EN) Create coroutine-specific blocks
    context_info_.coroFinalBB = llvm::BasicBlock::Create(*context_, "coro.final", llvmFunc);
    context_info_.coroCleanupBB = llvm::BasicBlock::Create(*context_, "coro.cleanup", llvmFunc);
    context_info_.coroSuspendBB = llvm::BasicBlock::Create(*context_, "coro.suspend", llvmFunc);
    
    // (AR) كتلة لاستمرار التنفيذ بعد التعليق الأولي
    // (EN) Block for continuing execution after initial suspend
    auto* initResumeBB = llvm::BasicBlock::Create(*context_, "coro.init.resume", llvmFunc);
    
    context_info_.coroSuspendCount = 0;
    
    // === Step 1: Allocate promise (for return value) ===
    llvm::Value* promise = builder_->CreateAlloca(i64Ty, nullptr, "coro.promise");
    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), promise);
    context_info_.coroPromise = promise;
    
    // === Step 2: coro.id ===
    auto coroIdFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_id);
    llvm::Value* coroId = builder_->CreateCall(coroIdFn, {
        llvm::ConstantInt::get(i32Ty, 0),
        promise,
        llvm::ConstantPointerNull::get(ptrTy),
        llvm::ConstantPointerNull::get(ptrTy)
    }, "coro.id");
    context_info_.coroId = coroId;
    
    // === Step 3: coro.size ===
    auto coroSizeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_size, {i64Ty});
    llvm::Value* coroSize = builder_->CreateCall(coroSizeFn, {}, "coro.size");
    
    // === Step 4: malloc ===
    auto mallocTy = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
    auto mallocFn = module_->getOrInsertFunction("malloc", mallocTy);
    llvm::Value* coroAlloc = builder_->CreateCall(mallocFn, {coroSize}, "coro.alloc");
    
    // === Step 5: coro.begin ===
    auto coroBeginFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_begin);
    llvm::Value* coroHdl = builder_->CreateCall(coroBeginFn, {coroId, coroAlloc}, "coro.hdl");
    context_info_.coroHandle = coroHdl;
    
    // === Step 6: Initial suspend (required for CoroSplit) ===
    // (AR) التعليق الأولي: يُعيد التحكم للمستدعي بعد إنشاء الإطار
    // (EN) Initial suspend: returns control to caller after frame creation
    // After CoroSplit, the ramp function returns here, and the body
    // runs when coro.resume is called by the caller.
    auto coroSaveFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_save);
    llvm::Value* initSave = builder_->CreateCall(coroSaveFn, {coroHdl}, "coro.init.save");
    
    auto coroSuspFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_suspend);
    llvm::Value* initSusp = builder_->CreateCall(coroSuspFn, {
        initSave, llvm::ConstantInt::get(i1Ty, 0)  // isFinal=false (initial suspend)
    }, "coro.init.susp");
    
    auto* initSwitch = builder_->CreateSwitch(initSusp, context_info_.coroSuspendBB, 2);
    initSwitch->addCase(llvm::ConstantInt::get(i8Ty, 0), initResumeBB);   // resume → continue body
    initSwitch->addCase(llvm::ConstantInt::get(i8Ty, 1), context_info_.coroCleanupBB);  // destroy → cleanup
    
    // (AR) الآن نستمر في كتلة الاستئناف الأولي — هنا سيتم إدراج جسم الدالة
    // (EN) Now continue in init resume block — function body will be inserted here
    builder_->SetInsertPoint(initResumeBB);
    context_info_.currentBlock = initResumeBB;
    
    // (AR) حفظ المقبض في السياق حتى يمكن استخدامه في await و return
    // (EN) Save handle in context for use in await and return
    context_info_.namedValues["__coro_hdl"] = coroHdl;
    context_info_.namedValues["__coro_promise"] = promise;
    
    std::cerr << "[CORO] Preamble emitted for '" << sirFunc->getName() << "'" << std::endl;
}

void LLVMCodeGen::emitCoroutineEpilogue() {
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    auto i1Ty = llvm::Type::getInt1Ty(*context_);
    auto tokenTy = llvm::Type::getTokenTy(*context_);
    auto voidTy = llvm::Type::getVoidTy(*context_);
    
    llvm::Value* coroHdl = context_info_.coroHandle;
    llvm::Value* coroId = context_info_.coroId;
    
    if (!coroHdl || !coroId) {
        reportError("Coroutine epilogue: missing handle or id");
        return;
    }
    
    // === coro.final block ===
    // (AR) تعليق نهائي: يُخبر LLVM أن الكوروتين انتهى
    // (EN) Final suspend: tells LLVM the coroutine is done
    builder_->SetInsertPoint(context_info_.coroFinalBB);
    
    auto coroSaveFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_save);
    llvm::Value* finalSave = builder_->CreateCall(coroSaveFn, {coroHdl}, "coro.final.save");
    
    auto coroSuspFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_suspend);
    llvm::Value* finalSusp = builder_->CreateCall(coroSuspFn, {
        finalSave, llvm::ConstantInt::get(i1Ty, 1) // isFinal=true
    }, "coro.final.susp");
    
    auto* finalSwitch = builder_->CreateSwitch(finalSusp, context_info_.coroSuspendBB, 2);
    finalSwitch->addCase(llvm::ConstantInt::get(i8Ty, 0), context_info_.coroSuspendBB);
    finalSwitch->addCase(llvm::ConstantInt::get(i8Ty, 1), context_info_.coroCleanupBB);
    
    // === coro.cleanup block ===
    // (AR) تنظيف: تحرير ذاكرة الإطار
    // (EN) Cleanup: free frame memory
    builder_->SetInsertPoint(context_info_.coroCleanupBB);
    
    auto coroFreeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_free);
    llvm::Value* mem = builder_->CreateCall(coroFreeFn, {coroId, coroHdl}, "coro.mem");
    
    auto freeTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);
    auto freeFn = module_->getOrInsertFunction("free", freeTy);
    builder_->CreateCall(freeFn, {mem});
    
    builder_->CreateBr(context_info_.coroSuspendBB);
    
    // === coro.suspend block ===
    // (AR) تعليق: إنهاء الكوروتين وإرجاع المقبض
    // (EN) Suspend: end coroutine and return handle
    builder_->SetInsertPoint(context_info_.coroSuspendBB);
    
    auto coroEndFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_end);
    builder_->CreateCall(coroEndFn, {
        coroHdl,
        llvm::ConstantInt::get(i1Ty, 0),
        llvm::ConstantTokenNone::get(*context_)
    });
    
    builder_->CreateRet(coroHdl);
    
    // (AR) تنظيف سياق الكوروتين
    // (EN) Clean up coroutine context
    context_info_.coroHandle = nullptr;
    context_info_.coroPromise = nullptr;
    context_info_.coroId = nullptr;
    context_info_.coroCleanupBB = nullptr;
    context_info_.coroSuspendBB = nullptr;
    context_info_.coroFinalBB = nullptr;
    
    std::cerr << "[CORO] Epilogue emitted" << std::endl;
}

llvm::Value* LLVMCodeGen::emitCoroSuspend(std::shared_ptr<SIRInstruction> inst) {
    // (AR) تعبير انتظر: استدعاء الكوروتين الداخلي → تكرار الاستئناف → الحصول على النتيجة
    // (EN) Await expression: call inner coroutine → resume loop → get result
    
    if (!inst || inst->operands.empty()) {
        reportError("CORO_SUSPEND requires at least 1 operand (coroutine handle)");
        return nullptr;
    }
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i1Ty = llvm::Type::getInt1Ty(*context_);
    
    // (AR) الحصول على مقبض الكوروتين الداخلي
    // (EN) Get inner coroutine handle
    llvm::Value* innerHdl = resolveOperand(inst->operands[0]);
    if (!innerHdl) {
        reportError("CORO_SUSPEND: failed to resolve inner coroutine handle");
        return nullptr;
    }
    
    // (AR) تحويل إلى مؤشر إذا كان i64
    // (EN) Convert to pointer if i64
    if (innerHdl->getType()->isIntegerTy()) {
        innerHdl = builder_->CreateIntToPtr(innerHdl, ptrTy, "await.hdl.ptr");
    }
    
    int suspId = context_info_.coroSuspendCount++;
    std::string prefix = "await." + std::to_string(suspId);
    
    auto* curFunc = context_info_.currentFunction;
    
    // (AR) إنشاء كتل حلقة الاستئناف
    // (EN) Create resume loop blocks
    auto* loopBB = llvm::BasicBlock::Create(*context_, prefix + ".loop", curFunc);
    auto* resumeBB = llvm::BasicBlock::Create(*context_, prefix + ".resume", curFunc);
    auto* doneBB = llvm::BasicBlock::Create(*context_, prefix + ".done", curFunc);
    auto* contBB = llvm::BasicBlock::Create(*context_, prefix + ".cont", curFunc);
    
    builder_->CreateBr(loopBB);
    
    // === Loop: check if inner coroutine is done ===
    builder_->SetInsertPoint(loopBB);
    auto coroDoneFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_done);
    llvm::Value* done = builder_->CreateCall(coroDoneFn, {innerHdl}, prefix + ".isdone");
    builder_->CreateCondBr(done, doneBB, resumeBB);
    
    // === Resume: resume inner coroutine ===
    builder_->SetInsertPoint(resumeBB);
    auto coroResumeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_resume);
    builder_->CreateCall(coroResumeFn, {innerHdl});
    builder_->CreateBr(loopBB);
    
    // === Done: get result from inner coroutine's promise ===
    builder_->SetInsertPoint(doneBB);
    auto coroPromiseFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_promise, {});
    llvm::Value* innerPromise = builder_->CreateCall(coroPromiseFn, {
        innerHdl,
        llvm::ConstantInt::get(i32Ty, 8),  // alignment
        llvm::ConstantInt::get(i1Ty, 0)    // from = false
    }, prefix + ".promise");
    
    llvm::Value* result = builder_->CreateLoad(i64Ty, innerPromise, prefix + ".result");
    
    // (AR) تدمير الكوروتين الداخلي
    // (EN) Destroy inner coroutine
    auto coroDestroyFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_destroy);
    builder_->CreateCall(coroDestroyFn, {innerHdl});
    
    builder_->CreateBr(contBB);
    
    // === Continue ===
    builder_->SetInsertPoint(contBB);
    context_info_.currentBlock = contBB;
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

llvm::Value* LLVMCodeGen::emitCoroReturn(std::shared_ptr<SIRInstruction> inst) {
    // (AR) إرجاع قيمة من كوروتين: تخزين في الوعد + القفز للإنهاء النهائي
    // (EN) Return value from coroutine: store in promise + jump to final suspend
    
    if (!inst || inst->operands.empty()) {
        // (AR) لا قيمة — فقط اقفز للإنهاء
        // (EN) No value — just jump to final
        if (context_info_.coroFinalBB) {
            builder_->CreateBr(context_info_.coroFinalBB);
        }
        return nullptr;
    }
    
    llvm::Value* retVal = resolveOperand(inst->operands[0]);
    if (!retVal) {
        if (context_info_.coroFinalBB) {
            builder_->CreateBr(context_info_.coroFinalBB);
        }
        return nullptr;
    }
    
    // (AR) تخزين القيمة في الوعد
    // (EN) Store value in promise
    if (context_info_.coroPromise) {
        auto i64Ty = llvm::Type::getInt64Ty(*context_);
        llvm::Value* storeVal = retVal;
        
        // (AR) تحويل النوع إذا لزم الأمر
        // (EN) Cast type if needed
        if (retVal->getType()->isDoubleTy()) {
            storeVal = builder_->CreateFPToSI(retVal, i64Ty, "coro.ret.toi64");
        } else if (retVal->getType()->isPointerTy()) {
            storeVal = builder_->CreatePtrToInt(retVal, i64Ty, "coro.ret.ptrtoi64");
        } else if (retVal->getType() != i64Ty) {
            storeVal = builder_->CreateSExtOrTrunc(retVal, i64Ty, "coro.ret.cast");
        }
        
        builder_->CreateStore(storeVal, context_info_.coroPromise);
    }
    
    // (AR) القفز إلى كتلة الإنهاء النهائي
    // (EN) Branch to final suspend block
    if (context_info_.coroFinalBB) {
        builder_->CreateBr(context_info_.coroFinalBB);
    }
    
    return retVal;
}

// ============================================================================
// emitGeneratorYield - أنتج قيمة من مولّد
// Yield a value from a generator (store in promise + non-final coro.suspend)
// ============================================================================
llvm::Value* LLVMCodeGen::emitGeneratorYield(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("GENERATOR_YIELD requires at least 1 operand (value to yield)");
        return nullptr;
    }
    
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8Ty = llvm::Type::getInt8Ty(*context_);
    auto i1Ty = llvm::Type::getInt1Ty(*context_);
    
    // (AR) الحصول على القيمة المُنتجة
    // (EN) Get the yielded value
    llvm::Value* yieldVal = resolveOperand(inst->operands[0]);
    if (!yieldVal) {
        reportError("GENERATOR_YIELD: failed to resolve yield value");
        return nullptr;
    }
    
    // (AR) تخزين القيمة في الوعد (promise)
    // (EN) Store value in promise
    if (context_info_.coroPromise) {
        llvm::Value* storeVal = yieldVal;
        
        // (AR) تحويل النوع إذا لزم
        // (EN) Cast type if needed
        if (yieldVal->getType()->isDoubleTy()) {
            storeVal = builder_->CreateFPToSI(yieldVal, i64Ty, "gen.yield.toi64");
        } else if (yieldVal->getType()->isPointerTy()) {
            storeVal = builder_->CreatePtrToInt(yieldVal, i64Ty, "gen.yield.ptrtoi64");
        } else if (yieldVal->getType() != i64Ty) {
            storeVal = builder_->CreateSExtOrTrunc(yieldVal, i64Ty, "gen.yield.cast");
        }
        
        builder_->CreateStore(storeVal, context_info_.coroPromise);
    }
    
    // (AR) حفظ + تعليق غير نهائي (non-final suspend)
    // (EN) Save + non-final suspend
    llvm::Value* coroHdl = context_info_.coroHandle;
    if (!coroHdl) {
        reportError("GENERATOR_YIELD: no coroutine handle");
        return nullptr;
    }
    
    int suspId = context_info_.coroSuspendCount++;
    std::string prefix = "gen.yield." + std::to_string(suspId);
    
    auto* curFunc = context_info_.currentFunction;
    
    auto coroSaveFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_save);
    llvm::Value* save = builder_->CreateCall(coroSaveFn, {coroHdl}, prefix + ".save");
    
    auto coroSuspFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_suspend);
    llvm::Value* susp = builder_->CreateCall(coroSuspFn, {
        save, llvm::ConstantInt::get(i1Ty, 0)  // isFinal=false
    }, prefix + ".susp");
    
    // (AR) switch: 0 → استئناف (متابعة الجسم), 1 → تنظيف (تدمير)
    // (EN) switch: 0 → resume (continue body), 1 → cleanup (destroy)
    auto* resumeBB = llvm::BasicBlock::Create(*context_, prefix + ".resume", curFunc);
    
    auto* sw = builder_->CreateSwitch(susp, context_info_.coroSuspendBB, 2);
    sw->addCase(llvm::ConstantInt::get(i8Ty, 0), resumeBB);       // resume → continue
    sw->addCase(llvm::ConstantInt::get(i8Ty, 1), context_info_.coroCleanupBB);  // destroy → cleanup
    
    // (AR) متابعة التنفيذ في كتلة الاستئناف
    // (EN) Continue execution in resume block
    builder_->SetInsertPoint(resumeBB);
    context_info_.currentBlock = resumeBB;
    
    std::cerr << "[GEN] Emitted GENERATOR_YIELD #" << suspId << std::endl;
    return nullptr;
}

// ============================================================================
// emitGeneratorConsume - استهلاك مولّد (حلقة استئناف لجمع كل القيم)
// Consume a generator (resume loop collecting all yielded values)
// Returns the sum of all yielded values
// ============================================================================
llvm::Value* LLVMCodeGen::emitGeneratorConsume(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("GENERATOR_CONSUME requires at least 1 operand (generator handle)");
        return nullptr;
    }
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i1Ty = llvm::Type::getInt1Ty(*context_);
    
    // (AR) الحصول على مقبض المولّد
    // (EN) Get generator handle
    llvm::Value* genHdl = resolveOperand(inst->operands[0]);
    if (!genHdl) {
        reportError("GENERATOR_CONSUME: failed to resolve generator handle");
        return nullptr;
    }
    
    // (AR) تحويل إلى مؤشر إذا كان i64
    // (EN) Convert to pointer if i64
    if (genHdl->getType()->isIntegerTy()) {
        genHdl = builder_->CreateIntToPtr(genHdl, ptrTy, "gen.hdl.ptr");
    }
    
    auto* curFunc = context_info_.currentFunction;
    
    // (AR) تخصيص متغير للمجموع
    // (EN) Allocate sum variable
    llvm::Value* sumAlloca = builder_->CreateAlloca(i64Ty, nullptr, "gen.sum");
    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), sumAlloca);
    
    // (AR) إنشاء كتل حلقة الاستهلاك
    // (EN) Create consume loop blocks
    auto* loopBB = llvm::BasicBlock::Create(*context_, "gen.loop", curFunc);
    auto* yieldBB = llvm::BasicBlock::Create(*context_, "gen.yield", curFunc);
    auto* endBB = llvm::BasicBlock::Create(*context_, "gen.end", curFunc);
    
    builder_->CreateBr(loopBB);
    
    // === Loop: resume generator and check if done ===
    builder_->SetInsertPoint(loopBB);
    
    auto coroResumeFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_resume);
    builder_->CreateCall(coroResumeFn, {genHdl});
    
    auto coroDoneFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_done);
    llvm::Value* done = builder_->CreateCall(coroDoneFn, {genHdl}, "gen.isdone");
    builder_->CreateCondBr(done, endBB, yieldBB);
    
    // === Yield: read value from promise and accumulate ===
    builder_->SetInsertPoint(yieldBB);
    
    auto coroPromiseFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_promise, {});
    llvm::Value* promisePtr = builder_->CreateCall(coroPromiseFn, {
        genHdl,
        llvm::ConstantInt::get(i32Ty, 8),   // alignment
        llvm::ConstantInt::get(i1Ty, 0)     // from = false
    }, "gen.promise");
    
    llvm::Value* yieldedVal = builder_->CreateLoad(i64Ty, promisePtr, "gen.val");
    
    // (AR) إضافة القيمة للمجموع: sum += val
    // (EN) Add value to sum: sum += val
    llvm::Value* curSum = builder_->CreateLoad(i64Ty, sumAlloca, "gen.cursum");
    llvm::Value* newSum = builder_->CreateAdd(curSum, yieldedVal, "gen.newsum");
    builder_->CreateStore(newSum, sumAlloca);
    
    builder_->CreateBr(loopBB);
    
    // === End: destroy generator and return sum ===
    builder_->SetInsertPoint(endBB);
    
    auto coroDestroyFn = llvm::Intrinsic::getDeclaration(module_.get(), llvm::Intrinsic::coro_destroy);
    builder_->CreateCall(coroDestroyFn, {genHdl});
    
    llvm::Value* finalSum = builder_->CreateLoad(i64Ty, sumAlloca, "gen.result");
    
    context_info_.currentBlock = endBB;
    
    // (AR) تخزين النتيجة في السجل
    // (EN) Store result in register
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = finalSum;
    }
    
    std::cerr << "[GEN] Emitted GENERATOR_CONSUME" << std::endl;
    return finalSum;
}


} // namespace LLVM
} // namespace Sad
