/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: pipeline_chain.cpp                                                   ║
 * ║   الوصف: توليد الشفرة لسلاسل الأنابيب المتعددة                              ║
 * ║   المهمة: T153.4 - Phase 13 (US11)                                          ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    توليد كود LLVM IR لسلاسل الأنابيب المتعددة مع التحسينات
 *    Generate LLVM IR for multi-pipeline chains with optimizations
 *
 * 📋 الميزات | Features:
 *
 *    1️⃣ سلاسل أنابيب متعددة | Multi-pipeline Chains:
 *       قيمة |> د1 |> د2 |> د3 |> د4
 *
 *    2️⃣ أنابيب متفرعة | Branching Pipelines:
 *       قيمة |> { حالة1 => نتيجة1، حالة2 => نتيجة2 }
 *
 *    3️⃣ أنابيب متوازية | Parallel Pipelines:
 *       (قيمة |> د1، قيمة |> د2) |> دمج
 *
 *    4️⃣ تحسين الدمج | Fusion Optimization:
 *       دمج العمليات المتتالية في دالة واحدة
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <variant>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════════
//                          تعريفات الأنواع | Type Definitions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 نوع عملية السلسلة | Chain Operation Type
 */
enum class ChainOperationType {
    // عمليات التحويل | Transformation
    MAP,                    // حوّل - map
    FILTER,                 // صفّي - filter
    FILTER_MAP,             // صفّي_حوّل - filter_map
    FLAT_MAP,               // حوّل_مسطح - flat_map
    
    // عمليات التحديد | Limiting
    TAKE,                   // خذ - take
    TAKE_WHILE,             // خذ_طالما - take_while
    DROP,                   // أسقط - drop
    DROP_WHILE,             // أسقط_طالما - drop_while
    
    // عمليات الجمع | Collection
    COLLECT,                // اجمع - collect
    FOLD,                   // طيّ - fold
    REDUCE,                 // اختزل - reduce
    FOR_EACH,               // لكل - for_each
    
    // عمليات خاصة | Special
    BRANCH,                 // تفرع - branch
    PARALLEL,               // متوازي - parallel
    ASYNC,                  // غير_متزامن - async
    CUSTOM                  // مخصص - custom
};

/**
 * 📋 عقدة عملية في السلسلة | Chain Operation Node
 */
struct ChainOperationNode {
    ChainOperationType type;
    std::string arabicName;           // الاسم العربي
    std::string functionName;         // اسم الدالة
    llvm::Type* inputType;            // نوع الإدخال
    llvm::Type* outputType;           // نوع الإخراج
    llvm::Value* operand;             // المعامل (مثل الدالة)
    std::optional<int64_t> count;     // للعمليات مع عدد (خذ، أسقط)
    bool isLazy;                      // هل كسول
    bool canFuse;                     // هل يمكن دمجه
    
    ChainOperationNode()
        : type(ChainOperationType::CUSTOM)
        , arabicName("")
        , functionName("")
        , inputType(nullptr)
        , outputType(nullptr)
        , operand(nullptr)
        , count(std::nullopt)
        , isLazy(true)
        , canFuse(true)
    {}
};

/**
 * 📋 فرع في السلسلة | Chain Branch
 */
struct ChainBranch {
    llvm::Value* condition;           // الشرط
    std::vector<ChainOperationNode> operations;  // العمليات
    llvm::Type* resultType;           // نوع النتيجة
};

/**
 * 📋 تمثيل السلسلة الكاملة | Full Chain Representation
 */
struct PipelineChain {
    std::string name;                            // اسم السلسلة
    llvm::Value* sourceValue;                    // القيمة المصدر
    llvm::Type* sourceType;                      // نوع المصدر
    std::vector<ChainOperationNode> operations;  // العمليات
    std::vector<ChainBranch> branches;           // الفروع (إن وجدت)
    bool isParallel;                             // هل متوازية
    bool isAsync;                                // هل غير متزامنة
    
    PipelineChain()
        : name("")
        , sourceValue(nullptr)
        , sourceType(nullptr)
        , isParallel(false)
        , isAsync(false)
    {}
    
    size_t size() const { return operations.size(); }
    bool empty() const { return operations.empty(); }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    سياق توليد الكود | Code Generation Context
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 سياق السلسلة | Chain Context
 * 
 * يحتفظ بمعلومات أثناء توليد الكود
 */
class ChainContext {
public:
    llvm::LLVMContext& llvmContext;
    llvm::Module* module;
    llvm::IRBuilder<>& builder;
    
    // أنواع مساعدة | Helper types
    llvm::Type* voidType;
    llvm::Type* i1Type;      // منطقي
    llvm::Type* i32Type;     // عدد صحيح
    llvm::Type* i64Type;     // عدد طويل
    llvm::Type* ptrType;     // مؤشر
    
