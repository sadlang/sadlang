; ModuleID = 'test_comprehensive_sadc'
source_filename = "test_comprehensive_sadc"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%"class.\D8\B4\D8\AE\D8\B5" = type { ptr, i64 }
%"class.\D8\B7\D8\A7\D9\84\D8\A8" = type { ptr, ptr, ptr }
%"class.\D9\85\D8\B1\D9\83\D8\A8\D8\A9" = type { ptr }
%"class.\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9" = type { ptr, ptr }
%"class.\D8\B4\D9\83\D9\84" = type { ptr }
%"class.\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84" = type { ptr, i64, i64 }
%SadArray = type { i64, i64, ptr }
%"class.\D9\85\D8\B1\D8\A8\D8\B9" = type { ptr, i64, i64 }

@"\D8\B1\D9\82\D9\85" = global i64 42
@"\D8\AA\D8\AD\D9\8A\D8\A9" = global i64 0
@"\D8\B9\D8\B4\D8\B1\D9\8A1" = global i64 3
@"\D9\85\D9\86\D8\B7\D9\82" = global i64 1
@"\D8\B9\D9\85\D8\B1" = global i64 25
@"\D8\B9\D8\AF\D8\A7\D8\AF" = global i64 0
@"\D8\A3" = global i64 0
@"\D9\85\D8\AC" = global i64 0
@"\D8\B4" = global i64 0
@"\D8\B7" = global i64 0
@"\D8\B31" = global i64 0
@"\D9\851" = global i64 0
@"\D8\AF\D8\B1\D8\AC\D8\A9" = global i64 85
@"\D9\8A\D9\88\D9\85" = global i64 3
@"\D9\82" = global i64 1
@"\D8\B91" = global i64 1
@str.const = private unnamed_addr constant [8 x i8] c"\D8\A3\D9\86\D8\A7 \00", align 1
@str.const.1 = private unnamed_addr constant [11 x i8] c" \D8\B9\D9\85\D8\B1\D9\8A \00", align 1
@int.fmt = private unnamed_addr constant [5 x i8] c"%lld\00", align 1
@fmt.s = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@str.const.2 = private unnamed_addr constant [7 x i8] c" \D9\81\D9\8A \00", align 1
@str.const.4 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B1\D9\83\D8\A8\D8\A9: \00", align 1
@str.const.6 = private unnamed_addr constant [11 x i8] c"\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.7 = private unnamed_addr constant [12 x i8] c"\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9 \00", align 1
@str.const.9 = private unnamed_addr constant [9 x i8] c"\D8\B4\D9\83\D9\84: \00", align 1
@str.const.11 = private unnamed_addr constant [13 x i8] c"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84\00", align 1
@str.const.12 = private unnamed_addr constant [4 x i8] c">> \00", align 1
@str.const.14 = private unnamed_addr constant [25 x i8] c"=== 1 \D9\85\D8\AA\D8\BA\D9\8A\D8\B1\D8\A7\D8\AA ===\00", align 1
@str.const.16 = private unnamed_addr constant [11 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7\00", align 1
@str.const.18 = private unnamed_addr constant [6 x i8] c"\D9\86\D8\B5=\00", align 1
@str.const.20 = private unnamed_addr constant [19 x i8] c"=== 2 \D8\AD\D8\B3\D8\A7\D8\A8 ===\00", align 1
@str.const.27 = private unnamed_addr constant [17 x i8] c"=== 3 \D8\B1\D8\A8\D8\B7 ===\00", align 1
@str.const.29 = private unnamed_addr constant [15 x i8] c"\D8\A7\D9\84\D9\86\D8\A7\D8\AA\D8\AC: \00", align 1
@str.const.32 = private unnamed_addr constant [5 x i8] c"5+3=\00", align 1
@str.const.35 = private unnamed_addr constant [19 x i8] c"=== 4 \D8\B4\D8\B1\D9\88\D8\B7 ===\00", align 1
@str.const.37 = private unnamed_addr constant [5 x i8] c"10>5\00", align 1
@str.const.39 = private unnamed_addr constant [5 x i8] c"3==3\00", align 1
@str.const.41 = private unnamed_addr constant [5 x i8] c"5!=3\00", align 1
@str.const.43 = private unnamed_addr constant [9 x i8] c"\D8\A8\D8\A7\D9\84\D8\BA\00", align 1
@str.const.45 = private unnamed_addr constant [9 x i8] c"\D9\82\D8\A7\D8\B5\D8\B1\00", align 1
@str.const.47 = private unnamed_addr constant [21 x i8] c"=== 5 \D8\A8\D9\8A\D9\86\D9\85\D8\A7 ===\00", align 1
@str.const.49 = private unnamed_addr constant [14 x i8] c"\D8\A7\D9\84\D8\B9\D8\AF\D8\A7\D8\AF=\00", align 1
@str.const.52 = private unnamed_addr constant [19 x i8] c"=== 6 \D8\AF\D9\88\D8\A7\D9\84 ===\00", align 1
@str.const.54 = private unnamed_addr constant [5 x i8] c"3+7=\00", align 1
@str.const.57 = private unnamed_addr constant [5 x i8] c"4*5=\00", align 1
@str.const.60 = private unnamed_addr constant [21 x i8] c"=== 7 \D8\AA\D9\83\D8\B1\D8\A7\D8\B1 ===\00", align 1
@str.const.62 = private unnamed_addr constant [4 x i8] c"5!=\00", align 1
@str.const.65 = private unnamed_addr constant [25 x i8] c"=== 8 \D9\85\D8\B5\D9\81\D9\88\D9\81\D8\A7\D8\AA ===\00", align 1
@str.const.67 = private unnamed_addr constant [8 x i8] c"\D8\B7\D9\88\D9\84=\00", align 1
@str.const.70 = private unnamed_addr constant [5 x i8] c"[0]=\00", align 1
@str.const.73 = private unnamed_addr constant [5 x i8] c"[4]=\00", align 1
@str.const.76 = private unnamed_addr constant [5 x i8] c"[2]=\00", align 1
@str.const.79 = private unnamed_addr constant [17 x i8] c"=== 9 \D9\84\D9\83\D9\84 ===\00", align 1
@str.const.81 = private unnamed_addr constant [16 x i8] c"\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9=\00", align 1
@str.const.84 = private unnamed_addr constant [10 x i8] c"\D8\B9\D9\86\D8\B5\D8\B1=\00", align 1
@str.const.87 = private unnamed_addr constant [22 x i8] c"=== 10 \D8\A3\D8\B5\D9\86\D8\A7\D9\81 ===\00", align 1
@str.const.89 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\AF\00", align 1
@str.const.90 = private unnamed_addr constant [8 x i8] c"\D8\A7\D8\B3\D9\85=\00", align 1
@str.const.92 = private unnamed_addr constant [22 x i8] c"=== 11 \D9\88\D8\B1\D8\A7\D8\AB\D8\A9 ===\00", align 1
@str.const.94 = private unnamed_addr constant [9 x i8] c"\D8\B3\D8\A7\D8\B1\D8\A9\00", align 1
@str.const.95 = private unnamed_addr constant [26 x i8] c"\D8\AC\D8\A7\D9\85\D8\B9\D8\A9 \D8\A7\D9\84\D9\82\D8\A7\D9\87\D8\B1\D8\A9\00", align 1
@str.const.96 = private unnamed_addr constant [22 x i8] c"=== 12 \D8\AA\D8\AC\D8\A7\D9\88\D8\B2 ===\00", align 1
@str.const.98 = private unnamed_addr constant [9 x i8] c"\D8\A3\D8\AD\D9\85\D8\B1\00", align 1
@str.const.99 = private unnamed_addr constant [22 x i8] c"=== 13 \D8\B3\D9\84\D8\B3\D9\84\D8\A9 ===\00", align 1
@str.const.101 = private unnamed_addr constant [8 x i8] c"\D8\B9\D8\B1\D8\B6=\00", align 1
@str.const.104 = private unnamed_addr constant [24 x i8] c"=== 14 \D9\85\D8\B7\D8\A7\D8\A8\D9\82\D8\A9 ===\00", align 1
@str.const.106 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\85\D8\AA\D8\A7\D8\B2\00", align 1
@str.const.108 = private unnamed_addr constant [16 x i8] c"\D8\AC\D9\8A\D8\AF \D8\AC\D8\AF\D8\A7\D9\8B\00", align 1
@str.const.110 = private unnamed_addr constant [7 x i8] c"\D8\AC\D9\8A\D8\AF\00", align 1
@str.const.112 = private unnamed_addr constant [11 x i8] c"\D9\85\D9\82\D8\A8\D9\88\D9\84\00", align 1
@str.const.114 = private unnamed_addr constant [22 x i8] c"=== 15 \D8\AA\D8\AD\D9\88\D9\8A\D9\84 ===\00", align 1
@str.const.116 = private unnamed_addr constant [8 x i8] c"\D8\B1\D9\82\D9\85=\00", align 1
@str.const.119 = private unnamed_addr constant [10 x i8] c"\D8\AD\D8\B3\D8\A7\D8\A8=\00", align 1
@str.const.121 = private unnamed_addr constant [22 x i8] c"=== 16 \D8\A5\D8\B3\D9\86\D8\A7\D8\AF ===\00", align 1
@str.const.123 = private unnamed_addr constant [4 x i8] c"\D9\82=\00", align 1
@str.const.126 = private unnamed_addr constant [20 x i8] c"=== 17 \D8\AF\D9\88\D8\A7\D9\84 ===\00", align 1
@str.const.128 = private unnamed_addr constant [15 x i8] c"\D8\A3\D9\83\D8\A8\D8\B1(5,3)=\00", align 1
@str.const.131 = private unnamed_addr constant [15 x i8] c"\D8\A3\D9\83\D8\A8\D8\B1(2,8)=\00", align 1
@str.const.134 = private unnamed_addr constant [25 x i8] c"=== 18 \D9\86\D8\B5_\D8\AF\D8\A7\D9\84\D8\A9 ===\00", align 1
@str.const.136 = private unnamed_addr constant [25 x i8] c"\D9\85\D8\B1\D8\AD\D8\A8\D8\A7 \D9\85\D9\86 \D8\AF\D8\A7\D9\84\D8\A9\00", align 1
@str.const.137 = private unnamed_addr constant [24 x i8] c"=== 19 \D9\85\D8\AA\D8\AF\D8\A7\D8\AE\D9\84 ===\00", align 1
@str.const.139 = private unnamed_addr constant [7 x i8] c"\D8\A3\D8\AD\D8\AF\00", align 1
@str.const.141 = private unnamed_addr constant [11 x i8] c"\D8\A7\D8\AB\D9\86\D9\8A\D9\86\00", align 1
@str.const.143 = private unnamed_addr constant [13 x i8] c"\D8\AB\D9\84\D8\A7\D8\AB\D8\A7\D8\A1\00", align 1
@str.const.145 = private unnamed_addr constant [7 x i8] c"\D8\A2\D8\AE\D8\B1\00", align 1
@str.const.147 = private unnamed_addr constant [20 x i8] c"=== 20 \D9\81\D9\8A\D8\A8\D9\88 ===\00", align 1
@str.const.149 = private unnamed_addr constant [14 x i8] c"\D9\81\D9\8A\D8\A8\D9\88(10)=\00", align 1
@str.const.152 = private unnamed_addr constant [43 x i8] c"=== \D9\83\D9\84 \D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D8\A8\D8\A7\D8\B1\D8\A7\D8\AA \D9\86\D8\AC\D8\AD\D8\AA ===\00", align 1

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

define i64 @"\D8\B6\D8\B1\D8\A8"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %multmp = mul i64 %"%\D8\A3.load", %"%\D8\A8.load"
  ret i64 %multmp
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

define void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A7\D8\B3\D9\85", i64 %"\D8\B9\D9\85\D8\B1") {
entry:
  %"\D8\B9\D9\85\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D8\B1", ptr %"\D8\B9\D9\85\D8\B13", align 8
  %"\D8\A7\D8\B3\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\A7\D8\B3\D9\852", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr", i32 0, i32 0
  %int2ptr = inttoptr i64 %"%\D8\A7\D8\B3\D9\85.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %"%\D8\B9\D9\85\D8\B1.load" = load i64, ptr %"\D8\B9\D9\85\D8\B13", align 8
  %"%self.ptrval4" = load i64, ptr %self1, align 8
  %"%self.objptr5" = inttoptr i64 %"%self.ptrval4" to ptr
  %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr5", i32 0, i32 1
  store i64 %"%\D8\B9\D9\85\D8\B1.load", ptr %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep", align 8
  ret void
}

