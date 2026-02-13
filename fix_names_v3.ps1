# fix_names_v3.ps1 - Fix all naming mismatches
$enc = New-Object System.Text.UTF8Encoding($false)
$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$c = [System.IO.File]::ReadAllText($path, $enc)

# 1. Fix Security method names: emitSecurity* -> emitBuiltinSecurity*
$secReplacements = @(
    @("emitSecurityAssert(","emitBuiltinSecurityAssert("),
    @("emitSecurityAssertType(","emitBuiltinSecurityAssertType("),
    @("emitSecurityAssertEqual(","emitBuiltinSecurityAssertEqual("),
    @("emitSecurityAssertGreater(","emitBuiltinSecurityAssertGreater("),
    @("emitSecurityVerify(","emitBuiltinSecurityVerify("),
    @("emitSecurityIsSafe(","emitBuiltinSecurityIsSafe("),
    @("emitSecurityPanic(","emitBuiltinSecurityPanic("),
    @("emitSecurityHash(","emitBuiltinSecurityHash("),
    @("emitSecurityEncrypt(","emitBuiltinSecurityEncrypt("),
    @("emitSecurityDecrypt(","emitBuiltinSecurityDecrypt("),
    @("emitSecuritySanitize(","emitBuiltinSecuritySanitize("),
    @("emitSecurityTimestamp(","emitBuiltinSecurityTimestamp("),
    @("emitSecuritySecureRandom(","emitBuiltinSecuritySecureRandom("),
    @("emitSecurityBase64Encode(","emitBuiltinSecurityBase64Encode(")
)
foreach($r in $secReplacements) { $c = $c.Replace($r[0], $r[1]) }
Write-Output "Fixed security method names"

# 2. Fix SIRType references in resolveOperand
$c = $c.Replace("case SIRType::I32:", "// case SIRType::I32:")
$c = $c.Replace("case SIRType::I16:", "// case SIRType::I16:")
$c = $c.Replace("case SIRType::I8:", "// case SIRType::I8:")
$c = $c.Replace("case SIRType::F32:", "// case SIRType::F32:")
Write-Output "Fixed SIRType references"

# 3. Replace emitBuiltinMath dispatch in switch with individual emitBuiltinXxx calls
# The switch currently has:
#   case SIROpcode::BUILTIN_SQRT: ... case SIROpcode::BUILTIN_POW: return emitBuiltinMath(inst);
# Need to split into individual cases

$oldMathBlock = @"
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
"@

$newMathBlock = @"
        case SIROpcode::BUILTIN_SQRT:
            return emitBuiltinSqrt(inst);
        case SIROpcode::BUILTIN_SIN:
            return emitBuiltinSin(inst);
        case SIROpcode::BUILTIN_COS:
            return emitBuiltinCos(inst);
        case SIROpcode::BUILTIN_TAN:
            return emitBuiltinTan(inst);
        case SIROpcode::BUILTIN_ABS:
            return emitBuiltinAbs(inst);
        case SIROpcode::BUILTIN_FLOOR:
            return emitBuiltinFloor(inst);
        case SIROpcode::BUILTIN_CEIL:
            return emitBuiltinCeil(inst);
        case SIROpcode::BUILTIN_ROUND:
            return emitBuiltinRound(inst);
        case SIROpcode::BUILTIN_POW:
            return emitBuiltinPow(inst);
"@

$c = $c.Replace($oldMathBlock, $newMathBlock)
Write-Output "Split math cases"

# 4. Replace the single emitBuiltinMath implementation with individual math methods
$oldMathImpl = 'llvm::Value* LLVMCodeGen::emitBuiltinMath(std::shared_ptr<SIRInstruction> inst)'
$idx = $c.IndexOf($oldMathImpl)
if ($idx -gt 0) {
    # Find end of the function
    $braceCount = 0
    $funcStart = $c.IndexOf("{", $idx)
    $funcEnd = $funcStart
    for ($i = $funcStart; $i -lt $c.Length; $i++) {
        if ($c[$i] -eq '{') { $braceCount++ }
        if ($c[$i] -eq '}') { $braceCount--; if ($braceCount -eq 0) { $funcEnd = $i; break } }
    }
    # Remove the old function
    $c = $c.Remove($idx, $funcEnd - $idx + 1)
    
    # Insert individual math functions
    $mathFunctions = @'
llvm::Value* LLVMCodeGen::emitBuiltinSqrt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("sqrt", ft);
    llvm::Value* result = builder_->CreateCall(fn, {dArg}, "sqrt.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinSin(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("sin", ft);
    llvm::Value* result = builder_->CreateCall(fn, {dArg}, "sin.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinCos(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("cos", ft);
    llvm::Value* result = builder_->CreateCall(fn, {dArg}, "cos.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinTan(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("tan", ft);
    llvm::Value* result = builder_->CreateCall(fn, {dArg}, "tan.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinAbs(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("fabs", ft);
    llvm::Value* result = builder_->CreateCall(fn, {dArg}, "fabs.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinFloor(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("floor", ft);
    llvm::Value* result = builder_->CreateFPToSI(builder_->CreateCall(fn, {dArg}, "floor.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinCeil(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("ceil", ft);
    llvm::Value* result = builder_->CreateFPToSI(builder_->CreateCall(fn, {dArg}, "ceil.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinRound(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOperand(inst->operands[0]);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg->getType()->isDoubleTy() ? arg : builder_->CreateSIToFP(arg, llvm::Type::getDoubleTy(*context_));
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {llvm::Type::getDoubleTy(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("round", ft);
    llvm::Value* result = builder_->CreateFPToSI(builder_->CreateCall(fn, {dArg}, "round.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinPow(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* base = resolveOperand(inst->operands[0]);
    llvm::Value* exp = resolveOperand(inst->operands[1]);
    if (!base || !exp) return nullptr;
    llvm::Type* dblTy = llvm::Type::getDoubleTy(*context_);
    llvm::Value* dBase = base->getType()->isDoubleTy() ? base : builder_->CreateSIToFP(base, dblTy);
    llvm::Value* dExp = exp->getType()->isDoubleTy() ? exp : builder_->CreateSIToFP(exp, dblTy);
    llvm::FunctionType* ft = llvm::FunctionType::get(dblTy, {dblTy, dblTy}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("pow", ft);
    llvm::Value* result = builder_->CreateCall(fn, {dBase, dExp}, "pow.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

'@
    $c = $c.Insert($idx, $mathFunctions)
    Write-Output "Replaced emitBuiltinMath with 9 individual math functions"
} else {
    Write-Output "WARNING: emitBuiltinMath not found"
}

[System.IO.File]::WriteAllText($path, $c, $enc)
Write-Output "Done! File: $([System.IO.File]::ReadAllBytes($path).Length) bytes"
