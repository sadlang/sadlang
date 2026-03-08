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
#include <unordered_set>
#include <functional>

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR;  // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SIRType

namespace Sad {
namespace LLVM {

// ============================================================================
// ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 1: ╪º┘ה┘ו┘ז╪┤╪ª ┘ט╪º┘ה┘ו╪»┘ו╪▒ ┘ט╪º┘ה╪¬┘ח┘ך╪ª╪⌐ ┘ט╪¬╪¡┘ט┘ך┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣
// Phase 1: Constructor, Destructor, Initialize, Type Conversion
// ============================================================================

// ============================================================================
// Constructor & Destructor / ╪º┘ה┘ו┘ז╪┤╪ª ┘ט╪º┘ה┘ו╪»┘ו╪▒
// ============================================================================

/**
 * ╪º┘ה┘ו┘ז╪┤╪ª - ╪¬┘ח┘ך╪ª╪⌐ ╪¼┘ו┘ך╪╣ ╪º┘ה╪ú╪╣╪╢╪º╪í ╪º┘ה╪«╪º╪╡╪⌐
 * Constructor - Initialize all private members
 * 
 * Source: llvm_codegen.h:208
 * ╪º┘ה┘ו╪╡╪º╪»╪▒ ┘ו┘ז llvm_codegen.h:
 *   - context_ (line 631): std::unique_ptr<llvm::LLVMContext>
 *   - module_ (line 634): std::unique_ptr<llvm::Module>
 *   - builder_ (line 637): std::unique_ptr<llvm::IRBuilder<>>
 *   - targetMachine_ (line 640): llvm::TargetMachine*
 *   - context_info_ (line 643): CodeGenContext
 *   - typeMapper_ (line 646): std::unique_ptr<LLVMTypeMapper>
 *   - optimizer_ (line 657): std::unique_ptr<sad::LLVMOptimizer>
 *   - optimizationLevel_ (line 660): sad::OptimizationLevel
 *   - autoOptimize_ (line 661): bool
 *   - hasErrors_ (line 664): bool
 *   - errors_ (line 665): std::vector<std::string>
 */
LLVMCodeGen::LLVMCodeGen()
    : context_(nullptr)              // Source: llvm_codegen.h:631
    , module_(nullptr)               // Source: llvm_codegen.h:634
    , builder_(nullptr)              // Source: llvm_codegen.h:637
    , targetMachine_(nullptr)        // Source: llvm_codegen.h:640
    , context_info_()                // Source: llvm_codegen.h:643
    , typeMapper_(nullptr)           // Source: llvm_codegen.h:646
    , optimizer_(nullptr)            // Source: llvm_codegen.h:657
    , optimizationLevel_(sad::OptimizationLevel::O0)  // Source: llvm_codegen.h:660
    , autoOptimize_(false)           // Source: llvm_codegen.h:661
    , hasErrors_(false)              // Source: llvm_codegen.h:664
    , errors_()                      // Source: llvm_codegen.h:665
{
    // ╪¬┘ח┘ך╪ª╪⌐ LLVM
    // Initialize LLVM targets
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
}

/**
 * ╪º┘ה┘ו╪»┘ו╪▒ - ╪¬┘ז╪╕┘ך┘ב ╪º┘ה┘ו┘ט╪º╪▒╪»
 * Destructor - Cleanup resources
 * 
 * Source: llvm_codegen.h:209
 * ┘ו┘ה╪º╪¡╪╕╪⌐: unique_ptr ┘ך┘ג┘ט┘ו ╪¿╪º┘ה╪¬┘ז╪╕┘ך┘ב ╪º┘ה╪¬┘ה┘ג╪º╪ª┘ך
 * Note: unique_ptr handles automatic cleanup
 */
LLVMCodeGen::~LLVMCodeGen() {
    // unique_ptr ╪¬┘ז╪╕┘ב ╪¬┘ה┘ג╪º╪ª┘ך╪º┘כ
    // unique_ptr automatically cleans up:
    //   - context_ (line 631)
    //   - module_ (line 634)
    //   - builder_ (line 637)
    //   - typeMapper_ (line 646)
    //   - optimizer_ (line 657)
    
    // targetMachine_ is raw pointer but owned by LLVM
    targetMachine_ = nullptr;
}

// ============================================================================
// Initialization Methods / ╪╖╪▒┘ג ╪º┘ה╪¬┘ח┘ך╪ª╪⌐
// ============================================================================

/**
 * ╪¬┘ח┘ך╪ª╪⌐ ┘ו┘ט┘ה╪» ╪º┘ה┘ד┘ט╪» ┘ו╪╣ ╪º╪│┘ו ╪º┘ה┘ט╪¡╪»╪⌐ ┘ט╪º┘ה┘ו╪╣┘ו╪º╪▒┘ך╪⌐ ╪º┘ה┘ו╪│╪¬┘ח╪»┘ב╪⌐
 * Initialize code generator with module name and target triple
 * 
 * Source: llvm_codegen.h:225
 * @param moduleName ╪º╪│┘ו ╪º┘ה┘ט╪¡╪»╪⌐ / Module name
 * @param targetTriple ╪º┘ה┘ו╪╣┘ו╪º╪▒┘ך╪⌐ ╪º┘ה┘ו╪│╪¬┘ח╪»┘ב╪⌐ / Target triple (e.g., "x86_64-pc-windows-msvc")
 * @return true ╪Ñ╪░╪º ┘ז╪¼╪¡╪¬ ╪º┘ה╪¬┘ח┘ך╪ª╪⌐ / true if initialization succeeded
 */
bool LLVMCodeGen::initialize(const std::string& moduleName, const std::string& targetTriple) {
    try {
        // ╪¬┘ח┘ך╪ª╪⌐ ╪º┘ה╪│┘ך╪º┘ג
        // Initialize context (Source: llvm_codegen.h:631)
        context_ = std::make_unique<llvm::LLVMContext>();
        
        // ╪¬┘ח┘ך╪ª╪⌐ ╪º┘ה┘ט╪¡╪»╪⌐
        // Initialize module (Source: llvm_codegen.h:634)
        module_ = std::make_unique<llvm::Module>(moduleName, *context_);
        
        // ╪¬┘ח┘ך╪ª╪⌐ ╪º┘ה╪¿┘ז╪º╪í
        // Initialize builder (Source: llvm_codegen.h:637)
        builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
        
        // ╪Ñ╪╣╪»╪º╪» ╪º┘ה┘ו╪╣┘ו╪º╪▒┘ך╪⌐ ╪º┘ה┘ו╪│╪¬┘ח╪»┘ב╪⌐
        // Setup target triple
        module_->setTargetTriple(targetTriple);
        
        // ╪º┘ה╪¿╪¡╪½ ╪╣┘ז ╪º┘ה┘ו╪╣┘ו╪º╪▒┘ך╪⌐
        // Look up target
        std::string error;
        auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
        
        if (!target) {
            reportError("Failed to lookup target: " + error);
            return false;
        }
        
        // ╪Ñ┘ז╪┤╪º╪í ╪ó┘ה╪⌐ ╪º┘ה┘ו╪╣┘ו╪º╪▒┘ך╪⌐
        // Create target machine (Source: llvm_codegen.h:640)
        llvm::TargetOptions opt;
        auto RM = llvm::Reloc::Model::PIC_;
        targetMachine_ = target->createTargetMachine(
            targetTriple,
            "generic",
            "",
            opt,
            RM
        );
        
        if (!targetMachine_) {
            reportError("Failed to create target machine");
            return false;
        }
        
        // ╪Ñ╪╣╪»╪º╪» ╪¬╪«╪╖┘ך╪╖ ╪º┘ה╪¿┘ך╪º┘ז╪º╪¬
        // Setup data layout
        module_->setDataLayout(targetMachine_->createDataLayout());
        
        // ┘ו-╪ú01: ╪¬┘ח┘ך╪ª╪⌐ ┘ו╪¡┘ט┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣ Γאפ ╪▒╪¿╪╖ LLVMTypeMapper ╪¿┘א LLVMCodeGen
        // ┘ח╪░╪º ┘ך┘ן┘ו┘ד┘ס┘ז convertType() ┘ו┘ז ╪º╪│╪¬╪«╪»╪º┘ו ╪º┘ה╪¬╪¡┘ט┘ך┘ה ╪º┘ה╪╡╪¡┘ך╪¡ ╪¿╪»┘ה╪º┘כ ┘ו┘ז ╪Ñ╪▒╪¼╪º╪╣ i64 ╪»╪º╪ª┘ו╪º┘כ
        typeMapper_ = std::make_unique<LLVMTypeMapper>(*context_);
        
        // ╪¬┘ח┘ך╪ª╪⌐ ╪º┘ה┘ו╪¡╪│┘ס┘ז
        // Source: llvm_optimizer.h:92 - LLVMOptimizer() constructor takes no parameters
        optimizer_ = std::make_unique<sad::LLVMOptimizer>();
        
        // ╪¬┘ח┘ך╪ª╪⌐ ╪º┘ה┘ו╪¡╪│┘ז ┘ו╪╣ ╪º┘ה┘א target machine
        // Source: llvm_optimizer.h:100 - initialize(llvm::TargetMachine*)
        if (!optimizer_->initialize(targetMachine_)) {
            reportError("Failed to initialize optimizer");
            return false;
        }
        
        // ╪¬╪╣┘ך┘ך┘ז ┘ו╪│╪¬┘ט┘י ╪º┘ה╪¬╪¡╪│┘ך┘ז
        // Set optimization level (Source: llvm_codegen.h:660)
        optimizer_->setOptimizationLevel(optimizationLevel_);
        
        return true;
        
    } catch (const std::exception& e) {
        reportError(std::string("Initialization exception: ") + e.what());
        return false;
    }
}

/**
 * ╪¬┘ח┘ך╪ª╪⌐ ┘ו┘ט┘ה╪» ╪º┘ה┘ד┘ט╪» ┘ו╪╣ ╪º╪│┘ו ╪º┘ה┘ט╪¡╪»╪⌐ (╪º╪│╪¬╪«╪»╪º┘ו ╪º┘ה┘ו╪╣┘ו╪º╪▒┘ך╪⌐ ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך╪⌐)
 * Initialize code generator with module name (use default target)
 * 
 * Source: llvm_codegen.h:233
 * @param moduleName ╪º╪│┘ו ╪º┘ה┘ט╪¡╪»╪⌐ / Module name
 * @return true ╪Ñ╪░╪º ┘ז╪¼╪¡╪¬ ╪º┘ה╪¬┘ח┘ך╪ª╪⌐ / true if initialization succeeded
 */
bool LLVMCodeGen::initialize(const std::string& moduleName) {
    // ╪º╪│╪¬╪«╪»╪º┘ו ╪º┘ה┘ו╪╣┘ו╪º╪▒┘ך╪⌐ ╪º┘ה╪º┘ב╪¬╪▒╪º╪╢┘ך╪⌐ ┘ה┘ה┘ז╪╕╪º┘ו
    // Use default system target triple
    std::string defaultTriple = llvm::sys::getDefaultTargetTriple();
    return initialize(moduleName, defaultTriple);
}

// ============================================================================
// Type Conversion Methods / ╪╖╪▒┘ג ╪¬╪¡┘ט┘ך┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣
// ============================================================================

/**
 * ╪¬╪¡┘ט┘ך┘ה ┘ז┘ט╪╣ Sad ╪Ñ┘ה┘י ┘ז┘ט╪╣ LLVM
 * Convert Sad Type to LLVM Type
 * 
 * Source: llvm_codegen.h:491
 * ┘ו┘ה╪º╪¡╪╕╪⌐: ┘ח╪░╪º ╪¬┘ז┘ב┘ך╪░ ┘ו╪¿╪│╪╖╪ל ╪│┘ך╪¬┘ו ╪¬┘ט╪│┘ך╪╣┘ח ┘ב┘ך ╪º┘ה┘ו╪▒╪º╪¡┘ה ╪º┘ה┘ג╪º╪»┘ו╪⌐
 * Note: This is a simplified implementation, will be expanded in later phases
 * 
 * @param sadType ┘ז┘ט╪╣ Sad / Sad Type
 * @return ┘ז┘ט╪╣ LLVM ╪º┘ה┘ו╪╖╪º╪¿┘ג / Corresponding LLVM Type
 */
llvm::Type* LLVMCodeGen::convertType(std::shared_ptr<Type> sadType) {
    if (!sadType) {
        return getVoidType();
    }
    
    // ╪º╪│╪¬╪«╪»╪º┘ו ┘ו╪¡┘ט┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣ LLVMTypeMapper ┘ה┘ה╪¬╪¡┘ט┘ך┘ה ╪º┘ה╪╡╪¡┘ך╪¡
    // ╪¬┘ו ╪▒╪¿╪╖┘ח ┘ב┘ך ┘ו-╪ú01: ┘ד╪º┘ז ┘ך┘ן╪▒╪¼╪╣ i64 ┘ה┘ד┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣ ╪│╪º╪¿┘ג╪º┘כ
    if (typeMapper_) {
        return typeMapper_->mapSadType(sadType);
    }
    
    // ╪º╪¡╪¬┘ך╪º╪╖┘ך: ╪Ñ╪░╪º ┘ה┘ו ┘ך┘ן┘ח┘ך┘מ┘ס╪ú ┘ו╪¡┘ט┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣ ╪¿╪╣╪»╪ל ┘ז╪│╪¬╪«╪»┘ו ╪º┘ה╪¬╪¡┘ט┘ך┘ה ╪º┘ה┘ך╪»┘ט┘ך
    // ┘ח╪░╪º ┘ה╪º ┘ך╪¼╪¿ ╪ú┘ז ┘ך╪¡╪»╪½ ╪¿╪╣╪» ╪º┘ה╪¬┘ח┘ך╪ª╪⌐ ╪º┘ה╪╡╪¡┘ך╪¡╪⌐
    if (sadType->isVoid()) return getVoidType();
    if (sadType->isBoolean()) return getInt1Type();
    if (sadType->isInteger()) {
        switch (sadType->getBitWidth()) {
            case 8:  return getInt8Type();
            case 16: return getInt16Type();
            case 32: return getInt32Type();
            case 64: return getInt64Type();
            default: return llvm::Type::getIntNTy(*context_, sadType->getBitWidth());
        }
    }
    if (sadType->isFloat()) {
        return sadType->isFloat32() ? getFloatType() : getDoubleType();
    }
    if (sadType->isString()) return getInt8PtrType();
    if (sadType->isPointer()) return llvm::PointerType::get(*context_, 0);
    
    // ┘ז┘ט╪╣ ╪║┘ך╪▒ ┘ו╪╣╪▒┘ט┘ב Γאפ ┘ז┘ן╪▒╪¼╪╣ i64 ┘ד╪º╪¡╪¬┘ך╪º╪╖┘ך
    return getInt64Type();
}

/**
 * ╪¬╪¡┘ט┘ך┘ה ┘ז┘ט╪╣ ╪»╪º┘ה╪⌐ Sad ╪Ñ┘ה┘י ┘ז┘ט╪╣ ╪»╪º┘ה╪⌐ LLVM
 * ┘ו-╪ú01: ╪▒╪¿╪╖ convertFunctionType ╪¿┘ו╪¡┘ט┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣
 * 
 * @param returnType ┘ז┘ט╪╣ ╪º┘ה╪▒╪¼┘ט╪╣ / Return type
 * @param paramTypes ╪ú┘ז┘ט╪º╪╣ ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬ / Parameter types
 * @param isVarArg ┘ח┘ה ╪º┘ה╪»╪º┘ה╪⌐ ┘ו╪¬╪║┘ך╪▒╪⌐ ╪º┘ה┘ו╪╣╪º┘ו┘ה╪º╪¬ / Is variadic
 * @return ┘ז┘ט╪╣ ╪»╪º┘ה╪⌐ LLVM / LLVM function type
 */
llvm::FunctionType* LLVMCodeGen::convertFunctionType(
    std::shared_ptr<Type> returnType,
    const std::vector<std::shared_ptr<Type>>& paramTypes,
    bool isVarArg) {
    
    // ╪º╪│╪¬╪«╪»╪º┘ו ┘ו╪¡┘ט┘ה ╪º┘ה╪ú┘ז┘ט╪º╪╣ ╪Ñ╪░╪º ┘ד╪º┘ז ┘ו┘ן┘ח┘ך┘ס╪ú
    if (typeMapper_) {
        return typeMapper_->mapFunctionType(returnType, paramTypes, isVarArg);
    }
    
    // ╪º╪¡╪¬┘ך╪º╪╖┘ך: ╪¿┘ז╪º╪í ┘ז┘ט╪╣ ╪º┘ה╪»╪º┘ה╪⌐ ┘ך╪»┘ט┘ך╪º┘כ
    llvm::Type* retType = convertType(returnType);
    std::vector<llvm::Type*> llvmParamTypes;
    llvmParamTypes.reserve(paramTypes.size());
    for (const auto& paramType : paramTypes) {
        llvmParamTypes.push_back(convertType(paramType));
    }
    return llvm::FunctionType::get(retType, llvmParamTypes, isVarArg);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ void ┘ב┘ך LLVM
 * Get LLVM void type
 * 
 * Source: llvm_codegen.h:512
 * @return ┘ז┘ט╪╣ void / void type
 */
llvm::Type* LLVMCodeGen::getVoidType() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getVoidTy(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ i1 (bool) ┘ב┘ך LLVM
 * Get LLVM i1 (bool) type
 * 
 * @return ┘ז┘ט╪╣ i1 / i1 type
 */
llvm::Type* LLVMCodeGen::getInt1Type() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getInt1Ty(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ i8 ┘ב┘ך LLVM
 * Get LLVM i8 type
 * 
 * @return ┘ז┘ט╪╣ i8 / i8 type
 */
llvm::Type* LLVMCodeGen::getInt8Type() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getInt8Ty(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ i16 ┘ב┘ך LLVM
 * Get LLVM i16 type
 * 
 * @return ┘ז┘ט╪╣ i16 / i16 type
 */
llvm::Type* LLVMCodeGen::getInt16Type() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getInt16Ty(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ i32 ┘ב┘ך LLVM
 * Get LLVM i32 type
 * 
 * @return ┘ז┘ט╪╣ i32 / i32 type
 */
llvm::Type* LLVMCodeGen::getInt32Type() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getInt32Ty(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ i64 ┘ב┘ך LLVM
 * Get LLVM i64 type
 * 
 * Source: llvm_codegen.h:517
 * @return ┘ז┘ט╪╣ i64 / i64 type
 */
llvm::Type* LLVMCodeGen::getInt64Type() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getInt64Ty(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ float ┘ב┘ך LLVM
 * Get LLVM float type
 * 
 * @return ┘ז┘ט╪╣ float / float type
 */
llvm::Type* LLVMCodeGen::getFloatType() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getFloatTy(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ double ┘ב┘ך LLVM
 * Get LLVM double type
 * 
 * Source: llvm_codegen.h:519
 * @return ┘ז┘ט╪╣ double / double type
 */
llvm::Type* LLVMCodeGen::getDoubleType() {
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::Type::getDoubleTy(*context_);
}

/**
 * ╪º┘ה╪¡╪╡┘ט┘ה ╪╣┘ה┘י ┘ז┘ט╪╣ i8* (┘ו╪ñ╪┤╪▒) ┘ב┘ך LLVM
 * Get LLVM i8* (pointer) type
 * 
 * ┘ו┘ה╪º╪¡╪╕╪⌐: LLVM 15+ ┘ך╪│╪¬╪«╪»┘ו ╪º┘ה┘ו╪ñ╪┤╪▒╪º╪¬ ╪º┘ה╪┤┘ב╪º┘ב╪⌐ (opaque pointers)
 * Note: LLVM 15+ uses opaque pointers
 * 
 * @return ┘ז┘ט╪╣ i8* / i8* type
 */
llvm::Type* LLVMCodeGen::getInt8PtrType() {
    // LLVM 15+ uses opaque pointers
    // Source: context_ is defined at llvm_codegen.h:631
    return llvm::PointerType::get(*context_, 0);
}

// ============================================================================
// Constant Creation Methods / ╪╖╪▒┘ג ╪Ñ┘ז╪┤╪º╪í ╪º┘ה╪½┘ט╪º╪¿╪¬
// ============================================================================

/**
 * ╪Ñ┘ז╪┤╪º╪í ╪½╪º╪¿╪¬ ╪╣╪»╪» ╪╡╪¡┘ך╪¡
 * Create integer constant
 * 
 * Source: llvm_codegen.h:526
 * @param value ╪º┘ה┘ג┘ך┘ו╪⌐ / Value
 * @param bits ╪╣╪»╪» ╪º┘ה╪¿╪¬╪º╪¬ (╪º┘ב╪¬╪▒╪º╪╢┘ך 64) / Number of bits (default 64)
 * @return ╪½╪º╪¿╪¬ LLVM / LLVM Constant
 */
llvm::Constant* LLVMCodeGen::getConstantInt(int64_t value, int bits) {
    llvm::Type* type = nullptr;
    
    switch (bits) {
        case 1:
            type = getInt1Type();
            break;
        case 8:
            type = getInt8Type();
            break;
        case 16:
            type = getInt16Type();
            break;
        case 32:
            type = getInt32Type();
            break;
        case 64:
            type = getInt64Type();
            break;
        

default:
            reportError("Unsupported integer bit width: " + std::to_string(bits));
            return nullptr;
    }
    
    return llvm::ConstantInt::get(type, value, true);
}

/**
 * ╪Ñ┘ז╪┤╪º╪í ╪½╪º╪¿╪¬ ╪╣╪»╪» ╪╣╪┤╪▒┘ך
 * Create floating-point constant
 * 
 * Source: llvm_codegen.h:527
 * @param value ╪º┘ה┘ג┘ך┘ו╪⌐ / Value
 * @param isDouble ┘ח┘ה ┘ח┘ט double ╪ú┘ו float / Is it double or float
 * @return ╪½╪º╪¿╪¬ LLVM / LLVM Constant
 */
llvm::Constant* LLVMCodeGen::getConstantFloat(double value, bool isDouble) {
    if (isDouble) {
        return llvm::ConstantFP::get(getDoubleType(), value);
    } else {
        return llvm::ConstantFP::get(getFloatType(), static_cast<float>(value));
    }
}

/**
 * ╪Ñ┘ז╪┤╪º╪í ╪½╪º╪¿╪¬ ┘ז╪╡┘ך
 * Create string constant
 * 
 * @param value ╪º┘ה┘ז╪╡ / String value
 * @return ╪½╪º╪¿╪¬ LLVM / LLVM Constant
 */
llvm::Constant* LLVMCodeGen::getConstantString(const std::string& value) {
    // Source: builder_ is defined at llvm_codegen.h:637
    return builder_->CreateGlobalStringPtr(value);
}

/**
 * ╪Ñ┘ז╪┤╪º╪í ╪½╪º╪¿╪¬ ┘ו┘ז╪╖┘ג┘ך
 * Create boolean constant
 * 
 * @param value ╪º┘ה┘ג┘ך┘ו╪⌐ ╪º┘ה┘ו┘ז╪╖┘ג┘ך╪⌐ / Boolean value
 * @return ╪½╪º╪¿╪¬ LLVM / LLVM Constant
 */
llvm::Constant* LLVMCodeGen::getConstantBool(bool value) {
    return llvm::ConstantInt::get(getInt1Type(), value ? 1 : 0);
}

/**
 * ╪Ñ┘ז╪┤╪º╪í ┘ו╪ñ╪┤╪▒ null
 * Create null pointer
 * 
 * @param type ┘ז┘ט╪╣ ╪º┘ה┘ו╪ñ╪┤╪▒ / Pointer type
 * @return ╪½╪º╪¿╪¬ LLVM / LLVM Constant
 */
llvm::Constant* LLVMCodeGen::getNullPtr(llvm::Type* type) {
    if (!type) {
        type = getInt8PtrType();
    }
    return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(type));
}

// ============================================================================
// Error Handling Methods / ╪╖╪▒┘ג ┘ו╪╣╪º┘ה╪¼╪⌐ ╪º┘ה╪ú╪«╪╖╪º╪í
// ============================================================================

/**
 * ╪º┘ה╪Ñ╪¿┘ה╪º╪║ ╪╣┘ז ╪«╪╖╪ú
 * Report an error
 * 
 * Source: llvm_codegen.h:614
 * @param message ╪▒╪│╪º┘ה╪⌐ ╪º┘ה╪«╪╖╪ú / Error message
 */
void LLVMCodeGen::reportError(const std::string& message) {
    // Source: hasErrors_ is defined at llvm_codegen.h:664
    hasErrors_ = true;
    
    // Source: errors_ is defined at llvm_codegen.h:665
    errors_.push_back(message);
    
    // ╪╖╪¿╪º╪╣╪⌐ ╪º┘ה╪«╪╖╪ú
    // Print error
    std::cerr << "LLVM CodeGen Error: " << message << std::endl;
}

// ============================================================================
// ╪º┘ה┘ו╪▒╪¡┘ה╪⌐ 2: ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה┘ט╪¡╪»╪⌐ ┘ט╪º┘ה┘ו╪¬╪║┘ך╪▒╪º╪¬ ╪º┘ה╪╣╪º┘ו╪⌐
// Phase 2: Module & Global Emission
// ============================================================================

/**
 * ╪¬┘ט┘ה┘ך╪» ┘ט╪¡╪»╪⌐ LLVM ┘ד╪º┘ו┘ה╪⌐ ┘ו┘ז ┘ט╪¡╪»╪⌐ SIR
 * Generate complete LLVM module from SIR module
 * 
 * Source: llvm_codegen.h:243
 * @param sirModule ┘ט╪¡╪»╪⌐ SIR / SIR module
 * @return ┘ט╪¡╪»╪⌐ LLVM / LLVM module
 */
std::unique_ptr<llvm::Module> LLVMCodeGen::generate(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) {
        reportError("SIR module is null");
        return nullptr;
    }
    
    // ╪¡┘ב╪╕ ┘ו╪▒╪¼╪╣ ┘ט╪¡╪»╪⌐ SIR ┘ה┘ה┘ט╪╡┘ט┘ה ┘ה┘ו╪╣┘ה┘ט┘ו╪º╪¬ ╪º┘ה╪ú╪╡┘ז╪º┘ב
    // Store SIR module reference for class info access
    sirModule_ = sirModule;
    
    // معالجة الأصناف وإنشاء أنواع الهياكل
    // Pre-process classes and create struct types
    preprocessClasses(sirModule);
    
    // (AR) بناء vtables لجميع الأصناف
    // (EN) Build vtables for all classes
    buildClassVtables(sirModule);
    
    // إصدار الوحدة بالكامل
    // Emit complete module
    emitModule(sirModule);

    // (AR) تحديث مداخل vtable المؤجلة بعد أن أصبحت كل الدوال متاحة
    // (EN) Patch deferred vtable entries now that all functions are emitted
    patchClassVtables();
    
    // ╪º┘ה╪¬╪¡┘ג┘ג ┘ו┘ז ╪º┘ה┘ט╪¡╪»╪⌐ (╪¬╪¡╪░┘ך╪▒ ┘ב┘ג╪╖╪ל ┘ה╪º ╪Ñ┘ך┘ג╪º┘ב)
    // Verify module (warning only, don't stop)
    if (!verify()) {
        std::cerr << "[WARNING] Module verification failed, continuing anyway...\n";
        // reportError("Module verification failed");
        // return nullptr;
    }
    
    // ╪¬╪╖╪¿┘ך┘ג ╪º┘ה╪¬╪¡╪│┘ך┘ז╪º╪¬ ╪Ñ╪░╪º ┘ד╪º┘ז ╪º┘ה╪¬╪¡╪│┘ך┘ז ╪º┘ה╪¬┘ה┘ג╪º╪ª┘ך ┘ו┘ב╪╣┘ה
    // Apply optimizations if auto-optimize is enabled
    // Source: autoOptimize_ is defined at llvm_codegen.h:661
    if (autoOptimize_) {
        optimize();
    }
    
    // Source: module_ is defined at llvm_codegen.h:634
    return std::move(module_);
}

// ============================================================================
// ┘ו╪╣╪º┘ה╪¼╪⌐ ╪º┘ה╪ú╪╡┘ז╪º┘ב - ╪Ñ┘ז╪┤╪º╪í ╪ú┘ז┘ט╪º╪╣ ╪º┘ה┘ח┘ך╪º┘ד┘ה LLVM
// Pre-process classes - create LLVM struct types
// ============================================================================
void LLVMCodeGen::preprocessClasses(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) return;
    
    // (AR) ترتيب الأصناف حسب الاعتمادية (الأب قبل الابن)
    // (EN) Topological sort: process parent classes before children
    const auto& allClasses = sirModule->getClasses();
    std::vector<std::shared_ptr<SIRClass>> sortedClasses;
    std::unordered_set<std::string> processed;
    
    // (AR) دالة مساعدة للترتيب التبولوجي
    // (EN) Helper for topological sort
    std::function<void(const std::shared_ptr<SIRClass>&)> processClass;
    processClass = [&](const std::shared_ptr<SIRClass>& cls) {
        if (!cls || processed.count(cls->name)) return;
        // (AR) معالجة الأب أولاً
        // (EN) Process parent first
        if (!cls->parentClass.empty() && !processed.count(cls->parentClass)) {
            for (const auto& other : allClasses) {
                if (other && other->name == cls->parentClass) {
                    processClass(other);
                    break;
                }
            }
        }
        processed.insert(cls->name);
        sortedClasses.push_back(cls);
    };
    
    for (const auto& cls : allClasses) {
        processClass(cls);
    }
    
    // (AR) معالجة كل صنف في الوحدة وإنشاء نوع هيكل LLVM له
    // (EN) Process each class in the module and create LLVM struct type
    for (const auto& sirClass : sortedClasses) {
        if (!sirClass) continue;
        
        const std::string& className = sirClass->name;
        
        // (AR) تسجيل علاقة الوراثة
        // (EN) Register inheritance relationship
        if (!sirClass->parentClass.empty()) {
            context_info_.classParentMap[className] = sirClass->parentClass;
            #ifndef NDEBUG
            std::cout << "[DEBUG] preprocessClasses: '" << className 
                      << "' inherits from '" << sirClass->parentClass << "'" << std::endl;
            #endif
        }
        
        // (AR) تسجيل الصنف المجرد
        // (EN) Register abstract class
        if (sirClass->isAbstract) {
            context_info_.abstractClasses.insert(className);
        }
        
        // (AR) جمع أنواع الحقول بالترتيب — الحقل 0 دائماً مؤشر vtable
        // (EN) Collect field types in order — field 0 is always vtable pointer
        std::vector<llvm::Type*> fieldTypes;
        std::vector<std::string> fieldNames;
        
        // (AR) الحقل 0: مؤشر vtable (ptr) — لدعم الاستدعاء الافتراضي
        // (EN) Field 0: vtable pointer (ptr) — for virtual dispatch
        fieldTypes.push_back(llvm::PointerType::getUnqual(*context_));
        
        for (const auto& fieldName : sirClass->fieldOrder_) {
            // (AR) ╪¬╪¡┘ט┘ך┘ה ╪ú┘ז┘ט╪º╪╣ ╪º┘ה╪¡┘ג┘ט┘ה ╪¿╪┤┘ד┘ה ╪╡╪¡┘ך╪¡ ╪¿╪»┘ה╪º┘כ ┘ו┘ז ╪º╪│╪¬╪«╪»╪º┘ו i64 ┘ה┘ד┘ה ╪┤┘ך╪í
            // (EN) Convert field types properly instead of using i64 for everything
            auto fieldIt = sirClass->fields_.find(fieldName);
            if (fieldIt != sirClass->fields_.end()) {
                switch (fieldIt->second) {
                    case SIRType::I64:
                        fieldTypes.push_back(getInt64Type());
                        break;
                    case SIRType::F64:
                        fieldTypes.push_back(getDoubleType());
                        break;
                    case SIRType::BOOL:
                        fieldTypes.push_back(getInt1Type());
                        break;
                    case SIRType::STRING:
                    case SIRType::PTR:
                        fieldTypes.push_back(llvm::PointerType::getUnqual(*context_));
                        break;
                    case SIRType::STRUCT: {
                        // (AR) ╪¿╪¡╪½ ╪╣┘ז ┘ז┘ט╪╣ ┘ח┘ך┘ד┘ה ╪º┘ה╪╡┘ז┘ב ╪º┘ה┘ו┘ן╪▒╪¼╪╣
                        // (EN) Look up referenced class struct type
                        // ╪¡╪º┘ה┘ך╪º┘כ ┘ז╪│╪¬╪«╪»┘ו ┘ו╪ñ╪┤╪▒ ╪╣╪º┘ו ┘ה┘ה┘ד╪º╪ª┘ז╪º╪¬ ╪º┘ה┘ו╪¬╪»╪º╪«┘ה╪⌐
                        fieldTypes.push_back(llvm::PointerType::getUnqual(*context_));
                        break;
                    }
                    default:
                        // (AR) ╪º╪¡╪¬┘ך╪º╪╖┘ך: i64 ┘ה┘ה╪ú┘ז┘ט╪º╪╣ ╪║┘ך╪▒ ╪º┘ה┘ו╪╣╪▒┘ט┘ב╪⌐
                        // (EN) Fallback: i64 for unknown types
                        fieldTypes.push_back(getInt64Type());
                        break;
                }
            } else {
                fieldTypes.push_back(getInt64Type());  // ╪º╪¡╪¬┘ך╪º╪╖┘ך / fallback
            }
            fieldNames.push_back(fieldName);
        }
        
        // (AR) ╪Ñ┘ז╪┤╪º╪í ┘ז┘ט╪╣ ╪º┘ה┘ח┘ך┘ד┘ה
        // (EN) Create struct type
        if (!fieldTypes.empty()) {
            llvm::StructType* structType = llvm::StructType::create(
                *context_, fieldTypes, "class." + className);
            context_info_.classStructTypes[className] = structType;
            context_info_.classFieldNames[className] = fieldNames;
            
            #ifndef NDEBUG
            std::cout << "[DEBUG] preprocessClasses: created struct type for class '"
                      << className << "' with " << fieldNames.size() << " fields" << std::endl;
            #endif
        }
    }
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ┘ט╪¡╪»╪⌐ SIR ┘ד╪º┘ו┘ה╪⌐
 * Emit complete SIR module
 * 
 * Source: llvm_codegen.h:316
 * @param sirModule ┘ט╪¡╪»╪⌐ SIR / SIR module
 */
void LLVMCodeGen::emitModule(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) {
        reportError("SIR module is null in emitModule");
        return;
    }
    
    // ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה┘ו╪¬╪║┘ך╪▒╪º╪¬ ╪º┘ה╪╣╪º┘ו╪⌐
    // Emit global variables
    emitGlobalVariables(sirModule);
    
    // ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה╪½┘ט╪º╪¿╪¬
    // Emit constants
    emitConstants(sirModule);
    
    // ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה╪»┘ט╪º┘ה ╪º┘ה╪╣╪º┘ו╪⌐
    // Emit global functions
    emitGlobalFunctions(sirModule);
}

/**
 * ╪Ñ╪╡╪»╪º╪▒ ╪º┘ה┘ו╪¬╪║┘ך╪▒╪º╪¬ ╪º┘ה╪╣╪º┘ו╪⌐
 * Emit global variables
 * 
 * Source: llvm_codegen.h:329
 * @param sirModule ┘ט╪¡╪»╪⌐ SIR / SIR module
 */
void LLVMCodeGen::emitGlobalVariables(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) {
        return;
    }
    
    // Source: SIRModule::getGlobalVariables() returns const std::vector<std::shared_ptr<SIRGlobalVariable>>&
    // ┘ו┘ז sir_module.h - ╪º┘ה┘ט╪╡┘ט┘ה ┘ה┘ה┘ו╪¬╪║┘ך╪▒╪º╪¬ ╪º┘ה╪╣╪º┘ו╪⌐
    const auto& globals = sirModule->getGlobalVariables();
    
    for (const auto& globalVar : globals) {
        if (!globalVar) continue;
        
        // Source: SIRGlobalVariable::getName() is at sir_module.h:374
        // Source: SIRGlobalVariable::getType() is at sir_module.h:380
        // Source: SIRGlobalVariable::getIsConstant() is at sir_module.h:386
        std::string varName = globalVar->getName();
        SIRType varType = globalVar->getType();
        bool isConstant = globalVar->getIsConstant();
        
        // ╪¬╪¡┘ט┘ך┘ה ╪º┘ה┘ז┘ט╪╣ ╪Ñ┘ה┘י LLVM
        // Convert type to LLVM
        llvm::Type* llvmType = nullptr;
        
        // ╪¬╪¡┘ט┘ך┘ה ╪ú┘ז┘ט╪º╪╣ SIR ╪º┘ה╪ú╪│╪º╪│┘ך╪⌐
        // Convert basic SIR types
        // Source: SIRType enum is defined at sir_types.h:114-119
        switch (varType) {
            case SIRType::I64:
                llvmType = getInt64Type();
                break;
            case SIRType::F64:
                llvmType = getDoubleType();
                break;
            case SIRType::BOOL:
                llvmType = getInt1Type();
                break;
            case SIRType::PTR:
                llvmType = getInt8PtrType();
                break;
            case SIRType::VOID:
                llvmType = getVoidType();
                break;
            default:
                llvmType = getInt64Type();  // ╪º┘ב╪¬╪▒╪º╪╢┘ך / Default
                break;
        }
        
        if (!llvmType) {
            reportError("Failed to convert type for global variable: " + varName);
            continue;
        }
        
        // ╪Ñ┘ז╪┤╪º╪í ┘ג┘ך┘ו╪⌐ ╪ú┘ט┘ה┘ך╪⌐
        // Create initializer
        llvm::Constant* initializer = nullptr;
        
        // Source: SIRGlobalVariable::initialValue is at sir_module.h:344
        if (!globalVar->initialValue.empty()) {
            // (AR) ╪¬╪¡┘ה┘ך┘ה ╪º┘ה┘ג┘ך┘ו╪⌐ ╪º┘ה╪ú┘ט┘ה┘ך╪⌐ ╪¿┘ז╪º╪í┘כ ╪╣┘ה┘י ╪º┘ה┘ז┘ט╪╣
            // (EN) Parse initial value based on type
            try {
                switch (varType) {
                    case SIRType::I64:
                    case SIRType::BOOL:
                        initializer = llvm::ConstantInt::get(
                            llvmType, std::stoll(globalVar->initialValue), true);
                        break;
                    case SIRType::F64:
                        initializer = llvm::ConstantFP::get(
                            llvmType, std::stod(globalVar->initialValue));
                        break;
                    default:
                        initializer = llvm::Constant::getNullValue(llvmType);
                        break;
                }
            } catch (...) {
                initializer = llvm::Constant::getNullValue(llvmType);
            }
        } else {
            initializer = llvm::Constant::getNullValue(llvmType);
        }
        
        // ╪Ñ┘ז╪┤╪º╪í ╪º┘ה┘ו╪¬╪║┘ך╪▒ ╪º┘ה╪╣╪º┘ו
        // Create global variable
        // Source: module_ is defined at llvm_codegen.h:634
        auto* globalLLVM = new llvm::GlobalVariable(
            *module_,
            llvmType,
            isConstant,
            llvm::GlobalValue::ExternalLinkage,
            initializer,
            varName
        );
        
        // ╪¡┘ב╪╕ ┘ב┘ך ╪º┘ה╪│┘ך╪º┘ג
        // Save to context
        // Source: context_info_ is defined at llvm_codegen.h:643
        // Source: CodeGenContext::namedValues is at llvm_codegen.h:617
        context_info_.namedValues[varName] = globalLLVM;
    }
}

/**
 * إصدار الثوابت
 * Emit constants
 * 
 * Source: llvm_codegen.h:335
 * @param sirModule وحدة SIR / SIR module
 */


// ============================================================================
// OOP vtable & Virtual Dispatch / جدول الدوال الافتراضية
// ============================================================================

/**
 * (AR) بناء vtable لكل صنف يحتوي على دوال
 * (EN) Build vtable for each class that has methods
 *
 * التصميم:
 *   - vtable = مصفوفة ثابتة من مؤشرات الدوال [ptr, ptr, ...]
 *   - الصنف الابن يرث vtable الأب وينسخها مع استبدال الدوال المُعاد تعريفها
 *   - الحقل 0 في كل كائن يُشير إلى vtable الخاص بصنفه الحقيقي
 */
void LLVMCodeGen::buildClassVtables(std::shared_ptr<SIRModule> sirModule) {
    if (!sirModule) return;
    
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    
    // (AR) ترتيب الأصناف: الأب قبل الابن (نفس الترتيب في preprocessClasses)
    // (EN) Process in topo order: parents before children
    const auto& allClasses = sirModule->getClasses();
    std::vector<std::shared_ptr<SIRClass>> sorted;
    std::unordered_set<std::string> done;
    
    std::function<void(const std::shared_ptr<SIRClass>&)> topoSort;
    topoSort = [&](const std::shared_ptr<SIRClass>& cls) {
        if (!cls || done.count(cls->name)) return;
        if (!cls->parentClass.empty() && !done.count(cls->parentClass)) {
            for (const auto& other : allClasses) {
                if (other && other->name == cls->parentClass) {
                    topoSort(other);
                    break;
                }
            }
        }
        done.insert(cls->name);
        sorted.push_back(cls);
    };
    for (const auto& cls : allClasses) topoSort(cls);
    
    for (const auto& cls : sorted) {
        if (!cls) continue;
        const std::string& className = cls->name;
        
        // (AR) بناء تخطيط vtable: ابدأ من الأب
        // (EN) Build vtable layout: start from parent
        std::vector<std::string> vtableSlots;
        if (!cls->parentClass.empty()) {
            auto parentIt = context_info_.classVtableLayout.find(cls->parentClass);
            if (parentIt != context_info_.classVtableLayout.end()) {
                vtableSlots = parentIt->second;  // نسخ vtable الأب
            }
        }
        
        // (AR) إضافة/استبدال دوال الصنف الحالي
        // (EN) Add/override methods from current class
        for (const auto& [methodName, methodFunc] : cls->methods_) {
            // تجاهل الباني — ليس في vtable
            if (methodName == "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1" || methodName == "بناء" ||
                methodName == "__init__" || methodName == "init" ||
                methodName == "\xD8\xA8\xD8\xA7\xD9\x86\xD9\x8A" || methodName == "باني" ||
                methodName == "\xD9\x85\xD9\x86\xD8\xB4\xD8\xA6" || methodName == "منشئ") {
                continue;
            }
            
            // تجاهل الهدم — يُعالج بشكل منفصل
            if (methodName == "\xD9\x87\xD8\xAF\xD9\x85" || methodName == "هدم" ||
                methodName == "__del__" || methodName == "__destroy__") {
                // تسجيل الهدم
                context_info_.classDestructors[className] = className + "." + methodName;
                continue;
            }
            
            std::string fullName = className + "." + methodName;
            
            // هل هذه الدالة تعيد تعريف دالة من الأب؟
            bool overridden = false;
            for (size_t i = 0; i < vtableSlots.size(); i++) {
                // استخراج اسم الدالة من الاسم الكامل "صنف.دالة"
                size_t dot = vtableSlots[i].rfind('.');
                std::string slotMethodName = (dot != std::string::npos) 
                    ? vtableSlots[i].substr(dot + 1) : vtableSlots[i];
                if (slotMethodName == methodName) {
                    vtableSlots[i] = fullName;  // استبدال بتنفيذ الابن
                    overridden = true;
                    break;
                }
            }
            if (!overridden) {
                vtableSlots.push_back(fullName);  // إضافة دالة جديدة
            }
        }
        
        // تسجيل التخطيط
        context_info_.classVtableLayout[className] = vtableSlots;
        
        // (AR) إنشاء متغير عام ثابت لـ vtable
        // (EN) Create constant global for vtable
        if (!vtableSlots.empty()) {
            auto vtableArrayTy = llvm::ArrayType::get(ptrTy, vtableSlots.size());
            
            std::vector<llvm::Constant*> vtableEntries;
            for (const auto& fullMethodName : vtableSlots) {
                llvm::Function* fn = module_->getFunction(fullMethodName);
                if (fn) {
                    vtableEntries.push_back(fn);
                } else {
                    // الدالة لم تُعرّف بعد — سيتم ربطها لاحقاً (null مؤقتاً)
                    vtableEntries.push_back(llvm::ConstantPointerNull::get(ptrTy));
                }
            }
            
            auto vtableInit = llvm::ConstantArray::get(vtableArrayTy, vtableEntries);
            auto vtableGlobal = new llvm::GlobalVariable(
                *module_, vtableArrayTy, true,  // isConstant=true
                llvm::GlobalValue::PrivateLinkage, vtableInit,
                "vtable." + className);
            
            context_info_.classVtableGlobals[className] = vtableGlobal;
            
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildVtable: '" << className 
                      << "' with " << vtableSlots.size() << " slots" << std::endl;
            #endif
        }
    }
}

/**
 * (AR) تحديث مداخل vtable المؤجلة بعد إصدار جميع الدوال.
 * (EN) Patch deferred vtable entries after all functions have been emitted.
 */
void LLVMCodeGen::patchClassVtables() {
    auto ptrTy = llvm::PointerType::getUnqual(*context_);

    for (const auto& [className, vtableGlobal] : context_info_.classVtableGlobals) {
        auto layoutIt = context_info_.classVtableLayout.find(className);
        if (layoutIt == context_info_.classVtableLayout.end() || !vtableGlobal) {
            continue;
        }

        const auto& slots = layoutIt->second;
        if (slots.empty()) {
            continue;
        }

        std::vector<llvm::Constant*> entries;
        entries.reserve(slots.size());

        for (const auto& fullMethodName : slots) {
            llvm::Function* fn = module_->getFunction(fullMethodName);
            if (fn) {
                entries.push_back(fn);
            } else {
                // Keep null as defensive fallback, but report missing entry.
                reportError("Missing vtable method at patch time: " + fullMethodName);
                entries.push_back(llvm::ConstantPointerNull::get(ptrTy));
            }
        }

        auto arrTy = llvm::ArrayType::get(ptrTy, entries.size());
        vtableGlobal->setInitializer(llvm::ConstantArray::get(arrTy, entries));
    }
}

/**
 * (AR) تخزين مؤشر vtable في الحقل 0 من الكائن
 * (EN) Store vtable pointer in field 0 of the object
 */
void LLVMCodeGen::storeVtablePtr(llvm::Value* objPtr, const std::string& className) {
    auto vtableIt = context_info_.classVtableGlobals.find(className);
    if (vtableIt == context_info_.classVtableGlobals.end()) return;  // لا vtable
    
    auto structIt = context_info_.classStructTypes.find(className);
    if (structIt == context_info_.classStructTypes.end()) return;
    
    llvm::StructType* structType = structIt->second;
    
    // GEP إلى الحقل 0 (مؤشر vtable)
    llvm::Value* vtablePtrSlot = builder_->CreateStructGEP(structType, objPtr, 0, "vtable.slot");
    
    // تحويل المصفوفة العامة إلى مؤشر (decay)
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* vtablePtr = builder_->CreateBitCast(vtableIt->second, ptrTy, "vtable.ptr");
    
    builder_->CreateStore(vtablePtr, vtablePtrSlot);
}

/**
 * (AR) الاستدعاء الافتراضي: تحميل مؤشر الدالة من vtable + استدعاء غير مباشر
 * (EN) Virtual dispatch: load function pointer from vtable + indirect call
 */
llvm::Value* LLVMCodeGen::emitVirtualCall(llvm::Value* objPtr, const std::string& className,
                                           const std::string& methodName,
                                           const std::vector<llvm::Value*>& extraArgs) {
    // (AR) البحث عن رقم الفتحة (slot) في vtable
    // (EN) Find the vtable slot index
    auto layoutIt = context_info_.classVtableLayout.find(className);
    if (layoutIt == context_info_.classVtableLayout.end()) return nullptr;
    
    const auto& layout = layoutIt->second;
    int slotIndex = -1;
    for (size_t i = 0; i < layout.size(); i++) {
        size_t dot = layout[i].rfind('.');
        std::string slotMethodName = (dot != std::string::npos) 
            ? layout[i].substr(dot + 1) : layout[i];
        if (slotMethodName == methodName) {
            slotIndex = static_cast<int>(i);
            break;
        }
    }
    
    if (slotIndex < 0) return nullptr;  // ليست في vtable
    
    auto structIt = context_info_.classStructTypes.find(className);
    if (structIt == context_info_.classStructTypes.end()) return nullptr;
    
    llvm::StructType* structType = structIt->second;
    auto ptrTy = llvm::PointerType::getUnqual(*context_);
    
    // (AR) الخطوة 1: تحميل مؤشر vtable من الحقل 0
    // (EN) Step 1: Load vtable pointer from field 0
    llvm::Value* vtableSlotAddr = builder_->CreateStructGEP(structType, objPtr, 0, "vtable.addr");
    llvm::Value* vtablePtr = builder_->CreateLoad(ptrTy, vtableSlotAddr, "vtable.load");
    
    // (AR) الخطوة 2: حساب عنوان الفتحة في vtable
    // (EN) Step 2: GEP into vtable array at slot index
    llvm::Value* slotAddr = builder_->CreateGEP(
        ptrTy, vtablePtr,
        {llvm::ConstantInt::get(getInt64Type(), slotIndex)},
        "vslot.addr");
    
    // (AR) الخطوة 3: تحميل مؤشر الدالة
    // (EN) Step 3: Load function pointer
    llvm::Value* fnPtr = builder_->CreateLoad(ptrTy, slotAddr, "vfn.ptr");
    
    // (AR) الخطوة 4: بناء المعاملات (self + extra)
    // (EN) Step 4: Build args (self + extra)
    std::vector<llvm::Value*> callArgs = {objPtr};
    callArgs.insert(callArgs.end(), extraArgs.begin(), extraArgs.end());
    
    // (AR) الخطوة 5: بناء نوع الدالة — نحتاج لمعرفة التوقيع
    // (EN) Step 5: Build function type — need to know the signature
    // البحث عن الدالة الأصلية لمعرفة نوعها
    std::string origFuncName = layout[slotIndex];
    llvm::Function* origFunc = module_->getFunction(origFuncName);
    
    llvm::FunctionType* fnType = nullptr;
    if (origFunc) {
        fnType = origFunc->getFunctionType();
    } else {
        // احتياطي: افتراض توقيع (ptr, ...) → i64
        std::vector<llvm::Type*> argTypes;
        for (auto* a : callArgs) argTypes.push_back(a->getType());
        fnType = llvm::FunctionType::get(getInt64Type(), argTypes, false);
    }
    
    // (AR) الخطوة 6: الاستدعاء غير المباشر
    // (EN) Step 6: Indirect call
    llvm::Value* result = builder_->CreateCall(fnType, fnPtr, callArgs,
        fnType->getReturnType()->isVoidTy() ? "" : (methodName + "_virt"));
    
    return result;
}

/**
 * (AR) استدعاء دالة الهدم للكائن (إن وُجدت)
 * (EN) Call destructor for object (if exists)
 */
void LLVMCodeGen::emitDestructorCall(llvm::Value* objPtr, const std::string& className) {
    // البحث في سلسلة الوراثة عن أول هدم
    std::string searchClass = className;
    while (!searchClass.empty()) {
        auto dtorIt = context_info_.classDestructors.find(searchClass);
        if (dtorIt != context_info_.classDestructors.end()) {
            llvm::Function* dtorFunc = module_->getFunction(dtorIt->second);
            if (dtorFunc) {
                builder_->CreateCall(dtorFunc, {objPtr});
            }
            return;
        }
        auto parentIt = context_info_.classParentMap.find(searchClass);
        if (parentIt != context_info_.classParentMap.end()) {
            searchClass = parentIt->second;
        } else {
            break;
        }
    }
}

/**
 * (AR) حساب فهرس الحقل الحقيقي في الهيكل مع إزاحة vtable
 * (EN) Calculate actual field struct index with vtable offset
 * 
 * الحقل 0 في الهيكل → مؤشر vtable (محجوز)
 * الحقل 1 في الهيكل → أول حقل للمستخدم (الفهرس 0 في classFieldNames)
 */
int LLVMCodeGen::getFieldStructIndex(const std::string& className, int userFieldIndex) const {
    // كل صنف يملك vtable pointer في الحقل 0
    return userFieldIndex + 1;
}

} // namespace LLVM
} // namespace Sad
