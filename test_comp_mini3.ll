; ModuleID = 'test_comp_mini3'
source_filename = "test_comp_mini3"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@"\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = global i64 0
@"\D9\85\D8\AC" = global i64 0
@str.const = private unnamed_addr constant [9 x i8] c"\D8\B7\D9\88\D9\84: \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [9 x i8] c"\D8\A3\D9\88\D9\84: \00", align 1
@str.const.4 = private unnamed_addr constant [9 x i8] c"\D8\A8\D8\B9\D8\AF: \00", align 1
@str.const.7 = private unnamed_addr constant [17 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9: \00", align 1

define void @__sad_main() {
entry:
  %"%\D8\B9\D9\86\D8\B5\D8\B1" = alloca ptr, align 8
  %"%_foreach_idx_0" = alloca i64, align 8
  %"%\D9\85\D8\AC" = alloca i64, align 8
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
  %arr.len.gep9 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep9, align 8
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %arr.len)
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
  %arr.ptr.int10 = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr11 = inttoptr i64 %arr.ptr.int10 to ptr
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr.ptr11, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 0
  %arr.get = load i64, ptr %arr.elem14, align 8
  %strbuf15 = alloca i8, i64 32, align 1
  %4 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf15, ptr @int.fmt, i64 %arr.get)
  %len116 = call i64 @strlen(ptr @str.const.1)
  %len217 = call i64 @strlen(ptr %strbuf15)
  %totallen18 = add i64 %len116, %len217
  %bufsize19 = add i64 %totallen18, 1
  %concat.buf20 = call ptr @malloc(i64 %bufsize19)
  %5 = call ptr @memcpy(ptr %concat.buf20, ptr @str.const.1, i64 %len116)
  %dest221 = getelementptr i8, ptr %concat.buf20, i64 %len116
  %copylen222 = add i64 %len217, 1
  %6 = call ptr @memcpy(ptr %dest221, ptr %strbuf15, i64 %copylen222)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf20)
  %arr.ptr.int23 = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr24 = inttoptr i64 %arr.ptr.int23 to ptr
  %arr.data.gep25 = getelementptr inbounds %SadArray, ptr %arr.ptr24, i32 0, i32 2
  %arr.data26 = load ptr, ptr %arr.data.gep25, align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 1
  store i64 99, ptr %arr.elem27, align 8
  %arr.ptr.int28 = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr29 = inttoptr i64 %arr.ptr.int28 to ptr
  %arr.data.gep30 = getelementptr inbounds %SadArray, ptr %arr.ptr29, i32 0, i32 2
  %arr.data31 = load ptr, ptr %arr.data.gep30, align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 1
  %arr.get33 = load i64, ptr %arr.elem32, align 8
  %strbuf34 = alloca i8, i64 32, align 1
  %8 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf34, ptr @int.fmt, i64 %arr.get33)
  %len135 = call i64 @strlen(ptr @str.const.4)
  %len236 = call i64 @strlen(ptr %strbuf34)
  %totallen37 = add i64 %len135, %len236
  %bufsize38 = add i64 %totallen37, 1
  %concat.buf39 = call ptr @malloc(i64 %bufsize38)
  %9 = call ptr @memcpy(ptr %concat.buf39, ptr @str.const.4, i64 %len135)
  %dest240 = getelementptr i8, ptr %concat.buf39, i64 %len135
  %copylen241 = add i64 %len236, 1
  %10 = call ptr @memcpy(ptr %dest240, ptr %strbuf34, i64 %copylen241)
  %11 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf39)
  store i64 0, ptr %"%\D9\85\D8\AC", align 8
  %arr_new42 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep43 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 0
  store i64 5, ptr %arr.len.gep43, align 8
  %arr.cap.gep44 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 1
  store i64 5, ptr %arr.cap.gep44, align 8
  %arr.data45 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 5))
  %arr.data.gep46 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 2
  store ptr %arr.data45, ptr %arr.data.gep46, align 8
  %arr.data.gep47 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 2
  %arr.data48 = load ptr, ptr %arr.data.gep47, align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 0
  store i64 1, ptr %arr.elem49, align 8
  %arr.data.gep50 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 2
  %arr.data51 = load ptr, ptr %arr.data.gep50, align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 1
  store i64 2, ptr %arr.elem52, align 8
  %arr.data.gep53 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 2
  %arr.data54 = load ptr, ptr %arr.data.gep53, align 8
  %arr.elem55 = getelementptr i64, ptr %arr.data54, i64 2
  store i64 3, ptr %arr.elem55, align 8
  %arr.data.gep56 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 2
  %arr.data57 = load ptr, ptr %arr.data.gep56, align 8
  %arr.elem58 = getelementptr i64, ptr %arr.data57, i64 3
  store i64 4, ptr %arr.elem58, align 8
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 4
  store i64 5, ptr %arr.elem61, align 8
  store i64 0, ptr %"%_foreach_idx_0", align 8
  br label %foreach_cond_0

foreach_body_1:                                   ; preds = %foreach_cond_0
  %loadtmp64 = load i64, ptr %"%_foreach_idx_0", align 8
  %arr.data.gep65 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 2
  %arr.data66 = load ptr, ptr %arr.data.gep65, align 8
  %arr.elem.ptr = getelementptr ptr, ptr %arr.data66, i64 %loadtmp64
  %arr.get.ptr = load ptr, ptr %arr.elem.ptr, align 8
  store ptr %arr.get.ptr, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %"%\D9\85\D8\AC.load" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load" = load ptr, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %addtmp = add i64 %"%\D9\85\D8\AC.load", ptr %"%\D8\B9\D9\86\D8\B5\D8\B1.load"
  store i64 %addtmp, ptr %"%\D9\85\D8\AC", align 8
  br label %foreach_inc_2

foreach_cond_0:                                   ; preds = %foreach_inc_2, %entry
  %loadtmp = load i64, ptr %"%_foreach_idx_0", align 8
  %arr.len.gep62 = getelementptr inbounds %SadArray, ptr %arr_new42, i32 0, i32 0
  %arr.len63 = load i64, ptr %arr.len.gep62, align 8
  %cmplttmp = icmp slt i64 %loadtmp, %arr.len63
  br i1 %cmplttmp, label %foreach_body_1, label %foreach_exit_3

foreach_exit_3:                                   ; preds = %foreach_cond_0
  %"%\D9\85\D8\AC.load69" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %strbuf70 = alloca i8, i64 32, align 1
  %12 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf70, ptr @int.fmt, i64 %"%\D9\85\D8\AC.load69")
  %len171 = call i64 @strlen(ptr @str.const.7)
  %len272 = call i64 @strlen(ptr %strbuf70)
  %totallen73 = add i64 %len171, %len272
  %bufsize74 = add i64 %totallen73, 1
  %concat.buf75 = call ptr @malloc(i64 %bufsize74)
  %13 = call ptr @memcpy(ptr %concat.buf75, ptr @str.const.7, i64 %len171)
  %dest276 = getelementptr i8, ptr %concat.buf75, i64 %len171
  %copylen277 = add i64 %len272, 1
  %14 = call ptr @memcpy(ptr %dest276, ptr %strbuf70, i64 %copylen277)
  %15 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf75)
  ret void

foreach_inc_2:                                    ; preds = %foreach_body_1
  %loadtmp67 = load i64, ptr %"%_foreach_idx_0", align 8
  %addtmp68 = add i64 %loadtmp67, 1
  store i64 %addtmp68, ptr %"%_foreach_idx_0", align 8
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
