; ModuleID = 'ata_نحلة'
source_filename = "ata_\D9\86\D8\AD\D9\84\D8\A9"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

@COM1 = constant i64 1016
@ATA_PRI_BASE = constant i64 496
@ATA_PRI_CTRL = constant i64 1014
@ATA_SEC_BASE = constant i64 368
@ATA_SEC_CTRL = constant i64 886
@ATA_REG_DATA = constant i64 0
@ATA_REG_ERROR = constant i64 1
@ATA_REG_COUNT = constant i64 2
@ATA_REG_LBALO = constant i64 3
@ATA_REG_LBAMID = constant i64 4
@ATA_REG_LBAHI = constant i64 5
@ATA_REG_DRIVE = constant i64 6
@ATA_REG_STATUS = constant i64 7
@ATA_REG_ALTSTATUS = constant i64 0
@ATA_SR_BSY = constant i64 128
@ATA_SR_DRDY = constant i64 64
@ATA_SR_DF = constant i64 32
@ATA_SR_DSC = constant i64 16
@ATA_SR_DRQ = constant i64 8
@ATA_SR_CORR = constant i64 4
@ATA_SR_IDX = constant i64 2
@ATA_SR_ERR = constant i64 1
@ATA_ER_BBK = constant i64 128
@ATA_ER_UNC = constant i64 64
@ATA_ER_MC = constant i64 32
@ATA_ER_IDNF = constant i64 16
@ATA_ER_MCR = constant i64 8
@ATA_ER_ABRT = constant i64 4
@ATA_ER_TK0NF = constant i64 2
@ATA_ER_AMNF = constant i64 1
@ATA_CMD_IDENTIFY = constant i64 236
@ATA_CMD_READ_PIO = constant i64 32
@ATA_CMD_WRITE_PIO = constant i64 48
@ATA_CMD_READ_PIO_EXT = constant i64 36
@ATA_CMD_WRITE_PIO_EXT = constant i64 52
@ATA_CMD_CACHE_FLUSH = constant i64 231
@ATA_CMD_CACHE_FLUSH_EXT = constant i64 234
@ATA_CMD_PACKET = constant i64 160
@ATA_CMD_IDENTIFY_PACKET = constant i64 161
@ATA_MASTER = constant i64 160
@ATA_SLAVE = constant i64 176
@ATA_TYPE_NONE = constant i64 0
@ATA_TYPE_ATA = constant i64 1
@ATA_TYPE_ATAPI = constant i64 2
@ATA_MAX_DRIVES = constant i64 4
@ata_base_0 = global i64 496
@ata_base_1 = global i64 496
@ata_base_2 = global i64 368
@ata_base_3 = global i64 368
@ata_ctrl_0 = global i64 1014
@ata_ctrl_1 = global i64 1014
@ata_ctrl_2 = global i64 886
@ata_ctrl_3 = global i64 886
@ata_type_0 = global i64 0
@ata_type_1 = global i64 0
@ata_type_2 = global i64 0
@ata_type_3 = global i64 0
@ata_select_0 = global i64 160
@ata_select_1 = global i64 176
@ata_select_2 = global i64 160
@ata_select_3 = global i64 176
@ata_present_0 = global i64 0
@ata_present_1 = global i64 0
@ata_present_2 = global i64 0
@ata_present_3 = global i64 0
@ata_sectors_0 = global i64 0
@ata_sectors_1 = global i64 0
@ata_sectors_2 = global i64 0
@ata_sectors_3 = global i64 0
@ata_size_mb_0 = global i64 0
@ata_size_mb_1 = global i64 0
@ata_size_mb_2 = global i64 0
@ata_size_mb_3 = global i64 0
@ata_lba48_0 = global i64 0
@ata_lba48_1 = global i64 0
@ata_lba48_2 = global i64 0
@ata_lba48_3 = global i64 0
@ata_model_0_0 = global i64 0
@ata_model_0_1 = global i64 0
@ata_model_0_2 = global i64 0
@ata_model_0_3 = global i64 0
@ata_model_0_4 = global i64 0
@ata_model_0_5 = global i64 0
@ata_model_0_6 = global i64 0
@ata_model_0_7 = global i64 0
@ata_model_0_8 = global i64 0
@ata_model_0_9 = global i64 0
@ata_model_1_0 = global i64 0
@ata_model_1_1 = global i64 0
@ata_model_1_2 = global i64 0
@ata_model_1_3 = global i64 0
@ata_model_1_4 = global i64 0
@ata_model_1_5 = global i64 0
@ata_model_1_6 = global i64 0
@ata_model_1_7 = global i64 0
@ata_model_1_8 = global i64 0
@ata_model_1_9 = global i64 0
@ata_model_2_0 = global i64 0
@ata_model_2_1 = global i64 0
@ata_model_2_2 = global i64 0
@ata_model_2_3 = global i64 0
@ata_model_2_4 = global i64 0
@ata_model_2_5 = global i64 0
@ata_model_2_6 = global i64 0
@ata_model_2_7 = global i64 0
@ata_model_2_8 = global i64 0
@ata_model_2_9 = global i64 0
@ata_model_3_0 = global i64 0
@ata_model_3_1 = global i64 0
@ata_model_3_2 = global i64 0
@ata_model_3_3 = global i64 0
@ata_model_3_4 = global i64 0
@ata_model_3_5 = global i64 0
@ata_model_3_6 = global i64 0
@ata_model_3_7 = global i64 0
@ata_model_3_8 = global i64 0
@ata_model_3_9 = global i64 0
@ata_identify_buf = global i64 0
@ata_drives_found = global i64 0
@ata_reads = global i64 0
@ata_writes = global i64 0
@ata_errors = global i64 0
@ata_part_type_0_0 = global i64 0
@ata_part_type_0_1 = global i64 0
@ata_part_type_0_2 = global i64 0
@ata_part_type_0_3 = global i64 0
@ata_part_start_0_0 = global i64 0
@ata_part_start_0_1 = global i64 0
@ata_part_start_0_2 = global i64 0
@ata_part_start_0_3 = global i64 0
@ata_part_size_0_0 = global i64 0
@ata_part_size_0_1 = global i64 0
@ata_part_size_0_2 = global i64 0
@ata_part_size_0_3 = global i64 0
@str.const = private unnamed_addr constant [13 x i8] c"[ATA] Drive \00", align 1
@str.const.1 = private unnamed_addr constant [10 x i8] c" is ATAPI\00", align 1
@str.const.3 = private unnamed_addr constant [7 x i8] c" ATA: \00", align 1
@str.const.4 = private unnamed_addr constant [6 x i8] c" MB, \00", align 1
@str.const.5 = private unnamed_addr constant [9 x i8] c" sectors\00", align 1
@str.const.6 = private unnamed_addr constant [14 x i8] c"[ATA] Model: \00", align 1
@str.const.7 = private unnamed_addr constant [22 x i8] c"[ATA] MBR read failed\00", align 1
@str.const.8 = private unnamed_addr constant [30 x i8] c"[ATA] Invalid MBR signature: \00", align 1
@str.const.9 = private unnamed_addr constant [39 x i8] c"[ATA] MBR valid, parsing partitions...\00", align 1
@str.const.10 = private unnamed_addr constant [12 x i8] c"[ATA] Part \00", align 1
@str.const.11 = private unnamed_addr constant [10 x i8] c": type=0x\00", align 1
@str.const.12 = private unnamed_addr constant [8 x i8] c" start=\00", align 1
@str.const.13 = private unnamed_addr constant [7 x i8] c" size=\00", align 1
@str.const.14 = private unnamed_addr constant [37 x i8] c"\E2\95\90\E2\95\90\E2\95\90 [ATA] \D8\AA\D9\82\D8\B1\D9\8A\D8\B1 \E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.15 = private unnamed_addr constant [15 x i8] c"  \D8\A3\D9\82\D8\B1\D8\A7\D8\B5: \00", align 1
@str.const.16 = private unnamed_addr constant [17 x i8] c"  \D9\82\D8\B1\D8\A7\D8\A1\D8\A7\D8\AA: \00", align 1
@str.const.17 = private unnamed_addr constant [16 x i8] c" \D9\83\D8\AA\D8\A7\D8\A8\D8\A7\D8\AA: \00", align 1
@str.const.18 = private unnamed_addr constant [14 x i8] c" \D8\A3\D8\AE\D8\B7\D8\A7\D8\A1: \00", align 1
@str.const.19 = private unnamed_addr constant [106 x i8] c"\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\E2\95\90\00", align 1
@str.const.20 = private unnamed_addr constant [9 x i8] c"  Drive \00", align 1
@str.const.21 = private unnamed_addr constant [3 x i8] c": \00", align 1
@str.const.24 = private unnamed_addr constant [39 x i8] c"[ATA] \D8\AA\D9\87\D9\8A\D8\A6\D8\A9 \D9\85\D8\AA\D8\AD\D9\83\D9\85 ATA/IDE...\00", align 1
@str.const.25 = private unnamed_addr constant [20 x i8] c"[ATA] \D8\AC\D8\A7\D9\87\D8\B2 \E2\80\94 \00", align 1
@str.const.26 = private unnamed_addr constant [14 x i8] c" drives found\00", align 1

declare void @sad_ll_port_inb(i64)

declare void @sad_ll_port_outb(i64, i64)

declare void @sad_ll_port_inw(i64)

declare void @sad_ll_port_outw(i64, i64)

declare void @sad_ll_port_inl(i64)

declare void @sad_ll_port_outl(i64, i64)

declare void @sad_ll_mem_read8(i64)

declare void @sad_ll_mem_write8(i64, i64)

declare void @sad_ll_mem_read16(i64)

declare void @sad_ll_mem_write16(i64, i64)

declare void @sad_ll_mem_read32(i64)

declare void @sad_ll_mem_write32(i64, i64)

declare void @sad_ll_serial_write_char(i64, i64)

declare void @sad_ll_kmalloc(i64)

declare void @sad_ll_memset(i64, i64, i64)

define i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9") {
entry:
  %"%ch" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91" = alloca i64, align 8
  store i64 %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A9", ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_0

merge_4:                                          ; preds = %while_body_1
  %"%COM1.load" = load i64, ptr @COM1, align 8
  %"%ch.load3" = load i64, ptr %"%ch", align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %"%ch.load3")
  %"%i.load4" = load i64, ptr %"%i", align 8
  %addtmp5 = add i64 %"%i.load4", 1
  store i64 %addtmp5, ptr %"%i", align 8
  br label %while_cond_0

then_3:                                           ; preds = %while_body_1
  ret i64 0

while_body_1:                                     ; preds = %while_cond_0
  %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load" = load i64, ptr %"\D8\B1\D8\B3\D8\A7\D9\84\D8\A91", align 8
  %"%i.load2" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%\D8\B1\D8\B3\D8\A7\D9\84\D8\A9.load", %"%i.load2"
  call void @sad_ll_mem_read8(i64 %addtmp)
  store i64 0, ptr %"%ch", align 8
  %"%ch.load" = load i64, ptr %"%ch", align 8
  %cmpeqtmp = icmp eq i64 %"%ch.load", 0
  br i1 %cmpeqtmp, label %then_3, label %merge_4

while_cond_0:                                     ; preds = %merge_4, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp = icmp slt i64 %"%i.load", 80
  br i1 %cmplttmp, label %while_body_1, label %while_exit_2

while_exit_2:                                     ; preds = %while_cond_0
  ret i64 0
}

