/**
 * @file pattern_nodes.h
 * @brief (AR) عقد AST لـ Pattern Matching / (EN) AST Nodes for Pattern Matching
 *
 * (AR) يحتوي على تعريفات عقد AST للأنماط ومطابقة الأنماط
 * (EN) Contains AST node definitions for patterns and pattern matching
 *
 * @author Sad Language Team
 * @date December 19, 2025
 * @version 2.0.0
 */

#pragma once

#include "ast_node.h"
#include "ast_visitor.h"
#include "expressions.h"
#include "statements.h"
#include "value.h"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace Sad
{
    namespace AST
    {

        // Forward declarations
        class Pattern;
        struct CaseClause;

        // ============================================================================
        // (AR) النمط الأساسي / (EN) Base Pattern
        // ============================================================================

        /**
         * @brief (AR) النمط الأساسي - فئة مجردة / (EN) Base Pattern - Abstract Class
         *
         * (AR) تمثل نمط في match statement يمكن مطابقته مع قيمة
         * (EN) Represents a pattern in match statement that can be matched against a value
         */
        class Pattern
        {
        public:
            virtual ~Pattern() = default;

            /**
             * @brief (AR) محاولة مطابقة النمط مع قيمة / (EN) Try to match pattern with value
             *
             * @param value (AR) القيمة المُختبرة / (EN) Value to test
             * @param bindings (AR) map للمتغيرات المربوطة / (EN) map for bound variables
             * @return (AR) true إذا تطابق / (EN) true if matched
             */
            virtual bool matches(const Data::Value &value,
                                 std::map<std::string, Data::Value> &bindings) const = 0;

            /**
             * @brief (AR) تحويل النمط إلى نص / (EN) Convert pattern to string
             */
            virtual std::string toString() const = 0;
        };

        // ============================================================================
        // (AR) نمط القيمة الحرفية / (EN) Literal Pattern
        // ============================================================================

        /**
         * @brief (AR) نمط قيمة حرفية / (EN) Literal Pattern
         *
         * (AR) يطابق قيمة حرفية محددة: 0، "hello"، true، إلخ
         * (EN) Matches a specific literal value: 0, "hello", true, etc.
         *
         * @example
         * @code
         * match x:
         *     case 0:         // LiteralPattern(0)
         *     case "hello":   // LiteralPattern("hello")
         *     case true:      // LiteralPattern(true)
         * @endcode
         */
        class LiteralPattern : public Pattern
        {
        public:
            Data::Value literal; ///< (AR) القيمة الحرفية / (EN) Literal value

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            explicit LiteralPattern(Data::Value val)
                : literal(std::move(val)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) مقارنة نوع + قيمة: إذا كلاهما رقمي (INTEGER أو DOUBLE) قارن بالقيمة الرقمية
                //      لتجنب فشل المقارنة بسبب تنسيق النص ("1" vs "1.0")
                //      مثال: ق = 1.0 (مُخزَّن كـ DOUBLE) يجب أن يُطابق "عندما 1:"
                // (EN) Type-aware comparison: if both are numeric (INTEGER or DOUBLE) compare by numeric value
                //      to avoid mismatch due to string formatting ("1" vs "1.0")
                //      Example: x = 1.0 (stored as DOUBLE) must match "when 1:"
                auto vk = value.getType();
                auto lk = literal.getType();
                bool vIsNum = (vk == ::Sad::Types::SadTypeKind::Integer || vk == ::Sad::Types::SadTypeKind::Float);
                bool lIsNum = (lk == ::Sad::Types::SadTypeKind::Integer || lk == ::Sad::Types::SadTypeKind::Float);
                if (vIsNum && lIsNum)
                {
                    return value.toDouble() == literal.toDouble();
                }
                return value.toString() == literal.toString();
            }

            std::string toString() const override
            {
                return literal.toString();
            }
        };

        // ============================================================================
        // (AR) النمط الشامل / (EN) Wildcard Pattern
        // ============================================================================

        /**
         * @brief (AR) النمط الشامل _ / (EN) Wildcard Pattern _
         *
         * (AR) يطابق أي قيمة (default case)
         * (EN) Matches any value (default case)
         *
         * @example
         * @code
         * match x:
         *     case 1:
         *         print("one")
         *     case _:         // WildcardPattern - يطابق كل شيء آخر
         *         print("other")
         * @endcode
         */
        class WildcardPattern : public Pattern
        {
        public:
            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                return true; // (AR) يطابق أي شيء / (EN) Matches anything
            }

            std::string toString() const override
            {
                return "_";
            }
        };

        // ============================================================================
        // (AR) نمط المتغير / (EN) Variable Pattern
        // ============================================================================

        /**
         * @brief (AR) نمط متغير / (EN) Variable Pattern
         *
         * (AR) يطابق أي قيمة ويربطها بمتغير
         * (EN) Matches any value and binds it to a variable
         *
         * @example
         * @code
         * match x:
         *     case n:              // VariablePattern("n") - يربط x بـ n
         *         print("value:", n)
         *     case [first, second]: // يربط أول عنصرين
         *         print(first, second)
         * @endcode
         */
        class VariablePattern : public Pattern
        {
        public:
            std::string name; ///< (AR) اسم المتغير / (EN) Variable name

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            explicit VariablePattern(std::string n)
                : name(std::move(n)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                bindings[name] = value; // (AR) ربط القيمة بالمتغير / (EN) Bind value to variable
                return true;
            }

            std::string toString() const override
            {
                return name;
            }
        };

        // ============================================================================
        // (AR) نمط القائمة / (EN) List Pattern
        // ============================================================================

        /**
         * @brief (AR) نمط قائمة / (EN) List Pattern
         *
         * (AR) يطابق قوائم بأنماط محددة للعناصر
         * (EN) Matches lists with specific patterns for elements
         *
         * @example
         * @code
         * match list:
         *     case []:              // قائمة فارغة
         *     case [x]:             // عنصر واحد
         *     case [x, y]:          // عنصران
         *     case [x, y, z]:       // ثلاثة عناصر
         *     case [first, *rest]:  // أول عنصر + الباقي
         * @endcode
         */
        class ListPattern : public Pattern
        {
        public:
            std::vector<std::unique_ptr<Pattern>> elements; ///< (AR) أنماط العناصر / (EN) Element patterns
            bool has_rest;                                  ///< (AR) هل يوجد *rest؟ / (EN) Has *rest?
            std::string rest_name;                          ///< (AR) اسم متغير rest / (EN) Rest variable name

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            ListPattern(std::vector<std::unique_ptr<Pattern>> elems,
                        bool rest = false,
                        std::string rest_n = "")
                : elements(std::move(elems)),
                  has_rest(rest),
                  rest_name(std::move(rest_n)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) يجب أن تكون القيمة array / (EN) Value must be an array
                if (value.getKind() != Types::SadTypeKind::Array)
                {
                    return false;
                }

                size_t arr_size = value.size();

                // (AR) إذا لم يكن لدينا rest، يجب أن يكون الحجم مطابق / (EN) If no rest, size must match
                if (!has_rest && arr_size != elements.size())
                {
                    return false;
                }

                // (AR) إذا لدينا rest، يجب أن يكون الحجم >= عدد العناصر / (EN) If rest, size must be >= elements count
                if (has_rest && arr_size < elements.size())
                {
                    return false;
                }

                // (AR) طابق كل عنصر / (EN) Match each element
                for (size_t i = 0; i < elements.size(); ++i)
                {
                    if (!elements[i]->matches(value[i], bindings))
                    {
                        return false;
                    }
                }

                // (AR) إذا كان لدينا rest، اربط العناصر المتبقية / (EN) If rest, bind remaining elements
                if (has_rest)
                {
                    std::vector<Data::Value> rest_values;
                    for (size_t i = elements.size(); i < arr_size; ++i)
                    {
                        rest_values.push_back(value[i]);
                    }
                    bindings[rest_name] = Data::Value(rest_values);
                }

                return true;
            }

            std::string toString() const override
            {
                std::string result = "[";
                for (size_t i = 0; i < elements.size(); ++i)
                {
                    if (i > 0)
                        result += ", ";
                    result += elements[i]->toString();
                }
                if (has_rest)
                {
                    if (!elements.empty())
                        result += ", ";
                    result += "*" + rest_name;
                }
                result += "]";
                return result;
            }
        };

        // ============================================================================
        // (AR) نمط النطاق / (EN) Range Pattern
        // ============================================================================

        /**
         * @brief (AR) نمط نطاق: 1..10 / (EN) Range Pattern: 1..10
         *
         * (AR) يطابق قيمة ضمن نطاق محدد
         * (EN) Matches a value within a specified range
         *
         * @example
         * @code
         * match x:
         *     case 1..10:           // RangePattern(1, 10, inclusive)
         *         print("بين 1 و 10")
         *     case 11..=20:         // RangePattern(11, 20, inclusive)
         *         print("بين 11 و 20 شاملاً")
         * @endcode
         */
        class RangePattern : public Pattern
        {
        public:
            Data::Value start; ///< (AR) بداية النطاق / (EN) Range start
            Data::Value end;   ///< (AR) نهاية النطاق / (EN) Range end
            bool inclusive;    ///< (AR) شامل للنهاية؟ / (EN) Inclusive end?

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            RangePattern(Data::Value s, Data::Value e, bool incl = false)
                : start(std::move(s)), end(std::move(e)), inclusive(incl) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) التحقق من أن القيمة رقم / (EN) Check value is a number
                if (value.getKind() != Types::SadTypeKind::Integer &&
                    value.getKind() != Types::SadTypeKind::Float)
                {
                    return false;
                }

                double val = value.toDouble();
                double startVal = start.toDouble();
                double endVal = end.toDouble();

                if (inclusive)
                {
                    return val >= startVal && val <= endVal;
                }
                else
                {
                    return val >= startVal && val < endVal;
                }
            }

            std::string toString() const override
            {
                return start.toString() + (inclusive ? "..=" : "..") + end.toString();
            }
        };

        // ============================================================================
        // (AR) نمط البنية/الصنف / (EN) Struct/Class Pattern
        // ============================================================================

        /**
         * @brief (AR) نمط بنية/صنف {حقل: نمط، ...} / (EN) Struct/Class Pattern {field: pattern, ...}
         *
         * (AR) يطابق كائنات مع أنماط لحقولها
         * (EN) Matches objects with patterns for their fields
         *
         * @example
         * @code
         * match person:
         *     case {الاسم: "أحمد", العمر: ع}:
         *         print("أحمد عمره " + ع)
         *     case {الاسم: ن, العمر: ع} if ع > 18:
         *         print(ن + " بالغ")
         * @endcode
         */
        class StructPattern : public Pattern
        {
        public:
            std::string typeName; ///< (AR) اسم النوع (اختياري) / (EN) Type name (optional)
            std::vector<std::pair<std::string, std::unique_ptr<Pattern>>> fields;

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            StructPattern(std::string type,
                          std::vector<std::pair<std::string, std::unique_ptr<Pattern>>> flds)
                : typeName(std::move(type)), fields(std::move(flds)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) يجب أن تكون القيمة كائن / (EN) Value must be an object
                if (value.getKind() != Types::SadTypeKind::Class)
                {
                    return false;
                }

                // (AR) التحقق من النوع إذا تم تحديده / (EN) Check type if specified
                if (!typeName.empty())
                {
                    // TODO: التحقق من اسم صنف الكائن
                }

                // (AR) مطابقة كل حقل عبر الوصول الكائنيّ (إصلاح ISSUE-034)
                // (EN) Match each field via object-aware field access (ISSUE-034 fix)
                //      tryGetField يقرأ حقل ObjectInstance دون رمي؛ بخلاف operator[]
                //      الذي يخدم الخرائط فقط وكان يرمي دائمًا على الكائنات.
                for (const auto &fieldPair : fields)
                {
                    const std::string &fieldName = fieldPair.first;
                    const auto &fieldPattern = fieldPair.second;

                    const Data::Value *fieldValue = value.tryGetField(fieldName);
                    if (fieldValue == nullptr)
                    {
                        return false; // (AR) الحقل غير موجود / (EN) Field not found
                    }
                    if (!fieldPattern->matches(*fieldValue, bindings))
                    {
                        return false;
                    }
                }

                return true;
            }

            std::string toString() const override
            {
                std::string result = typeName.empty() ? "{" : typeName + " {";
                for (size_t i = 0; i < fields.size(); ++i)
                {
                    if (i > 0)
                        result += ", ";
                    result += fields[i].first + ": " + fields[i].second->toString();
                }
                result += "}";
                return result;
            }
        };

        // ============================================================================
        // (AR) نمط الربط / (EN) Binding Pattern
        // ============================================================================

        /**
         * @brief (AR) نمط ربط: اسم @ نمط / (EN) Binding Pattern: name @ pattern
         *
         * (AR) يربط القيمة بمتغير مع التحقق من نمط آخر
         * (EN) Binds value to variable while checking another pattern
         *
         * @example
         * @code
         * match point:
         *     case نقطة @ {س: 0, ص: ص}:
         *         print("نقطة على محور Y: " + نقطة)
         *     case قيمة @ 1..100:
         *         print("قيمة في النطاق: " + قيمة)
         * @endcode
         */
        class BindingPattern : public Pattern
        {
        public:
            std::string name;                 ///< (AR) اسم المتغير / (EN) Variable name
            std::unique_ptr<Pattern> pattern; ///< (AR) النمط الداخلي / (EN) Inner pattern

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            BindingPattern(std::string n, std::unique_ptr<Pattern> p)
                : name(std::move(n)), pattern(std::move(p)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) أولاً تحقق من النمط الداخلي / (EN) First check inner pattern
                if (!pattern->matches(value, bindings))
                {
                    return false;
                }

                // (AR) اربط القيمة الكاملة بالمتغير / (EN) Bind full value to variable
                bindings[name] = value;
                return true;
            }

            std::string toString() const override
            {
                return name + " @ " + pattern->toString();
            }
        };

        // ============================================================================
        // (AR) نمط OR / (EN) OR Pattern
        // ============================================================================

        /**
         * @brief (AR) نمط OR (a | b | c) / (EN) OR Pattern (a | b | c)
         *
         * (AR) يطابق إذا تطابق أي من البدائل
         * (EN) Matches if any of the alternatives match
         *
         * @example
         * @code
         * match status:
         *     case "ok" | "success" | "done":  // OrPattern
         *         print("نجح")
         *     case "error" | "failed":
         *         print("فشل")
         * @endcode
         */
        class OrPattern : public Pattern
        {
        public:
            std::vector<std::unique_ptr<Pattern>> alternatives; ///< (AR) البدائل / (EN) Alternatives

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            explicit OrPattern(std::vector<std::unique_ptr<Pattern>> alts)
                : alternatives(std::move(alts)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) جرّب كل بديل / (EN) Try each alternative
                for (const auto &alt : alternatives)
                {
                    std::map<std::string, Data::Value> temp_bindings;
                    if (alt->matches(value, temp_bindings))
                    {
                        // (AR) نجح أحد البدائل / (EN) One alternative succeeded
                        bindings.insert(temp_bindings.begin(), temp_bindings.end());
                        return true;
                    }
                }
                return false;
            }

            std::string toString() const override
            {
                std::string result;
                for (size_t i = 0; i < alternatives.size(); ++i)
                {
                    if (i > 0)
                        result += " || ";
                    result += alternatives[i]->toString();
                }
                return result;
            }
        };

        // ============================================================================
        // (AR) نمط عضو تعداد جبري (ADT) / (EN) Enum Variant Pattern (ADT)
        // ============================================================================

        /**
         * @brief (AR) نمط عضو تعداد مع بيانات / (EN) Enum variant pattern with data
         *
         * (AR) يطابق عضو تعداد جبري (ADT) مع أنماط لبياناته
         *      يدعم صيغتين: مع بيانات وبدون بيانات
         * (EN) Matches an ADT enum variant with patterns for its data
         *      Supports two forms: with data and without data
         *
         * @example
         * @code
         * طابق (ش)
         *     عندما شكل.دائرة(ق):         # EnumVariantPattern("شكل", "دائرة", [VariablePattern("ق")])
         *         اطبع("نصف القطر: " + ق)
         *     عندما شكل.مستطيل(ع، ا):     # EnumVariantPattern("شكل", "مستطيل", [Var("ع"), Var("ا")])
         *         اطبع(ع + "×" + ا)
         *     عندما شكل.نقطة:              # EnumVariantPattern("شكل", "نقطة", [])
         *         اطبع("نقطة")
         * @endcode
         */
        class EnumVariantPattern : public Pattern
        {
        public:
            std::string enumName;                                ///< (AR) اسم التعداد / (EN) Enum name
            std::string variantName;                             ///< (AR) اسم العضو / (EN) Variant name
            std::vector<std::unique_ptr<Pattern>> fieldPatterns; ///< (AR) أنماط الحقول / (EN) Field patterns

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             * @param eName اسم التعداد / Enum name (e.g., "شكل")
             * @param vName اسم العضو / Variant name (e.g., "دائرة")
             * @param patterns أنماط الحقول / Field patterns
             */
            EnumVariantPattern(std::string eName, std::string vName,
                               std::vector<std::unique_ptr<Pattern>> patterns = {})
                : enumName(std::move(eName)), variantName(std::move(vName)),
                  fieldPatterns(std::move(patterns)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) القيمة يجب أن تكون خريطة تمثل variant من تعداد جبري
                //      الخريطة تحتوي: __تعداد__ (اسم التعداد)، __عضو__ (اسم العضو)، __حقول__ (بيانات)
                // (EN) Value must be a map representing an ADT variant
                //      Map contains: __تعداد__ (enum name), __عضو__ (variant name), __حقول__ (data)

                if (value.getKind() != Types::SadTypeKind::Map)
                {
                    return false;
                }

                // (AR) تحقق أن اسم التعداد واسم العضو يتطابقان
                // (EN) Check that enum name and variant name match
                try
                {
                    const Data::Value &enumNameVal = value["__تعداد__"];
                    const Data::Value &variantNameVal = value["__عضو__"];

                    if (enumNameVal.toString() != enumName || variantNameVal.toString() != variantName)
                    {
                        return false;
                    }
                }
                catch (...)
                {
                    return false;
                }

                // (AR) إذا لم تكن هناك أنماط حقول، فالمطابقة ناجحة (Unit variant)
                // (EN) If no field patterns, match succeeds (Unit variant)
                if (fieldPatterns.empty())
                {
                    return true;
                }

                // (AR) تحقق من وجود الحقول وطابقها
                // (EN) Check fields exist and match them
                try
                {
                    const Data::Value &fieldsVal = value["__حقول__"];
                    if (fieldsVal.getKind() != Types::SadTypeKind::Array)
                    {
                        return false;
                    }

                    size_t fieldsCount = fieldsVal.size();
                    if (fieldsCount != fieldPatterns.size())
                    {
                        return false;
                    }

                    // (AR) طابق كل حقل مع النمط المقابل
                    // (EN) Match each field with its corresponding pattern
                    for (size_t i = 0; i < fieldPatterns.size(); ++i)
                    {
                        if (!fieldPatterns[i]->matches(fieldsVal[i], bindings))
                        {
                            return false;
                        }
                    }

                    return true;
                }
                catch (...)
                {
                    return false;
                }
            }

            std::string toString() const override
            {
                std::string result = enumName + "." + variantName;
                if (!fieldPatterns.empty())
                {
                    result += "(";
                    for (size_t i = 0; i < fieldPatterns.size(); ++i)
                    {
                        if (i > 0)
                            result += ", ";
                        result += fieldPatterns[i]->toString();
                    }
                    result += ")";
                }
                return result;
            }
        };

        // ============================================================================
        // (AR) نمط الباني (غير مؤهَّل) / (EN) Constructor Pattern (unqualified)
        // ============================================================================

        /**
         * @brief (AR) نمط باني تعداد بحمولة — بصيغة غير مؤهَّلة: اسم(نمط، ...)
         *        (EN) Tagged-enum constructor pattern — unqualified form: Name(pattern, ...)
         *
         * (AR) يميّز عن نمط المتغيّر بوجود أقواس بعد الاسم مباشرةً. بخلاف EnumVariantPattern
         *      (المؤهَّل «تعداد.عضو(...)») هذا النمط يذكر اسم العضو وحده، فتُحسم هويّة التعداد
         *      دلاليًّا (أ-م٢). مثال المواصفة (الملحق أ):
         *          عدد(ق)        => ConstructorPattern("عدد", [VariablePattern("ق")])
         *          جمع(ي، ن)     => ConstructorPattern("جمع", [Var("ي"), Var("ن")])
         * (EN) Distinguished from a variable pattern by parentheses immediately after the
         *      name. Unlike EnumVariantPattern (qualified «Enum.Variant(...)») this names the
         *      variant alone; the enum identity is resolved semantically (phase A-M2).
         */
        class ConstructorPattern : public Pattern
        {
        public:
            std::string variantName;                             ///< (AR) اسم العضو/الباني / (EN) Variant/constructor name
            std::vector<std::unique_ptr<Pattern>> fieldPatterns; ///< (AR) أنماط الحمولة / (EN) Payload patterns

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             * @param vName اسم الباني / Constructor name (e.g., "جمع")
             * @param patterns أنماط الحمولة الموضعيّة / Positional payload patterns
             */
            ConstructorPattern(std::string vName,
                               std::vector<std::unique_ptr<Pattern>> patterns = {})
                : variantName(std::move(vName)), fieldPatterns(std::move(patterns)) {}

            bool matches(const Data::Value &value,
                         std::map<std::string, Data::Value> &bindings) const override
            {
                // (AR) القيمة تمثيلٌ لعضو تعداد بحمولة: خريطة تحوي __عضو__ و__حقول__.
                //      لا نطابق اسم التعداد (النمط غير مؤهَّل) — العضو وحده يكفي.
                // (EN) Value is a tagged-enum variant: a map with __عضو__ and __حقول__.
                //      Enum name is not matched (pattern is unqualified) — variant suffices.
                if (value.getKind() != Types::SadTypeKind::Map)
                {
                    return false;
                }

                try
                {
                    const Data::Value &variantNameVal = value["__عضو__"];
                    if (variantNameVal.toString() != variantName)
                    {
                        return false;
                    }
                }
                catch (...)
                {
                    return false;
                }

                // (AR) عضو وحدويّ (بلا حمولة) — المطابقة تنجح / (EN) Unit variant — succeeds
                if (fieldPatterns.empty())
                {
                    return true;
                }

                try
                {
                    const Data::Value &fieldsVal = value["__حقول__"];
                    if (fieldsVal.getKind() != Types::SadTypeKind::Array ||
                        fieldsVal.size() != fieldPatterns.size())
                    {
                        return false;
                    }
                    for (size_t i = 0; i < fieldPatterns.size(); ++i)
                    {
                        if (!fieldPatterns[i]->matches(fieldsVal[i], bindings))
                        {
                            return false;
                        }
                    }
                    return true;
                }
                catch (...)
                {
                    return false;
                }
            }

            std::string toString() const override
            {
                std::string result = variantName;
                result += "(";
                for (size_t i = 0; i < fieldPatterns.size(); ++i)
                {
                    if (i > 0)
                        result += ", ";
                    result += fieldPatterns[i]->toString();
                }
                result += ")";
                return result;
            }
        };

        // ============================================================================
        // (AR) فرع Case / (EN) Case Clause
        // ============================================================================

        /**
         * @brief (AR) فرع case في match statement / (EN) Case clause in match statement
         *
         * (AR) يمثل فرع case واحد مع نمط، guard اختياري، وجسم
         * (EN) Represents one case clause with pattern, optional guard, and body
         *
         * @example
         * @code
         * match x:
         *     case n if n > 0:     // pattern=n, guard=(n>0), body=[print...]
         *         print("positive")
         * @endcode
         */
        struct CaseClause
        {
            std::unique_ptr<Pattern> pattern; ///< (AR) النمط / (EN) Pattern
            ExprPtr guard;                    ///< (AR) شرط guard اختياري / (EN) Optional guard condition
            std::vector<StmtPtr> body;        ///< (AR) الكود المُنفذ / (EN) Code to execute

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            CaseClause(std::unique_ptr<Pattern> p,
                       ExprPtr g,
                       std::vector<StmtPtr> b)
                : pattern(std::move(p)),
                  guard(std::move(g)),
                  body(std::move(b)) {}

            // Move constructor
            CaseClause(CaseClause &&other) noexcept = default;
            CaseClause &operator=(CaseClause &&other) noexcept = default;

            // Delete copy operations
            CaseClause(const CaseClause &) = delete;
            CaseClause &operator=(const CaseClause &) = delete;
        };

        // ============================================================================
        // (AR) جملة Match / (EN) Match Statement
        // ============================================================================

        /**
         * @brief (AR) جملة match لمطابقة الأنماط / (EN) Match statement for pattern matching
         *
         * (AR) تمثل جملة match كاملة مع قيمة مُختبرة وقائمة حالات
         * (EN) Represents a complete match statement with test value and case list
         *
         * @example
         * @code
         * match x:
         *     case 0:
         *         print("zero")
         *     case n if n < 0:
         *         print("negative")
         *     case n:
         *         print("positive:", n)
         * @endcode
         */
        class MatchStmt : public Statement
        {
        public:
            ExprPtr value;                 ///< (AR) القيمة المُختبرة / (EN) Value to test
            std::vector<CaseClause> cases; ///< (AR) قائمة الحالات / (EN) List of cases

            /**
             * @brief (AR) المُنشئ / (EN) Constructor
             */
            MatchStmt(ExprPtr val,
                      std::vector<CaseClause> cs,
                      const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), value(std::move(val)), cases(std::move(cs)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitMatchStmt(*this);
            }

            std::string toString() const override
            {
                return "MatchStmt";
            }
        };

    } // namespace AST
} // namespace Sad