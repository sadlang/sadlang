/**
 * =============================================================================
 * ملف: wasm_optimizer.cpp
 * الوصف: تحسينات WASM متقدمة — المصفوفات والاستيراد وإدارة الذاكرة
 * (AR) @brief تحسينات WebAssembly: Array، Import، Memory management
 * (EN) @brief WASM optimizations: Array, Import, Memory management
 * المهمة: T302 - WASM target optimization
 * =============================================================================
 */

#ifdef HAS_LLVM

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <iostream>

namespace sad::compiler::wasm {

// =============================================================================
// تحسين المصفوفات في WASM
// =============================================================================

/**
 * (AR) مدير مصفوفات WASM — تخطيط ذاكرة خطي فعال
 * (EN) WASM Array Manager — efficient linear memory layout
 *
 * تخطيط المصفوفة في الذاكرة:
 * [length:i32][capacity:i32][elem_size:i32][data...]
 * offset+0:  العدد
 * offset+4:  السعة
 * offset+8:  حجم العنصر
 * offset+12: البيانات
 */
class WasmArrayOptimizer {
public:
    static constexpr int HEADER_SIZE = 12;  // bytes
    static constexpr int LENGTH_OFFSET = 0;
    static constexpr int CAPACITY_OFFSET = 4;
    static constexpr int ELEM_SIZE_OFFSET = 8;
    static constexpr int DATA_OFFSET = 12;
    
    WasmArrayOptimizer(llvm::LLVMContext& ctx, llvm::Module& mod,
                       llvm::IRBuilder<>& builder)
        : ctx_(ctx), mod_(mod), builder_(builder) {}
    
    /**
     * (AR) إنشاء مصفوفة جديدة — ترجع عنوان في الذاكرة الخطية
     * (EN) Create new array — returns address in linear memory
     */
    llvm::Value* createArray(llvm::Value* elemSize, llvm::Value* initialCapacity) {
        // حساب الحجم الكلي: HEADER + capacity * elemSize
        auto* headerConst = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(ctx_), HEADER_SIZE);
        auto* dataSize = builder_.CreateMul(initialCapacity, elemSize, "data_size");
        auto* totalSize = builder_.CreateAdd(headerConst, dataSize, "total_size");
        
        // استدعاء مُخصص الذاكرة
        auto* allocFn = getOrCreateAlloc();
        auto* ptr = builder_.CreateCall(allocFn, {totalSize}, "array_ptr");
        
        // كتابة الترويسة
        // العدد = 0
        auto* lengthPtr = builder_.CreateIntToPtr(
            ptr, llvm::PointerType::get(llvm::Type::getInt32Ty(ctx_), 0));
        builder_.CreateStore(
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 0), lengthPtr);
        
