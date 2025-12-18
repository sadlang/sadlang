

## 1️⃣ Integer (رقم)

### Syntax (EBNF)
```ebnf
integer_literal ::= decimal_literal | binary_literal | octal_literal | hex_literal

decimal_literal ::= ['+' | '-'] digit+
binary_literal  ::= ('0b'|'0ثن') ('0' | '1')+
octal_literal   ::= ('0o'|'0ث') [0-7]+
hex_literal     ::= ('0x'|'0س') [0-9a-fA-Fابجدهو]+

digit ::= [0-9]


## 2️⃣ Float (عشري)

### Syntax (EBNF)
```ebnf
float_literal ::= ['+' | '-'] digit+ '.' digit+ [exponent]
                | ['+' | '-'] digit+ exponent

exponent ::= ('e' | 'E'|'أ') ['+' | '-'] digit+
```


## 3️⃣ Boolean (منطقي)

### Syntax (EBNF)
```ebnf
boolean_literal ::= 'صحيح' | 'خطأ' | 'true' | 'false'
```


## 4️⃣ String (نص)

### Syntax (EBNF)
```ebnf
string_literal ::= '"' string_char* '"'
                 | "'" string_char* "'"
                 | '"' multiline_char* '"'
                 | ''' multiline_char* '''
                 | '"' fstring_char* '"'
                 

string_char    ::= any_unicode_char | escape_sequence
escape_sequence ::= '\n' | '\t' | '\r' | '\س' | '\ت' | '\ع'| '\\' | '\"' | '\''
```

## 5️⃣ None/Null (لاشيء)

### Syntax (EBNF)
```ebnf
none_literal ::= 'لاشيء' | 'none' | 'null'
```

## 6️⃣ Array (مصفوفة)

### Syntax (EBNF)
```ebnf
array_literal ::= '[' [expression (',' expression)*] ']'
array_access  ::= expression '[' expression ']'
```

## 7️⃣ Map/Dictionary (خريطة)

### Syntax (EBNF)
```ebnf
map_literal ::= '{' [map_pair (',' map_pair)*] '}'
map_pair    ::= expression ':' expression
map_access  ::= expression '[' expression ']'
```

## 1️⃣ Basic Function Declaration (الإعلان البسيط)

### Syntax / البناء النحوي (EBNF)
```ebnf
function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block

param_list ::= param ((',' | '،') param)*
param      ::= IDENTIFIER [':' type] ['=' expression]

block ::= statement* 'نهاية'
```

### Semantics
- **Declaration:** Function definitions create callable objects
- **Return Type:** Optional type before function name (e.g., `دالة رقم جمع(...)`)
- **Scope:** Functions have their own local scope
- **Return:** Explicit `إرجاع` (return) or implicit `None`
- **Name:** Must be unique in current scope (supports overloading in Phase 2)
- **Punctuation:** Both `،` and `,` supported for parameters; both `؛` and `;` supported for statements



## 2️⃣ Default Parameters (المعاملات الافتراضية)

### Syntax / البناء النحوي (EBNF)
```ebnf
param_with_default ::= IDENTIFIER [':' type] '=' expression
```


### Semantics
- **Evaluation:** Default values evaluated at call time, not definition time
- **Order:** Parameters with defaults must come after non-default parameters
- **Scope:** Default expressions have access to closure scope
**الشرح بالعربية:**
- `param_with_default`: معامل له قيمة افتراضية يتكون من: اسم المعامل، نوع اختياري بعد نقطتين، ثم علامة = متبوعة بتعبير القيمة الافتراضية

### Semantics / الدلالات
- **Evaluation / التقييم:** القيم الافتراضية يتم تقييمها وقت استدعاء الدالة وليس وقت تعريفها
- **Order / الترتيب:** المعاملات ذات القيم الافتراضية يجب أن تأتي بعد المعاملات الإلزامية
- **Scope / النطاق:** تعبيرات القيم الافتراضية لها إمكانية الوصول إلى نطاق الإغلاق



## 3️⃣ Lambda Functions (الدوال المجهولة)

### Syntax / البناء النحوي (EBNF)
```ebnf
lambda_expr ::= 'دالة' [type] '(' [param_list] ')' '=>' expression
              | '(' [param_list] ')' '=>' expression
