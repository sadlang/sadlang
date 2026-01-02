// ============================================================================
// llvm_generator.cpp - تطبيق مولد LLVM IR
// LLVM IR Generator Implementation
// ============================================================================
// المصدر: قراءة من llvm_generator.h:1-334
// Source: Read from llvm_generator.h:1-334
// ============================================================================

#include "llvm_generator.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>

// إضافة رؤوس AST / Add AST headers
// المصدر: include/parser/ast/ - قراءة من ast_node.h, expressions.h, statements.h
// Source: include/parser/ast/ - Read from ast_node.h, expressions.h, statements.h
#include "../../../include/parser/ast/ast_node.h"
#include "../../../include/parser/ast/expressions.h"
#include "../../../include/parser/ast/statements.h"
#include "../../../include/parser/ast/declarations.h"
#include "../../../include/lexer/token.h"

// TODO: سيتم إضافة Lexer, Parser, TypeChecker عندما يكونوا جاهزين
// TODO: Will add Lexer, Parser, TypeChecker when they're ready
// #include "../../../include/lexer/lexer.h"
// #include "../../../include/parser/parser.h"
// #include "../../../include/type_checker/type_checker.h"

namespace Sad {
namespace JIT {

// ============================================================================
// المُنشئ والمُدمر / Constructor & Destructor
// ============================================================================
// المصدر: llvm_generator.h:100-105
// Source: llvm_generator.h:100-105

LLVMGenerator::LLVMGenerator()
    : owned_context_(nullptr)        // لن نمتلك السياق في البداية / Don't own context initially
    , context_(nullptr)               // مؤشر فارغ / Null pointer
    , module_(nullptr)                // لا توجد وحدة بعد / No module yet
    , builder_(nullptr)               // لا يوجد بناء بعد / No builder yet
    , gen_context_(nullptr)           // لا يوجد سياق توليد / No generation context
    , last_error_()                   // رسالة خطأ فارغة / Empty error
    , initialized_(false)             // غير مُهيأ / Not initialized
{
    // لا شيء إضافي / Nothing else
}

LLVMGenerator::~LLVMGenerator() {
    // التنظيف التلقائي عبر unique_ptr / Automatic cleanup via unique_ptr
    // سيتم حذف: owned_context_, module_, builder_, gen_context_
    // Will delete: owned_context_, module_, builder_, gen_context_
}

// ============================================================================
// التهيئة / Initialization
// ============================================================================
// المصدر: llvm_generator.h:107-127
// Source: llvm_generator.h:107-127

bool LLVMGenerator::initialize(const std::string& module_name) {
    // إنشاء سياق جديد نمتلكه / Create new context that we own
    // المصدر: llvm::LLVMContext من LLVM API
    // Source: llvm::LLVMContext from LLVM API
    owned_context_ = std::make_unique<llvm::LLVMContext>();
    context_ = owned_context_.get();
    
    // إنشاء الوحدة / Create module
    // المصدر: llvm::Module(name, context) من LLVM API
    // Source: llvm::Module(name, context) from LLVM API
    module_ = std::make_unique<llvm::Module>(module_name, *context_);
    
    // إنشاء البناء / Create builder
    // المصدر: llvm::IRBuilder<>(context) من LLVM API
    // Source: llvm::IRBuilder<>(context) from LLVM API
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
    
    // إنشاء سياق التوليد / Create generation context
    // المصدر: LLVMGeneratorContext من llvm_generator.h:45-73
    // Source: LLVMGeneratorContext from llvm_generator.h:45-73
    gen_context_ = std::make_unique<LLVMGeneratorContext>(
        context_,
        builder_.get(),
        module_.get()
    );
    
    initialized_ = true;
    return true;
}

bool LLVMGenerator::initialize(llvm::LLVMContext* llvm_context, const std::string& module_name) {
    if (!llvm_context) {
        logError("LLVM context is null");
        return false;
    }
    
    // نستعير السياق (لا نمتلكه) / Borrow context (don't own it)
    context_ = llvm_context;
    owned_context_ = nullptr; // لا نمتلك السياق / Don't own context
    
    // إنشاء الوحدة / Create module
    module_ = std::make_unique<llvm::Module>(module_name, *context_);
    
    // إنشاء البناء / Create builder
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
    
    // إنشاء سياق التوليد / Create generation context
    gen_context_ = std::make_unique<LLVMGeneratorContext>(
        context_,
        builder_.get(),
        module_.get()
    );
    
    initialized_ = true;
    return true;
}

bool LLVMGenerator::isInitialized() const {
    // المصدر: initialized_ من llvm_generator.h:292
    // Source: initialized_ from llvm_generator.h:292
    return initialized_;
}

// ============================================================================
// التوليد من الكود المصدري / Generation from Source Code
// ============================================================================
// المصدر: llvm_generator.h:129-174
// Source: llvm_generator.h:129-174

std::unique_ptr<llvm::Module> LLVMGenerator::generateFromSource(const std::string& source_code) {
    if (!initialized_) {
        logError("Generator not initialized");
        return nullptr;
    }
    
    clearError();
    
    // TODO: التحليل الكامل سيُضاف لاحقاً
    // TODO: Full parsing will be added later
    // الآن: إنشاء وحدة بسيطة للاختبار / For now: Create simple module for testing
    
    // إنشاء دالة main بسيطة / Create simple main function
    // نوع الدالة: i32 main() / Function type: i32 main()
    llvm::FunctionType* main_type = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),  // نوع الإرجاع: i32 / Return type: i32
        false                                // لا توجد معاملات متغيرة / No varargs
    );
    
    // إنشاء دالة main / Create main function
    llvm::Function* main_func = llvm::Function::Create(
        main_type,
        llvm::Function::ExternalLinkage,
        "main",
        module_.get()
    );
    
    // إنشاء كتلة أساسية / Create basic block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(
        *context_,
        "entry",
        main_func
    );
    
    builder_->SetInsertPoint(entry);
    
    // إرجاع 0 / Return 0
    builder_->CreateRet(llvm::ConstantInt::get(*context_, llvm::APInt(32, 0)));
    
    return std::move(module_);
}

std::unique_ptr<llvm::Module> LLVMGenerator::generateFunction(
    const std::string& function_name,
    const std::string& source_code
) {
    // لتبسيط التطوير: نستخدم نفس generateFromSource
    // For simplicity: Use same generateFromSource
    // في المستقبل: يمكن تحسينها لمعالجة دالة واحدة فقط
    // In future: Can optimize for single function handling
    
    return generateFromSource(source_code);
}

// ============================================================================
// التوليد من AST / Generation from AST
// ============================================================================
// المصدر: llvm_generator.h:176-210
// Source: llvm_generator.h:176-210

std::unique_ptr<llvm::Module> LLVMGenerator::generateFromAST(std::shared_ptr<Sad::AST::ASTNode> ast) {
    if (!initialized_) {
        logError("Generator not initialized");
        return nullptr;
    }
    
    if (!ast) {
        logError("AST is null");
        return nullptr;
    }
    
    clearError();
    
    // TODO: فحص الأنواع سيُضاف لاحقاً
    // TODO: Type checking will be added later
    
    // الآن: إرجاع وحدة بسيطة / For now: Return simple module
    return generateFromSource("");
}

