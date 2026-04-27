// ============================================================================
// sir_builder_expr_index.cpp
// ============================================================================
// Index access, index assignment, and ternary expression builders
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"

#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprTernary
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprTernary(AST::TernaryExpr *ternaryExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found TernaryExpr" << std::endl;
#endif

                // (AR) الخطوة 1: بناء الشرط
                // (EN) Step 1: Build condition
                auto condResult = buildExpression(ternaryExpr->condition.get());

                // (AR) الخطوة 2: بناء فرعي then و else أولاً لتحديد النوع الفعلي
                //      ⚠️ مهم: نبني التعبيرين في كتل منفصلة لنعرف النوع قبل إصدار ALLOC
                //      هذا يحل مشكلة تخزين النصوص كـ ptrtoint(i64) ثم طباعتها كأرقام
                // (EN) Step 2: Build both branches first to determine actual type
                //      This fixes the bug where strings stored as ptrtoint(i64) print as numbers

                // (AR) الخطوة 3: إنشاء الكتل الأساسية والفرع الشرطي
                // (EN) Step 3: Create basic blocks and conditional branch
                std::string thenLabel = b_.newLabel("ternary_then");
                std::string elseLabel = b_.newLabel("ternary_else");
                std::string mergeLabel = b_.newLabel("ternary_merge");

                auto thenBlock = b_.createBasicBlock(thenLabel);
                auto elseBlock = b_.createBasicBlock(elseLabel);
                auto mergeBlock = b_.createBasicBlock(mergeLabel);

                // (AR) حفظ الكتلة الحالية لبناء الفرعين مؤقتاً لاستنتاج النوع
                // (EN) Save current block; build branches temporarily to infer type
                auto savedBlock = b_.currentBlock_;

                // (AR) بناء فرع then مؤقتاً لاستنتاج النوع
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(thenBlock);
                }
                b_.currentBlock_ = thenBlock;
                auto trueResult = buildExpression(ternaryExpr->trueExpr.get());

                // (AR) حفظ الكتلة النشطة بعد بناء trueExpr
                //      إذا كان trueExpr عاملاً ثلاثياً متداخلاً، فـ b_.currentBlock_ الآن
                //      يشير إلى mergeBlock الداخلي (وليس thenBlock الخارجي).
                // (EN) Save active block after building trueExpr.
                //      If trueExpr was a nested ternary, b_.currentBlock_ now points to
                //      the inner mergeBlock (not the outer thenBlock).
                auto thenExitBlock = b_.currentBlock_;

                // (AR) بناء فرع else مؤقتاً لاستنتاج النوع
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(elseBlock);
                }
                b_.currentBlock_ = elseBlock;
                auto falseResult = buildExpression(ternaryExpr->falseExpr.get());

                // (AR) حفظ الكتلة النشطة بعد بناء falseExpr
                //      إذا كان falseExpr عاملاً ثلاثياً متداخلاً، فـ b_.currentBlock_ الآن
                //      يشير إلى mergeBlock الداخلي (وليس elseBlock الخارجي).
                //      نحتاج هذه الكتلة في الخطوة 7 لإضافة STORE + BR بشكل صحيح.
                // (EN) Save active block after building falseExpr.
                //      If falseExpr was a nested ternary, b_.currentBlock_ now points to
                //      the inner mergeBlock (not the outer elseBlock).
                //      We need this block in step 7 to add STORE + BR correctly.
                auto elseExitBlock = b_.currentBlock_;

                // (AR) تحديد النوع الفعلي بناءً على الفرعين
                //      إذا كان أحدهما نصاً، يكون النوع STRING
                //      إذا كان أحدهما عشرياً، يكون النوع F64
                //      وإلا يكون I64
                // (EN) Determine actual type from both branches
                SadTypeKind allocType = SadTypeKind::Integer;
                if (trueResult.type == SadTypeKind::String || falseResult.type == SadTypeKind::String)
                {
                    allocType = SadTypeKind::String;
                }
                else if (trueResult.type == SadTypeKind::Float || falseResult.type == SadTypeKind::Float)
                {
                    allocType = SadTypeKind::Float;
                }
                else if (trueResult.type == SadTypeKind::Boolean && falseResult.type == SadTypeKind::Boolean)
                {
                    allocType = SadTypeKind::Boolean;
                }

                // (AR) الآن نعود للكتلة الأصلية لإصدار ALLOC والفرع الشرطي
                // (EN) Now return to original block to emit ALLOC and conditional branch
                b_.currentBlock_ = savedBlock;

                // (AR) تسجيل متغير مؤقت بالنوع الفعلي المستنتج
                // (EN) Register temp variable with inferred type
                std::string ternaryAllocaReg = b_.newTempRegister();

                VariableInfo ternaryVarInfo;
                ternaryVarInfo.name = ternaryAllocaReg;
                ternaryVarInfo.type = allocType;
                ternaryVarInfo.registerName = ternaryAllocaReg;
                ternaryVarInfo.isMutable = true;
                b_.addVariable(ternaryVarInfo);

                // (AR) الخطوة 4: إصدار ALLOC **في الكتلة الحالية قبل br.cond**
                //      هذا يضمن أن alloca يُنشأ دائماً بغض النظر عن الفروع
                // (EN) Step 4: Emit ALLOC **in current block BEFORE br.cond**
                {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register(ternaryAllocaReg, allocType);
                    allocInst.operands.push_back(SIROperand::ConstantI64(1));
                    allocInst.comment = "ternary result alloca (type-aware)";
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(allocInst);
                    }
                }

                // (AR) الخطوة 5: الفرع الشرطي
                // (EN) Step 5: Conditional branch
                if (b_.currentBlock_)
                {
                    SIRInstruction brInst = SIRInstruction::BranchCond(
                        SIROperand::Register(condResult.registerName, SadTypeKind::Boolean),
                        SIROperand::Label(thenLabel),
                        SIROperand::Label(elseLabel));
                    b_.currentBlock_->addInstruction(brInst);
                }

                // (AR) الخطوة 6: إكمال فرع then — STORE + BR merge
                //      نستخدم thenExitBlock بدلاً من thenBlock للسبب نفسه —
                //      إذا كان trueExpr عاملاً ثلاثياً متداخلاً، فـ thenBlock
                //      ينتهي بالفعل بـ BR_COND من العامل الداخلي.
                // (EN) Step 6: Complete then branch — STORE + BR merge
                //      Use thenExitBlock instead of thenBlock for the same reason.
                b_.currentBlock_ = thenExitBlock;
                if (b_.currentBlock_)
                {
                    SIRInstruction storeTrue;
                    storeTrue.opcode = SIROpcode::STORE;
                    storeTrue.operands.push_back(SIROperand::Register(trueResult.registerName, trueResult.type));
                    storeTrue.operands.push_back(SIROperand::Register(ternaryAllocaReg, allocType));
                    storeTrue.comment = "ternary true -> temp";
                    b_.currentBlock_->addInstruction(storeTrue);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) الخطوة 7: إكمال فرع else — STORE + BR merge
                //      نستخدم elseExitBlock بدلاً من elseBlock لأنه إذا كان
                //      falseExpr عاملاً ثلاثياً متداخلاً، فـ elseBlock ينتهي
                //      بالفعل بـ BR_COND (من العامل الداخلي). إضافة تعليمات
                //      بعد BR_COND تجعلها ميتة (dead code). الحل هو إضافة
                //      STORE + BR في كتلة merge الداخلية التي هي آخر كتلة نشطة.
                // (EN) Step 7: Complete else branch — STORE + BR merge
                //      Use elseExitBlock instead of elseBlock because if falseExpr
                //      was a nested ternary, elseBlock already ends with BR_COND.
                //      Adding instructions after BR_COND makes them dead code.
                //      The fix: add STORE + BR in the inner merge block.
                b_.currentBlock_ = elseExitBlock;
                if (b_.currentBlock_)
                {
                    SIRInstruction storeFalse;
                    storeFalse.opcode = SIROpcode::STORE;
                    storeFalse.operands.push_back(SIROperand::Register(falseResult.registerName, falseResult.type));
                    storeFalse.operands.push_back(SIROperand::Register(ternaryAllocaReg, allocType));
                    storeFalse.comment = "ternary false -> temp";
                    b_.currentBlock_->addInstruction(storeFalse);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) الخطوة 8: كتلة الدمج — LOAD النتيجة بالنوع الصحيح
                // (EN) Step 8: Merge block — LOAD result with correct type
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                }
                b_.currentBlock_ = mergeBlock;

                std::string loadReg = b_.newTempRegister();
                SIRInstruction loadInst;
                loadInst.opcode = SIROpcode::LOAD;
                loadInst.result = SIROperand::Register(loadReg, allocType);
                loadInst.operands.push_back(SIROperand::Register(ternaryAllocaReg, allocType));
                loadInst.comment = "ternary result load";
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(loadInst);
                }

                // (AR) نُرجع النتيجة بالنوع الفعلي المستنتج
                // (EN) Return result with actual inferred type
                return BuildResult(loadReg, allocType);
            }

            // ============================================================================
            // buildExprIndex
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprIndex(AST::IndexExpr *indexExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found IndexExpr" << std::endl;
#endif

                // (AR) بناء تعبير الكائن وتعبير الفهرس
                // (EN) Build object expression and index expression
                auto objResult = buildExpression(indexExpr->object.get());
                auto idxResult = buildExpression(indexExpr->index.get());

                // ================================================================
                // (AR) تحميل عامل الفهرسة الزائد []: إذا كان الكائن من نوع صنف، ابحث عن __op_index__
                //      هذا يتوافق مع سلوك المفسر في expression_evaluator_calls.cpp:visitIndexExpr
                // (EN) Index operator overload []: if object is a class, look for __op_index__
                //      This matches interpreter behavior in expression_evaluator_calls.cpp:visitIndexExpr
                // ================================================================
                std::string objClassName = objResult.className;
                if (objClassName.empty() && !objResult.registerName.empty())
                {
                    auto it = b_.classInstanceTypes_.find(objResult.registerName);
                    if (it != b_.classInstanceTypes_.end())
                    {
                        objClassName = it->second;
                    }
                }

                if (!objClassName.empty())
                {
                    // (AR) البحث عن __op_index__ في سلسلة الوراثة
                    // (EN) Search for __op_index__ in inheritance chain
                    std::string searchClass = objClassName;
                    std::string fullOpName;
                    bool found = false;
                    while (!searchClass.empty())
                    {
                        fullOpName = searchClass + ".__op_index__";
                        auto funcIt = b_.functionTable_.find(fullOpName);
                        if (funcIt != b_.functionTable_.end())
                        {
                            found = true;
                            break;
                        }
                        auto parentClass = b_.module_->getClass(searchClass);
                        if (parentClass && !parentClass->parentClass.empty())
                        {
                            searchClass = parentClass->parentClass;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (found)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildExpression: dispatching IndexExpr to operator overload '"
                                  << fullOpName << "'" << std::endl;
#endif

                        std::string resultReg = b_.newTempRegister();
                        auto &opInfo = b_.functionTable_[fullOpName];
                        SadTypeKind returnType = opInfo.returnType;

                        if (b_.currentBlock_)
                        {
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(resultReg, returnType);
                            callInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_index__"));
                            // (AR) إضافة معامل الفهرس
                            // (EN) Add index operand
                            if (idxResult.isConstant && !idxResult.constantValue.empty())
                            {
                                switch (idxResult.type)
                                {
                                case SadTypeKind::Integer:
                                    callInst.operands.push_back(SIROperand::ConstantI64(std::stoll(idxResult.constantValue)));
                                    break;
                                case SadTypeKind::String:
                                    callInst.operands.push_back(SIROperand::ConstantString(idxResult.constantValue));
                                    break;
                                default:
                                    callInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                                    break;
                                }
                            }
                            else
                            {
                                callInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                            }
                            b_.currentBlock_->addInstruction(callInst);
                        }

                        BuildResult result(resultReg, returnType);
                        // (AR) نقل className فقط إذا كان نوع الإرجاع يمثل كائناً (ليس نوعاً بدائياً)
                        //      هذا يمنع اطبع_سطر من محاولة __op_tostring__ على قيمة بدائية مثل رقم
                        // (EN) Only propagate className if return type represents an object (not primitive)
                        //      This prevents اطبع_سطر from attempting __op_tostring__ on a primitive like int
                        if (returnType != SadTypeKind::Integer && returnType != SadTypeKind::Float &&
                            returnType != SadTypeKind::String && returnType != SadTypeKind::Boolean &&
                            returnType != SadTypeKind::Array)
                        {
                            result.className = objClassName;
                        }
                        return result;
                    }
                }

                // (AR) تجسيد الفهرس إذا كان ثابتاً
                // (EN) Materialize index if constant
                if (idxResult.isConstant && b_.currentBlock_)
                {
                    std::string reg = b_.newTempRegister();
                    idxResult.registerName = reg;
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(reg, idxResult.type);
                    if (idxResult.type == SadTypeKind::String)
                    {
                        // (AR) تجسيد المفتاح النصي — مهم لوصول الخرائط بالمفتاح
                        // (EN) Materialize string key — important for map key access
                        moveInst.operands.push_back(SIROperand::ConstantString(idxResult.constantValue));
                    }
                    else if (idxResult.type == SadTypeKind::Float)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(idxResult.constantValue)));
                    }
                    else
                    {
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(idxResult.constantValue)));
                        }
                        catch (const std::exception &)
                        {
#ifndef NDEBUG
                            std::cerr << "[SIR] تحذير: فشل تحويل '" << idxResult.constantValue << "' إلى I64، القيمة الافتراضية 0\n";
#endif
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                    idxResult.isConstant = false;
                }

                // (AR) استنتاج نوع النتيجة: إذا كان الكائن مصفوفة وعنصره مصفوفة → ARRAY، وإلا → نوع العنصر
                // (EN) Infer result type: if object is array with array elements → ARRAY, else → elementType
                SadTypeKind resultType = SadTypeKind::Integer;
                if (objResult.type == SadTypeKind::Array)
                {
                    if (objResult.elementType == SadTypeKind::Array)
                    {
                        resultType = SadTypeKind::Array;
                    }
                    else if (objResult.elementType != SadTypeKind::Void)
                    {
                        resultType = objResult.elementType;
                    }
                }
                else if (objResult.type == SadTypeKind::String)
                {
                    // (AR) فهرسة نص — استدعاء sad_llvm_string_utf8_char_at لإرجاع حرف UTF-8 كنص
                    // (EN) String indexing — call sad_llvm_string_utf8_char_at to return UTF-8 char as string
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CALL;
                    callInst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    callInst.operands.push_back(SIROperand::ConstantString("sad_llvm_string_utf8_char_at"));
                    callInst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    callInst.operands.push_back(SIROperand::Register(idxResult.registerName, SadTypeKind::Integer));
                    callInst.comment = "string UTF-8 char at index";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(callInst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }
                else if (objResult.type == SadTypeKind::Map || objResult.type == SadTypeKind::Struct)
                {
                    // (AR) وصول خريطة بالمفتاح — نستخدم elementType لتحديد دالة القراءة والنوع المُرجع
                    //      للخرائط المختلطة (elementType=Void) نستخدم __sad_map_get الذكي
                    //      الذي يفحص type tag ويحول الأرقام لنصوص تلقائياً
                    // (EN) Map key access — use elementType to pick the right get function and return type
                    //      For heterogeneous maps (elementType=Void) use smart __sad_map_get
                    //      which checks type tag and auto-converts integers to strings
                    SadTypeKind mapElemType = objResult.elementType;

                    if (mapElemType == SadTypeKind::Integer || mapElemType == SadTypeKind::Boolean)
                    {
                        // (AR) القيم رقمية/منطقية — نستدعي __sad_map_get_i64 ونُرجع i64
                        // (EN) Numeric/boolean values — call __sad_map_get_i64, return i64
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction getInst;
                        getInst.opcode = SIROpcode::CALL;
                        getInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                        getInst.operands.push_back(SIROperand::ConstantString("__sad_map_get_i64"));
                        getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        getInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        getInst.comment = "map get i64 by key";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(getInst);

                        BuildResult res(resultReg, mapElemType);
                        res.isDirectValue = true;
                        return res;
                    }
                    else if (mapElemType == SadTypeKind::Float)
                    {
                        // (AR) القيم عشرية — نستدعي __sad_map_get (نصي) ثم نحوّل لعشري
                        // (EN) Float values — call __sad_map_get (string) then convert to float
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction getInst;
                        getInst.opcode = SIROpcode::CALL;
                        getInst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                        getInst.operands.push_back(SIROperand::ConstantString("__sad_map_get"));
                        getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        getInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        getInst.comment = "map get string for float by key";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(getInst);

                        std::string dblReg = b_.newTempRegister();
                        SIRInstruction toF64Inst(SIROpcode::STRING_TO_F64);
                        toF64Inst.result = SIROperand::Register(dblReg, SadTypeKind::Float);
                        toF64Inst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::String));
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(toF64Inst);

                        BuildResult res(dblReg, SadTypeKind::Float);
                        res.isDirectValue = true;
                        return res;
                    }
                    else if (mapElemType == SadTypeKind::Void)
                    {
                        // (AR) خريطة مختلطة الأنواع — نستخدم __sad_map_get الذكي
                        //      يقرأ type tag: إذا نص→يُرجع ptr مباشرة، إذا رقم→يحوله لنص عبر sprintf
                        //      النتيجة دائماً نص (ptr) — للمقارنة مع أرقام يجب تحويل النوع
                        // (EN) Heterogeneous map — use smart __sad_map_get that reads type tag
                        //      Result is always string (ptr)
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction getInst;
                        getInst.opcode = SIROpcode::CALL;
                        getInst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                        getInst.operands.push_back(SIROperand::ConstantString("__sad_map_get"));
                        getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        getInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        getInst.comment = "map get (heterogeneous) by key";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(getInst);

                        return BuildResult(resultReg, SadTypeKind::String);
                    }
                    else
                    {
                        // (AR) نصوص — نستدعي __sad_map_get (يُرجع نصاً)
                        // (EN) Strings — call __sad_map_get (returns string)
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction getInst;
                        getInst.opcode = SIROpcode::CALL;
                        getInst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                        getInst.operands.push_back(SIROperand::ConstantString("__sad_map_get"));
                        getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        getInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        getInst.comment = "map get string by key";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(getInst);

                        return BuildResult(resultReg, SadTypeKind::String);
                    }
                }

                // (AR) تعليمة ARRAY_GET للوصول بالفهرس
                // (EN) ARRAY_GET instruction for indexed access
                std::string resultReg = b_.newTempRegister();
                SIRInstruction takeInst;
                takeInst.opcode = SIROpcode::ARRAY_GET;
                takeInst.result = SIROperand::Register(resultReg, resultType);
                takeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                takeInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                takeInst.comment = "array element get";

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(takeInst);
                }

                BuildResult result(resultReg, resultType);
                // (AR) إذا كان العنصر مصفوفة، نرث نوع العنصر الداخلي
                // (EN) If element is array, inherit the inner element type (for chained indexing)
                if (resultType == SadTypeKind::Array)
                {
                    result.elementType = SadTypeKind::Integer; // default inner element type
                }
                return result;
            }

            // ============================================================================
            // buildExprIndexAssign
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprIndexAssign(AST::IndexAssignExpr *indexAssignExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found IndexAssignExpr" << std::endl;
#endif

                // (AR) بناء تعابير الكائن والفهرس والقيمة
                // (EN) Build object, index, and value expressions
                auto objResult = buildExpression(indexAssignExpr->object.get());
                auto idxResult = buildExpression(indexAssignExpr->index.get());
                auto valResult = buildExpression(indexAssignExpr->value.get());

                // ================================================================
                // (AR) تحميل عامل الإسناد بالفهرس الزائد []=: إذا كان الكائن من نوع صنف، ابحث عن __op_index_set__
                //      هذا يتوافق مع سلوك المفسر في expression_evaluator_members.cpp:visitIndexAssignExpr
                //      العامل يأخذ معاملين: الفهرس والقيمة
                // (EN) Index-assign operator overload []=: if object is a class, look for __op_index_set__
                //      This matches interpreter behavior in expression_evaluator_members.cpp:visitIndexAssignExpr
                //      Operator takes two params: index and value
                // ================================================================
                std::string objClassName = objResult.className;
                if (objClassName.empty() && !objResult.registerName.empty())
                {
                    auto it = b_.classInstanceTypes_.find(objResult.registerName);
                    if (it != b_.classInstanceTypes_.end())
                    {
                        objClassName = it->second;
                    }
                }

                if (!objClassName.empty())
                {
                    // (AR) البحث عن __op_index_set__ في سلسلة الوراثة
                    // (EN) Search for __op_index_set__ in inheritance chain
                    std::string searchClass = objClassName;
                    std::string fullOpName;
                    bool found = false;
                    while (!searchClass.empty())
                    {
                        fullOpName = searchClass + ".__op_index_set__";
                        auto funcIt = b_.functionTable_.find(fullOpName);
                        if (funcIt != b_.functionTable_.end())
                        {
                            found = true;
                            break;
                        }
                        auto parentClass = b_.module_->getClass(searchClass);
                        if (parentClass && !parentClass->parentClass.empty())
                        {
                            searchClass = parentClass->parentClass;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (found)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildExpression: dispatching IndexAssignExpr to operator overload '"
                                  << fullOpName << "'" << std::endl;
#endif

                        std::string resultReg = b_.newTempRegister();
                        auto &opInfo = b_.functionTable_[fullOpName];
                        SadTypeKind returnType = opInfo.returnType;

                        if (b_.currentBlock_)
                        {
                            // (AR) استدعاء عامل []=: OBJECT_CALL(self, __op_index_set__, فهرس, قيمة)
                            // (EN) Index-assign op call: OBJECT_CALL(self, __op_index_set__, index, value)
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(resultReg, returnType);
                            callInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_index_set__"));
                            // (AR) إضافة الفهرس والقيمة كمعاملات
                            // (EN) Add index and value as operands
                            callInst.operands.push_back(idxResult.isConstant
                                                            ? SIROperand::ConstantI64(std::stoll(idxResult.constantValue))
                                                            : SIROperand::Register(idxResult.registerName, idxResult.type));
                            callInst.operands.push_back(valResult.isConstant
                                                            ? SIROperand::ConstantI64(std::stoll(valResult.constantValue))
                                                            : SIROperand::Register(valResult.registerName, valResult.type));
                            b_.currentBlock_->addInstruction(callInst);
                        }

                        BuildResult result(resultReg, returnType);
                        // (AR) نقل className فقط إذا كان نوع الإرجاع يمثل كائناً (ليس نوعاً بدائياً)
                        // (EN) Only propagate className if return type represents an object (not primitive)
                        if (returnType != SadTypeKind::Integer && returnType != SadTypeKind::Float &&
                            returnType != SadTypeKind::String && returnType != SadTypeKind::Boolean &&
                            returnType != SadTypeKind::Array)
                        {
                            result.className = objClassName;
                        }
                        return result;
                    }
                }

                // (AR) تجسيد الفهرس إذا كان ثابتاً
                // (EN) Materialize index if constant
                if (idxResult.isConstant && b_.currentBlock_)
                {
                    std::string reg = b_.newTempRegister();
                    idxResult.registerName = reg;
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(reg, idxResult.type);
                    if (idxResult.type == SadTypeKind::String)
                    {
                        // (AR) تجسيد المفتاح النصي — مهم للخرائط
                        // (EN) Materialize string key — important for maps
                        moveInst.operands.push_back(SIROperand::ConstantString(idxResult.constantValue));
                    }
                    else if (idxResult.type == SadTypeKind::Float)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(idxResult.constantValue)));
                    }
                    else
                    {
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(idxResult.constantValue)));
                        }
                        catch (const std::exception &)
                        {
#ifndef NDEBUG
                            std::cerr << "[SIR] تحذير: فشل تحويل '" << idxResult.constantValue << "' إلى I64، القيمة الافتراضية 0\n";
#endif
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                    idxResult.isConstant = false;
                }

                // (AR) تجسيد القيمة إذا كانت ثابتة
                // (EN) Materialize value if constant
                if (valResult.isConstant && b_.currentBlock_)
                {
                    std::string reg = b_.newTempRegister();
                    valResult.registerName = reg;
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(reg, valResult.type);
                    if (valResult.type == SadTypeKind::String)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantString(valResult.constantValue));
                    }
                    else if (valResult.type == SadTypeKind::Float)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(valResult.constantValue)));
                    }
                    else if (valResult.type == SadTypeKind::Boolean)
                    {
                        moveInst.operands.push_back(SIROperand::ConstantBool(valResult.constantValue == "true" || valResult.constantValue == "1"));
                    }
                    else
                    {
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(valResult.constantValue)));
                        }
                        catch (...)
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                    valResult.isConstant = false;
                }

                // (AR) إذا كان الكائن خريطة → استخدام __sad_map_set_typed بدلاً من ARRAY_SET
                // (EN) If object is a map → use __sad_map_set_typed instead of ARRAY_SET
                if (objResult.type == SadTypeKind::Map || objResult.type == SadTypeKind::Struct)
                {
                    // ================================================================
                    // (AR) [Fix #46] تحديث elementType للمتغير عند أول تخزين في خريطة فارغة:
                    //      عندما تُنشأ خريطة فارغة بـ {} يكون elementType = Void.
                    //      عند تخزين أول قيمة (مثل عداد["أ"] = 0)، نُحدّث elementType
                    //      للمتغير ليعكس نوع القيمة المُخزّنة (Integer هنا).
                    //      بدون هذا: القراءة اللاحقة تستخدم __sad_map_get (نصي) بدل __sad_map_get_i64
                    //      مما يُرجع "0" كنص بدل 0 كرقم → "0" + 1 = "01" بدل 1
                    // (EN) [Fix #46] Update variable elementType on first store to empty map:
                    //      When a map is created empty with {}, elementType = Void.
                    //      On first value store (e.g. counter["a"] = 0), update the variable's
                    //      elementType to match the stored value type (Integer here).
                    //      Without this: subsequent reads use __sad_map_get (string) instead of
                    //      __sad_map_get_i64, returning "0" as string → "0" + 1 = "01" not 1
                    // ================================================================
                    if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(indexAssignExpr->object.get()))
                    {
                        VariableInfo *mapVar = b_.lookupVariable(varExpr->name);
                        if (mapVar && mapVar->elementType == SadTypeKind::Void &&
                            valResult.type != SadTypeKind::Void)
                        {
                            mapVar->elementType = valResult.type;
                        }
                    }

                    // (AR) تخزين مطبوع: نُرسل القيمة كـ i64 مع علامة النوع
                    // (EN) Typed storage: send value as i64 with type tag
                    SIROperand valOp = SIROperand::Register(valResult.registerName, valResult.type);

                    SIRInstruction setInst;
                    setInst.opcode = SIROpcode::CALL;
                    setInst.operands.push_back(SIROperand::ConstantString("__sad_map_set_typed"));
                    setInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    setInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));

                    int typeTag = 0;
                    if (valResult.type == SadTypeKind::Integer)
                    {
                        setInst.operands.push_back(valOp);
                        typeTag = 1;
                    }
                    else if (valResult.type == SadTypeKind::Float)
                    {
                        // (AR) عشري — نحوّل لنص ونخزن كنص (type=0)
                        // (EN) Float — convert to string, store as string (type=0)
                        std::string strReg = b_.newTempRegister();
                        SIRInstruction toStrInst(SIROpcode::F64_TO_STRING);
                        toStrInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                        toStrInst.operands.push_back(valOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(toStrInst);
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
                        setInst.operands.push_back(valOp);
                        typeTag = 0;
                    }
                    setInst.operands.push_back(SIROperand::ConstantI64(typeTag));
                    setInst.comment = "map index-assign set typed";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(setInst);

                    return BuildResult(valResult.registerName, valResult.type);
                }

                // (AR) تعليمة ARRAY_SET لتخزين القيمة في موضع الفهرس
                // (EN) ARRAY_SET instruction to store value at index position
                SIRInstruction storeInst;
                storeInst.opcode = SIROpcode::ARRAY_SET;
                storeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                storeInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                storeInst.comment = "array element set";

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(storeInst);
                }

                return BuildResult(valResult.registerName, valResult.type);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
