// ======================================================================
// token.h - فئة الرمز / Token Class
// ======================================================================
// الوصف بالعربية:
//   تعريف أنواع الرموز (TokenType) وفئة الرمز (Token) ومعلومات الموقع (Position)
//   يستخدم المحلل المعجمي (Lexer) هذه الفئات لتمثيل الرموز المستخرجة من النص المصدري
//
// English Description:
//   Definition of token types (TokenType), Token class, and Position information
//   The Lexer uses these classes to represent tokens extracted from source code
//
// ملاحظات / Notes:
//   - جميع الكلمات المفتاحية العربية ممثلة في TokenType
//     All Arabic keywords are represented in TokenType
//   - Position يحفظ رقم السطر والعمود لرسائل الأخطاء الدقيقة
//     Position stores line and column for precise error messages
//   - Token غير قابل للتغيير بعد الإنشاء (immutable)
//     Token is immutable after creation
// ======================================================================

#pragma once

#include <string>
#include <memory>

namespace Sad
{
    namespace Lexer
    {

        // ======================================================================
        // هيكل الموقع / Position Structure
        // ======================================================================
        /**
         * @brief (AR) يمثل موقع الرمز في ملف المصدر (رقم السطر والعمود)
         * @brief (EN) Represents token position in source file (line and column number)
         *
         * @struct Position
         *
         * ملاحظات إضافية:
         * - AR: يبدأ العد من 1 (السطر 1، العمود 1)
         * - EN: Counting starts from 1 (line 1, column 1)
         * - AR: يُستخدم في رسائل الأخطاء لإرشاد المبرمج بدقة
         * - EN: Used in error messages to guide programmer accurately
         */
        struct Position
        {
            size_t line;   ///< رقم السطر / Line number (1-based)
            size_t column; ///< رقم العمود / Column number (1-based)
            size_t offset; ///< الإزاحة من بداية الملف / Offset from file start (0-based)
            size_t length; ///< طول الرمز بالأحرف / Token length in characters

            /**
             * @brief (AR) منشئ افتراضي - يبدأ من السطر 1 العمود 1
             * @brief (EN) Default constructor - starts at line 1, column 1
             */
            Position() : line(1), column(1), offset(0), length(0) {}

            /**
             * @brief (AR) منشئ بقيم محددة (متوافق مع الكود القديم)
             * @brief (EN) Constructor with specific values (backward compatible)
             *
             * @param l (size_t) — (AR) رقم السطر / (EN) line number
             * @param c (size_t) — (AR) رقم العمود / (EN) column number
             */
            Position(size_t l, size_t c) : line(l), column(c), offset(0), length(0) {}

            /**
             * @brief (AR) منشئ كامل مع جميع المعلومات
             * @brief (EN) Full constructor with all information
             *
             * @param l (size_t) — (AR) رقم السطر / (EN) line number
             * @param c (size_t) — (AR) رقم العمود / (EN) column number
             * @param o (size_t) — (AR) الإزاحة من بداية الملف / (EN) offset from file start
             * @param len (size_t) — (AR) طول الرمز / (EN) token length
             */
            Position(size_t l, size_t c, size_t o, size_t len)
                : line(l), column(c), offset(o), length(len) {}

            /**
             * @brief (AR) تحويل الموقع إلى نص للطباعة
             * @brief (EN) Convert position to string for printing
             *
             * @return (std::string) — (AR) نص بصيغة "السطر:العمود" / (EN) string in format "line:column"
             *
             * مثال:
             * Position pos(5, 10);
             * std::cout << pos.toString(); // "5:10"
             */
            std::string toString() const
            {
                return std::to_string(line) + ":" + std::to_string(column);
            }
        };

