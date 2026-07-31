// ============================================================================
// (AR) مولَّدٌ آليًّا من language-truth/backend/x86_64/instructions.yaml — لا تُحرّره.
//      أعِد توليدَه بـ: python x.py gen  (وحارسُ الانجراف: python x.py gen --check).
// (EN) AUTO-GENERATED from the SoT instruction-encoding YAML. DO NOT EDIT.
//      Regenerate with: python x.py gen   (drift guard: python x.py gen --check).
// ============================================================================

#ifndef SAD_NATIVE_X86_64_ENCODING_GENERATED_H
#define SAD_NATIVE_X86_64_ENCODING_GENERATED_H

#include "backend/native/x86_variable_encoder.h"

#include <string>
#include <vector>

namespace sad { namespace native { namespace x86 {

// (AR) جدول ترميز x86_64 (variable) المولَّد من SoT (منمنمة، صيغة، EncSpec).
struct GenEncEntry { std::string mnemonic; std::string form; EncSpec spec; };

inline const std::vector<GenEncEntry> &encodingTable()
{
    static const std::vector<GenEncEntry> kTable = {
    { "انقل", "r32, imm32", []{ sad::native::x86::EncSpec s; s.opcode = {0xB8}; s.opcode_reg_add = true; s.opcode_reg_op = 0; s.imm_op = 1; s.imm_bits = 32; return s; }() },
    { "انقل", "r64, imm64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xB8}; s.opcode_reg_add = true; s.opcode_reg_op = 0; s.imm_op = 1; s.imm_bits = 64; return s; }() },
    { "انقل", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x89}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "انقل", "r64, m64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x8B}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "انقل", "m64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x89}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "انقل", "m8, r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x88}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "اجمع", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x01}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "اجمع", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x83}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "اجمع", "r64, imm32", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x81}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 32; return s; }() },
    { "نداء_نظام", "", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x05}; return s; }() },
    { "ارجع", "", []{ sad::native::x86::EncSpec s; s.opcode = {0xC3}; return s; }() },
    { "ادفع", "r64", []{ sad::native::x86::EncSpec s; s.opcode = {0x50}; s.opcode_reg_add = true; s.opcode_reg_op = 0; return s; }() },
    { "اسحب", "r64", []{ sad::native::x86::EncSpec s; s.opcode = {0x58}; s.opcode_reg_add = true; s.opcode_reg_op = 0; return s; }() },
    { "اطرح", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x83}; s.modrm.present = true; s.modrm.reg_ext = 5; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "اطرح", "r64, imm32", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x81}; s.modrm.present = true; s.modrm.reg_ext = 5; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 32; return s; }() },
    { "اطرح", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x29}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "اضرب", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x0F, 0xAF}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "اقسم", "r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xF7}; s.modrm.present = true; s.modrm.reg_ext = 7; s.modrm.rm_op = 0; return s; }() },
    { "مدد_الإشارة", "", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x99}; return s; }() },
    { "وافق", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x21}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "اضمم", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x09}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "غاير", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x31}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "اعكس", "r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xF7}; s.modrm.present = true; s.modrm.reg_ext = 2; s.modrm.rm_op = 0; return s; }() },
    { "أزح_يسار", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xC1}; s.modrm.present = true; s.modrm.reg_ext = 4; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "أزح_يمين", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xC1}; s.modrm.present = true; s.modrm.reg_ext = 5; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "قارن", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x39}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "قارن", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x83}; s.modrm.present = true; s.modrm.reg_ext = 7; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "قارن", "r64, imm32", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x81}; s.modrm.present = true; s.modrm.reg_ext = 7; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 32; return s; }() },
    { "اقفز", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0xE9}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "اقفز_إذا_ساوى", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x84}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "اقفز_إذا_لم_يساوِ", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x85}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "اقفز_إذا_أصغر", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x8C}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "اقفز_إذا_أكبر_أو_ساوى", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x8D}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "اقفز_إذا_أصغر_أو_ساوى", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x8E}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "اقفز_إذا_أكبر", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x8F}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "نادِ", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0xE8}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    };
    return kTable;
}

// (AR) ثوابتُ أسماءِ التعليمات (منمنمات) المولَّدة من SoT — تُستهلَك في المُخفِّض
//      بدلَ تأليفِ السلسلةِ العربيّةِ يدويًّا (منعُ انجرافِ الاسم عن مصدر الحقيقة).
// (EN) Instruction-name constants generated from SoT — consumed by the lowering
//      pass instead of hand-authored Arabic strings (prevents name drift).
namespace mnem {
inline const std::string kMov = "انقل";
inline const std::string kAdd = "اجمع";
inline const std::string kSyscall = "نداء_نظام";
inline const std::string kRet = "ارجع";
inline const std::string kPush = "ادفع";
inline const std::string kPop = "اسحب";
inline const std::string kSub = "اطرح";
inline const std::string kImul = "اضرب";
inline const std::string kIdiv = "اقسم";
inline const std::string kCqo = "مدد_الإشارة";
inline const std::string kAnd = "وافق";
inline const std::string kOr = "اضمم";
inline const std::string kXor = "غاير";
inline const std::string kNot = "اعكس";
inline const std::string kShl = "أزح_يسار";
inline const std::string kShr = "أزح_يمين";
inline const std::string kCmp = "قارن";
inline const std::string kJmp = "اقفز";
inline const std::string kJe = "اقفز_إذا_ساوى";
inline const std::string kJne = "اقفز_إذا_لم_يساوِ";
inline const std::string kJl = "اقفز_إذا_أصغر";
inline const std::string kJge = "اقفز_إذا_أكبر_أو_ساوى";
inline const std::string kJle = "اقفز_إذا_أصغر_أو_ساوى";
inline const std::string kJg = "اقفز_إذا_أكبر";
inline const std::string kCall = "نادِ";
} // namespace mnem

// (AR) بحثٌ عن مواصفة الترميز بالمنمنمة والصيغة؛ يعيد nullptr إن لم تُوجد.
// (EN) look up an encoding spec by mnemonic + form; nullptr if absent.
inline const EncSpec *lookupEncSpec(const std::string &mnemonic, const std::string &form)
{
    for (const auto &e : encodingTable())
        if (e.mnemonic == mnemonic && e.form == form)
            return &e.spec;
    return nullptr;
}

}}} // namespace sad::native::x86

#endif // SAD_NATIVE_X86_64_ENCODING_GENERATED_H
