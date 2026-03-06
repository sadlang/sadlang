; ModuleID = 'ملفات_عمليات_نحلة'
source_filename = "\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

declare void @sad_ll_mem_read64(i64)

declare void @sad_ll_mem_write64(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_mem_read16(i64)

declare void @sad_ll_mem_write16(i64, i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_page_alloc()

declare void @sad_ll_page_free(i64)

declare void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64, i64)

declare void @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\B9\D9\82\D8\AF\D8\A9"(i64, i64, i64)

declare void @"\D8\AE\D8\B5\D8\B5_\D9\83\D8\AA\D9\84\D8\A9"(i64)

declare void @"\D8\AD\D8\B1\D8\B1_\D9\83\D8\AA\D9\84\D8\A9"(i64, i64)

declare void @"\D8\AE\D8\B5\D8\B5_\D8\B9\D9\82\D8\AF\D8\A9"(i64)

declare void @"\D8\AD\D8\B1\D8\B1_\D8\B9\D9\82\D8\AF\D8\A9"(i64, i64)

declare void @"\D8\A7\D8\A8\D8\AD\D8\AB_\D9\81\D9\8A_\D8\AF\D9\84\D9\8A\D9\84"(i64, i64, i64, i64)

declare void @"\D8\A3\D8\B6\D9\81_\D9\85\D8\AF\D8\AE\D9\84\D8\A9_\D8\AF\D9\84\D9\8A\D9\84"(i64, i64, i64, i64, i64, i64)

declare void @"\D8\AA\D8\B1\D8\AC\D9\85_\D9\83\D8\AA\D9\84\D8\A9"(i64, i64, i64)

declare void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\B9"(i64)

declare void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64)

declare void @"\D8\B9\D9\82\D8\AF\D8\A9_\D9\83\D8\AA\D9\84\D8\A9"(i64, i64)

declare void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B7"(i64)

declare void @"blk_\D8\A7\D9\82\D8\B1\D8\A3"(i64, i64, i64)

declare void @"blk_\D8\A7\D9\83\D8\AA\D8\A8"(i64, i64, i64)

declare void @"blk_\D9\81\D8\B1\D9\91\D8\BA"()

define void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmplttmp, label %then_0, label %merge_1

merge_1:                                          ; preds = %entry
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load3" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load3", 10
  br i1 %cmpgetmp, label %then_2, label %merge_3

merge_3:                                          ; preds = %then_2, %merge_1
  ret void

then_0:                                           ; preds = %entry
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load2" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %subtmp = sub i64 0, %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load2"
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_2:                                           ; preds = %merge_1
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_3
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\84\D9\81\D8\A7\D8\AA"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  br i1 true, label %then_4, label %merge_5

merge_5:                                          ; preds = %entry
  br i1 true, label %then_6, label %merge_7

merge_7:                                          ; preds = %merge_5
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_8

then_4:                                           ; preds = %entry
  ret i64 -1

then_6:                                           ; preds = %merge_5
  ret i64 -1

while_body_9:                                     ; preds = %while_cond_8
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81.load2"
  store i64 0, ptr %arr.elem4, align 8
  %"%\D9\81.load5" = load i64, ptr %"%\D9\81", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\81.load5"
  store i64 0, ptr %arr.elem7, align 8
  %"%\D9\81.load8" = load i64, ptr %"%\D9\81", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81.load8"
  store i64 0, ptr %arr.elem10, align 8
  %"%\D9\81.load11" = load i64, ptr %"%\D9\81", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81.load11"
  store i64 0, ptr %arr.elem13, align 8
  %"%\D9\81.load14" = load i64, ptr %"%\D9\81", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81.load14"
  store i64 0, ptr %arr.elem16, align 8
  %"%\D9\81.load17" = load i64, ptr %"%\D9\81", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\81.load17"
  store i64 0, ptr %arr.elem19, align 8
  %"%\D9\81.load20" = load i64, ptr %"%\D9\81", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D9\81.load20"
  store i64 0, ptr %arr.elem22, align 8
  %"%\D9\81.load23" = load i64, ptr %"%\D9\81", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D9\81.load23"
  store i64 -1, ptr %arr.elem25, align 8
  %"%\D9\81.load26" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load26", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_8

while_cond_8:                                     ; preds = %while_body_9, %merge_7
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_9, label %while_exit_10

while_exit_10:                                    ; preds = %while_cond_8
  ret i64 0
}

define i64 @"\D8\AE\D8\B5\D8\B5_\D9\88\D8\A7\D8\B5\D9\81"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_11

merge_15:                                         ; preds = %while_body_12
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load2", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_11

then_14:                                          ; preds = %while_body_12
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_12:                                    ; preds = %while_cond_11
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_14, label %merge_15

while_cond_11:                                    ; preds = %merge_15, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_12, label %while_exit_13

while_exit_13:                                    ; preds = %while_cond_11
  ret i64 -1
}

define void @"\D8\AD\D8\B1\D8\B1_\D9\88\D8\A7\D8\B5\D9\81"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %then_16, label %merge_17

merge_17:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load2", 0
  br i1 %cmpgetmp, label %then_18, label %merge_19

merge_19:                                         ; preds = %merge_17
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_20, label %merge_21

merge_21:                                         ; preds = %then_20, %merge_19
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load11"
  store i64 0, ptr %arr.elem13, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load14" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load14"
  store i64 -1, ptr %arr.elem16, align 8
  ret void

then_16:                                          ; preds = %entry
  ret void

then_18:                                          ; preds = %merge_17
  ret void

then_20:                                          ; preds = %merge_19
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  call void @sad_ll_page_free(i64 %arr.get7)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load8" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load8"
  store i64 0, ptr %arr.elem10, align 8
  br label %merge_21
}

define void @"\D9\82\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1") {
entry:
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%pair" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86" = alloca i64, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B11" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B11", align 8
  store i64 0, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  store i64 0, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86", align 8
  store i64 0, ptr %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86", align 8
  br label %while_cond_22

else_29:                                          ; preds = %merge_26
  %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86.load11" = load i64, ptr %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86.load11", 0
  br i1 %cmpeqtmp12, label %then_34, label %merge_35

merge_26:                                         ; preds = %then_25, %while_body_23
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load", 47
  br i1 %cmpeqtmp8, label %then_27, label %else_29

merge_28:                                         ; preds = %merge_35, %merge_31
  %"%\D9\85\D9\88\D9\82\D8\B9.load13" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %addtmp14 = add i64 %"%\D9\85\D9\88\D9\82\D8\B9.load13", 1
  store i64 %addtmp14, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  br label %while_cond_22

merge_31:                                         ; preds = %merge_33, %then_27
  br label %merge_28

merge_33:                                         ; preds = %then_32, %then_30
  store i64 0, ptr %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86", align 8
  br label %merge_31

merge_35:                                         ; preds = %then_34, %else_29
  br label %merge_28

merge_37:                                         ; preds = %merge_39, %while_exit_24
  ret void

merge_39:                                         ; preds = %then_38, %then_36
  br label %merge_37

then_25:                                          ; preds = %while_body_23
  %"%pair.load7" = load i64, ptr %"%pair", align 8
  %shrtmp = lshr i64 %"%pair.load7", 8
  store i64 %shrtmp, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  br label %merge_26

then_27:                                          ; preds = %merge_26
  %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86.load9" = load i64, ptr %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86.load9", 1
  br i1 %cmpeqtmp10, label %then_30, label %merge_31

then_30:                                          ; preds = %then_27
  br i1 true, label %then_32, label %merge_33

then_32:                                          ; preds = %then_30
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load", ptr %arr.elem, align 8
  %"%\D9\85\D9\88\D9\82\D8\B9.load15" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load16" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86", align 8
  %subtmp17 = sub i64 %"%\D9\85\D9\88\D9\82\D8\B9.load15", %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load16"
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 0
  store i64 %subtmp17, ptr %arr.elem19, align 8
  br label %merge_33

then_34:                                          ; preds = %else_29
  %"%\D9\85\D9\88\D9\82\D8\B9.load20" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  store i64 %"%\D9\85\D9\88\D9\82\D8\B9.load20", ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86", align 8
  store i64 1, ptr %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86", align 8
  br label %merge_35

then_36:                                          ; preds = %while_exit_24
  br i1 true, label %then_38, label %merge_39

then_38:                                          ; preds = %then_36
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load21" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 0
  store i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load21", ptr %arr.elem23, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load24" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load25" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86", align 8
  %subtmp26 = sub i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load24", %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D9\83\D9\88\D9\86.load25"
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 0
  store i64 %subtmp26, ptr %arr.elem28, align 8
  br label %merge_39

while_body_23:                                    ; preds = %while_cond_22
  %"%\D9\85\D9\88\D9\82\D8\B9.load3" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %andtmp = and i64 %"%\D9\85\D9\88\D9\82\D8\B9.load3", 1
  %"%\D9\85\D9\88\D9\82\D8\B9.load4" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %subtmp = sub i64 %"%\D9\85\D9\88\D9\82\D8\B9.load4", %andtmp
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B11", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load", %subtmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_mem_read16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load")
  store i64 0, ptr %"%pair", align 8
  %"%pair.load" = load i64, ptr %"%pair", align 8
  %andtmp5 = and i64 %"%pair.load", 255
  store i64 %andtmp5, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_25, label %merge_26

while_cond_22:                                    ; preds = %merge_28, %entry
  %"%\D9\85\D9\88\D9\82\D8\B9.load" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D9\88\D9\82\D8\B9.load", %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load"
  br i1 %cmplttmp, label %while_body_23, label %while_exit_24

while_exit_24:                                    ; preds = %while_cond_22
  %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86.load" = load i64, ptr %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\81\D9\8A_\D9\85\D9\83\D9\88\D9\86.load", 1
  br i1 %cmpeqtmp6, label %then_36, label %merge_37
}

define i64 @"\D8\AD\D9\84\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1") {
entry:
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86" = alloca i64, align 8
  %"%\D9\85\D9\83" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  call void @"\D9\82\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load")
  br i1 true, label %then_40, label %merge_41

merge_41:                                         ; preds = %entry
  store i64 0, ptr %"%\D9\85\D9\83", align 8
  br label %while_cond_42

merge_46:                                         ; preds = %while_body_43
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load11" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  store i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load11", ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\85\D9\83.load12" = load i64, ptr %"%\D9\85\D9\83", align 8
  %addtmp13 = add i64 %"%\D9\85\D9\83.load12", 1
  store i64 %addtmp13, ptr %"%\D9\85\D9\83", align 8
  br label %while_cond_42

then_40:                                          ; preds = %entry
  ret i64 0

then_45:                                          ; preds = %while_body_43
  ret i64 -1

while_body_43:                                    ; preds = %while_cond_42
  %"%\D9\85\D9\83.load4" = load i64, ptr %"%\D9\85\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\85\D9\83.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load5" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load5", %arr.get
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  %"%\D9\85\D9\83.load6" = load i64, ptr %"%\D9\85\D9\83", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\85\D9\83.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  store i64 %arr.get9, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  call void @"\D8\A7\D8\A8\D8\AD\D8\AB_\D9\81\D9\8A_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load")
  store i64 0, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %cmplttmp10 = icmp slt i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp10, label %then_45, label %merge_46

while_cond_42:                                    ; preds = %merge_46, %merge_41
  %"%\D9\85\D9\83.load" = load i64, ptr %"%\D9\85\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D9\83.load", 0
  br i1 %cmplttmp, label %while_body_43, label %while_exit_44

while_exit_44:                                    ; preds = %while_cond_42
  %loadtmp = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\AD\D9\84\D9\84_\D9\85\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A3\D8\A8"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1") {
entry:
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86" = alloca i64, align 8
  %"%\D9\85\D9\83" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  call void @"\D9\82\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load")
  br i1 true, label %then_47, label %merge_48

merge_48:                                         ; preds = %entry
  store i64 0, ptr %"%\D9\85\D9\83", align 8
  br label %while_cond_49

merge_53:                                         ; preds = %while_body_50
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load11" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  store i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load11", ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D9\85\D9\83.load12" = load i64, ptr %"%\D9\85\D9\83", align 8
  %addtmp13 = add i64 %"%\D9\85\D9\83.load12", 1
  store i64 %addtmp13, ptr %"%\D9\85\D9\83", align 8
  br label %while_cond_49

then_47:                                          ; preds = %entry
  ret i64 0

then_52:                                          ; preds = %while_body_50
  ret i64 -1

while_body_50:                                    ; preds = %while_cond_49
  %"%\D9\85\D9\83.load4" = load i64, ptr %"%\D9\85\D9\83", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\85\D9\83.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load5" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load5", %arr.get
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  %"%\D9\85\D9\83.load6" = load i64, ptr %"%\D9\85\D9\83", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\85\D9\83.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  store i64 %arr.get9, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86", align 8
  call void @"\D8\A7\D8\A8\D8\AD\D8\AB_\D9\81\D9\8A_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86.load")
  store i64 0, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %cmplttmp10 = icmp slt i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp10, label %then_52, label %merge_53

while_cond_49:                                    ; preds = %merge_53, %merge_48
  %"%\D9\85\D9\83.load" = load i64, ptr %"%\D9\85\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D9\83.load", -1
  br i1 %cmplttmp, label %while_body_50, label %while_exit_51

while_exit_51:                                    ; preds = %while_cond_49
  %loadtmp = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D9\81\D8\AA\D8\AD"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD") {
entry:
  %"%\D9\86\D9\88\D8\B9_raw" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%fd" = alloca i64, align 8
  %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD4" = alloca i64, align 8
  store i64 %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD", ptr %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD4", align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  br i1 true, label %then_54, label %merge_55

else_62:                                          ; preds = %then_58
  ret i64 -1

else_69:                                          ; preds = %merge_66
  %"%fd.load44" = load i64, ptr %"%fd", align 8
  %arr.data45 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 %"%fd.load44"
  store i64 0, ptr %arr.elem46, align 8
  br label %merge_68

merge_55:                                         ; preds = %entry
  %calltmp = call i64 @"\D8\AE\D8\B5\D8\B5_\D9\88\D8\A7\D8\B5\D9\81"()
  store i64 %calltmp, ptr %"%fd", align 8
  %"%fd.load" = load i64, ptr %"%fd", align 8
  %cmplttmp = icmp slt i64 %"%fd.load", 0
  br i1 %cmplttmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %calltmp5 = call i64 @"\D8\AD\D9\84\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load")
  store i64 %calltmp5, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmplttmp6 = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp6, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_61, %merge_57
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load8" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load7", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load8")
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpeqtmp9, label %then_65, label %merge_66

merge_61:                                         ; preds = %merge_64
  br label %merge_59

merge_64:                                         ; preds = %then_60
  br label %merge_61

merge_66:                                         ; preds = %merge_59
  %"%fd.load12" = load i64, ptr %"%fd", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%fd.load12"
  store i64 1, ptr %arr.elem, align 8
  %"%fd.load13" = load i64, ptr %"%fd", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%fd.load13"
  store i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14", ptr %arr.elem16, align 8
  %"%fd.load17" = load i64, ptr %"%fd", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load18" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%fd.load17"
  store i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load18", ptr %arr.elem20, align 8
  %"%fd.load21" = load i64, ptr %"%fd", align 8
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load22" = load i64, ptr %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD4", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%fd.load21"
  store i64 %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load22", ptr %arr.elem24, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load25" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load25")
  %"%fd.load26" = load i64, ptr %"%fd", align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 %"%fd.load26"
  store i64 0, ptr %arr.elem28, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load29" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\B9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load29")
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %"%\D9\86\D9\88\D8\B9_raw.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %shrtmp30 = lshr i64 %"%\D9\86\D9\88\D8\B9_raw.load", 12
  %"%fd.load31" = load i64, ptr %"%fd", align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 %"%fd.load31"
  store i64 %shrtmp30, ptr %arr.elem33, align 8
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load34" = load i64, ptr %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD4", align 8
  %shrtmp35 = lshr i64 %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load34", 4
  %andtmp36 = and i64 %shrtmp35, 1
  %cmpeqtmp37 = icmp eq i64 %andtmp36, 1
  br i1 %cmpeqtmp37, label %then_67, label %else_69

merge_68:                                         ; preds = %else_69, %then_67
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load47" = load i64, ptr %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD4", align 8
  %shrtmp48 = lshr i64 %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load47", 3
  %andtmp49 = and i64 %shrtmp48, 1
  %cmpeqtmp50 = icmp eq i64 %andtmp49, 1
  br i1 %cmpeqtmp50, label %then_70, label %merge_71

merge_71:                                         ; preds = %then_70, %merge_68
  call void @sad_ll_page_alloc()
  %"%fd.load57" = load i64, ptr %"%fd", align 8
  %arr.data58 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem59 = getelementptr i64, ptr %arr.data58, i64 %"%fd.load57"
  store i64 0, ptr %arr.elem59, align 8
  %"%fd.load60" = load i64, ptr %"%fd", align 8
  %arr.data61 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 %"%fd.load60"
  %arr.get63 = load i64, ptr %arr.elem62, align 8
  %cmpeqtmp64 = icmp eq i64 %arr.get63, 0
  br i1 %cmpeqtmp64, label %then_72, label %merge_73

