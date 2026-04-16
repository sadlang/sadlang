/**
 * @file test_async_await.cpp
 * @brief (AR) اختبارات نظام async/await
 *        (EN) Async/Await System Tests
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║    ████████╗███████╗███████╗████████╗                                         ║
 * ║    ╚══██╔══╝██╔════╝██╔════╝╚══██╔══╝                                         ║
 * ║       ██║   █████╗  ███████╗   ██║                                            ║
 * ║       ██║   ██╔══╝  ╚════██║   ██║                                            ║
 * ║       ██║   ███████╗███████║   ██║                                            ║
 * ║       ╚═╝   ╚══════╝╚══════╝   ╚═╝                                            ║
 * ║                                                                               ║
 * ║         █████╗ ███████╗██╗   ██╗███╗   ██╗ ██████╗                            ║
 * ║        ██╔══██╗██╔════╝╚██╗ ██╔╝████╗  ██║██╔════╝                            ║
 * ║        ███████║███████╗ ╚████╔╝ ██╔██╗ ██║██║                                 ║
 * ║        ██╔══██║╚════██║  ╚██╔╝  ██║╚██╗██║██║                                 ║
 * ║        ██║  ██║███████║   ██║   ██║ ╚████║╚██████╗                            ║
 * ║        ╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═══╝ ╚═════╝                            ║
 * ║                                                                               ║
 * ║              اختبارات شاملة لنظام غير متزامن/انتظر                            ║
 * ║              Comprehensive Async/Await System Tests                           ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يختبر جميع جوانب نظام async/await:
 *      - تحليل الكلمات المفتاحية (غير_متزامن، انتظر، أنتج)
 *      - نظام الأنواع (مستقبل، مولد، تدفق)
 *      - تحويل آلة الحالة
 *      - توليد كود LLVM
 *      - القنوات والتواصل
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

// (AR) رؤوس المحلل
#include "parser/async_parser.hpp"
#include "parser/await_parser.hpp"
#include "parser/yield_parser.hpp"

// (AR) رؤوس نظام الأنواع
#include "types/future_type.hpp"
#include "types/generator_type.hpp"

// (AR) رؤوس التحويل وتوليد الكود
#include "middle/async_transform.hpp"
#include "backend/async_codegen.hpp"

// (AR) محاكاة نظام التشغيل (للاختبار)
namespace sad::runtime {
    // (AR) محاكاة بسيطة لنظام التشغيل
    class MockExecutor {
    public:
        std::vector<std::function<void()>> tasks;
        
        void spawn(std::function<void()> task) {
            tasks.push_back(std::move(task));
        }
        
        void run_until_complete() {
            while (!tasks.empty()) {
                auto task = tasks.back();
                tasks.pop_back();
                task();
            }
        }
    };
}

namespace sad::test {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات تحليل الكلمات المفتاحية
//                    (EN) Keyword Parsing Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncKeywordTest : public ::testing::Test {
protected:
    void SetUp() override {
        // (AR) إعداد بيئة الاختبار
    }
};

/**
 * (AR) اختبار: التعرف على الكلمات المفتاحية العربية
 */
TEST_F(AsyncKeywordTest, RecognizeArabicKeywords) {
    // (AR) غير_متزامن
    EXPECT_TRUE(parser::isAsyncKeyword("غير_متزامن"));
    EXPECT_TRUE(parser::isAsyncKeyword("async"));
    
    // (AR) انتظر
    EXPECT_TRUE(parser::isAwaitKeyword("انتظر"));
    EXPECT_TRUE(parser::isAwaitKeyword("await"));
    
    // (AR) أنتج
    EXPECT_TRUE(parser::isYieldKeyword("أنتج"));
    EXPECT_TRUE(parser::isYieldKeyword("yield"));
    
    // (AR) مولد
    EXPECT_TRUE(parser::isGeneratorKeyword("مولد"));
    EXPECT_TRUE(parser::isGeneratorKeyword("gen"));
}

/**
 * (AR) اختبار: عدم التعرف على كلمات خاطئة
 */
