; ModuleID = 'test_array_builtin'
source_filename = "test_array_builtin"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }
%SadArray.0 = type { i64, i64, ptr }

@"\D9\82\D8\A7\D8\A6\D9\85\D8\A9" = global i64 0
@str.const = private unnamed_addr constant [20 x i8] c"\D8\AA\D9\85 \D8\A7\D9\84\D8\A5\D8\B6\D8\A7\D9\81\D8\A9\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

define void @__sad_main() {
entry:
  %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9" = alloca i64, align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 0, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 8, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 8))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9", align 8
  %arr.len.gep1 = getelementptr inbounds %SadArray.0, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9", i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep1, align 8
  %arr.data.gep2 = getelementptr inbounds %SadArray.0, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9", i32 0, i32 2
  %arr.data3 = load ptr, ptr %arr.data.gep2, align 8
  %arr.elem = getelementptr i64, ptr %arr.data3, i64 %arr.len
  store i64 1, ptr %arr.elem, align 8
  %new.len = add i64 %arr.len, 1
  store i64 %new.len, ptr %arr.len.gep1, align 8
  %arr.len.gep4 = getelementptr inbounds %SadArray.0, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9", i32 0, i32 0
  %arr.len5 = load i64, ptr %arr.len.gep4, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray.0, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9", i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %arr.len5
  store i64 2, ptr %arr.elem8, align 8
  %new.len9 = add i64 %arr.len5, 1
  store i64 %new.len9, ptr %arr.len.gep4, align 8
  %arr.len.gep10 = getelementptr inbounds %SadArray.0, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9", i32 0, i32 0
  %arr.len11 = load i64, ptr %arr.len.gep10, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray.0, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9", i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %arr.len11
  store i64 3, ptr %arr.elem14, align 8
  %new.len15 = add i64 %arr.len11, 1
  store i64 %new.len15, ptr %arr.len.gep10, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret void
}

declare ptr @malloc(i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
