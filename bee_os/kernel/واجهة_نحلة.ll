; ModuleID = 'واجهة_نحلة'
source_filename = "\D9\88\D8\A7\D8\AC\D9\87\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
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

declare void @sad_ll_fb_init(i64, i64, i64)

declare void @sad_ll_fb_set_pixel(i64, i64, i64)

declare void @sad_ll_fb_fill_rect(i64, i64, i64, i64, i64)

declare void @sad_ll_fb_clear(i64)

define void @"gui_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
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
  call void @"gui_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_2:                                           ; preds = %merge_1
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"gui_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_3
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\AE\D8\B7"() {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  br i1 true, label %then_4, label %merge_5

merge_5:                                          ; preds = %entry
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_6

then_4:                                           ; preds = %entry
  ret i64 -1

while_body_7:                                     ; preds = %while_cond_6
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
  br label %while_cond_6

while_cond_6:                                     ; preds = %while_body_7, %merge_5
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 96
  br i1 %cmplttmp, label %while_body_7, label %while_exit_8

while_exit_8:                                     ; preds = %while_cond_6
  store i64 264, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %"%58.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  call void @sad_ll_mem_write64(i64 %"%58.load", i64 24)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %addtmp18 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load", 1
  call void @sad_ll_mem_write64(i64 %addtmp18, i64 60)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load19" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %addtmp20 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load19", 2
  call void @sad_ll_mem_write64(i64 %addtmp20, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load21" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %addtmp22 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load21", 3
  call void @sad_ll_mem_write64(i64 %addtmp22, i64 126)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load23" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %addtmp24 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load23", 4
  call void @sad_ll_mem_write64(i64 %addtmp24, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load25" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %addtmp26 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load25", 5
  call void @sad_ll_mem_write64(i64 %addtmp26, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load27" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %addtmp28 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load27", 6
  call void @sad_ll_mem_write64(i64 %addtmp28, i64 102)
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load29" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A", align 8
  %addtmp30 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_A.load29", 7
  call void @sad_ll_mem_write64(i64 %addtmp30, i64 0)
  ret i64 0
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %"\D8\AD\D8\B1\D9\81_ascii", i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y", i64 %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81") {
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
  br i1 %cmplttmp, label %then_9, label %merge_10

merge_10:                                         ; preds = %entry
  %"%\D8\AD\D8\B1\D9\81_ascii.load5" = load i64, ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\AD\D8\B1\D9\81_ascii.load5", 127
  br i1 %cmpgttmp, label %then_11, label %merge_12

merge_12:                                         ; preds = %merge_10
  %"%\D8\AD\D8\B1\D9\81_ascii.load6" = load i64, ptr %"\D8\AD\D8\B1\D9\81_ascii1", align 8
  %subtmp = sub i64 %"%\D8\AD\D8\B1\D9\81_ascii.load6", 32
  store i64 %subtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %shltmp = shl i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 3
  %addtmp = add i64 0, %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  store i64 0, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_13

merge_23:                                         ; preds = %then_22, %while_exit_21
  %"%\D8\B9\D9\85\D9\88\D8\AF.load22" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %addtmp23 = add i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load22", 1
  store i64 %addtmp23, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  br label %while_cond_16

then_11:                                          ; preds = %merge_10
  ret void

then_22:                                          ; preds = %while_exit_21
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load18" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %addtmp19 = add i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %"%\D8\B9\D9\85\D9\88\D8\AF.load18"
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %"%\D8\B5\D9\81.load20" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp21 = add i64 %"%\D8\B3\D8\B7\D8\AD_y.load", %"%\D8\B5\D9\81.load20"
  %"%\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\814", align 8
  call void @sad_ll_fb_set_pixel(i64 %addtmp19, i64 %addtmp21, i64 %"%\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load")
  br label %merge_23

then_9:                                           ; preds = %entry
  ret void

while_body_14:                                    ; preds = %while_cond_13
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81", align 8
  %"%\D8\B5\D9\81.load8" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp9 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\AD\D8\B1\D9\81.load", %"%\D8\B5\D9\81.load8"
  call void @sad_ll_mem_read64(i64 %addtmp9)
  store i64 0, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81", align 8
  store i64 0, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  br label %while_cond_16

while_body_17:                                    ; preds = %while_cond_16
  store i64 128, ptr %"%\D8\A8\D8\AA", align 8
  store i64 0, ptr %"%\D8\B4", align 8
  br label %while_cond_19

while_body_20:                                    ; preds = %while_cond_19
  %"%\D8\A8\D8\AA.load" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %shrtmp = lshr i64 %"%\D8\A8\D8\AA.load", 1
  store i64 %shrtmp, ptr %"%\D8\A8\D8\AA", align 8
  %"%\D8\B4.load15" = load i64, ptr %"%\D8\B4", align 8
  %addtmp16 = add i64 %"%\D8\B4.load15", 1
  store i64 %addtmp16, ptr %"%\D8\B4", align 8
  br label %while_cond_19

while_cond_13:                                    ; preds = %while_exit_18, %merge_12
  %"%\D8\B5\D9\81.load" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %cmplttmp7 = icmp slt i64 %"%\D8\B5\D9\81.load", 8
  br i1 %cmplttmp7, label %while_body_14, label %while_exit_15

while_cond_16:                                    ; preds = %merge_23, %while_body_14
  %"%\D8\B9\D9\85\D9\88\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp10 = icmp slt i64 %"%\D8\B9\D9\85\D9\88\D8\AF.load", 8
  br i1 %cmplttmp10, label %while_body_17, label %while_exit_18

while_cond_19:                                    ; preds = %while_body_20, %while_body_17
  %"%\D8\B4.load" = load i64, ptr %"%\D8\B4", align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF.load13" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF", align 8
  %cmplttmp14 = icmp slt i64 %"%\D8\B4.load", %"%\D8\B9\D9\85\D9\88\D8\AF.load13"
  br i1 %cmplttmp14, label %while_body_20, label %while_exit_21

while_exit_15:                                    ; preds = %while_cond_13
  ret void

while_exit_18:                                    ; preds = %while_cond_16
  %"%\D8\B5\D9\81.load11" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp12 = add i64 %"%\D8\B5\D9\81.load11", 1
  store i64 %addtmp12, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_13

while_exit_21:                                    ; preds = %while_cond_19
  %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81.load" = load i64, ptr %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81", align 8
  %"%\D8\A8\D8\AA.load17" = load i64, ptr %"%\D8\A8\D8\AA", align 8
  %divtmp = sdiv i64 %"%\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA_\D8\A7\D9\84\D8\B5\D9\81.load", %"%\D8\A8\D8\AA.load17"
  %andtmp = and i64 %divtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_22, label %merge_23
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\86\D8\B5_\D8\B0\D8\A7\D9\83\D8\B1\D8\A9"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5", i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y", i64 %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81") {
entry:
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D8\B2\D9\88\D8\AC" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\815" = alloca i64, align 8
  store i64 %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81", ptr %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\815", align 8
  %"\D8\B3\D8\B7\D8\AD_y4" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y4", align 8
  %"\D8\B3\D8\B7\D8\AD_x3" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x3", align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B52" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B52", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B51" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B51", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_24

merge_28:                                         ; preds = %then_27, %while_body_25
  %"%\D9\81.load10" = load i64, ptr %"%\D9\81", align 8
  %shltmp = shl i64 %"%\D9\81.load10", 3
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x3", align 8
  %addtmp11 = add i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %shltmp
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y4", align 8
  %"%\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\815", align 8
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\AD\D8\B1\D9\81"(i64 %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load", i64 %addtmp11, i64 %"%\D8\B3\D8\B7\D8\AD_y.load", i64 %"%\D9\84\D9\88\D9\86_\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load")
  %"%\D9\81.load12" = load i64, ptr %"%\D9\81", align 8
  %addtmp13 = add i64 %"%\D9\81.load12", 1
  store i64 %addtmp13, ptr %"%\D9\81", align 8
  br label %while_cond_24

then_27:                                          ; preds = %while_body_25
  %"%\D8\B2\D9\88\D8\AC.load9" = load i64, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %shrtmp = lshr i64 %"%\D8\B2\D9\88\D8\AC.load9", 8
  store i64 %shrtmp, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  br label %merge_28

while_body_25:                                    ; preds = %while_cond_24
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B51", align 8
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5.load", %"%\D9\81.load6"
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %andtmp = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load7" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %subtmp = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load7", %andtmp
  call void @sad_ll_mem_read16(i64 %subtmp)
  store i64 0, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %"%\D8\B2\D9\88\D8\AC.load" = load i64, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %andtmp8 = and i64 %"%\D8\B2\D9\88\D8\AC.load", 255
  store i64 %andtmp8, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_27, label %merge_28

while_cond_24:                                    ; preds = %merge_28, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B52", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5.load"
  br i1 %cmplttmp, label %while_body_25, label %while_exit_26

while_exit_26:                                    ; preds = %while_cond_24
  ret void
}

define i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"() {
entry:
  %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_29

merge_33:                                         ; preds = %while_body_30
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load1" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load1", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_29

then_32:                                          ; preds = %while_body_30
  ret i64 0

while_body_30:                                    ; preds = %while_cond_29
  %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl", align 8
  %shrtmp = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl.load", 1
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_32, label %merge_33

while_cond_29:                                    ; preds = %merge_33, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 10000
  br i1 %cmplttmp, label %while_body_30, label %while_exit_31

while_exit_31:                                    ; preds = %while_cond_29
  ret i64 -1
}

define i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"() {
entry:
  %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_34

merge_38:                                         ; preds = %while_body_35
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load1" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load1", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_34

then_37:                                          ; preds = %while_body_35
  ret i64 0

while_body_35:                                    ; preds = %while_cond_34
  %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl", align 8
  %andtmp = and i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_ctrl.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_37, label %merge_38

while_cond_34:                                    ; preds = %merge_38, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 10000
  br i1 %cmplttmp, label %while_body_35, label %while_exit_36

while_exit_36:                                    ; preds = %while_cond_34
  ret i64 -1
}

define i64 @"mouse_\D8\A3\D8\B1\D8\B3\D9\84_\D8\A3\D9\85\D8\B1"(i64 %"\D8\A7\D9\84\D8\A3\D9\85\D8\B1") {
entry:
  %"\D8\A7\D9\84\D8\A3\D9\85\D8\B11" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D8\A3\D9\85\D8\B1", ptr %"\D8\A7\D9\84\D8\A3\D9\85\D8\B11", align 8
  %calltmp = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp2 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp3 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"()
  ret i64 0
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3"() {
entry:
  %"%config" = alloca i64, align 8
  %calltmp = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp1 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp2 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"()
  %"%config.load" = load i64, ptr %"%config", align 8
  %addtmp = add i64 %"%config.load", 2
  store i64 %addtmp, ptr %"%config", align 8
  %calltmp3 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp4 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9"()
  %calltmp5 = call i64 @"mouse_\D8\A3\D8\B1\D8\B3\D9\84_\D8\A3\D9\85\D8\B1"(i64 255)
  %calltmp6 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"()
  %calltmp7 = call i64 @"mouse_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9"()
  %calltmp8 = call i64 @"mouse_\D8\A3\D8\B1\D8\B3\D9\84_\D8\A3\D9\85\D8\B1"(i64 244)
  ret i64 0
}

define void @"mouse_\D9\85\D8\B9\D8\A7\D9\84\D8\AC_\D8\A8\D8\A7\D9\8A\D8\AA"(i64 %"\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3") {
entry:
  %"%dy" = alloca i64, align 8
  %"%dx" = alloca i64, align 8
  %"\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B31" = alloca i64, align 8
  store i64 %"\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3", ptr %"\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B31", align 8
  br i1 true, label %then_39, label %merge_40

merge_40:                                         ; preds = %then_39, %entry
  br i1 false, label %then_41, label %merge_42

merge_42:                                         ; preds = %merge_44, %merge_40
  br i1 false, label %then_45, label %merge_46

merge_44:                                         ; preds = %then_43, %then_41
  br label %merge_42

merge_46:                                         ; preds = %merge_48, %merge_42
  ret void

merge_48:                                         ; preds = %merge_60, %then_45
  br label %merge_46

merge_50:                                         ; preds = %then_49, %then_47
  br i1 false, label %then_51, label %merge_52

merge_52:                                         ; preds = %then_51, %merge_50
  br i1 false, label %then_53, label %merge_54

merge_54:                                         ; preds = %then_53, %merge_52
  br i1 true, label %then_55, label %merge_56

merge_56:                                         ; preds = %then_55, %merge_54
  br i1 false, label %then_57, label %merge_58

merge_58:                                         ; preds = %then_57, %merge_56
  br i1 true, label %then_59, label %merge_60

merge_60:                                         ; preds = %then_59, %merge_58
  br label %merge_48

then_39:                                          ; preds = %entry
  br label %merge_40

then_41:                                          ; preds = %merge_40
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3.load" = load i64, ptr %"\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B31", align 8
  %cmpnetmp = icmp ne i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3.load", 0
  br i1 %cmpnetmp, label %then_43, label %merge_44

then_43:                                          ; preds = %then_41
  br label %merge_44

then_45:                                          ; preds = %merge_42
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3.load2" = load i64, ptr %"\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B31", align 8
  %cmpnetmp3 = icmp ne i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3.load2", 0
  br i1 %cmpnetmp3, label %then_47, label %merge_48

then_47:                                          ; preds = %then_45
  br i1 false, label %then_49, label %merge_50

then_49:                                          ; preds = %then_47
  %"%dx.load" = load i64, ptr %"%dx", align 8
  %subtmp = sub i64 %"%dx.load", 256
  store i64 %subtmp, ptr %"%dx", align 8
  br label %merge_50

then_51:                                          ; preds = %merge_50
  %"%dy.load" = load i64, ptr %"%dy", align 8
  %subtmp4 = sub i64 %"%dy.load", 256
  store i64 %subtmp4, ptr %"%dy", align 8
  br label %merge_52

then_53:                                          ; preds = %merge_52
  br label %merge_54

then_55:                                          ; preds = %merge_54
  br label %merge_56

then_57:                                          ; preds = %merge_56
  br label %merge_58

then_59:                                          ; preds = %merge_58
  br label %merge_60
}

define i64 @"gui_\D8\A3\D9\86\D8\B4\D8\A6_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %wx, i64 %wy, i64 %ww, i64 %wh, i64 %"\D9\86\D9\88\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D9\86\D9\88\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A95" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9", ptr %"\D9\86\D9\88\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A95", align 8
  %wh4 = alloca i64, align 8
  store i64 %wh, ptr %wh4, align 8
  %ww3 = alloca i64, align 8
  store i64 %ww, ptr %ww3, align 8
  %wy2 = alloca i64, align 8
  store i64 %wy, ptr %wy2, align 8
  %wx1 = alloca i64, align 8
  store i64 %wx, ptr %wx1, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_61

merge_65:                                         ; preds = %while_body_62
  %"%\D9\81.load50" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load50", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_61