std::unique_ptr<llvm::Module> LLVMGenerator::generateFromTypedAST(std::shared_ptr<TypedASTNode> typed_ast) {
    if (!initialized_) {
        logError("Generator not initialized");
        return nullptr;
    }
    
    if (!typed_ast) {
        logError("TypedAST is null");
        return nullptr;
    }
    
    clearError();
    
    // TODO: تنفيذ التوليد الفعلي من TypedAST
    // TODO: Implement actual generation from TypedAST
    // هذا placeholder للتطوير / This is a placeholder for development
    
    // في الوقت الحالي: إنشاء دالة بسيطة للاختبار
    // For now: Create simple function for testing
    
    // إنشاء نوع دالة: i32 main() / Create function type: i32 main()
    llvm::FunctionType* main_type = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(*context_),  // نوع الإرجاع: i32 / Return type: i32
        false                                // لا توجد معاملات متغيرة / No varargs
    );
    
    // إنشاء دالة main / Create main function
    llvm::Function* main_func = llvm::Function::Create(
        main_type,
        llvm::Function::ExternalLinkage,
        "main",
        module_.get()
    );
    
    // إنشاء كتلة أساسية / Create basic block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(
        *context_,
        "entry",
        main_func
    );
    
    builder_->SetInsertPoint(entry);
    
    // إرجاع 0 / Return 0
    builder_->CreateRet(llvm::ConstantInt::get(*context_, llvm::APInt(32, 0)));
    
    // التحقق من الوحدة / Verify module
    std::string verify_error;
    if (!verify(&verify_error)) {
        logError("Module verification failed: " + verify_error);
        return nullptr;
    }
    
    // إرجاع الوحدة / Return module
    // ملاحظة: ننقل الملكية / Note: Transfer ownership
    return std::move(module_);
}

// ============================================================================
// الحصول على الوحدة / Get Module
// ============================================================================
// المصدر: llvm_generator.h:212-226
// Source: llvm_generator.h:212-226

llvm::Module* LLVMGenerator::getModule() const {
    // المصدر: module_ من llvm_generator.h:283
    // Source: module_ from llvm_generator.h:283
    return module_.get();
}

llvm::LLVMContext* LLVMGenerator::getContext() const {
    // المصدر: context_ من llvm_generator.h:281
    // Source: context_ from llvm_generator.h:281
    return context_;
}

std::string LLVMGenerator::getIRString() const {
    if (!module_) {
        return ""; // لا توجد وحدة / No module
    }
    
    // تحويل الوحدة إلى نص / Convert module to string
    // المصدر: llvm::raw_string_ostream من LLVM API
    // Source: llvm::raw_string_ostream from LLVM API
    std::string ir_string;
    llvm::raw_string_ostream stream(ir_string);
    module_->print(stream, nullptr);
    stream.flush();
    
    return ir_string;
}

// ============================================================================
// التحقق والتحسين / Verification & Optimization
// ============================================================================
// المصدر: llvm_generator.h:228-243
// Source: llvm_generator.h:228-243

bool LLVMGenerator::verify(std::string* error_message) const {
    if (!module_) {
        if (error_message) {
            *error_message = "Module is null";
        }
        return false;
    }
    
    // التحقق من الوحدة / Verify module
    // المصدر: llvm::verifyModule() من LLVM API
    // Source: llvm::verifyModule() from LLVM API
    std::string errors;
    llvm::raw_string_ostream error_stream(errors);
    
    bool has_errors = llvm::verifyModule(*module_, &error_stream);
    error_stream.flush();
    
    if (has_errors && error_message) {
        *error_message = errors;
    }
    
    return !has_errors; // true إذا لا توجد أخطاء / true if no errors
}

void LLVMGenerator::optimize(int optimization_level) {
    // ============================================================================
    // اسم الدالة: optimize
    // مصدر التعريف: llvm_generator.h:238-243
    // التوقيع الكامل: void optimize(int optimization_level = 2);
    // المتغيرات المستخدمة:
    //   - module_: defined at llvm_generator.h:283 (std::unique_ptr<llvm::Module>)
    //   - context_: defined at llvm_generator.h:281 (llvm::LLVMContext*)
    // الدوال المستدعاة:
    //   - llvm::PassBuilder (LLVM PassBuilder API)
    //   - llvm::ModulePassManager (LLVM PassManager API)
    //   - llvm::FunctionPassManager (LLVM PassManager API)
    // ============================================================================
    
    if (!module_) {
        logError("Cannot optimize: Module is null");
        return;
    }
    
    // تحويل optimization_level إلى OptimizationLevel / Convert to OptimizationLevel
    // المصدر: llvm::OptimizationLevel من LLVM API
    // Source: llvm::OptimizationLevel from LLVM API
    llvm::OptimizationLevel opt_level;
    switch (optimization_level) {
        case 0: 
            opt_level = llvm::OptimizationLevel::O0; // لا تحسين / No optimization
            break;
        case 1: 
            opt_level = llvm::OptimizationLevel::O1; // تحسين أساسي / Basic optimization
            break;
        case 3: 
            opt_level = llvm::OptimizationLevel::O3; // تحسين قوي / Aggressive optimization
            break;
        case 2:
        default: 
            opt_level = llvm::OptimizationLevel::O2; // تحسين قياسي / Standard optimization
            break;
    }
    
    // إنشاء PassBuilder / Create PassBuilder
    // المصدر: llvm::PassBuilder من LLVM Passes API
    // Source: llvm::PassBuilder from LLVM Passes API
    llvm::PassBuilder pass_builder;
    
    // إنشاء AnalysisManagers / Create AnalysisManagers
    // المصدر: llvm::LoopAnalysisManager, llvm::FunctionAnalysisManager, etc. من LLVM API
    // Source: llvm::LoopAnalysisManager, llvm::FunctionAnalysisManager, etc. from LLVM API
    llvm::LoopAnalysisManager lam;
    llvm::FunctionAnalysisManager fam;
    llvm::CGSCCAnalysisManager cgam;
    llvm::ModuleAnalysisManager mam;
    
    // تسجيل Analysis passes / Register analysis passes
    // المصدر: PassBuilder::registerModuleAnalyses() من LLVM API
    // Source: PassBuilder::registerModuleAnalyses() from LLVM API
    pass_builder.registerModuleAnalyses(mam);
    pass_builder.registerCGSCCAnalyses(cgam);
    pass_builder.registerFunctionAnalyses(fam);
    pass_builder.registerLoopAnalyses(lam);
    pass_builder.crossRegisterProxies(lam, fam, cgam, mam);
    
    // بناء optimization pipeline / Build optimization pipeline
    // المصدر: PassBuilder::buildPerModuleDefaultPipeline() من LLVM API
    // Source: PassBuilder::buildPerModuleDefaultPipeline() from LLVM API
    llvm::ModulePassManager mpm = pass_builder.buildPerModuleDefaultPipeline(opt_level);
    
    // تشغيل optimization passes على الوحدة / Run optimization passes on module
    // المصدر: ModulePassManager::run() من LLVM API
    // Source: ModulePassManager::run() from LLVM API
    mpm.run(*module_, mam);
}

// ============================================================================
// معلومات الأخطاء / Error Information
// ============================================================================
// المصدر: llvm_generator.h:245-265
// Source: llvm_generator.h:245-265

std::string LLVMGenerator::getLastError() const {
    // المصدر: last_error_ من llvm_generator.h:289
    // Source: last_error_ from llvm_generator.h:289
    return last_error_;
}