define i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"\D9\86") {
entry:
  %"%d2" = alloca i64, align 8
  %"%d3" = alloca i64, align 8
  %"%d4" = alloca i64, align 8
  %"%d5" = alloca i64, align 8
  %"%d6" = alloca i64, align 8
  %"%d7" = alloca i64, align 8
  %"%val" = alloca i64, align 8
  %"\D9\861" = alloca i64, align 8
  store i64 %"\D9\86", ptr %"\D9\861", align 8
  %"%17.load" = load i64, ptr %"\D9\861", align 8
  store i64 %"%17.load", ptr %"%val", align 8
  %"%val.load" = load i64, ptr %"%val", align 8
  %cmplttmp = icmp slt i64 %"%val.load", 0
  br i1 %cmplttmp, label %then_5, label %merge_6

else_11:                                          ; preds = %merge_8
  %"%\D9\86.load" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp19 = icmp sge i64 %"%\D9\86.load", 1000000
  br i1 %cmpgetmp19, label %then_12, label %merge_13

else_16:                                          ; preds = %merge_10
  %"%\D9\86.load31" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp32 = icmp sge i64 %"%\D9\86.load31", 100000
  br i1 %cmpgetmp32, label %then_17, label %merge_18

else_21:                                          ; preds = %merge_15
  %"%\D9\86.load44" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp45 = icmp sge i64 %"%\D9\86.load44", 10000
  br i1 %cmpgetmp45, label %then_22, label %merge_23

else_26:                                          ; preds = %merge_20
  %"%\D9\86.load57" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp58 = icmp sge i64 %"%\D9\86.load57", 1000
  br i1 %cmpgetmp58, label %then_27, label %merge_28

else_31:                                          ; preds = %merge_25
  %"%\D9\86.load70" = load i64, ptr %"\D9\861", align 8
  %cmpgetmp71 = icmp sge i64 %"%\D9\86.load70", 100
  br i1 %cmpgetmp71, label %then_32, label %merge_33

merge_10:                                         ; preds = %merge_13, %then_9
  %"%val.load20" = load i64, ptr %"%val", align 8
  %cmpgetmp21 = icmp sge i64 %"%val.load20", 10000
  br i1 %cmpgetmp21, label %then_14, label %else_16

merge_13:                                         ; preds = %then_12, %else_11
  br label %merge_10

merge_15:                                         ; preds = %merge_18, %then_14
  %"%val.load33" = load i64, ptr %"%val", align 8
  %cmpgetmp34 = icmp sge i64 %"%val.load33", 1000
  br i1 %cmpgetmp34, label %then_19, label %else_21

merge_18:                                         ; preds = %then_17, %else_16
  br label %merge_15

merge_20:                                         ; preds = %merge_23, %then_19
  %"%val.load46" = load i64, ptr %"%val", align 8
  %cmpgetmp47 = icmp sge i64 %"%val.load46", 100
  br i1 %cmpgetmp47, label %then_24, label %else_26

merge_23:                                         ; preds = %then_22, %else_21
  br label %merge_20

merge_25:                                         ; preds = %merge_28, %then_24
  %"%val.load59" = load i64, ptr %"%val", align 8
  %cmpgetmp60 = icmp sge i64 %"%val.load59", 10
  br i1 %cmpgetmp60, label %then_29, label %else_31

merge_28:                                         ; preds = %then_27, %else_26
  br label %merge_25

merge_30:                                         ; preds = %merge_33, %then_29
  %"%val.load72" = load i64, ptr %"%val", align 8
  %addtmp73 = add i64 48, %"%val.load72"
  %"%COM1.load74" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load74", i64 %addtmp73)
  ret i64 0

merge_33:                                         ; preds = %then_32, %else_31
  br label %merge_30

merge_6:                                          ; preds = %then_5, %entry
  %"%val.load3" = load i64, ptr %"%val", align 8
  %cmpgetmp = icmp sge i64 %"%val.load3", 1000000
  br i1 %cmpgetmp, label %then_7, label %merge_8

merge_8:                                          ; preds = %then_7, %merge_6
  %"%val.load9" = load i64, ptr %"%val", align 8
  %cmpgetmp10 = icmp sge i64 %"%val.load9", 100000
  br i1 %cmpgetmp10, label %then_9, label %else_11

then_12:                                          ; preds = %else_11
  %"%COM1.load22" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load22", i64 48)
  br label %merge_13

then_14:                                          ; preds = %merge_10
  %"%val.load23" = load i64, ptr %"%val", align 8
  %divtmp24 = sdiv i64 %"%val.load23", 10000
  store i64 %divtmp24, ptr %"%d5", align 8
  %"%d5.load" = load i64, ptr %"%d5", align 8
  %addtmp25 = add i64 48, %"%d5.load"
  %"%COM1.load26" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load26", i64 %addtmp25)
  %"%d5.load27" = load i64, ptr %"%d5", align 8
  %multmp28 = mul i64 %"%d5.load27", 10000
  %"%val.load29" = load i64, ptr %"%val", align 8
  %subtmp30 = sub i64 %"%val.load29", %multmp28
  store i64 %subtmp30, ptr %"%val", align 8
  br label %merge_15

then_17:                                          ; preds = %else_16
  %"%COM1.load35" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load35", i64 48)
  br label %merge_18

then_19:                                          ; preds = %merge_15
  %"%val.load36" = load i64, ptr %"%val", align 8
  %divtmp37 = sdiv i64 %"%val.load36", 1000
  store i64 %divtmp37, ptr %"%d4", align 8
  %"%d4.load" = load i64, ptr %"%d4", align 8
  %addtmp38 = add i64 48, %"%d4.load"
  %"%COM1.load39" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load39", i64 %addtmp38)
  %"%d4.load40" = load i64, ptr %"%d4", align 8
  %multmp41 = mul i64 %"%d4.load40", 1000
  %"%val.load42" = load i64, ptr %"%val", align 8
  %subtmp43 = sub i64 %"%val.load42", %multmp41
  store i64 %subtmp43, ptr %"%val", align 8
  br label %merge_20

then_22:                                          ; preds = %else_21
  %"%COM1.load48" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load48", i64 48)
  br label %merge_23

then_24:                                          ; preds = %merge_20
  %"%val.load49" = load i64, ptr %"%val", align 8
  %divtmp50 = sdiv i64 %"%val.load49", 100
  store i64 %divtmp50, ptr %"%d3", align 8
  %"%d3.load" = load i64, ptr %"%d3", align 8
  %addtmp51 = add i64 48, %"%d3.load"
  %"%COM1.load52" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load52", i64 %addtmp51)
  %"%d3.load53" = load i64, ptr %"%d3", align 8
  %multmp54 = mul i64 %"%d3.load53", 100
  %"%val.load55" = load i64, ptr %"%val", align 8
  %subtmp56 = sub i64 %"%val.load55", %multmp54
  store i64 %subtmp56, ptr %"%val", align 8
  br label %merge_25

then_27:                                          ; preds = %else_26
  %"%COM1.load61" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load61", i64 48)
  br label %merge_28

then_29:                                          ; preds = %merge_25
  %"%val.load62" = load i64, ptr %"%val", align 8
  %divtmp63 = sdiv i64 %"%val.load62", 10
  store i64 %divtmp63, ptr %"%d2", align 8
  %"%d2.load" = load i64, ptr %"%d2", align 8
  %addtmp64 = add i64 48, %"%d2.load"
  %"%COM1.load65" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load65", i64 %addtmp64)
  %"%d2.load66" = load i64, ptr %"%d2", align 8
  %multmp67 = mul i64 %"%d2.load66", 10
  %"%val.load68" = load i64, ptr %"%val", align 8
  %subtmp69 = sub i64 %"%val.load68", %multmp67
  store i64 %subtmp69, ptr %"%val", align 8
  br label %merge_30

then_32:                                          ; preds = %else_31
  %"%COM1.load75" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load75", i64 48)
  br label %merge_33

then_5:                                           ; preds = %entry
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 45)
  %"%val.load2" = load i64, ptr %"%val", align 8
  %subtmp = sub i64 0, %"%val.load2"
  store i64 %subtmp, ptr %"%val", align 8
  br label %merge_6

then_7:                                           ; preds = %merge_6
  %"%val.load4" = load i64, ptr %"%val", align 8
  %divtmp = sdiv i64 %"%val.load4", 1000000
  store i64 %divtmp, ptr %"%d7", align 8
  %"%d7.load" = load i64, ptr %"%d7", align 8
  %addtmp = add i64 48, %"%d7.load"
  %"%COM1.load5" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load5", i64 %addtmp)
  %"%d7.load6" = load i64, ptr %"%d7", align 8
  %multmp = mul i64 %"%d7.load6", 1000000
  %"%val.load7" = load i64, ptr %"%val", align 8
  %subtmp8 = sub i64 %"%val.load7", %multmp
  store i64 %subtmp8, ptr %"%val", align 8
  br label %merge_8

then_9:                                           ; preds = %merge_8
  %"%val.load11" = load i64, ptr %"%val", align 8
  %divtmp12 = sdiv i64 %"%val.load11", 100000
  store i64 %divtmp12, ptr %"%d6", align 8
  %"%d6.load" = load i64, ptr %"%d6", align 8
  %addtmp13 = add i64 48, %"%d6.load"
  %"%COM1.load14" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load14", i64 %addtmp13)
  %"%d6.load15" = load i64, ptr %"%d6", align 8
  %multmp16 = mul i64 %"%d6.load15", 100000
  %"%val.load17" = load i64, ptr %"%val", align 8
  %subtmp18 = sub i64 %"%val.load17", %multmp16
  store i64 %subtmp18, ptr %"%val", align 8
  br label %merge_10
}

define i64 @ata_serial_hex8(i64 %val) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %val1 = alloca i64, align 8
  store i64 %val, ptr %val1, align 8
  %"%val.load" = load i64, ptr %val1, align 8
  %shrtmp = lshr i64 %"%val.load", 4
  store i64 %shrtmp, ptr %"%hi", align 8
  %"%val.load2" = load i64, ptr %val1, align 8
  %andtmp = and i64 %"%val.load2", 15
  store i64 %andtmp, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %cmplttmp = icmp slt i64 %"%hi.load", 10
  br i1 %cmplttmp, label %then_34, label %else_36

else_36:                                          ; preds = %entry
  %"%hi.load4" = load i64, ptr %"%hi", align 8
  %addtmp5 = add i64 55, %"%hi.load4"
  %"%COM1.load6" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load6", i64 %addtmp5)
  br label %merge_35

else_39:                                          ; preds = %merge_35
  %"%lo.load11" = load i64, ptr %"%lo", align 8
  %addtmp12 = add i64 55, %"%lo.load11"
  %"%COM1.load13" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load13", i64 %addtmp12)
  br label %merge_38

merge_35:                                         ; preds = %else_36, %then_34
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %cmplttmp7 = icmp slt i64 %"%lo.load", 10
  br i1 %cmplttmp7, label %then_37, label %else_39

merge_38:                                         ; preds = %else_39, %then_37
  ret i64 0

then_34:                                          ; preds = %entry
  %"%hi.load3" = load i64, ptr %"%hi", align 8
  %addtmp = add i64 48, %"%hi.load3"
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %addtmp)
  br label %merge_35

then_37:                                          ; preds = %merge_35
  %"%lo.load8" = load i64, ptr %"%lo", align 8
  %addtmp9 = add i64 48, %"%lo.load8"
  %"%COM1.load10" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load10", i64 %addtmp9)
  br label %merge_38
}

define i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"() {
entry:
  %"%COM1.load" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 13)
  %"%COM1.load1" = load i64, ptr @COM1, align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load1", i64 10)
  ret i64 0
}

define i64 @ata_get_base(i64 %drv) {
entry:
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_40, label %merge_41

merge_41:                                         ; preds = %entry
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%drv.load2", 1
  br i1 %cmpeqtmp3, label %then_42, label %merge_43

merge_43:                                         ; preds = %merge_41
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_44, label %merge_45

merge_45:                                         ; preds = %merge_43
  %"%drv.load8" = load i64, ptr %drv1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv.load8", 3
  br i1 %cmpeqtmp9, label %then_46, label %merge_47

merge_47:                                         ; preds = %merge_45
  ret i64 0

then_40:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ata_base_0, align 8
  ret i64 %loadtmp

then_42:                                          ; preds = %merge_41
  %loadtmp4 = load i64, ptr @ata_base_1, align 8
  ret i64 %loadtmp4

then_44:                                          ; preds = %merge_43
  %loadtmp7 = load i64, ptr @ata_base_2, align 8
  ret i64 %loadtmp7

then_46:                                          ; preds = %merge_45
  %loadtmp10 = load i64, ptr @ata_base_3, align 8
  ret i64 %loadtmp10
}

define i64 @ata_get_ctrl(i64 %drv) {
entry:
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_48, label %merge_49

merge_49:                                         ; preds = %entry
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%drv.load2", 1
  br i1 %cmpeqtmp3, label %then_50, label %merge_51

merge_51:                                         ; preds = %merge_49
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_52, label %merge_53

merge_53:                                         ; preds = %merge_51
  %"%drv.load8" = load i64, ptr %drv1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv.load8", 3
  br i1 %cmpeqtmp9, label %then_54, label %merge_55

merge_55:                                         ; preds = %merge_53
  ret i64 0

then_48:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ata_ctrl_0, align 8
  ret i64 %loadtmp

then_50:                                          ; preds = %merge_49
  %loadtmp4 = load i64, ptr @ata_ctrl_1, align 8
  ret i64 %loadtmp4

then_52:                                          ; preds = %merge_51
  %loadtmp7 = load i64, ptr @ata_ctrl_2, align 8
  ret i64 %loadtmp7

then_54:                                          ; preds = %merge_53
  %loadtmp10 = load i64, ptr @ata_ctrl_3, align 8
  ret i64 %loadtmp10
}

define i64 @ata_get_select(i64 %drv) {
entry:
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_56, label %merge_57

merge_57:                                         ; preds = %entry
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%drv.load2", 1
  br i1 %cmpeqtmp3, label %then_58, label %merge_59

merge_59:                                         ; preds = %merge_57
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_60, label %merge_61

merge_61:                                         ; preds = %merge_59
  %"%drv.load8" = load i64, ptr %drv1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv.load8", 3
  br i1 %cmpeqtmp9, label %then_62, label %merge_63

merge_63:                                         ; preds = %merge_61
  ret i64 0

then_56:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ata_select_0, align 8
  ret i64 %loadtmp

then_58:                                          ; preds = %merge_57
  %loadtmp4 = load i64, ptr @ata_select_1, align 8
  ret i64 %loadtmp4

then_60:                                          ; preds = %merge_59
  %loadtmp7 = load i64, ptr @ata_select_2, align 8
  ret i64 %loadtmp7

then_62:                                          ; preds = %merge_61
  %loadtmp10 = load i64, ptr @ata_select_3, align 8
  ret i64 %loadtmp10
}

define i64 @ata_get_type(i64 %drv) {
entry:
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_64, label %merge_65

merge_65:                                         ; preds = %entry
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%drv.load2", 1
  br i1 %cmpeqtmp3, label %then_66, label %merge_67

merge_67:                                         ; preds = %merge_65
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_68, label %merge_69

merge_69:                                         ; preds = %merge_67
  %"%drv.load8" = load i64, ptr %drv1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv.load8", 3
  br i1 %cmpeqtmp9, label %then_70, label %merge_71

merge_71:                                         ; preds = %merge_69
  ret i64 0

then_64:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ata_type_0, align 8
  ret i64 %loadtmp

then_66:                                          ; preds = %merge_65
  %loadtmp4 = load i64, ptr @ata_type_1, align 8
  ret i64 %loadtmp4

then_68:                                          ; preds = %merge_67
  %loadtmp7 = load i64, ptr @ata_type_2, align 8
  ret i64 %loadtmp7

then_70:                                          ; preds = %merge_69
  %loadtmp10 = load i64, ptr @ata_type_3, align 8
  ret i64 %loadtmp10
}

define i64 @ata_set_type(i64 %drv, i64 %val) {
entry:
  %"%ata_type_3" = alloca i64, align 8
  %"%ata_type_2" = alloca i64, align 8
  %"%ata_type_1" = alloca i64, align 8
  %"%ata_type_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_72, label %merge_73

merge_73:                                         ; preds = %then_72, %entry
  %"%drv.load3" = load i64, ptr %drv1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%drv.load3", 1
  br i1 %cmpeqtmp4, label %then_74, label %merge_75

merge_75:                                         ; preds = %then_74, %merge_73
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_76, label %merge_77

merge_77:                                         ; preds = %then_76, %merge_75
  %"%drv.load7" = load i64, ptr %drv1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%drv.load7", 3
  br i1 %cmpeqtmp8, label %then_78, label %merge_79

merge_79:                                         ; preds = %then_78, %merge_77
  ret i64 0

then_72:                                          ; preds = %entry
  %"%221.load" = load i64, ptr %val2, align 8
  store i64 %"%221.load", ptr %"%ata_type_0", align 8
  br label %merge_73

then_74:                                          ; preds = %merge_73
  %"%225.load" = load i64, ptr %val2, align 8
  store i64 %"%225.load", ptr %"%ata_type_1", align 8
  br label %merge_75

then_76:                                          ; preds = %merge_75
  %"%229.load" = load i64, ptr %val2, align 8
  store i64 %"%229.load", ptr %"%ata_type_2", align 8
  br label %merge_77

then_78:                                          ; preds = %merge_77
  %"%233.load" = load i64, ptr %val2, align 8
  store i64 %"%233.load", ptr %"%ata_type_3", align 8
  br label %merge_79
}

define i64 @ata_set_present(i64 %drv, i64 %val) {
entry:
  %"%ata_present_3" = alloca i64, align 8
  %"%ata_present_2" = alloca i64, align 8
  %"%ata_present_1" = alloca i64, align 8
  %"%ata_present_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_80, label %merge_81

merge_81:                                         ; preds = %then_80, %entry
  %"%drv.load3" = load i64, ptr %drv1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%drv.load3", 1
  br i1 %cmpeqtmp4, label %then_82, label %merge_83

merge_83:                                         ; preds = %then_82, %merge_81
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_84, label %merge_85

merge_85:                                         ; preds = %then_84, %merge_83
  %"%drv.load7" = load i64, ptr %drv1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%drv.load7", 3
  br i1 %cmpeqtmp8, label %then_86, label %merge_87

merge_87:                                         ; preds = %then_86, %merge_85
  ret i64 0

then_80:                                          ; preds = %entry
  %"%238.load" = load i64, ptr %val2, align 8
  store i64 %"%238.load", ptr %"%ata_present_0", align 8
  br label %merge_81

then_82:                                          ; preds = %merge_81
  %"%242.load" = load i64, ptr %val2, align 8
  store i64 %"%242.load", ptr %"%ata_present_1", align 8
  br label %merge_83

then_84:                                          ; preds = %merge_83
  %"%246.load" = load i64, ptr %val2, align 8
  store i64 %"%246.load", ptr %"%ata_present_2", align 8
  br label %merge_85

then_86:                                          ; preds = %merge_85
  %"%250.load" = load i64, ptr %val2, align 8
  store i64 %"%250.load", ptr %"%ata_present_3", align 8
  br label %merge_87
}