then_64:                                          ; preds = %while_body_62
  %"%\D9\81.load7" = load i64, ptr %"%\D9\81", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\81.load7"
  store i64 0, ptr %arr.elem9, align 8
  %"%\D9\81.load10" = load i64, ptr %"%\D9\81", align 8
  %"%\D9\86\D9\88\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A95", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\81.load10"
  store i64 %"%\D9\86\D9\88\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load", ptr %arr.elem12, align 8
  %"%\D9\81.load13" = load i64, ptr %"%\D9\81", align 8
  %"%wx.load" = load i64, ptr %wx1, align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D9\81.load13"
  store i64 %"%wx.load", ptr %arr.elem15, align 8
  %"%\D9\81.load16" = load i64, ptr %"%\D9\81", align 8
  %"%wy.load" = load i64, ptr %wy2, align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D9\81.load16"
  store i64 %"%wy.load", ptr %arr.elem18, align 8
  %"%\D9\81.load19" = load i64, ptr %"%\D9\81", align 8
  %"%ww.load" = load i64, ptr %ww3, align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 %"%\D9\81.load19"
  store i64 %"%ww.load", ptr %arr.elem21, align 8
  %"%\D9\81.load22" = load i64, ptr %"%\D9\81", align 8
  %"%wh.load" = load i64, ptr %wh4, align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%\D9\81.load22"
  store i64 %"%wh.load", ptr %arr.elem24, align 8
  %"%\D9\81.load25" = load i64, ptr %"%\D9\81", align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 %"%\D9\81.load25"
  store i64 0, ptr %arr.elem27, align 8
  %"%\D9\81.load28" = load i64, ptr %"%\D9\81", align 8
  %arr.data29 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem30 = getelementptr i64, ptr %arr.data29, i64 %"%\D9\81.load28"
  store i64 0, ptr %arr.elem30, align 8
  %"%\D9\81.load31" = load i64, ptr %"%\D9\81", align 8
  %"%wx.load32" = load i64, ptr %wx1, align 8
  %arr.data33 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 %"%\D9\81.load31"
  store i64 %"%wx.load32", ptr %arr.elem34, align 8
  %"%\D9\81.load35" = load i64, ptr %"%\D9\81", align 8
  %"%wy.load36" = load i64, ptr %wy2, align 8
  %arr.data37 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 %"%\D9\81.load35"
  store i64 %"%wy.load36", ptr %arr.elem38, align 8
  %"%\D9\81.load39" = load i64, ptr %"%\D9\81", align 8
  %"%ww.load40" = load i64, ptr %ww3, align 8
  %arr.data41 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 %"%\D9\81.load39"
  store i64 %"%ww.load40", ptr %arr.elem42, align 8
  %"%\D9\81.load43" = load i64, ptr %"%\D9\81", align 8
  %"%wh.load44" = load i64, ptr %wh4, align 8
  %arr.data45 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 %"%\D9\81.load43"
  store i64 %"%wh.load44", ptr %arr.elem46, align 8
  %"%\D9\81.load47" = load i64, ptr %"%\D9\81", align 8
  %arr.data48 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem49 = getelementptr i64, ptr %arr.data48, i64 %"%\D9\81.load47"
  store i64 0, ptr %arr.elem49, align 8
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_62:                                    ; preds = %while_cond_61
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load6"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_64, label %merge_65

while_cond_61:                                    ; preds = %merge_65, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_62, label %while_exit_63

while_exit_63:                                    ; preds = %while_cond_61
  ret i64 -1
}

define void @"gui_\D8\A3\D8\BA\D9\84\D9\82_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89_z" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_66, label %merge_67

merge_67:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2", 0
  br i1 %cmpgetmp, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_70, label %merge_71

merge_71:                                         ; preds = %then_70, %merge_69
  %"%\D9\81\D9\87\D8\B1\D8\B3.load8" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load8"
  store i64 0, ptr %arr.elem10, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load11"
  store i64 0, ptr %arr.elem13, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load14" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpeqtmp = icmp eq i64 0, %"%\D9\81\D9\87\D8\B1\D8\B3.load14"
  br i1 %cmpeqtmp, label %then_72, label %merge_73

merge_73:                                         ; preds = %while_exit_76, %merge_71
  ret void

merge_78:                                         ; preds = %merge_80, %while_body_75
  %"%\D9\81.load26" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load26", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_74

merge_80:                                         ; preds = %merge_82, %then_77
  br label %merge_78

merge_82:                                         ; preds = %then_81, %then_79
  br label %merge_80

then_66:                                          ; preds = %entry
  ret void

then_68:                                          ; preds = %merge_67
  ret void

then_70:                                          ; preds = %merge_69
  %"%\D9\81\D9\87\D8\B1\D8\B3.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  call void @sad_ll_page_free(i64 %arr.get7)
  br label %merge_71

then_72:                                          ; preds = %merge_71
  store i64 0, ptr %"%\D8\A3\D8\B9\D9\84\D9\89_z", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_74

then_77:                                          ; preds = %while_body_75
  %"%\D9\81.load21" = load i64, ptr %"%\D9\81", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81.load21"
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  %cmpnetmp25 = icmp ne i64 %arr.get24, 0
  br i1 %cmpnetmp25, label %then_79, label %merge_80

then_79:                                          ; preds = %then_77
  %"%\D9\81.load27" = load i64, ptr %"%\D9\81", align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 %"%\D9\81.load27"
  %arr.get30 = load i64, ptr %arr.elem29, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89_z.load" = load i64, ptr %"%\D8\A3\D8\B9\D9\84\D9\89_z", align 8
  %cmpgttmp = icmp sgt i64 %arr.get30, %"%\D8\A3\D8\B9\D9\84\D9\89_z.load"
  br i1 %cmpgttmp, label %then_81, label %merge_82

then_81:                                          ; preds = %then_79
  %"%\D9\81.load31" = load i64, ptr %"%\D9\81", align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 %"%\D9\81.load31"
  %arr.get34 = load i64, ptr %arr.elem33, align 8
  store i64 %arr.get34, ptr %"%\D8\A3\D8\B9\D9\84\D9\89_z", align 8
  br label %merge_82

while_body_75:                                    ; preds = %while_cond_74
  %"%\D9\81.load16" = load i64, ptr %"%\D9\81", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D9\81.load16"
  %arr.get19 = load i64, ptr %arr.elem18, align 8
  %cmpnetmp20 = icmp ne i64 %arr.get19, 0
  br i1 %cmpnetmp20, label %then_77, label %merge_78

while_cond_74:                                    ; preds = %merge_78, %then_72
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp15 = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp15, label %while_body_75, label %while_exit_76

while_exit_76:                                    ; preds = %while_cond_74
  br label %merge_73
}

define void @"gui_\D8\B1\D9\81\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_83, label %merge_84

merge_84:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2"
  store i64 0, ptr %arr.elem, align 8
  ret void

then_83:                                          ; preds = %entry
  ret void
}

define void @"gui_\D9\83\D8\A8\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_85, label %merge_86

else_89:                                          ; preds = %merge_86
  %"%\D9\81\D9\87\D8\B1\D8\B3.load34" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data35 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load34"
  %arr.get37 = load i64, ptr %arr.elem36, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load38" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data39 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load38"
  store i64 %arr.get37, ptr %arr.elem40, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load41" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data42 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load41"
  %arr.get44 = load i64, ptr %arr.elem43, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load45" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data46 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load45"
  store i64 %arr.get44, ptr %arr.elem47, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load48" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data49 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem50 = getelementptr i64, ptr %arr.data49, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load48"
  %arr.get51 = load i64, ptr %arr.elem50, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load52" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data53 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem54 = getelementptr i64, ptr %arr.data53, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load52"
  store i64 %arr.get51, ptr %arr.elem54, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load55" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data56 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem57 = getelementptr i64, ptr %arr.data56, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load55"
  %arr.get58 = load i64, ptr %arr.elem57, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load59" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data60 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem61 = getelementptr i64, ptr %arr.data60, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load59"
  store i64 %arr.get58, ptr %arr.elem61, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load62" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data63 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem64 = getelementptr i64, ptr %arr.data63, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load62"
  store i64 0, ptr %arr.elem64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load65" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data66 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem67 = getelementptr i64, ptr %arr.data66, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load65"
  store i64 0, ptr %arr.elem67, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load68" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data69 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem70 = getelementptr i64, ptr %arr.data69, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load68"
  store i64 0, ptr %arr.elem70, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load71" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data72 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem73 = getelementptr i64, ptr %arr.data72, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load71"
  store i64 0, ptr %arr.elem73, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load74" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data75 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem76 = getelementptr i64, ptr %arr.data75, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load74"
  store i64 0, ptr %arr.elem76, align 8
  br label %merge_88

merge_86:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_87, label %else_89

merge_88:                                         ; preds = %else_89, %then_87
  ret void

then_85:                                          ; preds = %entry
  ret void

then_87:                                          ; preds = %merge_86
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  store i64 0, ptr %arr.elem5, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load10" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load10"
  store i64 %arr.get9, ptr %arr.elem12, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load13" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load17" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load17"
  store i64 %arr.get16, ptr %arr.elem19, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load20" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load20"
  %arr.get23 = load i64, ptr %arr.elem22, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load24" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load24"
  store i64 %arr.get23, ptr %arr.elem26, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load27" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load27"
  %arr.get30 = load i64, ptr %arr.elem29, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load31" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load31"
  store i64 %arr.get30, ptr %arr.elem33, align 8
  br label %merge_88
}

define void @"gui_\D8\B5\D8\BA\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmplttmp, label %then_90, label %merge_91

else_94:                                          ; preds = %merge_91
  %"%\D9\81\D9\87\D8\B1\D8\B3.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load6"
  store i64 0, ptr %arr.elem8, align 8
  br label %merge_93

merge_91:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_92, label %else_94

merge_93:                                         ; preds = %else_94, %then_92
  ret void

then_90:                                          ; preds = %entry
  ret void

then_92:                                          ; preds = %merge_91
  %"%\D9\81\D9\87\D8\B1\D8\B3.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load3"
  store i64 0, ptr %arr.elem5, align 8
  br label %merge_93
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B3\D8\B7\D8\AD_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D8\A8"() {
entry:
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%btn_x" = alloca i64, align 8
  store i64 4, ptr %"%btn_x", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_95

merge_101:                                        ; preds = %then_100, %then_98
  %"%\D9\81.load4" = load i64, ptr %"%\D9\81", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %cmpeqtmp8 = icmp eq i64 %arr.get7, 0
  br i1 %cmpeqtmp8, label %then_102, label %merge_103

merge_103:                                        ; preds = %then_102, %merge_101
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81.load9"
  %arr.get12 = load i64, ptr %arr.elem11, align 8
  %cmpeqtmp13 = icmp eq i64 %arr.get12, 0
  br i1 %cmpeqtmp13, label %then_104, label %merge_105

merge_105:                                        ; preds = %then_104, %merge_103
  %"%\D9\81.load14" = load i64, ptr %"%\D9\81", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81.load14"
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  %cmpeqtmp18 = icmp eq i64 %arr.get17, 0
  br i1 %cmpeqtmp18, label %then_106, label %merge_107

merge_107:                                        ; preds = %then_106, %merge_105
  %"%btn_x.load" = load i64, ptr %"%btn_x", align 8
  %addtmp19 = add i64 %"%btn_x.load", 0
  %addtmp20 = add i64 %addtmp19, 2
  store i64 %addtmp20, ptr %"%btn_x", align 8
  br label %merge_99

merge_99:                                         ; preds = %merge_107, %while_body_96
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load3", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_95

then_100:                                         ; preds = %then_98
  br label %merge_101

then_102:                                         ; preds = %merge_101
  br label %merge_103

then_104:                                         ; preds = %merge_103
  br label %merge_105

then_106:                                         ; preds = %merge_105
  br label %merge_107

then_98:                                          ; preds = %while_body_96
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\81.load2", 0
  br i1 %cmpeqtmp, label %then_100, label %merge_101

while_body_96:                                    ; preds = %while_cond_95
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_98, label %merge_99

while_cond_95:                                    ; preds = %merge_99, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_96, label %while_exit_97

while_exit_97:                                    ; preds = %while_cond_95
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3") {
entry:
  %"%content_h" = alloca i64, align 8
  %"%content_w" = alloca i64, align 8
  %"%content_y" = alloca i64, align 8
  %"%content_x" = alloca i64, align 8
  %"%min_x" = alloca i64, align 8
  %"%max_x" = alloca i64, align 8
  %"%close_x" = alloca i64, align 8
  %"%btn_y" = alloca i64, align 8
  %"%wh" = alloca i64, align 8
  %"%ww" = alloca i64, align 8
  %"%wy" = alloca i64, align 8
  %"%wx" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_108, label %merge_109

merge_109:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %cmpeqtmp6 = icmp eq i64 %arr.get5, 0
  br i1 %cmpeqtmp6, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  %"%\D9\81\D9\87\D8\B1\D8\B3.load7" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  store i64 %arr.get10, ptr %"%wx", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load11"
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  store i64 %arr.get14, ptr %"%wy", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load15" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load15"
  %arr.get18 = load i64, ptr %arr.elem17, align 8
  store i64 %arr.get18, ptr %"%ww", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load19" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load19"
  %arr.get22 = load i64, ptr %arr.elem21, align 8
  store i64 %arr.get22, ptr %"%wh", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load23" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %cmpeqtmp24 = icmp eq i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load23", 0
  br i1 %cmpeqtmp24, label %then_112, label %merge_113

merge_113:                                        ; preds = %then_112, %merge_111
  %"%wy.load" = load i64, ptr %"%wy", align 8
  %addtmp = add i64 %"%wy.load", 0
  %addtmp25 = add i64 %addtmp, 4
  store i64 %addtmp25, ptr %"%btn_y", align 8
  %"%wx.load" = load i64, ptr %"%wx", align 8
  %"%ww.load" = load i64, ptr %"%ww", align 8
  %addtmp26 = add i64 %"%wx.load", %"%ww.load"
  %subtmp = sub i64 %addtmp26, 0
  %subtmp27 = sub i64 %subtmp, 0
  %subtmp28 = sub i64 %subtmp27, 2
  store i64 %subtmp28, ptr %"%close_x", align 8
  %"%close_x.load" = load i64, ptr %"%close_x", align 8
  %subtmp29 = sub i64 %"%close_x.load", 0
  %subtmp30 = sub i64 %subtmp29, 2
  store i64 %subtmp30, ptr %"%max_x", align 8
  %"%max_x.load" = load i64, ptr %"%max_x", align 8
  %subtmp31 = sub i64 %"%max_x.load", 0
  %subtmp32 = sub i64 %subtmp31, 2
  store i64 %subtmp32, ptr %"%min_x", align 8
  %"%wx.load33" = load i64, ptr %"%wx", align 8
  %addtmp34 = add i64 %"%wx.load33", 0
  store i64 %addtmp34, ptr %"%content_x", align 8
  %addtmp35 = add i64 %addtmp, 0
  store i64 %addtmp35, ptr %"%content_y", align 8
  %"%ww.load36" = load i64, ptr %"%ww", align 8
  %subtmp37 = sub i64 %"%ww.load36", 0
  store i64 %subtmp37, ptr %"%content_w", align 8
  %"%wh.load" = load i64, ptr %"%wh", align 8
  %subtmp38 = sub i64 %"%wh.load", 0
  %subtmp39 = sub i64 %subtmp38, 0
  store i64 %subtmp39, ptr %"%content_h", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load40" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data41 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load40"
  %arr.get43 = load i64, ptr %arr.elem42, align 8
  %cmpeqtmp44 = icmp eq i64 %arr.get43, 0
  br i1 %cmpeqtmp44, label %then_114, label %merge_115

merge_115:                                        ; preds = %then_114, %merge_113
  %"%\D9\81\D9\87\D8\B1\D8\B3.load46" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data47 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load46"
  %arr.get49 = load i64, ptr %arr.elem48, align 8
  %cmpeqtmp50 = icmp eq i64 %arr.get49, 0
  br i1 %cmpeqtmp50, label %then_116, label %merge_117

merge_117:                                        ; preds = %then_116, %merge_115
  %"%\D9\81\D9\87\D8\B1\D8\B3.load56" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data57 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem58 = getelementptr i64, ptr %arr.data57, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load56"
  %arr.get59 = load i64, ptr %arr.elem58, align 8
  %cmpeqtmp60 = icmp eq i64 %arr.get59, 0
  br i1 %cmpeqtmp60, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  %"%\D9\81\D9\87\D8\B1\D8\B3.load66" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data67 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem68 = getelementptr i64, ptr %arr.data67, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load66"
  %arr.get69 = load i64, ptr %arr.elem68, align 8
  %cmpnetmp = icmp ne i64 %arr.get69, 0
  br i1 %cmpnetmp, label %then_120, label %merge_121