bool LLVMGenerator::hasError() const {
    return !last_error_.empty();
}

void LLVMGenerator::clearError() {
    last_error_.clear();
}

// ============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ============================================================================
// المصدر: llvm_generator.h:267-334
// Source: llvm_generator.h:267-334

llvm::Type* LLVMGenerator::mapType(const std::string& type_name) {
    // تحويل أنواع Sad الأساسية إلى LLVM types
    // Convert Sad basic types to LLVM types
    // المصدر: مستوحى من llvm_type_mapper.h
    // Source: Inspired by llvm_type_mapper.h
    
    if (type_name == "عدد_صحيح" || type_name == "int") {
        return llvm::Type::getInt32Ty(*context_);
    } else if (type_name == "عدد_عشري" || type_name == "float") {
        return llvm::Type::getDoubleTy(*context_);
    } else if (type_name == "منطقي" || type_name == "bool") {
        return llvm::Type::getInt1Ty(*context_);
    } else if (type_name == "نص" || type_name == "string") {
        // نص = مؤشر لحرف / String = pointer to char
        // ملاحظة: في LLVM 15+، استخدم PointerType::getUnqual()
        // Note: In LLVM 15+, use PointerType::getUnqual()
        return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(*context_));
    } else if (type_name == "فارغ" || type_name == "void") {
        return llvm::Type::getVoidTy(*context_);
    }
    
    // نوع غير معروف: افتراضياً i32 / Unknown type: default to i32
    return llvm::Type::getInt32Ty(*context_);
}

