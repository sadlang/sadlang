; ModuleID = 'tests\ownership\test_01_basic.sad'
source_filename = "tests\\ownership\\test_01_basic.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [27 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\85\D9\86 \D9\84\D8\BA\D8\A9 \D8\B5!\00", align 1
@1 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@5 = private unnamed_addr constant [17 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1
@6 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@7 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@8 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@9 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@10 = private unnamed_addr constant [44 x i8] c"\E2\9C\85 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A \D9\86\D8\AC\D8\AD!\00", align 1
@11 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@12 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %alloca = alloca ptr, align 8
  store ptr @0, ptr %alloca, align 8
  %print.load = load ptr, ptr %alloca, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @1, ptr %print.load)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %alloca1 = alloca i64, align 8
  store i64 42, ptr %alloca1, align 8
  %alloca2 = alloca i64, align 8
  store i64 8, ptr %alloca2, align 8
  %auto.load = load i64, ptr %alloca1, align 8
  %auto.load3 = load i64, ptr %alloca2, align 8
  %addtmp = add i64 %auto.load, %auto.load3
  %alloca4 = alloca i64, align 8
  store i64 %addtmp, ptr %alloca4, align 8
  %2 = call i32 (ptr, ...) @printf(ptr @4, ptr @5)
  %print.load5 = load i64, ptr %alloca4, align 8
  %3 = call i32 (ptr, ...) @printf(ptr @6, i64 %print.load5)
  %4 = call i32 (ptr, ...) @printf(ptr @7, ptr @8)
  %5 = call i32 (ptr, ...) @printf(ptr @9, ptr @10)
  %6 = call i32 (ptr, ...) @printf(ptr @11, ptr @12)
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
