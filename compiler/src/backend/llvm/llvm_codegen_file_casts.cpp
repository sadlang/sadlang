/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
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
#include <limits>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {
// ============================================================================
// (AR) عمليات الملفات وتحويل الأنواع
// (EN) File I/O operations and type casting (bitcast, inttoptr, etc.)
// (AR) تم فصله من llvm_codegen_array_file_coro.cpp وفق CW-05
// ============================================================================
        llvm::Value *LLVMCodeGen::emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                reportError("FILE_READ requires 1 operand (filename)");
                return nullptr;
            }
            llvm::Value *filename = resolveOperand(inst->operands[0]);
            if (!filename)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i64Ty = getInt64Type();

            // fopen(filename, "r")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = builder_->CreateGlobalStringPtr("r", "mode_r");
            llvm::Value *file = builder_->CreateCall(fopenFunc, {filename, mode}, "file");

            // Allocate read buffer (4096 bytes)
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = builder_->CreateCall(mallocFunc,
                                                    {llvm::ConstantInt::get(i64Ty, 4096)}, "read_buf");

            // fread(buf, 1, 4095, file)
            auto *freadType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
            auto freadFunc = module_->getOrInsertFunction("fread", freadType);
            llvm::Value *bytesRead = builder_->CreateCall(freadFunc, {buf, llvm::ConstantInt::get(i64Ty, 1), llvm::ConstantInt::get(i64Ty, 4095), file}, "bytes_read");

            // Null-terminate
            llvm::Value *endPtr = builder_->CreateGEP(
                llvm::Type::getInt8Ty(*context_), buf, {bytesRead}, "end");
            builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context_), 0), endPtr);

            // fclose(file)
            auto *fcloseType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*context_), {ptrTy}, false);
            auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
            builder_->CreateCall(fcloseFunc, {file});

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("FILE_WRITE requires 2 operands (filename, content)");
                return nullptr;
            }
            llvm::Value *filename = resolveOperand(inst->operands[0]);
            llvm::Value *content = resolveOperand(inst->operands[1]);
            if (!filename || !content)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);

            // fopen(filename, "w")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = builder_->CreateGlobalStringPtr("w", "mode_w");
            llvm::Value *file = builder_->CreateCall(fopenFunc, {filename, mode}, "file");

            // fputs(content, file)
            auto *fputsType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto fputsFunc = module_->getOrInsertFunction("fputs", fputsType);
            llvm::Value *result = builder_->CreateCall(fputsFunc, {content, file}, "fputs_result");

            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
            builder_->CreateCall(fcloseFunc, {file});

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                reportError("FILE_APPEND requires 2 operands (filename, content)");
                return nullptr;
            }
            llvm::Value *filename = resolveOperand(inst->operands[0]);
            llvm::Value *content = resolveOperand(inst->operands[1]);
            if (!filename || !content)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);

            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = builder_->CreateGlobalStringPtr("a", "mode_a");
            llvm::Value *file = builder_->CreateCall(fopenFunc, {filename, mode}, "file");

            auto *fputsType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto fputsFunc = module_->getOrInsertFunction("fputs", fputsType);
            llvm::Value *result = builder_->CreateCall(fputsFunc, {content, file}, "fputs_result");

            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
            builder_->CreateCall(fcloseFunc, {file});

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *filename = resolveOperand(inst->operands[0]);
            if (!filename)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);

            auto *removeType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto removeFunc = module_->getOrInsertFunction("remove", removeType);
            llvm::Value *result = builder_->CreateCall(removeFunc, {filename}, "remove_result");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst)
        {
            // Cross-platform file copy using fopen/fread/fwrite
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *src = resolveOperand(inst->operands[0]);
            llvm::Value *dst = resolveOperand(inst->operands[1]);
            if (!src || !dst)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i64Ty = getInt64Type();
            auto i32Ty = llvm::Type::getInt32Ty(*context_);
            auto i8Ty = llvm::Type::getInt8Ty(*context_);

            // fopen(src, "rb") and fopen(dst, "wb")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = module_->getOrInsertFunction("fopen", fopenType);

            llvm::Value *rb = builder_->CreateGlobalStringPtr("rb", "mode.rb");
            llvm::Value *wb = builder_->CreateGlobalStringPtr("wb", "mode.wb");
            llvm::Value *srcFile = builder_->CreateCall(fopenFunc, {src, rb}, "src.file");
            llvm::Value *dstFile = builder_->CreateCall(fopenFunc, {dst, wb}, "dst.file");

            // Check if both opened successfully
            llvm::Function *curFunc = builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *copyBB = llvm::BasicBlock::Create(*context_, "fcopy.copy", curFunc);
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*context_, "fcopy.fail", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*context_, "fcopy.done", curFunc);

            llvm::Value *srcNull = builder_->CreateICmpEQ(srcFile,
                                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));
            llvm::Value *dstNull = builder_->CreateICmpEQ(dstFile,
                                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));
            llvm::Value *anyNull = builder_->CreateOr(srcNull, dstNull);
            builder_->CreateCondBr(anyNull, failBB, copyBB);

            // Copy loop: read 4096 bytes at a time
            builder_->SetInsertPoint(copyBB);
            llvm::Value *bufSize = llvm::ConstantInt::get(i64Ty, 4096);
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = builder_->CreateCall(mallocFunc, {bufSize}, "copy.buf");

            auto *freadType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
            auto freadFunc = module_->getOrInsertFunction("fread", freadType);
            auto *fwriteType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
            auto fwriteFunc = module_->getOrInsertFunction("fwrite", fwriteType);
            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = module_->getOrInsertFunction("fclose", fcloseType);
            auto *freeType = llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), {ptrTy}, false);
            auto freeFunc = module_->getOrInsertFunction("free", freeType);

            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*context_, "fcopy.loop", curFunc);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*context_, "fcopy.body", curFunc);
            llvm::BasicBlock *loopDoneBB = llvm::BasicBlock::Create(*context_, "fcopy.ldone", curFunc);
            builder_->CreateBr(loopBB);

            builder_->SetInsertPoint(loopBB);
            llvm::Value *bytesRead = builder_->CreateCall(freadFunc, {buf, llvm::ConstantInt::get(i64Ty, 1), bufSize, srcFile}, "bytes.read");
            llvm::Value *hasData = builder_->CreateICmpUGT(bytesRead, llvm::ConstantInt::get(i64Ty, 0));
            builder_->CreateCondBr(hasData, loopBodyBB, loopDoneBB);

            builder_->SetInsertPoint(loopBodyBB);
            builder_->CreateCall(fwriteFunc, {buf, llvm::ConstantInt::get(i64Ty, 1), bytesRead, dstFile});
            builder_->CreateBr(loopBB);

            builder_->SetInsertPoint(loopDoneBB);
            builder_->CreateCall(fcloseFunc, {srcFile});
            builder_->CreateCall(fcloseFunc, {dstFile});
            builder_->CreateCall(freeFunc, {buf});
            builder_->CreateBr(doneBB);

            builder_->SetInsertPoint(failBB);
            builder_->CreateBr(doneBB);

            builder_->SetInsertPoint(doneBB);
            llvm::PHINode *result = builder_->CreatePHI(i32Ty, 2, "fcopy.result");
            result->addIncoming(llvm::ConstantInt::get(i32Ty, 1), loopDoneBB); // success
            result->addIncoming(llvm::ConstantInt::get(i32Ty, 0), failBB);     // failure

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *src = resolveOperand(inst->operands[0]);
            llvm::Value *dst = resolveOperand(inst->operands[1]);
            if (!src || !dst)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);

            auto *renameType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto renameFunc = module_->getOrInsertFunction("rename", renameType);
            llvm::Value *result = builder_->CreateCall(renameFunc, {src, dst}, "rename_result");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            auto i32Ty = llvm::Type::getInt32Ty(*context_);

            // Use sad_file_create_dir runtime helper (cross-platform)
            // On Windows: _mkdir(path), On Linux: mkdir(path, 0755)
            auto *mkdirType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto mkdirFunc = module_->getOrInsertFunction("sad_file_create_dir", mkdirType);
            llvm::Value *result = builder_->CreateCall(mkdirFunc, {path}, "mkdir_result");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst)
        {
            // List directory contents using runtime helper
            // sad_file_list_dir(path) -> SadArray of strings
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);

            // Call runtime helper: char* sad_file_list_dir(const char* path)
            // Returns newline-separated list of directory entries
            auto *listDirType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto listDirFunc = module_->getOrInsertFunction("sad_file_list_dir", listDirType);
            llvm::Value *result = builder_->CreateCall(listDirFunc, {path}, "listdir.ret");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Dead Declaration Implementations / تنفيذ الإعلانات المعلقة
        // ============================================================================

        llvm::Value *LLVMCodeGen::emitBitCast(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // In LLVM opaque pointers era, bitcast between pointers is identity
            // For ptr->ptr, just return the value. For other types, use CreateBitCast.
            llvm::Type *destTy = val->getType(); // default: same type
            if (inst->operands.size() >= 2)
            {
                // If second operand specifies the target type name, use ptr
                destTy = llvm::PointerType::getUnqual(*context_);
            }

            llvm::Value *result = val;
            if (val->getType() != destTy)
            {
                result = builder_->CreateBitCast(val, destTy, "bitcast");
            }

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitIntToPtr(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*context_);
            llvm::Value *result = builder_->CreateIntToPtr(val, ptrTy, "inttoptr");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitPtrToInt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            auto i64Ty = getInt64Type();
            llvm::Value *result = builder_->CreatePtrToInt(val, i64Ty, "ptrtoint");

            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitTrunc(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Default: truncate to i32
            llvm::Type *destTy = llvm::Type::getInt32Ty(*context_);
            if (inst->operands.size() >= 2)
            {
                // Check for target bit width in metadata
                auto &meta = inst->operands[1];
                if (meta.name == "i8")
                    destTy = llvm::Type::getInt8Ty(*context_);
                else if (meta.name == "i16")
                    destTy = llvm::Type::getInt16Ty(*context_);
                else if (meta.name == "i1")
                    destTy = llvm::Type::getInt1Ty(*context_);
            }

            llvm::Value *result = builder_->CreateTrunc(val, destTy, "trunc");
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitZExt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Default: extend to i64
            llvm::Type *destTy = getInt64Type();
            if (inst->operands.size() >= 2)
            {
                auto &meta = inst->operands[1];
                if (meta.name == "i32")
                    destTy = llvm::Type::getInt32Ty(*context_);
                else if (meta.name == "i16")
                    destTy = llvm::Type::getInt16Ty(*context_);
            }

            llvm::Value *result = builder_->CreateZExt(val, destTy, "zext");
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *LLVMCodeGen::emitSExt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *val = resolveOperand(inst->operands[0]);
            if (!val)
                return nullptr;

            // Default: extend to i64
            llvm::Type *destTy = getInt64Type();
            if (inst->operands.size() >= 2)
            {
                auto &meta = inst->operands[1];
                if (meta.name == "i32")
                    destTy = llvm::Type::getInt32Ty(*context_);
                else if (meta.name == "i16")
                    destTy = llvm::Type::getInt16Ty(*context_);
            }

            llvm::Value *result = builder_->CreateSExt(val, destTy, "sext");
            if (inst->result.has_value())
            {
                context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // LLVM Coroutine Support / دعم كوروتينات LLVM
        // ============================================================================
        // (AR) تنفيذ غير_متزامن/انتظر عبر LLVM Coroutine Intrinsics
        // (EN) Implement async/await via LLVM Coroutine Intrinsics
        //
        // البنية / Structure:
        //   غير_متزامن دالة foo(params) → LLVM coroutine:
        //     entry:
        //       %promise = alloca i64 (return value storage)
        //       %coro.id = call @llvm.coro.id(0, %promise, null, null)
        //       %coro.size = call @llvm.coro.size.i64()
        //       %coro.alloc = call @malloc(%coro.size)
        //       %coro.hdl = call @llvm.coro.begin(%coro.id, %coro.alloc)
        //       ... function body ...
        //       store %retval, %promise
        //       br %coro.final
        //     coro.final:
        //       %final.save = call @llvm.coro.save(%coro.hdl)
        //       %final.susp = call @llvm.coro.suspend(%final.save, true)
        //       switch %final.susp [0→suspend, 1→cleanup, default→suspend]
        //     coro.cleanup:
        //       %mem = call @llvm.coro.free(%coro.id, %coro.hdl)
        //       call @free(%mem)
        //       br %coro.suspend
        //     coro.suspend:
        //       call @llvm.coro.end(%coro.hdl, false, none)
        //       ret ptr %coro.hdl
        // ============================================================================

    } // namespace LLVM
} // namespace Sad