; ModuleID = 'صدفة_نحلة'
source_filename = "\D8\B5\D8\AF\D9\81\D8\A9_\D9\86\D8\AD\D9\84\D8\A9"
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

declare void @"\D8\A7\D9\81\D8\AA\D8\AD"(i64, i64, i64, i64)

declare void @"\D8\A3\D8\BA\D9\84\D9\82"(i64)

declare void @"\D8\A7\D9\82\D8\B1\D8\A3"(i64, i64, i64)

declare void @"\D8\A7\D9\82\D8\B1\D8\A3_\D8\AF\D9\84\D9\8A\D9\84"(i64, i64, i64, i64)

declare void @"\D8\A3\D9\86\D8\B4\D8\A6_\D9\85\D9\84\D9\81"(i64, i64, i64)

declare void @"\D8\A3\D9\86\D8\B4\D8\A6_\D8\AF\D9\84\D9\8A\D9\84"(i64, i64, i64)

declare void @"\D8\A7\D8\AD\D8\B0\D9\81_\D9\85\D9\84\D9\81"(i64, i64, i64)

declare void @"\D8\AD\D9\84\D9\84_\D8\A7\D9\84\D9\85\D8\B3\D8\A7\D8\B1"(i64, i64, i64)

declare void @"\D8\A7\D8\B3\D8\AA\D8\B9\D9\84\D9\85"(i64, i64, i64, i64)

define void @"sh_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
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
  call void @"sh_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_2:                                           ; preds = %merge_1
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"sh_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_3
}

define void @"vga_\D8\B6\D8\B9_\D9\84\D9\88\D9\86"(i64 %"\D9\84\D9\88\D9\86_\D8\AC\D8\AF\D9\8A\D8\AF") {
entry:
  %"\D9\84\D9\88\D9\86_\D8\AC\D8\AF\D9\8A\D8\AF1" = alloca i64, align 8
  store i64 %"\D9\84\D9\88\D9\86_\D8\AC\D8\AF\D9\8A\D8\AF", ptr %"\D9\84\D9\88\D9\86_\D8\AC\D8\AF\D9\8A\D8\AF1", align 8
  ret void
}

define void @"vga_\D8\AA\D9\85\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%\D8\B9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A2\D8\AE\D8\B1" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\88\D8\AC\D9\87\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B5\D8\AF\D8\B1" = alloca i64, align 8
  %"%\D8\B5\D9\81" = alloca i64, align 8
  store i64 0, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_4

while_body_5:                                     ; preds = %while_cond_4
  %"%\D8\B5\D9\81.load1" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %addtmp = add i64 %"%\D8\B5\D9\81.load1", 1
  %multmp = mul i64 %addtmp, 0
  %shltmp = shl i64 %multmp, 1
  %addtmp2 = add i64 0, %shltmp
  store i64 %addtmp2, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B5\D8\AF\D8\B1", align 8
  %"%\D8\B5\D9\81.load3" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %multmp4 = mul i64 %"%\D8\B5\D9\81.load3", 0
  %shltmp5 = shl i64 %multmp4, 1
  %addtmp6 = add i64 0, %shltmp5
  store i64 %addtmp6, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\88\D8\AC\D9\87\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\88\D8\AC\D9\87\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\88\D8\AC\D9\87\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B5\D8\AF\D8\B1.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B5\D8\AF\D8\B1", align 8
  call void @sad_ll_memcpy(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\88\D8\AC\D9\87\D8\A9.load", i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B5\D8\AF\D8\B1.load", i64 0)
  store i64 %addtmp, ptr %"%\D8\B5\D9\81", align 8
  br label %while_cond_4

while_body_8:                                     ; preds = %while_cond_7
  %"%\D8\B9.load8" = load i64, ptr %"%\D8\B9", align 8
  %shltmp9 = shl i64 %"%\D8\B9.load8", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A2\D8\AE\D8\B1.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A2\D8\AE\D8\B1", align 8
  %addtmp10 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A2\D8\AE\D8\B1.load", %shltmp9
  call void @sad_ll_mem_write16(i64 %addtmp10, i64 32)
  %"%\D8\B9.load11" = load i64, ptr %"%\D8\B9", align 8
  %addtmp12 = add i64 %"%\D8\B9.load11", 1
  store i64 %addtmp12, ptr %"%\D8\B9", align 8
  br label %while_cond_7

while_cond_4:                                     ; preds = %while_body_5, %entry
  %"%\D8\B5\D9\81.load" = load i64, ptr %"%\D8\B5\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B5\D9\81.load", -1
  br i1 %cmplttmp, label %while_body_5, label %while_exit_6

while_cond_7:                                     ; preds = %while_body_8, %while_exit_6
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %cmplttmp7 = icmp slt i64 %"%\D8\B9.load", 0
  br i1 %cmplttmp7, label %while_body_8, label %while_exit_9

while_exit_6:                                     ; preds = %while_cond_4
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A2\D8\AE\D8\B1", align 8
  store i64 0, ptr %"%\D8\B9", align 8
  br label %while_cond_7

while_exit_9:                                     ; preds = %while_cond_7
  ret void
}

define void @"vga_\D9\85\D8\B3\D8\AD"() {
entry:
  %"%\D8\B9" = alloca i64, align 8
  store i64 0, ptr %"%\D8\B9", align 8
  br label %while_cond_10

while_body_11:                                    ; preds = %while_cond_10
  %"%\D8\B9.load1" = load i64, ptr %"%\D8\B9", align 8
  %shltmp = shl i64 %"%\D8\B9.load1", 1
  %addtmp = add i64 0, %shltmp
  call void @sad_ll_mem_write16(i64 %addtmp, i64 32)
  %"%\D8\B9.load2" = load i64, ptr %"%\D8\B9", align 8
  %addtmp3 = add i64 %"%\D8\B9.load2", 1
  store i64 %addtmp3, ptr %"%\D8\B9", align 8
  br label %while_cond_10

while_cond_10:                                    ; preds = %while_body_11, %entry
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", 0
  br i1 %cmplttmp, label %while_body_11, label %while_exit_12

while_exit_12:                                    ; preds = %while_cond_10
  ret void
}

define void @"vga_\D8\AD\D8\B1\D9\81"(i64 %"\D8\AD") {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_vga" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B3\D8\AD" = alloca i64, align 8
  %"\D8\AD1" = alloca i64, align 8
  store i64 %"\D8\AD", ptr %"\D8\AD1", align 8
  %"%\D8\AD.load" = load i64, ptr %"\D8\AD1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\AD.load", 10
  br i1 %cmpeqtmp, label %then_13, label %merge_14

merge_14:                                         ; preds = %entry
  %"%\D8\AD.load2" = load i64, ptr %"\D8\AD1", align 8
  %cmpeqtmp3 = icmp eq i64 %"%\D8\AD.load2", 13
  br i1 %cmpeqtmp3, label %then_17, label %merge_18

merge_16:                                         ; preds = %then_15, %then_13
  ret void

merge_18:                                         ; preds = %merge_14
  %"%\D8\AD.load4" = load i64, ptr %"\D8\AD1", align 8
  %cmpeqtmp5 = icmp eq i64 %"%\D8\AD.load4", 8
  br i1 %cmpeqtmp5, label %then_19, label %merge_20

merge_20:                                         ; preds = %merge_18
  br i1 true, label %then_23, label %merge_24

merge_22:                                         ; preds = %then_21, %then_19
  ret void

merge_24:                                         ; preds = %merge_26, %merge_20
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_vga", align 8
  %"%\D8\AD.load6" = load i64, ptr %"\D8\AD1", align 8
  %addtmp = add i64 0, %"%\D8\AD.load6"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_vga.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_vga", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_vga.load", i64 %addtmp)
  ret void

merge_26:                                         ; preds = %then_25, %then_23
  br label %merge_24

then_13:                                          ; preds = %entry
  br i1 true, label %then_15, label %merge_16

then_15:                                          ; preds = %then_13
  call void @"vga_\D8\AA\D9\85\D8\B1\D9\8A\D8\B1"()
  br label %merge_16

then_17:                                          ; preds = %merge_14
  ret void

then_19:                                          ; preds = %merge_18
  br i1 false, label %then_21, label %merge_22

then_21:                                          ; preds = %then_19
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B3\D8\AD", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B3\D8\AD.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B3\D8\AD", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\B3\D8\AD.load", i64 32)
  br label %merge_22

then_23:                                          ; preds = %merge_20
  br i1 true, label %then_25, label %merge_26

then_25:                                          ; preds = %then_23
  call void @"vga_\D8\AA\D9\85\D8\B1\D9\8A\D8\B1"()
  br label %merge_26
}

define void @"vga_\D9\86\D8\B5"(i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5") {
entry:
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D8\B2\D9\88\D8\AC" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B52" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B52", align 8
  %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B51" = alloca i64, align 8
  store i64 %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5", ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B51", align 8
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_27

merge_31:                                         ; preds = %then_30, %while_body_28
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load")
  %"%\D9\81.load7" = load i64, ptr %"%\D9\81", align 8
  %addtmp8 = add i64 %"%\D9\81.load7", 1
  store i64 %addtmp8, ptr %"%\D9\81", align 8
  br label %while_cond_27

then_30:                                          ; preds = %while_body_28
  %"%\D8\B2\D9\88\D8\AC.load6" = load i64, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %shrtmp = lshr i64 %"%\D8\B2\D9\88\D8\AC.load6", 8
  store i64 %shrtmp, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  br label %merge_31

while_body_28:                                    ; preds = %while_cond_27
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5.load" = load i64, ptr %"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B51", align 8
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\86\D8\B5.load", %"%\D9\81.load3"
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %andtmp = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load4" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %subtmp = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load4", %andtmp
  call void @sad_ll_mem_read16(i64 %subtmp)
  store i64 0, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %"%\D8\B2\D9\88\D8\AC.load" = load i64, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %andtmp5 = and i64 %"%\D8\B2\D9\88\D8\AC.load", 255
  store i64 %andtmp5, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_30, label %merge_31

while_cond_27:                                    ; preds = %merge_31, %entry
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B52", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\86\D8\B5.load"
  br i1 %cmplttmp, label %while_body_28, label %while_exit_29

while_exit_29:                                    ; preds = %while_cond_27
  ret void
}

define void @"vga_\D8\B1\D9\82\D9\85"(i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9") {
entry:
  %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9", ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmplttmp = icmp slt i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load", 0
  br i1 %cmplttmp, label %then_32, label %merge_33

merge_33:                                         ; preds = %entry
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load3" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load3", 10
  br i1 %cmpgetmp, label %then_34, label %merge_35

merge_35:                                         ; preds = %then_34, %merge_33
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load5" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %modtmp = srem i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load5", 10
  %addtmp = add i64 48, %modtmp
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 %addtmp)
  ret void

then_32:                                          ; preds = %entry
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 45)
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load2" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %subtmp = sub i64 0, %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load2"
  call void @"vga_\D8\B1\D9\82\D9\85"(i64 %subtmp)
  ret void

then_34:                                          ; preds = %merge_33
  %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4" = load i64, ptr %"\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A91", align 8
  %divtmp = sdiv i64 %"%\D8\A7\D9\84\D9\82\D9\8A\D9\85\D8\A9.load4", 10
  call void @"vga_\D8\B1\D9\82\D9\85"(i64 %divtmp)
  br label %merge_35
}

define void @"vga_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\85\D8\A4\D8\B4\D8\B1"() {
entry:
  %"%\D9\85\D9\88\D9\82\D8\B9" = alloca i64, align 8
  store i64 0, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  call void @sad_ll_port_outb(i64 948, i64 14)
  %"%\D9\85\D9\88\D9\82\D8\B9.load" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %shrtmp = lshr i64 %"%\D9\85\D9\88\D9\82\D8\B9.load", 8
  call void @sad_ll_port_outb(i64 949, i64 %shrtmp)
  call void @sad_ll_port_outb(i64 948, i64 15)
  %"%\D9\85\D9\88\D9\82\D8\B9.load1" = load i64, ptr %"%\D9\85\D9\88\D9\82\D8\B9", align 8
  %andtmp = and i64 %"%\D9\85\D9\88\D9\82\D8\B9.load1", 255
  call void @sad_ll_port_outb(i64 949, i64 %andtmp)
  ret void
}

