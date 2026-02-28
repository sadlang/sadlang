/**
 * @file builtin_registry_part38.cpp
 * @brief (AR) وحدة الاستثناءات والأمان — محاكاة ذاتية
 * @brief (EN) Exception & sandbox module — self-contained stub simulation
 */

#include "interpreter_core.h"
#include "value.h"

#include <sstream>
#include <string>
#include <stdexcept>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart38(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // (1) استثناء_جديد / exception_new
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            std::string msg = args.empty() ? "خطأ غير محدد" : args[0]->toString();
            std::string type = args.size() > 1 ? args[1]->toString() : "RuntimeError";
            std::ostringstream oss;
            oss << "{\"نوع\": \"" << type << "\", \"رسالة\": \"" << msg << "\"}";
            return std::make_shared<Data::Value>(oss.str());
        };
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf", f); // استثناء_جديد
        fm.registerBuiltinFunction("exception_new", f);
    }

    // (2) استثناء_رسالة / exception_message
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(std::string(""));
            return std::make_shared<Data::Value>(args[0]->toString());
        };
        fm.registerBuiltinFunction("\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xab\xd9\x86\xd8\xa7\xd8\xa1_\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9", f); // استثناء_رسالة
        fm.registerBuiltinFunction("exception_message", f);
    }

    // (3) مكدس_تتبع / stack_trace (stub)
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            // Stub: return simulated stack trace
            std::ostringstream oss;
            oss << "تتبع المكدس (1 إطار):";
            oss << "\n  [0] <main> (script:1)";
            return std::make_shared<Data::Value>(oss.str());
        };
        fm.registerBuiltinFunction("\xd9\x85\xd9\x83\xd8\xaf\xd8\xb3_\xd8\xaa\xd8\xaa\xd8\xa8\xd8\xb9", f); // مكدس_تتبع
        fm.registerBuiltinFunction("stack_trace", f);
    }

    // (4) مكدس_عمق / stack_depth (stub)
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            // Stub: return 1
            return std::make_shared<Data::Value>(1.0);
        };
        fm.registerBuiltinFunction("\xd9\x85\xd9\x83\xd8\xaf\xd8\xb3_\xd8\xb9\xd9\x85\xd9\x82", f); // مكدس_عمق
        fm.registerBuiltinFunction("stack_depth", f);
    }

    // (5) خطأ_نوع / error_type
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(std::string("غير محدد"));
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
        fm.registerBuiltinFunction("\xd8\xae\xd8\xb7\xd8\xa3_\xd9\x86\xd9\x88\xd8\xb9", f); // خطأ_نوع
        fm.registerBuiltinFunction("error_type", f);
    }

    // (6) أمان_فحص / sandbox_check
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            std::string permission = args[0]->toString();
            if (permission == "fs" || permission == "ملفات") return std::make_shared<Data::Value>(true);
            if (permission == "net" || permission == "شبكة") return std::make_shared<Data::Value>(true);
            if (permission == "exec" || permission == "تنفيذ") return std::make_shared<Data::Value>(true);
            if (permission == "env" || permission == "بيئة") return std::make_shared<Data::Value>(true);
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xa3\xd9\x85\xd8\xa7\xd9\x86_\xd9\x81\xd8\xad\xd8\xb5", f); // أمان_فحص
        fm.registerBuiltinFunction("sandbox_check", f);
    }

    // (7) أمان_قائمة_أذونات / sandbox_permissions
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            std::vector<Data::Value> perms;
            perms.push_back(Data::Value(std::string("ملفات")));
            perms.push_back(Data::Value(std::string("شبكة")));
            perms.push_back(Data::Value(std::string("تنفيذ")));
            perms.push_back(Data::Value(std::string("بيئة")));
            perms.push_back(Data::Value(std::string("ذاكرة")));
            return std::make_shared<Data::Value>(perms);
        };
        fm.registerBuiltinFunction("\xd8\xa3\xd9\x85\xd8\xa7\xd9\x86_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xa3\xd8\xb0\xd9\x88\xd9\x86\xd8\xa7\xd8\xaa", f); // أمان_قائمة_أذونات
        fm.registerBuiltinFunction("sandbox_permissions", f);
    }

    // (8) أمان_وضع / sandbox_mode
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            return std::make_shared<Data::Value>(std::string("تطوير"));
        };
        fm.registerBuiltinFunction("\xd8\xa3\xd9\x85\xd8\xa7\xd9\x86_\xd9\x88\xd8\xb6\xd8\xb9", f); // أمان_وضع
        fm.registerBuiltinFunction("sandbox_mode", f);
    }

    // (9) خطأ_تأكيد / assert_error
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (args.empty()) return std::make_shared<Data::Value>(false);
            bool condition = args[0]->toBool();
            if (!condition) {
                std::string msg = args.size() > 1 ? args[1]->toString() : "فشل التأكيد";
                throw std::runtime_error(msg);
            }
            return std::make_shared<Data::Value>(true);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd8\xb7\xd8\xa3_\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf", f); // خطأ_تأكيد
        fm.registerBuiltinFunction("assert_error", f);
    }

    // (10) خطأ_آخر / last_error
    {
        static std::string lastError;
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args)
            -> std::shared_ptr<Data::Value> {
            if (!args.empty()) {
                lastError = args[0]->toString();
            }
            return std::make_shared<Data::Value>(lastError);
        };
        fm.registerBuiltinFunction("\xd8\xae\xd8\xb7\xd8\xa3_\xd8\xa2\xd8\xae\xd8\xb1", f); // خطأ_آخر
        fm.registerBuiltinFunction("last_error", f);
    }
}

} // namespace Interpreter
} // namespace Sad
