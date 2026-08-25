// ============================================================================
// sir_builder_builtins_async.cpp
// (AR) Async/Await builtins: spawn, join, channel, select, APIC
// (EN) Async/Await & Concurrency Functions (23+ functions)
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>

#include "builtin_registry.h"
#include "error_manager.h"
#include "builders/builtin_arity_check.h"
// (AR) الأسماءُ من مصدرِ الحقيقةِ بدل بايتاتٍ مهرَّبة، والرتبةُ من حقلِ `arity`
//      بدل رقمٍ في الشرطِ وتشخيصٍ يذهبُ إلى `std::cerr` لا يراه أحد.
namespace Bn = Sad::Builtins::Names;
namespace Ar = Sad::Builtins::Arity;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_Async(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ========================================================================
                // (AR) Async/Await - 23 functions
                // (EN) Async/Await & Concurrency - 23 functions
                // ========================================================================

                // 1. spawn - async task
                if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9" || funcName == "spawn" || funcName == "async_spawn")
                {
                                        // (AR) رتبةٌ مفتوحةُ الأعلى (وسائطُ المهمّةِ تتبعُ اسمَها) — العقدُ الآن في مصدرِ الحقيقةِ
                    //      بـ`variadic: true`، والأدنى يُقرَأ من الثابتِ المُولَّدِ لا من رقمٍ
                    //      يُكتَب. والحدُّ الأعلى قائمٌ شكلًا (UNBOUNDED) فلا يمنعُ نداءً صحيحًا.
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::AsyncAdvanced::ASYNC_SPAWN, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    // (AR) بوابة الأنواع المصرحة — نظيرة بوابة «أطلق» حرفا:
                    //      كانت هذه المدمجة تمرر الوسائط إلى ASYNC_SPAWN بلا
                    //      فحص فتعيد فئة انحدار 099 من الباب الخلفي (معامل
                    //      افتراضي النوع يعلب i64 فيطبع المؤشر رقما) — رصدته
                    //      المراجعة. غير المؤهل ينفذ متزامنا بنداء CALL عادي
                    //      (سلوك «أطلق» الاحتياطي عينه)، لا إسقاطا صامتا.
                    // (EN) Declared-types gate — the exact «أطلق» gate: this
                    //      builtin used to forward args into ASYNC_SPAWN
                    //      unchecked, reintroducing the 099 regression class
                    //      through the back door. Ineligible calls run
                    //      synchronously via a plain CALL (same fallback as
                    //      «أطلق»), never a silent drop.
                    //      (وملاحظة أميليا: فحص المولد يشمل عديمة الوسائط أيضا،
                    //      والسقوط المتزامن يعيد قيمة الدالة لا مقبض خيط —
                    //      مأمون لأن «انتظر» تشترط انتزاعا ناجحا من السجل.)
                    bool spawnEligible = true;
                    if (argOperands.size() == 1 &&
                        argOperands[0].type == SIROperandType::FUNCTION)
                    {
                        // (AR) دالة بلا وسائط: المولدات تستثنى كبوابة «أطلق»
                        auto ftIt = b_.functionTable_.find(argOperands[0].name);
                        if (ftIt != b_.functionTable_.end() && ftIt->second.isGenerator)
                            spawnEligible = false;
                    }
                    else if (argOperands.size() > 1 &&
                             argOperands[0].type == SIROperandType::FUNCTION)
                    {
                        auto ftIt = b_.functionTable_.find(argOperands[0].name);
                        spawnEligible =
                            ftIt != b_.functionTable_.end() &&
                            !ftIt->second.isGenerator &&
                            ftIt->second.parameters.size() == argOperands.size() - 1 &&
                            ftIt->second.paramDefaulted.size() ==
                                ftIt->second.parameters.size();
                        if (spawnEligible)
                        {
                            for (size_t pi = 0; pi < ftIt->second.parameters.size(); ++pi)
                            {
                                const auto &param = ftIt->second.parameters[pi];
                                if (ftIt->second.paramDefaulted[pi] ||
                                    (param.type != SadTypeKind::Integer &&
                                     param.type != SadTypeKind::Float &&
                                     param.type != SadTypeKind::Boolean &&
                                     param.type != SadTypeKind::String &&
                                     param.type != SadTypeKind::Pointer))
                                {
                                    spawnEligible = false;
                                    break;
                                }
                            }
                        }
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(spawnEligible ? SIROpcode::ASYNC_SPAWN
                                                      : SIROpcode::CALL);
                    inst.result = resultOp;
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] async spawn() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 2. await - wait for future/task
                if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd9\x85\xd9\x87\xd9\x85\xd8\xa9" || funcName == "await" || funcName == "async_await" || funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::ASYNC_AWAIT_TASK,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_AWAIT);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] async await() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 3. yield
                if (funcName == "\xd8\xa3\xd9\x86\xd8\xaa\xd8\xac" || funcName == "yield" || funcName == "async_yield")
                {
                    SIRInstruction inst(SIROpcode::ASYNC_YIELD);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] async yield()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 4. async_sleep - non-blocking sleep
                if (funcName == "\xd9\x86\xd9\x88\xd9\x85_\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86" || funcName == "async_sleep" || funcName == "sleep_async")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::ASYNC_SLEEP,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::ASYNC_SLEEP);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] async_sleep()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 5. create_future
                if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "create_future" || funcName == "\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84")
                {
                    std::string resultReg = b_.newTempRegister();
                    // (AR) [S-TS-P4] نوع النتيجة Future (لا Integer) كي يُرجع نوع(مستقبل())=«مستقبل»
                    //      مطابقةً للمفسّر. التمثيل يبقى i64 (mapSIRType(Future)→i64: معرّف
                    //      المستقبل عدد)، وإرسال الطرق يعتمد className="__future__" مستقلًّا.
                    // (EN) [S-TS-P4] Tag result as Future (not Integer) so typeof=«مستقبل»,
                    //      matching the interpreter. Representation stays i64 (future-id), and
                    //      method dispatch keys on className="__future__" independently.
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Future);
                    SIRInstruction inst(SIROpcode::ASYNC_CREATE_FUTURE);
                    inst.result = resultOp;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] create_future() -> " << resultReg << std::endl;
