; ModuleID = 'ذاكرة_نحلة'
source_filename = "\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

@str.const = private unnamed_addr constant [121 x i8] c"\E2\95\94\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\97\00", align 1
@str.const.1 = private unnamed_addr constant [65 x i8] c"\E2\95\91    \F0\9F\90\9D \D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA \D8\B0\D8\A7\D9\83\D8\B1\D8\A9 \D8\A7\D9\84\D9\86\D8\AD\D9\84\D8\A9         \E2\95\91\00", align 1
@str.const.2 = private unnamed_addr constant [121 x i8] c"\E2\95\A0\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\A3\00", align 1
@str.const.3 = private unnamed_addr constant [121 x i8] c"\E2\95\9A\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\9D\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_mem_read64(i64)

declare void @sad_ll_mem_write64(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_read_cr3()

declare void @sad_ll_write_cr3(i64)

declare void @sad_ll_invlpg(i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_interrupt_disable()

declare void @sad_ll_interrupt_enable()

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_body_1:                                     ; preds = %while_cond_0
  store i64 0, ptr %"%\D8\AD\D8\B1\D9\81", align 8
  %"%\D8\B7.load2" = load i64, ptr %"%\D8\B7", align 8
  %addtmp = add i64 %"%\D8\B7.load2", 1
  store i64 %addtmp, ptr %"%\D8\B7", align 8
  br label %while_cond_0

while_cond_0:                                     ; preds = %while_body_1, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", 200
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret void
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D9\86" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

merge_4:                                          ; preds = %entry
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 20, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 20, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 20))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep2 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data3 = load ptr, ptr %arr.data.gep2, align 8
  %arr.elem = getelementptr i64, ptr %arr.data3, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep4 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data5 = load ptr, ptr %arr.data.gep4, align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 1
  store i64 0, ptr %arr.elem6, align 8
  %arr.data.gep7 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data8 = load ptr, ptr %arr.data.gep7, align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 2
  store i64 0, ptr %arr.elem9, align 8
  %arr.data.gep10 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data11 = load ptr, ptr %arr.data.gep10, align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 3
  store i64 0, ptr %arr.elem12, align 8
  %arr.data.gep13 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data14 = load ptr, ptr %arr.data.gep13, align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 4
  store i64 0, ptr %arr.elem15, align 8
  %arr.data.gep16 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data17 = load ptr, ptr %arr.data.gep16, align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 5
  store i64 0, ptr %arr.elem18, align 8
  %arr.data.gep19 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data20 = load ptr, ptr %arr.data.gep19, align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 6
  store i64 0, ptr %arr.elem21, align 8
  %arr.data.gep22 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data23 = load ptr, ptr %arr.data.gep22, align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 7
  store i64 0, ptr %arr.elem24, align 8
  %arr.data.gep25 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data26 = load ptr, ptr %arr.data.gep25, align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 8
  store i64 0, ptr %arr.elem27, align 8
  %arr.data.gep28 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data29 = load ptr, ptr %arr.data.gep28, align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 9
  store i64 0, ptr %arr.elem30, align 8
  %arr.data.gep31 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data32 = load ptr, ptr %arr.data.gep31, align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 10
  store i64 0, ptr %arr.elem33, align 8
  %arr.data.gep34 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data35 = load ptr, ptr %arr.data.gep34, align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 11
  store i64 0, ptr %arr.elem36, align 8
  %arr.data.gep37 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data38 = load ptr, ptr %arr.data.gep37, align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 12
  store i64 0, ptr %arr.elem39, align 8
  %arr.data.gep40 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data41 = load ptr, ptr %arr.data.gep40, align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 13
  store i64 0, ptr %arr.elem42, align 8
  %arr.data.gep43 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data44 = load ptr, ptr %arr.data.gep43, align 8
  %arr.elem45 = getelementptr i64, ptr %arr.data44, i64 14
  store i64 0, ptr %arr.elem45, align 8
  %arr.data.gep46 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data47 = load ptr, ptr %arr.data.gep46, align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 15
  store i64 0, ptr %arr.elem48, align 8
  %arr.data.gep49 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data50 = load ptr, ptr %arr.data.gep49, align 8
  %arr.elem51 = getelementptr i64, ptr %arr.data50, i64 16
  store i64 0, ptr %arr.elem51, align 8
  %arr.data.gep52 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data53 = load ptr, ptr %arr.data.gep52, align 8
  %arr.elem54 = getelementptr i64, ptr %arr.data53, i64 17
  store i64 0, ptr %arr.elem54, align 8
  %arr.data.gep55 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data56 = load ptr, ptr %arr.data.gep55, align 8
  %arr.elem57 = getelementptr i64, ptr %arr.data56, i64 18
  store i64 0, ptr %arr.elem57, align 8
  %arr.data.gep58 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data59 = load ptr, ptr %arr.data.gep58, align 8
  %arr.elem60 = getelementptr i64, ptr %arr.data59, i64 19
  store i64 0, ptr %arr.elem60, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load61" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  store i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load61", ptr %"%\D9\82", align 8
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

merge_6:                                          ; preds = %then_5, %merge_4
  br label %while_cond_7

then_3:                                           ; preds = %entry
  ret void

then_5:                                           ; preds = %merge_4
  %"%\D9\82.load62" = load i64, ptr %"%\D9\82", align 8
  %subtmp = sub i64 0, %"%\D9\82.load62"
  store i64 %subtmp, ptr %"%\D9\82", align 8
  br label %merge_6

while_body_11:                                    ; preds = %while_cond_10
  %"%\D9\86.load73" = load i64, ptr %"%\D9\86", align 8
  %subtmp74 = sub i64 %"%\D9\86.load73", 1
  store i64 %subtmp74, ptr %"%\D9\86", align 8
  br label %while_cond_10

while_body_8:                                     ; preds = %while_cond_7
  %"%\D9\82.load64" = load i64, ptr %"%\D9\82", align 8
  %modtmp = srem i64 %"%\D9\82.load64", 10
  %addtmp = add i64 %modtmp, 48
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep65 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data66 = load ptr, ptr %arr.data.gep65, align 8
  %arr.elem67 = getelementptr i64, ptr %arr.data66, i64 %"%\D8\B9\D8\AF\D8\AF.load"
  store i64 %addtmp, ptr %arr.elem67, align 8
  %"%\D8\B9\D8\AF\D8\AF.load68" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %addtmp69 = add i64 %"%\D8\B9\D8\AF\D8\AF.load68", 1
  store i64 %addtmp69, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %"%\D9\82.load70" = load i64, ptr %"%\D9\82", align 8
  %divtmp = sdiv i64 %"%\D9\82.load70", 10
  store i64 %divtmp, ptr %"%\D9\82", align 8
  br label %while_cond_7

while_cond_10:                                    ; preds = %while_body_11, %while_exit_9
  %"%\D9\86.load" = load i64, ptr %"%\D9\86", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\86.load", 0
  br i1 %cmpgetmp, label %while_body_11, label %while_exit_12

while_cond_7:                                     ; preds = %while_body_8, %merge_6
  %"%\D9\82.load63" = load i64, ptr %"%\D9\82", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\82.load63", 0
  br i1 %cmpgttmp, label %while_body_8, label %while_exit_9

while_exit_12:                                    ; preds = %while_cond_10
  ret void

while_exit_9:                                     ; preds = %while_cond_7
  %"%\D8\B9\D8\AF\D8\AF.load71" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %subtmp72 = sub i64 %"%\D8\B9\D8\AF\D8\AF.load71", 1
  store i64 %subtmp72, ptr %"%\D9\86", align 8
  br label %while_cond_10
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A"(i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\AD" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85" = alloca i64, align 8
  %"%\D9\86\D8\B5\D9\81_\D8\A8\D8\A7\D9\8A\D8\AA" = alloca i64, align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_hex" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 16, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep2 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data3 = load ptr, ptr %arr.data.gep2, align 8
  %arr.elem = getelementptr i64, ptr %arr.data3, i64 0
  store i64 48, ptr %arr.elem, align 8
  %arr.data.gep4 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data5 = load ptr, ptr %arr.data.gep4, align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 1
  store i64 49, ptr %arr.elem6, align 8
  %arr.data.gep7 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data8 = load ptr, ptr %arr.data.gep7, align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 2
  store i64 50, ptr %arr.elem9, align 8
  %arr.data.gep10 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data11 = load ptr, ptr %arr.data.gep10, align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 3
  store i64 51, ptr %arr.elem12, align 8
  %arr.data.gep13 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data14 = load ptr, ptr %arr.data.gep13, align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 4
  store i64 52, ptr %arr.elem15, align 8
  %arr.data.gep16 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data17 = load ptr, ptr %arr.data.gep16, align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 5
  store i64 53, ptr %arr.elem18, align 8
  %arr.data.gep19 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data20 = load ptr, ptr %arr.data.gep19, align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 6
  store i64 54, ptr %arr.elem21, align 8
  %arr.data.gep22 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data23 = load ptr, ptr %arr.data.gep22, align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 7
  store i64 55, ptr %arr.elem24, align 8
  %arr.data.gep25 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data26 = load ptr, ptr %arr.data.gep25, align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 8
  store i64 56, ptr %arr.elem27, align 8
  %arr.data.gep28 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data29 = load ptr, ptr %arr.data.gep28, align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 9
  store i64 57, ptr %arr.elem30, align 8
  %arr.data.gep31 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data32 = load ptr, ptr %arr.data.gep31, align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 10
  store i64 65, ptr %arr.elem33, align 8
  %arr.data.gep34 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data35 = load ptr, ptr %arr.data.gep34, align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 11
  store i64 66, ptr %arr.elem36, align 8
  %arr.data.gep37 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data38 = load ptr, ptr %arr.data.gep37, align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 12
  store i64 67, ptr %arr.elem39, align 8
  %arr.data.gep40 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data41 = load ptr, ptr %arr.data.gep40, align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 13
  store i64 68, ptr %arr.elem42, align 8
  %arr.data.gep43 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data44 = load ptr, ptr %arr.data.gep43, align 8
  %arr.elem45 = getelementptr i64, ptr %arr.data44, i64 14
  store i64 69, ptr %arr.elem45, align 8
  %arr.data.gep46 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data47 = load ptr, ptr %arr.data.gep46, align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 15
  store i64 70, ptr %arr.elem48, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\A3\D8\B1\D9\82\D8\A7\D9\85_hex", align 8
  store i64 15, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9", align 8
  br label %while_cond_13

while_body_14:                                    ; preds = %while_cond_13
  %"%107.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %andtmp = and i64 %"%107.load", 15
  store i64 %andtmp, ptr %"%\D9\86\D8\B5\D9\81_\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  store i64 1, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D8\AD", align 8
  br label %while_cond_16

while_body_17:                                    ; preds = %while_cond_16
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %shltmp = shl i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load", 4
  store i64 %shltmp, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %"%\D8\AD.load50" = load i64, ptr %"%\D8\AD", align 8
  %addtmp = add i64 %"%\D8\AD.load50", 1
  store i64 %addtmp, ptr %"%\D8\AD", align 8
  br label %while_cond_16

while_cond_13:                                    ; preds = %while_exit_18, %entry
  %"%\D8\AE\D8\A7\D9\86\D8\A9.load" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AE\D8\A7\D9\86\D8\A9.load", 0
  br i1 %cmpgetmp, label %while_body_14, label %while_exit_15

while_cond_16:                                    ; preds = %while_body_17, %while_body_14
  %"%\D8\AD.load" = load i64, ptr %"%\D8\AD", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A9.load49" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AD.load", %"%\D8\AE\D8\A7\D9\86\D8\A9.load49"
  br i1 %cmplttmp, label %while_body_17, label %while_exit_18

while_exit_15:                                    ; preds = %while_cond_13
  ret void

while_exit_18:                                    ; preds = %while_cond_16
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load51" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load", %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load51"
  %andtmp52 = and i64 %divtmp, 15
  store i64 %andtmp52, ptr %"%\D9\86\D8\B5\D9\81_\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  %"%\D8\AE\D8\A7\D9\86\D8\A9.load53" = load i64, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9", align 8
  %subtmp = sub i64 %"%\D8\AE\D8\A7\D9\86\D8\A9.load53", 1
  store i64 %subtmp, ptr %"%\D8\AE\D8\A7\D9\86\D8\A9", align 8
  br label %while_cond_13
}

define void @"\D8\B9\D9\8A\D9\86_\D8\A8\D8\AA"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9") {
entry:
  %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load", 6
  store i64 %shrtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %andtmp = and i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load2", 63
  store i64 %andtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %shltmp = shl i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load", 3
  %addtmp = add i64 0, %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_read64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_19

merge_23:                                         ; preds = %then_22, %while_exit_21
  ret void

then_22:                                          ; preds = %while_exit_21
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load8" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load9" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %addtmp10 = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load8", %"%\D9\82\D9\86\D8\A7\D8\B9.load9"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load11" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_write64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load11", i64 %addtmp10)
  br label %merge_23

while_body_20:                                    ; preds = %while_cond_19
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp3 = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp3, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D9\8A.load4" = load i64, ptr %"%\D9\8A", align 8
  %addtmp5 = add i64 %"%\D9\8A.load4", 1
  store i64 %addtmp5, ptr %"%\D9\8A", align 8
  br label %while_cond_19

while_cond_19:                                    ; preds = %while_body_20, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load"
  br i1 %cmplttmp, label %while_body_20, label %while_exit_21

while_exit_21:                                    ; preds = %while_cond_19
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load6" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load6"
  %andtmp7 = and i64 %divtmp, 1
  store i64 %andtmp7, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A.load", 0
  br i1 %cmpeqtmp, label %then_22, label %merge_23
}

define void @"\D8\A7\D9\85\D8\B3\D8\AD_\D8\A8\D8\AA"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9") {
entry:
  %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load", 6
  store i64 %shrtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %andtmp = and i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load2", 63
  store i64 %andtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %shltmp = shl i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load", 3
  %addtmp = add i64 0, %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_read64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_24

merge_28:                                         ; preds = %then_27, %while_exit_26
  ret void

then_27:                                          ; preds = %while_exit_26
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load8" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load9" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %subtmp = sub i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load8", %"%\D9\82\D9\86\D8\A7\D8\B9.load9"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load10" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_write64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load10", i64 %subtmp)
  br label %merge_28

while_body_25:                                    ; preds = %while_cond_24
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp3 = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp3, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D9\8A.load4" = load i64, ptr %"%\D9\8A", align 8
  %addtmp5 = add i64 %"%\D9\8A.load4", 1
  store i64 %addtmp5, ptr %"%\D9\8A", align 8
  br label %while_cond_24

while_cond_24:                                    ; preds = %while_body_25, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load"
  br i1 %cmplttmp, label %while_body_25, label %while_exit_26

while_exit_26:                                    ; preds = %while_cond_24
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load6" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load6"
  %andtmp7 = and i64 %divtmp, 1
  store i64 %andtmp7, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A.load", 1
  br i1 %cmpeqtmp, label %then_27, label %merge_28
}

