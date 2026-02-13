/**
 * @file inline_asm.cpp
 * @brief (AR) توليد LLVM IR لـ inline assembly
 *        (EN) LLVM IR generation for inline assembly
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ██╗     ██╗     ██╗   ██╗███╗   ███╗    █████╗ ███████╗███╗   ███╗       ║
 * ║     ██║     ██║     ██║   ██║████╗ ████║   ██╔══██╗██╔════╝████╗ ████║       ║
 * ║     ██║     ██║     ██║   ██║██╔████╔██║   ███████║███████╗██╔████╔██║       ║
 * ║     ██║     ██║     ╚██╗ ██╔╝██║╚██╔╝██║   ██╔══██║╚════██║██║╚██╔╝██║       ║
 * ║     ███████╗███████╗ ╚████╔╝ ██║ ╚═╝ ██║   ██║  ██║███████║██║ ╚═╝ ██║       ║
 * ║     ╚══════╝╚══════╝  ╚═══╝  ╚═╝     ╚═╝   ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝       ║
 * ║                                                                               ║
 * ║                     توليد كود LLVM لـ inline assembly                         ║
 * ║                     لغة ص للتحكم المباشر بالأجهزة                             ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يولّد كود LLVM IR من عبارات تجميع!:
 * 
 *      مثال على التحويل:
 *      @code
 *      // كود ص:
 *      تجميع!("mov $0, $1"
 *          : "=r"(نتيجة)
 *          : "r"(قيمة)
 *      )؛
 *      
 *      // LLVM IR الناتج:
 *      %result = call i64 asm "mov $0, $1", "=r,r"(i64 %value)
 *      @endcode
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <sstream>

namespace sad {
namespace compiler {
namespace backend {

// (AR) تضمين تعريفات المحلل
// (EN) Include parser definitions
// Forward declarations from asm_parser.cpp
namespace parser {
    struct AsmOperand;
    struct InlineAsmStatement;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) معلومات النوع لـ inline assembly
//                    (EN) Type information for inline assembly
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct AsmTypeInfo
 * @brief (AR) معلومات النوع للمعاملات
 *        (EN) Type information for operands
 */
struct AsmTypeInfo {
    std::string llvmType;       // (AR) نوع LLVM (مثل i32, i64, ptr)
    int bitWidth;               // (AR) عرض البتات
    bool isPointer;             // (AR) هل هو مؤشر؟
    bool isFloat;               // (AR) هل هو فاصلة عائمة؟
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد كود inline assembly
//                    (EN) Inline Assembly Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InlineAsmCodeGenerator
 * @brief (AR) مولّد LLVM IR لـ inline assembly
 *        (EN) LLVM IR generator for inline assembly
 */
class InlineAsmCodeGenerator {
public:
    InlineAsmCodeGenerator() = default;
    
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        targetArch_ = arch;
    }
    
    /**
     * @brief (AR) توليد LLVM IR لعبارة inline assembly
     *        (EN) Generate LLVM IR for inline assembly statement
     * 
     * @param asmTemplate (AR) قالب التجميع (EN) Assembly template
     * @param outputs (AR) المخرجات (EN) Outputs
     * @param inputs (AR) المدخلات (EN) Inputs
     * @param clobbers (AR) المُتلفات (EN) Clobbers
     * @param isVolatile (AR) متطاير؟ (EN) Volatile?
     * @param hasSideEffects (AR) له تأثيرات جانبية؟ (EN) Has side effects?
     * @return (AR) كود LLVM IR (EN) LLVM IR code
     */
    std::string generateInlineAsm(
        const std::string& asmTemplate,
        const std::vector<std::pair<std::string, AsmTypeInfo>>& outputs,
        const std::vector<std::pair<std::string, std::pair<AsmTypeInfo, std::string>>>& inputs,
        const std::vector<std::string>& clobbers,
        bool isVolatile,
        bool hasSideEffects
    ) {
        std::ostringstream ir;
        
        ir << "  ; " << u8"inline assembly" << "\n";
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) بناء سلسلة القيود
        // (EN) Build constraint string
        // ─────────────────────────────────────────────────────────────────────
        