```

**الشرح بالعربية:**
- `lambda_expr`: دالة مجهولة (lambda) إما تبدأ بكلمة 'دالة' متبوعة بنوع الإرجاع الاختياري قبل الأقواس، أو بدونها
- الصيغة الكاملة: `دالة [نوع] (معاملات) => تعبير` - نوع الإرجاع يأتي بعد كلمة 'دالة' وقبل الأقواس
- الصيغة المختصرة: `(معاملات) => تعبير` - بدون كلمة 'دالة' ونوع الإرجاع

### Semantics / الدلالات
- **Anonymous / مجهولة:** ليس لها اسم دالة
- **Single Expression / تعبير واحد:** الجسم عبارة عن تعبير واحد (إرجاع ضمني)
- **First-Class / من الدرجة الأولى:** يمكن تمريرها كمعاملات أو إرجاعها من دوال
- **Closures / إغلاقات:** تحتجز المتغيرات من النطاق المحيط



## 4️⃣ Closures (الإغلاقات)

### Syntax / البناء النحوي
الإغلاقات تستخدم نفس صيغة الدوال أو اللامدا العادية، لكنها تحتجز المتغيرات من النطاق المحيط.

Closures use standard function/lambda syntax but capture variables from enclosing scope.


### Semantics
- **Capture:** Functions capture references to variables in enclosing scope
- **Lifetime:** Captured variables live as long as the closure lives
- **Modification:** Closures can modify captured variables

### Semantics / الدلالات
- **Capture / الاحتجاز:** الدوال تحتجز مراجع للمتغيرات في النطاق المحيط
- **Lifetime / العمر:** المتغيرات المحتجزة تظل حية طالما الإغلاق حي
- **Modification / التعديل:** الإغلاقات يمكنها تعديل المتغيرات المحتجزة



## 5️⃣ Higher-Order Functions (الدوال العليا)


### Semantics
- **Function as Parameter:** Functions can accept other functions as arguments
- **Function as Return:** Functions can return other functions
- **Common Patterns:** map, filter, reduce, compose

### Semantics / الدلالات
- **Function as Parameter / دالة كمعامل:** الدوال يمكنها قبول دوال أخرى كمعاملات
- **Function as Return / دالة كإرجاع:** الدوال يمكنها إرجاع دوال أخرى
- **Common Patterns / الأنماط الشائعة:** map (تطبيق)، filter (تصفية)، reduce (طي)، compose (تركيب)



# 🏛️ قواعد البرمجة كائنية التوجه / OOP Rules

### (AR) الوصف
هذا الملف يوثق **نظام البرمجة كائنية التوجه الكامل** للغة "ص". يشمل الأصناف (Classes)، الوراثة (Inheritance)، الواجهات (Interfaces)، البواني والهدامات (Constructors/Destructors)، إدارة الذاكرة (Memory Management)، والكائنات (Objects).

### (EN) Description
This file documents the **complete Object-Oriented Programming system** for Sad language, including classes, inheritance, interfaces, constructors/destructors, memory management, and objects.

---


### Syntax (EBNF)
```ebnf
class_decl ::= 'صنف' IDENTIFIER [(':' | 'يرث') base_class_list] class_body 'نهاية'

base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*

class_body ::= (field_decl | method_decl | constructor_decl | destructor_decl)*

field_decl ::= [visibility] type IDENTIFIER ['=' expression] (';' | '؛')?

method_decl ::= [visibility] 'دالة' [type] IDENTIFIER '(' [param_list] ')' block

