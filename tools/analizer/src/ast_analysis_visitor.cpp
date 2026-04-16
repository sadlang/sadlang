#include "ast_analysis_visitor.h"

#include <algorithm>
#include <utility>

namespace Sad::Analizer
{

    namespace
    {

        std::string kindToArabic(SymbolKind kind)
        {
            switch (kind)
            {
            case SymbolKind::Variable:
                return "متغير";
            case SymbolKind::Constant:
                return "ثابت";
            case SymbolKind::Parameter:
                return "معامل";
            case SymbolKind::Function:
                return "دالة";
            case SymbolKind::Class:
                return "صنف";
            case SymbolKind::Method:
                return "طريقة";
            case SymbolKind::Field:
                return "حقل";
            case SymbolKind::Property:
                return "خاصية";
            case SymbolKind::ImportAlias:
                return "رمز مستورد";
            case SymbolKind::EnumName:
                return "تعداد";
            }

            return "رمز";
        }

        std::string issueOwnerLabel(const std::string &ownerLabel)
        {
            return ownerLabel.empty() ? "هذا الموضع" : ownerLabel;
        }

    } // namespace

    AstAnalysisVisitor::AstAnalysisVisitor(std::string filePath, AnalyzerOptions options)
        : filePath_(std::move(filePath)), options_(options)
    {
        result_.filePath = filePath_;
        result_.parsedSuccessfully = true;
        enterScope(ScopeKind::Global, "النطاق العام");
        initializeBuiltins();
    }

    void AstAnalysisVisitor::initializeBuiltins()
    {
        builtinNames_ = {
            "اطبع", "اطبع_سطر", "print", "println", "طول", "range",
            "جمع", "نوع", "تحويل_إلى_نص", "اقرأ_سطر"};

        for (const std::string &name : builtinNames_)
        {
            SymbolInfo symbol;
            symbol.name = name;
            symbol.kind = SymbolKind::Function;
            symbol.isCallable = true;
            symbol.isBuiltin = true;
            currentScope().symbols.emplace(name, std::move(symbol));
        }
    }

    void AstAnalysisVisitor::predeclareProgram(AST::StmtList &program)
    {
        for (const auto &statement : program)
        {
            predeclareTopLevelStatement(statement.get());
        }
    }

    void AstAnalysisVisitor::analyzeProgram(AST::StmtList &program)
    {
        analyzeStatementList(program);
        leaveScope();
    }

    AnalysisResult AstAnalysisVisitor::takeResult()
    {
        return std::move(result_);
    }

    void AstAnalysisVisitor::enterScope(ScopeKind kind, const std::string &label)
    {
        scopes_.push_back(ScopeFrame{kind, label, {}});
        ++result_.summary.totalScopes;
    }

    void AstAnalysisVisitor::leaveScope()
    {
        if (scopes_.empty())
        {
            return;
        }

        flushUnusedWarnings(scopes_.back());
        scopes_.pop_back();
    }

    ScopeFrame &AstAnalysisVisitor::currentScope()
    {
        return scopes_.back();
    }

    const ScopeFrame &AstAnalysisVisitor::currentScope() const
    {
        return scopes_.back();
    }

    Errors::SourceLocation AstAnalysisVisitor::makeLocation(const Lexer::Position &position) const
    {
        return Errors::SourceLocation(filePath_, position.line, position.column, position.offset, position.length == 0 ? 1 : position.length);
    }

    void AstAnalysisVisitor::addIssue(const std::string &ruleId,
                                      IssueKind kind,
                                      Errors::DiagnosticSeverity severity,
                                      const Errors::SourceLocation &location,
                                      const std::string &arabicMessage,
                                      const std::string &englishMessage,
                                      const std::string &symbolName)
    {
        AnalysisIssue issue;
        issue.ruleId = ruleId;
        issue.kind = kind;
        issue.severity = severity;
        issue.location = location;
        issue.messageArabic = arabicMessage;
        issue.messageEnglish = englishMessage;
        issue.symbolName = symbolName;
        result_.issues.push_back(std::move(issue));

        if (severity == Errors::DiagnosticSeverity::ERROR)
        {
            ++result_.summary.totalErrors;
        }
        else if (severity == Errors::DiagnosticSeverity::WARNING)
        {
            ++result_.summary.totalWarnings;
        }
        else if (severity == Errors::DiagnosticSeverity::HINT)
        {
            ++result_.summary.totalHints;
        }
    }

    void AstAnalysisVisitor::enterNestedControlFlow(const Lexer::Position &position, const std::string &ownerLabel)
    {
        ++currentNestingDepth_;
        result_.summary.maxNestingDepth = std::max(result_.summary.maxNestingDepth, currentNestingDepth_);

        if (options_.enableComplexityWarnings && currentNestingDepth_ > options_.maxNestingDepth)
        {
            addIssue(
                "SAD-CPLX-001",
                IssueKind::Style,
                Errors::DiagnosticSeverity::WARNING,
                makeLocation(position),
                "عمق التداخل في " + ownerLabel + " وصل إلى " + std::to_string(currentNestingDepth_) +
                    " وهو أعلى من الحد الموصى به (" + std::to_string(options_.maxNestingDepth) + ").",
                "Nesting depth in " + ownerLabel + " reached " + std::to_string(currentNestingDepth_) +
                    " which exceeds the recommended threshold (" + std::to_string(options_.maxNestingDepth) + ").");
        }
    }

    void AstAnalysisVisitor::leaveNestedControlFlow()
    {
        if (currentNestingDepth_ > 0)
        {
            --currentNestingDepth_;
        }
    }

