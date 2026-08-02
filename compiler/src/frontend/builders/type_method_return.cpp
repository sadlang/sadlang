// ============================================================================
// type_method_return.cpp
// ============================================================================
// (AR) تنفيذُ مرجعِ نوعِ عائدِ طرق الأنواع المدمجة. الأسماءُ من السجلّ المُولَّد
//      عن language-truth، والأنواعُ مطابقةٌ لما تبعثه بُناةُ الطرق في
//      method_call_string_map.cpp و method_call_array_basic.cpp.
// (EN) Implementation of the built-in type-method return-type reference.
// ============================================================================

#include "builders/type_method_return.h"

#include "builtin_registry.h"

namespace TM = Sad::Builtins::Names::TypeMethods;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            namespace
            {
                // (AR) عائدٌ بسيطٌ بلا نوعِ عنصر
                TypeMethodReturn simple(SadTypeKind kind)
                {
                    TypeMethodReturn r;
                    r.known = true;
                    r.kind = kind;
                    return r;
                }

                // (AR) عائدٌ مصفوفةٌ بنوع عنصرٍ معلوم
                TypeMethodReturn arrayOf(SadTypeKind elementKind)
                {
                    TypeMethodReturn r;
                    r.known = true;
                    r.kind = SadTypeKind::Array;
                    r.elementKind = elementKind;
                    return r;
                }

                // (AR) طرقُ النصّ / String methods
                TypeMethodReturn stringMethod(const std::string &m)
                {
                    if (m == TM::String::SPLIT)
                        return arrayOf(SadTypeKind::String);
                    if (m == TM::String::REPLACE || m == TM::String::TRIM ||
                        m == TM::String::SUBSTRING || m == TM::String::TO_UPPER ||
                        m == TM::String::TO_LOWER || m == TM::String::REPEAT ||
                        m == TM::String::CHAR_AT || m == TM::String::REVERSE ||
                        m == TM::String::COPY)
                        return simple(SadTypeKind::String);
                    if (m == TM::String::CONTAINS || m == TM::String::STARTS_WITH ||
                        m == TM::String::ENDS_WITH)
                        return simple(SadTypeKind::Boolean);
                    if (m == TM::String::LENGTH || m == TM::String::FIND)
                        return simple(SadTypeKind::Integer);
                    return TypeMethodReturn{};
                }

                // (AR) طرقُ المصفوفة / Array methods
                TypeMethodReturn arrayMethod(const std::string &m)
                {
                    if (m == TM::Array::CONTAINS || m == TM::Array::IS_EMPTY ||
                        m == TM::Array::EVERY || m == TM::Array::SOME)
                        return simple(SadTypeKind::Boolean);
                    if (m == TM::Array::LENGTH || m == TM::Array::INDEX_OF ||
                        m == TM::Array::FIND_INDEX || m == TM::Array::COUNT ||
                        m == TM::Array::DIMENSION)
                        return simple(SadTypeKind::Integer);
                    if (m == TM::Array::JOIN)
                        return simple(SadTypeKind::String);
                    if (m == TM::Array::SLICE || m == TM::Array::MAP ||
                        m == TM::Array::FILTER || m == TM::Array::SORT ||
                        m == TM::Array::REVERSE || m == TM::Array::FLATTEN ||
                        m == TM::Array::UNIQUE || m == TM::Array::CONCAT ||
                        m == TM::Array::ZIP || m == TM::Array::COPY)
                        return arrayOf(SadTypeKind::Unknown);
                    return TypeMethodReturn{};
                }

                // (AR) طرقُ الخريطة / Map methods
                TypeMethodReturn mapMethod(const std::string &m)
                {
                    if (m == TM::Map::CONTAINS || m == TM::Map::IS_EMPTY ||
                        m == TM::Map::HAS_KEY)
                        return simple(SadTypeKind::Boolean);
                    if (m == TM::Map::SIZE)
                        return simple(SadTypeKind::Integer);
                    if (m == TM::Map::KEYS)
                        return arrayOf(SadTypeKind::String);
                    if (m == TM::Map::VALUES || m == TM::Map::ENTRIES)
                        return arrayOf(SadTypeKind::Unknown);
                    // (AR) احصل تُعيد قيمةً مجهولةَ النوع ⇒ Any كي تعبر موسومةً
                    // (EN) get returns a statically unknown value ⇒ Any so it stays tagged
                    if (m == TM::Map::GET)
                        return simple(SadTypeKind::Any);
                    return TypeMethodReturn{};
                }
            } // namespace

            TypeMethodReturn typeMethodReturnKind(SadTypeKind objectKind,
                                                  const std::string &methodName)
            {
                switch (objectKind)
                {
                case SadTypeKind::String:
                    return stringMethod(methodName);
                case SadTypeKind::Array:
                    return arrayMethod(methodName);
                case SadTypeKind::Map:
                    return mapMethod(methodName);
                default:
                    return TypeMethodReturn{};
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
