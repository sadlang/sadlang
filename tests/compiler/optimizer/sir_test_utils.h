/**
 * @file sir_test_utils.h
 * @brief أدوات مساعدة لاختبارات SIR
 * @brief SIR Testing Utilities
 * 
 * @details
 * (AR) يوفر دوال مساعدة لإنشاء وحدات ودوال SIR للاختبار.
 * (EN) Provides helper functions to create SIR modules and functions for testing.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 */

#ifndef SAD_COMPILER_SIR_TEST_UTILS_H
#define SAD_COMPILER_SIR_TEST_UTILS_H

#include <memory>
#include <string>
#include <vector>
#include "../../../compiler/frontend/include/sir_builder.h"

namespace Sad {
namespace Compiler {
namespace Testing {

/**
 * @brief صنف مساعد لإنشاء SIR للاختبار
 * @brief Helper class for creating SIR for testing
 */
class SIRTestBuilder {
public:
    SIRTestBuilder();
    ~SIRTestBuilder();

    // إنشاء وحدة / Create module
    std::shared_ptr<SIR::SIRModule> createModule(const std::string& name = "test_module");
    
    // إنشاء دالة / Create function
    SIR::SIRFunction* createFunction(
        const std::string& name,
        SIR::SIRType returnType = SIR::SIRType::INT32
    );
    
    // إنشاء كتلة أساسية / Create basic block
    SIR::SIRBasicBlock* createBlock(const std::string& label = "");
    
    // إنشاء تعليمات / Create instructions
    SIR::SIRInstruction* createLoadConst(int64_t value, const std::string& dest);
    SIR::SIRInstruction* createLoadConst(double value, const std::string& dest);
    SIR::SIRInstruction* createBinaryOp(
        SIR::SIROpcode op,
        const std::string& left,
        const std::string& right,
        const std::string& dest
    );
    SIR::SIRInstruction* createUnaryOp(
        SIR::SIROpcode op,
        const std::string& operand,
        const std::string& dest
    );
    SIR::SIRInstruction* createReturn(const std::string& value = "");
    SIR::SIRInstruction* createBranch(const std::string& target);
    SIR::SIRInstruction* createCondBranch(
        const std::string& condition,
        const std::string& trueTarget,
        const std::string& falseTarget
    );
    
    // إضافة تعليمة لكتلة / Add instruction to block
    void addInstruction(SIR::SIRBasicBlock* block, SIR::SIRInstruction* inst);
    
    // إضافة كتلة لدالة / Add block to function
    void addBlock(SIR::SIRFunction* func, SIR::SIRBasicBlock* block);
    
    // إضافة دالة لوحدة / Add function to module
    void addFunction(SIR::SIRFunction* func);
    
    // الحصول على الوحدة الحالية / Get current module
    std::shared_ptr<SIR::SIRModule> getModule() const { return module_; }
    
    // إنشاء سجل مؤقت جديد / Create new temporary register
    std::string newTemp();
    
    // إعادة تعيين / Reset
    void reset();

private:
    std::shared_ptr<SIR::SIRModule> module_;
    SIR::SIRFunction* currentFunction_;
    int tempCounter_;
};

/**
 * @brief دوال مساعدة سريعة / Quick helper functions
 */

// إنشاء دالة بسيطة مع كتلة واحدة / Create simple function with one block
std::shared_ptr<SIR::SIRModule> createSimpleModule(
    const std::string& functionName = "test_func"
);

// إنشاء دالة مع عمليات حسابية ثابتة / Create function with constant arithmetic
std::shared_ptr<SIR::SIRModule> createConstantArithmeticModule();

// إنشاء دالة مع كود ميت / Create function with dead code
std::shared_ptr<SIR::SIRModule> createDeadCodeModule();

// إنشاء دالة مع كتل غير قابلة للوصول / Create function with unreachable blocks
std::shared_ptr<SIR::SIRModule> createUnreachableBlocksModule();

// إنشاء دالة مع تفرعات / Create function with branches
std::shared_ptr<SIR::SIRModule> createBranchingModule();

// إنشاء دالة معقدة / Create complex function
std::shared_ptr<SIR::SIRModule> createComplexModule();

/**
 * @brief دوال للتحقق من النتائج / Verification functions
 */

// عد التعليمات في دالة / Count instructions in function
int countInstructions(SIR::SIRFunction* func);

// عد الكتل في دالة / Count blocks in function
int countBlocks(SIR::SIRFunction* func);

// التحقق من وجود تعليمة / Check if instruction exists
bool hasInstruction(SIR::SIRFunction* func, SIR::SIROpcode opcode);

// طباعة دالة للتصحيح / Print function for debugging
void printFunction(SIR::SIRFunction* func);

// طباعة وحدة للتصحيح / Print module for debugging
void printModule(std::shared_ptr<SIR::SIRModule> module);

} // namespace Testing
} // namespace Compiler
} // namespace Sad

#endif // SAD_COMPILER_SIR_TEST_UTILS_H