    void AstAnalysisVisitor::checkFunctionComplexity(const std::string &functionName,
                                                     const Lexer::Position &position,
                                                     std::size_t statementCount,
                                                     std::size_t parameterCount)
    {
        if (!options_.enableComplexityWarnings)
        {
            return;
        }

        if (statementCount > options_.maxFunctionStatements)
        {
            addIssue(
                "SAD-CPLX-002",
                IssueKind::Style,
                Errors::DiagnosticSeverity::WARNING,
                makeLocation(position),
                "الدالة '" + functionName + "' طويلة جداً (" + std::to_string(statementCount) +
                    " جملة). يفضل تقسيمها إلى دوال أصغر.",
                "Function '" + functionName + "' is too long (" + std::to_string(statementCount) +
                    " statements). Consider splitting it into smaller functions.",
                functionName);
        }

        if (parameterCount > options_.maxFunctionParameters)
        {
            addIssue(
                "SAD-CPLX-003",
                IssueKind::Style,
                Errors::DiagnosticSeverity::WARNING,
                makeLocation(position),
                "الدالة '" + functionName + "' تستقبل معاملات كثيرة (" + std::to_string(parameterCount) +
                    "). يفضل استخدام كائن إعدادات أو بنية بيانات.",
                "Function '" + functionName + "' has too many parameters (" + std::to_string(parameterCount) +
                    "). Consider using an options object or a data structure.",
                functionName);
        }
    }

    void AstAnalysisVisitor::checkClassComplexity(const std::string &className,
                                                  const Lexer::Position &position,
                                                  std::size_t methodCount)
    {
        if (!options_.enableComplexityWarnings)
        {
            return;
        }

        if (methodCount > options_.maxClassMethods)
        {
            addIssue(
                "SAD-CPLX-004",
                IssueKind::Style,
                Errors::DiagnosticSeverity::WARNING,
                makeLocation(position),
                "الصنف '" + className + "' يحتوي على عدد كبير من الطرق (" + std::to_string(methodCount) +
                    "). قد يكون من الأفضل تقسيم المسؤوليات.",
                "Class '" + className + "' has too many methods (" + std::to_string(methodCount) +
                    "). Consider splitting responsibilities.",
                className);
        }
    }

    void AstAnalysisVisitor::checkMagicNumber(const AST::Expression *expression, const Lexer::Position &position)
    {
        if (!options_.enableMagicNumberWarnings || expression == nullptr)
        {
            return;
        }

        const auto *literal = dynamic_cast<const AST::LiteralExpr *>(expression);
        if (literal == nullptr)
        {
            return;
        }

        const std::string value = literal->toString();
        if (value == "0" || value == "1" || value == "-1" || value == "0.0" || value == "1.0")
        {
            return;
        }

        ++result_.summary.totalMagicNumbers;
        addIssue(
            "SAD-SMELL-001",
            IssueKind::Style,
            Errors::DiagnosticSeverity::HINT,
            makeLocation(position),
            "رقم ثابت '" + value + "' مستخدم مباشرة. إذا كان له معنى منطقي فالأفضل تعريفه كثابت باسم واضح.",
            "Magic number '" + value + "' used directly. Consider replacing it with a named constant.");
    }

    void AstAnalysisVisitor::checkRepeatedCallPattern(const AST::CallExpr &expr)
    {
        if (!options_.enablePerformanceWarnings)
        {
            return;
        }

        const auto *variable = dynamic_cast<const AST::VariableExpr *>(expr.callee.get());
        if (variable == nullptr)
        {
            return;
        }

        if ((variable->name == "طول" || variable->name == "length") && currentNestingDepth_ > 0)
        {
            addIssue(
                "SAD-PERF-001",
                IssueKind::Performance,
                Errors::DiagnosticSeverity::HINT,
                makeLocation(expr.position),
                "استدعاء '" + variable->name + "' داخل بنية متداخلة قد يتكرر كثيراً. فكر في حفظ النتيجة بمتحول محلي.",
                "Repeated '" + variable->name + "' call inside nested control flow may be expensive. Cache it in a local variable.");
        }
    }

    bool AstAnalysisVisitor::isNullLiteral(const AST::Expression *expression) const
    {
        const auto *literal = dynamic_cast<const AST::LiteralExpr *>(expression);
        if (literal == nullptr)
        {
            return false;
        }

        const std::string value = literal->toString();
        return value == "لاشيء" || value == "عدم" || value == "null" || value == "none";
    }

    bool AstAnalysisVisitor::isDefinitelyNonNullExpression(const AST::Expression *expression) const
    {
        if (expression == nullptr)
        {
            return false;
        }

        if (isNullLiteral(expression))
        {
            return false;
        }

        if (dynamic_cast<const AST::NewExpr *>(expression) != nullptr ||
            dynamic_cast<const AST::ArrayExpr *>(expression) != nullptr ||
            dynamic_cast<const AST::MapExpr *>(expression) != nullptr ||
            dynamic_cast<const AST::LambdaExpr *>(expression) != nullptr)
        {
            return true;
        }

        if (dynamic_cast<const AST::LiteralExpr *>(expression) != nullptr)
        {
            return true;
        }

        return false;
    }

    void AstAnalysisVisitor::checkPossibleNullDereference(const AST::Expression *target,
                                                          const Lexer::Position &position,
                                                          const std::string &contextLabel)
    {
        if (!options_.enableCodeSmellWarnings || target == nullptr)
        {
            return;
        }

        if (isNullLiteral(target))
        {
            addIssue(
                "SAD-NULL-001",
                IssueKind::Safety,
                Errors::DiagnosticSeverity::ERROR,
                makeLocation(position),
                "محاولة استخدام قيمة null مباشرة في " + contextLabel + ".",
                "Direct null dereference in " + contextLabel + ".");
            return;
        }

        const auto *variable = dynamic_cast<const AST::VariableExpr *>(target);
        if (variable == nullptr)
        {
            return;
        }

        const SymbolInfo *symbol = resolveSymbol(variable->name);
        if (symbol != nullptr && symbol->mayBeNull)
        {
            addIssue(
                "SAD-NULL-002",
                IssueKind::Safety,
                Errors::DiagnosticSeverity::WARNING,
                makeLocation(position),
                "قد تكون القيمة '" + variable->name + "' فارغة (null) عند استخدامها في " + contextLabel + ".",
                "Value '" + variable->name + "' may be null when used in " + contextLabel + ".",
                variable->name);
        }
    }