#endif
                    // (AR) [Fix BF-04] تعليم النتيجة بأنها مستقبل لدعم dot-syntax
                    // (EN) [Fix BF-04] Mark result as future for dot-syntax method dispatch
                    BuildResult res(resultReg, SadTypeKind::Future);
                    res.className = "__future__";
                    return res;
                }

                // 6. resolve_future
                if (funcName == Bn::AsyncAdvanced::FUTURE_OFF || funcName == "resolve_future" || funcName == "\xd8\xad\xd9\x82\xd9\x82_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::AsyncAdvanced::FUTURE_OFF, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::ASYNC_RESOLVE_FUTURE);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] resolve_future()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 7. get_future
                if (funcName == "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x85\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84" || funcName == "get_future")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::FUTURE_GET,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    // (AR) العائدُ «أي» لا «رقم» — الحمولةُ موسومةٌ زمنَ التشغيل.
                    //      السببُ الكاملُ عند `buildFutureMethodCall` في
                    //      `method_call_concurrency.cpp`؛ النظيران يتحرّكان معًا.
                    // (EN) Result is «any», not «int» — the payload is runtime-tagged.
                    //      Full rationale at buildFutureMethodCall in
                    //      method_call_concurrency.cpp; the two twins move together.
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Any);
                    SIRInstruction inst(SIROpcode::ASYNC_GET_FUTURE);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] get_future() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Any);
                }

                // 8. create_channel
                if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "create_channel" || funcName == "\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_CREATE_CHANNEL);
                    inst.result = resultOp;
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] create_channel() -> " << resultReg << std::endl;
#endif
                    // (AR) نُعلّم النتيجة بأنها قناة لدعم تكرار لكل في
                    // (EN) Mark result as channel to support for-each iteration
                    BuildResult res(resultReg, SadTypeKind::Integer);
                    res.className = "__channel__";
                    return res;
                }

                // 9. channel_send
                if (funcName == Bn::AsyncAdvanced::CHANNEL_SEND || funcName == "channel_send")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::AsyncAdvanced::CHANNEL_SEND, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_SEND);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(inst);
                    }