merge_121:                                        ; preds = %then_120, %merge_119
  ret void

then_108:                                         ; preds = %entry
  ret void

then_110:                                         ; preds = %merge_109
  ret void

then_112:                                         ; preds = %merge_111
  br label %merge_113

then_114:                                         ; preds = %merge_113
  %"%\D9\81\D9\87\D8\B1\D8\B3.load45" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%content_x.load" = load i64, ptr %"%content_x", align 8
  %"%content_y.load" = load i64, ptr %"%content_y", align 8
  %"%content_w.load" = load i64, ptr %"%content_w", align 8
  %"%content_h.load" = load i64, ptr %"%content_h", align 8
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\85\D8\AA\D8\B5\D9\81\D8\AD_\D9\85\D9\84\D9\81\D8\A7\D8\AA"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load45", i64 %"%content_x.load", i64 %"%content_y.load", i64 %"%content_w.load", i64 %"%content_h.load")
  br label %merge_115

then_116:                                         ; preds = %merge_115
  %"%\D9\81\D9\87\D8\B1\D8\B3.load51" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%content_x.load52" = load i64, ptr %"%content_x", align 8
  %"%content_y.load53" = load i64, ptr %"%content_y", align 8
  %"%content_w.load54" = load i64, ptr %"%content_w", align 8
  %"%content_h.load55" = load i64, ptr %"%content_h", align 8
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\85\D8\AD\D8\B1\D8\B1"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load51", i64 %"%content_x.load52", i64 %"%content_y.load53", i64 %"%content_w.load54", i64 %"%content_h.load55")
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%\D9\81\D9\87\D8\B1\D8\B3.load61" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%content_x.load62" = load i64, ptr %"%content_x", align 8
  %"%content_y.load63" = load i64, ptr %"%content_y", align 8
  %"%content_w.load64" = load i64, ptr %"%content_w", align 8
  %"%content_h.load65" = load i64, ptr %"%content_h", align 8
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B7\D8\B1\D9\81\D9\8A\D8\A9"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load61", i64 %"%content_x.load62", i64 %"%content_y.load63", i64 %"%content_w.load64", i64 %"%content_h.load65")
  br label %merge_119

then_120:                                         ; preds = %merge_119
  br label %merge_121
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\85\D8\AA\D8\B5\D9\81\D8\AD_\D9\85\D9\84\D9\81\D8\A7\D8\AA"(i64 %"\D9\81\D9\87\D8\B1\D8\B3", i64 %cx, i64 %cy, i64 %cw, i64 %ch) {
entry:
  %"%item_y" = alloca i64, align 8
  %"%\D8\B5\D9\81" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D9\88\D9\81" = alloca i64, align 8
  %"%row_h" = alloca i64, align 8
  %ch5 = alloca i64, align 8
  store i64 %ch, ptr %ch5, align 8
  %cw4 = alloca i64, align 8
  store i64 %cw, ptr %cw4, align 8
  %cy3 = alloca i64, align 8
  store i64 %cy, ptr %cy3, align 8
  %cx2 = alloca i64, align 8
  store i64 %cx, ptr %cx2, align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  store i64 16, ptr %"%row_h", align 8
  %"%ch.load" = load i64, ptr %ch5, align 8
  %"%row_h.load" = load i64, ptr %"%row_h", align 8
  %divtmp = sdiv i64 %"%ch.load", %"%row_h.load"
  store i64 %divtmp, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D9\88\D9\81", align 8
  store i64 0, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_122

else_129:                                         ; preds = %then_125
  br label %merge_128

merge_126:                                        ; preds = %merge_128, %while_body_123
  %"%\D8\B5\D9\81.load12" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp13 = add i64 %"%\D8\B5\D9\81.load12", 1
  store i64 %addtmp13, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_122

merge_128:                                        ; preds = %else_129, %then_127
  br label %merge_126

then_125:                                         ; preds = %while_body_123
  %"%\D8\B5\D9\81.load8" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %"%row_h.load9" = load i64, ptr %"%row_h", align 8
  %multmp = mul i64 %"%\D8\B5\D9\81.load8", %"%row_h.load9"
  %"%cy.load" = load i64, ptr %cy3, align 8
  %addtmp = add i64 %"%cy.load", %multmp
  store i64 %addtmp, ptr %"%item_y", align 8
  %"%\D8\B5\D9\81.load10" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %cmplttmp11 = icmp slt i64 %"%\D8\B5\D9\81.load10", 2
  br i1 %cmplttmp11, label %then_127, label %else_129

then_127:                                         ; preds = %then_125
  br label %merge_128

while_body_123:                                   ; preds = %while_cond_122
  %"%\D8\B5\D9\81.load6" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %cmplttmp7 = icmp slt i64 %"%\D8\B5\D9\81.load6", 5
  br i1 %cmplttmp7, label %then_125, label %merge_126

while_cond_122:                                   ; preds = %merge_126, %entry
  %"%\D8\B5\D9\81.load" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D9\88\D9\81.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D9\88\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B5\D9\81.load", %"%\D8\B9\D8\AF\D8\AF_\D8\B5\D9\81\D9\88\D9\81.load"
  br i1 %cmplttmp, label %while_body_123, label %while_exit_124

while_exit_124:                                   ; preds = %while_cond_122
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\85\D8\AD\D8\B1\D8\B1"(i64 %"\D9\81\D9\87\D8\B1\D8\B3", i64 %cx, i64 %cy, i64 %cw, i64 %ch) {
entry:
  %ch5 = alloca i64, align 8
  store i64 %ch, ptr %ch5, align 8
  %cw4 = alloca i64, align 8
  store i64 %cw, ptr %cw4, align 8
  %cy3 = alloca i64, align 8
  store i64 %cy, ptr %cy3, align 8
  %cx2 = alloca i64, align 8
  store i64 %cx, ptr %cx2, align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B7\D8\B1\D9\81\D9\8A\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3", i64 %cx, i64 %cy, i64 %cw, i64 %ch) {
entry:
  %ch5 = alloca i64, align 8
  store i64 %ch, ptr %ch5, align 8
  %cw4 = alloca i64, align 8
  store i64 %cw, ptr %cw4, align 8
  %cy3 = alloca i64, align 8
  store i64 %cy, ptr %cy3, align 8
  %cx2 = alloca i64, align 8
  store i64 %cx, ptr %cx2, align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%my" = alloca i64, align 8
  %"%mx" = alloca i64, align 8
  store i64 0, ptr %"%mx", align 8
  store i64 0, ptr %"%my", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_130

merge_134:                                        ; preds = %merge_136, %while_body_131
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %addtmp4 = add i64 %"%\D9\81.load3", 1
  store i64 %addtmp4, ptr %"%\D9\81", align 8
  br label %while_cond_130

merge_136:                                        ; preds = %then_135, %then_133
  br label %merge_134

merge_141:                                        ; preds = %merge_143, %while_body_138
  %"%\D9\81.load15" = load i64, ptr %"%\D9\81", align 8
  %addtmp16 = add i64 %"%\D9\81.load15", 1
  store i64 %addtmp16, ptr %"%\D9\81", align 8
  br label %while_cond_137

merge_143:                                        ; preds = %then_142, %then_140
  br label %merge_141

merge_148:                                        ; preds = %merge_150, %while_body_145
  %"%\D9\81.load26" = load i64, ptr %"%\D9\81", align 8
  %addtmp27 = add i64 %"%\D9\81.load26", 1
  store i64 %addtmp27, ptr %"%\D9\81", align 8
  br label %while_cond_144

merge_150:                                        ; preds = %then_149, %then_147
  br label %merge_148

then_133:                                         ; preds = %while_body_131
  %"%my.load" = load i64, ptr %"%my", align 8
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%my.load", %"%\D9\81.load1"
  %cmplttmp2 = icmp slt i64 %addtmp, 0
  br i1 %cmplttmp2, label %then_135, label %merge_136

then_135:                                         ; preds = %then_133
  br label %merge_136

then_140:                                         ; preds = %while_body_138
  %"%my.load11" = load i64, ptr %"%my", align 8
  %"%\D9\81.load12" = load i64, ptr %"%\D9\81", align 8
  %addtmp13 = add i64 %"%my.load11", %"%\D9\81.load12"
  %cmplttmp14 = icmp slt i64 %addtmp13, 0
  br i1 %cmplttmp14, label %then_142, label %merge_143

then_142:                                         ; preds = %then_140
  br label %merge_143

then_147:                                         ; preds = %while_body_145
  %"%my.load23" = load i64, ptr %"%my", align 8
  %addtmp24 = add i64 %"%my.load23", 8
  %cmplttmp25 = icmp slt i64 %addtmp24, 0
  br i1 %cmplttmp25, label %then_149, label %merge_150

then_149:                                         ; preds = %then_147
  br label %merge_150

while_body_131:                                   ; preds = %while_cond_130
  %"%mx.load" = load i64, ptr %"%mx", align 8
  %cmpgetmp = icmp sge i64 %"%mx.load", 0
  br i1 %cmpgetmp, label %then_133, label %merge_134

while_body_138:                                   ; preds = %while_cond_137
  %"%mx.load7" = load i64, ptr %"%mx", align 8
  %"%\D9\81.load8" = load i64, ptr %"%\D9\81", align 8
  %addtmp9 = add i64 %"%mx.load7", %"%\D9\81.load8"
  %cmplttmp10 = icmp slt i64 %addtmp9, 0
  br i1 %cmplttmp10, label %then_140, label %merge_141

while_body_145:                                   ; preds = %while_cond_144
  %"%mx.load19" = load i64, ptr %"%mx", align 8
  %"%\D9\81.load20" = load i64, ptr %"%\D9\81", align 8
  %addtmp21 = add i64 %"%mx.load19", %"%\D9\81.load20"
  %cmplttmp22 = icmp slt i64 %addtmp21, 0
  br i1 %cmplttmp22, label %then_147, label %merge_148

while_cond_130:                                   ; preds = %merge_134, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 12
  br i1 %cmplttmp, label %while_body_131, label %while_exit_132

while_cond_137:                                   ; preds = %merge_141, %while_exit_132
  %"%\D9\81.load5" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp6 = icmp slt i64 %"%\D9\81.load5", 8
  br i1 %cmplttmp6, label %while_body_138, label %while_exit_139

while_cond_144:                                   ; preds = %merge_148, %while_exit_139
  %"%\D9\81.load17" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp18 = icmp slt i64 %"%\D9\81.load17", 5
  br i1 %cmplttmp18, label %while_body_145, label %while_exit_146

while_exit_132:                                   ; preds = %while_cond_130
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_137

while_exit_139:                                   ; preds = %while_cond_137
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_144

while_exit_146:                                   ; preds = %while_cond_144
  ret void
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\83\D9\84_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D9\81\D8\B0"() {
entry:
  %"%\D9\85\D8\A4\D9\82\D8\AA" = alloca i64, align 8
  %"%\D8\AC" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8" = alloca i64, align 8
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
  store i64 1, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 2, ptr %arr.elem8, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 3
  store i64 3, ptr %arr.elem11, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 4
  store i64 4, ptr %arr.elem14, align 8
  %arr.data.gep15 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data16 = load ptr, ptr %arr.data.gep15, align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 5
  store i64 5, ptr %arr.elem17, align 8
  %arr.data.gep18 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data19 = load ptr, ptr %arr.data.gep18, align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 6
  store i64 6, ptr %arr.elem20, align 8
  %arr.data.gep21 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data22 = load ptr, ptr %arr.data.gep21, align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 7
  store i64 7, ptr %arr.elem23, align 8
  %arr.data.gep24 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data25 = load ptr, ptr %arr.data.gep24, align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 8
  store i64 8, ptr %arr.elem26, align 8
  %arr.data.gep27 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data28 = load ptr, ptr %arr.data.gep27, align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 9
  store i64 9, ptr %arr.elem29, align 8
  %arr.data.gep30 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data31 = load ptr, ptr %arr.data.gep30, align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 10
  store i64 10, ptr %arr.elem32, align 8
  %arr.data.gep33 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data34 = load ptr, ptr %arr.data.gep33, align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 11
  store i64 11, ptr %arr.elem35, align 8
  %arr.data.gep36 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data37 = load ptr, ptr %arr.data.gep36, align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 12
  store i64 12, ptr %arr.elem38, align 8
  %arr.data.gep39 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data40 = load ptr, ptr %arr.data.gep39, align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 13
  store i64 13, ptr %arr.elem41, align 8
  %arr.data.gep42 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data43 = load ptr, ptr %arr.data.gep42, align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 14
  store i64 14, ptr %arr.elem44, align 8
  %arr.data.gep45 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data46 = load ptr, ptr %arr.data.gep45, align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 15
  store i64 15, ptr %arr.elem47, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_151

merge_158:                                        ; preds = %then_157, %while_body_155
  %"%\D8\AC.load95" = load i64, ptr %"%\D8\AC", align 8
  %addtmp96 = add i64 %"%\D8\AC.load95", 1
  store i64 %addtmp96, ptr %"%\D8\AC", align 8
  br label %while_cond_154

then_157:                                         ; preds = %while_body_155
  %"%\D8\AC.load69" = load i64, ptr %"%\D8\AC", align 8
  %arr.ptr.int70 = load i64, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  %arr.ptr71 = inttoptr i64 %arr.ptr.int70 to ptr
  %arr.data.gep72 = getelementptr inbounds %SadArray, ptr %arr.ptr71, i32 0, i32 2
  %arr.data73 = load ptr, ptr %arr.data.gep72, align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 %"%\D8\AC.load69"
  %arr.get75 = load i64, ptr %arr.elem74, align 8
  store i64 %arr.get75, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %"%\D8\AC.load76" = load i64, ptr %"%\D8\AC", align 8
  %addtmp77 = add i64 %"%\D8\AC.load76", 1
  %arr.ptr.int78 = load i64, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  %arr.ptr79 = inttoptr i64 %arr.ptr.int78 to ptr
  %arr.data.gep80 = getelementptr inbounds %SadArray, ptr %arr.ptr79, i32 0, i32 2
  %arr.data81 = load ptr, ptr %arr.data.gep80, align 8
  %arr.elem82 = getelementptr i64, ptr %arr.data81, i64 %addtmp77
  %arr.get83 = load i64, ptr %arr.elem82, align 8
  %"%\D8\AC.load84" = load i64, ptr %"%\D8\AC", align 8
  %arr.ptr.int85 = load i64, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  %arr.ptr86 = inttoptr i64 %arr.ptr.int85 to ptr
  %arr.data.gep87 = getelementptr inbounds %SadArray, ptr %arr.ptr86, i32 0, i32 2
  %arr.data88 = load ptr, ptr %arr.data.gep87, align 8
  %arr.elem89 = getelementptr i64, ptr %arr.data88, i64 %"%\D8\AC.load84"
  store i64 %arr.get83, ptr %arr.elem89, align 8
  %"%\D9\85\D8\A4\D9\82\D8\AA.load" = load i64, ptr %"%\D9\85\D8\A4\D9\82\D8\AA", align 8
  %arr.ptr.int90 = load i64, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  %arr.ptr91 = inttoptr i64 %arr.ptr.int90 to ptr
  %arr.data.gep92 = getelementptr inbounds %SadArray, ptr %arr.ptr91, i32 0, i32 2
  %arr.data93 = load ptr, ptr %arr.data.gep92, align 8
  %arr.elem94 = getelementptr i64, ptr %arr.data93, i64 %addtmp77
  store i64 %"%\D9\85\D8\A4\D9\82\D8\AA.load", ptr %arr.elem94, align 8
  br label %merge_158

