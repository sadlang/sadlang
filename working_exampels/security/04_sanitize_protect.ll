; ModuleID = '04_sanitize_protect'
source_filename = "04_sanitize_protect"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [53 x i8] c"=== \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1 \D8\A7\D9\84\D8\AA\D9\86\D8\B8\D9\8A\D9\81 \D9\88\D8\A7\D9\84\D8\AD\D9\85\D8\A7\D9\8A\D8\A9 ===\00", align 1
@fmt.s = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.3 = private unnamed_addr constant [29 x i8] c"<script>alert(hack)</script>\00", align 1
@str.const.4 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84: \00", align 1
@str.const.8 = private unnamed_addr constant [24 x i8] c"\D8\A8\D8\B9\D8\AF \D8\A7\D9\84\D8\AA\D9\86\D8\B8\D9\8A\D9\81: \00", align 1
@str.const.12 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.16 = private unnamed_addr constant [34 x i8] c"\D9\86\D8\B5 \D8\B9\D8\A7\D8\AF\D9\8A \D8\A8\D8\AF\D9\88\D9\86 \D9\85\D8\B4\D8\A7\D9\83\D9\84\00", align 1
@str.const.25 = private unnamed_addr constant [38 x i8] c"\D8\A7\D9\84\D9\86\D8\B5 \D8\A7\D9\84\D8\B9\D8\A7\D8\AF\D9\8A \D9\84\D8\A7 \D9\8A\D8\AA\D8\BA\D9\8A\D8\B1\00", align 1
@str.const.29 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.33 = private unnamed_addr constant [41 x i8] c"\D8\A7\D9\84\D8\B7\D8\A7\D8\A8\D8\B9 \D8\A7\D9\84\D8\B2\D9\85\D9\86\D9\8A \D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.37 = private unnamed_addr constant [35 x i8] c"\D8\A7\D9\84\D8\B7\D8\A7\D8\A8\D8\B9 \D8\A7\D9\84\D8\B2\D9\85\D9\86\D9\8A \D9\85\D9\88\D8\AC\D8\A8\00", align 1
@str.const.41 = private unnamed_addr constant [24 x i8] c"\D8\B9\D8\B4\D9\88\D8\A7\D8\A6\D9\8A \D8\A2\D9\85\D9\86 1: \00", align 1
@str.const.46 = private unnamed_addr constant [24 x i8] c"\D8\B9\D8\B4\D9\88\D8\A7\D8\A6\D9\8A \D8\A2\D9\85\D9\86 2: \00", align 1
@str.const.51 = private unnamed_addr constant [65 x i8] c"\D8\A7\D9\84\D8\A3\D8\B1\D9\82\D8\A7\D9\85 \D8\A7\D9\84\D8\B9\D8\B4\D9\88\D8\A7\D8\A6\D9\8A\D8\A9 \D8\AA\D9\85 \D8\AA\D9\88\D9\84\D9\8A\D8\AF\D9\87\D8\A7 \D8\A8\D9\86\D8\AC\D8\A7\D8\AD\00", align 1
@str.const.55 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@str.const.59 = private unnamed_addr constant [68 x i8] c"\D8\AC\D9\85\D9\8A\D8\B9 \D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D8\A7\D9\84\D8\AA\D9\86\D8\B8\D9\8A\D9\81 \D9\88\D8\A7\D9\84\D8\AD\D9\85\D8\A7\D9\8A\D8\A9 \D9\86\D8\AC\D8\AD\D8\AA!\00", align 1

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca = alloca ptr, align 8
  store ptr @str.const.3, ptr %alloca, align 8
  %sanitize.ret = call ptr @sad_security_sanitize(ptr %alloca)
  %alloca1 = alloca ptr, align 8
  store ptr %sanitize.ret, ptr %alloca1, align 8
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %alloca)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = alloca i8, i64 %bufsize, align 1
  %2 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %3 = call ptr @memcpy(ptr %dest2, ptr %alloca, i64 %copylen2)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len12 = call i64 @strlen(ptr @str.const.8)
  %len23 = call i64 @strlen(ptr %alloca1)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = alloca i8, i64 %bufsize5, align 1
  %6 = call ptr @memcpy(ptr %concat.buf6, ptr @str.const.8, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %7 = call ptr @memcpy(ptr %dest27, ptr %alloca1, i64 %copylen28)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf6)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %10 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.12)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %alloca9 = alloca ptr, align 8
  store ptr @str.const.16, ptr %alloca9, align 8
  %sanitize.ret10 = call ptr @sad_security_sanitize(ptr %alloca9)
  %alloca11 = alloca ptr, align 8
  store ptr %sanitize.ret10, ptr %alloca11, align 8
  %len112 = call i64 @strlen(ptr @str.const.4)
  %len213 = call i64 @strlen(ptr %alloca9)
  %totallen14 = add i64 %len112, %len213
  %bufsize15 = add i64 %totallen14, 1
  %concat.buf16 = alloca i8, i64 %bufsize15, align 1
  %12 = call ptr @memcpy(ptr %concat.buf16, ptr @str.const.4, i64 %len112)
  %dest217 = getelementptr i8, ptr %concat.buf16, i64 %len112
  %copylen218 = add i64 %len213, 1
  %13 = call ptr @memcpy(ptr %dest217, ptr %alloca9, i64 %copylen218)
  %14 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf16)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %len119 = call i64 @strlen(ptr @str.const.8)
  %len220 = call i64 @strlen(ptr %alloca11)
  %totallen21 = add i64 %len119, %len220
  %bufsize22 = add i64 %totallen21, 1
  %concat.buf23 = alloca i8, i64 %bufsize22, align 1
  %16 = call ptr @memcpy(ptr %concat.buf23, ptr @str.const.8, i64 %len119)
  %dest224 = getelementptr i8, ptr %concat.buf23, i64 %len119
  %copylen225 = add i64 %len220, 1
  %17 = call ptr @memcpy(ptr %dest224, ptr %alloca11, i64 %copylen225)
  %18 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf23)
  %19 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %strcmp.ret = call i32 @strcmp(ptr %alloca11, ptr %alloca9)
  %streq = icmp eq i32 %strcmp.ret, 0
  %20 = icmp ne i1 %streq, false
  br i1 %20, label %sec.pass, label %sec.fail

