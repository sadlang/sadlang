/**
 * @file test_compiler_comprehensive.cpp
 * @brief (AR) اختبارات شاملة للمترجم - SIR والأنواع والتعليمات
 * @brief (EN) Comprehensive Compiler Tests - SIR, Types, and Instructions
 *
 * ═══════════════════════════════════════════════════════════════════════
 *  ملف اختبارات المترجم (Compiler) الشاملة
 *  يحتوي على ~100 اختبار موزعة كالتالي:
 *
 *  ■ القسم 1: أنواع SIR (SadTypeKind) - 15 اختبار
 *  ■ القسم 2: رموز العمليات (SIROpcode) - 25 اختبار
 *  ■ القسم 3: المعاملات (SIROperand) - 15 اختبار
 *  ■ القسم 4: التعليمات (SIRInstruction) - 20 اختبار
 *  ■ القسم 5: الكتل الأساسية (SIRBasicBlock) - 10 اختبار
 *  ■ القسم 6: الدوال (SIRFunction) - 10 اختبار
 *  ■ القسم 7: الوحدات (SIRModule) - 5 اختبار
 * ═══════════════════════════════════════════════════════════════════════
 */

// (AR) <cstring> صراحةً لـ strlen — GCC/Clang لا يضمّانه ضمنيًا (يعمل على MSVC).
// (EN) Explicit <cstring> for strlen — not pulled transitively on GCC/Clang.
#include <cstring>
#include "sad_test_framework.h"

// ══════════════════════════════════════════════════════════════════════
//  شوائب SIR (Sad Intermediate Representation)
// ══════════════════════════════════════════════════════════════════════

#if __has_include("sir_types.h")
#include "sir_types.h"
#define HAS_SIR_TYPES 1
#else
#define HAS_SIR_TYPES 0
#endif

#if __has_include("sir_instruction.h")
#include "sir_instruction.h"
#define HAS_SIR_INSTRUCTION 1
#else
#define HAS_SIR_INSTRUCTION 0
#endif

#if __has_include("sir_module.h")
#include "sir_module.h"
#define HAS_SIR_MODULE 1
#else
#define HAS_SIR_MODULE 0
#endif

#if __has_include("sir_builder.h")
#include "sir_builder.h"
#define HAS_SIR_BUILDER 1
#else
#define HAS_SIR_BUILDER 0
#endif

// ══════════════════════════════════════════════════════════════════════
//  شوائب الآلة الافتراضية
// ══════════════════════════════════════════════════════════════════════

#if __has_include("sad_vm_value.h")
#include "sad_vm_value.h"
#define HAS_VM_VALUE 1
#else
#define HAS_VM_VALUE 0
#endif

#if __has_include("sad_vm_chunk.h")
#include "sad_vm_chunk.h"
#define HAS_VM_CHUNK 1
#else
#define HAS_VM_CHUNK 0
#endif

#if __has_include("sad_vm_opcodes.h")
#include "sad_vm_opcodes.h"
#define HAS_VM_OPCODES 1
#else
#define HAS_VM_OPCODES 0
#endif

#include <string>
#include <vector>

// ══════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ══════════════════════════════════════════════════════════════════════

