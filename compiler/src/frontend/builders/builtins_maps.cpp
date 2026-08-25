// ============================================================================
// builtins_maps.cpp — الواجهةُ المسمّاةُ للخرائطِ ونصِّها / Maps named interface
// ============================================================================
// (AR) البطاقة م-٠٠١ (ق١ + ق٢): الدوالُّ المسمّاةُ لوحدةِ «خرائط» كانت معلَنةً في
//      مصدرِ الحقيقة (language-truth/builtins/maps.yaml) ومسجَّلةً في المفسّر،
//      بلا أيِّ توزيعٍ في المترجم ⇒ «استدعاءُ دالّةٍ غيرِ معرَّفة».
//      وليس العطبُ في الخرائطِ نفسِها: الخلفيّةُ (map_ops.cpp) تُنفّذ أصلًا
//      __sad_map_size/has/delete/keys/values/get/set_typed. فالفجوةُ في **حدِّ
//      النداءِ** وحدَه — وهو ما تُغلقه هذه الوحدة.
//
//      الصيغةُ بالأقواسِ (`م[ك]` و`م[ك] = ق`) كانت تعمل، فظُنَّ أنّ «الخرائطَ
//      تعمل». ما كان يعملُ جزءٌ واحدٌ منها لا غير.
//
// (EN) Card م-٠٠١ (ق١ + ق٢): the Maps module's named functions were declared in
//      the source of truth and registered in the interpreter, with no compiler
//      dispatch at all ⇒ "call to undefined function".
//      The maps themselves were never at fault: the backend already implements
//      __sad_map_size/has/delete/keys/values/get/set_typed. The gap was purely
//      at the **call boundary** — which this unit closes.
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_constants.h"
#include "sad_type_utils.h" // (AR) kindToArabic لرسالة حارس المستقبِل / (EN) Arabic kind name for the receiver guard

#include <optional>
#include <string>
#include <vector>

#include "builtin_registry.h"
#include "builders/builtin_arity_check.h"
namespace Bn = Sad::Builtins::Names;

