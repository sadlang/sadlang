# Merged Grammar Rules - Sad Language (ص)

## Data Types (من 01_types.md)

```ebnf
integer_literal ::= decimal_literal | binary_literal | octal_literal | hex_literal
decimal_literal ::= ['+' | '-'] digit+
binary_literal  ::= '0b' ('0' | '1')+
octal_literal   ::= '0o' [0-7]+
hex_literal     ::= '0x' [0-9a-fA-F]+
digit ::= [0-9]
float_literal ::= ['+' | '-'] digit+ '.' digit+ [exponent]
                | ['+' | '-'] digit+ exponent
exponent ::= ('e' | 'E') ['+' | '-'] digit+
boolean_literal ::= 'صحيح' | 'خطأ' | 'true' | 'false'
string_literal ::= '"' string_char* '"'
                 | "'" string_char* "'"
                 | '"' multiline_char* '"'
                 | ''' multiline_char* '''
                 | '"' fstring_char* '"'
string_char    ::= any_unicode_char | escape_sequence
escape_sequence ::= '\n' | '\t' | '\r' | '\\' | '\"' | '\''
none_literal ::= 'لاشيء' | 'none' | 'null'
array_literal ::= '[' [expression (',' expression)*] ']'
array_access  ::= expression '[' expression ']'
map_literal ::= '{' [map_pair (',' map_pair)*] '}'
map_pair    ::= expression ':' expression
map_access  ::= expression '[' expression ']'
```

## Functions (من 02_functions.md)

```ebnf
function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
param_list ::= param ((',' | '،') param)*
param      ::= IDENTIFIER [':' type] ['=' expression]
block ::= statement* 'نهاية'
param_with_default ::= IDENTIFIER [':' type] '=' expression
lambda_expr ::= 'دالة' [type] '(' [param_list] ')' '=>' expression
              | '(' [param_list] ')' '=>' expression
```

## OOP (من 03_oop.md)

```ebnf
class_decl ::= 'صنف' IDENTIFIER [(':' | 'يرث') base_class_list] class_body 'نهاية'
base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*
class_body ::= (field_decl | method_decl | constructor_decl | destructor_decl)*
field_decl ::= [visibility] type IDENTIFIER ['=' expression] (';' | '؛')?
method_decl ::= [visibility] 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
constructor_decl ::= ('باني' | 'منشئ') '(' [param_list] ')' [':' initializer_list] block
destructor_decl ::= ('~باني' | '~منشئ' | 'مدمر') '()' block
visibility ::= 'عام' | 'خاص' | 'محمي'
initializer_list ::= IDENTIFIER '(' expression ')' ((',' | '،') IDENTIFIER '(' expression ')')*
param_list ::= param ((',' | '،') param)*
static_field ::= 'ثابت' type IDENTIFIER '=' expression (';' | '؛')?
static_method ::= 'ثابت' 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
static_access ::= ClassName '.' static_member_name
param_list ::= param ((',' | '،') param)*
```

## General Syntax (من 04_syntax.md)

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

## Python Features (من 05_python_features.md)

```ebnf
list_comprehension ::= '[' expression 'لكل' IDENTIFIER 'في' iterable [if_clause] ']'
if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'
condition ::= expression
list_comprehension ::= '[' expression 'for' IDENTIFIER 'in' iterable ['if' '(' condition ')' | 'if' condition] ']'
dict_comprehension ::= '{' key_expr ':' value_expr 'لكل' IDENTIFIER 'في' iterable [if_clause] '}'
if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'
dict_comprehension ::= '{' key_expr ':' value_expr 'for' IDENTIFIER 'in' iterable ['if' '(' condition ')' | 'if' condition] '}'
generator_function ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block_with_yield
yield_statement ::= 'أنتج' expression
                  | 'yield' expression
generator_expr ::= '(' expression 'لكل' IDENTIFIER 'في' iterable [if_clause] ')'
if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'
decorated_function ::= decorator+ function_decl
decorator ::= '@' decorator_name ['(' [arg_list] ')'] NEWLINE
            | '@' IDENTIFIER ['(' [arg_list] ')'] NEWLINE
function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
with_statement ::= 'مع' expression ['كـ' IDENTIFIER] block
                 | 'with' expression ['as' IDENTIFIER] block
context_manager_class ::= class_with_enter_exit_methods
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

## C++ Features (من 06_cpp_features.md)

```ebnf
typed_declaration ::= type IDENTIFIER '=' expression
type ::= 'رقم' | 'عشري' | 'نص' | 'منطقي' | 'مصفوفة' | 'قاموس'
       | 'int' | 'float' | 'string' | 'bool' | 'array' | 'dict'
       | generic_type
generic_type ::= type '<' type_list '>'
type_list ::= type ('،' type)*
typed_function ::= 'دالة' [return_type] IDENTIFIER '(' typed_param_list ')' block
typed_param_list ::= typed_param ('،' typed_param)*
typed_param ::= type IDENTIFIER
generic_function ::= 'دالة' '<' type_params '>' [return_type] IDENTIFIER '(' [param_list] ')' block
type_params ::= type_param ('،' type_param)*
type_param ::= IDENTIFIER
generic_class ::= 'صنف' '<' type_params '>' IDENTIFIER class_body 'نهاية'
operator_overload ::= 'عامل' operator '(' param_list ')' block
operator ::= '+' | '-' | '*' | '/' | '%' | '==' | '!=' | '<' | '>' | '<=' | '>=' | '[]'
```

## Graphics (من 07_graphics.md)

```ebnf
window_creation ::= 'نافذة' '(' width ',' height ',' title ')'
window_methods ::= window '.' method_name '(' [args] ')'
```

## Modules & FFI (من 08_modules_and_ffi.md)

```ebnf
import_stmt ::= 'استورد' module_name ['كـ' alias]
              | 'من' module_name 'استورد' ('*' | identifier_list)
module_name ::= identifier ('.' identifier)*
identifier_list ::= identifier ('،' identifier)*
alias ::= identifier
export_stmt ::= 'صدر' (var_decl | func_decl | class_decl)
```

## Comments (من 04_syntax.md)

```ebnf
comment ::= line_comment | block_comment | doc_comment
line_comment  ::= '#' any_char* NEWLINE
block_comment ::= '#*' any_char* '*#'
doc_comment   ::= '##' any_char* NEWLINE
                | '#**' any_char* '**#'
```
