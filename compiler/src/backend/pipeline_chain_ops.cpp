/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: pipeline_chain_ops.cpp                                               ║
 * ║   الوصف: تنفيذ العمليات الفردية لسلاسل الأنابيب                              ║
 * ║   المهمة: CW-05 — مُستخرج من pipeline_chain.cpp                            ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * (AR) يحتوي على تنفيذ العمليات الفردية:
 *      - عمليات التحويل: map, filter, filterMap, flatMap
 *      - عمليات التحديد: take, takeWhile, drop, dropWhile
 *      - عمليات الجمع: collect, fold, reduce, forEach, custom
 *
 * (EN) Contains individual operation implementations:
 *      - Transformation: map, filter, filterMap, flatMap
 *      - Limiting: take, takeWhile, drop, dropWhile
 *      - Collection: collect, fold, reduce, forEach, custom
 */

#include "../../include/backend/pipeline_chain.h"

#include <llvm/IR/Intrinsics.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>

namespace sad
{
    namespace codegen
    {
        //                     توليد عمليات التحويل | Transformation Generation
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * توليد map / حوّل
         */
        llvm::Value *PipelineChainGenerator::generateMap(const ChainOperationNode &op, llvm::Value *input)
        {
            // إنشاء بنية المكرر الجديدة
            std::string iterName = generateTempName("map_iter");

            // إنشاء نوع المكرر
            llvm::StructType *iterType = createIteratorStruct(iterName, op.outputType);

            // تخصيص المكرر
            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            // حفظ المصدر والدالة
            llvm::Value *sourcePtr = ctx.builder.CreateStructGEP(iterType, iter, 0);
            ctx.builder.CreateStore(input, sourcePtr);

            if (op.operand)
            {
                llvm::Value *funcPtr = ctx.builder.CreateStructGEP(iterType, iter, 1);
                ctx.builder.CreateStore(op.operand, funcPtr);
            }

            return iter;
        }

        /**
         * توليد filter / صفّي
         */
        llvm::Value *PipelineChainGenerator::generateFilter(const ChainOperationNode &op, llvm::Value *input)
        {
            std::string iterName = generateTempName("filter_iter");

            llvm::StructType *iterType = createIteratorStruct(iterName, op.outputType);
            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            // حفظ المصدر والشرط
            llvm::Value *sourcePtr = ctx.builder.CreateStructGEP(iterType, iter, 0);
            ctx.builder.CreateStore(input, sourcePtr);

            if (op.operand)
            {
                llvm::Value *predPtr = ctx.builder.CreateStructGEP(iterType, iter, 1);
                ctx.builder.CreateStore(op.operand, predPtr);
            }

            return iter;
        }

        /**
         * توليد filter_map / صفّي_حوّل
         */
        llvm::Value *PipelineChainGenerator::generateFilterMap(const ChainOperationNode &op, llvm::Value *input)
        {
            std::string iterName = generateTempName("filter_map_iter");

            llvm::StructType *iterType = createIteratorStruct(iterName, op.outputType);
            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            llvm::Value *sourcePtr = ctx.builder.CreateStructGEP(iterType, iter, 0);
            ctx.builder.CreateStore(input, sourcePtr);

            if (op.operand)
            {
                llvm::Value *funcPtr = ctx.builder.CreateStructGEP(iterType, iter, 1);
                ctx.builder.CreateStore(op.operand, funcPtr);
            }

            return iter;
        }

