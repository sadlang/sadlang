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
    { "اجمع", "r64, r64", []{ sad::native::x86::EncSpec s; s.rex_w = true; s.opcode = {0x01}; s.modrm.present = true; s.modrm.reg_op = 1; s.modrm.rm_op = 0; return s; }() },
    { "نداء_نظام", "", []{ sad::native::x86::EncSpec s; s.opcode = {0x0F, 0x05}; return s; }() },
    { "ارجع", "", []{ sad::native::x86::EncSpec s; s.opcode = {0xC3}; return s; }() },
    };
    return kTable;
}

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
