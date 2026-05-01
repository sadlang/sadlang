/**
 * @file statement_executor.cpp
 * @brief (AR) تنفيذ منفّذ الجُمل
 * @brief (EN) Statement Executor Implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "statement_executor.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "error_manager.h"
#include "exception.h"
#include "object_instance.h"
#include "debug_server.h"
#include "class_manager.h"
#include "channel.h"
#include "sad_type_system.h"
#include "profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include <iostream>
#include <sstream>
#include <map>

namespace Sad
{
    namespace Interpreter
    {

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) خطاف التصحيح — يُستدعى قبل تنفيذ كل جملة
        // (EN) Debug hook — called before each statement execution
        // ═══════════════════════════════════════════════════════════════
        static inline void debugHook(const Sad::Lexer::Position &pos)
        {
            auto *dbg = Sad::Debug::DebugServer::getInstance();
            if (dbg && dbg->isConnected())
            {
                dbg->onBeforeStatement(
                    getSourceFilename(),
                    static_cast<int>(pos.line),
                    static_cast<int>(pos.column));
                if (dbg->shouldStop())
                {
                    throw std::runtime_error("تم فصل المصحح / Debugger disconnected");
                }
            }
        }

        // =========================================================================
        // (AR) البناء والإعداد / (EN) Construction and Setup
        // =========================================================================

        StatementExecutor::StatementExecutor(Data::VariableManager &varMgr,
                                             Data::FunctionManager &funcMgr,
                                             Data::ScopeManager &scopeMgr,
                                             Data::OwnershipManager &ownershipMgr)
            : variableManager_(varMgr), functionManager_(funcMgr), scopeManager_(scopeMgr), ownershipManager_(ownershipMgr), flowControl_(FlowControl::NONE), returnValue_(), yieldValue_(), inGenerator_(false), loopDepth_(0), currentFunctionReturnType_(Data::DataType::UNKNOWN), currentFunctionName_("")
        {
            // (AR) إنشاء مُقيِّم التعابير / (EN) Create expression evaluator
            // Note: Pass *this to allow ExpressionEvaluator to call back for function execution
            expressionEvaluator_ = std::make_unique<ExpressionEvaluator>(varMgr, funcMgr, scopeMgr, *this, ownershipMgr);
        }

        // =========================================================================
        // (AR) دوال مساعدة خاصة / (EN) Private Helper Functions
        // =========================================================================

        Data::Value StatementExecutor::evaluateExpression(AST::Expression &expr)
        {
            expr.accept(*expressionEvaluator_);
            return expressionEvaluator_->getResult();
        }

        // =========================================================================
        // (AR) تنفيذ زيارة الجُمل الأساسية / (EN) Basic Statement Visitors
        // =========================================================================

        void StatementExecutor::visitExprStmt(AST::ExprStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) تنفيذ التعبير وتجاهل النتيجة / (EN) Execute expression and ignore result
            evaluateExpression(*node.expression);
        }

        void StatementExecutor::visitVarDeclStmt(AST::VarDeclStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) تقييم القيمة الابتدائية إن وُجدت / (EN) Evaluate initializer if present
            Data::Value value;

            if (node.initializer)
            {
                value = evaluateExpression(*node.initializer);

                // (AR) تحويل النوع إذا لزم الأمر / (EN) Type conversion if needed
                if (node.type == Data::DataType::INTEGER && value.getKind() == Types::SadTypeKind::Float)
                {
                    // (AR) تحويل عشري → رقم صحيح / (EN) Convert double → integer
                    value = Data::Value(static_cast<int>(value.toDouble()));
                }
                else if (node.type == Data::DataType::FLOAT && value.getKind() == Types::SadTypeKind::Integer)
                {
                    // (AR) تحويل رقم صحيح → عشري / (EN) Convert integer → double
                    value = Data::Value(static_cast<double>(value.toInt()));
                }

                // ═══════════════════════════════════════════════════════════
                // (AR) التحقق من توافقية الأنواع عبر النظام الموحد
                // (EN) Unified type system compatibility check
                // ═══════════════════════════════════════════════════════════
                if (node.sadType && node.type != Data::DataType::UNKNOWN)
                {
                    auto valueType = Types::SadType::fromValueType(value.getType());
                    if (valueType && !valueType->isAssignableTo(node.sadType.get()))
                    {
                        // (AR) تحذير: عدم توافق الأنواع
                        // (EN) Warning: type mismatch
                        std::string warnAr =
                            "تحذير: تعيين قيمة من نوع '" + valueType->arabicName() +
                            "' لمتغير '" + node.name + "' من نوع '" +
                            node.sadType->arabicName() + "'";
                        std::string warnEn =
                            "Type mismatch: assigning '" + valueType->englishName() +
                            "' to variable '" + node.name + "' of type '" +
                            node.sadType->englishName() + "'";

                        // (AR) طباعة التحذير مباشرة / (EN) Print warning directly
                        std::cerr << "[تحذير نوع] سطر " << node.position.line
                                  << ": " << warnAr << std::endl;

                        // (AR) تسجيل في نظام التشخيصات أيضاً
                        Sad::Errors::SourceLocation loc(
                            "", node.position.line, node.position.column);
                        Sad::Errors::ErrorManager::getInstance().reportWarning(
                            Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,
                            loc, warnAr, warnEn);
                    }
                }
            }
            else
            {
                // (AR) قيمة افتراضية حسب النوع / (EN) Default value based on type
                switch (node.type)
                {
                case Data::DataType::INTEGER:
                    value = Data::Value(0);
                    break;
                case Data::DataType::FLOAT:
                    value = Data::Value(0.0);
                    break;
                case Data::DataType::STRING:
                    value = Data::Value("");
                    break;
                case Data::DataType::BOOLEAN:
                    value = Data::Value(false);
                    break;
                default:
                    value = Data::Value(); // VOID
                    break;
                }
            }

            // (AR) تعريف المتغير / (EN) Define variable
            try
            {
                if (node.isConst)
                {
                    variableManager_.defineConst(node.name, value);
                }
                else
                {
                    variableManager_.define(node.name, value);
                }

                // (AR) خطاف مصحح الأداء — تتبع إنشاء المتغيرات
                // (EN) Profiler hook — track variable creation
                if (auto *prof = Sad::Tools::getGlobalProfiler())
                {
                    prof->onVariableCreated(node.name, "");
                }

                // (AR) تسجيل المتغير في نظام الملكية / (EN) Register variable in ownership system
                if (ownershipManager_.isEnabled())
                {
                    std::string typeName;
                    switch (node.type)
                    {
                    case Data::DataType::INTEGER:
                        typeName = "INTEGER";
                        break;
                    case Data::DataType::FLOAT:
                        typeName = "FLOAT";
                        break;
                    case Data::DataType::STRING:
                        typeName = "نص";
                        break;
                    case Data::DataType::BOOLEAN:
                        typeName = "BOOLEAN";
                        break;
                    case Data::DataType::ARRAY:
                        typeName = "مصفوفة";
                        break;
                    case Data::DataType::MAP:
                        typeName = "قاموس";
                        break;
                    case Data::DataType::OBJECT:
                        typeName = "كائن";
                        break;
                    default:
                    {
                        // (AR) استدلال النوع من القيمة عندما يكون النوع غير محدد (متغير)
                        // (EN) Infer type from value when type is unspecified (متغير keyword)
                        auto vk = value.getKind();
                        if (vk == Types::SadTypeKind::Integer)
                            typeName = "INTEGER";
                        else if (vk == Types::SadTypeKind::Float)
                            typeName = "FLOAT";
                        else if (vk == Types::SadTypeKind::String)
                            typeName = "نص";
                        else if (vk == Types::SadTypeKind::Boolean)
                            typeName = "BOOLEAN";
                        else if (vk == Types::SadTypeKind::Array)
                            typeName = "مصفوفة";
                        else if (vk == Types::SadTypeKind::Map)
                            typeName = "قاموس";
                        else
                            typeName = "";
                        break;
                    }
                    }
                    ownershipManager_.declareVariable(node.name, typeName);

                    // (AR) نقل الملكية: إذا كان المُهيّئ متغيراً، انقل ملكيته
                    // (EN) Move semantics: if initializer is a variable, move ownership from it
                    if (node.initializer)
                    {
                        auto *varExpr = dynamic_cast<AST::VariableExpr *>(node.initializer.get());
                        if (varExpr && !varExpr->name.empty() && varExpr->name != node.name)
                        {
                            auto moveError = ownershipManager_.moveVariable(varExpr->name);
                            if (moveError.has_value())
                            {
                                // (AR) تمرير الخطأ عبر dispatch() — السلوك يُحدَّد من سياسة الذاكرة
                                //     Disabled → تجاهل، Warnings → تحذير، Strict/UltraStrict → استثناء
                                // (EN) Route error through dispatch() — behavior determined by memory policy
                                //     Disabled → ignore, Warnings → warn, Strict/UltraStrict → throw
                                ::Sad::Errors::SourceLocation loc;
                                loc.filename = currentFilePath_;
                                loc.line = node.position.line;
                                loc.column = node.position.column;

                                auto dispResult = ::Sad::Errors::dispatch(
                                    moveError->kind, memoryPolicy_, loc, varExpr->name);

                                if (dispResult.shouldStop())
                                {
                                    throw SadException(
                                        dispResult.messageAr + " / " + dispResult.messageEn,
                                        "OwnershipError",
                                        node.position);
                                }
                                // (AR) في --learn: طباعة التحذير والمضي
                                // (EN) In --learn: print warning and continue
                                if (dispResult.shouldEmit())
                                {
                                    std::cerr << "⚠ [ص-ملكية] " << dispResult.messageAr << "\n";
                                    if (dispResult.teachingNote.has_value())
                                    {
                                        std::cerr << dispResult.teachingNote.value() << "\n";
                                    }
                                }
                                // (AR) في --gc (Ignore): لا شيء — استمر بدون أي رسالة
                                // (EN) In --gc (Ignore): nothing — continue without any message
                            }
                        }
                    }
                }
            }
            catch (const std::runtime_error &e)
            {
                // (AR) إضافة معلومات الموقع للخطأ / (EN) Add position info to error
                std::ostringstream oss;
                oss << e.what() << "\n"
                    << "📍 (AR) الموقع / (EN) Location: السطر / Line " << node.position.line
                    << "، العمود / Column " << node.position.column;
                throw SadException(oss.str(), "RuntimeError", node.position);
            }
        }

        void StatementExecutor::visitBlockStmt(AST::BlockStmt &node)
        {
            // (AR) دخول نطاق جديد / (EN) Enter new scope
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            ownershipManager_.enterScope();

            // (AR) خطاف مصحح الأداء — تتبع النطاقات
            // (EN) Profiler hook — track scopes
            if (auto *prof = Sad::Tools::getGlobalProfiler())
            {
                prof->onScopeEnter("block", scopeManager_.getCurrentDepth());
            }

            // (AR) تنفيذ جميع الجمل في الكتلة / (EN) Execute all statements in block
            try
            {
                for (auto &stmt : node.statements)
                {
                    // (AR) خطاف التصحيح لكل جملة / (EN) Debug hook for each statement
                    debugHook(stmt->position);

                    stmt->accept(*this);

                    // (AR) إيقاف التنفيذ إذا كان هناك تحكم بالتدفق / (EN) Stop if flow control active
                    if (shouldStopExecution())
                    {
                        break;
                    }
                }
            }
            catch (...)
            {
                // (AR) ضمان تنظيف النطاق عند انتشار الاستثناء (مثل ارمي داخل كتلة إذا/بينما)
                // (EN) Ensure scope cleanup when exception propagates (e.g. throw inside if/while block)
                ownershipManager_.exitScope();
                variableManager_.exitScope();
                throw;
            }

            // (AR) الخروج من النطاق / (EN) Exit scope
            ownershipManager_.exitScope();
            variableManager_.exitScope();

            // (AR) خطاف مصحح الأداء — خروج من النطاق
            // (EN) Profiler hook — scope exit
            if (auto *prof = Sad::Tools::getGlobalProfiler())
            {
                prof->onScopeExit("block", scopeManager_.getCurrentDepth());
            }
        }

        // =========================================================================
        // (AR) تنفيذ جُمل التحكم بالتدفق / (EN) Control Flow Statement Visitors
        // =========================================================================

        void StatementExecutor::visitIfStmt(AST::IfStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) تقييم الشرط / (EN) Evaluate condition
            Data::Value condition = evaluateExpression(*node.condition);

            // (AR) تنفيذ الفرع المناسب — يدعم عامل منطقي() الضمني
            // (EN) Execute appropriate branch — supports implicit منطقي() operator
            if (expressionEvaluator_->convertObjectToBool(condition, node.position))
            {
                node.thenBranch->accept(*this);
            }
            else if (node.elseBranch)
            {
                node.elseBranch->accept(*this);
            }
        }

        void StatementExecutor::visitWhileStmt(AST::WhileStmt &node)
        {
            // (AR) خطاف التصحيح / (EN) Debug hook
            debugHook(node.position);
            // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
            loopDepth_++;

            // (AR) تنفيذ الحلقة / (EN) Execute loop
            while (true)
            {
                // (AR) تقييم الشرط — يدعم عامل منطقي() الضمني
                // (EN) Evaluate condition — supports implicit منطقي() operator
                Data::Value condition = evaluateExpression(*node.condition);

                if (!expressionEvaluator_->convertObjectToBool(condition, node.position))
                {
                    break;
                }

                // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                node.body->accept(*this);

                // (AR) معالجة break / (EN) Handle break
                if (flowControl_ == FlowControl::BREAK)
                {
                    flowControl_ = FlowControl::NONE;
                    break;
                }

                // (AR) معالجة continue / (EN) Handle continue
                if (flowControl_ == FlowControl::CONTINUE)
                {
                    flowControl_ = FlowControl::NONE;
                    continue;
                }

                // (AR) معالجة return (يُمرر للأعلى) / (EN) Handle return (pass to caller)
                if (flowControl_ == FlowControl::RETURN)
                {
                    break;
                }
            }

            // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
            loopDepth_--;
        }

        void StatementExecutor::visitForStmt(AST::ForStmt &node)
        {
            // (AR) دخول نطاق جديد للحلقة / (EN) Enter new scope for loop
            variableManager_.enterScope(Data::ScopeType::LOOP);

            // (AR) تنفيذ التهيئة / (EN) Execute initializer
            if (node.initializer)
            {
                node.initializer->accept(*this);
            }

            // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
            loopDepth_++;

            // (AR) تنفيذ الحلقة / (EN) Execute loop
            while (true)
            {
                // (AR) تقييم الشرط / (EN) Evaluate condition
                if (node.condition)
                {
                    Data::Value condition = evaluateExpression(*node.condition);
                    if (!condition.toBool())
                    {
                        break;
                    }
                }

                // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                node.body->accept(*this);

                // (AR) معالجة break / (EN) Handle break
                if (flowControl_ == FlowControl::BREAK)
                {
                    flowControl_ = FlowControl::NONE;
                    break;
                }

                // (AR) معالجة continue / (EN) Handle continue
                if (flowControl_ == FlowControl::CONTINUE)
                {
                    flowControl_ = FlowControl::NONE;
                }

                // (AR) معالجة return / (EN) Handle return
                if (flowControl_ == FlowControl::RETURN)
                {
                    break;
                }

                // (AR) تنفيذ الزيادة / (EN) Execute increment
                if (node.increment)
                {
                    evaluateExpression(*node.increment);
                }
            }

            // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
            loopDepth_--;

            // (AR) الخروج من نطاق الحلقة / (EN) Exit loop scope
            variableManager_.exitScope();
        }

        void StatementExecutor::visitForRangeStmt(AST::ForRangeStmt &node)
        {
            // (AR) تقييم المجموعة القابلة للتكرار / (EN) Evaluate iterable
            Data::Value iterable = evaluateExpression(*node.iterable);

            // (AR) دخول نطاق جديد / (EN) Enter new scope
            variableManager_.enterScope(Data::ScopeType::LOOP);

            // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
            loopDepth_++;

            // (AR) التكرار حسب نوع المجموعة / (EN) Iterate based on collection type
            if (iterable.isArray())
            {
                const auto &arr = iterable.toArrayRef();

                // ═══════════════════════════════════════════════════════════
                // (AR) تحسين أداء: تعريف متغير الحلقة مرة واحدة قبل التكرار
                //      ثم تحديثه فقط داخل الحلقة — يُلغي بحث exists() المزدوج
                // (EN) Performance: define loop var once before loop,
                //      then assign inside — eliminates double exists() lookup
                // ═══════════════════════════════════════════════════════════
                if (!arr.empty())
                {
                    variableManager_.define(node.variable, arr[0]);
                    node.body->accept(*this);

                    if (flowControl_ == FlowControl::BREAK)
                    {
                        flowControl_ = FlowControl::NONE;
                    }
                    else if (flowControl_ == FlowControl::RETURN)
                    { /* handled below */
                    }
                    else
                    {
                        if (flowControl_ == FlowControl::CONTINUE)
                        {
                            flowControl_ = FlowControl::NONE;
                        }

                        for (size_t i = 1; i < arr.size(); i++)
                        {
                            variableManager_.assign(node.variable, arr[i]);

                            // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                            node.body->accept(*this);

                            // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
                            if (flowControl_ == FlowControl::BREAK)
                            {
                                flowControl_ = FlowControl::NONE;
                                break;
                            }
                            if (flowControl_ == FlowControl::CONTINUE)
                            {
                                flowControl_ = FlowControl::NONE;
                                continue;
                            }
                            if (flowControl_ == FlowControl::RETURN)
                            {
                                break;
                            }
                        }
                    }
                }
            }
            else if (iterable.isMap())
            {
                const auto &map = iterable.toMap();

                // ═══════════════════════════════════════════════════════════
                // (AR) تحسين أداء: تعريف قبل الحلقة ثم تحديث فقط
                // (EN) Performance: define before loop, then assign only
                // ═══════════════════════════════════════════════════════════
                bool firstIteration = true;
                for (const auto &[key, value] : map)
                {
                    if (firstIteration)
                    {
                        variableManager_.define(node.variable, Data::Value(key));
                        if (!node.valueVar.empty())
                        {
                            variableManager_.define(node.valueVar, value);
                        }
                        firstIteration = false;
                    }
                    else
                    {
                        variableManager_.assign(node.variable, Data::Value(key));
                        if (!node.valueVar.empty())
                        {
                            variableManager_.assign(node.valueVar, value);
                        }
                    }

                    // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                    node.body->accept(*this);

                    // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
                    if (flowControl_ == FlowControl::BREAK)
                    {
                        flowControl_ = FlowControl::NONE;
                        break;
                    }
                    if (flowControl_ == FlowControl::CONTINUE)
                    {
                        flowControl_ = FlowControl::NONE;
                        continue;
                    }
                    if (flowControl_ == FlowControl::RETURN)
                    {
                        break;
                    }
                }
            }
            else if (iterable.isString())
            {
                // ═══════════════════════════════════════════════════════════
                // (AR) تحسين أداء: بناء مصفوفة الأحرف UTF-8 مرة واحدة
                //      ثم التكرار عبرها — يُلغي substr و exists() المزدوج
                // (EN) Performance: build UTF-8 char array once,
                //      then iterate — eliminates substr + double exists()
                // ═══════════════════════════════════════════════════════════
                const std::string &text = iterable.toString();

                // (AR) بناء مصفوفة الأحرف مسبقاً / (EN) Pre-build character array
                std::vector<std::string> chars;
                const unsigned char *bytes = reinterpret_cast<const unsigned char *>(text.data());
                size_t len = text.size();
                size_t pos = 0;
                chars.reserve(len); // (AR) حجز أقصى عدد ممكن / (EN) Reserve max possible count

                while (pos < len)
                {
                    size_t charLen = 1;
                    unsigned char b = bytes[pos];
                    if (b >= 0xF0)
                        charLen = 4;
                    else if (b >= 0xE0)
                        charLen = 3;
                    else if (b >= 0xC0)
                        charLen = 2;
                    if (pos + charLen > len)
                        charLen = len - pos;
                    chars.push_back(text.substr(pos, charLen));
                    pos += charLen;
                }

                // (AR) التكرار مع تعريف مرة واحدة / (EN) Iterate with define-once pattern
                if (!chars.empty())
                {
                    variableManager_.define(node.variable, Data::Value(chars[0]));
                    node.body->accept(*this);

                    if (flowControl_ == FlowControl::BREAK)
                    {
                        flowControl_ = FlowControl::NONE;
                    }
                    else if (flowControl_ == FlowControl::RETURN)
                    { /* handled below */
                    }
                    else
                    {
                        if (flowControl_ == FlowControl::CONTINUE)
                        {
                            flowControl_ = FlowControl::NONE;
                        }

                        for (size_t i = 1; i < chars.size(); i++)
                        {
                            variableManager_.assign(node.variable, Data::Value(std::move(chars[i])));

                            node.body->accept(*this);

                            if (flowControl_ == FlowControl::BREAK)
                            {
                                flowControl_ = FlowControl::NONE;
                                break;
                            }
                            if (flowControl_ == FlowControl::CONTINUE)
                            {
                                flowControl_ = FlowControl::NONE;
                                continue;
                            }
                            if (flowControl_ == FlowControl::RETURN)
                            {
                                break;
                            }
                        }
                    }
                }
            }
            // ═══════════════════════════════════════════════════════════════
            // (AR) تكرار عبر قناة — لكل عنصر في قناة ... نهاية
            //      يستقبل من القناة حتى تُغلق وتفرغ
            // (EN) Iterate over channel — for each element in channel ... end
            //      Receives from channel until closed and empty
            // ═══════════════════════════════════════════════════════════════
            else if (iterable.isObject())
            {
                auto objPtr = iterable.toObject();
                bool isChannel = false;
                if (objPtr)
                {
                    auto classFieldIt = objPtr->fields.find("__class__");
                    isChannel = (classFieldIt != objPtr->fields.end() &&
                                 classFieldIt->second.toString() == "__\xD9\x82\xD9\x86\xD8\xA7\xD8\xA9__"); // __قناة__
                }

                if (isChannel)
                {
                    // (AR) الحصول على القناة من السجل / (EN) Get channel from registry
                    auto channelIt = objPtr->fields.find("__channel_id__");
                    if (channelIt == objPtr->fields.end())
                    {
                        Sad::Errors::ErrorManager::getInstance().reportError(
                            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                            Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                            "كائن قناة تالف",
                            "Corrupt channel object");
                    }
                    else
                    {
                        size_t channelId = static_cast<size_t>(channelIt->second.toInt());
                        auto &registry = SadChannelRegistry::getInstance();
                        auto channel = registry.getChannel(channelId);

                        if (!channel)
                        {
                            Sad::Errors::ErrorManager::getInstance().reportError(
                                Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                                Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                                "قناة غير موجودة",
                                "Channel not found");
                        }
                        else
                        {
                            // (AR) التكرار: نستقبل من القناة حتى تُرجع لاشيء (مغلقة وفارغة)
                            // (EN) Iterate: receive from channel until null (closed and empty)
                            bool firstIteration = true;
                            while (true)
                            {
                                Data::Value received = channel->receive();

                                // (AR) إذا القناة مغلقة وفارغة، receive تُرجع Value() (void)
                                // (EN) If channel closed and empty, receive returns Value() (void)
                                if (received.isVoid() && channel->isClosed())
                                {
                                    break;
                                }

                                if (firstIteration)
                                {
                                    variableManager_.define(node.variable, received);
                                    firstIteration = false;
                                }
                                else
                                {
                                    variableManager_.assign(node.variable, received);
                                }

                                // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
                                node.body->accept(*this);

                                // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
                                if (flowControl_ == FlowControl::BREAK)
                                {
                                    flowControl_ = FlowControl::NONE;
                                    break;
                                }
                                if (flowControl_ == FlowControl::CONTINUE)
                                {
                                    flowControl_ = FlowControl::NONE;
                                    continue;
                                }
                                if (flowControl_ == FlowControl::RETURN)
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    Sad::Errors::ErrorManager::getInstance().reportError(
                        Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                        Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                        "نوع غير قابل للتكرار",
                        "Non-iterable type");
                }
            }
            else
            {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                    Sad::Errors::SourceLocation(getSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "نوع غير قابل للتكرار",
                    "Non-iterable type");
            }

            // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
            loopDepth_--;

            // (AR) الخروج من النطاق / (EN) Exit scope
            variableManager_.exitScope();
        }

        // =========================================================================

    } // namespace Interpreter
} // namespace Sad