// (AR) رتبةُ المدمجِ من حقلِ `arity` في مصدرِ الحقيقةِ — ثابتٌ مُولَّدٌ لا رقمٌ
//      يُكتَب. وكانت ههنا أربعةُ ثوابتَ محلّيّةٍ (`kArityMapOnly` وأخواتُها)
//      تحملُ العددَ نسخةً ثانيةً لا يقيسُها أحد.
namespace Ar = Sad::Builtins::Arity;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            namespace
            {
                // (AR) مواضعُ الوسائطِ في نداءِ الواجهةِ المسمّاة.
                // (EN) Argument positions in a named-interface call.
                constexpr size_t kArgMap = 0;
                constexpr size_t kArgKey = 1;
                constexpr size_t kArgValue = 2;
                constexpr size_t kArgDefault = 2; // (AR) الوسيطُ الثالثُ في `خريطة_احصل`
                constexpr size_t kArgText = 0;
            } // namespace

            /**
             * @brief (AR) توزيعُ الواجهةِ المسمّاةِ لوحدةِ «خرائط» على زمنِ التشغيل
             * @brief (EN) Dispatch the Maps module's named interface onto the runtime
             *
             * (AR) يُرجعُ nullopt إن لم يكنِ الاسمُ من هذه الوحدة، فيتابعُ المتصلُ بحثَه.
             * (EN) Returns nullopt when the name is not ours, so the caller keeps looking.
             */
            std::optional<BuildResult> BuiltinBuilder::buildBuiltinMapsCall(
                const std::string &funcName,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // (AR) مساعدٌ يبني نداءَ زمنِ تشغيلٍ بسيطًا ويُسجّلُ نتيجتَه بنوعٍ معلوم.
                // (EN) Helper building a plain runtime call and registering a typed result.
                auto emitRuntimeCall = [&](const char *runtimeName,
                                           const std::vector<size_t> &argIndices,
                                           SadTypeKind resultType,
                                           const char *comment) -> BuildResult
                {
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::CALL);
                    inst.result = SIROperand::Register(resultReg, resultType);
                    inst.operands.push_back(SIROperand::ConstantString(runtimeName));
                    for (size_t index : argIndices)
                        inst.operands.push_back(argOperands[index]);
                    inst.comment = comment;
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);
                    return BuildResult(resultReg, resultType);
                };

                // ════════════════════════════════════════════════════════════
                // (AR) حارسُ المستقبِل — الدَّينُ الموروث «مستقبِلٌ غيرُ خريطةٍ يُسقط
                //      الثنائيَّ المترجَم»: القيمةُ الموسومةُ (%SadDyn) تحرسها الخلفيّةُ
                //      أصلًا (normalizeMapPtr يفحص وسمَ Map)، أمّا i64 الخامُ فلا وسمَ
                //      له فيُحوَّل عنوانًا ويُفَكُّ ⇒ SIGSEGV. ما **يثبُت سكونيًّا** أنّه
                //      ليس خريطةً يُرفَض هنا رفضَ ترجمةٍ (نظيرُ سابقةِ الوسيطِ الثالث
                //      في الجلب المصنَّف: المفسّرُ يرفض زمنَ التشغيل RUN037 والمترجمُ
                //      زمنَ الترجمة).
                //
                //      🔑 والحدُّ مقصودٌ ضيّقًا اتّقاءَ الإيجابيِّ الكاذب (درسُ D1):
                //      سجلُّ Integer **غيرُ الثابت** يبقى مسموحًا — معاملُ دالّةٍ بلا
                //      تصنيفٍ نوعُه الافتراضيُّ Integer وقد يحمل خريطةً حقيقيّةً
                //      (ptrtoint)، فرفضُه يقتل برامجَ سليمة. فيبقى المستقبِلُ العدديُّ
                //      المتغيّرُ غيرَ المحروسِ في المسارِ المترجَم حدًّا مُعلَنًا
                //      (التمثيلُ الخامُ بلا وسمٍ يُفحَص).
                // (EN) Receiver guard — the inherited "non-map receiver segfaults the
                //      compiled binary" debt: tagged values are already guarded by the
                //      backend (normalizeMapPtr checks the Map tag); a raw i64 has no tag,
                //      gets int-to-ptr'd and dereferenced ⇒ SIGSEGV. Whatever is PROVABLY
                //      not a map statically is rejected here at compile time (same
                //      precedent as the typed-fetch third argument: interpreter rejects at
                //      runtime with RUN037, the compiler at compile time).
                //      Deliberately narrow to avoid false positives (the D1 lesson): a
                //      NON-constant Integer register stays allowed — an untyped parameter
                //      defaults to Integer and may hold a real (ptrtoint'd) map. The
                //      variable numeric receiver remains a declared, unguardable limit of
                //      the untagged representation.
                // ════════════════════════════════════════════════════════════
                // (AR) ⚠️ هذا الباني يُستدعى لكلِّ اسمِ نداءٍ مرشَّحٍ ويُرجع nullopt
                //      لغير أسماء الوحدة في **آخره** — فالحارسُ يُقيَّد بأسماء
                //      مدمجات الخرائط ذات المستقبِل حصرًا، وإلّا رفض نداءَ أيِّ
                //      اسمٍ آخرَ يمرّ به (قِيس: ٨٥ فشلَ ترجمةٍ في المسح الكامل —
                //      نداءُ معاملِ لامدا «دالة_» رُفض «ليس خريطةً»).
                // (EN) ⚠️ This builder is called for EVERY candidate call name and
                //      only returns nullopt for non-Maps names at its END — so the
                //      guard must be gated on the map-receiver builtin names
                //      explicitly, else it rejects any other call passing through
                //      (measured: 85 compile failures — a lambda-parameter call
                //      «دالة_» rejected as "not a map").
                const bool takesMapReceiver =
                    funcName == Bn::Maps::MAP_SIZE ||
                    funcName == Bn::Maps::MAP_HAS_KEY ||
                    funcName == Bn::Maps::MAP_DELETE ||
                    funcName == Bn::Maps::MAP_KEYS ||
                    funcName == Bn::Maps::MAP_VALUES ||
                    funcName == Bn::Maps::MAP_GET ||
                    funcName == Bn::Maps::MAP_FETCH_STR ||
                    funcName == Bn::Maps::MAP_FETCH_NUM ||
                    funcName == Bn::Maps::MAP_FETCH_BOOL ||
                    funcName == Bn::Maps::MAP_SET;
                if (takesMapReceiver && !argResults.empty())
                {
                    const BuildResult &receiver = argResults[kArgMap];
                    // (AR) العشريُّ والمنطقيُّ والنصّيُّ يرفضهما `shapedNonMap` أدناه
                    //      ثابتَين ومتغيّرَين — فلا يُكرَّران هنا.
                    // (EN) Float/Boolean/String are rejected by shapedNonMap below,
                    //      constant or not — no need to repeat them here.
                    const bool literalNonMap =
                        receiver.isConstant &&
                        (receiver.type == SadTypeKind::Integer ||
                         receiver.type == SadTypeKind::Null);
                    // (AR) الأنواعُ التي لا يسكنُها مؤشّرُ خريطةٍ ولو عبر سجلّ: العشريُّ
                    //      (تمثيله double)، والمنطقيُّ (بِتّ)، والنصُّ (مؤشّرُ محتوًى نصّيّ)،
                    //      والفراغُ (لا قيمةَ أصلًا).
                    // (EN) Kinds a map pointer can never inhabit, register or not: Float
                    //      (double repr), Boolean (a bit), String (text-content pointer),
                    //      and Void (no value at all).
                    const bool shapedNonMap =
                        receiver.type == SadTypeKind::Float ||
                        receiver.type == SadTypeKind::Boolean ||
                        receiver.type == SadTypeKind::String ||
                        receiver.type == SadTypeKind::Void;
                    if (literalNonMap || shapedNonMap)
                    {
                        b_.errors_.push_back(
                            std::string("Error: ") + funcName +
                            " استقبلت وسيطًا أوّل ليس خريطةً (نوعُه '" +
                            Sad::Types::kindToArabic(receiver.type) +
                            "') — مدمجاتُ الخرائطِ تتطلّب خريطةً مستقبِلًا");
                        return BuildResult();
                    }
                }

                // ────────────────────────────────────────────────────────────
                // (AR) خريطة_حجم(م) — عددُ الأزواج
                // (EN) خريطة_حجم(m) — pair count
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::MAP_SIZE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Maps::MAP_SIZE, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    BuildResult result = emitRuntimeCall(
                        kRuntimeMapSize, {kArgMap}, SadTypeKind::Integer, "maps: size");
                    result.isDirectValue = true;
                    return result;
                }

                // ────────────────────────────────────────────────────────────
                // (AR) خريطة_تحتوي(م، ك) — هل المفتاحُ موجود؟
                // (EN) خريطة_تحتوي(m, k) — does the key exist?
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::MAP_HAS_KEY)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Maps::MAP_HAS_KEY, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    BuildResult result = emitRuntimeCall(
                        kRuntimeMapHas, {kArgMap, kArgKey}, SadTypeKind::Boolean, "maps: has key");
                    result.isDirectValue = true;
                    return result;
                }

                // (AR) نسخٌ سطحيٌّ للخريطة — تمهيدًا لتعديلٍ نقيٍّ لا يمسُّ الأصل.
                // (EN) Shallow map copy — a prelude to a pure edit that leaves the
                //      original untouched.
                auto emitMapCopy = [&]() -> SIROperand
                {
                    std::string copyReg = b_.newTempRegister();
                    SIRInstruction copyInst(SIROpcode::CALL);
                    copyInst.result = SIROperand::Register(copyReg, SadTypeKind::Map);
                    copyInst.operands.push_back(SIROperand::ConstantString(kRuntimeMapCopy));
                    copyInst.operands.push_back(argOperands[kArgMap]);
                    copyInst.comment = "maps: shallow copy (pure edit)";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(copyInst);
                    return SIROperand::Register(copyReg, SadTypeKind::Map);
                };

                // ────────────────────────────────────────────────────────────
                // (AR) خريطة_احذف(م، ك) — حذفُ مفتاحٍ **نقيًّا**: يُرجعُ خريطةً جديدةً
                //      والأصلُ سالمٌ، مطابقةً للمفسّر. الصيغةُ بالأقواسِ تُعدّلُ في
                //      مكانِها؛ الاختلافُ عقدٌ مقصودٌ لا تفاوتُ تنفيذ.
                // (EN) خريطة_احذف(m, k) — a **pure** delete: returns a new map with the
                //      original intact, matching the interpreter. The bracket form
                //      mutates in place; the difference is a deliberate contract.
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::MAP_DELETE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Maps::MAP_DELETE, argResults.size()))
                        return BuildResult("", SadTypeKind::Boolean);

                    SIROperand copyOperand = emitMapCopy();
                    SIRInstruction inst(SIROpcode::CALL);
                    inst.result = SIROperand::Register(b_.newTempRegister(), SadTypeKind::Void);
                    inst.operands.push_back(SIROperand::ConstantString(kRuntimeMapDelete));
                    inst.operands.push_back(copyOperand);
                    inst.operands.push_back(argOperands[kArgKey]);
                    inst.comment = "maps: delete key (on the copy)";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);

                    BuildResult result(copyOperand.name, SadTypeKind::Map);
                    result.elementType = SadTypeKind::Void;
                    return result;
                }

                // ────────────────────────────────────────────────────────────
                // (AR) خريطة_مفاتيح(م) / خريطة_قيم(م) — مصفوفتا المفاتيحِ والقيم
                // (EN) خريطة_مفاتيح(m) / خريطة_قيم(m) — key and value arrays
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::MAP_KEYS || funcName == Bn::Maps::MAP_VALUES)
                {
                    const bool wantsKeys = (funcName == Bn::Maps::MAP_KEYS);
                    // (AR) ذراعٌ واحدةٌ لاسمَين ⇒ الرتبةُ تُقرأ من ثابتِ **الاسمِ
                    //      المنادى** لا من أحدِهما، وإلّا صار عقدُ «خريطة_قيم»
                    //      إعلانًا ميّتًا يحرسه ثابتُ غيرِه.
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           wantsKeys ? Ar::Maps::MAP_KEYS
                                                     : Ar::Maps::MAP_VALUES,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Array);
                    BuildResult result = emitRuntimeCall(
                        wantsKeys ? kRuntimeMapKeys : kRuntimeMapValues,
                        {kArgMap}, SadTypeKind::Array,
                        wantsKeys ? "maps: keys" : "maps: values");
                    // (AR) المفاتيحُ نصوصٌ قطعًا؛ والقيمُ مختلطةٌ ⇒ «أي» لا «فراغ»:
                    //      مسارُ القراءةِ الموسومِ زمنَ التشغيل في الخلفيّةِ مشروطٌ بأن يكون
                    //      نوعُ سجلِّ النتيجةِ **Any** بعينِه، فـ«فراغ» تسقطُ إلى القراءةِ
                    //      الخامّةِ فتُطبَعُ المؤشّراتُ أعدادًا. الوسومُ موجودةٌ، والمفتاحُ
                    //      أن يُطلَبَ المسارُ الذي يقرؤُها.
                    // (EN) Keys are always strings; values are mixed ⇒ «أي», not «فراغ»: the
                    //      backend's runtime-tagged read is gated on the result register's type
                    //      being **Any** specifically, so Void falls to the raw read and pointers
                    //      print as numbers. The tags exist; what matters is asking for the path
                    //      that reads them.
                    result.elementType = wantsKeys ? SadTypeKind::String : SadTypeKind::Any;
                    return result;
                }

                // ────────────────────────────────────────────────────────────
                // (AR) خريطة_احصل(م، ك) — قراءةٌ موسومةٌ زمنَ التشغيل تُرجع «أي»
                //      لا نصًّا. المسارُ القديمُ `__sad_map_get` كان يسوّي كلَّ قيمةٍ
                //      نصًّا فيضيعُ نوعُها (ق٣)، وهذه الواجهةُ الجديدةُ تحفظُه.
                // (EN) خريطة_احصل(m, k) — runtime-tagged read returning «أي», not a
                //      string. The old `__sad_map_get` flattened every value to text,
                //      losing its type (ق٣); this new interface preserves it.
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::MAP_GET)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Maps::MAP_GET, argResults.size()))
                        return BuildResult("", SadTypeKind::Any);
                    // (AR) الوسيطُ الثالثُ اختياريٌّ: قيمةٌ افتراضيّةٌ تُرجَعُ عندَ غيابِ
                    //      المفتاح (وبدونِه يُرجَعُ «لاشيء») — كما في المفسّر.
                    // (EN) The third argument is optional: a default returned when the
                    //      key is absent (without it, «لاشيء») — as in the interpreter.
                    std::vector<size_t> callArgs = {kArgMap, kArgKey};
                    if (argResults.size() >= Ar::Maps::MAP_GET.max)
                        callArgs.push_back(kArgDefault);
                    BuildResult result = emitRuntimeCall(
                        kRuntimeMapGetDyn, callArgs, SadTypeKind::Any, "maps: get (tagged)");
                    result.isDirectValue = true;
                    return result;
                }

                // ────────────────────────────────────────────────────────────
                // (AR) خريطة_اجلب_نص/رقم/منطقي(م، ك) — الجلبُ المصنَّف (RFC عقد
                //      الغياب — المرحلة ب): الغيابُ «لاشيء» حصرًا (لا «فراغ»)،
                //      والحضورُ بنوعٍ مغايرٍ أو بعدمٍ مخزَّنٍ خطأُ تشغيلٍ صريح
                //      (RUN074). لا وسيطَ بديلًا ثالثًا — التحصيلُ بـ«؟؟» هو
                //      الطريقُ الواحد. والثلاثةُ تُرجع «أي» (%SadDyn بوسمٍ خارجَ
                //      النطاقِ البِتّيّ): كان النصّيّ والرقميّ يُرمَزان بحارسِ
                //      kSadNullSentinel داخلَ النطاق، فقيمةٌ مخزَّنةٌ تساويه كانت
                //      تُقرأ غيابًا (دَينُ التصادمِ المعلَن — سُدّ بتوحيدِ القناة).
                // (EN) Typed fetch (stage ب): absence is Null exclusively; presence
                //      with a different type or a stored null is an explicit RUN074
                //      runtime error. No third default argument — «؟؟» is the way.
                //      All three return «أي» (%SadDyn, out-of-band tag): Str/Int
                //      used the in-band kSadNullSentinel, so a stored value equal
                //      to it read as absence (the declared collision debt — sealed
                //      by unifying the channel).
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::MAP_FETCH_STR ||
                    funcName == Bn::Maps::MAP_FETCH_NUM ||
                    funcName == Bn::Maps::MAP_FETCH_BOOL)
                {
                    // (AR) العدّة اثنان حصرًا: وسيطٌ ثالثٌ كان يُبتلَع صامتًا في المترجَم
                    //      بينما يرفضه المفسّر (قِيس — المراجعة العدائية)؛ والعقدُ ينصّ:
                    //      لا بديلَ ثالثًا، التحصيلُ بـ«؟؟» هو الطريقُ الواحد.
                    // (EN) Arity is exactly two: a third argument was silently swallowed
                    //      compiled while the interpreter rejects it (measured). The
                    //      contract forbids a third default — «؟؟» is the only path.
                    //      🔑 وكان العددُ ههنا ثابتًا محلّيًّا ورسالتُه نصًّا مكتوبًا
                    //      باليد — نسختان ثانيتان للحقيقةِ لا يقيسُهما أحد. الرتبةُ
                    //      الآن من مصدرِ الحقيقةِ لكلِّ اسمٍ من الثلاثةِ باسمِه.
                    const auto &fetchArity =
                        funcName == Bn::Maps::MAP_FETCH_NUM  ? Ar::Maps::MAP_FETCH_NUM
                        : funcName == Bn::Maps::MAP_FETCH_BOOL ? Ar::Maps::MAP_FETCH_BOOL
                                                               : Ar::Maps::MAP_FETCH_STR;
                    if (!checkBuiltinArity(b_.errors_, funcName, fetchArity,
                                           argResults.size()))
                        return BuildResult("", SadTypeKind::Any);
                    // (AR) resultKind واحدٌ (Any) للثلاثة — قناةُ الغيابِ خارجَ النطاقِ
                    //      البِتّيّ حصرًا (انظر بيانَ التصادمِ أعلاه).
                    // (EN) One resultKind (Any) for all three — the absence channel is
                    //      exclusively out-of-band (see the collision note above).
                    const SadTypeKind resultKind = SadTypeKind::Any;
                    const char *runtimeName = kRuntimeMapFetchStr;
                    const char *note = "maps: typed fetch (str, out-of-band)";
                    if (funcName == Bn::Maps::MAP_FETCH_NUM)
                    {
                        runtimeName = kRuntimeMapFetchInt;
                        note = "maps: typed fetch (int, out-of-band)";
                    }
                    else if (funcName == Bn::Maps::MAP_FETCH_BOOL)
                    {
                        runtimeName = kRuntimeMapFetchBool;
                        note = "maps: typed fetch (bool, out-of-band)";
                    }
                    BuildResult result = emitRuntimeCall(
                        runtimeName, {kArgMap, kArgKey}, resultKind, note);
                    result.isDirectValue = true;
                    return result;
                }

                // ────────────────────────────────────────────────────────────
                // (AR) خريطة_عين(م، ك، ق) — تعيينٌ **نقيٌّ**: خريطةٌ جديدةٌ والأصلُ
                //      سالمٌ، مطابقةً للمفسّر. والوسمُ يُشتقُّ من نوعِ القيمةِ سكونيًّا،
                //      وللقيمِ الموسومةِ زمنَ التشغيل تشتقُّه الخلفيّةُ من وسمِها.
                // (EN) خريطة_عين(m, k, v) — a **pure** set: a new map with the original
                //      intact, matching the interpreter. The tag is derived from the
                //      value's static type; for runtime-tagged values the backend
                //      derives it from the value's own kind.
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::MAP_SET)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Maps::MAP_SET, argResults.size()))
                        return BuildResult("", SadTypeKind::Map);

                    const int64_t valueTag = mapValueTagFor(argResults[kArgValue].type);

                    SIROperand copyOperand = emitMapCopy();
                    SIRInstruction inst(SIROpcode::CALL);
                    inst.result = SIROperand::Register(b_.newTempRegister(), SadTypeKind::Void);
                    inst.operands.push_back(SIROperand::ConstantString(kRuntimeMapSetTyped));
                    inst.operands.push_back(copyOperand);
                    inst.operands.push_back(argOperands[kArgKey]);
                    inst.operands.push_back(argOperands[kArgValue]);
                    inst.operands.push_back(SIROperand::ConstantI64(valueTag));
                    inst.comment = "maps: set typed (on the copy)";
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(inst);

                    BuildResult result(copyOperand.name, SadTypeKind::Map);
                    result.elementType = SadTypeKind::Void;
                    return result;
                }

                // ────────────────────────────────────────────────────────────
                // (AR) ازل_تشكيل(نصّ) — ق٢. العلّةُ نفسُها: مسجَّلةٌ في المفسّرِ
                //      ومعلَنةٌ في مصدرِ الحقيقةِ بلا نظيرٍ في المترجم.
                // (EN) ازل_تشكيل(text) — ق٢. Same cause: registered in the interpreter
                //      and declared in the SoT with no compiler counterpart.
                // ────────────────────────────────────────────────────────────
                if (funcName == Bn::Maps::STRIP_DIACRITICS)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Maps::STRIP_DIACRITICS, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    return emitRuntimeCall(
                        kRuntimeStripDiacritics, {kArgText}, SadTypeKind::String,
                        "maps: strip diacritics");
                }

                // (AR) ليست من هذه الوحدة — المتصلُ يتابعُ البحث
                // (EN) Not ours — the caller continues the lookup
                return std::nullopt;
            }

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