        // ======================================================================
        // تعداد أنواع الرموز / Token Types Enumeration
        // ======================================================================
        /**
         * @enum TokenType
         * @brief (AR) جميع أنواع الرموز المدعومة في لغة ص
         * @brief (EN) All token types supported in Sad language
         *
         * ملاحظات إضافية:
         * - AR: مقسّمة إلى فئات منطقية (كلمات مفتاحية، عوامل، قيم، إلخ)
         * - EN: Organized into logical categories (keywords, operators, literals, etc.)
         * - AR: كل كلمة مفتاحية عربية لها رمز خاص
         * - EN: Each Arabic keyword has its own token type
         */
        enum class TokenType
        {
            // ╔═════════════════════════════════════════════════════════════════════╗
            // ║  نظام الكلمات المحجوزة — 40 كلمة فقط                              ║
            // ║  Reserved Keywords System — Only 40 keywords                      ║
            // ║                                                                   ║
            // ║  الكلمات المسجلة فعلياً في المحلل المعجمي: 40 + 3 عوامل منطقية     ║
            // ║  الرموز الأخرى (KEYWORD_ASYNC, KEYWORD_PROPERTY, TYPE_*, ...)      ║
            // ║  تبقى في enum لكن المحلل المعجمي لا ينتجها —                       ║
            // ║  المحلل النحوي يتعرف عليها سياقياً كمُعرّفات                       ║
            // ║                                                                   ║
            // ║  ✅ محجوزة (40): دالة، ارجع، صنف، بنية، تعداد، يرث، نهاية،         ║
            // ║     جديد، هذا، باني، الأساس، إذا، وإلا، بينما، لكل، في،           ║
            // ║     توقف، استمر، طابق، عندما، افتراضي، حاول، امسك، ارمي،          ║
            // ║     أخيراً، عام، خاص، محمي، مجرد، استورد، من، كـ، صدّر،            ║
            // ║     متغير، ثابت، ساكن، خارجي، صحيح، خطأ، لاشيء                    ║
            // ║  ⚡ عوامل (3): و، أو، ليس                                         ║
            // ║  🔄 سياقية: خاصية، هدم، عامل، احصل، عيّن، غير_متزامن،             ║
            // ║     انتظر، لامدا، مولد، قالب، فضاء، سمة، نفّذ، اختبر، حالة        ║
            // ║  📝 أنواع مدمجة (كمعرّفات): رقم، عشري، نص، منطقي، فراغ،            ║
            // ║     عدم، مصفوفة، خريطة، أي                                        ║
            // ╚═════════════════════════════════════════════════════════════════════╝

            // ========== [محجوزة] بنيات — الدوال والأصناف / [RESERVED] Structures ==========
            KEYWORD_FUNCTION,    ///< ✅ دالة / function
            KEYWORD_MAIN,        ///< 🔄 رئيسية / main — سياقية (contextual: identifier "رئيسية")
            KEYWORD_RETURN,      ///< ✅ ارجع / return
            KEYWORD_RETURNS,     ///< 🔄 ترجع / returns — سياقية (contextual)
            KEYWORD_CLASS,       ///< ✅ صنف / class
            KEYWORD_INHERITS,    ///< ✅ يرث / inherits
            KEYWORD_END,         ///< ✅ نهاية / end
            KEYWORD_NEW,         ///< ✅ جديد / new
            KEYWORD_THIS,        ///< ✅ هذا / this
                                 // ─────────────────────────────────────────────────────────────────────────
                                 // (AR) كلمة الباني الموحدة - تشمل: باني، منشئ، بناء، constructor
                                 // (EN) Unified constructor keyword - includes: باني، منشئ، بناء، constructor
                                 // ─────────────────────────────────────────────────────────────────────────
            KEYWORD_CONSTRUCTOR, ///< ✅ باني / constructor
            KEYWORD_DESTRUCTOR,  ///< 🔄 هدم / destructor — سياقية (contextual: identifier "هدم")
            KEYWORD_SUPER,       ///< ✅ الأساس / super (base class)

            // ========== [محجوزة] التحكم في التدفق / [RESERVED] Control Flow ==========
            KEYWORD_IF,       ///< ✅ إذا / if
            KEYWORD_ELSE,     ///< ✅ وإلا / else
            KEYWORD_ELSE_IF,  ///< 🔄 وإلا_إذا — يُنتج عبر كلمتين منفصلتين (legacy token)
            KEYWORD_WHILE,    ///< ✅ بينما / while
            KEYWORD_FOR,      ///< ✅ لكل / for
            KEYWORD_IN,       ///< ✅ في / in
            KEYWORD_BREAK,    ///< ✅ توقف / break
            KEYWORD_CONTINUE, ///< ✅ استمر / continue