constructor_decl ::= ('باني' | 'منشئ') '(' [param_list] ')' [':' initializer_list] block

destructor_decl ::= ('~باني' | '~منشئ' | 'مدمر') '()' block

visibility ::= 'عام' | 'خاص' | 'محمي'  // public | private | protected

initializer_list ::= IDENTIFIER '(' expression ')' ((',' | '،') IDENTIFIER '(' expression ')')*

param_list ::= param ((',' | '،') param)*
```


## 2️⃣ Inheritance (الوراثة)

### Syntax (EBNF)
```ebnf
class_with_inheritance ::= 'صنف' IDENTIFIER (':' | 'يرث') base_class_list class_body 'نهاية'

base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*

super_call ::= 'الأساس' '.' method_name '(' [arg_list] ')'
             | 'الأساس' '(' [arg_list] ')'  // Constructor call

arg_list ::= expression ((',' | '،') expression)*
```

**الشرح بالعربية:**
- الوراثة يمكن التعبير عنها بـ ':' أو كلمة 'يرث' العربية
- مثال: `صنف كلب : حيوان` أو `صنف كلب يرث حيوان`
- دعم الفاصلة العربية (،) والإنجليزية (,) في قوائم الأصناف الأساسية والمعاملات

### Semantics
- **Single Inheritance:** One base class (Phase 1)
- **Multiple Inheritance:** Multiple base classes (Phase 2)
- **Method Resolution:** Child methods override parent methods
- **super keyword:** `الأساس` refers to parent class



## 3️⃣ Static Members (الأعضاء الثابتة)

### Syntax (EBNF)
```ebnf
static_field ::= 'ثابت' type IDENTIFIER '=' expression (';' | '؛')?

static_method ::= 'ثابت' 'دالة' [type] IDENTIFIER '(' [param_list] ')' block

static_access ::= ClassName '.' static_member_name

param_list ::= param ((',' | '،') param)*
```

**الشرح بالعربية:**
- نوع الإرجاع في الطريقة الثابتة اختياري ويأتي بعد 'دالة'
- دعم الفاصلة المنقوطة الاختيارية (عربية ؛ أو إنجليزية ;)
- دعم الفاصلة العربية (،) والإنجليزية (,)

### Semantics
- **Shared:** Static members shared across all instances
- **Access:** Can be accessed without creating instance
- **this:** Not available in static methods



## 4️⃣ Destructor & Memory Management (الهدام وإدارة الذاكرة)

### Syntax (EBNF)
```ebnf
destructor_decl ::= ('~باني' | '~منشئ' | 'مدمر') '()' block
```

**الشرح بالعربية:**
- يدعم ثلاثة أشكال للهدام: '~باني' أو '~منشئ' أو 'مدمر'
- الهدام لا يأخذ معاملات ولا يعيد قيمة

### Semantics
- **Automatic:** Destructor called when object goes out of scope
- **RAII:** Resource Acquisition Is Initialization pattern
- **GC Hybrid:** Reference counting + GC for cycles

# 📐 قواعد النحو العام / General Syntax Rules


## 1️⃣ Expressions (التعابير)

### Syntax (EBNF)
```ebnf
expression ::= assignment

assignment ::= (call '.')? IDENTIFIER '=' assignment
             | logical_or

logical_or  ::= logical_and ('||' logical_and)*
logical_and ::= equality ('&&' equality)*
equality    ::= comparison (('==' | '!=') comparison)*
comparison  ::= term (('>' | '>=' | '<' | '<=') term)*
term        ::= factor (('+' | '-') factor)*
factor      ::= unary (('*' | '/' | '%') unary)*
unary       ::= ('!' | '-' | '+') unary
              | power

power       ::= postfix ('**' unary)*
postfix     ::= primary (call | index | member)*

call        ::= '(' [arguments] ')'
index       ::= '[' expression ']'
member      ::= '.' IDENTIFIER