define i64 @ata_get_present(i64 %drv) {
entry:
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_88, label %merge_89

merge_89:                                         ; preds = %entry
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%drv.load2", 1
  br i1 %cmpeqtmp3, label %then_90, label %merge_91

merge_91:                                         ; preds = %merge_89
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_92, label %merge_93

merge_93:                                         ; preds = %merge_91
  %"%drv.load8" = load i64, ptr %drv1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv.load8", 3
  br i1 %cmpeqtmp9, label %then_94, label %merge_95

merge_95:                                         ; preds = %merge_93
  ret i64 0

then_88:                                          ; preds = %entry
  %loadtmp = load i64, ptr @ata_present_0, align 8
  ret i64 %loadtmp

then_90:                                          ; preds = %merge_89
  %loadtmp4 = load i64, ptr @ata_present_1, align 8
  ret i64 %loadtmp4

then_92:                                          ; preds = %merge_91
  %loadtmp7 = load i64, ptr @ata_present_2, align 8
  ret i64 %loadtmp7

then_94:                                          ; preds = %merge_93
  %loadtmp10 = load i64, ptr @ata_present_3, align 8
  ret i64 %loadtmp10
}

define i64 @ata_set_sectors(i64 %drv, i64 %val) {
entry:
  %"%ata_sectors_3" = alloca i64, align 8
  %"%ata_sectors_2" = alloca i64, align 8
  %"%ata_sectors_1" = alloca i64, align 8
  %"%ata_sectors_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_96, label %merge_97

merge_101:                                        ; preds = %then_100, %merge_99
  %"%drv.load7" = load i64, ptr %drv1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%drv.load7", 3
  br i1 %cmpeqtmp8, label %then_102, label %merge_103

merge_103:                                        ; preds = %then_102, %merge_101
  ret i64 0

merge_97:                                         ; preds = %then_96, %entry
  %"%drv.load3" = load i64, ptr %drv1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%drv.load3", 1
  br i1 %cmpeqtmp4, label %then_98, label %merge_99

merge_99:                                         ; preds = %then_98, %merge_97
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_100, label %merge_101

then_100:                                         ; preds = %merge_99
  %"%284.load" = load i64, ptr %val2, align 8
  store i64 %"%284.load", ptr %"%ata_sectors_2", align 8
  br label %merge_101

then_102:                                         ; preds = %merge_101
  %"%288.load" = load i64, ptr %val2, align 8
  store i64 %"%288.load", ptr %"%ata_sectors_3", align 8
  br label %merge_103

then_96:                                          ; preds = %entry
  %"%276.load" = load i64, ptr %val2, align 8
  store i64 %"%276.load", ptr %"%ata_sectors_0", align 8
  br label %merge_97

then_98:                                          ; preds = %merge_97
  %"%280.load" = load i64, ptr %val2, align 8
  store i64 %"%280.load", ptr %"%ata_sectors_1", align 8
  br label %merge_99
}

define i64 @ata_get_sectors(i64 %drv) {
entry:
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_104, label %merge_105

merge_105:                                        ; preds = %entry
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%drv.load2", 1
  br i1 %cmpeqtmp3, label %then_106, label %merge_107

merge_107:                                        ; preds = %merge_105
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_108, label %merge_109

merge_109:                                        ; preds = %merge_107
  %"%drv.load8" = load i64, ptr %drv1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv.load8", 3
  br i1 %cmpeqtmp9, label %then_110, label %merge_111

merge_111:                                        ; preds = %merge_109
  ret i64 0

then_104:                                         ; preds = %entry
  %loadtmp = load i64, ptr @ata_sectors_0, align 8
  ret i64 %loadtmp

then_106:                                         ; preds = %merge_105
  %loadtmp4 = load i64, ptr @ata_sectors_1, align 8
  ret i64 %loadtmp4

then_108:                                         ; preds = %merge_107
  %loadtmp7 = load i64, ptr @ata_sectors_2, align 8
  ret i64 %loadtmp7

then_110:                                         ; preds = %merge_109
  %loadtmp10 = load i64, ptr @ata_sectors_3, align 8
  ret i64 %loadtmp10
}

define i64 @ata_set_size(i64 %drv, i64 %val) {
entry:
  %"%ata_size_mb_3" = alloca i64, align 8
  %"%ata_size_mb_2" = alloca i64, align 8
  %"%ata_size_mb_1" = alloca i64, align 8
  %"%ata_size_mb_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_112, label %merge_113

merge_113:                                        ; preds = %then_112, %entry
  %"%drv.load3" = load i64, ptr %drv1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%drv.load3", 1
  br i1 %cmpeqtmp4, label %then_114, label %merge_115

merge_115:                                        ; preds = %then_114, %merge_113
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_116, label %merge_117

merge_117:                                        ; preds = %then_116, %merge_115
  %"%drv.load7" = load i64, ptr %drv1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%drv.load7", 3
  br i1 %cmpeqtmp8, label %then_118, label %merge_119

merge_119:                                        ; preds = %then_118, %merge_117
  ret i64 0

then_112:                                         ; preds = %entry
  %"%314.load" = load i64, ptr %val2, align 8
  store i64 %"%314.load", ptr %"%ata_size_mb_0", align 8
  br label %merge_113

then_114:                                         ; preds = %merge_113
  %"%318.load" = load i64, ptr %val2, align 8
  store i64 %"%318.load", ptr %"%ata_size_mb_1", align 8
  br label %merge_115

then_116:                                         ; preds = %merge_115
  %"%322.load" = load i64, ptr %val2, align 8
  store i64 %"%322.load", ptr %"%ata_size_mb_2", align 8
  br label %merge_117

then_118:                                         ; preds = %merge_117
  %"%326.load" = load i64, ptr %val2, align 8
  store i64 %"%326.load", ptr %"%ata_size_mb_3", align 8
  br label %merge_119
}

define i64 @ata_get_size(i64 %drv) {
entry:
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_120, label %merge_121

merge_121:                                        ; preds = %entry
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %cmpeqtmp3 = icmp eq i64 %"%drv.load2", 1
  br i1 %cmpeqtmp3, label %then_122, label %merge_123

merge_123:                                        ; preds = %merge_121
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_124, label %merge_125

merge_125:                                        ; preds = %merge_123
  %"%drv.load8" = load i64, ptr %drv1, align 8
  %cmpeqtmp9 = icmp eq i64 %"%drv.load8", 3
  br i1 %cmpeqtmp9, label %then_126, label %merge_127

merge_127:                                        ; preds = %merge_125
  ret i64 0

then_120:                                         ; preds = %entry
  %loadtmp = load i64, ptr @ata_size_mb_0, align 8
  ret i64 %loadtmp

then_122:                                         ; preds = %merge_121
  %loadtmp4 = load i64, ptr @ata_size_mb_1, align 8
  ret i64 %loadtmp4

then_124:                                         ; preds = %merge_123
  %loadtmp7 = load i64, ptr @ata_size_mb_2, align 8
  ret i64 %loadtmp7

then_126:                                         ; preds = %merge_125
  %loadtmp10 = load i64, ptr @ata_size_mb_3, align 8
  ret i64 %loadtmp10
}

define i64 @ata_set_lba48(i64 %drv, i64 %val) {
entry:
  %"%ata_lba48_3" = alloca i64, align 8
  %"%ata_lba48_2" = alloca i64, align 8
  %"%ata_lba48_1" = alloca i64, align 8
  %"%ata_lba48_0" = alloca i64, align 8
  %val2 = alloca i64, align 8
  store i64 %val, ptr %val2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_128, label %merge_129

merge_129:                                        ; preds = %then_128, %entry
  %"%drv.load3" = load i64, ptr %drv1, align 8
  %cmpeqtmp4 = icmp eq i64 %"%drv.load3", 1
  br i1 %cmpeqtmp4, label %then_130, label %merge_131

merge_131:                                        ; preds = %then_130, %merge_129
  %"%drv.load5" = load i64, ptr %drv1, align 8
  %cmpeqtmp6 = icmp eq i64 %"%drv.load5", 2
  br i1 %cmpeqtmp6, label %then_132, label %merge_133

merge_133:                                        ; preds = %then_132, %merge_131
  %"%drv.load7" = load i64, ptr %drv1, align 8
  %cmpeqtmp8 = icmp eq i64 %"%drv.load7", 3
  br i1 %cmpeqtmp8, label %then_134, label %merge_135

merge_135:                                        ; preds = %then_134, %merge_133
  ret i64 0

then_128:                                         ; preds = %entry
  %"%352.load" = load i64, ptr %val2, align 8
  store i64 %"%352.load", ptr %"%ata_lba48_0", align 8
  br label %merge_129

then_130:                                         ; preds = %merge_129
  %"%356.load" = load i64, ptr %val2, align 8
  store i64 %"%356.load", ptr %"%ata_lba48_1", align 8
  br label %merge_131

then_132:                                         ; preds = %merge_131
  %"%360.load" = load i64, ptr %val2, align 8
  store i64 %"%360.load", ptr %"%ata_lba48_2", align 8
  br label %merge_133

then_134:                                         ; preds = %merge_133
  %"%364.load" = load i64, ptr %val2, align 8
  store i64 %"%364.load", ptr %"%ata_lba48_3", align 8
  br label %merge_135
}