while_body_152:                                   ; preds = %while_cond_151
  store i64 0, ptr %"%\D8\AC", align 8
  br label %while_cond_154

while_body_155:                                   ; preds = %while_cond_154
  %"%\D8\AC.load50" = load i64, ptr %"%\D8\AC", align 8
  %arr.ptr.int = load i64, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  %arr.ptr = inttoptr i64 %arr.ptr.int to ptr
  %arr.data.gep51 = getelementptr inbounds %SadArray, ptr %arr.ptr, i32 0, i32 2
  %arr.data52 = load ptr, ptr %arr.data.gep51, align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 %"%\D8\AC.load50"
  %arr.get = load i64, ptr %arr.elem53, align 8
  %arr.data54 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem55 = getelementptr i64, ptr %arr.data54, i64 %arr.get
  %arr.get56 = load i64, ptr %arr.elem55, align 8
  %"%\D8\AC.load57" = load i64, ptr %"%\D8\AC", align 8
  %addtmp = add i64 %"%\D8\AC.load57", 1
  %arr.ptr.int58 = load i64, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  %arr.ptr59 = inttoptr i64 %arr.ptr.int58 to ptr
  %arr.data.gep60 = getelementptr inbounds %SadArray, ptr %arr.ptr59, i32 0, i32 2
  %arr.data61 = load ptr, ptr %arr.data.gep60, align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 %addtmp
  %arr.get63 = load i64, ptr %arr.elem62, align 8
  %arr.data64 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 %arr.get63
  %arr.get66 = load i64, ptr %arr.elem65, align 8
  %cmpgttmp = icmp sgt i64 %arr.get56, %arr.get66
  br i1 %cmpgttmp, label %then_157, label %merge_158

while_body_160:                                   ; preds = %while_cond_159
  %"%\D9\81.load99" = load i64, ptr %"%\D9\81", align 8
  %arr.ptr.int100 = load i64, ptr %"%\D8\AA\D8\B1\D8\AA\D9\8A\D8\A8", align 8
  %arr.ptr101 = inttoptr i64 %arr.ptr.int100 to ptr
  %arr.data.gep102 = getelementptr inbounds %SadArray, ptr %arr.ptr101, i32 0, i32 2
  %arr.data103 = load ptr, ptr %arr.data.gep102, align 8
  %arr.elem104 = getelementptr i64, ptr %arr.data103, i64 %"%\D9\81.load99"
  %arr.get105 = load i64, ptr %arr.elem104, align 8
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %arr.get105)
  %"%\D9\81.load106" = load i64, ptr %"%\D9\81", align 8
  %addtmp107 = add i64 %"%\D9\81.load106", 1
  store i64 %addtmp107, ptr %"%\D9\81", align 8
  br label %while_cond_159

while_cond_151:                                   ; preds = %while_exit_156, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", -1
  br i1 %cmplttmp, label %while_body_152, label %while_exit_153

while_cond_154:                                   ; preds = %merge_158, %while_body_152
  %"%\D9\81.load48" = load i64, ptr %"%\D9\81", align 8
  %subtmp = sub i64 -1, %"%\D9\81.load48"
  %"%\D8\AC.load" = load i64, ptr %"%\D8\AC", align 8
  %cmplttmp49 = icmp slt i64 %"%\D8\AC.load", %subtmp
  br i1 %cmplttmp49, label %while_body_155, label %while_exit_156

while_cond_159:                                   ; preds = %while_body_160, %while_exit_153
  %"%\D9\81.load97" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp98 = icmp slt i64 %"%\D9\81.load97", 0
  br i1 %cmplttmp98, label %while_body_160, label %while_exit_161

while_exit_153:                                   ; preds = %while_cond_151
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_159

while_exit_156:                                   ; preds = %while_cond_154
  %"%\D9\81.load67" = load i64, ptr %"%\D9\81", align 8
  %addtmp68 = add i64 %"%\D9\81.load67", 1
  store i64 %addtmp68, ptr %"%\D9\81", align 8
  br label %while_cond_151

while_exit_161:                                   ; preds = %while_cond_159
  ret void
}

define void @"\D8\A3\D8\B9\D8\AF_\D8\A7\D9\84\D8\B1\D8\B3\D9\85"() {
entry:
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B3\D8\B7\D8\AD_\D8\A7\D9\84\D9\85\D9\83\D8\AA\D8\A8"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D9\83\D9\84_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D9\81\D8\B0"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85"()
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\A4\D8\B4\D8\B1"()
  ret void
}

define i64 @"\D8\AC\D8\AF_\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\B9\D9\86\D8\AF"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89_z" = alloca i64, align 8
  %"%\D8\A3\D9\81\D8\B6\D9\84" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  store i64 -1, ptr %"%\D8\A3\D9\81\D8\B6\D9\84", align 8
  store i64 -1, ptr %"%\D8\A3\D8\B9\D9\84\D9\89_z", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_162

merge_166:                                        ; preds = %merge_168, %while_body_163
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load9", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_162

merge_168:                                        ; preds = %merge_170, %then_165
  br label %merge_166

merge_170:                                        ; preds = %merge_172, %then_167
  br label %merge_168

merge_172:                                        ; preds = %merge_174, %then_169
  br label %merge_170

merge_174:                                        ; preds = %merge_176, %then_171
  br label %merge_172

merge_176:                                        ; preds = %merge_178, %then_173
  br label %merge_174

merge_178:                                        ; preds = %then_177, %then_175
  br label %merge_176

then_165:                                         ; preds = %while_body_163
  %"%\D9\81.load4" = load i64, ptr %"%\D9\81", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %cmpnetmp8 = icmp ne i64 %arr.get7, 0
  br i1 %cmpnetmp8, label %then_167, label %merge_168

then_167:                                         ; preds = %then_165
  %"%\D9\81.load10" = load i64, ptr %"%\D9\81", align 8
  %arr.data11 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem12 = getelementptr i64, ptr %arr.data11, i64 %"%\D9\81.load10"
  %arr.get13 = load i64, ptr %arr.elem12, align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %arr.get13
  br i1 %cmpgetmp, label %then_169, label %merge_170

then_169:                                         ; preds = %then_167
  %"%\D9\81.load14" = load i64, ptr %"%\D9\81", align 8
  %arr.data15 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem16 = getelementptr i64, ptr %arr.data15, i64 %"%\D9\81.load14"
  %arr.get17 = load i64, ptr %arr.elem16, align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmpgetmp18 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load", %arr.get17
  br i1 %cmpgetmp18, label %then_171, label %merge_172

then_171:                                         ; preds = %then_169
  %"%\D9\81.load19" = load i64, ptr %"%\D9\81", align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 %"%\D9\81.load19"
  %arr.get22 = load i64, ptr %arr.elem21, align 8
  %"%\D9\81.load23" = load i64, ptr %"%\D9\81", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D9\81.load23"
  %arr.get26 = load i64, ptr %arr.elem25, align 8
  %addtmp27 = add i64 %arr.get22, %arr.get26
  %"%\D8\B3\D8\B7\D8\AD_x.load28" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp29 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load28", %addtmp27
  br i1 %cmplttmp29, label %then_173, label %merge_174

then_173:                                         ; preds = %then_171
  %"%\D9\81.load30" = load i64, ptr %"%\D9\81", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D9\81.load30"
  %arr.get33 = load i64, ptr %arr.elem32, align 8
  %"%\D9\81.load34" = load i64, ptr %"%\D9\81", align 8
  %arr.data35 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 %"%\D9\81.load34"
  %arr.get37 = load i64, ptr %arr.elem36, align 8
  %addtmp38 = add i64 %arr.get33, %arr.get37
  %"%\D8\B3\D8\B7\D8\AD_y.load39" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp40 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load39", %addtmp38
  br i1 %cmplttmp40, label %then_175, label %merge_176

then_175:                                         ; preds = %then_173
  %"%\D9\81.load41" = load i64, ptr %"%\D9\81", align 8
  %arr.data42 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 %"%\D9\81.load41"
  %arr.get44 = load i64, ptr %arr.elem43, align 8
  %"%\D8\A3\D8\B9\D9\84\D9\89_z.load" = load i64, ptr %"%\D8\A3\D8\B9\D9\84\D9\89_z", align 8
  %cmpgttmp = icmp sgt i64 %arr.get44, %"%\D8\A3\D8\B9\D9\84\D9\89_z.load"
  br i1 %cmpgttmp, label %then_177, label %merge_178

then_177:                                         ; preds = %then_175
  %"%\D9\81.load45" = load i64, ptr %"%\D9\81", align 8
  %arr.data46 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 %"%\D9\81.load45"
  %arr.get48 = load i64, ptr %arr.elem47, align 8
  store i64 %arr.get48, ptr %"%\D8\A3\D8\B9\D9\84\D9\89_z", align 8
  %"%\D9\81.load49" = load i64, ptr %"%\D9\81", align 8
  store i64 %"%\D9\81.load49", ptr %"%\D8\A3\D9\81\D8\B6\D9\84", align 8
  br label %merge_178

while_body_163:                                   ; preds = %while_cond_162
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_165, label %merge_166

while_cond_162:                                   ; preds = %merge_166, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_163, label %while_exit_164

while_exit_164:                                   ; preds = %while_cond_162
  %loadtmp = load i64, ptr %"%\D8\A3\D9\81\D8\B6\D9\84", align 8
  ret i64 %loadtmp
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1"(i64 %"\D9\81\D9\87\D8\B1\D8\B3", i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%min_x" = alloca i64, align 8
  %"%max_x" = alloca i64, align 8
  %"%close_x" = alloca i64, align 8
  %"%ww" = alloca i64, align 8
  %"%wy" = alloca i64, align 8
  %"%wx" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y3" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %"\D8\B3\D8\B7\D8\AD_x2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%wx", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  store i64 %arr.get7, ptr %"%wy", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load8" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load8"
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  store i64 %arr.get11, ptr %"%ww", align 8
  %"%wy.load" = load i64, ptr %"%wy", align 8
  %addtmp = add i64 %"%wy.load", 0
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load", %addtmp
  br i1 %cmplttmp, label %then_179, label %merge_180

merge_180:                                        ; preds = %entry
  %"%wy.load12" = load i64, ptr %"%wy", align 8
  %addtmp13 = add i64 %"%wy.load12", 0
  %addtmp14 = add i64 %addtmp13, 0
  %"%\D8\B3\D8\B7\D8\AD_y.load15" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B3\D8\B7\D8\AD_y.load15", %addtmp14
  br i1 %cmpgttmp, label %then_181, label %merge_182

merge_182:                                        ; preds = %merge_180
  %"%wx.load" = load i64, ptr %"%wx", align 8
  %"%ww.load" = load i64, ptr %"%ww", align 8
  %addtmp16 = add i64 %"%wx.load", %"%ww.load"
  %subtmp = sub i64 %addtmp16, 0
  %subtmp17 = sub i64 %subtmp, 0
  %subtmp18 = sub i64 %subtmp17, 2
  store i64 %subtmp18, ptr %"%close_x", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"%close_x.load" = load i64, ptr %"%close_x", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %"%close_x.load"
  br i1 %cmpgetmp, label %then_183, label %merge_184

merge_184:                                        ; preds = %merge_186, %merge_182
  %"%close_x.load22" = load i64, ptr %"%close_x", align 8
  %subtmp23 = sub i64 %"%close_x.load22", 0
  %subtmp24 = sub i64 %subtmp23, 2
  store i64 %subtmp24, ptr %"%max_x", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load25" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"%max_x.load" = load i64, ptr %"%max_x", align 8
  %cmpgetmp26 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load25", %"%max_x.load"
  br i1 %cmpgetmp26, label %then_187, label %merge_188

merge_186:                                        ; preds = %then_183
  br label %merge_184

merge_188:                                        ; preds = %merge_190, %merge_184
  %"%max_x.load31" = load i64, ptr %"%max_x", align 8
  %subtmp32 = sub i64 %"%max_x.load31", 0
  %subtmp33 = sub i64 %subtmp32, 2
  store i64 %subtmp33, ptr %"%min_x", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load34" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"%min_x.load" = load i64, ptr %"%min_x", align 8
  %cmpgetmp35 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load34", %"%min_x.load"
  br i1 %cmpgetmp35, label %then_191, label %merge_192

merge_190:                                        ; preds = %then_187
  br label %merge_188

merge_192:                                        ; preds = %merge_194, %merge_188
  ret i64 4

merge_194:                                        ; preds = %then_191
  br label %merge_192

then_179:                                         ; preds = %entry
  ret i64 0

then_181:                                         ; preds = %merge_180
  ret i64 0

then_183:                                         ; preds = %merge_182
  %"%close_x.load19" = load i64, ptr %"%close_x", align 8
  %addtmp20 = add i64 %"%close_x.load19", 0
  %"%\D8\B3\D8\B7\D8\AD_x.load21" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %cmpletmp = icmp sle i64 %"%\D8\B3\D8\B7\D8\AD_x.load21", %addtmp20
  br i1 %cmpletmp, label %then_185, label %merge_186

then_185:                                         ; preds = %then_183
  ret i64 1

then_187:                                         ; preds = %merge_184
  %"%max_x.load27" = load i64, ptr %"%max_x", align 8
  %addtmp28 = add i64 %"%max_x.load27", 0
  %"%\D8\B3\D8\B7\D8\AD_x.load29" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %cmpletmp30 = icmp sle i64 %"%\D8\B3\D8\B7\D8\AD_x.load29", %addtmp28
  br i1 %cmpletmp30, label %then_189, label %merge_190

then_189:                                         ; preds = %then_187
  ret i64 2

then_191:                                         ; preds = %merge_188
  %"%min_x.load36" = load i64, ptr %"%min_x", align 8
  %addtmp37 = add i64 %"%min_x.load36", 0
  %"%\D8\B3\D8\B7\D8\AD_x.load38" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %cmpletmp39 = icmp sle i64 %"%\D8\B3\D8\B7\D8\AD_x.load38", %addtmp37
  br i1 %cmpletmp39, label %then_193, label %merge_194

then_193:                                         ; preds = %then_191
  ret i64 3
}

define i64 @"\D9\81\D8\AD\D8\B5_\D9\85\D9\82\D8\A8\D8\B6_\D8\AD\D8\AC\D9\85"(i64 %"\D9\81\D9\87\D8\B1\D8\B3", i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%ry" = alloca i64, align 8
  %"%rx" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y3" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %"\D8\B3\D8\B7\D8\AD_x2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B31" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3", ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_195, label %merge_196

merge_196:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data5 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem6 = getelementptr i64, ptr %arr.data5, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load4"
  %arr.get7 = load i64, ptr %arr.elem6, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load8" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load8"
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  %addtmp = add i64 %arr.get7, %arr.get11
  %subtmp = sub i64 %addtmp, 8
  store i64 %subtmp, ptr %"%rx", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load12" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load12"
  %arr.get15 = load i64, ptr %arr.elem14, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load16" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B31", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load16"
  %arr.get19 = load i64, ptr %arr.elem18, align 8
  %addtmp20 = add i64 %arr.get15, %arr.get19
  %subtmp21 = sub i64 %addtmp20, 8
  store i64 %subtmp21, ptr %"%ry", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %"%rx.load" = load i64, ptr %"%rx", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load", %"%rx.load"
  br i1 %cmpgetmp, label %then_197, label %merge_198

merge_198:                                        ; preds = %merge_200, %merge_196
  ret i64 0

merge_200:                                        ; preds = %merge_202, %then_197
  br label %merge_198

merge_202:                                        ; preds = %merge_204, %then_199
  br label %merge_200

merge_204:                                        ; preds = %then_201
  br label %merge_202

then_195:                                         ; preds = %entry
  ret i64 0

then_197:                                         ; preds = %merge_196
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %"%ry.load" = load i64, ptr %"%ry", align 8
  %cmpgetmp22 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_y.load", %"%ry.load"
  br i1 %cmpgetmp22, label %then_199, label %merge_200

then_199:                                         ; preds = %then_197
  %"%rx.load23" = load i64, ptr %"%rx", align 8
  %addtmp24 = add i64 %"%rx.load23", 8
  %"%\D8\B3\D8\B7\D8\AD_x.load25" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load25", %addtmp24
  br i1 %cmplttmp, label %then_201, label %merge_202

then_201:                                         ; preds = %then_199
  %"%ry.load26" = load i64, ptr %"%ry", align 8
  %addtmp27 = add i64 %"%ry.load26", 8
  %"%\D8\B3\D8\B7\D8\AD_y.load28" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y3", align 8
  %cmplttmp29 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load28", %addtmp27
  br i1 %cmplttmp29, label %then_203, label %merge_204

then_203:                                         ; preds = %then_201
  ret i64 1
}