define void @"\D8\B4\D8\AE\D8\B5.\D8\AA\D9\82\D8\AF\D9\8A\D9\85"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val" = load ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %len1 = call i64 @strlen(ptr @str.const)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val", i64 %copylen2)
  %len12 = call i64 @strlen(ptr %concat.buf)
  %len23 = call i64 @strlen(ptr @str.const.1)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = call ptr @malloc(i64 %bufsize5)
  %2 = call ptr @memcpy(ptr %concat.buf6, ptr %concat.buf, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %3 = call ptr @memcpy(ptr %dest27, ptr @str.const.1, i64 %copylen28)
  %"%self.ptrval9" = load i64, ptr %self1, align 8
  %"%self.objptr10" = inttoptr i64 %"%self.ptrval9" to ptr
  %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%self.objptr10", i32 0, i32 1
  %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1.val" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1_gep", align 8
  %strbuf = alloca i8, i64 32, align 1
  %4 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 %"\D8\A7\D9\84\D8\B9\D9\85\D8\B1.val")
  %len111 = call i64 @strlen(ptr %concat.buf6)
  %len212 = call i64 @strlen(ptr %strbuf)
  %totallen13 = add i64 %len111, %len212
  %bufsize14 = add i64 %totallen13, 1
  %concat.buf15 = call ptr @malloc(i64 %bufsize14)
  %5 = call ptr @memcpy(ptr %concat.buf15, ptr %concat.buf6, i64 %len111)
  %dest216 = getelementptr i8, ptr %concat.buf15, i64 %len111
  %copylen217 = add i64 %len212, 1
  %6 = call ptr @memcpy(ptr %dest216, ptr %strbuf, i64 %copylen217)
  %7 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf15)
  ret void
}

define void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\A7\D8\B3\D9\85", i64 %"\D8\B9\D9\85\D8\B1", i64 %"\D8\AC\D8\A7\D9\85\D8\B9\D8\A9") {
entry:
  %"\D8\AC\D8\A7\D9\85\D8\B9\D8\A94" = alloca i64, align 8
  store i64 %"\D8\AC\D8\A7\D9\85\D8\B9\D8\A9", ptr %"\D8\AC\D8\A7\D9\85\D8\B9\D8\A94", align 8
  %"\D8\B9\D9\85\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D8\B1", ptr %"\D8\B9\D9\85\D8\B13", align 8
  %"\D8\A7\D8\B3\D9\852" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B3\D9\85", ptr %"\D8\A7\D8\B3\D9\852", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.load" = load i64, ptr %self1, align 8
  %"%\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\A7\D8\B3\D9\852", align 8
  %"%\D8\B9\D9\85\D8\B1.load" = load i64, ptr %"\D8\B9\D9\85\D8\B13", align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%self.load", i64 %"%\D8\A7\D8\B3\D9\85.load", i64 %"%\D8\B9\D9\85\D8\B1.load")
  %"%\D8\AC\D8\A7\D9\85\D8\B9\D8\A9.load" = load i64, ptr %"\D8\AC\D8\A7\D9\85\D8\B9\D8\A94", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\AC\D8\A7\D9\85\D8\B9\D8\A9_gep" = getelementptr inbounds %"class.\D8\B7\D8\A7\D9\84\D8\A8", ptr %"%self.objptr", i32 0, i32 2
  %int2ptr = inttoptr i64 %"%\D8\AC\D8\A7\D9\85\D8\B9\D8\A9.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D8\AC\D8\A7\D9\85\D8\B9\D8\A9_gep", align 8
  ret void
}

define void @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\B7\D8\A7\D9\84\D8\A8", ptr %"%self.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val" = load ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %len1 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val")
  %len2 = call i64 @strlen(ptr @str.const.2)
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val", i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr @str.const.2, i64 %copylen2)
  %"%self.ptrval2" = load i64, ptr %self1, align 8
  %"%self.objptr3" = inttoptr i64 %"%self.ptrval2" to ptr
  %"\D8\A7\D9\84\D8\AC\D8\A7\D9\85\D8\B9\D8\A9_gep" = getelementptr inbounds %"class.\D8\B7\D8\A7\D9\84\D8\A8", ptr %"%self.objptr3", i32 0, i32 2
  %"\D8\A7\D9\84\D8\AC\D8\A7\D9\85\D8\B9\D8\A9.val" = load ptr, ptr %"\D8\A7\D9\84\D8\AC\D8\A7\D9\85\D8\B9\D8\A9_gep", align 8
  %len14 = call i64 @strlen(ptr %concat.buf)
  %len25 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\AC\D8\A7\D9\85\D8\B9\D8\A9.val")
  %totallen6 = add i64 %len14, %len25
  %bufsize7 = add i64 %totallen6, 1
  %concat.buf8 = call ptr @malloc(i64 %bufsize7)
  %2 = call ptr @memcpy(ptr %concat.buf8, ptr %concat.buf, i64 %len14)
  %dest29 = getelementptr i8, ptr %concat.buf8, i64 %len14
  %copylen210 = add i64 %len25, 1
  %3 = call ptr @memcpy(ptr %dest29, ptr %"\D8\A7\D9\84\D8\AC\D8\A7\D9\85\D8\B9\D8\A9.val", i64 %copylen210)
  %4 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf8)
  ret void
}