merge_73:                                         ; preds = %merge_71
  %"%fd.load68" = load i64, ptr %"%fd", align 8
  %arr.data69 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem70 = getelementptr i64, ptr %arr.data69, i64 %"%fd.load68"
  %arr.get71 = load i64, ptr %arr.elem70, align 8
  call void @sad_ll_memset(i64 %arr.get71, i64 0, i64 4096)
  %"%fd.load72" = load i64, ptr %"%fd", align 8
  %arr.data73 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 %"%fd.load72"
  store i64 -1, ptr %arr.elem74, align 8
  %loadtmp = load i64, ptr %"%fd", align 8
  ret i64 %loadtmp

then_54:                                          ; preds = %entry
  ret i64 -1

then_56:                                          ; preds = %merge_55
  ret i64 -1

then_58:                                          ; preds = %merge_57
  %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load" = load i64, ptr %"\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD4", align 8
  %shrtmp = lshr i64 %"%\D9\88\D8\B6\D8\B9_\D8\A7\D9\84\D9\81\D8\AA\D8\AD.load", 2
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_60, label %else_62

then_60:                                          ; preds = %then_58
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load10" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmplttmp11 = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load10", 0
  br i1 %cmplttmp11, label %then_63, label %merge_64

then_63:                                          ; preds = %then_60
  ret i64 -1

then_65:                                          ; preds = %merge_59
  ret i64 -1

then_67:                                          ; preds = %merge_66
  %"%fd.load38" = load i64, ptr %"%fd", align 8
  %arr.data39 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 %"%fd.load38"
  %arr.get = load i64, ptr %arr.elem40, align 8
  %"%fd.load41" = load i64, ptr %"%fd", align 8
  %arr.data42 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 %"%fd.load41"
  store i64 %arr.get, ptr %arr.elem43, align 8
  br label %merge_68

then_70:                                          ; preds = %merge_68
  %"%fd.load51" = load i64, ptr %"%fd", align 8
  %arr.data52 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 %"%fd.load51"
  store i64 0, ptr %arr.elem53, align 8
  %"%fd.load54" = load i64, ptr %"%fd", align 8
  %arr.data55 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 %"%fd.load54"
  store i64 0, ptr %arr.elem56, align 8
  br label %merge_71

then_72:                                          ; preds = %merge_71
  %"%fd.load65" = load i64, ptr %"%fd", align 8
  %arr.data66 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem67 = getelementptr i64, ptr %arr.data66, i64 %"%fd.load65"
  store i64 0, ptr %arr.elem67, align 8
  ret i64 -1
}

define i64 @"\D8\A3\D8\BA\D9\84\D9\82"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %then_74, label %merge_75

merge_75:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load2", 0
  br i1 %cmpgetmp, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %andtmp = and i64 %arr.get7, 3
  %cmpgetmp8 = icmp sge i64 %andtmp, 2
  br i1 %cmpgetmp8, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_83, %merge_79
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load17" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  call void @"\D8\AD\D8\B1\D8\B1_\D9\88\D8\A7\D8\B5\D9\81"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load17")
  ret i64 0

merge_83:                                         ; preds = %then_82, %then_80
  br label %merge_81

then_74:                                          ; preds = %entry
  ret i64 -1

then_76:                                          ; preds = %merge_75
  ret i64 -1

then_78:                                          ; preds = %merge_77
  ret i64 -1

then_80:                                          ; preds = %merge_79
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load9" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load13" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %arr.get12, i64 %arr.get16)
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_82, label %merge_83

then_82:                                          ; preds = %then_80
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load18" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load18", 12
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load19" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load19"
  %arr.get22 = load i64, ptr %arr.elem21, align 8
  call void @sad_ll_mem_write64(i64 %addtmp, i64 %arr.get22)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load23" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load23"
  %arr.get26 = load i64, ptr %arr.elem25, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load27" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load27"
  %arr.get30 = load i64, ptr %arr.elem29, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load31" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %arr.get26, i64 %arr.get30, i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load31")
  br label %merge_83
}

define i64 @"fop_\D8\AD\D9\85\D9\84_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", i64 %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A" = alloca i64, align 8
  %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A92", align 8
  %cmpeqtmp = icmp eq i64 %arr.get, %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load"
  br i1 %cmpeqtmp, label %then_84, label %merge_85

merge_85:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  store i64 %arr.get6, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  store i64 %arr.get10, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A.load")
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp11 = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpeqtmp11, label %then_86, label %merge_87

merge_87:                                         ; preds = %merge_85
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A.load12" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load13" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load14" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A92", align 8
  call void @"\D8\AA\D8\B1\D8\AC\D9\85_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84_\D8\A7\D9\84\D9\85\D8\AD\D9\84\D9\8A.load12", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load13", i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load14")
  store i64 0, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9.load", 0
  br i1 %cmpeqtmp15, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_90

merge_94:                                         ; preds = %while_body_91
  %"%\D9\82.load37" = load i64, ptr %"%\D9\82", align 8
  %addtmp38 = add i64 %"%\D9\82.load37", 1
  store i64 %addtmp38, ptr %"%\D9\82", align 8
  br label %while_cond_90

then_84:                                          ; preds = %entry
  ret i64 0

then_86:                                          ; preds = %merge_85
  ret i64 -1

then_88:                                          ; preds = %merge_87
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load16" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load17" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A92", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load16"
  store i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load17", ptr %arr.elem19, align 8
  ret i64 0

then_93:                                          ; preds = %while_body_91
  ret i64 -1

while_body_91:                                    ; preds = %while_cond_90
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9.load20" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9", align 8
  %multmp = mul i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A\D8\A9.load20", 0
  %"%\D9\82.load21" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %multmp, %"%\D9\82.load21"
  store i64 %addtmp, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load22" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load22"
  %arr.get25 = load i64, ptr %arr.elem24, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load26" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load26"
  %arr.get29 = load i64, ptr %arr.elem28, align 8
  %"%\D9\82.load30" = load i64, ptr %"%\D9\82", align 8
  %multmp31 = mul i64 %"%\D9\82.load30", 0
  %addtmp32 = add i64 %arr.get29, %multmp31
  %"%\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  call void @"blk_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %arr.get25, i64 %"%\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %addtmp32)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_93, label %merge_94

while_cond_90:                                    ; preds = %merge_94, %merge_89
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp, label %while_body_91, label %while_exit_92

while_exit_92:                                    ; preds = %while_cond_90
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load33" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load34" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A92", align 8
  %arr.data35 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load33"
  store i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load34", ptr %arr.elem36, align 8
  ret i64 0
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA") {
entry:
  %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AA\D8\A7\D8\AD" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1" = alloca i64, align 8
  %"%\D8\A8\D9\82\D9\8A\D8\A9" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %then_95, label %merge_96

merge_100:                                        ; preds = %merge_98
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %andtmp = and i64 %arr.get9, 3
  %cmpeqtmp10 = icmp eq i64 %andtmp, 2
  br i1 %cmpeqtmp10, label %then_101, label %merge_102

merge_102:                                        ; preds = %merge_100
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load11"
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load15" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load15"
  %arr.get18 = load i64, ptr %arr.elem17, align 8
  %cmpgetmp19 = icmp sge i64 %arr.get14, %arr.get18
  br i1 %cmpgetmp19, label %then_103, label %merge_104

merge_104:                                        ; preds = %merge_102
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load20" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load20"
  %arr.get23 = load i64, ptr %arr.elem22, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load24" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load24"
  %arr.get27 = load i64, ptr %arr.elem26, align 8
  %subtmp = sub i64 %arr.get23, %arr.get27
  store i64 %subtmp, ptr %"%\D8\A8\D9\82\D9\8A\D8\A9", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3", align 8
  %"%\D8\A8\D9\82\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\A8\D9\82\D9\8A\D8\A9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load", %"%\D8\A8\D9\82\D9\8A\D8\A9.load"
  br i1 %cmpgttmp, label %then_105, label %merge_106

merge_106:                                        ; preds = %then_105, %merge_104
  store i64 0, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  br label %while_cond_107

merge_111:                                        ; preds = %while_body_108
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %subtmp37 = sub i64 0, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load"
  store i64 %subtmp37, ptr %"%\D9\85\D8\AA\D8\A7\D8\AD", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load38" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3", align 8
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load39" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %subtmp40 = sub i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load38", %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load39"
  store i64 %subtmp40, ptr %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %"%\D9\85\D8\AA\D8\A7\D8\AD", align 8
  %cmpgttmp41 = icmp sgt i64 %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", %"%\D9\85\D8\AA\D8\A7\D8\AD.load"
  br i1 %cmpgttmp41, label %then_112, label %merge_113

merge_113:                                        ; preds = %then_112, %merge_111
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A92", align 8
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load43" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load", %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load43"
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load44" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data45 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load44"
  %arr.get47 = load i64, ptr %arr.elem46, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load48" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %addtmp49 = add i64 %arr.get47, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load48"
  %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load50" = load i64, ptr %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_memcpy(i64 %addtmp, i64 %addtmp49, i64 %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load50")
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load51" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load52" = load i64, ptr %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %addtmp53 = add i64 %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load51", %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load52"
  store i64 %addtmp53, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load54" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data55 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load54"
  %arr.get57 = load i64, ptr %arr.elem56, align 8
  %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load58" = load i64, ptr %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %addtmp59 = add i64 %arr.get57, %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load58"
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load60" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data61 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load60"
  store i64 %addtmp59, ptr %arr.elem62, align 8
  br label %while_cond_107

merge_96:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4", 0
  br i1 %cmpgetmp, label %then_97, label %merge_98

merge_98:                                         ; preds = %merge_96
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_99, label %merge_100

then_101:                                         ; preds = %merge_100
  ret i64 -1

then_103:                                         ; preds = %merge_102
  ret i64 0

then_105:                                         ; preds = %merge_104
  br label %merge_106

then_110:                                         ; preds = %while_body_108
  %loadtmp36 = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp36

then_112:                                         ; preds = %merge_111
  %"%\D9\85\D8\AA\D8\A7\D8\AD.load42" = load i64, ptr %"%\D9\85\D8\AA\D8\A7\D8\AD", align 8
  store i64 %"%\D9\85\D8\AA\D8\A7\D8\AD.load42", ptr %"%\D9\84\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  br label %merge_113

then_95:                                          ; preds = %entry
  ret i64 -1

then_97:                                          ; preds = %merge_96
  ret i64 -1

then_99:                                          ; preds = %merge_98
  ret i64 -1

while_body_108:                                   ; preds = %while_cond_107
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load30" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load30"
  %arr.get33 = load i64, ptr %arr.elem32, align 8
  store i64 %arr.get33, ptr %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load", 0
  store i64 %divtmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load34" = load i64, ptr %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %modtmp = srem i64 %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load34", 0
  store i64 %modtmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load35" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %calltmp = call i64 @"fop_\D8\AD\D9\85\D9\84_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load35", i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_110, label %merge_111

while_cond_107:                                   ; preds = %merge_113, %merge_106
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load28" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3", align 8
  %cmplttmp29 = icmp slt i64 %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load28"
  br i1 %cmplttmp29, label %while_body_108, label %while_exit_109

while_exit_109:                                   ; preds = %while_cond_107
  %loadtmp = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA") {
entry:
  %"%\D9\82\D8\B7\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_f" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AA\D8\A7\D8\AD" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %then_114, label %merge_115

merge_115:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4", 0
  br i1 %cmpgetmp, label %then_116, label %merge_117

merge_117:                                        ; preds = %merge_115
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_118, label %merge_119

merge_119:                                        ; preds = %merge_117
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %andtmp = and i64 %arr.get9, 3
  %cmpeqtmp10 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp10, label %then_120, label %merge_121

merge_121:                                        ; preds = %merge_119
  store i64 0, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  br label %while_cond_122

merge_126:                                        ; preds = %while_body_123
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %subtmp = sub i64 0, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load"
  store i64 %subtmp, ptr %"%\D9\85\D8\AA\D8\A7\D8\AD", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load19" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3", align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load20" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %subtmp21 = sub i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load19", %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load20"
  store i64 %subtmp21, ptr %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load" = load i64, ptr %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %"%\D9\85\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %"%\D9\85\D8\AA\D8\A7\D8\AD", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load", %"%\D9\85\D8\AA\D8\A7\D8\AD.load"
  br i1 %cmpgttmp, label %then_127, label %merge_128

merge_128:                                        ; preds = %then_127, %merge_126
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load23" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load23"
  %arr.get26 = load i64, ptr %arr.elem25, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load27" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %addtmp = add i64 %arr.get26, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load27"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B12", align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load28" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %addtmp29 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load", %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load28"
  %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load30" = load i64, ptr %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  call void @sad_ll_memcpy(i64 %addtmp, i64 %addtmp29, i64 %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load30")
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_129

merge_133:                                        ; preds = %then_132, %while_exit_131
  br label %while_cond_122

then_114:                                         ; preds = %entry
  ret i64 -1

then_116:                                         ; preds = %merge_115
  ret i64 -1

then_118:                                         ; preds = %merge_117
  ret i64 -1

then_120:                                         ; preds = %merge_119
  ret i64 -1

then_125:                                         ; preds = %while_body_123
  %loadtmp18 = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp18

then_127:                                         ; preds = %merge_126
  %"%\D9\85\D8\AA\D8\A7\D8\AD.load22" = load i64, ptr %"%\D9\85\D8\AA\D8\A7\D8\AD", align 8
  store i64 %"%\D9\85\D8\AA\D8\A7\D8\AD.load22", ptr %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  br label %merge_128

then_132:                                         ; preds = %while_exit_131
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load72" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data73 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load72"
  %arr.get75 = load i64, ptr %arr.elem74, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load76" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data77 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem78 = getelementptr i64, ptr %arr.data77, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load76"
  store i64 %arr.get75, ptr %arr.elem78, align 8
  br label %merge_133

while_body_123:                                   ; preds = %while_cond_122
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load12" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load12"
  %arr.get15 = load i64, ptr %arr.elem14, align 8
  store i64 %arr.get15, ptr %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load" = load i64, ptr %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load", 0
  store i64 %divtmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load16" = load i64, ptr %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  %modtmp = srem i64 %"%\D8\A7\D9\84\D9\85\D9\88\D9\82\D8\B9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A.load16", 0
  store i64 %modtmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load17" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %calltmp = call i64 @"fop_\D8\AD\D9\85\D9\84_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load17", i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_125, label %merge_126

while_body_130:                                   ; preds = %while_cond_129
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load32" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data33 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load32"
  %arr.get35 = load i64, ptr %arr.elem34, align 8
  store i64 %arr.get35, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_f", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_f.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_f", align 8
  %multmp = mul i64 %"%\D9\83\D8\AA\D9\84\D8\A9_f.load", 0
  %"%\D9\82.load36" = load i64, ptr %"%\D9\82", align 8
  %addtmp37 = add i64 %multmp, %"%\D9\82.load36"
  store i64 %addtmp37, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load38" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data39 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load38"
  %arr.get41 = load i64, ptr %arr.elem40, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load42" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data43 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load42"
  %arr.get45 = load i64, ptr %arr.elem44, align 8
  %"%\D9\82.load46" = load i64, ptr %"%\D9\82", align 8
  %multmp47 = mul i64 %"%\D9\82.load46", 0
  %addtmp48 = add i64 %arr.get45, %multmp47
  %"%\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  call void @"blk_\D8\A7\D9\83\D8\AA\D8\A8"(i64 %arr.get41, i64 %"%\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %addtmp48)
  %"%\D9\82.load49" = load i64, ptr %"%\D9\82", align 8
  %addtmp50 = add i64 %"%\D9\82.load49", 1
  store i64 %addtmp50, ptr %"%\D9\82", align 8
  br label %while_cond_129

while_cond_122:                                   ; preds = %merge_133, %merge_121
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA3", align 8
  %cmplttmp11 = icmp slt i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA.load"
  br i1 %cmplttmp11, label %while_body_123, label %while_exit_124

while_cond_129:                                   ; preds = %while_body_130, %merge_128
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp31 = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp31, label %while_body_130, label %while_exit_131

while_exit_124:                                   ; preds = %while_cond_122
  %loadtmp = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp

while_exit_131:                                   ; preds = %while_cond_129
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load51" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load52" = load i64, ptr %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %addtmp53 = add i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load51", %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load52"
  store i64 %addtmp53, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load54" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data55 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load54"
  %arr.get57 = load i64, ptr %arr.elem56, align 8
  %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load58" = load i64, ptr %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %addtmp59 = add i64 %arr.get57, %"%\D9\84\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load58"
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load60" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data61 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load60"
  store i64 %addtmp59, ptr %arr.elem62, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load63" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data64 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load63"
  %arr.get66 = load i64, ptr %arr.elem65, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load67" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data68 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem69 = getelementptr i64, ptr %arr.data68, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load67"
  %arr.get70 = load i64, ptr %arr.elem69, align 8
  %cmpgttmp71 = icmp sgt i64 %arr.get66, %arr.get70
  br i1 %cmpgttmp71, label %then_132, label %merge_133
}