define i64 @ata_delay(i64 %drv) {
entry:
  %"%ctrl" = alloca i64, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_ctrl(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%ctrl", align 8
  %"%ctrl.load" = load i64, ptr %"%ctrl", align 8
  call void @sad_ll_port_inb(i64 %"%ctrl.load")
  %"%ctrl.load2" = load i64, ptr %"%ctrl", align 8
  call void @sad_ll_port_inb(i64 %"%ctrl.load2")
  %"%ctrl.load3" = load i64, ptr %"%ctrl", align 8
  call void @sad_ll_port_inb(i64 %"%ctrl.load3")
  %"%ctrl.load4" = load i64, ptr %"%ctrl", align 8
  call void @sad_ll_port_inb(i64 %"%ctrl.load4")
  ret i64 0
}

define i64 @ata_wait_bsy(i64 %drv) {
entry:
  %"%sts" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_base(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%base", align 8
  store i64 100000, ptr %"%timeout", align 8
  br label %while_cond_136

merge_140:                                        ; preds = %while_body_137
  %"%timeout.load2" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load2", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_136

then_139:                                         ; preds = %while_body_137
  %loadtmp = load i64, ptr %"%sts", align 8
  ret i64 %loadtmp

while_body_137:                                   ; preds = %while_cond_136
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%ATA_REG_STATUS.load" = load i64, ptr @ATA_REG_STATUS, align 8
  %addtmp = add i64 %"%base.load", %"%ATA_REG_STATUS.load"
  call void @sad_ll_port_inb(i64 %addtmp)
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %shrtmp = lshr i64 %"%sts.load", 7
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp = icmp eq i64 %andtmp, 0
  br i1 %cmpeqtmp, label %then_139, label %merge_140

while_cond_136:                                   ; preds = %merge_140, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_137, label %while_exit_138

while_exit_138:                                   ; preds = %while_cond_136
  ret i64 -1
}

define i64 @ata_wait_drq(i64 %drv) {
entry:
  %"%sts" = alloca i64, align 8
  %"%timeout" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_base(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%base", align 8
  store i64 100000, ptr %"%timeout", align 8
  br label %while_cond_141

merge_145:                                        ; preds = %while_body_142
  %"%sts.load2" = load i64, ptr %"%sts", align 8
  %shrtmp = lshr i64 %"%sts.load2", 5
  %andtmp3 = and i64 %shrtmp, 1
  %cmpeqtmp4 = icmp eq i64 %andtmp3, 1
  br i1 %cmpeqtmp4, label %then_146, label %merge_147

merge_147:                                        ; preds = %merge_145
  %"%sts.load5" = load i64, ptr %"%sts", align 8
  %shrtmp6 = lshr i64 %"%sts.load5", 3
  %andtmp7 = and i64 %shrtmp6, 1
  %cmpeqtmp8 = icmp eq i64 %andtmp7, 1
  br i1 %cmpeqtmp8, label %then_148, label %merge_149

merge_149:                                        ; preds = %merge_147
  %"%timeout.load9" = load i64, ptr %"%timeout", align 8
  %subtmp = sub i64 %"%timeout.load9", 1
  store i64 %subtmp, ptr %"%timeout", align 8
  br label %while_cond_141

then_144:                                         ; preds = %while_body_142
  ret i64 -1

then_146:                                         ; preds = %merge_145
  ret i64 -2

then_148:                                         ; preds = %merge_147
  ret i64 0

while_body_142:                                   ; preds = %while_cond_141
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%ATA_REG_STATUS.load" = load i64, ptr @ATA_REG_STATUS, align 8
  %addtmp = add i64 %"%base.load", %"%ATA_REG_STATUS.load"
  call void @sad_ll_port_inb(i64 %addtmp)
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %andtmp = and i64 %"%sts.load", 1
  %cmpeqtmp = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp, label %then_144, label %merge_145

while_cond_141:                                   ; preds = %merge_149, %entry
  %"%timeout.load" = load i64, ptr %"%timeout", align 8
  %cmpgttmp = icmp sgt i64 %"%timeout.load", 0
  br i1 %cmpgttmp, label %while_body_142, label %while_exit_143

while_exit_143:                                   ; preds = %while_cond_141
  ret i64 -3
}

define i64 @ata_select_drive(i64 %drv) {
entry:
  %"%sel" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_base(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %calltmp3 = call i64 @ata_get_select(i64 %"%drv.load2")
  store i64 %calltmp3, ptr %"%sel", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%ATA_REG_DRIVE.load" = load i64, ptr @ATA_REG_DRIVE, align 8
  %addtmp = add i64 %"%base.load", %"%ATA_REG_DRIVE.load"
  %"%sel.load" = load i64, ptr %"%sel", align 8
  call void @sad_ll_port_outb(i64 %addtmp, i64 %"%sel.load")
  %"%drv.load4" = load i64, ptr %drv1, align 8
  %calltmp5 = call i64 @ata_delay(i64 %"%drv.load4")
  ret i64 0
}

define i64 @ata_soft_reset(i64 %drv) {
entry:
  %"%ctrl" = alloca i64, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_ctrl(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%ctrl", align 8
  %"%ctrl.load" = load i64, ptr %"%ctrl", align 8
  call void @sad_ll_port_outb(i64 %"%ctrl.load", i64 4)
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %calltmp3 = call i64 @ata_delay(i64 %"%drv.load2")
  %"%drv.load4" = load i64, ptr %drv1, align 8
  %calltmp5 = call i64 @ata_delay(i64 %"%drv.load4")
  %"%ctrl.load6" = load i64, ptr %"%ctrl", align 8
  call void @sad_ll_port_outb(i64 %"%ctrl.load6", i64 0)
  %"%drv.load7" = load i64, ptr %drv1, align 8
  %calltmp8 = call i64 @ata_wait_bsy(i64 %"%drv.load7")
  ret i64 0
}

define i64 @ata_identify(i64 %drv) {
entry:
  %"%ata_drives_found" = alloca i64, align 8
  %"%size_mb" = alloca i64, align 8
  %"%total_sectors" = alloca i64, align 8
  %"%lba48_lo" = alloca i64, align 8
  %"%word83" = alloca i64, align 8
  %"%lba28_sectors" = alloca i64, align 8
  %"%word" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%buf" = alloca i64, align 8
  %"%drq_result" = alloca i64, align 8
  %"%lba_hi" = alloca i64, align 8
  %"%lba_mid" = alloca i64, align 8
  %"%wait_result" = alloca i64, align 8
  %"%sts" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_base(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%drv.load2" = load i64, ptr %drv1, align 8
  %calltmp3 = call i64 @ata_select_drive(i64 %"%drv.load2")
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%ATA_REG_COUNT.load" = load i64, ptr @ATA_REG_COUNT, align 8
  %addtmp = add i64 %"%base.load", %"%ATA_REG_COUNT.load"
  call void @sad_ll_port_outb(i64 %addtmp, i64 0)
  %"%base.load4" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBALO.load" = load i64, ptr @ATA_REG_LBALO, align 8
  %addtmp5 = add i64 %"%base.load4", %"%ATA_REG_LBALO.load"
  call void @sad_ll_port_outb(i64 %addtmp5, i64 0)
  %"%base.load6" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAMID.load" = load i64, ptr @ATA_REG_LBAMID, align 8
  %addtmp7 = add i64 %"%base.load6", %"%ATA_REG_LBAMID.load"
  call void @sad_ll_port_outb(i64 %addtmp7, i64 0)
  %"%base.load8" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAHI.load" = load i64, ptr @ATA_REG_LBAHI, align 8
  %addtmp9 = add i64 %"%base.load8", %"%ATA_REG_LBAHI.load"
  call void @sad_ll_port_outb(i64 %addtmp9, i64 0)
  %"%base.load10" = load i64, ptr %"%base", align 8
  %"%ATA_REG_STATUS.load" = load i64, ptr @ATA_REG_STATUS, align 8
  %addtmp11 = add i64 %"%base.load10", %"%ATA_REG_STATUS.load"
  %"%ATA_CMD_IDENTIFY.load" = load i64, ptr @ATA_CMD_IDENTIFY, align 8
  call void @sad_ll_port_outb(i64 %addtmp11, i64 %"%ATA_CMD_IDENTIFY.load")
  %"%drv.load12" = load i64, ptr %drv1, align 8
  %calltmp13 = call i64 @ata_delay(i64 %"%drv.load12")
  call void @sad_ll_port_inb(i64 %addtmp11)
  store i64 0, ptr %"%sts", align 8
  %"%sts.load" = load i64, ptr %"%sts", align 8
  %cmpeqtmp = icmp eq i64 %"%sts.load", 0
  br i1 %cmpeqtmp, label %then_150, label %merge_151

merge_151:                                        ; preds = %entry
  %"%drv.load14" = load i64, ptr %drv1, align 8
  %calltmp15 = call i64 @ata_wait_bsy(i64 %"%drv.load14")
  store i64 %calltmp15, ptr %"%wait_result", align 8
  %"%wait_result.load" = load i64, ptr %"%wait_result", align 8
  %cmplttmp = icmp slt i64 %"%wait_result.load", 0
  br i1 %cmplttmp, label %then_152, label %merge_153

merge_153:                                        ; preds = %merge_151
  %"%base.load16" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAMID.load17" = load i64, ptr @ATA_REG_LBAMID, align 8
  %addtmp18 = add i64 %"%base.load16", %"%ATA_REG_LBAMID.load17"
  call void @sad_ll_port_inb(i64 %addtmp18)
  store i64 0, ptr %"%lba_mid", align 8
  %"%base.load19" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAHI.load20" = load i64, ptr @ATA_REG_LBAHI, align 8
  %addtmp21 = add i64 %"%base.load19", %"%ATA_REG_LBAHI.load20"
  call void @sad_ll_port_inb(i64 %addtmp21)
  store i64 0, ptr %"%lba_hi", align 8
  %"%lba_mid.load" = load i64, ptr %"%lba_mid", align 8
  %cmpgttmp = icmp sgt i64 %"%lba_mid.load", 0
  br i1 %cmpgttmp, label %then_154, label %merge_155

merge_155:                                        ; preds = %merge_157, %merge_153
  %"%drv.load23" = load i64, ptr %drv1, align 8
  %calltmp24 = call i64 @ata_wait_drq(i64 %"%drv.load23")
  store i64 %calltmp24, ptr %"%drq_result", align 8
  %"%drq_result.load" = load i64, ptr %"%drq_result", align 8
  %cmplttmp25 = icmp slt i64 %"%drq_result.load", 0
  br i1 %cmplttmp25, label %then_162, label %merge_163

merge_157:                                        ; preds = %merge_159, %then_154
  br label %merge_155

merge_159:                                        ; preds = %merge_161, %then_156
  br label %merge_157

merge_161:                                        ; preds = %then_158
  br label %merge_159

merge_163:                                        ; preds = %merge_155
  %"%541.load" = load i64, ptr @ata_identify_buf, align 8
  store i64 %"%541.load", ptr %"%buf", align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_164

merge_168:                                        ; preds = %merge_170, %while_exit_166
  %"%drv.load60" = load i64, ptr %drv1, align 8
  %calltmp61 = call i64 @ata_get_sectors(i64 %"%drv.load60")
  store i64 %calltmp61, ptr %"%total_sectors", align 8
  %"%total_sectors.load" = load i64, ptr %"%total_sectors", align 8
  %shrtmp62 = lshr i64 %"%total_sectors.load", 11
  store i64 %shrtmp62, ptr %"%size_mb", align 8
  %"%drv.load63" = load i64, ptr %drv1, align 8
  %"%size_mb.load" = load i64, ptr %"%size_mb", align 8
  %calltmp64 = call i64 @ata_set_size(i64 %"%drv.load63", i64 %"%size_mb.load")
  %"%drv.load65" = load i64, ptr %drv1, align 8
  %"%buf.load66" = load i64, ptr %"%buf", align 8
  %calltmp67 = call i64 @ata_extract_model(i64 %"%drv.load65", i64 %"%buf.load66")
  %calltmp68 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%596.load" = load i64, ptr %drv1, align 8
  %calltmp69 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%596.load")
  %calltmp70 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.3 to i64))
  %"%601.load" = load i64, ptr %"%size_mb", align 8
  %calltmp71 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%601.load")
  %calltmp72 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%606.load" = load i64, ptr %"%total_sectors", align 8
  %calltmp73 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%606.load")
  %calltmp74 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %calltmp75 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  %"%ata_drives_found.load" = load i64, ptr @ata_drives_found, align 8
  %addtmp76 = add i64 %"%ata_drives_found.load", 1
  store i64 %addtmp76, ptr %"%ata_drives_found", align 8
  ret i64 0

merge_170:                                        ; preds = %then_169, %then_167
  br label %merge_168

then_150:                                         ; preds = %entry
  ret i64 -1

then_152:                                         ; preds = %merge_151
  ret i64 -2

then_154:                                         ; preds = %merge_153
  %"%lba_hi.load" = load i64, ptr %"%lba_hi", align 8
  %cmpgttmp22 = icmp sgt i64 %"%lba_hi.load", 0
  br i1 %cmpgttmp22, label %then_156, label %merge_157

then_156:                                         ; preds = %then_154
  %"%lba_mid.load26" = load i64, ptr %"%lba_mid", align 8
  %cmpeqtmp27 = icmp eq i64 %"%lba_mid.load26", 20
  br i1 %cmpeqtmp27, label %then_158, label %merge_159

then_158:                                         ; preds = %then_156
  %"%lba_hi.load28" = load i64, ptr %"%lba_hi", align 8
  %cmpeqtmp29 = icmp eq i64 %"%lba_hi.load28", 235
  br i1 %cmpeqtmp29, label %then_160, label %merge_161

then_160:                                         ; preds = %then_158
  %"%drv.load30" = load i64, ptr %drv1, align 8
  %"%ATA_TYPE_ATAPI.load" = load i64, ptr @ATA_TYPE_ATAPI, align 8
  %calltmp31 = call i64 @ata_set_type(i64 %"%drv.load30", i64 %"%ATA_TYPE_ATAPI.load")
  %calltmp32 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const to i64))
  %"%523.load" = load i64, ptr %drv1, align 8
  %calltmp33 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%523.load")
  %calltmp34 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.1 to i64))
  %calltmp35 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -3

then_162:                                         ; preds = %merge_155
  %loadtmp = load i64, ptr %"%drq_result", align 8
  ret i64 %loadtmp

then_167:                                         ; preds = %while_exit_166
  %"%drv.load54" = load i64, ptr %drv1, align 8
  %calltmp55 = call i64 @ata_set_lba48(i64 %"%drv.load54", i64 1)
  %"%buf.load56" = load i64, ptr %"%buf", align 8
  %addtmp57 = add i64 %"%buf.load56", 200
  call void @sad_ll_mem_read32(i64 %addtmp57)
  store i64 0, ptr %"%lba48_lo", align 8
  %"%lba48_lo.load" = load i64, ptr %"%lba48_lo", align 8
  %"%lba28_sectors.load58" = load i64, ptr %"%lba28_sectors", align 8
  %cmpgttmp59 = icmp sgt i64 %"%lba48_lo.load", %"%lba28_sectors.load58"
  br i1 %cmpgttmp59, label %then_169, label %merge_170

then_169:                                         ; preds = %then_167
  %"%drv.load77" = load i64, ptr %drv1, align 8
  %"%lba48_lo.load78" = load i64, ptr %"%lba48_lo", align 8
  %calltmp79 = call i64 @ata_set_sectors(i64 %"%drv.load77", i64 %"%lba48_lo.load78")
  br label %merge_170

while_body_165:                                   ; preds = %while_cond_164
  %"%base.load37" = load i64, ptr %"%base", align 8
  %"%ATA_REG_DATA.load" = load i64, ptr @ATA_REG_DATA, align 8
  %addtmp38 = add i64 %"%base.load37", %"%ATA_REG_DATA.load"
  call void @sad_ll_port_inw(i64 %addtmp38)
  store i64 0, ptr %"%word", align 8
  %"%i.load39" = load i64, ptr %"%i", align 8
  %shltmp = shl i64 %"%i.load39", 1
  %"%buf.load" = load i64, ptr %"%buf", align 8
  %addtmp40 = add i64 %"%buf.load", %shltmp
  %"%word.load" = load i64, ptr %"%word", align 8
  call void @sad_ll_mem_write16(i64 %addtmp40, i64 %"%word.load")
  %"%i.load41" = load i64, ptr %"%i", align 8
  %addtmp42 = add i64 %"%i.load41", 1
  store i64 %addtmp42, ptr %"%i", align 8
  br label %while_cond_164

while_cond_164:                                   ; preds = %while_body_165, %merge_163
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp36 = icmp slt i64 %"%i.load", 256
  br i1 %cmplttmp36, label %while_body_165, label %while_exit_166

while_exit_166:                                   ; preds = %while_cond_164
  %"%drv.load43" = load i64, ptr %drv1, align 8
  %"%ATA_TYPE_ATA.load" = load i64, ptr @ATA_TYPE_ATA, align 8
  %calltmp44 = call i64 @ata_set_type(i64 %"%drv.load43", i64 %"%ATA_TYPE_ATA.load")
  %"%drv.load45" = load i64, ptr %drv1, align 8
  %calltmp46 = call i64 @ata_set_present(i64 %"%drv.load45", i64 1)
  %"%buf.load47" = load i64, ptr %"%buf", align 8
  %addtmp48 = add i64 %"%buf.load47", 120
  call void @sad_ll_mem_read32(i64 %addtmp48)
  store i64 0, ptr %"%lba28_sectors", align 8
  %"%drv.load49" = load i64, ptr %drv1, align 8
  %"%lba28_sectors.load" = load i64, ptr %"%lba28_sectors", align 8
  %calltmp50 = call i64 @ata_set_sectors(i64 %"%drv.load49", i64 %"%lba28_sectors.load")
  %"%buf.load51" = load i64, ptr %"%buf", align 8
  %addtmp52 = add i64 %"%buf.load51", 166
  call void @sad_ll_mem_read16(i64 %addtmp52)
  store i64 0, ptr %"%word83", align 8
  %"%word83.load" = load i64, ptr %"%word83", align 8
  %shrtmp = lshr i64 %"%word83.load", 10
  %andtmp = and i64 %shrtmp, 1
  %cmpeqtmp53 = icmp eq i64 %andtmp, 1
  br i1 %cmpeqtmp53, label %then_167, label %merge_168
}