    bool AstAnalysisVisitor::declareSymbol(const SymbolInfo &symbol, bool warnOnShadowing)
    {
        auto &symbols = currentScope().symbols;
        auto existing = symbols.find(symbol.name);
        if (existing != symbols.end())
        {
            addIssue(
                "SAD-SEM-001",
                IssueKind::Semantic,
                Errors::DiagnosticSeverity::ERROR,
                symbol.location,
                "يوجد تعريف مكرر للاسم '" + symbol.name + "' داخل نفس النطاق.",
                "Duplicate definition for symbol '" + symbol.name + "' in the same scope.",
                symbol.name);
            return false;
        }

        if (warnOnShadowing && options_.enableShadowingWarnings && existsInParentScopes(symbol.name))
        {
            addIssue(
                "SAD-SEM-002",
                IssueKind::Semantic,
                Errors::DiagnosticSeverity::WARNING,
                symbol.location,
                "الاسم '" + symbol.name + "' يحجب تعريفاً أقدم في نطاق أعلى، وهذا قد يربك القارئ أو يغيّر السلوك المتوقع.",
                "Symbol '" + symbol.name + "' shadows an outer declaration.",
                symbol.name);
        }

        symbols.emplace(symbol.name, symbol);
        ++result_.summary.totalSymbols;
        return true;
    }

    SymbolInfo *AstAnalysisVisitor::resolveSymbol(const std::string &name)
    {
        for (auto scopeIt = scopes_.rbegin(); scopeIt != scopes_.rend(); ++scopeIt)
        {
            auto symbolIt = scopeIt->symbols.find(name);
            if (symbolIt != scopeIt->symbols.end())
            {
                return &symbolIt->second;
            }
        }

        return nullptr;
    }

    const SymbolInfo *AstAnalysisVisitor::resolveSymbol(const std::string &name) const
    {
        for (auto scopeIt = scopes_.rbegin(); scopeIt != scopes_.rend(); ++scopeIt)
        {
            auto symbolIt = scopeIt->symbols.find(name);
            if (symbolIt != scopeIt->symbols.end())
            {
                return &symbolIt->second;
            }
        }

        return nullptr;
    }

    bool AstAnalysisVisitor::existsInParentScopes(const std::string &name) const
    {
        if (scopes_.size() < 2)
        {
            return false;
        }

        for (std::size_t index = scopes_.size() - 1; index > 0; --index)
        {
            const auto &scope = scopes_[index - 1];
            if (scope.symbols.find(name) != scope.symbols.end())
            {
                return true;
            }
        }

        return false;
    }

    bool AstAnalysisVisitor::isSuppressedUnusedName(const std::string &name) const
    {
        return !name.empty() && name.front() == '_';
    }

    bool AstAnalysisVisitor::isTerminatingStatement(const AST::Statement *statement) const
    {
        return dynamic_cast<const AST::ReturnStmt *>(statement) != nullptr ||
               dynamic_cast<const AST::BreakStmt *>(statement) != nullptr ||
               dynamic_cast<const AST::ContinueStmt *>(statement) != nullptr ||
               dynamic_cast<const AST::RaiseStmt *>(statement) != nullptr;
    }

    void AstAnalysisVisitor::analyzeStatementList(AST::StmtList &statements)
    {
        bool terminated = false;

        for (const auto &statement : statements)
        {
            if (!statement)
            {
                continue;
            }

            ++result_.summary.totalStatements;

            if (terminated && options_.enableUnreachableCodeWarnings)
            {
                addIssue(
                    "SAD-FLOW-001",
                    IssueKind::Flow,
                    Errors::DiagnosticSeverity::WARNING,
                    makeLocation(statement->position),
                    "هذه التعليمة غير قابلة للوصول لأن التعليمة السابقة أنهت مسار التنفيذ داخل نفس الكتلة.",
                    "This statement is unreachable because a previous statement already terminated control flow.");
                continue;
            }

            statement->accept(*this);
            terminated = isTerminatingStatement(statement.get());
        }
    }

    void AstAnalysisVisitor::analyzeParameters(const std::vector<AST::Parameter> &parameters,
                                               const std::string &ownerName)
    {
        if (!options_.enableComplexityWarnings && parameters.size() > options_.maxFunctionParameters)
        {
            addIssue(
                "SAD-STYLE-001",
                IssueKind::Style,
                Errors::DiagnosticSeverity::WARNING,
                Errors::SourceLocation(filePath_, 1, 1, 0, 1),
                "العنصر '" + ownerName + "' يملك عدداً كبيراً من المعاملات. فكر في تجميعها ضمن بنية أو كائن إعدادات لتحسين القراءة والصيانة.",
                "'" + ownerName + "' has too many parameters; consider grouping them into a structure or options object.");
        }

        for (const AST::Parameter &parameter : parameters)
        {
            SymbolInfo symbol;
            symbol.name = parameter.name;
            symbol.kind = SymbolKind::Parameter;
            symbol.isConst = false;
            symbol.location = Errors::SourceLocation(filePath_, 1, 1, 0, 1);
            declareSymbol(symbol, false);

            if (parameter.defaultValue)
            {
                parameter.defaultValue->accept(*this);
            }
        }
    }