define i64 @"\D8\A7\D9\81\D8\AD\D8\B5_\D8\A8\D8\AA"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9") {
entry:
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load", 6
  store i64 %shrtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %andtmp = and i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load2", 63
  store i64 %andtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %shltmp = shl i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load", 3
  %addtmp = add i64 0, %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_read64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_29

while_body_30:                                    ; preds = %while_cond_29
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp3 = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp3, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D9\8A.load4" = load i64, ptr %"%\D9\8A", align 8
  %addtmp5 = add i64 %"%\D9\8A.load4", 1
  store i64 %addtmp5, ptr %"%\D9\8A", align 8
  br label %while_cond_29

while_cond_29:                                    ; preds = %while_body_30, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load"
  br i1 %cmplttmp, label %while_body_30, label %while_exit_31

while_exit_31:                                    ; preds = %while_cond_29
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load6" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load6"
  %andtmp7 = and i64 %divtmp, 1
  ret i64 %andtmp7
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A5\D9\84\D9\89_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %divtmp = sdiv i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 0
  ret i64 %divtmp
}

define i64 @"\D8\B5\D9\81\D8\AD\D8\A9_\D8\A5\D9\84\D9\89_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64 %"\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A91", align 8
  %multmp = mul i64 %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9.load", 0
  ret i64 %multmp
}

define i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A9"() {
entry:
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA" = alloca i64, align 8
  br i1 true, label %then_32, label %merge_33

merge_33:                                         ; preds = %entry
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA", align 8
  br i1 false, label %then_34, label %merge_35

merge_35:                                         ; preds = %then_34, %merge_33
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_36

merge_40:                                         ; preds = %while_exit_43, %while_body_37
  %"%\D9\83.load4" = load i64, ptr %"%\D9\83", align 8
  %addtmp5 = add i64 %"%\D9\83.load4", 1
  store i64 %addtmp5, ptr %"%\D9\83", align 8
  br label %while_cond_36

merge_48:                                         ; preds = %merge_50, %while_exit_46
  %"%\D8\A8\D8\AA.load19" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %addtmp20 = add i64 %"%\D8\A8\D8\AA.load19", 1
  store i64 %addtmp20, ptr %"%\D8\A8\D8\AA", align 8
  br label %while_cond_41

merge_50:                                         ; preds = %then_47
  br label %merge_48

then_32:                                          ; preds = %entry
  ret i64 0

then_34:                                          ; preds = %merge_33
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA", align 8
  br label %merge_35

then_39:                                          ; preds = %while_body_37
  store i64 0, ptr %"%\D8\A8\D8\AA", align 8
  br label %while_cond_41

then_47:                                          ; preds = %while_exit_46
  %"%\D9\83.load14" = load i64, ptr %"%\D9\83", align 8
  %shltmp15 = shl i64 %"%\D9\83.load14", 6
  %"%\D8\A8\D8\AA.load16" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %addtmp17 = add i64 %shltmp15, %"%\D8\A8\D8\AA.load16"
  store i64 %addtmp17, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %cmplttmp18 = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load", 0
  br i1 %cmplttmp18, label %then_49, label %merge_50

then_49:                                          ; preds = %then_47
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load21" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  call void @"\D8\B9\D9\8A\D9\86_\D8\A8\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load21")
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load22" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %calltmp = call i64 @"\D8\B5\D9\81\D8\AD\D8\A9_\D8\A5\D9\84\D9\89_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load22")
  ret i64 %calltmp

while_body_37:                                    ; preds = %while_cond_36
  %"%\D9\83.load2" = load i64, ptr %"%\D9\83", align 8
  %shltmp = shl i64 %"%\D9\83.load2", 3
  %addtmp3 = add i64 0, %shltmp
  store i64 %addtmp3, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_read64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load", -1
  br i1 %cmpnetmp, label %then_39, label %merge_40

while_body_42:                                    ; preds = %while_cond_41
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_44

while_body_45:                                    ; preds = %while_cond_44
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp9 = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp9, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D9\8A.load10" = load i64, ptr %"%\D9\8A", align 8
  %addtmp11 = add i64 %"%\D9\8A.load10", 1
  store i64 %addtmp11, ptr %"%\D9\8A", align 8
  br label %while_cond_44

while_cond_36:                                    ; preds = %merge_40, %merge_35
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA.load1" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A7\D8\AA.load1"
  br i1 %cmplttmp, label %while_body_37, label %while_exit_38

while_cond_41:                                    ; preds = %merge_48, %then_39
  %"%\D8\A8\D8\AA.load" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %cmplttmp6 = icmp slt i64 %"%\D8\A8\D8\AA.load", 64
  br i1 %cmplttmp6, label %while_body_42, label %while_exit_43

while_cond_44:                                    ; preds = %while_body_45, %while_body_42
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %"%\D8\A8\D8\AA.load7" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %cmplttmp8 = icmp slt i64 %"%\D9\8A.load", %"%\D8\A8\D8\AA.load7"
  br i1 %cmplttmp8, label %while_body_45, label %while_exit_46

while_exit_38:                                    ; preds = %while_cond_36
  ret i64 0

while_exit_43:                                    ; preds = %while_cond_41
  br label %merge_40

while_exit_46:                                    ; preds = %while_cond_44
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load12" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load13" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load12", %"%\D9\82\D9\86\D8\A7\D8\B9.load13"
  %andtmp = and i64 %divtmp, 1
  store i64 %andtmp, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A8\D8\AA_\D8\AD\D8\A7\D9\84\D9\8A.load", 0
  br i1 %cmpeqtmp, label %then_47, label %merge_48
}

define void @"\D8\AD\D8\B1\D8\B1_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 0
  br i1 %cmpeqtmp, label %then_51, label %merge_52

merge_52:                                         ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load2" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %modtmp = srem i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load2", 0
  %cmpnetmp = icmp ne i64 %modtmp, 0
  br i1 %cmpnetmp, label %then_53, label %merge_54

merge_54:                                         ; preds = %merge_52
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A5\D9\84\D9\89_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3")
  store i64 %calltmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load", 0
  br i1 %cmpgetmp, label %then_55, label %merge_56

merge_56:                                         ; preds = %merge_54
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load4" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %calltmp5 = call i64 @"\D8\A7\D9\81\D8\AD\D8\B5_\D8\A8\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load4")
  %cmpeqtmp6 = icmp eq i64 %calltmp5, 0
  br i1 %cmpeqtmp6, label %then_57, label %merge_58

merge_58:                                         ; preds = %merge_56
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load7" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  call void @"\D8\A7\D9\85\D8\B3\D8\AD_\D8\A8\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load7")
  ret void

then_51:                                          ; preds = %entry
  ret void

then_53:                                          ; preds = %merge_52
  ret void

then_55:                                          ; preds = %merge_54
  ret void

then_57:                                          ; preds = %merge_56
  ret void
}

define i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA"(i64 %"\D8\B9\D8\AF\D8\AF") {
entry:
  %"%\D8\AA" = alloca i64, align 8
  %"%\D8\B5" = alloca i64, align 8
  %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF1" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF", ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF.load", 0
  br i1 %cmpeqtmp, label %then_59, label %merge_60

else_70:                                          ; preds = %while_body_66
  store i64 0, ptr %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  br label %merge_69

merge_60:                                         ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF.load2" = load i64, ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF.load2", 1
  br i1 %cmpeqtmp3, label %then_61, label %merge_62

merge_62:                                         ; preds = %merge_60
  %"%\D8\B9\D8\AF\D8\AF.load4" = load i64, ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF.load4", 0
  br i1 %cmpgttmp, label %then_63, label %merge_64

merge_64:                                         ; preds = %merge_62
  store i64 0, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%\D8\B5", align 8
  br label %while_cond_65

merge_69:                                         ; preds = %merge_74, %else_70
  %"%\D8\B5.load9" = load i64, ptr %"%\D8\B5", align 8
  %addtmp = add i64 %"%\D8\B5.load9", 1
  store i64 %addtmp, ptr %"%\D8\B5", align 8
  br label %while_cond_65

merge_72:                                         ; preds = %then_71, %then_68
  %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load11" = load i64, ptr %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %addtmp12 = add i64 %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load11", 1
  store i64 %addtmp12, ptr %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load13" = load i64, ptr %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D8\B9\D8\AF\D8\AF.load14" = load i64, ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load13", %"%\D8\B9\D8\AF\D8\AF.load14"
  br i1 %cmpgetmp, label %then_73, label %merge_74

merge_74:                                         ; preds = %merge_72
  br label %merge_69

then_59:                                          ; preds = %entry
  ret i64 0

then_61:                                          ; preds = %merge_60
  %calltmp = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A9"()
  ret i64 %calltmp

then_63:                                          ; preds = %merge_62
  ret i64 0

then_68:                                          ; preds = %while_body_66
  %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\85\D8\AA\D8\AA\D8\A7\D9\84\D9\8A\D8\A9.load", 0
  br i1 %cmpeqtmp8, label %then_71, label %merge_72

then_71:                                          ; preds = %then_68
  %"%\D8\B5.load10" = load i64, ptr %"%\D8\B5", align 8
  store i64 %"%\D8\B5.load10", ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  br label %merge_72

then_73:                                          ; preds = %merge_72
  store i64 0, ptr %"%\D8\AA", align 8
  br label %while_cond_75

while_body_66:                                    ; preds = %while_cond_65
  %"%\D8\B5.load5" = load i64, ptr %"%\D8\B5", align 8
  %calltmp6 = call i64 @"\D8\A7\D9\81\D8\AD\D8\B5_\D8\A8\D8\AA"(i64 %"%\D8\B5.load5")
  %cmpeqtmp7 = icmp eq i64 %calltmp6, 0
  br i1 %cmpeqtmp7, label %then_68, label %else_70

while_body_76:                                    ; preds = %while_cond_75
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %"%\D8\AA.load17" = load i64, ptr %"%\D8\AA", align 8
  %addtmp18 = add i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", %"%\D8\AA.load17"
  call void @"\D8\B9\D9\8A\D9\86_\D8\A8\D8\AA"(i64 %addtmp18)
  %"%\D8\AA.load19" = load i64, ptr %"%\D8\AA", align 8
  %addtmp20 = add i64 %"%\D8\AA.load19", 1
  store i64 %addtmp20, ptr %"%\D8\AA", align 8
  br label %while_cond_75

while_cond_65:                                    ; preds = %merge_69, %merge_64
  %"%\D8\B5.load" = load i64, ptr %"%\D8\B5", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B5.load", 0
  br i1 %cmplttmp, label %while_body_66, label %while_exit_67

while_cond_75:                                    ; preds = %while_body_76, %then_73
  %"%\D8\AA.load" = load i64, ptr %"%\D8\AA", align 8
  %"%\D8\B9\D8\AF\D8\AF.load15" = load i64, ptr %"\D8\B9\D8\AF\D8\AF1", align 8
  %cmplttmp16 = icmp slt i64 %"%\D8\AA.load", %"%\D8\B9\D8\AF\D8\AF.load15"
  br i1 %cmplttmp16, label %while_body_76, label %while_exit_77

while_exit_67:                                    ; preds = %while_cond_65
  ret i64 0

while_exit_77:                                    ; preds = %while_cond_75
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load21" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %calltmp22 = call i64 @"\D8\B5\D9\81\D8\AD\D8\A9_\D8\A5\D9\84\D9\89_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load21")
  ret i64 %calltmp22
}

define void @"\D8\AD\D8\B1\D8\B1_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", i64 %"\D8\B9\D8\AF\D8\AF") {
entry:
  %"%\D8\AA" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF2" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF", ptr %"\D8\B9\D8\AF\D8\AF2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 0
  br i1 %cmpeqtmp, label %then_78, label %merge_79

merge_79:                                         ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF2", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF.load", 0
  br i1 %cmpeqtmp3, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A5\D9\84\D9\89_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4")
  store i64 %calltmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9", align 8
  store i64 0, ptr %"%\D8\AA", align 8
  br label %while_cond_82

merge_86:                                         ; preds = %then_85, %while_body_83
  %"%\D8\AA.load11" = load i64, ptr %"%\D8\AA", align 8
  %addtmp12 = add i64 %"%\D8\AA.load11", 1
  store i64 %addtmp12, ptr %"%\D8\AA", align 8
  br label %while_cond_82

then_78:                                          ; preds = %entry
  ret void

then_80:                                          ; preds = %merge_79
  ret void

then_85:                                          ; preds = %while_body_83
  %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9.load8" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %"%\D8\AA.load9" = load i64, ptr %"%\D8\AA", align 8
  %addtmp10 = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9.load8", %"%\D8\AA.load9"
  call void @"\D8\A7\D9\85\D8\B3\D8\AD_\D8\A8\D8\AA"(i64 %addtmp10)
  br label %merge_86

while_body_83:                                    ; preds = %while_cond_82
  %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %"%\D8\AA.load6" = load i64, ptr %"%\D8\AA", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84_\D8\B5\D9\81\D8\AD\D8\A9.load", %"%\D8\AA.load6"
  %cmplttmp7 = icmp slt i64 %addtmp, 0
  br i1 %cmplttmp7, label %then_85, label %merge_86

while_cond_82:                                    ; preds = %merge_86, %merge_81
  %"%\D8\AA.load" = load i64, ptr %"%\D8\AA", align 8
  %"%\D8\B9\D8\AF\D8\AF.load5" = load i64, ptr %"\D8\B9\D8\AF\D8\AF2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AA.load", %"%\D8\B9\D8\AF\D8\AF.load5"
  br i1 %cmplttmp, label %while_body_83, label %while_exit_84

while_exit_84:                                    ; preds = %while_cond_82
  ret void
}

define i64 @"\D8\AD\D8\AC\D9\85_\D9\85\D8\B3\D8\AA\D9\88\D9\89"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"%\D9\8A" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85" = alloca i64, align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  store i64 1, ptr %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_87

while_body_88:                                    ; preds = %while_cond_87
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85", align 8
  %shltmp = shl i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85.load", 1
  store i64 %shltmp, ptr %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85", align 8
  %"%\D9\8A.load2" = load i64, ptr %"%\D9\8A", align 8
  %addtmp = add i64 %"%\D9\8A.load2", 1
  store i64 %addtmp, ptr %"%\D9\8A", align 8
  br label %while_cond_87

while_cond_87:                                    ; preds = %while_body_88, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load"
  br i1 %cmplttmp, label %while_body_88, label %while_exit_89

while_exit_89:                                    ; preds = %while_cond_87
  %loadtmp = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA" = alloca i64, align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\892" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A91", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %calltmp = call i64 @"\D8\AD\D8\AC\D9\85_\D9\85\D8\B3\D8\AA\D9\88\D9\89"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load")
  %multmp = mul i64 %calltmp, 0
  store i64 %multmp, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A91", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  %divtmp = sdiv i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load", %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load"
  store i64 %divtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", align 8
  %andtmp = and i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_90, label %else_92

