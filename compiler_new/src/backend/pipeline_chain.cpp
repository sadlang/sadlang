/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: pipeline_chain.cpp                                                   ║
 * ║   الوصف: التوليد الرئيسي لسلاسل الأنابيب والواجهة الخارجية                   ║
 * ║   المهمة: CW-05 — مُقلّص بعد استخراج العمليات والتحسينات                   ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * (AR) يحتوي على:
 *      - التوليد الرئيسي (generateChain): نقطة الدخول
 *      - التوليد التسلسلي (generateSequentialChain)
 *      - توزيع العمليات (generateOperation)
 *      - الواجهة الخارجية (PipelineChainInterface)
 *
 * (EN) Contains main chain generation, sequential generation,
 *      operation dispatch, and external interface.
 *
 * الملفات المرتبطة:
 *   - pipeline_chain.h: التعريفات والتصريحات
 *   - pipeline_chain_ops.cpp: العمليات الفردية
 *   - pipeline_chain_fusion.cpp: الدمج والتحسينات والأدوات
 */

#include "../../include/backend/pipeline_chain.h"

#include <llvm/IR/Intrinsics.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <variant>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════
//                   التوليد الرئيسي | Main Chain Generation
// ═══════════════════════════════════════════════════════════════════════════

/**
 * (AR) التوليد الرئيسي للسلسلة — نقطة الدخول
 * (EN) Main chain generation — entry point
 */
llvm::Value* PipelineChainGenerator::generateChain(const PipelineChain& chain) {
    if (chain.empty()) {
        return chain.sourceValue;
    }

    // تحسين السلسلة أولاً
    PipelineChain optimized = optimizeChain(chain);

    // اختيار استراتيجية التوليد
    if (optimized.isAsync) {
        return generateAsyncChain(optimized);
    }
    if (optimized.isParallel) {
        return generateParallelChain(optimized);
    }
    if (!optimized.branches.empty()) {
        return generateBranchingChain(optimized);
    }
    if (canUseFusion(optimized)) {
        return generateFusedChain(optimized);
    }

    // التوليد التسلسلي الافتراضي
    return generateSequentialChain(optimized);
}

/**
 * (AR) توليد تسلسلي — كل عملية تطبق بعد الأخرى
 * (EN) Sequential generation — each operation applied after the previous
 */
llvm::Value* PipelineChainGenerator::generateSequentialChain(const PipelineChain& chain) {
    llvm::Value* current = chain.sourceValue;

    for (const auto& op : chain.operations) {
        current = generateOperation(op, current);
        if (!current) {
            return nullptr;  // خطأ
        }
    }

    return current;
}

/**
 * (AR) توليد عملية واحدة — توزيع حسب النوع
 * (EN) Generate single operation — dispatch by type
 */