            // ========== [محجوزة+سياقية] التفريع / [RESERVED+CONTEXTUAL] Switch/Match ==========
            KEYWORD_CASE,    ///< 🔄 حالة / case — سياقية (contextual: identifier "حالة")
            KEYWORD_WHEN,    ///< ✅ عندما / when
            KEYWORD_DEFAULT, ///< ✅ افتراضي / default

            // ========== [محجوزة] المطابقة / [RESERVED] Pattern Matching ==========
            KEYWORD_MATCH, ///< ✅ طابق / match

            // ========== [سياقية] Async/Await / [CONTEXTUAL] Async/Await ==========
            KEYWORD_ASYNC, ///< 🔄 غير_متزامن / async — سياقية
            KEYWORD_AWAIT, ///< 🔄 انتظر / await — سياقية

            // ========== [محجوزة] الأخطاء / [RESERVED] Error Handling ==========
            KEYWORD_TRY,     ///< ✅ حاول / try
            KEYWORD_CATCH,   ///< ✅ امسك / catch
            KEYWORD_THROW,   ///< ✅ ارمي / throw
            KEYWORD_FINALLY, ///< ✅ أخيراً / finally

            // ========== [محجوزة] الوصول / [RESERVED] Access Control ==========
            KEYWORD_PUBLIC,    ///< ✅ عام / public
            KEYWORD_PRIVATE,   ///< ✅ خاص / private
            KEYWORD_PROTECTED, ///< ✅ محمي / protected

            // ========== [legacy] الوراثة الإضافية / [LEGACY] Extra Inheritance ==========
            KEYWORD_EXTENDS,  ///< 🔄 legacy (alias for KEYWORD_INHERITS)
            KEYWORD_ABSTRACT, ///< ✅ مجرد / abstract
            KEYWORD_SEALED,   ///< 🔄 محكم / sealed — سياقية (contextual: identifier "محكم")
                              // NOTE: virtual removed - conflicts with DEFAULT keyword (افتراضي)

            // ========== [محجوزة] الوحدات / [RESERVED] Modules ==========
            KEYWORD_IMPORT, ///< ✅ استورد / import
            KEYWORD_FROM,   ///< ✅ من / from
            KEYWORD_AS,     ///< ✅ كـ / as
            KEYWORD_EXPORT, ///< ✅ صدّر / export
                            // NOTE: module and package keywords removed - not in spec

            // ========== [محجوزة+سياقية] المتغيرات / [RESERVED+CONTEXTUAL] Variables ==========
            KEYWORD_VAR,      ///< ✅ متغير / var
            KEYWORD_CONST,    ///< ✅ ثابت / const
            KEYWORD_STATIC,   ///< ✅ ساكن / static
            KEYWORD_PROPERTY, ///< 🔄 خاصية / property — سياقية
            KEYWORD_GET,      ///< 🔄 احصل / get — سياقية
            KEYWORD_SET,      ///< 🔄 عيّن / set — سياقية
            KEYWORD_ENUM,     ///< ✅ تعداد / enum

            // ========== [سياقية] ميزات Python / [CONTEXTUAL] Python Features ==========
            KEYWORD_LAMBDA,    ///< 🔄 لامدا / lambda — سياقية
            KEYWORD_YIELD,     ///< 🔄 أنتج / yield — سياقية
            KEYWORD_GENERATOR, ///< 🔄 مولد / generator — سياقية
            KEYWORD_WITH,      ///< 🔄 باستخدام / with — سياقية
            KEYWORD_END_WITH,  ///< 🔄 نهاية_استخدام / end_with — سياقية
                               // NOTE: Decorators (@) are Phase 2 - not yet implemented

            // ========== [سياقية] ميزات C++ / [CONTEXTUAL] C++ Features ==========
            KEYWORD_TEMPLATE,      ///< 🔄 قالب / template — سياقية
            KEYWORD_TYPENAME,      ///< 🔄 نوع / typename — سياقية
            KEYWORD_WHERE,         ///< 🔄 حيث / where — سياقية (قيود القوالب)
            KEYWORD_NAMESPACE,     ///< 🔄 فضاء / namespace — سياقية
            KEYWORD_END_NAMESPACE, ///< 🔄 نهاية_فضاء / end_namespace — سياقية
            KEYWORD_OPERATOR,      ///< 🔄 عامل / operator — سياقية