sec.fail:                                         ; preds = %entry
  call void @abort()
  unreachable

sec.pass:                                         ; preds = %entry
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.25)
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %23 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.29)
  %24 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %time.ret = call i64 @time(ptr null)
  %alloca26 = alloca i64, align 8
  store i64 %time.ret, ptr %alloca26, align 8
  %"%\D8\A7\D9\84\D9\88\D9\82\D8\AA.load" = load i64, ptr %alloca26, align 8
  %strbuf = alloca i8, i64 32, align 1
  %25 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 %"%\D8\A7\D9\84\D9\88\D9\82\D8\AA.load")
  %len127 = call i64 @strlen(ptr @str.const.33)
  %len228 = call i64 @strlen(ptr %strbuf)
  %totallen29 = add i64 %len127, %len228
  %bufsize30 = add i64 %totallen29, 1
  %concat.buf31 = alloca i8, i64 %bufsize30, align 1
  %26 = call ptr @memcpy(ptr %concat.buf31, ptr @str.const.33, i64 %len127)
  %dest232 = getelementptr i8, ptr %concat.buf31, i64 %len127
  %copylen233 = add i64 %len228, 1
  %27 = call ptr @memcpy(ptr %dest232, ptr %strbuf, i64 %copylen233)
  %28 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf31)
  %29 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\A7\D9\84\D9\88\D9\82\D8\AA.load34" = load i64, ptr %alloca26, align 8
  %assert.gt = icmp sgt i64 %"%\D8\A7\D9\84\D9\88\D9\82\D8\AA.load34", 0
  br i1 %assert.gt, label %agt.pass, label %agt.fail

agt.fail:                                         ; preds = %sec.pass
  call void @abort()
  unreachable

agt.pass:                                         ; preds = %sec.pass
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %rand.ret = call i32 @rand()
  %32 = sext i32 %rand.ret to i64
  %alloca35 = alloca i64, align 8
  store i64 %32, ptr %alloca35, align 8
  %rand.ret36 = call i32 @rand()
  %33 = sext i32 %rand.ret36 to i64
  %alloca37 = alloca i64, align 8
  store i64 %33, ptr %alloca37, align 8
  %"%\D8\B91.load" = load i64, ptr %alloca35, align 8
  %strbuf38 = alloca i8, i64 32, align 1
  %34 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf38, i64 32, ptr @int.fmt, i64 %"%\D8\B91.load")
  %len139 = call i64 @strlen(ptr @str.const.41)
  %len240 = call i64 @strlen(ptr %strbuf38)
  %totallen41 = add i64 %len139, %len240
  %bufsize42 = add i64 %totallen41, 1
  %concat.buf43 = alloca i8, i64 %bufsize42, align 1
  %35 = call ptr @memcpy(ptr %concat.buf43, ptr @str.const.41, i64 %len139)
  %dest244 = getelementptr i8, ptr %concat.buf43, i64 %len139
  %copylen245 = add i64 %len240, 1
  %36 = call ptr @memcpy(ptr %dest244, ptr %strbuf38, i64 %copylen245)
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf43)
  %38 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B92.load" = load i64, ptr %alloca37, align 8
  %strbuf46 = alloca i8, i64 32, align 1
  %39 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf46, i64 32, ptr @int.fmt, i64 %"%\D8\B92.load")
  %len147 = call i64 @strlen(ptr @str.const.46)
  %len248 = call i64 @strlen(ptr %strbuf46)
  %totallen49 = add i64 %len147, %len248
  %bufsize50 = add i64 %totallen49, 1
  %concat.buf51 = alloca i8, i64 %bufsize50, align 1
  %40 = call ptr @memcpy(ptr %concat.buf51, ptr @str.const.46, i64 %len147)
  %dest252 = getelementptr i8, ptr %concat.buf51, i64 %len147
  %copylen253 = add i64 %len248, 1
  %41 = call ptr @memcpy(ptr %dest252, ptr %strbuf46, i64 %copylen253)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf51)
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %"%\D8\B91.load54" = load i64, ptr %alloca35, align 8
  %assert.gt55 = icmp sgt i64 %"%\D8\B91.load54", 0
  br i1 %assert.gt55, label %agt.pass57, label %agt.fail56

agt.fail56:                                       ; preds = %agt.pass
  call void @abort()
  unreachable

agt.pass57:                                       ; preds = %agt.pass
  %"%\D8\B92.load58" = load i64, ptr %alloca37, align 8
  %assert.gt59 = icmp sgt i64 %"%\D8\B92.load58", 0
  br i1 %assert.gt59, label %agt.pass61, label %agt.fail60

agt.fail60:                                       ; preds = %agt.pass57
  call void @abort()
  unreachable

agt.pass61:                                       ; preds = %agt.pass57
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.51)
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.55)
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.59)
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret i64 0
}

declare i32 @printf(ptr, ...)

declare ptr @sad_security_sanitize(ptr)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @strcmp(ptr, ptr)

declare void @abort()

declare i64 @time(ptr)

declare i32 @snprintf(ptr, i64, ptr, ...)

declare i32 @rand()

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