define i64 @ata_extract_model(i64 %drv, i64 %buf) {
entry:
  %"%lo" = alloca i64, align 8
  %"%hi" = alloca i64, align 8
  %"%w" = alloca i64, align 8
  %"%j" = alloca i64, align 8
  %"%ata_model_3_9" = alloca i64, align 8
  %"%ata_model_3_8" = alloca i64, align 8
  %"%ata_model_3_7" = alloca i64, align 8
  %"%ata_model_3_6" = alloca i64, align 8
  %"%ata_model_3_5" = alloca i64, align 8
  %"%ata_model_3_4" = alloca i64, align 8
  %"%ata_model_3_3" = alloca i64, align 8
  %"%ata_model_3_2" = alloca i64, align 8
  %"%ata_model_3_1" = alloca i64, align 8
  %"%ata_model_3_0" = alloca i64, align 8
  %"%ata_model_2_9" = alloca i64, align 8
  %"%ata_model_2_8" = alloca i64, align 8
  %"%ata_model_2_7" = alloca i64, align 8
  %"%ata_model_2_6" = alloca i64, align 8
  %"%ata_model_2_5" = alloca i64, align 8
  %"%ata_model_2_4" = alloca i64, align 8
  %"%ata_model_2_3" = alloca i64, align 8
  %"%ata_model_2_2" = alloca i64, align 8
  %"%ata_model_2_1" = alloca i64, align 8
  %"%ata_model_2_0" = alloca i64, align 8
  %"%ata_model_1_9" = alloca i64, align 8
  %"%ata_model_1_8" = alloca i64, align 8
  %"%ata_model_1_7" = alloca i64, align 8
  %"%ata_model_1_6" = alloca i64, align 8
  %"%ata_model_1_5" = alloca i64, align 8
  %"%ata_model_1_4" = alloca i64, align 8
  %"%ata_model_1_3" = alloca i64, align 8
  %"%ata_model_1_2" = alloca i64, align 8
  %"%ata_model_1_1" = alloca i64, align 8
  %"%ata_model_1_0" = alloca i64, align 8
  %"%ata_model_0_9" = alloca i64, align 8
  %"%ata_model_0_8" = alloca i64, align 8
  %"%ata_model_0_7" = alloca i64, align 8
  %"%ata_model_0_6" = alloca i64, align 8
  %"%ata_model_0_5" = alloca i64, align 8
  %"%ata_model_0_4" = alloca i64, align 8
  %"%ata_model_0_3" = alloca i64, align 8
  %"%ata_model_0_2" = alloca i64, align 8
  %"%ata_model_0_1" = alloca i64, align 8
  %"%ata_model_0_0" = alloca i64, align 8
  %"%w9" = alloca i64, align 8
  %"%w8" = alloca i64, align 8
  %"%w7" = alloca i64, align 8
  %"%w6" = alloca i64, align 8
  %"%w5" = alloca i64, align 8
  %"%w4" = alloca i64, align 8
  %"%w3" = alloca i64, align 8
  %"%w2" = alloca i64, align 8
  %"%w1" = alloca i64, align 8
  %"%w0" = alloca i64, align 8
  %"%model_base" = alloca i64, align 8
  %buf2 = alloca i64, align 8
  store i64 %buf, ptr %buf2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%buf.load" = load i64, ptr %buf2, align 8
  %addtmp = add i64 %"%buf.load", 54
  store i64 %addtmp, ptr %"%model_base", align 8
  %"%model_base.load" = load i64, ptr %"%model_base", align 8
  call void @sad_ll_mem_read16(i64 %"%model_base.load")
  store i64 0, ptr %"%w0", align 8
  %"%model_base.load3" = load i64, ptr %"%model_base", align 8
  %addtmp4 = add i64 %"%model_base.load3", 2
  call void @sad_ll_mem_read16(i64 %addtmp4)
  store i64 0, ptr %"%w1", align 8
  %"%model_base.load5" = load i64, ptr %"%model_base", align 8
  %addtmp6 = add i64 %"%model_base.load5", 4
  call void @sad_ll_mem_read16(i64 %addtmp6)
  store i64 0, ptr %"%w2", align 8
  %"%model_base.load7" = load i64, ptr %"%model_base", align 8
  %addtmp8 = add i64 %"%model_base.load7", 6
  call void @sad_ll_mem_read16(i64 %addtmp8)
  store i64 0, ptr %"%w3", align 8
  %"%model_base.load9" = load i64, ptr %"%model_base", align 8
  %addtmp10 = add i64 %"%model_base.load9", 8
  call void @sad_ll_mem_read16(i64 %addtmp10)
  store i64 0, ptr %"%w4", align 8
  %"%model_base.load11" = load i64, ptr %"%model_base", align 8
  %addtmp12 = add i64 %"%model_base.load11", 10
  call void @sad_ll_mem_read16(i64 %addtmp12)
  store i64 0, ptr %"%w5", align 8
  %"%model_base.load13" = load i64, ptr %"%model_base", align 8
  %addtmp14 = add i64 %"%model_base.load13", 12
  call void @sad_ll_mem_read16(i64 %addtmp14)
  store i64 0, ptr %"%w6", align 8
  %"%model_base.load15" = load i64, ptr %"%model_base", align 8
  %addtmp16 = add i64 %"%model_base.load15", 14
  call void @sad_ll_mem_read16(i64 %addtmp16)
  store i64 0, ptr %"%w7", align 8
  %"%model_base.load17" = load i64, ptr %"%model_base", align 8
  %addtmp18 = add i64 %"%model_base.load17", 16
  call void @sad_ll_mem_read16(i64 %addtmp18)
  store i64 0, ptr %"%w8", align 8
  %"%model_base.load19" = load i64, ptr %"%model_base", align 8
  %addtmp20 = add i64 %"%model_base.load19", 18
  call void @sad_ll_mem_read16(i64 %addtmp20)
  store i64 0, ptr %"%w9", align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %cmpeqtmp = icmp eq i64 %"%drv.load", 0
  br i1 %cmpeqtmp, label %then_171, label %merge_172

merge_172:                                        ; preds = %then_171, %entry
  %"%drv.load21" = load i64, ptr %drv1, align 8
  %cmpeqtmp22 = icmp eq i64 %"%drv.load21", 1
  br i1 %cmpeqtmp22, label %then_173, label %merge_174

merge_174:                                        ; preds = %then_173, %merge_172
  %"%drv.load23" = load i64, ptr %drv1, align 8
  %cmpeqtmp24 = icmp eq i64 %"%drv.load23", 2
  br i1 %cmpeqtmp24, label %then_175, label %merge_176

merge_176:                                        ; preds = %then_175, %merge_174
  %"%drv.load25" = load i64, ptr %drv1, align 8
  %cmpeqtmp26 = icmp eq i64 %"%drv.load25", 3
  br i1 %cmpeqtmp26, label %then_177, label %merge_178

merge_178:                                        ; preds = %then_177, %merge_176
  %calltmp = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.6 to i64))
  store i64 0, ptr %"%j", align 8
  br label %while_cond_179

merge_183:                                        ; preds = %merge_185, %while_body_180
  %"%lo.load" = load i64, ptr %"%lo", align 8
  %cmpgttmp34 = icmp sgt i64 %"%lo.load", 31
  br i1 %cmpgttmp34, label %then_186, label %merge_187

merge_185:                                        ; preds = %then_184, %then_182
  br label %merge_183

merge_187:                                        ; preds = %merge_189, %merge_183
  %"%j.load38" = load i64, ptr %"%j", align 8
  %addtmp39 = add i64 %"%j.load38", 1
  store i64 %addtmp39, ptr %"%j", align 8
  br label %while_cond_179

merge_189:                                        ; preds = %then_188, %then_186
  br label %merge_187

then_171:                                         ; preds = %entry
  %"%667.load" = load i64, ptr %"%w0", align 8
  store i64 %"%667.load", ptr %"%ata_model_0_0", align 8
  %"%669.load" = load i64, ptr %"%w1", align 8
  store i64 %"%669.load", ptr %"%ata_model_0_1", align 8
  %"%671.load" = load i64, ptr %"%w2", align 8
  store i64 %"%671.load", ptr %"%ata_model_0_2", align 8
  %"%673.load" = load i64, ptr %"%w3", align 8
  store i64 %"%673.load", ptr %"%ata_model_0_3", align 8
  %"%675.load" = load i64, ptr %"%w4", align 8
  store i64 %"%675.load", ptr %"%ata_model_0_4", align 8
  %"%677.load" = load i64, ptr %"%w5", align 8
  store i64 %"%677.load", ptr %"%ata_model_0_5", align 8
  %"%679.load" = load i64, ptr %"%w6", align 8
  store i64 %"%679.load", ptr %"%ata_model_0_6", align 8
  %"%681.load" = load i64, ptr %"%w7", align 8
  store i64 %"%681.load", ptr %"%ata_model_0_7", align 8
  %"%683.load" = load i64, ptr %"%w8", align 8
  store i64 %"%683.load", ptr %"%ata_model_0_8", align 8
  %"%685.load" = load i64, ptr %"%w9", align 8
  store i64 %"%685.load", ptr %"%ata_model_0_9", align 8
  br label %merge_172

then_173:                                         ; preds = %merge_172
  %"%689.load" = load i64, ptr %"%w0", align 8
  store i64 %"%689.load", ptr %"%ata_model_1_0", align 8
  %"%691.load" = load i64, ptr %"%w1", align 8
  store i64 %"%691.load", ptr %"%ata_model_1_1", align 8
  %"%693.load" = load i64, ptr %"%w2", align 8
  store i64 %"%693.load", ptr %"%ata_model_1_2", align 8
  %"%695.load" = load i64, ptr %"%w3", align 8
  store i64 %"%695.load", ptr %"%ata_model_1_3", align 8
  %"%697.load" = load i64, ptr %"%w4", align 8
  store i64 %"%697.load", ptr %"%ata_model_1_4", align 8
  %"%699.load" = load i64, ptr %"%w5", align 8
  store i64 %"%699.load", ptr %"%ata_model_1_5", align 8
  %"%701.load" = load i64, ptr %"%w6", align 8
  store i64 %"%701.load", ptr %"%ata_model_1_6", align 8
  %"%703.load" = load i64, ptr %"%w7", align 8
  store i64 %"%703.load", ptr %"%ata_model_1_7", align 8
  %"%705.load" = load i64, ptr %"%w8", align 8
  store i64 %"%705.load", ptr %"%ata_model_1_8", align 8
  %"%707.load" = load i64, ptr %"%w9", align 8
  store i64 %"%707.load", ptr %"%ata_model_1_9", align 8
  br label %merge_174

then_175:                                         ; preds = %merge_174
  %"%711.load" = load i64, ptr %"%w0", align 8
  store i64 %"%711.load", ptr %"%ata_model_2_0", align 8
  %"%713.load" = load i64, ptr %"%w1", align 8
  store i64 %"%713.load", ptr %"%ata_model_2_1", align 8
  %"%715.load" = load i64, ptr %"%w2", align 8
  store i64 %"%715.load", ptr %"%ata_model_2_2", align 8
  %"%717.load" = load i64, ptr %"%w3", align 8
  store i64 %"%717.load", ptr %"%ata_model_2_3", align 8
  %"%719.load" = load i64, ptr %"%w4", align 8
  store i64 %"%719.load", ptr %"%ata_model_2_4", align 8
  %"%721.load" = load i64, ptr %"%w5", align 8
  store i64 %"%721.load", ptr %"%ata_model_2_5", align 8
  %"%723.load" = load i64, ptr %"%w6", align 8
  store i64 %"%723.load", ptr %"%ata_model_2_6", align 8
  %"%725.load" = load i64, ptr %"%w7", align 8
  store i64 %"%725.load", ptr %"%ata_model_2_7", align 8
  %"%727.load" = load i64, ptr %"%w8", align 8
  store i64 %"%727.load", ptr %"%ata_model_2_8", align 8
  %"%729.load" = load i64, ptr %"%w9", align 8
  store i64 %"%729.load", ptr %"%ata_model_2_9", align 8
  br label %merge_176

then_177:                                         ; preds = %merge_176
  %"%733.load" = load i64, ptr %"%w0", align 8
  store i64 %"%733.load", ptr %"%ata_model_3_0", align 8
  %"%735.load" = load i64, ptr %"%w1", align 8
  store i64 %"%735.load", ptr %"%ata_model_3_1", align 8
  %"%737.load" = load i64, ptr %"%w2", align 8
  store i64 %"%737.load", ptr %"%ata_model_3_2", align 8
  %"%739.load" = load i64, ptr %"%w3", align 8
  store i64 %"%739.load", ptr %"%ata_model_3_3", align 8
  %"%741.load" = load i64, ptr %"%w4", align 8
  store i64 %"%741.load", ptr %"%ata_model_3_4", align 8
  %"%743.load" = load i64, ptr %"%w5", align 8
  store i64 %"%743.load", ptr %"%ata_model_3_5", align 8
  %"%745.load" = load i64, ptr %"%w6", align 8
  store i64 %"%745.load", ptr %"%ata_model_3_6", align 8
  %"%747.load" = load i64, ptr %"%w7", align 8
  store i64 %"%747.load", ptr %"%ata_model_3_7", align 8
  %"%749.load" = load i64, ptr %"%w8", align 8
  store i64 %"%749.load", ptr %"%ata_model_3_8", align 8
  %"%751.load" = load i64, ptr %"%w9", align 8
  store i64 %"%751.load", ptr %"%ata_model_3_9", align 8
  br label %merge_178

then_182:                                         ; preds = %while_body_180
  %"%hi.load32" = load i64, ptr %"%hi", align 8
  %cmplttmp33 = icmp slt i64 %"%hi.load32", 127
  br i1 %cmplttmp33, label %then_184, label %merge_185

then_184:                                         ; preds = %then_182
  %"%COM1.load" = load i64, ptr @COM1, align 8
  %"%hi.load35" = load i64, ptr %"%hi", align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load", i64 %"%hi.load35")
  br label %merge_185

then_186:                                         ; preds = %merge_183
  %"%lo.load36" = load i64, ptr %"%lo", align 8
  %cmplttmp37 = icmp slt i64 %"%lo.load36", 127
  br i1 %cmplttmp37, label %then_188, label %merge_189

then_188:                                         ; preds = %then_186
  %"%COM1.load40" = load i64, ptr @COM1, align 8
  %"%lo.load41" = load i64, ptr %"%lo", align 8
  call void @sad_ll_serial_write_char(i64 %"%COM1.load40", i64 %"%lo.load41")
  br label %merge_189

while_body_180:                                   ; preds = %while_cond_179
  %"%j.load27" = load i64, ptr %"%j", align 8
  %shltmp = shl i64 %"%j.load27", 1
  %"%model_base.load28" = load i64, ptr %"%model_base", align 8
  %addtmp29 = add i64 %"%model_base.load28", %shltmp
  call void @sad_ll_mem_read16(i64 %addtmp29)
  store i64 0, ptr %"%w", align 8
  %"%w.load" = load i64, ptr %"%w", align 8
  %shrtmp = lshr i64 %"%w.load", 8
  store i64 %shrtmp, ptr %"%hi", align 8
  %"%w.load30" = load i64, ptr %"%w", align 8
  %andtmp = and i64 %"%w.load30", 255
  store i64 %andtmp, ptr %"%lo", align 8
  %"%hi.load" = load i64, ptr %"%hi", align 8
  %cmpgttmp = icmp sgt i64 %"%hi.load", 31
  br i1 %cmpgttmp, label %then_182, label %merge_183

while_cond_179:                                   ; preds = %merge_187, %merge_178
  %"%j.load" = load i64, ptr %"%j", align 8
  %cmplttmp = icmp slt i64 %"%j.load", 10
  br i1 %cmplttmp, label %while_body_180, label %while_exit_181