define i64 @"kb_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D9\81\D8\AA\D8\A7\D8\AD"() {
entry:
  %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1" = alloca i64, align 8
  %"%\D8\B3\D9\83\D8\A7\D9\86" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0", align 8
  %andtmp = and i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D9\85\D9\86\D9\81\D8\B0.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_36, label %merge_37

else_70:                                          ; preds = %then_66
  %"%\D8\B3\D9\83\D8\A7\D9\86.load29" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %arr.data30 = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem31 = getelementptr i64, ptr %arr.data30, i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load29"
  %arr.get32 = load i64, ptr %arr.elem31, align 8
  ret i64 %arr.get32

merge_37:                                         ; preds = %entry
  %"%\D8\B3\D9\83\D8\A7\D9\86.load" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load", 128
  br i1 %cmpgetmp, label %then_38, label %merge_39

merge_39:                                         ; preds = %merge_37
  %"%\D8\B3\D9\83\D8\A7\D9\86.load3" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load3", 42
  br i1 %cmpeqtmp4, label %then_46, label %merge_47

merge_41:                                         ; preds = %then_40, %then_38
  %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1.load5" = load i64, ptr %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1.load5", 54
  br i1 %cmpeqtmp6, label %then_42, label %merge_43

merge_43:                                         ; preds = %then_42, %merge_41
  %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1.load7" = load i64, ptr %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1.load7", 29
  br i1 %cmpeqtmp8, label %then_44, label %merge_45

merge_45:                                         ; preds = %then_44, %merge_43
  ret i64 0

merge_47:                                         ; preds = %merge_39
  %"%\D8\B3\D9\83\D8\A7\D9\86.load9" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load9", 54
  br i1 %cmpeqtmp10, label %then_48, label %merge_49

merge_49:                                         ; preds = %merge_47
  %"%\D8\B3\D9\83\D8\A7\D9\86.load11" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load11", 29
  br i1 %cmpeqtmp12, label %then_50, label %merge_51

merge_51:                                         ; preds = %merge_49
  %"%\D8\B3\D9\83\D8\A7\D9\86.load13" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp14 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load13", 72
  br i1 %cmpeqtmp14, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  %"%\D8\B3\D9\83\D8\A7\D9\86.load15" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp16 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load15", 80
  br i1 %cmpeqtmp16, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  %"%\D8\B3\D9\83\D8\A7\D9\86.load17" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp18 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load17", 75
  br i1 %cmpeqtmp18, label %then_56, label %merge_57

merge_57:                                         ; preds = %merge_55
  %"%\D8\B3\D9\83\D8\A7\D9\86.load19" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp20 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load19", 77
  br i1 %cmpeqtmp20, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%\D8\B3\D9\83\D8\A7\D9\86.load21" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp22 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load21", 71
  br i1 %cmpeqtmp22, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%\D8\B3\D9\83\D8\A7\D9\86.load23" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp24 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load23", 79
  br i1 %cmpeqtmp24, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  %"%\D8\B3\D9\83\D8\A7\D9\86.load25" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmpeqtmp26 = icmp eq i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load25", 83
  br i1 %cmpeqtmp26, label %then_64, label %merge_65

merge_65:                                         ; preds = %merge_63
  %"%\D8\B3\D9\83\D8\A7\D9\86.load27" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load27", 58
  br i1 %cmplttmp, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  ret i64 0

then_36:                                          ; preds = %entry
  ret i64 0

then_38:                                          ; preds = %merge_37
  %"%\D8\B3\D9\83\D8\A7\D9\86.load1" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %subtmp = sub i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load1", 128
  store i64 %subtmp, ptr %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1", align 8
  %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1.load" = load i64, ptr %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1", align 8
  %cmpeqtmp2 = icmp eq i64 %"%\D9\83\D9\88\D8\AF_\D8\AA\D8\AD\D8\B1\D9\8A\D8\B1.load", 42
  br i1 %cmpeqtmp2, label %then_40, label %merge_41

then_40:                                          ; preds = %then_38
  br label %merge_41

then_42:                                          ; preds = %merge_41
  br label %merge_43

then_44:                                          ; preds = %merge_43
  br label %merge_45

then_46:                                          ; preds = %merge_39
  ret i64 0

then_48:                                          ; preds = %merge_47
  ret i64 0

then_50:                                          ; preds = %merge_49
  ret i64 0

then_52:                                          ; preds = %merge_51
  ret i64 -1

then_54:                                          ; preds = %merge_53
  ret i64 -2

then_56:                                          ; preds = %merge_55
  ret i64 -3

then_58:                                          ; preds = %merge_57
  ret i64 -4

then_60:                                          ; preds = %merge_59
  ret i64 -5

then_62:                                          ; preds = %merge_61
  ret i64 -6

then_64:                                          ; preds = %merge_63
  ret i64 -7

then_66:                                          ; preds = %merge_65
  br i1 false, label %then_68, label %else_70

then_68:                                          ; preds = %then_66
  %"%\D8\B3\D9\83\D8\A7\D9\86.load28" = load i64, ptr %"%\D8\B3\D9\83\D8\A7\D9\86", align 8
  %arr.data = load ptr, ptr getelementptr inbounds (%SadArray, ptr null, i32 0, i32 2), align 8
  %arr.elem = getelementptr i64, ptr %arr.data, i64 %"%\D8\B3\D9\83\D8\A7\D9\86.load28"
  %arr.get = load i64, ptr %arr.elem, align 8
  ret i64 %arr.get
}

define i64 @"kb_\D8\A7\D9\86\D8\AA\D8\B8\D8\B1_\D9\85\D9\81\D8\AA\D8\A7\D8\AD"() {
entry:
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD" = alloca i64, align 8
  store i64 0, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br label %while_cond_71

while_body_72:                                    ; preds = %while_cond_71
  %calltmp = call i64 @"kb_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D9\81\D8\AA\D8\A7\D8\AD"()
  store i64 %calltmp, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  br label %while_cond_71

while_cond_71:                                    ; preds = %while_body_72, %entry
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load", 0
  br i1 %cmpeqtmp, label %while_body_72, label %while_exit_73

while_exit_73:                                    ; preds = %while_cond_71
  %loadtmp = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  ret i64 %loadtmp
}

define i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\B5\D8\AF\D9\81\D8\A9"() {
entry:
  br i1 true, label %then_74, label %merge_75

merge_75:                                         ; preds = %entry
  br i1 true, label %then_76, label %merge_77

merge_77:                                         ; preds = %merge_75
  br i1 true, label %then_78, label %merge_79

merge_79:                                         ; preds = %merge_77
  br i1 true, label %then_80, label %merge_81

merge_81:                                         ; preds = %merge_79
  ret i64 0

then_74:                                          ; preds = %entry
  ret i64 -1

then_76:                                          ; preds = %merge_75
  ret i64 -1

then_78:                                          ; preds = %merge_77
  ret i64 -1

then_80:                                          ; preds = %merge_79
  ret i64 -1
}

define void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\AD\D8\AB"() {
entry:
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 91)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 98)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 93)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 62)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  ret void
}

define void @"\D8\A3\D8\B9\D8\AF_\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\A3\D9\85\D8\B1"(i64 %"\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9") {
entry:
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81" = alloca i64, align 8
  %"%\D8\B2\D9\88\D8\AC" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"%addr" = alloca i64, align 8
  %"%\D8\B9" = alloca i64, align 8
  %"\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", ptr %"\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  %"%253.load" = load i64, ptr %"\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A91", align 8
  store i64 %"%253.load", ptr %"%\D8\B9", align 8
  br label %while_cond_82

merge_89:                                         ; preds = %then_88, %while_body_86
  %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81.load")
  %"%\D9\81.load12" = load i64, ptr %"%\D9\81", align 8
  %addtmp13 = add i64 %"%\D9\81.load12", 1
  store i64 %addtmp13, ptr %"%\D9\81", align 8
  br label %while_cond_85

then_88:                                          ; preds = %while_body_86
  %"%\D8\B2\D9\88\D8\AC.load11" = load i64, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %shrtmp = lshr i64 %"%\D8\B2\D9\88\D8\AC.load11", 8
  store i64 %shrtmp, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  br label %merge_89

while_body_83:                                    ; preds = %while_cond_82
  %"%\D8\B9.load2" = load i64, ptr %"%\D8\B9", align 8
  %addtmp = add i64 0, %"%\D8\B9.load2"
  %shltmp = shl i64 %addtmp, 1
  %addtmp3 = add i64 0, %shltmp
  store i64 %addtmp3, ptr %"%addr", align 8
  %"%addr.load" = load i64, ptr %"%addr", align 8
  call void @sad_ll_mem_write16(i64 %"%addr.load", i64 32)
  %"%\D8\B9.load4" = load i64, ptr %"%\D8\B9", align 8
  %addtmp5 = add i64 %"%\D8\B9.load4", 1
  store i64 %addtmp5, ptr %"%\D8\B9", align 8
  br label %while_cond_82

while_body_86:                                    ; preds = %while_cond_85
  %"%\D9\81.load7" = load i64, ptr %"%\D9\81", align 8
  %addtmp8 = add i64 0, %"%\D9\81.load7"
  store i64 %addtmp8, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %andtmp = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load9" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9", align 8
  %subtmp = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\82\D8\B1\D8\A7\D8\A1\D8\A9.load9", %andtmp
  call void @sad_ll_mem_read16(i64 %subtmp)
  store i64 0, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %"%\D8\B2\D9\88\D8\AC.load" = load i64, ptr %"%\D8\B2\D9\88\D8\AC", align 8
  %andtmp10 = and i64 %"%\D8\B2\D9\88\D8\AC.load", 255
  store i64 %andtmp10, ptr %"%\D8\A7\D9\84\D8\AD\D8\B1\D9\81", align 8
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_88, label %merge_89

while_cond_82:                                    ; preds = %while_body_83, %entry
  %"%\D8\B9.load" = load i64, ptr %"%\D8\B9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9.load", 0
  br i1 %cmplttmp, label %while_body_83, label %while_exit_84

while_cond_85:                                    ; preds = %merge_89, %while_exit_84
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp6 = icmp slt i64 %"%\D9\81.load", 0
  br i1 %cmplttmp6, label %while_body_86, label %while_exit_87

while_exit_84:                                    ; preds = %while_cond_82
  store i64 0, ptr %"%\D9\81", align 8
  br label %while_cond_85

while_exit_87:                                    ; preds = %while_cond_85
  call void @"vga_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\85\D8\A4\D8\B4\D8\B1"()
  ret void
}

define void @"\D8\A3\D8\B6\D9\81_\D9\84\D9\84\D8\B3\D8\AC\D9\84"() {
entry:
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF\D8\AE\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  br i1 true, label %then_90, label %merge_91

merge_91:                                         ; preds = %entry
  store i64 poison, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3", align 8
  %multmp = mul i64 %"%\D9\81\D9\87\D8\B1\D8\B3.load", 0
  %addtmp = add i64 0, %multmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  ret void

then_90:                                          ; preds = %entry
  ret void
}

define void @"\D8\A7\D8\B3\D8\AA\D8\B9\D8\AF_\D9\85\D9\86_\D8\A7\D9\84\D8\B3\D8\AC\D9\84"(i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\AC\D9\84") {
entry:
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AE\D8\B2\D9\86" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF\D8\AE\D9\84\D8\A9" = alloca i64, align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\AD\D9\82\D9\8A\D9\82\D9\8A" = alloca i64, align 8
  %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\AC\D9\841" = alloca i64, align 8
  store i64 %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\AC\D9\84", ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\AC\D9\841", align 8
  br i1 true, label %then_92, label %merge_93

merge_93:                                         ; preds = %entry
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\AC\D9\84.load" = load i64, ptr %"\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\AC\D9\841", align 8
  %modtmp = srem i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\A7\D9\84\D8\B3\D8\AC\D9\84.load", 0
  store i64 %modtmp, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\AD\D9\82\D9\8A\D9\82\D9\8A", align 8
  %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\AD\D9\82\D9\8A\D9\82\D9\8A.load" = load i64, ptr %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\AD\D9\82\D9\8A\D9\82\D9\8A", align 8
  %multmp = mul i64 %"%\D9\81\D9\87\D8\B1\D8\B3_\D8\AD\D9\82\D9\8A\D9\82\D9\8A.load", 0
  %addtmp = add i64 0, %multmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF\D8\AE\D9\84\D8\A9", align 8
  %addtmp2 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF\D8\AE\D9\84\D8\A9.load", 0
  %subtmp = sub i64 %addtmp2, 2
  call void @sad_ll_mem_read16(i64 %subtmp)
  store i64 0, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AE\D8\B2\D9\86", align 8
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AE\D8\B2\D9\86.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AE\D8\B2\D9\86", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AE\D8\B2\D9\86.load", -2
  br i1 %cmpgttmp, label %then_94, label %merge_95

merge_95:                                         ; preds = %then_94, %merge_93
  ret void

then_92:                                          ; preds = %entry
  ret void

then_94:                                          ; preds = %merge_93
  store i64 -2, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AE\D8\B2\D9\86", align 8
  br label %merge_95
}

define void @"cmd_\D8\A3\D8\AF\D8\AE\D9\84_\D8\AD\D8\B1\D9\81"(i64 %"\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\81") {
entry:
  %"%pair_dst" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_src" = alloca i64, align 8
  %"%pair_src" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src" = alloca i64, align 8
  %"%pair" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  %"\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\811" = alloca i64, align 8
  store i64 %"\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\81", ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\811", align 8
  br i1 true, label %then_96, label %merge_97

else_107:                                         ; preds = %merge_104
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load22" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %subtmp23 = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load22", 1
  %"%pair_dst.load24" = load i64, ptr %"%pair_dst", align 8
  %andtmp25 = and i64 %"%pair_dst.load24", 255
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load26" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  %shltmp = shl i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load26", 8
  %addtmp27 = add i64 %andtmp25, %shltmp
  call void @sad_ll_mem_write16(i64 %subtmp23, i64 %addtmp27)
  br label %merge_106

else_110:                                         ; preds = %merge_99
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load35" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %subtmp36 = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load35", 1
  %"%pair.load37" = load i64, ptr %"%pair", align 8
  %andtmp38 = and i64 %"%pair.load37", 255
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\81.load39" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\811", align 8
  %shltmp40 = shl i64 %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\81.load39", 8
  %addtmp41 = add i64 %andtmp38, %shltmp40
  call void @sad_ll_mem_write16(i64 %subtmp36, i64 %addtmp41)
  br label %merge_109

merge_104:                                        ; preds = %then_103, %while_body_101
  %"%\D9\81.load11" = load i64, ptr %"%\D9\81", align 8
  %addtmp12 = add i64 0, %"%\D9\81.load11"
  store i64 %addtmp12, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %andtmp13 = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load14" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %subtmp15 = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load14", %andtmp13
  call void @sad_ll_mem_read16(i64 %subtmp15)
  store i64 0, ptr %"%pair_dst", align 8
  %cmpeqtmp16 = icmp eq i64 %andtmp13, 0
  br i1 %cmpeqtmp16, label %then_105, label %else_107

merge_106:                                        ; preds = %else_107, %then_105
  %"%\D9\81.load28" = load i64, ptr %"%\D9\81", align 8
  %subtmp29 = sub i64 %"%\D9\81.load28", 1
  store i64 %subtmp29, ptr %"%\D9\81", align 8
  br label %while_cond_100

merge_109:                                        ; preds = %else_110, %then_108
  ret void

merge_97:                                         ; preds = %entry
  br i1 false, label %then_98, label %merge_99

merge_99:                                         ; preds = %while_exit_102, %merge_97
  store i64 0, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %andtmp = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load2" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  %subtmp = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load2", %andtmp
  call void @sad_ll_mem_read16(i64 %subtmp)
  store i64 0, ptr %"%pair", align 8
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_108, label %else_110

then_103:                                         ; preds = %while_body_101
  %"%pair_src.load10" = load i64, ptr %"%pair_src", align 8
  %shrtmp = lshr i64 %"%pair_src.load10", 8
  store i64 %shrtmp, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  br label %merge_104

then_105:                                         ; preds = %merge_104
  %"%pair_dst.load" = load i64, ptr %"%pair_dst", align 8
  %andtmp17 = and i64 %"%pair_dst.load", 255
  %"%pair_dst.load18" = load i64, ptr %"%pair_dst", align 8
  %subtmp19 = sub i64 %"%pair_dst.load18", %andtmp17
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  %addtmp20 = add i64 %subtmp19, %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load21" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load21", i64 %addtmp20)
  br label %merge_106