define i64 @"\D8\AD\D8\B1\D9\83"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", i64 %"\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", i64 %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9") {
entry:
  %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF" = alloca i64, align 8
  %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93" = alloca i64, align 8
  store i64 %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93", align 8
  %"\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", ptr %"\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %then_134, label %merge_135

merge_135:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4", 0
  br i1 %cmpgetmp, label %then_136, label %merge_137

merge_137:                                        ; preds = %merge_135
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  store i64 0, ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", 0
  br i1 %cmpeqtmp6, label %then_140, label %merge_141

merge_141:                                        ; preds = %then_140, %merge_139
  %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load7" = load i64, ptr %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load7", 0
  br i1 %cmpeqtmp8, label %then_142, label %merge_143

merge_143:                                        ; preds = %then_142, %merge_141
  %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load14" = load i64, ptr %"\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A93", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\86\D9\82\D8\B7\D8\A9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load14", 0
  br i1 %cmpeqtmp15, label %then_144, label %merge_145

merge_145:                                        ; preds = %then_144, %merge_143
  %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF.load" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  %cmplttmp22 = icmp slt i64 %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF.load", 0
  br i1 %cmplttmp22, label %then_146, label %merge_147

merge_147:                                        ; preds = %then_146, %merge_145
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load23" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF.load24" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load23"
  store i64 %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF.load24", ptr %arr.elem26, align 8
  %loadtmp = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  ret i64 %loadtmp

then_134:                                         ; preds = %entry
  ret i64 -1

then_136:                                         ; preds = %merge_135
  ret i64 -1

then_138:                                         ; preds = %merge_137
  ret i64 -1

then_140:                                         ; preds = %merge_139
  %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92", align 8
  store i64 %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load", ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  br label %merge_141

then_142:                                         ; preds = %merge_141
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load9" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load13" = load i64, ptr %"\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92", align 8
  %addtmp = add i64 %arr.get12, %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load13"
  store i64 %addtmp, ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load16" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load16"
  %arr.get19 = load i64, ptr %arr.elem18, align 8
  %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load20" = load i64, ptr %"\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92", align 8
  %addtmp21 = add i64 %arr.get19, %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load20"
  store i64 %addtmp21, ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  br label %merge_145

then_146:                                         ; preds = %merge_145
  store i64 0, ptr %"%\D9\85\D9\88\D9\82\D8\B9_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  br label %merge_147
}

define i64 @"\D8\A3\D8\AE\D8\A8\D8\B1"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %then_148, label %merge_149

merge_149:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load2", 0
  br i1 %cmpgetmp, label %then_150, label %merge_151

merge_151:                                        ; preds = %merge_149
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_152, label %merge_153

merge_153:                                        ; preds = %merge_151
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  ret i64 %arr.get7

then_148:                                         ; preds = %entry
  ret i64 -1

then_150:                                         ; preds = %merge_149
  ret i64 -1

then_152:                                         ; preds = %merge_151
  ret i64 -1
}

define i64 @"\D8\A3\D9\86\D8\B4\D8\A6_\D9\85\D9\84\D9\81_\D8\AF\D8\A7\D8\AE\D9\84\D9\8A"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81") {
entry:
  %"%\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\88\D8\AC\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8" = alloca i64, align 8
  %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81", ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814", align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %calltmp = call i64 @"\D8\AD\D9\84\D9\84_\D9\85\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A3\D8\A8"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load")
  store i64 %calltmp, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8.load", 0
  br i1 %cmplttmp, label %then_154, label %merge_155

merge_155:                                        ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load5" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load6" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  call void @"\D9\82\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load5", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load6")
  br i1 true, label %then_156, label %merge_157

merge_157:                                        ; preds = %merge_155
  store i64 -1, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load7" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load7", %arr.get
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load8" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load8"
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  store i64 %arr.get11, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load12" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8.load13" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  call void @"\D8\A7\D8\A8\D8\AD\D8\AB_\D9\81\D9\8A_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load12", i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8.load13", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load")
  store i64 0, ptr %"%\D9\85\D9\88\D8\AC\D9\88\D8\AF", align 8
  %"%\D9\85\D9\88\D8\AC\D9\88\D8\AF.load" = load i64, ptr %"%\D9\85\D9\88\D8\AC\D9\88\D8\AF", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\85\D9\88\D8\AC\D9\88\D8\AF.load", 0
  br i1 %cmpgetmp, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_157
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  call void @"\D8\AE\D8\B5\D8\B5_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14")
  store i64 0, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %cmplttmp15 = icmp slt i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp15, label %then_160, label %merge_161

merge_161:                                        ; preds = %merge_159
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load", 0
  br i1 %cmpeqtmp, label %then_162, label %merge_163

merge_163:                                        ; preds = %then_162, %merge_161
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load16" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814", align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load16", 0
  br i1 %cmpeqtmp17, label %then_164, label %merge_165

merge_165:                                        ; preds = %then_164, %merge_163
  call void @sad_ll_mem_write16(i64 2, i64 1)
  call void @sad_ll_mem_write64(i64 12, i64 0)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load18" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8.load19" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load20" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load21" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load22" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load23" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", align 8
  call void @"\D8\A3\D8\B6\D9\81_\D9\85\D8\AF\D8\AE\D9\84\D8\A9_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load18", i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\A3\D8\A8.load19", i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load20", i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load21", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load22", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load23")
  %loadtmp24 = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  ret i64 %loadtmp24

then_154:                                         ; preds = %entry
  ret i64 -1

then_156:                                         ; preds = %merge_155
  ret i64 -1

then_158:                                         ; preds = %merge_157
  %loadtmp = load i64, ptr %"%\D9\85\D9\88\D8\AC\D9\88\D8\AF", align 8
  ret i64 %loadtmp

then_160:                                         ; preds = %merge_159
  ret i64 -1

then_162:                                         ; preds = %merge_161
  store i64 420, ptr %"%\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA", align 8
  br label %merge_163

then_164:                                         ; preds = %merge_163
  store i64 493, ptr %"%\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA", align 8
  br label %merge_165
}

define i64 @"\D8\A3\D9\86\D8\B4\D8\A6_\D9\85\D9\84\D9\81"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1") {
entry:
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  ret i64 0
}

define i64 @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1") {
entry:
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  ret i64 0
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\B9\D9\84\D9\85"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9") {
entry:
  %"%\D9\86\D9\88\D8\B9_raw" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %calltmp = call i64 @"\D8\AD\D9\84\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load")
  store i64 %calltmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp, label %then_166, label %merge_167

merge_167:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load6" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load6")
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_168, label %merge_169

merge_169:                                        ; preds = %merge_167
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  call void @sad_ll_memset(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", i64 0, i64 64)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load7" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load8" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @sad_ll_mem_write32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load7", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load8")
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load9" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\B9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load9")
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load10" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load10", 4
  %"%\D9\86\D9\88\D8\B9_raw.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %shrtmp = lshr i64 %"%\D9\86\D9\88\D8\B9_raw.load", 12
  call void @sad_ll_mem_write16(i64 %addtmp, i64 %shrtmp)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load11" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %addtmp12 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load11", 6
  %"%\D9\86\D9\88\D8\B9_raw.load13" = load i64, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %andtmp = and i64 %"%\D9\86\D9\88\D8\B9_raw.load13", 4095
  call void @sad_ll_mem_write16(i64 %addtmp12, i64 %andtmp)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load14" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %addtmp15 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load14", 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load16" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load16")
  call void @sad_ll_mem_write64(i64 %addtmp15, i64 0)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load17" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %addtmp18 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load17", 16
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load19" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B7"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load19")
  call void @"\D9\81op_\D8\A7\D9\83\D8\AA\D8\A8_\D8\B0\D8\A7\D9\83\D8\B1\D8\A932"(i64 %addtmp18, i64 0)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load20" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %addtmp21 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load20", 20
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load22" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %addtmp23 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load22", 4
  call void @sad_ll_mem_read32(i64 %addtmp23)
  call void @sad_ll_mem_write32(i64 %addtmp21, i64 0)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load24" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %addtmp25 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load24", 24
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load26" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %addtmp27 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load26", 8
  call void @sad_ll_mem_read32(i64 %addtmp27)
  call void @sad_ll_mem_write32(i64 %addtmp25, i64 0)
  ret i64 0

then_166:                                         ; preds = %entry
  ret i64 -1

then_168:                                         ; preds = %merge_167
  ret i64 -1
}

define i64 @"\D8\A7\D8\B3\D8\AA\D8\B9\D9\84\D9\85_\D9\88\D8\A7\D8\B5\D9\81"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %then_170, label %merge_171

merge_171:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load3", 0
  br i1 %cmpgetmp, label %then_172, label %merge_173

merge_173:                                        ; preds = %merge_171
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_174, label %merge_175

merge_175:                                        ; preds = %merge_173
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  call void @sad_ll_memset(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", i64 0, i64 64)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load5"
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load9" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  call void @sad_ll_mem_write32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load9", i64 %arr.get8)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load10" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load10", 4
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load11"
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  call void @sad_ll_mem_write16(i64 %addtmp, i64 %arr.get14)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load15" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %addtmp16 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load15", 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load17" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\811", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\88\D8\A7\D8\B5\D9\81.load17"
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  call void @sad_ll_mem_write64(i64 %addtmp16, i64 %arr.get20)
  ret i64 0

then_170:                                         ; preds = %entry
  ret i64 -1

then_172:                                         ; preds = %merge_171
  ret i64 -1

then_174:                                         ; preds = %merge_173
  ret i64 -1
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", i64 %"\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA") {
entry:
  %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE" = alloca i64, align 8
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D9\85\D8\AF" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_raw" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA4" = alloca i64, align 8
  store i64 %"\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA", ptr %"\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA4", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A93", align 8
  %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load")
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_176, label %merge_177

merge_177:                                        ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load5" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\B9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load5")
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %"%\D9\86\D9\88\D8\B9_raw.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %shrtmp = lshr i64 %"%\D9\86\D9\88\D8\B9_raw.load", 12
  %cmpnetmp = icmp ne i64 %shrtmp, 0
  br i1 %cmpnetmp, label %then_178, label %merge_179

merge_179:                                        ; preds = %merge_177
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load6" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load6")
  store i64 0, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 12, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 12, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 12))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep7 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data8 = load ptr, ptr %arr.data.gep7, align 8
  %arr.elem = getelementptr i64, ptr %arr.data8, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 1
  store i64 0, ptr %arr.elem11, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 2
  store i64 0, ptr %arr.elem14, align 8
  %arr.data.gep15 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data16 = load ptr, ptr %arr.data.gep15, align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 3
  store i64 0, ptr %arr.elem17, align 8
  %arr.data.gep18 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data19 = load ptr, ptr %arr.data.gep18, align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 4
  store i64 0, ptr %arr.elem20, align 8
  %arr.data.gep21 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data22 = load ptr, ptr %arr.data.gep21, align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 5
  store i64 0, ptr %arr.elem23, align 8
  %arr.data.gep24 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data25 = load ptr, ptr %arr.data.gep24, align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 6
  store i64 0, ptr %arr.elem26, align 8
  %arr.data.gep27 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data28 = load ptr, ptr %arr.data.gep27, align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 7
  store i64 0, ptr %arr.elem29, align 8
  %arr.data.gep30 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data31 = load ptr, ptr %arr.data.gep30, align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 8
  store i64 0, ptr %arr.elem32, align 8
  %arr.data.gep33 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data34 = load ptr, ptr %arr.data.gep33, align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 9
  store i64 0, ptr %arr.elem35, align 8
  %arr.data.gep36 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data37 = load ptr, ptr %arr.data.gep36, align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 10
  store i64 0, ptr %arr.elem38, align 8
  %arr.data.gep39 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data40 = load ptr, ptr %arr.data.gep39, align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 11
  store i64 0, ptr %arr.elem41, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_180

merge_184:                                        ; preds = %then_183, %while_exit_182
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_185

merge_189:                                        ; preds = %while_body_186
  %"%\D9\83.load57" = load i64, ptr %"%\D9\83", align 8
  %arr.ptr.int58 = load i64, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %arr.ptr59 = inttoptr i64 %arr.ptr.int58 to ptr
  %arr.data.gep60 = getelementptr inbounds %SadArray, ptr %arr.ptr59, i32 0, i32 2
  %arr.data61 = load ptr, ptr %arr.data.gep60, align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 %"%\D9\83.load57"
  %arr.get = load i64, ptr %arr.elem62, align 8
  %cmpeqtmp63 = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp63, label %then_191, label %merge_192

merge_192:                                        ; preds = %merge_189
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_194

merge_201:                                        ; preds = %while_body_198
  %"%\D9\85\D8\AF.load86" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %shltmp = shl i64 %"%\D9\85\D8\AF.load86", 6
  %addtmp87 = add i64 0, %shltmp
  store i64 %addtmp87, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  call void @sad_ll_mem_read32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load")
  store i64 0, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF", align 8
  %cmpeqtmp88 = icmp eq i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF.load", 0
  br i1 %cmpeqtmp88, label %then_202, label %merge_203

merge_203:                                        ; preds = %merge_201
  %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9.load91" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9", align 8
  %shltmp92 = shl i64 %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9.load91", 5
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A93", align 8
  %addtmp93 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", %shltmp92
  store i64 %addtmp93, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF.load94" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF", align 8
  call void @sad_ll_mem_write32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", i64 %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF.load94")
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load95" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %addtmp96 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load95", 4
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load97" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %addtmp98 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load97", 6
  call void @sad_ll_mem_read16(i64 %addtmp98)
  call void @"\D9\81op_\D8\A7\D9\83\D8\AA\D8\A8_\D8\B0\D8\A7\D9\83\D8\B1\D8\A916"(i64 %addtmp96, i64 0)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load99" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %addtmp100 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load99", 4
  call void @sad_ll_mem_read16(i64 %addtmp100)
  store i64 0, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load101" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %addtmp102 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load101", 6
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF", align 8
  call void @sad_ll_mem_write16(i64 %addtmp102, i64 %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load")
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load103" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF", align 8
  store i64 %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load103", ptr %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE", align 8
  %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE.load", 24
  br i1 %cmpgttmp, label %then_205, label %merge_206

merge_206:                                        ; preds = %then_205, %merge_203
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load104" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %addtmp105 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load104", 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load106" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %addtmp107 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load106", 8
  %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE.load108" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE", align 8
  call void @sad_ll_memcpy(i64 %addtmp105, i64 %addtmp107, i64 %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE.load108")
  %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9.load109" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9", align 8
  %addtmp110 = add i64 %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9.load109", 1
  store i64 %addtmp110, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9", align 8
  %"%\D9\85\D8\AF.load111" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %addtmp112 = add i64 %"%\D9\85\D8\AF.load111", 1
  store i64 %addtmp112, ptr %"%\D9\85\D8\AF", align 8
  br label %while_cond_197

then_176:                                         ; preds = %entry
  ret i64 0

then_178:                                         ; preds = %merge_177
  ret i64 0

then_183:                                         ; preds = %while_exit_182
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84", align 8
  %addtmp52 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84.load", 1
  store i64 %addtmp52, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84", align 8
  br label %merge_184

then_188:                                         ; preds = %while_body_186
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84.load56" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84", align 8
  store i64 %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84.load56", ptr %"%\D9\83", align 8
  br label %while_cond_185

then_191:                                         ; preds = %merge_189
  %"%\D9\83.load64" = load i64, ptr %"%\D9\83", align 8
  %addtmp65 = add i64 %"%\D9\83.load64", 1
  store i64 %addtmp65, ptr %"%\D9\83", align 8
  br label %while_cond_185

then_200:                                         ; preds = %while_body_198
  %loadtmp85 = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9", align 8
  ret i64 %loadtmp85

then_202:                                         ; preds = %merge_201
  %"%\D9\85\D8\AF.load89" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %addtmp90 = add i64 %"%\D9\85\D8\AF.load89", 1
  store i64 %addtmp90, ptr %"%\D9\85\D8\AF", align 8
  br label %while_cond_197

then_205:                                         ; preds = %merge_203
  store i64 24, ptr %"%\D8\B7\D9\88\D9\84_\D9\86\D8\B3\D8\AE", align 8
  br label %merge_206

while_body_181:                                   ; preds = %while_cond_180
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load42" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D9\85.load43" = load i64, ptr %"%\D9\85", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load42", i64 %"%\D9\85.load43")
  %"%\D9\85.load44" = load i64, ptr %"%\D9\85", align 8
  %arr.ptr.int = load i64, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep45 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data46 = load ptr, ptr %arr.data.gep45, align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 %"%\D9\85.load44"
  store i64 0, ptr %arr.elem47, align 8
  %"%\D9\85.load48" = load i64, ptr %"%\D9\85", align 8
  %addtmp = add i64 %"%\D9\85.load48", 1
  store i64 %addtmp, ptr %"%\D9\85", align 8
  br label %while_cond_180

while_body_186:                                   ; preds = %while_cond_185
  %"%\D9\83.load55" = load i64, ptr %"%\D9\83", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\83.load55", 12
  br i1 %cmpgetmp, label %then_188, label %merge_189

while_body_195:                                   ; preds = %while_cond_194
  %"%\D9\83.load67" = load i64, ptr %"%\D9\83", align 8
  %arr.ptr.int68 = load i64, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %arr.ptr69 = inttoptr i64 %arr.ptr.int68 to ptr
  %arr.data.gep70 = getelementptr inbounds %SadArray, ptr %arr.ptr69, i32 0, i32 2
  %arr.data71 = load ptr, ptr %arr.data.gep70, align 8
  %arr.elem72 = getelementptr i64, ptr %arr.data71, i64 %"%\D9\83.load67"
  %arr.get73 = load i64, ptr %arr.elem72, align 8
  %multmp = mul i64 %arr.get73, 0
  %"%\D9\82.load74" = load i64, ptr %"%\D9\82", align 8
  %addtmp75 = add i64 %multmp, %"%\D9\82.load74"
  store i64 %addtmp75, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  %"%\D9\82.load76" = load i64, ptr %"%\D9\82", align 8
  %multmp77 = mul i64 %"%\D9\82.load76", 0
  %addtmp78 = add i64 0, %multmp77
  %"%\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  call void @"blk_\D8\A7\D9\82\D8\B1\D8\A3"(i64 0, i64 %"%\D9\82\D8\B7\D8\A7\D8\B9.load", i64 %addtmp78)
  %"%\D9\82.load79" = load i64, ptr %"%\D9\82", align 8
  %addtmp80 = add i64 %"%\D9\82.load79", 1
  store i64 %addtmp80, ptr %"%\D9\82", align 8
  br label %while_cond_194

while_body_198:                                   ; preds = %while_cond_197
  %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9", align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA4", align 8
  %cmpgetmp82 = icmp sge i64 %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9.load", %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA.load"
  br i1 %cmpgetmp82, label %then_200, label %merge_201

while_cond_180:                                   ; preds = %while_body_181, %merge_179
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", 12
  br i1 %cmplttmp, label %while_body_181, label %while_exit_182

while_cond_185:                                   ; preds = %while_exit_199, %then_191, %then_188, %merge_184
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84.load53" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84", align 8
  %cmplttmp54 = icmp slt i64 %"%\D9\83.load", %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84.load53"
  br i1 %cmplttmp54, label %while_body_186, label %while_exit_187

while_cond_194:                                   ; preds = %while_body_195, %merge_192
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp66 = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp66, label %while_body_195, label %while_exit_196

while_cond_197:                                   ; preds = %merge_206, %then_202, %while_exit_196
  %"%\D9\85\D8\AF.load" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %cmplttmp81 = icmp slt i64 %"%\D9\85\D8\AF.load", 64
  br i1 %cmplttmp81, label %while_body_198, label %while_exit_199

while_exit_182:                                   ; preds = %while_cond_180
  store i64 0, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %shrtmp49 = lshr i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load", 12
  store i64 %shrtmp49, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load50" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %andtmp = and i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load50", 4095
  %cmpnetmp51 = icmp ne i64 %andtmp, 0
  br i1 %cmpnetmp51, label %then_183, label %merge_184

while_exit_187:                                   ; preds = %while_cond_185
  %loadtmp = load i64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\85\D9\82\D8\B1\D9\88\D8\A1\D8\A9", align 8
  ret i64 %loadtmp

while_exit_196:                                   ; preds = %while_cond_194
  store i64 0, ptr %"%\D9\85\D8\AF", align 8
  br label %while_cond_197

while_exit_199:                                   ; preds = %while_cond_197
  %"%\D9\83.load83" = load i64, ptr %"%\D9\83", align 8
  %addtmp84 = add i64 %"%\D9\83.load83", 1
  store i64 %addtmp84, ptr %"%\D9\83", align 8
  br label %while_cond_185
}

define i64 @"\D8\A7\D8\AD\D8\B0\D9\81_\D9\85\D9\84\D9\81"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1") {
entry:
  %"%\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_raw" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B12", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B13", align 8
  %calltmp = call i64 @"\D8\AD\D9\84\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1.load")
  store i64 %calltmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp, label %then_207, label %merge_208

