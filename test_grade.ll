; ModuleID = 'working_exampels\14_tested\05_grade_calculator.sad'
source_filename = "working_exampels\\14_tested\\05_grade_calculator.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [28 x i8] c"===========================\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [31 x i8] c"     \D8\AD\D8\A7\D8\B3\D8\A8\D8\A9 \D8\A7\D9\84\D8\AF\D8\B1\D8\AC\D8\A7\D8\AA\00", align 1
@str.const.11 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.15 = private unnamed_addr constant [31 x i8] c"\D8\A7\D8\AF\D8\AE\D9\84 \D8\A7\D8\B3\D9\85 \D8\A7\D9\84\D8\B7\D8\A7\D9\84\D8\A8: \00", align 1
@str.const.17 = private unnamed_addr constant [47 x i8] c"\D8\A7\D8\AF\D8\AE\D9\84 \D8\AF\D8\B1\D8\AC\D8\A9 \D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA (0-100): \00", align 1
@str.const.19 = private unnamed_addr constant [41 x i8] c"\D8\A7\D8\AF\D8\AE\D9\84 \D8\AF\D8\B1\D8\AC\D8\A9 \D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85 (0-100): \00", align 1
@str.const.21 = private unnamed_addr constant [39 x i8] c"\D8\A7\D8\AF\D8\AE\D9\84 \D8\AF\D8\B1\D8\AC\D8\A9 \D8\A7\D9\84\D9\84\D8\BA\D8\A9 (0-100): \00", align 1
@str.const.23 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.31 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\B7\D8\A7\D9\84\D8\A8: \00", align 1
@str.const.40 = private unnamed_addr constant [21 x i8] c"\D8\A7\D9\84\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA: \00", align 1
@fmt.d = private unnamed_addr constant [6 x i8] c"%lld\0A\00", align 1
@str.const.42 = private unnamed_addr constant [10 x i8] c" \D8\AF\D8\B1\D8\AC\D8\A9\00", align 1
@str.const.46 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\B9\D9\84\D9\88\D9\85: \00", align 1
@str.const.53 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\84\D8\BA\D8\A9: \00", align 1
@str.const.60 = private unnamed_addr constant [28 x i8] c"---------------------------\00", align 1
@str.const.64 = private unnamed_addr constant [17 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1
@str.const.67 = private unnamed_addr constant [7 x i8] c" \D9\85\D9\86 \00", align 1
@str.const.70 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.74 = private unnamed_addr constant [33 x i8] c"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\8A\D8\B1: \D9\85\D9\85\D8\AA\D8\A7\D8\B2! \F0\9F\8C\9F\00", align 1
@str.const.82 = private unnamed_addr constant [30 x i8] c"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\8A\D8\B1: \D8\AC\D9\8A\D8\AF \D8\AC\D8\AF\D8\A7\00", align 1
@str.const.86 = private unnamed_addr constant [23 x i8] c"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\8A\D8\B1: \D8\AC\D9\8A\D8\AF\00", align 1
@str.const.90 = private unnamed_addr constant [27 x i8] c"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\8A\D8\B1: \D9\85\D9\82\D8\A8\D9\88\D9\84\00", align 1
@str.const.94 = private unnamed_addr constant [29 x i8] c"\D8\A7\D9\84\D8\AA\D9\82\D8\AF\D9\8A\D8\B1: \D8\B1\D8\A7\D8\B3\D8\A8 \E2\9D\8C\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.11)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.15)
  %read.buf = call ptr @malloc(i64 1024)
  %stdin.ptr = call ptr @__acrt_iob_func(i32 0)
  %9 = call ptr @fgets(ptr %read.buf, i32 1024, ptr %stdin.ptr)
  %alloca = alloca ptr, align 8
  store ptr %read.buf, ptr %alloca, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.17)
  %read.buf1 = call ptr @malloc(i64 1024)
  %stdin.ptr2 = call ptr @__acrt_iob_func(i32 0)
  %11 = call ptr @fgets(ptr %read.buf1, i32 1024, ptr %stdin.ptr2)
  %alloca3 = alloca i64, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.19)
  %read.buf4 = call ptr @malloc(i64 1024)
  %stdin.ptr5 = call ptr @__acrt_iob_func(i32 0)
  %13 = call ptr @fgets(ptr %read.buf4, i32 1024, ptr %stdin.ptr5)
  %alloca6 = alloca i64, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.21)
  %read.buf7 = call ptr @malloc(i64 1024)
  %stdin.ptr8 = call ptr @__acrt_iob_func(i32 0)
  %15 = call ptr @fgets(ptr %read.buf7, i32 1024, ptr %stdin.ptr8)
  %alloca9 = alloca i64, align 8
  %"%\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load" = load i64, ptr %alloca3, align 8
  %"%\D8\B9\D9\84\D9\88\D9\85.load" = load i64, ptr %alloca6, align 8
  %addtmp = add i64 %"%\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load", %"%\D8\B9\D9\84\D9\88\D9\85.load"
  %"%\D9\84\D8\BA\D8\A9.load" = load i64, ptr %alloca9, align 8
  %addtmp10 = add i64 %addtmp, %"%\D9\84\D8\BA\D8\A9.load"
  %alloca11 = alloca i64, align 8
  store i64 %addtmp10, ptr %alloca11, align 8
  %alloca12 = alloca i64, align 8
  store i64 3, ptr %alloca12, align 8
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.23)
  %17 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %"%\D8\A7\D8\B3\D9\85.load" = load ptr, ptr %alloca, align 8
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D8\A7\D8\B3\D9\85.load")
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %25 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %"%\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load13" = load i64, ptr %alloca3, align 8
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B1\D9\8A\D8\A7\D8\B6\D9\8A\D8\A7\D8\AA.load13")
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.42)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.46)
  %"%\D8\B9\D9\84\D9\88\D9\85.load14" = load i64, ptr %alloca6, align 8
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D8\B9\D9\84\D9\88\D9\85.load14")
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.42)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %"%\D9\84\D8\BA\D8\A9.load15" = load i64, ptr %alloca9, align 8
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\84\D8\BA\D8\A9.load15")
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.42)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.64)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load" = load i64, ptr %alloca11, align 8
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load")
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.67)
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\AF.load" = load i64, ptr %alloca12, align 8
  %multmp = mul i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\88\D8\A7\D8\AF.load", 100
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %multmp)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.70)
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load16" = load i64, ptr %alloca11, align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load16", 270
  br i1 %cmpgetmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.74)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