llvm::Value* LLVMGenerator::generateExpression(Sad::AST::ASTNode* expr) {
    // توليد LLVM IR من تعبير Sad AST
    // Generate LLVM IR from Sad AST expression
    // المصدر: قراءة من expressions.h:1-300 و ast_node.h:1-176
    // Source: Read from expressions.h:1-300 and ast_node.h:1-176
    
    if (!expr) {
        logError("Expression is null");
        return nullptr;
    }
    
    // استخدام dynamic_cast للتحقق من نوع العقدة / Use dynamic_cast to check node type
    // المصدر: نمط Visitor من ast_visitor.h:1-150
    // Source: Visitor pattern from ast_visitor.h:1-150
    
    // 1. LiteralExpr - التعبير الحرفي / Literal Expression
    // المصدر: expressions.h:174-207 (class LiteralExpr)
    // Source: expressions.h:174-207 (class LiteralExpr)
    if (auto* literal = dynamic_cast<Sad::AST::LiteralExpr*>(expr)) {
        // الحصول على الرمز / Get token
        // المصدر: LiteralExpr::token من expressions.h:195
        // Source: LiteralExpr::token from expressions.h:195
        const Lexer::Token& tok = literal->token;
        Lexer::TokenType tok_type = tok.getType();
        std::string tok_value = tok.getValue();
        
        // تحويل بناءً على نوع الرمز / Convert based on token type
        // المصدر: TokenType من lexer/token.h
        // Source: TokenType from lexer/token.h
        if (tok_type == Lexer::TokenType::NUMBER_INTEGER) {
            // عدد صحيح / Integer number
            // المصدر: llvm::ConstantInt::get() من LLVM API
            // Source: llvm::ConstantInt::get() from LLVM API
            int64_t value = std::stoll(tok_value);
            return llvm::ConstantInt::get(*context_, llvm::APInt(32, value));
        } else if (tok_type == Lexer::TokenType::NUMBER_DOUBLE) {
            // عدد عشري / Float number
            // المصدر: llvm::ConstantFP::get() من LLVM API
            // Source: llvm::ConstantFP::get() from LLVM API
            double value = std::stod(tok_value);
            return llvm::ConstantFP::get(*context_, llvm::APFloat(value));
        } else if (tok_type == Lexer::TokenType::LITERAL_TRUE || tok_type == Lexer::TokenType::LITERAL_FALSE) {
            // قيمة منطقية / Boolean value
            // المصدر: llvm::ConstantInt::get() مع i1 من LLVM API
            // Source: llvm::ConstantInt::get() with i1 from LLVM API
            bool value = (tok_type == Lexer::TokenType::LITERAL_TRUE);
            return llvm::ConstantInt::get(*context_, llvm::APInt(1, value ? 1 : 0));
        } else if (tok_type == Lexer::TokenType::STRING_LITERAL) {
            // نص / String
            // المصدر: llvm::IRBuilder::CreateGlobalStringPtr() من LLVM API
            // Source: llvm::IRBuilder::CreateGlobalStringPtr() from LLVM API
            // إنشاء string global constant وإرجاع مؤشر له / Create global string constant and return pointer
            return builder_->CreateGlobalStringPtr(tok_value, ".str");
        }
        
        logError("Unknown literal type");
        return nullptr;
    }
    
    // 2. VariableExpr - تعبير المتغير / Variable Expression
    // المصدر: expressions.h:209-241 (class VariableExpr)
    // Source: expressions.h:209-241 (class VariableExpr)
    if (auto* var_expr = dynamic_cast<Sad::AST::VariableExpr*>(expr)) {
        // الحصول على اسم المتغير / Get variable name
        // المصدر: VariableExpr::name من expressions.h:221
        // Source: VariableExpr::name from expressions.h:221
        std::string var_name = var_expr->name;
        
        // البحث في جدول المتغيرات / Search in variables table
        // المصدر: LLVMGeneratorContext::named_values من llvm_generator.h:64-66
        // Source: LLVMGeneratorContext::named_values from llvm_generator.h:64-66
        auto it = gen_context_->named_values.find(var_name);
        if (it == gen_context_->named_values.end()) {
            logError("Undefined variable: " + var_name);
            return nullptr;
        }
        
        // إنشاء تعليمة load / Create load instruction
        // المصدر: llvm::IRBuilder::CreateLoad() من LLVM API
        // Source: llvm::IRBuilder::CreateLoad() from LLVM API
        llvm::Value* var_ptr = it->second;
        // في LLVM 18، لا توجد getPointerElementType، نستخدم getAllocatedType()
        // In LLVM 18, no getPointerElementType, use getAllocatedType() for AllocaInst
        llvm::Type* var_type = nullptr;
        if (auto* alloca_inst = llvm::dyn_cast<llvm::AllocaInst>(var_ptr)) {
            var_type = alloca_inst->getAllocatedType();
        } else {
            // في حالة عدم وجود alloca، نستخدم i32 كافتراضي
            // If no alloca, use i32 as default
            var_type = llvm::Type::getInt32Ty(*context_);
        }
        return builder_->CreateLoad(var_type, var_ptr, var_name);
    }
    
    // 3. BinaryExpr - التعبير الثنائي / Binary Expression
    // المصدر: expressions.h:24-70 (class BinaryExpr)
    // Source: expressions.h:24-70 (class BinaryExpr)
    if (auto* binary = dynamic_cast<Sad::AST::BinaryExpr*>(expr)) {
        // توليد المعاملين / Generate operands
        // المصدر: BinaryExpr::left و BinaryExpr::right من expressions.h:40-42
        // Source: BinaryExpr::left and BinaryExpr::right from expressions.h:40-42
        llvm::Value* left_val = generateExpression(binary->left.get());
        llvm::Value* right_val = generateExpression(binary->right.get());
        
        if (!left_val || !right_val) {
            return nullptr; // خطأ في أحد المعاملين / Error in one of operands
        }
        
        // الحصول على العامل / Get operator
        // المصدر: BinaryExpr::op من expressions.h:41
        // Source: BinaryExpr::op from expressions.h:41
        Lexer::TokenType op = binary->op;
        
        // تحويل العامل إلى تعليمة LLVM / Convert operator to LLVM instruction
        // المصدر: TokenType من lexer/token.h
        // Source: TokenType from lexer/token.h
        switch (op) {
            // عمليات حسابية / Arithmetic operations
            case Lexer::TokenType::OP_PLUS:
                // المصدر: llvm::IRBuilder::CreateAdd() من LLVM API
                // Source: llvm::IRBuilder::CreateAdd() from LLVM API
                return builder_->CreateAdd(left_val, right_val, "addtmp");
            case Lexer::TokenType::OP_MINUS:
                // المصدر: llvm::IRBuilder::CreateSub() من LLVM API
                // Source: llvm::IRBuilder::CreateSub() from LLVM API
                return builder_->CreateSub(left_val, right_val, "subtmp");
            case Lexer::TokenType::OP_MULTIPLY:
                // المصدر: llvm::IRBuilder::CreateMul() من LLVM API
                // Source: llvm::IRBuilder::CreateMul() from LLVM API
                return builder_->CreateMul(left_val, right_val, "multmp");
            case Lexer::TokenType::OP_DIVIDE:
                // المصدر: llvm::IRBuilder::CreateSDiv() من LLVM API
                // Source: llvm::IRBuilder::CreateSDiv() from LLVM API
                return builder_->CreateSDiv(left_val, right_val, "divtmp");
            case Lexer::TokenType::OP_MODULO:
                // المصدر: llvm::IRBuilder::CreateSRem() من LLVM API
                // Source: llvm::IRBuilder::CreateSRem() from LLVM API
                return builder_->CreateSRem(left_val, right_val, "modtmp");
                
            // عمليات المقارنة / Comparison operations
            case Lexer::TokenType::OP_EQUAL:
                // المصدر: llvm::IRBuilder::CreateICmpEQ() من LLVM API
                // Source: llvm::IRBuilder::CreateICmpEQ() from LLVM API
                return builder_->CreateICmpEQ(left_val, right_val, "cmpeq");
            case Lexer::TokenType::OP_NOT_EQUAL:
                // المصدر: llvm::IRBuilder::CreateICmpNE() من LLVM API
                // Source: llvm::IRBuilder::CreateICmpNE() from LLVM API
                return builder_->CreateICmpNE(left_val, right_val, "cmpne");
            case Lexer::TokenType::OP_LESS:
                // المصدر: llvm::IRBuilder::CreateICmpSLT() من LLVM API
                // Source: llvm::IRBuilder::CreateICmpSLT() from LLVM API
                return builder_->CreateICmpSLT(left_val, right_val, "cmplt");
            case Lexer::TokenType::OP_LESS_EQUAL:
                // المصدر: llvm::IRBuilder::CreateICmpSLE() من LLVM API
                // Source: llvm::IRBuilder::CreateICmpSLE() from LLVM API
                return builder_->CreateICmpSLE(left_val, right_val, "cmple");
            case Lexer::TokenType::OP_GREATER:
                // المصدر: llvm::IRBuilder::CreateICmpSGT() من LLVM API
                // Source: llvm::IRBuilder::CreateICmpSGT() from LLVM API
                return builder_->CreateICmpSGT(left_val, right_val, "cmpgt");
            case Lexer::TokenType::OP_GREATER_EQUAL:
                // المصدر: llvm::IRBuilder::CreateICmpSGE() من LLVM API
                // Source: llvm::IRBuilder::CreateICmpSGE() from LLVM API
                return builder_->CreateICmpSGE(left_val, right_val, "cmpge");
                
            // عمليات منطقية / Logical operations
            case Lexer::TokenType::OP_AND:
                // المصدر: llvm::IRBuilder::CreateAnd() من LLVM API
                // Source: llvm::IRBuilder::CreateAnd() from LLVM API
                return builder_->CreateAnd(left_val, right_val, "andtmp");
            case Lexer::TokenType::OP_OR:
                // المصدر: llvm::IRBuilder::CreateOr() من LLVM API
                // Source: llvm::IRBuilder::CreateOr() from LLVM API
                return builder_->CreateOr(left_val, right_val, "ortmp");
                
            default:
                logError("Unsupported binary operator");
                return nullptr;
        }
    }
    
    // 4. UnaryExpr - التعبير الأحادي / Unary Expression
    // المصدر: expressions.h:72-109 (class UnaryExpr)
    // Source: expressions.h:72-109 (class UnaryExpr)
    if (auto* unary = dynamic_cast<Sad::AST::UnaryExpr*>(expr)) {
        // توليد المعامل / Generate operand
        // المصدر: UnaryExpr::operand من expressions.h:82
        // Source: UnaryExpr::operand from expressions.h:82
        llvm::Value* operand_val = generateExpression(unary->operand.get());
        if (!operand_val) {
            return nullptr;
        }
        
        // الحصول على العامل / Get operator
        // المصدر: UnaryExpr::op من expressions.h:81
        // Source: UnaryExpr::op from expressions.h:81
        Lexer::TokenType op = unary->op;
        
        // تحويل العامل إلى تعليمة LLVM / Convert operator to LLVM instruction
        switch (op) {
            case Lexer::TokenType::OP_MINUS:
                // نفي حسابي / Arithmetic negation
                // المصدر: llvm::IRBuilder::CreateNeg() من LLVM API
                // Source: llvm::IRBuilder::CreateNeg() from LLVM API
                return builder_->CreateNeg(operand_val, "negtmp");
            case Lexer::TokenType::OP_NOT:
                // نفي منطقي / Logical negation
                // المصدر: llvm::IRBuilder::CreateNot() من LLVM API
                // Source: llvm::IRBuilder::CreateNot() from LLVM API
                return builder_->CreateNot(operand_val, "nottmp");
            case Lexer::TokenType::OP_PLUS:
                // إشارة موجب (لا تفعل شيء) / Positive sign (do nothing)
                return operand_val;
            default:
                logError("Unsupported unary operator");
                return nullptr;
        }
    }
    
    // 5. AssignExpr - تعبير الإسناد / Assignment Expression
    // المصدر: expressions.h:243-275 (class AssignExpr)
    // Source: expressions.h:243-275 (class AssignExpr)
    if (auto* assign = dynamic_cast<Sad::AST::AssignExpr*>(expr)) {
        // الحصول على اسم المتغير / Get variable name
        // المصدر: AssignExpr::name من expressions.h:255
        // Source: AssignExpr::name from expressions.h:255
        std::string var_name = assign->name;
        
        // البحث في جدول المتغيرات / Search in variables table
        // المصدر: LLVMGeneratorContext::named_values من llvm_generator.h:64-66
        // Source: LLVMGeneratorContext::named_values from llvm_generator.h:64-66
        auto it = gen_context_->named_values.find(var_name);
        if (it == gen_context_->named_values.end()) {
            logError("Undefined variable in assignment: " + var_name);
            return nullptr;
        }
        
        // توليد قيمة الإسناد / Generate assignment value
        // المصدر: AssignExpr::value من expressions.h:256
        // Source: AssignExpr::value from expressions.h:256
        llvm::Value* new_value = generateExpression(assign->value.get());
        if (!new_value) {
            return nullptr;
        }
        
        // حفظ القيمة الجديدة / Store new value
        // المصدر: llvm::IRBuilder::CreateStore() من LLVM API
        // Source: llvm::IRBuilder::CreateStore() from LLVM API
        builder_->CreateStore(new_value, it->second);
        return new_value;
    }
    
    // نوع تعبير غير مدعوم حالياً / Unsupported expression type currently
    logError("Unsupported expression type");
    return nullptr;
}

