// ============================================================================
// (AR) ملف: builtins_processes.cpp
// (EN) File: builtins_processes.cpp
// المكون: Sad::Compiler::SIR
// المسار: compiler/src/frontend/builders/builtins_processes.cpp
// ============================================================================
// (AR) بناءُ تعليماتِ SIR لوحدةِ «عمليات» — نظيرُ المصرِّف لِما ينفّذه المفسّرُ في
//      interpreter/src/builtins/builtin_module_processes.cpp.
//
//      الطبقةُ هنا رقيقةٌ قصدًا: تُطابِقُ الاسمَ القانونيَّ من سجلِّ المدمجاتِ
//      المولَّد، وتُصدِرُ رمزَ عمليّةٍ واحدًا بالمعاملاتِ كما وردت. كلُّ الدلالةِ
//      (جدولُ المقابض، وسمُ النوعِ في البتّاتِ العليا، الأنبوبُ الذاتيّ، فكُّ
//      الحالة) في الخلفيّة: processes_builtins_ops.cpp.
//
//      **الوسيطُ الثالثُ لِـشغل_برنامجا اختياريّ**، وغيابُه يُعرَفُ بعددِ المعاملاتِ
//      لا بقيمةٍ حارسة: حقنُ صفرٍ مكانَه كان سيجعلُ `شغل_برنامجا(م، و، 0)` — وهي
//      خطأُ نوعٍ يرفضُه المفسّرُ — تمرُّ بوصفِها «بلا توجيه»، أي نجاحًا كاذبًا
//      صامتًا يكتبُ خرجَه إلى الشاشةِ بدل الملفّ.
//
// (EN) Builds SIR for the «عمليات» (processes) module — the compiler counterpart of
//      the interpreter's builtin_module_processes.cpp. This layer is deliberately
//      thin: match the canonical name from the generated registry and emit one
//      opcode. All semantics (handle table, high-bit kind tag, self-pipe, status
//      decoding) live in the backend: processes_builtins_ops.cpp.
//      شغل_برنامجا's third argument is optional; its absence is passed as an
//      explicit zero rather than a missing operand, so the backend distinguishes
//      "no redirection" by value instead of by operand count.
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "builtin_registry.h"
#include "builders/builtin_arity_check.h"
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            namespace Bpr = Sad::Builtins::Names::Processes;
            // (AR) رتبةُ المدمجِ من حقلِ `arity` في مصدرِ الحقيقةِ — ثابتٌ مُولَّد.
            namespace Ar = Sad::Builtins::Arity;

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinCallProcesses(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // (AR) دالّةُ المستخدمِ تحجبُ المدمجَ — نفسُ حارسِ سائرِ الوحدات.
                // (EN) A user-defined function shadows the builtin.
                if (isUserDefinedFunction)
                    return std::nullopt;

                // (AR) مساعدٌ محلّيّ: يُصدِرُ التعليمةَ ويُرجعُ سجلَّ النتيجة.
                // (EN) Local helper: emit the instruction and return the result register.
                auto emit = [&](SIROpcode opcode, SadTypeKind returnType,
                                const char *comment) -> BuildResult {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(opcode);
                    inst.result = SIROperand::Register(resultReg, returnType);
                    for (auto &arg : argOperands)
                        inst.operands.push_back(arg);
                    inst.comment = comment;
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    BuildResult res(resultReg, returnType);
                    // (AR) نتيجةُ استدعاءٍ مباشرةٌ لا تحتاجُ LOAD إضافيًّا — كما في
                    //      سائرِ المدمجاتِ التي تُرجِعُ قيمةً في سجلٍّ عابر.
                    // (EN) A direct call result needs no extra LOAD.
                    res.isDirectValue = true;
                    return res;
                };

                // ─────────────────────────────────────────────────────────────
                // (AR) شغل_برنامجا(مسار، وسائط، توجيه؟) → معرّفُ عمليّة (صحيح)
                // (EN) شغل_برنامجا(path, args, redirect?) → process handle (integer)
                // ─────────────────────────────────────────────────────────────
                if (funcName == Bpr::PROC_SPAWN)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Processes::PROC_SPAWN, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return emit(SIROpcode::BUILTIN_PROC_SPAWN, SadTypeKind::Integer,
                                "شغل_برنامجا / fork+execvp");
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) انتظر_عملية(عملية) → رمزُ الخروج (صحيح)
                // ─────────────────────────────────────────────────────────────
                if (funcName == Bpr::PROC_WAIT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Processes::PROC_WAIT, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return emit(SIROpcode::BUILTIN_PROC_WAIT, SadTypeKind::Integer,
                                "انتظر_عملية / waitpid");
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) انبوب() → خريطة {«قراءة»: وصف، «كتابة»: وصف}
                // ─────────────────────────────────────────────────────────────
                if (funcName == Bpr::PROC_PIPE)
                    return emit(SIROpcode::BUILTIN_PROC_PIPE, SadTypeKind::Map,
                                "انبوب / pipe2(O_CLOEXEC)");

                // ─────────────────────────────────────────────────────────────
                // (AR) اغلق(وصف) → صحيح (منطقيّ) — كما يُرجع المفسّرُ حرفيًّا
                // (EN) اغلق(fd) → true, exactly as the interpreter returns
                // ─────────────────────────────────────────────────────────────
                if (funcName == Bpr::PROC_CLOSE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Processes::PROC_CLOSE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return emit(SIROpcode::BUILTIN_PROC_CLOSE, SadTypeKind::Boolean,
                                "اغلق / close");
                }

                // ─────────────────────────────────────────────────────────────
                // (AR) افتح_وصفا(مسار، وضع) → وصف (صحيح)
                // ─────────────────────────────────────────────────────────────
                if (funcName == Bpr::PROC_OPEN_FD)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Processes::PROC_OPEN_FD, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return emit(SIROpcode::BUILTIN_PROC_OPEN_FD, SadTypeKind::Integer,
                                "افتح_وصفا / open(O_CLOEXEC)");
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