then_108:                                         ; preds = %merge_99
  %"%pair.load" = load i64, ptr %"%pair", align 8
  %andtmp30 = and i64 %"%pair.load", 255
  %"%pair.load31" = load i64, ptr %"%pair", align 8
  %subtmp32 = sub i64 %"%pair.load31", %andtmp30
  %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\81.load" = load i64, ptr %"\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\811", align 8
  %addtmp33 = add i64 %subtmp32, %"%\D9\82\D9\8A\D9\85\D8\A9_\D8\AD\D8\B1\D9\81.load"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load34" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\83\D8\AA\D8\A7\D8\A8\D8\A9.load34", i64 %addtmp33)
  br label %merge_109

then_96:                                          ; preds = %entry
  ret void

then_98:                                          ; preds = %merge_97
  br label %while_cond_100

while_body_101:                                   ; preds = %while_cond_100
  %"%\D9\81.load3" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 0, %"%\D9\81.load3"
  %subtmp4 = sub i64 %addtmp, 1
  store i64 %subtmp4, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src", align 8
  %andtmp5 = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load6" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src", align 8
  %subtmp7 = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load6", %andtmp5
  call void @sad_ll_mem_read16(i64 %subtmp7)
  store i64 0, ptr %"%pair_src", align 8
  %"%pair_src.load" = load i64, ptr %"%pair_src", align 8
  %andtmp8 = and i64 %"%pair_src.load", 255
  store i64 %andtmp8, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  %cmpeqtmp9 = icmp eq i64 %andtmp5, 1
  br i1 %cmpeqtmp9, label %then_103, label %merge_104

while_cond_100:                                   ; preds = %merge_106, %then_98
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\81.load", 0
  br i1 %cmpgttmp, label %while_body_101, label %while_exit_102

while_exit_102:                                   ; preds = %while_cond_100
  br label %merge_99
}

define void @"cmd_\D9\85\D8\B3\D8\AD_\D8\AE\D9\84\D9\81\D9\8A"() {
entry:
  %"%pair_dst" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_src" = alloca i64, align 8
  %"%pair_src" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src" = alloca i64, align 8
  %"%\D9\81" = alloca i64, align 8
  br i1 true, label %then_111, label %merge_112

else_120:                                         ; preds = %merge_117
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load17" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %subtmp18 = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load17", 1
  %"%pair_dst.load19" = load i64, ptr %"%pair_dst", align 8
  %andtmp20 = and i64 %"%pair_dst.load19", 255
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load21" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  %shltmp = shl i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load21", 8
  %addtmp22 = add i64 %andtmp20, %shltmp
  call void @sad_ll_mem_write16(i64 %subtmp18, i64 %addtmp22)
  br label %merge_119

merge_112:                                        ; preds = %entry
  store i64 -1, ptr %"%\D9\81", align 8
  br label %while_cond_113

merge_117:                                        ; preds = %then_116, %while_body_114
  %"%\D9\81.load6" = load i64, ptr %"%\D9\81", align 8
  %addtmp7 = add i64 0, %"%\D9\81.load6"
  store i64 %addtmp7, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %andtmp8 = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load9" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  %subtmp10 = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load9", %andtmp8
  call void @sad_ll_mem_read16(i64 %subtmp10)
  store i64 0, ptr %"%pair_dst", align 8
  %cmpeqtmp11 = icmp eq i64 %andtmp8, 0
  br i1 %cmpeqtmp11, label %then_118, label %else_120

merge_119:                                        ; preds = %else_120, %then_118
  %"%\D9\81.load23" = load i64, ptr %"%\D9\81", align 8
  %addtmp24 = add i64 %"%\D9\81.load23", 1
  store i64 %addtmp24, ptr %"%\D9\81", align 8
  br label %while_cond_113

then_111:                                         ; preds = %entry
  ret void

then_116:                                         ; preds = %while_body_114
  %"%pair_src.load5" = load i64, ptr %"%pair_src", align 8
  %shrtmp = lshr i64 %"%pair_src.load5", 8
  store i64 %shrtmp, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%pair_dst.load" = load i64, ptr %"%pair_dst", align 8
  %andtmp12 = and i64 %"%pair_dst.load", 255
  %"%pair_dst.load13" = load i64, ptr %"%pair_dst", align 8
  %subtmp14 = sub i64 %"%pair_dst.load13", %andtmp12
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  %addtmp15 = add i64 %subtmp14, %"%\D8\A8\D8\A7\D9\8A\D8\AA_src.load"
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load16" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst", align 8
  call void @sad_ll_mem_write16(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_dst.load16", i64 %addtmp15)
  br label %merge_119

while_body_114:                                   ; preds = %while_cond_113
  %"%\D9\81.load1" = load i64, ptr %"%\D9\81", align 8
  %addtmp = add i64 0, %"%\D9\81.load1"
  %addtmp2 = add i64 %addtmp, 1
  store i64 %addtmp2, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src", align 8
  %andtmp = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load3" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src", align 8
  %subtmp = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_src.load3", %andtmp
  call void @sad_ll_mem_read16(i64 %subtmp)
  store i64 0, ptr %"%pair_src", align 8
  %"%pair_src.load" = load i64, ptr %"%pair_src", align 8
  %andtmp4 = and i64 %"%pair_src.load", 255
  store i64 %andtmp4, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_src", align 8
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_116, label %merge_117

while_cond_113:                                   ; preds = %merge_119, %merge_112
  %"%\D9\81.load" = load i64, ptr %"%\D9\81", align 8
  %cmplttmp = icmp slt i64 %"%\D9\81.load", -1
  br i1 %cmplttmp, label %while_body_114, label %while_exit_115

while_exit_115:                                   ; preds = %while_cond_113
  ret void
}

define i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 %"\D8\AD\D8\B1\D9\810", i64 %"\D8\AD\D8\B1\D9\811", i64 %"\D8\AD\D8\B1\D9\812", i64 %"\D8\AD\D8\B1\D9\813", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9") {
entry:
  %"%\D8\A83" = alloca i64, align 8
  %"%\D8\A82" = alloca i64, align 8
  %"%\D8\A81" = alloca i64, align 8
  %"%\D8\A80" = alloca i64, align 8
  %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A95" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A95", align 8
  %"\D8\AD\D8\B1\D9\8134" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\813", ptr %"\D8\AD\D8\B1\D9\8134", align 8
  %"\D8\AD\D8\B1\D9\8123" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\812", ptr %"\D8\AD\D8\B1\D9\8123", align 8
  %"\D8\AD\D8\B1\D9\8112" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\811", ptr %"\D8\AD\D8\B1\D9\8112", align 8
  %"\D8\AD\D8\B1\D9\8101" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\810", ptr %"\D8\AD\D8\B1\D9\8101", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A95", align 8
  %cmplttmp = icmp slt i64 0, %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load"
  br i1 %cmplttmp, label %then_121, label %merge_122

merge_122:                                        ; preds = %entry
  call void @sad_ll_mem_read16(i64 2)
  store i64 0, ptr %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC", align 8
  %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load" = load i64, ptr %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC", align 8
  %andtmp = and i64 %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load", 255
  store i64 %andtmp, ptr %"%\D8\A80", align 8
  %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load6" = load i64, ptr %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC", align 8
  %shrtmp = lshr i64 %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load6", 8
  store i64 %shrtmp, ptr %"%\D8\A81", align 8
  %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC.load" = load i64, ptr %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC", align 8
  %andtmp7 = and i64 %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC.load", 255
  store i64 %andtmp7, ptr %"%\D8\A82", align 8
  %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC.load8" = load i64, ptr %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC", align 8
  %shrtmp9 = lshr i64 %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC.load8", 8
  store i64 %shrtmp9, ptr %"%\D8\A83", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load10" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A95", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load10", 1
  br i1 %cmpgetmp, label %then_123, label %merge_124

merge_124:                                        ; preds = %merge_126, %merge_122
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load11" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A95", align 8
  %cmpgetmp12 = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load11", 2
  br i1 %cmpgetmp12, label %then_127, label %merge_128

merge_126:                                        ; preds = %then_123
  br label %merge_124

merge_128:                                        ; preds = %merge_130, %merge_124
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load14" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A95", align 8
  %cmpgetmp15 = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load14", 3
  br i1 %cmpgetmp15, label %then_131, label %merge_132

merge_130:                                        ; preds = %then_127
  br label %merge_128

merge_132:                                        ; preds = %merge_134, %merge_128
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load17" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A95", align 8
  %cmpgetmp18 = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\85\D9\82\D8\A7\D8\B1\D9\86\D8\A9.load17", 4
  br i1 %cmpgetmp18, label %then_135, label %merge_136

merge_134:                                        ; preds = %then_131
  br label %merge_132

merge_136:                                        ; preds = %merge_138, %merge_132
  ret i64 1

merge_138:                                        ; preds = %then_135
  br label %merge_136

then_121:                                         ; preds = %entry
  ret i64 0

then_123:                                         ; preds = %merge_122
  %"%\D8\A80.load" = load i64, ptr %"%\D8\A80", align 8
  %"%\D8\AD\D8\B1\D9\810.load" = load i64, ptr %"\D8\AD\D8\B1\D9\8101", align 8
  %cmpnetmp = icmp ne i64 %"%\D8\A80.load", %"%\D8\AD\D8\B1\D9\810.load"
  br i1 %cmpnetmp, label %then_125, label %merge_126

then_125:                                         ; preds = %then_123
  ret i64 0

then_127:                                         ; preds = %merge_124
  %"%\D8\A81.load" = load i64, ptr %"%\D8\A81", align 8
  %"%\D8\AD\D8\B1\D9\811.load" = load i64, ptr %"\D8\AD\D8\B1\D9\8112", align 8
  %cmpnetmp13 = icmp ne i64 %"%\D8\A81.load", %"%\D8\AD\D8\B1\D9\811.load"
  br i1 %cmpnetmp13, label %then_129, label %merge_130

then_129:                                         ; preds = %then_127
  ret i64 0

then_131:                                         ; preds = %merge_128
  %"%\D8\A82.load" = load i64, ptr %"%\D8\A82", align 8
  %"%\D8\AD\D8\B1\D9\812.load" = load i64, ptr %"\D8\AD\D8\B1\D9\8123", align 8
  %cmpnetmp16 = icmp ne i64 %"%\D8\A82.load", %"%\D8\AD\D8\B1\D9\812.load"
  br i1 %cmpnetmp16, label %then_133, label %merge_134

then_133:                                         ; preds = %then_131
  ret i64 0

then_135:                                         ; preds = %merge_132
  %"%\D8\A83.load" = load i64, ptr %"%\D8\A83", align 8
  %"%\D8\AD\D8\B1\D9\813.load" = load i64, ptr %"\D8\AD\D8\B1\D9\8134", align 8
  %cmpnetmp19 = icmp ne i64 %"%\D8\A83.load", %"%\D8\AD\D8\B1\D9\813.load"
  br i1 %cmpnetmp19, label %then_137, label %merge_138

then_137:                                         ; preds = %then_135
  ret i64 0
}

