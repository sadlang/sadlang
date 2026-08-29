// ============================================================================
// sir_builder_expr_collections.cpp
// ============================================================================
// Collection expression builders (arrays, tuples, maps, walrus)
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "sir_constants.h" // (AR) أوسامُ قيمةِ الخريطة — عقدٌ مشترَكٌ مع الخلفيّة
#include "sad_debug_log.h"

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
            BuildResult ExpressionBuilder::buildExprArray(AST::ArrayExpr *arrayExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found ArrayExpr with "
                          << arrayExpr->elements.size() << " elements");
#endif

                // (AR) تخصيص مصفوفة جديدة
                // (EN) Allocate new array
                std::string arrReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ARRAY_NEW;
                allocInst.result = SIROperand::Register(arrReg, SadTypeKind::Array);
                allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(arrayExpr->elements.size())));
                allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(arrayExpr->elements.size())));
                allocInst.comment = "array new [" + std::to_string(arrayExpr->elements.size()) + "]";

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(allocInst);
                }

                // (AR) تخزين العناصر واحداً تلو الآخر
                // (EN) Store elements one by one
                SadTypeKind inferredElementType = SadTypeKind::Void;
                std::string inferredElementClassName;
                // (AR) ISSUE-067/070: تتبّع تجانس أنواع العناصر. المصفوفة المختلطة
                //      (`[[1،2]، 9]`) كانت تُستنتَج `Array` من العنصر الأوّل **كذبًا**،
                //      فيثق بها مُطابِق الأنماط المتداخل وينزل على عددٍ كمؤشّر ⇒ Segfault.
                //      حين تختلف الأنواع نُبقي نوع العنصر `Void` (مجهول) — وهو الصادق —
                //      فترفضه البوّابة الصارمة وتسقط المطابقة للافتراضيّ بأمان.
                // (EN) ISSUE-067/070: track element-type homogeneity. A mixed array was
                //      inferred as `Array` from element 0 — a lie the nested matcher trusts,
                //      dereferencing a scalar-as-pointer ⇒ Segfault. On heterogeneous types
                //      keep the element type `Void` (honest) so the strict gate rejects it.
                bool elementTypesHomogeneous = true;
                // (AR) [عناصر موسومة — option A] هل كلّ العناصر قِيَمٌ عدديّة/نصّيّة/منطقيّة؟
                //      نقصر التعليبَ على المختلطة القياسيّة (عدد/عشريّ/نصّ/منطقيّ) — لا
                //      المصفوفات/الخرائط المتداخلة — لتجنّب مسارَي المطابقة/الفهرسة المتسلسلة.
                // (EN) [boxed elements — option A] are all elements scalar (number/float/
                //      string/bool)? We box ONLY scalar-heterogeneous arrays, not ones with
                //      nested arrays/maps, to avoid the nested-matcher/chained-index paths.
                // (AR) المصفوفةُ عنصرًا: `DynKind::Array` موجودٌ في مصدرِ الحقيقة
                //      (`kDynKindArray=5`) وتكتبه الخلفيّةُ وتقرؤه، لكنّ هذا المُسنَد كان
                //      يستثنيه فتسقط `[[1، 2]، "س"]` إلى المسارِ الساكن (وسوم=null) ⇒
                //      تُقرأ العناصرُ عدديًّا فتُطبع عناوين. والمصفوفةُ — كالنصّ — مؤشّرٌ
                //      في خانةِ ثمانيةِ بايتات، فوسمُها كافٍ.
                // (EN) An array as an element: DynKind::Array exists in the SoT
                //      (kDynKindArray=5) and the backend both writes and reads it, but this
                //      predicate excluded it, so `[[1,2], "s"]` fell to the static path
                //      (tags=null) and its elements printed as addresses. An array — like a
                //      string — is a pointer in the 8-byte slot, so tagging it suffices.
                // (AR) ز.٤٣: والخريطةُ مثلُها تمامًا — `DynKind::Map=6` موجودٌ في
                //      `sad_dyn_repr.h`، و`toDyn` تكتبه، و`normalizeMapPtr` تقرؤه وتحرسه.
                //      كان استثناؤها هنا يُسقِط `[خريطة، عدد]` إلى المسارِ الساكن
                //      (وسوم=null) فتُقرأ الخريطةُ بافتراضِ `homogKind=Int` ⇒ «رقم» بدل
                //      «خريطة»، وتنهار `.احصل()` عليها. وهذا بعينِه ما يحجب مكتبةَ
                //      جيسون: كلُّ دالّةِ تحليلٍ تُرجع `[القيمة، الموضع]`، وقيمةُ الكائنِ
                //      خريطة. والخريطةُ — كالمصفوفةِ والنصّ — مؤشّرٌ في خانةِ ثمانيةِ
                //      بايتات، فوسمُها كافٍ ولا تعليبَ كومةٍ إضافيًّا.
                // (EN) ز.٤٣: a map is exactly the same case — DynKind::Map=6 exists in
                //      sad_dyn_repr.h, toDyn writes it and normalizeMapPtr reads and guards
                //      it. Excluding it here dropped `[map, int]` to the static path
                //      (tags=null), so the map was read under homogKind=Int ⇒ «رقم» instead
                //      of «خريطة» and `.احصل()` on it collapsed. This is precisely what
                //      blocked the JSON library: every parse function returns
                //      `[value, position]` and an object's value is a map. A map — like an
                //      array and a string — is a pointer in the 8-byte slot, so tagging it
                //      suffices; no extra heap boxing.
                auto isBoxableScalar = [](SadTypeKind t) {
                    return t == SadTypeKind::Integer || t == SadTypeKind::Float ||
                           t == SadTypeKind::String || t == SadTypeKind::Boolean ||
                           t == SadTypeKind::UInt8 || t == SadTypeKind::UInt64 ||
                           t == SadTypeKind::Null || t == SadTypeKind::Any ||
                           t == SadTypeKind::Array || t == SadTypeKind::Map;
                };
                bool allElementsScalar = true;
                // (AR) عنصرٌ ديناميّ النوع (Any، كنتيجة قسمة /،//): يوجب التعليبَ حتّى في
                //      المتجانسة (كلّها Any)، وإلّا نوعُ العنصر Any بلا صناديق ⇒ فكُّ القراءة
                //      يعبث بمؤشّرٍ غير موجود. (لولاه لانكسرت `[1000/2, 1000/3]`.)
                // (EN) A dynamically-typed element (Any, e.g. a /,// result) forces boxing even
                //      in a homogeneous (all-Any) array — else elementType is Any with no boxes,
                //      so the read's unbox dereferences a non-pointer. (Guards `[1000/2,1000/3]`.)
                bool hasDynamicElement = false;

                // (AR) نبني كلّ العناصر أوّلًا (لكشف التجانس قبل التخزين)، ثمّ نخزّن —
                //      إذ لا يُعرَف كون المصفوفة مختلطةً إلّا بعد رؤية كلّ الأنواع.
                // (EN) Build all elements first (to detect heterogeneity before storing),
                //      then store — a mixed array is only known after all types are seen.
                std::vector<BuildResult> builtElems;
                builtElems.reserve(arrayExpr->elements.size());
                for (size_t i = 0; i < arrayExpr->elements.size(); ++i)
                {
                    auto elemResult = buildExpression(arrayExpr->elements[i].get());

                    // (AR) استنتاج نوع العنصر من العنصر الأول + كشف الاختلاط
                    // (EN) Infer element type from first element + detect heterogeneity
                    if (i == 0)
                    {
                        inferredElementType = elemResult.type;
                        if (!elemResult.className.empty())
                        {
                            inferredElementClassName = elemResult.className;
                        }
                    }
                    else if (elemResult.type != inferredElementType)
                    {
                        elementTypesHomogeneous = false;
                    }
                    if (!isBoxableScalar(elemResult.type))
                    {
                        allElementsScalar = false;
                    }
                    if (elemResult.type == SadTypeKind::Any)
                    {
                        hasDynamicElement = true;
                    }

                    // (AR) تجسيد الثوابت قبل تخزينها (نفس الإصلاح المُطبَّق على MapExpr)
                    //      ملاحظة: عناصر المصفوفات المتداخلة (ARRAY) لا تحتاج تجسيداً — هي بالفعل في سجلات
                    // (EN) Materialize constants before storing (same fix applied to MapExpr)
                    //      Note: Nested array elements (ARRAY type) don't need materialization — already in registers
                    if (elemResult.isConstant && elemResult.type != SadTypeKind::Array && b_.currentBlock_)
                    {
                        std::string reg = b_.newTempRegister();
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
                        b_.currentBlock_->addInstruction(moveInst);
                        elemResult.isConstant = false;
                    }
                    builtElems.push_back(elemResult);
                }

                // (AR) [وسم زمن-التشغيل] مختلطةٌ قياسيّة ⇒ نضع علامة elementType=Any على
                //      معامل مصفوفة ARRAY_SET، فتخزّن الخلفيّةُ حمولةَ i64 الخام في خانة
                //      البيانات وتكتب بايتَ النوع (DynKind) في مخزن الوسوم الموازي (الحقل 3).
                //      لا تعليبَ كومةٍ (أُبطِل التصميم القديم boxDynToHeap). المتجانسة/غير-
                //      القياسيّة تبقى على مسارها حرفيًّا (وسوم=null، مسار ساكن).
                // (EN) [runtime tags] Scalar-heterogeneous ⇒ mark the ARRAY_SET array operand
                //      elementType=Any so the backend stores the raw i64 payload in the data
                //      slot and writes the type byte (DynKind) into the parallel tags buffer
                //      (field 3). No heap boxing (the old boxDynToHeap design is retired).
                //      Homogeneous / non-scalar arrays keep their exact prior path (tags=null,
                //      static path).
                const bool boxedHeterogeneous =
                    allElementsScalar && inferredElementType != SadTypeKind::Void &&
                    ((!elementTypesHomogeneous && arrayExpr->elements.size() > 1) ||
                     hasDynamicElement);

                for (size_t i = 0; i < builtElems.size(); ++i)
                {
                    auto &elemResult = builtElems[i];
                    SIRInstruction storeInst;
                    storeInst.opcode = SIROpcode::ARRAY_SET;
                    SIROperand arrOp = SIROperand::Register(arrReg, SadTypeKind::Array);
                    if (boxedHeterogeneous)
                        arrOp.elementType = SadTypeKind::Any; // ⇒ الخلفيّة تُعلّب العنصر
                    storeInst.operands.push_back(arrOp);
                    storeInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(i)));
                    storeInst.operands.push_back(SIROperand::Register(elemResult.registerName, elemResult.type));
                    storeInst.comment = "array[" + std::to_string(i) + "] = ...";

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(storeInst);
                    }
                }

                BuildResult result(arrReg, SadTypeKind::Array);
                // (AR) نوع العنصر: المختلطة القياسيّة ⇒ Any (موسومة)؛ المتجانسة ⇒ نوعها؛
                //      المختلطة غير-القياسيّة تبقى Void (صادق) كما قبلُ (ISSUE-067/070).
                // (EN) Element type: scalar-heterogeneous ⇒ Any (boxed); homogeneous ⇒ its
                //      type; non-scalar-heterogeneous stays Void (honest) as before.
                if (boxedHeterogeneous)
                {
                    result.elementType = SadTypeKind::Any;
                }
                else if (inferredElementType != SadTypeKind::Void && elementTypesHomogeneous)
                {
                    result.elementType = inferredElementType;
                }
                if (!inferredElementClassName.empty())
                {
                    result.elementClassName = inferredElementClassName;
                }
                return result;
            }

            // ============================================================================
            // buildExprTuple
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprTuple(AST::TupleExpr *tupleExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found TupleExpr with "
                          << tupleExpr->elements.size() << " elements");
