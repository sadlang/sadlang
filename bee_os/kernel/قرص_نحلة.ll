; ModuleID = 'قرص_نحلة'
source_filename = "\D9\82\D8\B1\D8\B5_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%SadArray = type { i64, i64, ptr }

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_port_inw(i64)

declare void @sad_ll_port_outw(i64, i64)

declare void @sad_ll_port_inl(i64)

declare void @sad_ll_port_outl(i64, i64)

declare void @sad_ll_mem_read64(i64)

declare void @sad_ll_mem_write64(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_mem_read16(i64)

declare void @sad_ll_mem_write16(i64, i64)

declare void @sad_ll_memset(i64, i64, i64)

declare void @sad_ll_memcpy(i64, i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_interrupt_disable()

declare void @sad_ll_interrupt_enable()

declare void @sad_ll_page_alloc()

declare void @sad_ll_page_free(i64)

declare void @sad_ll_pages_alloc(i64)

declare void @sad_ll_pages_free(i64, i64)

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
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
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_2:                                           ; preds = %merge_1
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_3
}

define void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load")
  ret void
}

define void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85") {
entry:
  %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\851" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\851", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\851", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load2" = load i64, ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\851", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load2")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load3" = load i64, ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\851", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load3")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load4" = load i64, ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\851", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load4")
  ret void
}

define i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 2
  br i1 %cmplttmp, label %then_4, label %merge_5

merge_5:                                          ; preds = %entry
  ret i64 0

then_4:                                           ; preds = %entry
  ret i64 0
}

define i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 2
  br i1 %cmplttmp, label %then_6, label %merge_7

merge_7:                                          ; preds = %entry
  ret i64 0

then_6:                                           ; preds = %entry
  ret i64 0
}

define i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AE\D8\B7\D8\A3_\D9\82\D8\B1\D8\B5"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 2
  br i1 %cmplttmp, label %then_8, label %merge_9

merge_9:                                          ; preds = %entry
  ret i64 0

then_8:                                           ; preds = %entry
  ret i64 0
}

define i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1") {
entry:
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11", align 8
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_10

merge_14:                                         ; preds = %while_body_11
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load2" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load2", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_10

then_13:                                          ; preds = %while_body_11
  ret i64 0

while_body_11:                                    ; preds = %while_cond_10
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %shrtmp = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 7
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_13, label %merge_14

while_cond_10:                                    ; preds = %merge_14, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 100000
  br i1 %cmplttmp, label %while_body_11, label %while_exit_12

while_exit_12:                                    ; preds = %while_cond_10
  ret i64 -1
}

define i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\B7\D9\84\D8\A8_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1") {
entry:
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11", align 8
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_15

merge_19:                                         ; preds = %while_body_16
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %shrtmp = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2", 5
  %andtmp3 = and i64 %shrtmp, 1
  %cmpeqtmp4 = icmp eq i64 %andtmp3, 1
  br i1 %cmpeqtmp4, label %then_20, label %merge_21

merge_21:                                         ; preds = %merge_19
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %shrtmp6 = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5", 7
  %andtmp7 = and i64 %shrtmp6, 1
  %cmpeqtmp8 = icmp eq i64 %andtmp7, 0
  br i1 %cmpeqtmp8, label %then_22, label %merge_23

merge_23:                                         ; preds = %merge_25, %merge_21
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load13" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load13", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_15

merge_25:                                         ; preds = %then_22
  br label %merge_23

then_18:                                          ; preds = %while_body_16
  ret i64 -1

then_20:                                          ; preds = %merge_19
  ret i64 -1

then_22:                                          ; preds = %merge_21
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %shrtmp10 = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9", 3
  %andtmp11 = and i64 %shrtmp10, 1
  %cmpeqtmp12 = icmp eq i64 %andtmp11, 1
  br i1 %cmpeqtmp12, label %then_24, label %merge_25

then_24:                                          ; preds = %then_22
  ret i64 0

while_body_16:                                    ; preds = %while_cond_15
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %andtmp = and i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_18, label %merge_19

while_cond_15:                                    ; preds = %merge_23, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 100000
  br i1 %cmplttmp, label %while_body_16, label %while_exit_17

while_exit_17:                                    ; preds = %while_cond_15
  ret i64 -2
}

define i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\AC\D8\A7\D9\87\D8\B2"(i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1") {
entry:
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11" = alloca i64, align 8
  store i64 %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11", align 8
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_26

merge_30:                                         ; preds = %merge_32, %while_body_27
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load6" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load6", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_26

merge_32:                                         ; preds = %then_29
  br label %merge_30

then_29:                                          ; preds = %while_body_27
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %shrtmp3 = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2", 6
  %andtmp4 = and i64 %shrtmp3, 1
  %cmpeqtmp5 = icmp eq i64 %andtmp4, 1
  br i1 %cmpeqtmp5, label %then_31, label %merge_32

then_31:                                          ; preds = %then_29
  ret i64 0

while_body_27:                                    ; preds = %while_cond_26
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B11", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %shrtmp = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 7
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_29, label %merge_30

while_cond_26:                                    ; preds = %merge_30, %entry
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 100000
  br i1 %cmplttmp, label %while_body_27, label %while_exit_28

while_exit_28:                                    ; preds = %while_cond_26
  ret i64 -1
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\AE\D8\B7\D8\A3_ATA"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AE\D8\B7\D8\A3" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load")
  store i64 %calltmp, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %andtmp = and i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_33, label %merge_34

merge_34:                                         ; preds = %entry
  ret i64 0

then_33:                                          ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp3 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AE\D8\B7\D8\A3_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2")
  store i64 %calltmp3, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AE\D8\B7\D8\A3", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AE\D8\B7\D8\A3.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AE\D8\B7\D8\A3", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AE\D8\B7\D8\A3.load")
  ret i64 0
}

define void @"\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D8\AA\D8\B9\D9\8A\D9\8A\D9\86_ATA"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\84") {
entry:
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\841" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\84", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\841", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\84.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\841", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\84.load", 1
  br i1 %cmpeqtmp, label %then_35, label %merge_36

merge_36:                                         ; preds = %then_35, %entry
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @sad_ll_port_outb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load", i64 4)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load2" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load2")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load3" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load3")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load4" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @sad_ll_port_outb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load4", i64 0)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load5" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load5")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load6" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load6")
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\84.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\841", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\86\D8\A7\D9\82\D9\84.load7", 1
  br i1 %cmpeqtmp8, label %then_37, label %merge_38

merge_38:                                         ; preds = %then_37, %merge_36
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  ret void

then_35:                                          ; preds = %entry
  br label %merge_36

then_37:                                          ; preds = %merge_36
  br label %merge_38
}

define void @"\D8\A7\D8\AE\D8\AA\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D9\8A\D8\A7\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  store i64 %arr.get5, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D9\8A\D8\A7\D8\B1", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %addtmp = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 6
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D9\8A\D8\A7\D8\B1.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D9\8A\D8\A7\D8\B1", align 8
  call void @sad_ll_port_outb(i64 %addtmp, i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D8\A7\D8\AE\D8\AA\D9\8A\D8\A7\D8\B1.load")
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6")
  store i64 %calltmp, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load")
  ret void
}

define i64 @"\D8\AA\D8\B9\D8\B1\D9\81_\D8\B9\D9\84\D9\89_\D9\82\D8\B1\D8\B5"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_hi" = alloca i64, align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_lo" = alloca i64, align 8
  %"%\D8\A3\D9\88\D8\A7\D9\85\D8\B1_83" = alloca i64, align 8
  %"%\D9\8A\D8\AF\D8\B9\D9\85_lba" = alloca i64, align 8
  %"%\D9\82\D8\AF\D8\B1\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\A8" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ" = alloca i64, align 8
  %"%lba_hi" = alloca i64, align 8
  %"%lba_mid" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  store i64 %arr.get, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load2")
  store i64 %calltmp, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load3" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp4 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load3")
  store i64 %calltmp4, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  call void @"\D8\A7\D8\AE\D8\AA\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 2
  call void @sad_ll_port_outb(i64 %addtmp, i64 0)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load6" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp7 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load6", 3
  call void @sad_ll_port_outb(i64 %addtmp7, i64 0)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load8" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp9 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load8", 4
  call void @sad_ll_port_outb(i64 %addtmp9, i64 0)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load10" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp11 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load10", 5
  call void @sad_ll_port_outb(i64 %addtmp11, i64 0)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  call void @sad_ll_port_inb(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 0
  br i1 %cmpeqtmp, label %then_39, label %merge_40

merge_40:                                         ; preds = %entry
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load15" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp16 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load15")
  store i64 %calltmp16, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_\D8\A7\D9\84\D8\A7\D9\86\D8\AA\D8\B8\D8\A7\D8\B1.load", 0
  br i1 %cmpnetmp, label %then_41, label %merge_42

merge_42:                                         ; preds = %merge_40
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load20" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp21 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load20", 4
  call void @sad_ll_port_inb(i64 %addtmp21)
  store i64 0, ptr %"%lba_mid", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load22" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp23 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load22", 5
  call void @sad_ll_port_inb(i64 %addtmp23)
  store i64 0, ptr %"%lba_hi", align 8
  %"%lba_mid.load" = load i64, ptr %"%lba_mid", align 8
  %cmpnetmp24 = icmp ne i64 %"%lba_mid.load", 0
  br i1 %cmpnetmp24, label %then_43, label %merge_44

merge_44:                                         ; preds = %merge_42
  %"%lba_hi.load" = load i64, ptr %"%lba_hi", align 8
  %cmpnetmp27 = icmp ne i64 %"%lba_hi.load", 0
  br i1 %cmpnetmp27, label %then_49, label %merge_50

merge_46:                                         ; preds = %merge_48, %then_43
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load30" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load30"
  store i64 0, ptr %arr.elem32, align 8
  ret i64 0

merge_48:                                         ; preds = %then_45
  br label %merge_46

merge_50:                                         ; preds = %merge_44
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load39" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp40 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\B7\D9\84\D8\A8_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load39")
  store i64 %calltmp40, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ", align 8
  %cmpnetmp41 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ.load", 0
  br i1 %cmpnetmp41, label %then_51, label %merge_52

merge_52:                                         ; preds = %merge_50
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_53

merge_60:                                         ; preds = %then_59, %while_exit_58
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load78" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28.load79" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28", align 8
  %arr.data80 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem81 = getelementptr i64, ptr %arr.data80, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load78"
  store i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28.load79", ptr %arr.elem81, align 8
  %arr.data82 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem83 = getelementptr i64, ptr %arr.data82, i64 27
  %arr.get84 = load i64, ptr %arr.elem83, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load85" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data86 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem87 = getelementptr i64, ptr %arr.data86, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load85"
  store i64 %arr.get84, ptr %arr.elem87, align 8
  %arr.data88 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem89 = getelementptr i64, ptr %arr.data88, i64 28
  %arr.get90 = load i64, ptr %arr.elem89, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load91" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data92 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem93 = getelementptr i64, ptr %arr.data92, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load91"
  store i64 %arr.get90, ptr %arr.elem93, align 8
  %arr.data94 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem95 = getelementptr i64, ptr %arr.data94, i64 29
  %arr.get96 = load i64, ptr %arr.elem95, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load97" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data98 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem99 = getelementptr i64, ptr %arr.data98, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load97"
  store i64 %arr.get96, ptr %arr.elem99, align 8
  %arr.data100 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem101 = getelementptr i64, ptr %arr.data100, i64 30
  %arr.get102 = load i64, ptr %arr.elem101, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load103" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data104 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem105 = getelementptr i64, ptr %arr.data104, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load103"
  store i64 %arr.get102, ptr %arr.elem105, align 8
  ret i64 1

then_39:                                          ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load12" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load12"
  store i64 0, ptr %arr.elem14, align 8
  ret i64 0

then_41:                                          ; preds = %merge_40
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load17" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load17"
  store i64 0, ptr %arr.elem19, align 8
  ret i64 0

then_43:                                          ; preds = %merge_42
  %"%lba_mid.load25" = load i64, ptr %"%lba_mid", align 8
  %cmpeqtmp26 = icmp eq i64 %"%lba_mid.load25", 20
  br i1 %cmpeqtmp26, label %then_45, label %merge_46

then_45:                                          ; preds = %then_43
  %"%lba_hi.load28" = load i64, ptr %"%lba_hi", align 8
  %cmpeqtmp29 = icmp eq i64 %"%lba_hi.load28", 235
  br i1 %cmpeqtmp29, label %then_47, label %merge_48

then_47:                                          ; preds = %then_45
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load33" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data34 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load33"
  store i64 2, ptr %arr.elem35, align 8
  ret i64 0

then_49:                                          ; preds = %merge_44
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load36" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data37 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load36"
  store i64 0, ptr %arr.elem38, align 8
  ret i64 0

then_51:                                          ; preds = %merge_50
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load42" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data43 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load42"
  store i64 0, ptr %arr.elem44, align 8
  ret i64 0

then_59:                                          ; preds = %while_exit_58
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load75" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data76 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem77 = getelementptr i64, ptr %arr.data76, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load75"
  store i64 1, ptr %arr.elem77, align 8
  br label %merge_60

while_body_54:                                    ; preds = %while_cond_53
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load45" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  call void @sad_ll_port_inw(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load45")
  %"%\D9\83.load46" = load i64, ptr %"%\D9\83", align 8
  %arr.data47 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem48 = getelementptr i64, ptr %arr.data47, i64 %"%\D9\83.load46"
  store i64 0, ptr %arr.elem48, align 8
  %"%\D9\83.load49" = load i64, ptr %"%\D9\83", align 8
  %addtmp50 = add i64 %"%\D9\83.load49", 1
  store i64 %addtmp50, ptr %"%\D9\83", align 8
  br label %while_cond_53

while_body_57:                                    ; preds = %while_cond_56
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load52" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  call void @sad_ll_port_inw(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load52")
  %"%\D8\A8.load53" = load i64, ptr %"%\D8\A8", align 8
  %addtmp54 = add i64 %"%\D8\A8.load53", 1
  store i64 %addtmp54, ptr %"%\D8\A8", align 8
  br label %while_cond_56

while_cond_53:                                    ; preds = %while_body_54, %merge_52
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp = icmp slt i64 %"%\D9\83.load", 64
  br i1 %cmplttmp, label %while_body_54, label %while_exit_55

while_cond_56:                                    ; preds = %while_body_57, %while_exit_55
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %cmplttmp51 = icmp slt i64 %"%\D8\A8.load", 192
  br i1 %cmplttmp51, label %while_body_57, label %while_exit_58

while_exit_55:                                    ; preds = %while_cond_53
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_56

while_exit_58:                                    ; preds = %while_cond_56
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load55" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data56 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem57 = getelementptr i64, ptr %arr.data56, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load55"
  store i64 1, ptr %arr.elem57, align 8
  %arr.data58 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem59 = getelementptr i64, ptr %arr.data58, i64 49
  %arr.get60 = load i64, ptr %arr.elem59, align 8
  store i64 %arr.get60, ptr %"%\D9\82\D8\AF\D8\B1\D8\A7\D8\AA", align 8
  %"%\D9\82\D8\AF\D8\B1\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\82\D8\AF\D8\B1\D8\A7\D8\AA", align 8
  %shrtmp = lshr i64 %"%\D9\82\D8\AF\D8\B1\D8\A7\D8\AA.load", 9
  %andtmp = and i64 %shrtmp, 1
  store i64 %andtmp, ptr %"%\D9\8A\D8\AF\D8\B9\D9\85_lba", align 8
  %arr.data61 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 53
  %arr.get63 = load i64, ptr %arr.elem62, align 8
  store i64 %arr.get63, ptr %"%\D8\A3\D9\88\D8\A7\D9\85\D8\B1_83", align 8
  %arr.data64 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 60
  %arr.get66 = load i64, ptr %arr.elem65, align 8
  store i64 %arr.get66, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_lo", align 8
  %arr.data67 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem68 = getelementptr i64, ptr %arr.data67, i64 61
  %arr.get69 = load i64, ptr %arr.elem68, align 8
  store i64 %arr.get69, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_hi", align 8
  %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_hi.load" = load i64, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_hi", align 8
  %shltmp = shl i64 %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_hi.load", 16
  %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_lo.load" = load i64, ptr %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_lo", align 8
  %addtmp70 = add i64 %"%\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_lo.load", %shltmp
  store i64 %addtmp70, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load71" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28.load" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28", align 8
  %arr.data72 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem73 = getelementptr i64, ptr %arr.data72, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load71"
  store i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28.load", ptr %arr.elem73, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28.load74" = load i64, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_28.load74", 268435455
  br i1 %cmpgttmp, label %then_59, label %merge_60
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ" = alloca i64, align 8
  %"%lba_\D8\A3\D8\B9\D9\84\D9\89" = alloca i64, align 8
  %"%drive_byte" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 0
  br i1 %cmplttmp, label %then_61, label %merge_62

merge_62:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", 0
  br i1 %cmpgetmp, label %then_63, label %merge_64

merge_64:                                         ; preds = %merge_62
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 1
  br i1 %cmpnetmp, label %then_65, label %merge_66

merge_66:                                         ; preds = %merge_64
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  store i64 %arr.get9, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10")
  store i64 %calltmp, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp12 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11")
  store i64 %calltmp12, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @sad_ll_interrupt_disable()
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp13 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  %cmpnetmp14 = icmp ne i64 %calltmp13, 0
  br i1 %cmpnetmp14, label %then_67, label %merge_68

