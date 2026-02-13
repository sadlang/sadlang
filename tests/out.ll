; ModuleID = 'tests\test_all_outputs.sad'
source_filename = "tests\\test_all_outputs.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [45 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\B4\D8\A7\D9\85\D9\84 \D9\84\D9\84\D9\85\D8\AA\D8\B1\D8\AC\D9\85 ===\00", align 1
@2 = private unnamed_addr constant [25 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\8A\D8\A7 \D8\B9\D8\A7\D9\84\D9\85\00", align 1
@3 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@4 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@5 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@6 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@7 = private unnamed_addr constant [3 x i8] c"%f\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@9 = private unnamed_addr constant [23 x i8] c"=== \D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9 ===\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %alloca = alloca ptr, align 8
  store ptr @2, ptr %alloca, align 8
  %print.load = load ptr, ptr %alloca, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @3, ptr %print.load)
  %alloca1 = alloca i64, align 8
  store i64 100, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 50, ptr %alloca2, align 8
  %auto.load = load i64, ptr %alloca1, align 8
  %auto.load3 = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %auto.load, %auto.load3
  %2 = call i32 (ptr, ...) @printf(ptr @4, i64 %addtmp)
  %auto.load4 = load i64, ptr %alloca1, align 8
  %auto.load5 = load i64, ptr %alloca2, align 8
  %subtmp = sub i64 %auto.load4, %auto.load5
  %3 = call i32 (ptr, ...) @printf(ptr @5, i64 %subtmp)
  %auto.load6 = load i64, ptr %alloca1, align 8
  %auto.load7 = load i64, ptr %alloca2, align 8
  %multmp = mul i64 %auto.load6, %auto.load7
  %4 = call i32 (ptr, ...) @printf(ptr @6, i64 %multmp)
  %alloca8 = alloca double, align 8
  store double 3.140000e+00, ptr %alloca8, align 8
  %print.load9 = load double, ptr %alloca8, align 8
  %5 = call i32 (ptr, ...) @printf(ptr @7, double %print.load9)
  %6 = call i32 (ptr, ...) @printf(ptr @8, ptr @9)
  ret i64 0
}

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