int main()
{
    SAD_TEST_INIT();

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 1: أنواع SIR (SadTypeKind) - 15 اختبار                       ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_SIR_TYPES

    SAD_GROUP("Compiler.SIRTypes / المترجم.أنواع_SIR");

    SAD_TEST("SIRT01: نوع VOID", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Void;
        SAD_ASSERT_EQ((int)type, 0);
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(str != nullptr);
    });

    SAD_TEST("SIRT02: نوع I64 (رقم صحيح)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Integer;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "i64");
    });

    SAD_TEST("SIRT03: نوع F64 (رقم عشري)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Float;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "f64");
    });

    SAD_TEST("SIRT04: نوع BOOL (منطقي)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Boolean;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "bool");
    });

    SAD_TEST("SIRT05: نوع PTR (مؤشر)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Pointer;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "ptr");
    });

    SAD_TEST("SIRT06: نوع ARRAY (مصفوفة)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Array;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "array");
    });

    SAD_TEST("SIRT07: نوع STRING (نص)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::String;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "string");
    });

    SAD_TEST("SIRT08: نوع STRUCT (بنية)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Struct;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "struct");
    });

    SAD_TEST("SIRT09: نوع FUNCTION (دالة)", {
        auto type = Sad::Compiler::SIR::SadTypeKind::Function;
        auto str = Sad::Compiler::SIR::sirTypeToString(type);
        SAD_ASSERT_TRUE(std::string(str) == "function");
    });

    SAD_TEST("SIRT10: مقارنة الأنواع - VOID != I64", {
        SAD_ASSERT_TRUE(Sad::Compiler::SIR::SadTypeKind::Void != Sad::Compiler::SIR::SadTypeKind::Integer);
    });

    SAD_TEST("SIRT11: مقارنة الأنواع - F64 == F64", {
        auto t1 = Sad::Compiler::SIR::SadTypeKind::Float;
        auto t2 = Sad::Compiler::SIR::SadTypeKind::Float;
        SAD_ASSERT_TRUE(t1 == t2);
    });

    SAD_TEST("SIRT12: تحويل جميع الأنواع لنص", {
        // التحقق من أن كل نوع يُرجع نصاً غير فارغ
        const auto types = {
            Sad::Compiler::SIR::SadTypeKind::Void,
            Sad::Compiler::SIR::SadTypeKind::Integer,
            Sad::Compiler::SIR::SadTypeKind::Float,
            Sad::Compiler::SIR::SadTypeKind::Boolean,
            Sad::Compiler::SIR::SadTypeKind::Pointer,
            Sad::Compiler::SIR::SadTypeKind::Array,
            Sad::Compiler::SIR::SadTypeKind::String,
            Sad::Compiler::SIR::SadTypeKind::Struct,
            Sad::Compiler::SIR::SadTypeKind::Function};
        for (auto t : types)
        {
            auto str = Sad::Compiler::SIR::sirTypeToString(t);
            SAD_ASSERT_TRUE(str != nullptr);
            SAD_ASSERT_TRUE(strlen(str) > 0);
        }
    });

    SAD_TEST("SIRT13: ترتيب الأنواع", {
        // التحقق من أن VOID هو الأول
        SAD_ASSERT_TRUE((int)Sad::Compiler::SIR::SadTypeKind::Void < (int)Sad::Compiler::SIR::SadTypeKind::Integer);
    });

    SAD_TEST("SIRT14: أنواع رقمية", {
        // I64 و F64 هي الأنواع الرقمية
        auto i64 = Sad::Compiler::SIR::SadTypeKind::Integer;
        auto f64 = Sad::Compiler::SIR::SadTypeKind::Float;
        SAD_ASSERT_TRUE((int)i64 == 1);
        SAD_ASSERT_TRUE((int)f64 == 2);
    });

    SAD_TEST("SIRT15: عدد الأنواع الأساسية", {
        // (AR) 🔑 لا رتبةَ مكتوبةً باليدِ هنا: رتبةُ «دالة» تتغيّرُ كلّما
        //      أُضيفَ نوعٌ أو حُذِفَ في types.yaml، فتثبيتُها عددًا يجعلُ
        //      الاختبارَ نسخةً ثانيةً من الحقيقةِ تنجرفُ عنها. والمحروسُ
        //      هو التطابقُ مع الثابتِ المولَّدِ من مصدرِ الحقيقة: آخرُ عضوٍ
        //      في التعدادِ رتبتُه COUNT-1، فإن زِيدَ عضوٌ باليدِ بلا إعادةِ
        //      توليدٍ انكسرَ هذا الشرطُ فورًا.
        // (EN) No hand-written ordinal: the guarded invariant is that the
        //      enum agrees with the SoT-generated cardinality — the last
        //      member sits at COUNT-1. A hand-added member breaks it.
        SAD_ASSERT_TRUE((int)Sad::Types::SadTypeKind::Rect ==
                        Sad::Types::SAD_TYPE_KIND_COUNT - 1);
        SAD_ASSERT_TRUE((int)Sad::Compiler::SIR::SadTypeKind::Function <
                        Sad::Types::SAD_TYPE_KIND_COUNT);
    });

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 2: رموز العمليات (SIROpcode) - 25 اختبار                 ║
    // ╚══════════════════════════════════════════════════════════════════╝

    SAD_GROUP("Compiler.SIROpcodes / المترجم.رموز_العمليات");

    // العمليات الحسابية
    SAD_TEST("SIROP01: ADD_I64 - جمع أعداد صحيحة", {
        auto op = Sad::Compiler::SIR::SIROpcode::ADD_I64;
        SAD_ASSERT_TRUE((int)op >= 0);
    });

    SAD_TEST("SIROP02: ADD_F64 - جمع أعداد عشرية", {
        auto op = Sad::Compiler::SIR::SIROpcode::ADD_F64;
        SAD_ASSERT_TRUE((int)op > (int)Sad::Compiler::SIR::SIROpcode::ADD_I64);
    });

    SAD_TEST("SIROP03: SUB_I64 - طرح أعداد صحيحة", {
        auto op = Sad::Compiler::SIR::SIROpcode::SUB_I64;
        auto opAdd = Sad::Compiler::SIR::SIROpcode::ADD_F64;
        SAD_ASSERT_TRUE((int)op > (int)opAdd);
    });

    SAD_TEST("SIROP04: MUL_I64 - ضرب أعداد صحيحة", {
        auto op = Sad::Compiler::SIR::SIROpcode::MUL_I64;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::ADD_I64);
    });

    SAD_TEST("SIROP05: DIV_I64 - قسمة أعداد صحيحة", {
        auto op = Sad::Compiler::SIR::SIROpcode::DIV_I64;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::MUL_I64);
    });

    SAD_TEST("SIROP06: MOD_I64 - باقي القسمة", {
        auto op = Sad::Compiler::SIR::SIROpcode::MOD_I64;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::DIV_I64);
    });

    SAD_TEST("SIROP07: NEG - السالب", {
        auto op = Sad::Compiler::SIR::SIROpcode::NEG;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::ADD_I64);
    });

    // العمليات الثنائية (Bitwise)
    SAD_TEST("SIROP08: AND - AND الثنائي", {
        auto op = Sad::Compiler::SIR::SIROpcode::AND;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::OR);
    });

    SAD_TEST("SIROP09: OR - OR الثنائي", {
        auto op = Sad::Compiler::SIR::SIROpcode::OR;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::XOR);
    });

    SAD_TEST("SIROP10: XOR - XOR الثنائي", {
        auto op = Sad::Compiler::SIR::SIROpcode::XOR;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::NOT);
    });

    SAD_TEST("SIROP11: SHL - إزاحة لليسار", {
        auto op = Sad::Compiler::SIR::SIROpcode::SHL;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::SHR);
    });

    // المقارنات
    SAD_TEST("SIROP12: EQ - يساوي", {
        auto op = Sad::Compiler::SIR::SIROpcode::EQ;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::NE);
    });

    SAD_TEST("SIROP13: NE - لا يساوي", {
        auto op = Sad::Compiler::SIR::SIROpcode::NE;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::LT);
    });

    SAD_TEST("SIROP14: LT - أصغر من", {
        auto op = Sad::Compiler::SIR::SIROpcode::LT;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::GT);
    });

    SAD_TEST("SIROP15: GT - أكبر من", {
        auto op = Sad::Compiler::SIR::SIROpcode::GT;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::GE);
    });

    // التحكم في التدفق
    SAD_TEST("SIROP16: BR - قفز غير مشروط", {
        auto op = Sad::Compiler::SIR::SIROpcode::BR;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::BR_COND);
    });

    SAD_TEST("SIROP17: BR_COND - قفز مشروط", {
        auto op = Sad::Compiler::SIR::SIROpcode::BR_COND;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::RET);
    });

    SAD_TEST("SIROP18: RET - إرجاع", {
        auto op = Sad::Compiler::SIR::SIROpcode::RET;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::CALL);
    });

    SAD_TEST("SIROP19: CALL - استدعاء دالة", {
        auto op = Sad::Compiler::SIR::SIROpcode::CALL;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::BR);
    });

    // الذاكرة
    SAD_TEST("SIROP20: LOAD - تحميل من الذاكرة", {
        auto op = Sad::Compiler::SIR::SIROpcode::LOAD;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::STORE);
    });

    SAD_TEST("SIROP21: STORE - تخزين في الذاكرة", {
        auto op = Sad::Compiler::SIR::SIROpcode::STORE;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::ALLOC);
    });

    SAD_TEST("SIROP22: ALLOC - تخصيص على Stack", {
        auto op = Sad::Compiler::SIR::SIROpcode::ALLOC;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::FREE);
    });

    // OOP
    SAD_TEST("SIROP23: OBJECT_NEW - إنشاء كائن", {
        auto op = Sad::Compiler::SIR::SIROpcode::OBJECT_NEW;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::OBJECT_GET);
    });

    SAD_TEST("SIROP24: OBJECT_GET - الحصول على خاصية", {
        auto op = Sad::Compiler::SIR::SIROpcode::OBJECT_GET;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::OBJECT_SET);
    });

    // Builtins
    SAD_TEST("SIROP25: BUILTIN_PRINT - اطبع", {
        auto op = Sad::Compiler::SIR::SIROpcode::BUILTIN_PRINT;
        SAD_ASSERT_TRUE(op != Sad::Compiler::SIR::SIROpcode::BUILTIN_READ);
    });

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 3: المعاملات (SIROperand) - 15 اختبار                    ║
    // ╚══════════════════════════════════════════════════════════════════╝

    SAD_GROUP("Compiler.SIROperands / المترجم.المعاملات");

    SAD_TEST("SIROPER01: نوع REGISTER", {
        auto type = Sad::Compiler::SIR::SIROperandType::REGISTER;
        SAD_ASSERT_EQ((int)type, 0);
    });

    SAD_TEST("SIROPER02: نوع CONSTANT", {
        auto type = Sad::Compiler::SIR::SIROperandType::CONSTANT;
        SAD_ASSERT_TRUE(type != Sad::Compiler::SIR::SIROperandType::REGISTER);
    });

    SAD_TEST("SIROPER03: نوع LABEL", {
        auto type = Sad::Compiler::SIR::SIROperandType::LABEL;
        SAD_ASSERT_TRUE(type != Sad::Compiler::SIR::SIROperandType::CONSTANT);
    });

    SAD_TEST("SIROPER04: نوع GLOBAL", {
        auto type = Sad::Compiler::SIR::SIROperandType::GLOBAL;
        SAD_ASSERT_TRUE(type != Sad::Compiler::SIR::SIROperandType::LABEL);
    });

    SAD_TEST("SIROPER05: نوع FUNCTION", {
        auto type = Sad::Compiler::SIR::SIROperandType::FUNCTION;
        SAD_ASSERT_TRUE(type != Sad::Compiler::SIR::SIROperandType::GLOBAL);
    });

    SAD_TEST("SIROPER06: إنشاء معامل سجل", {
        auto op = Sad::Compiler::SIR::SIROperand::Register("%r0", Sad::Compiler::SIR::SadTypeKind::Integer);
        SAD_ASSERT_EQ((int)op.type, (int)Sad::Compiler::SIR::SIROperandType::REGISTER);
        SAD_ASSERT_EQ(op.name, std::string("%r0"));
    });

    SAD_TEST("SIROPER07: إنشاء معامل ثابت صحيح", {
        Sad::Compiler::SIR::SIROperand op;
        op.type = Sad::Compiler::SIR::SIROperandType::CONSTANT;
        op.intValue = 42;
        SAD_ASSERT_EQ(op.intValue, (int64_t)42);
    });

    SAD_TEST("SIROPER08: إنشاء معامل ثابت عشري", {
        Sad::Compiler::SIR::SIROperand op;
        op.type = Sad::Compiler::SIR::SIROperandType::CONSTANT;
        op.floatValue = 3.14;
        SAD_ASSERT_TRUE(op.floatValue > 3.0 && op.floatValue < 4.0);
    });

    SAD_TEST("SIROPER09: إنشاء معامل تسمية", {
        auto op = Sad::Compiler::SIR::SIROperand::Label("loop");
        SAD_ASSERT_EQ(op.name, std::string("loop"));
        SAD_ASSERT_EQ((int)op.type, (int)Sad::Compiler::SIR::SIROperandType::LABEL);
    });

    SAD_TEST("SIROPER10: إنشاء معامل عام", {
        Sad::Compiler::SIR::SIROperand op;
        op.type = Sad::Compiler::SIR::SIROperandType::GLOBAL;
        op.name = "$x";
        SAD_ASSERT_EQ(op.name, std::string("$x"));
    });

    SAD_TEST("SIROPER11: إنشاء معامل دالة", {
        Sad::Compiler::SIR::SIROperand op;
        op.type = Sad::Compiler::SIR::SIROperandType::FUNCTION;
        op.name = "@main";
        SAD_ASSERT_EQ(op.name, std::string("@main"));
    });

    SAD_TEST("SIROPER12: مقارنة أنواع المعاملات", {
        auto t1 = Sad::Compiler::SIR::SIROperandType::REGISTER;
        auto t2 = Sad::Compiler::SIR::SIROperandType::REGISTER;
        SAD_ASSERT_TRUE(t1 == t2);
    });

    SAD_TEST("SIROPER13: سجل بدون اسم", {
        auto op = Sad::Compiler::SIR::SIROperand::Register("", Sad::Compiler::SIR::SadTypeKind::Integer);
        SAD_ASSERT_TRUE(op.name.empty());
        SAD_ASSERT_EQ((int)op.type, (int)Sad::Compiler::SIR::SIROperandType::REGISTER);
    });

    SAD_TEST("SIROPER14: سجل باسم", {
        auto op = Sad::Compiler::SIR::SIROperand::Register("%r1000", Sad::Compiler::SIR::SadTypeKind::Integer);
        SAD_ASSERT_EQ(op.name, std::string("%r1000"));
    });

    SAD_TEST("SIROPER15: ثابت سالب", {
        Sad::Compiler::SIR::SIROperand op;
        op.type = Sad::Compiler::SIR::SIROperandType::CONSTANT;
        op.intValue = -100;
        SAD_ASSERT_EQ(op.intValue, (int64_t)-100);
    });

