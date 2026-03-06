; ModuleID = 'test_string_param'
source_filename = "test_string_param"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@str.const = private unnamed_addr constant [4 x i8] c">> \00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.2 = private unnamed_addr constant [25 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\85\D9\86 \D8\AF\D8\A7\D9\84\D8\A9\00", align 1
@str.const.3 = private unnamed_addr constant [16 x i8] c"\D8\B3\D8\B7\D8\B1 \D8\AB\D8\A7\D9\86\D9\8A\00", align 1
@str.const.4 = private unnamed_addr constant [5 x i8] c"\D8\B5! \00", align 1
@str.const.5 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\86\D8\AA\D9\87\D9\89\00", align 1

define void @"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\AD\D8\AA\D9\88\D9\891" = alloca ptr, align 8
  store ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\891", align 8
  %"%\D9\85\D8\AD\D8\AA\D9\88\D9\89.load" = load ptr, ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\891", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"%\D9\85\D8\AD\D8\AA\D9\88\D9\89.load")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"%\D9\85\D8\AD\D8\AA\D9\88\D9\89.load", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  ret void
}

define void @"\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\B5"(ptr %"\D9\86\D8\B5", i64 %"\D8\B9\D8\AF\D8\AF") {
entry:
  %"%\D8\B9" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF2" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF", ptr %"\D8\B9\D8\AF\D8\AF2", align 8
  %"\D9\86\D8\B51" = alloca ptr, align 8
  store ptr %"\D9\86\D8\B5", ptr %"\D9\86\D8\B51", align 8
  store i64 0, ptr %"%\D8\B9", align 8
  br label %while_cond_0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D9\86\D8\B5.load" = load ptr, ptr %"\D9\86\D8\B51", align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D9\86\D8\B5.load")
  %"%\D8\B9.load3" = load i64, ptr %"%\D8\B9", align 8
  %addtmp = add i64 %"%\D8\B9.load3", 1
  store i64 %addtmp, ptr %"%\D8\B9", align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", %"%\D8\B9\D8\AF\D8\AF.load"
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret void
}

define void @__sad_main() {
entry:
  call void @"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(ptr @str.const.2)
  call void @"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(ptr @str.const.3)
  call void @"\D8\AA\D9\83\D8\B1\D8\A7\D8\B1_\D9\86\D8\B5"(ptr @str.const.4, i64 3)
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  ret void
}

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
