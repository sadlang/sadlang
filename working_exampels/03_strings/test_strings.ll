; ModuleID = 'C:\s\s_language\working_exampels\03_strings\test_strings.sad'
source_filename = "C:\\s\\s_language\\working_exampels\\03_strings\\test_strings.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [19 x i8] c"\D8\B7\D9\88\D9\84 \D8\A7\D9\84\D9\86\D8\B5 = \00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [23 x i8] c"\D8\AA\D8\AD\D9\88\D9\8A\D9\84 \D9\83\D8\A8\D9\8A\D8\B1 = \00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@9 = private unnamed_addr constant [23 x i8] c"\D8\AA\D8\AD\D9\88\D9\8A\D9\84 \D8\B5\D8\BA\D9\8A\D8\B1 = \00", align 1
@10 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@11 = private unnamed_addr constant [41 x i8] c"\D8\AA\D9\85 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5!\00", align 1
@12 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@13 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %3 = call i32 (ptr, ...) @printf(ptr @6, ptr @7)
  %4 = call i32 (ptr, ...) @printf(ptr @8, ptr @9)
  %5 = call i32 (ptr, ...) @printf(ptr @10, ptr @11)
  %6 = call i32 (ptr, ...) @printf(ptr @12, ptr @13)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
