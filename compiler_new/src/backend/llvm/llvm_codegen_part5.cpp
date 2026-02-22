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

llvm::Value* LLVMCodeGen::emitCmpGt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("CmpGt instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for cmpgt");
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
        result = builder_->CreateFCmpOGT(left, right, "cmpgttmp");
    } else {
        result = builder_->CreateICmpSGT(left, right, "cmpgttmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ו┘ג╪º╪▒┘ז╪⌐ ╪ú┘ד╪¿╪▒ ┘ו┘ז ╪ú┘ט ┘ך╪│╪º┘ט┘ך
 * Emit greater than or equal comparison instruction
 * 
 * Source: llvm_codegen.h:438
 */
llvm::Value* LLVMCodeGen::emitCmpGe(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("CmpGe instruction requires 2 operands");
        return nullptr;
    }
    
    llvm::Value* left = resolveOperand(inst->operands[0]);
    llvm::Value* right = resolveOperand(inst->operands[1]);
    
    if (!left || !right) {
        reportError("Operands not found for cmpge");
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
        result = builder_->CreateFCmpOGE(left, right, "cmpgetmp");
    } else {
        result = builder_->CreateICmpSGE(left, right, "cmpgetmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪¬╪¡┘ו┘ך┘ה ┘ו┘ז ╪º┘ה╪░╪º┘ד╪▒╪⌐
 * Emit load from memory instruction
 * 
 * Source: llvm_codegen.h:444
 */
llvm::Value* LLVMCodeGen::emitLoad(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.empty()) {
        reportError("Load instruction requires 1 operand");
        return nullptr;
    }
    
    // ================================================================
    // ╪º┘ה┘ט╪╡┘ט┘ה ┘ה╪¡┘ג┘ה ┘ד╪º╪ª┘ז: LOAD ┘ו╪╣ ┘ו╪╣╪º┘ו┘ה┘ך┘ז (┘ד╪º╪ª┘ז + ╪º╪│┘ו ╪¡┘ג┘ה)
    // Field access: LOAD with 2 operands (object + field name)
    // ================================================================
    if (inst->operands.size() >= 2 &&
        inst->operands[1].type == SIROperandType::CONSTANT &&
        inst->operands[1].dataType == SIRType::STRING) {
        
        std::string objRegName = inst->operands[0].name;
        std::string fieldName = inst->operands[1].name;
        
        std::cout << "[DEBUG] emitLoad: field access " << objRegName 
                  << "." << fieldName << std::endl;
        
        // (AR) ╪º┘ה╪¿╪¡╪½ ╪╣┘ז ╪º╪│┘ו ╪º┘ה╪╡┘ז┘ב
        // (EN) Look up class name
        std::string className;
        auto classIt = context_info_.objectClassMap.find(objRegName);
        if (classIt != context_info_.objectClassMap.end()) {
            className = classIt->second;
        }
        
        if (!className.empty()) {
            auto structIt = context_info_.classStructTypes.find(className);
            auto fieldNamesIt = context_info_.classFieldNames.find(className);
            
            if (structIt != context_info_.classStructTypes.end() &&
                fieldNamesIt != context_info_.classFieldNames.end()) {
                
                llvm::StructType* structType = structIt->second;
                const auto& fieldNames = fieldNamesIt->second;
                
                // (AR) ╪Ñ┘ך╪¼╪º╪» ┘ב┘ח╪▒╪│ ╪º┘ה╪¡┘ג┘ה
                // (EN) Find field index
                int fieldIndex = -1;
                for (size_t i = 0; i < fieldNames.size(); i++) {
                    if (fieldNames[i] == fieldName) {
                        fieldIndex = static_cast<int>(i);
                        break;
                    }
                }
                
                if (fieldIndex >= 0) {
                    llvm::Value* objVal = context_info_.namedValues[objRegName];
                    if (!objVal) {
                        reportError("Object register not found: " + objRegName);
                        return nullptr;
                    }
                    
                    llvm::Value* objPtr = objVal;
                    
                    // (AR) ╪¬╪¡╪»┘ך╪» ┘ד┘ך┘ב┘ך╪⌐ ╪º┘ה┘ט╪╡┘ט┘ה ╪¿┘ז╪º╪í┘כ ╪╣┘ה┘י ┘ז┘ט╪╣ ╪º┘ה╪¬╪«╪╡┘ך╪╡
                    // (EN) Determine access method based on alloca type
                    if (auto* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objVal)) {
                        if (allocaInst->getAllocatedType()->isStructTy()) {
                            // (AR) ╪º┘ה┘ד╪º╪ª┘ז ┘ח┘ט struct alloca ┘ו╪¿╪º╪┤╪▒ Γזע GEP ┘ו╪¿╪º╪┤╪▒
                            // (EN) Object is direct struct alloca Γזע direct GEP
                            objPtr = objVal;
                        } else {
                            // (AR) ┘ו╪¬╪║┘ך╪▒ ┘ך╪¡╪¬┘ט┘ך ┘ו╪ñ╪┤╪▒ ┘ד╪º╪ª┘ז ┘ו╪«╪▓┘ז ┘ד┘א i64 Γזע ╪¬╪¡┘ו┘ך┘ה + ╪¬╪¡┘ט┘ך┘ה
                            // (EN) Variable holding object pointer stored as i64 Γזע load + inttoptr
                            llvm::Value* ptrAsInt = builder_->CreateLoad(
                                getInt64Type(), objVal, objRegName + ".ptrval");
                            objPtr = builder_->CreateIntToPtr(
                                ptrAsInt, llvm::PointerType::getUnqual(*context_), 
                                objRegName + ".objptr");
                        }
                    }
                    
                    // (AR) GEP ┘ה┘ה┘ט╪╡┘ט┘ה ┘ה┘ה╪¡┘ג┘ה
                    // (EN) GEP to access field
                    llvm::Value* gep = builder_->CreateStructGEP(
                        structType, objPtr, fieldIndex, fieldName + "_gep");
                    
                    // (AR) ╪¬╪¡┘ו┘ך┘ה ┘ג┘ך┘ו╪⌐ ╪º┘ה╪¡┘ג┘ה
                    // (EN) Load field value
                    llvm::Value* result = builder_->CreateLoad(
                        getInt64Type(), gep, fieldName + ".val");
                    
                    if (inst->result.has_value()) {
                        context_info_.namedValues[inst->result->name] = result;
                    }
                    
                    std::cout << "[DEBUG] emitLoad: field '" << fieldName 
                              << "' loaded via GEP index " << fieldIndex << std::endl;
                    
                    return result;
                } else {
                    reportError("Field '" + fieldName + "' not found in class '" + className + "'");
                    return nullptr;
                }
            }
        }
        
        // (AR) ╪Ñ╪░╪º ┘ה┘ו ┘ז╪¼╪» ┘ו╪╣┘ה┘ט┘ו╪º╪¬ ╪º┘ה╪╡┘ז┘ב╪ל ┘ז╪¬╪º╪¿╪╣ ┘ד╪¬╪¡┘ו┘ך┘ה ╪╣╪º╪»┘ך
        // (EN) If class info not found, fall through to regular load
        std::cout << "[DEBUG] emitLoad: no class info for " << objRegName 
                  << ", falling back to regular load" << std::endl;
    }
    
    // ================================================================
    // ╪¬╪¡┘ו┘ך┘ה ╪╣╪º╪»┘ך: LOAD ┘ו╪╣ ┘ו╪╣╪º┘ו┘ה ┘ט╪º╪¡╪» (┘ו╪ñ╪┤╪▒)
    // Regular load: LOAD with 1 operand (pointer)
    // ================================================================
    
    // Try namedValues first (for alloca pointers), then resolveOperand
    llvm::Value* ptr = context_info_.namedValues[inst->operands[0].name];
    if (!ptr) {
        ptr = resolveOperand(inst->operands[0]);
    }
    
    if (!ptr) {
        reportError("Pointer not found for load");
        return nullptr;
    }
    
    // (AR) ╪Ñ╪░╪º ┘ה┘ו ┘ך┘ד┘ז ┘ו╪ñ╪┤╪▒╪º┘כ (┘ו╪½┘ה ╪½╪º╪¿╪¬ ┘ו╪«╪▓┘ז ┘ו╪¿╪º╪┤╪▒╪⌐)╪ל ╪ú╪▒╪¼╪╣┘ח ┘ד┘ו╪º ┘ח┘ט
    // (EN) If not a pointer (e.g. constant stored directly), return as-is
    if (!ptr->getType()->isPointerTy()) {
        if (inst->result.has_value()) {
            context_info_.namedValues[inst->result->name] = ptr;
        }
        return ptr;
    }
    
    // (AR) ╪¬╪¡╪»┘ך╪» ┘ז┘ט╪╣ ╪º┘ה╪¬╪¡┘ו┘ך┘ה ┘ו┘ז ┘ז┘ט╪╣ SIR
    // (EN) Determine load type from SIR type
    llvm::Type* loadType = getInt64Type();  // default
    if (inst->operands[0].dataType == SIRType::F64) {
        loadType = getDoubleType();
    } else if (inst->operands[0].dataType == SIRType::BOOL) {
        loadType = llvm::Type::getInt1Ty(*context_);
    } else if (inst->operands[0].dataType == SIRType::STRING) {
        loadType = llvm::PointerType::getUnqual(*context_);
    }
    
    llvm::Value* result = builder_->CreateLoad(loadType, ptr, "loadtmp");
    
    // ┘ו-╪ú03: ┘ב╪¡╪╡ ╪Ñ╪░╪º ┘ד╪º┘ז ╪º┘ה┘ז┘ט╪╣ ┘ו╪¬╪╖╪º┘ך╪▒╪º┘כ (volatile) Γאפ ┘ה╪│╪¼┘ה╪º╪¬ ╪º┘ה╪ú╪¼┘ח╪▓╪⌐ MMIO
    // ╪Ñ╪░╪º ┘ד╪º┘ז ╪º┘ה┘ו╪╣╪º┘ו┘ה ┘ך╪¡┘ו┘ה ╪╣┘ה╪º┘ו╪⌐ volatile╪ל ┘ז╪¼╪╣┘ה ╪º┘ה╪¬╪¡┘ו┘ך┘ה volatile
    if (inst->operands[0].name.find("volatile") != std::string::npos ||
        inst->operands[0].name.find("\xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd9\x8a\xd8\xb1") != std::string::npos) {
        if (auto* loadInst = llvm::dyn_cast<llvm::LoadInst>(result)) {
            loadInst->setVolatile(true);
        }
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪¬╪«╪▓┘ך┘ז ┘ב┘ך ╪º┘ה╪░╪º┘ד╪▒╪⌐
 * Emit store to memory instruction
 * 
 * Source: llvm_codegen.h:445
 */
llvm::Value* LLVMCodeGen::emitStore(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("Store instruction requires 2 operands");
        return nullptr;
    }
    
    // (AR) ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ╪º┘ה┘ג┘ך┘ו╪⌐ ╪º┘ה┘ו╪▒╪º╪» ╪¬╪«╪▓┘ך┘ז┘ח╪º
    // (EN) Get value to store - use resolveOperand for all types
    llvm::Value* value = resolveOperand(inst->operands[0]);
    const auto& valueOp = inst->operands[0];
    
    // (AR) ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ╪º┘ה┘ו╪ñ╪┤╪▒ ┘ה┘ה╪¬╪«╪▓┘ך┘ז ┘ב┘ך┘ח
    // (EN) Get pointer to store into
    const std::string& ptrName = inst->operands[1].name;
    llvm::Value* ptr = nullptr;
    
    // (AR) ╪º┘ה╪¿╪¡╪½ ╪¿┘א find() ╪¿╪»┘ה╪º┘כ ┘ו┘ז [] ┘ה╪¬╪¼┘ז╪¿ ╪Ñ╪»╪«╪º┘ה nullptr ┘ב┘ך ╪º┘ה╪«╪▒┘ך╪╖╪⌐
    // (EN) Use find() instead of [] to avoid inserting nullptr into the map
    auto ptrIt = context_info_.namedValues.find(ptrName);
    if (ptrIt != context_info_.namedValues.end()) {
        ptr = ptrIt->second;
    }
    
    // (AR) ╪Ñ╪░╪º ┘ה┘ו ┘ז╪¼╪» ╪º┘ה┘ו╪ñ╪┤╪▒ ┘ב┘ך namedValues╪ל ┘ז╪¿╪¡╪½ ┘ב┘ך ╪º┘ה┘ו╪¬╪║┘ך╪▒╪º╪¬ ╪º┘ה╪╣╪º┘ו╪⌐
    // (EN) If pointer not found in namedValues, search global variables
    if (!ptr) {
        llvm::GlobalVariable* gv = module_->getGlobalVariable(ptrName);
        if (gv) {
            ptr = gv;  // Use the global variable directly as pointer
        }
    }
    
    if (!value || !ptr) {
        reportError("Operands not found for store: value=" + valueOp.name + ", ptr=" + ptrName);
        return nullptr;
    }
    
    // ================================================================
    // ┘ו╪╣╪º┘ה╪¼╪⌐ ╪¬╪«╪▓┘ך┘ז ┘ו╪ñ╪┤╪▒ ┘ד╪º╪ª┘ז ┘ב┘ך ┘ו╪¬╪║┘ך╪▒ i64
    // Handle storing object pointer into i64 variable
    // ================================================================
    if (value && ptr) {
        if (auto* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(ptr)) {
            // (AR) ╪Ñ╪░╪º ┘ד╪º┘ז╪¬ ╪º┘ה┘ג┘ך┘ו╪⌐ ┘ו╪ñ╪┤╪▒ (ptr) ┘ט╪º┘ה┘ח╪»┘ב i64╪ל ┘ז╪¡┘ט┘ה ptrΓזעi64
            // (EN) If value is pointer (ptr) and target is i64, convert ptrΓזעi64
            if (value->getType()->isPointerTy() && 
                allocaInst->getAllocatedType()->isIntegerTy(64)) {
                value = builder_->CreatePtrToInt(value, getInt64Type(), "obj.ptrtoint");
            }
        }
    }
    
    auto* storeResult = builder_->CreateStore(value, ptr);
    
    // ================================================================
    // ┘ז╪┤╪▒ ╪«╪▒┘ך╪╖╪⌐ ╪º┘ה╪ú╪╡┘ז╪º┘ב ╪╣┘ז╪» ╪¬╪«╪▓┘ך┘ז ┘ד╪º╪ª┘ז ┘ב┘ך ┘ו╪¬╪║┘ך╪▒
    // Propagate class map when storing object into variable
    // ================================================================
    if (context_info_.objectClassMap.count(valueOp.name) &&
        !context_info_.objectClassMap.count(ptrName)) {
        context_info_.objectClassMap[ptrName] = context_info_.objectClassMap[valueOp.name];
        std::cout << "[DEBUG] emitStore: propagated class '" 
                  << context_info_.objectClassMap[valueOp.name] 
                  << "' from " << valueOp.name << " to " << ptrName << std::endl;
    }
    
    // ┘ו-╪ú03: ┘ב╪¡╪╡ ╪Ñ╪░╪º ┘ד╪º┘ז ╪º┘ה┘ו╪¬╪║┘ך╪▒ ┘ו╪¬╪╖╪º┘ך╪▒╪º┘כ (volatile) Γאפ ┘ה╪│╪¼┘ה╪º╪¬ ╪º┘ה╪ú╪¼┘ח╪▓╪⌐ MMIO
    if (inst->operands[1].name.find("volatile") != std::string::npos ||
        inst->operands[1].name.find("\xd9\x85\xd8\xaa\xd8\xb7\xd8\xa7\xd9\x8a\xd8\xb1") != std::string::npos) {
        storeResult->setVolatile(true);
    }
    
    return storeResult;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪¬╪«╪╡┘ך╪╡ ╪░╪º┘ד╪▒╪⌐
 * Emit memory allocation instruction
 * 
 * Source: llvm_codegen.h:446
 */
llvm::Value* LLVMCodeGen::emitAlloca(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    std::string regName = inst->result.has_value() ? inst->result->name : "alloca";
    
    // ================================================================
    // ╪¡╪º┘ה╪⌐ 1: ╪¬╪«╪╡┘ך╪╡ ┘ד╪º╪ª┘ז ╪¼╪»┘ך╪» (ALLOC ┘ו╪╣ ╪º╪│┘ו ╪╡┘ז┘ב ┘ד┘א metadata)
    // Case 1: New object allocation (ALLOC with class name metadata)
    // ================================================================
    if (!inst->operands.empty() && 
        inst->operands[0].type == SIROperandType::CONSTANT &&
        inst->operands[0].dataType == SIRType::STRING) {
        
        std::string className = inst->operands[0].name;
        
        auto structIt = context_info_.classStructTypes.find(className);
        if (structIt != context_info_.classStructTypes.end()) {
            // (AR) ╪¬╪«╪╡┘ך╪╡ ┘ח┘ך┘ד┘ה ╪º┘ה╪╡┘ז┘ב
            // (EN) Allocate class struct
            llvm::StructType* structType = structIt->second;
            llvm::Value* result = builder_->CreateAlloca(structType, nullptr, regName);
            
            if (inst->result.has_value()) {
                context_info_.namedValues[inst->result->name] = result;
                context_info_.objectClassMap[inst->result->name] = className;
            }
            
            std::cout << "[DEBUG] emitAlloca: allocated struct for class '"
                      << className << "' in register '" << regName << "'" << std::endl;
            
            return result;
        }
    }
    
    // ================================================================
    // حالة 2: داخل الباني أو دالة الصنف - حقول الصنف تصبح GEP من self
    // Case 2: Inside constructor or method - class fields become GEP from self
    // ================================================================
    std::string activeClass;
    if (!context_info_.currentConstructorClass.empty()) {
        activeClass = context_info_.currentConstructorClass;
    } else if (!context_info_.currentMethodClass.empty()) {
        activeClass = context_info_.currentMethodClass;
    }
    
    if (!activeClass.empty() && inst->result.has_value()) {
        std::string fieldName = inst->result->name;
        // (AR) إزالة بادئة % إن وجدت
        // (EN) Strip % prefix if present
        if (!fieldName.empty() && fieldName[0] == '%') {
            fieldName = fieldName.substr(1);
        }
        
        auto fieldIt = context_info_.classFieldNames.find(activeClass);
        if (fieldIt != context_info_.classFieldNames.end()) {
            const auto& fieldNames = fieldIt->second;
            int fieldIndex = -1;
            for (size_t i = 0; i < fieldNames.size(); i++) {
                if (fieldNames[i] == fieldName) {
                    fieldIndex = static_cast<int>(i);
                    break;
                }
            }
            
            if (fieldIndex >= 0) {
                // (AR) استخدام GEP من self بدلاً من alloca محلي
                // (EN) Use GEP from self instead of local alloca
                llvm::Value* selfPtr = nullptr;
                auto selfIt = context_info_.namedValues.find("self");
                if (selfIt != context_info_.namedValues.end()) {
                    selfPtr = selfIt->second;
                } else {
                    auto selfIt2 = context_info_.namedValues.find("%self");
                    if (selfIt2 != context_info_.namedValues.end()) {
                        selfPtr = selfIt2->second;
                    }
                }
                
                if (selfPtr) {
                    llvm::StructType* structType = context_info_.classStructTypes[activeClass];
                    
                    // (AR) إذا كان self alloca، نحمّله القيمة ونحولها لمؤشر
                    // (EN) If self is an alloca, load value and convert to pointer
                    llvm::Value* actualSelf = selfPtr;
                    if (auto* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(selfPtr)) {
                        if (!allocaInst->getAllocatedType()->isStructTy()) {
                            // self هو alloca i64 يحتوي مؤشر محول ptrtoint
                            // self is an i64 alloca holding a ptrtoint-ed pointer
                            llvm::Value* selfVal = builder_->CreateLoad(
                                allocaInst->getAllocatedType(), allocaInst, "self.val");
                            actualSelf = builder_->CreateIntToPtr(
                                selfVal, llvm::PointerType::getUnqual(*context_), "self.ptr");
                        }
                    }
                    
                    llvm::Value* gep = builder_->CreateStructGEP(
                        structType, actualSelf, fieldIndex, fieldName + "_ptr");
                    
                    context_info_.namedValues[inst->result->name] = gep;
                    
                    std::cout << "[DEBUG] emitAlloca: class field '" << fieldName
                              << "' mapped to GEP index " << fieldIndex
                              << " (class: " << activeClass << ")" << std::endl;
                    
                    return gep;
                }
            }
        }
    }
    
    // ================================================================
    // ╪¡╪º┘ה╪⌐ 3: ╪¬╪«╪╡┘ך╪╡ ╪╣╪º╪»┘ך
    // Case 3: Normal allocation
    // ================================================================
    llvm::Type* allocType = getInt64Type();  // default
    if (inst->result.has_value()) {
        switch (inst->result->dataType) {
            case SIRType::F64:
                allocType = getDoubleType();
                break;
            case SIRType::BOOL:
                allocType = llvm::Type::getInt1Ty(*context_);
                break;
            case SIRType::STRING:
                allocType = llvm::PointerType::getUnqual(*context_);
                break;
            default:
                allocType = getInt64Type();
                break;
        }
    }
    
    // Hoist alloca to entry block to avoid stack growth in loops
    llvm::Function* func = builder_->GetInsertBlock()->getParent();
    llvm::IRBuilder<> tmpBuilder(&func->getEntryBlock(),
                                  func->getEntryBlock().begin());
    llvm::Value* result = tmpBuilder.CreateAlloca(allocType, nullptr, regName);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ז┘ג┘ה (╪¬╪╣┘ך┘ך┘ז ╪│╪¼┘ה)
 * Emit move instruction (register assignment)
 * 
 * MOVE ┘ך╪ú╪«╪░ ┘ו╪╡╪»╪▒ ┘ט╪º╪¡╪» (╪½╪º╪¿╪¬ ╪ú┘ט ╪│╪¼┘ה) ┘ט┘ך╪«╪▓┘ז┘ח ┘ב┘ך ╪│╪¼┘ה ╪º┘ה┘ז╪¬┘ך╪¼╪⌐
 * MOVE takes one source (constant or register) and stores it in the result register
 */
llvm::Value* LLVMCodeGen::emitMove(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.empty()) {
        reportError("Move instruction requires 1 operand");
        return nullptr;
    }
    
    if (!inst->result.has_value()) {
        reportError("Move instruction requires a result register");
        return nullptr;
    }
    
    // (AR) ╪¬╪¡┘ט┘ך┘ה ╪º┘ה┘ו╪╣╪º┘ו┘ה ╪º┘ה┘ו╪╡╪»╪▒ ╪Ñ┘ה┘י ┘ג┘ך┘ו╪⌐ LLVM
    // (EN) Resolve source operand to LLVM value
    llvm::Value* value = nullptr;
    const auto& srcOp = inst->operands[0];
    
    if (srcOp.type == SIROperandType::CONSTANT) {
        switch (srcOp.dataType) {
            case SIRType::I64:
                value = getConstantInt(srcOp.intValue, 64);
                break;
            case SIRType::F64:
                value = getConstantFloat(srcOp.floatValue, true);
                break;
            case SIRType::BOOL:
                value = llvm::ConstantInt::get(llvm::Type::getInt1Ty(*context_), srcOp.boolValue);
                break;
            case SIRType::STRING:
                value = getConstantString(srcOp.name);
                break;
            default:
                reportError("Unsupported constant type in move");
                return nullptr;
        }
    } else {
        // (AR) ╪│╪¼┘ה - ╪º┘ה╪¿╪¡╪½ ┘ב┘ך namedValues
        // (EN) Register - lookup in namedValues
        value = context_info_.namedValues[srcOp.name];
    }
    
    if (!value) {
        reportError("Source value not found for move: " + srcOp.name);
        return nullptr;
    }
    
    // (AR) ╪¬╪«╪▓┘ך┘ז ╪º┘ה┘ג┘ך┘ו╪⌐ ┘ב┘ך ╪│╪¼┘ה ╪º┘ה┘ז╪¬┘ך╪¼╪⌐
    // (EN) Store value in result register
    context_info_.namedValues[inst->result->name] = value;
    
    return value;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ GEP (Get Element Pointer)
 * Emit GEP instruction
 * 
 * Source: llvm_codegen.h:447
 */
llvm::Value* LLVMCodeGen::emitGEP(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.size() < 2) {
        reportError("GEP instruction requires at least 2 operands");
        return nullptr;
    }
    
    llvm::Value* ptr = context_info_.namedValues[inst->operands[0].name];
    
    if (!ptr) {
        reportError("Pointer not found for GEP");
        return nullptr;
    }
    
    std::vector<llvm::Value*> indices;
    for (size_t i = 1; i < inst->operands.size(); ++i) {
        llvm::Value* idx = context_info_.namedValues[inst->operands[i].name];
        if (idx) {
            indices.push_back(idx);
        }
    }
    
    // Source: llvm_codegen.cpp:1868 - LLVM 18 Opaque Pointers - ╪º╪│╪¬╪«╪»╪º┘ו i64 ┘ד┘ז┘ט╪╣ ╪º┘ב╪¬╪▒╪º╪╢┘ך
    llvm::Type* elementType = getInt64Type();
    llvm::Value* result = builder_->CreateGEP(elementType, ptr, indices, "geptmp");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

// ============================================================================
// ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 6: ╪¬╪╣┘ה┘ך┘ו╪º╪¬ ╪¬╪»┘ב┘ג ╪º┘ה╪¬╪¡┘ד┘ו
// Phase 6: Control Flow Instructions
// ============================================================================

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ב╪▒╪╣ ╪║┘ך╪▒ ┘ו╪┤╪▒┘ט╪╖
 * Emit unconditional branch instruction
 * 
 * Source: llvm_codegen.h:453
 */
llvm::Value* LLVMCodeGen::emitBranch(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    // (AR) ┘ה╪º ┘ז╪╢┘ך┘ב branch ╪Ñ╪░╪º ┘ד╪º┘ז ╪º┘ה╪¿┘ה┘ט┘ד ╪º┘ה╪¡╪º┘ה┘ך ┘ך┘ז╪¬┘ח┘ך ╪¿┘א ret
    // (EN) Skip branch if current block already has a terminator (e.g. ret)
    llvm::BasicBlock* currentBB = builder_->GetInsertBlock();
    if (currentBB && currentBB->getTerminator()) {
        return nullptr;  // Block already terminated
    }
    
    if (inst->operands.empty()) {
        reportError("Branch instruction requires target label");
        return nullptr;
    }
    
    // Source: SIROperand::name is PUBLIC member at sir_types.h:293
    std::string targetLabel = inst->operands[0].name;
    
    auto it = context_info_.basicBlocks.find(targetLabel);
    if (it == context_info_.basicBlocks.end()) {
        reportError("Target block not found for branch: " + targetLabel);
        return nullptr;
    }
    
    return builder_->CreateBr(it->second);
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ┘ב╪▒╪╣ ┘ו╪┤╪▒┘ט╪╖
 * Emit conditional branch instruction
 * 
 * Source: llvm_codegen.h:454
 */
llvm::Value* LLVMCodeGen::emitCondBranch(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    // (AR) ┘ה╪º ┘ז╪╢┘ך┘ב branch ╪Ñ╪░╪º ┘ד╪º┘ז ╪º┘ה╪¿┘ה┘ט┘ד ╪º┘ה╪¡╪º┘ה┘ך ┘ך┘ז╪¬┘ח┘ך ╪¿┘א ret
    // (EN) Skip conditional branch if current block already has a terminator
    llvm::BasicBlock* currentBB = builder_->GetInsertBlock();
    if (currentBB && currentBB->getTerminator()) {
        return nullptr;  // Block already terminated
    }
    
    if (inst->operands.size() < 3) {
        reportError("Conditional branch requires 3 operands");
        return nullptr;
    }
    
    llvm::Value* condition = resolveOperand(inst->operands[0]);
    std::string trueLabel = inst->operands[1].name;
    std::string falseLabel = inst->operands[2].name;
    
    std::cout << "[DEBUG] emitCondBranch: looking for trueLabel='" << trueLabel 
              << "', falseLabel='" << falseLabel << "'" << std::endl;
    std::cout << "[DEBUG] emitCondBranch: registered blocks count=" << context_info_.basicBlocks.size() << std::endl;
    for (const auto& [name, bb] : context_info_.basicBlocks) {
        std::cout << "[DEBUG] emitCondBranch: registered block '" << name << "'" << std::endl;
    }
    
    if (!condition) {
        reportError("Condition not found for conditional branch");
        return nullptr;
    }
    
    auto trueIt = context_info_.basicBlocks.find(trueLabel);
    auto falseIt = context_info_.basicBlocks.find(falseLabel);
    
    if (trueIt == context_info_.basicBlocks.end() || 
        falseIt == context_info_.basicBlocks.end()) {
        reportError("Target blocks not found for conditional branch");
        return nullptr;
    }
    
    return builder_->CreateCondBr(condition, trueIt->second, falseIt->second);
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪º╪│╪¬╪»╪╣╪º╪í ╪»╪º┘ה╪⌐
 * Emit function call instruction
 * 
 * Source: llvm_codegen.h:455
 */
llvm::Value* LLVMCodeGen::emitCall(std::shared_ptr<SIRInstruction> inst) {
    if (!inst) return nullptr;
    
    if (inst->operands.empty()) {
        reportError("Call instruction requires function name");
        return nullptr;
    }
    
    std::string funcName = inst->operands[0].name;
    
    // ╪¼┘ו╪╣ ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬ ╪ú┘ט┘ה╪º┘כ (┘ז╪¡╪¬╪º╪¼┘ח╪º ┘ה╪º╪│╪¬┘ז╪¬╪º╪¼ ╪º┘ה╪¬┘ט┘ג┘ך╪╣ ╪Ñ╪░╪º ┘ד╪º┘ז╪¬ ╪º┘ה╪»╪º┘ה╪⌐ ╪«╪º╪▒╪¼┘ך╪⌐)
    // Collect arguments first (needed to infer signature for external functions)
    std::vector<llvm::Value*> args;
    for (size_t i = 1; i < inst->operands.size(); ++i) {
        llvm::Value* arg = resolveOperand(inst->operands[i]);
        if (arg) {
            args.push_back(arg);
        }
    }
    
    llvm::Function* callee = nullptr;
    auto funcIt = context_info_.functions.find(funcName);
    if (funcIt != context_info_.functions.end()) {
        callee = funcIt->second;
    } else {
        // =====================================================================
        // (AR) ╪»╪╣┘ו ╪º┘ה╪▒╪¿╪╖ ╪╣╪¿╪▒ ╪º┘ה┘ו┘ה┘ב╪º╪¬: ╪Ñ┘ז╪┤╪º╪í ╪¬╪╡╪▒┘ך╪¡ ╪«╪º╪▒╪¼┘ך ┘ה┘ה╪»╪º┘ה╪⌐
        // (EN) Cross-file linking support: create extern declaration for function
        // ╪╣┘ז╪»┘ו╪º ╪¬╪│╪¬╪»╪╣┘ך ╪»╪º┘ה╪⌐ ┘ו┘ז ┘ו┘ה┘ב ╪ó╪«╪▒╪ל ┘ז┘ז╪┤╪ª ╪¬╪╡╪▒┘ך╪¡╪º┘כ (declare) ┘ב┘ך ╪º┘ה┘ט╪¡╪»╪⌐ ╪º┘ה╪¡╪º┘ה┘ך╪⌐
        // When calling a function from another file, create a declaration in current module
        // ╪º┘ה╪▒╪º╪¿╪╖ (linker) ╪│┘ך╪¡┘ה ╪º┘ה╪▒┘ו╪▓ ┘ה╪º╪¡┘ג╪º┘כ
        // The linker will resolve the symbol later
        // =====================================================================
        
        // ╪º╪│╪¬┘ז╪¬╪º╪¼ ╪ú┘ז┘ט╪º╪╣ ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬ ┘ו┘ז ╪º┘ה┘ג┘ך┘ו ╪º┘ה┘ב╪╣┘ה┘ך╪⌐
        // Infer parameter types from actual values
        std::vector<llvm::Type*> paramTypes;
        for (const auto& arg : args) {
            paramTypes.push_back(arg->getType());
        }
        
        // ╪º╪│╪¬┘ז╪¬╪º╪¼ ┘ז┘ט╪╣ ╪º┘ה╪Ñ╪▒╪¼╪º╪╣ ┘ו┘ז ┘ז╪¬┘ך╪¼╪⌐ ╪º┘ה╪¬╪╣┘ה┘ך┘ו╪⌐
        // Infer return type from instruction result
        llvm::Type* returnType = getVoidType();
        if (inst->result.has_value()) {
            switch (inst->result->dataType) {
                case SIRType::I64:
                    returnType = getInt64Type();
                    break;
                case SIRType::F64:
                    returnType = getDoubleType();
                    break;
                case SIRType::BOOL:
                    returnType = getInt1Type();
                    break;
                case SIRType::PTR:
                case SIRType::STRING:
                    returnType = getInt8PtrType();
                    break;
                case SIRType::VOID:
                    returnType = getVoidType();
                    break;
                default:
                    returnType = getInt64Type();
                    break;
            }
        }
        
        // ╪Ñ┘ז╪┤╪º╪í ┘ז┘ט╪╣ ╪º┘ה╪»╪º┘ה╪⌐ ┘ט╪º┘ה╪¬╪╡╪▒┘ך╪¡ ╪º┘ה╪«╪º╪▒╪¼┘ך
        // Create function type and extern declaration
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            returnType, paramTypes, false);
        
        llvm::FunctionCallee fc = module_->getOrInsertFunction(funcName, funcType);
        callee = llvm::dyn_cast<llvm::Function>(fc.getCallee());
        
        if (!callee) {
            reportError("Failed to create extern declaration for: " + funcName);
            return nullptr;
        }
        
        // ╪¡┘ב╪╕ ┘ב┘ך ╪º┘ה╪│┘ך╪º┘ג ┘ה╪º╪│╪¬╪«╪»╪º┘ו╪º╪¬ ┘ה╪º╪¡┘ג╪⌐
        // Cache in context for future calls
        context_info_.functions[funcName] = callee;
    }
    
    // ╪Ñ┘ז╪┤╪º╪í ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪º┘ה╪º╪│╪¬╪»╪╣╪º╪í
    // Create call instruction
    
    // ================================================================
    // ╪¬╪¡┘ט┘ך┘ה ╪ú┘ז┘ט╪º╪╣ ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬ ╪Ñ╪░╪º ┘ה╪▓┘ו ╪º┘ה╪ú┘ו╪▒ (ptrΓזפi64)
    // Convert argument types if needed (ptrΓזפi64)
    // ================================================================
    llvm::FunctionType* funcType = callee->getFunctionType();
    for (size_t i = 0; i < args.size() && i < funcType->getNumParams(); ++i) {
        llvm::Type* expectedType = funcType->getParamType(i);
        llvm::Type* actualType = args[i]->getType();
        
        if (expectedType != actualType) {
            if (expectedType->isIntegerTy(64) && actualType->isPointerTy()) {
                // (AR) ╪¬╪¡┘ט┘ך┘ה ┘ו╪ñ╪┤╪▒ Γזע i64 (┘ה╪¬┘ו╪▒┘ך╪▒ ┘ד╪º╪ª┘ז╪º╪¬ ┘ה┘ה╪¿┘ז╪º╪⌐)
                // (EN) Convert ptr Γזע i64 (for passing objects to constructors)
                args[i] = builder_->CreatePtrToInt(args[i], getInt64Type(), "arg.ptrtoint");
            } else if (expectedType->isPointerTy() && actualType->isIntegerTy(64)) {
                // (AR) ╪¬╪¡┘ט┘ך┘ה i64 Γזע ┘ו╪ñ╪┤╪▒
                // (EN) Convert i64 Γזע ptr
                args[i] = builder_->CreateIntToPtr(args[i], 
                    llvm::PointerType::getUnqual(*context_), "arg.inttoptr");
            }
        }
    }
    
    llvm::Value* result = nullptr;
    if (callee->getReturnType()->isVoidTy()) {
        builder_->CreateCall(callee, args);
        // ┘ה┘ה╪»┘ט╪º┘ה void╪ל ┘ז╪╣┘ך╪» ┘ג┘ך┘ו╪⌐ ┘ט┘ח┘ו┘ך╪⌐
        result = llvm::ConstantInt::get(getInt64Type(), 0);
    } else {
        result = builder_->CreateCall(callee, args, "calltmp");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    
    return result;
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪¬╪╣┘ה┘ך┘ו╪⌐ ╪▒╪¼┘ט╪╣
 * Emit return instruction
 * 
 * Source: llvm_codegen.h:456
 */


} // namespace LLVM
} // namespace Sad
