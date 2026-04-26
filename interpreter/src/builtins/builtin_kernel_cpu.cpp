/**
 * @file builtin_kernel_cpu.cpp
 * @brief (AR) وحدة المعالج — إدارة سجلات ومقاطعات وجداول وصف المعالج
 * @brief (EN) CPU module — registers, interrupts, descriptor tables
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "builtins.h"
#include "interpreter_core.h"

// Low-level headers
#include "cpu.h"
#include "io_ports.h"
#include "bitwise_ops.h"
#include "memory_map.h"
#include "interrupts.h"
#include "pointer_type.h"
#include "gdt.h"
#include "paging.h"
#include "pci.h"
#include "dma.h"
#include "framebuffer.h"
#include "acpi.h"
#include "sync.h"
#include "scheduler.h"
#include "boot.h"
#include "vfs.h"
#include "apic.h"
#include "hpet.h"
#include "syscall.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

void registerBuiltinsKernelCPU(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // 1. وحدة المعالج / CPU Module
    // ═══════════════════════════════════════════════════════════════
    auto cpu_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& cpu = LowLevel::CPUManager::getInstance();
        return std::make_shared<Data::Value>(cpu.generateReport());
    };
    fm.registerBuiltinFunction("معالج_معلومات", cpu_info);

    // إيقاف المعالج (تعليمة HLT)
    auto cpu_halt = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::CPUManager::halt();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("معالج_توقف", cpu_halt);

    // قراءة عداد الطوابع الزمنية (TSC)
    auto cpu_rdtsc = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        uint64_t tsc = LowLevel::CPUManager::readTSC();
        return std::make_shared<Data::Value>(static_cast<double>(tsc));
    };
    fm.registerBuiltinFunction("معالج_عداد", cpu_rdtsc);

    // تعطيل المقاطعات (CLI)
    auto cpu_cli = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::CPUManager::disableInterrupts();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تعطيل_مقاطعات", cpu_cli);

    // تفعيل المقاطعات (STI)
    auto cpu_sti = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::CPUManager::enableInterrupts();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تفعيل_مقاطعات", cpu_sti);

    // قراءة سجل MSR بالرقم المحدد
    auto cpu_read_msr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("معالج_اقرأ_msr: يحتاج رقم MSR");
        uint32_t msr = static_cast<uint32_t>(args[0]->toInt());
        uint64_t val = LowLevel::CPUManager::readMSR(msr);
        return std::make_shared<Data::Value>(static_cast<double>(val));
    };
    fm.registerBuiltinFunction("معالج_اقرأ_msr", cpu_read_msr);

    // كتابة قيمة في سجل MSR
    auto cpu_write_msr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("معالج_اكتب_msr: يحتاج رقم MSR وقيمة");
        uint32_t msr = static_cast<uint32_t>(args[0]->toInt());
        uint64_t val = static_cast<uint64_t>(args[1]->toDouble());
        LowLevel::CPUManager::writeMSR(msr, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("معالج_اكتب_msr", cpu_write_msr);

    // إبطال صفحة ذاكرة محددة (INVLPG)
    auto cpu_invlpg = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("cpu_invlpg: يحتاج عنوان");
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        LowLevel::CPUManager::invlpg(addr);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("ابطل_صفحة", cpu_invlpg);

    // ═══════════════════════════════════════════════════════════════
    // 2. منافذ الإدخال/الإخراج / IO Ports
    // ═══════════════════════════════════════════════════════════════
    auto io_inb = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ: يحتاج رقم المنفذ");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint8_t val = LowLevel::IOPorts::inb(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("منفذ_اقرأ", io_inb);

    // كتابة بايت واحد إلى منفذ الإخراج (outb)
    auto io_outb = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب: يحتاج منفذ وقيمة");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint8_t val = static_cast<uint8_t>(args[1]->toInt());
        LowLevel::IOPorts::outb(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("منفذ_اكتب", io_outb);

    // قراءة كلمة 16-بت من منفذ الإدخال (inw)
    auto io_inw = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ16: يحتاج رقم المنفذ");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint16_t val = LowLevel::IOPorts::inw(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("منفذ_اقرأ16", io_inw);

    // كتابة كلمة 16-بت إلى منفذ الإخراج (outw)
    auto io_outw = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب16: يحتاج منفذ وقيمة");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint16_t val = static_cast<uint16_t>(args[1]->toInt());
        LowLevel::IOPorts::outw(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("منفذ_اكتب16", io_outw);

    // قراءة كلمة مزدوجة 32-بت من منفذ الإدخال (inl)
    auto io_inl = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ32: يحتاج رقم المنفذ");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint32_t val = LowLevel::IOPorts::inl(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("منفذ_اقرأ32", io_inl);

    // كتابة كلمة مزدوجة 32-بت إلى منفذ الإخراج (outl)
    auto io_outl = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب32: يحتاج منفذ وقيمة");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint32_t val = static_cast<uint32_t>(args[1]->toInt());
        LowLevel::IOPorts::outl(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("منفذ_اكتب32", io_outl);

    // ═══════════════════════════════════════════════════════════════
    // 3. العمليات البتية / Bitwise Operations
    // ═══════════════════════════════════════════════════════════════
    auto bit_and = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_و: يحتاج قيمتين");
        return std::make_shared<Data::Value>(args[0]->toInt() & args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_و", bit_and);

    // عملية OR بتية بين قيمتين
    auto bit_or = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_أو: يحتاج قيمتين");
        return std::make_shared<Data::Value>(args[0]->toInt() | args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_أو", bit_or);

    // عملية XOR بتية حصرية بين قيمتين
    auto bit_xor = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_حصري: يحتاج قيمتين");
        return std::make_shared<Data::Value>(args[0]->toInt() ^ args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_حصري", bit_xor);

    // عملية NOT بتية (نفي بتي)
    auto bit_not = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("بت_نفي: يحتاج قيمة");
        return std::make_shared<Data::Value>(~args[0]->toInt());
    };
    fm.registerBuiltinFunction("بت_نفي", bit_not);

    // إزاحة بتية لليسار (SHL)
    auto bit_shl = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_يسار: يحتاج قيمة وعدد");
        return std::make_shared<Data::Value>(args[0]->toInt() << args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_يسار", bit_shl);

    // إزاحة بتية لليمين (SHR)
    auto bit_shr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_يمين: يحتاج قيمة وعدد");
        return std::make_shared<Data::Value>(args[0]->toInt() >> args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_يمين", bit_shr);

    // اختبار بت محدد في قيمة
    auto bit_test = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_اختبار: يحتاج قيمة ورقم بت");
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>((val >> bit) & 1);
    };
    fm.registerBuiltinFunction("بت_اختبار", bit_test);

    // تعيين بت محدد في قيمة
    auto bit_set = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_عيّن: يحتاج قيمة ورقم بت");
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>(val | (1 << bit));
    };
    fm.registerBuiltinFunction("بت_عيّن", bit_set);

    // مسح بت محدد في قيمة
    auto bit_clear = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_امسح: يحتاج قيمة ورقم بت");
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>(val & ~(1 << bit));
    };
    fm.registerBuiltinFunction("بت_امسح", bit_clear);

    // ═══════════════════════════════════════════════════════════════
    // 4. إدارة الذاكرة / Memory Management
    // ═══════════════════════════════════════════════════════════════
    auto mem_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mm = LowLevel::MemoryManager::getInstance();
        return std::make_shared<Data::Value>(mm.getMemoryReport());
    };
    fm.registerBuiltinFunction("ذاكرة_معلومات", mem_info);

    // قراءة بايت من عنوان ذاكرة مباشر (peek)
    auto mem_peek = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ذاكرة_اقرأ: يحتاج عنوان");
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        uint8_t val = *reinterpret_cast<volatile uint8_t*>(addr);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("ذاكرة_اقرأ", mem_peek);

    // كتابة بايت إلى عنوان ذاكرة مباشر (poke)
    auto mem_poke = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("ذاكرة_اكتب: يحتاج عنوان وقيمة");
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        uint8_t val = static_cast<uint8_t>(args[1]->toInt());
        *reinterpret_cast<volatile uint8_t*>(addr) = val;
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("ذاكرة_اكتب", mem_poke);

    // ═══════════════════════════════════════════════════════════════
    // 5. المقاطعات / Interrupts
    // ═══════════════════════════════════════════════════════════════
    auto int_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& im = LowLevel::InterruptManager::getInstance();
        return std::make_shared<Data::Value>(im.getInterruptReport());
    };
    fm.registerBuiltinFunction("مقاطعات_معلومات", int_info);

    // تهيئة جدول المقاطعات IDT وتحميله
    auto int_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& im = LowLevel::InterruptManager::getInstance();
        im.initialize();
        im.loadIDT();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("مقاطعات_تهيئة", int_init);

    // ═══════════════════════════════════════════════════════════════
    // 6. GDT / TSS
    // ═══════════════════════════════════════════════════════════════
    auto gdt_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& gdt = LowLevel::GDTManager::getInstance();
        return std::make_shared<Data::Value>(gdt.generateReport());
    };

    // تهيئة جدول الواصفات العام GDT
    auto gdt_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& gdt = LowLevel::GDTManager::getInstance();
        gdt.initialize();
        return std::make_shared<Data::Value>(0);
    };

    // ═══════════════════════════════════════════════════════════════
    // 7. الترحيل (Paging)
    // ═══════════════════════════════════════════════════════════════
    auto paging_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& pg = LowLevel::PagingManager::getInstance();
        return std::make_shared<Data::Value>(pg.generateReport());
    };
    fm.registerBuiltinFunction("ترحيل_معلومات", paging_info);

    // ربط صفحة افتراضية بعنوان فيزيائي
    auto paging_map = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("ترحيل_خريطة: يحتاج عنوان افتراضي وفيزيائي");
        uint64_t virt = static_cast<uint64_t>(args[0]->toDouble());
        uint64_t phys = static_cast<uint64_t>(args[1]->toDouble());
        uint64_t flags = args.size() > 2 ? static_cast<uint64_t>(args[2]->toInt()) : 0x3;
        auto& pg = LowLevel::PagingManager::getInstance();
        bool ok = pg.mapPage(virt, phys, flags);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ترحيل_خريطة", paging_map);

    // إلغاء ربط صفحة افتراضية
    auto paging_unmap = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ترحيل_إلغاء: يحتاج عنوان افتراضي");
        uint64_t virt = static_cast<uint64_t>(args[0]->toDouble());
        auto& pg = LowLevel::PagingManager::getInstance();
        pg.unmapPage(virt);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("ترحيل_إلغاء", paging_unmap);

    // ═══════════════════════════════════════════════════════════════
    // 8. PCI / PCIe
    // ═══════════════════════════════════════════════════════════════
    auto pci_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        return std::make_shared<Data::Value>(pci.generateReport());
    };

    // مسح ناقل PCI واكتشاف الأجهزة
    auto pci_scan = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        pci.enumerate();
        return std::make_shared<Data::Value>(static_cast<int>(pci.getDevices().size()));
    };

    // قراءة سجل تهيئة PCI (32-بت)
    auto pci_read = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 4) throw std::runtime_error("pci_اقرأ: يحتاج ناقل، جهاز، وظيفة، إزاحة");
        LowLevel::PCIAddress addr;
        addr.bus = static_cast<uint8_t>(args[0]->toInt());
        addr.device = static_cast<uint8_t>(args[1]->toInt());
        addr.function = static_cast<uint8_t>(args[2]->toInt());
        uint8_t offset = static_cast<uint8_t>(args[3]->toInt());
        uint32_t val = LowLevel::PCIManager::readConfig32(addr, offset);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };

    // كتابة سجل تهيئة PCI (32-بت)
    auto pci_write = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("pci_اكتب: يحتاج ناقل، جهاز، وظيفة، إزاحة، قيمة");
        LowLevel::PCIAddress addr;
        addr.bus = static_cast<uint8_t>(args[0]->toInt());
        addr.device = static_cast<uint8_t>(args[1]->toInt());
        addr.function = static_cast<uint8_t>(args[2]->toInt());
        uint8_t offset = static_cast<uint8_t>(args[3]->toInt());
        uint32_t val = static_cast<uint32_t>(args[4]->toInt());
        LowLevel::PCIManager::writeConfig32(addr, offset, val);
        return std::make_shared<Data::Value>(0);
    };

    // البحث عن بطاقات GPU في ناقل PCI
    auto pci_find_gpu = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        auto gpus = pci.findGPUs();
        std::ostringstream oss;
        oss << "عدد GPUs: " << gpus.size() << "\n";
        for (size_t i = 0; i < gpus.size(); i++) {
            oss << "  [" << i << "] Bus=" << static_cast<int>(gpus[i].address.bus)
                << " Dev=" << static_cast<int>(gpus[i].address.device)
                << " Vendor=0x" << std::hex << gpus[i].vendorId
                << " Device=0x" << gpus[i].deviceId << std::dec << "\n";
        }
        return std::make_shared<Data::Value>(oss.str());
    };

    // ═══════════════════════════════════════════════════════════════
    // 9. DMA
    // ═══════════════════════════════════════════════════════════════
    auto dma_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& dma = LowLevel::DMAManager::getInstance();
        return std::make_shared<Data::Value>(dma.generateReport());
    };

    // ═══════════════════════════════════════════════════════════════
    // 10. إطار الشاشة / Framebuffer & GPU
    // ═══════════════════════════════════════════════════════════════
    auto fb_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        return std::make_shared<Data::Value>(fb.generateReport());
    };
    fm.registerBuiltinFunction("شاشة_معلومات", fb_info);

    // رسم نقطة (بكسل) على الشاشة بإحداثيات ولون RGB
    auto fb_pixel = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 5) throw std::runtime_error("شاشة_نقطة: يحتاج x, y, r, g, b");
        int32_t x = args[0]->toInt(), y = args[1]->toInt();
        uint8_t r = static_cast<uint8_t>(args[2]->toInt());
        uint8_t g = static_cast<uint8_t>(args[3]->toInt());
        uint8_t b = static_cast<uint8_t>(args[4]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.putPixel(x, y, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_نقطة", fb_pixel);

    // رسم خط مستقيم بين نقطتين بلون RGB
    auto fb_line = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 7) throw std::runtime_error("شاشة_خط: يحتاج x1,y1,x2,y2,r,g,b");
        int32_t x1 = args[0]->toInt(), y1 = args[1]->toInt();
        int32_t x2 = args[2]->toInt(), y2 = args[3]->toInt();
        uint8_t r = static_cast<uint8_t>(args[4]->toInt());
        uint8_t g = static_cast<uint8_t>(args[5]->toInt());
        uint8_t b = static_cast<uint8_t>(args[6]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawLine(x1, y1, x2, y2, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_خط", fb_line);

    // رسم إطار مستطيل (بدون تعبئة) بلون RGB
    auto fb_rect = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 7) throw std::runtime_error("شاشة_مستطيل: يحتاج x,y,w,h,r,g,b");
        LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(), static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
        uint8_t r = static_cast<uint8_t>(args[4]->toInt());
        uint8_t g = static_cast<uint8_t>(args[5]->toInt());
        uint8_t b = static_cast<uint8_t>(args[6]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawRect(rect, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_مستطيل", fb_rect);

    // رسم مستطيل ممتلئ بلون RGB
    auto fb_fill_rect = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 7) throw std::runtime_error("شاشة_مستطيل_ممتلئ: يحتاج x,y,w,h,r,g,b");
        LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(), static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
        uint8_t r = static_cast<uint8_t>(args[4]->toInt());
        uint8_t g = static_cast<uint8_t>(args[5]->toInt());
        uint8_t b = static_cast<uint8_t>(args[6]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.fillRect(rect, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_مستطيل_ممتلئ", fb_fill_rect);

    // رسم دائرة بمركز ونصف قطر ولون RGB
    auto fb_circle = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 6) throw std::runtime_error("شاشة_دائرة: يحتاج cx,cy,radius,r,g,b");
        int32_t cx = args[0]->toInt(), cy = args[1]->toInt();
        int32_t radius = args[2]->toInt();
        uint8_t r = static_cast<uint8_t>(args[3]->toInt());
        uint8_t g = static_cast<uint8_t>(args[4]->toInt());
        uint8_t b = static_cast<uint8_t>(args[5]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawCircle(cx, cy, radius, LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_دائرة", fb_circle);

    // رسم نص على الشاشة بإحداثيات ولون RGB
    auto fb_text = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 6) throw std::runtime_error("شاشة_نص: يحتاج x,y,نص,r,g,b");
        int32_t x = args[0]->toInt(), y = args[1]->toInt();
        std::string text = args[2]->toString();
        uint8_t r = static_cast<uint8_t>(args[3]->toInt());
        uint8_t g = static_cast<uint8_t>(args[4]->toInt());
        uint8_t b = static_cast<uint8_t>(args[5]->toInt());
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.drawString(x, y, text.c_str(), LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_نص", fb_text);

    // مسح الشاشة بلون محدد (أسود افتراضياً)
    auto fb_clear = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t r = 0, g = 0, b = 0;
        if (args.size() >= 3) {
            r = static_cast<uint8_t>(args[0]->toInt());
            g = static_cast<uint8_t>(args[1]->toInt());
            b = static_cast<uint8_t>(args[2]->toInt());
        }
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.clear(LowLevel::Color(r, g, b));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_مسح", fb_clear);

    // تبديل المخزن المؤقت (double buffering)
    auto fb_swap = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.swapBuffers();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_تبديل", fb_swap);

    // رسم تدرج لوني (أفقي أو عمودي) في مستطيل
    auto fb_gradient = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 11) throw std::runtime_error("شاشة_تدرج: يحتاج x,y,w,h,r1,g1,b1,r2,g2,b2,أفقي");
        LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(), static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
        LowLevel::Color c1(static_cast<uint8_t>(args[4]->toInt()),
                           static_cast<uint8_t>(args[5]->toInt()),
                           static_cast<uint8_t>(args[6]->toInt()));
        LowLevel::Color c2(static_cast<uint8_t>(args[7]->toInt()),
                           static_cast<uint8_t>(args[8]->toInt()),
                           static_cast<uint8_t>(args[9]->toInt()));
        bool horizontal = args[10]->toInt() != 0;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        if (horizontal) {
            fb.fillGradientH(rect, c1, c2);
        } else {
            fb.fillGradientV(rect, c1, c2);
        }
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_تدرج", fb_gradient);

    // ═══════════════════════════════════════════════════════════════
    // 11. ACPI — إدارة الطاقة
    // ═══════════════════════════════════════════════════════════════
    auto acpi_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(acpi.generateReport());
    };

    // إيقاف تشغيل الجهاز عبر ACPI
    auto acpi_shutdown = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.shutdown();
        return std::make_shared<Data::Value>(0);
    };

    // إعادة تشغيل الجهاز عبر ACPI
    auto acpi_reboot = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.reboot();
        return std::make_shared<Data::Value>(0);
    };

    // 11b. ACPI — الدوال الموسّعة / Extended ACPI functions (v2)

    // ═══════════════════════════════════════════════════════════════
    // 12. التزامن / Synchronization
    // ═══════════════════════════════════════════════════════════════
    auto sync_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        std::ostringstream oss;
        oss << "وحدة التزامن جاهزة / Sync module ready\n";
        oss << "المكونات: Spinlock, RWLock, Mutex, Semaphore, Barrier, Once\n";
        return std::make_shared<Data::Value>(oss.str());
    };
    fm.registerBuiltinFunction("تزامن_معلومات", sync_info);

    // توقف مؤقت للمعالج (تعليمة PAUSE للحلقات المزدحمة)
    auto sync_pause = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::Atomic::pause();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تزامن_توقف", sync_pause);

    // حاجز ذاكرة لضمان ترتيب العمليات
    auto sync_barrier = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::Atomic::memoryBarrier();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تزامن_حاجز", sync_barrier);

    // ═══════════════════════════════════════════════════════════════
    // 13. المجدول / Scheduler
    // ═══════════════════════════════════════════════════════════════
    auto sched_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sched = LowLevel::SchedulerManager::getInstance();
        return std::make_shared<Data::Value>(sched.generateReport());
    };
    fm.registerBuiltinFunction("مجدول_معلومات", sched_info);

    // إنشاء عملية جديدة في المجدول
    auto sched_create_process = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string name = args.empty() ? "عملية" : args[0]->toString();
        auto& sched = LowLevel::SchedulerManager::getInstance();
        uint64_t pid = sched.createProcess(name.c_str());
        return std::make_shared<Data::Value>(static_cast<double>(pid));
    };
    fm.registerBuiltinFunction("مجدول_عملية_جديدة", sched_create_process);

    // تنازل العملية الحالية عن المعالج
    auto sched_yield = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sched = LowLevel::SchedulerManager::getInstance();
        sched.yield();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("مجدول_تنازل", sched_yield);

    // ═══════════════════════════════════════════════════════════════
    // 14. الإقلاع / Boot
    // ═══════════════════════════════════════════════════════════════
    auto boot_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& boot = LowLevel::BootManager::getInstance();
        return std::make_shared<Data::Value>(boot.generateReport());
    };
    fm.registerBuiltinFunction("إقلاع_معلومات", boot_info);

    // ═══════════════════════════════════════════════════════════════
    // 15. VFS — نظام الملفات الافتراضي
    // ═══════════════════════════════════════════════════════════════
    auto vfs_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& vfs = LowLevel::VFSManager::getInstance();
        return std::make_shared<Data::Value>(vfs.generateReport());
    };
    fm.registerBuiltinFunction("ملفات_معلومات", vfs_info);

    // تركيب نظام ملفات على مسار محدد
    auto vfs_mount = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("ملفات_حمّل: يحتاج جهاز، مسار، نوع");
        std::string dev = args[0]->toString();
        std::string path = args[1]->toString();
        std::string fsType = args[2]->toString();
        bool ro = args.size() > 3 ? args[3]->toInt() != 0 : false;
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.mount(dev, path, fsType, ro);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ملفات_حمّل", vfs_mount);

    // فصل نظام ملفات مركّب
    auto vfs_unmount = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ملفات_افصل: يحتاج مسار");
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.unmount(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ملفات_افصل", vfs_unmount);

    // التحقق من وجود ملف أو مجلد
    auto vfs_exists = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ملفات_موجود: يحتاج مسار");
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.exists(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ملفات_موجود", vfs_exists);

    // إنشاء مجلد جديد في نظام الملفات
    auto vfs_mkdir = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ملفات_مجلد: يحتاج مسار");
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.mkdir(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ملفات_أنشئ_مجلد", vfs_mkdir);

    // ═══════════════════════════════════════════════════════════════
    // 16. APIC — المقاطعات المتقدمة
    // ═══════════════════════════════════════════════════════════════
    auto apic_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(apic.generateReport());
    };

    // إرسال نهاية المقاطعة (End of Interrupt)
    auto apic_eoi = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendEOI();
        return std::make_shared<Data::Value>(0);
    };

    // إرسال مقاطعة بين المعالجات (IPI)
    auto apic_ipi = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("apic_ipi: يحتاج معرّف APIC ومتجه");
        uint8_t dest = static_cast<uint8_t>(args[0]->toInt());
        uint8_t vec = static_cast<uint8_t>(args[1]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendIPI(dest, vec, LowLevel::DeliveryMode::FIXED);
        return std::make_shared<Data::Value>(0);
    };

    // توجيه مقاطعة IRQ إلى معالج محدد عبر IO APIC
    auto apic_route_irq = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("apic_وجّه_irq: يحتاج irq, متجه, معالج_هدف");
        uint8_t irq = static_cast<uint8_t>(args[0]->toInt());
        uint8_t vec = static_cast<uint8_t>(args[1]->toInt());
        uint8_t dest = static_cast<uint8_t>(args[2]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.routeIRQ(irq, vec, dest);
        return std::make_shared<Data::Value>(0);
    };

    // 16b. APIC — الدوال الموسّعة / Extended APIC functions

    // ═══════════════════════════════════════════════════════════════
    // 17. HPET — المؤقت عالي الدقة
    // ═══════════════════════════════════════════════════════════════
    auto hpet_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        return std::make_shared<Data::Value>(hpet.generateReport());
    };

    // تهيئة مؤقت HPET بعنوان MMIO
    auto hpet_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_تهيئة: يحتاج عنوان MMIO");
        uint64_t base = static_cast<uint64_t>(args[0]->toDouble());
        auto& hpet = LowLevel::HPETManager::getInstance();
        bool ok = hpet.init(base);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };

    // تفعيل مؤقت HPET
    auto hpet_enable = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.enable();
        return std::make_shared<Data::Value>(0);
    };

    // قراءة عداد HPET الحالي
    auto hpet_read = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        uint64_t count = hpet.readCounter();
        return std::make_shared<Data::Value>(static_cast<double>(count));
    };

    // تأخير بالمايكروثانية عبر HPET
    auto hpet_delay_us = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_تأخير: يحتاج مايكروثانية");
        uint64_t us = static_cast<uint64_t>(args[0]->toInt());
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.delayMicroseconds(us);
        return std::make_shared<Data::Value>(0);
    };

    // تأخير بالمللي ثانية عبر HPET
    auto hpet_delay_ms = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_تأخير_مللي: يحتاج ميلي ثانية");
        uint64_t ms = static_cast<uint64_t>(args[0]->toInt());
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.delayMilliseconds(ms);
        return std::make_shared<Data::Value>(0);
    };

    // حساب الوقت المنقضي بالنانوثانية منذ عداد بداية
    auto hpet_elapsed_ns = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_منقضي: يحتاج عداد بداية");
        uint64_t start = static_cast<uint64_t>(args[0]->toDouble());
        auto& hpet = LowLevel::HPETManager::getInstance();
        uint64_t ns = hpet.elapsedNanoseconds(start);
        return std::make_shared<Data::Value>(static_cast<double>(ns));
    };

    // الحصول على تردد مؤقت HPET بالهرتز
    auto hpet_frequency = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(hpet.getFrequency()));
    };

    // ═══════════════════════════════════════════════════════════════
    // 18. استدعاءات النظام / System Calls
    // ═══════════════════════════════════════════════════════════════
    auto syscall_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        return std::make_shared<Data::Value>(sc.generateReport());
    };

    // تهيئة آلية SYSCALL/SYSRET بشرائح النواة والمستخدم
    auto syscall_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint16_t kCS = args.size() > 0 ? static_cast<uint16_t>(args[0]->toInt()) : 0x08;
        uint16_t kSS = args.size() > 1 ? static_cast<uint16_t>(args[1]->toInt()) : 0x10;
        uint16_t uCS = args.size() > 2 ? static_cast<uint16_t>(args[2]->toInt()) : 0x18;
        uint16_t uSS = args.size() > 3 ? static_cast<uint16_t>(args[3]->toInt()) : 0x20;
        auto& sc = LowLevel::SyscallManager::getInstance();
        bool ok = sc.init(kCS, kSS, uCS, uSS);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };

    // تسجيل استدعاءات النظام الافتراضية
    auto syscall_register_defaults = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        sc.registerDefaultSyscalls();
        return std::make_shared<Data::Value>(0);
    };

    // الحصول على إحصائيات استدعاءات النظام
    auto syscall_stats = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        auto stats = sc.getStats();
        std::ostringstream oss;
        oss << "إجمالي: " << stats.totalCalls << " | فاشلة: " << stats.failedCalls << " | مجهولة: " << stats.unknownCalls;
        return std::make_shared<Data::Value>(oss.str());
    };

    // ═══════════════════════════════════════════════════════════════
    // 19. دالة تقرير شامل / Comprehensive Report
    // ═══════════════════════════════════════════════════════════════
    auto lowlevel_report = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        std::ostringstream oss;
        oss << "\n" << std::string(70, '#') << "\n";
        oss << "# تقرير المكتبة منخفضة المستوى الشامل\n";
        oss << "# Low-Level Library Comprehensive Report\n";
        oss << std::string(70, '#') << "\n\n";

        oss << "الوحدات المتاحة / Available Modules:\n";
        oss << "  1.  معالج (CPU)         — CPUID, MSR, TSC, CR0-CR4\n";
        oss << "  2.  منافذ (IO)          — inb/outb/inw/outw/inl/outl\n";
        oss << "  3.  بتات (Bitwise)      — AND/OR/XOR/NOT/SHL/SHR\n";
        oss << "  4.  ذاكرة (Memory)      — malloc/free/mmap/munmap\n";
        oss << "  5.  مقاطعات (Interrupts) — IDT/ISR/IRQ/PIC\n";
        oss << "  6.  GDT/TSS            — جداول الواصفات\n";
        oss << "  7.  ترحيل (Paging)      — صفحات 4K/2M/1G\n";
        oss << "  8.  PCI/PCIe           — مسح/قراءة/كتابة/GPU\n";
        oss << "  9.  DMA                — نقل ذاكرة مباشر\n";
        oss << "  10. شاشة (Framebuffer)  — رسومات HD/GPU\n";
        oss << "  11. ACPI               — طاقة/إيقاف/إعادة تشغيل\n";
        oss << "  12. تزامن (Sync)        — أقفال/حواجز/سيمافور\n";
        oss << "  13. مجدول (Scheduler)   — عمليات/خيوط\n";
        oss << "  14. إقلاع (Boot)        — Multiboot2\n";
        oss << "  15. ملفات (VFS)         — mount/open/read/write\n";
        oss << "  16. APIC               — مقاطعات متقدمة/IPI\n";
        oss << "  17. HPET               — مؤقت عالي الدقة\n";
        oss << "  18. Syscall            — SYSCALL/SYSRET\n";
        oss << "\nإجمالي الدوال المسجلة / Total registered functions: 100+\n";
        oss << std::string(70, '#') << "\n";
        return std::make_shared<Data::Value>(oss.str());
    };
    fm.registerBuiltinFunction("منخفض_تقرير", lowlevel_report);

} // registerBuiltinsKernelCPU

} // namespace Interpreter
} // namespace Sad