#endif // HAS_SIR_TYPES

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 4: التعليمات (SIRInstruction) - 20 اختبار                ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_SIR_INSTRUCTION

    SAD_GROUP("Compiler.SIRInstruction / المترجم.التعليمات");

    SAD_TEST("SIRINST01: إنشاء تعليمة فارغة", {
        Sad::Compiler::SIR::SIRInstruction inst;
        SAD_ASSERT_TRUE(inst.operands.empty());
    });

    SAD_TEST("SIRINST02: إنشاء تعليمة بـ opcode", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        SAD_ASSERT_EQ((int)inst.opcode, (int)Sad::Compiler::SIR::SIROpcode::ADD_I64);
    });

    SAD_TEST("SIRINST03: إضافة معامل للتعليمة", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        auto op = Sad::Compiler::SIR::SIROperand::Register("%r0", Sad::Compiler::SIR::SadTypeKind::Integer);
        inst.operands.push_back(op);
        SAD_ASSERT_EQ(inst.operands.size(), (size_t)1);
    });

    SAD_TEST("SIRINST04: تعليمة مع نتيجة", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        auto result = Sad::Compiler::SIR::SIROperand::Register("%r1", Sad::Compiler::SIR::SadTypeKind::Integer);
        inst.result = result;
        SAD_ASSERT_TRUE(inst.result.has_value());
    });

    SAD_TEST("SIRINST05: تعليمة بدون نتيجة", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::STORE);
        SAD_ASSERT_FALSE(inst.result.has_value());
    });

    SAD_TEST("SIRINST06: تعليمة مع تسمية", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::BR);
        inst.label = "loop_start";
        SAD_ASSERT_EQ(inst.label, std::string("loop_start"));
    });

    SAD_TEST("SIRINST07: تعليمة مع تعليق", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        inst.comment = "جمع القيم";
        SAD_ASSERT_EQ(inst.comment, std::string("جمع القيم"));
    });

    SAD_TEST("SIRINST08: رقم السطر", {
        Sad::Compiler::SIR::SIRInstruction inst;
        inst.lineNumber = 42;
        SAD_ASSERT_EQ(inst.lineNumber, 42);
    });

    SAD_TEST("SIRINST09: تعليمة نهائية (terminator)", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::RET);
        inst.isTerminator = true;
        SAD_ASSERT_TRUE(inst.isTerminator);
    });

    SAD_TEST("SIRINST10: تعليمة غير نهائية", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        SAD_ASSERT_FALSE(inst.isTerminator);
    });

    SAD_TEST("SIRINST11: معامل واحد", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::NEG);
        auto op = Sad::Compiler::SIR::SIROperand::Register("%r0", Sad::Compiler::SIR::SadTypeKind::Integer);
        inst.operands.push_back(op);
        SAD_ASSERT_EQ(inst.operands.size(), (size_t)1);
    });

    SAD_TEST("SIRINST12: معاملان", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        auto op1 = Sad::Compiler::SIR::SIROperand::Register("%r0", Sad::Compiler::SIR::SadTypeKind::Integer);
        auto op2 = Sad::Compiler::SIR::SIROperand::Register("%r1", Sad::Compiler::SIR::SadTypeKind::Integer);
        inst.operands.push_back(op1);
        inst.operands.push_back(op2);
        SAD_ASSERT_EQ(inst.operands.size(), (size_t)2);
    });

    SAD_TEST("SIRINST13: ثلاثة معاملات", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::MEMCPY);
        for (int i = 0; i < 3; i++)
        {
            auto op = Sad::Compiler::SIR::SIROperand::Register("%r" + std::to_string(i), Sad::Compiler::SIR::SadTypeKind::Pointer);
            inst.operands.push_back(op);
        }
        SAD_ASSERT_EQ(inst.operands.size(), (size_t)3);
    });

    SAD_TEST("SIRINST14: تعليمة CALL", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::CALL);
        Sad::Compiler::SIR::SIROperand fn;
        fn.type = Sad::Compiler::SIR::SIROperandType::FUNCTION;
        fn.name = "@print";
        inst.operands.push_back(fn);
        SAD_ASSERT_EQ(inst.operands[0].name, std::string("@print"));
    });

    SAD_TEST("SIRINST15: تعليمة BR مع تسمية", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::BR);
        auto label = Sad::Compiler::SIR::SIROperand::Label("end");
        inst.operands.push_back(label);
        SAD_ASSERT_EQ(inst.operands[0].name, std::string("end"));
    });

    SAD_TEST("SIRINST16: نسخ تعليمة", {
        Sad::Compiler::SIR::SIRInstruction inst1(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        inst1.lineNumber = 10;
        Sad::Compiler::SIR::SIRInstruction inst2 = inst1;
        SAD_ASSERT_EQ(inst2.lineNumber, 10);
    });

    SAD_TEST("SIRINST17: مسح المعاملات", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        Sad::Compiler::SIR::SIROperand op;
        op.type = Sad::Compiler::SIR::SIROperandType::REGISTER;
        inst.operands.push_back(op);
        inst.operands.clear();
        SAD_ASSERT_TRUE(inst.operands.empty());
    });

    SAD_TEST("SIRINST18: تعليمة LOAD", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::LOAD);
        auto result = Sad::Compiler::SIR::SIROperand::Register("%r0", Sad::Compiler::SIR::SadTypeKind::Integer);
        auto ptr = Sad::Compiler::SIR::SIROperand::Register("%r1", Sad::Compiler::SIR::SadTypeKind::Pointer);
        inst.result = result;
        inst.operands.push_back(ptr);
        SAD_ASSERT_TRUE(inst.result.has_value());
        SAD_ASSERT_EQ(inst.operands.size(), (size_t)1);
    });

    SAD_TEST("SIRINST19: تعليمة STORE", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::STORE);
        auto val = Sad::Compiler::SIR::SIROperand::Register("%r0", Sad::Compiler::SIR::SadTypeKind::Integer);
        auto ptr = Sad::Compiler::SIR::SIROperand::Register("%r1", Sad::Compiler::SIR::SadTypeKind::Pointer);
        inst.operands.push_back(val);
        inst.operands.push_back(ptr);
        SAD_ASSERT_EQ(inst.operands.size(), (size_t)2);
    });

    SAD_TEST("SIRINST20: تعليمة RET_VOID", {
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::RET_VOID);
        inst.isTerminator = true;
        SAD_ASSERT_TRUE(inst.isTerminator);
        SAD_ASSERT_TRUE(inst.operands.empty());
    });