    void AstAnalysisVisitor::predeclareTopLevelStatement(AST::Statement *statement)
    {
        if (statement == nullptr)
        {
            return;
        }

        if (auto *functionDecl = dynamic_cast<AST::FunctionDecl *>(statement))
        {
            SymbolInfo symbol;
            symbol.name = functionDecl->name;
            symbol.kind = SymbolKind::Function;
            symbol.isCallable = true;
            symbol.location = makeLocation(functionDecl->position);
            declareSymbol(symbol, false);
            return;
        }

        if (auto *classDecl = dynamic_cast<AST::ClassDecl *>(statement))
        {
            SymbolInfo symbol;
            symbol.name = classDecl->name;
            symbol.kind = SymbolKind::Class;
            symbol.location = makeLocation(classDecl->position);
            declareSymbol(symbol, false);
            return;
        }

        if (auto *classDeclStmt = dynamic_cast<AST::ClassDeclStmt *>(statement))
        {
            SymbolInfo symbol;
            symbol.name = classDeclStmt->name;
            symbol.kind = SymbolKind::Class;
            symbol.location = makeLocation(classDeclStmt->position);
            declareSymbol(symbol, false);
            return;
        }

        if (auto *enumDecl = dynamic_cast<AST::EnumDecl *>(statement))
        {
            SymbolInfo symbol;
            symbol.name = enumDecl->name;
            symbol.kind = SymbolKind::EnumName;
            symbol.location = makeLocation(enumDecl->position);
            declareSymbol(symbol, false);
            return;
        }

        if (auto *importStmt = dynamic_cast<AST::ImportStmt *>(statement))
        {
            SymbolInfo symbol;
            symbol.name = importStmt->getEffectiveName();
            symbol.kind = SymbolKind::ImportAlias;
            symbol.location = makeLocation(importStmt->position);
            declareSymbol(symbol, false);
            return;
        }

        if (auto *fromImportStmt = dynamic_cast<AST::FromImportStmt *>(statement))
        {
            for (const auto &item : fromImportStmt->items)
            {
                SymbolInfo symbol;
                symbol.name = item.getEffectiveName();
                symbol.kind = SymbolKind::ImportAlias;
                symbol.location = makeLocation(item.position.line == 0 ? fromImportStmt->position : item.position);
                declareSymbol(symbol, false);
            }
        }
    }

    void AstAnalysisVisitor::predeclareClassMembers(AST::ClassDecl &decl)
    {
        for (const auto &member : decl.members)
        {
            if (member == nullptr)
            {
                continue;
            }

            if (auto *field = dynamic_cast<AST::FieldDecl *>(member.get()))
            {
                SymbolInfo symbol;
                symbol.name = field->name;
                symbol.kind = SymbolKind::Field;
                symbol.isConst = false;
                symbol.location = makeLocation(field->position);
                declareSymbol(symbol, false);
            }
            else if (auto *method = dynamic_cast<AST::MethodDecl *>(member.get()))
            {
                SymbolInfo symbol;
                symbol.name = method->name;
                symbol.kind = SymbolKind::Method;
                symbol.isCallable = true;
                symbol.location = makeLocation(method->position);
                declareSymbol(symbol, false);
            }
            else if (auto *property = dynamic_cast<AST::PropertyDecl *>(member.get()))
            {
                SymbolInfo symbol;
                symbol.name = property->name;
                symbol.kind = SymbolKind::Property;
                symbol.location = makeLocation(property->position);
                declareSymbol(symbol, false);
            }
        }
    }

    void AstAnalysisVisitor::predeclareClassMembers(AST::ClassDeclStmt &stmt)
    {
        for (const auto &field : stmt.fields)
        {
            if (!field)
            {
                continue;
            }

            SymbolInfo symbol;
            symbol.name = field->name;
            symbol.kind = SymbolKind::Field;
            symbol.location = makeLocation(field->position);
            declareSymbol(symbol, false);
        }

        for (const auto &method : stmt.methods)
        {
            if (!method)
            {
                continue;
            }

            SymbolInfo symbol;
            symbol.name = method->name;
            symbol.kind = SymbolKind::Method;
            symbol.isCallable = true;
            symbol.location = makeLocation(method->position);
            declareSymbol(symbol, false);
        }
    }

    void AstAnalysisVisitor::flushUnusedWarnings(const ScopeFrame &scope)
    {
        if (!options_.enableUnusedSymbolWarnings)
        {
            return;
        }

        for (const auto &[name, symbol] : scope.symbols)
        {
            if (symbol.isBuiltin || isSuppressedUnusedName(name))
            {
                continue;
            }

            const bool localLike = symbol.kind == SymbolKind::Variable ||
                                   symbol.kind == SymbolKind::Parameter ||
                                   symbol.kind == SymbolKind::ImportAlias;

            if (!localLike)
            {
                continue;
            }

            if (symbol.readCount == 0)
            {
                addIssue(
                    "SAD-STYLE-002",
                    IssueKind::Style,
                    Errors::DiagnosticSeverity::WARNING,
                    symbol.location,
                    "تم تعريف " + kindToArabic(symbol.kind) + " '" + name + "' لكنه لم يُستخدم لاحقاً داخل النطاق الحالي.",
                    "The " + kindToArabic(symbol.kind) + " '" + name + "' is never used.",
                    name);
            }
        }
    }

    void AstAnalysisVisitor::markSymbolRead(const std::string &name, const Lexer::Position &position, bool requireCallable)
    {
        SymbolInfo *symbol = resolveSymbol(name);
        if (symbol == nullptr)
        {
            addIssue(
                requireCallable ? "SAD-SEM-004" : "SAD-SEM-003",
                IssueKind::Semantic,
                Errors::DiagnosticSeverity::ERROR,
                makeLocation(position),
                requireCallable
                    ? "استدعاء الرمز '" + name + "' فشل لأن الدالة غير معرّفة في النطاق الحالي أو النطاقات المحيطة."
                    : "الرمز '" + name + "' مستخدم قبل أن يكون معروفاً للمحلل داخل هذا السياق.",
                requireCallable
                    ? "Call to undefined function '" + name + "'."
                    : "Undefined symbol '" + name + "'.",
                name);
            return;
        }

        if (requireCallable && !symbol->isCallable)
        {
            addIssue(
                "SAD-SEM-005",
                IssueKind::Semantic,
                Errors::DiagnosticSeverity::ERROR,
                makeLocation(position),
                "الرمز '" + name + "' معروف، لكنه ليس دالة أو رمزاً قابلاً للاستدعاء.",
                "Symbol '" + name + "' is defined but is not callable.",
                name);
            return;
        }

        if (!requireCallable && symbol->kind == SymbolKind::Variable && symbol->writeCount == 0)
        {
            addIssue(
                "SAD-FLOW-003",
                IssueKind::Flow,
                Errors::DiagnosticSeverity::WARNING,
                makeLocation(position),
                "تمت قراءة المتغير '" + name + "' قبل أن يأخذ قيمة فعلية.",
                "Variable '" + name + "' is read before it is initialized.",
                name);
        }

        ++symbol->readCount;
    }