            // ========== ميزات برمجة أنظمة التشغيل / OS Development Features (Phase 8) ==========
            // (AR) هذا القسم يضيف دعم كتابة أنظمة التشغيل والبرمجة منخفضة المستوى
            //      تشمل: التجميع المضمّن، منافذ الإدخال/الإخراج، الوصول المباشر للذاكرة،
            //      المقاطعات، والتصريحات الخارجية
            // (EN) This section adds OS development and low-level programming support
            //      includes: inline assembly, port I/O, direct memory access,
            //      interrupts, and external declarations
            KEYWORD_ASM,      ///< تجميع / asm (inline assembly - كتابة أوامر التجميع مباشرة)
            KEYWORD_VOLATILE, ///< متطاير / volatile (منع التحسين - prevent optimization)
            KEYWORD_EXTERN,   ///< خارجي / extern (تصريح خارجي - external declaration)
            KEYWORD_SIZEOF,   ///< حجم / sizeof (حجم النوع بالبايتات - type size in bytes)
            KEYWORD_UNSAFE,   ///< غير_آمن / unsafe (كتلة غير آمنة - unsafe block for raw ops)
            KEYWORD_PACKED,   ///< محزوم / packed (بنية محزومة بلا حشو - packed struct without padding)
            KEYWORD_NORETURN, ///< بلا_رجوع / noreturn (دالة لا ترجع أبداً - function never returns)
            KEYWORD_NAKED,    ///< عارية / naked (دالة بدون مقدمة/خاتمة - no prologue/epilogue)
            KEYWORD_REGISTER, ///< سجل / register (تخزين في سجل المعالج - store in CPU register)
            KEYWORD_SECTION,  ///< قسم / section (تحديد قسم في الذاكرة - memory section)

            // ========== كلمات مفتاحية نظام النحلة / BeeOS Keywords (Phase 9) ==========
            // (AR) كلمات مفتاحية إضافية لدعم بناء نظام النحلة (BeeOS)
            // (EN) Additional keywords for BeeOS development support
            KEYWORD_LET,       ///< دع / let (تعريف متغير مع ملكية - variable with ownership)
            KEYWORD_STRUCT,    ///< بنية / struct (تعريف بنية بيانات - data structure definition)
            KEYWORD_TRAIT,     ///< سمة / trait (تعريف سمة/واجهة - trait/interface definition)
            KEYWORD_IMPL,      ///< نفّذ / impl (تنفيذ سمة لنوع - implement trait for type)
            KEYWORD_EXTENSION, ///< امتداد / extension (طرق الامتداد - extension methods)
            KEYWORD_MACRO,     ///< ماكرو / macro (تعريف ماكرو - macro definition)
            KEYWORD_DEFER,     ///< 🔄 أجّل / defer — سياقية (تنظيف مضمون - deferred execution)

            // ========== [سياقية] التزامن على نمط Go / [CONTEXTUAL] Go-style Concurrency ==========
            KEYWORD_GO,      ///< 🔄 أطلق / go — سياقية (إطلاق goroutine - launch goroutine)
            KEYWORD_SELECT,  ///< 🔄 اختر / select — سياقية (استقبال من قنوات متعددة - select from channels)
            KEYWORD_UI_DECL, ///< 🔄 واجهة / ui_decl — سياقية (تعريف مكون واجهة تصريحي - UI component declaration)

            LIFETIME, ///< 'أ / lifetime annotation (تعليق عمر - e.g., 'أ, 'ثابت, '_)

            // ========== [سياقية] العقود البرمجية / [CONTEXTUAL] Design by Contract ==========
            KEYWORD_REQUIRES, ///< 🔄 يتطلب / requires — سياقية (شرط مسبق - precondition)
            KEYWORD_ENSURES,  ///< 🔄 يضمن / ensures — سياقية (شرط لاحق - postcondition)
            KEYWORD_CONTRACT, ///< 🔄 عقد / contract — سياقية (عقد ذكي - smart contract)