arguments   ::= expression ((',' | '،') expression)*

primary     ::= literal
              | IDENTIFIER
              | '(' expression ')'
              | array_literal
              | map_literal
              | lambda

literal     ::= INTEGER | FLOAT | STRING | BOOLEAN | NONE
```


### Semantics
- **Evaluation Order:** Left to right, respecting precedence
- **Short-Circuit:** `&&` and `||` use short-circuit evaluation
- **Type Coercion:** Automatic in mixed arithmetic operations



## 2️⃣ Statements (الجمل)

### Syntax (EBNF)
```ebnf
statement ::= expr_stmt
            | var_decl
            | block
            | if_stmt
            | while_stmt
            | for_stmt
            | return_stmt
            | break_stmt
            | continue_stmt
            | import_stmt
            | class_decl
            | function_decl

expr_stmt ::= expression '؛'?

var_decl ::= type IDENTIFIER ['=' expression] '؛'?

block ::= statement* 'نهاية'

if_stmt ::= 'إذا' '(' expression ')' block ('وإلا' 'إذا' '(' expression ')' block)* ['وإلا' block]

while_stmt ::= 'بينما' '(' expression ')' block

for_stmt ::= 'لكل' IDENTIFIER 'في' expression block
           | 'لكل' '(' [var_decl] '؛' [expression] '؛' [expression] ')' block

return_stmt ::= 'إرجاع' [expression] '؛'?

break_stmt ::= 'اخرج' '؛'?

continue_stmt ::= 'تابع' '؛'?
```

### Semantics
- **Semicolons (؛ or ;):** Both Arabic (؛) and English (;) semicolons are supported and optional in most cases
- **Blocks:** End with 'نهاية' keyword, create new scope
- **Return:** Must be inside function
- **Break/Continue:** Must be inside loop



## 3️⃣ Control Flow (هياكل التحكم)

### Syntax Summary
```ebnf
if_stmt      ::= 'إذا' '(' expr ')' block ('وإلا' 'إذا' '(' expr ')' block)* ['وإلا' block]
while_stmt   ::= 'بينما' '(' expr ')' block
for_in_stmt  ::= 'لكل' ID 'في' expr block
for_stmt     ::= 'لكل' '(' [stmt] '؛' [expr] '؛' [expr] ')' block
switch_stmt  ::= 'حالة' expr case_branch* [default_branch] 'نهاية'
try_stmt     ::= 'حاول' block ('امسك' ID block)+ ['أخيراً' block] 'نهاية'
```


## 6️⃣ Comments (التعليقات)

### Syntax (EBNF)
```ebnf
comment ::= line_comment | block_comment | doc_comment

line_comment  ::= '#' any_char* NEWLINE

block_comment ::= '#*' any_char* '*#'

doc_comment   ::= '##' any_char* NEWLINE
                | '#**' any_char* '**#'
```


## 7️⃣ Encoding & RTL Support (الترميز ودعم النص من اليمين)

### UTF-8 Encoding
- **All source files:** UTF-8 encoded
- **Identifiers:** Support Arabic, English, numbers, underscore
- **Strings:** Full Unicode support

### RTL (Right-to-Left) Support
- **Direction:** Arabic text flows right-to-left
- **Mixing:** Can mix RTL (Arabic) and LTR (English) in same file
- **Comments:** Support both directions


# 🐍 ميزات Python في لغة "ص" / Python Features in Sad Language


## 1️⃣ List Comprehensions (تضمينات القوائم)

### Syntax (EBNF)
```ebnf
list_comprehension ::= '[' expression 'لكل' IDENTIFIER 'في' iterable [if_clause] ']'

if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'

condition ::= expression