define i64 @"\D9\81\D8\AD\D8\B5_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%btn_x" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_y.load", 0
  br i1 %cmplttmp, label %then_205, label %merge_206

merge_206:                                        ; preds = %entry
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load", 0
  br i1 %cmpgetmp, label %then_207, label %merge_208

merge_208:                                        ; preds = %merge_210, %merge_206
  store i64 4, ptr %"%btn_x", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_211

merge_210:                                        ; preds = %then_207
  br label %merge_208

merge_215:                                        ; preds = %merge_217, %while_body_212
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load9", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_211

merge_217:                                        ; preds = %merge_219, %then_214
  %"%btn_x.load14" = load i64, ptr %"%btn_x", align 8
  %addtmp15 = add i64 %"%btn_x.load14", 0
  %addtmp16 = add i64 %addtmp15, 2
  store i64 %addtmp16, ptr %"%btn_x", align 8
  br label %merge_215

merge_219:                                        ; preds = %then_216
  br label %merge_217

then_205:                                         ; preds = %entry
  ret i64 -1

then_207:                                         ; preds = %merge_206
  %"%\D8\B3\D8\B7\D8\AD_x.load3" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp4 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load3", 0
  br i1 %cmplttmp4, label %then_209, label %merge_210

then_209:                                         ; preds = %then_207
  ret i64 -2

then_214:                                         ; preds = %while_body_212
  %"%\D8\B3\D8\B7\D8\AD_x.load7" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%btn_x.load" = load i64, ptr %"%btn_x", align 8
  %cmpgetmp8 = icmp sge i64 %"%\D8\B3\D8\B7\D8\AD_x.load7", %"%btn_x.load"
  br i1 %cmpgetmp8, label %then_216, label %merge_217

then_216:                                         ; preds = %then_214
  %"%btn_x.load10" = load i64, ptr %"%btn_x", align 8
  %addtmp11 = add i64 %"%btn_x.load10", 0
  %"%\D8\B3\D8\B7\D8\AD_x.load12" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %cmplttmp13 = icmp slt i64 %"%\D8\B3\D8\B7\D8\AD_x.load12", %addtmp11
  br i1 %cmplttmp13, label %then_218, label %merge_219

then_218:                                         ; preds = %then_216
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_212:                                   ; preds = %while_cond_211
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load6"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_214, label %merge_215

while_cond_211:                                   ; preds = %merge_215, %merge_208
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp5 = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp5, label %while_body_212, label %while_exit_213

while_exit_213:                                   ; preds = %while_cond_211
  ret i64 -1
}

define void @"\D8\B9\D8\A7\D9\84\D8\AC_\D9\86\D9\82\D8\B1"(i64 %"\D8\B3\D8\B7\D8\AD_x", i64 %"\D8\B3\D8\B7\D8\AD_y") {
entry:
  %"%\D8\B2\D8\B1" = alloca i64, align 8
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9" = alloca i64, align 8
  %"%tb_result" = alloca i64, align 8
  %"\D8\B3\D8\B7\D8\AD_y2" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_y", ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %"\D8\B3\D8\B7\D8\AD_x1" = alloca i64, align 8
  store i64 %"\D8\B3\D8\B7\D8\AD_x", ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %calltmp = call i64 @"\D9\81\D8\AD\D8\B5_\D8\B4\D8\B1\D9\8A\D8\B7_\D8\A7\D9\84\D9\85\D9\87\D8\A7\D9\85"(i64 %"%\D8\B3\D8\B7\D8\AD_x.load", i64 %"%\D8\B3\D8\B7\D8\AD_y.load")
  store i64 %calltmp, ptr %"%tb_result", align 8
  %"%tb_result.load" = load i64, ptr %"%tb_result", align 8
  %cmpeqtmp = icmp eq i64 %"%tb_result.load", -2
  br i1 %cmpeqtmp, label %then_220, label %merge_221

else_226:                                         ; preds = %then_222
  %"%tb_result.load13" = load i64, ptr %"%tb_result", align 8
  %cmpeqtmp14 = icmp eq i64 %"%tb_result.load13", 0
  br i1 %cmpeqtmp14, label %then_227, label %else_229

else_229:                                         ; preds = %else_226
  %"%tb_result.load16" = load i64, ptr %"%tb_result", align 8
  call void @"gui_\D8\B1\D9\81\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%tb_result.load16")
  br label %merge_228

merge_221:                                        ; preds = %entry
  %"%tb_result.load3" = load i64, ptr %"%tb_result", align 8
  %cmpgetmp = icmp sge i64 %"%tb_result.load3", 0
  br i1 %cmpgetmp, label %then_222, label %merge_223

merge_223:                                        ; preds = %merge_221
  %"%\D8\B3\D8\B7\D8\AD_x.load6" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load7" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %calltmp8 = call i64 @"\D8\AC\D8\AF_\D9\86\D8\A7\D9\81\D8\B0\D8\A9_\D8\B9\D9\86\D8\AF"(i64 %"%\D8\B3\D8\B7\D8\AD_x.load6", i64 %"%\D8\B3\D8\B7\D8\AD_y.load7")
  store i64 %calltmp8, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load", 0
  br i1 %cmplttmp, label %then_230, label %merge_231

merge_225:                                        ; preds = %merge_228, %then_224
  ret void

merge_228:                                        ; preds = %else_229, %then_227
  br label %merge_225

merge_231:                                        ; preds = %merge_223
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load17" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  call void @"gui_\D8\B1\D9\81\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load17")
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load18" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load19" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load20" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %calltmp21 = call i64 @"\D9\81\D8\AD\D8\B5_\D9\85\D9\82\D8\A8\D8\B6_\D8\AD\D8\AC\D9\85"(i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load18", i64 %"%\D8\B3\D8\B7\D8\AD_x.load19", i64 %"%\D8\B3\D8\B7\D8\AD_y.load20")
  %cmpeqtmp22 = icmp eq i64 %calltmp21, 1
  br i1 %cmpeqtmp22, label %then_232, label %merge_233

merge_233:                                        ; preds = %merge_231
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load23" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  %"%\D8\B3\D8\B7\D8\AD_x.load24" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_x1", align 8
  %"%\D8\B3\D8\B7\D8\AD_y.load25" = load i64, ptr %"\D8\B3\D8\B7\D8\AD_y2", align 8
  %calltmp26 = call i64 @"\D9\81\D8\AD\D8\B5_\D8\A3\D8\B2\D8\B1\D8\A7\D8\B1"(i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load23", i64 %"%\D8\B3\D8\B7\D8\AD_x.load24", i64 %"%\D8\B3\D8\B7\D8\AD_y.load25")
  store i64 %calltmp26, ptr %"%\D8\B2\D8\B1", align 8
  %"%\D8\B2\D8\B1.load" = load i64, ptr %"%\D8\B2\D8\B1", align 8
  %cmpeqtmp27 = icmp eq i64 %"%\D8\B2\D8\B1.load", 1
  br i1 %cmpeqtmp27, label %then_234, label %merge_235

merge_235:                                        ; preds = %merge_233
  %"%\D8\B2\D8\B1.load29" = load i64, ptr %"%\D8\B2\D8\B1", align 8
  %cmpeqtmp30 = icmp eq i64 %"%\D8\B2\D8\B1.load29", 2
  br i1 %cmpeqtmp30, label %then_236, label %merge_237

merge_237:                                        ; preds = %merge_235
  %"%\D8\B2\D8\B1.load32" = load i64, ptr %"%\D8\B2\D8\B1", align 8
  %cmpeqtmp33 = icmp eq i64 %"%\D8\B2\D8\B1.load32", 3
  br i1 %cmpeqtmp33, label %then_238, label %merge_239

merge_239:                                        ; preds = %merge_237
  %"%\D8\B2\D8\B1.load35" = load i64, ptr %"%\D8\B2\D8\B1", align 8
  %cmpeqtmp36 = icmp eq i64 %"%\D8\B2\D8\B1.load35", 4
  br i1 %cmpeqtmp36, label %then_240, label %merge_241

merge_241:                                        ; preds = %merge_239
  ret void

merge_243:                                        ; preds = %then_242, %then_240
  ret void

then_220:                                         ; preds = %entry
  ret void

then_222:                                         ; preds = %merge_221
  %"%tb_result.load4" = load i64, ptr %"%tb_result", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%tb_result.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp5 = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp5, label %then_224, label %else_226

then_224:                                         ; preds = %then_222
  %"%tb_result.load9" = load i64, ptr %"%tb_result", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%tb_result.load9"
  store i64 0, ptr %arr.elem11, align 8
  %"%tb_result.load12" = load i64, ptr %"%tb_result", align 8
  call void @"gui_\D8\B1\D9\81\D8\B9_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%tb_result.load12")
  br label %merge_225

then_227:                                         ; preds = %else_226
  %"%tb_result.load15" = load i64, ptr %"%tb_result", align 8
  call void @"gui_\D8\B5\D8\BA\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%tb_result.load15")
  br label %merge_228

then_230:                                         ; preds = %merge_223
  ret void

then_232:                                         ; preds = %merge_231
  ret void

then_234:                                         ; preds = %merge_233
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load28" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  call void @"gui_\D8\A3\D8\BA\D9\84\D9\82_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load28")
  ret void

then_236:                                         ; preds = %merge_235
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load31" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  call void @"gui_\D9\83\D8\A8\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load31")
  ret void

then_238:                                         ; preds = %merge_237
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load34" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  call void @"gui_\D8\B5\D8\BA\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load34")
  ret void

then_240:                                         ; preds = %merge_239
  %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load37" = load i64, ptr %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9", align 8
  %arr.data38 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 %"%\D9\86\D8\A7\D9\81\D8\B0\D8\A9.load37"
  %arr.get40 = load i64, ptr %arr.elem39, align 8
  %cmpnetmp = icmp ne i64 %arr.get40, 0
  br i1 %cmpnetmp, label %then_242, label %merge_243

then_242:                                         ; preds = %then_240
  br label %merge_243
}

define void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\B3\D8\AD\D8\A8"() {
entry:
  %"%new_y" = alloca i64, align 8
  %"%new_x" = alloca i64, align 8
  br i1 true, label %then_244, label %merge_245

merge_245:                                        ; preds = %entry
  store i64 0, ptr %"%new_x", align 8
  store i64 0, ptr %"%new_y", align 8
  %"%new_x.load" = load i64, ptr %"%new_x", align 8
  %cmplttmp = icmp slt i64 %"%new_x.load", 0
  br i1 %cmplttmp, label %then_246, label %merge_247

merge_247:                                        ; preds = %then_246, %merge_245
  %"%new_y.load" = load i64, ptr %"%new_y", align 8
  %cmplttmp1 = icmp slt i64 %"%new_y.load", 0
  br i1 %cmplttmp1, label %then_248, label %merge_249

merge_249:                                        ; preds = %then_248, %merge_247
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  %arr.get = load i64, ptr %arr.elem, align 8
  %"%new_x.load2" = load i64, ptr %"%new_x", align 8
  %addtmp = add i64 %"%new_x.load2", %arr.get
  %cmpgttmp = icmp sgt i64 %addtmp, 0
  br i1 %cmpgttmp, label %then_250, label %merge_251

merge_251:                                        ; preds = %then_250, %merge_249
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 0
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  %"%new_y.load9" = load i64, ptr %"%new_y", align 8
  %addtmp10 = add i64 %"%new_y.load9", %arr.get8
  %cmpgttmp11 = icmp sgt i64 %addtmp10, 0
  br i1 %cmpgttmp11, label %then_252, label %merge_253

merge_253:                                        ; preds = %then_252, %merge_251
  %"%new_x.load16" = load i64, ptr %"%new_x", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 0
  store i64 %"%new_x.load16", ptr %arr.elem18, align 8
  %"%new_y.load19" = load i64, ptr %"%new_y", align 8
  %arr.data20 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem21 = getelementptr i64, ptr %arr.data20, i64 0
  store i64 %"%new_y.load19", ptr %arr.elem21, align 8
  ret void

then_244:                                         ; preds = %entry
  ret void

then_246:                                         ; preds = %merge_245
  store i64 0, ptr %"%new_x", align 8
  br label %merge_247

then_248:                                         ; preds = %merge_247
  store i64 0, ptr %"%new_y", align 8
  br label %merge_249

then_250:                                         ; preds = %merge_249
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 0
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %subtmp = sub i64 0, %arr.get5
  store i64 %subtmp, ptr %"%new_x", align 8
  br label %merge_251

then_252:                                         ; preds = %merge_251
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 0
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  %subtmp15 = sub i64 0, %arr.get14
  store i64 %subtmp15, ptr %"%new_y", align 8
  br label %merge_253
}

define void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\AA\D8\BA\D9\8A\D9\8A\D8\B1_\D8\AD\D8\AC\D9\85"() {
entry:
  %"%new_h" = alloca i64, align 8
  %"%new_w" = alloca i64, align 8
  %"%dy" = alloca i64, align 8
  %"%dx" = alloca i64, align 8
  br i1 true, label %then_254, label %merge_255

merge_255:                                        ; preds = %entry
  store i64 0, ptr %"%dx", align 8
  store i64 0, ptr %"%dy", align 8
  %"%dx.load" = load i64, ptr %"%dx", align 8
  %addtmp = add i64 0, %"%dx.load"
  store i64 %addtmp, ptr %"%new_w", align 8
  %"%dy.load" = load i64, ptr %"%dy", align 8
  %addtmp1 = add i64 0, %"%dy.load"
  store i64 %addtmp1, ptr %"%new_h", align 8
  %"%new_w.load" = load i64, ptr %"%new_w", align 8
  %cmplttmp = icmp slt i64 %"%new_w.load", 0
  br i1 %cmplttmp, label %then_256, label %merge_257

merge_257:                                        ; preds = %then_256, %merge_255
  %"%new_h.load" = load i64, ptr %"%new_h", align 8
  %cmplttmp2 = icmp slt i64 %"%new_h.load", 0
  br i1 %cmplttmp2, label %then_258, label %merge_259

merge_259:                                        ; preds = %then_258, %merge_257
  %"%new_w.load3" = load i64, ptr %"%new_w", align 8
  %cmpgttmp = icmp sgt i64 %"%new_w.load3", 0
  br i1 %cmpgttmp, label %then_260, label %merge_261

merge_261:                                        ; preds = %then_260, %merge_259
  %"%new_h.load4" = load i64, ptr %"%new_h", align 8
  %cmpgttmp5 = icmp sgt i64 %"%new_h.load4", 0
  br i1 %cmpgttmp5, label %then_262, label %merge_263

merge_263:                                        ; preds = %then_262, %merge_261
  %"%new_w.load6" = load i64, ptr %"%new_w", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 0
  store i64 %"%new_w.load6", ptr %arr.elem, align 8
  %"%new_h.load7" = load i64, ptr %"%new_h", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 0
  store i64 %"%new_h.load7", ptr %arr.elem9, align 8
  ret void