else_92:                                          ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load5" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A91", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load6" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  %subtmp = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load5", %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load6"
  ret i64 %subtmp

then_90:                                          ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load3" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A91", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load4" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load3", %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load4"
  ret i64 %addtmp
}

define void @"\D8\A3\D8\B6\D9\81_\D9\83\D8\AA\D9\84\D8\A9_\D8\AD\D8\B1\D8\A9"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\862" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load", 0
  br i1 %cmpeqtmp, label %then_93, label %merge_94

merge_100:                                        ; preds = %then_99, %then_97
  br label %merge_98

merge_102:                                        ; preds = %merge_104, %merge_98
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load10" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load10", 3
  br i1 %cmpeqtmp11, label %then_105, label %merge_106

merge_104:                                        ; preds = %then_103, %then_101
  br label %merge_102

merge_106:                                        ; preds = %merge_108, %merge_102
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load15" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp16 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load15", 4
  br i1 %cmpeqtmp16, label %then_109, label %merge_110

merge_108:                                        ; preds = %then_107, %then_105
  br label %merge_106

merge_110:                                        ; preds = %merge_112, %merge_106
  ret void

merge_112:                                        ; preds = %then_111, %then_109
  br label %merge_110

merge_94:                                         ; preds = %merge_96, %entry
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load3" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load3", 1
  br i1 %cmpeqtmp4, label %then_97, label %merge_98

merge_96:                                         ; preds = %then_95, %then_93
  br label %merge_94

merge_98:                                         ; preds = %merge_100, %merge_94
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load5" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load5", 2
  br i1 %cmpeqtmp6, label %then_101, label %merge_102

then_101:                                         ; preds = %merge_98
  br i1 true, label %then_103, label %merge_104

then_103:                                         ; preds = %then_101
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load12" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 0
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load12", ptr %arr.elem14, align 8
  br label %merge_104

then_105:                                         ; preds = %merge_102
  br i1 true, label %then_107, label %merge_108

then_107:                                         ; preds = %then_105
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load17" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 0
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load17", ptr %arr.elem19, align 8
  br label %merge_108

then_109:                                         ; preds = %merge_106
  br i1 true, label %then_111, label %merge_112

then_111:                                         ; preds = %then_109
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load20" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 0
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load20", ptr %arr.elem22, align 8
  br label %merge_112

then_93:                                          ; preds = %entry
  br i1 true, label %then_95, label %merge_96

then_95:                                          ; preds = %then_93
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", ptr %arr.elem, align 8
  br label %merge_96

then_97:                                          ; preds = %merge_94
  br i1 true, label %then_99, label %merge_100

then_99:                                          ; preds = %then_97
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load7" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\862", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 0
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load7", ptr %arr.elem9, align 8
  br label %merge_100
}

define i64 @"\D8\A7\D8\B3\D8\AD\D8\A8_\D9\83\D8\AA\D9\84\D8\A9_\D8\AD\D8\B1\D8\A9"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\8633" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load", 0
  br i1 %cmpeqtmp, label %then_113, label %merge_114

merge_114:                                        ; preds = %merge_116, %entry
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load2" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load2", 1
  br i1 %cmpeqtmp3, label %then_117, label %merge_118

merge_116:                                        ; preds = %then_113
  br label %merge_114

merge_118:                                        ; preds = %merge_120, %merge_114
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load6" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load6", 2
  br i1 %cmpeqtmp7, label %then_121, label %merge_122

merge_120:                                        ; preds = %then_117
  br label %merge_118

merge_122:                                        ; preds = %merge_124, %merge_118
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load14" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load14", 3
  br i1 %cmpeqtmp15, label %then_125, label %merge_126

merge_124:                                        ; preds = %then_121
  br label %merge_122

merge_126:                                        ; preds = %merge_128, %merge_122
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load22" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpeqtmp23 = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load22", 4
  br i1 %cmpeqtmp23, label %then_129, label %merge_130

merge_128:                                        ; preds = %then_125
  br label %merge_126

merge_130:                                        ; preds = %merge_132, %merge_126
  ret i64 0

merge_132:                                        ; preds = %then_129
  br label %merge_130

then_113:                                         ; preds = %entry
  br i1 false, label %then_115, label %merge_116

then_115:                                         ; preds = %then_113
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 0
  store i64 0, ptr %arr.elem5, align 8
  %loadtmp = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 %loadtmp

then_117:                                         ; preds = %merge_114
  br i1 false, label %then_119, label %merge_120

then_119:                                         ; preds = %then_117
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 0
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  store i64 %arr.get10, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 0
  store i64 0, ptr %arr.elem12, align 8
  %loadtmp13 = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 %loadtmp13

then_121:                                         ; preds = %merge_118
  br i1 false, label %then_123, label %merge_124

then_123:                                         ; preds = %then_121
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 0
  %arr.get18 = load i64, ptr %arr.elem17, align 8
  store i64 %arr.get18, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 0
  store i64 0, ptr %arr.elem20, align 8
  %loadtmp21 = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 %loadtmp21

then_125:                                         ; preds = %merge_122
  br i1 false, label %then_127, label %merge_128

then_127:                                         ; preds = %then_125
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 0
  %arr.get26 = load i64, ptr %arr.elem25, align 8
  store i64 %arr.get26, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 0
  store i64 0, ptr %arr.elem28, align 8
  %loadtmp29 = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 %loadtmp29

then_129:                                         ; preds = %merge_126
  br i1 false, label %then_131, label %merge_132

then_131:                                         ; preds = %then_129
  %arr.data30 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 0
  %arr.get32 = load i64, ptr %arr.elem31, align 8
  store i64 %arr.get32, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\8633", align 8
  %arr.data34 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 0
  store i64 0, ptr %arr.elem35, align 8
  %loadtmp36 = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\8633", align 8
  ret i64 %loadtmp36
}

define i64 @"\D8\AE\D8\B5\D8\B5_buddy"(i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"%\D8\A7\D9\84\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\86\D8\B5\D9\81" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\891" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %calltmp = call i64 @"\D8\A7\D8\B3\D8\AD\D8\A8_\D9\83\D8\AA\D9\84\D8\A9_\D8\AD\D8\B1\D8\A9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load")
  store i64 %calltmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %cmpnetmp = icmp ne i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 0
  br i1 %cmpnetmp, label %then_133, label %merge_134

merge_134:                                        ; preds = %entry
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load2" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load2", 4
  br i1 %cmpgetmp, label %then_135, label %merge_136

merge_136:                                        ; preds = %merge_134
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load3" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %addtmp = add i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load3", 1
  %calltmp4 = call i64 @"\D8\AE\D8\B5\D8\B5_buddy"(i64 %addtmp)
  store i64 %calltmp4, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_137, label %merge_138

merge_138:                                        ; preds = %merge_136
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load5" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %calltmp6 = call i64 @"\D8\AD\D8\AC\D9\85_\D9\85\D8\B3\D8\AA\D9\88\D9\89"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load5")
  %multmp = mul i64 %calltmp6, 0
  store i64 %multmp, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\86\D8\B5\D9\81", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9.load7" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\86\D8\B5\D9\81.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\86\D8\B5\D9\81", align 8
  %addtmp8 = add i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9.load7", %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\86\D8\B5\D9\81.load"
  store i64 %addtmp8, ptr %"%\D8\A7\D9\84\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load9" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\891", align 8
  %"%\D8\A7\D9\84\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A.load" = load i64, ptr %"%\D8\A7\D9\84\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A", align 8
  call void @"\D8\A3\D8\B6\D9\81_\D9\83\D8\AA\D9\84\D8\A9_\D8\AD\D8\B1\D8\A9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load9", i64 %"%\D8\A7\D9\84\D9\86\D8\B5\D9\81_\D8\A7\D9\84\D8\AB\D8\A7\D9\86\D9\8A.load")
  %loadtmp10 = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9", align 8
  ret i64 %loadtmp10

then_133:                                         ; preds = %entry
  %loadtmp = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret i64 %loadtmp

then_135:                                         ; preds = %merge_134
  ret i64 0

then_137:                                         ; preds = %merge_136
  ret i64 0
}

define void @"\D8\AD\D8\B1\D8\B1_buddy"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89") {
entry:
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D8\AF\D9\85\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\B3" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82" = alloca i64, align 8
  %"\D9\85\D8\B3\D8\AA\D9\88\D9\892" = alloca i64, align 8
  store i64 %"\D9\85\D8\B3\D8\AA\D9\88\D9\89", ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 0
  br i1 %cmpeqtmp, label %then_139, label %merge_140

merge_140:                                        ; preds = %entry
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load", 4
  br i1 %cmplttmp, label %then_141, label %merge_142

merge_142:                                        ; preds = %merge_144, %merge_140
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load7" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load8" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  call void @"\D8\A3\D8\B6\D9\81_\D9\83\D8\AA\D9\84\D8\A9_\D8\AD\D8\B1\D8\A9"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load7", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load8")
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load9" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %calltmp10 = call i64 @"\D8\AD\D8\AC\D9\85_\D9\85\D8\B3\D8\AA\D9\88\D9\89"(i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load9")
  store i64 %calltmp10, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load11" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %calltmp12 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A5\D9\84\D9\89_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load11")
  store i64 %calltmp12, ptr %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84", align 8
  store i64 0, ptr %"%\D8\B3", align 8
  br label %while_cond_149

merge_144:                                        ; preds = %merge_146, %then_141
  br label %merge_142

merge_146:                                        ; preds = %then_143
  br label %merge_144

merge_148:                                        ; preds = %then_147, %then_145
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load21" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %addtmp = add i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load21", 1
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D8\AF\D9\85\D8\AC\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D8\AF\D9\85\D8\AC\D8\A9", align 8
  call void @"\D8\AD\D8\B1\D8\B1_buddy"(i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D8\AF\D9\85\D8\AC\D8\A9.load", i64 %addtmp)
  ret void

merge_153:                                        ; preds = %then_152, %while_body_150
  %"%\D8\B3.load29" = load i64, ptr %"%\D8\B3", align 8
  %addtmp30 = add i64 %"%\D8\B3.load29", 1
  store i64 %addtmp30, ptr %"%\D8\B3", align 8
  br label %while_cond_149

then_139:                                         ; preds = %entry
  ret void

then_141:                                         ; preds = %merge_140
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load4" = load i64, ptr %"\D9\85\D8\B3\D8\AA\D9\88\D9\892", align 8
  %calltmp = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load3", i64 %"%\D9\85\D8\B3\D8\AA\D9\88\D9\89.load4")
  store i64 %calltmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82", align 8
  %calltmp5 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A5\D9\84\D9\89_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82.load")
  store i64 %calltmp5, ptr %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82", align 8
  %cmplttmp6 = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82.load", 0
  br i1 %cmplttmp6, label %then_143, label %merge_144

then_143:                                         ; preds = %then_141
  %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82.load13" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82", align 8
  %calltmp14 = call i64 @"\D8\A7\D9\81\D8\AD\D8\B5_\D8\A8\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D8\AF\D9\8A\D9\82.load13")
  %cmpeqtmp15 = icmp eq i64 %calltmp14, 0
  br i1 %cmpeqtmp15, label %then_145, label %merge_146

then_145:                                         ; preds = %then_143
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load16" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load16", ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D8\AF\D9\85\D8\AC\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82.load17" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load18" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %cmplttmp19 = icmp slt i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82.load17", %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load18"
  br i1 %cmplttmp19, label %then_147, label %merge_148

then_147:                                         ; preds = %then_145
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82.load20" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82", align 8
  store i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B5\D8\AF\D9\8A\D9\82.load20", ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D8\AF\D9\85\D8\AC\D8\A9", align 8
  br label %merge_148

then_152:                                         ; preds = %while_body_150
  %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84.load26" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84", align 8
  %"%\D8\B3.load27" = load i64, ptr %"%\D8\B3", align 8
  %addtmp28 = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84.load26", %"%\D8\B3.load27"
  call void @"\D8\A7\D9\85\D8\B3\D8\AD_\D8\A8\D8\AA"(i64 %addtmp28)
  br label %merge_153

while_body_150:                                   ; preds = %while_cond_149
  %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84", align 8
  %"%\D8\B3.load23" = load i64, ptr %"%\D8\B3", align 8
  %addtmp24 = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A3\D9\88\D9\84.load", %"%\D8\B3.load23"
  %cmplttmp25 = icmp slt i64 %addtmp24, 0
  br i1 %cmplttmp25, label %then_152, label %merge_153

while_cond_149:                                   ; preds = %merge_153, %merge_142
  %"%\D8\B3.load" = load i64, ptr %"%\D8\B3", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", align 8
  %cmplttmp22 = icmp slt i64 %"%\D8\B3.load", %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load"
  br i1 %cmplttmp22, label %while_body_150, label %while_exit_151

while_exit_151:                                   ; preds = %while_cond_149
  ret void
}

define i64 @"\D9\81\D9\87\D8\B1\D8\B3_PML4"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  store i64 1, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_154

while_body_155:                                   ; preds = %while_cond_154
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %shltmp = shl i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load", 1
  store i64 %shltmp, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %"%\D9\8A.load2" = load i64, ptr %"%\D9\8A", align 8
  %addtmp = add i64 %"%\D9\8A.load2", 1
  store i64 %addtmp, ptr %"%\D9\8A", align 8
  br label %while_cond_154

while_cond_154:                                   ; preds = %while_body_155, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", 39
  br i1 %cmplttmp, label %while_body_155, label %while_exit_156

while_exit_156:                                   ; preds = %while_cond_154
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %divtmp = sdiv i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3"
  %andtmp = and i64 %divtmp, 511
  ret i64 %andtmp
}

define i64 @"\D9\81\D9\87\D8\B1\D8\B3_PDPT"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  store i64 1, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_157

while_body_158:                                   ; preds = %while_cond_157
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %shltmp = shl i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load", 1
  store i64 %shltmp, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %"%\D9\8A.load2" = load i64, ptr %"%\D9\8A", align 8
  %addtmp = add i64 %"%\D9\8A.load2", 1
  store i64 %addtmp, ptr %"%\D9\8A", align 8
  br label %while_cond_157

while_cond_157:                                   ; preds = %while_body_158, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", 30
  br i1 %cmplttmp, label %while_body_158, label %while_exit_159

while_exit_159:                                   ; preds = %while_cond_157
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %divtmp = sdiv i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3"
  %andtmp = and i64 %divtmp, 511
  ret i64 %andtmp
}

define i64 @"\D9\81\D9\87\D8\B1\D8\B3_PD"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  store i64 1, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_160

while_body_161:                                   ; preds = %while_cond_160
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %shltmp = shl i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load", 1
  store i64 %shltmp, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %"%\D9\8A.load2" = load i64, ptr %"%\D9\8A", align 8
  %addtmp = add i64 %"%\D9\8A.load2", 1
  store i64 %addtmp, ptr %"%\D9\8A", align 8
  br label %while_cond_160

while_cond_160:                                   ; preds = %while_body_161, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", 21
  br i1 %cmplttmp, label %while_body_161, label %while_exit_162

