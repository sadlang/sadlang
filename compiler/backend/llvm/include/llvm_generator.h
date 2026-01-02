// ============================================================================
// llvm_generator.h - مولد LLVM IR من Sad AST/SIR
// LLVM IR Generator from Sad AST/SIR
// ============================================================================
// الغرض: تحويل Sad AST أو SIR إلى LLVM IR للتجميع الفوري
// Purpose: Convert Sad AST or SIR to LLVM IR for JIT compilation
// 
// المصدر: مستوحى من compiler/backends/llvm/llvm_codegen.h
// Source: Inspired by compiler/backends/llvm/llvm_codegen.h
// ============================================================================

#ifndef SAD_LLVM_GENERATOR_H
#define SAD_LLVM_GENERATOR_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// LLVM Headers / رؤوس LLVM
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>

namespace Sad {

// Forward declarations - التصريحات المسبقة للأنواع في namespace آخر
// Forward declarations - for types in other namespaces
namespace AST {
    class ASTNode;      // من parser/ast/ast_node.h / From parser/ast/ast_node.h
}

namespace JIT {

// ============================================================================
// التصريحات المسبقة / Forward Declarations
// ============================================================================

class TypedASTNode;      // من type_checker/typed_ast.h / From type_checker/typed_ast.h

// ============================================================================
// LLVMGeneratorContext - سياق التوليد
// LLVMGeneratorContext - Generation Context
// ============================================================================

/**
 * سياق التوليد يحتوي على المعلومات أثناء التحويل
 * Generation context holds information during conversion
 */
struct LLVMGeneratorContext {
    // السياق الرئيسي / Main context
    llvm::LLVMContext* llvm_context;
    
    // البناء الحالي / Current builder
    llvm::IRBuilder<>* builder;
    
    // الوحدة الحالية / Current module
    llvm::Module* module;
    
    // الدالة الحالية / Current function
    llvm::Function* current_function;
    
    // جدول المتغيرات المحلية / Local variables table
    // key: اسم المتغير / variable name
    // value: llvm::Value* للمتغير / llvm::Value* for the variable
    std::unordered_map<std::string, llvm::Value*> named_values;
    
    // جدول الدوال / Functions table
    // key: اسم الدالة / function name
    // value: llvm::Function* / llvm::Function*
    std::unordered_map<std::string, llvm::Function*> functions;
    
    // المُنشئ / Constructor
    LLVMGeneratorContext(
        llvm::LLVMContext* ctx,
        llvm::IRBuilder<>* bldr,
        llvm::Module* mod
    )
        : llvm_context(ctx)
        , builder(bldr)
        , module(mod)
        , current_function(nullptr)
    {}
};

// ============================================================================
// LLVMGenerator - مولد LLVM IR الرئيسي
// LLVMGenerator - Main LLVM IR Generator
// ============================================================================

/**
 * مولد LLVM IR يحول Sad AST/SIR إلى LLVM IR
 * LLVM IR generator converts Sad AST/SIR to LLVM IR
 * 
 * الاستخدام / Usage:
 *   ```cpp
 *   LLVMGenerator gen;
 *   gen.initialize("my_module");
 *   auto module = gen.generateFromSource(source_code);
 *   auto ir_string = gen.getIRString();
 *   ```
 */
class LLVMGenerator {
public:
    // ========================================================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================================================
    
    /**
     * المُنشئ / Constructor
     */
    LLVMGenerator();
    
    /**
     * المُدمر / Destructor
     */
    ~LLVMGenerator();
    
    // منع النسخ / Prevent copying
    LLVMGenerator(const LLVMGenerator&) = delete;
    LLVMGenerator& operator=(const LLVMGenerator&) = delete;
    
    // ========================================================================
    // التهيئة / Initialization
    // ========================================================================
    
    /**
     * تهيئة المولد / Initialize generator
     * 
     * @param module_name اسم الوحدة / Module name
     * @return true إذا نجحت / true if succeeded
     */
    bool initialize(const std::string& module_name);
    
    /**
     * تهيئة المولد مع سياق موجود / Initialize with existing context
     * 
     * @param llvm_context سياق LLVM موجود / Existing LLVM context
     * @param module_name اسم الوحدة / Module name
     * @return true إذا نجحت / true if succeeded
     */
    bool initialize(llvm::LLVMContext* llvm_context, const std::string& module_name);
    
    /**
     * هل المولد مُهيأ؟ / Is generator initialized?
     */
    bool isInitialized() const;
    
    // ========================================================================
    // التوليد من الكود المصدري / Generation from Source Code
    // ========================================================================
    
    /**
     * توليد LLVM IR من كود مصدري Sad
     * Generate LLVM IR from Sad source code
     * 
     * @param source_code الكود المصدري / Source code
     * @return وحدة LLVM / LLVM module (أو nullptr عند الفشل / or nullptr on failure)
     * 
     * الخطوات / Steps:
     *   1. تحليل الكود (Lexer + Parser) / Parse code
     *   2. فحص الأنواع (Type Checker) / Type check
     *   3. توليد LLVM IR / Generate LLVM IR
     */
    std::unique_ptr<llvm::Module> generateFromSource(const std::string& source_code);
    
