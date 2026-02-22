// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file llvm_drop_glue.h
 * @brief نظام توليد كود التنظيف (Drop Glue) - لغة ص
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * @phase م-هـ03: توليد كود Drop/Destructor بدون GC
 * 
 * =============================================================================
 *                    نظام التنظيف التلقائي (Drop Glue System)
 * =============================================================================
 * 
 *     عند خروج متغير من النطاق في وضع الملكية الصارمة:
 *     
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │  1. يُستدعى هدم() (destructor) إذا كان النوع يملك موارد       │
 *     │  2. التنظيف بترتيب عكسي (LIFO) — آخر متغير أُنشئ أول مُدمَّر  │
 *     │  3. الأنواع البدائية (ع32, منطق) لا تحتاج تنظيف                  │
 *     │  4. القيم المنقولة (*نقل*) لا يُستدعى لها drop                  │
 *     │  5. البنى تُهدم حقولها بترتيب عكسي                               │
 *     └─────────────────────────────────────────────────────────────────────┘
 * 
 *     مثال:
 *     @code
 *     {
 *         متغير أ = مورد::جديد();    // ← مُنشأ أولاً
 *         متغير ب = مورد::جديد();    // ← مُنشأ ثانياً
 *     }   // ← ب.هدم() يُستدعى أولاً، ثم أ.هدم()
 *     @endcode
 */

#ifndef SAD_LLVM_DROP_GLUE_H
#define SAD_LLVM_DROP_GLUE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

#include <llvm/IR/IRBuilder.h>

// LLVM Forward declarations
namespace llvm {
    class LLVMContext;
    class Module;
    class Function;
    class Value;
    class Type;
    class BasicBlock;
    class AllocaInst;
}

namespace Sad {
namespace LLVM {

// =============================================================================
//                    أوصاف الأنواع القابلة للتنظيف
//                    Droppable Type Descriptors
// =============================================================================

/**
 * @enum DropKind
 * @brief نوع عملية التنظيف / Kind of drop operation
 */
enum class DropKind {
    /// لا حاجة للتنظيف (أنواع بدائية: ع8, ع16, ع32, ع64, منطق, حرف, ...)
    None,
    
    /// استدعاء دالة هدم() مخصصة / Custom destructor call
    CustomDestructor,
    
    /// تحرير ذاكرة مُخصصة / Deallocate heap memory
    Deallocate,
    
    /// تنظيف البنية: هدم كل حقل بترتيب عكسي / Struct: drop each field in reverse
    StructFields,
    
    /// تنظيف مصفوفة: هدم كل عنصر / Array: drop each element
    ArrayElements,
    
    /// تنظيف عنصر تعداد: فحص التمييز ثم هدم / Enum: check discriminant then drop
    EnumVariant,
};

/**
 * @struct DropTypeInfo
 * @brief معلومات التنظيف لنوع معين / Drop information for a specific type
 */
struct DropTypeInfo {
    /// اسم النوع (عربي أو إنجليزي)
    std::string typeName;
    
    /// نوع عملية التنظيف
    DropKind dropKind = DropKind::None;
    
    /// اسم دالة الهدم المخصصة (إذا كان dropKind == CustomDestructor)
    std::string destructorName;
    
    /// أنواع الحقول (إذا كان dropKind == StructFields)
    std::vector<DropTypeInfo> fieldTypes;
    
    /// عدد العناصر (إذا كان dropKind == ArrayElements)
    size_t arraySize = 0;
    
    /// نوع عنصر المصفوفة
    std::shared_ptr<DropTypeInfo> elementType;
    
    /// هل النوع بدائي (لا يحتاج تنظيف)؟
    bool isPrimitive() const { return dropKind == DropKind::None; }
    
    /// هل يحتاج تنظيف؟
    bool needsDrop() const { return dropKind != DropKind::None; }
};

// =============================================================================
//                    متغيرات في النطاق
//                    Scoped Variables
// =============================================================================

/**
 * @struct ScopedVariable
 * @brief متغير في نطاق مع معلومات التنظيف / Variable in scope with drop info
 */
struct ScopedVariable {
    /// اسم المتغير
    std::string name;
    
    /// مؤشر LLVM للتخصيص (alloca)
    llvm::AllocaInst* alloca = nullptr;
    
    /// معلومات التنظيف
    DropTypeInfo dropInfo;
    
    /// هل تم نقل القيمة (لا تحتاج drop)؟
    bool isMoved = false;
    
    /// رقم السطر (للتنقيح)
    size_t sourceLine = 0;
    
    /// علم التنظيف (drop flag) — alloca لقيمة i1: true = يحتاج drop
    llvm::AllocaInst* dropFlag = nullptr;
};

/**
 * @struct DropScope
 * @brief نطاق يحتوي متغيرات تحتاج تنظيف / Scope with droppable variables
 */
struct DropScope {
    size_t scopeId = 0;
    std::vector<ScopedVariable> variables;
};

// =============================================================================
//                    مولّد كود التنظيف
//                    Drop Glue Code Generator
// =============================================================================

/**
 * @class LLVMDropGlue
 * @brief مولّد كود التنظيف (Drop Glue) لـ LLVM IR
 * 
 *     يُولد كود الهدم التلقائي عند نهاية النطاقات:
 *     - دوال __sad_drop_<TypeName> لكل نوع يحتاج تنظيف
 *     - استدعاءات drop عند نهاية النطاقات (LIFO)
 *     - أعلام drop للقيم التي قد تُنقل شرطياً
 *     - دعم الأنواع المركبة (بنى، مصفوفات، تعدادات)
 */
class LLVMDropGlue {
public:
    // =========================================================================
    //                    البناء / Construction
    // =========================================================================
    