        /**
         * توليد flat_map / حوّل_مسطح
         */
        llvm::Value *PipelineChainGenerator::generateFlatMap(const ChainOperationNode &op, llvm::Value *input)
        {
            std::string iterName = generateTempName("flat_map_iter");

            // flat_map يحتاج لحالة إضافية للمكرر الداخلي
            std::vector<llvm::Type *> fields = {
                ctx.ptrType, // المصدر
                ctx.ptrType, // الدالة
                ctx.ptrType, // المكرر الداخلي الحالي
                ctx.i1Type   // هل المكرر الداخلي نشط
            };

            llvm::StructType *iterType = llvm::StructType::create(
                ctx.llvmContext, fields, iterName);

            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            // تهيئة الحقول
            ctx.builder.CreateStore(input,
                                    ctx.builder.CreateStructGEP(iterType, iter, 0));

            if (op.operand)
            {
                ctx.builder.CreateStore(op.operand,
                                        ctx.builder.CreateStructGEP(iterType, iter, 1));
            }

            // المكرر الداخلي يبدأ فارغاً
            ctx.builder.CreateStore(
                llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
                ctx.builder.CreateStructGEP(iterType, iter, 2));

            ctx.builder.CreateStore(
                llvm::ConstantInt::getFalse(ctx.llvmContext),
                ctx.builder.CreateStructGEP(iterType, iter, 3));

            return iter;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        //                     توليد عمليات التحديد | Limiting Generation
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * توليد take / خذ
         */
        llvm::Value *PipelineChainGenerator::generateTake(const ChainOperationNode &op, llvm::Value *input)
        {
            std::string iterName = generateTempName("take_iter");

            std::vector<llvm::Type *> fields = {
                ctx.ptrType, // المصدر
                ctx.i64Type  // العدد المتبقي
            };

            llvm::StructType *iterType = llvm::StructType::create(
                ctx.llvmContext, fields, iterName);

            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            ctx.builder.CreateStore(input,
                                    ctx.builder.CreateStructGEP(iterType, iter, 0));

            int64_t count = op.count.value_or(0);
            ctx.builder.CreateStore(
                llvm::ConstantInt::get(ctx.i64Type, count),
                ctx.builder.CreateStructGEP(iterType, iter, 1));

            return iter;
        }

        /**
         * توليد take_while / خذ_طالما
         */
        llvm::Value *PipelineChainGenerator::generateTakeWhile(const ChainOperationNode &op, llvm::Value *input)
        {
            std::string iterName = generateTempName("take_while_iter");

            std::vector<llvm::Type *> fields = {
                ctx.ptrType, // المصدر
                ctx.ptrType, // الشرط
                ctx.i1Type   // هل انتهى
            };

            llvm::StructType *iterType = llvm::StructType::create(
                ctx.llvmContext, fields, iterName);

            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            ctx.builder.CreateStore(input,
                                    ctx.builder.CreateStructGEP(iterType, iter, 0));

            if (op.operand)
            {
                ctx.builder.CreateStore(op.operand,
                                        ctx.builder.CreateStructGEP(iterType, iter, 1));
            }

            ctx.builder.CreateStore(
                llvm::ConstantInt::getFalse(ctx.llvmContext),
                ctx.builder.CreateStructGEP(iterType, iter, 2));

            return iter;
        }

        /**
         * توليد drop / أسقط
         */
        llvm::Value *PipelineChainGenerator::generateDrop(const ChainOperationNode &op, llvm::Value *input)
        {
            std::string iterName = generateTempName("drop_iter");

            std::vector<llvm::Type *> fields = {
                ctx.ptrType, // المصدر
                ctx.i64Type, // العدد للتخطي
                ctx.i1Type   // هل تم التخطي
            };

            llvm::StructType *iterType = llvm::StructType::create(
                ctx.llvmContext, fields, iterName);

            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            ctx.builder.CreateStore(input,
                                    ctx.builder.CreateStructGEP(iterType, iter, 0));

            int64_t count = op.count.value_or(0);
            ctx.builder.CreateStore(
                llvm::ConstantInt::get(ctx.i64Type, count),
                ctx.builder.CreateStructGEP(iterType, iter, 1));

            ctx.builder.CreateStore(
                llvm::ConstantInt::getFalse(ctx.llvmContext),
                ctx.builder.CreateStructGEP(iterType, iter, 2));

            return iter;
        }

        /**
         * توليد drop_while / أسقط_طالما
         */
        llvm::Value *PipelineChainGenerator::generateDropWhile(const ChainOperationNode &op, llvm::Value *input)
        {
            std::string iterName = generateTempName("drop_while_iter");

            std::vector<llvm::Type *> fields = {
                ctx.ptrType, // المصدر
                ctx.ptrType, // الشرط
                ctx.i1Type   // هل تم الإسقاط
            };

            llvm::StructType *iterType = llvm::StructType::create(
                ctx.llvmContext, fields, iterName);

            llvm::Value *iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);

            ctx.builder.CreateStore(input,
                                    ctx.builder.CreateStructGEP(iterType, iter, 0));

            if (op.operand)
            {
                ctx.builder.CreateStore(op.operand,
                                        ctx.builder.CreateStructGEP(iterType, iter, 1));
            }

            ctx.builder.CreateStore(
                llvm::ConstantInt::getFalse(ctx.llvmContext),
                ctx.builder.CreateStructGEP(iterType, iter, 2));

            return iter;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        //                     توليد عمليات الجمع | Collection Generation
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * توليد collect / اجمع
         */
        llvm::Value *PipelineChainGenerator::generateCollect(const ChainOperationNode &op, llvm::Value *input)
        {
            // إنشاء دالة التجميع
            std::string collectName = generateTempName("collect");

            // تخصيص القائمة الناتجة
            llvm::Function *allocListFunc = getOrCreateFunction(
                "sad_list_new",
                llvm::FunctionType::get(ctx.ptrType, {}, false));
            llvm::Value *resultList = ctx.builder.CreateCall(allocListFunc, {}, "result_list");

            // دالة الإضافة
            llvm::Function *appendFunc = getOrCreateFunction(
                "sad_list_append",
                llvm::FunctionType::get(ctx.voidType, {ctx.ptrType, ctx.ptrType}, false));

            // دالة الحصول على التالي
            llvm::Function *nextFunc = getOrCreateFunction(
                "sad_iterator_next",
                llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false));

            // إنشاء حلقة التجميع
            llvm::Function *currentFunc = ctx.builder.GetInsertBlock()->getParent();

            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(ctx.llvmContext, "collect_loop", currentFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(ctx.llvmContext, "collect_body", currentFunc);
            llvm::BasicBlock *endBB = llvm::BasicBlock::Create(ctx.llvmContext, "collect_end", currentFunc);

            ctx.builder.CreateBr(loopBB);

            // Loop header
            ctx.builder.SetInsertPoint(loopBB);
            llvm::Value *element = ctx.builder.CreateCall(nextFunc, {input}, "element");
            llvm::Value *isNull = ctx.builder.CreateICmpEQ(
                element,
                llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
                "is_null");
            ctx.builder.CreateCondBr(isNull, endBB, bodyBB);

            // Loop body
            ctx.builder.SetInsertPoint(bodyBB);
            ctx.builder.CreateCall(appendFunc, {resultList, element});
            ctx.builder.CreateBr(loopBB);

            // End
            ctx.builder.SetInsertPoint(endBB);

            return resultList;
        }

        /**
         * توليد fold / طيّ
         */
        llvm::Value *PipelineChainGenerator::generateFold(const ChainOperationNode &op, llvm::Value *input)
        {
            // fold يحتاج لقيمة ابتدائية ودالة تجميع
            // التنفيذ مشابه لـ collect لكن مع تراكم القيمة

            llvm::Function *nextFunc = getOrCreateFunction(
                "sad_iterator_next",
                llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false));

            llvm::Function *currentFunc = ctx.builder.GetInsertBlock()->getParent();

            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(ctx.llvmContext, "fold_loop", currentFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(ctx.llvmContext, "fold_body", currentFunc);
            llvm::BasicBlock *endBB = llvm::BasicBlock::Create(ctx.llvmContext, "fold_end", currentFunc);

            // تخصيص المتراكم
            llvm::Value *accumPtr = ctx.builder.CreateAlloca(ctx.ptrType, nullptr, "accum");
            // القيمة الابتدائية - يجب أن تأتي من op.operand أو معامل آخر
            ctx.builder.CreateStore(llvm::ConstantPointerNull::get(
                                        llvm::PointerType::get(ctx.llvmContext, 0)),
                                    accumPtr);

            ctx.builder.CreateBr(loopBB);

            // Loop
            ctx.builder.SetInsertPoint(loopBB);
            llvm::Value *element = ctx.builder.CreateCall(nextFunc, {input}, "element");
            llvm::Value *isNull = ctx.builder.CreateICmpEQ(
                element,
                llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
                "is_null");
            ctx.builder.CreateCondBr(isNull, endBB, bodyBB);

            // Body
            ctx.builder.SetInsertPoint(bodyBB);
            llvm::Value *accum = ctx.builder.CreateLoad(ctx.ptrType, accumPtr, "accum_val");

            // استدعاء دالة الطي
            if (op.operand)
            {
                llvm::Value *newAccum = ctx.builder.CreateCall(
                    llvm::cast<llvm::Function>(op.operand),
                    {accum, element},
                    "new_accum");
                ctx.builder.CreateStore(newAccum, accumPtr);
            }

            ctx.builder.CreateBr(loopBB);

            // End
            ctx.builder.SetInsertPoint(endBB);
            return ctx.builder.CreateLoad(ctx.ptrType, accumPtr, "result");
        }

        /**
         * توليد reduce / اختزل
         */
        llvm::Value *PipelineChainGenerator::generateReduce(const ChainOperationNode &op, llvm::Value *input)
        {
            // reduce مثل fold لكن بدون قيمة ابتدائية
            // يستخدم أول عنصر كقيمة ابتدائية

            llvm::Function *nextFunc = getOrCreateFunction(
                "sad_iterator_next",
                llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false));

            // الحصول على أول عنصر
            llvm::Value *first = ctx.builder.CreateCall(nextFunc, {input}, "first");

            // التحقق من أن المكرر ليس فارغاً
            llvm::Value *isNull = ctx.builder.CreateICmpEQ(
                first,
                llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
                "is_empty");

            llvm::Function *currentFunc = ctx.builder.GetInsertBlock()->getParent();
            llvm::BasicBlock *hasElementsBB = llvm::BasicBlock::Create(ctx.llvmContext, "has_elements", currentFunc);
            llvm::BasicBlock *emptyBB = llvm::BasicBlock::Create(ctx.llvmContext, "empty", currentFunc);

            ctx.builder.CreateCondBr(isNull, emptyBB, hasElementsBB);

            // Empty case
            ctx.builder.SetInsertPoint(emptyBB);
            llvm::Value *noneResult = llvm::ConstantPointerNull::get(
                llvm::PointerType::get(ctx.llvmContext, 0));

            // Has elements - continue with fold logic
            ctx.builder.SetInsertPoint(hasElementsBB);

            // ... مشابه لـ fold مع first كقيمة ابتدائية

            return first; // مبسط
        }

