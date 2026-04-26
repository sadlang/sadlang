/*
 * مولّد معلومات Debug LLVM / LLVM Debug Info Generator
 * ===================================================
 * 
 * هذا الملف يوفر واجهة لإنشاء معلومات Debug بصيغة DWARF لتسهيل التصحيح.
 * This file provides an interface for generating DWARF debug information for easier debugging.
 * 
 * معلومات Debug المدعومة / Supported Debug Information:
 * - Compile Units: معلومات وحدات الترجمة
 * - Functions: معلومات الدوال (اسم، نوع، موقع)
 * - Variables: معلومات المتغيرات (محلية، عامة، معاملات)
 * - Types: معلومات الأنواع (أساسية، مركبة، مؤشرات)
 * - Source Locations: مواقع الكود المصدري (ملف، سطر، عمود)
 * - Scopes: نطاقات الكود (lexical blocks)
 * 
 * مستويات Debug المدعومة / Supported Debug Levels:
 * - None: بدون معلومات debug
 * - LineTablesOnly: جداول الأسطر فقط
 * - DebugInfo: معلومات debug كاملة
 * 
 * مثال الاستخدام / Usage Example:
 * ```cpp
 * LLVMDebugInfoGenerator debug_gen(module.get(), "program.ص", "/home/user/project");
 * debug_gen.initialize();
 * 
 * auto func_di = debug_gen.createFunction("main", 1, false);
 * auto var_di = debug_gen.createLocalVariable("x", 5, int_type);
 * debug_gen.insertDebugLocation(builder, 10, 5);
 * 
 * debug_gen.finalize();
 * ```
 */

#ifndef LLVM_DEBUG_H
#define LLVM_DEBUG_H

#include <llvm/IR/Module.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace sad {

/**
 * مستويات معلومات Debug / Debug Information Levels
 */
enum class DebugLevel {
    None,              // بدون معلومات debug / No debug info
    LineTablesOnly,    // جداول الأسطر فقط / Line tables only
    DebugInfo          // معلومات debug كاملة / Full debug info
};

/**
 * أنواع الأعلام / Debug Flags
 */
enum class DebugFlags {
    None = 0,
    Private = 1,       // خاص / Private
    Protected = 2,     // محمي / Protected
    Public = 4,        // عام / Public
    Forward = 8,       // إعلان مسبق / Forward declaration
    AppleBlock = 16,
    Virtual = 32,      // افتراضي / Virtual
    Artificial = 64    // مصطنع / Artificial
};

/**
 * معلومات موقع المصدر / Source Location Info
 */
struct SourceLocation {
    std::string filename;      // اسم الملف / Filename
    unsigned line;             // رقم السطر / Line number
    unsigned column;           // رقم العمود / Column number
    
    SourceLocation() : line(0), column(0) {}
    SourceLocation(const std::string& file, unsigned l, unsigned c)
        : filename(file), line(l), column(c) {}
};

/**
 * معلومات النطاق / Scope Information
 */
struct ScopeInfo {
    llvm::DIScope* scope;      // النطاق / Scope
    unsigned line;             // سطر البداية / Start line
    
    ScopeInfo() : scope(nullptr), line(0) {}
    ScopeInfo(llvm::DIScope* s, unsigned l) : scope(s), line(l) {}
};

/**
 * مولّد معلومات Debug LLVM / LLVM Debug Info Generator
 * 
 * هذه الفئة توفر واجهة عالية المستوى لإنشاء معلومات debug بصيغة DWARF.
 * This class provides a high-level interface for generating DWARF debug information.
 */
class LLVMDebugInfoGenerator {
public:
    /**
     * المُنشئ / Constructor
     * @param module الوحدة / Module
     * @param source_filename اسم ملف المصدر / Source filename
     * @param source_directory مجلد المصدر / Source directory
     */
    LLVMDebugInfoGenerator(llvm::Module* module,
                           const std::string& source_filename,
                           const std::string& source_directory);
    
    /**
     * المُدمر / Destructor
     */
    ~LLVMDebugInfoGenerator();
    
    /**
     * تهيئة مولّد معلومات Debug / Initialize debug info generator
     * @param level مستوى معلومات Debug / Debug info level
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize(DebugLevel level = DebugLevel::DebugInfo);
    
    /**
     * إنهاء إنشاء معلومات Debug / Finalize debug info generation
     */
    void finalize();
    
    /**
     * تعيين مستوى Debug / Set debug level
     * @param level المستوى / Level
     */
    void setDebugLevel(DebugLevel level);
    
    /**
     * الحصول على مستوى Debug الحالي / Get current debug level
     * @return المستوى / Level
     */
    DebugLevel getDebugLevel() const;
    
