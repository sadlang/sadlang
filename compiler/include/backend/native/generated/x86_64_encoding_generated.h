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
    { "انفِ", "r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xF7}; s.modrm.present = true; s.modrm.reg_ext = 3; s.modrm.rm_op = 0; return s; }() },
    { "أزح_يمين_حسابي", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xC1}; s.modrm.present = true; s.modrm.reg_ext = 7; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "أزح_يمين_حسابي", "r64, cl", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xD3}; s.modrm.present = true; s.modrm.reg_ext = 7; s.modrm.rm_op = 0; return s; }() },
    { "أزح_يسار", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xC1}; s.modrm.present = true; s.modrm.reg_ext = 4; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "أزح_يسار", "r64, cl", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xD3}; s.modrm.present = true; s.modrm.reg_ext = 4; s.modrm.rm_op = 0; return s; }() },
    { "أزح_يمين", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xC1}; s.modrm.present = true; s.modrm.reg_ext = 5; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "أزح_يمين", "r64, cl", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xD3}; s.modrm.present = true; s.modrm.reg_ext = 5; s.modrm.rm_op = 0; return s; }() },
    { "لِفّ_يسار", "r64, imm8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xC1}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; s.imm_op = 1; s.imm_bits = 8; return s; }() },
    { "لِفّ_يسار", "r64, cl", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0xD3}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_ساوى", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x94}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_لم_يساوِ", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x95}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_أصغر", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x9C}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_أصغر_أو_ساوى", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x9E}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_أكبر", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x9F}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_أكبر_أو_ساوى", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x9D}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_فوق", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x97}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_فوق_أو_ساوى", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x93}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_لا_تكافؤ", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x9B}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "عيّن_إذا_تكافؤ", "r8", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x9A}; s.modrm.present = true; s.modrm.reg_ext = 0; s.modrm.rm_op = 0; return s; }() },
    { "اختبر", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x85}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "مدد_بالصفر", "r64, r8", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x0F, 0xB6}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "قارن", "r64, m64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x3B}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
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
    { "اقفز_إذا_أدنى", "rel8", []{ sad::native::x86::EncSpec s; s.opcode = {0x72}; s.imm_op = 0; s.imm_bits = 8; return s; }() },
    { "نادِ", "rel32", []{ sad::native::x86::EncSpec s; s.opcode = {0xE8}; s.imm_op = 0; s.imm_bits = 32; return s; }() },
    { "عنوان_فعّال", "r64, m64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x8D}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "عبّئ_عشري", "xmm, r64", []{ sad::native::x86::EncSpec s; s.prefix = {0x66}; s.rex_w = true; s.opcode = {0x0F, 0x6E}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "استخرج_عشري", "r64, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0x66}; s.rex_w = true; s.opcode = {0x0F, 0x7E}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "حمّل_عشري", "xmm, m64", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.opcode = {0x0F, 0x10}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "خزّن_عشري", "m64, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.opcode = {0x0F, 0x11}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "اجمع_عشري", "xmm, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.opcode = {0x0F, 0x58}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "اطرح_عشري", "xmm, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.opcode = {0x0F, 0x5C}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "اضرب_عشري", "xmm, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.opcode = {0x0F, 0x59}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "اقسم_عشري", "xmm, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.opcode = {0x0F, 0x5E}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "حوّل_إلى_عشري", "xmm, r64", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.rex_w = true; s.opcode = {0x0F, 0x2A}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "حوّل_من_عشري", "r64, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.rex_w = true; s.opcode = {0x0F, 0x2C}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "قارن_عشري", "xmm, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0x66}; s.opcode = {0x0F, 0x2E}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
    { "قرّب_عشري", "r64, xmm", []{ sad::native::x86::EncSpec s; s.prefix = {0xF2}; s.rex_w = true; s.opcode = {0x0F, 0x2D}; s.modrm.present = true; s.modrm.reg_op = 0; s.modrm.rm_op = 1; return s; }() },
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
inline const std::string kNeg = "انفِ";
inline const std::string kSar = "أزح_يمين_حسابي";
inline const std::string kShl = "أزح_يسار";
inline const std::string kShr = "أزح_يمين";
inline const std::string kRol = "لِفّ_يسار";
inline const std::string kSete = "عيّن_إذا_ساوى";
inline const std::string kSetne = "عيّن_إذا_لم_يساوِ";
inline const std::string kSetl = "عيّن_إذا_أصغر";
inline const std::string kSetle = "عيّن_إذا_أصغر_أو_ساوى";
inline const std::string kSetg = "عيّن_إذا_أكبر";
inline const std::string kSetge = "عيّن_إذا_أكبر_أو_ساوى";
inline const std::string kSeta = "عيّن_إذا_فوق";
inline const std::string kSetae = "عيّن_إذا_فوق_أو_ساوى";
inline const std::string kSetnp = "عيّن_إذا_لا_تكافؤ";
inline const std::string kSetp = "عيّن_إذا_تكافؤ";
inline const std::string kTest = "اختبر";
inline const std::string kMovzx = "مدد_بالصفر";
inline const std::string kCmp = "قارن";
inline const std::string kJmp = "اقفز";
inline const std::string kJe = "اقفز_إذا_ساوى";
inline const std::string kJne = "اقفز_إذا_لم_يساوِ";
inline const std::string kJl = "اقفز_إذا_أصغر";
inline const std::string kJge = "اقفز_إذا_أكبر_أو_ساوى";
inline const std::string kJle = "اقفز_إذا_أصغر_أو_ساوى";
inline const std::string kJg = "اقفز_إذا_أكبر";
inline const std::string kJb = "اقفز_إذا_أدنى";
inline const std::string kCall = "نادِ";
inline const std::string kLea = "عنوان_فعّال";
inline const std::string kMovqXmmR64 = "عبّئ_عشري";
inline const std::string kMovqR64Xmm = "استخرج_عشري";
inline const std::string kMovsdLoad = "حمّل_عشري";
inline const std::string kMovsdStore = "خزّن_عشري";
inline const std::string kAddsd = "اجمع_عشري";
inline const std::string kSubsd = "اطرح_عشري";
inline const std::string kMulsd = "اضرب_عشري";
inline const std::string kDivsd = "اقسم_عشري";
inline const std::string kCvtsi2sd = "حوّل_إلى_عشري";
inline const std::string kCvttsd2si = "حوّل_من_عشري";
inline const std::string kUcomisd = "قارن_عشري";
inline const std::string kCvtsd2si = "قرّب_عشري";
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
