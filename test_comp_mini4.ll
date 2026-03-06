; ModuleID = 'test_comp_mini4'
source_filename = "test_comp_mini4"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%"class.\D8\AD\D9\8A\D9\88\D8\A7\D9\86" = type { ptr }
%"class.\D9\82\D8\B7\D8\A9" = type { ptr }

@"\D8\AF\D8\B1\D8\AC\D8\A9" = global i64 85
@"\D8\B1" = global i64 42
@"\D9\82" = global i64 0
@str.const = private unnamed_addr constant [4 x i8] c"...\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.1 = private unnamed_addr constant [10 x i8] c"\D9\85\D9\8A\D8\A7\D9\88!\00", align 1
@str.const.3 = private unnamed_addr constant [21 x i8] c"=== \D9\85\D8\B7\D8\A7\D8\A8\D9\82\D8\A9 ===\00", align 1
@str.const.5 = private unnamed_addr constant [19 x i8] c"=== \D8\AA\D8\AD\D9\88\D9\8A\D9\84 ===\00", align 1
@fmt_i64 = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@str.const.7 = private unnamed_addr constant [9 x i8] c"\D8\B1\D9\82\D9\85: \00", align 1
@str.const.9 = private unnamed_addr constant [15 x i8] c"=== \D8\B5\D9\86\D9\81 ===\00", align 1
@str.const.11 = private unnamed_addr constant [9 x i8] c"\D9\85\D9\8A\D9\85\D9\8A\00", align 1
@str.const.12 = private unnamed_addr constant [9 x i8] c"\D8\A7\D8\B3\D9\85: \00", align 1
@str.const.14 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\85\D8\AA\D8\A7\D8\B2\00", align 1
@str.const.16 = private unnamed_addr constant [16 x i8] c"\D8\AC\D9\8A\D8\AF \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.18 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\82\D8\A8\D9\88\D9\84\00", align 1

define void @"\D8\AD\D9\8A\D9\88\D8\A7\D9\86.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\A7\D8\B3\D9\852", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\AD\D9\8A\D9\88\D8\A7\D9\86", ptr %"%self.objptr", i32 0, i32 0
  %int2ptr = inttoptr i64 %"%\D8\A7\D8\B3\D9\85.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  ret void
}

define void @"\D8\AD\D9\8A\D9\88\D8\A7\D9\86.\D8\B5\D9\88\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const)
  ret void
}

define void @"\D9\82\D8\B7\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A7\D8\B3\D9\85") {
entry:
  %"\D8\A7\D8\B3\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.load" = load i64, ptr %self1, align 8
  %"%\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\A7\D8\B3\D9\852", align 8
  call void @"\D8\AD\D9\8A\D9\88\D8\A7\D9\86.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%self.load", i64 %"%\D8\A7\D8\B3\D9\85.load")
  ret void
}

define void @"\D9\82\D8\B7\D8\A9.\D8\B5\D9\88\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.1)
  ret void
}

define void @__sad_main() {
entry:
  %"%\D9\82" = alloca i64, align 8
  %"%\D8\B1" = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9" = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.3)
  store i64 85, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A9", align 8
  %loadtmp = load i64, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A9", align 8
  br label %match.case0.test_1

match.case0.body_2:                               ; preds = %match.case0.test_1
  %1 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.14)
  br label %match.end_0

match.case0.test_1:                               ; preds = %entry
  %cmpeqtmp = icmp eq i64 %loadtmp, 100
  br i1 %cmpeqtmp, label %match.case0.body_2, label %match.case1.test_3

match.case1.body_4:                               ; preds = %match.case1.test_3
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.16)
  br label %match.end_0

match.case1.test_3:                               ; preds = %match.case0.test_1
  %cmpeqtmp8 = icmp eq i64 %loadtmp, 85
  br i1 %cmpeqtmp8, label %match.case1.body_4, label %match.case2.test_5

match.case2.body_6:                               ; preds = %match.case2.test_5
  %3 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.18)
  br label %match.end_0

match.case2.test_5:                               ; preds = %match.case1.test_3
  br i1 true, label %match.case2.body_6, label %match.end_0

match.end_0:                                      ; preds = %match.case2.body_6, %match.case2.test_5, %match.case1.body_4, %match.case0.body_2
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.5)
  store i64 42, ptr %"%\D8\B1", align 8
  %"%\D8\B1.load" = load i64, ptr %"%\D8\B1", align 8
  %i64str_buf = call ptr @malloc(i64 32)
  %5 = call i32 (ptr, ptr, ...) @sprintf(ptr %i64str_buf, ptr @fmt_i64, i64 %"%\D8\B1.load")
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %i64str_buf)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %6 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %7 = call ptr @memcpy(ptr %dest2, ptr %i64str_buf, i64 %copylen2)
  %8 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %9 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.9)
  %"%20" = alloca %"class.\D9\82\D8\B7\D8\A9", align 8
  %arg.ptrtoint = ptrtoint ptr %"%20" to i64
  call void @"\D9\82\D8\B7\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint, i64 ptrtoint (ptr @str.const.11 to i64))
  %obj.ptrtoint = ptrtoint ptr %"%20" to i64
  store i64 %obj.ptrtoint, ptr %"%\D9\82", align 8
  %"%\D9\82.ptrval" = load i64, ptr %"%\D9\82", align 8
  %"%\D9\82.objptr" = inttoptr i64 %"%\D9\82.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D9\82\D8\B7\D8\A9", ptr %"%\D9\82.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val" = load ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %len11 = call i64 @strlen(ptr @str.const.12)
  %len22 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val")
  %totallen3 = add i64 %len11, %len22
  %bufsize4 = add i64 %totallen3, 1
  %concat.buf5 = call ptr @malloc(i64 %bufsize4)
  %10 = call ptr @memcpy(ptr %concat.buf5, ptr @str.const.12, i64 %len11)
  %dest26 = getelementptr i8, ptr %concat.buf5, i64 %len11
  %copylen27 = add i64 %len22, 1
  %11 = call ptr @memcpy(ptr %dest26, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val", i64 %copylen27)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf5)
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  call void @"\D9\82\D8\B7\D8\A9.\D8\B5\D9\88\D8\AA"(i64 %"%\D9\82.load")
  ret void
}

declare i32 @printf(ptr, ...)

declare ptr @malloc(i64)

declare i32 @sprintf(ptr, ptr, ...)

declare i64 @strlen(ptr)

declare ptr @memcpy(ptr, ptr, i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