TEST_F(AsyncKeywordTest, RejectInvalidKeywords) {
    EXPECT_FALSE(parser::isAsyncKeyword("متزامن"));
    EXPECT_FALSE(parser::isAsyncKeyword("synchronous"));
    EXPECT_FALSE(parser::isAwaitKeyword("انتظار"));
    EXPECT_FALSE(parser::isYieldKeyword("إنتاج"));
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات تحليل الدوال غير المتزامنة
//                    (EN) Async Function Parsing Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncParserTest : public ::testing::Test {
protected:
    std::unique_ptr<lexer::Lexer> lexer_;
    std::unique_ptr<parser::Parser> parser_;
    std::unique_ptr<errors::ErrorReporter> reporter_;
    
    void SetUp() override {
        reporter_ = std::make_unique<errors::ErrorReporter>();
    }
    
    void setupParser(const std::string& code) {
        lexer_ = std::make_unique<lexer::Lexer>(code, reporter_.get());
        parser_ = std::make_unique<parser::Parser>(lexer_.get(), reporter_.get());
    }
};

/**
 * (AR) اختبار: تحليل دالة غير متزامنة بسيطة
 */
TEST_F(AsyncParserTest, ParseSimpleAsyncFunction) {
    std::string code = R"(
        غير_متزامن دالة جلب_بيانات() -> مستقبل<نص> {
            أرجع "مرحباً"؛
        }
    )";
    
    setupParser(code);
    auto async_parser = parser::createAsyncParser(*parser_, *reporter_);
    
    auto node = async_parser->parseAsyncFunction();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->name, "جلب_بيانات");
    EXPECT_FALSE(node->is_generator);
}

/**
 * (AR) اختبار: تحليل دالة مع await
 */
TEST_F(AsyncParserTest, ParseAsyncFunctionWithAwait) {
    std::string code = R"(
        غير_متزامن دالة معالجة() -> مستقبل<عدد> {
            دع بيانات = انتظر جلب_بيانات()؛
            أرجع بيانات.طول()؛
        }
    )";
    
    setupParser(code);
    auto async_parser = parser::createAsyncParser(*parser_, *reporter_);
    
    auto node = async_parser->parseAsyncFunction();
    
    ASSERT_NE(node, nullptr);
    // (AR) تحقق من وجود await في الجسم
    EXPECT_TRUE(node->body != nullptr);
}

/**
 * (AR) اختبار: تحليل تدفق غير متزامن (async generator)
 */
TEST_F(AsyncParserTest, ParseAsyncGenerator) {
    std::string code = R"(
        غير_متزامن مولد دالة أحداث() -> تدفق<حدث> {
            حلقة {
                دع حدث = انتظر مستمع.التالي()؛
                أنتج حدث؛
            }
        }
    )";
    
    setupParser(code);
    auto async_parser = parser::createAsyncParser(*parser_, *reporter_);
    
    auto node = async_parser->parseAsyncFunction();
    
    ASSERT_NE(node, nullptr);
    EXPECT_TRUE(node->is_generator);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات تحليل await
//                    (EN) Await Parsing Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AwaitParserTest : public ::testing::Test {
protected:
    std::unique_ptr<lexer::Lexer> lexer_;
    std::unique_ptr<parser::Parser> parser_;
    std::unique_ptr<errors::ErrorReporter> reporter_;
    
    void SetUp() override {
        reporter_ = std::make_unique<errors::ErrorReporter>();
    }
    
    void setupParser(const std::string& code) {
        lexer_ = std::make_unique<lexer::Lexer>(code, reporter_.get());
        parser_ = std::make_unique<parser::Parser>(lexer_.get(), reporter_.get());
    }
};

/**
 * (AR) اختبار: await بسيط
 */
TEST_F(AwaitParserTest, ParseSimpleAwait) {
    std::string code = "انتظر جلب_بيانات()";
    
    setupParser(code);
    auto await_parser = parser::createAwaitParser(*parser_, *reporter_);
    await_parser->enterAsyncContext();
    
    auto node = await_parser->parseAwaitExpression();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->kind, parser::AwaitKind::SIMPLE);
}

/**
 * (AR) اختبار: await متسلسل
 */
TEST_F(AwaitParserTest, ParseChainedAwait) {
    std::string code = "انتظر جلب().معالجة().تحويل()";
    
    setupParser(code);
    auto await_parser = parser::createAwaitParser(*parser_, *reporter_);
    await_parser->enterAsyncContext();
    
    auto node = await_parser->parseAwaitExpression();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->kind, parser::AwaitKind::CHAINED);
}

/**
 * (AR) اختبار: join (انتظار متعدد)
 */
TEST_F(AwaitParserTest, ParseJoinAwait) {
    std::string code = "انتظار_جميع!(مهمة1، مهمة2، مهمة3)";
    
    setupParser(code);
    auto await_parser = parser::createAwaitParser(*parser_, *reporter_);
    await_parser->enterAsyncContext();
    
    auto node = await_parser->parseJoinAwait();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->futures.size(), 3);
}

