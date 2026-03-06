; ModuleID = 'test_directives_sadc'
source_filename = "test_directives_sadc"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [16 x i8] c"\D8\AD\D8\AC\D9\85 \D8\B1\D9\82\D9\85: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [18 x i8] c"\D8\AD\D8\AC\D9\85 \D8\B9\D8\B4\D8\B1\D9\8A: \00", align 1
@str.const.8 = private unnamed_addr constant [14 x i8] c"\D8\AD\D8\AC\D9\85 \D9\86\D8\B5: \00", align 1
@str.const.13 = private unnamed_addr constant [19 x i8] c"\D8\A7\D9\84\D9\85\D8\AA\D8\B7\D8\A7\D9\8A\D8\B1: \00", align 1
@str.const.18 = private unnamed_addr constant [23 x i8] c"\D8\A8\D8\B9\D8\AF \D8\BA\D9\8A\D8\B1_\D8\A2\D9\85\D9\86: \00", align 1
@str.const.23 = private unnamed_addr constant [31 x i8] c"\D9\88\D9\82\D8\AA \D8\A7\D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9 \D9\8A\D8\B9\D9\85\D9\84\00", align 1
@str.const.27 = private unnamed_addr constant [50 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\AA\D9\88\D8\AC\D9\8A\D9\87\D8\A7\D8\AA \D9\85\D8\B9 sadc ===\00", align 1
@str.const.31 = private unnamed_addr constant [44 x i8] c"\E2\9C\85 \D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D9\86\D8\AC\D8\AD\D8\AA!\00", align 1
@str.const.35 = private unnamed_addr constant [41 x i8] c"\E2\9D\8C \D8\A8\D8\B9\D8\B6 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D9\81\D8\B4\D9\84\D8\AA\00", align 1

define i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\AD\D8\AC\D9\85"() {
entry:
  %"%\D8\AD\D8\AC\D9\85_\D9\86\D8\B5" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\B9\D8\B4\D8\B1\D9\8A" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\82\D9\85" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\82\D9\85.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\82\D9\85", align 8
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\82\D9\85.load")
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %1 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %2 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AD\D8\AC\D9\85_\D8\B9\D8\B4\D8\B1\D9\8A.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\B9\D8\B4\D8\B1\D9\8A", align 8
  %strbuf1 = alloca i8, i64 32, align 1
  %5 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf1, i64 32, ptr @int.fmt, i64 %"%\D8\AD\D8\AC\D9\85_\D8\B9\D8\B4\D8\B1\D9\8A.load")
  %len12 = call i64 @strlen(ptr @str.const.3)
  %len23 = call i64 @strlen(ptr %strbuf1)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = call ptr @malloc(i64 %bufsize5)
  %6 = call ptr @memcpy(ptr %concat.buf6, ptr @str.const.3, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %7 = call ptr @memcpy(ptr %dest27, ptr %strbuf1, i64 %copylen28)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf6)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AD\D8\AC\D9\85_\D9\86\D8\B5.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D9\86\D8\B5", align 8
  %strbuf9 = alloca i8, i64 32, align 1
  %10 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf9, i64 32, ptr @int.fmt, i64 %"%\D8\AD\D8\AC\D9\85_\D9\86\D8\B5.load")
  %len110 = call i64 @strlen(ptr @str.const.8)
  %len211 = call i64 @strlen(ptr %strbuf9)
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = call ptr @malloc(i64 %bufsize13)
  %11 = call ptr @memcpy(ptr %concat.buf14, ptr @str.const.8, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %12 = call ptr @memcpy(ptr %dest215, ptr %strbuf9, i64 %copylen216)
  %13 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\82\D9\85.load17" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\82\D9\85", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD\D8\AC\D9\85_\D8\B1\D9\82\D9\85.load17", 8
  ret i1 %cmpeqtmp
}

define i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D9\85\D8\AA\D8\B7\D8\A7\D9\8A\D8\B1"() {
entry:
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 0)
  %len1 = call i64 @strlen(ptr @str.const.13)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %1 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.13, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %2 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i1 true
}

define i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\BA\D9\8A\D8\B1_\D8\A2\D9\85\D9\86"() {
entry:
  %"%\D8\B3" = alloca i64, align 8
  store i64 10, ptr %"%\D8\B3", align 8
  %"%\D8\B3.load" = load i64, ptr %"%\D8\B3", align 8
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 %"%\D8\B3.load")
  %len1 = call i64 @strlen(ptr @str.const.18)
  %len2 = call i64 @strlen(ptr %strbuf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %1 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.18, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %2 = call ptr @memcpy(ptr %dest2, ptr %strbuf, i64 %copylen2)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B3.load1" = load i64, ptr %"%\D8\B3", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B3.load1", 20
  ret i1 %cmpeqtmp
}

define i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D9\88\D9\82\D8\AA_\D8\A7\D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.23)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i1 true
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %"%\D9\86\D8\AC\D8\A7\D8\AD4" = alloca i1, align 1
  %"%\D9\86\D8\AC\D8\A7\D8\AD3" = alloca i1, align 1
  %"%\D9\86\D8\AC\D8\A7\D8\AD2" = alloca i1, align 1
  %"%\D9\86\D8\AC\D8\A7\D8\AD1" = alloca i1, align 1
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.27)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %calltmp = call i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\AD\D8\AC\D9\85"()
  store i1 %calltmp, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD1", align 1
  %calltmp1 = call i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D9\85\D8\AA\D8\B7\D8\A7\D9\8A\D8\B1"()
  store i1 %calltmp1, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD2", align 1
  %calltmp2 = call i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\BA\D9\8A\D8\B1_\D8\A2\D9\85\D9\86"()
  store i1 %calltmp2, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD3", align 1
  %calltmp3 = call i1 @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D9\88\D9\82\D8\AA_\D8\A7\D9\84\D8\AA\D8\B1\D8\AC\D9\85\D8\A9"()
  store i1 %calltmp3, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD4", align 1
  %"%\D9\86\D8\AC\D8\A7\D8\AD1.load" = load i1, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD1", align 1
  %"%\D9\86\D8\AC\D8\A7\D8\AD2.load" = load i1, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD2", align 1
  %andtmp = and i1 %"%\D9\86\D8\AC\D8\A7\D8\AD1.load", %"%\D9\86\D8\AC\D8\A7\D8\AD2.load"
  %"%\D9\86\D8\AC\D8\A7\D8\AD3.load" = load i1, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD3", align 1
  %andtmp4 = and i1 %andtmp, %"%\D9\86\D8\AC\D8\A7\D8\AD3.load"
  %"%\D9\86\D8\AC\D8\A7\D8\AD4.load" = load i1, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD4", align 1
  %andtmp5 = and i1 %andtmp4, %"%\D9\86\D8\AC\D8\A7\D8\AD4.load"
  br i1 %andtmp5, label %then_0, label %else_2

else_2:                                           ; preds = %entry
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.35)
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1

merge_1:                                          ; preds = %else_2, %then_0
  ret i64 0

then_0:                                           ; preds = %entry
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.31)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  br label %merge_1
}

declare i32 @snprintf(ptr, i64, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