    void AstAnalysisVisitor::visitIfPresent(AST::Expression *expression)
    {
        if (expression != nullptr)
        {
            expression->accept(*this);
        }
    }

    void AstAnalysisVisitor::visitIfPresent(AST::Statement *statement)
    {
        if (statement != nullptr)
        {
            statement->accept(*this);
        }
    }

    void AstAnalysisVisitor::checkConstantCondition(AST::Expression *expression,
                                                    const Lexer::Position &position,
                                                    const std::string &ownerLabel)
    {
        if (!options_.enableConstantConditionWarnings || expression == nullptr)
        {
            return;
        }

        auto *literal = dynamic_cast<AST::LiteralExpr *>(expression);
        if (literal == nullptr)
        {
            return;
        }

        addIssue(
            "SAD-FLOW-002",
            IssueKind::Flow,
            Errors::DiagnosticSeverity::WARNING,
            makeLocation(position),
            "شرط " + issueOwnerLabel(ownerLabel) + " ثابت حرفياً، وهذا غالباً يشير إلى منطق لم يكتمل أو فرع لم يعد يتغير.",
            "Condition for " + issueOwnerLabel(ownerLabel) + " is a literal constant.");
    }

    void AstAnalysisVisitor::visitExprStmt(AST::ExprStmt &stmt)
    {
        visitIfPresent(stmt.expression.get());
    }

    void AstAnalysisVisitor::visitVarDeclStmt(AST::VarDeclStmt &stmt)
    {
        SymbolInfo symbol;
        symbol.name = stmt.name;
        symbol.kind = stmt.isConst ? SymbolKind::Constant : SymbolKind::Variable;
        symbol.isConst = stmt.isConst;
        symbol.location = makeLocation(stmt.position);
        symbol.mayBeNull = stmt.initializer ? !isDefinitelyNonNullExpression(stmt.initializer.get()) : true;
        declareSymbol(symbol, true);
        visitIfPresent(stmt.initializer.get());

        if (SymbolInfo *declared = resolveSymbol(stmt.name))
        {
            if (stmt.initializer != nullptr)
            {
                ++declared->writeCount;
            }
        }
    }

    void AstAnalysisVisitor::visitIfStmt(AST::IfStmt &stmt)
    {
        visitIfPresent(stmt.condition.get());
        checkConstantCondition(stmt.condition.get(), stmt.position, "تعليمة إذا");
        enterNestedControlFlow(stmt.position, "تعليمة إذا");

        enterScope(ScopeKind::Conditional, "فرع إذا");
        visitIfPresent(stmt.thenBranch.get());
        leaveScope();

        if (stmt.elseBranch)
        {
            enterScope(ScopeKind::Conditional, "فرع وإلا");
            visitIfPresent(stmt.elseBranch.get());
            leaveScope();
        }

        leaveNestedControlFlow();
    }

    void AstAnalysisVisitor::visitWhileStmt(AST::WhileStmt &stmt)
    {
        visitIfPresent(stmt.condition.get());
        checkConstantCondition(stmt.condition.get(), stmt.position, "حلقة بينما");
        enterNestedControlFlow(stmt.position, "حلقة بينما");
        enterScope(ScopeKind::Loop, "حلقة بينما");
        visitIfPresent(stmt.body.get());
        leaveScope();
        leaveNestedControlFlow();
    }

    void AstAnalysisVisitor::visitForStmt(AST::ForStmt &stmt)
    {
        enterNestedControlFlow(stmt.position, "حلقة لكل");
        enterScope(ScopeKind::Loop, "حلقة لكل");
        visitIfPresent(stmt.initializer.get());
        visitIfPresent(stmt.condition.get());
        checkConstantCondition(stmt.condition.get(), stmt.position, "حلقة لكل");
        visitIfPresent(stmt.increment.get());
        visitIfPresent(stmt.body.get());
        leaveScope();
        leaveNestedControlFlow();
    }

    void AstAnalysisVisitor::visitForRangeStmt(AST::ForRangeStmt &stmt)
    {
        enterNestedControlFlow(stmt.position, "حلقة مجال");
        enterScope(ScopeKind::Loop, "حلقة مجال");

        SymbolInfo symbol;
        symbol.name = stmt.variable;
        symbol.kind = SymbolKind::Variable;
        symbol.location = makeLocation(stmt.position);
        declareSymbol(symbol, true);

        visitIfPresent(stmt.iterable.get());
        visitIfPresent(stmt.body.get());
        leaveScope();
        leaveNestedControlFlow();
    }

    void AstAnalysisVisitor::visitReturnStmt(AST::ReturnStmt &stmt)
    {
        visitIfPresent(stmt.value.get());
    }

    void AstAnalysisVisitor::visitBreakStmt(AST::BreakStmt &) {}

    void AstAnalysisVisitor::visitContinueStmt(AST::ContinueStmt &) {}

    void AstAnalysisVisitor::visitBlockStmt(AST::BlockStmt &stmt)
    {
        if (stmt.statements.empty())
        {
            addIssue(
                "SAD-STYLE-003",
                IssueKind::Style,
                Errors::DiagnosticSeverity::HINT,
                makeLocation(stmt.position),
                "هذه الكتلة فارغة. إذا كان الفراغ مقصوداً فالأفضل توضيح السبب بتعليق صريح حتى لا تبدو كأنها سهو.",
                "This block is empty. Consider documenting intentional emptiness with a comment.");
        }

        enterScope(ScopeKind::Block, "كتلة");
        analyzeStatementList(stmt.statements);
        leaveScope();
    }

    void AstAnalysisVisitor::visitTryStmt(AST::TryStmt &stmt)
    {
        enterScope(ScopeKind::Exception, "كتلة حاول");
        visitIfPresent(stmt.tryBlock.get());
        leaveScope();

        for (auto &clause : stmt.catchClauses)
        {
            enterScope(ScopeKind::Exception, "كتلة امسك");

            if (!clause.exceptionVar.empty())
            {
                SymbolInfo symbol;
                symbol.name = clause.exceptionVar;
                symbol.kind = SymbolKind::Variable;
                symbol.location = makeLocation(stmt.position);
                declareSymbol(symbol, false);
            }

            visitIfPresent(clause.body.get());
            leaveScope();
        }

        visitIfPresent(stmt.finallyBlock.get());
    }

