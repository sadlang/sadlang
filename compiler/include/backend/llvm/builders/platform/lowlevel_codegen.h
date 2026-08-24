/*
 * ============================================================================
 * LowlevelCodeGen — توليد LLVM IR لتعليمات منخفضة المستوى (CPU/UEFI/ACPI/APIC/...)
 * Phase 7 Step 11 — 153 methods من ملفي lowlevel + lowlevel_uefi
 * ============================================================================
 */
#ifndef SAD_LLVM_LOWLEVEL_CODEGEN_H
#define SAD_LLVM_LOWLEVEL_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class LowlevelCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit LowlevelCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    LowlevelCodeGen(const LowlevelCodeGen &) = delete;
    LowlevelCodeGen &operator=(const LowlevelCodeGen &) = delete;

    llvm::Value *emitLowlevelAcpiDelayUs(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiDisable(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiEcamBase(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiEnable(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiFindTable(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiInitFull(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiInitRsdp(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiIsInitialized(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiIsPm32bit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiLocalApicAddr(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiProcessorCount(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiReadPmTimer(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiReboot(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiShutdown(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiSleep(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelAcpiVersion(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicCalibrate(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicDisablePic(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicId(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicInitIo(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicInitTimer(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicIoCount(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicMaskIrq(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicRouteIrq(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSendEoi(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSendInitIpi(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSendIpi(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSendIpiAll(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSendSipi(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSetPriority(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSetTimer(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicStartTimer(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicStopTimer(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicSupported(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicTimerCount(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicUnmaskIrq(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicWaitDelivery(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelApicX2Supported(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelBarrierInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelBootGetInfo(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelBootGetMemoryMap(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelBootGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuGetFeatures(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuGetInfo(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuInvlpg(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuReadCR(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuReadMSR(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuWriteCR(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelCpuWriteMSR(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSymbolAddr(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelDmaGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelDmaInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelDmaStatus(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelDmaTransfer(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbClear(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbDrawLine(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbDrawRect(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbDrawString(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbFillRect(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelFbSetPixel(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelGdtGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelTaskRegisterLoad(std::shared_ptr<SIRInstruction>); ///< (AR) [RFC 0059] ltr
    llvm::Value *emitLowlevelGdtInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelGdtLoad(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelHpetGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelHpetInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelHpetRead(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelHpetSleep(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelIdtEnableIrq(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelIdtGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelIdtInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelIdtLoad(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelIdtRegisterIsr(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelMemAllocPhys(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelMemFreePhys(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelMemGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelMemMapRegion(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelMutexInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelMutexLock(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelMutexUnlock(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPagingFlushTlb(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPagingGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPagingInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPagingMap(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPagingUnmap(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPciEnumerate(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPciGetDeviceCount(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPciGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPciReadConfig(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelPciWriteConfig(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSchedCreateProc(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSchedCreateThread(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSchedGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSchedInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSchedSleep(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSchedYield(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSemaphoreInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSpinlockInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSpinlockLock(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSpinlockUnlock(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSyscallGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSyscallInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSyscallInvoke(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelSyscallRegister(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiAllocPages(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiAllocPool(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiBsExited(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiCloseFile(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiCurrentGopMode(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiDrawRect(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiExitBootServices(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiFileInfo(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiFillScreen(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiFramebufferBase(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiFramebufferSize(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiFreePages(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiFreePool(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiFwRevision(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiGetMemmapKey(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiGetMemoryMap(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiGetTime(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiGetVariable(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiGopBlt(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiGopModeCount(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiInit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiInitGop(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiIsInitialized(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiLocateProtocol(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiOpenFile(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiOpenVolume(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiQueryGopMode(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiReadFile(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiResetSystem(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiRevision(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiSetGopMode(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiSetTime(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiSetVariable(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiTotalMemory(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiVendor(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelUefiWriteFile(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelVfsClose(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelVfsGetReport(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelVfsMount(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelVfsOpen(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelVfsRead(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelVfsUnmount(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitLowlevelVfsWrite(std::shared_ptr<SIRInstruction>);
};

}} // namespace Sad::LLVM
#endif
