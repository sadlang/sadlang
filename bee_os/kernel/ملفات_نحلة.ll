; ModuleID = 'ملفات_نحلة'
source_filename = "\D9\85\D9\84\D9\81\D8\A7\D8\AA_\D9\86\D8\AD\D9\84\D8\A9"
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

declare void @sad_ll_pages_alloc(i64)

declare void @sad_ll_pages_free(i64, i64)

declare void @"blk_\D8\A7\D9\82\D8\B1\D8\A3"(i64, i64, i64)

declare void @"blk_\D8\A7\D9\83\D8\AA\D8\A8"(i64, i64, i64)

declare void @"blk_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D8\AA\D8\B9\D8\AF\D8\AF"(i64, i64, i64, i64)

declare void @"blk_\D9\81\D8\B1\D9\91\D8\BA"()

define void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
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
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_2:                                           ; preds = %merge_1
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_3
}

define i64 @"\D9\87\D9\8A\D8\A6_\D9\86\D8\B8\D8\A7\D9\85_\D8\A7\D9\84\D9\85\D9\84\D9\81\D8\A7\D8\AA"() {
entry:
  %"%\D9\85" = alloca i64, align 8
  br i1 true, label %then_4, label %merge_5

merge_5:                                          ; preds = %entry
  br i1 true, label %then_6, label %merge_7

merge_7:                                          ; preds = %merge_5
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_8

then_4:                                           ; preds = %entry
  ret i64 -1

then_6:                                           ; preds = %merge_5
  ret i64 -1

while_body_9:                                     ; preds = %while_cond_8
  %"%\D9\85.load1" = load i64, ptr %"%\D9\85", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\85.load1"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\85.load2" = load i64, ptr %"%\D9\85", align 8
  %addtmp = add i64 %"%\D9\85.load2", 1
  store i64 %addtmp, ptr %"%\D9\85", align 8
  br label %while_cond_8

while_cond_8:                                     ; preds = %while_body_9, %merge_7
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", 0
  br i1 %cmplttmp, label %while_body_9, label %while_exit_10

while_exit_10:                                    ; preds = %while_cond_8
  ret i64 0
}

define i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A5\D9\84\D9\89_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %multmp = mul i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load", 0
  %addtmp = add i64 %arr.get, %multmp
  ret i64 %addtmp
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9_FS"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A5\D9\84\D9\89_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_11

merge_15:                                         ; preds = %while_body_12
  %"%\D9\82.load8" = load i64, ptr %"%\D9\82", align 8
  %addtmp9 = add i64 %"%\D9\82.load8", 1
  store i64 %addtmp9, ptr %"%\D9\82", align 8
  br label %while_cond_11

then_14:                                          ; preds = %while_body_12
  ret i64 -1

while_body_12:                                    ; preds = %while_cond_11
  %"%\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  %"%\D9\82.load5" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D9\82\D8\B7\D8\A7\D8\B9.load", %"%\D9\82.load5"
  %"%\D9\82.load6" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load6", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A93", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load", %multmp
  %"%\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  call void @"blk_\D8\A7\D9\82\D8\B1\D8\A3"(i64 %"%\D9\82\D8\B1\D8\B5.load", i64 %addtmp, i64 %addtmp7)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_14, label %merge_15

while_cond_11:                                    ; preds = %merge_15, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp, label %while_body_12, label %while_exit_13

while_exit_13:                                    ; preds = %while_cond_11
  ret i64 0
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9_FS"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A5\D9\84\D9\89_\D9\82\D8\B7\D8\A7\D8\B9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_16

merge_20:                                         ; preds = %while_body_17
  %"%\D9\82.load8" = load i64, ptr %"%\D9\82", align 8
  %addtmp9 = add i64 %"%\D9\82.load8", 1
  store i64 %addtmp9, ptr %"%\D9\82", align 8
  br label %while_cond_16

then_19:                                          ; preds = %while_body_17
  ret i64 -1

while_body_17:                                    ; preds = %while_cond_16
  %"%\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9", align 8
  %"%\D9\82.load5" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D9\82\D8\B7\D8\A7\D8\B9.load", %"%\D9\82.load5"
  %"%\D9\82.load6" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load6", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load", %multmp
  %"%\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  call void @"blk_\D8\A7\D9\83\D8\AA\D8\A8"(i64 %"%\D9\82\D8\B1\D8\B5.load", i64 %addtmp, i64 %addtmp7)
  store i64 0, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_19, label %merge_20

while_cond_16:                                    ; preds = %merge_20, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp, label %while_body_17, label %while_exit_18

while_exit_18:                                    ; preds = %while_cond_16
  ret i64 0
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\81\D8\A7\D8\A6\D9\82\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_21, label %merge_22

merge_22:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  store i64 0, ptr %arr.elem, align 8
  call void @sad_ll_mem_read32(i64 4)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2"
  store i64 0, ptr %arr.elem4, align 8
  call void @sad_ll_mem_read64(i64 8)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5"
  store i64 0, ptr %arr.elem7, align 8
  call void @sad_ll_mem_read64(i64 16)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8"
  store i64 0, ptr %arr.elem10, align 8
  call void @sad_ll_mem_read64(i64 24)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load11"
  store i64 0, ptr %arr.elem13, align 8
  call void @sad_ll_mem_read64(i64 32)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14"
  store i64 0, ptr %arr.elem16, align 8
  call void @sad_ll_mem_read32(i64 40)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load17" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load17"
  store i64 0, ptr %arr.elem19, align 8
  call void @sad_ll_mem_read32(i64 44)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load20" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load20"
  store i64 0, ptr %arr.elem22, align 8
  call void @sad_ll_mem_read32(i64 48)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load23" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load23"
  store i64 0, ptr %arr.elem25, align 8
  call void @sad_ll_mem_read32(i64 52)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load26" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load26"
  store i64 0, ptr %arr.elem28, align 8
  call void @sad_ll_mem_read32(i64 56)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load29" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data30 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load29"
  store i64 0, ptr %arr.elem31, align 8
  call void @sad_ll_mem_read32(i64 60)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load32" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data33 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load32"
  store i64 0, ptr %arr.elem34, align 8
  ret i64 0

then_21:                                          ; preds = %entry
  ret i64 -1
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\81\D8\A7\D8\A6\D9\82\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @sad_ll_mem_write32(i64 4, i64 %arr.get)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  call void @sad_ll_mem_write64(i64 8, i64 %arr.get5)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  call void @sad_ll_mem_write64(i64 16, i64 %arr.get9)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load10" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  call void @sad_ll_mem_write64(i64 24, i64 %arr.get13)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14"
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  call void @sad_ll_mem_write64(i64 32, i64 %arr.get17)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load18" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load18"
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  call void @sad_ll_mem_write32(i64 40, i64 %arr.get21)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load22" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load22"
  %arr.get25 = load i64, ptr %arr.elem24, align 8
  call void @sad_ll_mem_write32(i64 44, i64 %arr.get25)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load26" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load26"
  %arr.get29 = load i64, ptr %arr.elem28, align 8
  call void @sad_ll_mem_write32(i64 48, i64 %arr.get29)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load30" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load30"
  %arr.get33 = load i64, ptr %arr.elem32, align 8
  call void @sad_ll_mem_write32(i64 52, i64 %arr.get33)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load34" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data35 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load34"
  %arr.get37 = load i64, ptr %arr.elem36, align 8
  call void @sad_ll_mem_write32(i64 56, i64 %arr.get37)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load38" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data39 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load38"
  %arr.get41 = load i64, ptr %arr.elem40, align 8
  call void @sad_ll_mem_write32(i64 60, i64 %arr.get41)
  call void @sad_ll_mem_write32(i64 68, i64 0)
  ret i64 0
}

define i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %multmp = mul i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load", %arr.get
  ret i64 %multmp
}

define i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D9\83\D8\AA\D9\84"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  %addtmp = add i64 %calltmp, 0
  ret i64 %addtmp
}

define i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  %addtmp = add i64 %calltmp, 0
  ret i64 %addtmp
}

define i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\88\D9\84_\D8\A7\D9\84\D8\B9\D9\82\D8\AF"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  %addtmp = add i64 %calltmp, 0
  ret i64 %addtmp
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A8\D8\AA"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA") {
entry:
  %"%\D8\A8" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA" = alloca i64, align 8
  %"%pair" = alloca i64, align 8
  %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load", 3
  store i64 %shrtmp, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %andtmp = and i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load3", 7
  store i64 %andtmp, ptr %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %andtmp4 = and i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load", 1
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load5" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %subtmp = sub i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load5", %andtmp4
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load", %subtmp
  call void @sad_ll_mem_read16(i64 %addtmp)
  store i64 0, ptr %"%pair", align 8
  %"%pair.load" = load i64, ptr %"%pair", align 8
  store i64 %"%pair.load", ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  %cmpeqtmp = icmp eq i64 %andtmp4, 1
  br i1 %cmpeqtmp, label %then_23, label %else_25

else_25:                                          ; preds = %entry
  %"%pair.load8" = load i64, ptr %"%pair", align 8
  %andtmp9 = and i64 %"%pair.load8", 255
  store i64 %andtmp9, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  br label %merge_24

merge_24:                                         ; preds = %else_25, %then_23
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_26

merge_30:                                         ; preds = %while_exit_28
  ret i64 0

then_23:                                          ; preds = %entry
  %"%pair.load6" = load i64, ptr %"%pair", align 8
  %shrtmp7 = lshr i64 %"%pair.load6", 8
  store i64 %shrtmp7, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  br label %merge_24

then_29:                                          ; preds = %while_exit_28
  ret i64 1

while_body_27:                                    ; preds = %while_cond_26
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D8\A8.load10" = load i64, ptr %"%\D8\A8", align 8
  %addtmp11 = add i64 %"%\D8\A8.load10", 1
  store i64 %addtmp11, ptr %"%\D8\A8", align 8
  br label %while_cond_26

while_cond_26:                                    ; preds = %while_body_27, %merge_24
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8.load", %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  br i1 %cmplttmp, label %while_body_27, label %while_exit_28

while_exit_28:                                    ; preds = %while_cond_26
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load12" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load12"
  %andtmp13 = and i64 %divtmp, 1
  %cmpeqtmp14 = icmp eq i64 %andtmp13, 1
  br i1 %cmpeqtmp14, label %then_29, label %merge_30
}

define void @"\D8\B9\D9\8A\D9\86_\D8\A8\D8\AA"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA") {
entry:
  %"%\D8\A8" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo" = alloca i64, align 8
  %"%pair" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load", 3
  store i64 %shrtmp, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %andtmp = and i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load3", 7
  store i64 %andtmp, ptr %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %andtmp4 = and i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load", 1
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load5" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %subtmp = sub i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load5", %andtmp4
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load", %subtmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_mem_read16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load")
  store i64 0, ptr %"%pair", align 8
  %"%pair.load" = load i64, ptr %"%pair", align 8
  %andtmp6 = and i64 %"%pair.load", 255
  store i64 %andtmp6, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %"%pair.load7" = load i64, ptr %"%pair", align 8
  %shrtmp8 = lshr i64 %"%pair.load7", 8
  store i64 %shrtmp8, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_31

else_36:                                          ; preds = %while_exit_33
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load16" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp17 = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load16"
  %andtmp18 = and i64 %divtmp17, 1
  %cmpeqtmp19 = icmp eq i64 %andtmp18, 0
  br i1 %cmpeqtmp19, label %then_39, label %merge_40

merge_35:                                         ; preds = %merge_40, %merge_38
  ret void

merge_38:                                         ; preds = %then_37, %then_34
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load23" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %shltmp24 = shl i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load23", 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load25" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %addtmp26 = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load25", %shltmp24
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load27" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load27", i64 %addtmp26)
  br label %merge_35

merge_40:                                         ; preds = %then_39, %else_36
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load31" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %shltmp32 = shl i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load31", 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load33" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %addtmp34 = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load33", %shltmp32
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load35" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load35", i64 %addtmp34)
  br label %merge_35

then_34:                                          ; preds = %while_exit_33
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load13" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load13"
  %andtmp14 = and i64 %divtmp, 1
  %cmpeqtmp15 = icmp eq i64 %andtmp14, 0
  br i1 %cmpeqtmp15, label %then_37, label %merge_38

then_37:                                          ; preds = %then_34
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load20" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load21" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %addtmp22 = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load20", %"%\D9\82\D9\86\D8\A7\D8\B9.load21"
  store i64 %addtmp22, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  br label %merge_38

then_39:                                          ; preds = %else_36
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load28" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load29" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %addtmp30 = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load28", %"%\D9\82\D9\86\D8\A7\D8\B9.load29"
  store i64 %addtmp30, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  br label %merge_40

while_body_32:                                    ; preds = %while_cond_31
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D8\A8.load9" = load i64, ptr %"%\D8\A8", align 8
  %addtmp10 = add i64 %"%\D8\A8.load9", 1
  store i64 %addtmp10, ptr %"%\D8\A8", align 8
  br label %while_cond_31

while_cond_31:                                    ; preds = %while_body_32, %entry
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8.load", %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  br i1 %cmplttmp, label %while_body_32, label %while_exit_33

while_exit_33:                                    ; preds = %while_cond_31
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load11" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %andtmp12 = and i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load11", 1
  %cmpeqtmp = icmp eq i64 %andtmp12, 0
  br i1 %cmpeqtmp, label %then_34, label %else_36
}

define void @"\D8\A7\D9\85\D8\B3\D8\AD_\D8\A8\D8\AA"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA") {
entry:
  %"%\D8\A8" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo" = alloca i64, align 8
  %"%pair" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load", 3
  store i64 %shrtmp, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA2", align 8
  %andtmp = and i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\A8\D8\AA.load3", 7
  store i64 %andtmp, ptr %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %andtmp4 = and i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load", 1
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load5" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %subtmp = sub i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load5", %andtmp4
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load", %subtmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_mem_read16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load")
  store i64 0, ptr %"%pair", align 8
  %"%pair.load" = load i64, ptr %"%pair", align 8
  %andtmp6 = and i64 %"%pair.load", 255
  store i64 %andtmp6, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %"%pair.load7" = load i64, ptr %"%pair", align 8
  %shrtmp8 = lshr i64 %"%pair.load7", 8
  store i64 %shrtmp8, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_41

else_46:                                          ; preds = %while_exit_43
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load16" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp17 = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load16"
  %andtmp18 = and i64 %divtmp17, 1
  %cmpeqtmp19 = icmp eq i64 %andtmp18, 1
  br i1 %cmpeqtmp19, label %then_49, label %merge_50

merge_45:                                         ; preds = %merge_50, %merge_48
  ret void

merge_48:                                         ; preds = %then_47, %then_44
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load23" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %shltmp24 = shl i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load23", 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load25" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %addtmp26 = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load25", %shltmp24
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load27" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load27", i64 %addtmp26)
  br label %merge_45

merge_50:                                         ; preds = %then_49, %else_46
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load31" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %shltmp32 = shl i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load31", 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load33" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %addtmp34 = add i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load33", %shltmp32
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load35" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load35", i64 %addtmp34)
  br label %merge_45

then_44:                                          ; preds = %while_exit_43
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load13" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load13"
  %andtmp14 = and i64 %divtmp, 1
  %cmpeqtmp15 = icmp eq i64 %andtmp14, 1
  br i1 %cmpeqtmp15, label %then_47, label %merge_48

then_47:                                          ; preds = %then_44
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load20" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load21" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %subtmp22 = sub i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo.load20", %"%\D9\82\D9\86\D8\A7\D8\B9.load21"
  store i64 %subtmp22, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_lo", align 8
  br label %merge_48

then_49:                                          ; preds = %else_46
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load28" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load29" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %subtmp30 = sub i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi.load28", %"%\D9\82\D9\86\D8\A7\D8\B9.load29"
  store i64 %subtmp30, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA_hi", align 8
  br label %merge_50

while_body_42:                                    ; preds = %while_cond_41
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D8\A8.load9" = load i64, ptr %"%\D8\A8", align 8
  %addtmp10 = add i64 %"%\D8\A8.load9", 1
  store i64 %addtmp10, ptr %"%\D8\A8", align 8
  br label %while_cond_41

while_cond_41:                                    ; preds = %while_body_42, %entry
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8.load", %"%\D8\A8\D8\AA_\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  br i1 %cmplttmp, label %while_body_42, label %while_exit_43

while_exit_43:                                    ; preds = %while_cond_41
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load11" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3", align 8
  %andtmp12 = and i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\81\D9\87\D8\B1\D8\B3.load11", 1
  %cmpeqtmp = icmp eq i64 %andtmp12, 0
  br i1 %cmpeqtmp, label %then_44, label %else_46
}

define i64 @"\D8\A7\D8\A8\D8\AD\D8\AB_\D8\A8\D8\AA_\D8\AD\D8\B1"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA") {
entry:
  %"%\D8\A8" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_51

merge_55:                                         ; preds = %while_body_52
  %"%\D8\A8.load4" = load i64, ptr %"%\D8\A8", align 8
  %addtmp = add i64 %"%\D8\A8.load4", 1
  store i64 %addtmp, ptr %"%\D8\A8", align 8
  br label %while_cond_51

then_54:                                          ; preds = %while_body_52
  %loadtmp = load i64, ptr %"%\D8\A8", align 8
  ret i64 %loadtmp

while_body_52:                                    ; preds = %while_cond_51
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A91", align 8
  %"%\D8\A8.load3" = load i64, ptr %"%\D8\A8", align 8
  %calltmp = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A8\D8\AA"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9.load", i64 %"%\D8\A8.load3")
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_54, label %merge_55

while_cond_51:                                    ; preds = %merge_55, %entry
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\AA\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_52, label %while_exit_53

while_exit_53:                                    ; preds = %while_cond_51
  ret i64 -1
}

define i64 @"\D8\AE\D8\B5\D8\B5_\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9") {
entry:
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D8\B7\D9\84\D9\82\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AA_\D8\AD\D8\B1" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D9\83\D8\AA\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\B1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load", 0
  br i1 %cmplttmp, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %subtmp = sub i64 %arr.get7, 1
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8"
  store i64 %subtmp, ptr %arr.elem10, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load12" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %calltmp13 = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load11", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load12")
  %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load14" = load i64, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\B1", align 8
  %addtmp = add i64 %calltmp13, %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load14"
  store i64 %addtmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D8\B7\D9\84\D9\82\D8\A9", align 8
  %loadtmp = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D8\B7\D9\84\D9\82\D8\A9", align 8
  ret i64 %loadtmp

then_56:                                          ; preds = %entry
  ret i64 -1

then_58:                                          ; preds = %merge_57
  ret i64 -1
}

define i64 @"\D8\AE\D8\B5\D8\B5_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84") {
entry:
  %"%\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AC" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_60, label %merge_61

merge_61:                                         ; preds = %entry
  store i64 0, ptr %"%\D9\85\D8\AC", align 8
  br label %while_cond_62

merge_66:                                         ; preds = %while_body_63
  %"%\D9\85\D8\AC.load8" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %addtmp = add i64 %"%\D9\85\D8\AC.load8", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AC", align 8
  br label %while_cond_62

then_60:                                          ; preds = %entry
  ret i64 -1

then_65:                                          ; preds = %while_body_63
  %loadtmp = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  ret i64 %loadtmp

while_body_63:                                    ; preds = %while_cond_62
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D9\85\D8\AC.load7" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %calltmp = call i64 @"\D8\AE\D8\B5\D8\B5_\D9\83\D8\AA\D9\84\D8\A9_\D9\81\D9\8A_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6", i64 %"%\D9\85\D8\AC.load7")
  store i64 %calltmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\83\D8\AA\D9\84\D8\A9.load", 0
  br i1 %cmpgetmp, label %then_65, label %merge_66

while_cond_62:                                    ; preds = %merge_66, %merge_61
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %"%\D9\85\D8\AC.load" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AC.load", %arr.get5
  br i1 %cmplttmp, label %while_body_63, label %while_exit_64

while_exit_64:                                    ; preds = %while_cond_62
  ret i64 -1
}

define i64 @"\D8\AD\D8\B1\D8\B1_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %divtmp = sdiv i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load", %arr.get
  store i64 %divtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %modtmp = srem i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load7", %arr.get6
  store i64 %modtmp, ptr %"%\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D9\83\D8\AA\D9\84"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_67, label %merge_68

merge_68:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load9" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %addtmp = add i64 %arr.get12, 1
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load13" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load13"
  store i64 %addtmp, ptr %arr.elem15, align 8
  ret i64 0

then_67:                                          ; preds = %entry
  ret i64 -1
}

define i64 @"\D8\AE\D8\B5\D8\B5_\D8\B9\D9\82\D8\AF\D8\A9_\D9\81\D9\8A_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9") {
entry:
  %"%\D8\A8\D8\AA_\D8\AD\D8\B1" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_69, label %merge_70

merge_70:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load" = load i64, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\B1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load", 0
  br i1 %cmplttmp, label %then_71, label %merge_72

merge_72:                                         ; preds = %merge_70
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %subtmp = sub i64 %arr.get7, 1
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8"
  store i64 %subtmp, ptr %arr.elem10, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A92", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %multmp = mul i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load11", %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load"
  %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load12" = load i64, ptr %"%\D8\A8\D8\AA_\D8\AD\D8\B1", align 8
  %addtmp = add i64 %multmp, %"%\D8\A8\D8\AA_\D8\AD\D8\B1.load12"
  ret i64 %addtmp

then_69:                                          ; preds = %entry
  ret i64 -1

then_71:                                          ; preds = %merge_70
  ret i64 -1
}

define i64 @"\D8\AE\D8\B5\D8\B5_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84") {
entry:
  %"%\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AC" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_73, label %merge_74

merge_74:                                         ; preds = %entry
  store i64 0, ptr %"%\D9\85\D8\AC", align 8
  br label %while_cond_75

merge_79:                                         ; preds = %while_body_76
  %"%\D9\85\D8\AC.load8" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %addtmp = add i64 %"%\D9\85\D8\AC.load8", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AC", align 8
  br label %while_cond_75

then_73:                                          ; preds = %entry
  ret i64 -1

then_78:                                          ; preds = %while_body_76
  %loadtmp = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9", align 8
  ret i64 %loadtmp

while_body_76:                                    ; preds = %while_cond_75
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D9\85\D8\AC.load7" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %calltmp = call i64 @"\D8\AE\D8\B5\D8\B5_\D8\B9\D9\82\D8\AF\D8\A9_\D9\81\D9\8A_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6", i64 %"%\D9\85\D8\AC.load7")
  store i64 %calltmp, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpgetmp, label %then_78, label %merge_79

while_cond_75:                                    ; preds = %merge_79, %merge_74
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %"%\D9\85\D8\AC.load" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AC.load", %arr.get5
  br i1 %cmplttmp, label %while_body_76, label %while_exit_77

while_exit_77:                                    ; preds = %while_cond_75
  ret i64 -1
}

define i64 @"\D8\AD\D8\B1\D8\B1_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %divtmp = sdiv i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load"
  store i64 %divtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load4" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %modtmp = srem i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load3", %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load4"
  store i64 %modtmp, ptr %"%\D8\A7\D9\84\D8\A8\D8\AA", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load5", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_80, label %merge_81

merge_81:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %addtmp = add i64 %arr.get9, 1
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load10" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load10"
  store i64 %addtmp, ptr %arr.elem12, align 8
  ret i64 0

then_80:                                          ; preds = %entry
  ret i64 -1
}

define i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\86\D8\B3\D8\A8\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %divtmp = sdiv i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load"
  store i64 %divtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load4" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %modtmp = srem i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load3", %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load4"
  store i64 %modtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A", align 8
  store i64 poison, ptr %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A", align 8
  %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %divtmp5 = sdiv i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A.load", %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load"
  store i64 %divtmp5, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\86\D8\B3\D8\A8\D9\8A\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\88\D9\84_\D8\A7\D9\84\D8\B9\D9\82\D8\AF"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load")
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\86\D8\B3\D8\A8\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\86\D8\B3\D8\A8\D9\8A\D8\A9", align 8
  %addtmp = add i64 %calltmp, %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\86\D8\B3\D8\A8\D9\8A\D8\A9.load"
  ret i64 %addtmp
}

define i64 @"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %modtmp = srem i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load"
  store i64 %modtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A", align 8
  store i64 poison, ptr %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A", align 8
  %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %modtmp3 = srem i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\85\D8\AD\D9\84\D9\8A.load", %"%\D8\B9\D9\82\D8\AF_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9.load"
  %multmp = mul i64 %modtmp3, 0
  store i64 %multmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %loadtmp = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %calltmp5 = call i64 @"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load4")
  store i64 %calltmp5, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_82, label %merge_83

merge_83:                                         ; preds = %entry
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %addtmp = add i64 0, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  ret i64 %addtmp

then_82:                                          ; preds = %entry
  ret i64 0
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A93", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %calltmp = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load")
  store i64 %calltmp, ptr %"%\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %calltmp6 = call i64 @"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load5")
  store i64 %calltmp6, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_84, label %merge_85

merge_85:                                         ; preds = %entry
  ret i64 0

then_84:                                          ; preds = %entry
  ret i64 -1
}

define i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\B9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  call void @sad_ll_mem_read16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load")
  ret i64 0
}

define i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 12
  call void @sad_ll_mem_read64(i64 %addtmp)
  ret i64 0
}

define i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load", 0
  br i1 %cmplttmp, label %then_86, label %merge_87

merge_87:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load3", 0
  br i1 %cmpgetmp, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 32
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %shltmp = shl i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load4", 2
  %addtmp5 = add i64 %addtmp, %shltmp
  call void @sad_ll_mem_read32(i64 %addtmp5)
  ret i64 0

then_86:                                          ; preds = %entry
  ret i64 0

then_88:                                          ; preds = %merge_87
  ret i64 0
}

define i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B11"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 80
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B12"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 84
  call void @sad_ll_mem_read32(i64 %addtmp)
  ret i64 0
}

define i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B7"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9") {
entry:
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 2
  call void @sad_ll_mem_read16(i64 %addtmp)
  ret i64 0
}

define void @"\D8\B9\D9\8A\D9\86_\D9\86\D9\88\D8\B9_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", i64 %"\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA") {
entry:
  %"\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA", ptr %"\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA2", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", i64 %"%\D9\86\D9\88\D8\B9_\D8\A3\D8\B0\D9\88\D9\86\D8\A7\D8\AA.load")
  ret void
}

define void @"\D8\B9\D9\8A\D9\86_\D8\AD\D8\AC\D9\85_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF") {
entry:
  %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF2" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF", ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 12
  %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF.load" = load i64, ptr %"\D8\A7\D9\84\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF2", align 8
  call void @sad_ll_mem_write64(i64 %addtmp, i64 %"%\D8\A7\D9\84\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AC\D8\AF\D9\8A\D8\AF.load")
  ret void
}

define void @"\D8\B9\D9\8A\D9\86_\D9\83\D8\AA\D9\84\D8\A9_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A93" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A93", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load", 0
  br i1 %cmplttmp, label %then_90, label %merge_91

merge_91:                                         ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load4", 0
  br i1 %cmpgetmp, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 32
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B12", align 8
  %shltmp = shl i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1.load5", 2
  %addtmp6 = add i64 %addtmp, %shltmp
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A93", align 8
  call void @sad_ll_mem_write32(i64 %addtmp6, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load")
  ret void

then_90:                                          ; preds = %entry
  ret void

then_92:                                          ; preds = %merge_91
  ret void
}

define void @"\D8\B9\D9\8A\D9\86_\D8\B1\D9\88\D8\A7\D8\A8\D8\B7_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B7") {
entry:
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B72" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B7", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B72", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A91", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 2
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B7.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B72", align 8
  call void @sad_ll_mem_write16(i64 %addtmp, i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\B1\D9\88\D8\A7\D8\A8\D8\B7.load")
  ret void
}

define i64 @"\D8\AA\D8\B1\D8\AC\D9\85_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", i64 %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\88\D8\B3\D8\B7\D9\89" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_2" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_1" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1" = alloca i64, align 8
  %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93" = alloca i64, align 8
  store i64 %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load", 0
  br i1 %cmplttmp, label %then_94, label %merge_95

merge_101:                                        ; preds = %merge_99
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load14" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1.load15" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1", align 8
  %subtmp = sub i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load14", %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1.load15"
  store i64 %subtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %shltmp = shl i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 2
  %addtmp16 = add i64 0, %shltmp
  call void @sad_ll_mem_read32(i64 %addtmp16)
  ret i64 0

merge_103:                                        ; preds = %merge_97
  ret i64 0

merge_105:                                        ; preds = %then_102
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load20" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A.load21" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A", align 8
  %subtmp22 = sub i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load20", %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A.load21"
  store i64 %subtmp22, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A", align 8
  %divtmp = sdiv i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A.load", 0
  store i64 %divtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A.load23" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A", align 8
  %modtmp = srem i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D9\86\D8\B3\D8\A8\D9\8A.load23", 0
  store i64 %modtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_2", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91", align 8
  %cmpnetmp24 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load", 0
  br i1 %cmpnetmp24, label %then_106, label %merge_107

merge_107:                                        ; preds = %merge_105
  %"%\D9\81\D9\87\D8\B1\D8\B3_1.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_1", align 8
  %shltmp25 = shl i64 %"%\D9\81\D9\87\D8\B1\D8\B3_1.load", 2
  %addtmp26 = add i64 0, %shltmp25
  call void @sad_ll_mem_read32(i64 %addtmp26)
  store i64 0, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\88\D8\B3\D8\B7\D9\89", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\88\D8\B3\D8\B7\D9\89.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\88\D8\B3\D8\B7\D9\89", align 8
  %cmpeqtmp27 = icmp eq i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\88\D8\B3\D8\B7\D9\89.load", 0
  br i1 %cmpeqtmp27, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92", align 8
  %cmpnetmp28 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A92.load", 0
  br i1 %cmpnetmp28, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%\D9\81\D9\87\D8\B1\D8\B3_2.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_2", align 8
  %shltmp29 = shl i64 %"%\D9\81\D9\87\D8\B1\D8\B3_2.load", 2
  %addtmp30 = add i64 0, %shltmp29
  call void @sad_ll_mem_read32(i64 %addtmp30)
  ret i64 0

merge_95:                                         ; preds = %entry
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1", align 8
  %addtmp = add i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1.load", 0
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load5" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93", align 8
  %cmplttmp6 = icmp slt i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load5", %addtmp
  br i1 %cmplttmp6, label %then_96, label %merge_97

merge_97:                                         ; preds = %merge_95
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1.load9" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1", align 8
  %addtmp10 = add i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1.load9", 0
  store i64 %addtmp10, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A", align 8
  %addtmp11 = add i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A.load", 0
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load12" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93", align 8
  %cmplttmp13 = icmp slt i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load12", %addtmp11
  br i1 %cmplttmp13, label %then_102, label %merge_103

merge_99:                                         ; preds = %then_96
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  ret i64 0

then_102:                                         ; preds = %merge_97
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load17" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %calltmp18 = call i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B12"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load17")
  store i64 %calltmp18, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A\D8\A9", align 8
  %cmpeqtmp19 = icmp eq i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AB\D9\86\D8\A7\D8\A6\D9\8A\D8\A9.load", 0
  br i1 %cmpeqtmp19, label %then_104, label %merge_105

then_104:                                         ; preds = %then_102
  ret i64 0

then_106:                                         ; preds = %merge_105
  ret i64 0

then_108:                                         ; preds = %merge_107
  ret i64 0

then_110:                                         ; preds = %merge_109
  ret i64 0

then_94:                                          ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load4" = load i64, ptr %"\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A93", align 8
  %calltmp = call i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load4")
  ret i64 %calltmp

then_96:                                          ; preds = %merge_95
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load7" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A92", align 8
  %calltmp8 = call i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B11"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load7")
  store i64 %calltmp8, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\BA\D9\8A\D8\B1_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_98, label %merge_99

then_98:                                          ; preds = %then_96
  ret i64 0
}

define i64 @"\D8\A7\D8\A8\D8\AD\D8\AB_\D9\81\D9\8A_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85") {
entry:
  %"%byte2" = alloca i64, align 8
  %"%byte1" = alloca i64, align 8
  %"%pair2" = alloca i64, align 8
  %"%pair1" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_2" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_1" = alloca i64, align 8
  %"%\D8\AD" = alloca i64, align 8
  %"%\D9\85\D8\AA\D8\B7\D8\A7\D8\A8\D9\82" = alloca i64, align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AF" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84" = alloca i64, align 8
  %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\854" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\854", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\853" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\853", align 8
  %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load")
  store i64 %calltmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_112, label %merge_113

merge_113:                                        ; preds = %entry
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 12, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 12, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 12))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep5 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data6 = load ptr, ptr %arr.data.gep5, align 8
  %arr.elem = getelementptr i64, ptr %arr.data6, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep7 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data8 = load ptr, ptr %arr.data.gep7, align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 1
  store i64 0, ptr %arr.elem9, align 8
  %arr.data.gep10 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data11 = load ptr, ptr %arr.data.gep10, align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 2
  store i64 0, ptr %arr.elem12, align 8
  %arr.data.gep13 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data14 = load ptr, ptr %arr.data.gep13, align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 3
  store i64 0, ptr %arr.elem15, align 8
  %arr.data.gep16 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data17 = load ptr, ptr %arr.data.gep16, align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 4
  store i64 0, ptr %arr.elem18, align 8
  %arr.data.gep19 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data20 = load ptr, ptr %arr.data.gep19, align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 5
  store i64 0, ptr %arr.elem21, align 8
  %arr.data.gep22 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data23 = load ptr, ptr %arr.data.gep22, align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 6
  store i64 0, ptr %arr.elem24, align 8
  %arr.data.gep25 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data26 = load ptr, ptr %arr.data.gep25, align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 7
  store i64 0, ptr %arr.elem27, align 8
  %arr.data.gep28 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data29 = load ptr, ptr %arr.data.gep28, align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 8
  store i64 0, ptr %arr.elem30, align 8
  %arr.data.gep31 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data32 = load ptr, ptr %arr.data.gep31, align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 9
  store i64 0, ptr %arr.elem33, align 8
  %arr.data.gep34 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data35 = load ptr, ptr %arr.data.gep34, align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 10
  store i64 0, ptr %arr.elem36, align 8
  %arr.data.gep37 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data38 = load ptr, ptr %arr.data.gep37, align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 11
  store i64 0, ptr %arr.elem39, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load40" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %calltmp41 = call i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load40")
  store i64 %calltmp41, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_114

merge_118:                                        ; preds = %then_117, %while_exit_116
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_119

merge_123:                                        ; preds = %while_body_120
  %"%\D9\83.load56" = load i64, ptr %"%\D9\83", align 8
  %arr.ptr.int57 = load i64, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA", align 8
  %arr.ptr58 = inttoptr i64 %arr.ptr.int57 to ptr
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr.ptr58, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 %"%\D9\83.load56"
  %arr.get = load i64, ptr %arr.elem61, align 8
  store i64 %arr.get, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %cmpeqtmp62 = icmp eq i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 0
  br i1 %cmpeqtmp62, label %then_125, label %merge_126

merge_126:                                        ; preds = %merge_123
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp65 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp65, label %then_128, label %merge_129

merge_129:                                        ; preds = %merge_126
  store i64 0, ptr %"%\D9\85\D8\AF", align 8
  br label %while_cond_131

merge_135:                                        ; preds = %while_body_132
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load76" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %addtmp77 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load76", 4
  call void @sad_ll_mem_read16(i64 %addtmp77)
  store i64 0, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\854", align 8
  %cmpeqtmp78 = icmp eq i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load", %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load"
  br i1 %cmpeqtmp78, label %then_137, label %merge_138

merge_138:                                        ; preds = %merge_147, %merge_135
  %"%\D9\85\D8\AF.load79" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %addtmp80 = add i64 %"%\D9\85\D8\AF.load79", 1
  store i64 %addtmp80, ptr %"%\D9\85\D8\AF", align 8
  br label %while_cond_131

merge_143:                                        ; preds = %then_142, %while_body_140
  %"%byte1.load" = load i64, ptr %"%byte1", align 8
  %"%byte2.load" = load i64, ptr %"%byte2", align 8
  %cmpnetmp96 = icmp ne i64 %"%byte1.load", %"%byte2.load"
  br i1 %cmpnetmp96, label %then_144, label %merge_145

merge_145:                                        ; preds = %then_144, %merge_143
  %"%\D8\AD.load98" = load i64, ptr %"%\D8\AD", align 8
  %addtmp99 = add i64 %"%\D8\AD.load98", 1
  store i64 %addtmp99, ptr %"%\D8\AD", align 8
  br label %while_cond_139

merge_147:                                        ; preds = %while_exit_141
  br label %merge_138

then_112:                                         ; preds = %entry
  ret i64 -1

then_117:                                         ; preds = %while_exit_116
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %addtmp51 = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load", 1
  store i64 %addtmp51, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  br label %merge_118

then_122:                                         ; preds = %while_body_120
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load55" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  store i64 %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load55", ptr %"%\D9\83", align 8
  br label %while_cond_119

then_125:                                         ; preds = %merge_123
  %"%\D9\83.load63" = load i64, ptr %"%\D9\83", align 8
  %addtmp64 = add i64 %"%\D9\83.load63", 1
  store i64 %addtmp64, ptr %"%\D9\83", align 8
  br label %while_cond_119

then_128:                                         ; preds = %merge_126
  %"%\D9\83.load66" = load i64, ptr %"%\D9\83", align 8
  %addtmp67 = add i64 %"%\D9\83.load66", 1
  store i64 %addtmp67, ptr %"%\D9\83", align 8
  br label %while_cond_119

then_134:                                         ; preds = %while_body_132
  %"%\D9\85\D8\AF.load74" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %addtmp75 = add i64 %"%\D9\85\D8\AF.load74", 1
  store i64 %addtmp75, ptr %"%\D9\85\D8\AF", align 8
  br label %while_cond_131

then_137:                                         ; preds = %merge_135
  store i64 1, ptr %"%\D9\85\D8\AA\D8\B7\D8\A7\D8\A8\D9\82", align 8
  store i64 0, ptr %"%\D8\AD", align 8
  br label %while_cond_139

then_142:                                         ; preds = %while_body_140
  %"%pair1.load93" = load i64, ptr %"%pair1", align 8
  %shrtmp = lshr i64 %"%pair1.load93", 8
  store i64 %shrtmp, ptr %"%byte1", align 8
  %"%pair2.load94" = load i64, ptr %"%pair2", align 8
  %shrtmp95 = lshr i64 %"%pair2.load94", 8
  store i64 %shrtmp95, ptr %"%byte2", align 8
  br label %merge_143

then_144:                                         ; preds = %merge_143
  store i64 0, ptr %"%\D9\85\D8\AA\D8\B7\D8\A7\D8\A8\D9\82", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load97" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\854", align 8
  store i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load97", ptr %"%\D8\AD", align 8
  br label %merge_145

then_146:                                         ; preds = %while_exit_141
  %loadtmp = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF", align 8
  ret i64 %loadtmp

while_body_115:                                   ; preds = %while_cond_114
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load42" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D9\85.load43" = load i64, ptr %"%\D9\85", align 8
  %calltmp44 = call i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load42", i64 %"%\D9\85.load43")
  %"%\D9\85.load45" = load i64, ptr %"%\D9\85", align 8
  %arr.ptr.int = load i64, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep46 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data47 = load ptr, ptr %arr.data.gep46, align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 %"%\D9\85.load45"
  store i64 %calltmp44, ptr %arr.elem48, align 8
  %"%\D9\85.load49" = load i64, ptr %"%\D9\85", align 8
  %addtmp = add i64 %"%\D9\85.load49", 1
  store i64 %addtmp, ptr %"%\D9\85", align 8
  br label %while_cond_114

while_body_120:                                   ; preds = %while_cond_119
  %"%\D9\83.load54" = load i64, ptr %"%\D9\83", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\83.load54", 0
  br i1 %cmpgetmp, label %then_122, label %merge_123

while_body_132:                                   ; preds = %while_cond_131
  %"%\D9\85\D8\AF.load69" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %multmp = mul i64 %"%\D9\85\D8\AF.load69", 0
  %addtmp70 = add i64 0, %multmp
  store i64 %addtmp70, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  call void @sad_ll_mem_read32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load")
  store i64 0, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF", align 8
  %cmpeqtmp71 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF.load", 0
  br i1 %cmpeqtmp71, label %then_134, label %merge_135

while_body_140:                                   ; preds = %while_cond_139
  %"%\D8\AD.load83" = load i64, ptr %"%\D8\AD", align 8
  %andtmp = and i64 %"%\D8\AD.load83", 1
  %"%\D8\AD.load84" = load i64, ptr %"%\D8\AD", align 8
  %subtmp = sub i64 %"%\D8\AD.load84", %andtmp
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\853", align 8
  %addtmp85 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load", %subtmp
  store i64 %addtmp85, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_1", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load86" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %addtmp87 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load86", 0
  %addtmp88 = add i64 %addtmp87, %subtmp
  store i64 %addtmp88, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_2", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_1.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_1", align 8
  call void @sad_ll_mem_read16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_1.load")
  store i64 0, ptr %"%pair1", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_2.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_2", align 8
  call void @sad_ll_mem_read16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_2.load")
  store i64 0, ptr %"%pair2", align 8
  %"%pair1.load" = load i64, ptr %"%pair1", align 8
  %andtmp89 = and i64 %"%pair1.load", 255
  store i64 %andtmp89, ptr %"%byte1", align 8
  %"%pair2.load" = load i64, ptr %"%pair2", align 8
  %andtmp90 = and i64 %"%pair2.load", 255
  store i64 %andtmp90, ptr %"%byte2", align 8
  %cmpeqtmp91 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp91, label %then_142, label %merge_143

while_cond_114:                                   ; preds = %while_body_115, %merge_113
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", 0
  br i1 %cmplttmp, label %while_body_115, label %while_exit_116

while_cond_119:                                   ; preds = %while_exit_133, %then_128, %then_125, %then_122, %merge_118
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load52" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %cmplttmp53 = icmp slt i64 %"%\D9\83.load", %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load52"
  br i1 %cmplttmp53, label %while_body_120, label %while_exit_121

while_cond_131:                                   ; preds = %merge_138, %then_134, %merge_129
  %"%\D9\85\D8\AF.load" = load i64, ptr %"%\D9\85\D8\AF", align 8
  %cmplttmp68 = icmp slt i64 %"%\D9\85\D8\AF.load", 0
  br i1 %cmplttmp68, label %while_body_132, label %while_exit_133

while_cond_139:                                   ; preds = %merge_145, %then_137
  %"%\D8\AD.load" = load i64, ptr %"%\D8\AD", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load81" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\854", align 8
  %cmplttmp82 = icmp slt i64 %"%\D8\AD.load", %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load81"
  br i1 %cmplttmp82, label %while_body_140, label %while_exit_141

while_exit_116:                                   ; preds = %while_cond_114
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %divtmp = sdiv i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load", 0
  store i64 %divtmp, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load50" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %modtmp = srem i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load50", 0
  %cmpnetmp = icmp ne i64 %modtmp, 0
  br i1 %cmpnetmp, label %then_117, label %merge_118

while_exit_121:                                   ; preds = %while_cond_119
  ret i64 -1

while_exit_133:                                   ; preds = %while_cond_131
  %"%\D9\83.load72" = load i64, ptr %"%\D9\83", align 8
  %addtmp73 = add i64 %"%\D9\83.load72", 1
  store i64 %addtmp73, ptr %"%\D9\83", align 8
  br label %while_cond_119

while_exit_141:                                   ; preds = %while_cond_139
  %"%\D9\85\D8\AA\D8\B7\D8\A7\D8\A8\D9\82.load" = load i64, ptr %"%\D9\85\D8\AA\D8\B7\D8\A7\D8\A8\D9\82", align 8
  %cmpeqtmp92 = icmp eq i64 %"%\D9\85\D8\AA\D8\B7\D8\A7\D8\A8\D9\82.load", 1
  br i1 %cmpeqtmp92, label %then_146, label %merge_147
}

define i64 @"\D8\A3\D8\B6\D9\81_\D9\85\D8\AF\D8\AE\D9\84\D8\A9_\D8\AF\D9\84\D9\8A\D9\84"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\84\D9\81", i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85") {
entry:
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\856" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\856", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\855" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\855", align 8
  %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81", ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814", align 8
  %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\84\D9\813" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\84\D9\81", ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\84\D9\813", align 8
  %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load")
  store i64 %calltmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_148, label %merge_149

else_155:                                         ; preds = %while_exit_152
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load54" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load55" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %calltmp56 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load54", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load55")
  store i64 %calltmp56, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load57" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %addtmp58 = add i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load57", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load59" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\8A\D9\86_\D8\AD\D8\AC\D9\85_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load59", i64 %addtmp58)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load60" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load61" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load62" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %calltmp63 = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load60", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load61", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load62")
  br label %merge_154

merge_149:                                        ; preds = %entry
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load7" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %calltmp8 = call i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AD\D8\AC\D9\85"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load7")
  store i64 %calltmp8, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 12, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 12, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 12))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem = getelementptr i64, ptr %arr.data10, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep11 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data12 = load ptr, ptr %arr.data.gep11, align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 1
  store i64 0, ptr %arr.elem13, align 8
  %arr.data.gep14 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data15 = load ptr, ptr %arr.data.gep14, align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 2
  store i64 0, ptr %arr.elem16, align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 3
  store i64 0, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 4
  store i64 0, ptr %arr.elem22, align 8
  %arr.data.gep23 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data24 = load ptr, ptr %arr.data.gep23, align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 5
  store i64 0, ptr %arr.elem25, align 8
  %arr.data.gep26 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data27 = load ptr, ptr %arr.data.gep26, align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 6
  store i64 0, ptr %arr.elem28, align 8
  %arr.data.gep29 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data30 = load ptr, ptr %arr.data.gep29, align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 7
  store i64 0, ptr %arr.elem31, align 8
  %arr.data.gep32 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data33 = load ptr, ptr %arr.data.gep32, align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 8
  store i64 0, ptr %arr.elem34, align 8
  %arr.data.gep35 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data36 = load ptr, ptr %arr.data.gep35, align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 9
  store i64 0, ptr %arr.elem37, align 8
  %arr.data.gep38 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data39 = load ptr, ptr %arr.data.gep38, align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 10
  store i64 0, ptr %arr.elem40, align 8
  %arr.data.gep41 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data42 = load ptr, ptr %arr.data.gep41, align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 11
  store i64 0, ptr %arr.elem43, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_150

merge_154:                                        ; preds = %merge_159, %else_155
  %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load64" = load i64, ptr %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", align 8
  %addtmp65 = add i64 0, %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load64"
  store i64 %addtmp65, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\84\D9\813", align 8
  call void @sad_ll_mem_write32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load")
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load66" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %addtmp67 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load66", 4
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\856", align 8
  call void @sad_ll_mem_write16(i64 %addtmp67, i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load")
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load68" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %addtmp69 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load68", 6
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\814", align 8
  call void @sad_ll_mem_write16(i64 %addtmp69, i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\84\D9\81.load")
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load70" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\856", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load70", 0
  br i1 %cmpgttmp, label %then_160, label %merge_161

merge_157:                                        ; preds = %then_153
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load71" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %calltmp72 = call i64 @"\D8\AE\D8\B5\D8\B5_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load71")
  store i64 %calltmp72, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %cmplttmp73 = icmp slt i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp73, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_157
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load74" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load75" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  %arr.ptr.int76 = load i64, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA", align 8
  %arr.ptr77 = inttoptr i64 %arr.ptr.int76 to ptr
  %arr.data.gep78 = getelementptr inbounds %SadArray, ptr %arr.ptr77, i32 0, i32 2
  %arr.data79 = load ptr, ptr %arr.data.gep78, align 8
  %arr.elem80 = getelementptr i64, ptr %arr.data79, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load74"
  store i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load75", ptr %arr.elem80, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load81" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load82" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %calltmp83 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load81", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load82")
  store i64 %calltmp83, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load84" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load85" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load86" = load i64, ptr %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\8A\D9\86_\D9\83\D8\AA\D9\84\D8\A9_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load84", i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load85", i64 %"%\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load86")
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load87" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %addtmp88 = add i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load87", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load89" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  call void @"\D8\B9\D9\8A\D9\86_\D8\AD\D8\AC\D9\85_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load89", i64 %addtmp88)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load90" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load91" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\842", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load92" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %calltmp93 = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\B9\D9\82\D8\AF\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load90", i64 %"%\D8\B1\D9\82\D9\85_\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load91", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load92")
  br label %merge_154

merge_161:                                        ; preds = %then_160, %merge_154
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load94" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %addtmp95 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load94", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\855", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load96" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\856", align 8
  call void @sad_ll_memcpy(i64 %addtmp95, i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load", i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A7\D8\B3\D9\85.load96")
  ret i64 0

then_148:                                         ; preds = %entry
  ret i64 -1

then_153:                                         ; preds = %while_exit_152
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9.load", 0
  br i1 %cmpgetmp, label %then_156, label %merge_157

then_156:                                         ; preds = %then_153
  ret i64 -1

then_158:                                         ; preds = %merge_157
  ret i64 -1

then_160:                                         ; preds = %merge_154
  br label %merge_161

while_body_151:                                   ; preds = %while_cond_150
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load44" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %"%\D9\85.load45" = load i64, ptr %"%\D9\85", align 8
  %calltmp46 = call i64 @"\D8\B9\D9\82\D8\AF\D8\A9_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9.load44", i64 %"%\D9\85.load45")
  %"%\D9\85.load47" = load i64, ptr %"%\D9\85", align 8
  %arr.ptr.int = load i64, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep48 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data49 = load ptr, ptr %arr.data.gep48, align 8
  %arr.elem50 = getelementptr i64, ptr %arr.data49, i64 %"%\D9\85.load47"
  store i64 %calltmp46, ptr %arr.elem50, align 8
  %"%\D9\85.load51" = load i64, ptr %"%\D9\85", align 8
  %addtmp = add i64 %"%\D9\85.load51", 1
  store i64 %addtmp, ptr %"%\D9\85", align 8
  br label %while_cond_150

while_cond_150:                                   ; preds = %while_body_151, %merge_149
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", 0
  br i1 %cmplttmp, label %while_body_151, label %while_exit_152

while_exit_152:                                   ; preds = %while_cond_150
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %divtmp = sdiv i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load", 0
  store i64 %divtmp, ptr %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D8\B7\D9\82\D9\8A\D8\A9", align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load52" = load i64, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  %modtmp = srem i64 %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84.load52", 0
  store i64 %modtmp, ptr %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", align 8
  %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", align 8
  %cmpeqtmp53 = icmp eq i64 %"%\D8\A7\D9\84\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D9\81\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load", 0
  br i1 %cmpeqtmp53, label %then_153, label %else_155
}

define i64 @"\D8\AA\D9\86\D8\B3\D9\8A\D9\82_BeeFS"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\A7\D9\85") {
entry:
  %"%\D8\AC\D8\AA" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\AC\D8\B0\D8\B1" = alloca i64, align 8
  %"%\D8\A8" = alloca i64, align 8
  %"%\D9\85\D8\AC" = alloca i64, align 8
  %"%mt" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8" = alloca i64, align 8
  %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\A7\D9\853" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\A7\D9\85", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\A7\D9\853", align 8
  %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\A7\D9\85.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\A7\D9\853", align 8
  %divtmp = sdiv i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D8\AE\D8\A7\D9\85.load", 0
  store i64 %divtmp, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84", align 8
  store i64 1024, ptr %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84", align 8
  %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %divtmp4 = sdiv i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load", %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load"
  store i64 %divtmp4, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load", 0
  br i1 %cmpeqtmp, label %then_162, label %merge_163

merge_163:                                        ; preds = %then_162, %entry
  store i64 256, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load5" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %multmp = mul i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load5", %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load"
  store i64 %multmp, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  store i64 0, ptr %"%mt", align 8
  %"%mt.load" = load i64, ptr %"%mt", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%mt.load"
  store i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", ptr %arr.elem, align 8
  %"%mt.load6" = load i64, ptr %"%mt", align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A92", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%mt.load6"
  store i64 %"%\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", ptr %arr.elem8, align 8
  %"%mt.load9" = load i64, ptr %"%mt", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%mt.load9"
  store i64 1, ptr %arr.elem11, align 8
  %"%mt.load12" = load i64, ptr %"%mt", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%mt.load12"
  store i64 0, ptr %arr.elem14, align 8
  %"%mt.load15" = load i64, ptr %"%mt", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%mt.load15"
  store i64 0, ptr %arr.elem17, align 8
  %"%mt.load18" = load i64, ptr %"%mt", align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load19" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84", align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 %"%mt.load18"
  store i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load19", ptr %arr.elem21, align 8
  %"%mt.load22" = load i64, ptr %"%mt", align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%mt.load22"
  store i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load", ptr %arr.elem24, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load25" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %multmp26 = mul i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load25", 10
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load27" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84", align 8
  %subtmp = sub i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load27", %multmp26
  %"%mt.load28" = load i64, ptr %"%mt", align 8
  %arr.data29 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 %"%mt.load28"
  store i64 %subtmp, ptr %arr.elem30, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load31" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %subtmp32 = sub i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load31", 3
  %"%mt.load33" = load i64, ptr %"%mt", align 8
  %arr.data34 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 %"%mt.load33"
  store i64 %subtmp32, ptr %arr.elem35, align 8
  %"%mt.load36" = load i64, ptr %"%mt", align 8
  %arr.data37 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 %"%mt.load36"
  store i64 0, ptr %arr.elem38, align 8
  %"%mt.load39" = load i64, ptr %"%mt", align 8
  %arr.data40 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 %"%mt.load39"
  store i64 0, ptr %arr.elem41, align 8
  %"%mt.load42" = load i64, ptr %"%mt", align 8
  %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load43" = load i64, ptr %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %arr.data44 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem45 = getelementptr i64, ptr %arr.data44, i64 %"%mt.load42"
  store i64 %"%\D9\83\D8\AA\D9\84_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load43", ptr %arr.elem45, align 8
  %"%mt.load46" = load i64, ptr %"%mt", align 8
  %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load47" = load i64, ptr %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %arr.data48 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 %"%mt.load46"
  store i64 %"%\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9.load47", ptr %arr.elem49, align 8
  %"%mt.load50" = load i64, ptr %"%mt", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load51" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %arr.data52 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 %"%mt.load50"
  store i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load51", ptr %arr.elem53, align 8
  %"%mt.load54" = load i64, ptr %"%mt", align 8
  %arr.data55 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 %"%mt.load54"
  store i64 1, ptr %arr.elem56, align 8
  %"%mt.load57" = load i64, ptr %"%mt", align 8
  %calltmp = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\81\D8\A7\D8\A6\D9\82\D8\A9"(i64 %"%mt.load57")
  store i64 0, ptr %"%\D9\85\D8\AC", align 8
  br label %while_cond_164

merge_171:                                        ; preds = %then_170, %while_exit_169
  %"%mt.load76" = load i64, ptr %"%mt", align 8
  %"%\D9\85\D8\AC.load77" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %calltmp78 = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D8\B9\D9\82\D8\AF"(i64 %"%mt.load76", i64 %"%\D9\85\D8\AC.load77")
  store i64 0, ptr %"%\D8\AC\D8\AA", align 8
  br label %while_cond_172

then_162:                                         ; preds = %entry
  store i64 1, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  br label %merge_163

then_170:                                         ; preds = %while_exit_169
  br label %merge_171

while_body_165:                                   ; preds = %while_cond_164
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_167

while_body_168:                                   ; preds = %while_cond_167
  %"%\D8\A8.load70" = load i64, ptr %"%\D8\A8", align 8
  %addtmp = add i64 %"%\D8\A8.load70", 1
  store i64 %addtmp, ptr %"%\D8\A8", align 8
  br label %while_cond_167

while_body_173:                                   ; preds = %while_cond_172
  %"%mt.load80" = load i64, ptr %"%mt", align 8
  %"%\D9\85\D8\AC.load81" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %calltmp82 = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AC\D8\AF\D9\88\D9\84_\D8\A7\D9\84\D8\B9\D9\82\D8\AF"(i64 %"%mt.load80", i64 %"%\D9\85\D8\AC.load81")
  %"%\D8\AC\D8\AA.load83" = load i64, ptr %"%\D8\AC\D8\AA", align 8
  %addtmp84 = add i64 %"%\D8\AC\D8\AA.load83", 1
  store i64 %addtmp84, ptr %"%\D8\AC\D8\AA", align 8
  br label %while_cond_172

while_cond_164:                                   ; preds = %while_exit_174, %merge_163
  %"%\D9\85\D8\AC.load" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load58" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AC.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load58"
  br i1 %cmplttmp, label %while_body_165, label %while_exit_166

while_cond_167:                                   ; preds = %while_body_168, %while_body_165
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %cmplttmp69 = icmp slt i64 %"%\D8\A8.load", 10
  br i1 %cmplttmp69, label %while_body_168, label %while_exit_169

while_cond_172:                                   ; preds = %while_body_173, %merge_171
  %"%\D8\AC\D8\AA.load" = load i64, ptr %"%\D8\AC\D8\AA", align 8
  %cmplttmp79 = icmp slt i64 %"%\D8\AC\D8\AA.load", 6
  br i1 %cmplttmp79, label %while_body_173, label %while_exit_174

while_exit_166:                                   ; preds = %while_cond_164
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC\D8\B0\D8\B1", align 8
  %"%mt.load59" = load i64, ptr %"%mt", align 8
  %arr.data60 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 %"%mt.load59"
  store i64 0, ptr %arr.elem61, align 8
  %"%mt.load62" = load i64, ptr %"%mt", align 8
  %calltmp63 = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\81\D8\A7\D8\A6\D9\82\D8\A9"(i64 %"%mt.load62")
  call void @"blk_\D9\81\D8\B1\D9\91\D8\BA"()
  %"%mt.load64" = load i64, ptr %"%mt", align 8
  %arr.data65 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem66 = getelementptr i64, ptr %arr.data65, i64 %"%mt.load64"
  store i64 0, ptr %arr.elem66, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load67" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84", align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load67")
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load68" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8", align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D8\B9\D9\82\D8\AF_\D8\A7\D9\84\D9\85\D8\AD\D8\B3\D9\88\D8\A8.load68")
  ret i64 0

while_exit_169:                                   ; preds = %while_cond_167
  %"%mt.load71" = load i64, ptr %"%mt", align 8
  %"%\D9\85\D8\AC.load72" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %calltmp73 = call i64 @"\D9\83\D8\AA\D9\84\D8\A9_\D8\AE\D8\B1\D9\8A\D8\B7\D8\A9_\D8\A7\D9\84\D9\83\D8\AA\D9\84"(i64 %"%mt.load71", i64 %"%\D9\85\D8\AC.load72")
  %"%\D9\85\D8\AC.load74" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %cmpeqtmp75 = icmp eq i64 %"%\D9\85\D8\AC.load74", 0
  br i1 %cmpeqtmp75, label %then_170, label %merge_171

while_exit_174:                                   ; preds = %while_cond_172
  %"%\D9\85\D8\AC.load85" = load i64, ptr %"%\D9\85\D8\AC", align 8
  %addtmp86 = add i64 %"%\D9\85\D8\AC.load85", 1
  store i64 %addtmp86, ptr %"%\D9\85\D8\AC", align 8
  br label %while_cond_164
}

define i64 @"\D8\AD\D9\85\D9\84_FS"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%mt" = alloca i64, align 8
  %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A92" = alloca i64, align 8
  store i64 %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  store i64 -1, ptr %"%mt", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_175

merge_179:                                        ; preds = %then_178, %while_body_176
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load6", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_175

merge_181:                                        ; preds = %while_exit_177
  %"%mt.load7" = load i64, ptr %"%mt", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%mt.load7"
  store i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", ptr %arr.elem9, align 8
  %"%mt.load10" = load i64, ptr %"%mt", align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A92", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%mt.load10"
  store i64 %"%\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", ptr %arr.elem12, align 8
  %"%mt.load13" = load i64, ptr %"%mt", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%mt.load13"
  store i64 1, ptr %arr.elem15, align 8
  %"%mt.load16" = load i64, ptr %"%mt", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\81\D8\A7\D8\A6\D9\82\D8\A9"(i64 %"%mt.load16")
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_182, label %merge_183

merge_183:                                        ; preds = %merge_181
  %"%mt.load20" = load i64, ptr %"%mt", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%mt.load20"
  %arr.get23 = load i64, ptr %arr.elem22, align 8
  %cmpnetmp24 = icmp ne i64 %arr.get23, 0
  br i1 %cmpnetmp24, label %then_184, label %merge_185

merge_185:                                        ; preds = %merge_183
  %"%mt.load28" = load i64, ptr %"%mt", align 8
  %arr.data29 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 %"%mt.load28"
  %arr.get31 = load i64, ptr %arr.elem30, align 8
  %cmpnetmp32 = icmp ne i64 %arr.get31, 0
  br i1 %cmpnetmp32, label %then_186, label %merge_187

merge_187:                                        ; preds = %merge_185
  %"%mt.load36" = load i64, ptr %"%mt", align 8
  %arr.data37 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 %"%mt.load36"
  store i64 1, ptr %arr.elem38, align 8
  %"%mt.load39" = load i64, ptr %"%mt", align 8
  %calltmp40 = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\81\D8\A7\D8\A6\D9\82\D8\A9"(i64 %"%mt.load39")
  %"%mt.load41" = load i64, ptr %"%mt", align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%mt.load41")
  %"%mt.load42" = load i64, ptr %"%mt", align 8
  %arr.data43 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 %"%mt.load42"
  %arr.get45 = load i64, ptr %arr.elem44, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get45)
  %"%mt.load46" = load i64, ptr %"%mt", align 8
  %arr.data47 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 %"%mt.load46"
  %arr.get49 = load i64, ptr %arr.elem48, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get49)
  %loadtmp = load i64, ptr %"%mt", align 8
  ret i64 %loadtmp

then_178:                                         ; preds = %while_body_176
  %"%\D9\81.load5" = load i64, ptr %"%\D9\81", align 8
  store i64 %"%\D9\81.load5", ptr %"%mt", align 8
  br label %merge_179

then_180:                                         ; preds = %while_exit_177
  ret i64 -1

then_182:                                         ; preds = %merge_181
  %"%mt.load17" = load i64, ptr %"%mt", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%mt.load17"
  store i64 0, ptr %arr.elem19, align 8
  ret i64 -1

then_184:                                         ; preds = %merge_183
  %"%mt.load25" = load i64, ptr %"%mt", align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 %"%mt.load25"
  store i64 0, ptr %arr.elem27, align 8
  ret i64 -1

then_186:                                         ; preds = %merge_185
  %"%mt.load33" = load i64, ptr %"%mt", align 8
  %arr.data34 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 %"%mt.load33"
  store i64 0, ptr %arr.elem35, align 8
  ret i64 -1

while_body_176:                                   ; preds = %while_cond_175
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_178, label %merge_179

while_cond_175:                                   ; preds = %merge_179, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_176, label %while_exit_177

while_exit_177:                                   ; preds = %while_cond_175
  %"%mt.load" = load i64, ptr %"%mt", align 8
  %cmplttmp4 = icmp slt i64 %"%mt.load", 0
  br i1 %cmplttmp4, label %then_180, label %merge_181
}

define i64 @"\D8\A7\D9\81\D8\B5\D9\84_FS"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load", 0
  br i1 %cmplttmp, label %then_188, label %merge_189

merge_189:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2", 0
  br i1 %cmpgetmp, label %then_190, label %merge_191

merge_191:                                        ; preds = %merge_189
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_192, label %merge_193

merge_193:                                        ; preds = %merge_191
  call void @"blk_\D9\81\D8\B1\D9\91\D8\BA"()
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load4"
  store i64 0, ptr %arr.elem6, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %calltmp = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9_\D8\A7\D9\84\D9\81\D8\A7\D8\A6\D9\82\D8\A9"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load7")
  call void @"blk_\D9\81\D8\B1\D9\91\D8\BA"()
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load8"
  store i64 0, ptr %arr.elem10, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load11" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load11")
  ret i64 0

then_188:                                         ; preds = %entry
  ret i64 -1

then_190:                                         ; preds = %merge_189
  ret i64 -1

then_192:                                         ; preds = %merge_191
  ret i64 -1
}

define void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D9\85\D8\B9\D9\84\D9\88\D9\85\D8\A7\D8\AA_FS"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_194, label %merge_195

merge_195:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get5)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get9)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load10" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get13)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load14"
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get17)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load18" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load18"
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get21)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load22" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\841", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84.load22"
  %arr.get25 = load i64, ptr %arr.elem24, align 8
  call void @"fs_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get25)
  ret void

then_194:                                         ; preds = %entry
  ret void
}

define void @__sad_main() {
entry:
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B7\D9\88\D9\84_\D8\A7\D8\B3\D9\85" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\A7\D8\B3\D9\85_\D9\85\D9\84\D9\81" = alloca i64, align 8
  %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9" = alloca i64, align 8
  %"%BG_INODE_TABLE" = alloca i64, align 8
  %"%BG_INODE_BMP" = alloca i64, align 8
  %"%BG_BLOCK_BMP" = alloca i64, align 8
  %"%BG_DESCRIPTOR" = alloca i64, align 8
  %"%BG_SUPERBLOCK" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AE\D8\B2\D9\862" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AE\D8\B2\D9\86_\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%sb_\D8\AD\D8\A7\D9\84\D8\A9_\D9\86\D8\B8\D8\A7\D9\85" = alloca i64, align 8
  %"%sb_\D8\B9\D8\AF\D8\AF_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA" = alloca i64, align 8
  %"%sb_\D8\B9\D9\82\D8\AF_\D9\84\D9\83\D9\84_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%sb_\D9\83\D8\AA\D9\84_\D9\84\D9\83\D9\84_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%sb_\D8\AD\D8\AC\D9\85_\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%sb_\D8\AD\D8\AC\D9\85_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%sb_\D8\B9\D9\82\D8\AF_\D8\AD\D8\B1\D8\A9" = alloca i64, align 8
  %"%sb_\D9\83\D8\AA\D9\84_\D8\AD\D8\B1\D8\A9" = alloca i64, align 8
  %"%sb_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\82\D8\AF" = alloca i64, align 8
  %"%sb_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\83\D8\AA\D9\84" = alloca i64, align 8
  %"%sb_\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1" = alloca i64, align 8
  %"%sb_\D8\B3\D8\AD\D8\B1\D9\8A" = alloca i64, align 8
  %"%mount_\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%mount_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%mount_\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85" = alloca i64, align 8
  %"%mount_\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D8\AF\D9\84\D9\8A\D9\84_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D9\85\D9\84\D9\81_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\A2\D8\AE\D8\B1" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D8\A2\D8\AE\D8\B1" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D8\A2\D8\AE\D8\B1" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D9\85\D8\A7\D9\84\D9\83" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D9\85\D8\A7\D9\84\D9\83" = alloca i64, align 8
  %"%\D8\A5\D8\B0\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\85\D8\A7\D9\84\D9\83" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A3\D9\86\D8\A8\D9\88\D8\A8" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\AC\D9\87\D8\A7\D8\B2" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\B1\D8\A7\D8\A8\D8\B7" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\AF\D9\84\D9\8A\D9\84" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\B9\D8\A7\D8\AF\D9\8A" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AA\D8\AD\D9\85\D9\8A\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AC\D8\B0\D8\B1" = alloca i64, align 8
  %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\84\D9\83\D9\84_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D9\85\D8\AF\D8\AE\D9\84\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF_\D9\84\D9\83\D9\84_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D9\84\D9\83\D9\84_\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9_FS" = alloca i64, align 8
  %"%BEEFS_VERSION" = alloca i64, align 8
  %"%BEEFS_MAGIC" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 48869, ptr %"%BEEFS_MAGIC", align 8
  store i64 1, ptr %"%BEEFS_VERSION", align 8
  store i64 512, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9_FS", align 8
  store i64 4096, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", align 8
  store i64 8, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D9\84\D9\83\D9\84_\D9\83\D8\AA\D9\84\D8\A9", align 8
  store i64 128, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D8\B9\D9\82\D8\AF\D8\A9", align 8
  store i64 32, ptr %"%\D8\B9\D9\82\D8\AF_\D9\84\D9\83\D9\84_\D9\83\D8\AA\D9\84\D8\A9", align 8
  store i64 64, ptr %"%\D8\AD\D8\AC\D9\85_\D9\85\D8\AF\D8\AE\D9\84\D8\A9_\D8\A7\D9\84\D8\AF\D9\84\D9\8A\D9\84", align 8
  store i64 64, ptr %"%\D9\85\D8\AF\D8\AE\D9\84\D8\A7\D8\AA_\D9\84\D9\83\D9\84_\D9\83\D8\AA\D9\84\D8\A9", align 8
  store i64 2, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D8\A7\D9\84\D8\AC\D8\B0\D8\B1", align 8
  store i64 4, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\AA\D8\AD\D9\85\D9\8A\D9\84\D8\A7\D8\AA", align 8
  store i64 1, ptr %"%\D9\86\D9\88\D8\B9_\D8\B9\D8\A7\D8\AF\D9\8A", align 8
  store i64 2, ptr %"%\D9\86\D9\88\D8\B9_\D8\AF\D9\84\D9\8A\D9\84", align 8
  store i64 3, ptr %"%\D9\86\D9\88\D8\B9_\D8\B1\D8\A7\D8\A8\D8\B7", align 8
  store i64 4, ptr %"%\D9\86\D9\88\D8\B9_\D8\AC\D9\87\D8\A7\D8\B2", align 8
  store i64 5, ptr %"%\D9\86\D9\88\D8\B9_\D8\A3\D9\86\D8\A8\D9\88\D8\A8", align 8
  store i64 256, ptr %"%\D8\A5\D8\B0\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\85\D8\A7\D9\84\D9\83", align 8
  store i64 128, ptr %"%\D8\A5\D8\B0\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D9\85\D8\A7\D9\84\D9\83", align 8
  store i64 64, ptr %"%\D8\A5\D8\B0\D9\86_\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D9\85\D8\A7\D9\84\D9\83", align 8
  store i64 32, ptr %"%\D8\A5\D8\B0\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  store i64 16, ptr %"%\D8\A5\D8\B0\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  store i64 8, ptr %"%\D8\A5\D8\B0\D9\86_\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  store i64 4, ptr %"%\D8\A5\D8\B0\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9_\D8\A2\D8\AE\D8\B1", align 8
  store i64 2, ptr %"%\D8\A5\D8\B0\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9_\D8\A2\D8\AE\D8\B1", align 8
  store i64 1, ptr %"%\D8\A5\D8\B0\D9\86_\D8\AA\D9\86\D9\81\D9\8A\D8\B0_\D8\A2\D8\AE\D8\B1", align 8
  store i64 420, ptr %"%\D8\A5\D8\B0\D9\86_\D9\85\D9\84\D9\81_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", align 8
  store i64 493, ptr %"%\D8\A5\D8\B0\D9\86_\D8\AF\D9\84\D9\8A\D9\84_\D8\A7\D9\81\D8\AA\D8\B1\D8\A7\D8\B6\D9\8A", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 4, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
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
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%mount_\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %arr_new12 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep13 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 0
  store i64 4, ptr %arr.len.gep13, align 8
  %arr.cap.gep14 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep14, align 8
  %arr.data15 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep16 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  store ptr %arr.data15, ptr %arr.data.gep16, align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 0
  store i64 0, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 1
  store i64 0, ptr %arr.elem22, align 8
  %arr.data.gep23 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  %arr.data24 = load ptr, ptr %arr.data.gep23, align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 2
  store i64 0, ptr %arr.elem25, align 8
  %arr.data.gep26 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  %arr.data27 = load ptr, ptr %arr.data.gep26, align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 3
  store i64 0, ptr %arr.elem28, align 8
  %obj.ptrtoint29 = ptrtoint ptr %arr_new12 to i64
  store i64 %obj.ptrtoint29, ptr %"%mount_\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %arr_new30 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep31 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 0
  store i64 4, ptr %arr.len.gep31, align 8
  %arr.cap.gep32 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep32, align 8
  %arr.data33 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep34 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  store ptr %arr.data33, ptr %arr.data.gep34, align 8
  %arr.data.gep35 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  %arr.data36 = load ptr, ptr %arr.data.gep35, align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 0
  store i64 0, ptr %arr.elem37, align 8
  %arr.data.gep38 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  %arr.data39 = load ptr, ptr %arr.data.gep38, align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 1
  store i64 0, ptr %arr.elem40, align 8
  %arr.data.gep41 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  %arr.data42 = load ptr, ptr %arr.data.gep41, align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 2
  store i64 0, ptr %arr.elem43, align 8
  %arr.data.gep44 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  %arr.data45 = load ptr, ptr %arr.data.gep44, align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 3
  store i64 0, ptr %arr.elem46, align 8
  %obj.ptrtoint47 = ptrtoint ptr %arr_new30 to i64
  store i64 %obj.ptrtoint47, ptr %"%mount_\D9\86\D8\B4\D8\B7", align 8
  %arr_new48 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep49 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 0
  store i64 4, ptr %arr.len.gep49, align 8
  %arr.cap.gep50 = getelementptr inbounds %SadArray, ptr %arr_new48, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep50, align 8
  %arr.data51 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
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
  %obj.ptrtoint65 = ptrtoint ptr %arr_new48 to i64
  store i64 %obj.ptrtoint65, ptr %"%mount_\D9\82\D8\B7\D8\A7\D8\B9_\D8\A7\D9\84\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %arr_new66 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep67 = getelementptr inbounds %SadArray, ptr %arr_new66, i32 0, i32 0
  store i64 4, ptr %arr.len.gep67, align 8
  %arr.cap.gep68 = getelementptr inbounds %SadArray, ptr %arr_new66, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep68, align 8
  %arr.data69 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep70 = getelementptr inbounds %SadArray, ptr %arr_new66, i32 0, i32 2
  store ptr %arr.data69, ptr %arr.data.gep70, align 8
  %arr.data.gep71 = getelementptr inbounds %SadArray, ptr %arr_new66, i32 0, i32 2
  %arr.data72 = load ptr, ptr %arr.data.gep71, align 8
  %arr.elem73 = getelementptr i64, ptr %arr.data72, i64 0
  store i64 0, ptr %arr.elem73, align 8
  %arr.data.gep74 = getelementptr inbounds %SadArray, ptr %arr_new66, i32 0, i32 2
  %arr.data75 = load ptr, ptr %arr.data.gep74, align 8
  %arr.elem76 = getelementptr i64, ptr %arr.data75, i64 1
  store i64 0, ptr %arr.elem76, align 8
  %arr.data.gep77 = getelementptr inbounds %SadArray, ptr %arr_new66, i32 0, i32 2
  %arr.data78 = load ptr, ptr %arr.data.gep77, align 8
  %arr.elem79 = getelementptr i64, ptr %arr.data78, i64 2
  store i64 0, ptr %arr.elem79, align 8
  %arr.data.gep80 = getelementptr inbounds %SadArray, ptr %arr_new66, i32 0, i32 2
  %arr.data81 = load ptr, ptr %arr.data.gep80, align 8
  %arr.elem82 = getelementptr i64, ptr %arr.data81, i64 3
  store i64 0, ptr %arr.elem82, align 8
  %obj.ptrtoint83 = ptrtoint ptr %arr_new66 to i64
  store i64 %obj.ptrtoint83, ptr %"%sb_\D8\B3\D8\AD\D8\B1\D9\8A", align 8
  %arr_new84 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep85 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 0
  store i64 4, ptr %arr.len.gep85, align 8
  %arr.cap.gep86 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep86, align 8
  %arr.data87 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep88 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  store ptr %arr.data87, ptr %arr.data.gep88, align 8
  %arr.data.gep89 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  %arr.data90 = load ptr, ptr %arr.data.gep89, align 8
  %arr.elem91 = getelementptr i64, ptr %arr.data90, i64 0
  store i64 0, ptr %arr.elem91, align 8
  %arr.data.gep92 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  %arr.data93 = load ptr, ptr %arr.data.gep92, align 8
  %arr.elem94 = getelementptr i64, ptr %arr.data93, i64 1
  store i64 0, ptr %arr.elem94, align 8
  %arr.data.gep95 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  %arr.data96 = load ptr, ptr %arr.data.gep95, align 8
  %arr.elem97 = getelementptr i64, ptr %arr.data96, i64 2
  store i64 0, ptr %arr.elem97, align 8
  %arr.data.gep98 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  %arr.data99 = load ptr, ptr %arr.data.gep98, align 8
  %arr.elem100 = getelementptr i64, ptr %arr.data99, i64 3
  store i64 0, ptr %arr.elem100, align 8
  %obj.ptrtoint101 = ptrtoint ptr %arr_new84 to i64
  store i64 %obj.ptrtoint101, ptr %"%sb_\D8\A5\D8\B5\D8\AF\D8\A7\D8\B1", align 8
  %arr_new102 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep103 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 0
  store i64 4, ptr %arr.len.gep103, align 8
  %arr.cap.gep104 = getelementptr inbounds %SadArray, ptr %arr_new102, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep104, align 8
  %arr.data105 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
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
  %obj.ptrtoint119 = ptrtoint ptr %arr_new102 to i64
  store i64 %obj.ptrtoint119, ptr %"%sb_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D9\83\D8\AA\D9\84", align 8
  %arr_new120 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep121 = getelementptr inbounds %SadArray, ptr %arr_new120, i32 0, i32 0
  store i64 4, ptr %arr.len.gep121, align 8
  %arr.cap.gep122 = getelementptr inbounds %SadArray, ptr %arr_new120, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep122, align 8
  %arr.data123 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep124 = getelementptr inbounds %SadArray, ptr %arr_new120, i32 0, i32 2
  store ptr %arr.data123, ptr %arr.data.gep124, align 8
  %arr.data.gep125 = getelementptr inbounds %SadArray, ptr %arr_new120, i32 0, i32 2
  %arr.data126 = load ptr, ptr %arr.data.gep125, align 8
  %arr.elem127 = getelementptr i64, ptr %arr.data126, i64 0
  store i64 0, ptr %arr.elem127, align 8
  %arr.data.gep128 = getelementptr inbounds %SadArray, ptr %arr_new120, i32 0, i32 2
  %arr.data129 = load ptr, ptr %arr.data.gep128, align 8
  %arr.elem130 = getelementptr i64, ptr %arr.data129, i64 1
  store i64 0, ptr %arr.elem130, align 8
  %arr.data.gep131 = getelementptr inbounds %SadArray, ptr %arr_new120, i32 0, i32 2
  %arr.data132 = load ptr, ptr %arr.data.gep131, align 8
  %arr.elem133 = getelementptr i64, ptr %arr.data132, i64 2
  store i64 0, ptr %arr.elem133, align 8
  %arr.data.gep134 = getelementptr inbounds %SadArray, ptr %arr_new120, i32 0, i32 2
  %arr.data135 = load ptr, ptr %arr.data.gep134, align 8
  %arr.elem136 = getelementptr i64, ptr %arr.data135, i64 3
  store i64 0, ptr %arr.elem136, align 8
  %obj.ptrtoint137 = ptrtoint ptr %arr_new120 to i64
  store i64 %obj.ptrtoint137, ptr %"%sb_\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\B9\D9\82\D8\AF", align 8
  %arr_new138 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep139 = getelementptr inbounds %SadArray, ptr %arr_new138, i32 0, i32 0
  store i64 4, ptr %arr.len.gep139, align 8
  %arr.cap.gep140 = getelementptr inbounds %SadArray, ptr %arr_new138, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep140, align 8
  %arr.data141 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep142 = getelementptr inbounds %SadArray, ptr %arr_new138, i32 0, i32 2
  store ptr %arr.data141, ptr %arr.data.gep142, align 8
  %arr.data.gep143 = getelementptr inbounds %SadArray, ptr %arr_new138, i32 0, i32 2
  %arr.data144 = load ptr, ptr %arr.data.gep143, align 8
  %arr.elem145 = getelementptr i64, ptr %arr.data144, i64 0
  store i64 0, ptr %arr.elem145, align 8
  %arr.data.gep146 = getelementptr inbounds %SadArray, ptr %arr_new138, i32 0, i32 2
  %arr.data147 = load ptr, ptr %arr.data.gep146, align 8
  %arr.elem148 = getelementptr i64, ptr %arr.data147, i64 1
  store i64 0, ptr %arr.elem148, align 8
  %arr.data.gep149 = getelementptr inbounds %SadArray, ptr %arr_new138, i32 0, i32 2
  %arr.data150 = load ptr, ptr %arr.data.gep149, align 8
  %arr.elem151 = getelementptr i64, ptr %arr.data150, i64 2
  store i64 0, ptr %arr.elem151, align 8
  %arr.data.gep152 = getelementptr inbounds %SadArray, ptr %arr_new138, i32 0, i32 2
  %arr.data153 = load ptr, ptr %arr.data.gep152, align 8
  %arr.elem154 = getelementptr i64, ptr %arr.data153, i64 3
  store i64 0, ptr %arr.elem154, align 8
  %obj.ptrtoint155 = ptrtoint ptr %arr_new138 to i64
  store i64 %obj.ptrtoint155, ptr %"%sb_\D9\83\D8\AA\D9\84_\D8\AD\D8\B1\D8\A9", align 8
  %arr_new156 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep157 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 0
  store i64 4, ptr %arr.len.gep157, align 8
  %arr.cap.gep158 = getelementptr inbounds %SadArray, ptr %arr_new156, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep158, align 8
  %arr.data159 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
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
  %obj.ptrtoint173 = ptrtoint ptr %arr_new156 to i64
  store i64 %obj.ptrtoint173, ptr %"%sb_\D8\B9\D9\82\D8\AF_\D8\AD\D8\B1\D8\A9", align 8
  %arr_new174 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep175 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 0
  store i64 4, ptr %arr.len.gep175, align 8
  %arr.cap.gep176 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep176, align 8
  %arr.data177 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep178 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  store ptr %arr.data177, ptr %arr.data.gep178, align 8
  %arr.data.gep179 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data180 = load ptr, ptr %arr.data.gep179, align 8
  %arr.elem181 = getelementptr i64, ptr %arr.data180, i64 0
  store i64 0, ptr %arr.elem181, align 8
  %arr.data.gep182 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data183 = load ptr, ptr %arr.data.gep182, align 8
  %arr.elem184 = getelementptr i64, ptr %arr.data183, i64 1
  store i64 0, ptr %arr.elem184, align 8
  %arr.data.gep185 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data186 = load ptr, ptr %arr.data.gep185, align 8
  %arr.elem187 = getelementptr i64, ptr %arr.data186, i64 2
  store i64 0, ptr %arr.elem187, align 8
  %arr.data.gep188 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data189 = load ptr, ptr %arr.data.gep188, align 8
  %arr.elem190 = getelementptr i64, ptr %arr.data189, i64 3
  store i64 0, ptr %arr.elem190, align 8
  %obj.ptrtoint191 = ptrtoint ptr %arr_new174 to i64
  store i64 %obj.ptrtoint191, ptr %"%sb_\D8\AD\D8\AC\D9\85_\D9\83\D8\AA\D9\84\D8\A9", align 8
  %arr_new192 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep193 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 0
  store i64 4, ptr %arr.len.gep193, align 8
  %arr.cap.gep194 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep194, align 8
  %arr.data195 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep196 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  store ptr %arr.data195, ptr %arr.data.gep196, align 8
  %arr.data.gep197 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data198 = load ptr, ptr %arr.data.gep197, align 8
  %arr.elem199 = getelementptr i64, ptr %arr.data198, i64 0
  store i64 0, ptr %arr.elem199, align 8
  %arr.data.gep200 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data201 = load ptr, ptr %arr.data.gep200, align 8
  %arr.elem202 = getelementptr i64, ptr %arr.data201, i64 1
  store i64 0, ptr %arr.elem202, align 8
  %arr.data.gep203 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data204 = load ptr, ptr %arr.data.gep203, align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 2
  store i64 0, ptr %arr.elem205, align 8
  %arr.data.gep206 = getelementptr inbounds %SadArray, ptr %arr_new192, i32 0, i32 2
  %arr.data207 = load ptr, ptr %arr.data.gep206, align 8
  %arr.elem208 = getelementptr i64, ptr %arr.data207, i64 3
  store i64 0, ptr %arr.elem208, align 8
  %obj.ptrtoint209 = ptrtoint ptr %arr_new192 to i64
  store i64 %obj.ptrtoint209, ptr %"%sb_\D8\AD\D8\AC\D9\85_\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %arr_new210 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep211 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 0
  store i64 4, ptr %arr.len.gep211, align 8
  %arr.cap.gep212 = getelementptr inbounds %SadArray, ptr %arr_new210, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep212, align 8
  %arr.data213 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
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
  %obj.ptrtoint227 = ptrtoint ptr %arr_new210 to i64
  store i64 %obj.ptrtoint227, ptr %"%sb_\D9\83\D8\AA\D9\84_\D9\84\D9\83\D9\84_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %arr_new228 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep229 = getelementptr inbounds %SadArray, ptr %arr_new228, i32 0, i32 0
  store i64 4, ptr %arr.len.gep229, align 8
  %arr.cap.gep230 = getelementptr inbounds %SadArray, ptr %arr_new228, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep230, align 8
  %arr.data231 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep232 = getelementptr inbounds %SadArray, ptr %arr_new228, i32 0, i32 2
  store ptr %arr.data231, ptr %arr.data.gep232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new228, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 0
  store i64 0, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new228, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 1
  store i64 0, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new228, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 2
  store i64 0, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new228, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 3
  store i64 0, ptr %arr.elem244, align 8
  %obj.ptrtoint245 = ptrtoint ptr %arr_new228 to i64
  store i64 %obj.ptrtoint245, ptr %"%sb_\D8\B9\D9\82\D8\AF_\D9\84\D9\83\D9\84_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A9", align 8
  %arr_new246 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep247 = getelementptr inbounds %SadArray, ptr %arr_new246, i32 0, i32 0
  store i64 4, ptr %arr.len.gep247, align 8
  %arr.cap.gep248 = getelementptr inbounds %SadArray, ptr %arr_new246, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep248, align 8
  %arr.data249 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep250 = getelementptr inbounds %SadArray, ptr %arr_new246, i32 0, i32 2
  store ptr %arr.data249, ptr %arr.data.gep250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new246, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 0
  store i64 0, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new246, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 1
  store i64 0, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new246, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 2
  store i64 0, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new246, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 3
  store i64 0, ptr %arr.elem262, align 8
  %obj.ptrtoint263 = ptrtoint ptr %arr_new246 to i64
  store i64 %obj.ptrtoint263, ptr %"%sb_\D8\B9\D8\AF\D8\AF_\D9\85\D8\AC\D9\85\D9\88\D8\B9\D8\A7\D8\AA", align 8
  %arr_new264 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep265 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 0
  store i64 4, ptr %arr.len.gep265, align 8
  %arr.cap.gep266 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep266, align 8
  %arr.data267 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
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
  %obj.ptrtoint281 = ptrtoint ptr %arr_new264 to i64
  store i64 %obj.ptrtoint281, ptr %"%sb_\D8\AD\D8\A7\D9\84\D8\A9_\D9\86\D8\B8\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AE\D8\B2\D9\86_\D9\85\D8\A4\D9\82\D8\AA", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AE\D8\B2\D9\862", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\AA\D8\AD\D9\85\D9\8A\D9\84\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%BG_SUPERBLOCK", align 8
  store i64 1, ptr %"%BG_DESCRIPTOR", align 8
  store i64 2, ptr %"%BG_BLOCK_BMP", align 8
  store i64 3, ptr %"%BG_INODE_BMP", align 8
  store i64 4, ptr %"%BG_INODE_TABLE", align 8
  store i64 12, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D9\85\D8\A8\D8\A7\D8\B4\D8\B1\D8\A9", align 8
  store i64 1024, ptr %"%\D9\85\D8\A4\D8\B4\D8\B1\D8\A7\D8\AA_\D9\81\D9\8A_\D9\83\D8\AA\D9\84\D8\A9", align 8
  store i64 8, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9_\D8\A7\D8\B3\D9\85_\D9\85\D9\84\D9\81", align 8
  store i64 56, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\B7\D9\88\D9\84_\D8\A7\D8\B3\D9\85", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
