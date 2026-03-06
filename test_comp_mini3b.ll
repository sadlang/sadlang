; ModuleID = 'test_comp_mini3b'
source_filename = "test_comp_mini3b"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@"\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = global i64 0
@fmt.d = private unnamed_addr constant [5 x i8] c"%lld\00", align 1

define void @__sad_main() {
entry:
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 3, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep1 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data2 = load ptr, ptr %arr.data.gep1, align 8
  %arr.elem = getelementptr i64, ptr %arr.data2, i64 0
  store i64 10, ptr %arr.elem, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 1
  store i64 20, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 30, ptr %arr.elem8, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 0
  %arr.get = load i64, ptr %arr.elem11, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %arr.get)
  %arr.ptr.int12 = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr13 = inttoptr i64 %arr.ptr.int12 to ptr
  %arr.data.gep14 = getelementptr inbounds %SadArray, ptr %arr.ptr13, i32 0, i32 2
  %arr.data15 = load ptr, ptr %arr.data.gep14, align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 1
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %arr.get17)
  %arr.ptr.int18 = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr19 = inttoptr i64 %arr.ptr.int18 to ptr
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr.ptr19, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 2
  %arr.get23 = load i64, ptr %arr.elem22, align 8
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.d, i64 %arr.get23)
  ret void
}

declare ptr @malloc(i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
