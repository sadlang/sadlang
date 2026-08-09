// ============================================================================
// sir_builder_stmt_types.cpp
// (AR) Type declarations: enum, struct, namespace, class, destructor
// (EN) Type declaration statements
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ================================================================
            // (AR) [أ-م٤] تحويل اسم النوع المصرَّح لحقل حمولة تعداد بحمولة إلى SadTypeKind.
            //      يُستعمل لبذر أنواع الحقول من التصريح مباشرةً (`عدد(رقم قيمة)`) بدل
            //      انتظار استنتاجها من موقع إنشاء (ISSUE-076) — فيعمل الاستخراج بنوعه
            //      الصحيح حتّى لو بُنيَت المطابقة قبل أوّل بناءٍ (كالتعداد ذاتيّ المرجع).
            //      القاعدة: بدائيّ معروف ⇒ نوعه؛ اسمٌ صريح غير بدائيّ (صنف/تعداد) ⇒ مؤشّر
            //      (قيمة ADT مُكوَّمة تُمرَّر مؤشّرًا)؛ فارغ (بلا نوع) ⇒ مجهول (سلوك ISSUE-076).
            // (EN) [A-M4] Map a declared payload-field type name to SadTypeKind. Used to seed
            //      field types straight from the declaration (`Num(int value)`) instead of
            //      waiting for construction-site inference (ISSUE-076) — so extraction runs
            //      with the right type even when the match is built before the first
            //      construction (as with a self-referential enum). Rule: a known primitive ⇒
            //      its type; an explicit non-primitive name (class/enum) ⇒ Pointer (a heap ADT
            //      value is passed by pointer); empty (untyped) ⇒ Unknown (ISSUE-076 behavior).
            static SadTypeKind adtDeclaredTypeNameToKind(const std::string &typeName)
            {
                if (typeName.empty())
                    return SadTypeKind::Unknown;
                if (typeName == "\xD8\xB1\xD9\x82\xD9\x85" /*رقم*/ ||
                    typeName == "\xD8\xB9\xD8\xAF\xD8\xAF" /*عدد*/ ||
                    typeName == "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD" /*صحيح*/ ||
                    typeName == "i64" || typeName == "int" || typeName == "integer")
                    return SadTypeKind::Integer;
                if (typeName == "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A" /*عشري*/ ||
                    typeName == "\xD9\x85\xD8\xB6\xD8\xA7\xD8\xB9\xD9\x81" /*مضاعف*/ ||
                    typeName == "\xD8\xAD\xD9\x82\xD9\x8A\xD9\x82\xD9\x8A" /*حقيقي*/ ||
                    typeName == "f64" || typeName == "float" || typeName == "double")
                    return SadTypeKind::Float;
                if (typeName == "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A" /*منطقي*/ ||
                    typeName == "bool" || typeName == "boolean")
                    return SadTypeKind::Boolean;
                if (typeName == "\xD9\x86\xD8\xB5" /*نص*/ ||
                    typeName == "string" || typeName == "str")
                    return SadTypeKind::String;
                if (typeName == "\xD9\x85\xD8\xB5\xD9\x81\xD9\x88\xD9\x81\xD8\xA9" /*مصفوفة*/ ||
                    typeName == "array")
                    return SadTypeKind::Array;
                // (AR) اسمٌ صريح غير بدائيّ ⇒ صنف/تعداد ⇒ يُمرَّر مؤشّرًا (قيمة ADT مُكوَّمة).
                // (EN) An explicit non-primitive name ⇒ class/enum ⇒ passed by pointer (heap ADT value).
                return SadTypeKind::Pointer;
            }

            bool StatementBuilder::buildStatement_Types(AST::Statement *stmt)
            {
                // ========================================================================
                // (AR) EnumDecl - تصريح تعداد بسيط أو جبري (ADT)
                //      تعداد بسيط: تعداد اللون { أحمر، أخضر، أزرق }
                //      تعداد جبري: تعداد شكل { دائرة(نصف_القطر)، مستطيل(عرض، ارتفاع)، نقطة }
                // (EN) Enum declaration - simple or ADT (Algebraic Data Type)
                //      Simple: enum Color { Red, Green, Blue }
                //      ADT:    enum Shape { Circle(radius), Rectangle(w, h), Point }
                // ========================================================================
                if (auto enumDecl = dynamic_cast<Sad::AST::EnumDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found EnumDecl: " << enumDecl->name
                              << " (isADT=" << enumDecl->isADT() << ")" << std::endl;
#endif

                    if (enumDecl->isADT())
                    {
                        // ==============================================================
                        // (AR) === مسار التعداد الجبري (ADT) ===
                        //      يُمثَّل كـ tagged union: بنية { __tag: I64, __f0: PTR, __f1: PTR, ... }
                        //      لكل حالة: دالة باني تُنشئ البنية مع المميّز الصحيح
                        //      الحالات بدون بيانات (Unit): ثابت عام يحمل البنية مع المميّز فقط
                        // (EN) === ADT Enum Path ===
                        //      Represented as tagged union: struct { __tag: I64, __f0: PTR, __f1: PTR, ... }
                        //      For each variant: constructor function that creates struct with correct tag
                        //      Unit variants: global constant with tag only
                        // ==============================================================

                        // (AR) الخطوة 1: جمع معلومات الحالات وحساب أكبر عدد حقول
                        // (EN) Step 1: Collect variant info and calculate max field count
                        ADTEnumInfo adtInfo;
                        adtInfo.name = enumDecl->name;
                        adtInfo.structName = enumDecl->name;

                        for (size_t i = 0; i < enumDecl->members.size(); ++i)
                        {
                            const auto &member = enumDecl->members[i];
                            ADTVariantInfo variant;
                            variant.name = member.name;
                            variant.tag = static_cast<int64_t>(i);
                            variant.fields = member.fields;
                            // (AR) ISSUE-076: أنواع الحقول Unknown حتّى تُستنتَج من موقع الإنشاء
                            // (EN) ISSUE-076: field types Unknown until inferred from a construction site
                            variant.fieldTypes.assign(member.fields.size(), SadTypeKind::Unknown);
                            // (AR) [أ-م٤] بذر أنواع الحقول من الأنواع المصرَّحة في التعداد إن وُجدت
                            //      (`عدد(رقم قيمة)` ⇒ Integer، `جمع(عقدة يسار)` ⇒ Pointer). الحقول
                            //      بلا نوع مصرَّح تبقى Unknown (توافق ISSUE-076 التامّ). يمنح هذا
                            //      الاستخراجَ نوعَه الصحيح حتّى إن بُنيَت المطابقة قبل أوّل بناء
                            //      (حاسمٌ للتعداد ذاتيّ المرجع في الاستضافة الذاتيّة).
                            // (EN) [A-M4] Seed field types from the declared enum types when present
                            //      (`Num(int value)` ⇒ Integer, `Add(Node left)` ⇒ Pointer). Untyped
                            //      fields stay Unknown (full ISSUE-076 compatibility). Gives extraction
                            //      the right type even if the match is built before the first
                            //      construction (crucial for the self-referential enum in self-hosting).
                            for (size_t fi = 0; fi < member.fields.size() &&
                                                fi < member.fieldTypes.size();
                                 ++fi)
                            {
                                SadTypeKind declaredKind = adtDeclaredTypeNameToKind(member.fieldTypes[fi]);
                                if (declaredKind != SadTypeKind::Unknown)
                                    variant.fieldTypes[fi] = declaredKind;
                            }

                            if (variant.fields.size() > adtInfo.maxFieldCount)
                            {
                                adtInfo.maxFieldCount = variant.fields.size();
                            }

                            adtInfo.variants.push_back(std::move(variant));
                        }

#ifndef NDEBUG
                        std::cout << "[DEBUG] ADT enum '" << enumDecl->name
                                  << "': " << adtInfo.variants.size() << " variants, maxFields="
                                  << adtInfo.maxFieldCount << std::endl;
#endif

                        // (AR) الخطوة 2: إنشاء بنية SIR للـ tagged union
                        //      البنية: { __tag: I64, __f0: Any(%SadDyn), __f1: Any, ..., __fN: Any }
                        //      حيث N = maxFieldCount - 1
                        //      (AR) ISSUE-076 (%SadDyn): حقول الحمولة **ديناميّة النوع جوهريًّا**
                        //      (الحقل بلا تعليق نوع يقبل صحيحًا/عشريًّا/نصًّا/منطقيًّا)، فنُسجّلها Any
                        //      لتُخفَض خانتُها إلى النوع الواصف لذاته %SadDyn بدل وسم البتّات القديم.
                        // (EN) Step 2: Create SIR struct for tagged union
                        //      Struct: { __tag: I64, __f0: Any(%SadDyn), __f1: Any, ..., __fN: Any }
                        //      where N = maxFieldCount - 1
                        //      ISSUE-076 (%SadDyn): payload fields are **inherently dynamically typed**
                        //      (an untyped field accepts int/float/string/bool), so we register them as
                        //      Any so their slot lowers to the self-describing %SadDyn type instead of
                        //      the old bit-tagging scheme.
                        auto sirClass = std::make_shared<SIRClass>(adtInfo.structName, "");
                        sirClass->addField("__tag", SadTypeKind::Integer);
                        for (size_t f = 0; f < adtInfo.maxFieldCount; ++f)
                        {
                            sirClass->addField("__f" + std::to_string(f), SadTypeKind::Any);
                        }

                        if (b_.module_)
                        {
                            b_.module_->addClass(sirClass);
                        }
                        b_.classTable_[adtInfo.structName] = sirClass;

                        // (AR) الخطوة 3: لكل حالة، إنشاء دالة باني أو ثابت عام
                        // (EN) Step 3: For each variant, create constructor function or global constant
                        for (const auto &variant : adtInfo.variants)
                        {
                            std::string fullName = enumDecl->name + "." + variant.name;

                            if (variant.isUnit())
                            {
                                // (AR) حالة بدون بيانات (Unit variant): ثابت i64 يحمل المميّز
                                //      هذا يحافظ على التوافق مع التعدادات البسيطة
                                //      ويُسجَّل أيضاً كباني بدون معاملات لدعم match
                                // (EN) Unit variant: i64 constant holding the discriminant
                                //      This maintains compatibility with simple enums
                                //      Also registered as zero-arg constructor for match support

                                if (b_.module_)
                                {
                                    auto global = std::make_shared<SIRGlobalVariable>(
                                        fullName, SadTypeKind::Integer, std::to_string(variant.tag), true);
                                    b_.module_->addGlobalVariable(global);
                                }

                                VariableInfo enumVar;
                                enumVar.name = fullName;
                                enumVar.type = SadTypeKind::Integer;
                                enumVar.registerName = "%" + fullName;
                                enumVar.isGlobal = true;
                                enumVar.isMutable = false;
                                enumVar.scopeLevel = 0;
                                b_.addVariable(enumVar);

                                // (AR) أيضاً: تسجيل دالة باني بدون معاملات
                                //      حتى يمكن استدعاء شكل.نقطة() كدالة أو استخدامها في match
                                // (EN) Also: register zero-arg constructor function
                                //      So Shape.Point() works as function and in match patterns
                                std::string ctorName = Sad::Compiler::kAdtCtorPrefix + enumDecl->name + "_" + variant.name;

                                auto ctorFunc = std::make_shared<SIRFunction>(ctorName, SadTypeKind::Struct);
                                auto entryBlock = b_.createBasicBlock(ctorName + ".entry");
                                ctorFunc->addBasicBlock(entryBlock);

                                // (AR) جسم الباني: ENUM_CONSTRUCT بدون حقول
                                // (EN) Constructor body: ENUM_CONSTRUCT with no fields
                                std::string resultReg = "%" + ctorName + ".result";
                                SIRInstruction constructInst(SIROpcode::ENUM_CONSTRUCT);
                                constructInst.result = SIROperand::Register(resultReg, SadTypeKind::Struct);
                                constructInst.operands.push_back(SIROperand::ConstantString(enumDecl->name));
                                constructInst.operands.push_back(SIROperand::ConstantI64(variant.tag));
                                constructInst.comment = "Unit variant: " + fullName;
                                entryBlock->addInstruction(constructInst);

                                // (AR) إرجاع البنية
                                // (EN) Return the struct
                                SIRInstruction retInst(SIROpcode::RET);
                                retInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Struct));
                                entryBlock->addInstruction(retInst);

                                if (b_.module_)
                                {
                                    b_.module_->addFunction(ctorFunc);
                                }

                                // (AR) تسجيل الباني في جدول الدوال
                                // (EN) Register constructor in function table
                                FunctionInfo ctorInfo;
                                ctorInfo.name = ctorName;
                                ctorInfo.returnType = SadTypeKind::Struct;
                                ctorInfo.returnClassName = adtInfo.structName;
                                ctorInfo.sirFunction = ctorFunc;
                                b_.functionTable_[fullName] = ctorInfo;
                            }
                            else
                            {
                                // (AR) حالة مع بيانات (Tuple variant): دالة باني تأخذ الحقول كمعاملات
                                //      مثال: شكل.دائرة(نصف_القطر) → __adt_ctor_شكل_دائرة(نصف_القطر)
                                //      الدالة تنشئ بنية { __tag=0, __f0=نصف_القطر }
                                // (EN) Data variant: constructor function that takes fields as parameters
                                //      Example: Shape.Circle(radius) → __adt_ctor_Shape_Circle(radius)
                                //      Function creates struct { __tag=0, __f0=radius }

                                std::string ctorName = Sad::Compiler::kAdtCtorPrefix + enumDecl->name + "_" + variant.name;

                                // (AR) إنشاء دالة SIR مع المعاملات
                                // (EN) Create SIR function with parameters
                                auto ctorFunc = std::make_shared<SIRFunction>(ctorName, SadTypeKind::Struct);
                                for (size_t fi = 0; fi < variant.fields.size(); ++fi)
                                {
                                    // (AR) ISSUE-076 (%SadDyn): معامل الباني ديناميّ ⇒ Any (%SadDyn)،
                                    //      فيغلّف حدُّ الاستدعاء الوسيطَ (double/int/…) إلى %SadDyn.
                                    // (EN) ISSUE-076 (%SadDyn): dynamic ctor param ⇒ Any (%SadDyn); the
                                    //      call boundary packs the argument (double/int/…) into %SadDyn.
                                    SIRParameter param(variant.fields[fi], SadTypeKind::Any);
                                    ctorFunc->addParameter(param);
                                }

                                auto entryBlock = b_.createBasicBlock(ctorName + ".entry");
                                ctorFunc->addBasicBlock(entryBlock);

                                // (AR) جسم الباني: ENUM_CONSTRUCT مع الحقول
                                // (EN) Constructor body: ENUM_CONSTRUCT with fields
                                std::string resultReg = "%" + ctorName + ".result";
                                SIRInstruction constructInst(SIROpcode::ENUM_CONSTRUCT);
                                constructInst.result = SIROperand::Register(resultReg, SadTypeKind::Struct);
                                constructInst.operands.push_back(SIROperand::ConstantString(enumDecl->name));
                                constructInst.operands.push_back(SIROperand::ConstantI64(variant.tag));

                                // (AR) إضافة معاملات الباني كمعاملات للتعليمة
                                // (EN) Add constructor parameters as instruction operands
                                for (size_t fi = 0; fi < variant.fields.size(); ++fi)
                                {
                                    std::string paramReg = "%" + variant.fields[fi];
                                    // (AR) ISSUE-076 (%SadDyn): معامل الحمولة ديناميّ (Any) ⇒ يُخزَّن %SadDyn
                                    // (EN) ISSUE-076 (%SadDyn): dynamic payload operand (Any) ⇒ stored as %SadDyn
                                    constructInst.operands.push_back(
                                        SIROperand::Register(paramReg, SadTypeKind::Any));
                                }
                                constructInst.comment = "Tuple variant: " + fullName + "(" +
                                                        std::to_string(variant.fields.size()) + " fields)";
                                entryBlock->addInstruction(constructInst);

                                // (AR) إرجاع البنية
                                // (EN) Return the struct
                                SIRInstruction retInst(SIROpcode::RET);
                                retInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Struct));
                                entryBlock->addInstruction(retInst);

                                if (b_.module_)
                                {
                                    b_.module_->addFunction(ctorFunc);
                                }

                                // (AR) تسجيل الباني في جدول الدوال
                                // (EN) Register constructor in function table
                                FunctionInfo ctorInfo;
                                ctorInfo.name = ctorName;
                                ctorInfo.returnType = SadTypeKind::Struct;
                                ctorInfo.returnClassName = adtInfo.structName;
                                ctorInfo.sirFunction = ctorFunc;
                                for (size_t fi = 0; fi < variant.fields.size(); ++fi)
                                {
                                    // (AR) ISSUE-076 (%SadDyn): معامل الباني ديناميّ / (EN) dynamic ctor param
                                    ctorInfo.parameters.push_back(SIRParameter(variant.fields[fi], SadTypeKind::Any));
                                }
                                b_.functionTable_[fullName] = ctorInfo;
                            }
                        }

                        // (AR) الخطوة 4: تسجيل معلومات ADT في الجدول العام
                        // (EN) Step 4: Register ADT info in global table
                        b_.adtEnumTable_[enumDecl->name] = std::move(adtInfo);