    /**
     * إنشاء وحدة ترجمة / Create compile unit
     * @param language اللغة (مثل: "SadLang") / Language name
     * @param producer المُنتج (مثل: "SadCompiler 1.0") / Producer name
     * @param optimized هل الكود محسّن؟ / Is code optimized?
     * @return وحدة الترجمة / Compile unit
     */
    llvm::DICompileUnit* createCompileUnit(const std::string& language,
                                          const std::string& producer,
                                          bool optimized = false);
    
    /**
     * إنشاء ملف / Create file
     * @param filename اسم الملف / Filename
     * @param directory المجلد / Directory
     * @return معلومات الملف / File info
     */
    llvm::DIFile* createFile(const std::string& filename,
                            const std::string& directory);
    
    /**
     * إنشاء دالة / Create function
     * @param name اسم الدالة / Function name
     * @param linkage_name اسم الربط / Linkage name
     * @param file الملف / File
     * @param line_no رقم السطر / Line number
     * @param type نوع الدالة / Function type
     * @param is_local هل محلية؟ / Is local?
     * @param is_definition هل تعريف؟ / Is definition?
     * @param scope_line سطر النطاق / Scope line
     * @param flags الأعلام / Flags
     * @param is_optimized هل محسّنة؟ / Is optimized?
     * @return معلومات الدالة / Function debug info
     */
    llvm::DISubprogram* createFunction(const std::string& name,
                                      const std::string& linkage_name,
                                      llvm::DIFile* file,
                                      unsigned line_no,
                                      llvm::DISubroutineType* type,
                                      bool is_local,
                                      bool is_definition,
                                      unsigned scope_line,
                                      llvm::DINode::DIFlags flags,
                                      bool is_optimized);
    
    /**
     * إنشاء دالة (نسخة مبسطة) / Create function (simplified)
     * @param name اسم الدالة / Function name
     * @param line_no رقم السطر / Line number
     * @param is_local هل محلية؟ / Is local?
     * @return معلومات الدالة / Function debug info
     */
    llvm::DISubprogram* createFunction(const std::string& name,
                                      unsigned line_no,
                                      bool is_local = false);
    
    /**
     * إنشاء متغير محلي / Create local variable
     * @param name اسم المتغير / Variable name
     * @param file الملف / File
     * @param line_no رقم السطر / Line number
     * @param type نوع المتغير / Variable type
     * @param arg_no رقم المعامل (0 للمتغيرات المحلية) / Arg number (0 for locals)
     * @return معلومات المتغير / Variable debug info
     */
    llvm::DILocalVariable* createLocalVariable(const std::string& name,
                                               llvm::DIFile* file,
                                               unsigned line_no,
                                               llvm::DIType* type,
                                               unsigned arg_no = 0);
    
    /**
     * إنشاء متغير محلي (نسخة مبسطة) / Create local variable (simplified)
     * @param name اسم المتغير / Variable name
     * @param line_no رقم السطر / Line number
     * @param type نوع المتغير / Variable type
     * @return معلومات المتغير / Variable debug info
     */
    llvm::DILocalVariable* createLocalVariable(const std::string& name,
                                               unsigned line_no,
                                               llvm::DIType* type);
    
    /**
     * إنشاء متغير عام / Create global variable
     * @param name اسم المتغير / Variable name
     * @param linkage_name اسم الربط / Linkage name
     * @param file الملف / File
     * @param line_no رقم السطر / Line number
     * @param type نوع المتغير / Variable type
     * @param is_local هل محلي؟ / Is local?
     * @return معلومات المتغير العام / Global variable debug info
     */
    llvm::DIGlobalVariableExpression* createGlobalVariable(const std::string& name,
                                                           const std::string& linkage_name,
                                                           llvm::DIFile* file,
                                                           unsigned line_no,
                                                           llvm::DIType* type,
                                                           bool is_local);
    
    /**
     * إنشاء نوع أساسي / Create basic type
     * @param name اسم النوع / Type name
     * @param size_in_bits الحجم بالبتات / Size in bits
     * @param encoding الترميز / Encoding
     * @return نوع أساسي / Basic type
     */
    llvm::DIBasicType* createBasicType(const std::string& name,
                                       uint64_t size_in_bits,
                                       unsigned encoding);
    
    /**
     * إنشاء نوع مؤشر / Create pointer type
     * @param pointee_type نوع المشار إليه / Pointee type
     * @param size_in_bits الحجم بالبتات / Size in bits
     * @return نوع مؤشر / Pointer type
     */
    llvm::DIDerivedType* createPointerType(llvm::DIType* pointee_type,
                                          uint64_t size_in_bits = 64);
    