// Alternative English-style
list_comprehension ::= '[' expression 'for' IDENTIFIER 'in' iterable ['if' '(' condition ')' | 'if' condition] ']'
```


**الشرح بالعربية:**
- `if_clause`: يدعم الشرط بدون أقواس `إذا شرط` أو مع أقواس `إذا (شرط)`
- الأقواس اختيارية لتحسين الوضوح في الشروط المعقدة

### الشرح بالعربية
تضمينات القوائم (List Comprehensions) تسمح بإنشاء قوائم جديدة من قوائم موجودة بطريقة موجزة وقابلة للقراءة. الصيغة الأساسية: `[تعبير لكل عنصر في قائمة]`

### (EN) Explanation
List Comprehensions allow creating new lists from existing iterables in a concise and readable way. Basic syntax: `[expression for item in list]`

### Semantics
- **Evaluation Order:** The iterable is evaluated first, then for each element, the expression is evaluated
- **Filtering:** Optional `if` clause filters elements before applying the expression
- **Type:** Result is always a `مصفوفة` (Array/List)
- **Scope:** Loop variable is local to the comprehension
- **Parentheses:** Optional parentheses around condition are supported for clarity (both `إذا شرط` and `إذا (شرط)` are valid)



## 2️⃣ Dict Comprehensions (تضمينات القواميس)

### Syntax (EBNF)
```ebnf
dict_comprehension ::= '{' key_expr ':' value_expr 'لكل' IDENTIFIER 'في' iterable [if_clause] '}'

if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'

// Alternative English-style
dict_comprehension ::= '{' key_expr ':' value_expr 'for' IDENTIFIER 'in' iterable ['if' '(' condition ')' | 'if' condition] '}'
```

**الشرح بالعربية:**
- يدعم الشرط بدون أقواس أو مع أقواس مثل list comprehensions

### الشرح بالعربية
تضمينات القواميس تسمح بإنشاء قواميس جديدة بطريقة موجزة. الصيغة: `{مفتاح: قيمة لكل عنصر في قائمة}`



## 3️⃣ Generators (المولدات)

### Syntax (EBNF)
```ebnf
generator_function ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block_with_yield

yield_statement ::= 'أنتج' expression
                  | 'yield' expression

generator_expr ::= '(' expression 'لكل' IDENTIFIER 'في' iterable [if_clause] ')'

if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'
```

**الشرح بالعربية:**
- تعبيرات المولدات تدعم الأقواس الاختيارية في الشرط مثل list comprehensions

### الشرح بالعربية
المولدات (Generators) هي دوال خاصة تستخدم كلمة `أنتج` (yield) بدلاً من `إرجاع` (return). المولدات تُنتج قيماً واحدة تلو الأخرى وتحافظ على حالتها بين الاستدعاءات، مما يجعلها فعّالة للذاكرة عند العمل مع تسلسلات كبيرة.

### (EN) Explanation
Generators are special functions that use `أنتج` (yield) instead of `إرجاع` (return). They produce values one at a time and maintain their state between calls, making them memory-efficient for large sequences.

### Semantics
- **Lazy Evaluation:** Values are computed on-demand, not all at once
- **State Preservation:** Function state (local variables) preserved between yields
- **Memory Efficient:** Only one item in memory at a time
- **Iterable:** Generators are iterable and can be used in loops


## 4️⃣ Decorators (المزخرفات)

### Syntax (EBNF)
```ebnf
decorated_function ::= decorator+ function_decl

decorator ::= '@' decorator_name ['(' [arg_list] ')'] NEWLINE
            | '@' IDENTIFIER ['(' [arg_list] ')'] NEWLINE

function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
```

### الشرح بالعربية
المزخرفات (Decorators) هي دوال تُغلّف دوال أخرى لإضافة وظائف إضافية دون تعديل الدالة الأصلية. تُستخدم كثيراً للتسجيل، التخزين المؤقت، التحقق من الصلاحيات، وقياس الأداء.



## 5️⃣ Context Managers (مديرو السياق)

### Syntax (EBNF)
```ebnf
with_statement ::= 'مع' expression ['كـ' IDENTIFIER] block
                 | 'with' expression ['as' IDENTIFIER] block