merge_208:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load4" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load4", 0
  br i1 %cmpeqtmp, label %then_209, label %merge_210

merge_210:                                        ; preds = %merge_208
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load6" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load6")
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpeqtmp7, label %then_211, label %merge_212

merge_212:                                        ; preds = %merge_210
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load8" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\B9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load8")
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %"%\D9\86\D9\88\D8\B9_raw.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_raw", align 8
  %shrtmp = lshr i64 %"%\D9\86\D9\88\D8\B9_raw.load", 12
  %cmpeqtmp9 = icmp eq i64 %shrtmp, 0
  br i1 %cmpeqtmp9, label %then_213, label %merge_214

merge_214:                                        ; preds = %merge_212
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_215

merge_219:                                        ; preds = %then_218, %while_body_216
  %"%\D9\85.load17" = load i64, ptr %"%\D9\85", align 8
  %addtmp = add i64 %"%\D9\85.load17", 1
  store i64 %addtmp, ptr %"%\D9\85", align 8
  br label %while_cond_215

then_207:                                         ; preds = %entry
  ret i64 -1

then_209:                                         ; preds = %merge_208
  ret i64 -1

then_211:                                         ; preds = %merge_210
  ret i64 -1

then_213:                                         ; preds = %merge_212
  ret i64 -1

then_218:                                         ; preds = %while_body_216
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load15" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9.load16" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  call void @"\D8\AD\D8\B1\D8\B1_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load15", i64 %"%\D9\83\D8\AA\D9\84\D8\A9.load16")
  br label %merge_219

while_body_216:                                   ; preds = %while_cond_215
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load11" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D9\85.load12" = load i64, ptr %"%\D9\85", align 8
  call void @"\D8\B9\D9\82\D8\AF\D8\A9_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load11", i64 %"%\D9\85.load12")
  store i64 0, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\83\D8\AA\D9\84\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_218, label %merge_219

while_cond_215:                                   ; preds = %merge_219, %merge_214
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp10 = icmp slt i64 %"%\D9\85.load", 12
  br i1 %cmplttmp10, label %while_body_216, label %while_exit_217

while_exit_217:                                   ; preds = %while_cond_215
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load13" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load14" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\AD\D8\B1\D8\B1_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load13", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load14")
  ret i64 0
}

define void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_VFS"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_220

merge_224:                                        ; preds = %then_223, %while_body_221
  %"%\D9\81.load15" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load15", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_220

then_223:                                         ; preds = %while_body_221
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\81.load2")
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get6)
  %"%\D9\81.load7" = load i64, ptr %"%\D9\81", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\81.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get10)
  %"%\D9\81.load11" = load i64, ptr %"%\D9\81", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81.load11"
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get14)
  br label %merge_224

while_body_221:                                   ; preds = %while_cond_220
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 1
  br i1 %cmpeqtmp, label %then_223, label %merge_224

while_cond_220:                                   ; preds = %merge_224, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_221, label %while_exit_222

while_exit_222:                                   ; preds = %while_cond_220
  ret void
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\84\D9\81\D8\A7\D8\AA"() {
entry:
  %"%fd" = alloca i64, align 8
  %"%\D9\86\D8\AA1" = alloca i64, align 8
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC" = alloca i64, align 8
  store i64 0, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %calltmp = call i64 @"\D9\87\D9\8A\D8\A6_\D8\B9\D9\85\D9\84\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\84\D9\81\D8\A7\D8\AA"()
  store i64 %calltmp, ptr %"%\D9\86\D8\AA1", align 8
  %"%\D9\86\D8\AA1.load" = load i64, ptr %"%\D9\86\D8\AA1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA1.load", 0
  br i1 %cmpeqtmp, label %then_225, label %else_227

else_227:                                         ; preds = %entry
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_226

else_230:                                         ; preds = %merge_226
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_229

else_233:                                         ; preds = %merge_229
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_232

else_236:                                         ; preds = %merge_232
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_235

else_239:                                         ; preds = %merge_235
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_238

merge_226:                                        ; preds = %else_227, %then_225
  %calltmp1 = call i64 @"\D8\AE\D8\B5\D8\B5_\D9\88\D8\A7\D8\B5\D9\81"()
  store i64 %calltmp1, ptr %"%fd", align 8
  %"%fd.load" = load i64, ptr %"%fd", align 8
  %cmpgetmp = icmp sge i64 %"%fd.load", 0
  br i1 %cmpgetmp, label %then_228, label %else_230

merge_229:                                        ; preds = %else_230, %then_228
  call void @sad_ll_mem_write16(i64 2, i64 25135)
  br i1 false, label %then_231, label %else_233

merge_232:                                        ; preds = %else_233, %then_231
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 0
  store i64 1, ptr %arr.elem8, align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 0
  store i64 1024, ptr %arr.elem10, align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 0
  store i64 0, ptr %arr.elem12, align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 0
  %arr.get = load i64, ptr %arr.elem14, align 8
  %cmpeqtmp15 = icmp eq i64 %arr.get, 100
  br i1 %cmpeqtmp15, label %then_234, label %else_236

merge_235:                                        ; preds = %else_236, %then_234
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 0
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  %cmpeqtmp21 = icmp eq i64 %arr.get20, 974
  br i1 %cmpeqtmp21, label %then_237, label %else_239

merge_238:                                        ; preds = %else_239, %then_237
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 0
  store i64 0, ptr %arr.elem25, align 8
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load26" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load26")
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 5)
  ret void

then_225:                                         ; preds = %entry
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load", 1
  store i64 %addtmp, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_226

then_228:                                         ; preds = %merge_226
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load2" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp3 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load2", 1
  store i64 %addtmp3, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  %"%fd.load4" = load i64, ptr %"%fd", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%fd.load4"
  store i64 0, ptr %arr.elem, align 8
  br label %merge_229

then_231:                                         ; preds = %merge_229
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load5" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp6 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load5", 1
  store i64 %addtmp6, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_232

then_234:                                         ; preds = %merge_232
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load16" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp17 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load16", 1
  store i64 %addtmp17, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_235

then_237:                                         ; preds = %merge_235
  %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load22" = load i64, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  %addtmp23 = add i64 %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC.load22", 1
  store i64 %addtmp23, ptr %"%\D9\86\D8\AA\D8\A7\D8\A6\D8\AC", align 8
  call void @"fop_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_238
}