while_exit_181:                                   ; preds = %while_cond_179
  %calltmp31 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @ata_read_sector(i64 %drv, i64 %lba, i64 %buf) {
entry:
  %"%ata_reads" = alloca i64, align 8
  %"%word" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%ata_errors" = alloca i64, align 8
  %"%drq_rc" = alloca i64, align 8
  %"%head" = alloca i64, align 8
  %"%sel" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %buf3 = alloca i64, align 8
  store i64 %buf, ptr %buf3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_base(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%drv.load4" = load i64, ptr %drv1, align 8
  %calltmp5 = call i64 @ata_get_select(i64 %"%drv.load4")
  store i64 %calltmp5, ptr %"%sel", align 8
  %"%drv.load6" = load i64, ptr %drv1, align 8
  %calltmp7 = call i64 @ata_wait_bsy(i64 %"%drv.load6")
  %"%sel.load" = load i64, ptr %"%sel", align 8
  %addtmp = add i64 %"%sel.load", 64
  %"%lba.load" = load i64, ptr %lba2, align 8
  %shrtmp = lshr i64 %"%lba.load", 24
  %andtmp = and i64 %shrtmp, 15
  %addtmp8 = add i64 %addtmp, %andtmp
  store i64 %addtmp8, ptr %"%head", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%ATA_REG_DRIVE.load" = load i64, ptr @ATA_REG_DRIVE, align 8
  %addtmp9 = add i64 %"%base.load", %"%ATA_REG_DRIVE.load"
  %"%head.load" = load i64, ptr %"%head", align 8
  call void @sad_ll_port_outb(i64 %addtmp9, i64 %"%head.load")
  %"%base.load10" = load i64, ptr %"%base", align 8
  %"%ATA_REG_COUNT.load" = load i64, ptr @ATA_REG_COUNT, align 8
  %addtmp11 = add i64 %"%base.load10", %"%ATA_REG_COUNT.load"
  call void @sad_ll_port_outb(i64 %addtmp11, i64 1)
  %"%base.load12" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBALO.load" = load i64, ptr @ATA_REG_LBALO, align 8
  %addtmp13 = add i64 %"%base.load12", %"%ATA_REG_LBALO.load"
  %"%lba.load14" = load i64, ptr %lba2, align 8
  %andtmp15 = and i64 %"%lba.load14", 255
  call void @sad_ll_port_outb(i64 %addtmp13, i64 %andtmp15)
  %"%base.load16" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAMID.load" = load i64, ptr @ATA_REG_LBAMID, align 8
  %addtmp17 = add i64 %"%base.load16", %"%ATA_REG_LBAMID.load"
  %"%lba.load18" = load i64, ptr %lba2, align 8
  %shrtmp19 = lshr i64 %"%lba.load18", 8
  %andtmp20 = and i64 %shrtmp19, 255
  call void @sad_ll_port_outb(i64 %addtmp17, i64 %andtmp20)
  %"%base.load21" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAHI.load" = load i64, ptr @ATA_REG_LBAHI, align 8
  %addtmp22 = add i64 %"%base.load21", %"%ATA_REG_LBAHI.load"
  %"%lba.load23" = load i64, ptr %lba2, align 8
  %shrtmp24 = lshr i64 %"%lba.load23", 16
  %andtmp25 = and i64 %shrtmp24, 255
  call void @sad_ll_port_outb(i64 %addtmp22, i64 %andtmp25)
  %"%base.load26" = load i64, ptr %"%base", align 8
  %"%ATA_REG_STATUS.load" = load i64, ptr @ATA_REG_STATUS, align 8
  %addtmp27 = add i64 %"%base.load26", %"%ATA_REG_STATUS.load"
  %"%ATA_CMD_READ_PIO.load" = load i64, ptr @ATA_CMD_READ_PIO, align 8
  call void @sad_ll_port_outb(i64 %addtmp27, i64 %"%ATA_CMD_READ_PIO.load")
  %"%drv.load28" = load i64, ptr %drv1, align 8
  %calltmp29 = call i64 @ata_wait_drq(i64 %"%drv.load28")
  store i64 %calltmp29, ptr %"%drq_rc", align 8
  %"%drq_rc.load" = load i64, ptr %"%drq_rc", align 8
  %cmplttmp = icmp slt i64 %"%drq_rc.load", 0
  br i1 %cmplttmp, label %then_190, label %merge_191

merge_191:                                        ; preds = %entry
  store i64 0, ptr %"%i", align 8
  br label %while_cond_192

then_190:                                         ; preds = %entry
  %"%ata_errors.load" = load i64, ptr @ata_errors, align 8
  %addtmp30 = add i64 %"%ata_errors.load", 1
  store i64 %addtmp30, ptr %"%ata_errors", align 8
  %loadtmp = load i64, ptr %"%drq_rc", align 8
  ret i64 %loadtmp

while_body_193:                                   ; preds = %while_cond_192
  %"%base.load32" = load i64, ptr %"%base", align 8
  %"%ATA_REG_DATA.load" = load i64, ptr @ATA_REG_DATA, align 8
  %addtmp33 = add i64 %"%base.load32", %"%ATA_REG_DATA.load"
  call void @sad_ll_port_inw(i64 %addtmp33)
  store i64 0, ptr %"%word", align 8
  %"%i.load34" = load i64, ptr %"%i", align 8
  %shltmp = shl i64 %"%i.load34", 1
  %"%buf.load" = load i64, ptr %buf3, align 8
  %addtmp35 = add i64 %"%buf.load", %shltmp
  %"%word.load" = load i64, ptr %"%word", align 8
  call void @sad_ll_mem_write16(i64 %addtmp35, i64 %"%word.load")
  %"%i.load36" = load i64, ptr %"%i", align 8
  %addtmp37 = add i64 %"%i.load36", 1
  store i64 %addtmp37, ptr %"%i", align 8
  br label %while_cond_192

while_cond_192:                                   ; preds = %while_body_193, %merge_191
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp31 = icmp slt i64 %"%i.load", 256
  br i1 %cmplttmp31, label %while_body_193, label %while_exit_194

while_exit_194:                                   ; preds = %while_cond_192
  %"%ata_reads.load" = load i64, ptr @ata_reads, align 8
  %addtmp38 = add i64 %"%ata_reads.load", 1
  store i64 %addtmp38, ptr %"%ata_reads", align 8
  ret i64 0
}

define i64 @ata_write_sector(i64 %drv, i64 %lba, i64 %buf) {
entry:
  %"%ata_writes" = alloca i64, align 8
  %"%word" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %"%ata_errors" = alloca i64, align 8
  %"%drq_rc" = alloca i64, align 8
  %"%head" = alloca i64, align 8
  %"%sel" = alloca i64, align 8
  %"%base" = alloca i64, align 8
  %buf3 = alloca i64, align 8
  store i64 %buf, ptr %buf3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_get_base(i64 %"%drv.load")
  store i64 %calltmp, ptr %"%base", align 8
  %"%drv.load4" = load i64, ptr %drv1, align 8
  %calltmp5 = call i64 @ata_get_select(i64 %"%drv.load4")
  store i64 %calltmp5, ptr %"%sel", align 8
  %"%drv.load6" = load i64, ptr %drv1, align 8
  %calltmp7 = call i64 @ata_wait_bsy(i64 %"%drv.load6")
  %"%sel.load" = load i64, ptr %"%sel", align 8
  %addtmp = add i64 %"%sel.load", 64
  %"%lba.load" = load i64, ptr %lba2, align 8
  %shrtmp = lshr i64 %"%lba.load", 24
  %andtmp = and i64 %shrtmp, 15
  %addtmp8 = add i64 %addtmp, %andtmp
  store i64 %addtmp8, ptr %"%head", align 8
  %"%base.load" = load i64, ptr %"%base", align 8
  %"%ATA_REG_DRIVE.load" = load i64, ptr @ATA_REG_DRIVE, align 8
  %addtmp9 = add i64 %"%base.load", %"%ATA_REG_DRIVE.load"
  %"%head.load" = load i64, ptr %"%head", align 8
  call void @sad_ll_port_outb(i64 %addtmp9, i64 %"%head.load")
  %"%base.load10" = load i64, ptr %"%base", align 8
  %"%ATA_REG_COUNT.load" = load i64, ptr @ATA_REG_COUNT, align 8
  %addtmp11 = add i64 %"%base.load10", %"%ATA_REG_COUNT.load"
  call void @sad_ll_port_outb(i64 %addtmp11, i64 1)
  %"%base.load12" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBALO.load" = load i64, ptr @ATA_REG_LBALO, align 8
  %addtmp13 = add i64 %"%base.load12", %"%ATA_REG_LBALO.load"
  %"%lba.load14" = load i64, ptr %lba2, align 8
  %andtmp15 = and i64 %"%lba.load14", 255
  call void @sad_ll_port_outb(i64 %addtmp13, i64 %andtmp15)
  %"%base.load16" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAMID.load" = load i64, ptr @ATA_REG_LBAMID, align 8
  %addtmp17 = add i64 %"%base.load16", %"%ATA_REG_LBAMID.load"
  %"%lba.load18" = load i64, ptr %lba2, align 8
  %shrtmp19 = lshr i64 %"%lba.load18", 8
  %andtmp20 = and i64 %shrtmp19, 255
  call void @sad_ll_port_outb(i64 %addtmp17, i64 %andtmp20)
  %"%base.load21" = load i64, ptr %"%base", align 8
  %"%ATA_REG_LBAHI.load" = load i64, ptr @ATA_REG_LBAHI, align 8
  %addtmp22 = add i64 %"%base.load21", %"%ATA_REG_LBAHI.load"
  %"%lba.load23" = load i64, ptr %lba2, align 8
  %shrtmp24 = lshr i64 %"%lba.load23", 16
  %andtmp25 = and i64 %shrtmp24, 255
  call void @sad_ll_port_outb(i64 %addtmp22, i64 %andtmp25)
  %"%base.load26" = load i64, ptr %"%base", align 8
  %"%ATA_REG_STATUS.load" = load i64, ptr @ATA_REG_STATUS, align 8
  %addtmp27 = add i64 %"%base.load26", %"%ATA_REG_STATUS.load"
  %"%ATA_CMD_WRITE_PIO.load" = load i64, ptr @ATA_CMD_WRITE_PIO, align 8
  call void @sad_ll_port_outb(i64 %addtmp27, i64 %"%ATA_CMD_WRITE_PIO.load")
  %"%drv.load28" = load i64, ptr %drv1, align 8
  %calltmp29 = call i64 @ata_wait_drq(i64 %"%drv.load28")
  store i64 %calltmp29, ptr %"%drq_rc", align 8
  %"%drq_rc.load" = load i64, ptr %"%drq_rc", align 8
  %cmplttmp = icmp slt i64 %"%drq_rc.load", 0
  br i1 %cmplttmp, label %then_195, label %merge_196

merge_196:                                        ; preds = %entry
  store i64 0, ptr %"%i", align 8
  br label %while_cond_197

then_195:                                         ; preds = %entry
  %"%ata_errors.load" = load i64, ptr @ata_errors, align 8
  %addtmp30 = add i64 %"%ata_errors.load", 1
  store i64 %addtmp30, ptr %"%ata_errors", align 8
  %loadtmp = load i64, ptr %"%drq_rc", align 8
  ret i64 %loadtmp

while_body_198:                                   ; preds = %while_cond_197
  %"%i.load32" = load i64, ptr %"%i", align 8
  %shltmp = shl i64 %"%i.load32", 1
  %"%buf.load" = load i64, ptr %buf3, align 8
  %addtmp33 = add i64 %"%buf.load", %shltmp
  call void @sad_ll_mem_read16(i64 %addtmp33)
  store i64 0, ptr %"%word", align 8
  %"%base.load34" = load i64, ptr %"%base", align 8
  %"%ATA_REG_DATA.load" = load i64, ptr @ATA_REG_DATA, align 8
  %addtmp35 = add i64 %"%base.load34", %"%ATA_REG_DATA.load"
  %"%word.load" = load i64, ptr %"%word", align 8
  call void @sad_ll_port_outw(i64 %addtmp35, i64 %"%word.load")
  %"%i.load36" = load i64, ptr %"%i", align 8
  %addtmp37 = add i64 %"%i.load36", 1
  store i64 %addtmp37, ptr %"%i", align 8
  br label %while_cond_197

while_cond_197:                                   ; preds = %while_body_198, %merge_196
  %"%i.load" = load i64, ptr %"%i", align 8
  %cmplttmp31 = icmp slt i64 %"%i.load", 256
  br i1 %cmplttmp31, label %while_body_198, label %while_exit_199

while_exit_199:                                   ; preds = %while_cond_197
  %"%base.load38" = load i64, ptr %"%base", align 8
  %"%ATA_REG_STATUS.load39" = load i64, ptr @ATA_REG_STATUS, align 8
  %addtmp40 = add i64 %"%base.load38", %"%ATA_REG_STATUS.load39"
  %"%ATA_CMD_CACHE_FLUSH.load" = load i64, ptr @ATA_CMD_CACHE_FLUSH, align 8
  call void @sad_ll_port_outb(i64 %addtmp40, i64 %"%ATA_CMD_CACHE_FLUSH.load")
  %"%drv.load41" = load i64, ptr %drv1, align 8
  %calltmp42 = call i64 @ata_wait_bsy(i64 %"%drv.load41")
  %"%ata_writes.load" = load i64, ptr @ata_writes, align 8
  %addtmp43 = add i64 %"%ata_writes.load", 1
  store i64 %addtmp43, ptr %"%ata_writes", align 8
  ret i64 0
}

define i64 @ata_read_sectors(i64 %drv, i64 %lba, i64 %count, i64 %buf) {
entry:
  %"%rc" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %buf4 = alloca i64, align 8
  store i64 %buf, ptr %buf4, align 8
  %count3 = alloca i64, align 8
  store i64 %count, ptr %count3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_200

merge_204:                                        ; preds = %while_body_201
  %"%i.load9" = load i64, ptr %"%i", align 8
  %addtmp10 = add i64 %"%i.load9", 1
  store i64 %addtmp10, ptr %"%i", align 8
  br label %while_cond_200

then_203:                                         ; preds = %while_body_201
  %loadtmp = load i64, ptr %"%rc", align 8
  ret i64 %loadtmp

while_body_201:                                   ; preds = %while_cond_200
  %"%lba.load" = load i64, ptr %lba2, align 8
  %"%i.load5" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%lba.load", %"%i.load5"
  %"%i.load6" = load i64, ptr %"%i", align 8
  %shltmp = shl i64 %"%i.load6", 9
  %"%buf.load" = load i64, ptr %buf4, align 8
  %addtmp7 = add i64 %"%buf.load", %shltmp
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_read_sector(i64 %"%drv.load", i64 %addtmp, i64 %addtmp7)
  store i64 %calltmp, ptr %"%rc", align 8
  %"%rc.load" = load i64, ptr %"%rc", align 8
  %cmplttmp8 = icmp slt i64 %"%rc.load", 0
  br i1 %cmplttmp8, label %then_203, label %merge_204

while_cond_200:                                   ; preds = %merge_204, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%count.load" = load i64, ptr %count3, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%count.load"
  br i1 %cmplttmp, label %while_body_201, label %while_exit_202

while_exit_202:                                   ; preds = %while_cond_200
  ret i64 0
}

define i64 @ata_write_sectors(i64 %drv, i64 %lba, i64 %count, i64 %buf) {
entry:
  %"%rc" = alloca i64, align 8
  %"%i" = alloca i64, align 8
  %buf4 = alloca i64, align 8
  store i64 %buf, ptr %buf4, align 8
  %count3 = alloca i64, align 8
  store i64 %count, ptr %count3, align 8
  %lba2 = alloca i64, align 8
  store i64 %lba, ptr %lba2, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  store i64 0, ptr %"%i", align 8
  br label %while_cond_205

merge_209:                                        ; preds = %while_body_206
  %"%i.load9" = load i64, ptr %"%i", align 8
  %addtmp10 = add i64 %"%i.load9", 1
  store i64 %addtmp10, ptr %"%i", align 8
  br label %while_cond_205

then_208:                                         ; preds = %while_body_206
  %loadtmp = load i64, ptr %"%rc", align 8
  ret i64 %loadtmp

while_body_206:                                   ; preds = %while_cond_205
  %"%lba.load" = load i64, ptr %lba2, align 8
  %"%i.load5" = load i64, ptr %"%i", align 8
  %addtmp = add i64 %"%lba.load", %"%i.load5"
  %"%i.load6" = load i64, ptr %"%i", align 8
  %shltmp = shl i64 %"%i.load6", 9
  %"%buf.load" = load i64, ptr %buf4, align 8
  %addtmp7 = add i64 %"%buf.load", %shltmp
  %"%drv.load" = load i64, ptr %drv1, align 8
  %calltmp = call i64 @ata_write_sector(i64 %"%drv.load", i64 %addtmp, i64 %addtmp7)
  store i64 %calltmp, ptr %"%rc", align 8
  %"%rc.load" = load i64, ptr %"%rc", align 8
  %cmplttmp8 = icmp slt i64 %"%rc.load", 0
  br i1 %cmplttmp8, label %then_208, label %merge_209

while_cond_205:                                   ; preds = %merge_209, %entry
  %"%i.load" = load i64, ptr %"%i", align 8
  %"%count.load" = load i64, ptr %count3, align 8
  %cmplttmp = icmp slt i64 %"%i.load", %"%count.load"
  br i1 %cmplttmp, label %while_body_206, label %while_exit_207

while_exit_207:                                   ; preds = %while_cond_205
  ret i64 0
}

define i64 @ata_read_mbr(i64 %drv) {
entry:
  %"%ata_part_size_0_3" = alloca i64, align 8
  %"%ata_part_start_0_3" = alloca i64, align 8
  %"%ata_part_type_0_3" = alloca i64, align 8
  %"%ata_part_size_0_2" = alloca i64, align 8
  %"%ata_part_start_0_2" = alloca i64, align 8
  %"%ata_part_type_0_2" = alloca i64, align 8
  %"%ata_part_size_0_1" = alloca i64, align 8
  %"%ata_part_start_0_1" = alloca i64, align 8
  %"%ata_part_type_0_1" = alloca i64, align 8
  %"%ata_part_size_0_0" = alloca i64, align 8
  %"%ata_part_start_0_0" = alloca i64, align 8
  %"%ata_part_type_0_0" = alloca i64, align 8
  %"%psize" = alloca i64, align 8
  %"%start" = alloca i64, align 8
  %"%ptype" = alloca i64, align 8
  %"%entry" = alloca i64, align 8
  %"%p" = alloca i64, align 8
  %"%sig" = alloca i64, align 8
  %"%rc" = alloca i64, align 8
  %"%buf" = alloca i64, align 8
  %drv1 = alloca i64, align 8
  store i64 %drv, ptr %drv1, align 8
  call void @sad_ll_kmalloc(i64 512)
  store i64 0, ptr %"%buf", align 8
  %"%buf.load" = load i64, ptr %"%buf", align 8
  call void @sad_ll_memset(i64 %"%buf.load", i64 0, i64 512)
  %"%drv.load" = load i64, ptr %drv1, align 8
  %"%buf.load2" = load i64, ptr %"%buf", align 8
  %calltmp = call i64 @ata_read_sector(i64 %"%drv.load", i64 0, i64 %"%buf.load2")
  store i64 %calltmp, ptr %"%rc", align 8
  %"%rc.load" = load i64, ptr %"%rc", align 8
  %cmplttmp = icmp slt i64 %"%rc.load", 0
  br i1 %cmplttmp, label %then_210, label %merge_211

merge_211:                                        ; preds = %entry
  %"%buf.load5" = load i64, ptr %"%buf", align 8
  %addtmp = add i64 %"%buf.load5", 510
  call void @sad_ll_mem_read16(i64 %addtmp)
  store i64 0, ptr %"%sig", align 8
  %"%sig.load" = load i64, ptr %"%sig", align 8
  %cmpnetmp = icmp ne i64 %"%sig.load", 43605
  br i1 %cmpnetmp, label %then_212, label %merge_213

merge_213:                                        ; preds = %merge_211
  %calltmp9 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.9 to i64))
  %calltmp10 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%p", align 8
  br label %while_cond_214

