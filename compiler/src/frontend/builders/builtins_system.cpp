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

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // sir_builder_builtins_system.cpp
            // (AR) ״§„״¯ˆ״§„ ״§„…״¯…״¬״© „„†״¸״§…: ״§„״£״¬‡״²״©״ ״§„…†״§״°״ ״§„…״₪‚״×״§״×״ ״§„״°״±״©״ ״§„״×ˆ״§״²״ ״§„״£…״§†
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

                if (funcName == "منفذ_اكتب" || funcName == "port_write" || funcName == "outb")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة منفذ_اكتب تتطلب معاملين: رقم المنفذ والقيمة" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE);
                    inst.operands.push_back(argOperands[0]); // (AR) رقم المنفذ / (EN) port number
                    inst.operands.push_back(argOperands[1]); // (AR) القيمة المكتوبة / (EN) value to write
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin منفذ_اكتب()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // (AR) منفذ_اكتب16 / منفذ_اكتب32
                if (funcName == "منفذ_اكتب16" || funcName == "port_write16" || funcName == "outw")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE_16);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "منفذ_اكتب32" || funcName == "port_write32" || funcName == "outl")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE_32);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) منفذ_اقرأ(منفذ) — قراءة بايت من منفذ I/O
                // (EN) port_read(port) — read byte from I/O port (inb)
                // ──────────────────────────────────────────────
                if (funcName == "منفذ_اقرأ" || funcName == "port_read" || funcName == "inb")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة منفذ_اقرأ تتطلب معامل واحد: رقم المنفذ" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_PORT_READ);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin منفذ_اقرأ() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) منفذ_اقرأ16 / منفذ_اقرأ32
                if (funcName == "منفذ_اقرأ16" || funcName == "port_read16" || funcName == "inw")
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
                if (funcName == "منفذ_اقرأ32" || funcName == "port_read32" || funcName == "inl")
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
                // (AR) ذاكرة_اكتب(عنوان، قيمة) — كتابة بايت في عنوان ذاكرة محدد
                // (EN) mem_write(address, value) — write byte to memory address (poke)
                // (AR) يستخدم في: الكتابة على ذاكرة VGA (0xB8000)، جداول المقاطعات
                // ──────────────────────────────────────────────
                if (funcName == "ذاكرة_اكتب" || funcName == "mem_write" || funcName == "poke")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة ذاكرة_اكتب تتطلب معاملين: العنوان والقيمة" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_8);
                    inst.operands.push_back(argOperands[0]); // (AR) عنوان الذاكرة / (EN) memory address
                    inst.operands.push_back(argOperands[1]); // (AR) القيمة / (EN) value
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin ذاكرة_اكتب()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // (AR) ذاكرة_اكتب16 / ذاكرة_اكتب32 / ذاكرة_اكتب64
                if (funcName == "ذاكرة_اكتب16" || funcName == "mem_write16" || funcName == "poke16")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_16);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ذاكرة_اكتب32" || funcName == "mem_write32" || funcName == "poke32")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_32);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ذاكرة_اكتب64" || funcName == "mem_write64" || funcName == "poke64")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_64);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) ذاكرة_اقرأ(عنوان) — قراءة بايت من عنوان ذاكرة محدد
                // (EN) mem_read(address) — read byte from memory address (peek)
                // ──────────────────────────────────────────────
                if (funcName == "ذاكرة_اقرأ" || funcName == "mem_read" || funcName == "peek")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة ذاكرة_اقرأ تتطلب معامل واحد: العنوان" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_READ_8);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin ذاكرة_اقرأ() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) ذاكرة_اقرأ16 / ذاكرة_اقرأ32 / ذاكرة_اقرأ64
                if (funcName == "ذاكرة_اقرأ16" || funcName == "mem_read16" || funcName == "peek16")
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
                if (funcName == "ذاكرة_اقرأ32" || funcName == "mem_read32" || funcName == "peek32")
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
                if (funcName == "ذاكرة_اقرأ64" || funcName == "mem_read64" || funcName == "peek64")
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
                if (funcName == "مقاطعة" || funcName == "interrupt" || funcName == "int_call")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة مقاطعة تتطلب معامل واحد: رقم المقاطعة" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                // (AR) توقف() — إيقاف المعالج حتى المقاطعة التالية
                // (EN) halt() — halt CPU until next interrupt (hlt instruction)
                // (AR) يستخدم في: حلقة الخمول الرئيسية للنواة
                // ──────────────────────────────────────────────
                if (funcName == "توقف" || funcName == "halt" || funcName == "hlt")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_HALT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin توقف()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) تعطيل_مقاطعات() — تعطيل جميع المقاطعات (cli)
                // (EN) disable_interrupts() — disable all interrupts (cli instruction)
                // (AR) ضروري عند تعديل جداول المقاطعات أو البيانات الحرجة
                // ──────────────────────────────────────────────
                if (funcName == "تعطيل_مقاطعات" || funcName == "disable_interrupts" || funcName == "cli")
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
                if (funcName == "تفعيل_مقاطعات" || funcName == "enable_interrupts" || funcName == "sti")
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
                if (funcName == "شاشة_اكتب" || funcName == "vga_write")
                {
                    if (argResults.size() < 4)
                    {
                        std::cerr << "[خطأ] دالة شاشة_اكتب تتطلب 4 معاملات: صف، عمود، حرف، لون" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                if (funcName == "شاشة_امسح" || funcName == "vga_clear")
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
                if (funcName == "انسخ_ذاكرة" || funcName == "mem_copy" || funcName == "memcpy")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] دالة انسخ_ذاكرة تتطلب 3 معاملات: وجهة، مصدر، حجم" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                if (funcName == "املأ_ذاكرة" || funcName == "mem_set" || funcName == "memset")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] دالة املأ_ذاكرة تتطلب 3 معاملات: وجهة، قيمة، حجم" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                // (AR) ذاكرة_املأ32(عنوان، قيمة، عدد) — ملء بكلمات 32-بت (REP STOSD)
                // (EN) mem_fill32(dest, value, count) — fill 32-bit words using REP STOSD
                // ──────────────────────────────────────────────
                if (funcName == "ذاكرة_املأ32" || funcName == "mem_fill32" || funcName == "memset32")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] دالة ذاكرة_املأ32 تتطلب 3 معاملات: عنوان، قيمة، عدد" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_MEM_FILL_32);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // (AR) ذاكرة_انسخ32(وجهة، مصدر، عدد) — نسخ كلمات 32-بت (REP MOVSD)
                // (EN) mem_copy32(dest, src, count) — copy 32-bit words using REP MOVSD
                // ──────────────────────────────────────────────
                if (funcName == "ذاكرة_انسخ32" || funcName == "mem_copy32" || funcName == "memcpy32")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] دالة ذاكرة_انسخ32 تتطلب 3 معاملات: وجهة، مصدر، عدد" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
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
