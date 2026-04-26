/**
 * @file ui_crypto_builtins.cpp
 * @brief (AR) نظام التشفير + قاعدة البيانات
 * @brief (EN) Crypto + database system
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

        void registerUICryptoBuiltins(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // (AR) اختصار لأسماء ثوابت التشفير
            namespace Bcc = Builtins::Names::UICrypto;

            // ═══ التشفير ═══

            // شفر_نص(نص, مفتاح) → نص مشفر
            auto encrypt_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->encryptText(args[0]->toString(), args[1]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bcc::ENCRYPT), encrypt_fn);

            // فك_تشفير(نص, مفتاح) → نص أصلي
            auto decrypt_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.size() < 2)
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->decryptText(args[0]->toString(), args[1]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bcc::DECRYPT), decrypt_fn);

            // هاش(نص, [خوارزمية]) → نص
            auto hash_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string(""));
                std::string algo = args.size() > 1 ? args[1]->toString() : "sha256";
                return std::make_shared<Data::Value>(bridge->hashText(args[0]->toString(), algo));
            };
            fm.registerBuiltinFunction(std::string(Bcc::HASH_FN), hash_fn);

            // ═══ قاعدة البيانات ═══

            // افتح_قاعدة(اسم) → منطقي
            auto db_open_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(false);
                return std::make_shared<Data::Value>(bridge->dbOpen(args[0]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bcc::DB_OPEN), db_open_fn);

            // نفذ_استعلام(أمر) → نص
            auto db_exec_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                auto *bridge = UIBridge::active();
                if (!bridge || args.empty())
                    return std::make_shared<Data::Value>(std::string(""));
                return std::make_shared<Data::Value>(bridge->dbExecute(args[0]->toString()));
            };
            fm.registerBuiltinFunction(std::string(Bcc::DB_EXEC), db_exec_fn);

            // أغلق_قاعدة()
            auto db_close_fn = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
            {
                (void)args;
                auto *bridge = UIBridge::active();
                if (bridge)
                    bridge->dbClose();
                return std::make_shared<Data::Value>();
            };
            fm.registerBuiltinFunction(std::string(Bcc::DB_CLOSE), db_close_fn);
        }

    } // namespace Interpreter
} // namespace Sad