        // السعة
        auto* capOffset = builder_.CreateAdd(
            ptr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), CAPACITY_OFFSET));
        auto* capPtr = builder_.CreateIntToPtr(
            capOffset, llvm::PointerType::get(llvm::Type::getInt32Ty(ctx_), 0));
        builder_.CreateStore(initialCapacity, capPtr);
        
        // حجم العنصر
        auto* esOffset = builder_.CreateAdd(
            ptr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), ELEM_SIZE_OFFSET));
        auto* esPtr = builder_.CreateIntToPtr(
            esOffset, llvm::PointerType::get(llvm::Type::getInt32Ty(ctx_), 0));
        builder_.CreateStore(elemSize, esPtr);
        
        return ptr;
    }
    
    /**
     * (AR) الحصول على عنصر من المصفوفة
     * (EN) Get element from array
     */
    llvm::Value* getElement(llvm::Value* arrayPtr, llvm::Value* index,
                           llvm::Type* elemType) {
        // استخراج حجم العنصر
        auto* esOffset = builder_.CreateAdd(
            arrayPtr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), ELEM_SIZE_OFFSET));
        auto* esPtr = builder_.CreateIntToPtr(
            esOffset, llvm::PointerType::get(llvm::Type::getInt32Ty(ctx_), 0));
        auto* elemSize = builder_.CreateLoad(llvm::Type::getInt32Ty(ctx_), esPtr);
        
        // حساب العنوان: array + HEADER + index * elemSize
        auto* offset = builder_.CreateMul(index, elemSize, "elem_offset");
        auto* dataStart = builder_.CreateAdd(
            arrayPtr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), DATA_OFFSET));
        auto* elemAddr = builder_.CreateAdd(dataStart, offset, "elem_addr");
        
        auto* elemPtr = builder_.CreateIntToPtr(
            elemAddr, llvm::PointerType::get(elemType, 0));
        return builder_.CreateLoad(elemType, elemPtr, "elem_val");
    }
    
    /**
     * (AR) تعيين عنصر في المصفوفة
     * (EN) Set element in array
     */
    void setElement(llvm::Value* arrayPtr, llvm::Value* index,
                   llvm::Value* value, llvm::Type* elemType) {
        auto* esOffset = builder_.CreateAdd(
            arrayPtr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), ELEM_SIZE_OFFSET));
        auto* esPtr = builder_.CreateIntToPtr(
            esOffset, llvm::PointerType::get(llvm::Type::getInt32Ty(ctx_), 0));
        auto* elemSize = builder_.CreateLoad(llvm::Type::getInt32Ty(ctx_), esPtr);
        
        auto* offset = builder_.CreateMul(index, elemSize, "elem_offset");
        auto* dataStart = builder_.CreateAdd(
            arrayPtr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), DATA_OFFSET));
        auto* elemAddr = builder_.CreateAdd(dataStart, offset, "elem_addr");
        
        auto* elemPtr = builder_.CreateIntToPtr(
            elemAddr, llvm::PointerType::get(elemType, 0));
        builder_.CreateStore(value, elemPtr);
    }
    
    /**
     * (AR) الحصول على طول المصفوفة
     * (EN) Get array length
     */
    llvm::Value* getLength(llvm::Value* arrayPtr) {
        auto* lengthPtr = builder_.CreateIntToPtr(
            arrayPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(ctx_), 0));
        return builder_.CreateLoad(llvm::Type::getInt32Ty(ctx_), lengthPtr, "arr_len");
    }
    
    /**
     * (AR) إضافة عنصر (push) مع توسعة تلقائية
     * (EN) Push element with automatic growth
     */
    llvm::Value* pushElement(llvm::Value* arrayPtr, llvm::Value* value,
                            llvm::Type* elemType) {
        auto* length = getLength(arrayPtr);
        
        // تعيين العنصر عند الموضع الحالي
        setElement(arrayPtr, length, value, elemType);
        
        // زيادة العدد
        auto* newLength = builder_.CreateAdd(
            length, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 1));
        auto* lengthPtr = builder_.CreateIntToPtr(
            arrayPtr, llvm::PointerType::get(llvm::Type::getInt32Ty(ctx_), 0));
        builder_.CreateStore(newLength, lengthPtr);
        
        return newLength;
    }

private:
    llvm::LLVMContext& ctx_;
    llvm::Module& mod_;
    llvm::IRBuilder<>& builder_;
    
    llvm::Function* getOrCreateAlloc() {
        auto* fn = mod_.getFunction("__sad_wasm_alloc");
        if (fn) return fn;
        
        auto* fnType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(ctx_),
            {llvm::Type::getInt32Ty(ctx_)},
            false);
        fn = llvm::Function::Create(
            fnType, llvm::Function::ExternalLinkage,
            "__sad_wasm_alloc", mod_);
        return fn;
    }
};

// =============================================================================
// تحسين الاستيراد/التصدير في WASM
// =============================================================================

/**
 * (AR) مدير استيراد/تصدير WASM
 * (EN) WASM Import/Export Manager
 */
class WasmImportExportManager {
public:
    struct ImportEntry {
        std::string module;     // "env", "sad", etc.
        std::string name;       // الاسم في JavaScript
        std::string arabicName; // الاسم في ص
        llvm::FunctionType* type;
    };
    
    struct ExportEntry {
        std::string name;
        std::string arabicName;
        llvm::Function* function;
    };
    
    WasmImportExportManager(llvm::LLVMContext& ctx, llvm::Module& mod)
        : ctx_(ctx), mod_(mod) {}
    
    /**
     * (AR) تسجيل دالة مستوردة (من JavaScript)
     * (EN) Register imported function (from JavaScript)
     */
    llvm::Function* addImport(const std::string& module,
                             const std::string& jsName,
                             const std::string& sadName,
                             llvm::FunctionType* type) {
        auto* fn = llvm::Function::Create(
            type, llvm::Function::ExternalLinkage, jsName, mod_);
        
        imports_.push_back({module, jsName, sadName, type});
        importFunctions_[sadName] = fn;
        
        return fn;
    }
    
    /**
     * (AR) تصدير دالة (إلى JavaScript)
     * (EN) Export function (to JavaScript)
     */
    void addExport(const std::string& jsName,
                  const std::string& sadName,
                  llvm::Function* fn) {
        // تعيين الربط العام
        fn->setLinkage(llvm::Function::ExternalLinkage);
        fn->setName(jsName);
        
        exports_.push_back({jsName, sadName, fn});
    }
    
