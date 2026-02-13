; ModuleID = 'test_less8.sad'
source_filename = "test_less8.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [7 x i8] c"\D8\B5\D9\81\D8\B1\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [9 x i8] c"\D9\83\D8\A8\D9\8A\D8\B1\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@9 = private unnamed_addr constant [9 x i8] c"\D8\B5\D8\BA\D9\8A\D8\B1\00", align 1
@10 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@11 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  %auto.load = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %auto.load, 0
  br i1 %cmpeqtmp, label %then_0, label %else_2

then_0:                                           ; preds = %entry
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  br label %merge_1

else_2:                                           ; preds = %entry
  %auto.load1 = load i64, ptr %alloca, align 8
  %cmpeqtmp2 = icmp eq i64 %auto.load1, 1
  br i1 %cmpeqtmp2, label %then_3, label %merge_4

merge_1:                                          ; preds = %merge_4, %then_0
  ret i64 0

then_3:                                           ; preds = %else_2
  %alloca3 = alloca i64, align 8
  store i64 10, ptr %alloca3, align 8
  %auto.load4 = load i64, ptr %alloca3, align 8
  %cmpgttmp = icmp sgt i64 %auto.load4, 100
  br i1 %cmpgttmp, label %then_5, label %merge_6

merge_4:                                          ; preds = %merge_8, %else_2
  br label %merge_1

then_5:                                           ; preds = %then_3
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %3 = call i32 (ptr, ...) @printf(ptr @6, ptr @7)
  br label %merge_6

merge_6:                                          ; preds = %then_5, %then_3
  %auto.load5 = load i64, ptr %alloca3, align 8
  %cmplttmp = icmp slt i64 %auto.load5, 5
  br i1 %cmplttmp, label %then_7, label %merge_8

then_7:                                           ; preds = %merge_6
  %4 = call i32 (ptr, ...) @printf(ptr @8, ptr @9)
  %5 = call i32 (ptr, ...) @printf(ptr @10, ptr @11)
  br label %merge_8

merge_8:                                          ; preds = %then_7, %merge_6
  br label %merge_4
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
