; ModuleID = 'tests\test_compiler_output.sad'
source_filename = "tests\\test_compiler_output.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [32 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\85\D9\86 \D8\A7\D9\84\D9\85\D8\AA\D8\B1\D8\AC\D9\85!\00", align 1
@2 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@3 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@4 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, i64 42)
  %2 = call i32 (ptr, ...) @printf(ptr @3, double 3.140000e+00)
  %alloca = alloca i64, align 8
  store i64 10, ptr %alloca, align 8
  %alloca1 = alloca i64, align 8
  store i64 20, ptr %alloca1, align 8
  %auto.load = load i64, ptr %alloca, align 8
  %auto.load2 = load i64, ptr %alloca1, align 8
  %addtmp = add i64 %auto.load, %auto.load2
  %3 = call i32 (ptr, ...) @printf(ptr @4, i64 %addtmp)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
