; ModuleID = 'test_ffi_compiler.sad'
source_filename = "test_ffi_compiler.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [46 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 FFI \D9\81\D9\8A \D8\A7\D9\84\D9\85\D8\AA\D8\B1\D8\AC\D9\85 ===\0A\00", align 1
@1 = private unnamed_addr constant [12 x i8] c"\D8\B1\D9\82\D9\85: %d\0A\00", align 1
@2 = private unnamed_addr constant [10 x i8] c"\D9\86\D8\B5: %s\0A\00", align 1
@3 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@4 = private unnamed_addr constant [23 x i8] c"\D8\AD\D8\AC\D8\B2 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9: %p\0A\00", align 1
@5 = private unnamed_addr constant [31 x i8] c"\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1 \D8\B0\D8\A7\D9\83\D8\B1\D8\A9: \D9\86\D8\AC\D8\AD\0A\00", align 1
@6 = private unnamed_addr constant [12 x i8] c"Hello World\00", align 1
@7 = private unnamed_addr constant [21 x i8] c"\D8\B7\D9\88\D9\84 \D8\A7\D9\84\D9\86\D8\B5: %d\0A\00", align 1
@8 = private unnamed_addr constant [4 x i8] c"abc\00", align 1
@9 = private unnamed_addr constant [4 x i8] c"abc\00", align 1
@10 = private unnamed_addr constant [33 x i8] c"\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9 \D9\85\D8\AA\D8\B7\D8\A7\D8\A8\D9\82\D8\A9: %d\0A\00", align 1
@11 = private unnamed_addr constant [6 x i8] c"12345\00", align 1
@12 = private unnamed_addr constant [30 x i8] c"\D8\AA\D8\AD\D9\88\D9\8A\D9\84 \D9\86\D8\B5 \D9\84\D8\B9\D8\AF\D8\AF: %d\0A\00", align 1
@13 = private unnamed_addr constant [5 x i8] c"PATH\00", align 1
@14 = private unnamed_addr constant [41 x i8] c"\D9\85\D8\AA\D8\BA\D9\8A\D8\B1 \D8\A7\D9\84\D8\A8\D9\8A\D8\A6\D8\A9 PATH \D9\85\D9\88\D8\AC\D9\88\D8\AF\0A\00", align 1
@15 = private unnamed_addr constant [32 x i8] c"echo \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 system \D9\86\D8\AC\D8\AD\00", align 1
@16 = private unnamed_addr constant [49 x i8] c"=== \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA FFI \D9\86\D8\AC\D8\AD\D8\AA! ===\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %printf.result = call i32 (ptr, ...) @printf(ptr @0)
  %printf.ext = sext i32 %printf.result to i64
  %printf.result1 = call i32 (ptr, ...) @printf(ptr @1, i64 42)
  %printf.ext2 = sext i32 %printf.result1 to i64
  %printf.result3 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %printf.ext4 = sext i32 %printf.result3 to i64
  %malloc.result = call ptr @malloc(i64 256)
  %alloca = alloca ptr, align 8
  store ptr %malloc.result, ptr %alloca, align 8
  %auto.load = load ptr, ptr %alloca, align 8
  %printf.result5 = call i32 (ptr, ...) @printf(ptr @4, ptr %auto.load)
  %printf.ext6 = sext i32 %printf.result5 to i64
  %auto.load7 = load ptr, ptr %alloca, align 8
  call void @free(ptr %auto.load7)
  %printf.result8 = call i32 (ptr, ...) @printf(ptr @5)
  %printf.ext9 = sext i32 %printf.result8 to i64
  %strlen.result = call i64 @strlen(ptr @6)
  %alloca10 = alloca i64, align 8
  store i64 %strlen.result, ptr %alloca10, align 8
  %auto.load11 = load i64, ptr %alloca10, align 8
  %printf.result12 = call i32 (ptr, ...) @printf(ptr @7, i64 %auto.load11)
  %printf.ext13 = sext i32 %printf.result12 to i64
  %strcmp.result = call i32 @strcmp(ptr @8, ptr @9)
  %strcmp.ext = sext i32 %strcmp.result to i64
  %alloca14 = alloca i64, align 8
  store i64 %strcmp.ext, ptr %alloca14, align 8
  %auto.load15 = load i64, ptr %alloca14, align 8
  %printf.result16 = call i32 (ptr, ...) @printf(ptr @10, i64 %auto.load15)
  %printf.ext17 = sext i32 %printf.result16 to i64
  %atoi.result = call i32 @atoi(ptr @11)
  %atoi.ext = sext i32 %atoi.result to i64
  %alloca18 = alloca i64, align 8
  store i64 %atoi.ext, ptr %alloca18, align 8
  %auto.load19 = load i64, ptr %alloca18, align 8
  %printf.result20 = call i32 (ptr, ...) @printf(ptr @12, i64 %auto.load19)
  %printf.ext21 = sext i32 %printf.result20 to i64
  %getenv.result = call ptr @getenv(ptr @13)
  %alloca22 = alloca ptr, align 8
  store ptr %getenv.result, ptr %alloca22, align 8
  %printf.result23 = call i32 (ptr, ...) @printf(ptr @14)
  %printf.ext24 = sext i32 %printf.result23 to i64
  %system.result = call i32 @system(ptr @15)
  %system.ext = sext i32 %system.result to i64
  %printf.result25 = call i32 (ptr, ...) @printf(ptr @16)
  %printf.ext26 = sext i32 %printf.result25 to i64
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare void @free(ptr)

declare i64 @strlen(ptr)

declare i32 @strcmp(ptr, ptr)

declare i32 @atoi(ptr)

declare ptr @getenv(ptr)

declare i32 @system(ptr)

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
