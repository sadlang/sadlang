/*
 * ============================================================================
 * تنفيذ نظام الترحيل للغة ص
 * Paging System Implementation for Sad Language
 * ============================================================================
 */

#include "paging.h"
#include "cpu.h"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <new>

namespace Sad {
namespace LowLevel {

// ============================================================================
// مخصص إطارات الصفحات / Page Frame Allocator
// ============================================================================

PageFrameAllocator& PageFrameAllocator::getInstance() {
    static PageFrameAllocator instance;
    return instance;
}

PageFrameAllocator::PageFrameAllocator()
    : bitmap_(nullptr), bitmapSize_(0)
    , totalFrames_(0), freeFrames_(0)
    , startAddress_(0), isInitialized_(false) {}

void PageFrameAllocator::initialize(uint64_t startAddress, uint64_t totalPages) {
    startAddress_ = startAddress;
    totalFrames_ = totalPages;
    freeFrames_ = totalPages;

    // حساب حجم خريطة البتات: كل uint64_t = 64 إطار
    // Bitmap size calculation: each uint64_t = 64 frames
    bitmapSize_ = (totalPages + 63) / 64;

    // خريطة البتات توضع في بداية النطاق المتاح
    // Bitmap placed at the start of available range
    bitmap_ = reinterpret_cast<uint64_t*>(startAddress);
    std::memset(bitmap_, 0, bitmapSize_ * sizeof(uint64_t));

    // حجز مساحة خريطة البتات ذاتها / Reserve bitmap space itself
    uint64_t bitmapBytes = bitmapSize_ * sizeof(uint64_t);
    uint64_t bitmapPages = (bitmapBytes + PagingConstants::PAGE_SIZE_4K - 1) / PagingConstants::PAGE_SIZE_4K;
    for (uint64_t i = 0; i < bitmapPages; ++i) {
        setBit(i);
    }
    freeFrames_ -= bitmapPages;

    isInitialized_ = true;
}

uint64_t PageFrameAllocator::allocateFrame() {
    if (!isInitialized_ || freeFrames_ == 0) return 0;

    uint64_t index = findFirstFree();
    if (index == UINT64_MAX) return 0;

    setBit(index);
    freeFrames_--;
    return startAddress_ + (index * PagingConstants::PAGE_SIZE_4K);
}

uint64_t PageFrameAllocator::allocateContiguousFrames(uint64_t count) {
    if (!isInitialized_ || freeFrames_ < count || count == 0) return 0;

    uint64_t startIndex = findContiguousFree(count);
    if (startIndex == UINT64_MAX) return 0;

    for (uint64_t i = 0; i < count; ++i) {
        setBit(startIndex + i);
    }
    freeFrames_ -= count;
    return startAddress_ + (startIndex * PagingConstants::PAGE_SIZE_4K);
}

void PageFrameAllocator::freeFrame(uint64_t physicalAddress) {
    if (!isInitialized_) return;
    if (physicalAddress < startAddress_) return;

    uint64_t index = (physicalAddress - startAddress_) / PagingConstants::PAGE_SIZE_4K;
    if (index >= totalFrames_) return;
    if (!testBit(index)) return; // ليست مخصصة أصلاً / Not allocated

    clearBit(index);
    freeFrames_++;
}

void PageFrameAllocator::freeContiguousFrames(uint64_t physicalAddress, uint64_t count) {
    for (uint64_t i = 0; i < count; ++i) {
        freeFrame(physicalAddress + i * PagingConstants::PAGE_SIZE_4K);
    }
}

void PageFrameAllocator::reserveRange(uint64_t startAddress, uint64_t sizeInBytes) {
    if (!isInitialized_) return;

    uint64_t startFrame = 0;
    if (startAddress > startAddress_) {
        startFrame = (startAddress - startAddress_) / PagingConstants::PAGE_SIZE_4K;
    }
    uint64_t frameCount = (sizeInBytes + PagingConstants::PAGE_SIZE_4K - 1) / PagingConstants::PAGE_SIZE_4K;

    for (uint64_t i = 0; i < frameCount && (startFrame + i) < totalFrames_; ++i) {
        if (!testBit(startFrame + i)) {
            setBit(startFrame + i);
            freeFrames_--;
        }
    }
}

void PageFrameAllocator::unreserveRange(uint64_t startAddress, uint64_t sizeInBytes) {
    if (!isInitialized_) return;

    uint64_t startFrame = 0;
    if (startAddress > startAddress_) {
        startFrame = (startAddress - startAddress_) / PagingConstants::PAGE_SIZE_4K;
    }
    uint64_t frameCount = (sizeInBytes + PagingConstants::PAGE_SIZE_4K - 1) / PagingConstants::PAGE_SIZE_4K;

    for (uint64_t i = 0; i < frameCount && (startFrame + i) < totalFrames_; ++i) {
        if (testBit(startFrame + i)) {
            clearBit(startFrame + i);
            freeFrames_++;
        }
    }
}

void PageFrameAllocator::setBit(uint64_t frameIndex) {
    if (frameIndex >= totalFrames_) return;
    bitmap_[frameIndex / 64] |= (1ULL << (frameIndex % 64));
}

void PageFrameAllocator::clearBit(uint64_t frameIndex) {
    if (frameIndex >= totalFrames_) return;
    bitmap_[frameIndex / 64] &= ~(1ULL << (frameIndex % 64));
}

bool PageFrameAllocator::testBit(uint64_t frameIndex) const {
    if (frameIndex >= totalFrames_) return true; // خارج النطاق = مشغول / Out of range = occupied
    return (bitmap_[frameIndex / 64] & (1ULL << (frameIndex % 64))) != 0;
}

uint64_t PageFrameAllocator::findFirstFree() const {
    for (uint64_t i = 0; i < bitmapSize_; ++i) {
        if (bitmap_[i] != UINT64_MAX) {
            // يوجد بت حر في هذا العنصر / There's a free bit in this element
            for (int bit = 0; bit < 64; ++bit) {
                uint64_t frameIndex = i * 64 + bit;
                if (frameIndex >= totalFrames_) return UINT64_MAX;
                if (!(bitmap_[i] & (1ULL << bit))) {
                    return frameIndex;
                }
            }
        }
    }
    return UINT64_MAX;
}

uint64_t PageFrameAllocator::findContiguousFree(uint64_t count) const {
    uint64_t consecutive = 0;
    uint64_t startIndex = 0;

    for (uint64_t i = 0; i < totalFrames_; ++i) {
        if (!testBit(i)) {
            if (consecutive == 0) startIndex = i;
            consecutive++;
            if (consecutive >= count) return startIndex;
        } else {
            consecutive = 0;
        }
    }
    return UINT64_MAX;
}

// ============================================================================
// مدير الترحيل / Paging Manager
// ============================================================================

PagingManager& PagingManager::getInstance() {
    static PagingManager instance;
    return instance;
}

PagingManager::PagingManager()
    : currentPML4_(0)
    , nextMmioVirtual_(0xFFFFFF0000000000ULL) // منطقة MMIO الافتراضية / Default MMIO region
    , mappedPageCount_(0)
    , isInitialized_(false) {}

void PagingManager::initialize() {
    if (isInitialized_) return;

    // تخصيص جدول PML4 جديد / Allocate new PML4 table
    uint64_t pml4Frame = PageFrameAllocator::getInstance().allocateFrame();
    if (pml4Frame == 0) return;

    PageTable* pml4 = reinterpret_cast<PageTable*>(pml4Frame);
    pml4->clear();
    currentPML4_ = pml4Frame;

    isInitialized_ = true;
}

TranslationResult PagingManager::translate(uint64_t virtualAddress) const {
    TranslationResult result;
    if (!isInitialized_ || currentPML4_ == 0) return result;

    PageTable* pml4 = reinterpret_cast<PageTable*>(currentPML4_);

    // المستوى 4: PML4 / Level 4: PML4
    int idx4 = pml4Index(virtualAddress);
    if (!pml4->entries[idx4].isPresent()) return result;

    // المستوى 3: PDPT / Level 3: PDPT
    PageTable* pdpt = reinterpret_cast<PageTable*>(pml4->entries[idx4].getPhysicalAddress());
    int idx3 = pdptIndex(virtualAddress);
    if (!pdpt->entries[idx3].isPresent()) return result;

    // فحص صفحة 1GB / Check 1GB page
    if (pdpt->entries[idx3].isHugePage()) {
        result.valid = true;
        result.pageBase = pdpt->entries[idx3].getPhysicalAddress();
        result.physicalAddress = result.pageBase + (virtualAddress & (PagingConstants::PAGE_SIZE_1G - 1));
        result.pageSize = PageSize::SIZE_1G;
        result.flags = pdpt->entries[idx3].value & 0xFFF;
        result.level = 3;
        return result;
    }

    // المستوى 2: PD / Level 2: Page Directory
    PageTable* pd = reinterpret_cast<PageTable*>(pdpt->entries[idx3].getPhysicalAddress());
    int idx2 = pdIndex(virtualAddress);
    if (!pd->entries[idx2].isPresent()) return result;

    // فحص صفحة 2MB / Check 2MB page
    if (pd->entries[idx2].isHugePage()) {
        result.valid = true;
        result.pageBase = pd->entries[idx2].getPhysicalAddress();
        result.physicalAddress = result.pageBase + (virtualAddress & (PagingConstants::PAGE_SIZE_2M - 1));
        result.pageSize = PageSize::SIZE_2M;
        result.flags = pd->entries[idx2].value & 0xFFF;
        result.level = 2;
        return result;
    }

    // المستوى 1: PT / Level 1: Page Table
    PageTable* pt = reinterpret_cast<PageTable*>(pd->entries[idx2].getPhysicalAddress());
    int idx1 = ptIndex(virtualAddress);
    if (!pt->entries[idx1].isPresent()) return result;

    result.valid = true;
    result.pageBase = pt->entries[idx1].getPhysicalAddress();
    result.physicalAddress = result.pageBase + (virtualAddress & (PagingConstants::PAGE_SIZE_4K - 1));
    result.pageSize = PageSize::SIZE_4K;
    result.flags = pt->entries[idx1].value & 0xFFF;
    result.level = 1;
    return result;
}

PageTable* PagingManager::getOrCreateTable(PageTable* parent, int index, uint64_t flags) {
    if (parent->entries[index].isPresent()) {
        return reinterpret_cast<PageTable*>(parent->entries[index].getPhysicalAddress());
    }

    // تخصيص جدول جديد / Allocate new table
    uint64_t frame = PageFrameAllocator::getInstance().allocateFrame();
    if (frame == 0) return nullptr;

    PageTable* table = reinterpret_cast<PageTable*>(frame);
    table->clear();

    // تعيين الإدخال في الجدول الأب / Set entry in parent table
    parent->entries[index].value = frame | flags | PageFlags::PRESENT;

    return table;
}

bool PagingManager::mapPage(uint64_t virtualAddress, uint64_t physicalAddress,
                            uint64_t flags, PageSize pageSize) {
    if (!isInitialized_ || currentPML4_ == 0) return false;

    PageTable* pml4 = reinterpret_cast<PageTable*>(currentPML4_);
    uint64_t tableFlags = PageFlags::PRESENT | PageFlags::WRITABLE;

    // إضافة علم المستخدم إذا كان مطلوباً في الأعلام / Add user flag if requested
    if (flags & PageFlags::USER) {
        tableFlags |= PageFlags::USER;
    }

    // المستوى 4 → 3 / Level 4 → 3
    PageTable* pdpt = getOrCreateTable(pml4, pml4Index(virtualAddress), tableFlags);
    if (!pdpt) return false;

    if (pageSize == PageSize::SIZE_1G) {
        // صفحة 1GB مباشرة في PDPT / 1GB page directly in PDPT
        pdpt->entries[pdptIndex(virtualAddress)].value =
            (physicalAddress & PagingConstants::PHYS_ADDR_MASK) | flags | PageFlags::HUGE_PAGE;
        mappedPageCount_++;
        return true;
    }

    // المستوى 3 → 2 / Level 3 → 2
    PageTable* pd = getOrCreateTable(pdpt, pdptIndex(virtualAddress), tableFlags);
    if (!pd) return false;

    if (pageSize == PageSize::SIZE_2M) {
        // صفحة 2MB مباشرة في PD / 2MB page directly in PD
        pd->entries[pdIndex(virtualAddress)].value =
            (physicalAddress & PagingConstants::PHYS_ADDR_MASK) | flags | PageFlags::HUGE_PAGE;
        mappedPageCount_++;
        return true;
    }

    // المستوى 2 → 1 / Level 2 → 1
    PageTable* pt = getOrCreateTable(pd, pdIndex(virtualAddress), tableFlags);
    if (!pt) return false;

    // صفحة 4KB / 4KB page
    pt->entries[ptIndex(virtualAddress)].value =
        (physicalAddress & PagingConstants::PHYS_ADDR_MASK) | flags;
    mappedPageCount_++;
    return true;
}

bool PagingManager::unmapPage(uint64_t virtualAddress) {
    if (!isInitialized_ || currentPML4_ == 0) return false;

    PageTable* pml4 = reinterpret_cast<PageTable*>(currentPML4_);

    int idx4 = pml4Index(virtualAddress);
    if (!pml4->entries[idx4].isPresent()) return false;

    PageTable* pdpt = reinterpret_cast<PageTable*>(pml4->entries[idx4].getPhysicalAddress());
    int idx3 = pdptIndex(virtualAddress);
    if (!pdpt->entries[idx3].isPresent()) return false;

    if (pdpt->entries[idx3].isHugePage()) {
        pdpt->entries[idx3].clear();
        invalidatePage(virtualAddress);
        mappedPageCount_--;
        return true;
    }

    PageTable* pd = reinterpret_cast<PageTable*>(pdpt->entries[idx3].getPhysicalAddress());
    int idx2 = pdIndex(virtualAddress);
    if (!pd->entries[idx2].isPresent()) return false;

    if (pd->entries[idx2].isHugePage()) {
        pd->entries[idx2].clear();
        invalidatePage(virtualAddress);
        mappedPageCount_--;
        return true;
    }

    PageTable* pt = reinterpret_cast<PageTable*>(pd->entries[idx2].getPhysicalAddress());
    int idx1 = ptIndex(virtualAddress);
    if (!pt->entries[idx1].isPresent()) return false;

    pt->entries[idx1].clear();
    invalidatePage(virtualAddress);
    mappedPageCount_--;
    return true;
}

bool PagingManager::mapRange(uint64_t virtualStart, uint64_t physicalStart,
                             uint64_t size, uint64_t flags, PageSize pageSize) {
    uint64_t step = PagingConstants::PAGE_SIZE_4K;
    if (pageSize == PageSize::SIZE_2M) step = PagingConstants::PAGE_SIZE_2M;
    if (pageSize == PageSize::SIZE_1G) step = PagingConstants::PAGE_SIZE_1G;

    for (uint64_t offset = 0; offset < size; offset += step) {
        if (!mapPage(virtualStart + offset, physicalStart + offset, flags, pageSize)) {
            return false;
        }
    }
    return true;
}

bool PagingManager::unmapRange(uint64_t virtualStart, uint64_t size) {
    for (uint64_t offset = 0; offset < size; offset += PagingConstants::PAGE_SIZE_4K) {
        unmapPage(virtualStart + offset);
    }
    return true;
}

uint64_t PagingManager::mapMMIO(uint64_t physicalAddress, uint64_t size) {
    uint64_t alignedSize = alignUp(size, PagingConstants::PAGE_SIZE_4K);
    uint64_t virtualAddress = nextMmioVirtual_;
    nextMmioVirtual_ += alignedSize;

    if (!mapRange(virtualAddress, physicalAddress, alignedSize, PageFlags::MMIO_PAGE)) {
        return 0;
    }

    return virtualAddress;
}

void PagingManager::unmapMMIO(uint64_t virtualAddress, uint64_t size) {
    uint64_t alignedSize = alignUp(size, PagingConstants::PAGE_SIZE_4K);
    unmapRange(virtualAddress, alignedSize);
}

bool PagingManager::changeFlags(uint64_t virtualAddress, uint64_t flags) {
    if (!isInitialized_ || currentPML4_ == 0) return false;

    PageTable* pml4 = reinterpret_cast<PageTable*>(currentPML4_);
    int idx4 = pml4Index(virtualAddress);
    if (!pml4->entries[idx4].isPresent()) return false;

    PageTable* pdpt = reinterpret_cast<PageTable*>(pml4->entries[idx4].getPhysicalAddress());
    int idx3 = pdptIndex(virtualAddress);
    if (!pdpt->entries[idx3].isPresent()) return false;

    if (pdpt->entries[idx3].isHugePage()) {
        uint64_t phys = pdpt->entries[idx3].getPhysicalAddress();
        pdpt->entries[idx3].value = phys | flags | PageFlags::HUGE_PAGE;
        invalidatePage(virtualAddress);
        return true;
    }

    PageTable* pd = reinterpret_cast<PageTable*>(pdpt->entries[idx3].getPhysicalAddress());
    int idx2 = pdIndex(virtualAddress);
    if (!pd->entries[idx2].isPresent()) return false;

    if (pd->entries[idx2].isHugePage()) {
        uint64_t phys = pd->entries[idx2].getPhysicalAddress();
        pd->entries[idx2].value = phys | flags | PageFlags::HUGE_PAGE;
        invalidatePage(virtualAddress);
        return true;
    }

    PageTable* pt = reinterpret_cast<PageTable*>(pd->entries[idx2].getPhysicalAddress());
    int idx1 = ptIndex(virtualAddress);
    if (!pt->entries[idx1].isPresent()) return false;

    uint64_t phys = pt->entries[idx1].getPhysicalAddress();
    pt->entries[idx1].value = phys | flags;
    invalidatePage(virtualAddress);
    return true;
}

uint64_t PagingManager::createAddressSpace() {
    uint64_t frame = PageFrameAllocator::getInstance().allocateFrame();
    if (frame == 0) return 0;

    PageTable* newPML4 = reinterpret_cast<PageTable*>(frame);
    newPML4->clear();

    // نسخ نصف النواة من PML4 الحالي (إدخالات 256-511)
    // Copy kernel half from current PML4 (entries 256-511)
    if (currentPML4_ != 0) {
        PageTable* currentPml4 = reinterpret_cast<PageTable*>(currentPML4_);
        for (int i = 256; i < PagingConstants::ENTRIES_PER_TABLE; ++i) {
            newPML4->entries[i] = currentPml4->entries[i];
        }
    }

    return frame;
}

void PagingManager::switchAddressSpace(uint64_t pml4PhysAddr) {
    currentPML4_ = pml4PhysAddr;

#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile ("mov %0, %%cr3" : : "r"(pml4PhysAddr) : "memory");
#elif defined(_MSC_VER)
    __writecr3(pml4PhysAddr);
#endif
}

void PagingManager::destroyAddressSpace(uint64_t pml4PhysAddr) {
    if (pml4PhysAddr == currentPML4_) return; // لا يمكن تدمير الفضاء النشط / Can't destroy active space

    PageTable* pml4 = reinterpret_cast<PageTable*>(pml4PhysAddr);

    // تحرير جداول المستخدم فقط (الإدخالات 0-255) / Free only user tables (entries 0-255)
    freeTable(pml4, 4);

    PageFrameAllocator::getInstance().freeFrame(pml4PhysAddr);
}

void PagingManager::freeTable(PageTable* table, int level) {
    if (level <= 1) return;

    // لجداول المستخدم فقط (صفحات أقل من النواة)
    int maxIndex = (level == 4) ? 256 : PagingConstants::ENTRIES_PER_TABLE;

    for (int i = 0; i < maxIndex; ++i) {
        if (table->entries[i].isPresent() && !table->entries[i].isHugePage()) {
            PageTable* child = reinterpret_cast<PageTable*>(table->entries[i].getPhysicalAddress());
            freeTable(child, level - 1);
            PageFrameAllocator::getInstance().freeFrame(table->entries[i].getPhysicalAddress());
        }
    }
}

void PagingManager::invalidatePage(uint64_t virtualAddress) {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile ("invlpg (%0)" : : "r"(virtualAddress) : "memory");
#elif defined(_MSC_VER)
    __invlpg(reinterpret_cast<void*>(virtualAddress));
#endif
}

void PagingManager::flushTLB() {
    if (currentPML4_ != 0) {
        switchAddressSpace(currentPML4_);
    }
}

std::string PagingManager::generateReport() const {
    std::ostringstream report;
    auto& alloc = PageFrameAllocator::getInstance();

    report << "\n" << std::string(70, '=') << "\n";
    report << "تقرير نظام الترحيل / Paging System Report\n";
    report << std::string(70, '=') << "\n\n";

    report << "PML4 الحالي / Current PML4: 0x" << std::hex << currentPML4_ << std::dec << "\n";
    report << "الصفحات المعينة / Mapped Pages: " << mappedPageCount_ << "\n\n";

    report << "مخصص الإطارات / Frame Allocator:\n";
    report << "  إجمالي الإطارات / Total Frames: " << alloc.getTotalFrameCount() << "\n";
    report << "  إطارات حرة / Free Frames: " << alloc.getFreeFrameCount() << "\n";
    report << "  إطارات مستخدمة / Used Frames: " << alloc.getUsedFrameCount() << "\n";

    if (alloc.getTotalFrameCount() > 0) {
        double usedPercent = 100.0 * alloc.getUsedFrameCount() / alloc.getTotalFrameCount();
        report << "  نسبة الاستخدام / Usage: " << std::fixed << std::setprecision(1) << usedPercent << "%\n";
    }

    report << std::string(70, '=') << "\n\n";
    return report.str();
}

} // namespace LowLevel
} // namespace Sad