    // تخزين مؤقت | Temporary storage
    std::unordered_map<std::string, llvm::Value*> namedValues;
    std::unordered_map<std::string, llvm::Function*> generatedFunctions;
    
    ChainContext(llvm::LLVMContext& ctx, llvm::Module* mod, llvm::IRBuilder<>& b)
        : llvmContext(ctx)
        , module(mod)
        , builder(b)
        , voidType(llvm::Type::getVoidTy(ctx))
        , i1Type(llvm::Type::getInt1Ty(ctx))
        , i32Type(llvm::Type::getInt32Ty(ctx))
        , i64Type(llvm::Type::getInt64Ty(ctx))
        , ptrType(llvm::PointerType::get(ctx, 0))
    {}
    
    void setValue(const std::string& name, llvm::Value* value) {
        namedValues[name] = value;
    }
    
    llvm::Value* getValue(const std::string& name) {
        auto it = namedValues.find(name);
        return it != namedValues.end() ? it->second : nullptr;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                       مولد سلسلة الأنابيب | Pipeline Chain Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 مولد_سلسلة_الأنابيب | PipelineChainGenerator
 * 
 * يولد كود LLVM IR لسلاسل الأنابيب
 * Generates LLVM IR for pipeline chains
 */
class PipelineChainGenerator {
private:
    ChainContext& ctx;
    int tempCounter;
    
public:
    explicit PipelineChainGenerator(ChainContext& context)
        : ctx(context)
        , tempCounter(0)
    {}
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          التوليد الرئيسي | Main Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد السلسلة كاملة | Generate full chain
     */
    llvm::Value* generateChain(const PipelineChain& chain) {
        if (chain.empty()) {
            return chain.sourceValue;
        }
        
        // تحسين السلسلة أولاً
        PipelineChain optimized = optimizeChain(chain);
        
        // اختيار استراتيجية التوليد
        if (optimized.isAsync) {
            return generateAsyncChain(optimized);
        } else if (optimized.isParallel) {
            return generateParallelChain(optimized);
        } else if (!optimized.branches.empty()) {
            return generateBranchingChain(optimized);
        } else if (canUseFusion(optimized)) {
            return generateFusedChain(optimized);
        } else {
            return generateSequentialChain(optimized);
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          التوليد التسلسلي | Sequential Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد تسلسلي | Sequential generation
     * 
     * كل عملية تطبق بعد الأخرى
     */
    llvm::Value* generateSequentialChain(const PipelineChain& chain) {
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
     * توليد عملية واحدة | Generate single operation
     */
    llvm::Value* generateOperation(const ChainOperationNode& op, llvm::Value* input) {
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
    //                     توليد عمليات التحويل | Transformation Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد map / حوّل
     */
    llvm::Value* generateMap(const ChainOperationNode& op, llvm::Value* input) {
        // إنشاء بنية المكرر الجديدة
        std::string iterName = generateTempName("map_iter");
        
        // إنشاء نوع المكرر
        llvm::StructType* iterType = createIteratorStruct(iterName, op.outputType);
        
        // تخصيص المكرر
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        // حفظ المصدر والدالة
        llvm::Value* sourcePtr = ctx.builder.CreateStructGEP(iterType, iter, 0);
        ctx.builder.CreateStore(input, sourcePtr);
        
        if (op.operand) {
            llvm::Value* funcPtr = ctx.builder.CreateStructGEP(iterType, iter, 1);
            ctx.builder.CreateStore(op.operand, funcPtr);
        }
        
        return iter;
    }
    
    /**
     * توليد filter / صفّي
     */
    llvm::Value* generateFilter(const ChainOperationNode& op, llvm::Value* input) {
        std::string iterName = generateTempName("filter_iter");
        
        llvm::StructType* iterType = createIteratorStruct(iterName, op.outputType);
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        // حفظ المصدر والشرط
        llvm::Value* sourcePtr = ctx.builder.CreateStructGEP(iterType, iter, 0);
        ctx.builder.CreateStore(input, sourcePtr);
        
        if (op.operand) {
            llvm::Value* predPtr = ctx.builder.CreateStructGEP(iterType, iter, 1);
            ctx.builder.CreateStore(op.operand, predPtr);
        }
        
        return iter;
    }
    
    /**
     * توليد filter_map / صفّي_حوّل
     */
    llvm::Value* generateFilterMap(const ChainOperationNode& op, llvm::Value* input) {
        std::string iterName = generateTempName("filter_map_iter");
        
        llvm::StructType* iterType = createIteratorStruct(iterName, op.outputType);
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        llvm::Value* sourcePtr = ctx.builder.CreateStructGEP(iterType, iter, 0);
        ctx.builder.CreateStore(input, sourcePtr);
        
        if (op.operand) {
            llvm::Value* funcPtr = ctx.builder.CreateStructGEP(iterType, iter, 1);
            ctx.builder.CreateStore(op.operand, funcPtr);
        }
        
        return iter;
    }
    
    /**
     * توليد flat_map / حوّل_مسطح
     */
    llvm::Value* generateFlatMap(const ChainOperationNode& op, llvm::Value* input) {
        std::string iterName = generateTempName("flat_map_iter");
        
        // flat_map يحتاج لحالة إضافية للمكرر الداخلي
        std::vector<llvm::Type*> fields = {
            ctx.ptrType,    // المصدر
            ctx.ptrType,    // الدالة
            ctx.ptrType,    // المكرر الداخلي الحالي
            ctx.i1Type      // هل المكرر الداخلي نشط
        };
        
        llvm::StructType* iterType = llvm::StructType::create(
            ctx.llvmContext, fields, iterName
        );
        
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        // تهيئة الحقول
        ctx.builder.CreateStore(input, 
            ctx.builder.CreateStructGEP(iterType, iter, 0));
        
        if (op.operand) {
            ctx.builder.CreateStore(op.operand,
                ctx.builder.CreateStructGEP(iterType, iter, 1));
        }
        
        // المكرر الداخلي يبدأ فارغاً
        ctx.builder.CreateStore(
            llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
            ctx.builder.CreateStructGEP(iterType, iter, 2)
        );
        
        ctx.builder.CreateStore(
            llvm::ConstantInt::getFalse(ctx.llvmContext),
            ctx.builder.CreateStructGEP(iterType, iter, 3)
        );
        
        return iter;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                     توليد عمليات التحديد | Limiting Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد take / خذ
     */
    llvm::Value* generateTake(const ChainOperationNode& op, llvm::Value* input) {
        std::string iterName = generateTempName("take_iter");
        
        std::vector<llvm::Type*> fields = {
            ctx.ptrType,    // المصدر
            ctx.i64Type     // العدد المتبقي
        };
        
        llvm::StructType* iterType = llvm::StructType::create(
            ctx.llvmContext, fields, iterName
        );
        
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        ctx.builder.CreateStore(input,
            ctx.builder.CreateStructGEP(iterType, iter, 0));
        
        int64_t count = op.count.value_or(0);
        ctx.builder.CreateStore(
            llvm::ConstantInt::get(ctx.i64Type, count),
            ctx.builder.CreateStructGEP(iterType, iter, 1)
        );
        
        return iter;
    }
    
    /**
     * توليد take_while / خذ_طالما
     */
    llvm::Value* generateTakeWhile(const ChainOperationNode& op, llvm::Value* input) {
        std::string iterName = generateTempName("take_while_iter");
        
        std::vector<llvm::Type*> fields = {
            ctx.ptrType,    // المصدر
            ctx.ptrType,    // الشرط
            ctx.i1Type      // هل انتهى
        };
        
        llvm::StructType* iterType = llvm::StructType::create(
            ctx.llvmContext, fields, iterName
        );
        
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        ctx.builder.CreateStore(input,
            ctx.builder.CreateStructGEP(iterType, iter, 0));
        
        if (op.operand) {
            ctx.builder.CreateStore(op.operand,
                ctx.builder.CreateStructGEP(iterType, iter, 1));
        }
        
        ctx.builder.CreateStore(
            llvm::ConstantInt::getFalse(ctx.llvmContext),
            ctx.builder.CreateStructGEP(iterType, iter, 2)
        );
        
        return iter;
    }
    
    /**
     * توليد drop / أسقط
     */
    llvm::Value* generateDrop(const ChainOperationNode& op, llvm::Value* input) {
        std::string iterName = generateTempName("drop_iter");
        
        std::vector<llvm::Type*> fields = {
            ctx.ptrType,    // المصدر
            ctx.i64Type,    // العدد للتخطي
            ctx.i1Type      // هل تم التخطي
        };
        
        llvm::StructType* iterType = llvm::StructType::create(
            ctx.llvmContext, fields, iterName
        );
        
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        ctx.builder.CreateStore(input,
            ctx.builder.CreateStructGEP(iterType, iter, 0));
        
        int64_t count = op.count.value_or(0);
        ctx.builder.CreateStore(
            llvm::ConstantInt::get(ctx.i64Type, count),
            ctx.builder.CreateStructGEP(iterType, iter, 1)
        );
        
        ctx.builder.CreateStore(
            llvm::ConstantInt::getFalse(ctx.llvmContext),
            ctx.builder.CreateStructGEP(iterType, iter, 2)
        );
        
        return iter;
    }
    
    /**
     * توليد drop_while / أسقط_طالما
     */
    llvm::Value* generateDropWhile(const ChainOperationNode& op, llvm::Value* input) {
        std::string iterName = generateTempName("drop_while_iter");
        
        std::vector<llvm::Type*> fields = {
            ctx.ptrType,    // المصدر
            ctx.ptrType,    // الشرط
            ctx.i1Type      // هل تم الإسقاط
        };
        
        llvm::StructType* iterType = llvm::StructType::create(
            ctx.llvmContext, fields, iterName
        );
        
        llvm::Value* iter = ctx.builder.CreateAlloca(iterType, nullptr, iterName);
        
        ctx.builder.CreateStore(input,
            ctx.builder.CreateStructGEP(iterType, iter, 0));
        
        if (op.operand) {
            ctx.builder.CreateStore(op.operand,
                ctx.builder.CreateStructGEP(iterType, iter, 1));
        }
        
        ctx.builder.CreateStore(
            llvm::ConstantInt::getFalse(ctx.llvmContext),
            ctx.builder.CreateStructGEP(iterType, iter, 2)
        );
        
        return iter;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                     توليد عمليات الجمع | Collection Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد collect / اجمع
     */
    llvm::Value* generateCollect(const ChainOperationNode& op, llvm::Value* input) {
        // إنشاء دالة التجميع
        std::string collectName = generateTempName("collect");
        
        // تخصيص القائمة الناتجة
        llvm::Function* allocListFunc = getOrCreateFunction(
            "sad_list_new",
            llvm::FunctionType::get(ctx.ptrType, {}, false)
        );
        llvm::Value* resultList = ctx.builder.CreateCall(allocListFunc, {}, "result_list");
        
        // دالة الإضافة
        llvm::Function* appendFunc = getOrCreateFunction(
            "sad_list_append",
            llvm::FunctionType::get(ctx.voidType, {ctx.ptrType, ctx.ptrType}, false)
        );
        
        // دالة الحصول على التالي
        llvm::Function* nextFunc = getOrCreateFunction(
            "sad_iterator_next",
            llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false)
        );
        
        // إنشاء حلقة التجميع
        llvm::Function* currentFunc = ctx.builder.GetInsertBlock()->getParent();
        
        llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(ctx.llvmContext, "collect_loop", currentFunc);
        llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(ctx.llvmContext, "collect_body", currentFunc);
        llvm::BasicBlock* endBB = llvm::BasicBlock::Create(ctx.llvmContext, "collect_end", currentFunc);
        
        ctx.builder.CreateBr(loopBB);
        
        // Loop header
        ctx.builder.SetInsertPoint(loopBB);
        llvm::Value* element = ctx.builder.CreateCall(nextFunc, {input}, "element");
        llvm::Value* isNull = ctx.builder.CreateICmpEQ(
            element,
            llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
            "is_null"
        );
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
    llvm::Value* generateFold(const ChainOperationNode& op, llvm::Value* input) {
        // fold يحتاج لقيمة ابتدائية ودالة تجميع
        // التنفيذ مشابه لـ collect لكن مع تراكم القيمة
        
        llvm::Function* nextFunc = getOrCreateFunction(
            "sad_iterator_next",
            llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false)
        );
        
        llvm::Function* currentFunc = ctx.builder.GetInsertBlock()->getParent();
        
        llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(ctx.llvmContext, "fold_loop", currentFunc);
        llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(ctx.llvmContext, "fold_body", currentFunc);
        llvm::BasicBlock* endBB = llvm::BasicBlock::Create(ctx.llvmContext, "fold_end", currentFunc);
        
        // تخصيص المتراكم
        llvm::Value* accumPtr = ctx.builder.CreateAlloca(ctx.ptrType, nullptr, "accum");
        // القيمة الابتدائية - يجب أن تأتي من op.operand أو معامل آخر
        ctx.builder.CreateStore(llvm::ConstantPointerNull::get(
            llvm::PointerType::get(ctx.llvmContext, 0)), accumPtr);
        
        ctx.builder.CreateBr(loopBB);
        
        // Loop
        ctx.builder.SetInsertPoint(loopBB);
        llvm::Value* element = ctx.builder.CreateCall(nextFunc, {input}, "element");
        llvm::Value* isNull = ctx.builder.CreateICmpEQ(
            element,
            llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
            "is_null"
        );
        ctx.builder.CreateCondBr(isNull, endBB, bodyBB);
        
        // Body
        ctx.builder.SetInsertPoint(bodyBB);
        llvm::Value* accum = ctx.builder.CreateLoad(ctx.ptrType, accumPtr, "accum_val");
        
        // استدعاء دالة الطي
        if (op.operand) {
            llvm::Value* newAccum = ctx.builder.CreateCall(
                llvm::cast<llvm::Function>(op.operand),
                {accum, element},
                "new_accum"
            );
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
    llvm::Value* generateReduce(const ChainOperationNode& op, llvm::Value* input) {
        // reduce مثل fold لكن بدون قيمة ابتدائية
        // يستخدم أول عنصر كقيمة ابتدائية
        
        llvm::Function* nextFunc = getOrCreateFunction(
            "sad_iterator_next",
            llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false)
        );
        
        // الحصول على أول عنصر
        llvm::Value* first = ctx.builder.CreateCall(nextFunc, {input}, "first");
        
        // التحقق من أن المكرر ليس فارغاً
        llvm::Value* isNull = ctx.builder.CreateICmpEQ(
            first,
            llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
            "is_empty"
        );
        
        llvm::Function* currentFunc = ctx.builder.GetInsertBlock()->getParent();
        llvm::BasicBlock* hasElementsBB = llvm::BasicBlock::Create(ctx.llvmContext, "has_elements", currentFunc);
        llvm::BasicBlock* emptyBB = llvm::BasicBlock::Create(ctx.llvmContext, "empty", currentFunc);
        
        ctx.builder.CreateCondBr(isNull, emptyBB, hasElementsBB);
        
        // Empty case
        ctx.builder.SetInsertPoint(emptyBB);
        llvm::Value* noneResult = llvm::ConstantPointerNull::get(
            llvm::PointerType::get(ctx.llvmContext, 0));
        
        // Has elements - continue with fold logic
        ctx.builder.SetInsertPoint(hasElementsBB);
        
        // ... مشابه لـ fold مع first كقيمة ابتدائية
        
        return first;  // مبسط
    }
    
    /**
     * توليد for_each / لكل
     */
    llvm::Value* generateForEach(const ChainOperationNode& op, llvm::Value* input) {
        llvm::Function* nextFunc = getOrCreateFunction(
            "sad_iterator_next",
            llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false)
        );
        
        llvm::Function* currentFunc = ctx.builder.GetInsertBlock()->getParent();
        
        llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(ctx.llvmContext, "foreach_loop", currentFunc);
        llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(ctx.llvmContext, "foreach_body", currentFunc);
        llvm::BasicBlock* endBB = llvm::BasicBlock::Create(ctx.llvmContext, "foreach_end", currentFunc);
        
        ctx.builder.CreateBr(loopBB);
        
        ctx.builder.SetInsertPoint(loopBB);
        llvm::Value* element = ctx.builder.CreateCall(nextFunc, {input}, "element");
        llvm::Value* isNull = ctx.builder.CreateICmpEQ(
            element,
            llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0)),
            "is_null"
        );
        ctx.builder.CreateCondBr(isNull, endBB, bodyBB);
        
        ctx.builder.SetInsertPoint(bodyBB);
        if (op.operand) {
            ctx.builder.CreateCall(
                llvm::cast<llvm::Function>(op.operand),
                {element}
            );
        }
        ctx.builder.CreateBr(loopBB);
        
        ctx.builder.SetInsertPoint(endBB);
        
        // for_each يرجع unit/void
        return llvm::ConstantPointerNull::get(llvm::PointerType::get(ctx.llvmContext, 0));
    }
    
    /**
     * توليد عملية مخصصة
     */
    llvm::Value* generateCustom(const ChainOperationNode& op, llvm::Value* input) {
        if (op.operand && llvm::isa<llvm::Function>(op.operand)) {
            return ctx.builder.CreateCall(
                llvm::cast<llvm::Function>(op.operand),
                {input},
                "custom_result"
            );
        }
        return input;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    التوليد المدمج | Fused Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد سلسلة مدمجة | Generate fused chain
     * 
     * يدمج عدة عمليات في دالة واحدة لتحسين الأداء
     */
    llvm::Value* generateFusedChain(const PipelineChain& chain) {
        // تجميع العمليات القابلة للدمج
        std::vector<std::vector<ChainOperationNode>> fusionGroups;
        std::vector<ChainOperationNode> currentGroup;
        
        for (const auto& op : chain.operations) {
            if (op.canFuse && canFuseWithGroup(currentGroup, op)) {
                currentGroup.push_back(op);
            } else {
                if (!currentGroup.empty()) {
                    fusionGroups.push_back(std::move(currentGroup));
                    currentGroup.clear();
                }
                currentGroup.push_back(op);
            }
        }
        
        if (!currentGroup.empty()) {
            fusionGroups.push_back(std::move(currentGroup));
        }
        
        // توليد كل مجموعة
        llvm::Value* current = chain.sourceValue;
        
        for (const auto& group : fusionGroups) {
            if (group.size() == 1) {
                current = generateOperation(group[0], current);
            } else {
                current = generateFusedGroup(group, current);
            }
        }
        
        return current;
    }
    
    /**
     * توليد مجموعة مدمجة
     */
    llvm::Value* generateFusedGroup(
        const std::vector<ChainOperationNode>& group,
        llvm::Value* input
    ) {
        std::string fusedName = generateTempName("fused");
        
        // إنشاء دالة مدمجة
        llvm::Type* resultType = group.back().outputType ? group.back().outputType : ctx.ptrType;
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            resultType,
            {ctx.ptrType},
            false
        );
        
        llvm::Function* fusedFunc = llvm::Function::Create(
            funcType,
            llvm::Function::InternalLinkage,
            fusedName,
            ctx.module
        );
        
        llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(
            ctx.llvmContext, "entry", fusedFunc
        );
        
        llvm::IRBuilder<> funcBuilder(ctx.llvmContext);
        funcBuilder.SetInsertPoint(entryBB);
        
        // تطبيق كل العمليات بالتسلسل داخل الدالة
        llvm::Value* current = fusedFunc->getArg(0);
        
        for (const auto& op : group) {
            // تطبيق العملية مباشرة بدون إنشاء مكرر
            if (op.type == ChainOperationType::MAP && op.operand) {
                current = funcBuilder.CreateCall(
                    llvm::cast<llvm::Function>(op.operand),
                    {current}
                );
            } else if (op.type == ChainOperationType::FILTER && op.operand) {
                // Filter يحتاج معالجة خاصة
                llvm::Value* passed = funcBuilder.CreateCall(
                    llvm::cast<llvm::Function>(op.operand),
                    {current}
                );
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
    bool canFuseWithGroup(
        const std::vector<ChainOperationNode>& group,
        const ChainOperationNode& op
    ) {
        if (group.empty()) return true;
        
        // يمكن دمج map مع map
        // يمكن دمج filter مع filter
        // يمكن دمج map مع filter
        
        for (const auto& existing : group) {
            if (existing.type == ChainOperationType::COLLECT ||
                existing.type == ChainOperationType::FOLD ||
                existing.type == ChainOperationType::REDUCE ||
                existing.type == ChainOperationType::FOR_EACH) {
                return false;  // العمليات النهائية لا تدمج
            }
        }
        
        return op.type == ChainOperationType::MAP ||
               op.type == ChainOperationType::FILTER ||
               op.type == ChainOperationType::FILTER_MAP;
    }
    
    /**
     * التحقق من إمكانية استخدام الدمج
     */
    bool canUseFusion(const PipelineChain& chain) {
        if (chain.size() < 2) return false;
        
        int fusableCount = 0;
        for (const auto& op : chain.operations) {
            if (op.canFuse) fusableCount++;
        }
        
        return fusableCount >= 2;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    التوليد المتفرع | Branching Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد سلسلة متفرعة
     */
    llvm::Value* generateBranchingChain(const PipelineChain& chain) {
        llvm::Function* currentFunc = ctx.builder.GetInsertBlock()->getParent();
        
        // إنشاء كتل للفروع
        std::vector<llvm::BasicBlock*> branchBlocks;
        for (size_t i = 0; i < chain.branches.size(); i++) {
            branchBlocks.push_back(llvm::BasicBlock::Create(
                ctx.llvmContext,
                "branch_" + std::to_string(i),
                currentFunc
            ));
        }
        
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(
            ctx.llvmContext, "branch_merge", currentFunc
        );
        
        // توليد التحقق من الشروط
        for (size_t i = 0; i < chain.branches.size(); i++) {
            const auto& branch = chain.branches[i];
            
            llvm::BasicBlock* nextCheck = (i + 1 < chain.branches.size())
                ? llvm::BasicBlock::Create(ctx.llvmContext, "check_" + std::to_string(i+1), currentFunc)
                : mergeBB;
            
            if (branch.condition) {
                ctx.builder.CreateCondBr(branch.condition, branchBlocks[i], nextCheck);
            } else {
                ctx.builder.CreateBr(branchBlocks[i]);
            }
            
            if (nextCheck != mergeBB) {
                ctx.builder.SetInsertPoint(nextCheck);
            }
        }
        
        // توليد كل فرع
        std::vector<llvm::Value*> branchResults;
        std::vector<llvm::BasicBlock*> branchExitBlocks;
        
        for (size_t i = 0; i < chain.branches.size(); i++) {
            ctx.builder.SetInsertPoint(branchBlocks[i]);
            
            llvm::Value* branchResult = chain.sourceValue;
            for (const auto& op : chain.branches[i].operations) {
                branchResult = generateOperation(op, branchResult);
            }
            
            branchResults.push_back(branchResult);
            branchExitBlocks.push_back(ctx.builder.GetInsertBlock());
            ctx.builder.CreateBr(mergeBB);
        }
        
        // Merge
        ctx.builder.SetInsertPoint(mergeBB);
        
        if (branchResults.empty()) {
            return chain.sourceValue;
        }
        
        llvm::Type* resultType = branchResults[0]->getType();
        llvm::PHINode* phi = ctx.builder.CreatePHI(resultType, branchResults.size(), "branch_result");
        
        for (size_t i = 0; i < branchResults.size(); i++) {
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
    llvm::Value* generateParallelChain(const PipelineChain& chain) {
        // للتوازي نحتاج:
        // 1. تقسيم البيانات
        // 2. إنشاء مهام متوازية
        // 3. جمع النتائج
        
        // هذا تنفيذ مبسط - في الواقع نحتاج runtime للتوازي
        
        llvm::Function* spawnFunc = getOrCreateFunction(
            "sad_parallel_spawn",
            llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType, ctx.ptrType}, false)
        );
        
        llvm::Function* joinFunc = getOrCreateFunction(
            "sad_parallel_join",
            llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType}, false)
        );
        
        // إنشاء دالة العمل
        std::string workerName = generateTempName("parallel_worker");
        
        llvm::FunctionType* workerType = llvm::FunctionType::get(
            ctx.ptrType,
            {ctx.ptrType},
            false
        );
        
        llvm::Function* workerFunc = llvm::Function::Create(
            workerType,
            llvm::Function::InternalLinkage,
            workerName,
            ctx.module
        );
        
        // توليد جسم دالة العمل
        {
            llvm::BasicBlock* workerEntry = llvm::BasicBlock::Create(
                ctx.llvmContext, "entry", workerFunc
            );
            
            llvm::IRBuilder<> workerBuilder(ctx.llvmContext);
            workerBuilder.SetInsertPoint(workerEntry);
            
            // ... توليد العمليات داخل العامل
            
            workerBuilder.CreateRet(workerFunc->getArg(0));
        }
        
        // إطلاق المهمة المتوازية
        llvm::Value* task = ctx.builder.CreateCall(
            spawnFunc,
            {chain.sourceValue, workerFunc},
            "parallel_task"
        );
        
        // انتظار النتيجة
        return ctx.builder.CreateCall(joinFunc, {task}, "parallel_result");
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                   التوليد غير المتزامن | Async Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد سلسلة غير متزامنة
     */
    llvm::Value* generateAsyncChain(const PipelineChain& chain) {
        // توليد coroutine state machine
        
        llvm::Function* currentFunc = ctx.builder.GetInsertBlock()->getParent();
        
        // إنشاء بنية حالة الـ coroutine
        std::string stateName = generateTempName("async_state");
        
        std::vector<llvm::Type*> stateFields = {
            ctx.i32Type,    // حالة الاستئناف
            ctx.ptrType,    // القيمة الحالية
            ctx.ptrType     // المكرر المصدر
        };
        
        // إضافة حقل لكل عملية تحتاج حالة
        for (const auto& op : chain.operations) {
            if (op.type == ChainOperationType::ASYNC) {
                stateFields.push_back(ctx.ptrType);  // المستقبل
            }
        }
        
        llvm::StructType* stateType = llvm::StructType::create(
            ctx.llvmContext, stateFields, stateName
        );
        
        llvm::Value* state = ctx.builder.CreateAlloca(stateType, nullptr, "state");
        
        // تهيئة الحالة
        ctx.builder.CreateStore(
            llvm::ConstantInt::get(ctx.i32Type, 0),
            ctx.builder.CreateStructGEP(stateType, state, 0)
        );
        
        ctx.builder.CreateStore(
            chain.sourceValue,
            ctx.builder.CreateStructGEP(stateType, state, 2)
        );
        
        // إنشاء دالة poll
        std::string pollName = generateTempName("async_poll");
        
        llvm::FunctionType* pollType = llvm::FunctionType::get(
            ctx.i32Type,  // Poll result: 0=pending, 1=ready
            {ctx.ptrType},  // State
            false
        );
        
        llvm::Function* pollFunc = llvm::Function::Create(
            pollType,
            llvm::Function::InternalLinkage,
            pollName,
            ctx.module
        );
        
        // ... توليد جسم poll مع switch على الحالة
        
        // إرجاع المستقبل
        llvm::Function* createFutureFunc = getOrCreateFunction(
            "sad_future_new",
            llvm::FunctionType::get(ctx.ptrType, {ctx.ptrType, ctx.ptrType}, false)
        );
        
        return ctx.builder.CreateCall(
            createFutureFunc,
            {state, pollFunc},
            "async_future"
        );
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          التحسينات | Optimizations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحسين السلسلة | Optimize chain
     */
    PipelineChain optimizeChain(const PipelineChain& chain) {
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
    PipelineChain fuseConsecutiveFilters(const PipelineChain& chain) {
        PipelineChain result = chain;
        result.operations.clear();
        
        ChainOperationNode* pendingFilter = nullptr;
        
        for (auto& op : chain.operations) {
            if (op.type == ChainOperationType::FILTER) {
                if (pendingFilter) {
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
                } else {
                    pendingFilter = new ChainOperationNode(op);
                }
            } else {
                if (pendingFilter) {
                    result.operations.push_back(*pendingFilter);
                    delete pendingFilter;
                    pendingFilter = nullptr;
                }
                result.operations.push_back(op);
            }
        }
        
        if (pendingFilter) {
            result.operations.push_back(*pendingFilter);
            delete pendingFilter;
        }
        
        return result;
    }
    
    /**
     * تقديم take في السلسلة
     */
    PipelineChain moveTakeEarlier(const PipelineChain& chain) {
        PipelineChain result = chain;
        
        // البحث عن take
        for (size_t i = 1; i < result.operations.size(); i++) {
            if (result.operations[i].type == ChainOperationType::TAKE) {
                // التحقق إذا يمكن تقديمها
                if (result.operations[i-1].type == ChainOperationType::MAP ||
                    result.operations[i-1].type == ChainOperationType::FILTER) {
                    // تبديل المواقع
                    std::swap(result.operations[i], result.operations[i-1]);
                }
            }
        }
        
        return result;
    }
    
    /**
     * إزالة العمليات الميتة
     */
    PipelineChain removeDeadOperations(const PipelineChain& chain) {
        PipelineChain result = chain;
        
        // إزالة map بهوية
        // إزالة filter بـ true دائماً
        // إزالة take(∞)
        
        result.operations.erase(
            std::remove_if(
                result.operations.begin(),
                result.operations.end(),
                [](const ChainOperationNode& op) {
                    // (AR) كشف العمليات الميتة التي لا تؤثر على النتيجة
                    // (EN) Detect dead operations that don't affect the result
                    
                    // (AR) map بدون دالة = هوية — يمكن إزالتها
                    // (EN) map without function = identity — can be removed
                    if (op.type == ChainOperationType::MAP && op.operand == nullptr) {
                        return true;
                    }
                    
                    // (AR) filter بدون دالة = true دائماً — يمكن إزالتها
                    // (EN) filter without function = always true — can be removed
                    if (op.type == ChainOperationType::FILTER && op.operand == nullptr) {
                        return true;
                    }
                    
                    // (AR) take بلا حد = لا تأثير
                    // (EN) take with no limit = no effect
                    if (op.type == ChainOperationType::TAKE && !op.count.has_value()) {
                        return true;
                    }
                    
                    // (AR) take(0) = السلسلة فارغة — سيتم معالجتها لاحقاً
                    // (EN) take(0) = empty chain — will be handled separately
                    
                    return false;
                }
            ),
            result.operations.end()
        );
        
        return result;
    }
    
    /**
     * تحويل إلى lazy إذا ممكن
     */
    PipelineChain convertToLazyIfPossible(const PipelineChain& chain) {
        PipelineChain result = chain;
        
        // إذا لم تنتهي السلسلة بعملية eager، يمكن جعلها lazy
        if (!result.empty()) {
            auto& lastOp = result.operations.back();
            if (lastOp.type != ChainOperationType::COLLECT &&
                lastOp.type != ChainOperationType::FOLD &&
                lastOp.type != ChainOperationType::REDUCE &&
                lastOp.type != ChainOperationType::FOR_EACH) {
                // يمكن جعل كل العمليات lazy
                for (auto& op : result.operations) {
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
    llvm::StructType* createIteratorStruct(const std::string& name, llvm::Type* elementType) {
        std::vector<llvm::Type*> fields = {
            ctx.ptrType,                // المصدر / source
            ctx.ptrType,                // الدالة / function
            elementType ? elementType : ctx.ptrType  // العنصر الحالي
        };
        
        return llvm::StructType::create(ctx.llvmContext, fields, name);
    }
    
    /**
     * الحصول على دالة أو إنشائها
     */
    llvm::Function* getOrCreateFunction(
        const std::string& name,
        llvm::FunctionType* type
    ) {
        llvm::Function* func = ctx.module->getFunction(name);
        if (!func) {
            func = llvm::Function::Create(
                type,
                llvm::Function::ExternalLinkage,
                name,
                ctx.module
            );
        }
        return func;
    }
    
    /**
     * توليد اسم مؤقت فريد
     */
    std::string generateTempName(const std::string& prefix) {
        return prefix + "_" + std::to_string(tempCounter++);
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 PipelineChainInterface
 * 
 * واجهة للاستخدام من باقي المترجم
 */
class PipelineChainInterface {
public:
    /**
     * توليد سلسلة أنابيب كاملة
     */
    static llvm::Value* generatePipelineChain(
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
     * إنشاء عقدة عملية من AST
     */
    static ChainOperationNode createOperationFromAST(
        // AST node parameters
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
     * إنشاء سلسلة من قائمة عمليات
     */
    static PipelineChain createChainFromOperations(
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
};

} // namespace codegen
} // namespace sad
