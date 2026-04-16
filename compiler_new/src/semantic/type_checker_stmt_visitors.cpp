// ============================================================================
// type_checker_stmt_visitors.cpp - زوار الجمل والتعريفات
// ============================================================================
// (AR) زوار الجمل: ExprStmt, VarDeclStmt, IfStmt, WhileStmt, ForStmt...
//      زوار التعريفات: ClassDecl, FunctionDecl, EnumDecl, ImportStmt...
// (EN) Statement visitors: ExprStmt, VarDeclStmt, IfStmt, WhileStmt, ForStmt...
//      Declaration visitors: ClassDecl, FunctionDecl, EnumDecl, ImportStmt...
// تم استخراج هذا الملف من type_checker.cpp وفقاً لقاعدة CW-05
// ============================================================================
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

#include "semantic/type_checker.h"
#include "token.h"
#include "class_nodes.h"
#include "types/composite_type_classes.h"
#include "types/enum_types.h"
#include "types/struct_types.h"

#include <iostream>
#include <algorithm>
#include <cassert>

namespace Sad
{
    namespace Semantic
    {

        using namespace TypeSystem;
        using TT = Lexer::TokenType;

        void TypeChecker::visitExprStmt(AST::ExprStmt &stmt)
        {
            if (stmt.expression)
            {
                inferExprType(stmt.expression.get());
            }
        }

        void TypeChecker::visitVarDeclStmt(AST::VarDeclStmt &stmt)
        {
            currentResult_.totalVariables++;

            // النوع المُصرّح / Declared type
            TypePtr declaredType = dataTypeToTypePtr(stmt.type);

            // النوع المُستنتج من القيمة / Inferred type from initializer
            TypePtr initType = nullptr;
            if (stmt.initializer)
            {
                initType = inferExprType(stmt.initializer.get());
            }

            // تسجيل نوع المتغير
            TypePtr finalType = declaredType;

            // إذا كان النوع المُصرّح unknown، استخدم المُستنتج
            if (declaredType && declaredType->isUnknown() && initType && !initType->isUnknown())
            {
                finalType = initType;
                currentResult_.totalInferred++;
            }

            // التحقق من التوافق / Check compatibility
            if (initType && declaredType && !declaredType->isUnknown() && !initType->isUnknown())
            {
                if (!areTypesCompatible(declaredType, initType))
                {
                    recordTypeError(stmt.name,
                                    declaredType->toString(), initType->toString(),
                                    &stmt);
                }
            }

            // تسجيل في البيئة / Register in environment
            declareVariable(stmt.name, finalType);

            if (debugMode_)
            {
                std::cerr << "  [TC] VarDecl '" << stmt.name << "': "
                          << (finalType ? finalType->toString() : "unknown")
                          << (stmt.isConst ? " (const)" : "") << "\n";
            }
        }

        void TypeChecker::visitIfStmt(AST::IfStmt &stmt)
        {
            // تحقق من نوع الشرط / Check condition type
            if (stmt.condition)
            {
                TypePtr condType = inferExprType(stmt.condition.get());
                if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown())
                {
                    recordTypeError("", "boolean", condType->toString(), &stmt,
                                    "If condition should be boolean");
                }
            }