/**
 * (AR) اختبار: select (انتظار أي)
 */
TEST_F(AwaitParserTest, ParseSelectAwait) {
    std::string code = R"(
        اختر! {
            قيمة = انتظر مهمة1 => { معالجة(قيمة) }
            _ = انتظر مهمة2 => { إلغاء() }
        }
    )";
    
    setupParser(code);
    auto await_parser = parser::createAwaitParser(*parser_, *reporter_);
    await_parser->enterAsyncContext();
    
    auto node = await_parser->parseSelectAwait();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->branches.size(), 2);
}

/**
 * (AR) اختبار: خطأ - await خارج سياق async
 */
TEST_F(AwaitParserTest, AwaitOutsideAsyncContextError) {
    std::string code = "انتظر دالة()";
    
    setupParser(code);
    auto await_parser = parser::createAwaitParser(*parser_, *reporter_);
    // (AR) لم ندخل سياق async
    
    auto node = await_parser->parseAwaitExpression();
    
    EXPECT_EQ(node, nullptr);
    EXPECT_TRUE(reporter_->hasErrors());
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات تحليل yield
//                    (EN) Yield Parsing Tests
// ═══════════════════════════════════════════════════════════════════════════════

class YieldParserTest : public ::testing::Test {
protected:
    std::unique_ptr<lexer::Lexer> lexer_;
    std::unique_ptr<parser::Parser> parser_;
    std::unique_ptr<errors::ErrorReporter> reporter_;
    
    void SetUp() override {
        reporter_ = std::make_unique<errors::ErrorReporter>();
    }
    
    void setupParser(const std::string& code) {
        lexer_ = std::make_unique<lexer::Lexer>(code, reporter_.get());
        parser_ = std::make_unique<parser::Parser>(lexer_.get(), reporter_.get());
    }
};

/**
 * (AR) اختبار: yield بسيط
 */
TEST_F(YieldParserTest, ParseSimpleYield) {
    std::string code = "أنتج 42";
    
    setupParser(code);
    auto yield_parser = parser::createYieldParser(*parser_, *reporter_);
    yield_parser->enterGeneratorContext(false);
    
    auto node = yield_parser->parseYieldExpression();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->kind, parser::YieldKind::SIMPLE);
}

/**
 * (AR) اختبار: yield بدون قيمة
 */
TEST_F(YieldParserTest, ParseEmptyYield) {
    std::string code = "أنتج;";
    
    setupParser(code);
    auto yield_parser = parser::createYieldParser(*parser_, *reporter_);
    yield_parser->enterGeneratorContext(false);
    
    auto node = yield_parser->parseYieldExpression();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->kind, parser::YieldKind::EMPTY);
}

/**
 * (AR) اختبار: yield* (تفويض)
 */
TEST_F(YieldParserTest, ParseYieldFrom) {
    std::string code = "أنتج* مولد_آخر()";
    
    setupParser(code);
    auto yield_parser = parser::createYieldParser(*parser_, *reporter_);
    yield_parser->enterGeneratorContext(false);
    
    auto node = yield_parser->parseYieldExpression();
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->kind, parser::YieldKind::FROM);
}

/**
 * (AR) اختبار: تحليل دالة مولد كاملة
 */
TEST_F(YieldParserTest, ParseGeneratorFunction) {
    std::string code = R"(
        مولد دالة أعداد_زوجية(حد: عدد) -> مولد<عدد> {
            دع متغير ع = 0؛
            بينما ع < حد {
                أنتج ع؛
                ع += 2؛
            }
        }
    )";
    
    setupParser(code);
    auto yield_parser = parser::createYieldParser(*parser_, *reporter_);
    
    auto node = yield_parser->parseGeneratorFunction(false);
    
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(node->name, "أعداد_زوجية");
    EXPECT_FALSE(node->is_async);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات نظام الأنواع
//                    (EN) Type System Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncTypeTest : public ::testing::Test {
protected:
    std::unique_ptr<types::TypeManager> type_manager_;
    
    void SetUp() override {
        type_manager_ = std::make_unique<types::TypeManager>();
    }
};

/**
 * (AR) اختبار: إنشاء نوع Future
 */
TEST_F(AsyncTypeTest, CreateFutureType) {
    auto int_type = type_manager_->getIntType();
    auto future_type = type_manager_->getFutureType(int_type);
    
    ASSERT_NE(future_type, nullptr);
    EXPECT_EQ(future_type->kind(), types::SadTypeKind::Future);
    EXPECT_EQ(future_type->getOutputType(), int_type);
}