define void @"\D9\85\D8\B1\D9\83\D8\A8\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D9\86\D9\88\D8\B9") {
entry:
  %"\D9\86\D9\88\D8\B92" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9", ptr %"\D9\86\D9\88\D8\B92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%\D9\86\D9\88\D8\B9.load" = load i64, ptr %"\D9\86\D9\88\D8\B92", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep" = getelementptr inbounds %"class.\D9\85\D8\B1\D9\83\D8\A8\D8\A9", ptr %"%self.objptr", i32 0, i32 0
  %int2ptr = inttoptr i64 %"%\D9\86\D9\88\D8\B9.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep", align 8
  ret void
}

define void @"\D9\85\D8\B1\D9\83\D8\A8\D8\A9.\D9\88\D8\B5\D9\81"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep" = getelementptr inbounds %"class.\D9\85\D8\B1\D9\83\D8\A8\D8\A9", ptr %"%self.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B9.val" = load ptr, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9_gep", align 8
  %len1 = call i64 @strlen(ptr @str.const.4)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9.val")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.4, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B9.val", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  ret void
}

define void @"\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D9\84\D9\88\D9\86") {
entry:
  %"\D9\84\D9\88\D9\862" = alloca i64, align 8
  store i64 %"\D9\84\D9\88\D9\86", ptr %"\D9\84\D9\88\D9\862", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.load" = load i64, ptr %self1, align 8
  call void @"\D9\85\D8\B1\D9\83\D8\A8\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%self.load", i64 ptrtoint (ptr @str.const.6 to i64))
  %"%\D9\84\D9\88\D9\86.load" = load i64, ptr %"\D9\84\D9\88\D9\862", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D9\84\D9\88\D9\86_gep" = getelementptr inbounds %"class.\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9", ptr %"%self.objptr", i32 0, i32 1
  %int2ptr = inttoptr i64 %"%\D9\84\D9\88\D9\86.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D9\84\D9\88\D9\86_gep", align 8
  ret void
}

define void @"\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9.\D9\88\D8\B5\D9\81"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D9\84\D9\88\D9\86_gep" = getelementptr inbounds %"class.\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9", ptr %"%self.objptr", i32 0, i32 1
  %"\D8\A7\D9\84\D9\84\D9\88\D9\86.val" = load ptr, ptr %"\D8\A7\D9\84\D9\84\D9\88\D9\86_gep", align 8
  %len1 = call i64 @strlen(ptr @str.const.7)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D9\84\D9\84\D9\88\D9\86.val")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.7, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D9\84\D9\84\D9\88\D9\86.val", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  ret void
}

define void @"\D8\B4\D9\83\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D9\86\D9\88\D8\B9") {
entry:
  %"\D9\86\D9\88\D8\B92" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9", ptr %"\D9\86\D9\88\D8\B92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%\D9\86\D9\88\D8\B9.load" = load i64, ptr %"\D9\86\D9\88\D8\B92", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B92_gep" = getelementptr inbounds %"class.\D8\B4\D9\83\D9\84", ptr %"%self.objptr", i32 0, i32 0
  %int2ptr = inttoptr i64 %"%\D9\86\D9\88\D8\B9.load" to ptr
  store ptr %int2ptr, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B92_gep", align 8
  ret void
}

define void @"\D8\B4\D9\83\D9\84.\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\B4\D9\83\D9\84"(i64 %self) {
entry:
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B92_gep" = getelementptr inbounds %"class.\D8\B4\D9\83\D9\84", ptr %"%self.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B92.val" = load ptr, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B92_gep", align 8
  %len1 = call i64 @strlen(ptr @str.const.9)
  %len2 = call i64 @strlen(ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B92.val")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.9, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B92.val", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  ret void
}

define void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\B9\D8\B1\D8\B6", i64 %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9") {
entry:
  %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B93" = alloca i64, align 8
  store i64 %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9", ptr %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B93", align 8
  %"\D8\B9\D8\B1\D8\B62" = alloca i64, align 8
  store i64 %"\D8\B9\D8\B1\D8\B6", ptr %"\D8\B9\D8\B1\D8\B62", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.load" = load i64, ptr %self1, align 8
  call void @"\D8\B4\D9\83\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%self.load", i64 ptrtoint (ptr @str.const.11 to i64))
  %"%\D8\B9\D8\B1\D8\B6.load" = load i64, ptr %"\D8\B9\D8\B1\D8\B62", align 8
  %"%self.ptrval" = load i64, ptr %self1, align 8
  %"%self.objptr" = inttoptr i64 %"%self.ptrval" to ptr
  %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6_gep" = getelementptr inbounds %"class.\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84", ptr %"%self.objptr", i32 0, i32 1
  store i64 %"%\D8\B9\D8\B1\D8\B6.load", ptr %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6_gep", align 8
  %"%\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9.load" = load i64, ptr %"\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B93", align 8
  %"%self.ptrval4" = load i64, ptr %self1, align 8
  %"%self.objptr5" = inttoptr i64 %"%self.ptrval4" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9_gep" = getelementptr inbounds %"class.\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84", ptr %"%self.objptr5", i32 0, i32 2
  store i64 %"%\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9.load", ptr %"\D8\A7\D9\84\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9_gep", align 8
  ret void
}

define void @"\D9\85\D8\B1\D8\A8\D8\B9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %self, i64 %"\D8\B6\D9\84\D8\B9") {
entry:
  %"\D8\B6\D9\84\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B6\D9\84\D8\B9", ptr %"\D8\B6\D9\84\D8\B92", align 8
  %self1 = alloca i64, align 8
  store i64 %self, ptr %self1, align 8
  %"%self.load" = load i64, ptr %self1, align 8
  %"%\D8\B6\D9\84\D8\B9.load" = load i64, ptr %"\D8\B6\D9\84\D8\B92", align 8
  %"%\D8\B6\D9\84\D8\B9.load3" = load i64, ptr %"\D8\B6\D9\84\D8\B92", align 8
  call void @"\D9\85\D8\B3\D8\AA\D8\B7\D9\8A\D9\84.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %"%self.load", i64 %"%\D8\B6\D9\84\D8\B9.load", i64 %"%\D8\B6\D9\84\D8\B9.load3")
  ret void
}

define i64 @"\D8\A3\D9\83\D8\A8\D8\B1"(i64 %"\D8\A3", i64 %"\D8\A8") {
entry:
  %"\D8\A82" = alloca i64, align 8
  store i64 %"\D8\A8", ptr %"\D8\A82", align 8
  %"\D8\A31" = alloca i64, align 8
  store i64 %"\D8\A3", ptr %"\D8\A31", align 8
  %"%\D8\A3.load" = load i64, ptr %"\D8\A31", align 8
  %"%\D8\A8.load" = load i64, ptr %"\D8\A82", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A3.load", %"%\D8\A8.load"
  br i1 %cmpgttmp, label %then_2, label %merge_3

merge_3:                                          ; preds = %entry
  %loadtmp3 = load i64, ptr %"\D8\A82", align 8
  ret i64 %loadtmp3

then_2:                                           ; preds = %entry
  %loadtmp = load i64, ptr %"\D8\A31", align 8
  ret i64 %loadtmp
}

define void @"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89") {
entry:
  %"\D9\85\D8\AD\D8\AA\D9\88\D9\891" = alloca ptr, align 8
  store ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\891", align 8
  %"%\D9\85\D8\AD\D8\AA\D9\88\D9\89.load" = load ptr, ptr %"\D9\85\D8\AD\D8\AA\D9\88\D9\891", align 8
  %len1 = call i64 @strlen(ptr @str.const.12)
  %len2 = call i64 @strlen(ptr %"%\D9\85\D8\AD\D8\AA\D9\88\D9\89.load")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %0 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.12, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %1 = call ptr @memcpy(ptr %dest2, ptr %"%\D9\85\D8\AD\D8\AA\D9\88\D9\89.load", i64 %copylen2)
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  ret void
}

