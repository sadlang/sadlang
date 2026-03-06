; ModuleID = 'دردشة'
source_filename = "\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@0 = private unnamed_addr constant [13 x i8] c"\D8\A7\D9\84\D9\85\D8\B1\D8\B3\D9\84\00", align 1
@1 = private unnamed_addr constant [9 x i8] c"\D8\A7\D9\84\D9\86\D8\B5\00", align 1
@2 = private unnamed_addr constant [11 x i8] c"\D8\A7\D9\84\D9\88\D9\82\D8\AA\00", align 1
@str.const = private unnamed_addr constant [3 x i8] c": \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@str.const.4 = private unnamed_addr constant [29 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\D9\8B \D8\A8\D8\A7\D9\84\D8\B9\D8\A7\D9\84\D9\85!\00", align 1
@str.const.5 = private unnamed_addr constant [41 x i8] c"\D9\87\D8\B0\D8\A7 \D8\AA\D8\B7\D8\A8\D9\8A\D9\82 \D8\AF\D8\B1\D8\AF\D8\B4\D8\A9 \D8\A8\D9\84\D8\BA\D8\A9 \D8\B5\00", align 1

define void @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 0, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 8, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 8))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %self1, align 8
  store i64 0, ptr %self1, align 8
  ret void
}

define i64 @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A3\D8\B1\D8\B3\D9\84_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(i64 %self, i64 %"\D9\86\D8\B5") {
entry:
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9" = alloca i64, align 8
  %"%2" = alloca i64, align 8
  %"\D9\86\D8\B52" = alloca i64, align 8
  store i64 %"\D9\86\D8\B5", ptr %"\D9\86\D8\B52", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %loadtmp = load i64, ptr %self1, align 8
  store i64 ptrtoint (ptr @0 to i64), ptr %"%2", align 8
  store i64 %loadtmp, ptr %"%2", align 8
  store i64 ptrtoint (ptr @1 to i64), ptr %"%2", align 8
  %"%\D9\86\D8\B5.load" = load i64, ptr %"\D9\86\D8\B52", align 8
  store i64 %"%\D9\86\D8\B5.load", ptr %"%2", align 8
  call void @"\D8\A7\D9\84\D8\A2\D9\86"()
  store i64 ptrtoint (ptr @2 to i64), ptr %"%2", align 8
  store i64 0, ptr %"%2", align 8
  %"%2.load" = load i64, ptr %"%2", align 8
  store i64 %"%2.load", ptr %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  %loadtmp3 = load i64, ptr %self1, align 8
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  call void @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A3\D8\B6\D9\81"(i64 %loadtmp3, i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  %loadtmp4 = load i64, ptr %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  ret i64 %loadtmp4
}

define void @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A7\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D8\A6\D9\84"(i64 %self) {
entry:
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9" = alloca i64, align 8
  %"%_foreach_idx_0" = alloca i64, align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %loadtmp = load i64, ptr %self1, align 8
  store i64 0, ptr %"%_foreach_idx_0", align 8
  br label %foreach_cond_0

foreach_body_1:                                   ; preds = %foreach_cond_0
  %loadtmp3 = load i64, ptr %"%_foreach_idx_0", align 8
  %arr.ptr.global4 = inttoptr i64 %loadtmp to ptr
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr.ptr.global4, i32 0, i32 2
  %arr.data = load ptr, ptr %arr.data.gep, align 8
  %arr.elem.ptr = getelementptr ptr, ptr %arr.data, i64 %loadtmp3
  %arr.get.ptr = load ptr, ptr %arr.elem.ptr, align 8
  %obj.ptrtoint = ptrtoint ptr %arr.get.ptr to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  %loadtmp5 = load i64, ptr %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  %strbuf = alloca i8, i64 32, align 1
  %0 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf, i64 32, ptr @int.fmt, i64 %loadtmp5)
  %len1 = call i64 @strlen(ptr %strbuf)
  %len2 = call i64 @strlen(ptr @str.const)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %1 = call ptr @memcpy(ptr %concat.buf, ptr %strbuf, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %2 = call ptr @memcpy(ptr %dest2, ptr @str.const, i64 %copylen2)
  %loadtmp6 = load i64, ptr %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", align 8
  %strbuf7 = alloca i8, i64 32, align 1
  %3 = call i32 (ptr, i64, ptr, ...) @snprintf(ptr %strbuf7, i64 32, ptr @int.fmt, i64 %loadtmp6)
  %len18 = call i64 @strlen(ptr %concat.buf)
  %len29 = call i64 @strlen(ptr %strbuf7)
  %totallen10 = add i64 %len18, %len29
  %bufsize11 = add i64 %totallen10, 1
  %concat.buf12 = call ptr @malloc(i64 %bufsize11)
  %4 = call ptr @memcpy(ptr %concat.buf12, ptr %concat.buf, i64 %len18)
  %dest213 = getelementptr i8, ptr %concat.buf12, i64 %len18
  %copylen214 = add i64 %len29, 1
  %5 = call ptr @memcpy(ptr %dest213, ptr %strbuf7, i64 %copylen214)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf12)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.2)
  br label %foreach_inc_2

foreach_cond_0:                                   ; preds = %foreach_inc_2, %entry
  %loadtmp2 = load i64, ptr %"%_foreach_idx_0", align 8
  %arr.ptr.global = inttoptr i64 %loadtmp to ptr
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr.ptr.global, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep, align 8
  %cmplttmp = icmp slt i64 %loadtmp2, %arr.len
  br i1 %cmplttmp, label %foreach_body_1, label %foreach_exit_3

foreach_exit_3:                                   ; preds = %foreach_cond_0
  ret void

foreach_inc_2:                                    ; preds = %foreach_body_1
  %loadtmp15 = load i64, ptr %"%_foreach_idx_0", align 8
  %addtmp = add i64 %loadtmp15, 1
  store i64 %addtmp, ptr %"%_foreach_idx_0", align 8
  br label %foreach_cond_0
}

define i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"() {
entry:
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82" = alloca i64, align 8
  %"%27" = alloca i64, align 8
  %"%27.load" = load i64, ptr %"%27", align 8
  call void @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%27.load")
  %"%27.load1" = load i64, ptr %"%27", align 8
  store i64 %"%27.load1", ptr %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", align 8
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load" = load i64, ptr %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", align 8
  %calltmp = call i64 @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A3\D8\B1\D8\B3\D9\84_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(i64 %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load", i64 ptrtoint (ptr @str.const.4 to i64))
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load2" = load i64, ptr %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", align 8
  %calltmp3 = call i64 @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A3\D8\B1\D8\B3\D9\84_\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(i64 %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load2", i64 ptrtoint (ptr @str.const.5 to i64))
  %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load4" = load i64, ptr %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82", align 8
  call void @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A7\D8\B9\D8\B1\D8\B6_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D8\A6\D9\84"(i64 %"%\D8\AA\D8\B7\D8\A8\D9\8A\D9\82.load4")
  ret i64 0
}

declare ptr @malloc(i64)

declare void @"\D8\A7\D9\84\D8\A2\D9\86"()

declare void @"\D8\AA\D8\B7\D8\A8\D9\8A\D9\82_\D8\AF\D8\B1\D8\AF\D8\B4\D8\A9.\D8\A3\D8\B6\D9\81"(i64, i64)

declare i32 @snprintf(ptr, i64, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %0 = call i64 @"\D8\B1\D8\A6\D9\8A\D8\B3\D9\8A\D8\A9"()
  %1 = trunc i64 %0 to i32
  ret i32 %1
}
