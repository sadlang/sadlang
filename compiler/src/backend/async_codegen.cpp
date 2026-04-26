/**
 * @file async_codegen.cpp
 * @brief (AR) توليد الكود لآلات الحالة غير المتزامنة
 *        (EN) Code Generation for Async State Machines
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ██████╗ ██████╗ ██████╗ ███████╗ ██████╗ ███████╗███╗   ██╗              ║
 * ║    ██╔════╝██╔═══██╗██╔══██╗██╔════╝██╔════╝ ██╔════╝████╗  ██║              ║
 * ║    ██║     ██║   ██║██║  ██║█████╗  ██║  ███╗█████╗  ██╔██╗ ██║              ║
 * ║    ██║     ██║   ██║██║  ██║██╔══╝  ██║   ██║██╔══╝  ██║╚██╗██║              ║
 * ║    ╚██████╗╚██████╔╝██████╔╝███████╗╚██████╔╝███████╗██║ ╚████║              ║
 * ║     ╚═════╝ ╚═════╝ ╚═════╝ ╚══════╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝              ║
 * ║                                                                               ║
 * ║              توليد كود LLVM IR لآلات الحالة                                  ║
 * ║              LLVM IR Code Generation for State Machines                       ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يولد كود LLVM IR لآلات الحالة المُحولة من async:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     تحويل آلة الحالة إلى LLVM IR                        │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    آلة الحالة ← تخطيط الذاكرة + دالة poll + تبديل الحالات              │
 *      │                                                                         │
 *      │    // الهيكل في LLVM IR                                                 │
 *      │    %AsyncMachine = type {                                               │
 *      │        i8,           ; state                                            │
 *      │        i64,          ; captured_var_1                                   │
 *      │        %Future*,     ; awaited_future_1                                 │
 *      │        ...                                                              │
 *      │    }                                                                    │
 *      │                                                                         │
 *      │    ; دالة poll                                                          │
 *      │    define %Poll @AsyncMachine_poll(%AsyncMachine* %self, %Context* %cx) {│
 *      │        %state = load i8, i8* getelementptr(%self, 0, 0)                 │
 *      │        switch i8 %state, label %unreachable [                           │
 *      │            i8 0, label %state_start                                     │
 *      │            i8 1, label %state_await_1                                   │
 *      │            ...                                                          │
 *      │        ]                                                                │
 *      │    state_start:                                                         │
 *      │        ...                                                              │
 *      │    }                                                                    │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "async_codegen.hpp"
#include "ir_generator.hpp"
// ملاحظة: الأنواع المطلوبة (middle::AsyncStateMachine، types::Type) معرّفة في async_codegen.hpp
// Note: Required types (middle::AsyncStateMachine, types::Type) are defined in async_codegen.hpp

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace sad::backend {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولد كود async
//                    (EN) Async Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) مولد كود آلة الحالة غير المتزامنة
 */
class AsyncCodeGenerator {
private:
    llvm::Module& module_;
    llvm::LLVMContext& context_;
    llvm::IRBuilder<>& builder_;
    
    // (AR) خرائط التحويل
    std::unordered_map<std::string, llvm::StructType*> state_machine_types_;
    std::unordered_map<std::string, llvm::Function*> poll_functions_;
    