#ifndef NDEBUG
                    std::cout << "[DEBUG] channel_send()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 10. channel_recv
                if (funcName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_recv" || funcName == "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x82\xd8\xa8\xd9\x84")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::CHANNEL_RECV,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_RECV);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] channel_recv() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 11. channel_close
                if (funcName == "\xd8\xa3\xd8\xba\xd9\x84\xd9\x82_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "channel_close")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::CHANNEL_CLOSE,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::ASYNC_CHANNEL_CLOSE);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] channel_close()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 12. create_mutex
                if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd9\x82\xd9\x81\xd9\x84" || funcName == "create_mutex" || funcName == "\xd9\x82\xd9\x81\xd9\x84")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_MUTEX_CREATE);
                    inst.result = resultOp;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] create_mutex() -> " << resultReg << std::endl;
#endif
                    // (AR) [Fix BF-04] تعليم النتيجة بأنها قفل لدعم dot-syntax
                    // (EN) [Fix BF-04] Mark result as mutex for dot-syntax method dispatch
                    BuildResult res(resultReg, SadTypeKind::Integer);
                    res.className = "__mutex__";
                    return res;
                }

                // 13. mutex_lock
                if (funcName == "\xd8\xa7\xd9\x82\xd9\x81\xd9\x84" || funcName == "mutex_lock" || funcName == "lock")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::MUTEX_LOCK,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::ASYNC_MUTEX_LOCK);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] mutex_lock()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 14. mutex_unlock
                if (funcName == "\xd8\xa7\xd9\x81\xd8\xaa\xd8\xad_\xd9\x82\xd9\x81\xd9\x84" || funcName == "mutex_unlock" || funcName == "unlock")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::MUTEX_UNLOCK,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::ASYNC_MUTEX_UNLOCK);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] mutex_unlock()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 15. thread_spawn
                if (funcName == "\xd8\xa3\xd9\x86\xd8\xb4\xd8\xa6_\xd8\xae\xd9\x8a\xd8\xb7" || funcName == "thread_spawn" || funcName == "\xd8\xae\xd9\x8a\xd8\xb7")
                {
                                        // (AR) رتبةٌ مفتوحةُ الأعلى (وسائطُ الخيطِ تتبعُ اسمَه) — العقدُ الآن في مصدرِ الحقيقةِ
                    //      بـ`variadic: true`، والأدنى يُقرَأ من الثابتِ المُولَّدِ لا من رقمٍ
                    //      يُكتَب. والحدُّ الأعلى قائمٌ شكلًا (UNBOUNDED) فلا يمنعُ نداءً صحيحًا.
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::AsyncAdvanced::CREATE_THREAD, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_THREAD_SPAWN);
                    inst.result = resultOp;
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] thread_spawn() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 16. thread_join
                if (funcName == "\xd8\xa7\xd9\x86\xd8\xb6\xd9\x85_\xd8\xae\xd9\x8a\xd8\xb7" || funcName == "thread_join")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::THREAD_JOIN,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_THREAD_JOIN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] thread_join() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 17. atomic_load
                if (funcName == "\xd8\xad\xd9\x85\xd9\x84_\xd8\xb0\xd8\xb1\xd9\x8a" || funcName == "atomic_load")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::AsyncAdvanced::ATOMIC_LOAD,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_LOAD);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] atomic_load() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 18. atomic_store
                if (funcName == Bn::AsyncAdvanced::ATOMIC_STORE || funcName == "atomic_store")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::AsyncAdvanced::ATOMIC_STORE, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_STORE);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] atomic_store()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // 19. atomic_add
                if (funcName == Bn::AsyncAdvanced::ATOMIC_ADD || funcName == "atomic_add")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::AsyncAdvanced::ATOMIC_ADD, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_ADD);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] atomic_add() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 20. compare_and_swap / CAS
                if (funcName == Bn::AsyncAdvanced::ATOMIC_CAS || funcName == "compare_and_swap" || funcName == "cas")
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::AsyncAdvanced::ATOMIC_CAS, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_ATOMIC_CAS);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    inst.operands.push_back(argOperands[2]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] compare_and_swap() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 21. wait_all
                if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x83\xd9\x84" || funcName == "wait_all")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_WAIT_ALL);
                    inst.result = resultOp;
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] wait_all() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 22. wait_any
                if (funcName == "\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1_\xd8\xa3\xd9\x8a" || funcName == "wait_any")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_WAIT_ANY);
                    inst.result = resultOp;
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] wait_any() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // 23. select / channel_select
                if (funcName == "\xd8\xa7\xd8\xae\xd8\xaa\xd8\xb1_\xd9\x82\xd9\x86\xd8\xa7\xd8\xa9" || funcName == "select" || funcName == "channel_select")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_SELECT);
                    inst.result = resultOp;
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] select() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ========================================================================
                // 24. مجموعة_انتظار / waitgroup_create
                // (AR) إنشاء مجموعة انتظار — handle بـ malloc(8) يحوي عداد i64
                // (EN) Create waitgroup — malloc(8) handle holding i64 counter
                // ========================================================================
                if (funcName == "\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9\xd8\xa9_\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xa7\xd8\xb1" || funcName == "waitgroup_create")
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::ASYNC_WG_CREATE);
                    inst.result = resultOp;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] waitgroup_create() -> " << resultReg << std::endl;
#endif
                    BuildResult res(resultReg, SadTypeKind::Integer);
                    res.className = "__waitgroup__";
                    return res;
                }

                // ========================================================================
                // (AR) تفويض إلى الدوال الفرعية لوحدات نظام التشغيل و UEFI
                // (EN) Delegate to OS modules and UEFI sub-functions
                // ========================================================================
                auto osCoreResult = buildBuiltinSystem_OsCore(funcName, isUserDefinedFunction, argResults, argOperands);
                if (osCoreResult.has_value())
                    return osCoreResult;

                auto osHwResult = buildBuiltinSystem_OsHardware(funcName, isUserDefinedFunction, argResults, argOperands);
                if (osHwResult.has_value())
                    return osHwResult;

                auto osSysResult = buildBuiltinSystem_OsSystem(funcName, isUserDefinedFunction, argResults, argOperands);
                if (osSysResult.has_value())
                    return osSysResult;

                auto uefiResult = buildBuiltinSystem_Uefi(funcName, isUserDefinedFunction, argResults, argOperands);
                if (uefiResult.has_value())
                    return uefiResult;

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