then_254:                                         ; preds = %entry
  ret void

then_256:                                         ; preds = %merge_255
  br label %merge_257

then_258:                                         ; preds = %merge_257
  br label %merge_259

then_260:                                         ; preds = %merge_259
  br label %merge_261

then_262:                                         ; preds = %merge_261
  br label %merge_263
}

define void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"() {
entry:
  ret void
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D9\88\D8\A7\D8\AC\D9\87\D8\A9"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  %calltmp = call i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\AE\D8\B7"()
  %calltmp1 = call i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D9\85\D8\A7\D9\88\D8\B3"()
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_264

while_body_265:                                   ; preds = %while_cond_264
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load2"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81.load3"
  store i64 0, ptr %arr.elem5, align 8
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81.load6"
  store i64 0, ptr %arr.elem8, align 8
  %"%\D9\81.load9" = load i64, ptr %"%\D9\81", align 8
  %arr.data10 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 %"%\D9\81.load9"
  store i64 0, ptr %arr.elem11, align 8
  %"%\D9\81.load12" = load i64, ptr %"%\D9\81", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D9\81.load12"
  store i64 0, ptr %arr.elem14, align 8
  %"%\D9\81.load15" = load i64, ptr %"%\D9\81", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D9\81.load15"
  store i64 0, ptr %arr.elem17, align 8
  %"%\D9\81.load18" = load i64, ptr %"%\D9\81", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D9\81.load18"
  store i64 0, ptr %arr.elem20, align 8
  %"%\D9\81.load21" = load i64, ptr %"%\D9\81", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81.load21"
  store i64 0, ptr %arr.elem23, align 8
  %"%\D9\81.load24" = load i64, ptr %"%\D9\81", align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 %"%\D9\81.load24"
  store i64 0, ptr %arr.elem26, align 8
  %"%\D9\81.load27" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load27", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_264

while_cond_264:                                   ; preds = %while_body_265, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_265, label %while_exit_266

while_exit_266:                                   ; preds = %while_cond_264
  call void @"\D8\A3\D8\B9\D8\AF_\D8\A7\D9\84\D8\B1\D8\B3\D9\85"()
  ret i64 0
}

define void @"gui_\D8\AD\D9\84\D9\82\D8\A9"() {
entry:
  %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82" = alloca i64, align 8
  %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86" = alloca i64, align 8
  br i1 true, label %then_267, label %merge_268

merge_268:                                        ; preds = %entry
  store i64 0, ptr %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86", align 8
  store i64 0, ptr %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82", align 8
  %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86.load" = load i64, ptr %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86.load", 1
  br i1 %cmpeqtmp, label %then_269, label %merge_270

merge_270:                                        ; preds = %merge_272, %merge_268
  %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86.load2" = load i64, ptr %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86.load2", 1
  br i1 %cmpeqtmp3, label %then_273, label %merge_274

merge_272:                                        ; preds = %then_271, %then_269
  br label %merge_270

merge_274:                                        ; preds = %then_273, %merge_270
  %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86.load4" = load i64, ptr %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\A7\D9\84\D8\A2\D9\86.load4", 0
  br i1 %cmpeqtmp5, label %then_275, label %merge_276

merge_276:                                        ; preds = %merge_278, %merge_274
  br i1 false, label %then_279, label %merge_280

merge_278:                                        ; preds = %then_277, %then_275
  br label %merge_276

merge_280:                                        ; preds = %then_279, %merge_276
  ret void

then_267:                                         ; preds = %entry
  ret void

then_269:                                         ; preds = %merge_268
  %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82.load" = load i64, ptr %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82", align 8
  %cmpeqtmp1 = icmp eq i64 %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82.load", 0
  br i1 %cmpeqtmp1, label %then_271, label %merge_272

then_271:                                         ; preds = %then_269
  br label %merge_272

then_273:                                         ; preds = %merge_270
  call void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\B3\D8\AD\D8\A8"()
  call void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\AA\D8\BA\D9\8A\D9\8A\D8\B1_\D8\AD\D8\AC\D9\85"()
  br label %merge_274

then_275:                                         ; preds = %merge_274
  %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82.load6" = load i64, ptr %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\8A\D8\B3\D8\A7\D8\B1_\D8\B3\D8\A7\D8\A8\D9\82.load6", 1
  br i1 %cmpeqtmp7, label %then_277, label %merge_278

then_277:                                         ; preds = %then_275
  call void @"\D8\B9\D8\A7\D9\84\D8\AC_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1"()
  br label %merge_278

then_279:                                         ; preds = %merge_276
  call void @"\D8\A3\D8\B9\D8\AF_\D8\A7\D9\84\D8\B1\D8\B3\D9\85"()
  br label %merge_280
}

define i64 @"gui_\D8\A7\D9\81\D8\AA\D8\AD_\D9\85\D8\AA\D8\B5\D9\81\D8\AD"() {
entry:
  ret i64 0
}

define i64 @"gui_\D8\A7\D9\81\D8\AA\D8\AD_\D9\85\D8\AD\D8\B1\D8\B1"() {
entry:
  ret i64 0
}

define i64 @"gui_\D8\A7\D9\81\D8\AA\D8\AD_\D8\B7\D8\B1\D9\81\D9\8A\D8\A9"() {
entry:
  ret i64 0
}

define i64 @"gui_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D9\81\D8\B0"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_281

merge_285:                                        ; preds = %then_284, %while_body_282
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %addtmp3 = add i64 %"%\D9\81.load2", 1
  store i64 %addtmp3, ptr %"%\D9\81", align 8
  br label %while_cond_281

then_284:                                         ; preds = %while_body_282
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF.load", 1
  store i64 %addtmp, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  br label %merge_285

while_body_282:                                   ; preds = %while_cond_281
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_284, label %merge_285

while_cond_281:                                   ; preds = %merge_285, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_282, label %while_exit_283

while_exit_283:                                   ; preds = %while_cond_281
  %loadtmp = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  ret i64 %loadtmp
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\A7\D9\84\D9\88\D8\A7\D8\AC\D9\87\D8\A9"() {
entry:
  %"%n2" = alloca i64, align 8
  %"%n1" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%\D9\86\D8\AC\D8\A7\D8\AD" = alloca i64, align 8
  store i64 0, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_286

merge_290:                                        ; preds = %then_289, %while_exit_288
  %"%n1.load4" = load i64, ptr %"%n1", align 8
  call void @"gui_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%n1.load4")
  %"%n2.load" = load i64, ptr %"%n2", align 8
  %cmpgetmp5 = icmp sge i64 %"%n2.load", 0
  br i1 %cmpgetmp5, label %then_291, label %merge_292

merge_292:                                        ; preds = %then_291, %merge_290
  %calltmp = call i64 @"gui_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D9\81\D8\B0"()
  %cmpeqtmp = icmp eq i64 %calltmp, 2
  br i1 %cmpeqtmp, label %then_293, label %merge_294

merge_294:                                        ; preds = %then_293, %merge_292
  %"%n1.load10" = load i64, ptr %"%n1", align 8
  call void @"gui_\D9\83\D8\A8\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%n1.load10")
  %"%n1.load11" = load i64, ptr %"%n1", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%n1.load11"
  %arr.get = load i64, ptr %arr.elem13, align 8
  %cmpeqtmp14 = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp14, label %then_295, label %merge_296

merge_296:                                        ; preds = %then_295, %merge_294
  %"%n1.load17" = load i64, ptr %"%n1", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%n1.load17"
  %arr.get20 = load i64, ptr %arr.elem19, align 8
  %cmpeqtmp21 = icmp eq i64 %arr.get20, 0
  br i1 %cmpeqtmp21, label %then_297, label %merge_298

merge_298:                                        ; preds = %then_297, %merge_296
  %"%n1.load24" = load i64, ptr %"%n1", align 8
  call void @"gui_\D9\83\D8\A8\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%n1.load24")
  %"%n1.load25" = load i64, ptr %"%n1", align 8
  %arr.data26 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem27 = getelementptr i64, ptr %arr.data26, i64 %"%n1.load25"
  %arr.get28 = load i64, ptr %arr.elem27, align 8
  %cmpeqtmp29 = icmp eq i64 %arr.get28, 0
  br i1 %cmpeqtmp29, label %then_299, label %merge_300

merge_300:                                        ; preds = %then_299, %merge_298
  %"%n1.load32" = load i64, ptr %"%n1", align 8
  %arr.data33 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem34 = getelementptr i64, ptr %arr.data33, i64 %"%n1.load32"
  %arr.get35 = load i64, ptr %arr.elem34, align 8
  %cmpeqtmp36 = icmp eq i64 %arr.get35, 200
  br i1 %cmpeqtmp36, label %then_301, label %merge_302

merge_302:                                        ; preds = %then_301, %merge_300
  %"%n2.load39" = load i64, ptr %"%n2", align 8
  call void @"gui_\D8\B5\D8\BA\D8\B1_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%n2.load39")
  %"%n2.load40" = load i64, ptr %"%n2", align 8
  %arr.data41 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem42 = getelementptr i64, ptr %arr.data41, i64 %"%n2.load40"
  %arr.get43 = load i64, ptr %arr.elem42, align 8
  %cmpeqtmp44 = icmp eq i64 %arr.get43, 0
  br i1 %cmpeqtmp44, label %then_303, label %merge_304

merge_304:                                        ; preds = %then_303, %merge_302
  %"%n1.load47" = load i64, ptr %"%n1", align 8
  call void @"gui_\D8\A3\D8\BA\D9\84\D9\82_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%n1.load47")
  %"%n1.load48" = load i64, ptr %"%n1", align 8
  %arr.data49 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem50 = getelementptr i64, ptr %arr.data49, i64 %"%n1.load48"
  %arr.get51 = load i64, ptr %arr.elem50, align 8
  %cmpeqtmp52 = icmp eq i64 %arr.get51, 0
  br i1 %cmpeqtmp52, label %then_305, label %merge_306

merge_306:                                        ; preds = %then_305, %merge_304
  %calltmp55 = call i64 @"gui_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D9\81\D8\B0"()
  %cmpeqtmp56 = icmp eq i64 %calltmp55, 1
  br i1 %cmpeqtmp56, label %then_307, label %merge_308

merge_308:                                        ; preds = %then_307, %merge_306
  %"%n2.load59" = load i64, ptr %"%n2", align 8
  call void @"gui_\D8\A3\D8\BA\D9\84\D9\82_\D9\86\D8\A7\D9\81\D8\B0\D8\A9"(i64 %"%n2.load59")
  br i1 false, label %then_309, label %merge_310

merge_310:                                        ; preds = %then_309, %merge_308
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load62" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  call void @"gui_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load62")
  call void @"gui_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 11)
  ret void

then_289:                                         ; preds = %while_exit_288
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp3 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load", 1
  store i64 %addtmp3, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_290

then_291:                                         ; preds = %merge_290
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load6" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp7 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load6", 1
  store i64 %addtmp7, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_292

then_293:                                         ; preds = %merge_292
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load8" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp9 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load8", 1
  store i64 %addtmp9, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_294

then_295:                                         ; preds = %merge_294
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load15" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp16 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load15", 1
  store i64 %addtmp16, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_296

then_297:                                         ; preds = %merge_296
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load22" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp23 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load22", 1
  store i64 %addtmp23, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_298

then_299:                                         ; preds = %merge_298
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load30" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp31 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load30", 1
  store i64 %addtmp31, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_300

then_301:                                         ; preds = %merge_300
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load37" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp38 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load37", 1
  store i64 %addtmp38, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_302

then_303:                                         ; preds = %merge_302
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load45" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp46 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load45", 1
  store i64 %addtmp46, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_304

then_305:                                         ; preds = %merge_304
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load53" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp54 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load53", 1
  store i64 %addtmp54, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_306

then_307:                                         ; preds = %merge_306
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load57" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp58 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load57", 1
  store i64 %addtmp58, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_308

then_309:                                         ; preds = %merge_308
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load60" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp61 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load60", 1
  store i64 %addtmp61, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_310

while_body_287:                                   ; preds = %while_cond_286
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  store i64 0, ptr %arr.elem, align 8
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load2", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_286

while_cond_286:                                   ; preds = %while_body_287, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_287, label %while_exit_288

while_exit_288:                                   ; preds = %while_cond_286
  %"%n1.load" = load i64, ptr %"%n1", align 8
  %cmpgetmp = icmp sge i64 %"%n1.load", 0
  br i1 %cmpgetmp, label %then_289, label %merge_290
}