    // (AR) أنواع الأساسية
    llvm::StructType* poll_type_;       // Poll<T> = { i8 tag, T value }
    llvm::StructType* context_type_;    // Context
    llvm::StructType* waker_type_;      // Waker
    
public:
    AsyncCodeGenerator(
        llvm::Module& module,
        llvm::LLVMContext& context,
        llvm::IRBuilder<>& builder
    )
        : module_(module)
        , context_(context)
        , builder_(builder)
        , poll_type_(nullptr)
        , context_type_(nullptr)
        , waker_type_(nullptr)
    {
        initializeBaseTypes();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تهيئة الأنواع الأساسية
    //                    (EN) Initialize Base Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تهيئة الأنواع الأساسية
     */
    void initializeBaseTypes() {
        // (AR) نوع Waker
        // struct Waker { void* data; void (*wake)(void*); void (*wake_by_ref)(void*); void (*drop)(void*); }
        waker_type_ = llvm::StructType::create(context_, "Waker");
        std::vector<llvm::Type*> waker_fields = {
            llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_)),  // data
            llvm::PointerType::getUnqual(llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_))},
                false
            )),  // wake
            llvm::PointerType::getUnqual(llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_))},
                false
            )),  // wake_by_ref
            llvm::PointerType::getUnqual(llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_))},
                false
            ))   // drop
        };
        waker_type_->setBody(waker_fields);
        
        // (AR) نوع Context
        // struct Context { Waker* waker; }
        context_type_ = llvm::StructType::create(context_, "Context");
        context_type_->setBody({llvm::PointerType::getUnqual(waker_type_)});
    }
    
    /**
     * (AR) إنشاء نوع Poll<T>
     */
    llvm::StructType* getPollType(llvm::Type* inner_type) {
        std::string name = "Poll_" + getTypeName(inner_type);
        
        auto it = state_machine_types_.find(name);
        if (it != state_machine_types_.end()) {
            return it->second;
        }
        
        // struct Poll<T> { i8 tag; T value; }
        // tag: 0 = Pending, 1 = Ready
        auto poll = llvm::StructType::create(context_, name);
        poll->setBody({
            llvm::Type::getInt8Ty(context_),
            inner_type
        });
        
        state_machine_types_[name] = poll;
        return poll;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) توليد نوع آلة الحالة
    //                    (EN) Generate State Machine Type
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) توليد نوع هيكل آلة الحالة
     */
    llvm::StructType* generateStateMachineType(
        const middle::AsyncStateMachine& machine
    ) {
        auto it = state_machine_types_.find(machine.name);
        if (it != state_machine_types_.end()) {
            return it->second;
        }
        
        auto struct_type = llvm::StructType::create(context_, machine.name);
        
        std::vector<llvm::Type*> fields;
        
        // (AR) حقل الحالة (i8)
        fields.push_back(llvm::Type::getInt8Ty(context_));
        
        // (AR) حقول المتغيرات المُلتقطة
        for (const auto& var : machine.captured_vars) {
            llvm::Type* var_type = getLLVMType(var.type.get());
            // (AR) تغليف بـ Option (i8 tag + value)
            auto opt_type = llvm::StructType::create(
                context_,
                machine.name + "_opt_" + var.name
            );
            opt_type->setBody({
                llvm::Type::getInt8Ty(context_),
                var_type
            });
            fields.push_back(opt_type);
        }
        
        // (AR) حقول المستقبلات المُنتظرة (مؤشرات)
        for (const auto& await : machine.await_points) {
            // (AR) مؤشر للمستقبل
            fields.push_back(llvm::PointerType::getUnqual(
                llvm::Type::getInt8Ty(context_)
            ));
        }
        
        struct_type->setBody(fields);
        state_machine_types_[machine.name] = struct_type;
        
        return struct_type;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) توليد دالة poll
    //                    (EN) Generate Poll Function
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) توليد دالة poll لآلة الحالة
     */
    llvm::Function* generatePollFunction(
        const middle::AsyncStateMachine& machine
    ) {
        auto* machine_type = generateStateMachineType(machine);
        auto* output_type = getLLVMType(machine.output_type.get());
        auto* poll_result_type = getPollType(output_type);
        
        // (AR) توقيع الدالة: Poll<T> poll(Machine* self, Context* cx)
        auto* func_type = llvm::FunctionType::get(
            poll_result_type,
            {
                llvm::PointerType::getUnqual(machine_type),
                llvm::PointerType::getUnqual(context_type_)
            },
            false
        );
        
        auto* func = llvm::Function::Create(
            func_type,
            llvm::Function::ExternalLinkage,
            machine.name + "_poll",
            module_
        );
        
        // (AR) تسمية المعاملات
        auto args = func->arg_begin();
        llvm::Value* self_ptr = args++;
        self_ptr->setName("self");
        llvm::Value* cx_ptr = args;
        cx_ptr->setName("cx");
        
        // (AR) إنشاء الكتل الأساسية
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func
        );
        llvm::BasicBlock* unreachable_block = llvm::BasicBlock::Create(
            context_, "unreachable", func
        );
        
        // (AR) كتلة لكل حالة
        std::vector<llvm::BasicBlock*> state_blocks;
        for (const auto& state : machine.states) {
            auto* block = llvm::BasicBlock::Create(
                context_,
                "state_" + state.name,
                func
            );
            state_blocks.push_back(block);
        }
        
        // (AR) كتلة الدخول: تحميل الحالة وتبديل
        builder_.SetInsertPoint(entry);
        auto* state_ptr = builder_.CreateStructGEP(
            machine_type, self_ptr, 0, "state_ptr"
        );
        auto* current_state = builder_.CreateLoad(
            llvm::Type::getInt8Ty(context_),
            state_ptr,
            "current_state"
        );
        
        // (AR) إنشاء switch
        auto* switch_inst = builder_.CreateSwitch(
            current_state,
            unreachable_block,
            static_cast<unsigned>(machine.states.size())
        );
        
        for (size_t i = 0; i < machine.states.size(); ++i) {
            switch_inst->addCase(
                llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), i),
                state_blocks[i]
            );
        }
        
        // (AR) كتلة unreachable
        builder_.SetInsertPoint(unreachable_block);
        builder_.CreateUnreachable();
        
        // (AR) توليد كود كل حالة
        for (size_t i = 0; i < machine.states.size(); ++i) {
            generateStateCode(
                machine,
                machine.states[i],
                state_blocks,
                self_ptr,
                cx_ptr,
                poll_result_type,
                output_type
            );
        }
        
        poll_functions_[machine.name] = func;
        return func;
    }
    
    /**
     * (AR) توليد كود حالة معينة
     */
    void generateStateCode(
        const middle::AsyncStateMachine& machine,
        const middle::AsyncState& state,
        const std::vector<llvm::BasicBlock*>& state_blocks,
        llvm::Value* self_ptr,
        llvm::Value* cx_ptr,
        llvm::StructType* poll_result_type,
        llvm::Type* output_type
    ) {
        builder_.SetInsertPoint(state_blocks[state.id]);
        
        switch (state.kind) {
            case middle::AsyncStateKind::START:
            case middle::AsyncStateKind::BRANCH:
            case middle::AsyncStateKind::LOOP:
                generateRegularStateCode(machine, state, state_blocks, self_ptr);
                break;
                
            case middle::AsyncStateKind::AWAIT_POINT:
                generateAwaitStateCode(
                    machine, state, state_blocks,
                    self_ptr, cx_ptr, poll_result_type, output_type
                );
                break;
                
            case middle::AsyncStateKind::END:
                generateEndStateCode(poll_result_type, output_type);
                break;
                
            default:
                builder_.CreateUnreachable();
                break;
        }
    }
    
    /**
     * (AR) توليد كود حالة عادية
     */
    void generateRegularStateCode(
        const middle::AsyncStateMachine& machine,
        const middle::AsyncState& state,
        const std::vector<llvm::BasicBlock*>& state_blocks,
        llvm::Value* self_ptr
    ) {
        // (AR) تنفيذ الكود في هذه الحالة
        // (يحتاج ربط مع مولد IR الرئيسي)
        
        // (AR) تحديث الحالة
        auto* machine_type = state_machine_types_[machine.name];
        auto* state_ptr = builder_.CreateStructGEP(
            machine_type, self_ptr, 0, "state_ptr"
        );
        
        if (state.next_state >= 0 && 
            state.next_state < static_cast<int>(state_blocks.size())) {
            builder_.CreateStore(
                llvm::ConstantInt::get(
                    llvm::Type::getInt8Ty(context_),
                    state.next_state
                ),
                state_ptr
            );
            builder_.CreateBr(state_blocks[state.next_state]);
        } else {
            builder_.CreateUnreachable();
        }
    }
    
    /**
     * (AR) توليد كود حالة انتظار
     */
    void generateAwaitStateCode(
        const middle::AsyncStateMachine& machine,
        const middle::AsyncState& state,
        const std::vector<llvm::BasicBlock*>& state_blocks,
        llvm::Value* self_ptr,
        llvm::Value* cx_ptr,
        llvm::StructType* poll_result_type,
        llvm::Type* output_type
    ) {
        // (AR) البحث عن نقطة الانتظار
        const middle::AwaitPoint* await_point = nullptr;
        for (const auto& ap : machine.await_points) {
            if (ap.state_id == state.id) {
                await_point = &ap;
                break;
            }
        }
        
        if (!await_point) {
            builder_.CreateUnreachable();
            return;
        }
        
        // (AR) تحميل مؤشر المستقبل
        // (يحتاج حساب الفهرس الصحيح)
        
        // (AR) استدعاء poll على المستقبل
        // auto* inner_poll_result = builder_.CreateCall(future_poll, {future_ptr, cx_ptr});
        
        // (AR) فحص النتيجة
        auto* ready_block = llvm::BasicBlock::Create(
            context_, "await_ready", builder_.GetInsertBlock()->getParent()
        );
        auto* pending_block = llvm::BasicBlock::Create(
            context_, "await_pending", builder_.GetInsertBlock()->getParent()
        );
        
        // (AR) مؤقتاً: نعتبر دائماً Pending
        builder_.CreateBr(pending_block);
        
        // (AR) كتلة Ready
        builder_.SetInsertPoint(ready_block);
        // (AR) استخراج القيمة وحفظها
        auto* machine_type = state_machine_types_[machine.name];
        auto* state_ptr = builder_.CreateStructGEP(
            machine_type, self_ptr, 0, "state_ptr"
        );
        builder_.CreateStore(
            llvm::ConstantInt::get(
                llvm::Type::getInt8Ty(context_),
                state.next_state
            ),
            state_ptr
        );
        builder_.CreateBr(state_blocks[state.next_state]);
        
        // (AR) كتلة Pending
        builder_.SetInsertPoint(pending_block);
        // (AR) إرجاع Pending
        auto* pending_result = llvm::UndefValue::get(poll_result_type);
        auto* pending_with_tag = builder_.CreateInsertValue(
            pending_result,
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 0), // Pending
            {0}
        );
        builder_.CreateRet(pending_with_tag);
    }
    
    /**
     * (AR) توليد كود حالة النهاية
     */
    void generateEndStateCode(
        llvm::StructType* poll_result_type,
        llvm::Type* output_type
    ) {
        // (AR) إرجاع Ready(value)
        auto* ready_result = llvm::UndefValue::get(poll_result_type);
        auto* ready_with_tag = builder_.CreateInsertValue(
            ready_result,
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 1), // Ready
            {0}
        );
        // (AR) إضافة القيمة النهائية
        // ready_with_tag = builder_.CreateInsertValue(ready_with_tag, final_value, {1});
        builder_.CreateRet(ready_with_tag);
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) توليد دوال المساعدة
    //                    (EN) Generate Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) توليد دالة إنشاء آلة الحالة
     */
    llvm::Function* generateConstructor(
        const middle::AsyncStateMachine& machine
    ) {
        auto* machine_type = generateStateMachineType(machine);
        
        // (AR) توقيع: Machine* create_machine(params...)
        std::vector<llvm::Type*> param_types;
        for (const auto& var : machine.captured_vars) {
            if (var.defined_in_state == 0) { // (AR) المعاملات
                param_types.push_back(getLLVMType(var.type.get()));
            }
        }
        
        auto* func_type = llvm::FunctionType::get(
            llvm::PointerType::getUnqual(machine_type),
            param_types,
            false
        );
        
        auto* func = llvm::Function::Create(
            func_type,
            llvm::Function::ExternalLinkage,
            machine.name + "_create",
            module_
        );
        
        auto* entry = llvm::BasicBlock::Create(context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // (AR) تخصيص الذاكرة
        auto* alloc = builder_.CreateAlloca(machine_type, nullptr, "machine");
        
        // (AR) تهيئة الحالة = 0 (بداية)
        auto* state_ptr = builder_.CreateStructGEP(machine_type, alloc, 0);
        builder_.CreateStore(
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 0),
            state_ptr
        );
        
        // (AR) تهيئة المعاملات
        size_t param_idx = 0;
        size_t field_idx = 1;
        for (auto& arg : func->args()) {
            for (const auto& var : machine.captured_vars) {
                if (var.defined_in_state == 0) {
                    auto* field_ptr = builder_.CreateStructGEP(
                        machine_type, alloc, field_idx
                    );
                    // (AR) تعيين Some(value)
                    // (يحتاج تنفيذ كامل)
                    field_idx++;
                    param_idx++;
                    break;
                }
            }
        }
        
        builder_.CreateRet(alloc);
        return func;
    }
    
    /**
     * (AR) توليد دالة إيقاظ المستقبل
     */
    llvm::Function* generateWakeFunction() {
        if (module_.getFunction("sad_waker_wake")) {
            return module_.getFunction("sad_waker_wake");
        }
        
        auto* func_type = llvm::FunctionType::get(
            llvm::Type::getVoidTy(context_),
            {llvm::PointerType::getUnqual(waker_type_)},
            false
        );
        
        auto* func = llvm::Function::Create(
            func_type,
            llvm::Function::ExternalLinkage,
            "sad_waker_wake",
            module_
        );
        
        auto* entry = llvm::BasicBlock::Create(context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        auto* waker_ptr = func->arg_begin();
        
        // (AR) تحميل دالة wake
        auto* wake_fn_ptr = builder_.CreateStructGEP(
            waker_type_, waker_ptr, 1, "wake_fn_ptr"
        );
        auto* wake_fn = builder_.CreateLoad(
            llvm::PointerType::getUnqual(llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_))},
                false
            )),
            wake_fn_ptr,
            "wake_fn"
        );
        
        // (AR) تحميل data
        auto* data_ptr = builder_.CreateStructGEP(
            waker_type_, waker_ptr, 0, "data_ptr"
        );
        auto* data = builder_.CreateLoad(
            llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_)),
            data_ptr,
            "data"
        );
        
        // (AR) استدعاء wake(data)
        builder_.CreateCall(
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_))},
                false
            ),
            wake_fn,
            {data}
        );
        
        builder_.CreateRetVoid();
        return func;
    }
    
