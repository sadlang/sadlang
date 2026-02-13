; ModuleID = 'test_less.sad'
source_filename = "test_less.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A7\D9\84\D8\A8\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca i64, align 8
  store i64 5, ptr %alloca, align 8
  %auto.load = load i64, ptr %alloca, align 8
  %cmplttmp = icmp slt i64 %auto.load, 0
  br i1 %cmplttmp, label %then_0, label %merge_1

then_0:                                           ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  br label %merge_1

merge_1:                                          ; preds = %then_0, %entry
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i32 @SetConsoleOutputCP(i32 65001)
  %1 = call i32 @SetConsoleCP(i32 65001)
  %2 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %3 = trunc i64 %2 to i32
  ret i32 %3
}

declare i32 @SetConsoleOutputCP(i32)

declare i32 @SetConsoleCP(i32)
