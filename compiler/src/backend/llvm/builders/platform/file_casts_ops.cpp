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
#include "sad_dyn_repr.h"                                  // (AR) DynKind::Int لوسم homogKind / (EN) DynKind::Int for homogKind
#include "builders/collections/array_ops_codegen.h"       // (AR) SAD_ARRAY_SLOT_BYTES
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
            llvm::Value *buf = cg_.emitMalloc(llvm::ConstantInt::get(i64Ty, 4096), "read_buf");

            // fread(buf, 1, 4095, file)
            // (AR) ‎fread/fwrite(ptr, size_t, size_t, FILE*)‎ والعائد ‎size_t‎ —
            //      بـ i64 ثابتًا على 32-بت تُزاح خانة ‎FILE*‎ ويُقرأ العائد قمامةً.
            llvm::Type *szTy = cg_.getSizeType();
            auto *freadType = llvm::FunctionType::get(szTy, {ptrTy, szTy, szTy, ptrTy}, false);
            auto freadFunc = cg_.module_->getOrInsertFunction("fread", freadType);
            llvm::Value *bytesRead = cg_.builder_->CreateZExtOrTrunc(
                cg_.builder_->CreateCall(freadFunc, {buf, llvm::ConstantInt::get(szTy, 1),
                    llvm::ConstantInt::get(szTy, 4095), file}, "bytes_read.sz"),
                i64Ty, "bytes_read");

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

        // ====================================================================
        // (AR) نوعُ بنية SadArray الخماسيّ (نظيرُ getArrayStructType في array_ops.cpp).
        //      نُكرّره محلّيًّا لأنّ الأصل ساكنٌ ملفّيّ؛ التطابقُ في التخطيط لا الاسم
        //      (الوصول بفهرس الحقل). الحقول: {len, cap, data, tags, homogKind}.
        // (EN) The 5-field SadArray struct type (mirror of getArrayStructType in
        //      array_ops.cpp). Replicated locally because the original is file-static;
        //      only the field LAYOUT matters (GEP by index), not the name.
        // ====================================================================
        static llvm::StructType *getSadArrayType5(llvm::LLVMContext &ctx)
        {
            static llvm::StructType *arrTy = nullptr;
            if (!arrTy)
            {
                arrTy = llvm::StructType::create(ctx, {
                                                          llvm::Type::getInt64Ty(ctx),       // length
                                                          llvm::Type::getInt64Ty(ctx),       // capacity
                                                          llvm::PointerType::getUnqual(ctx), // data pointer
                                                          llvm::PointerType::getUnqual(ctx), // tags (i8*) or null
                                                          llvm::Type::getInt8Ty(ctx)         // homogKind
                                                      },
                                                 "SadArray.bytes");
            }
            return arrTy;
        }

        // ====================================================================
        // (AR) اكتب_بايتات(مسار، مصفوفة) — كتابة بايتات خام إلى ملف.
        //      يقرأ طولَ SadArray (الحقل ٠) ومؤشّرَ بياناته (الحقل ٢)، يبني مخزنَ
        //      بايتاتٍ منخفضَ-البايت لكلّ عنصرٍ (i64→i8، تقنيع 0xFF ضمنيّ في trunc)،
        //      ثمّ fwrite(buf,1,len,file) بالوضع الثنائيّ "wb" ⇒ يكتب البايتات الصفريّة
        //      (بخلاف fputs في اكتب_ملف). يطابق المفسّرَ بايتًا ببايت.
        // (EN) write_bytes(path, array) — write raw bytes to a file. Reads SadArray
        //      length (field 0) and data pointer (field 2), builds a low-byte buffer
        //      (i64→i8 trunc), then fwrite(buf,1,len,file) in binary "wb" mode ⇒ writes
        //      embedded NUL bytes (unlike fputs in write_file). Matches the interpreter.
        // ====================================================================
        llvm::Value *FileCastsCodeGen::emitBuiltinFileWriteBytes(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "FILE_WRITE_BYTES"}});
                return nullptr;
            }
            llvm::Value *filename = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *arrOperand = cg_.resolveOperand(inst->operands[1]);
            if (!filename || !arrOperand)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = cg_.getInt8Type();
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Type *szTy = cg_.getSizeType();
            llvm::StructType *arrTy = getSadArrayType5(*cg_.context_);

            // (AR) تطبيع مؤشّر المصفوفة (يشمل خانةَ الالتقاط ptr) / (EN) normalize array ptr
            llvm::Value *arrPtr = cg_.normalizeArrayPtr(arrOperand, "wb.arr");

            // (AR) الطول (الحقل ٠) ومؤشّر البيانات (الحقل ٢) والوسوم (الحقل ٣) ووسم التجانس (الحقل ٤)
            // (EN) length (field 0), data ptr (field 2), tags (field 3), homogKind (field 4)
            llvm::Value *len = cg_.builder_->CreateLoad(
                i64Ty, cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "wb.len.gep"), "wb.len");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(
                ptrTy, cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "wb.data.gep"), "wb.data");
            llvm::Value *tagsPtr = cg_.builder_->CreateLoad(
                ptrTy, cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "wb.tags.gep"), "wb.tags");
            llvm::Value *homogKind = cg_.builder_->CreateLoad(
                i8Ty, cg_.builder_->CreateStructGEP(arrTy, arrPtr, 4, "wb.homogkind.gep"), "wb.homogkind");
            // (AR) هل المصفوفة متجانسة؟ (tags == null) — ثابتٌ عبر اللولب
            // (EN) homogeneous iff tags == null — loop-invariant
            llvm::Value *tagsIsNull = cg_.builder_->CreateICmpEQ(
                tagsPtr, llvm::ConstantPointerNull::get(ptrTy), "wb.tags.isnull");

            // (AR) مخزنُ بايتاتٍ بطول len (بايت لكلّ عنصر) / (EN) byte buffer of len bytes
            llvm::Value *lenSz = cg_.builder_->CreateZExtOrTrunc(len, szTy, "wb.len.sz");
            llvm::Value *buf = cg_.emitMalloc(lenSz, "wb.buf");

            // (AR) لولب: buf[i] = (i8) toInt64(data[i]) — نُعيد بناءَ نوعِ العنصر زمنَ التشغيل
            //      (وسمُه = tags[i] إن مختلطة، أو homogKind إن متجانسة) ثمّ نحوّله عدديًّا
            //      عبر makeDyn+unpackI64 (عشريّ⇒fptosi(bitcast)، صحيح⇒الحمولة) مطابقةً
            //      لـtoInt64() في المفسّر. لا اقتطاعٌ أعمى لبتّات double.
            // (EN) loop: buf[i] = (i8) toInt64(data[i]) — reconstruct each element's runtime
            //      kind (tags[i] if heterogeneous, else homogKind) then convert numerically
            //      via makeDyn+unpackI64 (Float⇒fptosi(bitcast), Int⇒payload), matching the
            //      interpreter's toInt64(). No blind truncation of a double's bit-pattern.
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "wb.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "wb.body", curFunc);
            llvm::BasicBlock *kHomogBB = llvm::BasicBlock::Create(*cg_.context_, "wb.k.homog", curFunc);
            llvm::BasicBlock *kTagsBB = llvm::BasicBlock::Create(*cg_.context_, "wb.k.tags", curFunc);
            llvm::BasicBlock *kMergeBB = llvm::BasicBlock::Create(*cg_.context_, "wb.k.merge", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "wb.done", curFunc);
            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *iVal = cg_.builder_->CreatePHI(i64Ty, 2, "wb.i");
            iVal->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            llvm::Value *cond = cg_.builder_->CreateICmpSLT(iVal, len, "wb.cond");
            cg_.builder_->CreateCondBr(cond, bodyBB, doneBB);

            // (AR) الجسم: حمّل الحمولةَ الخام ثمّ تفرّع لتحديد الوسم (بلا قراءةِ tags[i] عند null)
            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *elem = cg_.builder_->CreateLoad(
                i64Ty, cg_.builder_->CreateGEP(i64Ty, dataPtr, {iVal}, "wb.elem.gep"), "wb.elem");
            cg_.builder_->CreateCondBr(tagsIsNull, kHomogBB, kTagsBB);

            cg_.builder_->SetInsertPoint(kHomogBB);
            cg_.builder_->CreateBr(kMergeBB);

            cg_.builder_->SetInsertPoint(kTagsBB);
            llvm::Value *tagI = cg_.builder_->CreateLoad(
                i8Ty, cg_.builder_->CreateGEP(i8Ty, tagsPtr, {iVal}, "wb.tag.gep"), "wb.tag");
            cg_.builder_->CreateBr(kMergeBB);

            cg_.builder_->SetInsertPoint(kMergeBB);
            llvm::PHINode *kindByte = cg_.builder_->CreatePHI(i8Ty, 2, "wb.kind");
            kindByte->addIncoming(homogKind, kHomogBB);
            kindByte->addIncoming(tagI, kTagsBB);
            // (AR) أعِد بناءَ %SadDyn ثمّ استخرِج i64 عدديًّا (يطابق toInt64)
            llvm::Value *dyn = Sad::LLVM::makeDyn(cg_, kindByte, elem);
            llvm::Value *elemI64 = Sad::LLVM::unpackI64(cg_, dyn);
            llvm::Value *byteVal = cg_.builder_->CreateTrunc(elemI64, i8Ty, "wb.byte");
            cg_.builder_->CreateStore(byteVal, cg_.builder_->CreateGEP(i8Ty, buf, {iVal}, "wb.slot"));
            llvm::Value *nextI = cg_.builder_->CreateAdd(iVal, llvm::ConstantInt::get(i64Ty, 1), "wb.next");
            iVal->addIncoming(nextI, cg_.builder_->GetInsertBlock());
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            // fopen(filename, "wb")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = cg_.module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = cg_.builder_->CreateGlobalStringPtr("wb", "mode_wb");
            llvm::Value *file = cg_.builder_->CreateCall(fopenFunc, {filename, mode}, "wb.file");

            // fwrite(buf, 1, len, file) — العائد والوسائط size_t
            auto *fwriteType = llvm::FunctionType::get(szTy, {ptrTy, szTy, szTy, ptrTy}, false);
            auto fwriteFunc = cg_.module_->getOrInsertFunction("fwrite", fwriteType);
            llvm::Value *written = cg_.builder_->CreateCall(
                fwriteFunc, {buf, llvm::ConstantInt::get(szTy, 1), lenSz, file}, "wb.written");

            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = cg_.module_->getOrInsertFunction("fclose", fcloseType);
            cg_.builder_->CreateCall(fcloseFunc, {file});

            llvm::Value *result = cg_.builder_->CreateZExtOrTrunc(written, i64Ty, "wb.result");
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ====================================================================
        // (AR) اقرأ_بايتات(مسار) — قراءة بايتات خام إلى مصفوفة أعداد (0..255).
        //      يفتح "rb"، يحدّد الحجمَ بـ fseek(SEEK_END)/ftell، يعيد المؤشّر، يقرأ
        //      البايتات، ثمّ يبني SadArray خماسيّ الحقول (tags=null، homogKind=Int)
        //      حيث كلّ خانةٍ i64 = zext(بايت). عرضُ C long للـ fseek/ftell يُختار من
        //      ثالوث الهدف (i32 على ويندوز LLP64، i64 غيره). يطابق قراءةَ المفسّر
        //      بايتًا ببايت (0..255، zext لا-موقَّع).
        // (EN) read_bytes(path) — read raw bytes into an array of numbers (0..255).
        //      Opens "rb", sizes via fseek(SEEK_END)/ftell, rewinds, reads, then builds
        //      a 5-field SadArray (tags=null, homogKind=Int) where each i64 slot =
        //      zext(byte). The C `long` width for fseek/ftell is picked from the target
        //      triple (i32 on Windows LLP64, i64 elsewhere). Matches the interpreter's
        //      byte-for-byte read (0..255, unsigned zext).
        // ====================================================================
        llvm::Value *FileCastsCodeGen::emitBuiltinFileReadBytes(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "FILE_READ_BYTES"}});
                return nullptr;
            }
            llvm::Value *filename = cg_.resolveOperand(inst->operands[0]);
            if (!filename)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = cg_.getInt8Type();
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Type *szTy = cg_.getSizeType();
            llvm::StructType *arrTy = getSadArrayType5(*cg_.context_);

            // (AR) عرضُ C long حسب الهدف: 32-بت على ويندوز، 64-بت غيره
            const std::string &triple = cg_.module_->getTargetTriple();
            bool isWindows = triple.find("windows") != std::string::npos ||
                             triple.find("win32") != std::string::npos;
            llvm::Type *longTy = isWindows ? i32Ty : i64Ty;

            // fopen(filename, "rb")
            auto *fopenType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto fopenFunc = cg_.module_->getOrInsertFunction("fopen", fopenType);
            llvm::Value *mode = cg_.builder_->CreateGlobalStringPtr("rb", "mode_rb");
            llvm::Value *file = cg_.builder_->CreateCall(fopenFunc, {filename, mode}, "rb.file");

            // fseek(file, 0, SEEK_END=2) ; ftell(file) ; fseek(file, 0, SEEK_SET=0)
            auto *fseekType = llvm::FunctionType::get(i32Ty, {ptrTy, longTy, i32Ty}, false);
            auto fseekFunc = cg_.module_->getOrInsertFunction("fseek", fseekType);
            auto *ftellType = llvm::FunctionType::get(longTy, {ptrTy}, false);
            auto ftellFunc = cg_.module_->getOrInsertFunction("ftell", ftellType);
            const int kSeekEnd = 2, kSeekSet = 0;
            cg_.builder_->CreateCall(fseekFunc, {file, llvm::ConstantInt::get(longTy, 0),
                                                 llvm::ConstantInt::get(i32Ty, kSeekEnd)});
            llvm::Value *sizeLong = cg_.builder_->CreateCall(ftellFunc, {file}, "rb.size.long");
            llvm::Value *size = cg_.builder_->CreateSExtOrTrunc(sizeLong, i64Ty, "rb.size");
            cg_.builder_->CreateCall(fseekFunc, {file, llvm::ConstantInt::get(longTy, 0),
                                                 llvm::ConstantInt::get(i32Ty, kSeekSet)});

            // (AR) مخزنُ بايتاتٍ خام بحجم size ثمّ fread
            llvm::Value *sizeSz = cg_.builder_->CreateZExtOrTrunc(size, szTy, "rb.size.sz");
            llvm::Value *byteBuf = cg_.emitMalloc(sizeSz, "rb.bytebuf");
            auto *freadType = llvm::FunctionType::get(szTy, {ptrTy, szTy, szTy, ptrTy}, false);
            auto freadFunc = cg_.module_->getOrInsertFunction("fread", freadType);
            cg_.builder_->CreateCall(freadFunc, {byteBuf, llvm::ConstantInt::get(szTy, 1), sizeSz, file}, "rb.read");

            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = cg_.module_->getOrInsertFunction("fclose", fcloseType);
            cg_.builder_->CreateCall(fcloseFunc, {file});

            // (AR) بناء SadArray خماسيّ: len=cap=size، data=malloc(size×8)، tags=null، homogKind=Int
            llvm::Value *arrStruct = cg_.emitMalloc(
                cg_.builder_->CreateZExtOrTrunc(llvm::ConstantExpr::getSizeOf(arrTy), szTy, "rb.arr.sz"), "rb.arr");
            cg_.builder_->CreateStore(size, cg_.builder_->CreateStructGEP(arrTy, arrStruct, 0, "rb.len.gep"));
            cg_.builder_->CreateStore(size, cg_.builder_->CreateStructGEP(arrTy, arrStruct, 1, "rb.cap.gep"));
            llvm::Value *slotBytes = cg_.builder_->CreateMul(
                size, llvm::ConstantInt::get(i64Ty, SAD_ARRAY_SLOT_BYTES), "rb.data.bytes");
            llvm::Value *dataPtr = cg_.emitMalloc(
                cg_.builder_->CreateZExtOrTrunc(slotBytes, szTy, "rb.data.bytes.sz"), "rb.data");
            cg_.builder_->CreateStore(dataPtr, cg_.builder_->CreateStructGEP(arrTy, arrStruct, 2, "rb.data.gep"));
            cg_.builder_->CreateStore(llvm::ConstantPointerNull::get(ptrTy),
                                      cg_.builder_->CreateStructGEP(arrTy, arrStruct, 3, "rb.tags.gep"));
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Int),
                                      cg_.builder_->CreateStructGEP(arrTy, arrStruct, 4, "rb.homogkind.gep"));

            // (AR) لولب: data[i] = (i64) zext(byteBuf[i])  (0..255 لا-موقَّع)
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "rb.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "rb.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "rb.done", curFunc);
            llvm::BasicBlock *entryBB = cg_.builder_->GetInsertBlock();
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *iVal = cg_.builder_->CreatePHI(i64Ty, 2, "rb.i");
            iVal->addIncoming(llvm::ConstantInt::get(i64Ty, 0), entryBB);
            llvm::Value *cond = cg_.builder_->CreateICmpSLT(iVal, size, "rb.cond");
            cg_.builder_->CreateCondBr(cond, bodyBB, doneBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *rawByte = cg_.builder_->CreateLoad(
                i8Ty, cg_.builder_->CreateGEP(i8Ty, byteBuf, {iVal}, "rb.byte.gep"), "rb.byte");
            llvm::Value *byteZ = cg_.builder_->CreateZExt(rawByte, i64Ty, "rb.byte.z");
            cg_.builder_->CreateStore(byteZ, cg_.builder_->CreateGEP(i64Ty, dataPtr, {iVal}, "rb.slot"));
            llvm::Value *nextI = cg_.builder_->CreateAdd(iVal, llvm::ConstantInt::get(i64Ty, 1), "rb.next");
            iVal->addIncoming(nextI, cg_.builder_->GetInsertBlock());
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = arrStruct;
            }
            return arrStruct;
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
            llvm::Value *buf = cg_.emitMalloc(bufSize, "copy.buf");

            // (AR) ‎fread/fwrite(ptr, size_t, size_t, FILE*)‎ والعائد ‎size_t‎ —
            //      بـ i64 ثابتًا على 32-بت تُزاح خانة ‎FILE*‎ ويُقرأ العائد قمامةً.
            llvm::Type *szTy = cg_.getSizeType();
            auto *freadType = llvm::FunctionType::get(szTy, {ptrTy, szTy, szTy, ptrTy}, false);
            auto freadFunc = cg_.module_->getOrInsertFunction("fread", freadType);
            auto *fwriteType = llvm::FunctionType::get(szTy, {ptrTy, szTy, szTy, ptrTy}, false);
            auto fwriteFunc = cg_.module_->getOrInsertFunction("fwrite", fwriteType);
            auto *fcloseType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fcloseFunc = cg_.module_->getOrInsertFunction("fclose", fcloseType);

            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.loop", curFunc);
            llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.body", curFunc);
            llvm::BasicBlock *loopDoneBB = llvm::BasicBlock::Create(*cg_.context_, "fcopy.ldone", curFunc);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            // (AR) العدّ يبقى بعرض ‎size_t‎ داخل الحلقة — لا تمديد ثمّ اقتطاع.
            llvm::Value *bytesRead = cg_.builder_->CreateCall(freadFunc,
                {buf, llvm::ConstantInt::get(szTy, 1),
                 cg_.coerceToSize(bufSize, "buf.size.sz"), srcFile}, "bytes.read");
            llvm::Value *hasData = cg_.builder_->CreateICmpUGT(bytesRead, llvm::ConstantInt::get(szTy, 0));
            cg_.builder_->CreateCondBr(hasData, loopBodyBB, loopDoneBB);

            cg_.builder_->SetInsertPoint(loopBodyBB);
            cg_.builder_->CreateCall(fwriteFunc, {buf, llvm::ConstantInt::get(szTy, 1), bytesRead, dstFile});
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopDoneBB);
            cg_.builder_->CreateCall(fcloseFunc, {srcFile});
            cg_.builder_->CreateCall(fcloseFunc, {dstFile});
            cg_.emitFreeCall(buf);
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

        llvm::Value *FileCastsCodeGen::emitBuiltinFileIsDir(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = cg_.resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // (AR) جسر وقت التشغيل: int sad_file_is_dir(const char* path)
            //      مضيف: stat/GetFileAttributes. حرّ: كعب ضعيف (تتجاوزه النواة).
            // (EN) Runtime bridge: int sad_file_is_dir(const char* path)
            //      Hosted: stat/GetFileAttributes. Freestanding: weak stub (OS overrides).
            auto *isDirType = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto isDirFunc = cg_.module_->getOrInsertFunction("sad_file_is_dir", isDirType);
            llvm::Value *result = cg_.builder_->CreateCall(isDirFunc, {path}, "isdir_result");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileIsSymlink(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = cg_.resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // (AR) جسر وقت التشغيل: int sad_file_is_symlink(const char*) — نظير lstat.
            //      لا يتبع الرابط، خلافًا لـsad_file_is_dir/is_file، فيكشف الرابطَ نفسه.
            // (EN) Runtime bridge: int sad_file_is_symlink(const char*) — lstat equivalent.
            //      Does not follow the link (unlike is_dir/is_file), so it detects the link.
            auto *fnTy = llvm::FunctionType::get(i32Ty, {ptrTy}, false);
            auto fn = cg_.module_->getOrInsertFunction("sad_file_is_symlink", fnTy);
            llvm::Value *result = cg_.builder_->CreateCall(fn, {path}, "islink_result");

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileRealPath(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = cg_.resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) جسر وقت التشغيل: char* sad_file_real_path(const char*) — يحلّ الروابط
            //      ويطبّع «..». يُرجع NULL إن تعذّر الحلّ (مسارٌ غير موجود) — وهي حالةٌ
            //      متوقّعة لا خطأ، فيَظهر NULL في اللغة قيمةً عدميّة.
            // (EN) Runtime bridge: char* sad_file_real_path(const char*) — resolves symlinks
            //      and normalises "..". Returns NULL when resolution fails (missing path),
            //      an expected case rather than an error, surfacing as a null value.
            auto *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto fn = cg_.module_->getOrInsertFunction("sad_file_real_path", fnTy);
            llvm::Value *raw = cg_.builder_->CreateCall(fn, {path}, "realpath_raw");

            // (AR) NULL ⇒ نصٌّ فارغ. بلا هذا يتسرّب مؤشّرٌ خامٌّ إلى مسار النصوص
            //      فيقرأه المُصرَّف قمامةً — ويختلف عن المفسّر فيكسر التكافؤ.
            // (EN) NULL ⇒ empty string. Without this a raw pointer leaks into the string
            //      path and the compiled program reads garbage — diverging from the
            //      interpreter and breaking parity.
            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                raw, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "realpath_failed");
            llvm::Value *empty = cg_.builder_->CreateGlobalStringPtr("", "realpath_empty");
            llvm::Value *result = cg_.builder_->CreateSelect(isNull, empty, raw, "realpath_result");

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
            return result;
        }

        llvm::Value *FileCastsCodeGen::emitBuiltinFileAbsPath(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *path = cg_.resolveOperand(inst->operands[0]);
            if (!path)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // (AR) جسر وقت التشغيل: char* sad_file_abs_path(const char*) — تطبيعٌ نصّيّ
            //      بلا حلِّ الروابط، فيعمل على مسارٍ غير موجود. NULL ⇒ نصٌّ فارغ،
            //      كنظيره الحقيقيّ، حفظًا للتكافؤ مع المفسّر.
            // (EN) Runtime bridge: char* sad_file_abs_path(const char*) — textual
            //      normalisation without symlink resolution, so it works on a missing
            //      path. NULL ⇒ empty string, as in its real-path sibling, preserving
            //      parity with the interpreter.
            auto *fnTy = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto fn = cg_.module_->getOrInsertFunction("sad_file_abs_path", fnTy);
            llvm::Value *raw = cg_.builder_->CreateCall(fn, {path}, "abspath_raw");

            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(
                raw, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "abspath_failed");
            llvm::Value *empty = cg_.builder_->CreateGlobalStringPtr("", "abspath_empty");
            llvm::Value *result = cg_.builder_->CreateSelect(isNull, empty, raw, "abspath_result");

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = result;
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