define i64 @"\D9\81\D9\8A\D8\A8\D9\88"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpletmp = icmp sle i64 %"%\D9\86.load", 0
  br i1 %cmpletmp, label %then_4, label %merge_5

merge_5:                                          ; preds = %entry
  %"%\D9\86.load2" = load i64, ptr %"\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86.load2", 1
  br i1 %cmpeqtmp, label %then_6, label %merge_7

merge_7:                                          ; preds = %merge_5
  %"%\D9\86.load3" = load i64, ptr %"\D9\861", align 8
  %subtmp = sub i64 %"%\D9\86.load3", 1
  %calltmp = call i64 @"\D9\81\D9\8A\D8\A8\D9\88"(i64 %subtmp)
  %"%\D9\86.load4" = load i64, ptr %"\D9\861", align 8
  %subtmp5 = sub i64 %"%\D9\86.load4", 2
  %calltmp6 = call i64 @"\D9\81\D9\8A\D8\A8\D9\88"(i64 %subtmp5)
  %addtmp = add i64 %calltmp, %calltmp6
  ret i64 %addtmp

then_4:                                           ; preds = %entry
  ret i64 0

then_6:                                           ; preds = %merge_5
  ret i64 1
}

define void @__sad_main() {
entry:
  %"%\D8\B92" = alloca i64, align 8
  %"%\D8\B91" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\8A\D9\88\D9\85" = alloca i64, align 8
  %"%\D8\AF\D8\B1\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\851" = alloca i64, align 8
  %"%\D8\B31" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"%\D8\B4" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"%_foreach_idx_24" = alloca i64, align 8
  %"%\D8\B9\D9\86\D8\B5\D8\B1" = alloca i64, align 8
  %"%_foreach_idx_20" = alloca i64, align 8
  %"%\D9\85\D8\AC" = alloca i64, align 8
  %"%\D8\A3" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\A7\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\B7\D9\82" = alloca i1, align 1
  %"%\D8\B9\D8\B4\D8\B1\D9\8A1" = alloca i64, align 8
  %"%\D8\AA\D8\AD\D9\8A\D8\A9" = alloca ptr, align 8
  %"%\D8\B1\D9\82\D9\85" = alloca i64, align 8
  %0 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.14)
  store i64 42, ptr %"%\D8\B1\D9\82\D9\85", align 8
  store ptr @str.const.16, ptr %"%\D8\AA\D8\AD\D9\8A\D8\A9", align 8
  store i64 3, ptr %"%\D8\B9\D8\B4\D8\B1\D9\8A1", align 8
  store i1 true, ptr %"%\D9\85\D9\86\D8\B7\D9\82", align 1
  %"%\D8\B1\D9\82\D9\85.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85", align 8
  %1 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 %"%\D8\B1\D9\82\D9\85.load")
  %"%\D8\AA\D8\AD\D9\8A\D8\A9.load" = load ptr, ptr %"%\D8\AA\D8\AD\D9\8A\D8\A9", align 8
  %2 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %"%\D8\AA\D8\AD\D9\8A\D8\A9.load")
  %"%\D8\AA\D8\AD\D9\8A\D8\A9.load1" = load ptr, ptr %"%\D8\AA\D8\AD\D9\8A\D8\A9", align 8
  %len1 = call i64 @strlen(ptr @str.const.18)
  %len2 = call i64 @strlen(ptr %"%\D8\AA\D8\AD\D9\8A\D8\A9.load1")
  %totallen = add i64 %len1, %len2
  %bufsize = add i64 %totallen, 1
  %concat.buf = call ptr @malloc(i64 %bufsize)
  %3 = call ptr @memcpy(ptr %concat.buf, ptr @str.const.18, i64 %len1)
  %dest2 = getelementptr i8, ptr %concat.buf, i64 %len1
  %copylen2 = add i64 %len2, 1
  %4 = call ptr @memcpy(ptr %dest2, ptr %"%\D8\AA\D8\AD\D9\8A\D8\A9.load1", i64 %copylen2)
  %5 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf)
  %6 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.20)
  %7 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 15)
  %8 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 7)
  %9 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 28)
  %10 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 5)
  %11 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 2)
  %12 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.27)
  %strbuf = alloca i8, i64 32, align 1
  %13 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf, ptr @int.fmt, i64 100)
  %len12 = call i64 @strlen(ptr @str.const.29)
  %len23 = call i64 @strlen(ptr %strbuf)
  %totallen4 = add i64 %len12, %len23
  %bufsize5 = add i64 %totallen4, 1
  %concat.buf6 = call ptr @malloc(i64 %bufsize5)
  %14 = call ptr @memcpy(ptr %concat.buf6, ptr @str.const.29, i64 %len12)
  %dest27 = getelementptr i8, ptr %concat.buf6, i64 %len12
  %copylen28 = add i64 %len23, 1
  %15 = call ptr @memcpy(ptr %dest27, ptr %strbuf, i64 %copylen28)
  %16 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf6)
  %strbuf9 = alloca i8, i64 32, align 1
  %17 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf9, ptr @int.fmt, i64 8)
  %len110 = call i64 @strlen(ptr @str.const.32)
  %len211 = call i64 @strlen(ptr %strbuf9)
  %totallen12 = add i64 %len110, %len211
  %bufsize13 = add i64 %totallen12, 1
  %concat.buf14 = call ptr @malloc(i64 %bufsize13)
  %18 = call ptr @memcpy(ptr %concat.buf14, ptr @str.const.32, i64 %len110)
  %dest215 = getelementptr i8, ptr %concat.buf14, i64 %len110
  %copylen216 = add i64 %len211, 1
  %19 = call ptr @memcpy(ptr %dest215, ptr %strbuf9, i64 %copylen216)
  %20 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf14)
  %21 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.35)
  br i1 true, label %then_8, label %merge_9

else_16:                                          ; preds = %merge_13
  %22 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.45)
  br label %merge_15

foreach_body_21:                                  ; preds = %foreach_cond_20
  %loadtmp145 = load i64, ptr %"%_foreach_idx_20", align 8
  %arr.data.gep146 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 2
  %arr.data147 = load ptr, ptr %arr.data.gep146, align 8
  %arr.elem148 = getelementptr i64, ptr %arr.data147, i64 %loadtmp145
  %arr.get149 = load i64, ptr %arr.elem148, align 8
  store i64 %arr.get149, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %"%\D9\85\D8\AC.load" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %addtmp150 = add i64 %"%\D9\85\D8\AC.load", %"%\D8\B9\D9\86\D8\B5\D8\B1.load"
  store i64 %addtmp150, ptr %"%\D9\85\D8\AC", align 8
  br label %foreach_inc_22

foreach_body_25:                                  ; preds = %foreach_cond_24
  %loadtmp180 = load i64, ptr %"%_foreach_idx_24", align 8
  %arr.data.gep181 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 2
  %arr.data182 = load ptr, ptr %arr.data.gep181, align 8
  %arr.elem183 = getelementptr i64, ptr %arr.data182, i64 %loadtmp180
  %arr.get184 = load i64, ptr %arr.elem183, align 8
  store i64 %arr.get184, ptr %"%\D8\B9", align 8
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %strbuf185 = alloca i8, i64 32, align 1
  %23 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf185, ptr @int.fmt, i64 %"%\D8\B9.load")
  %len1186 = call i64 @strlen(ptr @str.const.84)
  %len2187 = call i64 @strlen(ptr %strbuf185)
  %totallen188 = add i64 %len1186, %len2187
  %bufsize189 = add i64 %totallen188, 1
  %concat.buf190 = call ptr @malloc(i64 %bufsize189)
  %24 = call ptr @memcpy(ptr %concat.buf190, ptr @str.const.84, i64 %len1186)
  %dest2191 = getelementptr i8, ptr %concat.buf190, i64 %len1186
  %copylen2192 = add i64 %len2187, 1
  %25 = call ptr @memcpy(ptr %dest2191, ptr %strbuf185, i64 %copylen2192)
  %26 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf190)
  br label %foreach_inc_26

foreach_cond_20:                                  ; preds = %foreach_inc_22, %while_exit_19
  %loadtmp = load i64, ptr %"%_foreach_idx_20", align 8
  %arr.len.gep142 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 0
  %arr.len143 = load i64, ptr %arr.len.gep142, align 8
  %cmplttmp144 = icmp slt i64 %loadtmp, %arr.len143
  br i1 %cmplttmp144, label %foreach_body_21, label %foreach_exit_23

foreach_cond_24:                                  ; preds = %foreach_inc_26, %foreach_exit_23
  %loadtmp176 = load i64, ptr %"%_foreach_idx_24", align 8
  %arr.len.gep177 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 0
  %arr.len178 = load i64, ptr %arr.len.gep177, align 8
  %cmplttmp179 = icmp slt i64 %loadtmp176, %arr.len178
  br i1 %cmplttmp179, label %foreach_body_25, label %foreach_exit_27