merge_218:                                        ; preds = %then_217, %while_body_215
  %"%p.load22" = load i64, ptr %"%p", align 8
  %cmpeqtmp23 = icmp eq i64 %"%p.load22", 1
  br i1 %cmpeqtmp23, label %then_219, label %merge_220

merge_220:                                        ; preds = %then_219, %merge_218
  %"%p.load24" = load i64, ptr %"%p", align 8
  %cmpeqtmp25 = icmp eq i64 %"%p.load24", 2
  br i1 %cmpeqtmp25, label %then_221, label %merge_222

merge_222:                                        ; preds = %then_221, %merge_220
  %"%p.load26" = load i64, ptr %"%p", align 8
  %cmpeqtmp27 = icmp eq i64 %"%p.load26", 3
  br i1 %cmpeqtmp27, label %then_223, label %merge_224

merge_224:                                        ; preds = %then_223, %merge_222
  %"%ptype.load" = load i64, ptr %"%ptype", align 8
  %cmpgttmp = icmp sgt i64 %"%ptype.load", 0
  br i1 %cmpgttmp, label %then_225, label %merge_226

merge_226:                                        ; preds = %then_225, %merge_224
  %"%p.load37" = load i64, ptr %"%p", align 8
  %addtmp38 = add i64 %"%p.load37", 1
  store i64 %addtmp38, ptr %"%p", align 8
  br label %while_cond_214

then_210:                                         ; preds = %entry
  %calltmp3 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.7 to i64))
  %calltmp4 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -1

then_212:                                         ; preds = %merge_211
  %calltmp6 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.8 to i64))
  %"%984.load" = load i64, ptr %"%sig", align 8
  %calltmp7 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%984.load")
  %calltmp8 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 -2

then_217:                                         ; preds = %while_body_215
  %"%1022.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1022.load", ptr %"%ata_part_type_0_0", align 8
  %"%1024.load" = load i64, ptr %"%start", align 8
  store i64 %"%1024.load", ptr %"%ata_part_start_0_0", align 8
  %"%1026.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1026.load", ptr %"%ata_part_size_0_0", align 8
  br label %merge_218

then_219:                                         ; preds = %merge_218
  %"%1030.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1030.load", ptr %"%ata_part_type_0_1", align 8
  %"%1032.load" = load i64, ptr %"%start", align 8
  store i64 %"%1032.load", ptr %"%ata_part_start_0_1", align 8
  %"%1034.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1034.load", ptr %"%ata_part_size_0_1", align 8
  br label %merge_220

then_221:                                         ; preds = %merge_220
  %"%1038.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1038.load", ptr %"%ata_part_type_0_2", align 8
  %"%1040.load" = load i64, ptr %"%start", align 8
  store i64 %"%1040.load", ptr %"%ata_part_start_0_2", align 8
  %"%1042.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1042.load", ptr %"%ata_part_size_0_2", align 8
  br label %merge_222

then_223:                                         ; preds = %merge_222
  %"%1046.load" = load i64, ptr %"%ptype", align 8
  store i64 %"%1046.load", ptr %"%ata_part_type_0_3", align 8
  %"%1048.load" = load i64, ptr %"%start", align 8
  store i64 %"%1048.load", ptr %"%ata_part_start_0_3", align 8
  %"%1050.load" = load i64, ptr %"%psize", align 8
  store i64 %"%1050.load", ptr %"%ata_part_size_0_3", align 8
  br label %merge_224

then_225:                                         ; preds = %merge_224
  %calltmp28 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.10 to i64))
  %"%1056.load" = load i64, ptr %"%p", align 8
  %calltmp29 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1056.load")
  %calltmp30 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.11 to i64))
  %"%1061.load" = load i64, ptr %"%ptype", align 8
  %calltmp31 = call i64 @ata_serial_hex8(i64 %"%1061.load")
  %calltmp32 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.12 to i64))
  %"%1066.load" = load i64, ptr %"%start", align 8
  %calltmp33 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1066.load")
  %calltmp34 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.13 to i64))
  %"%1071.load" = load i64, ptr %"%psize", align 8
  %calltmp35 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1071.load")
  %calltmp36 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_226

while_body_215:                                   ; preds = %while_cond_214
  %"%buf.load12" = load i64, ptr %"%buf", align 8
  %addtmp13 = add i64 %"%buf.load12", 446
  %"%p.load14" = load i64, ptr %"%p", align 8
  %shltmp = shl i64 %"%p.load14", 4
  %addtmp15 = add i64 %addtmp13, %shltmp
  store i64 %addtmp15, ptr %"%entry", align 8
  %"%entry.load" = load i64, ptr %"%entry", align 8
  %addtmp16 = add i64 %"%entry.load", 4
  call void @sad_ll_mem_read8(i64 %addtmp16)
  store i64 0, ptr %"%ptype", align 8
  %"%entry.load17" = load i64, ptr %"%entry", align 8
  %addtmp18 = add i64 %"%entry.load17", 8
  call void @sad_ll_mem_read32(i64 %addtmp18)
  store i64 0, ptr %"%start", align 8
  %"%entry.load19" = load i64, ptr %"%entry", align 8
  %addtmp20 = add i64 %"%entry.load19", 12
  call void @sad_ll_mem_read32(i64 %addtmp20)
  store i64 0, ptr %"%psize", align 8
  %"%p.load21" = load i64, ptr %"%p", align 8
  %cmpeqtmp = icmp eq i64 %"%p.load21", 0
  br i1 %cmpeqtmp, label %then_217, label %merge_218

while_cond_214:                                   ; preds = %merge_226, %merge_213
  %"%p.load" = load i64, ptr %"%p", align 8
  %cmplttmp11 = icmp slt i64 %"%p.load", 4
  br i1 %cmplttmp11, label %while_body_215, label %while_exit_216

while_exit_216:                                   ; preds = %while_cond_214
  ret i64 0
}

define i64 @"ata_\D8\AA\D9\82\D8\B1\D9\8A\D8\B1"() {
entry:
  %"%d" = alloca i64, align 8
  %calltmp = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.14 to i64))
  %calltmp1 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp2 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.15 to i64))
  %"%1083.load" = load i64, ptr @ata_drives_found, align 8
  %calltmp3 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1083.load")
  %calltmp4 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  store i64 0, ptr %"%d", align 8
  br label %while_cond_227

merge_231:                                        ; preds = %then_230, %while_body_228
  %"%d.load28" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load28", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_227

then_230:                                         ; preds = %while_body_228
  %calltmp16 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.20 to i64))
  %"%1097.load" = load i64, ptr %"%d", align 8
  %calltmp17 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1097.load")
  %calltmp18 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.21 to i64))
  %"%d.load19" = load i64, ptr %"%d", align 8
  %calltmp20 = call i64 @ata_get_size(i64 %"%d.load19")
  %calltmp21 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp20)
  %calltmp22 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.4 to i64))
  %"%d.load23" = load i64, ptr %"%d", align 8
  %calltmp24 = call i64 @ata_get_sectors(i64 %"%d.load23")
  %calltmp25 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %calltmp24)
  %calltmp26 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.5 to i64))
  %calltmp27 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  br label %merge_231

while_body_228:                                   ; preds = %while_cond_227
  %"%d.load5" = load i64, ptr %"%d", align 8
  %calltmp6 = call i64 @ata_get_present(i64 %"%d.load5")
  %cmpeqtmp = icmp eq i64 %calltmp6, 1
  br i1 %cmpeqtmp, label %then_230, label %merge_231

while_cond_227:                                   ; preds = %merge_231, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %cmplttmp = icmp slt i64 %"%d.load", 4
  br i1 %cmplttmp, label %while_body_228, label %while_exit_229

while_exit_229:                                   ; preds = %while_cond_227
  %calltmp7 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.16 to i64))
  %"%1119.load" = load i64, ptr @ata_reads, align 8
  %calltmp8 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1119.load")
  %calltmp9 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.17 to i64))
  %"%1124.load" = load i64, ptr @ata_writes, align 8
  %calltmp10 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1124.load")
  %calltmp11 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.18 to i64))
  %"%1129.load" = load i64, ptr @ata_errors, align 8
  %calltmp12 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1129.load")
  %calltmp13 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  %calltmp14 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.19 to i64))
  %calltmp15 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  ret i64 0
}

define i64 @"ata_\D9\87\D9\8A\D8\A6"() {
entry:
  %"%d" = alloca i64, align 8
  %"%ata_identify_buf" = alloca i64, align 8
  %calltmp = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.24 to i64))
  %calltmp1 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  call void @sad_ll_kmalloc(i64 512)
  store i64 0, ptr %"%ata_identify_buf", align 8
  %"%ata_identify_buf.load" = load i64, ptr %"%ata_identify_buf", align 8
  call void @sad_ll_memset(i64 %"%ata_identify_buf.load", i64 0, i64 512)
  %calltmp2 = call i64 @ata_soft_reset(i64 0)
  %calltmp3 = call i64 @ata_soft_reset(i64 2)
  store i64 0, ptr %"%d", align 8
  br label %while_cond_232

merge_236:                                        ; preds = %while_exit_239, %while_exit_234
  %calltmp7 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.25 to i64))
  %"%1172.load" = load i64, ptr @ata_drives_found, align 8
  %calltmp8 = call i64 @"ata_serial_\D8\B1\D9\82\D9\85"(i64 %"%1172.load")
  %calltmp9 = call i64 @"ata_serial_\D8\A7\D8\B7\D8\A8\D8\B9"(i64 ptrtoint (ptr @str.const.26 to i64))
  %calltmp10 = call i64 @"ata_serial_\D8\B3\D8\B7\D8\B1"()
  %loadtmp = load i64, ptr @ata_drives_found, align 8
  ret i64 %loadtmp

merge_241:                                        ; preds = %then_240, %while_body_238
  %"%d.load17" = load i64, ptr %"%d", align 8
  %addtmp18 = add i64 %"%d.load17", 1
  store i64 %addtmp18, ptr %"%d", align 8
  br label %while_cond_237

then_235:                                         ; preds = %while_exit_234
  store i64 0, ptr %"%d", align 8
  br label %while_cond_237

then_240:                                         ; preds = %while_body_238
  %"%d.load15" = load i64, ptr %"%d", align 8
  %calltmp16 = call i64 @ata_read_mbr(i64 %"%d.load15")
  br label %merge_241

while_body_233:                                   ; preds = %while_cond_232
  %"%d.load4" = load i64, ptr %"%d", align 8
  %calltmp5 = call i64 @ata_identify(i64 %"%d.load4")
  %"%d.load6" = load i64, ptr %"%d", align 8
  %addtmp = add i64 %"%d.load6", 1
  store i64 %addtmp, ptr %"%d", align 8
  br label %while_cond_232

while_body_238:                                   ; preds = %while_cond_237
  %"%d.load13" = load i64, ptr %"%d", align 8
  %calltmp14 = call i64 @ata_get_present(i64 %"%d.load13")
  %cmpeqtmp = icmp eq i64 %calltmp14, 1
  br i1 %cmpeqtmp, label %then_240, label %merge_241

while_cond_232:                                   ; preds = %while_body_233, %entry
  %"%d.load" = load i64, ptr %"%d", align 8
  %cmplttmp = icmp slt i64 %"%d.load", 4
  br i1 %cmplttmp, label %while_body_233, label %while_exit_234

while_cond_237:                                   ; preds = %merge_241, %then_235
  %"%d.load11" = load i64, ptr %"%d", align 8
  %cmplttmp12 = icmp slt i64 %"%d.load11", 4
  br i1 %cmplttmp12, label %while_body_238, label %while_exit_239

while_exit_234:                                   ; preds = %while_cond_232
  %"%ata_drives_found.load" = load i64, ptr @ata_drives_found, align 8
  %cmpgttmp = icmp sgt i64 %"%ata_drives_found.load", 0
  br i1 %cmpgttmp, label %then_235, label %merge_236

while_exit_239:                                   ; preds = %while_cond_237
  br label %merge_236
}

define i64 @"ata_\D8\AF\D9\88\D8\B1\D8\A9"() {
entry:
  ret i64 0
}