void LLVMGenerator::generateStatement(Sad::AST::ASTNode* stmt) {
    // توليد LLVM IR من عبارة Sad AST
    // Generate LLVM IR from Sad AST statement
    // المصدر: قراءة من statements.h:1-200 و ast_node.h:1-176
    // Source: Read from statements.h:1-200 and ast_node.h:1-176
    
    if (!stmt) {
        logError("Statement is null");
        return;
    }
    
    // استخدام dynamic_cast للتحقق من نوع العقدة / Use dynamic_cast to check node type
    // المصدر: نمط Visitor من ast_visitor.h:1-150
    // Source: Visitor pattern from ast_visitor.h:1-150
    
    // 1. ExprStmt - جملة التعبير / Expression Statement
    // المصدر: statements.h:24-56 (class ExprStmt)
    // Source: statements.h:24-56 (class ExprStmt)
    if (auto* expr_stmt = dynamic_cast<Sad::AST::ExprStmt*>(stmt)) {
        // توليد التعبير / Generate expression
        // المصدر: ExprStmt::expression من statements.h:37
        // Source: ExprStmt::expression from statements.h:37
        generateExpression(expr_stmt->expression.get());
        return;
    }
    
    // 2. VarDeclStmt - جملة تصريح المتغير / Variable Declaration Statement
    // المصدر: statements.h:58-101 (class VarDeclStmt)
    // Source: statements.h:58-101 (class VarDeclStmt)
    if (auto* var_decl = dynamic_cast<Sad::AST::VarDeclStmt*>(stmt)) {
        // الحصول على اسم المتغير / Get variable name
        // المصدر: VarDeclStmt::name من statements.h:71
        // Source: VarDeclStmt::name from statements.h:71
        std::string var_name = var_decl->name;
        
        // الحصول على نوع المتغير / Get variable type
        // المصدر: VarDeclStmt::type من statements.h:72
        // Source: VarDeclStmt::type from statements.h:72
        Data::DataType var_type = var_decl->type;
        
        // تحويل نوع Sad إلى نوع LLVM / Convert Sad type to LLVM type
        // المصدر: mapType() من llvm_generator.h:305-313
        // Source: mapType() from llvm_generator.h:305-313
        llvm::Type* llvm_type = nullptr;
        switch (var_type) {
            case Data::DataType::INTEGER:
                llvm_type = llvm::Type::getInt32Ty(*context_);
                break;
            case Data::DataType::FLOAT:
                llvm_type = llvm::Type::getDoubleTy(*context_);
                break;
            case Data::DataType::BOOLEAN:
                llvm_type = llvm::Type::getInt1Ty(*context_);
                break;
            default:
                logError("Unsupported variable type");
                return;
        }
        
        // إنشاء alloca للمتغير / Create alloca for variable
        // المصدر: llvm::IRBuilder::CreateAlloca() من LLVM API
        // Source: llvm::IRBuilder::CreateAlloca() from LLVM API
        llvm::AllocaInst* alloca_inst = builder_->CreateAlloca(llvm_type, nullptr, var_name);
        
        // حفظ في جدول المتغيرات / Save in variables table
        // المصدر: LLVMGeneratorContext::named_values من llvm_generator.h:64-66
        // Source: LLVMGeneratorContext::named_values from llvm_generator.h:64-66
        gen_context_->named_values[var_name] = alloca_inst;
        
        // إذا كان هناك قيمة أولية / If there's an initial value
        // المصدر: VarDeclStmt::initializer من statements.h:73
        // Source: VarDeclStmt::initializer from statements.h:73
        if (var_decl->initializer) {
            // توليد قيمة الإعداد / Generate initializer value
            llvm::Value* init_val = generateExpression(var_decl->initializer.get());
            if (init_val) {
                // حفظ القيمة في المتغير / Store value in variable
                // المصدر: llvm::IRBuilder::CreateStore() من LLVM API
                // Source: llvm::IRBuilder::CreateStore() from LLVM API
                builder_->CreateStore(init_val, alloca_inst);
            }
        }
        
        return;
    }
    
    // 3. ReturnStmt - جملة الإرجاع / Return Statement
    // المصدر: statements.h:267-291 (class ReturnStmt)
    // Source: statements.h:267-291 (class ReturnStmt)
    if (auto* ret_stmt = dynamic_cast<Sad::AST::ReturnStmt*>(stmt)) {
        // الحصول على قيمة الإرجاع / Get return value
        // المصدر: ReturnStmt::value من statements.h:281
        // Source: ReturnStmt::value from statements.h:281
        if (ret_stmt->value) {
            // توليد قيمة الإرجاع / Generate return value
            llvm::Value* ret_val = generateExpression(ret_stmt->value.get());
            if (ret_val) {
                // إرجاع القيمة / Return value
                // المصدر: llvm::IRBuilder::CreateRet() من LLVM API
                // Source: llvm::IRBuilder::CreateRet() from LLVM API
                builder_->CreateRet(ret_val);
            }
        } else {
            // إرجاع void / Return void
            // المصدر: llvm::IRBuilder::CreateRetVoid() من LLVM API
            // Source: llvm::IRBuilder::CreateRetVoid() from LLVM API
            builder_->CreateRetVoid();
        }
        return;
    }
    
    // 4. BlockStmt - جملة الكتلة / Block Statement
    // المصدر: statements.h:417-443 (class BlockStmt)
    // Source: statements.h:417-443 (class BlockStmt)
    if (auto* block = dynamic_cast<Sad::AST::BlockStmt*>(stmt)) {
        // توليد كل جملة في الكتلة / Generate each statement in block
        // المصدر: BlockStmt::statements من statements.h:429
        // Source: BlockStmt::statements from statements.h:429
        for (const auto& statement : block->statements) {
            generateStatement(statement.get());
        }
        return;
    }
    
    // 5. IfStmt - جملة إذا / If Statement
    // المصدر: statements.h:103-141 (class IfStmt)
    // Source: statements.h:103-141 (class IfStmt)
    if (auto* if_stmt = dynamic_cast<Sad::AST::IfStmt*>(stmt)) {
        // توليد شرط if / Generate if condition
        // المصدر: IfStmt::condition من statements.h:118
        // Source: IfStmt::condition from statements.h:118
        llvm::Value* cond_val = generateExpression(if_stmt->condition.get());
        if (!cond_val) {
            return;
        }
        
        // الحصول على الدالة الحالية / Get current function
        // المصدر: LLVMGeneratorContext::current_function من llvm_generator.h:59
        // Source: LLVMGeneratorContext::current_function from llvm_generator.h:59
        llvm::Function* func = gen_context_->current_function;
        if (!func) {
            logError("If statement outside function");
            return;
        }
        
        // إنشاء basic blocks للفروع / Create basic blocks for branches
        // المصدر: llvm::BasicBlock::Create() من LLVM API
        // Source: llvm::BasicBlock::Create() from LLVM API
        llvm::BasicBlock* then_bb = llvm::BasicBlock::Create(*context_, "then", func);
        llvm::BasicBlock* else_bb = if_stmt->elseBranch ? llvm::BasicBlock::Create(*context_, "else") : nullptr;
        llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*context_, "ifcont");
        
        // إنشاء فرع شرطي / Create conditional branch
        // المصدر: llvm::IRBuilder::CreateCondBr() من LLVM API
        // Source: llvm::IRBuilder::CreateCondBr() from LLVM API
        if (else_bb) {
            builder_->CreateCondBr(cond_val, then_bb, else_bb);
        } else {
            builder_->CreateCondBr(cond_val, then_bb, merge_bb);
        }
        
        // توليد then branch / Generate then branch
        // المصدر: IfStmt::thenBranch من statements.h:119
        // Source: IfStmt::thenBranch from statements.h:119
        builder_->SetInsertPoint(then_bb);
        generateStatement(if_stmt->thenBranch.get());
        // إضافة branch إلى merge إذا لم ينتهي البلوك بـ terminator / Add branch to merge if block doesn't end with terminator
        if (!then_bb->getTerminator()) {
            builder_->CreateBr(merge_bb);
        }
        
        // توليد else branch إن وجد / Generate else branch if exists
        // المصدر: IfStmt::elseBranch من statements.h:120
        // Source: IfStmt::elseBranch from statements.h:120
        if (else_bb) {
            func->insert(func->end(), else_bb);
            builder_->SetInsertPoint(else_bb);
            generateStatement(if_stmt->elseBranch.get());
            if (!else_bb->getTerminator()) {
                builder_->CreateBr(merge_bb);
            }
        }
        
        // متابعة في merge block / Continue in merge block
        func->insert(func->end(), merge_bb);
        builder_->SetInsertPoint(merge_bb);
        return;
    }
    
    // 6. WhileStmt - جملة بينما / While Statement
    // المصدر: statements.h:143-174 (class WhileStmt)
    // Source: statements.h:143-174 (class WhileStmt)
    if (auto* while_stmt = dynamic_cast<Sad::AST::WhileStmt*>(stmt)) {
        // الحصول على الدالة الحالية / Get current function
        llvm::Function* func = gen_context_->current_function;
        if (!func) {
            logError("While statement outside function");
            return;
        }
        
        // إنشاء basic blocks للحلقة / Create basic blocks for loop
        // المصدر: llvm::BasicBlock::Create() من LLVM API
        // Source: llvm::BasicBlock::Create() from LLVM API
        llvm::BasicBlock* loop_cond = llvm::BasicBlock::Create(*context_, "loopcond", func);
        llvm::BasicBlock* loop_body = llvm::BasicBlock::Create(*context_, "loopbody");
        llvm::BasicBlock* loop_end = llvm::BasicBlock::Create(*context_, "loopend");
        
        // قفز إلى loop condition / Jump to loop condition
        builder_->CreateBr(loop_cond);
        
        // توليد شرط الحلقة / Generate loop condition
        // المصدر: WhileStmt::condition من statements.h:155
        // Source: WhileStmt::condition from statements.h:155
        builder_->SetInsertPoint(loop_cond);
        llvm::Value* cond_val = generateExpression(while_stmt->condition.get());
        if (!cond_val) {
            return;
        }
        
        // فرع شرطي: إذا صح ادخل loop_body، وإلا اذهب إلى loop_end / Conditional branch
        builder_->CreateCondBr(cond_val, loop_body, loop_end);
        
        // توليد جسم الحلقة / Generate loop body
        // المصدر: WhileStmt::body من statements.h:156
        // Source: WhileStmt::body from statements.h:156
        func->insert(func->end(), loop_body);
        builder_->SetInsertPoint(loop_body);
        generateStatement(while_stmt->body.get());
        
        // قفز بالعودة إلى شرط الحلقة / Jump back to loop condition
        if (!loop_body->getTerminator()) {
            builder_->CreateBr(loop_cond);
        }
        
        // متابعة بعد الحلقة / Continue after loop
        func->insert(func->end(), loop_end);
        builder_->SetInsertPoint(loop_end);
        return;
    }
    
    // 3. ReturnStmt - جملة الإرجاع / Return Statement
    // المصدر: statements.h:250-289 (class ReturnStmt)
    // Source: statements.h:250-289 (class ReturnStmt)
    if (auto* ret_stmt = dynamic_cast<Sad::AST::ReturnStmt*>(stmt)) {
        // المصدر: ReturnStmt::value من statements.h:264
        // Source: ReturnStmt::value from statements.h:264
        if (ret_stmt->value) {
            // إرجاع مع قيمة / Return with value
            llvm::Value* ret_val = generateExpression(ret_stmt->value.get());
            if (ret_val) {
                // المصدر: llvm::IRBuilder::CreateRet() من LLVM API
                // Source: llvm::IRBuilder::CreateRet() from LLVM API
                builder_->CreateRet(ret_val);
            }
        } else {
            // إرجاع بدون قيمة (void) / Return without value (void)
            // المصدر: llvm::IRBuilder::CreateRetVoid() من LLVM API
            // Source: llvm::IRBuilder::CreateRetVoid() from LLVM API
            builder_->CreateRetVoid();
        }
        return;
    }
    
    // 4. IfStmt - جملة الشرط / If Statement
    // المصدر: statements.h:103-137 (class IfStmt)
    // Source: statements.h:103-137 (class IfStmt)
    if (auto* if_stmt = dynamic_cast<Sad::AST::IfStmt*>(stmt)) {
        // توليد شرط الـ if / Generate if condition
        // المصدر: IfStmt::condition من statements.h:117
        // Source: IfStmt::condition from statements.h:117
        llvm::Value* cond_val = generateExpression(if_stmt->condition.get());
        if (!cond_val) {
            return;
        }
        
        // الحصول على الدالة الحالية / Get current function
        // المصدر: LLVMGeneratorContext::current_function من llvm_generator.h:58
        // Source: LLVMGeneratorContext::current_function from llvm_generator.h:58
        llvm::Function* func = gen_context_->current_function;
        if (!func) {
            logError("If statement outside function");
            return;
        }
        
        // إنشاء basic blocks / Create basic blocks
        // المصدر: llvm::BasicBlock::Create() من LLVM API
        // Source: llvm::BasicBlock::Create() from LLVM API
        llvm::BasicBlock* then_bb = llvm::BasicBlock::Create(*context_, "then", func);
        llvm::BasicBlock* else_bb = if_stmt->elseBranch ? 
            llvm::BasicBlock::Create(*context_, "else") : nullptr;
        llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(*context_, "ifcont");
        
        // إنشاء branch conditional / Create conditional branch
        // المصدر: llvm::IRBuilder::CreateCondBr() من LLVM API
        // Source: llvm::IRBuilder::CreateCondBr() from LLVM API
        if (else_bb) {
            builder_->CreateCondBr(cond_val, then_bb, else_bb);
        } else {
            builder_->CreateCondBr(cond_val, then_bb, merge_bb);
        }
        
        // توليد then branch / Generate then branch
        builder_->SetInsertPoint(then_bb);
        // المصدر: IfStmt::thenBranch من statements.h:118
        // Source: IfStmt::thenBranch from statements.h:118
        generateStatement(if_stmt->thenBranch.get());
        // التأكد من عدم وجود terminator / Check no terminator exists
        if (!builder_->GetInsertBlock()->getTerminator()) {
            builder_->CreateBr(merge_bb);
        }
        
        // توليد else branch إن وُجد / Generate else branch if exists
        if (else_bb) {
            else_bb->insertInto(func);
            builder_->SetInsertPoint(else_bb);
            // المصدر: IfStmt::elseBranch من statements.h:119
            // Source: IfStmt::elseBranch from statements.h:119
            generateStatement(if_stmt->elseBranch.get());
            if (!builder_->GetInsertBlock()->getTerminator()) {
                builder_->CreateBr(merge_bb);
            }
        }
        
        // الانتقال إلى merge block / Move to merge block
        merge_bb->insertInto(func);
        builder_->SetInsertPoint(merge_bb);
        return;
    }
    
    // 5. WhileStmt - جملة بينما / While Statement
    // المصدر: statements.h:139-177 (class WhileStmt)
    // Source: statements.h:139-177 (class WhileStmt)
    if (auto* while_stmt = dynamic_cast<Sad::AST::WhileStmt*>(stmt)) {
        // الحصول على الدالة الحالية / Get current function
        llvm::Function* func = gen_context_->current_function;
        if (!func) {
            logError("While statement outside function");
            return;
        }
        
        // إنشاء basic blocks / Create basic blocks
        llvm::BasicBlock* cond_bb = llvm::BasicBlock::Create(*context_, "whilecond", func);
        llvm::BasicBlock* body_bb = llvm::BasicBlock::Create(*context_, "whilebody");
        llvm::BasicBlock* after_bb = llvm::BasicBlock::Create(*context_, "afterwhile");
        
        // القفز إلى condition / Jump to condition
        builder_->CreateBr(cond_bb);
        
        // توليد condition / Generate condition
        builder_->SetInsertPoint(cond_bb);
        // المصدر: WhileStmt::condition من statements.h:157
        // Source: WhileStmt::condition from statements.h:157
        llvm::Value* cond_val = generateExpression(while_stmt->condition.get());
        if (!cond_val) {
            return;
        }
        builder_->CreateCondBr(cond_val, body_bb, after_bb);
        
        // توليد body / Generate body
        body_bb->insertInto(func);
        builder_->SetInsertPoint(body_bb);
        // المصدر: WhileStmt::body من statements.h:158
        // Source: WhileStmt::body from statements.h:158
        generateStatement(while_stmt->body.get());
        if (!builder_->GetInsertBlock()->getTerminator()) {
            builder_->CreateBr(cond_bb);
        }
        
        // الانتقال إلى after block / Move to after block
        after_bb->insertInto(func);
        builder_->SetInsertPoint(after_bb);
        return;
    }
    
    // 6. ForStmt - جملة لكل / For Statement
    // المصدر: statements.h:179-217 (class ForStmt)
    // Source: statements.h:179-217 (class ForStmt)
    if (auto* for_stmt = dynamic_cast<Sad::AST::ForStmt*>(stmt)) {
        // الحصول على الدالة الحالية / Get current function
        llvm::Function* func = gen_context_->current_function;
        if (!func) {
            logError("For statement outside function");
            return;
        }
        
        // توليد initializer / Generate initializer
        // المصدر: ForStmt::initializer من statements.h:191
        // Source: ForStmt::initializer from statements.h:191
        if (for_stmt->initializer) {
            generateStatement(for_stmt->initializer.get());
        }
        
        // إنشاء basic blocks / Create basic blocks
        llvm::BasicBlock* cond_bb = llvm::BasicBlock::Create(*context_, "forcond", func);
        llvm::BasicBlock* body_bb = llvm::BasicBlock::Create(*context_, "forbody");
        llvm::BasicBlock* incr_bb = llvm::BasicBlock::Create(*context_, "forincr");
        llvm::BasicBlock* after_bb = llvm::BasicBlock::Create(*context_, "afterfor");
        
        // القفز إلى condition / Jump to condition
        builder_->CreateBr(cond_bb);
        
        // توليد condition / Generate condition
        builder_->SetInsertPoint(cond_bb);
        // المصدر: ForStmt::condition من statements.h:192
        // Source: ForStmt::condition from statements.h:192
        llvm::Value* cond_val = nullptr;
        if (for_stmt->condition) {
            cond_val = generateExpression(for_stmt->condition.get());
            if (!cond_val) {
                return;
            }
            builder_->CreateCondBr(cond_val, body_bb, after_bb);
        } else {
            // لا يوجد شرط = حلقة لا نهائية / No condition = infinite loop
            builder_->CreateBr(body_bb);
        }
        
        // توليد body / Generate body
        body_bb->insertInto(func);
        builder_->SetInsertPoint(body_bb);
        // المصدر: ForStmt::body من statements.h:194
        // Source: ForStmt::body from statements.h:194
        generateStatement(for_stmt->body.get());
        if (!builder_->GetInsertBlock()->getTerminator()) {
            builder_->CreateBr(incr_bb);
        }
        
        // توليد increment / Generate increment
        incr_bb->insertInto(func);
        builder_->SetInsertPoint(incr_bb);
        // المصدر: ForStmt::increment من statements.h:193
        // Source: ForStmt::increment from statements.h:193
        if (for_stmt->increment) {
            generateExpression(for_stmt->increment.get());
        }
        builder_->CreateBr(cond_bb);
        
        // الانتقال إلى after block / Move to after block
        after_bb->insertInto(func);
        builder_->SetInsertPoint(after_bb);
        return;
    }
    
    // 7. BlockStmt - جملة الكتلة / Block Statement
    // المصدر: statements.h:415-448 (class BlockStmt)
    // Source: statements.h:415-448 (class BlockStmt)
    if (auto* block_stmt = dynamic_cast<Sad::AST::BlockStmt*>(stmt)) {
        // توليد كل جملة في الكتلة / Generate each statement in block
        // المصدر: BlockStmt::statements من statements.h:429
        // Source: BlockStmt::statements from statements.h:429
        for (const auto& s : block_stmt->statements) {
            generateStatement(s.get());
        }
        return;
    }
    
    // نوع عبارة غير مدعوم حالياً / Unsupported statement type currently
    logError("Unsupported statement type");
}