merge_68:                                         ; preds = %merge_66
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15"
  %arr.get18 = load i64, ptr %arr.elem17, align 8
  store i64 %arr.get18, ptr %"%drive_byte", align 8
  %"%drive_byte.load" = load i64, ptr %"%drive_byte", align 8
  %addtmp = add i64 %"%drive_byte.load", 64
  store i64 %addtmp, ptr %"%drive_byte", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load", 24
  %andtmp = and i64 %shrtmp, 15
  store i64 %andtmp, ptr %"%lba_\D8\A3\D8\B9\D9\84\D9\89", align 8
  %"%drive_byte.load19" = load i64, ptr %"%drive_byte", align 8
  %"%lba_\D8\A3\D8\B9\D9\84\D9\89.load" = load i64, ptr %"%lba_\D8\A3\D8\B9\D9\84\D9\89", align 8
  %addtmp20 = add i64 %"%drive_byte.load19", %"%lba_\D8\A3\D8\B9\D9\84\D9\89.load"
  store i64 %addtmp20, ptr %"%drive_byte", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp21 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 6
  %"%drive_byte.load22" = load i64, ptr %"%drive_byte", align 8
  call void @sad_ll_port_outb(i64 %addtmp21, i64 %"%drive_byte.load22")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load23" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp24 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load23", 2
  call void @sad_ll_port_outb(i64 %addtmp24, i64 1)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load25" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp26 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load25", 3
  %"%\D8\B1\D9\82\D9\85_LBA.load27" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %andtmp28 = and i64 %"%\D8\B1\D9\82\D9\85_LBA.load27", 255
  call void @sad_ll_port_outb(i64 %addtmp26, i64 %andtmp28)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load29" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp30 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load29", 4
  %"%\D8\B1\D9\82\D9\85_LBA.load31" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp32 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load31", 8
  %andtmp33 = and i64 %shrtmp32, 255
  call void @sad_ll_port_outb(i64 %addtmp30, i64 %andtmp33)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load34" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp35 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load34", 5
  %"%\D8\B1\D9\82\D9\85_LBA.load36" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp37 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load36", 16
  %andtmp38 = and i64 %shrtmp37, 255
  call void @sad_ll_port_outb(i64 %addtmp35, i64 %andtmp38)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load39" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp40 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\B7\D9\84\D8\A8_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load39")
  store i64 %calltmp40, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ", align 8
  %cmpnetmp41 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ.load", 0
  br i1 %cmpnetmp41, label %then_69, label %merge_70

merge_70:                                         ; preds = %merge_68
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_71

then_61:                                          ; preds = %entry
  ret i64 -1

then_63:                                          ; preds = %merge_62
  ret i64 -1

then_65:                                          ; preds = %merge_64
  ret i64 -1

then_67:                                          ; preds = %merge_66
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_69:                                          ; preds = %merge_68
  call void @sad_ll_interrupt_enable()
  ret i64 -1

while_body_72:                                    ; preds = %while_cond_71
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load43" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  call void @sad_ll_port_inw(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load43")
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D9\83.load44" = load i64, ptr %"%\D9\83", align 8
  %shltmp = shl i64 %"%\D9\83.load44", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %addtmp45 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load", %shltmp
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_write16(i64 %addtmp45, i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  %"%\D9\83.load46" = load i64, ptr %"%\D9\83", align 8
  %addtmp47 = add i64 %"%\D9\83.load46", 1
  store i64 %addtmp47, ptr %"%\D9\83", align 8
  br label %while_cond_71

while_cond_71:                                    ; preds = %while_body_72, %merge_70
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp42 = icmp slt i64 %"%\D9\83.load", 256
  br i1 %cmplttmp42, label %while_body_72, label %while_exit_73

while_exit_73:                                    ; preds = %while_cond_71
  call void @sad_ll_interrupt_enable()
  ret i64 0
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_PIO"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\86\D8\AC\D8\AD" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  store i64 0, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_74

merge_78:                                         ; preds = %merge_85, %while_body_75
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %addtmp8 = add i64 %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load", 1
  store i64 %addtmp8, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %"%\D9\82.load9" = load i64, ptr %"%\D9\82", align 8
  %addtmp10 = add i64 %"%\D9\82.load9", 1
  store i64 %addtmp10, ptr %"%\D9\82", align 8
  br label %while_cond_74

merge_83:                                         ; preds = %then_82, %while_body_80
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load23" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  %addtmp24 = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load23", 1
  store i64 %addtmp24, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  br label %while_cond_79

merge_85:                                         ; preds = %while_exit_81
  br label %merge_78

then_77:                                          ; preds = %while_body_75
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D9\86\D8\AC\D8\AD", align 8
  br label %while_cond_79

then_82:                                          ; preds = %while_body_80
  store i64 1, ptr %"%\D9\86\D8\AC\D8\AD", align 8
  br label %merge_83

then_84:                                          ; preds = %while_exit_81
  %loadtmp25 = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp25

while_body_75:                                    ; preds = %while_cond_74
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D9\82.load5" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_LBA.load", %"%\D9\82.load5"
  %"%\D9\82.load6" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load6", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load", %multmp
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %addtmp, i64 %addtmp7)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_77, label %merge_78

while_body_80:                                    ; preds = %while_cond_79
  %"%\D8\B1\D9\82\D9\85_LBA.load12" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D9\82.load13" = load i64, ptr %"%\D9\82", align 8
  %addtmp14 = add i64 %"%\D8\B1\D9\82\D9\85_LBA.load12", %"%\D9\82.load13"
  %"%\D9\82.load15" = load i64, ptr %"%\D9\82", align 8
  %multmp16 = mul i64 %"%\D9\82.load15", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load17" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp18 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load17", %multmp16
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load19" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp20 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load19", i64 %addtmp14, i64 %addtmp18)
  store i64 %calltmp20, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load21" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load21", 0
  br i1 %cmpeqtmp, label %then_82, label %merge_83

while_cond_74:                                    ; preds = %merge_78, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_75, label %while_exit_76

while_cond_79:                                    ; preds = %merge_83, %then_77
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  %cmplttmp11 = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load", 0
  br i1 %cmplttmp11, label %while_body_80, label %while_exit_81

while_exit_76:                                    ; preds = %while_cond_74
  %loadtmp = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp

while_exit_81:                                    ; preds = %while_cond_79
  %"%\D9\86\D8\AC\D8\AD.load" = load i64, ptr %"%\D9\86\D8\AC\D8\AD", align 8
  %cmpeqtmp22 = icmp eq i64 %"%\D9\86\D8\AC\D8\AD.load", 0
  br i1 %cmpeqtmp22, label %then_84, label %merge_85
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ" = alloca i64, align 8
  %"%lba_\D8\A3\D8\B9\D9\84\D9\89" = alloca i64, align 8
  %"%drive_byte" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 0
  br i1 %cmplttmp, label %then_86, label %merge_87

merge_100:                                        ; preds = %while_exit_98
  call void @sad_ll_interrupt_enable()
  ret i64 0

merge_87:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", 0
  br i1 %cmpgetmp, label %then_88, label %merge_89

merge_89:                                         ; preds = %merge_87
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 1
  br i1 %cmpnetmp, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  store i64 %arr.get9, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10")
  store i64 %calltmp, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp12 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11")
  store i64 %calltmp12, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @sad_ll_interrupt_disable()
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp13 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  %cmpnetmp14 = icmp ne i64 %calltmp13, 0
  br i1 %cmpnetmp14, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data16 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15"
  %arr.get18 = load i64, ptr %arr.elem17, align 8
  %addtmp = add i64 %arr.get18, 64
  store i64 %addtmp, ptr %"%drive_byte", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load", 24
  %andtmp = and i64 %shrtmp, 15
  store i64 %andtmp, ptr %"%lba_\D8\A3\D8\B9\D9\84\D9\89", align 8
  %"%drive_byte.load" = load i64, ptr %"%drive_byte", align 8
  %"%lba_\D8\A3\D8\B9\D9\84\D9\89.load" = load i64, ptr %"%lba_\D8\A3\D8\B9\D9\84\D9\89", align 8
  %addtmp19 = add i64 %"%drive_byte.load", %"%lba_\D8\A3\D8\B9\D9\84\D9\89.load"
  store i64 %addtmp19, ptr %"%drive_byte", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp20 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 6
  %"%drive_byte.load21" = load i64, ptr %"%drive_byte", align 8
  call void @sad_ll_port_outb(i64 %addtmp20, i64 %"%drive_byte.load21")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load22" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp23 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load22", 2
  call void @sad_ll_port_outb(i64 %addtmp23, i64 1)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load24" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp25 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load24", 3
  %"%\D8\B1\D9\82\D9\85_LBA.load26" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %andtmp27 = and i64 %"%\D8\B1\D9\82\D9\85_LBA.load26", 255
  call void @sad_ll_port_outb(i64 %addtmp25, i64 %andtmp27)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load28" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp29 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load28", 4
  %"%\D8\B1\D9\82\D9\85_LBA.load30" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp31 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load30", 8
  %andtmp32 = and i64 %shrtmp31, 255
  call void @sad_ll_port_outb(i64 %addtmp29, i64 %andtmp32)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load33" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp34 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load33", 5
  %"%\D8\B1\D9\82\D9\85_LBA.load35" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp36 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load35", 16
  %andtmp37 = and i64 %shrtmp36, 255
  call void @sad_ll_port_outb(i64 %addtmp34, i64 %andtmp37)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load38" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp39 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\B7\D9\84\D8\A8_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load38")
  store i64 %calltmp39, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ", align 8
  %cmpnetmp40 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9_DRQ.load", 0
  br i1 %cmpnetmp40, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_96

then_86:                                          ; preds = %entry
  ret i64 -1

then_88:                                          ; preds = %merge_87
  ret i64 -1

then_90:                                          ; preds = %merge_89
  ret i64 -1

then_92:                                          ; preds = %merge_91
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_94:                                          ; preds = %merge_93
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_99:                                          ; preds = %while_exit_98
  call void @sad_ll_interrupt_enable()
  ret i64 -1

while_body_97:                                    ; preds = %while_cond_96
  %"%\D9\83.load42" = load i64, ptr %"%\D9\83", align 8
  %shltmp = shl i64 %"%\D9\83.load42", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %addtmp43 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load", %shltmp
  call void @sad_ll_mem_read16(i64 %addtmp43)
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load44" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_port_outw(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load44", i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  %"%\D9\83.load45" = load i64, ptr %"%\D9\83", align 8
  %addtmp46 = add i64 %"%\D9\83.load45", 1
  store i64 %addtmp46, ptr %"%\D9\83", align 8
  br label %while_cond_96

while_cond_96:                                    ; preds = %while_body_97, %merge_95
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp41 = icmp slt i64 %"%\D9\83.load", 256
  br i1 %cmplttmp41, label %while_body_97, label %while_exit_98

while_exit_98:                                    ; preds = %while_cond_96
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load47" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp48 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load47")
  %cmpnetmp49 = icmp ne i64 %calltmp48, 0
  br i1 %cmpnetmp49, label %then_99, label %merge_100
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_PIO"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\86\D8\AC\D8\AD" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  store i64 0, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_101

merge_105:                                        ; preds = %merge_112, %while_body_102
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %addtmp8 = add i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load", 1
  store i64 %addtmp8, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %"%\D9\82.load9" = load i64, ptr %"%\D9\82", align 8
  %addtmp10 = add i64 %"%\D9\82.load9", 1
  store i64 %addtmp10, ptr %"%\D9\82", align 8
  br label %while_cond_101

merge_110:                                        ; preds = %then_109, %while_body_107
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load23" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  %addtmp24 = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load23", 1
  store i64 %addtmp24, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  br label %while_cond_106

merge_112:                                        ; preds = %while_exit_108
  br label %merge_105

then_104:                                         ; preds = %while_body_102
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  store i64 0, ptr %"%\D9\86\D8\AC\D8\AD", align 8
  br label %while_cond_106

then_109:                                         ; preds = %while_body_107
  store i64 1, ptr %"%\D9\86\D8\AC\D8\AD", align 8
  br label %merge_110

then_111:                                         ; preds = %while_exit_108
  %loadtmp25 = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp25

while_body_102:                                   ; preds = %while_cond_101
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D9\82.load5" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_LBA.load", %"%\D9\82.load5"
  %"%\D9\82.load6" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load6", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load", %multmp
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %addtmp, i64 %addtmp7)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_104, label %merge_105

while_body_107:                                   ; preds = %while_cond_106
  %"%\D8\B1\D9\82\D9\85_LBA.load12" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D9\82.load13" = load i64, ptr %"%\D9\82", align 8
  %addtmp14 = add i64 %"%\D8\B1\D9\82\D9\85_LBA.load12", %"%\D9\82.load13"
  %"%\D9\82.load15" = load i64, ptr %"%\D9\82", align 8
  %multmp16 = mul i64 %"%\D9\82.load15", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load17" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp18 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load17", %multmp16
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load19" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp20 = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load19", i64 %addtmp14, i64 %addtmp18)
  store i64 %calltmp20, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load21" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load21", 0
  br i1 %cmpeqtmp, label %then_109, label %merge_110

while_cond_101:                                   ; preds = %merge_105, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load"
  br i1 %cmplttmp, label %while_body_102, label %while_exit_103

while_cond_106:                                   ; preds = %merge_110, %then_104
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9", align 8
  %cmplttmp11 = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A9.load", 0
  br i1 %cmplttmp11, label %while_body_107, label %while_exit_108

while_exit_103:                                   ; preds = %while_cond_101
  %loadtmp = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp

while_exit_108:                                   ; preds = %while_cond_106
  %"%\D9\86\D8\AC\D8\AD.load" = load i64, ptr %"%\D9\86\D8\AC\D8\AD", align 8
  %cmpeqtmp22 = icmp eq i64 %"%\D9\86\D8\AC\D8\AD.load", 0
  br i1 %cmpeqtmp22, label %then_111, label %merge_112
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO48"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%lba5" = alloca i64, align 8
  %"%lba4" = alloca i64, align 8
  %"%lba3" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 0
  br i1 %cmplttmp, label %then_113, label %merge_114

merge_114:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", 0
  br i1 %cmpgetmp, label %then_115, label %merge_116

merge_116:                                        ; preds = %merge_114
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 1
  br i1 %cmpnetmp, label %then_117, label %merge_118

merge_118:                                        ; preds = %merge_116
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %cmpeqtmp = icmp eq i64 %arr.get9, 0
  br i1 %cmpeqtmp, label %then_119, label %merge_120

merge_120:                                        ; preds = %merge_118
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11"
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  store i64 %arr.get14, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp16 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15")
  store i64 %calltmp16, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load17" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp18 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load17")
  store i64 %calltmp18, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @sad_ll_interrupt_disable()
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp19 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  %cmpnetmp20 = icmp ne i64 %calltmp19, 0
  br i1 %cmpnetmp20, label %then_121, label %merge_122

merge_122:                                        ; preds = %merge_120
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 6
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load21" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load21"
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  %addtmp25 = add i64 %arr.get24, 64
  call void @sad_ll_port_outb(i64 %addtmp, i64 %addtmp25)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load26" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp27 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load26", 2
  call void @sad_ll_port_outb(i64 %addtmp27, i64 0)
  %"%\D8\B1\D9\82\D9\85_LBA.load28" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load28", 24
  %andtmp = and i64 %shrtmp, 255
  store i64 %andtmp, ptr %"%lba3", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load29" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp30 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load29", 32
  %andtmp31 = and i64 %shrtmp30, 255
  store i64 %andtmp31, ptr %"%lba4", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load32" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp33 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load32", 40
  %andtmp34 = and i64 %shrtmp33, 255
  store i64 %andtmp34, ptr %"%lba5", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load35" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp36 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load35", 3
  %"%lba3.load" = load i64, ptr %"%lba3", align 8
  call void @sad_ll_port_outb(i64 %addtmp36, i64 %"%lba3.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load37" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp38 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load37", 4
  %"%lba4.load" = load i64, ptr %"%lba4", align 8
  call void @sad_ll_port_outb(i64 %addtmp38, i64 %"%lba4.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load39" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp40 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load39", 5
  %"%lba5.load" = load i64, ptr %"%lba5", align 8
  call void @sad_ll_port_outb(i64 %addtmp40, i64 %"%lba5.load")
  call void @sad_ll_port_outb(i64 %addtmp27, i64 1)
  %"%\D8\B1\D9\82\D9\85_LBA.load41" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %andtmp42 = and i64 %"%\D8\B1\D9\82\D9\85_LBA.load41", 255
  call void @sad_ll_port_outb(i64 %addtmp36, i64 %andtmp42)
  %"%\D8\B1\D9\82\D9\85_LBA.load43" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp44 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load43", 8
  %andtmp45 = and i64 %shrtmp44, 255
  call void @sad_ll_port_outb(i64 %addtmp38, i64 %andtmp45)
  %"%\D8\B1\D9\82\D9\85_LBA.load46" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp47 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load46", 16
  %andtmp48 = and i64 %shrtmp47, 255
  call void @sad_ll_port_outb(i64 %addtmp40, i64 %andtmp48)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load49" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp50 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\B7\D9\84\D8\A8_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load49")
  %cmpnetmp51 = icmp ne i64 %calltmp50, 0
  br i1 %cmpnetmp51, label %then_123, label %merge_124

merge_124:                                        ; preds = %merge_122
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_125

then_113:                                         ; preds = %entry
  ret i64 -1

then_115:                                         ; preds = %merge_114
  ret i64 -1

then_117:                                         ; preds = %merge_116
  ret i64 -1

then_119:                                         ; preds = %merge_118
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10", i64 %"%\D8\B1\D9\82\D9\85_LBA.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load")
  ret i64 %calltmp

then_121:                                         ; preds = %merge_120
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_123:                                         ; preds = %merge_122
  call void @sad_ll_interrupt_enable()
  ret i64 -1

while_body_126:                                   ; preds = %while_cond_125
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load53" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  call void @sad_ll_port_inw(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load53")
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D9\83.load54" = load i64, ptr %"%\D9\83", align 8
  %shltmp = shl i64 %"%\D9\83.load54", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load55" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %addtmp56 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load55", %shltmp
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_mem_write16(i64 %addtmp56, i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  %"%\D9\83.load57" = load i64, ptr %"%\D9\83", align 8
  %addtmp58 = add i64 %"%\D9\83.load57", 1
  store i64 %addtmp58, ptr %"%\D9\83", align 8
  br label %while_cond_125

while_cond_125:                                   ; preds = %while_body_126, %merge_124
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp52 = icmp slt i64 %"%\D9\83.load", 256
  br i1 %cmplttmp52, label %while_body_126, label %while_exit_127

while_exit_127:                                   ; preds = %while_cond_125
  call void @sad_ll_interrupt_enable()
  ret i64 0
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO48"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9" = alloca i64, align 8
  %"%\D9\83" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 0
  br i1 %cmplttmp, label %then_128, label %merge_129

merge_129:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4", 0
  br i1 %cmpgetmp, label %then_130, label %merge_131

merge_131:                                        ; preds = %merge_129
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 1
  br i1 %cmpnetmp, label %then_132, label %merge_133

merge_133:                                        ; preds = %merge_131
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data7 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6"
  %arr.get9 = load i64, ptr %arr.elem8, align 8
  %cmpeqtmp = icmp eq i64 %arr.get9, 0
  br i1 %cmpeqtmp, label %then_134, label %merge_135

merge_135:                                        ; preds = %merge_133
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data12 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem13 = getelementptr i64, ptr %arr.data12, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11"
  %arr.get14 = load i64, ptr %arr.elem13, align 8
  store i64 %arr.get14, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp16 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load15")
  store i64 %calltmp16, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load17" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp18 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load17")
  store i64 %calltmp18, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @sad_ll_interrupt_disable()
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp19 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  %cmpnetmp20 = icmp ne i64 %calltmp19, 0
  br i1 %cmpnetmp20, label %then_136, label %merge_137