define i64 @"cmd_\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8\D9\80"(i64 %"\D8\AD\D8\B1\D9\810", i64 %"\D8\AD\D8\B1\D9\811", i64 %"\D8\AD\D8\B1\D9\812", i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9") {
entry:
  %"%pair_\D8\A8\D8\B9\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF" = alloca i64, align 8
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A8\D8\B9\D8\AF" = alloca i64, align 8
  %"%\D8\A82" = alloca i64, align 8
  %"%\D8\A81" = alloca i64, align 8
  %"%\D8\A80" = alloca i64, align 8
  %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC" = alloca i64, align 8
  %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC" = alloca i64, align 8
  %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94" = alloca i64, align 8
  store i64 %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9", ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %"\D8\AD\D8\B1\D9\8123" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\812", ptr %"\D8\AD\D8\B1\D9\8123", align 8
  %"\D8\AD\D8\B1\D9\8112" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\811", ptr %"\D8\AD\D8\B1\D9\8112", align 8
  %"\D8\AD\D8\B1\D9\8101" = alloca i64, align 8
  store i64 %"\D8\AD\D8\B1\D9\810", ptr %"\D8\AD\D8\B1\D9\8101", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %cmpletmp = icmp sle i64 0, %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load"
  br i1 %cmpletmp, label %then_139, label %merge_140

else_157:                                         ; preds = %merge_152
  %"%pair_\D8\A8\D8\B9\D8\AF.load20" = load i64, ptr %"%pair_\D8\A8\D8\B9\D8\AF", align 8
  %shrtmp21 = lshr i64 %"%pair_\D8\A8\D8\B9\D8\AF.load20", 8
  store i64 %shrtmp21, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A8\D8\B9\D8\AF", align 8
  br label %merge_156

merge_140:                                        ; preds = %entry
  store i64 0, ptr %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load5" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %cmpgetmp = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load5", 3
  br i1 %cmpgetmp, label %then_141, label %merge_142

merge_142:                                        ; preds = %then_141, %merge_140
  %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load" = load i64, ptr %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC", align 8
  %andtmp = and i64 %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load", 255
  store i64 %andtmp, ptr %"%\D8\A80", align 8
  %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load6" = load i64, ptr %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC", align 8
  %shrtmp = lshr i64 %"%\D8\A3\D9\88\D9\84_\D8\B2\D9\88\D8\AC.load6", 8
  store i64 %shrtmp, ptr %"%\D8\A81", align 8
  %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC.load" = load i64, ptr %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC", align 8
  %andtmp7 = and i64 %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC.load", 255
  store i64 %andtmp7, ptr %"%\D8\A82", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load8" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %cmpgetmp9 = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load8", 1
  br i1 %cmpgetmp9, label %then_143, label %merge_144

merge_144:                                        ; preds = %merge_146, %merge_142
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load10" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %cmpgetmp11 = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load10", 2
  br i1 %cmpgetmp11, label %then_147, label %merge_148

merge_146:                                        ; preds = %then_143
  br label %merge_144

merge_148:                                        ; preds = %merge_150, %merge_144
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load13" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %cmpgetmp14 = icmp sge i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load13", 3
  br i1 %cmpgetmp14, label %then_151, label %merge_152

merge_150:                                        ; preds = %then_147
  br label %merge_148

merge_152:                                        ; preds = %merge_154, %merge_148
  store i64 0, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A8\D8\B9\D8\AF", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load16" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %addtmp = add i64 0, %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load16"
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF", align 8
  %andtmp17 = and i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF.load", 1
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF.load18" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF", align 8
  %subtmp = sub i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A8\D8\B9\D8\AF.load18", %andtmp17
  call void @sad_ll_mem_read16(i64 %subtmp)
  store i64 0, ptr %"%pair_\D8\A8\D8\B9\D8\AF", align 8
  %cmpeqtmp = icmp eq i64 %andtmp17, 0
  br i1 %cmpeqtmp, label %then_155, label %else_157

merge_154:                                        ; preds = %then_151
  br label %merge_152

merge_156:                                        ; preds = %else_157, %then_155
  %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A8\D8\B9\D8\AF.load" = load i64, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A8\D8\B9\D8\AF", align 8
  %cmpeqtmp22 = icmp eq i64 %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A8\D8\B9\D8\AF.load", 32
  br i1 %cmpeqtmp22, label %then_158, label %merge_159

merge_159:                                        ; preds = %merge_156
  ret i64 0

then_139:                                         ; preds = %entry
  ret i64 0

then_141:                                         ; preds = %merge_140
  call void @sad_ll_mem_read16(i64 2)
  store i64 0, ptr %"%\D8\AB\D8\A7\D9\86\D9\8A_\D8\B2\D9\88\D8\AC", align 8
  br label %merge_142

then_143:                                         ; preds = %merge_142
  %"%\D8\A80.load" = load i64, ptr %"%\D8\A80", align 8
  %"%\D8\AD\D8\B1\D9\810.load" = load i64, ptr %"\D8\AD\D8\B1\D9\8101", align 8
  %cmpnetmp = icmp ne i64 %"%\D8\A80.load", %"%\D8\AD\D8\B1\D9\810.load"
  br i1 %cmpnetmp, label %then_145, label %merge_146

then_145:                                         ; preds = %then_143
  ret i64 0

then_147:                                         ; preds = %merge_144
  %"%\D8\A81.load" = load i64, ptr %"%\D8\A81", align 8
  %"%\D8\AD\D8\B1\D9\811.load" = load i64, ptr %"\D8\AD\D8\B1\D9\8112", align 8
  %cmpnetmp12 = icmp ne i64 %"%\D8\A81.load", %"%\D8\AD\D8\B1\D9\811.load"
  br i1 %cmpnetmp12, label %then_149, label %merge_150

then_149:                                         ; preds = %then_147
  ret i64 0

then_151:                                         ; preds = %merge_148
  %"%\D8\A82.load" = load i64, ptr %"%\D8\A82", align 8
  %"%\D8\AD\D8\B1\D9\812.load" = load i64, ptr %"\D8\AD\D8\B1\D9\8123", align 8
  %cmpnetmp15 = icmp ne i64 %"%\D8\A82.load", %"%\D8\AD\D8\B1\D9\812.load"
  br i1 %cmpnetmp15, label %then_153, label %merge_154

then_153:                                         ; preds = %then_151
  ret i64 0

then_155:                                         ; preds = %merge_152
  %"%pair_\D8\A8\D8\B9\D8\AF.load" = load i64, ptr %"%pair_\D8\A8\D8\B9\D8\AF", align 8
  %andtmp19 = and i64 %"%pair_\D8\A8\D8\B9\D8\AF.load", 255
  store i64 %andtmp19, ptr %"%\D8\A8\D8\A7\D9\8A\D8\AA_\D8\A8\D8\B9\D8\AF", align 8
  br label %merge_156

then_158:                                         ; preds = %merge_156
  %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load23" = load i64, ptr %"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A94", align 8
  %addtmp24 = add i64 %"%\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D8\A8\D8\A7\D8\AF\D8\A6\D8\A9.load23", 1
  ret i64 %addtmp24
}

define i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\B3\D9\8A\D8\B7"(i64 %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9") {
entry:
  %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", ptr %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A91", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A91", align 8
  %addtmp = add i64 0, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  ret i64 %addtmp
}

define i64 @"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\88\D8\B3\D9\8A\D8\B7"(i64 %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9") {
entry:
  %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A91" = alloca i64, align 8
  store i64 %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", ptr %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A91", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"\D8\A5\D8\B2\D8\A7\D8\AD\D8\A91", align 8
  %subtmp = sub i64 0, %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load"
  ret i64 %subtmp
}

define void @"\D8\A3\D9\85\D8\B1_\D9\85\D8\B3\D8\AD"() {
entry:
  call void @"vga_\D9\85\D8\B3\D8\AD"()
  ret void
}

define void @"\D8\A3\D9\85\D8\B1_\D9\85\D8\B3\D8\A7\D8\B9\D8\AF\D8\A9"() {
entry:
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 66)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 79)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 83)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 67)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 109)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 109)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 97)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 110)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 115)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 58)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 115)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 45)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 105)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 115)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 116)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 99)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 68)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 73)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 82)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 99)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 97)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 116)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 70)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 73)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 76)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 69)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 109)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 107)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 105)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 44)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 109)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 44)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 112)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 119)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 44)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 99)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 104)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 44)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 99)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 97)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 44)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 104)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 112)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void
}

define void @"\D8\A3\D9\85\D8\B1_pwd"() {
entry:
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void
}

define void @"\D8\A3\D9\85\D8\B1_echo"() {
entry:
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"cmd_\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8\D9\80"(i64 101, i64 99, i64 104, i64 4)
  store i64 %calltmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load", 0
  br i1 %cmpgttmp, label %then_160, label %merge_161

merge_161:                                        ; preds = %merge_163, %entry
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void

merge_163:                                        ; preds = %then_162, %then_160
  br label %merge_161

then_160:                                         ; preds = %entry
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load1" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmpletmp = icmp sle i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load1", 0
  br i1 %cmpletmp, label %then_162, label %merge_163

then_162:                                         ; preds = %then_160
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load2" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %calltmp3 = call i64 @"\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D8\A7\D9\84\D9\88\D8\B3\D9\8A\D8\B7"(i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load2")
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load4" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %calltmp5 = call i64 @"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\88\D8\B3\D9\8A\D8\B7"(i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load4")
  call void @"vga_\D9\86\D8\B5"(i64 %calltmp3, i64 %calltmp5)
  br label %merge_163
}

define void @"\D8\A3\D9\85\D8\B1_ls"() {
entry:
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF" = alloca i64, align 8
  %"%\D9\85" = alloca i64, align 8
  %"%\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp, label %then_164, label %merge_165

else_173:                                         ; preds = %while_body_169
  br label %merge_172

merge_165:                                        ; preds = %entry
  %"%\D8\B9\D8\AF\D8\AF.load" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %cmpletmp = icmp sle i64 %"%\D8\B9\D8\AF\D8\AF.load", 0
  br i1 %cmpletmp, label %then_166, label %merge_167

merge_167:                                        ; preds = %merge_165
  store i64 0, ptr %"%\D9\85", align 8
  br label %while_cond_168

merge_172:                                        ; preds = %else_173, %then_171
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load", 24
  br i1 %cmpgttmp, label %then_174, label %merge_175

merge_175:                                        ; preds = %then_174, %merge_172
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load8" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %addtmp9 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load8", 8
  %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load10" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF", align 8
  call void @"vga_\D9\86\D8\B5"(i64 %addtmp9, i64 %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF.load10")
  %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF.load11" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF", align 8
  %cmpeqtmp12 = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF.load11", 2
  br i1 %cmpeqtmp12, label %then_176, label %merge_177

merge_177:                                        ; preds = %then_176, %merge_175
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  %"%\D9\85.load13" = load i64, ptr %"%\D9\85", align 8
  %addtmp14 = add i64 %"%\D9\85.load13", 1
  store i64 %addtmp14, ptr %"%\D9\85", align 8
  br label %while_cond_168

then_164:                                         ; preds = %entry
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 69)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void

then_166:                                         ; preds = %merge_165
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 40)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 109)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 112)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 116)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 121)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 41)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void

then_171:                                         ; preds = %while_body_169
  br label %merge_172

then_174:                                         ; preds = %merge_172
  store i64 24, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF", align 8
  br label %merge_175

then_176:                                         ; preds = %merge_175
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 47)
  br label %merge_177

while_body_169:                                   ; preds = %while_cond_168
  %"%\D9\85.load3" = load i64, ptr %"%\D9\85", align 8
  %shltmp = shl i64 %"%\D9\85.load3", 5
  %addtmp = add i64 0, %shltmp
  store i64 %addtmp, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  call void @sad_ll_mem_read32(i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load")
  store i64 0, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9_\D9\85\D8\AF", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load4" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %addtmp5 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load4", 4
  call void @sad_ll_mem_read16(i64 %addtmp5)
  store i64 0, ptr %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF", align 8
  %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load6" = load i64, ptr %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF", align 8
  %addtmp7 = add i64 %"%\D8\B9\D9\86\D9\88\D8\A7\D9\86_\D9\85\D8\AF.load6", 6
  call void @sad_ll_mem_read16(i64 %addtmp7)
  store i64 0, ptr %"%\D8\B7\D9\88\D9\84_\D9\85\D8\AF", align 8
  %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF.load" = load i64, ptr %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\86\D9\88\D8\B9_\D9\85\D8\AF.load", 2
  br i1 %cmpeqtmp, label %then_171, label %else_173

while_cond_168:                                   ; preds = %merge_177, %merge_167
  %"%\D9\85.load" = load i64, ptr %"%\D9\85", align 8
  %"%\D8\B9\D8\AF\D8\AF.load1" = load i64, ptr %"%\D8\B9\D8\AF\D8\AF", align 8
  %cmplttmp2 = icmp slt i64 %"%\D9\85.load", %"%\D8\B9\D8\AF\D8\AF.load1"
  br i1 %cmplttmp2, label %while_body_169, label %while_exit_170

while_exit_170:                                   ; preds = %while_cond_168
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void
}

define void @"\D8\A3\D9\85\D8\B1_cat"() {
entry:
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1" = alloca i64, align 8
  %"%fd" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"cmd_\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8\D9\80"(i64 99, i64 97, i64 116, i64 3)
  store i64 %calltmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_178, label %merge_179

merge_179:                                        ; preds = %entry
  %"%fd.load" = load i64, ptr %"%fd", align 8
  %cmplttmp = icmp slt i64 %"%fd.load", 0
  br i1 %cmplttmp, label %then_180, label %merge_181

merge_181:                                        ; preds = %merge_179
  br label %while_cond_182

then_178:                                         ; preds = %entry
  ret void

then_180:                                         ; preds = %merge_179
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 69)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 58)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 110)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 116)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 102)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 117)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 110)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void

while_body_183:                                   ; preds = %while_cond_182
  br label %while_cond_182

while_cond_182:                                   ; preds = %while_body_183, %merge_181
  %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load" = load i64, ptr %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D9\82\D8\B1\D9\88\D8\A1.load", 0
  br i1 %cmpgttmp, label %while_body_183, label %while_exit_184

while_exit_184:                                   ; preds = %while_cond_182
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  %"%fd.load1" = load i64, ptr %"%fd", align 8
  call void @"\D8\A3\D8\BA\D9\84\D9\82"(i64 %"%fd.load1")
  ret void
}