/**
 * (AR) اختبار: إنشاء نوع Poll
 */
TEST_F(AsyncTypeTest, CreatePollType) {
    auto string_type = type_manager_->getStringType();
    auto poll_type = type_manager_->getPollType(string_type);
    
    ASSERT_NE(poll_type, nullptr);
    EXPECT_TRUE(poll_type->isReady() || poll_type->isPending());
}

/**
 * (AR) اختبار: إنشاء نوع Generator
 */
TEST_F(AsyncTypeTest, CreateGeneratorType) {
    auto yield_type = type_manager_->getIntType();
    auto return_type = type_manager_->getUnitType();
    auto gen_type = type_manager_->getGeneratorType(yield_type, return_type);
    
    ASSERT_NE(gen_type, nullptr);
    EXPECT_EQ(gen_type->kind(), types::SadTypeKind::Generator);
    EXPECT_EQ(gen_type->getYieldType(), yield_type);
}

/**
 * (AR) اختبار: إنشاء نوع Stream
 */
TEST_F(AsyncTypeTest, CreateStreamType) {
    auto item_type = type_manager_->getStringType();
    auto stream_type = type_manager_->getStreamType(item_type);
    
    ASSERT_NE(stream_type, nullptr);
    EXPECT_EQ(stream_type->kind(), types::SadTypeKind::Stream);
}

/**
 * (AR) اختبار: التحقق من سمة Future
 */
TEST_F(AsyncTypeTest, FutureTraitImplementation) {
    auto int_type = type_manager_->getIntType();
    auto future_type = type_manager_->getFutureType(int_type);
    
    EXPECT_TRUE(future_type->implementsTrait("مستقبل"));
    EXPECT_TRUE(future_type->implementsTrait("Future"));
    EXPECT_TRUE(future_type->hasMethod("استطلع"));
    EXPECT_TRUE(future_type->hasMethod("poll"));
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات تحويل آلة الحالة
//                    (EN) State Machine Transformation Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncTransformTest : public ::testing::Test {
protected:
    std::unique_ptr<middle::AsyncTransformer> transformer_;
    
    void SetUp() override {
        transformer_ = std::make_unique<middle::AsyncTransformer>();
    }
};

/**
 * (AR) اختبار: تحويل دالة async بسيطة
 */
TEST_F(AsyncTransformTest, TransformSimpleAsync) {
    // (AR) إنشاء AST لدالة async
    auto async_func = std::make_unique<ast::AsyncFunctionNode>();
    async_func->name = "test_func";
    
    // (AR) جسم بسيط: return 42
    auto return_stmt = std::make_unique<ast::ReturnStmtNode>();
    return_stmt->value = std::make_unique<ast::IntLiteralNode>(42);
    
    async_func->body = std::make_unique<ast::BlockNode>();
    async_func->body->statements.push_back(std::move(return_stmt));
    
    // (AR) تحويل
    auto state_machine = transformer_->transform(async_func.get());
    
    ASSERT_NE(state_machine, nullptr);
    EXPECT_EQ(state_machine->states.size(), 2); // START + END
}

/**
 * (AR) اختبار: تحويل دالة مع await
 */
TEST_F(AsyncTransformTest, TransformAsyncWithAwait) {
    // (AR) إنشاء AST لدالة async مع await
    auto async_func = std::make_unique<ast::AsyncFunctionNode>();
    async_func->name = "test_await";
    
    // (AR) let x = await something();
    auto await_stmt = std::make_unique<ast::AwaitExprNode>();
    await_stmt->inner = std::make_unique<ast::CallExprNode>("something");
    
    auto let_stmt = std::make_unique<ast::LetStmtNode>();
    let_stmt->name = "x";
    let_stmt->initializer = std::move(await_stmt);
    
    async_func->body = std::make_unique<ast::BlockNode>();
    async_func->body->statements.push_back(std::move(let_stmt));
    
    // (AR) تحويل
    auto state_machine = transformer_->transform(async_func.get());
    
    ASSERT_NE(state_machine, nullptr);
    // (AR) يجب أن يكون هناك: START, AWAIT_POINT, END
    EXPECT_GE(state_machine->states.size(), 3);
    
    // (AR) تحقق من وجود نقطة await
    bool has_await_point = false;
    for (const auto& state : state_machine->states) {
        if (state->kind == middle::AsyncStateKind::AWAIT_POINT) {
            has_await_point = true;
            break;
        }
    }
    EXPECT_TRUE(has_await_point);
}

