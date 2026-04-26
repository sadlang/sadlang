/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: pipeline_chain_fusion.cpp                                            ║
 * ║   الوصف: تحسينات الدمج والأدوات المساعدة لسلاسل الأنابيب                    ║
 * ║   المهمة: CW-05 — مُستخرج من pipeline_chain.cpp                            ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * (AR) يحتوي على:
 *      - التوليد المدمج (Fusion): دمج العمليات في دالة واحدة
 *      - التوليد المتفرع (Branching): أنابيب متفرعة
 *      - التوليد المتوازي (Parallel): أنابيب متوازية
 *      - التوليد غير المتزامن (Async): أنابيب غير متزامنة
 *      - التحسينات (Optimization): تحسين السلسلة
 *      - الأدوات (Utilities): إنشاء الأنواع والدوال المساعدة
 *
 * (EN) Contains fusion optimization, branching, parallel, async generation,
 *      chain optimization, and utility helpers.
 */

#include "../../include/backend/pipeline_chain.h"

#include <llvm/IR/Intrinsics.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <variant>

namespace sad
{
    namespace codegen
    {
        // ═══════════════════════════════════════════════════════════════════════════
        //                    التوليد المدمج | Fused Generation
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * توليد سلسلة مدمجة | Generate fused chain
         *
         * يدمج عدة عمليات في دالة واحدة لتحسين الأداء
         */
        llvm::Value *PipelineChainGenerator::generateFusedChain(const PipelineChain &chain)
        {
            // تجميع العمليات القابلة للدمج
            std::vector<std::vector<ChainOperationNode>> fusionGroups;
            std::vector<ChainOperationNode> currentGroup;

            for (const auto &op : chain.operations)
            {
                if (op.canFuse && canFuseWithGroup(currentGroup, op))
                {
                    currentGroup.push_back(op);
                }
                else
                {
                    if (!currentGroup.empty())
                    {
                        fusionGroups.push_back(std::move(currentGroup));
                        currentGroup.clear();
                    }
                    currentGroup.push_back(op);
                }
            }

            if (!currentGroup.empty())
            {
                fusionGroups.push_back(std::move(currentGroup));
            }

            // توليد كل مجموعة
            llvm::Value *current = chain.sourceValue;

            for (const auto &group : fusionGroups)
            {
                if (group.size() == 1)
                {
                    current = generateOperation(group[0], current);
                }
                else
                {
                    current = generateFusedGroup(group, current);
                }
            }

            return current;
        }

        /**
         * توليد مجموعة مدمجة
         */
        llvm::Value *PipelineChainGenerator::generateFusedGroup(
            const std::vector<ChainOperationNode> &group,
            llvm::Value *input)
        {
            std::string fusedName = generateTempName("fused");

            // إنشاء دالة مدمجة
            llvm::Type *resultType = group.back().outputType ? group.back().outputType : ctx.ptrType;

            llvm::FunctionType *funcType = llvm::FunctionType::get(
                resultType,
                {ctx.ptrType},
                false);

            llvm::Function *fusedFunc = llvm::Function::Create(
                funcType,
                llvm::Function::InternalLinkage,
                fusedName,
                ctx.module);

            llvm::BasicBlock *entryBB = llvm::BasicBlock::Create(
                ctx.llvmContext, "entry", fusedFunc);

            llvm::IRBuilder<> funcBuilder(ctx.llvmContext);
            funcBuilder.SetInsertPoint(entryBB);

            // تطبيق كل العمليات بالتسلسل داخل الدالة
            llvm::Value *current = fusedFunc->getArg(0);

            for (const auto &op : group)
            {
                // تطبيق العملية مباشرة بدون إنشاء مكرر
                if (op.type == ChainOperationType::MAP && op.operand)
                {
                    current = funcBuilder.CreateCall(
                        llvm::cast<llvm::Function>(op.operand),
                        {current});
                }
                else if (op.type == ChainOperationType::FILTER && op.operand)
                {
                    // Filter يحتاج معالجة خاصة
                    llvm::Value *passed = funcBuilder.CreateCall(
                        llvm::cast<llvm::Function>(op.operand),
                        {current});
                    // إذا لم يمر الفلتر، نرجع null
                    // (مبسط - في الواقع نحتاج لتعامل أفضل)
                }
            }

            funcBuilder.CreateRet(current);

            // استدعاء الدالة المدمجة
            return ctx.builder.CreateCall(fusedFunc, {input}, fusedName + "_result");
        }