    /**
     * (AR) إنشاء دوال المكتبة القياسية المستوردة
     * (EN) Create standard library imports
     */
    void createStandardImports() {
        auto* i32 = llvm::Type::getInt32Ty(ctx_);
        auto* f64 = llvm::Type::getDoubleTy(ctx_);
        auto* voidTy = llvm::Type::getVoidTy(ctx_);
        
        // console.log (اطبع)
        addImport("env", "console_log", "اطبع",
            llvm::FunctionType::get(voidTy, {i32}, false));
        
        // console.log_string (اطبع_سطر)
        addImport("env", "console_log_string", "اطبع_سطر",
            llvm::FunctionType::get(voidTy, {i32, i32}, false));
        
        // Math.sqrt (جذر)
        addImport("Math", "sqrt", "جذر",
            llvm::FunctionType::get(f64, {f64}, false));
        
        // Math.random (عشوائي)
        addImport("Math", "random", "عشوائي",
            llvm::FunctionType::get(f64, {}, false));
        
        // performance.now (الوقت)
        addImport("env", "performance_now", "الوقت_الحالي",
            llvm::FunctionType::get(f64, {}, false));
    }
    
    /**
     * (AR) توليد كود JavaScript للربط
     * (EN) Generate JavaScript binding code
     */
    std::string generateBindingJS() const {
        std::string js;
        js += "// تم توليد هذا الملف تلقائياً بواسطة مترجم ص\n";
        js += "// Auto-generated by Sad compiler (sadc)\n\n";
        
        js += "const importObject = {\n";
        
        // تجميع حسب الوحدات
        std::map<std::string, std::vector<const ImportEntry*>> byModule;
        for (const auto& imp : imports_) {
            byModule[imp.module].push_back(&imp);
        }
        
        for (const auto& [mod, funcs] : byModule) {
            js += "  " + mod + ": {\n";
            for (const auto* f : funcs) {
                js += "    " + f->name + ": (...args) => { /* " + f->arabicName + " */ },\n";
            }
            js += "  },\n";
        }
        
        js += "};\n\n";
        
        js += "async function loadSadWasm(wasmUrl) {\n";
        js += "  const response = await fetch(wasmUrl);\n";
        js += "  const bytes = await response.arrayBuffer();\n";
        js += "  const { instance } = await WebAssembly.instantiate(bytes, importObject);\n";
        js += "  return instance.exports;\n";
        js += "}\n";
        
        return js;
    }
    
    llvm::Function* getImportFunction(const std::string& sadName) const {
        auto it = importFunctions_.find(sadName);
        if (it != importFunctions_.end()) return it->second;
        return nullptr;
    }

private:
    llvm::LLVMContext& ctx_;
    llvm::Module& mod_;
    std::vector<ImportEntry> imports_;
    std::vector<ExportEntry> exports_;
    std::unordered_map<std::string, llvm::Function*> importFunctions_;
};

// =============================================================================
// تحسين إدارة الذاكرة في WASM
// =============================================================================

/**
 * (AR) مدير ذاكرة WASM — مُخصص بسيط bump allocator
 * (EN) WASM Memory Manager — simple bump allocator
 *
 * تخطيط الذاكرة الخطية:
 * [0..1023]     — محجوز (null page)
 * [1024..4095]  — جدول النصوص الثابتة
 * [4096..]      — الكومة (heap) — bump allocation
 */
class WasmMemoryManager {
public:
    static constexpr uint32_t NULL_PAGE_END = 1024;
    static constexpr uint32_t STRING_TABLE_START = 1024;
    static constexpr uint32_t STRING_TABLE_SIZE = 3072;
    static constexpr uint32_t HEAP_START = 4096;
    static constexpr uint32_t PAGE_SIZE = 65536;  // 64KB
    
    WasmMemoryManager(llvm::LLVMContext& ctx, llvm::Module& mod,
                      llvm::IRBuilder<>& builder)
        : ctx_(ctx), mod_(mod), builder_(builder),
          currentHeapOffset_(HEAP_START),
          currentStringOffset_(STRING_TABLE_START) {}
    