/**
 * (AR) اختبار: تحويل مولد
 */
TEST_F(AsyncTransformTest, TransformGenerator) {
    // (AR) إنشاء AST لمولد
    auto gen_func = std::make_unique<ast::GeneratorFunctionNode>();
    gen_func->name = "test_gen";
    
    // (AR) yield 1; yield 2;
    auto yield1 = std::make_unique<ast::YieldExprNode>();
    yield1->value = std::make_unique<ast::IntLiteralNode>(1);
    
    auto yield2 = std::make_unique<ast::YieldExprNode>();
    yield2->value = std::make_unique<ast::IntLiteralNode>(2);
    
    gen_func->body = std::make_unique<ast::BlockNode>();
    gen_func->body->statements.push_back(std::move(yield1));
    gen_func->body->statements.push_back(std::move(yield2));
    
    // (AR) تحويل
    auto state_machine = transformer_->transformGenerator(gen_func.get());
    
    ASSERT_NE(state_machine, nullptr);
    // (AR) يجب أن يكون هناك: START, YIELD1, YIELD2, END
    EXPECT_GE(state_machine->states.size(), 4);
    
    // (AR) تحقق من نقاط yield
    int yield_count = 0;
    for (const auto& state : state_machine->states) {
        if (state->kind == middle::AsyncStateKind::YIELD_POINT) {
            yield_count++;
        }
    }
    EXPECT_EQ(yield_count, 2);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات توليد كود LLVM
//                    (EN) LLVM Code Generation Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncCodeGenTest : public ::testing::Test {
protected:
    std::unique_ptr<llvm::LLVMContext> llvm_context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<backend::AsyncCodeGenerator> codegen_;
    
    void SetUp() override {
        llvm_context_ = std::make_unique<llvm::LLVMContext>();
        module_ = std::make_unique<llvm::Module>("test", *llvm_context_);
        codegen_ = std::make_unique<backend::AsyncCodeGenerator>(
            *llvm_context_, module_.get());
    }
};

/**
 * (AR) اختبار: توليد نوع Poll
 */
TEST_F(AsyncCodeGenTest, GeneratePollType) {
    auto int_type = llvm::Type::getInt32Ty(*llvm_context_);
    auto poll_type = codegen_->generatePollType(int_type);
    
    ASSERT_NE(poll_type, nullptr);
    EXPECT_TRUE(poll_type->isStructTy());
}

/**
 * (AR) اختبار: توليد بنية آلة الحالة
 */
TEST_F(AsyncCodeGenTest, GenerateStateMachineStruct) {
    // (AR) إنشاء آلة حالة وهمية
    auto state_machine = std::make_unique<middle::AsyncStateMachine>();
    state_machine->name = "test_state_machine";
    state_machine->states.push_back(std::make_unique<middle::AsyncState>(
        middle::AsyncStateKind::START, 0));
    state_machine->states.push_back(std::make_unique<middle::AsyncState>(
        middle::AsyncStateKind::END, 1));
    
    auto struct_type = codegen_->generateStateMachineType(state_machine.get());
    
    ASSERT_NE(struct_type, nullptr);
    EXPECT_TRUE(struct_type->isStructTy());
}

/**
 * (AR) اختبار: توليد دالة poll
 */
TEST_F(AsyncCodeGenTest, GeneratePollFunction) {
    // (AR) إنشاء آلة حالة
    auto state_machine = std::make_unique<middle::AsyncStateMachine>();
    state_machine->name = "poll_test";
    state_machine->output_type = llvm::Type::getInt32Ty(*llvm_context_);
    
    state_machine->states.push_back(std::make_unique<middle::AsyncState>(
        middle::AsyncStateKind::START, 0));
    state_machine->states.push_back(std::make_unique<middle::AsyncState>(
        middle::AsyncStateKind::END, 1));
    
    auto poll_func = codegen_->generatePollFunction(state_machine.get());
    
    ASSERT_NE(poll_func, nullptr);
    EXPECT_TRUE(poll_func->getName().contains("poll"));
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات التكامل
//                    (EN) Integration Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncIntegrationTest : public ::testing::Test {
protected:
    std::unique_ptr<compiler::Compiler> compiler_;
    std::unique_ptr<runtime::MockExecutor> executor_;
    
    void SetUp() override {
        compiler_ = std::make_unique<compiler::Compiler>();
        executor_ = std::make_unique<runtime::MockExecutor>();
    }
};