merge_137:                                        ; preds = %merge_135
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 6
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load21" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data22 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load21"
  %arr.get24 = load i64, ptr %arr.elem23, align 8
  %addtmp25 = add i64 %arr.get24, 64
  call void @sad_ll_port_outb(i64 %addtmp, i64 %addtmp25)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load26" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp27 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load26", 2
  call void @sad_ll_port_outb(i64 %addtmp27, i64 0)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load28" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp29 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load28", 3
  %"%\D8\B1\D9\82\D9\85_LBA.load30" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load30", 24
  %andtmp = and i64 %shrtmp, 255
  call void @sad_ll_port_outb(i64 %addtmp29, i64 %andtmp)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load31" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp32 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load31", 4
  %"%\D8\B1\D9\82\D9\85_LBA.load33" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp34 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load33", 32
  %andtmp35 = and i64 %shrtmp34, 255
  call void @sad_ll_port_outb(i64 %addtmp32, i64 %andtmp35)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load36" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp37 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load36", 5
  %"%\D8\B1\D9\82\D9\85_LBA.load38" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp39 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load38", 40
  %andtmp40 = and i64 %shrtmp39, 255
  call void @sad_ll_port_outb(i64 %addtmp37, i64 %andtmp40)
  call void @sad_ll_port_outb(i64 %addtmp27, i64 1)
  %"%\D8\B1\D9\82\D9\85_LBA.load41" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %andtmp42 = and i64 %"%\D8\B1\D9\82\D9\85_LBA.load41", 255
  call void @sad_ll_port_outb(i64 %addtmp29, i64 %andtmp42)
  %"%\D8\B1\D9\82\D9\85_LBA.load43" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp44 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load43", 8
  %andtmp45 = and i64 %shrtmp44, 255
  call void @sad_ll_port_outb(i64 %addtmp32, i64 %andtmp45)
  %"%\D8\B1\D9\82\D9\85_LBA.load46" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp47 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load46", 16
  %andtmp48 = and i64 %shrtmp47, 255
  call void @sad_ll_port_outb(i64 %addtmp37, i64 %andtmp48)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  call void @"\D8\AA\D8\A3\D8\AE\D9\8A\D8\B1_ATA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85.load")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load49" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp50 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\B7\D9\84\D8\A8_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load49")
  %cmpnetmp51 = icmp ne i64 %calltmp50, 0
  br i1 %cmpnetmp51, label %then_138, label %merge_139

merge_139:                                        ; preds = %merge_137
  store i64 0, ptr %"%\D9\83", align 8
  br label %while_cond_140

then_128:                                         ; preds = %entry
  ret i64 -1

then_130:                                         ; preds = %merge_129
  ret i64 -1

then_132:                                         ; preds = %merge_131
  ret i64 -1

then_134:                                         ; preds = %merge_133
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %calltmp = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load10", i64 %"%\D8\B1\D9\82\D9\85_LBA.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load")
  ret i64 %calltmp

then_136:                                         ; preds = %merge_135
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_138:                                         ; preds = %merge_137
  call void @sad_ll_interrupt_enable()
  ret i64 -1

while_body_141:                                   ; preds = %while_cond_140
  %"%\D9\83.load53" = load i64, ptr %"%\D9\83", align 8
  %shltmp = shl i64 %"%\D9\83.load53", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load54" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\863", align 8
  %addtmp55 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load54", %shltmp
  call void @sad_ll_mem_read16(i64 %addtmp55)
  store i64 0, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load56" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load" = load i64, ptr %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9", align 8
  call void @sad_ll_port_outw(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load56", i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\A7\D9\84\D9\83\D9\84\D9\85\D8\A9.load")
  %"%\D9\83.load57" = load i64, ptr %"%\D9\83", align 8
  %addtmp58 = add i64 %"%\D9\83.load57", 1
  store i64 %addtmp58, ptr %"%\D9\83", align 8
  br label %while_cond_140

while_cond_140:                                   ; preds = %while_body_141, %merge_139
  %"%\D9\83.load" = load i64, ptr %"%\D9\83", align 8
  %cmplttmp52 = icmp slt i64 %"%\D9\83.load", 256
  br i1 %cmplttmp52, label %while_body_141, label %while_exit_142

while_exit_142:                                   ; preds = %while_cond_140
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load59" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp60 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load59")
  call void @sad_ll_interrupt_enable()
  ret i64 0
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A933" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 0
  br i1 %cmplttmp, label %then_143, label %merge_144

else_151:                                         ; preds = %merge_148
  br label %while_cond_157

merge_144:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5", 0
  br i1 %cmpgetmp, label %then_145, label %merge_146

merge_146:                                        ; preds = %merge_144
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_147, label %merge_148

merge_148:                                        ; preds = %merge_146
  store i64 0, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  store i64 0, ptr %"%\D9\82", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load7" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  %cmpeqtmp11 = icmp eq i64 %arr.get10, 1
  br i1 %cmpeqtmp11, label %then_149, label %else_151

merge_150:                                        ; preds = %while_exit_159, %while_exit_154
  %loadtmp = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp

merge_156:                                        ; preds = %while_body_153
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %addtmp18 = add i64 %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load", 1
  store i64 %addtmp18, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %"%\D9\82.load19" = load i64, ptr %"%\D9\82", align 8
  %addtmp20 = add i64 %"%\D9\82.load19", 1
  store i64 %addtmp20, ptr %"%\D9\82", align 8
  br label %while_cond_152

merge_161:                                        ; preds = %while_body_158
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load37" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %addtmp38 = add i64 %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load37", 1
  store i64 %addtmp38, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %"%\D9\82.load39" = load i64, ptr %"%\D9\82", align 8
  %addtmp40 = add i64 %"%\D9\82.load39", 1
  store i64 %addtmp40, ptr %"%\D9\82", align 8
  br label %while_cond_157

then_143:                                         ; preds = %entry
  ret i64 0

then_145:                                         ; preds = %merge_144
  ret i64 0

then_147:                                         ; preds = %merge_146
  ret i64 0

then_149:                                         ; preds = %merge_148
  br label %while_cond_152

then_155:                                         ; preds = %while_body_153
  %loadtmp17 = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp17

then_160:                                         ; preds = %while_body_158
  %loadtmp36 = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  ret i64 %loadtmp36

while_body_153:                                   ; preds = %while_cond_152
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D9\82.load13" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", %"%\D9\82.load13"
  %"%\D9\82.load14" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load14", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp15 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load", %multmp
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO48"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16", i64 %addtmp, i64 %addtmp15)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_155, label %merge_156

while_body_158:                                   ; preds = %while_cond_157
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load24" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D9\82.load25" = load i64, ptr %"%\D9\82", align 8
  %addtmp26 = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load24", %"%\D9\82.load25"
  %"%\D9\82.load27" = load i64, ptr %"%\D9\82", align 8
  %multmp28 = mul i64 %"%\D9\82.load27", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load29" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp30 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load29", %multmp28
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load31" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp32 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load31", i64 %addtmp26, i64 %addtmp30)
  store i64 %calltmp32, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A933", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load34" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A933", align 8
  %cmpnetmp35 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load34", 0
  br i1 %cmpnetmp35, label %then_160, label %merge_161

while_cond_152:                                   ; preds = %merge_156, %then_149
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp12 = icmp slt i64 %"%\D9\82.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load"
  br i1 %cmplttmp12, label %while_body_153, label %while_exit_154

while_cond_157:                                   ; preds = %merge_161, %else_151
  %"%\D9\82.load21" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load22" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp23 = icmp slt i64 %"%\D9\82.load21", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load22"
  br i1 %cmplttmp23, label %while_body_158, label %while_exit_159

while_exit_154:                                   ; preds = %while_cond_152
  br label %merge_150

while_exit_159:                                   ; preds = %while_cond_157
  br label %merge_150
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86") {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A933" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", 0
  br i1 %cmplttmp, label %then_162, label %merge_163

else_170:                                         ; preds = %merge_167
  br label %while_cond_176

merge_163:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5", 0
  br i1 %cmpgetmp, label %then_164, label %merge_165

merge_165:                                        ; preds = %merge_163
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load6"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_166, label %merge_167

merge_167:                                        ; preds = %merge_165
  store i64 0, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  store i64 0, ptr %"%\D9\82", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load7" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  %cmpeqtmp11 = icmp eq i64 %arr.get10, 1
  br i1 %cmpeqtmp11, label %then_168, label %else_170

merge_169:                                        ; preds = %while_exit_178, %while_exit_173
  %loadtmp = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp

merge_175:                                        ; preds = %while_body_172
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %addtmp18 = add i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load", 1
  store i64 %addtmp18, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %"%\D9\82.load19" = load i64, ptr %"%\D9\82", align 8
  %addtmp20 = add i64 %"%\D9\82.load19", 1
  store i64 %addtmp20, ptr %"%\D9\82", align 8
  br label %while_cond_171

merge_180:                                        ; preds = %while_body_177
  %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load37" = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %addtmp38 = add i64 %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8.load37", 1
  store i64 %addtmp38, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  %"%\D9\82.load39" = load i64, ptr %"%\D9\82", align 8
  %addtmp40 = add i64 %"%\D9\82.load39", 1
  store i64 %addtmp40, ptr %"%\D9\82", align 8
  br label %while_cond_176

then_162:                                         ; preds = %entry
  ret i64 0

then_164:                                         ; preds = %merge_163
  ret i64 0

then_166:                                         ; preds = %merge_165
  ret i64 0

then_168:                                         ; preds = %merge_167
  br label %while_cond_171

then_174:                                         ; preds = %while_body_172
  %loadtmp17 = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp17

then_179:                                         ; preds = %while_body_177
  %loadtmp36 = load i64, ptr %"%\D9\85\D9\83\D8\AA\D9\88\D8\A8", align 8
  ret i64 %loadtmp36

while_body_172:                                   ; preds = %while_cond_171
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D9\82.load13" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load", %"%\D9\82.load13"
  %"%\D9\82.load14" = load i64, ptr %"%\D9\82", align 8
  %multmp = mul i64 %"%\D9\82.load14", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp15 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load", %multmp
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO48"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16", i64 %addtmp, i64 %addtmp15)
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_174, label %merge_175

while_body_177:                                   ; preds = %while_cond_176
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load24" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B92", align 8
  %"%\D9\82.load25" = load i64, ptr %"%\D9\82", align 8
  %addtmp26 = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9.load24", %"%\D9\82.load25"
  %"%\D9\82.load27" = load i64, ptr %"%\D9\82", align 8
  %multmp28 = mul i64 %"%\D9\82.load27", 0
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load29" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\864", align 8
  %addtmp30 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\AE\D8\B2\D9\86.load29", %multmp28
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load31" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp32 = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load31", i64 %addtmp26, i64 %addtmp30)
  store i64 %calltmp32, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A933", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load34" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A933", align 8
  %cmpnetmp35 = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load34", 0
  br i1 %cmpnetmp35, label %then_179, label %merge_180

while_cond_171:                                   ; preds = %merge_175, %then_168
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp12 = icmp slt i64 %"%\D9\82.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load"
  br i1 %cmplttmp12, label %while_body_172, label %while_exit_173

while_cond_176:                                   ; preds = %merge_180, %else_170
  %"%\D9\82.load21" = load i64, ptr %"%\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load22" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA3", align 8
  %cmplttmp23 = icmp slt i64 %"%\D9\82.load21", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA.load22"
  br i1 %cmplttmp23, label %while_body_177, label %while_exit_178

while_exit_173:                                   ; preds = %while_cond_171
  br label %merge_169

while_exit_178:                                   ; preds = %while_cond_176
  br label %merge_169
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\AC\D8\AF\D9\88\D9\84_\D8\A3\D9\82\D8\B3\D8\A7\D9\85"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5") {
entry:
  %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85" = alloca i64, align 8
  %"%sec_hi" = alloca i64, align 8
  %"%sec_lo" = alloca i64, align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%lba_hi" = alloca i64, align 8
  %"%lba_lo" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85" = alloca i64, align 8
  %"%type_word" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B3\D9\85" = alloca i64, align 8
  %"%status_word" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9" = alloca i64, align 8
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9_lo" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_181, label %merge_182

merge_182:                                        ; preds = %entry
  br i1 true, label %then_183, label %merge_184

merge_184:                                        ; preds = %merge_186, %merge_182
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpnetmp = icmp ne i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmpnetmp, label %then_187, label %merge_188

merge_186:                                        ; preds = %then_183
  br label %merge_184

merge_188:                                        ; preds = %merge_184
  call void @sad_ll_mem_read16(i64 510)
  store i64 0, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9_lo", align 8
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9_lo.load" = load i64, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9_lo", align 8
  %cmpnetmp2 = icmp ne i64 %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9_lo.load", 21930
  br i1 %cmpnetmp2, label %then_189, label %merge_190

merge_190:                                        ; preds = %merge_188
  store i64 0, ptr %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9", align 8
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_191

merge_195:                                        ; preds = %while_body_192
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %addtmp4 = add i64 0, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  call void @sad_ll_mem_read16(i64 %addtmp4)
  store i64 0, ptr %"%status_word", align 8
  %"%status_word.load" = load i64, ptr %"%status_word", align 8
  %andtmp = and i64 %"%status_word.load", 255
  store i64 %andtmp, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %addtmp5 = add i64 %addtmp4, 4
  call void @sad_ll_mem_read16(i64 %addtmp5)
  store i64 0, ptr %"%type_word", align 8
  %"%type_word.load" = load i64, ptr %"%type_word", align 8
  %andtmp6 = and i64 %"%type_word.load", 255
  store i64 %andtmp6, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %cmpeqtmp7 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load", 0
  br i1 %cmpeqtmp7, label %then_197, label %merge_198

merge_198:                                        ; preds = %merge_195
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load10" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %addtmp11 = add i64 0, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load10"
  %addtmp12 = add i64 %addtmp11, 8
  call void @sad_ll_mem_read16(i64 %addtmp12)
  store i64 0, ptr %"%lba_lo", align 8
  %addtmp13 = add i64 %addtmp11, 10
  call void @sad_ll_mem_read16(i64 %addtmp13)
  store i64 0, ptr %"%lba_hi", align 8
  %"%lba_hi.load" = load i64, ptr %"%lba_hi", align 8
  %shltmp14 = shl i64 %"%lba_hi.load", 16
  %"%lba_lo.load" = load i64, ptr %"%lba_lo", align 8
  %addtmp15 = add i64 %"%lba_lo.load", %shltmp14
  store i64 %addtmp15, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %addtmp16 = add i64 %addtmp11, 12
  call void @sad_ll_mem_read16(i64 %addtmp16)
  store i64 0, ptr %"%sec_lo", align 8
  %addtmp17 = add i64 %addtmp11, 14
  call void @sad_ll_mem_read16(i64 %addtmp17)
  store i64 0, ptr %"%sec_hi", align 8
  %"%sec_hi.load" = load i64, ptr %"%sec_hi", align 8
  %shltmp18 = shl i64 %"%sec_hi.load", 16
  %"%sec_lo.load" = load i64, ptr %"%sec_lo", align 8
  %addtmp19 = add i64 %"%sec_lo.load", %shltmp18
  store i64 %addtmp19, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load20" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data21 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load20"
  store i64 1, ptr %arr.elem22, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load23" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data24 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load23"
  store i64 0, ptr %arr.elem25, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %cmpeqtmp26 = icmp eq i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load", 128
  br i1 %cmpeqtmp26, label %then_200, label %merge_201

merge_201:                                        ; preds = %then_200, %merge_198
  %"%\D9\81\D9\87\D8\B1\D8\B3.load30" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load31" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %arr.data32 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem33 = getelementptr i64, ptr %arr.data32, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load30"
  store i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load31", ptr %arr.elem33, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load34" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  %arr.data35 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem36 = getelementptr i64, ptr %arr.data35, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load34"
  store i64 %"%\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load", ptr %arr.elem36, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load37" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %arr.data38 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem39 = getelementptr i64, ptr %arr.data38, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load37"
  store i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load", ptr %arr.elem39, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load40" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85", align 8
  %shrtmp = lshr i64 %"%\D8\B9\D8\AF\D8\AF_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load40", 11
  %"%\D9\81\D9\87\D8\B1\D8\B3.load41" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data42 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load41"
  store i64 %shrtmp, ptr %arr.elem43, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load44" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load45" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data46 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load44"
  store i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load45", ptr %arr.elem47, align 8
  %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9.load" = load i64, ptr %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9", align 8
  %addtmp48 = add i64 %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9.load", 1
  store i64 %addtmp48, ptr %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9", align 8
  %"%\D9\82.load49" = load i64, ptr %"%\D9\82", align 8
  %addtmp50 = add i64 %"%\D9\82.load49", 1
  store i64 %addtmp50, ptr %"%\D9\82", align 8
  br label %while_cond_191

then_181:                                         ; preds = %entry
  ret i64 0

then_183:                                         ; preds = %merge_182
  br i1 true, label %then_185, label %merge_186

then_185:                                         ; preds = %then_183
  ret i64 0

then_187:                                         ; preds = %merge_184
  ret i64 0

then_189:                                         ; preds = %merge_188
  ret i64 0

then_194:                                         ; preds = %while_body_192
  store i64 4, ptr %"%\D9\82", align 8
  br label %while_cond_191

then_197:                                         ; preds = %merge_195
  %"%\D9\82.load8" = load i64, ptr %"%\D9\82", align 8
  %addtmp9 = add i64 %"%\D9\82.load8", 1
  store i64 %addtmp9, ptr %"%\D9\82", align 8
  br label %while_cond_191

then_200:                                         ; preds = %merge_198
  %"%\D9\81\D9\87\D8\B1\D8\B3.load27" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %arr.data28 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load27"
  store i64 1, ptr %arr.elem29, align 8
  br label %merge_201

while_body_192:                                   ; preds = %while_cond_191
  %"%\D9\82.load3" = load i64, ptr %"%\D9\82", align 8
  %shltmp = shl i64 %"%\D9\82.load3", 4
  %addtmp = add i64 446, %shltmp
  store i64 %addtmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  br i1 %cmpgetmp, label %then_194, label %merge_195

while_cond_191:                                   ; preds = %merge_201, %then_197, %then_194, %merge_190
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 4
  br i1 %cmplttmp, label %while_body_192, label %while_exit_193

while_exit_193:                                   ; preds = %while_cond_191
  %loadtmp = load i64, ptr %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85_\D9\85\D9\83\D8\AA\D8\B4\D9\81\D8\A9", align 8
  ret i64 %loadtmp
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9_\D9\82\D8\B3\D9\85"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9") {
entry:
  %"%\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%lba_\D9\85\D8\B7\D9\84\D9\82" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load", 0
  br i1 %cmplttmp, label %then_202, label %merge_203

merge_203:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load5", 0
  br i1 %cmpgetmp, label %then_204, label %merge_205

merge_205:                                        ; preds = %merge_203
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load6"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_206, label %merge_207

merge_207:                                        ; preds = %merge_205
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load"
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  %cmpgttmp = icmp sgt i64 %addtmp, %arr.get10
  br i1 %cmpgttmp, label %then_208, label %merge_209

merge_209:                                        ; preds = %merge_211, %merge_207
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load12" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load12"
  %arr.get15 = load i64, ptr %arr.elem14, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load16" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %addtmp17 = add i64 %arr.get15, %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load16"
  store i64 %addtmp17, ptr %"%lba_\D9\85\D8\B7\D9\84\D9\82", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load18" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load18"
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  store i64 %arr.get21, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  %"%\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  %"%lba_\D9\85\D8\B7\D9\84\D9\82.load" = load i64, ptr %"%lba_\D9\85\D8\B7\D9\84\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load22" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A94", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\82\D8\B1\D8\B5.load", i64 %"%lba_\D9\85\D8\B7\D9\84\D9\82.load", i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load22", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\AC\D9\87\D8\A9.load")
  ret i64 %calltmp

merge_211:                                        ; preds = %then_208
  br label %merge_209

then_202:                                         ; preds = %entry
  ret i64 0

then_204:                                         ; preds = %merge_203
  ret i64 0

then_206:                                         ; preds = %merge_205
  ret i64 0

then_208:                                         ; preds = %merge_207
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load11" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %cmpletmp = icmp sle i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load11", 0
  br i1 %cmpletmp, label %then_210, label %merge_211

then_210:                                         ; preds = %then_208
  ret i64 0
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9_\D9\82\D8\B3\D9\85"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85", i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"%\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%lba_\D9\85\D8\B7\D9\84\D9\82" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14", align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load", 0
  br i1 %cmplttmp, label %then_212, label %merge_213

merge_213:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load5" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load5", 0
  br i1 %cmpgetmp, label %then_214, label %merge_215

merge_215:                                        ; preds = %merge_213
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load6" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load6"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_216, label %merge_217

merge_217:                                        ; preds = %merge_215
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %addtmp = add i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load"
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load7" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  %cmpgttmp = icmp sgt i64 %addtmp, %arr.get10
  br i1 %cmpgttmp, label %then_218, label %merge_219

merge_219:                                        ; preds = %merge_221, %merge_217
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load12" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data13 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load12"
  %arr.get15 = load i64, ptr %arr.elem14, align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load16" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A92", align 8
  %addtmp17 = add i64 %arr.get15, %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\83\D8\AA\D9\84\D8\A9.load16"
  store i64 %addtmp17, ptr %"%lba_\D9\85\D8\B7\D9\84\D9\82", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load18" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load18"
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  store i64 %arr.get21, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  %"%\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"%\D9\82\D8\B1\D8\B5", align 8
  %"%lba_\D9\85\D8\B7\D9\84\D9\82.load" = load i64, ptr %"%lba_\D9\85\D8\B7\D9\84\D9\82", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load22" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B14", align 8
  %calltmp = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\83\D8\AA\D9\84\D8\A9"(i64 %"%\D9\82\D8\B1\D8\B5.load", i64 %"%lba_\D9\85\D8\B7\D9\84\D9\82.load", i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load22", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load")
  ret i64 %calltmp

merge_221:                                        ; preds = %then_218
  br label %merge_219

then_212:                                         ; preds = %entry
  ret i64 0

then_214:                                         ; preds = %merge_213
  ret i64 0

then_216:                                         ; preds = %merge_215
  ret i64 0

then_218:                                         ; preds = %merge_217
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load11" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\843", align 8
  %cmpletmp = icmp sle i64 %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\83\D8\AA\D9\84.load11", 0
  br i1 %cmpletmp, label %then_220, label %merge_221

then_220:                                         ; preds = %then_218
  ret i64 0
}