#endif

                // (AR) تخصيص صف جديد
                // (EN) Allocate new tuple
                std::string tupleReg = b_.newTempRegister();
                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::TUPLE_NEW;
                allocInst.result = SIROperand::Register(tupleReg, SadTypeKind::Tuple);
                allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(tupleExpr->elements.size())));
                // (AR) الطول الابتدائي = عدد العناصر (لتجاوز bounds check عند ARRAY_SET)
                // (EN) Initial length = element count (to pass bounds check during ARRAY_SET)
                allocInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(tupleExpr->elements.size())));
                allocInst.comment = "tuple new (" + std::to_string(tupleExpr->elements.size()) + ")";

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(allocInst);
                }

                // (AR) تخزين العناصر واحداً تلو الآخر (نفس منطق المصفوفة)
                // (EN) Store elements one by one (same logic as array)
                for (size_t i = 0; i < tupleExpr->elements.size(); ++i)
                {
                    auto elemResult = buildExpression(tupleExpr->elements[i].get());

                    // (AR) تجسيد الثوابت قبل تخزينها
                    // (EN) Materialize constants before storing
                    if (elemResult.isConstant && elemResult.type != SadTypeKind::Tuple && elemResult.type != SadTypeKind::Array && b_.currentBlock_)
                    {
                        std::string reg = b_.newTempRegister();
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
                        b_.currentBlock_->addInstruction(moveInst);
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

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(storeInst);
                    }
                }

                return BuildResult(tupleReg, SadTypeKind::Tuple);
            }

            // ============================================================================
            // buildExprMap
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprMap(AST::MapExpr *mapExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found MapExpr with "
                          << mapExpr->pairs.size() << " pairs");