            KEYWORD_TEST,      ///< اختبر / test (تعريف اختبار - test definition)
            KEYWORD_ATOMIC,    ///< ذرّي / atomic (عملية ذرية - atomic operation)
            KEYWORD_MODULE,    ///< وحدة / module (تعريف وحدة - module definition)
            KEYWORD_USE,       ///< استخدم / use (استيراد مختصر - short import)
            KEYWORD_MUT,       ///< متغيّر / mut (مؤشر/مرجع متغير - mutable pointer/reference)
            KEYWORD_CONST_PTR, ///< *ثابت / const_ptr (مؤشر ثابت - const pointer)
            KEYWORD_MUT_PTR,   ///< *متغير / mut_ptr (مؤشر متغير - mutable pointer)
            KEYWORD_COMPTIME,  ///< وقت_ترجمة / comptime (تقييم وقت الترجمة - compile-time evaluation)
            // ========== [سياقية v4.1] / [CONTEXTUAL v4.1] ==========
            KEYWORD_SHOW,            ///< 🔄 اعرض / show — سياقية (عرض ودجة UI - show widget)
            KEYWORD_GIVE_DEPRECATED, ///< 🔄 اعطِ / give (deprecated) — سياقية (مهجور - use 'أنتج' instead)
            KEYWORD_THEN,            ///< 🔄 ثم / then — سياقية (في تعبير إذا الثلاثي)
            KEYWORD_PROPAGATE,       ///< 🔄 انشر / propagate — سياقية (نشر الخطأ مبكراً)
            KEYWORD_TO,              ///< 🔄 الى/إلى / to — سياقية (في حلقة من..الى)
            TYPE_I64,          ///< ص64 / i64 (عدد صحيح 64-بت بإشارة / signed 64-bit)
            TYPE_USIZE,        ///< ط_حجم / usize (حجم عنوان الذاكرة / memory address size)
            TYPE_ISIZE,        ///< ص_حجم / isize (حجم عنوان بإشارة / signed address size)

            // ========== أنواع بيانات منخفضة المستوى / Low-Level Data Types ==========
            // (AR) أنواع بيانات للتحكم الدقيق في الحجم، ضرورية لبرمجة العتاد
            // (EN) Fixed-size types for precise control, essential for hardware programming
            TYPE_U8,  ///< ط8 / u8 (عدد صحيح 8-بت بدون إشارة / unsigned 8-bit)
            TYPE_U16, ///< ط16 / u16 (عدد صحيح 16-بت بدون إشارة / unsigned 16-bit)
            TYPE_U32, ///< ط32 / u32 (عدد صحيح 32-بت بدون إشارة / unsigned 32-bit)
            TYPE_U64, ///< ط64 / u64 (عدد صحيح 64-بت بدون إشارة / unsigned 64-bit)
            TYPE_I8,  ///< ص8 / i8 (عدد صحيح 8-بت بإشارة / signed 8-bit)
            TYPE_I16, ///< ص16 / i16 (عدد صحيح 16-بت بإشارة / signed 16-bit)
            TYPE_I32, ///< ص32 / i32 (عدد صحيح 32-بت بإشارة / signed 32-bit)
            TYPE_PTR, ///< مؤشر / ptr (مؤشر خام - raw pointer for memory addresses)

            // ========== [مدمجة] أنواع البيانات — لم تعد محجوزة / [BUILT-IN] Types — No longer reserved ==========
            // (AR) المحلل المعجمي لا ينتج هذه الرموز بعد الآن
            //      المحلل النحوي يتعرف على أسماء الأنواع سياقياً عبر IDENTIFIER
            // (EN) Lexer no longer produces these tokens
            //      Parser recognizes type names contextually via IDENTIFIER
            TYPE_INTEGER, ///< 📝 رقم / integer — مُعرّف مدمج
            TYPE_DOUBLE,  ///< 📝 عشري / double — مُعرّف مدمج
            TYPE_STRING,  ///< 📝 نص / string — مُعرّف مدمج
            TYPE_BOOLEAN, ///< 📝 منطقي / boolean — مُعرّف مدمج
            TYPE_VOID,    ///< 📝 فراغ / void — مُعرّف مدمج
            TYPE_NULL,    ///< 📝 عدم / null — مُعرّف مدمج
            TYPE_ARRAY,   ///< 📝 مصفوفة / array — مُعرّف مدمج
            TYPE_MAP,     ///< 📝 خريطة / map — مُعرّف مدمج
            TYPE_ANY,     ///< 📝 أي / any — مُعرّف مدمج

