/**
 * @file interpreter_test_stubs.cpp
 * @brief Stub for registerBuiltinFunctions to avoid pulling the entire stdlib
 *
 * The real builtin_registry.cpp depends on graphics, filesystem, networking,
 * etc. For interpreter tests we only need the core interpreter logic.
 *
 * (AR) ملف بدائل لتجنب ربط المكتبة القياسية الكاملة في اختبارات المفسر
 * (EN) Stub file to avoid linking the full standard library in interpreter tests
 */

#include "interpreter_core.h"
#include "stdlib_manager.h"
#include "builtin_module_registry.h"

namespace Sad
{
    namespace Interpreter
    {

        void registerBuiltinFunctions(Interpreter & /*interpreter*/)
        {
            // No-op stub for tests - builtins not needed for core interpreter testing
            // (AR) بديل فارغ — الدوال المضمنة غير مطلوبة لاختبار نواة المفسر
        }

    } // namespace Interpreter
} // namespace Sad

// Stubs for StandardLibraryManager (real impl in stdlib_manager.cpp)
namespace Sad
{
    namespace StdLib
    {

        StandardLibraryManager::StandardLibraryManager(Sad::Data::FunctionManager &fm)
            : functionManager_(fm) {}

        StandardLibraryManager::~StandardLibraryManager() {}

        bool StandardLibraryManager::registerAllFunctions()
        {
            // (AR) نُرجع true بدون تسجيل أي دالة — المفسر لا يحتاجها لاختبار النواة
            // (EN) Return true without registering anything — interpreter doesn't need stdlib for core tests
            return true;
        }

        bool StandardLibraryManager::registerPhase1_IOFunctions() { return true; }
        bool StandardLibraryManager::registerPhase2_StringFunctions() { return true; }
        bool StandardLibraryManager::registerPhase3_ArrayFunctions() { return true; }
        bool StandardLibraryManager::registerPhase4_MathFunctions() { return true; }
        bool StandardLibraryManager::registerPhase5_TypeFunctions() { return true; }

        std::string StandardLibraryManager::getRegistrationStatus() const
        {
            return "stub — no functions registered";
        }

    } // namespace StdLib
} // namespace Sad

// Stubs for BuiltinModuleRegistry::loadModule (real impl in builtin_registry.cpp)
namespace Sad
{
    namespace Interpreter
    {

        bool BuiltinModuleRegistry::loadModule(const std::string & /*name*/) { return false; }

    } // namespace Interpreter
} // namespace Sad

// Stub for suggestModuleForUnimportedFunction (real impl in builtin_registry.cpp)
// (AR) بديل لدالة اقتراح الوحدات — غير مطلوبة للاختبارات
namespace Sad
{
    namespace Interpreter
    {
        std::string suggestModuleForUnimportedFunction(const std::string & /*funcName*/)
        {
            return "";
        }
    } // namespace Interpreter
} // namespace Sad

// Stub for IOFunctions::setObjectToStringCallback (real impl in io_functions.cpp)
// (AR) بديل لـ callback تحويل الكائنات — غير مطلوب لاختبارات النواة
// (EN) Stub for object-to-string callback — not needed for core tests
#include "../../stdlib/io/io_functions.h"
namespace Sad
{
    namespace StdLib
    {
        namespace IO
        {
            IOFunctions::ObjectToStringCallback IOFunctions::objectToStringCallback_ = nullptr;
            void IOFunctions::setObjectToStringCallback(ObjectToStringCallback callback)
            {
                objectToStringCallback_ = std::move(callback);
            }
        } // namespace IO
    } // namespace StdLib
} // namespace Sad

// ======================================================================
// Stub for ExpressionEvaluator::visitUIWidgetExpr
// (AR) بديل لدالة تقييم عنصر الواجهة — غير مطلوبة لاختبارات النواة
// (EN) Stub for UI widget expression visitor — not needed for core tests
// ======================================================================
#include "expression_evaluator.h"
#include "ui_nodes.h"

namespace Sad
{
    namespace Interpreter
    {
        void ExpressionEvaluator::visitUIWidgetExpr(AST::UIWidgetExprNode & /*node*/)
        {
            // No-op stub — UI widgets not tested in core interpreter tests
        }
    } // namespace Interpreter
} // namespace Sad

// ======================================================================
// Stubs for UIStateManager methods
// (AR) بدائل لمدير الحالة التفاعلية — غير مطلوب لاختبارات المفسر الأساسية
// (EN) Stubs for UIStateManager — not needed for basic interpreter tests
// ======================================================================
#include "ui_state_manager.h"

namespace Sad
{
    namespace Interpreter
    {
        void UIStateManager::registerComponent(
            std::shared_ptr<Data::ObjectInstance> /*obj*/,
            const std::string & /*componentName*/,
            const std::unordered_set<std::string> & /*stateFields*/)
        {
            // No-op stub
        }

        void UIStateManager::unregisterComponent(Data::ObjectInstance * /*obj*/)
        {
            // No-op stub
        }

        void UIStateManager::onStateFieldChanged(
            Data::ObjectInstance * /*obj*/,
            const std::string & /*fieldName*/,
            const Data::Value & /*oldValue*/,
            const Data::Value & /*newValue*/)
        {
            // No-op stub
        }

        void UIStateManager::scheduleRebuild()
        {
            // No-op stub
        }

        void UIStateManager::flush()
        {
            // No-op stub
        }

        void UIStateManager::setRebuildCallback(std::function<void()> /*callback*/)
        {
            // No-op stub
        }

        void UIStateManager::reset()
        {
            // No-op stub
        }

        size_t UIStateManager::componentCount() const
        {
            return 0;
        }

        bool UIStateManager::isRegistered(Data::ObjectInstance * /*obj*/) const
        {
            return false;
        }

    } // namespace Interpreter
} // namespace Sad