        std::string constraints = buildConstraintString(outputs, inputs, clobbers);
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) تحديد نوع الإرجاع
        // (EN) Determine return type
        // ─────────────────────────────────────────────────────────────────────
        
        std::string returnType = buildReturnType(outputs);
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) بناء قائمة المدخلات
        // (EN) Build input list
        // ─────────────────────────────────────────────────────────────────────
        
        std::string inputList = buildInputList(inputs);
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) بناء سمات asm
        // (EN) Build asm attributes
        // ─────────────────────────────────────────────────────────────────────
        
        std::string asmAttrs;
        if (hasSideEffects) {
            asmAttrs = "sideeffect";
        }
        if (isVolatile) {
            if (!asmAttrs.empty()) asmAttrs += ", ";
            asmAttrs += "volatile";
        }
        
        // ─────────────────────────────────────────────────────────────────────
        // (AR) توليد التعليمة
        // (EN) Generate instruction
        // ─────────────────────────────────────────────────────────────────────
        
        if (outputs.empty()) {
            // (AR) بدون مخرجات - استخدام call void
            ir << "  call void asm ";
            if (!asmAttrs.empty()) {
                ir << asmAttrs << " ";
            }
            ir << "\"" << escapeAsmString(asmTemplate) << "\", ";
            ir << "\"" << constraints << "\"";
            ir << "(" << inputList << ")";
        } else if (outputs.size() == 1) {
            // (AR) مخرج واحد
            ir << "  %" << outputs[0].first << " = call " << returnType << " asm ";
            if (!asmAttrs.empty()) {
                ir << asmAttrs << " ";
            }
            ir << "\"" << escapeAsmString(asmTemplate) << "\", ";
            ir << "\"" << constraints << "\"";
            ir << "(" << inputList << ")";
        } else {
            // (AR) مخرجات متعددة - استخدام بنية
            ir << "  %" << "asm_result" << " = call " << returnType << " asm ";
            if (!asmAttrs.empty()) {
                ir << asmAttrs << " ";
            }
            ir << "\"" << escapeAsmString(asmTemplate) << "\", ";
            ir << "\"" << constraints << "\"";
            ir << "(" << inputList << ")";
            ir << "\n";
            
            // (AR) استخراج القيم من البنية
            for (size_t i = 0; i < outputs.size(); i++) {
                ir << "  %" << outputs[i].first << " = extractvalue " << returnType;
                ir << " %asm_result, " << i << "\n";
            }
        }
        
        ir << "\n";
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد تعليمة خاصة بالمعمارية
     *        (EN) Generate architecture-specific instruction
     */
    std::string generateArchSpecificAsm(
        const std::string& instruction,
        const std::vector<std::string>& operands
    ) {
        if (isARM()) {
            return generateARMAsm(instruction, operands);
        } else {
            return generateX86Asm(instruction, operands);
        }
    }
    