define i64 @"\D9\87\D9\8A\D8\A6_DMA"() {
entry:
  br i1 true, label %then_222, label %merge_223

merge_223:                                        ; preds = %entry
  br i1 true, label %then_224, label %merge_225

merge_225:                                        ; preds = %merge_223
  ret i64 0

then_222:                                         ; preds = %entry
  ret i64 -1

then_224:                                         ; preds = %merge_223
  ret i64 -1
}

define i64 @"\D8\A3\D8\B9\D8\AF_PRDT"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A", i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA") {
entry:
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA2" = alloca i64, align 8
  store i64 %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA", ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA2", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A1" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\81\D9\8A\D8\B2\D9\8A\D8\A7\D8\A6\D9\8A1", align 8
  br i1 true, label %then_226, label %merge_227

merge_227:                                        ; preds = %entry
  %"%890.load" = load i64, ptr %"\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A8\D8\A7\D9\8A\D8\AA\D8\A7\D8\AA2", align 8
  store i64 %"%890.load", ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9.load", 65536
  br i1 %cmpeqtmp, label %then_228, label %merge_229

merge_229:                                        ; preds = %then_228, %merge_227
  %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9.load3" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9", align 8
  %addtmp = add i64 %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9.load3", 2147483648
  call void @sad_ll_mem_write32(i64 4, i64 %addtmp)
  ret i64 0

then_226:                                         ; preds = %entry
  ret i64 -1

then_228:                                         ; preds = %merge_227
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D9\85\D8\B9_\D9\86\D9\87\D8\A7\D9\8A\D8\A9", align 8
  br label %merge_229
}

define i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_DMA"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\87\D8\AF\D9\81") {
entry:
  %"%\D8\AD\D8\A7\D9\84\D8\A9_dma" = alloca i64, align 8
  %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%drive_byte" = alloca i64, align 8
  %"%dma_\D9\85\D9\86\D9\81\D8\B0" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\87\D8\AF\D9\813" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\87\D8\AF\D9\81", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\87\D8\AF\D9\813", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  br i1 true, label %then_230, label %merge_231

merge_231:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 1
  br i1 %cmpnetmp, label %then_232, label %merge_233

merge_233:                                        ; preds = %merge_231
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5"
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  store i64 %arr.get8, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp10 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9")
  store i64 %calltmp10, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp12 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11")
  store i64 %calltmp12, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load13" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load13", 2
  br i1 %cmpgetmp, label %then_234, label %merge_235

merge_235:                                        ; preds = %then_234, %merge_233
  call void @sad_ll_interrupt_disable()
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load", 0
  call void @sad_ll_port_outb(i64 %addtmp, i64 6)
  call void @sad_ll_port_outb(i64 %addtmp, i64 8)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp14 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  %cmpnetmp15 = icmp ne i64 %calltmp14, 0
  br i1 %cmpnetmp15, label %then_236, label %merge_237

merge_237:                                        ; preds = %merge_235
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16"
  %arr.get19 = load i64, ptr %arr.elem18, align 8
  %addtmp20 = add i64 %arr.get19, 64
  store i64 %addtmp20, ptr %"%drive_byte", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load21" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load21", 24
  %andtmp = and i64 %shrtmp, 15
  %"%drive_byte.load" = load i64, ptr %"%drive_byte", align 8
  %addtmp22 = add i64 %"%drive_byte.load", %andtmp
  store i64 %addtmp22, ptr %"%drive_byte", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp23 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 6
  %"%drive_byte.load24" = load i64, ptr %"%drive_byte", align 8
  call void @sad_ll_port_outb(i64 %addtmp23, i64 %"%drive_byte.load24")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load25" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp26 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load25", 2
  call void @sad_ll_port_outb(i64 %addtmp26, i64 1)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load27" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp28 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load27", 3
  %"%\D8\B1\D9\82\D9\85_LBA.load29" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %andtmp30 = and i64 %"%\D8\B1\D9\82\D9\85_LBA.load29", 255
  call void @sad_ll_port_outb(i64 %addtmp28, i64 %andtmp30)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load31" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp32 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load31", 4
  %"%\D8\B1\D9\82\D9\85_LBA.load33" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp34 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load33", 8
  %andtmp35 = and i64 %shrtmp34, 255
  call void @sad_ll_port_outb(i64 %addtmp32, i64 %andtmp35)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load36" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp37 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load36", 5
  %"%\D8\B1\D9\82\D9\85_LBA.load38" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp39 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load38", 16
  %andtmp40 = and i64 %shrtmp39, 255
  call void @sad_ll_port_outb(i64 %addtmp37, i64 %andtmp40)
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load41" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp42 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load41", 0
  call void @sad_ll_port_outb(i64 %addtmp42, i64 9)
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84", align 8
  br label %while_cond_238

merge_242:                                        ; preds = %while_body_239
  %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load52" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_dma", align 8
  %shrtmp53 = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load52", 2
  %andtmp54 = and i64 %shrtmp53, 1
  %cmpeqtmp55 = icmp eq i64 %andtmp54, 1
  br i1 %cmpeqtmp55, label %then_243, label %merge_244

merge_244:                                        ; preds = %then_243, %merge_242
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load56" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp57 = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load56", 1
  store i64 %addtmp57, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_238

merge_246:                                        ; preds = %while_exit_240
  call void @sad_ll_interrupt_enable()
  ret i64 0

then_230:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\87\D8\AF\D9\81.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\87\D8\AF\D9\813", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_LBA.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\87\D8\AF\D9\81.load")
  ret i64 %calltmp

then_232:                                         ; preds = %merge_231
  ret i64 -1

then_234:                                         ; preds = %merge_233
  store i64 8, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  br label %merge_235

then_236:                                         ; preds = %merge_235
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_241:                                         ; preds = %while_body_239
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load50" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp51 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load50", 0
  call void @sad_ll_port_outb(i64 %addtmp51, i64 0)
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_243:                                         ; preds = %merge_242
  store i64 1, ptr %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84", align 8
  store i64 500000, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %merge_244

then_245:                                         ; preds = %while_exit_240
  call void @sad_ll_interrupt_enable()
  ret i64 -1

while_body_239:                                   ; preds = %while_cond_238
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load43" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp44 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load43", 0
  call void @sad_ll_port_inb(i64 %addtmp44)
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_dma", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_dma", align 8
  %shrtmp45 = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load", 1
  %andtmp46 = and i64 %shrtmp45, 1
  %cmpeqtmp = icmp eq i64 %andtmp46, 1
  br i1 %cmpeqtmp, label %then_241, label %merge_242

while_cond_238:                                   ; preds = %merge_244, %merge_237
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 500000
  br i1 %cmplttmp, label %while_body_239, label %while_exit_240

while_exit_240:                                   ; preds = %while_cond_238
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load47" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp48 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load47", 0
  call void @sad_ll_port_outb(i64 %addtmp48, i64 0)
  call void @sad_ll_port_outb(i64 %addtmp48, i64 6)
  %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84.load" = load i64, ptr %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84", align 8
  %cmpeqtmp49 = icmp eq i64 %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84.load", 0
  br i1 %cmpeqtmp49, label %then_245, label %merge_246
}

define i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_DMA"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", i64 %"\D8\B1\D9\82\D9\85_LBA", i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1") {
entry:
  %"%\D8\AD\D8\A7\D9\84\D8\A9_dma" = alloca i64, align 8
  %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%drive_byte" = alloca i64, align 8
  %"%dma_\D9\85\D9\86\D9\81\D8\B0" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1" = alloca i64, align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %"\D8\B1\D9\82\D9\85_LBA2" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_LBA", ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  br i1 true, label %then_247, label %merge_248

merge_248:                                        ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 1
  br i1 %cmpnetmp, label %then_249, label %merge_250

merge_250:                                        ; preds = %merge_248
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data6 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem7 = getelementptr i64, ptr %arr.data6, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load5"
  %arr.get8 = load i64, ptr %arr.elem7, align 8
  store i64 %arr.get8, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp10 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D9\88\D8\A7\D9\85\D8\B1_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load9")
  store i64 %calltmp10, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %calltmp12 = call i64 @"\D9\85\D9\86\D9\81\D8\B0_\D8\AA\D8\AD\D9\83\D9\85_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load11")
  store i64 %calltmp12, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\AA\D8\AD\D9\83\D9\85", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load13" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %cmpgetmp = icmp sge i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load13", 2
  br i1 %cmpgetmp, label %then_251, label %merge_252

merge_252:                                        ; preds = %then_251, %merge_250
  call void @sad_ll_interrupt_disable()
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load", 0
  call void @sad_ll_port_outb(i64 %addtmp, i64 6)
  call void @sad_ll_port_outb(i64 %addtmp, i64 0)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1", align 8
  %calltmp14 = call i64 @"\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D8\BA\D9\8A\D8\B1_\D9\85\D8\B4\D8\BA\D9\88\D9\84"(i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A3\D9\88\D8\A7\D9\85\D8\B1.load")
  %cmpnetmp15 = icmp ne i64 %calltmp14, 0
  br i1 %cmpnetmp15, label %then_253, label %merge_254

merge_254:                                        ; preds = %merge_252
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %arr.data17 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem18 = getelementptr i64, ptr %arr.data17, i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load16"
  %arr.get19 = load i64, ptr %arr.elem18, align 8
  %addtmp20 = add i64 %arr.get19, 64
  store i64 %addtmp20, ptr %"%drive_byte", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load21" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load21", 24
  %andtmp = and i64 %shrtmp, 15
  %"%drive_byte.load" = load i64, ptr %"%drive_byte", align 8
  %addtmp22 = add i64 %"%drive_byte.load", %andtmp
  store i64 %addtmp22, ptr %"%drive_byte", align 8
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp23 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load", 6
  %"%drive_byte.load24" = load i64, ptr %"%drive_byte", align 8
  call void @sad_ll_port_outb(i64 %addtmp23, i64 %"%drive_byte.load24")
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load25" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp26 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load25", 2
  call void @sad_ll_port_outb(i64 %addtmp26, i64 1)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load27" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp28 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load27", 3
  %"%\D8\B1\D9\82\D9\85_LBA.load29" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %andtmp30 = and i64 %"%\D8\B1\D9\82\D9\85_LBA.load29", 255
  call void @sad_ll_port_outb(i64 %addtmp28, i64 %andtmp30)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load31" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp32 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load31", 4
  %"%\D8\B1\D9\82\D9\85_LBA.load33" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp34 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load33", 8
  %andtmp35 = and i64 %shrtmp34, 255
  call void @sad_ll_port_outb(i64 %addtmp32, i64 %andtmp35)
  %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load36" = load i64, ptr %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
  %addtmp37 = add i64 %"%\D9\85\D9\86\D9\81\D8\B0_\D8\A7\D9\84\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA.load36", 5
  %"%\D8\B1\D9\82\D9\85_LBA.load38" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %shrtmp39 = lshr i64 %"%\D8\B1\D9\82\D9\85_LBA.load38", 16
  %andtmp40 = and i64 %shrtmp39, 255
  call void @sad_ll_port_outb(i64 %addtmp37, i64 %andtmp40)
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load41" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp42 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load41", 0
  call void @sad_ll_port_outb(i64 %addtmp42, i64 1)
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84", align 8
  br label %while_cond_255

merge_259:                                        ; preds = %while_body_256
  %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load52" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_dma", align 8
  %shrtmp53 = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load52", 2
  %andtmp54 = and i64 %shrtmp53, 1
  %cmpeqtmp55 = icmp eq i64 %andtmp54, 1
  br i1 %cmpeqtmp55, label %then_260, label %merge_261

merge_261:                                        ; preds = %then_260, %merge_259
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load56" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp57 = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load56", 1
  store i64 %addtmp57, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_255

merge_263:                                        ; preds = %while_exit_257
  call void @sad_ll_interrupt_enable()
  ret i64 0

then_247:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B51", align 8
  %"%\D8\B1\D9\82\D9\85_LBA.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_LBA2", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B13", align 8
  %calltmp = call i64 @"\D8\A7\D9\83\D8\AA\D8\A8_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D9\82\D8\B1\D8\B5.load", i64 %"%\D8\B1\D9\82\D9\85_LBA.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\85\D8\B5\D8\AF\D8\B1.load")
  ret i64 %calltmp

then_249:                                         ; preds = %merge_248
  ret i64 -1

then_251:                                         ; preds = %merge_250
  store i64 8, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  br label %merge_252

then_253:                                         ; preds = %merge_252
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_258:                                         ; preds = %while_body_256
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load50" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp51 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load50", 0
  call void @sad_ll_port_outb(i64 %addtmp51, i64 0)
  call void @sad_ll_interrupt_enable()
  ret i64 -1

then_260:                                         ; preds = %merge_259
  store i64 1, ptr %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84", align 8
  store i64 500000, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %merge_261

then_262:                                         ; preds = %while_exit_257
  call void @sad_ll_interrupt_enable()
  ret i64 -1

while_body_256:                                   ; preds = %while_cond_255
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load43" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp44 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load43", 0
  call void @sad_ll_port_inb(i64 %addtmp44)
  store i64 0, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_dma", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_dma", align 8
  %shrtmp45 = lshr i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_dma.load", 1
  %andtmp46 = and i64 %shrtmp45, 1
  %cmpeqtmp = icmp eq i64 %andtmp46, 1
  br i1 %cmpeqtmp, label %then_258, label %merge_259

while_cond_255:                                   ; preds = %merge_261, %merge_254
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 500000
  br i1 %cmplttmp, label %while_body_256, label %while_exit_257

while_exit_257:                                   ; preds = %while_cond_255
  %"%dma_\D9\85\D9\86\D9\81\D8\B0.load47" = load i64, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0", align 8
  %addtmp48 = add i64 %"%dma_\D9\85\D9\86\D9\81\D8\B0.load47", 0
  call void @sad_ll_port_outb(i64 %addtmp48, i64 0)
  call void @sad_ll_port_outb(i64 %addtmp48, i64 6)
  %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84.load" = load i64, ptr %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84", align 8
  %cmpeqtmp49 = icmp eq i64 %"%\D8\A7\D9\83\D8\AA\D9\85\D9\84.load", 0
  br i1 %cmpeqtmp49, label %then_262, label %merge_263
}