private:
    /**
     * (AR) تحويل نوع sad إلى نوع LLVM
     */
    llvm::Type* getLLVMType(const types::Type* type) {
        if (!type) {
            return llvm::Type::getVoidTy(context_);
        }
        
        // (AR) تنفيذ مبسط
        switch (type->kind) {
            case types::SadTypeKind::Int8:
                return llvm::Type::getInt8Ty(context_);
            case types::SadTypeKind::Int16:
                return llvm::Type::getInt16Ty(context_);
            case types::SadTypeKind::Int32:
                return llvm::Type::getInt32Ty(context_);
            case types::SadTypeKind::Int64:
                return llvm::Type::getInt64Ty(context_);
            case types::SadTypeKind::Float32:
                return llvm::Type::getFloatTy(context_);
            case types::SadTypeKind::Float64:
                return llvm::Type::getDoubleTy(context_);
            case types::SadTypeKind::Bool:
                return llvm::Type::getInt1Ty(context_);
            case types::SadTypeKind::Unit:
                return llvm::Type::getVoidTy(context_);
            default:
                return llvm::PointerType::getUnqual(llvm::Type::getInt8Ty(context_));
        }
    }
    
    /**
     * (AR) الحصول على اسم النوع
     */
    std::string getTypeName(llvm::Type* type) {
        if (auto* struct_type = llvm::dyn_cast<llvm::StructType>(type)) {
            return struct_type->getName().str();
        }
        if (type->isIntegerTy()) {
            return "i" + std::to_string(type->getIntegerBitWidth());
        }
        if (type->isFloatTy()) return "f32";
        if (type->isDoubleTy()) return "f64";
        if (type->isVoidTy()) return "void";
        if (type->isPointerTy()) return "ptr";
        return "unknown";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) توليد كود لآلة حالة async
 */
void generateAsyncStateMachineCode(
    const middle::AsyncStateMachine& machine,
    llvm::Module& module,
    llvm::LLVMContext& context,
    llvm::IRBuilderBase& builder
) {
    // (AR) تحويل IRBuilderBase إلى IRBuilder<> للاستخدام الداخلي
    // (EN) Cast IRBuilderBase to IRBuilder<> for internal use
    auto& typedBuilder = static_cast<llvm::IRBuilder<>&>(builder);
    AsyncCodeGenerator generator(module, context, typedBuilder);
    
    // (AR) توليد النوع
    generator.generateStateMachineType(machine);
    
    // (AR) توليد دالة poll
    generator.generatePollFunction(machine);
    
    // (AR) توليد المنشئ
    generator.generateConstructor(machine);
    
    // (AR) توليد دوال المساعدة
    generator.generateWakeFunction();
}

} // namespace sad::backend
