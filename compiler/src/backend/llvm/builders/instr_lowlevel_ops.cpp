/*
 * (AR) مولد تعليمات LLVM — مستوى منخفض: CPU / GDT / Paging / IDT / PCI / DMA / FB / ACPI / Sync / Sched / Boot / VFS / APIC / HPET / Syscall / Mem / UEFI
 * (EN) LLVM instruction emitter — Low-level: CPU / GDT / Paging / IDT / PCI / DMA / FB / ACPI / Sync / Sched / Boot / VFS / APIC / HPET / Syscall / Mem / UEFI
 * مستخرج من: llvm_codegen_instructions.cpp (سطور 741-1099)
 */

#include "llvm_codegen.h"
#include "builders/instr_lowlevel_codegen.h"
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
        llvm::Value *InstrLowlevelCodeGen::emitInstructionLowlevel(std::shared_ptr<SIRInstruction> inst)
        {
            switch (inst->opcode)
            {

            // --- 15a: CPU ---
            case SIROpcode::LOWLEVEL_CPU_GET_INFO:
                return cg_.emitLowlevelCpuGetInfo(inst);
            case SIROpcode::LOWLEVEL_CPU_GET_FEATURES:
                return cg_.emitLowlevelCpuGetFeatures(inst);
            case SIROpcode::LOWLEVEL_CPU_READ_MSR:
                return cg_.emitLowlevelCpuReadMSR(inst);
            case SIROpcode::LOWLEVEL_CPU_WRITE_MSR:
                return cg_.emitLowlevelCpuWriteMSR(inst);
            case SIROpcode::LOWLEVEL_CPU_READ_CR:
                return cg_.emitLowlevelCpuReadCR(inst);
            case SIROpcode::LOWLEVEL_CPU_WRITE_CR:
                return cg_.emitLowlevelCpuWriteCR(inst);
            case SIROpcode::LOWLEVEL_CPU_INVLPG:
                return cg_.emitLowlevelCpuInvlpg(inst);
            case SIROpcode::LOWLEVEL_CPU_GET_REPORT:
                return cg_.emitLowlevelCpuGetReport(inst);

            // --- 15b: GDT ---
            case SIROpcode::LOWLEVEL_GDT_INIT:
                return cg_.emitLowlevelGdtInit(inst);
            case SIROpcode::LOWLEVEL_GDT_LOAD:
                return cg_.emitLowlevelGdtLoad(inst);
            case SIROpcode::LOWLEVEL_GDT_GET_REPORT:
                return cg_.emitLowlevelGdtGetReport(inst);

            // --- 15c: Paging ---
            case SIROpcode::LOWLEVEL_PAGING_INIT:
                return cg_.emitLowlevelPagingInit(inst);
            case SIROpcode::LOWLEVEL_PAGING_MAP:
                return cg_.emitLowlevelPagingMap(inst);
            case SIROpcode::LOWLEVEL_PAGING_UNMAP:
                return cg_.emitLowlevelPagingUnmap(inst);
            case SIROpcode::LOWLEVEL_PAGING_FLUSH_TLB:
                return cg_.emitLowlevelPagingFlushTlb(inst);
            case SIROpcode::LOWLEVEL_PAGING_GET_REPORT:
                return cg_.emitLowlevelPagingGetReport(inst);

            // --- 15d: IDT ---
            case SIROpcode::LOWLEVEL_IDT_INIT:
                return cg_.emitLowlevelIdtInit(inst);
            case SIROpcode::LOWLEVEL_IDT_LOAD:
                return cg_.emitLowlevelIdtLoad(inst);
            case SIROpcode::LOWLEVEL_IDT_REGISTER_ISR:
                return cg_.emitLowlevelIdtRegisterIsr(inst);
            case SIROpcode::LOWLEVEL_IDT_ENABLE_IRQ:
                return cg_.emitLowlevelIdtEnableIrq(inst);
            case SIROpcode::LOWLEVEL_IDT_GET_REPORT:
                return cg_.emitLowlevelIdtGetReport(inst);

            // --- 15e: PCI ---
            case SIROpcode::LOWLEVEL_PCI_ENUMERATE:
                return cg_.emitLowlevelPciEnumerate(inst);
            case SIROpcode::LOWLEVEL_PCI_READ_CONFIG:
                return cg_.emitLowlevelPciReadConfig(inst);
            case SIROpcode::LOWLEVEL_PCI_WRITE_CONFIG:
                return cg_.emitLowlevelPciWriteConfig(inst);
            case SIROpcode::LOWLEVEL_PCI_GET_DEVICE_COUNT:
                return cg_.emitLowlevelPciGetDeviceCount(inst);
            case SIROpcode::LOWLEVEL_PCI_GET_REPORT:
                return cg_.emitLowlevelPciGetReport(inst);

            // --- 15f: DMA ---
            case SIROpcode::LOWLEVEL_DMA_INIT:
                return cg_.emitLowlevelDmaInit(inst);
            case SIROpcode::LOWLEVEL_DMA_TRANSFER:
                return cg_.emitLowlevelDmaTransfer(inst);
            case SIROpcode::LOWLEVEL_DMA_STATUS:
                return cg_.emitLowlevelDmaStatus(inst);
            case SIROpcode::LOWLEVEL_DMA_GET_REPORT:
                return cg_.emitLowlevelDmaGetReport(inst);

            // --- 15g: Framebuffer ---
            case SIROpcode::LOWLEVEL_FB_INIT:
                return cg_.emitLowlevelFbInit(inst);
            case SIROpcode::LOWLEVEL_FB_SET_PIXEL:
                return cg_.emitLowlevelFbSetPixel(inst);
            case SIROpcode::LOWLEVEL_FB_DRAW_RECT:
                return cg_.emitLowlevelFbDrawRect(inst);
            case SIROpcode::LOWLEVEL_FB_FILL_RECT:
                return cg_.emitLowlevelFbFillRect(inst);
            case SIROpcode::LOWLEVEL_FB_DRAW_LINE:
                return cg_.emitLowlevelFbDrawLine(inst);
            case SIROpcode::LOWLEVEL_FB_DRAW_STRING:
                return cg_.emitLowlevelFbDrawString(inst);
            case SIROpcode::LOWLEVEL_FB_CLEAR:
                return cg_.emitLowlevelFbClear(inst);
            case SIROpcode::LOWLEVEL_FB_GET_REPORT:
                return cg_.emitLowlevelFbGetReport(inst);

            // --- 15h: ACPI ---
            case SIROpcode::LOWLEVEL_ACPI_INIT:
                return cg_.emitLowlevelAcpiInit(inst);
            case SIROpcode::LOWLEVEL_ACPI_FIND_TABLE:
                return cg_.emitLowlevelAcpiFindTable(inst);
            case SIROpcode::LOWLEVEL_ACPI_SHUTDOWN:
                return cg_.emitLowlevelAcpiShutdown(inst);
            case SIROpcode::LOWLEVEL_ACPI_GET_REPORT:
                return cg_.emitLowlevelAcpiGetReport(inst);

            // --- 15i: Sync ---
            case SIROpcode::LOWLEVEL_SPINLOCK_INIT:
                return cg_.emitLowlevelSpinlockInit(inst);
            case SIROpcode::LOWLEVEL_SPINLOCK_LOCK:
                return cg_.emitLowlevelSpinlockLock(inst);
            case SIROpcode::LOWLEVEL_SPINLOCK_UNLOCK:
                return cg_.emitLowlevelSpinlockUnlock(inst);
            case SIROpcode::LOWLEVEL_MUTEX_INIT:
                return cg_.emitLowlevelMutexInit(inst);
            case SIROpcode::LOWLEVEL_MUTEX_LOCK:
                return cg_.emitLowlevelMutexLock(inst);
            case SIROpcode::LOWLEVEL_MUTEX_UNLOCK:
                return cg_.emitLowlevelMutexUnlock(inst);
            case SIROpcode::LOWLEVEL_SEMAPHORE_INIT:
                return cg_.emitLowlevelSemaphoreInit(inst);
            case SIROpcode::LOWLEVEL_BARRIER_INIT:
                return cg_.emitLowlevelBarrierInit(inst);

            // --- 15j: Scheduler ---
            case SIROpcode::LOWLEVEL_SCHED_INIT:
                return cg_.emitLowlevelSchedInit(inst);
            case SIROpcode::LOWLEVEL_SCHED_CREATE_PROC:
                return cg_.emitLowlevelSchedCreateProc(inst);
            case SIROpcode::LOWLEVEL_SCHED_CREATE_THREAD:
                return cg_.emitLowlevelSchedCreateThread(inst);
            case SIROpcode::LOWLEVEL_SCHED_YIELD:
                return cg_.emitLowlevelSchedYield(inst);
            case SIROpcode::LOWLEVEL_SCHED_SLEEP:
                return cg_.emitLowlevelSchedSleep(inst);
            case SIROpcode::LOWLEVEL_SCHED_GET_REPORT:
                return cg_.emitLowlevelSchedGetReport(inst);

            // --- 15k: Boot ---
            case SIROpcode::LOWLEVEL_BOOT_GET_INFO:
                return cg_.emitLowlevelBootGetInfo(inst);
            case SIROpcode::LOWLEVEL_BOOT_GET_MEMORY_MAP:
                return cg_.emitLowlevelBootGetMemoryMap(inst);
            case SIROpcode::LOWLEVEL_BOOT_GET_REPORT:
                return cg_.emitLowlevelBootGetReport(inst);

            // --- 15l: VFS ---
            case SIROpcode::LOWLEVEL_VFS_MOUNT:
                return cg_.emitLowlevelVfsMount(inst);
            case SIROpcode::LOWLEVEL_VFS_UNMOUNT:
                return cg_.emitLowlevelVfsUnmount(inst);
            case SIROpcode::LOWLEVEL_VFS_OPEN:
                return cg_.emitLowlevelVfsOpen(inst);
            case SIROpcode::LOWLEVEL_VFS_READ:
                return cg_.emitLowlevelVfsRead(inst);
            case SIROpcode::LOWLEVEL_VFS_WRITE:
                return cg_.emitLowlevelVfsWrite(inst);
            case SIROpcode::LOWLEVEL_VFS_CLOSE:
                return cg_.emitLowlevelVfsClose(inst);
            case SIROpcode::LOWLEVEL_VFS_GET_REPORT:
                return cg_.emitLowlevelVfsGetReport(inst);

            // --- 15m: APIC ---
            case SIROpcode::LOWLEVEL_APIC_INIT:
                return cg_.emitLowlevelApicInit(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_EOI:
                return cg_.emitLowlevelApicSendEoi(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_IPI:
                return cg_.emitLowlevelApicSendIpi(inst);
            case SIROpcode::LOWLEVEL_APIC_SET_TIMER:
                return cg_.emitLowlevelApicSetTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_GET_REPORT:
                return cg_.emitLowlevelApicGetReport(inst);

            // --- 15n: HPET ---
            case SIROpcode::LOWLEVEL_HPET_INIT:
                return cg_.emitLowlevelHpetInit(inst);
            case SIROpcode::LOWLEVEL_HPET_READ:
                return cg_.emitLowlevelHpetRead(inst);
            case SIROpcode::LOWLEVEL_HPET_SLEEP:
                return cg_.emitLowlevelHpetSleep(inst);
            case SIROpcode::LOWLEVEL_HPET_GET_REPORT:
                return cg_.emitLowlevelHpetGetReport(inst);

            // --- 15o: Syscall ---
            case SIROpcode::LOWLEVEL_SYSCALL_INIT:
                return cg_.emitLowlevelSyscallInit(inst);
            case SIROpcode::LOWLEVEL_SYSCALL_REGISTER:
                return cg_.emitLowlevelSyscallRegister(inst);
            case SIROpcode::LOWLEVEL_SYSCALL_INVOKE:
                return cg_.emitLowlevelSyscallInvoke(inst);
            case SIROpcode::LOWLEVEL_SYSCALL_GET_REPORT:
                return cg_.emitLowlevelSyscallGetReport(inst);

            // --- 15p: Memory ---
            case SIROpcode::LOWLEVEL_MEM_ALLOC_PHYS:
                return cg_.emitLowlevelMemAllocPhys(inst);
            case SIROpcode::LOWLEVEL_MEM_FREE_PHYS:
                return cg_.emitLowlevelMemFreePhys(inst);
            case SIROpcode::LOWLEVEL_MEM_MAP_REGION:
                return cg_.emitLowlevelMemMapRegion(inst);
            case SIROpcode::LOWLEVEL_MEM_GET_REPORT:
                return cg_.emitLowlevelMemGetReport(inst);

            // =================================================================
            // القسم 16: بروتوكول UEFI / UEFI Boot Protocol
            // =================================================================

            // 16a. التهيئة والتحكم
            case SIROpcode::LOWLEVEL_UEFI_INIT:
                return cg_.emitLowlevelUefiInit(inst);
            case SIROpcode::LOWLEVEL_UEFI_EXIT_BOOT_SERVICES:
                return cg_.emitLowlevelUefiExitBootServices(inst);
            case SIROpcode::LOWLEVEL_UEFI_IS_INITIALIZED:
                return cg_.emitLowlevelUefiIsInitialized(inst);
            case SIROpcode::LOWLEVEL_UEFI_BS_EXITED:
                return cg_.emitLowlevelUefiBsExited(inst);
            case SIROpcode::LOWLEVEL_UEFI_RESET_SYSTEM:
                return cg_.emitLowlevelUefiResetSystem(inst);

            // 16b. إدارة الذاكرة
            case SIROpcode::LOWLEVEL_UEFI_ALLOC_PAGES:
                return cg_.emitLowlevelUefiAllocPages(inst);
            case SIROpcode::LOWLEVEL_UEFI_FREE_PAGES:
                return cg_.emitLowlevelUefiFreePages(inst);
            case SIROpcode::LOWLEVEL_UEFI_ALLOC_POOL:
                return cg_.emitLowlevelUefiAllocPool(inst);
            case SIROpcode::LOWLEVEL_UEFI_FREE_POOL:
                return cg_.emitLowlevelUefiFreePool(inst);
            case SIROpcode::LOWLEVEL_UEFI_GET_MEMORY_MAP:
                return cg_.emitLowlevelUefiGetMemoryMap(inst);
            case SIROpcode::LOWLEVEL_UEFI_GET_MEMMAP_KEY:
                return cg_.emitLowlevelUefiGetMemmapKey(inst);
            case SIROpcode::LOWLEVEL_UEFI_TOTAL_MEMORY:
                return cg_.emitLowlevelUefiTotalMemory(inst);

            // 16c. بروتوكول الرسوميات GOP
            case SIROpcode::LOWLEVEL_UEFI_INIT_GOP:
                return cg_.emitLowlevelUefiInitGop(inst);
            case SIROpcode::LOWLEVEL_UEFI_SET_GOP_MODE:
                return cg_.emitLowlevelUefiSetGopMode(inst);
            case SIROpcode::LOWLEVEL_UEFI_QUERY_GOP_MODE:
                return cg_.emitLowlevelUefiQueryGopMode(inst);
            case SIROpcode::LOWLEVEL_UEFI_GOP_MODE_COUNT:
                return cg_.emitLowlevelUefiGopModeCount(inst);
            case SIROpcode::LOWLEVEL_UEFI_CURRENT_GOP_MODE:
                return cg_.emitLowlevelUefiCurrentGopMode(inst);
            case SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_BASE:
                return cg_.emitLowlevelUefiFramebufferBase(inst);
            case SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_SIZE:
                return cg_.emitLowlevelUefiFramebufferSize(inst);
            case SIROpcode::LOWLEVEL_UEFI_FILL_SCREEN:
                return cg_.emitLowlevelUefiFillScreen(inst);
            case SIROpcode::LOWLEVEL_UEFI_DRAW_RECT:
                return cg_.emitLowlevelUefiDrawRect(inst);
            case SIROpcode::LOWLEVEL_UEFI_GOP_BLT:
                return cg_.emitLowlevelUefiGopBlt(inst);

            // 16d. خدمات وقت التشغيل
            case SIROpcode::LOWLEVEL_UEFI_GET_TIME:
                return cg_.emitLowlevelUefiGetTime(inst);
            case SIROpcode::LOWLEVEL_UEFI_SET_TIME:
                return cg_.emitLowlevelUefiSetTime(inst);
            case SIROpcode::LOWLEVEL_UEFI_GET_VARIABLE:
                return cg_.emitLowlevelUefiGetVariable(inst);
            case SIROpcode::LOWLEVEL_UEFI_SET_VARIABLE:
                return cg_.emitLowlevelUefiSetVariable(inst);

            // 16e. نظام الملفات
            case SIROpcode::LOWLEVEL_UEFI_OPEN_VOLUME:
                return cg_.emitLowlevelUefiOpenVolume(inst);
            case SIROpcode::LOWLEVEL_UEFI_OPEN_FILE:
                return cg_.emitLowlevelUefiOpenFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_READ_FILE:
                return cg_.emitLowlevelUefiReadFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_WRITE_FILE:
                return cg_.emitLowlevelUefiWriteFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_CLOSE_FILE:
                return cg_.emitLowlevelUefiCloseFile(inst);
            case SIROpcode::LOWLEVEL_UEFI_FILE_INFO:
                return cg_.emitLowlevelUefiFileInfo(inst);

            // 16f. بروتوكولات ومعلومات
            case SIROpcode::LOWLEVEL_UEFI_LOCATE_PROTOCOL:
                return cg_.emitLowlevelUefiLocateProtocol(inst);
            case SIROpcode::LOWLEVEL_UEFI_REVISION:
                return cg_.emitLowlevelUefiRevision(inst);
            case SIROpcode::LOWLEVEL_UEFI_VENDOR:
                return cg_.emitLowlevelUefiVendor(inst);
            case SIROpcode::LOWLEVEL_UEFI_FW_REVISION:
                return cg_.emitLowlevelUefiFwRevision(inst);
            case SIROpcode::LOWLEVEL_UEFI_REPORT:
                return cg_.emitLowlevelUefiReport(inst);

            // --- القسم 17: ACPI الموسّع ---
            case SIROpcode::LOWLEVEL_ACPI_INIT_FULL:
                return cg_.emitLowlevelAcpiInitFull(inst);
            case SIROpcode::LOWLEVEL_ACPI_INIT_RSDP:
                return cg_.emitLowlevelAcpiInitRsdp(inst);
            case SIROpcode::LOWLEVEL_ACPI_ENABLE:
                return cg_.emitLowlevelAcpiEnable(inst);
            case SIROpcode::LOWLEVEL_ACPI_DISABLE:
                return cg_.emitLowlevelAcpiDisable(inst);
            case SIROpcode::LOWLEVEL_ACPI_IS_INITIALIZED:
                return cg_.emitLowlevelAcpiIsInitialized(inst);
            case SIROpcode::LOWLEVEL_ACPI_VERSION:
                return cg_.emitLowlevelAcpiVersion(inst);
            case SIROpcode::LOWLEVEL_ACPI_REBOOT:
                return cg_.emitLowlevelAcpiReboot(inst);
            case SIROpcode::LOWLEVEL_ACPI_SLEEP:
                return cg_.emitLowlevelAcpiSleep(inst);
            case SIROpcode::LOWLEVEL_ACPI_DELAY_US:
                return cg_.emitLowlevelAcpiDelayUs(inst);
            case SIROpcode::LOWLEVEL_ACPI_READ_PM_TIMER:
                return cg_.emitLowlevelAcpiReadPmTimer(inst);
            case SIROpcode::LOWLEVEL_ACPI_IS_PM_32BIT:
                return cg_.emitLowlevelAcpiIsPm32bit(inst);
            case SIROpcode::LOWLEVEL_ACPI_PROCESSOR_COUNT:
                return cg_.emitLowlevelAcpiProcessorCount(inst);
            case SIROpcode::LOWLEVEL_ACPI_LOCAL_APIC_ADDR:
                return cg_.emitLowlevelAcpiLocalApicAddr(inst);
            case SIROpcode::LOWLEVEL_ACPI_ECAM_BASE:
                return cg_.emitLowlevelAcpiEcamBase(inst);

            // --- القسم 18: APIC الموسّع ---
            case SIROpcode::LOWLEVEL_APIC_SUPPORTED:
                return cg_.emitLowlevelApicSupported(inst);
            case SIROpcode::LOWLEVEL_APIC_X2_SUPPORTED:
                return cg_.emitLowlevelApicX2Supported(inst);
            case SIROpcode::LOWLEVEL_APIC_ID:
                return cg_.emitLowlevelApicId(inst);
            case SIROpcode::LOWLEVEL_APIC_IO_COUNT:
                return cg_.emitLowlevelApicIoCount(inst);
            case SIROpcode::LOWLEVEL_APIC_INIT_TIMER:
                return cg_.emitLowlevelApicInitTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_START_TIMER:
                return cg_.emitLowlevelApicStartTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_STOP_TIMER:
                return cg_.emitLowlevelApicStopTimer(inst);
            case SIROpcode::LOWLEVEL_APIC_TIMER_COUNT:
                return cg_.emitLowlevelApicTimerCount(inst);
            case SIROpcode::LOWLEVEL_APIC_CALIBRATE:
                return cg_.emitLowlevelApicCalibrate(inst);
            case SIROpcode::LOWLEVEL_APIC_SET_PRIORITY:
                return cg_.emitLowlevelApicSetPriority(inst);
            case SIROpcode::LOWLEVEL_APIC_DISABLE_PIC:
                return cg_.emitLowlevelApicDisablePic(inst);
            case SIROpcode::LOWLEVEL_APIC_MASK_IRQ:
                return cg_.emitLowlevelApicMaskIrq(inst);
            case SIROpcode::LOWLEVEL_APIC_UNMASK_IRQ:
                return cg_.emitLowlevelApicUnmaskIrq(inst);
            case SIROpcode::LOWLEVEL_APIC_ROUTE_IRQ:
                return cg_.emitLowlevelApicRouteIrq(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_IPI_ALL:
                return cg_.emitLowlevelApicSendIpiAll(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_INIT_IPI:
                return cg_.emitLowlevelApicSendInitIpi(inst);
            case SIROpcode::LOWLEVEL_APIC_SEND_SIPI:
                return cg_.emitLowlevelApicSendSipi(inst);
            case SIROpcode::LOWLEVEL_APIC_WAIT_DELIVERY:
                return cg_.emitLowlevelApicWaitDelivery(inst);
            case SIROpcode::LOWLEVEL_APIC_INIT_IO:
                return cg_.emitLowlevelApicInitIo(inst);


            default:
                return nullptr; // (AR) غير مدعوم هنا / (EN) not handled here
            }
        }

    } // namespace LLVM
} // namespace Sad