    /**
     * @brief إنشاء مولّد drop glue
     * @param context سياق LLVM
     * @param module وحدة LLVM
     */
    LLVMDropGlue(llvm::LLVMContext& context, llvm::Module& module);
    ~LLVMDropGlue();
    
    // =========================================================================
    //                    تسجيل الأنواع / Type Registration
    // =========================================================================
    
    /**
     * @brief تسجيل نوع يحتاج تنظيف
     * @param info معلومات التنظيف
     * 
     *     يُولّد دالة __sad_drop_<TypeName> للنوع المسجل
     */
    void registerDroppableType(const DropTypeInfo& info);
    
    /**
     * @brief التحقق مما إذا كان النوع يحتاج تنظيف
     */
    bool needsDrop(const std::string& typeName) const;
    
    /**
     * @brief الحصول على معلومات التنظيف لنوع مسجل
     */
    const DropTypeInfo* getDropInfo(const std::string& typeName) const;
    
    // =========================================================================
    //                    إدارة النطاقات / Scope Management
    // =========================================================================
    
    /**
     * @brief دخول نطاق جديد
     * @return معرف النطاق
     */
    size_t enterScope();
    
    /**
     * @brief تسجيل متغير في النطاق الحالي
     * @param var معلومات المتغير
     */
    void registerVariable(const ScopedVariable& var);
    
    /**
     * @brief تعليم متغير كمنقول (لا يحتاج drop)
     * @param varName اسم المتغير
     */
    void markMoved(const std::string& varName);
    
    /**
     * @brief خروج من النطاق — يُولّد drop calls بترتيب LIFO
     * @param builder مولّد LLVM IR
     * 
     *     مثال الكود المُولّد:
     *     @code
     *     ; خروج من النطاق — تنظيف LIFO
     *     %dropflag.b = load i1, ptr %dropflag.b.addr
     *     br i1 %dropflag.b, label %drop.b, label %skip.b
     *     drop.b:
     *       call void @__sad_drop_مورد(ptr %b)
     *       br label %skip.b
     *     skip.b:
     *       call void @__sad_drop_مورد(ptr %a)
     *     @endcode
     */
    void emitScopeExit(llvm::IRBuilder<>& builder);
    
    // =========================================================================
    //                    توليد دوال Drop / Drop Function Generation
    // =========================================================================
    
    /**
     * @brief توليد جميع دوال drop المسجلة
     * 
     *     يُستدعى بعد تسجيل جميع الأنواع لتوليد:
     *     - __sad_drop_<TypeName> لكل نوع
     *     - دعم الأنواع المتداخلة (بنية تحتوي بنية)
     */
    void generateAllDropFunctions(llvm::IRBuilder<>& builder);
    
    /**
     * @brief توليد استدعاء drop لمتغير واحد
     * @param builder مولّد IR
     * @param var المتغير
     */
    void emitDropCall(llvm::IRBuilder<>& builder,
                      const ScopedVariable& var);
    
    /**
     * @brief توليد علم drop (drop flag) لمتغير
     * @param builder مولّد IR
     * @param varName اسم المتغير
     * @return مؤشر لعلم التنظيف (i1 alloca)
     * 
     *     أعلام التنظيف تُستخدم للمتغيرات التي قد تُنقل في فرع واحد فقط:
     *     @code
     *     إذا شرط {
     *         *نقل* س   // س مُنقل هنا
     *     }
     *     // هل نحتاج drop لـ س؟ يعتمد على المسار!
     *     @endcode
     */
    llvm::AllocaInst* emitDropFlag(llvm::IRBuilder<>& builder,
                                    const std::string& varName);

private:
    llvm::LLVMContext& context_;
    llvm::Module& module_;
    
    /// الأنواع المسجلة / Registered types
    std::unordered_map<std::string, DropTypeInfo> registeredTypes_;
    
    /// مكدس النطاقات / Scope stack
    std::vector<DropScope> scopeStack_;
    
    /// عداد النطاقات
    size_t nextScopeId_ = 0;
    
    /// دوال drop المُولّدة / Generated drop functions
    std::unordered_map<std::string, llvm::Function*> dropFunctions_;
    
    /// أنواع بدائية لا تحتاج تنظيف
    std::unordered_set<std::string> primitiveTypes_;
    
    // =========================================================================
    //                    دوال داخلية / Internal Functions
    // =========================================================================
    
    /**
     * @brief هل النوع بدائي (لا يحتاج drop)؟
     */
    bool isPrimitiveType(const std::string& typeName) const;
    
    /**
     * @brief توليد دالة drop لنوع واحد
     */
    llvm::Function* generateDropFunction(const DropTypeInfo& info);
    
    /**
     * @brief توليد كود drop لبنية (هدم الحقول بترتيب عكسي)
     */
    void emitStructDrop(llvm::IRBuilder<>& builder,
                        llvm::Value* structPtr,
                        const DropTypeInfo& info);
    
    /**
     * @brief توليد كود drop لمصفوفة (هدم كل عنصر)
     */
    void emitArrayDrop(llvm::IRBuilder<>& builder,
                       llvm::Value* arrayPtr,
                       const DropTypeInfo& info);
    
    /**
     * @brief الحصول على اسم دالة drop لنوع
     */
    std::string getDropFunctionName(const std::string& typeName) const;
};

} // namespace LLVM
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_LLVM_DROP_GLUE_H
