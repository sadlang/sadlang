/*
 * تنفيذ مولّد معلومات Debug LLVM / LLVM Debug Info Generator Implementation
 * ========================================================================
 */

#include "llvm_debug.h"
#include <llvm/IR/Constants.h>
#include <iostream>

namespace sad {

/**
 * المُنشئ / Constructor
 */
LLVMDebugInfoGenerator::LLVMDebugInfoGenerator(llvm::Module* module,
                                               const std::string& source_filename,
                                               const std::string& source_directory)
    : module_(module),
      source_filename_(source_filename),
      source_directory_(source_directory),
      compile_unit_(nullptr),
      current_file_(nullptr),
      debug_level_(DebugLevel::DebugInfo),
      enabled_(true) {
}

/**
 * المُدمر / Destructor
 */
LLVMDebugInfoGenerator::~LLVMDebugInfoGenerator() {
}

/**
 * تهيئة مولّد معلومات Debug / Initialize debug info generator
 */
bool LLVMDebugInfoGenerator::initialize(DebugLevel level) {
    if (!module_) {
        std::cerr << "خطأ: وحدة فارغة / Error: null module" << std::endl;
        return false;
    }
    
    debug_level_ = level;
    
    if (level == DebugLevel::None) {
        enabled_ = false;
        return true;
    }
    
    // إنشاء DIBuilder / Create DIBuilder
    di_builder_ = std::make_unique<llvm::DIBuilder>(*module_);
    
    // إنشاء الملف / Create file
    current_file_ = createFile(source_filename_, source_directory_);
    
    // إنشاء وحدة الترجمة / Create compile unit
    compile_unit_ = createCompileUnit("SadLang", "SadCompiler 1.0", false);
    
    if (!compile_unit_) {
        std::cerr << "خطأ: فشل إنشاء وحدة الترجمة / Error: Failed to create compile unit" << std::endl;
        return false;
    }
    
    // دفع وحدة الترجمة كنطاق أولي / Push compile unit as initial scope
    scope_stack_.push_back(compile_unit_);
    
    // إنشاء الأنواع الأساسية / Create basic types
    createBasicTypes();
    
    return true;
}

/**
 * إنهاء إنشاء معلومات Debug / Finalize debug info generation
 */
void LLVMDebugInfoGenerator::finalize() {
    if (di_builder_ && enabled_) {
        di_builder_->finalize();
    }
}

/**
 * تعيين مستوى Debug / Set debug level
 */
void LLVMDebugInfoGenerator::setDebugLevel(DebugLevel level) {
    debug_level_ = level;
    enabled_ = (level != DebugLevel::None);
}

/**
 * الحصول على مستوى Debug الحالي / Get current debug level
 */
DebugLevel LLVMDebugInfoGenerator::getDebugLevel() const {
    return debug_level_;
}

/**
 * إنشاء وحدة ترجمة / Create compile unit
 */
llvm::DICompileUnit* LLVMDebugInfoGenerator::createCompileUnit(const std::string& language,
                                                               const std::string& producer,
                                                               bool optimized) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    return di_builder_->createCompileUnit(
        llvm::dwarf::DW_LANG_C,     // اللغة / Language (استخدام C كبديل)
        current_file_,              // الملف / File
        producer,                   // المُنتج / Producer
        optimized,                  // محسّن / Optimized
        "",                         // أعلام / Flags
        0                           // رقم الإصدار / Runtime version
    );
}

/**
 * إنشاء ملف / Create file
 */
llvm::DIFile* LLVMDebugInfoGenerator::createFile(const std::string& filename,
                                                const std::string& directory) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    return di_builder_->createFile(filename, directory);
}

/**
 * إنشاء دالة / Create function
 */