            // ========== القيم الحرفية / Literal Values ==========
            LITERAL_TRUE,  ///< صحيح / true
            LITERAL_FALSE, ///< خطأ / false
            LITERAL_NULL,  ///< عدم / null

            // ========== المعرّفات والقيم / Identifiers & Values ==========
            IDENTIFIER,          ///< معرّف (اسم متغير، دالة، صنف) / identifier
            NUMBER_INTEGER,      ///< رقم صحيح / integer literal (123)
            NUMBER_DOUBLE,       ///< رقم عشري / double literal (3.14)
            STRING_LITERAL,      ///< نص حرفي / string literal ("text")
            STRING_RAW,          ///< نص خام / raw string (r"text\n" - backslashes are literal)
            STRING_FSTRING,      ///< نص منسق / formatted string (f"Hello {name}")
            STRING_FSTRING_PART, ///< جزء من نص منسق / part of f-string (between expressions)
            DOC_COMMENT,         ///< تعليق توثيقي / documentation comment (## or #** **#)

            // ========== العوامل الحسابية / Arithmetic Operators ==========
            OP_PLUS,         ///< + جمع / addition
            OP_MINUS,        ///< - طرح / subtraction
            OP_MULTIPLY,     ///< * ضرب / multiplication
            OP_DIVIDE,       ///< / قسمة / division
            OP_FLOOR_DIVIDE, ///< // قسمة صحيحة / floor division (integer division)
            OP_MODULO,       ///< % باقي القسمة / modulo
            OP_POWER,        ///< ** أس / power

            // ========== عوامل التعيين / Assignment Operators ==========
            OP_ASSIGN,              ///< = تعيين / assignment
            OP_WALRUS,              ///< := Walrus operator (assignment expression - Phase 2)
            OP_PLUS_ASSIGN,         ///< += جمع وتعيين / add and assign
            OP_MINUS_ASSIGN,        ///< -= طرح وتعيين / subtract and assign
            OP_MULTIPLY_ASSIGN,     ///< *= ضرب وتعيين / multiply and assign
            OP_DIVIDE_ASSIGN,       ///< /= قسمة وتعيين / divide and assign
            OP_FLOOR_DIVIDE_ASSIGN, ///< //= قسمة صحيحة وتعيين / floor divide and assign
            OP_MODULO_ASSIGN,       ///< %= باقي قسمة وتعيين / modulo and assign

            // ========== عوامل المقارنة / Comparison Operators ==========
            OP_EQUAL,         ///< == يساوي / equal to
            OP_NOT_EQUAL,     ///< != لا يساوي / not equal to
            OP_LESS,          ///< < أصغر من / less than
            OP_LESS_EQUAL,    ///< <= أصغر أو يساوي / less than or equal
            OP_GREATER,       ///< > أكبر من / greater than
            OP_GREATER_EQUAL, ///< >= أكبر أو يساوي / greater than or equal

            // ========== العوامل المنطقية / Logical Operators ==========
            OP_AND, ///< && و / logical AND
            OP_OR,  ///< || أو / logical OR
            OP_NOT, ///< ! ليس / logical NOT

            // ========== عوامل البت / Bitwise Operators ==========
            OP_XOR,         ///< ^ XOR بتّي / bitwise XOR
            OP_BITWISE_AND, ///< & AND بتّي / bitwise AND
            OP_BITWISE_OR,  ///< | OR بتّي / bitwise OR
            OP_SHIFT_LEFT,  ///< << إزاحة يسار / left shift
            OP_SHIFT_RIGHT, ///< >> إزاحة يمين / right shift
            OP_BITWISE_NOT, ///< ~ NOT بتّي / bitwise NOT

            // ========== عامل الأنبوب / Pipeline Operator ==========
            OP_PIPE_ARROW, ///< |> أنبوب / pipeline operator

            // ========== عوامل الزيادة والنقصان / Increment/Decrement ==========
            OP_INCREMENT, ///< ++ زيادة / increment
            OP_DECREMENT, ///< -- نقصان / decrement

            // ========== الأقواس والرموز الخاصة / Brackets & Special Symbols ==========
            PAREN_LEFT,    ///< ( قوس يسار / left parenthesis
            PAREN_RIGHT,   ///< ) قوس يمين / right parenthesis
            BRACE_LEFT,    ///< { قوس معقوف يسار / left brace
            BRACE_RIGHT,   ///< } قوس معقوف يمين / right brace
            BRACKET_LEFT,  ///< [ قوس مربع يسار / left bracket
            BRACKET_RIGHT, ///< ] قوس مربع يمين / right bracket