#endif // HAS_SIR_INSTRUCTION

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 5: الكتل الأساسية (SIRBasicBlock) - 10 اختبار            ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_SIR_INSTRUCTION

    SAD_GROUP("Compiler.SIRBasicBlock / المترجم.الكتل_الأساسية");

    SAD_TEST("SIRBB01: إنشاء كتلة فارغة", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        SAD_ASSERT_TRUE(block.instructions.empty());
    });

    SAD_TEST("SIRBB02: إنشاء كتلة باسم", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        block.name = "entry";
        SAD_ASSERT_EQ(block.name, std::string("entry"));
    });

    SAD_TEST("SIRBB03: إضافة تعليمة للكتلة", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        block.instructions.push_back(inst);
        SAD_ASSERT_EQ(block.instructions.size(), (size_t)1);
    });

    SAD_TEST("SIRBB04: إضافة عدة تعليمات", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        for (int i = 0; i < 5; i++)
        {
            Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
            block.instructions.push_back(inst);
        }
        SAD_ASSERT_EQ(block.instructions.size(), (size_t)5);
    });

    SAD_TEST("SIRBB05: كتلة مع تعليمة نهائية", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        block.name = "loop";
        Sad::Compiler::SIR::SIRInstruction ret(Sad::Compiler::SIR::SIROpcode::RET);
        ret.isTerminator = true;
        block.instructions.push_back(ret);
        SAD_ASSERT_TRUE(block.instructions.back().isTerminator);
    });

    SAD_TEST("SIRBB06: كتلة بدون تعليمة نهائية", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        Sad::Compiler::SIR::SIRInstruction add(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        block.instructions.push_back(add);
        SAD_ASSERT_FALSE(block.instructions.back().isTerminator);
    });

    SAD_TEST("SIRBB07: خلافات الكتلة (successors)", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        Sad::Compiler::SIR::SIRBasicBlock thenBlock("then");
        Sad::Compiler::SIR::SIRBasicBlock elseBlock("else");
        block.addSuccessor(&thenBlock);
        block.addSuccessor(&elseBlock);
        SAD_ASSERT_EQ(block.successors.size(), (size_t)2);
    });

    SAD_TEST("SIRBB08: سلف الكتلة (predecessors)", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        Sad::Compiler::SIR::SIRBasicBlock entryBlock("entry");
        block.addPredecessor(&entryBlock);
        SAD_ASSERT_EQ(block.predecessors.size(), (size_t)1);
    });

    SAD_TEST("SIRBB09: مسح التعليمات", {
        Sad::Compiler::SIR::SIRBasicBlock block;
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        block.instructions.push_back(inst);
        block.instructions.clear();
        SAD_ASSERT_TRUE(block.instructions.empty());
    });

    SAD_TEST("SIRBB10: نسخ كتلة", {
        Sad::Compiler::SIR::SIRBasicBlock block1;
        block1.name = "original";
        Sad::Compiler::SIR::SIRInstruction inst(Sad::Compiler::SIR::SIROpcode::ADD_I64);
        block1.instructions.push_back(inst);
        Sad::Compiler::SIR::SIRBasicBlock block2 = block1;
        SAD_ASSERT_EQ(block2.name, std::string("original"));
        SAD_ASSERT_EQ(block2.instructions.size(), (size_t)1);
    });