define void @"\D8\A3\D9\85\D8\B1_cd"() {
entry:
  %"%\D8\B7\D9\88\D9\84_\D8\AC\D8\AF\D9\8A\D8\AF" = alloca i64, align 8
  %"%\D8\B9\D9\82\D8\AF\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"cmd_\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8\D9\80"(i64 99, i64 100, i64 0, i64 2)
  store i64 %calltmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_185, label %merge_186

merge_186:                                        ; preds = %entry
  %"%\D8\B9\D9\82\D8\AF\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\82\D8\AF\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\B9\D9\82\D8\AF\D8\A9.load", 0
  br i1 %cmplttmp, label %then_187, label %merge_188

merge_188:                                        ; preds = %merge_186
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load1" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %calltmp2 = call i64 @"\D8\B7\D9\88\D9\84_\D8\A7\D9\84\D9\88\D8\B3\D9\8A\D8\B7"(i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load1")
  store i64 %calltmp2, ptr %"%\D8\B7\D9\88\D9\84_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  %"%\D8\B7\D9\88\D9\84_\D8\AC\D8\AF\D9\8A\D8\AF.load" = load i64, ptr %"%\D8\B7\D9\88\D9\84_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  %cmpgttmp = icmp sgt i64 %"%\D8\B7\D9\88\D9\84_\D8\AC\D8\AF\D9\8A\D8\AF.load", 255
  br i1 %cmpgttmp, label %then_189, label %merge_190

merge_190:                                        ; preds = %then_189, %merge_188
  ret void

then_185:                                         ; preds = %entry
  ret void

then_187:                                         ; preds = %merge_186
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 69)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void

then_189:                                         ; preds = %merge_188
  store i64 255, ptr %"%\D8\B7\D9\88\D9\84_\D8\AC\D8\AF\D9\8A\D8\AF", align 8
  br label %merge_190
}

define void @"\D8\A3\D9\85\D8\B1_mkdir"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"cmd_\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8\D9\80"(i64 109, i64 107, i64 100, i64 5)
  store i64 %calltmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_191, label %merge_192

else_195:                                         ; preds = %merge_192
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 79)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 75)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  br label %merge_194

merge_192:                                        ; preds = %entry
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmplttmp, label %then_193, label %else_195

merge_194:                                        ; preds = %else_195, %then_193
  ret void

then_191:                                         ; preds = %entry
  ret void

then_193:                                         ; preds = %merge_192
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 69)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  br label %merge_194
}

define void @"\D8\A3\D9\85\D8\B1_rm"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"cmd_\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8\D9\80"(i64 114, i64 109, i64 0, i64 2)
  store i64 %calltmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_196, label %merge_197

else_200:                                         ; preds = %merge_197
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 79)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 75)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  br label %merge_199

merge_197:                                        ; preds = %entry
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmplttmp, label %then_198, label %else_200

merge_199:                                        ; preds = %else_200, %then_198
  ret void

then_196:                                         ; preds = %entry
  ret void

then_198:                                         ; preds = %merge_197
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 69)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  br label %merge_199
}

define void @"\D8\A3\D9\85\D8\B1_stat"() {
entry:
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9" = alloca i64, align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"cmd_\D9\8A\D8\A8\D8\AF\D8\A3_\D8\A8\D9\80"(i64 115, i64 116, i64 97, i64 4)
  store i64 %calltmp, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load" = load i64, ptr %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9", align 8
  %cmpeqtmp = icmp eq i64 %"%\D8\A5\D8\B2\D8\A7\D8\AD\D8\A9.load", 0
  br i1 %cmpeqtmp, label %then_201, label %merge_202

merge_202:                                        ; preds = %entry
  %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load" = load i64, ptr %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D9\86\D8\AA\D9\8A\D8\AC\D8\A9.load", 0
  br i1 %cmplttmp, label %then_203, label %merge_204

merge_204:                                        ; preds = %merge_202
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 105)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 110)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 58)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 116)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 121)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 112)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 58)
  call void @sad_ll_mem_read16(i64 4)
  call void @"vga_\D8\B1\D9\82\D9\85"(i64 0)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 115)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 105)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 122)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 58)
  call void @sad_ll_mem_read64(i64 8)
  call void @"vga_\D8\B1\D9\82\D9\85"(i64 0)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void

then_201:                                         ; preds = %entry
  ret void

then_203:                                         ; preds = %merge_202
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 69)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void
}

define void @"\D9\86\D9\81\D8\B0_\D8\A3\D9\85\D8\B1"() {
entry:
  br i1 true, label %then_205, label %merge_206

merge_206:                                        ; preds = %entry
  call void @"\D8\A3\D8\B6\D9\81_\D9\84\D9\84\D8\B3\D8\AC\D9\84"()
  %calltmp = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 108, i64 115, i64 0, i64 0, i64 2)
  %cmpeqtmp = icmp eq i64 %calltmp, 1
  br i1 %cmpeqtmp, label %then_207, label %merge_208

merge_208:                                        ; preds = %merge_210, %merge_206
  %calltmp1 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 99, i64 100, i64 0, i64 0, i64 2)
  %cmpeqtmp2 = icmp eq i64 %calltmp1, 1
  br i1 %cmpeqtmp2, label %then_211, label %merge_212

merge_210:                                        ; preds = %then_207
  br label %merge_208

merge_212:                                        ; preds = %merge_208
  %calltmp3 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 99, i64 97, i64 116, i64 0, i64 3)
  %cmpeqtmp4 = icmp eq i64 %calltmp3, 1
  br i1 %cmpeqtmp4, label %then_213, label %merge_214

merge_214:                                        ; preds = %merge_212
  %calltmp5 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 112, i64 119, i64 100, i64 0, i64 3)
  %cmpeqtmp6 = icmp eq i64 %calltmp5, 1
  br i1 %cmpeqtmp6, label %then_215, label %merge_216

merge_216:                                        ; preds = %merge_218, %merge_214
  %calltmp7 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 101, i64 99, i64 104, i64 111, i64 4)
  %cmpeqtmp8 = icmp eq i64 %calltmp7, 1
  br i1 %cmpeqtmp8, label %then_219, label %merge_220

merge_218:                                        ; preds = %then_215
  br label %merge_216

merge_220:                                        ; preds = %merge_216
  %calltmp9 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 99, i64 108, i64 101, i64 97, i64 5)
  %cmpeqtmp10 = icmp eq i64 %calltmp9, 1
  br i1 %cmpeqtmp10, label %then_221, label %merge_222

merge_222:                                        ; preds = %merge_220
  %calltmp11 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 104, i64 101, i64 108, i64 112, i64 4)
  %cmpeqtmp12 = icmp eq i64 %calltmp11, 1
  br i1 %cmpeqtmp12, label %then_223, label %merge_224

merge_224:                                        ; preds = %merge_222
  %calltmp13 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 109, i64 107, i64 100, i64 105, i64 5)
  %cmpeqtmp14 = icmp eq i64 %calltmp13, 1
  br i1 %cmpeqtmp14, label %then_225, label %merge_226

merge_226:                                        ; preds = %merge_224
  %calltmp15 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 114, i64 109, i64 0, i64 0, i64 2)
  %cmpeqtmp16 = icmp eq i64 %calltmp15, 1
  br i1 %cmpeqtmp16, label %then_227, label %merge_228

merge_228:                                        ; preds = %merge_226
  %calltmp17 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 115, i64 116, i64 97, i64 116, i64 4)
  %cmpeqtmp18 = icmp eq i64 %calltmp17, 1
  br i1 %cmpeqtmp18, label %then_229, label %merge_230

merge_230:                                        ; preds = %merge_228
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 58)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 110)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 116)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 102)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 117)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 110)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  ret void

then_205:                                         ; preds = %entry
  ret void

then_207:                                         ; preds = %merge_206
  br i1 false, label %then_209, label %merge_210

then_209:                                         ; preds = %then_207
  call void @"\D8\A3\D9\85\D8\B1_ls"()
  ret void

then_211:                                         ; preds = %merge_208
  call void @"\D8\A3\D9\85\D8\B1_cd"()
  ret void

then_213:                                         ; preds = %merge_212
  call void @"\D8\A3\D9\85\D8\B1_cat"()
  ret void

then_215:                                         ; preds = %merge_214
  br i1 false, label %then_217, label %merge_218

then_217:                                         ; preds = %then_215
  call void @"\D8\A3\D9\85\D8\B1_pwd"()
  ret void

then_219:                                         ; preds = %merge_216
  call void @"\D8\A3\D9\85\D8\B1_echo"()
  ret void

then_221:                                         ; preds = %merge_220
  call void @"\D8\A3\D9\85\D8\B1_\D9\85\D8\B3\D8\AD"()
  ret void

then_223:                                         ; preds = %merge_222
  call void @"\D8\A3\D9\85\D8\B1_\D9\85\D8\B3\D8\A7\D8\B9\D8\AF\D8\A9"()
  ret void

then_225:                                         ; preds = %merge_224
  call void @"\D8\A3\D9\85\D8\B1_mkdir"()
  ret void

then_227:                                         ; preds = %merge_226
  call void @"\D8\A3\D9\85\D8\B1_rm"()
  ret void

then_229:                                         ; preds = %merge_228
  call void @"\D8\A3\D9\85\D8\B1_stat"()
  ret void
}

define void @"\D8\B4\D8\BA\D9\84_\D8\A7\D9\84\D8\B5\D8\AF\D9\81\D8\A9"() {
entry:
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD" = alloca i64, align 8
  %"%\D9\85\D8\B3\D8\AA\D9\85\D8\B1" = alloca i64, align 8
  %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9" = alloca i64, align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D8\AA\D9\87\D9\8A\D8\A6\D8\A9" = alloca i64, align 8
  %calltmp = call i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\B5\D8\AF\D9\81\D8\A9"()
  store i64 %calltmp, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D8\AA\D9\87\D9\8A\D8\A6\D8\A9", align 8
  %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D8\AA\D9\87\D9\8A\D8\A6\D8\A9.load" = load i64, ptr %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D8\AA\D9\87\D9\8A\D8\A6\D8\A9", align 8
  %cmplttmp = icmp slt i64 %"%\D8\AD\D8\A7\D9\84\D8\A9_\D8\A7\D9\84\D8\AA\D9\87\D9\8A\D8\A6\D8\A9.load", 0
  br i1 %cmplttmp, label %then_231, label %merge_232

else_256:                                         ; preds = %then_252
  br label %merge_255

merge_232:                                        ; preds = %entry
  call void @"vga_\D9\85\D8\B3\D8\AD"()
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 61)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 61)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 61)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 66)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 79)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 83)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 83)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 104)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 118)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 49)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 46)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 48)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 61)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 61)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 61)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 84)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 121)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 112)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 39)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 104)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 101)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 112)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 39)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 102)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 114)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 32)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 99)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 111)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 109)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 109)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 97)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 110)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 100)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 115)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  br label %while_cond_233

merge_240:                                        ; preds = %while_body_237
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load3" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp4 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load3", 10
  br i1 %cmpeqtmp4, label %then_242, label %merge_243

merge_243:                                        ; preds = %then_242, %merge_240
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load5" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp6 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load5", 8
  br i1 %cmpeqtmp6, label %then_244, label %merge_245

merge_245:                                        ; preds = %then_244, %merge_243
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load7" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp8 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load7", -1
  br i1 %cmpeqtmp8, label %then_246, label %merge_247

merge_247:                                        ; preds = %merge_249, %merge_245
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load9" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp10 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load9", -2
  br i1 %cmpeqtmp10, label %then_250, label %merge_251

merge_249:                                        ; preds = %then_248, %then_246
  br label %merge_247

merge_251:                                        ; preds = %merge_253, %merge_247
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load12" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp13 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load12", -3
  br i1 %cmpeqtmp13, label %then_257, label %merge_258

merge_253:                                        ; preds = %merge_255, %then_250
  br label %merge_251

merge_255:                                        ; preds = %else_256, %then_254
  %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load14" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  call void @"\D8\A3\D8\B9\D8\AF_\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\A3\D9\85\D8\B1"(i64 %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load14")
  br label %merge_253

merge_258:                                        ; preds = %merge_260, %merge_251
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load15" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp16 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load15", -4
  br i1 %cmpeqtmp16, label %then_261, label %merge_262

merge_260:                                        ; preds = %then_259, %then_257
  br label %merge_258

merge_262:                                        ; preds = %merge_264, %merge_258
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load17" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp18 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load17", -5
  br i1 %cmpeqtmp18, label %then_265, label %merge_266

merge_264:                                        ; preds = %then_263, %then_261
  br label %merge_262

merge_266:                                        ; preds = %then_265, %merge_262
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load19" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp20 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load19", -6
  br i1 %cmpeqtmp20, label %then_267, label %merge_268

merge_268:                                        ; preds = %then_267, %merge_266
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load21" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpgttmp = icmp sgt i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load21", 31
  br i1 %cmpgttmp, label %then_269, label %merge_270

merge_270:                                        ; preds = %merge_272, %merge_268
  br label %while_cond_236

merge_272:                                        ; preds = %then_271, %then_269
  br label %merge_270

then_231:                                         ; preds = %entry
  ret void

then_239:                                         ; preds = %while_body_237
  br label %while_cond_236

then_242:                                         ; preds = %merge_240
  call void @"vga_\D8\AD\D8\B1\D9\81"(i64 10)
  call void @"\D9\86\D9\81\D8\B0_\D8\A3\D9\85\D8\B1"()
  store i64 0, ptr %"%\D9\85\D8\B3\D8\AA\D9\85\D8\B1", align 8
  br label %merge_243