private:
    std::string targetArch_ = "x86_64";
    
    bool isARM() const {
        return targetArch_.find("arm") != std::string::npos ||
               targetArch_.find("thumb") != std::string::npos ||
               targetArch_.find("aarch") != std::string::npos;
    }
    
    /**
     * @brief (AR) بناء سلسلة القيود
     *        (EN) Build constraint string
     */
    std::string buildConstraintString(
        const std::vector<std::pair<std::string, AsmTypeInfo>>& outputs,
        const std::vector<std::pair<std::string, std::pair<AsmTypeInfo, std::string>>>& inputs,
        const std::vector<std::string>& clobbers
    ) {
        std::ostringstream constraints;
        
        // (AR) قيود المخرجات
        for (size_t i = 0; i < outputs.size(); i++) {
            if (i > 0) constraints << ",";
            constraints << "=r";  // (AR) افتراضياً سجل للمخرج
        }
        
        // (AR) قيود المدخلات
        for (size_t i = 0; i < inputs.size(); i++) {
            if (!outputs.empty() || i > 0) constraints << ",";
            constraints << inputs[i].second.second;  // (AR) القيد المحدد
        }
        
        // (AR) المُتلفات
        for (const auto& clobber : clobbers) {
            if (!outputs.empty() || !inputs.empty() || &clobber != &clobbers[0]) {
                constraints << ",";
            }
            constraints << "~{" << clobber << "}";
        }
        
        return constraints.str();
    }
    
    /**
     * @brief (AR) بناء نوع الإرجاع
     *        (EN) Build return type
     */
    std::string buildReturnType(
        const std::vector<std::pair<std::string, AsmTypeInfo>>& outputs
    ) {
        if (outputs.empty()) {
            return "void";
        } else if (outputs.size() == 1) {
            return outputs[0].second.llvmType;
        } else {
            // (AR) بنية للمخرجات المتعددة
            std::ostringstream type;
            type << "{";
            for (size_t i = 0; i < outputs.size(); i++) {
                if (i > 0) type << ", ";
                type << outputs[i].second.llvmType;
            }
            type << "}";
            return type.str();
        }
    }
    
    /**
     * @brief (AR) بناء قائمة المدخلات
     *        (EN) Build input list
     */
    std::string buildInputList(
        const std::vector<std::pair<std::string, std::pair<AsmTypeInfo, std::string>>>& inputs
    ) {
        std::ostringstream list;
        
        for (size_t i = 0; i < inputs.size(); i++) {
            if (i > 0) list << ", ";
            list << inputs[i].second.first.llvmType << " %" << inputs[i].first;
        }
        
        return list.str();
    }
    
    /**
     * @brief (AR) هروب محارف خاصة في سلسلة asm
     *        (EN) Escape special characters in asm string
     */
    std::string escapeAsmString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '"': result += "\\\""; break;
                case '\n': result += "\\n"; break;
                case '\t': result += "\\t"; break;
                case '\r': result += "\\r"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    /**
     * @brief (AR) توليد كود ARM assembly
     *        (EN) Generate ARM assembly code
     */
    std::string generateARMAsm(
        const std::string& instruction,
        const std::vector<std::string>& operands
    ) {
        std::ostringstream ir;
        
        // (AR) تعليمات ARM الشائعة
        if (instruction == "wfi" || instruction == u8"انتظر_مقاطعة") {
            ir << "  call void asm sideeffect \"wfi\", \"\"()\n";
        }
        else if (instruction == "dsb" || instruction == u8"حاجز_بيانات") {
            ir << "  call void asm sideeffect \"dsb\", \"~{memory}\"()\n";
        }
        else if (instruction == "dmb" || instruction == u8"حاجز_ذاكرة") {
            ir << "  call void asm sideeffect \"dmb\", \"~{memory}\"()\n";
        }
        else if (instruction == "isb" || instruction == u8"حاجز_تعليمات") {
            ir << "  call void asm sideeffect \"isb\", \"~{memory}\"()\n";
        }
        else if (instruction == "cpsid" || instruction == u8"عطّل_المقاطعات") {
            ir << "  call void asm sideeffect \"cpsid i\", \"~{memory}\"()\n";
        }
        else if (instruction == "cpsie" || instruction == u8"فعّل_المقاطعات") {
            ir << "  call void asm sideeffect \"cpsie i\", \"~{memory}\"()\n";
        }
        
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود x86 assembly
     *        (EN) Generate x86 assembly code
     */
    std::string generateX86Asm(
        const std::string& instruction,
        const std::vector<std::string>& operands
    ) {
        std::ostringstream ir;
        
        // (AR) تعليمات x86 الشائعة
        if (instruction == "hlt" || instruction == u8"توقف") {
            ir << "  call void asm sideeffect \"hlt\", \"\"()\n";
        }
        else if (instruction == "cli" || instruction == u8"عطّل_المقاطعات") {
            ir << "  call void asm sideeffect \"cli\", \"~{flags}\"()\n";
        }
        else if (instruction == "sti" || instruction == u8"فعّل_المقاطعات") {
            ir << "  call void asm sideeffect \"sti\", \"~{flags}\"()\n";
        }
        else if (instruction == "nop" || instruction == u8"لا_شيء") {
            ir << "  call void asm \"nop\", \"\"()\n";
        }
        else if (instruction == "pause" || instruction == u8"توقف_مؤقت") {
            ir << "  call void asm \"pause\", \"~{memory}\"()\n";
        }
        else if (instruction == "mfence" || instruction == u8"حاجز_ذاكرة") {
            ir << "  call void asm sideeffect \"mfence\", \"~{memory}\"()\n";
        }
        else if (instruction == "lfence" || instruction == u8"حاجز_تحميل") {
            ir << "  call void asm sideeffect \"lfence\", \"~{memory}\"()\n";
        }
        else if (instruction == "sfence" || instruction == u8"حاجز_تخزين") {
            ir << "  call void asm sideeffect \"sfence\", \"~{memory}\"()\n";
        }
        else if (instruction == "cpuid" || instruction == u8"معرف_المعالج") {
            // (AR) cpuid يُتلف eax, ebx, ecx, edx
            ir << "  call void asm sideeffect \"cpuid\", \"~{eax},~{ebx},~{ecx},~{edx}\"()\n";
        }
        else if (instruction == "rdtsc" || instruction == u8"اقرأ_عداد_الدورات") {
            ir << "  %tsc = call i64 asm \"rdtsc\", \"=A,~{edx}\"()\n";
        }
        
        return ir.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) دوال مساعدة للتعليمات الشائعة
//                    (EN) Helper Functions for Common Instructions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InlineAsmHelpers
 * @brief (AR) دوال مساعدة لتعليمات inline assembly الشائعة
 *        (EN) Helper functions for common inline assembly instructions
 */
class InlineAsmHelpers {
public:
    /**
     * @brief (AR) توليد كود قراءة من منفذ I/O
     *        (EN) Generate I/O port read code
     */
    static std::string generateInb(const std::string& resultVar, const std::string& portVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"قراءة بايت من منفذ I/O" << "\n";
        ir << "  %" << resultVar << " = call i8 asm \"inb %dx, %al\", ";
        ir << "\"=&{al},{dx}\"(i16 %" << portVar << ")\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود كتابة إلى منفذ I/O
     *        (EN) Generate I/O port write code
     */
    static std::string generateOutb(const std::string& valueVar, const std::string& portVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"كتابة بايت إلى منفذ I/O" << "\n";
        ir << "  call void asm sideeffect \"outb %al, %dx\", ";
        ir << "\"{al},{dx}\"(i8 %" << valueVar << ", i16 %" << portVar << ")\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود قراءة سجل CR
     *        (EN) Generate CR register read code
     */
    static std::string generateReadCR(int crNum, const std::string& resultVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"قراءة سجل CR" << crNum << "\n";
        ir << "  %" << resultVar << " = call i64 asm \"mov %cr" << crNum << ", $0\", ";
        ir << "\"=r\"()\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود كتابة إلى سجل CR
     *        (EN) Generate CR register write code
     */
    static std::string generateWriteCR(int crNum, const std::string& valueVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"كتابة إلى سجل CR" << crNum << "\n";
        ir << "  call void asm sideeffect \"mov $0, %cr" << crNum << "\", ";
        ir << "\"r\"(i64 %" << valueVar << ")\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود تبادل ذري
     *        (EN) Generate atomic exchange code
     */
    static std::string generateAtomicXchg(
        const std::string& resultVar,
        const std::string& memVar,
        const std::string& newValueVar
    ) {
        std::ostringstream ir;
        ir << "  ; " << u8"تبادل ذري (atomic xchg)" << "\n";
        ir << "  %" << resultVar << " = call i64 asm sideeffect \"lock xchg $0, $1\", ";
        ir << "\"=r,=*m,0\"(ptr %" << memVar << ", i64 %" << newValueVar << ")\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود مقارنة وتبديل ذري
     *        (EN) Generate atomic compare-and-swap code
     */
    static std::string generateAtomicCmpXchg(
        const std::string& successVar,
        const std::string& oldValueVar,
        const std::string& memVar,
        const std::string& expectedVar,
        const std::string& newValueVar
    ) {
        std::ostringstream ir;
        ir << "  ; " << u8"مقارنة وتبديل ذري (atomic cmpxchg)" << "\n";
        ir << "  %cmpxchg_result = call {i64, i1} asm sideeffect ";
        ir << "\"lock cmpxchg $2, $1\", ";
        ir << "\"=&{ax},=*m,r,0\"(ptr %" << memVar << ", i64 %" << newValueVar;
        ir << ", i64 %" << expectedVar << ")\n";
        ir << "  %" << oldValueVar << " = extractvalue {i64, i1} %cmpxchg_result, 0\n";
        ir << "  %" << successVar << " = extractvalue {i64, i1} %cmpxchg_result, 1\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود تحميل جدول الصفحات
     *        (EN) Generate page table load code
     */
    static std::string generateLoadPageTable(const std::string& addressVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"تحميل جدول الصفحات (mov to CR3)" << "\n";
        ir << "  call void asm sideeffect \"mov $0, %cr3\", ";
        ir << "\"r,~{memory}\"(i64 %" << addressVar << ")\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود إبطال TLB
     *        (EN) Generate TLB invalidation code
     */
    static std::string generateInvalidatePage(const std::string& addressVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"إبطال صفحة في TLB (invlpg)" << "\n";
        ir << "  call void asm sideeffect \"invlpg ($0)\", ";
        ir << "\"r,~{memory}\"(ptr %" << addressVar << ")\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود عملية ARM DSB
     *        (EN) Generate ARM DSB operation code
     */
    static std::string generateARMDataBarrier() {
        return "  call void asm sideeffect \"dsb sy\", \"~{memory}\"()\n";
    }
    
    /**
     * @brief (AR) توليد كود قراءة سجل ARM CPSR
     *        (EN) Generate ARM CPSR read code
     */
    static std::string generateARMReadCPSR(const std::string& resultVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"قراءة سجل CPSR" << "\n";
        ir << "  %" << resultVar << " = call i32 asm \"mrs $0, cpsr\", \"=r\"()\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) توليد كود كتابة إلى سجل ARM CPSR
     *        (EN) Generate ARM CPSR write code
     */
    static std::string generateARMWriteCPSR(const std::string& valueVar) {
        std::ostringstream ir;
        ir << "  ; " << u8"كتابة إلى سجل CPSR" << "\n";
        ir << "  call void asm sideeffect \"msr cpsr, $0\", \"r\"(i32 %" << valueVar << ")\n";
        return ir.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير inline assembly
//                    (EN) Inline Assembly Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class InlineAsmManager
 * @brief (AR) مدير inline assembly في المترجم
 *        (EN) Inline assembly manager in the compiler
 */
class InlineAsmManager {
public:
    InlineAsmManager() = default;
    
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        codeGen_.setTargetArch(arch);
        targetArch_ = arch;
    }
    
    /**
     * @brief (AR) توليد كود من سلسلة نصية
     *        (EN) Generate code from string
     */
    std::string generateFromString(const std::string& asmString) {
        // (AR) التحليل وتوليد الكود
        // (EN) Parse and generate code
        
        // (AR) إذا كانت تعليمة بسيطة
        if (isSimpleInstruction(asmString)) {
            return codeGen_.generateArchSpecificAsm(asmString, {});
        }
        
        // (AR) تعليمة معقدة - استخدام القيود الافتراضية
        std::ostringstream ir;
        ir << "  call void asm sideeffect \"" << asmString << "\", \"\"()\n";
        return ir.str();
    }
    
    /**
     * @brief (AR) الحصول على قائمة التعليمات المدعومة
     *        (EN) Get list of supported instructions
     */
    std::map<std::string, std::string> getSupportedInstructions() const {
        if (targetArch_.find("arm") != std::string::npos ||
            targetArch_.find("aarch") != std::string::npos) {
            return {
                {u8"انتظر_مقاطعة", "wfi - Wait For Interrupt"},
                {u8"حاجز_بيانات", "dsb - Data Synchronization Barrier"},
                {u8"حاجز_ذاكرة", "dmb - Data Memory Barrier"},
                {u8"حاجز_تعليمات", "isb - Instruction Synchronization Barrier"},
                {u8"عطّل_المقاطعات", "cpsid - Disable Interrupts"},
                {u8"فعّل_المقاطعات", "cpsie - Enable Interrupts"},
            };
        } else {
            return {
                {u8"توقف", "hlt - Halt"},
                {u8"عطّل_المقاطعات", "cli - Clear Interrupt Flag"},
                {u8"فعّل_المقاطعات", "sti - Set Interrupt Flag"},
                {u8"لا_شيء", "nop - No Operation"},
                {u8"توقف_مؤقت", "pause - Spin Loop Hint"},
                {u8"حاجز_ذاكرة", "mfence - Memory Fence"},
                {u8"حاجز_تحميل", "lfence - Load Fence"},
                {u8"حاجز_تخزين", "sfence - Store Fence"},
                {u8"معرف_المعالج", "cpuid - CPU Identification"},
                {u8"اقرأ_عداد_الدورات", "rdtsc - Read Time-Stamp Counter"},
            };
        }
    }
    
private:
    InlineAsmCodeGenerator codeGen_;
    std::string targetArch_ = "x86_64";
    
    bool isSimpleInstruction(const std::string& instr) const {
        static const std::set<std::string> simple = {
            "hlt", u8"توقف",
            "cli", u8"عطّل_المقاطعات",
            "sti", u8"فعّل_المقاطعات",
            "nop", u8"لا_شيء",
            "pause", u8"توقف_مؤقت",
            "mfence", u8"حاجز_ذاكرة",
            "lfence", u8"حاجز_تحميل",
            "sfence", u8"حاجز_تخزين",
            "cpuid", u8"معرف_المعالج",
            "rdtsc", u8"اقرأ_عداد_الدورات",
            "wfi", u8"انتظر_مقاطعة",
            "dsb", u8"حاجز_بيانات",
            "dmb", u8"حاجز_ذاكرة",
            "isb", u8"حاجز_تعليمات",
            "cpsid", u8"عطّل_المقاطعات",
            "cpsie", u8"فعّل_المقاطعات",
        };
        
        return simple.count(instr) > 0;
    }
};

} // namespace backend
} // namespace compiler
} // namespace sad