    /**
     * توليد LLVM IR من دالة واحدة
     * Generate LLVM IR from a single function
     * 
     * @param function_name اسم الدالة / Function name
     * @param source_code كود الدالة / Function source code
     * @return وحدة LLVM تحتوي على الدالة / LLVM module containing the function
     */
    std::unique_ptr<llvm::Module> generateFunction(
        const std::string& function_name,
        const std::string& source_code
    );
    
    // ========================================================================
    // التوليد من AST / Generation from AST
    // ========================================================================
    
    /**
     * توليد LLVM IR من AST
     * Generate LLVM IR from AST
     * 
     * @param ast عقدة AST الرئيسية / Root AST node
     * @return وحدة LLVM / LLVM module
     * 
     * ملاحظة: يجب أن تكون AST مفحوصة الأنواع (TypedAST)
     * Note: AST must be type-checked (TypedAST)
     */
    std::unique_ptr<llvm::Module> generateFromAST(std::shared_ptr<Sad::AST::ASTNode> ast);
    
    /**
     * توليد LLVM IR من TypedAST (بعد Type Checking)
     * Generate LLVM IR from TypedAST (after Type Checking)
     * 
     * @param typed_ast عقدة TypedAST الرئيسية / Root TypedAST node
     * @return وحدة LLVM / LLVM module
     */
    std::unique_ptr<llvm::Module> generateFromTypedAST(std::shared_ptr<TypedASTNode> typed_ast);
    
    // ========================================================================
    // الحصول على الوحدة / Get Module
    // ========================================================================
    
    /**
     * الحصول على الوحدة الحالية / Get current module
     */
    llvm::Module* getModule() const;
    
    /**
     * الحصول على السياق / Get context
     */
    llvm::LLVMContext* getContext() const;
    
    /**
     * الحصول على LLVM IR كنص / Get LLVM IR as string
     * 
     * @return LLVM IR بصيغة نصية / LLVM IR in textual format
     */
    std::string getIRString() const;
    
    // ========================================================================
    // التحقق والتحسين / Verification & Optimization
    // ========================================================================
    
    /**
     * التحقق من صحة الوحدة / Verify module correctness
     * 
     * @param error_message رسالة الخطأ (إن وجد) / Error message (if any)
     * @return true إذا كانت الوحدة صحيحة / true if module is valid
     */
    bool verify(std::string* error_message = nullptr) const;
    
    /**
     * تحسين الوحدة / Optimize module
     * 
     * @param optimization_level مستوى التحسين (0-3) / Optimization level (0-3)
     */
    void optimize(int optimization_level = 2);
    
    // ========================================================================
    // معلومات الأخطاء / Error Information
    // ========================================================================
    
    /**
     * الحصول على رسالة الخطأ الأخيرة / Get last error message
     */
    std::string getLastError() const;
    
    /**
     * هل حدث خطأ؟ / Did an error occur?
     */
    bool hasError() const;
    
    /**
     * مسح رسالة الخطأ / Clear error message
     */
    void clearError();

private:
    // ========================================================================
    // الحقول الخاصة / Private Fields
    // ========================================================================
    
    // السياق الرئيسي / Main context
    // ملاحظة: نمتلكه إذا أنشأناه، وإلا نستعير المؤشر فقط
    // Note: We own it if we created it, otherwise just borrow the pointer
    std::unique_ptr<llvm::LLVMContext> owned_context_;
    llvm::LLVMContext* context_;
    
    // الوحدة / Module
    std::unique_ptr<llvm::Module> module_;
    
    // البناء / Builder
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    
    // سياق التوليد / Generation context
    std::unique_ptr<LLVMGeneratorContext> gen_context_;
    
    // رسالة الخطأ الأخيرة / Last error message
    std::string last_error_;
    
    // حالة التهيئة / Initialization state
    bool initialized_;
    
    // ========================================================================
    // دوال مساعدة خاصة / Private Helper Functions
    // ========================================================================
    
    /**
     * تحويل نوع Sad إلى نوع LLVM
     * Convert Sad type to LLVM type
     * 
     * @param type_name اسم النوع في Sad / Type name in Sad
     * @return نوع LLVM المقابل / Corresponding LLVM type
     * 
     * المصدر: مستوحى من llvm_type_mapper.h:mapType()
     * Source: Inspired by llvm_type_mapper.h:mapType()
     */
    llvm::Type* mapType(const std::string& type_name);
    
    /**
     * توليد LLVM IR من تعبير / Generate LLVM IR from expression
     * 
     * @param expr عقدة التعبير / Expression node
     * @return قيمة LLVM / LLVM value
     */
    llvm::Value* generateExpression(Sad::AST::ASTNode* expr);
    
    /**
     * توليد LLVM IR من عبارة / Generate LLVM IR from statement
     * 
     * @param stmt عقدة العبارة / Statement node
     */
    void generateStatement(Sad::AST::ASTNode* stmt);
    
    /**
     * توليد LLVM IR من دالة / Generate LLVM IR from function
     * 
     * @param func_node عقدة الدالة / Function node
     * @return دالة LLVM / LLVM function
     */
    llvm::Function* generateFunctionDecl(Sad::AST::ASTNode* func_node);
    
    /**
     * تسجيل خطأ / Log error
     * 
     * @param message رسالة الخطأ / Error message
     */
    void logError(const std::string& message);
};

} // namespace JIT
} // namespace Sad

#endif // SAD_LLVM_GENERATOR_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
