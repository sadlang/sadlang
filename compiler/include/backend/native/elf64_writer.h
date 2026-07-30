// ============================================================================
// (AR) كاتب ELF64 تنفيذيّ ساكن دنيا — م١ من الخلفيّة السياديّة بلا LLVM.
//      يبني ثنائيَّ ELF64 قابلًا للتنفيذ على Linux/x86-64 بلا أيّ أداةٍ أجنبيّة
//      (لا ld ولا lld): رأسُ ELF (64 بايت) + program header واحد PT_LOAD (R+X) +
//      مقطعُ الشيفرة (.text). نقطةُ الدخول تشير إلى أوّل بايتٍ من الشيفرة.
//      header-only ⇒ لا يمسّ مكتبة المصرّف. المضيفُ little-endian (كلّ أهدافنا LE).
// (EN) Minimal static ELF64 executable writer (m1 of the sovereign no-LLVM
//      backend). Emits a Linux/x86-64 runnable ELF64 with no foreign tool
//      (no ld/lld): ELF header (64B) + one PT_LOAD program header (R+X) + .text.
// ============================================================================
#ifndef SAD_NATIVE_ELF64_WRITER_H
#define SAD_NATIVE_ELF64_WRITER_H

#include <cstdint>
#include <vector>

namespace sad
{
    namespace native
    {
        namespace elf
        {
            // (AR) ثوابت رأس ELF المستعملة (تُرآي e_machine=62 في abi/x86_64-linux.yaml).
            inline constexpr uint64_t kEhSize = 64;      // (AR) حجم رأس ELF64
            inline constexpr uint64_t kPhEntSize = 56;   // (AR) حجم مدخل program header
            inline constexpr uint64_t kPhNum = 1;        // (AR) program header واحد
            inline constexpr uint16_t kEtExec = 2;       // (AR) ET_EXEC
            inline constexpr uint16_t kEmX8664 = 62;     // (AR) EM_X86_64 (تُرآي e_machine في abi/x86_64-linux.yaml)
            inline constexpr uint16_t kEmAArch64 = 183;  // (AR) EM_AARCH64 (تُرآي e_machine في abi/aarch64-linux.yaml)
            inline constexpr uint32_t kPtLoad = 1;       // (AR) PT_LOAD
            inline constexpr uint32_t kPfRX = 5;         // (AR) R + X
            inline constexpr uint64_t kDefaultVBase = 0x400000; // (AR) عنوان التحميل الافتراضيّ
            inline constexpr uint64_t kPageAlign = 0x1000;

            // (AR) إزاحة الشيفرة داخل الملفّ = رأس ELF + program headers = 120 (0x78).
            inline constexpr uint64_t kCodeOffset = kEhSize + kPhEntSize * kPhNum;

            // (AR) يبني ثنائيَّ ELF64 ساكنًا يحوي الشيفرةَ المعطاة؛ نقطةُ الدخول = vbase+0x78.
            //      e_machine يُمرَّر من جدول الـABI (٦٢ لـx86-64، ١٨٣ لـAArch64) — بذلك
            //      يكون الكاتبُ محايدَ المعماريّة: بنيةُ ELF64 للتنفيذيّ الساكن واحدةٌ عبر
            //      الأهداف، والفارقُ حقلٌ واحد (e_machine) بيانًا لا كودًا.
            // (EN) Builds a static ELF64 binary wrapping `code`; entry = vbase + 0x78.
            //      e_machine is passed from the ABI table (62 for x86-64, 183 for
            //      AArch64), making the writer architecture-neutral: the static-exec
            //      ELF64 layout is identical across targets; only e_machine is data.
            inline std::vector<uint8_t> writeStaticExec(const std::vector<uint8_t> &code,
                                                        uint16_t e_machine = kEmX8664,
                                                        uint64_t vbase = kDefaultVBase)
            {
                std::vector<uint8_t> f;
                f.reserve(kCodeOffset + code.size());

                auto put8 = [&](uint8_t v) { f.push_back(v); };
                auto put16 = [&](uint16_t v) { put8(uint8_t(v)); put8(uint8_t(v >> 8)); };
                auto put32 = [&](uint32_t v) {
                    for (int i = 0; i < 4; ++i) put8(uint8_t(v >> (8 * i)));
                };
                auto put64 = [&](uint64_t v) {
                    for (int i = 0; i < 8; ++i) put8(uint8_t(v >> (8 * i)));
                };

                const uint64_t entry = vbase + kCodeOffset;
                const uint64_t total = kCodeOffset + code.size();

                // (AR) e_ident[16]: السحر 7F 'E' 'L' 'F'، class=2 (64)، data=1 (LE)، version=1
                put8(0x7F); put8('E'); put8('L'); put8('F');
                put8(2); put8(1); put8(1); put8(0);
                for (int i = 0; i < 8; ++i) put8(0);

                put16(kEtExec);            // e_type
                put16(e_machine);          // e_machine (من جدول الـABI)
                put32(1);                  // e_version
                put64(entry);              // e_entry
                put64(kEhSize);            // e_phoff = 64
                put64(0);                  // e_shoff (لا أقسام)
                put32(0);                  // e_flags
                put16(uint16_t(kEhSize));  // e_ehsize = 64
                put16(uint16_t(kPhEntSize)); // e_phentsize = 56
                put16(uint16_t(kPhNum));   // e_phnum = 1
                put16(0);                  // e_shentsize
                put16(0);                  // e_shnum
                put16(0);                  // e_shstrndx

                // (AR) program header وحيد PT_LOAD (R+X) يغطّي الرأسَ والشيفرة
                put32(kPtLoad);            // p_type
                put32(kPfRX);              // p_flags = R + X
                put64(0);                  // p_offset
                put64(vbase);              // p_vaddr
                put64(vbase);              // p_paddr
                put64(total);              // p_filesz
                put64(total);              // p_memsz
                put64(kPageAlign);         // p_align

                // (AR) مقطع الشيفرة .text
                f.insert(f.end(), code.begin(), code.end());
                return f;
            }
        } // namespace elf
    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_ELF64_WRITER_H