        /**
         * توليد for_each / لكل
         */
        llvm::Value *PipelineChainGenerator::generateForEach(const ChainOperationNode &op, llvm::Value *input)
        {
            llvm::Function *nextFunc = getOrCreateFunction(
                "sad_iterator_next",
                llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false));

            llvm::Function *currentFunc = ctx.builder.GetInsertBlock()->getParent();

            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(ctx.llvmContext, "foreach_loop", currentFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(ctx.llvmContext, "foreach_body", currentFunc);
            llvm::BasicBlock *endBB = llvm::BasicBlock::Create(ctx.llvmContext, "foreach_end", currentFunc);

            ctx.builder.CreateBr(loopBB);

            ctx.builder.SetInsertPoint(loopBB);
            llvm::Value *element = ctx.builder.CreateCall(nextFunc, {input}, "element");
            llvm::Value *isNull = ctx.builder.CreateICmpEQ(
                element,
                llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
                "is_null");
            ctx.builder.CreateCondBr(isNull, endBB, bodyBB);

            ctx.builder.SetInsertPoint(bodyBB);
            if (op.operand)
            {
                ctx.builder.CreateCall(
                    llvm::cast<llvm::Function>(op.operand),
                    {element});
            }
            ctx.builder.CreateBr(loopBB);

            ctx.builder.SetInsertPoint(endBB);

            // for_each يرجع unit/void
            return llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0));
        }

        /**
         * توليد عملية مخصصة
         */
        llvm::Value *PipelineChainGenerator::generateCustom(const ChainOperationNode &op, llvm::Value *input)
        {
            if (op.operand && llvm::isa<llvm::Function>(op.operand))
            {
                return ctx.builder.CreateCall(
                    llvm::cast<llvm::Function>(op.operand),
                    {input},
                    "custom_result");
            }
            return input;
        }

    } // namespace codegen
} // namespace sad