llvm::DISubprogram* LLVMDebugInfoGenerator::createFunction(const std::string& name,
                                                          const std::string& linkage_name,
                                                          llvm::DIFile* file,
                                                          unsigned line_no,
                                                          llvm::DISubroutineType* type,
                                                          bool is_local,
                                                          bool is_definition,
                                                          unsigned scope_line,
                                                          llvm::DINode::DIFlags flags,
                                                          bool is_optimized) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    llvm::DIScope* scope = getCurrentScope();
    
    auto func_di = di_builder_->createFunction(
        scope,                      // النطاق / Scope
        name,                       // الاسم / Name
        linkage_name,               // اسم الربط / Linkage name
        file,                       // الملف / File
        line_no,                    // رقم السطر / Line number
        type,                       // النوع / Type
        scope_line,                 // سطر النطاق / Scope line
        flags,                      // الأعلام / Flags
        llvm::DISubprogram::toSPFlags(is_local, is_definition, is_optimized)
    );
    
    return func_di;
}

/**
 * إنشاء دالة (نسخة مبسطة) / Create function (simplified)
 */
llvm::DISubprogram* LLVMDebugInfoGenerator::createFunction(const std::string& name,
                                                          unsigned line_no,
                                                          bool is_local) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    // إنشاء نوع دالة بسيط / Create simple function type
    llvm::DIType* void_type = basic_types_["void"];
    if (!void_type) {
        void_type = createBasicType("void", 0, llvm::dwarf::DW_ATE_address);
        basic_types_["void"] = void_type;
    }
    
    auto func_type = createFunctionType(void_type, {});
    
    return createFunction(
        name,                       // الاسم / Name
        name,                       // اسم الربط / Linkage name
        current_file_,              // الملف / File
        line_no,                    // رقم السطر / Line number
        func_type,                  // النوع / Type
        is_local,                   // محلية / Local
        true,                       // تعريف / Definition
        line_no,                    // سطر النطاق / Scope line
        llvm::DINode::FlagPrototyped, // الأعلام / Flags
        false                       // محسّنة / Optimized
    );
}

/**
 * إنشاء متغير محلي / Create local variable
 */
llvm::DILocalVariable* LLVMDebugInfoGenerator::createLocalVariable(const std::string& name,
                                                                  llvm::DIFile* file,
                                                                  unsigned line_no,
                                                                  llvm::DIType* type,
                                                                  unsigned arg_no) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    llvm::DIScope* scope = getCurrentScope();
    
    return di_builder_->createAutoVariable(
        scope,                      // النطاق / Scope
        name,                       // الاسم / Name
        file,                       // الملف / File
        line_no,                    // رقم السطر / Line number
        type,                       // النوع / Type
        true,                       // دائماً محفوظ / Always preserve
        llvm::DINode::FlagZero,     // الأعلام / Flags
        arg_no > 0 ? 8 : 0         // المحاذاة / Alignment
    );
}

/**
 * إنشاء متغير محلي (نسخة مبسطة) / Create local variable (simplified)
 */
llvm::DILocalVariable* LLVMDebugInfoGenerator::createLocalVariable(const std::string& name,
                                                                  unsigned line_no,
                                                                  llvm::DIType* type) {
    return createLocalVariable(name, current_file_, line_no, type, 0);
}

/**
 * إنشاء متغير عام / Create global variable
 */
llvm::DIGlobalVariableExpression* LLVMDebugInfoGenerator::createGlobalVariable(
    const std::string& name,
    const std::string& linkage_name,
    llvm::DIFile* file,
    unsigned line_no,
    llvm::DIType* type,
    bool is_local) {
    
    if (!di_builder_ || !enabled_) return nullptr;
    
    return di_builder_->createGlobalVariableExpression(
        getCurrentScope(),          // النطاق / Scope
        name,                       // الاسم / Name
        linkage_name,               // اسم الربط / Linkage name
        file,                       // الملف / File
        line_no,                    // رقم السطر / Line number
        type,                       // النوع / Type
        is_local                    // محلي / Local
    );
}

/**
 * إنشاء نوع أساسي / Create basic type
 */
llvm::DIBasicType* LLVMDebugInfoGenerator::createBasicType(const std::string& name,
                                                          uint64_t size_in_bits,
                                                          unsigned encoding) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    return di_builder_->createBasicType(name, size_in_bits, encoding);
}

