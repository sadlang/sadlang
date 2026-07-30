// ============================================================================
// (AR) جسر SIR→x86-64 أصليّ — أوّل ترجمة «لغة ص → شيفرة آلة» حقيقيّة بلا LLVM.
//      يستهلك SIR الذي يبنيه الأمامُ الفعليّ من مصدر ص (في الوضع الحرّ، حيث تُسقَط
//      سِقالةُ defer/الاستثناء فتبقى شيفرةٌ خطّيّة نظيفة)، ويخفّض مجموعةً دنيا من
//      الأوپكودات (MOVE / ADD_I64 / SUB_I64 / المقارنات / BR / BR_COND / RET) إلى
//      بايتات x86-64 عبر المحرّك الجدوليّ (lookupEncSpec + encodeVariable)، ثمّ
//      يلفّها كاتبُ ELF في تنفيذيٍّ ساكن يخرج بقيمة إرجاع `رئيسية`. لا clang/lld/as.
//
//      تدفّقُ التحكّم بمرورين: (١) يُصدر بايتاتِ كلّ كتلةٍ بالترتيب ويسجّل إزاحةَ
//      لصيقتها؛ يُصدر القفزاتِ بإزاحةٍ صفريّةٍ نائبة ويسجّل ترقيعًا. (٢) يرقّع كلَّ
//      rel32 بالفرق (هدف − نهايةُ القفز). المقارنةُ المُغذِّيةُ لـBR_COND تُدمَج
//      (cmp؛ jCC ثمّ؛ jmp وإلّا) فلا حاجةَ لـsetcc/movzx. تخصيصُ السجلّات دنيا عمدًا
//      («كلٌّ في سجلّ» بلا انسكاب، ≤٧ سجلّات حيّة، بلا PHI/ذاكرة). كلُّ بنيةٍ غير
//      مدعومةٍ (PHI/ذاكرة/مقارنةٌ غيرُ مدموجة) تُفشِل التخفيضَ صراحةً لا تُترجَم خطأً.
// (EN) SIR→native x86-64 bridge — the first real "S-lang → machine code" lowering
//      without LLVM. Consumes SIR built by the actual frontend from ص source (in
//      freestanding mode), lowers a minimal opcode set (MOVE/ADD_I64/SUB_I64/
//      comparisons/BR/BR_COND/RET) to x86-64 bytes via the table-driven encoder,
//      then wraps them in a static ELF that exits with `رئيسية`'s return value.
//
//      Control flow via two passes: (1) emit each block's bytes in order, record
//      its label offset, emit jumps with a placeholder rel32 and record a fixup;
//      (2) patch each rel32 with (target − end-of-jump). The comparison feeding a
//      BR_COND is fused (cmp; jCC then; jmp else) — no setcc/movzx needed. Register
//      allocation is minimal on purpose (everything-in-registers, no spilling, ≤7
//      live regs, no PHI/memory). Unsupported constructs (PHI/memory/an unfused
//      comparison) fail loudly rather than miscompile silently.
// ============================================================================
#ifndef SAD_NATIVE_SIR_LOWERING_H
#define SAD_NATIVE_SIR_LOWERING_H

#include "backend/native/x86_variable_encoder.h"
#include "backend/native/generated/x86_64_encoding_generated.h"
#include "backend/native/elf64_writer.h"

#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"

