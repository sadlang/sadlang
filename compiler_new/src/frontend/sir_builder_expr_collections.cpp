// ============================================================================
// sir_builder_expr_collections.cpp
// ============================================================================
// Collection expression builders (arrays, tuples, maps, walrus)
// ============================================================================
#include "sir_builder.h"

#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprArray
            // ============================================================================
            BuildResult SIRBuilder::buildExprArray(AST::ArrayExpr *arrayExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found ArrayExpr with "
                          << arrayExpr->elements.size() << " elements" << std::endl;
#endif

                // (AR) تخصيص مصفوفة جديدة
                // (EN) Allocate new array
                std::string arrReg = newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ARRAY_NEW;
                allocInst.result = SIROperand::Register(arrReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(arrayExpr->elements.size())));
                allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(arrayExpr->elements.size())));
                allocInst.comment = "array new [" + std::to_string(arrayExpr->elements.size()) + "]";

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(allocInst);
                }

                // (AR) تخزين العناصر واحداً تلو الآخر
                // (EN) Store elements one by one
                SadTypeKind inferredElementType = SadTypeKind::Void;
                for (size_t i = 0; i < arrayExpr->elements.size(); ++i)
                {
                    auto elemResult = buildExpression(arrayExpr->elements[i].get());

                    // (AR) استنتاج نوع العنصر من العنصر الأول
                    // (EN) Infer element type from first element
                    if (i == 0)
                    {
                        inferredElementType = elemResult.type;
                    }

                    // (AR) تجسيد الثوابت قبل تخزينها (نفس الإصلاح المُطبَّق على MapExpr)
                    //      ملاحظة: عناصر المصفوفات المتداخلة (ARRAY) لا تحتاج تجسيداً — هي بالفعل في سجلات
                    // (EN) Materialize constants before storing (same fix applied to MapExpr)
                    //      Note: Nested array elements (ARRAY type) don't need materialization — already in registers
                    if (elemResult.isConstant && elemResult.type != SadTypeKind::Array && currentBlock_)
                    {
                        std::string reg = newTempRegister();
                        elemResult.registerName = reg;
                        SIRInstruction moveInst(SIROpcode::MOVE);
                        moveInst.result = SIROperand::Register(reg, elemResult.type);
                        if (elemResult.type == SadTypeKind::String)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantString(elemResult.constantValue));
                        }
                        else if (elemResult.type == SadTypeKind::Float)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(elemResult.constantValue)));
                        }
                        else if (elemResult.type == SadTypeKind::Boolean)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantBool(elemResult.constantValue == "true" || elemResult.constantValue == "1"));
                        }
                        else
                        {
                            try
                            {
                                moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(elemResult.constantValue)));
                            }
                            catch (const std::exception &)
                            {
#ifndef NDEBUG
                                std::cerr << "[SIR] تحذير: فشل تحويل '" << elemResult.constantValue << "' إلى I64، القيمة الافتراضية 0\n";
#endif
                                moveInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        currentBlock_->addInstruction(moveInst);
                        elemResult.isConstant = false;
                    }

                    SIRInstruction storeInst;
                    storeInst.opcode = SIROpcode::ARRAY_SET;
                    storeInst.operands.push_back(SIROperand::Register(arrReg, SadTypeKind::Array));
                    storeInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                    storeInst.operands.push_back(SIROperand::Register(elemResult.registerName, elemResult.type));
                    storeInst.comment = "array[" + std::to_string(i) + "] = ...";

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(storeInst);
                    }
                }

                BuildResult result(arrReg, SadTypeKind::Array);
                // (AR) استنتاج نوع العنصر من العنصر الأول (تم حفظه في الحلقة أعلاه)
                // (EN) Infer element type from first element (saved during loop above)
                if (inferredElementType != SadTypeKind::Void)
                {
                    result.elementType = inferredElementType;
                }
                return result;
            }

            // ============================================================================
            // buildExprTuple
            // ============================================================================
            BuildResult SIRBuilder::buildExprTuple(AST::TupleExpr *tupleExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found TupleExpr with "
                          << tupleExpr->elements.size() << " elements" << std::endl;
#endif

                // (AR) تخصيص صف جديد
                // (EN) Allocate new tuple
                std::string tupleReg = newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::TUPLE_NEW;
                allocInst.result = SIROperand::Register(tupleReg, SadTypeKind::Tuple);
                allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(tupleExpr->elements.size())));
                allocInst.comment = "tuple new (" + std::to_string(tupleExpr->elements.size()) + ")";

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(allocInst);
                }

                // (AR) تخزين العناصر واحداً تلو الآخر (نفس منطق المصفوفة)
                // (EN) Store elements one by one (same logic as array)
                for (size_t i = 0; i < tupleExpr->elements.size(); ++i)
                {
                    auto elemResult = buildExpression(tupleExpr->elements[i].get());

                    // (AR) تجسيد الثوابت قبل تخزينها
                    // (EN) Materialize constants before storing
                    if (elemResult.isConstant && elemResult.type != SadTypeKind::Tuple && elemResult.type != SadTypeKind::Array && currentBlock_)
                    {
                        std::string reg = newTempRegister();
                        elemResult.registerName = reg;
                        SIRInstruction moveInst(SIROpcode::MOVE);
                        moveInst.result = SIROperand::Register(reg, elemResult.type);
                        if (elemResult.type == SadTypeKind::String)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantString(elemResult.constantValue));
                        }
                        else if (elemResult.type == SadTypeKind::Float)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(elemResult.constantValue)));
                        }
                        else if (elemResult.type == SadTypeKind::Boolean)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantBool(elemResult.constantValue == "true" || elemResult.constantValue == "1"));
                        }
                        else
                        {
                            try
                            {
                                moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(elemResult.constantValue)));
                            }
                            catch (const std::exception &)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        currentBlock_->addInstruction(moveInst);
                        elemResult.isConstant = false;
                    }

                    // (AR) نستخدم ARRAY_SET لتخزين العناصر في الصف
                    //      (الصفوف والمصفوفات تشترك في نفس بنية التخزين في runtime)
                    // (EN) Use ARRAY_SET to store elements in tuple
                    //      (Tuples and arrays share same storage structure at runtime)
                    SIRInstruction storeInst;
                    storeInst.opcode = SIROpcode::ARRAY_SET;
                    storeInst.operands.push_back(SIROperand::Register(tupleReg, SadTypeKind::Tuple));
                    storeInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                    storeInst.operands.push_back(SIROperand::Register(elemResult.registerName, elemResult.type));
                    storeInst.comment = "tuple[" + std::to_string(i) + "] = ...";

                    if (currentBlock_)
                    {
                        currentBlock_->addInstruction(storeInst);
                    }
                }

                return BuildResult(tupleReg, SadTypeKind::Tuple);
            }

            // ============================================================================
            // buildExprMap
            // ============================================================================
            BuildResult SIRBuilder::buildExprMap(AST::MapExpr *mapExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found MapExpr with "
                          << mapExpr->pairs.size() << " pairs" << std::endl;
#endif

                // (AR) إنشاء خريطة عبر استدعاء runtime
                // (EN) Create map via runtime call
                std::string mapReg = newTempRegister();
                SIRInstruction createInst;
                createInst.opcode = SIROpcode::CALL;
                createInst.result = SIROperand::Register(mapReg, SadTypeKind::Map);
                createInst.operands.push_back(SIROperand::ConstantString("__sad_map_create"));
                createInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(mapExpr->pairs.size())));
                createInst.comment = "map create {" + std::to_string(mapExpr->pairs.size()) + " pairs}";
                if (currentBlock_)
                    currentBlock_->addInstruction(createInst);

                // (AR) تتبع نوع عنصر الخريطة — يُستخدم لاحقاً عند القراءة
                // (EN) Track map element type — used later for typed get
                SadTypeKind mapElementType = SadTypeKind::Void;

                // (AR) إضافة الأزواج (مفتاح، قيمة) عبر runtime
                // (EN) Insert key-value pairs via runtime
                for (size_t i = 0; i < mapExpr->pairs.size(); ++i)
                {
                    // (AR) دعم النشر (spread): إذا كان الزوج spread، دمج الكائن
                    // (EN) Spread support: if pair is spread, merge object
                    if (mapExpr->pairs[i].isSpread())
                    {
                        auto spreadResult = buildExpression(mapExpr->pairs[i].value.get());
                        SIRInstruction mergeInst;
                        mergeInst.opcode = SIROpcode::CALL;
                        mergeInst.operands.push_back(SIROperand::ConstantString("__sad_map_merge"));
                        mergeInst.operands.push_back(SIROperand::Register(mapReg, SadTypeKind::Map));
                        mergeInst.operands.push_back(SIROperand::Register(spreadResult.registerName, spreadResult.type));
                        if (currentBlock_)
                            currentBlock_->addInstruction(mergeInst);
                        continue;
                    }

                    auto keyResult = buildExpression(mapExpr->pairs[i].key.get());
                    auto valResult = buildExpression(mapExpr->pairs[i].value.get());

                    // (AR) تجسيد الثوابت إلى سجلات
                    // (EN) Materialize constants to registers
                    auto materializeResult = [&](BuildResult &res) -> SIROperand
                    {
                        if (res.isConstant)
                        {
                            std::string reg = newTempRegister();
                            res.registerName = reg;
                            SIRInstruction moveInst(SIROpcode::MOVE);
                            moveInst.result = SIROperand::Register(reg, res.type);
                            if (res.type == SadTypeKind::String)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantString(res.constantValue));
                            }
                            else if (res.type == SadTypeKind::Float)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(res.constantValue)));
                            }
                            else if (res.type == SadTypeKind::Boolean)
                            {
                                moveInst.operands.push_back(SIROperand::ConstantBool(res.constantValue == "true" || res.constantValue == "1"));
                            }
                            else
                            {
                                try
                                {
                                    moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(res.constantValue)));
                                }
                                catch (const std::exception &)
                                {
#ifndef NDEBUG
                                    std::cerr << "[SIR] تحذير: فشل تحويل '" << res.constantValue << "' إلى I64، القيمة الافتراضية 0\n";
#endif
                                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                            }
                            if (currentBlock_)
                                currentBlock_->addInstruction(moveInst);
                            res.isConstant = false;
                        }
                        return SIROperand::Register(res.registerName, res.type);
                    };

                    SIROperand keyOp = materializeResult(keyResult);
                    SIROperand valOp = materializeResult(valResult);

                    // (AR) تتبع أنواع القيم — إذا تنوعت نعيّن Void (مختلط)
                    // (EN) Track value types — if mixed, set Void (heterogeneous)
                    if (i == 0 || !mapExpr->pairs[0].isSpread())
                    {
                        if (mapElementType == SadTypeKind::Void)
                            mapElementType = valResult.type;
                        else if (mapElementType != valResult.type)
                            mapElementType = SadTypeKind::Void; // (AR) أنواع مختلطة
                    }

                    // (AR) إدراج الزوج في الخريطة — تخزين مطبوع: القيمة كـ i64 + نوعها
                    // (EN) Insert pair into map — typed storage: value as i64 + type tag
                    SIRInstruction setInst;
                    setInst.opcode = SIROpcode::CALL;
                    setInst.operands.push_back(SIROperand::ConstantString("__sad_map_set_typed"));
                    setInst.operands.push_back(SIROperand::Register(mapReg, SadTypeKind::Map));
                    setInst.operands.push_back(keyOp);

                    // (AR) تحويل القيمة إلى i64 حسب نوعها
                    // (EN) Convert value to i64 based on its type
                    int typeTag = 0; // SVAL_STRING=0, SVAL_INT=1, SVAL_FLOAT=2, SVAL_BOOL=3
                    if (valResult.type == SadTypeKind::Integer)
                    {
                        setInst.operands.push_back(valOp);
                        typeTag = 1;
                    }
                    else if (valResult.type == SadTypeKind::Float)
                    {
                        // (AR) عشري — نحوّل لنص ثم نستخدم __sad_map_set العادي بنوع 0
                        //      لتجنب bitcast غير المدعوم في SIR حالياً
                        // (EN) Float — convert to string, use __sad_map_set via typed with type 0
                        std::string strReg = newTempRegister();
                        SIRInstruction toStrInst(SIROpcode::F64_TO_STRING);
                        toStrInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                        toStrInst.operands.push_back(valOp);
                        if (currentBlock_)
                            currentBlock_->addInstruction(toStrInst);
                        setInst.operands.push_back(SIROperand::Register(strReg, SadTypeKind::String));
                        typeTag = 0;
                    }
                    else if (valResult.type == SadTypeKind::Boolean)
                    {
                        setInst.operands.push_back(valOp);
                        typeTag = 3;
                    }
                    else
                    {
                        // (AR) نص أو أي نوع آخر — يُمرّر كمؤشر (ptr→i64)
                        // (EN) String or other — passed as pointer (ptr→i64)
                        setInst.operands.push_back(valOp);
                        typeTag = 0;
                    }
                    setInst.operands.push_back(SIROperand::ConstantI64(typeTag));
                    setInst.comment = "map set typed [" + std::to_string(i) + "]";
                    if (currentBlock_)
                        currentBlock_->addInstruction(setInst);
                }

                // (AR) إرجاع نتيجة الخريطة مع نوع العنصر المحفوظ
                // (EN) Return map result with tracked element type
                BuildResult mapResult(mapReg, SadTypeKind::Map);
                mapResult.elementType = mapElementType;
                return mapResult;
            }

            // ============================================================================
            // buildExprWalrus
            // ============================================================================
            BuildResult SIRBuilder::buildExprWalrus(AST::WalrusExpr *walrusExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found WalrusExpr: " << walrusExpr->variable << std::endl;
#endif

                // (AR) بناء تعبير القيمة
                // (EN) Build value expression
                auto valResult = buildExpression(walrusExpr->value.get());

                // (AR) تخصيص متغير جديد وتخزين القيمة فيه
                // (EN) Allocate new variable and store value
                std::string varReg = newTempRegister();

                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(varReg, valResult.type);
                allocInst.operands.push_back(SIROperand::ConstantI64(1));

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(allocInst);
                }

                SIRInstruction storeInst;
                storeInst.opcode = SIROpcode::STORE;
                storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                storeInst.operands.push_back(SIROperand::Register(varReg, valResult.type));

                if (currentBlock_)
                {
                    currentBlock_->addInstruction(storeInst);
                }

                // (AR) تسجيل المتغير في النطاق الحالي
                // (EN) Register variable in current scope
                VariableInfo varInfo;
                varInfo.name = walrusExpr->variable;
                varInfo.type = valResult.type;
                varInfo.registerName = varReg;
                varInfo.isMutable = true;
                varInfo.scopeLevel = currentScopeLevel_;
                addVariable(varInfo);

                // (AR) Walrus يرجع القيمة نفسها
                // (EN) Walrus returns the value itself
                return BuildResult(valResult.registerName, valResult.type);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
