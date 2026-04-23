/*
 * (AR) مولد تعليمات LLVM — مستوى منخفض: CPU / GDT / Paging / IDT / PCI / DMA / FB / ACPI / Sync / Sched / Boot / VFS / APIC / HPET / Syscall / Mem / UEFI
 * (EN) LLVM instruction emitter — Low-level: CPU / GDT / Paging / IDT / PCI / DMA / FB / ACPI / Sync / Sched / Boot / VFS / APIC / HPET / Syscall / Mem / UEFI
 * مستخرج من: llvm_codegen_instructions.cpp (سطور 741-1099)
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        /**
         * (AR) تنفيذ تعليمات المستوى المنخفض لنظام التشغيل (CPU, GDT, Paging, IDT, PCI, DMA, FB, ACPI, Sync, Sched, Boot, VFS, APIC, HPET, Syscall, Mem, UEFI)
         * (EN) Emit low-level OS instructions (CPU, GDT, Paging, IDT, PCI, DMA, FB, ACPI, Sync, Sched, Boot, VFS, APIC, HPET, Syscall, Mem, UEFI)
         * @return nullptr إذا لم يتعرف على الـ opcode / nullptr if opcode not handled
         */
        llvm::Value *LLVMCodeGen::emitInstructionLowlevel(std::shared_ptr<SIRInstruction> inst)
        {
            switch (inst->opcode)
            {

            // --- 15a: CPU ---
            case SIROpcode::LOWLEVEL_CPU_GET_INFO:
                return emitLowlevelCpuGetInfo(inst);
            case SIROpcode::LOWLEVEL_CPU_GET_FEATURES:
                return emitLowlevelCpuGetFeatures(inst);
            case SIROpcode::LOWLEVEL_CPU_READ_MSR:
                return emitLowlevelCpuReadMSR(inst);
            case SIROpcode::LOWLEVEL_CPU_WRITE_MSR:
                return emitLowlevelCpuWriteMSR(inst);
            case SIROpcode::LOWLEVEL_CPU_READ_CR:
                return emitLowlevelCpuReadCR(inst);
            case SIROpcode::LOWLEVEL_CPU_WRITE_CR:
                return emitLowlevelCpuWriteCR(inst);
            case SIROpcode::LOWLEVEL_CPU_INVLPG:
                return emitLowlevelCpuInvlpg(inst);
            case SIROpcode::LOWLEVEL_CPU_GET_REPORT:
                return emitLowlevelCpuGetReport(inst);

            // --- 15b: GDT ---
            case SIROpcode::LOWLEVEL_GDT_INIT:
                return emitLowlevelGdtInit(inst);
            case SIROpcode::LOWLEVEL_GDT_LOAD:
                return emitLowlevelGdtLoad(inst);
            case SIROpcode::LOWLEVEL_GDT_GET_REPORT:
                return emitLowlevelGdtGetReport(inst);

            // --- 15c: Paging ---
            case SIROpcode::LOWLEVEL_PAGING_INIT:
                return emitLowlevelPagingInit(inst);
            case SIROpcode::LOWLEVEL_PAGING_MAP:
                return emitLowlevelPagingMap(inst);
            case SIROpcode::LOWLEVEL_PAGING_UNMAP:
                return emitLowlevelPagingUnmap(inst);
            case SIROpcode::LOWLEVEL_PAGING_FLUSH_TLB:
                return emitLowlevelPagingFlushTlb(inst);
            case SIROpcode::LOWLEVEL_PAGING_GET_REPORT:
                return emitLowlevelPagingGetReport(inst);

            // --- 15d: IDT ---
            case SIROpcode::LOWLEVEL_IDT_INIT:
                return emitLowlevelIdtInit(inst);
            case SIROpcode::LOWLEVEL_IDT_LOAD:
                return emitLowlevelIdtLoad(inst);
            case SIROpcode::LOWLEVEL_IDT_REGISTER_ISR:
                return emitLowlevelIdtRegisterIsr(inst);
            case SIROpcode::LOWLEVEL_IDT_ENABLE_IRQ:
                return emitLowlevelIdtEnableIrq(inst);
            case SIROpcode::LOWLEVEL_IDT_GET_REPORT:
                return emitLowlevelIdtGetReport(inst);

            // --- 15e: PCI ---
            case SIROpcode::LOWLEVEL_PCI_ENUMERATE:
                return emitLowlevelPciEnumerate(inst);
            case SIROpcode::LOWLEVEL_PCI_READ_CONFIG:
                return emitLowlevelPciReadConfig(inst);
            case SIROpcode::LOWLEVEL_PCI_WRITE_CONFIG:
                return emitLowlevelPciWriteConfig(inst);
            case SIROpcode::LOWLEVEL_PCI_GET_DEVICE_COUNT:
                return emitLowlevelPciGetDeviceCount(inst);
            case SIROpcode::LOWLEVEL_PCI_GET_REPORT:
                return emitLowlevelPciGetReport(inst);

            // --- 15f: DMA ---
            case SIROpcode::LOWLEVEL_DMA_INIT:
                return emitLowlevelDmaInit(inst);
            case SIROpcode::LOWLEVEL_DMA_TRANSFER:
                return emitLowlevelDmaTransfer(inst);
            case SIROpcode::LOWLEVEL_DMA_STATUS:
                return emitLowlevelDmaStatus(inst);
            case SIROpcode::LOWLEVEL_DMA_GET_REPORT:
                return emitLowlevelDmaGetReport(inst);

            // --- 15g: Framebuffer ---
            case SIROpcode::LOWLEVEL_FB_INIT:
                return emitLowlevelFbInit(inst);
            case SIROpcode::LOWLEVEL_FB_SET_PIXEL:
                return emitLowlevelFbSetPixel(inst);
            case SIROpcode::LOWLEVEL_FB_DRAW_RECT:
                return emitLowlevelFbDrawRect(inst);
            case SIROpcode::LOWLEVEL_FB_FILL_RECT:
                return emitLowlevelFbFillRect(inst);
            case SIROpcode::LOWLEVEL_FB_DRAW_LINE:
                return emitLowlevelFbDrawLine(inst);
            case SIROpcode::LOWLEVEL_FB_DRAW_STRING:
                return emitLowlevelFbDrawString(inst);
            case SIROpcode::LOWLEVEL_FB_CLEAR:
                return emitLowlevelFbClear(inst);
            case SIROpcode::LOWLEVEL_FB_GET_REPORT:
                return emitLowlevelFbGetReport(inst);

            // --- 15h: ACPI ---
            case SIROpcode::LOWLEVEL_ACPI_INIT:
                return emitLowlevelAcpiInit(inst);
            case SIROpcode::LOWLEVEL_ACPI_FIND_TABLE:
                return emitLowlevelAcpiFindTable(inst);
            case SIROpcode::LOWLEVEL_ACPI_SHUTDOWN:
                return emitLowlevelAcpiShutdown(inst);
            case SIROpcode::LOWLEVEL_ACPI_GET_REPORT:
                return emitLowlevelAcpiGetReport(inst);

            // --- 15i: Sync ---
            case SIROpcode::LOWLEVEL_SPINLOCK_INIT:
                return emitLowlevelSpinlockInit(inst);
            case SIROpcode::LOWLEVEL_SPINLOCK_LOCK:
                return emitLowlevelSpinlockLock(inst);
            case SIROpcode::LOWLEVEL_SPINLOCK_UNLOCK:
                return emitLowlevelSpinlockUnlock(inst);
            case SIROpcode::LOWLEVEL_MUTEX_INIT:
                return emitLowlevelMutexInit(inst);
            case SIROpcode::LOWLEVEL_MUTEX_LOCK:
                return emitLowlevelMutexLock(inst);
            case SIROpcode::LOWLEVEL_MUTEX_UNLOCK:
                return emitLowlevelMutexUnlock(inst);
            case SIROpcode::LOWLEVEL_SEMAPHORE_INIT:
                return emitLowlevelSemaphoreInit(inst);
            case SIROpcode::LOWLEVEL_BARRIER_INIT:
                return emitLowlevelBarrierInit(inst);

            // --- 15j: Scheduler ---
            case SIROpcode::LOWLEVEL_SCHED_INIT:
                return emitLowlevelSchedInit(inst);
            case SIROpcode::LOWLEVEL_SCHED_CREATE_PROC:
                return emitLowlevelSchedCreateProc(inst);
            case SIROpcode::LOWLEVEL_SCHED_CREATE_THREAD:
                return emitLowlevelSchedCreateThread(inst);
            case SIROpcode::LOWLEVEL_SCHED_YIELD:
                return emitLowlevelSchedYield(inst);
            case SIROpcode::LOWLEVEL_SCHED_SLEEP:
                return emitLowlevelSchedSleep(inst);
            case SIROpcode::LOWLEVEL_SCHED_GET_REPORT:
                return emitLowlevelSchedGetReport(inst);

            // --- 15k: Boot ---
            case SIROpcode::LOWLEVEL_BOOT_GET_INFO:
                return emitLowlevelBootGetInfo(inst);
            case SIROpcode::LOWLEVEL_BOOT_GET_MEMORY_MAP:
                return emitLowlevelBootGetMemoryMap(inst);
            case SIROpcode::LOWLEVEL_BOOT_GET_REPORT:
                return emitLowlevelBootGetReport(inst);

            // --- 15l: VFS ---
            case SIROpcode::LOWLEVEL_VFS_MOUNT:
                return emitLowlevelVfsMount(inst);
            case SIROpcode::LOWLEVEL_VFS_UNMOUNT:
                return emitLowlevelVfsUnmount(inst);
            case SIROpcode::LOWLEVEL_VFS_OPEN:
                return emitLowlevelVfsOpen(inst);
            case SIROpcode::LOWLEVEL_VFS_READ:
                return emitLowlevelVfsRead(inst);
            case SIROpcode::LOWLEVEL_VFS_WRITE:
                return emitLowlevelVfsWrite(inst);
            case SIROpcode::LOWLEVEL_VFS_CLOSE:
                return emitLowlevelVfsClose(inst);
            case SIROpcode::LOWLEVEL_VFS_GET_REPORT:
                return emitLowlevelVfsGetReport(inst);

            // --- 15m: APIC ---
            case SIROpcode::LOWLEVEL_APIC_INIT:
                return emitLowlevelApicInit(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_EOI:
                return emitLowlevelApicSendEoi(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_IPI:
                return emitLowlevelApicSendIpi(inst);
            case SIROpcode::LOWLEVEL_APIC_SET_TIMER:
                return emitLowlevelApicSetTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_GET_REPORT:
                return emitLowlevelApicGetReport(inst);

            // --- 15n: HPET ---
            case SIROpcode::LOWLEVEL_HPET_INIT:
                return emitLowlevelHpetInit(inst);
            case SIROpcode::LOWLEVEL_HPET_READ:
                return emitLowlevelHpetRead(inst);
            case SIROpcode::LOWLEVEL_HPET_SLEEP:
                return emitLowlevelHpetSleep(inst);
            case SIROpcode::LOWLEVEL_HPET_GET_REPORT:
                return emitLowlevelHpetGetReport(inst);

            // --- 15o: Syscall ---
            case SIROpcode::LOWLEVEL_SYSCALL_INIT:
                return emitLowlevelSyscallInit(inst);
            case SIROpcode::LOWLEVEL_SYSCALL_REGISTER:
                return emitLowlevelSyscallRegister(inst);
            case SIROpcode::LOWLEVEL_SYSCALL_INVOKE:
                return emitLowlevelSyscallInvoke(inst);
            case SIROpcode::LOWLEVEL_SYSCALL_GET_REPORT:
                return emitLowlevelSyscallGetReport(inst);

            // --- 15p: Memory ---
            case SIROpcode::LOWLEVEL_MEM_ALLOC_PHYS:
                return emitLowlevelMemAllocPhys(inst);
            case SIROpcode::LOWLEVEL_MEM_FREE_PHYS:
                return emitLowlevelMemFreePhys(inst);
            case SIROpcode::LOWLEVEL_MEM_MAP_REGION:
                return emitLowlevelMemMapRegion(inst);
            case SIROpcode::LOWLEVEL_MEM_GET_REPORT:
                return emitLowlevelMemGetReport(inst);

            // =================================================================
            // القسم 16: بروتوكول UEFI / UEFI Boot Protocol
            // =================================================================

            // 16a. التهيئة والتحكم
            case SIROpcode::LOWLEVEL_UEFI_INIT:
                return emitLowlevelUefiInit(inst);
            case SIROpcode::LOWLEVEL_UEFI_EXIT_BOOT_SERVICES:
                return emitLowlevelUefiExitBootServices(inst);
            case SIROpcode::LOWLEVEL_UEFI_IS_INITIALIZED:
                return emitLowlevelUefiIsInitialized(inst);
            case SIROpcode::LOWLEVEL_UEFI_BS_EXITED:
                return emitLowlevelUefiBsExited(inst);
            case SIROpcode::LOWLEVEL_UEFI_RESET_SYSTEM:
                return emitLowlevelUefiResetSystem(inst);

            // 16b. إدارة الذاكرة
            case SIROpcode::LOWLEVEL_UEFI_ALLOC_PAGES:
                return emitLowlevelUefiAllocPages(inst);
            case SIROpcode::LOWLEVEL_UEFI_FREE_PAGES:
                return emitLowlevelUefiFreePages(inst);
            case SIROpcode::LOWLEVEL_UEFI_ALLOC_POOL:
                return emitLowlevelUefiAllocPool(inst);
            case SIROpcode::LOWLEVEL_UEFI_FREE_POOL:
                return emitLowlevelUefiFreePool(inst);
            case SIROpcode::LOWLEVEL_UEFI_GET_MEMORY_MAP:
                return emitLowlevelUefiGetMemoryMap(inst);
            case SIROpcode::LOWLEVEL_UEFI_GET_MEMMAP_KEY:
                return emitLowlevelUefiGetMemmapKey(inst);
            case SIROpcode::LOWLEVEL_UEFI_TOTAL_MEMORY:
                return emitLowlevelUefiTotalMemory(inst);

            // 16c. بروتوكول الرسوميات GOP
            case SIROpcode::LOWLEVEL_UEFI_INIT_GOP:
                return emitLowlevelUefiInitGop(inst);
            case SIROpcode::LOWLEVEL_UEFI_SET_GOP_MODE:
                return emitLowlevelUefiSetGopMode(inst);
            case SIROpcode::LOWLEVEL_UEFI_QUERY_GOP_MODE:
                return emitLowlevelUefiQueryGopMode(inst);
            case SIROpcode::LOWLEVEL_UEFI_GOP_MODE_COUNT:
                return emitLowlevelUefiGopModeCount(inst);
            case SIROpcode::LOWLEVEL_UEFI_CURRENT_GOP_MODE:
                return emitLowlevelUefiCurrentGopMode(inst);
            case SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_BASE:
                return emitLowlevelUefiFramebufferBase(inst);
            case SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_SIZE:
                return emitLowlevelUefiFramebufferSize(inst);
            case SIROpcode::LOWLEVEL_UEFI_FILL_SCREEN:
                return emitLowlevelUefiFillScreen(inst);
            case SIROpcode::LOWLEVEL_UEFI_DRAW_RECT:
                return emitLowlevelUefiDrawRect(inst);
            case SIROpcode::LOWLEVEL_UEFI_GOP_BLT:
                return emitLowlevelUefiGopBlt(inst);

            // 16d. خدمات وقت التشغيل
            case SIROpcode::LOWLEVEL_UEFI_GET_TIME:
                return emitLowlevelUefiGetTime(inst);
            case SIROpcode::LOWLEVEL_UEFI_SET_TIME:
                return emitLowlevelUefiSetTime(inst);
            case SIROpcode::LOWLEVEL_UEFI_GET_VARIABLE:
                return emitLowlevelUefiGetVariable(inst);
            case SIROpcode::LOWLEVEL_UEFI_SET_VARIABLE:
                return emitLowlevelUefiSetVariable(inst);

            // 16e. نظام الملفات
            case SIROpcode::LOWLEVEL_UEFI_OPEN_VOLUME:
                return emitLowlevelUefiOpenVolume(inst);
            case SIROpcode::LOWLEVEL_UEFI_OPEN_FILE:
                return emitLowlevelUefiOpenFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_READ_FILE:
                return emitLowlevelUefiReadFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_WRITE_FILE:
                return emitLowlevelUefiWriteFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_CLOSE_FILE:
                return emitLowlevelUefiCloseFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_FILE_INFO:
                return emitLowlevelUefiFileInfo(inst);

            // 16f. بروتوكولات ومعلومات
            case SIROpcode::LOWLEVEL_UEFI_LOCATE_PROTOCOL:
                return emitLowlevelUefiLocateProtocol(inst);
            case SIROpcode::LOWLEVEL_UEFI_REVISION:
                return emitLowlevelUefiRevision(inst);
            case SIROpcode::LOWLEVEL_UEFI_VENDOR:
                return emitLowlevelUefiVendor(inst);
            case SIROpcode::LOWLEVEL_UEFI_FW_REVISION:
                return emitLowlevelUefiFwRevision(inst);
            case SIROpcode::LOWLEVEL_UEFI_REPORT:
                return emitLowlevelUefiReport(inst);

            // --- القسم 17: ACPI الموسّع ---
            case SIROpcode::LOWLEVEL_ACPI_INIT_FULL:
                return emitLowlevelAcpiInitFull(inst);
            case SIROpcode::LOWLEVEL_ACPI_INIT_RSDP:
                return emitLowlevelAcpiInitRsdp(inst);
            case SIROpcode::LOWLEVEL_ACPI_ENABLE:
                return emitLowlevelAcpiEnable(inst);
            case SIROpcode::LOWLEVEL_ACPI_DISABLE:
                return emitLowlevelAcpiDisable(inst);
            case SIROpcode::LOWLEVEL_ACPI_IS_INITIALIZED:
                return emitLowlevelAcpiIsInitialized(inst);
            case SIROpcode::LOWLEVEL_ACPI_VERSION:
                return emitLowlevelAcpiVersion(inst);
            case SIROpcode::LOWLEVEL_ACPI_REBOOT:
                return emitLowlevelAcpiReboot(inst);
            case SIROpcode::LOWLEVEL_ACPI_SLEEP:
                return emitLowlevelAcpiSleep(inst);
            case SIROpcode::LOWLEVEL_ACPI_DELAY_US:
                return emitLowlevelAcpiDelayUs(inst);
            case SIROpcode::LOWLEVEL_ACPI_READ_PM_TIMER:
                return emitLowlevelAcpiReadPmTimer(inst);
            case SIROpcode::LOWLEVEL_ACPI_IS_PM_32BIT:
                return emitLowlevelAcpiIsPm32bit(inst);
            case SIROpcode::LOWLEVEL_ACPI_PROCESSOR_COUNT:
                return emitLowlevelAcpiProcessorCount(inst);
            case SIROpcode::LOWLEVEL_ACPI_LOCAL_APIC_ADDR:
                return emitLowlevelAcpiLocalApicAddr(inst);
            case SIROpcode::LOWLEVEL_ACPI_ECAM_BASE:
                return emitLowlevelAcpiEcamBase(inst);

            // --- القسم 18: APIC الموسّع ---
            case SIROpcode::LOWLEVEL_APIC_SUPPORTED:
                return emitLowlevelApicSupported(inst);
            case SIROpcode::LOWLEVEL_APIC_X2_SUPPORTED:
                return emitLowlevelApicX2Supported(inst);
            case SIROpcode::LOWLEVEL_APIC_ID:
                return emitLowlevelApicId(inst);
            case SIROpcode::LOWLEVEL_APIC_IO_COUNT:
                return emitLowlevelApicIoCount(inst);
            case SIROpcode::LOWLEVEL_APIC_INIT_TIMER:
                return emitLowlevelApicInitTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_START_TIMER:
                return emitLowlevelApicStartTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_STOP_TIMER:
                return emitLowlevelApicStopTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_TIMER_COUNT:
                return emitLowlevelApicTimerCount(inst);
            case SIROpcode::LOWLEVEL_APIC_CALIBRATE:
                return emitLowlevelApicCalibrate(inst);
            case SIROpcode::LOWLEVEL_APIC_SET_PRIORITY:
                return emitLowlevelApicSetPriority(inst);
            case SIROpcode::LOWLEVEL_APIC_DISABLE_PIC:
                return emitLowlevelApicDisablePic(inst);
            case SIROpcode::LOWLEVEL_APIC_MASK_IRQ:
                return emitLowlevelApicMaskIrq(inst);
            case SIROpcode::LOWLEVEL_APIC_UNMASK_IRQ:
                return emitLowlevelApicUnmaskIrq(inst);
            case SIROpcode::LOWLEVEL_APIC_ROUTE_IRQ:
                return emitLowlevelApicRouteIrq(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_IPI_ALL:
                return emitLowlevelApicSendIpiAll(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_INIT_IPI:
                return emitLowlevelApicSendInitIpi(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_SIPI:
                return emitLowlevelApicSendSipi(inst);
            case SIROpcode::LOWLEVEL_APIC_WAIT_DELIVERY:
                return emitLowlevelApicWaitDelivery(inst);
            case SIROpcode::LOWLEVEL_APIC_INIT_IO:
                return emitLowlevelApicInitIo(inst);


            default:
                return nullptr; // (AR) غير مدعوم هنا / (EN) not handled here
            }
        }

    } // namespace LLVM
} // namespace Sad