else_2:                                           ; preds = %entry
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load17" = load i64, ptr %alloca11, align 8
  %cmpgetmp18 = icmp sge i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load17", 240
  br i1 %cmpgetmp18, label %then_3, label %else_5

merge_1:                                          ; preds = %merge_4, %then_0
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0

then_3:                                           ; preds = %else_2
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.82)
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_4

else_5:                                           ; preds = %else_2
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load19" = load i64, ptr %alloca11, align 8
  %cmpgetmp20 = icmp sge i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load19", 210
  br i1 %cmpgetmp20, label %then_6, label %else_8

merge_4:                                          ; preds = %merge_7, %then_3
  br label %merge_1

then_6:                                           ; preds = %else_5
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.86)
  %52 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_7

else_8:                                           ; preds = %else_5
  %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load21" = load i64, ptr %alloca11, align 8
  %cmpgetmp22 = icmp sge i64 %"%\D9\85\D8\AC\D9\85\D9\88\D8\B9.load21", 150
  br i1 %cmpgetmp22, label %then_9, label %else_11

merge_7:                                          ; preds = %merge_10, %then_6
  br label %merge_4

then_9:                                           ; preds = %else_8
  %53 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.90)
  %54 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_10

else_11:                                          ; preds = %else_8
  %55 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.94)
  %56 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_10

merge_10:                                         ; preds = %else_11, %then_9
  br label %merge_7
}

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare ptr @fgets(ptr, i32, ptr)

declare ptr @__acrt_iob_func(i32)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