            // ========== علامات الترقيم / Punctuation ==========
            COMMA,             ///< , فاصلة / comma
            ARABIC_COMMA,      ///< ، فاصلة عربية (U+060C, spec 03_oop.md §1)
            SEMICOLON,         ///< ; فاصلة منقوطة / semicolon
            ARABIC_SEMICOLON,  ///< ؛ فاصلة منقوطة عربية (U+061B, spec 03_oop.md §1)
            COLON,             ///< : نقطتان / colon
            DOUBLE_COLON,      ///< :: نقطتان مزدوجتان / double colon (static member access)
            DOT,               ///< . نقطة / dot (member access)
            DOT_DOT,           ///< .. نقطتان / double dot (range operator)
            ELLIPSIS,          ///< ... ثلاث نقاط / ellipsis (spread operator)
            ARROW,             ///< -> سهم / arrow
            FAT_ARROW,         ///< => سهم عريض / fat arrow (lambda/arrow functions)
            QUESTION,          ///< ? علامة استفهام / question mark
            QUESTION_DOT,      ///< ?. وصول آمن / optional chaining (null safety)
            QUESTION_QUESTION, ///< ?? تجميع فارغ / null coalescing operator
            OP_NULL_ASSERT,    ///< مؤكَّد/مؤكَّدة تأكيد عدم الفراغ T؟→T / null assertion (NS-05)
            AT_SIGN,           ///< @ علامة آت / at sign (decorators)
            AMPERSAND,         ///< & علامة العطف / ampersand (borrow/reference)

            // ========== رموز خاصة / Special Tokens ==========
            COMMENT,     ///< تعليق / comment
            NEWLINE,     ///< سطر جديد / newline
            END_OF_FILE, ///< نهاية الملف / end of file
            INVALID,     ///< رمز غير صالح / invalid token
            WHITESPACE   ///< مسافة (عادة تُتجاهل) / whitespace
        };

        // ======================================================================
        // فئة الرمز / Token Class
        // ======================================================================
        /**
         * @class Token
         * @brief (AR) يمثل رمزاً واحداً من البرنامج المصدري
         * @brief (EN) Represents a single token from source code
         *
         * ملاحظات إضافية:
         * - AR: كل رمز يحتوي على: النوع، القيمة، الموقع
         * - EN: Each token contains: type, value, position
         * - AR: الرمز غير قابل للتغيير بعد الإنشاء (immutable)
         * - EN: Token is immutable after creation
         * - AR: يُستخدم في بناء شجرة التحليل النحوي (AST)
         * - EN: Used in building Abstract Syntax Tree (AST)
         */
        class Token
        {
        public:
            // ==================================================================
            // المنشئات / Constructors
            // ==================================================================

            /**
             * @brief (AR) منشئ كامل مع جميع المعلومات
             * @brief (EN) Full constructor with all information
             *
             * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
             * @param value (std::string) — (AR) قيمة الرمز النصية / (EN) token text value
             * @param position (Position) — (AR) موقع الرمز في المصدر / (EN) token position in source
             *
             * مثال:
             * Token token(TokenType::NUMBER_INTEGER, "42", Position(1, 5));
             *
             * ملاحظات إضافية:
             * - AR: القيمة تُحفظ كما هي من المصدر (قبل التحويل)
             * - EN: Value is stored as-is from source (before conversion)
             */
            Token(TokenType type, const std::string &value, const Position &position);

            /**
             * @brief (AR) منشئ مبسط بدون موقع (للاختبار)
             * @brief (EN) Simplified constructor without position (for testing)
             *
             * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
             * @param value (std::string) — (AR) قيمة الرمز / (EN) token value
             */
            Token(TokenType type, const std::string &value);

            /**
             * @brief (AR) منشئ مبسط بنوع فقط (لرموز بدون قيمة)
             * @brief (EN) Simplified constructor with type only (for valueless tokens)
             *
             * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
             *
             * مثال:
             * Token token(TokenType::PAREN_LEFT); // "("
             */
            explicit Token(TokenType type);

