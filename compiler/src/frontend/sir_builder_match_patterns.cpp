// ============================================================================
// sir_builder_match_patterns.cpp
// ============================================================================
// (AR) معالجة أنماط match — مستخرج من sir_builder_control_match.cpp (CW-05)
//      Wildcard/Variable/Literal/Range/OR/List/Struct/Enum/Binding patterns
// (EN) Match pattern condition builders — extracted from sir_builder_control_match.cpp
// ============================================================================

#include <string>
#include <iostream>
#include "sir_builder.h"
#include "pattern_nodes.h"
#include "sir_constants.h"

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::string SIRBuilder::buildMatchPatternCondition(
                const AST::PatternNode *pattern,
                const std::string &matchValueReg,
                SadTypeKind matchValueType,
                size_t caseIndex,
                std::vector<MatchDeferredField> &deferredExtractions)
            {
                std::string condReg;
                size_t i = caseIndex;

                if (dynamic_cast<const Sad::AST::WildcardPattern *>(pattern))
                {
                    // (AR) النمط الشامل _ - دائماً true
                    // (EN) Wildcard _ - always true
                    condReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                    SIROperand trueOp = SIROperand::ConstantBool(true);
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = resultOp;
                    moveInst.operands = {trueOp};
                    currentBlock_->instructions.push_back(moveInst);

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i << " is WildcardPattern" << std::endl;
#endif
                }
                else if (auto *varPat = dynamic_cast<const Sad::AST::VariablePattern *>(pattern))
                {
                    // (AR) نمط متغير - دائماً true ويربط القيمة
                    // (EN) Variable pattern - always true and binds value
                    condReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                    SIROperand trueOp = SIROperand::ConstantBool(true);
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = resultOp;
                    moveInst.operands = {trueOp};
                    currentBlock_->instructions.push_back(moveInst);

                    // (AR) ربط المتغير: حجز + تخزين
                    // (EN) Bind variable: alloc + store
                    std::string varReg = newTempRegister();

                    // (AR) إضافة المتغير إلى النطاق باسم النمط
                    // (EN) Add variable to scope with pattern name
                    VariableInfo varInfo;
                    varInfo.name = varPat->name;
                    varInfo.type = matchValueType;
                    varInfo.registerName = matchValueReg;
                    varInfo.isGlobal = false;
                    varInfo.isMutable = false;
                    varInfo.scopeLevel = currentScopeLevel_;
                    addVariable(varInfo);

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i
                              << " is VariablePattern(" << varPat->name << ")" << std::endl;
#endif
                }
                else if (auto *litPat = dynamic_cast<const Sad::AST::LiteralPattern *>(pattern))
                {
                    // (AR) نمط قيمة حرفية - مقارنة EQ
                    // (EN) Literal pattern - EQ comparison
                    condReg = newTempRegister();

                    // (AR) تحديد قيمة الثابت
                    // (EN) Determine constant value
                    std::string litValue;
                    SadTypeKind litType = matchValueType;

                    const auto &lit = litPat->literal;
                    if (lit.isInteger())
                    {
                        litValue = std::to_string(lit.toInt());
                        litType = SadTypeKind::Integer;
                    }
                    else if (lit.getKind() == SadTypeKind::Float)
                    {
                        litValue = std::to_string(lit.toDouble());
                        litType = SadTypeKind::Float;
                    }
                    else if (lit.getKind() == SadTypeKind::Boolean)
                    {
                        litValue = lit.toBool() ? "true" : "false";
                        litType = SadTypeKind::Boolean;
                    }
                    else if (lit.getKind() == SadTypeKind::String)
                    {
                        litValue = lit.toString();
                        litType = SadTypeKind::String;
                    }
                    else if (lit.getKind() == SadTypeKind::Void)
                    {
                        // (AR) إصلاح X04: لاشيء / null — Data::Value() بدون وسائط = Void kind
                        //      يجب مقارنتها مع kSadNullSentinel وليس 0
                        //      لأن 0 يُطابق false أيضاً مما يُسبّب تعارضاً
                        // (EN) FIX X04: null literal — Data::Value() with no args = Void kind
                        //      Must compare against kSadNullSentinel, not 0
                        //      Because 0 would also match false, causing collision
                        litValue = Sad::Compiler::kSadNullSentinelStr;
                        litType = SadTypeKind::Integer;
                    }
                    else
                    {
                        litValue = "0";
                        litType = SadTypeKind::Integer;
                    }

                    // (AR) تحويل النوع ليطابق نوع القيمة المُطابقة
                    // (EN) Coerce literal type to match the match value type
                    if (matchValueType == SadTypeKind::Integer && litType == SadTypeKind::Float)
                    {
                        // (AR) تحويل من عشري إلى صحيح
                        // (EN) Convert from float to integer
                        litValue = std::to_string(static_cast<int64_t>(lit.toDouble()));
                        litType = SadTypeKind::Integer;
                    }
                    else if (matchValueType == SadTypeKind::Float && litType == SadTypeKind::Integer)
                    {
                        // (AR) تحويل من صحيح إلى عشري
                        // (EN) Convert from integer to float
                        litValue = std::to_string(static_cast<double>(lit.toInt()));
                        litType = SadTypeKind::Float;
                    }

                    // (AR) تحميل الثابت في سجل
                    // (EN) Load constant into register
                    std::string litReg = newTempRegister();
                    SIROperand litResultOp = SIROperand::Register(litReg, litType);
                    SIROperand litConstOp;
                    if (litType == SadTypeKind::String)
                    {
                        litConstOp = SIROperand::ConstantString(litValue);
                    }
                    else if (litType == SadTypeKind::Float)
                    {
                        litConstOp = SIROperand::ConstantF64(std::stod(litValue));
                    }
                    else if (litType == SadTypeKind::Boolean)
                    {
                        litConstOp = SIROperand::ConstantBool(litValue == "true");
                    }
                    else
                    {
                        litConstOp = SIROperand::ConstantI64(std::stoll(litValue));
                    }
                    SIRInstruction moveLit(SIROpcode::MOVE);
                    moveLit.result = litResultOp;
                    moveLit.operands = {litConstOp};
                    currentBlock_->instructions.push_back(moveLit);

                    // (AR) مقارنة القيمتين
                    // (EN) Compare values
                    SIROperand matchOp = SIROperand::Register(matchValueReg, matchValueType);
                    SIROperand cmpLitOp = SIROperand::Register(litReg, litType);
                    SIROperand cmpResultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                    SIRInstruction cmpInst = SIRInstruction::Binary(SIROpcode::EQ, cmpResultOp, matchOp, cmpLitOp);
                    currentBlock_->instructions.push_back(cmpInst);

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i
                              << " is LiteralPattern(" << litValue << ")" << std::endl;
#endif
                }
                else if (auto *rangePat = dynamic_cast<const Sad::AST::RangePattern *>(pattern))
                {
                    // (AR) نمط نطاق: start..end أو start..=end
                    // (EN) Range pattern: start..end or start..=end
                    condReg = newTempRegister();

                    if (matchValueType != SadTypeKind::Integer && matchValueType != SadTypeKind::Float)
                    {
                        // (AR) نطاق على نوع غير رقمي: فشل آمن + تشخيص واضح
                        // (EN) Range on non-numeric value: safe fail + clear diagnostic
                        SIROperand resultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                        SIRInstruction moveFalse(SIROpcode::MOVE);
                        moveFalse.result = resultOp;
                        moveFalse.operands = {SIROperand::ConstantBool(false)};
                        currentBlock_->instructions.push_back(moveFalse);

                        errors_.push_back("Error: Range pattern requires numeric match value");
                    }
                    else
                    {
                        SadTypeKind boundType = matchValueType;

                        std::string startReg = newTempRegister();
                        std::string endReg = newTempRegister();

                        SIROperand startResultOp = SIROperand::Register(startReg, boundType);
                        SIROperand endResultOp = SIROperand::Register(endReg, boundType);

                        SIROperand startConstOp;
                        SIROperand endConstOp;

                        if (boundType == SadTypeKind::Float)
                        {
                            startConstOp = SIROperand::ConstantF64(rangePat->start.toDouble());
                            endConstOp = SIROperand::ConstantF64(rangePat->end.toDouble());
                        }
                        else
                        {
                            startConstOp = SIROperand::ConstantI64(rangePat->start.toInt());
                            endConstOp = SIROperand::ConstantI64(rangePat->end.toInt());
                        }

                        SIRInstruction moveStart(SIROpcode::MOVE);
                        moveStart.result = startResultOp;
                        moveStart.operands = {startConstOp};
                        currentBlock_->instructions.push_back(moveStart);

                        SIRInstruction moveEnd(SIROpcode::MOVE);
                        moveEnd.result = endResultOp;
                        moveEnd.operands = {endConstOp};
                        currentBlock_->instructions.push_back(moveEnd);

                        std::string lowerReg = newTempRegister();
                        std::string upperReg = newTempRegister();

                        SIROperand matchOp = SIROperand::Register(matchValueReg, matchValueType);
                        SIROperand lowerResultOp = SIROperand::Register(lowerReg, SadTypeKind::Boolean);
                        SIROperand upperResultOp = SIROperand::Register(upperReg, SadTypeKind::Boolean);

                        SIRInstruction lowerCmp = SIRInstruction::Binary(
                            SIROpcode::GE,
                            lowerResultOp,
                            matchOp,
                            SIROperand::Register(startReg, boundType));
                        currentBlock_->instructions.push_back(lowerCmp);

                        SIRInstruction upperCmp = SIRInstruction::Binary(
                            rangePat->inclusive ? SIROpcode::LE : SIROpcode::LT,
                            upperResultOp,
                            matchOp,
                            SIROperand::Register(endReg, boundType));
                        currentBlock_->instructions.push_back(upperCmp);

                        SIRInstruction andCmp = SIRInstruction::Binary(
                            SIROpcode::AND,
                            SIROperand::Register(condReg, SadTypeKind::Boolean),
                            SIROperand::Register(lowerReg, SadTypeKind::Boolean),
                            SIROperand::Register(upperReg, SadTypeKind::Boolean));
                        currentBlock_->instructions.push_back(andCmp);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i
                              << " is RangePattern(" << rangePat->toString() << ")" << std::endl;
#endif
                }
                else if (auto *orPat = dynamic_cast<const Sad::AST::OrPattern *>(pattern))
                {
                    // (AR) نمط OR - سلسلة مقارنات مع OR بدعم جميع أنواع الأنماط
                    //      الإصلاح: كل بديل في OR يُعالج عبر نداء تكراري لـ buildMatchPatternCondition
                    //      هذا يدعم: LiteralPattern، RangePattern، WildcardPattern، وغيرها
                    // (EN) OR pattern - chain comparisons with OR, supporting ALL pattern types
                    //      Fix: each OR alternative is processed via recursive call to buildMatchPatternCondition
                    //      This supports: LiteralPattern, RangePattern, WildcardPattern, and others
                    condReg = newTempRegister();

                    // (AR) بدء بـ false — نراكم نتيجة OR على كل البدائل
                    // (EN) Start with false — accumulate OR result over all alternatives
                    std::string accumReg = newTempRegister();
                    SIROperand accResultOp = SIROperand::Register(accumReg, SadTypeKind::Boolean);
                    SIROperand falseOp = SIROperand::ConstantBool(false);
                    SIRInstruction moveInit(SIROpcode::MOVE);
                    moveInit.result = accResultOp;
                    moveInit.operands = {falseOp};
                    currentBlock_->instructions.push_back(moveInit);

                    for (const auto &alt : orPat->alternatives)
                    {
                        // (AR) نداء تكراري: بناء شرط البديل الحالي بأي نوع نمط كان
                        //      يدعم: LiteralPattern، RangePattern، WildcardPattern، VariablePattern
                        // (EN) Recursive call: build condition for current alternative, any pattern type
                        //      Supports: LiteralPattern, RangePattern, WildcardPattern, VariablePattern
                        std::string altCondReg = buildMatchPatternCondition(
                            alt.get(), matchValueReg, matchValueType, caseIndex, deferredExtractions);

                        // (AR) دمج مع النتيجة المتراكمة عبر OR
                        // (EN) OR with accumulated result
                        std::string newAccumReg = newTempRegister();
                        SIRInstruction orInst = SIRInstruction::Binary(
                            SIROpcode::OR,
                            SIROperand::Register(newAccumReg, SadTypeKind::Boolean),
                            SIROperand::Register(accumReg, SadTypeKind::Boolean),
                            SIROperand::Register(altCondReg, SadTypeKind::Boolean));
                        currentBlock_->instructions.push_back(orInst);

                        accumReg = newAccumReg;
                    }

                    // (AR) نقل النتيجة النهائية إلى condReg
                    // (EN) Move final result to condReg
                    SIROperand finalResultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                    SIROperand finalAccOp = SIROperand::Register(accumReg, SadTypeKind::Boolean);
                    SIRInstruction moveFinal(SIROpcode::MOVE);
                    moveFinal.result = finalResultOp;
                    moveFinal.operands = {finalAccOp};
                    currentBlock_->instructions.push_back(moveFinal);

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i << " is OrPattern" << std::endl;
#endif
                }
                else if (auto *listPat = dynamic_cast<const Sad::AST::ListPattern *>(pattern))
                {
                    // ================================================================
                    // (AR) نمط المصفوفة [أ، ب، ج] - مقارنة الطول + مقارنة كل عنصر
                    // (EN) List pattern [a, b, c] - length check + element-wise comparison
                    // ================================================================
                    condReg = newTempRegister();

                    // (AR) الحصول على طول المصفوفة
                    // (EN) Get array length
                    std::string lenReg = newTempRegister();
                    {
                        SIRInstruction lenInst;
                        lenInst.opcode = SIROpcode::ARRAY_LEN;
                        lenInst.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                        lenInst.operands.push_back(SIROperand::Register(matchValueReg, matchValueType));
                        lenInst.comment = "list pattern: get array length";
                        if (currentBlock_)
                            currentBlock_->addInstruction(lenInst);
                    }

                    // (AR) مقارنة الطول
                    // (EN) Compare length
                    std::string lenCheckReg = newTempRegister();
                    if (listPat->has_rest)
                    {
                        // (AR) يجب أن يكون الطول >= عدد العناصر المحددة
                        // (EN) Length must be >= number of specified elements
                        SIRInstruction geInst = SIRInstruction::Binary(
                            SIROpcode::GE,
                            SIROperand::Register(lenCheckReg, SadTypeKind::Boolean),
                            SIROperand::Register(lenReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(static_cast<int64_t>(listPat->elements.size())));
                        if (currentBlock_)
                            currentBlock_->addInstruction(geInst);
                    }
                    else
                    {
                        // (AR) يجب أن يكون الطول مساوياً تماماً
                        // (EN) Length must match exactly
                        SIRInstruction eqInst = SIRInstruction::Binary(
                            SIROpcode::EQ,
                            SIROperand::Register(lenCheckReg, SadTypeKind::Boolean),
                            SIROperand::Register(lenReg, SadTypeKind::Integer),
                            SIROperand::ConstantI64(static_cast<int64_t>(listPat->elements.size())));
                        if (currentBlock_)
                            currentBlock_->addInstruction(eqInst);
                    }

                    // (AR) مقارنة كل عنصر — حرفي فقط في كتلة الاختبار
                    //      المتغيرات تُؤجَّل إلى كتلة الجسم لتجنب array.get خارج الحدود
                    //      عندما لا يتطابق الطول (مثلاً: المصفوفة فارغة + نمط [أ، ب])
                    // (EN) Compare each element — literal only in test block
                    //      Variable bindings are deferred to body block to avoid
                    //      out-of-bounds array.get when length doesn't match
                    std::string accumReg2 = lenCheckReg;
                    for (size_t e = 0; e < listPat->elements.size(); ++e)
                    {
                        const auto &elemPat = listPat->elements[e];

                        if (auto *elemLit = dynamic_cast<const Sad::AST::LiteralPattern *>(elemPat.get()))
                        {
                            // (AR) مطابقة عنصر حرفي — يجب أن يكون بعد فحص الطول
                            //      لكن لا يمكننا تأجيلها لأننا نحتاج condReg في الاختبار
                            //      الحل: التأجيل أيضاً — condReg = lengthCheck فقط
                            //      المقارنة الحرفية تُؤجل مع المتغيرات
                            // (EN) Literal match — defer to body block too for safety
                            //      condReg = length check only
                            //      (literal comparison can be done in body with early exit)

                            // (AR) تأجيل: حفظ فهرس + قيمة حرفية في deferredExtractions
                            // (EN) Defer: save index + literal value in deferredExtractions
                            MatchDeferredField deferred;
                            deferred.varName = "__lit_" + std::to_string(e);
                            deferred.fieldIndex = e;
                            deferred.fieldName = elemLit->literal.toString();
                            deferred.enumName = "__list_pattern_literal";
                            deferredExtractions.push_back(deferred);
                        }
                        else if (auto *elemVar = dynamic_cast<const Sad::AST::VariablePattern *>(elemPat.get()))
                        {
                            // (AR) ربط متغير — يُؤجَّل إلى كتلة الجسم
                            //      نخزن (اسم_المتغير، الفهرس) في deferredExtractions
                            //      مع enumName = "__list_pattern" كعلامة مميزة
                            // (EN) Variable binding — deferred to body block
                            //      Store (varName, index) in deferredExtractions
                            //      with enumName = "__list_pattern" as sentinel
                            MatchDeferredField deferred;
                            deferred.varName = elemVar->name;
                            deferred.fieldIndex = e;
                            deferred.fieldName = elemVar->name;
                            deferred.enumName = "__list_pattern";
                            deferredExtractions.push_back(deferred);
                        }
                        else if (dynamic_cast<const Sad::AST::WildcardPattern *>(elemPat.get()))
                        {
                            // (AR) البلاط الشامل _ — تجاهل العنصر
                            // (EN) Wildcard _ — ignore element
                        }
                    }

                    // (AR) نقل النتيجة النهائية
                    // (EN) Move final result
                    SIRInstruction moveFinalList(SIROpcode::MOVE);
                    moveFinalList.result = SIROperand::Register(condReg, SadTypeKind::Boolean);
                    moveFinalList.operands = {SIROperand::Register(accumReg2, SadTypeKind::Boolean)};
                    if (currentBlock_)
                        currentBlock_->addInstruction(moveFinalList);

                    // (AR) ربط متغير rest إن وجد
                    // (EN) Bind rest variable if present
                    if (listPat->has_rest && !listPat->rest_name.empty())
                    {
                        std::string restReg = newTempRegister();
                        SIRInstruction sliceInst;
                        sliceInst.opcode = SIROpcode::CALL;
                        sliceInst.result = SIROperand::Register(restReg, SadTypeKind::Pointer);
                        sliceInst.operands.push_back(SIROperand::Function("__sad_array_slice"));
                        sliceInst.operands.push_back(SIROperand::Register(matchValueReg, matchValueType));
                        sliceInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(listPat->elements.size())));
                        sliceInst.operands.push_back(SIROperand::Register(lenReg, SadTypeKind::Integer));
                        sliceInst.comment = "list pattern: slice rest elements";
                        if (currentBlock_)
                            currentBlock_->addInstruction(sliceInst);

                        VariableInfo restVarInfo;
                        restVarInfo.name = listPat->rest_name;
                        restVarInfo.type = SadTypeKind::Pointer;
                        restVarInfo.registerName = restReg;
                        restVarInfo.isGlobal = false;
                        restVarInfo.isMutable = false;
                        restVarInfo.scopeLevel = currentScopeLevel_;
                        addVariable(restVarInfo);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i << " is ListPattern" << std::endl;
#endif
                }
                else if (auto *structPat = dynamic_cast<const Sad::AST::StructPattern *>(pattern))
                {
                    // ================================================================
                    // (AR) نمط البنية {حقل1: نمط1، حقل2: نمط2}
                    // (EN) Struct pattern {field1: pat1, field2: pat2}
                    // ================================================================
                    condReg = newTempRegister();

                    // (AR) بداية: true (ثم AND مع كل اختبار حقل)
                    // (EN) Start: true (then AND with each field test)
                    std::string structAccum = newTempRegister();
                    {
                        SIRInstruction moveTrue(SIROpcode::MOVE);
                        moveTrue.result = SIROperand::Register(structAccum, SadTypeKind::Boolean);
                        moveTrue.operands = {SIROperand::ConstantBool(true)};
                        if (currentBlock_)
                            currentBlock_->addInstruction(moveTrue);
                    }

                    // (AR) التحقق من النوع إن وُجد
                    // (EN) Check type if specified
                    if (!structPat->typeName.empty())
                    {
                        std::string typeCheckReg = newTempRegister();
                        SIRInstruction typeInst;
                        typeInst.opcode = SIROpcode::CALL;
                        typeInst.result = SIROperand::Register(typeCheckReg, SadTypeKind::Boolean);
                        typeInst.operands.push_back(SIROperand::Function("__sad_instanceof"));
                        typeInst.operands.push_back(SIROperand::Register(matchValueReg, matchValueType));
                        typeInst.operands.push_back(SIROperand::ConstantString(structPat->typeName));
                        typeInst.comment = "struct pattern: type check '" + structPat->typeName + "'";
                        if (currentBlock_)
                            currentBlock_->addInstruction(typeInst);

                        std::string newAcc = newTempRegister();
                        SIRInstruction andType = SIRInstruction::Binary(
                            SIROpcode::AND,
                            SIROperand::Register(newAcc, SadTypeKind::Boolean),
                            SIROperand::Register(structAccum, SadTypeKind::Boolean),
                            SIROperand::Register(typeCheckReg, SadTypeKind::Boolean));
                        if (currentBlock_)
                            currentBlock_->addInstruction(andType);
                        structAccum = newAcc;
                    }

                    // (AR) مقارنة كل حقل
                    // (EN) Compare each field
                    for (const auto &fieldPair : structPat->fields)
                    {
                        const std::string &fieldName = fieldPair.first;
                        const auto &fieldPattern = fieldPair.second;

                        // (AR) استخراج قيمة الحقل من الكائن
                        // (EN) Extract field value from object
                        std::string fieldReg = newTempRegister();
                        {
                            SIRInstruction getFieldInst;
                            getFieldInst.opcode = SIROpcode::CALL;
                            getFieldInst.result = SIROperand::Register(fieldReg, SadTypeKind::Pointer);
                            getFieldInst.operands.push_back(SIROperand::Function("__sad_get_field"));
                            getFieldInst.operands.push_back(SIROperand::Register(matchValueReg, matchValueType));
                            getFieldInst.operands.push_back(SIROperand::ConstantString(fieldName));
                            getFieldInst.comment = "struct pattern: get field '" + fieldName + "'";
                            if (currentBlock_)
                                currentBlock_->addInstruction(getFieldInst);
                        }

                        if (auto *fldLit = dynamic_cast<const Sad::AST::LiteralPattern *>(fieldPattern.get()))
                        {
                            // (AR) مطابقة حرفية للحقل
                            // (EN) Literal match for field
                            std::string fldLitReg = newTempRegister();
                            SIROperand fldConstOp;
                            const auto &fldVal = fldLit->literal;
                            if (fldVal.isInteger())
                            {
                                fldConstOp = SIROperand::ConstantI64(fldVal.toInt());
                            }
                            else if (fldVal.getKind() == SadTypeKind::Float)
                            {
                                fldConstOp = SIROperand::ConstantF64(fldVal.toDouble());
                            }
                            else if (fldVal.getKind() == SadTypeKind::String)
                            {
                                fldConstOp = SIROperand::ConstantString(fldVal.toString());
                            }
                            else
                            {
                                fldConstOp = SIROperand::ConstantI64(0);
                            }

                            SIRInstruction moveFldLit(SIROpcode::MOVE);
                            moveFldLit.result = SIROperand::Register(fldLitReg, SadTypeKind::Integer);
                            moveFldLit.operands = {fldConstOp};
                            if (currentBlock_)
                                currentBlock_->addInstruction(moveFldLit);

                            std::string fldCmpReg = newTempRegister();
                            SIRInstruction cmpFld = SIRInstruction::Binary(
                                SIROpcode::EQ,
                                SIROperand::Register(fldCmpReg, SadTypeKind::Boolean),
                                SIROperand::Register(fieldReg, SadTypeKind::Pointer),
                                SIROperand::Register(fldLitReg, SadTypeKind::Integer));
                            if (currentBlock_)
                                currentBlock_->addInstruction(cmpFld);

                            std::string newAcc2 = newTempRegister();
                            SIRInstruction andFld = SIRInstruction::Binary(
                                SIROpcode::AND,
                                SIROperand::Register(newAcc2, SadTypeKind::Boolean),
                                SIROperand::Register(structAccum, SadTypeKind::Boolean),
                                SIROperand::Register(fldCmpReg, SadTypeKind::Boolean));
                            if (currentBlock_)
                                currentBlock_->addInstruction(andFld);
                            structAccum = newAcc2;
                        }
                        else if (auto *fldVar = dynamic_cast<const Sad::AST::VariablePattern *>(fieldPattern.get()))
                        {
                            // (AR) ربط حقل بمتغير — دائماً ناجح
                            // (EN) Bind field to variable — always succeeds
                            VariableInfo fldVarInfo;
                            fldVarInfo.name = fldVar->name;
                            fldVarInfo.type = SadTypeKind::Pointer;
                            fldVarInfo.registerName = fieldReg;
                            fldVarInfo.isGlobal = false;
                            fldVarInfo.isMutable = false;
                            fldVarInfo.scopeLevel = currentScopeLevel_;
                            addVariable(fldVarInfo);
                        }
                        else if (dynamic_cast<const Sad::AST::WildcardPattern *>(fieldPattern.get()))
                        {
                            // (AR) البلاط الشامل _ — تجاهل الحقل
                            // (EN) Wildcard _ — ignore field
                        }
                    }

                    // (AR) النتيجة النهائية
                    // (EN) Final result
                    SIRInstruction moveFinalStruct(SIROpcode::MOVE);
                    moveFinalStruct.result = SIROperand::Register(condReg, SadTypeKind::Boolean);
                    moveFinalStruct.operands = {SIROperand::Register(structAccum, SadTypeKind::Boolean)};
                    if (currentBlock_)
                        currentBlock_->addInstruction(moveFinalStruct);

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i << " is StructPattern" << std::endl;
#endif
                }
                else if (auto *enumVarPat = dynamic_cast<const Sad::AST::EnumVariantPattern *>(pattern))
                {
                    // ================================================================
                    // (AR) نمط حالة تعداد جبري (ADT Enum Variant Pattern)
                    //      عندما شكل.دائرة(ق):   → فحص المميّز + استخراج الحقول
                    //      عندما شكل.نقطة:        → فحص المميّز فقط (Unit)
                    //
                    //      الخطوات:
                    //      1. البحث عن معلومات التعداد في adtEnumTable_
                    //      2. استخراج المميّز من القيمة عبر ENUM_GET_TAG
                    //      3. مقارنة المميّز مع tag الحالة المحددة عبر EQ
                    //      4. إذا تطابق وكانت هناك حقول: استخراجها عبر ENUM_GET_PAYLOAD
                    //         وربطها بمتغيرات في النطاق الحالي
                    //
                    // (EN) ADT Enum Variant Pattern
                    //      when Shape.Circle(r):   → check discriminant + extract fields
                    //      when Shape.Point:        → check discriminant only (Unit)
                    //
                    //      Steps:
                    //      1. Look up enum info in adtEnumTable_
                    //      2. Extract discriminant via ENUM_GET_TAG
                    //      3. Compare discriminant with variant tag via EQ
                    //      4. If matched and has fields: extract via ENUM_GET_PAYLOAD
                    //         and bind them as variables in current scope
                    // ================================================================
                    condReg = newTempRegister();

                    // (AR) البحث عن معلومات التعداد الجبري
                    // (EN) Look up ADT enum info
                    auto adtIt = adtEnumTable_.find(enumVarPat->enumName);
                    if (adtIt == adtEnumTable_.end())
                    {
                        // (AR) التعداد غير موجود في جدول ADT — ربما تعداد بسيط
                        //      نقوم بمقارنة مباشرة مع ثابت المميّز
                        // (EN) Enum not in ADT table — might be simple enum
                        //      Do direct comparison with discriminant constant

                        // (AR) البحث عن الثابت العام
                        // (EN) Look up global constant
                        std::string fullName = enumVarPat->enumName + "." + enumVarPat->variantName;
                        VariableInfo *enumConstVar = lookupVariable(fullName);

                        if (enumConstVar)
                        {
                            // (AR) مقارنة مع ثابت التعداد البسيط
                            //      نستخدم Global بـ fullName (بدون %) لأن الثوابت العامة
                            //      مسجلة في LLVM بدون بادئة %
                            // (EN) Compare with simple enum constant
                            //      Use Global with fullName (no %) since globals
                            //      are registered in LLVM without % prefix
                            std::string enumConstReg = newTempRegister();
                            SIRInstruction loadConst(SIROpcode::LOAD);
                            loadConst.result = SIROperand::Register(enumConstReg, SadTypeKind::Integer);
                            loadConst.operands.push_back(
                                SIROperand::Global(fullName, SadTypeKind::Integer));
                            if (currentBlock_)
                                currentBlock_->addInstruction(loadConst);

                            SIRInstruction cmpSimple = SIRInstruction::Binary(
                                SIROpcode::EQ,
                                SIROperand::Register(condReg, SadTypeKind::Boolean),
                                SIROperand::Register(matchValueReg, matchValueType),
                                SIROperand::Register(enumConstReg, SadTypeKind::Integer));
                            if (currentBlock_)
                                currentBlock_->addInstruction(cmpSimple);
                        }
                        else
                        {
                            // (AR) لا يمكن إيجاد الحالة — false
                            // (EN) Cannot find variant — false
                            SIRInstruction moveFalseEnum(SIROpcode::MOVE);
                            moveFalseEnum.result = SIROperand::Register(condReg, SadTypeKind::Boolean);
                            moveFalseEnum.operands = {SIROperand::ConstantBool(false)};
                            if (currentBlock_)
                                currentBlock_->addInstruction(moveFalseEnum);

                            errors_.push_back("Error: Unknown enum variant '" +
                                              enumVarPat->enumName + "." + enumVarPat->variantName + "'");
                        }
                    }
                    else
                    {
                        // (AR) تعداد جبري (ADT) — استخدام تعليمات ADT المتخصصة
                        // (EN) ADT enum — use specialized ADT instructions
                        const ADTEnumInfo &adtInfo = adtIt->second;
                        const ADTVariantInfo *variantInfo = adtInfo.findVariant(enumVarPat->variantName);

                        if (!variantInfo)
                        {
                            // (AR) الحالة غير موجودة في هذا التعداد
                            // (EN) Variant not found in this enum
                            SIRInstruction moveFalseADT(SIROpcode::MOVE);
                            moveFalseADT.result = SIROperand::Register(condReg, SadTypeKind::Boolean);
                            moveFalseADT.operands = {SIROperand::ConstantBool(false)};
                            if (currentBlock_)
                                currentBlock_->addInstruction(moveFalseADT);

                            errors_.push_back("Error: Variant '" + enumVarPat->variantName +
                                              "' not found in enum '" + enumVarPat->enumName + "'");
                        }
                        else
                        {
                            // (AR) الخطوة 2: فحص المميّز عبر ENUM_IS_VARIANT
                            //      %is_variant = EnumIsVariant(%match_val, variantTag, enumName, isUnit)
                            //      نضيف اسم التعداد وعلامة isUnit كمعلومات إضافية
                            //      حتى يعرف LLVM CodeGen كيفية التعامل مع القيمة
                            //      بدون الحاجة إلى objectClassMap (الذي لا ينتقل عبر حدود الدوال)
                            // (EN) Step 2: Check discriminant via ENUM_IS_VARIANT
                            //      Add enum name and isUnit flag as extra info
                            //      so LLVM CodeGen knows how to handle the value
                            //      without relying on objectClassMap (which doesn't cross function boundaries)
                            SIRInstruction isVariantInst(SIROpcode::ENUM_IS_VARIANT);
                            isVariantInst.result = SIROperand::Register(condReg, SadTypeKind::Boolean);
                            isVariantInst.operands.push_back(
                                SIROperand::Register(matchValueReg, matchValueType));
                            isVariantInst.operands.push_back(
                                SIROperand::ConstantI64(variantInfo->tag));
                            // (AR) المعامل [2]: اسم التعداد — للبحث عن structType في LLVM
                            // (EN) Operand [2]: enum name — for structType lookup in LLVM
                            isVariantInst.operands.push_back(
                                SIROperand::ConstantString(enumVarPat->enumName));
                            // (AR) المعامل [3]: هل الحالة المُفحوصة وحدية (Unit)؟
                            //      0 = data variant (مؤشر لبنية)
                            //      1 = unit variant (قيمة tag مباشرة)
                            // (EN) Operand [3]: is the checked variant a Unit?
                            //      0 = data variant (pointer to struct)
                            //      1 = unit variant (tag value directly)
                            isVariantInst.operands.push_back(
                                SIROperand::ConstantI64(variantInfo->isUnit() ? 1 : 0));
                            isVariantInst.comment = "Check if " + enumVarPat->enumName +
                                                    " is " + enumVarPat->variantName + " (tag=" +
                                                    std::to_string(variantInfo->tag) +
                                                    ", unit=" + (variantInfo->isUnit() ? "yes" : "no") + ")";
                            if (currentBlock_)
                                currentBlock_->addInstruction(isVariantInst);

                            // (AR) الخطوة 3: استخراج الحقول وربطها بمتغيرات
                            //      لكل حقل في النمط:
                            //        إذا كان نمط متغير (VariablePattern):
                            //          %field_i = EnumGetPayload(%match_val, i)
                            //          ثم ربط المتغير بالسجل
                            //        إذا كان نمط شامل (_): تجاهل
                            //        إذا كان نمط حرفي: فحص إضافي (AND مع condReg)
                            // (EN) Step 3: Extract fields and bind to variables
                            //      For each field pattern:
                            //        If VariablePattern: extract via ENUM_GET_PAYLOAD and bind
                            //        If WildcardPattern: ignore
                            //        If LiteralPattern: additional check (AND with condReg)

                            for (size_t fi = 0; fi < enumVarPat->fieldPatterns.size() &&
                                                fi < variantInfo->fields.size();
                                 ++fi)
                            {
                                const auto &fieldPat = enumVarPat->fieldPatterns[fi];

                                if (auto *varFieldPat = dynamic_cast<const Sad::AST::VariablePattern *>(fieldPat.get()))
                                {
                                    // (AR) تأجيل استخراج الحقل إلى كتلة الجسم (بعد التحقق من المطابقة)
                                    //      هذا يمنع الوصول غير الصالح للذاكرة عندما تكون الحالة مختلفة
                                    // (EN) Defer field extraction to body block (after match verification)
                                    //      This prevents invalid memory access when variant doesn't match
                                    MatchDeferredField deferred;
                                    deferred.varName = varFieldPat->name;
                                    deferred.fieldIndex = fi;
                                    deferred.fieldName = variantInfo->fields[fi];
                                    deferred.enumName = enumVarPat->enumName;
                                    deferredExtractions.push_back(std::move(deferred));
                                }
                                else if (auto *litFieldPat = dynamic_cast<const Sad::AST::LiteralPattern *>(fieldPat.get()))
                                {
                                    // (AR) مطابقة حقل مع قيمة حرفية — فحص إضافي
                                    //      ملاحظة: الاستخراج هنا آمن لأن البنية tagged union تضمن
                                    //      أن جميع الحقول موجودة في الذاكرة (حتى للحالات الأخرى)
                                    // (EN) Match field against literal — additional check
                                    //      Note: extraction here is safe because tagged union guarantees
                                    //      all fields exist in memory (even for other variants)

                                    // (AR) استخراج الحقل من الحمولة
                                    // (EN) Extract field from payload
                                    std::string fieldReg = newTempRegister();
                                    SIRInstruction getPayload(SIROpcode::ENUM_GET_PAYLOAD);
                                    getPayload.result = SIROperand::Register(fieldReg, SadTypeKind::Integer);
                                    getPayload.operands.push_back(
                                        SIROperand::Register(matchValueReg, matchValueType));
                                    getPayload.operands.push_back(
                                        SIROperand::ConstantI64(static_cast<int64_t>(fi)));
                                    // (AR) المعامل [2]: اسم التعداد للبحث عن البنية عبر حدود الدوال
                                    // (EN) Operand [2]: enum name for struct lookup across function boundaries
                                    getPayload.operands.push_back(
                                        SIROperand::ConstantString(enumVarPat->enumName));
                                    getPayload.comment = "Extract field " + std::to_string(fi) +
                                                         " (" + variantInfo->fields[fi] + ") for literal check";
                                    if (currentBlock_)
                                        currentBlock_->addInstruction(getPayload);

                                    std::string litFieldReg = newTempRegister();
                                    SIROperand litFieldOp;
                                    const auto &litFieldVal = litFieldPat->literal;
                                    if (litFieldVal.isInteger())
                                    {
                                        litFieldOp = SIROperand::ConstantI64(litFieldVal.toInt());
                                    }
                                    else if (litFieldVal.getKind() == SadTypeKind::Float)
                                    {
                                        litFieldOp = SIROperand::ConstantF64(litFieldVal.toDouble());
                                    }
                                    else if (litFieldVal.getKind() == SadTypeKind::String)
                                    {
                                        litFieldOp = SIROperand::ConstantString(litFieldVal.toString());
                                    }
                                    else
                                    {
                                        litFieldOp = SIROperand::ConstantI64(0);
                                    }

                                    SIRInstruction moveLitField(SIROpcode::MOVE);
                                    moveLitField.result = SIROperand::Register(litFieldReg, SadTypeKind::Pointer);
                                    moveLitField.operands = {litFieldOp};
                                    if (currentBlock_)
                                        currentBlock_->addInstruction(moveLitField);

                                    std::string fieldCmpReg = newTempRegister();
                                    SIRInstruction cmpField = SIRInstruction::Binary(
                                        SIROpcode::EQ,
                                        SIROperand::Register(fieldCmpReg, SadTypeKind::Boolean),
                                        SIROperand::Register(fieldReg, SadTypeKind::Pointer),
                                        SIROperand::Register(litFieldReg, SadTypeKind::Pointer));
                                    if (currentBlock_)
                                        currentBlock_->addInstruction(cmpField);

                                    // (AR) دمج مع condReg عبر AND
                                    // (EN) Combine with condReg via AND
                                    std::string newCondReg = newTempRegister();
                                    SIRInstruction andField = SIRInstruction::Binary(
                                        SIROpcode::AND,
                                        SIROperand::Register(newCondReg, SadTypeKind::Boolean),
                                        SIROperand::Register(condReg, SadTypeKind::Boolean),
                                        SIROperand::Register(fieldCmpReg, SadTypeKind::Boolean));
                                    if (currentBlock_)
                                        currentBlock_->addInstruction(andField);
                                    condReg = newCondReg;
                                }
                                else if (dynamic_cast<const Sad::AST::WildcardPattern *>(fieldPat.get()))
                                {
                                    // (AR) تجاهل الحقل
                                    // (EN) Ignore field
                                }
                            }
                        }
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i
                              << " is EnumVariantPattern(" << enumVarPat->enumName
                              << "." << enumVarPat->variantName << ")" << std::endl;
#endif
                }
                else if (auto *bindPat = dynamic_cast<const Sad::AST::BindingPattern *>(pattern))
                {
                    // ================================================================
                    // (AR) نمط الربط: اسم @ نمط — ربط القيمة مع التحقق من نمط
                    // (EN) Binding pattern: name @ pattern — bind value while checking pattern
                    // ================================================================
                    condReg = newTempRegister();

                    // (AR) ربط القيمة بالمتغير أولاً
                    // (EN) Bind value to variable first
                    VariableInfo bindVarInfo;
                    bindVarInfo.name = bindPat->name;
                    bindVarInfo.type = matchValueType;
                    bindVarInfo.registerName = matchValueReg;
                    bindVarInfo.isGlobal = false;
                    bindVarInfo.isMutable = false;
                    bindVarInfo.scopeLevel = currentScopeLevel_;
                    addVariable(bindVarInfo);

                    // (AR) التحقق من النمط الداخلي
                    // (EN) Check inner pattern
                    const auto &innerPat = bindPat->pattern;
                    if (auto *innerLit = dynamic_cast<const Sad::AST::LiteralPattern *>(innerPat.get()))
                    {
                        // (AR) مطابقة حرفية
                        // (EN) Literal match
                        std::string innerLitReg = newTempRegister();
                        SIROperand innerConstOp;
                        const auto &innerVal = innerLit->literal;
                        if (innerVal.isInteger())
                        {
                            innerConstOp = SIROperand::ConstantI64(innerVal.toInt());
                        }
                        else if (innerVal.getKind() == SadTypeKind::Float)
                        {
                            innerConstOp = SIROperand::ConstantF64(innerVal.toDouble());
                        }
                        else if (innerVal.getKind() == SadTypeKind::String)
                        {
                            innerConstOp = SIROperand::ConstantString(innerVal.toString());
                        }
                        else
                        {
                            innerConstOp = SIROperand::ConstantI64(0);
                        }
                        SIRInstruction moveInnerLit(SIROpcode::MOVE);
                        moveInnerLit.result = SIROperand::Register(innerLitReg, matchValueType);
                        moveInnerLit.operands = {innerConstOp};
                        if (currentBlock_)
                            currentBlock_->addInstruction(moveInnerLit);

                        SIRInstruction cmpInner = SIRInstruction::Binary(
                            SIROpcode::EQ,
                            SIROperand::Register(condReg, SadTypeKind::Boolean),
                            SIROperand::Register(matchValueReg, matchValueType),
                            SIROperand::Register(innerLitReg, matchValueType));
                        if (currentBlock_)
                            currentBlock_->addInstruction(cmpInner);
                    }
                    else if (auto *innerRange = dynamic_cast<const Sad::AST::RangePattern *>(innerPat.get()))
                    {
                        // (AR) مطابقة نطاق
                        // (EN) Range match
                        std::string startReg2 = newTempRegister();
                        std::string endReg2 = newTempRegister();
                        SadTypeKind boundType2 = matchValueType;

                        SIROperand startOp2, endOp2;
                        if (boundType2 == SadTypeKind::Float)
                        {
                            startOp2 = SIROperand::ConstantF64(innerRange->start.toDouble());
                            endOp2 = SIROperand::ConstantF64(innerRange->end.toDouble());
                        }
                        else
                        {
                            startOp2 = SIROperand::ConstantI64(innerRange->start.toInt());
                            endOp2 = SIROperand::ConstantI64(innerRange->end.toInt());
                        }

                        SIRInstruction moveS2(SIROpcode::MOVE);
                        moveS2.result = SIROperand::Register(startReg2, boundType2);
                        moveS2.operands = {startOp2};
                        if (currentBlock_)
                            currentBlock_->addInstruction(moveS2);

                        SIRInstruction moveE2(SIROpcode::MOVE);
                        moveE2.result = SIROperand::Register(endReg2, boundType2);
                        moveE2.operands = {endOp2};
                        if (currentBlock_)
                            currentBlock_->addInstruction(moveE2);

                        std::string lReg2 = newTempRegister();
                        std::string uReg2 = newTempRegister();
                        SIRInstruction geInst2 = SIRInstruction::Binary(SIROpcode::GE,
                                                                        SIROperand::Register(lReg2, SadTypeKind::Boolean),
                                                                        SIROperand::Register(matchValueReg, matchValueType),
                                                                        SIROperand::Register(startReg2, boundType2));
                        if (currentBlock_)
                            currentBlock_->addInstruction(geInst2);

                        SIRInstruction leInst2 = SIRInstruction::Binary(
                            innerRange->inclusive ? SIROpcode::LE : SIROpcode::LT,
                            SIROperand::Register(uReg2, SadTypeKind::Boolean),
                            SIROperand::Register(matchValueReg, matchValueType),
                            SIROperand::Register(endReg2, boundType2));
                        if (currentBlock_)
                            currentBlock_->addInstruction(leInst2);

                        SIRInstruction andRange = SIRInstruction::Binary(SIROpcode::AND,
                                                                         SIROperand::Register(condReg, SadTypeKind::Boolean),
                                                                         SIROperand::Register(lReg2, SadTypeKind::Boolean),
                                                                         SIROperand::Register(uReg2, SadTypeKind::Boolean));
                        if (currentBlock_)
                            currentBlock_->addInstruction(andRange);
                    }
                    else if (dynamic_cast<const Sad::AST::WildcardPattern *>(innerPat.get()))
                    {
                        // (AR) دائماً ناجح
                        // (EN) Always succeeds
                        SIRInstruction moveBindTrue(SIROpcode::MOVE);
                        moveBindTrue.result = SIROperand::Register(condReg, SadTypeKind::Boolean);
                        moveBindTrue.operands = {SIROperand::ConstantBool(true)};
                        if (currentBlock_)
                            currentBlock_->addInstruction(moveBindTrue);
                    }
                    else
                    {
                        // (AR) نمط داخلي غير مدعوم — true افتراضياً
                        // (EN) Unsupported inner pattern — default true
                        SIRInstruction moveBindDefault(SIROpcode::MOVE);
                        moveBindDefault.result = SIROperand::Register(condReg, SadTypeKind::Boolean);
                        moveBindDefault.operands = {SIROperand::ConstantBool(true)};
                        if (currentBlock_)
                            currentBlock_->addInstruction(moveBindDefault);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i << " is BindingPattern(" << bindPat->name << ")" << std::endl;
#endif
                }
                else
                {
                    // (AR) نمط غير مدعوم - فشل آمن مع تشخيص واضح
                    // (EN) Unsupported pattern - safe failure with explicit diagnostic
                    condReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(condReg, SadTypeKind::Boolean);
                    SIROperand falseOp = SIROperand::ConstantBool(false);
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = resultOp;
                    moveInst.operands = {falseOp};
                    currentBlock_->instructions.push_back(moveInst);

                    errors_.push_back("Error: Unsupported pattern in match: " + pattern->toString());

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildMatchStatement: case " << i << " is unsupported pattern" << std::endl;
#endif
                }

                return condReg;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad