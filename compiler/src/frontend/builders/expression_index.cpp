// ============================================================================
// sir_builder_expr_index.cpp
// ============================================================================
// Index access, index assignment, and ternary expression builders
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "sir_constants.h" // (AR) أسماءُ زمنِ تشغيلِ الخريطة — عقدٌ مشترَكٌ مع الخلفيّة

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
                        // (AR) 🔑 و`Any`/`Unknown` كذلك: قيمةٌ معلَّبة (%SadDyn) نوعُها يُقرَأ من
                        //      وسمها زمنَ التشغيل، فليست مؤشِّرَ كائنٍ من صنفِ المستقبِل. ووسمُها
                        //      به يجعل أيَّ استعمالٍ لاحقٍ يقرأ جدولَ الدوالِّ الافتراضيّة من داخلِ
                        //      العلبة: getelementptr %"class.س", %SadDyn %__op_index___virt, …
                        //      وتنشأ حين لا يُصرَّح حقلُ الصنفِ ويُسنَد في الباني فقط، فيُستنتَج
                        //      عائدُ عاملِ الفهرسة ديناميًّا لا بدائيًّا.
                        // (EN) `Any`/`Unknown` too: a boxed value (%SadDyn) carries its type in a
                        //      runtime tag — it is not an object pointer of the receiver's class,
                        //      and tagging it as one makes every later use read a vtable out of
                        //      the box. Arises when a class field is never declared and is only
                        //      assigned in the constructor.
                        if (returnType != SadTypeKind::Integer && returnType != SadTypeKind::Float &&
                            returnType != SadTypeKind::String && returnType != SadTypeKind::Boolean &&
                            returnType != SadTypeKind::Array && returnType != SadTypeKind::Any &&
                            returnType != SadTypeKind::Unknown)
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
                    else if (objResult.elementType == SadTypeKind::Any)
                    {
                        // (AR) [عناصر موسومة — option A] مصفوفةٌ مختلطةٌ قياسيّة: العنصرُ
                        //      صندوقٌ ديناميّ ⇒ نوعُ القراءة Any فيفكّه ARRAY_GET، والطباعة/
                        //      الإسناد يفكّان Any أصلًا (dynToString).
                        // (EN) [boxed elements] scalar-heterogeneous array: the element is a
                        //      dynamic box ⇒ result type Any so ARRAY_GET unboxes it; print/
                        //      assign decode Any natively.
                        resultType = SadTypeKind::Any;
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
                // (AR) [م-٠٠١] «أي» تدخلُ هنا كذلك: نتيجةُ قراءةٍ موسومةٍ زمنَ التشغيل
                //      (`م["أ"]["ب"]`). لا يعرفُ المصرّفُ سكونيًّا أهي خريطةٌ أم غيرُها،
                //      فيتنازلُ للحارسِ زمنَ التشغيل: `normalizeMapPtr` يفحصُ الوسمَ ويفكُّ
                //      الحمولةَ مؤشّرًا، ويفشلُ **صاخبًا** إن لم تكن خريطة. وقبلَ هذا كان
                //      المسارُ يسقطُ إلى فهرسةِ المصفوفاتِ فيُجهِضُ المصرّفَ بتوكيدِ LLVM
                //      (`Op types should be identical`) — وفي بناءِ Release يمرُّ IR فاسدٌ
                //      بلا صوت.
                // (EN) [card م-٠٠١] «أي» enters here too: the result of a runtime-tagged read
                //      (`م["أ"]["ب"]`). The compiler cannot know statically whether it is a map,
                //      so it defers to the runtime guard: `normalizeMapPtr` checks the tag,
                //      unpacks the payload as a pointer, and fails **loudly** if it is not a map.
                //      Before this the path fell through to array indexing and aborted the
                //      compiler on an LLVM assertion (`Op types should be identical`) — and in a
                //      Release build, silently emitted invalid IR instead.
                //
                // (AR) والفهرسُ هو المميِّز: مفتاحُ الخريطةِ نصٌّ دائمًا، فالفهرسُ العدديُّ
                //      على قيمةٍ موسومةٍ يعني مصفوفةً (`م["ج"][0]`) فيمضي إلى ARRAY_GET
                //      حيثُ يحرسُ `normalizeArrayPtr` الوسمَ بالمثل.
                // (EN) The index is the discriminator: a map key is always a string, so an
                //      integer index on a tagged value means an array (`م["ج"][0]`) and goes to
                //      ARRAY_GET, where `normalizeArrayPtr` guards the tag likewise.
                else if (objResult.type == SadTypeKind::Map || objResult.type == SadTypeKind::Struct ||
                         (objResult.type == SadTypeKind::Any &&
                          idxResult.type != SadTypeKind::Integer))
                {
                    // (AR) وصول خريطة بالمفتاح — نستخدم elementType لتحديد دالة القراءة والنوع المُرجع
                    //      للخرائط المختلطة (elementType=Void) نستخدم __sad_map_get الذكي
                    //      الذي يفحص type tag ويحول الأرقام لنصوص تلقائياً
                    // (EN) Map key access — use elementType to pick the right get function and return type
                    //      For heterogeneous maps (elementType=Void) use smart __sad_map_get
                    //      which checks type tag and auto-converts integers to strings
                    // (AR) كائنٌ موسومٌ زمنَ التشغيل ⇒ نوعُ عنصرِه مجهولٌ يقينًا، مهما
                    //      كان `elementType` المحمولُ عن سياقٍ سابق.
                    // (EN) A runtime-tagged object ⇒ its element type is certainly unknown,
                    //      whatever `elementType` was carried over from an earlier context.
                    SadTypeKind mapElemType = (objResult.type == SadTypeKind::Any)
                                                  ? SadTypeKind::Any
                                                  : objResult.elementType;

                    // (AR) المفاتيح تُخزَّن كنصوص؛ حوّل المفتاح العدديّ/العشريّ/المنطقيّ إلى نص قبل
                    //      القراءة ليطابق ما خزّنه `__sad_map_set_typed` (ISSUE-044). نعدّل idxResult
                    //      ليحمل المفتاح النصّيّ فتستعمله كلّ فروع القراءة أدناه.
                    // (EN) Keys are stored as strings; convert integer/float/boolean key to a string
                    //      before lookup so it matches what `__sad_map_set_typed` stored (ISSUE-044).
                    //      Mutate idxResult to hold the string key so all get-branches below use it.
                    if ((idxResult.type == SadTypeKind::Integer ||
                         idxResult.type == SadTypeKind::Float ||
                         idxResult.type == SadTypeKind::Boolean) &&
                        b_.currentBlock_)
                    {
                        std::string keyStrReg = b_.newTempRegister();
                        SIROpcode kc = (idxResult.type == SadTypeKind::Float)     ? SIROpcode::F64_TO_STRING
                                       : (idxResult.type == SadTypeKind::Boolean) ? SIROpcode::BOOL_TO_STRING
                                                                                  : SIROpcode::I64_TO_STRING;
                        SIRInstruction keyConv(kc);
                        keyConv.result = SIROperand::Register(keyStrReg, SadTypeKind::String);
                        keyConv.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        b_.currentBlock_->addInstruction(keyConv);
                        idxResult.registerName = keyStrReg;
                        idxResult.type = SadTypeKind::String;
                    }

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
                        // (AR) [م-٠٠١] القيمُ عشريّةٌ — تُخزَّنُ اليومَ ببتّاتِها بوسمِ
                        //      العشريّ، فتُقرأُ خامًّا وتُعيدُها الخلفيّةُ double وفقَ
                        //      نوعِ سجلِّ النتيجة. كان المسارُ يمرُّ بالنصِّ ذهابًا
                        //      وإيابًا (نصٌّ ثمّ STRING_TO_F64): تخصيصٌ وفقدُ دقّةٍ
                        //      عندَ كلِّ قراءة.
                        // (EN) [card م-٠٠١] Float values are stored as raw bits under the
                        //      float tag, so they are read raw and the backend hands them
                        //      back as a double per the result register's type. The path
                        //      used to round-trip through a string (get + STRING_TO_F64):
                        //      an allocation and a precision loss on every read.
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction getInst;
                        getInst.opcode = SIROpcode::CALL;
                        getInst.result = SIROperand::Register(resultReg, SadTypeKind::Float);
                        getInst.operands.push_back(SIROperand::ConstantString(kRuntimeMapGetI64));
                        getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        getInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        getInst.comment = "map get float bits by key";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(getInst);

                        BuildResult res(resultReg, SadTypeKind::Float);
                        res.isDirectValue = true;
                        return res;
                    }
                    else if (mapElemType == SadTypeKind::Void || mapElemType == SadTypeKind::Any)
                    {
                        // ════════════════════════════════════════════════════
                        // (AR) [م-٠٠١ ق٣] خريطةٌ نوعُ قيمِها مجهولٌ سكونيًّا — معامِلٌ
                        //      مصرَّحٌ `خريطة`، أو ناتجُ دالّةٍ، أو قيمةٌ متداخلة.
                        //
                        //      كان المسارُ يُسوّي كلَّ قيمةٍ إلى نصٍّ ثمّ يُسلّمُها بنوعٍ
                        //      ساكنٍ String. فإن استهلكها سياقٌ عدديٌّ — كدالّةٍ
                        //      مُصرَّحةٍ `رقم` — رُدَّ المؤشّرُ عددًا: `1965798346976`
                        //      بدل `7`، بلا رسالةٍ ولا رمزِ خروجٍ غيرِ صفريّ. وهذا
                        //      أخطرُ ما في العنقودِ: اختبارٌ يمرُّ على محرّكٍ ويكذب.
                        //
                        //      الوسمُ محفوظٌ أصلًا في مصفوفةِ types داخلَ الخريطة،
                        //      فالقراءةُ الموسومةُ تُرجعُ %SadDyn بنوعِ القيمةِ الحقيقيِّ
                        //      وتتنازلُ سلاسلُ الاستهلاكِ القائمةُ (الطباعة، الإرجاع،
                        //      الثنائيّات) لفكِّ التعليبِ عندَ الحاجة.
                        // (EN) [card م-٠٠١, ق٣] A map whose value type is statically
                        //      unknown — a parameter declared `خريطة`, a call result, or
                        //      a nested value.
                        //
                        //      The path used to flatten every value to a string and hand
                        //      it over statically typed String. A numeric consumer — a
                        //      function declared `رقم` — then received the pointer as a
                        //      number: `1965798346976` instead of `7`, with no message
                        //      and a zero exit code. That is the worst defect in this
                        //      cluster: a test that passes on one engine and lies.
                        //
                        //      The tag already lives in the map's types array, so the
                        //      tagged read returns a %SadDyn carrying the value's real
                        //      type and the existing consumer chains (printing, return,
                        //      binary ops) unpack it where needed.
                        // ════════════════════════════════════════════════════
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction getInst;
                        getInst.opcode = SIROpcode::CALL;
                        getInst.result = SIROperand::Register(resultReg, SadTypeKind::Any);
                        getInst.operands.push_back(SIROperand::ConstantString(kRuntimeMapGetDyn));
                        getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        getInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        getInst.comment = "map get (heterogeneous, runtime-tagged) by key";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(getInst);

                        BuildResult result(resultReg, SadTypeKind::Any);
                        result.isDirectValue = true;
                        return result;
                    }
                    else if (mapElemType == SadTypeKind::Map ||
                             mapElemType == SadTypeKind::Array ||
                             mapElemType == SadTypeKind::Struct)
                    {
                        // (AR) [ISSUE-047] قيمة مركّبة متداخلة (خريطة/مصفوفة/بنية): القيمة
                        //      مخزَّنة typeTag=0 فيُرجِعها `__sad_map_get` مؤشّرًا سليمًا
                        //      (inttoptr بلا نسخ). نُعيدها بنوعها المركّب — لا كنصّ — كي تعمل
                        //      الفهرسة التالية «خ["أ"]["ب"]». نوع عنصر المتداخل مجهول وقت
                        //      الترجمة ⇒ Void فيستعمل الوصولُ التالي `__sad_map_get` الذكيّ.
                        // (EN) [ISSUE-047] Nested composite value (map/array/struct): stored with
                        //      typeTag=0 so `__sad_map_get` returns the intact pointer (inttoptr,
                        //      no copy). Return it with its composite type — not String — so a
                        //      following index `m["a"]["b"]` works. Inner element type is unknown
                        //      at compile time ⇒ Void so the next access uses the smart get.
                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction getInst;
                        getInst.opcode = SIROpcode::CALL;
                        getInst.result = SIROperand::Register(resultReg, mapElemType);
                        // (AR) [م-٠٠١] `__sad_map_get_i64` لا `__sad_map_get`: الأخيرةُ تُنسّقُ
                        //      كلَّ وسمٍ غيرِ نصّيٍّ نصًّا، ولمّا صارتِ الحاويةُ تُخزَّنُ بوسمِها
                        //      (٦/٧) بدل وسمِ النصّ عادت منها **نصًّا** لا مؤشّرًا، فتُفهرَسُ
                        //      قمامةً ⇒ SIGSEGV. الحمولةُ الخامُّ هي المطلوبةُ هنا، ويُطبّعُها
                        //      `normalizeMapPtr` مؤشّرًا.
                        // (EN) [card م-٠٠١] `__sad_map_get_i64`, not `__sad_map_get`: the latter
                        //      formats every non-string tag as text, and once a container was
                        //      stored under its own tag (6/7) instead of the string tag it came
                        //      back as **text** rather than a pointer, and indexing that garbage
                        //      crashed. The raw payload is what is wanted here, and
                        //      `normalizeMapPtr` turns it into a pointer.
                        getInst.operands.push_back(SIROperand::ConstantString(kRuntimeMapGetI64));
                        getInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                        getInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));
                        getInst.comment = "map get composite (map/array/struct) raw payload by key";
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(getInst);

                        BuildResult res(resultReg, mapElemType);
                        res.elementType = SadTypeKind::Void;
                        return res;
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

                // (AR) والمُفهرَسُ الموسومُ زمنَ التشغيل: نتيجتُه موسومةٌ مثلُه.
                //
                //      `resultType` يبدأُ `Integer` افتراضًا، ولا فرعَ في السلسلةِ أعلاه
                //      يمسُّ `أي`، فكان `متداخل[0]` يخرجُ بنوعٍ ساكنٍ «رقم» وحمولتُه
                //      `%SadDyn`. والنوعُ الكاذبُ يتسرّبُ إلى ما بعدَه: `متداخل[0].احصل("ب")`
                //      تُشخَّصُ «الطريقة احصل غير موجودة في الصنف رقم» — تشخيصٌ يشيرُ إلى
                //      نوعٍ لم يوجد قطّ. وهي المرآةُ الدقيقةُ لفرعِ `elementType == Any`
                //      أعلاه: هناك الحاويةُ معروفةٌ وعنصرُها موسوم، وهنا الحاويةُ نفسُها
                //      موسومة — والحكمُ واحد.
                //
                //      و`ARRAY_GET` يفكُّ الوسمَ أصلًا عبر `normalizeArrayPtr` ويعلّبُ
                //      النتيجة، فالتغييرُ يصحّحُ الوصفَ الساكنَ ليطابقَ ما يُبعَثُ فعلًا.
                // (EN) A runtime-tagged indexee yields a runtime-tagged result.
                //
                //      `resultType` starts as `Integer`, and no branch in the chain above
                //      touches `Any`, so `nested[0]` came out statically typed «رقم» while
                //      carrying a `%SadDyn`. The false type then leaks downstream:
                //      `nested[0].get("b")` is diagnosed as "method get not found on class
                //      رقم" — a diagnostic naming a type that never existed. This is the exact
                //      mirror of the `elementType == Any` branch above: there the container is
                //      known and its element tagged, here the container itself is tagged — the
                //      judgement is the same.
                //
                //      `ARRAY_GET` already unpacks the tag via `normalizeArrayPtr` and boxes
                //      the result, so this only makes the static description match what is
                //      actually emitted.
                if (objResult.type == SadTypeKind::Any)
                {
                    resultType = SadTypeKind::Any;
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
                        // (AR) 🔑 و`Any`/`Unknown` كذلك: قيمةٌ معلَّبة (%SadDyn) نوعُها يُقرَأ من
                        //      وسمها زمنَ التشغيل، فليست مؤشِّرَ كائنٍ من صنفِ المستقبِل. ووسمُها
                        //      به يجعل أيَّ استعمالٍ لاحقٍ يقرأ جدولَ الدوالِّ الافتراضيّة من داخلِ
                        //      العلبة: getelementptr %"class.س", %SadDyn %__op_index___virt, …
                        //      وتنشأ حين لا يُصرَّح حقلُ الصنفِ ويُسنَد في الباني فقط، فيُستنتَج
                        //      عائدُ عاملِ الفهرسة ديناميًّا لا بدائيًّا.
                        // (EN) `Any`/`Unknown` too: a boxed value (%SadDyn) carries its type in a
                        //      runtime tag — it is not an object pointer of the receiver's class,
                        //      and tagging it as one makes every later use read a vtable out of
                        //      the box. Arises when a class field is never declared and is only
                        //      assigned in the constructor.
                        if (returnType != SadTypeKind::Integer && returnType != SadTypeKind::Float &&
                            returnType != SadTypeKind::String && returnType != SadTypeKind::Boolean &&
                            returnType != SadTypeKind::Array && returnType != SadTypeKind::Any &&
                            returnType != SadTypeKind::Unknown)
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
                //
                // (AR) و«أي» يدخل هنا كذلك، **تناظرًا مع مسارِ القراءةِ أعلاه**: الكتابةُ
                //      المتداخلةُ `م["أ"]["ب"] = 5` هدفُها نتيجةُ قراءةٍ موسومةٍ زمنَ
                //      التشغيل، فنوعُها الساكنُ `أي`. وكان الشرطُ يذكر Map/Struct فقط،
                //      فتسقطُ الكتابةُ إلى ARRAY_SET بفهرسٍ **نصّيّ** ⇒ IR فاسدٌ في
                //      Release وتأكيدٌ حاجبٌ في Debug. والحارسُ زمنَ التشغيلِ واحدٌ في
                //      الطرفَين: `normalizeMapPtr` يفحصُ الوسمَ عند map.set كما عند
                //      map.get.
                // (EN) `أي` enters here too, symmetrically with the read path above: a nested
                //      write `م["أ"]["ب"] = 5` targets a runtime-tagged read result, so its
                //      static type is Any. The condition previously named Map/Struct only, so
                //      the write fell through to ARRAY_SET with a **string** index ⇒ invalid IR
                //      in Release, blocking assert in Debug.
                //
                // (AR) ⚠️ والشرطُ **نصٌّ حصرًا** لا «ليس صحيحًا»: مسارُ القراءةِ يستعمل
                //      `!= Integer` معتمِدًا على أنّ «مفتاحَ الخريطةِ نصٌّ دائمًا» — وهي
                //      دعوى **خاطئة** (اللغةُ تقبل مفاتيحَ عدديّة، والقراءةُ نفسُها
                //      تُنصِّصها). ولو نُسِخت هنا لَصرَفَت `ح[ف] = 99` — حيث `ح` مصفوفةٌ
                //      و`ف` فهرسٌ نوعُه الساكنُ `أي` — إلى مساعِدِ الخريطةِ فيُجهِض
                //      الثنائيُّ زمنَ التشغيل، وهما بُعدان يُتقنهما ARRAY_SET أصلًا
                //      (`normalizeArrayIndex` يفكُّ `%SadDyn` صراحةً). فالنصُّ وحدَه
                //      مفتاحُ خريطةٍ يقينًا، وهو بعينِه ما يرفضه حارسُ الخلفيّة.
                //      🔴 دَينٌ قائمٌ لا تُحدِثه هذه الهُنيْهةُ ولا تسدُّه: `خ["أ"][2] = 5`
                //      (خريطةٌ متداخلةٌ بمفتاحٍ عدديّ) ما زال يُصرَف إلى ARRAY_SET
                //      فيُجهِض. سدُّه يحتاج إرسالًا **بوسمِ الكائنِ زمنَ التشغيل** لا
                //      بالنوعِ الساكنِ للفهرس.
                // (EN) ⚠️ String only, not "not Integer": the read path's premise that a map
                //      key is always a string is false, and copying it here would divert
                //      `arr[i] = v` (Any-typed index) to the map helper and abort at runtime.
                //      Debt: a nested map with a numeric key still misroutes; the real fix is
                //      runtime-tag dispatch, not static index type.
                if (objResult.type == SadTypeKind::Map || objResult.type == SadTypeKind::Struct ||
                    (objResult.type == SadTypeKind::Any && idxResult.type == SadTypeKind::String))
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
                        if (mapVar && valResult.type != SadTypeKind::Void)
                        {
                            if (mapVar->elementType == SadTypeKind::Void)
                                mapVar->elementType = valResult.type;
                            else if (mapVar->elementType != valResult.type &&
                                     mapVar->elementType != SadTypeKind::Any)
                            {
                                // (AR) [م-٠٠١] تخزينٌ لاحقٌ بنوعٍ مغايرٍ ⇒ الخريطةُ مختلطةٌ
                                //      فعلًا. كان النوعُ يُثبَّتُ من **أوّلِ** تخزينٍ ولا
                                //      يُنقَضُ أبدًا، فـ`خ["أ"]=7` ثمّ `خ["ب"]=2.5` تجعلُ
                                //      كلَّ قراءةٍ «صحيحًا»، فيُقرأُ العشريُّ بتّاتِه عددًا:
                                //      `خ["أ"] + خ["ب"]` تُعطي ٤٦١٢٨١١٩١٨٣٣٤٢٣٠٥٣٥ لا ٩٫٥.
                                //      «أي» تُحوّلُ القراءةَ إلى المسارِ الموسومِ زمنَ التشغيل.
                                // (EN) [card م-٠٠١] A later store of a different type ⇒ the map
                                //      really is mixed. The type used to be pinned by the
                                //      **first** store and never invalidated, so `خ["أ"]=7` then
                                //      `خ["ب"]=2.5` made every read «integer» and the float was
                                //      read as its raw bits: `خ["أ"] + خ["ب"]` yielded
                                //      4612811918334230535 instead of 9.5. «أي» switches reads to
                                //      the runtime-tagged path.
                                mapVar->elementType = SadTypeKind::Any;
                            }
                        }
                    }

                    // (AR) المفاتيحُ تُخزَّنُ نصوصًا — نظيرُ التنصيصِ في مسارِ القراءة
                    //      (ISSUE-044) وكان **مفقودًا هنا**: `خ[2] = 5` كانت تُمرِّرُ عددًا
                    //      حيثُ ينتظرُ `__sad_map_set_typed` مؤشّرَ نصٍّ فيُقرَأُ العددُ
                    //      عنوانًا ⇒ **انهيارُ تجزئةٍ** زمنَ التشغيلِ بخروجِ ترجمةٍ ٠.
                    //      والمفسّرُ يطبع ٥. فالطرفانِ يجب أن يُنصِّصا بالطريقةِ نفسِها،
                    //      وإلّا كتبَ أحدُهما بمفتاحٍ لا يقرؤه الآخَر.
                    // (EN) Keys are stored as strings — the counterpart of the read path's
                    //      conversion (ISSUE-044), which was **missing here**: `خ[2] = 5`
                    //      passed an integer where __sad_map_set_typed expects a string
                    //      pointer, so the integer was read as an address ⇒ segfault at run
                    //      time with a 0 compile exit. Both sides must stringify alike.
                    if ((idxResult.type == SadTypeKind::Integer ||
                         idxResult.type == SadTypeKind::Float ||
                         idxResult.type == SadTypeKind::Boolean) &&
                        b_.currentBlock_)
                    {
                        std::string keyStrReg = b_.newTempRegister();
                        SIROpcode keyConvOp = (idxResult.type == SadTypeKind::Float)     ? SIROpcode::F64_TO_STRING
                                              : (idxResult.type == SadTypeKind::Boolean) ? SIROpcode::BOOL_TO_STRING
                                                                                         : SIROpcode::I64_TO_STRING;
                        SIRInstruction keyConv(keyConvOp);
                        keyConv.result = SIROperand::Register(keyStrReg, SadTypeKind::String);
                        keyConv.operands.push_back(
                            SIROperand::Register(idxResult.registerName, idxResult.type));
                        keyConv.comment = "map index-assign key to string";
                        b_.currentBlock_->addInstruction(keyConv);
                        idxResult.registerName = keyStrReg;
                        idxResult.type = SadTypeKind::String;
                    }

                    // (AR) تخزين مطبوع: نُرسل القيمة كـ i64 مع علامة النوع
                    // (EN) Typed storage: send value as i64 with type tag
                    SIROperand valOp = SIROperand::Register(valResult.registerName, valResult.type);

                    SIRInstruction setInst;
                    setInst.opcode = SIROpcode::CALL;
                    setInst.operands.push_back(SIROperand::ConstantString("__sad_map_set_typed"));
                    setInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    setInst.operands.push_back(SIROperand::Register(idxResult.registerName, idxResult.type));

                    // (AR) [م-٠٠١] الأوسامُ من `sir_constants.h` — عقدٌ مشترَكٌ مع الخلفيّة
                    //      كان مكتوبًا هنا أرقامًا عارية. والعشريُّ كان يُنصَّصُ فيُخزَّنُ
                    //      بوسمِ النصّ، فتضيعُ عشريّتُه: `نوع(م["ك"])` تقولُ «نصّ» والمفسّرُ
                    //      «عشريّ». صار يُخزَّنُ ببتّاتِه بوسمِه.
                    // (EN) [card م-٠٠١] Tags come from `sir_constants.h` — a contract shared
                    //      with the backend that used to be written here as bare numbers. And
                    //      floats were stringified and stored under the string tag, losing
                    //      their floatness: `نوع(م["k"])` said «نصّ» where the interpreter said
                    //      «عشريّ». They are now stored as raw bits under their own tag.
                    setInst.operands.push_back(valOp);
                    const int64_t typeTag = mapValueTagFor(valResult.type);
                    setInst.operands.push_back(SIROperand::ConstantI64(typeTag));
                    setInst.comment = "map index-assign set typed";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(setInst);

                    return BuildResult(valResult.registerName, valResult.type);
                }

                // (AR) تعليمة ARRAY_SET لتخزين القيمة في موضع الفهرس. ننشر elementType
                //      إلى معامل المصفوفة: إن كانت مختلطةً قياسيّة (Any) تُعلّب الخلفيّةُ
                //      القيمةَ في %SadDyn (كتابةٌ متّسقة مع الخانات المُعلَّبة، لا فساد).
                // (EN) ARRAY_SET to store the value at the index. Propagate elementType to the
                //      array operand: if the array is scalar-heterogeneous (Any), the backend
                //      boxes the value into a %SadDyn (a write consistent with the boxed slots).
                SIRInstruction storeInst;
                storeInst.opcode = SIROpcode::ARRAY_SET;
                SIROperand arrSetOp = SIROperand::Register(objResult.registerName, objResult.type);
                arrSetOp.elementType = objResult.elementType;
                storeInst.operands.push_back(arrSetOp);
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
