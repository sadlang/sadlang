; ModuleID = 'C:\s\s_language\working_exampels\11_compiler\test_strings.sad'
source_filename = "C:\\s\\s_language\\working_exampels\\11_compiler\\test_strings.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [18 x i8] c"\D8\B7\D9\88\D9\84 \D8\A7\D9\84\D9\86\D8\B5: \00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [16 x i8] c"\D9\86\D8\B5 \D9\83\D8\A8\D9\8A\D8\B1: \00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [16 x i8] c"\D9\86\D8\B5 \D8\B5\D8\BA\D9\8A\D8\B1: \00", align 1
@6 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@7 = private unnamed_addr constant [35 x i8] c"\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\AF\D9\88\D8\A7\D9\84 \D8\A7\D9\84\D9\86\D8\B5\D9\88\D8\B5\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@9 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca i64, align 8
  %alloca1 = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %alloca2 = alloca i64, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %alloca3 = alloca i64, align 8
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %3 = call i32 (ptr, ...) @printf(ptr @6, ptr @7)
  %4 = call i32 (ptr, ...) @printf(ptr @8, ptr @9)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