then_244:                                         ; preds = %merge_243
  call void @"cmd_\D9\85\D8\B3\D8\AD_\D8\AE\D9\84\D9\81\D9\8A"()
  %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  call void @"\D8\A3\D8\B9\D8\AF_\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\A3\D9\85\D8\B1"(i64 %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load")
  br label %merge_245

then_246:                                         ; preds = %merge_245
  br i1 false, label %then_248, label %merge_249

then_248:                                         ; preds = %then_246
  %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load11" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  call void @"\D8\A3\D8\B9\D8\AF_\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\A3\D9\85\D8\B1"(i64 %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load11")
  br label %merge_249

then_250:                                         ; preds = %merge_247
  br i1 false, label %then_252, label %merge_253

then_252:                                         ; preds = %then_250
  br i1 true, label %then_254, label %else_256

then_254:                                         ; preds = %then_252
  br label %merge_255

then_257:                                         ; preds = %merge_251
  br i1 false, label %then_259, label %merge_260

then_259:                                         ; preds = %then_257
  call void @"vga_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\85\D8\A4\D8\B4\D8\B1"()
  br label %merge_260

then_261:                                         ; preds = %merge_258
  br i1 false, label %then_263, label %merge_264

then_263:                                         ; preds = %then_261
  call void @"vga_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\85\D8\A4\D8\B4\D8\B1"()
  br label %merge_264

then_265:                                         ; preds = %merge_262
  call void @"vga_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\85\D8\A4\D8\B4\D8\B1"()
  br label %merge_266

then_267:                                         ; preds = %merge_266
  call void @"vga_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\85\D8\A4\D8\B4\D8\B1"()
  br label %merge_268

then_269:                                         ; preds = %merge_268
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load22" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmplttmp23 = icmp slt i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load22", 127
  br i1 %cmplttmp23, label %then_271, label %merge_272

then_271:                                         ; preds = %then_269
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load24" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  call void @"cmd_\D8\A3\D8\AF\D8\AE\D9\84_\D8\AD\D8\B1\D9\81"(i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load24")
  %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load25" = load i64, ptr %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  call void @"\D8\A3\D8\B9\D8\AF_\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D8\A3\D9\85\D8\B1"(i64 %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9.load25")
  br label %merge_272

while_body_234:                                   ; preds = %while_cond_233
  call void @"\D8\A7\D8\B1\D8\B3\D9\85_\D8\A7\D9\84\D9\85\D8\AD\D8\AB"()
  store i64 0, ptr %"%\D8\B9\D9\85\D9\88\D8\AF_\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9", align 8
  call void @"vga_\D8\AA\D8\AD\D8\AF\D9\8A\D8\AB_\D9\85\D8\A4\D8\B4\D8\B1"()
  store i64 1, ptr %"%\D9\85\D8\B3\D8\AA\D9\85\D8\B1", align 8
  br label %while_cond_236

while_body_237:                                   ; preds = %while_cond_236
  %calltmp1 = call i64 @"kb_\D8\A7\D9\82\D8\B1\D8\A3_\D9\85\D9\81\D8\AA\D8\A7\D8\AD"()
  store i64 %calltmp1, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load" = load i64, ptr %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  %cmpeqtmp2 = icmp eq i64 %"%\D9\85\D9\81\D8\AA\D8\A7\D8\AD.load", 0
  br i1 %cmpeqtmp2, label %then_239, label %merge_240

while_cond_233:                                   ; preds = %while_exit_238, %merge_232
  br i1 true, label %while_body_234, label %while_exit_235

while_cond_236:                                   ; preds = %merge_270, %then_239, %while_body_234
  %"%\D9\85\D8\B3\D8\AA\D9\85\D8\B1.load" = load i64, ptr %"%\D9\85\D8\B3\D8\AA\D9\85\D8\B1", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\85\D8\B3\D8\AA\D9\85\D8\B1.load", 1
  br i1 %cmpeqtmp, label %while_body_237, label %while_exit_238

while_exit_235:                                   ; preds = %while_cond_233
  ret void

while_exit_238:                                   ; preds = %while_cond_236
  br label %while_cond_233
}

define void @"\D8\A7\D8\AE\D8\AA\D8\A8\D8\B1_\D8\A7\D9\84\D8\B5\D8\AF\D9\81\D8\A9"() {
entry:
  %"%\D9\861" = alloca i64, align 8
  %"%\D9\86\D8\AC\D8\A7\D8\AD" = alloca i64, align 8
  store i64 0, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %calltmp = call i64 @"\D9\87\D9\8A\D8\A6_\D8\A7\D9\84\D8\B5\D8\AF\D9\81\D8\A9"()
  store i64 %calltmp, ptr %"%\D9\861", align 8
  %"%\D9\861.load" = load i64, ptr %"%\D9\861", align 8
  %cmpeqtmp = icmp eq i64 %"%\D9\861.load", 0
  br i1 %cmpeqtmp, label %then_273, label %merge_274

merge_274:                                        ; preds = %then_273, %entry
  %"%\D9\861.load1" = load i64, ptr %"%\D9\861", align 8
  call void @"sh_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\861.load1")
  call void @"cmd_\D8\A3\D8\AF\D8\AE\D9\84_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"cmd_\D8\A3\D8\AF\D8\AE\D9\84_\D8\AD\D8\B1\D9\81"(i64 115)
  br i1 false, label %then_275, label %merge_276

merge_276:                                        ; preds = %then_275, %merge_274
  br i1 false, label %then_277, label %merge_278

merge_278:                                        ; preds = %then_277, %merge_276
  call void @"cmd_\D9\85\D8\B3\D8\AD_\D8\AE\D9\84\D9\81\D9\8A"()
  br i1 false, label %then_279, label %merge_280

merge_280:                                        ; preds = %then_279, %merge_278
  call void @"cmd_\D8\A3\D8\AF\D8\AE\D9\84_\D8\AD\D8\B1\D9\81"(i64 108)
  call void @"cmd_\D8\A3\D8\AF\D8\AE\D9\84_\D8\AD\D8\B1\D9\81"(i64 115)
  %calltmp8 = call i64 @"cmd_\D9\8A\D8\B3\D8\A7\D9\88\D9\8A"(i64 108, i64 115, i64 0, i64 0, i64 2)
  %cmpeqtmp9 = icmp eq i64 %calltmp8, 1
  br i1 %cmpeqtmp9, label %then_281, label %merge_282

merge_282:                                        ; preds = %then_281, %merge_280
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load12" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  call void @"sh_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load12")
  call void @"sh_\D8\A7\D8\B7\D8\A8\D8\B9_\D8\B1\D9\82\D9\85"(i64 5)
  ret void

then_273:                                         ; preds = %entry
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load", 1
  store i64 %addtmp, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_274

then_275:                                         ; preds = %merge_274
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load2" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp3 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load2", 1
  store i64 %addtmp3, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_276

then_277:                                         ; preds = %merge_276
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load4" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp5 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load4", 1
  store i64 %addtmp5, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_278

then_279:                                         ; preds = %merge_278
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load6" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp7 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load6", 1
  store i64 %addtmp7, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_280

then_281:                                         ; preds = %merge_280
  %"%\D9\86\D8\AC\D8\A7\D8\AD.load10" = load i64, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  %addtmp11 = add i64 %"%\D9\86\D8\AC\D8\A7\D8\AD.load10", 1
  store i64 %addtmp11, ptr %"%\D9\86\D8\AC\D8\A7\D8\AD", align 8
  br label %merge_282
}