define void @__sad_main() {
entry:
  %"%font_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%TB_START_W" = alloca i64, align 8
  %"%TB_START_X" = alloca i64, align 8
  %"%TB_BTN_Y" = alloca i64, align 8
  %"%TB_BTN_H" = alloca i64, align 8
  %"%TB_BTN_W" = alloca i64, align 8
  %"%needs_redraw" = alloca i64, align 8
  %"%gui_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%next_z" = alloca i64, align 8
  %"%active_win" = alloca i64, align 8
  %"%win_scroll_y" = alloca i64, align 8
  %"%win_normal_h" = alloca i64, align 8
  %"%win_normal_w" = alloca i64, align 8
  %"%win_normal_y" = alloca i64, align 8
  %"%win_normal_x" = alloca i64, align 8
  %"%win_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%win_z" = alloca i64, align 8
  %"%win_h" = alloca i64, align 8
  %"%win_w" = alloca i64, align 8
  %"%win_y" = alloca i64, align 8
  %"%win_x" = alloca i64, align 8
  %"%win_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%win_\D8\AD\D8\A7\D9\84\D8\A9" = alloca i64, align 8
  %"%resize_orig_h" = alloca i64, align 8
  %"%resize_orig_w" = alloca i64, align 8
  %"%resize_start_y" = alloca i64, align 8
  %"%resize_start_x" = alloca i64, align 8
  %"%resize_win" = alloca i64, align 8
  %"%resize_active" = alloca i64, align 8
  %"%drag_offset_y" = alloca i64, align 8
  %"%drag_offset_x" = alloca i64, align 8
  %"%drag_win" = alloca i64, align 8
  %"%drag_active" = alloca i64, align 8
  %"%mouse_\D9\85\D9\87\D9\8A\D8\A3" = alloca i64, align 8
  %"%mouse_byte2" = alloca i64, align 8
  %"%mouse_byte1" = alloca i64, align 8
  %"%mouse_byte0" = alloca i64, align 8
  %"%mouse_byte_count" = alloca i64, align 8
  %"%mouse_prev_btns" = alloca i64, align 8
  %"%mouse_btns" = alloca i64, align 8
  %"%mouse_y" = alloca i64, align 8
  %"%mouse_x" = alloca i64, align 8
  %"%WIN_TYPE_TERMINAL" = alloca i64, align 8
  %"%WIN_TYPE_EDITOR" = alloca i64, align 8
  %"%WIN_TYPE_FILEBROWSER" = alloca i64, align 8
  %"%WIN_TYPE_GENERIC" = alloca i64, align 8
  %"%WIN_MINIMIZED" = alloca i64, align 8
  %"%WIN_MAXIMIZED" = alloca i64, align 8
  %"%WIN_NORMAL" = alloca i64, align 8
  %"%WIN_FREE" = alloca i64, align 8
  %"%MOUSE_CMD" = alloca i64, align 8
  %"%MOUSE_DATA" = alloca i64, align 8
  %"%MIN_WIN_H" = alloca i64, align 8
  %"%MIN_WIN_W" = alloca i64, align 8
  %"%BTN_SIZE" = alloca i64, align 8
  %"%BORDER_W" = alloca i64, align 8
  %"%TITLEBAR_H" = alloca i64, align 8
  %"%MAX_WINDOWS" = alloca i64, align 8
  %"%TASKBAR_Y" = alloca i64, align 8
  %"%TASKBAR_H" = alloca i64, align 8
  %"%COL_ICON_FILE" = alloca i64, align 8
  %"%COL_ICON_FOLDER" = alloca i64, align 8
  %"%COL_SCROLLBAR" = alloca i64, align 8
  %"%COL_HIGHLIGHT" = alloca i64, align 8
  %"%COL_CURSOR" = alloca i64, align 8
  %"%COL_BLACK" = alloca i64, align 8
  %"%COL_WHITE" = alloca i64, align 8
  %"%COL_TEXT" = alloca i64, align 8
  %"%COL_BTN_MIN" = alloca i64, align 8
  %"%COL_BTN_MAX" = alloca i64, align 8
  %"%COL_BTN_CLOSE" = alloca i64, align 8
  %"%COL_BORDER" = alloca i64, align 8
  %"%COL_TITLEBAR_INACTIVE" = alloca i64, align 8
  %"%COL_TITLEBAR" = alloca i64, align 8
  %"%COL_WIN_BG" = alloca i64, align 8
  %"%COL_TASKBAR" = alloca i64, align 8
  %"%COL_BG" = alloca i64, align 8
  %"%BPP" = alloca i64, align 8
  %"%SCREEN_H" = alloca i64, align 8
  %"%SCREEN_W" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 800, ptr %"%SCREEN_W", align 8
  store i64 600, ptr %"%SCREEN_H", align 8
  store i64 32, ptr %"%BPP", align 8
  store i64 3368601, ptr %"%COL_BG", align 8
  store i64 2236962, ptr %"%COL_TASKBAR", align 8
  store i64 15790320, ptr %"%COL_WIN_BG", align 8
  store i64 3381708, ptr %"%COL_TITLEBAR", align 8
  store i64 8421504, ptr %"%COL_TITLEBAR_INACTIVE", align 8
  store i64 6710886, ptr %"%COL_BORDER", align 8
  store i64 16711680, ptr %"%COL_BTN_CLOSE", align 8
  store i64 65280, ptr %"%COL_BTN_MAX", align 8
  store i64 16776960, ptr %"%COL_BTN_MIN", align 8
  store i64 1118481, ptr %"%COL_TEXT", align 8
  store i64 16777215, ptr %"%COL_WHITE", align 8
  store i64 0, ptr %"%COL_BLACK", align 8
  store i64 16777215, ptr %"%COL_CURSOR", align 8
  store i64 4474111, ptr %"%COL_HIGHLIGHT", align 8
  store i64 12632256, ptr %"%COL_SCROLLBAR", align 8
  store i64 16766720, ptr %"%COL_ICON_FOLDER", align 8
  store i64 11184810, ptr %"%COL_ICON_FILE", align 8
  store i64 30, ptr %"%TASKBAR_H", align 8
  store i64 570, ptr %"%TASKBAR_Y", align 8
  store i64 16, ptr %"%MAX_WINDOWS", align 8
  store i64 24, ptr %"%TITLEBAR_H", align 8
  store i64 2, ptr %"%BORDER_W", align 8
  store i64 16, ptr %"%BTN_SIZE", align 8
  store i64 100, ptr %"%MIN_WIN_W", align 8
  store i64 80, ptr %"%MIN_WIN_H", align 8
  store i64 96, ptr %"%MOUSE_DATA", align 8
  store i64 100, ptr %"%MOUSE_CMD", align 8
  store i64 0, ptr %"%WIN_FREE", align 8
  store i64 1, ptr %"%WIN_NORMAL", align 8
  store i64 2, ptr %"%WIN_MAXIMIZED", align 8
  store i64 3, ptr %"%WIN_MINIMIZED", align 8
  store i64 0, ptr %"%WIN_TYPE_GENERIC", align 8
  store i64 1, ptr %"%WIN_TYPE_FILEBROWSER", align 8
  store i64 2, ptr %"%WIN_TYPE_EDITOR", align 8
  store i64 3, ptr %"%WIN_TYPE_TERMINAL", align 8
  store i64 400, ptr %"%mouse_x", align 8
  store i64 300, ptr %"%mouse_y", align 8
  store i64 0, ptr %"%mouse_btns", align 8
  store i64 0, ptr %"%mouse_prev_btns", align 8
  store i64 0, ptr %"%mouse_byte_count", align 8
  store i64 0, ptr %"%mouse_byte0", align 8
  store i64 0, ptr %"%mouse_byte1", align 8
  store i64 0, ptr %"%mouse_byte2", align 8
  store i64 0, ptr %"%mouse_\D9\85\D9\87\D9\8A\D8\A3", align 8
  store i64 0, ptr %"%drag_active", align 8
  store i64 -1, ptr %"%drag_win", align 8
  store i64 0, ptr %"%drag_offset_x", align 8
  store i64 0, ptr %"%drag_offset_y", align 8
  store i64 0, ptr %"%resize_active", align 8
  store i64 -1, ptr %"%resize_win", align 8
  store i64 0, ptr %"%resize_start_x", align 8
  store i64 0, ptr %"%resize_start_y", align 8
  store i64 0, ptr %"%resize_orig_w", align 8
  store i64 0, ptr %"%resize_orig_h", align 8
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
  store i64 %obj.ptrtoint, ptr %"%win_\D8\AD\D8\A7\D9\84\D8\A9", align 8
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
  store i64 %obj.ptrtoint101, ptr %"%win_\D9\86\D9\88\D8\B9", align 8
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
  store i64 %obj.ptrtoint155, ptr %"%win_x", align 8
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
  store i64 %obj.ptrtoint209, ptr %"%win_y", align 8
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
  store i64 %obj.ptrtoint263, ptr %"%win_w", align 8
  %arr_new264 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep265 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 0
  store i64 16, ptr %arr.len.gep265, align 8
  %arr.cap.gep266 = getelementptr inbounds %SadArray, ptr %arr_new264, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep266, align 8
  %arr.data267 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
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
  %obj.ptrtoint317 = ptrtoint ptr %arr_new264 to i64
  store i64 %obj.ptrtoint317, ptr %"%win_h", align 8
  %arr_new318 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep319 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 0
  store i64 16, ptr %arr.len.gep319, align 8
  %arr.cap.gep320 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep320, align 8
  %arr.data321 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep322 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  store ptr %arr.data321, ptr %arr.data.gep322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 0
  store i64 0, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 1
  store i64 0, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 2
  store i64 0, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 3
  store i64 0, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 4
  store i64 0, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 5
  store i64 0, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 6
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 7
  store i64 0, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 8
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 9
  store i64 0, ptr %arr.elem352, align 8
  %arr.data.gep353 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data354 = load ptr, ptr %arr.data.gep353, align 8
  %arr.elem355 = getelementptr i64, ptr %arr.data354, i64 10
  store i64 0, ptr %arr.elem355, align 8
  %arr.data.gep356 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data357 = load ptr, ptr %arr.data.gep356, align 8
  %arr.elem358 = getelementptr i64, ptr %arr.data357, i64 11
  store i64 0, ptr %arr.elem358, align 8
  %arr.data.gep359 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data360 = load ptr, ptr %arr.data.gep359, align 8
  %arr.elem361 = getelementptr i64, ptr %arr.data360, i64 12
  store i64 0, ptr %arr.elem361, align 8
  %arr.data.gep362 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data363 = load ptr, ptr %arr.data.gep362, align 8
  %arr.elem364 = getelementptr i64, ptr %arr.data363, i64 13
  store i64 0, ptr %arr.elem364, align 8
  %arr.data.gep365 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data366 = load ptr, ptr %arr.data.gep365, align 8
  %arr.elem367 = getelementptr i64, ptr %arr.data366, i64 14
  store i64 0, ptr %arr.elem367, align 8
  %arr.data.gep368 = getelementptr inbounds %SadArray, ptr %arr_new318, i32 0, i32 2
  %arr.data369 = load ptr, ptr %arr.data.gep368, align 8
  %arr.elem370 = getelementptr i64, ptr %arr.data369, i64 15
  store i64 0, ptr %arr.elem370, align 8
  %obj.ptrtoint371 = ptrtoint ptr %arr_new318 to i64
  store i64 %obj.ptrtoint371, ptr %"%win_z", align 8
  %arr_new372 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep373 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 0
  store i64 16, ptr %arr.len.gep373, align 8
  %arr.cap.gep374 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep374, align 8
  %arr.data375 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep376 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  store ptr %arr.data375, ptr %arr.data.gep376, align 8
  %arr.data.gep377 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data378 = load ptr, ptr %arr.data.gep377, align 8
  %arr.elem379 = getelementptr i64, ptr %arr.data378, i64 0
  store i64 0, ptr %arr.elem379, align 8
  %arr.data.gep380 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data381 = load ptr, ptr %arr.data.gep380, align 8
  %arr.elem382 = getelementptr i64, ptr %arr.data381, i64 1
  store i64 0, ptr %arr.elem382, align 8
  %arr.data.gep383 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data384 = load ptr, ptr %arr.data.gep383, align 8
  %arr.elem385 = getelementptr i64, ptr %arr.data384, i64 2
  store i64 0, ptr %arr.elem385, align 8
  %arr.data.gep386 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data387 = load ptr, ptr %arr.data.gep386, align 8
  %arr.elem388 = getelementptr i64, ptr %arr.data387, i64 3
  store i64 0, ptr %arr.elem388, align 8
  %arr.data.gep389 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data390 = load ptr, ptr %arr.data.gep389, align 8
  %arr.elem391 = getelementptr i64, ptr %arr.data390, i64 4
  store i64 0, ptr %arr.elem391, align 8
  %arr.data.gep392 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data393 = load ptr, ptr %arr.data.gep392, align 8
  %arr.elem394 = getelementptr i64, ptr %arr.data393, i64 5
  store i64 0, ptr %arr.elem394, align 8
  %arr.data.gep395 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data396 = load ptr, ptr %arr.data.gep395, align 8
  %arr.elem397 = getelementptr i64, ptr %arr.data396, i64 6
  store i64 0, ptr %arr.elem397, align 8
  %arr.data.gep398 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data399 = load ptr, ptr %arr.data.gep398, align 8
  %arr.elem400 = getelementptr i64, ptr %arr.data399, i64 7
  store i64 0, ptr %arr.elem400, align 8
  %arr.data.gep401 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data402 = load ptr, ptr %arr.data.gep401, align 8
  %arr.elem403 = getelementptr i64, ptr %arr.data402, i64 8
  store i64 0, ptr %arr.elem403, align 8
  %arr.data.gep404 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data405 = load ptr, ptr %arr.data.gep404, align 8
  %arr.elem406 = getelementptr i64, ptr %arr.data405, i64 9
  store i64 0, ptr %arr.elem406, align 8
  %arr.data.gep407 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data408 = load ptr, ptr %arr.data.gep407, align 8
  %arr.elem409 = getelementptr i64, ptr %arr.data408, i64 10
  store i64 0, ptr %arr.elem409, align 8
  %arr.data.gep410 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data411 = load ptr, ptr %arr.data.gep410, align 8
  %arr.elem412 = getelementptr i64, ptr %arr.data411, i64 11
  store i64 0, ptr %arr.elem412, align 8
  %arr.data.gep413 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data414 = load ptr, ptr %arr.data.gep413, align 8
  %arr.elem415 = getelementptr i64, ptr %arr.data414, i64 12
  store i64 0, ptr %arr.elem415, align 8
  %arr.data.gep416 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data417 = load ptr, ptr %arr.data.gep416, align 8
  %arr.elem418 = getelementptr i64, ptr %arr.data417, i64 13
  store i64 0, ptr %arr.elem418, align 8
  %arr.data.gep419 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data420 = load ptr, ptr %arr.data.gep419, align 8
  %arr.elem421 = getelementptr i64, ptr %arr.data420, i64 14
  store i64 0, ptr %arr.elem421, align 8
  %arr.data.gep422 = getelementptr inbounds %SadArray, ptr %arr_new372, i32 0, i32 2
  %arr.data423 = load ptr, ptr %arr.data.gep422, align 8
  %arr.elem424 = getelementptr i64, ptr %arr.data423, i64 15
  store i64 0, ptr %arr.elem424, align 8
  %obj.ptrtoint425 = ptrtoint ptr %arr_new372 to i64
  store i64 %obj.ptrtoint425, ptr %"%win_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %arr_new426 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep427 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 0
  store i64 16, ptr %arr.len.gep427, align 8
  %arr.cap.gep428 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep428, align 8
  %arr.data429 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep430 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  store ptr %arr.data429, ptr %arr.data.gep430, align 8
  %arr.data.gep431 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data432 = load ptr, ptr %arr.data.gep431, align 8
  %arr.elem433 = getelementptr i64, ptr %arr.data432, i64 0
  store i64 0, ptr %arr.elem433, align 8
  %arr.data.gep434 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data435 = load ptr, ptr %arr.data.gep434, align 8
  %arr.elem436 = getelementptr i64, ptr %arr.data435, i64 1
  store i64 0, ptr %arr.elem436, align 8
  %arr.data.gep437 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data438 = load ptr, ptr %arr.data.gep437, align 8
  %arr.elem439 = getelementptr i64, ptr %arr.data438, i64 2
  store i64 0, ptr %arr.elem439, align 8
  %arr.data.gep440 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data441 = load ptr, ptr %arr.data.gep440, align 8
  %arr.elem442 = getelementptr i64, ptr %arr.data441, i64 3
  store i64 0, ptr %arr.elem442, align 8
  %arr.data.gep443 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data444 = load ptr, ptr %arr.data.gep443, align 8
  %arr.elem445 = getelementptr i64, ptr %arr.data444, i64 4
  store i64 0, ptr %arr.elem445, align 8
  %arr.data.gep446 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data447 = load ptr, ptr %arr.data.gep446, align 8
  %arr.elem448 = getelementptr i64, ptr %arr.data447, i64 5
  store i64 0, ptr %arr.elem448, align 8
  %arr.data.gep449 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data450 = load ptr, ptr %arr.data.gep449, align 8
  %arr.elem451 = getelementptr i64, ptr %arr.data450, i64 6
  store i64 0, ptr %arr.elem451, align 8
  %arr.data.gep452 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data453 = load ptr, ptr %arr.data.gep452, align 8
  %arr.elem454 = getelementptr i64, ptr %arr.data453, i64 7
  store i64 0, ptr %arr.elem454, align 8
  %arr.data.gep455 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data456 = load ptr, ptr %arr.data.gep455, align 8
  %arr.elem457 = getelementptr i64, ptr %arr.data456, i64 8
  store i64 0, ptr %arr.elem457, align 8
  %arr.data.gep458 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data459 = load ptr, ptr %arr.data.gep458, align 8
  %arr.elem460 = getelementptr i64, ptr %arr.data459, i64 9
  store i64 0, ptr %arr.elem460, align 8
  %arr.data.gep461 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data462 = load ptr, ptr %arr.data.gep461, align 8
  %arr.elem463 = getelementptr i64, ptr %arr.data462, i64 10
  store i64 0, ptr %arr.elem463, align 8
  %arr.data.gep464 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data465 = load ptr, ptr %arr.data.gep464, align 8
  %arr.elem466 = getelementptr i64, ptr %arr.data465, i64 11
  store i64 0, ptr %arr.elem466, align 8
  %arr.data.gep467 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data468 = load ptr, ptr %arr.data.gep467, align 8
  %arr.elem469 = getelementptr i64, ptr %arr.data468, i64 12
  store i64 0, ptr %arr.elem469, align 8
  %arr.data.gep470 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data471 = load ptr, ptr %arr.data.gep470, align 8
  %arr.elem472 = getelementptr i64, ptr %arr.data471, i64 13
  store i64 0, ptr %arr.elem472, align 8
  %arr.data.gep473 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data474 = load ptr, ptr %arr.data.gep473, align 8
  %arr.elem475 = getelementptr i64, ptr %arr.data474, i64 14
  store i64 0, ptr %arr.elem475, align 8
  %arr.data.gep476 = getelementptr inbounds %SadArray, ptr %arr_new426, i32 0, i32 2
  %arr.data477 = load ptr, ptr %arr.data.gep476, align 8
  %arr.elem478 = getelementptr i64, ptr %arr.data477, i64 15
  store i64 0, ptr %arr.elem478, align 8
  %obj.ptrtoint479 = ptrtoint ptr %arr_new426 to i64
  store i64 %obj.ptrtoint479, ptr %"%win_normal_x", align 8
  %arr_new480 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep481 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 0
  store i64 16, ptr %arr.len.gep481, align 8
  %arr.cap.gep482 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep482, align 8
  %arr.data483 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep484 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  store ptr %arr.data483, ptr %arr.data.gep484, align 8
  %arr.data.gep485 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data486 = load ptr, ptr %arr.data.gep485, align 8
  %arr.elem487 = getelementptr i64, ptr %arr.data486, i64 0
  store i64 0, ptr %arr.elem487, align 8
  %arr.data.gep488 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data489 = load ptr, ptr %arr.data.gep488, align 8
  %arr.elem490 = getelementptr i64, ptr %arr.data489, i64 1
  store i64 0, ptr %arr.elem490, align 8
  %arr.data.gep491 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data492 = load ptr, ptr %arr.data.gep491, align 8
  %arr.elem493 = getelementptr i64, ptr %arr.data492, i64 2
  store i64 0, ptr %arr.elem493, align 8
  %arr.data.gep494 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data495 = load ptr, ptr %arr.data.gep494, align 8
  %arr.elem496 = getelementptr i64, ptr %arr.data495, i64 3
  store i64 0, ptr %arr.elem496, align 8
  %arr.data.gep497 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data498 = load ptr, ptr %arr.data.gep497, align 8
  %arr.elem499 = getelementptr i64, ptr %arr.data498, i64 4
  store i64 0, ptr %arr.elem499, align 8
  %arr.data.gep500 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data501 = load ptr, ptr %arr.data.gep500, align 8
  %arr.elem502 = getelementptr i64, ptr %arr.data501, i64 5
  store i64 0, ptr %arr.elem502, align 8
  %arr.data.gep503 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data504 = load ptr, ptr %arr.data.gep503, align 8
  %arr.elem505 = getelementptr i64, ptr %arr.data504, i64 6
  store i64 0, ptr %arr.elem505, align 8
  %arr.data.gep506 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data507 = load ptr, ptr %arr.data.gep506, align 8
  %arr.elem508 = getelementptr i64, ptr %arr.data507, i64 7
  store i64 0, ptr %arr.elem508, align 8
  %arr.data.gep509 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data510 = load ptr, ptr %arr.data.gep509, align 8
  %arr.elem511 = getelementptr i64, ptr %arr.data510, i64 8
  store i64 0, ptr %arr.elem511, align 8
  %arr.data.gep512 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data513 = load ptr, ptr %arr.data.gep512, align 8
  %arr.elem514 = getelementptr i64, ptr %arr.data513, i64 9
  store i64 0, ptr %arr.elem514, align 8
  %arr.data.gep515 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data516 = load ptr, ptr %arr.data.gep515, align 8
  %arr.elem517 = getelementptr i64, ptr %arr.data516, i64 10
  store i64 0, ptr %arr.elem517, align 8
  %arr.data.gep518 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data519 = load ptr, ptr %arr.data.gep518, align 8
  %arr.elem520 = getelementptr i64, ptr %arr.data519, i64 11
  store i64 0, ptr %arr.elem520, align 8
  %arr.data.gep521 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data522 = load ptr, ptr %arr.data.gep521, align 8
  %arr.elem523 = getelementptr i64, ptr %arr.data522, i64 12
  store i64 0, ptr %arr.elem523, align 8
  %arr.data.gep524 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data525 = load ptr, ptr %arr.data.gep524, align 8
  %arr.elem526 = getelementptr i64, ptr %arr.data525, i64 13
  store i64 0, ptr %arr.elem526, align 8
  %arr.data.gep527 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data528 = load ptr, ptr %arr.data.gep527, align 8
  %arr.elem529 = getelementptr i64, ptr %arr.data528, i64 14
  store i64 0, ptr %arr.elem529, align 8
  %arr.data.gep530 = getelementptr inbounds %SadArray, ptr %arr_new480, i32 0, i32 2
  %arr.data531 = load ptr, ptr %arr.data.gep530, align 8
  %arr.elem532 = getelementptr i64, ptr %arr.data531, i64 15
  store i64 0, ptr %arr.elem532, align 8
  %obj.ptrtoint533 = ptrtoint ptr %arr_new480 to i64
  store i64 %obj.ptrtoint533, ptr %"%win_normal_y", align 8
  %arr_new534 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep535 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 0
  store i64 16, ptr %arr.len.gep535, align 8
  %arr.cap.gep536 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep536, align 8
  %arr.data537 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep538 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  store ptr %arr.data537, ptr %arr.data.gep538, align 8
  %arr.data.gep539 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data540 = load ptr, ptr %arr.data.gep539, align 8
  %arr.elem541 = getelementptr i64, ptr %arr.data540, i64 0
  store i64 0, ptr %arr.elem541, align 8
  %arr.data.gep542 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data543 = load ptr, ptr %arr.data.gep542, align 8
  %arr.elem544 = getelementptr i64, ptr %arr.data543, i64 1
  store i64 0, ptr %arr.elem544, align 8
  %arr.data.gep545 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data546 = load ptr, ptr %arr.data.gep545, align 8
  %arr.elem547 = getelementptr i64, ptr %arr.data546, i64 2
  store i64 0, ptr %arr.elem547, align 8
  %arr.data.gep548 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data549 = load ptr, ptr %arr.data.gep548, align 8
  %arr.elem550 = getelementptr i64, ptr %arr.data549, i64 3
  store i64 0, ptr %arr.elem550, align 8
  %arr.data.gep551 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data552 = load ptr, ptr %arr.data.gep551, align 8
  %arr.elem553 = getelementptr i64, ptr %arr.data552, i64 4
  store i64 0, ptr %arr.elem553, align 8
  %arr.data.gep554 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data555 = load ptr, ptr %arr.data.gep554, align 8
  %arr.elem556 = getelementptr i64, ptr %arr.data555, i64 5
  store i64 0, ptr %arr.elem556, align 8
  %arr.data.gep557 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data558 = load ptr, ptr %arr.data.gep557, align 8
  %arr.elem559 = getelementptr i64, ptr %arr.data558, i64 6
  store i64 0, ptr %arr.elem559, align 8
  %arr.data.gep560 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data561 = load ptr, ptr %arr.data.gep560, align 8
  %arr.elem562 = getelementptr i64, ptr %arr.data561, i64 7
  store i64 0, ptr %arr.elem562, align 8
  %arr.data.gep563 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data564 = load ptr, ptr %arr.data.gep563, align 8
  %arr.elem565 = getelementptr i64, ptr %arr.data564, i64 8
  store i64 0, ptr %arr.elem565, align 8
  %arr.data.gep566 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data567 = load ptr, ptr %arr.data.gep566, align 8
  %arr.elem568 = getelementptr i64, ptr %arr.data567, i64 9
  store i64 0, ptr %arr.elem568, align 8
  %arr.data.gep569 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data570 = load ptr, ptr %arr.data.gep569, align 8
  %arr.elem571 = getelementptr i64, ptr %arr.data570, i64 10
  store i64 0, ptr %arr.elem571, align 8
  %arr.data.gep572 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data573 = load ptr, ptr %arr.data.gep572, align 8
  %arr.elem574 = getelementptr i64, ptr %arr.data573, i64 11
  store i64 0, ptr %arr.elem574, align 8
  %arr.data.gep575 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data576 = load ptr, ptr %arr.data.gep575, align 8
  %arr.elem577 = getelementptr i64, ptr %arr.data576, i64 12
  store i64 0, ptr %arr.elem577, align 8
  %arr.data.gep578 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data579 = load ptr, ptr %arr.data.gep578, align 8
  %arr.elem580 = getelementptr i64, ptr %arr.data579, i64 13
  store i64 0, ptr %arr.elem580, align 8
  %arr.data.gep581 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data582 = load ptr, ptr %arr.data.gep581, align 8
  %arr.elem583 = getelementptr i64, ptr %arr.data582, i64 14
  store i64 0, ptr %arr.elem583, align 8
  %arr.data.gep584 = getelementptr inbounds %SadArray, ptr %arr_new534, i32 0, i32 2
  %arr.data585 = load ptr, ptr %arr.data.gep584, align 8
  %arr.elem586 = getelementptr i64, ptr %arr.data585, i64 15
  store i64 0, ptr %arr.elem586, align 8
  %obj.ptrtoint587 = ptrtoint ptr %arr_new534 to i64
  store i64 %obj.ptrtoint587, ptr %"%win_normal_w", align 8
  %arr_new588 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep589 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 0
  store i64 16, ptr %arr.len.gep589, align 8
  %arr.cap.gep590 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep590, align 8
  %arr.data591 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
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
  %arr.data.gep623 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data624 = load ptr, ptr %arr.data.gep623, align 8
  %arr.elem625 = getelementptr i64, ptr %arr.data624, i64 10
  store i64 0, ptr %arr.elem625, align 8
  %arr.data.gep626 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data627 = load ptr, ptr %arr.data.gep626, align 8
  %arr.elem628 = getelementptr i64, ptr %arr.data627, i64 11
  store i64 0, ptr %arr.elem628, align 8
  %arr.data.gep629 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data630 = load ptr, ptr %arr.data.gep629, align 8
  %arr.elem631 = getelementptr i64, ptr %arr.data630, i64 12
  store i64 0, ptr %arr.elem631, align 8
  %arr.data.gep632 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data633 = load ptr, ptr %arr.data.gep632, align 8
  %arr.elem634 = getelementptr i64, ptr %arr.data633, i64 13
  store i64 0, ptr %arr.elem634, align 8
  %arr.data.gep635 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data636 = load ptr, ptr %arr.data.gep635, align 8
  %arr.elem637 = getelementptr i64, ptr %arr.data636, i64 14
  store i64 0, ptr %arr.elem637, align 8
  %arr.data.gep638 = getelementptr inbounds %SadArray, ptr %arr_new588, i32 0, i32 2
  %arr.data639 = load ptr, ptr %arr.data.gep638, align 8
  %arr.elem640 = getelementptr i64, ptr %arr.data639, i64 15
  store i64 0, ptr %arr.elem640, align 8
  %obj.ptrtoint641 = ptrtoint ptr %arr_new588 to i64
  store i64 %obj.ptrtoint641, ptr %"%win_normal_h", align 8
  %arr_new642 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep643 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 0
  store i64 16, ptr %arr.len.gep643, align 8
  %arr.cap.gep644 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep644, align 8
  %arr.data645 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep646 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  store ptr %arr.data645, ptr %arr.data.gep646, align 8
  %arr.data.gep647 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data648 = load ptr, ptr %arr.data.gep647, align 8
  %arr.elem649 = getelementptr i64, ptr %arr.data648, i64 0
  store i64 0, ptr %arr.elem649, align 8
  %arr.data.gep650 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data651 = load ptr, ptr %arr.data.gep650, align 8
  %arr.elem652 = getelementptr i64, ptr %arr.data651, i64 1
  store i64 0, ptr %arr.elem652, align 8
  %arr.data.gep653 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data654 = load ptr, ptr %arr.data.gep653, align 8
  %arr.elem655 = getelementptr i64, ptr %arr.data654, i64 2
  store i64 0, ptr %arr.elem655, align 8
  %arr.data.gep656 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data657 = load ptr, ptr %arr.data.gep656, align 8
  %arr.elem658 = getelementptr i64, ptr %arr.data657, i64 3
  store i64 0, ptr %arr.elem658, align 8
  %arr.data.gep659 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data660 = load ptr, ptr %arr.data.gep659, align 8
  %arr.elem661 = getelementptr i64, ptr %arr.data660, i64 4
  store i64 0, ptr %arr.elem661, align 8
  %arr.data.gep662 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data663 = load ptr, ptr %arr.data.gep662, align 8
  %arr.elem664 = getelementptr i64, ptr %arr.data663, i64 5
  store i64 0, ptr %arr.elem664, align 8
  %arr.data.gep665 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data666 = load ptr, ptr %arr.data.gep665, align 8
  %arr.elem667 = getelementptr i64, ptr %arr.data666, i64 6
  store i64 0, ptr %arr.elem667, align 8
  %arr.data.gep668 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data669 = load ptr, ptr %arr.data.gep668, align 8
  %arr.elem670 = getelementptr i64, ptr %arr.data669, i64 7
  store i64 0, ptr %arr.elem670, align 8
  %arr.data.gep671 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data672 = load ptr, ptr %arr.data.gep671, align 8
  %arr.elem673 = getelementptr i64, ptr %arr.data672, i64 8
  store i64 0, ptr %arr.elem673, align 8
  %arr.data.gep674 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data675 = load ptr, ptr %arr.data.gep674, align 8
  %arr.elem676 = getelementptr i64, ptr %arr.data675, i64 9
  store i64 0, ptr %arr.elem676, align 8
  %arr.data.gep677 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data678 = load ptr, ptr %arr.data.gep677, align 8
  %arr.elem679 = getelementptr i64, ptr %arr.data678, i64 10
  store i64 0, ptr %arr.elem679, align 8
  %arr.data.gep680 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data681 = load ptr, ptr %arr.data.gep680, align 8
  %arr.elem682 = getelementptr i64, ptr %arr.data681, i64 11
  store i64 0, ptr %arr.elem682, align 8
  %arr.data.gep683 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data684 = load ptr, ptr %arr.data.gep683, align 8
  %arr.elem685 = getelementptr i64, ptr %arr.data684, i64 12
  store i64 0, ptr %arr.elem685, align 8
  %arr.data.gep686 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data687 = load ptr, ptr %arr.data.gep686, align 8
  %arr.elem688 = getelementptr i64, ptr %arr.data687, i64 13
  store i64 0, ptr %arr.elem688, align 8
  %arr.data.gep689 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data690 = load ptr, ptr %arr.data.gep689, align 8
  %arr.elem691 = getelementptr i64, ptr %arr.data690, i64 14
  store i64 0, ptr %arr.elem691, align 8
  %arr.data.gep692 = getelementptr inbounds %SadArray, ptr %arr_new642, i32 0, i32 2
  %arr.data693 = load ptr, ptr %arr.data.gep692, align 8
  %arr.elem694 = getelementptr i64, ptr %arr.data693, i64 15
  store i64 0, ptr %arr.elem694, align 8
  %obj.ptrtoint695 = ptrtoint ptr %arr_new642 to i64
  store i64 %obj.ptrtoint695, ptr %"%win_scroll_y", align 8
  store i64 -1, ptr %"%active_win", align 8
  store i64 1, ptr %"%next_z", align 8
  store i64 0, ptr %"%gui_\D9\85\D9\87\D9\8A\D8\A3", align 8
  store i64 1, ptr %"%needs_redraw", align 8
  store i64 100, ptr %"%TB_BTN_W", align 8
  store i64 24, ptr %"%TB_BTN_H", align 8
  store i64 573, ptr %"%TB_BTN_Y", align 8
  store i64 4, ptr %"%TB_START_X", align 8
  store i64 50, ptr %"%TB_START_W", align 8
  store i64 0, ptr %"%font_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
