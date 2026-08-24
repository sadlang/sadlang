// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "builders/builtin_arity_check.h"
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>

#include "builtin_registry.h"
#include "builtin_categories.h" // (AR) kBitwiseShiftCountMask المشترك بين المحرّكين (ت-1) / (EN) cross-engine shift-count mask (T-1)
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (د-2) / (EN) catalog bilingual messages (D-2)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
namespace Bn = Sad::Builtins::Names;
namespace Ar = Sad::Builtins::Arity;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // sir_builder_builtins_system.cpp
            // (AR) الدوال المدمجة للنظام: الأجهزة، المنافذ، المؤقتات، الذرية، التوازي، الأمان
            // (EN) System builtins: hardware, ports, timers, atomic ops, async, security
            // ============================================================================

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinCallSystem(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // (AR) إذا كانت الدالة معرّفة من المستخدم، لا تعامل كدالة مضمنة
                // (EN) If function is user-defined, skip all builtins
                if (isUserDefinedFunction)
                    return std::nullopt;

                if (funcName == Bn::KernelCpu::CPU_9)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_9, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE);
                    inst.operands.push_back(argOperands[0]); // (AR) رقم المنفذ / (EN) port number
                    inst.operands.push_back(argOperands[1]); // (AR) القيمة المكتوبة / (EN) value to write
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin اكتب_منفذ()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // (AR) اكتب_منفذ16 / اكتب_منفذ32
                if (funcName == Bn::KernelCpu::CPU_11)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_11, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE_16);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::KernelCpu::CPU_13)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_13, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE_32);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) اقرأ_منفذ(منفذ) — قراءة بايت من منفذ I/O
                // (EN) port_read(port) — read byte from I/O port (inb)
                // ──────────────────────────────────────────────
                if (funcName == Bn::KernelCpu::CPU_8)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_8, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_READ);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin اقرأ_منفذ() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) اقرأ_منفذ16 / اقرأ_منفذ32
                if (funcName == Bn::KernelCpu::CPU_10)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_READ_16);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }
                if (funcName == Bn::KernelCpu::CPU_12)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_READ_32);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ──────────────────────────────────────────────
                // (AR) المدمجات البتّيّة (نطاق KernelCpu، أسماء اللهجة بعد rfcs#43):
                //      وافق/ضمّ/خالف ثنائيّة، اعكس_البتّات أحاديّة، أزح_يسارًا/أزح_يمينًا إزاحة.
                //      القرار المعماريّ: تعمل على i64 (أعداد ص الصحيحة) في المحرّكين،
                //      وعدّاد الإزاحة يُقنَّع بـ&63 (دلالة عتاد x86-64) لتحديد سلوك
                //      العدّاد السالب/الكبير حتميًّا بلا UB — يطابق المفسّر بعد توحيده.
                //      الإزاحة اليمنى حسابيّة (SAR/AShr) حفظًا للإشارة كالمفسّر.
                // (EN) Bitwise builtins (KernelCpu scope, dialect names per rfcs#43).
                //      Architectural decision: i64 operands in both engines; shift count
                //      masked with &63 (x86-64 hardware semantics) so negative/oversized
                //      counts are deterministic and UB-free — mirrors the interpreter.
                //      Right shift is arithmetic (sign-preserving), like the interpreter.
                // ──────────────────────────────────────────────
                {
                    // (AR) قناع عدّاد الإزاحة المشترك بين المحرّكين (ت-1) — انظر
                    //      shared/builtins/include/builtin_categories.h
                    // (EN) Cross-engine shared shift-count mask (T-1) — see
                    //      shared/builtins/include/builtin_categories.h
                    constexpr int64_t kShiftCountMask = Sad::Builtins::kBitwiseShiftCountMask;

                    // (AR) نقطة التحويل الواحدة (إصلاح تكافؤ ARM/المسارات المتناثرة):
                    //      كلّ معامل بتّيّ ليس صحيحًا/منطقيًّا سكونيًّا (Float أو
                    //      مجهول/ديناميّ) يمرّ عبر F64_TO_I64_SAT أوّلًا — الدلالة
                    //      المُشبَعة الموحَّدة منصّيًّا (llvm.fptosi.sat) مهما كان أصل
                    //      المعامل (ثابت/متغيّر/SadDyn/معامل دالة منمَّط عشريًّا يصل
                    //      i64-بتّات خامًا). الصحيح السكونيّ يمرّ مباشرة.
                    // (EN) Single conversion point (ARM/scattered-paths parity fix):
                    //      every bitwise operand not statically Integer/Boolean
                    //      (Float or unknown/dynamic) goes through F64_TO_I64_SAT
                    //      first — the platform-independent saturating semantics
                    //      (llvm.fptosi.sat) regardless of operand origin (constant,
                    //      variable, SadDyn, or a double-typed function parameter
                    //      arriving as raw i64 bits). Statically-Integer passes through.
                    auto satNormalize = [&](size_t idx) -> SIROperand
                    {
                        const SadTypeKind k = argResults[idx].type;
                        if (k == SadTypeKind::Integer || k == SadTypeKind::Boolean)
                            return argOperands[idx];
                        std::string satReg = b_.newTempRegister();
                        SIRInstruction satInst(SIROpcode::F64_TO_I64_SAT);
                        satInst.result = SIROperand::Register(satReg, SadTypeKind::Integer);
                        // (AR) نمرّر المعامل بنوعه الأصليّ — الخلفيّة تفكّ بحسب ما تراه
                        //      (double⇒sat، i64 موسوم Float⇒bitcast ثم sat، SadDyn⇒فكّ
                        //      بالوسم، i64 مجهول⇒تمرير).
                        // (EN) Operand keeps its original type — the backend dispatches
                        //      on what it sees (double⇒sat, Float-typed i64⇒bitcast then
                        //      sat, SadDyn⇒tag-unpack, unknown i64⇒passthrough).
                        satInst.operands.push_back(argOperands[idx]);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(satInst);
                        return SIROperand::Register(satReg, SadTypeKind::Integer);
                    };

                    // (AR) حارس النوع العدديّ (د-2، توافق المحرّكين — نمط حارس زاوج
                    //      SEM002 في builtins_strings_arrays.cpp): معاملٌ معلومُ النوع
                    //      سكونيًّا وليس عدديًّا (منطقيّ/نصّ/مصفوفة/خريطة/صفّ/عدم/فراغ/
                    //      حرف/صنف/بنية/تعداد/دالّة/إغلاق/شريحة) ⇒ SEM002.
                    //      كان النصّ يُحوَّل ptrtoint (بتّات مؤشّر!) والمصفوفة تمرّ فراغًا —
                    //      كود خاطئ صامت. المفسّر يرفض النظير زمنيًّا بـRUN053. عند الجهل
                    //      السكونيّ (Any/Unknown/سجلّ عابر) يمرّ المعامل بلا وسم زمنيّ
                    //      ممكن — كحال حارس زاوج تمامًا: خانات القيم i64 خام بلا وسم
                    //      نوعيّ فلا حارس زمنيّ (دَين موثَّق د-3).
                    // (EN) Numeric-type guard (D-2, engine parity — the زاوج SEM002
                    //      guard pattern from builtins_strings_arrays.cpp): a statically
                    //      known non-numeric operand (bool/string/array/map/tuple/null/
                    //      void/char/class/struct/enum/function/closure/slice) ⇒
                    //      SEM002. Strings used to be ptrtoint'ed (pointer bits!) and
                    //      arrays passed through as garbage — silent wrong results. The
                    //      interpreter rejects the mirror case at runtime with RUN053.
                    //      A statically unknown type (Any/Unknown) passes — exactly like
                    //      the زاوج guard: value slots are raw untagged i64, so no
                    //      runtime guard is possible (documented debt, D-3).
                    auto rejectNonNumericArgs = [&](size_t count) -> bool
                    {
                        for (size_t bi = 0; bi < count && bi < argResults.size(); ++bi)
                        {
                            const SadTypeKind bk = argResults[bi].type;
                            const bool knownNonNumeric =
                                bk == SadTypeKind::Boolean || bk == SadTypeKind::String ||
                                bk == SadTypeKind::Array || bk == SadTypeKind::Map ||
                                bk == SadTypeKind::Tuple || bk == SadTypeKind::Null ||
                                bk == SadTypeKind::Void || bk == SadTypeKind::Char ||
                                bk == SadTypeKind::Class || bk == SadTypeKind::Struct ||
                                bk == SadTypeKind::Enum || bk == SadTypeKind::Function ||
                                bk == SadTypeKind::Closure || bk == SadTypeKind::Slice;
                            if (knownNonNumeric)
                            {
                                Sad::Errors::RenderContext ectx;
                                ectx.placeholders = {{"expected", sirTypeToString(SadTypeKind::Integer)},
                                                     {"found", sirTypeToString(bk)}};
                                b_.errors_.push_back(
                                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                        Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH, ectx));
                                return true;
                            }
                        }
                        return false;
                    };

                    // (AR) مساعد: إصدار عمليّة ثنائيّة بتّيّة وإرجاع سجلّ النتيجة
                    // (EN) Helper: emit a binary bitwise op and return the result register
                    auto emitBitBinary = [&](SIROpcode op, const SIROperand &lhs,
                                             const SIROperand &rhs) -> BuildResult
                    {
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction inst(op);
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                        inst.operands.push_back(lhs);
                        inst.operands.push_back(rhs);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(resultReg, SadTypeKind::Integer);
                    };

                    // (AR) مساعد: تقنيع عدّاد الإزاحة بـ&63 ثم إصدار الإزاحة
                    // (EN) Helper: mask the shift count with &63, then emit the shift
                    auto emitShift = [&](SIROpcode shiftOp) -> BuildResult
                    {
                        BuildResult masked = emitBitBinary(
                            SIROpcode::AND, satNormalize(1), SIROperand::ConstantI64(kShiftCountMask));
                        return emitBitBinary(
                            shiftOp, satNormalize(0),
                            SIROperand::Register(masked.registerName, SadTypeKind::Integer));
                    };

                    // وافق(أ، ب) — AND بتّيّ
                    if (funcName == Bn::KernelCpu::CPU_14)
                    {
                        if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_14, argResults.size()))
                            return BuildResult("", SadTypeKind::Integer);
                        if (rejectNonNumericArgs(2))
                            return BuildResult("", SadTypeKind::Integer);
                        return emitBitBinary(SIROpcode::AND, satNormalize(0), satNormalize(1));
                    }
                    // ضمّ(أ، ب) — OR بتّيّ
                    if (funcName == Bn::KernelCpu::CPU_15)
                    {
                        if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_15, argResults.size()))
                            return BuildResult("", SadTypeKind::Integer);
                        if (rejectNonNumericArgs(2))
                            return BuildResult("", SadTypeKind::Integer);
                        return emitBitBinary(SIROpcode::OR, satNormalize(0), satNormalize(1));
                    }
                    // خالف(أ، ب) — XOR بتّيّ
                    if (funcName == Bn::KernelCpu::CPU_16)
                    {
                        if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_16, argResults.size()))
                            return BuildResult("", SadTypeKind::Integer);
                        if (rejectNonNumericArgs(2))
                            return BuildResult("", SadTypeKind::Integer);
                        return emitBitBinary(SIROpcode::XOR, satNormalize(0), satNormalize(1));
                    }
                    // اعكس_البتّات(أ) — NOT بتّيّ (نتيجته Integer لا Boolean كي لا
                    // يسلك emitNot مسار النفي المنطقيّ)
                    if (funcName == Bn::KernelCpu::CPU_17)
                    {
                        if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_17, argResults.size()))
                            return BuildResult("", SadTypeKind::Integer);
                        if (rejectNonNumericArgs(1))
                            return BuildResult("", SadTypeKind::Integer);
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction inst(SIROpcode::NOT);
                        inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                        inst.operands.push_back(satNormalize(0));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(resultReg, SadTypeKind::Integer);
                    }
                    // أزح_يسارًا(أ، عدّاد) — SHL بعدّاد مُقنَّع
                    if (funcName == Bn::KernelCpu::CPU_18)
                    {
                        if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_18, argResults.size()))
                            return BuildResult("", SadTypeKind::Integer);
                        if (rejectNonNumericArgs(2))
                            return BuildResult("", SadTypeKind::Integer);
                        return emitShift(SIROpcode::SHL);
                    }
                    // أزح_يمينًا(أ، عدّاد) — إزاحة يمنى حسابيّة بعدّاد مُقنَّع
                    if (funcName == Bn::KernelCpu::CPU_19)
                    {
                        if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_19, argResults.size()))
                            return BuildResult("", SadTypeKind::Integer);
                        if (rejectNonNumericArgs(2))
                            return BuildResult("", SadTypeKind::Integer);
                        return emitShift(SIROpcode::SHR);
                    }
                }

                // ──────────────────────────────────────────────
                // (AR) اكتب_ذاكرة(عنوان، قيمة) — كتابة بايت في عنوان ذاكرة محدد
                // (EN) mem_write(address, value) — write byte to memory address (poke)
                // (AR) يستخدم في: الكتابة على ذاكرة VGA (0xB8000)، جداول المقاطعات
                // ──────────────────────────────────────────────
                if (funcName == Bn::KernelCpu::CPU_25)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_25, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_8);
                    inst.operands.push_back(argOperands[0]); // (AR) عنوان الذاكرة / (EN) memory address
                    inst.operands.push_back(argOperands[1]); // (AR) القيمة / (EN) value
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin اكتب_ذاكرة()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // (AR) اكتب_ذاكرة16 / اكتب_ذاكرة32 / اكتب_ذاكرة64
                if (funcName == Bn::CompilerMem::MEM_0)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerMem::MEM_0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_16);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerMem::MEM_1)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerMem::MEM_1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_32);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerMem::MEM_2)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerMem::MEM_2, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_64);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) اقرأ_ذاكرة(عنوان) — قراءة بايت من عنوان ذاكرة محدد
                // (EN) mem_read(address) — read byte from memory address (peek)
                // ──────────────────────────────────────────────
                if (funcName == Bn::KernelCpu::CPU_24)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::KernelCpu::CPU_24, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_READ_8);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin اقرأ_ذاكرة() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) اقرأ_ذاكرة16 / اقرأ_ذاكرة32 / اقرأ_ذاكرة64
                if (funcName == Bn::CompilerMem::MEM_3)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_READ_16);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerMem::MEM_4)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_READ_32);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerMem::MEM_5)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_READ_64);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ──────────────────────────────────────────────
                // (AR) مقاطعة(رقم) — إطلاق مقاطعة برمجية
                // (EN) interrupt(number) — trigger software interrupt (int N)
                // (AR) مثال: مقاطعة(0x80) لاستدعاء نظام Linux
                // ──────────────────────────────────────────────
                if (funcName == Bn::Kernel::INTERRUPT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Kernel::INTERRUPT, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_INTERRUPT);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin مقاطعة()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) أسبت_المعالج() — إيقاف المعالج حتى المقاطعة التالية
                // (EN) halt_cpu() — halt CPU until next interrupt (hlt instruction)
                // (AR) يستخدم في: حلقة الخمول الرئيسية للنواة
                // ──────────────────────────────────────────────
                if (funcName == Bn::Kernel::HALT)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_HALT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin أسبت_المعالج()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) تعطيل_مقاطعات() — تعطيل جميع المقاطعات (cli)
                // (EN) disable_interrupts() — disable all interrupts (cli instruction)
                // (AR) ضروري عند تعديل جداول المقاطعات أو البيانات الحرجة
                // ──────────────────────────────────────────────
                if (funcName == Bn::KernelCpu::CPU_3)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_CLI);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin تعطيل_مقاطعات()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) تفعيل_مقاطعات() — تفعيل جميع المقاطعات (sti)
                // (EN) enable_interrupts() — enable all interrupts (sti instruction)
                // ──────────────────────────────────────────────
                if (funcName == Bn::KernelCpu::CPU_4)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_STI);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin تفعيل_مقاطعات()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) شاشة_اكتب(صف، عمود، حرف، لون) — كتابة حرف في ذاكرة VGA
                // (EN) vga_write(row, col, char, color) — write char to VGA text memory
                // (AR) عنوان VGA النصي: 0xB8000 + (صف * 80 + عمود) * 2
                // ──────────────────────────────────────────────
                if (funcName == Bn::Kernel::VGA_WRITE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Kernel::VGA_WRITE, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_VGA_WRITE);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin شاشة_اكتب()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) شاشة_امسح(لون) — مسح شاشة VGA بلون محدد
                // (EN) vga_clear(color) — clear VGA screen with specified color
                // ──────────────────────────────────────────────
                if (funcName == Bn::Kernel::VGA_CLEAR)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_VGA_CLEAR);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin شاشة_امسح()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) انسخ_ذاكرة(وجهة، مصدر، حجم) — نسخ كتلة ذاكرة
                // (EN) mem_copy(dest, src, size) — copy memory block
                // ──────────────────────────────────────────────
                if (funcName == Bn::Kernel::MEMCPY)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Kernel::MEMCPY, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_COPY);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin انسخ_ذاكرة()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) املأ_ذاكرة(وجهة، قيمة، حجم) — ملء كتلة ذاكرة بقيمة محددة
                // (EN) mem_set(dest, value, size) — fill memory block with value
                // ──────────────────────────────────────────────
                if (funcName == Bn::Kernel::MEMSET)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::Kernel::MEMSET, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_SET);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin املأ_ذاكرة()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) املأ_ذاكرة32(عنوان، قيمة، عدد) — ملء بكلمات 32-بت (REP STOSD)
                // (EN) mem_fill32(dest, value, count) — fill 32-bit words using REP STOSD
                // ──────────────────────────────────────────────
                if (funcName == Bn::CompilerMem::MEM_6)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerMem::MEM_6, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_FILL_32);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) انسخ_ذاكرة32(وجهة، مصدر، عدد) — نسخ كلمات 32-بت (REP MOVSD)
                // (EN) mem_copy32(dest, src, count) — copy 32-bit words using REP MOVSD
                // ──────────────────────────────────────────────
                if (funcName == Bn::CompilerMem::MEM_7)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerMem::MEM_7, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_COPY_32);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }


                // === Delegation to sub-functions ===

                // Embedded functions (serial, GPIO, timer, DMA, etc.)
                auto embeddedResult = buildBuiltinSystem_Embedded(funcName, isUserDefinedFunction, argResults, argOperands);
                if (embeddedResult) return embeddedResult;

                // Security functions (assert, hash, encrypt, etc.)
                auto securityResult = buildBuiltinSystem_Security(funcName, isUserDefinedFunction, argResults, argOperands);
                if (securityResult) return securityResult;

                // Crypto module functions (BLAKE3 hash, keyed hash)
                auto cryptoResult = buildBuiltinSystem_Crypto(funcName, isUserDefinedFunction, argResults, argOperands);
                if (cryptoResult) return cryptoResult;

                // FFI functions (printf, malloc, fopen, etc.)
                auto ffiResult = buildBuiltinSystem_FFI(funcName, isUserDefinedFunction, argResults, argOperands);
                if (ffiResult) return ffiResult;

                // Async/Await functions (spawn, join, channel, etc.)
                auto asyncResult = buildBuiltinSystem_Async(funcName, isUserDefinedFunction, argResults, argOperands);
                if (asyncResult) return asyncResult;

                // UI Widget factories (column, row, text, button, etc.)
                auto uiResult = buildBuiltinSystem_UI(funcName, isUserDefinedFunction, argResults, argOperands);
                if (uiResult) return uiResult;

                // Not a system builtin
                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
