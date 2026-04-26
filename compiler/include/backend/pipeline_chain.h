/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: pipeline_chain.h                                                     ║
 * ║   الوصف: تعريفات أنواع وأصناف سلاسل الأنابيب المتعددة                       ║
 * ║   المهمة: CW-05 — تقسيم pipeline_chain.cpp إلى ملفات فرعية                 ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * (AR) هذا الملف يحتوي على تعريفات الأنواع والأصناف المشتركة
 *      بين ملفات تنفيذ سلاسل الأنابيب الثلاثة:
 *      - pipeline_chain.cpp: التوليد الرئيسي والواجهة
 *      - pipeline_chain_ops.cpp: العمليات الفردية
 *      - pipeline_chain_fusion.cpp: تحسينات الدمج والأدوات
 *
 * (EN) Contains shared type definitions and class declarations
 *      for the three pipeline chain implementation files.
 */

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/ADT/SmallVector.h>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

namespace sad
{
    namespace codegen
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        //                          تعريفات الأنواع | Type Definitions
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) نوع عملية السلسلة
         * @brief (EN) Chain Operation Type
         */
        enum class ChainOperationType
        {
            // عمليات التحويل | Transformation
            MAP,        // حوّل - map
            FILTER,     // صفّي - filter
            FILTER_MAP, // صفّي_حوّل - filter_map
            FLAT_MAP,   // حوّل_مسطح - flat_map

            // عمليات التحديد | Limiting
            TAKE,       // خذ - take
            TAKE_WHILE, // خذ_طالما - take_while
            DROP,       // أسقط - drop
            DROP_WHILE, // أسقط_طالما - drop_while

            // عمليات الجمع | Collection
            COLLECT,  // اجمع - collect
            FOLD,     // طيّ - fold
            REDUCE,   // اختزل - reduce
            FOR_EACH, // لكل - for_each

            // عمليات خاصة | Special
            BRANCH,   // تفرع - branch
            PARALLEL, // متوازي - parallel
            ASYNC,    // غير_متزامن - async
            CUSTOM    // مخصص - custom
        };

        /**
         * @brief (AR) عقدة عملية في السلسلة
         * @brief (EN) Chain Operation Node
         */
        struct ChainOperationNode
        {
            ChainOperationType type;
            std::string arabicName;       // الاسم العربي
            std::string functionName;     // اسم الدالة
            llvm::Type *inputType;        // نوع الإدخال
            llvm::Type *outputType;       // نوع الإخراج
            llvm::Value *operand;         // المعامل (مثل الدالة)
            std::optional<int64_t> count; // للعمليات مع عدد (خذ، أسقط)
            bool isLazy;                  // هل كسول
            bool canFuse;                 // هل يمكن دمجه

            ChainOperationNode()
                : type(ChainOperationType::CUSTOM), arabicName(""), functionName(""),
                  inputType(nullptr), outputType(nullptr), operand(nullptr),
                  count(std::nullopt), isLazy(true), canFuse(true)
            {
            }
        };

        /**
         * @brief (AR) فرع في السلسلة
         * @brief (EN) Chain Branch
         */
        struct ChainBranch
        {
            llvm::Value *condition;                     // الشرط
            std::vector<ChainOperationNode> operations; // العمليات
            llvm::Type *resultType;                     // نوع النتيجة
        };

        /**
         * @brief (AR) تمثيل السلسلة الكاملة
         * @brief (EN) Full Chain Representation
         */
        struct PipelineChain
        {
            std::string name;                           // اسم السلسلة
            llvm::Value *sourceValue;                   // القيمة المصدر
            llvm::Type *sourceType;                     // نوع المصدر
            std::vector<ChainOperationNode> operations; // العمليات
            std::vector<ChainBranch> branches;          // الفروع (إن وجدت)
            bool isParallel;                            // هل متوازية
            bool isAsync;                               // هل غير متزامنة

            PipelineChain()
                : name(""), sourceValue(nullptr), sourceType(nullptr),
                  isParallel(false), isAsync(false)
            {
            }

            size_t size() const { return operations.size(); }
            bool empty() const { return operations.empty(); }
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //                    سياق توليد الكود | Code Generation Context
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) سياق السلسلة — يحتفظ بمعلومات أثناء توليد الكود
         * @brief (EN) Chain Context — holds state during code generation
         */
        class ChainContext
        {
        public:
            llvm::LLVMContext &llvmContext;
            llvm::Module *module;
            llvm::IRBuilder<> &builder;

            // أنواع مساعدة | Helper types
            llvm::Type *voidType;
            llvm::Type *i1Type;  // منطقي
            llvm::Type *i32Type; // عدد صحيح
            llvm::Type *i64Type; // عدد طويل
            llvm::Type *ptrType; // مؤشر

            // تخزين مؤقت | Temporary storage
            std::unordered_map<std::string, llvm::Value *> namedValues;
            std::unordered_map<std::string, llvm::Function *> generatedFunctions;

            ChainContext(llvm::LLVMContext &ctx, llvm::Module *mod, llvm::IRBuilder<> &b)
                : llvmContext(ctx), module(mod), builder(b),
                  voidType(llvm::Type::getVoidTy(ctx)),
                  i1Type(llvm::Type::getInt1Ty(ctx)),
                  i32Type(llvm::Type::getInt32Ty(ctx)),
                  i64Type(llvm::Type::getInt64Ty(ctx)),
                  ptrType(llvm::PointerType::get(ctx, 0))
            {
            }

            void setValue(const std::string &name, llvm::Value *value)
            {
                namedValues[name] = value;
            }

            llvm::Value *getValue(const std::string &name)
            {
                auto it = namedValues.find(name);
                return it != namedValues.end() ? it->second : nullptr;
            }
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //                       مولد سلسلة الأنابيب | Pipeline Chain Generator
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) مولد سلسلة الأنابيب — يولد كود LLVM IR لسلاسل الأنابيب
         * @brief (EN) Pipeline Chain Generator — generates LLVM IR for pipeline chains
         */
        class PipelineChainGenerator
        {
        private:
            ChainContext &ctx;
            int tempCounter;

        public:
            explicit PipelineChainGenerator(ChainContext &context)
                : ctx(context), tempCounter(0)
            {
            }

            // ─── التوليد الرئيسي | Main Generation ──────────────────────────────
            llvm::Value *generateChain(const PipelineChain &chain);
            llvm::Value *generateSequentialChain(const PipelineChain &chain);
            llvm::Value *generateOperation(const ChainOperationNode &op, llvm::Value *input);

            // ─── العمليات الفردية | Individual Operations ───────────────────────
            // (AR) مُنفّذة في pipeline_chain_ops.cpp
            llvm::Value *generateMap(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateFilter(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateFilterMap(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateFlatMap(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateTake(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateTakeWhile(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateDrop(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateDropWhile(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateCollect(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateFold(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateReduce(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateForEach(const ChainOperationNode &op, llvm::Value *input);
            llvm::Value *generateCustom(const ChainOperationNode &op, llvm::Value *input);

            // ─── الدمج والتحسينات | Fusion & Optimization ──────────────────────
            // (AR) مُنفّذة في pipeline_chain_fusion.cpp
            llvm::Value *generateFusedChain(const PipelineChain &chain);
            llvm::Value *generateFusedGroup(
                const std::vector<ChainOperationNode> &group,
                llvm::Value *input);
            bool canFuseWithGroup(
                const std::vector<ChainOperationNode> &group,
                const ChainOperationNode &op);
            bool canUseFusion(const PipelineChain &chain);

            // ─── التفرعي والمتوازي | Branching & Parallel ──────────────────────
            llvm::Value *generateBranchingChain(const PipelineChain &chain);
            llvm::Value *generateParallelChain(const PipelineChain &chain);
            llvm::Value *generateAsyncChain(const PipelineChain &chain);

            // ─── التحسينات والأدوات | Optimization & Utilities ─────────────────
            // (AR) مُنفّذة في pipeline_chain_fusion.cpp
            PipelineChain optimizeChain(const PipelineChain &chain);
            PipelineChain fuseConsecutiveFilters(const PipelineChain &chain);
            PipelineChain moveTakeEarlier(const PipelineChain &chain);
            PipelineChain removeDeadOperations(const PipelineChain &chain);
            PipelineChain convertToLazyIfPossible(const PipelineChain &chain);
            llvm::StructType *createIteratorStruct(const std::string &name, llvm::Type *elementType);
            llvm::Function *getOrCreateFunction(const std::string &name, llvm::FunctionType *type);
            std::string generateTempName(const std::string &prefix);
        };

        // ═══════════════════════════════════════════════════════════════════════════════
        //                         واجهة خارجية | External Interface
        // ═══════════════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) واجهة سلسلة الأنابيب — للاستخدام من باقي المترجم
         * @brief (EN) Pipeline Chain Interface — for use from the rest of the compiler
         */
        class PipelineChainInterface
        {
        public:
            static llvm::Value *generatePipelineChain(
                llvm::LLVMContext &llvmContext,
                llvm::Module *module,
                llvm::IRBuilder<> &builder,
                const PipelineChain &chain);

            static ChainOperationNode createOperationFromAST(
                const std::string &opName,
                const std::string &arabicName,
                llvm::Value *operand,
                llvm::Type *inputType,
                llvm::Type *outputType);

            static PipelineChain createChainFromOperations(
                const std::string &name,
                llvm::Value *source,
                llvm::Type *sourceType,
                const std::vector<ChainOperationNode> &operations);
        };

    } // namespace codegen
} // namespace sad