#ifndef NDEBUG
                        std::cout << "[DEBUG] ADT enum '" << enumDecl->name
                                  << "' fully registered with " << enumDecl->members.size()
                                  << " variant constructors" << std::endl;
#endif
                    }
                    else
                    {
                        // ==============================================================
                        // (AR) === مسار التعداد البسيط (C-style) ===
                        //      كل عضو → ثابت i64 عام
                        //      تعداد اللون { أحمر، أخضر، أزرق }
                        //      → اللون.أحمر = 0, اللون.أخضر = 1, اللون.أزرق = 2
                        // (EN) === Simple Enum Path (C-style) ===
                        //      Each member → i64 global constant
                        //      enum Color { Red, Green, Blue }
                        //      → Color.Red = 0, Color.Green = 1, Color.Blue = 2
                        // ==============================================================

                        for (size_t i = 0; i < enumDecl->members.size(); ++i)
                        {
                            const auto &member = enumDecl->members[i];
                            std::string fullName = enumDecl->name + "." + member.name;

                            // (AR) تحديد القيمة: إما صريحة أو تلقائية
                            // (EN) Determine value: explicit or automatic
                            int64_t enumValue = static_cast<int64_t>(i);
                            if (member.value)
                            {
                                auto valResult = b_.buildExpression(member.value.get());
                                if (valResult.isConstant)
                                {
                                    // (AR) قيمة نصّيّة صريحة (مثل «أحمر = "أحمر"»): يشرّفها
                                    //   المفسّر كنصّ، فنطابقه — نخزّنها ثابتًا نصّيًّا ونتخطّى
                                    //   مسار الـi64 (تكافؤ المحرّكين). وإلّا فهي عدد.
                                    // (EN) Explicit string value: honor it as a string (parity
                                    //   with the interpreter) — store as a string constant and
                                    //   skip the i64 path. Otherwise treat as an integer.
                                    if (valResult.type == SadTypeKind::String)
                                    {
                                        b_.enumStringConstants_[fullName] = valResult.constantValue;
                                        continue;
                                    }
                                    try
                                    {
                                        enumValue = std::stoll(valResult.constantValue);
                                    }
                                    catch (const std::exception &)
                                    {
                                        // (AR) فشل التحويل: استخدام القيمة الافتراضية
                                        // (EN) Conversion failed: use default value
                                    }
                                }
                            }

                            // (AR) إنشاء متغير عام للثابت
                            // (EN) Create global variable for constant
                            std::string regName = "%" + fullName;

                            if (b_.module_)
                            {
                                auto global = std::make_shared<SIRGlobalVariable>(
                                    fullName, SadTypeKind::Integer, std::to_string(enumValue), true);
                                b_.module_->addGlobalVariable(global);
                            }

                            // (AR) تسجيل كثابت في النطاق
                            // (EN) Register as constant in scope
                            VariableInfo enumVar;
                            enumVar.name = fullName;
                            enumVar.type = SadTypeKind::Integer;
                            enumVar.registerName = regName;
                            enumVar.isGlobal = true;
                            enumVar.isMutable = false;
                            enumVar.scopeLevel = 0;
                            b_.addVariable(enumVar);
                        }
                    }
                    return true;
                }

                // ========================================================================
                // (AR) StructDecl - تصريح بنية: بنية نقطة { عشري س، عشري ي }
                // (EN) Struct declaration
                // ========================================================================
                if (auto structDecl = dynamic_cast<Sad::AST::StructDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found StructDecl: " << structDecl->name << std::endl;
#endif

                    // (AR) إنشاء "صنف" SIR بدون وراثة لتمثيل البنية
                    // (EN) Create SIR "class" without inheritance to represent struct
                    auto sirClass = std::make_shared<SIRClass>(structDecl->name, "");

                    // (AR) RFC #53 F2-ب: نقل سمة @تمثيل_سي — بنيةٌ بتخطيط C-متوافق (بلا ترويسة
                    //      vtable في الحقل 0) لعبور حدود C في الربط الخارجيّ (FFI).
                    // (EN) RFC #53 F2-ب: carry the @تمثيل_سي flag — a C-ABI-layout struct (no
                    //      vtable header at field 0) so it can cross the C boundary in FFI.
                    sirClass->isCRepr = structDecl->isCRepr;

                    // (AR) إضافة الحقول
                    // (EN) Add fields
                    for (const auto &field : structDecl->fields)
                    {
                        SadTypeKind fieldType = SadTypeKind::Integer;
                        switch (field.type)
                        {
                        case Sad::Types::SadTypeKind::Integer:
                            fieldType = SadTypeKind::Integer;
                            break;
                        case Sad::Types::SadTypeKind::Float:
                            fieldType = SadTypeKind::Float;
                            break;
                        case Sad::Types::SadTypeKind::Boolean:
                            fieldType = SadTypeKind::Boolean;
                            break;
                        case Sad::Types::SadTypeKind::String:
                            fieldType = SadTypeKind::String;
                            break;
                        case Sad::Types::SadTypeKind::Array:
                            fieldType = SadTypeKind::Array;
                            break;
                        default:
                            fieldType = SadTypeKind::Integer;
                            break;
                        }
                        sirClass->addField(field.name, fieldType);

                        // (AR) سجّل القيمة الافتراضية للحقل (إن وُجدت) لتُهيَّأ وقت الإنشاء (ISSUE-036)
                        // (EN) Record the field's default value (if any) to init at construction (ISSUE-036)
                        if (field.defaultValue)
                        {
                            b_.structFieldDefaults_[structDecl->name].emplace_back(
                                field.name, field.defaultValue.get());
                        }
                    }

                    // (AR) ISSUE-060: سجّل الصنف **قبل** بناء الباني/الطرق (كما في class_main)
                    //      حتى يجد بناءُ الأعضاء تخطيطَ الحقول عبر module_->getClass(name).
                    //      كان التسجيل بعد الحلقة ⇒ الطرق تُبنى بلا تخطيط حقول ⇒ تقرأ 0.
                    // (EN) ISSUE-060: register the class BEFORE building ctor/methods (like
                    //      class_main) so member building can look up the field layout via
                    //      module_->getClass(name). Registering after the loop made methods
                    //      build without field layout ⇒ read 0.
                    if (b_.module_)
                    {
                        b_.module_->addClass(sirClass);
                    }
                    b_.classTable_[structDecl->name] = sirClass;

                    // (AR) ISSUE-060: البنية تحصل على آليّة أعضاء الصنف الكاملة — الباني عبر
                    //      buildClassConstructor (يربط self/هذا ويسجّل «بنية.باني» فيُستدعى عند
                    //      الإنشاء)، والطرق عبر buildClassMethod (تخطيط الحقول وربط هذا).
                    //      كان buildStatement العامّ يُسقطهما بلا سياق صنف ⇒ يعيدان 0.
                    // (EN) ISSUE-060: give the struct the full class-member machinery — the
                    //      constructor via buildClassConstructor (binds self/this and registers
                    //      "Struct.باني" so it's called on construction), and methods via
                    //      buildClassMethod (field layout + this binding). The generic
                    //      buildStatement dropped both without class context ⇒ they returned 0.
                    {
                        // (AR) ClassDecl خفيف على المكدّس — buildClassConstructor/Method لا يقرآن
                        //      إلّا الاسم وsuperclasses (فارغة للبنية). لا نقل ملكيّة AST.
                        // (EN) Lightweight stack ClassDecl — buildClassConstructor/Method only read
                        //      name + superclasses (empty for a struct). No AST ownership transfer.
                        Sad::AST::ClassDecl tmpClass(structDecl->name, std::string(),
                                                     Sad::AST::StmtList{}, false);
                        auto savedClassName = b_.currentClassName_;
                        b_.currentClassName_ = structDecl->name;
                        for (auto &methodStmt : structDecl->methods)
                        {
                            if (auto *ctorDecl = dynamic_cast<Sad::AST::ConstructorDecl *>(methodStmt.get()))
                                b_.buildClassConstructor(&tmpClass, sirClass, ctorDecl);
                            else if (auto *methodDecl = dynamic_cast<Sad::AST::MethodDecl *>(methodStmt.get()))
                                b_.buildClassMethod(&tmpClass, sirClass, methodDecl);
                            else
                                buildStatement(methodStmt.get());
                        }
                        b_.currentClassName_ = savedClassName;
                    }
                    return true;
                }

                // ========================================================================
                // (AR) NamespaceDecl - فضاء أسماء: فضاء رياضيات ... نهاية_فضاء
                // (EN) Namespace declaration
                // ========================================================================
                if (auto nsDecl = dynamic_cast<Sad::AST::NamespaceDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found NamespaceDecl: " << nsDecl->name << std::endl;
#endif

                    // (AR) بناء أعضاء الفضاء (الدوال، الأصناف، المتغيرات)
                    //      نحفظ السياق قبل بناء الدوال لأن b_.buildFunction يكتب على
                    //      b_.currentFunction_ و b_.currentBlock_ ثم يُصفّرهما
                    // (EN) Build namespace members (functions, classes, variables)
                    //      Save context before building functions because b_.buildFunction
                    //      overwrites b_.currentFunction_ and b_.currentBlock_ then nullifies them
                    b_.enterScope();

                    for (const auto &member : nsDecl->members)
                    {
                        if (!member)
                            continue;

                        auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(member.get());
                        auto varDecl = dynamic_cast<Sad::AST::VarDeclStmt *>(member.get());
                        auto classDecl = dynamic_cast<Sad::AST::ClassDecl *>(member.get());

                        if (funcDecl)
                        {
                            // (AR) بناء الدالة مع بادئة الفضاء — نحفظ السياق ونستعيده
                            // (EN) Build function with namespace prefix — save and restore context
                            std::string originalName = funcDecl->name;
                            funcDecl->name = nsDecl->name + "::" + originalName;

                            auto savedCtxNs = b_.saveContext();
                            b_.buildFunction(funcDecl);
                            b_.restoreContext(std::move(savedCtxNs));

                            // (AR) تسجيل العضو في خريطة الفضاء
                            // (EN) Register member in namespace map
                            SIRBuilderContext::NamespaceMemberInfo info;
                            info.kind = "func";
                            info.sirName = funcDecl->name;
                            b_.namespaceMembers_[nsDecl->name][originalName] = info;

                            funcDecl->name = originalName;
                        }
                        else if (varDecl)
                        {
                            // (AR) بناء المتغير العام مع بادئة الفضاء
                            // (EN) Build global variable with namespace prefix
                            std::string originalName = varDecl->name;
                            varDecl->name = nsDecl->name + "::" + originalName;
                            b_.buildGlobalVariable(varDecl);

                            // (AR) تحديد نوع المتغير من المُهيئ
                            // (EN) Determine variable type from initializer
                            SadTypeKind varType = SadTypeKind::Integer;
                            if (varDecl->initializer)
                            {
                                if (auto *litExpr = dynamic_cast<Sad::AST::LiteralExpr *>(varDecl->initializer.get()))
                                {
                                    auto tokType = litExpr->token.getType();
                                    if (tokType == Sad::Lexer::TokenType::NUMBER_DOUBLE)
                                        varType = SadTypeKind::Float;
                                    else if (tokType == Sad::Lexer::TokenType::STRING_LITERAL)
                                        varType = SadTypeKind::String;
                                    else if (tokType == Sad::Lexer::TokenType::LITERAL_TRUE ||
                                             tokType == Sad::Lexer::TokenType::LITERAL_FALSE)
                                        varType = SadTypeKind::Boolean;
                                }
                            }

                            // (AR) تسجيل العضو في خريطة الفضاء
                            // (EN) Register member in namespace map
                            SIRBuilderContext::NamespaceMemberInfo info;
                            info.kind = "var";
                            info.sirName = varDecl->name;
                            info.type = varType;
                            b_.namespaceMembers_[nsDecl->name][originalName] = info;

                            varDecl->name = originalName;
                        }
                        else if (classDecl)
                        {
                            std::string originalName = classDecl->name;
                            classDecl->name = nsDecl->name + "::" + originalName;

                            auto savedCtxNs = b_.saveContext();
                            b_.buildClass(classDecl);
                            b_.restoreContext(std::move(savedCtxNs));

                            // (AR) تسجيل العضو في خريطة الفضاء
                            // (EN) Register member in namespace map
                            SIRBuilderContext::NamespaceMemberInfo info;
                            info.kind = "class";
                            info.sirName = classDecl->name;
                            b_.namespaceMembers_[nsDecl->name][originalName] = info;

                            classDecl->name = originalName;
                        }
                        else
                        {
                            buildStatement(member.get());
                        }
                    }

                    b_.exitScope();
                    return true;
                }

                // ========================================================================
                // (AR) ClassDeclStmt - تصريح صنف (الصيغة البديلة)
                // (EN) ClassDeclStmt - alternate class declaration syntax
                // ========================================================================
                if (auto classDeclStmt = dynamic_cast<Sad::AST::ClassDeclStmt *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found ClassDeclStmt: " << classDeclStmt->name << std::endl;
#endif

                    // (AR) تحويل ClassDeclStmt إلى SIRClass
                    // (EN) Convert ClassDeclStmt to SIRClass
                    std::string parentClass;
                    if (!classDeclStmt->baseClasses.empty())
                    {
                        parentClass = classDeclStmt->baseClasses[0];
                    }

                    auto sirClass = std::make_shared<SIRClass>(classDeclStmt->name, parentClass);

                    // (AR) معالجة الحقول
                    // (EN) Process fields
                    for (const auto &field : classDeclStmt->fields)
                    {
                        // (AR) استنتاج نوع الحقل من التعليق النوعي (DataType) في FieldDecl
                        //      الحقول بدون تعليق نوع صريح تبقى Integer (الافتراضي)
                        // (EN) Infer field type from DataType annotation in FieldDecl
                        //      Fields without explicit type annotation remain Integer (default)
                        SadTypeKind fieldType = SadTypeKind::Integer;
                        switch (field->type)
                        {
                        case Sad::Types::SadTypeKind::Integer:
                            fieldType = SadTypeKind::Integer;
                            break;
                        case Sad::Types::SadTypeKind::Float:
                            fieldType = SadTypeKind::Float;
                            break;
                        case Sad::Types::SadTypeKind::Boolean:
                            fieldType = SadTypeKind::Boolean;
                            break;
                        case Sad::Types::SadTypeKind::String:
                            fieldType = SadTypeKind::String;
                            break;
                        case Sad::Types::SadTypeKind::Array:
                            fieldType = SadTypeKind::Array;
                            break;
                        default:
                            fieldType = SadTypeKind::Integer;
                            break;
                        }

                        // (AR) الحقول الساكنة: تُنشأ كمتغيرات عامة بدلاً من حقول نسخة
                        // (EN) Static fields: created as global variables instead of instance fields
                        if (field->isStatic)
                        {
                            std::string globalName = classDeclStmt->name + "." + field->name;
                            VariableInfo globalVar;
                            globalVar.name = globalName;
                            globalVar.type = fieldType;
                            globalVar.registerName = "@" + globalName;
                            globalVar.isGlobal = true;
                            b_.addVariable(globalVar);

                            // (AR) تعيين القيمة الابتدائية
                            // (EN) Set initial value
                            if (b_.module_)
                            {
                                std::string initVal = "0";
                                if (field->initializer)
                                {
                                    auto initResult = b_.buildExpression(field->initializer.get());
                                    if (initResult.isConstant && !initResult.constantValue.empty())
                                    {
                                        initVal = initResult.constantValue;
                                    }
                                }
                                auto globalVar = std::make_shared<SIRGlobalVariable>(
                                    globalName, fieldType, initVal, false);
                                b_.module_->addGlobalVariable(globalVar);
                            }

                            // (AR) تسجيل الحقل الساكن لاكتشافه لاحقاً
                            // (EN) Register static field for later detection
                            b_.staticFields_[globalName] = fieldType;
                            continue;
                        }

                        sirClass->addField(field->name, fieldType);
                    }

                    // (AR) حفظ السياق
                    // (EN) Save context
                    std::string savedClassName = b_.currentClassName_;
                    b_.currentClassName_ = classDeclStmt->name;

                    // (AR) معالجة الباني
                    // (EN) Process constructor
                    if (classDeclStmt->constructor)
                    {
                        auto ctorDecl = classDeclStmt->constructor.get();
                        std::string ctorName = classDeclStmt->name + "::باني";

                        std::vector<SIRParameter> ctorParams;
                        ctorParams.push_back(SIRParameter(kSelfParamName, SadTypeKind::Pointer));
                        if (auto ctor = dynamic_cast<Sad::AST::ConstructorDecl *>(ctorDecl))
                        {
                            for (const auto &p : ctor->parameters)
                            {
                                ctorParams.push_back(SIRParameter(p.name, SadTypeKind::Integer));
                            }
                        }

                        auto ctorFunc = std::make_shared<SIRFunction>(ctorName, SadTypeKind::Void);
                        // (AR) الرايةُ عقدٌ دلاليٌّ — «هذه الدالّةُ بانٍ» — لا اختصارٌ لعلّةٍ
                        //      بعينِها. فترفَعُ عندَ كلِّ موضعٍ يُنشئُ بانيًا، لا عندَ الموضعِ
                        //      الذي انكشفَ منه العطبُ وحدَه، وإلّا صارَ صمتُها ثغرةً تنتظر.
                        // (EN) The flag is a semantic contract — "this function is a constructor"
                        //      — not shorthand for one particular defect. It is raised at every
                        //      site that creates a constructor, not only the site the defect
                        //      surfaced from; otherwise its silence is a gap waiting to open.
                        ctorFunc->isConstructor = true;
                        for (const auto &cp : ctorParams)
                            ctorFunc->addParameter(cp);
                        auto savedCtxCtor = b_.saveContext();

                        b_.currentFunction_ = ctorFunc;
                        auto entryBlock = b_.createBasicBlock("ctor_entry");
                        ctorFunc->addBasicBlock(entryBlock);
                        b_.currentBlock_ = entryBlock;

                        b_.enterScope();
                        VariableInfo selfVar;
                        selfVar.name = kSelfParamName;
                        selfVar.type = SadTypeKind::Pointer;
                        selfVar.registerName = kSelfRegisterName;
                        b_.addVariable(selfVar);

                        if (auto ctor = dynamic_cast<Sad::AST::ConstructorDecl *>(ctorDecl))
                        {
                            if (ctor->body)
                                buildStatement(ctor->body.get());
                        }

                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                        }
                        b_.exitScope();

                        if (b_.module_)
                            b_.module_->addFunction(ctorFunc);
                        sirClass->addMethod(ctorFunc);

                        b_.restoreContext(std::move(savedCtxCtor));
                    }

                    // (AR) معالجة الدوال
                    // (EN) Process methods
                    for (const auto &method : classDeclStmt->methods)
                    {
                        if (auto methodDecl = dynamic_cast<Sad::AST::MethodDecl *>(method.get()))
                        {
                            bool isStaticMethod = methodDecl->isStatic;
                            std::string methodName = classDeclStmt->name + "::" + methodDecl->name;

                            std::vector<SIRParameter> methodParams;
                            // (AR) الدوال الساكنة لا تأخذ self
                            // (EN) Static methods don't take self
                            if (!isStaticMethod)
                            {
                                methodParams.push_back(SIRParameter(kSelfParamName, SadTypeKind::Pointer));
                            }
                            for (const auto &p : methodDecl->parameters)
                            {
                                methodParams.push_back(SIRParameter(p.name, SadTypeKind::Integer));
                            }

                            auto methodFunc = std::make_shared<SIRFunction>(methodName, SadTypeKind::Integer);
                            for (const auto &mp : methodParams)
                                methodFunc->addParameter(mp);
                            auto savedCtxMethod = b_.saveContext();

                            b_.currentFunction_ = methodFunc;
                            auto entryBlock = b_.createBasicBlock("method_entry");
                            methodFunc->addBasicBlock(entryBlock);
                            b_.currentBlock_ = entryBlock;

                            b_.enterScope();

                            // (AR) الدوال الساكنة لا تحتاج self في النطاق
                            // (EN) Static methods don't need self in scope
                            if (!isStaticMethod)
                            {
                                VariableInfo selfVar;
                                selfVar.name = kSelfParamName;
                                selfVar.type = SadTypeKind::Pointer;
                                selfVar.registerName = kSelfRegisterName;
                                b_.addVariable(selfVar);
                            }

                            for (const auto &p : methodDecl->parameters)
                            {
                                VariableInfo pVar;
                                pVar.name = p.name;
                                pVar.type = SadTypeKind::Integer;
                                pVar.registerName = "%" + p.name;
                                b_.addVariable(pVar);
                            }

                            if (methodDecl->body)
                                buildStatement(methodDecl->body.get());
                            b_.exitScope();

                            if (b_.module_)
                                b_.module_->addFunction(methodFunc);
                            sirClass->addMethod(methodFunc);

                            // (AR) تسجيل الدالة في b_.functionTable_ بصيغة النقطة (للبحث الذكي)
                            // (EN) Register method in b_.functionTable_ with dot notation (for smart lookup)
                            std::string dotName = classDeclStmt->name + "." + methodDecl->name;
                            FunctionInfo fInfo;
                            fInfo.name = methodName;
                            fInfo.returnType = SadTypeKind::Integer;
                            b_.functionTable_[dotName] = fInfo;

                            // (AR) تسجيل الدالة الساكنة لتمييزها عند الاستدعاء
                            // (EN) Register static method for identification during calls
                            if (isStaticMethod)
                            {
                                b_.staticMethods_.insert(dotName);
                            }

                            b_.restoreContext(std::move(savedCtxMethod));
                        }
                    }

                    b_.currentClassName_ = savedClassName;

                    if (b_.module_)
                        b_.module_->addClass(sirClass);
                    b_.classTable_[classDeclStmt->name] = sirClass;
                    return true;
                }

                // ========================================================================
                // (AR) FunctionDecl داخل جسم دالة (دوال متداخلة)
                // (EN) FunctionDecl inside function body (nested function)
                // ========================================================================
                if (auto funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found nested FunctionDecl: " << funcDecl->name << std::endl;
#endif
                    b_.buildFunction(funcDecl);
                    return true;
                }

                // ========================================================================
                // (AR) ClassDecl داخل جسم (صنف متداخل)
                // (EN) ClassDecl inside body (nested class)
                // ========================================================================
                if (auto classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found nested ClassDecl: " << classDecl->name << std::endl;
#endif
                    b_.buildClass(classDecl);
                    return true;
                }

                // ========================================================================
                // (AR) DestructorDecl - هادم الصنف (declarations.h:DestructorDecl)
                // (EN) Destructor declaration — lowered to a special __destructor function
                // ========================================================================
                if (auto destructorDecl = dynamic_cast<Sad::AST::DestructorDecl *>(stmt))
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] Found DestructorDecl" << std::endl;
#endif

                    // (AR) بناء دالة هادم خاصة: __destructor_<className>
                    // (EN) Build special destructor function: __destructor_<className>
                    std::string dtorName = "__destructor";
                    auto dtorFunc = std::make_shared<SIRFunction>(dtorName, SadTypeKind::Void);

                    // (AR) إضافة معامل this
                    // (EN) Add 'this' parameter
                    dtorFunc->addParameter(SIRParameter("this", SadTypeKind::Pointer));

                    auto entryBlock = b_.createBasicBlock("destructor_entry");
                    dtorFunc->addBasicBlock(entryBlock);

                    auto savedCtxDtor = b_.saveContext();
                    b_.currentFunction_ = dtorFunc;
                    b_.currentBlock_ = entryBlock;

                    b_.enterScope();
                    if (destructorDecl->body)
                    {
                        buildStatement(destructorDecl->body.get());
                    }
                    b_.exitScope();

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->addInstruction(SIRInstruction::ReturnVoid());
                    }

                    b_.restoreContext(std::move(savedCtxDtor));
                    b_.module_->addFunction(dtorFunc);
                    return true;
                }

                return false;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad