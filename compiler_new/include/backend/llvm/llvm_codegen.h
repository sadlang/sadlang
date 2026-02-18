/*
 * ============================================================================
 * مولد كود LLVM IR - رأس الملف
 * LLVM IR Code Generator - Header File
 * ============================================================================
 * 
 * الوصف (Description):
 *   هذا الملف يحتوي على مولد كود LLVM IR الذي يترجم SIR (Sad Intermediate
 *   Representation) إلى LLVM IR. يوفر تحويل الأنواع، إصدار الدوال، ترجمة
 *   التعليمات، ومعالجة تدفق التحكم.
 * 
 *   This file contains the LLVM IR code generator that translates SIR (Sad
 *   Intermediate Representation) to LLVM IR. Provides type conversion,
 *   function emission, instruction translation, and control flow handling.
 * 
 * المكونات الرئيسية (Main Components):
 *   - LLVMCodeGen: الفئة الرئيسية لتوليد LLVM IR
 *   - LLVMCodeGen: Main class for LLVM IR generation
 * 
 *   - Type Mapping: تحويل أنواع Sad إلى أنواع LLVM
 *   - Type Mapping: Convert Sad types to LLVM types
 * 
 *   - Function Emission: إصدار دوال LLVM من SIR functions
 *   - Function Emission: Emit LLVM functions from SIR functions
 * 
 *   - Instruction Translation: ترجمة تعليمات SIR إلى LLVM IR
 *   - Instruction Translation: Translate SIR instructions to LLVM IR
 * 
 *   - Control Flow: معالجة القفزات والفروع والاستدعاءات
 *   - Control Flow: Handle jumps, branches, and calls
 * 
 * الاستخدام (Usage):
 *   ```cpp
 *   LLVMCodeGen codegen;
 *   codegen.initialize("my_module", "x86_64-linux-gnu");
 *   auto llvm_module = codegen.generate(sir_module);
 *   codegen.emitToFile("output.ll");
 *   ```
 * 
 * التبعيات (Dependencies):
 *   - LLVM Core Libraries
 *   - SIR (Sad IR) definitions
 *   - Type system
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_LLVM_CODEGEN_H
#define SAD_LLVM_CODEGEN_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// LLVM Headers
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>

// Sad LLVM Components (مكونات Sad LLVM)
#include "llvm_type_mapper.h"
#include "llvm_control_flow.h"
#include "llvm_expression_builder.h"
#include "llvm_memory_manager.h"
#include "llvm_optimizer.h"  // إضافة محسّن LLVM / Add LLVM optimizer

// Sad SIR Components (مكونات Sad SIR)
// Source: compiler/frontend/include/sir_*.h - مضاف في CMake include_directories line 27
#include "sir_module.h"     // Source: compiler/frontend/include/sir_module.h
#include "sir_instruction.h" // Source: compiler/frontend/include/sir_instruction.h
#include "sir_types.h"       // Source: compiler/frontend/include/sir_types.h

namespace Sad {
namespace LLVM {

// ============================================================================
// Forward Declarations / التصريحات المسبقة
// ============================================================================

// Use SIR types from Compiler namespace
// استخدام أنواع SIR من namespace Compiler
using SIRModule = Compiler::SIR::SIRModule;
using SIRFunction = Compiler::SIR::SIRFunction;
using SIRBasicBlock = Compiler::SIR::SIRBasicBlock;
using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIRType = Compiler::SIR::SIRType;
using SIROperand = Compiler::SIR::SIROperand;
using SIROperandType = Compiler::SIR::SIROperandType;

// ============================================================================
// CodeGenContext - سياق توليد الكود
// CodeGenContext - Code Generation Context
// ============================================================================

/**
 * سياق توليد الكود يحتوي على معلومات الحالة أثناء الترجمة
 * Code generation context holds state information during translation
 */
struct CodeGenContext {
    // الدالة الحالية / Current function
    llvm::Function* currentFunction = nullptr;
    
    // الكتلة الأساسية الحالية / Current basic block
    llvm::BasicBlock* currentBlock = nullptr;
    
    // جدول القيم المسماة (المتغيرات المحلية)
    // Named values table (local variables)
    std::unordered_map<std::string, llvm::Value*> namedValues;
    
    // جدول الدوال / Function table
    std::unordered_map<std::string, llvm::Function*> functions;
    
    // جدول الكتل الأساسية / Basic block table
    std::unordered_map<std::string, llvm::BasicBlock*> basicBlocks;
    
    // جدول القيم الثابتة / Constant values table
    std::unordered_map<std::string, llvm::Constant*> constants;
    
    // خريطة تخصيص الذاكرة / Allocation map
    std::unordered_map<std::string, llvm::AllocaInst*> allocas;
    
    // ================================================================
    // دعم الأصناف / Class Support
    // ================================================================
    
    // خريطة السجل/المتغير → اسم الصنف
    // Register/variable → class name mapping
    std::unordered_map<std::string, std::string> objectClassMap;
    
    // اسم الصنف → نوع الهيكل LLVM
    // Class name → LLVM struct type
    std::unordered_map<std::string, llvm::StructType*> classStructTypes;
    
    // اسم الصنف → أسماء الحقول بالترتيب
    // Class name → ordered field names
    std::unordered_map<std::string, std::vector<std::string>> classFieldNames;
    
    // اسم الصنف → اسم الصنف الأب (للوراثة)
    // Class name → parent class name (for inheritance)
    std::unordered_map<std::string, std::string> classParentMap;
    
    // اسم الصنف الحالي في حالة البناء (فارغ خارج الباني)
    // Current constructor class name (empty outside constructors)
    std::string currentConstructorClass;
};

// ============================================================================
// LLVMCodeGen - مولد كود LLVM الرئيسي
// LLVMCodeGen - Main LLVM Code Generator
// ============================================================================

