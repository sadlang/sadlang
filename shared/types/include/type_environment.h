// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: shared/types/include/type_environment.h
// الوصف: بيئةُ الأنواع — خريطةُ نطاقٍ (اسم → نوع) بسلسلةِ آباء.
//
// (AR) 🔑 **وكان هذا الصنفُ ساكنًا في `type_inferencer.h`** مع آلةِ استدلالٍ
//      كاملة (`TypeInferencer` · `InferenceContext`). وقِيسَ أنّ الآلةَ
//      **بلا مُنادٍ واحدٍ من الإنتاج** بينما `TypeEnvironment` وحدَه حيٌّ
//      (مكدَّسُ نطاقاتِ `TypeChecker`). أمرُ القياس:
//        grep -rn "TypeInferencer\|InferenceContext" --include=*.cpp --include=*.h //          shared compiler apps tools | grep -v shared/type_system   ⇒ صفر
//      فحُذِفَت الآلةُ ونُقِلَ الحيُّ إلى ملفِّه — **الميّتُ يُحذَفُ ولا يُنقَل**.
// (EN) TypeEnvironment used to live inside type_inferencer.h next to a full
//      inference engine with zero production callers. The engine was deleted;
//      the one live class moved here.
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_TYPE_SYSTEM_TYPE_ENVIRONMENT_H
#define SAD_TYPE_SYSTEM_TYPE_ENVIRONMENT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "sad_type_system.h"

namespace Sad {
namespace Types {

class TypeEnvironment : public std::enable_shared_from_this<TypeEnvironment> {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ افتراضي - بيئة فارغة / Default constructor - empty environment
    TypeEnvironment();
    
    // منشئ مع بيئة أب / Constructor with parent environment
    // المعامل / Parameter: parent - البيئة الأب / Parent environment
    explicit TypeEnvironment(std::shared_ptr<TypeEnvironment> parent);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الربط والبحث / Binding and Lookup
    // ═══════════════════════════════════════════════════════════════════════════
    
    // ربط اسم بنوع / Bind name to type
    // المعامل / Parameter: name - اسم المتغير / Variable name
    // المعامل / Parameter: type - النوع / Type
    void bind(const std::string& name, SadTypePtr type);
    
    // البحث عن نوع / Look up type
    // المعامل / Parameter: name - اسم المتغير / Variable name
    // الإرجاع / Return: النوع أو nullptr إذا لم يُوجد / Type or nullptr if not found
    SadTypePtr lookup(const std::string& name) const;
    
    // التحقق من وجود ربط / Check if binding exists
    // المعامل / Parameter: name - اسم المتغير / Variable name
    // الإرجاع / Return: true إذا وُجد / true if found
    bool contains(const std::string& name) const;
    
    // إزالة ربط / Remove binding
    // المعامل / Parameter: name - اسم المتغير / Variable name
    void unbind(const std::string& name);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إدارة النطاق / Scope Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    // إنشاء بيئة فرعية / Create child environment
    // الإرجاع / Return: بيئة جديدة تشير إلى هذه البيئة كأب / New environment with this as parent
    std::shared_ptr<TypeEnvironment> createChild();
    
    // الحصول على البيئة الأب / Get parent environment
    // الإرجاع / Return: البيئة الأب أو nullptr / Parent environment or nullptr
    std::shared_ptr<TypeEnvironment> getParent() const { return parent_; }
    
    // مسح البيئة / Clear environment
    void clear();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الاستعلام / Query
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد الروابط / Get number of bindings
    size_t size() const { return bindings_.size(); }
    
    // التحقق من كون البيئة فارغة / Check if environment is empty
    bool isEmpty() const { return bindings_.empty(); }
    
    // الحصول على جميع الأسماء / Get all names
    std::vector<std::string> getAllNames() const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // خريطة الروابط: اسم -> نوع / Bindings map: name -> type
    std::unordered_map<std::string, SadTypePtr> bindings_;
    
    // البيئة الأب (للنطاقات المتداخلة) / Parent environment (for nested scopes)
    std::shared_ptr<TypeEnvironment> parent_;
};

} // namespace Types
} // namespace Sad

#endif // SAD_TYPE_SYSTEM_TYPE_ENVIRONMENT_H
