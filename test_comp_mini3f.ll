; ModuleID = 'test_comp_mini3f'
source_filename = "test_comp_mini3f"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@"\D9\85\D8\AC" = global i64 0
@str.const = private unnamed_addr constant [17 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1

define void @__sad_main() {
entry:
  %"%\D8\B9\D9\86\D8\B5\D8\B1" = alloca ptr, align 8
  %"%_foreach_idx_0" = alloca i64, align 8
  %"%\D9\85\D8\AC" = alloca i64, align 8
  store i64 0, ptr %"%\D9\85\D8\AC", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 5, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 5, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 5))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep1 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data2 = load ptr, ptr %arr.data.gep1, align 8
  %arr.elem = getelementptr i64, ptr %arr.data2, i64 0
  store i64 1, ptr %arr.elem, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 1
  store i64 2, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 3, ptr %arr.elem8, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 3
  store i64 4, ptr %arr.elem11, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 4
  store i64 5, ptr %arr.elem14, align 8
  store i64 0, ptr %"%_foreach_idx_0", align 8
  br label %foreach_cond_0

foreach_body_1:                                   ; preds = %foreach_cond_0
  %loadtmp16 = load i64, ptr %"%_foreach_idx_0", align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem.ptr = getelementptr ptr, ptr %arr.data18, i64 %loadtmp16
  %arr.get.ptr = load ptr, ptr %arr.elem.ptr, align 8
  store ptr %arr.get.ptr, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %"%\D9\85\D8\AC.load" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load" = load ptr, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %addtmp = add i64 %"%\D9\85\D8\AC.load", ptr %"%\D8\B9\D9\86\D8\B5\D8\B1.load"
  store i64 %addtmp, ptr %"%\D9\85\D8\AC", align 8
  br label %foreach_inc_2

foreach_cond_0:                                   ; preds = %foreach_inc_2, %entry
  %loadtmp = load i64, ptr %"%_foreach_idx_0", align 8
  %arr.len.gep15 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep15, align 8
  %cmplttmp = icmp slt i64 %loadtmp, %arr.len
  br i1 %cmplttmp, label %foreach_body_1, label %foreach_exit_3

foreach_exit_3:                                   ; preds = %foreach_cond_0
  %"%\D9\85\D8\AC.load21" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %"%\D9\85\D8\AC.load21")
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
  ret void

foreach_inc_2:                                    ; preds = %foreach_body_1
  %loadtmp19 = load i64, ptr %"%_foreach_idx_0", align 8
  %addtmp20 = add i64 %loadtmp19, 1
  store i64 %addtmp20, ptr %"%_foreach_idx_0", align 8
  br label %foreach_cond_0
}

declare ptr @malloc(i64)

declare i32 @sprintf(ptr, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
