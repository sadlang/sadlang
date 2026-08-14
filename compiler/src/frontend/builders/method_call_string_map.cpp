// ============================================================================
// sir_builder_method_call_string_map.cpp
// ============================================================================
// (AR) طرق النصوص والخرائط المضمنة
// (EN) String and map built-in methods
// ============================================================================

#include <string>
#include <optional>
#include "sir_builder.h"
#include "builders/method_call_builder.h"
#include "sir_constants.h" // (AR) أسماءُ زمنِ تشغيلِ الخريطة — عقدٌ مشترَكٌ مع الخلفيّة
// (AR) ثوابت أسماء طرق الأنواع المُولَّدة
#include "builtin_registry.h"

namespace TM = Sad::Builtins::Names::TypeMethods;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::optional<BuildResult> MethodCallBuilder::buildStringBuiltinMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                if (methodName == TM::String::SPLIT)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SPLIT);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    BuildResult result(resultReg, SadTypeKind::Array);
                    result.elementType = SadTypeKind::String;
                    return result;
                }

                // (AR) استبدل / replace — استبدال نص فرعي
                // (EN) replace — replace substring
                if (methodName == TM::String::REPLACE)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_REPLACE);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]); // (AR) النص القديم
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]); // (AR) النص الجديد
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) يبدأ_بـ / starts_with — فحص بادئة النص
                // (EN) starts_with — check if string starts with prefix
                if (methodName == TM::String::STARTS_WITH)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_STARTS_WITH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) ينتهي_بـ / ends_with — فحص خاتمة النص
                // (EN) ends_with — check if string ends with suffix
                if (methodName == TM::String::ENDS_WITH)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_ENDS_WITH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) قص / trim — إزالة الفراغات من الطرفين
                // (EN) trim — remove whitespace from both ends
                if (methodName == TM::String::TRIM)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_TRIM);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) جزء / substring — استخراج جزء من النص
                // (EN) substring — extract part of string
                if (methodName == TM::String::SUBSTRING)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_SUBSTRING);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]); // (AR) فهرس البداية
                    if (args.size() > 2)
                        inst.operands.push_back(args[2]); // (AR) الطول (اختياري)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::String);
                }

                // (AR) طول / length — عدد محارف النص
                // (EN) length — number of characters in the string
                if (methodName == TM::String::LENGTH)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_STRING_LENGTH);
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, SadTypeKind::String));
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ════════════════════════════════════════════════════════════════════
                // (AR) طرقُ نصٍّ كانت مُعلَنةً في SoT وغيرَ مبنيّةٍ هنا، فكان الموزّعُ
                //      يمرّرها لبُناة المصفوفة (تصادمُ الأسماء) فتُبنى على مؤشّر النصّ
                //      ⇒ انهيارٌ صامت. تُبنى الآن نداءً مباشرًا لدوالّ زمن التشغيل
                //      المُضمَّن:
                //        حرف_عند/عكس — واعيتان بـUTF-8 (لا تفتّتان الحرف العربيّ)
                //        كرر — تكرارٌ بسيط؛ عددٌ غيرُ موجبٍ ⇒ نصٌّ فارغ (كالمفسّر)
                //      ⚠ فارقٌ معروفٌ عن المفسّر: «حرف_عند» خارجَ المدى تُرجع نصًّا فارغًا
                //        هنا، بينما يرمي المفسّر RUN_STRING_INDEX_OUT_OF_RANGE. وكذلك
                //        الوسيطُ المفقود: المفسّر يرمي RUN_MISSING_REQUIRED_ARG، والمترجم
                //        يرفض البناءَ أدناه. توحيدُ الرمي في المترجم بندٌ قائم.
                //      ⚠ «تحويل_كبير»/«تحويل_صغير» ليستا هنا: لهما أوپكودان قائمان
                //        (BUILTIN_STRING_TO_UPPER/TO_LOWER) يستعملان sad_llvm_str_upper/
                //        lower في زمن التشغيل نفسِه. وكانت نسختان جديدتان قد كُتبتا فحُذفتا
                //        تفاديًا لازدواج تنفيذٍ يفترق صامتًا. و«بحث» تركت للأوپكود القائم
                //        لأنّه يُرجع فهرسَ محرفٍ لا بايت — انظر التنبيه أدناه.
                // (EN) String methods declared in SoT but never built here, so the
                //      dispatcher handed them to the array builders (name collision) ⇒
                //      silent crash. Now emitted as direct calls to the embedded runtime.
                //      Known divergences from the interpreter are flagged above; to_upper/
                //      to_lower keep their existing opcodes rather than duplicating runtime
                //      implementations that would silently drift apart.
                // ════════════════════════════════════════════════════════════════════
                {
                    // (AR) أسماءُ رموز زمن التشغيل — ثوابتُ مسمّاةٌ لا سلاسلُ مباشرة
                    // (EN) Runtime symbol names — named constants, not raw literals
                    static constexpr const char *kRuntimeUtf8CharAt = "sad_llvm_string_utf8_char_at";
                    static constexpr const char *kRuntimeStringReverse = "sad_llvm_string_reverse";
                    static constexpr const char *kRuntimeStringRepeat = "sad_llvm_string_repeat";

                    // (AR) طرقٌ لها أوپكودٌ قائمٌ في الخلفيّة — تُوجَّه إليه بدل كتابة نسخةٍ
                    //      ثانيةٍ في زمن التشغيل تفترق عنه صامتًا. كانت غيرَ مبنيّةٍ هنا
                    //      فيمرّرها الموزّعُ لبُناة المصفوفة (تصادمُ الأسماء).
                    //      ⚠ «بحث» عبر هذا الأوپكود تُرجع فهرسَ **محرف** UTF-8، بينما
                    //        المفسّرُ يُرجع فهرسَ **بايت** (std::string::find). فارقٌ يظهر
                    //        في النصوص غيرِ اللاتينيّة فقط، مُسجَّلٌ للتوحيد في بندٍ مستقلّ.
                    // (EN) Methods with an existing backend opcode are routed to it rather than
                    //      duplicated in the runtime, where the two copies would silently drift.
                    //      NOTE: find via this opcode returns a UTF-8 CHARACTER index while the
                    //      interpreter returns a BYTE index; divergence tracked separately.
                    std::optional<SIROpcode> stringOpcode;
                    SadTypeKind opcodeResultKind = SadTypeKind::String;
                    bool opcodeTakesArgument = false;

                    if (methodName == TM::String::TO_UPPER)
                    {
                        stringOpcode = SIROpcode::BUILTIN_STRING_TO_UPPER;
                    }
                    else if (methodName == TM::String::TO_LOWER)
                    {
                        stringOpcode = SIROpcode::BUILTIN_STRING_TO_LOWER;
                    }
                    else if (methodName == TM::String::FIND)
                    {
                        stringOpcode = SIROpcode::BUILTIN_STRING_FIND;
                        opcodeResultKind = SadTypeKind::Integer;
                        opcodeTakesArgument = true;
                    }

                    if (stringOpcode.has_value())
                    {
                        if (opcodeTakesArgument && args.size() <= 1)
                            return std::nullopt;

                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction inst(*stringOpcode);
                        inst.result = SIROperand::Register(resultReg, opcodeResultKind);
                        inst.operands.push_back(
                            SIROperand::Register(objResult.registerName, SadTypeKind::String));
                        if (opcodeTakesArgument)
                            inst.operands.push_back(args[1]);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(resultReg, opcodeResultKind);
                    }

                    const char *runtimeFn = nullptr;
                    bool takesArgument = false;

                    if (methodName == TM::String::CHAR_AT)
                    {
                        runtimeFn = kRuntimeUtf8CharAt;
                        takesArgument = true;
                    }
                    else if (methodName == TM::String::REVERSE)
                    {
                        runtimeFn = kRuntimeStringReverse;
                    }
                    else if (methodName == TM::String::REPEAT)
                    {
                        runtimeFn = kRuntimeStringRepeat;
                        takesArgument = true;
                    }

                    if (runtimeFn)
                    {
                        // (AR) وسيطٌ مفقودٌ ⇒ لا نبني نداءً ناقصَ الأرقام: emitCall يستنتج
                        //      التوقيعَ من الوسائط الفعليّة **ويخبّئه**، فتصريحُ (ptr)->ptr
                        //      لدالّةِ C ذاتِ معاملَين يُنتج قراءةَ سجلٍّ من قمامةٍ (سلوكٌ غيرُ
                        //      معرَّف)، ويُفسد كلَّ نداءٍ لاحقٍ للرمز نفسِه.
                        // (EN) Missing argument ⇒ do not emit an under-supplied call: emitCall
                        //      infers the signature from the actual arguments AND caches it, so a
                        //      (ptr)->ptr declaration for a two-parameter C function reads a
                        //      garbage register (undefined behaviour) and poisons every later
                        //      call to the same symbol.
                        if (takesArgument && args.size() <= 1)
                            return std::nullopt;

                        std::string resultReg = b_.newTempRegister();
                        SIRInstruction callInst(SIROpcode::CALL);
                        callInst.result = SIROperand::Register(resultReg, SadTypeKind::String);
                        callInst.operands.push_back(SIROperand::ConstantString(runtimeFn));
                        callInst.operands.push_back(
                            SIROperand::Register(objResult.registerName, SadTypeKind::String));
                        if (takesArgument)
                            callInst.operands.push_back(args[1]);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(callInst);
                        return BuildResult(resultReg, SadTypeKind::String);
                    }
                }

                return std::nullopt;
            }

            // === buildMapBuiltinMethodCall ===
            std::optional<BuildResult> MethodCallBuilder::buildMapBuiltinMethodCall(
                const BuildResult &objResult, const std::string &methodName,
                const std::vector<SIROperand> &args)
            {
                // (AR) والمُستقبِلُ الموسومُ زمنَ التشغيل («أي») يدخلُ كذلك — لأسماءٍ
                //      لا تحتملُ عائلةً أخرى.
                //
                //      `حلل("{...}")` تُرجعُ قيمةً نوعُها الساكنُ «أي» وحمولتُها خريطةٌ
                //      موسومة. وكان الحارسُ يردُّها هنا، فتسقطُ إلى بوّابةِ الرمزِ في
                //      call_method_dispatch وتُفشِلُ البناءَ بـ«الطريقة احصل غير موجودة
                //      في الصنف أي» — بينما المفسّرُ ينفّذها. والخلفيّةُ جاهزةٌ أصلًا:
                //      `normalizeMapPtr` تفكُّ `%SadDyn` بعد فحصِ الوسمِ وتفشلُ صاخبةً
                //      إن لم يكن خريطة، وهو الحارسُ نفسُه الذي تعتمدُه الفهرسةُ
                //      `م["أ"]` على «أي» منذ [م-٠٠١].
                //
                //      والقصرُ على الأسماءِ المفردةِ مقصود: `احصل`/`عيّن`/`مفاتيح`/`قيم`
                //      لا معنى لها في المصفوفةِ ولا في النصّ، فتوجيهُها
                //      قطعيّ. أمّا `يحتوي`/`حجم`/`فارغة` فمشتركةٌ بين العائلاتِ الثلاث،
                //      وتوجيهُها بالنوعِ الساكنِ وحدَه يخطئُ نصفَ الحالات — فتلك تحتاج
                //      تفريعًا على الوسمِ زمنَ التشغيل، ولم يُبنَ بعد. حدٌّ مُعلَنٌ لا مسكوتٌ
                //      عنه: `أي.يحتوي` ما زال يذهبُ إلى مسارِ المصفوفةِ فيفشلُ صاخبًا
                //      على خريطة.
                // (EN) A runtime-tagged receiver («أي») is admitted too — for names that
                //      cannot belong to another family.
                //
                //      `parse("{...}")` returns a value whose static type is «أي» carrying a
                //      tagged map. The guard used to reject it here; it then fell to the symbol
                //      gate in call_method_dispatch and failed the build with "method احصل not
                //      found on class أي" — while the interpreter runs it. The backend is
                //      already prepared: `normalizeMapPtr` unpacks a `%SadDyn` after checking
                //      the tag and fails loudly when it is not a map — the very guard that
                //      indexing `m["a"]` on «أي» has relied on since card م-٠٠١.
                //
                //      Restricting this to unambiguous names is deliberate: get/set/delete/
                //      keys/values mean nothing on an array or a string, so the routing is
                //      certain. `contains`/`size`/`empty` are shared across all three families,
                //      and routing them by static type alone is wrong half the time — those
                //      need a runtime tag branch, which does not exist yet. A declared, not
                //      silent, limit: `any.contains` still goes down the array path and fails
                //      loudly on a map.
                // ⚠️ (AR) و`احذف` ليست منها وإن بدت كذلك: مصدرُ الحقيقةِ يعرّف
                //      `Array::DELETE` بالاسمِ نفسِه `احذف`، فهي مشتركةٌ لا مفردة.
                //      وكانت مُدرَجةً هنا في أوّلِ صياغةٍ لهذه البوّابة، **فقِيسَ الأثر**:
                //      `حلل("[1,2,3]").احذف(0)` كان يُردّ بتشخيصٍ زمنَ الترجمة، فصار
                //      يُبنى `__sad_map_delete` ثمّ يموتُ زمنَ التشغيلِ بـ«طريقةُ خريطةٍ
                //      طُبِّقت على قيمةٍ ليست خريطة» بينما يطبع المفسّر `2` — أي أنّ
                //      الإدراجَ حوّل خطأَ ترجمةٍ إلى إجهاضِ تشغيل، وسدَّ الطريقَ أمام
                //      تنفيذِ `احذف` للمصفوفاتِ الموسومةِ لاحقًا. فأُخرِجَت.
                //      (والمصفوفةُ لا مُعالِجَ لـ`احذف` في الواجهةِ أصلًا، فالسلوكُ
                //       الراجعُ هو التشخيصُ الساكنُ لا فقدانَ ميزةٍ كانت تعمل.)
                // (EN) `delete` is NOT one of them despite appearances: the source of truth
                //      defines `Array::DELETE` under the same Arabic name, so it is shared, not
                //      unambiguous. It was listed here in the first draft of this gate, and the
                //      effect was measured: `parse("[1,2,3]").delete(0)` used to be rejected at
                //      compile time and instead started emitting `__sad_map_delete`, dying at
                //      runtime with "a map method applied to a non-map value" while the
                //      interpreter prints `2` — the listing turned a compile error into a runtime
                //      abort and blocked any later array implementation. So it was removed.
                const bool receiverIsRuntimeTagged = (objResult.type == SadTypeKind::Any);
                const bool nameIsMapOnly =
                    (methodName == TM::Map::GET || methodName == TM::Map::SET ||
                     methodName == TM::Map::KEYS || methodName == TM::Map::VALUES);

                if (objResult.type != SadTypeKind::Map &&
                    !(receiverIsRuntimeTagged && nameIsMapOnly))
                    return std::nullopt;

                // ================================================================
                // (AR) [م-٠٠١] احصل / get — قراءة قيمة من الخريطة بالمفتاح.
                //      كانت تُرجعُ التمثيلَ النصّيَّ دائمًا عبر `__sad_map_get`، فاسمان
                //      في مصدرِ الحقيقةِ للعمليّةِ نفسِها بسلوكَين مختلفَين:
                //      `خريطة_احصل` تُرجعُ «صحيح» و`.احصل()` تُرجعُ «1»؛ والمفتاحُ
                //      الغائبُ نصٌّ فارغٌ هنا و«لاشيء» هناك. وُحّدا على القراءةِ
                //      الموسومةِ زمنَ التشغيل — الاسمان واجهتان لعقدٍ واحد.
                // (EN) [card م-٠٠١] get — read a value from the map by key. It used to always
                //      return the string form via `__sad_map_get`, so two names in the SoT for
                //      the same operation behaved differently: `خريطة_احصل` answered «صحيح»
                //      where `.احصل()` answered «1», and an absent key was an empty string here
                //      but «لاشيء» there. Both now take the runtime-tagged read — two spellings
                //      of one contract.
                // ================================================================
                if (methodName == TM::Map::GET)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Any);
                    inst.operands.push_back(SIROperand::ConstantString(kRuntimeMapGetDyn));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    inst.comment = "map get (tagged)";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    BuildResult result(resultReg, SadTypeKind::Any);
                    result.isDirectValue = true;
                    return result;
                }

                // ================================================================
                // (AR) عيّن / set — تعيين قيمة في الخريطة
                //      [م-٠٠١] كان هذا الموضعُ يكتبُ سُلَّمَ الأوسامِ يدويًّا بأرقامٍ عاريةٍ
                //      واسمَ زمنِ التشغيلِ سلسلةً خامّة، فلمّا أُضيف وسمُ العدمِ أُغفِلَ هنا
                //      وحدَه: `خ.عين("ك"، لاشيء)` تُخزّنُ مؤشّرَ عدمٍ بوسمِ النصّ فيُقرأُ
                //      `char*` ⇒ SIGSEGV. صار الاشتقاقُ واحدًا مشترَكًا لا يُنسى طرفٌ منه.
                // (EN) set — [card م-٠٠١] this site open-coded the tag ladder with bare numbers
                //      and the runtime name as a raw literal, so when the null tag was added it
                //      was missed here alone: `خ.عين("ك"، لاشيء)` stored a null pointer under
                //      the string tag and it was read as `char*` ⇒ SIGSEGV. The derivation is now
                //      shared, so no writer can be left behind.
                // ================================================================
                if (methodName == TM::Map::SET)
                {
                    if (args.size() < 3)
                        return BuildResult("", SadTypeKind::Integer);

                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    std::string resultReg = b_.newTempRegister();
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::ConstantString(kRuntimeMapSetTyped));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.operands.push_back(args[1]); // (AR) المفتاح / key
                    inst.operands.push_back(args[2]); // (AR) القيمة / value
                    inst.operands.push_back(
                        SIROperand::ConstantI64(mapValueTagFor(args[2].dataType)));
                    inst.comment = "map set typed";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ================================================================
                // (AR) احذف / delete — حذف عنصر من الخريطة بالمفتاح
                //      args: [0]=self(map), [1]=key(string)
                // (EN) delete — remove entry from map by key
                // ================================================================
                if (methodName == TM::Map::DELETE)
                {
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    std::string resultReg = b_.newTempRegister();
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_delete"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    inst.comment = "map delete";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) مفاتيح / keys — إرجاع مصفوفة مفاتيح الخريطة
                // (EN) keys — return array of map keys
                if (methodName == TM::Map::KEYS)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_keys"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map keys";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    BuildResult res(resultReg, SadTypeKind::Array);
                    res.elementType = SadTypeKind::String;
                    return res;
                }

                // (AR) قيم / values — إرجاع مصفوفة قيم الخريطة
                // (EN) values — return array of map values
                if (methodName == TM::Map::VALUES)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_values"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map values";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    BuildResult res(resultReg, SadTypeKind::Array);
                    res.elementType = objResult.elementType;
                    return res;
                }

                // (AR) حجم / size — عدد عناصر الخريطة
                // (EN) size — number of map entries
                if (methodName == TM::Map::SIZE)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_size"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    inst.comment = "map size";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) فارغة / empty — هل الخريطة فارغة
                // (EN) empty — is map empty
                if (methodName == TM::Map::IS_EMPTY)
                {
                    // (AR) حجم → ثم مقارنة == 0 عبر SIR EQ
                    std::string sizeReg = b_.newTempRegister();
                    SIRInstruction sizeInst;
                    sizeInst.opcode = SIROpcode::CALL;
                    sizeInst.result = SIROperand::Register(sizeReg, SadTypeKind::Integer);
                    sizeInst.operands.push_back(SIROperand::ConstantString("__sad_map_size"));
                    sizeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    sizeInst.comment = "map size for empty check";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(sizeInst);

                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction cmpInst(SIROpcode::EQ);
                    cmpInst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(sizeReg, SadTypeKind::Integer));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(cmpInst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // (AR) يحتوي / contains / has — فحص وجود مفتاح
                // (EN) contains / has — check if key exists
                if (methodName == TM::Map::CONTAINS)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst;
                    inst.opcode = SIROpcode::CALL;
                    inst.result = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    inst.operands.push_back(SIROperand::ConstantString("__sad_map_has"));
                    inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    if (args.size() > 1)
                        inst.operands.push_back(args[1]);
                    inst.comment = "map has key";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
