// ════════════════════════════════════════════════════════════════════════════════
// ملف: llvm_custom_sections.cpp
// File: llvm_custom_sections.cpp
//
// الوصف: تنفيذ إدارة الأقسام المخصصة في LLVM IR
// Description: Implementation of custom section management in LLVM IR
//
// المؤلف: Sad Compiler — Phase ز
// Author: Sad Compiler — Phase ز
// ════════════════════════════════════════════════════════════════════════════════

#include "backend/llvm/llvm_custom_sections.h"
#include <iostream>
#include <algorithm>

namespace Sad {
namespace LLVM {

// ════════════════════════════════════════════════════════════════════════════════
// المُنشئ / Constructor
// ════════════════════════════════════════════════════════════════════════════════

LLVMCustomSections::LLVMCustomSections(llvm::Module& module, llvm::LLVMContext& context)
    : module_(module), context_(context) {}

// ════════════════════════════════════════════════════════════════════════════════
// تسجيل الأقسام / Register Sections
// ════════════════════════════════════════════════════════════════════════════════

void LLVMCustomSections::registerSection(const SectionInfo& info) {
    sections_[info.name] = info;
}

bool LLVMCustomSections::hasSection(const std::string& name) const {
    return sections_.find(name) != sections_.end();
}

// ════════════════════════════════════════════════════════════════════════════════
// وضع المتغيرات في أقسام / Place Variables in Sections
// ════════════════════════════════════════════════════════════════════════════════

void LLVMCustomSections::placeGlobalInSection(llvm::GlobalVariable* global, 
                                                const std::string& sectionName) {
    if (!global) return;
    
    std::string resolvedName = translateSectionName(sectionName);
    global->setSection(resolvedName);
    usedSections_.insert(resolvedName);
    
    // تطبيق خصائص القسم / Apply section properties
    auto it = sections_.find(resolvedName);
    if (it != sections_.end()) {
        const auto& info = it->second;
        
        if (info.isReadOnly) {
            global->setConstant(true);
        }
        
        if (info.alignment > 0) {
            global->setAlignment(llvm::Align(info.alignment));
        }
    }
}

llvm::GlobalVariable* LLVMCustomSections::createGlobalInSection(
    const std::string& name,
    llvm::Type* type,
    llvm::Constant* initializer,
    const std::string& sectionName,
    bool isConstant) {
    
    auto* global = new llvm::GlobalVariable(
        module_,
        type,
        isConstant,
        llvm::GlobalValue::ExternalLinkage,
        initializer,
        name
    );
    
    placeGlobalInSection(global, sectionName);
    return global;
}

// ════════════════════════════════════════════════════════════════════════════════
// وضع الدوال في أقسام / Place Functions in Sections
// ════════════════════════════════════════════════════════════════════════════════

void LLVMCustomSections::placeFunctionInSection(llvm::Function* func, 
                                                  const std::string& sectionName) {
    if (!func) return;
    
    std::string resolvedName = translateSectionName(sectionName);
    func->setSection(resolvedName);
    usedSections_.insert(resolvedName);
}

// ════════════════════════════════════════════════════════════════════════════════
// معالجة سمة القسم / Process Section Attribute
// ════════════════════════════════════════════════════════════════════════════════

bool LLVMCustomSections::processSectionAttribute(const std::string& attributeValue, 
                                                   llvm::GlobalObject* globalObj) {
    if (!globalObj || attributeValue.empty()) {
        return false;
    }
    
    // استخراج اسم القسم من القيمة / Extract section name from value
    // الصيغة: "اسم_القسم" أو اسم_القسم / Format: "section_name" or section_name
    std::string sectionName = attributeValue;
    
    // إزالة علامات التنصيص / Remove quotes
    if (sectionName.size() >= 2 && sectionName.front() == '"' && sectionName.back() == '"') {
        sectionName = sectionName.substr(1, sectionName.size() - 2);
    }
    
    // ترجمة الاسم / Translate name
    sectionName = translateSectionName(sectionName);
    
    // وضع الكائن في القسم / Place object in section
    globalObj->setSection(sectionName);
    usedSections_.insert(sectionName);
    
    // تسجيل القسم إذا لم يكن مسجلاً / Register section if not already registered
    if (!hasSection(sectionName)) {
        SectionInfo info;
        info.name = sectionName;
        registerSection(info);
    }
    
    return true;
}

// ════════════════════════════════════════════════════════════════════════════════
// أقسام النواة الشائعة / Common Kernel Sections
// ════════════════════════════════════════════════════════════════════════════════

void LLVMCustomSections::registerKernelSections() {
    // قسم Multiboot / Multiboot section
    {
        SectionInfo info;
        info.name = ".multiboot";
        info.arabicName = "إقلاع_متعدد";
        info.isReadOnly = true;
        info.keepSection = true;
        info.alignment = 4;
        registerSection(info);
    }
    
    // قسم Multiboot2
    {
        SectionInfo info;
        info.name = ".multiboot2";
        info.arabicName = "إقلاع_متعدد2";
        info.isReadOnly = true;
        info.keepSection = true;
        info.alignment = 8;
        registerSection(info);
    }
    
    // جدول المقاطعات / Interrupt table
    {
        SectionInfo info;
        info.name = ".interrupt_table";
        info.arabicName = "جدول_المقاطعات";
        info.isReadOnly = false;
        info.keepSection = true;
        info.alignment = 4096;
        registerSection(info);
    }
    
    // جداول الصفحات / Page tables
    {
        SectionInfo info;
        info.name = ".page_tables";
        info.arabicName = "جداول_الصفحات";
        info.isReadOnly = false;
        info.keepSection = true;
        info.alignment = 4096;
        registerSection(info);
    }
    
    // متجه ISR / ISR vector (ARM)
    {
        SectionInfo info;
        info.name = ".isr_vector";
        info.arabicName = "متجه_المقاطعات";
        info.isReadOnly = true;
        info.keepSection = true;
        info.alignment = 256;
        registerSection(info);
    }
    
    // بيانات إقلاع / Boot data
    {
        SectionInfo info;
        info.name = ".boot";
        info.arabicName = "إقلاع";
        info.isReadOnly = true;
        info.isExecutable = true;
        info.keepSection = true;
        registerSection(info);
    }
    
    // بيانات init / Init data
    {
        SectionInfo info;
        info.name = ".init";
        info.arabicName = "تهيئة";
        info.isExecutable = true;
        info.keepSection = true;
        registerSection(info);
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// الاستعلام / Query
// ════════════════════════════════════════════════════════════════════════════════

std::vector<SectionInfo> LLVMCustomSections::getAllSections() const {
    std::vector<SectionInfo> result;
    for (const auto& [name, info] : sections_) {
        (void)name;
        result.push_back(info);
    }
    return result;
}

std::set<std::string> LLVMCustomSections::getUsedSections() const {
    return usedSections_;
}

// ════════════════════════════════════════════════════════════════════════════════
// مساعدات / Helpers
// ════════════════════════════════════════════════════════════════════════════════

std::string LLVMCustomSections::translateSectionName(const std::string& name) const {
    // ترجمة أسماء الأقسام العربية / Translate Arabic section names
    static const std::map<std::string, std::string> translations = {
        {"إقلاع_متعدد", ".multiboot"},
        {"إقلاع_متعدد2", ".multiboot2"},
        {"جدول_المقاطعات", ".interrupt_table"},
        {"جداول_الصفحات", ".page_tables"},
        {"متجه_المقاطعات", ".isr_vector"},
        {"إقلاع", ".boot"},
        {"تهيئة", ".init"},
        {"كود", ".text"},
        {"بيانات", ".data"},
        {"بيانات_ثابتة", ".rodata"},
        {"غير_مهيأة", ".bss"},
        {"مكدس", ".stack"},
        {"كومة", ".heap"},
    };
    
    auto it = translations.find(name);
    if (it != translations.end()) {
        return it->second;
    }
    
    // إذا لم يبدأ بنقطة، أضف نقطة / If doesn't start with dot, add one
    if (!name.empty() && name[0] != '.') {
        return "." + name;
    }
    
    return name;
}

} // namespace LLVM
} // namespace Sad
