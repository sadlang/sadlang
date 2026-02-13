; ModuleID = 'test_less5.sad'
source_filename = "test_less5.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [9 x i8] c"\D8\B5\D8\BA\D9\8A\D8\B1\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca i64, align 8
  store i64 1, ptr %alloca, align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %merge_4, %entry
  %auto.load = load i64, ptr %alloca, align 8
  %cmpgttmp = icmp sgt i64 %auto.load, 0
  br i1 %cmpgttmp, label %while_body_1, label %while_exit_2

while_body_1:                                     ; preds = %while_cond_0
  %alloca1 = alloca i64, align 8
  store i64 1, ptr %alloca1, align 8
  %auto.load2 = load i64, ptr %alloca1, align 8
  %cmpeqtmp = icmp eq i64 %auto.load2, 0
  br i1 %cmpeqtmp, label %then_3, label %else_5

while_exit_2:                                     ; preds = %while_cond_0
  ret i64 0

then_3:                                           ; preds = %while_body_1
  store i64 0, ptr %alloca, align 8
  br label %merge_4

else_5:                                           ; preds = %while_body_1
  %auto.load3 = load i64, ptr %alloca1, align 8
  %cmpeqtmp4 = icmp eq i64 %auto.load3, 1
  br i1 %cmpeqtmp4, label %then_6, label %merge_7

merge_4:                                          ; preds = %merge_7, %then_3
  store i64 0, ptr %alloca, align 8
  br label %while_cond_0

then_6:                                           ; preds = %else_5
  %alloca5 = alloca i64, align 8
  store i64 10, ptr %alloca5, align 8
  %auto.load6 = load i64, ptr %alloca5, align 8
  %cmplttmp = icmp slt i64 %auto.load6, 5
  br i1 %cmplttmp, label %then_8, label %merge_9

merge_7:                                          ; preds = %merge_9, %else_5
  br label %merge_4

then_8:                                           ; preds = %then_6
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  br label %merge_9

merge_9:                                          ; preds = %then_8, %then_6
  br label %merge_7
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
