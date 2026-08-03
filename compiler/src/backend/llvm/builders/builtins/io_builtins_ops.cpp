/*
 * ============================================================================
 * IOBuiltinsCodeGen implementation
 * Phase 7 Step 15
 * ============================================================================
 */
#include "llvm_codegen.h"
#include "sir_constants.h"
#include "builders/builtins/io_builtins_codegen.h"
#include "adt_payload_tags.h"
#include "sad_dyn_repr.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace Sad { namespace LLVM {

// (AR) نوعُ بنيةِ SadArray — يُفوَّض إلى التعريفِ الوحيدِ في sad_dyn_repr (كانت هذه النسخةُ
//      بنيويّةً `StructType::get` والأخرى مسمّاةً `create`؛ التخطيطُ واحدٌ والاسمُ يُوحَّد الآن).
// (EN) SadArray struct type — delegated to the single definition in sad_dyn_repr (this copy was
//      structural `StructType::get` while another was a named `create`; same layout, now unified).
static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
{
    return sadArrayStructType(ctx);
}



        llvm::Value *IOBuiltinsCodeGen::emitBuiltinPrint(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                // (AR) حتى بدون معاملات، الطباعة "نُفّذت" — إرجاع sentinel
                // (EN) Even with no operands, print was "handled" — return sentinel
                return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);

            // (AR) في الوضع المستقل (freestanding): استخدم دوال الإخراج التسلسلي المباشرة
            // (EN) In freestanding mode: use direct serial output functions
            if (cg_.freestanding_)
            {
                llvm::Type *ptrTy = llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo();
                llvm::Type *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                llvm::Type *voidTy = llvm::Type::getVoidTy(*cg_.context_);

                llvm::FunctionType *putsFT = llvm::FunctionType::get(voidTy, {ptrTy}, false);
                llvm::FunctionCallee putsFn = cg_.module_->getOrInsertFunction("__sad_serial_puts", putsFT);

                llvm::FunctionType *putintFT = llvm::FunctionType::get(voidTy, {i64Ty}, false);
                llvm::FunctionCallee putintFn = cg_.module_->getOrInsertFunction("__sad_serial_putint", putintFT);

                for (auto &op : inst->operands)
                {
                    llvm::Value *v = cg_.resolveOperand(op);
                    if (!v)
                        continue;
                    // (AR) ISSUE-063: %SadDyn أوّلًا قبل فروع dataType (النوع الأماميّ قد
                    //      يكون بائتًا) — dynToString حرّ المكتبة (يستدعي __sad_itoa/ftoa).
                    // (EN) ISSUE-063: %SadDyn first, before the dataType branches (the
                    //      frontend type may be stale) — dynToString is freestanding-safe.
                    if (isSadDyn(v))
                    {
                        cg_.builder_->CreateCall(putsFn, {dynToString(cg_, v)});
                        continue;
                    }
                    // (AR) القيم المنطقية: طباعة "صحيح"/"خطأ" في الوضع المستقل
                    // (EN) Boolean values: print "صحيح"/"خطأ" in freestanding mode
                    if (op.dataType == SadTypeKind::Boolean)
                    {
                        llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true");
                        llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "bool.false");
                        llvm::Value *cond;
                        if (v->getType()->isIntegerTy(1))
                        {
                            cond = v;
                        }
                        else if (v->getType()->isPointerTy())
                        {
                            // (AR) [Fix BF-04] المؤشرات تُقارن بـ null وليس بـ 0
                            // (EN) [Fix BF-04] Pointers must be compared to null, not integer 0
                            cond = cg_.builder_->CreateICmpNE(
                                v,
                                llvm::ConstantPointerNull::get(
                                    llvm::cast<llvm::PointerType>(v->getType())),
                                "bool.cmp.ptr");
                        }
                        else
                        {
                            cond = cg_.builder_->CreateICmpNE(v, llvm::ConstantInt::get(v->getType(), 0), "bool.cmp");
                        }
                        llvm::Value *selected = cg_.builder_->CreateSelect(cond, trueStr, falseStr, "bool.str");
                        cg_.builder_->CreateCall(putsFn, {selected});
                    }
                    else if (v->getType()->isPointerTy())
                    {
                        cg_.builder_->CreateCall(putsFn, {v});
                    }
                    else if (v->getType()->isIntegerTy())
                    {
                        llvm::Value *ext = cg_.builder_->CreateIntCast(v, i64Ty, true);
                        cg_.builder_->CreateCall(putintFn, {ext});
                    }
                    else if (v->getType()->isDoubleTy())
                    {
                        // (AR) تحويل عشري إلى صحيح مؤقتاً
                        llvm::Value *asInt = cg_.builder_->CreateFPToSI(v, i64Ty);
                        cg_.builder_->CreateCall(putintFn, {asInt});
                    }
                    else
                    {
                        llvm::Value *conv = cg_.builder_->CreateIntCast(v, i64Ty, true);
                        cg_.builder_->CreateCall(putintFn, {conv});
                    }
                }
                // (AR) إرجاع sentinel للوضع المستقل أيضاً
                // (EN) Return sentinel for freestanding mode too
                return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
            }

            // (AR) الوضع العادي: استخدم printf
            // (EN) Normal mode: use printf
            llvm::FunctionType *printfType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*cg_.context_), {llvm::Type::getInt8Ty(*cg_.context_)->getPointerTo()}, true);
            llvm::FunctionCallee printfFunc = cg_.module_->getOrInsertFunction("printf", printfType);
            for (auto &op : inst->operands)
            {
                llvm::Value *v = cg_.resolveOperand(op);
                if (!v)
                    continue;

                // ================================================================
                // (AR) ISSUE-063: قيمة %SadDyn تُحسَم أوّلًا وقبل فروع dataType —
                //      نوع SIR الأماميّ قد يكون بائتًا (منطقيّ/مصفوفة/خريطة) بينما
                //      القيمة فعليًّا %SadDyn (خانة رقّاها المسحُ المسبق)، وكانت فروع
                //      dataType تُصدر ICmp/IntToPtr على بنية ⇒ IR فاسد. النوع %SadDyn
                //      هو المعلومة — الموزِّع dynToString يطابق المفسّر لكلّ وسم.
                // (EN) ISSUE-063: a %SadDyn value is dispatched FIRST, before the
                //      dataType-driven branches — the frontend SIR type may be stale
                //      (bool/array/map) while the value is actually %SadDyn (a slot
                //      the pre-scan promoted); those branches used to emit ICmp/
                //      IntToPtr on a struct ⇒ invalid IR. The %SadDyn type IS the
                //      information — dynToString matches the interpreter per kind.
                // ================================================================
                if (isSadDyn(v))
                {
                    llvm::Value *dynStr = dynToString(cg_, v);
                    llvm::Value *dynFmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                    cg_.builder_->CreateCall(printfFunc, {dynFmt, dynStr});
                    continue;
                }

                // (AR) القيم المنطقية: طباعة "صحيح"/"خطأ" بدلاً من 1/0
                // (EN) Boolean values: print "صحيح"/"خطأ" instead of 1/0
                if (op.dataType == SadTypeKind::Boolean)
                {
                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                    llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true"); // صحيح
                    llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "bool.false");       // خطأ
                    llvm::Value *cond;
                    if (v->getType()->isIntegerTy(1))
                    {
                        cond = v;
                    }
                    else if (v->getType()->isPointerTy())
                    {
                        // (AR) [Fix BF-04] المؤشرات تُقارن بـ null وليس بـ 0
                        // (EN) [Fix BF-04] Pointers must be compared to null, not integer 0
                        cond = cg_.builder_->CreateICmpNE(
                            v,
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(v->getType())),
                            "bool.cmp.ptr");
                    }
                    else
                    {
                        cond = cg_.builder_->CreateICmpNE(v, llvm::ConstantInt::get(v->getType(), 0), "bool.cmp");
                    }
                    llvm::Value *selected = cg_.builder_->CreateSelect(cond, trueStr, falseStr, "bool.str");
                    cg_.builder_->CreateCall(printfFunc, {fmt, selected});
                }
                // (AR) طباعة المصفوفات: تحويل المصفوفة إلى نص ثم طباعتها
                // (EN) Array printing: convert array to string then print
                //      المؤشر قد يكون ptr مباشرة أو i64 (مخزّن في alloca)
                else if (op.dataType == SadTypeKind::Array)
                {
                    // (AR) التأكد من وجود دالة المساعدة __sad_array_to_string
                    cg_.ensureArrayToStringHelper();

                    auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                    auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                    // (AR) تحويل i64 إلى ptr إذا لزم الأمر
                    // (EN) Convert i64 to ptr if needed (array pointer stored as i64 in alloca)
                    llvm::Value *arrPtr = v;
                    if (arrPtr->getType()->isIntegerTy(64))
                    {
                        arrPtr = cg_.builder_->CreateIntToPtr(arrPtr, ptrTy, "print.arr.i2p");
                    }
                    else if (!arrPtr->getType()->isPointerTy())
                    {
                        // (AR) نوع غير متوقع — اطبع كرقم بدلاً من الانهيار
                        llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "fmt.d");
                        cg_.builder_->CreateCall(printfFunc, {fmt, v});
                        continue;
                    }

                    // (AR) تحميل طول المصفوفة وبياناتها من بنية SadArray
                    // (EN) Load array length and data from SadArray struct
                    llvm::StructType *arrTy = getArrayStructType(*cg_.context_);
                    llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "print.arr.len.gep");
                    llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "print.arr.len");
                    llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "print.arr.data.gep");
                    llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "print.arr.data");

                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");

                    // (AR) عناصر نصّيّة ⇒ نظير __sad_array_to_string_str (يخصّص مخزنه ويطبع بـ%s).
                    //      يُصلح طبع المصفوفات النصّيّة («[أ، ب]») بدل عناوين المؤشّرات. غير النصّيّة
                    //      تبقى على المسار العدديّ (%lld) الأصليّ — متوافق تمامًا مع السابق.
                    // (EN) String elements ⇒ the __sad_array_to_string_str variant (mallocs its own
                    //      buffer, prints via %s). Fixes string-array printing ("[a, b]") instead of
                    //      pointer addresses. Non-string arrays keep the original integer path (%lld).
                    if (op.elementType == SadTypeKind::String)
                    {
                        cg_.ensureArrayToStringStrHelper();
                        llvm::FunctionType *strHelperType = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy}, false);
                        llvm::FunctionCallee strHelperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string_str", strHelperType);
                        llvm::Value *strResult = cg_.builder_->CreateCall(strHelperFn, {arrLen, dataPtr}, "print.arr.sstr");
                        cg_.builder_->CreateCall(printfFunc, {fmt, strResult});
                        cg_.emitFreeCall(strResult); // (AR) المساعِد النصّيّ يخصّص، فنحرّر ناتجه
                    }
                    // (AR) عناصر عشريّة ⇒ نظير __sad_array_to_string_float (bitcast خانة⇒double
                    //      ثمّ __sad_format_double) ⇒ «[1.5, 2.5]» بدل بتّات double خام بـ%lld
                    //      (ISSUE-080). يخصّص مخزنه فنحرّره. المتجانسة فقط (elementType=Float).
                    // (EN) Float elements ⇒ the __sad_array_to_string_float variant (bitcast slot
                    //      ⇒double then __sad_format_double) ⇒ "[1.5, 2.5]" instead of raw double
                    //      bits via %lld (ISSUE-080). Mallocs, so we free it. Homogeneous only.
                    else if (op.elementType == SadTypeKind::Float)
                    {
                        cg_.ensureArrayToStringFloatHelper();
                        llvm::FunctionType *fHelperType = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy}, false);
                        llvm::FunctionCallee fHelperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string_float", fHelperType);
                        llvm::Value *fResult = cg_.builder_->CreateCall(fHelperFn, {arrLen, dataPtr}, "print.arr.fstr");
                        cg_.builder_->CreateCall(printfFunc, {fmt, fResult});
                        cg_.emitFreeCall(fResult);
                    }
                    // (AR) [عناصر موسومة — option A] عناصرُ مصفوفةٍ مختلطةٍ قياسيّة: الخانات
                    //      مؤشّرات صناديق %SadDyn ⇒ نظير __sad_array_to_string_dyn (يفكّ كلّ
                    //      عنصرٍ عبر dynToString) ⇒ «[500, 3.5]» بدل عناوين الصناديق.
                    // (EN) [boxed elements] scalar-heterogeneous array: slots are %SadDyn box
                    //      pointers ⇒ the __sad_array_to_string_dyn variant (per-element
                    //      dynToString) ⇒ "[500, 3.5]" instead of box addresses.
                    else if (op.elementType == SadTypeKind::Any)
                    {
                        // (AR) [وسم زمن-تشغيل] مرّر مخزنَ الوسوم (الحقل ٣) مع البيانات؛ المساعِد
                        //      يعيد بناء %SadDyn من (الوسم، الحمولة) لكلّ عنصر (أو Int إن null).
                        // (EN) [runtime tag] pass the tags buffer (field 3) with the data; the
                        //      helper rebuilds %SadDyn from (tag, payload) per element (Int if null).
                        llvm::Value *tagsGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 3, "print.arr.tags.gep");
                        llvm::Value *tagsPtr = cg_.builder_->CreateLoad(ptrTy, tagsGep, "print.arr.tags");
                        cg_.ensureArrayToStringDynHelper();
                        llvm::FunctionType *dHelperType = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy, ptrTy}, false);
                        llvm::FunctionCallee dHelperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string_dyn", dHelperType);
                        llvm::Value *dResult = cg_.builder_->CreateCall(dHelperFn, {arrLen, dataPtr, tagsPtr}, "print.arr.dstr");
                        cg_.builder_->CreateCall(printfFunc, {fmt, dResult});
                        cg_.emitFreeCall(dResult);
                    }
                    else
                    {
                        // (AR) المسار العدديّ الأصليّ: مخزن مُقدَّر (طول*34+4) + __sad_array_to_string.
                        // (EN) Original integer path: estimated buffer (len*34+4) + __sad_array_to_string.
                        llvm::Value *bufLen = cg_.builder_->CreateAdd(
                            cg_.builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 34)),
                            llvm::ConstantInt::get(i64Ty, 4), "print.arr.bufsz");
                        llvm::Value *buf = cg_.emitMalloc(bufLen, "print.arr.buf");

                        llvm::FunctionType *helperType = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
                        llvm::FunctionCallee helperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string", helperType);
                        llvm::Value *strResult = cg_.builder_->CreateCall(helperFn, {buf, arrLen, dataPtr}, "print.arr.str");
                        cg_.builder_->CreateCall(printfFunc, {fmt, strResult});
                        cg_.emitFreeCall(buf);
                    }
                }
                // ================================================================
                // (AR) طباعة الخريطة: «{"م": ق، …}» عبر __sad_map_to_string (مفاتيح مقتبسة،
                //      قيم حسب النوع). يطابق تنسيق المفسّر. يجب أن يسبق فرع المؤشّر لأنّ الخريطة
                //      مؤشّر (وإلّا طُبِعت بـ%s كقمامة). المساعِد يخصّص مخزنه فنحرّر ناتجه.
                // (EN) Map printing: "{"k": v, …}" via __sad_map_to_string (quoted keys, typed
                //      values), mirroring the interpreter. Must precede the pointer branch since a
                //      map is a pointer. The helper mallocs, so we free its result.
                // ================================================================
                else if (op.dataType == SadTypeKind::Map)
                {
                    auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                    auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                    llvm::Value *mapPtr = v;
                    if (mapPtr->getType()->isIntegerTy(64))
                        mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "print.map.i2p");
                    if (mapPtr->getType()->isPointerTy())
                    {
                        cg_.ensureMapToStringHelper();
                        llvm::FunctionType *mHelperType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
                        llvm::FunctionCallee mHelperFn = cg_.module_->getOrInsertFunction("__sad_map_to_string", mHelperType);
                        llvm::Value *mStr = cg_.builder_->CreateCall(mHelperFn, {mapPtr}, "print.map.str");
                        llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                        cg_.builder_->CreateCall(printfFunc, {fmt, mStr});
                        cg_.emitFreeCall(mStr);
                    }
                    else
                    {
                        // (AR) نوع غير متوقّع — اطبع رقمًا بدل الانهيار / (EN) unexpected — print as number
                        llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "fmt.d");
                        cg_.builder_->CreateCall(printfFunc, {fmt, v});
                    }
                }
                // (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): قيمةٌ ديناميّة %SadDyn ⇒ موزِّع dynToString
                //      (يفحص وسم النوع ويطابق المفسّر) ثمّ اطبع %s. النوع %SadDyn هو المعلومة —
                //      لا فكّ بتّاتٍ يدويّ ولا التباس بالصحيح.
                // (EN) ISSUE-076 (%SadDyn root fix): a dynamic %SadDyn value ⇒ the dynToString
                //      dispatcher (inspects the kind tag, matches the interpreter) then print %s.
                //      The %SadDyn type is the information — no manual bit decode, no int confusion.
                else if (isSadDyn(v))
                {
                    llvm::Value *s = dynToString(cg_, v);
                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                    cg_.builder_->CreateCall(printfFunc, {fmt, s});
                }
                else if (v->getType()->isPointerTy())
                {
                    // (AR) طباعة نص بدون سطر جديد تلقائي - SIR builder يضيف \n صراحة عند الحاجة
                    // (EN) Print string without auto-newline - SIR builder adds \n explicitly when needed
                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                    cg_.builder_->CreateCall(printfFunc, {fmt, v});
                }
                // ================================================================
                // (AR) [Fix Boolean-Print] طباعة قيم منطقية i1 كـ صحيح/خطأ
                //      عند تحميل متغير ساكن منطقي (i1) أو أي قيمة i1 في LLVM,
                //      يجب طباعتها كنص صحيح/خطأ لا كرقم 0/1.
                //      يعمل حتى إذا لم يكن op.dataType == Boolean (لأن LLVM type i1 كافٍ).
                // (EN) [Fix Boolean-Print] Print i1 boolean values as صحيح/خطأ
                //      When loading a static boolean member (i1) or any i1 in LLVM,
                //      must print as صحيح/خطأ text, not as 0/1 integer.
                //      Works even if op.dataType != Boolean (LLVM i1 type is sufficient).
                // ================================================================
                else if (v->getType()->isIntegerTy(1))
                {
                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                    llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.print.true"); // صحيح
                    llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "bool.print.false");       // خطأ
                    llvm::Value *selected = cg_.builder_->CreateSelect(v, trueStr, falseStr, "bool.print.sel");
                    cg_.builder_->CreateCall(printfFunc, {fmt, selected});
                }
                // ================================================================
                // (AR) [Fix #52] طباعة نص مخزّن في i64:
                //      عند إعادة إسناد متغير ديناميكي من Integer/Pointer إلى String،
                //      القيمة تُخزن كـ ptrtoint في alloca i64. عند القراءة، نحصل على i64.
                //      إذا كان dataType يشير إلى String (بعد تحديث النوع في Fix #52)
                //      نحوّل i64 إلى ptr ونطبع كنص %s.
                //      بدون هذا: النص يُطبع كعنوان رقمي (مثل 140696642985994)
                // (EN) [Fix #52] Print string stored as i64:
                //      When variable was dynamically reassigned Int→String,
                //      value is stored as ptrtoint in i64 alloca. On read, we get i64.
                //      If dataType indicates String, convert i64→ptr and print as %s.
                //      Without this: string prints as numeric pointer address.
                // ================================================================
                else if (op.dataType == SadTypeKind::String && v->getType()->isIntegerTy(64))
                {
                    llvm::Value *strPtr = cg_.builder_->CreateIntToPtr(
                        v, llvm::PointerType::getUnqual(*cg_.context_), "print.str.i2p");
                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                    cg_.builder_->CreateCall(printfFunc, {fmt, strPtr});
                }
                // ================================================================
                // (AR) [إصلاح قنوات] فك وسم MSB 2-bit للقيم المستقبَلة من القنوات
                //      نوع Any يعني أن القيمة موسومة بنظام:
                //      bit63=0 → مؤشر (نص) — inttoptr وطباعة %s
                //      bit63=1, bit62=0 → رقم — مسح bit63 وطباعة %lld
                //      bit63=1, bit62=1 → منطقي — مسح bit63+62 وطباعة صحيح/خطأ
                //      kSadNullSentinel → لاشيء
                // (EN) [Channel fix] Decode MSB 2-bit tagged values received from channels
                // ================================================================
                else if (op.dataType == SadTypeKind::Any && v->getType()->isIntegerTy(64))
                {
                    auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                    auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                    // (AR) فحص sentinel أولاً (لاشيء)
                    llvm::Value *isNullSentinel = cg_.builder_->CreateICmpEQ(
                        v, llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel),
                        "any.is_null");

                    auto *parentFunc = cg_.builder_->GetInsertBlock()->getParent();
                    auto *nullBB = llvm::BasicBlock::Create(*cg_.context_, "any.null", parentFunc);
                    auto *checkTagBB = llvm::BasicBlock::Create(*cg_.context_, "any.check_tag", parentFunc);
                    auto *ptrOrFloatBB = llvm::BasicBlock::Create(*cg_.context_, "any.ptr_or_float", parentFunc);
                    auto *ptrBB = llvm::BasicBlock::Create(*cg_.context_, "any.ptr", parentFunc);
                    auto *floatBB = llvm::BasicBlock::Create(*cg_.context_, "any.float", parentFunc);
                    auto *intOrBoolBB = llvm::BasicBlock::Create(*cg_.context_, "any.int_or_bool", parentFunc);
                    auto *boolBB = llvm::BasicBlock::Create(*cg_.context_, "any.bool", parentFunc);
                    auto *intBB = llvm::BasicBlock::Create(*cg_.context_, "any.int", parentFunc);
                    auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "any.merge", parentFunc);

                    cg_.builder_->CreateCondBr(isNullSentinel, nullBB, checkTagBB);

                    // (AR) لاشيء
                    cg_.builder_->SetInsertPoint(nullBB);
                    {
                        llvm::Value *fmtS = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                        llvm::Value *nullStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "null.str"); // لاشيء
                        cg_.builder_->CreateCall(printfFunc, {fmtS, nullStr});
                    }
                    cg_.builder_->CreateBr(mergeBB);

                    // (AR) فحص bit63: مضبوط ⇒ صحيح/منطقيّ (10/11)؛ مصفّر ⇒ مؤشّر/عشريّ (00/01)
                    // (EN) Check bit63: set ⇒ int/bool (10/11); clear ⇒ pointer/float (00/01)
                    cg_.builder_->SetInsertPoint(checkTagBB);
                    {
                        llvm::Value *bit63Mask = llvm::ConstantInt::get(i64Ty, kAdtPayloadBit63);
                        llvm::Value *bit63 = cg_.builder_->CreateAnd(v, bit63Mask, "any.bit63");
                        llvm::Value *hiClear = cg_.builder_->CreateICmpEQ(
                            bit63, llvm::ConstantInt::get(i64Ty, 0), "any.hi_clear");
                        cg_.builder_->CreateCondBr(hiClear, ptrOrFloatBB, intOrBoolBB);
                    }

                    // (AR) ISSUE-076/084: تمييز النصّ (00) عن الصندوق العشريّ (01) عبر bit62
                    // (EN) ISSUE-076/084: distinguish string (00) from boxed float (01) via bit62
                    cg_.builder_->SetInsertPoint(ptrOrFloatBB);
                    {
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62);
                        llvm::Value *bit62 = cg_.builder_->CreateAnd(v, bit62Mask, "any.bit62.pf");
                        llvm::Value *isFloat = cg_.builder_->CreateICmpNE(
                            bit62, llvm::ConstantInt::get(i64Ty, 0), "any.is_float");
                        cg_.builder_->CreateCondBr(isFloat, floatBB, ptrBB);
                    }

                    // (AR) مؤشر (نص) — 00
                    cg_.builder_->SetInsertPoint(ptrBB);
                    {
                        llvm::Value *strPtr = cg_.builder_->CreateIntToPtr(v, ptrTy, "any.str.i2p");
                        llvm::Value *fmtS = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                        cg_.builder_->CreateCall(printfFunc, {fmtS, strPtr});
                    }
                    cg_.builder_->CreateBr(mergeBB);

                    // (AR) صندوق عشريّ (01) — امسح bit62 ⇒ مؤشّر الصندوق ⇒ حمّل الـdouble ⇒
                    //      اطبع بـ__sad_print_double (نفس تنسيق المفسّر). ISSUE-076/084.
                    // (EN) Boxed float (01) — clear bit62 ⇒ box pointer ⇒ load double ⇒ print via
                    //      __sad_print_double (same format as the interpreter). ISSUE-076/084.
                    cg_.builder_->SetInsertPoint(floatBB);
                    {
                        llvm::Value *boxI64 = cg_.builder_->CreateAnd(
                            v, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit62), "any.float.clear");
                        llvm::Value *boxPtr = cg_.builder_->CreateIntToPtr(boxI64, ptrTy, "any.float.ptr");
                        llvm::Value *d = cg_.builder_->CreateLoad(
                            llvm::Type::getDoubleTy(*cg_.context_), boxPtr, "any.float.load");
                        llvm::FunctionType *printDoubleTy = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_),
                            {llvm::Type::getDoubleTy(*cg_.context_)}, false);
                        llvm::FunctionCallee printDoubleFn =
                            cg_.module_->getOrInsertFunction("__sad_print_double", printDoubleTy);
                        cg_.builder_->CreateCall(printDoubleFn, {d});
                    }
                    cg_.builder_->CreateBr(mergeBB);

                    // (AR) فحص bit62 — منطقي أم رقم
                    cg_.builder_->SetInsertPoint(intOrBoolBB);
                    {
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(i64Ty, 1ULL << 62);
                        llvm::Value *bit62 = cg_.builder_->CreateAnd(v, bit62Mask, "any.bit62");
                        llvm::Value *isBool = cg_.builder_->CreateICmpNE(
                            bit62, llvm::ConstantInt::get(i64Ty, 0), "any.is_bool");
                        cg_.builder_->CreateCondBr(isBool, boolBB, intBB);
                    }

                    // (AR) منطقي — مسح bit63+62
                    cg_.builder_->SetInsertPoint(boolBB);
                    {
                        llvm::Value *clearMask = llvm::ConstantInt::get(i64Ty, ~(3ULL << 62));
                        llvm::Value *cleanVal = cg_.builder_->CreateAnd(v, clearMask, "any.bool.clean");
                        llvm::Value *fmtS = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                        llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "any.bool.true"); // صحيح
                        llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xae\xd8\xb7\xd8\xa3", "any.bool.false"); // خطأ
                        llvm::Value *cond = cg_.builder_->CreateICmpNE(
                            cleanVal, llvm::ConstantInt::get(i64Ty, 0), "any.bool.cond");
                        llvm::Value *boolStr = cg_.builder_->CreateSelect(cond, trueStr, falseStr, "any.bool.sel");
                        cg_.builder_->CreateCall(printfFunc, {fmtS, boolStr});
                    }
                    cg_.builder_->CreateBr(mergeBB);

                    // (AR) رقم — مسح bit63
                    cg_.builder_->SetInsertPoint(intBB);
                    {
                        llvm::Value *clearBit63 = llvm::ConstantInt::get(i64Ty, ~(1ULL << 63));
                        llvm::Value *cleanVal = cg_.builder_->CreateAnd(v, clearBit63, "any.int.clean");
                        llvm::Value *fmtD = cg_.builder_->CreateGlobalStringPtr("%lld", "fmt.d");
                        cg_.builder_->CreateCall(printfFunc, {fmtD, cleanVal});
                    }
                    cg_.builder_->CreateBr(mergeBB);

                    cg_.builder_->SetInsertPoint(mergeBB);
                }
                else if (v->getType()->isIntegerTy(64))
                {
                    // ================================================================
                    // (AR) [Fix] فك تشفير MSB 2-bit لعناصر الصف/ADT عند الطباعة المباشرة
                    //      عند استدعاء اطبع_سطر(ص[0]) حيث ص صف (tuple)،
                    //      القيمة i64 تحمل وسم MSB:
                    //      bit63=0 → مؤشر (نص) — inttoptr وطباعة %s
                    //      bit63=1, bit62=0 → رقم — مسح bit63 وطباعة %lld
                    //      bit63=1, bit62=1 → منطقي — مسح bit63+bit62 وطباعة صحيح/خطأ
                    //      بدون هذا: القيمة الخام تُطبع كرقم سلبي ضخم
                    // (EN) [Fix] MSB 2-bit decoding for tuple/ADT elements in direct print
                    //      When calling print(tuple[0]), the i64 value carries MSB tags:
                    //      bit63=0 → pointer (string), bit63=1+bit62=0 → int, bit63=1+bit62=1 → bool
                    // ================================================================
                    auto isPtrIt = cg_.context_info_.namedValues.find(op.name + ".__is_ptr");
                    if (isPtrIt != cg_.context_info_.namedValues.end())
                    {
                        llvm::Value *isPtr = isPtrIt->second;
                        auto *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                        auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                        // (AR) فك وسم 2-bit: مسح bit63 و bit62
                        llvm::Value *clearMask = llvm::ConstantInt::get(i64Ty, ~kAdtPayloadTagMask);
                        llvm::Value *cleanVal = cg_.builder_->CreateAnd(v, clearMask, "tup.print.clean");

                        // (AR) فحص bit62 للتمييز بين رقم ومنطقي
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(i64Ty, kAdtPayloadBit62);
                        llvm::Value *bit62 = cg_.builder_->CreateAnd(v, bit62Mask, "tup.print.bit62");
                        llvm::Value *isBool = cg_.builder_->CreateICmpNE(
                            bit62, llvm::ConstantInt::get(i64Ty, 0), "tup.print.isbool");

                        // (AR) ISSUE-076/084: علَم الصندوق العشريّ (إن ضُبِط من الاستخراج) يتقدّم
                        //      على المؤشّر/الرقم/المنطقيّ. غيابه ⇒ ثابت false (لا فرع عشريّ).
                        // (EN) ISSUE-076/084: the boxed-float flag (if set by extraction) takes
                        //      precedence over ptr/int/bool. Absent ⇒ constant false (no float arm).
                        auto isFloatIt = cg_.context_info_.namedValues.find(op.name + ".__is_float");
                        llvm::Value *isFloat = (isFloatIt != cg_.context_info_.namedValues.end())
                                                   ? isFloatIt->second
                                                   : llvm::ConstantInt::getFalse(*cg_.context_);

                        auto *parentFunc = cg_.builder_->GetInsertBlock()->getParent();
                        auto *floatBB = llvm::BasicBlock::Create(*cg_.context_, "tup.print.float", parentFunc);
                        auto *notFloatBB = llvm::BasicBlock::Create(*cg_.context_, "tup.print.notfloat", parentFunc);
                        auto *ptrBB = llvm::BasicBlock::Create(*cg_.context_, "tup.print.ptr", parentFunc);
                        auto *nonPtrBB = llvm::BasicBlock::Create(*cg_.context_, "tup.print.nonptr", parentFunc);
                        auto *boolBB = llvm::BasicBlock::Create(*cg_.context_, "tup.print.bool", parentFunc);
                        auto *numBB = llvm::BasicBlock::Create(*cg_.context_, "tup.print.num", parentFunc);
                        auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "tup.print.merge", parentFunc);

                        cg_.builder_->CreateCondBr(isFloat, floatBB, notFloatBB);

                        // (AR) صندوق عشريّ: امسح bit62 ⇒ مؤشّر ⇒ حمّل double ⇒ __sad_print_double
                        // (EN) Boxed float: clear bit62 ⇒ pointer ⇒ load double ⇒ __sad_print_double
                        cg_.builder_->SetInsertPoint(floatBB);
                        {
                            llvm::Value *boxI64 = cg_.builder_->CreateAnd(
                                v, llvm::ConstantInt::get(i64Ty, ~kAdtPayloadBit62), "tup.print.float.clear");
                            llvm::Value *boxPtr = cg_.builder_->CreateIntToPtr(boxI64, ptrTy, "tup.print.float.ptr");
                            llvm::Value *d = cg_.builder_->CreateLoad(
                                llvm::Type::getDoubleTy(*cg_.context_), boxPtr, "tup.print.float.load");
                            llvm::FunctionType *printDoubleTy = llvm::FunctionType::get(
                                llvm::Type::getVoidTy(*cg_.context_),
                                {llvm::Type::getDoubleTy(*cg_.context_)}, false);
                            llvm::FunctionCallee printDoubleFn =
                                cg_.module_->getOrInsertFunction("__sad_print_double", printDoubleTy);
                            cg_.builder_->CreateCall(printDoubleFn, {d});
                        }
                        cg_.builder_->CreateBr(mergeBB);

                        cg_.builder_->SetInsertPoint(notFloatBB);
                        cg_.builder_->CreateCondBr(isPtr, ptrBB, nonPtrBB);

                        // (AR) مسار المؤشر (نص): inttoptr → printf %s
                        cg_.builder_->SetInsertPoint(ptrBB);
                        llvm::Value *strPtr = cg_.builder_->CreateIntToPtr(cleanVal, ptrTy, "tup.print.str");
                        llvm::Value *fmtSPtr = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                        llvm::Value *ptrIsNull = cg_.builder_->CreateICmpEQ(
                            strPtr, llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                            "tup.print.null");
                        llvm::Value *nullFallback = cg_.builder_->CreateGlobalStringPtr(
                            "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "null.fallback"); // لاشيء
                        llvm::Value *safeStr = cg_.builder_->CreateSelect(ptrIsNull, nullFallback, strPtr, "tup.print.safe");
                        cg_.builder_->CreateCall(printfFunc, {fmtSPtr, safeStr});
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) مسار غير المؤشر: فحص منطقي أم رقم
                        cg_.builder_->SetInsertPoint(nonPtrBB);
                        cg_.builder_->CreateCondBr(isBool, boolBB, numBB);

                        // (AR) مسار المنطقي: طباعة صحيح/خطأ
                        cg_.builder_->SetInsertPoint(boolBB);
                        llvm::Value *fmtSBool = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                        llvm::Value *boolCond = cg_.builder_->CreateICmpNE(
                            cleanVal, llvm::ConstantInt::get(i64Ty, 0), "tup.print.boolcond");
                        llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "tup.bool.true"); // صحيح
                        llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xae\xd8\xb7\xd8\xa3", "tup.bool.false"); // خطأ
                        llvm::Value *boolStr = cg_.builder_->CreateSelect(boolCond, trueStr, falseStr, "tup.print.boolstr");
                        cg_.builder_->CreateCall(printfFunc, {fmtSBool, boolStr});
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) مسار الرقم: مسح MSB وطباعة %lld
                        cg_.builder_->SetInsertPoint(numBB);
                        llvm::Value *fmtD = cg_.builder_->CreateGlobalStringPtr("%lld", "fmt.d");
                        cg_.builder_->CreateCall(printfFunc, {fmtD, cleanVal});
                        cg_.builder_->CreateBr(mergeBB);

                        cg_.builder_->SetInsertPoint(mergeBB);
                    }
                    else if (op.dataType == SadTypeKind::UInt64 ||
                             op.dataType == SadTypeKind::Byte)
                    {
                        // ════════════════════════════════════════════════════════════
                        // (AR) [إصلاح تصادم kSadNullSentinel] طبيعي64/بايت **لا يكونان نوعَ
                        //      العدم أبدًا** (العدمُ يُخزَّن ثابتَ i64 نوعُه Integer)، فلا نطبّق
                        //      فحص حارس العدم عليهما: الحارس قيمةٌ i64 بعينها (INT64_MIN+1 =
                        //      2^63+1 لا-موقَّعًا) وطبيعي64 شرعيّ قد يساويها ⇒ كان يُطبع «لاشيء»
                        //      بدل الرقم. نطبع مباشرةً (%llu لطبيعي64، %lld لبايت) مطابقةً للمفسّر.
                        //      Integer **مُستثنى** من هذا التخطّي: يتصادم جوهريًّا مع العدم
                        //      (كلاهما Integer) فلا يُميَّز — دَينُ تمثيل عدمٍ خارج النطاق.
                        // (EN) [kSadNullSentinel collision fix] طبيعي64/Byte are NEVER the null
                        //      type (null is stored as an Integer-typed i64 constant), so the null
                        //      sentinel check is not applied to them: the sentinel is one specific
                        //      i64 value (INT64_MIN+1 = 2^63+1 unsigned) a legitimate طبيعي64 can
                        //      equal ⇒ it used to print «لاشيء» instead of the number. Print
                        //      directly (%llu for طبيعي64, %lld for Byte), matching the interpreter.
                        //      Integer is EXCLUDED from this skip: it collides intrinsically with
                        //      null (both Integer) and can't be distinguished — an out-of-band-null
                        //      representation debt.
                        // ════════════════════════════════════════════════════════════
                        llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr(
                            (op.dataType == SadTypeKind::UInt64) ? "%llu" : "%lld", "fmt.dnn");
                        cg_.builder_->CreateCall(printfFunc, {fmt, v});
                    }
                    else
                    {
                        // (AR) طباعة قيمة null-sentinel كنص "لاشيء" بدلاً من الرقم الخام.
                        // (EN) Print null sentinel as "لاشيء" instead of raw integer.
                        llvm::Value *isNullSentinel = cg_.builder_->CreateICmpEQ(
                            v,
                            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), Sad::Compiler::kSadNullSentinel),
                            "print.is_null_sentinel");

                        auto *parentFunc = cg_.builder_->GetInsertBlock()->getParent();
                        auto *nullBB = llvm::BasicBlock::Create(*cg_.context_, "print_null", parentFunc);
                        auto *numBB = llvm::BasicBlock::Create(*cg_.context_, "print_num", parentFunc);
                        auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "print_merge", parentFunc);

                        cg_.builder_->CreateCondBr(isNullSentinel, nullBB, numBB);

                        cg_.builder_->SetInsertPoint(nullBB);
                        llvm::Value *fmtS = cg_.builder_->CreateGlobalStringPtr("%s", "fmt.s");
                        llvm::Value *nullStr = cg_.builder_->CreateGlobalStringPtr("\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "null.str"); // لاشيء
                        cg_.builder_->CreateCall(printfFunc, {fmtS, nullStr});
                        cg_.builder_->CreateBr(mergeBB);

                        cg_.builder_->SetInsertPoint(numBB);
                        // (AR) [طبقة طبيعي64 — الخطوة ٤] طبع مباشر لا-موقَّع: نمط بتّات
                        //      طبيعي64 يُنسَّق %llu (يُطابق المفسّر renderUnsignedArgs ومسار
                        //      نص() في emitI64ToString). Byte [0،255] فطباعته الموقَّعة صحيحة.
                        // (EN) [طبيعي64 layer — Step 4] Unsigned direct print: طبيعي64's bit
                        //      pattern formats %llu (mirrors the interpreter renderUnsignedArgs
                        //      and the نص() path in emitI64ToString). Byte [0,255] prints fine signed.
                        llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr(
                            (op.dataType == SadTypeKind::UInt64) ? "%llu" : "%lld", "fmt.d");
                        cg_.builder_->CreateCall(printfFunc, {fmt, v});
                        cg_.builder_->CreateBr(mergeBB);

                        cg_.builder_->SetInsertPoint(mergeBB);
                    }
                }
                else if (v->getType()->isDoubleTy())
                {
                    // (AR) [إصلاح نوع الطباعة]
                    //      إذا كان نوع SIR هو Integer، فالقيمة المخزّنة كـ double هي في الحقيقة عدد صحيح
                    //      (مثال: دالة تُرجع 42 لكن LLVM يخزّنها كـ double 42.0)
                    //      → نحوّلها إلى i64 ونطبع كـ %lld بدون .0
                    //      أما إذا كان نوع SIR هو Float/غير معروف → نطبع كـ double مع .0 عند الحاجة
                    // (EN) [Print type fix]
                    //      If SIR type is Integer, the double value is logically an integer
                    //      (e.g. function returning 42 stored as double 42.0 in LLVM)
                    //      → convert to i64 and print as %lld without .0
                    //      If SIR type is Float/unknown → print as double with .0 when needed
                    if (op.dataType == SadTypeKind::Integer)
                    {
                        // (AR) عدد صحيح مخزّن كـ double في LLVM — طباعة كـ %lld
                        // (EN) Integer stored as double in LLVM — print as %lld
                        llvm::Value *asInt = cg_.builder_->CreateFPToSI(v, llvm::Type::getInt64Ty(*cg_.context_), "print.dbl2int");
                        llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "fmt.d");
                        cg_.builder_->CreateCall(printfFunc, {fmt, asInt});
                    }
                    else
                    {
                        // (AR) عدد عشري حقيقي — استخدام __sad_print_double مع .0 للأعداد الصحيحة
                        // (EN) Real float — use __sad_print_double with .0 for whole numbers
                        llvm::FunctionType *printDoubleTy = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_),
                            {llvm::Type::getDoubleTy(*cg_.context_)},
                            false);
                        llvm::FunctionCallee printDoubleFn = cg_.module_->getOrInsertFunction("__sad_print_double", printDoubleTy);
                        cg_.builder_->CreateCall(printDoubleFn, {v});
                    }
                }
                else
                {
                    llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "fmt.d");
                    llvm::Value *conv = cg_.builder_->CreateIntCast(v, llvm::Type::getInt64Ty(*cg_.context_), true);
                    cg_.builder_->CreateCall(printfFunc, {fmt, conv});
                }
            }
            // (AR) إرجاع قيمة sentinel (0) بدلاً من nullptr
            //      لأن nullptr يُفسَّره الـ dispatcher على أنه "opcode غير مدعوم"
            //      بينما الطباعة نُفّذت بنجاح — هي فقط عملية void بدون قيمة مُرجعة
            // (EN) Return sentinel (0) instead of nullptr
            //      nullptr is interpreted by dispatcher as "unsupported opcode"
            //      but print executed successfully — it's just a void operation
            return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0);
        }



}} // namespace Sad::LLVM