// (AR) كتالوج الأخطاء المولَّد من مصدر الحقيقة (language-truth/errors/*.yaml):
//      المخفّضُ يحمل ErrorCode وبياناتِ {detail} فقط، والرسالةُ تُشتقّ من الكتالوج —
//      لا نصَّ رسالةٍ مباشرًا في هذا الكود (قاعدة SoT: كلّها من YAML).
#include "error_codes.h"
#include "error_messages_generated.h"

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace sad
{
    namespace native
    {
        // (AR) أسماءٌ مختصرة لأنواع SIR (فضاء الاسم Sad::Compiler::SIR) وأنواع البيانات.
        namespace sir = ::Sad::Compiler::SIR;
        namespace types = ::Sad::Types;

        // (AR) رقمُ نداء exit في SysV/x86-64 (من abi/x86_64-linux.yaml). ثابتٌ مسمّى لا رقمٌ سحريّ.
        inline constexpr long long kSysExitX86 = 60;

        // (AR) مخرَجُ التخفيض: بايتاتُ الشيفرة عند النجاح، أو رمزُ خطأٍ (ErrorCode من
        //      كتالوج SoT) + بياناتُ {detail} عند الفشل. الرسالةُ النصّيّة تُشتقّ من
        //      الكتالوج عبر message() — لا تُخزَّن كنصٍّ مباشر.
        struct LoweringResult
        {
            bool ok = false;
            ::Sad::Errors::ErrorCode errorCode = ::Sad::Errors::ErrorCode::INT_NATIVE_NO_ENTRY;
            std::string detail;        // (AR) بياناتُ {detail} فقط (لا نثر)
            std::vector<uint8_t> code; // (AR) شيفرة .text ثمّ ELF (عند النجاح)

            // (AR) الرسالةُ المختصرة من كتالوج SoT (briefAr) مع إحلال {detail} — لا نصَّ مباشرًا.
            // (EN) The brief message from the SoT catalog with {detail} substituted.
            std::string message() const
            {
                const auto *e = ::Sad::Errors::Generated::findByCode(errorCode);
                if (!e || !e->briefAr)
                    return {};
                std::string m = e->briefAr;
                static const std::string kPh = "{detail}";
                auto pos = m.find(kPh);
                if (pos != std::string::npos)
                    m.replace(pos, kPh.size(), detail);
                return m;
            }
        };

        // (AR) مخفّضٌ لكتلةٍ أساسيّة خطّيّة واحدة إلى x86-64.
        //      نموذجُ التخصيص: كلُّ سجلٍّ افتراضيّ (بالاسم) ⇒ سجلٌّ فيزيائيّ من حوضٍ
        //      لا يشمل rax (رقم النداء) وla rdi (وسيط الخروج) ولا rsp/rbp — بلا انسكاب.
        class X86SirLowering
        {
        public:
            LoweringResult lowerEntry(const sir::SIRModule &module)
            {
                LoweringResult r;
                const sir::SIRFunction *fn = findEntry(module);
                if (!fn)
                    return finishError(r, EC::INT_NATIVE_NO_ENTRY);
                const auto &blocks = fn->getBasicBlocks();
                if (blocks.empty())
                    return finishError(r, EC::INT_NATIVE_UNSUPPORTED, detailBlocks());

                // (AR) المرور ١: أصدِر بايتاتِ كلّ كتلةٍ بالترتيب، سجّل إزاحةَ لصيقتها،
                //      وسجّل ترقيعَ كلّ قفز. كلُّ كتلةٍ يجب أن تنتهيَ بمُنهٍ (ret/br/brcond).
                for (const auto &blockPtr : blocks)
                {
                    const sir::SIRBasicBlock &block = *blockPtr;
                    labelOffset_[block.name] = code_.size();
                    if (!lowerBlock(block))
                        return finishError(r, errorCode_, detail_);
                }

                // (AR) المرور ٢: رقّع كلَّ rel32 بالفرق (إزاحةُ الهدف − نهايةُ القفز).
                if (!applyFixups())
                    return finishError(r, errorCode_, detail_);

                r.ok = true;
                r.code = std::move(code_);
                return r;
            }

        private:
            using EC = ::Sad::Errors::ErrorCode;

            std::map<std::string, int> regOf_;                                    // (AR) اسم السجلّ الافتراضيّ ⇒ فيزيائيّ
            const std::vector<int> pool_{x86::RDX, x86::RCX, x86::RSI, x86::R8,    // (AR) حوضُ التخصيص
                                         x86::R9, x86::R10, x86::R11};
            size_t next_ = 0;
            std::vector<uint8_t> code_;
            EC errorCode_ = EC::INT_NATIVE_NO_ENTRY;
            std::string detail_;

            // (AR) تدفّق التحكّم: خريطةُ لصيقة الكتلة ⇒ إزاحتها بالبايت، وطابورُ ترقيع rel32.
            std::map<std::string, size_t> labelOffset_;
            struct Fixup
            {
                size_t rel32Pos;    // (AR) موضعُ بايتات الإزاحة النسبيّة في code_
                std::string target; // (AR) لصيقةُ الكتلة الهدف
                int width;          // (AR) عرضُ حقل الإزاحة بالبايت (من مواصفة الترميز، لا ثابتًا)
            };
            std::vector<Fixup> fixups_;

            // (AR) رموزُ {detail} كبياناتٍ محضة (لا نثر): وسمُ الحالة قصيرٌ يُميّز فرعَ الفشل.
            //      النثرُ كلُّه في كتالوج SoT؛ هذه القيمُ تملأ {detail} حصرًا.
            static std::string detailBlocks() { return "blocks=0"; }
            static std::string detailNoRet() { return "no-ret-terminator"; }
            static std::string detailOpcode(const sir::SIRInstruction &i)
            {
                return "opcode=" + std::to_string(static_cast<int>(i.opcode));
            }

            LoweringResult &finishError(LoweringResult &r, EC code, const std::string &detail = "")
            {
                r.ok = false;
                r.errorCode = code;
                r.detail = detail;
                return r;
            }

            const sir::SIRFunction *findEntry(const sir::SIRModule &m) const
            {
                // (AR) رئيسية (بايتات UTF-8) ثمّ بدائل، ثمّ أوّل دالّة.
                static const std::string kMain = "\xD8\xB1\xD8\xA6\xD9\x8A\xD8\xB3\xD9\x8A\xD8\xA9"; // رئيسية
                for (const auto &name : {kMain, std::string("main"), std::string("__sad_main")})
                {
                    auto f = m.getFunction(name);
                    if (f)
                        return f.get();
                }
                const auto &fns = m.getFunctions();
                return fns.empty() ? nullptr : fns[0].get();
            }

            // (AR) يسجّل رمزَ الخطأ (من كتالوج SoT) وبياناتِ {detail}؛ لا نصَّ رسالةٍ هنا.
            bool fail(EC code, const std::string &detail = "")
            {
                errorCode_ = code;
                detail_ = detail;
                return false;
            }

            // (AR) قيمةٌ محلولة: إمّا ثابتٌ فوريّ أو سجلٌّ فيزيائيّ.
            struct Val
            {
                bool isConst;
                long long c;
                int reg;
            };

            // (AR) يخصّص سجلًّا فيزيائيًّا لسجلٍّ افتراضيّ. عند نفاد الحوض **يفشل بوضوح**
            //      لا يلتفّ (التفافٌ صامتٌ يدهس سجلًّا حيًّا ⇒ إفساد — عيب أميليا BLOCKER-1).
            bool allocReg(const std::string &vreg, int &out)
            {
                auto it = regOf_.find(vreg);
                if (it != regOf_.end())
                {
                    out = it->second;
                    return true;
                }
                if (next_ >= pool_.size())
                    return fail(EC::INT_NATIVE_REGALLOC_EXHAUSTED, "pool=" + std::to_string(pool_.size()));
                out = pool_[next_++];
                regOf_[vreg] = out;
                return true;
            }

            bool resolve(const sir::SIROperand &op, Val &out)
            {
                if (op.type == sir::SIROperandType::CONSTANT)
                {
                    // (AR) الاتّحاد intValue صالحٌ فقط للأعداد الصحيحة؛ منطقيّ/عشريّ/نصّيّ
                    //      يترك intValue غير مُهيّأ ⇒ قمامة (عيب أميليا BLOCKER-2).
                    if (op.dataType != types::SadTypeKind::Integer)
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "const-type=" + std::to_string(static_cast<int>(op.dataType)));
                    out = Val{true, op.intValue, 0};
                    return true;
                }
                if (op.type == sir::SIROperandType::REGISTER)
                {
                    auto it = regOf_.find(op.name);
                    if (it == regOf_.end())
                        return fail(EC::INT_NATIVE_UNDEF_VREG, "%" + op.name);
                    out = Val{false, 0, it->second};
                    return true;
                }
                return fail(EC::INT_NATIVE_UNSUPPORTED,
                            "operand-kind=" + std::to_string(static_cast<int>(op.type)));
            }

            // ── مُصدِرات التعليمات (كلٌّ يقرأ مواصفتَه من الجدول المولَّد من SoT) ──
            //      كلٌّ يُرجع bool: الفشلُ (مواصفةٌ مفقودة/ثابتٌ خارج المدى) يصعد فيُوقف
            //      التخفيضَ بدل إنتاج ELF مبتورٍ بعلامة نجاح (عيب أميليا رقم ٤).
            bool emit(const std::string &mnemonic, const std::string &form,
                      const std::vector<x86::Operand> &ops)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(mnemonic, form);
                if (!spec)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, mnemonic + " " + form);
                auto bytes = x86::encodeVariable(*spec, ops);
                code_.insert(code_.end(), bytes.begin(), bytes.end());
                return true;
            }

            static const char *kMov;  // انقل
            static const char *kAdd;  // اجمع
            static const char *kSub;  // اطرح
            static const char *kSys;  // نداء_نظام
            static const char *kCmp;  // قارن
            static const char *kJmp;  // اقفز
            static const char *kJe;   // اقفز_إذا_ساوى
            static const char *kJne;  // اقفز_إذا_لم_يساوِ
            static const char *kJl;   // اقفز_إذا_أصغر
            static const char *kJle;  // اقفز_إذا_أصغر_أو_ساوى
            static const char *kJg;   // اقفز_إذا_أكبر
            static const char *kJge;  // اقفز_إذا_أكبر_أو_ساوى

            // (AR) mov r32, imm32 يمتدّ صفريًّا إلى ٦٤؛ لا يمثّل إلّا [0, 2³²). خارجَه
            //      (سالبٌ أو ≥2³²) يُبتَر ⇒ نرفضه بوضوح (عيب أميليا رقم ٣).
            bool movImm(int reg, long long imm)
            {
                if (imm < 0 || imm > 0xFFFFFFFFLL)
                    return fail(EC::INT_NATIVE_IMM_RANGE, "u32:" + std::to_string(imm));
                return emit(kMov, "r32, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }
            bool movReg(int dst, int src)
            {
                if (dst == src)
                    return true;
                return emit(kMov, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            // (AR) add/sub r64, imm32 يمتدّ إشاريًّا؛ نقصره على مدى imm32 الموقَّع.
            bool checkImm32(long long imm)
            {
                if (imm < -2147483648LL || imm > 2147483647LL)
                    return fail(EC::INT_NATIVE_IMM_RANGE, "s32:" + std::to_string(imm));
                return true;
            }
            bool addImm(int reg, long long imm)
            {
                return checkImm32(imm) && emit(kAdd, "r64, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }
            bool addReg(int dst, int src)
            {
                return emit(kAdd, "r64, r64", {x86::Operand::R(dst), x86::Operand::R(src)});
            }
            bool subImm(int reg, long long imm)
            {
                return checkImm32(imm) && emit(kSub, "r64, imm32", {x86::Operand::R(reg), x86::Operand::I(imm, 32)});
            }

            // (AR) يضع قيمةً (ثابتًا أو سجلًّا) في سجلٍّ وجهةٍ معيَّن.
            bool materialize(int dst, const Val &v)
            {
                return v.isConst ? movImm(dst, v.c) : movReg(dst, v.reg);
            }

            bool lowerInstruction(const sir::SIRInstruction &inst)
            {
                using OP = sir::SIROpcode;
                switch (inst.opcode)
                {
                case OP::MOVE:
                {
                    if (!inst.result || inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    Val s;
                    if (!resolve(inst.operands[0], s))
                        return false;
                    return materialize(dst, s);
                }
                case OP::ADD_I64:
                case OP::SUB_I64:
                {
                    if (!inst.result || inst.operands.size() != 2)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    int dst;
                    if (!allocReg(inst.result->name, dst))
                        return false;
                    Val a, b;
                    if (!resolve(inst.operands[0], a) || !resolve(inst.operands[1], b))
                        return false;
                    if (!materialize(dst, a)) // (AR) حمّل المعامل الأوّل في الوجهة
                        return false;
                    if (inst.opcode == OP::ADD_I64)
                        return b.isConst ? addImm(dst, b.c) : addReg(dst, b.reg);
                    // SUB_I64
                    if (!b.isConst)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, "sub-reg-reg:" + detailOpcode(inst));
                    return subImm(dst, b.c);
                }
                case OP::RET:
                {
                    if (inst.operands.size() != 1)
                        return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                    Val v;
                    if (!resolve(inst.operands[0], v))
                        return false;
                    // (AR) ضع قيمةَ الإرجاع في rdi (وسيط exit)، ثمّ استدعِ exit — كلٌّ مفحوص.
                    if (v.isConst ? !movImm(x86::RDI, v.c) : !movReg(x86::RDI, v.reg))
                        return false;
                    if (!movImm(x86::RAX, kSysExitX86))
                        return false;
                    return emit(kSys, "", {});
                }
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }

            // ── تدفّق التحكّم: تخفيضُ كتلةٍ كاملة + الفروع + الترقيع ──

            // (AR) هل الأوپكود مقارنةٌ عدديّةٌ موقَّعة؟ (تُدمَج في BR_COND التالي.)
            static bool isComparison(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                return op == OP::EQ || op == OP::NE || op == OP::LT ||
                       op == OP::LE || op == OP::GT || op == OP::GE;
            }

            // (AR) منمنمةُ القفز الشرطيّ المطابقة للمقارنة (موقَّعة): «إن صحّ الشرط اقفز لـthen».
            static const char *jccForCmp(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                switch (op)
                {
                case OP::EQ: return kJe;
                case OP::NE: return kJne;
                case OP::LT: return kJl;
                case OP::LE: return kJle;
                case OP::GT: return kJg;
                case OP::GE: return kJge;
                default: return nullptr;
                }
            }

            // (AR) يُصدر قفزًا (مشروطًا أو غير مشروط) بإزاحةٍ نسبيّةٍ صفريّةٍ نائبة، ويسجّل
            //      ترقيعًا عند حقل الإزاحة. عرضُ الحقل وموضعُه يُشتقّان من مواصفة الترميز
            //      (imm_bits) لا من ثابتٍ مُرمَّز ⇒ يبقى سليمًا لو أُضيفت صيغةُ قفزٍ بعرضٍ آخر.
            bool emitJump(const char *mnemonic, const std::string &targetLabel)
            {
                const x86::EncSpec *spec = x86::lookupEncSpec(mnemonic, "rel32");
                if (!spec || spec->imm_op < 0 || spec->imm_bits <= 0)
                    return fail(EC::INT_NATIVE_ENCODING_MISSING, std::string(mnemonic) + " rel32");
                const int width = spec->imm_bits / 8;
                if (!emit(mnemonic, "rel32", {x86::Operand::I(0, spec->imm_bits)}))
                    return false;
                fixups_.push_back({code_.size() - static_cast<size_t>(width), targetLabel, width});
                return true;
            }

            // (AR) يقارن سجلَّ المُبدَّد (RAX) بمعاملٍ ثانٍ (ثابتٌ يختار imm8/imm32، أو سجلّ).
            bool cmpScratchAgainst(const Val &b)
            {
                if (!b.isConst)
                    return emit(kCmp, "r64, r64", {x86::Operand::R(x86::RAX), x86::Operand::R(b.reg)});
                if (b.c >= -128 && b.c <= 127)
                    return emit(kCmp, "r64, imm8", {x86::Operand::R(x86::RAX), x86::Operand::I(b.c, 8)});
                if (!checkImm32(b.c))
                    return false;
                return emit(kCmp, "r64, imm32", {x86::Operand::R(x86::RAX), x86::Operand::I(b.c, 32)});
            }

            // (AR) القفزُ غير المشروط BR: operands[0] لصيقةُ الهدف.
            bool lowerBranch(const sir::SIRInstruction &inst)
            {
                if (inst.operands.size() != 1 || inst.operands[0].type != sir::SIROperandType::LABEL)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                return emitJump(kJmp, inst.operands[0].name);
            }

            // (AR) القفزُ المشروط BR_COND: {condition, thenLabel, elseLabel}.
            //      الشرطُ إمّا ثابتٌ منطقيّ (⇒ قفزٌ غير مشروطٍ للفرع المختار) أو سجلٌّ
            //      نتيجةُ مقارنةٍ مدموجةٍ في نفس الكتلة (⇒ cmp؛ jCC then؛ jmp else).
            bool lowerBranchCond(const sir::SIRInstruction &inst,
                                 const sir::SIRBasicBlock &block)
            {
                if (inst.operands.size() != 3 ||
                    inst.operands[1].type != sir::SIROperandType::LABEL ||
                    inst.operands[2].type != sir::SIROperandType::LABEL)
                    return fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                const sir::SIROperand &cond = inst.operands[0];
                const std::string &thenLbl = inst.operands[1].name;
                const std::string &elseLbl = inst.operands[2].name;

                // (AR) شرطٌ ثابتٌ منطقيّ: قفزٌ غير مشروطٍ للفرع المحدَّد ثابتًا.
                if (cond.type == sir::SIROperandType::CONSTANT)
                {
                    if (cond.dataType != types::SadTypeKind::Boolean)
                        return fail(EC::INT_NATIVE_UNSUPPORTED,
                                    "cond-const-type=" + std::to_string(static_cast<int>(cond.dataType)));
                    return emitJump(kJmp, cond.boolValue ? thenLbl : elseLbl);
                }
                if (cond.type != sir::SIROperandType::REGISTER)
                    return fail(EC::INT_NATIVE_UNSUPPORTED,
                                "cond-kind=" + std::to_string(static_cast<int>(cond.type)));

                // (AR) شرطٌ سجليّ: يجب أن يكون نتيجةَ مقارنةٍ في هذه الكتلة (إدماج).
                const sir::SIRInstruction *cmp = findFusedComparison(block, cond.name);
                if (!cmp)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, "cond-not-fused-cmp:%" + cond.name);
                Val a, b;
                if (!resolve(cmp->operands[0], a) || !resolve(cmp->operands[1], b))
                    return false;
                if (!materialize(x86::RAX, a)) // (AR) المعامل الأوّل في المُبدَّد RAX (خارج الحوض)
                    return false;
                if (!cmpScratchAgainst(b))
                    return false;
                const char *jcc = jccForCmp(cmp->opcode);
                if (!jcc)
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(*cmp));
                return emitJump(jcc, thenLbl) && emitJump(kJmp, elseLbl);
            }

            // (AR) يجد مقارنةً في الكتلة نتيجتُها cond تُغذّي BR_COND المُنهيَ لها. تُدمَج
            //      فقط إن كانت المقارنةُ آخرَ تعليمةٍ قبل المُنهي (نمطُ البانِي المعتاد) —
            //      وإلّا لا إدماجَ (المقارنةُ غيرُ المدموجة تفشل صراحةً في lowerInstruction).
            const sir::SIRInstruction *findFusedComparison(const sir::SIRBasicBlock &block,
                                                           const std::string &condName) const
            {
                const auto &is = block.instructions;
                if (is.size() < 2)
                    return nullptr;
                const sir::SIRInstruction &prev = is[is.size() - 2];
                if (isComparison(prev.opcode) && prev.result && prev.result->name == condName)
                    return &prev;
                return nullptr;
            }

            // (AR) يخفّض كتلةً كاملة: يتخطّى المقارنةَ المدموجة (يعالجها BR_COND)، ويوجّه
            //      المُنهياتِ لمعالِجاتها. الكتلةُ يجب أن تنتهيَ بمُنهٍ.
            bool lowerBlock(const sir::SIRBasicBlock &block)
            {
                const auto &is = block.instructions;
                if (is.empty() || !is.back().isTerminatorInst())
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailNoRet());

                // (AR) نموذج «كلٌّ في سجلّ» لا يحمل حياةً عابرةً للكتل: كلُّ كتلةٍ تبدأ بحوضٍ
                //      نظيف. فقراءةُ سجلٍّ عُرِّف في كتلةٍ أخرى (PHI ضمنيّ، يحتاج انسكابًا/دمجًا)
                //      تغيبُ عن الخريطة ⇒ تفشل صراحةً UNDEF_VREG بدل قراءةِ قمامةٍ صامتة.
                regOf_.clear();
                next_ = 0;

                // (AR) المقارنةُ المدموجة (إن وُجدت) لا تُخفَّض مستقلّةً.
                const sir::SIRInstruction *fused = nullptr;
                if (is.back().opcode == sir::SIROpcode::BR_COND &&
                    !is.back().operands.empty() &&
                    is.back().operands[0].type == sir::SIROperandType::REGISTER)
                    fused = findFusedComparison(block, is.back().operands[0].name);

                for (const sir::SIRInstruction &inst : is)
                {
                    if (&inst == fused)
                        continue; // (AR) مدموجة ⇒ يعالجها BR_COND
                    if (inst.opcode == sir::SIROpcode::BR)
                    {
                        if (!lowerBranch(inst))
                            return false;
                    }
                    else if (inst.opcode == sir::SIROpcode::BR_COND)
                    {
                        if (!lowerBranchCond(inst, block))
                            return false;
                    }
                    else if (!lowerInstruction(inst))
                        return false;
                }
                return true;
            }

            // (AR) المرور ٢: يرقّع كلَّ قفزٍ بالفرق الموقَّع (إزاحةُ الهدف − نهايةُ القفز).
            //      نهايةُ القفز = موضعُ الحقل + عرضُه (الحقلُ آخرُ ما يُصدَر). المدى والكتابة
            //      يحترمان عرضَ الحقل ⇒ يصحّان لأيّ عرض (rel32 اليوم، وrel8 مستقبلًا).
            bool applyFixups()
            {
                for (const Fixup &fx : fixups_)
                {
                    auto it = labelOffset_.find(fx.target);
                    if (it == labelOffset_.end())
                        return fail(EC::INT_NATIVE_LABEL_UNDEFINED, fx.target);
                    long long disp = static_cast<long long>(it->second) -
                                     static_cast<long long>(fx.rel32Pos + fx.width);
                    const long long lim = 1LL << (fx.width * 8 - 1); // (AR) نصفُ مدى الحقل الموقَّع
                    if (disp < -lim || disp > lim - 1)
                        return fail(EC::INT_NATIVE_IMM_RANGE, "rel:" + std::to_string(disp));
                    uint64_t u = static_cast<uint64_t>(disp); // (AR) متمّمُ الاثنين، LE بعرض الحقل
                    for (int i = 0; i < fx.width; ++i)
                        code_[fx.rel32Pos + i] = static_cast<uint8_t>((u >> (8 * i)) & 0xFF);
                }
                return true;
            }
        };

        // (AR) منمنمات SoT العربيّة (تُرآي backend/x86_64/instructions.yaml).
        inline const char *X86SirLowering::kMov = "\xD8\xA7\xD9\x86\xD9\x82\xD9\x84";                 // انقل
        inline const char *X86SirLowering::kAdd = "\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9";                 // اجمع
        inline const char *X86SirLowering::kSub = "\xD8\xA7\xD8\xB7\xD8\xB1\xD8\xAD";                 // اطرح
        inline const char *X86SirLowering::kSys = "\xD9\x86\xD8\xAF\xD8\xA7\xD8\xA1_\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85"; // نداء_نظام
        inline const char *X86SirLowering::kCmp = "\xD9\x82\xD8\xA7\xD8\xB1\xD9\x86";                                                                                                 // قارن
        inline const char *X86SirLowering::kJmp = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2";                                                                                                 // اقفز
        inline const char *X86SirLowering::kJe = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xB3\xD8\xA7\xD9\x88\xD9\x89";                                     // اقفز_إذا_ساوى
        inline const char *X86SirLowering::kJne = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD9\x84\xD9\x85\x5F\xD9\x8A\xD8\xB3\xD8\xA7\xD9\x88\xD9\x90";        // اقفز_إذا_لم_يساوِ
        inline const char *X86SirLowering::kJl = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD8\xB5\xD8\xBA\xD8\xB1";                                     // اقفز_إذا_أصغر
        inline const char *X86SirLowering::kJle = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD8\xB5\xD8\xBA\xD8\xB1\x5F\xD8\xA3\xD9\x88\x5F\xD8\xB3\xD8\xA7\xD9\x88\xD9\x89"; // اقفز_إذا_أصغر_أو_ساوى
        inline const char *X86SirLowering::kJg = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD9\x83\xD8\xA8\xD8\xB1";                                     // اقفز_إذا_أكبر
        inline const char *X86SirLowering::kJge = "\xD8\xA7\xD9\x82\xD9\x81\xD8\xB2\x5F\xD8\xA5\xD8\xB0\xD8\xA7\x5F\xD8\xA3\xD9\x83\xD8\xA8\xD8\xB1\x5F\xD8\xA3\xD9\x88\x5F\xD8\xB3\xD8\xA7\xD9\x88\xD9\x89"; // اقفز_إذا_أكبر_أو_ساوى

        // (AR) مُيسِّرٌ عالي المستوى: SIRModule ⇒ ثنائيُّ ELF64 ساكن (x86-64) قابل للتنفيذ.
        inline LoweringResult lowerModuleToElf(const sir::SIRModule &module)
        {
            X86SirLowering low;
            LoweringResult r = low.lowerEntry(module);
            if (!r.ok)
                return r;
            r.code = elf::writeStaticExec(r.code); // (AR) يلفّ الشيفرة في ELF ويعيدها مكانها
            return r;
        }

    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_SIR_LOWERING_H
