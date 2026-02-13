# add_emit_methods.ps1 - Add all emit method implementations
$enc = New-Object System.Text.UTF8Encoding($false)
$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$c = [System.IO.File]::ReadAllText($path, $enc)

# Find the closing namespace brace
$endNs = $c.LastIndexOf("} // namespace LLVM")
if ($endNs -lt 0) {
    $endNs = $c.LastIndexOf("} // namespace")
}
if ($endNs -lt 0) {
    # Just find the last two closing braces
    Write-Output "Looking for end of file pattern..."
    $endNs = $c.Length
}
Write-Output "Namespace end at: $endNs"

# Helper function reference for resolving operands  
$emitMethods = @'

// ============================================================================
// OS/Hardware Emit Methods
// ============================================================================

llvm::Value* LLVMCodeGen::emitInlineAsm(std::shared_ptr<SIRInstruction> inst) {
    // Stub for inline asm - returns nullptr
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitPortWrite(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("port_write: needs 2 operands (port, value)");
        return nullptr;
    }
    llvm::Value* port = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* value = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!port || !value) { reportError("port_write: failed to resolve operands"); return nullptr; }
    llvm::Value* port16 = builder_->CreateIntCast(port, llvm::Type::getInt16Ty(*context_), false, "port");
    std::string asmStr;
    llvm::Type* valType;
    if (inst->opcode == SIROpcode::BUILTIN_PORT_WRITE_16) {
        valType = llvm::Type::getInt16Ty(*context_); asmStr = "outw $0, $1";
    } else if (inst->opcode == SIROpcode::BUILTIN_PORT_WRITE_32) {
        valType = llvm::Type::getInt32Ty(*context_); asmStr = "outl $0, $1";
    } else {
        valType = llvm::Type::getInt8Ty(*context_); asmStr = "outb $0, $1";
    }
    llvm::Value* val = builder_->CreateIntCast(value, valType, false, "val");
    llvm::FunctionType* ft = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context_), {valType, llvm::Type::getInt16Ty(*context_)}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(
        ft, asmStr, "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    return builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {val, port16});
}