foreach_exit_23:                                  ; preds = %foreach_cond_20
  %"%\D9\85\D8\AC.load153" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %strbuf154 = alloca i8, i64 32, align 1
  %27 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf154, ptr @int.fmt, i64 %"%\D9\85\D8\AC.load153")
  %len1155 = call i64 @strlen(ptr @str.const.81)
  %len2156 = call i64 @strlen(ptr %strbuf154)
  %totallen157 = add i64 %len1155, %len2156
  %bufsize158 = add i64 %totallen157, 1
  %concat.buf159 = call ptr @malloc(i64 %bufsize158)
  %28 = call ptr @memcpy(ptr %concat.buf159, ptr @str.const.81, i64 %len1155)
  %dest2160 = getelementptr i8, ptr %concat.buf159, i64 %len1155
  %copylen2161 = add i64 %len2156, 1
  %29 = call ptr @memcpy(ptr %dest2160, ptr %strbuf154, i64 %copylen2161)
  %30 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf159)
  %arr_new162 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep163 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 0
  store i64 3, ptr %arr.len.gep163, align 8
  %arr.cap.gep164 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep164, align 8
  %arr.data165 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep166 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 2
  store ptr %arr.data165, ptr %arr.data.gep166, align 8
  %arr.data.gep167 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 2
  %arr.data168 = load ptr, ptr %arr.data.gep167, align 8
  %arr.elem169 = getelementptr i64, ptr %arr.data168, i64 0
  store i64 10, ptr %arr.elem169, align 8
  %arr.data.gep170 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 2
  %arr.data171 = load ptr, ptr %arr.data.gep170, align 8
  %arr.elem172 = getelementptr i64, ptr %arr.data171, i64 1
  store i64 20, ptr %arr.elem172, align 8
  %arr.data.gep173 = getelementptr inbounds %SadArray, ptr %arr_new162, i32 0, i32 2
  %arr.data174 = load ptr, ptr %arr.data.gep173, align 8
  %arr.elem175 = getelementptr i64, ptr %arr.data174, i64 2
  store i64 30, ptr %arr.elem175, align 8
  store i64 0, ptr %"%_foreach_idx_24", align 8
  br label %foreach_cond_24

foreach_exit_27:                                  ; preds = %foreach_cond_24
  %31 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.87)
  %"%204" = alloca %"class.\D8\B4\D8\AE\D8\B5", align 8
  %arg.ptrtoint = ptrtoint ptr %"%204" to i64
  call void @"\D8\B4\D8\AE\D8\B5.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint, i64 ptrtoint (ptr @str.const.89 to i64), i64 30)
  %obj.ptrtoint195 = ptrtoint ptr %"%204" to i64
  store i64 %obj.ptrtoint195, ptr %"%\D8\B4", align 8
  %"%\D8\B4.load" = load i64, ptr %"%\D8\B4", align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\AA\D9\82\D8\AF\D9\8A\D9\85"(i64 %"%\D8\B4.load")
  %"%\D8\B4.ptrval" = load i64, ptr %"%\D8\B4", align 8
  %"%\D8\B4.objptr" = inttoptr i64 %"%\D8\B4.ptrval" to ptr
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep" = getelementptr inbounds %"class.\D8\B4\D8\AE\D8\B5", ptr %"%\D8\B4.objptr", i32 0, i32 0
  %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val" = load ptr, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85_gep", align 8
  %len1196 = call i64 @strlen(ptr @str.const.90)
  %len2197 = call i64 @strlen(ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val")
  %totallen198 = add i64 %len1196, %len2197
  %bufsize199 = add i64 %totallen198, 1
  %concat.buf200 = call ptr @malloc(i64 %bufsize199)
  %32 = call ptr @memcpy(ptr %concat.buf200, ptr @str.const.90, i64 %len1196)
  %dest2201 = getelementptr i8, ptr %concat.buf200, i64 %len1196
  %copylen2202 = add i64 %len2197, 1
  %33 = call ptr @memcpy(ptr %dest2201, ptr %"\D8\A7\D9\84\D8\A7\D8\B3\D9\85.val", i64 %copylen2202)
  %34 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf200)
  %35 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.92)
  %"%213" = alloca %"class.\D8\B7\D8\A7\D9\84\D8\A8", align 8
  %arg.ptrtoint203 = ptrtoint ptr %"%213" to i64
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint203, i64 ptrtoint (ptr @str.const.94 to i64), i64 22, i64 ptrtoint (ptr @str.const.95 to i64))
  %obj.ptrtoint204 = ptrtoint ptr %"%213" to i64
  store i64 %obj.ptrtoint204, ptr %"%\D8\B7", align 8
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  call void @"\D8\B4\D8\AE\D8\B5.\D8\AA\D9\82\D8\AF\D9\8A\D9\85"(i64 %"%\D8\B7.load")
  %"%\D8\B7.load205" = load i64, ptr %"%\D8\B7", align 8
  call void @"\D8\B7\D8\A7\D9\84\D8\A8.\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA"(i64 %"%\D8\B7.load205")
  %36 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.96)
  %"%221" = alloca %"class.\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9", align 8
  %arg.ptrtoint206 = ptrtoint ptr %"%221" to i64
  call void @"\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint206, i64 ptrtoint (ptr @str.const.98 to i64))
  %obj.ptrtoint207 = ptrtoint ptr %"%221" to i64
  store i64 %obj.ptrtoint207, ptr %"%\D8\B31", align 8
  %"%\D8\B31.load" = load i64, ptr %"%\D8\B31", align 8
  call void @"\D8\B3\D9\8A\D8\A7\D8\B1\D8\A9.\D9\88\D8\B5\D9\81"(i64 %"%\D8\B31.load")
  %37 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.99)
  %"%226" = alloca %"class.\D9\85\D8\B1\D8\A8\D8\B9", align 8
  %arg.ptrtoint208 = ptrtoint ptr %"%226" to i64
  call void @"\D9\85\D8\B1\D8\A8\D8\B9.\D8\A8\D9\86\D8\A7\D8\A1"(i64 %arg.ptrtoint208, i64 5)
  %obj.ptrtoint209 = ptrtoint ptr %"%226" to i64
  store i64 %obj.ptrtoint209, ptr %"%\D9\851", align 8
  %"%\D9\851.load" = load i64, ptr %"%\D9\851", align 8
  call void @"\D8\B4\D9\83\D9\84.\D8\A7\D8\B3\D9\85_\D8\A7\D9\84\D8\B4\D9\83\D9\84"(i64 %"%\D9\851.load")
  %"%\D9\851.ptrval" = load i64, ptr %"%\D9\851", align 8
  %"%\D9\851.objptr" = inttoptr i64 %"%\D9\851.ptrval" to ptr
  %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6_gep" = getelementptr inbounds %"class.\D9\85\D8\B1\D8\A8\D8\B9", ptr %"%\D9\851.objptr", i32 0, i32 1
  %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.val" = load i64, ptr %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6_gep", align 8
  %strbuf210 = alloca i8, i64 32, align 1
  %38 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf210, ptr @int.fmt, i64 %"\D8\A7\D9\84\D8\B9\D8\B1\D8\B6.val")
  %len1211 = call i64 @strlen(ptr @str.const.101)
  %len2212 = call i64 @strlen(ptr %strbuf210)
  %totallen213 = add i64 %len1211, %len2212
  %bufsize214 = add i64 %totallen213, 1
  %concat.buf215 = call ptr @malloc(i64 %bufsize214)
  %39 = call ptr @memcpy(ptr %concat.buf215, ptr @str.const.101, i64 %len1211)
  %dest2216 = getelementptr i8, ptr %concat.buf215, i64 %len1211
  %copylen2217 = add i64 %len2212, 1
  %40 = call ptr @memcpy(ptr %dest2216, ptr %strbuf210, i64 %copylen2217)
  %41 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf215)
  %42 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.104)
  store i64 85, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A9", align 8
  %loadtmp218 = load i64, ptr %"%\D8\AF\D8\B1\D8\AC\D8\A9", align 8
  br label %match.case0.test_29

foreach_inc_22:                                   ; preds = %foreach_body_21
  %loadtmp151 = load i64, ptr %"%_foreach_idx_20", align 8
  %addtmp152 = add i64 %loadtmp151, 1
  store i64 %addtmp152, ptr %"%_foreach_idx_20", align 8
  br label %foreach_cond_20

