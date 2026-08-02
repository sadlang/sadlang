/*
 * ============================================================================
 * MapOpsCodeGen — توليد LLVM IR لعمليات الخرائط (HashMap)
 * Phase 7 Step 9 — 3 methods (emitCallMap entry + 2 helpers)
 * ============================================================================
 */
#ifndef SAD_LLVM_MAP_OPS_CODEGEN_H
#define SAD_LLVM_MAP_OPS_CODEGEN_H

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class MapOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit MapOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    MapOpsCodeGen(const MapOpsCodeGen &) = delete;
    MapOpsCodeGen &operator=(const MapOpsCodeGen &) = delete;

    // (AR) تطبيعُ مؤشّرِ الخريطة: قد يصل i64 خامًا (مؤشّرٌ مُحوَّلٌ بـptrtoint من
    //      سجلٍّ عابر) أو %SadDyn (قيمةٌ موسومةٌ زمنَ التشغيل حين يكون نوعُ الكائنِ
    //      الساكنُ «أي»). في الحالةِ الأخيرةِ نحرسُ الوسمَ زمنَ التشغيل: إن لم يكن
    //      Map هلعنا برمزٍ مميَّزٍ بدل قراءةِ مؤشّرٍ ليس خريطةً (SIGSEGV صامت).
    //      نظيرُ normalizeStringPtr للنصوص وnormalizeArrayPtr للمصفوفات.
    // (EN) Normalize the map pointer: it can arrive as a raw i64 (a ptrtoint'd pointer
    //      from a transient register) or as %SadDyn (a runtime-tagged value when the
    //      object's static type is «أي»). In the latter case we guard the tag at
    //      runtime: if it is not Map we panic with a distinct code rather than
    //      dereferencing a non-map pointer (a silent SIGSEGV). Mirrors
    //      normalizeStringPtr for strings and normalizeArrayPtr for arrays.
    llvm::Value *normalizeMapPtr(llvm::Value *mapValue, const char *label);

    // (AR) تطبيعُ مفتاحِ الخريطة: مفاتيحُ الخريطةِ نصوصٌ دائمًا (تُخزَّن بـstrdup
    //      وتُقارَن بـstrcmp)، فقيمةٌ موسومةٌ زمنَ التشغيل لا تصلح مفتاحًا إلّا إن
    //      كان وسمُها نصًّا. وهذا المسارُ **حيٌّ اليوم** (عنصرُ مصفوفةٍ مختلطةٍ
    //      يُستعمَل مفتاحًا)، فكان فكُّ الحمولةِ مؤشّرًا بلا حارسٍ يُمرِّر عددًا إلى
    //      strcmp ⇒ انهيارٌ صامت. نحرسُ الوسمَ ونفشلُ صاخبًا بدلَه.
    // (EN) Normalize a map key: map keys are always strings (stored via strdup and
    //      compared with strcmp), so a runtime-tagged value is a valid key only when
    //      its tag is Str. This path is **live today** (a mixed-array element used as
    //      a key), and unpacking the payload as a pointer without a guard handed an
    //      integer to strcmp ⇒ a silent crash. Guard the tag and fail loudly instead.
    llvm::Value *normalizeMapKey(llvm::Value *keyValue, const char *label);

    std::optional<llvm::Value *> emitCallMap(const std::string &funcName,
                                             std::vector<llvm::Value *> &args,
                                             std::shared_ptr<SIRInstruction> inst);
    llvm::Function *getOrCreateMapFindSlot();
    llvm::Function *getOrCreateMapCollect();

private:
    // (AR) يبعثُ كتلةَ فشلٍ لعدمِ تطابقِ وسمِ قيمةٍ ديناميكيّةٍ مع المتوقَّع، وينتهي
    //      بـunreachable: مستضاف ⇒ تشخيصٌ عربيٌّ + exit(1)؛ حرّ ⇒ __sad_panic برمزٍ
    //      مميَّز. مُستخرَجٌ كي لا يُكرَّر في كلِّ حارسِ وسم.
    // (EN) Emits the fail block for a dynamic value whose tag contradicts what is
    //      expected, terminated by unreachable: hosted ⇒ Arabic diagnostic + exit(1);
    //      freestanding ⇒ __sad_panic with a distinct code. Extracted so it is not
    //      duplicated in every tag guard.
    void emitDynTypeMismatchFailure(const char *label);
};

}} // namespace Sad::LLVM
#endif
