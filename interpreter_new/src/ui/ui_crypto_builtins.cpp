/**
 * @file ui_crypto_builtins.cpp
 * @brief (AR) نظام التشفير + قاعدة البيانات
 * @brief (EN) Crypto + database system
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

void registerUICryptoBuiltins(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══ التشفير ═══

    // شفر_نص(نص, مفتاح) → نص مشفر
    auto encrypt_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->encryptText(args[0]->toString(), args[1]->toString()));
    };
    fm.registerBuiltinFunction("\xd8\xb4\xd9\x81\xd8\xb1_\xd9\x86\xd8\xb5", encrypt_fn);
    fm.registerBuiltinFunction("encryptText", encrypt_fn);

    // فك_تشفير(نص, مفتاح) → نص أصلي
    auto decrypt_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.size() < 2) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->decryptText(args[0]->toString(), args[1]->toString()));
    };
    fm.registerBuiltinFunction("\xd9\x81\xd9\x83_\xd8\xaa\xd8\xb4\xd9\x81\xd9\x8a\xd8\xb1", decrypt_fn);
    fm.registerBuiltinFunction("decryptText", decrypt_fn);

    // هاش(نص, [خوارزمية]) → نص
    auto hash_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(std::string(""));
        std::string algo = args.size() > 1 ? args[1]->toString() : "sha256";
        return std::make_shared<Data::Value>(bridge->hashText(args[0]->toString(), algo));
    };
    fm.registerBuiltinFunction("\xd9\x87\xd8\xa7\xd8\xb4", hash_fn);
    fm.registerBuiltinFunction("hashText", hash_fn);

    // ═══ قاعدة البيانات ═══

    // افتح_قاعدة(اسم) → منطقي
    auto db_open_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(false);
        return std::make_shared<Data::Value>(bridge->dbOpen(args[0]->toString()));
    };
    fm.registerBuiltinFunction("\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x82\xd8\xa7\xd8\xb9\xd8\xaf\xd8\xa9", db_open_fn);
    fm.registerBuiltinFunction("dbOpen", db_open_fn);

    // نفذ_استعلام(أمر) → نص
    auto db_exec_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        auto* bridge = UIBridge::active();
        if (!bridge || args.empty()) return std::make_shared<Data::Value>(std::string(""));
        return std::make_shared<Data::Value>(bridge->dbExecute(args[0]->toString()));
    };
    fm.registerBuiltinFunction("\xd9\x86\xd9\x81\xd8\xb0_\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xb9\xd9\x84\xd8\xa7\xd9\x85", db_exec_fn);
    fm.registerBuiltinFunction("dbExecute", db_exec_fn);

    // أغلق_قاعدة()
    auto db_close_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto* bridge = UIBridge::active();
        if (bridge) bridge->dbClose();
        return std::make_shared<Data::Value>();
    };
    fm.registerBuiltinFunction("\xd8\xa3\xd8\xba\xd9\x84\xd9\x82_\xd9\x82\xd8\xa7\xd8\xb9\xd8\xaf\xd8\xa9", db_close_fn);
    fm.registerBuiltinFunction("dbClose", db_close_fn);
}

} // namespace Interpreter
} // namespace Sad
