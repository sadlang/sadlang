/**
 * @file builtin_module_exceptions.cpp
 * @brief (AR) وحدة الاستثناءات — أنواع الأخطاء المخصصة ومعالجتها
 * @brief (EN) Exceptions module — custom error types and handling
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "value.h"
#include "builtin_registry.h"
namespace Bexc = Sad::Builtins::Names::Exceptions;

#include <sstream>
#include <stdexcept>
#include <string>

namespace Sad
{
    namespace Interpreter
    {

        void registerBuiltinsExceptions(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (1) استثناء_جديد / exception_new
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    std::string msg = args.empty() ? "خطأ غير محدد" : args[0]->toString();
                    std::string type = args.size() > 1 ? args[1]->toString() : "RuntimeError";
                    std::ostringstream oss;
                    oss << "{\"نوع\": \"" << type << "\", \"رسالة\": \"" << msg << "\"}";
                    return std::make_shared<Data::Value>(oss.str());
                };
                fm.registerBuiltinFunction(std::string(Bexc::EXC_NEW), f); // استثناء_جديد
            }

            // (2) استثناء_رسالة / exception_message
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    if (args.empty())
                        return std::make_shared<Data::Value>(std::string(""));
                    return std::make_shared<Data::Value>(args[0]->toString());
                };
                fm.registerBuiltinFunction(std::string(Bexc::EXC_MESSAGE), f); // استثناء_رسالة
            }

            // (3) مكدس_تتبع / stack_trace (stub)
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    // Stub: return simulated stack trace
                    std::ostringstream oss;
                    oss << "تتبع المكدس (1 إطار):";
                    oss << "\n  [0] <main> (script:1)";
                    return std::make_shared<Data::Value>(oss.str());
                };
                fm.registerBuiltinFunction(std::string(Bexc::STACK_TRACE), f); // مكدس_تتبع
            }

            // (4) مكدس_عمق / stack_depth (stub)
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    // Stub: return 1
                    return std::make_shared<Data::Value>(1.0);
                };
                fm.registerBuiltinFunction(std::string(Bexc::STACK_DEPTH), f); // مكدس_عمق
            }

            // (5) خطأ_نوع / error_type
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    if (args.empty())
                        return std::make_shared<Data::Value>(std::string("غير محدد"));
                    std::string code = args[0]->toString();
                    if (code.find("خارج الحدود") != std::string::npos || code.find("OutOfBounds") != std::string::npos)
                        return std::make_shared<Data::Value>(std::string("خارج_الحدود"));
                    if (code.find("قسمة") != std::string::npos || code.find("Division") != std::string::npos)
                        return std::make_shared<Data::Value>(std::string("قسمة_على_صفر"));
                    if (code.find("فارغ") != std::string::npos || code.find("null") != std::string::npos)
                        return std::make_shared<Data::Value>(std::string("مرجع_فارغ"));
                    if (code.find("نوع") != std::string::npos || code.find("type") != std::string::npos)
                        return std::make_shared<Data::Value>(std::string("خطأ_نوع"));
                    return std::make_shared<Data::Value>(std::string("عام"));
                };
                fm.registerBuiltinFunction(std::string(Bexc::TYPE_ERROR), f); // خطأ_نوع
            }

            // (6) أمان_فحص / sandbox_check
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    std::string permission = args[0]->toString();
                    if (permission == "fs" || permission == "ملفات")
                        return std::make_shared<Data::Value>(true);
                    if (permission == "net" || permission == "شبكة")
                        return std::make_shared<Data::Value>(true);
                    if (permission == "exec" || permission == "تنفيذ")
                        return std::make_shared<Data::Value>(true);
                    if (permission == "env" || permission == "بيئة")
                        return std::make_shared<Data::Value>(true);
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bexc::SECURITY_CHECK), f); // أمان_فحص
            }

            // (7) أمان_قائمة_أذونات / sandbox_permissions
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    std::vector<Data::Value> perms;
                    perms.push_back(Data::Value(std::string("ملفات")));
                    perms.push_back(Data::Value(std::string("شبكة")));
                    perms.push_back(Data::Value(std::string("تنفيذ")));
                    perms.push_back(Data::Value(std::string("بيئة")));
                    perms.push_back(Data::Value(std::string("ذاكرة")));
                    return std::make_shared<Data::Value>(perms);
                };
                fm.registerBuiltinFunction(std::string(Bexc::SECURITY_PERMS), f); // أمان_قائمة_أذونات
            }

            // (8) أمان_وضع / sandbox_mode
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    return std::make_shared<Data::Value>(std::string("تطوير"));
                };
                fm.registerBuiltinFunction(std::string(Bexc::SECURITY_MODE), f); // أمان_وضع
            }

            // (9) خطأ_تأكيد / assert_error
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    if (args.empty())
                        return std::make_shared<Data::Value>(false);
                    bool condition = args[0]->toBool();
                    if (!condition)
                    {
                        std::string msg = args.size() > 1 ? args[1]->toString() : "فشل التأكيد";
                        throw std::runtime_error(msg);
                    }
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bexc::ASSERT_ERROR), f); // خطأ_تأكيد
            }

            // (10) خطأ_آخر / last_error
            {
                // (AR) thread_local لأمان الخيوط / (EN) thread_local for thread safety
                thread_local std::string lastError;
                // (AR) خطأ_آخر — تخزين/استرجاع آخر خطأ / (EN) last_error — store/retrieve last error
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args)
                    -> std::shared_ptr<Data::Value>
                {
                    if (!args.empty())
                    {
                        lastError = args[0]->toString();
                    }
                    return std::make_shared<Data::Value>(lastError);
                };
                fm.registerBuiltinFunction(std::string(Bexc::LAST_ERROR), f); // خطأ_آخر
            }
        }

    } // namespace Interpreter
} // namespace Sad