foreach_inc_26:                                   ; preds = %foreach_body_25
  %loadtmp193 = load i64, ptr %"%_foreach_idx_24", align 8
  %addtmp194 = add i64 %loadtmp193, 1
  store i64 %addtmp194, ptr %"%_foreach_idx_24", align 8
  br label %foreach_cond_24

match.case0.body_30:                              ; preds = %match.case0.test_29
  %43 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.106)
  br label %match.end_28

match.case0.body_39:                              ; preds = %match.case0.test_38
  %44 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.139)
  br label %match.end_37

match.case0.test_29:                              ; preds = %foreach_exit_27
  %cmpeqtmp = icmp eq i64 %loadtmp218, 100
  br i1 %cmpeqtmp, label %match.case0.body_30, label %match.case1.test_31

match.case0.test_38:                              ; preds = %match.end_28
  %cmpeqtmp267 = icmp eq i64 %loadtmp219, 1
  br i1 %cmpeqtmp267, label %match.case0.body_39, label %match.case1.test_40

match.case1.body_32:                              ; preds = %match.case1.test_31
  %45 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.108)
  br label %match.end_28

match.case1.body_41:                              ; preds = %match.case1.test_40
  %46 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.141)
  br label %match.end_37

match.case1.test_31:                              ; preds = %match.case0.test_29
  %cmpeqtmp220 = icmp eq i64 %loadtmp218, 85
  br i1 %cmpeqtmp220, label %match.case1.body_32, label %match.case2.test_33

match.case1.test_40:                              ; preds = %match.case0.test_38
  %cmpeqtmp268 = icmp eq i64 %loadtmp219, 2
  br i1 %cmpeqtmp268, label %match.case1.body_41, label %match.case2.test_42

match.case2.body_34:                              ; preds = %match.case2.test_33
  %47 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.110)
  br label %match.end_28

match.case2.body_43:                              ; preds = %match.case2.test_42
  %48 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.143)
  br label %match.end_37

match.case2.test_33:                              ; preds = %match.case1.test_31
  %cmpeqtmp221 = icmp eq i64 %loadtmp218, 70
  br i1 %cmpeqtmp221, label %match.case2.body_34, label %match.case3.test_35

match.case2.test_42:                              ; preds = %match.case1.test_40
  %cmpeqtmp269 = icmp eq i64 %loadtmp219, 3
  br i1 %cmpeqtmp269, label %match.case2.body_43, label %match.case3.test_44

match.case3.body_36:                              ; preds = %match.case3.test_35
  %49 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.112)
  br label %match.end_28

match.case3.body_45:                              ; preds = %match.case3.test_44
  %50 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.145)
  br label %match.end_37

match.case3.test_35:                              ; preds = %match.case2.test_33
  br i1 true, label %match.case3.body_36, label %match.end_28

match.case3.test_44:                              ; preds = %match.case2.test_42
  br i1 true, label %match.case3.body_45, label %match.end_37

match.end_28:                                     ; preds = %match.case3.body_36, %match.case3.test_35, %match.case2.body_34, %match.case1.body_32, %match.case0.body_30
  store i64 3, ptr %"%\D9\8A\D9\88\D9\85", align 8
  %loadtmp219 = load i64, ptr %"%\D9\8A\D9\88\D9\85", align 8
  br label %match.case0.test_38

match.end_37:                                     ; preds = %match.case3.body_45, %match.case3.test_44, %match.case2.body_43, %match.case1.body_41, %match.case0.body_39
  %51 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.114)
  %i64str_buf = call ptr @malloc(i64 32)
  %52 = call i32 (ptr, ptr, ...) @sprintf(ptr %i64str_buf, ptr @int.fmt, i64 42)
  %len1222 = call i64 @strlen(ptr @str.const.116)
  %len2223 = call i64 @strlen(ptr %i64str_buf)
  %totallen224 = add i64 %len1222, %len2223
  %bufsize225 = add i64 %totallen224, 1
  %concat.buf226 = call ptr @malloc(i64 %bufsize225)
  %53 = call ptr @memcpy(ptr %concat.buf226, ptr @str.const.116, i64 %len1222)
  %dest2227 = getelementptr i8, ptr %concat.buf226, i64 %len1222
  %copylen2228 = add i64 %len2223, 1
  %54 = call ptr @memcpy(ptr %dest2227, ptr %i64str_buf, i64 %copylen2228)
  %55 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf226)
  %calltmp229 = call i64 @"\D8\AC\D9\85\D8\B9"(i64 100, i64 200)
  %i64str_buf230 = call ptr @malloc(i64 32)
  %56 = call i32 (ptr, ptr, ...) @sprintf(ptr %i64str_buf230, ptr @int.fmt, i64 %calltmp229)
  %len1231 = call i64 @strlen(ptr @str.const.119)
  %len2232 = call i64 @strlen(ptr %i64str_buf230)
  %totallen233 = add i64 %len1231, %len2232
  %bufsize234 = add i64 %totallen233, 1
  %concat.buf235 = call ptr @malloc(i64 %bufsize234)
  %57 = call ptr @memcpy(ptr %concat.buf235, ptr @str.const.119, i64 %len1231)
  %dest2236 = getelementptr i8, ptr %concat.buf235, i64 %len1231
  %copylen2237 = add i64 %len2232, 1
  %58 = call ptr @memcpy(ptr %dest2236, ptr %i64str_buf230, i64 %copylen2237)
  %59 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf235)
  %60 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.121)
  store i64 1, ptr %"%\D9\82", align 8
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %addtmp238 = add i64 %"%\D9\82.load", 1
  store i64 %addtmp238, ptr %"%\D9\82", align 8
  %"%\D9\82.load239" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load239", 10
  store i64 %multmp, ptr %"%\D9\82", align 8
  %"%\D9\82.load240" = load i64, ptr %"%\D9\82", align 8
  %strbuf241 = alloca i8, i64 32, align 1
  %61 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf241, ptr @int.fmt, i64 %"%\D9\82.load240")
  %len1242 = call i64 @strlen(ptr @str.const.123)
  %len2243 = call i64 @strlen(ptr %strbuf241)
  %totallen244 = add i64 %len1242, %len2243
  %bufsize245 = add i64 %totallen244, 1
  %concat.buf246 = call ptr @malloc(i64 %bufsize245)
  %62 = call ptr @memcpy(ptr %concat.buf246, ptr @str.const.123, i64 %len1242)
  %dest2247 = getelementptr i8, ptr %concat.buf246, i64 %len1242
  %copylen2248 = add i64 %len2243, 1
  %63 = call ptr @memcpy(ptr %dest2247, ptr %strbuf241, i64 %copylen2248)
  %64 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf246)
  %65 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.126)
  %calltmp249 = call i64 @"\D8\A3\D9\83\D8\A8\D8\B1"(i64 5, i64 3)
  %strbuf250 = alloca i8, i64 32, align 1
  %66 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf250, ptr @int.fmt, i64 %calltmp249)
  %len1251 = call i64 @strlen(ptr @str.const.128)
  %len2252 = call i64 @strlen(ptr %strbuf250)
  %totallen253 = add i64 %len1251, %len2252
  %bufsize254 = add i64 %totallen253, 1
  %concat.buf255 = call ptr @malloc(i64 %bufsize254)
  %67 = call ptr @memcpy(ptr %concat.buf255, ptr @str.const.128, i64 %len1251)
  %dest2256 = getelementptr i8, ptr %concat.buf255, i64 %len1251
  %copylen2257 = add i64 %len2252, 1
  %68 = call ptr @memcpy(ptr %dest2256, ptr %strbuf250, i64 %copylen2257)
  %69 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf255)
  %calltmp258 = call i64 @"\D8\A3\D9\83\D8\A8\D8\B1"(i64 2, i64 8)
  %strbuf259 = alloca i8, i64 32, align 1
  %70 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf259, ptr @int.fmt, i64 %calltmp258)
  %len1260 = call i64 @strlen(ptr @str.const.131)
  %len2261 = call i64 @strlen(ptr %strbuf259)
  %totallen262 = add i64 %len1260, %len2261
  %bufsize263 = add i64 %totallen262, 1
  %concat.buf264 = call ptr @malloc(i64 %bufsize263)
  %71 = call ptr @memcpy(ptr %concat.buf264, ptr @str.const.131, i64 %len1260)
  %dest2265 = getelementptr i8, ptr %concat.buf264, i64 %len1260
  %copylen2266 = add i64 %len2261, 1
  %72 = call ptr @memcpy(ptr %dest2265, ptr %strbuf259, i64 %copylen2266)
  %73 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf264)
  %74 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.134)
  call void @"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9"(ptr @str.const.136)
  %75 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.137)
  store i64 1, ptr %"%\D8\B91", align 8
  br label %while_cond_46