while_exit_162:                                   ; preds = %while_cond_160
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %divtmp = sdiv i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3"
  %andtmp = and i64 %divtmp, 511
  ret i64 %andtmp
}

define i64 @"\D9\81\D9\87\D8\B1\D8\B3_PT"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"%\D9\8A" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  store i64 1, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  store i64 0, ptr %"%\D9\8A", align 8
  br label %while_cond_163

while_body_164:                                   ; preds = %while_cond_163
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %shltmp = shl i64 %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load", 1
  store i64 %shltmp, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %"%\D9\8A.load2" = load i64, ptr %"%\D9\8A", align 8
  %addtmp = add i64 %"%\D9\8A.load2", 1
  store i64 %addtmp, ptr %"%\D9\8A", align 8
  br label %while_cond_163

while_cond_163:                                   ; preds = %while_body_164, %entry
  %"%\D9\8A.load" = load i64, ptr %"%\D9\8A", align 8
  %cmplttmp = icmp slt i64 %"%\D9\8A.load", 12
  br i1 %cmplttmp, label %while_body_164, label %while_exit_165

while_exit_165:                                   ; preds = %while_cond_163
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3" = load i64, ptr %"%\D9\85\D9\82\D8\B3\D9\88\D9\85", align 8
  %divtmp = sdiv i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", %"%\D9\85\D9\82\D8\B3\D9\88\D9\85.load3"
  %andtmp = and i64 %divtmp, 511
  ret i64 %andtmp
}

define i64 @"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\861" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\861", align 8
  %modtmp = srem i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 0
  ret i64 %modtmp
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84", i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B32" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B32", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\841" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\841", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B32", align 8
  %shltmp = shl i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 3
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\841", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84.load", %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  call void @sad_ll_mem_read64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load")
  ret i64 0
}

define void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84", i64 %"\D9\81\D9\87\D8\B1\D8\B3", i64 %"\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A93" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D9\82\D9\8A\D9\85\D8\A93", align 8
  %"\D9\81\D9\87\D8\B1\D8\B32" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B32", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\841" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\841", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B32", align 8
  %shltmp = shl i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 3
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\841", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84.load", %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A93", align 8
  call void @sad_ll_mem_write64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84.load", i64 %"%\D9\82\D9\8A\D9\85\D8\A9.load")
  ret void
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"\D9\85\D8\AF\D8\AE\D9\84") {
entry:
  %"%\D8\A8\D9\84\D8\A7_\D8\A3\D8\B9\D9\84\D8\A7\D9\85" = alloca i64, align 8
  %"\D9\85\D8\AF\D8\AE\D9\841" = alloca i64, align 8
  store i64 %"\D9\85\D8\AF\D8\AE\D9\84", ptr %"\D9\85\D8\AF\D8\AE\D9\841", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84.load" = load i64, ptr %"\D9\85\D8\AF\D8\AE\D9\841", align 8
  %shrtmp = lshr i64 %"%\D9\85\D8\AF\D8\AE\D9\84.load", 12
  %shltmp = shl i64 %shrtmp, 12
  store i64 %shltmp, ptr %"%\D8\A8\D9\84\D8\A7_\D8\A3\D8\B9\D9\84\D8\A7\D9\85", align 8
  %"%\D8\A8\D9\84\D8\A7_\D8\A3\D8\B9\D9\84\D8\A7\D9\85.load" = load i64, ptr %"%\D8\A8\D9\84\D8\A7_\D8\A3\D8\B9\D9\84\D8\A7\D9\85", align 8
  %andtmp = and i64 %"%\D8\A8\D9\84\D8\A7_\D8\A3\D8\B9\D9\84\D8\A7\D9\85.load", 4503599627370495
  ret i64 %andtmp
}

define void @"\D8\B9\D9\8A\D9\86_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A", i64 %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4" = alloca i64, align 8
  %"%\D9\81_pt" = alloca i64, align 8
  %"%\D9\81_pd" = alloca i64, align 8
  %"%\D9\81_pdpt" = alloca i64, align 8
  %"%\D9\81_pml4" = alloca i64, align 8
  %"\D8\A3\D8\B9\D9\84\D8\A7\D9\853" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85", ptr %"\D8\A3\D8\B9\D9\84\D8\A7\D9\853", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A2" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  br i1 true, label %then_166, label %merge_167

else_170:                                         ; preds = %merge_167
  %calltmp12 = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A9"()
  store i64 %calltmp12, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load", 0
  br i1 %cmpeqtmp13, label %then_171, label %merge_172

else_175:                                         ; preds = %merge_169
  %calltmp20 = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A9"()
  store i64 %calltmp20, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %cmpeqtmp21 = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load", 0
  br i1 %cmpeqtmp21, label %then_176, label %merge_177

else_180:                                         ; preds = %merge_174
  %calltmp31 = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A9"()
  store i64 %calltmp31, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %cmpeqtmp32 = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load", 0
  br i1 %cmpeqtmp32, label %then_181, label %merge_182

merge_167:                                        ; preds = %then_166, %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PML4"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load")
  store i64 %calltmp, ptr %"%\D9\81_pml4", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load4" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp5 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PDPT"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load4")
  store i64 %calltmp5, ptr %"%\D9\81_pdpt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load6" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp7 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PD"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load6")
  store i64 %calltmp7, ptr %"%\D9\81_pd", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load8" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp9 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PT"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load8")
  store i64 %calltmp9, ptr %"%\D9\81_pt", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pml4", align 8
  %andtmp = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_168, label %else_170

merge_169:                                        ; preds = %merge_172, %then_168
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load14" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D9\81_pdpt.load" = load i64, ptr %"%\D9\81_pdpt", align 8
  %calltmp15 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load14", i64 %"%\D9\81_pdpt.load")
  store i64 %calltmp15, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %andtmp16 = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load", 1
  %cmpeqtmp17 = icmp eq i64 %andtmp16, 1
  br i1 %cmpeqtmp17, label %then_173, label %else_175

merge_172:                                        ; preds = %else_170
  br label %merge_169

merge_174:                                        ; preds = %merge_177, %then_173
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load22" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D9\81_pd.load" = load i64, ptr %"%\D9\81_pd", align 8
  %calltmp23 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load22", i64 %"%\D9\81_pd.load")
  store i64 %calltmp23, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %andtmp24 = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load", 1
  %cmpeqtmp25 = icmp eq i64 %andtmp24, 1
  br i1 %cmpeqtmp25, label %then_178, label %else_180

merge_177:                                        ; preds = %else_175
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load26" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load26", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load27" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D9\81_pdpt.load28" = load i64, ptr %"%\D9\81_pdpt", align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load27", i64 %"%\D9\81_pdpt.load28", i64 %addtmp)
  br label %merge_174

merge_179:                                        ; preds = %merge_182, %then_178
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A2", align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85.load" = load i64, ptr %"\D8\A3\D8\B9\D9\84\D8\A7\D9\853", align 8
  %addtmp33 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A.load", %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85.load"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load34" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %"%\D9\81_pt.load" = load i64, ptr %"%\D9\81_pt", align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load34", i64 %"%\D9\81_pt.load", i64 %addtmp33)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load35" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  call void @sad_ll_invlpg(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load35")
  ret void

merge_182:                                        ; preds = %else_180
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load36" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %addtmp37 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load36", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load38" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D9\81_pd.load39" = load i64, ptr %"%\D9\81_pd", align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load38", i64 %"%\D9\81_pd.load39", i64 %addtmp37)
  br label %merge_179

then_166:                                         ; preds = %entry
  br label %merge_167

then_168:                                         ; preds = %merge_167
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load10" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pml4", align 8
  %calltmp11 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load10")
  store i64 %calltmp11, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  br label %merge_169

then_171:                                         ; preds = %else_170
  ret void

then_173:                                         ; preds = %merge_169
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load18" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %calltmp19 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load18")
  store i64 %calltmp19, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  br label %merge_174

then_176:                                         ; preds = %else_175
  ret void

then_178:                                         ; preds = %merge_174
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load29" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %calltmp30 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load29")
  store i64 %calltmp30, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  br label %merge_179

then_181:                                         ; preds = %else_180
  ret void
}

define void @"\D8\A3\D9\84\D8\BA_\D8\AA\D8\B9\D9\8A\D9\8A\D9\86_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4" = alloca i64, align 8
  %"%\D9\81_pt" = alloca i64, align 8
  %"%\D9\81_pd" = alloca i64, align 8
  %"%\D9\81_pdpt" = alloca i64, align 8
  %"%\D9\81_pml4" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  br i1 true, label %then_183, label %merge_184

merge_184:                                        ; preds = %then_183, %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PML4"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load")
  store i64 %calltmp, ptr %"%\D9\81_pml4", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load2" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp3 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PDPT"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load2")
  store i64 %calltmp3, ptr %"%\D9\81_pdpt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load4" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp5 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PD"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load4")
  store i64 %calltmp5, ptr %"%\D9\81_pd", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load6" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp7 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PT"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load6")
  store i64 %calltmp7, ptr %"%\D9\81_pt", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pml4", align 8
  %andtmp = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_185, label %merge_186

merge_186:                                        ; preds = %merge_184
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load8" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pml4", align 8
  %calltmp9 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load8")
  store i64 %calltmp9, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D9\81_pdpt.load" = load i64, ptr %"%\D9\81_pdpt", align 8
  %calltmp10 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load", i64 %"%\D9\81_pdpt.load")
  store i64 %calltmp10, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %andtmp11 = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load", 1
  %cmpeqtmp12 = icmp eq i64 %andtmp11, 0
  br i1 %cmpeqtmp12, label %then_187, label %merge_188

merge_188:                                        ; preds = %merge_186
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load13" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %calltmp14 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load13")
  store i64 %calltmp14, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D9\81_pd.load" = load i64, ptr %"%\D9\81_pd", align 8
  %calltmp15 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load", i64 %"%\D9\81_pd.load")
  store i64 %calltmp15, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %andtmp16 = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load", 1
  %cmpeqtmp17 = icmp eq i64 %andtmp16, 0
  br i1 %cmpeqtmp17, label %then_189, label %merge_190

merge_190:                                        ; preds = %merge_188
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load18" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %calltmp19 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load18")
  store i64 %calltmp19, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %"%\D9\81_pt.load" = load i64, ptr %"%\D9\81_pt", align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load", i64 %"%\D9\81_pt.load", i64 0)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load20" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  call void @sad_ll_invlpg(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load20")
  ret void

then_183:                                         ; preds = %entry
  br label %merge_184

then_185:                                         ; preds = %merge_184
  ret void

then_187:                                         ; preds = %merge_186
  ret void

then_189:                                         ; preds = %merge_188
  ret void
}

define i64 @"\D8\AA\D8\B1\D8\AC\D9\85_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4" = alloca i64, align 8
  %"%\D9\81_pt" = alloca i64, align 8
  %"%\D9\81_pd" = alloca i64, align 8
  %"%\D9\81_pdpt" = alloca i64, align 8
  %"%\D9\81_pml4" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  br i1 true, label %then_191, label %merge_192

merge_192:                                        ; preds = %then_191, %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PML4"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load")
  store i64 %calltmp, ptr %"%\D9\81_pml4", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load2" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp3 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PDPT"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load2")
  store i64 %calltmp3, ptr %"%\D9\81_pdpt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load4" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp5 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PD"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load4")
  store i64 %calltmp5, ptr %"%\D9\81_pd", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load6" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp7 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PT"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load6")
  store i64 %calltmp7, ptr %"%\D9\81_pt", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pml4", align 8
  %andtmp = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_193, label %merge_194

merge_194:                                        ; preds = %merge_192
  %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load8" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pml4", align 8
  %calltmp9 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pml4.load8")
  store i64 %calltmp9, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt", align 8
  %"%\D9\81_pdpt.load" = load i64, ptr %"%\D9\81_pdpt", align 8
  %calltmp10 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pdpt.load", i64 %"%\D9\81_pdpt.load")
  store i64 %calltmp10, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %andtmp11 = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load", 1
  %cmpeqtmp12 = icmp eq i64 %andtmp11, 0
  br i1 %cmpeqtmp12, label %then_195, label %merge_196

merge_196:                                        ; preds = %merge_194
  %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load13" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt", align 8
  %calltmp14 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pdpt.load13")
  store i64 %calltmp14, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd", align 8
  %"%\D9\81_pd.load" = load i64, ptr %"%\D9\81_pd", align 8
  %calltmp15 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pd.load", i64 %"%\D9\81_pd.load")
  store i64 %calltmp15, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %andtmp16 = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load", 1
  %cmpeqtmp17 = icmp eq i64 %andtmp16, 0
  br i1 %cmpeqtmp17, label %then_197, label %merge_198

merge_198:                                        ; preds = %merge_196
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load18" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %shrtmp = lshr i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load18", 7
  %andtmp19 = and i64 %shrtmp, 1
  %cmpeqtmp20 = icmp eq i64 %andtmp19, 1
  br i1 %cmpeqtmp20, label %then_199, label %merge_200

merge_200:                                        ; preds = %merge_198
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load24" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %calltmp25 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load24")
  store i64 %calltmp25, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt", align 8
  %"%\D9\81_pt.load" = load i64, ptr %"%\D9\81_pt", align 8
  %calltmp26 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AF\D8\AE\D9\84_\D8\AC\D8\AF\D9\88\D9\84"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_pt.load", i64 %"%\D9\81_pt.load")
  store i64 %calltmp26, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pt", align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84_pt.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pt", align 8
  %andtmp27 = and i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pt.load", 1
  %cmpeqtmp28 = icmp eq i64 %andtmp27, 0
  br i1 %cmpeqtmp28, label %then_201, label %merge_202

merge_202:                                        ; preds = %merge_200
  %"%\D9\85\D8\AF\D8\AE\D9\84_pt.load29" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pt", align 8
  %calltmp30 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pt.load29")
  store i64 %calltmp30, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load31" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %calltmp32 = call i64 @"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load31")
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %addtmp33 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9.load", %calltmp32
  ret i64 %addtmp33

then_191:                                         ; preds = %entry
  br label %merge_192

then_193:                                         ; preds = %merge_192
  ret i64 0

then_195:                                         ; preds = %merge_194
  ret i64 0

then_197:                                         ; preds = %merge_196
  ret i64 0

then_199:                                         ; preds = %merge_198
  %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load21" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84_pd", align 8
  %calltmp22 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86_\D9\85\D8\AF\D8\AE\D9\84"(i64 %"%\D9\85\D8\AF\D8\AE\D9\84_pd.load21")
  store i64 %calltmp22, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load23" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %modtmp = srem i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load23", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load", %modtmp
  ret i64 %addtmp

then_201:                                         ; preds = %merge_200
  ret i64 0
}

define void @"\D8\A3\D8\B6\D9\81_\D9\85\D9\86\D8\B7\D9\82\D8\A9_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9", i64 %"\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9", i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9") {
entry:
  %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A93" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9", ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A93", align 8
  %"\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A92" = alloca i64, align 8
  store i64 %"\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9", ptr %"\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A92", align 8
  %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9", ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A91", align 8
  br i1 true, label %then_203, label %merge_204

