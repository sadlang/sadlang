; ModuleID = 'test_security_compiler.sad'
source_filename = "test_security_compiler.sad"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@0 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@1 = private unnamed_addr constant [54 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86 \D9\81\D9\8A \D8\A7\D9\84\D9\85\D8\AA\D8\B1\D8\AC\D9\85 ===\00", align 1
@2 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@3 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@4 = private unnamed_addr constant [41 x i8] c"Assertion failed / \D9\81\D8\B4\D9\84 \D8\A7\D9\84\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\00", align 1
@5 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@6 = private unnamed_addr constant [30 x i8] c"\D8\AA\D8\A3\D9\83\D8\AF(\D8\B3 == 10) \E2\80\94 \D9\86\D8\AC\D8\AD\00", align 1
@7 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@8 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@9 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@10 = private unnamed_addr constant [23 x i8] c"\D8\AA\D8\AD\D9\82\D9\82(5 > 3) \D9\86\D8\AC\D8\AD\00", align 1
@11 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@12 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@13 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@14 = private unnamed_addr constant [25 x i8] c"\D8\AA\D8\AD\D9\82\D9\82(1 > 100) \D9\86\D8\AC\D8\AD\00", align 1
@15 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@16 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@17 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@18 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@19 = private unnamed_addr constant [25 x i8] c"\D9\87\D8\A7\D8\B4 \D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\86\D8\AC\D8\AD\00", align 1
@20 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@21 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@22 = private unnamed_addr constant [7 x i8] c"\D8\B3\D8\B1\D9\8A\00", align 1
@23 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\81\D8\AA\D8\A7\D8\AD\00", align 1
@24 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@25 = private unnamed_addr constant [18 x i8] c"\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D9\86\D8\AC\D8\AD\00", align 1
@26 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@27 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@28 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\81\D8\AA\D8\A7\D8\AD\00", align 1
@29 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@30 = private unnamed_addr constant [23 x i8] c"\D9\81\D9\83 \D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D9\86\D8\AC\D8\AD\00", align 1
@31 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@32 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@33 = private unnamed_addr constant [26 x i8] c"<script>\D8\AE\D8\B7\D9\8A\D8\B1</script>\00", align 1
@34 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@35 = private unnamed_addr constant [18 x i8] c"\D8\AA\D9\86\D8\B8\D9\8A\D9\81 \D9\86\D8\AC\D8\AD\00", align 1
@36 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@37 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@38 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@39 = private unnamed_addr constant [23 x i8] c"\D9\88\D9\82\D8\AA \D8\A7\D9\84\D8\A2\D9\86 \D9\86\D8\AC\D8\AD\00", align 1
@40 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@41 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@42 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@43 = private unnamed_addr constant [27 x i8] c"\D8\B9\D8\B4\D9\88\D8\A7\D8\A6\D9\8A \D8\A2\D9\85\D9\86 \D9\86\D8\AC\D8\AD\00", align 1
@44 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@45 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@46 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@47 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@48 = private unnamed_addr constant [21 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2 64 \D9\86\D8\AC\D8\AD\00", align 1
@49 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@50 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@51 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@52 = private unnamed_addr constant [57 x i8] c"=== \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\A3\D9\85\D8\A7\D9\86 \D9\86\D8\AC\D8\AD\D8\AA! ===\00", align 1
@53 = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@54 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, ptr @1)
  %1 = call i32 (ptr, ...) @printf(ptr @2, ptr @3)
  %alloca = alloca i64, align 8
  store i64 10, ptr %alloca, align 8
  %auto.load = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %auto.load, 10
  call void @sad_security_assert(i1 %cmpeqtmp, ptr @4)
  %2 = call i32 (ptr, ...) @printf(ptr @5, ptr @6)
  %3 = call i32 (ptr, ...) @printf(ptr @7, ptr @8)
  %alloca1 = alloca i1, align 1
  store i64 1, ptr %alloca1, align 8
  %4 = call i32 (ptr, ...) @printf(ptr @9, ptr @10)
  %5 = call i32 (ptr, ...) @printf(ptr @11, ptr @12)
  %alloca2 = alloca i1, align 1
  store i64 0, ptr %alloca2, align 8
  %6 = call i32 (ptr, ...) @printf(ptr @13, ptr @14)
  %7 = call i32 (ptr, ...) @printf(ptr @15, ptr @16)
  %hash.result = call i64 @sad_security_hash(ptr @17)
  %alloca3 = alloca i64, align 8
  store i64 %hash.result, ptr %alloca3, align 8
  %8 = call i32 (ptr, ...) @printf(ptr @18, ptr @19)
  %9 = call i32 (ptr, ...) @printf(ptr @20, ptr @21)
  %encrypt.result = call ptr @sad_security_encrypt(ptr @22, ptr @23)
  %alloca4 = alloca ptr, align 8
  store ptr %encrypt.result, ptr %alloca4, align 8
  %10 = call i32 (ptr, ...) @printf(ptr @24, ptr @25)
  %11 = call i32 (ptr, ...) @printf(ptr @26, ptr @27)
  %auto.load5 = load ptr, ptr %alloca4, align 8
  %decrypt.result = call ptr @sad_security_decrypt(ptr %auto.load5, ptr @28)
  %alloca6 = alloca ptr, align 8
  store ptr %decrypt.result, ptr %alloca6, align 8
  %12 = call i32 (ptr, ...) @printf(ptr @29, ptr @30)
  %13 = call i32 (ptr, ...) @printf(ptr @31, ptr @32)
  %sanitize.result = call ptr @sad_security_sanitize(ptr @33)
  %alloca7 = alloca ptr, align 8
  store ptr %sanitize.result, ptr %alloca7, align 8
  %14 = call i32 (ptr, ...) @printf(ptr @34, ptr @35)
  %15 = call i32 (ptr, ...) @printf(ptr @36, ptr @37)
  %timestamp.result = call i64 @sad_security_timestamp()
  %alloca8 = alloca i64, align 8
  store i64 %timestamp.result, ptr %alloca8, align 8
  %16 = call i32 (ptr, ...) @printf(ptr @38, ptr @39)
  %17 = call i32 (ptr, ...) @printf(ptr @40, ptr @41)
  %secure_random.result = call i64 @sad_security_secure_random(i64 1, i64 100)
  %alloca9 = alloca i64, align 8
  store i64 %secure_random.result, ptr %alloca9, align 8
  %18 = call i32 (ptr, ...) @printf(ptr @42, ptr @43)
  %19 = call i32 (ptr, ...) @printf(ptr @44, ptr @45)
  %base64.result = call ptr @sad_security_base64_encode(ptr @46)
  %alloca10 = alloca ptr, align 8
  store ptr %base64.result, ptr %alloca10, align 8
  %20 = call i32 (ptr, ...) @printf(ptr @47, ptr @48)
  %21 = call i32 (ptr, ...) @printf(ptr @49, ptr @50)
  %22 = call i32 (ptr, ...) @printf(ptr @51, ptr @52)
  %23 = call i32 (ptr, ...) @printf(ptr @53, ptr @54)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare void @sad_security_assert(i1, ptr)

declare i64 @sad_security_hash(ptr)

declare ptr @sad_security_encrypt(ptr, ptr)

declare ptr @sad_security_decrypt(ptr, ptr)

declare ptr @sad_security_sanitize(ptr)

declare i64 @sad_security_timestamp()

declare i64 @sad_security_secure_random(i64, i64)

declare ptr @sad_security_base64_encode(ptr)

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
