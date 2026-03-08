// ============================================================================
// generics_codegen.cpp — توليد كود Generics / Generics Code Generation
// ============================================================================
// @brief (AR) محرك توليد الكود لنظام المعممات (Generics Codegen Engine)
//   يعمل كطبقة وسيطة بين SIR Builder و LLVM CodeGen:
//   - يفحص SIR Module للبحث عن استدعاءات دوال القوالب غير المُنشأة
//   - يستنتج أنواع المعاملات تلقائياً من موقع الاستدعاء
//   - يطلق عملية Monomorphization عبر SIRBuilder
//   - يضمن وجود جميع التخصصات المطلوبة قبل LLVM CodeGen
//
// @brief (EN) Generics code generation engine.
//   Acts as a middle layer between SIR Builder and LLVM CodeGen:
//   - Scans SIR Module for unresolved template function calls
//   - Infers type arguments from call-site argument types
//   - Triggers monomorphization via SIRBuilder
//   - Ensures all needed specializations exist before LLVM CodeGen
//
// Pipeline:
//   AST → SIRBuilder → [GenericInstantiationPass] → SIROptimizer → LLVMCodeGen
// ============================================================================

#include "sir_builder.h"
#include "sir_module.h"
#include "sir_instruction.h"
#include "sir_types.h"
#include "generics.h"
#include "generic_instantiation.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// إحصائيات توليد كود المعممات / Generics Codegen Statistics
// ============================================================================
struct GenericsCodegenStats {
    size_t templateFunctionsFound = 0;      // دوال القوالب المكتشفة
    size_t callSitesScanned = 0;            // مواقع الاستدعاء المفحوصة
    size_t typeInferencesPerformed = 0;      // عمليات استنتاج الأنواع
    size_t instantiationsTriggered = 0;      // عمليات الإنشاء المُطلقة
    size_t instantiationsCached = 0;        // إنشاءات من الذاكرة المخبأة
    size_t inferenceFailures = 0;           // فشل استنتاج الأنواع

    std::string toString() const {
        return "[GenericsCG] القوالب: " + std::to_string(templateFunctionsFound) +
               ", الاستدعاءات: " + std::to_string(callSitesScanned) +
               ", الاستنتاجات: " + std::to_string(typeInferencesPerformed) +
               ", الإنشاءات: " + std::to_string(instantiationsTriggered) +
               " (مخبأة: " + std::to_string(instantiationsCached) + ")" +
               (inferenceFailures > 0 ? ", فشل: " + std::to_string(inferenceFailures) : "");
    }
};

// ============================================================================
// نظام استنتاج الأنواع البسيط / Simple Type Inference System
// ============================================================================
// @brief (AR) يستنتج أنواع معاملات القالب من أنواع وسائط الاستدعاء
// @brief (EN) Infers template type parameters from call-site argument types
class SimpleTypeInferencer {
public:
    // (AR) استنتاج أنواع القالب من موقع الاستدعاء
    // (EN) Infer template types from a call site
    static bool inferTypes(
        const std::vector<std::string>& templateParamNames,
        const std::vector<std::string>& declaredParamTypeNames,
        const std::vector<SIRType>& actualArgTypes,
        std::vector<SIRType>& outInferredTypes)
    {
        outInferredTypes.clear();
        std::unordered_map<std::string, SIRType> inferred;

        // (AR) نمر على كل معامل ومقابله في الوسائط الفعلية
        // (EN) Walk each parameter and its corresponding actual argument
        size_t commonCount = std::min(declaredParamTypeNames.size(), actualArgTypes.size());
        for (size_t i = 0; i < commonCount; ++i) {
            const std::string& paramTypeName = declaredParamTypeNames[i];

            // (AR) إذا كان اسم النوع هو أحد معاملات القالب → استنتج النوع
            for (const auto& tpName : templateParamNames) {
                if (paramTypeName == tpName) {
                    auto it = inferred.find(tpName);
                    if (it == inferred.end()) {
                        inferred[tpName] = actualArgTypes[i];
                    } else {
                        // (AR) أنواع متعارضة لنفس المعامل
                        if (it->second != actualArgTypes[i]) {
                            return false;
                        }
                    }
                    break;
                }
            }
        }

        // (AR) تجميع النتائج بترتيب المعاملات
        for (const auto& tpName : templateParamNames) {
            auto it = inferred.find(tpName);
            if (it != inferred.end()) {
                outInferredTypes.push_back(it->second);
            } else {
                // (AR) لم نستطع استنتاج نوع هذا المعامل — نستخدم I64 كافتراضي
                outInferredTypes.push_back(SIRType::I64);
            }
        }

        return true;
    }

    // (AR) استنتاج نوع SIR من نوع الوسيطة في التعليمة
    // (EN) Infer SIR type from an instruction operand
    static SIRType inferOperandType(const SIROperand& operand) {
        // SIROperandType::CONSTANT — التمييز عبر dataType
        if (operand.type == SIROperandType::CONSTANT) {
            return operand.dataType;
        }
        // REGISTER — النوع محفوظ في dataType أيضاً
        return operand.dataType;
    }
};

// ============================================================================
// تمريرة إنشاء المعممات / Generic Instantiation Pass
// ============================================================================
// @brief (AR) تمريرة SIR تفحص الوحدة وتُنشئ نسخاً محددة من دوال القوالب
// @brief (EN) SIR pass that scans the module and creates concrete template instances
class GenericInstantiationPass {
public:
    GenericInstantiationPass() = default;

