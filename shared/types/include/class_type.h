// ======================================================================
// class_type.h - نوع بيانات الصنف / Class Type Data Structure
// ======================================================================
// الوصف بالعربية:
//   تعريف بنية بيانات لتمثيل تعريف صنف في نظام التصنيفات
//   يحتوي على معلومات عن الخصائص، الطرق، الباني، الهدام، والوراثة
//
// English Description:
//   Definition of class type data structure for representing class definitions
//   Contains information about fields, methods, constructor, destructor, and inheritance
//
// ملاحظات / Notes:
//   - يُنشأ عند تحليل تعريف صنف (ClassDeclStmt)
//     Created when parsing class declaration (ClassDeclStmt)
//   - يُخزّن في ClassManager لإعادة الاستخدام
//     Stored in ClassManager for reuse
//   - يُستخدم لإنشاء ObjectInstance
//     Used to create ObjectInstance objects
//   - توثيق ثنائي اللغة (عربي/إنجليزي)
//     Bilingual documentation (Arabic/English)
// ======================================================================

#pragma once

#include "type.h"
#include "value.h"
#include "class_nodes.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace Sad
{
    namespace Data
    {

        // Forward declarations
        class ObjectInstance;

        // ======================================================================
        // بنية خاصية / Field Structure
        // ======================================================================

        /**
         * @struct ClassField
         * @brief (AR) معلومات خاصية في صنف
         * @brief (EN) Class field information
         *
         * ملاحظات إضافية:
         * - AR: تحتوي على النوع، الرؤية، وقيمة افتراضية
         * - EN: Contains type, visibility, and default value
         * - AR: الخصائص الثابتة تُخزّن قيمتها في ClassType
         * - EN: Static fields store their value in ClassType
         *
         * مثال:
         * عام نص الاسم = "افتراضي"
         * ClassField: {name="الاسم", type=STRING, visibility=PUBLIC, defaultValue="افتراضي", isStatic=false}
         */
        struct ClassField
        {
            std::string name;           ///< (AR) اسم الخاصية / (EN) field name
            Type *type;                 ///< (AR) نوع البيانات / (EN) data type
            AST::Visibility visibility; ///< (AR) الرؤية / (EN) visibility
            Value defaultValue;         ///< (AR) قيمة افتراضية / (EN) default value
            bool isStatic;              ///< (AR) هل ثابتة؟ / (EN) is static?

            // ──────────────────────────────────────────────────────────────
            // (AR) SEM045 (عقد الغياب — حقول الأصناف): تصنيفُ الحقلِ المُصرَّح
            //      («رقم قيمة» ⇒ Integer). قِيس أنّ `type` أعلاه يُمرَّر nullptr
            //      في **كلِّ** مواضعِ التسجيل، فالنوعُ المُصرَّح لا يبلغ زمنَ
            //      التشغيل أصلًا — وحارسُ «الفراغُ لا يعبر إلى خانةٍ مصنَّفة»
            //      يحتاجه. يُملأ بعد addField في مواضعِ التسجيل (لا تغييرَ
            //      لتوقيع addField)؛ Unknown = حقلٌ مجرَّدٌ (خانةٌ ديناميّة).
            // (EN) SEM045 (absence contract — class fields): the field's DECLARED
            //      kind («رقم قيمة» ⇒ Integer). Measured: `type` above is passed
            //      nullptr at every registration site, so the declared type never
            //      reaches runtime — and the "Void must not cross into a typed
            //      slot" guard needs it. Populated after addField at registration
            //      sites (no addField signature change); Unknown = a bare
            //      (dynamic) field.
            // ──────────────────────────────────────────────────────────────
            Sad::Types::SadTypeKind declaredKind = Sad::Types::SadTypeKind::Unknown;

            // ──────────────────────────────────────────────────────────────
            // (AR) 🔑 مُهيّئٌ يُقيَّم **عند كلِّ إنشاء** لا مرّةً عند تصريحِ الصنف.
            //      `defaultValue` قيمةٌ مُقيَّمةٌ سلفًا تُستنسَخ لكلِّ كائن، وذلك
            //      يكفي للقيمِ البسيطة. أمّا حقلٌ نوعُه صنفٌ فقيمتُه كائنٌ،
            //      و`Value::clone()` نسخةُ **سطحٍ واحد**: قِيس أنّ كائنَين من
            //      صنفٍ فيه حقلٌ مركَّبٌ بمستويَين يتقاسمان الكائنَ الأعمق
            //      (الكتابةُ في أحدِهما تُقرأ في الآخر)، وأنّ بانيَ الحقلِ يُنفَّذ
            //      مرّةً واحدةً وقتَ تصريحِ الصنفِ ولو لم يُنشَأ منه كائنٌ قطّ.
            //      فالمؤشّرُ هنا يحمل التعبيرَ ليُعادَ تقييمُه لكلِّ نسخة.
            //      🔑 واسمٌ لا مؤشّر: كانت هنا إشارةٌ **مستعارةٌ** إلى عقدةٍ في
            //      الشجرة، فقِيس انهيارُها — `ClassType` يعيش في مُفردةِ
            //      `ClassManager` بينما تُحرَّر الشجرة، فأمرُ `:reset` في الصدَفةِ
            //      ثمّ إنشاءُ كائنٍ يُسقِط العمليّةَ بـSIGSEGV. والضابطان يعزلانه:
            //      صنفٌ بلا حقلٍ صنفيٍّ مع `:reset` يعمل، وحقلٌ صنفيٌّ بلا `:reset`
            //      يعمل. والاسمُ يملك نفسَه فلا عمرَ يُراقَب — وهو كافٍ لأنّ
            //      التحليةَ لا تُنتِج إلّا `NewExpr` بلا وسائط.
            // (EN) A name, not a borrowed pointer: ClassType outlives the AST (it
            //      lives in the ClassManager singleton), so a REPL `:reset` followed
            //      by an instantiation crashed measurably. A string owns itself, and
            //      it suffices because the desugar only ever emits an argument-less
            //      NewExpr.
            // ──────────────────────────────────────────────────────────────
            std::string defaultConstructClass;

            // ──────────────────────────────────────────────────────────────
            // (AR) موضعُ التصريحِ الذي وُلِّدت منه التحلية — سطرًا وعمودًا خامَّين.
            //      العقدةُ التي تُبنى وقتَ الإنشاءِ في `expression_evaluator_oop_new`
            //      عقدةٌ محلّيّةٌ على المكدّس، فلو تُركت بموضعِها الافتراضيِّ لأشارت
            //      كلُّ أخطاءِ الإنشاءِ إلى السطرِ ١ العمودِ ١. وعددان خامَّان لا
            //      `Lexer::Position` عمدًا: `shared/types` لا يعتمد على المعجم،
            //      ولا يصحّ أن يجرَّ إليه اعتمادٌ من أجلِ حقلَين.
            //      صفرٌ يعني «لا موضعَ معروف» — وهو إعلانُ جهلٍ لا ادّعاءُ علم.
            // (EN) Declaration position the desugar came from, as raw numbers.
            //      Zero means "unknown" — an admission, not a false claim.
            // ──────────────────────────────────────────────────────────────
            size_t defaultConstructLine = 0;
            size_t defaultConstructColumn = 0;

            /**
             * @brief (AR) منشئ مع اسم ونوع
             * @brief (EN) Constructor with name and type
             */
            ClassField(const std::string &n, Type *t, AST::Visibility vis)
                : name(n), type(t), visibility(vis), isStatic(false) {}
        };

        // ======================================================================
        // بنية طريقة / Method Structure
        // ======================================================================

        /**
         * @struct ClassMethod
         * @brief (AR) معلومات طريقة في صنف
         * @brief (EN) Class method information
         *
         * ملاحظات إضافية:
         * - AR: تحتوي على المعاملات، نوع الإرجاع، والجسم
         * - EN: Contains parameters, return type, and body
         * - AR: الطرق الثابتة لا تحتاج كائن للاستدعاء
         * - EN: Static methods don't need object for invocation
         * - AR: الطرق الافتراضية قابلة للتجاوز
         * - EN: Virtual methods can be overridden
         *
         * مثال:
         * عام دالة رقم احسب_العمر()
         * ClassMethod: {name="احسب_العمر", returnType=INTEGER, visibility=PUBLIC, isStatic=false, isVirtual=false}
         */
        struct ClassMethod
        {
            std::string name;                           ///< (AR) اسم الطريقة / (EN) method name
            Type *returnType;                           ///< (AR) نوع الإرجاع / (EN) return type
            std::vector<AST::Parameter> parameters;     ///< (AR) المعاملات / (EN) parameters
            AST::Visibility visibility;                 ///< (AR) الرؤية / (EN) visibility
            std::unique_ptr<AST::BlockStmt> body;       ///< (AR) جسم الطريقة (ملكية حصرية) / (EN) method body (exclusive ownership)
            std::shared_ptr<AST::BlockStmt> sharedBody; ///< (AR) جسم مشترك (للدوال الافتراضية من السمات) / (EN) shared body (for trait default methods)
            bool isStatic;                              ///< (AR) هل ثابتة؟ / (EN) is static?
            bool isVirtual;                             ///< (AR) هل افتراضية؟ / (EN) is virtual?
            bool isAbstract;                            ///< (AR) هل مجردة؟ / (EN) is abstract?

            // ═══ العقود البرمجية (Design by Contract) ═══
            /// (AR) شروط مسبقة: يتطلب — تُفحص قبل تنفيذ الطريقة
            /// (EN) Preconditions: requires — checked before method execution
            std::vector<std::shared_ptr<AST::Expression>> preconditions;
            /// (AR) شروط لاحقة: يضمن — تُفحص بعد تنفيذ الطريقة
            /// (EN) Postconditions: ensures — checked after method execution
            std::vector<std::shared_ptr<AST::Expression>> postconditions;

            /**
             * @brief (AR) الحصول على جسم الطريقة الفعلي (يفضل الحصري على المشترك)
             * @brief (EN) Get effective method body (prefers owned over shared)
             * @return (AST::BlockStmt*) — (AR) مؤشر للجسم أو nullptr / (EN) pointer to body or nullptr
             */
            AST::BlockStmt *getBody() const
            {
                return body ? body.get() : sharedBody.get();
            }

            /**
             * @brief (AR) منشئ مع اسم ورؤية
             * @brief (EN) Constructor with name and visibility
             */
            ClassMethod(const std::string &n, AST::Visibility vis, Type *ret = nullptr)
                : name(n), returnType(ret), visibility(vis),
                  isStatic(false), isVirtual(false), isAbstract(false) {}
        };

        // ======================================================================
        // بنية خاصية / Property Structure
        // ======================================================================

        /**
         * @struct ClassProperty
         * @brief (AR) معلومات خاصية (Property) في صنف مع getter/setter
         * @brief (EN) Class property information with getter/setter
         *
         * ملاحظات إضافية:
         * - AR: تحتوي على getter (إلزامي) و setter (اختياري)
         * - EN: Contains getter (required) and setter (optional)
         * - AR: الـ getter/setter عبارة عن كتل تنفيذية
         * - EN: Getter/setter are executable code blocks
         *
         * مثال:
         * خاصية رقم العمر احصل ارجع _العمر نهاية عيّن(قيمة) _العمر = قيمة نهاية نهاية
         */
        struct ClassProperty
        {
            std::string name;           ///< (AR) اسم الخاصية / (EN) property name
            Type *type;                 ///< (AR) نوع البيانات / (EN) data type
            AST::Visibility visibility; ///< (AR) الرؤية / (EN) visibility
            bool isStatic;              ///< (AR) هل ثابتة؟ / (EN) is static?

            std::unique_ptr<AST::BlockStmt> getterBody; ///< (AR) جسم الـ getter / (EN) getter body
            std::unique_ptr<AST::BlockStmt> setterBody; ///< (AR) جسم الـ setter / (EN) setter body (optional)
            std::string setterParamName;                ///< (AR) اسم معامل setter / (EN) setter parameter name

            /**
             * @brief (AR) منشئ مع اسم ورؤية
             * @brief (EN) Constructor with name and visibility
             */
            ClassProperty(const std::string &n, Type *t, AST::Visibility vis)
                : name(n), type(t), visibility(vis), isStatic(false) {}

            /**
             * @brief (AR) هل للقراءة فقط؟ (بدون setter)
             * @brief (EN) Is read-only? (no setter)
             */
            bool isReadOnly() const { return getterBody && !setterBody; }

            /**
             * @brief (AR) هل للكتابة فقط؟ (بدون getter)
             * @brief (EN) Is write-only? (no getter)
             */
            bool isWriteOnly() const { return !getterBody && setterBody; }
        };

        // ======================================================================
        // بنية تحميل عامل زائد / Operator Overload Structure
        // ======================================================================

        /**
         * @struct OperatorOverload
         * @brief (AR) معلومات تحميل عامل زائد في صنف
         * @brief (EN) Operator overload information in a class
         *
         * ملاحظات إضافية:
         * - AR: يحتوي على رمز العامل، المعاملات، نوع الإرجاع، والجسم
         * - EN: Contains operator symbol, parameters, return type, and body
         *
         * مثال:
         * عامل +(آخر: كسر) -> كسر { ... }
         * OperatorOverload: {operatorSymbol="+", parameters=[...], body=...}
         */
        struct OperatorOverload
        {
            std::string operatorSymbol;             ///< (AR) رمز العامل (+, -, *, /, ==, <, ...) / (EN) operator symbol
            std::vector<AST::Parameter> parameters; ///< (AR) المعاملات / (EN) parameters
            Types::SadTypeKind returnType;          ///< (AR) نوع الإرجاع / (EN) return type (S-TS-P2.5a)
            std::unique_ptr<AST::Statement> body;   ///< (AR) جسم العامل / (EN) operator body
            AST::AccessModifier access;             ///< (AR) معدّل الوصول / (EN) access modifier

            /**
             * @brief (AR) منشئ مع رمز العامل
             * @brief (EN) Constructor with operator symbol
             */
            OperatorOverload(const std::string &op, AST::AccessModifier acc = AST::AccessModifier::PUBLIC)
                : operatorSymbol(op), returnType(Types::SadTypeKind::Unknown), access(acc) {}
        };

        // ======================================================================
        // صنف ClassType الرئيسي / Main ClassType Class
        // ======================================================================

        /**
         * @class ClassType
         * @brief (AR) نوع بيانات يمثل تعريف صنف
         * @brief (EN) Data type representing a class definition
         *
         * الوظائف الرئيسية:
         * - AR: تخزين معلومات الصنف (اسم، خصائص، طرق، باني، هدام)
         * - EN: Store class information (name, fields, methods, constructor, destructor)
         * - AR: دعم الوراثة (baseClass)
         * - EN: Support inheritance (baseClass)
         * - AR: توفير واجهة للبحث عن الأعضاء
         * - EN: Provide interface for member lookup
         * - AR: إنشاء كائنات جديدة (createInstance)
         * - EN: Create new objects (createInstance)
         *
         * مثال استخدام:
         * ```
         * // تعريف صنف شخص
         * ClassType* personClass = new ClassType("شخص");
         * personClass->addField("الاسم", stringType, PUBLIC);
         * personClass->addField("العمر", intType, PRIVATE);
         * personClass->addMethod("اطبع_معلومات", ...);
         *
         * // إنشاء كائن
         * ObjectInstance* ahmed = personClass->createInstance();
         * ahmed->setField("الاسم", Value("أحمد"));
         * ```
         */
        class ClassType : public Type
        {
        public:
            // ──────────────────────────────────────────────────────────────────
            // البيانات الأساسية / Basic Data
            // ──────────────────────────────────────────────────────────────────

            std::string name;                         ///< (AR) اسم الصنف / (EN) class name
            ClassType *baseClass;                     ///< (AR) الصنف الأساسي الأول / (EN) primary base class
            std::vector<ClassType *> additionalBases; ///< (AR) الأصناف الأساسية الإضافية (وراثة متعددة) / (EN) additional base classes (multiple inheritance)

            // ──────────────────────────────────────────────────────────────────
            // الأعضاء / Members
            // ──────────────────────────────────────────────────────────────────

            std::vector<ClassField> fields;                    ///< (AR) قائمة الخصائص / (EN) list of fields
            std::vector<ClassMethod> methods;                  ///< (AR) قائمة الطرق / (EN) list of methods
            std::vector<ClassProperty> properties;             ///< (AR) قائمة الخصائص (Properties) / (EN) list of properties
            std::vector<OperatorOverload> operatorOverloads;   ///< (AR) قائمة العوامل المحملة زائداً / (EN) list of operator overloads
            std::unique_ptr<AST::ConstructorDecl> constructor; ///< (AR) الباني / (EN) constructor
            std::unique_ptr<AST::DestructorDecl> destructor;   ///< (AR) الهدام / (EN) destructor

            // ──────────────────────────────────────────────────────────────────
            // خرائط للبحث السريع / Hash Maps for Fast Lookup
            // ──────────────────────────────────────────────────────────────────

            std::unordered_map<std::string, size_t> fieldIndex;    ///< (AR) خريطة اسم→رقم الخاصية / (EN) name→field index map
            std::unordered_map<std::string, size_t> methodIndex;   ///< (AR) خريطة اسم→رقم الطريقة / (EN) name→method index map
            std::unordered_map<std::string, size_t> propertyIndex; ///< (AR) خريطة اسم→رقم Property / (EN) name→property index map
            std::unordered_map<std::string, Value> staticFields;   ///< (AR) الخصائص الثابتة / (EN) static fields values
            std::vector<std::string> implementedTraits;            ///< (AR) الواجهات المنفذة / (EN) implemented traits
            bool isContract = false;                               ///< (AR) هل هو عقد ذكي؟ / (EN) is smart contract?
            bool isSealed = false;                                 ///< (AR) هل الصنف محكم؟ / (EN) is sealed class?
            bool isUIComponent = false;                            ///< (AR) هل هو مكون واجهة (واجهة)؟ / (EN) is UI component (واجهة)?
            std::unordered_set<std::string> uiStateFields;         ///< (AR) أسماء حقول @حالة (للتفاعلية) / (EN) @state field names (for reactivity)
            std::string sourceFile;                                ///< (AR) مسار الملف المصدري (لفحص الوراثة المحكمة) / (EN) source file path (for sealed inheritance check)

            // ──────────────────────────────────────────────────────────────────
            // المنشئات / Constructors
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) منشئ مع اسم الصنف
             * @brief (EN) Constructor with class name
             *
             * @param className (std::string) — (AR) اسم الصنف / (EN) class name
             */
            explicit ClassType(const std::string &className);

            /**
             * @brief (AR) منشئ مع اسم وصنف أساسي
             * @brief (EN) Constructor with name and base class
             *
             * @param className (std::string) — (AR) اسم الصنف / (EN) class name
             * @param base (ClassType*) — (AR) الصنف الأساسي / (EN) base class
             */
            ClassType(const std::string &className, ClassType *base);

            /**
             * @brief (AR) هدام افتراضي
             * @brief (EN) Default destructor
             */
            ~ClassType() override = default;

            // ──────────────────────────────────────────────────────────────────
            // واجهة Type / Type Interface
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) الحصول على اسم النوع
             * @brief (EN) Get type name
             *
             * @return (std::string) — (AR) اسم الصنف / (EN) class name
             */
            std::string getName() const { return name; }

            /**
             * @brief (AR) هل النوع مطابق لنوع آخر؟
             * @brief (EN) Is this type equal to another?
             *
             * @param other (const Type*) — (AR) النوع الآخر / (EN) other type
             * @return (bool) — (AR) true إذا متطابق / (EN) true if equal
             */
            bool isEqual(const Type *other) const override;

            /**
             * @brief (AR) هل النوع قابل للتحويل لنوع آخر؟
             * @brief (EN) Is this type convertible to another?
             *
             * ملاحظات إضافية:
             * - AR: يدعم التحويل للصنف الأساسي (upcast)
             * - EN: Supports upcast to base class
             *
             * @param other (const Type*) — (AR) النوع المستهدف / (EN) target type
             * @return (bool) — (AR) true إذا قابل للتحويل / (EN) true if convertible
             */
            bool isConvertibleTo(const Type *other) const override;

            /**
             * @brief (AR) تمثيل نصي للنوع
             * @brief (EN) String representation of type
             *
             * @return (std::string) — (AR) "صنف <اسم>" / (EN) "class <name>"
             */
            std::string toString() const { return "class " + name; }

            // ──────────────────────────────────────────────────────────────────
            // إدارة الخصائص / Field Management
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) إضافة خاصية جديدة
             * @brief (EN) Add a new field
             *
             * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
             * @param type (Type*) — (AR) نوع البيانات / (EN) data type
             * @param visibility (AST::Visibility) — (AR) الرؤية / (EN) visibility
             * @param isStatic (bool) — (AR) هل ثابتة؟ / (EN) is static?
             * @param defaultValue (Value) — (AR) قيمة افتراضية / (EN) default value
             *
             * @return (bool) — (AR) true إذا نجحت الإضافة / (EN) true if added successfully
             *
             * ملاحظات إضافية:
             * - AR: ترجع false إذا كان الاسم موجود مسبقاً
             * - EN: Returns false if name already exists
             */
            bool addField(const std::string &fieldName, Type *type,
                          AST::Visibility visibility, bool isStatic = false,
                          const Value &defaultValue = Value(),
                          const std::string &defaultConstructClass = std::string(),
                          size_t defaultConstructLine = 0,
                          size_t defaultConstructColumn = 0);

            /**
             * @brief (AR) البحث عن خاصية بالاسم
             * @brief (EN) Find field by name
             *
             * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
             * @return (ClassField*) — (AR) مؤشر للخاصية أو nullptr / (EN) pointer to field or nullptr
             *
             * ملاحظات إضافية:
             * - AR: يبحث في الصنف الحالي ثم الأصناف الأساسية
             * - EN: Searches in current class then base classes
             */
            ClassField *findField(const std::string &fieldName);

            /**
             * @brief (AR) الحصول على قيمة خاصية ثابتة
             * @brief (EN) Get static field value
             *
             * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
             * @return (Value*) — (AR) مؤشر للقيمة أو nullptr / (EN) pointer to value or nullptr
             */
            Value *getStaticField(const std::string &fieldName);

            /**
             * @brief (AR) تعيين قيمة خاصية ثابتة
             * @brief (EN) Set static field value
             *
             * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
             * @param value (const Value&) — (AR) القيمة الجديدة / (EN) new value
             * @return (bool) — (AR) true إذا نجحت العملية / (EN) true if successful
             */
            bool setStaticField(const std::string &fieldName, const Value &value);

            // ──────────────────────────────────────────────────────────────────
            // إدارة الطرق / Method Management
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) إضافة طريقة جديدة
             * @brief (EN) Add a new method
             *
             * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
             * @param visibility (AST::Visibility) — (AR) الرؤية / (EN) visibility
             * @param returnType (Type*) — (AR) نوع الإرجاع / (EN) return type
             * @param parameters (std::vector<AST::Parameter>) — (AR) المعاملات / (EN) parameters
             * @param body (std::unique_ptr<AST::BlockStmt>) — (AR) جسم الطريقة / (EN) method body
             * @param isStatic (bool) — (AR) هل ثابتة؟ / (EN) is static?
             * @param isVirtual (bool) — (AR) هل افتراضية؟ / (EN) is virtual?
             *
             * @return (bool) — (AR) true إذا نجحت الإضافة / (EN) true if added successfully
             */
            bool addMethod(const std::string &methodName, AST::Visibility visibility,
                           Type *returnType, const std::vector<AST::Parameter> &parameters,
                           std::unique_ptr<AST::BlockStmt> body,
                           bool isStatic = false, bool isVirtual = false,
                           bool isAbstract = false);

            /**
             * @brief (AR) إضافة طريقة بجسم مشترك (للدوال الافتراضية من السمات)
             * @brief (EN) Add method with shared body (for trait default methods)
             *
             * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
             * @param visibility (AST::Visibility) — (AR) الرؤية / (EN) visibility
             * @param returnType (Type*) — (AR) نوع الإرجاع / (EN) return type
             * @param parameters (std::vector<AST::Parameter>) — (AR) المعاملات / (EN) parameters
             * @param sharedBody (std::shared_ptr<AST::BlockStmt>) — (AR) الجسم المشترك / (EN) shared body
             * @return (bool) — (AR) true إذا نجحت الإضافة / (EN) true if added successfully
             */
            bool addDefaultMethod(const std::string &methodName, AST::Visibility visibility,
                                  Type *returnType, const std::vector<AST::Parameter> &parameters,
                                  std::shared_ptr<AST::BlockStmt> sharedBody,
                                  bool isStatic = false, bool isVirtual = false);

            /**
             * @brief (AR) البحث عن طريقة بالاسم
             * @brief (EN) Find method by name
             *
             * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
             * @return (ClassMethod*) — (AR) مؤشر للطريقة أو nullptr / (EN) pointer to method or nullptr
             *
             * ملاحظات إضافية:
             * - AR: يبحث في الصنف الحالي ثم الأصناف الأساسية
             * - EN: Searches in current class then base classes
             * - AR: يراعي التجاوز (override) في الوراثة
             * - EN: Respects method override in inheritance
             */
            ClassMethod *findMethod(const std::string &methodName);

            // ──────────────────────────────────────────────────────────────────
            // إدارة الخصائص (Properties) / Property Management
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) إضافة خاصية (Property) للصنف
             * @brief (EN) Add property to class
             *
             * @param property (ClassProperty) — (AR) الخاصية / (EN) property to add
             */
            void addProperty(ClassProperty property);

            /**
             * @brief (AR) البحث عن خاصية (Property) بالاسم
             * @brief (EN) Find property by name
             *
             * @param propertyName (const std::string&) — (AR) اسم الخاصية / (EN) property name
             * @return (ClassProperty*) — (AR) مؤشر للخاصية أو nullptr / (EN) pointer to property or nullptr
             *
             * ملاحظات إضافية:
             * - AR: يبحث في الصنف الحالي ثم الأصناف الأساسية
             * - EN: Searches in current class then base classes
             */
            ClassProperty *findProperty(const std::string &propertyName);

            /**
             * @brief (AR) هل يوجد خاصية (Property) بهذا الاسم؟
             * @brief (EN) Does property with this name exist?
             *
             * @param propertyName (const std::string&) — (AR) اسم الخاصية / (EN) property name
             * @return (bool) — (AR) true إذا وُجدت / (EN) true if exists
             */
            bool hasProperty(const std::string &propertyName) const;

            // ──────────────────────────────────────────────────────────────────
            // إدارة العوامل المحملة زائداً / Operator Overload Management
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) إضافة تحميل عامل زائد
             * @brief (EN) Add operator overload
             *
             * @param overload (OperatorOverload) — (AR) العامل المحمل / (EN) operator overload to add
             */
            void addOperatorOverload(OperatorOverload overload);

            /**
             * @brief (AR) البحث عن تحميل عامل زائد بالرمز
             * @brief (EN) Find operator overload by symbol
             *
             * @param operatorSymbol (const std::string&) — (AR) رمز العامل / (EN) operator symbol (+, -, *, etc.)
             * @return (OperatorOverload*) — (AR) مؤشر للعامل أو nullptr / (EN) pointer to overload or nullptr
             *
             * ملاحظات إضافية:
             * - AR: يبحث في الصنف الحالي ثم الأصناف الأساسية
             * - EN: Searches in current class then base classes
             */
            OperatorOverload *findOperator(const std::string &operatorSymbol);

            /**
             * @brief (AR) هل يوجد تحميل عامل زائد بهذا الرمز؟
             * @brief (EN) Does operator overload with this symbol exist?
             *
             * @param operatorSymbol (const std::string&) — (AR) رمز العامل / (EN) operator symbol
             * @return (bool) — (AR) true إذا وُجد / (EN) true if exists
             */
            bool hasOperator(const std::string &operatorSymbol) const;

            // ──────────────────────────────────────────────────────────────────
            // الباني والهدام / Constructor and Destructor
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) تعيين الباني
             * @brief (EN) Set constructor
             *
             * @param ctor (std::unique_ptr<AST::ConstructorDecl>) — (AR) الباني / (EN) constructor
             */
            void setConstructor(std::unique_ptr<AST::ConstructorDecl> ctor);

            /**
             * @brief (AR) تعيين الهدام
             * @brief (EN) Set destructor
             *
             * @param dtor (std::unique_ptr<AST::DestructorDecl>) — (AR) الهدام / (EN) destructor
             */
            void setDestructor(std::unique_ptr<AST::DestructorDecl> dtor);

            /**
             * @brief (AR) هل يوجد باني مخصص؟
             * @brief (EN) Does class have custom constructor?
             *
             * @return (bool) — (AR) true إذا وُجد باني / (EN) true if constructor exists
             */
            bool hasConstructor() const { return constructor != nullptr; }

            /**
             * @brief (AR) هل يوجد هدام مخصص؟
             * @brief (EN) Does class have custom destructor?
             *
             * @return (bool) — (AR) true إذا وُجد هدام / (EN) true if destructor exists
             */
            bool hasDestructor() const { return destructor != nullptr; }

            // ──────────────────────────────────────────────────────────────────
            // الوراثة / Inheritance
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) هل يرث الصنف من صنف آخر؟
             * @brief (EN) Does this class inherit from another class?
             *
             * @param otherClass (const ClassType*) — (AR) الصنف الآخر / (EN) other class
             * @return (bool) — (AR) true إذا كان يرث / (EN) true if inherits
             *
             * ملاحظات إضافية:
             * - AR: يفحص الوراثة المباشرة وغير المباشرة
             * - EN: Checks direct and indirect inheritance
             */
            bool inheritsFrom(const ClassType *otherClass) const;

            /**
             * @brief (AR) الحصول على الصنف الأساسي
             * @brief (EN) Get base class
             *
             * @return (ClassType*) — (AR) مؤشر للصنف الأساسي أو nullptr / (EN) pointer to base class or nullptr
             */
            ClassType *getBaseClass() const { return baseClass; }

            /**
             * @brief (AR) الحصول على جميع الأصناف الأساسية الإضافية
             * @brief (EN) Get additional base classes
             */
            const std::vector<ClassType *> &getAdditionalBases() const { return additionalBases; }

            /**
             * @brief (AR) تعيين الصنف الأساسي
             * @brief (EN) Set base class
             *
             * @param base (ClassType*) — (AR) الصنف الأساسي / (EN) base class
             */
            void setBaseClass(ClassType *base) { baseClass = base; }

            // ──────────────────────────────────────────────────────────────────
            // إنشاء كائنات / Object Creation
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) إنشاء كائن جديد من هذا الصنف
             * @brief (EN) Create new object instance of this class
             *
             * @return (ObjectInstance*) — (AR) مؤشر لكائن جديد / (EN) pointer to new object
             *
             * ملاحظات إضافية:
             * - AR: تُهيّأ جميع الخصائص بقيمها الافتراضية
             * - EN: Initializes all fields with default values
             * - AR: الباني يُستدعى لاحقاً من الـ Interpreter
             * - EN: Constructor is called later by Interpreter
             */
            ObjectInstance *createInstance();

            // ──────────────────────────────────────────────────────────────────
            // فحص الرؤية / Visibility Checking
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) فحص إمكانية الوصول لخاصية
             * @brief (EN) Check if field is accessible
             *
             * @param fieldName (std::string) — (AR) اسم الخاصية / (EN) field name
             * @param fromClass (const ClassType*) — (AR) الصنف المستدعي / (EN) calling class
             * @return (bool) — (AR) true إذا كان الوصول مسموح / (EN) true if accessible
             *
             * ملاحظات إضافية:
             * - AR: عام (PUBLIC) → الكل يصل
             * - EN: PUBLIC → everyone can access
             * - AR: خاص (PRIVATE) → الصنف نفسه فقط
             * - EN: PRIVATE → only same class
             * - AR: محمي (PROTECTED) → الصنف والوارثون
             * - EN: PROTECTED → class and inheritors
             */
            bool isFieldAccessible(const std::string &fieldName, const ClassType *fromClass) const;

            /**
             * @brief (AR) فحص إمكانية الوصول لطريقة
             * @brief (EN) Check if method is accessible
             *
             * @param methodName (std::string) — (AR) اسم الطريقة / (EN) method name
             * @param fromClass (const ClassType*) — (AR) الصنف المستدعي / (EN) calling class
             * @return (bool) — (AR) true إذا كان الوصول مسموح / (EN) true if accessible
             */
            bool isMethodAccessible(const std::string &methodName, const ClassType *fromClass) const;

            // ──────────────────────────────────────────────────────────────────
            // أدوات مساعدة / Helper Functions
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) الحصول على عدد الخصائص (مع الأصناف الأساسية)
             * @brief (EN) Get total field count (including base classes)
             *
             * @return (size_t) — (AR) عدد الخصائص / (EN) number of fields
             */
            size_t getTotalFieldCount() const;

            /**
             * @brief (AR) الحصول على عدد الطرق (مع الأصناف الأساسية)
             * @brief (EN) Get total method count (including base classes)
             *
             * @return (size_t) — (AR) عدد الطرق / (EN) number of methods
             */
            size_t getTotalMethodCount() const;

            /**
             * @brief (AR) طباعة معلومات الصنف للتصحيح
             * @brief (EN) Print class information for debugging
             */
            void printDebugInfo() const;

        private:
            // ──────────────────────────────────────────────────────────────────
            // دوال مساعدة خاصة / Private Helper Functions
            // ──────────────────────────────────────────────────────────────────

            /**
             * @brief (AR) فحص رؤية عضو
             * @brief (EN) Check member visibility
             */
            bool checkVisibility(AST::Visibility vis, const ClassType *fromClass) const;
        };

        // ==============================================================
        // (AR) القيمةُ الافتراضيّةُ لنوعٍ — مصدرُ حقيقةٍ واحد
        // ==============================================================
        //
        // (AR) حكمُ المالك: التصريحُ بلا مُهيِّئٍ يساوي إسنادَ القيمةِ الافتراضيّةِ
        //      للنوع — صفرًا للعدد، و0.0 للعشريّ، ونصًّا فارغًا، وخطأً للمنطقيّ.
        //      وكان هذا الجدولُ منسوخًا في مواضعَ عدّة، فتفترق الخانةُ الواحدةُ
        //      بحسبِ البابِ الذي دخلت منه. فمن أراد قيمةً افتراضيّةً فليطلبها
        //      من هنا، ولا ينسخِ الجدولَ مرّةً أخرى.
        // (EN) One table for type defaults. It used to be copied per call site,
        //      so the same slot could differ by which path declared it.
        Value defaultValueForTypeKind(Types::SadTypeKind kind);

    } // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