define i64 @"ahci_\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D9\86\D9\81\D8\B0"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", i64 %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9") {
entry:
  %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92" = alloca i64, align 8
  store i64 %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", ptr %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92", align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %shltmp = shl i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 7
  %addtmp = add i64 256, %shltmp
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A92", align 8
  %addtmp3 = add i64 %addtmp, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  ret i64 %addtmp3
}

define i64 @"ahci_\D9\81\D8\AD\D8\B5_\D9\85\D9\86\D9\81\D8\B0"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9" = alloca i64, align 8
  %"%ipm" = alloca i64, align 8
  %"%det" = alloca i64, align 8
  %"%ssts" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%ssts.load" = load i64, ptr %"%ssts", align 8
  %andtmp = and i64 %"%ssts.load", 15
  store i64 %andtmp, ptr %"%det", align 8
  %"%ssts.load2" = load i64, ptr %"%ssts", align 8
  %shrtmp = lshr i64 %"%ssts.load2", 8
  %andtmp3 = and i64 %shrtmp, 15
  store i64 %andtmp3, ptr %"%ipm", align 8
  %"%det.load" = load i64, ptr %"%det", align 8
  %cmpnetmp = icmp ne i64 %"%det.load", 3
  br i1 %cmpnetmp, label %then_264, label %merge_265

merge_265:                                        ; preds = %entry
  %"%ipm.load" = load i64, ptr %"%ipm", align 8
  %cmpnetmp4 = icmp ne i64 %"%ipm.load", 1
  br i1 %cmpnetmp4, label %then_266, label %merge_267

merge_267:                                        ; preds = %merge_265
  %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load" = load i64, ptr %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AA\D9\88\D9\82\D9\8A\D8\B9.load", 257
  br i1 %cmpeqtmp, label %then_268, label %merge_269

merge_269:                                        ; preds = %merge_267
  ret i64 0

then_264:                                         ; preds = %entry
  ret i64 0

then_266:                                         ; preds = %merge_265
  ret i64 0

then_268:                                         ; preds = %merge_267
  ret i64 1
}

define i64 @"\D9\87\D9\8A\D8\A6_AHCI"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A9") {
entry:
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0" = alloca i64, align 8
  %"%\D8\AC" = alloca i64, align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D9\81\D8\B0" = alloca i64, align 8
  %"%cap" = alloca i64, align 8
  %"%pi" = alloca i64, align 8
  %"%ghc" = alloca i64, align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\A7\D8\B9\D8\AF\D8\A91", align 8
  br i1 true, label %then_270, label %merge_271

merge_271:                                        ; preds = %entry
  call void @sad_ll_mem_read32(i64 0)
  store i64 0, ptr %"%ghc", align 8
  %"%ghc.load" = load i64, ptr %"%ghc", align 8
  %addtmp = add i64 %"%ghc.load", 2147483648
  store i64 %addtmp, ptr %"%ghc", align 8
  %"%ghc.load2" = load i64, ptr %"%ghc", align 8
  call void @sad_ll_mem_write32(i64 0, i64 %"%ghc.load2")
  call void @sad_ll_mem_read32(i64 0)
  store i64 0, ptr %"%pi", align 8
  call void @sad_ll_mem_read32(i64 0)
  store i64 0, ptr %"%cap", align 8
  %"%cap.load" = load i64, ptr %"%cap", align 8
  %andtmp = and i64 %"%cap.load", 31
  %addtmp3 = add i64 %andtmp, 1
  store i64 %addtmp3, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D9\81\D8\B0", align 8
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_272

merge_276:                                        ; preds = %merge_281, %while_body_273
  %"%\D9\85.load6" = load i64, ptr %"%\D9\85", align 8
  %addtmp7 = add i64 %"%\D9\85.load6", 1
  store i64 %addtmp7, ptr %"%\D9\85", align 8
  br label %while_cond_272

merge_281:                                        ; preds = %merge_283, %while_exit_279
  br label %merge_276

merge_283:                                        ; preds = %then_282, %then_280
  br label %merge_281

then_270:                                         ; preds = %entry
  ret i64 -1

then_275:                                         ; preds = %while_body_273
  store i64 1, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  store i64 0, ptr %"%\D8\AC", align 8
  br label %while_cond_277

then_280:                                         ; preds = %while_exit_279
  %"%\D9\85.load14" = load i64, ptr %"%\D9\85", align 8
  %calltmp = call i64 @"ahci_\D9\81\D8\AD\D8\B5_\D9\85\D9\86\D9\81\D8\B0"(i64 %"%\D9\85.load14")
  store i64 %calltmp, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 1
  br i1 %cmpeqtmp15, label %then_282, label %merge_283

then_282:                                         ; preds = %then_280
  %"%\D9\85.load16" = load i64, ptr %"%\D9\85", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\85.load16"
  store i64 1, ptr %arr.elem, align 8
  %"%\D9\85.load17" = load i64, ptr %"%\D9\85", align 8
  %arr.data18 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem19 = getelementptr i64, ptr %arr.data18, i64 %"%\D9\85.load17"
  store i64 1, ptr %arr.elem19, align 8
  br label %merge_283

while_body_273:                                   ; preds = %while_cond_272
  %"%\D9\85.load4" = load i64, ptr %"%\D9\85", align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D9\81\D8\B0.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D9\81\D8\B0", align 8
  %cmplttmp5 = icmp slt i64 %"%\D9\85.load4", %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D9\81\D8\B0.load"
  br i1 %cmplttmp5, label %then_275, label %merge_276

while_body_278:                                   ; preds = %while_cond_277
  %"%\D9\82\D9\86\D8\A7\D8\B9.load" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %shltmp = shl i64 %"%\D9\82\D9\86\D8\A7\D8\B9.load", 1
  store i64 %shltmp, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %"%\D8\AC.load10" = load i64, ptr %"%\D8\AC", align 8
  %addtmp11 = add i64 %"%\D8\AC.load10", 1
  store i64 %addtmp11, ptr %"%\D8\AC", align 8
  br label %while_cond_277

while_cond_272:                                   ; preds = %merge_276, %merge_271
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp = icmp slt i64 %"%\D9\85.load", 16
  br i1 %cmplttmp, label %while_body_273, label %while_exit_274

while_cond_277:                                   ; preds = %while_body_278, %then_275
  %"%\D8\AC.load" = load i64, ptr %"%\D8\AC", align 8
  %"%\D9\85.load8" = load i64, ptr %"%\D9\85", align 8
  %cmplttmp9 = icmp slt i64 %"%\D8\AC.load", %"%\D9\85.load8"
  br i1 %cmplttmp9, label %while_body_278, label %while_exit_279

while_exit_274:                                   ; preds = %while_cond_272
  ret i64 0

while_exit_279:                                   ; preds = %while_cond_277
  %"%pi.load" = load i64, ptr %"%pi", align 8
  %"%\D9\82\D9\86\D8\A7\D8\B9.load12" = load i64, ptr %"%\D9\82\D9\86\D8\A7\D8\B9", align 8
  %divtmp = sdiv i64 %"%pi.load", %"%\D9\82\D9\86\D8\A7\D8\B9.load12"
  %andtmp13 = and i64 %divtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp13, 1
  br i1 %cmpeqtmp, label %then_280, label %merge_281
}

define i64 @"\D9\87\D9\8A\D8\A6_\D9\85\D9\86\D9\81\D8\B0_AHCI"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0") {
entry:
  %"%fis" = alloca i64, align 8
  %"%cmd_list" = alloca i64, align 8
  %"%fr" = alloca i64, align 8
  %"%cr" = alloca i64, align 8
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%cmd" = alloca i64, align 8
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 0
  br i1 %cmplttmp, label %then_284, label %merge_285

merge_285:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load2", 16
  br i1 %cmpgetmp, label %then_286, label %merge_287

merge_287:                                        ; preds = %merge_285
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 0
  br i1 %cmpeqtmp, label %then_288, label %merge_289

merge_289:                                        ; preds = %merge_287
  %"%cmd.load" = load i64, ptr %"%cmd", align 8
  %andtmp = and i64 %"%cmd.load", 1
  %cmpeqtmp4 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp4, label %then_290, label %merge_291

merge_291:                                        ; preds = %then_290, %merge_289
  %"%cmd.load6" = load i64, ptr %"%cmd", align 8
  %shrtmp = lshr i64 %"%cmd.load6", 4
  %andtmp7 = and i64 %shrtmp, 1
  %cmpeqtmp8 = icmp eq i64 %andtmp7, 1
  br i1 %cmpeqtmp8, label %then_292, label %merge_293

merge_293:                                        ; preds = %then_292, %merge_291
  store i64 0, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_294

merge_298:                                        ; preds = %merge_300, %while_body_295
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load21" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %addtmp = add i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load21", 1
  store i64 %addtmp, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %while_cond_294

merge_300:                                        ; preds = %then_299, %then_297
  br label %merge_298

merge_302:                                        ; preds = %while_exit_296
  %"%cmd_list.load22" = load i64, ptr %"%cmd_list", align 8
  %0 = inttoptr i64 %"%cmd_list.load22" to ptr
  call void @llvm.memset.p0.i64(ptr align 1 %0, i8 0, i64 4096, i1 false)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load23" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%cmd_list.load24" = load i64, ptr %"%cmd_list", align 8
  %arr.data25 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load23"
  store i64 %"%cmd_list.load24", ptr %arr.elem26, align 8
  call void @sad_ll_page_alloc()
  store i64 0, ptr %"%fis", align 8
  %"%fis.load" = load i64, ptr %"%fis", align 8
  %cmpeqtmp27 = icmp eq i64 %"%fis.load", 0
  br i1 %cmpeqtmp27, label %then_303, label %merge_304

merge_304:                                        ; preds = %merge_302
  %"%fis.load28" = load i64, ptr %"%fis", align 8
  %1 = inttoptr i64 %"%fis.load28" to ptr
  call void @llvm.memset.p0.i64(ptr align 1 %1, i8 0, i64 4096, i1 false)
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load29" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B01", align 8
  %"%fis.load30" = load i64, ptr %"%fis", align 8
  %arr.data31 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load29"
  store i64 %"%fis.load30", ptr %arr.elem32, align 8
  %"%cmd.load33" = load i64, ptr %"%cmd", align 8
  %addtmp34 = add i64 %"%cmd.load33", 16
  store i64 %addtmp34, ptr %"%cmd", align 8
  %"%cmd.load35" = load i64, ptr %"%cmd", align 8
  %addtmp36 = add i64 %"%cmd.load35", 1
  store i64 %addtmp36, ptr %"%cmd", align 8
  ret i64 0

then_284:                                         ; preds = %entry
  ret i64 -1

then_286:                                         ; preds = %merge_285
  ret i64 -1

then_288:                                         ; preds = %merge_287
  ret i64 -1

then_290:                                         ; preds = %merge_289
  %"%cmd.load5" = load i64, ptr %"%cmd", align 8
  %subtmp = sub i64 %"%cmd.load5", 1
  store i64 %subtmp, ptr %"%cmd", align 8
  br label %merge_291

then_292:                                         ; preds = %merge_291
  %"%cmd.load9" = load i64, ptr %"%cmd", align 8
  %subtmp10 = sub i64 %"%cmd.load9", 16
  store i64 %subtmp10, ptr %"%cmd", align 8
  br label %merge_293

then_297:                                         ; preds = %while_body_295
  %"%fr.load" = load i64, ptr %"%fr", align 8
  %cmpeqtmp20 = icmp eq i64 %"%fr.load", 0
  br i1 %cmpeqtmp20, label %then_299, label %merge_300

then_299:                                         ; preds = %then_297
  store i64 100000, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  br label %merge_300

then_301:                                         ; preds = %while_exit_296
  ret i64 -1

then_303:                                         ; preds = %merge_302
  ret i64 -1

while_body_295:                                   ; preds = %while_cond_294
  %"%cmd.load12" = load i64, ptr %"%cmd", align 8
  %shrtmp13 = lshr i64 %"%cmd.load12", 15
  %andtmp14 = and i64 %shrtmp13, 1
  store i64 %andtmp14, ptr %"%cr", align 8
  %"%cmd.load15" = load i64, ptr %"%cmd", align 8
  %shrtmp16 = lshr i64 %"%cmd.load15", 14
  %andtmp17 = and i64 %shrtmp16, 1
  store i64 %andtmp17, ptr %"%fr", align 8
  %"%cr.load" = load i64, ptr %"%cr", align 8
  %cmpeqtmp18 = icmp eq i64 %"%cr.load", 0
  br i1 %cmpeqtmp18, label %then_297, label %merge_298

while_cond_294:                                   ; preds = %merge_298, %merge_293
  %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load" = load i64, ptr %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  %cmplttmp11 = icmp slt i64 %"%\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA.load", 100000
  br i1 %cmplttmp11, label %while_body_295, label %while_exit_296

while_exit_296:                                   ; preds = %while_cond_294
  call void @sad_ll_page_alloc()
  store i64 0, ptr %"%cmd_list", align 8
  %"%cmd_list.load" = load i64, ptr %"%cmd_list", align 8
  %cmpeqtmp19 = icmp eq i64 %"%cmd_list.load", 0
  br i1 %cmpeqtmp19, label %then_301, label %merge_302
}

define void @"\D9\87\D9\8A\D8\A6_\D9\86\D8\B8\D8\A7\D9\85_\D8\A7\D9\84\D9\82\D8\B1\D8\B5"() {
entry:
  %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 1)
  call void @"\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D8\AA\D8\B9\D9\8A\D9\8A\D9\86_ATA"(i64 0)
  call void @"\D8\A5\D8\B9\D8\A7\D8\AF\D8\A9_\D8\AA\D8\B9\D9\8A\D9\8A\D9\86_ATA"(i64 1)
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_305

merge_309:                                        ; preds = %then_308, %while_body_306
  %"%\D9\82.load5" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D9\82.load5", 1
  store i64 %addtmp, ptr %"%\D9\82", align 8
  br label %while_cond_305

merge_314:                                        ; preds = %merge_316, %while_body_311
  %"%\D9\82.load15" = load i64, ptr %"%\D9\82", align 8
  %addtmp16 = add i64 %"%\D9\82.load15", 1
  store i64 %addtmp16, ptr %"%\D9\82", align 8
  br label %while_cond_310

merge_316:                                        ; preds = %then_315, %then_313
  br label %merge_314

then_308:                                         ; preds = %while_body_306
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 1)
  %"%\D9\82.load3" = load i64, ptr %"%\D9\82", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\82.load3")
  %"%\D9\82.load4" = load i64, ptr %"%\D9\82", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\82.load4"
  %arr.get = load i64, ptr %arr.elem, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get)
  br label %merge_309

then_313:                                         ; preds = %while_body_311
  %"%\D9\82.load13" = load i64, ptr %"%\D9\82", align 8
  %calltmp14 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\AC\D8\AF\D9\88\D9\84_\D8\A3\D9\82\D8\B3\D8\A7\D9\85"(i64 %"%\D9\82.load13")
  store i64 %calltmp14, ptr %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85", align 8
  %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85.load" = load i64, ptr %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85.load", 0
  br i1 %cmpgttmp, label %then_315, label %merge_316

then_315:                                         ; preds = %then_313
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 1)
  %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85.load17" = load i64, ptr %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D8\A3\D9\82\D8\B3\D8\A7\D9\85.load17")
  br label %merge_316

while_body_306:                                   ; preds = %while_cond_305
  %"%\D9\82.load1" = load i64, ptr %"%\D9\82", align 8
  %calltmp = call i64 @"\D8\AA\D8\B9\D8\B1\D9\81_\D8\B9\D9\84\D9\89_\D9\82\D8\B1\D8\B5"(i64 %"%\D9\82.load1")
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 1
  br i1 %cmpeqtmp, label %then_308, label %merge_309

while_body_311:                                   ; preds = %while_cond_310
  %"%\D9\82.load8" = load i64, ptr %"%\D9\82", align 8
  %arr.data9 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem10 = getelementptr i64, ptr %arr.data9, i64 %"%\D9\82.load8"
  %arr.get11 = load i64, ptr %arr.elem10, align 8
  %cmpeqtmp12 = icmp eq i64 %arr.get11, 1
  br i1 %cmpeqtmp12, label %then_313, label %merge_314

while_cond_305:                                   ; preds = %merge_309, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp, label %while_body_306, label %while_exit_307

while_cond_310:                                   ; preds = %merge_314, %while_exit_307
  %"%\D9\82.load6" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp7 = icmp slt i64 %"%\D9\82.load6", 0
  br i1 %cmplttmp7, label %while_body_311, label %while_exit_312

while_exit_307:                                   ; preds = %while_cond_305
  %calltmp2 = call i64 @"\D9\87\D9\8A\D8\A6_DMA"()
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_310

while_exit_312:                                   ; preds = %while_cond_310
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 1)
  ret void
}

define i64 @"\D8\A7\D8\A8\D8\AD\D8\AB_\D8\B9\D9\86_\D9\82\D8\B3\D9\85"(i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A8") {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A81" = alloca i64, align 8
  store i64 %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A8", ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_317

merge_321:                                        ; preds = %merge_323, %while_body_318
  %"%\D9\81.load8" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load8", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_317

merge_323:                                        ; preds = %then_320
  br label %merge_321

then_320:                                         ; preds = %while_body_318
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\81.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A8.load" = load i64, ptr %"\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A81", align 8
  %cmpeqtmp7 = icmp eq i64 %arr.get6, %"%\D9\86\D9\88\D8\B9_\D8\A7\D9\84\D9\85\D8\B7\D9\84\D9\88\D8\A8.load"
  br i1 %cmpeqtmp7, label %then_322, label %merge_323

then_322:                                         ; preds = %then_320
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_318:                                   ; preds = %while_cond_317
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load2"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 1
  br i1 %cmpeqtmp, label %then_320, label %merge_321

while_cond_317:                                   ; preds = %merge_321, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_318, label %while_exit_319

while_exit_319:                                   ; preds = %while_cond_317
  ret i64 -1
}

define i64 @"\D8\A7\D8\A8\D8\AD\D8\AB_\D8\B9\D9\86_\D9\82\D8\B3\D9\85_\D9\86\D8\B4\D8\B7"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_324

merge_328:                                        ; preds = %merge_330, %while_body_325
  %"%\D9\81.load7" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D9\81.load7", 1
  store i64 %addtmp, ptr %"%\D9\81", align 8
  br label %while_cond_324

merge_330:                                        ; preds = %then_327
  br label %merge_328