    // (AR) تشغيل التمريرة على الوحدة بالكامل
    // (EN) Run the pass on the entire module
    bool run(SIRModule& module, SIRBuilder& builder) {
        stats_ = {};

        // (AR) الخطوة 1: فحص جميع الدوال بحثاً عن استدعاءات قوالب
        // (EN) Step 1: Scan all functions for template call sites
        bool changed = false;

        const auto& functions = module.getFunctions();
        // (AR) نسخ القائمة لأن instantiation يضيف دوالاً جديدة
        std::vector<std::shared_ptr<SIRFunction>> snapshot(functions.begin(), functions.end());

        for (auto& func : snapshot) {
            if (func) {
                changed |= scanFunction(*func, builder);
            }
        }

        return changed;
    }

    const GenericsCodegenStats& getStats() const { return stats_; }

private:
    GenericsCodegenStats stats_;
    std::unordered_set<std::string> instantiatedNames_;   // أسماء النسخ المُنشأة

    // (AR) فحص دالة بحثاً عن استدعاءات قوالب
    bool scanFunction(SIRFunction& func, SIRBuilder& builder) {
        bool changed = false;

        for (auto& block : func.basicBlocks) {
            if (!block) continue;
            for (auto& inst : block->instructions) {
                stats_.callSitesScanned++;

                if (inst.opcode == SIROpcode::CALL || inst.opcode == SIROpcode::CALL_INDIRECT) {
                    // (AR) فحص ما إذا كان الاسم يشير إلى دالة قالب
                    if (!inst.operands.empty()) {
                        const std::string& callTarget = inst.operands[0].name;

                        // (AR) التحقق من نمط الاسم (يحتوي <> دلالة على قالب)
                        if (isTemplateCallPattern(callTarget)) {
                            changed |= processTemplateCall(inst, callTarget, builder);
                        }
                    }
                }
            }
        }

        return changed;
    }

    // (AR) التحقق من أن الاسم يمثل استدعاء قالب
    bool isTemplateCallPattern(const std::string& name) const {
        // نمط: اسم<نوع> (مثل أكبر<رقم>)
        auto ltPos = name.find('<');
        auto gtPos = name.find('>');
        return (ltPos != std::string::npos && gtPos != std::string::npos && gtPos > ltPos);
    }

    // (AR) معالجة استدعاء قالب وإنشاء نسخة محددة
    bool processTemplateCall(SIRInstruction& inst, const std::string& callTarget, SIRBuilder& builder) {
        // (AR) استخراج اسم القالب ومعاملات الأنواع من الاسم
        auto extracted = Sad::TypeSystem::extractTypeParameters(callTarget);
        const std::string& baseName = extracted.first;
        const std::vector<std::string>& typeArgNames = extracted.second;

        if (baseName.empty() || typeArgNames.empty()) {
            return false;
        }

        // (AR) تحويل أسماء الأنواع إلى SIRType
        std::vector<SIRType> typeArgs;
        typeArgs.reserve(typeArgNames.size());
        for (const auto& typeName : typeArgNames) {
            typeArgs.push_back(typeNameToSIRType(typeName));
        }

        // (AR) بناء الاسم المُشوَّه (mangled)
        std::string mangledName = baseName;
        for (const auto& type : typeArgs) {
            mangledName += "_";
            switch (type) {
                case SIRType::I64:    mangledName += "i64"; break;
                case SIRType::F64:    mangledName += "f64"; break;
                case SIRType::STRING: mangledName += "str"; break;
                case SIRType::BOOL:   mangledName += "bool"; break;
                default:              mangledName += "obj"; break;
            }
        }

        // (AR) التحقق من عدم وجود النسخة بالفعل
        if (instantiatedNames_.count(mangledName)) {
            stats_.instantiationsCached++;
            // (AR) تحديث اسم الاستدعاء ليشير للنسخة المُنشأة
            if (!inst.operands.empty()) {
                inst.operands[0].name = mangledName;
            }
            return false;
        }

        // (AR) إنشاء النسخة عبر SIRBuilder
        stats_.typeInferencesPerformed++;
        std::string result = builder.instantiateTemplate(baseName, typeArgs);

        if (!result.empty()) {
            stats_.instantiationsTriggered++;
            instantiatedNames_.insert(result);

            // (AR) تحديث تعليمة الاستدعاء
            if (!inst.operands.empty()) {
                inst.operands[0].name = result;
            }
            return true;
        } else {
            stats_.inferenceFailures++;
            return false;
        }
    }

    // (AR) تحويل اسم النوع العربي إلى SIRType
    SIRType typeNameToSIRType(const std::string& typeName) const {
        if (typeName == "رقم" || typeName == "عدد" || typeName == "int" || typeName == "i64") {
            return SIRType::I64;
        }
        if (typeName == "عشري" || typeName == "float" || typeName == "f64") {
            return SIRType::F64;
        }
        if (typeName == "نص" || typeName == "string" || typeName == "str") {
            return SIRType::STRING;
        }
        if (typeName == "منطقي" || typeName == "bool") {
            return SIRType::BOOL;
        }
        return SIRType::I64;
    }
};

// ============================================================================
// واجهة عامة / Public Interface
// ============================================================================

/// (AR) تشغيل تمريرة إنشاء المعممات على الوحدة
/// (EN) Run the generic instantiation pass on a module
/// يُستدعى من compiler_driver_frontend.cpp بعد بناء SIR وقبل LLVM CodeGen
bool runGenericInstantiationPass(SIRModule& module, SIRBuilder& builder) {
    GenericInstantiationPass pass;
    bool changed = pass.run(module, builder);

    if (changed) {
        const auto& stats = pass.getStats();
        std::cout << stats.toString() << std::endl;
    }

    return changed;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