#endif

                // (AR) إنشاء خريطة عبر استدعاء runtime
                // (EN) Create map via runtime call
                std::string mapReg = b_.newTempRegister();
                SIRInstruction createInst;
                createInst.opcode = SIROpcode::CALL;
                createInst.result = SIROperand::Register(mapReg, SadTypeKind::Map);
                createInst.operands.push_back(SIROperand::ConstantString("__sad_map_create"));
                createInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(mapExpr->pairs.size())));
                createInst.comment = "map create {" + std::to_string(mapExpr->pairs.size()) + " pairs}";
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(createInst);

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
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(mergeInst);
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
                            std::string reg = b_.newTempRegister();
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
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(moveInst);
                            res.isConstant = false;
                        }
                        return SIROperand::Register(res.registerName, res.type);
                    };

                    SIROperand keyOp = materializeResult(keyResult);
                    SIROperand valOp = materializeResult(valResult);

                    // (AR) المفتاح يُخزَّن كنصّ (الخريطة تستعمل strdup ومقارنة نصّيّة). حوّل المفاتيح
                    //      العدديّة/العشريّة/المنطقيّة إلى نص حتى يطابقها الوصول `خ[ك]` (ISSUE-044).
                    // (EN) Keys are stored as strings (map uses strdup + string compare). Convert
                    //      integer/float/boolean keys to a string so `خ[k]` lookup matches (ISSUE-044).
                    if (keyResult.type == SadTypeKind::Integer ||
                        keyResult.type == SadTypeKind::Float ||
                        keyResult.type == SadTypeKind::Boolean)
                    {
                        std::string keyStrReg = b_.newTempRegister();
                        SIROpcode kc = (keyResult.type == SadTypeKind::Float)     ? SIROpcode::F64_TO_STRING
                                       : (keyResult.type == SadTypeKind::Boolean) ? SIROpcode::BOOL_TO_STRING
                                                                                  : SIROpcode::I64_TO_STRING;
                        SIRInstruction keyConv(kc);
                        keyConv.result = SIROperand::Register(keyStrReg, SadTypeKind::String);
                        keyConv.operands.push_back(keyOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(keyConv);
                        keyOp = SIROperand::Register(keyStrReg, SadTypeKind::String);
                    }

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
                    // (AR) وسمُ قيمةِ الخريطة — عقدٌ مشترَكٌ مع الخلفيّة، مصدرُه sir_constants.h
                    // (EN) Map value tag — a contract shared with the backend (sir_constants.h)
                    // (AR) [م-٠٠١] العشريُّ كان يُنصَّصُ هنا ويُخزَّنُ بوسمِ النصّ «تجنّبًا
                    //      لـbitcast غيرِ المدعومِ في SIR» — والحاجةُ إليه زالت: القيمةُ
                    //      تُمرَّرُ double والخلفيّةُ تُحوّلُ بتّاتِها عندَ التخزين. فبقيَ
                    //      النوعُ محفوظًا بدل أن يُقرأَ «نصًّا» عندَ أوّلِ حدّ.
                    // (EN) [card م-٠٠١] Floats used to be stringified here and stored under
                    //      the string tag "to avoid a bitcast SIR does not support" — that
                    //      need is gone: the value is passed as a double and the backend
                    //      bitcasts its bits on store. The type survives instead of being
                    //      read back as «نصّ» at the first boundary.
                    setInst.operands.push_back(valOp);
                    const int64_t typeTag = Sad::Compiler::mapValueTagFor(valResult.type);
                    // (AR) وما عداها نصٌّ أو مؤشّرٌ — يُمرَّرُ كما هو بوسمِ النصّ.
                    // (EN) Anything else is a string or a pointer — passed as-is under the
                    //      string tag.
                    setInst.operands.push_back(SIROperand::ConstantI64(typeTag));
                    setInst.comment = "map set typed [" + std::to_string(i) + "]";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(setInst);
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
            BuildResult ExpressionBuilder::buildExprWalrus(AST::WalrusExpr *walrusExpr)
            {
#ifndef NDEBUG
                SAD_DEBUG_LOG_LINE("[DEBUG] buildExpression: found WalrusExpr: " << walrusExpr->variable);
#endif

                // (AR) بناء تعبير القيمة
                // (EN) Build value expression
                auto valResult = buildExpression(walrusExpr->value.get());

                // (AR) تخصيص متغير جديد وتخزين القيمة فيه
                // (EN) Allocate new variable and store value
                std::string varReg = b_.newTempRegister();

                SIRInstruction allocInst;
                allocInst.opcode = SIROpcode::ALLOC;
                allocInst.result = SIROperand::Register(varReg, valResult.type);
                allocInst.operands.push_back(SIROperand::ConstantI64(1));

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(allocInst);
                }

                SIRInstruction storeInst;
                storeInst.opcode = SIROpcode::STORE;
                storeInst.operands.push_back(SIROperand::Register(valResult.registerName, valResult.type));
                storeInst.operands.push_back(SIROperand::Register(varReg, valResult.type));

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(storeInst);
                }

                // (AR) تسجيل المتغير في النطاق الحالي
                // (EN) Register variable in current scope
                VariableInfo varInfo;
                varInfo.name = walrusExpr->variable;
                varInfo.type = valResult.type;
                varInfo.registerName = varReg;
                varInfo.isMutable = true;
                varInfo.scopeLevel = b_.currentScopeLevel_;
                b_.addVariable(varInfo);

                // (AR) Walrus يرجع القيمة نفسها
                // (EN) Walrus returns the value itself
                return BuildResult(valResult.registerName, valResult.type);
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
