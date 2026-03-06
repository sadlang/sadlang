; ModuleID = 'test_comp_mini1'
source_filename = "test_comp_mini1"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@"\D8\B3" = global i64 42
@str.const = private unnamed_addr constant [11 x i8] c"\D9\82\D9\8A\D9\85\D8\A9: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.3 = private unnamed_addr constant [9 x i8] c"\D8\B5\D8\AD\D9\8A\D8\AD\00", align 1
@str.const.5 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\AA\D8\B3\D8\A7\D9\88\D9\8A\00", align 1

define void @__sad_main() {
entry:
  %"%\D8\B3" = alloca i64, align 8
  store i64 42, ptr %"%\D8\B3", align 8
  %"%\D8\B3.load" = load i64, ptr %"%\D8\B3", align 8
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %"%\D8\B3.load")
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
  %4 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 15)
  %5 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 7)
  %6 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 28)
  br i1 true, label %then_0, label %merge_1

merge_1:                                          ; preds = %then_0, %entry
  br i1 true, label %then_2, label %merge_3

merge_3:                                          ; preds = %then_2, %merge_1
  ret void

then_0:                                           ; preds = %entry
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  br label %merge_1

then_2:                                           ; preds = %merge_1
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  br label %merge_3
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
