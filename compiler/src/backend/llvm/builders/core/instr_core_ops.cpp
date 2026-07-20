/*
 * (AR) مولد تعليمات LLVM — الجوهر: حساب / ثنائي / مقارنة / ذاكرة / سلاسل / تحكم / FFI / Async / كائنات / تحويل / مصفوفات / ملفات
 * (EN) LLVM instruction emitter — Core: arithmetic / bitwise / compare / memory / string / control / FFI / Async / objects / cast / arrays / files
 * مستخرج من: llvm_codegen_instructions.cpp (سطور 144-740)
 */

#include "llvm_codegen.h"
#include "builders/core/instr_core_codegen.h"
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
         * (AR) تنفيذ تعليمات الجوهر (حساب، ذاكرة، سلاسل، تحكم، FFI، async، كائنات، تحويل، مصفوفات، ملفات، متفرقات)
         * (EN) Emit core instructions (arithmetic, memory, string, control, FFI, async, objects, cast, arrays, files, misc)
         * @return nullptr إذا لم يتعرف على الـ opcode / nullptr if opcode not handled
         */
        llvm::Value *InstrCoreCodeGen::emitInstructionCore(std::shared_ptr<SIRInstruction> inst)
        {
            switch (inst->opcode)
            {
            // Arithmetic - Source: sir_types.h:120-129
            case SIROpcode::ADD_I64:
            case SIROpcode::ADD_F64:
                return cg_.emitAdd(inst);
            case SIROpcode::SUB_I64:
            case SIROpcode::SUB_F64:
                return cg_.emitSub(inst);
            case SIROpcode::MUL_I64:
            case SIROpcode::MUL_F64:
                return cg_.emitMul(inst);
            case SIROpcode::DIV_I64:
            case SIROpcode::DIV_F64:
            case SIROpcode::FLOOR_DIV_I64:
                return cg_.emitDiv(inst);
            case SIROpcode::MOD_I64:
                return cg_.emitMod(inst);
            case SIROpcode::NEG:
                return cg_.emitNeg(inst);
            case SIROpcode::NULL_ASSERT:
                return cg_.emitNullAssert(inst);

            // Bitwise - Source: sir_types.h:134-141
            case SIROpcode::AND:
                return cg_.emitAnd(inst);
            case SIROpcode::OR:
                return cg_.emitOr(inst);
            case SIROpcode::XOR:
                return cg_.emitXor(inst);
            case SIROpcode::NOT:
                return cg_.emitNot(inst);
            case SIROpcode::SHL:
                return cg_.emitShl(inst);
            case SIROpcode::SHR:
                return cg_.emitShr(inst);

            // Comparison - Source: sir_types.h:146-151
            case SIROpcode::EQ:
                return cg_.emitCmpEq(inst);
            case SIROpcode::NE:
                return cg_.emitCmpNe(inst);
            case SIROpcode::LT:
                return cg_.emitCmpLt(inst);
            case SIROpcode::LE:
                return cg_.emitCmpLe(inst);
            case SIROpcode::GT:
                return cg_.emitCmpGt(inst);
            case SIROpcode::GE:
                return cg_.emitCmpGe(inst);

            // Memory - Source: sir_types.h:168-177
            case SIROpcode::LOAD:
                return cg_.emitLoad(inst);
            case SIROpcode::STORE:
                return cg_.emitStore(inst);
            case SIROpcode::ALLOC:
                return cg_.emitAlloca(inst);
            case SIROpcode::MOVE:
                return cg_.emitMove(inst);

            // String - دمج النصوص
            case SIROpcode::STRING_CONCAT:
                return cg_.emitStringConcat(inst);
            case SIROpcode::STRING_CMP:
                return cg_.emitStringCmp(inst);
            case SIROpcode::BUILTIN_STRING_LENGTH:
                return cg_.emitFFIStrlen(inst);
            case SIROpcode::BUILTIN_STRING_CHAR_AT:
                return cg_.emitStringCharAt(inst);

            // Control Flow - Source: sir_types.h:156-163
            case SIROpcode::BR:
                return cg_.emitBranch(inst);
            case SIROpcode::BR_COND:
                return cg_.emitCondBranch(inst);
            case SIROpcode::CALL:
                return cg_.emitCall(inst);
            case SIROpcode::RET:
            case SIROpcode::RET_VOID:
                return cg_.emitReturn(inst);

            // ===== OS/Hardware - Port I/O =====
            case SIROpcode::INLINE_ASM:
                return cg_.emitInlineAsm(inst);
            case SIROpcode::BUILTIN_PORT_WRITE:
            case SIROpcode::BUILTIN_PORT_WRITE_16:
            case SIROpcode::BUILTIN_PORT_WRITE_32:
                return cg_.emitPortWrite(inst);
            case SIROpcode::BUILTIN_PORT_READ:
            case SIROpcode::BUILTIN_PORT_READ_16:
            case SIROpcode::BUILTIN_PORT_READ_32:
                return cg_.emitPortRead(inst);
            case SIROpcode::BUILTIN_MEM_WRITE_8:
            case SIROpcode::BUILTIN_MEM_WRITE_16:
            case SIROpcode::BUILTIN_MEM_WRITE_32:
            case SIROpcode::BUILTIN_MEM_WRITE_64:
                return cg_.emitMemWrite(inst);
            case SIROpcode::BUILTIN_MEM_READ_8:
            case SIROpcode::BUILTIN_MEM_READ_16:
            case SIROpcode::BUILTIN_MEM_READ_32:
            case SIROpcode::BUILTIN_MEM_READ_64:
                return cg_.emitMemRead(inst);
            case SIROpcode::BUILTIN_INTERRUPT:
                return cg_.emitInterrupt(inst);
            case SIROpcode::BUILTIN_HALT:
                return cg_.emitHalt(inst);
            case SIROpcode::BUILTIN_CLI:
                return cg_.emitCli(inst);
            case SIROpcode::BUILTIN_STI:
                return cg_.emitSti(inst);
            case SIROpcode::BUILTIN_VGA_WRITE:
                return cg_.emitVgaWrite(inst);
            case SIROpcode::BUILTIN_VGA_CLEAR:
                return cg_.emitVgaClear(inst);
            case SIROpcode::BUILTIN_ADDR_OF:
                return cg_.emitAddrOf(inst);
            case SIROpcode::BUILTIN_MEM_COPY:
                return cg_.emitMemCopy(inst);
            case SIROpcode::BUILTIN_MEM_SET:
                return cg_.emitMemSet(inst);
            case SIROpcode::BUILTIN_MEM_FILL_32:
                return cg_.emitMemFill32(inst);
            case SIROpcode::BUILTIN_MEM_COPY_32:
                return cg_.emitMemCopy32(inst);

            // ===== Embedded (18) =====
            case SIROpcode::BUILTIN_SERIAL_INIT:
                return cg_.emitSerialInit(inst);
            case SIROpcode::BUILTIN_SERIAL_WRITE:
                return cg_.emitSerialWrite(inst);
            case SIROpcode::BUILTIN_SERIAL_READ:
                return cg_.emitSerialRead(inst);
            case SIROpcode::BUILTIN_SERIAL_READY:
                return cg_.emitSerialReady(inst);
            case SIROpcode::BUILTIN_GPIO_WRITE:
                return cg_.emitGpioWrite(inst);
            case SIROpcode::BUILTIN_GPIO_READ:
                return cg_.emitGpioRead(inst);
            case SIROpcode::BUILTIN_GPIO_MODE:
                return cg_.emitGpioMode(inst);
            case SIROpcode::BUILTIN_TIMER_INIT:
                return cg_.emitTimerInit(inst);
            case SIROpcode::BUILTIN_TIMER_READ:
                return cg_.emitTimerRead(inst);
            case SIROpcode::BUILTIN_TIMER_WAIT:
                return cg_.emitTimerWait(inst);
            case SIROpcode::BUILTIN_RESET:
                return cg_.emitReset(inst);
            case SIROpcode::BUILTIN_CPUID:
                return cg_.emitCpuId(inst);
            case SIROpcode::BUILTIN_RDTSC:
                return cg_.emitRdtsc(inst);
            case SIROpcode::BUILTIN_MFENCE:
                return cg_.emitMfence(inst);
            case SIROpcode::BUILTIN_LFENCE:
                return cg_.emitLfence(inst);
            case SIROpcode::BUILTIN_SFENCE:
                return cg_.emitSfence(inst);
            case SIROpcode::BUILTIN_DMA_INIT:
                return cg_.emitDmaInit(inst);
            case SIROpcode::BUILTIN_DMA_START:
                return cg_.emitDmaStart(inst);

            // ===== Security (14) =====
            case SIROpcode::BUILTIN_SECURITY_ASSERT:
                return cg_.emitBuiltinSecurityAssert(inst);
            case SIROpcode::BUILTIN_SECURITY_VERIFY:
                return cg_.emitBuiltinSecurityVerify(inst);
            case SIROpcode::BUILTIN_SECURITY_IS_SAFE:
                return cg_.emitBuiltinSecurityIsSafe(inst);
            case SIROpcode::BUILTIN_SECURITY_PANIC:
                return cg_.emitBuiltinSecurityPanic(inst);
            case SIROpcode::BUILTIN_SECURITY_HASH:
                return cg_.emitBuiltinSecurityHash(inst);
            case SIROpcode::BUILTIN_SECURITY_ENCRYPT:
                return cg_.emitBuiltinSecurityEncrypt(inst);
            case SIROpcode::BUILTIN_SECURITY_DECRYPT:
                return cg_.emitBuiltinSecurityDecrypt(inst);
            case SIROpcode::BUILTIN_SECURITY_ASSERT_TYPE:
                return cg_.emitBuiltinSecurityAssertType(inst);
            case SIROpcode::BUILTIN_SECURITY_ASSERT_EQUAL:
                return cg_.emitBuiltinSecurityAssertEqual(inst);
            case SIROpcode::BUILTIN_SECURITY_ASSERT_GREATER:
                return cg_.emitBuiltinSecurityAssertGreater(inst);
            case SIROpcode::BUILTIN_SECURITY_SANITIZE:
                return cg_.emitBuiltinSecuritySanitize(inst);
            case SIROpcode::BUILTIN_SECURITY_TIMESTAMP:
                return cg_.emitBuiltinSecurityTimestamp(inst);
            case SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM:
                return cg_.emitBuiltinSecuritySecureRandom(inst);
            case SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE:
                return cg_.emitBuiltinSecurityBase64Encode(inst);
            case SIROpcode::BUILTIN_CRYPTO_BLAKE3_HASH:
                return cg_.emitBuiltinCryptoBlake3Hash(inst);
            case SIROpcode::BUILTIN_CRYPTO_BLAKE3_KEYED_HASH:
                return cg_.emitBuiltinCryptoBlake3KeyedHash(inst);

            // ===== FFI (20) =====
            case SIROpcode::FFI_PRINTF:
                return cg_.emitFFIPrintf(inst);
            case SIROpcode::FFI_MALLOC:
                return cg_.emitFFIMalloc(inst);
            case SIROpcode::FFI_FREE:
                return cg_.emitFFIFree(inst);
            case SIROpcode::FFI_REALLOC:
                return cg_.emitFFIRealloc(inst);
            case SIROpcode::FFI_CALLOC:
                return cg_.emitFFICalloc(inst);
            case SIROpcode::FFI_STRLEN:
                return cg_.emitFFIStrlen(inst);
            case SIROpcode::FFI_STRCPY:
                return cg_.emitFFIStrcpy(inst);
            case SIROpcode::FFI_STRCMP:
                return cg_.emitFFIStrcmp(inst);
            case SIROpcode::FFI_STRCAT:
                return cg_.emitFFIStrcat(inst);
            case SIROpcode::FFI_MEMCPY:
                return cg_.emitFFIMemcpy(inst);
            case SIROpcode::FFI_MEMSET:
                return cg_.emitFFIMemset(inst);
            case SIROpcode::FFI_FOPEN:
                return cg_.emitFFIFopen(inst);
            case SIROpcode::FFI_FCLOSE:
                return cg_.emitFFIFclose(inst);
            case SIROpcode::FFI_FWRITE:
                return cg_.emitFFIFwrite(inst);
            case SIROpcode::FFI_FREAD:
                return cg_.emitFFIFread(inst);
            case SIROpcode::FFI_SYSTEM:
                return cg_.emitFFISystem(inst);
            case SIROpcode::FFI_GETENV:
                return cg_.emitFFIGetenv(inst);
            case SIROpcode::FFI_ATOI:
                return cg_.emitFFIAtoi(inst);
            case SIROpcode::FFI_ATOF:
                return cg_.emitFFIAtof(inst);
            case SIROpcode::FFI_SNPRINTF:
                return cg_.emitFFISnprintf(inst);

            // ===== Builtin Core =====
            case SIROpcode::BUILTIN_PRINT:
                return cg_.emitBuiltinPrint(inst);
            case SIROpcode::BUILTIN_READ:
                return cg_.emitBuiltinRead(inst);
            case SIROpcode::BUILTIN_SQRT:
                return cg_.emitBuiltinSqrt(inst);
            case SIROpcode::BUILTIN_LOG:
                return cg_.emitBuiltinLog(inst);
            case SIROpcode::BUILTIN_SIN:
                return cg_.emitBuiltinSin(inst);
            case SIROpcode::BUILTIN_COS:
                return cg_.emitBuiltinCos(inst);
            case SIROpcode::BUILTIN_TAN:
                return cg_.emitBuiltinTan(inst);
            case SIROpcode::BUILTIN_ABS:
                return cg_.emitBuiltinAbs(inst);
            case SIROpcode::BUILTIN_FLOOR:
                return cg_.emitBuiltinFloor(inst);
            case SIROpcode::BUILTIN_CEIL:
                return cg_.emitBuiltinCeil(inst);
            case SIROpcode::BUILTIN_ROUND:
                return cg_.emitBuiltinRound(inst);
            case SIROpcode::BUILTIN_POW:
                return cg_.emitBuiltinPow(inst);
            case SIROpcode::BUILTIN_SLEEP:
                return cg_.emitBuiltinSleep(inst);
            case SIROpcode::BUILTIN_EXIT:
                return cg_.emitBuiltinExit(inst);
            case SIROpcode::BUILTIN_RANDOM:
                return cg_.emitBuiltinRandom(inst);
            case SIROpcode::BUILTIN_LOG10:
                return cg_.emitBuiltinLog10(inst);
            case SIROpcode::BUILTIN_LOG2:
                return cg_.emitBuiltinLog2(inst);
            case SIROpcode::BUILTIN_ASIN:
                return cg_.emitBuiltinAsin(inst);
            case SIROpcode::BUILTIN_ACOS:
                return cg_.emitBuiltinAcos(inst);
            case SIROpcode::BUILTIN_ATAN:
                return cg_.emitBuiltinAtan(inst);
            case SIROpcode::BUILTIN_TRUNC:
                return cg_.emitBuiltinTrunc(inst);
            case SIROpcode::BUILTIN_FMOD:
                return cg_.emitBuiltinFmod(inst);
            case SIROpcode::BUILTIN_CLAMP:
                return cg_.emitBuiltinClamp(inst);

            // ====================================================================
            // Async/Await & Concurrency (23 opcodes)
            // ====================================================================
            case SIROpcode::ASYNC_SPAWN:
                return cg_.emitAsyncSpawn(inst);
            case SIROpcode::ASYNC_AWAIT:
                return cg_.emitAsyncAwait(inst);
            case SIROpcode::ASYNC_YIELD:
                return cg_.emitAsyncYield(inst);
            case SIROpcode::ASYNC_SLEEP:
                return cg_.emitAsyncSleep(inst);
            case SIROpcode::ASYNC_CREATE_FUTURE:
                return cg_.emitAsyncCreateFuture(inst);
            case SIROpcode::ASYNC_RESOLVE_FUTURE:
                return cg_.emitAsyncResolveFuture(inst);
            case SIROpcode::ASYNC_GET_FUTURE:
                return cg_.emitAsyncGetFuture(inst);
            case SIROpcode::ASYNC_CREATE_CHANNEL:
                return cg_.emitAsyncCreateChannel(inst);
            case SIROpcode::ASYNC_CHANNEL_SEND:
                return cg_.emitAsyncChannelSend(inst);
            case SIROpcode::ASYNC_CHANNEL_RECV:
                return cg_.emitAsyncChannelRecv(inst);
            case SIROpcode::ASYNC_CHANNEL_CLOSE:
                return cg_.emitAsyncChannelClose(inst);
            case SIROpcode::ASYNC_CHANNEL_IS_CLOSED:
                return cg_.emitAsyncChannelIsClosed(inst);
            case SIROpcode::ASYNC_CHANNEL_HAS_DATA:
                return cg_.emitAsyncChannelHasData(inst);
            case SIROpcode::ASYNC_CHANNEL_SIZE:
                return cg_.emitAsyncChannelSize(inst);
            case SIROpcode::ASYNC_CHANNEL_CAPACITY:
                return cg_.emitAsyncChannelCapacity(inst);
            case SIROpcode::ASYNC_CHANNEL_TRY_SEND:
                return cg_.emitAsyncChannelTrySend(inst);
            case SIROpcode::ASYNC_CHANNEL_TRY_RECV:
                return cg_.emitAsyncChannelTryRecv(inst);
            case SIROpcode::ASYNC_CHANNEL_SEND_TIMEOUT:
                return cg_.emitAsyncChannelSendTimeout(inst);
            case SIROpcode::ASYNC_CHANNEL_RECV_TIMEOUT:
                return cg_.emitAsyncChannelRecvTimeout(inst);
            case SIROpcode::ASYNC_MUTEX_CREATE:
                return cg_.emitAsyncMutexCreate(inst);
            case SIROpcode::ASYNC_MUTEX_LOCK:
                return cg_.emitAsyncMutexLock(inst);
            case SIROpcode::ASYNC_MUTEX_UNLOCK:
                return cg_.emitAsyncMutexUnlock(inst);
            case SIROpcode::ASYNC_MUTEX_TRY_LOCK:
                return cg_.emitAsyncMutexTryLock(inst);
            case SIROpcode::ASYNC_MUTEX_IS_LOCKED:
                return cg_.emitAsyncMutexIsLocked(inst);
            case SIROpcode::ASYNC_FUTURE_IS_READY:
                return cg_.emitAsyncFutureIsReady(inst);
            case SIROpcode::ASYNC_THREAD_SPAWN:
                return cg_.emitAsyncThreadSpawn(inst);
            case SIROpcode::ASYNC_THREAD_JOIN:
                return cg_.emitAsyncThreadJoin(inst);
            case SIROpcode::ASYNC_ATOMIC_LOAD:
                return cg_.emitAsyncAtomicLoad(inst);
            case SIROpcode::ASYNC_ATOMIC_STORE:
                return cg_.emitAsyncAtomicStore(inst);
            case SIROpcode::ASYNC_ATOMIC_ADD:
                return cg_.emitAsyncAtomicAdd(inst);
            case SIROpcode::ASYNC_ATOMIC_CAS:
                return cg_.emitAsyncAtomicCAS(inst);
            case SIROpcode::ASYNC_WAIT_ALL:
                return cg_.emitAsyncWaitAll(inst);
            case SIROpcode::ASYNC_WAIT_ANY:
                return cg_.emitAsyncWaitAny(inst);
            case SIROpcode::ASYNC_SELECT:
                return cg_.emitAsyncSelect(inst);

            // ===== WaitGroup Opcodes =====
            case SIROpcode::ASYNC_WG_CREATE:
                return cg_.emitAsyncWgCreate(inst);
            case SIROpcode::ASYNC_WG_ADD:
                return cg_.emitAsyncWgAdd(inst);
            case SIROpcode::ASYNC_WG_DONE:
                return cg_.emitAsyncWgDone(inst);
            case SIROpcode::ASYNC_WG_WAIT:
                return cg_.emitAsyncWgWait(inst);
            case SIROpcode::ASYNC_WG_COUNT:
                return cg_.emitAsyncWgCount(inst);

            // ===== LLVM Coroutine Opcodes =====
            case SIROpcode::CORO_SUSPEND:
                return cg_.emitCoroSuspend(inst);
            case SIROpcode::CORO_RETURN:
                return cg_.emitCoroReturn(inst);

            // ===== Generator Opcodes =====
            case SIROpcode::GENERATOR_YIELD:
                return cg_.emitGeneratorYield(inst);
            case SIROpcode::GENERATOR_CONSUME:
                return cg_.emitGeneratorConsume(inst);

            // ===== Missing Control Flow =====
            case SIROpcode::SWITCH:
                return cg_.emitSwitch(inst);
            case SIROpcode::PHI:
                return cg_.emitPhi(inst);
            case SIROpcode::CALL_INDIRECT:
                return cg_.emitCallIndirect(inst);

            // ===== Closures / الإغلاقات =====
            case SIROpcode::CLOSURE_CREATE:
                return cg_.emitClosureCreate(inst);
            case SIROpcode::CLOSURE_CALL:
                return cg_.emitClosureCall(inst);
            case SIROpcode::ENV_LOAD:
                return cg_.emitEnvLoad(inst);
            case SIROpcode::ENV_STORE:
                return cg_.emitEnvStore(inst);

            // ===== Missing Bitwise =====
            case SIROpcode::SAR:
                return cg_.emitSar(inst);
            case SIROpcode::ROL:
                return cg_.emitRol(inst);

            // ===== Missing Memory =====
            case SIROpcode::ALLOC_HEAP:
                return cg_.emitAllocHeap(inst);
            case SIROpcode::FREE:
                return cg_.emitFreeMem(inst);
            case SIROpcode::ADDR:
                return cg_.emitAddr(inst);
            case SIROpcode::PTR_ADD:
                return cg_.emitPtrAdd(inst);
            case SIROpcode::PTR_CAST:
                return cg_.emitPtrCast(inst);
            case SIROpcode::MEMCPY:
                return cg_.emitMemCopy(inst);
            case SIROpcode::MEMSET:
                return cg_.emitMemSet(inst);

            // ===== Arrays (6) =====
            case SIROpcode::ARRAY_NEW:
                return cg_.emitArrayNew(inst);
            case SIROpcode::ARRAY_GET:
                return cg_.emitArrayGet(inst);
            case SIROpcode::ARRAY_SET:
                return cg_.emitArraySet(inst);
            case SIROpcode::ARRAY_LEN:
                return cg_.emitArrayLen(inst);
            case SIROpcode::ARRAY_APPEND:
                return cg_.emitBuiltinArrayAppend(inst);
            case SIROpcode::ARRAY_REMOVE:
                return cg_.emitBuiltinArrayRemove(inst);
            case SIROpcode::ARRAY_CONCAT:
                return cg_.emitArrayConcat(inst);

            // ===== Tuples (3) - يُعاد استخدام بنية المصفوفة =====
            // (AR) الصفوف تشترك مع المصفوفات في نفس بنية runtime
            // (EN) Tuples share the same runtime structure as arrays
            case SIROpcode::TUPLE_NEW:
                return cg_.emitArrayNew(inst);
            case SIROpcode::TUPLE_GET:
                return cg_.emitArrayGet(inst);
            case SIROpcode::TUPLE_LEN:
                return cg_.emitArrayLen(inst);

            // ===== Strings (8) =====
            case SIROpcode::STRING_NEW:
                return cg_.emitStringNew(inst);
            case SIROpcode::STRING_LEN:
                return cg_.emitFFIStrlen(inst);
            case SIROpcode::STRING_SUBSTR:
                return cg_.emitBuiltinStringSubstring(inst);
            case SIROpcode::STRING_FIND:
                return cg_.emitBuiltinStringFind(inst);
            case SIROpcode::STRING_REPLACE:
                return cg_.emitBuiltinStringReplace(inst);
            case SIROpcode::STRING_TO_I64:
                return cg_.emitStringToI64(inst);
            case SIROpcode::STRING_TO_F64:
                return cg_.emitStringToF64(inst);

            // ===== OOP (10) =====
            case SIROpcode::OBJECT_NEW:
                return cg_.emitObjectNew(inst);
            case SIROpcode::OBJECT_GET:
                return cg_.emitObjectGet(inst);
            case SIROpcode::OBJECT_SET:
                return cg_.emitObjectSet(inst);
            case SIROpcode::OBJECT_CALL:
                return cg_.emitObjectCall(inst);
            case SIROpcode::INSTANCEOF:
                return cg_.emitInstanceOf(inst);
            case SIROpcode::OBJECT_CAST:
                return cg_.emitObjectCast(inst);
            case SIROpcode::CLASS_DEF:
                return cg_.emitClassDef(inst);
            case SIROpcode::METHOD_DEF:
                return cg_.emitMethodDef(inst);
            case SIROpcode::FIELD_DEF:
                return cg_.emitFieldDef(inst);
            case SIROpcode::CONSTRUCTOR_CALL:
                return cg_.emitConstructorCall(inst);

            // ===== ADT Enums (8b) =====
            case SIROpcode::ENUM_CONSTRUCT:
                return cg_.emitEnumConstruct(inst);
            case SIROpcode::ENUM_GET_TAG:
                return cg_.emitEnumGetTag(inst);
            case SIROpcode::ENUM_GET_PAYLOAD:
                return cg_.emitEnumGetPayload(inst);
            case SIROpcode::ENUM_IS_VARIANT:
                return cg_.emitEnumIsVariant(inst);
            case SIROpcode::ENUM_FREE:
                return cg_.emitEnumFree(inst);

            // ===== Type Conversion (8) =====
            case SIROpcode::I64_TO_F64:
                return cg_.emitI64ToF64(inst);
            case SIROpcode::F64_TO_I64:
                return cg_.emitF64ToI64(inst);
            case SIROpcode::I64_TO_BOOL:
                return cg_.emitI64ToBool(inst);
            case SIROpcode::BOOL_TO_I64:
                return cg_.emitBoolToI64(inst);
            case SIROpcode::I64_TO_STRING:
                return cg_.emitI64ToString(inst);
            case SIROpcode::F64_TO_STRING:
                return cg_.emitF64ToString(inst);
            case SIROpcode::BOOL_TO_STRING:
                return cg_.emitBoolToString(inst);
            case SIROpcode::ARRAY_TO_STRING:
                return cg_.emitArrayToString(inst);
            case SIROpcode::TUPLE_TO_STRING:
                return cg_.emitArrayToString(inst);
            case SIROpcode::CAST:
                return cg_.emitCast(inst);

            // ===== Builtin String (10 more) =====
            case SIROpcode::BUILTIN_STRING_TO_UPPER:
                return cg_.emitBuiltinStringToUpper(inst);
            case SIROpcode::BUILTIN_STRING_TO_LOWER:
                return cg_.emitBuiltinStringToLower(inst);
            case SIROpcode::BUILTIN_STRING_FIND:
                return cg_.emitBuiltinStringFind(inst);
            case SIROpcode::BUILTIN_STRING_REPLACE:
                return cg_.emitBuiltinStringReplace(inst);
            case SIROpcode::BUILTIN_STRING_SUBSTRING:
                return cg_.emitBuiltinStringSubstring(inst);
            case SIROpcode::BUILTIN_STRING_TRIM:
                return cg_.emitBuiltinStringTrim(inst);
            case SIROpcode::BUILTIN_STRING_SPLIT:
                return cg_.emitBuiltinStringSplit(inst);
            case SIROpcode::BUILTIN_STRING_JOIN:
                return cg_.emitBuiltinStringJoin(inst);
            case SIROpcode::BUILTIN_STRING_STARTS_WITH:
                return cg_.emitBuiltinStringStartsWith(inst);
            case SIROpcode::BUILTIN_STRING_ENDS_WITH:
                return cg_.emitBuiltinStringEndsWith(inst);
            case SIROpcode::BUILTIN_STRING_CONTAINS:
                return cg_.emitBuiltinStringContains(inst);

            // ===== Builtin Array (10) =====
            case SIROpcode::BUILTIN_ARRAY_APPEND:
                return cg_.emitBuiltinArrayAppend(inst);
            case SIROpcode::BUILTIN_ARRAY_REMOVE:
                return cg_.emitBuiltinArrayRemove(inst);
            case SIROpcode::BUILTIN_ARRAY_SIZE:
                return cg_.emitBuiltinArraySize(inst);
            case SIROpcode::BUILTIN_ARRAY_INDEX_OF:
                return cg_.emitBuiltinArrayIndexOf(inst);
            case SIROpcode::BUILTIN_ARRAY_CONTAINS:
                return cg_.emitBuiltinArrayContains(inst);
            case SIROpcode::BUILTIN_ARRAY_REVERSE:
                return cg_.emitBuiltinArrayReverse(inst);
            case SIROpcode::BUILTIN_ARRAY_SORT:
                return cg_.emitBuiltinArraySort(inst);
            case SIROpcode::BUILTIN_ARRAY_FIRST:
                return cg_.emitBuiltinArrayFirst(inst);
            case SIROpcode::BUILTIN_ARRAY_LAST:
                return cg_.emitBuiltinArrayLast(inst);
            case SIROpcode::BUILTIN_ARRAY_SLICE:
                return cg_.emitBuiltinArraySlice(inst);

            // ===== Builtin File I/O (8) =====
            case SIROpcode::BUILTIN_FILE_READ:
                return cg_.emitBuiltinFileRead(inst);
            case SIROpcode::BUILTIN_FILE_WRITE:
                return cg_.emitBuiltinFileWrite(inst);
            case SIROpcode::BUILTIN_FILE_APPEND:
                return cg_.emitBuiltinFileAppend(inst);
            case SIROpcode::BUILTIN_FILE_DELETE:
                return cg_.emitBuiltinFileDelete(inst);
            case SIROpcode::BUILTIN_FILE_COPY:
                return cg_.emitBuiltinFileCopy(inst);
            case SIROpcode::BUILTIN_FILE_MOVE:
                return cg_.emitBuiltinFileMove(inst);
            case SIROpcode::BUILTIN_FILE_CREATE_DIR:
                return cg_.emitBuiltinFileCreateDir(inst);
            case SIROpcode::BUILTIN_FILE_LIST_DIR:
                return cg_.emitBuiltinFileListDir(inst);

            // ===== Builtin Extra =====
            case SIROpcode::BUILTIN_MIN:
                return cg_.emitBuiltinMin(inst);
            case SIROpcode::BUILTIN_MAX:
                return cg_.emitBuiltinMax(inst);
            case SIROpcode::BUILTIN_ASSERT:
                return cg_.emitBuiltinAssert(inst);
            case SIROpcode::BUILTIN_DEBUG:
                return cg_.emitBuiltinDebug(inst);
            case SIROpcode::BUILTIN_TYPE_OF:
                return cg_.emitBuiltinTypeOf(inst);

            // ===== New stdlib builtins =====
            case SIROpcode::BUILTIN_IS_INTEGER:
                return cg_.emitBuiltinIsType(inst, "integer");
            case SIROpcode::BUILTIN_IS_FLOAT:
                return cg_.emitBuiltinIsType(inst, "float");
            case SIROpcode::BUILTIN_IS_STRING:
                return cg_.emitBuiltinIsType(inst, "string");
            case SIROpcode::BUILTIN_IS_ARRAY:
                return cg_.emitBuiltinIsType(inst, "array");
            case SIROpcode::BUILTIN_TO_BOOL:
                return cg_.emitBuiltinToBool(inst);
            case SIROpcode::BUILTIN_READ_LINE:
                return cg_.emitBuiltinReadLine(inst);
            case SIROpcode::BUILTIN_CLEAR_SCREEN:
                return cg_.emitBuiltinClearScreen(inst);
            case SIROpcode::BUILTIN_SUM:
                return cg_.emitBuiltinSum(inst);

                // ====================================================================
                // Section 15: Low-Level OS Module Operations (85 opcodes)
                // (AR) عمليات الوحدات المنخفضة المستوى لتطوير أنظمة التشغيل
                // (EN) Low-level module operations for OS development
                // ====================================================================

            default:
                return nullptr; // (AR) غير مدعوم هنا — يُحوَّل للطبقة التالية / (EN) not handled here — forwarded to next tier
            }
        }

    } // namespace LLVM
} // namespace Sad
