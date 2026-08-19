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

    // ══════════════════════════════════════════════════════════════════════
    // (AR) 🔑 يبني `__sad_map_compact` — يُطبِقُ الخاناتِ بعدَ الحذفِ بدلَ أن
    //      يترُكَ ثقبًا. الحذفُ كان يُصفّرُ خانةً وحدَها، والإدراجُ يبحثُ عن
    //      **أوّلِ خانةٍ فارغة** — فأوّلُ إدراجٍ بعدَ حذفٍ يسقطُ في الثقبِ
    //      متقدّمًا على مَن أُدخِلَ قبلَه. قِيس: أدخِلْ أ،ب،ج ثمّ احذف «أ» ثمّ
    //      أدخِلْ «د» ⇒ المفاتيحُ `[د, ب, ج]` لا `[ب, ج, د]`.
    //      وبالإطباقِ يصيرُ المشغولُ **بادئةً متّصلةً** `[0, count)` دائمًا،
    //      فـ«أوّلُ خانةٍ فارغة» تُساوي «الإلحاقَ في الذيل» بالضرورةِ لا بالحظّ.
    // (EN) Builds `__sad_map_compact` — closes the gap left by a delete instead of
    //      leaving a hole. Delete used to null one slot while insert scans for the
    //      FIRST EMPTY SLOT, so the next insert after a delete landed ahead of keys
    //      inserted before it. Measured: insert أ,ب,ج, delete أ, insert د ⇒ keys read
    //      [د, ب, ج] instead of [ب, ج, د]. Compaction makes the occupied region an
    //      unbroken prefix [0, count), so "first empty slot" IS "append at the tail".
    //
    // (AR) التوقيع: (ptr keys, ptr values, ptr types, i64 capacity, i64 idx) → void
    // ══════════════════════════════════════════════════════════════════════
    llvm::Function *getOrCreateMapCompact();

    // (AR) [م-٠٠١ ق٢] يبني `__sad_strip_diacritics` — إزالةُ التشكيلِ العربيِّ من
    //      نصٍّ بترميزِ UTF-8. مصدرُ الحقيقةِ يضعُ `ازل_تشكيل` في مساحةِ «خرائط»
    //      فمكانُها هنا. تُرجعُ نصًّا جديدًا مخصَّصًا بالكومة.
    // (EN) [card م-٠٠١, ق٢] Builds `__sad_strip_diacritics` — strips Arabic
    //      diacritics from a UTF-8 string. The SoT places `ازل_تشكيل` in the Maps
    //      namespace, hence its home here. Returns a fresh heap-allocated string.
    //
    // (AR) عقدُ الملكيّة: تُرجعُ نصًّا جديدًا مخصَّصًا بالكومة، **إلّا** أن يكونَ المصدرُ
    //      عدمًا فيُعادُ كما هو. فلا يجوزُ للمتصلِ أن يفترضَ ملكيّةً غيرَ مشروطة.
    // (EN) Ownership contract: returns a fresh heap-allocated string, **except** when the
    //      source is null, which is returned unchanged. Callers must not assume
    //      unconditional ownership.
    llvm::Function *getOrCreateStripDiacritics();

private:
    // (AR) يبعثُ كتلةَ فشلٍ لعدمِ تطابقِ وسمِ قيمةٍ ديناميكيّةٍ مع المتوقَّع، وينتهي
    //      بـunreachable: مستضاف ⇒ تشخيصٌ عربيٌّ + exit(1)؛ حرّ ⇒ __sad_panic برمزٍ
    //      مميَّز. مُستخرَجٌ كي لا يُكرَّر في كلِّ حارسِ وسم.
    // (EN) Emits the fail block for a dynamic value whose tag contradicts what is
    //      expected, terminated by unreachable: hosted ⇒ Arabic diagnostic + exit(1);
    //      freestanding ⇒ __sad_panic with a distinct code. Extracted so it is not
    //      duplicated in every tag guard.
    void emitDynTypeMismatchFailure(const char *label);

    // (AR) [م-٠٠١] يوسّعُ الخريطةَ إن أوشكت أن تمتلئ، حافظًا الثابتَ **«خانةٌ فارغةٌ
    //      واحدةٌ على الأقلّ»**. لم تكن الخريطةُ تنمو قطُّ: بلغت العددُ السعةَ فصار
    //      `__sad_map_find_slot` — وهو بحثٌ خطّيٌّ يُرجعُ صفرًا حين لا يجدُ — يُرجعُ
    //      الخانةَ صفرًا لكلِّ مفتاحٍ غائب. فمفتاحٌ جديدٌ يدهسُ الخانةَ الأولى بلا
    //      زيادةِ العدد، و«يحتوي» تقولُ صحيحًا لمفتاحٍ غيرِ موجود، والقيمةُ
    //      الافتراضيّةُ تُهمَل — كلُّه صامتٌ برمزِ خروجٍ صفريّ.
    //
    //      والتوسيعُ هنا **بلا إعادةِ تجزئة**: البحثُ خطّيٌّ على كلِّ الخانات، فلا
    //      يرتبطُ موضعُ المفتاحِ بالسعة، فيكفي نسخُ المصفوفاتِ الثلاثِ كما هي.
    // (EN) [card م-٠٠١] Grows the map when it is about to fill, maintaining the **"at
    //      least one empty slot"** invariant. The map never grew: once count reached
    //      capacity, `__sad_map_find_slot` — a linear scan returning 0 when it finds
    //      nothing — returned slot 0 for every absent key. A new key then overwrote the
    //      first entry without raising the count, «contains» answered true for a key that
    //      is not there, and the default value was ignored — all silently, exit code 0.
    //
    //      Growth needs **no rehash**: the search is a linear scan over every slot, so a
    //      key's position is unrelated to capacity and copying the three arrays suffices.
    void emitMapGrowIfFull(llvm::Value *mapPtr);
};

}} // namespace Sad::LLVM
#endif
