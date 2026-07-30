// ============================================================================
// (AR) جسر SIR→x86-64 أصليّ — أوّل ترجمة «لغة ص → شيفرة آلة» حقيقيّة بلا LLVM.
//      يستهلك SIR الذي يبنيه الأمامُ الفعليّ من مصدر ص (في الوضع الحرّ، حيث تُسقَط
//      سِقالةُ defer/الاستثناء فتبقى شيفرةٌ خطّيّة نظيفة)، ويخفّض مجموعةً دنيا من
//      الأوپكودات (MOVE / ADD_I64 / SUB_I64 / RET) إلى بايتات x86-64 عبر المحرّك
//      الجدوليّ (lookupEncSpec + encodeVariable)، ثمّ يلفّها كاتبُ ELF في تنفيذيٍّ
//      ساكن يخرج بقيمة إرجاع `رئيسية`. لا clang/lld/as ولا زمن تشغيل.
//
//      اختيارُ التعليمات + تخصيصُ السجلّات هنا دنيا عمدًا (كتلةٌ أساسيّة واحدة،
//      «كلٌّ في سجلّ» بلا انسكاب) — كافٍ لإثبات المسار من طرفٍ لطرف. الأوپكودات
//      غير المدعومة تُفشِل التخفيضَ برسالةٍ صريحة (لا توليدَ صامتًا خاطئًا).
// (EN) SIR→native x86-64 bridge — the first real "S-lang → machine code" lowering
//      without LLVM. Consumes SIR built by the actual frontend from ص source (in
//      freestanding mode, where defer/exception scaffolding is dropped, leaving
//      clean straight-line code), lowers a minimal opcode set (MOVE/ADD_I64/
//      SUB_I64/RET) to x86-64 bytes via the table-driven encoder, then wraps them
//      in a static ELF that exits with `رئيسية`'s return value. Minimal on purpose:
//      one basic block, everything-in-registers, no spilling. Unsupported opcodes
//      fail loudly rather than miscompile silently.
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
                // (AR) نكتفي بالكتلة الأولى (entry). القفزُ متعدّدُ الكتل مؤجَّلٌ لطبقة CFG.
                const sir::SIRBasicBlock &entry = *blocks[0];

                for (const sir::SIRInstruction &inst : entry.instructions)
                {
                    if (!lowerInstruction(inst))
                        return finishError(r, errorCode_, detail_);
                    if (returned_)
                        break; // (AR) بعد RET لا شيفرةَ تُنفَّذ
                }

                if (!returned_)
                    return finishError(r, EC::INT_NATIVE_UNSUPPORTED, detailNoRet());
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
            bool returned_ = false;

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
                    if (!emit(kSys, "", {}))
                        return false;
                    returned_ = true;
                    return true;
                }
                default:
                    return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                }
            }
        };

        // (AR) منمنمات SoT العربيّة (تُرآي backend/x86_64/instructions.yaml).
        inline const char *X86SirLowering::kMov = "\xD8\xA7\xD9\x86\xD9\x82\xD9\x84";                 // انقل
        inline const char *X86SirLowering::kAdd = "\xD8\xA7\xD8\xAC\xD9\x85\xD8\xB9";                 // اجمع
        inline const char *X86SirLowering::kSub = "\xD8\xA7\xD8\xB7\xD8\xB1\xD8\xAD";                 // اطرح
        inline const char *X86SirLowering::kSys = "\xD9\x86\xD8\xAF\xD8\xA7\xD8\xA1_\xD9\x86\xD8\xB8\xD8\xA7\xD9\x85"; // نداء_نظام

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