else_207:                                         ; preds = %merge_204
  br label %merge_206

merge_204:                                        ; preds = %entry
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load" = load i64, ptr %"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A91", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load", ptr %arr.elem, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load" = load i64, ptr %"\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A92", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 0
  store i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load", ptr %arr.elem5, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A93", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 0
  store i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load", ptr %arr.elem7, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load8" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A93", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load8", 0
  br i1 %cmpeqtmp, label %then_205, label %else_207

merge_206:                                        ; preds = %else_207, %then_205
  ret void

then_203:                                         ; preds = %entry
  ret void

then_205:                                         ; preds = %merge_204
  br label %merge_206
}

define void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"() {
entry:
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B5" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\B5\D9\81\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D8\AB" = alloca i64, align 8
  %"%\D9\83\D9\84\D9\85\D8\A7\D8\AA" = alloca i64, align 8
  store i64 0, ptr %"%\D9\83\D9\84\D9\85\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\AB", align 8
  br label %while_cond_208

merge_215:                                        ; preds = %while_exit_218, %while_body_212
  %"%\D9\85.load15" = load i64, ptr %"%\D9\85", align 8
  %addtmp16 = add i64 %"%\D9\85.load15", 1
  store i64 %addtmp16, ptr %"%\D9\85", align 8
  br label %while_cond_211

merge_220:                                        ; preds = %then_219, %while_body_217
  %"%\D8\B5.load22" = load i64, ptr %"%\D8\B5", align 8
  %addtmp23 = add i64 %"%\D8\B5.load22", 1
  store i64 %addtmp23, ptr %"%\D8\B5", align 8
  br label %while_cond_216

then_214:                                         ; preds = %while_body_212
  %"%\D9\85.load6" = load i64, ptr %"%\D9\85", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\85.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %divtmp = sdiv i64 %arr.get9, 0
  store i64 %divtmp, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %"%\D9\85.load10" = load i64, ptr %"%\D9\85", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\85.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  %divtmp14 = sdiv i64 %arr.get13, 0
  store i64 %divtmp14, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9", align 8
  store i64 0, ptr %"%\D8\B5", align 8
  br label %while_cond_216

then_219:                                         ; preds = %while_body_217
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load21" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  call void @"\D8\A7\D9\85\D8\B3\D8\AD_\D8\A8\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load21")
  br label %merge_220

while_body_209:                                   ; preds = %while_cond_208
  %"%\D8\AB.load1" = load i64, ptr %"%\D8\AB", align 8
  %shltmp = shl i64 %"%\D8\AB.load1", 3
  %addtmp = add i64 0, %shltmp
  call void @sad_ll_mem_write64(i64 %addtmp, i64 -1)
  %"%\D8\AB.load2" = load i64, ptr %"%\D8\AB", align 8
  %addtmp3 = add i64 %"%\D8\AB.load2", 1
  store i64 %addtmp3, ptr %"%\D8\AB", align 8
  br label %while_cond_208

while_body_212:                                   ; preds = %while_cond_211
  %"%\D9\85.load5" = load i64, ptr %"%\D9\85", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\85.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_214, label %merge_215

while_body_217:                                   ; preds = %while_cond_216
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\B5\D9\81\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\B5\D9\81\D8\AD\D8\A9", align 8
  %"%\D8\B5.load18" = load i64, ptr %"%\D8\B5", align 8
  %addtmp19 = add i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\B5\D9\81\D8\AD\D8\A9.load", %"%\D8\B5.load18"
  store i64 %addtmp19, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %cmplttmp20 = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", 0
  br i1 %cmplttmp20, label %then_219, label %merge_220

while_cond_208:                                   ; preds = %while_body_209, %entry
  %"%\D8\AB.load" = load i64, ptr %"%\D8\AB", align 8
  %"%\D9\83\D9\84\D9\85\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\83\D9\84\D9\85\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AB.load", %"%\D9\83\D9\84\D9\85\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_209, label %while_exit_210

while_cond_211:                                   ; preds = %merge_215, %while_exit_210
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp4 = icmp slt i64 %"%\D9\85.load", 0
  br i1 %cmplttmp4, label %while_body_212, label %while_exit_213

while_cond_216:                                   ; preds = %merge_220, %then_214
  %"%\D8\B5.load" = load i64, ptr %"%\D8\B5", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9", align 8
  %cmplttmp17 = icmp slt i64 %"%\D8\B5.load", %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D8\A9.load"
  br i1 %cmplttmp17, label %while_body_217, label %while_exit_218

while_exit_210:                                   ; preds = %while_cond_208
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_211

while_exit_213:                                   ; preds = %while_cond_211
  ret void

while_exit_218:                                   ; preds = %while_cond_216
  br label %merge_215
}

define void @"\D8\AD\D9\85_\D9\85\D9\86\D8\B7\D9\82\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", i64 %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9") {
entry:
  %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%\D8\B7" = alloca i64, align 8
  %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A93" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", ptr %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A93", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  store i64 0, ptr %"%\D8\B7", align 8
  br label %while_cond_221

merge_225:                                        ; preds = %then_224, %while_body_222
  %"%\D8\B7.load7" = load i64, ptr %"%\D8\B7", align 8
  %addtmp8 = add i64 %"%\D8\B7.load7", 1
  store i64 %addtmp8, ptr %"%\D8\B7", align 8
  br label %while_cond_221

then_224:                                         ; preds = %while_body_222
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load5" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A.load6" = load i64, ptr %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A", align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load" = load i64, ptr %"\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A93", align 8
  call void @"\D8\B9\D9\8A\D9\86_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load5", i64 %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A.load6", i64 %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load")
  br label %merge_225

while_body_222:                                   ; preds = %while_cond_221
  %"%\D8\B7.load4" = load i64, ptr %"%\D8\B7", align 8
  %multmp = mul i64 %"%\D8\B7.load4", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", %multmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %calltmp = call i64 @"\D8\AA\D8\B1\D8\AC\D9\85_\D8\B9\D9\86\D9\88\D8\A7\D9\86"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load")
  store i64 %calltmp, ptr %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A", align 8
  %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A.load" = load i64, ptr %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A.load", 0
  br i1 %cmpnetmp, label %then_224, label %merge_225

while_cond_221:                                   ; preds = %merge_225, %entry
  %"%\D8\B7.load" = load i64, ptr %"%\D8\B7", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B7.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_222, label %while_exit_223

while_exit_223:                                   ; preds = %while_cond_221
  ret void
}

define void @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\B5\D9\81\D8\AD\D8\A9_\D8\AD\D8\A7\D8\B1\D8\B3\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A1", align 8
  call void @"\D8\A3\D9\84\D8\BA_\D8\AA\D8\B9\D9\8A\D9\8A\D9\86_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A.load")
  ret void
}

define i64 @"\D8\A3\D9\86\D8\B4\D8\A6_\D9\85\D9\83\D8\AF\D8\B3"(i64 %"\D8\AD\D8\AC\D9\85_\D8\A8\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"\D8\AD\D8\AC\D9\85_\D8\A8\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA1" = alloca i64, align 8
  store i64 %"\D8\AD\D8\AC\D9\85_\D8\A8\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", ptr %"\D8\AD\D8\AC\D9\85_\D8\A8\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA1", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A8\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load" = load i64, ptr %"\D8\AD\D8\AC\D9\85_\D8\A8\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA1", align 8
  %addtmp = add i64 %"%\D8\AD\D8\AC\D9\85_\D8\A8\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA.load", 1
  store i64 %addtmp, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A", align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A", align 8
  %calltmp = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA"(i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load")
  store i64 %calltmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load", 0
  br i1 %cmpeqtmp, label %then_226, label %merge_227

merge_227:                                        ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load2" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  call void @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\B5\D9\81\D8\AD\D8\A9_\D8\AD\D8\A7\D8\B1\D8\B3\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load2")
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load3" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A", align 8
  %multmp = mul i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A.load3", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  %addtmp5 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86.load4", %multmp
  ret i64 %addtmp5

then_226:                                         ; preds = %entry
  ret i64 0
}

define void @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_ram") {
entry:
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\AE" = alloca i64, align 8
  %"%\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9" = alloca i64, align 8
  %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_ram1" = alloca i64, align 8
  store i64 %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_ram", ptr %"\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_ram1", align 8
  br i1 false, label %then_228, label %merge_229

merge_229:                                        ; preds = %then_228, %entry
  br i1 poison, label %then_230, label %merge_231

merge_231:                                        ; preds = %then_230, %merge_229
  br i1 false, label %then_232, label %merge_233

merge_233:                                        ; preds = %then_232, %merge_231
  store i64 poison, ptr %"%\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", align 8
  store i64 0, ptr %"%\D8\AE", align 8
  br label %while_cond_234

merge_238:                                        ; preds = %merge_240, %while_body_235
  %"%\D8\AE.load5" = load i64, ptr %"%\D8\AE", align 8
  %addtmp6 = add i64 %"%\D8\AE.load5", 1
  store i64 %addtmp6, ptr %"%\D8\AE", align 8
  br label %while_cond_234

merge_240:                                        ; preds = %then_239, %then_237
  br label %merge_238

then_228:                                         ; preds = %entry
  br label %merge_229

then_230:                                         ; preds = %merge_229
  br label %merge_231

then_232:                                         ; preds = %merge_231
  call void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"()
  br label %merge_233

then_237:                                         ; preds = %while_body_235
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load4" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %calltmp = call i64 @"\D8\A7\D9\81\D8\AD\D8\B5_\D8\A8\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load4")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_239, label %merge_240

then_239:                                         ; preds = %then_237
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load7" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  call void @"\D8\B9\D9\8A\D9\86_\D8\A8\D8\AA"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load7")
  br label %merge_240

while_body_235:                                   ; preds = %while_cond_234
  %"%\D8\AE.load2" = load i64, ptr %"%\D8\AE", align 8
  %addtmp = add i64 0, %"%\D8\AE.load2"
  store i64 %addtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %cmplttmp3 = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", 0
  br i1 %cmplttmp3, label %then_237, label %merge_238

while_cond_234:                                   ; preds = %merge_238, %merge_233
  %"%\D8\AE.load" = load i64, ptr %"%\D8\AE", align 8
  %"%\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load" = load i64, ptr %"%\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AE.load", %"%\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load"
  br i1 %cmplttmp, label %while_body_235, label %while_exit_236

while_exit_236:                                   ; preds = %while_cond_234
  ret void
}

define void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"() {
entry:
  %"%\D9\85" = alloca i64, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9_MB" = alloca i64, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\AD\D8\B1\D8\A9_MB" = alloca i64, align 8
  %"%\D8\B0\D9\8A\D9\84" = alloca ptr, align 8
  %"%\D9\81\D8\A7\D8\B5\D9\84" = alloca ptr, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca ptr, align 8
  %"%\D8\B1\D8\A3\D8\B3" = alloca ptr, align 8
  store ptr @str.const, ptr %"%\D8\B1\D8\A3\D8\B3", align 8
  store ptr @str.const.1, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  store ptr @str.const.2, ptr %"%\D9\81\D8\A7\D8\B5\D9\84", align 8
  store ptr @str.const.3, ptr %"%\D8\B0\D9\8A\D9\84", align 8
  store i64 0, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\AD\D8\B1\D8\A9_MB", align 8
  store i64 0, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9_MB", align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\AD\D8\B1\D8\A9_MB.load" = load i64, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\AD\D8\B1\D8\A9_MB", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\AD\D8\B1\D8\A9_MB.load")
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9_MB.load" = load i64, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9_MB", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9_MB.load")
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_241

while_body_242:                                   ; preds = %while_cond_241
  %"%\D9\85.load1" = load i64, ptr %"%\D9\85", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\85.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\AA_\D8\B9\D8\B4\D8\B1\D9\8A"(i64 %arr.get)
  %"%\D9\85.load2" = load i64, ptr %"%\D9\85", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\85.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %shrtmp = lshr i64 %arr.get5, 20
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %shrtmp)
  %"%\D9\85.load6" = load i64, ptr %"%\D9\85", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\85.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get9)
  %"%\D9\85.load10" = load i64, ptr %"%\D9\85", align 8
  %addtmp = add i64 %"%\D9\85.load10", 1
  store i64 %addtmp, ptr %"%\D9\85", align 8
  br label %while_cond_241

while_cond_241:                                   ; preds = %while_body_242, %entry
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", 0
  br i1 %cmplttmp, label %while_body_242, label %while_exit_243

while_exit_243:                                   ; preds = %while_cond_241
  ret void
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B5" = alloca i64, align 8
  %"%\D8\A81" = alloca i64, align 8
  %"%\D8\B52" = alloca i64, align 8
  %"%\D8\B51" = alloca i64, align 8
  %calltmp = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A9"()
  store i64 %calltmp, ptr %"%\D8\B51", align 8
  %"%\D8\B51.load" = load i64, ptr %"%\D8\B51", align 8
  %cmpnetmp = icmp ne i64 %"%\D8\B51.load", 0
  br i1 %cmpnetmp, label %then_244, label %else_246

else_246:                                         ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_245

else_249:                                         ; preds = %merge_245
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_248

else_252:                                         ; preds = %merge_248
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_251

else_255:                                         ; preds = %merge_251
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_254

merge_245:                                        ; preds = %else_246, %then_244
  %calltmp2 = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA"(i64 4)
  store i64 %calltmp2, ptr %"%\D8\B52", align 8
  %"%\D8\B52.load" = load i64, ptr %"%\D8\B52", align 8
  %cmpnetmp3 = icmp ne i64 %"%\D8\B52.load", 0
  br i1 %cmpnetmp3, label %then_247, label %else_249

merge_248:                                        ; preds = %else_249, %then_247
  %calltmp5 = call i64 @"\D8\AE\D8\B5\D8\B5_buddy"(i64 2)
  store i64 %calltmp5, ptr %"%\D8\A81", align 8
  %"%\D8\A81.load" = load i64, ptr %"%\D8\A81", align 8
  %cmpnetmp6 = icmp ne i64 %"%\D8\A81.load", 0
  br i1 %cmpnetmp6, label %then_250, label %else_252

merge_251:                                        ; preds = %else_252, %then_250
  %calltmp8 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A5\D9\84\D9\89_\D8\B5\D9\81\D8\AD\D8\A9"(i64 8192)
  store i64 %calltmp8, ptr %"%\D8\B1\D9\82\D9\85_\D8\B5", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B5.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\B5", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\B5.load", 2
  br i1 %cmpeqtmp, label %then_253, label %else_255

merge_254:                                        ; preds = %else_255, %then_253
  %calltmp9 = call i64 @"\D9\81\D9\87\D8\B1\D8\B3_PT"(i64 305419896)
  store i64 %calltmp9, ptr %"%\D9\81", align 8
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\81.load")
  ret void

