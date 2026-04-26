// ════════════════════════════════════════════════════════════════════════════════
// ملف: llvm_custom_sections.h
// File: llvm_custom_sections.h
//
// الوصف: إدارة الأقسام المخصصة في LLVM IR — وضع البيانات والدوال في أقسام محددة
// Description: Manage custom sections in LLVM IR — place data and functions in specific sections
//
// المؤلف: Sad Compiler — Phase ز
// Author: Sad Compiler — Phase ز
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_LLVM_CUSTOM_SECTIONS_H
#define SAD_COMPILER_LLVM_CUSTOM_SECTIONS_H

#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

#include <string>
#include <vector>
#include <map>
#include <set>

namespace Sad {
namespace LLVM {

// ════════════════════════════════════════════════════════════════════════════════
// بنية معلومات القسم / Section Info
// ════════════════════════════════════════════════════════════════════════════════

struct SectionInfo {
    std::string name;           // اسم القسم / Section name (e.g., ".multiboot")
    std::string arabicName;     // الاسم العربي / Arabic name
    bool isReadOnly = false;    // قراءة فقط / Read-only
    bool isExecutable = false;  // قابل للتنفيذ / Executable
    bool isNoLoad = false;      // لا يُحمل في الذاكرة / Not loaded into memory
    bool keepSection = true;    // KEEP() في سكربت الربط / KEEP() in linker script
    uint64_t alignment = 0;    // المحاذاة / Alignment (0 = default)
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: LLVMCustomSections
// Class: LLVMCustomSections
//
// الوصف: يدير وضع الثوابت والمتغيرات والدوال في أقسام محددة
// Description: Manages placing globals, variables, and functions in specific sections
//
// الاستخدام / Usage:
//   #[قسم(".multiboot")] ثابت ترويسة = ...  →  @header in section .multiboot
//   #[قسم(".interrupt_table")] ثابت جدول = ...
// ════════════════════════════════════════════════════════════════════════════════

class LLVMCustomSections {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئ / Constructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    explicit LLVMCustomSections(llvm::Module& module, llvm::LLVMContext& context);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تسجيل الأقسام / Register Sections
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تسجيل قسم مخصص / Register a custom section
    void registerSection(const SectionInfo& info);
    
    // التحقق من وجود قسم / Check if section exists
    bool hasSection(const std::string& name) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // وضع المتغيرات في أقسام / Place Variables in Sections
    // ═══════════════════════════════════════════════════════════════════════════
    
    // وضع متغير عام في قسم محدد / Place a global variable in a section
    void placeGlobalInSection(llvm::GlobalVariable* global, const std::string& sectionName);
    
    // إنشاء متغير عام في قسم محدد / Create a global variable in a section
    llvm::GlobalVariable* createGlobalInSection(
        const std::string& name,
        llvm::Type* type,
        llvm::Constant* initializer,
        const std::string& sectionName,
        bool isConstant = false
    );
    
    // ═══════════════════════════════════════════════════════════════════════════
    // وضع الدوال في أقسام / Place Functions in Sections
    // ═══════════════════════════════════════════════════════════════════════════
    
    // وضع دالة في قسم محدد / Place a function in a section
    void placeFunctionInSection(llvm::Function* func, const std::string& sectionName);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // معالجة سمة القسم / Process Section Attribute
    // ═══════════════════════════════════════════════════════════════════════════
    
    // معالجة سمة #[قسم("...")] من AST / Process #[section("...")] attribute from AST
    // يُرجع true إذا نُفذ بنجاح / Returns true if processed successfully
    bool processSectionAttribute(const std::string& attributeValue, 
                                  llvm::GlobalObject* globalObj);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // أقسام شائعة / Common Sections
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تسجيل أقسام النواة الشائعة / Register common kernel sections
    void registerKernelSections();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الاستعلام / Query
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على جميع الأقسام المخصصة / Get all custom sections
    std::vector<SectionInfo> getAllSections() const;
    
    // الحصول على الأقسام المستخدمة / Get used sections
    std::set<std::string> getUsedSections() const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء / Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    llvm::Module& module_;
    llvm::LLVMContext& context_;
    
    // خريطة الأقسام المسجلة / Registered sections map
    std::map<std::string, SectionInfo> sections_;
    
    // الأقسام المستخدمة فعلاً / Actually used sections
    std::set<std::string> usedSections_;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // مساعدات / Helpers
    // ═══════════════════════════════════════════════════════════════════════════
    
    // ترجمة اسم القسم العربي / Translate Arabic section name
    std::string translateSectionName(const std::string& name) const;
};

} // namespace LLVM
} // namespace Sad

#endif // SAD_COMPILER_LLVM_CUSTOM_SECTIONS_H