context_manager_class ::= class_with_enter_exit_methods
```

### الشرح بالعربية
مديرو السياق (Context Managers) يوفرون طريقة نظيفة لإدارة الموارد (مثل الملفات، الاتصالات) مع ضمان تنظيفها حتى لو حدث خطأ.



## 6️⃣ Exceptions (الاستثناءات)

### Syntax (EBNF)
```ebnf
try_statement ::= 'جرب' block except_clauses [else_clause] [finally_clause]
                | 'try' block except_clauses ['else' block] ['finally' block]

except_clauses ::= except_clause+

except_clause ::= 'اعترض' [exception_type ['كـ' IDENTIFIER]] block
                | 'except' [exception_type ['as' IDENTIFIER]] block

else_clause ::= 'وإلا' block

finally_clause ::= 'أخيراً' block
                  | 'finally' block

raise_statement ::= 'ارفع' [expression]
                  | 'raise' [expression]
```

### الشرح بالعربية
نظام الاستثناءات يوفر طريقة منظمة للتعامل مع الأخطاء. يمكنك رفع استثناءات مخصصة واعتراضها والتعامل معها.




# ⚡ ميزات C++ في لغة "ص" / C++ Features in Sad Language



## 1️⃣ Optional Static Typing (الكتابة الثابتة الاختيارية)

### Syntax (EBNF)
```ebnf
typed_declaration ::= type IDENTIFIER '=' expression

type ::= 'رقم' | 'عشري' | 'نص' | 'منطقي' | 'مصفوفة' | 'قاموس'
       | 'int' | 'float' | 'string' | 'bool' | 'array' | 'dict'
       | generic_type

generic_type ::= type '<' type_list '>'

type_list ::= type ('،' type)*

// Function with typed parameters
typed_function ::= 'دالة' [return_type] IDENTIFIER '(' typed_param_list ')' block

typed_param_list ::= typed_param ('،' typed_param)*

typed_param ::= type IDENTIFIER
```

### الشرح بالعربية
الكتابة الثابتة الاختيارية تسمح للمطورين بتحديد الأنواع صراحةً للحصول على فحص أفضل للأخطاء وأداء محسّن. يمكن خلط الكتابة الثابتة والديناميكية في نفس البرنامج.

### (EN) Explanation
Optional static typing allows developers to explicitly specify types for better error checking and improved performance. Static and dynamic typing can be mixed in the same program.



## 2️⃣ Generics/Templates (القوالب)

### Syntax (EBNF)
```ebnf
generic_function ::= 'دالة' '<' type_params '>' [return_type] IDENTIFIER '(' [param_list] ')' block

type_params ::= type_param ('،' type_param)*

type_param ::= IDENTIFIER

generic_class ::= 'صنف' '<' type_params '>' IDENTIFIER class_body 'نهاية'
```

### الشرح بالعربية
القوالب (Generics) تسمح بكتابة كود يعمل مع أنواع متعددة دون تكرار. مفيدة جداً لهياكل البيانات والخوارزميات العامة.


## 3️⃣ RAII (Resource Acquisition Is Initialization)

### الشرح بالعربية
RAII هو نمط برمجي يربط دورة حياة الموارد (ملفات، ذاكرة، اتصالات) بدورة حياة الكائنات. عندما يُنشأ الكائن، يُحصّل المورد. عندما يُدمّر الكائن، يُحرّر المورد تلقائياً.


## 4️⃣ Operator Overloading (تحميل المعاملات)

### Syntax (EBNF)
```ebnf
operator_overload ::= 'عامل' operator '(' param_list ')' block

