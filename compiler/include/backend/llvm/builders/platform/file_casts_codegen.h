/*
 * ============================================================================
 * FileCastsCodeGen - Phase 8 Step 10 - 8 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_FILECASTS_CODEGEN_H
#define SAD_LLVM_FILECASTS_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class FileCastsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit FileCastsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    FileCastsCodeGen(const FileCastsCodeGen &) = delete;
    FileCastsCodeGen &operator=(const FileCastsCodeGen &) = delete;

    // ====================================================================
    // (AR) فتحُ ملفٍّ محروس: يفتح، فإن أخفق يقفز إلى مسارِ فشلٍ يُنتج القيمةَ
    //      البديلة، ولا يمرّ NULL إلى fread/fputs/fclose.
    //
    //      العلّة: كلُّ مواضع الفتح كانت تمرّر عائدَ fopen مباشرةً. وفشلُ الفتح
    //      حالةٌ **عاديّة** (ملفٌّ غائب · مجلَّدٌ غائب · إذنٌ ناقص)، لكنّ تمريرَ
    //      NULL إلى CRT يستدعي مُعالِجَ الوسيط الباطل فيُنهي العمليّةَ بـ
    //      __fastfail (‏0xC0000409) بلا رسالةٍ ولا رمزِ خطأ — فيُقرأ الفشلُ
    //      المتوقَّع انهيارًا غامضًا. الحارسُ يعيده قيمةَ فشلٍ يفحصها المستدعي.
    //
    //      دالّةٌ واحدةٌ لا فحصٌ منسوخٌ ستَّ مرّات: النسخُ هو ما يجعل الموضعَ
    //      السابعَ يُكتَب بلا حارسٍ فيعود العيب.
    // (EN) Guarded fopen: on failure, branch to a fail path yielding `failValue`
    //      instead of passing NULL into CRT calls (which fast-fails with
    //      0xC0000409 and no message). One helper, not six copied checks.
    //
    // @param path    مؤشّرُ المسار / path pointer
    // @param mode    وضعُ الفتح ("r" · "w" · "rb" …) / fopen mode
    // @param tag     بادئةُ تسميةِ السجلّات / register-name prefix
    // @param failValue القيمةُ المُرجَعة عند الفشل / value produced on failure
    // @param[out] mergeBB كتلةُ الالتقاء — يقفز إليها المستدعي بعد نجاحه
    // @param[out] phi عقدةُ PHI التي يضيف إليها المستدعي قيمةَ النجاح
    // @param failMsg إن لم يكن فارغًا: يُرمى خطأٌ قابلٌ للالتقاط بدلَ إنتاج
    //        `failValue` — لأنّ المفسّر (المرجع) يرمي RUN007 عند فشلِ الفتح.
    //        وحينها لا يُضاف طرفُ الفشلِ إلى PHI: كتلتُه تنتهي بـ`unreachable`.
    // @return مؤشّرُ الملفّ في مسار النجاح (نقطةُ الإدراج تصير كتلةَ النجاح)
    // ====================================================================
    llvm::Value *emitFopenGuarded(llvm::Value *path, const char *mode, const char *tag,
                                  llvm::Value *failValue, llvm::BasicBlock *&mergeBB,
                                  llvm::PHINode *&phi, const char *failMsg = nullptr);

    llvm::Value *emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileWriteBytes(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileReadBytes(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileSize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinRegexSearch(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinRegexMatch(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileIsDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileRemoveDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileExists(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileIsFile(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileIsSymlink(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileRealPath(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileAbsPath(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
