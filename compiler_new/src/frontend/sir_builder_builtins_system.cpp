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
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// sir_builder_builtins_system.cpp
// (AR) ״§„״¯ˆ״§„ ״§„…״¯…״¬״© „„†״¸״§…: ״§„״£״¬‡״²״©״ ״§„…†״§״°״ ״§„…״₪‚״×״§״×״ ״§„״°״±״©״ ״§„״×ˆ״§״²״ ״§„״£…״§†
// (EN) System builtins: hardware, ports, timers, atomic ops, async, security
// ============================================================================

std::optional<BuildResult> SIRBuilder::buildBuiltinCallSystem(
    const std::string& funcName,
    bool isUserDefinedFunction,
    std::vector<BuildResult>& argResults,
    std::vector<SIROperand>& argOperands) {
    // (AR) إذا كانت الدالة معرّفة من المستخدم، لا تعامل كدالة مضمنة
    // (EN) If function is user-defined, skip all builtins
    if (isUserDefinedFunction) return std::nullopt;
    
    if (funcName == "منفذ_اكتب" || funcName == "port_write" || funcName == "outb") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة منفذ_اكتب تتطلب معاملين: رقم المنفذ والقيمة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_PORT_WRITE);
        inst.operands.push_back(argOperands[0]);  // (AR) رقم المنفذ / (EN) port number
        inst.operands.push_back(argOperands[1]);  // (AR) القيمة المكتوبة / (EN) value to write
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin منفذ_اكتب()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) منفذ_اقرأ(منفذ) — قراءة بايت من منفذ I/O
    // (EN) port_read(port) — read byte from I/O port (inb)
    // ──────────────────────────────────────────────
    if (funcName == "منفذ_اقرأ" || funcName == "port_read" || funcName == "inb") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة منفذ_اقرأ تتطلب معامل واحد: رقم المنفذ" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_PORT_READ);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin منفذ_اقرأ() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // ──────────────────────────────────────────────
    // (AR) ذاكرة_اكتب(عنوان، قيمة) — كتابة بايت في عنوان ذاكرة محدد
    // (EN) mem_write(address, value) — write byte to memory address (poke)
    // (AR) يستخدم في: الكتابة على ذاكرة VGA (0xB8000)، جداول المقاطعات
    // ──────────────────────────────────────────────
    if (funcName == "ذاكرة_اكتب" || funcName == "mem_write" || funcName == "poke") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة ذاكرة_اكتب تتطلب معاملين: العنوان والقيمة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_WRITE_8);
        inst.operands.push_back(argOperands[0]);  // (AR) عنوان الذاكرة / (EN) memory address
        inst.operands.push_back(argOperands[1]);  // (AR) القيمة / (EN) value
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin ذاكرة_اكتب()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) ذاكرة_اقرأ(عنوان) — قراءة بايت من عنوان ذاكرة محدد
    // (EN) mem_read(address) — read byte from memory address (peek)
    // ──────────────────────────────────────────────
    if (funcName == "ذاكرة_اقرأ" || funcName == "mem_read" || funcName == "peek") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة ذاكرة_اقرأ تتطلب معامل واحد: العنوان" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_READ_8);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin ذاكرة_اقرأ() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // ──────────────────────────────────────────────
    // (AR) مقاطعة(رقم) — إطلاق مقاطعة برمجية
    // (EN) interrupt(number) — trigger software interrupt (int N)
    // (AR) مثال: مقاطعة(0x80) لاستدعاء نظام Linux
    // ──────────────────────────────────────────────
    if (funcName == "مقاطعة" || funcName == "interrupt" || funcName == "int_call") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة مقاطعة تتطلب معامل واحد: رقم المقاطعة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_INTERRUPT);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin مقاطعة()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) توقف() — إيقاف المعالج حتى المقاطعة التالية
    // (EN) halt() — halt CPU until next interrupt (hlt instruction)
    // (AR) يستخدم في: حلقة الخمول الرئيسية للنواة
    // ──────────────────────────────────────────────
    if (funcName == "توقف" || funcName == "halt" || funcName == "hlt") {
        SIRInstruction inst(SIROpcode::BUILTIN_HALT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin توقف()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) تعطيل_مقاطعات() — تعطيل جميع المقاطعات (cli)
    // (EN) disable_interrupts() — disable all interrupts (cli instruction)
    // (AR) ضروري عند تعديل جداول المقاطعات أو البيانات الحرجة
    // ──────────────────────────────────────────────
    if (funcName == "تعطيل_مقاطعات" || funcName == "disable_interrupts" || funcName == "cli") {
        SIRInstruction inst(SIROpcode::BUILTIN_CLI);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin تعطيل_مقاطعات()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) تفعيل_مقاطعات() — تفعيل جميع المقاطعات (sti)
    // (EN) enable_interrupts() — enable all interrupts (sti instruction)
    // ──────────────────────────────────────────────
    if (funcName == "تفعيل_مقاطعات" || funcName == "enable_interrupts" || funcName == "sti") {
        SIRInstruction inst(SIROpcode::BUILTIN_STI);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin تفعيل_مقاطعات()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) شاشة_اكتب(صف، عمود، حرف، لون) — كتابة حرف في ذاكرة VGA
    // (EN) vga_write(row, col, char, color) — write char to VGA text memory
    // (AR) عنوان VGA النصي: 0xB8000 + (صف * 80 + عمود) * 2
    // ──────────────────────────────────────────────
    if (funcName == "شاشة_اكتب" || funcName == "vga_write") {
        if (argResults.size() < 4) {
            std::cerr << "[خطأ] دالة شاشة_اكتب تتطلب 4 معاملات: صف، عمود، حرف، لون" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_VGA_WRITE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin شاشة_اكتب()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) شاشة_امسح(لون) — مسح شاشة VGA بلون محدد
    // (EN) vga_clear(color) — clear VGA screen with specified color
    // ──────────────────────────────────────────────
    if (funcName == "شاشة_امسح" || funcName == "vga_clear") {
        SIRInstruction inst(SIROpcode::BUILTIN_VGA_CLEAR);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin شاشة_امسح()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) انسخ_ذاكرة(وجهة، مصدر، حجم) — نسخ كتلة ذاكرة
    // (EN) mem_copy(dest, src, size) — copy memory block
    // ──────────────────────────────────────────────
    if (funcName == "انسخ_ذاكرة" || funcName == "mem_copy" || funcName == "memcpy") {
        if (argResults.size() < 3) {
            std::cerr << "[خطأ] دالة انسخ_ذاكرة تتطلب 3 معاملات: وجهة، مصدر، حجم" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_COPY);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin انسخ_ذاكرة()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // ──────────────────────────────────────────────
    // (AR) املأ_ذاكرة(وجهة، قيمة، حجم) — ملء كتلة ذاكرة بقيمة محددة
    // (EN) mem_set(dest, value, size) — fill memory block with value
    // ──────────────────────────────────────────────
    if (funcName == "املأ_ذاكرة" || funcName == "mem_set" || funcName == "memset") {
        if (argResults.size() < 3) {
            std::cerr << "[خطأ] دالة املأ_ذاكرة تتطلب 3 معاملات: وجهة، قيمة، حجم" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_MEM_SET);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin املأ_ذاكرة()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ========================================================================
    // (AR) دوال Embedded المتقدمة — Advanced Embedded Builtin Functions (18)
    // (EN) Advanced Embedded Builtin Functions (18 functions)
    // ========================================================================
    // تسلسلي / Serial I/O — 4 دوال
    // GPIO — 3 دوال
    // مؤقت / Timer — 3 دوال
    // تحكم بالنظام / System Control — 3 دوال
    // حواجز ذاكرة / Memory Barriers — 3 دوال
    // DMA — 2 دوال
    // ========================================================================

    // ──────────────────────────────────────────────
    // 1. تسلسلي_هيئ(منفذ، سرعة) — تهيئة منفذ تسلسلي
    //    serial_init(port, baud_rate) — initialize serial port
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_هيئ" || funcName == "serial_init")) {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تسلسلي_هيئ تتطلب 2 معاملات: منفذ، سرعة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_INIT);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin تسلسلي_هيئ()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 2. تسلسلي_ارسل(منفذ، بايت) — إرسال بايت عبر التسلسلي
    //    serial_send(port, byte) — send byte via serial
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_ارسل" || funcName == "serial_send" || funcName == "serial_write")) {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تسلسلي_ارسل تتطلب 2 معاملات: منفذ، بايت" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_WRITE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin تسلسلي_ارسل()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 3. تسلسلي_استقبل(منفذ) — استقبال بايت من التسلسلي
    //    serial_receive(port) — receive byte from serial
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_استقبل" || funcName == "serial_receive" || funcName == "serial_read")) {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تسلسلي_استقبل تتطلب معامل: منفذ" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_READ);
        inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin تسلسلي_استقبل()" << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 4. تسلسلي_جاهز(منفذ) — فحص جاهزية البيانات
    //    serial_ready(port) — check if data available
    // ──────────────────────────────────────────────
    if (!isUserDefinedFunction && (funcName == "تسلسلي_جاهز" || funcName == "serial_ready")) {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تسلسلي_جاهز تتطلب معامل: منفذ" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_READY);
        inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin تسلسلي_جاهز()" << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }

    // ──────────────────────────────────────────────
    // 5. منفذ_رقمي_اكتب(رقم، قيمة) — كتابة GPIO
    //    gpio_write(pin, value) — write to GPIO pin
    // ──────────────────────────────────────────────
    if (funcName == "منفذ_رقمي_اكتب" || funcName == "gpio_write" || funcName == "digital_write") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة منفذ_رقمي_اكتب تتطلب 2 معاملات: رقم_المنفذ، قيمة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_GPIO_WRITE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin منفذ_رقمي_اكتب()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 6. منفذ_رقمي_اقرأ(رقم) — قراءة GPIO
    //    gpio_read(pin) — read from GPIO pin
    // ──────────────────────────────────────────────
    if (funcName == "منفذ_رقمي_اقرأ" || funcName == "gpio_read" || funcName == "digital_read") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة منفذ_رقمي_اقرأ تتطلب معامل: رقم_المنفذ" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_GPIO_READ);
        inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin منفذ_رقمي_اقرأ()" << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 7. حدد_وضع_منفذ(رقم، وضع) — تحديد وضع GPIO
    //    gpio_mode(pin, mode) — set GPIO pin mode (0=input, 1=output)
    // ──────────────────────────────────────────────
    if (funcName == "حدد_وضع_منفذ" || funcName == "gpio_mode" || funcName == "pin_mode") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة حدد_وضع_منفذ تتطلب 2 معاملات: رقم_المنفذ، الوضع" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_GPIO_MODE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin حدد_وضع_منفذ()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 8. مؤقت_هيئ(تردد) — تهيئة مؤقت العتاد
    //    timer_init(freq) — initialize hardware timer
    // ──────────────────────────────────────────────
    if (funcName == "مؤقت_هيئ" || funcName == "timer_init") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة مؤقت_هيئ تتطلب معامل: التردد" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_TIMER_INIT);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin مؤقت_هيئ()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 9. مؤقت_قراءة() — قراءة قيمة المؤقت الحالية
    //    timer_read() — read current timer value
    // ──────────────────────────────────────────────
    if (funcName == "مؤقت_قراءة" || funcName == "timer_read") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_TIMER_READ);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin مؤقت_قراءة()" << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 10. مؤقت_انتظر(ميكروثانية) — انتظار عدد ميكروثوان
    //     timer_wait(us) — wait for microseconds
    // ──────────────────────────────────────────────
    if (funcName == "مؤقت_انتظر" || funcName == "timer_wait" || funcName == "delay_us") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة مؤقت_انتظر تتطلب معامل: ميكروثوان" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_TIMER_WAIT);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin مؤقت_انتظر()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 11. اعد_تشغيل() — إعادة تشغيل النظام
    //     reset() / reboot() — system reset
    // ──────────────────────────────────────────────
    if (funcName == "اعد_تشغيل" || funcName == "reset" || funcName == "reboot") {
        SIRInstruction inst(SIROpcode::BUILTIN_RESET);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin اعد_تشغيل()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 12. معرف_المعالج() — الحصول على معرّف المعالج
    //     cpu_id() / cpuid() — get CPU identification
    // ──────────────────────────────────────────────
    if (funcName == "معرف_المعالج" || funcName == "cpu_id" || funcName == "cpuid") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_CPUID);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin معرف_المعالج()" << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 13. عداد_الدورات() — قراءة عداد الساعة (TSC)
    //     rdtsc() / cycle_count() — read timestamp counter
    // ──────────────────────────────────────────────
    if (funcName == "عداد_الدورات" || funcName == "rdtsc" || funcName == "cycle_count") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_RDTSC);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin عداد_الدورات()" << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // ──────────────────────────────────────────────
    // 14. حاجز_ذاكرة() — حاجز ذاكرة كامل (mfence)
    //     memory_barrier() / mfence() — full memory fence
    // ──────────────────────────────────────────────
    if (funcName == "حاجز_ذاكرة" || funcName == "memory_barrier" || funcName == "mfence") {
        SIRInstruction inst(SIROpcode::BUILTIN_MFENCE);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin حاجز_ذاكرة()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 15. حاجز_قراءة() — حاجز قراءة ذاكرة (lfence)
    //     read_barrier() / lfence() — load fence
    // ──────────────────────────────────────────────
    if (funcName == "حاجز_قراءة" || funcName == "read_barrier" || funcName == "lfence") {
        SIRInstruction inst(SIROpcode::BUILTIN_LFENCE);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin حاجز_قراءة()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 16. حاجز_كتابة() — حاجز كتابة ذاكرة (sfence)
    //     write_barrier() / sfence() — store fence
    // ──────────────────────────────────────────────
    if (funcName == "حاجز_كتابة" || funcName == "write_barrier" || funcName == "sfence") {
        SIRInstruction inst(SIROpcode::BUILTIN_SFENCE);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin حاجز_كتابة()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 17. نقل_مباشر_هيئ(قناة، مصدر، وجهة، حجم) — تهيئة DMA
    //     dma_init(channel, src, dest, size) — initialize DMA channel
    // ──────────────────────────────────────────────
    if (funcName == "نقل_مباشر_هيئ" || funcName == "dma_init") {
        if (argResults.size() < 4) {
            std::cerr << "[خطأ] دالة نقل_مباشر_هيئ تتطلب 4 معاملات: قناة، مصدر، وجهة، حجم" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_DMA_INIT);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin نقل_مباشر_هيئ()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ──────────────────────────────────────────────
    // 18. نقل_مباشر_ابدأ(قناة) — بدء نقل DMA
    //     dma_start(channel) — start DMA transfer
    // ──────────────────────────────────────────────
    if (funcName == "نقل_مباشر_ابدأ" || funcName == "dma_start") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة نقل_مباشر_ابدأ تتطلب معامل: رقم_القناة" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_DMA_START);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin نقل_مباشر_ابدأ()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // ========================================================================
    // (AR) دوال الأمان — Security Builtin Functions (14 functions)
    // (EN) Security Builtin Functions (14 functions)
    // ========================================================================
    // (AR) هذا القسم يضيف الدوال المضمنة لنظام الأمان:
    //      - تأكد/تحقق/آمن: فحص الشروط والتحقق من الأمان
    //      - ذعر: إيقاف طارئ مع رسالة
    //      - هاش/شفّر/فك_تشفير: عمليات التشفير
    //      - تأكد_نوع/تأكد_مساواة/تأكد_أكبر: تأكيدات متقدمة
    //      - نظّف: تنظيف المدخلات من HTML
    //      - وقت_الآن/عشوائي_آمن/ترميز_64: أدوات مساعدة
    // ========================================================================
    
    // 1. تأكد / assert - يتحقق من شرط ويوقف البرنامج إذا كان خاطئاً
    if (funcName == "تأكد" || funcName == "assert" || funcName == "تاكد") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تأكد تتطلب معامل واحد على الأقل (الشرط)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT);
        inst.operands.push_back(argOperands[0]); // condition
        if (argOperands.size() > 1) {
            inst.operands.push_back(argOperands[1]); // optional message
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 2. تحقق / verify - يعيد صحيح أو خطأ دون إيقاف البرنامج
    if (funcName == "تحقق" || funcName == "verify") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة تحقق تتطلب معامل واحد (الشرط)" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_VERIFY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 3. آمن / is_safe - يتحقق من أمان القيمة
    if (funcName == "آمن" || funcName == "is_safe" || funcName == "امن") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة آمن تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::BOOL);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::BOOL);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_IS_SAFE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // 4. ذعر / panic - إيقاف طارئ مع رسالة خطأ
    if (funcName == "ذعر" || funcName == "panic") {
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_PANIC);
        if (!argOperands.empty()) {
            inst.operands.push_back(argOperands[0]); // message
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 5. هاش / hash - حساب هاش FNV-1a للنص
    if (funcName == "هاش" || funcName == "hash") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة هاش تتطلب معامل واحد (النص)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_HASH);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 6. شفّر / encrypt - تشفير XOR
    if (funcName == "شفّر" || funcName == "شفر" || funcName == "encrypt") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة شفّر تتطلب معاملين (النص، المفتاح)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ENCRYPT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 7. فك_تشفير / decrypt - فك تشفير XOR
    if (funcName == "فك_تشفير" || funcName == "decrypt") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة فك_تشفير تتطلب معاملين (النص، المفتاح)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_DECRYPT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 8. تأكد_نوع / assert_type - التحقق من نوع القيمة
    if (funcName == "تأكد_نوع" || funcName == "assert_type" || funcName == "تاكد_نوع") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تأكد_نوع تتطلب معاملين (القيمة، النوع_المتوقع)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_TYPE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 9. تأكد_مساواة / assert_equal - التحقق من تساوي قيمتين
    if (funcName == "تأكد_مساواة" || funcName == "assert_equal" || funcName == "تاكد_مساواة") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تأكد_مساواة تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_EQUAL);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 10. تأكد_أكبر / assert_greater - التحقق من أن القيمة الأولى أكبر
    if (funcName == "تأكد_أكبر" || funcName == "assert_greater" || funcName == "تاكد_اكبر") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة تأكد_أكبر تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_ASSERT_GREATER);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 11. نظّف / sanitize - تنظيف نص من HTML
    if (funcName == "نظّف" || funcName == "نظف" || funcName == "sanitize") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة نظّف تتطلب معامل واحد (النص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_SANITIZE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 12. وقت_الآن / الآن / timestamp - الحصول على الوقت الحالي
    // (AR) يدعم: وقت_الآن، الآن، الان، timestamp، now
    // (EN) Supports: وقت_الآن, الآن, الان, timestamp, now
    if (funcName == "وقت_الآن" || funcName == "وقت_الان" || 
        funcName == "الآن" || funcName == "الان" ||
        funcName == "timestamp" || funcName == "now") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_TIMESTAMP);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 13. عشوائي_آمن / secure_random - رقم عشوائي آمن
    if (funcName == "عشوائي_آمن" || funcName == "عشوائي_امن" || funcName == "secure_random") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] دالة عشوائي_آمن تتطلب معاملين (الحد_الأدنى، الحد_الأقصى)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 14. ترميز_64 / base64_encode - ترميز Base64
    if (funcName == "ترميز_64" || funcName == "base64_encode" || funcName == "base64") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دالة ترميز_64 تتطلب معامل واحد (النص)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin " << funcName << "() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::STRING);
    }

    // ========================================================================
    // (AR) التكامل مع C/C++ — FFI Functions (20 دالة)
    // (EN) C/C++ Foreign Function Interface — 20 functions
    // ========================================================================
    
    // 1. طباعة_تنسيق / printf — formatted print (variadic)
    if (funcName == "\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xaa\xd9\x86\xd8\xb3\xd9\x8a\xd9\x82" || funcName == "printf" || funcName == "c_printf") {
        if (argOperands.empty()) {
            std::cerr << "[ERROR] printf requires at least 1 argument (format string)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_PRINTF);
        inst.result = resultOp;
        for (size_t i = 0; i < argOperands.size(); i++) {
            inst.operands.push_back(argOperands[i]);
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin printf() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 2. حجز / malloc — allocate memory
    if (funcName == "\xd8\xad\xd8\xac\xd8\xb2" || funcName == "malloc" || funcName == "c_malloc") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] malloc requires 1 argument (size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_MALLOC);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin malloc() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 3. حرر / free — free memory
    if (funcName == "\xd8\xad\xd8\xb1\xd8\xb1" || funcName == "free" || funcName == "c_free") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] free requires 1 argument (pointer)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::FFI_FREE);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] builtin free()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }
    
    // 4. اعد_حجز / realloc
    if (funcName == "\xd8\xa7\xd8\xb9\xd8\xaf_\xd8\xad\xd8\xac\xd8\xb2" || funcName == "realloc" || funcName == "c_realloc") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] realloc requires 2 arguments (ptr, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_REALLOC);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 5. حجز_صفري / calloc
    if (funcName == "\xd8\xad\xd8\xac\xd8\xb2_\xd8\xb5\xd9\x81\xd8\xb1\xd9\x8a" || funcName == "calloc" || funcName == "c_calloc") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] calloc requires 2 arguments (count, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_CALLOC);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 6. طول_نص_س / strlen
    if (funcName == "\xd8\xb7\xd9\x88\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strlen" || funcName == "c_strlen") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] strlen requires 1 argument (string)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_STRLEN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 7. انسخ_نص_س / strcpy
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcpy" || funcName == "c_strcpy") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] strcpy requires 2 arguments (dest, src)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_STRCPY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 8. قارن_نص_س / strcmp
    if (funcName == "\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcmp" || funcName == "c_strcmp") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] strcmp requires 2 arguments (s1, s2)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_STRCMP);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 9. الحق_نص_س / strcat
    if (funcName == "\xd8\xa7\xd9\x84\xd8\xad\xd9\x82_\xd9\x86\xd8\xb5_\xd8\xb3" || funcName == "strcat" || funcName == "c_strcat") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] strcat requires 2 arguments (dest, src)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_STRCAT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 10. انسخ_ذاكرة_س / memcpy
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xb3\xd8\xae_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memcpy" || funcName == "c_memcpy") {
        if (argOperands.size() != 3) {
            std::cerr << "[ERROR] memcpy requires 3 arguments (dest, src, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_MEMCPY);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 11. عبئ_ذاكرة_س / memset
    if (funcName == "\xd8\xb9\xd8\xa8\xd8\xa6_\xd8\xb0\xd8\xa7\xd9\x83\xd8\xb1\xd8\xa9_\xd8\xb3" || funcName == "memset" || funcName == "c_memset") {
        if (argOperands.size() != 3) {
            std::cerr << "[ERROR] memset requires 3 arguments (ptr, value, size)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_MEMSET);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 12. افتح_ملف_س / fopen
    if (funcName == "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fopen" || funcName == "c_fopen") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] fopen requires 2 arguments (filename, mode)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_FOPEN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 13. اغلق_ملف_س / fclose
    if (funcName == "\xd8\xa7\xd8\xba\xd9\x84\xd9\x82_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fclose" || funcName == "c_fclose") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] fclose requires 1 argument (file pointer)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_FCLOSE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 14. اكتب_ملف_س / fputs — write string to file
    if (funcName == "\xd8\xa7\xd9\x83\xd8\xaa\xd8\xa8_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fputs" || funcName == "c_fputs") {
        if (argOperands.size() != 2) {
            std::cerr << "[ERROR] fputs requires 2 arguments (string, file)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_FWRITE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 15. اقرأ_ملف_س / fgets — read line from file
    if (funcName == "\xd8\xa7\xd9\x82\xd8\xb1\xd8\xa3_\xd9\x85\xd9\x84\xd9\x81_\xd8\xb3" || funcName == "fgets" || funcName == "c_fgets") {
        if (argOperands.size() != 3) {
            std::cerr << "[ERROR] fgets requires 3 arguments (buffer, size, file)" << std::endl;
            return BuildResult("", SIRType::PTR);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::PTR);
        SIRInstruction inst(SIROpcode::FFI_FREAD);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::PTR);
    }
    
    // 16. نفذ_امر / system — execute system command
    if (funcName == "\xd9\x86\xd9\x81\xd8\xb0_\xd8\xa7\xd9\x85\xd8\xb1" || funcName == "system" || funcName == "c_system") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] system requires 1 argument (command)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_SYSTEM);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 17. قيمة_بيئة / getenv
    if (funcName == "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9_\xd8\xa8\xd9\x8a\xd8\xa6\xd8\xa9" || funcName == "getenv" || funcName == "c_getenv") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] getenv requires 1 argument (name)" << std::endl;
            return BuildResult("", SIRType::STRING);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::STRING);
        SIRInstruction inst(SIROpcode::FFI_GETENV);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::STRING);
    }
    
    // 18. نص_لعدد / atoi
    if (funcName == "\xd9\x86\xd8\xb5_\xd9\x84\xd8\xb9\xd8\xaf\xd8\xaf" || funcName == "atoi" || funcName == "c_atoi") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] atoi requires 1 argument (string)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_ATOI);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // 19. نص_لعشري / atof
    if (funcName == "\xd9\x86\xd8\xb5_\xd9\x84\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a" || funcName == "atof" || funcName == "c_atof") {
        if (argOperands.size() != 1) {
            std::cerr << "[ERROR] atof requires 1 argument (string)" << std::endl;
            return BuildResult("", SIRType::F64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::F64);
        SIRInstruction inst(SIROpcode::FFI_ATOF);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::F64);
    }
    
    // 20. تنسيق_نص / snprintf — format to buffer (variadic)
    if (funcName == "\xd8\xaa\xd9\x86\xd8\xb3\xd9\x8a\xd9\x82_\xd9\x86\xd8\xb5" || funcName == "snprintf" || funcName == "c_snprintf") {
        if (argOperands.size() < 3) {
            std::cerr << "[ERROR] snprintf requires at least 3 arguments (buf, size, fmt)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::FFI_SNPRINTF);
        inst.result = resultOp;
        for (size_t i = 0; i < argOperands.size(); i++) {
            inst.operands.push_back(argOperands[i]);
        }
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }


    // ========================================================================
    // (AR) Async/Await - 23 functions
    // (EN) Async/Await & Concurrency - 23 functions
    // ========================================================================

    // 1. spawn - async task
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9" || funcName == "spawn" || funcName == "async_spawn") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] spawn requires at least 1 argument (function name)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_SPAWN);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] async spawn() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 2. await - wait for future/task
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9" || funcName == "await" || funcName == "async_await" || funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] await requires 1 argument (task/future id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_AWAIT);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] async await() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 3. yield
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xaa\xd8\xac" || funcName == "yield" || funcName == "async_yield") {
        SIRInstruction inst(SIROpcode::ASYNC_YIELD);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] async yield()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 4. async_sleep - non-blocking sleep
    if (funcName == "\xd9\x86\xd9\x88\xd9\x85_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86" || funcName == "async_sleep" || funcName == "sleep_async") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] async_sleep requires 1 argument (milliseconds)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_SLEEP);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] async_sleep()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 5. create_future
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "create_future" || funcName == "\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_CREATE_FUTURE);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] create_future() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 6. resolve_future
    if (funcName == "\xd8\xa3\xd9\x88\xd9\x81_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "resolve_future" || funcName == "\xd8\xad\xd9\x82\xd9\x82_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] resolve_future requires 2 args (future_id, value)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_RESOLVE_FUTURE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] resolve_future()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 7. get_future
    if (funcName == "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "get_future") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] get_future requires 1 argument (future_id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_GET_FUTURE);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] get_future() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 8. create_channel
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "create_channel" || funcName == "\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_CREATE_CHANNEL);
        inst.result = resultOp;
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] create_channel() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 9. channel_send
    if (funcName == "\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_send" || funcName == "\xd8\xa3\xd8\xb1\xd8\xb3\xd9\x84") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] channel_send requires 2 args (channel_id, value)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_SEND);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] channel_send()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 10. channel_recv
    if (funcName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_recv" || funcName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] channel_recv requires 1 argument (channel_id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_RECV);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] channel_recv() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 11. channel_close
    if (funcName == "\xd8\xa3\xd8\xba\xd9\x84\xd9\x82_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_close") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] channel_close requires 1 argument (channel_id)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_CLOSE);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] channel_close()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 12. create_mutex
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x81\xd9\x84" || funcName == "create_mutex" || funcName == "\xd9\x82\xd9\x81\xd9\x84") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_MUTEX_CREATE);
        inst.result = resultOp;
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] create_mutex() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 13. mutex_lock
    if (funcName == "\xd8\xa7\xd9\x82\xd9\x81\xd9\x84" || funcName == "mutex_lock" || funcName == "lock") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] mutex_lock requires 1 argument (mutex_id)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_MUTEX_LOCK);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] mutex_lock()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 14. mutex_unlock
    if (funcName == "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x82\xd9\x81\xd9\x84" || funcName == "mutex_unlock" || funcName == "unlock") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] mutex_unlock requires 1 argument (mutex_id)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_MUTEX_UNLOCK);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] mutex_unlock()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 15. thread_spawn
    if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xae\xd9\x8a\xd8\xb7" || funcName == "thread_spawn" || funcName == "\xd8\xae\xd9\x8a\xd8\xb7") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] thread_spawn requires at least 1 argument" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_THREAD_SPAWN);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] thread_spawn() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 16. thread_join
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xb6\xd9\x85_\xd8\xae\xd9\x8a\xd8\xb7" || funcName == "thread_join") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] thread_join requires 1 argument (thread_id)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_THREAD_JOIN);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] thread_join() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 17. atomic_load
    if (funcName == "\xd8\xad\xd9\x85\xd9\x84_\xd8\xb0\xd8\xb1\xd9\x8a" || funcName == "atomic_load") {
        if (argResults.empty()) {
            std::cerr << "[ERROR] atomic_load requires 1 argument (address)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_LOAD);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] atomic_load() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 18. atomic_store
    if (funcName == "\xd8\xae\xd8\xb2\xd9\x86_\xd8\xb0\xd8\xb1\xd9\x8a" || funcName == "atomic_store") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] atomic_store requires 2 args (address, value)" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_STORE);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] atomic_store()" << std::endl;
        #endif
        return BuildResult("", SIRType::VOID);
    }

    // 19. atomic_add
    if (funcName == "\xd8\xa3\xd8\xb6\xd9\x81_\xd8\xb0\xd8\xb1\xd9\x8a" || funcName == "atomic_add") {
        if (argResults.size() < 2) {
            std::cerr << "[ERROR] atomic_add requires 2 args (address, value)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_ADD);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] atomic_add() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 20. compare_and_swap / CAS
    if (funcName == "\xd9\x82\xd8\xa7\xd8\xb1\xd9\x86_\xd9\x88\xd8\xa8\xd8\xaf\xd9\x84" || funcName == "compare_and_swap" || funcName == "cas") {
        if (argResults.size() < 3) {
            std::cerr << "[ERROR] CAS requires 3 args (addr, expected, desired)" << std::endl;
            return BuildResult("", SIRType::I64);
        }
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_CAS);
        inst.result = resultOp;
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        inst.operands.push_back(argOperands[2]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] compare_and_swap() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 21. wait_all
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84" || funcName == "wait_all") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_WAIT_ALL);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] wait_all() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 22. wait_any
    if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa3\xd9\x8a" || funcName == "wait_any") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_WAIT_ANY);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] wait_any() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // 23. select / channel_select
    if (funcName == "\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb1_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "select" || funcName == "channel_select") {
        std::string resultReg = newTempRegister();
        SIROperand resultOp = SIROperand::Register(resultReg, SIRType::I64);
        SIRInstruction inst(SIROpcode::ASYNC_SELECT);
        inst.result = resultOp;
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] select() -> " << resultReg << std::endl;
        #endif
        return BuildResult(resultReg, SIRType::I64);
    }

    // ========================================================================
    // Section 15: عمليات وحدات نظام التشغيل المتقدمة / Advanced OS Modules
    // (AR) دعم المكتبة المنخفضة المستوى الكاملة (19 وحدة) لوضع بدون نظام تشغيل
    // (EN) Full low-level library support (19 modules) for freestanding/bare-metal
    // ========================================================================

    // ─── 15a. وحدة المعالج المتقدمة / Advanced CPU Module ───
    if (funcName == "معلومات_المعالج" || funcName == "cpu_get_info") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_INFO);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "ميزات_المعالج" || funcName == "cpu_get_features") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_FEATURES);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اقرأ_سجل_نموذج" || funcName == "read_msr") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_READ_MSR);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اكتب_سجل_نموذج" || funcName == "write_msr") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_WRITE_MSR);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "اقرأ_سجل_تحكم" || funcName == "read_cr") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_READ_CR);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اكتب_سجل_تحكم" || funcName == "write_cr") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_WRITE_CR);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ابطل_صفحة" || funcName == "invlpg") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_INVLPG);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_المعالج" || funcName == "cpu_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15b. وحدة GDT ───
    if (funcName == "هيئ_جدول_واصفات" || funcName == "gdt_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "حمل_جدول_واصفات" || funcName == "gdt_load") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_LOAD);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_واصفات" || funcName == "gdt_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15c. وحدة الترحيل / Paging ───
    if (funcName == "هيئ_ترحيل" || funcName == "paging_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "رحل_صفحة" || funcName == "paging_map") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_MAP);
        inst.operands.push_back(argOperands[0]); // virtual address
        inst.operands.push_back(argOperands[1]); // physical address
        if (argResults.size() > 2) inst.operands.push_back(argOperands[2]); // flags
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "الغ_ترحيل" || funcName == "paging_unmap") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_UNMAP);
        inst.operands.push_back(argOperands[0]); // virtual address
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "افرغ_ذاكرة_ترجمة" || funcName == "paging_flush_tlb") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_FLUSH_TLB);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_ترحيل" || funcName == "paging_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts (IDT) ───
    if (funcName == "هيئ_جدول_مقاطعات" || funcName == "idt_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "حمل_جدول_مقاطعات" || funcName == "idt_load") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_LOAD);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "سجل_معالج_مقاطعة" || funcName == "register_isr") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_REGISTER_ISR);
        inst.operands.push_back(argOperands[0]); // ISR number
        inst.operands.push_back(argOperands[1]); // handler function ptr
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "فعل_طلب_مقاطعة" || funcName == "enable_irq") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_ENABLE_IRQ);
        inst.operands.push_back(argOperands[0]); // IRQ number
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_مقاطعات" || funcName == "idt_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15e. وحدة PCI ───
    if (funcName == "عدد_أجهزة_ناقل" || funcName == "pci_enumerate") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_ENUMERATE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اقرأ_اعدادات_ناقل" || funcName == "pci_read_config") {
        if (argResults.size() < 4) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_READ_CONFIG);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // bus
        inst.operands.push_back(argOperands[1]); // device
        inst.operands.push_back(argOperands[2]); // function
        inst.operands.push_back(argOperands[3]); // offset
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اكتب_اعدادات_ناقل" || funcName == "pci_write_config") {
        if (argResults.size() < 5) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_WRITE_CONFIG);
        inst.operands.push_back(argOperands[0]); // bus
        inst.operands.push_back(argOperands[1]); // device
        inst.operands.push_back(argOperands[2]); // function
        inst.operands.push_back(argOperands[3]); // offset
        inst.operands.push_back(argOperands[4]); // value
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "عدد_الأجهزة" || funcName == "pci_device_count") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_GET_DEVICE_COUNT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "تقرير_ناقل" || funcName == "pci_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15f. وحدة DMA المتقدمة ───
    if (funcName == "هيئ_نقل_مباشر" || funcName == "dma_init_full") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ابدأ_نقل" || funcName == "dma_transfer") {
        if (argResults.size() < 3) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_TRANSFER);
        inst.operands.push_back(argOperands[0]); // source
        inst.operands.push_back(argOperands[1]); // destination
        inst.operands.push_back(argOperands[2]); // size
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "حالة_نقل" || funcName == "dma_status") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_STATUS);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "تقرير_نقل" || funcName == "dma_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15g. وحدة الشاشة / Framebuffer ───
    if (funcName == "هيئ_شاشة" || funcName == "fb_init") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_INIT);
        inst.operands.push_back(argOperands[0]); // width
        inst.operands.push_back(argOperands[1]); // height
        if (argResults.size() > 2) inst.operands.push_back(argOperands[2]); // bpp
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ارسم_نقطة" || funcName == "fb_set_pixel") {
        if (argResults.size() < 3) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_SET_PIXEL);
        inst.operands.push_back(argOperands[0]); // x
        inst.operands.push_back(argOperands[1]); // y
        inst.operands.push_back(argOperands[2]); // color
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ارسم_مستطيل" || funcName == "fb_draw_rect") {
        if (argResults.size() < 5) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_RECT);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "املأ_مستطيل" || funcName == "fb_fill_rect") {
        if (argResults.size() < 5) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_FILL_RECT);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ارسم_خط" || funcName == "fb_draw_line") {
        if (argResults.size() < 5) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_LINE);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ارسم_نص" || funcName == "fb_draw_string") {
        if (argResults.size() < 3) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_STRING);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "امسح_شاشة" || funcName == "fb_clear") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_CLEAR);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]); // color (optional)
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_شاشة" || funcName == "fb_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_FB_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15h. وحدة ACPI ───
    if (funcName == "هيئ_طاقة" || funcName == "acpi_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ابحث_جدول_طاقة" || funcName == "acpi_find_table") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_FIND_TABLE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // table signature
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اطفئ" || funcName == "acpi_shutdown") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_SHUTDOWN);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_طاقة" || funcName == "acpi_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15i. وحدة التزامن / Sync ───
    if (funcName == "هيئ_قفل_دوار" || funcName == "spinlock_init") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_INIT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اقفل_دوار" || funcName == "spinlock_lock") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_LOCK);
        inst.operands.push_back(argOperands[0]); // lock ptr
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "افتح_قفل_دوار" || funcName == "spinlock_unlock") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_UNLOCK);
        inst.operands.push_back(argOperands[0]); // lock ptr
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "هيئ_كابح" || funcName == "mutex_init") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_INIT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "اقفل_كابح" || funcName == "mutex_lock") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_LOCK);
        inst.operands.push_back(argOperands[0]); // mutex ptr
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "افتح_كابح" || funcName == "mutex_unlock") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_UNLOCK);
        inst.operands.push_back(argOperands[0]); // mutex ptr
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "هيئ_اشارة" || funcName == "semaphore_init") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_SEMAPHORE_INIT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // count
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "هيئ_حاجز" || funcName == "barrier_init") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_BARRIER_INIT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // count
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // ─── 15j. وحدة المجدول / Scheduler ───
    if (funcName == "هيئ_مجدول" || funcName == "sched_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "انشئ_عملية" || funcName == "sched_create_process") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_CREATE_PROC);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // name
        if (argResults.size() > 1) inst.operands.push_back(argOperands[1]); // priority
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64); // PID
    }
    if (funcName == "انشئ_خيط_نواة" || funcName == "sched_create_thread") {
        if (argResults.size() < 2) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_CREATE_THREAD);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // name
        inst.operands.push_back(argOperands[1]); // entry function
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64); // TID
    }
    if (funcName == "تنازل" || funcName == "sched_yield") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_YIELD);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "نوم_مجدول" || funcName == "sched_sleep") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_SLEEP);
        inst.operands.push_back(argOperands[0]); // milliseconds
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_مجدول" || funcName == "sched_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15k. وحدة الإقلاع / Boot ───
    if (funcName == "معلومات_اقلاع" || funcName == "boot_info") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_INFO);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "خريطة_ذاكرة_اقلاع" || funcName == "boot_memory_map") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_MEMORY_MAP);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "تقرير_اقلاع" || funcName == "boot_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15l. وحدة نظام الملفات الافتراضي / VFS ───
    if (funcName == "حمل_قرص" || funcName == "vfs_mount") {
        if (argResults.size() < 2) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_MOUNT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // device
        inst.operands.push_back(argOperands[1]); // mount point
        if (argResults.size() > 2) inst.operands.push_back(argOperands[2]); // fs type
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "افصل_قرص" || funcName == "vfs_unmount") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_UNMOUNT);
        inst.operands.push_back(argOperands[0]); // mount point
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "افتح_ملف_نواة" || funcName == "vfs_open") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_OPEN);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // path
        if (argResults.size() > 1) inst.operands.push_back(argOperands[1]); // flags
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64); // fd
    }
    if (funcName == "اقرأ_ملف_نواة" || funcName == "vfs_read") {
        if (argResults.size() < 2) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_READ);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // fd
        inst.operands.push_back(argOperands[1]); // buffer/size
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64); // bytes read
    }
    if (funcName == "اكتب_ملف_نواة" || funcName == "vfs_write") {
        if (argResults.size() < 2) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_WRITE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // fd
        inst.operands.push_back(argOperands[1]); // data
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64); // bytes written
    }
    if (funcName == "اغلق_ملف_نواة" || funcName == "vfs_close") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_CLOSE);
        inst.operands.push_back(argOperands[0]); // fd
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_ملفات" || funcName == "vfs_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15m. وحدة APIC ───
    if (funcName == "هيئ_متحكم_مقاطعات" || funcName == "apic_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ارسل_نهاية_مقاطعة" || funcName == "apic_send_eoi") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_EOI);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "ارسل_مقاطعة_معالج" || funcName == "apic_send_ipi") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_IPI);
        inst.operands.push_back(argOperands[0]); // target CPU
        inst.operands.push_back(argOperands[1]); // vector
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "اضبط_مؤقت_متحكم" || funcName == "apic_set_timer") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SET_TIMER);
        inst.operands.push_back(argOperands[0]); // divisor
        inst.operands.push_back(argOperands[1]); // count
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_متحكم_مقاطعات" || funcName == "apic_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15n. وحدة HPET ───
    if (funcName == "هيئ_مؤقت_دقيق" || funcName == "hpet_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "اقرأ_مؤقت_دقيق" || funcName == "hpet_read") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_READ);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "نوم_دقيق" || funcName == "hpet_sleep") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_SLEEP);
        inst.operands.push_back(argOperands[0]); // nanoseconds
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_مؤقت_دقيق" || funcName == "hpet_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15o. وحدة استدعاءات النظام / Syscall ───
    if (funcName == "هيئ_استدعاءات" || funcName == "syscall_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_INIT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "سجل_استدعاء" || funcName == "syscall_register") {
        if (argResults.size() < 2) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_REGISTER);
        inst.operands.push_back(argOperands[0]); // syscall number
        inst.operands.push_back(argOperands[1]); // handler function
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "نفذ_استدعاء" || funcName == "syscall_invoke") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_INVOKE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "تقرير_استدعاءات" || funcName == "syscall_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── 15p. عمليات الذاكرة المتقدمة / Advanced Memory ───
    if (funcName == "خصص_فيزيائي" || funcName == "alloc_physical") {
        if (argResults.empty()) return BuildResult("", SIRType::I64);
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_ALLOC_PHYS);
        inst.result = SIROperand::Register(r, SIRType::I64);
        inst.operands.push_back(argOperands[0]); // size
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64); // physical address
    }
    if (funcName == "حرر_فيزيائي" || funcName == "free_physical") {
        if (argResults.empty()) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_FREE_PHYS);
        inst.operands.push_back(argOperands[0]); // physical address
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "رحل_منطقة" || funcName == "map_region") {
        if (argResults.size() < 3) return BuildResult("", SIRType::VOID);
        SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_MAP_REGION);
        inst.operands.push_back(argOperands[0]); // physical address
        inst.operands.push_back(argOperands[1]); // virtual address
        inst.operands.push_back(argOperands[2]); // size
        if (argResults.size() > 3) inst.operands.push_back(argOperands[3]); // flags
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "تقرير_ذاكرة_نواة" || funcName == "mem_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_GET_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // =================================================================
    // القسم 16: بروتوكول الإقلاع الموحد UEFI / UEFI Boot Protocol
    // =================================================================

    // --- 16a. التهيئة والتحكم ---
    if (funcName == "uefi_تهيئة" || funcName == "uefi_initialize") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_INIT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_إنهاء_خدمات_إقلاع" || funcName == "uefi_exit_boot_services") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_EXIT_BOOT_SERVICES);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]); // map key
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_هل_مهيأ" || funcName == "uefi_is_initialized") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_IS_INITIALIZED);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_خدمات_إقلاع_منتهية" || funcName == "uefi_boot_services_exited") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_BS_EXITED);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_إعادة_تشغيل" || funcName == "uefi_reset_system") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_RESET_SYSTEM);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]); // reset type
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // --- 16b. إدارة الذاكرة ---
    if (funcName == "uefi_تخصيص_صفحات" || funcName == "uefi_allocate_pages") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_ALLOC_PAGES);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_تحرير_صفحات" || funcName == "uefi_free_pages") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FREE_PAGES);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_تخصيص_كتلة" || funcName == "uefi_allocate_pool") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_ALLOC_POOL);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_تحرير_كتلة" || funcName == "uefi_free_pool") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FREE_POOL);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_خريطة_ذاكرة" || funcName == "uefi_get_memory_map") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMORY_MAP);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }
    if (funcName == "uefi_مفتاح_خريطة" || funcName == "uefi_get_memory_map_key") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMMAP_KEY);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_ذاكرة_متاحة" || funcName == "uefi_total_memory") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_TOTAL_MEMORY);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // --- 16c. بروتوكول الرسوميات GOP ---
    if (funcName == "uefi_تهيئة_رسوميات" || funcName == "uefi_init_gop") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_INIT_GOP);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_تعيين_وضع_رسوميات" || funcName == "uefi_set_gop_mode") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_SET_GOP_MODE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_استعلام_وضع" || funcName == "uefi_query_gop_mode") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_QUERY_GOP_MODE);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }
    if (funcName == "uefi_عدد_أوضاع_رسوميات" || funcName == "uefi_gop_mode_count") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GOP_MODE_COUNT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_وضع_رسوميات_حالي" || funcName == "uefi_current_gop_mode") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_CURRENT_GOP_MODE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_عنوان_إطار" || funcName == "uefi_framebuffer_base") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_BASE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_حجم_إطار" || funcName == "uefi_framebuffer_size") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_SIZE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_ملء_شاشة" || funcName == "uefi_fill_screen") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FILL_SCREEN);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op); // r, g, b
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_رسم_مستطيل" || funcName == "uefi_draw_rect") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_DRAW_RECT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op); // x, y, w, h, r, g, b
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_blt" || funcName == "uefi_gop_blt") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GOP_BLT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // --- 16d. خدمات وقت التشغيل ---
    if (funcName == "uefi_الوقت" || funcName == "uefi_get_time") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_TIME);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }
    if (funcName == "uefi_تعيين_وقت" || funcName == "uefi_set_time") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_SET_TIME);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_قراءة_متغير" || funcName == "uefi_get_variable") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_VARIABLE);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]); // var name
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }
    if (funcName == "uefi_كتابة_متغير" || funcName == "uefi_set_variable") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_SET_VARIABLE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op); // name, value, [attrs]
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // --- 16e. نظام الملفات ---
    if (funcName == "uefi_فتح_وحدة_تخزين" || funcName == "uefi_open_volume") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_OPEN_VOLUME);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_فتح_ملف" || funcName == "uefi_open_file") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_OPEN_FILE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_قراءة_ملف" || funcName == "uefi_read_file") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_READ_FILE);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }
    if (funcName == "uefi_كتابة_ملف" || funcName == "uefi_write_file") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_WRITE_FILE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        for (auto& op : argOperands) inst.operands.push_back(op);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_إغلاق_ملف" || funcName == "uefi_close_file") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_CLOSE_FILE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_معلومات_ملف" || funcName == "uefi_file_info") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FILE_INFO);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // --- 16f. بروتوكولات ومعلومات ---
    if (funcName == "uefi_بحث_بروتوكول" || funcName == "uefi_locate_protocol") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_LOCATE_PROTOCOL);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_إصدار" || funcName == "uefi_revision") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_REVISION);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }
    if (funcName == "uefi_بائع" || funcName == "uefi_firmware_vendor") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_VENDOR);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }
    if (funcName == "uefi_إصدار_بائع" || funcName == "uefi_firmware_revision") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FW_REVISION);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "uefi_تقرير" || funcName == "uefi_report") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_REPORT);
        inst.result = SIROperand::Register(r, SIRType::STRING);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::STRING);
    }

    // ─── القسم 17: ACPI الموسّع / Extended ACPI ───

    // 17a: تهيئة وتفعيل
    if (funcName == "acpi_تهيئة" || funcName == "acpi_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_FULL);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "acpi_تهيئة_من_rsdp" || funcName == "acpi_init_from_rsdp") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_RSDP);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "acpi_تفعيل" || funcName == "acpi_enable") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_ENABLE);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "acpi_تعطيل" || funcName == "acpi_disable") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_DISABLE);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "acpi_هل_مهيأ" || funcName == "acpi_is_initialized") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_INITIALIZED);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "acpi_إصدار" || funcName == "acpi_version") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_VERSION);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // 17b: إدارة الطاقة
    if (funcName == "acpi_إعادة_تشغيل" || funcName == "acpi_reboot") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_REBOOT);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "acpi_نوم" || funcName == "acpi_sleep") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_SLEEP);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "acpi_تأخير" || funcName == "acpi_delay_us") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_DELAY_US);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // 17c: مؤقت ومعالجات
    if (funcName == "acpi_قراءة_مؤقت" || funcName == "acpi_read_pm_timer") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_READ_PM_TIMER);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "acpi_مؤقت_32بت" || funcName == "acpi_is_pm_timer_32bit") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_PM_32BIT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "acpi_عدد_معالجات" || funcName == "acpi_processor_count") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_PROCESSOR_COUNT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "acpi_عنوان_apic" || funcName == "acpi_local_apic_address") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_LOCAL_APIC_ADDR);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // 17d: PCIe ECAM
    if (funcName == "acpi_ecam_قاعدة" || funcName == "acpi_ecam_base") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_ECAM_BASE);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (argOperands.size() > 1) inst.operands.push_back(argOperands[1]);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // ─── القسم 18: APIC الموسّع / Extended APIC ───

    // 18a: استعلام ومعلومات
    if (funcName == "apic_مدعوم" || funcName == "apic_supported") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SUPPORTED);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "apic_x2_مدعوم" || funcName == "apic_x2_supported") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_X2_SUPPORTED);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "apic_معرّف" || funcName == "apic_id") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_ID);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "apic_عدد_io" || funcName == "apic_io_count") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_IO_COUNT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // 18b: مؤقت APIC
    if (funcName == "apic_تهيئة_مؤقت" || funcName == "apic_init_timer") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT_TIMER);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_بدء_مؤقت" || funcName == "apic_start_timer") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_START_TIMER);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_إيقاف_مؤقت" || funcName == "apic_stop_timer") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_STOP_TIMER);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_عداد_مؤقت" || funcName == "apic_timer_count") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_TIMER_COUNT);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }
    if (funcName == "apic_معايرة_مؤقت" || funcName == "apic_calibrate_timer") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_CALIBRATE);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // 18c: أولوية ومقاطعات
    if (funcName == "apic_أولوية_مهمة" || funcName == "apic_set_priority") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SET_PRIORITY);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_عطّل_pic" || funcName == "apic_disable_pic") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_DISABLE_PIC);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_قناع_irq" || funcName == "apic_mask_irq") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_MASK_IRQ);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_إلغاء_قناع_irq" || funcName == "apic_unmask_irq") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_UNMASK_IRQ);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_وجّه_irq" || funcName == "apic_route_irq") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_ROUTE_IRQ);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // 18d: IPI متقدم
    if (funcName == "apic_أرسل_للكل" || funcName == "apic_send_ipi_all") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_IPI_ALL);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_أرسل_init" || funcName == "apic_send_init") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_INIT_IPI);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_أرسل_sipi" || funcName == "apic_send_sipi") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_SIPI);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_انتظر_تسليم" || funcName == "apic_wait_delivery") {
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_WAIT_DELIVERY);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }
    if (funcName == "apic_تهيئة_io" || funcName == "apic_init_io") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT_IO);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::I64);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::I64);
    }

    // =====================================================================
    // (AR) نظام الواجهة الموحد — مصانع العناصر / Unified UI Widget Factories
    // (EN) Returns SadWidget* pointer for each widget creation function
    // =====================================================================

    // ─── عمود() / sad_column() ───
    if (funcName == "عمود" || funcName == "ui_column") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_COLUMN);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── صف() / sad_row() ───
    if (funcName == "صف" || funcName == "ui_row") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_ROW);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── مكدس() / sad_stack() ───
    if (funcName == "مكدس" || funcName == "ui_stack") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_STACK);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── حاوية() / sad_container() ───
    if (funcName == "حاوية" || funcName == "ui_container") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_CONTAINER);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── نص_عرض(نص) / sad_text(text) ───
    if (funcName == "نص_عرض" || funcName == "ui_text") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── نص_منسق(نص,حجم,أحمر,أخضر,أزرق,شفافية) / sad_text_styled(...) ───
    if (funcName == "نص_منسق" || funcName == "ui_text_styled") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT_STYLED);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── زر(عنوان,دالة_ضغط,بيانات) / sad_button(label,cb,data) ───
    if (funcName == "زر" || funcName == "ui_button") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_BUTTON);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── زر_نوع(عنوان,نوع,لون,دالة,بيانات) / sad_button_variant(...) ───
    if (funcName == "زر_نوع" || funcName == "ui_button_variant") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_BUTTON_VARIANT);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── زر_ايقونة(ايقونة,دالة,بيانات) / sad_icon_button(icon,cb,data) ───
    if (funcName == "زر_ايقونة" || funcName == "ui_icon_button") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_ICON_BUTTON);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── زر_عائم(ايقونة,لون_r,لون_g,لون_b,لون_a,دالة,بيانات) / sad_fab(...) ───
    if (funcName == "زر_عائم" || funcName == "ui_fab") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_FAB);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── حقل_نص(تلميح,دالة,بيانات) / sad_text_field(hint,cb,data) ───
    if (funcName == "حقل_نص" || funcName == "ui_text_field") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT_FIELD);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── مربع_تحقق(دالة,بيانات) / sad_checkbox(cb,data) ───
    if (funcName == "مربع_تحقق" || funcName == "ui_checkbox") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_CHECKBOX);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── مبدل(دالة,بيانات) / sad_switch_toggle(cb,data) ───
    if (funcName == "مبدل" || funcName == "ui_switch") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SWITCH);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── منزلق(حد_أدنى,حد_أقصى,دالة,بيانات) / sad_slider(min,max,cb,data) ───
    if (funcName == "منزلق" || funcName == "ui_slider") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SLIDER);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── بطاقة() / sad_card() ───
    if (funcName == "بطاقة" || funcName == "ui_card") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_CARD);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── هيكل() / sad_scaffold() ───
    if (funcName == "هيكل" || funcName == "ui_scaffold") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SCAFFOLD);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── شريط_تطبيق(عنوان) / sad_app_bar(title) ───
    if (funcName == "شريط_تطبيق" || funcName == "ui_app_bar") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_BAR);
        if (!argOperands.empty()) inst.operands.push_back(argOperands[0]);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── فاصل() / sad_spacer() ───
    if (funcName == "فاصل" || funcName == "ui_spacer") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SPACER);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── خط_فاصل() / sad_divider() ───
    if (funcName == "خط_فاصل" || funcName == "ui_divider") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_DIVIDER);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── حوار(عنوان,رسالة) / sad_dialog(title,msg) ───
    if (funcName == "حوار" || funcName == "ui_dialog") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_DIALOG);
        for (auto& a : argOperands) inst.operands.push_back(a);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // =====================================================================
    // (AR) إدارة شجرة العناصر / Tree Management
    // =====================================================================

    // ─── أضف_ابن(أب, ابن) / sad_add_child(parent, child) ───
    if (funcName == "أضف_ابن" || funcName == "ui_add_child") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] أضف_ابن تتطلب معاملين: الأب والابن" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_ADD_CHILD);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── أزل_ابن(أب, ابن) / sad_remove_child(parent, child) ───
    if (funcName == "أزل_ابن" || funcName == "ui_remove_child") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] أزل_ابن تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_REMOVE_CHILD);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── امسح_الأبناء(عنصر) / sad_clear_children(widget) ───
    if (funcName == "امسح_الأبناء" || funcName == "ui_clear_children") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] امسح_الأبناء تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_CLEAR_CHILDREN);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // =====================================================================
    // (AR) ضبط خصائص العناصر / Property Setters
    // =====================================================================

    // ─── عين_النص(عنصر, نص) / sad_set_text(widget, text) ───
    if (funcName == "عين_النص" || funcName == "ui_set_text") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_النص تتطلب معاملين: العنصر والنص" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_TEXT);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_الحجم(عنصر, عرض, ارتفاع) / sad_set_size(widget, w, h) ───
    if (funcName == "عين_الحجم" || funcName == "ui_set_size") {
        if (argResults.size() < 3) {
            std::cerr << "[خطأ] عين_الحجم تتطلب 3 معاملات" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_SIZE);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_المرونة(عنصر, مرونة) / sad_set_flex(widget, flex) ───
    if (funcName == "عين_المرونة" || funcName == "ui_set_flex") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_المرونة تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_FLEX);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_الخلفية(عنصر, أحمر, أخضر, أزرق, شفافية) / sad_set_background(w, r,g,b,a) ───
    if (funcName == "عين_الخلفية" || funcName == "ui_set_background") {
        if (argResults.size() < 5) {
            std::cerr << "[خطأ] عين_الخلفية تتطلب 5 معاملات: العنصر + 4 ألوان" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_BACKGROUND);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_اللون(عنصر, أحمر, أخضر, أزرق, شفافية) / sad_set_foreground(w, r,g,b,a) ───
    if (funcName == "عين_اللون" || funcName == "ui_set_foreground") {
        if (argResults.size() < 5) {
            std::cerr << "[خطأ] عين_اللون تتطلب 5 معاملات" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_FOREGROUND);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_التباعد(عنصر, تباعد) / sad_set_spacing(w, spacing) ───
    if (funcName == "عين_التباعد" || funcName == "ui_set_spacing") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_التباعد تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_SPACING);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_الحشوة(عنصر, فوق, يمين, تحت, يسار) / sad_set_padding(w, t,r,b,l) ───
    if (funcName == "عين_الحشوة" || funcName == "ui_set_padding") {
        if (argResults.size() < 5) {
            std::cerr << "[خطأ] عين_الحشوة تتطلب 5 معاملات" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_PADDING);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_المحاذاة(عنصر, رئيسية, فرعية) / sad_set_alignment(w, main, cross) ───
    if (funcName == "عين_المحاذاة" || funcName == "ui_set_alignment") {
        if (argResults.size() < 3) {
            std::cerr << "[خطأ] عين_المحاذاة تتطلب 3 معاملات" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_ALIGNMENT);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_الحدود(عنصر, سمك, أحمر, أخضر, أزرق, شفافية) / sad_set_border(...) ───
    if (funcName == "عين_الحدود" || funcName == "ui_set_border") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_الحدود تتطلب معاملين على الأقل" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_BORDER);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_الارتفاع(عنصر, ارتفاع) / sad_set_elevation(w, elev) ───
    if (funcName == "عين_الارتفاع" || funcName == "ui_set_elevation") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_الارتفاع تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_ELEVATION);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_الشفافية(عنصر, شفافية) / sad_set_opacity(w, opacity) ───
    if (funcName == "عين_الشفافية" || funcName == "ui_set_opacity") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_الشفافية تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_OPACITY);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── عين_الظهور(عنصر, مرئي) / sad_set_visibility(w, visible) ───
    if (funcName == "عين_الظهور" || funcName == "ui_set_visibility") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_الظهور تتطلب معاملين" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_VISIBILITY);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // =====================================================================
    // (AR) إدارة التطبيق / App Management
    // =====================================================================

    // ─── انشئ_تطبيق() / sad_app_create() ───
    if (funcName == "انشئ_تطبيق" || funcName == "ui_app_create") {
        std::string r = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_CREATE);
        inst.result = SIROperand::Register(r, SIRType::PTR);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(r, SIRType::PTR);
    }

    // ─── عين_الجذر(تطبيق, عنصر) / sad_app_set_root(app, widget) ───
    if (funcName == "عين_الجذر" || funcName == "ui_app_set_root") {
        if (argResults.size() < 2) {
            std::cerr << "[خطأ] عين_الجذر تتطلب معاملين: التطبيق والعنصر الجذر" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_SET_ROOT);
        inst.operands.push_back(argOperands[0]);
        inst.operands.push_back(argOperands[1]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── خطط(تطبيق, عرض, ارتفاع) / sad_app_layout(app, w, h) ───
    if (funcName == "خطط" || funcName == "ui_app_layout") {
        if (argResults.size() < 3) {
            std::cerr << "[خطأ] خطط تتطلب 3 معاملات" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_LAYOUT);
        for (auto& a : argOperands) inst.operands.push_back(a);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── ارسم(تطبيق) / sad_app_render(app) ───
    if (funcName == "ارسم" || funcName == "ui_app_render") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] ارسم تتطلب معامل واحد: التطبيق" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_RENDER);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── دمر_تطبيق(تطبيق) / sad_app_destroy(app) ───
    if (funcName == "دمر_تطبيق" || funcName == "ui_app_destroy") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دمر_تطبيق تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_DESTROY);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // ─── دمر_عنصر(عنصر) / sad_widget_destroy(widget) ───
    if (funcName == "دمر_عنصر" || funcName == "ui_widget_destroy") {
        if (argResults.empty()) {
            std::cerr << "[خطأ] دمر_عنصر تتطلب معامل واحد" << std::endl;
            return BuildResult("", SIRType::VOID);
        }
        SIRInstruction inst(SIROpcode::BUILTIN_UI_WIDGET_DESTROY);
        inst.operands.push_back(argOperands[0]);
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult("", SIRType::VOID);
    }

    // Not a system builtin
    return std::nullopt;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
