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
    
    // 12. وقت_الآن / timestamp - الحصول على الوقت الحالي
    if (funcName == "وقت_الآن" || funcName == "وقت_الان" || funcName == "timestamp" || funcName == "now") {
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

    // Not a system builtin
    return std::nullopt;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