    void AstAnalysisVisitor::visitRaiseStmt(AST::RaiseStmt &stmt)
    {
        visitIfPresent(stmt.exception.get());
    }

    void AstAnalysisVisitor::visitWithStmt(AST::WithStmt &stmt)
    {
        enterScope(ScopeKind::Block, "كتلة باستخدام");
        visitIfPresent(stmt.resource.get());

        if (!stmt.alias.empty())
        {
            SymbolInfo symbol;
            symbol.name = stmt.alias;
            symbol.kind = SymbolKind::Variable;
            symbol.location = makeLocation(stmt.position);
            declareSymbol(symbol, false);
        }

        visitIfPresent(stmt.body.get());
        leaveScope();
    }

    // ============================================================================
    // (AR) جمل التزامن — أطلق / اختر / (EN) Concurrency statements — go / select
    // ============================================================================

    void AstAnalysisVisitor::visitGoStmt(AST::GoStmt &stmt)
    {
        // (AR) تحليل التعبير المُطلَق (استدعاء دالة أو لامدا)
        // (EN) Analyze the launched expression (function call or lambda)
        if (stmt.expression)
        {
            visitIfPresent(stmt.expression.get());
        }
        // (AR) تحليل كتلة الكود إذا كان "أطلق ... نهاية"
        // (EN) Analyze the block body if "go ... end"
        if (stmt.blockBody)
        {
            enterScope(ScopeKind::Block, "كتلة أطلق");
            visitIfPresent(stmt.blockBody.get());
            leaveScope();
        }
    }

    void AstAnalysisVisitor::visitSelectCase(AST::SelectCase &stmt)
    {
        // (AR) تحليل تعبير القناة
        // (EN) Analyze channel expression
        if (stmt.channelExpr)
        {
            visitIfPresent(stmt.channelExpr.get());
        }
        // (AR) تحليل جسم الحالة مع نطاق خاص لربط المتغير
        // (EN) Analyze case body with scope for bound variable
        enterScope(ScopeKind::Block, "حالة اختر");
        if (!stmt.bindVar.empty())
        {
            SymbolInfo symbol;
            symbol.name = stmt.bindVar;
            symbol.kind = SymbolKind::Variable;
            symbol.location = makeLocation(stmt.position);
            declareSymbol(symbol, false);
        }
        for (auto &s : stmt.body)
        {
            visitIfPresent(s.get());
        }
        leaveScope();
    }

    void AstAnalysisVisitor::visitSelectStmt(AST::SelectStmt &stmt)
    {
        // (AR) تحليل كل حالة اختيار
        // (EN) Analyze each select case
        for (auto &c : stmt.cases)
        {
            if (c)
                c->accept(*this);
        }
        // (AR) تحليل الجسم الافتراضي
        // (EN) Analyze default body
        if (!stmt.defaultBody.empty())
        {
            enterScope(ScopeKind::Block, "افتراضي اختر");
            for (auto &s : stmt.defaultBody)
            {
                visitIfPresent(s.get());
            }
            leaveScope();
        }
    }

    void AstAnalysisVisitor::visitFunctionDecl(AST::FunctionDecl &decl)
    {
        ++result_.summary.totalFunctions;
        std::size_t functionStatementCount = 0;
        if (decl.body)
        {
            if (auto *block = dynamic_cast<AST::BlockStmt *>(decl.body.get()))
            {
                functionStatementCount = block->statements.size();
            }
            else
            {
                functionStatementCount = 1;
            }
        }
        checkFunctionComplexity(decl.name, decl.position, functionStatementCount, decl.parameters.size());

        enterScope(ScopeKind::Function, "دالة " + decl.name);
        analyzeParameters(decl.parameters, decl.name);
        visitIfPresent(decl.body.get());
        leaveScope();
    }

    void AstAnalysisVisitor::visitClassDecl(AST::ClassDecl &decl)
    {
        ++result_.summary.totalClasses;
        std::size_t methodCount = 0;
        for (const auto &member : decl.members)
        {
            if (member && dynamic_cast<AST::MethodDecl *>(member.get()))
            {
                ++methodCount;
            }
        }
        checkClassComplexity(decl.name, decl.position, methodCount);

        enterScope(ScopeKind::Class, "صنف " + decl.name);
        predeclareClassMembers(decl);

        for (const auto &member : decl.members)
        {
            if (member)
            {
                member->accept(*this);
            }
        }

        leaveScope();
    }

    void AstAnalysisVisitor::visitClassDeclStmt(AST::ClassDeclStmt &stmt)
    {
        ++result_.summary.totalClasses;
        checkClassComplexity(stmt.name, stmt.position, stmt.methods.size());

        enterScope(ScopeKind::Class, "صنف " + stmt.name);
        predeclareClassMembers(stmt);

        for (const auto &field : stmt.fields)
        {
            if (field)
            {
                field->accept(*this);
            }
        }

        if (stmt.constructor)
        {
            stmt.constructor->accept(*this);
        }

        if (stmt.destructor)
        {
            stmt.destructor->accept(*this);
        }

        for (const auto &method : stmt.methods)
        {
            if (method)
            {
                method->accept(*this);
            }
        }

        leaveScope();
    }

    void AstAnalysisVisitor::visitFieldDecl(AST::FieldDecl &decl)
    {
        visitIfPresent(decl.initializer.get());
    }

    void AstAnalysisVisitor::visitMethodDecl(AST::MethodDecl &decl)
    {
        ++result_.summary.totalFunctions;
        std::size_t methodStatementCount = 0;
        if (decl.body)
        {
            if (auto *block = dynamic_cast<AST::BlockStmt *>(decl.body.get()))
            {
                methodStatementCount = block->statements.size();
            }
            else
            {
                methodStatementCount = 1;
            }
        }
        checkFunctionComplexity(decl.name, decl.position, methodStatementCount, decl.parameters.size());

        enterScope(ScopeKind::Function, "طريقة " + decl.name);
        analyzeParameters(decl.parameters, decl.name);
        visitIfPresent(decl.body.get());
        leaveScope();
    }