then_327:                                         ; preds = %while_body_325
  %"%\D9\81.load2" = load i64, ptr %"%\D9\81", align 8
  %arr.data3 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem4 = getelementptr i64, ptr %arr.data3, i64 %"%\D9\81.load2"
  %arr.get5 = load i64, ptr %arr.elem4, align 8
  %cmpeqtmp6 = icmp eq i64 %arr.get5, 1
  br i1 %cmpeqtmp6, label %then_329, label %merge_330

then_329:                                         ; preds = %then_327
  %loadtmp = load i64, ptr %"%\D9\81", align 8
  ret i64 %loadtmp

while_body_325:                                   ; preds = %while_cond_324
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\81.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpeqtmp = icmp eq i64 %arr.get, 1
  br i1 %cmpeqtmp, label %then_327, label %merge_328

while_cond_324:                                   ; preds = %merge_328, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp, label %while_body_325, label %while_exit_326

while_exit_326:                                   ; preds = %while_cond_324
  ret i64 -1
}

define i64 @"\D8\AD\D8\AC\D9\85_\D9\82\D8\B3\D9\85_MB"(i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85") {
entry:
  %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851" = alloca i64, align 8
  store i64 %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85", ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load", 0
  br i1 %cmplttmp, label %then_331, label %merge_332

merge_332:                                        ; preds = %entry
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load2" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load2", 0
  br i1 %cmpgetmp, label %then_333, label %merge_334

merge_334:                                        ; preds = %merge_332
  %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load3" = load i64, ptr %"\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\851", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B1\D9\82\D9\85_\D8\A7\D9\84\D9\82\D8\B3\D9\85.load3"
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get

then_331:                                         ; preds = %entry
  ret i64 0

then_333:                                         ; preds = %merge_332
  ret i64 0
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\A7\D9\84\D9\82\D8\B1\D8\B5"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93" = alloca i64, align 8
  %"%sig" = alloca i64, align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91" = alloca i64, align 8
  %"%\D8\A8" = alloca i64, align 8
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 1)
  br i1 true, label %then_335, label %merge_336

else_346:                                         ; preds = %while_exit_343
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_345

else_349:                                         ; preds = %merge_345
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_348

else_352:                                         ; preds = %merge_348
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_351

else_355:                                         ; preds = %merge_351
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  br label %merge_354

merge_336:                                        ; preds = %entry
  call void @sad_ll_page_alloc()
  store i64 0, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  call void @sad_ll_page_alloc()
  store i64 0, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_337, label %merge_338

merge_338:                                        ; preds = %merge_336
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %cmpeqtmp1 = icmp eq i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 0
  br i1 %cmpeqtmp1, label %then_339, label %merge_340

merge_340:                                        ; preds = %merge_338
  store i64 0, ptr %"%\D8\A8", align 8
  br label %while_cond_341

merge_345:                                        ; preds = %else_346, %then_344
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load10" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %addtmp11 = add i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load10", 510
  call void @sad_ll_mem_read16(i64 %addtmp11)
  store i64 0, ptr %"%sig", align 8
  %"%sig.load" = load i64, ptr %"%sig", align 8
  %cmpeqtmp12 = icmp eq i64 %"%sig.load", 21930
  br i1 %cmpeqtmp12, label %then_347, label %else_349

merge_348:                                        ; preds = %else_349, %then_347
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load13" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %calltmp14 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_PIO"(i64 0, i64 1, i64 2, i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load13")
  store i64 %calltmp14, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93", align 8
  %cmpeqtmp15 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A93.load", 2
  br i1 %cmpeqtmp15, label %then_350, label %else_352

merge_351:                                        ; preds = %else_352, %then_350
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load16" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %calltmp17 = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\83\D8\AA\D9\84\D8\A9"(i64 0, i64 0, i64 1, i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load16")
  store i64 %calltmp17, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94", align 8
  %cmpeqtmp18 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A94.load", 1
  br i1 %cmpeqtmp18, label %then_353, label %else_355

merge_354:                                        ; preds = %else_355, %then_353
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 1)
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load19" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  call void @sad_ll_page_free(i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load19")
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load20" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  call void @sad_ll_page_free(i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load20")
  ret void

then_335:                                         ; preds = %entry
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 3)
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 0)
  ret void

then_337:                                         ; preds = %merge_336
  ret void

then_339:                                         ; preds = %merge_338
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load2" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  call void @sad_ll_page_free(i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load2")
  ret void

then_344:                                         ; preds = %while_exit_343
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_345

then_347:                                         ; preds = %merge_345
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_348

then_350:                                         ; preds = %merge_348
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_351

then_353:                                         ; preds = %merge_351
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 1)
  br label %merge_354

while_body_342:                                   ; preds = %while_cond_341
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load3" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %"%\D8\A8.load4" = load i64, ptr %"%\D8\A8", align 8
  %addtmp = add i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load3", %"%\D8\A8.load4"
  %"%\D8\A8.load5" = load i64, ptr %"%\D8\A8", align 8
  %shrtmp = lshr i64 %"%\D8\A8.load5", 1
  %andtmp = and i64 %shrtmp, 255
  call void @sad_ll_mem_write16(i64 %addtmp, i64 %andtmp)
  %"%\D8\A8.load6" = load i64, ptr %"%\D8\A8", align 8
  %addtmp7 = add i64 %"%\D8\A8.load6", 2
  store i64 %addtmp7, ptr %"%\D8\A8", align 8
  br label %while_cond_341

while_cond_341:                                   ; preds = %while_body_342, %merge_340
  %"%\D8\A8.load" = load i64, ptr %"%\D8\A8", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A8.load", 512
  br i1 %cmplttmp, label %while_body_342, label %while_exit_343

while_exit_343:                                   ; preds = %while_cond_341
  %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load8" = load i64, ptr %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %calltmp = call i64 @"\D8\A7\D9\82\D8\B1\D8\A3_\D9\82\D8\B7\D8\A7\D8\B9_PIO"(i64 0, i64 0, i64 %"%\D9\85\D8\AE\D8\B2\D9\86_\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load8")
  store i64 %calltmp, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91", align 8
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91", align 8
  %cmpeqtmp9 = icmp eq i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A91.load", 0
  br i1 %cmpeqtmp9, label %then_344, label %else_346
}

define void @"\D8\A7\D8\B7\D8\A8\D8\B9_\D8\A5\D8\AD\D8\B5\D8\A7\D8\A6\D9\8A\D8\A7\D8\AA_\D8\A7\D9\84\D9\82\D8\B1\D8\B5"() {
entry:
  %"%\D9\81" = alloca i64, align 8
  %"%\D9\82" = alloca i64, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 1)
  store i64 0, ptr %"%\D9\82", align 8
  br label %while_cond_356

merge_360:                                        ; preds = %then_359, %while_body_357
  %"%\D9\82.load11" = load i64, ptr %"%\D9\82", align 8
  %addtmp = add i64 %"%\D9\82.load11", 1
  store i64 %addtmp, ptr %"%\D9\82", align 8
  br label %while_cond_356

merge_365:                                        ; preds = %merge_367, %while_body_362
  %"%\D9\81.load31" = load i64, ptr %"%\D9\81", align 8
  %addtmp32 = add i64 %"%\D9\81.load31", 1
  store i64 %addtmp32, ptr %"%\D9\81", align 8
  br label %while_cond_361

merge_367:                                        ; preds = %then_366, %then_364
  br label %merge_365

merge_369:                                        ; preds = %then_368, %while_exit_363
  ret void

then_359:                                         ; preds = %while_body_357
  %"%\D9\82.load2" = load i64, ptr %"%\D9\82", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\82.load2")
  %"%\D9\82.load3" = load i64, ptr %"%\D9\82", align 8
  %arr.data4 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 %"%\D9\82.load3"
  %arr.get6 = load i64, ptr %arr.elem5, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get6)
  %"%\D9\82.load7" = load i64, ptr %"%\D9\82", align 8
  %arr.data8 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem9 = getelementptr i64, ptr %arr.data8, i64 %"%\D9\82.load7"
  %arr.get10 = load i64, ptr %arr.elem9, align 8
  %shrtmp = lshr i64 %arr.get10, 11
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %shrtmp)
  br label %merge_360

then_364:                                         ; preds = %while_body_362
  %"%\D9\81.load17" = load i64, ptr %"%\D9\81", align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\81.load17")
  %"%\D9\81.load18" = load i64, ptr %"%\D9\81", align 8
  %arr.data19 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 %"%\D9\81.load18"
  %arr.get21 = load i64, ptr %arr.elem20, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get21)
  %"%\D9\81.load22" = load i64, ptr %"%\D9\81", align 8
  %arr.data23 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem24 = getelementptr i64, ptr %arr.data23, i64 %"%\D9\81.load22"
  %arr.get25 = load i64, ptr %arr.elem24, align 8
  call void @"serial_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %arr.get25)
  %"%\D9\81.load26" = load i64, ptr %"%\D9\81", align 8
  %arr.data27 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 %"%\D9\81.load26"
  %arr.get29 = load i64, ptr %arr.elem28, align 8
  %cmpeqtmp30 = icmp eq i64 %arr.get29, 1
  br i1 %cmpeqtmp30, label %then_366, label %merge_367

then_366:                                         ; preds = %then_364
  br label %merge_367

then_368:                                         ; preds = %while_exit_363
  br label %merge_369

while_body_357:                                   ; preds = %while_cond_356
  %"%\D9\82.load1" = load i64, ptr %"%\D9\82", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D9\82.load1"
  %arr.get = load i64, ptr %arr.elem, align 8
  %cmpnetmp = icmp ne i64 %arr.get, 0
  br i1 %cmpnetmp, label %then_359, label %merge_360

while_body_362:                                   ; preds = %while_cond_361
  %"%\D9\81.load13" = load i64, ptr %"%\D9\81", align 8
  %arr.data14 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem15 = getelementptr i64, ptr %arr.data14, i64 %"%\D9\81.load13"
  %arr.get16 = load i64, ptr %arr.elem15, align 8
  %cmpeqtmp = icmp eq i64 %arr.get16, 1
  br i1 %cmpeqtmp, label %then_364, label %merge_365

while_cond_356:                                   ; preds = %merge_360, %entry
  %"%\D9\82.load" = load i64, ptr %"%\D9\82", align 8
  %cmplttmp = icmp slt i64 %"%\D9\82.load", 0
  br i1 %cmplttmp, label %while_body_357, label %while_exit_358

while_cond_361:                                   ; preds = %merge_365, %while_exit_358
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp12 = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp12, label %while_body_362, label %while_exit_363

while_exit_358:                                   ; preds = %while_cond_356
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_361

while_exit_363:                                   ; preds = %while_cond_361
  br i1 false, label %then_368, label %merge_369
}