/**
 * إنشاء نوع مؤشر / Create pointer type
 */
llvm::DIDerivedType* LLVMDebugInfoGenerator::createPointerType(llvm::DIType* pointee_type,
                                                              uint64_t size_in_bits) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    return di_builder_->createPointerType(pointee_type, size_in_bits);
}

/**
 * إنشاء نوع مصفوفة / Create array type
 */
llvm::DICompositeType* LLVMDebugInfoGenerator::createArrayType(llvm::DIType* element_type,
                                                              uint64_t size_in_bits,
                                                              uint64_t count) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    llvm::SmallVector<llvm::Metadata*, 1> subscripts;
    subscripts.push_back(di_builder_->getOrCreateSubrange(0, count));
    
    return di_builder_->createArrayType(
        size_in_bits,               // الحجم / Size
        0,                          // المحاذاة / Alignment
        element_type,               // نوع العنصر / Element type
        di_builder_->getOrCreateArray(subscripts)
    );
}

/**
 * إنشاء نوع بنية / Create structure type
 */
llvm::DICompositeType* LLVMDebugInfoGenerator::createStructType(const std::string& name,
                                                               llvm::DIFile* file,
                                                               unsigned line_no,
                                                               uint64_t size_in_bits,
                                                               uint32_t align_in_bits,
                                                               llvm::DINodeArray elements) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    return di_builder_->createStructType(
        getCurrentScope(),          // النطاق / Scope
        name,                       // الاسم / Name
        file,                       // الملف / File
        line_no,                    // رقم السطر / Line number
        size_in_bits,               // الحجم / Size
        align_in_bits,              // المحاذاة / Alignment
        llvm::DINode::FlagZero,     // الأعلام / Flags
        nullptr,                    // النوع المشتق منه / Derived from
        elements                    // العناصر / Elements
    );
}

/**
 * إنشاء نوع دالة / Create function type
 */
llvm::DISubroutineType* LLVMDebugInfoGenerator::createFunctionType(
    llvm::DIType* return_type,
    const std::vector<llvm::DIType*>& param_types) {
    
    if (!di_builder_ || !enabled_) return nullptr;
    
    llvm::SmallVector<llvm::Metadata*, 8> types;
    types.push_back(return_type);
    
    for (auto param_type : param_types) {
        types.push_back(param_type);
    }
    
    return di_builder_->createSubroutineType(di_builder_->getOrCreateTypeArray(types));
}

/**
 * إدراج موقع debug / Insert debug location
 */
void LLVMDebugInfoGenerator::insertDebugLocation(llvm::IRBuilder<>& builder,
                                                unsigned line,
                                                unsigned column,
                                                llvm::DIScope* scope) {
    if (!enabled_) return;
    
    if (!scope) {
        scope = getCurrentScope();
    }
    
    if (scope) {
        auto loc = llvm::DILocation::get(
            scope->getContext(),
            line,
            column,
            scope
        );
        builder.SetCurrentDebugLocation(loc);
    }
}

/**
 * إنشاء نطاق معجمي / Create lexical block
 */
llvm::DILexicalBlock* LLVMDebugInfoGenerator::createLexicalBlock(llvm::DIScope* scope,
                                                                llvm::DIFile* file,
                                                                unsigned line,
                                                                unsigned column) {
    if (!di_builder_ || !enabled_) return nullptr;
    
    if (!scope) {
        scope = getCurrentScope();
    }
    
    return di_builder_->createLexicalBlock(scope, file, line, column);
}

/**
 * الحصول على DIBuilder / Get DIBuilder
 */
llvm::DIBuilder* LLVMDebugInfoGenerator::getDIBuilder() const {
    return di_builder_.get();
}

/**
 * الحصول على وحدة الترجمة / Get compile unit
 */
llvm::DICompileUnit* LLVMDebugInfoGenerator::getCompileUnit() const {
    return compile_unit_;
}

/**
 * الحصول على الملف الحالي / Get current file
 */