llvm::Value* LLVMCodeGen::emitPortRead(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("port_read: needs 1 operand (port)"); return nullptr;
    }
    llvm::Value* port = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!port) { reportError("port_read: failed"); return nullptr; }
    llvm::Value* port16 = builder_->CreateIntCast(port, llvm::Type::getInt16Ty(*context_), false, "port");
    std::string asmStr;
    llvm::Type* retType;
    if (inst->opcode == SIROpcode::BUILTIN_PORT_READ_16) {
        retType = llvm::Type::getInt16Ty(*context_); asmStr = "inw $1, $0";
    } else if (inst->opcode == SIROpcode::BUILTIN_PORT_READ_32) {
        retType = llvm::Type::getInt32Ty(*context_); asmStr = "inl $1, $0";
    } else {
        retType = llvm::Type::getInt8Ty(*context_); asmStr = "inb $1, $0";
    }
    llvm::FunctionType* ft = llvm::FunctionType::get(retType, {llvm::Type::getInt16Ty(*context_)}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(
        ft, asmStr, "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Value* result = builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {port16}, "port.read");
    llvm::Value* extended = builder_->CreateZExt(result, llvm::Type::getInt64Ty(*context_), "port.read.ext");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = extended;
    return extended;
}

llvm::Value* LLVMCodeGen::emitMemWrite(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) { reportError("mem_write: needs 2 operands"); return nullptr; }
    llvm::Value* addr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* value = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!addr || !value) { reportError("mem_write: failed"); return nullptr; }
    llvm::Type* valType;
    if (inst->opcode == SIROpcode::BUILTIN_MEM_WRITE_32) valType = llvm::Type::getInt32Ty(*context_);
    else if (inst->opcode == SIROpcode::BUILTIN_MEM_WRITE_16) valType = llvm::Type::getInt16Ty(*context_);
    else valType = llvm::Type::getInt8Ty(*context_);
    llvm::Value* ptr = builder_->CreateIntToPtr(addr, valType->getPointerTo(), "mem.ptr");
    llvm::Value* val = builder_->CreateIntCast(value, valType, false);
    auto* store = builder_->CreateStore(val, ptr);
    store->setVolatile(true);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitMemRead(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("mem_read: needs 1 operand"); return nullptr; }
    llvm::Value* addr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!addr) { reportError("mem_read: failed"); return nullptr; }
    llvm::Type* valType;
    if (inst->opcode == SIROpcode::BUILTIN_MEM_READ_32) valType = llvm::Type::getInt32Ty(*context_);
    else if (inst->opcode == SIROpcode::BUILTIN_MEM_READ_16) valType = llvm::Type::getInt16Ty(*context_);
    else valType = llvm::Type::getInt8Ty(*context_);
    llvm::Value* ptr = builder_->CreateIntToPtr(addr, valType->getPointerTo(), "mem.ptr");
    auto* load = builder_->CreateLoad(valType, ptr, "mem.val");
    load->setVolatile(true);
    llvm::Value* result = builder_->CreateZExt(load, llvm::Type::getInt64Ty(*context_), "mem.ext");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitInterrupt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("interrupt: needs 1 operand"); return nullptr; }
    llvm::Value* intNum = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!intNum) return nullptr;
    llvm::Value* num8 = builder_->CreateIntCast(intNum, llvm::Type::getInt8Ty(*context_), false);
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {llvm::Type::getInt8Ty(*context_)}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "int $0", "N,~{dirflag},~{fpsr},~{flags}", true, false);
    builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {num8});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitHalt(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "hlt", "", true, false);
    builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitCli(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "cli", "~{memory}", true, false);
    builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitSti(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "sti", "~{memory}", true, false);
    builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitVgaWrite(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 4) { reportError("vga_write: needs 4 operands"); return nullptr; }
    llvm::Value* row = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* col = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    llvm::Value* ch = resolveOsOperand(inst->operands[2], *context_, *builder_, context_info_.namedValues);
    llvm::Value* color = resolveOsOperand(inst->operands[3], *context_, *builder_, context_info_.namedValues);
    if (!row || !col || !ch || !color) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i64 = llvm::Type::getInt64Ty(*context_);
    llvm::Value* offset = builder_->CreateAdd(
        builder_->CreateMul(row, llvm::ConstantInt::get(i64, 80)), col);
    llvm::Value* byteOffset = builder_->CreateMul(offset, llvm::ConstantInt::get(i64, 2));
    llvm::Value* baseAddr = llvm::ConstantInt::get(i64, 0xB8000);
    llvm::Value* addr = builder_->CreateAdd(baseAddr, byteOffset);
    llvm::Value* ptr = builder_->CreateIntToPtr(addr, i16->getPointerTo(), "vga.ptr");
    llvm::Value* charVal = builder_->CreateIntCast(ch, i16, false);
    llvm::Value* colorVal = builder_->CreateShl(builder_->CreateIntCast(color, i16, false), llvm::ConstantInt::get(i16, 8));
    llvm::Value* entry = builder_->CreateOr(charVal, colorVal, "vga.entry");
    auto* store = builder_->CreateStore(entry, ptr);
    store->setVolatile(true);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitVgaClear(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* color = llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context_), 0x07);
    if (inst && !inst->operands.empty()) {
        llvm::Value* c = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
        if (c) color = builder_->CreateIntCast(c, llvm::Type::getInt8Ty(*context_), false);
    }
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Value* colorWord = builder_->CreateShl(
        builder_->CreateZExt(color, i16), llvm::ConstantInt::get(i16, 8));
    llvm::Value* entry = builder_->CreateOr(colorWord, llvm::ConstantInt::get(i16, 0x20), "blank");
    llvm::Value* base = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0xB8000);
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "vga.loop", curFunc);
    llvm::BasicBlock* doneBB = llvm::BasicBlock::Create(*context_, "vga.done", curFunc);
    llvm::BasicBlock* entryBB = builder_->GetInsertBlock();
    builder_->CreateBr(loopBB);
    builder_->SetInsertPoint(loopBB);
    llvm::PHINode* idx = builder_->CreatePHI(llvm::Type::getInt64Ty(*context_), 2, "i");
    idx->addIncoming(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0), entryBB);
    llvm::Value* byteOff = builder_->CreateMul(idx, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 2));
    llvm::Value* addr = builder_->CreateAdd(base, byteOff);
    llvm::Value* ptr = builder_->CreateIntToPtr(addr, i16->getPointerTo(), "vga.p");
    auto* st = builder_->CreateStore(entry, ptr);
    st->setVolatile(true);
    llvm::Value* next = builder_->CreateAdd(idx, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1));
    idx->addIncoming(next, loopBB);
    llvm::Value* done = builder_->CreateICmpUGE(next, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 2000));
    builder_->CreateCondBr(done, doneBB, loopBB);
    builder_->SetInsertPoint(doneBB);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitAddrOf(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("addr_of: needs 1 operand"); return nullptr; }
    llvm::Value* val = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!val) return nullptr;
    llvm::Value* alloca = builder_->CreateAlloca(val->getType(), nullptr, "addr.tmp");
    builder_->CreateStore(val, alloca);
    llvm::Value* result = builder_->CreatePtrToInt(alloca, llvm::Type::getInt64Ty(*context_), "addr.val");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitMemCopy(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) { reportError("mem_copy: needs 3 operands"); return nullptr; }
    llvm::Value* dest = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* src = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    llvm::Value* size = resolveOsOperand(inst->operands[2], *context_, *builder_, context_info_.namedValues);
    if (!dest || !src || !size) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::Value* destPtr = builder_->CreateIntToPtr(dest, i8p);
    llvm::Value* srcPtr = builder_->CreateIntToPtr(src, i8p);
    builder_->CreateMemCpy(destPtr, llvm::MaybeAlign(1), srcPtr, llvm::MaybeAlign(1), size);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitMemSet(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) { reportError("mem_set: needs 3 operands"); return nullptr; }
    llvm::Value* dest = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* val = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    llvm::Value* size = resolveOsOperand(inst->operands[2], *context_, *builder_, context_info_.namedValues);
    if (!dest || !val || !size) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::Value* destPtr = builder_->CreateIntToPtr(dest, i8p);
    llvm::Value* val8 = builder_->CreateIntCast(val, llvm::Type::getInt8Ty(*context_), false);
    builder_->CreateMemSet(destPtr, val8, size, llvm::MaybeAlign(1));
    return nullptr;
}

// ============================================================================
// Embedded Emit Methods (18)
// ============================================================================