            // ==================================================================
            // الوظائف الاستعلامية / Getter Functions
            // ==================================================================

            /**
             * @brief (AR) الحصول على نوع الرمز
             * @brief (EN) Get token type
             *
             * @return (TokenType) — (AR) نوع الرمز / (EN) token type
             */
            TokenType getType() const { return type_; }

            /**
             * @brief (AR) الحصول على قيمة الرمز النصية
             * @brief (EN) Get token text value
             *
             * @return (std::string) — (AR) القيمة النصية / (EN) text value
             */
            std::string getValue() const { return value_; }

            /**
             * @brief (AR) الحصول على موقع الرمز
             * @brief (EN) Get token position
             *
             * @return (Position) — (AR) موقع الرمز / (EN) token position
             */
            Position getPosition() const { return position_; }

            // ==================================================================
            // وظائف الفحص / Check Functions
            // ==================================================================

            /**
             * @brief (AR) التحقق من أن الرمز من نوع معين
             * @brief (EN) Check if token is of specific type
             *
             * @param type (TokenType) — (AR) النوع المراد المقارنة به / (EN) type to compare with
             * @return (bool) — (AR) true إذا كان النوع مطابق / (EN) true if types match
             *
             * مثال:
             * if (token.is(TokenType::KEYWORD_IF)) {
             *     // معالجة شرط if
             * }
             */
            bool is(TokenType type) const { return type_ == type; }

            /**
             * @brief (AR) التحقق من أن الرمز أحد عدة أنواع
             * @brief (EN) Check if token is one of several types
             *
             * @param types (std::initializer_list<TokenType>) — (AR) قائمة الأنواع / (EN) list of types
             * @return (bool) — (AR) true إذا كان النوع ضمن القائمة / (EN) true if type is in list
             *
             * مثال:
             * if (token.isOneOf({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
             *     // معالجة عملية جمع أو طرح
             * }
             */
            bool isOneOf(std::initializer_list<TokenType> types) const;

            /**
             * @brief (AR) التحقق من أن الرمز كلمة مفتاحية
             * @brief (EN) Check if token is a keyword
             *
             * @return (bool) — (AR) true إذا كان الرمز كلمة مفتاحية / (EN) true if token is keyword
             */
            bool isKeyword() const;

            /**
             * @brief (AR) التحقق من أن الرمز عامل (operator)
             * @brief (EN) Check if token is an operator
             *
             * @return (bool) — (AR) true إذا كان الرمز عامل / (EN) true if token is operator
             */
            bool isOperator() const;

            /**
             * @brief (AR) التحقق من أن الرمز قيمة حرفية (literal)
             * @brief (EN) Check if token is a literal
             *
             * @return (bool) — (AR) true إذا كان الرمز قيمة حرفية / (EN) true if token is literal
             */
            bool isLiteral() const;

            // ==================================================================
            // وظائف التحويل / Conversion Functions
            // ==================================================================

            /**
             * @brief (AR) تحويل الرمز إلى نص للطباعة
             * @brief (EN) Convert token to string for printing
             *
             * @return (std::string) — (AR) تمثيل نصي للرمز / (EN) string representation of token
             *
             * مثال:
             * Token token(TokenType::NUMBER_INTEGER, "42", Position(1, 5));
             * std::cout << token.toString();
             * // Output: "Token(NUMBER_INTEGER, '42', 1:5)"
             */
            std::string toString() const;

            /**
             * @brief (AR) الحصول على اسم نوع الرمز
             * @brief (EN) Get token type name
             *
             * @return (std::string) — (AR) اسم النوع / (EN) type name
             *
             * مثال:
             * Token token(TokenType::KEYWORD_IF);
             * std::cout << token.getTypeName(); // "KEYWORD_IF"
             */
            std::string getTypeName() const;

            /**
             * @brief (AR) تحويل نوع الرمز إلى اسم (دالة ثابتة)
             * @brief (EN) Convert token type to name (static function)
             *
             * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
             * @return (std::string) — (AR) اسم النوع / (EN) type name
             */
            static std::string typeToString(TokenType type);

        private:
            TokenType type_;    ///< نوع الرمز / Token type
            std::string value_; ///< قيمة الرمز النصية / Token text value
            Position position_; ///< موقع الرمز / Token position
        };

    } // namespace Lexer
} // namespace Sad