llvm::Value* PipelineChainGenerator::generateOperation(const ChainOperationNode& op, llvm::Value* input) {
    switch (op.type) {
        case ChainOperationType::MAP:
            return generateMap(op, input);
        case ChainOperationType::FILTER:
            return generateFilter(op, input);
        case ChainOperationType::FILTER_MAP:
            return generateFilterMap(op, input);
        case ChainOperationType::FLAT_MAP:
            return generateFlatMap(op, input);
        case ChainOperationType::TAKE:
            return generateTake(op, input);
        case ChainOperationType::TAKE_WHILE:
            return generateTakeWhile(op, input);
        case ChainOperationType::DROP:
            return generateDrop(op, input);
        case ChainOperationType::DROP_WHILE:
            return generateDropWhile(op, input);
        case ChainOperationType::COLLECT:
            return generateCollect(op, input);
        case ChainOperationType::FOLD:
            return generateFold(op, input);
        case ChainOperationType::REDUCE:
            return generateReduce(op, input);
        case ChainOperationType::FOR_EACH:
            return generateForEach(op, input);
        default:
            return generateCustom(op, input);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                     الواجهة الخارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════

/**
 * (AR) توليد سلسلة أنابيب كاملة
 * (EN) Generate a complete pipeline chain
 */
llvm::Value* PipelineChainInterface::generatePipelineChain(
    llvm::LLVMContext& llvmContext,
    llvm::Module* module,
    llvm::IRBuilder<>& builder,
    const PipelineChain& chain
) {
    ChainContext ctx(llvmContext, module, builder);
    PipelineChainGenerator generator(ctx);
    return generator.generateChain(chain);
}

/**
 * (AR) إنشاء عقدة عملية من AST
 * (EN) Create operation node from AST
 */
ChainOperationNode PipelineChainInterface::createOperationFromAST(
    const std::string& opName,
    const std::string& arabicName,
    llvm::Value* operand,
    llvm::Type* inputType,
    llvm::Type* outputType
) {
    ChainOperationNode node;
    node.functionName = opName;
    node.arabicName = arabicName;
    node.operand = operand;
    node.inputType = inputType;
    node.outputType = outputType;

    // تحديد النوع من الاسم
    static const std::unordered_map<std::string, ChainOperationType> nameToType = {
        {"حوّل", ChainOperationType::MAP},
        {"map", ChainOperationType::MAP},
        {"صفّي", ChainOperationType::FILTER},
        {"filter", ChainOperationType::FILTER},
        {"صفّي_حوّل", ChainOperationType::FILTER_MAP},
        {"filter_map", ChainOperationType::FILTER_MAP},
        {"حوّل_مسطح", ChainOperationType::FLAT_MAP},
        {"flat_map", ChainOperationType::FLAT_MAP},
        {"خذ", ChainOperationType::TAKE},
        {"take", ChainOperationType::TAKE},
        {"خذ_طالما", ChainOperationType::TAKE_WHILE},
        {"take_while", ChainOperationType::TAKE_WHILE},
        {"أسقط", ChainOperationType::DROP},
        {"drop", ChainOperationType::DROP},
        {"skip", ChainOperationType::DROP},
        {"أسقط_طالما", ChainOperationType::DROP_WHILE},
        {"drop_while", ChainOperationType::DROP_WHILE},
        {"skip_while", ChainOperationType::DROP_WHILE},
        {"اجمع", ChainOperationType::COLLECT},
        {"collect", ChainOperationType::COLLECT},
        {"طيّ", ChainOperationType::FOLD},
        {"fold", ChainOperationType::FOLD},
        {"اختزل", ChainOperationType::REDUCE},
        {"reduce", ChainOperationType::REDUCE},
        {"لكل", ChainOperationType::FOR_EACH},
        {"for_each", ChainOperationType::FOR_EACH}
    };

    auto it = nameToType.find(arabicName);
    if (it != nameToType.end()) {
        node.type = it->second;
    } else {
        it = nameToType.find(opName);
        if (it != nameToType.end()) {
            node.type = it->second;
        }
    }

    // تحديد خصائص العملية
    node.canFuse = (node.type == ChainOperationType::MAP ||
                    node.type == ChainOperationType::FILTER ||
                    node.type == ChainOperationType::FILTER_MAP);

    node.isLazy = (node.type != ChainOperationType::COLLECT &&
                   node.type != ChainOperationType::FOLD &&
                   node.type != ChainOperationType::REDUCE &&
                   node.type != ChainOperationType::FOR_EACH);

    return node;
}

/**
 * (AR) إنشاء سلسلة من قائمة عمليات
 * (EN) Create chain from a list of operations
 */
PipelineChain PipelineChainInterface::createChainFromOperations(
    const std::string& name,
    llvm::Value* source,
    llvm::Type* sourceType,
    const std::vector<ChainOperationNode>& operations
) {
    PipelineChain chain;
    chain.name = name;
    chain.sourceValue = source;
    chain.sourceType = sourceType;
    chain.operations = operations;
    chain.isParallel = false;
    chain.isAsync = false;

    // التحقق من وجود عمليات async
    for (const auto& op : operations) {
        if (op.type == ChainOperationType::ASYNC) {
            chain.isAsync = true;
            break;
        }
        if (op.type == ChainOperationType::PARALLEL) {
            chain.isParallel = true;
        }
    }

    return chain;
}

} // namespace codegen
} // namespace sad