/**
 * @file builtin_registry_part19.cpp
 * @brief (AR) الجزء 19: مكتبة البرمجة منخفضة المستوى — دعم كتابة أنظمة تشغيل كاملة
 * @brief (EN) Part 19: Low-Level OS Development Library
 *
 * يغطي هذا الملف الوحدات التالية:
 * 1. المعالج (CPU) — CPUID, MSR, سجلات التحكم
 * 2. منافذ الإدخال/الإخراج (IO Ports) — inb/outb/inw/outw/inl/outl
 * 3. العمليات البتية (Bitwise) — AND, OR, XOR, NOT, تدوير
 * 4. إدارة الذاكرة (Memory) — malloc/free/mmap/munmap
 * 5. المقاطعات (Interrupts) — IDT, ISR, IRQ
 * 6. GDT/TSS — جداول الواصفات
 * 7. الترحيل (Paging) — صفحات الذاكرة الافتراضية
 * 8. PCI/PCIe — مسح/تهيئة أجهزة PCI
 * 9. DMA — نقل الذاكرة المباشر
 * 10. إطار الشاشة (Framebuffer/GPU) — رسومات HD
 * 11. ACPI — إدارة الطاقة
 * 12. التزامن (Sync) — أقفال/حواجز
 * 13. المجدول (Scheduler) — عمليات/خيوط
 * 14. الإقلاع (Boot) — تحليل Multiboot2
 * 15. نظام الملفات الافتراضي (VFS) — تحميل/فتح/قراءة/كتابة
 * 16. APIC — مقاطعات متقدمة/IPI/مؤقت
 * 17. HPET — مؤقت عالي الدقة
 * 18. استدعاءات النظام (Syscall) — SYSCALL/SYSRET
 *
 * @author فريق تطوير لغة ص
 * @date 2026-02-27
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

#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart19(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ===================================================================
    // 1. وحدة المعالج / CPU Module
    // ===================================================================

    auto cpu_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& cpu = LowLevel::CPUManager::getInstance();
        return std::make_shared<Data::Value>(cpu.generateReport());
    };
    fm.registerBuiltinFunction("معالج_معلومات", cpu_info);
    fm.registerBuiltinFunction("cpu_info", cpu_info);

    auto cpu_halt = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::CPUManager::halt();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("معالج_توقف", cpu_halt);
    fm.registerBuiltinFunction("cpu_halt", cpu_halt);
    fm.registerBuiltinFunction("hlt", cpu_halt);

    auto cpu_rdtsc = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        uint64_t tsc = LowLevel::CPUManager::readTSC();
        return std::make_shared<Data::Value>(static_cast<double>(tsc));
    };
    fm.registerBuiltinFunction("معالج_عداد", cpu_rdtsc);
    fm.registerBuiltinFunction("cpu_rdtsc", cpu_rdtsc);
    fm.registerBuiltinFunction("rdtsc", cpu_rdtsc);

    auto cpu_cli = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::CPUManager::disableInterrupts();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تعطيل_مقاطعات", cpu_cli);
    fm.registerBuiltinFunction("cpu_cli", cpu_cli);
    fm.registerBuiltinFunction("cli", cpu_cli);

    auto cpu_sti = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::CPUManager::enableInterrupts();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تفعيل_مقاطعات", cpu_sti);
    fm.registerBuiltinFunction("cpu_sti", cpu_sti);
    fm.registerBuiltinFunction("sti", cpu_sti);

    auto cpu_read_msr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("معالج_اقرأ_msr: يحتاج رقم MSR");
        uint32_t msr = static_cast<uint32_t>(args[0]->toInt());
        uint64_t val = LowLevel::CPUManager::readMSR(msr);
        return std::make_shared<Data::Value>(static_cast<double>(val));
    };
    fm.registerBuiltinFunction("معالج_اقرأ_msr", cpu_read_msr);
    fm.registerBuiltinFunction("cpu_read_msr", cpu_read_msr);
    fm.registerBuiltinFunction("rdmsr", cpu_read_msr);

    auto cpu_write_msr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("معالج_اكتب_msr: يحتاج رقم MSR وقيمة");
        uint32_t msr = static_cast<uint32_t>(args[0]->toInt());
        uint64_t val = static_cast<uint64_t>(args[1]->toDouble());
        LowLevel::CPUManager::writeMSR(msr, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("معالج_اكتب_msr", cpu_write_msr);
    fm.registerBuiltinFunction("cpu_write_msr", cpu_write_msr);
    fm.registerBuiltinFunction("wrmsr", cpu_write_msr);

    auto cpu_invlpg = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("cpu_invlpg: يحتاج عنوان");
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        LowLevel::CPUManager::invlpg(addr);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("ابطل_صفحة", cpu_invlpg);
    fm.registerBuiltinFunction("cpu_invlpg", cpu_invlpg);
    fm.registerBuiltinFunction("invlpg", cpu_invlpg);

    // ===================================================================
    // 2. منافذ الإدخال/الإخراج / IO Ports
    // ===================================================================

    auto io_inb = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ: يحتاج رقم المنفذ");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint8_t val = LowLevel::IOPorts::inb(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("منفذ_اقرأ", io_inb);
    fm.registerBuiltinFunction("port_read", io_inb);
    fm.registerBuiltinFunction("inb", io_inb);

    auto io_outb = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب: يحتاج منفذ وقيمة");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint8_t val = static_cast<uint8_t>(args[1]->toInt());
        LowLevel::IOPorts::outb(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("منفذ_اكتب", io_outb);
    fm.registerBuiltinFunction("port_write", io_outb);
    fm.registerBuiltinFunction("outb", io_outb);

    auto io_inw = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ16: يحتاج رقم المنفذ");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint16_t val = LowLevel::IOPorts::inw(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("منفذ_اقرأ16", io_inw);
    fm.registerBuiltinFunction("port_read16", io_inw);
    fm.registerBuiltinFunction("inw", io_inw);

    auto io_outw = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب16: يحتاج منفذ وقيمة");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint16_t val = static_cast<uint16_t>(args[1]->toInt());
        LowLevel::IOPorts::outw(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("منفذ_اكتب16", io_outw);
    fm.registerBuiltinFunction("port_write16", io_outw);
    fm.registerBuiltinFunction("outw", io_outw);

    auto io_inl = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("منفذ_اقرأ32: يحتاج رقم المنفذ");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint32_t val = LowLevel::IOPorts::inl(port);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("منفذ_اقرأ32", io_inl);
    fm.registerBuiltinFunction("port_read32", io_inl);
    fm.registerBuiltinFunction("inl", io_inl);

    auto io_outl = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("منفذ_اكتب32: يحتاج منفذ وقيمة");
        uint16_t port = static_cast<uint16_t>(args[0]->toInt());
        uint32_t val = static_cast<uint32_t>(args[1]->toInt());
        LowLevel::IOPorts::outl(port, val);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("منفذ_اكتب32", io_outl);
    fm.registerBuiltinFunction("port_write32", io_outl);
    fm.registerBuiltinFunction("outl", io_outl);

    // ===================================================================
    // 3. العمليات البتية / Bitwise Operations
    // ===================================================================

    auto bit_and = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_و: يحتاج قيمتين");
        return std::make_shared<Data::Value>(args[0]->toInt() & args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_و", bit_and);
    fm.registerBuiltinFunction("bit_and", bit_and);

    auto bit_or = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_أو: يحتاج قيمتين");
        return std::make_shared<Data::Value>(args[0]->toInt() | args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_أو", bit_or);
    fm.registerBuiltinFunction("bit_or", bit_or);

    auto bit_xor = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_حصري: يحتاج قيمتين");
        return std::make_shared<Data::Value>(args[0]->toInt() ^ args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_حصري", bit_xor);
    fm.registerBuiltinFunction("bit_xor", bit_xor);

    auto bit_not = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("بت_نفي: يحتاج قيمة");
        return std::make_shared<Data::Value>(~args[0]->toInt());
    };
    fm.registerBuiltinFunction("بت_نفي", bit_not);
    fm.registerBuiltinFunction("bit_not", bit_not);

    auto bit_shl = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_يسار: يحتاج قيمة وعدد");
        return std::make_shared<Data::Value>(args[0]->toInt() << args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_يسار", bit_shl);
    fm.registerBuiltinFunction("bit_shl", bit_shl);

    auto bit_shr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_يمين: يحتاج قيمة وعدد");
        return std::make_shared<Data::Value>(args[0]->toInt() >> args[1]->toInt());
    };
    fm.registerBuiltinFunction("بت_يمين", bit_shr);
    fm.registerBuiltinFunction("bit_shr", bit_shr);

    auto bit_test = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_اختبار: يحتاج قيمة ورقم بت");
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>((val >> bit) & 1);
    };
    fm.registerBuiltinFunction("بت_اختبار", bit_test);
    fm.registerBuiltinFunction("bit_test", bit_test);

    auto bit_set = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_عيّن: يحتاج قيمة ورقم بت");
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>(val | (1 << bit));
    };
    fm.registerBuiltinFunction("بت_عيّن", bit_set);
    fm.registerBuiltinFunction("bit_set", bit_set);

    auto bit_clear = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("بت_امسح: يحتاج قيمة ورقم بت");
        int val = args[0]->toInt();
        int bit = args[1]->toInt();
        return std::make_shared<Data::Value>(val & ~(1 << bit));
    };
    fm.registerBuiltinFunction("بت_امسح", bit_clear);
    fm.registerBuiltinFunction("bit_clear", bit_clear);

    // ===================================================================
    // 4. إدارة الذاكرة / Memory Management
    // ===================================================================

    auto mem_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& mm = LowLevel::MemoryManager::getInstance();
        return std::make_shared<Data::Value>(mm.getMemoryReport());
    };
    fm.registerBuiltinFunction("ذاكرة_معلومات", mem_info);
    fm.registerBuiltinFunction("mem_info", mem_info);

    auto mem_peek = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ذاكرة_اقرأ: يحتاج عنوان");
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        uint8_t val = *reinterpret_cast<volatile uint8_t*>(addr);
        return std::make_shared<Data::Value>(static_cast<int>(val));
    };
    fm.registerBuiltinFunction("ذاكرة_اقرأ", mem_peek);
    fm.registerBuiltinFunction("mem_peek", mem_peek);
    fm.registerBuiltinFunction("peek", mem_peek);

    auto mem_poke = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("ذاكرة_اكتب: يحتاج عنوان وقيمة");
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        uint8_t val = static_cast<uint8_t>(args[1]->toInt());
        *reinterpret_cast<volatile uint8_t*>(addr) = val;
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("ذاكرة_اكتب", mem_poke);
    fm.registerBuiltinFunction("mem_poke", mem_poke);
    fm.registerBuiltinFunction("poke", mem_poke);

    // ===================================================================
    // 5. المقاطعات / Interrupts
    // ===================================================================

    auto int_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& im = LowLevel::InterruptManager::getInstance();
        return std::make_shared<Data::Value>(im.getInterruptReport());
    };
    fm.registerBuiltinFunction("مقاطعات_معلومات", int_info);
    fm.registerBuiltinFunction("int_info", int_info);

    auto int_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& im = LowLevel::InterruptManager::getInstance();
        im.initialize();
        im.loadIDT();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("مقاطعات_تهيئة", int_init);
    fm.registerBuiltinFunction("int_init", int_init);

    // ===================================================================
    // 6. GDT / TSS
    // ===================================================================

    auto gdt_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& gdt = LowLevel::GDTManager::getInstance();
        return std::make_shared<Data::Value>(gdt.generateReport());
    };
    fm.registerBuiltinFunction("gdt_معلومات", gdt_info);
    fm.registerBuiltinFunction("gdt_info", gdt_info);

    auto gdt_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& gdt = LowLevel::GDTManager::getInstance();
        gdt.initialize();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("gdt_تهيئة", gdt_init);
    fm.registerBuiltinFunction("gdt_init", gdt_init);

    // ===================================================================
    // 7. الترحيل (Paging)
    // ===================================================================

    auto paging_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& pg = LowLevel::PagingManager::getInstance();
        return std::make_shared<Data::Value>(pg.generateReport());
    };
    fm.registerBuiltinFunction("ترحيل_معلومات", paging_info);
    fm.registerBuiltinFunction("paging_info", paging_info);

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
    fm.registerBuiltinFunction("paging_map", paging_map);

    auto paging_unmap = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ترحيل_إلغاء: يحتاج عنوان افتراضي");
        uint64_t virt = static_cast<uint64_t>(args[0]->toDouble());
        auto& pg = LowLevel::PagingManager::getInstance();
        pg.unmapPage(virt);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("ترحيل_إلغاء", paging_unmap);
    fm.registerBuiltinFunction("paging_unmap", paging_unmap);

    // ===================================================================
    // 8. PCI / PCIe
    // ===================================================================

    auto pci_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        return std::make_shared<Data::Value>(pci.generateReport());
    };
    fm.registerBuiltinFunction("pci_معلومات", pci_info);
    fm.registerBuiltinFunction("pci_info", pci_info);

    auto pci_scan = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& pci = LowLevel::PCIManager::getInstance();
        pci.enumerate();
        return std::make_shared<Data::Value>(static_cast<int>(pci.getDevices().size()));
    };
    fm.registerBuiltinFunction("pci_مسح", pci_scan);
    fm.registerBuiltinFunction("pci_scan", pci_scan);

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
    fm.registerBuiltinFunction("pci_اقرأ", pci_read);
    fm.registerBuiltinFunction("pci_read", pci_read);

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
    fm.registerBuiltinFunction("pci_اكتب", pci_write);
    fm.registerBuiltinFunction("pci_write", pci_write);

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
    fm.registerBuiltinFunction("pci_ابحث_gpu", pci_find_gpu);
    fm.registerBuiltinFunction("pci_find_gpu", pci_find_gpu);

    // ===================================================================
    // 9. DMA
    // ===================================================================

    auto dma_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& dma = LowLevel::DMAManager::getInstance();
        return std::make_shared<Data::Value>(dma.generateReport());
    };
    fm.registerBuiltinFunction("dma_معلومات", dma_info);
    fm.registerBuiltinFunction("dma_info", dma_info);

    // ===================================================================
    // 10. إطار الشاشة / Framebuffer & GPU
    // ===================================================================

    auto fb_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        return std::make_shared<Data::Value>(fb.generateReport());
    };
    fm.registerBuiltinFunction("شاشة_معلومات", fb_info);
    fm.registerBuiltinFunction("fb_info", fb_info);

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
    fm.registerBuiltinFunction("fb_pixel", fb_pixel);
    fm.registerBuiltinFunction("putpixel", fb_pixel);

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
    fm.registerBuiltinFunction("fb_line", fb_line);

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
    fm.registerBuiltinFunction("fb_rect", fb_rect);

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
    fm.registerBuiltinFunction("fb_fill_rect", fb_fill_rect);

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
    fm.registerBuiltinFunction("fb_circle", fb_circle);

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
    fm.registerBuiltinFunction("fb_text", fb_text);

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
    fm.registerBuiltinFunction("fb_clear", fb_clear);

    auto fb_swap = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.swapBuffers();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("شاشة_تبديل", fb_swap);
    fm.registerBuiltinFunction("fb_swap", fb_swap);

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
    fm.registerBuiltinFunction("fb_gradient", fb_gradient);

    // ===================================================================
    // 11. ACPI — إدارة الطاقة
    // ===================================================================

    auto acpi_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(acpi.generateReport());
    };
    fm.registerBuiltinFunction("acpi_معلومات", acpi_info);
    fm.registerBuiltinFunction("acpi_info", acpi_info);

    auto acpi_shutdown = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.shutdown();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_إيقاف", acpi_shutdown);
    fm.registerBuiltinFunction("acpi_shutdown", acpi_shutdown);

    auto acpi_reboot = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.reboot();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_إعادة_تشغيل", acpi_reboot);
    fm.registerBuiltinFunction("acpi_reboot", acpi_reboot);

    // 11b. ACPI — الدوال الموسّعة / Extended ACPI functions (v2)

    auto acpi_init_fn = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.initialize();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تهيئة", acpi_init_fn);
    fm.registerBuiltinFunction("acpi_init", acpi_init_fn);

    auto acpi_init_rsdp = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(-1);
        uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.initializeFromRSDP(addr);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تهيئة_من_rsdp", acpi_init_rsdp);
    fm.registerBuiltinFunction("acpi_init_from_rsdp", acpi_init_rsdp);

    auto acpi_find_table = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) return std::make_shared<Data::Value>(0);
        auto& acpi = LowLevel::ACPIManager::getInstance();
        auto ptr = acpi.findTable(args[0]->toString().c_str());
        return std::make_shared<Data::Value>(static_cast<double>(reinterpret_cast<uintptr_t>(ptr)));
    };
    fm.registerBuiltinFunction("acpi_بحث_جدول", acpi_find_table);
    fm.registerBuiltinFunction("acpi_find_table", acpi_find_table);

    auto acpi_enable = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        bool ok = acpi.enableACPI();
        return std::make_shared<Data::Value>(ok ? 0 : -1);
    };
    fm.registerBuiltinFunction("acpi_تفعيل", acpi_enable);
    fm.registerBuiltinFunction("acpi_enable", acpi_enable);

    auto acpi_disable = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.disableACPI();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تعطيل", acpi_disable);
    fm.registerBuiltinFunction("acpi_disable", acpi_disable);

    auto acpi_sleep = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        int state = args.empty() ? 3 : args[0]->toInt();
        auto& acpi = LowLevel::ACPIManager::getInstance();
        bool ok = acpi.enterSleepState(static_cast<LowLevel::SleepState>(state));
        return std::make_shared<Data::Value>(ok ? 0 : -1);
    };
    fm.registerBuiltinFunction("acpi_نوم", acpi_sleep);
    fm.registerBuiltinFunction("acpi_sleep", acpi_sleep);

    auto acpi_pm_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(acpi.readPMTimer()));
    };
    fm.registerBuiltinFunction("acpi_قراءة_مؤقت", acpi_pm_timer);
    fm.registerBuiltinFunction("acpi_read_pm_timer", acpi_pm_timer);

    auto acpi_timer_32 = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(acpi.isPMTimer32Bit() ? 1 : 0);
    };
    fm.registerBuiltinFunction("acpi_مؤقت_32بت", acpi_timer_32);
    fm.registerBuiltinFunction("acpi_is_pm_timer_32bit", acpi_timer_32);

    auto acpi_delay = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint32_t us = args.empty() ? 1000 : static_cast<uint32_t>(args[0]->toInt());
        auto& acpi = LowLevel::ACPIManager::getInstance();
        acpi.delayMicroseconds(us);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("acpi_تأخير", acpi_delay);
    fm.registerBuiltinFunction("acpi_delay_us", acpi_delay);

    auto acpi_proc_count = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(acpi.getProcessorCount()));
    };
    fm.registerBuiltinFunction("acpi_عدد_معالجات", acpi_proc_count);
    fm.registerBuiltinFunction("acpi_processor_count", acpi_proc_count);

    auto acpi_version = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(acpi.getACPIVersion()));
    };
    fm.registerBuiltinFunction("acpi_إصدار", acpi_version);
    fm.registerBuiltinFunction("acpi_version", acpi_version);

    auto acpi_is_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(acpi.isInitialized() ? 1 : 0);
    };
    fm.registerBuiltinFunction("acpi_هل_مهيأ", acpi_is_init);
    fm.registerBuiltinFunction("acpi_is_initialized", acpi_is_init);

    auto acpi_apic_addr = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(acpi.getLocalAPICAddress()));
    };
    fm.registerBuiltinFunction("acpi_عنوان_apic", acpi_apic_addr);
    fm.registerBuiltinFunction("acpi_local_apic_address", acpi_apic_addr);

    auto acpi_ecam = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint16_t seg = args.size() > 0 ? static_cast<uint16_t>(args[0]->toInt()) : 0;
        uint8_t bus = args.size() > 1 ? static_cast<uint8_t>(args[1]->toInt()) : 0;
        auto& acpi = LowLevel::ACPIManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(acpi.getECAMBase(seg, bus)));
    };
    fm.registerBuiltinFunction("acpi_ecam_قاعدة", acpi_ecam);
    fm.registerBuiltinFunction("acpi_ecam_base", acpi_ecam);

    // ===================================================================
    // 12. التزامن / Synchronization
    // ===================================================================

    auto sync_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        std::ostringstream oss;
        oss << "وحدة التزامن جاهزة / Sync module ready\n";
        oss << "المكونات: Spinlock, RWLock, Mutex, Semaphore, Barrier, Once\n";
        return std::make_shared<Data::Value>(oss.str());
    };
    fm.registerBuiltinFunction("تزامن_معلومات", sync_info);
    fm.registerBuiltinFunction("sync_info", sync_info);

    auto sync_pause = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::Atomic::pause();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تزامن_توقف", sync_pause);
    fm.registerBuiltinFunction("sync_pause", sync_pause);

    auto sync_barrier = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        LowLevel::Atomic::memoryBarrier();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("تزامن_حاجز", sync_barrier);
    fm.registerBuiltinFunction("sync_memory_barrier", sync_barrier);

    // ===================================================================
    // 13. المجدول / Scheduler
    // ===================================================================

    auto sched_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sched = LowLevel::SchedulerManager::getInstance();
        return std::make_shared<Data::Value>(sched.generateReport());
    };
    fm.registerBuiltinFunction("مجدول_معلومات", sched_info);
    fm.registerBuiltinFunction("sched_info", sched_info);

    auto sched_create_process = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        std::string name = args.empty() ? "عملية" : args[0]->toString();
        auto& sched = LowLevel::SchedulerManager::getInstance();
        uint64_t pid = sched.createProcess(name.c_str());
        return std::make_shared<Data::Value>(static_cast<double>(pid));
    };
    fm.registerBuiltinFunction("مجدول_عملية_جديدة", sched_create_process);
    fm.registerBuiltinFunction("sched_create_process", sched_create_process);

    auto sched_yield = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sched = LowLevel::SchedulerManager::getInstance();
        sched.yield();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("مجدول_تنازل", sched_yield);
    fm.registerBuiltinFunction("sched_yield", sched_yield);

    // ===================================================================
    // 14. الإقلاع / Boot
    // ===================================================================

    auto boot_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& boot = LowLevel::BootManager::getInstance();
        return std::make_shared<Data::Value>(boot.generateReport());
    };
    fm.registerBuiltinFunction("إقلاع_معلومات", boot_info);
    fm.registerBuiltinFunction("boot_info", boot_info);

    // ===================================================================
    // 15. VFS — نظام الملفات الافتراضي
    // ===================================================================

    auto vfs_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& vfs = LowLevel::VFSManager::getInstance();
        return std::make_shared<Data::Value>(vfs.generateReport());
    };
    fm.registerBuiltinFunction("ملفات_معلومات", vfs_info);
    fm.registerBuiltinFunction("vfs_info", vfs_info);

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
    fm.registerBuiltinFunction("vfs_mount", vfs_mount);

    auto vfs_unmount = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ملفات_افصل: يحتاج مسار");
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.unmount(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ملفات_افصل", vfs_unmount);
    fm.registerBuiltinFunction("vfs_unmount", vfs_unmount);

    auto vfs_exists = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ملفات_موجود: يحتاج مسار");
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.exists(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ملفات_موجود", vfs_exists);
    fm.registerBuiltinFunction("vfs_exists", vfs_exists);

    auto vfs_mkdir = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("ملفات_مجلد: يحتاج مسار");
        auto& vfs = LowLevel::VFSManager::getInstance();
        bool ok = vfs.mkdir(args[0]->toString());
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("ملفات_أنشئ_مجلد", vfs_mkdir);
    fm.registerBuiltinFunction("vfs_mkdir", vfs_mkdir);

    // ===================================================================
    // 16. APIC — المقاطعات المتقدمة
    // ===================================================================

    auto apic_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(apic.generateReport());
    };
    fm.registerBuiltinFunction("apic_معلومات", apic_info);
    fm.registerBuiltinFunction("apic_info", apic_info);

    auto apic_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint64_t base = args.empty() ? 0xFEE00000ULL : static_cast<uint64_t>(args[0]->toDouble());
        auto& apic = LowLevel::APICManager::getInstance();
        bool ok = apic.initLocalAPIC(base);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("apic_تهيئة", apic_init);
    fm.registerBuiltinFunction("apic_init", apic_init);

    auto apic_eoi = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendEOI();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_نهاية_مقاطعة", apic_eoi);
    fm.registerBuiltinFunction("apic_eoi", apic_eoi);

    auto apic_ipi = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) throw std::runtime_error("apic_ipi: يحتاج معرّف APIC ومتجه");
        uint8_t dest = static_cast<uint8_t>(args[0]->toInt());
        uint8_t vec = static_cast<uint8_t>(args[1]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendIPI(dest, vec, LowLevel::DeliveryMode::FIXED);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أرسل_مقاطعة", apic_ipi);
    fm.registerBuiltinFunction("apic_send_ipi", apic_ipi);

    auto apic_disable_pic = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.disablePIC();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_عطّل_pic", apic_disable_pic);
    fm.registerBuiltinFunction("apic_disable_pic", apic_disable_pic);

    auto apic_route_irq = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) throw std::runtime_error("apic_وجّه_irq: يحتاج irq, متجه, معالج_هدف");
        uint8_t irq = static_cast<uint8_t>(args[0]->toInt());
        uint8_t vec = static_cast<uint8_t>(args[1]->toInt());
        uint8_t dest = static_cast<uint8_t>(args[2]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.routeIRQ(irq, vec, dest);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_وجّه_irq", apic_route_irq);
    fm.registerBuiltinFunction("apic_route_irq", apic_route_irq);

    auto apic_timer_calibrate = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint32_t hz = args.empty() ? 1000 : static_cast<uint32_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        uint32_t count = apic.calibrateTimer(hz);
        return std::make_shared<Data::Value>(static_cast<int>(count));
    };
    fm.registerBuiltinFunction("apic_معايرة_مؤقت", apic_timer_calibrate);
    fm.registerBuiltinFunction("apic_calibrate_timer", apic_timer_calibrate);

    // 16b. APIC — الدوال الموسّعة / Extended APIC functions

    auto apic_supported = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(apic.isAPICSupported() ? 1 : 0);
    };
    fm.registerBuiltinFunction("apic_مدعوم", apic_supported);
    fm.registerBuiltinFunction("apic_supported", apic_supported);

    auto apic_x2_supported = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(apic.isX2APICSupported() ? 1 : 0);
    };
    fm.registerBuiltinFunction("apic_x2_مدعوم", apic_x2_supported);
    fm.registerBuiltinFunction("apic_x2_supported", apic_x2_supported);

    auto apic_id = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(apic.getAPICId()));
    };
    fm.registerBuiltinFunction("apic_معرّف", apic_id);
    fm.registerBuiltinFunction("apic_id", apic_id);

    auto apic_set_priority = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t prio = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.setTaskPriority(prio);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أولوية_مهمة", apic_set_priority);
    fm.registerBuiltinFunction("apic_set_priority", apic_set_priority);

    auto apic_init_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t vec = args.size() > 0 ? static_cast<uint8_t>(args[0]->toInt()) : 32;
        int mode = args.size() > 1 ? args[1]->toInt() : 0;
        int div = args.size() > 2 ? args[2]->toInt() : 3;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.initTimer(vec, static_cast<LowLevel::TimerMode>(mode), static_cast<LowLevel::TimerDivide>(div));
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_تهيئة_مؤقت", apic_init_timer);
    fm.registerBuiltinFunction("apic_init_timer", apic_init_timer);

    auto apic_start_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint32_t count = args.empty() ? 1000000 : static_cast<uint32_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.startTimer(count);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_بدء_مؤقت", apic_start_timer);
    fm.registerBuiltinFunction("apic_start_timer", apic_start_timer);

    auto apic_stop_timer = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.stopTimer();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_إيقاف_مؤقت", apic_stop_timer);
    fm.registerBuiltinFunction("apic_stop_timer", apic_stop_timer);

    auto apic_timer_count = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(apic.getTimerCount()));
    };
    fm.registerBuiltinFunction("apic_عداد_مؤقت", apic_timer_count);
    fm.registerBuiltinFunction("apic_timer_count", apic_timer_count);

    auto apic_send_ipi_all = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t vec = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        bool self = args.size() > 1 ? (args[1]->toInt() != 0) : false;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendIPIToAll(vec, self);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أرسل_للكل", apic_send_ipi_all);
    fm.registerBuiltinFunction("apic_send_ipi_all", apic_send_ipi_all);

    auto apic_send_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t dest = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendINIT(dest);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أرسل_init", apic_send_init);
    fm.registerBuiltinFunction("apic_send_init", apic_send_init);

    auto apic_send_sipi = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 2) return std::make_shared<Data::Value>(-1);
        uint8_t dest = static_cast<uint8_t>(args[0]->toInt());
        uint8_t page = static_cast<uint8_t>(args[1]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.sendSIPI(dest, page);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_أرسل_sipi", apic_send_sipi);
    fm.registerBuiltinFunction("apic_send_sipi", apic_send_sipi);

    auto apic_wait_delivery = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        apic.waitForDelivery();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_انتظر_تسليم", apic_wait_delivery);
    fm.registerBuiltinFunction("apic_wait_delivery", apic_wait_delivery);

    auto apic_mask_irq = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t irq = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.maskIRQ(irq);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_قناع_irq", apic_mask_irq);
    fm.registerBuiltinFunction("apic_mask_irq", apic_mask_irq);

    auto apic_unmask_irq = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint8_t irq = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        apic.unmaskIRQ(irq);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("apic_إلغاء_قناع_irq", apic_unmask_irq);
    fm.registerBuiltinFunction("apic_unmask_irq", apic_unmask_irq);

    auto apic_io_count = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& apic = LowLevel::APICManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<int>(apic.getIOAPICCount()));
    };
    fm.registerBuiltinFunction("apic_عدد_io", apic_io_count);
    fm.registerBuiltinFunction("apic_io_count", apic_io_count);

    auto apic_init_io = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.size() < 3) return std::make_shared<Data::Value>(-1);
        uint8_t id = static_cast<uint8_t>(args[0]->toInt());
        uint64_t base = static_cast<uint64_t>(args[1]->toDouble());
        uint32_t gsib = static_cast<uint32_t>(args[2]->toInt());
        auto& apic = LowLevel::APICManager::getInstance();
        bool ok = apic.initIOAPIC(id, base, gsib);
        return std::make_shared<Data::Value>(ok ? 0 : -1);
    };
    fm.registerBuiltinFunction("apic_تهيئة_io", apic_init_io);
    fm.registerBuiltinFunction("apic_init_io", apic_init_io);

    // ===================================================================
    // 17. HPET — المؤقت عالي الدقة
    // ===================================================================

    auto hpet_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        return std::make_shared<Data::Value>(hpet.generateReport());
    };
    fm.registerBuiltinFunction("hpet_معلومات", hpet_info);
    fm.registerBuiltinFunction("hpet_info", hpet_info);

    auto hpet_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_تهيئة: يحتاج عنوان MMIO");
        uint64_t base = static_cast<uint64_t>(args[0]->toDouble());
        auto& hpet = LowLevel::HPETManager::getInstance();
        bool ok = hpet.init(base);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("hpet_تهيئة", hpet_init);
    fm.registerBuiltinFunction("hpet_init", hpet_init);

    auto hpet_enable = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.enable();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("hpet_فعّل", hpet_enable);
    fm.registerBuiltinFunction("hpet_enable", hpet_enable);

    auto hpet_read = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        uint64_t count = hpet.readCounter();
        return std::make_shared<Data::Value>(static_cast<double>(count));
    };
    fm.registerBuiltinFunction("hpet_اقرأ", hpet_read);
    fm.registerBuiltinFunction("hpet_read", hpet_read);

    auto hpet_delay_us = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_تأخير: يحتاج مايكروثانية");
        uint64_t us = static_cast<uint64_t>(args[0]->toInt());
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.delayMicroseconds(us);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("hpet_تأخير", hpet_delay_us);
    fm.registerBuiltinFunction("hpet_delay_us", hpet_delay_us);

    auto hpet_delay_ms = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_تأخير_مللي: يحتاج ميلي ثانية");
        uint64_t ms = static_cast<uint64_t>(args[0]->toInt());
        auto& hpet = LowLevel::HPETManager::getInstance();
        hpet.delayMilliseconds(ms);
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("hpet_تأخير_مللي", hpet_delay_ms);
    fm.registerBuiltinFunction("hpet_delay_ms", hpet_delay_ms);

    auto hpet_elapsed_ns = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("hpet_منقضي: يحتاج عداد بداية");
        uint64_t start = static_cast<uint64_t>(args[0]->toDouble());
        auto& hpet = LowLevel::HPETManager::getInstance();
        uint64_t ns = hpet.elapsedNanoseconds(start);
        return std::make_shared<Data::Value>(static_cast<double>(ns));
    };
    fm.registerBuiltinFunction("hpet_منقضي", hpet_elapsed_ns);
    fm.registerBuiltinFunction("hpet_elapsed_ns", hpet_elapsed_ns);

    auto hpet_frequency = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& hpet = LowLevel::HPETManager::getInstance();
        return std::make_shared<Data::Value>(static_cast<double>(hpet.getFrequency()));
    };
    fm.registerBuiltinFunction("hpet_تردد", hpet_frequency);
    fm.registerBuiltinFunction("hpet_frequency", hpet_frequency);

    // ===================================================================
    // 18. استدعاءات النظام / System Calls
    // ===================================================================

    auto syscall_info = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        return std::make_shared<Data::Value>(sc.generateReport());
    };
    fm.registerBuiltinFunction("syscall_معلومات", syscall_info);
    fm.registerBuiltinFunction("syscall_info", syscall_info);

    auto syscall_init = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        uint16_t kCS = args.size() > 0 ? static_cast<uint16_t>(args[0]->toInt()) : 0x08;
        uint16_t kSS = args.size() > 1 ? static_cast<uint16_t>(args[1]->toInt()) : 0x10;
        uint16_t uCS = args.size() > 2 ? static_cast<uint16_t>(args[2]->toInt()) : 0x18;
        uint16_t uSS = args.size() > 3 ? static_cast<uint16_t>(args[3]->toInt()) : 0x20;
        auto& sc = LowLevel::SyscallManager::getInstance();
        bool ok = sc.init(kCS, kSS, uCS, uSS);
        return std::make_shared<Data::Value>(ok ? 1 : 0);
    };
    fm.registerBuiltinFunction("syscall_تهيئة", syscall_init);
    fm.registerBuiltinFunction("syscall_init", syscall_init);

    auto syscall_register_defaults = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        sc.registerDefaultSyscalls();
        return std::make_shared<Data::Value>(0);
    };
    fm.registerBuiltinFunction("syscall_سجّل_افتراضي", syscall_register_defaults);
    fm.registerBuiltinFunction("syscall_register_defaults", syscall_register_defaults);

    auto syscall_stats = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
        (void)args;
        auto& sc = LowLevel::SyscallManager::getInstance();
        auto stats = sc.getStats();
        std::ostringstream oss;
        oss << "إجمالي: " << stats.totalCalls << " | فاشلة: " << stats.failedCalls << " | مجهولة: " << stats.unknownCalls;
        return std::make_shared<Data::Value>(oss.str());
    };
    fm.registerBuiltinFunction("syscall_إحصائيات", syscall_stats);
    fm.registerBuiltinFunction("syscall_stats", syscall_stats);

    // ===================================================================
    // 19. دالة تقرير شامل / Comprehensive Report
    // ===================================================================

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
    fm.registerBuiltinFunction("lowlevel_report", lowlevel_report);

} // registerBuiltinsPart19

} // namespace Interpreter
} // namespace Sad
