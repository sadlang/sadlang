/**
 * @file expression_evaluator_oop_concurrency.cpp
 * @brief (AR) معالجة الطرق على كائنات التزامن (قناة/قفل/مستقبل/مجموعة_انتظار)
 * @brief (EN) Concurrency object method call handling (channel/mutex/future/waitgroup)
 *
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h"
#include "expressions.h"
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "runtime_throw.h"
#include "user_thrown.h"
#include "runtime_throw.h"
#include "channel.h"
#include <vector>

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // =========================================================================
        // (AR) معالجة الطرق على كائنات التزامن
        // (EN) Handle Concurrency Object Method Calls
        // =========================================================================

        bool ExpressionEvaluator::handleConcurrencyMethodCall(MethodCallExpr &node, const Value &objectValue)
        {
            if (!objectValue.isObject())
                return false;
            auto objPtr = objectValue.toObject();
            if (!objPtr)
                return false;
            auto classFieldIt = objPtr->fields.find("__class__");
            bool isChannel = (classFieldIt != objPtr->fields.end() &&
                              classFieldIt->second.toString() == "__\xD9\x82\xD9\x86\xD8\xA7\xD8\xA9__"); // __قناة__
            if (isChannel)
            {
                // (AR) تقييم المعاملات
                std::vector<Value> args;
                for (auto &arg : node.arguments)
                {
                    arg->accept(*this);
                    args.push_back(lastResult_);
                }

                const std::string &m = node.methodName;

                // (AR) الحصول على القناة من السجل
                auto channelIt = objPtr->fields.find("__channel_id__");
                if (channelIt == objPtr->fields.end())
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "corrupt channel object"}});
                }
                size_t channelId = static_cast<size_t>(channelIt->second.toInt());

                // (AR) السجل العام للقنوات — دالة ثابتة لإرجاع مرجع للسجل
                auto &registry = SadChannelRegistry::getInstance();
                auto channel = registry.getChannel(channelId);

                if (!channel)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "channel not found"}});
                }

                // ─── أرسل (send) ───
                if (m == "\xD8\xA3\xD8\xB1\xD8\xB3\xD9\x84" || m == "\xD8\xA7\xD8\xB1\xD8\xB3\xD9\x84")
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "أرسل/send"}, {"argument", "value"}});
                    channel->send(args[0]);
                    lastResult_ = Value(); // void
                    return true;
                }

                // ─── استقبل (receive) ───
                if (m == "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84")
                {
                    lastResult_ = channel->receive();
                    return true;
                }

                // ─── أغلق (close) ───
                if (m == "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82" || m == "\xD8\xA7\xD8\xBA\xD9\x84\xD9\x82")
                {
                    channel->close();
                    lastResult_ = Value(); // void
                    return true;
                }

                // ─── حاول_استقبل (tryReceive) ───
                if (m == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84")
                {
                    auto result = channel->tryReceive();
                    if (result.has_value())
                    {
                        lastResult_ = result.value();
                    }
                    else
                    {
                        lastResult_ = Value(); // لاشيء
                    }
                    return true;
                }

                // ─── حاول_ارسل (trySend) ───
                if (m == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD8\xA7\xD8\xB1\xD8\xB3\xD9\x84")
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "حاول_ارسل/trySend"}, {"argument", "value"}});
                    bool ok = channel->trySend(args[0]);
                    lastResult_ = Value(ok);
                    return true;
                }

                // ─── مغلقة (isClosed) ───
                if (m == "\xD9\x85\xD8\xBA\xD9\x84\xD9\x82\xD8\xA9")
                {
                    lastResult_ = Value(channel->isClosed());
                    return true;
                }

                // ─── الحجم (size) ───
                if (m == "\xD8\xA7\xD9\x84\xD8\xAD\xD8\xAC\xD9\x85" || m == "\xD8\xB7\xD9\x88\xD9\x84")
                {
                    lastResult_ = Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(channel->size(), "expression_evaluator_oop_concurrency_size"));
                    return true;
                }

                // ─── السعة (capacity) ───
                if (m == "\xD8\xA7\xD9\x84\xD8\xB3\xD8\xB9\xD8\xA9")
                {
                    lastResult_ = Value(static_cast<int>(channel->capacity()));
                    return true;
                }

                // ─── فارغة (empty) ───
                if (m == "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA\xD8\xA9")
                {
                    lastResult_ = Value(channel->empty());
                    return true;
                }

                // ─── أرسل_بمهلة (sendTimeout) ───
                if (m == "\xD8\xA3\xD8\xB1\xD8\xB3\xD9\x84_\xD8\xA8\xD9\x85\xD9\x87\xD9\x84\xD8\xA9" || m == "\xD8\xA7\xD8\xB1\xD8\xB3\xD9\x84_\xD8\xA8\xD9\x85\xD9\x87\xD9\x84\xD8\xA9")
                {
                    if (args.size() < 2)
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "أرسل_بمهلة/sendTimeout"}, {"argument", "value, ms"}});
                    int timeoutMs = args[1].toInt();
                    bool ok = channel->sendTimeout(args[0], timeoutMs);
                    lastResult_ = Value(ok);
                    return true;
                }

                // ─── استقبل_بمهلة (receiveTimeout) ───
                if (m == "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84_\xD8\xA8\xD9\x85\xD9\x87\xD9\x84\xD8\xA9")
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "استقبل_بمهلة/receiveTimeout"}, {"argument", "ms"}});
                    int timeoutMs = args[0].toInt();
                    auto result = channel->receiveTimeout(timeoutMs);
                    if (result.has_value())
                    {
                        lastResult_ = result.value();
                    }
                    else
                    {
                        lastResult_ = Value(); // لاشيء — انتهى الوقت
                    }
                    return true;
                }

                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                    node.position,
                    {{"method", m}, {"class", "channel"}});
            }

            // ═══════════════════════════════════════════════════════════
            // (AR) التحقق من كائن مجموعة الانتظار (WaitGroup)
            // (EN) Check for WaitGroup object
            // ═══════════════════════════════════════════════════════════
            auto wgClassIt = objPtr->fields.find("__class__");
            bool isWaitGroup = (wgClassIt != objPtr->fields.end() &&
                                wgClassIt->second.toString() == "__\xD9\x85\xD8\xAC\xD9\x85\xD9\x88\xD8\xB9\xD8\xA9_\xD8\xA7\xD9\x86\xD8\xAA\xD8\xB8\xD8\xA7\xD8\xB1__"); // __مجموعة_انتظار__
            if (isWaitGroup)
            {
                std::vector<Value> args;
                for (auto &arg : node.arguments)
                {
                    arg->accept(*this);
                    args.push_back(lastResult_);
                }
                const std::string &m = node.methodName;

                auto wgIt = objPtr->fields.find("__waitgroup_id__");
                if (wgIt == objPtr->fields.end())
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "corrupt waitgroup object"}});
                }
                size_t wgId = static_cast<size_t>(wgIt->second.toInt());
                auto &wgReg = SadWaitGroupRegistry::getInstance();
                auto wg = wgReg.getWaitGroup(wgId);
                if (!wg)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "waitgroup not found"}});
                }

                // ─── أضف (add) ───
                if (m == "\xD8\xA3\xD8\xB6\xD9\x81" || m == "\xD8\xA7\xD8\xB6\xD9\x81")
                {
                    int delta = 1;
                    if (!args.empty())
                        delta = args[0].toInt();
                    wg->add(delta);
                    lastResult_ = Value();
                    return true;
                }

                // ─── أنهي (done) ───
                if (m == "\xD8\xA3\xD9\x86\xD9\x87\xD9\x8A" || m == "\xD8\xA7\xD9\x86\xD9\x87\xD9\x8A")
                {
                    wg->done();
                    lastResult_ = Value();
                    return true;
                }

                // ─── انتظر (wait) ───
                if (m == "\xD8\xA7\xD9\x86\xD8\xAA\xD8\xB8\xD8\xB1")
                {
                    if (!args.empty())
                    {
                        // (AR) انتظار مع حد زمني / (EN) Wait with timeout
                        int timeoutMs = args[0].toInt();
                        bool ok = wg->waitFor(timeoutMs);
                        lastResult_ = Value(ok);
                    }
                    else
                    {
                        wg->wait();
                        lastResult_ = Value();
                    }
                    return true;
                }

                // ─── العداد (count) ───
                if (m == "\xD8\xA7\xD9\x84\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF")
                {
                    lastResult_ = Value(wg->count());
                    return true;
                }

                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                    node.position,
                    {{"method", m}, {"class", "WaitGroup"}});
            }

            // ═══════════════════════════════════════════════════════════
            // (AR) التحقق من كائن القفل (Mutex)
            // (EN) Check for Mutex object
            // ═══════════════════════════════════════════════════════════
            auto mtxClassIt = objPtr->fields.find("__class__");
            bool isMutex = (mtxClassIt != objPtr->fields.end() &&
                            mtxClassIt->second.toString() == "__\xD9\x82\xD9\x81\xD9\x84__"); // __قفل__
            if (isMutex)
            {
                std::vector<Value> args;
                for (auto &arg : node.arguments)
                {
                    arg->accept(*this);
                    args.push_back(lastResult_);
                }
                const std::string &m = node.methodName;

                auto mtxIt = objPtr->fields.find("__mutex_id__");
                if (mtxIt == objPtr->fields.end())
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "corrupt mutex object"}});
                }
                size_t mtxId = static_cast<size_t>(mtxIt->second.toInt());
                auto &mtxReg = SadMutexRegistry::getInstance();
                auto mtx = mtxReg.getMutex(mtxId);
                if (!mtx)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "mutex not found"}});
                }

                // ─── اقفل (lock) ───
                if (m == "\xD8\xA7\xD9\x82\xD9\x81\xD9\x84")
                {
                    mtx->lock();
                    lastResult_ = Value();
                    return true;
                }

                // ─── افتح (unlock) ───
                if (m == "\xD8\xA7\xD9\x81\xD8\xAA\xD8\xAD")
                {
                    mtx->unlock();
                    lastResult_ = Value();
                    return true;
                }

                // ─── حاول_قفل (tryLock) ───
                if (m == "\xD8\xAD\xD8\xA7\xD9\x88\xD9\x84_\xD9\x82\xD9\x81\xD9\x84")
                {
                    lastResult_ = Value(mtx->tryLock());
                    return true;
                }

                // ─── مقفل (isLocked) ───
                if (m == "\xD9\x85\xD9\x82\xD9\x81\xD9\x84")
                {
                    lastResult_ = Value(mtx->isLocked());
                    return true;
                }

                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                    node.position,
                    {{"method", m}, {"class", "Mutex"}});
            }

            // ═══════════════════════════════════════════════════════════
            // (AR) التحقق من كائن المستقبل (Future)
            // (EN) Check for Future object
            // ═══════════════════════════════════════════════════════════
            auto futClassIt = objPtr->fields.find("__class__");
            bool isFuture = (futClassIt != objPtr->fields.end() &&
                             futClassIt->second.toString() == "__\xD9\x85\xD8\xB3\xD8\xAA\xD9\x82\xD8\xA8\xD9\x84__"); // __مستقبل__
            if (isFuture)
            {
                std::vector<Value> args;
                for (auto &arg : node.arguments)
                {
                    arg->accept(*this);
                    args.push_back(lastResult_);
                }
                const std::string &m = node.methodName;

                auto futIt = objPtr->fields.find("__future_id__");
                if (futIt == objPtr->fields.end())
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "corrupt future object"}});
                }
                size_t futId = static_cast<size_t>(futIt->second.toInt());
                auto &futReg = SadFutureRegistry::getInstance();
                auto fut = futReg.getFuture(futId);
                if (!fut)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CHANNEL_OPERATION_FAILED,
                        node.position,
                        {{"operation", "lookup"}, {"reason", "future not found"}});
                }

                // ─── احصل (get) — يحجب حتى تجهز النتيجة ───
                if (m == "\xD8\xA7\xD8\xAD\xD8\xB5\xD9\x84")
                {
                    if (!args.empty())
                    {
                        // (AR) احصل مع حد زمني / (EN) Get with timeout
                        int timeoutMs = args[0].toInt();
                        auto result = fut->getResultFor(timeoutMs);
                        if (result.has_value())
                        {
                            lastResult_ = result.value();
                        }
                        else
                        {
                            lastResult_ = Value(); // لاشيء — انتهى الوقت
                        }
                    }
                    else
                    {
                        // (AR) await() يحجب حتى تجهز النتيجة ثم يُرجعها
                        // (EN) await() blocks until the result is ready then returns it
                        lastResult_ = fut->await();
                    }
                    return true;
                }

                // ─── جاهز (isReady) ───
                if (m == "\xD8\xAC\xD8\xA7\xD9\x87\xD8\xB2")
                {
                    lastResult_ = Value(fut->isReady());
                    return true;
                }

                // ─── عيّن (set / setResult) — تعيين النتيجة ───
                if (m == "\xD8\xB9\xD9\x8A\xD9\x91\xD9\x86" || m == "\xD8\xB9\xD9\x8A\xD9\x86")
                {
                    if (args.empty())
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_MISSING_REQUIRED_ARG,
                            node.position,
                            {{"function", "عيّن/set"}, {"argument", "value"}});
                    fut->setResult(args[0]);
                    lastResult_ = Value(); // void
                    return true;
                }

                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                    node.position,
                    {{"method", m}, {"class", "Future"}});
            }

            return false;
        }

    } // namespace Interpreter
} // namespace Sad
