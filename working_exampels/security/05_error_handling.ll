; ModuleID = '05_error_handling'
source_filename = "05_error_handling"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [58 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9 \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86 ===\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [44 x i8] c"--- \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 1: \D8\AA\D8\A3\D9\83\D9\8A\D8\AF \D9\81\D8\A7\D8\B4\D9\84 ---\00", align 1
@str.const.7 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.11 = private unnamed_addr constant [31 x i8] c"--- \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 2: \D8\B0\D8\B9\D8\B1 ---\00", align 1
@str.const.15 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.19 = private unnamed_addr constant [40 x i8] c"--- \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 3: \D9\86\D9\88\D8\B9 \D8\AE\D8\A7\D8\B7\D8\A6 ---\00", align 1
@str.const.23 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.27 = private unnamed_addr constant [48 x i8] c"--- \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 4: \D9\85\D8\B3\D8\A7\D9\88\D8\A7\D8\A9 \D9\81\D8\A7\D8\B4\D9\84\D8\A9 ---\00", align 1
@str.const.31 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.35 = private unnamed_addr constant [48 x i8] c"--- \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 5: \D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9 \D9\81\D8\A7\D8\B4\D9\84\D8\A9 ---\00", align 1
@str.const.39 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.43 = private unnamed_addr constant [64 x i8] c"\D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D9\85\D8\B9\D8\A7\D9\84\D8\AC\D8\A9 \D8\A7\D9\84\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1 \D9\86\D8\AC\D8\AD\D8\AA!\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.7)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.19)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.23)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.27)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.35)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