/**
 * مولد كود LLVM يترجم SIR إلى LLVM IR
 * LLVM code generator translates SIR to LLVM IR
 * 
 * الوظائف الرئيسية (Main Functions):
 *   - initialize(): تهيئة السياق والوحدة
 *   - generate(): توليد LLVM IR من SIR
 *   - emitToFile(): حفظ LLVM IR إلى ملف
 * 
 * مثال (Example):
 *   ```cpp
 *   LLVMCodeGen gen;
 *   gen.initialize("program", "x86_64-linux-gnu");
 *   auto module = gen.generate(sir);
 *   gen.verify();
 *   gen.emitToFile("output.ll");
 *   ```
 */
class LLVMCodeGen {
public:
    // ========================================================================
    // Constructor & Destructor / المنشئ والمدمر
    // ========================================================================
    
    LLVMCodeGen();
    ~LLVMCodeGen();
    
    // منع النسخ / Prevent copying
    LLVMCodeGen(const LLVMCodeGen&) = delete;
    LLVMCodeGen& operator=(const LLVMCodeGen&) = delete;
    
    // ========================================================================
    // Initialization / التهيئة
    // ========================================================================
    
    /**
     * تهيئة مولد الكود
     * Initialize the code generator
     * 
     * @param moduleName اسم الوحدة / Module name
     * @param targetTriple سلسلة الهدف (مثل x86_64-linux-gnu) / Target triple
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize(const std::string& moduleName, const std::string& targetTriple);
    
    /**
     * تهيئة السياق فقط بدون هدف محدد
     * Initialize context only without specific target
     * 
     * @param moduleName اسم الوحدة / Module name
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize(const std::string& moduleName);
    
    // ========================================================================
    // Main Generation / التوليد الرئيسي
    // ========================================================================
    
    /**
     * توليد LLVM IR من وحدة SIR
     * Generate LLVM IR from SIR module
     * 
     * @param sirModule وحدة SIR المدخلة / Input SIR module
     * @return وحدة LLVM الناتجة / Resulting LLVM module
     */
    std::unique_ptr<llvm::Module> generate(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * الحصول على الوحدة الحالية
     * Get current module
     * 
     * @return مؤشر للوحدة / Pointer to module
     */
    llvm::Module* getModule() const { return module_.get(); }
    
    /**
     * التحقق من صحة الوحدة
     * Verify module correctness
     * 
     * @return true إذا كانت الوحدة صحيحة / true if module is valid
     */
    bool verify() const;
    
    // ========================================================================
    // Optimization / التحسين
    // ========================================================================
    
    /**
     * تعيين مستوى التحسين
     * Set optimization level
     * 
     * @param level مستوى التحسين (O0, O1, O2, O3, Os, Oz)
     */
    void setOptimizationLevel(sad::OptimizationLevel level);
    
    /**
     * الحصول على مستوى التحسين الحالي
     * Get current optimization level
     * 
     * @return مستوى التحسين / Optimization level
     */
    sad::OptimizationLevel getOptimizationLevel() const;
    
    /**
     * تحسين الوحدة الحالية
     * Optimize current module
     * 
     * @return true إذا نجح التحسين / true if optimization succeeded
     * 
     * @details
     * (AR) يطبق تحسينات LLVM حسب المستوى المحدد:
     *      - O0: بدون تحسين (للتنقيح)
     *      - O1: تحسينات أساسية
     *      - O2: تحسينات قياسية (موصى به)
     *      - O3: تحسينات عدوانية (أقصى أداء)
     *      - Os/Oz: تحسين للحجم
     * 
     * (EN) Applies LLVM optimizations based on level:
     *      - O0: No optimization (debug)
     *      - O1: Basic optimizations
     *      - O2: Standard optimizations (recommended)
     *      - O3: Aggressive optimizations (max performance)
     *      - Os/Oz: Size optimizations
     */
    bool optimize();
    
    /**
     * تمكين/تعطيل التحسين التلقائي بعد التوليد
     * Enable/disable automatic optimization after generation
     * 
     * @param enable true للتمكين / true to enable
     */
    void setAutoOptimize(bool enable) { autoOptimize_ = enable; }
    
    /**
     * الحصول على إحصائيات التحسين
     * Get optimization statistics
     * 
     * @return إحصائيات التحسين / Optimization statistics
     */
    const sad::OptimizationStats& getOptimizationStats() const;
    
    /**
     * طباعة إحصائيات التحسين
     * Print optimization statistics
     */
    void printOptimizationStats() const;
    
    // ========================================================================
    // Module Emission / إصدار الوحدة
    // ========================================================================
    
    /**
     * إصدار وحدة SIR كاملة
     * Emit complete SIR module
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitModule(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * معالجة الأصناف وإنشاء أنواع الهياكل
     * Pre-process classes and create struct types
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void preprocessClasses(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إصدار الدوال العامة / Global functions
     * Emit global functions
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إضافة دالة main كـ wrapper للدالة الرئيسية العربية
     * Add main wrapper for Arabic main function
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitMainWrapper(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إصدار المتغيرات العامة
     * Emit global variables
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitGlobalVariables(std::shared_ptr<SIRModule> sirModule);
    
    /**
     * إصدار الثوابت
     * Emit constants
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitConstants(std::shared_ptr<SIRModule> sirModule);
    
    // ========================================================================
    // Function Emission / إصدار الدوال
    // ========================================================================
    
    /**
     * إصدار دالة LLVM من دالة SIR
     * Emit LLVM function from SIR function
     * 
     * @param sirFunc دالة SIR / SIR function
     * @return دالة LLVM / LLVM function
     */
    llvm::Function* emitFunction(std::shared_ptr<SIRFunction> sirFunc);
    
    /**
     * إصدار جسم الدالة
     * Emit function body
     * 
     * @param sirFunc دالة SIR / SIR function
     * @param llvmFunc دالة LLVM / LLVM function
     */
    void emitFunctionBody(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc);
    
    /**
     * إصدار توقيع الدالة فقط (بدون جسم)
     * Emit function signature only (without body)
     * 
     * @param sirFunc دالة SIR / SIR function
     * @return دالة LLVM / LLVM function
     */
    llvm::Function* emitFunctionPrototype(std::shared_ptr<SIRFunction> sirFunc);
    
    /**
     * إصدار معاملات الدالة
     * Emit function parameters
     * 
     * @param sirFunc دالة SIR / SIR function
     * @param llvmFunc دالة LLVM / LLVM function
     */
    void emitFunctionParameters(std::shared_ptr<SIRFunction> sirFunc, llvm::Function* llvmFunc);
    
    // ========================================================================
    // Basic Block Emission / إصدار الكتل الأساسية
    // ========================================================================
    
    /**
     * إصدار كتلة أساسية
     * Emit basic block
     * 
     * @param sirBlock كتلة SIR الأساسية / SIR basic block
     * @param function الدالة المحتوية / Containing function
     * @return كتلة LLVM الأساسية / LLVM basic block
     */
    llvm::BasicBlock* emitBasicBlock(std::shared_ptr<SIRBasicBlock> sirBlock,
                                     llvm::Function* function);
    
    /**
     * إنشاء كتلة أساسية فارغة
     * Create empty basic block
     * 
     * @param name اسم الكتلة / Block name
     * @param function الدالة المحتوية / Containing function
     * @return كتلة LLVM الأساسية / LLVM basic block
     */
    llvm::BasicBlock* createBasicBlock(const std::string& name, llvm::Function* function);
    
    // ========================================================================
    // Instruction Emission / إصدار التعليمات
    // ========================================================================
    
    /**
     * إصدار تعليمة من SIR
     * Emit instruction from SIR
     * 
     * @param inst تعليمة SIR / SIR instruction
     * @return قيمة LLVM الناتجة / Resulting LLVM value
     */
    llvm::Value* emitInstruction(std::shared_ptr<SIRInstruction> inst);
    
    // ------------------------------------------------------------------------
    // Arithmetic Instructions / التعليمات الحسابية
    // ------------------------------------------------------------------------
    
    llvm::Value* emitAdd(std::shared_ptr<SIRInstruction> inst);      // جمع / Add
    llvm::Value* emitSub(std::shared_ptr<SIRInstruction> inst);      // طرح / Subtract
    llvm::Value* emitMul(std::shared_ptr<SIRInstruction> inst);      // ضرب / Multiply
    llvm::Value* emitDiv(std::shared_ptr<SIRInstruction> inst);      // قسمة / Divide
    llvm::Value* emitMod(std::shared_ptr<SIRInstruction> inst);      // باقي القسمة / Modulo
    llvm::Value* emitNeg(std::shared_ptr<SIRInstruction> inst);      // نفي / Negate
    
    // ------------------------------------------------------------------------
    // Bitwise Instructions / التعليمات الثنائية
    // ------------------------------------------------------------------------
    
    llvm::Value* emitAnd(std::shared_ptr<SIRInstruction> inst);      // AND ثنائي
    llvm::Value* emitOr(std::shared_ptr<SIRInstruction> inst);       // OR ثنائي
    llvm::Value* emitXor(std::shared_ptr<SIRInstruction> inst);      // XOR ثنائي
    llvm::Value* emitNot(std::shared_ptr<SIRInstruction> inst);      // NOT ثنائي
    llvm::Value* emitShl(std::shared_ptr<SIRInstruction> inst);      // إزاحة يسار / Shift left
    llvm::Value* emitShr(std::shared_ptr<SIRInstruction> inst);      // إزاحة يمين / Shift right
    
    // ------------------------------------------------------------------------
    // Comparison Instructions / تعليمات المقارنة
    // ------------------------------------------------------------------------
    
    llvm::Value* emitCmpEq(std::shared_ptr<SIRInstruction> inst);    // يساوي / Equal
    llvm::Value* emitCmpNe(std::shared_ptr<SIRInstruction> inst);    // لا يساوي / Not equal
    llvm::Value* emitCmpLt(std::shared_ptr<SIRInstruction> inst);    // أصغر من / Less than
    llvm::Value* emitCmpLe(std::shared_ptr<SIRInstruction> inst);    // أصغر أو يساوي / Less or equal
    llvm::Value* emitCmpGt(std::shared_ptr<SIRInstruction> inst);    // أكبر من / Greater than
    llvm::Value* emitCmpGe(std::shared_ptr<SIRInstruction> inst);    // أكبر أو يساوي / Greater or equal
    
    // ------------------------------------------------------------------------
    // Memory Instructions / تعليمات الذاكرة
    // ------------------------------------------------------------------------
    
    llvm::Value* emitLoad(std::shared_ptr<SIRInstruction> inst);     // تحميل / Load
    llvm::Value* emitStore(std::shared_ptr<SIRInstruction> inst);    // تخزين / Store
    llvm::Value* emitAlloca(std::shared_ptr<SIRInstruction> inst);   // تخصيص / Allocate
    llvm::Value* emitGEP(std::shared_ptr<SIRInstruction> inst);      // Get Element Ptr
    llvm::Value* emitMove(std::shared_ptr<SIRInstruction> inst);     // نقل / Move (register assignment)
    
    // ------------------------------------------------------------------------
    // Control Flow Instructions / تعليمات تدفق التحكم
    // ------------------------------------------------------------------------
    
    llvm::Value* emitBranch(std::shared_ptr<SIRInstruction> inst);           // فرع / Branch
    llvm::Value* emitCondBranch(std::shared_ptr<SIRInstruction> inst);       // فرع شرطي / Conditional branch
    llvm::Value* emitCall(std::shared_ptr<SIRInstruction> inst);             // استدعاء / Call
    llvm::Value* emitReturn(std::shared_ptr<SIRInstruction> inst);           // رجوع / Return
    llvm::Value* emitSwitch(std::shared_ptr<SIRInstruction> inst);           // تبديل / Switch
    
    // ------------------------------------------------------------------------
    // Builtin Functions / الدوال المضمنة
    // ------------------------------------------------------------------------
    
    llvm::Value* emitBuiltinPrint(std::shared_ptr<SIRInstruction> inst);     // اطبع / Print
    llvm::Value* emitBuiltinRead(std::shared_ptr<SIRInstruction> inst);      // اقرأ / Read/Input
    llvm::Value* emitStringConcat(std::shared_ptr<SIRInstruction> inst);     // دمج نص / String concat
    llvm::Value* emitStringCharAt(std::shared_ptr<SIRInstruction> inst);     // رمز_حرف / Char at index
    llvm::Value* emitStringCmp(std::shared_ptr<SIRInstruction> inst);        // مقارنة نصوص / String compare
    
    // (AR) دالة مساعدة: تحميل القيمة تلقائياً من alloca إذا لزم الأمر
    // (EN) Helper: Auto-load value from alloca pointer if needed
    llvm::Value* resolveValue(llvm::Value* val, SIRType sirType);
    
    llvm::Value* emitStringToI64(std::shared_ptr<SIRInstruction> inst);      // لرقم / String to I64
    llvm::Value* emitStringToF64(std::shared_ptr<SIRInstruction> inst);      // لعشري / String to F64
    llvm::Value* emitBuiltinSqrt(std::shared_ptr<SIRInstruction> inst);      // جذر / Sqrt
    llvm::Value* emitBuiltinPow(std::shared_ptr<SIRInstruction> inst);       // أس / Power
    llvm::Value* emitBuiltinAbs(std::shared_ptr<SIRInstruction> inst);       // مطلق / Abs
    llvm::Value* emitBuiltinRound(std::shared_ptr<SIRInstruction> inst);     // تقريب / Round
    llvm::Value* emitBuiltinFloor(std::shared_ptr<SIRInstruction> inst);     // أرضية / Floor
    llvm::Value* emitBuiltinCeil(std::shared_ptr<SIRInstruction> inst);      // سقف / Ceil
    llvm::Value* emitBuiltinSin(std::shared_ptr<SIRInstruction> inst);       // جيب / Sin
    llvm::Value* emitBuiltinCos(std::shared_ptr<SIRInstruction> inst);       // جيب_تمام / Cos
    llvm::Value* emitBuiltinTan(std::shared_ptr<SIRInstruction> inst);       // ظل / Tan
    
    // String Functions (12)
    llvm::Value* emitBuiltinStringLength(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringToLower(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringFind(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringReplace(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringTrim(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringSplit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringJoin(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinStringContains(std::shared_ptr<SIRInstruction> inst);
    
    // Array Functions (10)
    llvm::Value* emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArraySize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayIndexOf(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayContains(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayReverse(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArraySort(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayFirst(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArrayLast(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinArraySlice(std::shared_ptr<SIRInstruction> inst);
    
    // File I/O Functions (8)
    llvm::Value* emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst);
    
    // Utility Functions (4)
    llvm::Value* emitBuiltinRandom(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinSleep(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinExit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value* emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst);
    
    // ================================================================
    // عمليات برمجة أنظمة التشغيل — OS Development Operations
    // ================================================================
    llvm::Value* emitInlineAsm(std::shared_ptr<SIRInstruction> inst);       // تجميع مضمّن
    llvm::Value* emitPortWrite(std::shared_ptr<SIRInstruction> inst);       // منفذ_اكتب / outb/outw/outl
    llvm::Value* emitPortRead(std::shared_ptr<SIRInstruction> inst);        // منفذ_اقرأ / inb/inw/inl
    llvm::Value* emitMemWrite(std::shared_ptr<SIRInstruction> inst);        // ذاكرة_اكتب / poke
    llvm::Value* emitMemRead(std::shared_ptr<SIRInstruction> inst);         // ذاكرة_اقرأ / peek
    llvm::Value* emitInterrupt(std::shared_ptr<SIRInstruction> inst);       // مقاطعة / int
    llvm::Value* emitHalt(std::shared_ptr<SIRInstruction> inst);            // توقف / hlt
    llvm::Value* emitCli(std::shared_ptr<SIRInstruction> inst);             // تعطيل_مقاطعات / cli
    llvm::Value* emitSti(std::shared_ptr<SIRInstruction> inst);             // تفعيل_مقاطعات / sti
    llvm::Value* emitVgaWrite(std::shared_ptr<SIRInstruction> inst);        // شاشة_اكتب / vga_write
    llvm::Value* emitVgaClear(std::shared_ptr<SIRInstruction> inst);        // شاشة_امسح / vga_clear
    llvm::Value* emitMemCopy(std::shared_ptr<SIRInstruction> inst);         // انسخ_ذاكرة / memcpy
    llvm::Value* emitMemSet(std::shared_ptr<SIRInstruction> inst);          // املأ_ذاكرة / memset
    llvm::Value* emitAddrOf(std::shared_ptr<SIRInstruction> inst);          // عنوان / addr_of
    
    // ================================================================
    // عمليات Embedded المتقدمة — Advanced Embedded Operations (18)
    // ================================================================
    // Serial I/O (4)
    llvm::Value* emitSerialInit(std::shared_ptr<SIRInstruction> inst);      // تسلسلي_هيئ / serial_init
    llvm::Value* emitSerialWrite(std::shared_ptr<SIRInstruction> inst);     // تسلسلي_ارسل / serial_send
    llvm::Value* emitSerialRead(std::shared_ptr<SIRInstruction> inst);      // تسلسلي_استقبل / serial_receive
    llvm::Value* emitSerialReady(std::shared_ptr<SIRInstruction> inst);     // تسلسلي_جاهز / serial_ready
    // GPIO (3)
    llvm::Value* emitGpioWrite(std::shared_ptr<SIRInstruction> inst);       // منفذ_رقمي_اكتب / gpio_write
    llvm::Value* emitGpioRead(std::shared_ptr<SIRInstruction> inst);        // منفذ_رقمي_اقرأ / gpio_read
    llvm::Value* emitGpioMode(std::shared_ptr<SIRInstruction> inst);        // حدد_وضع_منفذ / gpio_mode
    // Timer (3)
    llvm::Value* emitTimerInit(std::shared_ptr<SIRInstruction> inst);       // مؤقت_هيئ / timer_init
    llvm::Value* emitTimerRead(std::shared_ptr<SIRInstruction> inst);       // مؤقت_قراءة / timer_read
    llvm::Value* emitTimerWait(std::shared_ptr<SIRInstruction> inst);       // مؤقت_انتظر / timer_wait
    // System Control (3)
    llvm::Value* emitReset(std::shared_ptr<SIRInstruction> inst);           // اعد_تشغيل / reset
    llvm::Value* emitCpuId(std::shared_ptr<SIRInstruction> inst);           // معرف_المعالج / cpuid
    llvm::Value* emitRdtsc(std::shared_ptr<SIRInstruction> inst);           // عداد_الدورات / rdtsc
    // Memory Barriers (3)
    llvm::Value* emitMfence(std::shared_ptr<SIRInstruction> inst);          // حاجز_ذاكرة / mfence
    llvm::Value* emitLfence(std::shared_ptr<SIRInstruction> inst);          // حاجز_قراءة / lfence
    llvm::Value* emitSfence(std::shared_ptr<SIRInstruction> inst);          // حاجز_كتابة / sfence
    // DMA (2)
    llvm::Value* emitDmaInit(std::shared_ptr<SIRInstruction> inst);         // نقل_مباشر_هيئ / dma_init
    llvm::Value* emitDmaStart(std::shared_ptr<SIRInstruction> inst);        // نقل_مباشر_ابدأ / dma_start
    
    // ================================================================
    // دوال الأمان — Security Functions (14)
    // ================================================================
    llvm::Value* emitBuiltinSecurityAssert(std::shared_ptr<SIRInstruction> inst);        // تأكد
    llvm::Value* emitBuiltinSecurityVerify(std::shared_ptr<SIRInstruction> inst);        // تحقق
    llvm::Value* emitBuiltinSecurityIsSafe(std::shared_ptr<SIRInstruction> inst);        // آمن
    llvm::Value* emitBuiltinSecurityPanic(std::shared_ptr<SIRInstruction> inst);         // ذعر
    llvm::Value* emitBuiltinSecurityHash(std::shared_ptr<SIRInstruction> inst);          // هاش
    llvm::Value* emitBuiltinSecurityEncrypt(std::shared_ptr<SIRInstruction> inst);       // شفّر
    llvm::Value* emitBuiltinSecurityDecrypt(std::shared_ptr<SIRInstruction> inst);       // فك_تشفير
    llvm::Value* emitBuiltinSecurityAssertType(std::shared_ptr<SIRInstruction> inst);    // تأكد_نوع
    llvm::Value* emitBuiltinSecurityAssertEqual(std::shared_ptr<SIRInstruction> inst);   // تأكد_مساواة
    llvm::Value* emitBuiltinSecurityAssertGreater(std::shared_ptr<SIRInstruction> inst); // تأكد_أكبر
    llvm::Value* emitBuiltinSecuritySanitize(std::shared_ptr<SIRInstruction> inst);      // نظّف
    llvm::Value* emitBuiltinSecurityTimestamp(std::shared_ptr<SIRInstruction> inst);     // وقت_الآن
    llvm::Value* emitBuiltinSecuritySecureRandom(std::shared_ptr<SIRInstruction> inst);  // عشوائي_آمن
    llvm::Value* emitBuiltinSecurityBase64Encode(std::shared_ptr<SIRInstruction> inst);  // ترميز_64
    
    // ================================================================
    // التكامل مع C/C++ — FFI Functions (20)
    // ================================================================
    llvm::Value* emitFFIPrintf(std::shared_ptr<SIRInstruction> inst);       // طباعة_تنسيق / printf
    llvm::Value* emitFFIMalloc(std::shared_ptr<SIRInstruction> inst);       // حجز / malloc
    llvm::Value* emitFFIFree(std::shared_ptr<SIRInstruction> inst);         // حرر / free
    llvm::Value* emitFFIRealloc(std::shared_ptr<SIRInstruction> inst);      // اعد_حجز / realloc
    llvm::Value* emitFFICalloc(std::shared_ptr<SIRInstruction> inst);       // حجز_صفري / calloc
    llvm::Value* emitFFIStrlen(std::shared_ptr<SIRInstruction> inst);       // طول_نص_س / strlen
    llvm::Value* emitFFIStrcpy(std::shared_ptr<SIRInstruction> inst);       // انسخ_نص_س / strcpy
    llvm::Value* emitFFIStrcmp(std::shared_ptr<SIRInstruction> inst);       // قارن_نص_س / strcmp
    llvm::Value* emitFFIStrcat(std::shared_ptr<SIRInstruction> inst);       // الحق_نص_س / strcat
    llvm::Value* emitFFIMemcpy(std::shared_ptr<SIRInstruction> inst);       // انسخ_ذاكرة_س / memcpy
    llvm::Value* emitFFIMemset(std::shared_ptr<SIRInstruction> inst);       // عبئ_ذاكرة_س / memset
    llvm::Value* emitFFIFopen(std::shared_ptr<SIRInstruction> inst);        // افتح_ملف_س / fopen
    llvm::Value* emitFFIFclose(std::shared_ptr<SIRInstruction> inst);       // اغلق_ملف_س / fclose
    llvm::Value* emitFFIFwrite(std::shared_ptr<SIRInstruction> inst);       // اكتب_ملف_س / fputs
    llvm::Value* emitFFIFread(std::shared_ptr<SIRInstruction> inst);        // اقرأ_ملف_س / fgets
    llvm::Value* emitFFISystem(std::shared_ptr<SIRInstruction> inst);       // نفذ_امر / system
    llvm::Value* emitFFIGetenv(std::shared_ptr<SIRInstruction> inst);       // قيمة_بيئة / getenv
    llvm::Value* emitFFIAtoi(std::shared_ptr<SIRInstruction> inst);         // نص_لعدد / atoi
    llvm::Value* emitFFIAtof(std::shared_ptr<SIRInstruction> inst);         // نص_لعشري / atof
    llvm::Value* emitFFISnprintf(std::shared_ptr<SIRInstruction> inst);     // تنسيق_نص / snprintf
    
    // ========================================================================
    // Async/Await & Concurrency / تعليمات التزامن
    // ========================================================================
    llvm::Value* emitAsyncSpawn(std::shared_ptr<SIRInstruction> inst);        // أنشئ_مهمة
    llvm::Value* emitAsyncAwait(std::shared_ptr<SIRInstruction> inst);        // انتظر_مهمة
    llvm::Value* emitAsyncYield(std::shared_ptr<SIRInstruction> inst);        // أنتج
    llvm::Value* emitAsyncSleep(std::shared_ptr<SIRInstruction> inst);        // نوم_غير_متزامن
    llvm::Value* emitAsyncCreateFuture(std::shared_ptr<SIRInstruction> inst); // أنشئ_مستقبل
    llvm::Value* emitAsyncResolveFuture(std::shared_ptr<SIRInstruction> inst);// أوفِ_مستقبل
    llvm::Value* emitAsyncGetFuture(std::shared_ptr<SIRInstruction> inst);    // احصل_مستقبل
    llvm::Value* emitAsyncCreateChannel(std::shared_ptr<SIRInstruction> inst);// أنشئ_قناة
    llvm::Value* emitAsyncChannelSend(std::shared_ptr<SIRInstruction> inst);  // أرسل_قناة
    llvm::Value* emitAsyncChannelRecv(std::shared_ptr<SIRInstruction> inst);  // استقبل_قناة
    llvm::Value* emitAsyncChannelClose(std::shared_ptr<SIRInstruction> inst); // أغلق_قناة
    llvm::Value* emitAsyncMutexCreate(std::shared_ptr<SIRInstruction> inst);  // أنشئ_قفل
    llvm::Value* emitAsyncMutexLock(std::shared_ptr<SIRInstruction> inst);    // اقفل
    llvm::Value* emitAsyncMutexUnlock(std::shared_ptr<SIRInstruction> inst);  // افتح_قفل
    llvm::Value* emitAsyncThreadSpawn(std::shared_ptr<SIRInstruction> inst);  // أنشئ_خيط
    llvm::Value* emitAsyncThreadJoin(std::shared_ptr<SIRInstruction> inst);   // انضم_خيط
    llvm::Value* emitAsyncAtomicLoad(std::shared_ptr<SIRInstruction> inst);   // حمّل_ذري
    llvm::Value* emitAsyncAtomicStore(std::shared_ptr<SIRInstruction> inst);  // خزّن_ذري
    llvm::Value* emitAsyncAtomicAdd(std::shared_ptr<SIRInstruction> inst);    // أضف_ذري
    llvm::Value* emitAsyncAtomicCAS(std::shared_ptr<SIRInstruction> inst);    // قارن_وبدّل
    llvm::Value* emitAsyncWaitAll(std::shared_ptr<SIRInstruction> inst);      // انتظر_الكل
    llvm::Value* emitAsyncWaitAny(std::shared_ptr<SIRInstruction> inst);      // انتظر_أي
    llvm::Value* emitAsyncSelect(std::shared_ptr<SIRInstruction> inst);       // اختر_قناة
    
    // Helper for declaring runtime functions
    llvm::Function* declareRuntimeFunction(const std::string& name, 
                                          llvm::Type* returnType,
                                          const std::vector<llvm::Type*>& argTypes);
    
    // Helper for resolving SIR operands to LLVM values (constants + registers)
    llvm::Value* resolveOperand(const SIROperand& operand);
    
    // ------------------------------------------------------------------------
    // Type Conversion Instructions / تعليمات تحويل الأنواع
    // ------------------------------------------------------------------------
    
    llvm::Value* emitCast(std::shared_ptr<SIRInstruction> inst);             // تحويل عام / Cast
    llvm::Value* emitBitCast(std::shared_ptr<SIRInstruction> inst);          // تحويل ثنائي / Bitcast
    llvm::Value* emitIntToPtr(std::shared_ptr<SIRInstruction> inst);         // عدد إلى مؤشر
    llvm::Value* emitPtrToInt(std::shared_ptr<SIRInstruction> inst);         // مؤشر إلى عدد
    llvm::Value* emitTrunc(std::shared_ptr<SIRInstruction> inst);            // اقتطاع / Truncate
    llvm::Value* emitZExt(std::shared_ptr<SIRInstruction> inst);             // توسيع بصفر / Zero extend
    llvm::Value* emitSExt(std::shared_ptr<SIRInstruction> inst);             // توسيع بإشارة / Sign extend
    
    // SIR Type Conversion opcodes / أكواد تحويل الأنواع
    llvm::Value* emitI64ToF64(std::shared_ptr<SIRInstruction> inst);         // صحيح → عشري
    llvm::Value* emitF64ToI64(std::shared_ptr<SIRInstruction> inst);         // عشري → صحيح
    llvm::Value* emitI64ToBool(std::shared_ptr<SIRInstruction> inst);        // صحيح → منطقي
    llvm::Value* emitBoolToI64(std::shared_ptr<SIRInstruction> inst);        // منطقي → صحيح
    llvm::Value* emitI64ToString(std::shared_ptr<SIRInstruction> inst);      // صحيح → نص
    llvm::Value* emitF64ToString(std::shared_ptr<SIRInstruction> inst);      // عشري → نص
    llvm::Value* emitBoolToString(std::shared_ptr<SIRInstruction> inst);     // منطقي → نص
    
    // ------------------------------------------------------------------------
    // OOP Instructions / تعليمات البرمجة الكائنية
    // ------------------------------------------------------------------------
    llvm::Value* emitObjectNew(std::shared_ptr<SIRInstruction> inst);        // إنشاء كائن
    llvm::Value* emitObjectGet(std::shared_ptr<SIRInstruction> inst);        // قراءة خاصية
    llvm::Value* emitObjectSet(std::shared_ptr<SIRInstruction> inst);        // تعيين خاصية
    llvm::Value* emitObjectCall(std::shared_ptr<SIRInstruction> inst);       // استدعاء طريقة
    llvm::Value* emitInstanceOf(std::shared_ptr<SIRInstruction> inst);       // تحقق النوع
    llvm::Value* emitObjectCast(std::shared_ptr<SIRInstruction> inst);       // تحويل كائن
    llvm::Value* emitClassDef(std::shared_ptr<SIRInstruction> inst);         // تعريف صنف
    llvm::Value* emitMethodDef(std::shared_ptr<SIRInstruction> inst);        // تعريف طريقة
    llvm::Value* emitFieldDef(std::shared_ptr<SIRInstruction> inst);         // تعريف حقل
    llvm::Value* emitConstructorCall(std::shared_ptr<SIRInstruction> inst);  // استدعاء منشئ
    
    // ------------------------------------------------------------------------
    // Missing Bitwise / عمليات ثنائية ناقصة
    // ------------------------------------------------------------------------
    llvm::Value* emitSar(std::shared_ptr<SIRInstruction> inst);              // إزاحة حسابية يمين
    llvm::Value* emitRol(std::shared_ptr<SIRInstruction> inst);              // دوران يسار
    
    // ------------------------------------------------------------------------
    // Missing Memory / عمليات ذاكرة ناقصة
    // ------------------------------------------------------------------------
    llvm::Value* emitCallIndirect(std::shared_ptr<SIRInstruction> inst);     // استدعاء غير مباشر
    llvm::Value* emitAllocHeap(std::shared_ptr<SIRInstruction> inst);        // تخصيص كومة
    llvm::Value* emitFreeMem(std::shared_ptr<SIRInstruction> inst);          // تحرير ذاكرة
    llvm::Value* emitAddr(std::shared_ptr<SIRInstruction> inst);             // عنوان متغير
    llvm::Value* emitPtrAdd(std::shared_ptr<SIRInstruction> inst);           // حساب مؤشرات
    llvm::Value* emitPtrCast(std::shared_ptr<SIRInstruction> inst);          // تحويل مؤشر
    
    // ------------------------------------------------------------------------
    // Array core / عمليات المصفوفات الأساسية
    // ------------------------------------------------------------------------
    llvm::Value* emitArrayNew(std::shared_ptr<SIRInstruction> inst);         // إنشاء مصفوفة
    llvm::Value* emitArrayGet(std::shared_ptr<SIRInstruction> inst);         // قراءة عنصر
    llvm::Value* emitArraySet(std::shared_ptr<SIRInstruction> inst);         // تعيين عنصر
    llvm::Value* emitArrayLen(std::shared_ptr<SIRInstruction> inst);         // طول مصفوفة
    
    // String core
    llvm::Value* emitStringNew(std::shared_ptr<SIRInstruction> inst);        // إنشاء نص
    
    // Builtin Extra
    llvm::Value* emitBuiltinMin(std::shared_ptr<SIRInstruction> inst);       // الأصغر
    llvm::Value* emitBuiltinMax(std::shared_ptr<SIRInstruction> inst);       // الأكبر
    llvm::Value* emitBuiltinAssert(std::shared_ptr<SIRInstruction> inst);    // تأكيد
    llvm::Value* emitBuiltinDebug(std::shared_ptr<SIRInstruction> inst);     // تنقيح
    
    // ------------------------------------------------------------------------
    // Aggregate Instructions / تعليمات التجميع
    // ------------------------------------------------------------------------
    
    llvm::Value* emitExtractValue(std::shared_ptr<SIRInstruction> inst);     // استخراج قيمة
    llvm::Value* emitInsertValue(std::shared_ptr<SIRInstruction> inst);      // إدراج قيمة
    llvm::Value* emitExtractElement(std::shared_ptr<SIRInstruction> inst);   // استخراج عنصر
    llvm::Value* emitInsertElement(std::shared_ptr<SIRInstruction> inst);    // إدراج عنصر
    
    // ------------------------------------------------------------------------
    // Phi & Select / فاي والاختيار
    // ------------------------------------------------------------------------
    
    llvm::Value* emitPhi(std::shared_ptr<SIRInstruction> inst);              // عقدة فاي / Phi node
    llvm::Value* emitSelect(std::shared_ptr<SIRInstruction> inst);           // اختيار / Select
    
    // ========================================================================
    // Type Conversion / تحويل الأنواع
    // ========================================================================
    
    /**
     * تحويل نوع Sad إلى نوع LLVM
     * Convert Sad type to LLVM type
     * 
     * @param sadType نوع Sad / Sad type
     * @return نوع LLVM / LLVM type
     */
    llvm::Type* convertType(std::shared_ptr<Type> sadType);
    
    /**
     * تحويل نوع دالة Sad إلى نوع دالة LLVM
     * Convert Sad function type to LLVM function type
     * 
     * @param returnType نوع الرجوع / Return type
     * @param paramTypes أنواع المعاملات / Parameter types
     * @param isVarArg هل الدالة متغيرة المعاملات / Is variadic
     * @return نوع دالة LLVM / LLVM function type
     */
    llvm::FunctionType* convertFunctionType(std::shared_ptr<Type> returnType,
                                           const std::vector<std::shared_ptr<Type>>& paramTypes,
                                           bool isVarArg = false);
    
    /**
     * الحصول على الأنواع الأساسية / Get primitive types
     */
    llvm::Type* getVoidType();                    // نوع فارغ / Void type
    llvm::Type* getInt1Type();                    // منطقي (1 bit) / Boolean
    llvm::Type* getInt8Type();                    // صحيح 8 بت / 8-bit integer
    llvm::Type* getInt16Type();                   // صحيح 16 بت / 16-bit integer
    llvm::Type* getInt32Type();                   // صحيح 32 بت / 32-bit integer
    llvm::Type* getInt64Type();                   // صحيح 64 بت / 64-bit integer
    llvm::Type* getFloatType();                   // عشري 32 بت / 32-bit float
    llvm::Type* getDoubleType();                  // عشري 64 بت / 64-bit double
    llvm::Type* getInt8PtrType();                 // مؤشر حرف / Char pointer (i8*)
    
    // ========================================================================
    // Constants / الثوابت
    // ========================================================================
    
    llvm::Constant* getConstantInt(int64_t value, int bits = 64);
    llvm::Constant* getConstantFloat(double value, bool isDouble = false);
    llvm::Constant* getConstantString(const std::string& value);
    llvm::Constant* getConstantBool(bool value);
    llvm::Constant* getNullPtr(llvm::Type* ptrType);
    
    // ========================================================================
    // Output / الإخراج
    // ========================================================================
    
    /**
     * حفظ LLVM IR إلى ملف نصي
     * Save LLVM IR to text file
     * 
     * @param filename اسم الملف / File name
     * @return true إذا نجح الحفظ / true if save succeeded
     */
    bool emitToFile(const std::string& filename) const;
    
    /**
     * حفظ LLVM IR إلى نص
     * Save LLVM IR to string
     * 
     * @return نص LLVM IR / LLVM IR text
     */
    std::string emitToString() const;
    
    /**
     * إصدار ملف assembly
     * Emit assembly file
     * 
     * @param filename اسم الملف / File name
     * @return true if successful
     */
    bool emitAssembly(const std::string& filename);

    /**
     * إصدار ملف assembly من وحدة خارجية
     * Emit assembly file from external module
     * 
     * @param filename اسم الملف / File name
     * @param module الوحدة / Module
     * @return true if successful
     */
    bool emitAssembly(const std::string& filename, llvm::Module* module);
    
    /**
     * إصدار ملف object
     * Emit object file
     * 
     * @param filename اسم الملف / File name
     * @return true if successful
     */
    bool emitObjectFile(const std::string& filename);

    /**
     * إصدار ملف object من وحدة خارجية
     * Emit object file from external module
     * 
     * @param filename اسم الملف / File name
     * @param module الوحدة / Module
     * @return true if successful
     */
    bool emitObjectFile(const std::string& filename, llvm::Module* module);
    
    /**
     * طباعة LLVM IR إلى stderr
     * Print LLVM IR to stderr
     */
    void dump() const;
    
    // ========================================================================
    // Error Handling / معالجة الأخطاء
    // ========================================================================
    
    /**
     * الإبلاغ عن خطأ
     * Report error
     * 
     * @param message رسالة الخطأ / Error message
     */
    void reportError(const std::string& message);
    
    /**
     * هل حدثت أخطاء؟
     * Has errors occurred?
     * 
     * @return true إذا حدثت أخطاء / true if errors occurred
     */
    bool hasErrors() const { return hasErrors_; }
    
    /**
     * الحصول على رسائل الأخطاء
     * Get error messages
     * 
     * @return قائمة الأخطاء / Error list
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
private:
    // ========================================================================
    // Private Members / الأعضاء الخاصة
    // ========================================================================
    
    // LLVM Context (سياق LLVM الرئيسي)
    std::unique_ptr<llvm::LLVMContext> context_;
    
    // LLVM Module (الوحدة)
    std::unique_ptr<llvm::Module> module_;
    
    // LLVM IR Builder (بناء التعليمات)
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    
    // Target Machine (الآلة الهدف)
    llvm::TargetMachine* targetMachine_;
    
    // Code Generation Context (سياق التوليد)
    CodeGenContext context_info_;
    
    // Type Mapper (محول الأنواع) - NEW
    std::unique_ptr<LLVMTypeMapper> typeMapper_;
    
    // Control Flow Manager (مدير تدفق التحكم) - NEW
    std::unique_ptr<LLVMControlFlow> controlFlow_;
    
    // Expression Builder (بناء التعابير) - NEW
    std::unique_ptr<LLVMExpressionBuilder> expressionBuilder_;
    
    // Memory Manager (مدير الذاكرة) - NEW
    std::unique_ptr<LLVMMemoryManager> memoryManager_;
    
    // Optimizer (محسّن LLVM) - NEW Phase 1.1.3
    std::unique_ptr<sad::LLVMOptimizer> optimizer_;
    
    // SIR Module reference for class info access
    // مرجع وحدة SIR للوصول لمعلومات الأصناف
    std::shared_ptr<SIRModule> sirModule_;
    
    // Optimization settings (إعدادات التحسين)
    sad::OptimizationLevel optimizationLevel_;  // مستوى التحسين / Optimization level
    bool autoOptimize_;                         // تحسين تلقائي / Auto optimize
    
    // Error tracking (تتبع الأخطاء)
    bool hasErrors_;
    std::vector<std::string> errors_;
    
    // ========================================================================
    // Helper Methods / دوال مساعدة
    // ========================================================================
    
    /**
     * الحصول على قيمة متغير
     * Get variable value
     */
    llvm::Value* getNamedValue(const std::string& name);
    
    /**
     * تخزين قيمة متغير
     * Store variable value
     */
    void setNamedValue(const std::string& name, llvm::Value* value);
    
    /**
     * إنشاء entry block alloca لمتغير
     * Create entry block alloca for variable
     */
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function,
                                             const std::string& varName,
                                             llvm::Type* type);
    
    /**
     * الحصول على الكتلة الحالية
     * Get current block
     */
    llvm::BasicBlock* getCurrentBlock() const { return context_info_.currentBlock; }
    
    /**
     * تعيين الكتلة الحالية
     * Set current block
     */
    void setCurrentBlock(llvm::BasicBlock* block) { context_info_.currentBlock = block; }
};

} // namespace LLVM
} // namespace Sad

#endif // SAD_LLVM_CODEGEN_H