define void @__sad_main() {
entry:
  %"%\D9\85\D8\B3\D8\A7\D8\B1_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\85\D8\B3\D8\A7\D8\B1_\D8\B7\D9\88\D9\84" = alloca i64, align 8
  %"%\D9\85\D8\B3\D8\A7\D8\B1_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%fop_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%fop_\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B9\D9\85\D9\842" = alloca i64, align 8
  %"%fop_\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B9\D9\85\D9\84" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D9\88\D8\AD\D8\A9" = alloca i64, align 8
  %"%fd_\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D8\AD\D9\85\D9\84\D8\A9" = alloca i64, align 8
  %"%fd_\D9\85\D8\AE\D8\B2\D9\86" = alloca i64, align 8
  %"%fd_\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81" = alloca i64, align 8
  %"%fd_\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\84\D9\81" = alloca i64, align 8
  %"%fd_\D9\88\D8\B6\D8\B9" = alloca i64, align 8
  %"%fd_\D9\85\D9\88\D9\82\D8\B9" = alloca i64, align 8
  %"%fd_\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%fd_\D8\AA\D8\AD\D9\85\D9\8A\D9\84" = alloca i64, align 8
  %"%fd_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%ROOT_INODE" = alloca i64, align 8
  %"%TYPE_LINK" = alloca i64, align 8
  %"%TYPE_DIR" = alloca i64, align 8
  %"%TYPE_FILE" = alloca i64, align 8
  %"%\D9\85\D9\86_\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\85\D9\86_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%\D9\85\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\88\D8\B6\D8\B9_\D8\A5\D9\84\D8\AD\D8\A7\D9\82" = alloca i64, align 8
  %"%\D9\88\D8\B6\D8\B9_\D9\82\D8\B7\D8\B9" = alloca i64, align 8
  %"%\D9\88\D8\B6\D8\B9_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1" = alloca i64, align 8
  %"%\D9\88\D8\B6\D8\B9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D9\88\D8\B6\D8\B9_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D9\88\D8\B6\D8\B9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\88\D8\A7\D8\B5\D9\81\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_FOP" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9_FOP" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_FOP" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 4096, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_FOP", align 8
  store i64 8, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", align 8
  store i64 512, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9_FOP", align 8
  store i64 128, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_FOP", align 8
  store i64 32, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\88\D8\A7\D8\B5\D9\81\D8\A7\D8\AA", align 8
  store i64 1, ptr %"%\D9\88\D8\B6\D8\B9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  store i64 2, ptr %"%\D9\88\D8\B6\D8\B9_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  store i64 3, ptr %"%\D9\88\D8\B6\D8\B9_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  store i64 4, ptr %"%\D9\88\D8\B6\D8\B9_\D8\A5\D9\86\D8\B4\D8\A7\D8\A1", align 8
  store i64 8, ptr %"%\D9\88\D8\B6\D8\B9_\D9\82\D8\B7\D8\B9", align 8
  store i64 16, ptr %"%\D9\88\D8\B6\D8\B9_\D8\A5\D9\84\D8\AD\D8\A7\D9\82", align 8
  store i64 0, ptr %"%\D9\85\D9\86_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  store i64 1, ptr %"%\D9\85\D9\86_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 2, ptr %"%\D9\85\D9\86_\D8\A7\D9\84\D9\86\D9\87\D8\A7\D9\8A\D8\A9", align 8
  store i64 1, ptr %"%TYPE_FILE", align 8
  store i64 2, ptr %"%TYPE_DIR", align 8
  store i64 3, ptr %"%TYPE_LINK", align 8
  store i64 2, ptr %"%ROOT_INODE", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 32, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
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
  %arr.data.gep48 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data49 = load ptr, ptr %arr.data.gep48, align 8
  %arr.elem50 = getelementptr i64, ptr %arr.data49, i64 16
  store i64 0, ptr %arr.elem50, align 8
  %arr.data.gep51 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data52 = load ptr, ptr %arr.data.gep51, align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 17
  store i64 0, ptr %arr.elem53, align 8
  %arr.data.gep54 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data55 = load ptr, ptr %arr.data.gep54, align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 18
  store i64 0, ptr %arr.elem56, align 8
  %arr.data.gep57 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data58 = load ptr, ptr %arr.data.gep57, align 8
  %arr.elem59 = getelementptr i64, ptr %arr.data58, i64 19
  store i64 0, ptr %arr.elem59, align 8
  %arr.data.gep60 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data61 = load ptr, ptr %arr.data.gep60, align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 20
  store i64 0, ptr %arr.elem62, align 8
  %arr.data.gep63 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data64 = load ptr, ptr %arr.data.gep63, align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 21
  store i64 0, ptr %arr.elem65, align 8
  %arr.data.gep66 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data67 = load ptr, ptr %arr.data.gep66, align 8
  %arr.elem68 = getelementptr i64, ptr %arr.data67, i64 22
  store i64 0, ptr %arr.elem68, align 8
  %arr.data.gep69 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data70 = load ptr, ptr %arr.data.gep69, align 8
  %arr.elem71 = getelementptr i64, ptr %arr.data70, i64 23
  store i64 0, ptr %arr.elem71, align 8
  %arr.data.gep72 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data73 = load ptr, ptr %arr.data.gep72, align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 24
  store i64 0, ptr %arr.elem74, align 8
  %arr.data.gep75 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data76 = load ptr, ptr %arr.data.gep75, align 8
  %arr.elem77 = getelementptr i64, ptr %arr.data76, i64 25
  store i64 0, ptr %arr.elem77, align 8
  %arr.data.gep78 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data79 = load ptr, ptr %arr.data.gep78, align 8
  %arr.elem80 = getelementptr i64, ptr %arr.data79, i64 26
  store i64 0, ptr %arr.elem80, align 8
  %arr.data.gep81 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data82 = load ptr, ptr %arr.data.gep81, align 8
  %arr.elem83 = getelementptr i64, ptr %arr.data82, i64 27
  store i64 0, ptr %arr.elem83, align 8
  %arr.data.gep84 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data85 = load ptr, ptr %arr.data.gep84, align 8
  %arr.elem86 = getelementptr i64, ptr %arr.data85, i64 28
  store i64 0, ptr %arr.elem86, align 8
  %arr.data.gep87 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data88 = load ptr, ptr %arr.data.gep87, align 8
  %arr.elem89 = getelementptr i64, ptr %arr.data88, i64 29
  store i64 0, ptr %arr.elem89, align 8
  %arr.data.gep90 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data91 = load ptr, ptr %arr.data.gep90, align 8
  %arr.elem92 = getelementptr i64, ptr %arr.data91, i64 30
  store i64 0, ptr %arr.elem92, align 8
  %arr.data.gep93 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data94 = load ptr, ptr %arr.data.gep93, align 8
  %arr.elem95 = getelementptr i64, ptr %arr.data94, i64 31
  store i64 0, ptr %arr.elem95, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%fd_\D9\86\D8\B4\D8\B7", align 8
  %arr_new96 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep97 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 0
  store i64 32, ptr %arr.len.gep97, align 8
  %arr.cap.gep98 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep98, align 8
  %arr.data99 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep100 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  store ptr %arr.data99, ptr %arr.data.gep100, align 8
  %arr.data.gep101 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data102 = load ptr, ptr %arr.data.gep101, align 8
  %arr.elem103 = getelementptr i64, ptr %arr.data102, i64 0
  store i64 0, ptr %arr.elem103, align 8
  %arr.data.gep104 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data105 = load ptr, ptr %arr.data.gep104, align 8
  %arr.elem106 = getelementptr i64, ptr %arr.data105, i64 1
  store i64 0, ptr %arr.elem106, align 8
  %arr.data.gep107 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data108 = load ptr, ptr %arr.data.gep107, align 8
  %arr.elem109 = getelementptr i64, ptr %arr.data108, i64 2
  store i64 0, ptr %arr.elem109, align 8
  %arr.data.gep110 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data111 = load ptr, ptr %arr.data.gep110, align 8
  %arr.elem112 = getelementptr i64, ptr %arr.data111, i64 3
  store i64 0, ptr %arr.elem112, align 8
  %arr.data.gep113 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data114 = load ptr, ptr %arr.data.gep113, align 8
  %arr.elem115 = getelementptr i64, ptr %arr.data114, i64 4
  store i64 0, ptr %arr.elem115, align 8
  %arr.data.gep116 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data117 = load ptr, ptr %arr.data.gep116, align 8
  %arr.elem118 = getelementptr i64, ptr %arr.data117, i64 5
  store i64 0, ptr %arr.elem118, align 8
  %arr.data.gep119 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data120 = load ptr, ptr %arr.data.gep119, align 8
  %arr.elem121 = getelementptr i64, ptr %arr.data120, i64 6
  store i64 0, ptr %arr.elem121, align 8
  %arr.data.gep122 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data123 = load ptr, ptr %arr.data.gep122, align 8
  %arr.elem124 = getelementptr i64, ptr %arr.data123, i64 7
  store i64 0, ptr %arr.elem124, align 8
  %arr.data.gep125 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data126 = load ptr, ptr %arr.data.gep125, align 8
  %arr.elem127 = getelementptr i64, ptr %arr.data126, i64 8
  store i64 0, ptr %arr.elem127, align 8
  %arr.data.gep128 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data129 = load ptr, ptr %arr.data.gep128, align 8
  %arr.elem130 = getelementptr i64, ptr %arr.data129, i64 9
  store i64 0, ptr %arr.elem130, align 8
  %arr.data.gep131 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data132 = load ptr, ptr %arr.data.gep131, align 8
  %arr.elem133 = getelementptr i64, ptr %arr.data132, i64 10
  store i64 0, ptr %arr.elem133, align 8
  %arr.data.gep134 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data135 = load ptr, ptr %arr.data.gep134, align 8
  %arr.elem136 = getelementptr i64, ptr %arr.data135, i64 11
  store i64 0, ptr %arr.elem136, align 8
  %arr.data.gep137 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data138 = load ptr, ptr %arr.data.gep137, align 8
  %arr.elem139 = getelementptr i64, ptr %arr.data138, i64 12
  store i64 0, ptr %arr.elem139, align 8
  %arr.data.gep140 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data141 = load ptr, ptr %arr.data.gep140, align 8
  %arr.elem142 = getelementptr i64, ptr %arr.data141, i64 13
  store i64 0, ptr %arr.elem142, align 8
  %arr.data.gep143 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data144 = load ptr, ptr %arr.data.gep143, align 8
  %arr.elem145 = getelementptr i64, ptr %arr.data144, i64 14
  store i64 0, ptr %arr.elem145, align 8
  %arr.data.gep146 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data147 = load ptr, ptr %arr.data.gep146, align 8
  %arr.elem148 = getelementptr i64, ptr %arr.data147, i64 15
  store i64 0, ptr %arr.elem148, align 8
  %arr.data.gep149 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data150 = load ptr, ptr %arr.data.gep149, align 8
  %arr.elem151 = getelementptr i64, ptr %arr.data150, i64 16
  store i64 0, ptr %arr.elem151, align 8
  %arr.data.gep152 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data153 = load ptr, ptr %arr.data.gep152, align 8
  %arr.elem154 = getelementptr i64, ptr %arr.data153, i64 17
  store i64 0, ptr %arr.elem154, align 8
  %arr.data.gep155 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data156 = load ptr, ptr %arr.data.gep155, align 8
  %arr.elem157 = getelementptr i64, ptr %arr.data156, i64 18
  store i64 0, ptr %arr.elem157, align 8
  %arr.data.gep158 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data159 = load ptr, ptr %arr.data.gep158, align 8
  %arr.elem160 = getelementptr i64, ptr %arr.data159, i64 19
  store i64 0, ptr %arr.elem160, align 8
  %arr.data.gep161 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data162 = load ptr, ptr %arr.data.gep161, align 8
  %arr.elem163 = getelementptr i64, ptr %arr.data162, i64 20
  store i64 0, ptr %arr.elem163, align 8
  %arr.data.gep164 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data165 = load ptr, ptr %arr.data.gep164, align 8
  %arr.elem166 = getelementptr i64, ptr %arr.data165, i64 21
  store i64 0, ptr %arr.elem166, align 8
  %arr.data.gep167 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data168 = load ptr, ptr %arr.data.gep167, align 8
  %arr.elem169 = getelementptr i64, ptr %arr.data168, i64 22
  store i64 0, ptr %arr.elem169, align 8
  %arr.data.gep170 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data171 = load ptr, ptr %arr.data.gep170, align 8
  %arr.elem172 = getelementptr i64, ptr %arr.data171, i64 23
  store i64 0, ptr %arr.elem172, align 8
  %arr.data.gep173 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data174 = load ptr, ptr %arr.data.gep173, align 8
  %arr.elem175 = getelementptr i64, ptr %arr.data174, i64 24
  store i64 0, ptr %arr.elem175, align 8
  %arr.data.gep176 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data177 = load ptr, ptr %arr.data.gep176, align 8
  %arr.elem178 = getelementptr i64, ptr %arr.data177, i64 25
  store i64 0, ptr %arr.elem178, align 8
  %arr.data.gep179 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data180 = load ptr, ptr %arr.data.gep179, align 8
  %arr.elem181 = getelementptr i64, ptr %arr.data180, i64 26
  store i64 0, ptr %arr.elem181, align 8
  %arr.data.gep182 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data183 = load ptr, ptr %arr.data.gep182, align 8
  %arr.elem184 = getelementptr i64, ptr %arr.data183, i64 27
  store i64 0, ptr %arr.elem184, align 8
  %arr.data.gep185 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data186 = load ptr, ptr %arr.data.gep185, align 8
  %arr.elem187 = getelementptr i64, ptr %arr.data186, i64 28
  store i64 0, ptr %arr.elem187, align 8
  %arr.data.gep188 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data189 = load ptr, ptr %arr.data.gep188, align 8
  %arr.elem190 = getelementptr i64, ptr %arr.data189, i64 29
  store i64 0, ptr %arr.elem190, align 8
  %arr.data.gep191 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data192 = load ptr, ptr %arr.data.gep191, align 8
  %arr.elem193 = getelementptr i64, ptr %arr.data192, i64 30
  store i64 0, ptr %arr.elem193, align 8
  %arr.data.gep194 = getelementptr inbounds %SadArray, ptr %arr_new96, i32 0, i32 2
  %arr.data195 = load ptr, ptr %arr.data.gep194, align 8
  %arr.elem196 = getelementptr i64, ptr %arr.data195, i64 31
  store i64 0, ptr %arr.elem196, align 8
  %obj.ptrtoint197 = ptrtoint ptr %arr_new96 to i64
  store i64 %obj.ptrtoint197, ptr %"%fd_\D8\AA\D8\AD\D9\85\D9\8A\D9\84", align 8
  %arr_new198 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep199 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 0
  store i64 32, ptr %arr.len.gep199, align 8
  %arr.cap.gep200 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep200, align 8
  %arr.data201 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep202 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  store ptr %arr.data201, ptr %arr.data.gep202, align 8
  %arr.data.gep203 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data204 = load ptr, ptr %arr.data.gep203, align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 0
  store i64 0, ptr %arr.elem205, align 8
  %arr.data.gep206 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data207 = load ptr, ptr %arr.data.gep206, align 8
  %arr.elem208 = getelementptr i64, ptr %arr.data207, i64 1
  store i64 0, ptr %arr.elem208, align 8
  %arr.data.gep209 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data210 = load ptr, ptr %arr.data.gep209, align 8
  %arr.elem211 = getelementptr i64, ptr %arr.data210, i64 2
  store i64 0, ptr %arr.elem211, align 8
  %arr.data.gep212 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data213 = load ptr, ptr %arr.data.gep212, align 8
  %arr.elem214 = getelementptr i64, ptr %arr.data213, i64 3
  store i64 0, ptr %arr.elem214, align 8
  %arr.data.gep215 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data216 = load ptr, ptr %arr.data.gep215, align 8
  %arr.elem217 = getelementptr i64, ptr %arr.data216, i64 4
  store i64 0, ptr %arr.elem217, align 8
  %arr.data.gep218 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data219 = load ptr, ptr %arr.data.gep218, align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 5
  store i64 0, ptr %arr.elem220, align 8
  %arr.data.gep221 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data222 = load ptr, ptr %arr.data.gep221, align 8
  %arr.elem223 = getelementptr i64, ptr %arr.data222, i64 6
  store i64 0, ptr %arr.elem223, align 8
  %arr.data.gep224 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data225 = load ptr, ptr %arr.data.gep224, align 8
  %arr.elem226 = getelementptr i64, ptr %arr.data225, i64 7
  store i64 0, ptr %arr.elem226, align 8
  %arr.data.gep227 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data228 = load ptr, ptr %arr.data.gep227, align 8
  %arr.elem229 = getelementptr i64, ptr %arr.data228, i64 8
  store i64 0, ptr %arr.elem229, align 8
  %arr.data.gep230 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data231 = load ptr, ptr %arr.data.gep230, align 8
  %arr.elem232 = getelementptr i64, ptr %arr.data231, i64 9
  store i64 0, ptr %arr.elem232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 10
  store i64 0, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 11
  store i64 0, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 12
  store i64 0, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 13
  store i64 0, ptr %arr.elem244, align 8
  %arr.data.gep245 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data246 = load ptr, ptr %arr.data.gep245, align 8
  %arr.elem247 = getelementptr i64, ptr %arr.data246, i64 14
  store i64 0, ptr %arr.elem247, align 8
  %arr.data.gep248 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data249 = load ptr, ptr %arr.data.gep248, align 8
  %arr.elem250 = getelementptr i64, ptr %arr.data249, i64 15
  store i64 0, ptr %arr.elem250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 16
  store i64 0, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 17
  store i64 0, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 18
  store i64 0, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 19
  store i64 0, ptr %arr.elem262, align 8
  %arr.data.gep263 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data264 = load ptr, ptr %arr.data.gep263, align 8
  %arr.elem265 = getelementptr i64, ptr %arr.data264, i64 20
  store i64 0, ptr %arr.elem265, align 8
  %arr.data.gep266 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data267 = load ptr, ptr %arr.data.gep266, align 8
  %arr.elem268 = getelementptr i64, ptr %arr.data267, i64 21
  store i64 0, ptr %arr.elem268, align 8
  %arr.data.gep269 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data270 = load ptr, ptr %arr.data.gep269, align 8
  %arr.elem271 = getelementptr i64, ptr %arr.data270, i64 22
  store i64 0, ptr %arr.elem271, align 8
  %arr.data.gep272 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data273 = load ptr, ptr %arr.data.gep272, align 8
  %arr.elem274 = getelementptr i64, ptr %arr.data273, i64 23
  store i64 0, ptr %arr.elem274, align 8
  %arr.data.gep275 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data276 = load ptr, ptr %arr.data.gep275, align 8
  %arr.elem277 = getelementptr i64, ptr %arr.data276, i64 24
  store i64 0, ptr %arr.elem277, align 8
  %arr.data.gep278 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data279 = load ptr, ptr %arr.data.gep278, align 8
  %arr.elem280 = getelementptr i64, ptr %arr.data279, i64 25
  store i64 0, ptr %arr.elem280, align 8
  %arr.data.gep281 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data282 = load ptr, ptr %arr.data.gep281, align 8
  %arr.elem283 = getelementptr i64, ptr %arr.data282, i64 26
  store i64 0, ptr %arr.elem283, align 8
  %arr.data.gep284 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data285 = load ptr, ptr %arr.data.gep284, align 8
  %arr.elem286 = getelementptr i64, ptr %arr.data285, i64 27
  store i64 0, ptr %arr.elem286, align 8
  %arr.data.gep287 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data288 = load ptr, ptr %arr.data.gep287, align 8
  %arr.elem289 = getelementptr i64, ptr %arr.data288, i64 28
  store i64 0, ptr %arr.elem289, align 8
  %arr.data.gep290 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data291 = load ptr, ptr %arr.data.gep290, align 8
  %arr.elem292 = getelementptr i64, ptr %arr.data291, i64 29
  store i64 0, ptr %arr.elem292, align 8
  %arr.data.gep293 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data294 = load ptr, ptr %arr.data.gep293, align 8
  %arr.elem295 = getelementptr i64, ptr %arr.data294, i64 30
  store i64 0, ptr %arr.elem295, align 8
  %arr.data.gep296 = getelementptr inbounds %SadArray, ptr %arr_new198, i32 0, i32 2
  %arr.data297 = load ptr, ptr %arr.data.gep296, align 8
  %arr.elem298 = getelementptr i64, ptr %arr.data297, i64 31
  store i64 0, ptr %arr.elem298, align 8
  %obj.ptrtoint299 = ptrtoint ptr %arr_new198 to i64
  store i64 %obj.ptrtoint299, ptr %"%fd_\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %arr_new300 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep301 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 0
  store i64 32, ptr %arr.len.gep301, align 8
  %arr.cap.gep302 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep302, align 8
  %arr.data303 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep304 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  store ptr %arr.data303, ptr %arr.data.gep304, align 8
  %arr.data.gep305 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data306 = load ptr, ptr %arr.data.gep305, align 8
  %arr.elem307 = getelementptr i64, ptr %arr.data306, i64 0
  store i64 0, ptr %arr.elem307, align 8
  %arr.data.gep308 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data309 = load ptr, ptr %arr.data.gep308, align 8
  %arr.elem310 = getelementptr i64, ptr %arr.data309, i64 1
  store i64 0, ptr %arr.elem310, align 8
  %arr.data.gep311 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data312 = load ptr, ptr %arr.data.gep311, align 8
  %arr.elem313 = getelementptr i64, ptr %arr.data312, i64 2
  store i64 0, ptr %arr.elem313, align 8
  %arr.data.gep314 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data315 = load ptr, ptr %arr.data.gep314, align 8
  %arr.elem316 = getelementptr i64, ptr %arr.data315, i64 3
  store i64 0, ptr %arr.elem316, align 8
  %arr.data.gep317 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data318 = load ptr, ptr %arr.data.gep317, align 8
  %arr.elem319 = getelementptr i64, ptr %arr.data318, i64 4
  store i64 0, ptr %arr.elem319, align 8
  %arr.data.gep320 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data321 = load ptr, ptr %arr.data.gep320, align 8
  %arr.elem322 = getelementptr i64, ptr %arr.data321, i64 5
  store i64 0, ptr %arr.elem322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 6
  store i64 0, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 7
  store i64 0, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 8
  store i64 0, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 9
  store i64 0, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 10
  store i64 0, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 11
  store i64 0, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 12
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 13
  store i64 0, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 14
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 15
  store i64 0, ptr %arr.elem352, align 8
  %arr.data.gep353 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data354 = load ptr, ptr %arr.data.gep353, align 8
  %arr.elem355 = getelementptr i64, ptr %arr.data354, i64 16
  store i64 0, ptr %arr.elem355, align 8
  %arr.data.gep356 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data357 = load ptr, ptr %arr.data.gep356, align 8
  %arr.elem358 = getelementptr i64, ptr %arr.data357, i64 17
  store i64 0, ptr %arr.elem358, align 8
  %arr.data.gep359 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data360 = load ptr, ptr %arr.data.gep359, align 8
  %arr.elem361 = getelementptr i64, ptr %arr.data360, i64 18
  store i64 0, ptr %arr.elem361, align 8
  %arr.data.gep362 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data363 = load ptr, ptr %arr.data.gep362, align 8
  %arr.elem364 = getelementptr i64, ptr %arr.data363, i64 19
  store i64 0, ptr %arr.elem364, align 8
  %arr.data.gep365 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data366 = load ptr, ptr %arr.data.gep365, align 8
  %arr.elem367 = getelementptr i64, ptr %arr.data366, i64 20
  store i64 0, ptr %arr.elem367, align 8
  %arr.data.gep368 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data369 = load ptr, ptr %arr.data.gep368, align 8
  %arr.elem370 = getelementptr i64, ptr %arr.data369, i64 21
  store i64 0, ptr %arr.elem370, align 8
  %arr.data.gep371 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data372 = load ptr, ptr %arr.data.gep371, align 8
  %arr.elem373 = getelementptr i64, ptr %arr.data372, i64 22
  store i64 0, ptr %arr.elem373, align 8
  %arr.data.gep374 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data375 = load ptr, ptr %arr.data.gep374, align 8
  %arr.elem376 = getelementptr i64, ptr %arr.data375, i64 23
  store i64 0, ptr %arr.elem376, align 8
  %arr.data.gep377 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data378 = load ptr, ptr %arr.data.gep377, align 8
  %arr.elem379 = getelementptr i64, ptr %arr.data378, i64 24
  store i64 0, ptr %arr.elem379, align 8
  %arr.data.gep380 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data381 = load ptr, ptr %arr.data.gep380, align 8
  %arr.elem382 = getelementptr i64, ptr %arr.data381, i64 25
  store i64 0, ptr %arr.elem382, align 8
  %arr.data.gep383 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data384 = load ptr, ptr %arr.data.gep383, align 8
  %arr.elem385 = getelementptr i64, ptr %arr.data384, i64 26
  store i64 0, ptr %arr.elem385, align 8
  %arr.data.gep386 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data387 = load ptr, ptr %arr.data.gep386, align 8
  %arr.elem388 = getelementptr i64, ptr %arr.data387, i64 27
  store i64 0, ptr %arr.elem388, align 8
  %arr.data.gep389 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data390 = load ptr, ptr %arr.data.gep389, align 8
  %arr.elem391 = getelementptr i64, ptr %arr.data390, i64 28
  store i64 0, ptr %arr.elem391, align 8
  %arr.data.gep392 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data393 = load ptr, ptr %arr.data.gep392, align 8
  %arr.elem394 = getelementptr i64, ptr %arr.data393, i64 29
  store i64 0, ptr %arr.elem394, align 8
  %arr.data.gep395 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data396 = load ptr, ptr %arr.data.gep395, align 8
  %arr.elem397 = getelementptr i64, ptr %arr.data396, i64 30
  store i64 0, ptr %arr.elem397, align 8
  %arr.data.gep398 = getelementptr inbounds %SadArray, ptr %arr_new300, i32 0, i32 2
  %arr.data399 = load ptr, ptr %arr.data.gep398, align 8
  %arr.elem400 = getelementptr i64, ptr %arr.data399, i64 31
  store i64 0, ptr %arr.elem400, align 8
  %obj.ptrtoint401 = ptrtoint ptr %arr_new300 to i64
  store i64 %obj.ptrtoint401, ptr %"%fd_\D9\85\D9\88\D9\82\D8\B9", align 8
  %arr_new402 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep403 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 0
  store i64 32, ptr %arr.len.gep403, align 8
  %arr.cap.gep404 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep404, align 8
  %arr.data405 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep406 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  store ptr %arr.data405, ptr %arr.data.gep406, align 8
  %arr.data.gep407 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data408 = load ptr, ptr %arr.data.gep407, align 8
  %arr.elem409 = getelementptr i64, ptr %arr.data408, i64 0
  store i64 0, ptr %arr.elem409, align 8
  %arr.data.gep410 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data411 = load ptr, ptr %arr.data.gep410, align 8
  %arr.elem412 = getelementptr i64, ptr %arr.data411, i64 1
  store i64 0, ptr %arr.elem412, align 8
  %arr.data.gep413 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data414 = load ptr, ptr %arr.data.gep413, align 8
  %arr.elem415 = getelementptr i64, ptr %arr.data414, i64 2
  store i64 0, ptr %arr.elem415, align 8
  %arr.data.gep416 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data417 = load ptr, ptr %arr.data.gep416, align 8
  %arr.elem418 = getelementptr i64, ptr %arr.data417, i64 3
  store i64 0, ptr %arr.elem418, align 8
  %arr.data.gep419 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data420 = load ptr, ptr %arr.data.gep419, align 8
  %arr.elem421 = getelementptr i64, ptr %arr.data420, i64 4
  store i64 0, ptr %arr.elem421, align 8
  %arr.data.gep422 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data423 = load ptr, ptr %arr.data.gep422, align 8
  %arr.elem424 = getelementptr i64, ptr %arr.data423, i64 5
  store i64 0, ptr %arr.elem424, align 8
  %arr.data.gep425 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data426 = load ptr, ptr %arr.data.gep425, align 8
  %arr.elem427 = getelementptr i64, ptr %arr.data426, i64 6
  store i64 0, ptr %arr.elem427, align 8
  %arr.data.gep428 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data429 = load ptr, ptr %arr.data.gep428, align 8
  %arr.elem430 = getelementptr i64, ptr %arr.data429, i64 7
  store i64 0, ptr %arr.elem430, align 8
  %arr.data.gep431 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data432 = load ptr, ptr %arr.data.gep431, align 8
  %arr.elem433 = getelementptr i64, ptr %arr.data432, i64 8
  store i64 0, ptr %arr.elem433, align 8
  %arr.data.gep434 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data435 = load ptr, ptr %arr.data.gep434, align 8
  %arr.elem436 = getelementptr i64, ptr %arr.data435, i64 9
  store i64 0, ptr %arr.elem436, align 8
  %arr.data.gep437 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data438 = load ptr, ptr %arr.data.gep437, align 8
  %arr.elem439 = getelementptr i64, ptr %arr.data438, i64 10
  store i64 0, ptr %arr.elem439, align 8
  %arr.data.gep440 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data441 = load ptr, ptr %arr.data.gep440, align 8
  %arr.elem442 = getelementptr i64, ptr %arr.data441, i64 11
  store i64 0, ptr %arr.elem442, align 8
  %arr.data.gep443 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data444 = load ptr, ptr %arr.data.gep443, align 8
  %arr.elem445 = getelementptr i64, ptr %arr.data444, i64 12
  store i64 0, ptr %arr.elem445, align 8
  %arr.data.gep446 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data447 = load ptr, ptr %arr.data.gep446, align 8
  %arr.elem448 = getelementptr i64, ptr %arr.data447, i64 13
  store i64 0, ptr %arr.elem448, align 8
  %arr.data.gep449 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data450 = load ptr, ptr %arr.data.gep449, align 8
  %arr.elem451 = getelementptr i64, ptr %arr.data450, i64 14
  store i64 0, ptr %arr.elem451, align 8
  %arr.data.gep452 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data453 = load ptr, ptr %arr.data.gep452, align 8
  %arr.elem454 = getelementptr i64, ptr %arr.data453, i64 15
  store i64 0, ptr %arr.elem454, align 8
  %arr.data.gep455 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data456 = load ptr, ptr %arr.data.gep455, align 8
  %arr.elem457 = getelementptr i64, ptr %arr.data456, i64 16
  store i64 0, ptr %arr.elem457, align 8
  %arr.data.gep458 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data459 = load ptr, ptr %arr.data.gep458, align 8
  %arr.elem460 = getelementptr i64, ptr %arr.data459, i64 17
  store i64 0, ptr %arr.elem460, align 8
  %arr.data.gep461 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data462 = load ptr, ptr %arr.data.gep461, align 8
  %arr.elem463 = getelementptr i64, ptr %arr.data462, i64 18
  store i64 0, ptr %arr.elem463, align 8
  %arr.data.gep464 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data465 = load ptr, ptr %arr.data.gep464, align 8
  %arr.elem466 = getelementptr i64, ptr %arr.data465, i64 19
  store i64 0, ptr %arr.elem466, align 8
  %arr.data.gep467 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data468 = load ptr, ptr %arr.data.gep467, align 8
  %arr.elem469 = getelementptr i64, ptr %arr.data468, i64 20
  store i64 0, ptr %arr.elem469, align 8
  %arr.data.gep470 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data471 = load ptr, ptr %arr.data.gep470, align 8
  %arr.elem472 = getelementptr i64, ptr %arr.data471, i64 21
  store i64 0, ptr %arr.elem472, align 8
  %arr.data.gep473 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data474 = load ptr, ptr %arr.data.gep473, align 8
  %arr.elem475 = getelementptr i64, ptr %arr.data474, i64 22
  store i64 0, ptr %arr.elem475, align 8
  %arr.data.gep476 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data477 = load ptr, ptr %arr.data.gep476, align 8
  %arr.elem478 = getelementptr i64, ptr %arr.data477, i64 23
  store i64 0, ptr %arr.elem478, align 8
  %arr.data.gep479 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data480 = load ptr, ptr %arr.data.gep479, align 8
  %arr.elem481 = getelementptr i64, ptr %arr.data480, i64 24
  store i64 0, ptr %arr.elem481, align 8
  %arr.data.gep482 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data483 = load ptr, ptr %arr.data.gep482, align 8
  %arr.elem484 = getelementptr i64, ptr %arr.data483, i64 25
  store i64 0, ptr %arr.elem484, align 8
  %arr.data.gep485 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data486 = load ptr, ptr %arr.data.gep485, align 8
  %arr.elem487 = getelementptr i64, ptr %arr.data486, i64 26
  store i64 0, ptr %arr.elem487, align 8
  %arr.data.gep488 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data489 = load ptr, ptr %arr.data.gep488, align 8
  %arr.elem490 = getelementptr i64, ptr %arr.data489, i64 27
  store i64 0, ptr %arr.elem490, align 8
  %arr.data.gep491 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data492 = load ptr, ptr %arr.data.gep491, align 8
  %arr.elem493 = getelementptr i64, ptr %arr.data492, i64 28
  store i64 0, ptr %arr.elem493, align 8
  %arr.data.gep494 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data495 = load ptr, ptr %arr.data.gep494, align 8
  %arr.elem496 = getelementptr i64, ptr %arr.data495, i64 29
  store i64 0, ptr %arr.elem496, align 8
  %arr.data.gep497 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data498 = load ptr, ptr %arr.data.gep497, align 8
  %arr.elem499 = getelementptr i64, ptr %arr.data498, i64 30
  store i64 0, ptr %arr.elem499, align 8
  %arr.data.gep500 = getelementptr inbounds %SadArray, ptr %arr_new402, i32 0, i32 2
  %arr.data501 = load ptr, ptr %arr.data.gep500, align 8
  %arr.elem502 = getelementptr i64, ptr %arr.data501, i64 31
  store i64 0, ptr %arr.elem502, align 8
  %obj.ptrtoint503 = ptrtoint ptr %arr_new402 to i64
  store i64 %obj.ptrtoint503, ptr %"%fd_\D9\88\D8\B6\D8\B9", align 8
  %arr_new504 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep505 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 0
  store i64 32, ptr %arr.len.gep505, align 8
  %arr.cap.gep506 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep506, align 8
  %arr.data507 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep508 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  store ptr %arr.data507, ptr %arr.data.gep508, align 8
  %arr.data.gep509 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data510 = load ptr, ptr %arr.data.gep509, align 8
  %arr.elem511 = getelementptr i64, ptr %arr.data510, i64 0
  store i64 0, ptr %arr.elem511, align 8
  %arr.data.gep512 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data513 = load ptr, ptr %arr.data.gep512, align 8
  %arr.elem514 = getelementptr i64, ptr %arr.data513, i64 1
  store i64 0, ptr %arr.elem514, align 8
  %arr.data.gep515 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data516 = load ptr, ptr %arr.data.gep515, align 8
  %arr.elem517 = getelementptr i64, ptr %arr.data516, i64 2
  store i64 0, ptr %arr.elem517, align 8
  %arr.data.gep518 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data519 = load ptr, ptr %arr.data.gep518, align 8
  %arr.elem520 = getelementptr i64, ptr %arr.data519, i64 3
  store i64 0, ptr %arr.elem520, align 8
  %arr.data.gep521 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data522 = load ptr, ptr %arr.data.gep521, align 8
  %arr.elem523 = getelementptr i64, ptr %arr.data522, i64 4
  store i64 0, ptr %arr.elem523, align 8
  %arr.data.gep524 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data525 = load ptr, ptr %arr.data.gep524, align 8
  %arr.elem526 = getelementptr i64, ptr %arr.data525, i64 5
  store i64 0, ptr %arr.elem526, align 8
  %arr.data.gep527 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data528 = load ptr, ptr %arr.data.gep527, align 8
  %arr.elem529 = getelementptr i64, ptr %arr.data528, i64 6
  store i64 0, ptr %arr.elem529, align 8
  %arr.data.gep530 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data531 = load ptr, ptr %arr.data.gep530, align 8
  %arr.elem532 = getelementptr i64, ptr %arr.data531, i64 7
  store i64 0, ptr %arr.elem532, align 8
  %arr.data.gep533 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data534 = load ptr, ptr %arr.data.gep533, align 8
  %arr.elem535 = getelementptr i64, ptr %arr.data534, i64 8
  store i64 0, ptr %arr.elem535, align 8
  %arr.data.gep536 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data537 = load ptr, ptr %arr.data.gep536, align 8
  %arr.elem538 = getelementptr i64, ptr %arr.data537, i64 9
  store i64 0, ptr %arr.elem538, align 8
  %arr.data.gep539 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data540 = load ptr, ptr %arr.data.gep539, align 8
  %arr.elem541 = getelementptr i64, ptr %arr.data540, i64 10
  store i64 0, ptr %arr.elem541, align 8
  %arr.data.gep542 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data543 = load ptr, ptr %arr.data.gep542, align 8
  %arr.elem544 = getelementptr i64, ptr %arr.data543, i64 11
  store i64 0, ptr %arr.elem544, align 8
  %arr.data.gep545 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data546 = load ptr, ptr %arr.data.gep545, align 8
  %arr.elem547 = getelementptr i64, ptr %arr.data546, i64 12
  store i64 0, ptr %arr.elem547, align 8
  %arr.data.gep548 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data549 = load ptr, ptr %arr.data.gep548, align 8
  %arr.elem550 = getelementptr i64, ptr %arr.data549, i64 13
  store i64 0, ptr %arr.elem550, align 8
  %arr.data.gep551 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data552 = load ptr, ptr %arr.data.gep551, align 8
  %arr.elem553 = getelementptr i64, ptr %arr.data552, i64 14
  store i64 0, ptr %arr.elem553, align 8
  %arr.data.gep554 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data555 = load ptr, ptr %arr.data.gep554, align 8
  %arr.elem556 = getelementptr i64, ptr %arr.data555, i64 15
  store i64 0, ptr %arr.elem556, align 8
  %arr.data.gep557 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data558 = load ptr, ptr %arr.data.gep557, align 8
  %arr.elem559 = getelementptr i64, ptr %arr.data558, i64 16
  store i64 0, ptr %arr.elem559, align 8
  %arr.data.gep560 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data561 = load ptr, ptr %arr.data.gep560, align 8
  %arr.elem562 = getelementptr i64, ptr %arr.data561, i64 17
  store i64 0, ptr %arr.elem562, align 8
  %arr.data.gep563 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data564 = load ptr, ptr %arr.data.gep563, align 8
  %arr.elem565 = getelementptr i64, ptr %arr.data564, i64 18
  store i64 0, ptr %arr.elem565, align 8
  %arr.data.gep566 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data567 = load ptr, ptr %arr.data.gep566, align 8
  %arr.elem568 = getelementptr i64, ptr %arr.data567, i64 19
  store i64 0, ptr %arr.elem568, align 8
  %arr.data.gep569 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data570 = load ptr, ptr %arr.data.gep569, align 8
  %arr.elem571 = getelementptr i64, ptr %arr.data570, i64 20
  store i64 0, ptr %arr.elem571, align 8
  %arr.data.gep572 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data573 = load ptr, ptr %arr.data.gep572, align 8
  %arr.elem574 = getelementptr i64, ptr %arr.data573, i64 21
  store i64 0, ptr %arr.elem574, align 8
  %arr.data.gep575 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data576 = load ptr, ptr %arr.data.gep575, align 8
  %arr.elem577 = getelementptr i64, ptr %arr.data576, i64 22
  store i64 0, ptr %arr.elem577, align 8
  %arr.data.gep578 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data579 = load ptr, ptr %arr.data.gep578, align 8
  %arr.elem580 = getelementptr i64, ptr %arr.data579, i64 23
  store i64 0, ptr %arr.elem580, align 8
  %arr.data.gep581 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data582 = load ptr, ptr %arr.data.gep581, align 8
  %arr.elem583 = getelementptr i64, ptr %arr.data582, i64 24
  store i64 0, ptr %arr.elem583, align 8
  %arr.data.gep584 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data585 = load ptr, ptr %arr.data.gep584, align 8
  %arr.elem586 = getelementptr i64, ptr %arr.data585, i64 25
  store i64 0, ptr %arr.elem586, align 8
  %arr.data.gep587 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data588 = load ptr, ptr %arr.data.gep587, align 8
  %arr.elem589 = getelementptr i64, ptr %arr.data588, i64 26
  store i64 0, ptr %arr.elem589, align 8
  %arr.data.gep590 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data591 = load ptr, ptr %arr.data.gep590, align 8
  %arr.elem592 = getelementptr i64, ptr %arr.data591, i64 27
  store i64 0, ptr %arr.elem592, align 8
  %arr.data.gep593 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data594 = load ptr, ptr %arr.data.gep593, align 8
  %arr.elem595 = getelementptr i64, ptr %arr.data594, i64 28
  store i64 0, ptr %arr.elem595, align 8
  %arr.data.gep596 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data597 = load ptr, ptr %arr.data.gep596, align 8
  %arr.elem598 = getelementptr i64, ptr %arr.data597, i64 29
  store i64 0, ptr %arr.elem598, align 8
  %arr.data.gep599 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data600 = load ptr, ptr %arr.data.gep599, align 8
  %arr.elem601 = getelementptr i64, ptr %arr.data600, i64 30
  store i64 0, ptr %arr.elem601, align 8
  %arr.data.gep602 = getelementptr inbounds %SadArray, ptr %arr_new504, i32 0, i32 2
  %arr.data603 = load ptr, ptr %arr.data.gep602, align 8
  %arr.elem604 = getelementptr i64, ptr %arr.data603, i64 31
  store i64 0, ptr %arr.elem604, align 8
  %obj.ptrtoint605 = ptrtoint ptr %arr_new504 to i64
  store i64 %obj.ptrtoint605, ptr %"%fd_\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\85\D9\84\D9\81", align 8
  %arr_new606 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep607 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 0
  store i64 32, ptr %arr.len.gep607, align 8
  %arr.cap.gep608 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep608, align 8
  %arr.data609 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep610 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  store ptr %arr.data609, ptr %arr.data.gep610, align 8
  %arr.data.gep611 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data612 = load ptr, ptr %arr.data.gep611, align 8
  %arr.elem613 = getelementptr i64, ptr %arr.data612, i64 0
  store i64 0, ptr %arr.elem613, align 8
  %arr.data.gep614 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data615 = load ptr, ptr %arr.data.gep614, align 8
  %arr.elem616 = getelementptr i64, ptr %arr.data615, i64 1
  store i64 0, ptr %arr.elem616, align 8
  %arr.data.gep617 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data618 = load ptr, ptr %arr.data.gep617, align 8
  %arr.elem619 = getelementptr i64, ptr %arr.data618, i64 2
  store i64 0, ptr %arr.elem619, align 8
  %arr.data.gep620 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data621 = load ptr, ptr %arr.data.gep620, align 8
  %arr.elem622 = getelementptr i64, ptr %arr.data621, i64 3
  store i64 0, ptr %arr.elem622, align 8
  %arr.data.gep623 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data624 = load ptr, ptr %arr.data.gep623, align 8
  %arr.elem625 = getelementptr i64, ptr %arr.data624, i64 4
  store i64 0, ptr %arr.elem625, align 8
  %arr.data.gep626 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data627 = load ptr, ptr %arr.data.gep626, align 8
  %arr.elem628 = getelementptr i64, ptr %arr.data627, i64 5
  store i64 0, ptr %arr.elem628, align 8
  %arr.data.gep629 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data630 = load ptr, ptr %arr.data.gep629, align 8
  %arr.elem631 = getelementptr i64, ptr %arr.data630, i64 6
  store i64 0, ptr %arr.elem631, align 8
  %arr.data.gep632 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data633 = load ptr, ptr %arr.data.gep632, align 8
  %arr.elem634 = getelementptr i64, ptr %arr.data633, i64 7
  store i64 0, ptr %arr.elem634, align 8
  %arr.data.gep635 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data636 = load ptr, ptr %arr.data.gep635, align 8
  %arr.elem637 = getelementptr i64, ptr %arr.data636, i64 8
  store i64 0, ptr %arr.elem637, align 8
  %arr.data.gep638 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data639 = load ptr, ptr %arr.data.gep638, align 8
  %arr.elem640 = getelementptr i64, ptr %arr.data639, i64 9
  store i64 0, ptr %arr.elem640, align 8
  %arr.data.gep641 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data642 = load ptr, ptr %arr.data.gep641, align 8
  %arr.elem643 = getelementptr i64, ptr %arr.data642, i64 10
  store i64 0, ptr %arr.elem643, align 8
  %arr.data.gep644 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data645 = load ptr, ptr %arr.data.gep644, align 8
  %arr.elem646 = getelementptr i64, ptr %arr.data645, i64 11
  store i64 0, ptr %arr.elem646, align 8
  %arr.data.gep647 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data648 = load ptr, ptr %arr.data.gep647, align 8
  %arr.elem649 = getelementptr i64, ptr %arr.data648, i64 12
  store i64 0, ptr %arr.elem649, align 8
  %arr.data.gep650 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data651 = load ptr, ptr %arr.data.gep650, align 8
  %arr.elem652 = getelementptr i64, ptr %arr.data651, i64 13
  store i64 0, ptr %arr.elem652, align 8
  %arr.data.gep653 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data654 = load ptr, ptr %arr.data.gep653, align 8
  %arr.elem655 = getelementptr i64, ptr %arr.data654, i64 14
  store i64 0, ptr %arr.elem655, align 8
  %arr.data.gep656 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data657 = load ptr, ptr %arr.data.gep656, align 8
  %arr.elem658 = getelementptr i64, ptr %arr.data657, i64 15
  store i64 0, ptr %arr.elem658, align 8
  %arr.data.gep659 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data660 = load ptr, ptr %arr.data.gep659, align 8
  %arr.elem661 = getelementptr i64, ptr %arr.data660, i64 16
  store i64 0, ptr %arr.elem661, align 8
  %arr.data.gep662 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data663 = load ptr, ptr %arr.data.gep662, align 8
  %arr.elem664 = getelementptr i64, ptr %arr.data663, i64 17
  store i64 0, ptr %arr.elem664, align 8
  %arr.data.gep665 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data666 = load ptr, ptr %arr.data.gep665, align 8
  %arr.elem667 = getelementptr i64, ptr %arr.data666, i64 18
  store i64 0, ptr %arr.elem667, align 8
  %arr.data.gep668 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data669 = load ptr, ptr %arr.data.gep668, align 8
  %arr.elem670 = getelementptr i64, ptr %arr.data669, i64 19
  store i64 0, ptr %arr.elem670, align 8
  %arr.data.gep671 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data672 = load ptr, ptr %arr.data.gep671, align 8
  %arr.elem673 = getelementptr i64, ptr %arr.data672, i64 20
  store i64 0, ptr %arr.elem673, align 8
  %arr.data.gep674 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data675 = load ptr, ptr %arr.data.gep674, align 8
  %arr.elem676 = getelementptr i64, ptr %arr.data675, i64 21
  store i64 0, ptr %arr.elem676, align 8
  %arr.data.gep677 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data678 = load ptr, ptr %arr.data.gep677, align 8
  %arr.elem679 = getelementptr i64, ptr %arr.data678, i64 22
  store i64 0, ptr %arr.elem679, align 8
  %arr.data.gep680 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data681 = load ptr, ptr %arr.data.gep680, align 8
  %arr.elem682 = getelementptr i64, ptr %arr.data681, i64 23
  store i64 0, ptr %arr.elem682, align 8
  %arr.data.gep683 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data684 = load ptr, ptr %arr.data.gep683, align 8
  %arr.elem685 = getelementptr i64, ptr %arr.data684, i64 24
  store i64 0, ptr %arr.elem685, align 8
  %arr.data.gep686 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data687 = load ptr, ptr %arr.data.gep686, align 8
  %arr.elem688 = getelementptr i64, ptr %arr.data687, i64 25
  store i64 0, ptr %arr.elem688, align 8
  %arr.data.gep689 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data690 = load ptr, ptr %arr.data.gep689, align 8
  %arr.elem691 = getelementptr i64, ptr %arr.data690, i64 26
  store i64 0, ptr %arr.elem691, align 8
  %arr.data.gep692 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data693 = load ptr, ptr %arr.data.gep692, align 8
  %arr.elem694 = getelementptr i64, ptr %arr.data693, i64 27
  store i64 0, ptr %arr.elem694, align 8
  %arr.data.gep695 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data696 = load ptr, ptr %arr.data.gep695, align 8
  %arr.elem697 = getelementptr i64, ptr %arr.data696, i64 28
  store i64 0, ptr %arr.elem697, align 8
  %arr.data.gep698 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data699 = load ptr, ptr %arr.data.gep698, align 8
  %arr.elem700 = getelementptr i64, ptr %arr.data699, i64 29
  store i64 0, ptr %arr.elem700, align 8
  %arr.data.gep701 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data702 = load ptr, ptr %arr.data.gep701, align 8
  %arr.elem703 = getelementptr i64, ptr %arr.data702, i64 30
  store i64 0, ptr %arr.elem703, align 8
  %arr.data.gep704 = getelementptr inbounds %SadArray, ptr %arr_new606, i32 0, i32 2
  %arr.data705 = load ptr, ptr %arr.data.gep704, align 8
  %arr.elem706 = getelementptr i64, ptr %arr.data705, i64 31
  store i64 0, ptr %arr.elem706, align 8
  %obj.ptrtoint707 = ptrtoint ptr %arr_new606 to i64
  store i64 %obj.ptrtoint707, ptr %"%fd_\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81", align 8
  %arr_new708 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep709 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 0
  store i64 32, ptr %arr.len.gep709, align 8
  %arr.cap.gep710 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep710, align 8
  %arr.data711 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep712 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  store ptr %arr.data711, ptr %arr.data.gep712, align 8
  %arr.data.gep713 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data714 = load ptr, ptr %arr.data.gep713, align 8
  %arr.elem715 = getelementptr i64, ptr %arr.data714, i64 0
  store i64 0, ptr %arr.elem715, align 8
  %arr.data.gep716 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data717 = load ptr, ptr %arr.data.gep716, align 8
  %arr.elem718 = getelementptr i64, ptr %arr.data717, i64 1
  store i64 0, ptr %arr.elem718, align 8
  %arr.data.gep719 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data720 = load ptr, ptr %arr.data.gep719, align 8
  %arr.elem721 = getelementptr i64, ptr %arr.data720, i64 2
  store i64 0, ptr %arr.elem721, align 8
  %arr.data.gep722 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data723 = load ptr, ptr %arr.data.gep722, align 8
  %arr.elem724 = getelementptr i64, ptr %arr.data723, i64 3
  store i64 0, ptr %arr.elem724, align 8
  %arr.data.gep725 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data726 = load ptr, ptr %arr.data.gep725, align 8
  %arr.elem727 = getelementptr i64, ptr %arr.data726, i64 4
  store i64 0, ptr %arr.elem727, align 8
  %arr.data.gep728 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data729 = load ptr, ptr %arr.data.gep728, align 8
  %arr.elem730 = getelementptr i64, ptr %arr.data729, i64 5
  store i64 0, ptr %arr.elem730, align 8
  %arr.data.gep731 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data732 = load ptr, ptr %arr.data.gep731, align 8
  %arr.elem733 = getelementptr i64, ptr %arr.data732, i64 6
  store i64 0, ptr %arr.elem733, align 8
  %arr.data.gep734 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data735 = load ptr, ptr %arr.data.gep734, align 8
  %arr.elem736 = getelementptr i64, ptr %arr.data735, i64 7
  store i64 0, ptr %arr.elem736, align 8
  %arr.data.gep737 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data738 = load ptr, ptr %arr.data.gep737, align 8
  %arr.elem739 = getelementptr i64, ptr %arr.data738, i64 8
  store i64 0, ptr %arr.elem739, align 8
  %arr.data.gep740 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data741 = load ptr, ptr %arr.data.gep740, align 8
  %arr.elem742 = getelementptr i64, ptr %arr.data741, i64 9
  store i64 0, ptr %arr.elem742, align 8
  %arr.data.gep743 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data744 = load ptr, ptr %arr.data.gep743, align 8
  %arr.elem745 = getelementptr i64, ptr %arr.data744, i64 10
  store i64 0, ptr %arr.elem745, align 8
  %arr.data.gep746 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data747 = load ptr, ptr %arr.data.gep746, align 8
  %arr.elem748 = getelementptr i64, ptr %arr.data747, i64 11
  store i64 0, ptr %arr.elem748, align 8
  %arr.data.gep749 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data750 = load ptr, ptr %arr.data.gep749, align 8
  %arr.elem751 = getelementptr i64, ptr %arr.data750, i64 12
  store i64 0, ptr %arr.elem751, align 8
  %arr.data.gep752 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data753 = load ptr, ptr %arr.data.gep752, align 8
  %arr.elem754 = getelementptr i64, ptr %arr.data753, i64 13
  store i64 0, ptr %arr.elem754, align 8
  %arr.data.gep755 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data756 = load ptr, ptr %arr.data.gep755, align 8
  %arr.elem757 = getelementptr i64, ptr %arr.data756, i64 14
  store i64 0, ptr %arr.elem757, align 8
  %arr.data.gep758 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data759 = load ptr, ptr %arr.data.gep758, align 8
  %arr.elem760 = getelementptr i64, ptr %arr.data759, i64 15
  store i64 0, ptr %arr.elem760, align 8
  %arr.data.gep761 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data762 = load ptr, ptr %arr.data.gep761, align 8
  %arr.elem763 = getelementptr i64, ptr %arr.data762, i64 16
  store i64 0, ptr %arr.elem763, align 8
  %arr.data.gep764 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data765 = load ptr, ptr %arr.data.gep764, align 8
  %arr.elem766 = getelementptr i64, ptr %arr.data765, i64 17
  store i64 0, ptr %arr.elem766, align 8
  %arr.data.gep767 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data768 = load ptr, ptr %arr.data.gep767, align 8
  %arr.elem769 = getelementptr i64, ptr %arr.data768, i64 18
  store i64 0, ptr %arr.elem769, align 8
  %arr.data.gep770 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data771 = load ptr, ptr %arr.data.gep770, align 8
  %arr.elem772 = getelementptr i64, ptr %arr.data771, i64 19
  store i64 0, ptr %arr.elem772, align 8
  %arr.data.gep773 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data774 = load ptr, ptr %arr.data.gep773, align 8
  %arr.elem775 = getelementptr i64, ptr %arr.data774, i64 20
  store i64 0, ptr %arr.elem775, align 8
  %arr.data.gep776 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data777 = load ptr, ptr %arr.data.gep776, align 8
  %arr.elem778 = getelementptr i64, ptr %arr.data777, i64 21
  store i64 0, ptr %arr.elem778, align 8
  %arr.data.gep779 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data780 = load ptr, ptr %arr.data.gep779, align 8
  %arr.elem781 = getelementptr i64, ptr %arr.data780, i64 22
  store i64 0, ptr %arr.elem781, align 8
  %arr.data.gep782 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data783 = load ptr, ptr %arr.data.gep782, align 8
  %arr.elem784 = getelementptr i64, ptr %arr.data783, i64 23
  store i64 0, ptr %arr.elem784, align 8
  %arr.data.gep785 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data786 = load ptr, ptr %arr.data.gep785, align 8
  %arr.elem787 = getelementptr i64, ptr %arr.data786, i64 24
  store i64 0, ptr %arr.elem787, align 8
  %arr.data.gep788 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data789 = load ptr, ptr %arr.data.gep788, align 8
  %arr.elem790 = getelementptr i64, ptr %arr.data789, i64 25
  store i64 0, ptr %arr.elem790, align 8
  %arr.data.gep791 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data792 = load ptr, ptr %arr.data.gep791, align 8
  %arr.elem793 = getelementptr i64, ptr %arr.data792, i64 26
  store i64 0, ptr %arr.elem793, align 8
  %arr.data.gep794 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data795 = load ptr, ptr %arr.data.gep794, align 8
  %arr.elem796 = getelementptr i64, ptr %arr.data795, i64 27
  store i64 0, ptr %arr.elem796, align 8
  %arr.data.gep797 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data798 = load ptr, ptr %arr.data.gep797, align 8
  %arr.elem799 = getelementptr i64, ptr %arr.data798, i64 28
  store i64 0, ptr %arr.elem799, align 8
  %arr.data.gep800 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data801 = load ptr, ptr %arr.data.gep800, align 8
  %arr.elem802 = getelementptr i64, ptr %arr.data801, i64 29
  store i64 0, ptr %arr.elem802, align 8
  %arr.data.gep803 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data804 = load ptr, ptr %arr.data.gep803, align 8
  %arr.elem805 = getelementptr i64, ptr %arr.data804, i64 30
  store i64 0, ptr %arr.elem805, align 8
  %arr.data.gep806 = getelementptr inbounds %SadArray, ptr %arr_new708, i32 0, i32 2
  %arr.data807 = load ptr, ptr %arr.data.gep806, align 8
  %arr.elem808 = getelementptr i64, ptr %arr.data807, i64 31
  store i64 0, ptr %arr.elem808, align 8
  %obj.ptrtoint809 = ptrtoint ptr %arr_new708 to i64
  store i64 %obj.ptrtoint809, ptr %"%fd_\D9\85\D8\AE\D8\B2\D9\86", align 8
  %arr_new810 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep811 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 0
  store i64 32, ptr %arr.len.gep811, align 8
  %arr.cap.gep812 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 1
  store i64 32, ptr %arr.cap.gep812, align 8
  %arr.data813 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 32))
  %arr.data.gep814 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  store ptr %arr.data813, ptr %arr.data.gep814, align 8
  %arr.data.gep815 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data816 = load ptr, ptr %arr.data.gep815, align 8
  %arr.elem817 = getelementptr i64, ptr %arr.data816, i64 0
  store i64 -1, ptr %arr.elem817, align 8
  %arr.data.gep818 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data819 = load ptr, ptr %arr.data.gep818, align 8
  %arr.elem820 = getelementptr i64, ptr %arr.data819, i64 1
  store i64 -1, ptr %arr.elem820, align 8
  %arr.data.gep821 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data822 = load ptr, ptr %arr.data.gep821, align 8
  %arr.elem823 = getelementptr i64, ptr %arr.data822, i64 2
  store i64 -1, ptr %arr.elem823, align 8
  %arr.data.gep824 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data825 = load ptr, ptr %arr.data.gep824, align 8
  %arr.elem826 = getelementptr i64, ptr %arr.data825, i64 3
  store i64 -1, ptr %arr.elem826, align 8
  %arr.data.gep827 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data828 = load ptr, ptr %arr.data.gep827, align 8
  %arr.elem829 = getelementptr i64, ptr %arr.data828, i64 4
  store i64 -1, ptr %arr.elem829, align 8
  %arr.data.gep830 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data831 = load ptr, ptr %arr.data.gep830, align 8
  %arr.elem832 = getelementptr i64, ptr %arr.data831, i64 5
  store i64 -1, ptr %arr.elem832, align 8
  %arr.data.gep833 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data834 = load ptr, ptr %arr.data.gep833, align 8
  %arr.elem835 = getelementptr i64, ptr %arr.data834, i64 6
  store i64 -1, ptr %arr.elem835, align 8
  %arr.data.gep836 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data837 = load ptr, ptr %arr.data.gep836, align 8
  %arr.elem838 = getelementptr i64, ptr %arr.data837, i64 7
  store i64 -1, ptr %arr.elem838, align 8
  %arr.data.gep839 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data840 = load ptr, ptr %arr.data.gep839, align 8
  %arr.elem841 = getelementptr i64, ptr %arr.data840, i64 8
  store i64 -1, ptr %arr.elem841, align 8
  %arr.data.gep842 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data843 = load ptr, ptr %arr.data.gep842, align 8
  %arr.elem844 = getelementptr i64, ptr %arr.data843, i64 9
  store i64 -1, ptr %arr.elem844, align 8
  %arr.data.gep845 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data846 = load ptr, ptr %arr.data.gep845, align 8
  %arr.elem847 = getelementptr i64, ptr %arr.data846, i64 10
  store i64 -1, ptr %arr.elem847, align 8
  %arr.data.gep848 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data849 = load ptr, ptr %arr.data.gep848, align 8
  %arr.elem850 = getelementptr i64, ptr %arr.data849, i64 11
  store i64 -1, ptr %arr.elem850, align 8
  %arr.data.gep851 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data852 = load ptr, ptr %arr.data.gep851, align 8
  %arr.elem853 = getelementptr i64, ptr %arr.data852, i64 12
  store i64 -1, ptr %arr.elem853, align 8
  %arr.data.gep854 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data855 = load ptr, ptr %arr.data.gep854, align 8
  %arr.elem856 = getelementptr i64, ptr %arr.data855, i64 13
  store i64 -1, ptr %arr.elem856, align 8
  %arr.data.gep857 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data858 = load ptr, ptr %arr.data.gep857, align 8
  %arr.elem859 = getelementptr i64, ptr %arr.data858, i64 14
  store i64 -1, ptr %arr.elem859, align 8
  %arr.data.gep860 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data861 = load ptr, ptr %arr.data.gep860, align 8
  %arr.elem862 = getelementptr i64, ptr %arr.data861, i64 15
  store i64 -1, ptr %arr.elem862, align 8
  %arr.data.gep863 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data864 = load ptr, ptr %arr.data.gep863, align 8
  %arr.elem865 = getelementptr i64, ptr %arr.data864, i64 16
  store i64 -1, ptr %arr.elem865, align 8
  %arr.data.gep866 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data867 = load ptr, ptr %arr.data.gep866, align 8
  %arr.elem868 = getelementptr i64, ptr %arr.data867, i64 17
  store i64 -1, ptr %arr.elem868, align 8
  %arr.data.gep869 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data870 = load ptr, ptr %arr.data.gep869, align 8
  %arr.elem871 = getelementptr i64, ptr %arr.data870, i64 18
  store i64 -1, ptr %arr.elem871, align 8
  %arr.data.gep872 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data873 = load ptr, ptr %arr.data.gep872, align 8
  %arr.elem874 = getelementptr i64, ptr %arr.data873, i64 19
  store i64 -1, ptr %arr.elem874, align 8
  %arr.data.gep875 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data876 = load ptr, ptr %arr.data.gep875, align 8
  %arr.elem877 = getelementptr i64, ptr %arr.data876, i64 20
  store i64 -1, ptr %arr.elem877, align 8
  %arr.data.gep878 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data879 = load ptr, ptr %arr.data.gep878, align 8
  %arr.elem880 = getelementptr i64, ptr %arr.data879, i64 21
  store i64 -1, ptr %arr.elem880, align 8
  %arr.data.gep881 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data882 = load ptr, ptr %arr.data.gep881, align 8
  %arr.elem883 = getelementptr i64, ptr %arr.data882, i64 22
  store i64 -1, ptr %arr.elem883, align 8
  %arr.data.gep884 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data885 = load ptr, ptr %arr.data.gep884, align 8
  %arr.elem886 = getelementptr i64, ptr %arr.data885, i64 23
  store i64 -1, ptr %arr.elem886, align 8
  %arr.data.gep887 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data888 = load ptr, ptr %arr.data.gep887, align 8
  %arr.elem889 = getelementptr i64, ptr %arr.data888, i64 24
  store i64 -1, ptr %arr.elem889, align 8
  %arr.data.gep890 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data891 = load ptr, ptr %arr.data.gep890, align 8
  %arr.elem892 = getelementptr i64, ptr %arr.data891, i64 25
  store i64 -1, ptr %arr.elem892, align 8
  %arr.data.gep893 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data894 = load ptr, ptr %arr.data.gep893, align 8
  %arr.elem895 = getelementptr i64, ptr %arr.data894, i64 26
  store i64 -1, ptr %arr.elem895, align 8
  %arr.data.gep896 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data897 = load ptr, ptr %arr.data.gep896, align 8
  %arr.elem898 = getelementptr i64, ptr %arr.data897, i64 27
  store i64 -1, ptr %arr.elem898, align 8
  %arr.data.gep899 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data900 = load ptr, ptr %arr.data.gep899, align 8
  %arr.elem901 = getelementptr i64, ptr %arr.data900, i64 28
  store i64 -1, ptr %arr.elem901, align 8
  %arr.data.gep902 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data903 = load ptr, ptr %arr.data.gep902, align 8
  %arr.elem904 = getelementptr i64, ptr %arr.data903, i64 29
  store i64 -1, ptr %arr.elem904, align 8
  %arr.data.gep905 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data906 = load ptr, ptr %arr.data.gep905, align 8
  %arr.elem907 = getelementptr i64, ptr %arr.data906, i64 30
  store i64 -1, ptr %arr.elem907, align 8
  %arr.data.gep908 = getelementptr inbounds %SadArray, ptr %arr_new810, i32 0, i32 2
  %arr.data909 = load ptr, ptr %arr.data.gep908, align 8
  %arr.elem910 = getelementptr i64, ptr %arr.data909, i64 31
  store i64 -1, ptr %arr.elem910, align 8
  %obj.ptrtoint911 = ptrtoint ptr %arr_new810 to i64
  store i64 %obj.ptrtoint911, ptr %"%fd_\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D8\AD\D9\85\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D9\88\D8\AD\D8\A9", align 8
  store i64 0, ptr %"%fop_\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B9\D9\85\D9\84", align 8
  store i64 0, ptr %"%fop_\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\B9\D9\85\D9\842", align 8
  store i64 0, ptr %"%fop_\D9\85\D9\87\D9\8A\D8\A3", align 8
  %arr_new912 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep913 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 0
  store i64 16, ptr %arr.len.gep913, align 8
  %arr.cap.gep914 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep914, align 8
  %arr.data915 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep916 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  store ptr %arr.data915, ptr %arr.data.gep916, align 8
  %arr.data.gep917 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data918 = load ptr, ptr %arr.data.gep917, align 8
  %arr.elem919 = getelementptr i64, ptr %arr.data918, i64 0
  store i64 0, ptr %arr.elem919, align 8
  %arr.data.gep920 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data921 = load ptr, ptr %arr.data.gep920, align 8
  %arr.elem922 = getelementptr i64, ptr %arr.data921, i64 1
  store i64 0, ptr %arr.elem922, align 8
  %arr.data.gep923 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data924 = load ptr, ptr %arr.data.gep923, align 8
  %arr.elem925 = getelementptr i64, ptr %arr.data924, i64 2
  store i64 0, ptr %arr.elem925, align 8
  %arr.data.gep926 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data927 = load ptr, ptr %arr.data.gep926, align 8
  %arr.elem928 = getelementptr i64, ptr %arr.data927, i64 3
  store i64 0, ptr %arr.elem928, align 8
  %arr.data.gep929 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data930 = load ptr, ptr %arr.data.gep929, align 8
  %arr.elem931 = getelementptr i64, ptr %arr.data930, i64 4
  store i64 0, ptr %arr.elem931, align 8
  %arr.data.gep932 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data933 = load ptr, ptr %arr.data.gep932, align 8
  %arr.elem934 = getelementptr i64, ptr %arr.data933, i64 5
  store i64 0, ptr %arr.elem934, align 8
  %arr.data.gep935 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data936 = load ptr, ptr %arr.data.gep935, align 8
  %arr.elem937 = getelementptr i64, ptr %arr.data936, i64 6
  store i64 0, ptr %arr.elem937, align 8
  %arr.data.gep938 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data939 = load ptr, ptr %arr.data.gep938, align 8
  %arr.elem940 = getelementptr i64, ptr %arr.data939, i64 7
  store i64 0, ptr %arr.elem940, align 8
  %arr.data.gep941 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data942 = load ptr, ptr %arr.data.gep941, align 8
  %arr.elem943 = getelementptr i64, ptr %arr.data942, i64 8
  store i64 0, ptr %arr.elem943, align 8
  %arr.data.gep944 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data945 = load ptr, ptr %arr.data.gep944, align 8
  %arr.elem946 = getelementptr i64, ptr %arr.data945, i64 9
  store i64 0, ptr %arr.elem946, align 8
  %arr.data.gep947 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data948 = load ptr, ptr %arr.data.gep947, align 8
  %arr.elem949 = getelementptr i64, ptr %arr.data948, i64 10
  store i64 0, ptr %arr.elem949, align 8
  %arr.data.gep950 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data951 = load ptr, ptr %arr.data.gep950, align 8
  %arr.elem952 = getelementptr i64, ptr %arr.data951, i64 11
  store i64 0, ptr %arr.elem952, align 8
  %arr.data.gep953 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data954 = load ptr, ptr %arr.data.gep953, align 8
  %arr.elem955 = getelementptr i64, ptr %arr.data954, i64 12
  store i64 0, ptr %arr.elem955, align 8
  %arr.data.gep956 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data957 = load ptr, ptr %arr.data.gep956, align 8
  %arr.elem958 = getelementptr i64, ptr %arr.data957, i64 13
  store i64 0, ptr %arr.elem958, align 8
  %arr.data.gep959 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data960 = load ptr, ptr %arr.data.gep959, align 8
  %arr.elem961 = getelementptr i64, ptr %arr.data960, i64 14
  store i64 0, ptr %arr.elem961, align 8
  %arr.data.gep962 = getelementptr inbounds %SadArray, ptr %arr_new912, i32 0, i32 2
  %arr.data963 = load ptr, ptr %arr.data.gep962, align 8
  %arr.elem964 = getelementptr i64, ptr %arr.data963, i64 15
  store i64 0, ptr %arr.elem964, align 8
  %obj.ptrtoint965 = ptrtoint ptr %arr_new912 to i64
  store i64 %obj.ptrtoint965, ptr %"%\D9\85\D8\B3\D8\A7\D8\B1_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %arr_new966 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep967 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 0
  store i64 16, ptr %arr.len.gep967, align 8
  %arr.cap.gep968 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep968, align 8
  %arr.data969 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep970 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  store ptr %arr.data969, ptr %arr.data.gep970, align 8
  %arr.data.gep971 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data972 = load ptr, ptr %arr.data.gep971, align 8
  %arr.elem973 = getelementptr i64, ptr %arr.data972, i64 0
  store i64 0, ptr %arr.elem973, align 8
  %arr.data.gep974 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data975 = load ptr, ptr %arr.data.gep974, align 8
  %arr.elem976 = getelementptr i64, ptr %arr.data975, i64 1
  store i64 0, ptr %arr.elem976, align 8
  %arr.data.gep977 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data978 = load ptr, ptr %arr.data.gep977, align 8
  %arr.elem979 = getelementptr i64, ptr %arr.data978, i64 2
  store i64 0, ptr %arr.elem979, align 8
  %arr.data.gep980 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data981 = load ptr, ptr %arr.data.gep980, align 8
  %arr.elem982 = getelementptr i64, ptr %arr.data981, i64 3
  store i64 0, ptr %arr.elem982, align 8
  %arr.data.gep983 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data984 = load ptr, ptr %arr.data.gep983, align 8
  %arr.elem985 = getelementptr i64, ptr %arr.data984, i64 4
  store i64 0, ptr %arr.elem985, align 8
  %arr.data.gep986 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data987 = load ptr, ptr %arr.data.gep986, align 8
  %arr.elem988 = getelementptr i64, ptr %arr.data987, i64 5
  store i64 0, ptr %arr.elem988, align 8
  %arr.data.gep989 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data990 = load ptr, ptr %arr.data.gep989, align 8
  %arr.elem991 = getelementptr i64, ptr %arr.data990, i64 6
  store i64 0, ptr %arr.elem991, align 8
  %arr.data.gep992 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data993 = load ptr, ptr %arr.data.gep992, align 8
  %arr.elem994 = getelementptr i64, ptr %arr.data993, i64 7
  store i64 0, ptr %arr.elem994, align 8
  %arr.data.gep995 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data996 = load ptr, ptr %arr.data.gep995, align 8
  %arr.elem997 = getelementptr i64, ptr %arr.data996, i64 8
  store i64 0, ptr %arr.elem997, align 8
  %arr.data.gep998 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data999 = load ptr, ptr %arr.data.gep998, align 8
  %arr.elem1000 = getelementptr i64, ptr %arr.data999, i64 9
  store i64 0, ptr %arr.elem1000, align 8
  %arr.data.gep1001 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data1002 = load ptr, ptr %arr.data.gep1001, align 8
  %arr.elem1003 = getelementptr i64, ptr %arr.data1002, i64 10
  store i64 0, ptr %arr.elem1003, align 8
  %arr.data.gep1004 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data1005 = load ptr, ptr %arr.data.gep1004, align 8
  %arr.elem1006 = getelementptr i64, ptr %arr.data1005, i64 11
  store i64 0, ptr %arr.elem1006, align 8
  %arr.data.gep1007 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data1008 = load ptr, ptr %arr.data.gep1007, align 8
  %arr.elem1009 = getelementptr i64, ptr %arr.data1008, i64 12
  store i64 0, ptr %arr.elem1009, align 8
  %arr.data.gep1010 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data1011 = load ptr, ptr %arr.data.gep1010, align 8
  %arr.elem1012 = getelementptr i64, ptr %arr.data1011, i64 13
  store i64 0, ptr %arr.elem1012, align 8
  %arr.data.gep1013 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data1014 = load ptr, ptr %arr.data.gep1013, align 8
  %arr.elem1015 = getelementptr i64, ptr %arr.data1014, i64 14
  store i64 0, ptr %arr.elem1015, align 8
  %arr.data.gep1016 = getelementptr inbounds %SadArray, ptr %arr_new966, i32 0, i32 2
  %arr.data1017 = load ptr, ptr %arr.data.gep1016, align 8
  %arr.elem1018 = getelementptr i64, ptr %arr.data1017, i64 15
  store i64 0, ptr %arr.elem1018, align 8
  %obj.ptrtoint1019 = ptrtoint ptr %arr_new966 to i64
  store i64 %obj.ptrtoint1019, ptr %"%\D9\85\D8\B3\D8\A7\D8\B1_\D8\B7\D9\88\D9\84", align 8
  store i64 0, ptr %"%\D9\85\D8\B3\D8\A7\D8\B1_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\83\D9\88\D9\86\D8\A7\D8\AA", align 8
  ret void
}

declare void @"\D9\81op_\D8\A7\D9\83\D8\AA\D8\A8_\D8\B0\D8\A7\D9\83\D8\B1\D8\A932"(i64, i64)

declare ptr @malloc(i64)

declare void @"\D9\81op_\D8\A7\D9\83\D8\AA\D8\A8_\D8\B0\D8\A7\D9\83\D8\B1\D8\A916"(i64, i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
