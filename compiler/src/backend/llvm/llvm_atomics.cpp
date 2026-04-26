// ======================================================================
// llvm_atomics.cpp - توليد كود العمليات الذرية / Atomic Codegen
// ======================================================================
// الوصف بالعربية:
//   توليد LLVM IR للعمليات الذرية
//   يدعم: load, store, exchange, CAS, fetch_add, etc.
//
// English Description:
//   LLVM IR generation for atomic operations
//   Supports: load, store, exchange, CAS, fetch_add, etc.
// ======================================================================

#include "types/atomic_types.h"
#include <string>
#include <sstream>

namespace Sad {
namespace Backend {

/**
 * @brief Generate LLVM IR for atomic operations
 * (AR) توليد LLVM IR للعمليات الذرية
 */
class LLVMAtomicCodegen {
public:
    /**
     * @brief Generate atomic load / توليد تحميل ذرّي
     * 
     * @example ص: عداد.حمّل(ترتيب::اكتساب)
     * LLVM: %val = load atomic i64, i64* %ptr acquire, align 8
     */
    static std::string generateAtomicLoad(const std::string& resultVar,
                                           const std::string& ptrVar,
                                           const std::string& type,
                                           Types::MemoryOrdering ordering) {
        std::ostringstream ir;
        ir << resultVar << " = load atomic " << type << ", " << type 
           << "* " << ptrVar << " " << Types::orderingToLLVM(ordering) 
           << ", align " << getAlignment(type) << "\n";
        return ir.str();
    }
    
    /**
     * @brief Generate atomic store / توليد تخزين ذرّي
     * 
     * @example ص: عداد.خزّن(42، ترتيب::إطلاق)
     * LLVM: store atomic i64 42, i64* %ptr release, align 8
     */
    static std::string generateAtomicStore(const std::string& value,
                                            const std::string& ptrVar,
                                            const std::string& type,
                                            Types::MemoryOrdering ordering) {
        std::ostringstream ir;
        ir << "store atomic " << type << " " << value << ", " << type 
           << "* " << ptrVar << " " << Types::orderingToLLVM(ordering)
           << ", align " << getAlignment(type) << "\n";
        return ir.str();
    }
    
    /**
     * @brief Generate atomic exchange / توليد تبديل ذرّي
     * 
     * @example ص: دع قديم = عداد.بدّل(100)
     * LLVM: %old = atomicrmw xchg i64* %ptr, i64 100 seq_cst
     */
    static std::string generateAtomicExchange(const std::string& resultVar,
                                               const std::string& ptrVar,
                                               const std::string& newValue,
                                               const std::string& type,
                                               Types::MemoryOrdering ordering) {
        std::ostringstream ir;
        ir << resultVar << " = atomicrmw xchg " << type << "* " << ptrVar 
           << ", " << type << " " << newValue << " " 
           << Types::orderingToLLVM(ordering) << "\n";
        return ir.str();
    }
    
    /**
     * @brief Generate compare-and-swap / توليد قارن وبدّل
     * 
     * @example ص: دع (نجح، قديم) = عداد.قارن_وبدّل(متوقع، جديد)
     * LLVM: %res = cmpxchg i64* %ptr, i64 %expected, i64 %new seq_cst seq_cst
     */
    static std::string generateCAS(const std::string& resultVar,
                                    const std::string& ptrVar,
                                    const std::string& expected,
                                    const std::string& desired,
                                    const std::string& type,
                                    Types::MemoryOrdering successOrd,
                                    Types::MemoryOrdering failureOrd) {
        std::ostringstream ir;
        ir << resultVar << " = cmpxchg " << type << "* " << ptrVar 
           << ", " << type << " " << expected << ", " << type << " " << desired 
           << " " << Types::orderingToLLVM(successOrd)
           << " " << Types::orderingToLLVM(failureOrd) << "\n";
        return ir.str();
    }
    
    /**
     * @brief Generate atomic fetch-add / توليد جلب وأضف
     * 
     * @example ص: عداد.أضف(1، ترتيب::تسلسلي)
     * LLVM: %old = atomicrmw add i64* %ptr, i64 1 seq_cst
     */
    static std::string generateAtomicRMW(const std::string& resultVar,
                                          const std::string& op,
                                          const std::string& ptrVar,
                                          const std::string& value,
                                          const std::string& type,
                                          Types::MemoryOrdering ordering) {
        std::ostringstream ir;
        ir << resultVar << " = atomicrmw " << op << " " << type << "* " 
           << ptrVar << ", " << type << " " << value << " " 
           << Types::orderingToLLVM(ordering) << "\n";
        return ir.str();
    }
    
    /**
     * @brief Generate memory fence / توليد سياج ذاكرة
     * 
     * @example ص: سياج_ذاكرة(ترتيب::تسلسلي)
     * LLVM: fence seq_cst
     */
    static std::string generateFence(Types::MemoryOrdering ordering) {
        return "fence " + Types::orderingToLLVM(ordering) + "\n";
    }
    
    /**
     * @brief Map Arabic atomic operation to LLVM atomicrmw op
     * (AR) تحويل اسم العملية الذرية العربي لعملية LLVM
     */
    static std::string arabicOpToLLVM(const std::string& arabicOp) {
        if (arabicOp == "أضف" || arabicOp == "add") return "add";
        if (arabicOp == "اطرح" || arabicOp == "sub") return "sub";
        if (arabicOp == "و" || arabicOp == "and") return "and";
        if (arabicOp == "أو" || arabicOp == "or") return "or";
        if (arabicOp == "حصري" || arabicOp == "xor") return "xor";
        if (arabicOp == "الأصغر" || arabicOp == "min") return "min";
        if (arabicOp == "الأكبر" || arabicOp == "max") return "max";
        if (arabicOp == "بدّل" || arabicOp == "xchg") return "xchg";
        return "add"; // default
    }
    
private:
    static int getAlignment(const std::string& type) {
        if (type == "i8") return 1;
        if (type == "i16") return 2;
        if (type == "i32") return 4;
        if (type == "i64" || type == "ptr") return 8;
        return 8;
    }
};

} // namespace Backend
} // namespace Sad
