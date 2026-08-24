/*
 * ============================================================================
 * (AR) التمثيل الديناميّ المميّز `%SadDyn` — تنفيذ حلّ ISSUE-076 الجذريّ.
 * (EN) The distinct dynamic representation `%SadDyn` — ISSUE-076 root fix impl.
 * ============================================================================
 */

#include "sad_dyn_repr.h"
#include "llvm_codegen.h"
#include "sir_constants.h" // (AR) kDivZeroRun001Msg + kSadPanicCheckViolation (د-1) / (EN) D-1 div-zero guard constants
#include "sad_event_layout_generated.h" // (② rfcs#46) اسم صنف «حدث» المضمَّن من SoT — استثناؤه من توسيع الحقول بالاسم
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Intrinsics.h>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
        // (AR) اسم النوع المميّز — مصدرٌ واحد / (EN) the distinct type name — single source
        static const char *kSadDynTypeName = "SadDyn";

        // ====================================================================
        // (AR) ISSUE-063: المسح المسبق لديناميّة الخانات — انظر توثيق llvm_codegen.h
        // (EN) ISSUE-063: the dyn-slot pre-scan — see the llvm_codegen.h docs
        // ====================================================================
        namespace
        {
            // (AR) تطبيع اسم الخانة: السجلّات تبدأ بـ% والعامّة لا / (EN) strip the % register prefix
            std::string cleanSlotName(const std::string &n)
            {
                return (!n.empty() && n[0] == '%') ? n.substr(1) : n;
            }

            // (AR) تجريد علامات التنصيص عن اسم الحقل / (EN) strip quotes off a field name
            std::string cleanFieldName(std::string f)
            {
                if (!f.empty() && f.front() == '"')
                    f = f.substr(1);
                if (!f.empty() && f.back() == '"')
                    f = f.substr(0, f.size() - 1);
                return f;
            }

            // (AR) حدّ التقارب الأعلى للمسح (تلوّثٌ عبر الدوال متسلسل) — عمليًّا يتقارب في ≤3
            // (EN) fixpoint upper bound (cross-function taint chains) — converges in ≤3 in practice
            constexpr int kDynScanMaxIterations = 8;
        } // namespace

        bool LLVMCodeGen::isDynSlot(const std::string &funcName, const std::string &slotName) const
        {
            std::string n = cleanSlotName(slotName);
            auto it = dynLocalSlots_.find(funcName);
            if (it != dynLocalSlots_.end() && it->second.count(n) != 0)
                return true;
            // (AR) العامُّ لا يُطبَّق على دالّةٍ تُصرّح الاسمَ محلّيًّا: `dynGlobalSlots_`
            //      مجموعةٌ مفتاحُها الاسمُ المجرَّد، وتطبيقُها بلا هذا الشرطِ كان يجعل
            //      محلّيًّا محسوسًا ديناميًّا لمجرّدِ مصادفةِ الاسم (انظر localSlotNames).
            // (EN) A global does not apply to a function that declares the name locally:
            //      `dynGlobalSlots_` is bare-name-keyed, and applying it unconditionally
            //      made a concrete local dynamic on a mere name clash (see localSlotNames).
            auto lit = funcLocalNames_.find(funcName);
            if (lit != funcLocalNames_.end() && lit->second.count(n) != 0)
                return false;
            return dynGlobalSlots_.count(n) != 0;
        }

        // (AR) SEM045 (أ٢): النوع المصرَّح للخانة — Unknown لغير المصرَّحة.
        // (EN) SEM045 (stage أ٢): the slot's declared kind; Unknown when undeclared.
        Sad::Types::SadTypeKind LLVMCodeGen::declaredSlotKind(
            const std::string &funcName, const std::string &slotName) const
        {
            const std::string n = cleanSlotName(slotName);
            auto it = declaredTypedSlots_.find(funcName);
            if (it == declaredTypedSlots_.end())
                return Sad::Types::SadTypeKind::Unknown;
            auto found = it->second.find(n);
            if (found == it->second.end())
                return Sad::Types::SadTypeKind::Unknown;
            return found->second;
        }

        void LLVMCodeGen::collectDynSlots(std::shared_ptr<SIRModule> sirModule)
        {
            dynGlobalSlots_.clear();
            dynLocalSlots_.clear();
            funcLocalNames_.clear();
            declaredTypedSlots_.clear();
            if (!sirModule)
                return;

            // (AR) نسخُ سجلِّ الأسماءِ المحلّيّةِ من SIR ليُقرأ في isDynSlot زمنَ التوليد
            // (EN) Copy the per-function local-name record from SIR for isDynSlot at emit time
            for (const auto &fn : sirModule->getFunctions())
                if (fn)
                {
                    funcLocalNames_[fn->getName()] = fn->localSlotNames;
                    // (AR) SEM045 (أ٢): سجلُّ الخانات المصرَّحة — لحارس STORE.
                    declaredTypedSlots_[fn->getName()] = fn->declaredTypedSlots;
                }

            // (AR) أسماء المتغيّرات العامّة + العامّ المصرَّح Any أصلًا (المستوى الأعلى)
            // (EN) global names + globals the frontend already declared Any (top level)
            std::set<std::string> globalNames;
            for (const auto &g : sirModule->getGlobalVariables())
            {
                if (!g)
                    continue;
                globalNames.insert(g->name);
                if (g->type == SadTypeKind::Any)
                    dynGlobalSlots_.insert(g->name);
            }

            // (AR) الدوال التي تُرجع ديناميًّا (تتقارب عبر الدورات) / (EN) dyn-returning functions
            std::map<std::string, bool> dynReturnFuncs;
            for (const auto &fn : sirModule->getFunctions())
                if (fn)
                    dynReturnFuncs[fn->getName()] = (fn->returnType == SadTypeKind::Any);

            // (AR) [بذرة [٨]] الدوالُّ المصرَّحةُ «رقم» التي رُقّي عائدُها إلى
            //      %SadDyn — يُدرَجُ لها قاسرُ RET في التمريرةِ اللاحقةِ أدناه.
            // (EN) [seed [٨]] Declared-«رقم» functions whose return was promoted
            //      to %SadDyn — the RET coercer is inserted for them below.
            std::set<std::string> declaredNumRetFuncs;

            bool changed = true;
            for (int iter = 0; changed && iter < kDynScanMaxIterations; ++iter)
            {
                changed = false;
                for (const auto &fn : sirModule->getFunctions())
                {
                    if (!fn)
                        continue;
                    std::set<std::string> &localDyn = dynLocalSlots_[fn->getName()];

                    // (AR) سجلّاتٌ قيمتها ديناميّة زمنَ التشغيل (مسح أماميّ داخل الدالّة)
                    // (EN) registers whose runtime value is dynamic (forward scan within the function)
                    std::set<std::string> dynRegs;
                    // (AR) أنواع القيم المخزَّنة لكلّ خانة — لقاعدة مزيج نصّ/عدد
                    // (EN) stored value kinds per slot — for the text/number mix rule
                    std::map<std::string, std::set<SadTypeKind>> slotStoredKinds;

                    // (AR) الأسماءُ المُصرَّحةُ محلّيًّا في هذه الدالّة — تُظلِّلُ العامَّ
                    // (EN) Names declared locally in this function — they shadow the global
                    const std::set<std::string> &localNames = fn->localSlotNames;

                    auto isDynSlotName = [&](const std::string &raw)
                    {
                        std::string n = cleanSlotName(raw);
                        if (localDyn.count(n) != 0)
                            return true;
                        if (localNames.count(n) != 0)
                            return false;
                        return dynGlobalSlots_.count(n) != 0;
                    };
                    // (AR) قيمةُ معامل ديناميّة؟ Any ساكنًا، أو سجلّ ملوَّث، أو خانةٌ-كقيمة
                    //      ديناميّة (resolveOperand يحمّل الخانة تلقائيًّا)
                    // (EN) dynamic operand value? statically Any, a tainted register, or a
                    //      dyn slot used as a value (resolveOperand auto-loads slots)
                    auto valueIsDyn = [&](const SIROperand &op)
                    {
                        if (op.dataType == SadTypeKind::Any)
                            return true;
                        // (AR) SEM045 (دَين الخانة المجرَّدة): ثابتُ الفراغِ (ConstantVoid) لا
                        //      يُمثَّل إلّا في خانةِ %SadDyn — وسمُ Void لا نظيرَ له في خانةٍ
                        //      خامٍ i64، فحمولتُه الصفريّةُ تُقرأ رقمًا كاذبًا. يُقصَر على
                        //      CONSTANT عمدًا: معاملُ LABEL يحمل dataType=Void وليس قيمةً.
                        // (EN) SEM045 (bare-slot debt): the Void constant is representable only
                        //      in a %SadDyn slot — a raw i64 slot has no Void tag, so its zero
                        //      payload reads back as a lying number. Restricted to CONSTANT on
                        //      purpose: LABEL operands carry dataType=Void without being values.
                        if (op.dataType == SadTypeKind::Void &&
                            op.type == SIROperandType::CONSTANT)
                            return true;
                        if (op.type != SIROperandType::REGISTER)
                            return false;
                        return dynRegs.count(op.name) != 0 || isDynSlotName(op.name);
                    };
                    auto markSlotDyn = [&](const std::string &raw)
                    {
                        std::string n = cleanSlotName(raw);
                        // (AR) ⚠️ الترقيةُ إلى المجموعةِ العامّةِ **لا** تُقيَّد بـ`localNames`،
                        //      وقد قِيسَ الأمران. فالتصريحُ في المستوى الأعلى (`متغير س = 10`)
                        //      يُسجَّلُ محلّيًّا في `main` كذلك — الوجهُ الأماميُّ يبنيه بالباني
                        //      العامِّ نفسِه بـ`isGlobal = false` — فتقييدُ الترقيةِ به يمنعُ
                        //      **الترقيةَ الحقيقيّةَ** للعامّ: `س = س / 4` في المستوى الأعلى
                        //      يُرقّي `س` عشريًّا، فتقرأُ `main` ٢٫٥ بينما تقرأُ دالّةٌ أخرى ٢.
                        //      قِيسَ على `005_global_promotion_split` و`003_self_division_loop`
                        //      و`079_بايت_اقتطاع_إعادة_إسناد_من_قسمة`: ثلاثتُها سقطت بالتقييد.
                        //      والتظليلُ يُحسَمُ على جهةِ **القراءة** وحدَها (isDynSlotName أعلاه
                        //      وisDynSlot) وهي كافيةٌ لعطبِ الاصطدام، إذ كان العطبُ قراءةً لا كتابة.
                        //      وتبقى بقيّةٌ مُعلَنةٌ لا مسكوتٌ عنها: محلّيٌّ **ديناميٌّ** في دالّةٍ
                        //      غيرِ `main` يصادفُ اسمَ عامٍّ ما زال يُرقّي ذلك العامَّ. سدُّها
                        //      يلزمه أن يُصرّح الوجهُ الأماميُّ عن العامِّ بـ`isGlobal = true` في
                        //      المستوى الأعلى، وذلك تغييرٌ في مِلكيّةِ المعلومةِ لا في هذا المسح.
                        // (EN) ⚠️ Escalation is deliberately NOT gated on `localNames`; both ways
                        //      were measured. A top-level declaration (`var x = 10`) is ALSO
                        //      recorded as a local of `main` — the front end builds it with the
                        //      same generic builder, with `isGlobal = false` — so gating on it
                        //      blocks the REAL promotion of the global: `x = x / 4` at top level
                        //      promotes `x` to float, and `main` reads 2.5 while another function
                        //      reads 2. Measured on 005_global_promotion_split,
                        //      003_self_division_loop and 079 (byte truncation on reassignment):
                        //      all three failed under the gate. Shadowing is decided on the READ
                        //      side alone (isDynSlotName above and isDynSlot), which suffices for
                        //      the collision defect — that defect was a read, not a write.
                        //      A declared, not silent, remainder: a DYNAMIC local in a non-main
                        //      function clashing with a global name still promotes that global.
                        //      Closing it requires the front end to mark top-level declarations
                        //      `isGlobal = true` — a change of information ownership, not of this scan.
                        if (globalNames.count(n))
                            changed = dynGlobalSlots_.insert(n).second || changed;
                        changed = localDyn.insert(n).second || changed;
                    };

                    for (const auto &bb : fn->getBasicBlocks())
                    {
                        if (!bb)
                            continue;
                        for (const auto &inst : bb->instructions)
                        {
                            // (AR) 1) تلويث السجلّات / (EN) 1) register tainting
                            if (inst.result.has_value())
                            {
                                bool resDyn = (inst.result->dataType == SadTypeKind::Any);
                                if (!resDyn && !inst.operands.empty())
                                {
                                    switch (inst.opcode)
                                    {
                                    case SIROpcode::LOAD:
                                        resDyn = isDynSlotName(inst.operands[0].name);
                                        break;
                                    case SIROpcode::MOVE:
                                        resDyn = valueIsDyn(inst.operands[0]);
                                        break;
                                    case SIROpcode::CALL:
                                    {
                                        auto cit = dynReturnFuncs.find(inst.operands[0].name);
                                        resDyn = (cit != dynReturnFuncs.end() && cit->second);
                                        break;
                                    }
                                    default:
                                        break;
                                    }
                                }
                                if (resDyn)
                                    dynRegs.insert(inst.result->name);
                            }

                            // (AR) 2) تخزينٌ بخانة (معاملان): ديناميّ ⇒ خانة %SadDyn؛
                            //         وجمعُ أنواع المخزون لقاعدة مزيج نصّ/عشريّ (دَين 5)
                            // (EN) 2) 2-operand STORE: dynamic ⇒ %SadDyn slot; also collect
                            //         stored kinds for the string/float mix rule (debt 5)
                            if (inst.opcode == SIROpcode::STORE && inst.operands.size() == 2)
                            {
                                const std::string slot = cleanSlotName(inst.operands[1].name);
                                if (valueIsDyn(inst.operands[0]))
                                {
                                    markSlotDyn(slot);
                                }
                                else
                                {
                                    // (AR) قاعدة المزيج: نجمع النصّيّ والعدديّ فقط (isNumericKind من SoT الأنواع)
                                    // (EN) mix rule: track only string and numeric kinds (SoT isNumericKind)
                                    SadTypeKind k = inst.operands[0].dataType;
                                    if (k == SadTypeKind::String || ::Sad::Types::isNumericKind(k))
                                    {
                                        auto &kinds = slotStoredKinds[slot];
                                        kinds.insert(k);
                                        // (AR) مزيجُ نصٍّ وعشريّ بخانةٍ واحدة: ترقية double
                                        //      القديمة تُنتج inttoptr(double) ⇒ فشل verifyModule
                                        // (EN) string+float mixed in one slot: the legacy double
                                        //      promotion yields inttoptr(double) ⇒ verifyModule failure
                                        if (kinds.count(SadTypeKind::String) &&
                                            kinds.count(SadTypeKind::Float))
                                            markSlotDyn(slot);
                                    }
                                }
                            }

                            // (AR) 3) تخزينٌ بحقل: OBJECT_SET (القيمة [2]) أو STORE ثلاثيّ
                            //         (القيمة [0] والحقل [2]) ⇒ رفعُ نوع الحقل إلى Any عند
                            //         الديناميّ أو المخالف للمعلَن (عشريّ/نصّ بحقل صحيح)
                            // (EN) 3) field stores: OBJECT_SET (value [2]) or 3-operand STORE
                            //         (value [0], field [2]) ⇒ raise the SIR field type to Any
                            //         for dynamic or declared-mismatching values
                            const bool isObjSet = (inst.opcode == SIROpcode::OBJECT_SET &&
                                                   inst.operands.size() >= 3);
                            const bool isMemberStore = (inst.opcode == SIROpcode::STORE &&
                                                        inst.operands.size() >= 3);
                            if (isObjSet || isMemberStore)
                            {
                                const SIROperand &valOp = isObjSet ? inst.operands[2] : inst.operands[0];
                                const std::string fieldName = cleanFieldName(
                                    isObjSet ? inst.operands[1].name : inst.operands[2].name);
                                const SadTypeKind v = valOp.dataType;
                                const bool dynVal = valueIsDyn(valOp);
                                for (const auto &cls : sirModule->getClasses())
                                {
                                    // (AR) صنف «حدث» المضمَّن مستثنى: مطابقة الاسم هنا كانت
                                    //      تُوسّع حقوله (س/ص/قيمة…) من تخزينات المستخدم
                                    //      فتفسد تخطيط thunk الحدث (انحدار #251).
                                    // (EN) The builtin «حدث» class is skipped: name-matching
                                    //      here widened its fields (x/y/value…) from user
                                    //      stores, corrupting the event-thunk layout (#251).
                                    if (!cls ||
                                        cls->name ==
                                            ::Sad::Types::EventLayout::SAD_EVENT_STRUCT_NAME)
                                        continue;
                                    auto fit = cls->fields_.find(fieldName);
                                    if (fit == cls->fields_.end() || fit->second == SadTypeKind::Any)
                                        continue;
                                    const SadTypeKind d = fit->second;
                                    const bool mismatch =
                                        (v == SadTypeKind::Float && d == SadTypeKind::Integer) ||
                                        (v == SadTypeKind::String &&
                                         (d == SadTypeKind::Integer || d == SadTypeKind::Float));
                                    if (dynVal || mismatch)
                                    {
                                        fit->second = SadTypeKind::Any;
                                        changed = true;
                                    }
                                }
                            }

                            // (AR) 4) إرجاعُ قيمةٍ ديناميّة ⇒ نوع إرجاع الدالّة Any (%SadDyn)
                            //         — كان dynPayloadI64 يقتطع الوسم عند حدود الدالّة.
                            //
                            //         ⚠️ إلّا العائدَ **المصرَّحَ في المصدر**: التصريحُ عقدُ
                            //         الكاتبِ والمفسّرُ (المرجعُ) يقسر الموسومَ عند حدِّه —
                            //         ترقيتُه هنا جعلت «دالة رقم» تُرجِع عشريًّا موسومًا
                            //         يفلت 5.0 حيث يطبع المفسّرُ 5 (قِيس — كشفُ المراجعةِ
                            //         العدائيّةِ لموجةِ وسمِ حدِّ المعامل). العائدُ المصرَّحُ
                            //         يبقى على تمثيلِه وبابُ RET يفكُّ الموسومَ بوسمِه
                            //         (unpackI64: عشريّ⇒fptosi، cf_return_switch.cpp).
                            // (EN) 4) returning a dynamic value ⇒ Any (%SadDyn) return type —
                            //         dynPayloadI64 used to strip the kind at the boundary.
                            //         EXCEPT a source-declared return: the declaration is the
                            //         author's contract and the interpreter coerces at it —
                            //         promoting it let a Float-tagged value escape a declared
                            //         «رقم» as 5.0 where the interpreter prints 5 (measured;
                            //         adversarial review of the param-boundary tag wave).
                            //         The RET door unpacks by tag (unpackI64).
                            // (AR) [بذرة [٨] — تعديلُ الاستثناء] المصرَّحُ «رقم» يُرقّى
                            //      **هو أيضًا** حين يُرجِعُ موسومًا — لكنْ مع قاسرِ RET
                            //      (kRuntimeDeclaredNumRetCoerce، يُدرَجُ أدناه): العشريُّ
                            //      يُقسَرُ رقمًا داخلَ الوسمِ فلا يفلت 5.0، وسائرُ الأوسامِ
                            //      تعبرُ بوسمِها كما يمرّرها المفسّرُ (كانت بذرةُ العدمِ
                            //      الواحدةُ تُسوّي فراغًا بعدمٍ وتُدخِلُ الحسابَ بذرةً).
                            //      وغيرُ «رقم» المصرَّحُ باقٍ على تمثيلِه المحسوس.
                            // (EN) [seed [٨] — the exception refined] A declared «رقم»
                            //      return is promoted TOO when it returns tagged — but
                            //      with the RET coercer inserted below: Float is coerced
                            //      to Int inside the tag (5.0 cannot escape) and every
                            //      other kind crosses tagged as the interpreter passes
                            //      it (the single null sentinel conflated Void with Null
                            //      and fed the sentinel into arithmetic). Non-«رقم»
                            //      declared returns keep their concrete representation.
                            {
                                const bool declaredNumeric =
                                    fn->returnTypeIsDeclared &&
                                    fn->returnType == SadTypeKind::Integer;
                                if (inst.opcode == SIROpcode::RET && !inst.operands.empty() &&
                                    fn->returnType != SadTypeKind::Any &&
                                    fn->returnType != SadTypeKind::Void &&
                                    (!fn->returnTypeIsDeclared || declaredNumeric) &&
                                    valueIsDyn(inst.operands[0]))
                                {
                                    if (declaredNumeric)
                                        declaredNumRetFuncs.insert(fn->getName());
                                    fn->returnType = SadTypeKind::Any;
                                    dynReturnFuncs[fn->getName()] = true;
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }

            // (AR) [بذرة [٨] — التمريرةُ اللاحقة] إدراجُ قاسرِ RET في كلِّ دالّةٍ
            //      مصرَّحةٍ «رقم» رُقّي عائدُها: قبلَ **كلِّ** RET بقيمةٍ يُدرَجُ
            //      نداءُ kRuntimeDeclaredNumRetCoerce ويُستبدَلُ معاملُ RET
            //      بنتيجتِه — فالمساراتُ المحسوسةُ (ارجع 2.5 الحرفيّة) تُقسَرُ
            //      كالموسومةِ سواءً، ولا مسارَ يفلتُ من العقد. بعدَ اكتمالِ
            //      النقطةِ الثابتةِ عمدًا: الإدراجُ أثناءَها يبطلُ المكرّرات.
            // (EN) [seed [٨] — the post-pass] Insert the RET coercer into every
            //      promoted declared-«رقم» function: before EVERY value-carrying
            //      RET a kRuntimeDeclaredNumRetCoerce call is inserted and the
            //      RET operand replaced with its result — concrete paths (a
            //      literal `ارجع 2.5`) are coerced exactly like tagged ones, so
            //      no path escapes the contract. Deliberately after the fixed
            //      point: inserting mid-iteration would invalidate iterators.
            {
                size_t coerceCounter = 0;
                for (const auto &fn : sirModule->getFunctions())
                {
                    if (!fn || declaredNumRetFuncs.count(fn->getName()) == 0)
                        continue;
                    for (const auto &bb : fn->getBasicBlocks())
                    {
                        if (!bb)
                            continue;
                        for (size_t i = 0; i < bb->instructions.size(); ++i)
                        {
                            SIRInstruction &retInst = bb->instructions[i];
                            if (retInst.opcode != SIROpcode::RET ||
                                retInst.operands.empty())
                                continue;
                            const std::string coerceReg =
                                std::string("%ret.coerce.") +
                                std::to_string(coerceCounter++);
                            SIRInstruction coerceCall(SIROpcode::CALL);
                            coerceCall.result =
                                SIROperand::Register(coerceReg, SadTypeKind::Any);
                            coerceCall.operands.push_back(
                                SIROperand::ConstantString(
                                    Sad::Compiler::kRuntimeDeclaredNumRetCoerce));
                            coerceCall.operands.push_back(retInst.operands[0]);
                            retInst.operands[0] =
                                SIROperand::Register(coerceReg, SadTypeKind::Any);
                            bb->instructions.insert(
                                bb->instructions.begin() +
                                    static_cast<std::ptrdiff_t>(i),
                                coerceCall);
                            ++i;
                        }
                    }
                }
            }
        }

        llvm::StructType *getSadDynType(llvm::LLVMContext &ctx)
        {
            // (AR) أعِد النوع المسمّى إن وُجد (يضمن هويّة واحدة عبر الوحدة كلّها)
            // (EN) reuse the named type if present (guarantees a single identity module-wide)
            if (llvm::StructType *existing = llvm::StructType::getTypeByName(ctx, kSadDynTypeName))
                return existing;
            return llvm::StructType::create(
                ctx,
                {llvm::Type::getInt8Ty(ctx), llvm::Type::getInt64Ty(ctx)},
                kSadDynTypeName);
        }

        bool isSadDyn(const llvm::Value *v)
        {
            if (!v)
                return false;
            llvm::Type *t = v->getType();
            if (!t->isStructTy())
                return false;
            auto *st = llvm::cast<llvm::StructType>(t);
            return st->hasName() && st->getName() == kSadDynTypeName;
        }

        llvm::Value *loadDynSlot(LLVMCodeGen &cg, llvm::Value *v)
        {
            if (!v || !cg.builder_ || !cg.context_)
                return v;
            llvm::Type *slotTy = nullptr;
            if (auto *alloca = llvm::dyn_cast<llvm::AllocaInst>(v))
                slotTy = alloca->getAllocatedType();
            else if (auto *gv = llvm::dyn_cast<llvm::GlobalVariable>(v))
                slotTy = gv->getValueType();
            if (!slotTy || slotTy != getSadDynType(*cg.context_))
                return v;
            return cg.builder_->CreateLoad(slotTy, v, "dyn.slot.load");
        }

        // ====================================================================
        // البناء / construction
        // ====================================================================
        llvm::Value *makeDyn(LLVMCodeGen &cg, llvm::Value *kindI8, llvm::Value *payloadI64)
        {
            auto &b = *cg.builder_;
            auto *ty = getSadDynType(*cg.context_);
            llvm::Value *agg = llvm::UndefValue::get(ty);
            agg = b.CreateInsertValue(agg, kindI8, {kDynKindFieldIdx}, "dyn.k");
            agg = b.CreateInsertValue(agg, payloadI64, {kDynPayloadFieldIdx}, "dyn.v");
            return agg;
        }

        llvm::Value *packDyn(LLVMCodeGen &cg, llvm::Value *concrete, uint8_t kind)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            llvm::Value *payload = nullptr;
            llvm::Type *ct = concrete ? concrete->getType() : nullptr;

            if (kind == DynKind::Float)
            {
                if (ct && ct->isDoubleTy())
                    payload = b.CreateBitCast(concrete, i64, "pack.f.bits");
                else if (ct && ct->isIntegerTy(64))
                    payload = concrete; // (AR) بِتّات double خام / (EN) raw double bits
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateBitCast(b.CreateSIToFP(concrete, dbl, "pack.i2f"), i64, "pack.f.bits2");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }
            else if (kind == DynKind::Int)
            {
                if (ct && ct->isIntegerTy(64))
                    payload = concrete;
                else if (ct && ct->isIntegerTy(1))
                    payload = b.CreateZExt(concrete, i64, "pack.b2i");
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateSExt(concrete, i64, "pack.i.ext");
                else if (ct && ct->isDoubleTy())
                    payload = b.CreateFPToSI(concrete, i64, "pack.d2i");
                else if (ct && ct->isPointerTy())
                    payload = b.CreatePtrToInt(concrete, i64, "pack.p2i");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }
            else if (kind == DynKind::Bool)
            {
                if (ct && ct->isIntegerTy(1))
                    payload = b.CreateZExt(concrete, i64, "pack.b2i");
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateZExtOrTrunc(concrete, i64, "pack.b.ext");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }
            else // Str / Array / Map / Obj / Adt / Null → مؤشّر أو صفر
            {
                if (ct && ct->isPointerTy())
                    payload = b.CreatePtrToInt(concrete, i64, "pack.p2i");
                else if (ct && ct->isIntegerTy(64))
                    payload = concrete;
                else if (ct && ct->isIntegerTy())
                    payload = b.CreateZExtOrTrunc(concrete, i64, "pack.x.ext");
                else
                    payload = llvm::ConstantInt::get(i64, 0);
            }

            return makeDyn(cg, llvm::ConstantInt::get(i8, kind), payload);
        }

        llvm::Value *resolveUnboxedIntOperand(LLVMCodeGen &cg,
                                              const Compiler::SIR::SIROperand &op)
        {
            // (AR) التوثيق والقياس في الترويسة — الفكُّ بوسمِه (unpackI64: عشريٌّ ⇒
            //      fptosi قيمةً لا بتّاتٍ) لا الحمولةُ الخام.
            // (EN) Docs and measurement in the header — tag-respecting unpack
            //      (unpackI64: Float ⇒ fptosi by value, not bits), never the raw
            //      payload.
            llvm::Value *v = cg.resolveOperand(op);
            if (v && isSadDyn(v))
                return unpackI64(cg, v);
            return v;
        }

        llvm::Value *coerceToParamType(LLVMCodeGen &cg, llvm::Value *v, llvm::Type *want,
                                       SadTypeKind sirType)
        {
            if (!v || !want || v->getType() == want)
                return v;
            llvm::StructType *dynTy = getSadDynType(*cg.context_);
            if (want == dynTy)
                return toDyn(cg, v, sirType);
            if (isSadDyn(v))
            {
                if (want->isDoubleTy())
                    return unpackDouble(cg, v);
                if (want->isPointerTy())
                    return unpackPtr(cg, v);
                // (AR) `unpackI64` لا `dynPayloadI64`: الأولى تحترم الوسمَ (عشريّ⇒fptosi)،
                //      والثانيةُ تُعيد الحمولةَ خامًّا فتُقرَأ بتّاتُ الـdouble عددًا صحيحًا.
                llvm::Value *raw = unpackI64(cg, v);
                // (AR) [عقدُ الغياب — حدُّ الوسيط] وسمُ الغيابِ (فراغ/عدم) لا يُفكُّ
                //      حمولةً صفريّةً تتنكّرُ رقمًا، بل يُهجَّأُ ببذرةِ العدمِ
                //      `kSadNullSentinel` — نظيرُ بابِ RET حرفيًّا: المستهلكُ يعرضُ
                //      «لاشيء» كما يمرّرُ المفسّرُ الغيابَ إلى المعاملِ ويعرضُه
                //      (قِيس 2026-08-23: عائدٌ غائبٌ موسومٌ مُرِّرَ لمعاملِ i64
                //      فطُبع 0 حيث يطبع المفسّرُ «لاشيء»).
                // (EN) [absence contract — the argument boundary] An absence tag
                //      (Void/Null) is not unpacked as a zero masquerading as a
                //      number but spelled with the null sentinel — the RET door's
                //      exact twin: the consumer displays «لاشيء» as the
                //      interpreter does when it passes absence into the parameter
                //      (measured 2026-08-23: a tagged absent return passed to an
                //      i64 parameter printed 0 where the interpreter prints
                //      «لاشيء»).
                if (want->isIntegerTy(64))
                {
                    auto *i8Ty = llvm::Type::getInt8Ty(*cg.context_);
                    llvm::Value *kindByte = dynKindByte(cg, v);
                    llvm::Value *isVoidK = cg.builder_->CreateICmpEQ(
                        kindByte, llvm::ConstantInt::get(i8Ty, DynKind::Void),
                        "arg.dyn.is.void");
                    llvm::Value *isNullK = cg.builder_->CreateICmpEQ(
                        kindByte, llvm::ConstantInt::get(i8Ty, DynKind::Null),
                        "arg.dyn.is.null");
                    llvm::Value *isAbsent = cg.builder_->CreateOr(
                        isVoidK, isNullK, "arg.dyn.absent");
                    raw = cg.builder_->CreateSelect(
                        isAbsent,
                        llvm::ConstantInt::get(
                            cg.getInt64Type(),
                            static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel)),
                        raw, "arg.dyn.i64");
                }
                return (want->isIntegerTy() && !want->isIntegerTy(64))
                           ? cg.builder_->CreateTrunc(raw, want, "arg.dyn.trunc")
                           : raw;
            }
            llvm::Type *have = v->getType();
            if (want->isIntegerTy(64) && have->isPointerTy())
                return cg.builder_->CreatePtrToInt(v, want, "arg.p2i");
            if (want->isPointerTy() && have->isIntegerTy(64))
                return cg.builder_->CreateIntToPtr(v, want, "arg.i2p");
            // (AR) منطقيٌّ ⇒ مؤشّر: عبر i64 (لا bitcast بين عرضَين مختلفَين).
            if (want->isPointerTy() && have->isIntegerTy(1))
                return cg.builder_->CreateIntToPtr(
                    cg.builder_->CreateZExt(v, cg.getInt64Type(), "arg.zext"), want, "arg.i2p");
            // (AR) عشريٌّ ⇒ عشريٌّ بعرضٍ آخر. أُضيف حين صار هذا **الجدولَ الوحيدَ**
            //      لثلاثةِ مواضع: السلسلةُ المحذوفةُ في `objects_arrays_ops.cpp` كانت
            //      تغطّيه بـ`CreateFPCast`، وغيابُه هنا يُسقِط الحالةَ إلى `poison`.
            //      ⚠️ ولا مسارَ مقيسًا يبلغُه اليومَ (`عشري32` غيرُ سطحيٍّ في
            //      `language-truth/types.yaml`)، فهو سدُّ تضييقٍ لا إصلاحُ عطبٍ حيّ —
            //      ويُذكَر كذلك كي لا يُقرأَ ادّعاءَ قياس.
            // (EN) float ⇒ float of another width. Added when this became the SINGLE table
            //      for three sites: the chain it replaced covered this with CreateFPCast, and
            //      its absence drops the case to `poison`. No measured path reaches it today
            //      (عشري32 is not surface-exposed), so this closes a narrowing, not a live
            //      defect — stated plainly so it is not read as a measurement.
            if (want->isFloatingPointTy() && have->isFloatingPointTy())
                return cg.builder_->CreateFPCast(v, want, "arg.fcast");
            if (want->isDoubleTy() && have->isIntegerTy())
                return cg.builder_->CreateSIToFP(v, want, "arg.i2f");
            if (want->isIntegerTy() && have->isDoubleTy())
                return cg.builder_->CreateFPToSI(v, want, "arg.f2i");
            // (AR) صحيحٌ ⇒ صحيحٌ بعرضٍ آخر. والتوسيعُ يجب أن يتبع **إشارةَ نوعِ المصدر**
            //      لا عرضَه: المنطقيُّ (i1) واللا-موقَّعُ (`بايت`/`طبيعي*`) بالأصفار،
            //      والموقَّعُ بالإشارة. وتوسيعُ `بايت ب = ٢٠٠` بالإشارةِ يُعطي ‑٥٦ —
            //      **جوابٌ خاطئٌ صامتٌ** لا خطأَ تحقّق، ولذلك تُمرَّر `sirType` أصلًا.
            if (want->isIntegerTy() && have->isIntegerTy())
            {
                const unsigned wantBits = want->getIntegerBitWidth();
                const unsigned haveBits = have->getIntegerBitWidth();
                if (wantBits < haveBits)
                    return cg.builder_->CreateTrunc(v, want, "arg.trunc");
                const bool unsignedSrc =
                    haveBits == 1 || sirType == SadTypeKind::Byte ||
                    sirType == SadTypeKind::UInt8 || sirType == SadTypeKind::UInt16 ||
                    sirType == SadTypeKind::UInt32 || sirType == SadTypeKind::UInt64 ||
                    sirType == SadTypeKind::Boolean || sirType == SadTypeKind::Char;
                return unsignedSrc ? cg.builder_->CreateZExt(v, want, "arg.zext")
                                   : cg.builder_->CreateSExt(v, want, "arg.sext");
            }
            return v;
        }

        llvm::Value *toDyn(LLVMCodeGen &cg, llvm::Value *v, SadTypeKind sirType)
        {
            if (isSadDyn(v))
                return v;
            uint8_t kind;
            // (AR) حرفيّةُ «لاشيء» تصل هنا أحيانًا بنوعٍ ساكنٍ صحيحٍ لا Null (ذراعُ
            //      `طابق` مقيسة)، وحمولتُها الحارس. فنَسِمُها Null **عند التغليف**، حيث
            //      يُعرَف أنّها ثابتٌ حرفيّ — بدل أن يستنتجَ المُقارِنُ العدمَ من الحمولةِ
            //      زمنَ التشغيل، فيَعُدَّ عددًا صحيحًا صالحًا قيمتُه الحارسُ عدمًا.
            // (EN) The «null» literal sometimes arrives with a static Integer type (measured in
            //      `match` arms) carrying the sentinel payload. Tag it Null **at boxing**, where
            //      it is known to be a literal constant, instead of having the comparator infer
            //      nullness from the payload at run time — which would make a legitimate integer
            //      equal to the sentinel compare as null.
            if (auto *constantOperand = llvm::dyn_cast_or_null<llvm::ConstantInt>(v))
            {
                if (constantOperand->getBitWidth() == 64 &&
                    constantOperand->getSExtValue() == Sad::Compiler::kSadNullSentinel)
                {
                    return packDyn(cg, v, DynKind::Null);
                }
            }
            switch (sirType)
            {
            case SadTypeKind::Integer:
                kind = DynKind::Int;
                break;
            case SadTypeKind::Float:
                kind = DynKind::Float;
                break;
            case SadTypeKind::Boolean:
                kind = DynKind::Bool;
                break;
            case SadTypeKind::String:
                kind = DynKind::Str;
                break;
            case SadTypeKind::Pointer:
                kind = DynKind::Str;
                break;
            // ═════════════════════════════════════════════════════════════════
            // (AR) 🔑 الكائنُ وسمٌ قائمٌ بذاته — وإسقاطُه إلى `default` كان يُثبِتُ وسمًا كاذبًا
            // ═════════════════════════════════════════════════════════════════
            //
            // (AR) المقيسُ قبلَ السدِّ: `أي ك = شخص()` ثمّ `نوع(ك)` يُجيبُ **«نص»**
            //      والمرجعُ «كائن» — بـrc=0 في الطرفَين، أي كذبٌ صامت. والعلّةُ
            //      أنّ `صنف`/`بنية` لم تكن لهما ذراعٌ هنا، فتسقطان إلى `default`
            //      فيُستدَلَّ على الوسمِ من **نوع LLVM**: الكائنُ مؤشّرٌ ⇒ `DynKind::Str`.
            //
            // (AR) ⚠️ وأثرُه أوسعُ من `نوع()`: حارسُ المُستقبِلِ العدميِّ
            //      (`oop_ops.cpp`) مُنِعَ من فحصِ الوسمِ على غرارِ `array_ops.cpp:176` **لأجلِ
            //      هذا الوسمِ بعينِه**: حارسٌ يُبنى على وسمٍ كاذبٍ يُخفِقُ على المُستقبِلِ
            //      السليم. فصدقُ الوسمِ هنا شرطٌ لـISSUE-142 لا تحسينٌ لـ`نوع()`.
            //
            // (EN) An object is a kind of its own; letting Class/Struct fall to `default`
            //      inferred the tag from the LLVM type (a pointer) and stamped `Str` — so
            //      `نوع()` answered «string» for an object, and the null-receiver guard in
            //      oop_ops was barred from checking the tag at all, because a guard built on
            //      a lying tag rejects valid receivers.
            case SadTypeKind::Class:
            case SadTypeKind::Struct:
                kind = DynKind::Obj;
                break;
            // (AR) والتعدادُ الجبريُّ وسمٌ ثالثٌ لا ثالثَ لهما: كان يصلُ `Struct`
            //      فيُوسَمُ كائنًا (ISSUE-153)، والمرجعُ يُجيبُ عنه «خريطة».
            // (EN) An ADT is a third tag: it used to arrive as Struct and be tagged as an
            //      object (ISSUE-153); the reference answers «map» for it.
            case SadTypeKind::Enum:
                kind = DynKind::Adt;
                break;
            case SadTypeKind::Null:
                kind = DynKind::Null;
                break;
            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 الفراغُ وسمٌ قائمٌ بذاته، لا يُستدَلُّ عليه من نوع LLVM
            // ════════════════════════════════════════════════════════════════
            //
            // (AR) للعدمِ **شكلان** يفرّق بينهما المحرّكان في نصِّهما:
            //        • خانةٌ صُرِّحت ولم تُهيَّأ  ⇒ `فراغ` (Void)
            //        • خانةٌ هُيِّئت بـ«لاشيء»    ⇒ `عدم`  (Null)
            //      وتوحيدُهما يجعل كلَّ تشخيصِ تصريحٍ مجرَّدٍ يكذبُ «عدم».
            //
            //      وبلا هذه الذراعِ يسقط `Void` في `default` فيُستدَلَّ عليه من
            //      **نوع LLVM**: حمولةُ الصفرِ i64 ⇒ `DynKind::Int` ⇒ `نوع()`
            //      يُجيب «رقم». أي أنّ الاستدلالَ من التمثيلِ يمحو تمييزًا
            //      يحمله النوعُ الساكنُ وحدَه — والتمثيلُ لا يُميّز فراغًا من صفر.
            // (EN) Void is a kind of its own, never inferred from the LLVM type: the
            //      zero i64 payload would infer Int and نوع() would answer «رقم»,
            //      erasing a distinction only the static type carries.
            case SadTypeKind::Void:
                kind = DynKind::Void;
                break;
            case SadTypeKind::Array:
                kind = DynKind::Array;
                break;
            case SadTypeKind::Map:
                kind = DynKind::Map;
                break;
            default:
            {
                // (AR) استدلالٌ من نوع LLVM حين يكون SIR مبهمًا (Any/Void/…)
                // (EN) infer from the LLVM type when the SIR type is opaque (Any/Void/…)
                llvm::Type *t = v ? v->getType() : nullptr;
                if (t && t->isDoubleTy())
                    kind = DynKind::Float;
                else if (t && t->isIntegerTy(1))
                    kind = DynKind::Bool;
                else if (t && t->isPointerTy())
                    kind = DynKind::Str;
                else
                    kind = DynKind::Int;
                break;
            }
            }
            return packDyn(cg, v, kind);
        }

        // ====================================================================
        // الاستخراج / extraction
        // ====================================================================
        llvm::Value *dynKindByte(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            return cg.builder_->CreateExtractValue(dyn, {kDynKindFieldIdx}, "dyn.kind");
        }

        llvm::Value *dynPayloadI64(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            return cg.builder_->CreateExtractValue(dyn, {kDynPayloadFieldIdx}, "dyn.payload");
        }

        llvm::Value *unpackDouble(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            llvm::Value *payload = dynPayloadI64(cg, dyn);
            llvm::Value *kind = dynKindByte(cg, dyn);
            llvm::Value *isF = b.CreateICmpEQ(kind, llvm::ConstantInt::get(i8, DynKind::Float), "dyn.isf");
            llvm::Value *asBits = b.CreateBitCast(payload, dbl, "dyn.f.bc");
            llvm::Value *asInt = b.CreateSIToFP(payload, dbl, "dyn.i.f");
            return b.CreateSelect(isF, asBits, asInt, "dyn.d");
        }

        llvm::Value *unpackPtr(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto *ptrTy = llvm::PointerType::getUnqual(*cg.context_);
            return cg.builder_->CreateIntToPtr(dynPayloadI64(cg, dyn), ptrTy, "dyn.ptr");
        }

        llvm::Value *unpackI64(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            llvm::Value *payload = dynPayloadI64(cg, dyn);
            llvm::Value *isF = b.CreateICmpEQ(
                dynKindByte(cg, dyn), llvm::ConstantInt::get(i8, DynKind::Float), "dyn.i.isf");
            // (AR) دلالة الإشباع الموحَّدة منصّيًّا (fptosi.sat) — كمواضع البتّيّات في
            //      arith_main/arith_cmp، وإلّا يتباعد مسار SadDyn عن الدلالة المعلَنة.
            // (EN) Platform-independent saturation (fptosi.sat) — matches the bitwise
            //      sites in arith_main/arith_cmp; a raw fptosi here would make the
            //      SadDyn path diverge from the declared semantics.
            llvm::Function *satFn = llvm::Intrinsic::getDeclaration(
                cg.module_.get(), llvm::Intrinsic::fptosi_sat, {i64, dbl});
            llvm::Value *fromF = b.CreateCall(
                satFn, {b.CreateBitCast(payload, dbl, "dyn.i.fbc")}, "dyn.i.f2i.sat");
            return b.CreateSelect(isF, fromF, payload, "dyn.i64");
        }

        // (AR) علّب قيمةً في %SadDyn بالكومة، وأرجِع مؤشّرًا إليه (option A لعناصر المصفوفة).
        // (EN) Box a value into a heap %SadDyn; return a pointer to it (option A array elements).
        llvm::Value *boxDynToHeap(LLVMCodeGen &cg, llvm::Value *v, SadTypeKind sirType)
        {
            llvm::Value *dyn = toDyn(cg, v, sirType);
            llvm::StructType *dynTy = getSadDynType(*cg.context_);
            uint64_t dynBytes = cg.module_->getDataLayout().getTypeAllocSize(dynTy);
            llvm::Value *sizeV =
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg.context_), dynBytes);
            llvm::Value *box = cg.emitMalloc(sizeV, "box.dyn");
            cg.builder_->CreateStore(dyn, box);
            return box;
        }

        // (AR) اقرأ %SadDyn من مؤشّر كومةٍ سبق تعليبه (عكس boxDynToHeap).
        // (EN) Load a %SadDyn from a heap box pointer (inverse of boxDynToHeap).
        llvm::Value *unboxDynFromHeap(LLVMCodeGen &cg, llvm::Value *boxPtr)
        {
            llvm::StructType *dynTy = getSadDynType(*cg.context_);
            return cg.builder_->CreateLoad(dynTy, boxPtr, "unbox.dyn");
        }

        // ====================================================================
        // الموزِّعات / dispatchers
        // ====================================================================
        static llvm::Value *isFloatKind(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto *i8 = llvm::Type::getInt8Ty(*cg.context_);
            return cg.builder_->CreateICmpEQ(
                dynKindByte(cg, dyn), llvm::ConstantInt::get(i8, DynKind::Float), "dyn.k.isf");
        }

        // (AR) حارس القاسم العشريّ الصفريّ للمسار الديناميّ (د-1 + إغلاق بوّابتَي
        //      NaN المتبقّيتين RUN009/RUN010 — مرآة emitFloatDivZeroGuard في المسار
        //      الساكن): failCond جاهز عند النداء (عادةً eitherF ∧ القاسم == 0.0 —
        //      أو intDivZero للفرع الصحيح — رسالة %g أو %lld بحسب المقسوم). مستضاف ⇒ تشخيص عربيّ
        //      بالكتالوج (hostedMsg بموضع %g للمقسوم) + exit(1)؛ حرّ ⇒ __sad_panic.
        // (EN) Dynamic-path float zero-divisor guard (D-1 + closing the remaining
        //      NaN gates RUN009/RUN010 — mirror of the static-path
        //      emitFloatDivZeroGuard): failCond is precomputed by the caller
        //      (typically eitherF ∧ divisor == 0.0 — the integer branch keeps its
        //      or intDivZero for the int branch — %g or %lld message to match the
        //      dividend). Hosted ⇒ Arabic catalog diagnostic
        //      (hostedMsg with %g for the dividend) + exit(1); freestanding ⇒
        //      __sad_panic(check-violation).
        // (AR) الحاجز ٧ (مشترك): يُعرَّف هنا ويُعلَن في sad_dyn_repr.h. راجع التوثيق هناك.
        // (EN) Barrier 7 (shared): defined here, declared in sad_dyn_repr.h. See docs there.
        void emitRecoverablePanicToHandler(LLVMCodeGen &cg, llvm::Value *msgPtr)
        {
            namespace SC = ::Sad::Compiler;
            llvm::IRBuilder<> &b = *cg.builder_;
            llvm::LLVMContext &ctx = *cg.context_;
            auto *ptrType = llvm::PointerType::getUnqual(ctx);
            auto *i32Type = llvm::Type::getInt32Ty(ctx);
            auto *i64Type = cg.getInt64Type();

            // (AR) ع-16: كل مواضع إنشاء حالة الاستثناء تضبط TLS في الوضع المستضاف
            //      — هذا الملف كان **النسخة الثالثة** غير المرقعة (نمط «ثلاث نسخ:
            //      أصلحت اثنتين وبقيت واحدة»): من ينشئ أولا يحسم، فبرنامج أول
            //      قسمة ديناميكية فيه تسبق أول «حاول» كان يخرج بحالة غير معزولة.
            // (EN) ع-16: every exception-state creation site sets TLS when hosted
            //      — this file was the unpatched THIRD COPY (the "fixed two,
            //      left one" pattern): first creator wins, so a program whose
            //      first dynamic division precedes its first «try» kept the
            //      state shared across threads.
            auto applyExceptionTls = [&](llvm::GlobalVariable *g)
            {
                if (g && !cg.freestanding_)
                    g->setThreadLocal(true);
            };

            auto ensurePtrGlobal = [&](const char *name) -> llvm::GlobalVariable *
            {
                auto *g = cg.module_->getNamedGlobal(name);
                if (!g)
                {
                    g = new llvm::GlobalVariable(
                        *cg.module_, ptrType, false, llvm::GlobalValue::InternalLinkage,
                        llvm::ConstantPointerNull::get(ptrType), name);
                    applyExceptionTls(g);
                }
                return g;
            };

            auto *handlerCount = cg.module_->getNamedGlobal(SC::kRuntimeHandlerCount);
            if (!handlerCount)
            {
                handlerCount = new llvm::GlobalVariable(
                    *cg.module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                    llvm::ConstantInt::get(i32Type, 0), SC::kRuntimeHandlerCount);
                applyExceptionTls(handlerCount);
            }
            auto *handlerStack = cg.module_->getNamedGlobal(SC::kRuntimeHandlerStack);
            if (!handlerStack)
            {
                auto *arrTy = llvm::ArrayType::get(ptrType, SC::kSadHandlerStackCapacity);
                handlerStack = new llvm::GlobalVariable(
                    *cg.module_, arrTy, false, llvm::GlobalValue::InternalLinkage,
                    llvm::ConstantAggregateZero::get(arrTy), SC::kRuntimeHandlerStack);
                applyExceptionTls(handlerStack);
            }
            // (AR) القرار على عدّاد «حاول» النشطة لا على handlerCount: كلُّ دالّة تدفع
            //      معالِجَ تنظيفٍ فيصبح handlerCount ≥ 1 دائمًا. نرفع فقط إن كانت ثمّة
            //      «حاول» فعليّة، وإلّا نُبقي السلوكَ القديم (تشخيص + exit) فلا انحدار
            //      لقسمةٍ خارج «حاول».
            // (EN) Decide on the active-try counter, not handlerCount: every function
            //      pushes a cleanup handler so handlerCount is always ≥ 1. Raise only if a
            //      real «try» is active; otherwise keep the old behaviour (diagnostic +
            //      exit), so no regression for a division outside any «try».
            // (AR) ع-16: عداد «حاول» النشطة TLS أيضا — قرار مشترك فوق مكدس TLS
            //      كان يجعل «حاول» في الرئيسي تحول هلع خيط آخر إلى قفزة longjmp
            //      إلى مكدسه هو (والعكس)
            // (EN) ع-16: the active-try counter is TLS too — a shared decision
            //      flag over a TLS stack let a main-thread «try» turn another
            //      thread's panic into a longjmp within that other thread's own
            //      stack (and vice versa)
            auto *tryActive = cg.module_->getNamedGlobal(SC::kRuntimeTryActive);
            if (!tryActive)
            {
                tryActive = new llvm::GlobalVariable(
                    *cg.module_, i32Type, false, llvm::GlobalValue::InternalLinkage,
                    llvm::ConstantInt::get(i32Type, 0), SC::kRuntimeTryActive);
                applyExceptionTls(tryActive);
            }

            llvm::Function *curFunc = b.GetInsertBlock()->getParent();
            auto *raiseBB = llvm::BasicBlock::Create(ctx, "panic.raise", curFunc);
            auto *noHandlerBB = llvm::BasicBlock::Create(ctx, "panic.nohandler", curFunc);

            llvm::Value *count = b.CreateLoad(i32Type, handlerCount, "panic.hcount");
            llvm::Value *activeTries = b.CreateLoad(i32Type, tryActive, "panic.tryactive");
            llvm::Value *hasHandler = b.CreateICmpSGT(activeTries, b.getInt32(0), "panic.hashandler");
            b.CreateCondBr(hasHandler, raiseBB, noHandlerBB);

            b.SetInsertPoint(raiseBB);
            b.CreateStore(cg.getConstantString("\xd8\xae\xd8\xb7\xd8\xa3"), // "خطأ"
                          ensurePtrGlobal(SC::kRuntimeExceptionType));
            b.CreateStore(msgPtr ? msgPtr : llvm::ConstantPointerNull::get(ptrType),
                          ensurePtrGlobal(SC::kRuntimeExceptionMsg));
            auto *exceptionValue = cg.module_->getNamedGlobal(SC::kRuntimeExceptionValue);
            if (!exceptionValue)
                exceptionValue = new llvm::GlobalVariable(
                    *cg.module_, i64Type, false, llvm::GlobalValue::InternalLinkage,
                    llvm::ConstantInt::get(i64Type, 0), SC::kRuntimeExceptionValue);
            b.CreateStore(llvm::ConstantInt::get(i64Type, 0), exceptionValue);

            llvm::Value *idx = b.CreateSub(count, b.getInt32(1), "panic.idx");
            // (AR) تقصيرُ الفهرس — نظيرُ ما في exception_ops. راجِعْ kSadHandlerStackCapacity.
            // (EN) Clamp the index — the twin of exception_ops. See kSadHandlerStackCapacity.
            {
                llvm::Value *lastSlot =
                    b.getInt32(static_cast<int>(SC::kSadHandlerStackCapacity) - 1);
                llvm::Value *tooDeep = b.CreateICmpSGT(idx, lastSlot, "panic.idx.deep");
                idx = b.CreateSelect(tooDeep, lastSlot, idx, "panic.idx.clamped");
            }
            auto *arrTy = llvm::ArrayType::get(ptrType, SC::kSadHandlerStackCapacity);
            llvm::Value *slot = b.CreateGEP(arrTy, handlerStack, {b.getInt32(0), idx}, "panic.slot");
            llvm::Value *jmpbuf = b.CreateLoad(ptrType, slot, "panic.jmpbuf");
            auto *longjmpTy = llvm::FunctionType::get(
                llvm::Type::getVoidTy(ctx), {ptrType, i32Type}, false);
            auto longjmpCallee = cg.module_->getOrInsertFunction("longjmp", longjmpTy);
            if (auto *lj = llvm::dyn_cast<llvm::Function>(longjmpCallee.getCallee()))
                lj->addFnAttr(llvm::Attribute::NoReturn);
            b.CreateCall(longjmpCallee, {jmpbuf, b.getInt32(1)});
            b.CreateUnreachable();

            b.SetInsertPoint(noHandlerBB);
        }

        static void emitDynDivZeroGuard(LLVMCodeGen &cg, llvm::Value *failCond,
                                             llvm::Value *dividendD,
                                             const char *hostedMsg, const char *tag)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            llvm::Function *curFunc = b.GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB =
                llvm::BasicBlock::Create(ctx, std::string(tag) + ".fail", curFunc);
            llvm::BasicBlock *contBB =
                llvm::BasicBlock::Create(ctx, std::string(tag) + ".ok", curFunc);
            b.CreateCondBr(failCond, failBB, contBB);

            b.SetInsertPoint(failBB);
            if (cg.freestanding_)
            {
                cg.emitFreestandingPanicCall(Sad::Compiler::kSadPanicCheckViolation);
            }
            else
            {
                auto *ptrTy = llvm::PointerType::getUnqual(ctx);
                auto *printfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx), {ptrTy}, true);
                auto printfFunc = cg.module_->getOrInsertFunction("printf", printfType);
                llvm::Value *msg = b.CreateGlobalStringPtr(
                    hostedMsg, std::string(tag) + ".fmt");
                // (AR) الحاجز ٧: إن كان ثمّة «حاول» نشط ارفع استثناءً قابلًا للالتقاط
                // (EN) Barrier 7: if a «try» is active, raise a catchable exception
                emitRecoverablePanicToHandler(cg, msg);
                b.CreateCall(printfFunc, {msg, dividendD});
                auto *exitType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(ctx), {llvm::Type::getInt32Ty(ctx)}, false);
                auto exitFunc = cg.module_->getOrInsertFunction("exit", exitType);
                b.CreateCall(exitFunc,
                             {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
            }
            b.CreateUnreachable();

            b.SetInsertPoint(contBB);
        }

        // ====================================================================
        // (AR) SEM045 (أ٢): الحارس الزمنيّ قبل STORE «فراغ ⇒ خانة مصنّفة».
        //      على نمط emitDynDivZeroGuard: فحص وسمٍ + فرعا فشل/استمرار. الرسالة
        //      تُطبع بـ«%s» لا كنسق مباشرةً — اسمُ الخانةِ نصُّ مستخدمٍ وقد يحمل
        //      محارفَ نسقِ printf (ثغرةُ نسقٍ لا مجردُ تشويه).
        // (EN) SEM045 (stage أ٢): pre-STORE guard, mirroring emitDynDivZeroGuard.
        //      The message is printed via "%s", never as a format string — the
        //      slot name is user text and may contain printf specifiers.
        // ====================================================================
        void emitDynVoidStoreGuard(LLVMCodeGen &cg, llvm::Value *dynValue,
                                   const std::string &slotName,
                                   const std::string &typeName, bool fatal)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            llvm::Function *curFunc = b.GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB =
                llvm::BasicBlock::Create(ctx, "sem045.fail", curFunc);
            llvm::BasicBlock *contBB =
                llvm::BasicBlock::Create(ctx, "sem045.ok", curFunc);
            llvm::Value *kind = dynKindByte(cg, dynValue);
            llvm::Value *isVoid = b.CreateICmpEQ(
                kind, b.getInt8(static_cast<uint8_t>(DynKind::Void)), "sem045.isvoid");
            b.CreateCondBr(isVoid, failBB, contBB);

            b.SetInsertPoint(failBB);
            const std::string tag = fatal ? "[خطأ نوع SEM045] " : "[تحذير نوع SEM045] ";
            // (AR) اسم السجلّ يحمل بادئة % — تُجرَّد قبل العرض للمستخدم.
            const std::string bareName = cleanSlotName(slotName);
            const std::string msgText =
                tag + "الخانة '" + bareName + "' من نوع '" + typeName +
                "' أُسند إليها 'فراغ' وقت التشغيل — غيابُ نتيجةٍ لا قيمة، فلا يصلح حشوًا لخانةٍ أعلنت نوعَها\n";
            if (cg.freestanding_)
            {
                // (AR) حرًّا لا printf — والتحذيرُ الحرّ يُسقَط (لا قناةَ تشخيصٍ غيرُ الهلع).
                // (EN) Freestanding: no printf; the warn flavor is dropped (panic is
                //      the only diagnostic channel).
                if (fatal)
                    cg.emitFreestandingPanicCall(Sad::Compiler::kSadPanicDynTypeMismatch);
                else
                    b.CreateBr(contBB);
            }
            else
            {
                auto *ptrTy = llvm::PointerType::getUnqual(ctx);
                auto *printfType = llvm::FunctionType::get(
                    llvm::Type::getInt32Ty(ctx), {ptrTy}, true);
                auto printfFunc = cg.module_->getOrInsertFunction("printf", printfType);
                llvm::Value *fmt = b.CreateGlobalStringPtr("%s", "sem045.fmt");
                llvm::Value *msg = b.CreateGlobalStringPtr(msgText, "sem045.msg");
                if (fatal)
                {
                    // (AR) الطباعةُ **قبل** حاجزِ «حاول»: القفزُ إلى الماسكِ كان يسبق
                    //      printf فيختفي التشخيصُ متى وُجد ماسك — والمفسّرُ يطبع قبل
                    //      الرمي، فالترتيبُ هنا يحفظ تكافؤَ حضورِ التشخيص (قِيس).
                    // (EN) Print BEFORE the try-handler barrier: the longjmp used to
                    //      precede printf, hiding the diagnostic whenever a handler
                    //      existed — the interpreter prints before throwing (measured).
                    b.CreateCall(printfFunc, {fmt, msg});
                    // (AR) الحاجز ٧: إن كان ثمّة «حاول» نشط ارفع استثناءً قابلًا للالتقاط
                    // (EN) Barrier 7: if a «try» is active, raise a catchable exception
                    emitRecoverablePanicToHandler(cg, msg);
                    auto *exitType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(ctx), {llvm::Type::getInt32Ty(ctx)}, false);
                    auto exitFunc = cg.module_->getOrInsertFunction("exit", exitType);
                    b.CreateCall(exitFunc,
                                 {llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 1)});
                }
                else
                {
                    b.CreateCall(printfFunc, {fmt, msg});
                    b.CreateBr(contBB);
                }
            }
            if (fatal)
                b.CreateUnreachable();

            b.SetInsertPoint(contBB);
        }

        llvm::Value *dynBinOp(LLVMCodeGen &cg, SIROpcode op, llvm::Value *l, llvm::Value *r)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);

            llvm::Value *eitherF = b.CreateOr(isFloatKind(cg, l), isFloatKind(cg, r), "dyn.either.f");
            llvm::Value *lD = unpackDouble(cg, l);
            llvm::Value *rD = unpackDouble(cg, r);
            llvm::Value *lI = dynPayloadI64(cg, l);
            llvm::Value *rI = dynPayloadI64(cg, r);

            // (AR) حارس الغياب (نظير RUN053، توافق المحرّكين): معاملٌ وسمُه Null أو
            //      Void كان يُفكُّ حمولتُه صفرًا فيُنتج «غياب + 1 = 1» بخروجِ صفرٍ —
            //      بينما المفسّر يُبلغ RUN053 ويخرج 1. الفحص هنا في **الباب الواحد**
            //      لكلّ الحسابيّات الموسومة (+، -، *، /، %، //) لا في مواضع الظهور؛
            //      والمقارناتُ (== لاشيء) لا تمرّ بهذا الباب فلا يمسّها الحارس.
            //      وسما Str/Bool يبقيان خارج الحارس عمدًا: لم يُقاسا بعدُ (دَين معلَن).
            //      مقيس: سلسلةُ نصٍّ مع جلبٍ غائبٍ («غائب + "لاحقة"») لا تمرّ بهذا
            //      البابِ أصلًا وتبقى مشروعةً في المحرّكين؛ بقيّةُ توليفاتِ الغيابِ
            //      مع الحاوياتِ غيرُ مقيسةٍ بعد. والسالبُ الأحاديّ على غيابٍ موسومٍ
            //      يمرّ به (‎-غائب ⇒ RUN053 مترجَمًا) — إعلانُ مرحلةٍ لا قياسُ تكافؤ.
            // (EN) Absence guard (RUN053 counterpart, engine parity): an operand
            //      tagged Null or Void had its payload unpacked as zero, producing
            //      «absence + 1 = 1» with exit 0 — the interpreter reports RUN053 and
            //      exits 1. The check lives in this single door for all tagged
            //      arithmetic (+,-,*,/,%,//), not at symptom sites; comparisons
            //      (== لاشيء) never route here, so they stay legal. Str/Bool tags are
            //      deliberately outside the guard: unmeasured yet (declared debt).
            {
                auto *nullK = llvm::ConstantInt::get(i8, DynKind::Null);
                auto *voidK = llvm::ConstantInt::get(i8, DynKind::Void);
                llvm::Value *lK = dynKindByte(cg, l);
                llvm::Value *rK = dynKindByte(cg, r);
                llvm::Value *lAbsent = b.CreateOr(
                    b.CreateICmpEQ(lK, nullK, "dyn.abs.ln"),
                    b.CreateICmpEQ(lK, voidK, "dyn.abs.lv"), "dyn.abs.l");
                llvm::Value *rAbsent = b.CreateOr(
                    b.CreateICmpEQ(rK, nullK, "dyn.abs.rn"),
                    b.CreateICmpEQ(rK, voidK, "dyn.abs.rv"), "dyn.abs.r");
                emitDynDivZeroGuard(cg,
                                    b.CreateOr(lAbsent, rAbsent, "dyn.abs.either"),
                                    lI, Sad::Compiler::kNumericRequiredRun053Msg,
                                    "dyn.absence");
            }

            // (AR) === Amelia (ISSUE-076): قاسمٌ آمنٌ للفرع الصحيح المُهمَل ===
            //      الموزِّع بلا فروع: يحسب iRes (صحيح) و fRes (عشريّ) دائمًا ثمّ يختار بالوسم.
            //      لكنّ sdiv/srem على صفرٍ (أو INT_MIN/-1) سلوكٌ غير معرَّف يُطلق #DE (SIGFPE)
            //      على x86 **وإن أُهمِلت النتيجة**. عند عمليّةٍ عشريّة (eitherF) تكون rI بِتّات
            //      double خام (قد تساوي 0 للقاسم 0.0) ⇒ الـsdiv المُهمَل ينهار. نستبدل القاسمَ 1
            //      على الفرع العشريّ فقط؛ الفرع الصحيح الحقيقيّ يُبقي rI (دلالةُ القسمة محفوظة).
            // (EN) === Amelia (ISSUE-076): safe divisor for the discarded integer branch ===
            //      The dispatcher is branchless: it always computes iRes (int) and fRes (float)
            //      then selects by the tag. But sdiv/srem by zero (or INT_MIN/-1) is UB that
            //      raises #DE (SIGFPE) on x86 **even when the result is unused**. For a float op
            //      (eitherF) rI holds raw double bits (0 for divisor 0.0) ⇒ the discarded sdiv
            //      traps. Substitute divisor 1 on the float branch only; the true integer path
            //      keeps rI intact (integer-division semantics preserved).
            // (AR) Amelia (ISSUE-063): توسيع الحارس — إلى جانب الفرع العشريّ، القاسمُ
            //      الصحيح صفر (sdiv/srem ⇒ #DE انهيار) وINT64_MIN/-1 (فيض sdiv ⇒ #DE أيضًا)
            //      يستبدلان القاسمَ 1؛ ثمّ تُفرَض النتيجة 0 لحالة القسمة على صفر (سياسة
            //      المسار الساكن المترجم نفسها — المفسّر يرمي RUN001/RUN009، تباعدٌ موثَّق).
            // (EN) Amelia (ISSUE-063): widen the guard — besides the float branch, an integer
            //      zero divisor (sdiv/srem ⇒ #DE crash) and INT64_MIN/-1 (sdiv overflow ⇒ #DE
            //      too) substitute divisor 1. The int zero-divisor case is no longer a
            //      silent result-0 policy: it is rejected by the guards below with
            //      RUN001/RUN009/RUN010, matching the interpreter (divergence closed).
            llvm::Value *dynZero64 = llvm::ConstantInt::get(i64, 0);
            llvm::Value *intDivZero = b.CreateAnd(
                b.CreateNot(eitherF, "dyn.not.f"),
                b.CreateICmpEQ(rI, dynZero64, "dyn.rz"), "dyn.idivz");
            llvm::Value *minOverflow = b.CreateAnd(
                b.CreateICmpEQ(lI, llvm::ConstantInt::get(
                                       i64, llvm::APInt::getSignedMinValue(64)), "dyn.lmin"),
                b.CreateICmpEQ(rI, llvm::ConstantInt::getSigned(i64, -1), "dyn.rneg1"),
                "dyn.minov");
            llvm::Value *unsafeDivisor = b.CreateOr(
                eitherF, b.CreateOr(intDivZero, minOverflow, "dyn.iunsafe"), "dyn.unsafe");
            llvm::Value *safeRI = b.CreateSelect(
                unsafeDivisor, llvm::ConstantInt::get(i64, 1), rI, "dyn.safe.divisor");

            // (AR) نحسب النتيجتين العشريّة والصحيحة ثمّ نختار زمنَ التشغيل حسب الوسم؛ يطابق
            //      المفسّر: عشريّ ⇒ fadd/… (و% ⇒ frem أي fmod، // ⇒ floor(fdiv))؛ صحيح ⇒ add/…
            //      (و% ⇒ srem، // ⇒ sdiv). المعاملات الصحيحة تُقرأ من الحمولة مباشرة (نظيفة).
            // (EN) Compute both the float and integer results, then runtime-select by the tag,
            //      matching the interpreter: float ⇒ fadd/… (% ⇒ frem i.e. fmod, // ⇒ floor(fdiv));
            //      int ⇒ add/… (% ⇒ srem, // ⇒ sdiv). Int operands read the payload directly (clean).
            llvm::Value *fRes = nullptr;
            llvm::Value *iRes = nullptr;
            // (AR) ISSUE-063: وسمُ النتيجة قد يتجاوز eitherF لبعض العمليّات (القسمة `/`
            //      على صحيحين بباقٍ ⇒ عشريّ) — يبدأ بـeitherF وتعدّله الحالة عند الحاجة.
            // (EN) ISSUE-063: the result kind may exceed eitherF for some ops (`/` on two
            //      ints with a remainder ⇒ float) — starts as eitherF, cases may extend it.
            llvm::Value *isFloatRes = eitherF;

            switch (op)
            {
            case SIROpcode::ADD_I64:
            case SIROpcode::ADD_F64:
                fRes = b.CreateFAdd(lD, rD, "dyn.fadd");
                iRes = b.CreateAdd(lI, rI, "dyn.iadd");
                break;
            case SIROpcode::SUB_I64:
            case SIROpcode::SUB_F64:
                fRes = b.CreateFSub(lD, rD, "dyn.fsub");
                iRes = b.CreateSub(lI, rI, "dyn.isub");
                break;
            case SIROpcode::MUL_I64:
            case SIROpcode::MUL_F64:
                fRes = b.CreateFMul(lD, rD, "dyn.fmul");
                iRes = b.CreateMul(lI, rI, "dyn.imul");
                break;
            case SIROpcode::DIV_I64:
            case SIROpcode::DIV_F64:
            {
                // (AR) ISSUE-063: دلالة المفسّر للقسمة `/` على صحيحين: صحيح عند انعدام
                //      الباقي (6/3=2) وعشريّ عند وجوده (7/2=3.5). نحسب srem؛ على الفرع
                //      العشريّ safeRI=1 ⇒ الباقي 0 ولا أثر (يحسم eitherF). fdiv(lD,rD)
                //      صحيحٌ في الحالتين لأنّ unpackDouble يرقّي الحمولة الصحيحة sitofp.
                // (EN) ISSUE-063: interpreter semantics for `/` on two ints: int when the
                //      remainder is zero (6/3=2), float otherwise (7/2=3.5). Compute srem;
                //      on the float branch safeRI=1 ⇒ remainder 0, no effect (eitherF wins).
                //      fdiv(lD,rD) is correct either way since unpackDouble sitofp-promotes
                //      integer payloads.
                // (AR) حارس القسمة العشريّة على صفر (د-1، توافق المحرّكين): فرعٌ
                //      عشريّ (eitherF) بمقسومٍ عليه 0.0 كان يُنتج nan/inf بصمت —
                //      نفس بوّابة NaN التي سُدَّت في المسار الساكن (emitDiv). المفسّر
                //      يرمي RUN001؛ هنا نرفض زمنيًّا قبل fdiv: مستضاف ⇒ تشخيص RUN001
                //      العربيّ + exit(1)؛ حرّ ⇒ __sad_panic برمز انتهاك الفحص.
                //      والفرع الصحيح (قاسم صفر صحيح) يُرفض بـRUN001 كذلك — سياسة
                //      النتيجة 0 الصامتة أُلغيت (سدّ تباعد الصحيحين).
                // (EN) Float division-by-zero guard (D-1, engine parity): a float
                //      branch (eitherF) with divisor 0.0 silently produced nan/inf —
                //      the same NaN gate closed on the static path (emitDiv). The
                //      interpreter throws RUN001; reject at runtime before fdiv:
                //      hosted ⇒ Arabic RUN001 diagnostic + exit(1); freestanding ⇒
                //      __sad_panic(check-violation). The integer branch (int zero
                //      divisor) is rejected with RUN001 too — the silent result-0
                //      policy is gone (int divergence closed).
                emitDynDivZeroGuard(
                    cg,
                    b.CreateAnd(eitherF,
                                b.CreateFCmpOEQ(rD, llvm::ConstantFP::get(dbl, 0.0),
                                                "dyn.fdiv.rz"),
                                "dyn.fdivz"),
                    lD, Sad::Compiler::kDivZeroRun001Msg, "dyn.fdiv.dz");
                // (AR) والفرع الصحيح: قاسم صفر ⇒ RUN001 كالمفسّر (كانت سياسة 0 صامتة)
                // (EN) Int branch: zero divisor ⇒ RUN001 like the interpreter
                //      (was a silent result-0 policy)
                emitDynDivZeroGuard(cg, intDivZero, lI,
                                    Sad::Compiler::kDivZeroRun001IntMsg, "dyn.idiv.dz");
                fRes = b.CreateFDiv(lD, rD, "dyn.fdiv");
                iRes = b.CreateSDiv(lI, safeRI, "dyn.idiv");
                // (AR) 🔑 `/` صارت قسمةً صحيحةً باقتطاعٍ نحو الصفر (كـC): النتيجةُ صحيحةٌ
                //      ما لم يكن أحدُ المعاملَين عشريًّا. فسقطَ فحصُ الباقي (srem) ووسمُه
                //      النتيجةَ عشريّةً عند 7/2 — وهو الفحصُ الذي كان يجعل **نوعَ** النتيجة
                //      حقيقةَ زمنِ تشغيل. و`isFloatRes` تبقى `eitherF` كما هُيّئت.
                // (EN) `/` is now C-style truncating integer division: the result is an
                //      integer unless an operand is a float. The remainder test that tagged
                //      7/2 as float is gone — it was what made the result KIND a runtime fact.
                // (AR) وفيضُ INT64_MIN / -1 على الفرعِ الصحيحِ يُرمى RUN011 بدلَ ترقيتِه إلى
                //      عشريّ: الترقيةُ تُعيد النوعَ إلى زمنِ التشغيل، وهي ما أُزيل هنا.
                //      ويُقيَّد بـ`!eitherF` لأنّ lI على الفرعِ العشريّ حمولةٌ مفكوكةٌ قد
                //      تصادف القيمتَين بلا أن تكون قسمةً صحيحةً أصلًا.
                // (EN) INT64_MIN / -1 on the integer branch throws RUN011 instead of being
                //      promoted; gated on !eitherF because lI on the float branch is an
                //      unpacked payload that could coincidentally match.
                emitDynDivZeroGuard(
                    cg,
                    b.CreateAnd(b.CreateNot(eitherF, "dyn.idiv.notf"), minOverflow,
                                "dyn.idiv.ovfc"),
                    lI, Sad::Compiler::kDivOverflowRun011IntMsg, "dyn.idiv.ovf");
                break;
            }
            case SIROpcode::MOD_I64:
                // (AR) % : عشريّ ⇒ frem (fmod، مثل المفسّر 7.5%2=1.5)؛ صحيح ⇒ srem.
                //      حارس القاسم الصفريّ العشريّ ⇒ RUN010 (المفسّر يرمي؛ كان frem
                //      يُنتج NaN بصمت)، والفرع الصحيح ⇒ RUN010 كذلك (كانت سياسة 0 صامتة).
                // (EN) % : float ⇒ frem (fmod, like the interpreter 7.5%2=1.5); int ⇒ srem.
                //      Float zero-divisor guard ⇒ RUN010 (interpreter throws; frem
                //      silently produced NaN); int branch ⇒ RUN010 too (was silent 0).
                emitDynDivZeroGuard(
                    cg,
                    b.CreateAnd(eitherF,
                                b.CreateFCmpOEQ(rD, llvm::ConstantFP::get(dbl, 0.0),
                                                "dyn.frem.rz"),
                                "dyn.fremz"),
                    lD, Sad::Compiler::kModZeroRun010Msg, "dyn.frem.dz");
                emitDynDivZeroGuard(cg, intDivZero, lI,
                                    Sad::Compiler::kModZeroRun010IntMsg, "dyn.srem.dz");
                fRes = b.CreateFRem(lD, rD, "dyn.frem");
                iRes = b.CreateSRem(lI, safeRI, "dyn.srem");
                break;
            case SIROpcode::FLOOR_DIV_I64:
            {
                // (AR) // : عشريّ ⇒ floor(fdiv) (مثل المفسّر 7.5//2=3.0)؛ صحيح ⇒ sdiv.
                // (EN) // : float ⇒ floor(fdiv) (like the interpreter 7.5//2=3.0); int ⇒ sdiv.
                // (AR) حارس القاسم الصفريّ العشريّ ⇒ RUN009 (المفسّر يرمي؛ كان
                //      fdiv/floor يُنتج nan/inf بصمت)، والفرع الصحيح ⇒ RUN009 كذلك.
                // (EN) Float zero-divisor guard ⇒ RUN009 (interpreter throws;
                //      fdiv/floor silently produced nan/inf); int branch ⇒ RUN009 too.
                emitDynDivZeroGuard(
                    cg,
                    b.CreateAnd(eitherF,
                                b.CreateFCmpOEQ(rD, llvm::ConstantFP::get(dbl, 0.0),
                                                "dyn.fd.rz"),
                                "dyn.fdz"),
                    lD, Sad::Compiler::kFloorDivZeroRun009Msg, "dyn.ffd.dz");
                emitDynDivZeroGuard(cg, intDivZero, lI,
                                    Sad::Compiler::kFloorDivZeroRun009IntMsg,
                                    "dyn.ifd.dz");
                llvm::Value *q = b.CreateFDiv(lD, rD, "dyn.fdiv.q");
                llvm::Function *floorFn = llvm::Intrinsic::getDeclaration(
                    cg.module_.get(), llvm::Intrinsic::floor, {dbl});
                fRes = b.CreateCall(floorFn, {q}, "dyn.floor");
                // (AR) Amelia (ISSUE-063): تسويةٌ أرضيّة للفرع الصحيح (-7//2=-4 كالمفسّر)
                //      بدل اقتطاع sdiv نحو الصفر (-3).
                // (EN) Amelia (ISSUE-063): floor adjustment on the integer branch
                //      (-7//2=-4 like the interpreter) instead of sdiv truncation (-3).
                llvm::Value *iq = b.CreateSDiv(lI, safeRI, "dyn.sdiv");
                llvm::Value *irem = b.CreateSRem(lI, safeRI, "dyn.sdiv.rem");
                llvm::Value *signsDiffer = b.CreateICmpSLT(
                    b.CreateXor(lI, safeRI, "dyn.fd.sx"), dynZero64, "dyn.fd.sd");
                llvm::Value *inexactI = b.CreateICmpNE(irem, dynZero64, "dyn.fd.ix");
                llvm::Value *needAdj = b.CreateAnd(signsDiffer, inexactI, "dyn.fd.na");
                iRes = b.CreateSub(iq, b.CreateZExt(needAdj, i64, "dyn.fd.adj"), "dyn.fd.q");
                // (AR) INT64_MIN // -1 يفيض ⇒ المفسّر يرقّيه إلى عشريّ (9223372036854775808.0)
                //      كالقسمة `/`. fRes = floor(fdiv(lD,rD)) يحسبه؛ نضمّ minOverflow لوسمه عشريًّا.
                // (EN) INT64_MIN // -1 overflows ⇒ the interpreter promotes to float
                //      (9223372036854775808.0) like `/`. fRes = floor(fdiv(lD,rD)) yields it;
                //      OR in minOverflow to tag the result float.
                isFloatRes = b.CreateOr(eitherF, minOverflow, "dyn.fd.isf");
                break;
            }
            case SIROpcode::BUILTIN_POW:
            {
                // (AR) الأسُّ يمرُّ بالبابِ الواحدِ كسائرِ الحسابيّات: معاملٌ موسومٌ
                //      (قراءةُ خريطةٍ بالقوس مثلًا) كان يبلغُ emitBuiltinPow بنيةً
                //      %SadDyn فيسقطُ CreateSIToFP بتأكيدِ «Invalid cast!».
                //      unpackDouble يرقّي الحمولةَ الصحيحةَ sitofp، فنداءُ pow واحدٌ
                //      يكفي الفرعَين؛ الصحيحان يُقتطعان صحيحًا كالمفسّر (5**2=25)
                //      **داخل مدى i64 فقط**: خارجَه FPToSI سُمٌّ (poison)، والمفسّرُ
                //      يرقّي الفيضَ عشريًّا (2**100) — فيُوسَّع isFloatRes بفحصِ
                //      المدى على نمطِ minOverflow في ذراعِ FLOOR_DIV.
                // (EN) Power routes through the single door like the rest of the
                //      arithmetic: a tagged operand (e.g. bracket map read) used to
                //      reach emitBuiltinPow as a %SadDyn struct and CreateSIToFP
                //      died on the "Invalid cast!" assert. unpackDouble already
                //      promotes an integer payload via sitofp, so one pow call
                //      serves both branches; two ints truncate back to i64 like the
                //      interpreter **inside the i64 range only**: outside it FPToSI
                //      is poison while the interpreter promotes overflow to float
                //      (2**100) — so isFloatRes widens with a range check, the
                //      FLOOR_DIV minOverflow pattern.
                llvm::FunctionType *powTy = llvm::FunctionType::get(dbl, {dbl, dbl}, false);
                llvm::FunctionCallee powFn = cg.module_->getOrInsertFunction("pow", powTy);
                fRes = b.CreateCall(powFn, {lD, rD}, "dyn.fpow");
                iRes = b.CreateFPToSI(fRes, i64, "dyn.ipow");
                llvm::Value *powTooBig = b.CreateFCmpOGE(
                    fRes, llvm::ConstantFP::get(dbl, 9223372036854775808.0), "dyn.pow.hi");
                llvm::Value *powTooSmall = b.CreateFCmpOLT(
                    fRes, llvm::ConstantFP::get(dbl, -9223372036854775808.0), "dyn.pow.lo");
                isFloatRes = b.CreateOr(
                    eitherF, b.CreateOr(powTooBig, powTooSmall, "dyn.pow.ov"), "dyn.pow.isf");
                break;
            }
            default:
                fRes = b.CreateFAdd(lD, rD, "dyn.fadd.def");
                iRes = b.CreateAdd(lI, rI, "dyn.iadd.def");
                break;
            }

            llvm::Value *resKind = b.CreateSelect(
                isFloatRes, llvm::ConstantInt::get(i8, DynKind::Float),
                llvm::ConstantInt::get(i8, DynKind::Int), "dyn.res.kind");
            llvm::Value *fBits = b.CreateBitCast(fRes, i64, "dyn.res.fbits");
            llvm::Value *resPayload = b.CreateSelect(isFloatRes, fBits, iRes, "dyn.res.payload");
            return makeDyn(cg, resKind, resPayload);
        }

        llvm::Value *dynCompare(LLVMCodeGen &cg, DynCmp cmp, llvm::Value *l, llvm::Value *r)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *i1 = llvm::Type::getInt1Ty(ctx);
            auto *i32 = llvm::Type::getInt32Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            // (AR) م1ب ISSUE-076: كلا المعامِلين نصّ (وسم Str) ⇒ قارِن بالمحتوى عبر strcmp لا
            //      الحمولةَ (المؤشّرات — عناوين malloc تختلف لنصّين متطابقين). فرعٌ زمنَ التشغيل
            //      لأنّ strcmp غير آمن على حمولةٍ غير مؤشّريّة (صحيح/عشريّ). يخدم كامل عائلة %SadDyn:
            //      EQ/NE (عبر emitDynamicEqNe) وLT/LE/GT/GE (عبر emitCmpLt…) يوحّدان الطرفين هنا.
            // (EN) م1ب ISSUE-076: both operands are strings (Str tag) ⇒ compare by content via strcmp,
            //      not the payload (pointers — distinct malloc addresses for equal strings). A runtime
            //      branch since strcmp is unsafe on a non-pointer payload (int/float). Serves the whole
            //      %SadDyn family: EQ/NE (via emitDynamicEqNe) and LT/LE/GT/GE (via emitCmpLt…) unify here.
            llvm::Value *strK = llvm::ConstantInt::get(i8, DynKind::Str);
            llvm::Value *lKind = dynKindByte(cg, l);
            llvm::Value *rKind = dynKindByte(cg, r);

            // (AR) «عدميّةٌ» زمنَ التشغيل: وسمُ Null، أو وسمُ Str وحمولتُه حارسُ العدم.
            //      الثانيةُ ليست احتياطًا نظريًّا: خانةٌ نوعُها المُصرَّحُ «نصّ» تُعبّأ عدمًا
            //      في موقعٍ ونصًّا في آخر، فتُغلَّف بوسمِ Str وحمولةِ الحارس. ولولا هذا
            //      الفحصُ لذهب المسارُ إلى strcmp على المؤشرِ الحارسِ فسقط البرنامجُ
            //      بـSIGSEGV — وهو ما يحرسه هذا الشرطُ **قبل** التفرّع لا بعده.
            // (EN) Runtime "nullishness": the Null tag, or the Str tag with the null sentinel
            //      as payload. The second case is not hypothetical: a slot declared `string`
            //      may be filled with null at one site and a string at another, so it boxes as
            //      Str-tagged with the sentinel payload. Without this test the path would reach
            //      strcmp on the sentinel pointer and the program would die with SIGSEGV — which
            //      this condition guards **before** the branch, not after it.
            llvm::Value *sentinel =
                llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), Sad::Compiler::kSadNullSentinel);
            //      والحكمُ على **الحمولةِ** أيًّا كان الوسم، لا على وسمِ Null وحدَه: حارسُ
            //      العدمِ قيمةٌ محجوزةٌ على مستوى اللغةِ كلِّها (kSadNullSentinel)، والتغليفُ
            //      لا يضع وسمَ Null دائمًا — مقيسًا: حرفيّةُ «لاشيء» في ذراعِ `طابق` تُغلَّف
            //      وسمًا صحيحًا وحمولتُها الحارس. فلو حكَمنا بالوسمِ وحدَه لصار
            //      `طابق(لاشيء)` لا يطابق `عندما لاشيء` — عدمٌ لا يساوي عدمًا.
            //      والثمنُ المُعلَن: عددٌ قيمتُه الحارسُ نفسُه يُعدّ عدمًا. وهو ثمنُ التمثيلِ
            //      القائمِ لا ثمنُ هذا الموضع: `normaliseToSentinelWidth` في مقارنةِ الأنواعِ
            //      الساكنةِ تحكم بالحكمِ نفسِه، فالموضعان متّسقان.
            // (EN) The test is on the **payload** whatever the tag, not on the Null tag alone:
            //      the null sentinel is a language-wide reserved value (kSadNullSentinel), and
            //      boxing does not always attach the Null tag — measured: the «null» literal in a
            //      `match` arm is boxed with the Int tag and the sentinel as its payload. Judging
            //      by the tag alone would make `match(null)` fail to match `case null` — null not
            //      equal to null.
            //      Declared cost: an integer whose value is the sentinel itself counts as null.
            //      That is the cost of the existing representation, not of this site:
            //      `normaliseToSentinelWidth` on the static-type path makes the same judgement,
            //      so the two are consistent.
            auto isNullish = [&](llvm::Value *v, llvm::Value *kind, const char *tag) {
                llvm::Value *tagIsNull =
                    b.CreateICmpEQ(kind, llvm::ConstantInt::get(i8, DynKind::Null),
                                   std::string("dyn.cmp.") + tag + ".tagnull");
                // (AR) والحمولةُ تُفحَص للوسمِ النصّيِّ وحدَه: هناك الحارسُ **مؤشّرٌ**
                //      لا عدد، وبلوغُه strcmp موتٌ بـSIGSEGV، فالفحصُ يقي عطبًا حقيقيًّا.
                //      أمّا وسمُ العددِ فحمولتُه عددٌ يعنيه المستعمل: فحصُها هناك كان
                //      يجعل ‎-9223372036854775807 (وهو مدخلُ جيسونَ صالحٌ عند حدِّ i64)
                //      يساوي «لاشيء» ويُطبَع `null` — جوابٌ خاطئٌ صامتٌ بخروجٍ ٠.
                //      والعدمُ المغلَّفُ صار موسومًا Null عند التغليفِ (toDyn)، فلا حاجةَ
                //      إلى استنتاجِه من حمولةِ عدد.
                // (EN) The payload is tested for the Str tag only: there the sentinel is a
                //      pointer, and reaching strcmp with it is a SIGSEGV, so the test prevents a
                //      real defect. Under the Int tag the payload is a number the user meant:
                //      testing it there made -9223372036854775807 (a valid JSON input at the i64
                //      boundary) equal null and print `null` — a silent wrong answer with exit 0.
                //      Boxed nulls now carry the Null tag from toDyn, so no inference is needed.
                llvm::Value *payloadIsSentinel = b.CreateAnd(
                    b.CreateICmpEQ(kind, strK, std::string("dyn.cmp.") + tag + ".sent.isstr"),
                    b.CreateICmpEQ(dynPayloadI64(cg, v), sentinel,
                                   std::string("dyn.cmp.") + tag + ".sent.val"),
                    std::string("dyn.cmp.") + tag + ".sent");
                return b.CreateOr(tagIsNull, payloadIsSentinel,
                                  std::string("dyn.cmp.") + tag + ".nullish");
            };
            llvm::Value *lNullish = isNullish(l, lKind, "l");
            llvm::Value *rNullish = isNullish(r, rKind, "r");
            llvm::Value *eitherNullish = b.CreateOr(lNullish, rNullish, "dyn.cmp.either.null");
            llvm::Value *bothNullish = b.CreateAnd(lNullish, rNullish, "dyn.cmp.both.null");

            llvm::Value *bothStr = b.CreateAnd(
                b.CreateAnd(b.CreateICmpEQ(lKind, strK, "dyn.cmp.lstr"),
                            b.CreateICmpEQ(rKind, strK, "dyn.cmp.rstr"),
                            "dyn.cmp.bothstr.tags"),
                b.CreateNot(eitherNullish, "dyn.cmp.neither.null"),
                "dyn.cmp.bothstr");

            auto *parent = b.GetInsertBlock()->getParent();
            auto *strBB = llvm::BasicBlock::Create(ctx, "dyn.cmp.str", parent);
            auto *numBB = llvm::BasicBlock::Create(ctx, "dyn.cmp.num", parent);
            auto *mergeBB = llvm::BasicBlock::Create(ctx, "dyn.cmp.merge", parent);
            b.CreateCondBr(bothStr, strBB, numBB);

            // (AR) فرع النصّ: strcmp(المحتوى، المحتوى) ثمّ قارِن الناتج بالصفر حسب المُقارِن
            //      (نفس اصطلاح strcmp المستعمل في emitCmpEq للنصوص العاديّة). / (EN) String branch:
            //      strcmp(content, content), then compare the result to zero per the operator.
            b.SetInsertPoint(strBB);
            llvm::Value *lp = unpackPtr(cg, l);
            llvm::Value *rp = unpackPtr(cg, r);
            auto *strcmpTy = llvm::FunctionType::get(i32, {ptrTy, ptrTy}, false);
            auto strcmpFn = cg.module_->getOrInsertFunction("strcmp", strcmpTy);
            llvm::Value *sc = b.CreateCall(strcmpFn, {lp, rp}, "dyn.cmp.strcmp");
            llvm::Value *z32 = llvm::ConstantInt::get(i32, 0);
            llvm::Value *strRes = nullptr;
            switch (cmp)
            {
            case DynCmp::EQ: strRes = b.CreateICmpEQ(sc, z32, "dyn.cmp.seq"); break;
            case DynCmp::NE: strRes = b.CreateICmpNE(sc, z32, "dyn.cmp.sne"); break;
            case DynCmp::LT: strRes = b.CreateICmpSLT(sc, z32, "dyn.cmp.slt"); break;
            case DynCmp::LE: strRes = b.CreateICmpSLE(sc, z32, "dyn.cmp.sle"); break;
            case DynCmp::GT: strRes = b.CreateICmpSGT(sc, z32, "dyn.cmp.sgt"); break;
            case DynCmp::GE: strRes = b.CreateICmpSGE(sc, z32, "dyn.cmp.sge"); break;
            }
            b.CreateBr(mergeBB);
            strBB = b.GetInsertBlock();

            // (AR) فرع العدد (المنطق الأصليّ محفوظًا حرفيًّا): عشريّ⇒fcmp، صحيح⇒icmp موقَّع على الحمولة.
            // (EN) Numeric branch (original logic preserved verbatim): float⇒fcmp, int⇒signed icmp on payload.
            b.SetInsertPoint(numBB);
            llvm::Value *eitherF = b.CreateOr(isFloatKind(cg, l), isFloatKind(cg, r), "dyn.cmp.either.f");
            llvm::Value *lD = unpackDouble(cg, l);
            llvm::Value *rD = unpackDouble(cg, r);
            llvm::Value *lI = dynPayloadI64(cg, l);
            llvm::Value *rI = dynPayloadI64(cg, r);

            llvm::Value *fRes = nullptr;
            llvm::Value *iRes = nullptr;
            switch (cmp)
            {
            case DynCmp::EQ:
                fRes = b.CreateFCmpOEQ(lD, rD, "dyn.feq");
                iRes = b.CreateICmpEQ(lI, rI, "dyn.ieq");
                break;
            case DynCmp::NE:
                fRes = b.CreateFCmpONE(lD, rD, "dyn.fne");
                iRes = b.CreateICmpNE(lI, rI, "dyn.ine");
                break;
            case DynCmp::LT:
                fRes = b.CreateFCmpOLT(lD, rD, "dyn.flt");
                iRes = b.CreateICmpSLT(lI, rI, "dyn.ilt");
                break;
            case DynCmp::LE:
                fRes = b.CreateFCmpOLE(lD, rD, "dyn.fle");
                iRes = b.CreateICmpSLE(lI, rI, "dyn.ile");
                break;
            case DynCmp::GT:
                fRes = b.CreateFCmpOGT(lD, rD, "dyn.fgt");
                iRes = b.CreateICmpSGT(lI, rI, "dyn.igt");
                break;
            case DynCmp::GE:
                fRes = b.CreateFCmpOGE(lD, rD, "dyn.fge");
                iRes = b.CreateICmpSGE(lI, rI, "dyn.ige");
                break;
            }
            llvm::Value *numRes = b.CreateSelect(eitherF, fRes, iRes, "dyn.cmp.res");
            b.CreateBr(mergeBB);
            numBB = b.GetInsertBlock();

            // (AR) الدمج: النتيجة من فرع النصّ أو العدد. / (EN) Merge: result from the string or numeric branch.
            b.SetInsertPoint(mergeBB);
            auto *phi = b.CreatePHI(i1, 2, "dyn.cmp.merge.res");
            phi->addIncoming(strRes, strBB);
            phi->addIncoming(numRes, numBB);

            // (AR) بوّابةُ الوسمِ للمساواةِ وحدَها.
            //
            //      الفرعُ العدديُّ أعلاه يقارن **الحمولةَ** دون سؤالٍ عن الوسم، فكان
            //      «أي» يحملُ العددَ ١ يساوي «صحيح» (حمولةُ المنطقيِّ ١)، و«أي» يحملُ ٠
            //      يساوي «خطأ». والمفسّرُ يقول في الحالتَين «خطأ»: المنطقيُّ نوعٌ قائمٌ
            //      بذاته لا عددٌ متنكّر. وهذا هو الذي كان يجعل مولّدَ جيسون يُخرِج
            //      {"أ":true} لقيمةٍ عددُها ١ — جيسونٌ **صالحُ الشكلِ كاذبُ المعنى**،
            //      وهو أسوأُ من انهيارٍ لأنّه يمرّ صامتًا.
            //
            //      عقدُ المفسّرِ (evaluateComparisonOp) حرفًا:
            //        · عدمٌ يساوي عدمًا فقط — أيًّا كان نوعُ الطرفِ الآخر.
            //        · فراغٌ يساوي فراغًا فقط.
            //        · عددان مختلفا الوسمِ (صحيحٌ/عشريّ) يُرقَّيان ويُقارَنان.
            //        · وسمان مختلفان بعدَ ذلك ⇒ غيرُ متساويَين، بلا مقارنةِ محتوى.
            //      والترتيبُ خارجٌ عن هذه البوّابةِ عمدًا: المفسّرُ يرفضه على الأنواعِ
            //      المختلطةِ بخطأٍ لا بجواب، فمحاكاتُه هنا تحتاج تشخيصًا زمنَ التشغيلِ
            //      لا انتقاءَ قيمة.
            // (EN) Tag gate, for equality only.
            //
            //      The numeric branch above compares the **payload** without asking about the
            //      tag, so an `any` holding the integer 1 equalled `true` (whose bool payload is
            //      1), and an `any` holding 0 equalled `false`. The interpreter says false in
            //      both cases: bool is its own type, not an integer in disguise. This is what
            //      made the JSON generator emit an object member as `true` for a value that was
            //      the integer 1 — well-formed JSON with the wrong meaning, which is worse than a
            //      crash because it passes silently.
            //
            //      The interpreter's contract (evaluateComparisonOp), verbatim:
            //        - null equals only null, whatever the other side's type.
            //        - void equals only void.
            //        - two numerics of differing tags (int/float) are promoted and compared.
            //        - any other tag mismatch => not equal, with no content comparison.
            //      Ordering is deliberately outside this gate: the interpreter rejects it on
            //      mixed types with an error rather than an answer, so mirroring it here needs a
            //      runtime diagnostic, not a value selection.
            if (cmp != DynCmp::EQ && cmp != DynCmp::NE)
                return phi;

            llvm::Value *voidK = llvm::ConstantInt::get(i8, DynKind::Void);
            llvm::Value *lVoid = b.CreateICmpEQ(lKind, voidK, "dyn.cmp.l.void");
            llvm::Value *rVoid = b.CreateICmpEQ(rKind, voidK, "dyn.cmp.r.void");
            llvm::Value *eitherVoid = b.CreateOr(lVoid, rVoid, "dyn.cmp.either.void");
            llvm::Value *bothVoid = b.CreateAnd(lVoid, rVoid, "dyn.cmp.both.void");

            auto isNumericKind = [&](llvm::Value *kind, const char *tag) {
                return b.CreateOr(
                    b.CreateICmpEQ(kind, llvm::ConstantInt::get(i8, DynKind::Int),
                                   std::string("dyn.cmp.") + tag + ".isint"),
                    b.CreateICmpEQ(kind, llvm::ConstantInt::get(i8, DynKind::Float),
                                   std::string("dyn.cmp.") + tag + ".isflt"),
                    std::string("dyn.cmp.") + tag + ".isnum");
            };
            llvm::Value *bothNumeric = b.CreateAnd(isNumericKind(lKind, "l"),
                                                   isNumericKind(rKind, "r"),
                                                   "dyn.cmp.both.num");
            llvm::Value *sameKind = b.CreateICmpEQ(lKind, rKind, "dyn.cmp.same.kind");
            llvm::Value *comparable = b.CreateOr(sameKind, bothNumeric, "dyn.cmp.comparable");

            const bool wantEqual = (cmp == DynCmp::EQ);
            llvm::Value *mismatchAnswer = llvm::ConstantInt::get(i1, wantEqual ? 0 : 1);
            llvm::Value *voidAnswer = wantEqual ? bothVoid : b.CreateNot(bothVoid, "dyn.cmp.void.ne");
            llvm::Value *nullAnswer =
                wantEqual ? bothNullish : b.CreateNot(bothNullish, "dyn.cmp.null.ne");

            llvm::Value *gated =
                b.CreateSelect(comparable, phi, mismatchAnswer, "dyn.cmp.gated.kind");
            gated = b.CreateSelect(eitherVoid, voidAnswer, gated, "dyn.cmp.gated.void");
            gated = b.CreateSelect(eitherNullish, nullAnswer, gated, "dyn.cmp.gated.null");
            return gated;
        }

        llvm::StructType *sadArrayStructType(llvm::LLVMContext &ctx)
        {
            // (AR) نوعٌ مسمّى: LLVM يوحّد الاسمَ داخل السياق، فالنداءُ المتكرّر يعيد النوعَ نفسَه
            //      بلا حاجةٍ إلى مخبّأٍ ساكن (والمخبّأُ الساكنُ كان يحمل نوعًا من سياقٍ ميّتٍ لو
            //      تعدّدت السياقات).
            // (EN) A named type: LLVM uniques names within a context, so repeated calls return
            //      the same type without a function-local static cache (which would hold a type
            //      belonging to a dead context if several contexts ever existed).
            if (llvm::StructType *existing = llvm::StructType::getTypeByName(ctx, "SadArray"))
            {
                // (AR) الاسمُ وحدَه ليس عقدًا: لو أنشأ مسارٌ آخرُ نوعًا بهذا الاسمِ وتخطيطٍ
                //      مغاير (أو مبهمًا بلا جسم) لَتبنّيناه صامتين، فصارت `StructGEP(…،0)`
                //      تقرأ حقلًا غيرَ الطول ⇒ إفسادُ ذاكرةٍ في ذراعِ المصفوفةِ نفسِها.
                //      نُثبّت الحقولَ الخمسةَ صراحةً بدل الثقةِ بالاسم.
                // (EN) The name alone is not the contract: if another path created a type of
                //      the same name with a different (or opaque) layout we would silently
                //      adopt it, making StructGEP(…,0) read a field that is not the length —
                //      memory corruption inside the array arm itself. Assert the five fields.
                assert(!existing->isOpaque() && existing->getNumElements() == 5 &&
                       "SadArray: تخطيطٌ مغايرٌ بالاسمِ نفسِه / layout mismatch under the same name");
                return existing;
            }
            return llvm::StructType::create(ctx,
                                            {
                                                llvm::Type::getInt64Ty(ctx),       // length
                                                llvm::Type::getInt64Ty(ctx),       // capacity
                                                llvm::PointerType::getUnqual(ctx), // data pointer
                                                llvm::PointerType::getUnqual(ctx), // tags (i8*) or null [option A]
                                                llvm::Type::getInt8Ty(ctx)         // homogKind [option A2]: read only when tags==null
                                            },
                                            "SadArray");
        }

        llvm::Function *ensureDynToStringFn(LLVMCodeGen &cg)
        {
            auto &ctx = *cg.context_;
            auto *i64 = llvm::Type::getInt64Ty(ctx);
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *dbl = llvm::Type::getDoubleTy(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);

            // (AR) الحارسُ نفسُه المستعمَلُ في ensureArrayToStringDynHelper: «موجودةٌ ولها جسم».
            //      وهو طرفُ الحلقةِ الذي يوقف العَوْدَ المتبادلَ بين المولّدَين.
            // (EN) The same guard ensureArrayToStringDynHelper uses. It is the end of the loop
            //      that stops the mutual recursion between the two generators.
            if (llvm::Function *existing = cg.module_->getFunction("__sad_dyn_to_string"))
                if (!existing->empty())
                    return existing;

            auto *fnTy = llvm::FunctionType::get(ptrTy, {i8, i64}, false);
            auto *fn = llvm::Function::Create(fnTy, llvm::Function::InternalLinkage,
                                              "__sad_dyn_to_string", cg.module_.get());
            fn->getArg(0)->setName("kind");
            fn->getArg(1)->setName("payload");
            llvm::Value *kind = fn->getArg(0);
            llvm::Value *payload = fn->getArg(1);

            llvm::BasicBlock *savedBB = cg.builder_->GetInsertBlock();
            llvm::BasicBlock::iterator savedPoint = cg.builder_->GetInsertPoint();

            // (AR) ⚠️ ترتيبٌ حَرِج: كتلةُ entry تُنشأ **الآن**، قبل أيِّ نداءٍ للمساعِدِ المصفوفيّ.
            //      وإلّا رآنا ذاك المساعِدُ «فارغين» فأعاد توليدَنا ⇒ حلقةُ ترجمةٍ لا نهائيّة.
            // (EN) ⚠️ Order is critical: the entry block is created NOW, before any call to the
            //      array helper — otherwise that helper would see us "empty" and regenerate us,
            //      an infinite compile-time loop.
            auto *entryBB = llvm::BasicBlock::Create(ctx, "entry", fn);
            auto &b = *cg.builder_;
            b.SetInsertPoint(entryBB);

            // (AR) مخزّنُ الصحيح/العشريّ (512 لـ%.6f لـDBL_MAX). يُخصَّص **داخلَ ذراعِه** لا في
            //      المدخل: الأذرعُ الأخرى (منطقيّ/نصّ/مصفوفة/عدم) لا تلمسه، وتخصيصُه مقدَّمًا كان
            //      يُهدر ٥١٢ بايتًا في كلِّ نداء. وقد تضاعف الأثرُ بذراعِ المصفوفة: المساعِدُ
            //      المصفوفيُّ ينادينا **مرّتين لكلِّ عنصر** (تمريرتا القياسِ والملء) ⇒ الإهدارُ
            //      يتناسب مع طولِ المصفوفةِ وعمقِ تعشيشِها. صار الفصلُ ممكنًا لأنّ الأذرعَ كتلٌ
            //      مستقلّةٌ بعد تحويلِ الموسِّع إلى دالّة.
            // (EN) The int/float buffer is now allocated INSIDE its own arm rather than in the
            //      entry block: the other arms never touch it, so a prologue allocation wasted
            //      512 bytes per call — amplified by the new array arm, whose helper calls us
            //      twice per element (sizing + fill), scaling with array length and nesting
            //      depth. Splitting it became possible once the expander became a function.
            llvm::Type *szTy = cg.getSizeType();
            auto *mallocTy = llvm::FunctionType::get(ptrTy, {szTy}, false);
            auto mallocFn = cg.module_->getOrInsertFunction("malloc", mallocTy);
            auto emitScratchBuffer = [&]() -> llvm::Value * {
                return b.CreateCall(mallocFn, {llvm::ConstantInt::get(szTy, 512)}, "dyn.ts.buf");
            };

            auto *parent = fn;
            auto *intBB = llvm::BasicBlock::Create(ctx, "dyn.ts.int", parent);
            auto *floatBB = llvm::BasicBlock::Create(ctx, "dyn.ts.float", parent);
            auto *boolBB = llvm::BasicBlock::Create(ctx, "dyn.ts.bool", parent);
            auto *strBB = llvm::BasicBlock::Create(ctx, "dyn.ts.str", parent);
            auto *nullBB = llvm::BasicBlock::Create(ctx, "dyn.ts.null", parent);
            auto *mergeBB = llvm::BasicBlock::Create(ctx, "dyn.ts.merge", parent);

            // (AR) [ز.٢١-أ] ذراعُ المصفوفة يحتاج malloc/sprintf داخل __sad_array_to_string_dyn،
            //      وهما غيرُ متاحين في الوضع الحرّ. فيُحذَف الذراعُ هناك ويبقى السقوطُ إلى
            //      «لاشيء» كما كان — لا انحدار، لأنّ طباعةَ المصفوفةِ المختلطةِ معطّلةٌ حرًّا أصلًا.
            // (EN) [ز.٢١-أ] The array arm needs malloc/sprintf inside __sad_array_to_string_dyn,
            //      neither available freestanding. There the arm is omitted and the old fallback
            //      to «لاشيء» stands — no regression, since heterogeneous-array printing is
            //      already unavailable freestanding.
            llvm::BasicBlock *arrayBB =
                cg.freestanding_ ? nullptr : llvm::BasicBlock::Create(ctx, "dyn.ts.array", parent);
            // (AR) ز.٤٣: ذراعُ الخريطة — نظيرُ ذراعِ المصفوفةِ حرفيًّا. بدونها يسقط وسمُ
            //      الخريطةِ إلى `nullBB` فيُطبع «لاشيء» بينما المفسّرُ يطبع `{أ: ١}`،
            //      وهو تباعُدٌ **أحدثَه** إدخالُ الخريطةِ مسارَ الوسم: `نوع()` يقول
            //      «خريطة» و`نص()` يقول «لاشيء» — تناقُضٌ داخليٌّ لا يُقبَل. والمساعِدُ
            //      `__sad_map_to_string` قائمٌ أصلًا (تستعمله الطباعةُ الساكنة).
            //      مستضافٌ فقط كالمصفوفة: يحتاج malloc/sprintf.
            // (EN) ز.٤٣: the map arm — a literal sibling of the array arm. Without it the
            //      Map tag falls to nullBB and prints «لاشيء» while the interpreter prints
            //      `{أ: ١}` — a divergence *introduced* by routing maps through the tag
            //      path: نوع() says «خريطة» while نص() says «لاشيء», an internal
            //      contradiction. The helper __sad_map_to_string already exists (used by
            //      static printing). Hosted-only, like the array arm: needs malloc/sprintf.
            llvm::BasicBlock *mapBB =
                cg.freestanding_ ? nullptr : llvm::BasicBlock::Create(ctx, "dyn.ts.map", parent);
            // (AR) [ISSUE-047] الكائن: كان وسمُه يسقطُ إلى default فيُعرَض «لاشيء» —
            //      كذبٌ على قيمةٍ موجودة. عرضٌ مُعتِمٌ من مصدرِ الحقيقةِ بدلَه (لا
            //      ترويسةَ أنواعٍ للكائنِ المترجَمِ فلا سبيلَ لاسمِ صنفِه). بلا
            //      malloc، فالذراعُ صالحةٌ للوضعِ الحرِّ أيضًا.
            // (EN) [ISSUE-047] Object: its kind used to fall to default and render
            //      «لاشيء» — a lie about a present value. Opaque SoT display instead
            //      (a compiled object has no runtime type header, so its class name
            //      is unreachable). No malloc, so the arm is freestanding-safe.
            llvm::BasicBlock *objBB = llvm::BasicBlock::Create(ctx, "dyn.ts.obj", parent);

            llvm::SwitchInst *sw = b.CreateSwitch(kind, nullBB, arrayBB ? 7 : 5);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Int), intBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Float), floatBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Bool), boolBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Str), strBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Obj), objBB);
            if (arrayBB)
                sw->addCase(llvm::ConstantInt::get(i8, DynKind::Array), arrayBB);
            if (mapBB)
                sw->addCase(llvm::ConstantInt::get(i8, DynKind::Map), mapBB);

            // (AR) صحيح: %lld / (EN) int: %lld
            b.SetInsertPoint(intBB);
            llvm::Value *buf = emitScratchBuffer();
            if (cg.freestanding_)
            {
                auto *itoaTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), {ptrTy, i64}, false);
                auto itoaFn = cg.module_->getOrInsertFunction("__sad_itoa", itoaTy);
                b.CreateCall(itoaFn, {buf, payload});
            }
            else
            {
                auto *sprintfTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), {ptrTy, ptrTy}, true);
                auto sprintfFn = cg.module_->getOrInsertFunction("sprintf", sprintfTy);
                llvm::Value *ifmt = b.CreateGlobalStringPtr("%lld", "dyn.ts.ifmt");
                b.CreateCall(sprintfFn, {buf, ifmt, payload});
            }
            llvm::Value *intRes = buf;
            b.CreateBr(mergeBB);
            intBB = b.GetInsertBlock();

            // (AR) عشريّ: __sad_format_double(bitcast(payload)) / (EN) float
            b.SetInsertPoint(floatBB);
            llvm::Value *fbuf = emitScratchBuffer();
            llvm::Value *fdbl = b.CreateBitCast(payload, dbl, "dyn.ts.fbc");
            if (cg.freestanding_)
            {
                auto *ftoaTy = llvm::FunctionType::get(llvm::Type::getInt32Ty(ctx), {ptrTy, dbl}, false);
                auto ftoaFn = cg.module_->getOrInsertFunction("__sad_ftoa", ftoaTy);
                b.CreateCall(ftoaFn, {fbuf, fdbl});
            }
            else
            {
                auto *fmtDblTy = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {ptrTy, dbl}, false);
                auto fmtDblFn = cg.module_->getOrInsertFunction("__sad_format_double", fmtDblTy);
                b.CreateCall(fmtDblFn, {fbuf, fdbl});
            }
            llvm::Value *floatRes = fbuf;
            b.CreateBr(mergeBB);
            floatBB = b.GetInsertBlock();

            // (AR) منطقيّ: صحيح/خطأ / (EN) bool: صحيح/خطأ
            b.SetInsertPoint(boolBB);
            llvm::Value *bcond = b.CreateICmpNE(payload, llvm::ConstantInt::get(i64, 0), "dyn.ts.bc");
            // (AR) عرضٌ من مصدرِ الحقيقة الموحَّد (value_repr.yaml / Sad::Types::repr) — لا حرفيّاتٌ خام.
            llvm::Value *trueStr = b.CreateGlobalStringPtr(::Sad::Types::repr::kBoolTrueDisplay, "dyn.ts.true");
            llvm::Value *falseStr = b.CreateGlobalStringPtr(::Sad::Types::repr::kBoolFalseDisplay, "dyn.ts.false");
            llvm::Value *boolRes = b.CreateSelect(bcond, trueStr, falseStr, "dyn.ts.bstr");
            b.CreateBr(mergeBB);
            boolBB = b.GetInsertBlock();

            // (AR) نصّ: inttoptr(payload) مع حماية null / (EN) str: inttoptr with null guard
            b.SetInsertPoint(strBB);
            llvm::Value *strPtr = b.CreateIntToPtr(payload, ptrTy, "dyn.ts.strp");
            llvm::Value *strNull = b.CreateICmpEQ(
                strPtr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "dyn.ts.strnull");
            // (AR) عرضُ مؤشّرِ النصِّ الفارغ: من المصدرِ الموحَّد لا حرفيّةَ "void" الإنجليزيّة —
            //      كانت تسرّب عرضًا إنجليزيًّا للمستخدم يخالف «لاشيء» في المفسّر.
            // (EN) Display for a null string pointer: from the unified SoT, not the English
            //      "void" literal, which leaked a user-visible display diverging from the
            //      interpreter's «لاشيء».
            llvm::Value *voidStr = b.CreateGlobalStringPtr(::Sad::Types::repr::kNullDisplay, "dyn.ts.void");
            llvm::Value *strRes = b.CreateSelect(strNull, voidStr, strPtr, "dyn.ts.safestr");
            b.CreateBr(mergeBB);
            strBB = b.GetInsertBlock();

            // (AR) [ز.٢١-أ] مصفوفة: الحمولةُ مؤشّرُ SadArray. نقرأ (الطول، البيانات، الوسوم)
            //      وننادي المساعِدَ المولَّدَ الذي يعود إلينا لكلِّ عنصر ⇒ تعشيشٌ لا محدود.
            //      حارسُ null يطابق حارسَ النصِّ أعلاه (مصفوفةٌ غيرُ مُهيّأة).
            // (EN) [ز.٢١-أ] Array: the payload is a SadArray pointer. Read (len, data, tags) and
            //      call the generated helper, which recurses back here per element ⇒ unbounded
            //      nesting. The null guard mirrors the string guard above (uninitialised array).
            llvm::Value *arrayRes = nullptr;      // (AR) نتيجةُ المسارِ السليم / (EN) ok-path result
            llvm::BasicBlock *arrayOkBB = nullptr; // (AR) كتلتُه عند الوصول للدمج
            llvm::Value *arrayNullRes = nullptr;   // (AR) نتيجةُ مسارِ المؤشّرِ العدم
            llvm::BasicBlock *arrayNullBB = nullptr;
            if (arrayBB)
            {
                b.SetInsertPoint(arrayBB);
                cg.ensureArrayToStringDynHelper();
                auto *arrTy = sadArrayStructType(ctx);
                llvm::Value *arrPtr = b.CreateIntToPtr(payload, ptrTy, "dyn.ts.arrp");
                llvm::Value *arrNull = b.CreateICmpEQ(
                    arrPtr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                    "dyn.ts.arrnull");
                auto *arrOkBB = llvm::BasicBlock::Create(ctx, "dyn.ts.array.ok", parent);
                auto *arrNullBB = llvm::BasicBlock::Create(ctx, "dyn.ts.array.null", parent);
                b.CreateCondBr(arrNull, arrNullBB, arrOkBB);

                b.SetInsertPoint(arrOkBB);
                llvm::Value *lenV = b.CreateLoad(i64, b.CreateStructGEP(arrTy, arrPtr, 0, "dyn.ts.arr.lenp"),
                                                 "dyn.ts.arr.len");
                llvm::Value *dataV = b.CreateLoad(ptrTy, b.CreateStructGEP(arrTy, arrPtr, 2, "dyn.ts.arr.datap"),
                                                  "dyn.ts.arr.data");
                llvm::Value *tagsV = b.CreateLoad(ptrTy, b.CreateStructGEP(arrTy, arrPtr, 3, "dyn.ts.arr.tagsp"),
                                                  "dyn.ts.arr.tags");
                auto helperFn = cg.module_->getOrInsertFunction(
                    "__sad_array_to_string_dyn",
                    llvm::FunctionType::get(ptrTy, {i64, ptrTy, ptrTy}, false));
                arrayRes = b.CreateCall(helperFn, {lenV, dataV, tagsV}, "dyn.ts.arr.str");
                arrayOkBB = b.GetInsertBlock();
                b.CreateBr(mergeBB);

                b.SetInsertPoint(arrNullBB);
                arrayNullRes =
                    b.CreateGlobalStringPtr(::Sad::Types::repr::kNullDisplay, "dyn.ts.arr.nullstr");
                arrayNullBB = b.GetInsertBlock();
                b.CreateBr(mergeBB);
                // (AR) الذراعُ يصل إلى الدمج من كتلتين (سليمة/عدم)، فتُسجَّلان معًا في PHI أدناه.
                // (EN) The arm reaches the merge from two blocks (ok/null); both feed the PHI below.
            }

            // (AR) ز.٤٣: الخريطة — المؤشّرُ العدمُ يُعرَض «لاشيء» كنظيرِه في ذراعِ المصفوفة.
            // (EN) ز.٤٣: map — a null pointer renders «لاشيء», mirroring the array arm.
            llvm::Value *mapRes = nullptr;
            llvm::BasicBlock *mapOkBB = nullptr;
            llvm::Value *mapNullRes = nullptr;
            llvm::BasicBlock *mapNullBB = nullptr;
            if (mapBB)
            {
                b.SetInsertPoint(mapBB);
                // ════════════════════════════════════════════════════════════
                // (AR) 🚧 **حدٌّ معروفٌ يُعلَنُ هنا ولا يُخبَّأ**: `dynToString` مسارٌ
                //   واحدٌ يخدمُ مستهلِكَين يطلبانِ صيغتَين مختلفتَين — الطباعةُ تطلبُ
                //   المقتبسةَ و`نص()` تطلبُ غيرَ المقتبسة. فالاختيارُ هنا **يُصيبُ
                //   الطباعةَ ويُخطئُ التحويلَ** لحاويةٍ متداخلة:
                //     `اطبع([1، {"أ":2}])` ⇒ «[1, {"أ": 2}]» في المحرّكَين ✅
                //     `نص([1، {"أ":2}])`   ⇒ المفسّرُ «[1, {أ: 2}]» والمصرّفُ المقتبسة ❌
                //   وهذا **سابقٌ لرايةِ الصيغتَين** لا انحدارٌ عنها. وإغلاقُه يقتضي
                //   عالمَينِ كاملَينِ من المساعِدين (`dynToString` ومساعِدُ المصفوفةِ
                //   الموسومةِ يستدعي كلٌّ منهما الآخرَ فتتضاعفُ النسخُ) — دفعةٌ مستقلّةٌ
                //   لا ذيلٌ لهذه. والإبقاءُ على المقتبسةِ هنا يحفظُ الطباعةَ كما هي.
                // (EN) KNOWN, DECLARED LIMIT: dynToString is one path serving two consumers
                //   that want different spellings; this choice is right for print and wrong
                //   for نص() of a NESTED container. Pre-existing, not a regression from the
                //   flag. Closing it needs two full worlds of mutually-recursive helpers.
                // ════════════════════════════════════════════════════════════
                cg.ensureMapToStringHelper(/*quoteKeys=*/true);
                llvm::Value *mapPtr = b.CreateIntToPtr(payload, ptrTy, "dyn.ts.mapp");
                llvm::Value *mapIsNull = b.CreateICmpEQ(
                    mapPtr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                    "dyn.ts.mapnull");
                auto *mapOk = llvm::BasicBlock::Create(ctx, "dyn.ts.map.ok", parent);
                auto *mapNull = llvm::BasicBlock::Create(ctx, "dyn.ts.map.null", parent);
                b.CreateCondBr(mapIsNull, mapNull, mapOk);

                b.SetInsertPoint(mapOk);
                auto mapHelperFn = cg.module_->getOrInsertFunction(
                    "__sad_map_to_string", llvm::FunctionType::get(ptrTy, {ptrTy}, false));
                mapRes = b.CreateCall(mapHelperFn, {mapPtr}, "dyn.ts.map.str");
                mapOkBB = b.GetInsertBlock();
                b.CreateBr(mergeBB);

                b.SetInsertPoint(mapNull);
                mapNullRes =
                    b.CreateGlobalStringPtr(::Sad::Types::repr::kNullDisplay, "dyn.ts.map.nullstr");
                mapNullBB = b.GetInsertBlock();
                b.CreateBr(mergeBB);
            }

            // (AR) كائن: العرضُ المُعتِمُ الموحَّد / (EN) object: the unified opaque display
            b.SetInsertPoint(objBB);
            llvm::Value *objRes = b.CreateGlobalStringPtr(
                ::Sad::Types::repr::kObjectOpaqueDisplay, "dyn.ts.objstr");
            b.CreateBr(mergeBB);
            objBB = b.GetInsertBlock();

            // (AR) عدم/غيره: لاشيء / (EN) null/other: لاشيء
            b.SetInsertPoint(nullBB);
            llvm::Value *nullRes = b.CreateGlobalStringPtr(
                ::Sad::Types::repr::kNullDisplay, "dyn.ts.nullstr"); // لاشيء (مصدرٌ موحَّد)
            b.CreateBr(mergeBB);
            nullBB = b.GetInsertBlock();

            b.SetInsertPoint(mergeBB);
            auto *phi = b.CreatePHI(ptrTy, arrayRes ? 10 : 6, "dyn.ts.result");
            phi->addIncoming(intRes, intBB);
            phi->addIncoming(floatRes, floatBB);
            phi->addIncoming(boolRes, boolBB);
            phi->addIncoming(strRes, strBB);
            phi->addIncoming(objRes, objBB);
            phi->addIncoming(nullRes, nullBB);
            if (arrayRes)
            {
                phi->addIncoming(arrayRes, arrayOkBB);
                phi->addIncoming(arrayNullRes, arrayNullBB);
            }
            if (mapRes)
            {
                phi->addIncoming(mapRes, mapOkBB);
                phi->addIncoming(mapNullRes, mapNullBB);
            }
            b.CreateRet(phi);

            if (savedBB)
                cg.builder_->SetInsertPoint(savedBB, savedPoint);
            return fn;
        }

        llvm::Value *dynToString(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            // (AR) نداءٌ لا توسيعٌ سطريّ: يمنع العَوْدَ اللانهائيَّ زمنَ الترجمة، وينكمش IR في
            //      عشراتِ مواقعِ الاستدعاء، ويُلغي حاجةَ المستدعين إلى مراعاةِ شقِّ الكتلة.
            // (EN) A call, not an inline expansion: prevents infinite compile-time recursion,
            //      shrinks IR at dozens of call sites, and removes the callers' need to account
            //      for a split basic block.
            llvm::Function *fn = ensureDynToStringFn(cg);
            llvm::Value *kind = dynKindByte(cg, dyn);
            llvm::Value *payload = dynPayloadI64(cg, dyn);
            return cg.builder_->CreateCall(fn, {kind, payload}, "dyn.ts.call");
        }

        llvm::Value *dynTypeName(LLVMCodeGen &cg, llvm::Value *dyn)
        {
            auto &b = *cg.builder_;
            auto &ctx = *cg.context_;
            auto *i8 = llvm::Type::getInt8Ty(ctx);
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            llvm::Value *kind = dynKindByte(cg, dyn);

            // (AR) اسم النوع من نفس مصدر sadTypeKindArabicName (types.yaml) لكلّ وسمٍ زمنَ التشغيل
            //      ⇒ يطابق نوع() المفسّر بايتيًّا (رقم/عشري/منطقي/نص). لا سلاسل حرفيّة مكرّرة.
            // (EN) The type name from the same source as sadTypeKindArabicName (types.yaml) for each
            //      runtime kind ⇒ matches نوع() byte-for-byte (رقم/عشري/منطقي/نص). No duplicated literals.
            auto nameFor = [&](SadTypeKind k)
            { return b.CreateGlobalStringPtr(::Sad::Types::sadTypeKindArabicName(k), "dyn.tn"); };

            auto *parent = b.GetInsertBlock()->getParent();
            auto *intBB = llvm::BasicBlock::Create(ctx, "dyn.tn.int", parent);
            auto *floatBB = llvm::BasicBlock::Create(ctx, "dyn.tn.float", parent);
            auto *boolBB = llvm::BasicBlock::Create(ctx, "dyn.tn.bool", parent);
            auto *strBB = llvm::BasicBlock::Create(ctx, "dyn.tn.str", parent);
            auto *defBB = llvm::BasicBlock::Create(ctx, "dyn.tn.def", parent);
            // (AR) Amelia (ISSUE-076): وسمُ العدم (Null) ⇒ اسم النوع «عدم» من SoT لا «مجهول».
            //      نوع(حمولةٍ ديناميّةٍ = لاشيء) يطابق المفسّرَ («عدم») بدل تراجُع default («مجهول»).
            // (EN) Amelia (ISSUE-076): the Null kind ⇒ the SoT name «عدم», not «مجهول». نوع() of a
            //      null dynamic payload matches the interpreter («عدم») instead of the default.
            auto *nullBB = llvm::BasicBlock::Create(ctx, "dyn.tn.null", parent);
            // (AR) ز.٣٧: وسمُ المصفوفة كان يسقط إلى default فيُرجع «مجهول» بينما المفسّرُ
            //      يُرجع «مصفوفة» — تباعُدٌ صامتٌ يُفسِد كلَّ توزيعٍ على نوع(قيمةٍ ديناميّة).
            // (EN) ز.٣٧: the Array kind used to fall to default and yield «مجهول» while the
            //      interpreter yields «مصفوفة» — a silent divergence breaking any dispatch
            //      on نوع() of a dynamic value.
            auto *arrayBB = llvm::BasicBlock::Create(ctx, "dyn.tn.array", parent);
            // (AR) ز.٤٣: والخريطةُ مثلُها — صارت تُوسَم عنصرًا في مصفوفةٍ مختلطة، فلولا
            //      ذراعُها هنا لأرجع `نوع()` «مجهول» بينما يُرجع المفسّرُ «خريطة».
            // (EN) ز.٤٣: a map is now tagged as an element of a heterogeneous array; without
            //      this arm نوع() would answer «مجهول» while the interpreter answers «خريطة».
            auto *mapBB = llvm::BasicBlock::Create(ctx, "dyn.tn.map", parent);
            // (AR) [م-٠٠١] والفراغُ يُميَّزُ عن العدم: مفتاحٌ غائبٌ من خريطةٍ يُرجعُ
            //      في المفسّرِ قيمةَ Void فـ`نوع()` «فراغ»، لا «عدم».
            // (EN) [card م-٠٠١] Void is distinguished from Null: an absent map key yields a
            //      Void value in the interpreter, so نوع() answers «فراغ», not «عدم».
            auto *voidBB = llvm::BasicBlock::Create(ctx, "dyn.tn.void", parent);
            // (AR) وذراعُ الكائنِ تلزمُ منذُ صار `صنف`/`بنية` يُوسَمُ `Obj` في `toDyn`:
            //      ولولاها لسقطَ الوسمُ إلى `default` فأجابَ `نوع()` «مجهول» — أي لا يُستبدَلُ
            //      كذبٌ بصدقٍ بل بكذبٍ آخر. والاسمُ من SoT: `Class ⇒ typeof_ar: كائن`.
            // (EN) The object arm is required once Class/Struct are tagged Obj in toDyn:
            //      without it the tag would fall to `default` and نوع() would answer
            //      «unknown» — replacing one lie with another rather than with the truth.
            auto *objBB = llvm::BasicBlock::Create(ctx, "dyn.tn.obj", parent);
            auto *adtBB = llvm::BasicBlock::Create(ctx, "dyn.tn.adt", parent);
            auto *mergeBB = llvm::BasicBlock::Create(ctx, "dyn.tn.merge", parent);

            llvm::SwitchInst *sw = b.CreateSwitch(kind, defBB, 10);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Void), voidBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Int), intBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Float), floatBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Bool), boolBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Str), strBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Null), nullBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Array), arrayBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Map), mapBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Obj), objBB);
            sw->addCase(llvm::ConstantInt::get(i8, DynKind::Adt), adtBB);

            b.SetInsertPoint(intBB);
            llvm::Value *ri = nameFor(SadTypeKind::Integer);
            b.CreateBr(mergeBB);
            intBB = b.GetInsertBlock();
            b.SetInsertPoint(floatBB);
            llvm::Value *rf = nameFor(SadTypeKind::Float);
            b.CreateBr(mergeBB);
            floatBB = b.GetInsertBlock();
            b.SetInsertPoint(boolBB);
            llvm::Value *rb = nameFor(SadTypeKind::Boolean);
            b.CreateBr(mergeBB);
            boolBB = b.GetInsertBlock();
            b.SetInsertPoint(strBB);
            llvm::Value *rs = nameFor(SadTypeKind::String);
            b.CreateBr(mergeBB);
            strBB = b.GetInsertBlock();
            b.SetInsertPoint(defBB);
            llvm::Value *rd = nameFor(SadTypeKind::Unknown);
            b.CreateBr(mergeBB);
            defBB = b.GetInsertBlock();
            b.SetInsertPoint(nullBB);
            llvm::Value *rnull = nameFor(SadTypeKind::Null);
            b.CreateBr(mergeBB);
            nullBB = b.GetInsertBlock();
            b.SetInsertPoint(arrayBB);
            llvm::Value *rarray = nameFor(SadTypeKind::Array);
            b.CreateBr(mergeBB);
            arrayBB = b.GetInsertBlock();
            b.SetInsertPoint(mapBB);
            llvm::Value *rmap = nameFor(SadTypeKind::Map);
            b.CreateBr(mergeBB);
            mapBB = b.GetInsertBlock();
            b.SetInsertPoint(voidBB);
            llvm::Value *rvoid = nameFor(SadTypeKind::Void);
            b.CreateBr(mergeBB);
            voidBB = b.GetInsertBlock();
            b.SetInsertPoint(objBB);
            llvm::Value *robj = nameFor(SadTypeKind::Class);
            b.CreateBr(mergeBB);
            objBB = b.GetInsertBlock();
            // (AR) ⚠️ واسمُ التعدادِ «خريطة» لا «تعداد»: المرجعُ يُمثِّلُ المتغيِّرةَ
            //      خريطةً فيُجيبُ `نوع()` «خريطة» — مقيسٌ لا مفترَض، والمطابقةُ
            //      للمرجعِ هي العقدُ وإن بدا اللفظُ غريبًا.
            // (EN) The ADT name is «map», not «enum»: the reference models a variant as a
            //      map, so its نوع() answers «map» — measured, and matching the reference
            //      is the contract even where the word reads oddly.
            b.SetInsertPoint(adtBB);
            llvm::Value *radt = nameFor(SadTypeKind::Map);
            b.CreateBr(mergeBB);
            adtBB = b.GetInsertBlock();

            b.SetInsertPoint(mergeBB);
            auto *phi = b.CreatePHI(ptrTy, 11, "dyn.tn.result");
            phi->addIncoming(rvoid, voidBB);
            phi->addIncoming(ri, intBB);
            phi->addIncoming(rf, floatBB);
            phi->addIncoming(rb, boolBB);
            phi->addIncoming(rs, strBB);
            phi->addIncoming(rd, defBB);
            phi->addIncoming(rnull, nullBB);
            phi->addIncoming(rarray, arrayBB);
            phi->addIncoming(rmap, mapBB);
            phi->addIncoming(robj, objBB);
            phi->addIncoming(radt, adtBB);
            return phi;
        }

    } // namespace LLVM
} // namespace Sad