define void @__sad_main() {
entry:
  %"%ata_part_size_0_3" = alloca i64, align 8
  %"%ata_part_size_0_2" = alloca i64, align 8
  %"%ata_part_size_0_1" = alloca i64, align 8
  %"%ata_part_size_0_0" = alloca i64, align 8
  %"%ata_part_start_0_3" = alloca i64, align 8
  %"%ata_part_start_0_2" = alloca i64, align 8
  %"%ata_part_start_0_1" = alloca i64, align 8
  %"%ata_part_start_0_0" = alloca i64, align 8
  %"%ata_part_type_0_3" = alloca i64, align 8
  %"%ata_part_type_0_2" = alloca i64, align 8
  %"%ata_part_type_0_1" = alloca i64, align 8
  %"%ata_part_type_0_0" = alloca i64, align 8
  %"%ata_errors" = alloca i64, align 8
  %"%ata_writes" = alloca i64, align 8
  %"%ata_reads" = alloca i64, align 8
  %"%ata_drives_found" = alloca i64, align 8
  %"%ata_identify_buf" = alloca i64, align 8
  %"%ata_model_3_9" = alloca i64, align 8
  %"%ata_model_3_8" = alloca i64, align 8
  %"%ata_model_3_7" = alloca i64, align 8
  %"%ata_model_3_6" = alloca i64, align 8
  %"%ata_model_3_5" = alloca i64, align 8
  %"%ata_model_3_4" = alloca i64, align 8
  %"%ata_model_3_3" = alloca i64, align 8
  %"%ata_model_3_2" = alloca i64, align 8
  %"%ata_model_3_1" = alloca i64, align 8
  %"%ata_model_3_0" = alloca i64, align 8
  %"%ata_model_2_9" = alloca i64, align 8
  %"%ata_model_2_8" = alloca i64, align 8
  %"%ata_model_2_7" = alloca i64, align 8
  %"%ata_model_2_6" = alloca i64, align 8
  %"%ata_model_2_5" = alloca i64, align 8
  %"%ata_model_2_4" = alloca i64, align 8
  %"%ata_model_2_3" = alloca i64, align 8
  %"%ata_model_2_2" = alloca i64, align 8
  %"%ata_model_2_1" = alloca i64, align 8
  %"%ata_model_2_0" = alloca i64, align 8
  %"%ata_model_1_9" = alloca i64, align 8
  %"%ata_model_1_8" = alloca i64, align 8
  %"%ata_model_1_7" = alloca i64, align 8
  %"%ata_model_1_6" = alloca i64, align 8
  %"%ata_model_1_5" = alloca i64, align 8
  %"%ata_model_1_4" = alloca i64, align 8
  %"%ata_model_1_3" = alloca i64, align 8
  %"%ata_model_1_2" = alloca i64, align 8
  %"%ata_model_1_1" = alloca i64, align 8
  %"%ata_model_1_0" = alloca i64, align 8
  %"%ata_model_0_9" = alloca i64, align 8
  %"%ata_model_0_8" = alloca i64, align 8
  %"%ata_model_0_7" = alloca i64, align 8
  %"%ata_model_0_6" = alloca i64, align 8
  %"%ata_model_0_5" = alloca i64, align 8
  %"%ata_model_0_4" = alloca i64, align 8
  %"%ata_model_0_3" = alloca i64, align 8
  %"%ata_model_0_2" = alloca i64, align 8
  %"%ata_model_0_1" = alloca i64, align 8
  %"%ata_model_0_0" = alloca i64, align 8
  %"%ata_lba48_3" = alloca i64, align 8
  %"%ata_lba48_2" = alloca i64, align 8
  %"%ata_lba48_1" = alloca i64, align 8
  %"%ata_lba48_0" = alloca i64, align 8
  %"%ata_size_mb_3" = alloca i64, align 8
  %"%ata_size_mb_2" = alloca i64, align 8
  %"%ata_size_mb_1" = alloca i64, align 8
  %"%ata_size_mb_0" = alloca i64, align 8
  %"%ata_sectors_3" = alloca i64, align 8
  %"%ata_sectors_2" = alloca i64, align 8
  %"%ata_sectors_1" = alloca i64, align 8
  %"%ata_sectors_0" = alloca i64, align 8
  %"%ata_present_3" = alloca i64, align 8
  %"%ata_present_2" = alloca i64, align 8
  %"%ata_present_1" = alloca i64, align 8
  %"%ata_present_0" = alloca i64, align 8
  %"%ata_select_3" = alloca i64, align 8
  %"%ata_select_2" = alloca i64, align 8
  %"%ata_select_1" = alloca i64, align 8
  %"%ata_select_0" = alloca i64, align 8
  %"%ata_type_3" = alloca i64, align 8
  %"%ata_type_2" = alloca i64, align 8
  %"%ata_type_1" = alloca i64, align 8
  %"%ata_type_0" = alloca i64, align 8
  %"%ata_ctrl_3" = alloca i64, align 8
  %"%ata_ctrl_2" = alloca i64, align 8
  %"%ata_ctrl_1" = alloca i64, align 8
  %"%ata_ctrl_0" = alloca i64, align 8
  %"%ata_base_3" = alloca i64, align 8
  %"%ata_base_2" = alloca i64, align 8
  %"%ata_base_1" = alloca i64, align 8
  %"%ata_base_0" = alloca i64, align 8
  %"%ATA_MAX_DRIVES" = alloca i64, align 8
  %"%ATA_TYPE_ATAPI" = alloca i64, align 8
  %"%ATA_TYPE_ATA" = alloca i64, align 8
  %"%ATA_TYPE_NONE" = alloca i64, align 8
  %"%ATA_SLAVE" = alloca i64, align 8
  %"%ATA_MASTER" = alloca i64, align 8
  %"%ATA_CMD_IDENTIFY_PACKET" = alloca i64, align 8
  %"%ATA_CMD_PACKET" = alloca i64, align 8
  %"%ATA_CMD_CACHE_FLUSH_EXT" = alloca i64, align 8
  %"%ATA_CMD_CACHE_FLUSH" = alloca i64, align 8
  %"%ATA_CMD_WRITE_PIO_EXT" = alloca i64, align 8
  %"%ATA_CMD_READ_PIO_EXT" = alloca i64, align 8
  %"%ATA_CMD_WRITE_PIO" = alloca i64, align 8
  %"%ATA_CMD_READ_PIO" = alloca i64, align 8
  %"%ATA_CMD_IDENTIFY" = alloca i64, align 8
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
  %"%ATA_REG_ALTSTATUS" = alloca i64, align 8
  %"%ATA_REG_STATUS" = alloca i64, align 8
  %"%ATA_REG_DRIVE" = alloca i64, align 8
  %"%ATA_REG_LBAHI" = alloca i64, align 8
  %"%ATA_REG_LBAMID" = alloca i64, align 8
  %"%ATA_REG_LBALO" = alloca i64, align 8
  %"%ATA_REG_COUNT" = alloca i64, align 8
  %"%ATA_REG_ERROR" = alloca i64, align 8
  %"%ATA_REG_DATA" = alloca i64, align 8
  %"%ATA_SEC_CTRL" = alloca i64, align 8
  %"%ATA_SEC_BASE" = alloca i64, align 8
  %"%ATA_PRI_CTRL" = alloca i64, align 8
  %"%ATA_PRI_BASE" = alloca i64, align 8
  %"%COM1" = alloca i64, align 8
  store i64 1016, ptr %"%COM1", align 8
  store i64 496, ptr %"%ATA_PRI_BASE", align 8
  store i64 1014, ptr %"%ATA_PRI_CTRL", align 8
  store i64 368, ptr %"%ATA_SEC_BASE", align 8
  store i64 886, ptr %"%ATA_SEC_CTRL", align 8
  store i64 0, ptr %"%ATA_REG_DATA", align 8
  store i64 1, ptr %"%ATA_REG_ERROR", align 8
  store i64 2, ptr %"%ATA_REG_COUNT", align 8
  store i64 3, ptr %"%ATA_REG_LBALO", align 8
  store i64 4, ptr %"%ATA_REG_LBAMID", align 8
  store i64 5, ptr %"%ATA_REG_LBAHI", align 8
  store i64 6, ptr %"%ATA_REG_DRIVE", align 8
  store i64 7, ptr %"%ATA_REG_STATUS", align 8
  store i64 0, ptr %"%ATA_REG_ALTSTATUS", align 8
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
  store i64 236, ptr %"%ATA_CMD_IDENTIFY", align 8
  store i64 32, ptr %"%ATA_CMD_READ_PIO", align 8
  store i64 48, ptr %"%ATA_CMD_WRITE_PIO", align 8
  store i64 36, ptr %"%ATA_CMD_READ_PIO_EXT", align 8
  store i64 52, ptr %"%ATA_CMD_WRITE_PIO_EXT", align 8
  store i64 231, ptr %"%ATA_CMD_CACHE_FLUSH", align 8
  store i64 234, ptr %"%ATA_CMD_CACHE_FLUSH_EXT", align 8
  store i64 160, ptr %"%ATA_CMD_PACKET", align 8
  store i64 161, ptr %"%ATA_CMD_IDENTIFY_PACKET", align 8
  store i64 160, ptr %"%ATA_MASTER", align 8
  store i64 176, ptr %"%ATA_SLAVE", align 8
  store i64 0, ptr %"%ATA_TYPE_NONE", align 8
  store i64 1, ptr %"%ATA_TYPE_ATA", align 8
  store i64 2, ptr %"%ATA_TYPE_ATAPI", align 8
  store i64 4, ptr %"%ATA_MAX_DRIVES", align 8
  store i64 496, ptr %"%ata_base_0", align 8
  store i64 496, ptr %"%ata_base_1", align 8
  store i64 368, ptr %"%ata_base_2", align 8
  store i64 368, ptr %"%ata_base_3", align 8
  store i64 1014, ptr %"%ata_ctrl_0", align 8
  store i64 1014, ptr %"%ata_ctrl_1", align 8
  store i64 886, ptr %"%ata_ctrl_2", align 8
  store i64 886, ptr %"%ata_ctrl_3", align 8
  store i64 0, ptr %"%ata_type_0", align 8
  store i64 0, ptr %"%ata_type_1", align 8
  store i64 0, ptr %"%ata_type_2", align 8
  store i64 0, ptr %"%ata_type_3", align 8
  store i64 160, ptr %"%ata_select_0", align 8
  store i64 176, ptr %"%ata_select_1", align 8
  store i64 160, ptr %"%ata_select_2", align 8
  store i64 176, ptr %"%ata_select_3", align 8
  store i64 0, ptr %"%ata_present_0", align 8
  store i64 0, ptr %"%ata_present_1", align 8
  store i64 0, ptr %"%ata_present_2", align 8
  store i64 0, ptr %"%ata_present_3", align 8
  store i64 0, ptr %"%ata_sectors_0", align 8
  store i64 0, ptr %"%ata_sectors_1", align 8
  store i64 0, ptr %"%ata_sectors_2", align 8
  store i64 0, ptr %"%ata_sectors_3", align 8
  store i64 0, ptr %"%ata_size_mb_0", align 8
  store i64 0, ptr %"%ata_size_mb_1", align 8
  store i64 0, ptr %"%ata_size_mb_2", align 8
  store i64 0, ptr %"%ata_size_mb_3", align 8
  store i64 0, ptr %"%ata_lba48_0", align 8
  store i64 0, ptr %"%ata_lba48_1", align 8
  store i64 0, ptr %"%ata_lba48_2", align 8
  store i64 0, ptr %"%ata_lba48_3", align 8
  store i64 0, ptr %"%ata_model_0_0", align 8
  store i64 0, ptr %"%ata_model_0_1", align 8
  store i64 0, ptr %"%ata_model_0_2", align 8
  store i64 0, ptr %"%ata_model_0_3", align 8
  store i64 0, ptr %"%ata_model_0_4", align 8
  store i64 0, ptr %"%ata_model_0_5", align 8
  store i64 0, ptr %"%ata_model_0_6", align 8
  store i64 0, ptr %"%ata_model_0_7", align 8
  store i64 0, ptr %"%ata_model_0_8", align 8
  store i64 0, ptr %"%ata_model_0_9", align 8
  store i64 0, ptr %"%ata_model_1_0", align 8
  store i64 0, ptr %"%ata_model_1_1", align 8
  store i64 0, ptr %"%ata_model_1_2", align 8
  store i64 0, ptr %"%ata_model_1_3", align 8
  store i64 0, ptr %"%ata_model_1_4", align 8
  store i64 0, ptr %"%ata_model_1_5", align 8
  store i64 0, ptr %"%ata_model_1_6", align 8
  store i64 0, ptr %"%ata_model_1_7", align 8
  store i64 0, ptr %"%ata_model_1_8", align 8
  store i64 0, ptr %"%ata_model_1_9", align 8
  store i64 0, ptr %"%ata_model_2_0", align 8
  store i64 0, ptr %"%ata_model_2_1", align 8
  store i64 0, ptr %"%ata_model_2_2", align 8
  store i64 0, ptr %"%ata_model_2_3", align 8
  store i64 0, ptr %"%ata_model_2_4", align 8
  store i64 0, ptr %"%ata_model_2_5", align 8
  store i64 0, ptr %"%ata_model_2_6", align 8
  store i64 0, ptr %"%ata_model_2_7", align 8
  store i64 0, ptr %"%ata_model_2_8", align 8
  store i64 0, ptr %"%ata_model_2_9", align 8
  store i64 0, ptr %"%ata_model_3_0", align 8
  store i64 0, ptr %"%ata_model_3_1", align 8
  store i64 0, ptr %"%ata_model_3_2", align 8
  store i64 0, ptr %"%ata_model_3_3", align 8
  store i64 0, ptr %"%ata_model_3_4", align 8
  store i64 0, ptr %"%ata_model_3_5", align 8
  store i64 0, ptr %"%ata_model_3_6", align 8
  store i64 0, ptr %"%ata_model_3_7", align 8
  store i64 0, ptr %"%ata_model_3_8", align 8
  store i64 0, ptr %"%ata_model_3_9", align 8
  store i64 0, ptr %"%ata_identify_buf", align 8
  store i64 0, ptr %"%ata_drives_found", align 8
  store i64 0, ptr %"%ata_reads", align 8
  store i64 0, ptr %"%ata_writes", align 8
  store i64 0, ptr %"%ata_errors", align 8
  store i64 0, ptr %"%ata_part_type_0_0", align 8
  store i64 0, ptr %"%ata_part_type_0_1", align 8
  store i64 0, ptr %"%ata_part_type_0_2", align 8
  store i64 0, ptr %"%ata_part_type_0_3", align 8
  store i64 0, ptr %"%ata_part_start_0_0", align 8
  store i64 0, ptr %"%ata_part_start_0_1", align 8
  store i64 0, ptr %"%ata_part_start_0_2", align 8
  store i64 0, ptr %"%ata_part_start_0_3", align 8
  store i64 0, ptr %"%ata_part_size_0_0", align 8
  store i64 0, ptr %"%ata_part_size_0_1", align 8
  store i64 0, ptr %"%ata_part_size_0_2", align 8
  store i64 0, ptr %"%ata_part_size_0_3", align 8
  ret void
}

define i32 @main() {
entry:
  call void @__sad_main()
  ret i32 0
}