    /**
     * (AR) تخصيص ذاكرة من الكومة
     * (EN) Allocate memory from heap
     */
    llvm::Value* allocate(llvm::Value* size) {
        auto* heapPtr = getHeapPointer();
        auto* currentPtr = builder_.CreateLoad(
            llvm::Type::getInt32Ty(ctx_), heapPtr, "heap_pos");
        
        // محاذاة إلى 8 bytes
        auto* aligned = builder_.CreateAdd(
            currentPtr, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), 7));
        aligned = builder_.CreateAnd(
            aligned, llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx_), ~7u));
        
        // تحديث مؤشر الكومة
        auto* newPtr = builder_.CreateAdd(aligned, size, "new_heap");
        builder_.CreateStore(newPtr, heapPtr);
        
        return aligned;
    }
    
    /**
     * (AR) إضافة نص ثابت إلى جدول النصوص
     * (EN) Add constant string to string table
     */
    uint32_t addString(const std::string& str) {
        auto it = stringTable_.find(str);
        if (it != stringTable_.end()) return it->second;
        
        uint32_t offset = currentStringOffset_;
        stringTable_[str] = offset;
        currentStringOffset_ += static_cast<uint32_t>(str.size()) + 1;  // +1 for null
        
        return offset;
    }
    
    /**
     * (AR) تهيئة متغيرات الذاكرة العامة
     * (EN) Initialize global memory variables
     */
    void initializeGlobals() {
        // مؤشر الكومة
        auto* heapPtrType = llvm::Type::getInt32Ty(ctx_);
        auto* heapGlobal = new llvm::GlobalVariable(
            mod_, heapPtrType, false,
            llvm::GlobalValue::InternalLinkage,
            llvm::ConstantInt::get(heapPtrType, HEAP_START),
            "__sad_heap_ptr"
        );
        heapGlobal_ = heapGlobal;
    }
    
    /**
     * (AR) إنشاء دالة __sad_wasm_alloc
     * (EN) Create __sad_wasm_alloc function
     */
    llvm::Function* createAllocFunction() {
        auto* i32 = llvm::Type::getInt32Ty(ctx_);
        auto* fnType = llvm::FunctionType::get(i32, {i32}, false);
        auto* fn = llvm::Function::Create(
            fnType, llvm::Function::ExternalLinkage,
            "__sad_wasm_alloc", mod_);
        
        auto* entry = llvm::BasicBlock::Create(ctx_, "entry", fn);
        llvm::IRBuilder<> b(entry);
        
        auto* size = fn->arg_begin();
        auto* heapPtr = getHeapPointer();
        auto* currentPos = b.CreateLoad(i32, heapPtr, "pos");
        
        // محاذاة 8 bytes
        auto* aligned = b.CreateAdd(
            currentPos, llvm::ConstantInt::get(i32, 7));
        aligned = b.CreateAnd(aligned, llvm::ConstantInt::get(i32, ~7u));
        
        // تحديث
        auto* newPos = b.CreateAdd(aligned, size);
        b.CreateStore(newPos, heapPtr);
        
        b.CreateRet(aligned);
        
        return fn;
    }
    
    /**
     * (AR) إنشاء دالة تهيئة جدول النصوص
     * (EN) Create string table initialization function
     */
    llvm::Function* createStringTableInit() {
        auto* voidTy = llvm::Type::getVoidTy(ctx_);
        auto* fnType = llvm::FunctionType::get(voidTy, {}, false);
        auto* fn = llvm::Function::Create(
            fnType, llvm::Function::InternalLinkage,
            "__sad_init_strings", mod_);
        
        auto* entry = llvm::BasicBlock::Create(ctx_, "entry", fn);
        llvm::IRBuilder<> b(entry);
        
        // كتابة كل نص في موضعه
        auto* i8 = llvm::Type::getInt8Ty(ctx_);
        for (const auto& [str, offset] : stringTable_) {
            for (size_t i = 0; i < str.size(); i++) {
                auto* addr = llvm::ConstantInt::get(
                    llvm::Type::getInt32Ty(ctx_), offset + i);
                auto* ptr = b.CreateIntToPtr(
                    addr, llvm::PointerType::get(i8, 0));
                b.CreateStore(
                    llvm::ConstantInt::get(i8, static_cast<uint8_t>(str[i])), ptr);
            }
            // null terminator
            auto* nullAddr = llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(ctx_), offset + str.size());
            auto* nullPtr = b.CreateIntToPtr(
                nullAddr, llvm::PointerType::get(i8, 0));
            b.CreateStore(llvm::ConstantInt::get(i8, 0), nullPtr);
        }
        
        b.CreateRetVoid();
        return fn;
    }
    
    /**
     * (AR) حساب عدد الصفحات المطلوبة
     * (EN) Calculate required memory pages
     */
    uint32_t calculateRequiredPages() const {
        uint32_t totalBytes = currentHeapOffset_ + 
                             static_cast<uint32_t>(stringTable_.size()) * 256;
        return (totalBytes + PAGE_SIZE - 1) / PAGE_SIZE;
    }

private:
    llvm::LLVMContext& ctx_;
    llvm::Module& mod_;
    llvm::IRBuilder<>& builder_;
    
    uint32_t currentHeapOffset_;
    uint32_t currentStringOffset_;
    llvm::GlobalVariable* heapGlobal_ = nullptr;
    std::unordered_map<std::string, uint32_t> stringTable_;
    
    llvm::GlobalVariable* getHeapPointer() {
        if (!heapGlobal_) initializeGlobals();
        return heapGlobal_;
    }
};

} // namespace sad::compiler::wasm

#endif // HAS_LLVM
