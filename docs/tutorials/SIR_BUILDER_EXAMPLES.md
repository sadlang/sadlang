# SIR Builder Usage Examples
# أمثلة استخدام بناء SIR

**Version**: 1.0  
**Date**: December 3, 2025  
**Purpose**: Practical examples for using SIRBuilder

---

## 📋 Table of Contents

1. [Basic Expressions](#basic-expressions)
2. [Variables and Assignments](#variables-and-assignments)
3. [Control Flow](#control-flow)
4. [Functions](#functions)
5. [Classes and OOP](#classes-and-oop)
6. [Arrays and Data Structures](#arrays-and-data-structures)
7. [Generic Functions](#generic-functions)
8. [Type System](#type-system)
9. [Complete Programs](#complete-programs)

---

## Basic Expressions | التعابير الأساسية

### Example 1: Arithmetic Operations

**S Language Code**:
```s
var x = 10 + 20 * 3;
```

**Generated SIR**:
```sir
entry:
    %0 = LOAD_CONST 10
    %1 = LOAD_CONST 20
    %2 = LOAD_CONST 3
    %3 = MUL %1, %2        ; 20 * 3 = 60
    %4 = ADD %0, %3        ; 10 + 60 = 70
    %x = ALLOCA i64
    STORE %x, %4
```

**C++ Builder Code**:
```cpp
#include "sir_builder.h"
#include "parser.h"

int main() {
    const char* source = "var x = 10 + 20 * 3;";
    
    // Parse
    Parser parser;
    auto ast = parser.parse(source);
    
    // Build SIR
    SIRBuilder builder;
    auto module = builder.buildModule(ast);
    
    // Print
    std::cout << module->toString() << std::endl;
    
    return 0;
}
```

---

### Example 2: String Concatenation

**S Language Code**:
```s
var greeting = "Hello, " + "World!";
```

**Generated SIR**:
```sir
entry:
    %0 = LOAD_CONST "Hello, "
    %1 = LOAD_CONST "World!"
    %2 = STRING_CONCAT %0, %1
    %greeting = ALLOCA string
    STORE %greeting, %2
```

---

### Example 3: Boolean Logic

**S Language Code**:
```s
var result = (x > 10) && (y < 20);
```

**Generated SIR**:
```sir
entry:
    %0 = LOAD %x
    %1 = ICMP_GT %0, 10
    %2 = LOAD %y
    %3 = ICMP_LT %2, 20
    %4 = AND %1, %3
    %result = ALLOCA bool
    STORE %result, %4
```

---

## Variables and Assignments | المتغيرات والإسناد

### Example 4: Variable Declaration with Type

**S Language Code**:
```s
var count: int = 0;
var price: float = 99.99;
var name: string = "Product";
var active: bool = true;
```

**Generated SIR**:
```sir
entry:
    ; int variable
    %0 = LOAD_CONST 0
    %count = ALLOCA i64
    STORE %count, %0
    
    ; float variable
    %1 = LOAD_CONST 99.99
    %price = ALLOCA f64
    STORE %price, %1
    
    ; string variable
    %2 = LOAD_CONST "Product"
    %name = ALLOCA string
    STORE %name, %2
    
    ; bool variable
    %3 = LOAD_CONST true
    %active = ALLOCA bool
    STORE %active, %3
```

---

### Example 5: Multiple Assignments

**S Language Code**:
```s
var x = 10;
x = x + 5;
x = x * 2;
```

**Generated SIR**:
```sir
entry:
    ; var x = 10
    %0 = LOAD_CONST 10
    %x = ALLOCA i64
    STORE %x, %0
    
    ; x = x + 5
    %1 = LOAD %x
    %2 = ADD %1, 5
    STORE %x, %2
    
    ; x = x * 2
    %3 = LOAD %x
    %4 = MUL %3, 2
    STORE %x, %4
```

---

## Control Flow | التحكم في التدفق

### Example 6: If-Else Statement

**S Language Code**:
```s
if (x > 0) {
    print("Positive");
} else {
    print("Non-positive");
}
```

**Generated SIR**:
```sir
entry:
    %0 = LOAD %x
    %1 = ICMP_GT %0, 0
    BR %1, then_block_0, else_block_1

then_block_0:
    %2 = LOAD_CONST "Positive"
    CALL @print(%2)
    JUMP merge_block_2

else_block_1:
    %3 = LOAD_CONST "Non-positive"
    CALL @print(%3)
    JUMP merge_block_2

merge_block_2:
    ; continue...
```

---

### Example 7: While Loop

**S Language Code**:
```s
var i = 0;
while (i < 10) {
    print(i);
    i = i + 1;
}
```

**Generated SIR**:
```sir
entry:
    %0 = LOAD_CONST 0
    %i = ALLOCA i64
    STORE %i, %0
    JUMP loop_cond_0

loop_cond_0:
    %1 = LOAD %i
    %2 = ICMP_LT %1, 10
    BR %2, loop_body_1, loop_exit_2

loop_body_1:
    ; print(i)
    %3 = LOAD %i
    CALL @print(%3)
    
    ; i = i + 1
    %4 = LOAD %i
    %5 = ADD %4, 1
    STORE %i, %5
    
    JUMP loop_cond_0

loop_exit_2:
    ; continue...
```

---

### Example 8: For Loop

**S Language Code**:
```s
for (var i = 0; i < 5; i = i + 1) {
    print(i * i);
}
```

**Generated SIR**:
```sir
entry:
    ; Initialize: var i = 0
    %0 = LOAD_CONST 0
    %i = ALLOCA i64
    STORE %i, %0
    JUMP loop_cond_0

loop_cond_0:
    ; Condition: i < 5
    %1 = LOAD %i
    %2 = ICMP_LT %1, 5
    BR %2, loop_body_1, loop_exit_2

loop_body_1:
    ; Body: print(i * i)
    %3 = LOAD %i
    %4 = LOAD %i
    %5 = MUL %3, %4
    CALL @print(%5)
    
    ; Increment: i = i + 1
    %6 = LOAD %i
    %7 = ADD %6, 1
    STORE %i, %7
    
    JUMP loop_cond_0

loop_exit_2:
    ; continue...
```

---

## Functions | الدوال

### Example 9: Simple Function

**S Language Code**:
```s
function add(a: int, b: int) -> int {
    return a + b;
}

var result = add(10, 20);
```

**Generated SIR**:
```sir
; Function definition
function @add(i64 %a, i64 %b) -> i64 {
entry:
    %0 = ADD %a, %b
    RET %0
}

; Function call
entry:
    %0 = LOAD_CONST 10
    %1 = LOAD_CONST 20
    %2 = CALL @add(%0, %1)
    %result = ALLOCA i64
    STORE %result, %2
```

---

### Example 10: Function with Multiple Returns

**S Language Code**:
```s
function abs(x: int) -> int {
    if (x < 0) {
        return -x;
    }
    return x;
}
```

**Generated SIR**:
```sir
function @abs(i64 %x) -> i64 {
entry:
    %0 = ICMP_LT %x, 0
    BR %0, then_block_0, else_block_1

then_block_0:
    %1 = NEG %x
    RET %1

else_block_1:
    RET %x
}
```

---

### Example 11: Recursive Function

**S Language Code**:
```s
function factorial(n: int) -> int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

**Generated SIR**:
```sir
function @factorial(i64 %n) -> i64 {
entry:
    %0 = ICMP_LE %n, 1
    BR %0, base_case_0, recursive_case_1

base_case_0:
    %1 = LOAD_CONST 1
    RET %1

recursive_case_1:
    %2 = SUB %n, 1
    %3 = CALL @factorial(%2)
    %4 = MUL %n, %3
    RET %4
}
```

---

## Classes and OOP | الأصناف والبرمجة الكائنية

### Example 12: Simple Class

**S Language Code**:
```s
class Point {
    x: int;
    y: int;
    
    function getX() -> int {
        return x;
    }
    
    function setX(newX: int) {
        x = newX;
    }
}

var p = new Point();
p.setX(10);
var value = p.getX();
```

**Generated SIR**:
```sir
; Class structure
struct @Point {
    i64 x
    i64 y
}

; Method: getX
function @Point_getX(ptr %this) -> i64 {
entry:
    %0 = MEMBER_ACCESS %this, 0  ; field x
    %1 = LOAD %0
    RET %1
}

; Method: setX
function @Point_setX(ptr %this, i64 %newX) -> void {
entry:
    %0 = MEMBER_ACCESS %this, 0  ; field x
    STORE %0, %newX
    RET
}

; Usage
entry:
    ; var p = new Point()
    %0 = NEW @Point
    %p = ALLOCA ptr
    STORE %p, %0
    
    ; p.setX(10)
    %1 = LOAD %p
    %2 = LOAD_CONST 10
    CALL @Point_setX(%1, %2)
    
    ; var value = p.getX()
    %3 = LOAD %p
    %4 = CALL @Point_getX(%3)
    %value = ALLOCA i64
    STORE %value, %4
```

---

### Example 13: Class with Constructor

**S Language Code**:
```s
class Rectangle {
    width: int;
    height: int;
    
    function constructor(w: int, h: int) {
        width = w;
        height = h;
    }
    
    function area() -> int {
        return width * height;
    }
}

var rect = new Rectangle(5, 10);
var a = rect.area();
```

**Generated SIR**:
```sir
struct @Rectangle {
    i64 width
    i64 height
}

; Constructor
function @Rectangle_constructor(ptr %this, i64 %w, i64 %h) -> void {
entry:
    %0 = MEMBER_ACCESS %this, 0  ; width
    STORE %0, %w
    
    %1 = MEMBER_ACCESS %this, 1  ; height
    STORE %1, %h
    
    RET
}

; Method: area
function @Rectangle_area(ptr %this) -> i64 {
entry:
    %0 = MEMBER_ACCESS %this, 0  ; width
    %1 = LOAD %0
    
    %2 = MEMBER_ACCESS %this, 1  ; height
    %3 = LOAD %2
    
    %4 = MUL %1, %3
    RET %4
}

; Usage
entry:
    ; var rect = new Rectangle(5, 10)
    %0 = NEW @Rectangle
    %1 = LOAD_CONST 5
    %2 = LOAD_CONST 10
    CALL @Rectangle_constructor(%0, %1, %2)
    %rect = ALLOCA ptr
    STORE %rect, %0
    
    ; var a = rect.area()
    %3 = LOAD %rect
    %4 = CALL @Rectangle_area(%3)
    %a = ALLOCA i64
    STORE %a, %4
```

---

## Arrays and Data Structures | المصفوفات وهياكل البيانات

### Example 14: Array Creation and Access

**S Language Code**:
```s
var numbers = [1, 2, 3, 4, 5];
var first = numbers[0];
numbers[2] = 10;
```

**Generated SIR**:
```sir
entry:
    ; Create array
    %0 = LOAD_CONST 1
    %1 = LOAD_CONST 2
    %2 = LOAD_CONST 3
    %3 = LOAD_CONST 4
    %4 = LOAD_CONST 5
    %5 = ARRAY_CREATE i64, 5
    ARRAY_SET %5, 0, %0
    ARRAY_SET %5, 1, %1
    ARRAY_SET %5, 2, %2
    ARRAY_SET %5, 3, %3
    ARRAY_SET %5, 4, %4
    %numbers = ALLOCA ptr
    STORE %numbers, %5
    
    ; Access: var first = numbers[0]
    %6 = LOAD %numbers
    %7 = ARRAY_GET %6, 0
    %first = ALLOCA i64
    STORE %first, %7
    
    ; Modify: numbers[2] = 10
    %8 = LOAD %numbers
    %9 = LOAD_CONST 10
    ARRAY_SET %8, 2, %9
```

---

### Example 15: Array Iteration

**S Language Code**:
```s
var arr = [10, 20, 30];
for (var i = 0; i < 3; i = i + 1) {
    var value = arr[i];
    print(value);
}
```

**Generated SIR**:
```sir
entry:
    ; Create array
    %0 = ARRAY_CREATE i64, 3
    ARRAY_SET %0, 0, 10
    ARRAY_SET %0, 1, 20
    ARRAY_SET %0, 2, 30
    %arr = ALLOCA ptr
    STORE %arr, %0
    
    ; Initialize loop
    %1 = LOAD_CONST 0
    %i = ALLOCA i64
    STORE %i, %1
    JUMP loop_cond_0

loop_cond_0:
    %2 = LOAD %i
    %3 = ICMP_LT %2, 3
    BR %3, loop_body_1, loop_exit_2

loop_body_1:
    ; var value = arr[i]
    %4 = LOAD %arr
    %5 = LOAD %i
    %6 = ARRAY_GET %4, %5
    %value = ALLOCA i64
    STORE %value, %6
    
    ; print(value)
    %7 = LOAD %value
    CALL @print(%7)
    
    ; i = i + 1
    %8 = LOAD %i
    %9 = ADD %8, 1
    STORE %i, %9
    
    JUMP loop_cond_0

loop_exit_2:
    ; continue...
```

---

## Generic Functions | الدوال العامة

### Example 16: Generic Identity Function

**S Language Code**:
```s
function identity<T>(x: T) -> T {
    return x;
}

var a = identity<int>(42);
var b = identity<string>("hello");
```

**Generated SIR**:
```sir
; Specialized for int
function @identity_int(i64 %x) -> i64 {
entry:
    RET %x
}

; Specialized for string
function @identity_string(string %x) -> string {
entry:
    RET %x
}

; Calls
entry:
    ; var a = identity<int>(42)
    %0 = LOAD_CONST 42
    %1 = CALL @identity_int(%0)
    %a = ALLOCA i64
    STORE %a, %1
    
    ; var b = identity<string>("hello")
    %2 = LOAD_CONST "hello"
    %3 = CALL @identity_string(%2)
    %b = ALLOCA string
    STORE %b, %3
```

---

### Example 17: Generic Swap Function

**S Language Code**:
```s
function swap<T>(a: T, b: T) -> void {
    var temp = a;
    a = b;
    b = temp;
}

var x = 10;
var y = 20;
swap<int>(x, y);
```

**Generated SIR**:
```sir
; Generic function (specialized for int)
function @swap_int(ptr %a, ptr %b) -> void {
entry:
    ; var temp = a
    %0 = LOAD %a
    %temp = ALLOCA i64
    STORE %temp, %0
    
    ; a = b
    %1 = LOAD %b
    STORE %a, %1
    
    ; b = temp
    %2 = LOAD %temp
    STORE %b, %2
    
    RET
}

; Call
entry:
    %0 = LOAD_CONST 10
    %x = ALLOCA i64
    STORE %x, %0
    
    %1 = LOAD_CONST 20
    %y = ALLOCA i64
    STORE %y, %1
    
    ; Get addresses for swap
    %2 = ADDR_OF %x
    %3 = ADDR_OF %y
    CALL @swap_int(%2, %3)
```

---

### Example 18: Generic Array Map

**S Language Code**:
```s
function map<T, U>(arr: T[], fn: (T)->U) -> U[] {
    var result: U[] = [];
    for (var i = 0; i < length(arr); i = i + 1) {
        var elem = arr[i];
        var mapped = fn(elem);
        result[i] = mapped;
    }
    return result;
}

function double(x: int) -> int {
    return x * 2;
}

var numbers = [1, 2, 3];
var doubled = map<int, int>(numbers, double);
```

**Generated SIR** (simplified):
```sir
; Specialized map for int->int
function @map_int_int(ptr %arr, ptr %fn) -> ptr {
entry:
    ; var result = []
    %0 = ARRAY_CREATE i64, 0
    %result = ALLOCA ptr
    STORE %result, %0
    
    ; Initialize loop
    %1 = LOAD_CONST 0
    %i = ALLOCA i64
    STORE %i, %1
    JUMP loop_cond_0

loop_cond_0:
    %2 = LOAD %i
    %3 = LOAD %arr
    %4 = ARRAY_LENGTH %3
    %5 = ICMP_LT %2, %4
    BR %5, loop_body_1, loop_exit_2

loop_body_1:
    ; var elem = arr[i]
    %6 = LOAD %arr
    %7 = LOAD %i
    %8 = ARRAY_GET %6, %7
    
    ; var mapped = fn(elem)
    %9 = LOAD %fn
    %10 = CALL_INDIRECT %9(%8)
    
    ; result[i] = mapped
    %11 = LOAD %result
    %12 = LOAD %i
    ARRAY_SET %11, %12, %10
    
    ; i++
    %13 = LOAD %i
    %14 = ADD %13, 1
    STORE %i, %14
    
    JUMP loop_cond_0

loop_exit_2:
    %15 = LOAD %result
    RET %15
}
```

---

## Type System | نظام الأنواع

### Example 19: Type Conversions

**S Language Code**:
```s
var i: int = 42;
var f: float = i;        // int -> float
var s: string = "123";
var n: int = parseInt(s); // string -> int
```

**Generated SIR**:
```sir
entry:
    ; var i: int = 42
    %0 = LOAD_CONST 42
    %i = ALLOCA i64
    STORE %i, %0
    
    ; var f: float = i (implicit conversion)
    %1 = LOAD %i
    %2 = I64_TO_F64 %1
    %f = ALLOCA f64
    STORE %f, %2
    
    ; var s: string = "123"
    %3 = LOAD_CONST "123"
    %s = ALLOCA string
    STORE %s, %3
    
    ; var n: int = parseInt(s)
    %4 = LOAD %s
    %5 = CALL @parseInt(%4)
    %n = ALLOCA i64
    STORE %n, %5
```

---

### Example 20: Type Checking

**S Language Code** (with error):
```s
var x: int = "hello";  // ERROR: Type mismatch
```

**Builder Error**:
```
Error: Type mismatch in variable declaration
  Expected: int
  Got: string
  Variable: x
```

---

## Complete Programs | برامج كاملة

### Example 21: Fibonacci Sequence

**S Language Code**:
```s
function fib(n: int) -> int {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

function main() {
    for (var i = 0; i < 10; i = i + 1) {
        var result = fib(i);
        print(result);
    }
}
```

**Generated SIR**:
```sir
; Fibonacci function
function @fib(i64 %n) -> i64 {
entry:
    %0 = ICMP_LE %n, 1
    BR %0, base_case_0, recursive_case_1

base_case_0:
    RET %n

recursive_case_1:
    %1 = SUB %n, 1
    %2 = CALL @fib(%1)
    
    %3 = SUB %n, 2
    %4 = CALL @fib(%3)
    
    %5 = ADD %2, %4
    RET %5
}

; Main function
function @main() -> void {
entry:
    ; Initialize loop
    %0 = LOAD_CONST 0
    %i = ALLOCA i64
    STORE %i, %0
    JUMP loop_cond_0

loop_cond_0:
    %1 = LOAD %i
    %2 = ICMP_LT %1, 10
    BR %2, loop_body_1, loop_exit_2

loop_body_1:
    ; var result = fib(i)
    %3 = LOAD %i
    %4 = CALL @fib(%3)
    %result = ALLOCA i64
    STORE %result, %4
    
    ; print(result)
    %5 = LOAD %result
    CALL @print(%5)
    
    ; i++
    %6 = LOAD %i
    %7 = ADD %6, 1
    STORE %i, %7
    
    JUMP loop_cond_0

loop_exit_2:
    RET
}
```

---

### Example 22: Calculator Class

**S Language Code**:
```s
class Calculator {
    lastResult: float;
    
    function add(a: float, b: float) -> float {
        lastResult = a + b;
        return lastResult;
    }
    
    function multiply(a: float, b: float) -> float {
        lastResult = a * b;
        return lastResult;
    }
    
    function getLastResult() -> float {
        return lastResult;
    }
}

function main() {
    var calc = new Calculator();
    var sum = calc.add(10.5, 20.3);
    var product = calc.multiply(5.0, 4.0);
    var last = calc.getLastResult();
    print(last);
}
```

**Generated SIR**:
```sir
; Class structure
struct @Calculator {
    f64 lastResult
}

; Method: add
function @Calculator_add(ptr %this, f64 %a, f64 %b) -> f64 {
entry:
    %0 = ADD %a, %b
    %1 = MEMBER_ACCESS %this, 0  ; lastResult
    STORE %1, %0
    
    %2 = LOAD %1
    RET %2
}

; Method: multiply
function @Calculator_multiply(ptr %this, f64 %a, f64 %b) -> f64 {
entry:
    %0 = MUL %a, %b
    %1 = MEMBER_ACCESS %this, 0  ; lastResult
    STORE %1, %0
    
    %2 = LOAD %1
    RET %2
}

; Method: getLastResult
function @Calculator_getLastResult(ptr %this) -> f64 {
entry:
    %0 = MEMBER_ACCESS %this, 0
    %1 = LOAD %0
    RET %1
}

; Main function
function @main() -> void {
entry:
    ; var calc = new Calculator()
    %0 = NEW @Calculator
    %calc = ALLOCA ptr
    STORE %calc, %0
    
    ; var sum = calc.add(10.5, 20.3)
    %1 = LOAD %calc
    %2 = LOAD_CONST 10.5
    %3 = LOAD_CONST 20.3
    %4 = CALL @Calculator_add(%1, %2, %3)
    %sum = ALLOCA f64
    STORE %sum, %4
    
    ; var product = calc.multiply(5.0, 4.0)
    %5 = LOAD %calc
    %6 = LOAD_CONST 5.0
    %7 = LOAD_CONST 4.0
    %8 = CALL @Calculator_multiply(%5, %6, %7)
    %product = ALLOCA f64
    STORE %product, %8
    
    ; var last = calc.getLastResult()
    %9 = LOAD %calc
    %10 = CALL @Calculator_getLastResult(%9)
    %last = ALLOCA f64
    STORE %last, %10
    
    ; print(last)
    %11 = LOAD %last
    CALL @print(%11)
    
    RET
}
```

---

## Advanced Examples | أمثلة متقدمة

### Example 23: Generic Stack

**S Language Code**:
```s
class Stack<T> {
    items: T[];
    size: int;
    
    function constructor() {
        items = [];
        size = 0;
    }
    
    function push(item: T) {
        items[size] = item;
        size = size + 1;
    }
    
    function pop() -> T {
        if (size > 0) {
            size = size - 1;
            return items[size];
        }
        return null;
    }
}

function main() {
    var stack = new Stack<int>();
    stack.push(10);
    stack.push(20);
    var value = stack.pop();
    print(value);  // Output: 20
}
```

**Generated SIR** (specialized for int):
```sir
; Specialized Stack<int>
struct @Stack_int {
    ptr items     ; i64[]
    i64 size
}

; Constructor
function @Stack_int_constructor(ptr %this) -> void {
entry:
    ; items = []
    %0 = ARRAY_CREATE i64, 0
    %1 = MEMBER_ACCESS %this, 0
    STORE %1, %0
    
    ; size = 0
    %2 = LOAD_CONST 0
    %3 = MEMBER_ACCESS %this, 1
    STORE %3, %2
    
    RET
}

; Method: push
function @Stack_int_push(ptr %this, i64 %item) -> void {
entry:
    ; items[size] = item
    %0 = MEMBER_ACCESS %this, 0  ; items
    %1 = LOAD %0
    %2 = MEMBER_ACCESS %this, 1  ; size
    %3 = LOAD %2
    ARRAY_SET %1, %3, %item
    
    ; size = size + 1
    %4 = ADD %3, 1
    STORE %2, %4
    
    RET
}

; Method: pop
function @Stack_int_pop(ptr %this) -> i64 {
entry:
    ; if (size > 0)
    %0 = MEMBER_ACCESS %this, 1  ; size
    %1 = LOAD %0
    %2 = ICMP_GT %1, 0
    BR %2, then_block_0, else_block_1

then_block_0:
    ; size = size - 1
    %3 = SUB %1, 1
    STORE %0, %3
    
    ; return items[size]
    %4 = MEMBER_ACCESS %this, 0  ; items
    %5 = LOAD %4
    %6 = ARRAY_GET %5, %3
    RET %6

else_block_1:
    ; return null (0 for int)
    %7 = LOAD_CONST 0
    RET %7
}
```

---

## Testing with SIRBuilder | الاختبار مع بناء SIR

### Example 24: Complete Test

**C++ Test Code**:
```cpp
#include "sir_builder.h"
#include "parser.h"
#include <cassert>

void test_complete_program() {
    const char* source = R"(
        function factorial(n: int) -> int {
            if (n <= 1) {
                return 1;
            }
            return n * factorial(n - 1);
        }
        
        function main() {
            var result = factorial(5);
            print(result);  // Should print 120
        }
    )";
    
    // Parse
    Parser parser;
    auto ast = parser.parse(source);
    assert(ast != nullptr);
    
    // Build SIR
    SIRBuilder builder;
    auto module = builder.buildModule(ast);
    
    // Verify no errors
    assert(!builder.hasErrors());
    assert(module != nullptr);
    
    // Verify functions exist
    auto factorial = module->getFunction("factorial");
    auto main = module->getFunction("main");
    assert(factorial != nullptr);
    assert(main != nullptr);
    
    // Verify factorial has correct signature
    assert(factorial->getParameterCount() == 1);
    assert(factorial->getReturnType() == SIRType::I64);
    
    // Verify factorial has correct blocks
    auto blocks = factorial->getBlocks();
    assert(blocks.size() >= 2);  // base case + recursive case
    
    // Print SIR for manual verification
    std::cout << "=== Generated SIR ===" << std::endl;
    std::cout << module->toString() << std::endl;
    
    std::cout << "✅ Test passed!" << std::endl;
}

int main() {
    test_complete_program();
    return 0;
}
```

---

## Summary | الملخص

This document covered:
- ✅ Basic expressions and operators
- ✅ Variables and assignments
- ✅ Control flow (if, while, for)
- ✅ Functions (simple, recursive)
- ✅ Classes and OOP
- ✅ Arrays and data structures
- ✅ Generic types and functions
- ✅ Type system and conversions
- ✅ Complete programs
- ✅ Testing strategies

**Key Takeaways**:
1. SIRBuilder converts AST to three-address code (TAC)
2. Virtual registers are unlimited (optimizer reduces later)
3. Control flow creates explicit CFG with blocks
4. Generic types are specialized at call sites
5. Type checking happens during SIR generation
6. Error accumulation allows multiple error reports

**Next Steps**:
- Study [SIR Builder API](../api/SIR_BUILDER_API.md)
- Read [Developer Guide](SIR_BUILDER_DEVELOPER_GUIDE.md)
- Explore [Architecture Diagrams](../architecture/)

---

**Document Version**: 1.0  
**Last Updated**: December 3, 2025  
**Maintained By**: SadLanguage Compiler Team
