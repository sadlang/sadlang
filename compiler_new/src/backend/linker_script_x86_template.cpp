#include <sstream>
#include <string>
#include <cstdint>

namespace sad
{
    namespace compiler
    {
        namespace backend
        {

            std::string buildX86BareMetalLinkerScript(uint64_t loadAddress)
            {
                std::ostringstream script;

                script << "/* " << "سكريبت رابط x86 bare-metal للغة ص" << " */\n";
                script << "/* Sad Language x86 Bare-metal Linker Script */\n\n";

                script << "OUTPUT_FORMAT(elf64-x86-64)\n";
                script << "ENTRY(_start)\n\n";

                script << "SECTIONS\n";
                script << "{\n";
                script << "    . = " << std::hex << "0x" << loadAddress << ";\n\n";

                script << "    /* " << "قسم الإقلاع" << " */\n";
                script << "    .boot : {\n";
                script << "        KEEP(*(.multiboot))\n";
                script << "        KEEP(*(.multiboot2))\n";
                script << "    }\n\n";

                script << "    /* " << "قسم الكود" << " */\n";
                script << "    .text : {\n";
                script << "        *(.text._start)\n";
                script << "        *(.text.boot)\n";
                script << "        *(.text*)\n";
                script << "    }\n\n";

                script << "    /* " << "بيانات للقراءة فقط" << " */\n";
                script << "    .rodata : {\n";
                script << "        *(.rodata*)\n";
                script << "    }\n\n";

                script << "    . = ALIGN(4096);\n\n";

                script << "    /* " << "بيانات مهيأة" << " */\n";
                script << "    .data : {\n";
                script << "        *(.data*)\n";
                script << "    }\n\n";

                script << "    /* " << "بيانات غير مهيأة" << " */\n";
                script << "    .bss : {\n";
                script << "        __bss_start = .;\n";
                script << "        *(.bss*)\n";
                script << "        *(COMMON)\n";
                script << "        __bss_end = .;\n";
                script << "    }\n\n";

                script << "    . = ALIGN(16);\n";
                script << "    __stack_bottom = .;\n";
                script << "    . += 0x10000;  /* 64KB stack */\n";
                script << "    __stack_top = .;\n\n";

                script << "    __kernel_end = .;\n";
                script << "}\n";

                return script.str();
            }

        } // namespace backend
    } // namespace compiler
} // namespace sad