merge_11:                                         ; preds = %then_10, %merge_9
  br i1 true, label %then_12, label %merge_13

merge_13:                                         ; preds = %then_12, %merge_11
  store i64 25, ptr %"%\D8\B9\D9\85\D8\B1", align 8
  %"%\D8\B9\D9\85\D8\B1.load" = load i64, ptr %"%\D8\B9\D9\85\D8\B1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D9\85\D8\B1.load", 18
  br i1 %cmpgetmp, label %then_14, label %else_16

merge_15:                                         ; preds = %else_16, %then_14
  %76 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.47)
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  br label %while_cond_17

merge_53:                                         ; preds = %then_52, %while_body_50
  %"%\D8\B92.load286" = load i64, ptr %"%\D8\B92", align 8
  %addtmp287 = add i64 %"%\D8\B92.load286", 1
  store i64 %addtmp287, ptr %"%\D8\B92", align 8
  br label %while_cond_49

merge_9:                                          ; preds = %then_8, %entry
  br i1 true, label %then_10, label %merge_11

then_10:                                          ; preds = %merge_9
  %77 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.39)
  br label %merge_11

then_12:                                          ; preds = %merge_11
  %78 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.41)
  br label %merge_13

then_14:                                          ; preds = %merge_13
  %79 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.43)
  br label %merge_15

then_52:                                          ; preds = %while_body_50
  %"%\D8\B91.load285" = load i64, ptr %"%\D8\B91", align 8
  %80 = call i32 (ptr, ...) @printf(ptr @int.fmt, i64 %"%\D8\B91.load285")
  br label %merge_53

then_8:                                           ; preds = %entry
  %81 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.37)
  br label %merge_9

while_body_18:                                    ; preds = %while_cond_17
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load17" = load i64, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load17", 1
  store i64 %addtmp, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  br label %while_cond_17

while_body_47:                                    ; preds = %while_cond_46
  store i64 1, ptr %"%\D8\B92", align 8
  br label %while_cond_49

while_body_50:                                    ; preds = %while_cond_49
  %"%\D8\B91.load280" = load i64, ptr %"%\D8\B91", align 8
  %"%\D8\B92.load281" = load i64, ptr %"%\D8\B92", align 8
  %cmpeqtmp282 = icmp eq i64 %"%\D8\B91.load280", %"%\D8\B92.load281"
  br i1 %cmpeqtmp282, label %then_52, label %merge_53

while_cond_17:                                    ; preds = %while_body_18, %merge_15
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load", 5
  br i1 %cmplttmp, label %while_body_18, label %while_exit_19

while_cond_46:                                    ; preds = %while_exit_51, %match.end_37
  %"%\D8\B91.load" = load i64, ptr %"%\D8\B91", align 8
  %cmpletmp = icmp sle i64 %"%\D8\B91.load", 3
  br i1 %cmpletmp, label %while_body_47, label %while_exit_48

while_cond_49:                                    ; preds = %merge_53, %while_body_47
  %"%\D8\B92.load" = load i64, ptr %"%\D8\B92", align 8
  %cmpletmp279 = icmp sle i64 %"%\D8\B92.load", 3
  br i1 %cmpletmp279, label %while_body_50, label %while_exit_51