            // فحص الفرعين / Check both branches
            if (stmt.thenBranch)
            {
                enterScope();
                stmt.thenBranch->accept(*this);
                exitScope();
            }
            if (stmt.elseBranch)
            {
                enterScope();
                stmt.elseBranch->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitWhileStmt(AST::WhileStmt &stmt)
        {
            if (stmt.condition)
            {
                TypePtr condType = inferExprType(stmt.condition.get());
                if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown())
                {
                    recordTypeError("", "boolean", condType->toString(), &stmt,
                                    "While condition should be boolean");
                }
            }

            if (stmt.body)
            {
                enterScope();
                stmt.body->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitForStmt(AST::ForStmt &stmt)
        {
            enterScope();
            if (stmt.initializer)
                stmt.initializer->accept(*this);
            if (stmt.condition)
                inferExprType(stmt.condition.get());
            if (stmt.increment)
                inferExprType(stmt.increment.get());
            if (stmt.body)
                stmt.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitForRangeStmt(AST::ForRangeStmt &stmt)
        {
            enterScope();

            if (stmt.iterable)
            {
                inferExprType(stmt.iterable.get());
            }

            // تسجيل متغير الحلقة / Register loop variable
            declareVariable(stmt.variable, registry_.getUnknownType());
            if (!stmt.valueVar.empty())
            {
                declareVariable(stmt.valueVar, registry_.getUnknownType());
            }

            if (stmt.body)
                stmt.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitSwitchStmt(AST::SwitchStmt &stmt)
        {
            TypePtr switchType = nullptr;
            if (stmt.expression)
            {
                switchType = inferExprType(stmt.expression.get());
            }

            for (auto &caseBranch : stmt.cases)
            {
                if (caseBranch.value)
                {
                    TypePtr caseType = inferExprType(caseBranch.value.get());
                    if (switchType && caseType && !areTypesCompatible(switchType, caseType))
                    {
                        recordTypeError("", switchType->toString(), caseType->toString(),
                                        caseBranch.value.get(),
                                        "Switch case type mismatch");
                    }
                }
                if (caseBranch.body)
                {
                    enterScope();
                    caseBranch.body->accept(*this);
                    exitScope();
                }
            }

            if (stmt.defaultCase)
            {
                enterScope();
                stmt.defaultCase->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitReturnStmt(AST::ReturnStmt &stmt)
        {
            TypePtr retType = registry_.getVoidType();
            if (stmt.value)
            {
                retType = inferExprType(stmt.value.get());
            }

            // تحقق من توافق نوع الإرجاع / Check return type compatibility
            if (expectedReturnType_ && retType)
            {
                if (!areTypesCompatible(expectedReturnType_, retType))
                {
                    recordTypeError(currentFunction_,
                                    expectedReturnType_->toString(), retType->toString(),
                                    &stmt,
                                    "Return type mismatch in function '" + currentFunction_ + "'");
                }
            }
        }

        void TypeChecker::visitYieldStmt(AST::YieldStmt &stmt)
        {
            if (stmt.value)
            {
                inferExprType(stmt.value.get());
            }
        }

        void TypeChecker::visitBreakStmt(AST::BreakStmt & /*stmt*/)
        {
            // لا فحص أنواع / No type checking needed
        }

        void TypeChecker::visitContinueStmt(AST::ContinueStmt & /*stmt*/)
        {
            // لا فحص أنواع / No type checking needed
        }

        void TypeChecker::visitBlockStmt(AST::BlockStmt &stmt)
        {
            enterScope();
            for (auto &s : stmt.statements)
            {
                if (s)
                    s->accept(*this);
            }
            exitScope();
        }

        void TypeChecker::visitTryStmt(AST::TryStmt &stmt)
        {
            if (stmt.tryBlock)
            {
                enterScope();
                stmt.tryBlock->accept(*this);
                exitScope();
            }

            for (auto &clause : stmt.catchClauses)
            {
                enterScope();
                if (!clause.exceptionVar.empty())
                {
                    declareVariable(clause.exceptionVar, dataTypeToTypePtr(clause.exceptionType));
                }
                if (clause.body)
                    clause.body->accept(*this);
                exitScope();
            }

            if (stmt.finallyBlock)
            {
                enterScope();
                stmt.finallyBlock->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitRaiseStmt(AST::RaiseStmt &stmt)
        {
            if (stmt.exception)
            {
                inferExprType(stmt.exception.get());
            }
        }

        void TypeChecker::visitWithStmt(AST::WithStmt &stmt)
        {
            enterScope();
            if (stmt.resource)
            {
                TypePtr resType = inferExprType(stmt.resource.get());
                if (!stmt.alias.empty())
                {
                    declareVariable(stmt.alias, resType);
                }
            }
            if (stmt.body)
                stmt.body->accept(*this);
            exitScope();
        }

        // ============================================================================
        // (AR) جمل التزامن — أطلق / اختر / (EN) Concurrency statements — go / select
        // ============================================================================

        void TypeChecker::visitGoStmt(AST::GoStmt &stmt)
        {
            // (AR) فحص التعبير المُطلَق (استدعاء دالة أو لامدا)
            // (EN) Check the launched expression (function call or lambda)
            if (stmt.expression)
            {
                inferExprType(stmt.expression.get());
            }
            // (AR) فحص كتلة الكود إذا كان "أطلق ... نهاية"
            // (EN) Check block body if "go ... end"
            if (stmt.blockBody)
            {
                enterScope();
                stmt.blockBody->accept(*this);
                exitScope();
            }
        }

        void TypeChecker::visitSelectCase(AST::SelectCase &stmt)
        {
            // (AR) فحص تعبير القناة
            // (EN) Check channel expression
            if (stmt.channelExpr)
            {
                inferExprType(stmt.channelExpr.get());
            }
            // (AR) فحص جسم الحالة
            // (EN) Check case body
            enterScope();
            if (!stmt.bindVar.empty())
            {
                declareVariable(stmt.bindVar, registry_.getUnknownType());
            }
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
            exitScope();
        }

        void TypeChecker::visitSelectStmt(AST::SelectStmt &stmt)
        {
            // (AR) فحص كل حالة اختيار
            // (EN) Check each select case
            for (auto &c : stmt.cases)
            {
                if (c)
                    c->accept(*this);
            }
            // (AR) فحص الجسم الافتراضي
            // (EN) Check default body
            if (!stmt.defaultBody.empty())
            {
                enterScope();
                for (auto &s : stmt.defaultBody)
                {
                    if (s)
                        s->accept(*this);
                }
                exitScope();
            }
        }

        void TypeChecker::visitClassDeclStmt(AST::ClassDeclStmt &stmt)
        {
            enterScope();

            // تسجيل الحقول / Register fields
            for (auto &field : stmt.fields)
            {
                if (field)
                    field->accept(*this);
            }

            // فحص المُنشئ / Check constructor
            if (stmt.constructor)
            {
                stmt.constructor->accept(*this);
            }

            // فحص المُدمّر / Check destructor
            if (stmt.destructor)
            {
                stmt.destructor->accept(*this);
            }

            // فحص الدوال / Check methods
            for (auto &method : stmt.methods)
            {
                if (method)
                    method->accept(*this);
            }

            exitScope();
        }

        // ============================================================================
        // زيارة التصريحات / Visit Declarations
        // ============================================================================

        void TypeChecker::visitFunctionDecl(AST::FunctionDecl &decl)
        {
            currentResult_.totalFunctions++;

            std::string prevFunction = currentFunction_;
            TypePtr prevReturnType = expectedReturnType_;

            currentFunction_ = decl.name;
            // (AR) إذا كان نوع الإرجاع غير معروف أو NONE
            // (EN) If return type is UNKNOWN or NONE
            if (decl.returnType == Data::DataType::UNKNOWN ||
                decl.returnType == Data::DataType::NONE)
            {
                if (decl.isExtern)
                {
                    // (AR) الدوال الخارجية بدون نوع إرجاع تُفترض رقم (I64)
                    // (EN) Extern functions without return type default to integer
                    expectedReturnType_ = registry_.getIntegerType();
                }
                else
                {
                    expectedReturnType_ = nullptr;
                }
            }
            else
            {
                expectedReturnType_ = dataTypeToTypePtr(decl.returnType);
            }

            // (AR) تسجيل الدالة بنوع دالة كامل (معاملات + إرجاع)
            TypeList paramTypes;
            for (auto &param : decl.parameters)
            {
                paramTypes.push_back(dataTypeToTypePtr(param.type));
            }
            auto fnType = std::make_shared<FunctionType>(std::move(paramTypes), expectedReturnType_);
            declareVariable(decl.name, fnType);

            enterScope();

            // تسجيل المعاملات / Register parameters
            for (auto &param : decl.parameters)
            {
                TypePtr paramType = dataTypeToTypePtr(param.type);
                declareVariable(param.name, paramType);
            }

            // فحص جسم الدالة / Check function body
            if (decl.body)
            {
                decl.body->accept(*this);
            }

            exitScope();

            currentFunction_ = prevFunction;
            expectedReturnType_ = prevReturnType;

            if (debugMode_)
            {
                std::cerr << "  [TC] Function '" << decl.name << "' -> "
                          << (expectedReturnType_ ? expectedReturnType_->toString() : "void") << "\n";
            }
        }

        void TypeChecker::visitClassDecl(AST::ClassDecl &decl)
        {
            enterScope();

            // (AR) التحقق من وجود الصنف في StructRegistry
            bool isKnown = StructRegistry::instance().isStructType(decl.name);
            declareVariable(decl.name, isKnown
                                           ? registry_.internPrimitiveType(SadTypeKind::Class)
                                           : registry_.getUnknownType());

            // فحص الأعضاء / Check members
            for (auto &member : decl.members)
            {
                if (member)
                    member->accept(*this);
            }

            exitScope();
        }

        void TypeChecker::visitFieldDecl(AST::FieldDecl &decl)
        {
            TypePtr fieldType = dataTypeToTypePtr(decl.type);
            declareVariable(decl.name, fieldType);

            if (decl.initializer)
            {
                TypePtr initType = inferExprType(decl.initializer.get());
                if (initType && fieldType && !areTypesCompatible(fieldType, initType))
                {
                    recordTypeError(decl.name,
                                    fieldType->toString(), initType->toString(),
                                    &decl, "Field initializer type mismatch");
                }
            }
        }

        void TypeChecker::visitMethodDecl(AST::MethodDecl &decl)
        {
            currentResult_.totalFunctions++;

            std::string prevFunction = currentFunction_;
            TypePtr prevReturnType = expectedReturnType_;

            currentFunction_ = decl.name;
            // (AR) إذا كان نوع الإرجاع غير معروف أو NONE، لا نفحص نوع return
            // (EN) If return type is UNKNOWN or NONE, skip return type checking
            if (decl.returnType == Data::DataType::UNKNOWN ||
                decl.returnType == Data::DataType::NONE)
            {
                expectedReturnType_ = nullptr;
            }
            else
            {
                expectedReturnType_ = dataTypeToTypePtr(decl.returnType);
            }

            enterScope();

            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }

            if (decl.body)
                decl.body->accept(*this);

            exitScope();

            currentFunction_ = prevFunction;
            expectedReturnType_ = prevReturnType;
        }

        void TypeChecker::visitPropertyDecl(AST::PropertyDecl &decl)
        {
            // PropertyDecl may not have standard fields we can check
            // Just visit children
            (void)decl;
        }

        void TypeChecker::visitConstructorDecl(AST::ConstructorDecl &decl)
        {
            currentResult_.totalFunctions++;

            enterScope();
            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }
            if (decl.body)
                decl.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitDestructorDecl(AST::DestructorDecl &decl)
        {
            enterScope();
            if (decl.body)
                decl.body->accept(*this);
            exitScope();
        }

        void TypeChecker::visitEnumDecl(AST::EnumDecl &decl)
        {
            // (AR) التحقق من وجود التعداد في EnumRegistry
            bool isKnown = EnumRegistry::instance().isEnumType(decl.name);
            declareVariable(decl.name, isKnown
                                           ? registry_.internPrimitiveType(SadTypeKind::Class) // استخدام Class كبديل لحين يتوفر EnumKind
                                           : registry_.getUnknownType());

            for (auto &member : decl.members)
            {
                if (member.value)
                {
                    inferExprType(member.value.get());
                }
            }
        }

        void TypeChecker::visitImportStmt(AST::ImportStmt &stmt)
        {
            // (AR) تحميل أنواع الوحدة — تسجيل اسم الوحدة كمتغير من نوع Module
            // (EN) Module type loading — register module name as Module-typed variable
            if (!stmt.modulePath.empty())
            {
                std::string effectiveName = stmt.getEffectiveName();

                // (AR) تسجيل الوحدة كنطاق اسم — كل أعضائها Any حتى يتم ربط الوحدات
                // (EN) Register module as namespace — members are Any until module linking
                declareVariable(effectiveName, registry_.getAnyType());

                // (AR) تسجيل مسار الوحدة لمرحلة الربط في SIR
                // (EN) Record module path for SIR linking phase
                currentResult_.moduleDependencies.push_back(stmt.getFullModuleName());
            }
        }

        void TypeChecker::visitFromImportStmt(AST::FromImportStmt &stmt)
        {
            // (AR) تسجيل كل رمز مستورد مع نوعه المبدئي
            // (EN) Register each imported symbol with its initial type

            // (AR) تسجيل اعتماد الوحدة
            currentResult_.moduleDependencies.push_back(stmt.getFullModuleName());

            for (const auto &item : stmt.items)
            {
                std::string name = item.getEffectiveName();
                // (AR) كل رمز مستورد يبدأ كـ Any — سيُحدَّث عند ربط الوحدات
                declareVariable(name, registry_.getAnyType());
            }
        }

        void TypeChecker::visitExportStmt(AST::ExportStmt &stmt)
        {
            if (stmt.declaration)
                stmt.declaration->accept(*this);
        }

        void TypeChecker::visitExportDecl(AST::ExportDecl &decl)
        {
            (void)decl;
        }

        void TypeChecker::visitReExportStmt(AST::ReExportStmt &stmt)
        {
            (void)stmt;
        }

        // ============================================================================
        // المعممات / Templates & Generics
        // ============================================================================

        void TypeChecker::visitTemplateFunctionDecl(AST::TemplateFunctionDecl &decl)
        {
            currentResult_.totalFunctions++;

            enterScope();

            // (AR) تسجيل معاملات النوع كمتغيرات نوع معممة
            for (auto &tp : decl.typeParameters)
            {
                declareVariable(tp.name, registry_.getUnknownType());
            }

            // تسجيل معاملات الدالة / Register function parameters
            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }

            if (decl.body)
                decl.body->accept(*this);

            exitScope();

            // تسجيل الدالة المعممة / Register generic function
            declareVariable(decl.name, dataTypeToTypePtr(decl.returnType));

            if (debugMode_)
            {
                std::cerr << "  [TC] TemplateFunction '" << decl.name << "' with "
                          << decl.typeParameters.size() << " type params\n";
            }
        }

        void TypeChecker::visitTemplateClassDecl(AST::TemplateClassDecl &decl)
        {
            enterScope();

            for (auto &tp : decl.typeParameters)
            {
                declareVariable(tp.name, registry_.getUnknownType());
            }

            for (auto &member : decl.members)
            {
                if (member)
                    member->accept(*this);
            }

            exitScope();

            // (AR) تسجيل الصنف المعمم — البحث في StructRegistry
            bool isKnown = StructRegistry::instance().isStructType(decl.name);
            declareVariable(decl.name, isKnown
                                           ? registry_.internPrimitiveType(SadTypeKind::Class)
                                           : registry_.getUnknownType());
        }

        void TypeChecker::visitTemplateInstantiation(AST::TemplateInstantiation &inst)
        {
            currentResult_.totalExpressions++;

            // (AR) البحث عن القالب والتحقق من عدد معاملات الأنواع
            // (EN) Look up template and validate type argument count
            auto type = lookupVariable(inst.templateName);

            if (!type)
            {
                // (AR) قالب غير معرّف
                // (EN) Template not defined
                TypeCheckError err;
                err.variableName = inst.templateName;
                err.line = inst.position.line;
                err.column = inst.position.column;
                err.message = "Undefined template: '" + inst.templateName + "'";
                err.arabicMessage = "قالب غير معرّف: '" + inst.templateName + "'";
                currentResult_.addError(err);
                lastInferredType_ = registry_.getUnknownType();
            }
            else
            {
                // (AR) التحقق من أنه ليس فارغًا من معاملات الأنواع
                // (EN) Ensure type arguments are provided
                if (inst.typeArguments.empty())
                {
                    TypeCheckError err;
                    err.variableName = inst.templateName;
                    err.line = inst.position.line;
                    err.column = inst.position.column;
                    err.message = "Empty type arguments for template: '" + inst.templateName + "'";
                    err.arabicMessage = "معاملات أنواع فارغة للقالب: '" + inst.templateName + "'";
                    currentResult_.addError(err);
                }
                lastInferredType_ = type;
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] TemplateInstantiation '" << inst.templateName
                          << "' with " << inst.typeArguments.size() << " type args\n";
            }
        }

        void TypeChecker::visitNamespaceDecl(AST::NamespaceDecl &decl)
        {
            enterScope();
            for (auto &member : decl.members)
            {
                if (member)
                    member->accept(*this);
            }
            exitScope();
        }

        void TypeChecker::visitOperatorDecl(AST::OperatorDecl &decl)
        {
            currentResult_.totalFunctions++;

            enterScope();
            for (auto &param : decl.parameters)
            {
                declareVariable(param.name, dataTypeToTypePtr(param.type));
            }
            if (decl.body)
                decl.body->accept(*this);
            exitScope();
        }


    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