llvm::DIFile* LLVMDebugInfoGenerator::getCurrentFile() const {
    return current_file_;
}

/**
 * دفع نطاق / Push scope
 */
void LLVMDebugInfoGenerator::pushScope(llvm::DIScope* scope) {
    if (scope) {
        scope_stack_.push_back(scope);
    }
}

/**
 * سحب نطاق / Pop scope
 */
void LLVMDebugInfoGenerator::popScope() {
    if (scope_stack_.size() > 1) {  // دائماً احتفظ بوحدة الترجمة / Always keep compile unit
        scope_stack_.pop_back();
    }
}

/**
 * الحصول على النطاق الحالي / Get current scope
 */
llvm::DIScope* LLVMDebugInfoGenerator::getCurrentScope() const {
    if (!scope_stack_.empty()) {
        return scope_stack_.back();
    }
    return compile_unit_;
}

/**
 * تمكين/تعطيل معلومات Debug / Enable/disable debug info
 */
void LLVMDebugInfoGenerator::setEnabled(bool enabled) {
    enabled_ = enabled;
}

/**
 * التحقق من تمكين معلومات Debug / Check if debug info is enabled
 */
bool LLVMDebugInfoGenerator::isEnabled() const {
    return enabled_;
}

/**
 * إنشاء الأنواع الأساسية / Create basic types
 */
void LLVMDebugInfoGenerator::createBasicTypes() {
    if (!di_builder_ || !enabled_) return;
    
    // void
    basic_types_["void"] = createBasicType("void", 0, llvm::dwarf::DW_ATE_address);
    
    // bool
    basic_types_["bool"] = createBasicType("bool", 8, llvm::dwarf::DW_ATE_boolean);
    
    // الأعداد الصحيحة / Integers
    basic_types_["i8"] = createBasicType("i8", 8, llvm::dwarf::DW_ATE_signed);
    basic_types_["i16"] = createBasicType("i16", 16, llvm::dwarf::DW_ATE_signed);
    basic_types_["i32"] = createBasicType("i32", 32, llvm::dwarf::DW_ATE_signed);
    basic_types_["i64"] = createBasicType("i64", 64, llvm::dwarf::DW_ATE_signed);
    
    basic_types_["u8"] = createBasicType("u8", 8, llvm::dwarf::DW_ATE_unsigned);
    basic_types_["u16"] = createBasicType("u16", 16, llvm::dwarf::DW_ATE_unsigned);
    basic_types_["u32"] = createBasicType("u32", 32, llvm::dwarf::DW_ATE_unsigned);
    basic_types_["u64"] = createBasicType("u64", 64, llvm::dwarf::DW_ATE_unsigned);
    
    // أعداد عشرية / Floating point
    basic_types_["f32"] = createBasicType("f32", 32, llvm::dwarf::DW_ATE_float);
    basic_types_["f64"] = createBasicType("f64", 64, llvm::dwarf::DW_ATE_float);
    
    // أنواع Sad المخصصة / Sad custom types
    basic_types_["int"] = basic_types_["i64"];      // int = i64
    basic_types_["float"] = basic_types_["f64"];    // float = f64
    basic_types_["string"] = createPointerType(basic_types_["i8"], 64);  // string = i8*
}

/**
 * الحصول على ترميز النوع الأساسي / Get basic type encoding
 */
unsigned LLVMDebugInfoGenerator::getBasicTypeEncoding(const std::string& type_name) {
    if (type_name == "bool") {
        return llvm::dwarf::DW_ATE_boolean;
    } else if (type_name.find("i") == 0 || type_name == "int") {
        return llvm::dwarf::DW_ATE_signed;
    } else if (type_name.find("u") == 0) {
        return llvm::dwarf::DW_ATE_unsigned;
    } else if (type_name.find("f") == 0 || type_name == "float") {
        return llvm::dwarf::DW_ATE_float;
    } else if (type_name == "string") {
        return llvm::dwarf::DW_ATE_address;
    }
    return llvm::dwarf::DW_ATE_address;
}

} // namespace sad