while_exit_19:                                    ; preds = %while_cond_17
  %"%\D8\B9\D8\AF\D8\A7\D8\AF.load18" = load i64, ptr %"%\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  %strbuf19 = alloca i8, i64 32, align 1
  %82 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf19, ptr @int.fmt, i64 %"%\D8\B9\D8\AF\D8\A7\D8\AF.load18")
  %len120 = call i64 @strlen(ptr @str.const.49)
  %len221 = call i64 @strlen(ptr %strbuf19)
  %totallen22 = add i64 %len120, %len221
  %bufsize23 = add i64 %totallen22, 1
  %concat.buf24 = call ptr @malloc(i64 %bufsize23)
  %83 = call ptr @memcpy(ptr %concat.buf24, ptr @str.const.49, i64 %len120)
  %dest225 = getelementptr i8, ptr %concat.buf24, i64 %len120
  %copylen226 = add i64 %len221, 1
  %84 = call ptr @memcpy(ptr %dest225, ptr %strbuf19, i64 %copylen226)
  %85 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf24)
  %86 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.52)
  %calltmp = call i64 @"\D8\AC\D9\85\D8\B9"(i64 3, i64 7)
  %strbuf27 = alloca i8, i64 32, align 1
  %87 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf27, ptr @int.fmt, i64 %calltmp)
  %len128 = call i64 @strlen(ptr @str.const.54)
  %len229 = call i64 @strlen(ptr %strbuf27)
  %totallen30 = add i64 %len128, %len229
  %bufsize31 = add i64 %totallen30, 1
  %concat.buf32 = call ptr @malloc(i64 %bufsize31)
  %88 = call ptr @memcpy(ptr %concat.buf32, ptr @str.const.54, i64 %len128)
  %dest233 = getelementptr i8, ptr %concat.buf32, i64 %len128
  %copylen234 = add i64 %len229, 1
  %89 = call ptr @memcpy(ptr %dest233, ptr %strbuf27, i64 %copylen234)
  %90 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf32)
  %calltmp35 = call i64 @"\D8\B6\D8\B1\D8\A8"(i64 4, i64 5)
  %strbuf36 = alloca i8, i64 32, align 1
  %91 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf36, ptr @int.fmt, i64 %calltmp35)
  %len137 = call i64 @strlen(ptr @str.const.57)
  %len238 = call i64 @strlen(ptr %strbuf36)
  %totallen39 = add i64 %len137, %len238
  %bufsize40 = add i64 %totallen39, 1
  %concat.buf41 = call ptr @malloc(i64 %bufsize40)
  %92 = call ptr @memcpy(ptr %concat.buf41, ptr @str.const.57, i64 %len137)
  %dest242 = getelementptr i8, ptr %concat.buf41, i64 %len137
  %copylen243 = add i64 %len238, 1
  %93 = call ptr @memcpy(ptr %dest242, ptr %strbuf36, i64 %copylen243)
  %94 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf41)
  %95 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.60)
  %calltmp44 = call i64 @"\D8\B9\D8\A7\D9\85\D9\84\D9\8A"(i64 5)
  %strbuf45 = alloca i8, i64 32, align 1
  %96 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf45, ptr @int.fmt, i64 %calltmp44)
  %len146 = call i64 @strlen(ptr @str.const.62)
  %len247 = call i64 @strlen(ptr %strbuf45)
  %totallen48 = add i64 %len146, %len247
  %bufsize49 = add i64 %totallen48, 1
  %concat.buf50 = call ptr @malloc(i64 %bufsize49)
  %97 = call ptr @memcpy(ptr %concat.buf50, ptr @str.const.62, i64 %len146)
  %dest251 = getelementptr i8, ptr %concat.buf50, i64 %len146
  %copylen252 = add i64 %len247, 1
  %98 = call ptr @memcpy(ptr %dest251, ptr %strbuf45, i64 %copylen252)
  %99 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf50)
  %100 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.65)
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 5, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 5, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 5))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep53 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data54 = load ptr, ptr %arr.data.gep53, align 8
  %arr.elem = getelementptr i64, ptr %arr.data54, i64 0
  store i64 10, ptr %arr.elem, align 8
  %arr.data.gep55 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data56 = load ptr, ptr %arr.data.gep55, align 8
  %arr.elem57 = getelementptr i64, ptr %arr.data56, i64 1
  store i64 20, ptr %arr.elem57, align 8
  %arr.data.gep58 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data59 = load ptr, ptr %arr.data.gep58, align 8
  %arr.elem60 = getelementptr i64, ptr %arr.data59, i64 2
  store i64 30, ptr %arr.elem60, align 8
  %arr.data.gep61 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data62 = load ptr, ptr %arr.data.gep61, align 8
  %arr.elem63 = getelementptr i64, ptr %arr.data62, i64 3
  store i64 40, ptr %arr.elem63, align 8
  %arr.data.gep64 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data65 = load ptr, ptr %arr.data.gep64, align 8
  %arr.elem66 = getelementptr i64, ptr %arr.data65, i64 4
  store i64 50, ptr %arr.elem66, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.len.gep67 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 0
  %arr.len = load i64, ptr %arr.len.gep67, align 8
  %strbuf68 = alloca i8, i64 32, align 1
  %101 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf68, ptr @int.fmt, i64 %arr.len)
  %len169 = call i64 @strlen(ptr @str.const.67)
  %len270 = call i64 @strlen(ptr %strbuf68)
  %totallen71 = add i64 %len169, %len270
  %bufsize72 = add i64 %totallen71, 1
  %concat.buf73 = call ptr @malloc(i64 %bufsize72)
  %102 = call ptr @memcpy(ptr %concat.buf73, ptr @str.const.67, i64 %len169)
  %dest274 = getelementptr i8, ptr %concat.buf73, i64 %len169
  %copylen275 = add i64 %len270, 1
  %103 = call ptr @memcpy(ptr %dest274, ptr %strbuf68, i64 %copylen275)
  %104 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf73)
  %arr.ptr.int76 = load i64, ptr %"%\D8\A3", align 8
  %arr.ptr77 = inttoptr i64 %arr.ptr.int76 to ptr
  %arr.data.gep78 = getelementptr inbounds %SadArray, ptr %arr.ptr77, i32 0, i32 2
  %arr.data79 = load ptr, ptr %arr.data.gep78, align 8
  %arr.elem80 = getelementptr i64, ptr %arr.data79, i64 0
  %arr.get = load i64, ptr %arr.elem80, align 8
  %strbuf81 = alloca i8, i64 32, align 1
  %105 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf81, ptr @int.fmt, i64 %arr.get)
  %len182 = call i64 @strlen(ptr @str.const.70)
  %len283 = call i64 @strlen(ptr %strbuf81)
  %totallen84 = add i64 %len182, %len283
  %bufsize85 = add i64 %totallen84, 1
  %concat.buf86 = call ptr @malloc(i64 %bufsize85)
  %106 = call ptr @memcpy(ptr %concat.buf86, ptr @str.const.70, i64 %len182)
  %dest287 = getelementptr i8, ptr %concat.buf86, i64 %len182
  %copylen288 = add i64 %len283, 1
  %107 = call ptr @memcpy(ptr %dest287, ptr %strbuf81, i64 %copylen288)
  %108 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf86)
  %arr.ptr.int89 = load i64, ptr %"%\D8\A3", align 8
  %arr.ptr90 = inttoptr i64 %arr.ptr.int89 to ptr
  %arr.data.gep91 = getelementptr inbounds %SadArray, ptr %arr.ptr90, i32 0, i32 2
  %arr.data92 = load ptr, ptr %arr.data.gep91, align 8
  %arr.elem93 = getelementptr i64, ptr %arr.data92, i64 4
  %arr.get94 = load i64, ptr %arr.elem93, align 8
  %strbuf95 = alloca i8, i64 32, align 1
  %109 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf95, ptr @int.fmt, i64 %arr.get94)
  %len196 = call i64 @strlen(ptr @str.const.73)
  %len297 = call i64 @strlen(ptr %strbuf95)
  %totallen98 = add i64 %len196, %len297
  %bufsize99 = add i64 %totallen98, 1
  %concat.buf100 = call ptr @malloc(i64 %bufsize99)
  %110 = call ptr @memcpy(ptr %concat.buf100, ptr @str.const.73, i64 %len196)
  %dest2101 = getelementptr i8, ptr %concat.buf100, i64 %len196
  %copylen2102 = add i64 %len297, 1
  %111 = call ptr @memcpy(ptr %dest2101, ptr %strbuf95, i64 %copylen2102)
  %112 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf100)
  %arr.ptr.int103 = load i64, ptr %"%\D8\A3", align 8
  %arr.ptr104 = inttoptr i64 %arr.ptr.int103 to ptr
  %arr.data.gep105 = getelementptr inbounds %SadArray, ptr %arr.ptr104, i32 0, i32 2
  %arr.data106 = load ptr, ptr %arr.data.gep105, align 8
  %arr.elem107 = getelementptr i64, ptr %arr.data106, i64 2
  store i64 99, ptr %arr.elem107, align 8
  %arr.ptr.int108 = load i64, ptr %"%\D8\A3", align 8
  %arr.ptr109 = inttoptr i64 %arr.ptr.int108 to ptr
  %arr.data.gep110 = getelementptr inbounds %SadArray, ptr %arr.ptr109, i32 0, i32 2
  %arr.data111 = load ptr, ptr %arr.data.gep110, align 8
  %arr.elem112 = getelementptr i64, ptr %arr.data111, i64 2
  %arr.get113 = load i64, ptr %arr.elem112, align 8
  %strbuf114 = alloca i8, i64 32, align 1
  %113 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf114, ptr @int.fmt, i64 %arr.get113)
  %len1115 = call i64 @strlen(ptr @str.const.76)
  %len2116 = call i64 @strlen(ptr %strbuf114)
  %totallen117 = add i64 %len1115, %len2116
  %bufsize118 = add i64 %totallen117, 1
  %concat.buf119 = call ptr @malloc(i64 %bufsize118)
  %114 = call ptr @memcpy(ptr %concat.buf119, ptr @str.const.76, i64 %len1115)
  %dest2120 = getelementptr i8, ptr %concat.buf119, i64 %len1115
  %copylen2121 = add i64 %len2116, 1
  %115 = call ptr @memcpy(ptr %dest2120, ptr %strbuf114, i64 %copylen2121)
  %116 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf119)
  %117 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.79)
  store i64 0, ptr %"%\D9\85\D8\AC", align 8
  %arr_new122 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep123 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 0
  store i64 5, ptr %arr.len.gep123, align 8
  %arr.cap.gep124 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 1
  store i64 5, ptr %arr.cap.gep124, align 8
  %arr.data125 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 5))
  %arr.data.gep126 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 2
  store ptr %arr.data125, ptr %arr.data.gep126, align 8
  %arr.data.gep127 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 2
  %arr.data128 = load ptr, ptr %arr.data.gep127, align 8
  %arr.elem129 = getelementptr i64, ptr %arr.data128, i64 0
  store i64 1, ptr %arr.elem129, align 8
  %arr.data.gep130 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 2
  %arr.data131 = load ptr, ptr %arr.data.gep130, align 8
  %arr.elem132 = getelementptr i64, ptr %arr.data131, i64 1
  store i64 2, ptr %arr.elem132, align 8
  %arr.data.gep133 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 2
  %arr.data134 = load ptr, ptr %arr.data.gep133, align 8
  %arr.elem135 = getelementptr i64, ptr %arr.data134, i64 2
  store i64 3, ptr %arr.elem135, align 8
  %arr.data.gep136 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 2
  %arr.data137 = load ptr, ptr %arr.data.gep136, align 8
  %arr.elem138 = getelementptr i64, ptr %arr.data137, i64 3
  store i64 4, ptr %arr.elem138, align 8
  %arr.data.gep139 = getelementptr inbounds %SadArray, ptr %arr_new122, i32 0, i32 2
  %arr.data140 = load ptr, ptr %arr.data.gep139, align 8
  %arr.elem141 = getelementptr i64, ptr %arr.data140, i64 4
  store i64 5, ptr %arr.elem141, align 8
  store i64 0, ptr %"%_foreach_idx_20", align 8
  br label %foreach_cond_20

while_exit_48:                                    ; preds = %while_cond_46
  %118 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.147)
  %calltmp270 = call i64 @"\D9\81\D9\8A\D8\A8\D9\88"(i64 10)
  %strbuf271 = alloca i8, i64 32, align 1
  %119 = call i32 (ptr, ptr, ...) @sprintf(ptr %strbuf271, ptr @int.fmt, i64 %calltmp270)
  %len1272 = call i64 @strlen(ptr @str.const.149)
  %len2273 = call i64 @strlen(ptr %strbuf271)
  %totallen274 = add i64 %len1272, %len2273
  %bufsize275 = add i64 %totallen274, 1
  %concat.buf276 = call ptr @malloc(i64 %bufsize275)
  %120 = call ptr @memcpy(ptr %concat.buf276, ptr @str.const.149, i64 %len1272)
  %dest2277 = getelementptr i8, ptr %concat.buf276, i64 %len1272
  %copylen2278 = add i64 %len2273, 1
  %121 = call ptr @memcpy(ptr %dest2277, ptr %strbuf271, i64 %copylen2278)
  %122 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr %concat.buf276)
  %123 = call i32 (ptr, ...) @printf(ptr @fmt.s, ptr @str.const.152)
  ret void

while_exit_51:                                    ; preds = %while_cond_49
  %"%\D8\B91.load283" = load i64, ptr %"%\D8\B91", align 8
  %addtmp284 = add i64 %"%\D8\B91.load283", 1
  store i64 %addtmp284, ptr %"%\D8\B91", align 8
  br label %while_cond_46
}

declare i64 @strlen(ptr)

declare ptr @malloc(i64)

declare ptr @memcpy(ptr, ptr, i64)

declare i32 @sprintf(ptr, ptr, ...)

declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