    /**
     * إنشاء نوع مصفوفة / Create array type
     * @param element_type نوع العنصر / Element type
     * @param size_in_bits الحجم بالبتات / Size in bits
     * @param count عدد العناصر / Element count
     * @return نوع مصفوفة / Array type
     */
    llvm::DICompositeType* createArrayType(llvm::DIType* element_type,
                                          uint64_t size_in_bits,
                                          uint64_t count);
    
    /**
     * إنشاء نوع بنية / Create structure type
     * @param name اسم البنية / Structure name
     * @param file الملف / File
     * @param line_no رقم السطر / Line number
     * @param size_in_bits الحجم بالبتات / Size in bits
     * @param align_in_bits المحاذاة بالبتات / Alignment in bits
     * @param elements العناصر / Elements
     * @return نوع بنية / Structure type
     */
    llvm::DICompositeType* createStructType(const std::string& name,
                                           llvm::DIFile* file,
                                           unsigned line_no,
                                           uint64_t size_in_bits,
                                           uint32_t align_in_bits,
                                           llvm::DINodeArray elements);
    
    /**
     * إنشاء نوع دالة / Create function type
     * @param return_type نوع الإرجاع / Return type
     * @param param_types أنواع المعاملات / Parameter types
     * @return نوع دالة / Function type
     */
    llvm::DISubroutineType* createFunctionType(llvm::DIType* return_type,
                                               const std::vector<llvm::DIType*>& param_types);
    
    /**
     * إدراج موقع debug / Insert debug location
     * @param builder باني IR / IR builder
     * @param line رقم السطر / Line number
     * @param column رقم العمود / Column number
     * @param scope النطاق / Scope
     */
    void insertDebugLocation(llvm::IRBuilder<>& builder,
                           unsigned line,
                           unsigned column,
                           llvm::DIScope* scope = nullptr);
    
    /**
     * إنشاء نطاق معجمي / Create lexical block
     * @param scope النطاق الأب / Parent scope
     * @param file الملف / File
     * @param line رقم السطر / Line number
     * @param column رقم العمود / Column number
     * @return النطاق المعجمي / Lexical block
     */
    llvm::DILexicalBlock* createLexicalBlock(llvm::DIScope* scope,
                                            llvm::DIFile* file,
                                            unsigned line,
                                            unsigned column);
    
    /**
     * الحصول على DIBuilder / Get DIBuilder
     * @return باني معلومات Debug / Debug info builder
     */
    llvm::DIBuilder* getDIBuilder() const;
    
    /**
     * الحصول على وحدة الترجمة / Get compile unit
     * @return وحدة الترجمة / Compile unit
     */
    llvm::DICompileUnit* getCompileUnit() const;
    
    /**
     * الحصول على الملف الحالي / Get current file
     * @return معلومات الملف / File info
     */
    llvm::DIFile* getCurrentFile() const;
    
    /**
     * دفع نطاق / Push scope
     * @param scope النطاق / Scope
     */
    void pushScope(llvm::DIScope* scope);
    
    /**
     * سحب نطاق / Pop scope
     */
    void popScope();
    
    /**
     * الحصول على النطاق الحالي / Get current scope
     * @return النطاق / Scope
     */
    llvm::DIScope* getCurrentScope() const;
    
    /**
     * تمكين/تعطيل معلومات Debug / Enable/disable debug info
     * @param enabled true للتمكين / true to enable
     */
    void setEnabled(bool enabled);
    
    /**
     * التحقق من تمكين معلومات Debug / Check if debug info is enabled
     * @return true إذا كانت ممكّنة / true if enabled
     */
    bool isEnabled() const;

private:
    /**
     * إنشاء الأنواع الأساسية / Create basic types
     */
    void createBasicTypes();
    
    /**
     * الحصول على ترميز النوع الأساسي / Get basic type encoding
     * @param type_name اسم النوع / Type name
     * @return الترميز / Encoding
     */
    unsigned getBasicTypeEncoding(const std::string& type_name);

private:
    llvm::Module* module_;                           // الوحدة / Module
    std::unique_ptr<llvm::DIBuilder> di_builder_;    // باني معلومات Debug / Debug info builder
    
    std::string source_filename_;                    // اسم ملف المصدر / Source filename
    std::string source_directory_;                   // مجلد المصدر / Source directory
    
    llvm::DICompileUnit* compile_unit_;              // وحدة الترجمة / Compile unit
    llvm::DIFile* current_file_;                     // الملف الحالي / Current file
    
    DebugLevel debug_level_;                         // مستوى Debug / Debug level
    bool enabled_;                                   // هل ممكّن؟ / Is enabled?
    
    std::vector<llvm::DIScope*> scope_stack_;        // مكدس النطاقات / Scope stack
    
    // أنواع أساسية مخزّنة / Cached basic types
    std::unordered_map<std::string, llvm::DIType*> basic_types_;
};

} // namespace sad

#endif // LLVM_DEBUG_H
