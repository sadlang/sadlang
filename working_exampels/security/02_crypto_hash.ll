; ModuleID = '02_crypto_hash'
source_filename = "02_crypto_hash"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [49 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D9\88\D8\A7\D9\84\D9\87\D8\A7\D8\B4 ===\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [28 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\D9\8B \D8\A8\D8\A7\D9\84\D8\B9\D8\A7\D9\84\D9\85\00", align 1
@str.const.4 = private unnamed_addr constant [30 x i8] c"\D9\87\D8\A7\D8\B4 \D8\A7\D9\84\D9\86\D8\B5 \D8\A7\D9\84\D8\A3\D9\88\D9\84 = \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.8 = private unnamed_addr constant [35 x i8] c"\D9\83\D9\84\D9\85\D8\A9 \D8\A7\D9\84\D9\85\D8\B1\D9\88\D8\B1 \D8\A7\D9\84\D8\B3\D8\B1\D9\8A\D8\A9\00", align 1
@str.const.9 = private unnamed_addr constant [32 x i8] c"\D9\87\D8\A7\D8\B4 \D8\A7\D9\84\D9\86\D8\B5 \D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A = \00", align 1
@str.const.14 = private unnamed_addr constant [36 x i8] c"\D9\86\D9\81\D8\B3 \D8\A7\D9\84\D9\86\D8\B5 = \D9\86\D9\81\D8\B3 \D8\A7\D9\84\D9\87\D8\A7\D8\B4\00", align 1
@str.const.18 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.22 = private unnamed_addr constant [16 x i8] c"\D8\B3\D8\B1\D9\8A \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.23 = private unnamed_addr constant [14 x i8] c"\D9\85\D9\81\D8\AA\D8\A7\D8\AD123\00", align 1
@str.const.24 = private unnamed_addr constant [24 x i8] c"\D8\A7\D9\84\D9\86\D8\B5 \D8\A7\D9\84\D8\A3\D8\B5\D9\84\D9\8A: \00", align 1
@str.const.28 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D9\85\D8\B4\D9\81\D8\B1: \00", align 1
@str.const.32 = private unnamed_addr constant [29 x i8] c"\D8\A8\D8\B9\D8\AF \D9\81\D9\83 \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1: \00", align 1
@str.const.36 = private unnamed_addr constant [62 x i8] c"\D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D9\88\D9\81\D9\83 \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D9\8A\D8\B9\D9\85\D9\84\D8\A7\D9\86 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD!\00", align 1
@str.const.40 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.44 = private unnamed_addr constant [12 x i8] c"Hello World\00", align 1
@str.const.45 = private unnamed_addr constant [21 x i8] c"\D8\AA\D8\B1\D9\85\D9\8A\D8\B2 Base64 = \00", align 1
@str.const.49 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.53 = private unnamed_addr constant [51 x i8] c"\D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D8\B4\D9\81\D9\8A\D8\B1 \D9\86\D8\AC\D8\AD\D8\AA!\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca ptr, align 8
  store ptr @str.const.3, ptr %alloca, align 8
  %hash.ret = call i64 @sad_security_hash(ptr %alloca)
  %alloca1 = alloca i64, align 8
  store i64 %hash.ret, ptr %alloca1, align 8
  %"%\D9\87\D8\A7\D8\B41.load" = load i64, ptr %alloca1, align 8
  %strbuf = alloca i8, i64 32, align 1
  %2 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 %"%\D9\87\D8\A7\D8\B41.load")
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %3 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %4 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca2 = alloca ptr, align 8
  store ptr @str.const.8, ptr %alloca2, align 8
  %hash.ret3 = call i64 @sad_security_hash(ptr %alloca2)
  %alloca4 = alloca i64, align 8
  store i64 %hash.ret3, ptr %alloca4, align 8
  %"%\D9\87\D8\A7\D8\B42.load" = load i64, ptr %alloca4, align 8
  %strbuf5 = alloca i8, i64 32, align 1
  %7 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf5, i64 32, ptr @int.fmt, i64 %"%\D9\87\D8\A7\D8\B42.load")
  %len16 = call i64 @strlen(ptr @str.const.9)
  %len27 = call i64 @strlen(ptr %strbuf5)
  %totallen8 = add i64 %len16, %len27
  %bufsize9 = add i64 %totallen8, 1
  %concat.buf10 = alloca i8, i64 %bufsize9, align 1
  %8 = call ptr @memcpy(ptr %concat.buf10, ptr @str.const.9, i64 %len16)
  %dest211 = getelementptr i8, ptr %concat.buf10, i64 %len16
  %copylen212 = add i64 %len27, 1
  %9 = call ptr @memcpy(ptr %dest211, ptr %strbuf5, i64 %copylen212)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf10)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %hash.ret13 = call i64 @sad_security_hash(ptr %alloca)
  %alloca14 = alloca i64, align 8
  store i64 %hash.ret13, ptr %alloca14, align 8
  %"%\D9\87\D8\A7\D8\B41.load15" = load i64, ptr %alloca1, align 8
  %"%\D9\87\D8\A7\D8\B43.load" = load i64, ptr %alloca14, align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\87\D8\A7\D8\B41.load15", %"%\D9\87\D8\A7\D8\B43.load"
  %12 = icmp ne i1 %cmpeqtmp, false
  br i1 %12, label %sec.pass, label %sec.fail

sec.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

sec.pass:                                         ; preds = %entry
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.14)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca16 = alloca ptr, align 8
  store ptr @str.const.22, ptr %alloca16, align 8
  %alloca17 = alloca ptr, align 8
  store ptr @str.const.23, ptr %alloca17, align 8
  %encrypt.ret = call ptr @sad_security_encrypt(ptr %alloca16, ptr %alloca17)
  %alloca18 = alloca ptr, align 8
  store ptr %encrypt.ret, ptr %alloca18, align 8
  %len119 = call i64 @strlen(ptr @str.const.24)
  %len220 = call i64 @strlen(ptr %alloca16)
  %totallen21 = add i64 %len119, %len220
  %bufsize22 = add i64 %totallen21, 1
  %concat.buf23 = alloca i8, i64 %bufsize22, align 1
  %17 = call ptr @memcpy(ptr %concat.buf23, ptr @str.const.24, i64 %len119)
  %dest224 = getelementptr i8, ptr %concat.buf23, i64 %len119
  %copylen225 = add i64 %len220, 1
  %18 = call ptr @memcpy(ptr %dest224, ptr %alloca16, i64 %copylen225)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf23)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len126 = call i64 @strlen(ptr @str.const.28)
  %len227 = call i64 @strlen(ptr %alloca18)
  %totallen28 = add i64 %len126, %len227
  %bufsize29 = add i64 %totallen28, 1
  %concat.buf30 = alloca i8, i64 %bufsize29, align 1
  %21 = call ptr @memcpy(ptr %concat.buf30, ptr @str.const.28, i64 %len126)
  %dest231 = getelementptr i8, ptr %concat.buf30, i64 %len126
  %copylen232 = add i64 %len227, 1
  %22 = call ptr @memcpy(ptr %dest231, ptr %alloca18, i64 %copylen232)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf30)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %decrypt.ret = call ptr @sad_security_decrypt(ptr %alloca18, ptr %alloca17)
  %alloca33 = alloca ptr, align 8
  store ptr %decrypt.ret, ptr %alloca33, align 8
  %len134 = call i64 @strlen(ptr @str.const.32)
  %len235 = call i64 @strlen(ptr %alloca33)
  %totallen36 = add i64 %len134, %len235
  %bufsize37 = add i64 %totallen36, 1
  %concat.buf38 = alloca i8, i64 %bufsize37, align 1
  %25 = call ptr @memcpy(ptr %concat.buf38, ptr @str.const.32, i64 %len134)
  %dest239 = getelementptr i8, ptr %concat.buf38, i64 %len134
  %copylen240 = add i64 %len235, 1
  %26 = call ptr @memcpy(ptr %dest239, ptr %alloca33, i64 %copylen240)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf38)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %strcmp.ret = call i32 @strcmp(ptr %alloca33, ptr %alloca16)
  %streq = icmp eq i32 %strcmp.ret, 0
  %29 = icmp ne i1 %streq, false
  br i1 %29, label %sec.pass42, label %sec.fail41

sec.fail41:                                       ; preds = %sec.pass
  call void @abort()
  unreachable

sec.pass42:                                       ; preds = %sec.pass
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.36)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %32 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.40)
  %33 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca43 = alloca ptr, align 8
  store ptr @str.const.44, ptr %alloca43, align 8
  %base64.ret = call ptr @sad_security_base64_encode(ptr %alloca43)
  %alloca44 = alloca ptr, align 8
  store ptr %base64.ret, ptr %alloca44, align 8
  %len145 = call i64 @strlen(ptr @str.const.45)
  %len246 = call i64 @strlen(ptr %alloca44)
  %totallen47 = add i64 %len145, %len246
  %bufsize48 = add i64 %totallen47, 1
  %concat.buf49 = alloca i8, i64 %bufsize48, align 1
  %34 = call ptr @memcpy(ptr %concat.buf49, ptr @str.const.45, i64 %len145)
  %dest250 = getelementptr i8, ptr %concat.buf49, i64 %len145
  %copylen251 = add i64 %len246, 1
  %35 = call ptr @memcpy(ptr %dest250, ptr %alloca44, i64 %copylen251)
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf49)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.49)
  %39 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %40 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.53)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare i64 @sad_security_hash(ptr)

declare i32 @snprintf(ptr, i64, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare void @abort()

declare ptr @sad_security_encrypt(ptr, ptr)

declare ptr @sad_security_decrypt(ptr, ptr)

declare i32 @strcmp(ptr, ptr)

declare ptr @sad_security_base64_encode(ptr)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