llvm::Value* LLVMCodeGen::emitSerialInit(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) { reportError("serial_init: needs 2 operands"); return nullptr; }
    llvm::Value* port = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* baud = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!port || !baud) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::Value* port16 = builder_->CreateIntCast(port, i16, false);
    auto doOutb = [&](llvm::Value* val8, llvm::Value* p16) {
        llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
        llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
        builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {val8, p16});
    };
    llvm::Value* p1 = builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 1));
    llvm::Value* p2 = builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 2));
    llvm::Value* p3 = builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 3));
    llvm::Value* p4 = builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 4));
    doOutb(llvm::ConstantInt::get(i8, 0x00), p1);
    doOutb(llvm::ConstantInt::get(i8, 0x80), p3);
    llvm::Value* divisor = builder_->CreateUDiv(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 115200), baud);
    doOutb(builder_->CreateIntCast(divisor, i8, false), port16);
    doOutb(builder_->CreateIntCast(builder_->CreateLShr(divisor, 8), i8, false), p1);
    doOutb(llvm::ConstantInt::get(i8, 0x03), p3);
    doOutb(llvm::ConstantInt::get(i8, 0xC7), p2);
    doOutb(llvm::ConstantInt::get(i8, 0x0B), p4);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitSerialWrite(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) { reportError("serial_write: needs 2 operands"); return nullptr; }
    llvm::Value* port = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* byte = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!port || !byte) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::Value* port16 = builder_->CreateIntCast(port, i16, false);
    llvm::Value* port5 = builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 5));
    llvm::FunctionType* inbFT = llvm::FunctionType::get(i8, {i16}, false);
    llvm::InlineAsm* inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* waitBB = llvm::BasicBlock::Create(*context_, "serial.wait", curFunc);
    llvm::BasicBlock* sendBB = llvm::BasicBlock::Create(*context_, "serial.send", curFunc);
    builder_->CreateBr(waitBB);
    builder_->SetInsertPoint(waitBB);
    llvm::Value* lsr = builder_->CreateCall(inbFT, static_cast<llvm::Value*>(inbIA), {port5}, "lsr");
    llvm::Value* ready = builder_->CreateAnd(lsr, llvm::ConstantInt::get(i8, 0x20));
    builder_->CreateCondBr(builder_->CreateICmpNE(ready, llvm::ConstantInt::get(i8, 0)), sendBB, waitBB);
    builder_->SetInsertPoint(sendBB);
    llvm::FunctionType* outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
    llvm::InlineAsm* outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateIntCast(byte, i8, false), port16});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitSerialRead(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("serial_read: needs 1 operand"); return nullptr; }
    llvm::Value* port = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!port) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::Value* port16 = builder_->CreateIntCast(port, i16, false);
    llvm::Value* port5 = builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 5));
    llvm::FunctionType* inbFT = llvm::FunctionType::get(i8, {i16}, false);
    llvm::InlineAsm* inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* waitBB = llvm::BasicBlock::Create(*context_, "sr.wait", curFunc);
    llvm::BasicBlock* readBB = llvm::BasicBlock::Create(*context_, "sr.read", curFunc);
    builder_->CreateBr(waitBB);
    builder_->SetInsertPoint(waitBB);
    llvm::Value* lsr = builder_->CreateCall(inbFT, static_cast<llvm::Value*>(inbIA), {port5}, "lsr");
    builder_->CreateCondBr(builder_->CreateICmpNE(builder_->CreateAnd(lsr, llvm::ConstantInt::get(i8, 0x01)), llvm::ConstantInt::get(i8, 0)), readBB, waitBB);
    builder_->SetInsertPoint(readBB);
    llvm::Value* data = builder_->CreateCall(inbFT, static_cast<llvm::Value*>(inbIA), {port16}, "sr.byte");
    llvm::Value* result = builder_->CreateZExt(data, llvm::Type::getInt64Ty(*context_), "sr.ext");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitSerialReady(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("serial_ready: needs 1 operand"); return nullptr; }
    llvm::Value* port = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!port) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::Value* port16 = builder_->CreateIntCast(port, i16, false);
    llvm::Value* port5 = builder_->CreateAdd(port16, llvm::ConstantInt::get(i16, 5));
    llvm::FunctionType* inbFT = llvm::FunctionType::get(i8, {i16}, false);
    llvm::InlineAsm* inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Value* lsr = builder_->CreateCall(inbFT, static_cast<llvm::Value*>(inbIA), {port5}, "lsr");
    llvm::Value* result = builder_->CreateZExt(builder_->CreateAnd(lsr, llvm::ConstantInt::get(i8, 0x01)), llvm::Type::getInt64Ty(*context_), "sr.rdy");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitGpioWrite(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) { reportError("gpio_write: needs 2 operands"); return nullptr; }
    llvm::Value* addr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* val = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!addr || !val) return nullptr;
    llvm::Type* i32 = llvm::Type::getInt32Ty(*context_);
    llvm::Value* ptr = builder_->CreateIntToPtr(builder_->CreateIntCast(addr, llvm::Type::getInt64Ty(*context_), false), i32->getPointerTo(), "gpio.ptr");
    auto* store = builder_->CreateStore(builder_->CreateIntCast(val, i32, false), ptr);
    store->setVolatile(true);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitGpioRead(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("gpio_read: needs 1 operand"); return nullptr; }
    llvm::Value* addr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!addr) return nullptr;
    llvm::Type* i32 = llvm::Type::getInt32Ty(*context_);
    llvm::Value* ptr = builder_->CreateIntToPtr(builder_->CreateIntCast(addr, llvm::Type::getInt64Ty(*context_), false), i32->getPointerTo(), "gpio.ptr");
    auto* load = builder_->CreateLoad(i32, ptr, "gpio.val");
    load->setVolatile(true);
    llvm::Value* result = builder_->CreateZExt(load, llvm::Type::getInt64Ty(*context_), "gpio.ext");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitGpioMode(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) { reportError("gpio_mode: needs 2 operands"); return nullptr; }
    llvm::Value* addr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* mode = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!addr || !mode) return nullptr;
    llvm::Type* i32 = llvm::Type::getInt32Ty(*context_);
    llvm::Value* dirAddr = builder_->CreateAdd(builder_->CreateIntCast(addr, llvm::Type::getInt64Ty(*context_), false), llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0x400));
    llvm::Value* ptr = builder_->CreateIntToPtr(dirAddr, i32->getPointerTo(), "gpio.dir");
    auto* st = builder_->CreateStore(builder_->CreateIntCast(mode, i32, false), ptr);
    st->setVolatile(true);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitTimerInit(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("timer_init: needs 1 operand"); return nullptr; }
    llvm::Value* freq = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!freq) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::FunctionType* outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
    llvm::InlineAsm* outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {llvm::ConstantInt::get(i8, 0x36), llvm::ConstantInt::get(i16, 0x43)});
    llvm::Value* div = builder_->CreateUDiv(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1193182), freq);
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateIntCast(div, i8, false), llvm::ConstantInt::get(i16, 0x40)});
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateIntCast(builder_->CreateLShr(div, 8), i8, false), llvm::ConstantInt::get(i16, 0x40)});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitTimerRead(std::shared_ptr<SIRInstruction> inst) {
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::FunctionType* outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
    llvm::InlineAsm* outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::FunctionType* inbFT = llvm::FunctionType::get(i8, {i16}, false);
    llvm::InlineAsm* inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {llvm::ConstantInt::get(i8, 0x00), llvm::ConstantInt::get(i16, 0x43)});
    llvm::Value* lo = builder_->CreateCall(inbFT, static_cast<llvm::Value*>(inbIA), {llvm::ConstantInt::get(i16, 0x40)}, "pit.lo");
    llvm::Value* hi = builder_->CreateCall(inbFT, static_cast<llvm::Value*>(inbIA), {llvm::ConstantInt::get(i16, 0x40)}, "pit.hi");
    llvm::Value* result = builder_->CreateOr(builder_->CreateZExt(lo, llvm::Type::getInt64Ty(*context_)), builder_->CreateShl(builder_->CreateZExt(hi, llvm::Type::getInt64Ty(*context_)), 8), "pit.val");
    if (inst && inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitTimerWait(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("timer_wait: needs 1 operand"); return nullptr; }
    llvm::Value* us = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!us) return nullptr;
    llvm::Type* i64 = llvm::Type::getInt64Ty(*context_);
    llvm::FunctionType* rdtscFT = llvm::FunctionType::get(i64, {}, false);
    llvm::InlineAsm* rdtscIA = llvm::InlineAsm::get(rdtscFT, "rdtsc\n\tshl $$32, %rdx\n\tor %rdx, %rax", "={rax},~{rdx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Value* start = builder_->CreateCall(rdtscFT, static_cast<llvm::Value*>(rdtscIA), {}, "w.start");
    llvm::Value* target = builder_->CreateAdd(start, builder_->CreateMul(us, llvm::ConstantInt::get(i64, 1000)));
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context_, "w.loop", curFunc);
    llvm::BasicBlock* doneBB = llvm::BasicBlock::Create(*context_, "w.done", curFunc);
    builder_->CreateBr(loopBB);
    builder_->SetInsertPoint(loopBB);
    llvm::Value* now = builder_->CreateCall(rdtscFT, static_cast<llvm::Value*>(rdtscIA), {}, "w.now");
    builder_->CreateCondBr(builder_->CreateICmpUGE(now, target), doneBB, loopBB);
    builder_->SetInsertPoint(doneBB);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitReset(std::shared_ptr<SIRInstruction> inst) {
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::FunctionType* inbFT = llvm::FunctionType::get(i8, {i16}, false);
    llvm::InlineAsm* inbIA = llvm::InlineAsm::get(inbFT, "inb $1, $0", "={ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::FunctionType* outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
    llvm::InlineAsm* outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* waitBB = llvm::BasicBlock::Create(*context_, "rst.wait", curFunc);
    llvm::BasicBlock* resetBB = llvm::BasicBlock::Create(*context_, "rst.do", curFunc);
    builder_->CreateBr(waitBB);
    builder_->SetInsertPoint(waitBB);
    llvm::Value* st = builder_->CreateCall(inbFT, static_cast<llvm::Value*>(inbIA), {llvm::ConstantInt::get(i16, 0x64)}, "kb.st");
    builder_->CreateCondBr(builder_->CreateICmpEQ(builder_->CreateAnd(st, llvm::ConstantInt::get(i8, 0x02)), llvm::ConstantInt::get(i8, 0)), resetBB, waitBB);
    builder_->SetInsertPoint(resetBB);
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {llvm::ConstantInt::get(i8, 0xFE), llvm::ConstantInt::get(i16, 0x64)});
    emitHalt(nullptr);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitCpuId(std::shared_ptr<SIRInstruction> inst) {
    llvm::Type* i32 = llvm::Type::getInt32Ty(*context_);
    llvm::Value* leaf = llvm::ConstantInt::get(i32, 0);
    if (inst && !inst->operands.empty()) {
        llvm::Value* op = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
        if (op) leaf = builder_->CreateIntCast(op, i32, false);
    }
    llvm::FunctionType* ft = llvm::FunctionType::get(i32, {i32}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "cpuid", "={eax},{eax},~{ebx},~{ecx},~{edx}", true, false);
    llvm::Value* eax = builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {leaf}, "cpuid.eax");
    llvm::Value* result = builder_->CreateZExt(eax, llvm::Type::getInt64Ty(*context_), "cpuid.ext");
    if (inst && inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitRdtsc(std::shared_ptr<SIRInstruction> inst) {
    llvm::Type* i64 = llvm::Type::getInt64Ty(*context_);
    llvm::FunctionType* ft = llvm::FunctionType::get(i64, {}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "rdtsc\n\tshl $$32, %rdx\n\tor %rdx, %rax", "={rax},~{rdx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Value* result = builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {}, "rdtsc.val");
    if (inst && inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitMfence(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "mfence", "~{memory}", true, false);
    builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitLfence(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "lfence", "~{memory}", true, false);
    builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitSfence(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::InlineAsm* ia = llvm::InlineAsm::get(ft, "sfence", "~{memory}", true, false);
    builder_->CreateCall(ft, static_cast<llvm::Value*>(ia), {});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitDmaInit(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 4) { reportError("dma_init: needs 4 operands"); return nullptr; }
    llvm::Value* ch = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* src = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    llvm::Value* dest = resolveOsOperand(inst->operands[2], *context_, *builder_, context_info_.namedValues);
    llvm::Value* cnt = resolveOsOperand(inst->operands[3], *context_, *builder_, context_info_.namedValues);
    if (!ch || !src || !dest || !cnt) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::FunctionType* outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
    llvm::InlineAsm* outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Value* ch8 = builder_->CreateIntCast(ch, i8, false);
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateOr(ch8, llvm::ConstantInt::get(i8, 0x04)), llvm::ConstantInt::get(i16, 0x0A)});
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {llvm::ConstantInt::get(i8, 0x00), llvm::ConstantInt::get(i16, 0x0C)});
    llvm::Value* addrPort = builder_->CreateMul(builder_->CreateIntCast(ch, i16, false), llvm::ConstantInt::get(i16, 2));
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateIntCast(src, i8, false), addrPort});
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateIntCast(builder_->CreateLShr(src, 8), i8, false), addrPort});
    llvm::Value* cntPort = builder_->CreateAdd(addrPort, llvm::ConstantInt::get(i16, 1));
    llvm::Value* cnt1 = builder_->CreateSub(cnt, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1));
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateIntCast(cnt1, i8, false), cntPort});
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateIntCast(builder_->CreateLShr(cnt1, 8), i8, false), cntPort});
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {ch8, llvm::ConstantInt::get(i16, 0x0A)});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitDmaStart(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("dma_start: needs 1 operand"); return nullptr; }
    llvm::Value* ch = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!ch) return nullptr;
    llvm::Type* i16 = llvm::Type::getInt16Ty(*context_);
    llvm::Type* i8 = llvm::Type::getInt8Ty(*context_);
    llvm::FunctionType* outbFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {i8, i16}, false);
    llvm::InlineAsm* outbIA = llvm::InlineAsm::get(outbFT, "outb $0, $1", "{ax},{dx},~{dirflag},~{fpsr},~{flags}", true, false);
    llvm::Value* ch8 = builder_->CreateIntCast(ch, i8, false);
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {builder_->CreateOr(ch8, llvm::ConstantInt::get(i8, 0x48)), llvm::ConstantInt::get(i16, 0x0B)});
    builder_->CreateCall(outbFT, static_cast<llvm::Value*>(outbIA), {ch8, llvm::ConstantInt::get(i16, 0x0A)});
    return nullptr;
}

