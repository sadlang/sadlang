/**
 * @file ui_network_builtins.cpp
 * @brief (AR) نظام الشبكة + JSON
 * @brief (EN) Network + JSON system
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include "builtin_registry.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

        void registerUINetworkBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت الشبكة
            namespace Bnn = Builtins::Names::UINetwork;

            // ═══ الشبكة ═══

            // طلب_شبكة(طريقة, رابط, [جسم], [ترويسات]) → نص
            auto http_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(std::string(""));
                std::string method = args[0]->toString();
                std::string url = args[1]->toString();
                std::string body = args.size() > 2 ? args[2]->toString() : "";
                std::string headers = args.size() > 3 ? args[3]->toString() : "";
                return std::make_shared<Data::Value>(bridge->httpRequest(method, url, body, headers));
            };
            fm.registerBuiltinFunction(std::string(Bnn::HTTP_REQUEST), http_fn);

            // هل_متصل() → منطقي
            auto online_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->isOnline());
            };
            fm.registerBuiltinFunction(std::string(Bnn::IS_ONLINE), online_fn);

            // ═══ JSON ═══

            // حلل_جيسون(نص, مفتاح) → نص
            auto json_parse_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->jsonParse(args[0]->toString(), args[1]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bnn::JSON_PARSE), json_parse_fn);

            // صدر_جيسون(مفتاح1, قيمة1, ...) → نص JSON
            auto json_stringify_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge)
                    return std::make_shared<Data::Value>(std::string("{}"));
                std::vector<std::pair<std::string, std::string>> pairs;
                for (size_t i = 0; i + 1 < args.size(); i += 2)
                {
                    pairs.push_back({args[i]->toString(), args[i + 1]->toString()});
                }
                return std::make_shared<Data::Value>(bridge->jsonStringify(pairs));
            };
            fm.registerBuiltinFunction(std::string(Bnn::JSON_STRINGIFY), json_stringify_fn);
        }

    } // namespace Interpreter
} // namespace Sad
