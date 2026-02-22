/**
 * @file wasm_codegen.cpp
 * @brief WASM code generation for Sad compiler
 * 
 * توليد كود WebAssembly من AST لغة ص
 * Generates WebAssembly code from Sad AST
 */

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace sad {
namespace backend {

/**
 * مولد كود WebAssembly
 * WebAssembly Code Generator
 */
class WasmCodeGen {
public:
    /**
     * نوع دالة الاستيراد/التصدير
     */
    struct WasmFunction {
        std::string name;           // اسم الدالة
        std::string arabicName;     // الاسم العربي
        llvm::FunctionType* type;   // نوع الدالة
        bool isExport = false;      // هل يتم تصديرها
        bool isImport = false;      // هل هي مستوردة
        std::string importModule;   // وحدة الاستيراد
    };
    
    /**
     * تعريف ذاكرة WASM
     */
    struct WasmMemory {
        size_t initialPages = 16;   // الصفحات الأولية (64KB لكل صفحة)
        size_t maxPages = 256;      // الحد الأقصى للصفحات
        bool isShared = false;      // ذاكرة مشتركة
    };
    
private:
    llvm::LLVMContext& context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    
    // الذاكرة والجدول
    WasmMemory memory_;
    llvm::GlobalVariable* memoryPtr_ = nullptr;
    llvm::GlobalVariable* heapBase_ = nullptr;
    llvm::GlobalVariable* stackPointer_ = nullptr;
    
    // سلاسل النصوص
    std::unordered_map<std::string, llvm::GlobalVariable*> stringPool_;
    size_t stringOffset_ = 0;
    
    // الدوال المستوردة والمصدرة
    std::vector<WasmFunction> imports_;
    std::vector<WasmFunction> exports_;
    
public:
    WasmCodeGen(llvm::LLVMContext& ctx, const std::string& moduleName)
        : context_(ctx) {
        module_ = std::make_unique<llvm::Module>(moduleName, context_);
        builder_ = std::make_unique<llvm::IRBuilder<>>(context_);
        
        // تعيين Triple لـ WebAssembly
        module_->setTargetTriple("wasm32-unknown-unknown");
        
        // تهيئة الذاكرة
        initializeMemory();
        
        // تسجيل دوال الاستيراد القياسية
        registerStandardImports();
    }
    
    /**
     * تهيئة متغيرات الذاكرة
     */
    void initializeMemory() {
        // مؤشر قاعدة الكومة
        auto i32Type = llvm::Type::getInt32Ty(context_);
        
        heapBase_ = new llvm::GlobalVariable(
            *module_, i32Type, false,
            llvm::GlobalValue::InternalLinkage,
            llvm::ConstantInt::get(i32Type, memory_.initialPages * 65536 / 2),
            "__heap_base"
        );
        
        // مؤشر المكدس
        stackPointer_ = new llvm::GlobalVariable(
            *module_, i32Type, false,
            llvm::GlobalValue::InternalLinkage,
            llvm::ConstantInt::get(i32Type, memory_.initialPages * 65536),
            "__stack_pointer"
        );
    }
    
    /**
     * تسجيل دوال الاستيراد القياسية
     */
    void registerStandardImports() {
        // دوال الطباعة
        registerImport("env", "اطبع", "__sad_print", 
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::Type::getInt32Ty(context_)},  // مؤشر النص
                false
            ));
        
        registerImport("env", "اطبع_رقم", "__sad_print_int",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::Type::getInt32Ty(context_)},
                false
            ));
        
        registerImport("env", "اطبع_عشري", "__sad_print_float",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::Type::getDoubleTy(context_)},
                false
            ));
        
        // دوال DOM
        registerImport("env", "__sad_dom_query", "__sad_dom_query",
            llvm::FunctionType::get(
                llvm::Type::getInt32Ty(context_),
                {llvm::Type::getInt32Ty(context_)},  // selector ptr
                false
            ));
        
        registerImport("env", "__sad_dom_create", "__sad_dom_create",
            llvm::FunctionType::get(
                llvm::Type::getInt32Ty(context_),
                {llvm::Type::getInt32Ty(context_)},  // tag ptr
                false
            ));
        
        registerImport("env", "__sad_dom_set_text", "__sad_dom_set_text",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::Type::getInt32Ty(context_),   // element handle
                 llvm::Type::getInt32Ty(context_)},  // text ptr
                false
            ));
        
        registerImport("env", "__sad_dom_append", "__sad_dom_append",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::Type::getInt32Ty(context_),   // parent handle
                 llvm::Type::getInt32Ty(context_)},  // child handle
                false
            ));
        
        // دوال الشبكة
        registerImport("env", "__sad_fetch_start", "__sad_fetch_start",
            llvm::FunctionType::get(
                llvm::Type::getInt32Ty(context_),   // request handle
                {llvm::Type::getInt32Ty(context_)}, // url ptr
                false
            ));
        
        registerImport("env", "__sad_fetch_callback", "__sad_fetch_callback",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::Type::getInt32Ty(context_),   // request handle
                 llvm::Type::getInt32Ty(context_)},  // callback ptr
                false
            ));
        
        // دوال الذاكرة
        registerImport("env", "__sad_malloc", "__sad_malloc",
            llvm::FunctionType::get(
                llvm::Type::getInt32Ty(context_),
                {llvm::Type::getInt32Ty(context_)},  // size
                false
            ));
        
        registerImport("env", "__sad_free", "__sad_free",
            llvm::FunctionType::get(
                llvm::Type::getVoidTy(context_),
                {llvm::Type::getInt32Ty(context_)},  // ptr
                false
            ));
    }
    
    /**
     * تسجيل دالة استيراد
     */
    void registerImport(const std::string& module, 
                        const std::string& arabicName,
                        const std::string& name,
                        llvm::FunctionType* type) {
        // إنشاء الدالة في الوحدة
        auto func = llvm::Function::Create(
            type,
            llvm::Function::ExternalLinkage,
            name,
            module_.get()
        );
        
        // تعيين سمات WASM
        func->addFnAttr("wasm-import-module", module);
        func->addFnAttr("wasm-import-name", arabicName.empty() ? name : arabicName);
        
        imports_.push_back({name, arabicName, type, false, true, module});
    }
    
    /**
     * تسجيل دالة للتصدير
     */
    void registerExport(llvm::Function* func, const std::string& exportName = "") {
        std::string name = exportName.empty() ? func->getName().str() : exportName;
        
        // تعيين سمة التصدير
        func->addFnAttr("wasm-export-name", name);
        func->setLinkage(llvm::Function::ExternalLinkage);
        
        exports_.push_back({func->getName().str(), name, func->getFunctionType(), true, false, ""});
    }
    
    /**
     * توليد دالة رئيسية
     */
    llvm::Function* generateMainFunction() {
        auto funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context_),
            {},
            false
        );
        
        auto func = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            "رئيسية",
            module_.get()
        );
        
        // تصدير الدالة الرئيسية
        registerExport(func, "main");
        registerExport(func, "رئيسية");
        
        return func;
    }
    
    /**
     * إضافة نص ثابت إلى تجمع السلاسل
     * @return عنوان النص في الذاكرة
     */
    llvm::Value* addStringConstant(const std::string& str) {
        // التحقق من وجود النص مسبقاً
        auto it = stringPool_.find(str);
        if (it != stringPool_.end()) {
            return builder_->CreatePtrToInt(it->second, 
                llvm::Type::getInt32Ty(context_));
        }
        
        // إنشاء ثابت نصي جديد
        auto strConstant = llvm::ConstantDataArray::getString(context_, str, true);
        
        auto globalStr = new llvm::GlobalVariable(
            *module_,
            strConstant->getType(),
            true,  // constant
            llvm::GlobalValue::PrivateLinkage,
            strConstant,
            ".str." + std::to_string(stringPool_.size())
        );
        
        // تعيين القسم للبيانات
        globalStr->setSection(".rodata");
        globalStr->setAlignment(llvm::Align(1));
        
        stringPool_[str] = globalStr;
        
        return builder_->CreatePtrToInt(globalStr, 
            llvm::Type::getInt32Ty(context_));
    }
    
    /**
     * توليد استدعاء دالة طباعة
     */
    void generatePrint(const std::string& text) {
        auto strPtr = addStringConstant(text);
        auto printFunc = module_->getFunction("__sad_print");
        if (printFunc) {
            builder_->CreateCall(printFunc, {strPtr});
        }
    }
    
    /**
     * توليد استدعاء طباعة رقم
     */
    void generatePrintInt(llvm::Value* value) {
        auto printFunc = module_->getFunction("__sad_print_int");
        if (printFunc) {
            builder_->CreateCall(printFunc, {value});
        }
    }
    
    /**
     * توليد عمليات DOM
     */
    llvm::Value* generateDOMQuery(const std::string& selector) {
        auto strPtr = addStringConstant(selector);
        auto func = module_->getFunction("__sad_dom_query");
        if (func) {
            return builder_->CreateCall(func, {strPtr});
        }
        return nullptr;
    }
    
    llvm::Value* generateDOMCreate(const std::string& tag) {
        auto strPtr = addStringConstant(tag);
        auto func = module_->getFunction("__sad_dom_create");
        if (func) {
            return builder_->CreateCall(func, {strPtr});
        }
        return nullptr;
    }
    
    void generateDOMSetText(llvm::Value* element, const std::string& text) {
        auto strPtr = addStringConstant(text);
        auto func = module_->getFunction("__sad_dom_set_text");
        if (func) {
            builder_->CreateCall(func, {element, strPtr});
        }
    }
    
    void generateDOMAppend(llvm::Value* parent, llvm::Value* child) {
        auto func = module_->getFunction("__sad_dom_append");
        if (func) {
            builder_->CreateCall(func, {parent, child});
        }
    }
    
    /**
     * توليد تخصيص ذاكرة
     */
    llvm::Value* generateMalloc(llvm::Value* size) {
        auto func = module_->getFunction("__sad_malloc");
        if (func) {
            return builder_->CreateCall(func, {size});
        }
        return nullptr;
    }
    
    /**
     * توليد تحرير ذاكرة
     */
    void generateFree(llvm::Value* ptr) {
        auto func = module_->getFunction("__sad_free");
        if (func) {
            builder_->CreateCall(func, {ptr});
        }
    }
    
    // وصول
    llvm::Module* getModule() { return module_.get(); }
    llvm::IRBuilder<>* getBuilder() { return builder_.get(); }
    llvm::LLVMContext& getContext() { return context_; }
    
    /**
     * إنشاء دالة مساعدة للتصدير
     */
    llvm::Function* createFunction(const std::string& name,
                                   llvm::Type* returnType,
                                   const std::vector<llvm::Type*>& paramTypes,
                                   bool isExported = false) {
        auto funcType = llvm::FunctionType::get(returnType, paramTypes, false);
        auto func = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            name,
            module_.get()
        );
        
        if (isExported) {
            registerExport(func);
        }
        
        return func;
    }
    
    /**
     * إنشاء كتلة أساسية
     */
    llvm::BasicBlock* createBlock(llvm::Function* func, const std::string& name = "entry") {
        auto block = llvm::BasicBlock::Create(context_, name, func);
        builder_->SetInsertPoint(block);
        return block;
    }
    
    /**
     * الحصول على نوع LLVM من نوع ص
     */
    llvm::Type* getLLVMType(const std::string& sadType) {
        if (sadType == "رقم" || sadType == "عدد" || sadType == "ع32") {
            return llvm::Type::getInt32Ty(context_);
        } else if (sadType == "ع64") {
            return llvm::Type::getInt64Ty(context_);
        } else if (sadType == "عش32" || sadType == "عشري") {
            return llvm::Type::getFloatTy(context_);
        } else if (sadType == "عش64") {
            return llvm::Type::getDoubleTy(context_);
        } else if (sadType == "منطقي") {
            return llvm::Type::getInt1Ty(context_);
        } else if (sadType == "نص") {
            return llvm::Type::getInt32Ty(context_);  // مؤشر
        } else if (sadType == "فراغ" || sadType == "لاشيء") {
            return llvm::Type::getVoidTy(context_);
        }
        
        // نوع افتراضي
        return llvm::Type::getInt32Ty(context_);
    }
    
    /**
     * تفريغ الوحدة إلى IR
     */
    void dumpIR(llvm::raw_ostream& out) {
        module_->print(out, nullptr);
    }
};

/**
 * منشئ WASM من AST
 */
class WasmModuleBuilder {
private:
    std::unique_ptr<WasmCodeGen> codegen_;
    
public:
    WasmModuleBuilder(llvm::LLVMContext& ctx, const std::string& name) {
        codegen_ = std::make_unique<WasmCodeGen>(ctx, name);
    }
    
    /**
     * بناء وحدة بسيطة للاختبار
     */
    void buildTestModule() {
        // إنشاء الدالة الرئيسية
        auto mainFunc = codegen_->generateMainFunction();
        codegen_->createBlock(mainFunc);
        
        // طباعة رسالة
        codegen_->generatePrint("مرحباً من WebAssembly! 🎉");
        
        // إرجاع 0
        codegen_->getBuilder()->CreateRet(
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(codegen_->getContext()), 0)
        );
    }
    
    WasmCodeGen* getCodeGen() { return codegen_.get(); }
};

} // namespace backend
} // namespace sad
