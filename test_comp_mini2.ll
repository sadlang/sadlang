; ModuleID = 'test_comp_mini2'
source_filename = "test_comp_mini2"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@"\D8\B9\D8\AF\D8\A7\D8\AF" = global i64 0
@str.const = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D8\B9\D8\AF\D8\A7\D8\AF: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [9 x i8] c"\D8\AC\D9\85\D8\B9: \00", align 1
@str.const.4 = private unnamed_addr constant [6 x i8] c"5! = \00", align 1

define i64 @"\D8\AC\D9\85\D8\B9"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %addtmp = add i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %addtmp
}

define i64 @"\D8\B9\D8\A7\D9\85\D9\84\D9\8A"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpletmp = icmp sle i64 %"%\D9\86.load", 1
  br i1 %cmpletmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %entry
  %"%\D9\86.load2" = load i64, ptr %"\D9\861", align 8
  %subtmp = sub i64 %"%\D9\86.load2", 1
  %calltmp = call i64 @"\D8\B9\D8\A7\D9\85\D9\84\D9\8A"(i64 %subtmp)
  %"%\D9\86.load3" = load i64, ptr %"\D9\861", align 8
  %multmp = mul i64 %"%\D9\86.load3", %calltmp
  ret i64 %multmp

then_0:                                           ; preds = %entry
  ret i64 1
}

define void @__sad_main() {
entry:
  %"%\D8\B9\D8\AF\D8\A7\D8\AF" = alloca i64, align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  br label %while_cond_2

while_body_3:                                     ; preds = %while_cond_2
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load1" = load i64, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load1", 1
  store i64 %addtmp, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  br label %while_cond_2

while_cond_2:                                     ; preds = %while_body_3, %entry
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 3
  br i1 %cmplttmp, label %while_body_3, label %while_exit_4

while_exit_4:                                     ; preds = %while_cond_2
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load2" = load i64, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load2")
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
  %calltmp = call i64 @"\D8\AC\D9\85\D8\B9"(i64 3, i64 7)
  %strbuf3 = alloca i8, i64 32, align 1
  %4 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf3, ptr @int.fmt, i64 %calltmp)
  %len14 = call i64 @strlen(ptr @str.const.1)
  %len25 = call i64 @strlen(ptr %strbuf3)
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = call ptr @malloc(i64 %bufsize7)
  %5 = call ptr @memcpy(ptr %concat.buf8, ptr @str.const.1, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %6 = call ptr @memcpy(ptr %dest29, ptr %strbuf3, i64 %copylen210)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf8)
  %calltmp11 = call i64 @"\D8\B9\D8\A7\D9\85\D9\84\D9\8A"(i64 5)
  %strbuf12 = alloca i8, i64 32, align 1
  %8 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf12, ptr @int.fmt, i64 %calltmp11)
  %len113 = call i64 @strlen(ptr @str.const.4)
  %len214 = call i64 @strlen(ptr %strbuf12)
  %totallen15 = add i64 %len113, %len214
  %bufsize16 = add i64 %totallen15, 1
  %concat.buf17 = call ptr @malloc(i64 %bufsize16)
  %9 = call ptr @memcpy(ptr %concat.buf17, ptr @str.const.4, i64 %len113)
  %dest218 = getelementptr i8, ptr %concat.buf17, i64 %len113
  %copylen219 = add i64 %len214, 1
  %10 = call ptr @memcpy(ptr %dest218, ptr %strbuf12, i64 %copylen219)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf17)
  ret void
}

declare i32 @sprintf(ptr, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
