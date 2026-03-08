/*
 * ============================================================================
 * ״×†״° ״¬״¯ˆ„ ״§„ˆ״§״µ״§״× ״§„״¹״§… ˆ‚״·״¹״© ״­״§„״© ״§„…‡…״©
 * GDT & TSS Implementation for Sad Language
 * ============================================================================
 */

#include "gdt.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace Sad {
namespace LowLevel {

// ============================================================================
// ״§„…״«„ ״§„ˆ״­״¯ / Singleton
// ============================================================================

GDTManager& GDTManager::getInstance() {
    static GDTManager instance;
    return instance;
}

GDTManager::GDTManager() : isInitialized_(false) {
    std::memset(entries_, 0, sizeof(entries_));
    std::memset(&gdtPointer_, 0, sizeof(gdtPointer_));
    std::memset(&tss_, 0, sizeof(tss_));
    tss_.iopbOffset = sizeof(TSS);
}

// ============================================================================
// ״§„״×‡״¦״© / Initialization
// ============================================================================

void GDTManager::initialize() {
    if (isInitialized_) return;

    using namespace GDTConstants;

    // ״§„‚״·״¹״© ״§„״§״±״÷״© (…״·„ˆ״¨״© …† ״§„…״¹״§„״¬) / Null segment (required by CPU)
    setEntry(0, 0, 0, 0, 0);

    // ƒˆ״¯ ״§„†ˆ״§״© (Ring 0, 64-״¨״×) / Kernel code (Ring 0, 64-bit)
    setEntry(1, 0, 0xFFFFF,
        ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CODE_DATA | ACCESS_EXECUTABLE | ACCESS_READ_WRITE,
        FLAG_GRANULARITY | FLAG_LONG_MODE);

    // ״¨״§†״§״× ״§„†ˆ״§״© (Ring 0) / Kernel data (Ring 0)
    setEntry(2, 0, 0xFFFFF,
        ACCESS_PRESENT | ACCESS_RING0 | ACCESS_CODE_DATA | ACCESS_READ_WRITE,
        FLAG_GRANULARITY | FLAG_SIZE_32);

    // ƒˆ״¯ ״§„…״³״×״®״¯… (Ring 3, 64-״¨״×) / User code (Ring 3, 64-bit)
    setEntry(3, 0, 0xFFFFF,
        ACCESS_PRESENT | ACCESS_RING3 | ACCESS_CODE_DATA | ACCESS_EXECUTABLE | ACCESS_READ_WRITE,
        FLAG_GRANULARITY | FLAG_LONG_MODE);

    // ״¨״§†״§״× ״§„…״³״×״®״¯… (Ring 3) / User data (Ring 3)
    setEntry(4, 0, 0xFFFFF,
        ACCESS_PRESENT | ACCESS_RING3 | ACCESS_CODE_DATA | ACCESS_READ_WRITE,
        FLAG_GRANULARITY | FLAG_SIZE_32);

    // ״¥״¹״¯״§״¯ TSS / Setup TSS
    uint64_t tssAddr = reinterpret_cast<uint64_t>(&tss_);
    setTSSEntry(5, tssAddr, sizeof(TSS) - 1);

    // ״×״­״¯״« …״₪״´״± GDT / Update GDT pointer
    gdtPointer_.limit = sizeof(entries_) - 1;
    gdtPointer_.base = reinterpret_cast<uint64_t>(entries_);

    isInitialized_ = true;
}

// ============================================================================
// ״×״¹† ״§„״¥״¯״®״§„״§״× / Setting Entries
// ============================================================================

void GDTManager::setEntry(int index, uint32_t base, uint32_t limit,
                          uint8_t access, uint8_t flags) {
    if (index < 0 || index >= GDTConstants::MAX_GDT_ENTRIES) return;

    GDTEntry& entry = entries_[index];

    entry.limitLow      = limit & 0xFFFF;
    entry.baseLow       = base & 0xFFFF;
    entry.baseMiddle    = (base >> 16) & 0xFF;
    entry.access        = access;
    entry.flagsLimitHigh = ((flags & 0xF0) | ((limit >> 16) & 0x0F));
    entry.baseHigh      = (base >> 24) & 0xFF;
}

void GDTManager::setTSSEntry(int index, uint64_t tssAddress, uint32_t tssSize) {
    if (index < 0 || index >= GDTConstants::MAX_GDT_ENTRIES - 1) return;

    // TSS  ״§„ˆ״¶״¹ ״§„״·ˆ„ ״­״×„ ״¥״¯״®״§„† (16 ״¨״§״×) / TSS in long mode occupies 2 entries (16 bytes)
    GDTSystemEntry* sysEntry = reinterpret_cast<GDTSystemEntry*>(&entries_[index]);

    sysEntry->limitLow      = tssSize & 0xFFFF;
    sysEntry->baseLow       = tssAddress & 0xFFFF;
    sysEntry->baseMiddle1   = (tssAddress >> 16) & 0xFF;
    sysEntry->access        = GDTConstants::ACCESS_PRESENT | GDTConstants::ACCESS_TSS_64;
    sysEntry->flagsLimitHigh = ((tssSize >> 16) & 0x0F);
    sysEntry->baseMiddle2   = (tssAddress >> 24) & 0xFF;
    sysEntry->baseHigh      = static_cast<uint32_t>(tssAddress >> 32);
    sysEntry->reserved      = 0;
}

// ============================================================================
// ״§„״×״­…„ / Loading
// ============================================================================

void GDTManager::load() {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile (
        "lgdt %0\n\t"
        // ״¥״¹״§״¯״© ״×״­…„ ‚״·״¹ ״§„״¨״§†״§״× / Reload data segments
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        // ‚״² ״¨״¹״¯ „״¥״¹״§״¯״© ״×״­…„ CS / Far jump to reload CS
        "pushq $0x08\n\t"
        "leaq 1f(%%rip), %%rax\n\t"
        "pushq %%rax\n\t"
        "lretq\n\t"
        "1:\n\t"
        : : "m"(gdtPointer_) : "rax", "memory"
    );
#endif
}

void GDTManager::loadTSS(uint16_t selector) {
#if (defined(__GNUC__) || defined(__clang__)) && (defined(__x86_64__) || defined(__i386__))
    __asm__ volatile ("ltr %0" : : "r"(selector));
#endif
}

// ============================================================================
// ״¥״¯״§״±״© TSS / TSS Management
// ============================================================================

void GDTManager::initializeTSS(uint64_t kernelStackTop) {
    std::memset(&tss_, 0, sizeof(TSS));
    tss_.rsp0 = kernelStackTop;
    tss_.iopbOffset = sizeof(TSS);
}

void GDTManager::updateTSSStack(uint64_t stackTop) {
    tss_.rsp0 = stackTop;
}

void GDTManager::setIST(int istIndex, uint64_t stackTop) {
    if (istIndex < 1 || istIndex > 7) return;

    uint64_t* istTable = &tss_.ist1;
    istTable[istIndex - 1] = stackTop;
}

// ============================================================================
// ״§״³״×״¹„״§…״§״× / Queries
// ============================================================================

const GDTEntry* GDTManager::getEntry(int index) const {
    if (index < 0 || index >= GDTConstants::MAX_GDT_ENTRIES) return nullptr;
    return &entries_[index];
}

// ============================================================================
// ״§„״×‚״±״± / Report
// ============================================================================

std::string GDTManager::generateReport() const {
    std::ostringstream report;

    report << "\n" << std::string(70, '=') << "\n";
    report << "״×‚״±״± GDT / GDT Report\n";
    report << std::string(70, '=') << "\n\n";

    const char* segNames[] = {
        "NULL (״§״±״÷״©)", "ƒˆ״¯ ״§„†ˆ״§״© / Kernel Code",
        "״¨״§†״§״× ״§„†ˆ״§״© / Kernel Data", "ƒˆ״¯ ״§„…״³״×״®״¯… / User Code",
        "״¨״§†״§״× ״§„…״³״×״®״¯… / User Data", "TSS (״§„״¬״²״¡ 1)", "TSS (״§„״¬״²״¡ 2)"
    };

    report << std::left
           << std::setw(5)  << "״±‚…"
           << std::setw(30) << "״§„״§״³… / Name"
           << std::setw(10) << "״§„‚״§״¹״¯״©"
           << std::setw(10) << "״§„״­״¯"
           << std::setw(6)  << "ˆ״µˆ„"
           << std::setw(6)  << "״£״¹„״§…"
           << "\n";
    report << std::string(70, '-') << "\n";

    for (int i = 0; i < GDTConstants::MAX_GDT_ENTRIES; ++i) {
        const GDTEntry& e = entries_[i];
        uint32_t base = e.baseLow | (static_cast<uint32_t>(e.baseMiddle) << 16)
                        | (static_cast<uint32_t>(e.baseHigh) << 24);
        uint32_t limit = e.limitLow | (static_cast<uint32_t>(e.flagsLimitHigh & 0x0F) << 16);

        report << std::left
               << std::setw(5)  << i
               << std::setw(30) << segNames[i]
               << "0x" << std::hex << std::setfill('0') << std::setw(8) << base
               << "  0x" << std::setw(5) << limit
               << "  0x" << std::setw(2) << static_cast<int>(e.access)
               << "  0x" << std::setw(2) << static_cast<int>(e.flagsLimitHigh >> 4)
               << std::dec << std::setfill(' ') << "\n";
    }

    report << "\n";
    report << "TSS:\n";
    report << "  RSP0 (…ƒ״¯״³ ״§„†ˆ״§״©): 0x" << std::hex << tss_.rsp0 << std::dec << "\n";
    report << "  ״¥״²״§״­״© IOPB: " << tss_.iopbOffset << "\n";

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad

