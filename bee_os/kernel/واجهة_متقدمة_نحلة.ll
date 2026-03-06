; ModuleID = 'واجهة_متقدمة_نحلة'
source_filename = "\D9\88\D8\A7\D8\AC\D9\87\D8\A9_\D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

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

declare void @sad_ll_fb_set_pixel(i64, i64, i64)

declare void @sad_ll_fb_fill_rect(i64, i64, i64, i64, i64)

declare void @sad_ll_fb_clear(i64)

define void @"ag_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
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
  call void @"ag_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_2:                                           ; preds = %merge_1
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"ag_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_3
}

define void @"ag_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B3\D8\B7\D8\B1"(i64 %"\D9\86") {
entry:
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  call void @"ag_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86.load")
  ret void
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AE\D9\84\D9\81\D9\8A\D8\A9"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\AE\D9\84\D9\81\D9\8A\D8\A9"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AE\D8\A7\D9\85\D9\84"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\AF\D9\88\D8\AF"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D9\86\D8\B5"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AA\D9\85\D9\8A\D9\8A\D8\B2"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\B2\D8\B1"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\B2\D8\B1_\D9\81\D9\88\D9\82"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9_\D9\81\D9\88\D9\82"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AA\D9\85\D8\B1\D9\8A\D8\B1"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define void @"\D8\A8\D8\AF\D9\84_\D8\A7\D9\84\D8\AB\D9\8A\D9\85"(i64 %"\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF") {
entry:
  %"\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF1" = alloca i64, align 8
  store i64 %"\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF", ptr %"\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF1", align 8
  %"%\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF.load" = load i64, ptr %"\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF.load", 0
  br i1 %cmpgetmp, label %then_4, label %merge_5

merge_5:                                          ; preds = %merge_7, %entry
  ret void

merge_7:                                          ; preds = %then_6, %then_4
  br label %merge_5

then_4:                                           ; preds = %entry
  %"%\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF.load2" = load i64, ptr %"\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF1", align 8
  %cmpletmp = icmp sle i64 %"%\D8\AB\D9\8A\D9\85_\D8\AC\D8\AF\D9\8A\D8\AF.load2", 2
  br i1 %cmpletmp, label %then_6, label %merge_7

then_6:                                           ; preds = %then_4
  br label %merge_7
}

define i64 @"\D8\A7\D9\84\D8\AB\D9\8A\D9\85_\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A"() {
entry:
  ret i64 0
}

define i64 @"ag_\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\AE\D8\B7"() {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  br i1 true, label %then_8, label %merge_9

merge_9:                                          ; preds = %entry
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_10

then_8:                                           ; preds = %entry
  ret i64 -1

while_body_11:                                    ; preds = %while_cond_10
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %shltmp = shl i64 %"%\D9\81.load1", 3
  %addtmp = add i64 0, %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  call void @sad_ll_mem_write64(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load", i64 0)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load2" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %addtmp3 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load2", 1
  call void @sad_ll_mem_write64(i64 %addtmp3, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load4" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %addtmp5 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load4", 2
  call void @sad_ll_mem_write64(i64 %addtmp5, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load6" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load6", 3
  call void @sad_ll_mem_write64(i64 %addtmp7, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load8" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %addtmp9 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load8", 4
  call void @sad_ll_mem_write64(i64 %addtmp9, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load10" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %addtmp11 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load10", 5
  call void @sad_ll_mem_write64(i64 %addtmp11, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load12" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %addtmp13 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load12", 6
  call void @sad_ll_mem_write64(i64 %addtmp13, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load14" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %addtmp15 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load14", 7
  call void @sad_ll_mem_write64(i64 %addtmp15, i64 0)
  %"%\D9\81.load16" = load i64, ptr %"%\D9\81", align 8
  %addtmp17 = add i64 %"%\D9\81.load16", 1
  store i64 %addtmp17, ptr %"%\D9\81", align 8
  br label %while_cond_10

while_cond_10:                                    ; preds = %while_body_11, %merge_9
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 96
  br i1 %cmplttmp, label %while_body_11, label %while_exit_12

while_exit_12:                                    ; preds = %while_cond_10
  store i64 264, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %"%89.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  call void @sad_ll_mem_write64(i64 %"%89.load", i64 24)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %addtmp18 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load", 1
  call void @sad_ll_mem_write64(i64 %addtmp18, i64 60)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load19" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %addtmp20 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load19", 2
  call void @sad_ll_mem_write64(i64 %addtmp20, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load21" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %addtmp22 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load21", 3
  call void @sad_ll_mem_write64(i64 %addtmp22, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load23" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %addtmp24 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load23", 4
  call void @sad_ll_mem_write64(i64 %addtmp24, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load25" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %addtmp26 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load25", 5
  call void @sad_ll_mem_write64(i64 %addtmp26, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load27" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %addtmp28 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load27", 6
  call void @sad_ll_mem_write64(i64 %addtmp28, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load29" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val", align 8
  %addtmp30 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A_val.load29", 7
  call void @sad_ll_mem_write64(i64 %addtmp30, i64 0)
  ret i64 0
}

define void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %"\D8\AD\D8\B1\D9\81_ascii", i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y", i64 %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81") {
entry:
  %"%\D8\B4" = alloca i64, align 8
  %"%\D8\A8\D8\AA" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81" = alloca i64, align 8
  %"%\D8\B5\D9\81" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\814" = alloca i64, align 8
  store i64 %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81", ptr %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\814", align 8
  %"\D8\B3\D8\B7\D8\AD_y3" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %"\D8\B3\D8\B7\D8\AD_x2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"\D8\AD\D8\B1\D9\81_ascii1" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\81_ascii", ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  %"%\D8\AD\D8\B1\D9\81_ascii.load" = load i64, ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AD\D8\B1\D9\81_ascii.load", 32
  br i1 %cmplttmp, label %then_13, label %merge_14

merge_14:                                         ; preds = %entry
  %"%\D8\AD\D8\B1\D9\81_ascii.load5" = load i64, ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AD\D8\B1\D9\81_ascii.load5", 127
  br i1 %cmpgttmp, label %then_15, label %merge_16

merge_16:                                         ; preds = %merge_14
  %"%\D8\AD\D8\B1\D9\81_ascii.load6" = load i64, ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  %subtmp = sub i64 %"%\D8\AD\D8\B1\D9\81_ascii.load6", 32
  store i64 %subtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %shltmp = shl i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 3
  %addtmp = add i64 0, %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  store i64 0, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_17

merge_27:                                         ; preds = %then_26, %while_exit_25
  %"%\D8\B9\D9\85\D9\88\D8\AF.load22" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %addtmp23 = add i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load22", 1
  store i64 %addtmp23, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  br label %while_cond_20

then_13:                                          ; preds = %entry
  ret void

then_15:                                          ; preds = %merge_14
  ret void

then_26:                                          ; preds = %while_exit_25
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load18" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %addtmp19 = add i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %"%\D8\B9\D9\85\D9\88\D8\AF.load18"
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %"%\D8\B5\D9\81.load20" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp21 = add i64 %"%\D8\B3\D8\B7\D8\AD_y.load", %"%\D8\B5\D9\81.load20"
  %"%\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\814", align 8
  call void @sad_ll_fb_set_pixel(i64 %addtmp19, i64 %addtmp21, i64 %"%\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load")
  br label %merge_27

while_body_18:                                    ; preds = %while_cond_17
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %"%\D8\B5\D9\81.load8" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp9 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load", %"%\D8\B5\D9\81.load8"
  call void @sad_ll_mem_read64(i64 %addtmp9)
  store i64 0, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81", align 8
  store i64 0, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  br label %while_cond_20

while_body_21:                                    ; preds = %while_cond_20
  store i64 128, ptr %"%\D8\A8\D8\AA", align 8
  store i64 0, ptr %"%\D8\B4", align 8
  br label %while_cond_23

while_body_24:                                    ; preds = %while_cond_23
  %"%\D8\A8\D8\AA.load" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %shrtmp = lshr i64 %"%\D8\A8\D8\AA.load", 1
  store i64 %shrtmp, ptr %"%\D8\A8\D8\AA", align 8
  %"%\D8\B4.load15" = load i64, ptr %"%\D8\B4", align 8
  %addtmp16 = add i64 %"%\D8\B4.load15", 1
  store i64 %addtmp16, ptr %"%\D8\B4", align 8
  br label %while_cond_23

while_cond_17:                                    ; preds = %while_exit_22, %merge_16
  %"%\D8\B5\D9\81.load" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %cmplttmp7 = icmp slt i64 %"%\D8\B5\D9\81.load", 8
  br i1 %cmplttmp7, label %while_body_18, label %while_exit_19

while_cond_20:                                    ; preds = %merge_27, %while_body_18
  %"%\D8\B9\D9\85\D9\88\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp10 = icmp slt i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load", 8
  br i1 %cmplttmp10, label %while_body_21, label %while_exit_22

while_cond_23:                                    ; preds = %while_body_24, %while_body_21
  %"%\D8\B4.load" = load i64, ptr %"%\D8\B4", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load13" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp14 = icmp slt i64 %"%\D8\B4.load", %"%\D8\B9\D9\85\D9\88\D8\AF.load13"
  br i1 %cmplttmp14, label %while_body_24, label %while_exit_25

while_exit_19:                                    ; preds = %while_cond_17
  ret void

while_exit_22:                                    ; preds = %while_cond_20
  %"%\D8\B5\D9\81.load11" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp12 = add i64 %"%\D8\B5\D9\81.load11", 1
  store i64 %addtmp12, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_17

while_exit_25:                                    ; preds = %while_cond_23
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81.load" = load i64, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81", align 8
  %"%\D8\A8\D8\AA.load17" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %divtmp = sdiv i64 %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81.load", %"%\D8\A8\D8\AA.load17"
  %andtmp = and i64 %divtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_26, label %merge_27
}

define void @"\D8\A8\D8\AF\D9\84_\D8\A7\D9\84\D8\AE\D9\84\D9\81\D9\8A\D8\A9"(i64 %"\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9") {
entry:
  %"\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9", ptr %"\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %"%\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load" = load i64, ptr %"\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load", 0
  br i1 %cmpgetmp, label %then_28, label %merge_29

merge_29:                                         ; preds = %merge_31, %entry
  ret void

merge_31:                                         ; preds = %then_30, %then_28
  br label %merge_29

then_28:                                          ; preds = %entry
  %"%\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load2" = load i64, ptr %"\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A91", align 8
  %cmpletmp = icmp sle i64 %"%\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\AC\D8\AF\D9\8A\D8\AF\D8\A9.load2", 5
  br i1 %cmpletmp, label %then_30, label %merge_31

then_30:                                          ; preds = %then_28
  br label %merge_31
}

define i64 @"\D8\A7\D9\84\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9"() {
entry:
  ret i64 0
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\AE\D9\84\D9\81\D9\8A\D8\A9"() {
entry:
  %"%nx" = alloca i64, align 8
  %"%ny" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D9\86\D9\82\D8\B7\D8\A9" = alloca i64, align 8
  %"%cx" = alloca i64, align 8
  %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85" = alloca i64, align 8
  %"%cy" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\B4\D8\B1\D9\8A\D8\B7" = alloca i64, align 8
  %"%\D8\B4\D8\B1\D9\8A\D8\B7" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\B9\D9\85\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A943" = alloca i64, align 8
  %"%\D8\A840" = alloca i64, align 8
  %"%\D8\B237" = alloca i64, align 8
  %"%\D8\B133" = alloca i64, align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A929" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\B5\D9\81" = alloca i64, align 8
  %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9" = alloca i64, align 8
  %"%\D8\A8" = alloca i64, align 8
  %"%\D8\B2" = alloca i64, align 8
  %"%\D8\B1" = alloca i64, align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D8\B5\D9\81" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9" = alloca i64, align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  br i1 true, label %then_32, label %merge_33

merge_33:                                         ; preds = %entry
  br i1 true, label %then_34, label %merge_35

merge_35:                                         ; preds = %merge_33
  br i1 true, label %then_45, label %merge_46

merge_40:                                         ; preds = %then_39, %while_body_37
  %"%\D8\B2.load13" = load i64, ptr %"%\D8\B2", align 8
  %cmpgttmp14 = icmp sgt i64 %"%\D8\B2.load13", 255
  br i1 %cmpgttmp14, label %then_41, label %merge_42

merge_42:                                         ; preds = %then_41, %merge_40
  %"%\D8\A8.load15" = load i64, ptr %"%\D8\A8", align 8
  %cmpgttmp16 = icmp sgt i64 %"%\D8\A8.load15", 255
  br i1 %cmpgttmp16, label %then_43, label %merge_44

merge_44:                                         ; preds = %then_43, %merge_42
  %"%\D8\B1.load17" = load i64, ptr %"%\D8\B1", align 8
  %shltmp = shl i64 %"%\D8\B1.load17", 16
  %"%\D8\B2.load18" = load i64, ptr %"%\D8\B2", align 8
  %shltmp19 = shl i64 %"%\D8\B2.load18", 8
  %addtmp20 = add i64 %shltmp, %shltmp19
  %"%\D8\A8.load21" = load i64, ptr %"%\D8\A8", align 8
  %addtmp22 = add i64 %addtmp20, %"%\D8\A8.load21"
  store i64 %addtmp22, ptr %"%\D9\84\D9\88\D9\86_\D8\B5\D9\81", align 8
  %"%\D8\B5\D9\81.load23" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp24 = add i64 %"%\D8\B5\D9\81.load23", 4
  store i64 %addtmp24, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_36

merge_46:                                         ; preds = %merge_35
  br i1 true, label %then_54, label %merge_55

merge_51:                                         ; preds = %then_50, %while_body_48
  %"%\D8\B2.load52" = load i64, ptr %"%\D8\B237", align 8
  %cmpgttmp53 = icmp sgt i64 %"%\D8\B2.load52", 255
  br i1 %cmpgttmp53, label %then_52, label %merge_53

merge_53:                                         ; preds = %then_52, %merge_51
  %"%\D8\B1.load54" = load i64, ptr %"%\D8\B133", align 8
  %shltmp55 = shl i64 %"%\D8\B1.load54", 16
  %"%\D8\B2.load56" = load i64, ptr %"%\D8\B237", align 8
  %shltmp57 = shl i64 %"%\D8\B2.load56", 8
  %addtmp58 = add i64 %shltmp55, %shltmp57
  %"%\D8\A8.load59" = load i64, ptr %"%\D8\A840", align 8
  %addtmp60 = add i64 %addtmp58, %"%\D8\A8.load59"
  store i64 %addtmp60, ptr %"%\D9\84\D9\88\D9\86_\D8\B9\D9\85\D9\88\D8\AF", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load61" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %addtmp62 = add i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load61", 4
  store i64 %addtmp62, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  br label %while_cond_47

merge_55:                                         ; preds = %merge_46
  br i1 true, label %then_59, label %merge_60

merge_60:                                         ; preds = %merge_55
  br i1 true, label %then_69, label %merge_70

merge_65:                                         ; preds = %then_64, %while_body_62
  br label %while_cond_66

merge_70:                                         ; preds = %merge_60
  ret void

then_32:                                          ; preds = %entry
  ret void

then_34:                                          ; preds = %merge_33
  store i64 0, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_36

then_39:                                          ; preds = %while_body_37
  store i64 255, ptr %"%\D8\B1", align 8
  br label %merge_40

then_41:                                          ; preds = %merge_40
  store i64 255, ptr %"%\D8\B2", align 8
  br label %merge_42

then_43:                                          ; preds = %merge_42
  store i64 255, ptr %"%\D8\A8", align 8
  br label %merge_44

then_45:                                          ; preds = %merge_35
  store i64 0, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  br label %while_cond_47

then_50:                                          ; preds = %while_body_48
  store i64 255, ptr %"%\D8\B133", align 8
  br label %merge_51

then_52:                                          ; preds = %merge_51
  store i64 255, ptr %"%\D8\B237", align 8
  br label %merge_53

then_54:                                          ; preds = %merge_46
  store i64 0, ptr %"%\D8\B4\D8\B1\D9\8A\D8\B7", align 8
  br label %while_cond_56

then_59:                                          ; preds = %merge_55
  %arr.data69 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem70 = getelementptr i64, ptr %arr.data69, i64 0
  %arr.get71 = load i64, ptr %arr.elem70, align 8
  store i64 %arr.get71, ptr %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%cy", align 8
  store i64 0, ptr %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85", align 8
  br label %while_cond_61

then_64:                                          ; preds = %while_body_62
  store i64 20, ptr %"%cx", align 8
  br label %merge_65

then_69:                                          ; preds = %merge_60
  %arr.data100 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem101 = getelementptr i64, ptr %arr.data100, i64 0
  %arr.get102 = load i64, ptr %arr.elem101, align 8
  store i64 %arr.get102, ptr %"%\D9\84\D9\88\D9\86_\D9\86\D9\82\D8\B7\D8\A9", align 8
  store i64 10, ptr %"%ny", align 8
  br label %while_cond_71

while_body_37:                                    ; preds = %while_cond_36
  %"%\D8\B5\D9\81.load1" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %multmp = mul i64 %"%\D8\B5\D9\81.load1", 100
  %divtmp = sdiv i64 %multmp, 0
  store i64 %divtmp, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9", align 8
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %shrtmp = lshr i64 %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load", 16
  %andtmp = and i64 %shrtmp, 255
  store i64 %andtmp, ptr %"%\D8\B1", align 8
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load2" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %shrtmp3 = lshr i64 %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load2", 8
  %andtmp4 = and i64 %shrtmp3, 255
  store i64 %andtmp4, ptr %"%\D8\B2", align 8
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load5" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %andtmp6 = and i64 %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load5", 255
  store i64 %andtmp6, ptr %"%\D8\A8", align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9", align 8
  %shrtmp7 = lshr i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load", 2
  store i64 %shrtmp7, ptr %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9", align 8
  %"%\D8\B1.load" = load i64, ptr %"%\D8\B1", align 8
  %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9", align 8
  %addtmp = add i64 %"%\D8\B1.load", %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load"
  store i64 %addtmp, ptr %"%\D8\B1", align 8
  %"%\D8\B2.load" = load i64, ptr %"%\D8\B2", align 8
  %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load8" = load i64, ptr %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9", align 8
  %addtmp9 = add i64 %"%\D8\B2.load", %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load8"
  store i64 %addtmp9, ptr %"%\D8\B2", align 8
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load10" = load i64, ptr %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9", align 8
  %addtmp11 = add i64 %"%\D8\A8.load", %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load10"
  store i64 %addtmp11, ptr %"%\D8\A8", align 8
  %"%\D8\B1.load12" = load i64, ptr %"%\D8\B1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B1.load12", 255
  br i1 %cmpgttmp, label %then_39, label %merge_40

while_body_48:                                    ; preds = %while_cond_47
  %"%\D8\B9\D9\85\D9\88\D8\AF.load26" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %multmp27 = mul i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load26", 100
  %divtmp28 = sdiv i64 %multmp27, 0
  store i64 %divtmp28, ptr %"%\D9\86\D8\B3\D8\A8\D8\A929", align 8
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load30" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %shrtmp31 = lshr i64 %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load30", 16
  %andtmp32 = and i64 %shrtmp31, 255
  store i64 %andtmp32, ptr %"%\D8\B133", align 8
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load34" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %shrtmp35 = lshr i64 %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load34", 8
  %andtmp36 = and i64 %shrtmp35, 255
  store i64 %andtmp36, ptr %"%\D8\B237", align 8
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load38" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %andtmp39 = and i64 %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load38", 255
  store i64 %andtmp39, ptr %"%\D8\A840", align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9.load41" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D8\A929", align 8
  %divtmp42 = sdiv i64 %"%\D9\86\D8\B3\D8\A8\D8\A9.load41", 3
  store i64 %divtmp42, ptr %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A943", align 8
  %"%\D8\B1.load44" = load i64, ptr %"%\D8\B133", align 8
  %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load45" = load i64, ptr %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A943", align 8
  %addtmp46 = add i64 %"%\D8\B1.load44", %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load45"
  store i64 %addtmp46, ptr %"%\D8\B133", align 8
  %"%\D8\B2.load47" = load i64, ptr %"%\D8\B237", align 8
  %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load48" = load i64, ptr %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A943", align 8
  %addtmp49 = add i64 %"%\D8\B2.load47", %"%\D8\A5\D8\B6\D8\A7\D9\81\D8\A9.load48"
  store i64 %addtmp49, ptr %"%\D8\B237", align 8
  %"%\D8\B1.load50" = load i64, ptr %"%\D8\B133", align 8
  %cmpgttmp51 = icmp sgt i64 %"%\D8\B1.load50", 255
  br i1 %cmpgttmp51, label %then_50, label %merge_51

while_body_57:                                    ; preds = %while_cond_56
  %arr.data64 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 0
  %arr.get66 = load i64, ptr %arr.elem65, align 8
  store i64 %arr.get66, ptr %"%\D9\84\D9\88\D9\86_\D8\B4\D8\B1\D9\8A\D8\B7", align 8
  %"%\D8\B4\D8\B1\D9\8A\D8\B7.load67" = load i64, ptr %"%\D8\B4\D8\B1\D9\8A\D8\B7", align 8
  %addtmp68 = add i64 %"%\D8\B4\D8\B1\D9\8A\D8\B7.load67", 20
  store i64 %addtmp68, ptr %"%\D8\B4\D8\B1\D9\8A\D8\B7", align 8
  br label %while_cond_56

while_body_62:                                    ; preds = %while_cond_61
  store i64 0, ptr %"%cx", align 8
  %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85.load" = load i64, ptr %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85", align 8
  %andtmp73 = and i64 %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp73, 1
  br i1 %cmpeqtmp, label %then_64, label %merge_65

while_body_67:                                    ; preds = %while_cond_66
  %"%cx.load75" = load i64, ptr %"%cx", align 8
  %addtmp76 = add i64 %"%cx.load75", 4
  %"%266.load" = load i64, ptr %"%cy", align 8
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp76, i64 %"%266.load", i64 32, i64 2, i64 %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load")
  %"%cx.load77" = load i64, ptr %"%cx", align 8
  %addtmp78 = add i64 %"%cx.load77", 2
  %"%cy.load79" = load i64, ptr %"%cy", align 8
  %addtmp80 = add i64 %"%cy.load79", 2
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load81" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp78, i64 %addtmp80, i64 36, i64 2, i64 %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load81")
  %"%cy.load82" = load i64, ptr %"%cy", align 8
  %addtmp83 = add i64 %"%cy.load82", 4
  %"%278.load" = load i64, ptr %"%cx", align 8
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load84" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%278.load", i64 %addtmp83, i64 40, i64 16, i64 %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load84")
  %"%cy.load85" = load i64, ptr %"%cy", align 8
  %addtmp86 = add i64 %"%cy.load85", 20
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load87" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp78, i64 %addtmp86, i64 36, i64 2, i64 %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load87")
  %"%cy.load88" = load i64, ptr %"%cy", align 8
  %addtmp89 = add i64 %"%cy.load88", 22
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load90" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp76, i64 %addtmp89, i64 32, i64 2, i64 %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\8A\D8\A9.load90")
  %"%cx.load91" = load i64, ptr %"%cx", align 8
  %addtmp92 = add i64 %"%cx.load91", 6
  %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load93" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp92, i64 %addtmp83, i64 28, i64 16, i64 %"%\D9\84\D9\88\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9.load93")
  %"%cx.load94" = load i64, ptr %"%cx", align 8
  %addtmp95 = add i64 %"%cx.load94", 44
  store i64 %addtmp95, ptr %"%cx", align 8
  br label %while_cond_66

while_body_72:                                    ; preds = %while_cond_71
  store i64 10, ptr %"%nx", align 8
  br label %while_cond_74

while_body_75:                                    ; preds = %while_cond_74
  %"%nx.load105" = load i64, ptr %"%nx", align 8
  %"%ny.load106" = load i64, ptr %"%ny", align 8
  %"%\D9\84\D9\88\D9\86_\D9\86\D9\82\D8\B7\D8\A9.load" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\86\D9\82\D8\B7\D8\A9", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%nx.load105", i64 %"%ny.load106", i64 3, i64 3, i64 %"%\D9\84\D9\88\D9\86_\D9\86\D9\82\D8\B7\D8\A9.load")
  %"%nx.load107" = load i64, ptr %"%nx", align 8
  %addtmp108 = add i64 %"%nx.load107", 30
  store i64 %addtmp108, ptr %"%nx", align 8
  br label %while_cond_74

while_cond_36:                                    ; preds = %merge_44, %then_34
  %"%\D8\B5\D9\81.load" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B5\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_37, label %while_exit_38

while_cond_47:                                    ; preds = %merge_53, %then_45
  %"%\D8\B9\D9\85\D9\88\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp25 = icmp slt i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load", 0
  br i1 %cmplttmp25, label %while_body_48, label %while_exit_49

while_cond_56:                                    ; preds = %while_body_57, %then_54
  %"%\D8\B4\D8\B1\D9\8A\D8\B7.load" = load i64, ptr %"%\D8\B4\D8\B1\D9\8A\D8\B7", align 8
  %cmplttmp63 = icmp slt i64 %"%\D8\B4\D8\B1\D9\8A\D8\B7.load", 0
  br i1 %cmplttmp63, label %while_body_57, label %while_exit_58

while_cond_61:                                    ; preds = %while_exit_68, %then_59
  %"%cy.load" = load i64, ptr %"%cy", align 8
  %cmplttmp72 = icmp slt i64 %"%cy.load", 0
  br i1 %cmplttmp72, label %while_body_62, label %while_exit_63

while_cond_66:                                    ; preds = %while_body_67, %merge_65
  %"%cx.load" = load i64, ptr %"%cx", align 8
  %cmplttmp74 = icmp slt i64 %"%cx.load", 0
  br i1 %cmplttmp74, label %while_body_67, label %while_exit_68

while_cond_71:                                    ; preds = %while_exit_76, %then_69
  %"%ny.load" = load i64, ptr %"%ny", align 8
  %cmplttmp103 = icmp slt i64 %"%ny.load", 0
  br i1 %cmplttmp103, label %while_body_72, label %while_exit_73

while_cond_74:                                    ; preds = %while_body_75, %while_body_72
  %"%nx.load" = load i64, ptr %"%nx", align 8
  %cmplttmp104 = icmp slt i64 %"%nx.load", 0
  br i1 %cmplttmp104, label %while_body_75, label %while_exit_76

while_exit_38:                                    ; preds = %while_cond_36
  ret void

while_exit_49:                                    ; preds = %while_cond_47
  ret void

while_exit_58:                                    ; preds = %while_cond_56
  ret void

while_exit_63:                                    ; preds = %while_cond_61
  ret void

while_exit_68:                                    ; preds = %while_cond_66
  %"%cy.load96" = load i64, ptr %"%cy", align 8
  %addtmp97 = add i64 %"%cy.load96", 26
  store i64 %addtmp97, ptr %"%cy", align 8
  %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85.load98" = load i64, ptr %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85", align 8
  %addtmp99 = add i64 %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85.load98", 1
  store i64 %addtmp99, ptr %"%\D8\B5\D9\81_\D8\B1\D9\82\D9\85", align 8
  br label %while_cond_61

while_exit_73:                                    ; preds = %while_cond_71
  ret void

while_exit_76:                                    ; preds = %while_cond_74
  %"%ny.load109" = load i64, ptr %"%ny", align 8
  %addtmp110 = add i64 %"%ny.load109", 30
  store i64 %addtmp110, ptr %"%ny", align 8
  br label %while_cond_71
}

define void @"\D9\87\D9\8A\D8\A6_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A7\D8\AA"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_77

while_body_78:                                    ; preds = %while_cond_77
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81.load2"
  store i64 0, ptr %arr.elem4, align 8
  %"%\D9\81.load5" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load5", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_77

while_cond_77:                                    ; preds = %while_body_78, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_78, label %while_exit_79

while_exit_79:                                    ; preds = %while_cond_77
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 0
  store i64 0, ptr %arr.elem7, align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 0
  store i64 0, ptr %arr.elem9, align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 0
  store i64 0, ptr %arr.elem11, align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 1
  store i64 0, ptr %arr.elem13, align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 1
  store i64 0, ptr %arr.elem15, align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 1
  store i64 0, ptr %arr.elem17, align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 2
  store i64 0, ptr %arr.elem19, align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 2
  store i64 0, ptr %arr.elem21, align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 2
  store i64 0, ptr %arr.elem23, align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 3
  store i64 0, ptr %arr.elem25, align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 3
  store i64 0, ptr %arr.elem27, align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 3
  store i64 0, ptr %arr.elem29, align 8
  %arr.data30 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 4
  store i64 0, ptr %arr.elem31, align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 4
  store i64 0, ptr %arr.elem33, align 8
  %arr.data34 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 4
  store i64 0, ptr %arr.elem35, align 8
  %arr.data36 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 5
  store i64 0, ptr %arr.elem37, align 8
  %arr.data38 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 5
  store i64 0, ptr %arr.elem39, align 8
  %arr.data40 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 5
  store i64 0, ptr %arr.elem41, align 8
  %arr.data42 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 6
  store i64 0, ptr %arr.elem43, align 8
  %arr.data44 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem45 = getelementptr i64, ptr %arr.data44, i64 6
  store i64 0, ptr %arr.elem45, align 8
  %arr.data46 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 6
  store i64 0, ptr %arr.elem47, align 8
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"%iy" = alloca i64, align 8
  %"%ix" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_80, label %merge_81

merge_81:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  store i64 %arr.get5, ptr %"%ix", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  store i64 %arr.get9, ptr %"%iy", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load10" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  %cmpeqtmp14 = icmp eq i64 %arr.get13, 1
  br i1 %cmpeqtmp14, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%\D9\81\D9\87\D8\B1\D8\B3.load19" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load19"
  %arr.get22 = load i64, ptr %arr.elem21, align 8
  %cmpeqtmp23 = icmp eq i64 %arr.get22, 0
  br i1 %cmpeqtmp23, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%\D9\81\D9\87\D8\B1\D8\B3.load50" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data51 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load50"
  %arr.get53 = load i64, ptr %arr.elem52, align 8
  %cmpeqtmp54 = icmp eq i64 %arr.get53, 0
  br i1 %cmpeqtmp54, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  %"%\D9\81\D9\87\D8\B1\D8\B3.load86" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data87 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem88 = getelementptr i64, ptr %arr.data87, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load86"
  %arr.get89 = load i64, ptr %arr.elem88, align 8
  %cmpeqtmp90 = icmp eq i64 %arr.get89, 0
  br i1 %cmpeqtmp90, label %then_88, label %merge_89

merge_89:                                         ; preds = %then_88, %merge_87
  %"%\D9\81\D9\87\D8\B1\D8\B3.load116" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data117 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem118 = getelementptr i64, ptr %arr.data117, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load116"
  %arr.get119 = load i64, ptr %arr.elem118, align 8
  %cmpeqtmp120 = icmp eq i64 %arr.get119, 0
  br i1 %cmpeqtmp120, label %then_90, label %merge_91

merge_91:                                         ; preds = %then_90, %merge_89
  %"%\D9\81\D9\87\D8\B1\D8\B3.load149" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data150 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem151 = getelementptr i64, ptr %arr.data150, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load149"
  %arr.get152 = load i64, ptr %arr.elem151, align 8
  %cmpeqtmp153 = icmp eq i64 %arr.get152, 0
  br i1 %cmpeqtmp153, label %then_92, label %merge_93

merge_93:                                         ; preds = %then_92, %merge_91
  %"%\D9\81\D9\87\D8\B1\D8\B3.load178" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data179 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem180 = getelementptr i64, ptr %arr.data179, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load178"
  %arr.get181 = load i64, ptr %arr.elem180, align 8
  %cmpeqtmp182 = icmp eq i64 %arr.get181, 0
  br i1 %cmpeqtmp182, label %then_94, label %merge_95

merge_95:                                         ; preds = %then_94, %merge_93
  ret void

then_80:                                          ; preds = %entry
  ret void

then_82:                                          ; preds = %merge_81
  %"%ix.load" = load i64, ptr %"%ix", align 8
  %subtmp = sub i64 %"%ix.load", 2
  %"%iy.load" = load i64, ptr %"%iy", align 8
  %subtmp15 = sub i64 %"%iy.load", 2
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 0
  %arr.get18 = load i64, ptr %arr.elem17, align 8
  call void @sad_ll_fb_fill_rect(i64 %subtmp, i64 %subtmp15, i64 4, i64 4, i64 %arr.get18)
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%ix.load24" = load i64, ptr %"%ix", align 8
  %addtmp = add i64 %"%ix.load24", 4
  %"%iy.load25" = load i64, ptr %"%iy", align 8
  %addtmp26 = add i64 %"%iy.load25", 6
  call void @sad_ll_fb_fill_rect(i64 %addtmp, i64 %addtmp26, i64 16, i64 4, i64 16766720)
  %"%iy.load27" = load i64, ptr %"%iy", align 8
  %addtmp28 = add i64 %"%iy.load27", 10
  call void @sad_ll_fb_fill_rect(i64 %addtmp, i64 %addtmp28, i64 40, i64 28, i64 16766720)
  %"%ix.load29" = load i64, ptr %"%ix", align 8
  %addtmp30 = add i64 %"%ix.load29", 6
  %"%iy.load31" = load i64, ptr %"%iy", align 8
  %addtmp32 = add i64 %"%iy.load31", 12
  call void @sad_ll_fb_fill_rect(i64 %addtmp30, i64 %addtmp32, i64 36, i64 24, i64 16755200)
  %"%ix.load33" = load i64, ptr %"%ix", align 8
  %addtmp34 = add i64 %"%ix.load33", 18
  %"%iy.load35" = load i64, ptr %"%iy", align 8
  %addtmp36 = add i64 %"%iy.load35", 42
  %arr.data37 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 0
  %arr.get39 = load i64, ptr %arr.elem38, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 68, i64 %addtmp34, i64 %addtmp36, i64 %arr.get39)
  %"%ix.load40" = load i64, ptr %"%ix", align 8
  %addtmp41 = add i64 %"%ix.load40", 26
  %arr.data42 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 0
  %arr.get44 = load i64, ptr %arr.elem43, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 105, i64 %addtmp41, i64 %addtmp36, i64 %arr.get44)
  %"%ix.load45" = load i64, ptr %"%ix", align 8
  %addtmp46 = add i64 %"%ix.load45", 34
  %arr.data47 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 0
  %arr.get49 = load i64, ptr %arr.elem48, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 114, i64 %addtmp46, i64 %addtmp36, i64 %arr.get49)
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%ix.load55" = load i64, ptr %"%ix", align 8
  %addtmp56 = add i64 %"%ix.load55", 8
  %"%iy.load57" = load i64, ptr %"%iy", align 8
  %addtmp58 = add i64 %"%iy.load57", 4
  call void @sad_ll_fb_fill_rect(i64 %addtmp56, i64 %addtmp58, i64 32, i64 36, i64 16777215)
  %"%ix.load59" = load i64, ptr %"%ix", align 8
  %addtmp60 = add i64 %"%ix.load59", 28
  call void @sad_ll_fb_fill_rect(i64 %addtmp60, i64 %addtmp58, i64 12, i64 12, i64 12632256)
  %"%ix.load61" = load i64, ptr %"%ix", align 8
  %addtmp62 = add i64 %"%ix.load61", 10
  %"%iy.load63" = load i64, ptr %"%iy", align 8
  %addtmp64 = add i64 %"%iy.load63", 16
  call void @sad_ll_fb_fill_rect(i64 %addtmp62, i64 %addtmp64, i64 28, i64 2, i64 10526880)
  %"%iy.load65" = load i64, ptr %"%iy", align 8
  %addtmp66 = add i64 %"%iy.load65", 22
  call void @sad_ll_fb_fill_rect(i64 %addtmp62, i64 %addtmp66, i64 28, i64 2, i64 10526880)
  %"%iy.load67" = load i64, ptr %"%iy", align 8
  %addtmp68 = add i64 %"%iy.load67", 28
  call void @sad_ll_fb_fill_rect(i64 %addtmp62, i64 %addtmp68, i64 20, i64 2, i64 10526880)
  %"%ix.load69" = load i64, ptr %"%ix", align 8
  %addtmp70 = add i64 %"%ix.load69", 14
  %"%iy.load71" = load i64, ptr %"%iy", align 8
  %addtmp72 = add i64 %"%iy.load71", 42
  %arr.data73 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 0
  %arr.get75 = load i64, ptr %arr.elem74, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 70, i64 %addtmp70, i64 %addtmp72, i64 %arr.get75)
  %"%ix.load76" = load i64, ptr %"%ix", align 8
  %addtmp77 = add i64 %"%ix.load76", 22
  %arr.data78 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem79 = getelementptr i64, ptr %arr.data78, i64 0
  %arr.get80 = load i64, ptr %arr.elem79, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 105, i64 %addtmp77, i64 %addtmp72, i64 %arr.get80)
  %"%ix.load81" = load i64, ptr %"%ix", align 8
  %addtmp82 = add i64 %"%ix.load81", 30
  %arr.data83 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem84 = getelementptr i64, ptr %arr.data83, i64 0
  %arr.get85 = load i64, ptr %arr.elem84, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 108, i64 %addtmp82, i64 %addtmp72, i64 %arr.get85)
  br label %merge_87

then_88:                                          ; preds = %merge_87
  %"%ix.load91" = load i64, ptr %"%ix", align 8
  %addtmp92 = add i64 %"%ix.load91", 8
  %"%iy.load93" = load i64, ptr %"%iy", align 8
  %addtmp94 = add i64 %"%iy.load93", 4
  call void @sad_ll_fb_fill_rect(i64 %addtmp92, i64 %addtmp94, i64 32, i64 36, i64 4474207)
  %"%ix.load95" = load i64, ptr %"%ix", align 8
  %addtmp96 = add i64 %"%ix.load95", 14
  %"%iy.load97" = load i64, ptr %"%iy", align 8
  %addtmp98 = add i64 %"%iy.load97", 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp96, i64 %addtmp98, i64 8, i64 24, i64 16777215)
  %"%iy.load99" = load i64, ptr %"%iy", align 8
  %addtmp100 = add i64 %"%iy.load99", 32
  call void @sad_ll_fb_fill_rect(i64 %addtmp96, i64 %addtmp100, i64 8, i64 4, i64 16755200)
  %"%iy.load101" = load i64, ptr %"%iy", align 8
  %addtmp102 = add i64 %"%iy.load101", 42
  %arr.data103 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem104 = getelementptr i64, ptr %arr.data103, i64 0
  %arr.get105 = load i64, ptr %arr.elem104, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 69, i64 %addtmp96, i64 %addtmp102, i64 %arr.get105)
  %"%ix.load106" = load i64, ptr %"%ix", align 8
  %addtmp107 = add i64 %"%ix.load106", 22
  %arr.data108 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem109 = getelementptr i64, ptr %arr.data108, i64 0
  %arr.get110 = load i64, ptr %arr.elem109, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 100, i64 %addtmp107, i64 %addtmp102, i64 %arr.get110)
  %"%ix.load111" = load i64, ptr %"%ix", align 8
  %addtmp112 = add i64 %"%ix.load111", 30
  %arr.data113 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem114 = getelementptr i64, ptr %arr.data113, i64 0
  %arr.get115 = load i64, ptr %arr.elem114, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 105, i64 %addtmp112, i64 %addtmp102, i64 %arr.get115)
  br label %merge_89

then_90:                                          ; preds = %merge_89
  %"%ix.load121" = load i64, ptr %"%ix", align 8
  %addtmp122 = add i64 %"%ix.load121", 6
  %"%iy.load123" = load i64, ptr %"%iy", align 8
  %addtmp124 = add i64 %"%iy.load123", 4
  call void @sad_ll_fb_fill_rect(i64 %addtmp122, i64 %addtmp124, i64 36, i64 28, i64 2236962)
  %"%ix.load125" = load i64, ptr %"%ix", align 8
  %addtmp126 = add i64 %"%ix.load125", 8
  %"%iy.load127" = load i64, ptr %"%iy", align 8
  %addtmp128 = add i64 %"%iy.load127", 6
  call void @sad_ll_fb_fill_rect(i64 %addtmp126, i64 %addtmp128, i64 32, i64 24, i64 0)
  %"%ix.load129" = load i64, ptr %"%ix", align 8
  %addtmp130 = add i64 %"%ix.load129", 10
  %"%iy.load131" = load i64, ptr %"%iy", align 8
  %addtmp132 = add i64 %"%iy.load131", 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp130, i64 %addtmp132, i64 6, i64 8, i64 65280)
  %"%ix.load133" = load i64, ptr %"%ix", align 8
  %addtmp134 = add i64 %"%ix.load133", 18
  %"%iy.load135" = load i64, ptr %"%iy", align 8
  %addtmp136 = add i64 %"%iy.load135", 10
  call void @sad_ll_fb_fill_rect(i64 %addtmp134, i64 %addtmp136, i64 10, i64 4, i64 65280)
  %"%ix.load137" = load i64, ptr %"%ix", align 8
  %addtmp138 = add i64 %"%ix.load137", 14
  %"%iy.load139" = load i64, ptr %"%iy", align 8
  %addtmp140 = add i64 %"%iy.load139", 42
  %arr.data141 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem142 = getelementptr i64, ptr %arr.data141, i64 0
  %arr.get143 = load i64, ptr %arr.elem142, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 62, i64 %addtmp138, i64 %addtmp140, i64 %arr.get143)
  %"%ix.load144" = load i64, ptr %"%ix", align 8
  %addtmp145 = add i64 %"%ix.load144", 22
  %arr.data146 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem147 = getelementptr i64, ptr %arr.data146, i64 0
  %arr.get148 = load i64, ptr %arr.elem147, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 95, i64 %addtmp145, i64 %addtmp140, i64 %arr.get148)
  br label %merge_91

then_92:                                          ; preds = %merge_91
  %"%ix.load154" = load i64, ptr %"%ix", align 8
  %addtmp155 = add i64 %"%ix.load154", 6
  %"%iy.load156" = load i64, ptr %"%iy", align 8
  %addtmp157 = add i64 %"%iy.load156", 4
  call void @sad_ll_fb_fill_rect(i64 %addtmp155, i64 %addtmp157, i64 36, i64 32, i64 16766720)
  %"%ix.load158" = load i64, ptr %"%ix", align 8
  %addtmp159 = add i64 %"%ix.load158", 8
  %"%iy.load160" = load i64, ptr %"%iy", align 8
  %addtmp161 = add i64 %"%iy.load160", 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp159, i64 %addtmp161, i64 32, i64 26, i64 16755200)
  %"%ix.load162" = load i64, ptr %"%ix", align 8
  %addtmp163 = add i64 %"%ix.load162", 12
  %"%iy.load164" = load i64, ptr %"%iy", align 8
  %addtmp165 = add i64 %"%iy.load164", 12
  call void @sad_ll_fb_fill_rect(i64 %addtmp163, i64 %addtmp165, i64 24, i64 4, i64 16777215)
  %"%ix.load166" = load i64, ptr %"%ix", align 8
  %addtmp167 = add i64 %"%ix.load166", 10
  %"%iy.load168" = load i64, ptr %"%iy", align 8
  %addtmp169 = add i64 %"%iy.load168", 42
  %arr.data170 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem171 = getelementptr i64, ptr %arr.data170, i64 0
  %arr.get172 = load i64, ptr %arr.elem171, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 70, i64 %addtmp167, i64 %addtmp169, i64 %arr.get172)
  %"%ix.load173" = load i64, ptr %"%ix", align 8
  %addtmp174 = add i64 %"%ix.load173", 18
  %arr.data175 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem176 = getelementptr i64, ptr %arr.data175, i64 0
  %arr.get177 = load i64, ptr %arr.elem176, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 66, i64 %addtmp174, i64 %addtmp169, i64 %arr.get177)
  br label %merge_93

then_94:                                          ; preds = %merge_93
  %"%ix.load183" = load i64, ptr %"%ix", align 8
  %addtmp184 = add i64 %"%ix.load183", 12
  %"%iy.load185" = load i64, ptr %"%iy", align 8
  %addtmp186 = add i64 %"%iy.load185", 4
  call void @sad_ll_fb_fill_rect(i64 %addtmp184, i64 %addtmp186, i64 24, i64 32, i64 10066329)
  %"%ix.load187" = load i64, ptr %"%ix", align 8
  %addtmp188 = add i64 %"%ix.load187", 16
  %"%iy.load189" = load i64, ptr %"%iy", align 8
  %addtmp190 = add i64 %"%iy.load189", 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp188, i64 %addtmp190, i64 16, i64 24, i64 8421504)
  %"%ix.load191" = load i64, ptr %"%ix", align 8
  %addtmp192 = add i64 %"%ix.load191", 20
  %"%iy.load193" = load i64, ptr %"%iy", align 8
  %addtmp194 = add i64 %"%iy.load193", 14
  call void @sad_ll_fb_fill_rect(i64 %addtmp192, i64 %addtmp194, i64 8, i64 8, i64 16777215)
  %"%ix.load195" = load i64, ptr %"%ix", align 8
  %addtmp196 = add i64 %"%ix.load195", 10
  %"%iy.load197" = load i64, ptr %"%iy", align 8
  %addtmp198 = add i64 %"%iy.load197", 42
  %arr.data199 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem200 = getelementptr i64, ptr %arr.data199, i64 0
  %arr.get201 = load i64, ptr %arr.elem200, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 67, i64 %addtmp196, i64 %addtmp198, i64 %arr.get201)
  %"%ix.load202" = load i64, ptr %"%ix", align 8
  %addtmp203 = add i64 %"%ix.load202", 18
  %arr.data204 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 0
  %arr.get206 = load i64, ptr %arr.elem205, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 102, i64 %addtmp203, i64 %addtmp198, i64 %arr.get206)
  %"%ix.load207" = load i64, ptr %"%ix", align 8
  %addtmp208 = add i64 %"%ix.load207", 26
  %arr.data209 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem210 = getelementptr i64, ptr %arr.data209, i64 0
  %arr.get211 = load i64, ptr %arr.elem210, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 103, i64 %addtmp208, i64 %addtmp198, i64 %arr.get211)
  br label %merge_95
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\83\D9\84_\D8\A7\D9\84\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A7\D8\AA"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_96

while_body_97:                                    ; preds = %while_cond_96
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9"(i64 %"%\D9\81.load1")
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load2", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_96

while_cond_96:                                    ; preds = %while_body_97, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_97, label %while_exit_98

while_exit_98:                                    ; preds = %while_cond_96
  ret void
}

define i64 @"\D8\AC\D8\AF_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B9\D9\86\D8\AF"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_99

merge_103:                                        ; preds = %merge_105, %while_body_100
  %"%\D9\81.load8" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load8", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_99

merge_105:                                        ; preds = %merge_107, %then_102
  br label %merge_103

merge_107:                                        ; preds = %merge_109, %then_104
  br label %merge_105

merge_109:                                        ; preds = %merge_111, %then_106
  br label %merge_107

merge_111:                                        ; preds = %then_108
  br label %merge_109

then_102:                                         ; preds = %while_body_100
  %"%\D9\81.load4" = load i64, ptr %"%\D9\81", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %arr.get7
  br i1 %cmpgetmp, label %then_104, label %merge_105

then_104:                                         ; preds = %then_102
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmpgetmp13 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load", %arr.get12
  br i1 %cmpgetmp13, label %then_106, label %merge_107

then_106:                                         ; preds = %then_104
  %"%\D9\81.load14" = load i64, ptr %"%\D9\81", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81.load14"
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  %addtmp18 = add i64 %arr.get17, 0
  %"%\D8\B3\D8\B7\D8\AD_x.load19" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp20 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load19", %addtmp18
  br i1 %cmplttmp20, label %then_108, label %merge_109

then_108:                                         ; preds = %then_106
  %"%\D9\81.load21" = load i64, ptr %"%\D9\81", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81.load21"
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  %addtmp25 = add i64 %arr.get24, 0
  %"%\D8\B3\D8\B7\D8\AD_y.load26" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp27 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load26", %addtmp25
  br i1 %cmplttmp27, label %then_110, label %merge_111

then_110:                                         ; preds = %then_108
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_100:                                   ; preds = %while_cond_99
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_102, label %merge_103

while_cond_99:                                    ; preds = %merge_103, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_100, label %while_exit_101

while_exit_101:                                   ; preds = %while_cond_99
  ret i64 -1
}

define void @"\D8\A3\D8\B2\D9\84_\D9\83\D9\84_\D8\A7\D9\84\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_112

while_body_113:                                   ; preds = %while_cond_112
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load2", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_112

while_cond_112:                                   ; preds = %while_body_113, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_113, label %while_exit_114

while_exit_114:                                   ; preds = %while_cond_112
  ret void
}

define i64 @"\D8\B9\D8\A7\D9\84\D8\AC_\D9\86\D9\82\D8\B1_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %calltmp = call i64 @"\D8\AC\D8\AF_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B9\D9\86\D8\AF"(i64 %"%\D8\B3\D8\B7\D8\AD_x.load", i64 %"%\D8\B3\D8\B7\D8\AD_y.load")
  store i64 %calltmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_115, label %merge_116

merge_116:                                        ; preds = %entry
  call void @"\D8\A3\D8\B2\D9\84_\D9\83\D9\84_\D8\A7\D9\84\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF"()
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  store i64 1, ptr %arr.elem, align 8
  %loadtmp = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  ret i64 %loadtmp

then_115:                                         ; preds = %entry
  call void @"\D8\A3\D8\B2\D9\84_\D9\83\D9\84_\D8\A7\D9\84\D8\AA\D8\AD\D8\AF\D9\8A\D8\AF"()
  ret i64 -1
}

define i64 @"\D9\86\D9\82\D8\B1_\D9\85\D8\B2\D8\AF\D9\88\D8\AC_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_117, label %merge_118

merge_118:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2", 0
  br i1 %cmpgetmp, label %then_119, label %merge_120

merge_120:                                        ; preds = %merge_118
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get

then_117:                                         ; preds = %entry
  ret i64 -1

then_119:                                         ; preds = %merge_118
  ret i64 -1
}

define void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\B3\D8\AD\D8\A8_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%new_y" = alloca i64, align 8
  %"%new_x" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  br i1 true, label %then_121, label %merge_122

merge_122:                                        ; preds = %entry
  br i1 false, label %then_123, label %merge_124

merge_124:                                        ; preds = %merge_122
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %subtmp = sub i64 %"%\D8\B3\D8\B7\D8\AD_x.load", 0
  store i64 %subtmp, ptr %"%new_x", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %subtmp3 = sub i64 %"%\D8\B3\D8\B7\D8\AD_y.load", 0
  store i64 %subtmp3, ptr %"%new_y", align 8
  %"%new_x.load" = load i64, ptr %"%new_x", align 8
  %cmplttmp = icmp slt i64 %"%new_x.load", 0
  br i1 %cmplttmp, label %then_125, label %merge_126

merge_126:                                        ; preds = %then_125, %merge_124
  %"%new_y.load" = load i64, ptr %"%new_y", align 8
  %cmplttmp4 = icmp slt i64 %"%new_y.load", 0
  br i1 %cmplttmp4, label %then_127, label %merge_128

merge_128:                                        ; preds = %then_127, %merge_126
  %"%new_x.load5" = load i64, ptr %"%new_x", align 8
  %addtmp = add i64 %"%new_x.load5", 0
  %cmpgttmp = icmp sgt i64 %addtmp, 0
  br i1 %cmpgttmp, label %then_129, label %merge_130

merge_130:                                        ; preds = %then_129, %merge_128
  %"%new_y.load6" = load i64, ptr %"%new_y", align 8
  %addtmp7 = add i64 %"%new_y.load6", 0
  %cmpgttmp8 = icmp sgt i64 %addtmp7, 0
  br i1 %cmpgttmp8, label %then_131, label %merge_132

merge_132:                                        ; preds = %then_131, %merge_130
  %"%new_x.load9" = load i64, ptr %"%new_x", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 %"%new_x.load9", ptr %arr.elem, align 8
  %"%new_y.load10" = load i64, ptr %"%new_y", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 0
  store i64 %"%new_y.load10", ptr %arr.elem12, align 8
  ret void

then_121:                                         ; preds = %entry
  ret void

then_123:                                         ; preds = %merge_122
  ret void

then_125:                                         ; preds = %merge_124
  store i64 0, ptr %"%new_x", align 8
  br label %merge_126

then_127:                                         ; preds = %merge_126
  store i64 0, ptr %"%new_y", align 8
  br label %merge_128

then_129:                                         ; preds = %merge_128
  store i64 0, ptr %"%new_x", align 8
  br label %merge_130

then_131:                                         ; preds = %merge_130
  store i64 0, ptr %"%new_y", align 8
  br label %merge_132
}

define void @"\D8\A3\D9\81\D9\84\D8\AA_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9"() {
entry:
  ret void
}

define void @"\D8\A3\D8\B8\D9\87\D8\B1_\D9\82\D8\A7\D8\A6\D9\85\D8\A9_\D8\A7\D8\A8\D8\AF\D8\A3"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data1 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem2 = getelementptr i64, ptr %arr.data1, i64 0
  store i64 70, ptr %arr.elem2, align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 0
  store i64 1, ptr %arr.elem4, align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 1
  store i64 0, ptr %arr.elem6, align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 1
  store i64 69, ptr %arr.elem8, align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 1
  store i64 2, ptr %arr.elem10, align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 2
  store i64 0, ptr %arr.elem12, align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 2
  store i64 62, ptr %arr.elem14, align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 2
  store i64 3, ptr %arr.elem16, align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 3
  store i64 -1, ptr %arr.elem18, align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 3
  store i64 0, ptr %arr.elem20, align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 3
  store i64 0, ptr %arr.elem22, align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 4
  store i64 0, ptr %arr.elem24, align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 4
  store i64 84, ptr %arr.elem26, align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 4
  store i64 10, ptr %arr.elem28, align 8
  %arr.data29 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 5
  store i64 0, ptr %arr.elem30, align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 5
  store i64 87, ptr %arr.elem32, align 8
  %arr.data33 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 5
  store i64 11, ptr %arr.elem34, align 8
  %arr.data35 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 6
  store i64 0, ptr %arr.elem36, align 8
  %arr.data37 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 6
  store i64 75, ptr %arr.elem38, align 8
  %arr.data39 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 6
  store i64 12, ptr %arr.elem40, align 8
  ret void
}

define void @"\D8\A3\D8\B8\D9\87\D8\B1_\D9\82\D8\A7\D8\A6\D9\85\D8\A9_\D8\B3\D9\8A\D8\A7\D9\82\D9\8A\D8\A9"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  br i1 false, label %then_133, label %merge_134

merge_134:                                        ; preds = %then_133, %entry
  br i1 false, label %then_135, label %merge_136

merge_136:                                        ; preds = %then_135, %merge_134
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 0
  store i64 78, ptr %arr.elem4, align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 0
  store i64 20, ptr %arr.elem6, align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 1
  store i64 0, ptr %arr.elem8, align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 1
  store i64 79, ptr %arr.elem10, align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 1
  store i64 21, ptr %arr.elem12, align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 2
  store i64 -1, ptr %arr.elem14, align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 2
  store i64 0, ptr %arr.elem16, align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 2
  store i64 0, ptr %arr.elem18, align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 3
  store i64 0, ptr %arr.elem20, align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 3
  store i64 67, ptr %arr.elem22, align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 3
  store i64 30, ptr %arr.elem24, align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 4
  store i64 0, ptr %arr.elem26, align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 4
  store i64 86, ptr %arr.elem28, align 8
  %arr.data29 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 4
  store i64 31, ptr %arr.elem30, align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 5
  store i64 0, ptr %arr.elem32, align 8
  %arr.data33 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 5
  store i64 82, ptr %arr.elem34, align 8
  %arr.data35 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 5
  store i64 22, ptr %arr.elem36, align 8
  ret void

then_133:                                         ; preds = %entry
  br label %merge_134

then_135:                                         ; preds = %merge_134
  br label %merge_136
}

define void @"\D8\A3\D8\AE\D9\81_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"() {
entry:
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"() {
entry:
  %"%item_y" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D9\81\D9\88\D9\82" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D9\86\D8\B5_\D9\82\D8\A7\D8\A6\D9\85\D8\A9" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\AD\D8\AF\D9\88\D8\AF" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\81" = alloca i64, align 8
  br i1 true, label %then_137, label %merge_138

else_144:                                         ; preds = %while_body_140
  %"%\D9\81.load16" = load i64, ptr %"%\D9\81", align 8
  %cmpeqtmp17 = icmp eq i64 %"%\D9\81.load16", 0
  br i1 %cmpeqtmp17, label %then_145, label %merge_146

merge_138:                                        ; preds = %entry
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\81", align 8
  %arr.data1 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem2 = getelementptr i64, ptr %arr.data1, i64 0
  %arr.get3 = load i64, ptr %arr.elem2, align 8
  store i64 %arr.get3, ptr %"%\D9\84\D9\88\D9\86_\D8\AD\D8\AF\D9\88\D8\AF", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 0
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  store i64 %arr.get6, ptr %"%\D9\84\D9\88\D9\86_\D9\86\D8\B5_\D9\82\D8\A7\D8\A6\D9\85\D8\A9", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 0
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  store i64 %arr.get9, ptr %"%\D9\84\D9\88\D9\86_\D9\81\D9\88\D9\82", align 8
  store i64 4, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D8\B1\D8\AA\D9\81\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_139

merge_143:                                        ; preds = %merge_146, %then_142
  %"%\D9\81.load18" = load i64, ptr %"%\D9\81", align 8
  %addtmp19 = add i64 %"%\D9\81.load18", 1
  store i64 %addtmp19, ptr %"%\D9\81", align 8
  br label %while_cond_139

merge_146:                                        ; preds = %then_145, %else_144
  %"%item_y.load20" = load i64, ptr %"%item_y", align 8
  %addtmp21 = add i64 %"%item_y.load20", 4
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 0
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  call void @sad_ll_fb_fill_rect(i64 8, i64 %addtmp21, i64 16, i64 16, i64 %arr.get24)
  %"%\D9\81.load25" = load i64, ptr %"%\D9\81", align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 %"%\D9\81.load25"
  %arr.get28 = load i64, ptr %arr.elem27, align 8
  %"%item_y.load29" = load i64, ptr %"%item_y", align 8
  %addtmp30 = add i64 %"%item_y.load29", 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get28, i64 12, i64 %addtmp30, i64 16777215)
  %"%\D9\81.load31" = load i64, ptr %"%\D9\81", align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 %"%\D9\81.load31"
  %arr.get34 = load i64, ptr %arr.elem33, align 8
  %"%\D9\84\D9\88\D9\86_\D9\86\D8\B5_\D9\82\D8\A7\D8\A6\D9\85\D8\A9.load" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D9\86\D8\B5_\D9\82\D8\A7\D8\A6\D9\85\D8\A9", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get34, i64 30, i64 %addtmp30, i64 %"%\D9\84\D9\88\D9\86_\D9\86\D8\B5_\D9\82\D8\A7\D8\A6\D9\85\D8\A9.load")
  br label %merge_143

then_137:                                         ; preds = %entry
  ret void

then_142:                                         ; preds = %while_body_140
  %"%item_y.load" = load i64, ptr %"%item_y", align 8
  %addtmp15 = add i64 %"%item_y.load", 10
  %"%\D9\84\D9\88\D9\86_\D8\AD\D8\AF\D9\88\D8\AF.load" = load i64, ptr %"%\D9\84\D9\88\D9\86_\D8\AD\D8\AF\D9\88\D8\AF", align 8
  call void @sad_ll_fb_fill_rect(i64 6, i64 %addtmp15, i64 -8, i64 2, i64 %"%\D9\84\D9\88\D9\86_\D8\AD\D8\AF\D9\88\D8\AF.load")
  br label %merge_143

then_145:                                         ; preds = %else_144
  br label %merge_146

while_body_140:                                   ; preds = %while_cond_139
  %"%\D9\81.load10" = load i64, ptr %"%\D9\81", align 8
  %multmp = mul i64 %"%\D9\81.load10", 0
  %addtmp = add i64 2, %multmp
  store i64 %addtmp, ptr %"%item_y", align 8
  %"%\D9\81.load11" = load i64, ptr %"%\D9\81", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81.load11"
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  %cmpeqtmp = icmp eq i64 %arr.get14, -1
  br i1 %cmpeqtmp, label %then_142, label %else_144

while_cond_139:                                   ; preds = %merge_143, %merge_138
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_140, label %while_exit_141

while_exit_141:                                   ; preds = %while_cond_139
  ret void
}

define void @"\D8\AD\D8\AF\D8\AB_\D9\82\D8\A7\D8\A6\D9\85\D8\A9_\D9\81\D9\88\D9\82"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D8\B9\D9\86\D8\B5\D8\B1" = alloca i64, align 8
  %"%\D9\86\D8\B3\D8\A8\D9\8A_y" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  br i1 true, label %then_147, label %merge_148

merge_148:                                        ; preds = %entry
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load", 0
  br i1 %cmplttmp, label %then_149, label %merge_150

merge_150:                                        ; preds = %merge_148
  %"%\D8\B3\D8\B7\D8\AD_x.load3" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B3\D8\B7\D8\AD_x.load3", 4
  br i1 %cmpgttmp, label %then_151, label %merge_152

merge_152:                                        ; preds = %merge_150
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp4 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load", 0
  br i1 %cmplttmp4, label %then_153, label %merge_154

merge_154:                                        ; preds = %merge_152
  %"%\D8\B3\D8\B7\D8\AD_y.load5" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %subtmp = sub i64 %"%\D8\B3\D8\B7\D8\AD_y.load5", 0
  %subtmp6 = sub i64 %subtmp, 2
  store i64 %subtmp6, ptr %"%\D9\86\D8\B3\D8\A8\D9\8A_y", align 8
  %"%\D9\86\D8\B3\D8\A8\D9\8A_y.load" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D9\8A_y", align 8
  %divtmp = sdiv i64 %"%\D9\86\D8\B3\D8\A8\D9\8A_y.load", 0
  store i64 %divtmp, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D9\86\D8\B5\D8\B1.load", 0
  br i1 %cmpgetmp, label %then_155, label %merge_156

merge_156:                                        ; preds = %merge_158, %merge_154
  ret void

merge_158:                                        ; preds = %merge_160, %then_155
  br label %merge_156

merge_160:                                        ; preds = %then_157
  br label %merge_158

then_147:                                         ; preds = %entry
  ret void

then_149:                                         ; preds = %merge_148
  ret void

then_151:                                         ; preds = %merge_150
  ret void

then_153:                                         ; preds = %merge_152
  ret void

then_155:                                         ; preds = %merge_154
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load7" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %cmplttmp8 = icmp slt i64 %"%\D8\B9\D9\86\D8\B5\D8\B1.load7", 0
  br i1 %cmplttmp8, label %then_157, label %merge_158

then_157:                                         ; preds = %then_155
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load9" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B9\D9\86\D8\B5\D8\B1.load9"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, -1
  br i1 %cmpnetmp, label %then_159, label %merge_160

then_159:                                         ; preds = %then_157
  ret void
}

define i64 @"\D9\86\D9\82\D8\B1_\D8\B9\D9\84\D9\89_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D8\A3\D9\85\D8\B1" = alloca i64, align 8
  %"%\D8\B9\D9\86\D8\B5\D8\B1" = alloca i64, align 8
  %"%\D9\86\D8\B3\D8\A8\D9\8A_y" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  br i1 true, label %then_161, label %merge_162

merge_162:                                        ; preds = %entry
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load", 0
  br i1 %cmplttmp, label %then_163, label %merge_164

merge_164:                                        ; preds = %merge_162
  %"%\D8\B3\D8\B7\D8\AD_x.load3" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B3\D8\B7\D8\AD_x.load3", 4
  br i1 %cmpgttmp, label %then_165, label %merge_166

merge_166:                                        ; preds = %merge_164
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp4 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load", 0
  br i1 %cmplttmp4, label %then_167, label %merge_168

merge_168:                                        ; preds = %merge_166
  %"%\D8\B3\D8\B7\D8\AD_y.load5" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %subtmp = sub i64 %"%\D8\B3\D8\B7\D8\AD_y.load5", 0
  %subtmp6 = sub i64 %subtmp, 2
  store i64 %subtmp6, ptr %"%\D9\86\D8\B3\D8\A8\D9\8A_y", align 8
  %"%\D9\86\D8\B3\D8\A8\D9\8A_y.load" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D9\8A_y", align 8
  %divtmp = sdiv i64 %"%\D9\86\D8\B3\D8\A8\D9\8A_y.load", 0
  store i64 %divtmp, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B9\D9\86\D8\B5\D8\B1.load", 0
  br i1 %cmpgetmp, label %then_169, label %merge_170

merge_170:                                        ; preds = %merge_172, %merge_168
  call void @"\D8\A3\D8\AE\D9\81_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"()
  ret i64 -1

merge_172:                                        ; preds = %merge_174, %then_169
  br label %merge_170

merge_174:                                        ; preds = %then_171
  br label %merge_172

then_161:                                         ; preds = %entry
  ret i64 -1

then_163:                                         ; preds = %merge_162
  call void @"\D8\A3\D8\AE\D9\81_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"()
  ret i64 -1

then_165:                                         ; preds = %merge_164
  call void @"\D8\A3\D8\AE\D9\81_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"()
  ret i64 -1

then_167:                                         ; preds = %merge_166
  call void @"\D8\A3\D8\AE\D9\81_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"()
  ret i64 -1

then_169:                                         ; preds = %merge_168
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load7" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %cmplttmp8 = icmp slt i64 %"%\D8\B9\D9\86\D8\B5\D8\B1.load7", 0
  br i1 %cmplttmp8, label %then_171, label %merge_172

then_171:                                         ; preds = %then_169
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load9" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B9\D9\86\D8\B5\D8\B1.load9"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, -1
  br i1 %cmpnetmp, label %then_173, label %merge_174

then_173:                                         ; preds = %then_171
  %"%\D8\B9\D9\86\D8\B5\D8\B1.load10" = load i64, ptr %"%\D8\B9\D9\86\D8\B5\D8\B1", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D8\B9\D9\86\D8\B5\D8\B1.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  store i64 %arr.get13, ptr %"%\D8\A3\D9\85\D8\B1", align 8
  call void @"\D8\A3\D8\AE\D9\81_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"()
  %loadtmp = load i64, ptr %"%\D8\A3\D9\85\D8\B1", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\B9\D9\86\D8\B5\D8\B1"(i64 %"\D8\A7\D9\84\D9\86\D9\88\D8\B9", i64 %wx, i64 %wy, i64 %ww, i64 %wh, i64 %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81", i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\A3\D8\B5\D9\84") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\A3\D8\B5\D9\847" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\A3\D8\B5\D9\84", ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\A3\D8\B5\D9\847", align 8
  %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\816" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\816", align 8
  %wh5 = alloca i64, align 8
  store i64 %wh, ptr %wh5, align 8
  %ww4 = alloca i64, align 8
  store i64 %ww, ptr %ww4, align 8
  %wy3 = alloca i64, align 8
  store i64 %wy, ptr %wy3, align 8
  %wx2 = alloca i64, align 8
  store i64 %wx, ptr %wx2, align 8
  %"\D8\A7\D9\84\D9\86\D9\88\D8\B91" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D9\86\D9\88\D8\B9", ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B91", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_175

merge_179:                                        ; preds = %while_body_176
  %"%\D9\81.load33" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load33", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_175

then_178:                                         ; preds = %while_body_176
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %"%\D8\A7\D9\84\D9\86\D9\88\D8\B9.load" = load i64, ptr %"\D8\A7\D9\84\D9\86\D9\88\D8\B91", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81.load9"
  store i64 %"%\D8\A7\D9\84\D9\86\D9\88\D8\B9.load", ptr %arr.elem11, align 8
  %"%\D9\81.load12" = load i64, ptr %"%\D9\81", align 8
  %"%wx.load" = load i64, ptr %wx2, align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D9\81.load12"
  store i64 %"%wx.load", ptr %arr.elem14, align 8
  %"%\D9\81.load15" = load i64, ptr %"%\D9\81", align 8
  %"%wy.load" = load i64, ptr %wy3, align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D9\81.load15"
  store i64 %"%wy.load", ptr %arr.elem17, align 8
  %"%\D9\81.load18" = load i64, ptr %"%\D9\81", align 8
  %"%ww.load" = load i64, ptr %ww4, align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D9\81.load18"
  store i64 %"%ww.load", ptr %arr.elem20, align 8
  %"%\D9\81.load21" = load i64, ptr %"%\D9\81", align 8
  %"%wh.load" = load i64, ptr %wh5, align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81.load21"
  store i64 %"%wh.load", ptr %arr.elem23, align 8
  %"%\D9\81.load24" = load i64, ptr %"%\D9\81", align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 %"%\D9\81.load24"
  store i64 0, ptr %arr.elem26, align 8
  %"%\D9\81.load27" = load i64, ptr %"%\D9\81", align 8
  %"%\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81.load" = load i64, ptr %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\816", align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 %"%\D9\81.load27"
  store i64 %"%\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81.load", ptr %arr.elem29, align 8
  %"%\D9\81.load30" = load i64, ptr %"%\D9\81", align 8
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\A3\D8\B5\D9\84.load" = load i64, ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\A3\D8\B5\D9\847", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D9\81.load30"
  store i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\A3\D8\B5\D9\84.load", ptr %arr.elem32, align 8
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_176:                                   ; preds = %while_cond_175
  %"%\D9\81.load8" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load8"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_178, label %merge_179

while_cond_175:                                   ; preds = %merge_179, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_176, label %while_exit_177

while_exit_177:                                   ; preds = %while_cond_175
  ret i64 -1
}

define void @"\D8\A7\D8\AD\D8\B0\D9\81_\D8\B9\D9\86\D8\B5\D8\B1"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_180, label %merge_181

merge_181:                                        ; preds = %merge_183, %entry
  ret void

merge_183:                                        ; preds = %then_182, %then_180
  br label %merge_181

then_180:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2", 0
  br i1 %cmplttmp, label %then_182, label %merge_183

then_182:                                         ; preds = %then_180
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  store i64 0, ptr %arr.elem, align 8
  br label %merge_183
}

define void @"\D8\A7\D8\AD\D8\B0\D9\81_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B31", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_184

merge_188:                                        ; preds = %then_187, %while_body_185
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load6", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_184

then_187:                                         ; preds = %while_body_185
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81.load3"
  store i64 0, ptr %arr.elem5, align 8
  br label %merge_188

while_body_185:                                   ; preds = %while_cond_184
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpeqtmp = icmp eq i64 %arr.get, %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3.load"
  br i1 %cmpeqtmp, label %then_187, label %merge_188

while_cond_184:                                   ; preds = %merge_188, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_185, label %while_exit_186

while_exit_186:                                   ; preds = %while_cond_184
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B9\D9\86\D8\B5\D8\B1"(i64 %"\D9\81\D9\87\D8\B1\D8\B3", i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_x", i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_y") {
entry:
  %"%\D8\B9\D8\B1\D8\B6_\D8\A7\D9\85\D8\AA\D9\84\D8\A7\D8\A1" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\A8\D8\B6_y" = alloca i64, align 8
  %"%wh" = alloca i64, align 8
  %"%ww" = alloca i64, align 8
  %"%wy" = alloca i64, align 8
  %"%wx" = alloca i64, align 8
  %"\D8\A3\D8\B3\D8\A7\D8\B3_y3" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_y", ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_y3", align 8
  %"\D8\A3\D8\B3\D8\A7\D8\B3_x2" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_x", ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_x2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_189, label %merge_190

merge_190:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %"%\D8\A3\D8\B3\D8\A7\D8\B3_x.load" = load i64, ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_x2", align 8
  %addtmp = add i64 %"%\D8\A3\D8\B3\D8\A7\D8\B3_x.load", %arr.get7
  store i64 %addtmp, ptr %"%wx", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load8" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load8"
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  %"%\D8\A3\D8\B3\D8\A7\D8\B3_y.load" = load i64, ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_y3", align 8
  %addtmp12 = add i64 %"%\D8\A3\D8\B3\D8\A7\D8\B3_y.load", %arr.get11
  store i64 %addtmp12, ptr %"%wy", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load13" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  store i64 %arr.get16, ptr %"%ww", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load17" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load17"
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  store i64 %arr.get20, ptr %"%wh", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load21" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load21"
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  %cmpeqtmp25 = icmp eq i64 %arr.get24, 0
  br i1 %cmpeqtmp25, label %then_191, label %merge_192

merge_192:                                        ; preds = %then_191, %merge_190
  %"%\D9\81\D9\87\D8\B1\D8\B3.load55" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data56 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem57 = getelementptr i64, ptr %arr.data56, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load55"
  %arr.get58 = load i64, ptr %arr.elem57, align 8
  %cmpeqtmp59 = icmp eq i64 %arr.get58, 0
  br i1 %cmpeqtmp59, label %then_193, label %merge_194

merge_194:                                        ; preds = %then_193, %merge_192
  %"%\D9\81\D9\87\D8\B1\D8\B3.load71" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data72 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem73 = getelementptr i64, ptr %arr.data72, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load71"
  %arr.get74 = load i64, ptr %arr.elem73, align 8
  %cmpeqtmp75 = icmp eq i64 %arr.get74, 0
  br i1 %cmpeqtmp75, label %then_195, label %merge_196

merge_196:                                        ; preds = %then_195, %merge_194
  %"%\D9\81\D9\87\D8\B1\D8\B3.load102" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data103 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem104 = getelementptr i64, ptr %arr.data103, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load102"
  %arr.get105 = load i64, ptr %arr.elem104, align 8
  %cmpeqtmp106 = icmp eq i64 %arr.get105, 0
  br i1 %cmpeqtmp106, label %then_197, label %merge_198

merge_198:                                        ; preds = %merge_200, %merge_196
  %"%\D9\81\D9\87\D8\B1\D8\B3.load121" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data122 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem123 = getelementptr i64, ptr %arr.data122, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load121"
  %arr.get124 = load i64, ptr %arr.elem123, align 8
  %cmpeqtmp125 = icmp eq i64 %arr.get124, 0
  br i1 %cmpeqtmp125, label %then_201, label %merge_202

merge_200:                                        ; preds = %then_199, %then_197
  %"%\D9\81\D9\87\D8\B1\D8\B3.load133" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data134 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem135 = getelementptr i64, ptr %arr.data134, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load133"
  %arr.get136 = load i64, ptr %arr.elem135, align 8
  %"%wx.load137" = load i64, ptr %"%wx", align 8
  %addtmp138 = add i64 %"%wx.load137", 20
  %"%wy.load139" = load i64, ptr %"%wy", align 8
  %addtmp140 = add i64 %"%wy.load139", 4
  %arr.data141 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem142 = getelementptr i64, ptr %arr.data141, i64 0
  %arr.get143 = load i64, ptr %arr.elem142, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get136, i64 %addtmp138, i64 %addtmp140, i64 %arr.get143)
  br label %merge_198

merge_202:                                        ; preds = %merge_204, %merge_198
  %"%\D9\81\D9\87\D8\B1\D8\B3.load169" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data170 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem171 = getelementptr i64, ptr %arr.data170, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load169"
  %arr.get172 = load i64, ptr %arr.elem171, align 8
  %cmpeqtmp173 = icmp eq i64 %arr.get172, 0
  br i1 %cmpeqtmp173, label %then_205, label %merge_206

merge_204:                                        ; preds = %then_203, %then_201
  %"%\D9\81\D9\87\D8\B1\D8\B3.load181" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data182 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem183 = getelementptr i64, ptr %arr.data182, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load181"
  %arr.get184 = load i64, ptr %arr.elem183, align 8
  %"%wx.load185" = load i64, ptr %"%wx", align 8
  %addtmp186 = add i64 %"%wx.load185", 20
  %"%wy.load187" = load i64, ptr %"%wy", align 8
  %addtmp188 = add i64 %"%wy.load187", 4
  %arr.data189 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem190 = getelementptr i64, ptr %arr.data189, i64 0
  %arr.get191 = load i64, ptr %arr.elem190, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get184, i64 %addtmp186, i64 %addtmp188, i64 %arr.get191)
  br label %merge_202

merge_206:                                        ; preds = %then_205, %merge_202
  %"%\D9\81\D9\87\D8\B1\D8\B3.load214" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data215 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem216 = getelementptr i64, ptr %arr.data215, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load214"
  %arr.get217 = load i64, ptr %arr.elem216, align 8
  %cmpeqtmp218 = icmp eq i64 %arr.get217, 0
  br i1 %cmpeqtmp218, label %then_207, label %merge_208

merge_208:                                        ; preds = %then_207, %merge_206
  ret void

then_189:                                         ; preds = %entry
  ret void

then_191:                                         ; preds = %merge_190
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 0
  %arr.get28 = load i64, ptr %arr.elem27, align 8
  %"%wx.load" = load i64, ptr %"%wx", align 8
  %"%wy.load" = load i64, ptr %"%wy", align 8
  %"%ww.load" = load i64, ptr %"%ww", align 8
  %"%wh.load" = load i64, ptr %"%wh", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%wx.load", i64 %"%wy.load", i64 %"%ww.load", i64 %"%wh.load", i64 %arr.get28)
  %"%wx.load29" = load i64, ptr %"%wx", align 8
  %addtmp30 = add i64 %"%wx.load29", 1
  %"%wy.load31" = load i64, ptr %"%wy", align 8
  %addtmp32 = add i64 %"%wy.load31", 1
  %"%ww.load33" = load i64, ptr %"%ww", align 8
  %subtmp = sub i64 %"%ww.load33", 2
  %"%wh.load34" = load i64, ptr %"%wh", align 8
  %subtmp35 = sub i64 %"%wh.load34", 2
  %arr.data36 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 0
  %arr.get38 = load i64, ptr %arr.elem37, align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp30, i64 %addtmp32, i64 %subtmp, i64 %subtmp35, i64 %arr.get38)
  %"%\D9\81\D9\87\D8\B1\D8\B3.load39" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data40 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load39"
  %arr.get42 = load i64, ptr %arr.elem41, align 8
  %"%ww.load43" = load i64, ptr %"%ww", align 8
  %shrtmp = lshr i64 %"%ww.load43", 1
  %"%wx.load44" = load i64, ptr %"%wx", align 8
  %addtmp45 = add i64 %"%wx.load44", %shrtmp
  %subtmp46 = sub i64 %addtmp45, 4
  %"%wh.load47" = load i64, ptr %"%wh", align 8
  %shrtmp48 = lshr i64 %"%wh.load47", 1
  %"%wy.load49" = load i64, ptr %"%wy", align 8
  %addtmp50 = add i64 %"%wy.load49", %shrtmp48
  %subtmp51 = sub i64 %addtmp50, 4
  %arr.data52 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 0
  %arr.get54 = load i64, ptr %arr.elem53, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get42, i64 %subtmp46, i64 %subtmp51, i64 %arr.get54)
  br label %merge_192

then_193:                                         ; preds = %merge_192
  %"%\D9\81\D9\87\D8\B1\D8\B3.load60" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data61 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load60"
  %arr.get63 = load i64, ptr %arr.elem62, align 8
  %"%wx.load64" = load i64, ptr %"%wx", align 8
  %addtmp65 = add i64 %"%wx.load64", 2
  %"%wy.load66" = load i64, ptr %"%wy", align 8
  %addtmp67 = add i64 %"%wy.load66", 2
  %arr.data68 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem69 = getelementptr i64, ptr %arr.data68, i64 0
  %arr.get70 = load i64, ptr %arr.elem69, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get63, i64 %addtmp65, i64 %addtmp67, i64 %arr.get70)
  br label %merge_194

then_195:                                         ; preds = %merge_194
  %arr.data76 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem77 = getelementptr i64, ptr %arr.data76, i64 0
  %arr.get78 = load i64, ptr %arr.elem77, align 8
  %"%wx.load79" = load i64, ptr %"%wx", align 8
  %"%wy.load80" = load i64, ptr %"%wy", align 8
  %"%ww.load81" = load i64, ptr %"%ww", align 8
  %"%wh.load82" = load i64, ptr %"%wh", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%wx.load79", i64 %"%wy.load80", i64 %"%ww.load81", i64 %"%wh.load82", i64 %arr.get78)
  %"%wx.load83" = load i64, ptr %"%wx", align 8
  %addtmp84 = add i64 %"%wx.load83", 1
  %"%wy.load85" = load i64, ptr %"%wy", align 8
  %addtmp86 = add i64 %"%wy.load85", 1
  %"%ww.load87" = load i64, ptr %"%ww", align 8
  %subtmp88 = sub i64 %"%ww.load87", 2
  %"%wh.load89" = load i64, ptr %"%wh", align 8
  %subtmp90 = sub i64 %"%wh.load89", 2
  call void @sad_ll_fb_fill_rect(i64 %addtmp84, i64 %addtmp86, i64 %subtmp88, i64 %subtmp90, i64 16777215)
  %"%wx.load91" = load i64, ptr %"%wx", align 8
  %addtmp92 = add i64 %"%wx.load91", 4
  %"%\D9\81\D9\87\D8\B1\D8\B3.load93" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data94 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem95 = getelementptr i64, ptr %arr.data94, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load93"
  %arr.get96 = load i64, ptr %arr.elem95, align 8
  %shltmp = shl i64 %arr.get96, 3
  %addtmp97 = add i64 %addtmp92, %shltmp
  %"%wy.load98" = load i64, ptr %"%wy", align 8
  %addtmp99 = add i64 %"%wy.load98", 4
  %"%wh.load100" = load i64, ptr %"%wh", align 8
  %subtmp101 = sub i64 %"%wh.load100", 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp97, i64 %addtmp99, i64 2, i64 %subtmp101, i64 0)
  br label %merge_196

then_197:                                         ; preds = %merge_196
  %arr.data107 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem108 = getelementptr i64, ptr %arr.data107, i64 0
  %arr.get109 = load i64, ptr %arr.elem108, align 8
  %"%wx.load110" = load i64, ptr %"%wx", align 8
  %"%wy.load111" = load i64, ptr %"%wy", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%wx.load110", i64 %"%wy.load111", i64 16, i64 16, i64 %arr.get109)
  %"%wx.load112" = load i64, ptr %"%wx", align 8
  %addtmp113 = add i64 %"%wx.load112", 2
  %"%wy.load114" = load i64, ptr %"%wy", align 8
  %addtmp115 = add i64 %"%wy.load114", 2
  call void @sad_ll_fb_fill_rect(i64 %addtmp113, i64 %addtmp115, i64 12, i64 12, i64 16777215)
  %"%\D9\81\D9\87\D8\B1\D8\B3.load116" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data117 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem118 = getelementptr i64, ptr %arr.data117, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load116"
  %arr.get119 = load i64, ptr %arr.elem118, align 8
  %cmpeqtmp120 = icmp eq i64 %arr.get119, 1
  br i1 %cmpeqtmp120, label %then_199, label %merge_200

then_199:                                         ; preds = %then_197
  %"%wx.load126" = load i64, ptr %"%wx", align 8
  %addtmp127 = add i64 %"%wx.load126", 4
  %"%wy.load128" = load i64, ptr %"%wy", align 8
  %addtmp129 = add i64 %"%wy.load128", 4
  %arr.data130 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem131 = getelementptr i64, ptr %arr.data130, i64 0
  %arr.get132 = load i64, ptr %arr.elem131, align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp127, i64 %addtmp129, i64 8, i64 8, i64 %arr.get132)
  br label %merge_200

then_201:                                         ; preds = %merge_198
  %"%wx.load144" = load i64, ptr %"%wx", align 8
  %addtmp145 = add i64 %"%wx.load144", 2
  %arr.data146 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem147 = getelementptr i64, ptr %arr.data146, i64 0
  %arr.get148 = load i64, ptr %arr.elem147, align 8
  %"%wy.load149" = load i64, ptr %"%wy", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp145, i64 %"%wy.load149", i64 12, i64 16, i64 %arr.get148)
  %"%wy.load150" = load i64, ptr %"%wy", align 8
  %addtmp151 = add i64 %"%wy.load150", 2
  %arr.data152 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem153 = getelementptr i64, ptr %arr.data152, i64 0
  %arr.get154 = load i64, ptr %arr.elem153, align 8
  %"%wx.load155" = load i64, ptr %"%wx", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%wx.load155", i64 %addtmp151, i64 16, i64 12, i64 %arr.get154)
  %"%wx.load156" = load i64, ptr %"%wx", align 8
  %addtmp157 = add i64 %"%wx.load156", 3
  %"%wy.load158" = load i64, ptr %"%wy", align 8
  %addtmp159 = add i64 %"%wy.load158", 1
  call void @sad_ll_fb_fill_rect(i64 %addtmp157, i64 %addtmp159, i64 10, i64 14, i64 16777215)
  %"%wx.load160" = load i64, ptr %"%wx", align 8
  %addtmp161 = add i64 %"%wx.load160", 1
  %"%wy.load162" = load i64, ptr %"%wy", align 8
  %addtmp163 = add i64 %"%wy.load162", 3
  call void @sad_ll_fb_fill_rect(i64 %addtmp161, i64 %addtmp163, i64 14, i64 10, i64 16777215)
  %"%\D9\81\D9\87\D8\B1\D8\B3.load164" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data165 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem166 = getelementptr i64, ptr %arr.data165, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load164"
  %arr.get167 = load i64, ptr %arr.elem166, align 8
  %cmpeqtmp168 = icmp eq i64 %arr.get167, 1
  br i1 %cmpeqtmp168, label %then_203, label %merge_204

then_203:                                         ; preds = %then_201
  %"%wx.load174" = load i64, ptr %"%wx", align 8
  %addtmp175 = add i64 %"%wx.load174", 5
  %"%wy.load176" = load i64, ptr %"%wy", align 8
  %addtmp177 = add i64 %"%wy.load176", 5
  %arr.data178 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem179 = getelementptr i64, ptr %arr.data178, i64 0
  %arr.get180 = load i64, ptr %arr.elem179, align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp175, i64 %addtmp177, i64 6, i64 6, i64 %arr.get180)
  br label %merge_204

then_205:                                         ; preds = %merge_202
  %arr.data192 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem193 = getelementptr i64, ptr %arr.data192, i64 0
  %arr.get194 = load i64, ptr %arr.elem193, align 8
  %"%wx.load195" = load i64, ptr %"%wx", align 8
  %"%wy.load196" = load i64, ptr %"%wy", align 8
  %"%ww.load197" = load i64, ptr %"%ww", align 8
  %"%wh.load198" = load i64, ptr %"%wh", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%wx.load195", i64 %"%wy.load196", i64 %"%ww.load197", i64 %"%wh.load198", i64 %arr.get194)
  %"%\D9\81\D9\87\D8\B1\D8\B3.load199" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data200 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem201 = getelementptr i64, ptr %arr.data200, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load199"
  %arr.get202 = load i64, ptr %arr.elem201, align 8
  %"%wh.load203" = load i64, ptr %"%wh", align 8
  %subtmp204 = sub i64 %"%wh.load203", 30
  %multmp = mul i64 %arr.get202, %subtmp204
  %divtmp = sdiv i64 %multmp, 100
  %"%wy.load205" = load i64, ptr %"%wy", align 8
  %addtmp206 = add i64 %"%wy.load205", %divtmp
  store i64 %addtmp206, ptr %"%\D9\85\D9\82\D8\A8\D8\B6_y", align 8
  %"%wx.load207" = load i64, ptr %"%wx", align 8
  %addtmp208 = add i64 %"%wx.load207", 2
  %"%ww.load209" = load i64, ptr %"%ww", align 8
  %subtmp210 = sub i64 %"%ww.load209", 4
  %arr.data211 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem212 = getelementptr i64, ptr %arr.data211, i64 0
  %arr.get213 = load i64, ptr %arr.elem212, align 8
  %"%\D9\85\D9\82\D8\A8\D8\B6_y.load" = load i64, ptr %"%\D9\85\D9\82\D8\A8\D8\B6_y", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp208, i64 %"%\D9\85\D9\82\D8\A8\D8\B6_y.load", i64 %subtmp210, i64 30, i64 %arr.get213)
  br label %merge_206

then_207:                                         ; preds = %merge_206
  %arr.data219 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 0
  %arr.get221 = load i64, ptr %arr.elem220, align 8
  %"%wx.load222" = load i64, ptr %"%wx", align 8
  %"%wy.load223" = load i64, ptr %"%wy", align 8
  %"%ww.load224" = load i64, ptr %"%ww", align 8
  %"%wh.load225" = load i64, ptr %"%wh", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%wx.load222", i64 %"%wy.load223", i64 %"%ww.load224", i64 %"%wh.load225", i64 %arr.get221)
  %"%wx.load226" = load i64, ptr %"%wx", align 8
  %addtmp227 = add i64 %"%wx.load226", 1
  %"%wy.load228" = load i64, ptr %"%wy", align 8
  %addtmp229 = add i64 %"%wy.load228", 1
  %"%ww.load230" = load i64, ptr %"%ww", align 8
  %subtmp231 = sub i64 %"%ww.load230", 2
  %"%wh.load232" = load i64, ptr %"%wh", align 8
  %subtmp233 = sub i64 %"%wh.load232", 2
  call void @sad_ll_fb_fill_rect(i64 %addtmp227, i64 %addtmp229, i64 %subtmp231, i64 %subtmp233, i64 16777215)
  %"%ww.load234" = load i64, ptr %"%ww", align 8
  %subtmp235 = sub i64 %"%ww.load234", 4
  %"%\D9\81\D9\87\D8\B1\D8\B3.load236" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data237 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load236"
  %arr.get239 = load i64, ptr %arr.elem238, align 8
  %multmp240 = mul i64 %subtmp235, %arr.get239
  %divtmp241 = sdiv i64 %multmp240, 100
  store i64 %divtmp241, ptr %"%\D8\B9\D8\B1\D8\B6_\D8\A7\D9\85\D8\AA\D9\84\D8\A7\D8\A1", align 8
  %"%wx.load242" = load i64, ptr %"%wx", align 8
  %addtmp243 = add i64 %"%wx.load242", 2
  %"%wy.load244" = load i64, ptr %"%wy", align 8
  %addtmp245 = add i64 %"%wy.load244", 2
  %"%wh.load246" = load i64, ptr %"%wh", align 8
  %subtmp247 = sub i64 %"%wh.load246", 4
  %arr.data248 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem249 = getelementptr i64, ptr %arr.data248, i64 0
  %arr.get250 = load i64, ptr %arr.elem249, align 8
  %"%\D8\B9\D8\B1\D8\B6_\D8\A7\D9\85\D8\AA\D9\84\D8\A7\D8\A1.load" = load i64, ptr %"%\D8\B9\D8\B1\D8\B6_\D8\A7\D9\85\D8\AA\D9\84\D8\A7\D8\A1", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp243, i64 %addtmp245, i64 %"%\D8\B9\D8\B1\D8\B6_\D8\A7\D9\85\D8\AA\D9\84\D8\A7\D8\A1.load", i64 %subtmp247, i64 %arr.get250)
  br label %merge_208
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B9\D9\86\D8\A7\D8\B5\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3", i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_x", i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_y") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D8\A3\D8\B3\D8\A7\D8\B3_y3" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_y", ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_y3", align 8
  %"\D8\A3\D8\B3\D8\A7\D8\B3_x2" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_x", ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_x2", align 8
  %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B31", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_209

merge_213:                                        ; preds = %merge_215, %while_body_210
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load9", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_209

merge_215:                                        ; preds = %then_214, %then_212
  br label %merge_213

then_212:                                         ; preds = %while_body_210
  %"%\D9\81.load5" = load i64, ptr %"%\D9\81", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\81.load5"
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpeqtmp = icmp eq i64 %arr.get8, %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3.load"
  br i1 %cmpeqtmp, label %then_214, label %merge_215

then_214:                                         ; preds = %then_212
  %"%\D9\81.load10" = load i64, ptr %"%\D9\81", align 8
  %"%\D8\A3\D8\B3\D8\A7\D8\B3_x.load" = load i64, ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_x2", align 8
  %"%\D8\A3\D8\B3\D8\A7\D8\B3_y.load" = load i64, ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_y3", align 8
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B9\D9\86\D8\B5\D8\B1"(i64 %"%\D9\81.load10", i64 %"%\D8\A3\D8\B3\D8\A7\D8\B3_x.load", i64 %"%\D8\A3\D8\B3\D8\A7\D8\B3_y.load")
  br label %merge_215

while_body_210:                                   ; preds = %while_cond_209
  %"%\D9\81.load4" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_212, label %merge_213

while_cond_209:                                   ; preds = %merge_213, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_210, label %while_exit_211

while_exit_211:                                   ; preds = %while_cond_209
  ret void
}

define i64 @"\D9\86\D9\82\D8\B1_\D8\B9\D9\84\D9\89_\D8\B9\D9\86\D8\B5\D8\B1"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y", i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3", i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_x", i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_y") {
entry:
  %"%wy" = alloca i64, align 8
  %"%wx" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"\D8\A3\D8\B3\D8\A7\D8\B3_y5" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_y", ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_y5", align 8
  %"\D8\A3\D8\B3\D8\A7\D8\B3_x4" = alloca i64, align 8
  store i64 %"\D8\A3\D8\B3\D8\A7\D8\B3_x", ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_x4", align 8
  %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B33" = alloca i64, align 8
  store i64 %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B33", align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_216

else_235:                                         ; preds = %then_231
  %"%\D9\81.load57" = load i64, ptr %"%\D9\81", align 8
  %arr.data58 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem59 = getelementptr i64, ptr %arr.data58, i64 %"%\D9\81.load57"
  store i64 0, ptr %arr.elem59, align 8
  br label %merge_234

merge_220:                                        ; preds = %merge_222, %while_body_217
  %"%\D9\81.load11" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load11", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_216

merge_222:                                        ; preds = %merge_224, %then_219
  br label %merge_220

merge_224:                                        ; preds = %merge_226, %then_221
  br label %merge_222

merge_226:                                        ; preds = %merge_228, %then_223
  br label %merge_224

merge_228:                                        ; preds = %merge_230, %then_225
  br label %merge_226

merge_230:                                        ; preds = %then_227
  br label %merge_228

merge_232:                                        ; preds = %merge_234, %then_229
  %"%\D9\81.load49" = load i64, ptr %"%\D9\81", align 8
  %arr.data50 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem51 = getelementptr i64, ptr %arr.data50, i64 %"%\D9\81.load49"
  %arr.get52 = load i64, ptr %arr.elem51, align 8
  %cmpeqtmp53 = icmp eq i64 %arr.get52, 0
  br i1 %cmpeqtmp53, label %then_236, label %merge_237

merge_234:                                        ; preds = %else_235, %then_233
  br label %merge_232

merge_237:                                        ; preds = %then_236, %merge_232
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

then_219:                                         ; preds = %while_body_217
  %"%\D9\81.load7" = load i64, ptr %"%\D9\81", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\81.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B33", align 8
  %cmpeqtmp = icmp eq i64 %arr.get10, %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D9\81\D9\87\D8\B1\D8\B3.load"
  br i1 %cmpeqtmp, label %then_221, label %merge_222

then_221:                                         ; preds = %then_219
  %"%\D9\81.load12" = load i64, ptr %"%\D9\81", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D9\81.load12"
  %arr.get15 = load i64, ptr %arr.elem14, align 8
  %"%\D8\A3\D8\B3\D8\A7\D8\B3_x.load" = load i64, ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_x4", align 8
  %addtmp16 = add i64 %"%\D8\A3\D8\B3\D8\A7\D8\B3_x.load", %arr.get15
  store i64 %addtmp16, ptr %"%wx", align 8
  %"%\D9\81.load17" = load i64, ptr %"%\D9\81", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\81.load17"
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  %"%\D8\A3\D8\B3\D8\A7\D8\B3_y.load" = load i64, ptr %"\D8\A3\D8\B3\D8\A7\D8\B3_y5", align 8
  %addtmp21 = add i64 %"%\D8\A3\D8\B3\D8\A7\D8\B3_y.load", %arr.get20
  store i64 %addtmp21, ptr %"%wy", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%wx.load" = load i64, ptr %"%wx", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %"%wx.load"
  br i1 %cmpgetmp, label %then_223, label %merge_224

then_223:                                         ; preds = %then_221
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"%wy.load" = load i64, ptr %"%wy", align 8
  %cmpgetmp22 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load", %"%wy.load"
  br i1 %cmpgetmp22, label %then_225, label %merge_226

then_225:                                         ; preds = %then_223
  %"%\D9\81.load23" = load i64, ptr %"%\D9\81", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D9\81.load23"
  %arr.get26 = load i64, ptr %arr.elem25, align 8
  %"%wx.load27" = load i64, ptr %"%wx", align 8
  %addtmp28 = add i64 %"%wx.load27", %arr.get26
  %"%\D8\B3\D8\B7\D8\AD_x.load29" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp30 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load29", %addtmp28
  br i1 %cmplttmp30, label %then_227, label %merge_228

then_227:                                         ; preds = %then_225
  %"%\D9\81.load31" = load i64, ptr %"%\D9\81", align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 %"%\D9\81.load31"
  %arr.get34 = load i64, ptr %arr.elem33, align 8
  %"%wy.load35" = load i64, ptr %"%wy", align 8
  %addtmp36 = add i64 %"%wy.load35", %arr.get34
  %"%\D8\B3\D8\B7\D8\AD_y.load37" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp38 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load37", %addtmp36
  br i1 %cmplttmp38, label %then_229, label %merge_230

then_229:                                         ; preds = %then_227
  %"%\D9\81.load39" = load i64, ptr %"%\D9\81", align 8
  %arr.data40 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 %"%\D9\81.load39"
  %arr.get42 = load i64, ptr %arr.elem41, align 8
  %cmpeqtmp43 = icmp eq i64 %arr.get42, 0
  br i1 %cmpeqtmp43, label %then_231, label %merge_232

then_231:                                         ; preds = %then_229
  %"%\D9\81.load44" = load i64, ptr %"%\D9\81", align 8
  %arr.data45 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 %"%\D9\81.load44"
  %arr.get47 = load i64, ptr %arr.elem46, align 8
  %cmpeqtmp48 = icmp eq i64 %arr.get47, 0
  br i1 %cmpeqtmp48, label %then_233, label %else_235

then_233:                                         ; preds = %then_231
  %"%\D9\81.load54" = load i64, ptr %"%\D9\81", align 8
  %arr.data55 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 %"%\D9\81.load54"
  store i64 1, ptr %arr.elem56, align 8
  br label %merge_234

then_236:                                         ; preds = %merge_232
  %"%\D9\81.load60" = load i64, ptr %"%\D9\81", align 8
  %arr.data61 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 %"%\D9\81.load60"
  store i64 1, ptr %arr.elem62, align 8
  br label %merge_237

while_body_217:                                   ; preds = %while_cond_216
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load6"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_219, label %merge_220

while_cond_216:                                   ; preds = %merge_220, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_217, label %while_exit_218

while_exit_218:                                   ; preds = %while_cond_216
  ret i64 -1
}

define void @"\D8\AD\D8\AF\D8\AB_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A9"() {
entry:
  br i1 false, label %then_238, label %merge_239

merge_239:                                        ; preds = %merge_241, %entry
  ret void

merge_241:                                        ; preds = %merge_243, %then_238
  br label %merge_239

merge_243:                                        ; preds = %merge_245, %then_240
  br label %merge_241

merge_245:                                        ; preds = %then_244, %then_242
  br label %merge_243

then_238:                                         ; preds = %entry
  br i1 false, label %then_240, label %merge_241

then_240:                                         ; preds = %then_238
  br i1 false, label %then_242, label %merge_243

then_242:                                         ; preds = %then_240
  br i1 false, label %then_244, label %merge_245

then_244:                                         ; preds = %then_242
  br label %merge_245
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A9"() {
entry:
  %"%m2" = alloca i64, align 8
  %"%m1" = alloca i64, align 8
  %"%h2" = alloca i64, align 8
  %"%h1" = alloca i64, align 8
  %"%cy" = alloca i64, align 8
  %"%cx" = alloca i64, align 8
  store i64 -60, ptr %"%cx", align 8
  store i64 8, ptr %"%cy", align 8
  %"%cx.load" = load i64, ptr %"%cx", align 8
  %subtmp = sub i64 %"%cx.load", 4
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @sad_ll_fb_fill_rect(i64 %subtmp, i64 4, i64 56, i64 22, i64 %arr.get)
  store i64 0, ptr %"%h1", align 8
  store i64 0, ptr %"%h2", align 8
  %"%h1.load" = load i64, ptr %"%h1", align 8
  %addtmp = add i64 48, %"%h1.load"
  %"%cx.load1" = load i64, ptr %"%cx", align 8
  %"%cy.load" = load i64, ptr %"%cy", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %addtmp, i64 %"%cx.load1", i64 %"%cy.load", i64 16777215)
  %"%h2.load" = load i64, ptr %"%h2", align 8
  %addtmp2 = add i64 48, %"%h2.load"
  %"%cx.load3" = load i64, ptr %"%cx", align 8
  %addtmp4 = add i64 %"%cx.load3", 8
  %"%cy.load5" = load i64, ptr %"%cy", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %addtmp2, i64 %addtmp4, i64 %"%cy.load5", i64 16777215)
  %"%cx.load6" = load i64, ptr %"%cx", align 8
  %addtmp7 = add i64 %"%cx.load6", 16
  %"%cy.load8" = load i64, ptr %"%cy", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 58, i64 %addtmp7, i64 %"%cy.load8", i64 16777215)
  store i64 0, ptr %"%m1", align 8
  store i64 0, ptr %"%m2", align 8
  %"%m1.load" = load i64, ptr %"%m1", align 8
  %addtmp9 = add i64 48, %"%m1.load"
  %"%cx.load10" = load i64, ptr %"%cx", align 8
  %addtmp11 = add i64 %"%cx.load10", 24
  %"%cy.load12" = load i64, ptr %"%cy", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %addtmp9, i64 %addtmp11, i64 %"%cy.load12", i64 16777215)
  %"%m2.load" = load i64, ptr %"%m2", align 8
  %addtmp13 = add i64 48, %"%m2.load"
  %"%cx.load14" = load i64, ptr %"%cx", align 8
  %addtmp15 = add i64 %"%cx.load14", 32
  %"%cy.load16" = load i64, ptr %"%cy", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %addtmp13, i64 %addtmp15, i64 %"%cy.load16", i64 16777215)
  ret void
}

define i64 @"\D8\A3\D8\B6\D9\81_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9"(i64 %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\811", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_246

merge_250:                                        ; preds = %while_body_247
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load9", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_246

then_249:                                         ; preds = %while_body_247
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81.load3"
  store i64 1, ptr %arr.elem5, align 8
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %"%\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81.load" = load i64, ptr %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\811", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81.load6"
  store i64 %"%\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81.load", ptr %arr.elem8, align 8
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_247:                                   ; preds = %while_cond_246
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_249, label %merge_250

while_cond_246:                                   ; preds = %merge_250, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_247, label %while_exit_248

while_exit_248:                                   ; preds = %while_cond_246
  ret i64 -1
}

define void @"\D8\A3\D8\B2\D9\84_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_251, label %merge_252

merge_252:                                        ; preds = %merge_254, %entry
  ret void

merge_254:                                        ; preds = %then_253, %then_251
  br label %merge_252

then_251:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2", 0
  br i1 %cmplttmp, label %then_253, label %merge_254

then_253:                                         ; preds = %then_251
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  store i64 0, ptr %arr.elem, align 8
  br label %merge_254
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9_\D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%ty" = alloca i64, align 8
  %"%tx" = alloca i64, align 8
  store i64 -130, ptr %"%tx", align 8
  store i64 6, ptr %"%ty", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_255

merge_259:                                        ; preds = %then_258, %while_body_256
  %"%\D9\81.load16" = load i64, ptr %"%\D9\81", align 8
  %addtmp17 = add i64 %"%\D9\81.load16", 1
  store i64 %addtmp17, ptr %"%\D9\81", align 8
  br label %while_cond_255

then_258:                                         ; preds = %while_body_256
  %arr.data2 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem3 = getelementptr i64, ptr %arr.data2, i64 0
  %arr.get4 = load i64, ptr %arr.elem3, align 8
  %"%tx.load" = load i64, ptr %"%tx", align 8
  %"%ty.load" = load i64, ptr %"%ty", align 8
  call void @sad_ll_fb_fill_rect(i64 %"%tx.load", i64 %"%ty.load", i64 16, i64 18, i64 %arr.get4)
  %"%\D9\81.load5" = load i64, ptr %"%\D9\81", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\81.load5"
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  %"%tx.load9" = load i64, ptr %"%tx", align 8
  %addtmp = add i64 %"%tx.load9", 4
  %"%ty.load10" = load i64, ptr %"%ty", align 8
  %addtmp11 = add i64 %"%ty.load10", 5
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 0
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get8, i64 %addtmp, i64 %addtmp11, i64 %arr.get14)
  %"%tx.load15" = load i64, ptr %"%tx", align 8
  %subtmp = sub i64 %"%tx.load15", 20
  store i64 %subtmp, ptr %"%tx", align 8
  br label %merge_259

while_body_256:                                   ; preds = %while_cond_255
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 1
  br i1 %cmpeqtmp, label %then_258, label %merge_259

while_cond_255:                                   ; preds = %merge_259, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_256, label %while_exit_257

while_exit_257:                                   ; preds = %while_cond_255
  ret void
}

define i64 @"\D8\A3\D8\B6\D9\81_\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1"(i64 %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81", i64 %"\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\81") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\812" = alloca i64, align 8
  store i64 %"\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\81", ptr %"\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\812", align 8
  %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81", ptr %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\811", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_260

merge_264:                                        ; preds = %while_body_261
  %"%\D9\81.load16" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load16", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_260

then_263:                                         ; preds = %while_body_261
  %"%\D9\81.load4" = load i64, ptr %"%\D9\81", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81.load4"
  store i64 1, ptr %arr.elem6, align 8
  %"%\D9\81.load7" = load i64, ptr %"%\D9\81", align 8
  %"%\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81.load" = load i64, ptr %"\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\811", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\81.load7"
  store i64 %"%\D8\AD\D8\B1\D9\81_\D9\85\D8\B9\D8\B1\D9\81.load", ptr %arr.elem9, align 8
  %"%\D9\81.load10" = load i64, ptr %"%\D9\81", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\81.load10"
  store i64 0, ptr %arr.elem12, align 8
  %"%\D9\81.load13" = load i64, ptr %"%\D9\81", align 8
  %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\81.load" = load i64, ptr %"\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\812", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D9\81.load13"
  store i64 %"%\D9\84\D9\88\D9\86_\D8\AE\D9\84\D9\81.load", ptr %arr.elem15, align 8
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_261:                                   ; preds = %while_cond_260
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_263, label %merge_264

while_cond_260:                                   ; preds = %merge_264, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_261, label %while_exit_262

while_exit_262:                                   ; preds = %while_cond_260
  ret i64 -1
}

define void @"\D8\AD\D8\AF\D8\AB_\D8\A7\D9\84\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_265

merge_269:                                        ; preds = %merge_271, %while_body_266
  %"%\D9\81.load13" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load13", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_265

merge_271:                                        ; preds = %then_270, %then_268
  br label %merge_269

then_268:                                         ; preds = %while_body_266
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %subtmp = sub i64 %arr.get5, 1
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81.load6"
  store i64 %subtmp, ptr %arr.elem8, align 8
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %cmpletmp = icmp sle i64 %arr.get12, 0
  br i1 %cmpletmp, label %then_270, label %merge_271

then_270:                                         ; preds = %then_268
  %"%\D9\81.load14" = load i64, ptr %"%\D9\81", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81.load14"
  store i64 0, ptr %arr.elem16, align 8
  br label %merge_271

while_body_266:                                   ; preds = %while_cond_265
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 1
  br i1 %cmpeqtmp, label %then_268, label %merge_269

while_cond_265:                                   ; preds = %merge_269, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_266, label %while_exit_267

while_exit_267:                                   ; preds = %while_cond_265
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA"() {
entry:
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\A8\D9\82\D8\A7\D8\A1" = alloca i64, align 8
  %"%nx" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%ny" = alloca i64, align 8
  store i64 10, ptr %"%ny", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_272

merge_276:                                        ; preds = %then_275, %while_body_273
  %"%\D9\81.load32" = load i64, ptr %"%\D9\81", align 8
  %addtmp33 = add i64 %"%\D9\81.load32", 1
  store i64 %addtmp33, ptr %"%\D9\81", align 8
  br label %while_cond_272

then_275:                                         ; preds = %while_body_273
  store i64 -10, ptr %"%nx", align 8
  %"%nx.load" = load i64, ptr %"%nx", align 8
  %addtmp = add i64 %"%nx.load", 2
  %"%ny.load" = load i64, ptr %"%ny", align 8
  %addtmp2 = add i64 %"%ny.load", 2
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp, i64 %addtmp2, i64 -4, i64 -4, i64 %arr.get6)
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 0
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp, i64 %addtmp2, i64 4, i64 -4, i64 %arr.get9)
  %"%nx.load10" = load i64, ptr %"%nx", align 8
  %addtmp11 = add i64 %"%nx.load10", 12
  %"%ny.load12" = load i64, ptr %"%ny", align 8
  %addtmp13 = add i64 %"%ny.load12", 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 33, i64 %addtmp11, i64 %addtmp13, i64 16777215)
  %"%\D9\81.load14" = load i64, ptr %"%\D9\81", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81.load14"
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  %"%nx.load18" = load i64, ptr %"%nx", align 8
  %addtmp19 = add i64 %"%nx.load18", 24
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %arr.get17, i64 %addtmp19, i64 %addtmp13, i64 16777215)
  %"%\D9\81.load20" = load i64, ptr %"%\D9\81", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D9\81.load20"
  %arr.get23 = load i64, ptr %arr.elem22, align 8
  %multmp = mul i64 %arr.get23, -8
  %divtmp = sdiv i64 %multmp, 0
  store i64 %divtmp, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\A8\D9\82\D8\A7\D8\A1", align 8
  %"%nx.load24" = load i64, ptr %"%nx", align 8
  %addtmp25 = add i64 %"%nx.load24", 4
  %"%ny.load26" = load i64, ptr %"%ny", align 8
  %addtmp27 = add i64 %"%ny.load26", 0
  %subtmp = sub i64 %addtmp27, 6
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 0
  %arr.get30 = load i64, ptr %arr.elem29, align 8
  %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\A8\D9\82\D8\A7\D8\A1.load" = load i64, ptr %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\A8\D9\82\D8\A7\D8\A1", align 8
  call void @sad_ll_fb_fill_rect(i64 %addtmp25, i64 %subtmp, i64 %"%\D9\86\D8\B3\D8\A8\D8\A9_\D8\A8\D9\82\D8\A7\D8\A1.load", i64 2, i64 %arr.get30)
  %addtmp31 = add i64 %addtmp27, 8
  store i64 %addtmp31, ptr %"%ny", align 8
  br label %merge_276

while_body_273:                                   ; preds = %while_cond_272
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 1
  br i1 %cmpeqtmp, label %then_275, label %merge_276

while_cond_272:                                   ; preds = %merge_276, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_273, label %while_exit_274

while_exit_274:                                   ; preds = %while_cond_272
  ret void
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\AD\D8\A7\D9\81\D8\B8\D8\A9"() {
entry:
  br i1 true, label %then_277, label %merge_278

merge_278:                                        ; preds = %entry
  ret i64 0

then_277:                                         ; preds = %entry
  ret i64 -1
}

define i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A3\D8\B6\D9\81_\D8\AD\D8\B1\D9\81"(i64 %"\D8\AD\D8\B1\D9\81_ascii") {
entry:
  %"\D8\AD\D8\B1\D9\81_ascii1" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\81_ascii", ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  br i1 true, label %then_279, label %merge_280

merge_280:                                        ; preds = %entry
  %"%\D8\AD\D8\B1\D9\81_ascii.load" = load i64, ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  call void @sad_ll_mem_write64(i64 0, i64 %"%\D8\AD\D8\B1\D9\81_ascii.load")
  ret i64 0

then_279:                                         ; preds = %entry
  ret i64 -1
}

define void @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A7\D9\85\D8\B3\D8\AD"() {
entry:
  ret void
}

define i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A7\D9\82\D8\B1\D8\A3_\D8\AD\D8\B1\D9\81"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_281, label %merge_282

merge_282:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2", 0
  br i1 %cmpgetmp, label %then_283, label %merge_284

merge_284:                                        ; preds = %merge_282
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %addtmp = add i64 0, %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  call void @sad_ll_mem_read64(i64 %addtmp)
  ret i64 0

then_281:                                         ; preds = %entry
  ret i64 0

then_283:                                         ; preds = %merge_282
  ret i64 0
}

define i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A7\D9\84\D8\B7\D9\88\D9\84"() {
entry:
  ret i64 0
}

define void @"\D8\A3\D8\B8\D9\87\D8\B1_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"() {
entry:
  ret void
}

define void @"\D8\A3\D8\AE\D9\81_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"() {
entry:
  ret void
}

define void @"\D8\A8\D8\AF\D9\84_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"() {
entry:
  br i1 true, label %then_285, label %else_287

else_287:                                         ; preds = %entry
  br label %merge_286

merge_286:                                        ; preds = %else_287, %then_285
  ret void

then_285:                                         ; preds = %entry
  br label %merge_286
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"() {
entry:
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD113" = alloca i64, align 8
  %"%kx108" = alloca i64, align 8
  %"%\D9\84\D9\88\D9\86_\D8\B4\D9\8A\D9\81\D8\AA" = alloca i64, align 8
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD" = alloca i64, align 8
  %"%kx" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%base_y" = alloca i64, align 8
  br i1 true, label %then_288, label %merge_289

merge_289:                                        ; preds = %entry
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @sad_ll_fb_fill_rect(i64 -4, i64 -4, i64 8, i64 8, i64 %arr.get)
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 75, i64 8, i64 6, i64 16777215)
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 66, i64 16, i64 6, i64 16777215)
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 68, i64 24, i64 6, i64 16777215)
  call void @sad_ll_fb_fill_rect(i64 -20, i64 2, i64 16, i64 16, i64 16711680)
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 88, i64 -16, i64 6, i64 16777215)
  store i64 28, ptr %"%base_y", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_290

merge_294:                                        ; preds = %then_293, %while_body_291
  %"%kx.load" = load i64, ptr %"%kx", align 8
  %addtmp12 = add i64 %"%kx.load", 1
  %"%base_y.load13" = load i64, ptr %"%base_y", align 8
  %addtmp14 = add i64 %"%base_y.load13", 1
  call void @sad_ll_fb_fill_rect(i64 %addtmp12, i64 %addtmp14, i64 -2, i64 -2, i64 16777215)
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load", 31
  br i1 %cmpgttmp, label %then_295, label %merge_296

merge_296:                                        ; preds = %then_295, %merge_294
  %"%\D9\81.load23" = load i64, ptr %"%\D9\81", align 8
  %addtmp24 = add i64 %"%\D9\81.load23", 1
  store i64 %addtmp24, ptr %"%\D9\81", align 8
  br label %while_cond_290

merge_301:                                        ; preds = %then_300, %while_body_298
  %"%kx.load41" = load i64, ptr %"%kx", align 8
  %addtmp42 = add i64 %"%kx.load41", 1
  %"%base_y.load43" = load i64, ptr %"%base_y", align 8
  %addtmp44 = add i64 %"%base_y.load43", 1
  call void @sad_ll_fb_fill_rect(i64 %addtmp42, i64 %addtmp44, i64 -2, i64 -2, i64 16777215)
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load45" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpgttmp46 = icmp sgt i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load45", 31
  br i1 %cmpgttmp46, label %then_302, label %merge_303

merge_303:                                        ; preds = %then_302, %merge_301
  %"%\D9\81.load55" = load i64, ptr %"%\D9\81", align 8
  %addtmp56 = add i64 %"%\D9\81.load55", 1
  store i64 %addtmp56, ptr %"%\D9\81", align 8
  br label %while_cond_297

merge_308:                                        ; preds = %then_307, %while_body_305
  %"%kx.load76" = load i64, ptr %"%kx", align 8
  %addtmp77 = add i64 %"%kx.load76", 1
  %"%base_y.load78" = load i64, ptr %"%base_y", align 8
  %addtmp79 = add i64 %"%base_y.load78", 1
  call void @sad_ll_fb_fill_rect(i64 %addtmp77, i64 %addtmp79, i64 -2, i64 -2, i64 16777215)
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load80" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpgttmp81 = icmp sgt i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load80", 31
  br i1 %cmpgttmp81, label %then_309, label %merge_310

merge_310:                                        ; preds = %then_309, %merge_308
  %"%\D9\81.load90" = load i64, ptr %"%\D9\81", align 8
  %addtmp91 = add i64 %"%\D9\81.load90", 1
  store i64 %addtmp91, ptr %"%\D9\81", align 8
  br label %while_cond_304

merge_312:                                        ; preds = %then_311, %while_exit_306
  %"%base_y.load95" = load i64, ptr %"%base_y", align 8
  %addtmp96 = add i64 %"%base_y.load95", 14
  %arr.data97 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem98 = getelementptr i64, ptr %arr.data97, i64 0
  %arr.get99 = load i64, ptr %arr.elem98, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 83, i64 22, i64 %addtmp96, i64 %arr.get99)
  %arr.data100 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem101 = getelementptr i64, ptr %arr.data100, i64 0
  %arr.get102 = load i64, ptr %arr.elem101, align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 104, i64 30, i64 %addtmp96, i64 %arr.get102)
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_313

merge_317:                                        ; preds = %then_316, %while_body_314
  %"%kx.load120" = load i64, ptr %"%kx108", align 8
  %addtmp121 = add i64 %"%kx.load120", 1
  %"%base_y.load122" = load i64, ptr %"%base_y", align 8
  %addtmp123 = add i64 %"%base_y.load122", 1
  call void @sad_ll_fb_fill_rect(i64 %addtmp121, i64 %addtmp123, i64 -2, i64 -2, i64 16777215)
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load124" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD113", align 8
  %cmpgttmp125 = icmp sgt i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load124", 31
  br i1 %cmpgttmp125, label %then_318, label %merge_319

merge_319:                                        ; preds = %then_318, %merge_317
  %"%\D9\81.load134" = load i64, ptr %"%\D9\81", align 8
  %addtmp135 = add i64 %"%\D9\81.load134", 1
  store i64 %addtmp135, ptr %"%\D9\81", align 8
  br label %while_cond_313

then_288:                                         ; preds = %entry
  ret void

then_293:                                         ; preds = %while_body_291
  %"%\D9\81.load8" = load i64, ptr %"%\D9\81", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81.load8"
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  store i64 %arr.get11, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br label %merge_294

then_295:                                         ; preds = %merge_294
  %"%kx.load15" = load i64, ptr %"%kx", align 8
  %addtmp16 = add i64 %"%kx.load15", 16
  %"%base_y.load17" = load i64, ptr %"%base_y", align 8
  %addtmp18 = add i64 %"%base_y.load17", 14
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 0
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load22" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load22", i64 %addtmp16, i64 %addtmp18, i64 %arr.get21)
  br label %merge_296

then_300:                                         ; preds = %while_body_298
  %"%\D9\81.load37" = load i64, ptr %"%\D9\81", align 8
  %arr.data38 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 %"%\D9\81.load37"
  %arr.get40 = load i64, ptr %arr.elem39, align 8
  store i64 %arr.get40, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br label %merge_301

then_302:                                         ; preds = %merge_301
  %"%kx.load47" = load i64, ptr %"%kx", align 8
  %addtmp48 = add i64 %"%kx.load47", 16
  %"%base_y.load49" = load i64, ptr %"%base_y", align 8
  %addtmp50 = add i64 %"%base_y.load49", 14
  %arr.data51 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 0
  %arr.get53 = load i64, ptr %arr.elem52, align 8
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load54" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load54", i64 %addtmp48, i64 %addtmp50, i64 %arr.get53)
  br label %merge_303

then_307:                                         ; preds = %while_body_305
  %"%\D9\81.load72" = load i64, ptr %"%\D9\81", align 8
  %arr.data73 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 %"%\D9\81.load72"
  %arr.get75 = load i64, ptr %arr.elem74, align 8
  store i64 %arr.get75, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br label %merge_308

then_309:                                         ; preds = %merge_308
  %"%kx.load82" = load i64, ptr %"%kx", align 8
  %addtmp83 = add i64 %"%kx.load82", 16
  %"%base_y.load84" = load i64, ptr %"%base_y", align 8
  %addtmp85 = add i64 %"%base_y.load84", 14
  %arr.data86 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem87 = getelementptr i64, ptr %arr.data86, i64 0
  %arr.get88 = load i64, ptr %arr.elem87, align 8
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load89" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load89", i64 %addtmp83, i64 %addtmp85, i64 %arr.get88)
  br label %merge_310

then_311:                                         ; preds = %while_exit_306
  %arr.data92 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem93 = getelementptr i64, ptr %arr.data92, i64 0
  %arr.get94 = load i64, ptr %arr.elem93, align 8
  store i64 %arr.get94, ptr %"%\D9\84\D9\88\D9\86_\D8\B4\D9\8A\D9\81\D8\AA", align 8
  br label %merge_312

then_316:                                         ; preds = %while_body_314
  %"%\D9\81.load116" = load i64, ptr %"%\D9\81", align 8
  %arr.data117 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem118 = getelementptr i64, ptr %arr.data117, i64 %"%\D9\81.load116"
  %arr.get119 = load i64, ptr %arr.elem118, align 8
  store i64 %arr.get119, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD113", align 8
  br label %merge_317

then_318:                                         ; preds = %merge_317
  %"%kx.load126" = load i64, ptr %"%kx108", align 8
  %addtmp127 = add i64 %"%kx.load126", 16
  %"%base_y.load128" = load i64, ptr %"%base_y", align 8
  %addtmp129 = add i64 %"%base_y.load128", 14
  %arr.data130 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem131 = getelementptr i64, ptr %arr.data130, i64 0
  %arr.get132 = load i64, ptr %arr.elem131, align 8
  %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load133" = load i64, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD113", align 8
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load133", i64 %addtmp127, i64 %addtmp129, i64 %arr.get132)
  br label %merge_319

while_body_291:                                   ; preds = %while_cond_290
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %multmp = mul i64 %"%\D9\81.load1", 2
  %addtmp = add i64 10, %multmp
  store i64 %addtmp, ptr %"%kx", align 8
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  store i64 %arr.get5, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br i1 false, label %then_293, label %merge_294

while_body_298:                                   ; preds = %while_cond_297
  %"%\D9\81.load27" = load i64, ptr %"%\D9\81", align 8
  %multmp28 = mul i64 %"%\D9\81.load27", 2
  %addtmp29 = add i64 20, %multmp28
  store i64 %addtmp29, ptr %"%kx", align 8
  %"%\D9\81.load30" = load i64, ptr %"%\D9\81", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D9\81.load30"
  %arr.get33 = load i64, ptr %arr.elem32, align 8
  store i64 %arr.get33, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br i1 false, label %then_300, label %merge_301

while_body_305:                                   ; preds = %while_cond_304
  %"%\D9\81.load59" = load i64, ptr %"%\D9\81", align 8
  %multmp60 = mul i64 %"%\D9\81.load59", 2
  %addtmp61 = add i64 30, %multmp60
  store i64 %addtmp61, ptr %"%kx", align 8
  %"%\D9\81.load62" = load i64, ptr %"%\D9\81", align 8
  %arr.data63 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem64 = getelementptr i64, ptr %arr.data63, i64 %"%\D9\81.load62"
  %arr.get65 = load i64, ptr %arr.elem64, align 8
  store i64 %arr.get65, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br i1 false, label %then_307, label %merge_308

while_body_314:                                   ; preds = %while_cond_313
  %"%\D9\81.load105" = load i64, ptr %"%\D9\81", align 8
  %multmp106 = mul i64 %"%\D9\81.load105", 2
  %addtmp107 = add i64 70, %multmp106
  store i64 %addtmp107, ptr %"%kx108", align 8
  %"%\D9\81.load109" = load i64, ptr %"%\D9\81", align 8
  %arr.data110 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem111 = getelementptr i64, ptr %arr.data110, i64 %"%\D9\81.load109"
  %arr.get112 = load i64, ptr %arr.elem111, align 8
  store i64 %arr.get112, ptr %"%\D8\AD\D8\B1\D9\81_\D8\A7\D9\84\D9\85\D9\81\D8\AA\D8\A7\D8\AD113", align 8
  br i1 false, label %then_316, label %merge_317

while_cond_290:                                   ; preds = %merge_296, %merge_289
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_291, label %while_exit_292

while_cond_297:                                   ; preds = %merge_303, %while_exit_292
  %"%\D9\81.load25" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp26 = icmp slt i64 %"%\D9\81.load25", 0
  br i1 %cmplttmp26, label %while_body_298, label %while_exit_299

while_cond_304:                                   ; preds = %merge_310, %while_exit_299
  %"%\D9\81.load57" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp58 = icmp slt i64 %"%\D9\81.load57", 12
  br i1 %cmplttmp58, label %while_body_305, label %while_exit_306

while_cond_313:                                   ; preds = %merge_319, %merge_312
  %"%\D9\81.load103" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp104 = icmp slt i64 %"%\D9\81.load103", 7
  br i1 %cmplttmp104, label %while_body_314, label %while_exit_315

while_exit_292:                                   ; preds = %while_cond_290
  %"%base_y.load" = load i64, ptr %"%base_y", align 8
  %addtmp6 = add i64 %"%base_y.load", 0
  %addtmp7 = add i64 %addtmp6, 4
  store i64 %addtmp7, ptr %"%base_y", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_297

while_exit_299:                                   ; preds = %while_cond_297
  %"%base_y.load34" = load i64, ptr %"%base_y", align 8
  %addtmp35 = add i64 %"%base_y.load34", 0
  %addtmp36 = add i64 %addtmp35, 4
  store i64 %addtmp36, ptr %"%base_y", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_304

while_exit_306:                                   ; preds = %while_cond_304
  %"%base_y.load66" = load i64, ptr %"%base_y", align 8
  %addtmp67 = add i64 %"%base_y.load66", 0
  %addtmp68 = add i64 %addtmp67, 4
  store i64 %addtmp68, ptr %"%base_y", align 8
  %arr.data69 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem70 = getelementptr i64, ptr %arr.data69, i64 0
  %arr.get71 = load i64, ptr %arr.elem70, align 8
  store i64 %arr.get71, ptr %"%\D9\84\D9\88\D9\86_\D8\B4\D9\8A\D9\81\D8\AA", align 8
  br i1 false, label %then_311, label %merge_312

while_exit_315:                                   ; preds = %while_cond_313
  %"%base_y.load114" = load i64, ptr %"%base_y", align 8
  %addtmp115 = add i64 %"%base_y.load114", 1
  call void @sad_ll_fb_fill_rect(i64 85, i64 %addtmp115, i64 -2, i64 -2, i64 16777215)
  ret void
}

define i64 @"\D9\86\D9\82\D8\B1_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D8\B9\D9\85\D9\88\D8\AF74" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF" = alloca i64, align 8
  %"%base_y3" = alloca i64, align 8
  %"%base_y2" = alloca i64, align 8
  %"%base_y1" = alloca i64, align 8
  %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7" = alloca i64, align 8
  %"%base_y" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  br i1 true, label %then_320, label %merge_321

else_354:                                         ; preds = %then_350
  br label %merge_353

else_363:                                         ; preds = %then_359
  br label %merge_362

else_372:                                         ; preds = %then_368
  br label %merge_371

else_381:                                         ; preds = %then_377
  br label %merge_380

else_388:                                         ; preds = %then_384
  br label %merge_387

merge_321:                                        ; preds = %entry
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load", 0
  br i1 %cmpgetmp, label %then_322, label %merge_323

merge_323:                                        ; preds = %merge_325, %merge_321
  store i64 28, ptr %"%base_y", align 8
  store i64 -1, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load4" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"%base_y.load" = load i64, ptr %"%base_y", align 8
  %cmpgetmp5 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load4", %"%base_y.load"
  br i1 %cmpgetmp5, label %then_328, label %merge_329

merge_325:                                        ; preds = %merge_327, %then_322
  br label %merge_323

merge_327:                                        ; preds = %then_324
  br label %merge_325

merge_329:                                        ; preds = %merge_331, %merge_323
  %"%base_y.load10" = load i64, ptr %"%base_y", align 8
  %addtmp11 = add i64 %"%base_y.load10", 0
  %addtmp12 = add i64 %addtmp11, 4
  store i64 %addtmp12, ptr %"%base_y1", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load13" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"%base_y1.load" = load i64, ptr %"%base_y1", align 8
  %cmpgetmp14 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load13", %"%base_y1.load"
  br i1 %cmpgetmp14, label %then_332, label %merge_333

merge_331:                                        ; preds = %then_330, %then_328
  br label %merge_329

merge_333:                                        ; preds = %merge_335, %merge_329
  %"%base_y1.load19" = load i64, ptr %"%base_y1", align 8
  %addtmp20 = add i64 %"%base_y1.load19", 0
  %addtmp21 = add i64 %addtmp20, 4
  store i64 %addtmp21, ptr %"%base_y2", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load22" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"%base_y2.load" = load i64, ptr %"%base_y2", align 8
  %cmpgetmp23 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load22", %"%base_y2.load"
  br i1 %cmpgetmp23, label %then_336, label %merge_337

merge_335:                                        ; preds = %then_334, %then_332
  br label %merge_333

merge_337:                                        ; preds = %merge_339, %merge_333
  %"%base_y2.load28" = load i64, ptr %"%base_y2", align 8
  %addtmp29 = add i64 %"%base_y2.load28", 0
  %addtmp30 = add i64 %addtmp29, 4
  store i64 %addtmp30, ptr %"%base_y3", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load31" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"%base_y3.load" = load i64, ptr %"%base_y3", align 8
  %cmpgetmp32 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load31", %"%base_y3.load"
  br i1 %cmpgetmp32, label %then_340, label %merge_341

merge_339:                                        ; preds = %then_338, %then_336
  br label %merge_337

merge_341:                                        ; preds = %merge_343, %merge_337
  %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load" = load i64, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  %cmplttmp37 = icmp slt i64 %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load", 0
  br i1 %cmplttmp37, label %then_344, label %merge_345

merge_343:                                        ; preds = %then_342, %then_340
  br label %merge_341

merge_345:                                        ; preds = %merge_341
  %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load38" = load i64, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load38", 0
  br i1 %cmpeqtmp, label %then_346, label %merge_347

merge_347:                                        ; preds = %merge_349, %merge_345
  %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load42" = load i64, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  %cmpeqtmp43 = icmp eq i64 %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load42", 1
  br i1 %cmpeqtmp43, label %then_355, label %merge_356

merge_349:                                        ; preds = %merge_351, %then_346
  br label %merge_347

merge_351:                                        ; preds = %then_348
  br label %merge_349

merge_353:                                        ; preds = %else_354, %then_352
  ret i64 0

merge_356:                                        ; preds = %merge_358, %merge_347
  %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load52" = load i64, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  %cmpeqtmp53 = icmp eq i64 %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load52", 2
  br i1 %cmpeqtmp53, label %then_364, label %merge_365

merge_358:                                        ; preds = %merge_360, %then_355
  br label %merge_356

merge_360:                                        ; preds = %then_357
  br label %merge_358

merge_362:                                        ; preds = %else_363, %then_361
  ret i64 0

merge_365:                                        ; preds = %merge_367, %merge_356
  %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load62" = load i64, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  %cmpeqtmp63 = icmp eq i64 %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7.load62", 3
  br i1 %cmpeqtmp63, label %then_373, label %merge_374

merge_367:                                        ; preds = %merge_369, %then_364
  br label %merge_365

merge_369:                                        ; preds = %then_366
  br label %merge_367

merge_371:                                        ; preds = %else_372, %then_370
  ret i64 0

merge_374:                                        ; preds = %merge_390, %merge_365
  ret i64 -1

merge_376:                                        ; preds = %merge_378, %then_373
  %"%\D8\B3\D8\B7\D8\AD_x.load70" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %subtmp71 = sub i64 %"%\D8\B3\D8\B7\D8\AD_x.load70", 0
  %subtmp72 = sub i64 %subtmp71, 70
  %divtmp73 = sdiv i64 %subtmp72, 2
  store i64 %divtmp73, ptr %"%\D8\B9\D9\85\D9\88\D8\AF74", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load75" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF74", align 8
  %cmpgetmp76 = icmp sge i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load75", 0
  br i1 %cmpgetmp76, label %then_382, label %merge_383

merge_378:                                        ; preds = %then_375
  br label %merge_376

merge_380:                                        ; preds = %else_381, %then_379
  ret i64 -2

merge_383:                                        ; preds = %merge_385, %merge_376
  %"%\D8\B9\D9\85\D9\88\D8\AF.load79" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF74", align 8
  %cmpgetmp80 = icmp sge i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load79", 7
  br i1 %cmpgetmp80, label %then_389, label %merge_390

merge_385:                                        ; preds = %then_382
  br label %merge_383

merge_387:                                        ; preds = %else_388, %then_386
  ret i64 0

merge_390:                                        ; preds = %merge_383
  br label %merge_374

then_320:                                         ; preds = %entry
  ret i64 -1

then_322:                                         ; preds = %merge_321
  %"%\D8\B3\D8\B7\D8\AD_y.load3" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load3", 20
  br i1 %cmplttmp, label %then_324, label %merge_325

then_324:                                         ; preds = %then_322
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmpgetmp6 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load", -20
  br i1 %cmpgetmp6, label %then_326, label %merge_327

then_326:                                         ; preds = %then_324
  call void @"\D8\A3\D8\AE\D9\81_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"()
  ret i64 -1

then_328:                                         ; preds = %merge_323
  %"%base_y.load7" = load i64, ptr %"%base_y", align 8
  %addtmp = add i64 %"%base_y.load7", 0
  %"%\D8\B3\D8\B7\D8\AD_y.load8" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp9 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load8", %addtmp
  br i1 %cmplttmp9, label %then_330, label %merge_331

then_330:                                         ; preds = %then_328
  store i64 0, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  br label %merge_331

then_332:                                         ; preds = %merge_329
  %"%base_y1.load15" = load i64, ptr %"%base_y1", align 8
  %addtmp16 = add i64 %"%base_y1.load15", 0
  %"%\D8\B3\D8\B7\D8\AD_y.load17" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp18 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load17", %addtmp16
  br i1 %cmplttmp18, label %then_334, label %merge_335

then_334:                                         ; preds = %then_332
  store i64 1, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  br label %merge_335

then_336:                                         ; preds = %merge_333
  %"%base_y2.load24" = load i64, ptr %"%base_y2", align 8
  %addtmp25 = add i64 %"%base_y2.load24", 0
  %"%\D8\B3\D8\B7\D8\AD_y.load26" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp27 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load26", %addtmp25
  br i1 %cmplttmp27, label %then_338, label %merge_339

then_338:                                         ; preds = %then_336
  store i64 2, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  br label %merge_339

then_340:                                         ; preds = %merge_337
  %"%base_y3.load33" = load i64, ptr %"%base_y3", align 8
  %addtmp34 = add i64 %"%base_y3.load33", 0
  %"%\D8\B3\D8\B7\D8\AD_y.load35" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp36 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load35", %addtmp34
  br i1 %cmplttmp36, label %then_342, label %merge_343

then_342:                                         ; preds = %then_340
  store i64 3, ptr %"%\D8\B5\D9\81_\D8\B6\D8\BA\D8\B7", align 8
  br label %merge_343

then_344:                                         ; preds = %merge_341
  ret i64 -1

then_346:                                         ; preds = %merge_345
  %"%\D8\B3\D8\B7\D8\AD_x.load39" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %subtmp = sub i64 %"%\D8\B3\D8\B7\D8\AD_x.load39", 0
  %subtmp40 = sub i64 %subtmp, 10
  %divtmp = sdiv i64 %subtmp40, 2
  store i64 %divtmp, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmpgetmp41 = icmp sge i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load", 0
  br i1 %cmpgetmp41, label %then_348, label %merge_349

then_348:                                         ; preds = %then_346
  %"%\D8\B9\D9\85\D9\88\D8\AF.load44" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp45 = icmp slt i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load44", 0
  br i1 %cmplttmp45, label %then_350, label %merge_351

then_350:                                         ; preds = %then_348
  br i1 false, label %then_352, label %else_354

then_352:                                         ; preds = %then_350
  br label %merge_353

then_355:                                         ; preds = %merge_347
  %"%\D8\B3\D8\B7\D8\AD_x.load46" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %subtmp47 = sub i64 %"%\D8\B3\D8\B7\D8\AD_x.load46", 0
  %subtmp48 = sub i64 %subtmp47, 20
  %divtmp49 = sdiv i64 %subtmp48, 2
  store i64 %divtmp49, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load50" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmpgetmp51 = icmp sge i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load50", 0
  br i1 %cmpgetmp51, label %then_357, label %merge_358

then_357:                                         ; preds = %then_355
  %"%\D8\B9\D9\85\D9\88\D8\AF.load54" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp55 = icmp slt i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load54", 0
  br i1 %cmplttmp55, label %then_359, label %merge_360

then_359:                                         ; preds = %then_357
  br i1 false, label %then_361, label %else_363

then_361:                                         ; preds = %then_359
  br label %merge_362

then_364:                                         ; preds = %merge_356
  %"%\D8\B3\D8\B7\D8\AD_x.load56" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %subtmp57 = sub i64 %"%\D8\B3\D8\B7\D8\AD_x.load56", 0
  %subtmp58 = sub i64 %subtmp57, 30
  %divtmp59 = sdiv i64 %subtmp58, 2
  store i64 %divtmp59, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load60" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmpgetmp61 = icmp sge i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load60", 0
  br i1 %cmpgetmp61, label %then_366, label %merge_367

then_366:                                         ; preds = %then_364
  %"%\D8\B9\D9\85\D9\88\D8\AF.load64" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp65 = icmp slt i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load64", 12
  br i1 %cmplttmp65, label %then_368, label %merge_369

then_368:                                         ; preds = %then_366
  br i1 false, label %then_370, label %else_372

then_370:                                         ; preds = %then_368
  br label %merge_371

then_373:                                         ; preds = %merge_365
  %"%\D8\B3\D8\B7\D8\AD_x.load66" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmpgetmp67 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load66", 10
  br i1 %cmpgetmp67, label %then_375, label %merge_376

then_375:                                         ; preds = %then_373
  %"%\D8\B3\D8\B7\D8\AD_x.load68" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp69 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load68", 66
  br i1 %cmplttmp69, label %then_377, label %merge_378

then_377:                                         ; preds = %then_375
  br i1 true, label %then_379, label %else_381

then_379:                                         ; preds = %then_377
  br label %merge_380

then_382:                                         ; preds = %merge_376
  %"%\D8\B9\D9\85\D9\88\D8\AF.load77" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF74", align 8
  %cmplttmp78 = icmp slt i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load77", 7
  br i1 %cmplttmp78, label %then_384, label %merge_385

then_384:                                         ; preds = %then_382
  br i1 false, label %then_386, label %else_388

then_386:                                         ; preds = %then_384
  br label %merge_387

then_389:                                         ; preds = %merge_383
  ret i64 32
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D9\85\D8\AA\D9\82\D8\AF\D9\85"() {
entry:
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @sad_ll_fb_fill_rect(i64 4, i64 3, i64 54, i64 24, i64 %arr.get)
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 66, i64 12, i64 10, i64 16777215)
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 101, i64 20, i64 10, i64 16777215)
  call void @"ag_\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 101, i64 28, i64 10, i64 16777215)
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A9"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9_\D8\A7\D9\84\D9\86\D8\B8\D8\A7\D9\85"()
  ret void
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D9\88\D8\A7\D8\AC\D9\87\D8\A9_\D8\A7\D9\84\D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9"() {
entry:
  %calltmp = call i64 @"ag_\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\AE\D8\B7"()
  call void @"\D9\87\D9\8A\D8\A6_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A7\D8\AA"()
  %calltmp1 = call i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\AD\D8\A7\D9\81\D8\B8\D8\A9"()
  %calltmp2 = call i64 @"\D8\A3\D8\B6\D9\81_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9"(i64 83)
  %calltmp3 = call i64 @"\D8\A3\D8\B6\D9\81_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9"(i64 78)
  %calltmp4 = call i64 @"\D8\A3\D8\B6\D9\81_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9"(i64 66)
  ret i64 0
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\88\D8\A7\D8\AC\D9\87\D8\A9_\D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9_\D9\83\D8\A7\D9\85\D9\84\D8\A9"() {
entry:
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\AE\D9\84\D9\81\D9\8A\D8\A9"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\83\D9\84_\D8\A7\D9\84\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A7\D8\AA"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85_\D9\85\D8\AA\D9\82\D8\AF\D9\85"()
  call void @"\D8\AD\D8\AF\D8\AB_\D8\A7\D9\84\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1\D8\A7\D8\AA"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"()
  ret void
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\A7\D9\84\D9\88\D8\A7\D8\AC\D9\87\D8\A9_\D8\A7\D9\84\D9\85\D8\AA\D9\82\D8\AF\D9\85\D8\A9"() {
entry:
  %"%pb" = alloca i64, align 8
  %"%t1" = alloca i64, align 8
  %"%n1" = alloca i64, align 8
  %"%cb1" = alloca i64, align 8
  %"%btn1" = alloca i64, align 8
  %"%\D9\815" = alloca i64, align 8
  %"%\D9\86\D8\AC\D8\A7\D8\AD" = alloca i64, align 8
  store i64 0, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br i1 true, label %then_391, label %merge_392

merge_392:                                        ; preds = %then_391, %entry
  %calltmp = call i64 @"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AE\D9\84\D9\81\D9\8A\D8\A9"()
  %cmpeqtmp = icmp eq i64 %calltmp, 0
  br i1 %cmpeqtmp, label %then_393, label %merge_394

merge_394:                                        ; preds = %then_393, %merge_392
  %calltmp3 = call i64 @"\D8\A7\D9\84\D8\AB\D9\8A\D9\85_\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A"()
  br i1 true, label %then_395, label %merge_396

merge_396:                                        ; preds = %then_395, %merge_394
  call void @"\D9\87\D9\8A\D8\A6_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A7\D8\AA"()
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp6 = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp6, label %then_397, label %merge_398

merge_398:                                        ; preds = %then_397, %merge_396
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 3
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  %cmpeqtmp12 = icmp eq i64 %arr.get11, 0
  br i1 %cmpeqtmp12, label %then_399, label %merge_400

merge_400:                                        ; preds = %then_399, %merge_398
  %calltmp15 = call i64 @"\D8\AC\D8\AF_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B9\D9\86\D8\AF"(i64 5, i64 5)
  store i64 %calltmp15, ptr %"%\D9\815", align 8
  %"%\D9\815.load" = load i64, ptr %"%\D9\815", align 8
  %cmpeqtmp16 = icmp eq i64 %"%\D9\815.load", 0
  br i1 %cmpeqtmp16, label %then_401, label %merge_402

merge_402:                                        ; preds = %then_401, %merge_400
  %calltmp19 = call i64 @"\D8\B9\D8\A7\D9\84\D8\AC_\D9\86\D9\82\D8\B1_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9"(i64 5, i64 5)
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 0
  %arr.get22 = load i64, ptr %arr.elem21, align 8
  %cmpeqtmp23 = icmp eq i64 %arr.get22, 1
  br i1 %cmpeqtmp23, label %then_403, label %merge_404

merge_404:                                        ; preds = %then_403, %merge_402
  %"%btn1.load" = load i64, ptr %"%btn1", align 8
  %cmpgetmp = icmp sge i64 %"%btn1.load", 0
  br i1 %cmpgetmp, label %then_405, label %merge_406

merge_406:                                        ; preds = %then_405, %merge_404
  %"%cb1.load" = load i64, ptr %"%cb1", align 8
  %cmpgetmp28 = icmp sge i64 %"%cb1.load", 0
  br i1 %cmpgetmp28, label %then_407, label %merge_408

merge_408:                                        ; preds = %then_407, %merge_406
  call void @"\D8\A3\D8\B8\D9\87\D8\B1_\D9\82\D8\A7\D8\A6\D9\85\D8\A9_\D8\A7\D8\A8\D8\AF\D8\A3"()
  br i1 false, label %then_409, label %merge_410

merge_410:                                        ; preds = %then_409, %merge_408
  br i1 false, label %then_411, label %merge_412

merge_412:                                        ; preds = %then_411, %merge_410
  call void @"\D8\A3\D8\AE\D9\81_\D8\A7\D9\84\D9\82\D8\A7\D8\A6\D9\85\D8\A9"()
  br i1 true, label %then_413, label %merge_414

merge_414:                                        ; preds = %then_413, %merge_412
  %calltmp37 = call i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\AD\D8\A7\D9\81\D8\B8\D8\A9"()
  %calltmp38 = call i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A3\D8\B6\D9\81_\D8\AD\D8\B1\D9\81"(i64 72)
  %calltmp39 = call i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A3\D8\B6\D9\81_\D8\AD\D8\B1\D9\81"(i64 105)
  %calltmp40 = call i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A7\D9\84\D8\B7\D9\88\D9\84"()
  %cmpeqtmp41 = icmp eq i64 %calltmp40, 2
  br i1 %cmpeqtmp41, label %then_415, label %merge_416

merge_416:                                        ; preds = %then_415, %merge_414
  %calltmp44 = call i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A7\D9\82\D8\B1\D8\A3_\D8\AD\D8\B1\D9\81"(i64 0)
  %cmpeqtmp45 = icmp eq i64 %calltmp44, 72
  br i1 %cmpeqtmp45, label %then_417, label %merge_418

merge_418:                                        ; preds = %then_417, %merge_416
  call void @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A7\D9\85\D8\B3\D8\AD"()
  %calltmp48 = call i64 @"\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\A7\D9\84\D8\B7\D9\88\D9\84"()
  %cmpeqtmp49 = icmp eq i64 %calltmp48, 0
  br i1 %cmpeqtmp49, label %then_419, label %merge_420

merge_420:                                        ; preds = %then_419, %merge_418
  %calltmp52 = call i64 @"\D8\A3\D8\B6\D9\81_\D8\A5\D8\B4\D8\B9\D8\A7\D8\B1"(i64 73, i64 3355443)
  store i64 %calltmp52, ptr %"%n1", align 8
  %"%n1.load" = load i64, ptr %"%n1", align 8
  %cmpgetmp53 = icmp sge i64 %"%n1.load", 0
  br i1 %cmpgetmp53, label %then_421, label %merge_422

merge_422:                                        ; preds = %then_421, %merge_420
  %calltmp56 = call i64 @"\D8\A3\D8\B6\D9\81_\D8\A3\D9\8A\D9\82\D9\88\D9\86\D8\A9_\D8\B5\D9\8A\D9\86\D9\8A\D8\A9"(i64 87)
  store i64 %calltmp56, ptr %"%t1", align 8
  %"%t1.load" = load i64, ptr %"%t1", align 8
  %cmpgetmp57 = icmp sge i64 %"%t1.load", 0
  br i1 %cmpgetmp57, label %then_423, label %merge_424

merge_424:                                        ; preds = %then_423, %merge_422
  call void @"\D8\AD\D8\AF\D8\AB_\D8\A7\D9\84\D8\B3\D8\A7\D8\B9\D8\A9"()
  br i1 false, label %then_425, label %merge_426

merge_426:                                        ; preds = %then_425, %merge_424
  call void @"\D8\A3\D8\B8\D9\87\D8\B1_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"()
  br i1 false, label %then_427, label %merge_428

merge_428:                                        ; preds = %then_427, %merge_426
  call void @"\D8\A8\D8\AF\D9\84_\D9\84\D9\88\D8\AD\D8\A9_\D8\A7\D9\84\D9\85\D9\81\D8\A7\D8\AA\D9\8A\D8\AD"()
  br i1 true, label %then_429, label %merge_430

merge_430:                                        ; preds = %then_429, %merge_428
  br i1 true, label %then_431, label %merge_432

merge_432:                                        ; preds = %then_431, %merge_430
  %calltmp68 = call i64 @"\D8\A7\D9\84\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\A7\D9\84\D8\AA\D8\A7\D9\84\D9\8A\D8\A9"()
  br i1 true, label %then_433, label %merge_434

merge_434:                                        ; preds = %then_433, %merge_432
  %"%pb.load" = load i64, ptr %"%pb", align 8
  %cmpgetmp71 = icmp sge i64 %"%pb.load", 0
  br i1 %cmpgetmp71, label %then_435, label %merge_436

merge_436:                                        ; preds = %merge_438, %merge_434
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load80" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  call void @"ag_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load80")
  call void @"ag_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 22)
  ret void

merge_438:                                        ; preds = %then_437, %then_435
  br label %merge_436

then_391:                                         ; preds = %entry
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load", 1
  store i64 %addtmp, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_392

then_393:                                         ; preds = %merge_392
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load1" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp2 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load1", 1
  store i64 %addtmp2, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_394

then_395:                                         ; preds = %merge_394
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load4" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp5 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load4", 1
  store i64 %addtmp5, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_396

then_397:                                         ; preds = %merge_396
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load7" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp8 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load7", 1
  store i64 %addtmp8, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_398

then_399:                                         ; preds = %merge_398
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load13" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp14 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load13", 1
  store i64 %addtmp14, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_400

then_401:                                         ; preds = %merge_400
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load17" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp18 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load17", 1
  store i64 %addtmp18, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_402

then_403:                                         ; preds = %merge_402
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load24" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp25 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load24", 1
  store i64 %addtmp25, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_404

then_405:                                         ; preds = %merge_404
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load26" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp27 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load26", 1
  store i64 %addtmp27, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_406

then_407:                                         ; preds = %merge_406
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load29" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp30 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load29", 1
  store i64 %addtmp30, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_408

then_409:                                         ; preds = %merge_408
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load31" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp32 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load31", 1
  store i64 %addtmp32, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_410

then_411:                                         ; preds = %merge_410
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load33" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp34 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load33", 1
  store i64 %addtmp34, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_412

then_413:                                         ; preds = %merge_412
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load35" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp36 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load35", 1
  store i64 %addtmp36, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_414

then_415:                                         ; preds = %merge_414
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load42" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp43 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load42", 1
  store i64 %addtmp43, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_416

then_417:                                         ; preds = %merge_416
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load46" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp47 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load46", 1
  store i64 %addtmp47, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_418

then_419:                                         ; preds = %merge_418
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load50" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp51 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load50", 1
  store i64 %addtmp51, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_420

then_421:                                         ; preds = %merge_420
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load54" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp55 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load54", 1
  store i64 %addtmp55, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_422

then_423:                                         ; preds = %merge_422
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load58" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp59 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load58", 1
  store i64 %addtmp59, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_424

then_425:                                         ; preds = %merge_424
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load60" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp61 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load60", 1
  store i64 %addtmp61, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_426

then_427:                                         ; preds = %merge_426
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load62" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp63 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load62", 1
  store i64 %addtmp63, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_428

then_429:                                         ; preds = %merge_428
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load64" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp65 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load64", 1
  store i64 %addtmp65, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_430

then_431:                                         ; preds = %merge_430
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load66" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp67 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load66", 1
  store i64 %addtmp67, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_432

then_433:                                         ; preds = %merge_432
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load69" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp70 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load69", 1
  store i64 %addtmp70, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_434

then_435:                                         ; preds = %merge_434
  %"%pb.load72" = load i64, ptr %"%pb", align 8
  %arr.data73 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 %"%pb.load72"
  store i64 75, ptr %arr.elem74, align 8
  %"%pb.load75" = load i64, ptr %"%pb", align 8
  %arr.data76 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem77 = getelementptr i64, ptr %arr.data76, i64 %"%pb.load75"
  %arr.get78 = load i64, ptr %arr.elem77, align 8
  %cmpeqtmp79 = icmp eq i64 %arr.get78, 75
  br i1 %cmpeqtmp79, label %then_437, label %merge_438

then_437:                                         ; preds = %then_435
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load81" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp82 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load81", 1
  store i64 %addtmp82, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_438
}

define void @__sad_main() {
entry:
  %"%vkbd_row3_shift" = alloca i64, align 8
  %"%vkbd_row2_shift" = alloca i64, align 8
  %"%vkbd_row1_shift" = alloca i64, align 8
  %"%vkbd_row0_shift" = alloca i64, align 8
  %"%vkbd_row3" = alloca i64, align 8
  %"%vkbd_row2" = alloca i64, align 8
  %"%vkbd_row1" = alloca i64, align 8
  %"%vkbd_row0" = alloca i64, align 8
  %"%vkbd_\D8\A2\D8\AE\D8\B1_\D9\85\D9\81\D8\AA\D8\A7\D8\AD" = alloca i64, align 8
  %"%vkbd_shift" = alloca i64, align 8
  %"%vkbd_\D9\85\D8\B1\D8\A6\D9\8A\D8\A9" = alloca i64, align 8
  %"%VKBD_ROWS" = alloca i64, align 8
  %"%VKBD_COLS" = alloca i64, align 8
  %"%VKBD_KEY_H" = alloca i64, align 8
  %"%VKBD_KEY_W" = alloca i64, align 8
  %"%VKBD_H" = alloca i64, align 8
  %"%VKBD_W" = alloca i64, align 8
  %"%VKBD_Y" = alloca i64, align 8
  %"%VKBD_X" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\B7\D9\88\D9\84" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%CLIPBOARD_SIZE" = alloca i64, align 8
  %"%notif_\D9\84\D9\88\D9\86" = alloca i64, align 8
  %"%notif_\D8\B9\D8\AF\D8\A7\D8\AF" = alloca i64, align 8
  %"%notif_\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%notif_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%NOTIF_DURATION" = alloca i64, align 8
  %"%NOTIF_H" = alloca i64, align 8
  %"%NOTIF_W" = alloca i64, align 8
  %"%MAX_NOTIFICATIONS" = alloca i64, align 8
  %"%tray_\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%tray_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%MAX_TRAY" = alloca i64, align 8
  %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\AF\D9\82\D8\A7\D8\A6\D9\82" = alloca i64, align 8
  %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\AB\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\AA\D9\83\D8\A7\D8\AA" = alloca i64, align 8
  %"%wdg_\D9\86\D8\A7\D9\81\D8\B0\D8\A9" = alloca i64, align 8
  %"%wdg_\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%wdg_\D9\82\D9\8A\D9\85\D8\A9" = alloca i64, align 8
  %"%wdg_h" = alloca i64, align 8
  %"%wdg_w" = alloca i64, align 8
  %"%wdg_y" = alloca i64, align 8
  %"%wdg_x" = alloca i64, align 8
  %"%wdg_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%WDG_PROGRESSBAR" = alloca i64, align 8
  %"%WDG_SCROLLBAR" = alloca i64, align 8
  %"%WDG_RADIO" = alloca i64, align 8
  %"%WDG_CHECKBOX" = alloca i64, align 8
  %"%WDG_TEXTBOX" = alloca i64, align 8
  %"%WDG_LABEL" = alloca i64, align 8
  %"%WDG_BUTTON" = alloca i64, align 8
  %"%WDG_NONE" = alloca i64, align 8
  %"%MAX_WIDGETS" = alloca i64, align 8
  %"%menu_item_\D8\A3\D9\85\D8\B1" = alloca i64, align 8
  %"%menu_item_\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%menu_item_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%menu_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%menu_\D9\81\D9\88\D9\82" = alloca i64, align 8
  %"%menu_\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%menu_y" = alloca i64, align 8
  %"%menu_x" = alloca i64, align 8
  %"%menu_\D9\85\D8\B1\D8\A6\D9\8A\D8\A9" = alloca i64, align 8
  %"%MENU_SEPARATOR" = alloca i64, align 8
  %"%MENU_W" = alloca i64, align 8
  %"%MENU_ITEM_H" = alloca i64, align 8
  %"%MAX_MENU_ITEMS" = alloca i64, align 8
  %"%icon_\D8\B3\D8\AD\D8\A8_offset_y" = alloca i64, align 8
  %"%icon_\D8\B3\D8\AD\D8\A8_offset_x" = alloca i64, align 8
  %"%icon_\D8\B3\D8\AD\D8\A8_\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%icon_\D8\B3\D8\AD\D8\A8" = alloca i64, align 8
  %"%icon_\D9\85\D8\AD\D8\AF\D8\AF" = alloca i64, align 8
  %"%icon_y" = alloca i64, align 8
  %"%icon_x" = alloca i64, align 8
  %"%icon_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%ICON_APP_SETTINGS" = alloca i64, align 8
  %"%ICON_APP_BROWSER" = alloca i64, align 8
  %"%ICON_APP_TERMINAL" = alloca i64, align 8
  %"%ICON_APP_EDITOR" = alloca i64, align 8
  %"%ICON_FILE" = alloca i64, align 8
  %"%ICON_FOLDER" = alloca i64, align 8
  %"%ICON_NONE" = alloca i64, align 8
  %"%ICON_SPACING_Y" = alloca i64, align 8
  %"%ICON_SPACING_X" = alloca i64, align 8
  %"%ICON_GRID_Y" = alloca i64, align 8
  %"%ICON_GRID_X" = alloca i64, align 8
  %"%ICON_H" = alloca i64, align 8
  %"%ICON_W" = alloca i64, align 8
  %"%MAX_ICONS" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9" = alloca i64, align 8
  %"%WP_DOTS" = alloca i64, align 8
  %"%WP_HONEYCOMB" = alloca i64, align 8
  %"%WP_STRIPES" = alloca i64, align 8
  %"%WP_GRADIENT_H" = alloca i64, align 8
  %"%WP_GRADIENT_V" = alloca i64, align 8
  %"%WP_SOLID" = alloca i64, align 8
  %"%ag_font_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%theme_icon_label" = alloca i64, align 8
  %"%theme_scrollbar" = alloca i64, align 8
  %"%theme_menu_hover" = alloca i64, align 8
  %"%theme_menu_bg" = alloca i64, align 8
  %"%theme_btn_hover" = alloca i64, align 8
  %"%theme_btn" = alloca i64, align 8
  %"%theme_accent" = alloca i64, align 8
  %"%theme_\D9\86\D8\B5" = alloca i64, align 8
  %"%theme_border" = alloca i64, align 8
  %"%theme_titlebar_inactive" = alloca i64, align 8
  %"%theme_titlebar" = alloca i64, align 8
  %"%theme_win_bg" = alloca i64, align 8
  %"%theme_taskbar" = alloca i64, align 8
  %"%theme_bg" = alloca i64, align 8
  %"%LIGHT_ICON_LABEL" = alloca i64, align 8
  %"%LIGHT_SCROLLBAR" = alloca i64, align 8
  %"%LIGHT_MENU_HOVER" = alloca i64, align 8
  %"%LIGHT_MENU_BG" = alloca i64, align 8
  %"%LIGHT_BTN_HOVER" = alloca i64, align 8
  %"%LIGHT_BTN" = alloca i64, align 8
  %"%LIGHT_ACCENT" = alloca i64, align 8
  %"%LIGHT_TEXT" = alloca i64, align 8
  %"%LIGHT_BORDER" = alloca i64, align 8
  %"%LIGHT_TITLEBAR_INACTIVE" = alloca i64, align 8
  %"%LIGHT_TITLEBAR" = alloca i64, align 8
  %"%LIGHT_WIN_BG" = alloca i64, align 8
  %"%LIGHT_TASKBAR" = alloca i64, align 8
  %"%LIGHT_BG" = alloca i64, align 8
  %"%NIGHT_ICON_LABEL" = alloca i64, align 8
  %"%NIGHT_SCROLLBAR" = alloca i64, align 8
  %"%NIGHT_MENU_HOVER" = alloca i64, align 8
  %"%NIGHT_MENU_BG" = alloca i64, align 8
  %"%NIGHT_BTN_HOVER" = alloca i64, align 8
  %"%NIGHT_BTN" = alloca i64, align 8
  %"%NIGHT_ACCENT" = alloca i64, align 8
  %"%NIGHT_TEXT" = alloca i64, align 8
  %"%NIGHT_BORDER" = alloca i64, align 8
  %"%NIGHT_TITLEBAR_INACTIVE" = alloca i64, align 8
  %"%NIGHT_TITLEBAR" = alloca i64, align 8
  %"%NIGHT_WIN_BG" = alloca i64, align 8
  %"%NIGHT_TASKBAR" = alloca i64, align 8
  %"%NIGHT_BG" = alloca i64, align 8
  %"%HONEY_ICON_LABEL" = alloca i64, align 8
  %"%HONEY_SCROLLBAR" = alloca i64, align 8
  %"%HONEY_MENU_HOVER" = alloca i64, align 8
  %"%HONEY_MENU_BG" = alloca i64, align 8
  %"%HONEY_BTN_HOVER" = alloca i64, align 8
  %"%HONEY_BTN" = alloca i64, align 8
  %"%HONEY_ACCENT" = alloca i64, align 8
  %"%HONEY_TEXT" = alloca i64, align 8
  %"%HONEY_BORDER" = alloca i64, align 8
  %"%HONEY_TITLEBAR_INACTIVE" = alloca i64, align 8
  %"%HONEY_TITLEBAR" = alloca i64, align 8
  %"%HONEY_WIN_BG" = alloca i64, align 8
  %"%HONEY_TASKBAR" = alloca i64, align 8
  %"%HONEY_BG" = alloca i64, align 8
  %"%\D8\A7\D9\84\D8\AB\D9\8A\D9\85_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A" = alloca i64, align 8
  %"%THEME_LIGHT" = alloca i64, align 8
  %"%THEME_NIGHT" = alloca i64, align 8
  %"%THEME_HONEY" = alloca i64, align 8
  %"%AG_TASKBAR_Y" = alloca i64, align 8
  %"%AG_TASKBAR_H" = alloca i64, align 8
  %"%AG_SCREEN_H" = alloca i64, align 8
  %"%AG_SCREEN_W" = alloca i64, align 8
  %"%AG_COM1" = alloca i64, align 8
  store i64 1016, ptr %"%AG_COM1", align 8
  store i64 800, ptr %"%AG_SCREEN_W", align 8
  store i64 600, ptr %"%AG_SCREEN_H", align 8
  store i64 30, ptr %"%AG_TASKBAR_H", align 8
  store i64 570, ptr %"%AG_TASKBAR_Y", align 8
  store i64 0, ptr %"%THEME_HONEY", align 8
  store i64 1, ptr %"%THEME_NIGHT", align 8
  store i64 2, ptr %"%THEME_LIGHT", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D8\AB\D9\8A\D9\85_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A", align 8
  store i64 3368601, ptr %"%HONEY_BG", align 8
  store i64 3355443, ptr %"%HONEY_TASKBAR", align 8
  store i64 16775920, ptr %"%HONEY_WIN_BG", align 8
  store i64 16755200, ptr %"%HONEY_TITLEBAR", align 8
  store i64 10066329, ptr %"%HONEY_TITLEBAR_INACTIVE", align 8
  store i64 8930560, ptr %"%HONEY_BORDER", align 8
  store i64 3355443, ptr %"%HONEY_TEXT", align 8
  store i64 16755200, ptr %"%HONEY_ACCENT", align 8
  store i64 14329600, ptr %"%HONEY_BTN", align 8
  store i64 16766720, ptr %"%HONEY_BTN_HOVER", align 8
  store i64 16777200, ptr %"%HONEY_MENU_BG", align 8
  store i64 16766720, ptr %"%HONEY_MENU_HOVER", align 8
  store i64 13421772, ptr %"%HONEY_SCROLLBAR", align 8
  store i64 16777215, ptr %"%HONEY_ICON_LABEL", align 8
  store i64 1710618, ptr %"%NIGHT_BG", align 8
  store i64 1118481, ptr %"%NIGHT_TASKBAR", align 8
  store i64 2631720, ptr %"%NIGHT_WIN_BG", align 8
  store i64 4473924, ptr %"%NIGHT_TITLEBAR", align 8
  store i64 3289650, ptr %"%NIGHT_TITLEBAR_INACTIVE", align 8
  store i64 3947580, ptr %"%NIGHT_BORDER", align 8
  store i64 14540253, ptr %"%NIGHT_TEXT", align 8
  store i64 6737151, ptr %"%NIGHT_ACCENT", align 8
  store i64 3947580, ptr %"%NIGHT_BTN", align 8
  store i64 5263440, ptr %"%NIGHT_BTN_HOVER", align 8
  store i64 2631720, ptr %"%NIGHT_MENU_BG", align 8
  store i64 4473924, ptr %"%NIGHT_MENU_HOVER", align 8
  store i64 4473924, ptr %"%NIGHT_SCROLLBAR", align 8
  store i64 14540253, ptr %"%NIGHT_ICON_LABEL", align 8
  store i64 15132390, ptr %"%LIGHT_BG", align 8
  store i64 14474460, ptr %"%LIGHT_TASKBAR", align 8
  store i64 16777215, ptr %"%LIGHT_WIN_BG", align 8
  store i64 3381708, ptr %"%LIGHT_TITLEBAR", align 8
  store i64 12632256, ptr %"%LIGHT_TITLEBAR_INACTIVE", align 8
  store i64 10526880, ptr %"%LIGHT_BORDER", align 8
  store i64 2236962, ptr %"%LIGHT_TEXT", align 8
  store i64 3381708, ptr %"%LIGHT_ACCENT", align 8
  store i64 14474460, ptr %"%LIGHT_BTN", align 8
  store i64 12632256, ptr %"%LIGHT_BTN_HOVER", align 8
  store i64 16777215, ptr %"%LIGHT_MENU_BG", align 8
  store i64 12246783, ptr %"%LIGHT_MENU_HOVER", align 8
  store i64 12632256, ptr %"%LIGHT_SCROLLBAR", align 8
  store i64 0, ptr %"%LIGHT_ICON_LABEL", align 8
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
  store i64 3368601, ptr %arr.elem, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 1
  store i64 1710618, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 15132390, ptr %arr.elem8, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%theme_bg", align 8
  %arr_new9 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep10 = getelementptr inbounds %SadArray, ptr %arr_new9, i32 0, i32 0
  store i64 3, ptr %arr.len.gep10, align 8
  %arr.cap.gep11 = getelementptr inbounds %SadArray, ptr %arr_new9, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep11, align 8
  %arr.data12 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep13 = getelementptr inbounds %SadArray, ptr %arr_new9, i32 0, i32 2
  store ptr %arr.data12, ptr %arr.data.gep13, align 8
  %arr.data.gep14 = getelementptr inbounds %SadArray, ptr %arr_new9, i32 0, i32 2
  %arr.data15 = load ptr, ptr %arr.data.gep14, align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 0
  store i64 3355443, ptr %arr.elem16, align 8
  %arr.data.gep17 = getelementptr inbounds %SadArray, ptr %arr_new9, i32 0, i32 2
  %arr.data18 = load ptr, ptr %arr.data.gep17, align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 1
  store i64 1118481, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new9, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 2
  store i64 14474460, ptr %arr.elem22, align 8
  %obj.ptrtoint23 = ptrtoint ptr %arr_new9 to i64
  store i64 %obj.ptrtoint23, ptr %"%theme_taskbar", align 8
  %arr_new24 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep25 = getelementptr inbounds %SadArray, ptr %arr_new24, i32 0, i32 0
  store i64 3, ptr %arr.len.gep25, align 8
  %arr.cap.gep26 = getelementptr inbounds %SadArray, ptr %arr_new24, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep26, align 8
  %arr.data27 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep28 = getelementptr inbounds %SadArray, ptr %arr_new24, i32 0, i32 2
  store ptr %arr.data27, ptr %arr.data.gep28, align 8
  %arr.data.gep29 = getelementptr inbounds %SadArray, ptr %arr_new24, i32 0, i32 2
  %arr.data30 = load ptr, ptr %arr.data.gep29, align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 0
  store i64 16775920, ptr %arr.elem31, align 8
  %arr.data.gep32 = getelementptr inbounds %SadArray, ptr %arr_new24, i32 0, i32 2
  %arr.data33 = load ptr, ptr %arr.data.gep32, align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 1
  store i64 2631720, ptr %arr.elem34, align 8
  %arr.data.gep35 = getelementptr inbounds %SadArray, ptr %arr_new24, i32 0, i32 2
  %arr.data36 = load ptr, ptr %arr.data.gep35, align 8
  %arr.elem37 = getelementptr i64, ptr %arr.data36, i64 2
  store i64 16777215, ptr %arr.elem37, align 8
  %obj.ptrtoint38 = ptrtoint ptr %arr_new24 to i64
  store i64 %obj.ptrtoint38, ptr %"%theme_win_bg", align 8
  %arr_new39 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep40 = getelementptr inbounds %SadArray, ptr %arr_new39, i32 0, i32 0
  store i64 3, ptr %arr.len.gep40, align 8
  %arr.cap.gep41 = getelementptr inbounds %SadArray, ptr %arr_new39, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep41, align 8
  %arr.data42 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep43 = getelementptr inbounds %SadArray, ptr %arr_new39, i32 0, i32 2
  store ptr %arr.data42, ptr %arr.data.gep43, align 8
  %arr.data.gep44 = getelementptr inbounds %SadArray, ptr %arr_new39, i32 0, i32 2
  %arr.data45 = load ptr, ptr %arr.data.gep44, align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 0
  store i64 16755200, ptr %arr.elem46, align 8
  %arr.data.gep47 = getelementptr inbounds %SadArray, ptr %arr_new39, i32 0, i32 2
  %arr.data48 = load ptr, ptr %arr.data.gep47, align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 1
  store i64 4473924, ptr %arr.elem49, align 8
  %arr.data.gep50 = getelementptr inbounds %SadArray, ptr %arr_new39, i32 0, i32 2
  %arr.data51 = load ptr, ptr %arr.data.gep50, align 8
  %arr.elem52 = getelementptr i64, ptr %arr.data51, i64 2
  store i64 3381708, ptr %arr.elem52, align 8
  %obj.ptrtoint53 = ptrtoint ptr %arr_new39 to i64
  store i64 %obj.ptrtoint53, ptr %"%theme_titlebar", align 8
  %arr_new54 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep55 = getelementptr inbounds %SadArray, ptr %arr_new54, i32 0, i32 0
  store i64 3, ptr %arr.len.gep55, align 8
  %arr.cap.gep56 = getelementptr inbounds %SadArray, ptr %arr_new54, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep56, align 8
  %arr.data57 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep58 = getelementptr inbounds %SadArray, ptr %arr_new54, i32 0, i32 2
  store ptr %arr.data57, ptr %arr.data.gep58, align 8
  %arr.data.gep59 = getelementptr inbounds %SadArray, ptr %arr_new54, i32 0, i32 2
  %arr.data60 = load ptr, ptr %arr.data.gep59, align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 0
  store i64 10066329, ptr %arr.elem61, align 8
  %arr.data.gep62 = getelementptr inbounds %SadArray, ptr %arr_new54, i32 0, i32 2
  %arr.data63 = load ptr, ptr %arr.data.gep62, align 8
  %arr.elem64 = getelementptr i64, ptr %arr.data63, i64 1
  store i64 3289650, ptr %arr.elem64, align 8
  %arr.data.gep65 = getelementptr inbounds %SadArray, ptr %arr_new54, i32 0, i32 2
  %arr.data66 = load ptr, ptr %arr.data.gep65, align 8
  %arr.elem67 = getelementptr i64, ptr %arr.data66, i64 2
  store i64 12632256, ptr %arr.elem67, align 8
  %obj.ptrtoint68 = ptrtoint ptr %arr_new54 to i64
  store i64 %obj.ptrtoint68, ptr %"%theme_titlebar_inactive", align 8
  %arr_new69 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep70 = getelementptr inbounds %SadArray, ptr %arr_new69, i32 0, i32 0
  store i64 3, ptr %arr.len.gep70, align 8
  %arr.cap.gep71 = getelementptr inbounds %SadArray, ptr %arr_new69, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep71, align 8
  %arr.data72 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep73 = getelementptr inbounds %SadArray, ptr %arr_new69, i32 0, i32 2
  store ptr %arr.data72, ptr %arr.data.gep73, align 8
  %arr.data.gep74 = getelementptr inbounds %SadArray, ptr %arr_new69, i32 0, i32 2
  %arr.data75 = load ptr, ptr %arr.data.gep74, align 8
  %arr.elem76 = getelementptr i64, ptr %arr.data75, i64 0
  store i64 8930560, ptr %arr.elem76, align 8
  %arr.data.gep77 = getelementptr inbounds %SadArray, ptr %arr_new69, i32 0, i32 2
  %arr.data78 = load ptr, ptr %arr.data.gep77, align 8
  %arr.elem79 = getelementptr i64, ptr %arr.data78, i64 1
  store i64 3947580, ptr %arr.elem79, align 8
  %arr.data.gep80 = getelementptr inbounds %SadArray, ptr %arr_new69, i32 0, i32 2
  %arr.data81 = load ptr, ptr %arr.data.gep80, align 8
  %arr.elem82 = getelementptr i64, ptr %arr.data81, i64 2
  store i64 10526880, ptr %arr.elem82, align 8
  %obj.ptrtoint83 = ptrtoint ptr %arr_new69 to i64
  store i64 %obj.ptrtoint83, ptr %"%theme_border", align 8
  %arr_new84 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep85 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 0
  store i64 3, ptr %arr.len.gep85, align 8
  %arr.cap.gep86 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep86, align 8
  %arr.data87 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep88 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  store ptr %arr.data87, ptr %arr.data.gep88, align 8
  %arr.data.gep89 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  %arr.data90 = load ptr, ptr %arr.data.gep89, align 8
  %arr.elem91 = getelementptr i64, ptr %arr.data90, i64 0
  store i64 3355443, ptr %arr.elem91, align 8
  %arr.data.gep92 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  %arr.data93 = load ptr, ptr %arr.data.gep92, align 8
  %arr.elem94 = getelementptr i64, ptr %arr.data93, i64 1
  store i64 14540253, ptr %arr.elem94, align 8
  %arr.data.gep95 = getelementptr inbounds %SadArray, ptr %arr_new84, i32 0, i32 2
  %arr.data96 = load ptr, ptr %arr.data.gep95, align 8
  %arr.elem97 = getelementptr i64, ptr %arr.data96, i64 2
  store i64 2236962, ptr %arr.elem97, align 8
  %obj.ptrtoint98 = ptrtoint ptr %arr_new84 to i64
  store i64 %obj.ptrtoint98, ptr %"%theme_\D9\86\D8\B5", align 8
  %arr_new99 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep100 = getelementptr inbounds %SadArray, ptr %arr_new99, i32 0, i32 0
  store i64 3, ptr %arr.len.gep100, align 8
  %arr.cap.gep101 = getelementptr inbounds %SadArray, ptr %arr_new99, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep101, align 8
  %arr.data102 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep103 = getelementptr inbounds %SadArray, ptr %arr_new99, i32 0, i32 2
  store ptr %arr.data102, ptr %arr.data.gep103, align 8
  %arr.data.gep104 = getelementptr inbounds %SadArray, ptr %arr_new99, i32 0, i32 2
  %arr.data105 = load ptr, ptr %arr.data.gep104, align 8
  %arr.elem106 = getelementptr i64, ptr %arr.data105, i64 0
  store i64 16755200, ptr %arr.elem106, align 8
  %arr.data.gep107 = getelementptr inbounds %SadArray, ptr %arr_new99, i32 0, i32 2
  %arr.data108 = load ptr, ptr %arr.data.gep107, align 8
  %arr.elem109 = getelementptr i64, ptr %arr.data108, i64 1
  store i64 6737151, ptr %arr.elem109, align 8
  %arr.data.gep110 = getelementptr inbounds %SadArray, ptr %arr_new99, i32 0, i32 2
  %arr.data111 = load ptr, ptr %arr.data.gep110, align 8
  %arr.elem112 = getelementptr i64, ptr %arr.data111, i64 2
  store i64 3381708, ptr %arr.elem112, align 8
  %obj.ptrtoint113 = ptrtoint ptr %arr_new99 to i64
  store i64 %obj.ptrtoint113, ptr %"%theme_accent", align 8
  %arr_new114 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep115 = getelementptr inbounds %SadArray, ptr %arr_new114, i32 0, i32 0
  store i64 3, ptr %arr.len.gep115, align 8
  %arr.cap.gep116 = getelementptr inbounds %SadArray, ptr %arr_new114, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep116, align 8
  %arr.data117 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep118 = getelementptr inbounds %SadArray, ptr %arr_new114, i32 0, i32 2
  store ptr %arr.data117, ptr %arr.data.gep118, align 8
  %arr.data.gep119 = getelementptr inbounds %SadArray, ptr %arr_new114, i32 0, i32 2
  %arr.data120 = load ptr, ptr %arr.data.gep119, align 8
  %arr.elem121 = getelementptr i64, ptr %arr.data120, i64 0
  store i64 14329600, ptr %arr.elem121, align 8
  %arr.data.gep122 = getelementptr inbounds %SadArray, ptr %arr_new114, i32 0, i32 2
  %arr.data123 = load ptr, ptr %arr.data.gep122, align 8
  %arr.elem124 = getelementptr i64, ptr %arr.data123, i64 1
  store i64 3947580, ptr %arr.elem124, align 8
  %arr.data.gep125 = getelementptr inbounds %SadArray, ptr %arr_new114, i32 0, i32 2
  %arr.data126 = load ptr, ptr %arr.data.gep125, align 8
  %arr.elem127 = getelementptr i64, ptr %arr.data126, i64 2
  store i64 14474460, ptr %arr.elem127, align 8
  %obj.ptrtoint128 = ptrtoint ptr %arr_new114 to i64
  store i64 %obj.ptrtoint128, ptr %"%theme_btn", align 8
  %arr_new129 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep130 = getelementptr inbounds %SadArray, ptr %arr_new129, i32 0, i32 0
  store i64 3, ptr %arr.len.gep130, align 8
  %arr.cap.gep131 = getelementptr inbounds %SadArray, ptr %arr_new129, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep131, align 8
  %arr.data132 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep133 = getelementptr inbounds %SadArray, ptr %arr_new129, i32 0, i32 2
  store ptr %arr.data132, ptr %arr.data.gep133, align 8
  %arr.data.gep134 = getelementptr inbounds %SadArray, ptr %arr_new129, i32 0, i32 2
  %arr.data135 = load ptr, ptr %arr.data.gep134, align 8
  %arr.elem136 = getelementptr i64, ptr %arr.data135, i64 0
  store i64 16766720, ptr %arr.elem136, align 8
  %arr.data.gep137 = getelementptr inbounds %SadArray, ptr %arr_new129, i32 0, i32 2
  %arr.data138 = load ptr, ptr %arr.data.gep137, align 8
  %arr.elem139 = getelementptr i64, ptr %arr.data138, i64 1
  store i64 5263440, ptr %arr.elem139, align 8
  %arr.data.gep140 = getelementptr inbounds %SadArray, ptr %arr_new129, i32 0, i32 2
  %arr.data141 = load ptr, ptr %arr.data.gep140, align 8
  %arr.elem142 = getelementptr i64, ptr %arr.data141, i64 2
  store i64 12632256, ptr %arr.elem142, align 8
  %obj.ptrtoint143 = ptrtoint ptr %arr_new129 to i64
  store i64 %obj.ptrtoint143, ptr %"%theme_btn_hover", align 8
  %arr_new144 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep145 = getelementptr inbounds %SadArray, ptr %arr_new144, i32 0, i32 0
  store i64 3, ptr %arr.len.gep145, align 8
  %arr.cap.gep146 = getelementptr inbounds %SadArray, ptr %arr_new144, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep146, align 8
  %arr.data147 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep148 = getelementptr inbounds %SadArray, ptr %arr_new144, i32 0, i32 2
  store ptr %arr.data147, ptr %arr.data.gep148, align 8
  %arr.data.gep149 = getelementptr inbounds %SadArray, ptr %arr_new144, i32 0, i32 2
  %arr.data150 = load ptr, ptr %arr.data.gep149, align 8
  %arr.elem151 = getelementptr i64, ptr %arr.data150, i64 0
  store i64 16777200, ptr %arr.elem151, align 8
  %arr.data.gep152 = getelementptr inbounds %SadArray, ptr %arr_new144, i32 0, i32 2
  %arr.data153 = load ptr, ptr %arr.data.gep152, align 8
  %arr.elem154 = getelementptr i64, ptr %arr.data153, i64 1
  store i64 2631720, ptr %arr.elem154, align 8
  %arr.data.gep155 = getelementptr inbounds %SadArray, ptr %arr_new144, i32 0, i32 2
  %arr.data156 = load ptr, ptr %arr.data.gep155, align 8
  %arr.elem157 = getelementptr i64, ptr %arr.data156, i64 2
  store i64 16777215, ptr %arr.elem157, align 8
  %obj.ptrtoint158 = ptrtoint ptr %arr_new144 to i64
  store i64 %obj.ptrtoint158, ptr %"%theme_menu_bg", align 8
  %arr_new159 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep160 = getelementptr inbounds %SadArray, ptr %arr_new159, i32 0, i32 0
  store i64 3, ptr %arr.len.gep160, align 8
  %arr.cap.gep161 = getelementptr inbounds %SadArray, ptr %arr_new159, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep161, align 8
  %arr.data162 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep163 = getelementptr inbounds %SadArray, ptr %arr_new159, i32 0, i32 2
  store ptr %arr.data162, ptr %arr.data.gep163, align 8
  %arr.data.gep164 = getelementptr inbounds %SadArray, ptr %arr_new159, i32 0, i32 2
  %arr.data165 = load ptr, ptr %arr.data.gep164, align 8
  %arr.elem166 = getelementptr i64, ptr %arr.data165, i64 0
  store i64 16766720, ptr %arr.elem166, align 8
  %arr.data.gep167 = getelementptr inbounds %SadArray, ptr %arr_new159, i32 0, i32 2
  %arr.data168 = load ptr, ptr %arr.data.gep167, align 8
  %arr.elem169 = getelementptr i64, ptr %arr.data168, i64 1
  store i64 4473924, ptr %arr.elem169, align 8
  %arr.data.gep170 = getelementptr inbounds %SadArray, ptr %arr_new159, i32 0, i32 2
  %arr.data171 = load ptr, ptr %arr.data.gep170, align 8
  %arr.elem172 = getelementptr i64, ptr %arr.data171, i64 2
  store i64 12246783, ptr %arr.elem172, align 8
  %obj.ptrtoint173 = ptrtoint ptr %arr_new159 to i64
  store i64 %obj.ptrtoint173, ptr %"%theme_menu_hover", align 8
  %arr_new174 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep175 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 0
  store i64 3, ptr %arr.len.gep175, align 8
  %arr.cap.gep176 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep176, align 8
  %arr.data177 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep178 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  store ptr %arr.data177, ptr %arr.data.gep178, align 8
  %arr.data.gep179 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data180 = load ptr, ptr %arr.data.gep179, align 8
  %arr.elem181 = getelementptr i64, ptr %arr.data180, i64 0
  store i64 13421772, ptr %arr.elem181, align 8
  %arr.data.gep182 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data183 = load ptr, ptr %arr.data.gep182, align 8
  %arr.elem184 = getelementptr i64, ptr %arr.data183, i64 1
  store i64 4473924, ptr %arr.elem184, align 8
  %arr.data.gep185 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data186 = load ptr, ptr %arr.data.gep185, align 8
  %arr.elem187 = getelementptr i64, ptr %arr.data186, i64 2
  store i64 12632256, ptr %arr.elem187, align 8
  %obj.ptrtoint188 = ptrtoint ptr %arr_new174 to i64
  store i64 %obj.ptrtoint188, ptr %"%theme_scrollbar", align 8
  %arr_new189 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep190 = getelementptr inbounds %SadArray, ptr %arr_new189, i32 0, i32 0
  store i64 3, ptr %arr.len.gep190, align 8
  %arr.cap.gep191 = getelementptr inbounds %SadArray, ptr %arr_new189, i32 0, i32 1
  store i64 3, ptr %arr.cap.gep191, align 8
  %arr.data192 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 3))
  %arr.data.gep193 = getelementptr inbounds %SadArray, ptr %arr_new189, i32 0, i32 2
  store ptr %arr.data192, ptr %arr.data.gep193, align 8
  %arr.data.gep194 = getelementptr inbounds %SadArray, ptr %arr_new189, i32 0, i32 2
  %arr.data195 = load ptr, ptr %arr.data.gep194, align 8
  %arr.elem196 = getelementptr i64, ptr %arr.data195, i64 0
  store i64 16777215, ptr %arr.elem196, align 8
  %arr.data.gep197 = getelementptr inbounds %SadArray, ptr %arr_new189, i32 0, i32 2
  %arr.data198 = load ptr, ptr %arr.data.gep197, align 8
  %arr.elem199 = getelementptr i64, ptr %arr.data198, i64 1
  store i64 14540253, ptr %arr.elem199, align 8
  %arr.data.gep200 = getelementptr inbounds %SadArray, ptr %arr_new189, i32 0, i32 2
  %arr.data201 = load ptr, ptr %arr.data.gep200, align 8
  %arr.elem202 = getelementptr i64, ptr %arr.data201, i64 2
  store i64 0, ptr %arr.elem202, align 8
  %obj.ptrtoint203 = ptrtoint ptr %arr_new189 to i64
  store i64 %obj.ptrtoint203, ptr %"%theme_icon_label", align 8
  store i64 0, ptr %"%ag_font_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  store i64 0, ptr %"%WP_SOLID", align 8
  store i64 1, ptr %"%WP_GRADIENT_V", align 8
  store i64 2, ptr %"%WP_GRADIENT_H", align 8
  store i64 3, ptr %"%WP_STRIPES", align 8
  store i64 4, ptr %"%WP_HONEYCOMB", align 8
  store i64 5, ptr %"%WP_DOTS", align 8
  store i64 0, ptr %"%\D8\A7\D9\84\D8\AE\D9\84\D9\81\D9\8A\D8\A9_\D8\A7\D9\84\D8\AD\D8\A7\D9\84\D9\8A\D8\A9", align 8
  store i64 24, ptr %"%MAX_ICONS", align 8
  store i64 48, ptr %"%ICON_W", align 8
  store i64 56, ptr %"%ICON_H", align 8
  store i64 20, ptr %"%ICON_GRID_X", align 8
  store i64 20, ptr %"%ICON_GRID_Y", align 8
  store i64 80, ptr %"%ICON_SPACING_X", align 8
  store i64 80, ptr %"%ICON_SPACING_Y", align 8
  store i64 0, ptr %"%ICON_NONE", align 8
  store i64 1, ptr %"%ICON_FOLDER", align 8
  store i64 2, ptr %"%ICON_FILE", align 8
  store i64 3, ptr %"%ICON_APP_EDITOR", align 8
  store i64 4, ptr %"%ICON_APP_TERMINAL", align 8
  store i64 5, ptr %"%ICON_APP_BROWSER", align 8
  store i64 6, ptr %"%ICON_APP_SETTINGS", align 8
  %arr_new204 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep205 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 0
  store i64 24, ptr %arr.len.gep205, align 8
  %arr.cap.gep206 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 1
  store i64 24, ptr %arr.cap.gep206, align 8
  %arr.data207 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 24))
  %arr.data.gep208 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  store ptr %arr.data207, ptr %arr.data.gep208, align 8
  %arr.data.gep209 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data210 = load ptr, ptr %arr.data.gep209, align 8
  %arr.elem211 = getelementptr i64, ptr %arr.data210, i64 0
  store i64 0, ptr %arr.elem211, align 8
  %arr.data.gep212 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data213 = load ptr, ptr %arr.data.gep212, align 8
  %arr.elem214 = getelementptr i64, ptr %arr.data213, i64 1
  store i64 0, ptr %arr.elem214, align 8
  %arr.data.gep215 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data216 = load ptr, ptr %arr.data.gep215, align 8
  %arr.elem217 = getelementptr i64, ptr %arr.data216, i64 2
  store i64 0, ptr %arr.elem217, align 8
  %arr.data.gep218 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data219 = load ptr, ptr %arr.data.gep218, align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 3
  store i64 0, ptr %arr.elem220, align 8
  %arr.data.gep221 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data222 = load ptr, ptr %arr.data.gep221, align 8
  %arr.elem223 = getelementptr i64, ptr %arr.data222, i64 4
  store i64 0, ptr %arr.elem223, align 8
  %arr.data.gep224 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data225 = load ptr, ptr %arr.data.gep224, align 8
  %arr.elem226 = getelementptr i64, ptr %arr.data225, i64 5
  store i64 0, ptr %arr.elem226, align 8
  %arr.data.gep227 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data228 = load ptr, ptr %arr.data.gep227, align 8
  %arr.elem229 = getelementptr i64, ptr %arr.data228, i64 6
  store i64 0, ptr %arr.elem229, align 8
  %arr.data.gep230 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data231 = load ptr, ptr %arr.data.gep230, align 8
  %arr.elem232 = getelementptr i64, ptr %arr.data231, i64 7
  store i64 0, ptr %arr.elem232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 8
  store i64 0, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 9
  store i64 0, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 10
  store i64 0, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 11
  store i64 0, ptr %arr.elem244, align 8
  %arr.data.gep245 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data246 = load ptr, ptr %arr.data.gep245, align 8
  %arr.elem247 = getelementptr i64, ptr %arr.data246, i64 12
  store i64 0, ptr %arr.elem247, align 8
  %arr.data.gep248 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data249 = load ptr, ptr %arr.data.gep248, align 8
  %arr.elem250 = getelementptr i64, ptr %arr.data249, i64 13
  store i64 0, ptr %arr.elem250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 14
  store i64 0, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 15
  store i64 0, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 16
  store i64 0, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 17
  store i64 0, ptr %arr.elem262, align 8
  %arr.data.gep263 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data264 = load ptr, ptr %arr.data.gep263, align 8
  %arr.elem265 = getelementptr i64, ptr %arr.data264, i64 18
  store i64 0, ptr %arr.elem265, align 8
  %arr.data.gep266 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data267 = load ptr, ptr %arr.data.gep266, align 8
  %arr.elem268 = getelementptr i64, ptr %arr.data267, i64 19
  store i64 0, ptr %arr.elem268, align 8
  %arr.data.gep269 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data270 = load ptr, ptr %arr.data.gep269, align 8
  %arr.elem271 = getelementptr i64, ptr %arr.data270, i64 20
  store i64 0, ptr %arr.elem271, align 8
  %arr.data.gep272 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data273 = load ptr, ptr %arr.data.gep272, align 8
  %arr.elem274 = getelementptr i64, ptr %arr.data273, i64 21
  store i64 0, ptr %arr.elem274, align 8
  %arr.data.gep275 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data276 = load ptr, ptr %arr.data.gep275, align 8
  %arr.elem277 = getelementptr i64, ptr %arr.data276, i64 22
  store i64 0, ptr %arr.elem277, align 8
  %arr.data.gep278 = getelementptr inbounds %SadArray, ptr %arr_new204, i32 0, i32 2
  %arr.data279 = load ptr, ptr %arr.data.gep278, align 8
  %arr.elem280 = getelementptr i64, ptr %arr.data279, i64 23
  store i64 0, ptr %arr.elem280, align 8
  %obj.ptrtoint281 = ptrtoint ptr %arr_new204 to i64
  store i64 %obj.ptrtoint281, ptr %"%icon_\D9\86\D9\88\D8\B9", align 8
  %arr_new282 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep283 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 0
  store i64 24, ptr %arr.len.gep283, align 8
  %arr.cap.gep284 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 1
  store i64 24, ptr %arr.cap.gep284, align 8
  %arr.data285 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 24))
  %arr.data.gep286 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  store ptr %arr.data285, ptr %arr.data.gep286, align 8
  %arr.data.gep287 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data288 = load ptr, ptr %arr.data.gep287, align 8
  %arr.elem289 = getelementptr i64, ptr %arr.data288, i64 0
  store i64 0, ptr %arr.elem289, align 8
  %arr.data.gep290 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data291 = load ptr, ptr %arr.data.gep290, align 8
  %arr.elem292 = getelementptr i64, ptr %arr.data291, i64 1
  store i64 0, ptr %arr.elem292, align 8
  %arr.data.gep293 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data294 = load ptr, ptr %arr.data.gep293, align 8
  %arr.elem295 = getelementptr i64, ptr %arr.data294, i64 2
  store i64 0, ptr %arr.elem295, align 8
  %arr.data.gep296 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data297 = load ptr, ptr %arr.data.gep296, align 8
  %arr.elem298 = getelementptr i64, ptr %arr.data297, i64 3
  store i64 0, ptr %arr.elem298, align 8
  %arr.data.gep299 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data300 = load ptr, ptr %arr.data.gep299, align 8
  %arr.elem301 = getelementptr i64, ptr %arr.data300, i64 4
  store i64 0, ptr %arr.elem301, align 8
  %arr.data.gep302 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data303 = load ptr, ptr %arr.data.gep302, align 8
  %arr.elem304 = getelementptr i64, ptr %arr.data303, i64 5
  store i64 0, ptr %arr.elem304, align 8
  %arr.data.gep305 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data306 = load ptr, ptr %arr.data.gep305, align 8
  %arr.elem307 = getelementptr i64, ptr %arr.data306, i64 6
  store i64 0, ptr %arr.elem307, align 8
  %arr.data.gep308 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data309 = load ptr, ptr %arr.data.gep308, align 8
  %arr.elem310 = getelementptr i64, ptr %arr.data309, i64 7
  store i64 0, ptr %arr.elem310, align 8
  %arr.data.gep311 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data312 = load ptr, ptr %arr.data.gep311, align 8
  %arr.elem313 = getelementptr i64, ptr %arr.data312, i64 8
  store i64 0, ptr %arr.elem313, align 8
  %arr.data.gep314 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data315 = load ptr, ptr %arr.data.gep314, align 8
  %arr.elem316 = getelementptr i64, ptr %arr.data315, i64 9
  store i64 0, ptr %arr.elem316, align 8
  %arr.data.gep317 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data318 = load ptr, ptr %arr.data.gep317, align 8
  %arr.elem319 = getelementptr i64, ptr %arr.data318, i64 10
  store i64 0, ptr %arr.elem319, align 8
  %arr.data.gep320 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data321 = load ptr, ptr %arr.data.gep320, align 8
  %arr.elem322 = getelementptr i64, ptr %arr.data321, i64 11
  store i64 0, ptr %arr.elem322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 12
  store i64 0, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 13
  store i64 0, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 14
  store i64 0, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 15
  store i64 0, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 16
  store i64 0, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 17
  store i64 0, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 18
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 19
  store i64 0, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 20
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 21
  store i64 0, ptr %arr.elem352, align 8
  %arr.data.gep353 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data354 = load ptr, ptr %arr.data.gep353, align 8
  %arr.elem355 = getelementptr i64, ptr %arr.data354, i64 22
  store i64 0, ptr %arr.elem355, align 8
  %arr.data.gep356 = getelementptr inbounds %SadArray, ptr %arr_new282, i32 0, i32 2
  %arr.data357 = load ptr, ptr %arr.data.gep356, align 8
  %arr.elem358 = getelementptr i64, ptr %arr.data357, i64 23
  store i64 0, ptr %arr.elem358, align 8
  %obj.ptrtoint359 = ptrtoint ptr %arr_new282 to i64
  store i64 %obj.ptrtoint359, ptr %"%icon_x", align 8
  %arr_new360 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep361 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 0
  store i64 24, ptr %arr.len.gep361, align 8
  %arr.cap.gep362 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 1
  store i64 24, ptr %arr.cap.gep362, align 8
  %arr.data363 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 24))
  %arr.data.gep364 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  store ptr %arr.data363, ptr %arr.data.gep364, align 8
  %arr.data.gep365 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data366 = load ptr, ptr %arr.data.gep365, align 8
  %arr.elem367 = getelementptr i64, ptr %arr.data366, i64 0
  store i64 0, ptr %arr.elem367, align 8
  %arr.data.gep368 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data369 = load ptr, ptr %arr.data.gep368, align 8
  %arr.elem370 = getelementptr i64, ptr %arr.data369, i64 1
  store i64 0, ptr %arr.elem370, align 8
  %arr.data.gep371 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data372 = load ptr, ptr %arr.data.gep371, align 8
  %arr.elem373 = getelementptr i64, ptr %arr.data372, i64 2
  store i64 0, ptr %arr.elem373, align 8
  %arr.data.gep374 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data375 = load ptr, ptr %arr.data.gep374, align 8
  %arr.elem376 = getelementptr i64, ptr %arr.data375, i64 3
  store i64 0, ptr %arr.elem376, align 8
  %arr.data.gep377 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data378 = load ptr, ptr %arr.data.gep377, align 8
  %arr.elem379 = getelementptr i64, ptr %arr.data378, i64 4
  store i64 0, ptr %arr.elem379, align 8
  %arr.data.gep380 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data381 = load ptr, ptr %arr.data.gep380, align 8
  %arr.elem382 = getelementptr i64, ptr %arr.data381, i64 5
  store i64 0, ptr %arr.elem382, align 8
  %arr.data.gep383 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data384 = load ptr, ptr %arr.data.gep383, align 8
  %arr.elem385 = getelementptr i64, ptr %arr.data384, i64 6
  store i64 0, ptr %arr.elem385, align 8
  %arr.data.gep386 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data387 = load ptr, ptr %arr.data.gep386, align 8
  %arr.elem388 = getelementptr i64, ptr %arr.data387, i64 7
  store i64 0, ptr %arr.elem388, align 8
  %arr.data.gep389 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data390 = load ptr, ptr %arr.data.gep389, align 8
  %arr.elem391 = getelementptr i64, ptr %arr.data390, i64 8
  store i64 0, ptr %arr.elem391, align 8
  %arr.data.gep392 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data393 = load ptr, ptr %arr.data.gep392, align 8
  %arr.elem394 = getelementptr i64, ptr %arr.data393, i64 9
  store i64 0, ptr %arr.elem394, align 8
  %arr.data.gep395 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data396 = load ptr, ptr %arr.data.gep395, align 8
  %arr.elem397 = getelementptr i64, ptr %arr.data396, i64 10
  store i64 0, ptr %arr.elem397, align 8
  %arr.data.gep398 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data399 = load ptr, ptr %arr.data.gep398, align 8
  %arr.elem400 = getelementptr i64, ptr %arr.data399, i64 11
  store i64 0, ptr %arr.elem400, align 8
  %arr.data.gep401 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data402 = load ptr, ptr %arr.data.gep401, align 8
  %arr.elem403 = getelementptr i64, ptr %arr.data402, i64 12
  store i64 0, ptr %arr.elem403, align 8
  %arr.data.gep404 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data405 = load ptr, ptr %arr.data.gep404, align 8
  %arr.elem406 = getelementptr i64, ptr %arr.data405, i64 13
  store i64 0, ptr %arr.elem406, align 8
  %arr.data.gep407 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data408 = load ptr, ptr %arr.data.gep407, align 8
  %arr.elem409 = getelementptr i64, ptr %arr.data408, i64 14
  store i64 0, ptr %arr.elem409, align 8
  %arr.data.gep410 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data411 = load ptr, ptr %arr.data.gep410, align 8
  %arr.elem412 = getelementptr i64, ptr %arr.data411, i64 15
  store i64 0, ptr %arr.elem412, align 8
  %arr.data.gep413 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data414 = load ptr, ptr %arr.data.gep413, align 8
  %arr.elem415 = getelementptr i64, ptr %arr.data414, i64 16
  store i64 0, ptr %arr.elem415, align 8
  %arr.data.gep416 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data417 = load ptr, ptr %arr.data.gep416, align 8
  %arr.elem418 = getelementptr i64, ptr %arr.data417, i64 17
  store i64 0, ptr %arr.elem418, align 8
  %arr.data.gep419 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data420 = load ptr, ptr %arr.data.gep419, align 8
  %arr.elem421 = getelementptr i64, ptr %arr.data420, i64 18
  store i64 0, ptr %arr.elem421, align 8
  %arr.data.gep422 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data423 = load ptr, ptr %arr.data.gep422, align 8
  %arr.elem424 = getelementptr i64, ptr %arr.data423, i64 19
  store i64 0, ptr %arr.elem424, align 8
  %arr.data.gep425 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data426 = load ptr, ptr %arr.data.gep425, align 8
  %arr.elem427 = getelementptr i64, ptr %arr.data426, i64 20
  store i64 0, ptr %arr.elem427, align 8
  %arr.data.gep428 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data429 = load ptr, ptr %arr.data.gep428, align 8
  %arr.elem430 = getelementptr i64, ptr %arr.data429, i64 21
  store i64 0, ptr %arr.elem430, align 8
  %arr.data.gep431 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data432 = load ptr, ptr %arr.data.gep431, align 8
  %arr.elem433 = getelementptr i64, ptr %arr.data432, i64 22
  store i64 0, ptr %arr.elem433, align 8
  %arr.data.gep434 = getelementptr inbounds %SadArray, ptr %arr_new360, i32 0, i32 2
  %arr.data435 = load ptr, ptr %arr.data.gep434, align 8
  %arr.elem436 = getelementptr i64, ptr %arr.data435, i64 23
  store i64 0, ptr %arr.elem436, align 8
  %obj.ptrtoint437 = ptrtoint ptr %arr_new360 to i64
  store i64 %obj.ptrtoint437, ptr %"%icon_y", align 8
  %arr_new438 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep439 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 0
  store i64 24, ptr %arr.len.gep439, align 8
  %arr.cap.gep440 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 1
  store i64 24, ptr %arr.cap.gep440, align 8
  %arr.data441 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 24))
  %arr.data.gep442 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  store ptr %arr.data441, ptr %arr.data.gep442, align 8
  %arr.data.gep443 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data444 = load ptr, ptr %arr.data.gep443, align 8
  %arr.elem445 = getelementptr i64, ptr %arr.data444, i64 0
  store i64 0, ptr %arr.elem445, align 8
  %arr.data.gep446 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data447 = load ptr, ptr %arr.data.gep446, align 8
  %arr.elem448 = getelementptr i64, ptr %arr.data447, i64 1
  store i64 0, ptr %arr.elem448, align 8
  %arr.data.gep449 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data450 = load ptr, ptr %arr.data.gep449, align 8
  %arr.elem451 = getelementptr i64, ptr %arr.data450, i64 2
  store i64 0, ptr %arr.elem451, align 8
  %arr.data.gep452 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data453 = load ptr, ptr %arr.data.gep452, align 8
  %arr.elem454 = getelementptr i64, ptr %arr.data453, i64 3
  store i64 0, ptr %arr.elem454, align 8
  %arr.data.gep455 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data456 = load ptr, ptr %arr.data.gep455, align 8
  %arr.elem457 = getelementptr i64, ptr %arr.data456, i64 4
  store i64 0, ptr %arr.elem457, align 8
  %arr.data.gep458 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data459 = load ptr, ptr %arr.data.gep458, align 8
  %arr.elem460 = getelementptr i64, ptr %arr.data459, i64 5
  store i64 0, ptr %arr.elem460, align 8
  %arr.data.gep461 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data462 = load ptr, ptr %arr.data.gep461, align 8
  %arr.elem463 = getelementptr i64, ptr %arr.data462, i64 6
  store i64 0, ptr %arr.elem463, align 8
  %arr.data.gep464 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data465 = load ptr, ptr %arr.data.gep464, align 8
  %arr.elem466 = getelementptr i64, ptr %arr.data465, i64 7
  store i64 0, ptr %arr.elem466, align 8
  %arr.data.gep467 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data468 = load ptr, ptr %arr.data.gep467, align 8
  %arr.elem469 = getelementptr i64, ptr %arr.data468, i64 8
  store i64 0, ptr %arr.elem469, align 8
  %arr.data.gep470 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data471 = load ptr, ptr %arr.data.gep470, align 8
  %arr.elem472 = getelementptr i64, ptr %arr.data471, i64 9
  store i64 0, ptr %arr.elem472, align 8
  %arr.data.gep473 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data474 = load ptr, ptr %arr.data.gep473, align 8
  %arr.elem475 = getelementptr i64, ptr %arr.data474, i64 10
  store i64 0, ptr %arr.elem475, align 8
  %arr.data.gep476 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data477 = load ptr, ptr %arr.data.gep476, align 8
  %arr.elem478 = getelementptr i64, ptr %arr.data477, i64 11
  store i64 0, ptr %arr.elem478, align 8
  %arr.data.gep479 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data480 = load ptr, ptr %arr.data.gep479, align 8
  %arr.elem481 = getelementptr i64, ptr %arr.data480, i64 12
  store i64 0, ptr %arr.elem481, align 8
  %arr.data.gep482 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data483 = load ptr, ptr %arr.data.gep482, align 8
  %arr.elem484 = getelementptr i64, ptr %arr.data483, i64 13
  store i64 0, ptr %arr.elem484, align 8
  %arr.data.gep485 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data486 = load ptr, ptr %arr.data.gep485, align 8
  %arr.elem487 = getelementptr i64, ptr %arr.data486, i64 14
  store i64 0, ptr %arr.elem487, align 8
  %arr.data.gep488 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data489 = load ptr, ptr %arr.data.gep488, align 8
  %arr.elem490 = getelementptr i64, ptr %arr.data489, i64 15
  store i64 0, ptr %arr.elem490, align 8
  %arr.data.gep491 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data492 = load ptr, ptr %arr.data.gep491, align 8
  %arr.elem493 = getelementptr i64, ptr %arr.data492, i64 16
  store i64 0, ptr %arr.elem493, align 8
  %arr.data.gep494 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data495 = load ptr, ptr %arr.data.gep494, align 8
  %arr.elem496 = getelementptr i64, ptr %arr.data495, i64 17
  store i64 0, ptr %arr.elem496, align 8
  %arr.data.gep497 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data498 = load ptr, ptr %arr.data.gep497, align 8
  %arr.elem499 = getelementptr i64, ptr %arr.data498, i64 18
  store i64 0, ptr %arr.elem499, align 8
  %arr.data.gep500 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data501 = load ptr, ptr %arr.data.gep500, align 8
  %arr.elem502 = getelementptr i64, ptr %arr.data501, i64 19
  store i64 0, ptr %arr.elem502, align 8
  %arr.data.gep503 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data504 = load ptr, ptr %arr.data.gep503, align 8
  %arr.elem505 = getelementptr i64, ptr %arr.data504, i64 20
  store i64 0, ptr %arr.elem505, align 8
  %arr.data.gep506 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data507 = load ptr, ptr %arr.data.gep506, align 8
  %arr.elem508 = getelementptr i64, ptr %arr.data507, i64 21
  store i64 0, ptr %arr.elem508, align 8
  %arr.data.gep509 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data510 = load ptr, ptr %arr.data.gep509, align 8
  %arr.elem511 = getelementptr i64, ptr %arr.data510, i64 22
  store i64 0, ptr %arr.elem511, align 8
  %arr.data.gep512 = getelementptr inbounds %SadArray, ptr %arr_new438, i32 0, i32 2
  %arr.data513 = load ptr, ptr %arr.data.gep512, align 8
  %arr.elem514 = getelementptr i64, ptr %arr.data513, i64 23
  store i64 0, ptr %arr.elem514, align 8
  %obj.ptrtoint515 = ptrtoint ptr %arr_new438 to i64
  store i64 %obj.ptrtoint515, ptr %"%icon_\D9\85\D8\AD\D8\AF\D8\AF", align 8
  store i64 0, ptr %"%icon_\D8\B3\D8\AD\D8\A8", align 8
  store i64 -1, ptr %"%icon_\D8\B3\D8\AD\D8\A8_\D9\81\D9\87\D8\B1\D8\B3", align 8
  store i64 0, ptr %"%icon_\D8\B3\D8\AD\D8\A8_offset_x", align 8
  store i64 0, ptr %"%icon_\D8\B3\D8\AD\D8\A8_offset_y", align 8
  store i64 10, ptr %"%MAX_MENU_ITEMS", align 8
  store i64 24, ptr %"%MENU_ITEM_H", align 8
  store i64 160, ptr %"%MENU_W", align 8
  store i64 -1, ptr %"%MENU_SEPARATOR", align 8
  store i64 0, ptr %"%menu_\D9\85\D8\B1\D8\A6\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%menu_x", align 8
  store i64 0, ptr %"%menu_y", align 8
  store i64 0, ptr %"%menu_\D8\B9\D8\AF\D8\AF", align 8
  store i64 -1, ptr %"%menu_\D9\81\D9\88\D9\82", align 8
  store i64 0, ptr %"%menu_\D9\86\D9\88\D8\B9", align 8
  %arr_new516 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep517 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 0
  store i64 10, ptr %arr.len.gep517, align 8
  %arr.cap.gep518 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 1
  store i64 10, ptr %arr.cap.gep518, align 8
  %arr.data519 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 10))
  %arr.data.gep520 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  store ptr %arr.data519, ptr %arr.data.gep520, align 8
  %arr.data.gep521 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data522 = load ptr, ptr %arr.data.gep521, align 8
  %arr.elem523 = getelementptr i64, ptr %arr.data522, i64 0
  store i64 0, ptr %arr.elem523, align 8
  %arr.data.gep524 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data525 = load ptr, ptr %arr.data.gep524, align 8
  %arr.elem526 = getelementptr i64, ptr %arr.data525, i64 1
  store i64 0, ptr %arr.elem526, align 8
  %arr.data.gep527 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data528 = load ptr, ptr %arr.data.gep527, align 8
  %arr.elem529 = getelementptr i64, ptr %arr.data528, i64 2
  store i64 0, ptr %arr.elem529, align 8
  %arr.data.gep530 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data531 = load ptr, ptr %arr.data.gep530, align 8
  %arr.elem532 = getelementptr i64, ptr %arr.data531, i64 3
  store i64 0, ptr %arr.elem532, align 8
  %arr.data.gep533 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data534 = load ptr, ptr %arr.data.gep533, align 8
  %arr.elem535 = getelementptr i64, ptr %arr.data534, i64 4
  store i64 0, ptr %arr.elem535, align 8
  %arr.data.gep536 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data537 = load ptr, ptr %arr.data.gep536, align 8
  %arr.elem538 = getelementptr i64, ptr %arr.data537, i64 5
  store i64 0, ptr %arr.elem538, align 8
  %arr.data.gep539 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data540 = load ptr, ptr %arr.data.gep539, align 8
  %arr.elem541 = getelementptr i64, ptr %arr.data540, i64 6
  store i64 0, ptr %arr.elem541, align 8
  %arr.data.gep542 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data543 = load ptr, ptr %arr.data.gep542, align 8
  %arr.elem544 = getelementptr i64, ptr %arr.data543, i64 7
  store i64 0, ptr %arr.elem544, align 8
  %arr.data.gep545 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data546 = load ptr, ptr %arr.data.gep545, align 8
  %arr.elem547 = getelementptr i64, ptr %arr.data546, i64 8
  store i64 0, ptr %arr.elem547, align 8
  %arr.data.gep548 = getelementptr inbounds %SadArray, ptr %arr_new516, i32 0, i32 2
  %arr.data549 = load ptr, ptr %arr.data.gep548, align 8
  %arr.elem550 = getelementptr i64, ptr %arr.data549, i64 9
  store i64 0, ptr %arr.elem550, align 8
  %obj.ptrtoint551 = ptrtoint ptr %arr_new516 to i64
  store i64 %obj.ptrtoint551, ptr %"%menu_item_\D9\86\D9\88\D8\B9", align 8
  %arr_new552 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep553 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 0
  store i64 10, ptr %arr.len.gep553, align 8
  %arr.cap.gep554 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 1
  store i64 10, ptr %arr.cap.gep554, align 8
  %arr.data555 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 10))
  %arr.data.gep556 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  store ptr %arr.data555, ptr %arr.data.gep556, align 8
  %arr.data.gep557 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data558 = load ptr, ptr %arr.data.gep557, align 8
  %arr.elem559 = getelementptr i64, ptr %arr.data558, i64 0
  store i64 0, ptr %arr.elem559, align 8
  %arr.data.gep560 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data561 = load ptr, ptr %arr.data.gep560, align 8
  %arr.elem562 = getelementptr i64, ptr %arr.data561, i64 1
  store i64 0, ptr %arr.elem562, align 8
  %arr.data.gep563 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data564 = load ptr, ptr %arr.data.gep563, align 8
  %arr.elem565 = getelementptr i64, ptr %arr.data564, i64 2
  store i64 0, ptr %arr.elem565, align 8
  %arr.data.gep566 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data567 = load ptr, ptr %arr.data.gep566, align 8
  %arr.elem568 = getelementptr i64, ptr %arr.data567, i64 3
  store i64 0, ptr %arr.elem568, align 8
  %arr.data.gep569 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data570 = load ptr, ptr %arr.data.gep569, align 8
  %arr.elem571 = getelementptr i64, ptr %arr.data570, i64 4
  store i64 0, ptr %arr.elem571, align 8
  %arr.data.gep572 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data573 = load ptr, ptr %arr.data.gep572, align 8
  %arr.elem574 = getelementptr i64, ptr %arr.data573, i64 5
  store i64 0, ptr %arr.elem574, align 8
  %arr.data.gep575 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data576 = load ptr, ptr %arr.data.gep575, align 8
  %arr.elem577 = getelementptr i64, ptr %arr.data576, i64 6
  store i64 0, ptr %arr.elem577, align 8
  %arr.data.gep578 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data579 = load ptr, ptr %arr.data.gep578, align 8
  %arr.elem580 = getelementptr i64, ptr %arr.data579, i64 7
  store i64 0, ptr %arr.elem580, align 8
  %arr.data.gep581 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data582 = load ptr, ptr %arr.data.gep581, align 8
  %arr.elem583 = getelementptr i64, ptr %arr.data582, i64 8
  store i64 0, ptr %arr.elem583, align 8
  %arr.data.gep584 = getelementptr inbounds %SadArray, ptr %arr_new552, i32 0, i32 2
  %arr.data585 = load ptr, ptr %arr.data.gep584, align 8
  %arr.elem586 = getelementptr i64, ptr %arr.data585, i64 9
  store i64 0, ptr %arr.elem586, align 8
  %obj.ptrtoint587 = ptrtoint ptr %arr_new552 to i64
  store i64 %obj.ptrtoint587, ptr %"%menu_item_\D8\AD\D8\B1\D9\81", align 8
  %arr_new588 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep589 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 0
  store i64 10, ptr %arr.len.gep589, align 8
  %arr.cap.gep590 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 1
  store i64 10, ptr %arr.cap.gep590, align 8
  %arr.data591 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 10))
  %arr.data.gep592 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  store ptr %arr.data591, ptr %arr.data.gep592, align 8
  %arr.data.gep593 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data594 = load ptr, ptr %arr.data.gep593, align 8
  %arr.elem595 = getelementptr i64, ptr %arr.data594, i64 0
  store i64 0, ptr %arr.elem595, align 8
  %arr.data.gep596 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data597 = load ptr, ptr %arr.data.gep596, align 8
  %arr.elem598 = getelementptr i64, ptr %arr.data597, i64 1
  store i64 0, ptr %arr.elem598, align 8
  %arr.data.gep599 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data600 = load ptr, ptr %arr.data.gep599, align 8
  %arr.elem601 = getelementptr i64, ptr %arr.data600, i64 2
  store i64 0, ptr %arr.elem601, align 8
  %arr.data.gep602 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data603 = load ptr, ptr %arr.data.gep602, align 8
  %arr.elem604 = getelementptr i64, ptr %arr.data603, i64 3
  store i64 0, ptr %arr.elem604, align 8
  %arr.data.gep605 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data606 = load ptr, ptr %arr.data.gep605, align 8
  %arr.elem607 = getelementptr i64, ptr %arr.data606, i64 4
  store i64 0, ptr %arr.elem607, align 8
  %arr.data.gep608 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data609 = load ptr, ptr %arr.data.gep608, align 8
  %arr.elem610 = getelementptr i64, ptr %arr.data609, i64 5
  store i64 0, ptr %arr.elem610, align 8
  %arr.data.gep611 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data612 = load ptr, ptr %arr.data.gep611, align 8
  %arr.elem613 = getelementptr i64, ptr %arr.data612, i64 6
  store i64 0, ptr %arr.elem613, align 8
  %arr.data.gep614 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data615 = load ptr, ptr %arr.data.gep614, align 8
  %arr.elem616 = getelementptr i64, ptr %arr.data615, i64 7
  store i64 0, ptr %arr.elem616, align 8
  %arr.data.gep617 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data618 = load ptr, ptr %arr.data.gep617, align 8
  %arr.elem619 = getelementptr i64, ptr %arr.data618, i64 8
  store i64 0, ptr %arr.elem619, align 8
  %arr.data.gep620 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data621 = load ptr, ptr %arr.data.gep620, align 8
  %arr.elem622 = getelementptr i64, ptr %arr.data621, i64 9
  store i64 0, ptr %arr.elem622, align 8
  %obj.ptrtoint623 = ptrtoint ptr %arr_new588 to i64
  store i64 %obj.ptrtoint623, ptr %"%menu_item_\D8\A3\D9\85\D8\B1", align 8
  store i64 64, ptr %"%MAX_WIDGETS", align 8
  store i64 0, ptr %"%WDG_NONE", align 8
  store i64 1, ptr %"%WDG_BUTTON", align 8
  store i64 2, ptr %"%WDG_LABEL", align 8
  store i64 3, ptr %"%WDG_TEXTBOX", align 8
  store i64 4, ptr %"%WDG_CHECKBOX", align 8
  store i64 5, ptr %"%WDG_RADIO", align 8
  store i64 6, ptr %"%WDG_SCROLLBAR", align 8
  store i64 7, ptr %"%WDG_PROGRESSBAR", align 8
  %arr_new624 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep625 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 0
  store i64 64, ptr %arr.len.gep625, align 8
  %arr.cap.gep626 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep626, align 8
  %arr.data627 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep628 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  store ptr %arr.data627, ptr %arr.data.gep628, align 8
  %arr.data.gep629 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data630 = load ptr, ptr %arr.data.gep629, align 8
  %arr.elem631 = getelementptr i64, ptr %arr.data630, i64 0
  store i64 0, ptr %arr.elem631, align 8
  %arr.data.gep632 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data633 = load ptr, ptr %arr.data.gep632, align 8
  %arr.elem634 = getelementptr i64, ptr %arr.data633, i64 1
  store i64 0, ptr %arr.elem634, align 8
  %arr.data.gep635 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data636 = load ptr, ptr %arr.data.gep635, align 8
  %arr.elem637 = getelementptr i64, ptr %arr.data636, i64 2
  store i64 0, ptr %arr.elem637, align 8
  %arr.data.gep638 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data639 = load ptr, ptr %arr.data.gep638, align 8
  %arr.elem640 = getelementptr i64, ptr %arr.data639, i64 3
  store i64 0, ptr %arr.elem640, align 8
  %arr.data.gep641 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data642 = load ptr, ptr %arr.data.gep641, align 8
  %arr.elem643 = getelementptr i64, ptr %arr.data642, i64 4
  store i64 0, ptr %arr.elem643, align 8
  %arr.data.gep644 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data645 = load ptr, ptr %arr.data.gep644, align 8
  %arr.elem646 = getelementptr i64, ptr %arr.data645, i64 5
  store i64 0, ptr %arr.elem646, align 8
  %arr.data.gep647 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data648 = load ptr, ptr %arr.data.gep647, align 8
  %arr.elem649 = getelementptr i64, ptr %arr.data648, i64 6
  store i64 0, ptr %arr.elem649, align 8
  %arr.data.gep650 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data651 = load ptr, ptr %arr.data.gep650, align 8
  %arr.elem652 = getelementptr i64, ptr %arr.data651, i64 7
  store i64 0, ptr %arr.elem652, align 8
  %arr.data.gep653 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data654 = load ptr, ptr %arr.data.gep653, align 8
  %arr.elem655 = getelementptr i64, ptr %arr.data654, i64 8
  store i64 0, ptr %arr.elem655, align 8
  %arr.data.gep656 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data657 = load ptr, ptr %arr.data.gep656, align 8
  %arr.elem658 = getelementptr i64, ptr %arr.data657, i64 9
  store i64 0, ptr %arr.elem658, align 8
  %arr.data.gep659 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data660 = load ptr, ptr %arr.data.gep659, align 8
  %arr.elem661 = getelementptr i64, ptr %arr.data660, i64 10
  store i64 0, ptr %arr.elem661, align 8
  %arr.data.gep662 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data663 = load ptr, ptr %arr.data.gep662, align 8
  %arr.elem664 = getelementptr i64, ptr %arr.data663, i64 11
  store i64 0, ptr %arr.elem664, align 8
  %arr.data.gep665 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data666 = load ptr, ptr %arr.data.gep665, align 8
  %arr.elem667 = getelementptr i64, ptr %arr.data666, i64 12
  store i64 0, ptr %arr.elem667, align 8
  %arr.data.gep668 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data669 = load ptr, ptr %arr.data.gep668, align 8
  %arr.elem670 = getelementptr i64, ptr %arr.data669, i64 13
  store i64 0, ptr %arr.elem670, align 8
  %arr.data.gep671 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data672 = load ptr, ptr %arr.data.gep671, align 8
  %arr.elem673 = getelementptr i64, ptr %arr.data672, i64 14
  store i64 0, ptr %arr.elem673, align 8
  %arr.data.gep674 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data675 = load ptr, ptr %arr.data.gep674, align 8
  %arr.elem676 = getelementptr i64, ptr %arr.data675, i64 15
  store i64 0, ptr %arr.elem676, align 8
  %arr.data.gep677 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data678 = load ptr, ptr %arr.data.gep677, align 8
  %arr.elem679 = getelementptr i64, ptr %arr.data678, i64 16
  store i64 0, ptr %arr.elem679, align 8
  %arr.data.gep680 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data681 = load ptr, ptr %arr.data.gep680, align 8
  %arr.elem682 = getelementptr i64, ptr %arr.data681, i64 17
  store i64 0, ptr %arr.elem682, align 8
  %arr.data.gep683 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data684 = load ptr, ptr %arr.data.gep683, align 8
  %arr.elem685 = getelementptr i64, ptr %arr.data684, i64 18
  store i64 0, ptr %arr.elem685, align 8
  %arr.data.gep686 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data687 = load ptr, ptr %arr.data.gep686, align 8
  %arr.elem688 = getelementptr i64, ptr %arr.data687, i64 19
  store i64 0, ptr %arr.elem688, align 8
  %arr.data.gep689 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data690 = load ptr, ptr %arr.data.gep689, align 8
  %arr.elem691 = getelementptr i64, ptr %arr.data690, i64 20
  store i64 0, ptr %arr.elem691, align 8
  %arr.data.gep692 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data693 = load ptr, ptr %arr.data.gep692, align 8
  %arr.elem694 = getelementptr i64, ptr %arr.data693, i64 21
  store i64 0, ptr %arr.elem694, align 8
  %arr.data.gep695 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data696 = load ptr, ptr %arr.data.gep695, align 8
  %arr.elem697 = getelementptr i64, ptr %arr.data696, i64 22
  store i64 0, ptr %arr.elem697, align 8
  %arr.data.gep698 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data699 = load ptr, ptr %arr.data.gep698, align 8
  %arr.elem700 = getelementptr i64, ptr %arr.data699, i64 23
  store i64 0, ptr %arr.elem700, align 8
  %arr.data.gep701 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data702 = load ptr, ptr %arr.data.gep701, align 8
  %arr.elem703 = getelementptr i64, ptr %arr.data702, i64 24
  store i64 0, ptr %arr.elem703, align 8
  %arr.data.gep704 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data705 = load ptr, ptr %arr.data.gep704, align 8
  %arr.elem706 = getelementptr i64, ptr %arr.data705, i64 25
  store i64 0, ptr %arr.elem706, align 8
  %arr.data.gep707 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data708 = load ptr, ptr %arr.data.gep707, align 8
  %arr.elem709 = getelementptr i64, ptr %arr.data708, i64 26
  store i64 0, ptr %arr.elem709, align 8
  %arr.data.gep710 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data711 = load ptr, ptr %arr.data.gep710, align 8
  %arr.elem712 = getelementptr i64, ptr %arr.data711, i64 27
  store i64 0, ptr %arr.elem712, align 8
  %arr.data.gep713 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data714 = load ptr, ptr %arr.data.gep713, align 8
  %arr.elem715 = getelementptr i64, ptr %arr.data714, i64 28
  store i64 0, ptr %arr.elem715, align 8
  %arr.data.gep716 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data717 = load ptr, ptr %arr.data.gep716, align 8
  %arr.elem718 = getelementptr i64, ptr %arr.data717, i64 29
  store i64 0, ptr %arr.elem718, align 8
  %arr.data.gep719 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data720 = load ptr, ptr %arr.data.gep719, align 8
  %arr.elem721 = getelementptr i64, ptr %arr.data720, i64 30
  store i64 0, ptr %arr.elem721, align 8
  %arr.data.gep722 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data723 = load ptr, ptr %arr.data.gep722, align 8
  %arr.elem724 = getelementptr i64, ptr %arr.data723, i64 31
  store i64 0, ptr %arr.elem724, align 8
  %arr.data.gep725 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data726 = load ptr, ptr %arr.data.gep725, align 8
  %arr.elem727 = getelementptr i64, ptr %arr.data726, i64 32
  store i64 0, ptr %arr.elem727, align 8
  %arr.data.gep728 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data729 = load ptr, ptr %arr.data.gep728, align 8
  %arr.elem730 = getelementptr i64, ptr %arr.data729, i64 33
  store i64 0, ptr %arr.elem730, align 8
  %arr.data.gep731 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data732 = load ptr, ptr %arr.data.gep731, align 8
  %arr.elem733 = getelementptr i64, ptr %arr.data732, i64 34
  store i64 0, ptr %arr.elem733, align 8
  %arr.data.gep734 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data735 = load ptr, ptr %arr.data.gep734, align 8
  %arr.elem736 = getelementptr i64, ptr %arr.data735, i64 35
  store i64 0, ptr %arr.elem736, align 8
  %arr.data.gep737 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data738 = load ptr, ptr %arr.data.gep737, align 8
  %arr.elem739 = getelementptr i64, ptr %arr.data738, i64 36
  store i64 0, ptr %arr.elem739, align 8
  %arr.data.gep740 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data741 = load ptr, ptr %arr.data.gep740, align 8
  %arr.elem742 = getelementptr i64, ptr %arr.data741, i64 37
  store i64 0, ptr %arr.elem742, align 8
  %arr.data.gep743 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data744 = load ptr, ptr %arr.data.gep743, align 8
  %arr.elem745 = getelementptr i64, ptr %arr.data744, i64 38
  store i64 0, ptr %arr.elem745, align 8
  %arr.data.gep746 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data747 = load ptr, ptr %arr.data.gep746, align 8
  %arr.elem748 = getelementptr i64, ptr %arr.data747, i64 39
  store i64 0, ptr %arr.elem748, align 8
  %arr.data.gep749 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data750 = load ptr, ptr %arr.data.gep749, align 8
  %arr.elem751 = getelementptr i64, ptr %arr.data750, i64 40
  store i64 0, ptr %arr.elem751, align 8
  %arr.data.gep752 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data753 = load ptr, ptr %arr.data.gep752, align 8
  %arr.elem754 = getelementptr i64, ptr %arr.data753, i64 41
  store i64 0, ptr %arr.elem754, align 8
  %arr.data.gep755 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data756 = load ptr, ptr %arr.data.gep755, align 8
  %arr.elem757 = getelementptr i64, ptr %arr.data756, i64 42
  store i64 0, ptr %arr.elem757, align 8
  %arr.data.gep758 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data759 = load ptr, ptr %arr.data.gep758, align 8
  %arr.elem760 = getelementptr i64, ptr %arr.data759, i64 43
  store i64 0, ptr %arr.elem760, align 8
  %arr.data.gep761 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data762 = load ptr, ptr %arr.data.gep761, align 8
  %arr.elem763 = getelementptr i64, ptr %arr.data762, i64 44
  store i64 0, ptr %arr.elem763, align 8
  %arr.data.gep764 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data765 = load ptr, ptr %arr.data.gep764, align 8
  %arr.elem766 = getelementptr i64, ptr %arr.data765, i64 45
  store i64 0, ptr %arr.elem766, align 8
  %arr.data.gep767 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data768 = load ptr, ptr %arr.data.gep767, align 8
  %arr.elem769 = getelementptr i64, ptr %arr.data768, i64 46
  store i64 0, ptr %arr.elem769, align 8
  %arr.data.gep770 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data771 = load ptr, ptr %arr.data.gep770, align 8
  %arr.elem772 = getelementptr i64, ptr %arr.data771, i64 47
  store i64 0, ptr %arr.elem772, align 8
  %arr.data.gep773 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data774 = load ptr, ptr %arr.data.gep773, align 8
  %arr.elem775 = getelementptr i64, ptr %arr.data774, i64 48
  store i64 0, ptr %arr.elem775, align 8
  %arr.data.gep776 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data777 = load ptr, ptr %arr.data.gep776, align 8
  %arr.elem778 = getelementptr i64, ptr %arr.data777, i64 49
  store i64 0, ptr %arr.elem778, align 8
  %arr.data.gep779 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data780 = load ptr, ptr %arr.data.gep779, align 8
  %arr.elem781 = getelementptr i64, ptr %arr.data780, i64 50
  store i64 0, ptr %arr.elem781, align 8
  %arr.data.gep782 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data783 = load ptr, ptr %arr.data.gep782, align 8
  %arr.elem784 = getelementptr i64, ptr %arr.data783, i64 51
  store i64 0, ptr %arr.elem784, align 8
  %arr.data.gep785 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data786 = load ptr, ptr %arr.data.gep785, align 8
  %arr.elem787 = getelementptr i64, ptr %arr.data786, i64 52
  store i64 0, ptr %arr.elem787, align 8
  %arr.data.gep788 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data789 = load ptr, ptr %arr.data.gep788, align 8
  %arr.elem790 = getelementptr i64, ptr %arr.data789, i64 53
  store i64 0, ptr %arr.elem790, align 8
  %arr.data.gep791 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data792 = load ptr, ptr %arr.data.gep791, align 8
  %arr.elem793 = getelementptr i64, ptr %arr.data792, i64 54
  store i64 0, ptr %arr.elem793, align 8
  %arr.data.gep794 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data795 = load ptr, ptr %arr.data.gep794, align 8
  %arr.elem796 = getelementptr i64, ptr %arr.data795, i64 55
  store i64 0, ptr %arr.elem796, align 8
  %arr.data.gep797 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data798 = load ptr, ptr %arr.data.gep797, align 8
  %arr.elem799 = getelementptr i64, ptr %arr.data798, i64 56
  store i64 0, ptr %arr.elem799, align 8
  %arr.data.gep800 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data801 = load ptr, ptr %arr.data.gep800, align 8
  %arr.elem802 = getelementptr i64, ptr %arr.data801, i64 57
  store i64 0, ptr %arr.elem802, align 8
  %arr.data.gep803 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data804 = load ptr, ptr %arr.data.gep803, align 8
  %arr.elem805 = getelementptr i64, ptr %arr.data804, i64 58
  store i64 0, ptr %arr.elem805, align 8
  %arr.data.gep806 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data807 = load ptr, ptr %arr.data.gep806, align 8
  %arr.elem808 = getelementptr i64, ptr %arr.data807, i64 59
  store i64 0, ptr %arr.elem808, align 8
  %arr.data.gep809 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data810 = load ptr, ptr %arr.data.gep809, align 8
  %arr.elem811 = getelementptr i64, ptr %arr.data810, i64 60
  store i64 0, ptr %arr.elem811, align 8
  %arr.data.gep812 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data813 = load ptr, ptr %arr.data.gep812, align 8
  %arr.elem814 = getelementptr i64, ptr %arr.data813, i64 61
  store i64 0, ptr %arr.elem814, align 8
  %arr.data.gep815 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data816 = load ptr, ptr %arr.data.gep815, align 8
  %arr.elem817 = getelementptr i64, ptr %arr.data816, i64 62
  store i64 0, ptr %arr.elem817, align 8
  %arr.data.gep818 = getelementptr inbounds %SadArray, ptr %arr_new624, i32 0, i32 2
  %arr.data819 = load ptr, ptr %arr.data.gep818, align 8
  %arr.elem820 = getelementptr i64, ptr %arr.data819, i64 63
  store i64 0, ptr %arr.elem820, align 8
  %obj.ptrtoint821 = ptrtoint ptr %arr_new624 to i64
  store i64 %obj.ptrtoint821, ptr %"%wdg_\D9\86\D9\88\D8\B9", align 8
  %arr_new822 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep823 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 0
  store i64 64, ptr %arr.len.gep823, align 8
  %arr.cap.gep824 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep824, align 8
  %arr.data825 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep826 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  store ptr %arr.data825, ptr %arr.data.gep826, align 8
  %arr.data.gep827 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data828 = load ptr, ptr %arr.data.gep827, align 8
  %arr.elem829 = getelementptr i64, ptr %arr.data828, i64 0
  store i64 0, ptr %arr.elem829, align 8
  %arr.data.gep830 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data831 = load ptr, ptr %arr.data.gep830, align 8
  %arr.elem832 = getelementptr i64, ptr %arr.data831, i64 1
  store i64 0, ptr %arr.elem832, align 8
  %arr.data.gep833 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data834 = load ptr, ptr %arr.data.gep833, align 8
  %arr.elem835 = getelementptr i64, ptr %arr.data834, i64 2
  store i64 0, ptr %arr.elem835, align 8
  %arr.data.gep836 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data837 = load ptr, ptr %arr.data.gep836, align 8
  %arr.elem838 = getelementptr i64, ptr %arr.data837, i64 3
  store i64 0, ptr %arr.elem838, align 8
  %arr.data.gep839 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data840 = load ptr, ptr %arr.data.gep839, align 8
  %arr.elem841 = getelementptr i64, ptr %arr.data840, i64 4
  store i64 0, ptr %arr.elem841, align 8
  %arr.data.gep842 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data843 = load ptr, ptr %arr.data.gep842, align 8
  %arr.elem844 = getelementptr i64, ptr %arr.data843, i64 5
  store i64 0, ptr %arr.elem844, align 8
  %arr.data.gep845 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data846 = load ptr, ptr %arr.data.gep845, align 8
  %arr.elem847 = getelementptr i64, ptr %arr.data846, i64 6
  store i64 0, ptr %arr.elem847, align 8
  %arr.data.gep848 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data849 = load ptr, ptr %arr.data.gep848, align 8
  %arr.elem850 = getelementptr i64, ptr %arr.data849, i64 7
  store i64 0, ptr %arr.elem850, align 8
  %arr.data.gep851 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data852 = load ptr, ptr %arr.data.gep851, align 8
  %arr.elem853 = getelementptr i64, ptr %arr.data852, i64 8
  store i64 0, ptr %arr.elem853, align 8
  %arr.data.gep854 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data855 = load ptr, ptr %arr.data.gep854, align 8
  %arr.elem856 = getelementptr i64, ptr %arr.data855, i64 9
  store i64 0, ptr %arr.elem856, align 8
  %arr.data.gep857 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data858 = load ptr, ptr %arr.data.gep857, align 8
  %arr.elem859 = getelementptr i64, ptr %arr.data858, i64 10
  store i64 0, ptr %arr.elem859, align 8
  %arr.data.gep860 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data861 = load ptr, ptr %arr.data.gep860, align 8
  %arr.elem862 = getelementptr i64, ptr %arr.data861, i64 11
  store i64 0, ptr %arr.elem862, align 8
  %arr.data.gep863 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data864 = load ptr, ptr %arr.data.gep863, align 8
  %arr.elem865 = getelementptr i64, ptr %arr.data864, i64 12
  store i64 0, ptr %arr.elem865, align 8
  %arr.data.gep866 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data867 = load ptr, ptr %arr.data.gep866, align 8
  %arr.elem868 = getelementptr i64, ptr %arr.data867, i64 13
  store i64 0, ptr %arr.elem868, align 8
  %arr.data.gep869 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data870 = load ptr, ptr %arr.data.gep869, align 8
  %arr.elem871 = getelementptr i64, ptr %arr.data870, i64 14
  store i64 0, ptr %arr.elem871, align 8
  %arr.data.gep872 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data873 = load ptr, ptr %arr.data.gep872, align 8
  %arr.elem874 = getelementptr i64, ptr %arr.data873, i64 15
  store i64 0, ptr %arr.elem874, align 8
  %arr.data.gep875 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data876 = load ptr, ptr %arr.data.gep875, align 8
  %arr.elem877 = getelementptr i64, ptr %arr.data876, i64 16
  store i64 0, ptr %arr.elem877, align 8
  %arr.data.gep878 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data879 = load ptr, ptr %arr.data.gep878, align 8
  %arr.elem880 = getelementptr i64, ptr %arr.data879, i64 17
  store i64 0, ptr %arr.elem880, align 8
  %arr.data.gep881 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data882 = load ptr, ptr %arr.data.gep881, align 8
  %arr.elem883 = getelementptr i64, ptr %arr.data882, i64 18
  store i64 0, ptr %arr.elem883, align 8
  %arr.data.gep884 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data885 = load ptr, ptr %arr.data.gep884, align 8
  %arr.elem886 = getelementptr i64, ptr %arr.data885, i64 19
  store i64 0, ptr %arr.elem886, align 8
  %arr.data.gep887 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data888 = load ptr, ptr %arr.data.gep887, align 8
  %arr.elem889 = getelementptr i64, ptr %arr.data888, i64 20
  store i64 0, ptr %arr.elem889, align 8
  %arr.data.gep890 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data891 = load ptr, ptr %arr.data.gep890, align 8
  %arr.elem892 = getelementptr i64, ptr %arr.data891, i64 21
  store i64 0, ptr %arr.elem892, align 8
  %arr.data.gep893 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data894 = load ptr, ptr %arr.data.gep893, align 8
  %arr.elem895 = getelementptr i64, ptr %arr.data894, i64 22
  store i64 0, ptr %arr.elem895, align 8
  %arr.data.gep896 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data897 = load ptr, ptr %arr.data.gep896, align 8
  %arr.elem898 = getelementptr i64, ptr %arr.data897, i64 23
  store i64 0, ptr %arr.elem898, align 8
  %arr.data.gep899 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data900 = load ptr, ptr %arr.data.gep899, align 8
  %arr.elem901 = getelementptr i64, ptr %arr.data900, i64 24
  store i64 0, ptr %arr.elem901, align 8
  %arr.data.gep902 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data903 = load ptr, ptr %arr.data.gep902, align 8
  %arr.elem904 = getelementptr i64, ptr %arr.data903, i64 25
  store i64 0, ptr %arr.elem904, align 8
  %arr.data.gep905 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data906 = load ptr, ptr %arr.data.gep905, align 8
  %arr.elem907 = getelementptr i64, ptr %arr.data906, i64 26
  store i64 0, ptr %arr.elem907, align 8
  %arr.data.gep908 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data909 = load ptr, ptr %arr.data.gep908, align 8
  %arr.elem910 = getelementptr i64, ptr %arr.data909, i64 27
  store i64 0, ptr %arr.elem910, align 8
  %arr.data.gep911 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data912 = load ptr, ptr %arr.data.gep911, align 8
  %arr.elem913 = getelementptr i64, ptr %arr.data912, i64 28
  store i64 0, ptr %arr.elem913, align 8
  %arr.data.gep914 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data915 = load ptr, ptr %arr.data.gep914, align 8
  %arr.elem916 = getelementptr i64, ptr %arr.data915, i64 29
  store i64 0, ptr %arr.elem916, align 8
  %arr.data.gep917 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data918 = load ptr, ptr %arr.data.gep917, align 8
  %arr.elem919 = getelementptr i64, ptr %arr.data918, i64 30
  store i64 0, ptr %arr.elem919, align 8
  %arr.data.gep920 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data921 = load ptr, ptr %arr.data.gep920, align 8
  %arr.elem922 = getelementptr i64, ptr %arr.data921, i64 31
  store i64 0, ptr %arr.elem922, align 8
  %arr.data.gep923 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data924 = load ptr, ptr %arr.data.gep923, align 8
  %arr.elem925 = getelementptr i64, ptr %arr.data924, i64 32
  store i64 0, ptr %arr.elem925, align 8
  %arr.data.gep926 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data927 = load ptr, ptr %arr.data.gep926, align 8
  %arr.elem928 = getelementptr i64, ptr %arr.data927, i64 33
  store i64 0, ptr %arr.elem928, align 8
  %arr.data.gep929 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data930 = load ptr, ptr %arr.data.gep929, align 8
  %arr.elem931 = getelementptr i64, ptr %arr.data930, i64 34
  store i64 0, ptr %arr.elem931, align 8
  %arr.data.gep932 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data933 = load ptr, ptr %arr.data.gep932, align 8
  %arr.elem934 = getelementptr i64, ptr %arr.data933, i64 35
  store i64 0, ptr %arr.elem934, align 8
  %arr.data.gep935 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data936 = load ptr, ptr %arr.data.gep935, align 8
  %arr.elem937 = getelementptr i64, ptr %arr.data936, i64 36
  store i64 0, ptr %arr.elem937, align 8
  %arr.data.gep938 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data939 = load ptr, ptr %arr.data.gep938, align 8
  %arr.elem940 = getelementptr i64, ptr %arr.data939, i64 37
  store i64 0, ptr %arr.elem940, align 8
  %arr.data.gep941 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data942 = load ptr, ptr %arr.data.gep941, align 8
  %arr.elem943 = getelementptr i64, ptr %arr.data942, i64 38
  store i64 0, ptr %arr.elem943, align 8
  %arr.data.gep944 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data945 = load ptr, ptr %arr.data.gep944, align 8
  %arr.elem946 = getelementptr i64, ptr %arr.data945, i64 39
  store i64 0, ptr %arr.elem946, align 8
  %arr.data.gep947 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data948 = load ptr, ptr %arr.data.gep947, align 8
  %arr.elem949 = getelementptr i64, ptr %arr.data948, i64 40
  store i64 0, ptr %arr.elem949, align 8
  %arr.data.gep950 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data951 = load ptr, ptr %arr.data.gep950, align 8
  %arr.elem952 = getelementptr i64, ptr %arr.data951, i64 41
  store i64 0, ptr %arr.elem952, align 8
  %arr.data.gep953 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data954 = load ptr, ptr %arr.data.gep953, align 8
  %arr.elem955 = getelementptr i64, ptr %arr.data954, i64 42
  store i64 0, ptr %arr.elem955, align 8
  %arr.data.gep956 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data957 = load ptr, ptr %arr.data.gep956, align 8
  %arr.elem958 = getelementptr i64, ptr %arr.data957, i64 43
  store i64 0, ptr %arr.elem958, align 8
  %arr.data.gep959 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data960 = load ptr, ptr %arr.data.gep959, align 8
  %arr.elem961 = getelementptr i64, ptr %arr.data960, i64 44
  store i64 0, ptr %arr.elem961, align 8
  %arr.data.gep962 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data963 = load ptr, ptr %arr.data.gep962, align 8
  %arr.elem964 = getelementptr i64, ptr %arr.data963, i64 45
  store i64 0, ptr %arr.elem964, align 8
  %arr.data.gep965 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data966 = load ptr, ptr %arr.data.gep965, align 8
  %arr.elem967 = getelementptr i64, ptr %arr.data966, i64 46
  store i64 0, ptr %arr.elem967, align 8
  %arr.data.gep968 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data969 = load ptr, ptr %arr.data.gep968, align 8
  %arr.elem970 = getelementptr i64, ptr %arr.data969, i64 47
  store i64 0, ptr %arr.elem970, align 8
  %arr.data.gep971 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data972 = load ptr, ptr %arr.data.gep971, align 8
  %arr.elem973 = getelementptr i64, ptr %arr.data972, i64 48
  store i64 0, ptr %arr.elem973, align 8
  %arr.data.gep974 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data975 = load ptr, ptr %arr.data.gep974, align 8
  %arr.elem976 = getelementptr i64, ptr %arr.data975, i64 49
  store i64 0, ptr %arr.elem976, align 8
  %arr.data.gep977 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data978 = load ptr, ptr %arr.data.gep977, align 8
  %arr.elem979 = getelementptr i64, ptr %arr.data978, i64 50
  store i64 0, ptr %arr.elem979, align 8
  %arr.data.gep980 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data981 = load ptr, ptr %arr.data.gep980, align 8
  %arr.elem982 = getelementptr i64, ptr %arr.data981, i64 51
  store i64 0, ptr %arr.elem982, align 8
  %arr.data.gep983 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data984 = load ptr, ptr %arr.data.gep983, align 8
  %arr.elem985 = getelementptr i64, ptr %arr.data984, i64 52
  store i64 0, ptr %arr.elem985, align 8
  %arr.data.gep986 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data987 = load ptr, ptr %arr.data.gep986, align 8
  %arr.elem988 = getelementptr i64, ptr %arr.data987, i64 53
  store i64 0, ptr %arr.elem988, align 8
  %arr.data.gep989 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data990 = load ptr, ptr %arr.data.gep989, align 8
  %arr.elem991 = getelementptr i64, ptr %arr.data990, i64 54
  store i64 0, ptr %arr.elem991, align 8
  %arr.data.gep992 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data993 = load ptr, ptr %arr.data.gep992, align 8
  %arr.elem994 = getelementptr i64, ptr %arr.data993, i64 55
  store i64 0, ptr %arr.elem994, align 8
  %arr.data.gep995 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data996 = load ptr, ptr %arr.data.gep995, align 8
  %arr.elem997 = getelementptr i64, ptr %arr.data996, i64 56
  store i64 0, ptr %arr.elem997, align 8
  %arr.data.gep998 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data999 = load ptr, ptr %arr.data.gep998, align 8
  %arr.elem1000 = getelementptr i64, ptr %arr.data999, i64 57
  store i64 0, ptr %arr.elem1000, align 8
  %arr.data.gep1001 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data1002 = load ptr, ptr %arr.data.gep1001, align 8
  %arr.elem1003 = getelementptr i64, ptr %arr.data1002, i64 58
  store i64 0, ptr %arr.elem1003, align 8
  %arr.data.gep1004 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data1005 = load ptr, ptr %arr.data.gep1004, align 8
  %arr.elem1006 = getelementptr i64, ptr %arr.data1005, i64 59
  store i64 0, ptr %arr.elem1006, align 8
  %arr.data.gep1007 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data1008 = load ptr, ptr %arr.data.gep1007, align 8
  %arr.elem1009 = getelementptr i64, ptr %arr.data1008, i64 60
  store i64 0, ptr %arr.elem1009, align 8
  %arr.data.gep1010 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data1011 = load ptr, ptr %arr.data.gep1010, align 8
  %arr.elem1012 = getelementptr i64, ptr %arr.data1011, i64 61
  store i64 0, ptr %arr.elem1012, align 8
  %arr.data.gep1013 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data1014 = load ptr, ptr %arr.data.gep1013, align 8
  %arr.elem1015 = getelementptr i64, ptr %arr.data1014, i64 62
  store i64 0, ptr %arr.elem1015, align 8
  %arr.data.gep1016 = getelementptr inbounds %SadArray, ptr %arr_new822, i32 0, i32 2
  %arr.data1017 = load ptr, ptr %arr.data.gep1016, align 8
  %arr.elem1018 = getelementptr i64, ptr %arr.data1017, i64 63
  store i64 0, ptr %arr.elem1018, align 8
  %obj.ptrtoint1019 = ptrtoint ptr %arr_new822 to i64
  store i64 %obj.ptrtoint1019, ptr %"%wdg_x", align 8
  %arr_new1020 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1021 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1021, align 8
  %arr.cap.gep1022 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1022, align 8
  %arr.data1023 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1024 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  store ptr %arr.data1023, ptr %arr.data.gep1024, align 8
  %arr.data.gep1025 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1026 = load ptr, ptr %arr.data.gep1025, align 8
  %arr.elem1027 = getelementptr i64, ptr %arr.data1026, i64 0
  store i64 0, ptr %arr.elem1027, align 8
  %arr.data.gep1028 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1029 = load ptr, ptr %arr.data.gep1028, align 8
  %arr.elem1030 = getelementptr i64, ptr %arr.data1029, i64 1
  store i64 0, ptr %arr.elem1030, align 8
  %arr.data.gep1031 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1032 = load ptr, ptr %arr.data.gep1031, align 8
  %arr.elem1033 = getelementptr i64, ptr %arr.data1032, i64 2
  store i64 0, ptr %arr.elem1033, align 8
  %arr.data.gep1034 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1035 = load ptr, ptr %arr.data.gep1034, align 8
  %arr.elem1036 = getelementptr i64, ptr %arr.data1035, i64 3
  store i64 0, ptr %arr.elem1036, align 8
  %arr.data.gep1037 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1038 = load ptr, ptr %arr.data.gep1037, align 8
  %arr.elem1039 = getelementptr i64, ptr %arr.data1038, i64 4
  store i64 0, ptr %arr.elem1039, align 8
  %arr.data.gep1040 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1041 = load ptr, ptr %arr.data.gep1040, align 8
  %arr.elem1042 = getelementptr i64, ptr %arr.data1041, i64 5
  store i64 0, ptr %arr.elem1042, align 8
  %arr.data.gep1043 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1044 = load ptr, ptr %arr.data.gep1043, align 8
  %arr.elem1045 = getelementptr i64, ptr %arr.data1044, i64 6
  store i64 0, ptr %arr.elem1045, align 8
  %arr.data.gep1046 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1047 = load ptr, ptr %arr.data.gep1046, align 8
  %arr.elem1048 = getelementptr i64, ptr %arr.data1047, i64 7
  store i64 0, ptr %arr.elem1048, align 8
  %arr.data.gep1049 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1050 = load ptr, ptr %arr.data.gep1049, align 8
  %arr.elem1051 = getelementptr i64, ptr %arr.data1050, i64 8
  store i64 0, ptr %arr.elem1051, align 8
  %arr.data.gep1052 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1053 = load ptr, ptr %arr.data.gep1052, align 8
  %arr.elem1054 = getelementptr i64, ptr %arr.data1053, i64 9
  store i64 0, ptr %arr.elem1054, align 8
  %arr.data.gep1055 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1056 = load ptr, ptr %arr.data.gep1055, align 8
  %arr.elem1057 = getelementptr i64, ptr %arr.data1056, i64 10
  store i64 0, ptr %arr.elem1057, align 8
  %arr.data.gep1058 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1059 = load ptr, ptr %arr.data.gep1058, align 8
  %arr.elem1060 = getelementptr i64, ptr %arr.data1059, i64 11
  store i64 0, ptr %arr.elem1060, align 8
  %arr.data.gep1061 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1062 = load ptr, ptr %arr.data.gep1061, align 8
  %arr.elem1063 = getelementptr i64, ptr %arr.data1062, i64 12
  store i64 0, ptr %arr.elem1063, align 8
  %arr.data.gep1064 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1065 = load ptr, ptr %arr.data.gep1064, align 8
  %arr.elem1066 = getelementptr i64, ptr %arr.data1065, i64 13
  store i64 0, ptr %arr.elem1066, align 8
  %arr.data.gep1067 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1068 = load ptr, ptr %arr.data.gep1067, align 8
  %arr.elem1069 = getelementptr i64, ptr %arr.data1068, i64 14
  store i64 0, ptr %arr.elem1069, align 8
  %arr.data.gep1070 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1071 = load ptr, ptr %arr.data.gep1070, align 8
  %arr.elem1072 = getelementptr i64, ptr %arr.data1071, i64 15
  store i64 0, ptr %arr.elem1072, align 8
  %arr.data.gep1073 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1074 = load ptr, ptr %arr.data.gep1073, align 8
  %arr.elem1075 = getelementptr i64, ptr %arr.data1074, i64 16
  store i64 0, ptr %arr.elem1075, align 8
  %arr.data.gep1076 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1077 = load ptr, ptr %arr.data.gep1076, align 8
  %arr.elem1078 = getelementptr i64, ptr %arr.data1077, i64 17
  store i64 0, ptr %arr.elem1078, align 8
  %arr.data.gep1079 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1080 = load ptr, ptr %arr.data.gep1079, align 8
  %arr.elem1081 = getelementptr i64, ptr %arr.data1080, i64 18
  store i64 0, ptr %arr.elem1081, align 8
  %arr.data.gep1082 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1083 = load ptr, ptr %arr.data.gep1082, align 8
  %arr.elem1084 = getelementptr i64, ptr %arr.data1083, i64 19
  store i64 0, ptr %arr.elem1084, align 8
  %arr.data.gep1085 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1086 = load ptr, ptr %arr.data.gep1085, align 8
  %arr.elem1087 = getelementptr i64, ptr %arr.data1086, i64 20
  store i64 0, ptr %arr.elem1087, align 8
  %arr.data.gep1088 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1089 = load ptr, ptr %arr.data.gep1088, align 8
  %arr.elem1090 = getelementptr i64, ptr %arr.data1089, i64 21
  store i64 0, ptr %arr.elem1090, align 8
  %arr.data.gep1091 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1092 = load ptr, ptr %arr.data.gep1091, align 8
  %arr.elem1093 = getelementptr i64, ptr %arr.data1092, i64 22
  store i64 0, ptr %arr.elem1093, align 8
  %arr.data.gep1094 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1095 = load ptr, ptr %arr.data.gep1094, align 8
  %arr.elem1096 = getelementptr i64, ptr %arr.data1095, i64 23
  store i64 0, ptr %arr.elem1096, align 8
  %arr.data.gep1097 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1098 = load ptr, ptr %arr.data.gep1097, align 8
  %arr.elem1099 = getelementptr i64, ptr %arr.data1098, i64 24
  store i64 0, ptr %arr.elem1099, align 8
  %arr.data.gep1100 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1101 = load ptr, ptr %arr.data.gep1100, align 8
  %arr.elem1102 = getelementptr i64, ptr %arr.data1101, i64 25
  store i64 0, ptr %arr.elem1102, align 8
  %arr.data.gep1103 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1104 = load ptr, ptr %arr.data.gep1103, align 8
  %arr.elem1105 = getelementptr i64, ptr %arr.data1104, i64 26
  store i64 0, ptr %arr.elem1105, align 8
  %arr.data.gep1106 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1107 = load ptr, ptr %arr.data.gep1106, align 8
  %arr.elem1108 = getelementptr i64, ptr %arr.data1107, i64 27
  store i64 0, ptr %arr.elem1108, align 8
  %arr.data.gep1109 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1110 = load ptr, ptr %arr.data.gep1109, align 8
  %arr.elem1111 = getelementptr i64, ptr %arr.data1110, i64 28
  store i64 0, ptr %arr.elem1111, align 8
  %arr.data.gep1112 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1113 = load ptr, ptr %arr.data.gep1112, align 8
  %arr.elem1114 = getelementptr i64, ptr %arr.data1113, i64 29
  store i64 0, ptr %arr.elem1114, align 8
  %arr.data.gep1115 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1116 = load ptr, ptr %arr.data.gep1115, align 8
  %arr.elem1117 = getelementptr i64, ptr %arr.data1116, i64 30
  store i64 0, ptr %arr.elem1117, align 8
  %arr.data.gep1118 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1119 = load ptr, ptr %arr.data.gep1118, align 8
  %arr.elem1120 = getelementptr i64, ptr %arr.data1119, i64 31
  store i64 0, ptr %arr.elem1120, align 8
  %arr.data.gep1121 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1122 = load ptr, ptr %arr.data.gep1121, align 8
  %arr.elem1123 = getelementptr i64, ptr %arr.data1122, i64 32
  store i64 0, ptr %arr.elem1123, align 8
  %arr.data.gep1124 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1125 = load ptr, ptr %arr.data.gep1124, align 8
  %arr.elem1126 = getelementptr i64, ptr %arr.data1125, i64 33
  store i64 0, ptr %arr.elem1126, align 8
  %arr.data.gep1127 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1128 = load ptr, ptr %arr.data.gep1127, align 8
  %arr.elem1129 = getelementptr i64, ptr %arr.data1128, i64 34
  store i64 0, ptr %arr.elem1129, align 8
  %arr.data.gep1130 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1131 = load ptr, ptr %arr.data.gep1130, align 8
  %arr.elem1132 = getelementptr i64, ptr %arr.data1131, i64 35
  store i64 0, ptr %arr.elem1132, align 8
  %arr.data.gep1133 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1134 = load ptr, ptr %arr.data.gep1133, align 8
  %arr.elem1135 = getelementptr i64, ptr %arr.data1134, i64 36
  store i64 0, ptr %arr.elem1135, align 8
  %arr.data.gep1136 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1137 = load ptr, ptr %arr.data.gep1136, align 8
  %arr.elem1138 = getelementptr i64, ptr %arr.data1137, i64 37
  store i64 0, ptr %arr.elem1138, align 8
  %arr.data.gep1139 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1140 = load ptr, ptr %arr.data.gep1139, align 8
  %arr.elem1141 = getelementptr i64, ptr %arr.data1140, i64 38
  store i64 0, ptr %arr.elem1141, align 8
  %arr.data.gep1142 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1143 = load ptr, ptr %arr.data.gep1142, align 8
  %arr.elem1144 = getelementptr i64, ptr %arr.data1143, i64 39
  store i64 0, ptr %arr.elem1144, align 8
  %arr.data.gep1145 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1146 = load ptr, ptr %arr.data.gep1145, align 8
  %arr.elem1147 = getelementptr i64, ptr %arr.data1146, i64 40
  store i64 0, ptr %arr.elem1147, align 8
  %arr.data.gep1148 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1149 = load ptr, ptr %arr.data.gep1148, align 8
  %arr.elem1150 = getelementptr i64, ptr %arr.data1149, i64 41
  store i64 0, ptr %arr.elem1150, align 8
  %arr.data.gep1151 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1152 = load ptr, ptr %arr.data.gep1151, align 8
  %arr.elem1153 = getelementptr i64, ptr %arr.data1152, i64 42
  store i64 0, ptr %arr.elem1153, align 8
  %arr.data.gep1154 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1155 = load ptr, ptr %arr.data.gep1154, align 8
  %arr.elem1156 = getelementptr i64, ptr %arr.data1155, i64 43
  store i64 0, ptr %arr.elem1156, align 8
  %arr.data.gep1157 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1158 = load ptr, ptr %arr.data.gep1157, align 8
  %arr.elem1159 = getelementptr i64, ptr %arr.data1158, i64 44
  store i64 0, ptr %arr.elem1159, align 8
  %arr.data.gep1160 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1161 = load ptr, ptr %arr.data.gep1160, align 8
  %arr.elem1162 = getelementptr i64, ptr %arr.data1161, i64 45
  store i64 0, ptr %arr.elem1162, align 8
  %arr.data.gep1163 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1164 = load ptr, ptr %arr.data.gep1163, align 8
  %arr.elem1165 = getelementptr i64, ptr %arr.data1164, i64 46
  store i64 0, ptr %arr.elem1165, align 8
  %arr.data.gep1166 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1167 = load ptr, ptr %arr.data.gep1166, align 8
  %arr.elem1168 = getelementptr i64, ptr %arr.data1167, i64 47
  store i64 0, ptr %arr.elem1168, align 8
  %arr.data.gep1169 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1170 = load ptr, ptr %arr.data.gep1169, align 8
  %arr.elem1171 = getelementptr i64, ptr %arr.data1170, i64 48
  store i64 0, ptr %arr.elem1171, align 8
  %arr.data.gep1172 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1173 = load ptr, ptr %arr.data.gep1172, align 8
  %arr.elem1174 = getelementptr i64, ptr %arr.data1173, i64 49
  store i64 0, ptr %arr.elem1174, align 8
  %arr.data.gep1175 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1176 = load ptr, ptr %arr.data.gep1175, align 8
  %arr.elem1177 = getelementptr i64, ptr %arr.data1176, i64 50
  store i64 0, ptr %arr.elem1177, align 8
  %arr.data.gep1178 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1179 = load ptr, ptr %arr.data.gep1178, align 8
  %arr.elem1180 = getelementptr i64, ptr %arr.data1179, i64 51
  store i64 0, ptr %arr.elem1180, align 8
  %arr.data.gep1181 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1182 = load ptr, ptr %arr.data.gep1181, align 8
  %arr.elem1183 = getelementptr i64, ptr %arr.data1182, i64 52
  store i64 0, ptr %arr.elem1183, align 8
  %arr.data.gep1184 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1185 = load ptr, ptr %arr.data.gep1184, align 8
  %arr.elem1186 = getelementptr i64, ptr %arr.data1185, i64 53
  store i64 0, ptr %arr.elem1186, align 8
  %arr.data.gep1187 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1188 = load ptr, ptr %arr.data.gep1187, align 8
  %arr.elem1189 = getelementptr i64, ptr %arr.data1188, i64 54
  store i64 0, ptr %arr.elem1189, align 8
  %arr.data.gep1190 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1191 = load ptr, ptr %arr.data.gep1190, align 8
  %arr.elem1192 = getelementptr i64, ptr %arr.data1191, i64 55
  store i64 0, ptr %arr.elem1192, align 8
  %arr.data.gep1193 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1194 = load ptr, ptr %arr.data.gep1193, align 8
  %arr.elem1195 = getelementptr i64, ptr %arr.data1194, i64 56
  store i64 0, ptr %arr.elem1195, align 8
  %arr.data.gep1196 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1197 = load ptr, ptr %arr.data.gep1196, align 8
  %arr.elem1198 = getelementptr i64, ptr %arr.data1197, i64 57
  store i64 0, ptr %arr.elem1198, align 8
  %arr.data.gep1199 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1200 = load ptr, ptr %arr.data.gep1199, align 8
  %arr.elem1201 = getelementptr i64, ptr %arr.data1200, i64 58
  store i64 0, ptr %arr.elem1201, align 8
  %arr.data.gep1202 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1203 = load ptr, ptr %arr.data.gep1202, align 8
  %arr.elem1204 = getelementptr i64, ptr %arr.data1203, i64 59
  store i64 0, ptr %arr.elem1204, align 8
  %arr.data.gep1205 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1206 = load ptr, ptr %arr.data.gep1205, align 8
  %arr.elem1207 = getelementptr i64, ptr %arr.data1206, i64 60
  store i64 0, ptr %arr.elem1207, align 8
  %arr.data.gep1208 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1209 = load ptr, ptr %arr.data.gep1208, align 8
  %arr.elem1210 = getelementptr i64, ptr %arr.data1209, i64 61
  store i64 0, ptr %arr.elem1210, align 8
  %arr.data.gep1211 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1212 = load ptr, ptr %arr.data.gep1211, align 8
  %arr.elem1213 = getelementptr i64, ptr %arr.data1212, i64 62
  store i64 0, ptr %arr.elem1213, align 8
  %arr.data.gep1214 = getelementptr inbounds %SadArray, ptr %arr_new1020, i32 0, i32 2
  %arr.data1215 = load ptr, ptr %arr.data.gep1214, align 8
  %arr.elem1216 = getelementptr i64, ptr %arr.data1215, i64 63
  store i64 0, ptr %arr.elem1216, align 8
  %obj.ptrtoint1217 = ptrtoint ptr %arr_new1020 to i64
  store i64 %obj.ptrtoint1217, ptr %"%wdg_y", align 8
  %arr_new1218 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1219 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1219, align 8
  %arr.cap.gep1220 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1220, align 8
  %arr.data1221 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1222 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  store ptr %arr.data1221, ptr %arr.data.gep1222, align 8
  %arr.data.gep1223 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1224 = load ptr, ptr %arr.data.gep1223, align 8
  %arr.elem1225 = getelementptr i64, ptr %arr.data1224, i64 0
  store i64 0, ptr %arr.elem1225, align 8
  %arr.data.gep1226 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1227 = load ptr, ptr %arr.data.gep1226, align 8
  %arr.elem1228 = getelementptr i64, ptr %arr.data1227, i64 1
  store i64 0, ptr %arr.elem1228, align 8
  %arr.data.gep1229 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1230 = load ptr, ptr %arr.data.gep1229, align 8
  %arr.elem1231 = getelementptr i64, ptr %arr.data1230, i64 2
  store i64 0, ptr %arr.elem1231, align 8
  %arr.data.gep1232 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1233 = load ptr, ptr %arr.data.gep1232, align 8
  %arr.elem1234 = getelementptr i64, ptr %arr.data1233, i64 3
  store i64 0, ptr %arr.elem1234, align 8
  %arr.data.gep1235 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1236 = load ptr, ptr %arr.data.gep1235, align 8
  %arr.elem1237 = getelementptr i64, ptr %arr.data1236, i64 4
  store i64 0, ptr %arr.elem1237, align 8
  %arr.data.gep1238 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1239 = load ptr, ptr %arr.data.gep1238, align 8
  %arr.elem1240 = getelementptr i64, ptr %arr.data1239, i64 5
  store i64 0, ptr %arr.elem1240, align 8
  %arr.data.gep1241 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1242 = load ptr, ptr %arr.data.gep1241, align 8
  %arr.elem1243 = getelementptr i64, ptr %arr.data1242, i64 6
  store i64 0, ptr %arr.elem1243, align 8
  %arr.data.gep1244 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1245 = load ptr, ptr %arr.data.gep1244, align 8
  %arr.elem1246 = getelementptr i64, ptr %arr.data1245, i64 7
  store i64 0, ptr %arr.elem1246, align 8
  %arr.data.gep1247 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1248 = load ptr, ptr %arr.data.gep1247, align 8
  %arr.elem1249 = getelementptr i64, ptr %arr.data1248, i64 8
  store i64 0, ptr %arr.elem1249, align 8
  %arr.data.gep1250 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1251 = load ptr, ptr %arr.data.gep1250, align 8
  %arr.elem1252 = getelementptr i64, ptr %arr.data1251, i64 9
  store i64 0, ptr %arr.elem1252, align 8
  %arr.data.gep1253 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1254 = load ptr, ptr %arr.data.gep1253, align 8
  %arr.elem1255 = getelementptr i64, ptr %arr.data1254, i64 10
  store i64 0, ptr %arr.elem1255, align 8
  %arr.data.gep1256 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1257 = load ptr, ptr %arr.data.gep1256, align 8
  %arr.elem1258 = getelementptr i64, ptr %arr.data1257, i64 11
  store i64 0, ptr %arr.elem1258, align 8
  %arr.data.gep1259 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1260 = load ptr, ptr %arr.data.gep1259, align 8
  %arr.elem1261 = getelementptr i64, ptr %arr.data1260, i64 12
  store i64 0, ptr %arr.elem1261, align 8
  %arr.data.gep1262 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1263 = load ptr, ptr %arr.data.gep1262, align 8
  %arr.elem1264 = getelementptr i64, ptr %arr.data1263, i64 13
  store i64 0, ptr %arr.elem1264, align 8
  %arr.data.gep1265 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1266 = load ptr, ptr %arr.data.gep1265, align 8
  %arr.elem1267 = getelementptr i64, ptr %arr.data1266, i64 14
  store i64 0, ptr %arr.elem1267, align 8
  %arr.data.gep1268 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1269 = load ptr, ptr %arr.data.gep1268, align 8
  %arr.elem1270 = getelementptr i64, ptr %arr.data1269, i64 15
  store i64 0, ptr %arr.elem1270, align 8
  %arr.data.gep1271 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1272 = load ptr, ptr %arr.data.gep1271, align 8
  %arr.elem1273 = getelementptr i64, ptr %arr.data1272, i64 16
  store i64 0, ptr %arr.elem1273, align 8
  %arr.data.gep1274 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1275 = load ptr, ptr %arr.data.gep1274, align 8
  %arr.elem1276 = getelementptr i64, ptr %arr.data1275, i64 17
  store i64 0, ptr %arr.elem1276, align 8
  %arr.data.gep1277 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1278 = load ptr, ptr %arr.data.gep1277, align 8
  %arr.elem1279 = getelementptr i64, ptr %arr.data1278, i64 18
  store i64 0, ptr %arr.elem1279, align 8
  %arr.data.gep1280 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1281 = load ptr, ptr %arr.data.gep1280, align 8
  %arr.elem1282 = getelementptr i64, ptr %arr.data1281, i64 19
  store i64 0, ptr %arr.elem1282, align 8
  %arr.data.gep1283 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1284 = load ptr, ptr %arr.data.gep1283, align 8
  %arr.elem1285 = getelementptr i64, ptr %arr.data1284, i64 20
  store i64 0, ptr %arr.elem1285, align 8
  %arr.data.gep1286 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1287 = load ptr, ptr %arr.data.gep1286, align 8
  %arr.elem1288 = getelementptr i64, ptr %arr.data1287, i64 21
  store i64 0, ptr %arr.elem1288, align 8
  %arr.data.gep1289 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1290 = load ptr, ptr %arr.data.gep1289, align 8
  %arr.elem1291 = getelementptr i64, ptr %arr.data1290, i64 22
  store i64 0, ptr %arr.elem1291, align 8
  %arr.data.gep1292 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1293 = load ptr, ptr %arr.data.gep1292, align 8
  %arr.elem1294 = getelementptr i64, ptr %arr.data1293, i64 23
  store i64 0, ptr %arr.elem1294, align 8
  %arr.data.gep1295 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1296 = load ptr, ptr %arr.data.gep1295, align 8
  %arr.elem1297 = getelementptr i64, ptr %arr.data1296, i64 24
  store i64 0, ptr %arr.elem1297, align 8
  %arr.data.gep1298 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1299 = load ptr, ptr %arr.data.gep1298, align 8
  %arr.elem1300 = getelementptr i64, ptr %arr.data1299, i64 25
  store i64 0, ptr %arr.elem1300, align 8
  %arr.data.gep1301 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1302 = load ptr, ptr %arr.data.gep1301, align 8
  %arr.elem1303 = getelementptr i64, ptr %arr.data1302, i64 26
  store i64 0, ptr %arr.elem1303, align 8
  %arr.data.gep1304 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1305 = load ptr, ptr %arr.data.gep1304, align 8
  %arr.elem1306 = getelementptr i64, ptr %arr.data1305, i64 27
  store i64 0, ptr %arr.elem1306, align 8
  %arr.data.gep1307 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1308 = load ptr, ptr %arr.data.gep1307, align 8
  %arr.elem1309 = getelementptr i64, ptr %arr.data1308, i64 28
  store i64 0, ptr %arr.elem1309, align 8
  %arr.data.gep1310 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1311 = load ptr, ptr %arr.data.gep1310, align 8
  %arr.elem1312 = getelementptr i64, ptr %arr.data1311, i64 29
  store i64 0, ptr %arr.elem1312, align 8
  %arr.data.gep1313 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1314 = load ptr, ptr %arr.data.gep1313, align 8
  %arr.elem1315 = getelementptr i64, ptr %arr.data1314, i64 30
  store i64 0, ptr %arr.elem1315, align 8
  %arr.data.gep1316 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1317 = load ptr, ptr %arr.data.gep1316, align 8
  %arr.elem1318 = getelementptr i64, ptr %arr.data1317, i64 31
  store i64 0, ptr %arr.elem1318, align 8
  %arr.data.gep1319 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1320 = load ptr, ptr %arr.data.gep1319, align 8
  %arr.elem1321 = getelementptr i64, ptr %arr.data1320, i64 32
  store i64 0, ptr %arr.elem1321, align 8
  %arr.data.gep1322 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1323 = load ptr, ptr %arr.data.gep1322, align 8
  %arr.elem1324 = getelementptr i64, ptr %arr.data1323, i64 33
  store i64 0, ptr %arr.elem1324, align 8
  %arr.data.gep1325 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1326 = load ptr, ptr %arr.data.gep1325, align 8
  %arr.elem1327 = getelementptr i64, ptr %arr.data1326, i64 34
  store i64 0, ptr %arr.elem1327, align 8
  %arr.data.gep1328 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1329 = load ptr, ptr %arr.data.gep1328, align 8
  %arr.elem1330 = getelementptr i64, ptr %arr.data1329, i64 35
  store i64 0, ptr %arr.elem1330, align 8
  %arr.data.gep1331 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1332 = load ptr, ptr %arr.data.gep1331, align 8
  %arr.elem1333 = getelementptr i64, ptr %arr.data1332, i64 36
  store i64 0, ptr %arr.elem1333, align 8
  %arr.data.gep1334 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1335 = load ptr, ptr %arr.data.gep1334, align 8
  %arr.elem1336 = getelementptr i64, ptr %arr.data1335, i64 37
  store i64 0, ptr %arr.elem1336, align 8
  %arr.data.gep1337 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1338 = load ptr, ptr %arr.data.gep1337, align 8
  %arr.elem1339 = getelementptr i64, ptr %arr.data1338, i64 38
  store i64 0, ptr %arr.elem1339, align 8
  %arr.data.gep1340 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1341 = load ptr, ptr %arr.data.gep1340, align 8
  %arr.elem1342 = getelementptr i64, ptr %arr.data1341, i64 39
  store i64 0, ptr %arr.elem1342, align 8
  %arr.data.gep1343 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1344 = load ptr, ptr %arr.data.gep1343, align 8
  %arr.elem1345 = getelementptr i64, ptr %arr.data1344, i64 40
  store i64 0, ptr %arr.elem1345, align 8
  %arr.data.gep1346 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1347 = load ptr, ptr %arr.data.gep1346, align 8
  %arr.elem1348 = getelementptr i64, ptr %arr.data1347, i64 41
  store i64 0, ptr %arr.elem1348, align 8
  %arr.data.gep1349 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1350 = load ptr, ptr %arr.data.gep1349, align 8
  %arr.elem1351 = getelementptr i64, ptr %arr.data1350, i64 42
  store i64 0, ptr %arr.elem1351, align 8
  %arr.data.gep1352 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1353 = load ptr, ptr %arr.data.gep1352, align 8
  %arr.elem1354 = getelementptr i64, ptr %arr.data1353, i64 43
  store i64 0, ptr %arr.elem1354, align 8
  %arr.data.gep1355 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1356 = load ptr, ptr %arr.data.gep1355, align 8
  %arr.elem1357 = getelementptr i64, ptr %arr.data1356, i64 44
  store i64 0, ptr %arr.elem1357, align 8
  %arr.data.gep1358 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1359 = load ptr, ptr %arr.data.gep1358, align 8
  %arr.elem1360 = getelementptr i64, ptr %arr.data1359, i64 45
  store i64 0, ptr %arr.elem1360, align 8
  %arr.data.gep1361 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1362 = load ptr, ptr %arr.data.gep1361, align 8
  %arr.elem1363 = getelementptr i64, ptr %arr.data1362, i64 46
  store i64 0, ptr %arr.elem1363, align 8
  %arr.data.gep1364 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1365 = load ptr, ptr %arr.data.gep1364, align 8
  %arr.elem1366 = getelementptr i64, ptr %arr.data1365, i64 47
  store i64 0, ptr %arr.elem1366, align 8
  %arr.data.gep1367 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1368 = load ptr, ptr %arr.data.gep1367, align 8
  %arr.elem1369 = getelementptr i64, ptr %arr.data1368, i64 48
  store i64 0, ptr %arr.elem1369, align 8
  %arr.data.gep1370 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1371 = load ptr, ptr %arr.data.gep1370, align 8
  %arr.elem1372 = getelementptr i64, ptr %arr.data1371, i64 49
  store i64 0, ptr %arr.elem1372, align 8
  %arr.data.gep1373 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1374 = load ptr, ptr %arr.data.gep1373, align 8
  %arr.elem1375 = getelementptr i64, ptr %arr.data1374, i64 50
  store i64 0, ptr %arr.elem1375, align 8
  %arr.data.gep1376 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1377 = load ptr, ptr %arr.data.gep1376, align 8
  %arr.elem1378 = getelementptr i64, ptr %arr.data1377, i64 51
  store i64 0, ptr %arr.elem1378, align 8
  %arr.data.gep1379 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1380 = load ptr, ptr %arr.data.gep1379, align 8
  %arr.elem1381 = getelementptr i64, ptr %arr.data1380, i64 52
  store i64 0, ptr %arr.elem1381, align 8
  %arr.data.gep1382 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1383 = load ptr, ptr %arr.data.gep1382, align 8
  %arr.elem1384 = getelementptr i64, ptr %arr.data1383, i64 53
  store i64 0, ptr %arr.elem1384, align 8
  %arr.data.gep1385 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1386 = load ptr, ptr %arr.data.gep1385, align 8
  %arr.elem1387 = getelementptr i64, ptr %arr.data1386, i64 54
  store i64 0, ptr %arr.elem1387, align 8
  %arr.data.gep1388 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1389 = load ptr, ptr %arr.data.gep1388, align 8
  %arr.elem1390 = getelementptr i64, ptr %arr.data1389, i64 55
  store i64 0, ptr %arr.elem1390, align 8
  %arr.data.gep1391 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1392 = load ptr, ptr %arr.data.gep1391, align 8
  %arr.elem1393 = getelementptr i64, ptr %arr.data1392, i64 56
  store i64 0, ptr %arr.elem1393, align 8
  %arr.data.gep1394 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1395 = load ptr, ptr %arr.data.gep1394, align 8
  %arr.elem1396 = getelementptr i64, ptr %arr.data1395, i64 57
  store i64 0, ptr %arr.elem1396, align 8
  %arr.data.gep1397 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1398 = load ptr, ptr %arr.data.gep1397, align 8
  %arr.elem1399 = getelementptr i64, ptr %arr.data1398, i64 58
  store i64 0, ptr %arr.elem1399, align 8
  %arr.data.gep1400 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1401 = load ptr, ptr %arr.data.gep1400, align 8
  %arr.elem1402 = getelementptr i64, ptr %arr.data1401, i64 59
  store i64 0, ptr %arr.elem1402, align 8
  %arr.data.gep1403 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1404 = load ptr, ptr %arr.data.gep1403, align 8
  %arr.elem1405 = getelementptr i64, ptr %arr.data1404, i64 60
  store i64 0, ptr %arr.elem1405, align 8
  %arr.data.gep1406 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1407 = load ptr, ptr %arr.data.gep1406, align 8
  %arr.elem1408 = getelementptr i64, ptr %arr.data1407, i64 61
  store i64 0, ptr %arr.elem1408, align 8
  %arr.data.gep1409 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1410 = load ptr, ptr %arr.data.gep1409, align 8
  %arr.elem1411 = getelementptr i64, ptr %arr.data1410, i64 62
  store i64 0, ptr %arr.elem1411, align 8
  %arr.data.gep1412 = getelementptr inbounds %SadArray, ptr %arr_new1218, i32 0, i32 2
  %arr.data1413 = load ptr, ptr %arr.data.gep1412, align 8
  %arr.elem1414 = getelementptr i64, ptr %arr.data1413, i64 63
  store i64 0, ptr %arr.elem1414, align 8
  %obj.ptrtoint1415 = ptrtoint ptr %arr_new1218 to i64
  store i64 %obj.ptrtoint1415, ptr %"%wdg_w", align 8
  %arr_new1416 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1417 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1417, align 8
  %arr.cap.gep1418 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1418, align 8
  %arr.data1419 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1420 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  store ptr %arr.data1419, ptr %arr.data.gep1420, align 8
  %arr.data.gep1421 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1422 = load ptr, ptr %arr.data.gep1421, align 8
  %arr.elem1423 = getelementptr i64, ptr %arr.data1422, i64 0
  store i64 0, ptr %arr.elem1423, align 8
  %arr.data.gep1424 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1425 = load ptr, ptr %arr.data.gep1424, align 8
  %arr.elem1426 = getelementptr i64, ptr %arr.data1425, i64 1
  store i64 0, ptr %arr.elem1426, align 8
  %arr.data.gep1427 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1428 = load ptr, ptr %arr.data.gep1427, align 8
  %arr.elem1429 = getelementptr i64, ptr %arr.data1428, i64 2
  store i64 0, ptr %arr.elem1429, align 8
  %arr.data.gep1430 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1431 = load ptr, ptr %arr.data.gep1430, align 8
  %arr.elem1432 = getelementptr i64, ptr %arr.data1431, i64 3
  store i64 0, ptr %arr.elem1432, align 8
  %arr.data.gep1433 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1434 = load ptr, ptr %arr.data.gep1433, align 8
  %arr.elem1435 = getelementptr i64, ptr %arr.data1434, i64 4
  store i64 0, ptr %arr.elem1435, align 8
  %arr.data.gep1436 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1437 = load ptr, ptr %arr.data.gep1436, align 8
  %arr.elem1438 = getelementptr i64, ptr %arr.data1437, i64 5
  store i64 0, ptr %arr.elem1438, align 8
  %arr.data.gep1439 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1440 = load ptr, ptr %arr.data.gep1439, align 8
  %arr.elem1441 = getelementptr i64, ptr %arr.data1440, i64 6
  store i64 0, ptr %arr.elem1441, align 8
  %arr.data.gep1442 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1443 = load ptr, ptr %arr.data.gep1442, align 8
  %arr.elem1444 = getelementptr i64, ptr %arr.data1443, i64 7
  store i64 0, ptr %arr.elem1444, align 8
  %arr.data.gep1445 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1446 = load ptr, ptr %arr.data.gep1445, align 8
  %arr.elem1447 = getelementptr i64, ptr %arr.data1446, i64 8
  store i64 0, ptr %arr.elem1447, align 8
  %arr.data.gep1448 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1449 = load ptr, ptr %arr.data.gep1448, align 8
  %arr.elem1450 = getelementptr i64, ptr %arr.data1449, i64 9
  store i64 0, ptr %arr.elem1450, align 8
  %arr.data.gep1451 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1452 = load ptr, ptr %arr.data.gep1451, align 8
  %arr.elem1453 = getelementptr i64, ptr %arr.data1452, i64 10
  store i64 0, ptr %arr.elem1453, align 8
  %arr.data.gep1454 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1455 = load ptr, ptr %arr.data.gep1454, align 8
  %arr.elem1456 = getelementptr i64, ptr %arr.data1455, i64 11
  store i64 0, ptr %arr.elem1456, align 8
  %arr.data.gep1457 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1458 = load ptr, ptr %arr.data.gep1457, align 8
  %arr.elem1459 = getelementptr i64, ptr %arr.data1458, i64 12
  store i64 0, ptr %arr.elem1459, align 8
  %arr.data.gep1460 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1461 = load ptr, ptr %arr.data.gep1460, align 8
  %arr.elem1462 = getelementptr i64, ptr %arr.data1461, i64 13
  store i64 0, ptr %arr.elem1462, align 8
  %arr.data.gep1463 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1464 = load ptr, ptr %arr.data.gep1463, align 8
  %arr.elem1465 = getelementptr i64, ptr %arr.data1464, i64 14
  store i64 0, ptr %arr.elem1465, align 8
  %arr.data.gep1466 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1467 = load ptr, ptr %arr.data.gep1466, align 8
  %arr.elem1468 = getelementptr i64, ptr %arr.data1467, i64 15
  store i64 0, ptr %arr.elem1468, align 8
  %arr.data.gep1469 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1470 = load ptr, ptr %arr.data.gep1469, align 8
  %arr.elem1471 = getelementptr i64, ptr %arr.data1470, i64 16
  store i64 0, ptr %arr.elem1471, align 8
  %arr.data.gep1472 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1473 = load ptr, ptr %arr.data.gep1472, align 8
  %arr.elem1474 = getelementptr i64, ptr %arr.data1473, i64 17
  store i64 0, ptr %arr.elem1474, align 8
  %arr.data.gep1475 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1476 = load ptr, ptr %arr.data.gep1475, align 8
  %arr.elem1477 = getelementptr i64, ptr %arr.data1476, i64 18
  store i64 0, ptr %arr.elem1477, align 8
  %arr.data.gep1478 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1479 = load ptr, ptr %arr.data.gep1478, align 8
  %arr.elem1480 = getelementptr i64, ptr %arr.data1479, i64 19
  store i64 0, ptr %arr.elem1480, align 8
  %arr.data.gep1481 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1482 = load ptr, ptr %arr.data.gep1481, align 8
  %arr.elem1483 = getelementptr i64, ptr %arr.data1482, i64 20
  store i64 0, ptr %arr.elem1483, align 8
  %arr.data.gep1484 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1485 = load ptr, ptr %arr.data.gep1484, align 8
  %arr.elem1486 = getelementptr i64, ptr %arr.data1485, i64 21
  store i64 0, ptr %arr.elem1486, align 8
  %arr.data.gep1487 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1488 = load ptr, ptr %arr.data.gep1487, align 8
  %arr.elem1489 = getelementptr i64, ptr %arr.data1488, i64 22
  store i64 0, ptr %arr.elem1489, align 8
  %arr.data.gep1490 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1491 = load ptr, ptr %arr.data.gep1490, align 8
  %arr.elem1492 = getelementptr i64, ptr %arr.data1491, i64 23
  store i64 0, ptr %arr.elem1492, align 8
  %arr.data.gep1493 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1494 = load ptr, ptr %arr.data.gep1493, align 8
  %arr.elem1495 = getelementptr i64, ptr %arr.data1494, i64 24
  store i64 0, ptr %arr.elem1495, align 8
  %arr.data.gep1496 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1497 = load ptr, ptr %arr.data.gep1496, align 8
  %arr.elem1498 = getelementptr i64, ptr %arr.data1497, i64 25
  store i64 0, ptr %arr.elem1498, align 8
  %arr.data.gep1499 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1500 = load ptr, ptr %arr.data.gep1499, align 8
  %arr.elem1501 = getelementptr i64, ptr %arr.data1500, i64 26
  store i64 0, ptr %arr.elem1501, align 8
  %arr.data.gep1502 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1503 = load ptr, ptr %arr.data.gep1502, align 8
  %arr.elem1504 = getelementptr i64, ptr %arr.data1503, i64 27
  store i64 0, ptr %arr.elem1504, align 8
  %arr.data.gep1505 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1506 = load ptr, ptr %arr.data.gep1505, align 8
  %arr.elem1507 = getelementptr i64, ptr %arr.data1506, i64 28
  store i64 0, ptr %arr.elem1507, align 8
  %arr.data.gep1508 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1509 = load ptr, ptr %arr.data.gep1508, align 8
  %arr.elem1510 = getelementptr i64, ptr %arr.data1509, i64 29
  store i64 0, ptr %arr.elem1510, align 8
  %arr.data.gep1511 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1512 = load ptr, ptr %arr.data.gep1511, align 8
  %arr.elem1513 = getelementptr i64, ptr %arr.data1512, i64 30
  store i64 0, ptr %arr.elem1513, align 8
  %arr.data.gep1514 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1515 = load ptr, ptr %arr.data.gep1514, align 8
  %arr.elem1516 = getelementptr i64, ptr %arr.data1515, i64 31
  store i64 0, ptr %arr.elem1516, align 8
  %arr.data.gep1517 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1518 = load ptr, ptr %arr.data.gep1517, align 8
  %arr.elem1519 = getelementptr i64, ptr %arr.data1518, i64 32
  store i64 0, ptr %arr.elem1519, align 8
  %arr.data.gep1520 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1521 = load ptr, ptr %arr.data.gep1520, align 8
  %arr.elem1522 = getelementptr i64, ptr %arr.data1521, i64 33
  store i64 0, ptr %arr.elem1522, align 8
  %arr.data.gep1523 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1524 = load ptr, ptr %arr.data.gep1523, align 8
  %arr.elem1525 = getelementptr i64, ptr %arr.data1524, i64 34
  store i64 0, ptr %arr.elem1525, align 8
  %arr.data.gep1526 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1527 = load ptr, ptr %arr.data.gep1526, align 8
  %arr.elem1528 = getelementptr i64, ptr %arr.data1527, i64 35
  store i64 0, ptr %arr.elem1528, align 8
  %arr.data.gep1529 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1530 = load ptr, ptr %arr.data.gep1529, align 8
  %arr.elem1531 = getelementptr i64, ptr %arr.data1530, i64 36
  store i64 0, ptr %arr.elem1531, align 8
  %arr.data.gep1532 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1533 = load ptr, ptr %arr.data.gep1532, align 8
  %arr.elem1534 = getelementptr i64, ptr %arr.data1533, i64 37
  store i64 0, ptr %arr.elem1534, align 8
  %arr.data.gep1535 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1536 = load ptr, ptr %arr.data.gep1535, align 8
  %arr.elem1537 = getelementptr i64, ptr %arr.data1536, i64 38
  store i64 0, ptr %arr.elem1537, align 8
  %arr.data.gep1538 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1539 = load ptr, ptr %arr.data.gep1538, align 8
  %arr.elem1540 = getelementptr i64, ptr %arr.data1539, i64 39
  store i64 0, ptr %arr.elem1540, align 8
  %arr.data.gep1541 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1542 = load ptr, ptr %arr.data.gep1541, align 8
  %arr.elem1543 = getelementptr i64, ptr %arr.data1542, i64 40
  store i64 0, ptr %arr.elem1543, align 8
  %arr.data.gep1544 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1545 = load ptr, ptr %arr.data.gep1544, align 8
  %arr.elem1546 = getelementptr i64, ptr %arr.data1545, i64 41
  store i64 0, ptr %arr.elem1546, align 8
  %arr.data.gep1547 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1548 = load ptr, ptr %arr.data.gep1547, align 8
  %arr.elem1549 = getelementptr i64, ptr %arr.data1548, i64 42
  store i64 0, ptr %arr.elem1549, align 8
  %arr.data.gep1550 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1551 = load ptr, ptr %arr.data.gep1550, align 8
  %arr.elem1552 = getelementptr i64, ptr %arr.data1551, i64 43
  store i64 0, ptr %arr.elem1552, align 8
  %arr.data.gep1553 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1554 = load ptr, ptr %arr.data.gep1553, align 8
  %arr.elem1555 = getelementptr i64, ptr %arr.data1554, i64 44
  store i64 0, ptr %arr.elem1555, align 8
  %arr.data.gep1556 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1557 = load ptr, ptr %arr.data.gep1556, align 8
  %arr.elem1558 = getelementptr i64, ptr %arr.data1557, i64 45
  store i64 0, ptr %arr.elem1558, align 8
  %arr.data.gep1559 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1560 = load ptr, ptr %arr.data.gep1559, align 8
  %arr.elem1561 = getelementptr i64, ptr %arr.data1560, i64 46
  store i64 0, ptr %arr.elem1561, align 8
  %arr.data.gep1562 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1563 = load ptr, ptr %arr.data.gep1562, align 8
  %arr.elem1564 = getelementptr i64, ptr %arr.data1563, i64 47
  store i64 0, ptr %arr.elem1564, align 8
  %arr.data.gep1565 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1566 = load ptr, ptr %arr.data.gep1565, align 8
  %arr.elem1567 = getelementptr i64, ptr %arr.data1566, i64 48
  store i64 0, ptr %arr.elem1567, align 8
  %arr.data.gep1568 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1569 = load ptr, ptr %arr.data.gep1568, align 8
  %arr.elem1570 = getelementptr i64, ptr %arr.data1569, i64 49
  store i64 0, ptr %arr.elem1570, align 8
  %arr.data.gep1571 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1572 = load ptr, ptr %arr.data.gep1571, align 8
  %arr.elem1573 = getelementptr i64, ptr %arr.data1572, i64 50
  store i64 0, ptr %arr.elem1573, align 8
  %arr.data.gep1574 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1575 = load ptr, ptr %arr.data.gep1574, align 8
  %arr.elem1576 = getelementptr i64, ptr %arr.data1575, i64 51
  store i64 0, ptr %arr.elem1576, align 8
  %arr.data.gep1577 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1578 = load ptr, ptr %arr.data.gep1577, align 8
  %arr.elem1579 = getelementptr i64, ptr %arr.data1578, i64 52
  store i64 0, ptr %arr.elem1579, align 8
  %arr.data.gep1580 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1581 = load ptr, ptr %arr.data.gep1580, align 8
  %arr.elem1582 = getelementptr i64, ptr %arr.data1581, i64 53
  store i64 0, ptr %arr.elem1582, align 8
  %arr.data.gep1583 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1584 = load ptr, ptr %arr.data.gep1583, align 8
  %arr.elem1585 = getelementptr i64, ptr %arr.data1584, i64 54
  store i64 0, ptr %arr.elem1585, align 8
  %arr.data.gep1586 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1587 = load ptr, ptr %arr.data.gep1586, align 8
  %arr.elem1588 = getelementptr i64, ptr %arr.data1587, i64 55
  store i64 0, ptr %arr.elem1588, align 8
  %arr.data.gep1589 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1590 = load ptr, ptr %arr.data.gep1589, align 8
  %arr.elem1591 = getelementptr i64, ptr %arr.data1590, i64 56
  store i64 0, ptr %arr.elem1591, align 8
  %arr.data.gep1592 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1593 = load ptr, ptr %arr.data.gep1592, align 8
  %arr.elem1594 = getelementptr i64, ptr %arr.data1593, i64 57
  store i64 0, ptr %arr.elem1594, align 8
  %arr.data.gep1595 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1596 = load ptr, ptr %arr.data.gep1595, align 8
  %arr.elem1597 = getelementptr i64, ptr %arr.data1596, i64 58
  store i64 0, ptr %arr.elem1597, align 8
  %arr.data.gep1598 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1599 = load ptr, ptr %arr.data.gep1598, align 8
  %arr.elem1600 = getelementptr i64, ptr %arr.data1599, i64 59
  store i64 0, ptr %arr.elem1600, align 8
  %arr.data.gep1601 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1602 = load ptr, ptr %arr.data.gep1601, align 8
  %arr.elem1603 = getelementptr i64, ptr %arr.data1602, i64 60
  store i64 0, ptr %arr.elem1603, align 8
  %arr.data.gep1604 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1605 = load ptr, ptr %arr.data.gep1604, align 8
  %arr.elem1606 = getelementptr i64, ptr %arr.data1605, i64 61
  store i64 0, ptr %arr.elem1606, align 8
  %arr.data.gep1607 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1608 = load ptr, ptr %arr.data.gep1607, align 8
  %arr.elem1609 = getelementptr i64, ptr %arr.data1608, i64 62
  store i64 0, ptr %arr.elem1609, align 8
  %arr.data.gep1610 = getelementptr inbounds %SadArray, ptr %arr_new1416, i32 0, i32 2
  %arr.data1611 = load ptr, ptr %arr.data.gep1610, align 8
  %arr.elem1612 = getelementptr i64, ptr %arr.data1611, i64 63
  store i64 0, ptr %arr.elem1612, align 8
  %obj.ptrtoint1613 = ptrtoint ptr %arr_new1416 to i64
  store i64 %obj.ptrtoint1613, ptr %"%wdg_h", align 8
  %arr_new1614 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1615 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1615, align 8
  %arr.cap.gep1616 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1616, align 8
  %arr.data1617 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1618 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  store ptr %arr.data1617, ptr %arr.data.gep1618, align 8
  %arr.data.gep1619 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1620 = load ptr, ptr %arr.data.gep1619, align 8
  %arr.elem1621 = getelementptr i64, ptr %arr.data1620, i64 0
  store i64 0, ptr %arr.elem1621, align 8
  %arr.data.gep1622 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1623 = load ptr, ptr %arr.data.gep1622, align 8
  %arr.elem1624 = getelementptr i64, ptr %arr.data1623, i64 1
  store i64 0, ptr %arr.elem1624, align 8
  %arr.data.gep1625 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1626 = load ptr, ptr %arr.data.gep1625, align 8
  %arr.elem1627 = getelementptr i64, ptr %arr.data1626, i64 2
  store i64 0, ptr %arr.elem1627, align 8
  %arr.data.gep1628 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1629 = load ptr, ptr %arr.data.gep1628, align 8
  %arr.elem1630 = getelementptr i64, ptr %arr.data1629, i64 3
  store i64 0, ptr %arr.elem1630, align 8
  %arr.data.gep1631 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1632 = load ptr, ptr %arr.data.gep1631, align 8
  %arr.elem1633 = getelementptr i64, ptr %arr.data1632, i64 4
  store i64 0, ptr %arr.elem1633, align 8
  %arr.data.gep1634 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1635 = load ptr, ptr %arr.data.gep1634, align 8
  %arr.elem1636 = getelementptr i64, ptr %arr.data1635, i64 5
  store i64 0, ptr %arr.elem1636, align 8
  %arr.data.gep1637 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1638 = load ptr, ptr %arr.data.gep1637, align 8
  %arr.elem1639 = getelementptr i64, ptr %arr.data1638, i64 6
  store i64 0, ptr %arr.elem1639, align 8
  %arr.data.gep1640 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1641 = load ptr, ptr %arr.data.gep1640, align 8
  %arr.elem1642 = getelementptr i64, ptr %arr.data1641, i64 7
  store i64 0, ptr %arr.elem1642, align 8
  %arr.data.gep1643 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1644 = load ptr, ptr %arr.data.gep1643, align 8
  %arr.elem1645 = getelementptr i64, ptr %arr.data1644, i64 8
  store i64 0, ptr %arr.elem1645, align 8
  %arr.data.gep1646 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1647 = load ptr, ptr %arr.data.gep1646, align 8
  %arr.elem1648 = getelementptr i64, ptr %arr.data1647, i64 9
  store i64 0, ptr %arr.elem1648, align 8
  %arr.data.gep1649 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1650 = load ptr, ptr %arr.data.gep1649, align 8
  %arr.elem1651 = getelementptr i64, ptr %arr.data1650, i64 10
  store i64 0, ptr %arr.elem1651, align 8
  %arr.data.gep1652 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1653 = load ptr, ptr %arr.data.gep1652, align 8
  %arr.elem1654 = getelementptr i64, ptr %arr.data1653, i64 11
  store i64 0, ptr %arr.elem1654, align 8
  %arr.data.gep1655 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1656 = load ptr, ptr %arr.data.gep1655, align 8
  %arr.elem1657 = getelementptr i64, ptr %arr.data1656, i64 12
  store i64 0, ptr %arr.elem1657, align 8
  %arr.data.gep1658 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1659 = load ptr, ptr %arr.data.gep1658, align 8
  %arr.elem1660 = getelementptr i64, ptr %arr.data1659, i64 13
  store i64 0, ptr %arr.elem1660, align 8
  %arr.data.gep1661 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1662 = load ptr, ptr %arr.data.gep1661, align 8
  %arr.elem1663 = getelementptr i64, ptr %arr.data1662, i64 14
  store i64 0, ptr %arr.elem1663, align 8
  %arr.data.gep1664 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1665 = load ptr, ptr %arr.data.gep1664, align 8
  %arr.elem1666 = getelementptr i64, ptr %arr.data1665, i64 15
  store i64 0, ptr %arr.elem1666, align 8
  %arr.data.gep1667 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1668 = load ptr, ptr %arr.data.gep1667, align 8
  %arr.elem1669 = getelementptr i64, ptr %arr.data1668, i64 16
  store i64 0, ptr %arr.elem1669, align 8
  %arr.data.gep1670 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1671 = load ptr, ptr %arr.data.gep1670, align 8
  %arr.elem1672 = getelementptr i64, ptr %arr.data1671, i64 17
  store i64 0, ptr %arr.elem1672, align 8
  %arr.data.gep1673 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1674 = load ptr, ptr %arr.data.gep1673, align 8
  %arr.elem1675 = getelementptr i64, ptr %arr.data1674, i64 18
  store i64 0, ptr %arr.elem1675, align 8
  %arr.data.gep1676 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1677 = load ptr, ptr %arr.data.gep1676, align 8
  %arr.elem1678 = getelementptr i64, ptr %arr.data1677, i64 19
  store i64 0, ptr %arr.elem1678, align 8
  %arr.data.gep1679 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1680 = load ptr, ptr %arr.data.gep1679, align 8
  %arr.elem1681 = getelementptr i64, ptr %arr.data1680, i64 20
  store i64 0, ptr %arr.elem1681, align 8
  %arr.data.gep1682 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1683 = load ptr, ptr %arr.data.gep1682, align 8
  %arr.elem1684 = getelementptr i64, ptr %arr.data1683, i64 21
  store i64 0, ptr %arr.elem1684, align 8
  %arr.data.gep1685 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1686 = load ptr, ptr %arr.data.gep1685, align 8
  %arr.elem1687 = getelementptr i64, ptr %arr.data1686, i64 22
  store i64 0, ptr %arr.elem1687, align 8
  %arr.data.gep1688 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1689 = load ptr, ptr %arr.data.gep1688, align 8
  %arr.elem1690 = getelementptr i64, ptr %arr.data1689, i64 23
  store i64 0, ptr %arr.elem1690, align 8
  %arr.data.gep1691 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1692 = load ptr, ptr %arr.data.gep1691, align 8
  %arr.elem1693 = getelementptr i64, ptr %arr.data1692, i64 24
  store i64 0, ptr %arr.elem1693, align 8
  %arr.data.gep1694 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1695 = load ptr, ptr %arr.data.gep1694, align 8
  %arr.elem1696 = getelementptr i64, ptr %arr.data1695, i64 25
  store i64 0, ptr %arr.elem1696, align 8
  %arr.data.gep1697 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1698 = load ptr, ptr %arr.data.gep1697, align 8
  %arr.elem1699 = getelementptr i64, ptr %arr.data1698, i64 26
  store i64 0, ptr %arr.elem1699, align 8
  %arr.data.gep1700 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1701 = load ptr, ptr %arr.data.gep1700, align 8
  %arr.elem1702 = getelementptr i64, ptr %arr.data1701, i64 27
  store i64 0, ptr %arr.elem1702, align 8
  %arr.data.gep1703 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1704 = load ptr, ptr %arr.data.gep1703, align 8
  %arr.elem1705 = getelementptr i64, ptr %arr.data1704, i64 28
  store i64 0, ptr %arr.elem1705, align 8
  %arr.data.gep1706 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1707 = load ptr, ptr %arr.data.gep1706, align 8
  %arr.elem1708 = getelementptr i64, ptr %arr.data1707, i64 29
  store i64 0, ptr %arr.elem1708, align 8
  %arr.data.gep1709 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1710 = load ptr, ptr %arr.data.gep1709, align 8
  %arr.elem1711 = getelementptr i64, ptr %arr.data1710, i64 30
  store i64 0, ptr %arr.elem1711, align 8
  %arr.data.gep1712 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1713 = load ptr, ptr %arr.data.gep1712, align 8
  %arr.elem1714 = getelementptr i64, ptr %arr.data1713, i64 31
  store i64 0, ptr %arr.elem1714, align 8
  %arr.data.gep1715 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1716 = load ptr, ptr %arr.data.gep1715, align 8
  %arr.elem1717 = getelementptr i64, ptr %arr.data1716, i64 32
  store i64 0, ptr %arr.elem1717, align 8
  %arr.data.gep1718 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1719 = load ptr, ptr %arr.data.gep1718, align 8
  %arr.elem1720 = getelementptr i64, ptr %arr.data1719, i64 33
  store i64 0, ptr %arr.elem1720, align 8
  %arr.data.gep1721 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1722 = load ptr, ptr %arr.data.gep1721, align 8
  %arr.elem1723 = getelementptr i64, ptr %arr.data1722, i64 34
  store i64 0, ptr %arr.elem1723, align 8
  %arr.data.gep1724 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1725 = load ptr, ptr %arr.data.gep1724, align 8
  %arr.elem1726 = getelementptr i64, ptr %arr.data1725, i64 35
  store i64 0, ptr %arr.elem1726, align 8
  %arr.data.gep1727 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1728 = load ptr, ptr %arr.data.gep1727, align 8
  %arr.elem1729 = getelementptr i64, ptr %arr.data1728, i64 36
  store i64 0, ptr %arr.elem1729, align 8
  %arr.data.gep1730 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1731 = load ptr, ptr %arr.data.gep1730, align 8
  %arr.elem1732 = getelementptr i64, ptr %arr.data1731, i64 37
  store i64 0, ptr %arr.elem1732, align 8
  %arr.data.gep1733 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1734 = load ptr, ptr %arr.data.gep1733, align 8
  %arr.elem1735 = getelementptr i64, ptr %arr.data1734, i64 38
  store i64 0, ptr %arr.elem1735, align 8
  %arr.data.gep1736 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1737 = load ptr, ptr %arr.data.gep1736, align 8
  %arr.elem1738 = getelementptr i64, ptr %arr.data1737, i64 39
  store i64 0, ptr %arr.elem1738, align 8
  %arr.data.gep1739 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1740 = load ptr, ptr %arr.data.gep1739, align 8
  %arr.elem1741 = getelementptr i64, ptr %arr.data1740, i64 40
  store i64 0, ptr %arr.elem1741, align 8
  %arr.data.gep1742 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1743 = load ptr, ptr %arr.data.gep1742, align 8
  %arr.elem1744 = getelementptr i64, ptr %arr.data1743, i64 41
  store i64 0, ptr %arr.elem1744, align 8
  %arr.data.gep1745 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1746 = load ptr, ptr %arr.data.gep1745, align 8
  %arr.elem1747 = getelementptr i64, ptr %arr.data1746, i64 42
  store i64 0, ptr %arr.elem1747, align 8
  %arr.data.gep1748 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1749 = load ptr, ptr %arr.data.gep1748, align 8
  %arr.elem1750 = getelementptr i64, ptr %arr.data1749, i64 43
  store i64 0, ptr %arr.elem1750, align 8
  %arr.data.gep1751 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1752 = load ptr, ptr %arr.data.gep1751, align 8
  %arr.elem1753 = getelementptr i64, ptr %arr.data1752, i64 44
  store i64 0, ptr %arr.elem1753, align 8
  %arr.data.gep1754 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1755 = load ptr, ptr %arr.data.gep1754, align 8
  %arr.elem1756 = getelementptr i64, ptr %arr.data1755, i64 45
  store i64 0, ptr %arr.elem1756, align 8
  %arr.data.gep1757 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1758 = load ptr, ptr %arr.data.gep1757, align 8
  %arr.elem1759 = getelementptr i64, ptr %arr.data1758, i64 46
  store i64 0, ptr %arr.elem1759, align 8
  %arr.data.gep1760 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1761 = load ptr, ptr %arr.data.gep1760, align 8
  %arr.elem1762 = getelementptr i64, ptr %arr.data1761, i64 47
  store i64 0, ptr %arr.elem1762, align 8
  %arr.data.gep1763 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1764 = load ptr, ptr %arr.data.gep1763, align 8
  %arr.elem1765 = getelementptr i64, ptr %arr.data1764, i64 48
  store i64 0, ptr %arr.elem1765, align 8
  %arr.data.gep1766 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1767 = load ptr, ptr %arr.data.gep1766, align 8
  %arr.elem1768 = getelementptr i64, ptr %arr.data1767, i64 49
  store i64 0, ptr %arr.elem1768, align 8
  %arr.data.gep1769 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1770 = load ptr, ptr %arr.data.gep1769, align 8
  %arr.elem1771 = getelementptr i64, ptr %arr.data1770, i64 50
  store i64 0, ptr %arr.elem1771, align 8
  %arr.data.gep1772 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1773 = load ptr, ptr %arr.data.gep1772, align 8
  %arr.elem1774 = getelementptr i64, ptr %arr.data1773, i64 51
  store i64 0, ptr %arr.elem1774, align 8
  %arr.data.gep1775 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1776 = load ptr, ptr %arr.data.gep1775, align 8
  %arr.elem1777 = getelementptr i64, ptr %arr.data1776, i64 52
  store i64 0, ptr %arr.elem1777, align 8
  %arr.data.gep1778 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1779 = load ptr, ptr %arr.data.gep1778, align 8
  %arr.elem1780 = getelementptr i64, ptr %arr.data1779, i64 53
  store i64 0, ptr %arr.elem1780, align 8
  %arr.data.gep1781 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1782 = load ptr, ptr %arr.data.gep1781, align 8
  %arr.elem1783 = getelementptr i64, ptr %arr.data1782, i64 54
  store i64 0, ptr %arr.elem1783, align 8
  %arr.data.gep1784 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1785 = load ptr, ptr %arr.data.gep1784, align 8
  %arr.elem1786 = getelementptr i64, ptr %arr.data1785, i64 55
  store i64 0, ptr %arr.elem1786, align 8
  %arr.data.gep1787 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1788 = load ptr, ptr %arr.data.gep1787, align 8
  %arr.elem1789 = getelementptr i64, ptr %arr.data1788, i64 56
  store i64 0, ptr %arr.elem1789, align 8
  %arr.data.gep1790 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1791 = load ptr, ptr %arr.data.gep1790, align 8
  %arr.elem1792 = getelementptr i64, ptr %arr.data1791, i64 57
  store i64 0, ptr %arr.elem1792, align 8
  %arr.data.gep1793 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1794 = load ptr, ptr %arr.data.gep1793, align 8
  %arr.elem1795 = getelementptr i64, ptr %arr.data1794, i64 58
  store i64 0, ptr %arr.elem1795, align 8
  %arr.data.gep1796 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1797 = load ptr, ptr %arr.data.gep1796, align 8
  %arr.elem1798 = getelementptr i64, ptr %arr.data1797, i64 59
  store i64 0, ptr %arr.elem1798, align 8
  %arr.data.gep1799 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1800 = load ptr, ptr %arr.data.gep1799, align 8
  %arr.elem1801 = getelementptr i64, ptr %arr.data1800, i64 60
  store i64 0, ptr %arr.elem1801, align 8
  %arr.data.gep1802 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1803 = load ptr, ptr %arr.data.gep1802, align 8
  %arr.elem1804 = getelementptr i64, ptr %arr.data1803, i64 61
  store i64 0, ptr %arr.elem1804, align 8
  %arr.data.gep1805 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1806 = load ptr, ptr %arr.data.gep1805, align 8
  %arr.elem1807 = getelementptr i64, ptr %arr.data1806, i64 62
  store i64 0, ptr %arr.elem1807, align 8
  %arr.data.gep1808 = getelementptr inbounds %SadArray, ptr %arr_new1614, i32 0, i32 2
  %arr.data1809 = load ptr, ptr %arr.data.gep1808, align 8
  %arr.elem1810 = getelementptr i64, ptr %arr.data1809, i64 63
  store i64 0, ptr %arr.elem1810, align 8
  %obj.ptrtoint1811 = ptrtoint ptr %arr_new1614 to i64
  store i64 %obj.ptrtoint1811, ptr %"%wdg_\D9\82\D9\8A\D9\85\D8\A9", align 8
  %arr_new1812 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep1813 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 0
  store i64 64, ptr %arr.len.gep1813, align 8
  %arr.cap.gep1814 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep1814, align 8
  %arr.data1815 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep1816 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  store ptr %arr.data1815, ptr %arr.data.gep1816, align 8
  %arr.data.gep1817 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1818 = load ptr, ptr %arr.data.gep1817, align 8
  %arr.elem1819 = getelementptr i64, ptr %arr.data1818, i64 0
  store i64 0, ptr %arr.elem1819, align 8
  %arr.data.gep1820 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1821 = load ptr, ptr %arr.data.gep1820, align 8
  %arr.elem1822 = getelementptr i64, ptr %arr.data1821, i64 1
  store i64 0, ptr %arr.elem1822, align 8
  %arr.data.gep1823 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1824 = load ptr, ptr %arr.data.gep1823, align 8
  %arr.elem1825 = getelementptr i64, ptr %arr.data1824, i64 2
  store i64 0, ptr %arr.elem1825, align 8
  %arr.data.gep1826 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1827 = load ptr, ptr %arr.data.gep1826, align 8
  %arr.elem1828 = getelementptr i64, ptr %arr.data1827, i64 3
  store i64 0, ptr %arr.elem1828, align 8
  %arr.data.gep1829 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1830 = load ptr, ptr %arr.data.gep1829, align 8
  %arr.elem1831 = getelementptr i64, ptr %arr.data1830, i64 4
  store i64 0, ptr %arr.elem1831, align 8
  %arr.data.gep1832 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1833 = load ptr, ptr %arr.data.gep1832, align 8
  %arr.elem1834 = getelementptr i64, ptr %arr.data1833, i64 5
  store i64 0, ptr %arr.elem1834, align 8
  %arr.data.gep1835 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1836 = load ptr, ptr %arr.data.gep1835, align 8
  %arr.elem1837 = getelementptr i64, ptr %arr.data1836, i64 6
  store i64 0, ptr %arr.elem1837, align 8
  %arr.data.gep1838 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1839 = load ptr, ptr %arr.data.gep1838, align 8
  %arr.elem1840 = getelementptr i64, ptr %arr.data1839, i64 7
  store i64 0, ptr %arr.elem1840, align 8
  %arr.data.gep1841 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1842 = load ptr, ptr %arr.data.gep1841, align 8
  %arr.elem1843 = getelementptr i64, ptr %arr.data1842, i64 8
  store i64 0, ptr %arr.elem1843, align 8
  %arr.data.gep1844 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1845 = load ptr, ptr %arr.data.gep1844, align 8
  %arr.elem1846 = getelementptr i64, ptr %arr.data1845, i64 9
  store i64 0, ptr %arr.elem1846, align 8
  %arr.data.gep1847 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1848 = load ptr, ptr %arr.data.gep1847, align 8
  %arr.elem1849 = getelementptr i64, ptr %arr.data1848, i64 10
  store i64 0, ptr %arr.elem1849, align 8
  %arr.data.gep1850 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1851 = load ptr, ptr %arr.data.gep1850, align 8
  %arr.elem1852 = getelementptr i64, ptr %arr.data1851, i64 11
  store i64 0, ptr %arr.elem1852, align 8
  %arr.data.gep1853 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1854 = load ptr, ptr %arr.data.gep1853, align 8
  %arr.elem1855 = getelementptr i64, ptr %arr.data1854, i64 12
  store i64 0, ptr %arr.elem1855, align 8
  %arr.data.gep1856 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1857 = load ptr, ptr %arr.data.gep1856, align 8
  %arr.elem1858 = getelementptr i64, ptr %arr.data1857, i64 13
  store i64 0, ptr %arr.elem1858, align 8
  %arr.data.gep1859 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1860 = load ptr, ptr %arr.data.gep1859, align 8
  %arr.elem1861 = getelementptr i64, ptr %arr.data1860, i64 14
  store i64 0, ptr %arr.elem1861, align 8
  %arr.data.gep1862 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1863 = load ptr, ptr %arr.data.gep1862, align 8
  %arr.elem1864 = getelementptr i64, ptr %arr.data1863, i64 15
  store i64 0, ptr %arr.elem1864, align 8
  %arr.data.gep1865 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1866 = load ptr, ptr %arr.data.gep1865, align 8
  %arr.elem1867 = getelementptr i64, ptr %arr.data1866, i64 16
  store i64 0, ptr %arr.elem1867, align 8
  %arr.data.gep1868 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1869 = load ptr, ptr %arr.data.gep1868, align 8
  %arr.elem1870 = getelementptr i64, ptr %arr.data1869, i64 17
  store i64 0, ptr %arr.elem1870, align 8
  %arr.data.gep1871 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1872 = load ptr, ptr %arr.data.gep1871, align 8
  %arr.elem1873 = getelementptr i64, ptr %arr.data1872, i64 18
  store i64 0, ptr %arr.elem1873, align 8
  %arr.data.gep1874 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1875 = load ptr, ptr %arr.data.gep1874, align 8
  %arr.elem1876 = getelementptr i64, ptr %arr.data1875, i64 19
  store i64 0, ptr %arr.elem1876, align 8
  %arr.data.gep1877 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1878 = load ptr, ptr %arr.data.gep1877, align 8
  %arr.elem1879 = getelementptr i64, ptr %arr.data1878, i64 20
  store i64 0, ptr %arr.elem1879, align 8
  %arr.data.gep1880 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1881 = load ptr, ptr %arr.data.gep1880, align 8
  %arr.elem1882 = getelementptr i64, ptr %arr.data1881, i64 21
  store i64 0, ptr %arr.elem1882, align 8
  %arr.data.gep1883 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1884 = load ptr, ptr %arr.data.gep1883, align 8
  %arr.elem1885 = getelementptr i64, ptr %arr.data1884, i64 22
  store i64 0, ptr %arr.elem1885, align 8
  %arr.data.gep1886 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1887 = load ptr, ptr %arr.data.gep1886, align 8
  %arr.elem1888 = getelementptr i64, ptr %arr.data1887, i64 23
  store i64 0, ptr %arr.elem1888, align 8
  %arr.data.gep1889 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1890 = load ptr, ptr %arr.data.gep1889, align 8
  %arr.elem1891 = getelementptr i64, ptr %arr.data1890, i64 24
  store i64 0, ptr %arr.elem1891, align 8
  %arr.data.gep1892 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1893 = load ptr, ptr %arr.data.gep1892, align 8
  %arr.elem1894 = getelementptr i64, ptr %arr.data1893, i64 25
  store i64 0, ptr %arr.elem1894, align 8
  %arr.data.gep1895 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1896 = load ptr, ptr %arr.data.gep1895, align 8
  %arr.elem1897 = getelementptr i64, ptr %arr.data1896, i64 26
  store i64 0, ptr %arr.elem1897, align 8
  %arr.data.gep1898 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1899 = load ptr, ptr %arr.data.gep1898, align 8
  %arr.elem1900 = getelementptr i64, ptr %arr.data1899, i64 27
  store i64 0, ptr %arr.elem1900, align 8
  %arr.data.gep1901 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1902 = load ptr, ptr %arr.data.gep1901, align 8
  %arr.elem1903 = getelementptr i64, ptr %arr.data1902, i64 28
  store i64 0, ptr %arr.elem1903, align 8
  %arr.data.gep1904 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1905 = load ptr, ptr %arr.data.gep1904, align 8
  %arr.elem1906 = getelementptr i64, ptr %arr.data1905, i64 29
  store i64 0, ptr %arr.elem1906, align 8
  %arr.data.gep1907 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1908 = load ptr, ptr %arr.data.gep1907, align 8
  %arr.elem1909 = getelementptr i64, ptr %arr.data1908, i64 30
  store i64 0, ptr %arr.elem1909, align 8
  %arr.data.gep1910 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1911 = load ptr, ptr %arr.data.gep1910, align 8
  %arr.elem1912 = getelementptr i64, ptr %arr.data1911, i64 31
  store i64 0, ptr %arr.elem1912, align 8
  %arr.data.gep1913 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1914 = load ptr, ptr %arr.data.gep1913, align 8
  %arr.elem1915 = getelementptr i64, ptr %arr.data1914, i64 32
  store i64 0, ptr %arr.elem1915, align 8
  %arr.data.gep1916 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1917 = load ptr, ptr %arr.data.gep1916, align 8
  %arr.elem1918 = getelementptr i64, ptr %arr.data1917, i64 33
  store i64 0, ptr %arr.elem1918, align 8
  %arr.data.gep1919 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1920 = load ptr, ptr %arr.data.gep1919, align 8
  %arr.elem1921 = getelementptr i64, ptr %arr.data1920, i64 34
  store i64 0, ptr %arr.elem1921, align 8
  %arr.data.gep1922 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1923 = load ptr, ptr %arr.data.gep1922, align 8
  %arr.elem1924 = getelementptr i64, ptr %arr.data1923, i64 35
  store i64 0, ptr %arr.elem1924, align 8
  %arr.data.gep1925 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1926 = load ptr, ptr %arr.data.gep1925, align 8
  %arr.elem1927 = getelementptr i64, ptr %arr.data1926, i64 36
  store i64 0, ptr %arr.elem1927, align 8
  %arr.data.gep1928 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1929 = load ptr, ptr %arr.data.gep1928, align 8
  %arr.elem1930 = getelementptr i64, ptr %arr.data1929, i64 37
  store i64 0, ptr %arr.elem1930, align 8
  %arr.data.gep1931 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1932 = load ptr, ptr %arr.data.gep1931, align 8
  %arr.elem1933 = getelementptr i64, ptr %arr.data1932, i64 38
  store i64 0, ptr %arr.elem1933, align 8
  %arr.data.gep1934 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1935 = load ptr, ptr %arr.data.gep1934, align 8
  %arr.elem1936 = getelementptr i64, ptr %arr.data1935, i64 39
  store i64 0, ptr %arr.elem1936, align 8
  %arr.data.gep1937 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1938 = load ptr, ptr %arr.data.gep1937, align 8
  %arr.elem1939 = getelementptr i64, ptr %arr.data1938, i64 40
  store i64 0, ptr %arr.elem1939, align 8
  %arr.data.gep1940 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1941 = load ptr, ptr %arr.data.gep1940, align 8
  %arr.elem1942 = getelementptr i64, ptr %arr.data1941, i64 41
  store i64 0, ptr %arr.elem1942, align 8
  %arr.data.gep1943 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1944 = load ptr, ptr %arr.data.gep1943, align 8
  %arr.elem1945 = getelementptr i64, ptr %arr.data1944, i64 42
  store i64 0, ptr %arr.elem1945, align 8
  %arr.data.gep1946 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1947 = load ptr, ptr %arr.data.gep1946, align 8
  %arr.elem1948 = getelementptr i64, ptr %arr.data1947, i64 43
  store i64 0, ptr %arr.elem1948, align 8
  %arr.data.gep1949 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1950 = load ptr, ptr %arr.data.gep1949, align 8
  %arr.elem1951 = getelementptr i64, ptr %arr.data1950, i64 44
  store i64 0, ptr %arr.elem1951, align 8
  %arr.data.gep1952 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1953 = load ptr, ptr %arr.data.gep1952, align 8
  %arr.elem1954 = getelementptr i64, ptr %arr.data1953, i64 45
  store i64 0, ptr %arr.elem1954, align 8
  %arr.data.gep1955 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1956 = load ptr, ptr %arr.data.gep1955, align 8
  %arr.elem1957 = getelementptr i64, ptr %arr.data1956, i64 46
  store i64 0, ptr %arr.elem1957, align 8
  %arr.data.gep1958 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1959 = load ptr, ptr %arr.data.gep1958, align 8
  %arr.elem1960 = getelementptr i64, ptr %arr.data1959, i64 47
  store i64 0, ptr %arr.elem1960, align 8
  %arr.data.gep1961 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1962 = load ptr, ptr %arr.data.gep1961, align 8
  %arr.elem1963 = getelementptr i64, ptr %arr.data1962, i64 48
  store i64 0, ptr %arr.elem1963, align 8
  %arr.data.gep1964 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1965 = load ptr, ptr %arr.data.gep1964, align 8
  %arr.elem1966 = getelementptr i64, ptr %arr.data1965, i64 49
  store i64 0, ptr %arr.elem1966, align 8
  %arr.data.gep1967 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1968 = load ptr, ptr %arr.data.gep1967, align 8
  %arr.elem1969 = getelementptr i64, ptr %arr.data1968, i64 50
  store i64 0, ptr %arr.elem1969, align 8
  %arr.data.gep1970 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1971 = load ptr, ptr %arr.data.gep1970, align 8
  %arr.elem1972 = getelementptr i64, ptr %arr.data1971, i64 51
  store i64 0, ptr %arr.elem1972, align 8
  %arr.data.gep1973 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1974 = load ptr, ptr %arr.data.gep1973, align 8
  %arr.elem1975 = getelementptr i64, ptr %arr.data1974, i64 52
  store i64 0, ptr %arr.elem1975, align 8
  %arr.data.gep1976 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1977 = load ptr, ptr %arr.data.gep1976, align 8
  %arr.elem1978 = getelementptr i64, ptr %arr.data1977, i64 53
  store i64 0, ptr %arr.elem1978, align 8
  %arr.data.gep1979 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1980 = load ptr, ptr %arr.data.gep1979, align 8
  %arr.elem1981 = getelementptr i64, ptr %arr.data1980, i64 54
  store i64 0, ptr %arr.elem1981, align 8
  %arr.data.gep1982 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1983 = load ptr, ptr %arr.data.gep1982, align 8
  %arr.elem1984 = getelementptr i64, ptr %arr.data1983, i64 55
  store i64 0, ptr %arr.elem1984, align 8
  %arr.data.gep1985 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1986 = load ptr, ptr %arr.data.gep1985, align 8
  %arr.elem1987 = getelementptr i64, ptr %arr.data1986, i64 56
  store i64 0, ptr %arr.elem1987, align 8
  %arr.data.gep1988 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1989 = load ptr, ptr %arr.data.gep1988, align 8
  %arr.elem1990 = getelementptr i64, ptr %arr.data1989, i64 57
  store i64 0, ptr %arr.elem1990, align 8
  %arr.data.gep1991 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1992 = load ptr, ptr %arr.data.gep1991, align 8
  %arr.elem1993 = getelementptr i64, ptr %arr.data1992, i64 58
  store i64 0, ptr %arr.elem1993, align 8
  %arr.data.gep1994 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1995 = load ptr, ptr %arr.data.gep1994, align 8
  %arr.elem1996 = getelementptr i64, ptr %arr.data1995, i64 59
  store i64 0, ptr %arr.elem1996, align 8
  %arr.data.gep1997 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data1998 = load ptr, ptr %arr.data.gep1997, align 8
  %arr.elem1999 = getelementptr i64, ptr %arr.data1998, i64 60
  store i64 0, ptr %arr.elem1999, align 8
  %arr.data.gep2000 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data2001 = load ptr, ptr %arr.data.gep2000, align 8
  %arr.elem2002 = getelementptr i64, ptr %arr.data2001, i64 61
  store i64 0, ptr %arr.elem2002, align 8
  %arr.data.gep2003 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data2004 = load ptr, ptr %arr.data.gep2003, align 8
  %arr.elem2005 = getelementptr i64, ptr %arr.data2004, i64 62
  store i64 0, ptr %arr.elem2005, align 8
  %arr.data.gep2006 = getelementptr inbounds %SadArray, ptr %arr_new1812, i32 0, i32 2
  %arr.data2007 = load ptr, ptr %arr.data.gep2006, align 8
  %arr.elem2008 = getelementptr i64, ptr %arr.data2007, i64 63
  store i64 0, ptr %arr.elem2008, align 8
  %obj.ptrtoint2009 = ptrtoint ptr %arr_new1812 to i64
  store i64 %obj.ptrtoint2009, ptr %"%wdg_\D8\AD\D8\B1\D9\81", align 8
  %arr_new2010 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2011 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 0
  store i64 64, ptr %arr.len.gep2011, align 8
  %arr.cap.gep2012 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep2012, align 8
  %arr.data2013 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
  %arr.data.gep2014 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  store ptr %arr.data2013, ptr %arr.data.gep2014, align 8
  %arr.data.gep2015 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2016 = load ptr, ptr %arr.data.gep2015, align 8
  %arr.elem2017 = getelementptr i64, ptr %arr.data2016, i64 0
  store i64 0, ptr %arr.elem2017, align 8
  %arr.data.gep2018 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2019 = load ptr, ptr %arr.data.gep2018, align 8
  %arr.elem2020 = getelementptr i64, ptr %arr.data2019, i64 1
  store i64 0, ptr %arr.elem2020, align 8
  %arr.data.gep2021 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2022 = load ptr, ptr %arr.data.gep2021, align 8
  %arr.elem2023 = getelementptr i64, ptr %arr.data2022, i64 2
  store i64 0, ptr %arr.elem2023, align 8
  %arr.data.gep2024 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2025 = load ptr, ptr %arr.data.gep2024, align 8
  %arr.elem2026 = getelementptr i64, ptr %arr.data2025, i64 3
  store i64 0, ptr %arr.elem2026, align 8
  %arr.data.gep2027 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2028 = load ptr, ptr %arr.data.gep2027, align 8
  %arr.elem2029 = getelementptr i64, ptr %arr.data2028, i64 4
  store i64 0, ptr %arr.elem2029, align 8
  %arr.data.gep2030 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2031 = load ptr, ptr %arr.data.gep2030, align 8
  %arr.elem2032 = getelementptr i64, ptr %arr.data2031, i64 5
  store i64 0, ptr %arr.elem2032, align 8
  %arr.data.gep2033 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2034 = load ptr, ptr %arr.data.gep2033, align 8
  %arr.elem2035 = getelementptr i64, ptr %arr.data2034, i64 6
  store i64 0, ptr %arr.elem2035, align 8
  %arr.data.gep2036 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2037 = load ptr, ptr %arr.data.gep2036, align 8
  %arr.elem2038 = getelementptr i64, ptr %arr.data2037, i64 7
  store i64 0, ptr %arr.elem2038, align 8
  %arr.data.gep2039 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2040 = load ptr, ptr %arr.data.gep2039, align 8
  %arr.elem2041 = getelementptr i64, ptr %arr.data2040, i64 8
  store i64 0, ptr %arr.elem2041, align 8
  %arr.data.gep2042 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2043 = load ptr, ptr %arr.data.gep2042, align 8
  %arr.elem2044 = getelementptr i64, ptr %arr.data2043, i64 9
  store i64 0, ptr %arr.elem2044, align 8
  %arr.data.gep2045 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2046 = load ptr, ptr %arr.data.gep2045, align 8
  %arr.elem2047 = getelementptr i64, ptr %arr.data2046, i64 10
  store i64 0, ptr %arr.elem2047, align 8
  %arr.data.gep2048 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2049 = load ptr, ptr %arr.data.gep2048, align 8
  %arr.elem2050 = getelementptr i64, ptr %arr.data2049, i64 11
  store i64 0, ptr %arr.elem2050, align 8
  %arr.data.gep2051 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2052 = load ptr, ptr %arr.data.gep2051, align 8
  %arr.elem2053 = getelementptr i64, ptr %arr.data2052, i64 12
  store i64 0, ptr %arr.elem2053, align 8
  %arr.data.gep2054 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2055 = load ptr, ptr %arr.data.gep2054, align 8
  %arr.elem2056 = getelementptr i64, ptr %arr.data2055, i64 13
  store i64 0, ptr %arr.elem2056, align 8
  %arr.data.gep2057 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2058 = load ptr, ptr %arr.data.gep2057, align 8
  %arr.elem2059 = getelementptr i64, ptr %arr.data2058, i64 14
  store i64 0, ptr %arr.elem2059, align 8
  %arr.data.gep2060 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2061 = load ptr, ptr %arr.data.gep2060, align 8
  %arr.elem2062 = getelementptr i64, ptr %arr.data2061, i64 15
  store i64 0, ptr %arr.elem2062, align 8
  %arr.data.gep2063 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2064 = load ptr, ptr %arr.data.gep2063, align 8
  %arr.elem2065 = getelementptr i64, ptr %arr.data2064, i64 16
  store i64 0, ptr %arr.elem2065, align 8
  %arr.data.gep2066 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2067 = load ptr, ptr %arr.data.gep2066, align 8
  %arr.elem2068 = getelementptr i64, ptr %arr.data2067, i64 17
  store i64 0, ptr %arr.elem2068, align 8
  %arr.data.gep2069 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2070 = load ptr, ptr %arr.data.gep2069, align 8
  %arr.elem2071 = getelementptr i64, ptr %arr.data2070, i64 18
  store i64 0, ptr %arr.elem2071, align 8
  %arr.data.gep2072 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2073 = load ptr, ptr %arr.data.gep2072, align 8
  %arr.elem2074 = getelementptr i64, ptr %arr.data2073, i64 19
  store i64 0, ptr %arr.elem2074, align 8
  %arr.data.gep2075 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2076 = load ptr, ptr %arr.data.gep2075, align 8
  %arr.elem2077 = getelementptr i64, ptr %arr.data2076, i64 20
  store i64 0, ptr %arr.elem2077, align 8
  %arr.data.gep2078 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2079 = load ptr, ptr %arr.data.gep2078, align 8
  %arr.elem2080 = getelementptr i64, ptr %arr.data2079, i64 21
  store i64 0, ptr %arr.elem2080, align 8
  %arr.data.gep2081 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2082 = load ptr, ptr %arr.data.gep2081, align 8
  %arr.elem2083 = getelementptr i64, ptr %arr.data2082, i64 22
  store i64 0, ptr %arr.elem2083, align 8
  %arr.data.gep2084 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2085 = load ptr, ptr %arr.data.gep2084, align 8
  %arr.elem2086 = getelementptr i64, ptr %arr.data2085, i64 23
  store i64 0, ptr %arr.elem2086, align 8
  %arr.data.gep2087 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2088 = load ptr, ptr %arr.data.gep2087, align 8
  %arr.elem2089 = getelementptr i64, ptr %arr.data2088, i64 24
  store i64 0, ptr %arr.elem2089, align 8
  %arr.data.gep2090 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2091 = load ptr, ptr %arr.data.gep2090, align 8
  %arr.elem2092 = getelementptr i64, ptr %arr.data2091, i64 25
  store i64 0, ptr %arr.elem2092, align 8
  %arr.data.gep2093 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2094 = load ptr, ptr %arr.data.gep2093, align 8
  %arr.elem2095 = getelementptr i64, ptr %arr.data2094, i64 26
  store i64 0, ptr %arr.elem2095, align 8
  %arr.data.gep2096 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2097 = load ptr, ptr %arr.data.gep2096, align 8
  %arr.elem2098 = getelementptr i64, ptr %arr.data2097, i64 27
  store i64 0, ptr %arr.elem2098, align 8
  %arr.data.gep2099 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2100 = load ptr, ptr %arr.data.gep2099, align 8
  %arr.elem2101 = getelementptr i64, ptr %arr.data2100, i64 28
  store i64 0, ptr %arr.elem2101, align 8
  %arr.data.gep2102 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2103 = load ptr, ptr %arr.data.gep2102, align 8
  %arr.elem2104 = getelementptr i64, ptr %arr.data2103, i64 29
  store i64 0, ptr %arr.elem2104, align 8
  %arr.data.gep2105 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2106 = load ptr, ptr %arr.data.gep2105, align 8
  %arr.elem2107 = getelementptr i64, ptr %arr.data2106, i64 30
  store i64 0, ptr %arr.elem2107, align 8
  %arr.data.gep2108 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2109 = load ptr, ptr %arr.data.gep2108, align 8
  %arr.elem2110 = getelementptr i64, ptr %arr.data2109, i64 31
  store i64 0, ptr %arr.elem2110, align 8
  %arr.data.gep2111 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2112 = load ptr, ptr %arr.data.gep2111, align 8
  %arr.elem2113 = getelementptr i64, ptr %arr.data2112, i64 32
  store i64 0, ptr %arr.elem2113, align 8
  %arr.data.gep2114 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2115 = load ptr, ptr %arr.data.gep2114, align 8
  %arr.elem2116 = getelementptr i64, ptr %arr.data2115, i64 33
  store i64 0, ptr %arr.elem2116, align 8
  %arr.data.gep2117 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2118 = load ptr, ptr %arr.data.gep2117, align 8
  %arr.elem2119 = getelementptr i64, ptr %arr.data2118, i64 34
  store i64 0, ptr %arr.elem2119, align 8
  %arr.data.gep2120 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2121 = load ptr, ptr %arr.data.gep2120, align 8
  %arr.elem2122 = getelementptr i64, ptr %arr.data2121, i64 35
  store i64 0, ptr %arr.elem2122, align 8
  %arr.data.gep2123 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2124 = load ptr, ptr %arr.data.gep2123, align 8
  %arr.elem2125 = getelementptr i64, ptr %arr.data2124, i64 36
  store i64 0, ptr %arr.elem2125, align 8
  %arr.data.gep2126 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2127 = load ptr, ptr %arr.data.gep2126, align 8
  %arr.elem2128 = getelementptr i64, ptr %arr.data2127, i64 37
  store i64 0, ptr %arr.elem2128, align 8
  %arr.data.gep2129 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2130 = load ptr, ptr %arr.data.gep2129, align 8
  %arr.elem2131 = getelementptr i64, ptr %arr.data2130, i64 38
  store i64 0, ptr %arr.elem2131, align 8
  %arr.data.gep2132 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2133 = load ptr, ptr %arr.data.gep2132, align 8
  %arr.elem2134 = getelementptr i64, ptr %arr.data2133, i64 39
  store i64 0, ptr %arr.elem2134, align 8
  %arr.data.gep2135 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2136 = load ptr, ptr %arr.data.gep2135, align 8
  %arr.elem2137 = getelementptr i64, ptr %arr.data2136, i64 40
  store i64 0, ptr %arr.elem2137, align 8
  %arr.data.gep2138 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2139 = load ptr, ptr %arr.data.gep2138, align 8
  %arr.elem2140 = getelementptr i64, ptr %arr.data2139, i64 41
  store i64 0, ptr %arr.elem2140, align 8
  %arr.data.gep2141 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2142 = load ptr, ptr %arr.data.gep2141, align 8
  %arr.elem2143 = getelementptr i64, ptr %arr.data2142, i64 42
  store i64 0, ptr %arr.elem2143, align 8
  %arr.data.gep2144 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2145 = load ptr, ptr %arr.data.gep2144, align 8
  %arr.elem2146 = getelementptr i64, ptr %arr.data2145, i64 43
  store i64 0, ptr %arr.elem2146, align 8
  %arr.data.gep2147 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2148 = load ptr, ptr %arr.data.gep2147, align 8
  %arr.elem2149 = getelementptr i64, ptr %arr.data2148, i64 44
  store i64 0, ptr %arr.elem2149, align 8
  %arr.data.gep2150 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2151 = load ptr, ptr %arr.data.gep2150, align 8
  %arr.elem2152 = getelementptr i64, ptr %arr.data2151, i64 45
  store i64 0, ptr %arr.elem2152, align 8
  %arr.data.gep2153 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2154 = load ptr, ptr %arr.data.gep2153, align 8
  %arr.elem2155 = getelementptr i64, ptr %arr.data2154, i64 46
  store i64 0, ptr %arr.elem2155, align 8
  %arr.data.gep2156 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2157 = load ptr, ptr %arr.data.gep2156, align 8
  %arr.elem2158 = getelementptr i64, ptr %arr.data2157, i64 47
  store i64 0, ptr %arr.elem2158, align 8
  %arr.data.gep2159 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2160 = load ptr, ptr %arr.data.gep2159, align 8
  %arr.elem2161 = getelementptr i64, ptr %arr.data2160, i64 48
  store i64 0, ptr %arr.elem2161, align 8
  %arr.data.gep2162 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2163 = load ptr, ptr %arr.data.gep2162, align 8
  %arr.elem2164 = getelementptr i64, ptr %arr.data2163, i64 49
  store i64 0, ptr %arr.elem2164, align 8
  %arr.data.gep2165 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2166 = load ptr, ptr %arr.data.gep2165, align 8
  %arr.elem2167 = getelementptr i64, ptr %arr.data2166, i64 50
  store i64 0, ptr %arr.elem2167, align 8
  %arr.data.gep2168 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2169 = load ptr, ptr %arr.data.gep2168, align 8
  %arr.elem2170 = getelementptr i64, ptr %arr.data2169, i64 51
  store i64 0, ptr %arr.elem2170, align 8
  %arr.data.gep2171 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2172 = load ptr, ptr %arr.data.gep2171, align 8
  %arr.elem2173 = getelementptr i64, ptr %arr.data2172, i64 52
  store i64 0, ptr %arr.elem2173, align 8
  %arr.data.gep2174 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2175 = load ptr, ptr %arr.data.gep2174, align 8
  %arr.elem2176 = getelementptr i64, ptr %arr.data2175, i64 53
  store i64 0, ptr %arr.elem2176, align 8
  %arr.data.gep2177 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2178 = load ptr, ptr %arr.data.gep2177, align 8
  %arr.elem2179 = getelementptr i64, ptr %arr.data2178, i64 54
  store i64 0, ptr %arr.elem2179, align 8
  %arr.data.gep2180 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2181 = load ptr, ptr %arr.data.gep2180, align 8
  %arr.elem2182 = getelementptr i64, ptr %arr.data2181, i64 55
  store i64 0, ptr %arr.elem2182, align 8
  %arr.data.gep2183 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2184 = load ptr, ptr %arr.data.gep2183, align 8
  %arr.elem2185 = getelementptr i64, ptr %arr.data2184, i64 56
  store i64 0, ptr %arr.elem2185, align 8
  %arr.data.gep2186 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2187 = load ptr, ptr %arr.data.gep2186, align 8
  %arr.elem2188 = getelementptr i64, ptr %arr.data2187, i64 57
  store i64 0, ptr %arr.elem2188, align 8
  %arr.data.gep2189 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2190 = load ptr, ptr %arr.data.gep2189, align 8
  %arr.elem2191 = getelementptr i64, ptr %arr.data2190, i64 58
  store i64 0, ptr %arr.elem2191, align 8
  %arr.data.gep2192 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2193 = load ptr, ptr %arr.data.gep2192, align 8
  %arr.elem2194 = getelementptr i64, ptr %arr.data2193, i64 59
  store i64 0, ptr %arr.elem2194, align 8
  %arr.data.gep2195 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2196 = load ptr, ptr %arr.data.gep2195, align 8
  %arr.elem2197 = getelementptr i64, ptr %arr.data2196, i64 60
  store i64 0, ptr %arr.elem2197, align 8
  %arr.data.gep2198 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2199 = load ptr, ptr %arr.data.gep2198, align 8
  %arr.elem2200 = getelementptr i64, ptr %arr.data2199, i64 61
  store i64 0, ptr %arr.elem2200, align 8
  %arr.data.gep2201 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2202 = load ptr, ptr %arr.data.gep2201, align 8
  %arr.elem2203 = getelementptr i64, ptr %arr.data2202, i64 62
  store i64 0, ptr %arr.elem2203, align 8
  %arr.data.gep2204 = getelementptr inbounds %SadArray, ptr %arr_new2010, i32 0, i32 2
  %arr.data2205 = load ptr, ptr %arr.data.gep2204, align 8
  %arr.elem2206 = getelementptr i64, ptr %arr.data2205, i64 63
  store i64 0, ptr %arr.elem2206, align 8
  %obj.ptrtoint2207 = ptrtoint ptr %arr_new2010 to i64
  store i64 %obj.ptrtoint2207, ptr %"%wdg_\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  store i64 0, ptr %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\AA\D9\83\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\AB\D9\88\D8\A7\D9\86", align 8
  store i64 0, ptr %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\AF\D9\82\D8\A7\D8\A6\D9\82", align 8
  store i64 12, ptr %"%\D8\B3\D8\A7\D8\B9\D8\A9_\D8\B3\D8\A7\D8\B9\D8\A7\D8\AA", align 8
  store i64 6, ptr %"%MAX_TRAY", align 8
  %arr_new2208 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2209 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 0
  store i64 6, ptr %arr.len.gep2209, align 8
  %arr.cap.gep2210 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 1
  store i64 6, ptr %arr.cap.gep2210, align 8
  %arr.data2211 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 6))
  %arr.data.gep2212 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 2
  store ptr %arr.data2211, ptr %arr.data.gep2212, align 8
  %arr.data.gep2213 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 2
  %arr.data2214 = load ptr, ptr %arr.data.gep2213, align 8
  %arr.elem2215 = getelementptr i64, ptr %arr.data2214, i64 0
  store i64 0, ptr %arr.elem2215, align 8
  %arr.data.gep2216 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 2
  %arr.data2217 = load ptr, ptr %arr.data.gep2216, align 8
  %arr.elem2218 = getelementptr i64, ptr %arr.data2217, i64 1
  store i64 0, ptr %arr.elem2218, align 8
  %arr.data.gep2219 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 2
  %arr.data2220 = load ptr, ptr %arr.data.gep2219, align 8
  %arr.elem2221 = getelementptr i64, ptr %arr.data2220, i64 2
  store i64 0, ptr %arr.elem2221, align 8
  %arr.data.gep2222 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 2
  %arr.data2223 = load ptr, ptr %arr.data.gep2222, align 8
  %arr.elem2224 = getelementptr i64, ptr %arr.data2223, i64 3
  store i64 0, ptr %arr.elem2224, align 8
  %arr.data.gep2225 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 2
  %arr.data2226 = load ptr, ptr %arr.data.gep2225, align 8
  %arr.elem2227 = getelementptr i64, ptr %arr.data2226, i64 4
  store i64 0, ptr %arr.elem2227, align 8
  %arr.data.gep2228 = getelementptr inbounds %SadArray, ptr %arr_new2208, i32 0, i32 2
  %arr.data2229 = load ptr, ptr %arr.data.gep2228, align 8
  %arr.elem2230 = getelementptr i64, ptr %arr.data2229, i64 5
  store i64 0, ptr %arr.elem2230, align 8
  %obj.ptrtoint2231 = ptrtoint ptr %arr_new2208 to i64
  store i64 %obj.ptrtoint2231, ptr %"%tray_\D9\86\D8\B4\D8\B7", align 8
  %arr_new2232 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2233 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 0
  store i64 6, ptr %arr.len.gep2233, align 8
  %arr.cap.gep2234 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 1
  store i64 6, ptr %arr.cap.gep2234, align 8
  %arr.data2235 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 6))
  %arr.data.gep2236 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 2
  store ptr %arr.data2235, ptr %arr.data.gep2236, align 8
  %arr.data.gep2237 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 2
  %arr.data2238 = load ptr, ptr %arr.data.gep2237, align 8
  %arr.elem2239 = getelementptr i64, ptr %arr.data2238, i64 0
  store i64 0, ptr %arr.elem2239, align 8
  %arr.data.gep2240 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 2
  %arr.data2241 = load ptr, ptr %arr.data.gep2240, align 8
  %arr.elem2242 = getelementptr i64, ptr %arr.data2241, i64 1
  store i64 0, ptr %arr.elem2242, align 8
  %arr.data.gep2243 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 2
  %arr.data2244 = load ptr, ptr %arr.data.gep2243, align 8
  %arr.elem2245 = getelementptr i64, ptr %arr.data2244, i64 2
  store i64 0, ptr %arr.elem2245, align 8
  %arr.data.gep2246 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 2
  %arr.data2247 = load ptr, ptr %arr.data.gep2246, align 8
  %arr.elem2248 = getelementptr i64, ptr %arr.data2247, i64 3
  store i64 0, ptr %arr.elem2248, align 8
  %arr.data.gep2249 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 2
  %arr.data2250 = load ptr, ptr %arr.data.gep2249, align 8
  %arr.elem2251 = getelementptr i64, ptr %arr.data2250, i64 4
  store i64 0, ptr %arr.elem2251, align 8
  %arr.data.gep2252 = getelementptr inbounds %SadArray, ptr %arr_new2232, i32 0, i32 2
  %arr.data2253 = load ptr, ptr %arr.data.gep2252, align 8
  %arr.elem2254 = getelementptr i64, ptr %arr.data2253, i64 5
  store i64 0, ptr %arr.elem2254, align 8
  %obj.ptrtoint2255 = ptrtoint ptr %arr_new2232 to i64
  store i64 %obj.ptrtoint2255, ptr %"%tray_\D8\AD\D8\B1\D9\81", align 8
  store i64 4, ptr %"%MAX_NOTIFICATIONS", align 8
  store i64 200, ptr %"%NOTIF_W", align 8
  store i64 40, ptr %"%NOTIF_H", align 8
  store i64 200, ptr %"%NOTIF_DURATION", align 8
  %arr_new2256 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2257 = getelementptr inbounds %SadArray, ptr %arr_new2256, i32 0, i32 0
  store i64 4, ptr %arr.len.gep2257, align 8
  %arr.cap.gep2258 = getelementptr inbounds %SadArray, ptr %arr_new2256, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep2258, align 8
  %arr.data2259 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep2260 = getelementptr inbounds %SadArray, ptr %arr_new2256, i32 0, i32 2
  store ptr %arr.data2259, ptr %arr.data.gep2260, align 8
  %arr.data.gep2261 = getelementptr inbounds %SadArray, ptr %arr_new2256, i32 0, i32 2
  %arr.data2262 = load ptr, ptr %arr.data.gep2261, align 8
  %arr.elem2263 = getelementptr i64, ptr %arr.data2262, i64 0
  store i64 0, ptr %arr.elem2263, align 8
  %arr.data.gep2264 = getelementptr inbounds %SadArray, ptr %arr_new2256, i32 0, i32 2
  %arr.data2265 = load ptr, ptr %arr.data.gep2264, align 8
  %arr.elem2266 = getelementptr i64, ptr %arr.data2265, i64 1
  store i64 0, ptr %arr.elem2266, align 8
  %arr.data.gep2267 = getelementptr inbounds %SadArray, ptr %arr_new2256, i32 0, i32 2
  %arr.data2268 = load ptr, ptr %arr.data.gep2267, align 8
  %arr.elem2269 = getelementptr i64, ptr %arr.data2268, i64 2
  store i64 0, ptr %arr.elem2269, align 8
  %arr.data.gep2270 = getelementptr inbounds %SadArray, ptr %arr_new2256, i32 0, i32 2
  %arr.data2271 = load ptr, ptr %arr.data.gep2270, align 8
  %arr.elem2272 = getelementptr i64, ptr %arr.data2271, i64 3
  store i64 0, ptr %arr.elem2272, align 8
  %obj.ptrtoint2273 = ptrtoint ptr %arr_new2256 to i64
  store i64 %obj.ptrtoint2273, ptr %"%notif_\D9\86\D8\B4\D8\B7", align 8
  %arr_new2274 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2275 = getelementptr inbounds %SadArray, ptr %arr_new2274, i32 0, i32 0
  store i64 4, ptr %arr.len.gep2275, align 8
  %arr.cap.gep2276 = getelementptr inbounds %SadArray, ptr %arr_new2274, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep2276, align 8
  %arr.data2277 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep2278 = getelementptr inbounds %SadArray, ptr %arr_new2274, i32 0, i32 2
  store ptr %arr.data2277, ptr %arr.data.gep2278, align 8
  %arr.data.gep2279 = getelementptr inbounds %SadArray, ptr %arr_new2274, i32 0, i32 2
  %arr.data2280 = load ptr, ptr %arr.data.gep2279, align 8
  %arr.elem2281 = getelementptr i64, ptr %arr.data2280, i64 0
  store i64 0, ptr %arr.elem2281, align 8
  %arr.data.gep2282 = getelementptr inbounds %SadArray, ptr %arr_new2274, i32 0, i32 2
  %arr.data2283 = load ptr, ptr %arr.data.gep2282, align 8
  %arr.elem2284 = getelementptr i64, ptr %arr.data2283, i64 1
  store i64 0, ptr %arr.elem2284, align 8
  %arr.data.gep2285 = getelementptr inbounds %SadArray, ptr %arr_new2274, i32 0, i32 2
  %arr.data2286 = load ptr, ptr %arr.data.gep2285, align 8
  %arr.elem2287 = getelementptr i64, ptr %arr.data2286, i64 2
  store i64 0, ptr %arr.elem2287, align 8
  %arr.data.gep2288 = getelementptr inbounds %SadArray, ptr %arr_new2274, i32 0, i32 2
  %arr.data2289 = load ptr, ptr %arr.data.gep2288, align 8
  %arr.elem2290 = getelementptr i64, ptr %arr.data2289, i64 3
  store i64 0, ptr %arr.elem2290, align 8
  %obj.ptrtoint2291 = ptrtoint ptr %arr_new2274 to i64
  store i64 %obj.ptrtoint2291, ptr %"%notif_\D8\AD\D8\B1\D9\81", align 8
  %arr_new2292 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2293 = getelementptr inbounds %SadArray, ptr %arr_new2292, i32 0, i32 0
  store i64 4, ptr %arr.len.gep2293, align 8
  %arr.cap.gep2294 = getelementptr inbounds %SadArray, ptr %arr_new2292, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep2294, align 8
  %arr.data2295 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep2296 = getelementptr inbounds %SadArray, ptr %arr_new2292, i32 0, i32 2
  store ptr %arr.data2295, ptr %arr.data.gep2296, align 8
  %arr.data.gep2297 = getelementptr inbounds %SadArray, ptr %arr_new2292, i32 0, i32 2
  %arr.data2298 = load ptr, ptr %arr.data.gep2297, align 8
  %arr.elem2299 = getelementptr i64, ptr %arr.data2298, i64 0
  store i64 0, ptr %arr.elem2299, align 8
  %arr.data.gep2300 = getelementptr inbounds %SadArray, ptr %arr_new2292, i32 0, i32 2
  %arr.data2301 = load ptr, ptr %arr.data.gep2300, align 8
  %arr.elem2302 = getelementptr i64, ptr %arr.data2301, i64 1
  store i64 0, ptr %arr.elem2302, align 8
  %arr.data.gep2303 = getelementptr inbounds %SadArray, ptr %arr_new2292, i32 0, i32 2
  %arr.data2304 = load ptr, ptr %arr.data.gep2303, align 8
  %arr.elem2305 = getelementptr i64, ptr %arr.data2304, i64 2
  store i64 0, ptr %arr.elem2305, align 8
  %arr.data.gep2306 = getelementptr inbounds %SadArray, ptr %arr_new2292, i32 0, i32 2
  %arr.data2307 = load ptr, ptr %arr.data.gep2306, align 8
  %arr.elem2308 = getelementptr i64, ptr %arr.data2307, i64 3
  store i64 0, ptr %arr.elem2308, align 8
  %obj.ptrtoint2309 = ptrtoint ptr %arr_new2292 to i64
  store i64 %obj.ptrtoint2309, ptr %"%notif_\D8\B9\D8\AF\D8\A7\D8\AF", align 8
  %arr_new2310 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2311 = getelementptr inbounds %SadArray, ptr %arr_new2310, i32 0, i32 0
  store i64 4, ptr %arr.len.gep2311, align 8
  %arr.cap.gep2312 = getelementptr inbounds %SadArray, ptr %arr_new2310, i32 0, i32 1
  store i64 4, ptr %arr.cap.gep2312, align 8
  %arr.data2313 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 4))
  %arr.data.gep2314 = getelementptr inbounds %SadArray, ptr %arr_new2310, i32 0, i32 2
  store ptr %arr.data2313, ptr %arr.data.gep2314, align 8
  %arr.data.gep2315 = getelementptr inbounds %SadArray, ptr %arr_new2310, i32 0, i32 2
  %arr.data2316 = load ptr, ptr %arr.data.gep2315, align 8
  %arr.elem2317 = getelementptr i64, ptr %arr.data2316, i64 0
  store i64 0, ptr %arr.elem2317, align 8
  %arr.data.gep2318 = getelementptr inbounds %SadArray, ptr %arr_new2310, i32 0, i32 2
  %arr.data2319 = load ptr, ptr %arr.data.gep2318, align 8
  %arr.elem2320 = getelementptr i64, ptr %arr.data2319, i64 1
  store i64 0, ptr %arr.elem2320, align 8
  %arr.data.gep2321 = getelementptr inbounds %SadArray, ptr %arr_new2310, i32 0, i32 2
  %arr.data2322 = load ptr, ptr %arr.data.gep2321, align 8
  %arr.elem2323 = getelementptr i64, ptr %arr.data2322, i64 2
  store i64 0, ptr %arr.elem2323, align 8
  %arr.data.gep2324 = getelementptr inbounds %SadArray, ptr %arr_new2310, i32 0, i32 2
  %arr.data2325 = load ptr, ptr %arr.data.gep2324, align 8
  %arr.elem2326 = getelementptr i64, ptr %arr.data2325, i64 3
  store i64 0, ptr %arr.elem2326, align 8
  %obj.ptrtoint2327 = ptrtoint ptr %arr_new2310 to i64
  store i64 %obj.ptrtoint2327, ptr %"%notif_\D9\84\D9\88\D9\86", align 8
  store i64 256, ptr %"%CLIPBOARD_SIZE", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\81\D8\B8\D8\A9_\D8\B7\D9\88\D9\84", align 8
  store i64 100, ptr %"%VKBD_X", align 8
  store i64 350, ptr %"%VKBD_Y", align 8
  store i64 600, ptr %"%VKBD_W", align 8
  store i64 200, ptr %"%VKBD_H", align 8
  store i64 40, ptr %"%VKBD_KEY_W", align 8
  store i64 36, ptr %"%VKBD_KEY_H", align 8
  store i64 14, ptr %"%VKBD_COLS", align 8
  store i64 4, ptr %"%VKBD_ROWS", align 8
  store i64 0, ptr %"%vkbd_\D9\85\D8\B1\D8\A6\D9\8A\D8\A9", align 8
  store i64 0, ptr %"%vkbd_shift", align 8
  store i64 0, ptr %"%vkbd_\D8\A2\D8\AE\D8\B1_\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %arr_new2328 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2329 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2329, align 8
  %arr.cap.gep2330 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2330, align 8
  %arr.data2331 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2332 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  store ptr %arr.data2331, ptr %arr.data.gep2332, align 8
  %arr.data.gep2333 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2334 = load ptr, ptr %arr.data.gep2333, align 8
  %arr.elem2335 = getelementptr i64, ptr %arr.data2334, i64 0
  store i64 49, ptr %arr.elem2335, align 8
  %arr.data.gep2336 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2337 = load ptr, ptr %arr.data.gep2336, align 8
  %arr.elem2338 = getelementptr i64, ptr %arr.data2337, i64 1
  store i64 50, ptr %arr.elem2338, align 8
  %arr.data.gep2339 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2340 = load ptr, ptr %arr.data.gep2339, align 8
  %arr.elem2341 = getelementptr i64, ptr %arr.data2340, i64 2
  store i64 51, ptr %arr.elem2341, align 8
  %arr.data.gep2342 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2343 = load ptr, ptr %arr.data.gep2342, align 8
  %arr.elem2344 = getelementptr i64, ptr %arr.data2343, i64 3
  store i64 52, ptr %arr.elem2344, align 8
  %arr.data.gep2345 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2346 = load ptr, ptr %arr.data.gep2345, align 8
  %arr.elem2347 = getelementptr i64, ptr %arr.data2346, i64 4
  store i64 53, ptr %arr.elem2347, align 8
  %arr.data.gep2348 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2349 = load ptr, ptr %arr.data.gep2348, align 8
  %arr.elem2350 = getelementptr i64, ptr %arr.data2349, i64 5
  store i64 54, ptr %arr.elem2350, align 8
  %arr.data.gep2351 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2352 = load ptr, ptr %arr.data.gep2351, align 8
  %arr.elem2353 = getelementptr i64, ptr %arr.data2352, i64 6
  store i64 55, ptr %arr.elem2353, align 8
  %arr.data.gep2354 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2355 = load ptr, ptr %arr.data.gep2354, align 8
  %arr.elem2356 = getelementptr i64, ptr %arr.data2355, i64 7
  store i64 56, ptr %arr.elem2356, align 8
  %arr.data.gep2357 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2358 = load ptr, ptr %arr.data.gep2357, align 8
  %arr.elem2359 = getelementptr i64, ptr %arr.data2358, i64 8
  store i64 57, ptr %arr.elem2359, align 8
  %arr.data.gep2360 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2361 = load ptr, ptr %arr.data.gep2360, align 8
  %arr.elem2362 = getelementptr i64, ptr %arr.data2361, i64 9
  store i64 48, ptr %arr.elem2362, align 8
  %arr.data.gep2363 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2364 = load ptr, ptr %arr.data.gep2363, align 8
  %arr.elem2365 = getelementptr i64, ptr %arr.data2364, i64 10
  store i64 45, ptr %arr.elem2365, align 8
  %arr.data.gep2366 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2367 = load ptr, ptr %arr.data.gep2366, align 8
  %arr.elem2368 = getelementptr i64, ptr %arr.data2367, i64 11
  store i64 61, ptr %arr.elem2368, align 8
  %arr.data.gep2369 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2370 = load ptr, ptr %arr.data.gep2369, align 8
  %arr.elem2371 = getelementptr i64, ptr %arr.data2370, i64 12
  store i64 8, ptr %arr.elem2371, align 8
  %arr.data.gep2372 = getelementptr inbounds %SadArray, ptr %arr_new2328, i32 0, i32 2
  %arr.data2373 = load ptr, ptr %arr.data.gep2372, align 8
  %arr.elem2374 = getelementptr i64, ptr %arr.data2373, i64 13
  store i64 127, ptr %arr.elem2374, align 8
  %obj.ptrtoint2375 = ptrtoint ptr %arr_new2328 to i64
  store i64 %obj.ptrtoint2375, ptr %"%vkbd_row0", align 8
  %arr_new2376 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2377 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2377, align 8
  %arr.cap.gep2378 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2378, align 8
  %arr.data2379 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2380 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  store ptr %arr.data2379, ptr %arr.data.gep2380, align 8
  %arr.data.gep2381 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2382 = load ptr, ptr %arr.data.gep2381, align 8
  %arr.elem2383 = getelementptr i64, ptr %arr.data2382, i64 0
  store i64 113, ptr %arr.elem2383, align 8
  %arr.data.gep2384 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2385 = load ptr, ptr %arr.data.gep2384, align 8
  %arr.elem2386 = getelementptr i64, ptr %arr.data2385, i64 1
  store i64 119, ptr %arr.elem2386, align 8
  %arr.data.gep2387 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2388 = load ptr, ptr %arr.data.gep2387, align 8
  %arr.elem2389 = getelementptr i64, ptr %arr.data2388, i64 2
  store i64 101, ptr %arr.elem2389, align 8
  %arr.data.gep2390 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2391 = load ptr, ptr %arr.data.gep2390, align 8
  %arr.elem2392 = getelementptr i64, ptr %arr.data2391, i64 3
  store i64 114, ptr %arr.elem2392, align 8
  %arr.data.gep2393 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2394 = load ptr, ptr %arr.data.gep2393, align 8
  %arr.elem2395 = getelementptr i64, ptr %arr.data2394, i64 4
  store i64 116, ptr %arr.elem2395, align 8
  %arr.data.gep2396 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2397 = load ptr, ptr %arr.data.gep2396, align 8
  %arr.elem2398 = getelementptr i64, ptr %arr.data2397, i64 5
  store i64 121, ptr %arr.elem2398, align 8
  %arr.data.gep2399 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2400 = load ptr, ptr %arr.data.gep2399, align 8
  %arr.elem2401 = getelementptr i64, ptr %arr.data2400, i64 6
  store i64 117, ptr %arr.elem2401, align 8
  %arr.data.gep2402 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2403 = load ptr, ptr %arr.data.gep2402, align 8
  %arr.elem2404 = getelementptr i64, ptr %arr.data2403, i64 7
  store i64 105, ptr %arr.elem2404, align 8
  %arr.data.gep2405 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2406 = load ptr, ptr %arr.data.gep2405, align 8
  %arr.elem2407 = getelementptr i64, ptr %arr.data2406, i64 8
  store i64 111, ptr %arr.elem2407, align 8
  %arr.data.gep2408 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2409 = load ptr, ptr %arr.data.gep2408, align 8
  %arr.elem2410 = getelementptr i64, ptr %arr.data2409, i64 9
  store i64 112, ptr %arr.elem2410, align 8
  %arr.data.gep2411 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2412 = load ptr, ptr %arr.data.gep2411, align 8
  %arr.elem2413 = getelementptr i64, ptr %arr.data2412, i64 10
  store i64 91, ptr %arr.elem2413, align 8
  %arr.data.gep2414 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2415 = load ptr, ptr %arr.data.gep2414, align 8
  %arr.elem2416 = getelementptr i64, ptr %arr.data2415, i64 11
  store i64 93, ptr %arr.elem2416, align 8
  %arr.data.gep2417 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2418 = load ptr, ptr %arr.data.gep2417, align 8
  %arr.elem2419 = getelementptr i64, ptr %arr.data2418, i64 12
  store i64 92, ptr %arr.elem2419, align 8
  %arr.data.gep2420 = getelementptr inbounds %SadArray, ptr %arr_new2376, i32 0, i32 2
  %arr.data2421 = load ptr, ptr %arr.data.gep2420, align 8
  %arr.elem2422 = getelementptr i64, ptr %arr.data2421, i64 13
  store i64 0, ptr %arr.elem2422, align 8
  %obj.ptrtoint2423 = ptrtoint ptr %arr_new2376 to i64
  store i64 %obj.ptrtoint2423, ptr %"%vkbd_row1", align 8
  %arr_new2424 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2425 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2425, align 8
  %arr.cap.gep2426 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2426, align 8
  %arr.data2427 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2428 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  store ptr %arr.data2427, ptr %arr.data.gep2428, align 8
  %arr.data.gep2429 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2430 = load ptr, ptr %arr.data.gep2429, align 8
  %arr.elem2431 = getelementptr i64, ptr %arr.data2430, i64 0
  store i64 97, ptr %arr.elem2431, align 8
  %arr.data.gep2432 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2433 = load ptr, ptr %arr.data.gep2432, align 8
  %arr.elem2434 = getelementptr i64, ptr %arr.data2433, i64 1
  store i64 115, ptr %arr.elem2434, align 8
  %arr.data.gep2435 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2436 = load ptr, ptr %arr.data.gep2435, align 8
  %arr.elem2437 = getelementptr i64, ptr %arr.data2436, i64 2
  store i64 100, ptr %arr.elem2437, align 8
  %arr.data.gep2438 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2439 = load ptr, ptr %arr.data.gep2438, align 8
  %arr.elem2440 = getelementptr i64, ptr %arr.data2439, i64 3
  store i64 102, ptr %arr.elem2440, align 8
  %arr.data.gep2441 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2442 = load ptr, ptr %arr.data.gep2441, align 8
  %arr.elem2443 = getelementptr i64, ptr %arr.data2442, i64 4
  store i64 103, ptr %arr.elem2443, align 8
  %arr.data.gep2444 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2445 = load ptr, ptr %arr.data.gep2444, align 8
  %arr.elem2446 = getelementptr i64, ptr %arr.data2445, i64 5
  store i64 104, ptr %arr.elem2446, align 8
  %arr.data.gep2447 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2448 = load ptr, ptr %arr.data.gep2447, align 8
  %arr.elem2449 = getelementptr i64, ptr %arr.data2448, i64 6
  store i64 106, ptr %arr.elem2449, align 8
  %arr.data.gep2450 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2451 = load ptr, ptr %arr.data.gep2450, align 8
  %arr.elem2452 = getelementptr i64, ptr %arr.data2451, i64 7
  store i64 107, ptr %arr.elem2452, align 8
  %arr.data.gep2453 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2454 = load ptr, ptr %arr.data.gep2453, align 8
  %arr.elem2455 = getelementptr i64, ptr %arr.data2454, i64 8
  store i64 108, ptr %arr.elem2455, align 8
  %arr.data.gep2456 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2457 = load ptr, ptr %arr.data.gep2456, align 8
  %arr.elem2458 = getelementptr i64, ptr %arr.data2457, i64 9
  store i64 59, ptr %arr.elem2458, align 8
  %arr.data.gep2459 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2460 = load ptr, ptr %arr.data.gep2459, align 8
  %arr.elem2461 = getelementptr i64, ptr %arr.data2460, i64 10
  store i64 39, ptr %arr.elem2461, align 8
  %arr.data.gep2462 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2463 = load ptr, ptr %arr.data.gep2462, align 8
  %arr.elem2464 = getelementptr i64, ptr %arr.data2463, i64 11
  store i64 13, ptr %arr.elem2464, align 8
  %arr.data.gep2465 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2466 = load ptr, ptr %arr.data.gep2465, align 8
  %arr.elem2467 = getelementptr i64, ptr %arr.data2466, i64 12
  store i64 0, ptr %arr.elem2467, align 8
  %arr.data.gep2468 = getelementptr inbounds %SadArray, ptr %arr_new2424, i32 0, i32 2
  %arr.data2469 = load ptr, ptr %arr.data.gep2468, align 8
  %arr.elem2470 = getelementptr i64, ptr %arr.data2469, i64 13
  store i64 0, ptr %arr.elem2470, align 8
  %obj.ptrtoint2471 = ptrtoint ptr %arr_new2424 to i64
  store i64 %obj.ptrtoint2471, ptr %"%vkbd_row2", align 8
  %arr_new2472 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2473 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2473, align 8
  %arr.cap.gep2474 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2474, align 8
  %arr.data2475 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2476 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  store ptr %arr.data2475, ptr %arr.data.gep2476, align 8
  %arr.data.gep2477 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2478 = load ptr, ptr %arr.data.gep2477, align 8
  %arr.elem2479 = getelementptr i64, ptr %arr.data2478, i64 0
  store i64 122, ptr %arr.elem2479, align 8
  %arr.data.gep2480 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2481 = load ptr, ptr %arr.data.gep2480, align 8
  %arr.elem2482 = getelementptr i64, ptr %arr.data2481, i64 1
  store i64 120, ptr %arr.elem2482, align 8
  %arr.data.gep2483 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2484 = load ptr, ptr %arr.data.gep2483, align 8
  %arr.elem2485 = getelementptr i64, ptr %arr.data2484, i64 2
  store i64 99, ptr %arr.elem2485, align 8
  %arr.data.gep2486 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2487 = load ptr, ptr %arr.data.gep2486, align 8
  %arr.elem2488 = getelementptr i64, ptr %arr.data2487, i64 3
  store i64 118, ptr %arr.elem2488, align 8
  %arr.data.gep2489 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2490 = load ptr, ptr %arr.data.gep2489, align 8
  %arr.elem2491 = getelementptr i64, ptr %arr.data2490, i64 4
  store i64 98, ptr %arr.elem2491, align 8
  %arr.data.gep2492 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2493 = load ptr, ptr %arr.data.gep2492, align 8
  %arr.elem2494 = getelementptr i64, ptr %arr.data2493, i64 5
  store i64 110, ptr %arr.elem2494, align 8
  %arr.data.gep2495 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2496 = load ptr, ptr %arr.data.gep2495, align 8
  %arr.elem2497 = getelementptr i64, ptr %arr.data2496, i64 6
  store i64 109, ptr %arr.elem2497, align 8
  %arr.data.gep2498 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2499 = load ptr, ptr %arr.data.gep2498, align 8
  %arr.elem2500 = getelementptr i64, ptr %arr.data2499, i64 7
  store i64 44, ptr %arr.elem2500, align 8
  %arr.data.gep2501 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2502 = load ptr, ptr %arr.data.gep2501, align 8
  %arr.elem2503 = getelementptr i64, ptr %arr.data2502, i64 8
  store i64 46, ptr %arr.elem2503, align 8
  %arr.data.gep2504 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2505 = load ptr, ptr %arr.data.gep2504, align 8
  %arr.elem2506 = getelementptr i64, ptr %arr.data2505, i64 9
  store i64 47, ptr %arr.elem2506, align 8
  %arr.data.gep2507 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2508 = load ptr, ptr %arr.data.gep2507, align 8
  %arr.elem2509 = getelementptr i64, ptr %arr.data2508, i64 10
  store i64 32, ptr %arr.elem2509, align 8
  %arr.data.gep2510 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2511 = load ptr, ptr %arr.data.gep2510, align 8
  %arr.elem2512 = getelementptr i64, ptr %arr.data2511, i64 11
  store i64 32, ptr %arr.elem2512, align 8
  %arr.data.gep2513 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2514 = load ptr, ptr %arr.data.gep2513, align 8
  %arr.elem2515 = getelementptr i64, ptr %arr.data2514, i64 12
  store i64 32, ptr %arr.elem2515, align 8
  %arr.data.gep2516 = getelementptr inbounds %SadArray, ptr %arr_new2472, i32 0, i32 2
  %arr.data2517 = load ptr, ptr %arr.data.gep2516, align 8
  %arr.elem2518 = getelementptr i64, ptr %arr.data2517, i64 13
  store i64 32, ptr %arr.elem2518, align 8
  %obj.ptrtoint2519 = ptrtoint ptr %arr_new2472 to i64
  store i64 %obj.ptrtoint2519, ptr %"%vkbd_row3", align 8
  %arr_new2520 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2521 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2521, align 8
  %arr.cap.gep2522 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2522, align 8
  %arr.data2523 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2524 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  store ptr %arr.data2523, ptr %arr.data.gep2524, align 8
  %arr.data.gep2525 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2526 = load ptr, ptr %arr.data.gep2525, align 8
  %arr.elem2527 = getelementptr i64, ptr %arr.data2526, i64 0
  store i64 33, ptr %arr.elem2527, align 8
  %arr.data.gep2528 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2529 = load ptr, ptr %arr.data.gep2528, align 8
  %arr.elem2530 = getelementptr i64, ptr %arr.data2529, i64 1
  store i64 64, ptr %arr.elem2530, align 8
  %arr.data.gep2531 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2532 = load ptr, ptr %arr.data.gep2531, align 8
  %arr.elem2533 = getelementptr i64, ptr %arr.data2532, i64 2
  store i64 35, ptr %arr.elem2533, align 8
  %arr.data.gep2534 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2535 = load ptr, ptr %arr.data.gep2534, align 8
  %arr.elem2536 = getelementptr i64, ptr %arr.data2535, i64 3
  store i64 36, ptr %arr.elem2536, align 8
  %arr.data.gep2537 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2538 = load ptr, ptr %arr.data.gep2537, align 8
  %arr.elem2539 = getelementptr i64, ptr %arr.data2538, i64 4
  store i64 37, ptr %arr.elem2539, align 8
  %arr.data.gep2540 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2541 = load ptr, ptr %arr.data.gep2540, align 8
  %arr.elem2542 = getelementptr i64, ptr %arr.data2541, i64 5
  store i64 94, ptr %arr.elem2542, align 8
  %arr.data.gep2543 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2544 = load ptr, ptr %arr.data.gep2543, align 8
  %arr.elem2545 = getelementptr i64, ptr %arr.data2544, i64 6
  store i64 38, ptr %arr.elem2545, align 8
  %arr.data.gep2546 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2547 = load ptr, ptr %arr.data.gep2546, align 8
  %arr.elem2548 = getelementptr i64, ptr %arr.data2547, i64 7
  store i64 42, ptr %arr.elem2548, align 8
  %arr.data.gep2549 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2550 = load ptr, ptr %arr.data.gep2549, align 8
  %arr.elem2551 = getelementptr i64, ptr %arr.data2550, i64 8
  store i64 40, ptr %arr.elem2551, align 8
  %arr.data.gep2552 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2553 = load ptr, ptr %arr.data.gep2552, align 8
  %arr.elem2554 = getelementptr i64, ptr %arr.data2553, i64 9
  store i64 41, ptr %arr.elem2554, align 8
  %arr.data.gep2555 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2556 = load ptr, ptr %arr.data.gep2555, align 8
  %arr.elem2557 = getelementptr i64, ptr %arr.data2556, i64 10
  store i64 95, ptr %arr.elem2557, align 8
  %arr.data.gep2558 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2559 = load ptr, ptr %arr.data.gep2558, align 8
  %arr.elem2560 = getelementptr i64, ptr %arr.data2559, i64 11
  store i64 43, ptr %arr.elem2560, align 8
  %arr.data.gep2561 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2562 = load ptr, ptr %arr.data.gep2561, align 8
  %arr.elem2563 = getelementptr i64, ptr %arr.data2562, i64 12
  store i64 8, ptr %arr.elem2563, align 8
  %arr.data.gep2564 = getelementptr inbounds %SadArray, ptr %arr_new2520, i32 0, i32 2
  %arr.data2565 = load ptr, ptr %arr.data.gep2564, align 8
  %arr.elem2566 = getelementptr i64, ptr %arr.data2565, i64 13
  store i64 127, ptr %arr.elem2566, align 8
  %obj.ptrtoint2567 = ptrtoint ptr %arr_new2520 to i64
  store i64 %obj.ptrtoint2567, ptr %"%vkbd_row0_shift", align 8
  %arr_new2568 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2569 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2569, align 8
  %arr.cap.gep2570 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2570, align 8
  %arr.data2571 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2572 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  store ptr %arr.data2571, ptr %arr.data.gep2572, align 8
  %arr.data.gep2573 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2574 = load ptr, ptr %arr.data.gep2573, align 8
  %arr.elem2575 = getelementptr i64, ptr %arr.data2574, i64 0
  store i64 81, ptr %arr.elem2575, align 8
  %arr.data.gep2576 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2577 = load ptr, ptr %arr.data.gep2576, align 8
  %arr.elem2578 = getelementptr i64, ptr %arr.data2577, i64 1
  store i64 87, ptr %arr.elem2578, align 8
  %arr.data.gep2579 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2580 = load ptr, ptr %arr.data.gep2579, align 8
  %arr.elem2581 = getelementptr i64, ptr %arr.data2580, i64 2
  store i64 69, ptr %arr.elem2581, align 8
  %arr.data.gep2582 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2583 = load ptr, ptr %arr.data.gep2582, align 8
  %arr.elem2584 = getelementptr i64, ptr %arr.data2583, i64 3
  store i64 82, ptr %arr.elem2584, align 8
  %arr.data.gep2585 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2586 = load ptr, ptr %arr.data.gep2585, align 8
  %arr.elem2587 = getelementptr i64, ptr %arr.data2586, i64 4
  store i64 84, ptr %arr.elem2587, align 8
  %arr.data.gep2588 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2589 = load ptr, ptr %arr.data.gep2588, align 8
  %arr.elem2590 = getelementptr i64, ptr %arr.data2589, i64 5
  store i64 89, ptr %arr.elem2590, align 8
  %arr.data.gep2591 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2592 = load ptr, ptr %arr.data.gep2591, align 8
  %arr.elem2593 = getelementptr i64, ptr %arr.data2592, i64 6
  store i64 85, ptr %arr.elem2593, align 8
  %arr.data.gep2594 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2595 = load ptr, ptr %arr.data.gep2594, align 8
  %arr.elem2596 = getelementptr i64, ptr %arr.data2595, i64 7
  store i64 73, ptr %arr.elem2596, align 8
  %arr.data.gep2597 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2598 = load ptr, ptr %arr.data.gep2597, align 8
  %arr.elem2599 = getelementptr i64, ptr %arr.data2598, i64 8
  store i64 79, ptr %arr.elem2599, align 8
  %arr.data.gep2600 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2601 = load ptr, ptr %arr.data.gep2600, align 8
  %arr.elem2602 = getelementptr i64, ptr %arr.data2601, i64 9
  store i64 80, ptr %arr.elem2602, align 8
  %arr.data.gep2603 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2604 = load ptr, ptr %arr.data.gep2603, align 8
  %arr.elem2605 = getelementptr i64, ptr %arr.data2604, i64 10
  store i64 123, ptr %arr.elem2605, align 8
  %arr.data.gep2606 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2607 = load ptr, ptr %arr.data.gep2606, align 8
  %arr.elem2608 = getelementptr i64, ptr %arr.data2607, i64 11
  store i64 125, ptr %arr.elem2608, align 8
  %arr.data.gep2609 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2610 = load ptr, ptr %arr.data.gep2609, align 8
  %arr.elem2611 = getelementptr i64, ptr %arr.data2610, i64 12
  store i64 124, ptr %arr.elem2611, align 8
  %arr.data.gep2612 = getelementptr inbounds %SadArray, ptr %arr_new2568, i32 0, i32 2
  %arr.data2613 = load ptr, ptr %arr.data.gep2612, align 8
  %arr.elem2614 = getelementptr i64, ptr %arr.data2613, i64 13
  store i64 0, ptr %arr.elem2614, align 8
  %obj.ptrtoint2615 = ptrtoint ptr %arr_new2568 to i64
  store i64 %obj.ptrtoint2615, ptr %"%vkbd_row1_shift", align 8
  %arr_new2616 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2617 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2617, align 8
  %arr.cap.gep2618 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2618, align 8
  %arr.data2619 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2620 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  store ptr %arr.data2619, ptr %arr.data.gep2620, align 8
  %arr.data.gep2621 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2622 = load ptr, ptr %arr.data.gep2621, align 8
  %arr.elem2623 = getelementptr i64, ptr %arr.data2622, i64 0
  store i64 65, ptr %arr.elem2623, align 8
  %arr.data.gep2624 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2625 = load ptr, ptr %arr.data.gep2624, align 8
  %arr.elem2626 = getelementptr i64, ptr %arr.data2625, i64 1
  store i64 83, ptr %arr.elem2626, align 8
  %arr.data.gep2627 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2628 = load ptr, ptr %arr.data.gep2627, align 8
  %arr.elem2629 = getelementptr i64, ptr %arr.data2628, i64 2
  store i64 68, ptr %arr.elem2629, align 8
  %arr.data.gep2630 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2631 = load ptr, ptr %arr.data.gep2630, align 8
  %arr.elem2632 = getelementptr i64, ptr %arr.data2631, i64 3
  store i64 70, ptr %arr.elem2632, align 8
  %arr.data.gep2633 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2634 = load ptr, ptr %arr.data.gep2633, align 8
  %arr.elem2635 = getelementptr i64, ptr %arr.data2634, i64 4
  store i64 71, ptr %arr.elem2635, align 8
  %arr.data.gep2636 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2637 = load ptr, ptr %arr.data.gep2636, align 8
  %arr.elem2638 = getelementptr i64, ptr %arr.data2637, i64 5
  store i64 72, ptr %arr.elem2638, align 8
  %arr.data.gep2639 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2640 = load ptr, ptr %arr.data.gep2639, align 8
  %arr.elem2641 = getelementptr i64, ptr %arr.data2640, i64 6
  store i64 74, ptr %arr.elem2641, align 8
  %arr.data.gep2642 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2643 = load ptr, ptr %arr.data.gep2642, align 8
  %arr.elem2644 = getelementptr i64, ptr %arr.data2643, i64 7
  store i64 75, ptr %arr.elem2644, align 8
  %arr.data.gep2645 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2646 = load ptr, ptr %arr.data.gep2645, align 8
  %arr.elem2647 = getelementptr i64, ptr %arr.data2646, i64 8
  store i64 76, ptr %arr.elem2647, align 8
  %arr.data.gep2648 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2649 = load ptr, ptr %arr.data.gep2648, align 8
  %arr.elem2650 = getelementptr i64, ptr %arr.data2649, i64 9
  store i64 58, ptr %arr.elem2650, align 8
  %arr.data.gep2651 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2652 = load ptr, ptr %arr.data.gep2651, align 8
  %arr.elem2653 = getelementptr i64, ptr %arr.data2652, i64 10
  store i64 34, ptr %arr.elem2653, align 8
  %arr.data.gep2654 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2655 = load ptr, ptr %arr.data.gep2654, align 8
  %arr.elem2656 = getelementptr i64, ptr %arr.data2655, i64 11
  store i64 13, ptr %arr.elem2656, align 8
  %arr.data.gep2657 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2658 = load ptr, ptr %arr.data.gep2657, align 8
  %arr.elem2659 = getelementptr i64, ptr %arr.data2658, i64 12
  store i64 0, ptr %arr.elem2659, align 8
  %arr.data.gep2660 = getelementptr inbounds %SadArray, ptr %arr_new2616, i32 0, i32 2
  %arr.data2661 = load ptr, ptr %arr.data.gep2660, align 8
  %arr.elem2662 = getelementptr i64, ptr %arr.data2661, i64 13
  store i64 0, ptr %arr.elem2662, align 8
  %obj.ptrtoint2663 = ptrtoint ptr %arr_new2616 to i64
  store i64 %obj.ptrtoint2663, ptr %"%vkbd_row2_shift", align 8
  %arr_new2664 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep2665 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 0
  store i64 14, ptr %arr.len.gep2665, align 8
  %arr.cap.gep2666 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 1
  store i64 14, ptr %arr.cap.gep2666, align 8
  %arr.data2667 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 14))
  %arr.data.gep2668 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  store ptr %arr.data2667, ptr %arr.data.gep2668, align 8
  %arr.data.gep2669 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2670 = load ptr, ptr %arr.data.gep2669, align 8
  %arr.elem2671 = getelementptr i64, ptr %arr.data2670, i64 0
  store i64 90, ptr %arr.elem2671, align 8
  %arr.data.gep2672 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2673 = load ptr, ptr %arr.data.gep2672, align 8
  %arr.elem2674 = getelementptr i64, ptr %arr.data2673, i64 1
  store i64 88, ptr %arr.elem2674, align 8
  %arr.data.gep2675 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2676 = load ptr, ptr %arr.data.gep2675, align 8
  %arr.elem2677 = getelementptr i64, ptr %arr.data2676, i64 2
  store i64 67, ptr %arr.elem2677, align 8
  %arr.data.gep2678 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2679 = load ptr, ptr %arr.data.gep2678, align 8
  %arr.elem2680 = getelementptr i64, ptr %arr.data2679, i64 3
  store i64 86, ptr %arr.elem2680, align 8
  %arr.data.gep2681 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2682 = load ptr, ptr %arr.data.gep2681, align 8
  %arr.elem2683 = getelementptr i64, ptr %arr.data2682, i64 4
  store i64 66, ptr %arr.elem2683, align 8
  %arr.data.gep2684 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2685 = load ptr, ptr %arr.data.gep2684, align 8
  %arr.elem2686 = getelementptr i64, ptr %arr.data2685, i64 5
  store i64 78, ptr %arr.elem2686, align 8
  %arr.data.gep2687 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2688 = load ptr, ptr %arr.data.gep2687, align 8
  %arr.elem2689 = getelementptr i64, ptr %arr.data2688, i64 6
  store i64 77, ptr %arr.elem2689, align 8
  %arr.data.gep2690 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2691 = load ptr, ptr %arr.data.gep2690, align 8
  %arr.elem2692 = getelementptr i64, ptr %arr.data2691, i64 7
  store i64 60, ptr %arr.elem2692, align 8
  %arr.data.gep2693 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2694 = load ptr, ptr %arr.data.gep2693, align 8
  %arr.elem2695 = getelementptr i64, ptr %arr.data2694, i64 8
  store i64 62, ptr %arr.elem2695, align 8
  %arr.data.gep2696 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2697 = load ptr, ptr %arr.data.gep2696, align 8
  %arr.elem2698 = getelementptr i64, ptr %arr.data2697, i64 9
  store i64 63, ptr %arr.elem2698, align 8
  %arr.data.gep2699 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2700 = load ptr, ptr %arr.data.gep2699, align 8
  %arr.elem2701 = getelementptr i64, ptr %arr.data2700, i64 10
  store i64 32, ptr %arr.elem2701, align 8
  %arr.data.gep2702 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2703 = load ptr, ptr %arr.data.gep2702, align 8
  %arr.elem2704 = getelementptr i64, ptr %arr.data2703, i64 11
  store i64 32, ptr %arr.elem2704, align 8
  %arr.data.gep2705 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2706 = load ptr, ptr %arr.data.gep2705, align 8
  %arr.elem2707 = getelementptr i64, ptr %arr.data2706, i64 12
  store i64 32, ptr %arr.elem2707, align 8
  %arr.data.gep2708 = getelementptr inbounds %SadArray, ptr %arr_new2664, i32 0, i32 2
  %arr.data2709 = load ptr, ptr %arr.data.gep2708, align 8
  %arr.elem2710 = getelementptr i64, ptr %arr.data2709, i64 13
  store i64 32, ptr %arr.elem2710, align 8
  %obj.ptrtoint2711 = ptrtoint ptr %arr_new2664 to i64
  store i64 %obj.ptrtoint2711, ptr %"%vkbd_row3_shift", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