then_244:                                         ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  %"%\D8\B51.load1" = load i64, ptr %"%\D8\B51", align 8
  call void @"\D8\AD\D8\B1\D8\B1_\D8\B5\D9\81\D8\AD\D8\A9"(i64 %"%\D8\B51.load1")
  br label %merge_245

then_247:                                         ; preds = %merge_245
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  %"%\D8\B52.load4" = load i64, ptr %"%\D8\B52", align 8
  call void @"\D8\AD\D8\B1\D8\B1_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA"(i64 %"%\D8\B52.load4", i64 4)
  br label %merge_248

then_250:                                         ; preds = %merge_248
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  %"%\D8\A81.load7" = load i64, ptr %"%\D8\A81", align 8
  call void @"\D8\AD\D8\B1\D8\B1_buddy"(i64 %"%\D8\A81.load7", i64 2)
  br label %merge_251

then_253:                                         ; preds = %merge_251
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_254
}

define void @__sad_main() {
entry:
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\A7\D9\84\D9\85\D8\AD\D8\AC\D9\88\D8\B2\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D8\B7\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D8\AD\D8\AC\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D9\86\D8\A7\D8\B7\D9\82" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_PML4" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_buddy_4" = alloca i64, align 8
  %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_4" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_buddy_3" = alloca i64, align 8
  %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_3" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_buddy_2" = alloca i64, align 8
  %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_2" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_buddy_1" = alloca i64, align 8
  %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_1" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_buddy_0" = alloca i64, align 8
  %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_0" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84_\D9\84\D9\83\D9\84_\D9\85\D8\B3\D8\AA\D9\88\D9\89" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D8\AD\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B9\D9\8A\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_ACPI" = alloca i64, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\AD\D8\AC\D9\88\D8\B2\D8\A9" = alloca i64, align 8
  %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\88\D9\8A\D8\A7\D8\AA_buddy" = alloca i64, align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85" = alloca i64, align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\86\D9\88\D8\A7\D8\A9_\D9\83\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\86\D9\88\D8\A7\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D8\A8\D9\84\D8\A7_\D8\AA\D9\86\D9\81\D9\8A\D8\B0" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D8\B9\D9\85\D9\88\D9\85\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D8\AA\D9\85_\D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D8\AA\D9\85_\D8\A7\D9\84\D9\88\D8\B5\D9\88\D9\84" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D8\A8\D9\84\D8\A7_\D8\AA\D8\AE\D8\B2\D9\8A\D9\86" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\84\D9\85_\D9\85\D9\88\D8\AC\D9\88\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D9\83\D9\88\D9\85\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\83\D9\88\D9\85\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\86\D9\88\D8\A7\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\B9\D9\85\D9\84\D8\A7\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9" = alloca i64, align 8
  store i64 4096, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A9", align 8
  store i64 2097152, ptr %"%\D8\AD\D8\AC\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9", align 8
  store i64 1073741824, ptr %"%\D8\AD\D8\AC\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D8\B9\D9\85\D9\84\D8\A7\D9\82\D8\A9", align 8
  store i64 4294967296, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9", align 8
  store i64 1048576, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", align 8
  store i64 -140737488355328, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\86\D9\88\D8\A7\D8\A9", align 8
  store i64 -140733193388032, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\83\D9\88\D9\85\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9", align 8
  store i64 268435456, ptr %"%\D8\AD\D8\AC\D9\85_\D9\83\D9\88\D9\85\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9", align 8
  store i64 -140728898420736, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA", align 8
  store i64 512, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D8\A7\D9\84\D8\AC\D8\AF\D9\88\D9\84", align 8
  store i64 1, ptr %"%\D8\B9\D9\84\D9\85_\D9\85\D9\88\D8\AC\D9\88\D8\AF\D8\A9", align 8
  store i64 2, ptr %"%\D8\B9\D9\84\D9\85_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  store i64 4, ptr %"%\D8\B9\D9\84\D9\85_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85", align 8
  store i64 8, ptr %"%\D8\B9\D9\84\D9\85_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9", align 8
  store i64 16, ptr %"%\D8\B9\D9\84\D9\85_\D8\A8\D9\84\D8\A7_\D8\AA\D8\AE\D8\B2\D9\8A\D9\86", align 8
  store i64 32, ptr %"%\D8\B9\D9\84\D9\85_\D8\AA\D9\85_\D8\A7\D9\84\D9\88\D8\B5\D9\88\D9\84", align 8
  store i64 64, ptr %"%\D8\B9\D9\84\D9\85_\D8\AA\D9\85_\D8\A7\D9\84\D8\AA\D8\B9\D8\AF\D9\8A\D9\84", align 8
  store i64 128, ptr %"%\D8\B9\D9\84\D9\85_\D8\B5\D9\81\D8\AD\D8\A9_\D9\83\D8\A8\D9\8A\D8\B1\D8\A9", align 8
  store i64 256, ptr %"%\D8\B9\D9\84\D9\85_\D8\B9\D9\85\D9\88\D9\85\D9\8A", align 8
  store i64 -9223372036854775808, ptr %"%\D8\B9\D9\84\D9\85_\D8\A8\D9\84\D8\A7_\D8\AA\D9\86\D9\81\D9\8A\D8\B0", align 8
  store i64 3, ptr %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\86\D9\88\D8\A7\D8\A9_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  store i64 1, ptr %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\86\D9\88\D8\A7\D8\A9_\D9\83\D9\88\D8\AF", align 8
  store i64 7, ptr %"%\D8\A3\D8\B9\D9\84\D8\A7\D9\85_\D9\85\D8\B3\D8\AA\D8\AE\D8\AF\D9\85", align 8
  store i64 11, ptr %"%\D9\85\D8\B3\D8\AA\D9\88\D9\8A\D8\A7\D8\AA_buddy", align 8
  store i64 1, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\AA\D8\A7\D8\AD\D8\A9", align 8
  store i64 2, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\AD\D8\AC\D9\88\D8\B2\D8\A9", align 8
  store i64 3, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_ACPI", align 8
  store i64 5, ptr %"%\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D9\85\D8\B9\D9\8A\D8\A8\D8\A9", align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\AD\D8\AC\D9\85_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D8\AD\D8\B1\D8\A9", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D8\B5\D9\81\D8\AD\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\B4\D8\BA\D9\88\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\AE\D8\B5\D9\8A\D8\B5\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1\D8\A7\D8\AA", align 8
  store i64 256, ptr %"%\D9\83\D8\AA\D9\84_\D9\84\D9\83\D9\84_\D9\85\D8\B3\D8\AA\D9\88\D9\89", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 16, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep1 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data2 = load ptr, ptr %arr.data.gep1, align 8
  %arr.elem = getelementptr i64, ptr %arr.data2, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 1
  store i64 0, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 0, ptr %arr.elem8, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 3
  store i64 0, ptr %arr.elem11, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 4
  store i64 0, ptr %arr.elem14, align 8
  %arr.data.gep15 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data16 = load ptr, ptr %arr.data.gep15, align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 5
  store i64 0, ptr %arr.elem17, align 8
  %arr.data.gep18 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data19 = load ptr, ptr %arr.data.gep18, align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 6
  store i64 0, ptr %arr.elem20, align 8
  %arr.data.gep21 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data22 = load ptr, ptr %arr.data.gep21, align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 7
  store i64 0, ptr %arr.elem23, align 8
  %arr.data.gep24 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data25 = load ptr, ptr %arr.data.gep24, align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 8
  store i64 0, ptr %arr.elem26, align 8
  %arr.data.gep27 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data28 = load ptr, ptr %arr.data.gep27, align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 9
  store i64 0, ptr %arr.elem29, align 8
  %arr.data.gep30 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data31 = load ptr, ptr %arr.data.gep30, align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 10
  store i64 0, ptr %arr.elem32, align 8
  %arr.data.gep33 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data34 = load ptr, ptr %arr.data.gep33, align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 11
  store i64 0, ptr %arr.elem35, align 8
  %arr.data.gep36 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data37 = load ptr, ptr %arr.data.gep36, align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 12
  store i64 0, ptr %arr.elem38, align 8
  %arr.data.gep39 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data40 = load ptr, ptr %arr.data.gep39, align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 13
  store i64 0, ptr %arr.elem41, align 8
  %arr.data.gep42 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data43 = load ptr, ptr %arr.data.gep42, align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 14
  store i64 0, ptr %arr.elem44, align 8
  %arr.data.gep45 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data46 = load ptr, ptr %arr.data.gep45, align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 15
  store i64 0, ptr %arr.elem47, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_0", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_buddy_0", align 8
  %arr_new48 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep49 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 0
  store i64 16, ptr %arr.len.gep49, align 8
  %arr.cap.gep50 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep50, align 8
  %arr.data51 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep52 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  store ptr %arr.data51, ptr %arr.data.gep52, align 8
  %arr.data.gep53 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data54 = load ptr, ptr %arr.data.gep53, align 8
  %arr.elem55 = getelementptr i64, ptr %arr.data54, i64 0
  store i64 0, ptr %arr.elem55, align 8
  %arr.data.gep56 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data57 = load ptr, ptr %arr.data.gep56, align 8
  %arr.elem58 = getelementptr i64, ptr %arr.data57, i64 1
  store i64 0, ptr %arr.elem58, align 8
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 2
  store i64 0, ptr %arr.elem61, align 8
  %arr.data.gep62 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data63 = load ptr, ptr %arr.data.gep62, align 8
  %arr.elem64 = getelementptr i64, ptr %arr.data63, i64 3
  store i64 0, ptr %arr.elem64, align 8
  %arr.data.gep65 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data66 = load ptr, ptr %arr.data.gep65, align 8
  %arr.elem67 = getelementptr i64, ptr %arr.data66, i64 4
  store i64 0, ptr %arr.elem67, align 8
  %arr.data.gep68 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data69 = load ptr, ptr %arr.data.gep68, align 8
  %arr.elem70 = getelementptr i64, ptr %arr.data69, i64 5
  store i64 0, ptr %arr.elem70, align 8
  %arr.data.gep71 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data72 = load ptr, ptr %arr.data.gep71, align 8
  %arr.elem73 = getelementptr i64, ptr %arr.data72, i64 6
  store i64 0, ptr %arr.elem73, align 8
  %arr.data.gep74 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data75 = load ptr, ptr %arr.data.gep74, align 8
  %arr.elem76 = getelementptr i64, ptr %arr.data75, i64 7
  store i64 0, ptr %arr.elem76, align 8
  %arr.data.gep77 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data78 = load ptr, ptr %arr.data.gep77, align 8
  %arr.elem79 = getelementptr i64, ptr %arr.data78, i64 8
  store i64 0, ptr %arr.elem79, align 8
  %arr.data.gep80 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data81 = load ptr, ptr %arr.data.gep80, align 8
  %arr.elem82 = getelementptr i64, ptr %arr.data81, i64 9
  store i64 0, ptr %arr.elem82, align 8
  %arr.data.gep83 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data84 = load ptr, ptr %arr.data.gep83, align 8
  %arr.elem85 = getelementptr i64, ptr %arr.data84, i64 10
  store i64 0, ptr %arr.elem85, align 8
  %arr.data.gep86 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data87 = load ptr, ptr %arr.data.gep86, align 8
  %arr.elem88 = getelementptr i64, ptr %arr.data87, i64 11
  store i64 0, ptr %arr.elem88, align 8
  %arr.data.gep89 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data90 = load ptr, ptr %arr.data.gep89, align 8
  %arr.elem91 = getelementptr i64, ptr %arr.data90, i64 12
  store i64 0, ptr %arr.elem91, align 8
  %arr.data.gep92 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data93 = load ptr, ptr %arr.data.gep92, align 8
  %arr.elem94 = getelementptr i64, ptr %arr.data93, i64 13
  store i64 0, ptr %arr.elem94, align 8
  %arr.data.gep95 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data96 = load ptr, ptr %arr.data.gep95, align 8
  %arr.elem97 = getelementptr i64, ptr %arr.data96, i64 14
  store i64 0, ptr %arr.elem97, align 8
  %arr.data.gep98 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 2
  %arr.data99 = load ptr, ptr %arr.data.gep98, align 8
  %arr.elem100 = getelementptr i64, ptr %arr.data99, i64 15
  store i64 0, ptr %arr.elem100, align 8
  %obj.ptrtoint101 = ptrtoint ptr %arr_new48 to i64
  store i64 %obj.ptrtoint101, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_1", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_buddy_1", align 8
  %arr_new102 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep103 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 0
  store i64 16, ptr %arr.len.gep103, align 8
  %arr.cap.gep104 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep104, align 8
  %arr.data105 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep106 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  store ptr %arr.data105, ptr %arr.data.gep106, align 8
  %arr.data.gep107 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data108 = load ptr, ptr %arr.data.gep107, align 8
  %arr.elem109 = getelementptr i64, ptr %arr.data108, i64 0
  store i64 0, ptr %arr.elem109, align 8
  %arr.data.gep110 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data111 = load ptr, ptr %arr.data.gep110, align 8
  %arr.elem112 = getelementptr i64, ptr %arr.data111, i64 1
  store i64 0, ptr %arr.elem112, align 8
  %arr.data.gep113 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data114 = load ptr, ptr %arr.data.gep113, align 8
  %arr.elem115 = getelementptr i64, ptr %arr.data114, i64 2
  store i64 0, ptr %arr.elem115, align 8
  %arr.data.gep116 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data117 = load ptr, ptr %arr.data.gep116, align 8
  %arr.elem118 = getelementptr i64, ptr %arr.data117, i64 3
  store i64 0, ptr %arr.elem118, align 8
  %arr.data.gep119 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data120 = load ptr, ptr %arr.data.gep119, align 8
  %arr.elem121 = getelementptr i64, ptr %arr.data120, i64 4
  store i64 0, ptr %arr.elem121, align 8
  %arr.data.gep122 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data123 = load ptr, ptr %arr.data.gep122, align 8
  %arr.elem124 = getelementptr i64, ptr %arr.data123, i64 5
  store i64 0, ptr %arr.elem124, align 8
  %arr.data.gep125 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data126 = load ptr, ptr %arr.data.gep125, align 8
  %arr.elem127 = getelementptr i64, ptr %arr.data126, i64 6
  store i64 0, ptr %arr.elem127, align 8
  %arr.data.gep128 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data129 = load ptr, ptr %arr.data.gep128, align 8
  %arr.elem130 = getelementptr i64, ptr %arr.data129, i64 7
  store i64 0, ptr %arr.elem130, align 8
  %arr.data.gep131 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data132 = load ptr, ptr %arr.data.gep131, align 8
  %arr.elem133 = getelementptr i64, ptr %arr.data132, i64 8
  store i64 0, ptr %arr.elem133, align 8
  %arr.data.gep134 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data135 = load ptr, ptr %arr.data.gep134, align 8
  %arr.elem136 = getelementptr i64, ptr %arr.data135, i64 9
  store i64 0, ptr %arr.elem136, align 8
  %arr.data.gep137 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data138 = load ptr, ptr %arr.data.gep137, align 8
  %arr.elem139 = getelementptr i64, ptr %arr.data138, i64 10
  store i64 0, ptr %arr.elem139, align 8
  %arr.data.gep140 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data141 = load ptr, ptr %arr.data.gep140, align 8
  %arr.elem142 = getelementptr i64, ptr %arr.data141, i64 11
  store i64 0, ptr %arr.elem142, align 8
  %arr.data.gep143 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data144 = load ptr, ptr %arr.data.gep143, align 8
  %arr.elem145 = getelementptr i64, ptr %arr.data144, i64 12
  store i64 0, ptr %arr.elem145, align 8
  %arr.data.gep146 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data147 = load ptr, ptr %arr.data.gep146, align 8
  %arr.elem148 = getelementptr i64, ptr %arr.data147, i64 13
  store i64 0, ptr %arr.elem148, align 8
  %arr.data.gep149 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data150 = load ptr, ptr %arr.data.gep149, align 8
  %arr.elem151 = getelementptr i64, ptr %arr.data150, i64 14
  store i64 0, ptr %arr.elem151, align 8
  %arr.data.gep152 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 2
  %arr.data153 = load ptr, ptr %arr.data.gep152, align 8
  %arr.elem154 = getelementptr i64, ptr %arr.data153, i64 15
  store i64 0, ptr %arr.elem154, align 8
  %obj.ptrtoint155 = ptrtoint ptr %arr_new102 to i64
  store i64 %obj.ptrtoint155, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_2", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_buddy_2", align 8
  %arr_new156 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep157 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 0
  store i64 16, ptr %arr.len.gep157, align 8
  %arr.cap.gep158 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep158, align 8
  %arr.data159 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep160 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  store ptr %arr.data159, ptr %arr.data.gep160, align 8
  %arr.data.gep161 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data162 = load ptr, ptr %arr.data.gep161, align 8
  %arr.elem163 = getelementptr i64, ptr %arr.data162, i64 0
  store i64 0, ptr %arr.elem163, align 8
  %arr.data.gep164 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data165 = load ptr, ptr %arr.data.gep164, align 8
  %arr.elem166 = getelementptr i64, ptr %arr.data165, i64 1
  store i64 0, ptr %arr.elem166, align 8
  %arr.data.gep167 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data168 = load ptr, ptr %arr.data.gep167, align 8
  %arr.elem169 = getelementptr i64, ptr %arr.data168, i64 2
  store i64 0, ptr %arr.elem169, align 8
  %arr.data.gep170 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data171 = load ptr, ptr %arr.data.gep170, align 8
  %arr.elem172 = getelementptr i64, ptr %arr.data171, i64 3
  store i64 0, ptr %arr.elem172, align 8
  %arr.data.gep173 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data174 = load ptr, ptr %arr.data.gep173, align 8
  %arr.elem175 = getelementptr i64, ptr %arr.data174, i64 4
  store i64 0, ptr %arr.elem175, align 8
  %arr.data.gep176 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data177 = load ptr, ptr %arr.data.gep176, align 8
  %arr.elem178 = getelementptr i64, ptr %arr.data177, i64 5
  store i64 0, ptr %arr.elem178, align 8
  %arr.data.gep179 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data180 = load ptr, ptr %arr.data.gep179, align 8
  %arr.elem181 = getelementptr i64, ptr %arr.data180, i64 6
  store i64 0, ptr %arr.elem181, align 8
  %arr.data.gep182 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data183 = load ptr, ptr %arr.data.gep182, align 8
  %arr.elem184 = getelementptr i64, ptr %arr.data183, i64 7
  store i64 0, ptr %arr.elem184, align 8
  %arr.data.gep185 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data186 = load ptr, ptr %arr.data.gep185, align 8
  %arr.elem187 = getelementptr i64, ptr %arr.data186, i64 8
  store i64 0, ptr %arr.elem187, align 8
  %arr.data.gep188 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data189 = load ptr, ptr %arr.data.gep188, align 8
  %arr.elem190 = getelementptr i64, ptr %arr.data189, i64 9
  store i64 0, ptr %arr.elem190, align 8
  %arr.data.gep191 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data192 = load ptr, ptr %arr.data.gep191, align 8
  %arr.elem193 = getelementptr i64, ptr %arr.data192, i64 10
  store i64 0, ptr %arr.elem193, align 8
  %arr.data.gep194 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data195 = load ptr, ptr %arr.data.gep194, align 8
  %arr.elem196 = getelementptr i64, ptr %arr.data195, i64 11
  store i64 0, ptr %arr.elem196, align 8
  %arr.data.gep197 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data198 = load ptr, ptr %arr.data.gep197, align 8
  %arr.elem199 = getelementptr i64, ptr %arr.data198, i64 12
  store i64 0, ptr %arr.elem199, align 8
  %arr.data.gep200 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data201 = load ptr, ptr %arr.data.gep200, align 8
  %arr.elem202 = getelementptr i64, ptr %arr.data201, i64 13
  store i64 0, ptr %arr.elem202, align 8
  %arr.data.gep203 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data204 = load ptr, ptr %arr.data.gep203, align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 14
  store i64 0, ptr %arr.elem205, align 8
  %arr.data.gep206 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 2
  %arr.data207 = load ptr, ptr %arr.data.gep206, align 8
  %arr.elem208 = getelementptr i64, ptr %arr.data207, i64 15
  store i64 0, ptr %arr.elem208, align 8
  %obj.ptrtoint209 = ptrtoint ptr %arr_new156 to i64
  store i64 %obj.ptrtoint209, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_3", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_buddy_3", align 8
  %arr_new210 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep211 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 0
  store i64 16, ptr %arr.len.gep211, align 8
  %arr.cap.gep212 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep212, align 8
  %arr.data213 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep214 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  store ptr %arr.data213, ptr %arr.data.gep214, align 8
  %arr.data.gep215 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data216 = load ptr, ptr %arr.data.gep215, align 8
  %arr.elem217 = getelementptr i64, ptr %arr.data216, i64 0
  store i64 0, ptr %arr.elem217, align 8
  %arr.data.gep218 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data219 = load ptr, ptr %arr.data.gep218, align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 1
  store i64 0, ptr %arr.elem220, align 8
  %arr.data.gep221 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data222 = load ptr, ptr %arr.data.gep221, align 8
  %arr.elem223 = getelementptr i64, ptr %arr.data222, i64 2
  store i64 0, ptr %arr.elem223, align 8
  %arr.data.gep224 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data225 = load ptr, ptr %arr.data.gep224, align 8
  %arr.elem226 = getelementptr i64, ptr %arr.data225, i64 3
  store i64 0, ptr %arr.elem226, align 8
  %arr.data.gep227 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data228 = load ptr, ptr %arr.data.gep227, align 8
  %arr.elem229 = getelementptr i64, ptr %arr.data228, i64 4
  store i64 0, ptr %arr.elem229, align 8
  %arr.data.gep230 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data231 = load ptr, ptr %arr.data.gep230, align 8
  %arr.elem232 = getelementptr i64, ptr %arr.data231, i64 5
  store i64 0, ptr %arr.elem232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 6
  store i64 0, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 7
  store i64 0, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 8
  store i64 0, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 9
  store i64 0, ptr %arr.elem244, align 8
  %arr.data.gep245 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data246 = load ptr, ptr %arr.data.gep245, align 8
  %arr.elem247 = getelementptr i64, ptr %arr.data246, i64 10
  store i64 0, ptr %arr.elem247, align 8
  %arr.data.gep248 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data249 = load ptr, ptr %arr.data.gep248, align 8
  %arr.elem250 = getelementptr i64, ptr %arr.data249, i64 11
  store i64 0, ptr %arr.elem250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 12
  store i64 0, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 13
  store i64 0, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 14
  store i64 0, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 15
  store i64 0, ptr %arr.elem262, align 8
  %obj.ptrtoint263 = ptrtoint ptr %arr_new210 to i64
  store i64 %obj.ptrtoint263, ptr %"%\D9\82\D8\A7\D8\A6\D9\85\D8\A9_buddy_4", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_buddy_4", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_PML4", align 8
  store i64 32, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D9\86\D8\A7\D8\B7\D9\82", align 8
  %arr_new264 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep265 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 0
  store i64 32, ptr %arr.len.gep265, align 8
  %arr.cap.gep266 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep266, align 8
  %arr.data267 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep268 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  store ptr %arr.data267, ptr %arr.data.gep268, align 8
  %arr.data.gep269 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data270 = load ptr, ptr %arr.data.gep269, align 8
  %arr.elem271 = getelementptr i64, ptr %arr.data270, i64 0
  store i64 0, ptr %arr.elem271, align 8
  %arr.data.gep272 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data273 = load ptr, ptr %arr.data.gep272, align 8
  %arr.elem274 = getelementptr i64, ptr %arr.data273, i64 1
  store i64 0, ptr %arr.elem274, align 8
  %arr.data.gep275 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data276 = load ptr, ptr %arr.data.gep275, align 8
  %arr.elem277 = getelementptr i64, ptr %arr.data276, i64 2
  store i64 0, ptr %arr.elem277, align 8
  %arr.data.gep278 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data279 = load ptr, ptr %arr.data.gep278, align 8
  %arr.elem280 = getelementptr i64, ptr %arr.data279, i64 3
  store i64 0, ptr %arr.elem280, align 8
  %arr.data.gep281 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data282 = load ptr, ptr %arr.data.gep281, align 8
  %arr.elem283 = getelementptr i64, ptr %arr.data282, i64 4
  store i64 0, ptr %arr.elem283, align 8
  %arr.data.gep284 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data285 = load ptr, ptr %arr.data.gep284, align 8
  %arr.elem286 = getelementptr i64, ptr %arr.data285, i64 5
  store i64 0, ptr %arr.elem286, align 8
  %arr.data.gep287 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data288 = load ptr, ptr %arr.data.gep287, align 8
  %arr.elem289 = getelementptr i64, ptr %arr.data288, i64 6
  store i64 0, ptr %arr.elem289, align 8
  %arr.data.gep290 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data291 = load ptr, ptr %arr.data.gep290, align 8
  %arr.elem292 = getelementptr i64, ptr %arr.data291, i64 7
  store i64 0, ptr %arr.elem292, align 8
  %arr.data.gep293 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data294 = load ptr, ptr %arr.data.gep293, align 8
  %arr.elem295 = getelementptr i64, ptr %arr.data294, i64 8
  store i64 0, ptr %arr.elem295, align 8
  %arr.data.gep296 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data297 = load ptr, ptr %arr.data.gep296, align 8
  %arr.elem298 = getelementptr i64, ptr %arr.data297, i64 9
  store i64 0, ptr %arr.elem298, align 8
  %arr.data.gep299 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data300 = load ptr, ptr %arr.data.gep299, align 8
  %arr.elem301 = getelementptr i64, ptr %arr.data300, i64 10
  store i64 0, ptr %arr.elem301, align 8
  %arr.data.gep302 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data303 = load ptr, ptr %arr.data.gep302, align 8
  %arr.elem304 = getelementptr i64, ptr %arr.data303, i64 11
  store i64 0, ptr %arr.elem304, align 8
  %arr.data.gep305 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data306 = load ptr, ptr %arr.data.gep305, align 8
  %arr.elem307 = getelementptr i64, ptr %arr.data306, i64 12
  store i64 0, ptr %arr.elem307, align 8
  %arr.data.gep308 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data309 = load ptr, ptr %arr.data.gep308, align 8
  %arr.elem310 = getelementptr i64, ptr %arr.data309, i64 13
  store i64 0, ptr %arr.elem310, align 8
  %arr.data.gep311 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data312 = load ptr, ptr %arr.data.gep311, align 8
  %arr.elem313 = getelementptr i64, ptr %arr.data312, i64 14
  store i64 0, ptr %arr.elem313, align 8
  %arr.data.gep314 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data315 = load ptr, ptr %arr.data.gep314, align 8
  %arr.elem316 = getelementptr i64, ptr %arr.data315, i64 15
  store i64 0, ptr %arr.elem316, align 8
  %arr.data.gep317 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data318 = load ptr, ptr %arr.data.gep317, align 8
  %arr.elem319 = getelementptr i64, ptr %arr.data318, i64 16
  store i64 0, ptr %arr.elem319, align 8
  %arr.data.gep320 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data321 = load ptr, ptr %arr.data.gep320, align 8
  %arr.elem322 = getelementptr i64, ptr %arr.data321, i64 17
  store i64 0, ptr %arr.elem322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 18
  store i64 0, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 19
  store i64 0, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 20
  store i64 0, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 21
  store i64 0, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 22
  store i64 0, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 23
  store i64 0, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 24
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 25
  store i64 0, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 26
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 27
  store i64 0, ptr %arr.elem352, align 8
  %arr.data.gep353 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data354 = load ptr, ptr %arr.data.gep353, align 8
  %arr.elem355 = getelementptr i64, ptr %arr.data354, i64 28
  store i64 0, ptr %arr.elem355, align 8
  %arr.data.gep356 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data357 = load ptr, ptr %arr.data.gep356, align 8
  %arr.elem358 = getelementptr i64, ptr %arr.data357, i64 29
  store i64 0, ptr %arr.elem358, align 8
  %arr.data.gep359 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data360 = load ptr, ptr %arr.data.gep359, align 8
  %arr.elem361 = getelementptr i64, ptr %arr.data360, i64 30
  store i64 0, ptr %arr.elem361, align 8
  %arr.data.gep362 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 2
  %arr.data363 = load ptr, ptr %arr.data.gep362, align 8
  %arr.elem364 = getelementptr i64, ptr %arr.data363, i64 31
  store i64 0, ptr %arr.elem364, align 8
  %obj.ptrtoint365 = ptrtoint ptr %arr_new264 to i64
  store i64 %obj.ptrtoint365, ptr %"%\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %arr_new366 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep367 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 0
  store i64 32, ptr %arr.len.gep367, align 8
  %arr.cap.gep368 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep368, align 8
  %arr.data369 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep370 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  store ptr %arr.data369, ptr %arr.data.gep370, align 8
  %arr.data.gep371 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data372 = load ptr, ptr %arr.data.gep371, align 8
  %arr.elem373 = getelementptr i64, ptr %arr.data372, i64 0
  store i64 0, ptr %arr.elem373, align 8
  %arr.data.gep374 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data375 = load ptr, ptr %arr.data.gep374, align 8
  %arr.elem376 = getelementptr i64, ptr %arr.data375, i64 1
  store i64 0, ptr %arr.elem376, align 8
  %arr.data.gep377 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data378 = load ptr, ptr %arr.data.gep377, align 8
  %arr.elem379 = getelementptr i64, ptr %arr.data378, i64 2
  store i64 0, ptr %arr.elem379, align 8
  %arr.data.gep380 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data381 = load ptr, ptr %arr.data.gep380, align 8
  %arr.elem382 = getelementptr i64, ptr %arr.data381, i64 3
  store i64 0, ptr %arr.elem382, align 8
  %arr.data.gep383 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data384 = load ptr, ptr %arr.data.gep383, align 8
  %arr.elem385 = getelementptr i64, ptr %arr.data384, i64 4
  store i64 0, ptr %arr.elem385, align 8
  %arr.data.gep386 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data387 = load ptr, ptr %arr.data.gep386, align 8
  %arr.elem388 = getelementptr i64, ptr %arr.data387, i64 5
  store i64 0, ptr %arr.elem388, align 8
  %arr.data.gep389 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data390 = load ptr, ptr %arr.data.gep389, align 8
  %arr.elem391 = getelementptr i64, ptr %arr.data390, i64 6
  store i64 0, ptr %arr.elem391, align 8
  %arr.data.gep392 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data393 = load ptr, ptr %arr.data.gep392, align 8
  %arr.elem394 = getelementptr i64, ptr %arr.data393, i64 7
  store i64 0, ptr %arr.elem394, align 8
  %arr.data.gep395 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data396 = load ptr, ptr %arr.data.gep395, align 8
  %arr.elem397 = getelementptr i64, ptr %arr.data396, i64 8
  store i64 0, ptr %arr.elem397, align 8
  %arr.data.gep398 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data399 = load ptr, ptr %arr.data.gep398, align 8
  %arr.elem400 = getelementptr i64, ptr %arr.data399, i64 9
  store i64 0, ptr %arr.elem400, align 8
  %arr.data.gep401 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data402 = load ptr, ptr %arr.data.gep401, align 8
  %arr.elem403 = getelementptr i64, ptr %arr.data402, i64 10
  store i64 0, ptr %arr.elem403, align 8
  %arr.data.gep404 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data405 = load ptr, ptr %arr.data.gep404, align 8
  %arr.elem406 = getelementptr i64, ptr %arr.data405, i64 11
  store i64 0, ptr %arr.elem406, align 8
  %arr.data.gep407 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data408 = load ptr, ptr %arr.data.gep407, align 8
  %arr.elem409 = getelementptr i64, ptr %arr.data408, i64 12
  store i64 0, ptr %arr.elem409, align 8
  %arr.data.gep410 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data411 = load ptr, ptr %arr.data.gep410, align 8
  %arr.elem412 = getelementptr i64, ptr %arr.data411, i64 13
  store i64 0, ptr %arr.elem412, align 8
  %arr.data.gep413 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data414 = load ptr, ptr %arr.data.gep413, align 8
  %arr.elem415 = getelementptr i64, ptr %arr.data414, i64 14
  store i64 0, ptr %arr.elem415, align 8
  %arr.data.gep416 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data417 = load ptr, ptr %arr.data.gep416, align 8
  %arr.elem418 = getelementptr i64, ptr %arr.data417, i64 15
  store i64 0, ptr %arr.elem418, align 8
  %arr.data.gep419 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data420 = load ptr, ptr %arr.data.gep419, align 8
  %arr.elem421 = getelementptr i64, ptr %arr.data420, i64 16
  store i64 0, ptr %arr.elem421, align 8
  %arr.data.gep422 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data423 = load ptr, ptr %arr.data.gep422, align 8
  %arr.elem424 = getelementptr i64, ptr %arr.data423, i64 17
  store i64 0, ptr %arr.elem424, align 8
  %arr.data.gep425 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data426 = load ptr, ptr %arr.data.gep425, align 8
  %arr.elem427 = getelementptr i64, ptr %arr.data426, i64 18
  store i64 0, ptr %arr.elem427, align 8
  %arr.data.gep428 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data429 = load ptr, ptr %arr.data.gep428, align 8
  %arr.elem430 = getelementptr i64, ptr %arr.data429, i64 19
  store i64 0, ptr %arr.elem430, align 8
  %arr.data.gep431 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data432 = load ptr, ptr %arr.data.gep431, align 8
  %arr.elem433 = getelementptr i64, ptr %arr.data432, i64 20
  store i64 0, ptr %arr.elem433, align 8
  %arr.data.gep434 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data435 = load ptr, ptr %arr.data.gep434, align 8
  %arr.elem436 = getelementptr i64, ptr %arr.data435, i64 21
  store i64 0, ptr %arr.elem436, align 8
  %arr.data.gep437 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data438 = load ptr, ptr %arr.data.gep437, align 8
  %arr.elem439 = getelementptr i64, ptr %arr.data438, i64 22
  store i64 0, ptr %arr.elem439, align 8
  %arr.data.gep440 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data441 = load ptr, ptr %arr.data.gep440, align 8
  %arr.elem442 = getelementptr i64, ptr %arr.data441, i64 23
  store i64 0, ptr %arr.elem442, align 8
  %arr.data.gep443 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data444 = load ptr, ptr %arr.data.gep443, align 8
  %arr.elem445 = getelementptr i64, ptr %arr.data444, i64 24
  store i64 0, ptr %arr.elem445, align 8
  %arr.data.gep446 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data447 = load ptr, ptr %arr.data.gep446, align 8
  %arr.elem448 = getelementptr i64, ptr %arr.data447, i64 25
  store i64 0, ptr %arr.elem448, align 8
  %arr.data.gep449 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data450 = load ptr, ptr %arr.data.gep449, align 8
  %arr.elem451 = getelementptr i64, ptr %arr.data450, i64 26
  store i64 0, ptr %arr.elem451, align 8
  %arr.data.gep452 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data453 = load ptr, ptr %arr.data.gep452, align 8
  %arr.elem454 = getelementptr i64, ptr %arr.data453, i64 27
  store i64 0, ptr %arr.elem454, align 8
  %arr.data.gep455 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data456 = load ptr, ptr %arr.data.gep455, align 8
  %arr.elem457 = getelementptr i64, ptr %arr.data456, i64 28
  store i64 0, ptr %arr.elem457, align 8
  %arr.data.gep458 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data459 = load ptr, ptr %arr.data.gep458, align 8
  %arr.elem460 = getelementptr i64, ptr %arr.data459, i64 29
  store i64 0, ptr %arr.elem460, align 8
  %arr.data.gep461 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data462 = load ptr, ptr %arr.data.gep461, align 8
  %arr.elem463 = getelementptr i64, ptr %arr.data462, i64 30
  store i64 0, ptr %arr.elem463, align 8
  %arr.data.gep464 = getelementptr inbounds %SadArray, ptr %arr_new366, i32 0, i32 2
  %arr.data465 = load ptr, ptr %arr.data.gep464, align 8
  %arr.elem466 = getelementptr i64, ptr %arr.data465, i64 31
  store i64 0, ptr %arr.elem466, align 8
  %obj.ptrtoint467 = ptrtoint ptr %arr_new366 to i64
  store i64 %obj.ptrtoint467, ptr %"%\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D8\AD\D8\AC\D9\85", align 8
  %arr_new468 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep469 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 0
  store i64 32, ptr %arr.len.gep469, align 8
  %arr.cap.gep470 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep470, align 8
  %arr.data471 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep472 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  store ptr %arr.data471, ptr %arr.data.gep472, align 8
  %arr.data.gep473 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data474 = load ptr, ptr %arr.data.gep473, align 8
  %arr.elem475 = getelementptr i64, ptr %arr.data474, i64 0
  store i64 0, ptr %arr.elem475, align 8
  %arr.data.gep476 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data477 = load ptr, ptr %arr.data.gep476, align 8
  %arr.elem478 = getelementptr i64, ptr %arr.data477, i64 1
  store i64 0, ptr %arr.elem478, align 8
  %arr.data.gep479 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data480 = load ptr, ptr %arr.data.gep479, align 8
  %arr.elem481 = getelementptr i64, ptr %arr.data480, i64 2
  store i64 0, ptr %arr.elem481, align 8
  %arr.data.gep482 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data483 = load ptr, ptr %arr.data.gep482, align 8
  %arr.elem484 = getelementptr i64, ptr %arr.data483, i64 3
  store i64 0, ptr %arr.elem484, align 8
  %arr.data.gep485 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data486 = load ptr, ptr %arr.data.gep485, align 8
  %arr.elem487 = getelementptr i64, ptr %arr.data486, i64 4
  store i64 0, ptr %arr.elem487, align 8
  %arr.data.gep488 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data489 = load ptr, ptr %arr.data.gep488, align 8
  %arr.elem490 = getelementptr i64, ptr %arr.data489, i64 5
  store i64 0, ptr %arr.elem490, align 8
  %arr.data.gep491 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data492 = load ptr, ptr %arr.data.gep491, align 8
  %arr.elem493 = getelementptr i64, ptr %arr.data492, i64 6
  store i64 0, ptr %arr.elem493, align 8
  %arr.data.gep494 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data495 = load ptr, ptr %arr.data.gep494, align 8
  %arr.elem496 = getelementptr i64, ptr %arr.data495, i64 7
  store i64 0, ptr %arr.elem496, align 8
  %arr.data.gep497 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data498 = load ptr, ptr %arr.data.gep497, align 8
  %arr.elem499 = getelementptr i64, ptr %arr.data498, i64 8
  store i64 0, ptr %arr.elem499, align 8
  %arr.data.gep500 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data501 = load ptr, ptr %arr.data.gep500, align 8
  %arr.elem502 = getelementptr i64, ptr %arr.data501, i64 9
  store i64 0, ptr %arr.elem502, align 8
  %arr.data.gep503 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data504 = load ptr, ptr %arr.data.gep503, align 8
  %arr.elem505 = getelementptr i64, ptr %arr.data504, i64 10
  store i64 0, ptr %arr.elem505, align 8
  %arr.data.gep506 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data507 = load ptr, ptr %arr.data.gep506, align 8
  %arr.elem508 = getelementptr i64, ptr %arr.data507, i64 11
  store i64 0, ptr %arr.elem508, align 8
  %arr.data.gep509 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data510 = load ptr, ptr %arr.data.gep509, align 8
  %arr.elem511 = getelementptr i64, ptr %arr.data510, i64 12
  store i64 0, ptr %arr.elem511, align 8
  %arr.data.gep512 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data513 = load ptr, ptr %arr.data.gep512, align 8
  %arr.elem514 = getelementptr i64, ptr %arr.data513, i64 13
  store i64 0, ptr %arr.elem514, align 8
  %arr.data.gep515 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data516 = load ptr, ptr %arr.data.gep515, align 8
  %arr.elem517 = getelementptr i64, ptr %arr.data516, i64 14
  store i64 0, ptr %arr.elem517, align 8
  %arr.data.gep518 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data519 = load ptr, ptr %arr.data.gep518, align 8
  %arr.elem520 = getelementptr i64, ptr %arr.data519, i64 15
  store i64 0, ptr %arr.elem520, align 8
  %arr.data.gep521 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data522 = load ptr, ptr %arr.data.gep521, align 8
  %arr.elem523 = getelementptr i64, ptr %arr.data522, i64 16
  store i64 0, ptr %arr.elem523, align 8
  %arr.data.gep524 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data525 = load ptr, ptr %arr.data.gep524, align 8
  %arr.elem526 = getelementptr i64, ptr %arr.data525, i64 17
  store i64 0, ptr %arr.elem526, align 8
  %arr.data.gep527 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data528 = load ptr, ptr %arr.data.gep527, align 8
  %arr.elem529 = getelementptr i64, ptr %arr.data528, i64 18
  store i64 0, ptr %arr.elem529, align 8
  %arr.data.gep530 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data531 = load ptr, ptr %arr.data.gep530, align 8
  %arr.elem532 = getelementptr i64, ptr %arr.data531, i64 19
  store i64 0, ptr %arr.elem532, align 8
  %arr.data.gep533 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data534 = load ptr, ptr %arr.data.gep533, align 8
  %arr.elem535 = getelementptr i64, ptr %arr.data534, i64 20
  store i64 0, ptr %arr.elem535, align 8
  %arr.data.gep536 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data537 = load ptr, ptr %arr.data.gep536, align 8
  %arr.elem538 = getelementptr i64, ptr %arr.data537, i64 21
  store i64 0, ptr %arr.elem538, align 8
  %arr.data.gep539 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data540 = load ptr, ptr %arr.data.gep539, align 8
  %arr.elem541 = getelementptr i64, ptr %arr.data540, i64 22
  store i64 0, ptr %arr.elem541, align 8
  %arr.data.gep542 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data543 = load ptr, ptr %arr.data.gep542, align 8
  %arr.elem544 = getelementptr i64, ptr %arr.data543, i64 23
  store i64 0, ptr %arr.elem544, align 8
  %arr.data.gep545 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data546 = load ptr, ptr %arr.data.gep545, align 8
  %arr.elem547 = getelementptr i64, ptr %arr.data546, i64 24
  store i64 0, ptr %arr.elem547, align 8
  %arr.data.gep548 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data549 = load ptr, ptr %arr.data.gep548, align 8
  %arr.elem550 = getelementptr i64, ptr %arr.data549, i64 25
  store i64 0, ptr %arr.elem550, align 8
  %arr.data.gep551 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data552 = load ptr, ptr %arr.data.gep551, align 8
  %arr.elem553 = getelementptr i64, ptr %arr.data552, i64 26
  store i64 0, ptr %arr.elem553, align 8
  %arr.data.gep554 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data555 = load ptr, ptr %arr.data.gep554, align 8
  %arr.elem556 = getelementptr i64, ptr %arr.data555, i64 27
  store i64 0, ptr %arr.elem556, align 8
  %arr.data.gep557 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data558 = load ptr, ptr %arr.data.gep557, align 8
  %arr.elem559 = getelementptr i64, ptr %arr.data558, i64 28
  store i64 0, ptr %arr.elem559, align 8
  %arr.data.gep560 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data561 = load ptr, ptr %arr.data.gep560, align 8
  %arr.elem562 = getelementptr i64, ptr %arr.data561, i64 29
  store i64 0, ptr %arr.elem562, align 8
  %arr.data.gep563 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data564 = load ptr, ptr %arr.data.gep563, align 8
  %arr.elem565 = getelementptr i64, ptr %arr.data564, i64 30
  store i64 0, ptr %arr.elem565, align 8
  %arr.data.gep566 = getelementptr inbounds %SadArray, ptr %arr_new468, i32 0, i32 2
  %arr.data567 = load ptr, ptr %arr.data.gep566, align 8
  %arr.elem568 = getelementptr i64, ptr %arr.data567, i64 31
  store i64 0, ptr %arr.elem568, align 8
  %obj.ptrtoint569 = ptrtoint ptr %arr_new468 to i64
  store i64 %obj.ptrtoint569, ptr %"%\D9\85\D9\86\D8\A7\D8\B7\D9\82_\D9\86\D9\88\D8\B9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D8\B7\D9\82", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\A7\D9\84\D9\85\D8\AA\D8\A7\D8\AD\D8\A9", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B0\D8\A7\D9\83\D8\B1\D8\A9_\D8\A7\D9\84\D9\85\D8\AD\D8\AC\D9\88\D8\B2\D8\A9", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