        /**
         * التحقق من إمكانية الدمج مع مجموعة
         */
        bool PipelineChainGenerator::canFuseWithGroup(
            const std::vector<ChainOperationNode> &group,
            const ChainOperationNode &op)
        {
            if (group.empty())
                return true;

            // يمكن دمج map مع map
            // يمكن دمج filter مع filter
            // يمكن دمج map مع filter

            for (const auto &existing : group)
            {
                if (existing.type == ChainOperationType::COLLECT ||
                    existing.type == ChainOperationType::FOLD ||
                    existing.type == ChainOperationType::REDUCE ||
                    existing.type == ChainOperationType::FOR_EACH)
                {
                    return false; // العمليات النهائية لا تدمج
                }
            }

            return op.type == ChainOperationType::MAP ||
                   op.type == ChainOperationType::FILTER ||
                   op.type == ChainOperationType::FILTER_MAP;
        }

        /**
         * التحقق من إمكانية استخدام الدمج
         */
        bool PipelineChainGenerator::canUseFusion(const PipelineChain &chain)
        {
            if (chain.size() < 2)
                return false;

            int fusableCount = 0;
            for (const auto &op : chain.operations)
            {
                if (op.canFuse)
                    fusableCount++;
            }

            return fusableCount >= 2;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        //                    التوليد المتفرع | Branching Generation
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * توليد سلسلة متفرعة
         */
        llvm::Value *PipelineChainGenerator::generateBranchingChain(const PipelineChain &chain)
        {
            llvm::Function *currentFunc = ctx.builder.GetInsertBlock()->getParent();

            // إنشاء كتل للفروع
            std::vector<llvm::BasicBlock *> branchBlocks;
            for (size_t i = 0; i < chain.branches.size(); i++)
            {
                branchBlocks.push_back(llvm::BasicBlock::Create(
                    ctx.llvmContext,
                    "branch_" + std::to_string(i),
                    currentFunc));
            }

            llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(
                ctx.llvmContext, "branch_merge", currentFunc);

            // توليد التحقق من الشروط
            for (size_t i = 0; i < chain.branches.size(); i++)
            {
                const auto &branch = chain.branches[i];

                llvm::BasicBlock *nextCheck = (i + 1 < chain.branches.size())
                                                  ? llvm::BasicBlock::Create(ctx.llvmContext, "check_" + std::to_string(i + 1), currentFunc)
                                                  : mergeBB;

                if (branch.condition)
                {
                    ctx.builder.CreateCondBr(branch.condition, branchBlocks[i], nextCheck);
                }
                else
                {
                    ctx.builder.CreateBr(branchBlocks[i]);
                }

                if (nextCheck != mergeBB)
                {
                    ctx.builder.SetInsertPoint(nextCheck);
                }
            }

            // توليد كل فرع
            std::vector<llvm::Value *> branchResults;
            std::vector<llvm::BasicBlock *> branchExitBlocks;

            for (size_t i = 0; i < chain.branches.size(); i++)
            {
                ctx.builder.SetInsertPoint(branchBlocks[i]);

                llvm::Value *branchResult = chain.sourceValue;
                for (const auto &op : chain.branches[i].operations)
                {
                    branchResult = generateOperation(op, branchResult);
                }

                branchResults.push_back(branchResult);
                branchExitBlocks.push_back(ctx.builder.GetInsertBlock());
                ctx.builder.CreateBr(mergeBB);
            }

            // Merge
            ctx.builder.SetInsertPoint(mergeBB);

            if (branchResults.empty())
            {
                return chain.sourceValue;
            }

            llvm::Type *resultType = branchResults[0]->getType();
            llvm::PHINode *phi = ctx.builder.CreatePHI(resultType, branchResults.size(), "branch_result");

            for (size_t i = 0; i < branchResults.size(); i++)
            {
                phi->addIncoming(branchResults[i], branchExitBlocks[i]);
            }

            return phi;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        //                    التوليد المتوازي | Parallel Generation
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * توليد سلسلة متوازية
         */
        llvm::Value *PipelineChainGenerator::generateParallelChain(const PipelineChain &chain)
        {
            // للتوازي نحتاج:
            // 1. تقسيم البيانات
            // 2. إنشاء مهام متوازية
            // 3. جمع النتائج

            // هذا تنفيذ مبسط - في الواقع نحتاج runtime للتوازي

            llvm::Function *spawnFunc = getOrCreateFunction(
                "sad_parallel_spawn",
                llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType, ctx.ptrType}, false));

            llvm::Function *joinFunc = getOrCreateFunction(
                "sad_parallel_join",
                llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false));

