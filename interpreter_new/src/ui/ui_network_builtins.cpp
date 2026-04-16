/**
 * @file ui_network_builtins.cpp
 * @brief (AR) نظام الشبكة + JSON
 * @brief (EN) Network + JSON system
 */

#include "ui_module_builtins.h"
#include "ui_bridge.h"
#include "builtins.h"
#include "interpreter_core.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerUINetworkBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══ الشبكة ═══

    // طلب_شبكة(طريقة, رابط, [جسم], [ترويسات]) → نص
    auto http_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>(std::string(""));
        std::string method = args[0]->toString();
        std::string url = args[1]->toString();
        std::string body = args.size() > 2 ? args[2]->toString() : "";
        std::string headers = args.size() > 3 ? args[3]->toString() : "";
        return std::make_shared<Data::Value>(bridge->httpRequest(method, url, body, headers));
    };
    fm.registerBuiltinFunction("\xd8\xb7\xd9\x84\xd8\xa8_\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9", http_fn);
    fm.registerBuiltinFunction("httpRequest", http_fn);

    // هل_متصل() → منطقي
    auto online_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(bridge->isOnline());
    };
    fm.registerBuiltinFunction("\xd9\x87\xd9\x84_\xd9\x85\xd8\xaa\xd8\xb5\xd9\x84", online_fn);
    fm.registerBuiltinFunction("isOnline", online_fn);

    // ═══ JSON ═══

    // حلل_جيسون(نص, مفتاح) → نص
    auto json_parse_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->jsonParse(args[0]->toString(), args[1]->toString()));
    };
    fm.registerBuiltinFunction("\xd8\xad\xd9\x84\xd9\x84_\xd8\xac\xd9\x8a\xd8\xb3\xd9\x88\xd9\x86", json_parse_fn);
    fm.registerBuiltinFunction("jsonParse", json_parse_fn);

    // صدر_جيسون(مفتاح1, قيمة1, ...) → نص JSON
    auto json_stringify_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge) return std::make_shared<Data::Value>(std::string("{}"));
        std::vector<std::pair<std::string, std::string>> pairs;
        for (size_t i = 0; i + 1 < args.size(); i += 2) {
            pairs.push_back({args[i]->toString(), args[i+1]->toString()});
        }
        return std::make_shared<Data::Value>(bridge->jsonStringify(pairs));
    };
    fm.registerBuiltinFunction("\xd8\xb5\xd8\xaf\xd8\xb1_\xd8\xac\xd9\x8a\xd8\xb3\xd9\x88\xd9\x86", json_stringify_fn);
    fm.registerBuiltinFunction("jsonStringify", json_stringify_fn);
}

} // namespace Interpreter
} // namespace Sad