llvm::Function* LLVMGenerator::generateFunctionDecl(Sad::AST::ASTNode* func_node) {
    // ============================================================================
    // اسم الدالة: generateFunctionDecl
    // مصدر التعريف: llvm_generator.h:322-330
    // التوقيع الكامل: llvm::Function* generateFunctionDecl(Sad::AST::ASTNode* func_node);
    // المتغيرات المستخدمة:
    //   - module_: defined at llvm_generator.h:283
    //   - context_: defined at llvm_generator.h:281
    //   - builder_: defined at llvm_generator.h:286
    //   - gen_context_: defined at llvm_generator.h:289
    // الدوال المستدعاة:
    //   - dynamic_cast (C++ RTTI)
    //   - llvm::FunctionType::get() (LLVM API)
    //   - llvm::Function::Create() (LLVM API)
    //   - llvm::BasicBlock::Create() (LLVM API)
    // ============================================================================
    
    if (!func_node) {
        logError("Function node is null");
        return nullptr;
    }
    
    // التحقق من أن هذه FunctionDecl / Check that this is FunctionDecl
    // المصدر: declarations.h:24-88 (class FunctionDecl)
    // Source: declarations.h:24-88 (class FunctionDecl)
    auto* func_decl = dynamic_cast<Sad::AST::FunctionDecl*>(func_node);
    if (!func_decl) {
        logError("Node is not a FunctionDecl");
        return nullptr;
    }
    
    // الحصول علة معلومات الدالة / Get function information
    // المصدر: FunctionDecl::name من declarations.h:38
    // Source: FunctionDecl::name from declarations.h:38
    std::string func_name = func_decl->name;
    
    // بناء أنواع المعاملات / Build parameter types
    // المصدر: FunctionDecl::parameters من declarations.h:39
    // Source: FunctionDecl::parameters from declarations.h:39
    std::vector<llvm::Type*> param_types;
    for (const auto& param : func_decl->parameters) {
        // تحويل نوع Sad إلى نوع LLVM / Convert Sad type to LLVM type
        // المصدر: Parameter::type من expressions.h:544
        // Source: Parameter::type from expressions.h:544
        llvm::Type* param_type = nullptr;
        switch (param.type) {
            case Data::DataType::INTEGER:
                param_type = llvm::Type::getInt32Ty(*context_);
                break;
            case Data::DataType::FLOAT:
                param_type = llvm::Type::getDoubleTy(*context_);
                break;
            case Data::DataType::BOOLEAN:
                param_type = llvm::Type::getInt1Ty(*context_);
                break;
            case Data::DataType::STRING:
                // String = i8* / String = i8*
                param_type = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(*context_));
                break;
            default:
                // افتراضي: i32 / Default: i32
                param_type = llvm::Type::getInt32Ty(*context_);
                break;
        }
        param_types.push_back(param_type);
    }
    
    // بناء نوع الإرجاع / Build return type
    // المصدر: FunctionDecl::returnType من declarations.h:40
    // Source: FunctionDecl::returnType from declarations.h:40
    llvm::Type* ret_type = nullptr;
    switch (func_decl->returnType) {
        case Data::DataType::INTEGER:
            ret_type = llvm::Type::getInt32Ty(*context_);
            break;
        case Data::DataType::FLOAT:
            ret_type = llvm::Type::getDoubleTy(*context_);
            break;
        case Data::DataType::BOOLEAN:
            ret_type = llvm::Type::getInt1Ty(*context_);
            break;
        case Data::DataType::STRING:
            ret_type = llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(*context_));
            break;
        case Data::DataType::NONE:
        default:
            // void return / إرجاع void
            ret_type = llvm::Type::getVoidTy(*context_);
            break;
    }
    
    // إنشاء نوع الدالة / Create function type
    // المصدر: llvm::FunctionType::get() من LLVM API
    // Source: llvm::FunctionType::get() from LLVM API
    llvm::FunctionType* func_type = llvm::FunctionType::get(ret_type, param_types, false);
    
    // إنشاء الدالة / Create function
    // المصدر: llvm::Function::Create() من LLVM API
    // Source: llvm::Function::Create() from LLVM API
    llvm::Function* func = llvm::Function::Create(
        func_type,
        llvm::Function::ExternalLinkage,
        func_name,
        module_.get()
    );
    
    // تسمية المعاملات / Name parameters
    // المصدر: llvm::Argument::setName() من LLVM API
    // Source: llvm::Argument::setName() from LLVM API
    size_t idx = 0;
    for (auto& arg : func->args()) {
        if (idx < func_decl->parameters.size()) {
            // المصدر: Parameter::name من expressions.h:543
            // Source: Parameter::name from expressions.h:543
            arg.setName(func_decl->parameters[idx].name);
        }
        idx++;
    }
    
    // تسجيل الدالة في جدول الدوال / Register function in functions table
    // المصدر: LLVMGeneratorContext::functions من llvm_generator.h:68-70
    // Source: LLVMGeneratorContext::functions from llvm_generator.h:68-70
    gen_context_->functions[func_name] = func;
    
    // إنشاء كتلة الدخول entry block / Create entry block
    // المصدر: llvm::BasicBlock::Create() من LLVM API
    // Source: llvm::BasicBlock::Create() from LLVM API
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(*context_, "entry", func);
    builder_->SetInsertPoint(entry);
    
    // حفظ الدالة الحالية / Save current function
    // المصدر: LLVMGeneratorContext::current_function من llvm_generator.h:59
    // Source: LLVMGeneratorContext::current_function from llvm_generator.h:59
    llvm::Function* prev_func = gen_context_->current_function;
    gen_context_->current_function = func;
    
    // إنشاء alloca لكل معامل / Create alloca for each parameter
    // المصدر: llvm::IRBuilder::CreateAlloca() من LLVM API
    // Source: llvm::IRBuilder::CreateAlloca() from LLVM API
    idx = 0;
    for (auto& arg : func->args()) {
        if (idx < func_decl->parameters.size()) {
            std::string param_name = func_decl->parameters[idx].name;
            llvm::Type* param_type = arg.getType();
            
            // إنشاء alloca / Create alloca
            llvm::AllocaInst* alloca_inst = builder_->CreateAlloca(param_type, nullptr, param_name);
            
            // حفظ قيمة المعامل / Store parameter value
            builder_->CreateStore(&arg, alloca_inst);
            
            // تسجيل في جدول المتغيرات / Register in variables table
            // المصدر: LLVMGeneratorContext::named_values من llvm_generator.h:64-66
            // Source: LLVMGeneratorContext::named_values from llvm_generator.h:64-66
            gen_context_->named_values[param_name] = alloca_inst;
        }
        idx++;
    }
    
    // توليد جسم الدالة / Generate function body
    // المصدر: FunctionDecl::body من declarations.h:41
    // Source: FunctionDecl::body from declarations.h:41
    if (func_decl->body) {
        generateStatement(func_decl->body.get());
    }
    
    // إضافة return افتراضي إذا لم ينتهي البلوك بـ terminator / Add default return if block doesn't end with terminator
    llvm::BasicBlock* curr_bb = builder_->GetInsertBlock();
    if (curr_bb && !curr_bb->getTerminator()) {
        if (ret_type->isVoidTy()) {
            // إرجاع void / Return void
            builder_->CreateRetVoid();
        } else if (ret_type->isIntegerTy(32)) {
            // إرجاع 0 / Return 0
            builder_->CreateRet(llvm::ConstantInt::get(ret_type, 0));
        } else if (ret_type->isDoubleTy()) {
            // إرجاع 0.0 / Return 0.0
            builder_->CreateRet(llvm::ConstantFP::get(ret_type, 0.0));
        } else {
            // إرجاع null / Return null
            builder_->CreateRet(llvm::Constant::getNullValue(ret_type));
        }
    }
    
    // مسح جدول المتغيرات المحلية / Clear local variables table
    gen_context_->named_values.clear();
    
    // استرجاع الدالة السابقة / Restore previous function
    gen_context_->current_function = prev_func;
    
    return func;
}

void LLVMGenerator::logError(const std::string& message) {
    // تسجيل رسالة الخطأ / Log error message
    // المصدر: last_error_ من llvm_generator.h:289
    // Source: last_error_ from llvm_generator.h:289
    last_error_ = message;
}

} // namespace JIT
} // namespace Sad

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