    void AstAnalysisVisitor::visitPropertyDecl(AST::PropertyDecl &decl)
    {
        if (decl.getter && decl.getter->body)
        {
            enterScope(ScopeKind::Function, "قارئ الخاصية " + decl.name);
            decl.getter->body->accept(*this);
            leaveScope();
        }

        if (decl.setter && decl.setter->body)
        {
            enterScope(ScopeKind::Function, "كاتب الخاصية " + decl.name);
            if (!decl.setter->parameterName.empty())
            {
                SymbolInfo symbol;
                symbol.name = decl.setter->parameterName;
                symbol.kind = SymbolKind::Parameter;
                symbol.location = makeLocation(decl.position);
                declareSymbol(symbol, false);
            }
            decl.setter->body->accept(*this);
            leaveScope();
        }
    }

    void AstAnalysisVisitor::visitConstructorDecl(AST::ConstructorDecl &decl)
    {
        ++result_.summary.totalFunctions;
        std::size_t ctorStatementCount = 0;
        if (decl.body)
        {
            if (auto *block = dynamic_cast<AST::BlockStmt *>(decl.body.get()))
            {
                ctorStatementCount = block->statements.size();
            }
            else
            {
                ctorStatementCount = 1;
            }
        }
        checkFunctionComplexity("باني", decl.position, ctorStatementCount, decl.parameters.size());

        enterScope(ScopeKind::Function, "باني");
        analyzeParameters(decl.parameters, "باني");
        for (auto &argument : decl.superArgs)
        {
            if (argument)
            {
                argument->accept(*this);
            }
        }
        visitIfPresent(decl.body.get());
        leaveScope();
    }

    void AstAnalysisVisitor::visitDestructorDecl(AST::DestructorDecl &decl)
    {
        enterScope(ScopeKind::Function, "هادم");
        visitIfPresent(decl.body.get());
        leaveScope();
    }

    void AstAnalysisVisitor::visitEnumDecl(AST::EnumDecl &decl)
    {
        enterScope(ScopeKind::Class, "تعداد " + decl.name);
        for (const auto &member : decl.members)
        {
            SymbolInfo symbol;
            symbol.name = member.name;
            symbol.kind = SymbolKind::Constant;
            symbol.isConst = true;
            symbol.location = makeLocation(decl.position);
            declareSymbol(symbol, false);
            visitIfPresent(member.value.get());
        }
        leaveScope();
    }

    void AstAnalysisVisitor::visitImportStmt(AST::ImportStmt &) {}

    void AstAnalysisVisitor::visitFromImportStmt(AST::FromImportStmt &) {}

    void AstAnalysisVisitor::visitExportStmt(AST::ExportStmt &stmt)
    {
        visitIfPresent(stmt.declaration.get());
    }

    void AstAnalysisVisitor::visitExportDecl(AST::ExportDecl &decl)
    {
        visitIfPresent(decl.declaration.get());
    }

    void AstAnalysisVisitor::visitBinaryExpr(AST::BinaryExpr &expr)
    {
        ++result_.summary.totalExpressions;
        ++currentExpressionComplexity_;

        if (options_.enableComplexityWarnings && currentExpressionComplexity_ > options_.maxExpressionComplexity)
        {
            addIssue(
                "SAD-CPLX-005",
                IssueKind::Style,
                Errors::DiagnosticSeverity::HINT,
                makeLocation(expr.position),
                "تعبير مركب بدرجة عالية (تعقيد " + std::to_string(currentExpressionComplexity_) +
                    "). يمكن تبسيطه بتقسيمه إلى متغيرات وسيطة.",
                "Highly complex expression (complexity " + std::to_string(currentExpressionComplexity_) +
                    "). Consider breaking it into intermediate variables.");
        }

        visitIfPresent(expr.left.get());
        checkMagicNumber(expr.left.get(), expr.position);
        visitIfPresent(expr.right.get());
        checkMagicNumber(expr.right.get(), expr.position);

        if (currentExpressionComplexity_ > 0)
        {
            --currentExpressionComplexity_;
        }
    }

    void AstAnalysisVisitor::visitUnaryExpr(AST::UnaryExpr &expr)
    {
        ++result_.summary.totalExpressions;
        visitIfPresent(expr.operand.get());
    }

    void AstAnalysisVisitor::visitTernaryExpr(AST::TernaryExpr &expr)
    {
        ++result_.summary.totalExpressions;
        visitIfPresent(expr.condition.get());
        visitIfPresent(expr.trueExpr.get());
        visitIfPresent(expr.falseExpr.get());
    }

    void AstAnalysisVisitor::visitLiteralExpr(AST::LiteralExpr &)
    {
        ++result_.summary.totalExpressions;
    }

    void AstAnalysisVisitor::visitVariableExpr(AST::VariableExpr &expr)
    {
        ++result_.summary.totalExpressions;
        markSymbolRead(expr.name, expr.position, false);
    }

    void AstAnalysisVisitor::visitBorrowExpr(AST::BorrowExpr &expr)
    {
        ++result_.summary.totalExpressions;
        markSymbolRead(expr.variableName, expr.position, false);
    }