operator ::= '+' | '-' | '*' | '/' | '%' | '==' | '!=' | '<' | '>' | '<=' | '>=' | '[]'
```

## 5️⃣ Native Bindings (الربط الأصلي)

### الشرح بالعربية
الربط الأصلي يسمح باستدعاء دوال C/C++ مباشرة من كود "ص". مفيد للأداء العالي والوصول إلى مكتبات النظام.


# 📦 النماذج و FFI / Modules & FFI

## 1️⃣ Basic Import (الاستيراد الأساسي)

### Syntax

```ebnf
import_stmt ::= 'استورد' module_name ['كـ' alias]
              | 'من' module_name 'استورد' ('*' | identifier_list)
              
module_name ::= identifier ('.' identifier)*
identifier_list ::= identifier ('،' identifier)*
alias ::= identifier
```



## 3️⃣ Export System (نظام التصدير)

### Export Keyword

```ebnf
export_stmt ::= 'صدر' (var_decl | func_decl | class_decl)
```

### Visibility Rules

| الكلمة (AR) | Keyword (EN) | Visibility |
|-----------|-----------|-----------|
| `صدر` | export | Public - accessible from outside |
| (none) | (none) | Private - only within module |


# 🔧 دعم تطوير الأنظمة / System Development Support

## 1️⃣ المؤشرات / Pointers

### Syntax (EBNF)
```ebnf
pointer_type ::= 'مؤشر' '<' base_type '>'

pointer_declaration ::= pointer_type identifier ['=' pointer_expression]

base_type ::= 'رقم' | 'number' | 'عشري' | 'float' | 'نص' | 'string' 
            | 'منطقي' | 'boolean' | 'فارغ' | 'void'

pointer_expression ::= 'عنوان_من' '(' identifier ')' 
                     | null_literal
                     | pointer_arithmetic

dereference_expression ::= 'قيمة_من' '(' pointer_expression ')'
                         | '*' pointer_expression

write_to_pointer ::= 'اكتب_في' '(' pointer_expression ',' expression ')'

pointer_arithmetic ::= pointer_expression ('+' | '-') integer_expression
```

### Semantics
- **Size:** 8 bytes (64-bit address)
- **Default:** null (0x0)
- **Safety:** Runtime null checks
- **Operations:** address_of, value_of, write_to, arithmetic


## 2️⃣ العمليات الثنائية / Bitwise Operations

### Syntax (EBNF)
```ebnf
bitwise_and ::= expression '&' expression
bitwise_or  ::= expression '|' expression
bitwise_xor ::= expression '^' expression
bitwise_not ::= '~' expression

left_shift  ::= expression '<<' expression
right_shift ::= expression '>>' expression

rotate_left  ::= 'دوران_يسار' '(' expression ',' expression ')'
rotate_right ::= 'دوران_يمين' '(' expression ',' expression ')'
```

### Semantics
- **AND (&):** Logical AND on each bit
- **OR (|):** Logical OR on each bit  
- **XOR (^):** Exclusive OR on each bit
- **NOT (~):** Inverts all bits
- **Shifts:** << (left), >> (right)
- **Rotation:** دوران_يسار, دوران_يمين


## 3️⃣ إدارة الذاكرة / Memory Management

### Syntax (EBNF)
```ebnf
malloc_call  ::= 'خصص_ذاكرة' '(' size_expression ')'
free_call    ::= 'حرر_ذاكرة' '(' pointer_expression ')'
calloc_call  ::= 'خصص_وصفر' '(' count_expr ',' size_expr ')'
realloc_call ::= 'أعد_تخصيص' '(' pointer_expr ',' new_size_expr ')'

aligned_malloc ::= 'خصص_محاذي' '(' size_expr ',' alignment_expr ')'