define void @__sad_main() {
entry:
  %"%AHCI_PxCI" = alloca i64, align 8
  %"%AHCI_PxSACT" = alloca i64, align 8
  %"%AHCI_PxSERR" = alloca i64, align 8
  %"%AHCI_PxSCTL" = alloca i64, align 8
  %"%AHCI_PxSSTS" = alloca i64, align 8
  %"%AHCI_PxSIG" = alloca i64, align 8
  %"%AHCI_PxTFD" = alloca i64, align 8
  %"%AHCI_PxCMD" = alloca i64, align 8
  %"%AHCI_PxIE" = alloca i64, align 8
  %"%AHCI_PxIS" = alloca i64, align 8
  %"%AHCI_PxFBU" = alloca i64, align 8
  %"%AHCI_PxFB" = alloca i64, align 8
  %"%AHCI_PxCLBU" = alloca i64, align 8
  %"%AHCI_PxCLB" = alloca i64, align 8
  %"%AHCI_VS" = alloca i64, align 8
  %"%AHCI_PI" = alloca i64, align 8
  %"%AHCI_IS" = alloca i64, align 8
  %"%AHCI_GHC" = alloca i64, align 8
  %"%AHCI_CAP" = alloca i64, align 8
  %"%ahci_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D9\81\D8\B0" = alloca i64, align 8
  %"%ahci_fis_addr" = alloca i64, align 8
  %"%ahci_cmd_list" = alloca i64, align 8
  %"%ahci_\D9\85\D9\86\D9\81\D8\B0_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%ahci_\D9\85\D9\86\D9\81\D8\B0_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%ahci_\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_DMA_\D9\85\D8\AE\D8\B2\D9\86" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_PRDT" = alloca i64, align 8
  %"%DMA_PRDT" = alloca i64, align 8
  %"%DMA_STAT" = alloca i64, align 8
  %"%DMA_CMD" = alloca i64, align 8
  %"%dma_\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AE\D8\B2\D9\86_MBR" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B3\D8\A7\D9\85" = alloca i64, align 8
  %"%\D9\82\D8\B3\D9\85_\D9\82\D8\B1\D8\B5" = alloca i64, align 8
  %"%\D9\82\D8\B3\D9\85_\D8\AD\D8\AC\D9\85_MB" = alloca i64, align 8
  %"%\D9\82\D8\B3\D9\85_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\82\D8\B3\D9\85_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_LBA" = alloca i64, align 8
  %"%\D9\82\D8\B3\D9\85_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D9\82\D8\B3\D9\85_\D9\86\D8\B4\D8\B7" = alloca i64, align 8
  %"%\D9\82\D8\B3\D9\85_\D9\85\D9\88\D8\AC\D9\88\D8\AF" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D9\82\D8\B3\D8\A7\D9\85" = alloca i64, align 8
  %"%identify_\D9\85\D8\AE\D8\B2\D9\86" = alloca i64, align 8
  %"%_dma_\D9\85\D9\83\D8\AA\D9\85\D9\84" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B1\D8\A7\D8\B5_\D8\A7\D9\84\D9\86\D8\B4\D8\B7\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_3" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_2" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_1" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_0" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_48" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_28" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D9\8A\D8\AF\D8\B9\D9\85_lba48" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D8\A7\D8\AE\D8\AA\D9\8A\D8\A7\D8\B1" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D9\85\D9\86\D9\81\D8\B0_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D9\82\D8\B1\D8\B5_\D9\86\D9\88\D8\B9" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D9\82\D8\B1\D8\A7\D8\B5" = alloca i64, align 8
  %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA" = alloca i64, align 8
  %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9" = alloca i64, align 8
  %"%ATA_ER_AMNF" = alloca i64, align 8
  %"%ATA_ER_TK0NF" = alloca i64, align 8
  %"%ATA_ER_ABRT" = alloca i64, align 8
  %"%ATA_ER_MCR" = alloca i64, align 8
  %"%ATA_ER_IDNF" = alloca i64, align 8
  %"%ATA_ER_MC" = alloca i64, align 8
  %"%ATA_ER_UNC" = alloca i64, align 8
  %"%ATA_ER_BBK" = alloca i64, align 8
  %"%ATA_SR_ERR" = alloca i64, align 8
  %"%ATA_SR_IDX" = alloca i64, align 8
  %"%ATA_SR_CORR" = alloca i64, align 8
  %"%ATA_SR_DRQ" = alloca i64, align 8
  %"%ATA_SR_DSC" = alloca i64, align 8
  %"%ATA_SR_DF" = alloca i64, align 8
  %"%ATA_SR_DRDY" = alloca i64, align 8
  %"%ATA_SR_BSY" = alloca i64, align 8
  %"%ATA_CMD_CACHE_FLUSH_EXT" = alloca i64, align 8
  %"%ATA_CMD_CACHE_FLUSH" = alloca i64, align 8
  %"%ATA_CMD_WRITE_DMA_EXT" = alloca i64, align 8
  %"%ATA_CMD_READ_DMA_EXT" = alloca i64, align 8
  %"%ATA_CMD_WRITE_DMA" = alloca i64, align 8
  %"%ATA_CMD_READ_DMA" = alloca i64, align 8
  %"%ATA_CMD_WRITE_PIO_EXT" = alloca i64, align 8
  %"%ATA_CMD_READ_PIO_EXT" = alloca i64, align 8
  %"%ATA_CMD_WRITE_PIO" = alloca i64, align 8
  %"%ATA_CMD_READ_PIO" = alloca i64, align 8
  %"%ATA_CMD_IDENTIFY" = alloca i64, align 8
  %"%ATA_SEC_CTRL" = alloca i64, align 8
  %"%ATA_SEC_CMD" = alloca i64, align 8
  %"%ATA_SEC_DRIVE" = alloca i64, align 8
  %"%ATA_SEC_LBA_HI" = alloca i64, align 8
  %"%ATA_SEC_LBA_MID" = alloca i64, align 8
  %"%ATA_SEC_LBA_LO" = alloca i64, align 8
  %"%ATA_SEC_SECTORS" = alloca i64, align 8
  %"%ATA_SEC_ERROR" = alloca i64, align 8
  %"%ATA_SEC_DATA" = alloca i64, align 8
  %"%ATA_PRIM_CTRL" = alloca i64, align 8
  %"%ATA_PRIM_CMD" = alloca i64, align 8
  %"%ATA_PRIM_DRIVE" = alloca i64, align 8
  %"%ATA_PRIM_LBA_HI" = alloca i64, align 8
  %"%ATA_PRIM_LBA_MID" = alloca i64, align 8
  %"%ATA_PRIM_LBA_LO" = alloca i64, align 8
  %"%ATA_PRIM_SECTORS" = alloca i64, align 8
  %"%ATA_PRIM_ERROR" = alloca i64, align 8
  %"%ATA_PRIM_DATA" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 496, ptr %"%ATA_PRIM_DATA", align 8
  store i64 497, ptr %"%ATA_PRIM_ERROR", align 8
  store i64 498, ptr %"%ATA_PRIM_SECTORS", align 8
  store i64 499, ptr %"%ATA_PRIM_LBA_LO", align 8
  store i64 500, ptr %"%ATA_PRIM_LBA_MID", align 8
  store i64 501, ptr %"%ATA_PRIM_LBA_HI", align 8
  store i64 502, ptr %"%ATA_PRIM_DRIVE", align 8
  store i64 503, ptr %"%ATA_PRIM_CMD", align 8
  store i64 886, ptr %"%ATA_PRIM_CTRL", align 8
  store i64 368, ptr %"%ATA_SEC_DATA", align 8
  store i64 369, ptr %"%ATA_SEC_ERROR", align 8
  store i64 370, ptr %"%ATA_SEC_SECTORS", align 8
  store i64 371, ptr %"%ATA_SEC_LBA_LO", align 8
  store i64 372, ptr %"%ATA_SEC_LBA_MID", align 8
  store i64 373, ptr %"%ATA_SEC_LBA_HI", align 8
  store i64 374, ptr %"%ATA_SEC_DRIVE", align 8
  store i64 375, ptr %"%ATA_SEC_CMD", align 8
  store i64 886, ptr %"%ATA_SEC_CTRL", align 8
  store i64 236, ptr %"%ATA_CMD_IDENTIFY", align 8
  store i64 32, ptr %"%ATA_CMD_READ_PIO", align 8
  store i64 48, ptr %"%ATA_CMD_WRITE_PIO", align 8
  store i64 36, ptr %"%ATA_CMD_READ_PIO_EXT", align 8
  store i64 52, ptr %"%ATA_CMD_WRITE_PIO_EXT", align 8
  store i64 200, ptr %"%ATA_CMD_READ_DMA", align 8
  store i64 202, ptr %"%ATA_CMD_WRITE_DMA", align 8
  store i64 37, ptr %"%ATA_CMD_READ_DMA_EXT", align 8
  store i64 53, ptr %"%ATA_CMD_WRITE_DMA_EXT", align 8
  store i64 231, ptr %"%ATA_CMD_CACHE_FLUSH", align 8
  store i64 234, ptr %"%ATA_CMD_CACHE_FLUSH_EXT", align 8
  store i64 128, ptr %"%ATA_SR_BSY", align 8
  store i64 64, ptr %"%ATA_SR_DRDY", align 8
  store i64 32, ptr %"%ATA_SR_DF", align 8
  store i64 16, ptr %"%ATA_SR_DSC", align 8
  store i64 8, ptr %"%ATA_SR_DRQ", align 8
  store i64 4, ptr %"%ATA_SR_CORR", align 8
  store i64 2, ptr %"%ATA_SR_IDX", align 8
  store i64 1, ptr %"%ATA_SR_ERR", align 8
  store i64 128, ptr %"%ATA_ER_BBK", align 8
  store i64 64, ptr %"%ATA_ER_UNC", align 8
  store i64 32, ptr %"%ATA_ER_MC", align 8
  store i64 16, ptr %"%ATA_ER_IDNF", align 8
  store i64 8, ptr %"%ATA_ER_MCR", align 8
  store i64 4, ptr %"%ATA_ER_ABRT", align 8
  store i64 2, ptr %"%ATA_ER_TK0NF", align 8
  store i64 1, ptr %"%ATA_ER_AMNF", align 8
  store i64 512, ptr %"%\D8\AD\D8\AC\D9\85_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9", align 8
  store i64 3, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D9\85\D8\AD\D8\A7\D9\88\D9\84\D8\A7\D8\AA", align 8
  store i64 4, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D9\82\D8\B1\D8\A7\D8\B5", align 8
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
  store i64 %obj.ptrtoint, ptr %"%\D9\82\D8\B1\D8\B5_\D9\86\D9\88\D8\B9", align 8
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
  store i64 496, ptr %arr.elem19, align 8
  %arr.data.gep20 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  %arr.data21 = load ptr, ptr %arr.data.gep20, align 8
  %arr.elem22 = getelementptr i64, ptr %arr.data21, i64 1
  store i64 496, ptr %arr.elem22, align 8
  %arr.data.gep23 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  %arr.data24 = load ptr, ptr %arr.data.gep23, align 8
  %arr.elem25 = getelementptr i64, ptr %arr.data24, i64 2
  store i64 368, ptr %arr.elem25, align 8
  %arr.data.gep26 = getelementptr inbounds %SadArray, ptr %arr_new12, i32 0, i32 2
  %arr.data27 = load ptr, ptr %arr.data.gep26, align 8
  %arr.elem28 = getelementptr i64, ptr %arr.data27, i64 3
  store i64 368, ptr %arr.elem28, align 8
  %obj.ptrtoint29 = ptrtoint ptr %arr_new12 to i64
  store i64 %obj.ptrtoint29, ptr %"%\D9\82\D8\B1\D8\B5_\D9\85\D9\86\D9\81\D8\B0_\D8\A8\D9\8A\D8\A7\D9\86\D8\A7\D8\AA", align 8
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
  store i64 160, ptr %arr.elem37, align 8
  %arr.data.gep38 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  %arr.data39 = load ptr, ptr %arr.data.gep38, align 8
  %arr.elem40 = getelementptr i64, ptr %arr.data39, i64 1
  store i64 176, ptr %arr.elem40, align 8
  %arr.data.gep41 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  %arr.data42 = load ptr, ptr %arr.data.gep41, align 8
  %arr.elem43 = getelementptr i64, ptr %arr.data42, i64 2
  store i64 160, ptr %arr.elem43, align 8
  %arr.data.gep44 = getelementptr inbounds %SadArray, ptr %arr_new30, i32 0, i32 2
  %arr.data45 = load ptr, ptr %arr.data.gep44, align 8
  %arr.elem46 = getelementptr i64, ptr %arr.data45, i64 3
  store i64 176, ptr %arr.elem46, align 8
  %obj.ptrtoint47 = ptrtoint ptr %arr_new30 to i64
  store i64 %obj.ptrtoint47, ptr %"%\D9\82\D8\B1\D8\B5_\D8\A7\D8\AE\D8\AA\D9\8A\D8\A7\D8\B1", align 8
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
  store i64 %obj.ptrtoint65, ptr %"%\D9\82\D8\B1\D8\B5_\D9\8A\D8\AF\D8\B9\D9\85_lba48", align 8
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
  store i64 %obj.ptrtoint83, ptr %"%\D9\82\D8\B1\D8\B5_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_28", align 8
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
  store i64 %obj.ptrtoint101, ptr %"%\D9\82\D8\B1\D8\B5_\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA_48", align 8
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
  store i64 %obj.ptrtoint119, ptr %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_0", align 8
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
  store i64 %obj.ptrtoint137, ptr %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_1", align 8
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
  store i64 %obj.ptrtoint155, ptr %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_2", align 8
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
  store i64 %obj.ptrtoint173, ptr %"%\D9\82\D8\B1\D8\B5_\D8\B7\D8\B1\D8\A7\D8\B2_3", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\A5\D8\AC\D9\85\D8\A7\D9\84\D9\8A_\D8\A7\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A7\D8\AA", align 8
  store i64 0, ptr %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  store i64 0, ptr %"%\D8\A3\D8\AE\D8\B7\D8\A7\D8\A1_\D8\A7\D9\84\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B1\D8\A7\D8\B5_\D8\A7\D9\84\D9\86\D8\B4\D8\B7\D8\A9", align 8
  store i64 0, ptr %"%_dma_\D9\85\D9\83\D8\AA\D9\85\D9\84", align 8
  %arr_new174 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep175 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 0
  store i64 64, ptr %arr.len.gep175, align 8
  %arr.cap.gep176 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 1
  store i64 64, ptr %arr.cap.gep176, align 8
  %arr.data177 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 64))
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
  %arr.data.gep191 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data192 = load ptr, ptr %arr.data.gep191, align 8
  %arr.elem193 = getelementptr i64, ptr %arr.data192, i64 4
  store i64 0, ptr %arr.elem193, align 8
  %arr.data.gep194 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data195 = load ptr, ptr %arr.data.gep194, align 8
  %arr.elem196 = getelementptr i64, ptr %arr.data195, i64 5
  store i64 0, ptr %arr.elem196, align 8
  %arr.data.gep197 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data198 = load ptr, ptr %arr.data.gep197, align 8
  %arr.elem199 = getelementptr i64, ptr %arr.data198, i64 6
  store i64 0, ptr %arr.elem199, align 8
  %arr.data.gep200 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data201 = load ptr, ptr %arr.data.gep200, align 8
  %arr.elem202 = getelementptr i64, ptr %arr.data201, i64 7
  store i64 0, ptr %arr.elem202, align 8
  %arr.data.gep203 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data204 = load ptr, ptr %arr.data.gep203, align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 8
  store i64 0, ptr %arr.elem205, align 8
  %arr.data.gep206 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data207 = load ptr, ptr %arr.data.gep206, align 8
  %arr.elem208 = getelementptr i64, ptr %arr.data207, i64 9
  store i64 0, ptr %arr.elem208, align 8
  %arr.data.gep209 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data210 = load ptr, ptr %arr.data.gep209, align 8
  %arr.elem211 = getelementptr i64, ptr %arr.data210, i64 10
  store i64 0, ptr %arr.elem211, align 8
  %arr.data.gep212 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data213 = load ptr, ptr %arr.data.gep212, align 8
  %arr.elem214 = getelementptr i64, ptr %arr.data213, i64 11
  store i64 0, ptr %arr.elem214, align 8
  %arr.data.gep215 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data216 = load ptr, ptr %arr.data.gep215, align 8
  %arr.elem217 = getelementptr i64, ptr %arr.data216, i64 12
  store i64 0, ptr %arr.elem217, align 8
  %arr.data.gep218 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data219 = load ptr, ptr %arr.data.gep218, align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 13
  store i64 0, ptr %arr.elem220, align 8
  %arr.data.gep221 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data222 = load ptr, ptr %arr.data.gep221, align 8
  %arr.elem223 = getelementptr i64, ptr %arr.data222, i64 14
  store i64 0, ptr %arr.elem223, align 8
  %arr.data.gep224 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data225 = load ptr, ptr %arr.data.gep224, align 8
  %arr.elem226 = getelementptr i64, ptr %arr.data225, i64 15
  store i64 0, ptr %arr.elem226, align 8
  %arr.data.gep227 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data228 = load ptr, ptr %arr.data.gep227, align 8
  %arr.elem229 = getelementptr i64, ptr %arr.data228, i64 16
  store i64 0, ptr %arr.elem229, align 8
  %arr.data.gep230 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data231 = load ptr, ptr %arr.data.gep230, align 8
  %arr.elem232 = getelementptr i64, ptr %arr.data231, i64 17
  store i64 0, ptr %arr.elem232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 18
  store i64 0, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 19
  store i64 0, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 20
  store i64 0, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 21
  store i64 0, ptr %arr.elem244, align 8
  %arr.data.gep245 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data246 = load ptr, ptr %arr.data.gep245, align 8
  %arr.elem247 = getelementptr i64, ptr %arr.data246, i64 22
  store i64 0, ptr %arr.elem247, align 8
  %arr.data.gep248 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data249 = load ptr, ptr %arr.data.gep248, align 8
  %arr.elem250 = getelementptr i64, ptr %arr.data249, i64 23
  store i64 0, ptr %arr.elem250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 24
  store i64 0, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 25
  store i64 0, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 26
  store i64 0, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 27
  store i64 0, ptr %arr.elem262, align 8
  %arr.data.gep263 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data264 = load ptr, ptr %arr.data.gep263, align 8
  %arr.elem265 = getelementptr i64, ptr %arr.data264, i64 28
  store i64 0, ptr %arr.elem265, align 8
  %arr.data.gep266 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data267 = load ptr, ptr %arr.data.gep266, align 8
  %arr.elem268 = getelementptr i64, ptr %arr.data267, i64 29
  store i64 0, ptr %arr.elem268, align 8
  %arr.data.gep269 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data270 = load ptr, ptr %arr.data.gep269, align 8
  %arr.elem271 = getelementptr i64, ptr %arr.data270, i64 30
  store i64 0, ptr %arr.elem271, align 8
  %arr.data.gep272 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data273 = load ptr, ptr %arr.data.gep272, align 8
  %arr.elem274 = getelementptr i64, ptr %arr.data273, i64 31
  store i64 0, ptr %arr.elem274, align 8
  %arr.data.gep275 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data276 = load ptr, ptr %arr.data.gep275, align 8
  %arr.elem277 = getelementptr i64, ptr %arr.data276, i64 32
  store i64 0, ptr %arr.elem277, align 8
  %arr.data.gep278 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data279 = load ptr, ptr %arr.data.gep278, align 8
  %arr.elem280 = getelementptr i64, ptr %arr.data279, i64 33
  store i64 0, ptr %arr.elem280, align 8
  %arr.data.gep281 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data282 = load ptr, ptr %arr.data.gep281, align 8
  %arr.elem283 = getelementptr i64, ptr %arr.data282, i64 34
  store i64 0, ptr %arr.elem283, align 8
  %arr.data.gep284 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data285 = load ptr, ptr %arr.data.gep284, align 8
  %arr.elem286 = getelementptr i64, ptr %arr.data285, i64 35
  store i64 0, ptr %arr.elem286, align 8
  %arr.data.gep287 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data288 = load ptr, ptr %arr.data.gep287, align 8
  %arr.elem289 = getelementptr i64, ptr %arr.data288, i64 36
  store i64 0, ptr %arr.elem289, align 8
  %arr.data.gep290 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data291 = load ptr, ptr %arr.data.gep290, align 8
  %arr.elem292 = getelementptr i64, ptr %arr.data291, i64 37
  store i64 0, ptr %arr.elem292, align 8
  %arr.data.gep293 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data294 = load ptr, ptr %arr.data.gep293, align 8
  %arr.elem295 = getelementptr i64, ptr %arr.data294, i64 38
  store i64 0, ptr %arr.elem295, align 8
  %arr.data.gep296 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data297 = load ptr, ptr %arr.data.gep296, align 8
  %arr.elem298 = getelementptr i64, ptr %arr.data297, i64 39
  store i64 0, ptr %arr.elem298, align 8
  %arr.data.gep299 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data300 = load ptr, ptr %arr.data.gep299, align 8
  %arr.elem301 = getelementptr i64, ptr %arr.data300, i64 40
  store i64 0, ptr %arr.elem301, align 8
  %arr.data.gep302 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data303 = load ptr, ptr %arr.data.gep302, align 8
  %arr.elem304 = getelementptr i64, ptr %arr.data303, i64 41
  store i64 0, ptr %arr.elem304, align 8
  %arr.data.gep305 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data306 = load ptr, ptr %arr.data.gep305, align 8
  %arr.elem307 = getelementptr i64, ptr %arr.data306, i64 42
  store i64 0, ptr %arr.elem307, align 8
  %arr.data.gep308 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data309 = load ptr, ptr %arr.data.gep308, align 8
  %arr.elem310 = getelementptr i64, ptr %arr.data309, i64 43
  store i64 0, ptr %arr.elem310, align 8
  %arr.data.gep311 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data312 = load ptr, ptr %arr.data.gep311, align 8
  %arr.elem313 = getelementptr i64, ptr %arr.data312, i64 44
  store i64 0, ptr %arr.elem313, align 8
  %arr.data.gep314 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data315 = load ptr, ptr %arr.data.gep314, align 8
  %arr.elem316 = getelementptr i64, ptr %arr.data315, i64 45
  store i64 0, ptr %arr.elem316, align 8
  %arr.data.gep317 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data318 = load ptr, ptr %arr.data.gep317, align 8
  %arr.elem319 = getelementptr i64, ptr %arr.data318, i64 46
  store i64 0, ptr %arr.elem319, align 8
  %arr.data.gep320 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data321 = load ptr, ptr %arr.data.gep320, align 8
  %arr.elem322 = getelementptr i64, ptr %arr.data321, i64 47
  store i64 0, ptr %arr.elem322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 48
  store i64 0, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 49
  store i64 0, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 50
  store i64 0, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 51
  store i64 0, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 52
  store i64 0, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 53
  store i64 0, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 54
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 55
  store i64 0, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 56
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 57
  store i64 0, ptr %arr.elem352, align 8
  %arr.data.gep353 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data354 = load ptr, ptr %arr.data.gep353, align 8
  %arr.elem355 = getelementptr i64, ptr %arr.data354, i64 58
  store i64 0, ptr %arr.elem355, align 8
  %arr.data.gep356 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data357 = load ptr, ptr %arr.data.gep356, align 8
  %arr.elem358 = getelementptr i64, ptr %arr.data357, i64 59
  store i64 0, ptr %arr.elem358, align 8
  %arr.data.gep359 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data360 = load ptr, ptr %arr.data.gep359, align 8
  %arr.elem361 = getelementptr i64, ptr %arr.data360, i64 60
  store i64 0, ptr %arr.elem361, align 8
  %arr.data.gep362 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data363 = load ptr, ptr %arr.data.gep362, align 8
  %arr.elem364 = getelementptr i64, ptr %arr.data363, i64 61
  store i64 0, ptr %arr.elem364, align 8
  %arr.data.gep365 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data366 = load ptr, ptr %arr.data.gep365, align 8
  %arr.elem367 = getelementptr i64, ptr %arr.data366, i64 62
  store i64 0, ptr %arr.elem367, align 8
  %arr.data.gep368 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data369 = load ptr, ptr %arr.data.gep368, align 8
  %arr.elem370 = getelementptr i64, ptr %arr.data369, i64 63
  store i64 0, ptr %arr.elem370, align 8
  %obj.ptrtoint371 = ptrtoint ptr %arr_new174 to i64
  store i64 %obj.ptrtoint371, ptr %"%identify_\D9\85\D8\AE\D8\B2\D9\86", align 8
  store i64 16, ptr %"%\D8\A3\D9\82\D8\B5\D9\89_\D8\A3\D9\82\D8\B3\D8\A7\D9\85", align 8
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
  store i64 %obj.ptrtoint425, ptr %"%\D9\82\D8\B3\D9\85_\D9\85\D9\88\D8\AC\D9\88\D8\AF", align 8
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
  store i64 %obj.ptrtoint479, ptr %"%\D9\82\D8\B3\D9\85_\D9\86\D8\B4\D8\B7", align 8
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
  store i64 %obj.ptrtoint533, ptr %"%\D9\82\D8\B3\D9\85_\D9\86\D9\88\D8\B9", align 8
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
  store i64 %obj.ptrtoint587, ptr %"%\D9\82\D8\B3\D9\85_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_LBA", align 8
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
  store i64 %obj.ptrtoint641, ptr %"%\D9\82\D8\B3\D9\85_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\82\D8\B7\D8\A7\D8\B9\D8\A7\D8\AA", align 8
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
  store i64 %obj.ptrtoint695, ptr %"%\D9\82\D8\B3\D9\85_\D8\AD\D8\AC\D9\85_MB", align 8
  %arr_new696 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep697 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 0
  store i64 16, ptr %arr.len.gep697, align 8
  %arr.cap.gep698 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep698, align 8
  %arr.data699 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep700 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  store ptr %arr.data699, ptr %arr.data.gep700, align 8
  %arr.data.gep701 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data702 = load ptr, ptr %arr.data.gep701, align 8
  %arr.elem703 = getelementptr i64, ptr %arr.data702, i64 0
  store i64 0, ptr %arr.elem703, align 8
  %arr.data.gep704 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data705 = load ptr, ptr %arr.data.gep704, align 8
  %arr.elem706 = getelementptr i64, ptr %arr.data705, i64 1
  store i64 0, ptr %arr.elem706, align 8
  %arr.data.gep707 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data708 = load ptr, ptr %arr.data.gep707, align 8
  %arr.elem709 = getelementptr i64, ptr %arr.data708, i64 2
  store i64 0, ptr %arr.elem709, align 8
  %arr.data.gep710 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data711 = load ptr, ptr %arr.data.gep710, align 8
  %arr.elem712 = getelementptr i64, ptr %arr.data711, i64 3
  store i64 0, ptr %arr.elem712, align 8
  %arr.data.gep713 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data714 = load ptr, ptr %arr.data.gep713, align 8
  %arr.elem715 = getelementptr i64, ptr %arr.data714, i64 4
  store i64 0, ptr %arr.elem715, align 8
  %arr.data.gep716 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data717 = load ptr, ptr %arr.data.gep716, align 8
  %arr.elem718 = getelementptr i64, ptr %arr.data717, i64 5
  store i64 0, ptr %arr.elem718, align 8
  %arr.data.gep719 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data720 = load ptr, ptr %arr.data.gep719, align 8
  %arr.elem721 = getelementptr i64, ptr %arr.data720, i64 6
  store i64 0, ptr %arr.elem721, align 8
  %arr.data.gep722 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data723 = load ptr, ptr %arr.data.gep722, align 8
  %arr.elem724 = getelementptr i64, ptr %arr.data723, i64 7
  store i64 0, ptr %arr.elem724, align 8
  %arr.data.gep725 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data726 = load ptr, ptr %arr.data.gep725, align 8
  %arr.elem727 = getelementptr i64, ptr %arr.data726, i64 8
  store i64 0, ptr %arr.elem727, align 8
  %arr.data.gep728 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data729 = load ptr, ptr %arr.data.gep728, align 8
  %arr.elem730 = getelementptr i64, ptr %arr.data729, i64 9
  store i64 0, ptr %arr.elem730, align 8
  %arr.data.gep731 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data732 = load ptr, ptr %arr.data.gep731, align 8
  %arr.elem733 = getelementptr i64, ptr %arr.data732, i64 10
  store i64 0, ptr %arr.elem733, align 8
  %arr.data.gep734 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data735 = load ptr, ptr %arr.data.gep734, align 8
  %arr.elem736 = getelementptr i64, ptr %arr.data735, i64 11
  store i64 0, ptr %arr.elem736, align 8
  %arr.data.gep737 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data738 = load ptr, ptr %arr.data.gep737, align 8
  %arr.elem739 = getelementptr i64, ptr %arr.data738, i64 12
  store i64 0, ptr %arr.elem739, align 8
  %arr.data.gep740 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data741 = load ptr, ptr %arr.data.gep740, align 8
  %arr.elem742 = getelementptr i64, ptr %arr.data741, i64 13
  store i64 0, ptr %arr.elem742, align 8
  %arr.data.gep743 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data744 = load ptr, ptr %arr.data.gep743, align 8
  %arr.elem745 = getelementptr i64, ptr %arr.data744, i64 14
  store i64 0, ptr %arr.elem745, align 8
  %arr.data.gep746 = getelementptr inbounds %SadArray, ptr %arr_new696, i32 0, i32 2
  %arr.data747 = load ptr, ptr %arr.data.gep746, align 8
  %arr.elem748 = getelementptr i64, ptr %arr.data747, i64 15
  store i64 0, ptr %arr.elem748, align 8
  %obj.ptrtoint749 = ptrtoint ptr %arr_new696 to i64
  store i64 %obj.ptrtoint749, ptr %"%\D9\82\D8\B3\D9\85_\D9\82\D8\B1\D8\B5", align 8
  store i64 0, ptr %"%\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D8\A3\D9\82\D8\B3\D8\A7\D9\85", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AE\D8\B2\D9\86_MBR", align 8
  store i64 0, ptr %"%dma_\D9\85\D9\86\D9\81\D8\B0_\D8\A3\D8\B3\D8\A7\D8\B3\D9\8A", align 8
  store i64 0, ptr %"%DMA_CMD", align 8
  store i64 2, ptr %"%DMA_STAT", align 8
  store i64 4, ptr %"%DMA_PRDT", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_PRDT", align 8
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_DMA_\D9\85\D8\AE\D8\B2\D9\86", align 8
  store i64 0, ptr %"%ahci_\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\82\D8\A7\D8\B9\D8\AF\D8\A9", align 8
  %arr_new750 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep751 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 0
  store i64 16, ptr %arr.len.gep751, align 8
  %arr.cap.gep752 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep752, align 8
  %arr.data753 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep754 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  store ptr %arr.data753, ptr %arr.data.gep754, align 8
  %arr.data.gep755 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data756 = load ptr, ptr %arr.data.gep755, align 8
  %arr.elem757 = getelementptr i64, ptr %arr.data756, i64 0
  store i64 0, ptr %arr.elem757, align 8
  %arr.data.gep758 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data759 = load ptr, ptr %arr.data.gep758, align 8
  %arr.elem760 = getelementptr i64, ptr %arr.data759, i64 1
  store i64 0, ptr %arr.elem760, align 8
  %arr.data.gep761 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data762 = load ptr, ptr %arr.data.gep761, align 8
  %arr.elem763 = getelementptr i64, ptr %arr.data762, i64 2
  store i64 0, ptr %arr.elem763, align 8
  %arr.data.gep764 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data765 = load ptr, ptr %arr.data.gep764, align 8
  %arr.elem766 = getelementptr i64, ptr %arr.data765, i64 3
  store i64 0, ptr %arr.elem766, align 8
  %arr.data.gep767 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data768 = load ptr, ptr %arr.data.gep767, align 8
  %arr.elem769 = getelementptr i64, ptr %arr.data768, i64 4
  store i64 0, ptr %arr.elem769, align 8
  %arr.data.gep770 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data771 = load ptr, ptr %arr.data.gep770, align 8
  %arr.elem772 = getelementptr i64, ptr %arr.data771, i64 5
  store i64 0, ptr %arr.elem772, align 8
  %arr.data.gep773 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data774 = load ptr, ptr %arr.data.gep773, align 8
  %arr.elem775 = getelementptr i64, ptr %arr.data774, i64 6
  store i64 0, ptr %arr.elem775, align 8
  %arr.data.gep776 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data777 = load ptr, ptr %arr.data.gep776, align 8
  %arr.elem778 = getelementptr i64, ptr %arr.data777, i64 7
  store i64 0, ptr %arr.elem778, align 8
  %arr.data.gep779 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data780 = load ptr, ptr %arr.data.gep779, align 8
  %arr.elem781 = getelementptr i64, ptr %arr.data780, i64 8
  store i64 0, ptr %arr.elem781, align 8
  %arr.data.gep782 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data783 = load ptr, ptr %arr.data.gep782, align 8
  %arr.elem784 = getelementptr i64, ptr %arr.data783, i64 9
  store i64 0, ptr %arr.elem784, align 8
  %arr.data.gep785 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data786 = load ptr, ptr %arr.data.gep785, align 8
  %arr.elem787 = getelementptr i64, ptr %arr.data786, i64 10
  store i64 0, ptr %arr.elem787, align 8
  %arr.data.gep788 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data789 = load ptr, ptr %arr.data.gep788, align 8
  %arr.elem790 = getelementptr i64, ptr %arr.data789, i64 11
  store i64 0, ptr %arr.elem790, align 8
  %arr.data.gep791 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data792 = load ptr, ptr %arr.data.gep791, align 8
  %arr.elem793 = getelementptr i64, ptr %arr.data792, i64 12
  store i64 0, ptr %arr.elem793, align 8
  %arr.data.gep794 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data795 = load ptr, ptr %arr.data.gep794, align 8
  %arr.elem796 = getelementptr i64, ptr %arr.data795, i64 13
  store i64 0, ptr %arr.elem796, align 8
  %arr.data.gep797 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data798 = load ptr, ptr %arr.data.gep797, align 8
  %arr.elem799 = getelementptr i64, ptr %arr.data798, i64 14
  store i64 0, ptr %arr.elem799, align 8
  %arr.data.gep800 = getelementptr inbounds %SadArray, ptr %arr_new750, i32 0, i32 2
  %arr.data801 = load ptr, ptr %arr.data.gep800, align 8
  %arr.elem802 = getelementptr i64, ptr %arr.data801, i64 15
  store i64 0, ptr %arr.elem802, align 8
  %obj.ptrtoint803 = ptrtoint ptr %arr_new750 to i64
  store i64 %obj.ptrtoint803, ptr %"%ahci_\D9\85\D9\86\D9\81\D8\B0_\D9\86\D8\B4\D8\B7", align 8
  %arr_new804 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep805 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 0
  store i64 16, ptr %arr.len.gep805, align 8
  %arr.cap.gep806 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep806, align 8
  %arr.data807 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep808 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  store ptr %arr.data807, ptr %arr.data.gep808, align 8
  %arr.data.gep809 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data810 = load ptr, ptr %arr.data.gep809, align 8
  %arr.elem811 = getelementptr i64, ptr %arr.data810, i64 0
  store i64 0, ptr %arr.elem811, align 8
  %arr.data.gep812 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data813 = load ptr, ptr %arr.data.gep812, align 8
  %arr.elem814 = getelementptr i64, ptr %arr.data813, i64 1
  store i64 0, ptr %arr.elem814, align 8
  %arr.data.gep815 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data816 = load ptr, ptr %arr.data.gep815, align 8
  %arr.elem817 = getelementptr i64, ptr %arr.data816, i64 2
  store i64 0, ptr %arr.elem817, align 8
  %arr.data.gep818 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data819 = load ptr, ptr %arr.data.gep818, align 8
  %arr.elem820 = getelementptr i64, ptr %arr.data819, i64 3
  store i64 0, ptr %arr.elem820, align 8
  %arr.data.gep821 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data822 = load ptr, ptr %arr.data.gep821, align 8
  %arr.elem823 = getelementptr i64, ptr %arr.data822, i64 4
  store i64 0, ptr %arr.elem823, align 8
  %arr.data.gep824 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data825 = load ptr, ptr %arr.data.gep824, align 8
  %arr.elem826 = getelementptr i64, ptr %arr.data825, i64 5
  store i64 0, ptr %arr.elem826, align 8
  %arr.data.gep827 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data828 = load ptr, ptr %arr.data.gep827, align 8
  %arr.elem829 = getelementptr i64, ptr %arr.data828, i64 6
  store i64 0, ptr %arr.elem829, align 8
  %arr.data.gep830 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data831 = load ptr, ptr %arr.data.gep830, align 8
  %arr.elem832 = getelementptr i64, ptr %arr.data831, i64 7
  store i64 0, ptr %arr.elem832, align 8
  %arr.data.gep833 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data834 = load ptr, ptr %arr.data.gep833, align 8
  %arr.elem835 = getelementptr i64, ptr %arr.data834, i64 8
  store i64 0, ptr %arr.elem835, align 8
  %arr.data.gep836 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data837 = load ptr, ptr %arr.data.gep836, align 8
  %arr.elem838 = getelementptr i64, ptr %arr.data837, i64 9
  store i64 0, ptr %arr.elem838, align 8
  %arr.data.gep839 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data840 = load ptr, ptr %arr.data.gep839, align 8
  %arr.elem841 = getelementptr i64, ptr %arr.data840, i64 10
  store i64 0, ptr %arr.elem841, align 8
  %arr.data.gep842 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data843 = load ptr, ptr %arr.data.gep842, align 8
  %arr.elem844 = getelementptr i64, ptr %arr.data843, i64 11
  store i64 0, ptr %arr.elem844, align 8
  %arr.data.gep845 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data846 = load ptr, ptr %arr.data.gep845, align 8
  %arr.elem847 = getelementptr i64, ptr %arr.data846, i64 12
  store i64 0, ptr %arr.elem847, align 8
  %arr.data.gep848 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data849 = load ptr, ptr %arr.data.gep848, align 8
  %arr.elem850 = getelementptr i64, ptr %arr.data849, i64 13
  store i64 0, ptr %arr.elem850, align 8
  %arr.data.gep851 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data852 = load ptr, ptr %arr.data.gep851, align 8
  %arr.elem853 = getelementptr i64, ptr %arr.data852, i64 14
  store i64 0, ptr %arr.elem853, align 8
  %arr.data.gep854 = getelementptr inbounds %SadArray, ptr %arr_new804, i32 0, i32 2
  %arr.data855 = load ptr, ptr %arr.data.gep854, align 8
  %arr.elem856 = getelementptr i64, ptr %arr.data855, i64 15
  store i64 0, ptr %arr.elem856, align 8
  %obj.ptrtoint857 = ptrtoint ptr %arr_new804 to i64
  store i64 %obj.ptrtoint857, ptr %"%ahci_\D9\85\D9\86\D9\81\D8\B0_\D9\86\D9\88\D8\B9", align 8
  %arr_new858 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep859 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 0
  store i64 16, ptr %arr.len.gep859, align 8
  %arr.cap.gep860 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 1
  store i64 16, ptr %arr.cap.gep860, align 8
  %arr.data861 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 16))
  %arr.data.gep862 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  store ptr %arr.data861, ptr %arr.data.gep862, align 8
  %arr.data.gep863 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data864 = load ptr, ptr %arr.data.gep863, align 8
  %arr.elem865 = getelementptr i64, ptr %arr.data864, i64 0
  store i64 0, ptr %arr.elem865, align 8
  %arr.data.gep866 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data867 = load ptr, ptr %arr.data.gep866, align 8
  %arr.elem868 = getelementptr i64, ptr %arr.data867, i64 1
  store i64 0, ptr %arr.elem868, align 8
  %arr.data.gep869 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data870 = load ptr, ptr %arr.data.gep869, align 8
  %arr.elem871 = getelementptr i64, ptr %arr.data870, i64 2
  store i64 0, ptr %arr.elem871, align 8
  %arr.data.gep872 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data873 = load ptr, ptr %arr.data.gep872, align 8
  %arr.elem874 = getelementptr i64, ptr %arr.data873, i64 3
  store i64 0, ptr %arr.elem874, align 8
  %arr.data.gep875 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data876 = load ptr, ptr %arr.data.gep875, align 8
  %arr.elem877 = getelementptr i64, ptr %arr.data876, i64 4
  store i64 0, ptr %arr.elem877, align 8
  %arr.data.gep878 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data879 = load ptr, ptr %arr.data.gep878, align 8
  %arr.elem880 = getelementptr i64, ptr %arr.data879, i64 5
  store i64 0, ptr %arr.elem880, align 8
  %arr.data.gep881 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data882 = load ptr, ptr %arr.data.gep881, align 8
  %arr.elem883 = getelementptr i64, ptr %arr.data882, i64 6
  store i64 0, ptr %arr.elem883, align 8
  %arr.data.gep884 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data885 = load ptr, ptr %arr.data.gep884, align 8
  %arr.elem886 = getelementptr i64, ptr %arr.data885, i64 7
  store i64 0, ptr %arr.elem886, align 8
  %arr.data.gep887 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data888 = load ptr, ptr %arr.data.gep887, align 8
  %arr.elem889 = getelementptr i64, ptr %arr.data888, i64 8
  store i64 0, ptr %arr.elem889, align 8
  %arr.data.gep890 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data891 = load ptr, ptr %arr.data.gep890, align 8
  %arr.elem892 = getelementptr i64, ptr %arr.data891, i64 9
  store i64 0, ptr %arr.elem892, align 8
  %arr.data.gep893 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data894 = load ptr, ptr %arr.data.gep893, align 8
  %arr.elem895 = getelementptr i64, ptr %arr.data894, i64 10
  store i64 0, ptr %arr.elem895, align 8
  %arr.data.gep896 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data897 = load ptr, ptr %arr.data.gep896, align 8
  %arr.elem898 = getelementptr i64, ptr %arr.data897, i64 11
  store i64 0, ptr %arr.elem898, align 8
  %arr.data.gep899 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data900 = load ptr, ptr %arr.data.gep899, align 8
  %arr.elem901 = getelementptr i64, ptr %arr.data900, i64 12
  store i64 0, ptr %arr.elem901, align 8
  %arr.data.gep902 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data903 = load ptr, ptr %arr.data.gep902, align 8
  %arr.elem904 = getelementptr i64, ptr %arr.data903, i64 13
  store i64 0, ptr %arr.elem904, align 8
  %arr.data.gep905 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data906 = load ptr, ptr %arr.data.gep905, align 8
  %arr.elem907 = getelementptr i64, ptr %arr.data906, i64 14
  store i64 0, ptr %arr.elem907, align 8
  %arr.data.gep908 = getelementptr inbounds %SadArray, ptr %arr_new858, i32 0, i32 2
  %arr.data909 = load ptr, ptr %arr.data.gep908, align 8
  %arr.elem910 = getelementptr i64, ptr %arr.data909, i64 15
  store i64 0, ptr %arr.elem910, align 8
  %obj.ptrtoint911 = ptrtoint ptr %arr_new858 to i64
  store i64 %obj.ptrtoint911, ptr %"%ahci_cmd_list", align 8
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
  store i64 %obj.ptrtoint965, ptr %"%ahci_fis_addr", align 8
  store i64 0, ptr %"%ahci_\D8\B9\D8\AF\D8\AF_\D8\A7\D9\84\D9\85\D9\86\D8\A7\D9\81\D8\B0", align 8
  store i64 0, ptr %"%AHCI_CAP", align 8
  store i64 4, ptr %"%AHCI_GHC", align 8
  store i64 8, ptr %"%AHCI_IS", align 8
  store i64 12, ptr %"%AHCI_PI", align 8
  store i64 16, ptr %"%AHCI_VS", align 8
  store i64 0, ptr %"%AHCI_PxCLB", align 8
  store i64 4, ptr %"%AHCI_PxCLBU", align 8
  store i64 8, ptr %"%AHCI_PxFB", align 8
  store i64 12, ptr %"%AHCI_PxFBU", align 8
  store i64 16, ptr %"%AHCI_PxIS", align 8
  store i64 20, ptr %"%AHCI_PxIE", align 8
  store i64 24, ptr %"%AHCI_PxCMD", align 8
  store i64 32, ptr %"%AHCI_PxTFD", align 8
  store i64 36, ptr %"%AHCI_PxSIG", align 8
  store i64 40, ptr %"%AHCI_PxSSTS", align 8
  store i64 44, ptr %"%AHCI_PxSCTL", align 8
  store i64 48, ptr %"%AHCI_PxSERR", align 8
  store i64 52, ptr %"%AHCI_PxSACT", align 8
  store i64 56, ptr %"%AHCI_PxCI", align 8
  ret void
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i64(ptr nocapture writeonly, i8, i64, i1 immarg) #0

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: write) }