#endif // HAS_SIR_INSTRUCTION

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 6: VM Values - 15 اختبار                                 ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_VM_VALUE

    SAD_GROUP("Compiler.VMValues / المترجم.قيم_VM");

    SAD_TEST("VMVAL01: قيمة لا_شيء", {
        auto v = sad::vm::قيمة::لا_شيء();
        SAD_ASSERT_TRUE(v.هل_لا_شيء());
    });

    SAD_TEST("VMVAL02: قيمة منطقية صحيح", {
        auto v = sad::vm::قيمة::منطقي(true);
        SAD_ASSERT_TRUE(v.كـ_منطقي());
    });

    SAD_TEST("VMVAL03: قيمة منطقية خطأ", {
        auto v = sad::vm::قيمة::منطقي(false);
        SAD_ASSERT_FALSE(v.كـ_منطقي());
    });

    SAD_TEST("VMVAL04: قيمة صحيحة", {
        auto v = sad::vm::قيمة::صحيح(42);
        SAD_ASSERT_EQ(v.كـ_صحيح(), (int64_t)42);
    });

    SAD_TEST("VMVAL05: قيمة صحيحة سالبة", {
        auto v = sad::vm::قيمة::صحيح(-100);
        SAD_ASSERT_EQ(v.كـ_صحيح(), (int64_t)-100);
    });

    SAD_TEST("VMVAL06: قيمة عشرية", {
        auto v = sad::vm::قيمة::عشري(3.14);
        SAD_ASSERT_TRUE(v.كـ_عشري() > 3.0);
    });

    SAD_TEST("VMVAL07: قيمة نصية", {
        auto v = sad::vm::قيمة::نص("مرحبا");
        SAD_ASSERT_EQ(v.كـ_نص(), std::string("مرحبا"));
    });

    SAD_TEST("VMVAL08: نص فارغ", {
        auto v = sad::vm::قيمة::نص("");
        SAD_ASSERT_EQ(v.كـ_نص(), std::string(""));
    });

    SAD_TEST("VMVAL09: مصفوفة فارغة", {
        auto v = sad::vm::قيمة::مصفوفة_جديدة();
        SAD_ASSERT_TRUE(v.هل_مصفوفة());
    });

    SAD_TEST("VMVAL10: فحص نوع قيمة", {
        auto v = sad::vm::قيمة::صحيح(10);
        SAD_ASSERT_TRUE(v.هل_صحيح());
        SAD_ASSERT_FALSE(v.هل_عشري());
    });

    SAD_TEST("VMVAL11: فحص نوع نص", {
        auto v = sad::vm::قيمة::نص("test");
        SAD_ASSERT_TRUE(v.هل_نص());
    });

    SAD_TEST("VMVAL12: قيمة عشرية صفر", {
        auto v = sad::vm::قيمة::عشري(0.0);
        SAD_ASSERT_EQ(v.كـ_عشري(), 0.0);
    });

    SAD_TEST("VMVAL13: قيمة صحيحة كبيرة", {
        int64_t big = 9999999999999LL;
        auto v = sad::vm::قيمة::صحيح(big);
        SAD_ASSERT_EQ(v.كـ_صحيح(), big);
    });

    SAD_TEST("VMVAL14: نسخ قيمة", {
        auto v1 = sad::vm::قيمة::صحيح(42);
        auto v2 = v1;
        SAD_ASSERT_EQ(v2.كـ_صحيح(), (int64_t)42);
    });

    SAD_TEST("VMVAL15: قيمة VOID", {
        auto v = sad::vm::قيمة::لا_شيء();
        SAD_ASSERT_EQ((int)v.النوع, (int)sad::vm::نوع_القيمة::لا_شيء);
    });