mmap_call   ::= 'خريطة_ذاكرة' '(' address ',' size ',' protection ',' flags ')'
munmap_call ::= 'ألغ_خريطة' '(' pointer ',' size ')'
mprotect_call ::= 'حماية_ذاكرة' '(' pointer ',' size ',' protection ')'
```

### Semantics
- **Allocation:** malloc, calloc, realloc, aligned_malloc
- **Deallocation:** free
- **Memory Mapping:** mmap, munmap
- **Protection:** mprotect (read/write/execute permissions)


## 4️⃣ منافذ الإدخال/الإخراج / I/O Ports

### Syntax (EBNF)
```ebnf
port_read_8  ::= 'اقرأ_منفذ8' '(' port_number ')'
port_write_8 ::= 'اكتب_منفذ8' '(' port_number ',' value ')'

port_read_16  ::= 'اقرأ_منفذ16' '(' port_number ')'
port_write_16 ::= 'اكتب_منفذ16' '(' port_number ',' value ')'

port_read_32  ::= 'اقرأ_منفذ32' '(' port_number ')'
port_write_32 ::= 'اكتب_منفذ32' '(' port_number ',' value ')'

io_wait ::= 'انتظار_منفذ' '(' ')'
```

### Semantics
- **8-bit:** inb/outb operations
- **16-bit:** inw/outw operations
- **32-bit:** inl/outl operations
- **Wait:** io_wait for I/O delay


## 5️⃣ معالجة المقاطعات / Interrupt Handling

### Syntax (EBNF)
```ebnf
register_interrupt ::= 'سجل_مقاطعة' '(' interrupt_number ',' handler_function ')'

enable_interrupts  ::= 'فعل_مقاطعات' '(' ')'
disable_interrupts ::= 'عطل_مقاطعات' '(' ')'

interrupt_handler ::= 'معالج_مقاطعة' function_declaration

volatile_var ::= 'متطاير' type identifier
```

### Semantics
- **Registration:** Register ISR handlers
- **Enable/Disable:** Control interrupt state
- **Handlers:** Special functions for interrupts
- **Volatile:** Prevent compiler optimizations


## 6️⃣ الأنواع محددة الحجم / Fixed-Size Types

### Syntax (EBNF)
```ebnf
fixed_size_type ::= signed_fixed_type | unsigned_fixed_type

signed_fixed_type ::= 'صحيح8' | 'صحيح16' | 'صحيح32' | 'صحيح64'
                    | 'int8' | 'int16' | 'int32' | 'int64'

unsigned_fixed_type ::= 'غير_صحيح8' | 'غير_صحيح16' | 'غير_صحيح32' | 'غير_صحيح64'
                      | 'uint8' | 'uint16' | 'uint32' | 'uint64'
```

### Type Table

| النوع (AR) | Type (EN) | الحجم | Size | المدى | Range |
|-----------|-----------|------|------|-------|--------|
| صحيح8 | int8 | 1 byte | 1 byte | -128 to 127 | -128 to 127 |
| صحيح16 | int16 | 2 bytes | 2 bytes | -32,768 to 32,767 | -32,768 to 32,767 |
| صحيح32 | int32 | 4 bytes | 4 bytes | -2³¹ to 2³¹-1 | -2³¹ to 2³¹-1 |
| صحيح64 | int64 | 8 bytes | 8 bytes | -2⁶³ to 2⁶³-1 | -2⁶³ to 2⁶³-1 |
| غير_صحيح8 | uint8 | 1 byte | 1 byte | 0 to 255 | 0 to 255 |
| غير_صحيح16 | uint16 | 2 bytes | 2 bytes | 0 to 65,535 | 0 to 65,535 |
| غير_صحيح32 | uint32 | 4 bytes | 4 bytes | 0 to 2³²-1 | 0 to 2³²-1 |
| غير_صحيح64 | uint64 | 8 bytes | 8 bytes | 0 to 2⁶⁴-1 | 0 to 2⁶⁴-1 |

### Semantics
- **Fixed Size:** Guaranteed size across platforms
- **Alignment:** Properly aligned in memory
- **Operations:** All arithmetic and bitwise operations
- **Use Case:** Hardware registers, protocols, file formats
