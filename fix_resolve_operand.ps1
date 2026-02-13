# fix_resolve_operand.ps1
$enc = New-Object System.Text.UTF8Encoding($false)
$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$c = [System.IO.File]::ReadAllText($path, $enc)

# 1. Add #include <llvm/IR/InlineAsm.h> if not present
if ($c.IndexOf("InlineAsm.h") -lt 0) {
    # Find a good place to add it - after the last llvm include
    $lastInclude = $c.LastIndexOf("#include <llvm/")
    $endOfLine = $c.IndexOf("`n", $lastInclude)
    $c = $c.Insert($endOfLine + 1, "#include <llvm/IR/InlineAsm.h>`r`n")
    Write-Output "Added InlineAsm.h include"
}

# 2. Replace all resolveOsOperand(..., *context_, *builder_, context_info_.namedValues) with resolveOperand(...)
# The pattern: resolveOsOperand(X, *context_, *builder_, context_info_.namedValues)
# -> resolveOperand(X)
$count = 0
while ($c.Contains("resolveOsOperand(")) {
    $idx = $c.IndexOf("resolveOsOperand(")
    # Find the first argument end - it's the first comma after the opening paren, but we need to handle nested parens
    $parenStart = $idx + "resolveOsOperand".Length
    $firstArg = ""
    $depth = 0
    $pos = $parenStart
    $argEnd = -1
    for ($i = $pos; $i -lt $c.Length; $i++) {
        $ch = $c[$i]
        if ($ch -eq '(') { $depth++ }
        elseif ($ch -eq ')') { 
            $depth--
            if ($depth -eq 0) {
                # No comma found - single arg (shouldn't happen)
                $argEnd = $i
                $firstArg = $c.Substring($parenStart + 1, $i - $parenStart - 1).Trim()
                break
            }
        }
        elseif ($ch -eq ',' -and $depth -eq 1) {
            # Found first comma at depth 1
            $firstArg = $c.Substring($parenStart + 1, $i - $parenStart - 1).Trim()
            # Now find the closing paren
            for ($j = $i + 1; $j -lt $c.Length; $j++) {
                if ($c[$j] -eq '(') { $depth++ }
                elseif ($c[$j] -eq ')') {
                    $depth--
                    if ($depth -eq 0) { $argEnd = $j; break }
                }
            }
            break
        }
    }
    
    if ($argEnd -lt 0) {
        Write-Output "ERROR: Could not parse resolveOsOperand call at pos $idx"
        break
    }
    
    $oldCall = $c.Substring($idx, $argEnd - $idx + 1)
    $newCall = "resolveOperand($firstArg)"
    $c = $c.Remove($idx, $oldCall.Length)
    $c = $c.Insert($idx, $newCall)
    $count++
}
Write-Output "Replaced $count resolveOsOperand calls"

# 3. Add resolveOperand implementation if not present
if ($c.IndexOf("LLVMCodeGen::resolveOperand") -lt 0) {
    # Insert before the first emit method (emitAdd or similar)
    $insertPoint = $c.IndexOf("llvm::Value* LLVMCodeGen::emitInlineAsm")
    if ($insertPoint -lt 0) {
        $insertPoint = $c.IndexOf("llvm::Value* LLVMCodeGen::emitPortWrite")
    }
    if ($insertPoint -lt 0) {
        $insertPoint = $c.IndexOf("// OS/Hardware Emit Methods")
        if ($insertPoint -gt 0) {
            $insertPoint = $c.LastIndexOf("`n", $insertPoint) + 1
        }
    }
    if ($insertPoint -lt 0) {
        Write-Output "ERROR: Cannot find insertion point for resolveOperand"
    } else {
        $resolveImpl = @'

// ============================================================================
// Operand Resolution
// ============================================================================

llvm::Value* LLVMCodeGen::resolveOperand(const SIROperand& operand) {
    switch (operand.type) {
        case SIROperandType::CONSTANT: {
            switch (operand.dataType) {
                case SIRType::I64:
                case SIRType::I32:
                case SIRType::I16:
                case SIRType::I8:
                case SIRType::BOOL:
                    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), operand.intValue, true);
                case SIRType::F64:
                case SIRType::F32:
                    return llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), operand.floatValue);
                case SIRType::STRING: {
                    return builder_->CreateGlobalStringPtr(operand.name, "str.const");
                }
                default:
                    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), operand.intValue, true);
            }
        }
        case SIROperandType::REGISTER: {
            auto it = context_info_.namedValues.find(operand.name);
            if (it != context_info_.namedValues.end()) {
                return it->second;
            }
            reportError("Undefined register: " + operand.name);
            return nullptr;
        }
        case SIROperandType::GLOBAL: {
            llvm::GlobalVariable* gv = module_->getGlobalVariable(operand.name);
            if (gv) {
                return builder_->CreateLoad(gv->getValueType(), gv, operand.name + ".val");
            }
            reportError("Undefined global: " + operand.name);
            return nullptr;
        }
        case SIROperandType::FUNCTION: {
            llvm::Function* fn = module_->getFunction(operand.name);
            if (fn) return fn;
            reportError("Undefined function: " + operand.name);
            return nullptr;
        }
        case SIROperandType::LABEL: {
            return nullptr;
        }
        default:
            reportError("Unknown operand type");
            return nullptr;
    }
}

'@
        $c = $c.Insert($insertPoint, $resolveImpl)
        Write-Output "Added resolveOperand implementation"
    }
}

[System.IO.File]::WriteAllText($path, $c, $enc)
$bytes = [System.IO.File]::ReadAllBytes($path)
Write-Output "Done! File: $($bytes.Length) bytes, first 3: $($bytes[0]) $($bytes[1]) $($bytes[2])"