define void @__sad_main() {
entry:
  %"%work_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%cwd_\D8\B7\D9\88\D9\84" = alloca i64, align 8
  %"%cwd_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%hist_\D9\81\D9\87\D8\B1\D8\B3" = alloca i64, align 8
  %"%hist_\D8\B9\D8\AF\D8\AF" = alloca i64, align 8
  %"%hist_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%cmd_\D9\85\D8\A4\D8\B4\D8\B1" = alloca i64, align 8
  %"%cmd_\D8\B7\D9\88\D9\84" = alloca i64, align 8
  %"%cmd_\D8\B9\D9\86\D9\88\D8\A7\D9\86" = alloca i64, align 8
  %"%kb_\D8\A2\D8\AE\D8\B1_\D9\85\D9\81\D8\AA\D8\A7\D8\AD" = alloca i64, align 8
  %"%kb_\D9\83\D9\86\D8\AA\D8\B1\D9\88\D9\84_\D9\85\D8\B6\D8\BA\D9\88\D8\B7" = alloca i64, align 8
  %"%kb_\D8\B4\D9\81\D8\AA_\D9\85\D8\B6\D8\BA\D9\88\D8\B7" = alloca i64, align 8
  %"%kb_\D8\AC\D8\AF\D9\88\D9\84_\D8\B4\D9\81\D8\AA" = alloca i64, align 8
  %"%kb_\D8\AC\D8\AF\D9\88\D9\84" = alloca i64, align 8
  %"%vga_\D9\84\D9\88\D9\86" = alloca i64, align 8
  %"%vga_\D8\B9\D9\85\D9\88\D8\AF" = alloca i64, align 8
  %"%vga_\D8\B5\D9\81" = alloca i64, align 8
  %"%DEFAULT_MOUNT" = alloca i64, align 8
  %"%MAX_HISTORY" = alloca i64, align 8
  %"%MAX_CMD" = alloca i64, align 8
  %"%KB_STATUS" = alloca i64, align 8
  %"%KB_DATA" = alloca i64, align 8
  %"%VGA_WHITE_BLACK" = alloca i64, align 8
  %"%VGA_RED_BLACK" = alloca i64, align 8
  %"%VGA_YELLOW_BLACK" = alloca i64, align 8
  %"%VGA_CYAN_BLACK" = alloca i64, align 8
  %"%VGA_GREEN_BLACK" = alloca i64, align 8
  %"%VGA_WHITE_BLUE" = alloca i64, align 8
  %"%VGA_SIZE" = alloca i64, align 8
  %"%VGA_ROWS" = alloca i64, align 8
  %"%VGA_COLS" = alloca i64, align 8
  %"%VGA_BASE" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 753664, ptr %"%VGA_BASE", align 8
  store i64 80, ptr %"%VGA_COLS", align 8
  store i64 25, ptr %"%VGA_ROWS", align 8
  store i64 4000, ptr %"%VGA_SIZE", align 8
  store i64 31, ptr %"%VGA_WHITE_BLUE", align 8
  store i64 10, ptr %"%VGA_GREEN_BLACK", align 8
  store i64 11, ptr %"%VGA_CYAN_BLACK", align 8
  store i64 14, ptr %"%VGA_YELLOW_BLACK", align 8
  store i64 12, ptr %"%VGA_RED_BLACK", align 8
  store i64 7, ptr %"%VGA_WHITE_BLACK", align 8
  store i64 96, ptr %"%KB_DATA", align 8
  store i64 100, ptr %"%KB_STATUS", align 8
  store i64 256, ptr %"%MAX_CMD", align 8
  store i64 16, ptr %"%MAX_HISTORY", align 8
  store i64 0, ptr %"%DEFAULT_MOUNT", align 8
  store i64 0, ptr %"%vga_\D8\B5\D9\81", align 8
  store i64 0, ptr %"%vga_\D8\B9\D9\85\D9\88\D8\AF", align 8
  store i64 7, ptr %"%vga_\D9\84\D9\88\D9\86", align 8
  %arr_new = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 0
  store i64 58, ptr %arr.len.gep, align 8
  %arr.cap.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 1
  store i64 58, ptr %arr.cap.gep, align 8
  %arr.data = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 58))
  %arr.data.gep = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  store ptr %arr.data, ptr %arr.data.gep, align 8
  %arr.data.gep1 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data2 = load ptr, ptr %arr.data.gep1, align 8
  %arr.elem = getelementptr i64, ptr %arr.data2, i64 0
  store i64 0, ptr %arr.elem, align 8
  %arr.data.gep3 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data4 = load ptr, ptr %arr.data.gep3, align 8
  %arr.elem5 = getelementptr i64, ptr %arr.data4, i64 1
  store i64 27, ptr %arr.elem5, align 8
  %arr.data.gep6 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data7 = load ptr, ptr %arr.data.gep6, align 8
  %arr.elem8 = getelementptr i64, ptr %arr.data7, i64 2
  store i64 49, ptr %arr.elem8, align 8
  %arr.data.gep9 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data10 = load ptr, ptr %arr.data.gep9, align 8
  %arr.elem11 = getelementptr i64, ptr %arr.data10, i64 3
  store i64 50, ptr %arr.elem11, align 8
  %arr.data.gep12 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data13 = load ptr, ptr %arr.data.gep12, align 8
  %arr.elem14 = getelementptr i64, ptr %arr.data13, i64 4
  store i64 51, ptr %arr.elem14, align 8
  %arr.data.gep15 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data16 = load ptr, ptr %arr.data.gep15, align 8
  %arr.elem17 = getelementptr i64, ptr %arr.data16, i64 5
  store i64 52, ptr %arr.elem17, align 8
  %arr.data.gep18 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data19 = load ptr, ptr %arr.data.gep18, align 8
  %arr.elem20 = getelementptr i64, ptr %arr.data19, i64 6
  store i64 53, ptr %arr.elem20, align 8
  %arr.data.gep21 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data22 = load ptr, ptr %arr.data.gep21, align 8
  %arr.elem23 = getelementptr i64, ptr %arr.data22, i64 7
  store i64 54, ptr %arr.elem23, align 8
  %arr.data.gep24 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data25 = load ptr, ptr %arr.data.gep24, align 8
  %arr.elem26 = getelementptr i64, ptr %arr.data25, i64 8
  store i64 55, ptr %arr.elem26, align 8
  %arr.data.gep27 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data28 = load ptr, ptr %arr.data.gep27, align 8
  %arr.elem29 = getelementptr i64, ptr %arr.data28, i64 9
  store i64 56, ptr %arr.elem29, align 8
  %arr.data.gep30 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data31 = load ptr, ptr %arr.data.gep30, align 8
  %arr.elem32 = getelementptr i64, ptr %arr.data31, i64 10
  store i64 57, ptr %arr.elem32, align 8
  %arr.data.gep33 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data34 = load ptr, ptr %arr.data.gep33, align 8
  %arr.elem35 = getelementptr i64, ptr %arr.data34, i64 11
  store i64 48, ptr %arr.elem35, align 8
  %arr.data.gep36 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data37 = load ptr, ptr %arr.data.gep36, align 8
  %arr.elem38 = getelementptr i64, ptr %arr.data37, i64 12
  store i64 45, ptr %arr.elem38, align 8
  %arr.data.gep39 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data40 = load ptr, ptr %arr.data.gep39, align 8
  %arr.elem41 = getelementptr i64, ptr %arr.data40, i64 13
  store i64 61, ptr %arr.elem41, align 8
  %arr.data.gep42 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data43 = load ptr, ptr %arr.data.gep42, align 8
  %arr.elem44 = getelementptr i64, ptr %arr.data43, i64 14
  store i64 8, ptr %arr.elem44, align 8
  %arr.data.gep45 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data46 = load ptr, ptr %arr.data.gep45, align 8
  %arr.elem47 = getelementptr i64, ptr %arr.data46, i64 15
  store i64 9, ptr %arr.elem47, align 8
  %arr.data.gep48 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data49 = load ptr, ptr %arr.data.gep48, align 8
  %arr.elem50 = getelementptr i64, ptr %arr.data49, i64 16
  store i64 113, ptr %arr.elem50, align 8
  %arr.data.gep51 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data52 = load ptr, ptr %arr.data.gep51, align 8
  %arr.elem53 = getelementptr i64, ptr %arr.data52, i64 17
  store i64 119, ptr %arr.elem53, align 8
  %arr.data.gep54 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data55 = load ptr, ptr %arr.data.gep54, align 8
  %arr.elem56 = getelementptr i64, ptr %arr.data55, i64 18
  store i64 101, ptr %arr.elem56, align 8
  %arr.data.gep57 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data58 = load ptr, ptr %arr.data.gep57, align 8
  %arr.elem59 = getelementptr i64, ptr %arr.data58, i64 19
  store i64 114, ptr %arr.elem59, align 8
  %arr.data.gep60 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data61 = load ptr, ptr %arr.data.gep60, align 8
  %arr.elem62 = getelementptr i64, ptr %arr.data61, i64 20
  store i64 116, ptr %arr.elem62, align 8
  %arr.data.gep63 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data64 = load ptr, ptr %arr.data.gep63, align 8
  %arr.elem65 = getelementptr i64, ptr %arr.data64, i64 21
  store i64 121, ptr %arr.elem65, align 8
  %arr.data.gep66 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data67 = load ptr, ptr %arr.data.gep66, align 8
  %arr.elem68 = getelementptr i64, ptr %arr.data67, i64 22
  store i64 117, ptr %arr.elem68, align 8
  %arr.data.gep69 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data70 = load ptr, ptr %arr.data.gep69, align 8
  %arr.elem71 = getelementptr i64, ptr %arr.data70, i64 23
  store i64 105, ptr %arr.elem71, align 8
  %arr.data.gep72 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data73 = load ptr, ptr %arr.data.gep72, align 8
  %arr.elem74 = getelementptr i64, ptr %arr.data73, i64 24
  store i64 111, ptr %arr.elem74, align 8
  %arr.data.gep75 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data76 = load ptr, ptr %arr.data.gep75, align 8
  %arr.elem77 = getelementptr i64, ptr %arr.data76, i64 25
  store i64 112, ptr %arr.elem77, align 8
  %arr.data.gep78 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data79 = load ptr, ptr %arr.data.gep78, align 8
  %arr.elem80 = getelementptr i64, ptr %arr.data79, i64 26
  store i64 91, ptr %arr.elem80, align 8
  %arr.data.gep81 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data82 = load ptr, ptr %arr.data.gep81, align 8
  %arr.elem83 = getelementptr i64, ptr %arr.data82, i64 27
  store i64 93, ptr %arr.elem83, align 8
  %arr.data.gep84 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data85 = load ptr, ptr %arr.data.gep84, align 8
  %arr.elem86 = getelementptr i64, ptr %arr.data85, i64 28
  store i64 10, ptr %arr.elem86, align 8
  %arr.data.gep87 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data88 = load ptr, ptr %arr.data.gep87, align 8
  %arr.elem89 = getelementptr i64, ptr %arr.data88, i64 29
  store i64 0, ptr %arr.elem89, align 8
  %arr.data.gep90 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data91 = load ptr, ptr %arr.data.gep90, align 8
  %arr.elem92 = getelementptr i64, ptr %arr.data91, i64 30
  store i64 97, ptr %arr.elem92, align 8
  %arr.data.gep93 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data94 = load ptr, ptr %arr.data.gep93, align 8
  %arr.elem95 = getelementptr i64, ptr %arr.data94, i64 31
  store i64 115, ptr %arr.elem95, align 8
  %arr.data.gep96 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data97 = load ptr, ptr %arr.data.gep96, align 8
  %arr.elem98 = getelementptr i64, ptr %arr.data97, i64 32
  store i64 100, ptr %arr.elem98, align 8
  %arr.data.gep99 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data100 = load ptr, ptr %arr.data.gep99, align 8
  %arr.elem101 = getelementptr i64, ptr %arr.data100, i64 33
  store i64 102, ptr %arr.elem101, align 8
  %arr.data.gep102 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data103 = load ptr, ptr %arr.data.gep102, align 8
  %arr.elem104 = getelementptr i64, ptr %arr.data103, i64 34
  store i64 103, ptr %arr.elem104, align 8
  %arr.data.gep105 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data106 = load ptr, ptr %arr.data.gep105, align 8
  %arr.elem107 = getelementptr i64, ptr %arr.data106, i64 35
  store i64 104, ptr %arr.elem107, align 8
  %arr.data.gep108 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data109 = load ptr, ptr %arr.data.gep108, align 8
  %arr.elem110 = getelementptr i64, ptr %arr.data109, i64 36
  store i64 106, ptr %arr.elem110, align 8
  %arr.data.gep111 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data112 = load ptr, ptr %arr.data.gep111, align 8
  %arr.elem113 = getelementptr i64, ptr %arr.data112, i64 37
  store i64 107, ptr %arr.elem113, align 8
  %arr.data.gep114 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data115 = load ptr, ptr %arr.data.gep114, align 8
  %arr.elem116 = getelementptr i64, ptr %arr.data115, i64 38
  store i64 108, ptr %arr.elem116, align 8
  %arr.data.gep117 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data118 = load ptr, ptr %arr.data.gep117, align 8
  %arr.elem119 = getelementptr i64, ptr %arr.data118, i64 39
  store i64 59, ptr %arr.elem119, align 8
  %arr.data.gep120 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data121 = load ptr, ptr %arr.data.gep120, align 8
  %arr.elem122 = getelementptr i64, ptr %arr.data121, i64 40
  store i64 39, ptr %arr.elem122, align 8
  %arr.data.gep123 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data124 = load ptr, ptr %arr.data.gep123, align 8
  %arr.elem125 = getelementptr i64, ptr %arr.data124, i64 41
  store i64 96, ptr %arr.elem125, align 8
  %arr.data.gep126 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data127 = load ptr, ptr %arr.data.gep126, align 8
  %arr.elem128 = getelementptr i64, ptr %arr.data127, i64 42
  store i64 0, ptr %arr.elem128, align 8
  %arr.data.gep129 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data130 = load ptr, ptr %arr.data.gep129, align 8
  %arr.elem131 = getelementptr i64, ptr %arr.data130, i64 43
  store i64 92, ptr %arr.elem131, align 8
  %arr.data.gep132 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data133 = load ptr, ptr %arr.data.gep132, align 8
  %arr.elem134 = getelementptr i64, ptr %arr.data133, i64 44
  store i64 122, ptr %arr.elem134, align 8
  %arr.data.gep135 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data136 = load ptr, ptr %arr.data.gep135, align 8
  %arr.elem137 = getelementptr i64, ptr %arr.data136, i64 45
  store i64 120, ptr %arr.elem137, align 8
  %arr.data.gep138 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data139 = load ptr, ptr %arr.data.gep138, align 8
  %arr.elem140 = getelementptr i64, ptr %arr.data139, i64 46
  store i64 99, ptr %arr.elem140, align 8
  %arr.data.gep141 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data142 = load ptr, ptr %arr.data.gep141, align 8
  %arr.elem143 = getelementptr i64, ptr %arr.data142, i64 47
  store i64 118, ptr %arr.elem143, align 8
  %arr.data.gep144 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data145 = load ptr, ptr %arr.data.gep144, align 8
  %arr.elem146 = getelementptr i64, ptr %arr.data145, i64 48
  store i64 98, ptr %arr.elem146, align 8
  %arr.data.gep147 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data148 = load ptr, ptr %arr.data.gep147, align 8
  %arr.elem149 = getelementptr i64, ptr %arr.data148, i64 49
  store i64 110, ptr %arr.elem149, align 8
  %arr.data.gep150 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data151 = load ptr, ptr %arr.data.gep150, align 8
  %arr.elem152 = getelementptr i64, ptr %arr.data151, i64 50
  store i64 109, ptr %arr.elem152, align 8
  %arr.data.gep153 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data154 = load ptr, ptr %arr.data.gep153, align 8
  %arr.elem155 = getelementptr i64, ptr %arr.data154, i64 51
  store i64 44, ptr %arr.elem155, align 8
  %arr.data.gep156 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data157 = load ptr, ptr %arr.data.gep156, align 8
  %arr.elem158 = getelementptr i64, ptr %arr.data157, i64 52
  store i64 46, ptr %arr.elem158, align 8
  %arr.data.gep159 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data160 = load ptr, ptr %arr.data.gep159, align 8
  %arr.elem161 = getelementptr i64, ptr %arr.data160, i64 53
  store i64 47, ptr %arr.elem161, align 8
  %arr.data.gep162 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data163 = load ptr, ptr %arr.data.gep162, align 8
  %arr.elem164 = getelementptr i64, ptr %arr.data163, i64 54
  store i64 0, ptr %arr.elem164, align 8
  %arr.data.gep165 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data166 = load ptr, ptr %arr.data.gep165, align 8
  %arr.elem167 = getelementptr i64, ptr %arr.data166, i64 55
  store i64 42, ptr %arr.elem167, align 8
  %arr.data.gep168 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data169 = load ptr, ptr %arr.data.gep168, align 8
  %arr.elem170 = getelementptr i64, ptr %arr.data169, i64 56
  store i64 0, ptr %arr.elem170, align 8
  %arr.data.gep171 = getelementptr inbounds %SadArray, ptr %arr_new, i32 0, i32 2
  %arr.data172 = load ptr, ptr %arr.data.gep171, align 8
  %arr.elem173 = getelementptr i64, ptr %arr.data172, i64 57
  store i64 32, ptr %arr.elem173, align 8
  %obj.ptrtoint = ptrtoint ptr %arr_new to i64
  store i64 %obj.ptrtoint, ptr %"%kb_\D8\AC\D8\AF\D9\88\D9\84", align 8
  %arr_new174 = call ptr @malloc(i64 ptrtoint (ptr getelementptr (%SadArray, ptr null, i32 1) to i64))
  %arr.len.gep175 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 0
  store i64 58, ptr %arr.len.gep175, align 8
  %arr.cap.gep176 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 1
  store i64 58, ptr %arr.cap.gep176, align 8
  %arr.data177 = call ptr @malloc(i64 mul (i64 ptrtoint (ptr getelementptr (ptr, ptr null, i32 1) to i64), i64 58))
  %arr.data.gep178 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  store ptr %arr.data177, ptr %arr.data.gep178, align 8
  %arr.data.gep179 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data180 = load ptr, ptr %arr.data.gep179, align 8
  %arr.elem181 = getelementptr i64, ptr %arr.data180, i64 0
  store i64 0, ptr %arr.elem181, align 8
  %arr.data.gep182 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data183 = load ptr, ptr %arr.data.gep182, align 8
  %arr.elem184 = getelementptr i64, ptr %arr.data183, i64 1
  store i64 27, ptr %arr.elem184, align 8
  %arr.data.gep185 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data186 = load ptr, ptr %arr.data.gep185, align 8
  %arr.elem187 = getelementptr i64, ptr %arr.data186, i64 2
  store i64 33, ptr %arr.elem187, align 8
  %arr.data.gep188 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data189 = load ptr, ptr %arr.data.gep188, align 8
  %arr.elem190 = getelementptr i64, ptr %arr.data189, i64 3
  store i64 64, ptr %arr.elem190, align 8
  %arr.data.gep191 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data192 = load ptr, ptr %arr.data.gep191, align 8
  %arr.elem193 = getelementptr i64, ptr %arr.data192, i64 4
  store i64 35, ptr %arr.elem193, align 8
  %arr.data.gep194 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data195 = load ptr, ptr %arr.data.gep194, align 8
  %arr.elem196 = getelementptr i64, ptr %arr.data195, i64 5
  store i64 36, ptr %arr.elem196, align 8
  %arr.data.gep197 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data198 = load ptr, ptr %arr.data.gep197, align 8
  %arr.elem199 = getelementptr i64, ptr %arr.data198, i64 6
  store i64 37, ptr %arr.elem199, align 8
  %arr.data.gep200 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data201 = load ptr, ptr %arr.data.gep200, align 8
  %arr.elem202 = getelementptr i64, ptr %arr.data201, i64 7
  store i64 94, ptr %arr.elem202, align 8
  %arr.data.gep203 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data204 = load ptr, ptr %arr.data.gep203, align 8
  %arr.elem205 = getelementptr i64, ptr %arr.data204, i64 8
  store i64 38, ptr %arr.elem205, align 8
  %arr.data.gep206 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data207 = load ptr, ptr %arr.data.gep206, align 8
  %arr.elem208 = getelementptr i64, ptr %arr.data207, i64 9
  store i64 42, ptr %arr.elem208, align 8
  %arr.data.gep209 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data210 = load ptr, ptr %arr.data.gep209, align 8
  %arr.elem211 = getelementptr i64, ptr %arr.data210, i64 10
  store i64 40, ptr %arr.elem211, align 8
  %arr.data.gep212 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data213 = load ptr, ptr %arr.data.gep212, align 8
  %arr.elem214 = getelementptr i64, ptr %arr.data213, i64 11
  store i64 41, ptr %arr.elem214, align 8
  %arr.data.gep215 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data216 = load ptr, ptr %arr.data.gep215, align 8
  %arr.elem217 = getelementptr i64, ptr %arr.data216, i64 12
  store i64 95, ptr %arr.elem217, align 8
  %arr.data.gep218 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data219 = load ptr, ptr %arr.data.gep218, align 8
  %arr.elem220 = getelementptr i64, ptr %arr.data219, i64 13
  store i64 43, ptr %arr.elem220, align 8
  %arr.data.gep221 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data222 = load ptr, ptr %arr.data.gep221, align 8
  %arr.elem223 = getelementptr i64, ptr %arr.data222, i64 14
  store i64 8, ptr %arr.elem223, align 8
  %arr.data.gep224 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data225 = load ptr, ptr %arr.data.gep224, align 8
  %arr.elem226 = getelementptr i64, ptr %arr.data225, i64 15
  store i64 9, ptr %arr.elem226, align 8
  %arr.data.gep227 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data228 = load ptr, ptr %arr.data.gep227, align 8
  %arr.elem229 = getelementptr i64, ptr %arr.data228, i64 16
  store i64 81, ptr %arr.elem229, align 8
  %arr.data.gep230 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data231 = load ptr, ptr %arr.data.gep230, align 8
  %arr.elem232 = getelementptr i64, ptr %arr.data231, i64 17
  store i64 87, ptr %arr.elem232, align 8
  %arr.data.gep233 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data234 = load ptr, ptr %arr.data.gep233, align 8
  %arr.elem235 = getelementptr i64, ptr %arr.data234, i64 18
  store i64 69, ptr %arr.elem235, align 8
  %arr.data.gep236 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data237 = load ptr, ptr %arr.data.gep236, align 8
  %arr.elem238 = getelementptr i64, ptr %arr.data237, i64 19
  store i64 82, ptr %arr.elem238, align 8
  %arr.data.gep239 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data240 = load ptr, ptr %arr.data.gep239, align 8
  %arr.elem241 = getelementptr i64, ptr %arr.data240, i64 20
  store i64 84, ptr %arr.elem241, align 8
  %arr.data.gep242 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data243 = load ptr, ptr %arr.data.gep242, align 8
  %arr.elem244 = getelementptr i64, ptr %arr.data243, i64 21
  store i64 89, ptr %arr.elem244, align 8
  %arr.data.gep245 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data246 = load ptr, ptr %arr.data.gep245, align 8
  %arr.elem247 = getelementptr i64, ptr %arr.data246, i64 22
  store i64 85, ptr %arr.elem247, align 8
  %arr.data.gep248 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data249 = load ptr, ptr %arr.data.gep248, align 8
  %arr.elem250 = getelementptr i64, ptr %arr.data249, i64 23
  store i64 73, ptr %arr.elem250, align 8
  %arr.data.gep251 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data252 = load ptr, ptr %arr.data.gep251, align 8
  %arr.elem253 = getelementptr i64, ptr %arr.data252, i64 24
  store i64 79, ptr %arr.elem253, align 8
  %arr.data.gep254 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data255 = load ptr, ptr %arr.data.gep254, align 8
  %arr.elem256 = getelementptr i64, ptr %arr.data255, i64 25
  store i64 80, ptr %arr.elem256, align 8
  %arr.data.gep257 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data258 = load ptr, ptr %arr.data.gep257, align 8
  %arr.elem259 = getelementptr i64, ptr %arr.data258, i64 26
  store i64 123, ptr %arr.elem259, align 8
  %arr.data.gep260 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data261 = load ptr, ptr %arr.data.gep260, align 8
  %arr.elem262 = getelementptr i64, ptr %arr.data261, i64 27
  store i64 125, ptr %arr.elem262, align 8
  %arr.data.gep263 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data264 = load ptr, ptr %arr.data.gep263, align 8
  %arr.elem265 = getelementptr i64, ptr %arr.data264, i64 28
  store i64 10, ptr %arr.elem265, align 8
  %arr.data.gep266 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data267 = load ptr, ptr %arr.data.gep266, align 8
  %arr.elem268 = getelementptr i64, ptr %arr.data267, i64 29
  store i64 0, ptr %arr.elem268, align 8
  %arr.data.gep269 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data270 = load ptr, ptr %arr.data.gep269, align 8
  %arr.elem271 = getelementptr i64, ptr %arr.data270, i64 30
  store i64 65, ptr %arr.elem271, align 8
  %arr.data.gep272 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data273 = load ptr, ptr %arr.data.gep272, align 8
  %arr.elem274 = getelementptr i64, ptr %arr.data273, i64 31
  store i64 83, ptr %arr.elem274, align 8
  %arr.data.gep275 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data276 = load ptr, ptr %arr.data.gep275, align 8
  %arr.elem277 = getelementptr i64, ptr %arr.data276, i64 32
  store i64 68, ptr %arr.elem277, align 8
  %arr.data.gep278 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data279 = load ptr, ptr %arr.data.gep278, align 8
  %arr.elem280 = getelementptr i64, ptr %arr.data279, i64 33
  store i64 70, ptr %arr.elem280, align 8
  %arr.data.gep281 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data282 = load ptr, ptr %arr.data.gep281, align 8
  %arr.elem283 = getelementptr i64, ptr %arr.data282, i64 34
  store i64 71, ptr %arr.elem283, align 8
  %arr.data.gep284 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data285 = load ptr, ptr %arr.data.gep284, align 8
  %arr.elem286 = getelementptr i64, ptr %arr.data285, i64 35
  store i64 72, ptr %arr.elem286, align 8
  %arr.data.gep287 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data288 = load ptr, ptr %arr.data.gep287, align 8
  %arr.elem289 = getelementptr i64, ptr %arr.data288, i64 36
  store i64 74, ptr %arr.elem289, align 8
  %arr.data.gep290 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data291 = load ptr, ptr %arr.data.gep290, align 8
  %arr.elem292 = getelementptr i64, ptr %arr.data291, i64 37
  store i64 75, ptr %arr.elem292, align 8
  %arr.data.gep293 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data294 = load ptr, ptr %arr.data.gep293, align 8
  %arr.elem295 = getelementptr i64, ptr %arr.data294, i64 38
  store i64 76, ptr %arr.elem295, align 8
  %arr.data.gep296 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data297 = load ptr, ptr %arr.data.gep296, align 8
  %arr.elem298 = getelementptr i64, ptr %arr.data297, i64 39
  store i64 58, ptr %arr.elem298, align 8
  %arr.data.gep299 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data300 = load ptr, ptr %arr.data.gep299, align 8
  %arr.elem301 = getelementptr i64, ptr %arr.data300, i64 40
  store i64 34, ptr %arr.elem301, align 8
  %arr.data.gep302 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data303 = load ptr, ptr %arr.data.gep302, align 8
  %arr.elem304 = getelementptr i64, ptr %arr.data303, i64 41
  store i64 126, ptr %arr.elem304, align 8
  %arr.data.gep305 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data306 = load ptr, ptr %arr.data.gep305, align 8
  %arr.elem307 = getelementptr i64, ptr %arr.data306, i64 42
  store i64 0, ptr %arr.elem307, align 8
  %arr.data.gep308 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data309 = load ptr, ptr %arr.data.gep308, align 8
  %arr.elem310 = getelementptr i64, ptr %arr.data309, i64 43
  store i64 124, ptr %arr.elem310, align 8
  %arr.data.gep311 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data312 = load ptr, ptr %arr.data.gep311, align 8
  %arr.elem313 = getelementptr i64, ptr %arr.data312, i64 44
  store i64 90, ptr %arr.elem313, align 8
  %arr.data.gep314 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data315 = load ptr, ptr %arr.data.gep314, align 8
  %arr.elem316 = getelementptr i64, ptr %arr.data315, i64 45
  store i64 88, ptr %arr.elem316, align 8
  %arr.data.gep317 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data318 = load ptr, ptr %arr.data.gep317, align 8
  %arr.elem319 = getelementptr i64, ptr %arr.data318, i64 46
  store i64 67, ptr %arr.elem319, align 8
  %arr.data.gep320 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data321 = load ptr, ptr %arr.data.gep320, align 8
  %arr.elem322 = getelementptr i64, ptr %arr.data321, i64 47
  store i64 86, ptr %arr.elem322, align 8
  %arr.data.gep323 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data324 = load ptr, ptr %arr.data.gep323, align 8
  %arr.elem325 = getelementptr i64, ptr %arr.data324, i64 48
  store i64 66, ptr %arr.elem325, align 8
  %arr.data.gep326 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data327 = load ptr, ptr %arr.data.gep326, align 8
  %arr.elem328 = getelementptr i64, ptr %arr.data327, i64 49
  store i64 78, ptr %arr.elem328, align 8
  %arr.data.gep329 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data330 = load ptr, ptr %arr.data.gep329, align 8
  %arr.elem331 = getelementptr i64, ptr %arr.data330, i64 50
  store i64 77, ptr %arr.elem331, align 8
  %arr.data.gep332 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data333 = load ptr, ptr %arr.data.gep332, align 8
  %arr.elem334 = getelementptr i64, ptr %arr.data333, i64 51
  store i64 60, ptr %arr.elem334, align 8
  %arr.data.gep335 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data336 = load ptr, ptr %arr.data.gep335, align 8
  %arr.elem337 = getelementptr i64, ptr %arr.data336, i64 52
  store i64 62, ptr %arr.elem337, align 8
  %arr.data.gep338 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data339 = load ptr, ptr %arr.data.gep338, align 8
  %arr.elem340 = getelementptr i64, ptr %arr.data339, i64 53
  store i64 63, ptr %arr.elem340, align 8
  %arr.data.gep341 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data342 = load ptr, ptr %arr.data.gep341, align 8
  %arr.elem343 = getelementptr i64, ptr %arr.data342, i64 54
  store i64 0, ptr %arr.elem343, align 8
  %arr.data.gep344 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data345 = load ptr, ptr %arr.data.gep344, align 8
  %arr.elem346 = getelementptr i64, ptr %arr.data345, i64 55
  store i64 42, ptr %arr.elem346, align 8
  %arr.data.gep347 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data348 = load ptr, ptr %arr.data.gep347, align 8
  %arr.elem349 = getelementptr i64, ptr %arr.data348, i64 56
  store i64 0, ptr %arr.elem349, align 8
  %arr.data.gep350 = getelementptr inbounds %SadArray, ptr %arr_new174, i32 0, i32 2
  %arr.data351 = load ptr, ptr %arr.data.gep350, align 8
  %arr.elem352 = getelementptr i64, ptr %arr.data351, i64 57
  store i64 32, ptr %arr.elem352, align 8
  %obj.ptrtoint353 = ptrtoint ptr %arr_new174 to i64
  store i64 %obj.ptrtoint353, ptr %"%kb_\D8\AC\D8\AF\D9\88\D9\84_\D8\B4\D9\81\D8\AA", align 8
  store i64 0, ptr %"%kb_\D8\B4\D9\81\D8\AA_\D9\85\D8\B6\D8\BA\D9\88\D8\B7", align 8
  store i64 0, ptr %"%kb_\D9\83\D9\86\D8\AA\D8\B1\D9\88\D9\84_\D9\85\D8\B6\D8\BA\D9\88\D8\B7", align 8
  store i64 0, ptr %"%kb_\D8\A2\D8\AE\D8\B1_\D9\85\D9\81\D8\AA\D8\A7\D8\AD", align 8
  store i64 0, ptr %"%cmd_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  store i64 0, ptr %"%cmd_\D8\B7\D9\88\D9\84", align 8
  store i64 0, ptr %"%cmd_\D9\85\D8\A4\D8\B4\D8\B1", align 8
  store i64 0, ptr %"%hist_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  store i64 0, ptr %"%hist_\D8\B9\D8\AF\D8\AF", align 8
  store i64 0, ptr %"%hist_\D9\81\D9\87\D8\B1\D8\B3", align 8
  store i64 0, ptr %"%cwd_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  store i64 1, ptr %"%cwd_\D8\B7\D9\88\D9\84", align 8
  store i64 0, ptr %"%work_\D8\B9\D9\86\D9\88\D8\A7\D9\86", align 8
  ret void
}

declare ptr @malloc(i64)

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