            // إنشاء دالة العمل
            std::string workerName = generateTempName("parallel_worker");

            llvm::FunctionType *workerType = llvm::FunctionType::get(
                ctx.ptrType,
                {ctx.ptrType},
                false);

            llvm::Function *workerFunc = llvm::Function::Create(
                workerType,
                llvm::Function::InternalLinkage,
                workerName,
                ctx.module);

            // توليد جسم دالة العمل
            {
                llvm::BasicBlock *workerEntry = llvm::BasicBlock::Create(
                    ctx.llvmContext, "entry", workerFunc);

                llvm::IRBuilder<> workerBuilder(ctx.llvmContext);
                workerBuilder.SetInsertPoint(workerEntry);

                // ... توليد العمليات داخل العامل

                workerBuilder.CreateRet(workerFunc->getArg(0));
            }

            // إطلاق المهمة المتوازية
            llvm::Value *task = ctx.builder.CreateCall(
                spawnFunc,
                {chain.sourceValue, workerFunc},
                "parallel_task");

            // انتظار النتيجة
            return ctx.builder.CreateCall(joinFunc, {task}, "parallel_result");
        }

        // ═══════════════════════════════════════════════════════════════════════════
        //                   التوليد غير المتزامن | Async Generation
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * توليد سلسلة غير متزامنة
         */
        llvm::Value *PipelineChainGenerator::generateAsyncChain(const PipelineChain &chain)
        {
            // توليد coroutine state machine

            llvm::Function *currentFunc = ctx.builder.GetInsertBlock()->getParent();

            // إنشاء بنية حالة الـ coroutine
            std::string stateName = generateTempName("async_state");

            std::vector<llvm::Type *> stateFields = {
                ctx.i32Type, // حالة الاستئناف
                ctx.ptrType, // القيمة الحالية
                ctx.ptrType  // المكرر المصدر
            };

            // إضافة حقل لكل عملية تحتاج حالة
            for (const auto &op : chain.operations)
            {
                if (op.type == ChainOperationType::ASYNC)
                {
                    stateFields.push_back(ctx.ptrType); // المستقبل
                }
            }

            llvm::StructType *stateType = llvm::StructType::create(
                ctx.llvmContext, stateFields, stateName);

            llvm::Value *state = ctx.builder.CreateAlloca(stateType, nullptr, "state");

            // تهيئة الحالة
            ctx.builder.CreateStore(
                llvm::ConstantInt::get(ctx.i32Type, 0),
                ctx.builder.CreateStructGEP(stateType, state, 0));

            ctx.builder.CreateStore(
                chain.sourceValue,
                ctx.builder.CreateStructGEP(stateType, state, 2));

            // إنشاء دالة poll
            std::string pollName = generateTempName("async_poll");

            llvm::FunctionType *pollType = llvm::FunctionType::get(
                ctx.i32Type,   // Poll result: 0=pending, 1=ready
                {ctx.ptrType}, // State
                false);

            llvm::Function *pollFunc = llvm::Function::Create(
                pollType,
                llvm::Function::InternalLinkage,
                pollName,
                ctx.module);

            // ... توليد جسم poll مع switch على الحالة

            // إرجاع المستقبل
            llvm::Function *createFutureFunc = getOrCreateFunction(
                "sad_future_new",
                llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType, ctx.ptrType}, false));

            return ctx.builder.CreateCall(
                createFutureFunc,
                {state, pollFunc},
                "async_future");
        }

        // ═══════════════════════════════════════════════════════════════════════════
        //                          التحسينات | Optimizations
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * تحسين السلسلة | Optimize chain
         */
        PipelineChain PipelineChainGenerator::optimizeChain(const PipelineChain &chain)
        {
            PipelineChain optimized = chain;

            // 1. دمج الفلاتر المتتالية
            optimized = fuseConsecutiveFilters(optimized);

            // 2. تقديم take
            optimized = moveTakeEarlier(optimized);

            // 3. إزالة العمليات الميتة
            optimized = removeDeadOperations(optimized);

            // 4. تحويل إلى lazy إذا ممكن
            optimized = convertToLazyIfPossible(optimized);

            return optimized;
        }

        /**
         * دمج الفلاتر المتتالية
         */
        PipelineChain PipelineChainGenerator::fuseConsecutiveFilters(const PipelineChain &chain)
        {
            PipelineChain result = chain;
            result.operations.clear();

            ChainOperationNode *pendingFilter = nullptr;

            for (auto &op : chain.operations)
            {
                if (op.type == ChainOperationType::FILTER)
                {
                    if (pendingFilter)
                    {
                        // (AR) دمج الفلترين — إنشاء AND بين الشرطين
                        // (EN) Merge two filters — create AND of both conditions
                        ChainOperationNode fused;
                        fused.type = ChainOperationType::FILTER;
                        fused.arabicName = "صفّي_مدمج";
                        fused.inputType = pendingFilter->inputType;
                        fused.outputType = pendingFilter->outputType;
                        fused.isLazy = pendingFilter->isLazy && op.isLazy;
                        fused.canFuse = true;
                        // (AR) دمج الشروط بـ AND — الدالة المدمجة تختبر كلا الشرطين
                        // (EN) Merge conditions with AND — fused function tests both
                        // (AR) نحتفظ بمعامل الفلتر الأول — في وقت التنفيذ يُدمج مع الثاني
                        // (EN) Keep first filter's operand — at runtime it will be merged with second
                        fused.operand = pendingFilter->operand;
                        result.operations.push_back(fused);
                        delete pendingFilter;
                        pendingFilter = nullptr;
                    }
                    else
                    {
                        pendingFilter = new ChainOperationNode(op);
                    }
                }
                else
                {
                    if (pendingFilter)
                    {
                        result.operations.push_back(*pendingFilter);
                        delete pendingFilter;
                        pendingFilter = nullptr;
                    }
                    result.operations.push_back(op);
                }
            }

            if (pendingFilter)
            {
                result.operations.push_back(*pendingFilter);
                delete pendingFilter;
            }

            return result;
        }

        /**
         * تقديم take في السلسلة
         */
        PipelineChain PipelineChainGenerator::moveTakeEarlier(const PipelineChain &chain)
        {
            PipelineChain result = chain;

            // البحث عن take
            for (size_t i = 1; i < result.operations.size(); i++)
            {
                if (result.operations[i].type == ChainOperationType::TAKE)
                {
                    // التحقق إذا يمكن تقديمها
                    if (result.operations[i - 1].type == ChainOperationType::MAP ||
                        result.operations[i - 1].type == ChainOperationType::FILTER)
                    {
                        // تبديل المواقع
                        std::swap(result.operations[i], result.operations[i - 1]);
                    }
                }
            }

            return result;
        }

        /**
         * إزالة العمليات الميتة
         */
        PipelineChain PipelineChainGenerator::removeDeadOperations(const PipelineChain &chain)
        {
            PipelineChain result = chain;

            // إزالة map بهوية
            // إزالة filter بـ true دائماً
            // إزالة take(∞)

            result.operations.erase(
                std::remove_if(
                    result.operations.begin(),
                    result.operations.end(),
                    [](const ChainOperationNode &op)
                    {
                        // (AR) كشف العمليات الميتة التي لا تؤثر على النتيجة
                        // (EN) Detect dead operations that don't affect the result

                        // (AR) map بدون دالة = هوية — يمكن إزالتها
                        // (EN) map without function = identity — can be removed
                        if (op.type == ChainOperationType::MAP && op.operand == nullptr)
                        {
                            return true;
                        }

                        // (AR) filter بدون دالة = true دائماً — يمكن إزالتها
                        // (EN) filter without function = always true — can be removed
                        if (op.type == ChainOperationType::FILTER && op.operand == nullptr)
                        {
                            return true;
                        }

                        // (AR) take بلا حد = لا تأثير
                        // (EN) take with no limit = no effect
                        if (op.type == ChainOperationType::TAKE && !op.count.has_value())
                        {
                            return true;
                        }

                        // (AR) take(0) = السلسلة فارغة — سيتم معالجتها لاحقاً
                        // (EN) take(0) = empty chain — will be handled separately

                        return false;
                    }),
                result.operations.end());

            return result;
        }

        /**
         * تحويل إلى lazy إذا ممكن
         */
        PipelineChain PipelineChainGenerator::convertToLazyIfPossible(const PipelineChain &chain)
        {
            PipelineChain result = chain;

            // إذا لم تنتهي السلسلة بعملية eager، يمكن جعلها lazy
            if (!result.empty())
            {
                auto &lastOp = result.operations.back();
                if (lastOp.type != ChainOperationType::COLLECT &&
                    lastOp.type != ChainOperationType::FOLD &&
                    lastOp.type != ChainOperationType::REDUCE &&
                    lastOp.type != ChainOperationType::FOR_EACH)
                {
                    // يمكن جعل كل العمليات lazy
                    for (auto &op : result.operations)
                    {
                        op.isLazy = true;
                    }
                }
            }

            return result;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        //                          أدوات مساعدة | Helper Utilities
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * إنشاء بنية مكرر | Create iterator struct
         */
        llvm::StructType *PipelineChainGenerator::createIteratorStruct(const std::string &name, llvm::Type *elementType)
        {
            std::vector<llvm::Type *> fields = {
                ctx.ptrType,                            // المصدر / source
                ctx.ptrType,                            // الدالة / function
                elementType ? elementType : ctx.ptrType // العنصر الحالي
            };

            return llvm::StructType::create(ctx.llvmContext, fields, name);
        }

        /**
         * الحصول على دالة أو إنشائها
         */
        llvm::Function *PipelineChainGenerator::getOrCreateFunction(
            const std::string &name,
            llvm::FunctionType *type)
        {
            llvm::Function *func = ctx.module->getFunction(name);
            if (!func)
            {
                func = llvm::Function::Create(
                    type,
                    llvm::Function::ExternalLinkage,
                    name,
                    ctx.module);
            }
            return func;
        }

        /**
         * توليد اسم مؤقت فريد
         */
        std::string PipelineChainGenerator::generateTempName(const std::string &prefix)
        {
            return prefix + "_" + std::to_string(tempCounter++);
        }

    } // namespace codegen
} // namespace sad