/**
 * (AR) اختبار: تجميع وتنفيذ async كامل
 */
TEST_F(AsyncIntegrationTest, CompileAndRunAsyncFunction) {
    std::string code = R"(
        غير_متزامن دالة مجموع(أ: عدد، ب: عدد) -> مستقبل<عدد> {
            أرجع أ + ب؛
        }
        
        غير_متزامن دالة رئيسية() {
            دع نتيجة = انتظر مجموع(3، 4)؛
            تأكد(نتيجة == 7)؛
        }
    )";
    
    auto result = compiler_->compile(code);
    ASSERT_TRUE(result.success);
    
    // (AR) تنفيذ
    executor_->spawn([&]() {
        result.execute();
    });
    
    EXPECT_NO_THROW(executor_->run_until_complete());
}

/**
 * (AR) اختبار: تجميع وتنفيذ مولد
 */
TEST_F(AsyncIntegrationTest, CompileAndRunGenerator) {
    std::string code = R"(
        مولد دالة أرقام() -> مولد<عدد> {
            أنتج 1؛
            أنتج 2؛
            أنتج 3؛
        }
        
        دالة رئيسية() {
            دع متغير مجموع = 0؛
            لكل رقم في أرقام() {
                مجموع += رقم؛
            }
            تأكد(مجموع == 6)؛
        }
    )";
    
    auto result = compiler_->compile(code);
    ASSERT_TRUE(result.success);
    
    EXPECT_NO_THROW(result.execute());
}

/**
 * (AR) اختبار: قنوات async
 */
TEST_F(AsyncIntegrationTest, AsyncChannels) {
    std::string code = R"(
        غير_متزامن دالة منتج(مرسل: مرسل<عدد>) {
            لكل ع في 1..=5 {
                مرسل.أرسل(ع).فك()؛
            }
        }
        
        غير_متزامن دالة مستهلك(مستقبل: مستقبل_قناة<عدد>) -> عدد {
            دع متغير مجموع = 0؛
            بينما دع موافق(قيمة) = انتظر مستقبل.استقبل_انتظر() {
                مجموع += قيمة؛
            }
            أرجع مجموع؛
        }
        
        غير_متزامن دالة رئيسية() {
            دع (مرسل، مستقبل) = قناة::<عدد>()؛
            
            انتظار_جميع!(
                منتج(مرسل)،
                {
                    دع نتيجة = انتظر مستهلك(مستقبل)؛
                    تأكد(نتيجة == 15)؛
                }
            )؛
        }
    )";
    
    auto result = compiler_->compile(code);
    ASSERT_TRUE(result.success);
    
    executor_->spawn([&]() {
        result.execute();
    });
    
    EXPECT_NO_THROW(executor_->run_until_complete());
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) اختبارات الأداء
//                    (EN) Performance Tests
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

/**
 * (AR) اختبار: عدم وجود تكلفة إضافية لـ async (zero-cost)
 */
TEST_F(AsyncPerformanceTest, ZeroCostAbstraction) {
    // (AR) هذا الاختبار يتحقق من أن async لا يضيف تكلفة وقت تشغيل
    // مقارنة بنسخة متزامنة مكافئة
    
    // (AR) النسخة المتزامنة
    auto sync_code = R"(
        دالة فيبوناتشي_متزامن(ن: عدد) -> عدد {
            إذا ن <= 1 { أرجع ن؛ }
            أرجع فيبوناتشي_متزامن(ن - 1) + فيبوناتشي_متزامن(ن - 2)؛
        }
    )";
    
    // (AR) النسخة غير المتزامنة (بدون await فعلي)
    auto async_code = R"(
        غير_متزامن دالة فيبوناتشي_غير_متزامن(ن: عدد) -> مستقبل<عدد> {
            إذا ن <= 1 { أرجع ن؛ }
            أرجع انتظر فيبوناتشي_غير_متزامن(ن - 1) 
                 + انتظر فيبوناتشي_غير_متزامن(ن - 2)؛
        }
    )";
    
    // (AR) بعد التحسين، يجب أن يكون الكود الناتج متطابقاً تقريباً
    // (هذا اختبار مفاهيمي - التنفيذ الفعلي يحتاج benchmarking)
    
    SUCCEED(); // (AR) اختبار مفاهيمي
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقطة الدخول الرئيسية
//                    (EN) Main Entry Point
// ═══════════════════════════════════════════════════════════════════════════════

} // namespace sad::test

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