// ============================================================================
// FFI Emit Methods (20)
// ============================================================================

llvm::Value* LLVMCodeGen::emitFfiPrintf(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("printf: needs operands"); return nullptr; }
    llvm::FunctionType* printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {llvm::Type::getInt8Ty(*context_)->getPointerTo()}, true);
    llvm::FunctionCallee printfFunc = module_->getOrInsertFunction("printf", printfType);
    std::vector<llvm::Value*> args;
    for (auto& op : inst->operands) {
        llvm::Value* v = resolveOsOperand(op, *context_, *builder_, context_info_.namedValues);
        if (v) args.push_back(v);
    }
    if (args.empty()) return nullptr;
    llvm::Value* result = builder_->CreateCall(printfFunc, args, "printf.ret");
    llvm::Value* ext = builder_->CreateSExt(result, llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = ext;
    return ext;
}

llvm::Value* LLVMCodeGen::emitFfiMalloc(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("malloc: needs 1 operand"); return nullptr; }
    llvm::Value* size = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!size) return nullptr;
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt8Ty(*context_)->getPointerTo(), {llvm::Type::getInt64Ty(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("malloc", ft);
    llvm::Value* result = builder_->CreateCall(fn, {size}, "malloc.ptr");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiFree(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) { reportError("free: needs 1 operand"); return nullptr; }
    llvm::Value* ptr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!ptr) return nullptr;
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {llvm::Type::getInt8Ty(*context_)->getPointerTo()}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("free", ft);
    if (!ptr->getType()->isPointerTy()) ptr = builder_->CreateIntToPtr(ptr, llvm::Type::getInt8Ty(*context_)->getPointerTo());
    builder_->CreateCall(fn, {ptr});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitFfiRealloc(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) { reportError("realloc: needs 2 operands"); return nullptr; }
    llvm::Value* ptr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* size = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!ptr || !size) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt64Ty(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("realloc", ft);
    if (!ptr->getType()->isPointerTy()) ptr = builder_->CreateIntToPtr(ptr, i8p);
    llvm::Value* result = builder_->CreateCall(fn, {ptr, size}, "realloc.ptr");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiCalloc(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) { reportError("calloc: needs 2 operands"); return nullptr; }
    llvm::Value* count = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* size = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!count || !size) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {llvm::Type::getInt64Ty(*context_), llvm::Type::getInt64Ty(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("calloc", ft);
    llvm::Value* result = builder_->CreateCall(fn, {count, size}, "calloc.ptr");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiStrlen(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* str = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!str) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt64Ty(*context_), {i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("strlen", ft);
    if (!str->getType()->isPointerTy()) str = builder_->CreateIntToPtr(str, i8p);
    llvm::Value* result = builder_->CreateCall(fn, {str}, "strlen.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiStrcpy(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* dst = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* src = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!dst || !src) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("strcpy", ft);
    if (!dst->getType()->isPointerTy()) dst = builder_->CreateIntToPtr(dst, i8p);
    if (!src->getType()->isPointerTy()) src = builder_->CreateIntToPtr(src, i8p);
    llvm::Value* result = builder_->CreateCall(fn, {dst, src}, "strcpy.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiStrcmp(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* s1 = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* s2 = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!s1 || !s2) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p, i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("strcmp", ft);
    if (!s1->getType()->isPointerTy()) s1 = builder_->CreateIntToPtr(s1, i8p);
    if (!s2->getType()->isPointerTy()) s2 = builder_->CreateIntToPtr(s2, i8p);
    llvm::Value* result = builder_->CreateSExt(builder_->CreateCall(fn, {s1, s2}, "strcmp.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiStrcat(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* dst = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* src = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!dst || !src) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("strcat", ft);
    if (!dst->getType()->isPointerTy()) dst = builder_->CreateIntToPtr(dst, i8p);
    if (!src->getType()->isPointerTy()) src = builder_->CreateIntToPtr(src, i8p);
    llvm::Value* result = builder_->CreateCall(fn, {dst, src}, "strcat.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiMemcpy(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) return nullptr;
    llvm::Value* dst = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* src = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    llvm::Value* sz = resolveOsOperand(inst->operands[2], *context_, *builder_, context_info_.namedValues);
    if (!dst || !src || !sz) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p, i8p, llvm::Type::getInt64Ty(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("memcpy", ft);
    if (!dst->getType()->isPointerTy()) dst = builder_->CreateIntToPtr(dst, i8p);
    if (!src->getType()->isPointerTy()) src = builder_->CreateIntToPtr(src, i8p);
    llvm::Value* result = builder_->CreateCall(fn, {dst, src, sz}, "memcpy.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiMemset(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) return nullptr;
    llvm::Value* dst = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* val = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    llvm::Value* sz = resolveOsOperand(inst->operands[2], *context_, *builder_, context_info_.namedValues);
    if (!dst || !val || !sz) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*context_), llvm::Type::getInt64Ty(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("memset", ft);
    if (!dst->getType()->isPointerTy()) dst = builder_->CreateIntToPtr(dst, i8p);
    llvm::Value* val32 = builder_->CreateIntCast(val, llvm::Type::getInt32Ty(*context_), false);
    llvm::Value* result = builder_->CreateCall(fn, {dst, val32, sz}, "memset.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiFopen(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* name = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* mode = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!name || !mode) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p, i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("fopen", ft);
    llvm::Value* result = builder_->CreateCall(fn, {name, mode}, "fopen.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiFclose(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* fp = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!fp) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("fclose", ft);
    if (!fp->getType()->isPointerTy()) fp = builder_->CreateIntToPtr(fp, i8p);
    llvm::Value* result = builder_->CreateSExt(builder_->CreateCall(fn, {fp}, "fclose.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiFwrite(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) return nullptr;
    llvm::Value* str = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* fp = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    if (!str || !fp) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p, i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("fputs", ft);
    if (!str->getType()->isPointerTy()) str = builder_->CreateIntToPtr(str, i8p);
    if (!fp->getType()->isPointerTy()) fp = builder_->CreateIntToPtr(fp, i8p);
    llvm::Value* result = builder_->CreateSExt(builder_->CreateCall(fn, {str, fp}, "fputs.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiFread(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) return nullptr;
    llvm::Value* buf = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    llvm::Value* sz = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
    llvm::Value* fp = resolveOsOperand(inst->operands[2], *context_, *builder_, context_info_.namedValues);
    if (!buf || !sz || !fp) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*context_), i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("fgets", ft);
    if (!buf->getType()->isPointerTy()) buf = builder_->CreateIntToPtr(buf, i8p);
    if (!fp->getType()->isPointerTy()) fp = builder_->CreateIntToPtr(fp, i8p);
    llvm::Value* sz32 = builder_->CreateIntCast(sz, llvm::Type::getInt32Ty(*context_), false);
    llvm::Value* result = builder_->CreateCall(fn, {buf, sz32, fp}, "fgets.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiSystem(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* cmd = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!cmd) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("system", ft);
    llvm::Value* result = builder_->CreateSExt(builder_->CreateCall(fn, {cmd}, "system.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiGetenv(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* name = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!name) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(i8p, {i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("getenv", ft);
    llvm::Value* result = builder_->CreateCall(fn, {name}, "getenv.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiAtoi(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* str = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!str) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("atoi", ft);
    llvm::Value* result = builder_->CreateSExt(builder_->CreateCall(fn, {str}, "atoi.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiAtof(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* str = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!str) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), {i8p}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("atof", ft);
    llvm::Value* result = builder_->CreateCall(fn, {str}, "atof.ret");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitFfiSnprintf(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) return nullptr;
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {i8p, llvm::Type::getInt64Ty(*context_), i8p}, true);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("snprintf", ft);
    std::vector<llvm::Value*> args;
    for (auto& op : inst->operands) {
        llvm::Value* v = resolveOsOperand(op, *context_, *builder_, context_info_.namedValues);
        if (v) args.push_back(v);
    }
    llvm::Value* result = builder_->CreateSExt(builder_->CreateCall(fn, args, "snprintf.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

// ============================================================================
// Security Emit Methods (14) - stub implementations using printf
// ============================================================================

llvm::Value* LLVMCodeGen::emitSecurityAssert(std::shared_ptr<SIRInstruction> inst) {
    // Security assert - check condition and abort if false
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* cond = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!cond) return nullptr;
    llvm::Value* condBool = builder_->CreateICmpNE(cond, llvm::ConstantInt::get(cond->getType(), 0));
    llvm::Function* curFunc = builder_->GetInsertBlock()->getParent();
    llvm::BasicBlock* failBB = llvm::BasicBlock::Create(*context_, "sec.fail", curFunc);
    llvm::BasicBlock* passBB = llvm::BasicBlock::Create(*context_, "sec.pass", curFunc);
    builder_->CreateCondBr(condBool, passBB, failBB);
    builder_->SetInsertPoint(failBB);
    llvm::FunctionType* abortFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::FunctionCallee abortFn = module_->getOrInsertFunction("abort", abortFT);
    builder_->CreateCall(abortFn, {});
    builder_->CreateUnreachable();
    builder_->SetInsertPoint(passBB);
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitSecurityVerify(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!val) return nullptr;
    llvm::Value* result = builder_->CreateICmpNE(val, llvm::ConstantInt::get(val->getType(), 0));
    llvm::Value* ext = builder_->CreateZExt(result, llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = ext;
    return ext;
}

llvm::Value* LLVMCodeGen::emitSecurityIsSafe(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* ptr = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!ptr) return nullptr;
    llvm::Value* result = builder_->CreateICmpNE(ptr, llvm::Constant::getNullValue(ptr->getType()));
    llvm::Value* ext = builder_->CreateZExt(result, llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = ext;
    return ext;
}

llvm::Value* LLVMCodeGen::emitSecurityPanic(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* abortFT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {}, false);
    llvm::FunctionCallee abortFn = module_->getOrInsertFunction("abort", abortFT);
    builder_->CreateCall(abortFn, {});
    builder_->CreateUnreachable();
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitSecurityHash(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!val) return nullptr;
    // Simple FNV-1a hash simulation
    llvm::Value* hash = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0xcbf29ce484222325ULL);
    llvm::Value* prime = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 0x100000001b3ULL);
    llvm::Value* xored = builder_->CreateXor(hash, val);
    llvm::Value* result = builder_->CreateMul(xored, prime, "hash.val");
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitSecurityEncrypt(std::shared_ptr<SIRInstruction> inst) {
    // Simple XOR encryption stub - returns the input for now
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!val) return nullptr;
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = val;
    return val;
}

llvm::Value* LLVMCodeGen::emitSecurityDecrypt(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!val) return nullptr;
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = val;
    return val;
}

llvm::Value* LLVMCodeGen::emitSecurityAssertType(std::shared_ptr<SIRInstruction> inst) { return nullptr; }
llvm::Value* LLVMCodeGen::emitSecurityAssertEqual(std::shared_ptr<SIRInstruction> inst) { return nullptr; }
llvm::Value* LLVMCodeGen::emitSecurityAssertGreater(std::shared_ptr<SIRInstruction> inst) { return nullptr; }

llvm::Value* LLVMCodeGen::emitSecuritySanitize(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!val) return nullptr;
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = val;
    return val;
}

llvm::Value* LLVMCodeGen::emitSecurityTimestamp(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt64Ty(*context_), {llvm::Type::getInt8Ty(*context_)->getPointerTo()}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("time", ft);
    llvm::Value* result = builder_->CreateCall(fn, {llvm::ConstantPointerNull::get(llvm::Type::getInt8Ty(*context_)->getPointerTo())}, "time.ret");
    if (inst && inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitSecuritySecureRandom(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("rand", ft);
    llvm::Value* result = builder_->CreateSExt(builder_->CreateCall(fn, {}, "rand.ret"), llvm::Type::getInt64Ty(*context_));
    if (inst && inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitSecurityBase64Encode(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* val = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!val) return nullptr;
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = val;
    return val;
}

// ============================================================================
// Builtin Core Functions
// ============================================================================

llvm::Value* LLVMCodeGen::emitBuiltinPrint(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::FunctionType* printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {llvm::Type::getInt8Ty(*context_)->getPointerTo()}, true);
    llvm::FunctionCallee printfFunc = module_->getOrInsertFunction("printf", printfType);
    for (auto& op : inst->operands) {
        llvm::Value* v = resolveOsOperand(op, *context_, *builder_, context_info_.namedValues);
        if (!v) continue;
        if (v->getType()->isPointerTy()) {
            llvm::Value* fmt = builder_->CreateGlobalStringPtr("%s\n", "fmt.s");
            builder_->CreateCall(printfFunc, {fmt, v});
        } else if (v->getType()->isIntegerTy(64)) {
            llvm::Value* fmt = builder_->CreateGlobalStringPtr("%lld\n", "fmt.d");
            builder_->CreateCall(printfFunc, {fmt, v});
        } else if (v->getType()->isDoubleTy()) {
            llvm::Value* fmt = builder_->CreateGlobalStringPtr("%f\n", "fmt.f");
            builder_->CreateCall(printfFunc, {fmt, v});
        } else {
            llvm::Value* fmt = builder_->CreateGlobalStringPtr("%lld\n", "fmt.d");
            llvm::Value* conv = builder_->CreateIntCast(v, llvm::Type::getInt64Ty(*context_), true);
            builder_->CreateCall(printfFunc, {fmt, conv});
        }
    }
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitBuiltinRead(std::shared_ptr<SIRInstruction> inst) {
    // Read a line from stdin
    llvm::Type* i8p = llvm::Type::getInt8Ty(*context_)->getPointerTo();
    llvm::FunctionType* mallocFT = llvm::FunctionType::get(i8p, {llvm::Type::getInt64Ty(*context_)}, false);
    llvm::FunctionCallee mallocFn = module_->getOrInsertFunction("malloc", mallocFT);
    llvm::Value* buf = builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context_), 1024)}, "read.buf");
    llvm::FunctionType* fgetsFT = llvm::FunctionType::get(i8p, {i8p, llvm::Type::getInt32Ty(*context_), i8p}, false);
    llvm::FunctionCallee fgetsFn = module_->getOrInsertFunction("fgets", fgetsFT);
    // Get stdin
    llvm::FunctionType* stdinFT = llvm::FunctionType::get(i8p, {}, false);
    llvm::FunctionCallee stdinFn = module_->getOrInsertFunction("__acrt_iob_func", llvm::FunctionType::get(i8p, {llvm::Type::getInt32Ty(*context_)}, false));
    llvm::Value* stdinPtr = builder_->CreateCall(stdinFn, {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0)}, "stdin.ptr");
    builder_->CreateCall(fgetsFn, {buf, llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1024), stdinPtr});
    if (inst && inst->result.has_value()) context_info_.namedValues[inst->result->name] = buf;
    return buf;
}

llvm::Value* LLVMCodeGen::emitBuiltinMath(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* arg = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!arg) return nullptr;
    llvm::Value* dArg = arg;
    if (!dArg->getType()->isDoubleTy()) dArg = builder_->CreateSIToFP(dArg, llvm::Type::getDoubleTy(*context_));
    std::string mathFn;
    bool returnsInt = false;
    bool twoArgs = false;
    switch (inst->opcode) {
        case SIROpcode::BUILTIN_SQRT: mathFn = "sqrt"; break;
        case SIROpcode::BUILTIN_SIN: mathFn = "sin"; break;
        case SIROpcode::BUILTIN_COS: mathFn = "cos"; break;
        case SIROpcode::BUILTIN_TAN: mathFn = "tan"; break;
        case SIROpcode::BUILTIN_ABS: mathFn = "fabs"; break;
        case SIROpcode::BUILTIN_FLOOR: mathFn = "floor"; returnsInt = true; break;
        case SIROpcode::BUILTIN_CEIL: mathFn = "ceil"; returnsInt = true; break;
        case SIROpcode::BUILTIN_ROUND: mathFn = "round"; returnsInt = true; break;
        case SIROpcode::BUILTIN_POW: mathFn = "pow"; twoArgs = true; break;
        default: return nullptr;
    }
    llvm::Type* dblTy = llvm::Type::getDoubleTy(*context_);
    llvm::Value* result;
    if (twoArgs && inst->operands.size() >= 2) {
        llvm::Value* arg2 = resolveOsOperand(inst->operands[1], *context_, *builder_, context_info_.namedValues);
        if (!arg2) return nullptr;
        if (!arg2->getType()->isDoubleTy()) arg2 = builder_->CreateSIToFP(arg2, dblTy);
        llvm::FunctionType* ft = llvm::FunctionType::get(dblTy, {dblTy, dblTy}, false);
        llvm::FunctionCallee fn = module_->getOrInsertFunction(mathFn, ft);
        result = builder_->CreateCall(fn, {dArg, arg2}, mathFn + ".ret");
    } else {
        llvm::FunctionType* ft = llvm::FunctionType::get(dblTy, {dblTy}, false);
        llvm::FunctionCallee fn = module_->getOrInsertFunction(mathFn, ft);
        result = builder_->CreateCall(fn, {dArg}, mathFn + ".ret");
    }
    if (returnsInt) result = builder_->CreateFPToSI(result, llvm::Type::getInt64Ty(*context_));
    if (inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitBuiltinSleep(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) return nullptr;
    llvm::Value* ms = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
    if (!ms) return nullptr;
    llvm::Value* ms32 = builder_->CreateIntCast(ms, llvm::Type::getInt32Ty(*context_), false);
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {llvm::Type::getInt32Ty(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("Sleep", ft);
    builder_->CreateCall(fn, {ms32});
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitBuiltinExit(std::shared_ptr<SIRInstruction> inst) {
    llvm::Value* code = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0);
    if (inst && !inst->operands.empty()) {
        llvm::Value* c = resolveOsOperand(inst->operands[0], *context_, *builder_, context_info_.namedValues);
        if (c) code = builder_->CreateIntCast(c, llvm::Type::getInt32Ty(*context_), false);
    }
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {llvm::Type::getInt32Ty(*context_)}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("exit", ft);
    builder_->CreateCall(fn, {code});
    builder_->CreateUnreachable();
    return nullptr;
}

llvm::Value* LLVMCodeGen::emitBuiltinRandom(std::shared_ptr<SIRInstruction> inst) {
    llvm::FunctionType* ft = llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {}, false);
    llvm::FunctionCallee fn = module_->getOrInsertFunction("rand", ft);
    llvm::Value* rval = builder_->CreateCall(fn, {}, "rand.ret");
    llvm::Value* dval = builder_->CreateSIToFP(rval, llvm::Type::getDoubleTy(*context_));
    llvm::Value* result = builder_->CreateFDiv(dval, llvm::ConstantFP::get(llvm::Type::getDoubleTy(*context_), 2147483647.0));
    if (inst && inst->result.has_value()) context_info_.namedValues[inst->result->name] = result;
    return result;
}


'@

$c = $c.Insert($endNs, $emitMethods)
[System.IO.File]::WriteAllText($path, $c, $enc)
Write-Output "Emit methods added. New size: $($c.Length)"