    void AstAnalysisVisitor::visitAssignExpr(AST::AssignExpr &expr)
    {
        ++result_.summary.totalExpressions;
        visitIfPresent(expr.value.get());
        checkMagicNumber(expr.value.get(), expr.position);

        SymbolInfo *symbol = resolveSymbol(expr.name);
        if (symbol == nullptr)
        {
            addIssue(
                "SAD-SEM-006",
                IssueKind::Semantic,
                Errors::DiagnosticSeverity::ERROR,
                makeLocation(expr.position),
                "لا يمكن الإسناد إلى '" + expr.name + "' لأنه غير معروف في النطاق الحالي.",
                "Cannot assign to undefined symbol '" + expr.name + "'.",
                expr.name);
            return;
        }

        if (symbol->isConst)
        {
            addIssue(
                "SAD-SEM-007",
                IssueKind::Semantic,
                Errors::DiagnosticSeverity::ERROR,
                makeLocation(expr.position),
                "يوجد إسناد إلى الثابت '" + expr.name + "'، وهذا غير مسموح بعد تعريفه.",
                "Assignment to constant '" + expr.name + "' is not allowed.",
                expr.name);
            return;
        }

        ++symbol->writeCount;
        symbol->mayBeNull = !isDefinitelyNonNullExpression(expr.value.get());

        if (auto *variableValue = dynamic_cast<AST::VariableExpr *>(expr.value.get()))
        {
            if (variableValue->name == expr.name)
            {
                addIssue(
                    "SAD-STYLE-004",
                    IssueKind::Style,
                    Errors::DiagnosticSeverity::WARNING,
                    makeLocation(expr.position),
                    "هذه العملية تبدو كإسناد ذاتي '" + expr.name + " = " + expr.name + "'، وغالباً لا تضيف سلوكاً فعلياً.",
                    "This looks like a self-assignment and likely has no effect.",
                    expr.name);
            }
        }
    }

    void AstAnalysisVisitor::visitCallExpr(AST::CallExpr &expr)
    {
        ++result_.summary.totalExpressions;
        checkRepeatedCallPattern(expr);

        if (auto *variable = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
        {
            markSymbolRead(variable->name, variable->position, true);
        }
        else
        {
            visitIfPresent(expr.callee.get());
        }

        for (auto &argument : expr.arguments)
        {
            if (argument)
            {
                argument->accept(*this);
            }
        }
    }

    void AstAnalysisVisitor::visitIndexExpr(AST::IndexExpr &expr)
    {
        ++result_.summary.totalExpressions;
        checkPossibleNullDereference(expr.object.get(), expr.position, "الوصول بالفهرس");
        visitIfPresent(expr.object.get());
        visitIfPresent(expr.index.get());
    }

    void AstAnalysisVisitor::visitMemberExpr(AST::MemberExpr &expr)
    {
        ++result_.summary.totalExpressions;
        checkPossibleNullDereference(expr.object.get(), expr.position, "الوصول لعضو");
        visitIfPresent(expr.object.get());
    }

    void AstAnalysisVisitor::visitMemberAssignExpr(AST::MemberAssignExpr &expr)
    {
        ++result_.summary.totalExpressions;
        checkPossibleNullDereference(expr.object.get(), expr.position, "إسناد عضو");
        visitIfPresent(expr.object.get());
        visitIfPresent(expr.value.get());
    }

    void AstAnalysisVisitor::visitIndexAssignExpr(AST::IndexAssignExpr &expr)
    {
        ++result_.summary.totalExpressions;
        visitIfPresent(expr.object.get());
        visitIfPresent(expr.index.get());
        visitIfPresent(expr.value.get());
    }

    void AstAnalysisVisitor::visitArrayExpr(AST::ArrayExpr &expr)
    {
        ++result_.summary.totalExpressions;
        for (auto &element : expr.elements)
        {
            if (element)
            {
                element->accept(*this);
            }
        }
    }

    void AstAnalysisVisitor::visitMapExpr(AST::MapExpr &expr)
    {
        ++result_.summary.totalExpressions;
        for (auto &pair : expr.pairs)
        {
            if (pair.key)
            {
                pair.key->accept(*this);
            }
            if (pair.value)
            {
                pair.value->accept(*this);
            }
        }
    }

    void AstAnalysisVisitor::visitWalrusExpr(AST::WalrusExpr &expr)
    {
        ++result_.summary.totalExpressions;
        visitIfPresent(expr.value.get());

        SymbolInfo symbol;
        symbol.name = expr.variable;
        symbol.kind = SymbolKind::Variable;
        symbol.location = makeLocation(expr.position);
        if (declareSymbol(symbol, true))
        {
            if (SymbolInfo *declared = resolveSymbol(expr.variable))
            {
                ++declared->writeCount;
            }
        }
    }

    void AstAnalysisVisitor::visitLambdaExpr(AST::LambdaExpr &expr)
    {
        ++result_.summary.totalExpressions;
        enterScope(ScopeKind::Function, "لامدا");
        analyzeParameters(expr.parameters, "لامدا");
        visitIfPresent(expr.body.get());
        visitIfPresent(expr.blockBody.get());
        leaveScope();
    }

    void AstAnalysisVisitor::visitNewExpr(AST::NewExpr &expr)
    {
        ++result_.summary.totalExpressions;

        const SymbolInfo *classSymbol = resolveSymbol(expr.className);
        if (classSymbol == nullptr || classSymbol->kind != SymbolKind::Class)
        {
            addIssue(
                "SAD-SEM-008",
                IssueKind::Semantic,
                Errors::DiagnosticSeverity::ERROR,
                makeLocation(expr.position),
                "محاولة إنشاء كائن من الصنف '" + expr.className + "' فشلت لأن الصنف غير معروف للمحلل.",
                "Cannot instantiate unknown class '" + expr.className + "'.",
                expr.className);
        }

        for (auto &argument : expr.arguments)
        {
            if (argument)
            {
                argument->accept(*this);
            }
        }
    }

    void AstAnalysisVisitor::visitMethodCallExpr(AST::MethodCallExpr &expr)
    {
        ++result_.summary.totalExpressions;
        checkPossibleNullDereference(expr.object.get(), expr.position, "استدعاء طريقة");
        visitIfPresent(expr.object.get());
        for (auto &argument : expr.arguments)
        {
            if (argument)
            {
                argument->accept(*this);
            }
        }
    }

    void AstAnalysisVisitor::visitMemberAccessExpr(AST::MemberAccessExpr &expr)
    {
        ++result_.summary.totalExpressions;
        checkPossibleNullDereference(expr.object.get(), expr.position, "الوصول لخاصية");
        visitIfPresent(expr.object.get());
    }

    void AstAnalysisVisitor::visitThisExpr(AST::ThisExpr &)
    {
        ++result_.summary.totalExpressions;
    }

    void AstAnalysisVisitor::visitSuperExpr(AST::SuperExpr &)
    {
        ++result_.summary.totalExpressions;
    }

} // namespace Sad::Analizer
