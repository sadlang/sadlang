// ============================================================================
// (AR) مُولَّد آليًّا من language-truth/backend/arch_specific_opcodes.yaml
//      لا يُحرَّر يدويًّا — عدِّل الـYAML ثمّ أعِد التوليد (`x.py gen`).
// (EN) Auto-generated. Do not edit by hand.
// ============================================================================
#pragma once

#include <array>
#include <string_view>

// (AR) تعدادُ الأوپكودات — المفتاحُ قيمةٌ لا نصّ.
#include "frontend/sir_types.h"

namespace Sad { namespace Backend {

// (AR) قيدُ عائلةِ معالجٍ على خفضِ أوپكود.
// (EN) A CPU-family constraint on an opcode's lowering.
struct ArchConstrainedOpcode
{
    std::string_view opcode;   ///< (AR) اسمُ الأوپكود كما في SIROpcode
    std::string_view familyId; ///< (AR) معرّفُ العائلة (x86…)
    std::string_view familyAr; ///< (AR) اسمُ العائلة عربيًّا — لنصّ التشخيص
    std::string_view witness;  ///< (AR) منمنمةٌ شاهدةٌ على القيد
};

inline constexpr std::array<ArchConstrainedOpcode, 34> ARCH_CONSTRAINED_OPCODES = {{
    {"BUILTIN_CLI", "x86", "عائلة x86 (i686 وx86_64)", "cli"},
    {"BUILTIN_CPUID", "x86", "عائلة x86 (i686 وx86_64)", "cpuid"},
    {"BUILTIN_DMA_INIT", "x86", "عائلة x86 (i686 وx86_64)", "outb"},
    {"BUILTIN_DMA_START", "x86", "عائلة x86 (i686 وx86_64)", "outb"},
    {"BUILTIN_HALT", "x86", "عائلة x86 (i686 وx86_64)", "hlt"},
    {"BUILTIN_INTERRUPT", "x86", "عائلة x86 (i686 وx86_64)", "int N"},
    {"BUILTIN_LFENCE", "x86", "عائلة x86 (i686 وx86_64)", "lfence"},
    {"BUILTIN_MEM_FILL_32", "x86", "عائلة x86 (i686 وx86_64)", "rep stosl"},
    {"BUILTIN_MFENCE", "x86", "عائلة x86 (i686 وx86_64)", "mfence"},
    {"BUILTIN_PORT_READ", "x86", "عائلة x86 (i686 وx86_64)", "inb"},
    {"BUILTIN_PORT_READ_16", "x86", "عائلة x86 (i686 وx86_64)", "inw"},
    {"BUILTIN_PORT_READ_32", "x86", "عائلة x86 (i686 وx86_64)", "inl"},
    {"BUILTIN_PORT_WRITE", "x86", "عائلة x86 (i686 وx86_64)", "outb"},
    {"BUILTIN_PORT_WRITE_16", "x86", "عائلة x86 (i686 وx86_64)", "outw"},
    {"BUILTIN_PORT_WRITE_32", "x86", "عائلة x86 (i686 وx86_64)", "outl"},
    {"BUILTIN_RDTSC", "x86", "عائلة x86 (i686 وx86_64)", "rdtsc"},
    {"BUILTIN_RESET", "x86", "عائلة x86 (i686 وx86_64)", "outb"},
    {"BUILTIN_SERIAL_INIT", "x86", "عائلة x86 (i686 وx86_64)", "outb"},
    {"BUILTIN_SERIAL_READ", "x86", "عائلة x86 (i686 وx86_64)", "inb"},
    {"BUILTIN_SERIAL_READY", "x86", "عائلة x86 (i686 وx86_64)", "inb"},
    {"BUILTIN_SERIAL_WRITE", "x86", "عائلة x86 (i686 وx86_64)", "outb"},
    {"BUILTIN_SFENCE", "x86", "عائلة x86 (i686 وx86_64)", "sfence"},
    {"BUILTIN_STI", "x86", "عائلة x86 (i686 وx86_64)", "sti"},
    {"BUILTIN_TIMER_INIT", "x86", "عائلة x86 (i686 وx86_64)", "outb"},
    {"BUILTIN_TIMER_READ", "x86", "عائلة x86 (i686 وx86_64)", "inb"},
    {"BUILTIN_TIMER_WAIT", "x86", "عائلة x86 (i686 وx86_64)", "rdtsc"},
    {"LOWLEVEL_CPU_INVLPG", "x86", "عائلة x86 (i686 وx86_64)", "invlpg"},
    {"LOWLEVEL_CPU_READ_CR", "x86", "عائلة x86 (i686 وx86_64)", "mov %crN"},
    {"LOWLEVEL_CPU_READ_MSR", "x86", "عائلة x86 (i686 وx86_64)", "rdmsr"},
    {"LOWLEVEL_CPU_WRITE_CR", "x86", "عائلة x86 (i686 وx86_64)", "mov %crN"},
    {"LOWLEVEL_CPU_WRITE_MSR", "x86", "عائلة x86 (i686 وx86_64)", "wrmsr"},
    {"LOWLEVEL_GDT_LOAD", "x86", "عائلة x86 (i686 وx86_64)", "lgdt"},
    {"LOWLEVEL_IDT_LOAD", "x86", "عائلة x86 (i686 وx86_64)", "lidt"},
    {"LOWLEVEL_PAGING_FLUSH_TLB", "x86", "عائلة x86 (i686 وx86_64)", "mov %cr3"},
}};

// (AR) أسماءُ llvm::Triple::ArchType المقبولةُ لعائلة x86.
inline constexpr std::array<std::string_view, 2> X86_LLVM_ARCHS = {{
    "i386",
    "x86_64",
}};

// (AR) يعيد القيدَ إن كان الأوپكودُ مقيَّدًا، وإلّا nullptr.
//
//      ⚠️ المفتاحُ **قيمةُ التعداد** لا نصُّ الاسم. أوّلُ صياغةٍ طابقت
//      اسمَ الأوپكود نصًّا عبر `sirOpcodeToString` — وتلك تعيد المنمنمة
//      (`add.i64`) لا اسمَ التعداد، فلم تطابق ولا مرّةً واحدة: بوّابةٌ
//      مبنيّةٌ تمامًا ولا تحرس شيئًا، وخضرةٌ بصفرِ تطابق. وبقيمة التعداد
//      يصير الاسمُ الخاطئُ في مصدر الحقيقة **خطأَ ترجمة** لا صمتًا.
// (EN) Keyed on the enum value, not the name: sirOpcodeToString returns
//      the mnemonic, so a string match never fired. A wrong name in the
//      SoT is now a compile error instead of a silent miss.
inline const ArchConstrainedOpcode* findArchConstraint(::Sad::Compiler::SIR::SIROpcode opcode)
{
    using Op = ::Sad::Compiler::SIR::SIROpcode;
    switch (opcode)
    {
    case Op::BUILTIN_CLI: return &ARCH_CONSTRAINED_OPCODES[0];
    case Op::BUILTIN_CPUID: return &ARCH_CONSTRAINED_OPCODES[1];
    case Op::BUILTIN_DMA_INIT: return &ARCH_CONSTRAINED_OPCODES[2];
    case Op::BUILTIN_DMA_START: return &ARCH_CONSTRAINED_OPCODES[3];
    case Op::BUILTIN_HALT: return &ARCH_CONSTRAINED_OPCODES[4];
    case Op::BUILTIN_INTERRUPT: return &ARCH_CONSTRAINED_OPCODES[5];
    case Op::BUILTIN_LFENCE: return &ARCH_CONSTRAINED_OPCODES[6];
    case Op::BUILTIN_MEM_FILL_32: return &ARCH_CONSTRAINED_OPCODES[7];
    case Op::BUILTIN_MFENCE: return &ARCH_CONSTRAINED_OPCODES[8];
    case Op::BUILTIN_PORT_READ: return &ARCH_CONSTRAINED_OPCODES[9];
    case Op::BUILTIN_PORT_READ_16: return &ARCH_CONSTRAINED_OPCODES[10];
    case Op::BUILTIN_PORT_READ_32: return &ARCH_CONSTRAINED_OPCODES[11];
    case Op::BUILTIN_PORT_WRITE: return &ARCH_CONSTRAINED_OPCODES[12];
    case Op::BUILTIN_PORT_WRITE_16: return &ARCH_CONSTRAINED_OPCODES[13];
    case Op::BUILTIN_PORT_WRITE_32: return &ARCH_CONSTRAINED_OPCODES[14];
    case Op::BUILTIN_RDTSC: return &ARCH_CONSTRAINED_OPCODES[15];
    case Op::BUILTIN_RESET: return &ARCH_CONSTRAINED_OPCODES[16];
    case Op::BUILTIN_SERIAL_INIT: return &ARCH_CONSTRAINED_OPCODES[17];
    case Op::BUILTIN_SERIAL_READ: return &ARCH_CONSTRAINED_OPCODES[18];
    case Op::BUILTIN_SERIAL_READY: return &ARCH_CONSTRAINED_OPCODES[19];
    case Op::BUILTIN_SERIAL_WRITE: return &ARCH_CONSTRAINED_OPCODES[20];
    case Op::BUILTIN_SFENCE: return &ARCH_CONSTRAINED_OPCODES[21];
    case Op::BUILTIN_STI: return &ARCH_CONSTRAINED_OPCODES[22];
    case Op::BUILTIN_TIMER_INIT: return &ARCH_CONSTRAINED_OPCODES[23];
    case Op::BUILTIN_TIMER_READ: return &ARCH_CONSTRAINED_OPCODES[24];
    case Op::BUILTIN_TIMER_WAIT: return &ARCH_CONSTRAINED_OPCODES[25];
    case Op::LOWLEVEL_CPU_INVLPG: return &ARCH_CONSTRAINED_OPCODES[26];
    case Op::LOWLEVEL_CPU_READ_CR: return &ARCH_CONSTRAINED_OPCODES[27];
    case Op::LOWLEVEL_CPU_READ_MSR: return &ARCH_CONSTRAINED_OPCODES[28];
    case Op::LOWLEVEL_CPU_WRITE_CR: return &ARCH_CONSTRAINED_OPCODES[29];
    case Op::LOWLEVEL_CPU_WRITE_MSR: return &ARCH_CONSTRAINED_OPCODES[30];
    case Op::LOWLEVEL_GDT_LOAD: return &ARCH_CONSTRAINED_OPCODES[31];
    case Op::LOWLEVEL_IDT_LOAD: return &ARCH_CONSTRAINED_OPCODES[32];
    case Op::LOWLEVEL_PAGING_FLUSH_TLB: return &ARCH_CONSTRAINED_OPCODES[33];
    default: return nullptr;
    }
}

// (AR) هل معماريّةُ الهدف (باسم llvm::Triple) داخلَ العائلة المطلوبة؟
inline bool archSatisfiesFamily(std::string_view familyId, std::string_view arch)
{
    if (familyId == "x86")
    {
        for (const auto& a : X86_LLVM_ARCHS)
            if (a == arch) return true;
        return false;
    }
    // (AR) لا يُبلَغ: المُولِّدُ يُخفِق على أوپكودٍ يشير إلى عائلةٍ غيرِ
    //      معلَنة، و`familyId` هنا يأتي دائمًا من `families`. و`false`
    //      لا `true` كي يكون البلوغُ المستحيلُ **رفضًا صريحًا** إن وقع،
    //      لا قيدًا يسقط صامتًا.
    // (EN) Unreachable: the generator rejects an unknown family. false,
    //      not true, so an impossible reach fails loudly.
    return false;
}

} } // namespace Sad::Backend
