#!/usr/bin/env python3
"""
إصلاح API في llvm_codegen.cpp
Fix API in llvm_codegen.cpp

Source: sir_instruction.h:60-62 - SIRInstruction has public members
Source: sir_types.h:293 - SIROperand has public member name
"""

import re

def fix_llvm_codegen():
    filepath = r'C:\s\s_language\compiler\backends\llvm\llvm_codegen.cpp'
    
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    original_content = content
    
    # Fix 1: inst->getOperand(N)->getName() → inst->operands[N].name
    # Pattern: inst->getOperand(N)->getName() where N is a digit or expression
    content = re.sub(
        r'inst->getOperand\((\d+)\)->getName\(\)',
        r'inst->operands[\1].name',
        content
    )
    
    # Fix 2: inst->getOperand(i)->getName() for loop variables
    content = re.sub(
        r'inst->getOperand\(i\)->getName\(\)',
        r'inst->operands[i].name',
        content
    )
    
    # Fix 3: inst->getOperand(expression)->getName() for complex expressions
    content = re.sub(
        r'inst->getOperand\(([^)]+)\)->getName\(\)',
        r'inst->operands[\1].name',
        content
    )
    
    # Fix 4: inst->getName() → inst->result->name
    # (for result names in LLVM IR generation)
    content = re.sub(
        r'inst->getName\(\)',
        r'inst->result->name',
        content
    )
    
    # Fix 5: inst->getType() → inst->result->dataType
    # (when we need the data type of the result)
    content = re.sub(
        r'convertType\(inst->getType\(\)\)',
        r'convertType(inst->result->dataType)',
        content
    )
    
    # Fix 6: inst->getNumOperands() → inst->operands.size()
    content = re.sub(
        r'inst->getNumOperands\(\)',
        r'inst->operands.size()',
        content
    )
    
    # Fix 7: auto operand = inst->getOperand(i) → auto& operand = inst->operands[i]
    content = re.sub(
        r'auto operand = inst->getOperand\(i\)',
        r'auto& operand = inst->operands[i]',
        content
    )
    
    if content != original_content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"✅ Successfully fixed {filepath}")
        print(f"   Applied fixes:")
        print(f"   - inst->getOperand(N)->getName() → inst->operands[N].name")
        print(f"   - inst->getName() → inst->result->name")
        print(f"   - inst->getType() → inst->result->dataType")
        print(f"   - inst->getNumOperands() → inst->operands.size()")
        return True
    else:
        print(f"⚠️  No changes needed in {filepath}")
        return False

if __name__ == '__main__':
    fix_llvm_codegen()
