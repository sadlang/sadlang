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

namespace Sad {
namespace LLVM {

// ============================================================================
// Forward Declarations / التصريحات المسبقة
// ============================================================================

class SIRModule;
class SIRFunction;
class SIRBasicBlock;
class SIRInstruction;
class Type;

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
     * إصدار الدوال العامة / Global functions
     * Emit global functions
     * 
     * @param sirModule وحدة SIR / SIR module
     */
    void emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule);
    
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
    
    // ------------------------------------------------------------------------
    // Control Flow Instructions / تعليمات تدفق التحكم
    // ------------------------------------------------------------------------
    
    llvm::Value* emitBranch(std::shared_ptr<SIRInstruction> inst);           // فرع / Branch
    llvm::Value* emitCondBranch(std::shared_ptr<SIRInstruction> inst);       // فرع شرطي / Conditional branch
    llvm::Value* emitCall(std::shared_ptr<SIRInstruction> inst);             // استدعاء / Call
    llvm::Value* emitReturn(std::shared_ptr<SIRInstruction> inst);           // رجوع / Return
    llvm::Value* emitSwitch(std::shared_ptr<SIRInstruction> inst);           // تبديل / Switch
    
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
