/*
 * ============================================================================
 * مولد كود LLVM IR - ملف التنفيذ
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/platform/file_casts_codegen.h"
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
        llvm::Value *FileCastsCodeGen::emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "FILE_READ"}});
                return nullptr;
            }
            llvm::Value *filename = cg_.resolveOperand(inst->operands[0]);
            if (!filename)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            // fopen(filename, "r")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = cg_.module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = cg_.builder_->CreateGlobalStringPtr("r", "mode_r");
            llvm::Value *file = cg_.builder_->CreateCall(fopenFunc, {filename, mode}, "file");

            // Allocate read buffer (4096 bytes)
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc,
                                                    {llvm::ConstantInt::get(i64Ty, 4096)}, "read_buf");

            // fread(buf, 1, 4095, file)
            auto *freadType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
            auto freadFunc = cg_.module_->getOrInsertFunction("fread", freadType);
            llvm::Value *bytesRead = cg_.builder_->CreateCall(freadFunc, {buf, llvm::ConstantInt::get(i64Ty, 1), llvm::ConstantInt::get(i64Ty, 4095), file}, "bytes_read");

            // Null-terminate
            llvm::Value *endPtr = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), buf, {bytesRead}, "end");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*cg_.context_), 0), endPtr);

            // fclose(file)
            auto *fcloseType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_), {ptrTy}, false);
            auto fcloseFunc = cg_.module_->getOrInsertFunction("fclose", fcloseType);
            cg_.builder_->CreateCall(fcloseFunc, {file});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "FILE_WRITE"}});
                return nullptr;
            }
            llvm::Value *filename = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *content = cg_.resolveOperand(inst->operands[1]);
            if (!filename || !content)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // fopen(filename, "w")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = cg_.module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = cg_.builder_->CreateGlobalStringPtr("w", "mode_w");
            llvm::Value *file = cg_.builder_->CreateCall(fopenFunc, {filename, mode}, "file");

            // fputs(content, file)
            auto *fputsType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto fputsFunc = cg_.module_->getOrInsertFunction("fputs", fputsType);
            llvm::Value *result = cg_.builder_->CreateCall(fputsFunc, {content, file}, "fputs_result");

            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = cg_.module_->getOrInsertFunction("fclose", fcloseType);
            cg_.builder_->CreateCall(fcloseFunc, {file});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "FILE_APPEND"}});
                return nullptr;
            }
            llvm::Value *filename = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *content = cg_.resolveOperand(inst->operands[1]);
            if (!filename || !content)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = cg_.module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = cg_.builder_->CreateGlobalStringPtr("a", "mode_a");
            llvm::Value *file = cg_.builder_->CreateCall(fopenFunc, {filename, mode}, "file");

            auto *fputsType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto fputsFunc = cg_.module_->getOrInsertFunction("fputs", fputsType);
            llvm::Value *result = cg_.builder_->CreateCall(fputsFunc, {content, file}, "fputs_result");

            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = cg_.module_->getOrInsertFunction("fclose", fcloseType);
            cg_.builder_->CreateCall(fcloseFunc, {file});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *filename = cg_.resolveOperand(inst->operands[0]);
            if (!filename)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            auto *removeType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto removeFunc = cg_.module_->getOrInsertFunction("remove", removeType);
            llvm::Value *result = cg_.builder_->CreateCall(removeFunc, {filename}, "remove_result");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst)
        {
            // Cross-platform file copy using fopen/fread/fwrite
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *src = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *dst = cg_.resolveOperand(inst->operands[1]);
            if (!src || !dst)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

            // fopen(src, "rb") and fopen(dst, "wb")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = cg_.module_->getOrInsertFunction("fopen", fopenType);

            llvm::Value *rb = cg_.builder_->CreateGlobalStringPtr("rb", "mode.rb");
            llvm::Value *wb = cg_.builder_->CreateGlobalStringPtr("wb", "mode.wb");
            llvm::Value *srcFile = cg_.builder_->CreateCall(fopenFunc, {src, rb}, "src.file");
            llvm::Value *dstFile = cg_.builder_->CreateCall(fopenFunc, {dst, wb}, "dst.file");

            // Check if both opened successfully
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *copyBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.copy", curFunc);
            llvm::BasicBlock *failBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.fail", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.done", curFunc);

            llvm::Value *srcNull = cg_.builder_->CreateICmpEQ(srcFile,
                                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));
            llvm::Value *dstNull = cg_.builder_->CreateICmpEQ(dstFile,
                                                          llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)));
            llvm::Value *anyNull = cg_.builder_->CreateOr(srcNull, dstNull);
            cg_.builder_->CreateCondBr(anyNull, failBB, copyBB);

            // Copy loop: read 4096 bytes at a time
            cg_.builder_->SetInsertPoint(copyBB);
            llvm::Value *bufSize = llvm::ConstantInt::get(i64Ty, 4096);
            auto *mallocType = llvm::FunctionType::get(ptrTy, {i64Ty}, false);
            auto mallocFunc = cg_.module_->getOrInsertFunction("malloc", mallocType);
            llvm::Value *buf = cg_.builder_->CreateCall(mallocFunc, {bufSize}, "copy.buf");

            auto *freadType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
            auto freadFunc = cg_.module_->getOrInsertFunction("fread", freadType);
            auto *fwriteType = llvm::FunctionType::get(i64Ty, {ptrTy, i64Ty, i64Ty, ptrTy}, false);
            auto fwriteFunc = cg_.module_->getOrInsertFunction("fwrite", fwriteType);
            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = cg_.module_->getOrInsertFunction("fclose", fcloseType);
            auto *freeType = llvm::FunctionType::get(llvm::Type::getVoidTy(*cg_.context_), {ptrTy}, false);
            auto freeFunc = cg_.module_->getOrInsertFunction("free", freeType);

            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.loop", curFunc);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.body", curFunc);
            llvm::BasicBlock *loopDoneBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.ldone", curFunc);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::Value *bytesRead = cg_.builder_->CreateCall(freadFunc, {buf, llvm::ConstantInt::get(i64Ty, 1), bufSize, srcFile}, "bytes.read");
            llvm::Value *hasData = cg_.builder_->CreateICmpUGT(bytesRead, llvm::ConstantInt::get(i64Ty, 0));
            cg_.builder_->CreateCondBr(hasData, loopBodyBB, loopDoneBB);

            cg_.builder_->SetInsertPoint(loopBodyBB);
            cg_.builder_->CreateCall(fwriteFunc, {buf, llvm::ConstantInt::get(i64Ty, 1), bytesRead, dstFile});
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopDoneBB);
            cg_.builder_->CreateCall(fcloseFunc, {srcFile});
            cg_.builder_->CreateCall(fcloseFunc, {dstFile});
            cg_.builder_->CreateCall(freeFunc, {buf});
            cg_.builder_->CreateBr(doneBB);

            cg_.builder_->SetInsertPoint(failBB);
            cg_.builder_->CreateBr(doneBB);

            cg_.builder_->SetInsertPoint(doneBB);
            llvm::PHINode *result = cg_.builder_->CreatePHI(i32Ty, 2, "fcopy.result");
            result->addIncoming(llvm::ConstantInt::get(i32Ty, 1), loopDoneBB); // success
            result->addIncoming(llvm::ConstantInt::get(i32Ty, 0), failBB);     // failure

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *src = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *dst = cg_.resolveOperand(inst->operands[1]);
            if (!src || !dst)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            auto *renameType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto renameFunc = cg_.module_->getOrInsertFunction("rename", renameType);
            llvm::Value *result = cg_.builder_->CreateCall(renameFunc, {src, dst}, "rename_result");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = cg_.resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // Use sad_file_create_dir runtime helper (cross-platform)
            // On Windows: _mkdir(path), On Linux: mkdir(path, 0755)
            auto *mkdirType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto mkdirFunc = cg_.module_->getOrInsertFunction("sad_file_create_dir", mkdirType);
            llvm::Value *result = cg_.builder_->CreateCall(mkdirFunc, {path}, "mkdir_result");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst)
        {
            // List directory contents using runtime helper
            // sad_file_list_dir(path) -> SadArray of strings
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = cg_.resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // Call runtime helper: char* sad_file_list_dir(const char* path)
            // Returns newline-separated list of directory entries
            auto *listDirType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto listDirFunc = cg_.module_->getOrInsertFunction("sad_file_list_dir", listDirType);
            llvm::Value *result = cg_.builder_->CreateCall(listDirFunc, {path}, "listdir.ret");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Dead Declaration Implementations / تنفيذ الإعلانات المعلقة
        // ============================================================================

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