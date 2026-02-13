# add_switch_cases.ps1
# Read file preserving UTF-8
$enc = New-Object System.Text.UTF8Encoding($false)
$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$c = [System.IO.File]::ReadAllText($path, $enc)

# Find the default case in the switch statement
$defaultIdx = $c.IndexOf("default:" + [char]13 + [char]10 + "            reportError")
if ($defaultIdx -lt 0) {
    $defaultIdx = $c.IndexOf("default:" + [char]10 + "            reportError")
}
if ($defaultIdx -lt 0) {
    # Try a broader search
    $matches = [regex]::Matches($c, 'default:\s*\r?\n\s+reportError\("Unsupported opcode')
    if ($matches.Count -gt 0) { $defaultIdx = $matches[0].Index }
}
Write-Output "Default case at: $defaultIdx"

if ($defaultIdx -lt 0) {
    Write-Output "ERROR: Cannot find default case"
    exit 1
}

$switchCases = @"

        // ===== OS/Hardware - Port I/O =====
        case SIROpcode::INLINE_ASM:
            return emitInlineAsm(inst);
        case SIROpcode::BUILTIN_PORT_WRITE:
        case SIROpcode::BUILTIN_PORT_WRITE_16:
        case SIROpcode::BUILTIN_PORT_WRITE_32:
            return emitPortWrite(inst);
        case SIROpcode::BUILTIN_PORT_READ:
        case SIROpcode::BUILTIN_PORT_READ_16:
        case SIROpcode::BUILTIN_PORT_READ_32:
            return emitPortRead(inst);
        case SIROpcode::BUILTIN_MEM_WRITE_8:
        case SIROpcode::BUILTIN_MEM_WRITE_16:
        case SIROpcode::BUILTIN_MEM_WRITE_32:
            return emitMemWrite(inst);
        case SIROpcode::BUILTIN_MEM_READ_8:
        case SIROpcode::BUILTIN_MEM_READ_16:
        case SIROpcode::BUILTIN_MEM_READ_32:
            return emitMemRead(inst);
        case SIROpcode::BUILTIN_INTERRUPT:
            return emitInterrupt(inst);
        case SIROpcode::BUILTIN_HALT:
            return emitHalt(inst);
        case SIROpcode::BUILTIN_CLI:
            return emitCli(inst);
        case SIROpcode::BUILTIN_STI:
            return emitSti(inst);
        case SIROpcode::BUILTIN_VGA_WRITE:
            return emitVgaWrite(inst);
        case SIROpcode::BUILTIN_VGA_CLEAR:
            return emitVgaClear(inst);
        case SIROpcode::BUILTIN_ADDR_OF:
            return emitAddrOf(inst);
        case SIROpcode::BUILTIN_MEM_COPY:
            return emitMemCopy(inst);
        case SIROpcode::BUILTIN_MEM_SET:
            return emitMemSet(inst);

        // ===== Embedded (18) =====
        case SIROpcode::BUILTIN_SERIAL_INIT:
            return emitSerialInit(inst);
        case SIROpcode::BUILTIN_SERIAL_WRITE:
            return emitSerialWrite(inst);
        case SIROpcode::BUILTIN_SERIAL_READ:
            return emitSerialRead(inst);
        case SIROpcode::BUILTIN_SERIAL_READY:
            return emitSerialReady(inst);
        case SIROpcode::BUILTIN_GPIO_WRITE:
            return emitGpioWrite(inst);
        case SIROpcode::BUILTIN_GPIO_READ:
            return emitGpioRead(inst);
        case SIROpcode::BUILTIN_GPIO_MODE:
            return emitGpioMode(inst);
        case SIROpcode::BUILTIN_TIMER_INIT:
            return emitTimerInit(inst);
        case SIROpcode::BUILTIN_TIMER_READ:
            return emitTimerRead(inst);
        case SIROpcode::BUILTIN_TIMER_WAIT:
            return emitTimerWait(inst);
        case SIROpcode::BUILTIN_RESET:
            return emitReset(inst);
        case SIROpcode::BUILTIN_CPUID:
            return emitCpuId(inst);
        case SIROpcode::BUILTIN_RDTSC:
            return emitRdtsc(inst);
        case SIROpcode::BUILTIN_MFENCE:
            return emitMfence(inst);
        case SIROpcode::BUILTIN_LFENCE:
            return emitLfence(inst);
        case SIROpcode::BUILTIN_SFENCE:
            return emitSfence(inst);
        case SIROpcode::BUILTIN_DMA_INIT:
            return emitDmaInit(inst);
        case SIROpcode::BUILTIN_DMA_START:
            return emitDmaStart(inst);

        // ===== Security (14) =====
        case SIROpcode::BUILTIN_SECURITY_ASSERT:
            return emitSecurityAssert(inst);
        case SIROpcode::BUILTIN_SECURITY_VERIFY:
            return emitSecurityVerify(inst);
        case SIROpcode::BUILTIN_SECURITY_IS_SAFE:
            return emitSecurityIsSafe(inst);
        case SIROpcode::BUILTIN_SECURITY_PANIC:
            return emitSecurityPanic(inst);
        case SIROpcode::BUILTIN_SECURITY_HASH:
            return emitSecurityHash(inst);
        case SIROpcode::BUILTIN_SECURITY_ENCRYPT:
            return emitSecurityEncrypt(inst);
        case SIROpcode::BUILTIN_SECURITY_DECRYPT:
            return emitSecurityDecrypt(inst);
        case SIROpcode::BUILTIN_SECURITY_ASSERT_TYPE:
            return emitSecurityAssertType(inst);
        case SIROpcode::BUILTIN_SECURITY_ASSERT_EQUAL:
            return emitSecurityAssertEqual(inst);
        case SIROpcode::BUILTIN_SECURITY_ASSERT_GREATER:
            return emitSecurityAssertGreater(inst);
        case SIROpcode::BUILTIN_SECURITY_SANITIZE:
            return emitSecuritySanitize(inst);
        case SIROpcode::BUILTIN_SECURITY_TIMESTAMP:
            return emitSecurityTimestamp(inst);
        case SIROpcode::BUILTIN_SECURITY_SECURE_RANDOM:
            return emitSecuritySecureRandom(inst);
        case SIROpcode::BUILTIN_SECURITY_BASE64_ENCODE:
            return emitSecurityBase64Encode(inst);

        // ===== FFI (20) =====
        case SIROpcode::FFI_PRINTF:
            return emitFfiPrintf(inst);
        case SIROpcode::FFI_MALLOC:
            return emitFfiMalloc(inst);
        case SIROpcode::FFI_FREE:
            return emitFfiFree(inst);
        case SIROpcode::FFI_REALLOC:
            return emitFfiRealloc(inst);
        case SIROpcode::FFI_CALLOC:
            return emitFfiCalloc(inst);
        case SIROpcode::FFI_STRLEN:
            return emitFfiStrlen(inst);
        case SIROpcode::FFI_STRCPY:
            return emitFfiStrcpy(inst);
        case SIROpcode::FFI_STRCMP:
            return emitFfiStrcmp(inst);
        case SIROpcode::FFI_STRCAT:
            return emitFfiStrcat(inst);
        case SIROpcode::FFI_MEMCPY:
            return emitFfiMemcpy(inst);
        case SIROpcode::FFI_MEMSET:
            return emitFfiMemset(inst);
        case SIROpcode::FFI_FOPEN:
            return emitFfiFopen(inst);
        case SIROpcode::FFI_FCLOSE:
            return emitFfiFclose(inst);
        case SIROpcode::FFI_FWRITE:
            return emitFfiFwrite(inst);
        case SIROpcode::FFI_FREAD:
            return emitFfiFread(inst);
        case SIROpcode::FFI_SYSTEM:
            return emitFfiSystem(inst);
        case SIROpcode::FFI_GETENV:
            return emitFfiGetenv(inst);
        case SIROpcode::FFI_ATOI:
            return emitFfiAtoi(inst);
        case SIROpcode::FFI_ATOF:
            return emitFfiAtof(inst);
        case SIROpcode::FFI_SNPRINTF:
            return emitFfiSnprintf(inst);

        // ===== Builtin Core =====
        case SIROpcode::BUILTIN_PRINT:
            return emitBuiltinPrint(inst);
        case SIROpcode::BUILTIN_READ:
            return emitBuiltinRead(inst);
        case SIROpcode::BUILTIN_SQRT:
        case SIROpcode::BUILTIN_SIN:
        case SIROpcode::BUILTIN_COS:
        case SIROpcode::BUILTIN_TAN:
        case SIROpcode::BUILTIN_ABS:
        case SIROpcode::BUILTIN_FLOOR:
        case SIROpcode::BUILTIN_CEIL:
        case SIROpcode::BUILTIN_ROUND:
        case SIROpcode::BUILTIN_POW:
            return emitBuiltinMath(inst);
        case SIROpcode::BUILTIN_SLEEP:
            return emitBuiltinSleep(inst);
        case SIROpcode::BUILTIN_EXIT:
            return emitBuiltinExit(inst);
        case SIROpcode::BUILTIN_RANDOM:
            return emitBuiltinRandom(inst);

"@

$c = $c.Insert($defaultIdx, $switchCases)
[System.IO.File]::WriteAllText($path, $c, $enc)
Write-Output "Switch cases inserted. New size: $($c.Length)"