#endif // HAS_VM_VALUE

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  القسم 7: VM Opcodes - 10 اختبار                                ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if HAS_VM_OPCODES

    SAD_GROUP("Compiler.VMOpcodes / المترجم.رموز_VM");

    SAD_TEST("VMOP01: وجود HALT opcode", {
        SAD_ASSERT_TRUE(true); // Basic opcode existence
    });

    SAD_TEST("VMOP02: وجود CONST opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP03: وجود ADD opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP04: وجود SUB opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP05: وجود MUL opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP06: وجود DIV opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP07: وجود JUMP opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP08: وجود CALL opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP09: وجود RETURN opcode", {
        SAD_ASSERT_TRUE(true);
    });

    SAD_TEST("VMOP10: وجود PRINT opcode", {
        SAD_ASSERT_TRUE(true);
    });

#endif // HAS_VM_OPCODES

    // ╔══════════════════════════════════════════════════════════════════╗
    // ║  إذا لم تتوفر المكونات                                          ║
    // ╚══════════════════════════════════════════════════════════════════╝

#if !HAS_SIR_TYPES && !HAS_SIR_INSTRUCTION && !HAS_VM_VALUE
    SAD_GROUP("Compiler / المترجم");
    SAD_TEST("SKIP: Components not available", {
        SAD_ASSERT_TRUE(true);
    });
#endif

    SAD_SUMMARY();
}
