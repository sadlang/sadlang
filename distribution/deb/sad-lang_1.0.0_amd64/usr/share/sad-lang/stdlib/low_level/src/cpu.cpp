/*
 * ============================================================================
 * ״×†״° ˆ״­״¯״© ״§„…״¹״§„״¬ ״§„…״±ƒ״² „„״÷״© ״µ
 * CPU Module Implementation for Sad Language
 * ============================================================================
 */

#include "cpu.h"
#include <sstream>
#include <iomanip>
#include <cstring>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace Sad {
namespace LowLevel {

// ============================================================================
// CPUFeatures - ״§„…†״´״¦ ״§„״§״×״±״§״¶ / Default Constructor
// ============================================================================

CPUFeatures::CPUFeatures()
    : fpu(false), vme(false), de(false), pse(false), tsc(false)
    , msr(false), pae(false), mce(false), cx8(false), apic(false)
    , sep(false), mtrr(false), pge(false), mca(false), cmov(false)
    , pat(false), pse36(false), mmx(false), fxsr(false), sse(false)
    , sse2(false), sse3(false), pclmulqdq(false), ssse3(false)
    , fma(false), cx16(false), sse41(false), sse42(false)
    , x2apic(false), popcnt(false), aes(false), xsave(false)
    , avx(false), f16c(false), rdrand(false), hypervisor(false)
    , syscall(false), nx(false), gbPages(false), rdtscp(false)
    , longMode(false), avx2(false), bmi1(false), bmi2(false)
    , avx512f(false), rdseed(false), smap(false), smep(false)
{}

// ============================================================================
// CPUManager - ״§„…״«„ ״§„ˆ״­״¯ / Singleton Instance
// ============================================================================

CPUManager& CPUManager::getInstance() {
    static CPUManager instance;
    return instance;
}

CPUManager::CPUManager() : isInitialized_(false) {
    std::memset(&info_, 0, sizeof(CPUInfo));
}

// ============================================================================
// ״§„״×‡״¦״© / Initialization
// ============================================================================

void CPUManager::initialize() {
    if (isInitialized_) return;

    detectVendor();
    detectBrand();
    detectFamily();
    detectFeatures();
    detectTopology();

    isInitialized_ = true;
}

// ============================================================================
// ״×״¹„…״© CPUID / CPUID Instruction
// ============================================================================

CPUIDResult CPUManager::cpuid(uint32_t leaf, uint32_t subleaf) {
    CPUIDResult result = {0, 0, 0, 0};

#if defined(_MSC_VER)
    int regs[4] = {0};
    __cpuidex(regs, static_cast<int>(leaf), static_cast<int>(subleaf));
    result.eax = static_cast<uint32_t>(regs[0]);
    result.ebx = static_cast<uint32_t>(regs[1]);
    result.ecx = static_cast<uint32_t>(regs[2]);
    result.edx = static_cast<uint32_t>(regs[3]);
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile (
        "cpuid"
        : "=a"(result.eax), "=b"(result.ebx),
          "=c"(result.ecx), "=d"(result.edx)
        : "a"(leaf), "c"(subleaf)
    );
#endif

    return result;
}

// ============================================================================
// ƒ״´ ״§„״´״±ƒ״© ״§„…״µ†״¹״© / Vendor Detection
// ============================================================================

void CPUManager::detectVendor() {
    CPUIDResult r = cpuid(0);
    info_.maxBasicLeaf = r.eax;

    char vendor[13] = {0};
    std::memcpy(vendor + 0, &r.ebx, 4);
    std::memcpy(vendor + 4, &r.edx, 4);
    std::memcpy(vendor + 8, &r.ecx, 4);
    info_.vendorId = std::string(vendor, 12);
}

// ============================================================================
// ƒ״´ ״§״³… ״§„…״¹״§„״¬ / Brand Detection
// ============================================================================

void CPUManager::detectBrand() {
    CPUIDResult r = cpuid(0x80000000);
    info_.maxExtendedLeaf = r.eax;

    if (info_.maxExtendedLeaf >= 0x80000004) {
        char brand[49] = {0};
        for (uint32_t i = 0; i < 3; ++i) {
            CPUIDResult br = cpuid(0x80000002 + i);
            std::memcpy(brand + (i * 16) + 0,  &br.eax, 4);
            std::memcpy(brand + (i * 16) + 4,  &br.ebx, 4);
            std::memcpy(brand + (i * 16) + 8,  &br.ecx, 4);
            std::memcpy(brand + (i * 16) + 12, &br.edx, 4);
        }
        info_.brandString = std::string(brand);
        // ״¥״²״§„״© ״§„…״³״§״§״× ״§„״²״§״¦״¯״© / Trim trailing spaces
        size_t end = info_.brandString.find_last_not_of(" \t\0");
        if (end != std::string::npos) {
            info_.brandString.resize(end + 1);
        }
    } else {
        info_.brandString = "״÷״± …״¹״±ˆ / Unknown";
    }
}

// ============================================================================
// ƒ״´ ״§„״¹״§״¦„״© ˆ״§„†…ˆ״°״¬ / Family & Model Detection
// ============================================================================

void CPUManager::detectFamily() {
    if (info_.maxBasicLeaf < 1) return;

    CPUIDResult r = cpuid(1);
    uint32_t baseFamily = (r.eax >> 8) & 0xF;
    uint32_t extFamily  = (r.eax >> 20) & 0xFF;
    uint32_t baseModel  = (r.eax >> 4) & 0xF;
    uint32_t extModel   = (r.eax >> 16) & 0xF;

    info_.stepping = r.eax & 0xF;

    if (baseFamily == 0x0F) {
        info_.family = baseFamily + extFamily;
    } else {
        info_.family = baseFamily;
    }

    if (baseFamily == 0x06 || baseFamily == 0x0F) {
        info_.model = (extModel << 4) | baseModel;
    } else {
        info_.model = baseModel;
    }

    info_.cacheLineSize = ((r.ebx >> 8) & 0xFF) * 8;
}

// ============================================================================
// ƒ״´ ״§„…״²״§״× / Feature Detection
// ============================================================================

void CPUManager::detectFeatures() {
    if (info_.maxBasicLeaf < 1) return;

    // ״§„ˆ״±‚״© 1 / Leaf 1
    CPUIDResult r1 = cpuid(1);
    CPUFeatures& f = info_.features;

    // EDX: ״§„…״²״§״× ״§„״£״³״§״³״© / Basic features
    f.fpu    = (r1.edx >> 0)  & 1;
    f.vme    = (r1.edx >> 1)  & 1;
    f.de     = (r1.edx >> 3)  & 1;
    f.pse    = (r1.edx >> 4)  & 1;
    f.tsc    = (r1.edx >> 5)  & 1;
    f.msr    = (r1.edx >> 6)  & 1;
    f.pae    = (r1.edx >> 7)  & 1;
    f.mce    = (r1.edx >> 8)  & 1;
    f.cx8    = (r1.edx >> 9)  & 1;
    f.apic   = (r1.edx >> 10) & 1;
    f.sep    = (r1.edx >> 12) & 1;
    f.mtrr   = (r1.edx >> 13) & 1;
    f.pge    = (r1.edx >> 14) & 1;
    f.mca    = (r1.edx >> 15) & 1;
    f.cmov   = (r1.edx >> 16) & 1;
    f.pat    = (r1.edx >> 17) & 1;
    f.pse36  = (r1.edx >> 18) & 1;
    f.mmx    = (r1.edx >> 24) & 1;
    f.fxsr   = (r1.edx >> 25) & 1;
    f.sse    = (r1.edx >> 26) & 1;
    f.sse2   = (r1.edx >> 27) & 1;

    // ECX: …״²״§״× ״¥״¶״§״© / Additional features
    f.sse3       = (r1.ecx >> 0)  & 1;
    f.pclmulqdq  = (r1.ecx >> 1)  & 1;
    f.ssse3      = (r1.ecx >> 9)  & 1;
    f.fma        = (r1.ecx >> 12) & 1;
    f.cx16       = (r1.ecx >> 13) & 1;
    f.sse41      = (r1.ecx >> 19) & 1;
    f.sse42      = (r1.ecx >> 20) & 1;
    f.x2apic     = (r1.ecx >> 21) & 1;
    f.popcnt     = (r1.ecx >> 23) & 1;
    f.aes        = (r1.ecx >> 25) & 1;
    f.xsave      = (r1.ecx >> 26) & 1;
    f.avx        = (r1.ecx >> 28) & 1;
    f.f16c       = (r1.ecx >> 29) & 1;
    f.rdrand     = (r1.ecx >> 30) & 1;
    f.hypervisor = (r1.ecx >> 31) & 1;

    // ״§„ˆ״±‚״© ״§„…ˆ״³״¹״© 80000001H / Extended leaf 80000001H
    if (info_.maxExtendedLeaf >= 0x80000001) {
        CPUIDResult ext1 = cpuid(0x80000001);
        f.syscall  = (ext1.edx >> 11) & 1;
        f.nx       = (ext1.edx >> 20) & 1;
        f.gbPages  = (ext1.edx >> 26) & 1;
        f.rdtscp   = (ext1.edx >> 27) & 1;
        f.longMode = (ext1.edx >> 29) & 1;
    }

    // ״§„ˆ״±‚״© 7 (…״²״§״× ‡ƒ„״©) / Leaf 7 (Structured features)
    if (info_.maxBasicLeaf >= 7) {
        CPUIDResult r7 = cpuid(7, 0);
        f.bmi1    = (r7.ebx >> 3)  & 1;
        f.avx2    = (r7.ebx >> 5)  & 1;
        f.smep    = (r7.ebx >> 7)  & 1;
        f.bmi2    = (r7.ebx >> 8)  & 1;
        f.avx512f = (r7.ebx >> 16) & 1;
        f.rdseed  = (r7.ebx >> 18) & 1;
        f.smap    = (r7.ebx >> 20) & 1;
    }
}

// ============================================================================
// ƒ״´ ״§„״·ˆ״¨ˆ„ˆ״¬״§ / Topology Detection
// ============================================================================

void CPUManager::detectTopology() {
    if (info_.maxBasicLeaf < 1) return;

    CPUIDResult r1 = cpuid(1);
    info_.logicalProcessors = (r1.ebx >> 16) & 0xFF;

    // …״­״§ˆ„״© ƒ״´ ״§„†ˆ‰ ״§„…״§״¯״© ״¹״¨״± ״§„ˆ״±‚״© 4 / Try leaf 4 for physical cores
    if (info_.maxBasicLeaf >= 4) {
        CPUIDResult r4 = cpuid(4, 0);
        info_.physicalCores = ((r4.eax >> 26) & 0x3F) + 1;
    } else {
        info_.physicalCores = info_.logicalProcessors;
    }

    if (info_.logicalProcessors == 0) {
        info_.logicalProcessors = 1;
    }
    if (info_.physicalCores == 0) {
        info_.physicalCores = 1;
    }
}

// ============================================================================
// ״³״¬„״§״× ״§„״×״­ƒ… / Control Registers
// ============================================================================

uint64_t CPUManager::readCR0() {
    uint64_t value = 0;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %%cr0, %0" : "=r"(value));
#elif defined(_MSC_VER)
    value = __readcr0();
#endif
    return value;
}

void CPUManager::writeCR0(uint64_t value) {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %0, %%cr0" : : "r"(value) : "memory");
#elif defined(_MSC_VER)
    __writecr0(value);
#endif
}

uint64_t CPUManager::readCR2() {
    uint64_t value = 0;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %%cr2, %0" : "=r"(value));
#elif defined(_MSC_VER)
    value = __readcr2();
#endif
    return value;
}

uint64_t CPUManager::readCR3() {
    uint64_t value = 0;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
#elif defined(_MSC_VER)
    value = __readcr3();
#endif
    return value;
}

void CPUManager::writeCR3(uint64_t value) {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %0, %%cr3" : : "r"(value) : "memory");
#elif defined(_MSC_VER)
    __writecr3(value);
#endif
}

uint64_t CPUManager::readCR4() {
    uint64_t value = 0;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %%cr4, %0" : "=r"(value));
#elif defined(_MSC_VER)
    value = __readcr4();
#endif
    return value;
}

void CPUManager::writeCR4(uint64_t value) {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %0, %%cr4" : : "r"(value) : "memory");
#elif defined(_MSC_VER)
    __writecr4(value);
#endif
}

// ============================================================================
// ״³״¬„״§״× ״§„†…ˆ״°״¬ ״§„…״­״¯״¯ / Model Specific Registers
// ============================================================================

uint64_t CPUManager::readMSR(uint32_t msr) {
    uint64_t value = 0;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    uint32_t lo, hi;
    __asm__ volatile ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
    value = (static_cast<uint64_t>(hi) << 32) | lo;
#elif defined(_MSC_VER)
    value = __readmsr(msr);
#endif
    return value;
}

void CPUManager::writeMSR(uint32_t msr, uint64_t value) {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    uint32_t lo = static_cast<uint32_t>(value);
    uint32_t hi = static_cast<uint32_t>(value >> 32);
    __asm__ volatile ("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
#elif defined(_MSC_VER)
    __writemsr(msr, value);
#endif
}

// ============================================================================
// ״×״¹„…״§״× ״®״§״µ״© / Special Instructions
// ============================================================================

void CPUManager::halt() {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("hlt");
#elif defined(_MSC_VER)
    __halt();
#endif
}

void CPUManager::disableInterrupts() {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    // Only available in kernel mode / ring 0
    // __asm__ volatile ("cli" ::: "memory");
#elif defined(_MSC_VER)
    // _disable() requires kernel mode ג€” stubbed for user-mode build
#endif
}

void CPUManager::enableInterrupts() {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    // Only available in kernel mode / ring 0
    // __asm__ volatile ("sti" ::: "memory");
#elif defined(_MSC_VER)
    // _enable() requires kernel mode ג€” stubbed for user-mode build
#endif
}

uint64_t CPUManager::readTSC() {
    uint64_t value = 0;
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    value = (static_cast<uint64_t>(hi) << 32) | lo;
#elif defined(_MSC_VER)
    value = __rdtsc();
#endif
    return value;
}

void CPUManager::invlpg(uint64_t virtualAddress) {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("invlpg (%0)" : : "r"(virtualAddress) : "memory");
#elif defined(_MSC_VER)
    __invlpg(reinterpret_cast<void*>(virtualAddress));
#endif
}

void CPUManager::flushTLB() {
    // ״£״³״±״¹ ״·״±‚״©: ״¥״¹״§״¯״© ƒ״×״§״¨״© CR3 / Fastest way: reload CR3
    writeCR3(readCR3());
}

void CPUManager::switchStack(uint64_t newStack) {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("mov %0, %%rsp" : : "r"(newStack) : "memory");
#endif
}

// ============================================================================
// ״¥†״´״§״¡ ״§„״×‚״±״± / Report Generation
// ============================================================================

std::string CPUManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "״×‚״±״± ״§„…״¹״§„״¬ ״§„…״±ƒ״² / CPU Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "״§„״´״±ƒ״© ״§„…״µ†״¹״© / Vendor: " << info_.vendorId << "\n";
    report << "״§״³… ״§„…״¹״§„״¬ / Brand: " << info_.brandString << "\n";
    report << "״§„״¹״§״¦„״© / Family: " << info_.family << "\n";
    report << "״§„†…ˆ״°״¬ / Model: " << info_.model << "\n";
    report << "״§„״®״·ˆ״© / Stepping: " << info_.stepping << "\n";
    report << "״§„†ˆ‰ ״§„…״§״¯״© / Physical Cores: " << info_.physicalCores << "\n";
    report << "״§„…״¹״§„״¬״§״× ״§„…†״·‚״© / Logical Processors: " << info_.logicalProcessors << "\n";
    report << "״­״¬… ״®״· ״§„״°״§ƒ״±״© ״§„…״®״¨״¦״© / Cache Line: " << info_.cacheLineSize << " ״¨״§״×/bytes\n\n";

    report << "״§„…״²״§״× ״§„…״¯״¹ˆ…״© / Supported Features:\n";
    report << std::string(70, '-') << "\n";

    auto flag = [](bool v) -> const char* { return v ? "†״¹…/Yes" : "„״§/No"; };
    const CPUFeatures& f = info_.features;

    report << "  FPU: " << flag(f.fpu) << "  |  SSE: " << flag(f.sse) << "  |  SSE2: " << flag(f.sse2) << "\n";
    report << "  SSE3: " << flag(f.sse3) << "  |  SSSE3: " << flag(f.ssse3) << "  |  SSE4.1: " << flag(f.sse41) << "\n";
    report << "  SSE4.2: " << flag(f.sse42) << "  |  AVX: " << flag(f.avx) << "  |  AVX2: " << flag(f.avx2) << "\n";
    report << "  AVX-512: " << flag(f.avx512f) << "  |  FMA: " << flag(f.fma) << "  |  AES: " << flag(f.aes) << "\n";
    report << "  PAE: " << flag(f.pae) << "  |  NX: " << flag(f.nx) << "  |  ״§„ˆ״¶״¹ ״§„״·ˆ„/Long Mode: " << flag(f.longMode) << "\n";
    report << "  ״µ״­״§״× 1GB/GB Pages: " << flag(f.gbPages) << "  |  TSC: " << flag(f.tsc) << "  |  MSR: " << flag(f.msr) << "\n";
    report << "  APIC: " << flag(f.apic) << "  |  x2APIC: " << flag(f.x2apic) << "  |  SYSCALL: " << flag(f.syscall) << "\n";
    report << "  SMEP: " << flag(f.smep) << "  |  SMAP: " << flag(f.smap) << "  |  ״¢„״© ״§״×״±״§״¶״©/Hypervisor: " << flag(f.hypervisor) << "\n";

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad

