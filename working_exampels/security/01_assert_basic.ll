; ModuleID = '01_assert_basic'
source_filename = "01_assert_basic"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [57 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\AA\D8\A3\D9\83\D9\8A\D8\AF\D8\A7\D8\AA \D8\A7\D9\84\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A\D8\A9 ===\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [42 x i8] c"\D8\AA\D8\A3\D9\83\D8\AF \D8\A3\D9\86 \D8\B3 \D8\AA\D8\B3\D8\A7\D9\88\D9\8A 10 \E2\80\94 \D9\86\D8\AC\D8\AD\00", align 1
@str.const.7 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.9 = private unnamed_addr constant [45 x i8] c"\D8\AA\D8\A3\D9\83\D8\AF \D8\A3\D9\86 \D8\A7\D9\84\D8\A7\D8\B3\D9\85 \D8\B5\D8\AD\D9\8A\D8\AD \E2\80\94 \D9\86\D8\AC\D8\AD\00", align 1
@str.const.13 = private unnamed_addr constant [19 x i8] c"\D8\AA\D8\AD\D9\82\D9\82(5 > 3) = \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.17 = private unnamed_addr constant [21 x i8] c"\D8\AA\D8\AD\D9\82\D9\82(1 > 100) = \00", align 1
@str.const.22 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\D9\8B\00", align 1
@str.const.23 = private unnamed_addr constant [23 x i8] c"\D8\A2\D9\85\D9\86 \D9\85\D8\B1\D8\AD\D8\A8\D8\A7\D9\8B = \00", align 1
@str.const.28 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.32 = private unnamed_addr constant [57 x i8] c"\D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A\D8\A9 \D9\86\D8\AC\D8\AD\D8\AA!\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca i64, align 8
  store i64 10, ptr %alloca, align 8
  %"%\D8\B3.load" = load i64, ptr %alloca, align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B3.load", 10
  %2 = icmp ne i1 %cmpeqtmp, false
  br i1 %2, label %sec.pass, label %sec.fail

sec.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

sec.pass:                                         ; preds = %entry
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca1 = alloca ptr, align 8
  store ptr @str.const.7, ptr %alloca1, align 8
  %strcmp.ret = call i32 @strcmp(ptr %alloca1, ptr @str.const.7)
  %streq = icmp eq i32 %strcmp.ret, 0
  %5 = icmp ne i1 %streq, false
  br i1 %5, label %sec.pass3, label %sec.fail2

sec.fail2:                                        ; preds = %sec.pass
  call void @abort()
  unreachable

sec.pass3:                                        ; preds = %sec.pass
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca4 = alloca i1, align 1
  store i64 1, ptr %alloca4, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load" = load i1, ptr %alloca4, align 1
  %strbuf = alloca i8, i64 32, align 1
  %8 = sext i1 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load" to i64
  %9 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 %8)
  %len1 = call i64 @strlen(ptr @str.const.13)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %10 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.13, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %11 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca5 = alloca i1, align 1
  store i64 0, ptr %alloca5, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load" = load i1, ptr %alloca5, align 1
  %strbuf6 = alloca i8, i64 32, align 1
  %14 = sext i1 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load" to i64
  %15 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf6, i64 32, ptr @int.fmt, i64 %14)
  %len17 = call i64 @strlen(ptr @str.const.17)
  %len28 = call i64 @strlen(ptr %strbuf6)
  %totallen9 = add i64 %len17, %len28
  %bufsize10 = add i64 %totallen9, 1
  %concat.buf11 = alloca i8, i64 %bufsize10, align 1
  %16 = call ptr @memcpy(ptr %concat.buf11, ptr @str.const.17, i64 %len17)
  %dest212 = getelementptr i8, ptr %concat.buf11, i64 %len17
  %copylen213 = add i64 %len28, 1
  %17 = call ptr @memcpy(ptr %dest212, ptr %strbuf6, i64 %copylen213)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf11)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca14 = alloca ptr, align 8
  store ptr @str.const.22, ptr %alloca14, align 8
  %20 = icmp ne ptr %alloca14, null
  %21 = zext i1 %20 to i64
  %alloca15 = alloca i1, align 1
  store i64 %21, ptr %alloca15, align 8
  %"%\D8\A2\D9\85\D9\86\D8\A9.load" = load i1, ptr %alloca15, align 1
  %strbuf16 = alloca i8, i64 32, align 1
  %22 = sext i1 %"%\D8\A2\D9\85\D9\86\D8\A9.load" to i64
  %23 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf16, i64 32, ptr @int.fmt, i64 %22)
  %len117 = call i64 @strlen(ptr @str.const.23)
  %len218 = call i64 @strlen(ptr %strbuf16)
  %totallen19 = add i64 %len117, %len218
  %bufsize20 = add i64 %totallen19, 1
  %concat.buf21 = alloca i8, i64 %bufsize20, align 1
  %24 = call ptr @memcpy(ptr %concat.buf21, ptr @str.const.23, i64 %len117)
  %dest222 = getelementptr i8, ptr %concat.buf21, i64 %len117
  %copylen223 = add i64 %len218, 1
  %25 = call ptr @memcpy(ptr %dest222, ptr %strbuf16, i64 %copylen223)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf21)
  %27 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.28)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.32)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare void @abort()

declare i32 @strcmp(ptr, ptr)

declare i32 @snprintf(ptr, i64, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
